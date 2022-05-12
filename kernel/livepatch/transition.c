// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * transition.c - Kernel Live Patching transition functions
 *
 * Copyright (C) 2015-2016 Josh Poimboeuf <jpoimboe@redhat.com>
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/cpu.h>
#include <linux/stacktrace.h>
#include <linux/tracehook.h>
#include "core.h"
#include "patch.h"
#include "transition.h"
#include "../sched/sched.h"

#define MAX_STACK_ENTRIES  100
#define STACK_ERR_BUF_SIZE 128

#define SIGNALS_TIMEOUT 15

struct klp_patch *klp_transition_patch;

static int klp_target_state = KLP_UNDEFINED;

static unsigned int klp_signals_cnt;

/*
 * This work can be performed periodically to finish patching or unpatching any
 * "straggler" tasks which failed to transition in the first attempt.
 */
static void klp_transition_work_fn(struct work_struct *work)
{
	mutex_lock(&klp_mutex);

	if (klp_transition_patch)
		klp_try_complete_transition();

	mutex_unlock(&klp_mutex);
}
static DECLARE_DELAYED_WORK(klp_transition_work, klp_transition_work_fn);

/*
 * This function is just a stub to implement a hard force
 * of synchronize_rcu(). This requires synchronizing
 * tasks even in userspace and idle.
 */
static void klp_sync(struct work_struct *work)
{
}

/*
 * We allow to patch also functions where RCU is not watching,
 * e.g. before user_exit(). We can not rely on the RCU infrastructure
 * to do the synchronization. Instead hard force the sched synchronization.
 *
 * This approach allows to use RCU functions for manipulating func_stack
 * safely.
 */
static void klp_synchronize_transition(void)
{
	schedule_on_each_cpu(klp_sync);
}

/*
 * The transition to the target patch state is complete.  Clean up the data
 * structures.
 */
static void klp_complete_transition(void)
{
	struct klp_object *obj;
	struct klp_func *func;
	struct task_struct *g, *task;
	unsigned int cpu;

	pr_debug("'%s': completing %s transition\n",
		 klp_transition_patch->mod->name,
		 klp_target_state == KLP_PATCHED ? "patching" : "unpatching");

	if (klp_transition_patch->replace && klp_target_state == KLP_PATCHED) {
		klp_unpatch_replaced_patches(klp_transition_patch);
		klp_discard_nops(klp_transition_patch);
	}

	if (klp_target_state == KLP_UNPATCHED) {
		/*
		 * All tasks have transitioned to KLP_UNPATCHED so we can now
		 * remove the new functions from the func_stack.
		 */
		klp_unpatch_objects(klp_transition_patch);

		/*
		 * Make sure klp_ftrace_handler() can no longer see functions
		 * from this patch on the ops->func_stack.  Otherwise, after
		 * func->transition gets cleared, the handler may choose a
		 * removed function.
		 */
		klp_synchronize_transition();
	}

	klp_for_each_object(klp_transition_patch, obj)
		klp_for_each_func(obj, func)
			func->transition = false;

	/* Prevent klp_ftrace_handler() from seeing KLP_UNDEFINED state */
	if (klp_target_state == KLP_PATCHED)
		klp_synchronize_transition();

	read_lock(&tasklist_lock);
	for_each_process_thread(g, task) {
		WARN_ON_ONCE(test_tsk_thread_flag(task, TIF_PATCH_PENDING));
		task->patch_state = KLP_UNDEFINED;
	}
	read_unlock(&tasklist_lock);

	for_each_possible_cpu(cpu) {
		task = idle_task(cpu);
		WARN_ON_ONCE(test_tsk_thread_flag(task, TIF_PATCH_PENDING));
		task->patch_state = KLP_UNDEFINED;
	}

	klp_for_each_object(klp_transition_patch, obj) {
		if (!klp_is_object_loaded(obj))
			continue;
		if (klp_target_state == KLP_PATCHED)
			klp_post_patch_callback(obj);
		else if (klp_target_state == KLP_UNPATCHED)
			klp_post_unpatch_callback(obj);
	}

	pr_notice("'%s': %s complete\n", klp_transition_patch->mod->name,
		  klp_target_state == KLP_PATCHED ? "patching" : "unpatching");

	klp_target_state = KLP_UNDEFINED;
	klp_transition_patch = NULL;
}

