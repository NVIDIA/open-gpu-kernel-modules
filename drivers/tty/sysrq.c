// SPDX-License-Identifier: GPL-2.0
/*
 *	Linux Magic System Request Key Hacks
 *
 *	(c) 1997 Martin Mares <mj@atrey.karlin.mff.cuni.cz>
 *	based on ideas by Pavel Machek <pavel@atrey.karlin.mff.cuni.cz>
 *
 *	(c) 2000 Crutcher Dunnavant <crutcher+kernel@datastacks.com>
 *	overhauled to use key registration
 *	based upon discusions in irc://irc.openprojects.net/#kernelnewbies
 *
 *	Copyright (c) 2010 Dmitry Torokhov
 *	Input handler conversion
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/sched/signal.h>
#include <linux/sched/rt.h>
#include <linux/sched/debug.h>
#include <linux/sched/task.h>
#include <linux/ctype.h>
#include <linux/interrupt.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/mount.h>
#include <linux/kdev_t.h>
#include <linux/major.h>
#include <linux/reboot.h>
#include <linux/sysrq.h>
#include <linux/kbd_kern.h>
#include <linux/proc_fs.h>
#include <linux/nmi.h>
#include <linux/quotaops.h>
#include <linux/perf_event.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/suspend.h>
#include <linux/writeback.h>
#include <linux/swap.h>
#include <linux/spinlock.h>
#include <linux/vt_kern.h>
#include <linux/workqueue.h>
#include <linux/hrtimer.h>
#include <linux/oom.h>
#include <linux/slab.h>
#include <linux/input.h>
#include <linux/uaccess.h>
#include <linux/moduleparam.h>
#include <linux/jiffies.h>
#include <linux/syscalls.h>
#include <linux/of.h>
#include <linux/rcupdate.h>

#include <asm/ptrace.h>
#include <asm/irq_regs.h>

/* Whether we react on sysrq keys or just ignore them */
static int __read_mostly sysrq_enabled = CONFIG_MAGIC_SYSRQ_DEFAULT_ENABLE;
static bool __read_mostly sysrq_always_enabled;

static bool sysrq_on(void)
{
	return sysrq_enabled || sysrq_always_enabled;
}

/**
 * sysrq_mask - Getter for sysrq_enabled mask.
 *
 * Return: 1 if sysrq is always enabled, enabled sysrq_key_op mask otherwise.
 */
int sysrq_mask(void)
{
	if (sysrq_always_enabled)
		return 1;
	return sysrq_enabled;
}
EXPORT_SYMBOL_GPL(sysrq_mask);

/*
 * A value of 1 means 'all', other nonzero values are an op mask:
 */
static bool sysrq_on_mask(int mask)
{
	return sysrq_always_enabled ||
	       sysrq_enabled == 1 ||
	       (sysrq_enabled & mask);
}

static int __init sysrq_always_enabled_setup(char *str)
{
	sysrq_always_enabled = true;
	pr_info("sysrq always enabled.\n");

	return 1;
}

__setup("sysrq_always_enabled", sysrq_always_enabled_setup);


static void sysrq_handle_loglevel(int key)
{
	int i;

	i = key - '0';
	console_loglevel = CONSOLE_LOGLEVEL_DEFAULT;
	pr_info("Loglevel set to %d\n", i);
	console_loglevel = i;
}
static const struct sysrq_key_op sysrq_loglevel_op = {
	.handler	= sysrq_handle_loglevel,
	.help_msg	= "loglevel(0-9)",
	.action_msg	= "Changing Loglevel",
	.enable_mask	= SYSRQ_ENABLE_LOG,
};

#ifdef CONFIG_VT
static void sysrq_handle_SAK(int key)
{
	struct work_struct *SAK_work = &vc_cons[fg_console].SAK_work;

	schedule_work(SAK_work);
}
static const struct sysrq_key_op sysrq_SAK_op = {
	.handler	= sysrq_handle_SAK,
	.help_msg	= "sak(k)",
	.action_msg	= "SAK",
	.enable_mask	= SYSRQ_ENABLE_KEYBOARD,
};
#else
#define sysrq_SAK_op (*(const struct sysrq_key_op *)NULL)
#endif

#ifdef CONFIG_VT
static void sysrq_handle_unraw(int key)
{
	vt_reset_unicode(fg_console);
}

static const struct sysrq_key_op sysrq_unraw_op = {
	.handler	= sysrq_handle_unraw,
	.help_msg	= "unraw(r)",
	.action_msg	= "Keyboard mode set to system default",
	.enable_mask	= SYSRQ_ENABLE_KEYBOARD,
};
#else
#define sysrq_unraw_op (*(const struct sysrq_key_op *)NULL)
#endif /* CONFIG_VT */

