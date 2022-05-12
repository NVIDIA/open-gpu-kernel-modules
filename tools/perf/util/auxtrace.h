/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * auxtrace.h: AUX area trace support
 * Copyright (c) 2013-2015, Intel Corporation.
 */

#ifndef __PERF_AUXTRACE_H
#define __PERF_AUXTRACE_H

#include <sys/types.h>
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h> // FILE
#include <linux/list.h>
#include <linux/perf_event.h>
#include <linux/types.h>
#include <asm/bitsperlong.h>
#include <asm/barrier.h>

union perf_event;
struct perf_session;
struct evlist;
struct evsel;
struct perf_tool;
struct mmap;
struct perf_sample;
struct option;
struct record_opts;
struct perf_record_auxtrace_error;
struct perf_record_auxtrace_info;
struct events_stats;
struct perf_pmu;

enum auxtrace_error_type {
       PERF_AUXTRACE_ERROR_ITRACE  = 1,
       PERF_AUXTRACE_ERROR_MAX
};

/* Auxtrace records must have the same alignment as perf event records */
#define PERF_AUXTRACE_RECORD_ALIGNMENT 8

enum auxtrace_type {
	PERF_AUXTRACE_UNKNOWN,
	PERF_AUXTRACE_INTEL_PT,
	PERF_AUXTRACE_INTEL_BTS,
	PERF_AUXTRACE_CS_ETM,
	PERF_AUXTRACE_ARM_SPE,
	PERF_AUXTRACE_S390_CPUMSF,
};

enum itrace_period_type {
	PERF_ITRACE_PERIOD_INSTRUCTIONS,
	PERF_ITRACE_PERIOD_TICKS,
	PERF_ITRACE_PERIOD_NANOSECS,
};

#define AUXTRACE_ERR_FLG_OVERFLOW	(1 << ('o' - 'a'))
#define AUXTRACE_ERR_FLG_DATA_LOST	(1 << ('l' - 'a'))

#define AUXTRACE_LOG_FLG_ALL_PERF_EVTS	(1 << ('a' - 'a'))

/**
 * struct itrace_synth_opts - AUX area tracing synthesis options.
 * @set: indicates whether or not options have been set
 * @default_no_sample: Default to no sampling.
 * @inject: indicates the event (not just the sample) must be fully synthesized
 *          because 'perf inject' will write it out
 * @instructions: whether to synthesize 'instructions' events
 * @branches: whether to synthesize 'branches' events
 *            (branch misses only for Arm SPE)
 * @transactions: whether to synthesize events for transactions
 * @ptwrites: whether to synthesize events for ptwrites
 * @pwr_events: whether to synthesize power events
 * @other_events: whether to synthesize other events recorded due to the use of
 *                aux_output
 * @errors: whether to synthesize decoder error events
 * @dont_decode: whether to skip decoding entirely
 * @log: write a decoding log
 * @calls: limit branch samples to calls (can be combined with @returns)
 * @returns: limit branch samples to returns (can be combined with @calls)
 * @callchain: add callchain to 'instructions' events
 * @add_callchain: add callchain to existing event records
 * @thread_stack: feed branches to the thread_stack
 * @last_branch: add branch context to 'instruction' events
 * @add_last_branch: add branch context to existing event records
 * @flc: whether to synthesize first level cache events
 * @llc: whether to synthesize last level cache events
 * @tlb: whether to synthesize TLB events
 * @remote_access: whether to synthesize remote access events
 * @mem: whether to synthesize memory events
 * @callchain_sz: maximum callchain size
 * @last_branch_sz: branch context size
 * @period: 'instructions' events period
 * @period_type: 'instructions' events period type
 * @initial_skip: skip N events at the beginning.
 * @cpu_bitmap: CPUs for which to synthesize events, or NULL for all
 * @ptime_range: time intervals to trace or NULL
 * @range_num: number of time intervals to trace
 * @error_plus_flags: flags to affect what errors are reported
 * @error_minus_flags: flags to affect what errors are reported
 * @log_plus_flags: flags to affect what is logged
 * @log_minus_flags: flags to affect what is logged
 * @quick: quicker (less detailed) decoding
 */
