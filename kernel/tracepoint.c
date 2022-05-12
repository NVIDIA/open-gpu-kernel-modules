// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (C) 2008-2014 Mathieu Desnoyers
 */
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/types.h>
#include <linux/jhash.h>
#include <linux/list.h>
#include <linux/rcupdate.h>
#include <linux/tracepoint.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/sched/signal.h>
#include <linux/sched/task.h>
#include <linux/static_key.h>

extern tracepoint_ptr_t __start___tracepoints_ptrs[];
extern tracepoint_ptr_t __stop___tracepoints_ptrs[];

DEFINE_SRCU(tracepoint_srcu);
EXPORT_SYMBOL_GPL(tracepoint_srcu);

/* Set to 1 to enable tracepoint debug output */
static const int tracepoint_debug;

#ifdef CONFIG_MODULES
/*
 * Tracepoint module list mutex protects the local module list.
 */
static DEFINE_MUTEX(tracepoint_module_list_mutex);

/* Local list of struct tp_module */
static LIST_HEAD(tracepoint_module_list);
#endif /* CONFIG_MODULES */

/*
 * tracepoints_mutex protects the builtin and module tracepoints.
 * tracepoints_mutex nests inside tracepoint_module_list_mutex.
 */
static DEFINE_MUTEX(tracepoints_mutex);

static struct rcu_head *early_probes;
static bool ok_to_free_tracepoints;

/*
 * Note about RCU :
 * It is used to delay the free of multiple probes array until a quiescent
 * state is reached.
 */
struct tp_probes {
	struct rcu_head rcu;
	struct tracepoint_func probes[];
};

/* Called in removal of a func but failed to allocate a new tp_funcs */
static void tp_stub_func(void)
{
	return;
}

static inline void *allocate_probes(int count)
{
	struct tp_probes *p  = kmalloc(struct_size(p, probes, count),
				       GFP_KERNEL);
	return p == NULL ? NULL : p->probes;
}

static void srcu_free_old_probes(struct rcu_head *head)
{
	kfree(container_of(head, struct tp_probes, rcu));
}

static void rcu_free_old_probes(struct rcu_head *head)
{
	call_srcu(&tracepoint_srcu, head, srcu_free_old_probes);
}

static __init int release_early_probes(void)
{
	struct rcu_head *tmp;

	ok_to_free_tracepoints = true;

	while (early_probes) {
		tmp = early_probes;
		early_probes = tmp->next;
		call_rcu(tmp, rcu_free_old_probes);
	}

	return 0;
}

/* SRCU is initialized at core_initcall */
postcore_initcall(release_early_probes);

static inline void release_probes(struct tracepoint_func *old)
{
	if (old) {
		struct tp_probes *tp_probes = container_of(old,
			struct tp_probes, probes[0]);

		/*
		 * We can't free probes if SRCU is not initialized yet.
		 * Postpone the freeing till after SRCU is initialized.
		 */
		if (unlikely(!ok_to_free_tracepoints)) {
			tp_probes->rcu.next = early_probes;
			early_probes = &tp_probes->rcu;
			return;
		}

		/*
		 * Tracepoint probes are protected by both sched RCU and SRCU,
		 * by calling the SRCU callback in the sched RCU callback we
		 * cover both cases. So let us chain the SRCU and sched RCU
		 * callbacks to wait for both grace periods.
		 */
		call_rcu(&tp_probes->rcu, rcu_free_old_probes);
	}
}

static void debug_print_probes(struct tracepoint_func *funcs)
{
	int i;

	if (!tracepoint_debug || !funcs)
		return;

	for (i = 0; funcs[i].func; i++)
		printk(KERN_DEBUG "Probe %d : %p\n", i, funcs[i].func);
}