static void sysrq_handle_crash(int key)
{
	/* release the RCU read lock before crashing */
	rcu_read_unlock();

	panic("sysrq triggered crash\n");
}
static const struct sysrq_key_op sysrq_crash_op = {
	.handler	= sysrq_handle_crash,
	.help_msg	= "crash(c)",
	.action_msg	= "Trigger a crash",
	.enable_mask	= SYSRQ_ENABLE_DUMP,
};

static void sysrq_handle_reboot(int key)
{
	lockdep_off();
	local_irq_enable();
	emergency_restart();
}
static const struct sysrq_key_op sysrq_reboot_op = {
	.handler	= sysrq_handle_reboot,
	.help_msg	= "reboot(b)",
	.action_msg	= "Resetting",
	.enable_mask	= SYSRQ_ENABLE_BOOT,
};

const struct sysrq_key_op *__sysrq_reboot_op = &sysrq_reboot_op;

static void sysrq_handle_sync(int key)
{
	emergency_sync();
}
static const struct sysrq_key_op sysrq_sync_op = {
	.handler	= sysrq_handle_sync,
	.help_msg	= "sync(s)",
	.action_msg	= "Emergency Sync",
	.enable_mask	= SYSRQ_ENABLE_SYNC,
};

static void sysrq_handle_show_timers(int key)
{
	sysrq_timer_list_show();
}

static const struct sysrq_key_op sysrq_show_timers_op = {
	.handler	= sysrq_handle_show_timers,
	.help_msg	= "show-all-timers(q)",
	.action_msg	= "Show clockevent devices & pending hrtimers (no others)",
};

static void sysrq_handle_mountro(int key)
{
	emergency_remount();
}
static const struct sysrq_key_op sysrq_mountro_op = {
	.handler	= sysrq_handle_mountro,
	.help_msg	= "unmount(u)",
	.action_msg	= "Emergency Remount R/O",
	.enable_mask	= SYSRQ_ENABLE_REMOUNT,
};

#ifdef CONFIG_LOCKDEP
static void sysrq_handle_showlocks(int key)
{
	debug_show_all_locks();
}

static const struct sysrq_key_op sysrq_showlocks_op = {
	.handler	= sysrq_handle_showlocks,
	.help_msg	= "show-all-locks(d)",
	.action_msg	= "Show Locks Held",
};
#else
#define sysrq_showlocks_op (*(const struct sysrq_key_op *)NULL)
#endif

#ifdef CONFIG_SMP
static DEFINE_RAW_SPINLOCK(show_lock);

static void showacpu(void *dummy)
{
	unsigned long flags;

	/* Idle CPUs have no interesting backtrace. */
	if (idle_cpu(smp_processor_id()))
		return;

	raw_spin_lock_irqsave(&show_lock, flags);
	pr_info("CPU%d:\n", smp_processor_id());
	show_stack(NULL, NULL, KERN_INFO);
	raw_spin_unlock_irqrestore(&show_lock, flags);
}

static void sysrq_showregs_othercpus(struct work_struct *dummy)
{
	smp_call_function(showacpu, NULL, 0);
}

static DECLARE_WORK(sysrq_showallcpus, sysrq_showregs_othercpus);

static void sysrq_handle_showallcpus(int key)
{
	/*
	 * Fall back to the workqueue based printing if the
	 * backtrace printing did not succeed or the
	 * architecture has no support for it:
	 */
	if (!trigger_all_cpu_backtrace()) {
		struct pt_regs *regs = NULL;

		if (in_irq())
			regs = get_irq_regs();
		if (regs) {
			pr_info("CPU%d:\n", smp_processor_id());
			show_regs(regs);
		}
		schedule_work(&sysrq_showallcpus);
	}
}

static const struct sysrq_key_op sysrq_showallcpus_op = {
	.handler	= sysrq_handle_showallcpus,
	.help_msg	= "show-backtrace-all-active-cpus(l)",
	.action_msg	= "Show backtrace of all active CPUs",
	.enable_mask	= SYSRQ_ENABLE_DUMP,
};
#endif

static void sysrq_handle_showregs(int key)
{
	struct pt_regs *regs = NULL;

	if (in_irq())
		regs = get_irq_regs();
	if (regs)
		show_regs(regs);
	perf_event_print_debug();
}
static const struct sysrq_key_op sysrq_showregs_op = {
	.handler	= sysrq_handle_showregs,
	.help_msg	= "show-registers(p)",
	.action_msg	= "Show Regs",
	.enable_mask	= SYSRQ_ENABLE_DUMP,
};