struct itrace_synth_opts {
	bool			set;
	bool			default_no_sample;
	bool			inject;
	bool			instructions;
	bool			branches;
	bool			transactions;
	bool			ptwrites;
	bool			pwr_events;
	bool			other_events;
	bool			errors;
	bool			dont_decode;
	bool			log;
	bool			calls;
	bool			returns;
	bool			callchain;
	bool			add_callchain;
	bool			thread_stack;
	bool			last_branch;
	bool			add_last_branch;
	bool			flc;
	bool			llc;
	bool			tlb;
	bool			remote_access;
	bool			mem;
	unsigned int		callchain_sz;
	unsigned int		last_branch_sz;
	unsigned long long	period;
	enum itrace_period_type	period_type;
	unsigned long		initial_skip;
	unsigned long		*cpu_bitmap;
	struct perf_time_interval *ptime_range;
	int			range_num;
	unsigned int		error_plus_flags;
	unsigned int		error_minus_flags;
	unsigned int		log_plus_flags;
	unsigned int		log_minus_flags;
	unsigned int		quick;
};

/**
 * struct auxtrace_index_entry - indexes a AUX area tracing event within a
 *                               perf.data file.
 * @file_offset: offset within the perf.data file
 * @sz: size of the event
 */
struct auxtrace_index_entry {
	u64			file_offset;
	u64			sz;
};

#define PERF_AUXTRACE_INDEX_ENTRY_COUNT 256

/**
 * struct auxtrace_index - index of AUX area tracing events within a perf.data
 *                         file.
 * @list: linking a number of arrays of entries
 * @nr: number of entries
 * @entries: array of entries
 */
struct auxtrace_index {
	struct list_head	list;
	size_t			nr;
	struct auxtrace_index_entry entries[PERF_AUXTRACE_INDEX_ENTRY_COUNT];
};

/**
 * struct auxtrace - session callbacks to allow AUX area data decoding.
 * @process_event: lets the decoder see all session events
 * @process_auxtrace_event: process a PERF_RECORD_AUXTRACE event
 * @queue_data: queue an AUX sample or PERF_RECORD_AUXTRACE event for later
 *              processing
 * @dump_auxtrace_sample: dump AUX area sample data
 * @flush_events: process any remaining data
 * @free_events: free resources associated with event processing
 * @free: free resources associated with the session
 */
struct auxtrace {
	int (*process_event)(struct perf_session *session,
			     union perf_event *event,
			     struct perf_sample *sample,
			     struct perf_tool *tool);
	int (*process_auxtrace_event)(struct perf_session *session,
				      union perf_event *event,
				      struct perf_tool *tool);
	int (*queue_data)(struct perf_session *session,
			  struct perf_sample *sample, union perf_event *event,
			  u64 data_offset);
	void (*dump_auxtrace_sample)(struct perf_session *session,
				     struct perf_sample *sample);
	int (*flush_events)(struct perf_session *session,
			    struct perf_tool *tool);
	void (*free_events)(struct perf_session *session);
	void (*free)(struct perf_session *session);
	bool (*evsel_is_auxtrace)(struct perf_session *session,
				  struct evsel *evsel);
};