static struct tracepoint_func *
func_add(struct tracepoint_func **funcs, struct tracepoint_func *tp_func,
	 int prio)
{
	struct tracepoint_func *old, *new;
	int iter_probes;	/* Iterate over old probe array. */
	int nr_probes = 0;	/* Counter for probes */
	int pos = -1;		/* Insertion position into new array */

	if (WARN_ON(!tp_func->func))
		return ERR_PTR(-EINVAL);

	debug_print_probes(*funcs);
	old = *funcs;
	if (old) {
		/* (N -> N+1), (N != 0, 1) probes */
		for (iter_probes = 0; old[iter_probes].func; iter_probes++) {
			if (old[iter_probes].func == tp_stub_func)
				continue;	/* Skip stub functions. */
			if (old[iter_probes].func == tp_func->func &&
			    old[iter_probes].data == tp_func->data)
				return ERR_PTR(-EEXIST);
			nr_probes++;
		}
	}
	/* + 2 : one for new probe, one for NULL func */
	new = allocate_probes(nr_probes + 2);
	if (new == NULL)
		return ERR_PTR(-ENOMEM);
	if (old) {
		nr_probes = 0;
		for (iter_probes = 0; old[iter_probes].func; iter_probes++) {
			if (old[iter_probes].func == tp_stub_func)
				continue;
			/* Insert before probes of lower priority */
			if (pos < 0 && old[iter_probes].prio < prio)
				pos = nr_probes++;
			new[nr_probes++] = old[iter_probes];
		}
		if (pos < 0)
			pos = nr_probes++;
		/* nr_probes now points to the end of the new array */
	} else {
		pos = 0;
		nr_probes = 1; /* must point at end of array */
	}
	new[pos] = *tp_func;
	new[nr_probes].func = NULL;
	*funcs = new;
	debug_print_probes(*funcs);
	return old;
}

static void *func_remove(struct tracepoint_func **funcs,
		struct tracepoint_func *tp_func)
{
	int nr_probes = 0, nr_del = 0, i;
	struct tracepoint_func *old, *new;

	old = *funcs;

	if (!old)
		return ERR_PTR(-ENOENT);

	debug_print_probes(*funcs);
	/* (N -> M), (N > 1, M >= 0) probes */
	if (tp_func->func) {
		for (nr_probes = 0; old[nr_probes].func; nr_probes++) {
			if ((old[nr_probes].func == tp_func->func &&
			     old[nr_probes].data == tp_func->data) ||
			    old[nr_probes].func == tp_stub_func)
				nr_del++;
		}
	}

	/*
	 * If probe is NULL, then nr_probes = nr_del = 0, and then the
	 * entire entry will be removed.
	 */
	if (nr_probes - nr_del == 0) {
		/* N -> 0, (N > 1) */
		*funcs = NULL;
		debug_print_probes(*funcs);
		return old;
	} else {
		int j = 0;
		/* N -> M, (N > 1, M > 0) */
		/* + 1 for NULL */
		new = allocate_probes(nr_probes - nr_del + 1);
		if (new) {
			for (i = 0; old[i].func; i++) {
				if ((old[i].func != tp_func->func ||
				     old[i].data != tp_func->data) &&
				    old[i].func != tp_stub_func)
					new[j++] = old[i];
			}
			new[nr_probes - nr_del].func = NULL;
			*funcs = new;
		} else {
			/*
			 * Failed to allocate, replace the old function
			 * with calls to tp_stub_func.
			 */
			for (i = 0; old[i].func; i++) {
				if (old[i].func == tp_func->func &&
				    old[i].data == tp_func->data)
					WRITE_ONCE(old[i].func, tp_stub_func);
			}
			*funcs = old;
		}
	}
	debug_print_probes(*funcs);
	return old;
}

static void tracepoint_update_call(struct tracepoint *tp, struct tracepoint_func *tp_funcs, bool sync)
{
	void *func = tp->iterator;

	/* Synthetic events do not have static call sites */
	if (!tp->static_call_key)
		return;

	if (!tp_funcs[1].func) {
		func = tp_funcs[0].func;
		/*
		 * If going from the iterator back to a single caller,
		 * we need to synchronize with __DO_TRACE to make sure
		 * that the data passed to the callback is the one that
		 * belongs to that callback.
		 */
		if (sync)
			tracepoint_synchronize_unregister();
	}

	__static_call_update(tp->static_call_key, tp->static_call_tramp, func);
}