static void sysrq_handle_showstate(int key)
{
	show_state();
	show_workqueue_state();
}
static const struct sysrq_key_op sysrq_showstate_op = {
	.handler	= sysrq_handle_showstate,
	.help_msg	= "show-task-states(t)",
	.action_msg	= "Show State",
	.enable_mask	= SYSRQ_ENABLE_DUMP,
};

static void sysrq_handle_showstate_blocked(int key)
{
	show_state_filter(TASK_UNINTERRUPTIBLE);
}
static const struct sysrq_key_op sysrq_showstate_blocked_op = {
	.handler	= sysrq_handle_showstate_blocked,
	.help_msg	= "show-blocked-tasks(w)",
	.action_msg	= "Show Blocked State",
	.enable_mask	= SYSRQ_ENABLE_DUMP,
};

#ifdef CONFIG_TRACING
#include <linux/ftrace.h>

static void sysrq_ftrace_dump(int key)
{
	ftrace_dump(DUMP_ALL);
}
static const struct sysrq_key_op sysrq_ftrace_dump_op = {
	.handler	= sysrq_ftrace_dump,
	.help_msg	= "dump-ftrace-buffer(z)",
	.action_msg	= "Dump ftrace buffer",
	.enable_mask	= SYSRQ_ENABLE_DUMP,
};
#else
#define sysrq_ftrace_dump_op (*(const struct sysrq_key_op *)NULL)
#endif

static void sysrq_handle_showmem(int key)
{
	show_mem(0, NULL);
}
static const struct sysrq_key_op sysrq_showmem_op = {
	.handler	= sysrq_handle_showmem,
	.help_msg	= "show-memory-usage(m)",
	.action_msg	= "Show Memory",
	.enable_mask	= SYSRQ_ENABLE_DUMP,
};

/*
 * Signal sysrq helper function.  Sends a signal to all user processes.
 */
static void send_sig_all(int sig)
{
	struct task_struct *p;

	read_lock(&tasklist_lock);
	for_each_process(p) {
		if (p->flags & PF_KTHREAD)
			continue;
		if (is_global_init(p))
			continue;

		do_send_sig_info(sig, SEND_SIG_PRIV, p, PIDTYPE_MAX);
	}
	read_unlock(&tasklist_lock);
}

static void sysrq_handle_term(int key)
{
	send_sig_all(SIGTERM);
	console_loglevel = CONSOLE_LOGLEVEL_DEBUG;
}
static const struct sysrq_key_op sysrq_term_op = {
	.handler	= sysrq_handle_term,
	.help_msg	= "terminate-all-tasks(e)",
	.action_msg	= "Terminate All Tasks",
	.enable_mask	= SYSRQ_ENABLE_SIGNAL,
};

static void moom_callback(struct work_struct *ignored)
{
	const gfp_t gfp_mask = GFP_KERNEL;
	struct oom_control oc = {
		.zonelist = node_zonelist(first_memory_node, gfp_mask),
		.nodemask = NULL,
		.memcg = NULL,
		.gfp_mask = gfp_mask,
		.order = -1,
	};

	mutex_lock(&oom_lock);
	if (!out_of_memory(&oc))
		pr_info("OOM request ignored. No task eligible\n");
	mutex_unlock(&oom_lock);
}

static DECLARE_WORK(moom_work, moom_callback);

static void sysrq_handle_moom(int key)
{
	schedule_work(&moom_work);
}
static const struct sysrq_key_op sysrq_moom_op = {
	.handler	= sysrq_handle_moom,
	.help_msg	= "memory-full-oom-kill(f)",
	.action_msg	= "Manual OOM execution",
	.enable_mask	= SYSRQ_ENABLE_SIGNAL,
};

static void sysrq_handle_thaw(int key)
{
	emergency_thaw_all();
}
static const struct sysrq_key_op sysrq_thaw_op = {
	.handler	= sysrq_handle_thaw,
	.help_msg	= "thaw-filesystems(j)",
	.action_msg	= "Emergency Thaw of all frozen filesystems",
	.enable_mask	= SYSRQ_ENABLE_SIGNAL,
};

static void sysrq_handle_kill(int key)
{
	send_sig_all(SIGKILL);
	console_loglevel = CONSOLE_LOGLEVEL_DEBUG;
}
static const struct sysrq_key_op sysrq_kill_op = {
	.handler	= sysrq_handle_kill,
	.help_msg	= "kill-all-tasks(i)",
	.action_msg	= "Kill All Tasks",
	.enable_mask	= SYSRQ_ENABLE_SIGNAL,
};