/*
 * This is called in the error path, to cancel a transition before it has
 * started, i.e. klp_init_transition() has been called but
 * klp_start_transition() hasn't.  If the transition *has* been started,
 * klp_reverse_transition() should be used instead.
 */
void klp_cancel_transition(void)
{
	if (WARN_ON_ONCE(klp_target_state != KLP_PATCHED))
		return;

	pr_debug("'%s': canceling patching transition, going to unpatch\n",
		 klp_transition_patch->mod->name);

	klp_target_state = KLP_UNPATCHED;
	klp_complete_transition();
}

/*
 * Switch the patched state of the task to the set of functions in the target
 * patch state.
 *
 * NOTE: If task is not 'current', the caller must ensure the task is inactive.
 * Otherwise klp_ftrace_handler() might read the wrong 'patch_state' value.
 */
void klp_update_patch_state(struct task_struct *task)
{
	/*
	 * A variant of synchronize_rcu() is used to allow patching functions
	 * where RCU is not watching, see klp_synchronize_transition().
	 */
	preempt_disable_notrace();

	/*
	 * This test_and_clear_tsk_thread_flag() call also serves as a read
	 * barrier (smp_rmb) for two cases:
	 *
	 * 1) Enforce the order of the TIF_PATCH_PENDING read and the
	 *    klp_target_state read.  The corresponding write barrier is in
	 *    klp_init_transition().
	 *
	 * 2) Enforce the order of the TIF_PATCH_PENDING read and a future read
	 *    of func->transition, if klp_ftrace_handler() is called later on
	 *    the same CPU.  See __klp_disable_patch().
	 */
	if (test_and_clear_tsk_thread_flag(task, TIF_PATCH_PENDING))
		task->patch_state = READ_ONCE(klp_target_state);

	preempt_enable_notrace();
}

/*
 * Determine whether the given stack trace includes any references to a
 * to-be-patched or to-be-unpatched function.
 */
static int klp_check_stack_func(struct klp_func *func, unsigned long *entries,
				unsigned int nr_entries)
{
	unsigned long func_addr, func_size, address;
	struct klp_ops *ops;
	int i;

	for (i = 0; i < nr_entries; i++) {
		address = entries[i];

		if (klp_target_state == KLP_UNPATCHED) {
			 /*
			  * Check for the to-be-unpatched function
			  * (the func itself).
			  */
			func_addr = (unsigned long)func->new_func;
			func_size = func->new_size;
		} else {
			/*
			 * Check for the to-be-patched function
			 * (the previous func).
			 */
			ops = klp_find_ops(func->old_func);

			if (list_is_singular(&ops->func_stack)) {
				/* original function */
				func_addr = (unsigned long)func->old_func;
				func_size = func->old_size;
			} else {
				/* previously patched function */
				struct klp_func *prev;

				prev = list_next_entry(func, stack_node);
				func_addr = (unsigned long)prev->new_func;
				func_size = prev->new_size;
			}
		}

		if (address >= func_addr && address < func_addr + func_size)
			return -EAGAIN;
	}

	return 0;
}

/*
 * Determine whether it's safe to transition the task to the target patch state
 * by looking for any to-be-patched or to-be-unpatched functions on its stack.
 */
static int klp_check_stack(struct task_struct *task, char *err_buf)
{
	static unsigned long entries[MAX_STACK_ENTRIES];
	struct klp_object *obj;
	struct klp_func *func;
	int ret, nr_entries;

	ret = stack_trace_save_tsk_reliable(task, entries, ARRAY_SIZE(entries));
	if (ret < 0) {
		snprintf(err_buf, STACK_ERR_BUF_SIZE,
			 "%s: %s:%d has an unreliable stack\n",
			 __func__, task->comm, task->pid);
		return ret;
	}
	nr_entries = ret;

	klp_for_each_object(klp_transition_patch, obj) {
		if (!obj->patched)
			continue;
		klp_for_each_func(obj, func) {
			ret = klp_check_stack_func(func, entries, nr_entries);
			if (ret) {
				snprintf(err_buf, STACK_ERR_BUF_SIZE,
					 "%s: %s:%d is sleeping on function %s\n",
					 __func__, task->comm, task->pid,
					 func->old_name);
				return ret;
			}
		}
	}

	return 0;
}

