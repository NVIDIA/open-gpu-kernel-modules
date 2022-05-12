/*
 * Performance events:
 *
 *    Copyright (C) 2008-2009, Thomas Gleixner <tglx@linutronix.de>
 *    Copyright (C) 2008-2011, Red Hat, Inc., Ingo Molnar
 *    Copyright (C) 2008-2011, Red Hat, Inc., Peter Zijlstra
 *
 * Data type definitions, declarations, prototypes.
 *
 *    Started by: Thomas Gleixner and Ingo Molnar
 *
 * For licencing details see kernel-base/COPYING
 */
#ifndef _LINUX_PERF_EVENT_H
#define _LINUX_PERF_EVENT_H

#include <uapi/linux/perf_event.h>
#include <uapi/linux/bpf_perf_event.h>

/*
 * Kernel-internal data types and definitions:
 */

#ifdef CONFIG_PERF_EVENTS
# include <asm/perf_event.h>
# include <asm/local64.h>
#endif

struct perf_guest_info_callbacks {
	int				(*is_in_guest)(void);
	int				(*is_user_mode)(void);
	unsigned long			(*get_guest_ip)(void);
	void				(*handle_intel_pt_intr)(void);
};

#ifdef CONFIG_HAVE_HW_BREAKPOINT
#include <asm/hw_breakpoint.h>
#endif

#include <linux/list.h>
#include <linux/mutex.h>
#include <linux/rculist.h>
#include <linux/rcupdate.h>
#include <linux/spinlock.h>
#include <linux/hrtimer.h>
#include <linux/fs.h>
#include <linux/pid_namespace.h>
#include <linux/workqueue.h>
#include <linux/ftrace.h>
#include <linux/cpu.h>
#include <linux/irq_work.h>
#include <linux/static_key.h>
#include <linux/jump_label_ratelimit.h>
#include <linux/atomic.h>
#include <linux/sysfs.h>
#include <linux/perf_regs.h>
#include <linux/cgroup.h>
#include <linux/refcount.h>
#include <linux/security.h>
#include <asm/local.h>

struct perf_callchain_entry {
	__u64				nr;
	__u64				ip[]; /* /proc/sys/kernel/perf_event_max_stack */
};

struct perf_callchain_entry_ctx {
	struct perf_callchain_entry *entry;
	u32			    max_stack;
	u32			    nr;
	short			    contexts;
	bool			    contexts_maxed;
};

typedef unsigned long (*perf_copy_f)(void *dst, const void *src,
				     unsigned long off, unsigned long len);

struct perf_raw_frag {
	union {
		struct perf_raw_frag	*next;
		unsigned long		pad;
	};
	perf_copy_f			copy;
	void				*data;
	u32				size;
} __packed;

struct perf_raw_record {
	struct perf_raw_frag		frag;
	u32				size;
};

/*
 * branch stack layout:
 *  nr: number of taken branches stored in entries[]
 *  hw_idx: The low level index of raw branch records
 *          for the most recent branch.
 *          -1ULL means invalid/unknown.
 *
 * Note that nr can vary from sample to sample
 * branches (to, from) are stored from most recent
 * to least recent, i.e., entries[0] contains the most
 * recent branch.
 * The entries[] is an abstraction of raw branch records,
 * which may not be stored in age order in HW, e.g. Intel LBR.
 * The hw_idx is to expose the low level index of raw
 * branch record for the most recent branch aka entries[0].
 * The hw_idx index is between -1 (unknown) and max depth,
 * which can be retrieved in /sys/devices/cpu/caps/branches.
 * For the architectures whose raw branch records are
 * already stored in age order, the hw_idx should be 0.
 */
struct perf_branch_stack {
	__u64				nr;
	__u64				hw_idx;
	struct perf_branch_entry	entries[];
};

struct task_struct;

/*
 * extra PMU register associated with an event
 */
struct hw_perf_event_extra {
	u64		config;	/* register value */
	unsigned int	reg;	/* register address or index */
	int		alloc;	/* extra register already allocated */
	int		idx;	/* index in shared_regs->regs[] */
};

/**
 * struct hw_perf_event - performance event hardware details:
 */
struct hw_perf_event {
#ifdef CONFIG_PERF_EVENTS
	union {
		struct { /* hardware */
			u64		config;
			u64		last_tag;
			unsigned long	config_base;
			unsigned long	event_base;
			int		event_base_rdpmc;
			int		idx;
			int		last_cpu;
			int		flags;

			struct hw_perf_event_extra extra_reg;
			struct hw_perf_event_extra branch_reg;
		};
		struct { /* software */
			struct hrtimer	hrtimer;
		};
		struct { /* tracepoint */
			/* for tp_event->class */
			struct list_head	tp_list;
		};
		struct { /* amd_power */
			u64	pwr_acc;
			u64	ptsc;
		};
#ifdef CONFIG_HAVE_HW_BREAKPOINT
		struct { /* breakpoint */
			/*
			 * Crufty hack to avoid the chicken and egg
			 * problem hw_breakpoint has with context
			 * creation and event initalization.
			 */
			struct arch_hw_breakpoint	info;
			struct list_head		bp_list;
		};
#endif
		struct { /* amd_iommu */
			u8	iommu_bank;
			u8	iommu_cntr;
			u16	padding;
			u64	conf;
			u64	conf1;
		};
	};
	/*
	 * If the event is a per task event, this will point to the task in
	 * question. See the comment in perf_event_alloc().
	 */
	struct task_struct		*target;

	/*
	 * PMU would store hardware filter configuration
	 * here.
	 */
	void				*addr_filters;

	/* Last sync'ed generation of filters */
	unsigned long			addr_filters_gen;

/*
 * hw_perf_event::state flags; used to track the PERF_EF_* state.
 */
#define PERF_HES_STOPPED	0x01 /* the counter is stopped */
#define PERF_HES_UPTODATE	0x02 /* event->count up-to-date */
#define PERF_HES_ARCH		0x04

	int				state;

	/*
	 * The last observed hardware counter value, updated with a
	 * local64_cmpxchg() such that pmu::read() can be called nested.
	 */
	local64_t			prev_count;

	/*
	 * The period to start the next sample with.
	 */
	u64				sample_period;

	union {
		struct { /* Sampling */
			/*
			 * The period we started this sample with.
			 */
			u64				last_period;

			/*
			 * However much is left of the current period;
			 * note that this is a full 64bit value and
			 * allows for generation of periods longer
			 * than hardware might allow.
			 */
			local64_t			period_left;
		};
		struct { /* Topdown events counting for context switch */
			u64				saved_metric;
			u64				saved_slots;
		};
	};

	/*
	 * State for throttling the event, see __perf_event_overflow() and
	 * perf_adjust_freq_unthr_context().
	 */
	u64                             interrupts_seq;
	u64				interrupts;