static void sysrq_handle_unrt(int key)
{
	normalize_rt_tasks();
}
static const struct sysrq_key_op sysrq_unrt_op = {
	.handler	= sysrq_handle_unrt,
	.help_msg	= "nice-all-RT-tasks(n)",
	.action_msg	= "Nice All RT Tasks",
	.enable_mask	= SYSRQ_ENABLE_RTNICE,
};

/* Key Operations table and lock */
static DEFINE_SPINLOCK(sysrq_key_table_lock);

static const struct sysrq_key_op *sysrq_key_table[62] = {
	&sysrq_loglevel_op,		/* 0 */
	&sysrq_loglevel_op,		/* 1 */
	&sysrq_loglevel_op,		/* 2 */
	&sysrq_loglevel_op,		/* 3 */
	&sysrq_loglevel_op,		/* 4 */
	&sysrq_loglevel_op,		/* 5 */
	&sysrq_loglevel_op,		/* 6 */
	&sysrq_loglevel_op,		/* 7 */
	&sysrq_loglevel_op,		/* 8 */
	&sysrq_loglevel_op,		/* 9 */

	/*
	 * a: Don't use for system provided sysrqs, it is handled specially on
	 * sparc and will never arrive.
	 */
	NULL,				/* a */
	&sysrq_reboot_op,		/* b */
	&sysrq_crash_op,		/* c */
	&sysrq_showlocks_op,		/* d */
	&sysrq_term_op,			/* e */
	&sysrq_moom_op,			/* f */
	/* g: May be registered for the kernel debugger */
	NULL,				/* g */
	NULL,				/* h - reserved for help */
	&sysrq_kill_op,			/* i */
#ifdef CONFIG_BLOCK
	&sysrq_thaw_op,			/* j */
#else
	NULL,				/* j */
#endif
	&sysrq_SAK_op,			/* k */
#ifdef CONFIG_SMP
	&sysrq_showallcpus_op,		/* l */
#else
	NULL,				/* l */
#endif
	&sysrq_showmem_op,		/* m */
	&sysrq_unrt_op,			/* n */
	/* o: This will often be registered as 'Off' at init time */
	NULL,				/* o */
	&sysrq_showregs_op,		/* p */
	&sysrq_show_timers_op,		/* q */
	&sysrq_unraw_op,		/* r */
	&sysrq_sync_op,			/* s */
	&sysrq_showstate_op,		/* t */
	&sysrq_mountro_op,		/* u */
	/* v: May be registered for frame buffer console restore */
	NULL,				/* v */
	&sysrq_showstate_blocked_op,	/* w */
	/* x: May be registered on mips for TLB dump */
	/* x: May be registered on ppc/powerpc for xmon */
	/* x: May be registered on sparc64 for global PMU dump */
	NULL,				/* x */
	/* y: May be registered on sparc64 for global register dump */
	NULL,				/* y */
	&sysrq_ftrace_dump_op,		/* z */
	NULL,				/* A */
	NULL,				/* B */
	NULL,				/* C */
	NULL,				/* D */
	NULL,				/* E */
	NULL,				/* F */
	NULL,				/* G */
	NULL,				/* H */
	NULL,				/* I */
	NULL,				/* J */
	NULL,				/* K */
	NULL,				/* L */
	NULL,				/* M */
	NULL,				/* N */
	NULL,				/* O */
	NULL,				/* P */
	NULL,				/* Q */
	NULL,				/* R */
	NULL,				/* S */
	NULL,				/* T */
	NULL,				/* U */
	NULL,				/* V */
	NULL,				/* W */
	NULL,				/* X */
	NULL,				/* Y */
	NULL,				/* Z */
};

/* key2index calculation, -1 on invalid index */
static int sysrq_key_table_key2index(int key)
{
	int retval;

	if ((key >= '0') && (key <= '9'))
		retval = key - '0';
	else if ((key >= 'a') && (key <= 'z'))
		retval = key + 10 - 'a';
	else if ((key >= 'A') && (key <= 'Z'))
		retval = key + 36 - 'A';
	else
		retval = -1;
	return retval;
}

/*
 * get and put functions for the table, exposed to modules.
 */
static const struct sysrq_key_op *__sysrq_get_key_op(int key)
{
	const struct sysrq_key_op *op_p = NULL;
	int i;

	i = sysrq_key_table_key2index(key);
	if (i != -1)
		op_p = sysrq_key_table[i];

	return op_p;
}