/**
 * struct auxtrace_buffer - a buffer containing AUX area tracing data.
 * @list: buffers are queued in a list held by struct auxtrace_queue
 * @size: size of the buffer in bytes
 * @pid: in per-thread mode, the pid this buffer is associated with
 * @tid: in per-thread mode, the tid this buffer is associated with
 * @cpu: in per-cpu mode, the cpu this buffer is associated with
 * @data: actual buffer data (can be null if the data has not been loaded)
 * @data_offset: file offset at which the buffer can be read
 * @mmap_addr: mmap address at which the buffer can be read
 * @mmap_size: size of the mmap at @mmap_addr
 * @data_needs_freeing: @data was malloc'd so free it when it is no longer
 *                      needed
 * @consecutive: the original data was split up and this buffer is consecutive
 *               to the previous buffer
 * @offset: offset as determined by aux_head / aux_tail members of struct
 *          perf_event_mmap_page
 * @reference: an implementation-specific reference determined when the data is
 *             recorded
 * @buffer_nr: used to number each buffer
 * @use_size: implementation actually only uses this number of bytes
 * @use_data: implementation actually only uses data starting at this address
 */
struct auxtrace_buffer {
	struct list_head	list;
	size_t			size;
	pid_t			pid;
	pid_t			tid;
	int			cpu;
	void			*data;
	off_t			data_offset;
	void			*mmap_addr;
	size_t			mmap_size;
	bool			data_needs_freeing;
	bool			consecutive;
	u64			offset;
	u64			reference;
	u64			buffer_nr;
	size_t			use_size;
	void			*use_data;
};

/**
 * struct auxtrace_queue - a queue of AUX area tracing data buffers.
 * @head: head of buffer list
 * @tid: in per-thread mode, the tid this queue is associated with
 * @cpu: in per-cpu mode, the cpu this queue is associated with
 * @set: %true once this queue has been dedicated to a specific thread or cpu
 * @priv: implementation-specific data
 */
struct auxtrace_queue {
	struct list_head	head;
	pid_t			tid;
	int			cpu;
	bool			set;
	void			*priv;
};

/**
 * struct auxtrace_queues - an array of AUX area tracing queues.
 * @queue_array: array of queues
 * @nr_queues: number of queues
 * @new_data: set whenever new data is queued
 * @populated: queues have been fully populated using the auxtrace_index
 * @next_buffer_nr: used to number each buffer
 */
struct auxtrace_queues {
	struct auxtrace_queue	*queue_array;
	unsigned int		nr_queues;
	bool			new_data;
	bool			populated;
	u64			next_buffer_nr;
};

/**
 * struct auxtrace_heap_item - element of struct auxtrace_heap.
 * @queue_nr: queue number
 * @ordinal: value used for sorting (lowest ordinal is top of the heap) expected
 *           to be a timestamp
 */
struct auxtrace_heap_item {
	unsigned int		queue_nr;
	u64			ordinal;
};

/**
 * struct auxtrace_heap - a heap suitable for sorting AUX area tracing queues.
 * @heap_array: the heap
 * @heap_cnt: the number of elements in the heap
 * @heap_sz: maximum number of elements (grows as needed)
 */
struct auxtrace_heap {
	struct auxtrace_heap_item	*heap_array;
	unsigned int		heap_cnt;
	unsigned int		heap_sz;
};

/**
 * struct auxtrace_mmap - records an mmap of the auxtrace buffer.
 * @base: address of mapped area
 * @userpg: pointer to buffer's perf_event_mmap_page
 * @mask: %0 if @len is not a power of two, otherwise (@len - %1)
 * @len: size of mapped area
 * @prev: previous aux_head
 * @idx: index of this mmap
 * @tid: tid for a per-thread mmap (also set if there is only 1 tid on a per-cpu
 *       mmap) otherwise %0
 * @cpu: cpu number for a per-cpu mmap otherwise %-1
 */
struct auxtrace_mmap {
	void		*base;
	void		*userpg;
	size_t		mask;
	size_t		len;
	u64		prev;
	int		idx;
	pid_t		tid;
	int		cpu;
};

