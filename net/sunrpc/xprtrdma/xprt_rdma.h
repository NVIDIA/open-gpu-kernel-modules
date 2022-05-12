/* SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause */
/*
 * Copyright (c) 2014-2017 Oracle.  All rights reserved.
 * Copyright (c) 2003-2007 Network Appliance, Inc. All rights reserved.
 *
 * This software is available to you under a choice of one of two
 * licenses.  You may choose to be licensed under the terms of the GNU
 * General Public License (GPL) Version 2, available from the file
 * COPYING in the main directory of this source tree, or the BSD-type
 * license below:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *      Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *      Redistributions in binary form must reproduce the above
 *      copyright notice, this list of conditions and the following
 *      disclaimer in the documentation and/or other materials provided
 *      with the distribution.
 *
 *      Neither the name of the Network Appliance, Inc. nor the names of
 *      its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written
 *      permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _LINUX_SUNRPC_XPRT_RDMA_H
#define _LINUX_SUNRPC_XPRT_RDMA_H

#include <linux/wait.h> 		/* wait_queue_head_t, etc */
#include <linux/spinlock.h> 		/* spinlock_t, etc */
#include <linux/atomic.h>		/* atomic_t, etc */
#include <linux/kref.h>			/* struct kref */
#include <linux/workqueue.h>		/* struct work_struct */
#include <linux/llist.h>

#include <rdma/rdma_cm.h>		/* RDMA connection api */
#include <rdma/ib_verbs.h>		/* RDMA verbs api */

#include <linux/sunrpc/clnt.h> 		/* rpc_xprt */
#include <linux/sunrpc/rpc_rdma_cid.h> 	/* completion IDs */
#include <linux/sunrpc/rpc_rdma.h> 	/* RPC/RDMA protocol */
#include <linux/sunrpc/xprtrdma.h> 	/* xprt parameters */

#define RDMA_RESOLVE_TIMEOUT	(5000)	/* 5 seconds */
#define RDMA_CONNECT_RETRY_MAX	(2)	/* retries if no listener backlog */

#define RPCRDMA_BIND_TO		(60U * HZ)
#define RPCRDMA_INIT_REEST_TO	(5U * HZ)
#define RPCRDMA_MAX_REEST_TO	(30U * HZ)
#define RPCRDMA_IDLE_DISC_TO	(5U * 60 * HZ)

/*
 * RDMA Endpoint -- connection endpoint details
 */
struct rpcrdma_ep {
	struct kref		re_kref;
	struct rdma_cm_id 	*re_id;
	struct ib_pd		*re_pd;
	unsigned int		re_max_rdma_segs;
	unsigned int		re_max_fr_depth;
	bool			re_implicit_roundup;
	enum ib_mr_type		re_mrtype;
	struct completion	re_done;
	unsigned int		re_send_count;
	unsigned int		re_send_batch;
	unsigned int		re_max_inline_send;
	unsigned int		re_max_inline_recv;
	int			re_async_rc;
	int			re_connect_status;
	atomic_t		re_receiving;
	atomic_t		re_force_disconnect;
	struct ib_qp_init_attr	re_attr;
	wait_queue_head_t       re_connect_wait;
	struct rpc_xprt		*re_xprt;
	struct rpcrdma_connect_private
				re_cm_private;
	struct rdma_conn_param	re_remote_cma;
	int			re_receive_count;
	unsigned int		re_max_requests; /* depends on device */
	unsigned int		re_inline_send;	/* negotiated */
	unsigned int		re_inline_recv;	/* negotiated */

	atomic_t		re_completion_ids;
};

/* Pre-allocate extra Work Requests for handling reverse-direction
 * Receives and Sends. This is a fixed value because the Work Queues
 * are allocated when the forward channel is set up, long before the
 * backchannel is provisioned. This value is two times
 * NFS4_DEF_CB_SLOT_TABLE_SIZE.
 */
