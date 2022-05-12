/* SPDX-License-Identifier: GPL-2.0 */
#undef TRACE_SYSTEM
#define TRACE_SYSTEM io_uring

#if !defined(_TRACE_IO_URING_H) || defined(TRACE_HEADER_MULTI_READ)
#define _TRACE_IO_URING_H

#include <linux/tracepoint.h>

struct io_wq_work;

/**
 * io_uring_create - called after a new io_uring context was prepared
 *
 * @fd:			corresponding file descriptor
 * @ctx:		pointer to a ring context structure
 * @sq_entries:	actual SQ size
 * @cq_entries:	actual CQ size
 * @flags:		SQ ring flags, provided to io_uring_setup(2)
 *
 * Allows to trace io_uring creation and provide pointer to a context, that can
 * be used later to find correlated events.
 */
TRACE_EVENT(io_uring_create,

	TP_PROTO(int fd, void *ctx, u32 sq_entries, u32 cq_entries, u32 flags),

	TP_ARGS(fd, ctx, sq_entries, cq_entries, flags),

	TP_STRUCT__entry (
		__field(  int,		fd			)
		__field(  void *,	ctx			)
		__field(  u32,		sq_entries	)
		__field(  u32,		cq_entries	)
		__field(  u32,		flags		)
	),

	TP_fast_assign(
		__entry->fd			= fd;
		__entry->ctx		= ctx;
		__entry->sq_entries	= sq_entries;
		__entry->cq_entries	= cq_entries;
		__entry->flags		= flags;
	),

	TP_printk("ring %p, fd %d sq size %d, cq size %d, flags %d",
			  __entry->ctx, __entry->fd, __entry->sq_entries,
			  __entry->cq_entries, __entry->flags)
);

/**
 * io_uring_register - called after a buffer/file/eventfd was successfully
 * 					   registered for a ring
 *
 * @ctx:			pointer to a ring context structure
 * @opcode:			describes which operation to perform
 * @nr_user_files:	number of registered files
 * @nr_user_bufs:	number of registered buffers
 * @cq_ev_fd:		whether eventfs registered or not
 * @ret:			return code
 *
 * Allows to trace fixed files/buffers/eventfds, that could be registered to
 * avoid an overhead of getting references to them for every operation. This
 * event, together with io_uring_file_get, can provide a full picture of how
 * much overhead one can reduce via fixing.
 */
TRACE_EVENT(io_uring_register,

	TP_PROTO(void *ctx, unsigned opcode, unsigned nr_files,
			 unsigned nr_bufs, bool eventfd, long ret),

	TP_ARGS(ctx, opcode, nr_files, nr_bufs, eventfd, ret),

	TP_STRUCT__entry (
		__field(  void *,	ctx			)
		__field(  unsigned,	opcode		)
		__field(  unsigned,	nr_files	)
		__field(  unsigned,	nr_bufs		)
		__field(  bool,		eventfd		)
		__field(  long,		ret			)
	),

	TP_fast_assign(
		__entry->ctx		= ctx;
		__entry->opcode		= opcode;
		__entry->nr_files	= nr_files;
		__entry->nr_bufs	= nr_bufs;
		__entry->eventfd	= eventfd;
		__entry->ret		= ret;
	),

	TP_printk("ring %p, opcode %d, nr_user_files %d, nr_user_bufs %d, "
			  "eventfd %d, ret %ld",
			  __entry->ctx, __entry->opcode, __entry->nr_files,
			  __entry->nr_bufs, __entry->eventfd, __entry->ret)
);

/**
 * io_uring_file_get - called before getting references to an SQE file
 *
 * @ctx:	pointer to a ring context structure
 * @fd:		SQE file descriptor
 *
 * Allows to trace out how often an SQE file reference is obtained, which can
 * help figuring out if it makes sense to use fixed files, or check that fixed
 * files are used correctly.
 */
TRACE_EVENT(io_uring_file_get,

	TP_PROTO(void *ctx, int fd),

	TP_ARGS(ctx, fd),

	TP_STRUCT__entry (
		__field(  void *,	ctx	)
		__field(  int,		fd	)
	),

	TP_fast_assign(
		__entry->ctx	= ctx;
		__entry->fd		= fd;
	),

	TP_printk("ring %p, fd %d", __entry->ctx, __entry->fd)
);

/**
 * io_uring_queue_async_work - called before submitting a new async work
 *
 * @ctx:	pointer to a ring context structure
 * @hashed:	type of workqueue, hashed or normal
 * @req:	pointer to a submitted request
 * @work:	pointer to a submitted io_wq_work
 *
 * Allows to trace asynchronous work submission.
 */