/**
 * struct auxtrace_mmap_params - parameters to set up struct auxtrace_mmap.
 * @mask: %0 if @len is not a power of two, otherwise (@len - %1)
 * @offset: file offset of mapped area
 * @len: size of mapped area
 * @prot: mmap memory protection
 * @idx: index of this mmap
 * @tid: tid for a per-thread mmap (also set if there is only 1 tid on a per-cpu
 *       mmap) otherwise %0
 * @cpu: cpu number for a per-cpu mmap otherwise %-1
 */
struct auxtrace_mmap_params {
	size_t		mask;
	off_t		offset;
	size_t		len;
	int		prot;
	int		idx;
	pid_t		tid;
	int		cpu;
};

/**
 * struct auxtrace_record - callbacks for recording AUX area data.
 * @recording_options: validate and process recording options
 * @info_priv_size: return the size of the private data in auxtrace_info_event
 * @info_fill: fill-in the private data in auxtrace_info_event
 * @free: free this auxtrace record structure
 * @snapshot_start: starting a snapshot
 * @snapshot_finish: finishing a snapshot
 * @find_snapshot: find data to snapshot within auxtrace mmap
 * @parse_snapshot_options: parse snapshot options
 * @reference: provide a 64-bit reference number for auxtrace_event
 * @read_finish: called after reading from an auxtrace mmap
 * @alignment: alignment (if any) for AUX area data
 * @default_aux_sample_size: default sample size for --aux sample option
 * @pmu: associated pmu
 * @evlist: selected events list
 */
struct auxtrace_record {
	int (*recording_options)(struct auxtrace_record *itr,
				 struct evlist *evlist,
				 struct record_opts *opts);
	size_t (*info_priv_size)(struct auxtrace_record *itr,
				 struct evlist *evlist);
	int (*info_fill)(struct auxtrace_record *itr,
			 struct perf_session *session,
			 struct perf_record_auxtrace_info *auxtrace_info,
			 size_t priv_size);
	void (*free)(struct auxtrace_record *itr);
	int (*snapshot_start)(struct auxtrace_record *itr);
	int (*snapshot_finish)(struct auxtrace_record *itr);
	int (*find_snapshot)(struct auxtrace_record *itr, int idx,
			     struct auxtrace_mmap *mm, unsigned char *data,
			     u64 *head, u64 *old);
	int (*parse_snapshot_options)(struct auxtrace_record *itr,
				      struct record_opts *opts,
				      const char *str);
	u64 (*reference)(struct auxtrace_record *itr);
	int (*read_finish)(struct auxtrace_record *itr, int idx);
	unsigned int alignment;
	unsigned int default_aux_sample_size;
	struct perf_pmu *pmu;
	struct evlist *evlist;
};

/**
 * struct addr_filter - address filter.
 * @list: list node
 * @range: true if it is a range filter
 * @start: true if action is 'filter' or 'start'
 * @action: 'filter', 'start' or 'stop' ('tracestop' is accepted but converted
 *          to 'stop')
 * @sym_from: symbol name for the filter address
 * @sym_to: symbol name that determines the filter size
 * @sym_from_idx: selects n'th from symbols with the same name (0 means global
 *                and less than 0 means symbol must be unique)
 * @sym_to_idx: same as @sym_from_idx but for @sym_to
 * @addr: filter address
 * @size: filter region size (for range filters)
 * @filename: DSO file name or NULL for the kernel
 * @str: allocated string that contains the other string members
 */
struct addr_filter {
	struct list_head	list;
	bool			range;
	bool			start;
	const char		*action;
	const char		*sym_from;
	const char		*sym_to;
	int			sym_from_idx;
	int			sym_to_idx;
	u64			addr;
	u64			size;
	const char		*filename;
	char			*str;
};

/**
 * struct addr_filters - list of address filters.
 * @head: list of address filters
 * @cnt: number of address filters
 */
struct addr_filters {
	struct list_head	head;
	int			cnt;
};

struct auxtrace_cache;

#ifdef HAVE_AUXTRACE_SUPPORT

