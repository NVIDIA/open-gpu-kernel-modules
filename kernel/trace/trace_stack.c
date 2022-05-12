// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2008 Steven Rostedt <srostedt@redhat.com>
 *
 */
#include <linux/sched/task_stack.h>
#include <linux/stacktrace.h>
#include <linux/security.h>
#include <linux/kallsyms.h>
#include <linux/seq_file.h>
#include <linux/spinlock.h>
#include <linux/uaccess.h>
#include <linux/ftrace.h>
#include <linux/module.h>
#include <linux/sysctl.h>
#include <linux/init.h>

#include <asm/setup.h>

#include "trace.h"

#define STACK_TRACE_ENTRIES 500

static unsigned long stack_dump_trace[STACK_TRACE_ENTRIES];
static unsigned stack_trace_index[STACK_TRACE_ENTRIES];

static unsigned int stack_trace_nr_entries;
static unsigned long stack_trace_max_size;
static arch_spinlock_t stack_trace_max_lock =
	(arch_spinlock_t)__ARCH_SPIN_LOCK_UNLOCKED;

DEFINE_PER_CPU(int, disable_stack_tracer);
static DEFINE_MUTEX(stack_sysctl_mutex);

int stack_tracer_enabled;

static void print_max_stack(void)
{
	long i;
	int size;

	pr_emerg("        Depth    Size   Location    (%d entries)\n"
			   "        -----    ----   --------\n",
			   stack_trace_nr_entries);

	for (i = 0; i < stack_trace_nr_entries; i++) {
		if (i + 1 == stack_trace_nr_entries)
			size = stack_trace_index[i];
		else
			size = stack_trace_index[i] - stack_trace_index[i+1];

		pr_emerg("%3ld) %8d   %5d   %pS\n", i, stack_trace_index[i],
				size, (void *)stack_dump_trace[i]);
	}
}

/*
 * The stack tracer looks for a maximum stack at each call from a function. It
 * registers a callback from ftrace, and in that callback it examines the stack
 * size. It determines the stack size from the variable passed in, which is the
 * address of a local variable in the stack_trace_call() callback function.
 * The stack size is calculated by the address of the local variable to the top
 * of the current stack. If that size is smaller than the currently saved max
 * stack size, nothing more is done.
 *
 * If the size of the stack is greater than the maximum recorded size, then the
 * following algorithm takes place.
 *
 * For architectures (like x86) that store the function's return address before
 * saving the function's local variables, the stack will look something like
 * this:
 *
 *   [ top of stack ]
 *    0: sys call entry frame
 *   10: return addr to entry code
 *   11: start of sys_foo frame
 *   20: return addr to sys_foo
 *   21: start of kernel_func_bar frame
 *   30: return addr to kernel_func_bar
 *   31: [ do trace stack here ]
 *
 * The save_stack_trace() is called returning all the functions it finds in the
 * current stack. Which would be (from the bottom of the stack to the top):
 *
 *   return addr to kernel_func_bar
 *   return addr to sys_foo
 *   return addr to entry code
 *
 * Now to figure out how much each of these functions' local variable size is,
 * a search of the stack is made to find these values. When a match is made, it
 * is added to the stack_dump_trace[] array. The offset into the stack is saved
 * in the stack_trace_index[] array. The above example would show:
 *
 *        stack_dump_trace[]        |   stack_trace_index[]
 *        ------------------        +   -------------------
 *  return addr to kernel_func_bar  |          30
 *  return addr to sys_foo          |          20
 *  return addr to entry            |          10
 *
 * The print_max_stack() function above, uses these values to print the size of
 * each function's portion of the stack.
 *
 *  for (i = 0; i < nr_entries; i++) {
 *     size = i == nr_entries - 1 ? stack_trace_index[i] :
 *                    stack_trace_index[i] - stack_trace_index[i+1]
 *     print "%d %d %d %s\n", i, stack_trace_index[i], size, stack_dump_trace[i]);
 *  }
 *
 * The above shows
 *
 *     depth size  location
 *     ----- ----  --------
 *  0    30   10   kernel_func_bar
 *  1    20   10   sys_foo
 *  2    10   10   entry code
 *
 * Now for architectures that might save the return address after the functions
 * local variables (saving the link register before calling nested functions),
 * this will cause the stack to look a little different:
 *
 * [ top of stack ]
 *  0: sys call entry frame
 * 10: start of sys_foo_frame
 * 19: return addr to entry code << lr saved before calling kernel_func_bar
 * 20: start of kernel_func_bar frame
 * 29: return addr to sys_foo_frame << lr saved before calling next function
 * 30: [ do trace stack here ]
 *
 * Although the functions returned by save_stack_trace() may be the same, the
 * placement in the stack will be different. Using the same algorithm as above
 * would yield:
 *
 *        stack_dump_trace[]        |   stack_trace_index[]
 *        ------------------        +   -------------------
 *  return addr to kernel_func_bar  |          30
 *  return addr to sys_foo          |          29
 *  return addr to entry            |          19
 *
 * Where the mapping is off by one:
 *
 *   kernel_func_bar stack frame size is 29 - 19 not 30 - 29!
 *
 * To fix this, if the architecture sets ARCH_RET_ADDR_AFTER_LOCAL_VARS the
 * values in stack_trace_index[] are shifted by one to and the number of
 * stack trace entries is decremented by one.
 *
 *        stack_dump_trace[]        |   stack_trace_index[]
 *        ------------------        +   -------------------
 *  return addr to kernel_func_bar  |          29
 *  return addr to sys_foo          |          19
 *
 * Although the entry function is not displayed, the first function (sys_foo)
 * will still include the stack size of it.
 */