#if defined(CONFIG_SUNRPC_BACKCHANNEL)
#define RPCRDMA_BACKWARD_WRS (32)
#else
#define RPCRDMA_BACKWARD_WRS (0)
#endif

/* Registered buffer -- registered kmalloc'd memory for RDMA SEND/RECV
 */

struct rpcrdma_regbuf {
	struct ib_sge		rg_iov;
	struct ib_device	*rg_device;
	enum dma_data_direction	rg_direction;
	void			*rg_data;
};

static inline u64 rdmab_addr(struct rpcrdma_regbuf *rb)
{
	return rb->rg_iov.addr;
}

static inline u32 rdmab_length(struct rpcrdma_regbuf *rb)
{
	return rb->rg_iov.length;
}

static inline u32 rdmab_lkey(struct rpcrdma_regbuf *rb)
{
	return rb->rg_iov.lkey;
}

static inline struct ib_device *rdmab_device(struct rpcrdma_regbuf *rb)
{
	return rb->rg_device;
}

static inline void *rdmab_data(const struct rpcrdma_regbuf *rb)
{
	return rb->rg_data;
}

#define RPCRDMA_DEF_GFP		(GFP_NOIO | __GFP_NOWARN)

/* To ensure a transport can always make forward progress,
 * the number of RDMA segments allowed in header chunk lists
 * is capped at 16. This prevents less-capable devices from
 * overrunning the Send buffer while building chunk lists.
 *
 * Elements of the Read list take up more room than the
 * Write list or Reply chunk. 16 read segments means the
 * chunk lists cannot consume more than
 *
 * ((16 + 2) * read segment size) + 1 XDR words,
 *
 * or about 400 bytes. The fixed part of the header is
 * another 24 bytes. Thus when the inline threshold is
 * 1024 bytes, at least 600 bytes are available for RPC
 * message bodies.
 */
enum {
	RPCRDMA_MAX_HDR_SEGS = 16,
};

/*
 * struct rpcrdma_rep -- this structure encapsulates state required
 * to receive and complete an RPC Reply, asychronously. It needs
 * several pieces of state:
 *
 *   o receive buffer and ib_sge (donated to provider)
 *   o status of receive (success or not, length, inv rkey)
 *   o bookkeeping state to get run by reply handler (XDR stream)
 *
 * These structures are allocated during transport initialization.
 * N of these are associated with a transport instance, managed by
 * struct rpcrdma_buffer. N is the max number of outstanding RPCs.
 */

struct rpcrdma_rep {
	struct ib_cqe		rr_cqe;
	struct rpc_rdma_cid	rr_cid;

	__be32			rr_xid;
	__be32			rr_vers;
	__be32			rr_proc;
	int			rr_wc_flags;
	u32			rr_inv_rkey;
	bool			rr_temp;
	struct rpcrdma_regbuf	*rr_rdmabuf;
	struct rpcrdma_xprt	*rr_rxprt;
	struct rpc_rqst		*rr_rqst;
	struct xdr_buf		rr_hdrbuf;
	struct xdr_stream	rr_stream;
	struct llist_node	rr_node;
	struct ib_recv_wr	rr_recv_wr;
	struct list_head	rr_all;
};

/* To reduce the rate at which a transport invokes ib_post_recv
 * (and thus the hardware doorbell rate), xprtrdma posts Receive
 * WRs in batches.
 *
 * Setting this to zero disables Receive post batching.
 */
enum {
	RPCRDMA_MAX_RECV_BATCH = 7,
};

/* struct rpcrdma_sendctx - DMA mapped SGEs to unmap after Send completes
 */
struct rpcrdma_req;
struct rpcrdma_sendctx {
	struct ib_cqe		sc_cqe;
	struct rpc_rdma_cid	sc_cid;
	struct rpcrdma_req	*sc_req;
	unsigned int		sc_unmap_count;
	struct ib_sge		sc_sges[];
};