	/*
	 * State for freq target events, see __perf_event_overflow() and
	 * perf_adjust_freq_unthr_context().
	 */
	u64				freq_time_stamp;
	u64				freq_count_stamp;
#endif
};

struct perf_event;

/*
 * Common implementation detail of pmu::{start,commit,cancel}_txn
 */
#define PERF_PMU_TXN_ADD  0x1		/* txn to add/schedule event on PMU */
#define PERF_PMU_TXN_READ 0x2		/* txn to read event group from PMU */

/**
 * pmu::capabilities flags
 */
#define PERF_PMU_CAP_NO_INTERRUPT		0x0001
#define PERF_PMU_CAP_NO_NMI			0x0002
#define PERF_PMU_CAP_AUX_NO_SG			0x0004
#define PERF_PMU_CAP_EXTENDED_REGS		0x0008
#define PERF_PMU_CAP_EXCLUSIVE			0x0010
#define PERF_PMU_CAP_ITRACE			0x0020
#define PERF_PMU_CAP_HETEROGENEOUS_CPUS		0x0040
#define PERF_PMU_CAP_NO_EXCLUDE			0x0080
#define PERF_PMU_CAP_AUX_OUTPUT			0x0100
#define PERF_PMU_CAP_EXTENDED_HW_TYPE		0x0200

struct perf_output_handle;

/**
 * struct pmu - generic performance monitoring unit
 */
struct pmu {
	struct list_head		entry;

	struct module			*module;
	struct device			*dev;
	const struct attribute_group	**attr_groups;
	const struct attribute_group	**attr_update;
	const char			*name;
	int				type;

	/*
	 * various common per-pmu feature flags
	 */
	int				capabilities;

	int __percpu			*pmu_disable_count;
	struct perf_cpu_context __percpu *pmu_cpu_context;
	atomic_t			exclusive_cnt; /* < 0: cpu; > 0: tsk */
	int				task_ctx_nr;
	int				hrtimer_interval_ms;

	/* number of address filters this PMU can do */
	unsigned int			nr_addr_filters;

	/*
	 * Fully disable/enable this PMU, can be used to protect from the PMI
	 * as well as for lazy/batch writing of the MSRs.
	 */
	void (*pmu_enable)		(struct pmu *pmu); /* optional */
	void (*pmu_disable)		(struct pmu *pmu); /* optional */

	/*
	 * Try and initialize the event for this PMU.
	 *
	 * Returns:
	 *  -ENOENT	-- @event is not for this PMU
	 *
	 *  -ENODEV	-- @event is for this PMU but PMU not present
	 *  -EBUSY	-- @event is for this PMU but PMU temporarily unavailable
	 *  -EINVAL	-- @event is for this PMU but @event is not valid
	 *  -EOPNOTSUPP -- @event is for this PMU, @event is valid, but not supported
	 *  -EACCES	-- @event is for this PMU, @event is valid, but no privileges
	 *
	 *  0		-- @event is for this PMU and valid
	 *
	 * Other error return values are allowed.
	 */
	int (*event_init)		(struct perf_event *event);

	/*
	 * Notification that the event was mapped or unmapped.  Called
	 * in the context of the mapping task.
	 */
	void (*event_mapped)		(struct perf_event *event, struct mm_struct *mm); /* optional */
	void (*event_unmapped)		(struct perf_event *event, struct mm_struct *mm); /* optional */

	/*
	 * Flags for ->add()/->del()/ ->start()/->stop(). There are
	 * matching hw_perf_event::state flags.
	 */
#define PERF_EF_START	0x01		/* start the counter when adding    */
#define PERF_EF_RELOAD	0x02		/* reload the counter when starting */
#define PERF_EF_UPDATE	0x04		/* update the counter when stopping */

	/*
	 * Adds/Removes a counter to/from the PMU, can be done inside a
	 * transaction, see the ->*_txn() methods.
	 *
	 * The add/del callbacks will reserve all hardware resources required
	 * to service the event, this includes any counter constraint
	 * scheduling etc.
	 *
	 * Called with IRQs disabled and the PMU disabled on the CPU the event
	 * is on.
	 *
	 * ->add() called without PERF_EF_START should result in the same state
	 *  as ->add() followed by ->stop().
	 *
	 * ->del() must always PERF_EF_UPDATE stop an event. If it calls
	 *  ->stop() that must deal with already being stopped without
	 *  PERF_EF_UPDATE.
	 */
	int  (*add)			(struct perf_event *event, int flags);
	void (*del)			(struct perf_event *event, int flags);

	/*
	 * Starts/Stops a counter present on the PMU.
	 *
	 * The PMI handler should stop the counter when perf_event_overflow()
	 * returns !0. ->start() will be used to continue.
	 *
	 * Also used to change the sample period.
	 *
	 * Called with IRQs disabled and the PMU disabled on the CPU the event
	 * is on -- will be called from NMI context with the PMU generates
	 * NMIs.
	 *
	 * ->stop() with PERF_EF_UPDATE will read the counter and update
	 *  period/count values like ->read() would.
	 *
	 * ->start() with PERF_EF_RELOAD will reprogram the counter
	 *  value, must be preceded by a ->stop() with PERF_EF_UPDATE.
	 */
	void (*start)			(struct perf_event *event, int flags);
	void (*stop)			(struct perf_event *event, int flags);

	/*
	 * Updates the counter value of the event.
	 *
	 * For sampling capable PMUs this will also update the software period
	 * hw_perf_event::period_left field.
	 */
	void (*read)			(struct perf_event *event);

	/*
	 * Group events scheduling is treated as a transaction, add
	 * group events as a whole and perform one schedulability test.
	 * If the test fails, roll back the whole group
	 *
	 * Start the transaction, after this ->add() doesn't need to
	 * do schedulability tests.
	 *
	 * Optional.
	 */
	void (*start_txn)		(struct pmu *pmu, unsigned int txn_flags);
	/*
	 * If ->start_txn() disabled the ->add() schedulability test
	 * then ->commit_txn() is required to perform one. On success
	 * the transaction is closed. On error the transaction is kept
	 * open until ->cancel_txn() is called.
	 *
	 * Optional.
	 */
	int  (*commit_txn)		(struct pmu *pmu);
	/*
	 * Will cancel the transaction, assumes ->del() is called
	 * for each successful ->add() during the transaction.
	 *
	 * Optional.
	 */
	void (*cancel_txn)		(struct pmu *pmu);

	/*
	 * Will return the value for perf_event_mmap_page::index for this event,
	 * if no implementation is provided it will default to: event->hw.idx + 1.
	 */
	int (*event_idx)		(struct perf_event *event); /*optional */