/*
 * Add the probe function to a tracepoint.
 */
static int tracepoint_add_func(struct tracepoint *tp,
			       struct tracepoint_func *func, int prio)
{
	struct tracepoint_func *old, *tp_funcs;
	int ret;

	if (tp->regfunc && !static_key_enabled(&tp->key)) {
		ret = tp->regfunc();
		if (ret < 0)
			return ret;
	}

	tp_funcs = rcu_dereference_protected(tp->funcs,
			lockdep_is_held(&tracepoints_mutex));
	old = func_add(&tp_funcs, func, prio);
	if (IS_ERR(old)) {
		WARN_ON_ONCE(PTR_ERR(old) != -ENOMEM);
		return PTR_ERR(old);
	}

	/*
	 * rcu_assign_pointer has as smp_store_release() which makes sure
	 * that the new probe callbacks array is consistent before setting
	 * a pointer to it.  This array is referenced by __DO_TRACE from
	 * include/linux/tracepoint.h using rcu_dereference_sched().
	 */
	rcu_assign_pointer(tp->funcs, tp_funcs);
	tracepoint_update_call(tp, tp_funcs, false);
	static_key_enable(&tp->key);

	release_probes(old);
	return 0;
}

/*
 * Remove a probe function from a tracepoint.
 * Note: only waiting an RCU period after setting elem->call to the empty
 * function insures that the original callback is not used anymore. This insured
 * by preempt_disable around the call site.
 */
static int tracepoint_remove_func(struct tracepoint *tp,
		struct tracepoint_func *func)
{
	struct tracepoint_func *old, *tp_funcs;

	tp_funcs = rcu_dereference_protected(tp->funcs,
			lockdep_is_held(&tracepoints_mutex));
	old = func_remove(&tp_funcs, func);
	if (WARN_ON_ONCE(IS_ERR(old)))
		return PTR_ERR(old);

	if (tp_funcs == old)
		/* Failed allocating new tp_funcs, replaced func with stub */
		return 0;

	if (!tp_funcs) {
		/* Removed last function */
		if (tp->unregfunc && static_key_enabled(&tp->key))
			tp->unregfunc();

		static_key_disable(&tp->key);
		rcu_assign_pointer(tp->funcs, tp_funcs);
	} else {
		rcu_assign_pointer(tp->funcs, tp_funcs);
		tracepoint_update_call(tp, tp_funcs,
				       tp_funcs[0].func != old[0].func);
	}
	release_probes(old);
	return 0;
}

/**
 * tracepoint_probe_register_prio -  Connect a probe to a tracepoint with priority
 * @tp: tracepoint
 * @probe: probe handler
 * @data: tracepoint data
 * @prio: priority of this function over other registered functions
 *
 * Returns 0 if ok, error value on error.
 * Note: if @tp is within a module, the caller is responsible for
 * unregistering the probe before the module is gone. This can be
 * performed either with a tracepoint module going notifier, or from
 * within module exit functions.
 */
int tracepoint_probe_register_prio(struct tracepoint *tp, void *probe,
				   void *data, int prio)
{
	struct tracepoint_func tp_func;
	int ret;

	mutex_lock(&tracepoints_mutex);
	tp_func.func = probe;
	tp_func.data = data;
	tp_func.prio = prio;
	ret = tracepoint_add_func(tp, &tp_func, prio);
	mutex_unlock(&tracepoints_mutex);
	return ret;
}
EXPORT_SYMBOL_GPL(tracepoint_probe_register_prio);

/**
 * tracepoint_probe_register -  Connect a probe to a tracepoint
 * @tp: tracepoint
 * @probe: probe handler
 * @data: tracepoint data
 *
 * Returns 0 if ok, error value on error.
 * Note: if @tp is within a module, the caller is responsible for
 * unregistering the probe before the module is gone. This can be
 * performed either with a tracepoint module going notifier, or from
 * within module exit functions.
 */
int tracepoint_probe_register(struct tracepoint *tp, void *probe, void *data)
{
	return tracepoint_probe_register_prio(tp, probe, data, TRACEPOINT_DEFAULT_PRIO);
}
EXPORT_SYMBOL_GPL(tracepoint_probe_register);