/*
 * Try to safely switch a task to the target patch state.  If it's currently
 * running, or it's sleeping on a to-be-patched or to-be-unpatched function, or
 * if the stack is unreliable, return false.
 */
static bool klp_try_switch_task(struct task_struct *task)
{
	static char err_buf[STACK_ERR_BUF_SIZE];
	struct rq *rq;
	struct rq_flags flags;
	int ret;
	bool success = false;

	err_buf[0] = '\0';

	/* check if this task has already switched over */
	if (task->patch_state == klp_target_state)
		return true;

	/*
	 * For arches which don't have reliable stack traces, we have to rely
	 * on other methods (e.g., switching tasks at kernel exit).
	 */
	if (!klp_have_reliable_stack())
		return false;

	/*
	 * Now try to check the stack for any to-be-patched or to-be-unpatched
	 * functions.  If all goes well, switch the task to the target patch
	 * state.
	 */
	rq = task_rq_lock(task, &flags);

	if (task_running(rq, task) && task != current) {
		snprintf(err_buf, STACK_ERR_BUF_SIZE,
			 "%s: %s:%d is running\n", __func__, task->comm,
			 task->pid);
		goto done;
	}

	ret = klp_check_stack(task, err_buf);
	if (ret)
		goto done;

	success = true;

	clear_tsk_thread_flag(task, TIF_PATCH_PENDING);
	task->patch_state = klp_target_state;

done:
	task_rq_unlock(rq, task, &flags);

	/*
	 * Due to console deadlock issues, pr_debug() can't be used while
	 * holding the task rq lock.  Instead we have to use a temporary buffer
	 * and print the debug message after releasing the lock.
	 */
	if (err_buf[0] != '\0')
		pr_debug("%s", err_buf);

	return success;
}

/*
 * Sends a fake signal to all non-kthread tasks with TIF_PATCH_PENDING set.
 * Kthreads with TIF_PATCH_PENDING set are woken up.
 */
static void klp_send_signals(void)
{
	struct task_struct *g, *task;

	if (klp_signals_cnt == SIGNALS_TIMEOUT)
		pr_notice("signaling remaining tasks\n");

	read_lock(&tasklist_lock);
	for_each_process_thread(g, task) {
		if (!klp_patch_pending(task))
			continue;

		/*
		 * There is a small race here. We could see TIF_PATCH_PENDING
		 * set and decide to wake up a kthread or send a fake signal.
		 * Meanwhile the task could migrate itself and the action
		 * would be meaningless. It is not serious though.
		 */
		if (task->flags & PF_KTHREAD) {
			/*
			 * Wake up a kthread which sleeps interruptedly and
			 * still has not been migrated.
			 */
			wake_up_state(task, TASK_INTERRUPTIBLE);
		} else {
			/*
			 * Send fake signal to all non-kthread tasks which are
			 * still not migrated.
			 */
			set_notify_signal(task);
		}
	}
	read_unlock(&tasklist_lock);
}

/*
 * Try to switch all remaining tasks to the target patch state by walking the
 * stacks of sleeping tasks and looking for any to-be-patched or
 * to-be-unpatched functions.  If such functions are found, the task can't be
 * switched yet.
 *
 * If any tasks are still stuck in the initial patch state, schedule a retry.
 */