static void check_stack(unsigned long ip, unsigned long *stack)
{
	unsigned long this_size, flags; unsigned long *p, *top, *start;
	static int tracer_frame;
	int frame_size = READ_ONCE(tracer_frame);
	int i, x;

	this_size = ((unsigned long)stack) & (THREAD_SIZE-1);
	this_size = THREAD_SIZE - this_size;
	/* Remove the frame of the tracer */
	this_size -= frame_size;

	if (this_size <= stack_trace_max_size)
		return;

	/* we do not handle interrupt stacks yet */
	if (!object_is_on_stack(stack))
		return;

	/* Can't do this from NMI context (can cause deadlocks) */
	if (in_nmi())
		return;

	local_irq_save(flags);
	arch_spin_lock(&stack_trace_max_lock);

	/* In case another CPU set the tracer_frame on us */
	if (unlikely(!frame_size))
		this_size -= tracer_frame;

	/* a race could have already updated it */
	if (this_size <= stack_trace_max_size)
		goto out;

	stack_trace_max_size = this_size;

	stack_trace_nr_entries = stack_trace_save(stack_dump_trace,
					       ARRAY_SIZE(stack_dump_trace) - 1,
					       0);

	/* Skip over the overhead of the stack tracer itself */
	for (i = 0; i < stack_trace_nr_entries; i++) {
		if (stack_dump_trace[i] == ip)
			break;
	}

	/*
	 * Some archs may not have the passed in ip in the dump.
	 * If that happens, we need to show everything.
	 */
	if (i == stack_trace_nr_entries)
		i = 0;

	/*
	 * Now find where in the stack these are.
	 */
	x = 0;
	start = stack;
	top = (unsigned long *)
		(((unsigned long)start & ~(THREAD_SIZE-1)) + THREAD_SIZE);

	/*
	 * Loop through all the entries. One of the entries may
	 * for some reason be missed on the stack, so we may
	 * have to account for them. If they are all there, this
	 * loop will only happen once. This code only takes place
	 * on a new max, so it is far from a fast path.
	 */
	while (i < stack_trace_nr_entries) {
		int found = 0;

		stack_trace_index[x] = this_size;
		p = start;

		for (; p < top && i < stack_trace_nr_entries; p++) {
			/*
			 * The READ_ONCE_NOCHECK is used to let KASAN know that
			 * this is not a stack-out-of-bounds error.
			 */
			if ((READ_ONCE_NOCHECK(*p)) == stack_dump_trace[i]) {
				stack_dump_trace[x] = stack_dump_trace[i++];
				this_size = stack_trace_index[x++] =
					(top - p) * sizeof(unsigned long);
				found = 1;
				/* Start the search from here */
				start = p + 1;
				/*
				 * We do not want to show the overhead
				 * of the stack tracer stack in the
				 * max stack. If we haven't figured
				 * out what that is, then figure it out
				 * now.
				 */
				if (unlikely(!tracer_frame)) {
					tracer_frame = (p - stack) *
						sizeof(unsigned long);
					stack_trace_max_size -= tracer_frame;
				}
			}
		}

		if (!found)
			i++;
	}

#ifdef ARCH_FTRACE_SHIFT_STACK_TRACER
	/*
	 * Some archs will store the link register before calling
	 * nested functions. This means the saved return address
	 * comes after the local storage, and we need to shift
	 * for that.
	 */
	if (x > 1) {
		memmove(&stack_trace_index[0], &stack_trace_index[1],
			sizeof(stack_trace_index[0]) * (x - 1));
		x--;
	}
#endif

	stack_trace_nr_entries = x;

	if (task_stack_end_corrupted(current)) {
		print_max_stack();
		BUG();
	}

 out:
	arch_spin_unlock(&stack_trace_max_lock);
	local_irq_restore(flags);
}