TRACE_EVENT(io_uring_queue_async_work,

	TP_PROTO(void *ctx, int rw, void * req, struct io_wq_work *work,
			 unsigned int flags),

	TP_ARGS(ctx, rw, req, work, flags),

	TP_STRUCT__entry (
		__field(  void *,				ctx		)
		__field(  int,					rw		)
		__field(  void *,				req		)
		__field(  struct io_wq_work *,		work	)
		__field(  unsigned int,			flags	)
	),

	TP_fast_assign(
		__entry->ctx	= ctx;
		__entry->rw		= rw;
		__entry->req	= req;
		__entry->work	= work;
		__entry->flags	= flags;
	),

	TP_printk("ring %p, request %p, flags %d, %s queue, work %p",
			  __entry->ctx, __entry->req, __entry->flags,
			  __entry->rw ? "hashed" : "normal", __entry->work)
);

/**
 * io_uring_defer - called when an io_uring request is deferred
 *
 * @ctx:	pointer to a ring context structure
 * @req:	pointer to a deferred request
 * @user_data:	user data associated with the request
 *
 * Allows to track deferred requests, to get an insight about what requests are
 * not started immediately.
 */
TRACE_EVENT(io_uring_defer,

	TP_PROTO(void *ctx, void *req, unsigned long long user_data),

	TP_ARGS(ctx, req, user_data),

	TP_STRUCT__entry (
		__field(  void *,	ctx		)
		__field(  void *,	req		)
		__field(  unsigned long long, data	)
	),

	TP_fast_assign(
		__entry->ctx	= ctx;
		__entry->req	= req;
		__entry->data	= user_data;
	),

	TP_printk("ring %p, request %p user_data %llu", __entry->ctx,
			__entry->req, __entry->data)
);

/**
 * io_uring_link - called before the io_uring request added into link_list of
 * 				   another request
 *
 * @ctx:			pointer to a ring context structure
 * @req:			pointer to a linked request
 * @target_req:		pointer to a previous request, that would contain @req
 *
 * Allows to track linked requests, to understand dependencies between requests
 * and how does it influence their execution flow.
 */
TRACE_EVENT(io_uring_link,

	TP_PROTO(void *ctx, void *req, void *target_req),

	TP_ARGS(ctx, req, target_req),

	TP_STRUCT__entry (
		__field(  void *,	ctx			)
		__field(  void *,	req			)
		__field(  void *,	target_req	)
	),

	TP_fast_assign(
		__entry->ctx		= ctx;
		__entry->req		= req;
		__entry->target_req	= target_req;
	),

	TP_printk("ring %p, request %p linked after %p",
			  __entry->ctx, __entry->req, __entry->target_req)
);

/**
 * io_uring_cqring_wait - called before start waiting for an available CQE
 *
 * @ctx:		pointer to a ring context structure
 * @min_events:	minimal number of events to wait for
 *
 * Allows to track waiting for CQE, so that we can e.g. troubleshoot
 * situations, when an application wants to wait for an event, that never
 * comes.
 */
TRACE_EVENT(io_uring_cqring_wait,

	TP_PROTO(void *ctx, int min_events),

	TP_ARGS(ctx, min_events),

	TP_STRUCT__entry (
		__field(  void *,	ctx			)
		__field(  int,		min_events	)
	),

	TP_fast_assign(
		__entry->ctx	= ctx;
		__entry->min_events	= min_events;
	),

	TP_printk("ring %p, min_events %d", __entry->ctx, __entry->min_events)
);

/**
 * io_uring_fail_link - called before failing a linked request
 *
 * @req:	request, which links were cancelled
 * @link:	cancelled link
 *
 * Allows to track linked requests cancellation, to see not only that some work
 * was cancelled, but also which request was the reason.
 */
TRACE_EVENT(io_uring_fail_link,

	TP_PROTO(void *req, void *link),

	TP_ARGS(req, link),

	TP_STRUCT__entry (
		__field(  void *,	req		)
		__field(  void *,	link	)
	),

	TP_fast_assign(
		__entry->req	= req;
		__entry->link	= link;
	),

	TP_printk("request %p, link %p", __entry->req, __entry->link)
);

/**
 * io_uring_complete - called when completing an SQE
 *
 * @ctx:		pointer to a ring context structure
 * @user_data:		user data associated with the request
 * @res:		result of the request
 * @cflags:		completion flags
 *
 */
TRACE_EVENT(io_uring_complete,

	TP_PROTO(void *ctx, u64 user_data, long res, unsigned cflags),

	TP_ARGS(ctx, user_data, res, cflags),

	TP_STRUCT__entry (
		__field(  void *,	ctx		)
		__field(  u64,		user_data	)
		__field(  long,		res		)
		__field(  unsigned,	cflags		)
	),

	TP_fast_assign(
		__entry->ctx		= ctx;
		__entry->user_data	= user_data;
		__entry->res		= res;
		__entry->cflags		= cflags;
	),

	TP_printk("ring %p, user_data 0x%llx, result %ld, cflags %x",
			  __entry->ctx, (unsigned long long)__entry->user_data,
			  __entry->res, __entry->cflags)
);