void klp_try_complete_transition(void)
{
	unsigned int cpu;
	struct task_struct *g, *task;
	struct klp_patch *patch;
	bool complete = true;

	WARN_ON_ONCE(klp_target_state == KLP_UNDEFINED);

	/*
	 * Try to switch the tasks to the target patch state by walking their
	 * stacks and looking for any to-be-patched or to-be-unpatched
	 * functions.  If such functions are found on a stack, or if the stack
	 * is deemed unreliable, the task can't be switched yet.
	 *
	 * Usually this will transition most (or all) of the tasks on a system
	 * unless the patch includes changes to a very common function.
	 */
	read_lock(&tasklist_lock);
	for_each_process_thread(g, task)
		if (!klp_try_switch_task(task))
			complete = false;
	read_unlock(&tasklist_lock);

	/*
	 * Ditto for the idle "swapper" tasks.
	 */
	get_online_cpus();
	for_each_possible_cpu(cpu) {
		task = idle_task(cpu);
		if (cpu_online(cpu)) {
			if (!klp_try_switch_task(task))
				complete = false;
		} else if (task->patch_state != klp_target_state) {
			/* offline idle tasks can be switched immediately */
			clear_tsk_thread_flag(task, TIF_PATCH_PENDING);
			task->patch_state = klp_target_state;
		}
	}
	put_online_cpus();

	if (!complete) {
		if (klp_signals_cnt && !(klp_signals_cnt % SIGNALS_TIMEOUT))
			klp_send_signals();
		klp_signals_cnt++;

		/*
		 * Some tasks weren't able to be switched over.  Try again
		 * later and/or wait for other methods like kernel exit
		 * switching.
		 */
		schedule_delayed_work(&klp_transition_work,
				      round_jiffies_relative(HZ));
		return;
	}

	/* we're done, now cleanup the data structures */
	patch = klp_transition_patch;
	klp_complete_transition();

	/*
	 * It would make more sense to free the unused patches in
	 * klp_complete_transition() but it is called also
	 * from klp_cancel_transition().
	 */
	if (!patch->enabled)
		klp_free_patch_async(patch);
	else if (patch->replace)
		klp_free_replaced_patches_async(patch);
}

/*
 * Start the transition to the specified target patch state so tasks can begin
 * switching to it.
 */
void klp_start_transition(void)
{
	struct task_struct *g, *task;
	unsigned int cpu;

	WARN_ON_ONCE(klp_target_state == KLP_UNDEFINED);

	pr_notice("'%s': starting %s transition\n",
		  klp_transition_patch->mod->name,
		  klp_target_state == KLP_PATCHED ? "patching" : "unpatching");

	/*
	 * Mark all normal tasks as needing a patch state update.  They'll
	 * switch either in klp_try_complete_transition() or as they exit the
	 * kernel.
	 */
	read_lock(&tasklist_lock);
	for_each_process_thread(g, task)
		if (task->patch_state != klp_target_state)
			set_tsk_thread_flag(task, TIF_PATCH_PENDING);
	read_unlock(&tasklist_lock);

	/*
	 * Mark all idle tasks as needing a patch state update.  They'll switch
	 * either in klp_try_complete_transition() or at the idle loop switch
	 * point.
	 */
	for_each_possible_cpu(cpu) {
		task = idle_task(cpu);
		if (task->patch_state != klp_target_state)
			set_tsk_thread_flag(task, TIF_PATCH_PENDING);
	}

	klp_signals_cnt = 0;
}

/*
 * Initialize the global target patch state and all tasks to the initial patch
 * state, and initialize all function transition states to true in preparation
 * for patching or unpatching.
 */