	/*
	 * context-switches callback
	 */
	void (*sched_task)		(struct perf_event_context *ctx,
					bool sched_in);

	/*
	 * Kmem cache of PMU specific data
	 */
	struct kmem_cache		*task_ctx_cache;

	/*
	 * PMU specific parts of task perf event context (i.e. ctx->task_ctx_data)
	 * can be synchronized using this function. See Intel LBR callstack support
	 * implementation and Perf core context switch handling callbacks for usage
	 * examples.
	 */
	void (*swap_task_ctx)		(struct perf_event_context *prev,
					 struct perf_event_context *next);
					/* optional */

	/*
	 * Set up pmu-private data structures for an AUX area
	 */
	void *(*setup_aux)		(struct perf_event *event, void **pages,
					 int nr_pages, bool overwrite);
					/* optional */

	/*
	 * Free pmu-private AUX data structures
	 */
	void (*free_aux)		(void *aux); /* optional */

	/*
	 * Take a snapshot of the AUX buffer without touching the event
	 * state, so that preempting ->start()/->stop() callbacks does
	 * not interfere with their logic. Called in PMI context.
	 *
	 * Returns the size of AUX data copied to the output handle.
	 *
	 * Optional.
	 */
	long (*snapshot_aux)		(struct perf_event *event,
					 struct perf_output_handle *handle,
					 unsigned long size);

	/*
	 * Validate address range filters: make sure the HW supports the
	 * requested configuration and number of filters; return 0 if the
	 * supplied filters are valid, -errno otherwise.
	 *
	 * Runs in the context of the ioctl()ing process and is not serialized
	 * with the rest of the PMU callbacks.
	 */
	int (*addr_filters_validate)	(struct list_head *filters);
					/* optional */

	/*
	 * Synchronize address range filter configuration:
	 * translate hw-agnostic filters into hardware configuration in
	 * event::hw::addr_filters.
	 *
	 * Runs as a part of filter sync sequence that is done in ->start()
	 * callback by calling perf_event_addr_filters_sync().
	 *
	 * May (and should) traverse event::addr_filters::list, for which its
	 * caller provides necessary serialization.
	 */
	void (*addr_filters_sync)	(struct perf_event *event);
					/* optional */

	/*
	 * Check if event can be used for aux_output purposes for
	 * events of this PMU.
	 *
	 * Runs from perf_event_open(). Should return 0 for "no match"
	 * or non-zero for "match".
	 */
	int (*aux_output_match)		(struct perf_event *event);
					/* optional */

	/*
	 * Filter events for PMU-specific reasons.
	 */
	int (*filter_match)		(struct perf_event *event); /* optional */

	/*
	 * Check period value for PERF_EVENT_IOC_PERIOD ioctl.
	 */
	int (*check_period)		(struct perf_event *event, u64 value); /* optional */
};

enum perf_addr_filter_action_t {
	PERF_ADDR_FILTER_ACTION_STOP = 0,
	PERF_ADDR_FILTER_ACTION_START,
	PERF_ADDR_FILTER_ACTION_FILTER,
};

/**
 * struct perf_addr_filter - address range filter definition
 * @entry:	event's filter list linkage
 * @path:	object file's path for file-based filters
 * @offset:	filter range offset
 * @size:	filter range size (size==0 means single address trigger)
 * @action:	filter/start/stop
 *
 * This is a hardware-agnostic filter configuration as specified by the user.
 */
struct perf_addr_filter {
	struct list_head	entry;
	struct path		path;
	unsigned long		offset;
	unsigned long		size;
	enum perf_addr_filter_action_t	action;
};

/**
 * struct perf_addr_filters_head - container for address range filters
 * @list:	list of filters for this event
 * @lock:	spinlock that serializes accesses to the @list and event's
 *		(and its children's) filter generations.
 * @nr_file_filters:	number of file-based filters
 *
 * A child event will use parent's @list (and therefore @lock), so they are
 * bundled together; see perf_event_addr_filters().
 */
struct perf_addr_filters_head {
	struct list_head	list;
	raw_spinlock_t		lock;
	unsigned int		nr_file_filters;
};

struct perf_addr_filter_range {
	unsigned long		start;
	unsigned long		size;
};

/**
 * enum perf_event_state - the states of an event:
 */
enum perf_event_state {
	PERF_EVENT_STATE_DEAD		= -4,
	PERF_EVENT_STATE_EXIT		= -3,
	PERF_EVENT_STATE_ERROR		= -2,
	PERF_EVENT_STATE_OFF		= -1,
	PERF_EVENT_STATE_INACTIVE	=  0,
	PERF_EVENT_STATE_ACTIVE		=  1,
};

struct file;
struct perf_sample_data;

typedef void (*perf_overflow_handler_t)(struct perf_event *,
					struct perf_sample_data *,
					struct pt_regs *regs);

/*
 * Event capabilities. For event_caps and groups caps.
 *
 * PERF_EV_CAP_SOFTWARE: Is a software event.
 * PERF_EV_CAP_READ_ACTIVE_PKG: A CPU event (or cgroup event) that can be read
 * from any CPU in the package where it is active.
 * PERF_EV_CAP_SIBLING: An event with this flag must be a group sibling and
 * cannot be a group leader. If an event with this flag is detached from the
 * group it is scheduled out and moved into an unrecoverable ERROR state.
 */
#define PERF_EV_CAP_SOFTWARE		BIT(0)
#define PERF_EV_CAP_READ_ACTIVE_PKG	BIT(1)
#define PERF_EV_CAP_SIBLING		BIT(2)

#define SWEVENT_HLIST_BITS		8
#define SWEVENT_HLIST_SIZE		(1 << SWEVENT_HLIST_BITS)

struct swevent_hlist {
	struct hlist_head		heads[SWEVENT_HLIST_SIZE];
	struct rcu_head			rcu_head;
};

#define PERF_ATTACH_CONTEXT	0x01
#define PERF_ATTACH_GROUP	0x02
#define PERF_ATTACH_TASK	0x04
#define PERF_ATTACH_TASK_DATA	0x08
#define PERF_ATTACH_ITRACE	0x10
#define PERF_ATTACH_SCHED_CB	0x20
#define PERF_ATTACH_CHILD	0x40

struct perf_cgroup;
struct perf_buffer;

struct pmu_event_list {
	raw_spinlock_t		lock;
	struct list_head	list;
};

#define for_each_sibling_event(sibling, event)			\
	if ((event)->group_leader == (event))			\
		list_for_each_entry((sibling), &(event)->sibling_list, sibling_list)

/**
 * struct perf_event - performance event kernel representation:
 */
struct perf_event {
#ifdef CONFIG_PERF_EVENTS
	/*
	 * entry onto perf_event_context::event_list;
	 *   modifications require ctx->lock
	 *   RCU safe iterations.
	 */
	struct list_head		event_entry;