/**
 * tracepoint_probe_unregister -  Disconnect a probe from a tracepoint
 * @tp: tracepoint
 * @probe: probe function pointer
 * @data: tracepoint data
 *
 * Returns 0 if ok, error value on error.
 */
int tracepoint_probe_unregister(struct tracepoint *tp, void *probe, void *data)
{
	struct tracepoint_func tp_func;
	int ret;

	mutex_lock(&tracepoints_mutex);
	tp_func.func = probe;
	tp_func.data = data;
	ret = tracepoint_remove_func(tp, &tp_func);
	mutex_unlock(&tracepoints_mutex);
	return ret;
}
EXPORT_SYMBOL_GPL(tracepoint_probe_unregister);

static void for_each_tracepoint_range(
		tracepoint_ptr_t *begin, tracepoint_ptr_t *end,
		void (*fct)(struct tracepoint *tp, void *priv),
		void *priv)
{
	tracepoint_ptr_t *iter;

	if (!begin)
		return;
	for (iter = begin; iter < end; iter++)
		fct(tracepoint_ptr_deref(iter), priv);
}

#ifdef CONFIG_MODULES
bool trace_module_has_bad_taint(struct module *mod)
{
	return mod->taints & ~((1 << TAINT_OOT_MODULE) | (1 << TAINT_CRAP) |
			       (1 << TAINT_UNSIGNED_MODULE));
}

static BLOCKING_NOTIFIER_HEAD(tracepoint_notify_list);

/**
 * register_tracepoint_notifier - register tracepoint coming/going notifier
 * @nb: notifier block
 *
 * Notifiers registered with this function are called on module
 * coming/going with the tracepoint_module_list_mutex held.
 * The notifier block callback should expect a "struct tp_module" data
 * pointer.
 */
int register_tracepoint_module_notifier(struct notifier_block *nb)
{
	struct tp_module *tp_mod;
	int ret;

	mutex_lock(&tracepoint_module_list_mutex);
	ret = blocking_notifier_chain_register(&tracepoint_notify_list, nb);
	if (ret)
		goto end;
	list_for_each_entry(tp_mod, &tracepoint_module_list, list)
		(void) nb->notifier_call(nb, MODULE_STATE_COMING, tp_mod);
end:
	mutex_unlock(&tracepoint_module_list_mutex);
	return ret;
}
EXPORT_SYMBOL_GPL(register_tracepoint_module_notifier);

/**
 * unregister_tracepoint_notifier - unregister tracepoint coming/going notifier
 * @nb: notifier block
 *
 * The notifier block callback should expect a "struct tp_module" data
 * pointer.
 */
int unregister_tracepoint_module_notifier(struct notifier_block *nb)
{
	struct tp_module *tp_mod;
	int ret;

	mutex_lock(&tracepoint_module_list_mutex);
	ret = blocking_notifier_chain_unregister(&tracepoint_notify_list, nb);
	if (ret)
		goto end;
	list_for_each_entry(tp_mod, &tracepoint_module_list, list)
		(void) nb->notifier_call(nb, MODULE_STATE_GOING, tp_mod);
end:
	mutex_unlock(&tracepoint_module_list_mutex);
	return ret;

}
EXPORT_SYMBOL_GPL(unregister_tracepoint_module_notifier);

/*
 * Ensure the tracer unregistered the module's probes before the module
 * teardown is performed. Prevents leaks of probe and data pointers.
 */
static void tp_module_going_check_quiescent(struct tracepoint *tp, void *priv)
{
	WARN_ON_ONCE(tp->funcs);
}