/* Some archs may not define MCOUNT_INSN_SIZE */
#ifndef MCOUNT_INSN_SIZE
# define MCOUNT_INSN_SIZE 0
#endif

static void
stack_trace_call(unsigned long ip, unsigned long parent_ip,
		 struct ftrace_ops *op, struct ftrace_regs *fregs)
{
	unsigned long stack;

	preempt_disable_notrace();

	/* no atomic needed, we only modify this variable by this cpu */
	__this_cpu_inc(disable_stack_tracer);
	if (__this_cpu_read(disable_stack_tracer) != 1)
		goto out;

	/* If rcu is not watching, then save stack trace can fail */
	if (!rcu_is_watching())
		goto out;

	ip += MCOUNT_INSN_SIZE;

	check_stack(ip, &stack);

 out:
	__this_cpu_dec(disable_stack_tracer);
	/* prevent recursion in schedule */
	preempt_enable_notrace();
}

static struct ftrace_ops trace_ops __read_mostly =
{
	.func = stack_trace_call,
};

static ssize_t
stack_max_size_read(struct file *filp, char __user *ubuf,
		    size_t count, loff_t *ppos)
{
	unsigned long *ptr = filp->private_data;
	char buf[64];
	int r;

	r = snprintf(buf, sizeof(buf), "%ld\n", *ptr);
	if (r > sizeof(buf))
		r = sizeof(buf);
	return simple_read_from_buffer(ubuf, count, ppos, buf, r);
}

static ssize_t
stack_max_size_write(struct file *filp, const char __user *ubuf,
		     size_t count, loff_t *ppos)
{
	long *ptr = filp->private_data;
	unsigned long val, flags;
	int ret;

	ret = kstrtoul_from_user(ubuf, count, 10, &val);
	if (ret)
		return ret;

	local_irq_save(flags);

	/*
	 * In case we trace inside arch_spin_lock() or after (NMI),
	 * we will cause circular lock, so we also need to increase
	 * the percpu disable_stack_tracer here.
	 */
	__this_cpu_inc(disable_stack_tracer);

	arch_spin_lock(&stack_trace_max_lock);
	*ptr = val;
	arch_spin_unlock(&stack_trace_max_lock);

	__this_cpu_dec(disable_stack_tracer);
	local_irq_restore(flags);

	return count;
}

static const struct file_operations stack_max_size_fops = {
	.open		= tracing_open_generic,
	.read		= stack_max_size_read,
	.write		= stack_max_size_write,
	.llseek		= default_llseek,
};

static void *
__next(struct seq_file *m, loff_t *pos)
{
	long n = *pos - 1;

	if (n >= stack_trace_nr_entries)
		return NULL;

	m->private = (void *)n;
	return &m->private;
}

static void *
t_next(struct seq_file *m, void *v, loff_t *pos)
{
	(*pos)++;
	return __next(m, pos);
}

static void *t_start(struct seq_file *m, loff_t *pos)
{
	local_irq_disable();

	__this_cpu_inc(disable_stack_tracer);

	arch_spin_lock(&stack_trace_max_lock);

	if (*pos == 0)
		return SEQ_START_TOKEN;

	return __next(m, pos);
}

static void t_stop(struct seq_file *m, void *p)
{
	arch_spin_unlock(&stack_trace_max_lock);

	__this_cpu_dec(disable_stack_tracer);

	local_irq_enable();
}

static void trace_lookup_stack(struct seq_file *m, long i)
{
	unsigned long addr = stack_dump_trace[i];

	seq_printf(m, "%pS\n", (void *)addr);
}