/*
 * In snapshot mode the mmapped page is read-only which makes using
 * __sync_val_compare_and_swap() problematic.  However, snapshot mode expects
 * the buffer is not updated while the snapshot is made (e.g. Intel PT disables
 * the event) so there is not a race anyway.
 */
static inline u64 auxtrace_mmap__read_snapshot_head(struct auxtrace_mmap *mm)
{
	struct perf_event_mmap_page *pc = mm->userpg;
	u64 head = READ_ONCE(pc->aux_head);

	/* Ensure all reads are done after we read the head */
	rmb();
	return head;
}

static inline u64 auxtrace_mmap__read_head(struct auxtrace_mmap *mm)
{
	struct perf_event_mmap_page *pc = mm->userpg;
#if BITS_PER_LONG == 64 || !defined(HAVE_SYNC_COMPARE_AND_SWAP_SUPPORT)
	u64 head = READ_ONCE(pc->aux_head);
#else
	u64 head = __sync_val_compare_and_swap(&pc->aux_head, 0, 0);
#endif

	/* Ensure all reads are done after we read the head */
	rmb();
	return head;
}

static inline void auxtrace_mmap__write_tail(struct auxtrace_mmap *mm, u64 tail)
{
	struct perf_event_mmap_page *pc = mm->userpg;
#if BITS_PER_LONG != 64 && defined(HAVE_SYNC_COMPARE_AND_SWAP_SUPPORT)
	u64 old_tail;
#endif

	/* Ensure all reads are done before we write the tail out */
	mb();
#if BITS_PER_LONG == 64 || !defined(HAVE_SYNC_COMPARE_AND_SWAP_SUPPORT)
	pc->aux_tail = tail;
#else
	do {
		old_tail = __sync_val_compare_and_swap(&pc->aux_tail, 0, 0);
	} while (!__sync_bool_compare_and_swap(&pc->aux_tail, old_tail, tail));
#endif
}

int auxtrace_mmap__mmap(struct auxtrace_mmap *mm,
			struct auxtrace_mmap_params *mp,
			void *userpg, int fd);
void auxtrace_mmap__munmap(struct auxtrace_mmap *mm);
void auxtrace_mmap_params__init(struct auxtrace_mmap_params *mp,
				off_t auxtrace_offset,
				unsigned int auxtrace_pages,
				bool auxtrace_overwrite);
void auxtrace_mmap_params__set_idx(struct auxtrace_mmap_params *mp,
				   struct evlist *evlist, int idx,
				   bool per_cpu);

typedef int (*process_auxtrace_t)(struct perf_tool *tool,
				  struct mmap *map,
				  union perf_event *event, void *data1,
				  size_t len1, void *data2, size_t len2);

int auxtrace_mmap__read(struct mmap *map, struct auxtrace_record *itr,
			struct perf_tool *tool, process_auxtrace_t fn);

int auxtrace_mmap__read_snapshot(struct mmap *map,
				 struct auxtrace_record *itr,
				 struct perf_tool *tool, process_auxtrace_t fn,
				 size_t snapshot_size);

int auxtrace_queues__init(struct auxtrace_queues *queues);
int auxtrace_queues__add_event(struct auxtrace_queues *queues,
			       struct perf_session *session,
			       union perf_event *event, off_t data_offset,
			       struct auxtrace_buffer **buffer_ptr);
struct auxtrace_queue *
auxtrace_queues__sample_queue(struct auxtrace_queues *queues,
			      struct perf_sample *sample,
			      struct perf_session *session);
int auxtrace_queues__add_sample(struct auxtrace_queues *queues,
				struct perf_session *session,
				struct perf_sample *sample, u64 data_offset,
				u64 reference);
void auxtrace_queues__free(struct auxtrace_queues *queues);
int auxtrace_queues__process_index(struct auxtrace_queues *queues,
				   struct perf_session *session);
int auxtrace_queue_data(struct perf_session *session, bool samples,
			bool events);