/*
 * struct rpcrdma_mr - external memory region metadata
 *
 * An external memory region is any buffer or page that is registered
 * on the fly (ie, not pre-registered).
 */
struct rpcrdma_req;
struct rpcrdma_mr {
	struct list_head	mr_list;
	struct rpcrdma_req	*mr_req;

	struct ib_mr		*mr_ibmr;
	struct ib_device	*mr_device;
	struct scatterlist	*mr_sg;
	int			mr_nents;
	enum dma_data_direction	mr_dir;
	struct ib_cqe		mr_cqe;
	struct completion	mr_linv_done;
	union {
		struct ib_reg_wr	mr_regwr;
		struct ib_send_wr	mr_invwr;
	};
	struct rpcrdma_xprt	*mr_xprt;
	u32			mr_handle;
	u32			mr_length;
	u64			mr_offset;
	struct list_head	mr_all;
	struct rpc_rdma_cid	mr_cid;
};

/*
 * struct rpcrdma_req -- structure central to the request/reply sequence.
 *
 * N of these are associated with a transport instance, and stored in
 * struct rpcrdma_buffer. N is the max number of outstanding requests.
 *
 * It includes pre-registered buffer memory for send AND recv.
 * The recv buffer, however, is not owned by this structure, and
 * is "donated" to the hardware when a recv is posted. When a
 * reply is handled, the recv buffer used is given back to the
 * struct rpcrdma_req associated with the request.
 *
 * In addition to the basic memory, this structure includes an array
 * of iovs for send operations. The reason is that the iovs passed to
 * ib_post_{send,recv} must not be modified until the work request
 * completes.
 */

/* Maximum number of page-sized "segments" per chunk list to be
 * registered or invalidated. Must handle a Reply chunk:
 */
enum {
	RPCRDMA_MAX_IOV_SEGS	= 3,
	RPCRDMA_MAX_DATA_SEGS	= ((1 * 1024 * 1024) / PAGE_SIZE) + 1,
	RPCRDMA_MAX_SEGS	= RPCRDMA_MAX_DATA_SEGS +
				  RPCRDMA_MAX_IOV_SEGS,
};

/* Arguments for DMA mapping and registration */
struct rpcrdma_mr_seg {
	u32		mr_len;		/* length of segment */
	struct page	*mr_page;	/* underlying struct page */
	u64		mr_offset;	/* IN: page offset, OUT: iova */
};

/* The Send SGE array is provisioned to send a maximum size
 * inline request:
 * - RPC-over-RDMA header
 * - xdr_buf head iovec
 * - RPCRDMA_MAX_INLINE bytes, in pages
 * - xdr_buf tail iovec
 *
 * The actual number of array elements consumed by each RPC
 * depends on the device's max_sge limit.
 */
enum {
	RPCRDMA_MIN_SEND_SGES = 3,
	RPCRDMA_MAX_PAGE_SGES = RPCRDMA_MAX_INLINE >> PAGE_SHIFT,
	RPCRDMA_MAX_SEND_SGES = 1 + 1 + RPCRDMA_MAX_PAGE_SGES + 1,
};

struct rpcrdma_buffer;
struct rpcrdma_req {
	struct list_head	rl_list;
	struct rpc_rqst		rl_slot;
	struct rpcrdma_rep	*rl_reply;
	struct xdr_stream	rl_stream;
	struct xdr_buf		rl_hdrbuf;
	struct ib_send_wr	rl_wr;
	struct rpcrdma_sendctx	*rl_sendctx;
	struct rpcrdma_regbuf	*rl_rdmabuf;	/* xprt header */
	struct rpcrdma_regbuf	*rl_sendbuf;	/* rq_snd_buf */
	struct rpcrdma_regbuf	*rl_recvbuf;	/* rq_rcv_buf */

	struct list_head	rl_all;
	struct kref		rl_kref;

