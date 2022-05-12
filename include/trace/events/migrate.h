/* SPDX-License-Identifier: GPL-2.0 */
#undef TRACE_SYSTEM
#define TRACE_SYSTEM migrate

#if !defined(_TRACE_MIGRATE_H) || defined(TRACE_HEADER_MULTI_READ)
#define _TRACE_MIGRATE_H

#include <linux/tracepoint.h>

#define MIGRATE_MODE						\
	EM( MIGRATE_ASYNC,	"MIGRATE_ASYNC")		\
	EM( MIGRATE_SYNC_LIGHT,	"MIGRATE_SYNC_LIGHT")		\
	EMe(MIGRATE_SYNC,	"MIGRATE_SYNC")


#define MIGRATE_REASON						\
	EM( MR_COMPACTION,	"compaction")			\
	EM( MR_MEMORY_FAILURE,	"memory_failure")		\
	EM( MR_MEMORY_HOTPLUG,	"memory_hotplug")		\
	EM( MR_SYSCALL,		"syscall_or_cpuset")		\
	EM( MR_MEMPOLICY_MBIND,	"mempolicy_mbind")		\
	EM( MR_NUMA_MISPLACED,	"numa_misplaced")		\
	EM( MR_CONTIG_RANGE,	"contig_range")			\
	EMe(MR_LONGTERM_PIN,	"longterm_pin")

/*
 * First define the enums in the above macros to be exported to userspace
 * via TRACE_DEFINE_ENUM().
 */
#undef EM
#undef EMe
#define EM(a, b)	TRACE_DEFINE_ENUM(a);
#define EMe(a, b)	TRACE_DEFINE_ENUM(a);

MIGRATE_MODE
MIGRATE_REASON

/*
 * Now redefine the EM() and EMe() macros to map the enums to the strings
 * that will be printed in the output.
 */
#undef EM
#undef EMe
#define EM(a, b)	{a, b},
#define EMe(a, b)	{a, b}

TRACE_EVENT(mm_migrate_pages,

	TP_PROTO(unsigned long succeeded, unsigned long failed,
		 unsigned long thp_succeeded, unsigned long thp_failed,
		 unsigned long thp_split, enum migrate_mode mode, int reason),

	TP_ARGS(succeeded, failed, thp_succeeded, thp_failed,
		thp_split, mode, reason),

	TP_STRUCT__entry(
		__field(	unsigned long,		succeeded)
		__field(	unsigned long,		failed)
		__field(	unsigned long,		thp_succeeded)
		__field(	unsigned long,		thp_failed)
		__field(	unsigned long,		thp_split)
		__field(	enum migrate_mode,	mode)
		__field(	int,			reason)
	),

	TP_fast_assign(
		__entry->succeeded	= succeeded;
		__entry->failed		= failed;
		__entry->thp_succeeded	= thp_succeeded;
		__entry->thp_failed	= thp_failed;
		__entry->thp_split	= thp_split;
		__entry->mode		= mode;
		__entry->reason		= reason;
	),

	TP_printk("nr_succeeded=%lu nr_failed=%lu nr_thp_succeeded=%lu nr_thp_failed=%lu nr_thp_split=%lu mode=%s reason=%s",
		__entry->succeeded,
		__entry->failed,
		__entry->thp_succeeded,
		__entry->thp_failed,
		__entry->thp_split,
		__print_symbolic(__entry->mode, MIGRATE_MODE),
		__print_symbolic(__entry->reason, MIGRATE_REASON))
);

TRACE_EVENT(mm_migrate_pages_start,

	TP_PROTO(enum migrate_mode mode, int reason),

	TP_ARGS(mode, reason),

	TP_STRUCT__entry(
		__field(enum migrate_mode, mode)
		__field(int, reason)
	),

	TP_fast_assign(
		__entry->mode	= mode;
		__entry->reason	= reason;
	),

	TP_printk("mode=%s reason=%s",
		  __print_symbolic(__entry->mode, MIGRATE_MODE),
		  __print_symbolic(__entry->reason, MIGRATE_REASON))
);

#endif /* _TRACE_MIGRATE_H */

/* This part must be outside protection */
#include <trace/define_trace.h>