struct auxtrace_buffer *auxtrace_buffer__next(struct auxtrace_queue *queue,
					      struct auxtrace_buffer *buffer);
void *auxtrace_buffer__get_data(struct auxtrace_buffer *buffer, int fd);
void auxtrace_buffer__put_data(struct auxtrace_buffer *buffer);
void auxtrace_buffer__drop_data(struct auxtrace_buffer *buffer);
void auxtrace_buffer__free(struct auxtrace_buffer *buffer);

int auxtrace_heap__add(struct auxtrace_heap *heap, unsigned int queue_nr,
		       u64 ordinal);
void auxtrace_heap__pop(struct auxtrace_heap *heap);
void auxtrace_heap__free(struct auxtrace_heap *heap);

struct auxtrace_cache_entry {
	struct hlist_node hash;
	u32 key;
};

struct auxtrace_cache *auxtrace_cache__new(unsigned int bits, size_t entry_size,
					   unsigned int limit_percent);
void auxtrace_cache__free(struct auxtrace_cache *auxtrace_cache);
void *auxtrace_cache__alloc_entry(struct auxtrace_cache *c);
void auxtrace_cache__free_entry(struct auxtrace_cache *c, void *entry);
int auxtrace_cache__add(struct auxtrace_cache *c, u32 key,
			struct auxtrace_cache_entry *entry);
void auxtrace_cache__remove(struct auxtrace_cache *c, u32 key);
void *auxtrace_cache__lookup(struct auxtrace_cache *c, u32 key);

struct auxtrace_record *auxtrace_record__init(struct evlist *evlist,
					      int *err);

int auxtrace_parse_snapshot_options(struct auxtrace_record *itr,
				    struct record_opts *opts,
				    const char *str);
int auxtrace_parse_sample_options(struct auxtrace_record *itr,
				  struct evlist *evlist,
				  struct record_opts *opts, const char *str);
void auxtrace_regroup_aux_output(struct evlist *evlist);
int auxtrace_record__options(struct auxtrace_record *itr,
			     struct evlist *evlist,
			     struct record_opts *opts);
size_t auxtrace_record__info_priv_size(struct auxtrace_record *itr,
				       struct evlist *evlist);
int auxtrace_record__info_fill(struct auxtrace_record *itr,
			       struct perf_session *session,
			       struct perf_record_auxtrace_info *auxtrace_info,
			       size_t priv_size);
void auxtrace_record__free(struct auxtrace_record *itr);
int auxtrace_record__snapshot_start(struct auxtrace_record *itr);
int auxtrace_record__snapshot_finish(struct auxtrace_record *itr, bool on_exit);
int auxtrace_record__find_snapshot(struct auxtrace_record *itr, int idx,
				   struct auxtrace_mmap *mm,
				   unsigned char *data, u64 *head, u64 *old);
u64 auxtrace_record__reference(struct auxtrace_record *itr);
int auxtrace_record__read_finish(struct auxtrace_record *itr, int idx);

int auxtrace_index__auxtrace_event(struct list_head *head, union perf_event *event,
				   off_t file_offset);
int auxtrace_index__write(int fd, struct list_head *head);
int auxtrace_index__process(int fd, u64 size, struct perf_session *session,
			    bool needs_swap);
void auxtrace_index__free(struct list_head *head);

void auxtrace_synth_error(struct perf_record_auxtrace_error *auxtrace_error, int type,
			  int code, int cpu, pid_t pid, pid_t tid, u64 ip,
			  const char *msg, u64 timestamp);

int perf_event__process_auxtrace_info(struct perf_session *session,
				      union perf_event *event);
s64 perf_event__process_auxtrace(struct perf_session *session,
				 union perf_event *event);
int perf_event__process_auxtrace_error(struct perf_session *session,
				       union perf_event *event);
int itrace_parse_synth_opts(const struct option *opt, const char *str,
			    int unset);
