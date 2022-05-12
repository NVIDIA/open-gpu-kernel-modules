/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * iSCSI over TCP/IP Data-Path lib
 *
 * Copyright (C) 2008 Mike Christie
 * Copyright (C) 2008 Red Hat, Inc.  All rights reserved.
 * maintained by open-iscsi@googlegroups.com
 */

#ifndef LIBISCSI_TCP_H
#define LIBISCSI_TCP_H

#include <scsi/libiscsi.h>

struct iscsi_tcp_conn;
struct iscsi_segment;
struct sk_buff;
struct ahash_request;

typedef int iscsi_segment_done_fn_t(struct iscsi_tcp_conn *,
				    struct iscsi_segment *);

struct iscsi_segment {
	unsigned char		*data;
	unsigned int		size;
	unsigned int		copied;
	unsigned int		total_size;
	unsigned int		total_copied;

	struct ahash_request	*hash;
	unsigned char		padbuf[ISCSI_PAD_LEN];
	unsigned char		recv_digest[ISCSI_DIGEST_SIZE];
	unsigned char		digest[ISCSI_DIGEST_SIZE];
	unsigned int		digest_len;

	struct scatterlist	*sg;
	void			*sg_mapped;
	unsigned int		sg_offset;
	bool			atomic_mapped;

	iscsi_segment_done_fn_t	*done;
};

/* Socket connection receive helper */
struct iscsi_tcp_recv {
	struct iscsi_hdr	*hdr;
	struct iscsi_segment	segment;

	/* Allocate buffer for BHS + AHS */
	uint32_t		hdr_buf[64];

	/* copied and flipped values */
	int			datalen;
};

struct iscsi_tcp_conn {
	struct iscsi_conn	*iscsi_conn;
	void			*dd_data;
	int			stop_stage;	/* conn_stop() flag: *
						 * stop to recover,  *
						 * stop to terminate */
	/* control data */
	struct iscsi_tcp_recv	in;		/* TCP receive context */
	/* CRC32C (Rx) LLD should set this is they do not offload */
	struct ahash_request	*rx_hash;
};

struct iscsi_tcp_task {
	uint32_t		exp_datasn;	/* expected target's R2TSN/DataSN */
	int			data_offset;
	struct iscsi_r2t_info	*r2t;		/* in progress solict R2T */
	struct iscsi_pool	r2tpool;
	struct kfifo		r2tqueue;
	void			*dd_data;
	spinlock_t		pool2queue;
	spinlock_t		queue2pool;
};

enum {
	ISCSI_TCP_SEGMENT_DONE,		/* curr seg has been processed */
	ISCSI_TCP_SKB_DONE,		/* skb is out of data */
	ISCSI_TCP_CONN_ERR,		/* iscsi layer has fired a conn err */
	ISCSI_TCP_SUSPENDED,		/* conn is suspended */
};

extern void iscsi_tcp_hdr_recv_prep(struct iscsi_tcp_conn *tcp_conn);
extern int iscsi_tcp_recv_skb(struct iscsi_conn *conn, struct sk_buff *skb,
			      unsigned int offset, bool offloaded, int *status);
extern void iscsi_tcp_cleanup_task(struct iscsi_task *task);
extern int iscsi_tcp_task_init(struct iscsi_task *task);
extern int iscsi_tcp_task_xmit(struct iscsi_task *task);

/* segment helpers */
extern int iscsi_tcp_recv_segment_is_hdr(struct iscsi_tcp_conn *tcp_conn);
extern int iscsi_tcp_segment_done(struct iscsi_tcp_conn *tcp_conn,
				  struct iscsi_segment *segment, int recv,
				  unsigned copied);
extern void iscsi_tcp_segment_unmap(struct iscsi_segment *segment);

extern void iscsi_segment_init_linear(struct iscsi_segment *segment,
				      void *data, size_t size,
				      iscsi_segment_done_fn_t *done,
				      struct ahash_request *hash);
extern int
iscsi_segment_seek_sg(struct iscsi_segment *segment,
		      struct scatterlist *sg_list, unsigned int sg_count,
		      unsigned int offset, size_t size,
		      iscsi_segment_done_fn_t *done,
		      struct ahash_request *hash);

/* digest helpers */
extern void iscsi_tcp_dgst_header(struct ahash_request *hash, const void *hdr,
				  size_t hdrlen,
				  unsigned char digest[ISCSI_DIGEST_SIZE]);
extern struct iscsi_cls_conn *
iscsi_tcp_conn_setup(struct iscsi_cls_session *cls_session, int dd_data_size,
		     uint32_t conn_idx);
extern void iscsi_tcp_conn_teardown(struct iscsi_cls_conn *cls_conn);

/* misc helpers */
extern int iscsi_tcp_r2tpool_alloc(struct iscsi_session *session);
extern void iscsi_tcp_r2tpool_free(struct iscsi_session *session);
extern int iscsi_tcp_set_max_r2t(struct iscsi_conn *conn, char *buf);
extern void iscsi_tcp_conn_get_stats(struct iscsi_cls_conn *cls_conn,
				     struct iscsi_stats *stats);
#endif /* LIBISCSI_TCP_H */