static int tracepoint_module_coming(struct module *mod)
{
	struct tp_module *tp_mod;
	int ret = 0;

	if (!mod->num_tracepoints)
		return 0;

	/*
	 * We skip modules that taint the kernel, especially those with different
	 * module headers (for forced load), to make sure we don't cause a crash.
	 * Staging, out-of-tree, and unsigned GPL modules are fine.
	 */
	if (trace_module_has_bad_taint(mod))
		return 0;
	mutex_lock(&tracepoint_module_list_mutex);
	tp_mod = kmalloc(sizeof(struct tp_module), GFP_KERNEL);
	if (!tp_mod) {
		ret = -ENOMEM;
		goto end;
	}
	tp_mod->mod = mod;
	list_add_tail(&tp_mod->list, &tracepoint_module_list);
	blocking_notifier_call_chain(&tracepoint_notify_list,
			MODULE_STATE_COMING, tp_mod);
end:
	mutex_unlock(&tracepoint_module_list_mutex);
	return ret;
}

static void tracepoint_module_going(struct module *mod)
{
	struct tp_module *tp_mod;

	if (!mod->num_tracepoints)
		return;

	mutex_lock(&tracepoint_module_list_mutex);
	list_for_each_entry(tp_mod, &tracepoint_module_list, list) {
		if (tp_mod->mod == mod) {
			blocking_notifier_call_chain(&tracepoint_notify_list,
					MODULE_STATE_GOING, tp_mod);
			list_del(&tp_mod->list);
			kfree(tp_mod);
			/*
			 * Called the going notifier before checking for
			 * quiescence.
			 */
			for_each_tracepoint_range(mod->tracepoints_ptrs,
				mod->tracepoints_ptrs + mod->num_tracepoints,
				tp_module_going_check_quiescent, NULL);
			break;
		}
	}
	/*
	 * In the case of modules that were tainted at "coming", we'll simply
	 * walk through the list without finding it. We cannot use the "tainted"
	 * flag on "going", in case a module taints the kernel only after being
	 * loaded.
	 */
	mutex_unlock(&tracepoint_module_list_mutex);
}

static int tracepoint_module_notify(struct notifier_block *self,
		unsigned long val, void *data)
{
	struct module *mod = data;
	int ret = 0;

	switch (val) {
	case MODULE_STATE_COMING:
		ret = tracepoint_module_coming(mod);
		break;
	case MODULE_STATE_LIVE:
		break;
	case MODULE_STATE_GOING:
		tracepoint_module_going(mod);
		break;
	case MODULE_STATE_UNFORMED:
		break;
	}
	return notifier_from_errno(ret);
}

static struct notifier_block tracepoint_module_nb = {
	.notifier_call = tracepoint_module_notify,
	.priority = 0,
};

static __init int init_tracepoints(void)
{
	int ret;

	ret = register_module_notifier(&tracepoint_module_nb);
	if (ret)
		pr_warn("Failed to register tracepoint module enter notifier\n");

	return ret;
}
__initcall(init_tracepoints);
#endif /* CONFIG_MODULES */

/**
 * for_each_kernel_tracepoint - iteration on all kernel tracepoints
 * @fct: callback
 * @priv: private data
 */
void for_each_kernel_tracepoint(void (*fct)(struct tracepoint *tp, void *priv),
		void *priv)
{
	for_each_tracepoint_range(__start___tracepoints_ptrs,
		__stop___tracepoints_ptrs, fct, priv);
}
EXPORT_SYMBOL_GPL(for_each_kernel_tracepoint);

#ifdef CONFIG_HAVE_SYSCALL_TRACEPOINTS

/* NB: reg/unreg are called while guarded with the tracepoints_mutex */
static int sys_tracepoint_refcount;

int syscall_regfunc(void)
{
	struct task_struct *p, *t;

	if (!sys_tracepoint_refcount) {
		read_lock(&tasklist_lock);
		for_each_process_thread(p, t) {
			set_task_syscall_work(t, SYSCALL_TRACEPOINT);
		}
		read_unlock(&tasklist_lock);
	}
	sys_tracepoint_refcount++;

	return 0;
}

void syscall_unregfunc(void)
{
	struct task_struct *p, *t;

	sys_tracepoint_refcount--;
	if (!sys_tracepoint_refcount) {
		read_lock(&tasklist_lock);
		for_each_process_thread(p, t) {
			clear_task_syscall_work(t, SYSCALL_TRACEPOINT);
		}
		read_unlock(&tasklist_lock);
	}
}
#endif