	/*
	 * Locked for modification by both ctx->mutex and ctx->lock; holding
	 * either sufficies for read.
	 */
	struct list_head		sibling_list;
	struct list_head		active_list;
	/*
	 * Node on the pinned or flexible tree located at the event context;
	 */
	struct rb_node			group_node;
	u64				group_index;
	/*
	 * We need storage to track the entries in perf_pmu_migrate_context; we
	 * cannot use the event_entry because of RCU and we want to keep the
	 * group in tact which avoids us using the other two entries.
	 */
	struct list_head		migrate_entry;

	struct hlist_node		hlist_entry;
	struct list_head		active_entry;
	int				nr_siblings;

	/* Not serialized. Only written during event initialization. */
	int				event_caps;
	/* The cumulative AND of all event_caps for events in this group. */
	int				group_caps;

	struct perf_event		*group_leader;
	struct pmu			*pmu;
	void				*pmu_private;

	enum perf_event_state		state;
	unsigned int			attach_state;
	local64_t			count;
	atomic64_t			child_count;

	/*
	 * These are the total time in nanoseconds that the event
	 * has been enabled (i.e. eligible to run, and the task has
	 * been scheduled in, if this is a per-task event)
	 * and running (scheduled onto the CPU), respectively.
	 */
	u64				total_time_enabled;
	u64				total_time_running;
	u64				tstamp;

	/*
	 * timestamp shadows the actual context timing but it can
	 * be safely used in NMI interrupt context. It reflects the
	 * context time as it was when the event was last scheduled in.
	 *
	 * ctx_time already accounts for ctx->timestamp. Therefore to
	 * compute ctx_time for a sample, simply add perf_clock().
	 */
	u64				shadow_ctx_time;

	struct perf_event_attr		attr;
	u16				header_size;
	u16				id_header_size;
	u16				read_size;
	struct hw_perf_event		hw;

	struct perf_event_context	*ctx;
	atomic_long_t			refcount;

	/*
	 * These accumulate total time (in nanoseconds) that children
	 * events have been enabled and running, respectively.
	 */
	atomic64_t			child_total_time_enabled;
	atomic64_t			child_total_time_running;

	/*
	 * Protect attach/detach and child_list:
	 */
	struct mutex			child_mutex;
	struct list_head		child_list;
	struct perf_event		*parent;

	int				oncpu;
	int				cpu;

	struct list_head		owner_entry;
	struct task_struct		*owner;

	/* mmap bits */
	struct mutex			mmap_mutex;
	atomic_t			mmap_count;

	struct perf_buffer		*rb;
	struct list_head		rb_entry;
	unsigned long			rcu_batches;
	int				rcu_pending;

	/* poll related */
	wait_queue_head_t		waitq;
	struct fasync_struct		*fasync;

	/* delayed work for NMIs and such */
	int				pending_wakeup;
	int				pending_kill;
	int				pending_disable;
	unsigned long			pending_addr;	/* SIGTRAP */
	struct irq_work			pending;

	atomic_t			event_limit;

	/* address range filters */
	struct perf_addr_filters_head	addr_filters;
	/* vma address array for file-based filders */
	struct perf_addr_filter_range	*addr_filter_ranges;
	unsigned long			addr_filters_gen;

	/* for aux_output events */
	struct perf_event		*aux_event;

	void (*destroy)(struct perf_event *);
	struct rcu_head			rcu_head;

	struct pid_namespace		*ns;
	u64				id;

	u64				(*clock)(void);
	perf_overflow_handler_t		overflow_handler;
	void				*overflow_handler_context;
#ifdef CONFIG_BPF_SYSCALL
	perf_overflow_handler_t		orig_overflow_handler;
	struct bpf_prog			*prog;
#endif

#ifdef CONFIG_EVENT_TRACING
	struct trace_event_call		*tp_event;
	struct event_filter		*filter;
#ifdef CONFIG_FUNCTION_TRACER
	struct ftrace_ops               ftrace_ops;
#endif
#endif

#ifdef CONFIG_CGROUP_PERF
	struct perf_cgroup		*cgrp; /* cgroup event is attach to */
#endif

#ifdef CONFIG_SECURITY
	void *security;
#endif
	struct list_head		sb_list;
#endif /* CONFIG_PERF_EVENTS */
};


struct perf_event_groups {
	struct rb_root	tree;
	u64		index;
};

/**
 * struct perf_event_context - event context structure
 *
 * Used as a container for task events and CPU events as well:
 */
struct perf_event_context {
	struct pmu			*pmu;
	/*
	 * Protect the states of the events in the list,
	 * nr_active, and the list:
	 */
	raw_spinlock_t			lock;
	/*
	 * Protect the list of events.  Locking either mutex or lock
	 * is sufficient to ensure the list doesn't change; to change
	 * the list you need to lock both the mutex and the spinlock.
	 */
	struct mutex			mutex;

	struct list_head		active_ctx_list;
	struct perf_event_groups	pinned_groups;
	struct perf_event_groups	flexible_groups;
	struct list_head		event_list;

	struct list_head		pinned_active;
	struct list_head		flexible_active;

	int				nr_events;
	int				nr_active;
	int				is_active;
	int				nr_stat;
	int				nr_freq;
	int				rotate_disable;
	/*
	 * Set when nr_events != nr_active, except tolerant to events not
	 * necessary to be active due to scheduling constraints, such as cgroups.
	 */
	int				rotate_necessary;
	refcount_t			refcount;
	struct task_struct		*task;

	/*
	 * Context clock, runs when context enabled.
	 */
	u64				time;
	u64				timestamp;

	/*
	 * These fields let us detect when two contexts have both
	 * been cloned (inherited) from a common ancestor.
	 */
	struct perf_event_context	*parent_ctx;
	u64				parent_gen;
	u64				generation;
	int				pin_count;
#ifdef CONFIG_CGROUP_PERF
	int				nr_cgroups;	 /* cgroup evts */
#endif
	void				*task_ctx_data; /* pmu specific data */
	struct rcu_head			rcu_head;
};

/*
 * Number of contexts where an event can trigger:
 *	task, softirq, hardirq, nmi.
 */
#define PERF_NR_CONTEXTS	4

/**
 * struct perf_event_cpu_context - per cpu event context structure
 */
struct perf_cpu_context {
	struct perf_event_context	ctx;
	struct perf_event_context	*task_ctx;
	int				active_oncpu;
	int				exclusive;

	raw_spinlock_t			hrtimer_lock;
	struct hrtimer			hrtimer;
	ktime_t				hrtimer_interval;
	unsigned int			hrtimer_active;

#ifdef CONFIG_CGROUP_PERF
	struct perf_cgroup		*cgrp;
	struct list_head		cgrp_cpuctx_entry;
#endif