static void __sysrq_put_key_op(int key, const struct sysrq_key_op *op_p)
{
	int i = sysrq_key_table_key2index(key);

	if (i != -1)
		sysrq_key_table[i] = op_p;
}

void __handle_sysrq(int key, bool check_mask)
{
	const struct sysrq_key_op *op_p;
	int orig_log_level;
	int orig_suppress_printk;
	int i;

	orig_suppress_printk = suppress_printk;
	suppress_printk = 0;

	rcu_sysrq_start();
	rcu_read_lock();
	/*
	 * Raise the apparent loglevel to maximum so that the sysrq header
	 * is shown to provide the user with positive feedback.  We do not
	 * simply emit this at KERN_EMERG as that would change message
	 * routing in the consumers of /proc/kmsg.
	 */
	orig_log_level = console_loglevel;
	console_loglevel = CONSOLE_LOGLEVEL_DEFAULT;

	op_p = __sysrq_get_key_op(key);
	if (op_p) {
		/*
		 * Should we check for enabled operations (/proc/sysrq-trigger
		 * should not) and is the invoked operation enabled?
		 */
		if (!check_mask || sysrq_on_mask(op_p->enable_mask)) {
			pr_info("%s\n", op_p->action_msg);
			console_loglevel = orig_log_level;
			op_p->handler(key);
		} else {
			pr_info("This sysrq operation is disabled.\n");
			console_loglevel = orig_log_level;
		}
	} else {
		pr_info("HELP : ");
		/* Only print the help msg once per handler */
		for (i = 0; i < ARRAY_SIZE(sysrq_key_table); i++) {
			if (sysrq_key_table[i]) {
				int j;

				for (j = 0; sysrq_key_table[i] !=
						sysrq_key_table[j]; j++)
					;
				if (j != i)
					continue;
				pr_cont("%s ", sysrq_key_table[i]->help_msg);
			}
		}
		pr_cont("\n");
		console_loglevel = orig_log_level;
	}
	rcu_read_unlock();
	rcu_sysrq_end();

	suppress_printk = orig_suppress_printk;
}

void handle_sysrq(int key)
{
	if (sysrq_on())
		__handle_sysrq(key, true);
}
EXPORT_SYMBOL(handle_sysrq);

#ifdef CONFIG_INPUT
static int sysrq_reset_downtime_ms;

/* Simple translation table for the SysRq keys */
static const unsigned char sysrq_xlate[KEY_CNT] =
	"\000\0331234567890-=\177\t"                    /* 0x00 - 0x0f */
	"qwertyuiop[]\r\000as"                          /* 0x10 - 0x1f */
	"dfghjkl;'`\000\\zxcv"                          /* 0x20 - 0x2f */
	"bnm,./\000*\000 \000\201\202\203\204\205"      /* 0x30 - 0x3f */
	"\206\207\210\211\212\000\000789-456+1"         /* 0x40 - 0x4f */
	"230\177\000\000\213\214\000\000\000\000\000\000\000\000\000\000" /* 0x50 - 0x5f */
	"\r\000/";                                      /* 0x60 - 0x6f */

struct sysrq_state {
	struct input_handle handle;
	struct work_struct reinject_work;
	unsigned long key_down[BITS_TO_LONGS(KEY_CNT)];
	unsigned int alt;
	unsigned int alt_use;
	unsigned int shift;
	unsigned int shift_use;
	bool active;
	bool need_reinject;
	bool reinjecting;

	/* reset sequence handling */
	bool reset_canceled;
	bool reset_requested;
	unsigned long reset_keybit[BITS_TO_LONGS(KEY_CNT)];
	int reset_seq_len;
	int reset_seq_cnt;
	int reset_seq_version;
	struct timer_list keyreset_timer;
};

#define SYSRQ_KEY_RESET_MAX	20 /* Should be plenty */
static unsigned short sysrq_reset_seq[SYSRQ_KEY_RESET_MAX];
static unsigned int sysrq_reset_seq_len;
static unsigned int sysrq_reset_seq_version = 1;

static void sysrq_parse_reset_sequence(struct sysrq_state *state)
{
	int i;
	unsigned short key;

	state->reset_seq_cnt = 0;

	for (i = 0; i < sysrq_reset_seq_len; i++) {
		key = sysrq_reset_seq[i];

		if (key == KEY_RESERVED || key > KEY_MAX)
			break;

		__set_bit(key, state->reset_keybit);
		state->reset_seq_len++;

		if (test_bit(key, state->key_down))
			state->reset_seq_cnt++;
	}

	/* Disable reset until old keys are not released */
	state->reset_canceled = state->reset_seq_cnt != 0;

	state->reset_seq_version = sysrq_reset_seq_version;
}