void klp_init_transition(struct klp_patch *patch, int state)
{
	struct task_struct *g, *task;
	unsigned int cpu;
	struct klp_object *obj;
	struct klp_func *func;
	int initial_state = !state;

	WARN_ON_ONCE(klp_target_state != KLP_UNDEFINED);

	klp_transition_patch = patch;

	/*
	 * Set the global target patch state which tasks will switch to.  This
	 * has no effect until the TIF_PATCH_PENDING flags get set later.
	 */
	klp_target_state = state;

	pr_debug("'%s': initializing %s transition\n", patch->mod->name,
		 klp_target_state == KLP_PATCHED ? "patching" : "unpatching");

	/*
	 * Initialize all tasks to the initial patch state to prepare them for
	 * switching to the target state.
	 */
	read_lock(&tasklist_lock);
	for_each_process_thread(g, task) {
		WARN_ON_ONCE(task->patch_state != KLP_UNDEFINED);
		task->patch_state = initial_state;
	}
	read_unlock(&tasklist_lock);

	/*
	 * Ditto for the idle "swapper" tasks.
	 */
	for_each_possible_cpu(cpu) {
		task = idle_task(cpu);
		WARN_ON_ONCE(task->patch_state != KLP_UNDEFINED);
		task->patch_state = initial_state;
	}

	/*
	 * Enforce the order of the task->patch_state initializations and the
	 * func->transition updates to ensure that klp_ftrace_handler() doesn't
	 * see a func in transition with a task->patch_state of KLP_UNDEFINED.
	 *
	 * Also enforce the order of the klp_target_state write and future
	 * TIF_PATCH_PENDING writes to ensure klp_update_patch_state() doesn't
	 * set a task->patch_state to KLP_UNDEFINED.
	 */
	smp_wmb();

	/*
	 * Set the func transition states so klp_ftrace_handler() will know to
	 * switch to the transition logic.
	 *
	 * When patching, the funcs aren't yet in the func_stack and will be
	 * made visible to the ftrace handler shortly by the calls to
	 * klp_patch_object().
	 *
	 * When unpatching, the funcs are already in the func_stack and so are
	 * already visible to the ftrace handler.
	 */
	klp_for_each_object(patch, obj)
		klp_for_each_func(obj, func)
			func->transition = true;
}

/*
 * This function can be called in the middle of an existing transition to
 * reverse the direction of the target patch state.  This can be done to
 * effectively cancel an existing enable or disable operation if there are any
 * tasks which are stuck in the initial patch state.
 */
void klp_reverse_transition(void)
{
	unsigned int cpu;
	struct task_struct *g, *task;

	pr_debug("'%s': reversing transition from %s\n",
		 klp_transition_patch->mod->name,
		 klp_target_state == KLP_PATCHED ? "patching to unpatching" :
						   "unpatching to patching");

	klp_transition_patch->enabled = !klp_transition_patch->enabled;

	klp_target_state = !klp_target_state;

	/*
	 * Clear all TIF_PATCH_PENDING flags to prevent races caused by
	 * klp_update_patch_state() running in parallel with
	 * klp_start_transition().
	 */
	read_lock(&tasklist_lock);
	for_each_process_thread(g, task)
		clear_tsk_thread_flag(task, TIF_PATCH_PENDING);
	read_unlock(&tasklist_lock);

	for_each_possible_cpu(cpu)
		clear_tsk_thread_flag(idle_task(cpu), TIF_PATCH_PENDING);

	/* Let any remaining calls to klp_update_patch_state() complete */
	klp_synchronize_transition();

	klp_start_transition();
}

/* Called from copy_process() during fork */
void klp_copy_process(struct task_struct *child)
{
	child->patch_state = current->patch_state;

	/* TIF_PATCH_PENDING gets copied in setup_thread_stack() */
}

/*
 * Drop TIF_PATCH_PENDING of all tasks on admin's request. This forces an
 * existing transition to finish.
 *
 * NOTE: klp_update_patch_state(task) requires the task to be inactive or
 * 'current'. This is not the case here and the consistency model could be
 * broken. Administrator, who is the only one to execute the
 * klp_force_transitions(), has to be aware of this.
 */
void klp_force_transition(void)
{
	struct klp_patch *patch;
	struct task_struct *g, *task;
	unsigned int cpu;

	pr_warn("forcing remaining tasks to the patched state\n");

	read_lock(&tasklist_lock);
	for_each_process_thread(g, task)
		klp_update_patch_state(task);
	read_unlock(&tasklist_lock);

	for_each_possible_cpu(cpu)
		klp_update_patch_state(idle_task(cpu));

	klp_for_each_patch(patch)
		patch->forced = true;
}