	struct list_head		sched_cb_entry;
	int				sched_cb_usage;

	int				online;
	/*
	 * Per-CPU storage for iterators used in visit_groups_merge. The default
	 * storage is of size 2 to hold the CPU and any CPU event iterators.
	 */
	int				heap_size;
	struct perf_event		**heap;
	struct perf_event		*heap_default[2];
};

struct perf_output_handle {
	struct perf_event		*event;
	struct perf_buffer		*rb;
	unsigned long			wakeup;
	unsigned long			size;
	u64				aux_flags;
	union {
		void			*addr;
		unsigned long		head;
	};
	int				page;
};

struct bpf_perf_event_data_kern {
	bpf_user_pt_regs_t *regs;
	struct perf_sample_data *data;
	struct perf_event *event;
};

#ifdef CONFIG_CGROUP_PERF

/*
 * perf_cgroup_info keeps track of time_enabled for a cgroup.
 * This is a per-cpu dynamically allocated data structure.
 */
struct perf_cgroup_info {
	u64				time;
	u64				timestamp;
};

struct perf_cgroup {
	struct cgroup_subsys_state	css;
	struct perf_cgroup_info	__percpu *info;
};

/*
 * Must ensure cgroup is pinned (css_get) before calling
 * this function. In other words, we cannot call this function
 * if there is no cgroup event for the current CPU context.
 */
static inline struct perf_cgroup *
perf_cgroup_from_task(struct task_struct *task, struct perf_event_context *ctx)
{
	return container_of(task_css_check(task, perf_event_cgrp_id,
					   ctx ? lockdep_is_held(&ctx->lock)
					       : true),
			    struct perf_cgroup, css);
}
#endif /* CONFIG_CGROUP_PERF */

#ifdef CONFIG_PERF_EVENTS

extern void *perf_aux_output_begin(struct perf_output_handle *handle,
				   struct perf_event *event);
extern void perf_aux_output_end(struct perf_output_handle *handle,
				unsigned long size);
extern int perf_aux_output_skip(struct perf_output_handle *handle,
				unsigned long size);
extern void *perf_get_aux(struct perf_output_handle *handle);
extern void perf_aux_output_flag(struct perf_output_handle *handle, u64 flags);
extern void perf_event_itrace_started(struct perf_event *event);

extern int perf_pmu_register(struct pmu *pmu, const char *name, int type);
extern void perf_pmu_unregister(struct pmu *pmu);

extern void __perf_event_task_sched_in(struct task_struct *prev,
				       struct task_struct *task);
extern void __perf_event_task_sched_out(struct task_struct *prev,
					struct task_struct *next);
extern int perf_event_init_task(struct task_struct *child, u64 clone_flags);
extern void perf_event_exit_task(struct task_struct *child);
extern void perf_event_free_task(struct task_struct *task);
extern void perf_event_delayed_put(struct task_struct *task);
extern struct file *perf_event_get(unsigned int fd);
extern const struct perf_event *perf_get_event(struct file *file);
extern const struct perf_event_attr *perf_event_attrs(struct perf_event *event);
extern void perf_event_print_debug(void);
extern void perf_pmu_disable(struct pmu *pmu);
extern void perf_pmu_enable(struct pmu *pmu);
extern void perf_sched_cb_dec(struct pmu *pmu);
extern void perf_sched_cb_inc(struct pmu *pmu);
extern int perf_event_task_disable(void);
extern int perf_event_task_enable(void);

extern void perf_pmu_resched(struct pmu *pmu);

extern int perf_event_refresh(struct perf_event *event, int refresh);
extern void perf_event_update_userpage(struct perf_event *event);
extern int perf_event_release_kernel(struct perf_event *event);
extern struct perf_event *
perf_event_create_kernel_counter(struct perf_event_attr *attr,
				int cpu,
				struct task_struct *task,
				perf_overflow_handler_t callback,
				void *context);
extern void perf_pmu_migrate_context(struct pmu *pmu,
				int src_cpu, int dst_cpu);
int perf_event_read_local(struct perf_event *event, u64 *value,
			  u64 *enabled, u64 *running);
extern u64 perf_event_read_value(struct perf_event *event,
				 u64 *enabled, u64 *running);


struct perf_sample_data {
	/*
	 * Fields set by perf_sample_data_init(), group so as to
	 * minimize the cachelines touched.
	 */
	u64				addr;
	struct perf_raw_record		*raw;
	struct perf_branch_stack	*br_stack;
	u64				period;
	union perf_sample_weight	weight;
	u64				txn;
	union  perf_mem_data_src	data_src;

	/*
	 * The other fields, optionally {set,used} by
	 * perf_{prepare,output}_sample().
	 */
	u64				type;
	u64				ip;
	struct {
		u32	pid;
		u32	tid;
	}				tid_entry;
	u64				time;
	u64				id;
	u64				stream_id;
	struct {
		u32	cpu;
		u32	reserved;
	}				cpu_entry;
	struct perf_callchain_entry	*callchain;
	u64				aux_size;

	struct perf_regs		regs_user;
	struct perf_regs		regs_intr;
	u64				stack_user_size;

	u64				phys_addr;
	u64				cgroup;
	u64				data_page_size;
	u64				code_page_size;
} ____cacheline_aligned;

/* default value for data source */
#define PERF_MEM_NA (PERF_MEM_S(OP, NA)   |\
		    PERF_MEM_S(LVL, NA)   |\
		    PERF_MEM_S(SNOOP, NA) |\
		    PERF_MEM_S(LOCK, NA)  |\
		    PERF_MEM_S(TLB, NA))

static inline void perf_sample_data_init(struct perf_sample_data *data,
					 u64 addr, u64 period)
{
	/* remaining struct members initialized in perf_prepare_sample() */
	data->addr = addr;
	data->raw  = NULL;
	data->br_stack = NULL;
	data->period = period;
	data->weight.full = 0;
	data->data_src.val = PERF_MEM_NA;
	data->txn = 0;
}

extern void perf_output_sample(struct perf_output_handle *handle,
			       struct perf_event_header *header,
			       struct perf_sample_data *data,
			       struct perf_event *event);
extern void perf_prepare_sample(struct perf_event_header *header,
				struct perf_sample_data *data,
				struct perf_event *event,
				struct pt_regs *regs);

extern int perf_event_overflow(struct perf_event *event,
				 struct perf_sample_data *data,
				 struct pt_regs *regs);

extern void perf_event_output_forward(struct perf_event *event,
				     struct perf_sample_data *data,
				     struct pt_regs *regs);
extern void perf_event_output_backward(struct perf_event *event,
				       struct perf_sample_data *data,
				       struct pt_regs *regs);