static void sysrq_do_reset(struct timer_list *t)
{
	struct sysrq_state *state = from_timer(state, t, keyreset_timer);

	state->reset_requested = true;

	orderly_reboot();
}

static void sysrq_handle_reset_request(struct sysrq_state *state)
{
	if (state->reset_requested)
		__handle_sysrq(sysrq_xlate[KEY_B], false);

	if (sysrq_reset_downtime_ms)
		mod_timer(&state->keyreset_timer,
			jiffies + msecs_to_jiffies(sysrq_reset_downtime_ms));
	else
		sysrq_do_reset(&state->keyreset_timer);
}

static void sysrq_detect_reset_sequence(struct sysrq_state *state,
					unsigned int code, int value)
{
	if (!test_bit(code, state->reset_keybit)) {
		/*
		 * Pressing any key _not_ in reset sequence cancels
		 * the reset sequence.  Also cancelling the timer in
		 * case additional keys were pressed after a reset
		 * has been requested.
		 */
		if (value && state->reset_seq_cnt) {
			state->reset_canceled = true;
			del_timer(&state->keyreset_timer);
		}
	} else if (value == 0) {
		/*
		 * Key release - all keys in the reset sequence need
		 * to be pressed and held for the reset timeout
		 * to hold.
		 */
		del_timer(&state->keyreset_timer);

		if (--state->reset_seq_cnt == 0)
			state->reset_canceled = false;
	} else if (value == 1) {
		/* key press, not autorepeat */
		if (++state->reset_seq_cnt == state->reset_seq_len &&
		    !state->reset_canceled) {
			sysrq_handle_reset_request(state);
		}
	}
}

#ifdef CONFIG_OF
static void sysrq_of_get_keyreset_config(void)
{
	u32 key;
	struct device_node *np;
	struct property *prop;
	const __be32 *p;

	np = of_find_node_by_path("/chosen/linux,sysrq-reset-seq");
	if (!np) {
		pr_debug("No sysrq node found");
		return;
	}

	/* Reset in case a __weak definition was present */
	sysrq_reset_seq_len = 0;

	of_property_for_each_u32(np, "keyset", prop, p, key) {
		if (key == KEY_RESERVED || key > KEY_MAX ||
		    sysrq_reset_seq_len == SYSRQ_KEY_RESET_MAX)
			break;

		sysrq_reset_seq[sysrq_reset_seq_len++] = (unsigned short)key;
	}

	/* Get reset timeout if any. */
	of_property_read_u32(np, "timeout-ms", &sysrq_reset_downtime_ms);

	of_node_put(np);
}
#else
static void sysrq_of_get_keyreset_config(void)
{
}
#endif

static void sysrq_reinject_alt_sysrq(struct work_struct *work)
{
	struct sysrq_state *sysrq =
			container_of(work, struct sysrq_state, reinject_work);
	struct input_handle *handle = &sysrq->handle;
	unsigned int alt_code = sysrq->alt_use;

	if (sysrq->need_reinject) {
		/* we do not want the assignment to be reordered */
		sysrq->reinjecting = true;
		mb();

		/* Simulate press and release of Alt + SysRq */
		input_inject_event(handle, EV_KEY, alt_code, 1);
		input_inject_event(handle, EV_KEY, KEY_SYSRQ, 1);
		input_inject_event(handle, EV_SYN, SYN_REPORT, 1);

		input_inject_event(handle, EV_KEY, KEY_SYSRQ, 0);
		input_inject_event(handle, EV_KEY, alt_code, 0);
		input_inject_event(handle, EV_SYN, SYN_REPORT, 1);

		mb();
		sysrq->reinjecting = false;
	}
}