void itrace_synth_opts__set_default(struct itrace_synth_opts *synth_opts,
				    bool no_sample);

size_t perf_event__fprintf_auxtrace_error(union perf_event *event, FILE *fp);
void perf_session__auxtrace_error_inc(struct perf_session *session,
				      union perf_event *event);
void events_stats__auxtrace_error_warn(const struct events_stats *stats);

void addr_filters__init(struct addr_filters *filts);
void addr_filters__exit(struct addr_filters *filts);
int addr_filters__parse_bare_filter(struct addr_filters *filts,
				    const char *filter);
int auxtrace_parse_filters(struct evlist *evlist);

int auxtrace__process_event(struct perf_session *session, union perf_event *event,
			    struct perf_sample *sample, struct perf_tool *tool);
void auxtrace__dump_auxtrace_sample(struct perf_session *session,
				    struct perf_sample *sample);
int auxtrace__flush_events(struct perf_session *session, struct perf_tool *tool);
void auxtrace__free_events(struct perf_session *session);
void auxtrace__free(struct perf_session *session);
bool auxtrace__evsel_is_auxtrace(struct perf_session *session,
				 struct evsel *evsel);

#define ITRACE_HELP \
"				i[period]:    		synthesize instructions events\n" \
"				b:	    		synthesize branches events (branch misses for Arm SPE)\n" \
"				c:	    		synthesize branches events (calls only)\n"	\
"				r:	    		synthesize branches events (returns only)\n" \
"				x:	    		synthesize transactions events\n"		\
"				w:	    		synthesize ptwrite events\n"		\
"				p:	    		synthesize power events\n"			\
"				o:			synthesize other events recorded due to the use\n" \
"							of aux-output (refer to perf record)\n"	\
"				e[flags]:		synthesize error events\n" \
"							each flag must be preceded by + or -\n" \
"							error flags are: o (overflow)\n" \
"									 l (data lost)\n" \
"				d[flags]:		create a debug log\n" \
"							each flag must be preceded by + or -\n" \
"							log flags are: a (all perf events)\n" \
"				f:	    		synthesize first level cache events\n" \
"				m:	    		synthesize last level cache events\n" \
"				t:	    		synthesize TLB events\n" \
"				a:	    		synthesize remote access events\n" \
"				g[len]:     		synthesize a call chain (use with i or x)\n" \
"				G[len]:			synthesize a call chain on existing event records\n" \
"				l[len]:     		synthesize last branch entries (use with i or x)\n" \
"				L[len]:			synthesize last branch entries on existing event records\n" \
"				sNUMBER:    		skip initial number of events\n"		\
"				q:			quicker (less detailed) decoding\n" \
"				PERIOD[ns|us|ms|i|t]:   specify period to sample stream\n" \
"				concatenate multiple options. Default is ibxwpe or cewp\n"

static inline
void itrace_synth_opts__set_time_range(struct itrace_synth_opts *opts,
				       struct perf_time_interval *ptime_range,
				       int range_num)
{
	opts->ptime_range = ptime_range;
	opts->range_num = range_num;
}

static inline
void itrace_synth_opts__clear_time_range(struct itrace_synth_opts *opts)
{
	opts->ptime_range = NULL;
	opts->range_num = 0;
}

#else
#include "debug.h"

static inline struct auxtrace_record *
auxtrace_record__init(struct evlist *evlist __maybe_unused,
		      int *err)
{
	*err = 0;
	return NULL;
}

static inline
void auxtrace_record__free(struct auxtrace_record *itr __maybe_unused)
{
}

static inline
int auxtrace_record__options(struct auxtrace_record *itr __maybe_unused,
			     struct evlist *evlist __maybe_unused,
			     struct record_opts *opts __maybe_unused)
{
	return 0;
}

#define perf_event__process_auxtrace_info		0
#define perf_event__process_auxtrace			0
#define perf_event__process_auxtrace_error		0