extern int perf_event_output(struct perf_event *event,
			     struct perf_sample_data *data,
			     struct pt_regs *regs);

static inline bool
is_default_overflow_handler(struct perf_event *event)
{
	if (likely(event->overflow_handler == perf_event_output_forward))
		return true;
	if (unlikely(event->overflow_handler == perf_event_output_backward))
		return true;
	return false;
}

extern void
perf_event_header__init_id(struct perf_event_header *header,
			   struct perf_sample_data *data,
			   struct perf_event *event);
extern void
perf_event__output_id_sample(struct perf_event *event,
			     struct perf_output_handle *handle,
			     struct perf_sample_data *sample);

extern void
perf_log_lost_samples(struct perf_event *event, u64 lost);

static inline bool event_has_any_exclude_flag(struct perf_event *event)
{
	struct perf_event_attr *attr = &event->attr;

	return attr->exclude_idle || attr->exclude_user ||
	       attr->exclude_kernel || attr->exclude_hv ||
	       attr->exclude_guest || attr->exclude_host;
}

static inline bool is_sampling_event(struct perf_event *event)
{
	return event->attr.sample_period != 0;
}

/*
 * Return 1 for a software event, 0 for a hardware event
 */
static inline int is_software_event(struct perf_event *event)
{
	return event->event_caps & PERF_EV_CAP_SOFTWARE;
}

/*
 * Return 1 for event in sw context, 0 for event in hw context
 */
static inline int in_software_context(struct perf_event *event)
{
	return event->ctx->pmu->task_ctx_nr == perf_sw_context;
}

static inline int is_exclusive_pmu(struct pmu *pmu)
{
	return pmu->capabilities & PERF_PMU_CAP_EXCLUSIVE;
}

extern struct static_key perf_swevent_enabled[PERF_COUNT_SW_MAX];

extern void ___perf_sw_event(u32, u64, struct pt_regs *, u64);
extern void __perf_sw_event(u32, u64, struct pt_regs *, u64);

#ifndef perf_arch_fetch_caller_regs
static inline void perf_arch_fetch_caller_regs(struct pt_regs *regs, unsigned long ip) { }
#endif

/*
 * When generating a perf sample in-line, instead of from an interrupt /
 * exception, we lack a pt_regs. This is typically used from software events
 * like: SW_CONTEXT_SWITCHES, SW_MIGRATIONS and the tie-in with tracepoints.
 *
 * We typically don't need a full set, but (for x86) do require:
 * - ip for PERF_SAMPLE_IP
 * - cs for user_mode() tests
 * - sp for PERF_SAMPLE_CALLCHAIN
 * - eflags for MISC bits and CALLCHAIN (see: perf_hw_regs())
 *
 * NOTE: assumes @regs is otherwise already 0 filled; this is important for
 * things like PERF_SAMPLE_REGS_INTR.
 */
static inline void perf_fetch_caller_regs(struct pt_regs *regs)
{
	perf_arch_fetch_caller_regs(regs, CALLER_ADDR0);
}

static __always_inline void
perf_sw_event(u32 event_id, u64 nr, struct pt_regs *regs, u64 addr)
{
	if (static_key_false(&perf_swevent_enabled[event_id]))
		__perf_sw_event(event_id, nr, regs, addr);
}

DECLARE_PER_CPU(struct pt_regs, __perf_regs[4]);

/*
 * 'Special' version for the scheduler, it hard assumes no recursion,
 * which is guaranteed by us not actually scheduling inside other swevents
 * because those disable preemption.
 */
static __always_inline void __perf_sw_event_sched(u32 event_id, u64 nr, u64 addr)
{
	struct pt_regs *regs = this_cpu_ptr(&__perf_regs[0]);

	perf_fetch_caller_regs(regs);
	___perf_sw_event(event_id, nr, regs, addr);
}

extern struct static_key_false perf_sched_events;

static __always_inline bool __perf_sw_enabled(int swevt)
{
	return static_key_false(&perf_swevent_enabled[swevt]);
}

static inline void perf_event_task_migrate(struct task_struct *task)
{
	if (__perf_sw_enabled(PERF_COUNT_SW_CPU_MIGRATIONS))
		task->sched_migrated = 1;
}

static inline void perf_event_task_sched_in(struct task_struct *prev,
					    struct task_struct *task)
{
	if (static_branch_unlikely(&perf_sched_events))
		__perf_event_task_sched_in(prev, task);

	if (__perf_sw_enabled(PERF_COUNT_SW_CPU_MIGRATIONS) &&
	    task->sched_migrated) {
		__perf_sw_event_sched(PERF_COUNT_SW_CPU_MIGRATIONS, 1, 0);
		task->sched_migrated = 0;
	}
}

static inline void perf_event_task_sched_out(struct task_struct *prev,
					     struct task_struct *next)
{
	if (__perf_sw_enabled(PERF_COUNT_SW_CONTEXT_SWITCHES))
		__perf_sw_event_sched(PERF_COUNT_SW_CONTEXT_SWITCHES, 1, 0);

#ifdef CONFIG_CGROUP_PERF
	if (__perf_sw_enabled(PERF_COUNT_SW_CGROUP_SWITCHES) &&
	    perf_cgroup_from_task(prev, NULL) !=
	    perf_cgroup_from_task(next, NULL))
		__perf_sw_event_sched(PERF_COUNT_SW_CGROUP_SWITCHES, 1, 0);
#endif

	if (static_branch_unlikely(&perf_sched_events))
		__perf_event_task_sched_out(prev, next);
}

extern void perf_event_mmap(struct vm_area_struct *vma);

extern void perf_event_ksymbol(u16 ksym_type, u64 addr, u32 len,
			       bool unregister, const char *sym);
extern void perf_event_bpf_event(struct bpf_prog *prog,
				 enum perf_bpf_event_type type,
				 u16 flags);

extern struct perf_guest_info_callbacks *perf_guest_cbs;
extern int perf_register_guest_info_callbacks(struct perf_guest_info_callbacks *callbacks);
extern int perf_unregister_guest_info_callbacks(struct perf_guest_info_callbacks *callbacks);

extern void perf_event_exec(void);
extern void perf_event_comm(struct task_struct *tsk, bool exec);
extern void perf_event_namespaces(struct task_struct *tsk);
extern void perf_event_fork(struct task_struct *tsk);
extern void perf_event_text_poke(const void *addr,
				 const void *old_bytes, size_t old_len,
				 const void *new_bytes, size_t new_len);

/* Callchains */
DECLARE_PER_CPU(struct perf_callchain_entry, perf_callchain_entry);