static bool sysrq_handle_keypress(struct sysrq_state *sysrq,
				  unsigned int code, int value)
{
	bool was_active = sysrq->active;
	bool suppress;

	switch (code) {

	case KEY_LEFTALT:
	case KEY_RIGHTALT:
		if (!value) {
			/* One of ALTs is being released */
			if (sysrq->active && code == sysrq->alt_use)
				sysrq->active = false;

			sysrq->alt = KEY_RESERVED;

		} else if (value != 2) {
			sysrq->alt = code;
			sysrq->need_reinject = false;
		}
		break;

	case KEY_LEFTSHIFT:
	case KEY_RIGHTSHIFT:
		if (!value)
			sysrq->shift = KEY_RESERVED;
		else if (value != 2)
			sysrq->shift = code;
		break;

	case KEY_SYSRQ:
		if (value == 1 && sysrq->alt != KEY_RESERVED) {
			sysrq->active = true;
			sysrq->alt_use = sysrq->alt;
			/* either RESERVED (for released) or actual code */
			sysrq->shift_use = sysrq->shift;
			/*
			 * If nothing else will be pressed we'll need
			 * to re-inject Alt-SysRq keysroke.
			 */
			sysrq->need_reinject = true;
		}

		/*
		 * Pretend that sysrq was never pressed at all. This
		 * is needed to properly handle KGDB which will try
		 * to release all keys after exiting debugger. If we
		 * do not clear key bit it KGDB will end up sending
		 * release events for Alt and SysRq, potentially
		 * triggering print screen function.
		 */
		if (sysrq->active)
			clear_bit(KEY_SYSRQ, sysrq->handle.dev->key);

		break;

	default:
		if (sysrq->active && value && value != 2) {
			unsigned char c = sysrq_xlate[code];

			sysrq->need_reinject = false;
			if (sysrq->shift_use != KEY_RESERVED)
				c = toupper(c);
			__handle_sysrq(c, true);
		}
		break;
	}

	suppress = sysrq->active;

	if (!sysrq->active) {

		/*
		 * See if reset sequence has changed since the last time.
		 */
		if (sysrq->reset_seq_version != sysrq_reset_seq_version)
			sysrq_parse_reset_sequence(sysrq);

		/*
		 * If we are not suppressing key presses keep track of
		 * keyboard state so we can release keys that have been
		 * pressed before entering SysRq mode.
		 */
		if (value)
			set_bit(code, sysrq->key_down);
		else
			clear_bit(code, sysrq->key_down);

		if (was_active)
			schedule_work(&sysrq->reinject_work);

		/* Check for reset sequence */
		sysrq_detect_reset_sequence(sysrq, code, value);

	} else if (value == 0 && test_and_clear_bit(code, sysrq->key_down)) {
		/*
		 * Pass on release events for keys that was pressed before
		 * entering SysRq mode.
		 */
		suppress = false;
	}

	return suppress;
}

static bool sysrq_filter(struct input_handle *handle,
			 unsigned int type, unsigned int code, int value)
{
	struct sysrq_state *sysrq = handle->private;
	bool suppress;

	/*
	 * Do not filter anything if we are in the process of re-injecting
	 * Alt+SysRq combination.
	 */
	if (sysrq->reinjecting)
		return false;

	switch (type) {

	case EV_SYN:
		suppress = false;
		break;

	case EV_KEY:
		suppress = sysrq_handle_keypress(sysrq, code, value);
		break;

	default:
		suppress = sysrq->active;
		break;
	}

	return suppress;
}

static int sysrq_connect(struct input_handler *handler,
			 struct input_dev *dev,
			 const struct input_device_id *id)
{
	struct sysrq_state *sysrq;
	int error;

	sysrq = kzalloc(sizeof(struct sysrq_state), GFP_KERNEL);
	if (!sysrq)
		return -ENOMEM;

	INIT_WORK(&sysrq->reinject_work, sysrq_reinject_alt_sysrq);

	sysrq->handle.dev = dev;
	sysrq->handle.handler = handler;
	sysrq->handle.name = "sysrq";
	sysrq->handle.private = sysrq;
	timer_setup(&sysrq->keyreset_timer, sysrq_do_reset, 0);

	error = input_register_handle(&sysrq->handle);
	if (error) {
		pr_err("Failed to register input sysrq handler, error %d\n",
			error);
		goto err_free;
	}

	error = input_open_device(&sysrq->handle);
	if (error) {
		pr_err("Failed to open input device, error %d\n", error);
		goto err_unregister;
	}

	return 0;

 err_unregister:
	input_unregister_handle(&sysrq->handle);
 err_free:
	kfree(sysrq);
	return error;
}

static void sysrq_disconnect(struct input_handle *handle)
{
	struct sysrq_state *sysrq = handle->private;

	input_close_device(handle);
	cancel_work_sync(&sysrq->reinject_work);
	del_timer_sync(&sysrq->keyreset_timer);
	input_unregister_handle(handle);
	kfree(sysrq);
}

/*
 * We are matching on KEY_LEFTALT instead of KEY_SYSRQ because not all
 * keyboards have SysRq key predefined and so user may add it to keymap
 * later, but we expect all such keyboards to have left alt.
 */
static const struct input_device_id sysrq_ids[] = {
	{
		.flags = INPUT_DEVICE_ID_MATCH_EVBIT |
				INPUT_DEVICE_ID_MATCH_KEYBIT,
		.evbit = { [BIT_WORD(EV_KEY)] = BIT_MASK(EV_KEY) },
		.keybit = { [BIT_WORD(KEY_LEFTALT)] = BIT_MASK(KEY_LEFTALT) },
	},
	{ },
};