static void print_disabled(struct seq_file *m)
{
	seq_puts(m, "#\n"
		 "#  Stack tracer disabled\n"
		 "#\n"
		 "# To enable the stack tracer, either add 'stacktrace' to the\n"
		 "# kernel command line\n"
		 "# or 'echo 1 > /proc/sys/kernel/stack_tracer_enabled'\n"
		 "#\n");
}

static int t_show(struct seq_file *m, void *v)
{
	long i;
	int size;

	if (v == SEQ_START_TOKEN) {
		seq_printf(m, "        Depth    Size   Location"
			   "    (%d entries)\n"
			   "        -----    ----   --------\n",
			   stack_trace_nr_entries);

		if (!stack_tracer_enabled && !stack_trace_max_size)
			print_disabled(m);

		return 0;
	}

	i = *(long *)v;

	if (i >= stack_trace_nr_entries)
		return 0;

	if (i + 1 == stack_trace_nr_entries)
		size = stack_trace_index[i];
	else
		size = stack_trace_index[i] - stack_trace_index[i+1];

	seq_printf(m, "%3ld) %8d   %5d   ", i, stack_trace_index[i], size);

	trace_lookup_stack(m, i);

	return 0;
}

static const struct seq_operations stack_trace_seq_ops = {
	.start		= t_start,
	.next		= t_next,
	.stop		= t_stop,
	.show		= t_show,
};

static int stack_trace_open(struct inode *inode, struct file *file)
{
	int ret;

	ret = security_locked_down(LOCKDOWN_TRACEFS);
	if (ret)
		return ret;

	return seq_open(file, &stack_trace_seq_ops);
}

static const struct file_operations stack_trace_fops = {
	.open		= stack_trace_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= seq_release,
};

#ifdef CONFIG_DYNAMIC_FTRACE

static int
stack_trace_filter_open(struct inode *inode, struct file *file)
{
	struct ftrace_ops *ops = inode->i_private;

	/* Checks for tracefs lockdown */
	return ftrace_regex_open(ops, FTRACE_ITER_FILTER,
				 inode, file);
}

static const struct file_operations stack_trace_filter_fops = {
	.open = stack_trace_filter_open,
	.read = seq_read,
	.write = ftrace_filter_write,
	.llseek = tracing_lseek,
	.release = ftrace_regex_release,
};

#endif /* CONFIG_DYNAMIC_FTRACE */

int
stack_trace_sysctl(struct ctl_table *table, int write, void *buffer,
		   size_t *lenp, loff_t *ppos)
{
	int was_enabled;
	int ret;

	mutex_lock(&stack_sysctl_mutex);
	was_enabled = !!stack_tracer_enabled;

	ret = proc_dointvec(table, write, buffer, lenp, ppos);

	if (ret || !write || (was_enabled == !!stack_tracer_enabled))
		goto out;

	if (stack_tracer_enabled)
		register_ftrace_function(&trace_ops);
	else
		unregister_ftrace_function(&trace_ops);
 out:
	mutex_unlock(&stack_sysctl_mutex);
	return ret;
}

static char stack_trace_filter_buf[COMMAND_LINE_SIZE+1] __initdata;

static __init int enable_stacktrace(char *str)
{
	int len;

	if ((len = str_has_prefix(str, "_filter=")))
		strncpy(stack_trace_filter_buf, str + len, COMMAND_LINE_SIZE);

	stack_tracer_enabled = 1;
	return 1;
}
__setup("stacktrace", enable_stacktrace);

static __init int stack_trace_init(void)
{
	int ret;

	ret = tracing_init_dentry();
	if (ret)
		return 0;

	trace_create_file("stack_max_size", 0644, NULL,
			&stack_trace_max_size, &stack_max_size_fops);

	trace_create_file("stack_trace", 0444, NULL,
			NULL, &stack_trace_fops);

#ifdef CONFIG_DYNAMIC_FTRACE
	trace_create_file("stack_trace_filter", 0644, NULL,
			  &trace_ops, &stack_trace_filter_fops);
#endif

	if (stack_trace_filter_buf[0])
		ftrace_set_early_filter(&trace_ops, stack_trace_filter_buf, 1);

	if (stack_tracer_enabled)
		register_ftrace_function(&trace_ops);

	return 0;
}

device_initcall(stack_trace_init);