	struct list_head	rl_free_mrs;
	struct list_head	rl_registered;
	struct rpcrdma_mr_seg	rl_segments[RPCRDMA_MAX_SEGS];
};

static inline struct rpcrdma_req *
rpcr_to_rdmar(const struct rpc_rqst *rqst)
{
	return container_of(rqst, struct rpcrdma_req, rl_slot);
}

static inline void
rpcrdma_mr_push(struct rpcrdma_mr *mr, struct list_head *list)
{
	list_add(&mr->mr_list, list);
}

static inline struct rpcrdma_mr *
rpcrdma_mr_pop(struct list_head *list)
{
	struct rpcrdma_mr *mr;

	mr = list_first_entry_or_null(list, struct rpcrdma_mr, mr_list);
	if (mr)
		list_del_init(&mr->mr_list);
	return mr;
}

/*
 * struct rpcrdma_buffer -- holds list/queue of pre-registered memory for
 * inline requests/replies, and client/server credits.
 *
 * One of these is associated with a transport instance
 */
struct rpcrdma_buffer {
	spinlock_t		rb_lock;
	struct list_head	rb_send_bufs;
	struct list_head	rb_mrs;

	unsigned long		rb_sc_head;
	unsigned long		rb_sc_tail;
	unsigned long		rb_sc_last;
	struct rpcrdma_sendctx	**rb_sc_ctxs;

	struct list_head	rb_allreqs;
	struct list_head	rb_all_mrs;
	struct list_head	rb_all_reps;

	struct llist_head	rb_free_reps;

	__be32			rb_max_requests;
	u32			rb_credits;	/* most recent credit grant */

	u32			rb_bc_srv_max_requests;
	u32			rb_bc_max_requests;

	struct work_struct	rb_refresh_worker;
};

/*
 * Statistics for RPCRDMA
 */
struct rpcrdma_stats {
	/* accessed when sending a call */
	unsigned long		read_chunk_count;
	unsigned long		write_chunk_count;
	unsigned long		reply_chunk_count;
	unsigned long long	total_rdma_request;

	/* rarely accessed error counters */
	unsigned long long	pullup_copy_count;
	unsigned long		hardway_register_count;
	unsigned long		failed_marshal_count;
	unsigned long		bad_reply_count;
	unsigned long		mrs_recycled;
	unsigned long		mrs_orphaned;
	unsigned long		mrs_allocated;
	unsigned long		empty_sendctx_q;

	/* accessed when receiving a reply */
	unsigned long long	total_rdma_reply;
	unsigned long long	fixup_copy_count;
	unsigned long		reply_waits_for_send;
	unsigned long		local_inv_needed;
	unsigned long		nomsg_call_count;
	unsigned long		bcall_count;
};

/*
 * RPCRDMA transport -- encapsulates the structures above for
 * integration with RPC.
 *
 * The contained structures are embedded, not pointers,
 * for convenience. This structure need not be visible externally.
 *
 * It is allocated and initialized during mount, and released
 * during unmount.
 */
struct rpcrdma_xprt {
	struct rpc_xprt		rx_xprt;
	struct rpcrdma_ep	*rx_ep;
	struct rpcrdma_buffer	rx_buf;
	struct delayed_work	rx_connect_worker;
	struct rpc_timeout	rx_timeout;
	struct rpcrdma_stats	rx_stats;
};

#define rpcx_to_rdmax(x) container_of(x, struct rpcrdma_xprt, rx_xprt)

static inline const char *
rpcrdma_addrstr(const struct rpcrdma_xprt *r_xprt)
{
	return r_xprt->rx_xprt.address_strings[RPC_DISPLAY_ADDR];
}

static inline const char *
rpcrdma_portstr(const struct rpcrdma_xprt *r_xprt)
{
	return r_xprt->rx_xprt.address_strings[RPC_DISPLAY_PORT];
}