extern void perf_callchain_user(struct perf_callchain_entry_ctx *entry, struct pt_regs *regs);
extern void perf_callchain_kernel(struct perf_callchain_entry_ctx *entry, struct pt_regs *regs);
extern struct perf_callchain_entry *
get_perf_callchain(struct pt_regs *regs, u32 init_nr, bool kernel, bool user,
		   u32 max_stack, bool crosstask, bool add_mark);
extern struct perf_callchain_entry *perf_callchain(struct perf_event *event, struct pt_regs *regs);
extern int get_callchain_buffers(int max_stack);
extern void put_callchain_buffers(void);
extern struct perf_callchain_entry *get_callchain_entry(int *rctx);
extern void put_callchain_entry(int rctx);

extern int sysctl_perf_event_max_stack;
extern int sysctl_perf_event_max_contexts_per_stack;

static inline int perf_callchain_store_context(struct perf_callchain_entry_ctx *ctx, u64 ip)
{
	if (ctx->contexts < sysctl_perf_event_max_contexts_per_stack) {
		struct perf_callchain_entry *entry = ctx->entry;
		entry->ip[entry->nr++] = ip;
		++ctx->contexts;
		return 0;
	} else {
		ctx->contexts_maxed = true;
		return -1; /* no more room, stop walking the stack */
	}
}

static inline int perf_callchain_store(struct perf_callchain_entry_ctx *ctx, u64 ip)
{
	if (ctx->nr < ctx->max_stack && !ctx->contexts_maxed) {
		struct perf_callchain_entry *entry = ctx->entry;
		entry->ip[entry->nr++] = ip;
		++ctx->nr;
		return 0;
	} else {
		return -1; /* no more room, stop walking the stack */
	}
}

extern int sysctl_perf_event_paranoid;
extern int sysctl_perf_event_mlock;
extern int sysctl_perf_event_sample_rate;
extern int sysctl_perf_cpu_time_max_percent;

extern void perf_sample_event_took(u64 sample_len_ns);

int perf_proc_update_handler(struct ctl_table *table, int write,
		void *buffer, size_t *lenp, loff_t *ppos);
int perf_cpu_time_max_percent_handler(struct ctl_table *table, int write,
		void *buffer, size_t *lenp, loff_t *ppos);
int perf_event_max_stack_handler(struct ctl_table *table, int write,
		void *buffer, size_t *lenp, loff_t *ppos);

/* Access to perf_event_open(2) syscall. */
#define PERF_SECURITY_OPEN		0

/* Finer grained perf_event_open(2) access control. */
#define PERF_SECURITY_CPU		1
#define PERF_SECURITY_KERNEL		2
#define PERF_SECURITY_TRACEPOINT	3

static inline int perf_is_paranoid(void)
{
	return sysctl_perf_event_paranoid > -1;
}

static inline int perf_allow_kernel(struct perf_event_attr *attr)
{
	if (sysctl_perf_event_paranoid > 1 && !perfmon_capable())
		return -EACCES;

	return security_perf_event_open(attr, PERF_SECURITY_KERNEL);
}

static inline int perf_allow_cpu(struct perf_event_attr *attr)
{
	if (sysctl_perf_event_paranoid > 0 && !perfmon_capable())
		return -EACCES;

	return security_perf_event_open(attr, PERF_SECURITY_CPU);
}

static inline int perf_allow_tracepoint(struct perf_event_attr *attr)
{
	if (sysctl_perf_event_paranoid > -1 && !perfmon_capable())
		return -EPERM;

	return security_perf_event_open(attr, PERF_SECURITY_TRACEPOINT);
}

extern void perf_event_init(void);
extern void perf_tp_event(u16 event_type, u64 count, void *record,
			  int entry_size, struct pt_regs *regs,
			  struct hlist_head *head, int rctx,
			  struct task_struct *task);
extern void perf_bp_event(struct perf_event *event, void *data);

#ifndef perf_misc_flags
# define perf_misc_flags(regs) \
		(user_mode(regs) ? PERF_RECORD_MISC_USER : PERF_RECORD_MISC_KERNEL)
# define perf_instruction_pointer(regs)	instruction_pointer(regs)
#endif
#ifndef perf_arch_bpf_user_pt_regs
# define perf_arch_bpf_user_pt_regs(regs) regs
#endif

static inline bool has_branch_stack(struct perf_event *event)
{
	return event->attr.sample_type & PERF_SAMPLE_BRANCH_STACK;
}

static inline bool needs_branch_stack(struct perf_event *event)
{
	return event->attr.branch_sample_type != 0;
}

static inline bool has_aux(struct perf_event *event)
{
	return event->pmu->setup_aux;
}

static inline bool is_write_backward(struct perf_event *event)
{
	return !!event->attr.write_backward;
}

static inline bool has_addr_filter(struct perf_event *event)
{
	return event->pmu->nr_addr_filters;
}

/*
 * An inherited event uses parent's filters
 */
static inline struct perf_addr_filters_head *
perf_event_addr_filters(struct perf_event *event)
{
	struct perf_addr_filters_head *ifh = &event->addr_filters;

	if (event->parent)
		ifh = &event->parent->addr_filters;

	return ifh;
}

extern void perf_event_addr_filters_sync(struct perf_event *event);

extern int perf_output_begin(struct perf_output_handle *handle,
			     struct perf_sample_data *data,
			     struct perf_event *event, unsigned int size);
extern int perf_output_begin_forward(struct perf_output_handle *handle,
				     struct perf_sample_data *data,
				     struct perf_event *event,
				     unsigned int size);
extern int perf_output_begin_backward(struct perf_output_handle *handle,
				      struct perf_sample_data *data,
				      struct perf_event *event,
				      unsigned int size);

extern void perf_output_end(struct perf_output_handle *handle);
extern unsigned int perf_output_copy(struct perf_output_handle *handle,
			     const void *buf, unsigned int len);
extern unsigned int perf_output_skip(struct perf_output_handle *handle,
				     unsigned int len);
extern long perf_output_copy_aux(struct perf_output_handle *aux_handle,
				 struct perf_output_handle *handle,
				 unsigned long from, unsigned long to);
extern int perf_swevent_get_recursion_context(void);
extern void perf_swevent_put_recursion_context(int rctx);
extern u64 perf_swevent_set_period(struct perf_event *event);
extern void perf_event_enable(struct perf_event *event);
extern void perf_event_disable(struct perf_event *event);
extern void perf_event_disable_local(struct perf_event *event);
extern void perf_event_disable_inatomic(struct perf_event *event);
extern void perf_event_task_tick(void);
extern int perf_event_account_interrupt(struct perf_event *event);
extern int perf_event_period(struct perf_event *event, u64 value);
extern u64 perf_event_pause(struct perf_event *event, bool reset);
#else /* !CONFIG_PERF_EVENTS: */
static inline void *
perf_aux_output_begin(struct perf_output_handle *handle,
		      struct perf_event *event)				{ return NULL; }