static struct input_handler sysrq_handler = {
	.filter		= sysrq_filter,
	.connect	= sysrq_connect,
	.disconnect	= sysrq_disconnect,
	.name		= "sysrq",
	.id_table	= sysrq_ids,
};

static inline void sysrq_register_handler(void)
{
	int error;

	sysrq_of_get_keyreset_config();

	error = input_register_handler(&sysrq_handler);
	if (error)
		pr_err("Failed to register input handler, error %d", error);
}

static inline void sysrq_unregister_handler(void)
{
	input_unregister_handler(&sysrq_handler);
}

static int sysrq_reset_seq_param_set(const char *buffer,
				     const struct kernel_param *kp)
{
	unsigned long val;
	int error;

	error = kstrtoul(buffer, 0, &val);
	if (error < 0)
		return error;

	if (val > KEY_MAX)
		return -EINVAL;

	*((unsigned short *)kp->arg) = val;
	sysrq_reset_seq_version++;

	return 0;
}

static const struct kernel_param_ops param_ops_sysrq_reset_seq = {
	.get	= param_get_ushort,
	.set	= sysrq_reset_seq_param_set,
};

#define param_check_sysrq_reset_seq(name, p)	\
	__param_check(name, p, unsigned short)

/*
 * not really modular, but the easiest way to keep compat with existing
 * bootargs behaviour is to continue using module_param here.
 */
module_param_array_named(reset_seq, sysrq_reset_seq, sysrq_reset_seq,
			 &sysrq_reset_seq_len, 0644);

module_param_named(sysrq_downtime_ms, sysrq_reset_downtime_ms, int, 0644);

#else

static inline void sysrq_register_handler(void)
{
}

static inline void sysrq_unregister_handler(void)
{
}

#endif /* CONFIG_INPUT */

int sysrq_toggle_support(int enable_mask)
{
	bool was_enabled = sysrq_on();

	sysrq_enabled = enable_mask;

	if (was_enabled != sysrq_on()) {
		if (sysrq_on())
			sysrq_register_handler();
		else
			sysrq_unregister_handler();
	}

	return 0;
}
EXPORT_SYMBOL_GPL(sysrq_toggle_support);

static int __sysrq_swap_key_ops(int key, const struct sysrq_key_op *insert_op_p,
				const struct sysrq_key_op *remove_op_p)
{
	int retval;

	spin_lock(&sysrq_key_table_lock);
	if (__sysrq_get_key_op(key) == remove_op_p) {
		__sysrq_put_key_op(key, insert_op_p);
		retval = 0;
	} else {
		retval = -1;
	}
	spin_unlock(&sysrq_key_table_lock);

	/*
	 * A concurrent __handle_sysrq either got the old op or the new op.
	 * Wait for it to go away before returning, so the code for an old
	 * op is not freed (eg. on module unload) while it is in use.
	 */
	synchronize_rcu();

	return retval;
}

int register_sysrq_key(int key, const struct sysrq_key_op *op_p)
{
	return __sysrq_swap_key_ops(key, op_p, NULL);
}
EXPORT_SYMBOL(register_sysrq_key);

int unregister_sysrq_key(int key, const struct sysrq_key_op *op_p)
{
	return __sysrq_swap_key_ops(key, NULL, op_p);
}
EXPORT_SYMBOL(unregister_sysrq_key);

#ifdef CONFIG_PROC_FS
/*
 * writing 'C' to /proc/sysrq-trigger is like sysrq-C
 */
static ssize_t write_sysrq_trigger(struct file *file, const char __user *buf,
				   size_t count, loff_t *ppos)
{
	if (count) {
		char c;

		if (get_user(c, buf))
			return -EFAULT;
		__handle_sysrq(c, false);
	}

	return count;
}

static const struct proc_ops sysrq_trigger_proc_ops = {
	.proc_write	= write_sysrq_trigger,
	.proc_lseek	= noop_llseek,
};

static void sysrq_init_procfs(void)
{
	if (!proc_create("sysrq-trigger", S_IWUSR, NULL,
			 &sysrq_trigger_proc_ops))
		pr_err("Failed to register proc interface\n");
}

#else

static inline void sysrq_init_procfs(void)
{
}

#endif /* CONFIG_PROC_FS */

static int __init sysrq_init(void)
{
	sysrq_init_procfs();

	if (sysrq_on())
		sysrq_register_handler();

	return 0;
}
device_initcall(sysrq_init);