static inline
void perf_session__auxtrace_error_inc(struct perf_session *session
				      __maybe_unused,
				      union perf_event *event
				      __maybe_unused)
{
}

static inline
void events_stats__auxtrace_error_warn(const struct events_stats *stats
				       __maybe_unused)
{
}

static inline
int itrace_parse_synth_opts(const struct option *opt __maybe_unused,
			    const char *str __maybe_unused,
			    int unset __maybe_unused)
{
	pr_err("AUX area tracing not supported\n");
	return -EINVAL;
}

static inline
int auxtrace_parse_snapshot_options(struct auxtrace_record *itr __maybe_unused,
				    struct record_opts *opts __maybe_unused,
				    const char *str)
{
	if (!str)
		return 0;
	pr_err("AUX area tracing not supported\n");
	return -EINVAL;
}

static inline
int auxtrace_parse_sample_options(struct auxtrace_record *itr __maybe_unused,
				  struct evlist *evlist __maybe_unused,
				  struct record_opts *opts __maybe_unused,
				  const char *str)
{
	if (!str)
		return 0;
	pr_err("AUX area tracing not supported\n");
	return -EINVAL;
}

static inline
void auxtrace_regroup_aux_output(struct evlist *evlist __maybe_unused)
{
}

static inline
int auxtrace__process_event(struct perf_session *session __maybe_unused,
			    union perf_event *event __maybe_unused,
			    struct perf_sample *sample __maybe_unused,
			    struct perf_tool *tool __maybe_unused)
{
	return 0;
}

static inline
void auxtrace__dump_auxtrace_sample(struct perf_session *session __maybe_unused,
				    struct perf_sample *sample __maybe_unused)
{
}

static inline
int auxtrace__flush_events(struct perf_session *session __maybe_unused,
			   struct perf_tool *tool __maybe_unused)
{
	return 0;
}

static inline
void auxtrace__free_events(struct perf_session *session __maybe_unused)
{
}

static inline
void auxtrace_cache__free(struct auxtrace_cache *auxtrace_cache __maybe_unused)
{
}

static inline
void auxtrace__free(struct perf_session *session __maybe_unused)
{
}

static inline
int auxtrace_index__write(int fd __maybe_unused,
			  struct list_head *head __maybe_unused)
{
	return -EINVAL;
}

static inline
int auxtrace_index__process(int fd __maybe_unused,
			    u64 size __maybe_unused,
			    struct perf_session *session __maybe_unused,
			    bool needs_swap __maybe_unused)
{
	return -EINVAL;
}

static inline
void auxtrace_index__free(struct list_head *head __maybe_unused)
{
}

static inline
bool auxtrace__evsel_is_auxtrace(struct perf_session *session __maybe_unused,
				 struct evsel *evsel __maybe_unused)
{
	return false;
}

static inline
int auxtrace_parse_filters(struct evlist *evlist __maybe_unused)
{
	return 0;
}

int auxtrace_mmap__mmap(struct auxtrace_mmap *mm,
			struct auxtrace_mmap_params *mp,
			void *userpg, int fd);
void auxtrace_mmap__munmap(struct auxtrace_mmap *mm);
void auxtrace_mmap_params__init(struct auxtrace_mmap_params *mp,
				off_t auxtrace_offset,
				unsigned int auxtrace_pages,
				bool auxtrace_overwrite);
void auxtrace_mmap_params__set_idx(struct auxtrace_mmap_params *mp,
				   struct evlist *evlist, int idx,
				   bool per_cpu);

#define ITRACE_HELP ""

static inline
void itrace_synth_opts__set_time_range(struct itrace_synth_opts *opts
				       __maybe_unused,
				       struct perf_time_interval *ptime_range
				       __maybe_unused,
				       int range_num __maybe_unused)
{
}

static inline
void itrace_synth_opts__clear_time_range(struct itrace_synth_opts *opts
					 __maybe_unused)
{
}

#endif

#endif