static inline void
perf_aux_output_end(struct perf_output_handle *handle, unsigned long size)
									{ }
static inline int
perf_aux_output_skip(struct perf_output_handle *handle,
		     unsigned long size)				{ return -EINVAL; }
static inline void *
perf_get_aux(struct perf_output_handle *handle)				{ return NULL; }
static inline void
perf_event_task_migrate(struct task_struct *task)			{ }
static inline void
perf_event_task_sched_in(struct task_struct *prev,
			 struct task_struct *task)			{ }
static inline void
perf_event_task_sched_out(struct task_struct *prev,
			  struct task_struct *next)			{ }
static inline int perf_event_init_task(struct task_struct *child,
				       u64 clone_flags)			{ return 0; }
static inline void perf_event_exit_task(struct task_struct *child)	{ }
static inline void perf_event_free_task(struct task_struct *task)	{ }
static inline void perf_event_delayed_put(struct task_struct *task)	{ }
static inline struct file *perf_event_get(unsigned int fd)	{ return ERR_PTR(-EINVAL); }
static inline const struct perf_event *perf_get_event(struct file *file)
{
	return ERR_PTR(-EINVAL);
}
static inline const struct perf_event_attr *perf_event_attrs(struct perf_event *event)
{
	return ERR_PTR(-EINVAL);
}
static inline int perf_event_read_local(struct perf_event *event, u64 *value,
					u64 *enabled, u64 *running)
{
	return -EINVAL;
}
static inline void perf_event_print_debug(void)				{ }
static inline int perf_event_task_disable(void)				{ return -EINVAL; }
static inline int perf_event_task_enable(void)				{ return -EINVAL; }
static inline int perf_event_refresh(struct perf_event *event, int refresh)
{
	return -EINVAL;
}

static inline void
perf_sw_event(u32 event_id, u64 nr, struct pt_regs *regs, u64 addr)	{ }
static inline void
perf_bp_event(struct perf_event *event, void *data)			{ }

static inline int perf_register_guest_info_callbacks
(struct perf_guest_info_callbacks *callbacks)				{ return 0; }
static inline int perf_unregister_guest_info_callbacks
(struct perf_guest_info_callbacks *callbacks)				{ return 0; }

static inline void perf_event_mmap(struct vm_area_struct *vma)		{ }

typedef int (perf_ksymbol_get_name_f)(char *name, int name_len, void *data);
static inline void perf_event_ksymbol(u16 ksym_type, u64 addr, u32 len,
				      bool unregister, const char *sym)	{ }
static inline void perf_event_bpf_event(struct bpf_prog *prog,
					enum perf_bpf_event_type type,
					u16 flags)			{ }
static inline void perf_event_exec(void)				{ }
static inline void perf_event_comm(struct task_struct *tsk, bool exec)	{ }
static inline void perf_event_namespaces(struct task_struct *tsk)	{ }
static inline void perf_event_fork(struct task_struct *tsk)		{ }
static inline void perf_event_text_poke(const void *addr,
					const void *old_bytes,
					size_t old_len,
					const void *new_bytes,
					size_t new_len)			{ }
static inline void perf_event_init(void)				{ }
static inline int  perf_swevent_get_recursion_context(void)		{ return -1; }
static inline void perf_swevent_put_recursion_context(int rctx)		{ }
static inline u64 perf_swevent_set_period(struct perf_event *event)	{ return 0; }
static inline void perf_event_enable(struct perf_event *event)		{ }
static inline void perf_event_disable(struct perf_event *event)		{ }
static inline int __perf_event_disable(void *info)			{ return -1; }
static inline void perf_event_task_tick(void)				{ }
static inline int perf_event_release_kernel(struct perf_event *event)	{ return 0; }
static inline int perf_event_period(struct perf_event *event, u64 value)
{
	return -EINVAL;
}
static inline u64 perf_event_pause(struct perf_event *event, bool reset)
{
	return 0;
}
#endif

#if defined(CONFIG_PERF_EVENTS) && defined(CONFIG_CPU_SUP_INTEL)
extern void perf_restore_debug_store(void);
#else
static inline void perf_restore_debug_store(void)			{ }
#endif

static __always_inline bool perf_raw_frag_last(const struct perf_raw_frag *frag)
{
	return frag->pad < sizeof(u64);
}

#define perf_output_put(handle, x) perf_output_copy((handle), &(x), sizeof(x))

struct perf_pmu_events_attr {
	struct device_attribute attr;
	u64 id;
	const char *event_str;
};

struct perf_pmu_events_ht_attr {
	struct device_attribute			attr;
	u64					id;
	const char				*event_str_ht;
	const char				*event_str_noht;
};

struct perf_pmu_events_hybrid_attr {
	struct device_attribute			attr;
	u64					id;
	const char				*event_str;
	u64					pmu_type;
};

struct perf_pmu_format_hybrid_attr {
	struct device_attribute			attr;
	u64					pmu_type;
};

ssize_t perf_event_sysfs_show(struct device *dev, struct device_attribute *attr,
			      char *page);

#define PMU_EVENT_ATTR(_name, _var, _id, _show)				\
static struct perf_pmu_events_attr _var = {				\
	.attr = __ATTR(_name, 0444, _show, NULL),			\
	.id   =  _id,							\
};

#define PMU_EVENT_ATTR_STRING(_name, _var, _str)			    \
static struct perf_pmu_events_attr _var = {				    \
	.attr		= __ATTR(_name, 0444, perf_event_sysfs_show, NULL), \
	.id		= 0,						    \
	.event_str	= _str,						    \
};

#define PMU_FORMAT_ATTR(_name, _format)					\
static ssize_t								\
_name##_show(struct device *dev,					\
			       struct device_attribute *attr,		\
			       char *page)				\
{									\
	BUILD_BUG_ON(sizeof(_format) >= PAGE_SIZE);			\
	return sprintf(page, _format "\n");				\
}									\
									\
static struct device_attribute format_attr_##_name = __ATTR_RO(_name)

/* Performance counter hotplug functions */
#ifdef CONFIG_PERF_EVENTS
int perf_event_init_cpu(unsigned int cpu);
int perf_event_exit_cpu(unsigned int cpu);
#else
#define perf_event_init_cpu	NULL
#define perf_event_exit_cpu	NULL
#endif

extern void __weak arch_perf_update_userpage(struct perf_event *event,
					     struct perf_event_mmap_page *userpg,
					     u64 now);

#ifdef CONFIG_MMU
extern __weak u64 arch_perf_get_page_size(struct mm_struct *mm, unsigned long addr);
#endif

#endif /* _LINUX_PERF_EVENT_H */