/**
 * io_uring_submit_sqe - called before submitting one SQE
 *
 * @ctx:		pointer to a ring context structure
 * @opcode:		opcode of request
 * @user_data:		user data associated with the request
 * @force_nonblock:	whether a context blocking or not
 * @sq_thread:		true if sq_thread has submitted this SQE
 *
 * Allows to track SQE submitting, to understand what was the source of it, SQ
 * thread or io_uring_enter call.
 */
TRACE_EVENT(io_uring_submit_sqe,

	TP_PROTO(void *ctx, u8 opcode, u64 user_data, bool force_nonblock,
		 bool sq_thread),

	TP_ARGS(ctx, opcode, user_data, force_nonblock, sq_thread),

	TP_STRUCT__entry (
		__field(  void *,	ctx		)
		__field(  u8,		opcode		)
		__field(  u64,		user_data	)
		__field(  bool,		force_nonblock	)
		__field(  bool,		sq_thread	)
	),

	TP_fast_assign(
		__entry->ctx		= ctx;
		__entry->opcode		= opcode;
		__entry->user_data	= user_data;
		__entry->force_nonblock	= force_nonblock;
		__entry->sq_thread	= sq_thread;
	),

	TP_printk("ring %p, op %d, data 0x%llx, non block %d, sq_thread %d",
			  __entry->ctx, __entry->opcode,
			  (unsigned long long) __entry->user_data,
			  __entry->force_nonblock, __entry->sq_thread)
);

TRACE_EVENT(io_uring_poll_arm,

	TP_PROTO(void *ctx, u8 opcode, u64 user_data, int mask, int events),

	TP_ARGS(ctx, opcode, user_data, mask, events),

	TP_STRUCT__entry (
		__field(  void *,	ctx		)
		__field(  u8,		opcode		)
		__field(  u64,		user_data	)
		__field(  int,		mask		)
		__field(  int,		events		)
	),

	TP_fast_assign(
		__entry->ctx		= ctx;
		__entry->opcode		= opcode;
		__entry->user_data	= user_data;
		__entry->mask		= mask;
		__entry->events		= events;
	),

	TP_printk("ring %p, op %d, data 0x%llx, mask 0x%x, events 0x%x",
			  __entry->ctx, __entry->opcode,
			  (unsigned long long) __entry->user_data,
			  __entry->mask, __entry->events)
);

TRACE_EVENT(io_uring_poll_wake,

	TP_PROTO(void *ctx, u8 opcode, u64 user_data, int mask),

	TP_ARGS(ctx, opcode, user_data, mask),

	TP_STRUCT__entry (
		__field(  void *,	ctx		)
		__field(  u8,		opcode		)
		__field(  u64,		user_data	)
		__field(  int,		mask		)
	),

	TP_fast_assign(
		__entry->ctx		= ctx;
		__entry->opcode		= opcode;
		__entry->user_data	= user_data;
		__entry->mask		= mask;
	),

	TP_printk("ring %p, op %d, data 0x%llx, mask 0x%x",
			  __entry->ctx, __entry->opcode,
			  (unsigned long long) __entry->user_data,
			  __entry->mask)
);

TRACE_EVENT(io_uring_task_add,

	TP_PROTO(void *ctx, u8 opcode, u64 user_data, int mask),

	TP_ARGS(ctx, opcode, user_data, mask),

	TP_STRUCT__entry (
		__field(  void *,	ctx		)
		__field(  u8,		opcode		)
		__field(  u64,		user_data	)
		__field(  int,		mask		)
	),

	TP_fast_assign(
		__entry->ctx		= ctx;
		__entry->opcode		= opcode;
		__entry->user_data	= user_data;
		__entry->mask		= mask;
	),

	TP_printk("ring %p, op %d, data 0x%llx, mask %x",
			  __entry->ctx, __entry->opcode,
			  (unsigned long long) __entry->user_data,
			  __entry->mask)
);

TRACE_EVENT(io_uring_task_run,

	TP_PROTO(void *ctx, u8 opcode, u64 user_data),

	TP_ARGS(ctx, opcode, user_data),

	TP_STRUCT__entry (
		__field(  void *,	ctx		)
		__field(  u8,		opcode		)
		__field(  u64,		user_data	)
	),

	TP_fast_assign(
		__entry->ctx		= ctx;
		__entry->opcode		= opcode;
		__entry->user_data	= user_data;
	),

	TP_printk("ring %p, op %d, data 0x%llx",
			  __entry->ctx, __entry->opcode,
			  (unsigned long long) __entry->user_data)
);

#endif /* _TRACE_IO_URING_H */

/* This part must be outside protection */
#include <trace/define_trace.h>