/* Setting this to 0 ensures interoperability with early servers.
 * Setting this to 1 enhances certain unaligned read/write performance.
 * Default is 0, see sysctl entry and rpc_rdma.c rpcrdma_convert_iovs() */
extern int xprt_rdma_pad_optimize;

/* This setting controls the hunt for a supported memory
 * registration strategy.
 */
extern unsigned int xprt_rdma_memreg_strategy;

/*
 * Endpoint calls - xprtrdma/verbs.c
 */
void rpcrdma_flush_disconnect(struct rpcrdma_xprt *r_xprt, struct ib_wc *wc);
int rpcrdma_xprt_connect(struct rpcrdma_xprt *r_xprt);
void rpcrdma_xprt_disconnect(struct rpcrdma_xprt *r_xprt);

int rpcrdma_post_sends(struct rpcrdma_xprt *r_xprt, struct rpcrdma_req *req);
void rpcrdma_post_recvs(struct rpcrdma_xprt *r_xprt, int needed, bool temp);

/*
 * Buffer calls - xprtrdma/verbs.c
 */
struct rpcrdma_req *rpcrdma_req_create(struct rpcrdma_xprt *r_xprt, size_t size,
				       gfp_t flags);
int rpcrdma_req_setup(struct rpcrdma_xprt *r_xprt, struct rpcrdma_req *req);
void rpcrdma_req_destroy(struct rpcrdma_req *req);
int rpcrdma_buffer_create(struct rpcrdma_xprt *);
void rpcrdma_buffer_destroy(struct rpcrdma_buffer *);
struct rpcrdma_sendctx *rpcrdma_sendctx_get_locked(struct rpcrdma_xprt *r_xprt);

struct rpcrdma_mr *rpcrdma_mr_get(struct rpcrdma_xprt *r_xprt);
void rpcrdma_mrs_refresh(struct rpcrdma_xprt *r_xprt);

struct rpcrdma_req *rpcrdma_buffer_get(struct rpcrdma_buffer *);
void rpcrdma_buffer_put(struct rpcrdma_buffer *buffers,
			struct rpcrdma_req *req);
void rpcrdma_rep_put(struct rpcrdma_buffer *buf, struct rpcrdma_rep *rep);
void rpcrdma_reply_put(struct rpcrdma_buffer *buffers, struct rpcrdma_req *req);

bool rpcrdma_regbuf_realloc(struct rpcrdma_regbuf *rb, size_t size,
			    gfp_t flags);
bool __rpcrdma_regbuf_dma_map(struct rpcrdma_xprt *r_xprt,
			      struct rpcrdma_regbuf *rb);

/**
 * rpcrdma_regbuf_is_mapped - check if buffer is DMA mapped
 *
 * Returns true if the buffer is now mapped to rb->rg_device.
 */
static inline bool rpcrdma_regbuf_is_mapped(struct rpcrdma_regbuf *rb)
{
	return rb->rg_device != NULL;
}

/**
 * rpcrdma_regbuf_dma_map - DMA-map a regbuf
 * @r_xprt: controlling transport instance
 * @rb: regbuf to be mapped
 *
 * Returns true if the buffer is currently DMA mapped.
 */
static inline bool rpcrdma_regbuf_dma_map(struct rpcrdma_xprt *r_xprt,
					  struct rpcrdma_regbuf *rb)
{
	if (likely(rpcrdma_regbuf_is_mapped(rb)))
		return true;
	return __rpcrdma_regbuf_dma_map(r_xprt, rb);
}

/*
 * Wrappers for chunk registration, shared by read/write chunk code.
 */

static inline enum dma_data_direction
rpcrdma_data_dir(bool writing)
{
	return writing ? DMA_FROM_DEVICE : DMA_TO_DEVICE;
}

/* Memory registration calls xprtrdma/frwr_ops.c
 */
void frwr_reset(struct rpcrdma_req *req);
int frwr_query_device(struct rpcrdma_ep *ep, const struct ib_device *device);
int frwr_mr_init(struct rpcrdma_xprt *r_xprt, struct rpcrdma_mr *mr);
void frwr_mr_release(struct rpcrdma_mr *mr);
struct rpcrdma_mr_seg *frwr_map(struct rpcrdma_xprt *r_xprt,
				struct rpcrdma_mr_seg *seg,
				int nsegs, bool writing, __be32 xid,
				struct rpcrdma_mr *mr);
int frwr_send(struct rpcrdma_xprt *r_xprt, struct rpcrdma_req *req);
void frwr_reminv(struct rpcrdma_rep *rep, struct list_head *mrs);
void frwr_unmap_sync(struct rpcrdma_xprt *r_xprt, struct rpcrdma_req *req);
void frwr_unmap_async(struct rpcrdma_xprt *r_xprt, struct rpcrdma_req *req);

/*
 * RPC/RDMA protocol calls - xprtrdma/rpc_rdma.c
 */

enum rpcrdma_chunktype {
	rpcrdma_noch = 0,
	rpcrdma_noch_pullup,
	rpcrdma_noch_mapped,
	rpcrdma_readch,
	rpcrdma_areadch,
	rpcrdma_writech,
	rpcrdma_replych
};

int rpcrdma_prepare_send_sges(struct rpcrdma_xprt *r_xprt,
			      struct rpcrdma_req *req, u32 hdrlen,
			      struct xdr_buf *xdr,
			      enum rpcrdma_chunktype rtype);
void rpcrdma_sendctx_unmap(struct rpcrdma_sendctx *sc);
int rpcrdma_marshal_req(struct rpcrdma_xprt *r_xprt, struct rpc_rqst *rqst);
void rpcrdma_set_max_header_sizes(struct rpcrdma_ep *ep);
void rpcrdma_reset_cwnd(struct rpcrdma_xprt *r_xprt);
void rpcrdma_complete_rqst(struct rpcrdma_rep *rep);
void rpcrdma_unpin_rqst(struct rpcrdma_rep *rep);
void rpcrdma_reply_handler(struct rpcrdma_rep *rep);

static inline void rpcrdma_set_xdrlen(struct xdr_buf *xdr, size_t len)
{
	xdr->head[0].iov_len = len;
	xdr->len = len;
}

/* RPC/RDMA module init - xprtrdma/transport.c
 */
extern unsigned int xprt_rdma_max_inline_read;
extern unsigned int xprt_rdma_max_inline_write;
void xprt_rdma_format_addresses(struct rpc_xprt *xprt, struct sockaddr *sap);
void xprt_rdma_free_addresses(struct rpc_xprt *xprt);
void xprt_rdma_close(struct rpc_xprt *xprt);
void xprt_rdma_print_stats(struct rpc_xprt *xprt, struct seq_file *seq);
int xprt_rdma_init(void);
void xprt_rdma_cleanup(void);

/* Backchannel calls - xprtrdma/backchannel.c
 */
#if defined(CONFIG_SUNRPC_BACKCHANNEL)
int xprt_rdma_bc_setup(struct rpc_xprt *, unsigned int);
size_t xprt_rdma_bc_maxpayload(struct rpc_xprt *);
unsigned int xprt_rdma_bc_max_slots(struct rpc_xprt *);
int rpcrdma_bc_post_recv(struct rpcrdma_xprt *, unsigned int);
void rpcrdma_bc_receive_call(struct rpcrdma_xprt *, struct rpcrdma_rep *);
int xprt_rdma_bc_send_reply(struct rpc_rqst *rqst);
void xprt_rdma_bc_free_rqst(struct rpc_rqst *);
void xprt_rdma_bc_destroy(struct rpc_xprt *, unsigned int);
#endif	/* CONFIG_SUNRPC_BACKCHANNEL */

extern struct xprt_class xprt_rdma_bc;

#endif				/* _LINUX_SUNRPC_XPRT_RDMA_H */
