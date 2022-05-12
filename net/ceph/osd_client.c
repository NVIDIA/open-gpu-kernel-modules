// SPDX-License-Identifier: GPL-2.0

#include <linux/ceph/ceph_debug.h>

#include <linux/module.h>
#include <linux/err.h>
#include <linux/highmem.h>
#include <linux/mm.h>
#include <linux/pagemap.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#ifdef CONFIG_BLOCK
#include <linux/bio.h>
#endif

#include <linux/ceph/ceph_features.h>
#include <linux/ceph/libceph.h>
#include <linux/ceph/osd_client.h>
#include <linux/ceph/messenger.h>
#include <linux/ceph/decode.h>
#include <linux/ceph/auth.h>
#include <linux/ceph/pagelist.h>
#include <linux/ceph/striper.h>

#define OSD_OPREPLY_FRONT_LEN	512

static struct kmem_cache	*ceph_osd_request_cache;

static const struct ceph_connection_operations osd_con_ops;

/*
 * Implement client access to distributed object storage cluster.
 *
 * All data objects are stored within a cluster/cloud of OSDs, or
 * "object storage devices."  (Note that Ceph OSDs have _nothing_ to
 * do with the T10 OSD extensions to SCSI.)  Ceph OSDs are simply
 * remote daemons serving up and coordinating consistent and safe
 * access to storage.
 *
 * Cluster membership and the mapping of data objects onto storage devices
 * are described by the osd map.
 *
 * We keep track of pending OSD requests (read, write), resubmit
 * requests to different OSDs when the cluster topology/data layout
 * change, or retry the affected requests when the communications
 * channel with an OSD is reset.
 */

static void link_request(struct ceph_osd *osd, struct ceph_osd_request *req);
static void unlink_request(struct ceph_osd *osd, struct ceph_osd_request *req);
static void link_linger(struct ceph_osd *osd,
			struct ceph_osd_linger_request *lreq);
static void unlink_linger(struct ceph_osd *osd,
			  struct ceph_osd_linger_request *lreq);
static void clear_backoffs(struct ceph_osd *osd);

#if 1
static inline bool rwsem_is_wrlocked(struct rw_semaphore *sem)
{
	bool wrlocked = true;

	if (unlikely(down_read_trylock(sem))) {
		wrlocked = false;
		up_read(sem);
	}

	return wrlocked;
}
static inline void verify_osdc_locked(struct ceph_osd_client *osdc)
{
	WARN_ON(!rwsem_is_locked(&osdc->lock));
}
static inline void verify_osdc_wrlocked(struct ceph_osd_client *osdc)
{
	WARN_ON(!rwsem_is_wrlocked(&osdc->lock));
}
static inline void verify_osd_locked(struct ceph_osd *osd)
{
	struct ceph_osd_client *osdc = osd->o_osdc;

	WARN_ON(!(mutex_is_locked(&osd->lock) &&
		  rwsem_is_locked(&osdc->lock)) &&
		!rwsem_is_wrlocked(&osdc->lock));
}
static inline void verify_lreq_locked(struct ceph_osd_linger_request *lreq)
{
	WARN_ON(!mutex_is_locked(&lreq->lock));
}
#else
static inline void verify_osdc_locked(struct ceph_osd_client *osdc) { }
static inline void verify_osdc_wrlocked(struct ceph_osd_client *osdc) { }
static inline void verify_osd_locked(struct ceph_osd *osd) { }
static inline void verify_lreq_locked(struct ceph_osd_linger_request *lreq) { }
#endif

/*
 * calculate the mapping of a file extent onto an object, and fill out the
 * request accordingly.  shorten extent as necessary if it crosses an
 * object boundary.
 *
 * fill osd op in request message.
 */
static int calc_layout(struct ceph_file_layout *layout, u64 off, u64 *plen,
			u64 *objnum, u64 *objoff, u64 *objlen)
{
	u64 orig_len = *plen;
	u32 xlen;

	/* object extent? */
	ceph_calc_file_object_mapping(layout, off, orig_len, objnum,
					  objoff, &xlen);
	*objlen = xlen;
	if (*objlen < orig_len) {
		*plen = *objlen;
		dout(" skipping last %llu, final file extent %llu~%llu\n",
		     orig_len - *plen, off, *plen);
	}

	dout("calc_layout objnum=%llx %llu~%llu\n", *objnum, *objoff, *objlen);
	return 0;
}

static void ceph_osd_data_init(struct ceph_osd_data *osd_data)
{
	memset(osd_data, 0, sizeof (*osd_data));
	osd_data->type = CEPH_OSD_DATA_TYPE_NONE;
}

/*
 * Consumes @pages if @own_pages is true.
 */
static void ceph_osd_data_pages_init(struct ceph_osd_data *osd_data,
			struct page **pages, u64 length, u32 alignment,
			bool pages_from_pool, bool own_pages)
{
	osd_data->type = CEPH_OSD_DATA_TYPE_PAGES;
	osd_data->pages = pages;
	osd_data->length = length;
	osd_data->alignment = alignment;
	osd_data->pages_from_pool = pages_from_pool;
	osd_data->own_pages = own_pages;
}

/*
 * Consumes a ref on @pagelist.
 */
static void ceph_osd_data_pagelist_init(struct ceph_osd_data *osd_data,
			struct ceph_pagelist *pagelist)
{
	osd_data->type = CEPH_OSD_DATA_TYPE_PAGELIST;
	osd_data->pagelist = pagelist;
}

#ifdef CONFIG_BLOCK
static void ceph_osd_data_bio_init(struct ceph_osd_data *osd_data,
				   struct ceph_bio_iter *bio_pos,
				   u32 bio_length)
{
	osd_data->type = CEPH_OSD_DATA_TYPE_BIO;
	osd_data->bio_pos = *bio_pos;
	osd_data->bio_length = bio_length;
}
#endif /* CONFIG_BLOCK */

static void ceph_osd_data_bvecs_init(struct ceph_osd_data *osd_data,
				     struct ceph_bvec_iter *bvec_pos,
				     u32 num_bvecs)
{
	osd_data->type = CEPH_OSD_DATA_TYPE_BVECS;
	osd_data->bvec_pos = *bvec_pos;
	osd_data->num_bvecs = num_bvecs;
}

static struct ceph_osd_data *
osd_req_op_raw_data_in(struct ceph_osd_request *osd_req, unsigned int which)
{
	BUG_ON(which >= osd_req->r_num_ops);

	return &osd_req->r_ops[which].raw_data_in;
}

struct ceph_osd_data *
osd_req_op_extent_osd_data(struct ceph_osd_request *osd_req,
			unsigned int which)
{
	return osd_req_op_data(osd_req, which, extent, osd_data);
}
EXPORT_SYMBOL(osd_req_op_extent_osd_data);

void osd_req_op_raw_data_in_pages(struct ceph_osd_request *osd_req,
			unsigned int which, struct page **pages,
			u64 length, u32 alignment,
			bool pages_from_pool, bool own_pages)
{
	struct ceph_osd_data *osd_data;

	osd_data = osd_req_op_raw_data_in(osd_req, which);
	ceph_osd_data_pages_init(osd_data, pages, length, alignment,
				pages_from_pool, own_pages);
}
EXPORT_SYMBOL(osd_req_op_raw_data_in_pages);

void osd_req_op_extent_osd_data_pages(struct ceph_osd_request *osd_req,
			unsigned int which, struct page **pages,
			u64 length, u32 alignment,
			bool pages_from_pool, bool own_pages)
{
	struct ceph_osd_data *osd_data;

	osd_data = osd_req_op_data(osd_req, which, extent, osd_data);
	ceph_osd_data_pages_init(osd_data, pages, length, alignment,
				pages_from_pool, own_pages);
}
EXPORT_SYMBOL(osd_req_op_extent_osd_data_pages);

void osd_req_op_extent_osd_data_pagelist(struct ceph_osd_request *osd_req,
			unsigned int which, struct ceph_pagelist *pagelist)
{
	struct ceph_osd_data *osd_data;

	osd_data = osd_req_op_data(osd_req, which, extent, osd_data);
	ceph_osd_data_pagelist_init(osd_data, pagelist);
}
EXPORT_SYMBOL(osd_req_op_extent_osd_data_pagelist);

#ifdef CONFIG_BLOCK
void osd_req_op_extent_osd_data_bio(struct ceph_osd_request *osd_req,
				    unsigned int which,
				    struct ceph_bio_iter *bio_pos,
				    u32 bio_length)
{
	struct ceph_osd_data *osd_data;

	osd_data = osd_req_op_data(osd_req, which, extent, osd_data);
	ceph_osd_data_bio_init(osd_data, bio_pos, bio_length);
}
EXPORT_SYMBOL(osd_req_op_extent_osd_data_bio);
#endif /* CONFIG_BLOCK */

void osd_req_op_extent_osd_data_bvecs(struct ceph_osd_request *osd_req,
				      unsigned int which,
				      struct bio_vec *bvecs, u32 num_bvecs,
				      u32 bytes)
{
	struct ceph_osd_data *osd_data;
	struct ceph_bvec_iter it = {
		.bvecs = bvecs,
		.iter = { .bi_size = bytes },
	};

	osd_data = osd_req_op_data(osd_req, which, extent, osd_data);
	ceph_osd_data_bvecs_init(osd_data, &it, num_bvecs);
}
EXPORT_SYMBOL(osd_req_op_extent_osd_data_bvecs);

void osd_req_op_extent_osd_data_bvec_pos(struct ceph_osd_request *osd_req,
					 unsigned int which,
					 struct ceph_bvec_iter *bvec_pos)
{
	struct ceph_osd_data *osd_data;

	osd_data = osd_req_op_data(osd_req, which, extent, osd_data);
	ceph_osd_data_bvecs_init(osd_data, bvec_pos, 0);
}
EXPORT_SYMBOL(osd_req_op_extent_osd_data_bvec_pos);

static void osd_req_op_cls_request_info_pagelist(
			struct ceph_osd_request *osd_req,
			unsigned int which, struct ceph_pagelist *pagelist)
{
	struct ceph_osd_data *osd_data;

	osd_data = osd_req_op_data(osd_req, which, cls, request_info);
	ceph_osd_data_pagelist_init(osd_data, pagelist);
}

void osd_req_op_cls_request_data_pagelist(
			struct ceph_osd_request *osd_req,
			unsigned int which, struct ceph_pagelist *pagelist)
{
	struct ceph_osd_data *osd_data;

	osd_data = osd_req_op_data(osd_req, which, cls, request_data);
	ceph_osd_data_pagelist_init(osd_data, pagelist);
	osd_req->r_ops[which].cls.indata_len += pagelist->length;
	osd_req->r_ops[which].indata_len += pagelist->length;
}
EXPORT_SYMBOL(osd_req_op_cls_request_data_pagelist);

void osd_req_op_cls_request_data_pages(struct ceph_osd_request *osd_req,
			unsigned int which, struct page **pages, u64 length,
			u32 alignment, bool pages_from_pool, bool own_pages)
{
	struct ceph_osd_data *osd_data;

	osd_data = osd_req_op_data(osd_req, which, cls, request_data);
	ceph_osd_data_pages_init(osd_data, pages, length, alignment,
				pages_from_pool, own_pages);
	osd_req->r_ops[which].cls.indata_len += length;
	osd_req->r_ops[which].indata_len += length;
}
EXPORT_SYMBOL(osd_req_op_cls_request_data_pages);

void osd_req_op_cls_request_data_bvecs(struct ceph_osd_request *osd_req,
				       unsigned int which,
				       struct bio_vec *bvecs, u32 num_bvecs,
				       u32 bytes)
{
	struct ceph_osd_data *osd_data;
	struct ceph_bvec_iter it = {
		.bvecs = bvecs,
		.iter = { .bi_size = bytes },
	};

	osd_data = osd_req_op_data(osd_req, which, cls, request_data);
	ceph_osd_data_bvecs_init(osd_data, &it, num_bvecs);
	osd_req->r_ops[which].cls.indata_len += bytes;
	osd_req->r_ops[which].indata_len += bytes;
}
EXPORT_SYMBOL(osd_req_op_cls_request_data_bvecs);

void osd_req_op_cls_response_data_pages(struct ceph_osd_request *osd_req,
			unsigned int which, struct page **pages, u64 length,
			u32 alignment, bool pages_from_pool, bool own_pages)
{
	struct ceph_osd_data *osd_data;

	osd_data = osd_req_op_data(osd_req, which, cls, response_data);
	ceph_osd_data_pages_init(osd_data, pages, length, alignment,
				pages_from_pool, own_pages);
}
EXPORT_SYMBOL(osd_req_op_cls_response_data_pages);

static u64 ceph_osd_data_length(struct ceph_osd_data *osd_data)
{
	switch (osd_data->type) {
	case CEPH_OSD_DATA_TYPE_NONE:
		return 0;
	case CEPH_OSD_DATA_TYPE_PAGES:
		return osd_data->length;
	case CEPH_OSD_DATA_TYPE_PAGELIST:
		return (u64)osd_data->pagelist->length;
#ifdef CONFIG_BLOCK
	case CEPH_OSD_DATA_TYPE_BIO:
		return (u64)osd_data->bio_length;
#endif /* CONFIG_BLOCK */
	case CEPH_OSD_DATA_TYPE_BVECS:
		return osd_data->bvec_pos.iter.bi_size;
	default:
		WARN(true, "unrecognized data type %d\n", (int)osd_data->type);
		return 0;
	}
}

static void ceph_osd_data_release(struct ceph_osd_data *osd_data)
{
	if (osd_data->type == CEPH_OSD_DATA_TYPE_PAGES && osd_data->own_pages) {
		int num_pages;

		num_pages = calc_pages_for((u64)osd_data->alignment,
						(u64)osd_data->length);
		ceph_release_page_vector(osd_data->pages, num_pages);
	} else if (osd_data->type == CEPH_OSD_DATA_TYPE_PAGELIST) {
		ceph_pagelist_release(osd_data->pagelist);
	}
	ceph_osd_data_init(osd_data);
}

static void osd_req_op_data_release(struct ceph_osd_request *osd_req,
			unsigned int which)
{
	struct ceph_osd_req_op *op;

	BUG_ON(which >= osd_req->r_num_ops);
	op = &osd_req->r_ops[which];

	switch (op->op) {
	case CEPH_OSD_OP_READ:
	case CEPH_OSD_OP_WRITE:
	case CEPH_OSD_OP_WRITEFULL:
		ceph_osd_data_release(&op->extent.osd_data);
		break;
	case CEPH_OSD_OP_CALL:
		ceph_osd_data_release(&op->cls.request_info);
		ceph_osd_data_release(&op->cls.request_data);
		ceph_osd_data_release(&op->cls.response_data);
		break;
	case CEPH_OSD_OP_SETXATTR:
	case CEPH_OSD_OP_CMPXATTR:
		ceph_osd_data_release(&op->xattr.osd_data);
		break;
	case CEPH_OSD_OP_STAT:
		ceph_osd_data_release(&op->raw_data_in);
		break;
	case CEPH_OSD_OP_NOTIFY_ACK:
		ceph_osd_data_release(&op->notify_ack.request_data);
		break;
	case CEPH_OSD_OP_NOTIFY:
		ceph_osd_data_release(&op->notify.request_data);
		ceph_osd_data_release(&op->notify.response_data);
		break;
	case CEPH_OSD_OP_LIST_WATCHERS:
		ceph_osd_data_release(&op->list_watchers.response_data);
		break;
	case CEPH_OSD_OP_COPY_FROM2:
		ceph_osd_data_release(&op->copy_from.osd_data);
		break;
	default:
		break;
	}
}

/*
 * Assumes @t is zero-initialized.
 */
static void target_init(struct ceph_osd_request_target *t)
{
	ceph_oid_init(&t->base_oid);
	ceph_oloc_init(&t->base_oloc);
	ceph_oid_init(&t->target_oid);
	ceph_oloc_init(&t->target_oloc);

	ceph_osds_init(&t->acting);
	ceph_osds_init(&t->up);
	t->size = -1;
	t->min_size = -1;

	t->osd = CEPH_HOMELESS_OSD;
}

static void target_copy(struct ceph_osd_request_target *dest,
			const struct ceph_osd_request_target *src)
{
	ceph_oid_copy(&dest->base_oid, &src->base_oid);
	ceph_oloc_copy(&dest->base_oloc, &src->base_oloc);
	ceph_oid_copy(&dest->target_oid, &src->target_oid);
	ceph_oloc_copy(&dest->target_oloc, &src->target_oloc);

	dest->pgid = src->pgid; /* struct */
	dest->spgid = src->spgid; /* struct */
	dest->pg_num = src->pg_num;
	dest->pg_num_mask = src->pg_num_mask;
	ceph_osds_copy(&dest->acting, &src->acting);
	ceph_osds_copy(&dest->up, &src->up);
	dest->size = src->size;
	dest->min_size = src->min_size;
	dest->sort_bitwise = src->sort_bitwise;
	dest->recovery_deletes = src->recovery_deletes;

	dest->flags = src->flags;
	dest->used_replica = src->used_replica;
	dest->paused = src->paused;

	dest->epoch = src->epoch;
	dest->last_force_resend = src->last_force_resend;

	dest->osd = src->osd;
}

static void target_destroy(struct ceph_osd_request_target *t)
{
	ceph_oid_destroy(&t->base_oid);
	ceph_oloc_destroy(&t->base_oloc);
	ceph_oid_destroy(&t->target_oid);
	ceph_oloc_destroy(&t->target_oloc);
}

/*
 * requests
 */
static void request_release_checks(struct ceph_osd_request *req)
{
	WARN_ON(!RB_EMPTY_NODE(&req->r_node));
	WARN_ON(!RB_EMPTY_NODE(&req->r_mc_node));
	WARN_ON(!list_empty(&req->r_private_item));
	WARN_ON(req->r_osd);
}

static void ceph_osdc_release_request(struct kref *kref)
{
	struct ceph_osd_request *req = container_of(kref,
					    struct ceph_osd_request, r_kref);
	unsigned int which;

	dout("%s %p (r_request %p r_reply %p)\n", __func__, req,
	     req->r_request, req->r_reply);
	request_release_checks(req);

	if (req->r_request)
		ceph_msg_put(req->r_request);
	if (req->r_reply)
		ceph_msg_put(req->r_reply);

	for (which = 0; which < req->r_num_ops; which++)
		osd_req_op_data_release(req, which);

	target_destroy(&req->r_t);
	ceph_put_snap_context(req->r_snapc);

	if (req->r_mempool)
		mempool_free(req, req->r_osdc->req_mempool);
	else if (req->r_num_ops <= CEPH_OSD_SLAB_OPS)
		kmem_cache_free(ceph_osd_request_cache, req);
	else
		kfree(req);
}

void ceph_osdc_get_request(struct ceph_osd_request *req)
{
	dout("%s %p (was %d)\n", __func__, req,
	     kref_read(&req->r_kref));
	kref_get(&req->r_kref);
}
EXPORT_SYMBOL(ceph_osdc_get_request);

void ceph_osdc_put_request(struct ceph_osd_request *req)
{
	if (req) {
		dout("%s %p (was %d)\n", __func__, req,
		     kref_read(&req->r_kref));
		kref_put(&req->r_kref, ceph_osdc_release_request);
	}
}
EXPORT_SYMBOL(ceph_osdc_put_request);

static void request_init(struct ceph_osd_request *req)
{
	/* req only, each op is zeroed in osd_req_op_init() */
	memset(req, 0, sizeof(*req));

	kref_init(&req->r_kref);
	init_completion(&req->r_completion);
	RB_CLEAR_NODE(&req->r_node);
	RB_CLEAR_NODE(&req->r_mc_node);
	INIT_LIST_HEAD(&req->r_private_item);

	target_init(&req->r_t);
}

/*
 * This is ugly, but it allows us to reuse linger registration and ping
 * requests, keeping the structure of the code around send_linger{_ping}()
 * reasonable.  Setting up a min_nr=2 mempool for each linger request
 * and dealing with copying ops (this blasts req only, watch op remains
 * intact) isn't any better.
 */
static void request_reinit(struct ceph_osd_request *req)
{
	struct ceph_osd_client *osdc = req->r_osdc;
	bool mempool = req->r_mempool;
	unsigned int num_ops = req->r_num_ops;
	u64 snapid = req->r_snapid;
	struct ceph_snap_context *snapc = req->r_snapc;
	bool linger = req->r_linger;
	struct ceph_msg *request_msg = req->r_request;
	struct ceph_msg *reply_msg = req->r_reply;

	dout("%s req %p\n", __func__, req);
	WARN_ON(kref_read(&req->r_kref) != 1);
	request_release_checks(req);

	WARN_ON(kref_read(&request_msg->kref) != 1);
	WARN_ON(kref_read(&reply_msg->kref) != 1);
	target_destroy(&req->r_t);

	request_init(req);
	req->r_osdc = osdc;
	req->r_mempool = mempool;
	req->r_num_ops = num_ops;
	req->r_snapid = snapid;
	req->r_snapc = snapc;
	req->r_linger = linger;
	req->r_request = request_msg;
	req->r_reply = reply_msg;
}

struct ceph_osd_request *ceph_osdc_alloc_request(struct ceph_osd_client *osdc,
					       struct ceph_snap_context *snapc,
					       unsigned int num_ops,
					       bool use_mempool,
					       gfp_t gfp_flags)
{
	struct ceph_osd_request *req;

	if (use_mempool) {
		BUG_ON(num_ops > CEPH_OSD_SLAB_OPS);
		req = mempool_alloc(osdc->req_mempool, gfp_flags);
	} else if (num_ops <= CEPH_OSD_SLAB_OPS) {
		req = kmem_cache_alloc(ceph_osd_request_cache, gfp_flags);
	} else {
		BUG_ON(num_ops > CEPH_OSD_MAX_OPS);
		req = kmalloc(struct_size(req, r_ops, num_ops), gfp_flags);
	}
	if (unlikely(!req))
		return NULL;

	request_init(req);
	req->r_osdc = osdc;
	req->r_mempool = use_mempool;
	req->r_num_ops = num_ops;
	req->r_snapid = CEPH_NOSNAP;
	req->r_snapc = ceph_get_snap_context(snapc);

	dout("%s req %p\n", __func__, req);
	return req;
}
EXPORT_SYMBOL(ceph_osdc_alloc_request);

static int ceph_oloc_encoding_size(const struct ceph_object_locator *oloc)
{
	return 8 + 4 + 4 + 4 + (oloc->pool_ns ? oloc->pool_ns->len : 0);
}

static int __ceph_osdc_alloc_messages(struct ceph_osd_request *req, gfp_t gfp,
				      int num_request_data_items,
				      int num_reply_data_items)
{
	struct ceph_osd_client *osdc = req->r_osdc;
	struct ceph_msg *msg;
	int msg_size;

	WARN_ON(req->r_request || req->r_reply);
	WARN_ON(ceph_oid_empty(&req->r_base_oid));
	WARN_ON(ceph_oloc_empty(&req->r_base_oloc));

	/* create request message */
	msg_size = CEPH_ENCODING_START_BLK_LEN +
			CEPH_PGID_ENCODING_LEN + 1; /* spgid */
	msg_size += 4 + 4 + 4; /* hash, osdmap_epoch, flags */
	msg_size += CEPH_ENCODING_START_BLK_LEN +
			sizeof(struct ceph_osd_reqid); /* reqid */
	msg_size += sizeof(struct ceph_blkin_trace_info); /* trace */
	msg_size += 4 + sizeof(struct ceph_timespec); /* client_inc, mtime */
	msg_size += CEPH_ENCODING_START_BLK_LEN +
			ceph_oloc_encoding_size(&req->r_base_oloc); /* oloc */
	msg_size += 4 + req->r_base_oid.name_len; /* oid */
	msg_size += 2 + req->r_num_ops * sizeof(struct ceph_osd_op);
	msg_size += 8; /* snapid */
	msg_size += 8; /* snap_seq */
	msg_size += 4 + 8 * (req->r_snapc ? req->r_snapc->num_snaps : 0);
	msg_size += 4 + 8; /* retry_attempt, features */

	if (req->r_mempool)
		msg = ceph_msgpool_get(&osdc->msgpool_op, msg_size,
				       num_request_data_items);
	else
		msg = ceph_msg_new2(CEPH_MSG_OSD_OP, msg_size,
				    num_request_data_items, gfp, true);
	if (!msg)
		return -ENOMEM;

	memset(msg->front.iov_base, 0, msg->front.iov_len);
	req->r_request = msg;

	/* create reply message */
	msg_size = OSD_OPREPLY_FRONT_LEN;
	msg_size += req->r_base_oid.name_len;
	msg_size += req->r_num_ops * sizeof(struct ceph_osd_op);

	if (req->r_mempool)
		msg = ceph_msgpool_get(&osdc->msgpool_op_reply, msg_size,
				       num_reply_data_items);
	else
		msg = ceph_msg_new2(CEPH_MSG_OSD_OPREPLY, msg_size,
				    num_reply_data_items, gfp, true);
	if (!msg)
		return -ENOMEM;

	req->r_reply = msg;

	return 0;
}

static bool osd_req_opcode_valid(u16 opcode)
{
	switch (opcode) {
#define GENERATE_CASE(op, opcode, str)	case CEPH_OSD_OP_##op: return true;
__CEPH_FORALL_OSD_OPS(GENERATE_CASE)
#undef GENERATE_CASE
	default:
		return false;
	}
}

static void get_num_data_items(struct ceph_osd_request *req,
			       int *num_request_data_items,
			       int *num_reply_data_items)
{
	struct ceph_osd_req_op *op;

	*num_request_data_items = 0;
	*num_reply_data_items = 0;

	for (op = req->r_ops; op != &req->r_ops[req->r_num_ops]; op++) {
		switch (op->op) {
		/* request */
		case CEPH_OSD_OP_WRITE:
		case CEPH_OSD_OP_WRITEFULL:
		case CEPH_OSD_OP_SETXATTR:
		case CEPH_OSD_OP_CMPXATTR:
		case CEPH_OSD_OP_NOTIFY_ACK:
		case CEPH_OSD_OP_COPY_FROM2:
			*num_request_data_items += 1;
			break;

		/* reply */
		case CEPH_OSD_OP_STAT:
		case CEPH_OSD_OP_READ:
		case CEPH_OSD_OP_LIST_WATCHERS:
			*num_reply_data_items += 1;
			break;

		/* both */
		case CEPH_OSD_OP_NOTIFY:
			*num_request_data_items += 1;
			*num_reply_data_items += 1;
			break;
		case CEPH_OSD_OP_CALL:
			*num_request_data_items += 2;
			*num_reply_data_items += 1;
			break;

		default:
			WARN_ON(!osd_req_opcode_valid(op->op));
			break;
		}
	}
}

/*
 * oid, oloc and OSD op opcode(s) must be filled in before this function
 * is called.
 */
int ceph_osdc_alloc_messages(struct ceph_osd_request *req, gfp_t gfp)
{
	int num_request_data_items, num_reply_data_items;

	get_num_data_items(req, &num_request_data_items, &num_reply_data_items);
	return __ceph_osdc_alloc_messages(req, gfp, num_request_data_items,
					  num_reply_data_items);
}
EXPORT_SYMBOL(ceph_osdc_alloc_messages);

/*
 * This is an osd op init function for opcodes that have no data or
 * other information associated with them.  It also serves as a
 * common init routine for all the other init functions, below.
 */
struct ceph_osd_req_op *
osd_req_op_init(struct ceph_osd_request *osd_req, unsigned int which,
		 u16 opcode, u32 flags)
{
	struct ceph_osd_req_op *op;

	BUG_ON(which >= osd_req->r_num_ops);
	BUG_ON(!osd_req_opcode_valid(opcode));

	op = &osd_req->r_ops[which];
	memset(op, 0, sizeof (*op));
	op->op = opcode;
	op->flags = flags;

	return op;
}
EXPORT_SYMBOL(osd_req_op_init);

void osd_req_op_extent_init(struct ceph_osd_request *osd_req,
				unsigned int which, u16 opcode,
				u64 offset, u64 length,
				u64 truncate_size, u32 truncate_seq)
{
	struct ceph_osd_req_op *op = osd_req_op_init(osd_req, which,
						     opcode, 0);
	size_t payload_len = 0;

	BUG_ON(opcode != CEPH_OSD_OP_READ && opcode != CEPH_OSD_OP_WRITE &&
	       opcode != CEPH_OSD_OP_WRITEFULL && opcode != CEPH_OSD_OP_ZERO &&
	       opcode != CEPH_OSD_OP_TRUNCATE);

	op->extent.offset = offset;
	op->extent.length = length;
	op->extent.truncate_size = truncate_size;
	op->extent.truncate_seq = truncate_seq;
	if (opcode == CEPH_OSD_OP_WRITE || opcode == CEPH_OSD_OP_WRITEFULL)
		payload_len += length;

	op->indata_len = payload_len;
}
EXPORT_SYMBOL(osd_req_op_extent_init);

void osd_req_op_extent_update(struct ceph_osd_request *osd_req,
				unsigned int which, u64 length)
{
	struct ceph_osd_req_op *op;
	u64 previous;

	BUG_ON(which >= osd_req->r_num_ops);
	op = &osd_req->r_ops[which];
	previous = op->extent.length;

	if (length == previous)
		return;		/* Nothing to do */
	BUG_ON(length > previous);

	op->extent.length = length;
	if (op->op == CEPH_OSD_OP_WRITE || op->op == CEPH_OSD_OP_WRITEFULL)
		op->indata_len -= previous - length;
}
EXPORT_SYMBOL(osd_req_op_extent_update);

void osd_req_op_extent_dup_last(struct ceph_osd_request *osd_req,
				unsigned int which, u64 offset_inc)
{
	struct ceph_osd_req_op *op, *prev_op;

	BUG_ON(which + 1 >= osd_req->r_num_ops);

	prev_op = &osd_req->r_ops[which];
	op = osd_req_op_init(osd_req, which + 1, prev_op->op, prev_op->flags);
	/* dup previous one */
	op->indata_len = prev_op->indata_len;
	op->outdata_len = prev_op->outdata_len;
	op->extent = prev_op->extent;
	/* adjust offset */
	op->extent.offset += offset_inc;
	op->extent.length -= offset_inc;

	if (op->op == CEPH_OSD_OP_WRITE || op->op == CEPH_OSD_OP_WRITEFULL)
		op->indata_len -= offset_inc;
}
EXPORT_SYMBOL(osd_req_op_extent_dup_last);

int osd_req_op_cls_init(struct ceph_osd_request *osd_req, unsigned int which,
			const char *class, const char *method)
{
	struct ceph_osd_req_op *op;
	struct ceph_pagelist *pagelist;
	size_t payload_len = 0;
	size_t size;
	int ret;

	op = osd_req_op_init(osd_req, which, CEPH_OSD_OP_CALL, 0);

	pagelist = ceph_pagelist_alloc(GFP_NOFS);
	if (!pagelist)
		return -ENOMEM;

	op->cls.class_name = class;
	size = strlen(class);
	BUG_ON(size > (size_t) U8_MAX);
	op->cls.class_len = size;
	ret = ceph_pagelist_append(pagelist, class, size);
	if (ret)
		goto err_pagelist_free;
	payload_len += size;

	op->cls.method_name = method;
	size = strlen(method);
	BUG_ON(size > (size_t) U8_MAX);
	op->cls.method_len = size;
	ret = ceph_pagelist_append(pagelist, method, size);
	if (ret)
		goto err_pagelist_free;
	payload_len += size;

	osd_req_op_cls_request_info_pagelist(osd_req, which, pagelist);
	op->indata_len = payload_len;
	return 0;

err_pagelist_free:
	ceph_pagelist_release(pagelist);
	return ret;
}
EXPORT_SYMBOL(osd_req_op_cls_init);

int osd_req_op_xattr_init(struct ceph_osd_request *osd_req, unsigned int which,
			  u16 opcode, const char *name, const void *value,
			  size_t size, u8 cmp_op, u8 cmp_mode)
{
	struct ceph_osd_req_op *op = osd_req_op_init(osd_req, which,
						     opcode, 0);
	struct ceph_pagelist *pagelist;
	size_t payload_len;
	int ret;

	BUG_ON(opcode != CEPH_OSD_OP_SETXATTR && opcode != CEPH_OSD_OP_CMPXATTR);

	pagelist = ceph_pagelist_alloc(GFP_NOFS);
	if (!pagelist)
		return -ENOMEM;

	payload_len = strlen(name);
	op->xattr.name_len = payload_len;
	ret = ceph_pagelist_append(pagelist, name, payload_len);
	if (ret)
		goto err_pagelist_free;

	op->xattr.value_len = size;
	ret = ceph_pagelist_append(pagelist, value, size);
	if (ret)
		goto err_pagelist_free;
	payload_len += size;

	op->xattr.cmp_op = cmp_op;
	op->xattr.cmp_mode = cmp_mode;

	ceph_osd_data_pagelist_init(&op->xattr.osd_data, pagelist);
	op->indata_len = payload_len;
	return 0;

err_pagelist_free:
	ceph_pagelist_release(pagelist);
	return ret;
}
EXPORT_SYMBOL(osd_req_op_xattr_init);

/*
 * @watch_opcode: CEPH_OSD_WATCH_OP_*
 */
static void osd_req_op_watch_init(struct ceph_osd_request *req, int which,
				  u64 cookie, u8 watch_opcode)
{
	struct ceph_osd_req_op *op;

	op = osd_req_op_init(req, which, CEPH_OSD_OP_WATCH, 0);
	op->watch.cookie = cookie;
	op->watch.op = watch_opcode;
	op->watch.gen = 0;
}

/*
 * @flags: CEPH_OSD_OP_ALLOC_HINT_FLAG_*
 */
void osd_req_op_alloc_hint_init(struct ceph_osd_request *osd_req,
				unsigned int which,
				u64 expected_object_size,
				u64 expected_write_size,
				u32 flags)
{
	struct ceph_osd_req_op *op;

	op = osd_req_op_init(osd_req, which, CEPH_OSD_OP_SETALLOCHINT, 0);
	op->alloc_hint.expected_object_size = expected_object_size;
	op->alloc_hint.expected_write_size = expected_write_size;
	op->alloc_hint.flags = flags;

	/*
	 * CEPH_OSD_OP_SETALLOCHINT op is advisory and therefore deemed
	 * not worth a feature bit.  Set FAILOK per-op flag to make
	 * sure older osds don't trip over an unsupported opcode.
	 */
	op->flags |= CEPH_OSD_OP_FLAG_FAILOK;
}
EXPORT_SYMBOL(osd_req_op_alloc_hint_init);

static void ceph_osdc_msg_data_add(struct ceph_msg *msg,
				struct ceph_osd_data *osd_data)
{
	u64 length = ceph_osd_data_length(osd_data);

	if (osd_data->type == CEPH_OSD_DATA_TYPE_PAGES) {
		BUG_ON(length > (u64) SIZE_MAX);
		if (length)
			ceph_msg_data_add_pages(msg, osd_data->pages,
					length, osd_data->alignment, false);
	} else if (osd_data->type == CEPH_OSD_DATA_TYPE_PAGELIST) {
		BUG_ON(!length);
		ceph_msg_data_add_pagelist(msg, osd_data->pagelist);
#ifdef CONFIG_BLOCK
	} else if (osd_data->type == CEPH_OSD_DATA_TYPE_BIO) {
		ceph_msg_data_add_bio(msg, &osd_data->bio_pos, length);
#endif
	} else if (osd_data->type == CEPH_OSD_DATA_TYPE_BVECS) {
		ceph_msg_data_add_bvecs(msg, &osd_data->bvec_pos);
	} else {
		BUG_ON(osd_data->type != CEPH_OSD_DATA_TYPE_NONE);
	}
}

static u32 osd_req_encode_op(struct ceph_osd_op *dst,
			     const struct ceph_osd_req_op *src)
{
	switch (src->op) {
	case CEPH_OSD_OP_STAT:
		break;
	case CEPH_OSD_OP_READ:
	case CEPH_OSD_OP_WRITE:
	case CEPH_OSD_OP_WRITEFULL:
	case CEPH_OSD_OP_ZERO:
	case CEPH_OSD_OP_TRUNCATE:
		dst->extent.offset = cpu_to_le64(src->extent.offset);
		dst->extent.length = cpu_to_le64(src->extent.length);
		dst->extent.truncate_size =
			cpu_to_le64(src->extent.truncate_size);
		dst->extent.truncate_seq =
			cpu_to_le32(src->extent.truncate_seq);
		break;
	case CEPH_OSD_OP_CALL:
		dst->cls.class_len = src->cls.class_len;
		dst->cls.method_len = src->cls.method_len;
		dst->cls.indata_len = cpu_to_le32(src->cls.indata_len);
		break;
	case CEPH_OSD_OP_WATCH:
		dst->watch.cookie = cpu_to_le64(src->watch.cookie);
		dst->watch.ver = cpu_to_le64(0);
		dst->watch.op = src->watch.op;
		dst->watch.gen = cpu_to_le32(src->watch.gen);
		break;
	case CEPH_OSD_OP_NOTIFY_ACK:
		break;
	case CEPH_OSD_OP_NOTIFY:
		dst->notify.cookie = cpu_to_le64(src->notify.cookie);
		break;
	case CEPH_OSD_OP_LIST_WATCHERS:
		break;
	case CEPH_OSD_OP_SETALLOCHINT:
		dst->alloc_hint.expected_object_size =
		    cpu_to_le64(src->alloc_hint.expected_object_size);
		dst->alloc_hint.expected_write_size =
		    cpu_to_le64(src->alloc_hint.expected_write_size);
		dst->alloc_hint.flags = cpu_to_le32(src->alloc_hint.flags);
		break;
	case CEPH_OSD_OP_SETXATTR:
	case CEPH_OSD_OP_CMPXATTR:
		dst->xattr.name_len = cpu_to_le32(src->xattr.name_len);
		dst->xattr.value_len = cpu_to_le32(src->xattr.value_len);
		dst->xattr.cmp_op = src->xattr.cmp_op;
		dst->xattr.cmp_mode = src->xattr.cmp_mode;
		break;
	case CEPH_OSD_OP_CREATE:
	case CEPH_OSD_OP_DELETE:
		break;
	case CEPH_OSD_OP_COPY_FROM2:
		dst->copy_from.snapid = cpu_to_le64(src->copy_from.snapid);
		dst->copy_from.src_version =
			cpu_to_le64(src->copy_from.src_version);
		dst->copy_from.flags = src->copy_from.flags;
		dst->copy_from.src_fadvise_flags =
			cpu_to_le32(src->copy_from.src_fadvise_flags);
		break;
	default:
		pr_err("unsupported osd opcode %s\n",
			ceph_osd_op_name(src->op));
		WARN_ON(1);

		return 0;
	}

	dst->op = cpu_to_le16(src->op);
	dst->flags = cpu_to_le32(src->flags);
	dst->payload_len = cpu_to_le32(src->indata_len);

	return src->indata_len;
}

/*
 * build new request AND message, calculate layout, and adjust file
 * extent as needed.
 *
 * if the file was recently truncated, we include information about its
 * old and new size so that the object can be updated appropriately.  (we
 * avoid synchronously deleting truncated objects because it's slow.)
 */
struct ceph_osd_request *ceph_osdc_new_request(struct ceph_osd_client *osdc,
					       struct ceph_file_layout *layout,
					       struct ceph_vino vino,
					       u64 off, u64 *plen,
					       unsigned int which, int num_ops,
					       int opcode, int flags,
					       struct ceph_snap_context *snapc,
					       u32 truncate_seq,
					       u64 truncate_size,
					       bool use_mempool)
{
	struct ceph_osd_request *req;
	u64 objnum = 0;
	u64 objoff = 0;
	u64 objlen = 0;
	int r;

	BUG_ON(opcode != CEPH_OSD_OP_READ && opcode != CEPH_OSD_OP_WRITE &&
	       opcode != CEPH_OSD_OP_ZERO && opcode != CEPH_OSD_OP_TRUNCATE &&
	       opcode != CEPH_OSD_OP_CREATE && opcode != CEPH_OSD_OP_DELETE);

	req = ceph_osdc_alloc_request(osdc, snapc, num_ops, use_mempool,
					GFP_NOFS);
	if (!req) {
		r = -ENOMEM;
		goto fail;
	}

	/* calculate max write size */
	r = calc_layout(layout, off, plen, &objnum, &objoff, &objlen);
	if (r)
		goto fail;

	if (opcode == CEPH_OSD_OP_CREATE || opcode == CEPH_OSD_OP_DELETE) {
		osd_req_op_init(req, which, opcode, 0);
	} else {
		u32 object_size = layout->object_size;
		u32 object_base = off - objoff;
		if (!(truncate_seq == 1 && truncate_size == -1ULL)) {
			if (truncate_size <= object_base) {
				truncate_size = 0;
			} else {
				truncate_size -= object_base;
				if (truncate_size > object_size)
					truncate_size = object_size;
			}
		}
		osd_req_op_extent_init(req, which, opcode, objoff, objlen,
				       truncate_size, truncate_seq);
	}

	req->r_base_oloc.pool = layout->pool_id;
	req->r_base_oloc.pool_ns = ceph_try_get_string(layout->pool_ns);
	ceph_oid_printf(&req->r_base_oid, "%llx.%08llx", vino.ino, objnum);
	req->r_flags = flags | osdc->client->options->read_from_replica;

	req->r_snapid = vino.snap;
	if (flags & CEPH_OSD_FLAG_WRITE)
		req->r_data_offset = off;

	if (num_ops > 1)
		/*
		 * This is a special case for ceph_writepages_start(), but it
		 * also covers ceph_uninline_data().  If more multi-op request
		 * use cases emerge, we will need a separate helper.
		 */
		r = __ceph_osdc_alloc_messages(req, GFP_NOFS, num_ops, 0);
	else
		r = ceph_osdc_alloc_messages(req, GFP_NOFS);
	if (r)
		goto fail;

	return req;

fail:
	ceph_osdc_put_request(req);
	return ERR_PTR(r);
}
EXPORT_SYMBOL(ceph_osdc_new_request);

/*
 * We keep osd requests in an rbtree, sorted by ->r_tid.
 */
DEFINE_RB_FUNCS(request, struct ceph_osd_request, r_tid, r_node)
DEFINE_RB_FUNCS(request_mc, struct ceph_osd_request, r_tid, r_mc_node)

/*
 * Call @fn on each OSD request as long as @fn returns 0.
 */
static void for_each_request(struct ceph_osd_client *osdc,
			int (*fn)(struct ceph_osd_request *req, void *arg),
			void *arg)
{
	struct rb_node *n, *p;

	for (n = rb_first(&osdc->osds); n; n = rb_next(n)) {
		struct ceph_osd *osd = rb_entry(n, struct ceph_osd, o_node);

		for (p = rb_first(&osd->o_requests); p; ) {
			struct ceph_osd_request *req =
			    rb_entry(p, struct ceph_osd_request, r_node);

			p = rb_next(p);
			if (fn(req, arg))
				return;
		}
	}

	for (p = rb_first(&osdc->homeless_osd.o_requests); p; ) {
		struct ceph_osd_request *req =
		    rb_entry(p, struct ceph_osd_request, r_node);

		p = rb_next(p);
		if (fn(req, arg))
			return;
	}
}

static bool osd_homeless(struct ceph_osd *osd)
{
	return osd->o_osd == CEPH_HOMELESS_OSD;
}

static bool osd_registered(struct ceph_osd *osd)
{
	verify_osdc_locked(osd->o_osdc);

	return !RB_EMPTY_NODE(&osd->o_node);
}

/*
 * Assumes @osd is zero-initialized.
 */
static void osd_init(struct ceph_osd *osd)
{
	refcount_set(&osd->o_ref, 1);
	RB_CLEAR_NODE(&osd->o_node);
	osd->o_requests = RB_ROOT;
	osd->o_linger_requests = RB_ROOT;
	osd->o_backoff_mappings = RB_ROOT;
	osd->o_backoffs_by_id = RB_ROOT;
	INIT_LIST_HEAD(&osd->o_osd_lru);
	INIT_LIST_HEAD(&osd->o_keepalive_item);
	osd->o_incarnation = 1;
	mutex_init(&osd->lock);
}

static void osd_cleanup(struct ceph_osd *osd)
{
	WARN_ON(!RB_EMPTY_NODE(&osd->o_node));
	WARN_ON(!RB_EMPTY_ROOT(&osd->o_requests));
	WARN_ON(!RB_EMPTY_ROOT(&osd->o_linger_requests));
	WARN_ON(!RB_EMPTY_ROOT(&osd->o_backoff_mappings));
	WARN_ON(!RB_EMPTY_ROOT(&osd->o_backoffs_by_id));
	WARN_ON(!list_empty(&osd->o_osd_lru));
	WARN_ON(!list_empty(&osd->o_keepalive_item));

	if (osd->o_auth.authorizer) {
		WARN_ON(osd_homeless(osd));
		ceph_auth_destroy_authorizer(osd->o_auth.authorizer);
	}
}

/*
 * Track open sessions with osds.
 */
static struct ceph_osd *create_osd(struct ceph_osd_client *osdc, int onum)
{
	struct ceph_osd *osd;

	WARN_ON(onum == CEPH_HOMELESS_OSD);

	osd = kzalloc(sizeof(*osd), GFP_NOIO | __GFP_NOFAIL);
	osd_init(osd);
	osd->o_osdc = osdc;
	osd->o_osd = onum;

	ceph_con_init(&osd->o_con, osd, &osd_con_ops, &osdc->client->msgr);

	return osd;
}

static struct ceph_osd *get_osd(struct ceph_osd *osd)
{
	if (refcount_inc_not_zero(&osd->o_ref)) {
		dout("get_osd %p %d -> %d\n", osd, refcount_read(&osd->o_ref)-1,
		     refcount_read(&osd->o_ref));
		return osd;
	} else {
		dout("get_osd %p FAIL\n", osd);
		return NULL;
	}
}

static void put_osd(struct ceph_osd *osd)
{
	dout("put_osd %p %d -> %d\n", osd, refcount_read(&osd->o_ref),
	     refcount_read(&osd->o_ref) - 1);
	if (refcount_dec_and_test(&osd->o_ref)) {
		osd_cleanup(osd);
		kfree(osd);
	}
}

DEFINE_RB_FUNCS(osd, struct ceph_osd, o_osd, o_node)

static void __move_osd_to_lru(struct ceph_osd *osd)
{
	struct ceph_osd_client *osdc = osd->o_osdc;

	dout("%s osd %p osd%d\n", __func__, osd, osd->o_osd);
	BUG_ON(!list_empty(&osd->o_osd_lru));

	spin_lock(&osdc->osd_lru_lock);
	list_add_tail(&osd->o_osd_lru, &osdc->osd_lru);
	spin_unlock(&osdc->osd_lru_lock);

	osd->lru_ttl = jiffies + osdc->client->options->osd_idle_ttl;
}

static void maybe_move_osd_to_lru(struct ceph_osd *osd)
{
	if (RB_EMPTY_ROOT(&osd->o_requests) &&
	    RB_EMPTY_ROOT(&osd->o_linger_requests))
		__move_osd_to_lru(osd);
}

static void __remove_osd_from_lru(struct ceph_osd *osd)
{
	struct ceph_osd_client *osdc = osd->o_osdc;

	dout("%s osd %p osd%d\n", __func__, osd, osd->o_osd);

	spin_lock(&osdc->osd_lru_lock);
	if (!list_empty(&osd->o_osd_lru))
		list_del_init(&osd->o_osd_lru);
	spin_unlock(&osdc->osd_lru_lock);
}

/*
 * Close the connection and assign any leftover requests to the
 * homeless session.
 */
static void close_osd(struct ceph_osd *osd)
{
	struct ceph_osd_client *osdc = osd->o_osdc;
	struct rb_node *n;

	verify_osdc_wrlocked(osdc);
	dout("%s osd %p osd%d\n", __func__, osd, osd->o_osd);

	ceph_con_close(&osd->o_con);

	for (n = rb_first(&osd->o_requests); n; ) {
		struct ceph_osd_request *req =
		    rb_entry(n, struct ceph_osd_request, r_node);

		n = rb_next(n); /* unlink_request() */

		dout(" reassigning req %p tid %llu\n", req, req->r_tid);
		unlink_request(osd, req);
		link_request(&osdc->homeless_osd, req);
	}
	for (n = rb_first(&osd->o_linger_requests); n; ) {
		struct ceph_osd_linger_request *lreq =
		    rb_entry(n, struct ceph_osd_linger_request, node);

		n = rb_next(n); /* unlink_linger() */

		dout(" reassigning lreq %p linger_id %llu\n", lreq,
		     lreq->linger_id);
		unlink_linger(osd, lreq);
		link_linger(&osdc->homeless_osd, lreq);
	}
	clear_backoffs(osd);

	__remove_osd_from_lru(osd);
	erase_osd(&osdc->osds, osd);
	put_osd(osd);
}

/*
 * reset osd connect
 */
static int reopen_osd(struct ceph_osd *osd)
{
	struct ceph_entity_addr *peer_addr;

	dout("%s osd %p osd%d\n", __func__, osd, osd->o_osd);

	if (RB_EMPTY_ROOT(&osd->o_requests) &&
	    RB_EMPTY_ROOT(&osd->o_linger_requests)) {
		close_osd(osd);
		return -ENODEV;
	}

	peer_addr = &osd->o_osdc->osdmap->osd_addr[osd->o_osd];
	if (!memcmp(peer_addr, &osd->o_con.peer_addr, sizeof (*peer_addr)) &&
			!ceph_con_opened(&osd->o_con)) {
		struct rb_node *n;

		dout("osd addr hasn't changed and connection never opened, "
		     "letting msgr retry\n");
		/* touch each r_stamp for handle_timeout()'s benfit */
		for (n = rb_first(&osd->o_requests); n; n = rb_next(n)) {
			struct ceph_osd_request *req =
			    rb_entry(n, struct ceph_osd_request, r_node);
			req->r_stamp = jiffies;
		}

		return -EAGAIN;
	}

	ceph_con_close(&osd->o_con);
	ceph_con_open(&osd->o_con, CEPH_ENTITY_TYPE_OSD, osd->o_osd, peer_addr);
	osd->o_incarnation++;

	return 0;
}

static struct ceph_osd *lookup_create_osd(struct ceph_osd_client *osdc, int o,
					  bool wrlocked)
{
	struct ceph_osd *osd;

	if (wrlocked)
		verify_osdc_wrlocked(osdc);
	else
		verify_osdc_locked(osdc);

	if (o != CEPH_HOMELESS_OSD)
		osd = lookup_osd(&osdc->osds, o);
	else
		osd = &osdc->homeless_osd;
	if (!osd) {
		if (!wrlocked)
			return ERR_PTR(-EAGAIN);

		osd = create_osd(osdc, o);
		insert_osd(&osdc->osds, osd);
		ceph_con_open(&osd->o_con, CEPH_ENTITY_TYPE_OSD, osd->o_osd,
			      &osdc->osdmap->osd_addr[osd->o_osd]);
	}

	dout("%s osdc %p osd%d -> osd %p\n", __func__, osdc, o, osd);
	return osd;
}

/*
 * Create request <-> OSD session relation.
 *
 * @req has to be assigned a tid, @osd may be homeless.
 */
static void link_request(struct ceph_osd *osd, struct ceph_osd_request *req)
{
	verify_osd_locked(osd);
	WARN_ON(!req->r_tid || req->r_osd);
	dout("%s osd %p osd%d req %p tid %llu\n", __func__, osd, osd->o_osd,
	     req, req->r_tid);

	if (!osd_homeless(osd))
		__remove_osd_from_lru(osd);
	else
		atomic_inc(&osd->o_osdc->num_homeless);

	get_osd(osd);
	insert_request(&osd->o_requests, req);
	req->r_osd = osd;
}

static void unlink_request(struct ceph_osd *osd, struct ceph_osd_request *req)
{
	verify_osd_locked(osd);
	WARN_ON(req->r_osd != osd);
	dout("%s osd %p osd%d req %p tid %llu\n", __func__, osd, osd->o_osd,
	     req, req->r_tid);

	req->r_osd = NULL;
	erase_request(&osd->o_requests, req);
	put_osd(osd);

	if (!osd_homeless(osd))
		maybe_move_osd_to_lru(osd);
	else
		atomic_dec(&osd->o_osdc->num_homeless);
}

static bool __pool_full(struct ceph_pg_pool_info *pi)
{
	return pi->flags & CEPH_POOL_FLAG_FULL;
}

static bool have_pool_full(struct ceph_osd_client *osdc)
{
	struct rb_node *n;

	for (n = rb_first(&osdc->osdmap->pg_pools); n; n = rb_next(n)) {
		struct ceph_pg_pool_info *pi =
		    rb_entry(n, struct ceph_pg_pool_info, node);

		if (__pool_full(pi))
			return true;
	}

	return false;
}

static bool pool_full(struct ceph_osd_client *osdc, s64 pool_id)
{
	struct ceph_pg_pool_info *pi;

	pi = ceph_pg_pool_by_id(osdc->osdmap, pool_id);
	if (!pi)
		return false;

	return __pool_full(pi);
}

/*
 * Returns whether a request should be blocked from being sent
 * based on the current osdmap and osd_client settings.
 */
static bool target_should_be_paused(struct ceph_osd_client *osdc,
				    const struct ceph_osd_request_target *t,
				    struct ceph_pg_pool_info *pi)
{
	bool pauserd = ceph_osdmap_flag(osdc, CEPH_OSDMAP_PAUSERD);
	bool pausewr = ceph_osdmap_flag(osdc, CEPH_OSDMAP_PAUSEWR) ||
		       ceph_osdmap_flag(osdc, CEPH_OSDMAP_FULL) ||
		       __pool_full(pi);

	WARN_ON(pi->id != t->target_oloc.pool);
	return ((t->flags & CEPH_OSD_FLAG_READ) && pauserd) ||
	       ((t->flags & CEPH_OSD_FLAG_WRITE) && pausewr) ||
	       (osdc->osdmap->epoch < osdc->epoch_barrier);
}

static int pick_random_replica(const struct ceph_osds *acting)
{
	int i = prandom_u32() % acting->size;

	dout("%s picked osd%d, primary osd%d\n", __func__,
	     acting->osds[i], acting->primary);
	return i;
}

/*
 * Picks the closest replica based on client's location given by
 * crush_location option.  Prefers the primary if the locality is
 * the same.
 */
static int pick_closest_replica(struct ceph_osd_client *osdc,
				const struct ceph_osds *acting)
{
	struct ceph_options *opt = osdc->client->options;
	int best_i, best_locality;
	int i = 0, locality;

	do {
		locality = ceph_get_crush_locality(osdc->osdmap,
						   acting->osds[i],
						   &opt->crush_locs);
		if (i == 0 ||
		    (locality >= 0 && best_locality < 0) ||
		    (locality >= 0 && best_locality >= 0 &&
		     locality < best_locality)) {
			best_i = i;
			best_locality = locality;
		}
	} while (++i < acting->size);

	dout("%s picked osd%d with locality %d, primary osd%d\n", __func__,
	     acting->osds[best_i], best_locality, acting->primary);
	return best_i;
}

enum calc_target_result {
	CALC_TARGET_NO_ACTION = 0,
	CALC_TARGET_NEED_RESEND,
	CALC_TARGET_POOL_DNE,
};

static enum calc_target_result calc_target(struct ceph_osd_client *osdc,
					   struct ceph_osd_request_target *t,
					   bool any_change)
{
	struct ceph_pg_pool_info *pi;
	struct ceph_pg pgid, last_pgid;
	struct ceph_osds up, acting;
	bool is_read = t->flags & CEPH_OSD_FLAG_READ;
	bool is_write = t->flags & CEPH_OSD_FLAG_WRITE;
	bool force_resend = false;
	bool unpaused = false;
	bool legacy_change = false;
	bool split = false;
	bool sort_bitwise = ceph_osdmap_flag(osdc, CEPH_OSDMAP_SORTBITWISE);
	bool recovery_deletes = ceph_osdmap_flag(osdc,
						 CEPH_OSDMAP_RECOVERY_DELETES);
	enum calc_target_result ct_res;

	t->epoch = osdc->osdmap->epoch;
	pi = ceph_pg_pool_by_id(osdc->osdmap, t->base_oloc.pool);
	if (!pi) {
		t->osd = CEPH_HOMELESS_OSD;
		ct_res = CALC_TARGET_POOL_DNE;
		goto out;
	}

	if (osdc->osdmap->epoch == pi->last_force_request_resend) {
		if (t->last_force_resend < pi->last_force_request_resend) {
			t->last_force_resend = pi->last_force_request_resend;
			force_resend = true;
		} else if (t->last_force_resend == 0) {
			force_resend = true;
		}
	}

	/* apply tiering */
	ceph_oid_copy(&t->target_oid, &t->base_oid);
	ceph_oloc_copy(&t->target_oloc, &t->base_oloc);
	if ((t->flags & CEPH_OSD_FLAG_IGNORE_OVERLAY) == 0) {
		if (is_read && pi->read_tier >= 0)
			t->target_oloc.pool = pi->read_tier;
		if (is_write && pi->write_tier >= 0)
			t->target_oloc.pool = pi->write_tier;

		pi = ceph_pg_pool_by_id(osdc->osdmap, t->target_oloc.pool);
		if (!pi) {
			t->osd = CEPH_HOMELESS_OSD;
			ct_res = CALC_TARGET_POOL_DNE;
			goto out;
		}
	}

	__ceph_object_locator_to_pg(pi, &t->target_oid, &t->target_oloc, &pgid);
	last_pgid.pool = pgid.pool;
	last_pgid.seed = ceph_stable_mod(pgid.seed, t->pg_num, t->pg_num_mask);

	ceph_pg_to_up_acting_osds(osdc->osdmap, pi, &pgid, &up, &acting);
	if (any_change &&
	    ceph_is_new_interval(&t->acting,
				 &acting,
				 &t->up,
				 &up,
				 t->size,
				 pi->size,
				 t->min_size,
				 pi->min_size,
				 t->pg_num,
				 pi->pg_num,
				 t->sort_bitwise,
				 sort_bitwise,
				 t->recovery_deletes,
				 recovery_deletes,
				 &last_pgid))
		force_resend = true;

	if (t->paused && !target_should_be_paused(osdc, t, pi)) {
		t->paused = false;
		unpaused = true;
	}
	legacy_change = ceph_pg_compare(&t->pgid, &pgid) ||
			ceph_osds_changed(&t->acting, &acting,
					  t->used_replica || any_change);
	if (t->pg_num)
		split = ceph_pg_is_split(&last_pgid, t->pg_num, pi->pg_num);

	if (legacy_change || force_resend || split) {
		t->pgid = pgid; /* struct */
		ceph_pg_to_primary_shard(osdc->osdmap, pi, &pgid, &t->spgid);
		ceph_osds_copy(&t->acting, &acting);
		ceph_osds_copy(&t->up, &up);
		t->size = pi->size;
		t->min_size = pi->min_size;
		t->pg_num = pi->pg_num;
		t->pg_num_mask = pi->pg_num_mask;
		t->sort_bitwise = sort_bitwise;
		t->recovery_deletes = recovery_deletes;

		if ((t->flags & (CEPH_OSD_FLAG_BALANCE_READS |
				 CEPH_OSD_FLAG_LOCALIZE_READS)) &&
		    !is_write && pi->type == CEPH_POOL_TYPE_REP &&
		    acting.size > 1) {
			int pos;

			WARN_ON(!is_read || acting.osds[0] != acting.primary);
			if (t->flags & CEPH_OSD_FLAG_BALANCE_READS) {
				pos = pick_random_replica(&acting);
			} else {
				pos = pick_closest_replica(osdc, &acting);
			}
			t->osd = acting.osds[pos];
			t->used_replica = pos > 0;
		} else {
			t->osd = acting.primary;
			t->used_replica = false;
		}
	}

	if (unpaused || legacy_change || force_resend || split)
		ct_res = CALC_TARGET_NEED_RESEND;
	else
		ct_res = CALC_TARGET_NO_ACTION;

out:
	dout("%s t %p -> %d%d%d%d ct_res %d osd%d\n", __func__, t, unpaused,
	     legacy_change, force_resend, split, ct_res, t->osd);
	return ct_res;
}

static struct ceph_spg_mapping *alloc_spg_mapping(void)
{
	struct ceph_spg_mapping *spg;

	spg = kmalloc(sizeof(*spg), GFP_NOIO);
	if (!spg)
		return NULL;

	RB_CLEAR_NODE(&spg->node);
	spg->backoffs = RB_ROOT;
	return spg;
}

static void free_spg_mapping(struct ceph_spg_mapping *spg)
{
	WARN_ON(!RB_EMPTY_NODE(&spg->node));
	WARN_ON(!RB_EMPTY_ROOT(&spg->backoffs));

	kfree(spg);
}

/*
 * rbtree of ceph_spg_mapping for handling map<spg_t, ...>, similar to
 * ceph_pg_mapping.  Used to track OSD backoffs -- a backoff [range] is
 * defined only within a specific spgid; it does not pass anything to
 * children on split, or to another primary.
 */
DEFINE_RB_FUNCS2(spg_mapping, struct ceph_spg_mapping, spgid, ceph_spg_compare,
		 RB_BYPTR, const struct ceph_spg *, node)

static u64 hoid_get_bitwise_key(const struct ceph_hobject_id *hoid)
{
	return hoid->is_max ? 0x100000000ull : hoid->hash_reverse_bits;
}

static void hoid_get_effective_key(const struct ceph_hobject_id *hoid,
				   void **pkey, size_t *pkey_len)
{
	if (hoid->key_len) {
		*pkey = hoid->key;
		*pkey_len = hoid->key_len;
	} else {
		*pkey = hoid->oid;
		*pkey_len = hoid->oid_len;
	}
}

static int compare_names(const void *name1, size_t name1_len,
			 const void *name2, size_t name2_len)
{
	int ret;

	ret = memcmp(name1, name2, min(name1_len, name2_len));
	if (!ret) {
		if (name1_len < name2_len)
			ret = -1;
		else if (name1_len > name2_len)
			ret = 1;
	}
	return ret;
}

static int hoid_compare(const struct ceph_hobject_id *lhs,
			const struct ceph_hobject_id *rhs)
{
	void *effective_key1, *effective_key2;
	size_t effective_key1_len, effective_key2_len;
	int ret;

	if (lhs->is_max < rhs->is_max)
		return -1;
	if (lhs->is_max > rhs->is_max)
		return 1;

	if (lhs->pool < rhs->pool)
		return -1;
	if (lhs->pool > rhs->pool)
		return 1;

	if (hoid_get_bitwise_key(lhs) < hoid_get_bitwise_key(rhs))
		return -1;
	if (hoid_get_bitwise_key(lhs) > hoid_get_bitwise_key(rhs))
		return 1;

	ret = compare_names(lhs->nspace, lhs->nspace_len,
			    rhs->nspace, rhs->nspace_len);
	if (ret)
		return ret;

	hoid_get_effective_key(lhs, &effective_key1, &effective_key1_len);
	hoid_get_effective_key(rhs, &effective_key2, &effective_key2_len);
	ret = compare_names(effective_key1, effective_key1_len,
			    effective_key2, effective_key2_len);
	if (ret)
		return ret;

	ret = compare_names(lhs->oid, lhs->oid_len, rhs->oid, rhs->oid_len);
	if (ret)
		return ret;

	if (lhs->snapid < rhs->snapid)
		return -1;
	if (lhs->snapid > rhs->snapid)
		return 1;

	return 0;
}

/*
 * For decoding ->begin and ->end of MOSDBackoff only -- no MIN/MAX
 * compat stuff here.
 *
 * Assumes @hoid is zero-initialized.
 */
static int decode_hoid(void **p, void *end, struct ceph_hobject_id *hoid)
{
	u8 struct_v;
	u32 struct_len;
	int ret;

	ret = ceph_start_decoding(p, end, 4, "hobject_t", &struct_v,
				  &struct_len);
	if (ret)
		return ret;

	if (struct_v < 4) {
		pr_err("got struct_v %d < 4 of hobject_t\n", struct_v);
		goto e_inval;
	}

	hoid->key = ceph_extract_encoded_string(p, end, &hoid->key_len,
						GFP_NOIO);
	if (IS_ERR(hoid->key)) {
		ret = PTR_ERR(hoid->key);
		hoid->key = NULL;
		return ret;
	}

	hoid->oid = ceph_extract_encoded_string(p, end, &hoid->oid_len,
						GFP_NOIO);
	if (IS_ERR(hoid->oid)) {
		ret = PTR_ERR(hoid->oid);
		hoid->oid = NULL;
		return ret;
	}

	ceph_decode_64_safe(p, end, hoid->snapid, e_inval);
	ceph_decode_32_safe(p, end, hoid->hash, e_inval);
	ceph_decode_8_safe(p, end, hoid->is_max, e_inval);

	hoid->nspace = ceph_extract_encoded_string(p, end, &hoid->nspace_len,
						   GFP_NOIO);
	if (IS_ERR(hoid->nspace)) {
		ret = PTR_ERR(hoid->nspace);
		hoid->nspace = NULL;
		return ret;
	}

	ceph_decode_64_safe(p, end, hoid->pool, e_inval);

	ceph_hoid_build_hash_cache(hoid);
	return 0;

e_inval:
	return -EINVAL;
}

static int hoid_encoding_size(const struct ceph_hobject_id *hoid)
{
	return 8 + 4 + 1 + 8 + /* snapid, hash, is_max, pool */
	       4 + hoid->key_len + 4 + hoid->oid_len + 4 + hoid->nspace_len;
}

static void encode_hoid(void **p, void *end, const struct ceph_hobject_id *hoid)
{
	ceph_start_encoding(p, 4, 3, hoid_encoding_size(hoid));
	ceph_encode_string(p, end, hoid->key, hoid->key_len);
	ceph_encode_string(p, end, hoid->oid, hoid->oid_len);
	ceph_encode_64(p, hoid->snapid);
	ceph_encode_32(p, hoid->hash);
	ceph_encode_8(p, hoid->is_max);
	ceph_encode_string(p, end, hoid->nspace, hoid->nspace_len);
	ceph_encode_64(p, hoid->pool);
}

static void free_hoid(struct ceph_hobject_id *hoid)
{
	if (hoid) {
		kfree(hoid->key);
		kfree(hoid->oid);
		kfree(hoid->nspace);
		kfree(hoid);
	}
}

static struct ceph_osd_backoff *alloc_backoff(void)
{
	struct ceph_osd_backoff *backoff;

	backoff = kzalloc(sizeof(*backoff), GFP_NOIO);
	if (!backoff)
		return NULL;

	RB_CLEAR_NODE(&backoff->spg_node);
	RB_CLEAR_NODE(&backoff->id_node);
	return backoff;
}

static void free_backoff(struct ceph_osd_backoff *backoff)
{
	WARN_ON(!RB_EMPTY_NODE(&backoff->spg_node));
	WARN_ON(!RB_EMPTY_NODE(&backoff->id_node));

	free_hoid(backoff->begin);
	free_hoid(backoff->end);
	kfree(backoff);
}

/*
 * Within a specific spgid, backoffs are managed by ->begin hoid.
 */
DEFINE_RB_INSDEL_FUNCS2(backoff, struct ceph_osd_backoff, begin, hoid_compare,
			RB_BYVAL, spg_node);

static struct ceph_osd_backoff *lookup_containing_backoff(struct rb_root *root,
					    const struct ceph_hobject_id *hoid)
{
	struct rb_node *n = root->rb_node;

	while (n) {
		struct ceph_osd_backoff *cur =
		    rb_entry(n, struct ceph_osd_backoff, spg_node);
		int cmp;

		cmp = hoid_compare(hoid, cur->begin);
		if (cmp < 0) {
			n = n->rb_left;
		} else if (cmp > 0) {
			if (hoid_compare(hoid, cur->end) < 0)
				return cur;

			n = n->rb_right;
		} else {
			return cur;
		}
	}

	return NULL;
}

/*
 * Each backoff has a unique id within its OSD session.
 */
DEFINE_RB_FUNCS(backoff_by_id, struct ceph_osd_backoff, id, id_node)

static void clear_backoffs(struct ceph_osd *osd)
{
	while (!RB_EMPTY_ROOT(&osd->o_backoff_mappings)) {
		struct ceph_spg_mapping *spg =
		    rb_entry(rb_first(&osd->o_backoff_mappings),
			     struct ceph_spg_mapping, node);

		while (!RB_EMPTY_ROOT(&spg->backoffs)) {
			struct ceph_osd_backoff *backoff =
			    rb_entry(rb_first(&spg->backoffs),
				     struct ceph_osd_backoff, spg_node);

			erase_backoff(&spg->backoffs, backoff);
			erase_backoff_by_id(&osd->o_backoffs_by_id, backoff);
			free_backoff(backoff);
		}
		erase_spg_mapping(&osd->o_backoff_mappings, spg);
		free_spg_mapping(spg);
	}
}

/*
 * Set up a temporary, non-owning view into @t.
 */
static void hoid_fill_from_target(struct ceph_hobject_id *hoid,
				  const struct ceph_osd_request_target *t)
{
	hoid->key = NULL;
	hoid->key_len = 0;
	hoid->oid = t->target_oid.name;
	hoid->oid_len = t->target_oid.name_len;
	hoid->snapid = CEPH_NOSNAP;
	hoid->hash = t->pgid.seed;
	hoid->is_max = false;
	if (t->target_oloc.pool_ns) {
		hoid->nspace = t->target_oloc.pool_ns->str;
		hoid->nspace_len = t->target_oloc.pool_ns->len;
	} else {
		hoid->nspace = NULL;
		hoid->nspace_len = 0;
	}
	hoid->pool = t->target_oloc.pool;
	ceph_hoid_build_hash_cache(hoid);
}

static bool should_plug_request(struct ceph_osd_request *req)
{
	struct ceph_osd *osd = req->r_osd;
	struct ceph_spg_mapping *spg;
	struct ceph_osd_backoff *backoff;
	struct ceph_hobject_id hoid;

	spg = lookup_spg_mapping(&osd->o_backoff_mappings, &req->r_t.spgid);
	if (!spg)
		return false;

	hoid_fill_from_target(&hoid, &req->r_t);
	backoff = lookup_containing_backoff(&spg->backoffs, &hoid);
	if (!backoff)
		return false;

	dout("%s req %p tid %llu backoff osd%d spgid %llu.%xs%d id %llu\n",
	     __func__, req, req->r_tid, osd->o_osd, backoff->spgid.pgid.pool,
	     backoff->spgid.pgid.seed, backoff->spgid.shard, backoff->id);
	return true;
}

/*
 * Keep get_num_data_items() in sync with this function.
 */
static void setup_request_data(struct ceph_osd_request *req)
{
	struct ceph_msg *request_msg = req->r_request;
	struct ceph_msg *reply_msg = req->r_reply;
	struct ceph_osd_req_op *op;

	if (req->r_request->num_data_items || req->r_reply->num_data_items)
		return;

	WARN_ON(request_msg->data_length || reply_msg->data_length);
	for (op = req->r_ops; op != &req->r_ops[req->r_num_ops]; op++) {
		switch (op->op) {
		/* request */
		case CEPH_OSD_OP_WRITE:
		case CEPH_OSD_OP_WRITEFULL:
			WARN_ON(op->indata_len != op->extent.length);
			ceph_osdc_msg_data_add(request_msg,
					       &op->extent.osd_data);
			break;
		case CEPH_OSD_OP_SETXATTR:
		case CEPH_OSD_OP_CMPXATTR:
			WARN_ON(op->indata_len != op->xattr.name_len +
						  op->xattr.value_len);
			ceph_osdc_msg_data_add(request_msg,
					       &op->xattr.osd_data);
			break;
		case CEPH_OSD_OP_NOTIFY_ACK:
			ceph_osdc_msg_data_add(request_msg,
					       &op->notify_ack.request_data);
			break;
		case CEPH_OSD_OP_COPY_FROM2:
			ceph_osdc_msg_data_add(request_msg,
					       &op->copy_from.osd_data);
			break;

		/* reply */
		case CEPH_OSD_OP_STAT:
			ceph_osdc_msg_data_add(reply_msg,
					       &op->raw_data_in);
			break;
		case CEPH_OSD_OP_READ:
			ceph_osdc_msg_data_add(reply_msg,
					       &op->extent.osd_data);
			break;
		case CEPH_OSD_OP_LIST_WATCHERS:
			ceph_osdc_msg_data_add(reply_msg,
					       &op->list_watchers.response_data);
			break;

		/* both */
		case CEPH_OSD_OP_CALL:
			WARN_ON(op->indata_len != op->cls.class_len +
						  op->cls.method_len +
						  op->cls.indata_len);
			ceph_osdc_msg_data_add(request_msg,
					       &op->cls.request_info);
			/* optional, can be NONE */
			ceph_osdc_msg_data_add(request_msg,
					       &op->cls.request_data);
			/* optional, can be NONE */
			ceph_osdc_msg_data_add(reply_msg,
					       &op->cls.response_data);
			break;
		case CEPH_OSD_OP_NOTIFY:
			ceph_osdc_msg_data_add(request_msg,
					       &op->notify.request_data);
			ceph_osdc_msg_data_add(reply_msg,
					       &op->notify.response_data);
			break;
		}
	}
}

static void encode_pgid(void **p, const struct ceph_pg *pgid)
{
	ceph_encode_8(p, 1);
	ceph_encode_64(p, pgid->pool);
	ceph_encode_32(p, pgid->seed);
	ceph_encode_32(p, -1); /* preferred */
}

static void encode_spgid(void **p, const struct ceph_spg *spgid)
{
	ceph_start_encoding(p, 1, 1, CEPH_PGID_ENCODING_LEN + 1);
	encode_pgid(p, &spgid->pgid);
	ceph_encode_8(p, spgid->shard);
}

static void encode_oloc(void **p, void *end,
			const struct ceph_object_locator *oloc)
{
	ceph_start_encoding(p, 5, 4, ceph_oloc_encoding_size(oloc));
	ceph_encode_64(p, oloc->pool);
	ceph_encode_32(p, -1); /* preferred */
	ceph_encode_32(p, 0);  /* key len */
	if (oloc->pool_ns)
		ceph_encode_string(p, end, oloc->pool_ns->str,
				   oloc->pool_ns->len);
	else
		ceph_encode_32(p, 0);
}

static void encode_request_partial(struct ceph_osd_request *req,
				   struct ceph_msg *msg)
{
	void *p = msg->front.iov_base;
	void *const end = p + msg->front_alloc_len;
	u32 data_len = 0;
	int i;

	if (req->r_flags & CEPH_OSD_FLAG_WRITE) {
		/* snapshots aren't writeable */
		WARN_ON(req->r_snapid != CEPH_NOSNAP);
	} else {
		WARN_ON(req->r_mtime.tv_sec || req->r_mtime.tv_nsec ||
			req->r_data_offset || req->r_snapc);
	}

	setup_request_data(req);

	encode_spgid(&p, &req->r_t.spgid); /* actual spg */
	ceph_encode_32(&p, req->r_t.pgid.seed); /* raw hash */
	ceph_encode_32(&p, req->r_osdc->osdmap->epoch);
	ceph_encode_32(&p, req->r_flags);

	/* reqid */
	ceph_start_encoding(&p, 2, 2, sizeof(struct ceph_osd_reqid));
	memset(p, 0, sizeof(struct ceph_osd_reqid));
	p += sizeof(struct ceph_osd_reqid);

	/* trace */
	memset(p, 0, sizeof(struct ceph_blkin_trace_info));
	p += sizeof(struct ceph_blkin_trace_info);

	ceph_encode_32(&p, 0); /* client_inc, always 0 */
	ceph_encode_timespec64(p, &req->r_mtime);
	p += sizeof(struct ceph_timespec);

	encode_oloc(&p, end, &req->r_t.target_oloc);
	ceph_encode_string(&p, end, req->r_t.target_oid.name,
			   req->r_t.target_oid.name_len);

	/* ops, can imply data */
	ceph_encode_16(&p, req->r_num_ops);
	for (i = 0; i < req->r_num_ops; i++) {
		data_len += osd_req_encode_op(p, &req->r_ops[i]);
		p += sizeof(struct ceph_osd_op);
	}

	ceph_encode_64(&p, req->r_snapid); /* snapid */
	if (req->r_snapc) {
		ceph_encode_64(&p, req->r_snapc->seq);
		ceph_encode_32(&p, req->r_snapc->num_snaps);
		for (i = 0; i < req->r_snapc->num_snaps; i++)
			ceph_encode_64(&p, req->r_snapc->snaps[i]);
	} else {
		ceph_encode_64(&p, 0); /* snap_seq */
		ceph_encode_32(&p, 0); /* snaps len */
	}

	ceph_encode_32(&p, req->r_attempts); /* retry_attempt */
	BUG_ON(p > end - 8); /* space for features */

	msg->hdr.version = cpu_to_le16(8); /* MOSDOp v8 */
	/* front_len is finalized in encode_request_finish() */
	msg->front.iov_len = p - msg->front.iov_base;
	msg->hdr.front_len = cpu_to_le32(msg->front.iov_len);
	msg->hdr.data_len = cpu_to_le32(data_len);
	/*
	 * The header "data_off" is a hint to the receiver allowing it
	 * to align received data into its buffers such that there's no
	 * need to re-copy it before writing it to disk (direct I/O).
	 */
	msg->hdr.data_off = cpu_to_le16(req->r_data_offset);

	dout("%s req %p msg %p oid %s oid_len %d\n", __func__, req, msg,
	     req->r_t.target_oid.name, req->r_t.target_oid.name_len);
}

static void encode_request_finish(struct ceph_msg *msg)
{
	void *p = msg->front.iov_base;
	void *const partial_end = p + msg->front.iov_len;
	void *const end = p + msg->front_alloc_len;

	if (CEPH_HAVE_FEATURE(msg->con->peer_features, RESEND_ON_SPLIT)) {
		/* luminous OSD -- encode features and be done */
		p = partial_end;
		ceph_encode_64(&p, msg->con->peer_features);
	} else {
		struct {
			char spgid[CEPH_ENCODING_START_BLK_LEN +
				   CEPH_PGID_ENCODING_LEN + 1];
			__le32 hash;
			__le32 epoch;
			__le32 flags;
			char reqid[CEPH_ENCODING_START_BLK_LEN +
				   sizeof(struct ceph_osd_reqid)];
			char trace[sizeof(struct ceph_blkin_trace_info)];
			__le32 client_inc;
			struct ceph_timespec mtime;
		} __packed head;
		struct ceph_pg pgid;
		void *oloc, *oid, *tail;
		int oloc_len, oid_len, tail_len;
		int len;

		/*
		 * Pre-luminous OSD -- reencode v8 into v4 using @head
		 * as a temporary buffer.  Encode the raw PG; the rest
		 * is just a matter of moving oloc, oid and tail blobs
		 * around.
		 */
		memcpy(&head, p, sizeof(head));
		p += sizeof(head);

		oloc = p;
		p += CEPH_ENCODING_START_BLK_LEN;
		pgid.pool = ceph_decode_64(&p);
		p += 4 + 4; /* preferred, key len */
		len = ceph_decode_32(&p);
		p += len;   /* nspace */
		oloc_len = p - oloc;

		oid = p;
		len = ceph_decode_32(&p);
		p += len;
		oid_len = p - oid;

		tail = p;
		tail_len = partial_end - p;

		p = msg->front.iov_base;
		ceph_encode_copy(&p, &head.client_inc, sizeof(head.client_inc));
		ceph_encode_copy(&p, &head.epoch, sizeof(head.epoch));
		ceph_encode_copy(&p, &head.flags, sizeof(head.flags));
		ceph_encode_copy(&p, &head.mtime, sizeof(head.mtime));

		/* reassert_version */
		memset(p, 0, sizeof(struct ceph_eversion));
		p += sizeof(struct ceph_eversion);

		BUG_ON(p >= oloc);
		memmove(p, oloc, oloc_len);
		p += oloc_len;

		pgid.seed = le32_to_cpu(head.hash);
		encode_pgid(&p, &pgid); /* raw pg */

		BUG_ON(p >= oid);
		memmove(p, oid, oid_len);
		p += oid_len;

		/* tail -- ops, snapid, snapc, retry_attempt */
		BUG_ON(p >= tail);
		memmove(p, tail, tail_len);
		p += tail_len;

		msg->hdr.version = cpu_to_le16(4); /* MOSDOp v4 */
	}

	BUG_ON(p > end);
	msg->front.iov_len = p - msg->front.iov_base;
	msg->hdr.front_len = cpu_to_le32(msg->front.iov_len);

	dout("%s msg %p tid %llu %u+%u+%u v%d\n", __func__, msg,
	     le64_to_cpu(msg->hdr.tid), le32_to_cpu(msg->hdr.front_len),
	     le32_to_cpu(msg->hdr.middle_len), le32_to_cpu(msg->hdr.data_len),
	     le16_to_cpu(msg->hdr.version));
}

/*
 * @req has to be assigned a tid and registered.
 */
static void send_request(struct ceph_osd_request *req)
{
	struct ceph_osd *osd = req->r_osd;

	verify_osd_locked(osd);
	WARN_ON(osd->o_osd != req->r_t.osd);

	/* backoff? */
	if (should_plug_request(req))
		return;

	/*
	 * We may have a previously queued request message hanging
	 * around.  Cancel it to avoid corrupting the msgr.
	 */
	if (req->r_sent)
		ceph_msg_revoke(req->r_request);

	req->r_flags |= CEPH_OSD_FLAG_KNOWN_REDIR;
	if (req->r_attempts)
		req->r_flags |= CEPH_OSD_FLAG_RETRY;
	else
		WARN_ON(req->r_flags & CEPH_OSD_FLAG_RETRY);

	encode_request_partial(req, req->r_request);

	dout("%s req %p tid %llu to pgid %llu.%x spgid %llu.%xs%d osd%d e%u flags 0x%x attempt %d\n",
	     __func__, req, req->r_tid, req->r_t.pgid.pool, req->r_t.pgid.seed,
	     req->r_t.spgid.pgid.pool, req->r_t.spgid.pgid.seed,
	     req->r_t.spgid.shard, osd->o_osd, req->r_t.epoch, req->r_flags,
	     req->r_attempts);

	req->r_t.paused = false;
	req->r_stamp = jiffies;
	req->r_attempts++;

	req->r_sent = osd->o_incarnation;
	req->r_request->hdr.tid = cpu_to_le64(req->r_tid);
	ceph_con_send(&osd->o_con, ceph_msg_get(req->r_request));
}

static void maybe_request_map(struct ceph_osd_client *osdc)
{
	bool continuous = false;

	verify_osdc_locked(osdc);
	WARN_ON(!osdc->osdmap->epoch);

	if (ceph_osdmap_flag(osdc, CEPH_OSDMAP_FULL) ||
	    ceph_osdmap_flag(osdc, CEPH_OSDMAP_PAUSERD) ||
	    ceph_osdmap_flag(osdc, CEPH_OSDMAP_PAUSEWR)) {
		dout("%s osdc %p continuous\n", __func__, osdc);
		continuous = true;
	} else {
		dout("%s osdc %p onetime\n", __func__, osdc);
	}

	if (ceph_monc_want_map(&osdc->client->monc, CEPH_SUB_OSDMAP,
			       osdc->osdmap->epoch + 1, continuous))
		ceph_monc_renew_subs(&osdc->client->monc);
}

static void complete_request(struct ceph_osd_request *req, int err);
static void send_map_check(struct ceph_osd_request *req);

static void __submit_request(struct ceph_osd_request *req, bool wrlocked)
{
	struct ceph_osd_client *osdc = req->r_osdc;
	struct ceph_osd *osd;
	enum calc_target_result ct_res;
	int err = 0;
	bool need_send = false;
	bool promoted = false;

	WARN_ON(req->r_tid);
	dout("%s req %p wrlocked %d\n", __func__, req, wrlocked);

again:
	ct_res = calc_target(osdc, &req->r_t, false);
	if (ct_res == CALC_TARGET_POOL_DNE && !wrlocked)
		goto promote;

	osd = lookup_create_osd(osdc, req->r_t.osd, wrlocked);
	if (IS_ERR(osd)) {
		WARN_ON(PTR_ERR(osd) != -EAGAIN || wrlocked);
		goto promote;
	}

	if (osdc->abort_err) {
		dout("req %p abort_err %d\n", req, osdc->abort_err);
		err = osdc->abort_err;
	} else if (osdc->osdmap->epoch < osdc->epoch_barrier) {
		dout("req %p epoch %u barrier %u\n", req, osdc->osdmap->epoch,
		     osdc->epoch_barrier);
		req->r_t.paused = true;
		maybe_request_map(osdc);
	} else if ((req->r_flags & CEPH_OSD_FLAG_WRITE) &&
		   ceph_osdmap_flag(osdc, CEPH_OSDMAP_PAUSEWR)) {
		dout("req %p pausewr\n", req);
		req->r_t.paused = true;
		maybe_request_map(osdc);
	} else if ((req->r_flags & CEPH_OSD_FLAG_READ) &&
		   ceph_osdmap_flag(osdc, CEPH_OSDMAP_PAUSERD)) {
		dout("req %p pauserd\n", req);
		req->r_t.paused = true;
		maybe_request_map(osdc);
	} else if ((req->r_flags & CEPH_OSD_FLAG_WRITE) &&
		   !(req->r_flags & (CEPH_OSD_FLAG_FULL_TRY |
				     CEPH_OSD_FLAG_FULL_FORCE)) &&
		   (ceph_osdmap_flag(osdc, CEPH_OSDMAP_FULL) ||
		    pool_full(osdc, req->r_t.base_oloc.pool))) {
		dout("req %p full/pool_full\n", req);
		if (ceph_test_opt(osdc->client, ABORT_ON_FULL)) {
			err = -ENOSPC;
		} else {
			pr_warn_ratelimited("FULL or reached pool quota\n");
			req->r_t.paused = true;
			maybe_request_map(osdc);
		}
	} else if (!osd_homeless(osd)) {
		need_send = true;
	} else {
		maybe_request_map(osdc);
	}

	mutex_lock(&osd->lock);
	/*
	 * Assign the tid atomically with send_request() to protect
	 * multiple writes to the same object from racing with each
	 * other, resulting in out of order ops on the OSDs.
	 */
	req->r_tid = atomic64_inc_return(&osdc->last_tid);
	link_request(osd, req);
	if (need_send)
		send_request(req);
	else if (err)
		complete_request(req, err);
	mutex_unlock(&osd->lock);

	if (!err && ct_res == CALC_TARGET_POOL_DNE)
		send_map_check(req);

	if (promoted)
		downgrade_write(&osdc->lock);
	return;

promote:
	up_read(&osdc->lock);
	down_write(&osdc->lock);
	wrlocked = true;
	promoted = true;
	goto again;
}

static void account_request(struct ceph_osd_request *req)
{
	WARN_ON(req->r_flags & (CEPH_OSD_FLAG_ACK | CEPH_OSD_FLAG_ONDISK));
	WARN_ON(!(req->r_flags & (CEPH_OSD_FLAG_READ | CEPH_OSD_FLAG_WRITE)));

	req->r_flags |= CEPH_OSD_FLAG_ONDISK;
	atomic_inc(&req->r_osdc->num_requests);

	req->r_start_stamp = jiffies;
	req->r_start_latency = ktime_get();
}

static void submit_request(struct ceph_osd_request *req, bool wrlocked)
{
	ceph_osdc_get_request(req);
	account_request(req);
	__submit_request(req, wrlocked);
}

static void finish_request(struct ceph_osd_request *req)
{
	struct ceph_osd_client *osdc = req->r_osdc;

	WARN_ON(lookup_request_mc(&osdc->map_checks, req->r_tid));
	dout("%s req %p tid %llu\n", __func__, req, req->r_tid);

	req->r_end_latency = ktime_get();

	if (req->r_osd)
		unlink_request(req->r_osd, req);
	atomic_dec(&osdc->num_requests);

	/*
	 * If an OSD has failed or returned and a request has been sent
	 * twice, it's possible to get a reply and end up here while the
	 * request message is queued for delivery.  We will ignore the
	 * reply, so not a big deal, but better to try and catch it.
	 */
	ceph_msg_revoke(req->r_request);
	ceph_msg_revoke_incoming(req->r_reply);
}

static void __complete_request(struct ceph_osd_request *req)
{
	dout("%s req %p tid %llu cb %ps result %d\n", __func__, req,
	     req->r_tid, req->r_callback, req->r_result);

	if (req->r_callback)
		req->r_callback(req);
	complete_all(&req->r_completion);
	ceph_osdc_put_request(req);
}

static void complete_request_workfn(struct work_struct *work)
{
	struct ceph_osd_request *req =
	    container_of(work, struct ceph_osd_request, r_complete_work);

	__complete_request(req);
}

/*
 * This is open-coded in handle_reply().
 */
static void complete_request(struct ceph_osd_request *req, int err)
{
	dout("%s req %p tid %llu err %d\n", __func__, req, req->r_tid, err);

	req->r_result = err;
	finish_request(req);

	INIT_WORK(&req->r_complete_work, complete_request_workfn);
	queue_work(req->r_osdc->completion_wq, &req->r_complete_work);
}

static void cancel_map_check(struct ceph_osd_request *req)
{
	struct ceph_osd_client *osdc = req->r_osdc;
	struct ceph_osd_request *lookup_req;

	verify_osdc_wrlocked(osdc);

	lookup_req = lookup_request_mc(&osdc->map_checks, req->r_tid);
	if (!lookup_req)
		return;

	WARN_ON(lookup_req != req);
	erase_request_mc(&osdc->map_checks, req);
	ceph_osdc_put_request(req);
}

static void cancel_request(struct ceph_osd_request *req)
{
	dout("%s req %p tid %llu\n", __func__, req, req->r_tid);

	cancel_map_check(req);
	finish_request(req);
	complete_all(&req->r_completion);
	ceph_osdc_put_request(req);
}

static void abort_request(struct ceph_osd_request *req, int err)
{
	dout("%s req %p tid %llu err %d\n", __func__, req, req->r_tid, err);

	cancel_map_check(req);
	complete_request(req, err);
}

static int abort_fn(struct ceph_osd_request *req, void *arg)
{
	int err = *(int *)arg;

	abort_request(req, err);
	return 0; /* continue iteration */
}

/*
 * Abort all in-flight requests with @err and arrange for all future
 * requests to be failed immediately.
 */
void ceph_osdc_abort_requests(struct ceph_osd_client *osdc, int err)
{
	dout("%s osdc %p err %d\n", __func__, osdc, err);
	down_write(&osdc->lock);
	for_each_request(osdc, abort_fn, &err);
	osdc->abort_err = err;
	up_write(&osdc->lock);
}
EXPORT_SYMBOL(ceph_osdc_abort_requests);

void ceph_osdc_clear_abort_err(struct ceph_osd_client *osdc)
{
	down_write(&osdc->lock);
	osdc->abort_err = 0;
	up_write(&osdc->lock);
}
EXPORT_SYMBOL(ceph_osdc_clear_abort_err);

static void update_epoch_barrier(struct ceph_osd_client *osdc, u32 eb)
{
	if (likely(eb > osdc->epoch_barrier)) {
		dout("updating epoch_barrier from %u to %u\n",
				osdc->epoch_barrier, eb);
		osdc->epoch_barrier = eb;
		/* Request map if we're not to the barrier yet */
		if (eb > osdc->osdmap->epoch)
			maybe_request_map(osdc);
	}
}

void ceph_osdc_update_epoch_barrier(struct ceph_osd_client *osdc, u32 eb)
{
	down_read(&osdc->lock);
	if (unlikely(eb > osdc->epoch_barrier)) {
		up_read(&osdc->lock);
		down_write(&osdc->lock);
		update_epoch_barrier(osdc, eb);
		up_write(&osdc->lock);
	} else {
		up_read(&osdc->lock);
	}
}
EXPORT_SYMBOL(ceph_osdc_update_epoch_barrier);

/*
 * We can end up releasing caps as a result of abort_request().
 * In that case, we probably want to ensure that the cap release message
 * has an updated epoch barrier in it, so set the epoch barrier prior to
 * aborting the first request.
 */
static int abort_on_full_fn(struct ceph_osd_request *req, void *arg)
{
	struct ceph_osd_client *osdc = req->r_osdc;
	bool *victims = arg;

	if ((req->r_flags & CEPH_OSD_FLAG_WRITE) &&
	    (ceph_osdmap_flag(osdc, CEPH_OSDMAP_FULL) ||
	     pool_full(osdc, req->r_t.base_oloc.pool))) {
		if (!*victims) {
			update_epoch_barrier(osdc, osdc->osdmap->epoch);
			*victims = true;
		}
		abort_request(req, -ENOSPC);
	}

	return 0; /* continue iteration */
}

/*
 * Drop all pending requests that are stalled waiting on a full condition to
 * clear, and complete them with ENOSPC as the return code. Set the
 * osdc->epoch_barrier to the latest map epoch that we've seen if any were
 * cancelled.
 */
static void ceph_osdc_abort_on_full(struct ceph_osd_client *osdc)
{
	bool victims = false;

	if (ceph_test_opt(osdc->client, ABORT_ON_FULL) &&
	    (ceph_osdmap_flag(osdc, CEPH_OSDMAP_FULL) || have_pool_full(osdc)))
		for_each_request(osdc, abort_on_full_fn, &victims);
}

static void check_pool_dne(struct ceph_osd_request *req)
{
	struct ceph_osd_client *osdc = req->r_osdc;
	struct ceph_osdmap *map = osdc->osdmap;

	verify_osdc_wrlocked(osdc);
	WARN_ON(!map->epoch);

	if (req->r_attempts) {
		/*
		 * We sent a request earlier, which means that
		 * previously the pool existed, and now it does not
		 * (i.e., it was deleted).
		 */
		req->r_map_dne_bound = map->epoch;
		dout("%s req %p tid %llu pool disappeared\n", __func__, req,
		     req->r_tid);
	} else {
		dout("%s req %p tid %llu map_dne_bound %u have %u\n", __func__,
		     req, req->r_tid, req->r_map_dne_bound, map->epoch);
	}

	if (req->r_map_dne_bound) {
		if (map->epoch >= req->r_map_dne_bound) {
			/* we had a new enough map */
			pr_info_ratelimited("tid %llu pool does not exist\n",
					    req->r_tid);
			complete_request(req, -ENOENT);
		}
	} else {
		send_map_check(req);
	}
}

static void map_check_cb(struct ceph_mon_generic_request *greq)
{
	struct ceph_osd_client *osdc = &greq->monc->client->osdc;
	struct ceph_osd_request *req;
	u64 tid = greq->private_data;

	WARN_ON(greq->result || !greq->u.newest);

	down_write(&osdc->lock);
	req = lookup_request_mc(&osdc->map_checks, tid);
	if (!req) {
		dout("%s tid %llu dne\n", __func__, tid);
		goto out_unlock;
	}

	dout("%s req %p tid %llu map_dne_bound %u newest %llu\n", __func__,
	     req, req->r_tid, req->r_map_dne_bound, greq->u.newest);
	if (!req->r_map_dne_bound)
		req->r_map_dne_bound = greq->u.newest;
	erase_request_mc(&osdc->map_checks, req);
	check_pool_dne(req);

	ceph_osdc_put_request(req);
out_unlock:
	up_write(&osdc->lock);
}

static void send_map_check(struct ceph_osd_request *req)
{
	struct ceph_osd_client *osdc = req->r_osdc;
	struct ceph_osd_request *lookup_req;
	int ret;

	verify_osdc_wrlocked(osdc);

	lookup_req = lookup_request_mc(&osdc->map_checks, req->r_tid);
	if (lookup_req) {
		WARN_ON(lookup_req != req);
		return;
	}

	ceph_osdc_get_request(req);
	insert_request_mc(&osdc->map_checks, req);
	ret = ceph_monc_get_version_async(&osdc->client->monc, "osdmap",
					  map_check_cb, req->r_tid);
	WARN_ON(ret);
}

/*
 * lingering requests, watch/notify v2 infrastructure
 */
static void linger_release(struct kref *kref)
{
	struct ceph_osd_linger_request *lreq =
	    container_of(kref, struct ceph_osd_linger_request, kref);

	dout("%s lreq %p reg_req %p ping_req %p\n", __func__, lreq,
	     lreq->reg_req, lreq->ping_req);
	WARN_ON(!RB_EMPTY_NODE(&lreq->node));
	WARN_ON(!RB_EMPTY_NODE(&lreq->osdc_node));
	WARN_ON(!RB_EMPTY_NODE(&lreq->mc_node));
	WARN_ON(!list_empty(&lreq->scan_item));
	WARN_ON(!list_empty(&lreq->pending_lworks));
	WARN_ON(lreq->osd);

	if (lreq->reg_req)
		ceph_osdc_put_request(lreq->reg_req);
	if (lreq->ping_req)
		ceph_osdc_put_request(lreq->ping_req);
	target_destroy(&lreq->t);
	kfree(lreq);
}

static void linger_put(struct ceph_osd_linger_request *lreq)
{
	if (lreq)
		kref_put(&lreq->kref, linger_release);
}

static struct ceph_osd_linger_request *
linger_get(struct ceph_osd_linger_request *lreq)
{
	kref_get(&lreq->kref);
	return lreq;
}

static struct ceph_osd_linger_request *
linger_alloc(struct ceph_osd_client *osdc)
{
	struct ceph_osd_linger_request *lreq;

	lreq = kzalloc(sizeof(*lreq), GFP_NOIO);
	if (!lreq)
		return NULL;

	kref_init(&lreq->kref);
	mutex_init(&lreq->lock);
	RB_CLEAR_NODE(&lreq->node);
	RB_CLEAR_NODE(&lreq->osdc_node);
	RB_CLEAR_NODE(&lreq->mc_node);
	INIT_LIST_HEAD(&lreq->scan_item);
	INIT_LIST_HEAD(&lreq->pending_lworks);
	init_completion(&lreq->reg_commit_wait);
	init_completion(&lreq->notify_finish_wait);

	lreq->osdc = osdc;
	target_init(&lreq->t);

	dout("%s lreq %p\n", __func__, lreq);
	return lreq;
}

DEFINE_RB_INSDEL_FUNCS(linger, struct ceph_osd_linger_request, linger_id, node)
DEFINE_RB_FUNCS(linger_osdc, struct ceph_osd_linger_request, linger_id, osdc_node)
DEFINE_RB_FUNCS(linger_mc, struct ceph_osd_linger_request, linger_id, mc_node)

/*
 * Create linger request <-> OSD session relation.
 *
 * @lreq has to be registered, @osd may be homeless.
 */
static void link_linger(struct ceph_osd *osd,
			struct ceph_osd_linger_request *lreq)
{
	verify_osd_locked(osd);
	WARN_ON(!lreq->linger_id || lreq->osd);
	dout("%s osd %p osd%d lreq %p linger_id %llu\n", __func__, osd,
	     osd->o_osd, lreq, lreq->linger_id);

	if (!osd_homeless(osd))
		__remove_osd_from_lru(osd);
	else
		atomic_inc(&osd->o_osdc->num_homeless);

	get_osd(osd);
	insert_linger(&osd->o_linger_requests, lreq);
	lreq->osd = osd;
}

static void unlink_linger(struct ceph_osd *osd,
			  struct ceph_osd_linger_request *lreq)
{
	verify_osd_locked(osd);
	WARN_ON(lreq->osd != osd);
	dout("%s osd %p osd%d lreq %p linger_id %llu\n", __func__, osd,
	     osd->o_osd, lreq, lreq->linger_id);

	lreq->osd = NULL;
	erase_linger(&osd->o_linger_requests, lreq);
	put_osd(osd);

	if (!osd_homeless(osd))
		maybe_move_osd_to_lru(osd);
	else
		atomic_dec(&osd->o_osdc->num_homeless);
}

static bool __linger_registered(struct ceph_osd_linger_request *lreq)
{
	verify_osdc_locked(lreq->osdc);

	return !RB_EMPTY_NODE(&lreq->osdc_node);
}

static bool linger_registered(struct ceph_osd_linger_request *lreq)
{
	struct ceph_osd_client *osdc = lreq->osdc;
	bool registered;

	down_read(&osdc->lock);
	registered = __linger_registered(lreq);
	up_read(&osdc->lock);

	return registered;
}

static void linger_register(struct ceph_osd_linger_request *lreq)
{
	struct ceph_osd_client *osdc = lreq->osdc;

	verify_osdc_wrlocked(osdc);
	WARN_ON(lreq->linger_id);

	linger_get(lreq);
	lreq->linger_id = ++osdc->last_linger_id;
	insert_linger_osdc(&osdc->linger_requests, lreq);
}

static void linger_unregister(struct ceph_osd_linger_request *lreq)
{
	struct ceph_osd_client *osdc = lreq->osdc;

	verify_osdc_wrlocked(osdc);

	erase_linger_osdc(&osdc->linger_requests, lreq);
	linger_put(lreq);
}

static void cancel_linger_request(struct ceph_osd_request *req)
{
	struct ceph_osd_linger_request *lreq = req->r_priv;

	WARN_ON(!req->r_linger);
	cancel_request(req);
	linger_put(lreq);
}

struct linger_work {
	struct work_struct work;
	struct ceph_osd_linger_request *lreq;
	struct list_head pending_item;
	unsigned long queued_stamp;

	union {
		struct {
			u64 notify_id;
			u64 notifier_id;
			void *payload; /* points into @msg front */
			size_t payload_len;

			struct ceph_msg *msg; /* for ceph_msg_put() */
		} notify;
		struct {
			int err;
		} error;
	};
};

static struct linger_work *lwork_alloc(struct ceph_osd_linger_request *lreq,
				       work_func_t workfn)
{
	struct linger_work *lwork;

	lwork = kzalloc(sizeof(*lwork), GFP_NOIO);
	if (!lwork)
		return NULL;

	INIT_WORK(&lwork->work, workfn);
	INIT_LIST_HEAD(&lwork->pending_item);
	lwork->lreq = linger_get(lreq);

	return lwork;
}

static void lwork_free(struct linger_work *lwork)
{
	struct ceph_osd_linger_request *lreq = lwork->lreq;

	mutex_lock(&lreq->lock);
	list_del(&lwork->pending_item);
	mutex_unlock(&lreq->lock);

	linger_put(lreq);
	kfree(lwork);
}

static void lwork_queue(struct linger_work *lwork)
{
	struct ceph_osd_linger_request *lreq = lwork->lreq;
	struct ceph_osd_client *osdc = lreq->osdc;

	verify_lreq_locked(lreq);
	WARN_ON(!list_empty(&lwork->pending_item));

	lwork->queued_stamp = jiffies;
	list_add_tail(&lwork->pending_item, &lreq->pending_lworks);
	queue_work(osdc->notify_wq, &lwork->work);
}

static void do_watch_notify(struct work_struct *w)
{
	struct linger_work *lwork = container_of(w, struct linger_work, work);
	struct ceph_osd_linger_request *lreq = lwork->lreq;

	if (!linger_registered(lreq)) {
		dout("%s lreq %p not registered\n", __func__, lreq);
		goto out;
	}

	WARN_ON(!lreq->is_watch);
	dout("%s lreq %p notify_id %llu notifier_id %llu payload_len %zu\n",
	     __func__, lreq, lwork->notify.notify_id, lwork->notify.notifier_id,
	     lwork->notify.payload_len);
	lreq->wcb(lreq->data, lwork->notify.notify_id, lreq->linger_id,
		  lwork->notify.notifier_id, lwork->notify.payload,
		  lwork->notify.payload_len);

out:
	ceph_msg_put(lwork->notify.msg);
	lwork_free(lwork);
}

static void do_watch_error(struct work_struct *w)
{
	struct linger_work *lwork = container_of(w, struct linger_work, work);
	struct ceph_osd_linger_request *lreq = lwork->lreq;

	if (!linger_registered(lreq)) {
		dout("%s lreq %p not registered\n", __func__, lreq);
		goto out;
	}

	dout("%s lreq %p err %d\n", __func__, lreq, lwork->error.err);
	lreq->errcb(lreq->data, lreq->linger_id, lwork->error.err);

out:
	lwork_free(lwork);
}

static void queue_watch_error(struct ceph_osd_linger_request *lreq)
{
	struct linger_work *lwork;

	lwork = lwork_alloc(lreq, do_watch_error);
	if (!lwork) {
		pr_err("failed to allocate error-lwork\n");
		return;
	}

	lwork->error.err = lreq->last_error;
	lwork_queue(lwork);
}

static void linger_reg_commit_complete(struct ceph_osd_linger_request *lreq,
				       int result)
{
	if (!completion_done(&lreq->reg_commit_wait)) {
		lreq->reg_commit_error = (result <= 0 ? result : 0);
		complete_all(&lreq->reg_commit_wait);
	}
}

static void linger_commit_cb(struct ceph_osd_request *req)
{
	struct ceph_osd_linger_request *lreq = req->r_priv;

	mutex_lock(&lreq->lock);
	dout("%s lreq %p linger_id %llu result %d\n", __func__, lreq,
	     lreq->linger_id, req->r_result);
	linger_reg_commit_complete(lreq, req->r_result);
	lreq->committed = true;

	if (!lreq->is_watch) {
		struct ceph_osd_data *osd_data =
		    osd_req_op_data(req, 0, notify, response_data);
		void *p = page_address(osd_data->pages[0]);

		WARN_ON(req->r_ops[0].op != CEPH_OSD_OP_NOTIFY ||
			osd_data->type != CEPH_OSD_DATA_TYPE_PAGES);

		/* make note of the notify_id */
		if (req->r_ops[0].outdata_len >= sizeof(u64)) {
			lreq->notify_id = ceph_decode_64(&p);
			dout("lreq %p notify_id %llu\n", lreq,
			     lreq->notify_id);
		} else {
			dout("lreq %p no notify_id\n", lreq);
		}
	}

	mutex_unlock(&lreq->lock);
	linger_put(lreq);
}

static int normalize_watch_error(int err)
{
	/*
	 * Translate ENOENT -> ENOTCONN so that a delete->disconnection
	 * notification and a failure to reconnect because we raced with
	 * the delete appear the same to the user.
	 */
	if (err == -ENOENT)
		err = -ENOTCONN;

	return err;
}

static void linger_reconnect_cb(struct ceph_osd_request *req)
{
	struct ceph_osd_linger_request *lreq = req->r_priv;

	mutex_lock(&lreq->lock);
	dout("%s lreq %p linger_id %llu result %d last_error %d\n", __func__,
	     lreq, lreq->linger_id, req->r_result, lreq->last_error);
	if (req->r_result < 0) {
		if (!lreq->last_error) {
			lreq->last_error = normalize_watch_error(req->r_result);
			queue_watch_error(lreq);
		}
	}

	mutex_unlock(&lreq->lock);
	linger_put(lreq);
}

static void send_linger(struct ceph_osd_linger_request *lreq)
{
	struct ceph_osd_request *req = lreq->reg_req;
	struct ceph_osd_req_op *op = &req->r_ops[0];

	verify_osdc_wrlocked(req->r_osdc);
	dout("%s lreq %p linger_id %llu\n", __func__, lreq, lreq->linger_id);

	if (req->r_osd)
		cancel_linger_request(req);

	request_reinit(req);
	target_copy(&req->r_t, &lreq->t);
	req->r_mtime = lreq->mtime;

	mutex_lock(&lreq->lock);
	if (lreq->is_watch && lreq->committed) {
		WARN_ON(op->op != CEPH_OSD_OP_WATCH ||
			op->watch.cookie != lreq->linger_id);
		op->watch.op = CEPH_OSD_WATCH_OP_RECONNECT;
		op->watch.gen = ++lreq->register_gen;
		dout("lreq %p reconnect register_gen %u\n", lreq,
		     op->watch.gen);
		req->r_callback = linger_reconnect_cb;
	} else {
		if (!lreq->is_watch)
			lreq->notify_id = 0;
		else
			WARN_ON(op->watch.op != CEPH_OSD_WATCH_OP_WATCH);
		dout("lreq %p register\n", lreq);
		req->r_callback = linger_commit_cb;
	}
	mutex_unlock(&lreq->lock);

	req->r_priv = linger_get(lreq);
	req->r_linger = true;

	submit_request(req, true);
}

static void linger_ping_cb(struct ceph_osd_request *req)
{
	struct ceph_osd_linger_request *lreq = req->r_priv;

	mutex_lock(&lreq->lock);
	dout("%s lreq %p linger_id %llu result %d ping_sent %lu last_error %d\n",
	     __func__, lreq, lreq->linger_id, req->r_result, lreq->ping_sent,
	     lreq->last_error);
	if (lreq->register_gen == req->r_ops[0].watch.gen) {
		if (!req->r_result) {
			lreq->watch_valid_thru = lreq->ping_sent;
		} else if (!lreq->last_error) {
			lreq->last_error = normalize_watch_error(req->r_result);
			queue_watch_error(lreq);
		}
	} else {
		dout("lreq %p register_gen %u ignoring old pong %u\n", lreq,
		     lreq->register_gen, req->r_ops[0].watch.gen);
	}

	mutex_unlock(&lreq->lock);
	linger_put(lreq);
}

static void send_linger_ping(struct ceph_osd_linger_request *lreq)
{
	struct ceph_osd_client *osdc = lreq->osdc;
	struct ceph_osd_request *req = lreq->ping_req;
	struct ceph_osd_req_op *op = &req->r_ops[0];

	if (ceph_osdmap_flag(osdc, CEPH_OSDMAP_PAUSERD)) {
		dout("%s PAUSERD\n", __func__);
		return;
	}

	lreq->ping_sent = jiffies;
	dout("%s lreq %p linger_id %llu ping_sent %lu register_gen %u\n",
	     __func__, lreq, lreq->linger_id, lreq->ping_sent,
	     lreq->register_gen);

	if (req->r_osd)
		cancel_linger_request(req);

	request_reinit(req);
	target_copy(&req->r_t, &lreq->t);

	WARN_ON(op->op != CEPH_OSD_OP_WATCH ||
		op->watch.cookie != lreq->linger_id ||
		op->watch.op != CEPH_OSD_WATCH_OP_PING);
	op->watch.gen = lreq->register_gen;
	req->r_callback = linger_ping_cb;
	req->r_priv = linger_get(lreq);
	req->r_linger = true;

	ceph_osdc_get_request(req);
	account_request(req);
	req->r_tid = atomic64_inc_return(&osdc->last_tid);
	link_request(lreq->osd, req);
	send_request(req);
}

static void linger_submit(struct ceph_osd_linger_request *lreq)
{
	struct ceph_osd_client *osdc = lreq->osdc;
	struct ceph_osd *osd;

	down_write(&osdc->lock);
	linger_register(lreq);
	if (lreq->is_watch) {
		lreq->reg_req->r_ops[0].watch.cookie = lreq->linger_id;
		lreq->ping_req->r_ops[0].watch.cookie = lreq->linger_id;
	} else {
		lreq->reg_req->r_ops[0].notify.cookie = lreq->linger_id;
	}

	calc_target(osdc, &lreq->t, false);
	osd = lookup_create_osd(osdc, lreq->t.osd, true);
	link_linger(osd, lreq);

	send_linger(lreq);
	up_write(&osdc->lock);
}

static void cancel_linger_map_check(struct ceph_osd_linger_request *lreq)
{
	struct ceph_osd_client *osdc = lreq->osdc;
	struct ceph_osd_linger_request *lookup_lreq;

	verify_osdc_wrlocked(osdc);

	lookup_lreq = lookup_linger_mc(&osdc->linger_map_checks,
				       lreq->linger_id);
	if (!lookup_lreq)
		return;

	WARN_ON(lookup_lreq != lreq);
	erase_linger_mc(&osdc->linger_map_checks, lreq);
	linger_put(lreq);
}

/*
 * @lreq has to be both registered and linked.
 */
static void __linger_cancel(struct ceph_osd_linger_request *lreq)
{
	if (lreq->is_watch && lreq->ping_req->r_osd)
		cancel_linger_request(lreq->ping_req);
	if (lreq->reg_req->r_osd)
		cancel_linger_request(lreq->reg_req);
	cancel_linger_map_check(lreq);
	unlink_linger(lreq->osd, lreq);
	linger_unregister(lreq);
}

static void linger_cancel(struct ceph_osd_linger_request *lreq)
{
	struct ceph_osd_client *osdc = lreq->osdc;

	down_write(&osdc->lock);
	if (__linger_registered(lreq))
		__linger_cancel(lreq);
	up_write(&osdc->lock);
}

static void send_linger_map_check(struct ceph_osd_linger_request *lreq);

static void check_linger_pool_dne(struct ceph_osd_linger_request *lreq)
{
	struct ceph_osd_client *osdc = lreq->osdc;
	struct ceph_osdmap *map = osdc->osdmap;

	verify_osdc_wrlocked(osdc);
	WARN_ON(!map->epoch);

	if (lreq->register_gen) {
		lreq->map_dne_bound = map->epoch;
		dout("%s lreq %p linger_id %llu pool disappeared\n", __func__,
		     lreq, lreq->linger_id);
	} else {
		dout("%s lreq %p linger_id %llu map_dne_bound %u have %u\n",
		     __func__, lreq, lreq->linger_id, lreq->map_dne_bound,
		     map->epoch);
	}

	if (lreq->map_dne_bound) {
		if (map->epoch >= lreq->map_dne_bound) {
			/* we had a new enough map */
			pr_info("linger_id %llu pool does not exist\n",
				lreq->linger_id);
			linger_reg_commit_complete(lreq, -ENOENT);
			__linger_cancel(lreq);
		}
	} else {
		send_linger_map_check(lreq);
	}
}

static void linger_map_check_cb(struct ceph_mon_generic_request *greq)
{
	struct ceph_osd_client *osdc = &greq->monc->client->osdc;
	struct ceph_osd_linger_request *lreq;
	u64 linger_id = greq->private_data;

	WARN_ON(greq->result || !greq->u.newest);

	down_write(&osdc->lock);
	lreq = lookup_linger_mc(&osdc->linger_map_checks, linger_id);
	if (!lreq) {
		dout("%s linger_id %llu dne\n", __func__, linger_id);
		goto out_unlock;
	}

	dout("%s lreq %p linger_id %llu map_dne_bound %u newest %llu\n",
	     __func__, lreq, lreq->linger_id, lreq->map_dne_bound,
	     greq->u.newest);
	if (!lreq->map_dne_bound)
		lreq->map_dne_bound = greq->u.newest;
	erase_linger_mc(&osdc->linger_map_checks, lreq);
	check_linger_pool_dne(lreq);

	linger_put(lreq);
out_unlock:
	up_write(&osdc->lock);
}

static void send_linger_map_check(struct ceph_osd_linger_request *lreq)
{
	struct ceph_osd_client *osdc = lreq->osdc;
	struct ceph_osd_linger_request *lookup_lreq;
	int ret;

	verify_osdc_wrlocked(osdc);

	lookup_lreq = lookup_linger_mc(&osdc->linger_map_checks,
				       lreq->linger_id);
	if (lookup_lreq) {
		WARN_ON(lookup_lreq != lreq);
		return;
	}

	linger_get(lreq);
	insert_linger_mc(&osdc->linger_map_checks, lreq);
	ret = ceph_monc_get_version_async(&osdc->client->monc, "osdmap",
					  linger_map_check_cb, lreq->linger_id);
	WARN_ON(ret);
}

static int linger_reg_commit_wait(struct ceph_osd_linger_request *lreq)
{
	int ret;

	dout("%s lreq %p linger_id %llu\n", __func__, lreq, lreq->linger_id);
	ret = wait_for_completion_interruptible(&lreq->reg_commit_wait);
	return ret ?: lreq->reg_commit_error;
}

static int linger_notify_finish_wait(struct ceph_osd_linger_request *lreq)
{
	int ret;

	dout("%s lreq %p linger_id %llu\n", __func__, lreq, lreq->linger_id);
	ret = wait_for_completion_interruptible(&lreq->notify_finish_wait);
	return ret ?: lreq->notify_finish_error;
}

/*
 * Timeout callback, called every N seconds.  When 1 or more OSD
 * requests has been active for more than N seconds, we send a keepalive
 * (tag + timestamp) to its OSD to ensure any communications channel
 * reset is detected.
 */
static void handle_timeout(struct work_struct *work)
{
	struct ceph_osd_client *osdc =
		container_of(work, struct ceph_osd_client, timeout_work.work);
	struct ceph_options *opts = osdc->client->options;
	unsigned long cutoff = jiffies - opts->osd_keepalive_timeout;
	unsigned long expiry_cutoff = jiffies - opts->osd_request_timeout;
	LIST_HEAD(slow_osds);
	struct rb_node *n, *p;

	dout("%s osdc %p\n", __func__, osdc);
	down_write(&osdc->lock);

	/*
	 * ping osds that are a bit slow.  this ensures that if there
	 * is a break in the TCP connection we will notice, and reopen
	 * a connection with that osd (from the fault callback).
	 */
	for (n = rb_first(&osdc->osds); n; n = rb_next(n)) {
		struct ceph_osd *osd = rb_entry(n, struct ceph_osd, o_node);
		bool found = false;

		for (p = rb_first(&osd->o_requests); p; ) {
			struct ceph_osd_request *req =
			    rb_entry(p, struct ceph_osd_request, r_node);

			p = rb_next(p); /* abort_request() */

			if (time_before(req->r_stamp, cutoff)) {
				dout(" req %p tid %llu on osd%d is laggy\n",
				     req, req->r_tid, osd->o_osd);
				found = true;
			}
			if (opts->osd_request_timeout &&
			    time_before(req->r_start_stamp, expiry_cutoff)) {
				pr_err_ratelimited("tid %llu on osd%d timeout\n",
				       req->r_tid, osd->o_osd);
				abort_request(req, -ETIMEDOUT);
			}
		}
		for (p = rb_first(&osd->o_linger_requests); p; p = rb_next(p)) {
			struct ceph_osd_linger_request *lreq =
			    rb_entry(p, struct ceph_osd_linger_request, node);

			dout(" lreq %p linger_id %llu is served by osd%d\n",
			     lreq, lreq->linger_id, osd->o_osd);
			found = true;

			mutex_lock(&lreq->lock);
			if (lreq->is_watch && lreq->committed && !lreq->last_error)
				send_linger_ping(lreq);
			mutex_unlock(&lreq->lock);
		}

		if (found)
			list_move_tail(&osd->o_keepalive_item, &slow_osds);
	}

	if (opts->osd_request_timeout) {
		for (p = rb_first(&osdc->homeless_osd.o_requests); p; ) {
			struct ceph_osd_request *req =
			    rb_entry(p, struct ceph_osd_request, r_node);

			p = rb_next(p); /* abort_request() */

			if (time_before(req->r_start_stamp, expiry_cutoff)) {
				pr_err_ratelimited("tid %llu on osd%d timeout\n",
				       req->r_tid, osdc->homeless_osd.o_osd);
				abort_request(req, -ETIMEDOUT);
			}
		}
	}

	if (atomic_read(&osdc->num_homeless) || !list_empty(&slow_osds))
		maybe_request_map(osdc);

	while (!list_empty(&slow_osds)) {
		struct ceph_osd *osd = list_first_entry(&slow_osds,
							struct ceph_osd,
							o_keepalive_item);
		list_del_init(&osd->o_keepalive_item);
		ceph_con_keepalive(&osd->o_con);
	}

	up_write(&osdc->lock);
	schedule_delayed_work(&osdc->timeout_work,
			      osdc->client->options->osd_keepalive_timeout);
}

static void handle_osds_timeout(struct work_struct *work)
{
	struct ceph_osd_client *osdc =
		container_of(work, struct ceph_osd_client,
			     osds_timeout_work.work);
	unsigned long delay = osdc->client->options->osd_idle_ttl / 4;
	struct ceph_osd *osd, *nosd;

	dout("%s osdc %p\n", __func__, osdc);
	down_write(&osdc->lock);
	list_for_each_entry_safe(osd, nosd, &osdc->osd_lru, o_osd_lru) {
		if (time_before(jiffies, osd->lru_ttl))
			break;

		WARN_ON(!RB_EMPTY_ROOT(&osd->o_requests));
		WARN_ON(!RB_EMPTY_ROOT(&osd->o_linger_requests));
		close_osd(osd);
	}

	up_write(&osdc->lock);
	schedule_delayed_work(&osdc->osds_timeout_work,
			      round_jiffies_relative(delay));
}

static int ceph_oloc_decode(void **p, void *end,
			    struct ceph_object_locator *oloc)
{
	u8 struct_v, struct_cv;
	u32 len;
	void *struct_end;
	int ret = 0;

	ceph_decode_need(p, end, 1 + 1 + 4, e_inval);
	struct_v = ceph_decode_8(p);
	struct_cv = ceph_decode_8(p);
	if (struct_v < 3) {
		pr_warn("got v %d < 3 cv %d of ceph_object_locator\n",
			struct_v, struct_cv);
		goto e_inval;
	}
	if (struct_cv > 6) {
		pr_warn("got v %d cv %d > 6 of ceph_object_locator\n",
			struct_v, struct_cv);
		goto e_inval;
	}
	len = ceph_decode_32(p);
	ceph_decode_need(p, end, len, e_inval);
	struct_end = *p + len;

	oloc->pool = ceph_decode_64(p);
	*p += 4; /* skip preferred */

	len = ceph_decode_32(p);
	if (len > 0) {
		pr_warn("ceph_object_locator::key is set\n");
		goto e_inval;
	}

	if (struct_v >= 5) {
		bool changed = false;

		len = ceph_decode_32(p);
		if (len > 0) {
			ceph_decode_need(p, end, len, e_inval);
			if (!oloc->pool_ns ||
			    ceph_compare_string(oloc->pool_ns, *p, len))
				changed = true;
			*p += len;
		} else {
			if (oloc->pool_ns)
				changed = true;
		}
		if (changed) {
			/* redirect changes namespace */
			pr_warn("ceph_object_locator::nspace is changed\n");
			goto e_inval;
		}
	}

	if (struct_v >= 6) {
		s64 hash = ceph_decode_64(p);
		if (hash != -1) {
			pr_warn("ceph_object_locator::hash is set\n");
			goto e_inval;
		}
	}

	/* skip the rest */
	*p = struct_end;
out:
	return ret;

e_inval:
	ret = -EINVAL;
	goto out;
}

static int ceph_redirect_decode(void **p, void *end,
				struct ceph_request_redirect *redir)
{
	u8 struct_v, struct_cv;
	u32 len;
	void *struct_end;
	int ret;

	ceph_decode_need(p, end, 1 + 1 + 4, e_inval);
	struct_v = ceph_decode_8(p);
	struct_cv = ceph_decode_8(p);
	if (struct_cv > 1) {
		pr_warn("got v %d cv %d > 1 of ceph_request_redirect\n",
			struct_v, struct_cv);
		goto e_inval;
	}
	len = ceph_decode_32(p);
	ceph_decode_need(p, end, len, e_inval);
	struct_end = *p + len;

	ret = ceph_oloc_decode(p, end, &redir->oloc);
	if (ret)
		goto out;

	len = ceph_decode_32(p);
	if (len > 0) {
		pr_warn("ceph_request_redirect::object_name is set\n");
		goto e_inval;
	}

	/* skip the rest */
	*p = struct_end;
out:
	return ret;

e_inval:
	ret = -EINVAL;
	goto out;
}

struct MOSDOpReply {
	struct ceph_pg pgid;
	u64 flags;
	int result;
	u32 epoch;
	int num_ops;
	u32 outdata_len[CEPH_OSD_MAX_OPS];
	s32 rval[CEPH_OSD_MAX_OPS];
	int retry_attempt;
	struct ceph_eversion replay_version;
	u64 user_version;
	struct ceph_request_redirect redirect;
};

static int decode_MOSDOpReply(const struct ceph_msg *msg, struct MOSDOpReply *m)
{
	void *p = msg->front.iov_base;
	void *const end = p + msg->front.iov_len;
	u16 version = le16_to_cpu(msg->hdr.version);
	struct ceph_eversion bad_replay_version;
	u8 decode_redir;
	u32 len;
	int ret;
	int i;

	ceph_decode_32_safe(&p, end, len, e_inval);
	ceph_decode_need(&p, end, len, e_inval);
	p += len; /* skip oid */

	ret = ceph_decode_pgid(&p, end, &m->pgid);
	if (ret)
		return ret;

	ceph_decode_64_safe(&p, end, m->flags, e_inval);
	ceph_decode_32_safe(&p, end, m->result, e_inval);
	ceph_decode_need(&p, end, sizeof(bad_replay_version), e_inval);
	memcpy(&bad_replay_version, p, sizeof(bad_replay_version));
	p += sizeof(bad_replay_version);
	ceph_decode_32_safe(&p, end, m->epoch, e_inval);

	ceph_decode_32_safe(&p, end, m->num_ops, e_inval);
	if (m->num_ops > ARRAY_SIZE(m->outdata_len))
		goto e_inval;

	ceph_decode_need(&p, end, m->num_ops * sizeof(struct ceph_osd_op),
			 e_inval);
	for (i = 0; i < m->num_ops; i++) {
		struct ceph_osd_op *op = p;

		m->outdata_len[i] = le32_to_cpu(op->payload_len);
		p += sizeof(*op);
	}

	ceph_decode_32_safe(&p, end, m->retry_attempt, e_inval);
	for (i = 0; i < m->num_ops; i++)
		ceph_decode_32_safe(&p, end, m->rval[i], e_inval);

	if (version >= 5) {
		ceph_decode_need(&p, end, sizeof(m->replay_version), e_inval);
		memcpy(&m->replay_version, p, sizeof(m->replay_version));
		p += sizeof(m->replay_version);
		ceph_decode_64_safe(&p, end, m->user_version, e_inval);
	} else {
		m->replay_version = bad_replay_version; /* struct */
		m->user_version = le64_to_cpu(m->replay_version.version);
	}

	if (version >= 6) {
		if (version >= 7)
			ceph_decode_8_safe(&p, end, decode_redir, e_inval);
		else
			decode_redir = 1;
	} else {
		decode_redir = 0;
	}

	if (decode_redir) {
		ret = ceph_redirect_decode(&p, end, &m->redirect);
		if (ret)
			return ret;
	} else {
		ceph_oloc_init(&m->redirect.oloc);
	}

	return 0;

e_inval:
	return -EINVAL;
}

/*
 * Handle MOSDOpReply.  Set ->r_result and call the callback if it is
 * specified.
 */
static void handle_reply(struct ceph_osd *osd, struct ceph_msg *msg)
{
	struct ceph_osd_client *osdc = osd->o_osdc;
	struct ceph_osd_request *req;
	struct MOSDOpReply m;
	u64 tid = le64_to_cpu(msg->hdr.tid);
	u32 data_len = 0;
	int ret;
	int i;

	dout("%s msg %p tid %llu\n", __func__, msg, tid);

	down_read(&osdc->lock);
	if (!osd_registered(osd)) {
		dout("%s osd%d unknown\n", __func__, osd->o_osd);
		goto out_unlock_osdc;
	}
	WARN_ON(osd->o_osd != le64_to_cpu(msg->hdr.src.num));

	mutex_lock(&osd->lock);
	req = lookup_request(&osd->o_requests, tid);
	if (!req) {
		dout("%s osd%d tid %llu unknown\n", __func__, osd->o_osd, tid);
		goto out_unlock_session;
	}

	m.redirect.oloc.pool_ns = req->r_t.target_oloc.pool_ns;
	ret = decode_MOSDOpReply(msg, &m);
	m.redirect.oloc.pool_ns = NULL;
	if (ret) {
		pr_err("failed to decode MOSDOpReply for tid %llu: %d\n",
		       req->r_tid, ret);
		ceph_msg_dump(msg);
		goto fail_request;
	}
	dout("%s req %p tid %llu flags 0x%llx pgid %llu.%x epoch %u attempt %d v %u'%llu uv %llu\n",
	     __func__, req, req->r_tid, m.flags, m.pgid.pool, m.pgid.seed,
	     m.epoch, m.retry_attempt, le32_to_cpu(m.replay_version.epoch),
	     le64_to_cpu(m.replay_version.version), m.user_version);

	if (m.retry_attempt >= 0) {
		if (m.retry_attempt != req->r_attempts - 1) {
			dout("req %p tid %llu retry_attempt %d != %d, ignoring\n",
			     req, req->r_tid, m.retry_attempt,
			     req->r_attempts - 1);
			goto out_unlock_session;
		}
	} else {
		WARN_ON(1); /* MOSDOpReply v4 is assumed */
	}

	if (!ceph_oloc_empty(&m.redirect.oloc)) {
		dout("req %p tid %llu redirect pool %lld\n", req, req->r_tid,
		     m.redirect.oloc.pool);
		unlink_request(osd, req);
		mutex_unlock(&osd->lock);

		/*
		 * Not ceph_oloc_copy() - changing pool_ns is not
		 * supported.
		 */
		req->r_t.target_oloc.pool = m.redirect.oloc.pool;
		req->r_flags |= CEPH_OSD_FLAG_REDIRECTED |
				CEPH_OSD_FLAG_IGNORE_OVERLAY |
				CEPH_OSD_FLAG_IGNORE_CACHE;
		req->r_tid = 0;
		__submit_request(req, false);
		goto out_unlock_osdc;
	}

	if (m.result == -EAGAIN) {
		dout("req %p tid %llu EAGAIN\n", req, req->r_tid);
		unlink_request(osd, req);
		mutex_unlock(&osd->lock);

		/*
		 * The object is missing on the replica or not (yet)
		 * readable.  Clear pgid to force a resend to the primary
		 * via legacy_change.
		 */
		req->r_t.pgid.pool = 0;
		req->r_t.pgid.seed = 0;
		WARN_ON(!req->r_t.used_replica);
		req->r_flags &= ~(CEPH_OSD_FLAG_BALANCE_READS |
				  CEPH_OSD_FLAG_LOCALIZE_READS);
		req->r_tid = 0;
		__submit_request(req, false);
		goto out_unlock_osdc;
	}

	if (m.num_ops != req->r_num_ops) {
		pr_err("num_ops %d != %d for tid %llu\n", m.num_ops,
		       req->r_num_ops, req->r_tid);
		goto fail_request;
	}
	for (i = 0; i < req->r_num_ops; i++) {
		dout(" req %p tid %llu op %d rval %d len %u\n", req,
		     req->r_tid, i, m.rval[i], m.outdata_len[i]);
		req->r_ops[i].rval = m.rval[i];
		req->r_ops[i].outdata_len = m.outdata_len[i];
		data_len += m.outdata_len[i];
	}
	if (data_len != le32_to_cpu(msg->hdr.data_len)) {
		pr_err("sum of lens %u != %u for tid %llu\n", data_len,
		       le32_to_cpu(msg->hdr.data_len), req->r_tid);
		goto fail_request;
	}
	dout("%s req %p tid %llu result %d data_len %u\n", __func__,
	     req, req->r_tid, m.result, data_len);

	/*
	 * Since we only ever request ONDISK, we should only ever get
	 * one (type of) reply back.
	 */
	WARN_ON(!(m.flags & CEPH_OSD_FLAG_ONDISK));
	req->r_result = m.result ?: data_len;
	finish_request(req);
	mutex_unlock(&osd->lock);
	up_read(&osdc->lock);

	__complete_request(req);
	return;

fail_request:
	complete_request(req, -EIO);
out_unlock_session:
	mutex_unlock(&osd->lock);
out_unlock_osdc:
	up_read(&osdc->lock);
}

static void set_pool_was_full(struct ceph_osd_client *osdc)
{
	struct rb_node *n;

	for (n = rb_first(&osdc->osdmap->pg_pools); n; n = rb_next(n)) {
		struct ceph_pg_pool_info *pi =
		    rb_entry(n, struct ceph_pg_pool_info, node);

		pi->was_full = __pool_full(pi);
	}
}

static bool pool_cleared_full(struct ceph_osd_client *osdc, s64 pool_id)
{
	struct ceph_pg_pool_info *pi;

	pi = ceph_pg_pool_by_id(osdc->osdmap, pool_id);
	if (!pi)
		return false;

	return pi->was_full && !__pool_full(pi);
}

static enum calc_target_result
recalc_linger_target(struct ceph_osd_linger_request *lreq)
{
	struct ceph_osd_client *osdc = lreq->osdc;
	enum calc_target_result ct_res;

	ct_res = calc_target(osdc, &lreq->t, true);
	if (ct_res == CALC_TARGET_NEED_RESEND) {
		struct ceph_osd *osd;

		osd = lookup_create_osd(osdc, lreq->t.osd, true);
		if (osd != lreq->osd) {
			unlink_linger(lreq->osd, lreq);
			link_linger(osd, lreq);
		}
	}

	return ct_res;
}

/*
 * Requeue requests whose mapping to an OSD has changed.
 */
static void scan_requests(struct ceph_osd *osd,
			  bool force_resend,
			  bool cleared_full,
			  bool check_pool_cleared_full,
			  struct rb_root *need_resend,
			  struct list_head *need_resend_linger)
{
	struct ceph_osd_client *osdc = osd->o_osdc;
	struct rb_node *n;
	bool force_resend_writes;

	for (n = rb_first(&osd->o_linger_requests); n; ) {
		struct ceph_osd_linger_request *lreq =
		    rb_entry(n, struct ceph_osd_linger_request, node);
		enum calc_target_result ct_res;

		n = rb_next(n); /* recalc_linger_target() */

		dout("%s lreq %p linger_id %llu\n", __func__, lreq,
		     lreq->linger_id);
		ct_res = recalc_linger_target(lreq);
		switch (ct_res) {
		case CALC_TARGET_NO_ACTION:
			force_resend_writes = cleared_full ||
			    (check_pool_cleared_full &&
			     pool_cleared_full(osdc, lreq->t.base_oloc.pool));
			if (!force_resend && !force_resend_writes)
				break;

			fallthrough;
		case CALC_TARGET_NEED_RESEND:
			cancel_linger_map_check(lreq);
			/*
			 * scan_requests() for the previous epoch(s)
			 * may have already added it to the list, since
			 * it's not unlinked here.
			 */
			if (list_empty(&lreq->scan_item))
				list_add_tail(&lreq->scan_item, need_resend_linger);
			break;
		case CALC_TARGET_POOL_DNE:
			list_del_init(&lreq->scan_item);
			check_linger_pool_dne(lreq);
			break;
		}
	}

	for (n = rb_first(&osd->o_requests); n; ) {
		struct ceph_osd_request *req =
		    rb_entry(n, struct ceph_osd_request, r_node);
		enum calc_target_result ct_res;

		n = rb_next(n); /* unlink_request(), check_pool_dne() */

		dout("%s req %p tid %llu\n", __func__, req, req->r_tid);
		ct_res = calc_target(osdc, &req->r_t, false);
		switch (ct_res) {
		case CALC_TARGET_NO_ACTION:
			force_resend_writes = cleared_full ||
			    (check_pool_cleared_full &&
			     pool_cleared_full(osdc, req->r_t.base_oloc.pool));
			if (!force_resend &&
			    (!(req->r_flags & CEPH_OSD_FLAG_WRITE) ||
			     !force_resend_writes))
				break;

			fallthrough;
		case CALC_TARGET_NEED_RESEND:
			cancel_map_check(req);
			unlink_request(osd, req);
			insert_request(need_resend, req);
			break;
		case CALC_TARGET_POOL_DNE:
			check_pool_dne(req);
			break;
		}
	}
}

static int handle_one_map(struct ceph_osd_client *osdc,
			  void *p, void *end, bool incremental,
			  struct rb_root *need_resend,
			  struct list_head *need_resend_linger)
{
	struct ceph_osdmap *newmap;
	struct rb_node *n;
	bool skipped_map = false;
	bool was_full;

	was_full = ceph_osdmap_flag(osdc, CEPH_OSDMAP_FULL);
	set_pool_was_full(osdc);

	if (incremental)
		newmap = osdmap_apply_incremental(&p, end,
						  ceph_msgr2(osdc->client),
						  osdc->osdmap);
	else
		newmap = ceph_osdmap_decode(&p, end, ceph_msgr2(osdc->client));
	if (IS_ERR(newmap))
		return PTR_ERR(newmap);

	if (newmap != osdc->osdmap) {
		/*
		 * Preserve ->was_full before destroying the old map.
		 * For pools that weren't in the old map, ->was_full
		 * should be false.
		 */
		for (n = rb_first(&newmap->pg_pools); n; n = rb_next(n)) {
			struct ceph_pg_pool_info *pi =
			    rb_entry(n, struct ceph_pg_pool_info, node);
			struct ceph_pg_pool_info *old_pi;

			old_pi = ceph_pg_pool_by_id(osdc->osdmap, pi->id);
			if (old_pi)
				pi->was_full = old_pi->was_full;
			else
				WARN_ON(pi->was_full);
		}

		if (osdc->osdmap->epoch &&
		    osdc->osdmap->epoch + 1 < newmap->epoch) {
			WARN_ON(incremental);
			skipped_map = true;
		}

		ceph_osdmap_destroy(osdc->osdmap);
		osdc->osdmap = newmap;
	}

	was_full &= !ceph_osdmap_flag(osdc, CEPH_OSDMAP_FULL);
	scan_requests(&osdc->homeless_osd, skipped_map, was_full, true,
		      need_resend, need_resend_linger);

	for (n = rb_first(&osdc->osds); n; ) {
		struct ceph_osd *osd = rb_entry(n, struct ceph_osd, o_node);

		n = rb_next(n); /* close_osd() */

		scan_requests(osd, skipped_map, was_full, true, need_resend,
			      need_resend_linger);
		if (!ceph_osd_is_up(osdc->osdmap, osd->o_osd) ||
		    memcmp(&osd->o_con.peer_addr,
			   ceph_osd_addr(osdc->osdmap, osd->o_osd),
			   sizeof(struct ceph_entity_addr)))
			close_osd(osd);
	}

	return 0;
}

static void kick_requests(struct ceph_osd_client *osdc,
			  struct rb_root *need_resend,
			  struct list_head *need_resend_linger)
{
	struct ceph_osd_linger_request *lreq, *nlreq;
	enum calc_target_result ct_res;
	struct rb_node *n;

	/* make sure need_resend targets reflect latest map */
	for (n = rb_first(need_resend); n; ) {
		struct ceph_osd_request *req =
		    rb_entry(n, struct ceph_osd_request, r_node);

		n = rb_next(n);

		if (req->r_t.epoch < osdc->osdmap->epoch) {
			ct_res = calc_target(osdc, &req->r_t, false);
			if (ct_res == CALC_TARGET_POOL_DNE) {
				erase_request(need_resend, req);
				check_pool_dne(req);
			}
		}
	}

	for (n = rb_first(need_resend); n; ) {
		struct ceph_osd_request *req =
		    rb_entry(n, struct ceph_osd_request, r_node);
		struct ceph_osd *osd;

		n = rb_next(n);
		erase_request(need_resend, req); /* before link_request() */

		osd = lookup_create_osd(osdc, req->r_t.osd, true);
		link_request(osd, req);
		if (!req->r_linger) {
			if (!osd_homeless(osd) && !req->r_t.paused)
				send_request(req);
		} else {
			cancel_linger_request(req);
		}
	}

	list_for_each_entry_safe(lreq, nlreq, need_resend_linger, scan_item) {
		if (!osd_homeless(lreq->osd))
			send_linger(lreq);

		list_del_init(&lreq->scan_item);
	}
}

/*
 * Process updated osd map.
 *
 * The message contains any number of incremental and full maps, normally
 * indicating some sort of topology change in the cluster.  Kick requests
 * off to different OSDs as needed.
 */
void ceph_osdc_handle_map(struct ceph_osd_client *osdc, struct ceph_msg *msg)
{
	void *p = msg->front.iov_base;
	void *const end = p + msg->front.iov_len;
	u32 nr_maps, maplen;
	u32 epoch;
	struct ceph_fsid fsid;
	struct rb_root need_resend = RB_ROOT;
	LIST_HEAD(need_resend_linger);
	bool handled_incremental = false;
	bool was_pauserd, was_pausewr;
	bool pauserd, pausewr;
	int err;

	dout("%s have %u\n", __func__, osdc->osdmap->epoch);
	down_write(&osdc->lock);

	/* verify fsid */
	ceph_decode_need(&p, end, sizeof(fsid), bad);
	ceph_decode_copy(&p, &fsid, sizeof(fsid));
	if (ceph_check_fsid(osdc->client, &fsid) < 0)
		goto bad;

	was_pauserd = ceph_osdmap_flag(osdc, CEPH_OSDMAP_PAUSERD);
	was_pausewr = ceph_osdmap_flag(osdc, CEPH_OSDMAP_PAUSEWR) ||
		      ceph_osdmap_flag(osdc, CEPH_OSDMAP_FULL) ||
		      have_pool_full(osdc);

	/* incremental maps */
	ceph_decode_32_safe(&p, end, nr_maps, bad);
	dout(" %d inc maps\n", nr_maps);
	while (nr_maps > 0) {
		ceph_decode_need(&p, end, 2*sizeof(u32), bad);
		epoch = ceph_decode_32(&p);
		maplen = ceph_decode_32(&p);
		ceph_decode_need(&p, end, maplen, bad);
		if (osdc->osdmap->epoch &&
		    osdc->osdmap->epoch + 1 == epoch) {
			dout("applying incremental map %u len %d\n",
			     epoch, maplen);
			err = handle_one_map(osdc, p, p + maplen, true,
					     &need_resend, &need_resend_linger);
			if (err)
				goto bad;
			handled_incremental = true;
		} else {
			dout("ignoring incremental map %u len %d\n",
			     epoch, maplen);
		}
		p += maplen;
		nr_maps--;
	}
	if (handled_incremental)
		goto done;

	/* full maps */
	ceph_decode_32_safe(&p, end, nr_maps, bad);
	dout(" %d full maps\n", nr_maps);
	while (nr_maps) {
		ceph_decode_need(&p, end, 2*sizeof(u32), bad);
		epoch = ceph_decode_32(&p);
		maplen = ceph_decode_32(&p);
		ceph_decode_need(&p, end, maplen, bad);
		if (nr_maps > 1) {
			dout("skipping non-latest full map %u len %d\n",
			     epoch, maplen);
		} else if (osdc->osdmap->epoch >= epoch) {
			dout("skipping full map %u len %d, "
			     "older than our %u\n", epoch, maplen,
			     osdc->osdmap->epoch);
		} else {
			dout("taking full map %u len %d\n", epoch, maplen);
			err = handle_one_map(osdc, p, p + maplen, false,
					     &need_resend, &need_resend_linger);
			if (err)
				goto bad;
		}
		p += maplen;
		nr_maps--;
	}

done:
	/*
	 * subscribe to subsequent osdmap updates if full to ensure
	 * we find out when we are no longer full and stop returning
	 * ENOSPC.
	 */
	pauserd = ceph_osdmap_flag(osdc, CEPH_OSDMAP_PAUSERD);
	pausewr = ceph_osdmap_flag(osdc, CEPH_OSDMAP_PAUSEWR) ||
		  ceph_osdmap_flag(osdc, CEPH_OSDMAP_FULL) ||
		  have_pool_full(osdc);
	if (was_pauserd || was_pausewr || pauserd || pausewr ||
	    osdc->osdmap->epoch < osdc->epoch_barrier)
		maybe_request_map(osdc);

	kick_requests(osdc, &need_resend, &need_resend_linger);

	ceph_osdc_abort_on_full(osdc);
	ceph_monc_got_map(&osdc->client->monc, CEPH_SUB_OSDMAP,
			  osdc->osdmap->epoch);
	up_write(&osdc->lock);
	wake_up_all(&osdc->client->auth_wq);
	return;

bad:
	pr_err("osdc handle_map corrupt msg\n");
	ceph_msg_dump(msg);
	up_write(&osdc->lock);
}

/*
 * Resubmit requests pending on the given osd.
 */
static void kick_osd_requests(struct ceph_osd *osd)
{
	struct rb_node *n;

	clear_backoffs(osd);

	for (n = rb_first(&osd->o_requests); n; ) {
		struct ceph_osd_request *req =
		    rb_entry(n, struct ceph_osd_request, r_node);

		n = rb_next(n); /* cancel_linger_request() */

		if (!req->r_linger) {
			if (!req->r_t.paused)
				send_request(req);
		} else {
			cancel_linger_request(req);
		}
	}
	for (n = rb_first(&osd->o_linger_requests); n; n = rb_next(n)) {
		struct ceph_osd_linger_request *lreq =
		    rb_entry(n, struct ceph_osd_linger_request, node);

		send_linger(lreq);
	}
}

/*
 * If the osd connection drops, we need to resubmit all requests.
 */
static void osd_fault(struct ceph_connection *con)
{
	struct ceph_osd *osd = con->private;
	struct ceph_osd_client *osdc = osd->o_osdc;

	dout("%s osd %p osd%d\n", __func__, osd, osd->o_osd);

	down_write(&osdc->lock);
	if (!osd_registered(osd)) {
		dout("%s osd%d unknown\n", __func__, osd->o_osd);
		goto out_unlock;
	}

	if (!reopen_osd(osd))
		kick_osd_requests(osd);
	maybe_request_map(osdc);

out_unlock:
	up_write(&osdc->lock);
}

struct MOSDBackoff {
	struct ceph_spg spgid;
	u32 map_epoch;
	u8 op;
	u64 id;
	struct ceph_hobject_id *begin;
	struct ceph_hobject_id *end;
};

static int decode_MOSDBackoff(const struct ceph_msg *msg, struct MOSDBackoff *m)
{
	void *p = msg->front.iov_base;
	void *const end = p + msg->front.iov_len;
	u8 struct_v;
	u32 struct_len;
	int ret;

	ret = ceph_start_decoding(&p, end, 1, "spg_t", &struct_v, &struct_len);
	if (ret)
		return ret;

	ret = ceph_decode_pgid(&p, end, &m->spgid.pgid);
	if (ret)
		return ret;

	ceph_decode_8_safe(&p, end, m->spgid.shard, e_inval);
	ceph_decode_32_safe(&p, end, m->map_epoch, e_inval);
	ceph_decode_8_safe(&p, end, m->op, e_inval);
	ceph_decode_64_safe(&p, end, m->id, e_inval);

	m->begin = kzalloc(sizeof(*m->begin), GFP_NOIO);
	if (!m->begin)
		return -ENOMEM;

	ret = decode_hoid(&p, end, m->begin);
	if (ret) {
		free_hoid(m->begin);
		return ret;
	}

	m->end = kzalloc(sizeof(*m->end), GFP_NOIO);
	if (!m->end) {
		free_hoid(m->begin);
		return -ENOMEM;
	}

	ret = decode_hoid(&p, end, m->end);
	if (ret) {
		free_hoid(m->begin);
		free_hoid(m->end);
		return ret;
	}

	return 0;

e_inval:
	return -EINVAL;
}

static struct ceph_msg *create_backoff_message(
				const struct ceph_osd_backoff *backoff,
				u32 map_epoch)
{
	struct ceph_msg *msg;
	void *p, *end;
	int msg_size;

	msg_size = CEPH_ENCODING_START_BLK_LEN +
			CEPH_PGID_ENCODING_LEN + 1; /* spgid */
	msg_size += 4 + 1 + 8; /* map_epoch, op, id */
	msg_size += CEPH_ENCODING_START_BLK_LEN +
			hoid_encoding_size(backoff->begin);
	msg_size += CEPH_ENCODING_START_BLK_LEN +
			hoid_encoding_size(backoff->end);

	msg = ceph_msg_new(CEPH_MSG_OSD_BACKOFF, msg_size, GFP_NOIO, true);
	if (!msg)
		return NULL;

	p = msg->front.iov_base;
	end = p + msg->front_alloc_len;

	encode_spgid(&p, &backoff->spgid);
	ceph_encode_32(&p, map_epoch);
	ceph_encode_8(&p, CEPH_OSD_BACKOFF_OP_ACK_BLOCK);
	ceph_encode_64(&p, backoff->id);
	encode_hoid(&p, end, backoff->begin);
	encode_hoid(&p, end, backoff->end);
	BUG_ON(p != end);

	msg->front.iov_len = p - msg->front.iov_base;
	msg->hdr.version = cpu_to_le16(1); /* MOSDBackoff v1 */
	msg->hdr.front_len = cpu_to_le32(msg->front.iov_len);

	return msg;
}

static void handle_backoff_block(struct ceph_osd *osd, struct MOSDBackoff *m)
{
	struct ceph_spg_mapping *spg;
	struct ceph_osd_backoff *backoff;
	struct ceph_msg *msg;

	dout("%s osd%d spgid %llu.%xs%d id %llu\n", __func__, osd->o_osd,
	     m->spgid.pgid.pool, m->spgid.pgid.seed, m->spgid.shard, m->id);

	spg = lookup_spg_mapping(&osd->o_backoff_mappings, &m->spgid);
	if (!spg) {
		spg = alloc_spg_mapping();
		if (!spg) {
			pr_err("%s failed to allocate spg\n", __func__);
			return;
		}
		spg->spgid = m->spgid; /* struct */
		insert_spg_mapping(&osd->o_backoff_mappings, spg);
	}

	backoff = alloc_backoff();
	if (!backoff) {
		pr_err("%s failed to allocate backoff\n", __func__);
		return;
	}
	backoff->spgid = m->spgid; /* struct */
	backoff->id = m->id;
	backoff->begin = m->begin;
	m->begin = NULL; /* backoff now owns this */
	backoff->end = m->end;
	m->end = NULL;   /* ditto */

	insert_backoff(&spg->backoffs, backoff);
	insert_backoff_by_id(&osd->o_backoffs_by_id, backoff);

	/*
	 * Ack with original backoff's epoch so that the OSD can
	 * discard this if there was a PG split.
	 */
	msg = create_backoff_message(backoff, m->map_epoch);
	if (!msg) {
		pr_err("%s failed to allocate msg\n", __func__);
		return;
	}
	ceph_con_send(&osd->o_con, msg);
}

static bool target_contained_by(const struct ceph_osd_request_target *t,
				const struct ceph_hobject_id *begin,
				const struct ceph_hobject_id *end)
{
	struct ceph_hobject_id hoid;
	int cmp;

	hoid_fill_from_target(&hoid, t);
	cmp = hoid_compare(&hoid, begin);
	return !cmp || (cmp > 0 && hoid_compare(&hoid, end) < 0);
}

static void handle_backoff_unblock(struct ceph_osd *osd,
				   const struct MOSDBackoff *m)
{
	struct ceph_spg_mapping *spg;
	struct ceph_osd_backoff *backoff;
	struct rb_node *n;

	dout("%s osd%d spgid %llu.%xs%d id %llu\n", __func__, osd->o_osd,
	     m->spgid.pgid.pool, m->spgid.pgid.seed, m->spgid.shard, m->id);

	backoff = lookup_backoff_by_id(&osd->o_backoffs_by_id, m->id);
	if (!backoff) {
		pr_err("%s osd%d spgid %llu.%xs%d id %llu backoff dne\n",
		       __func__, osd->o_osd, m->spgid.pgid.pool,
		       m->spgid.pgid.seed, m->spgid.shard, m->id);
		return;
	}

	if (hoid_compare(backoff->begin, m->begin) &&
	    hoid_compare(backoff->end, m->end)) {
		pr_err("%s osd%d spgid %llu.%xs%d id %llu bad range?\n",
		       __func__, osd->o_osd, m->spgid.pgid.pool,
		       m->spgid.pgid.seed, m->spgid.shard, m->id);
		/* unblock it anyway... */
	}

	spg = lookup_spg_mapping(&osd->o_backoff_mappings, &backoff->spgid);
	BUG_ON(!spg);

	erase_backoff(&spg->backoffs, backoff);
	erase_backoff_by_id(&osd->o_backoffs_by_id, backoff);
	free_backoff(backoff);

	if (RB_EMPTY_ROOT(&spg->backoffs)) {
		erase_spg_mapping(&osd->o_backoff_mappings, spg);
		free_spg_mapping(spg);
	}

	for (n = rb_first(&osd->o_requests); n; n = rb_next(n)) {
		struct ceph_osd_request *req =
		    rb_entry(n, struct ceph_osd_request, r_node);

		if (!ceph_spg_compare(&req->r_t.spgid, &m->spgid)) {
			/*
			 * Match against @m, not @backoff -- the PG may
			 * have split on the OSD.
			 */
			if (target_contained_by(&req->r_t, m->begin, m->end)) {
				/*
				 * If no other installed backoff applies,
				 * resend.
				 */
				send_request(req);
			}
		}
	}
}

static void handle_backoff(struct ceph_osd *osd, struct ceph_msg *msg)
{
	struct ceph_osd_client *osdc = osd->o_osdc;
	struct MOSDBackoff m;
	int ret;

	down_read(&osdc->lock);
	if (!osd_registered(osd)) {
		dout("%s osd%d unknown\n", __func__, osd->o_osd);
		up_read(&osdc->lock);
		return;
	}
	WARN_ON(osd->o_osd != le64_to_cpu(msg->hdr.src.num));

	mutex_lock(&osd->lock);
	ret = decode_MOSDBackoff(msg, &m);
	if (ret) {
		pr_err("failed to decode MOSDBackoff: %d\n", ret);
		ceph_msg_dump(msg);
		goto out_unlock;
	}

	switch (m.op) {
	case CEPH_OSD_BACKOFF_OP_BLOCK:
		handle_backoff_block(osd, &m);
		break;
	case CEPH_OSD_BACKOFF_OP_UNBLOCK:
		handle_backoff_unblock(osd, &m);
		break;
	default:
		pr_err("%s osd%d unknown op %d\n", __func__, osd->o_osd, m.op);
	}

	free_hoid(m.begin);
	free_hoid(m.end);

out_unlock:
	mutex_unlock(&osd->lock);
	up_read(&osdc->lock);
}

/*
 * Process osd watch notifications
 */
static void handle_watch_notify(struct ceph_osd_client *osdc,
				struct ceph_msg *msg)
{
	void *p = msg->front.iov_base;
	void *const end = p + msg->front.iov_len;
	struct ceph_osd_linger_request *lreq;
	struct linger_work *lwork;
	u8 proto_ver, opcode;
	u64 cookie, notify_id;
	u64 notifier_id = 0;
	s32 return_code = 0;
	void *payload = NULL;
	u32 payload_len = 0;

	ceph_decode_8_safe(&p, end, proto_ver, bad);
	ceph_decode_8_safe(&p, end, opcode, bad);
	ceph_decode_64_safe(&p, end, cookie, bad);
	p += 8; /* skip ver */
	ceph_decode_64_safe(&p, end, notify_id, bad);

	if (proto_ver >= 1) {
		ceph_decode_32_safe(&p, end, payload_len, bad);
		ceph_decode_need(&p, end, payload_len, bad);
		payload = p;
		p += payload_len;
	}

	if (le16_to_cpu(msg->hdr.version) >= 2)
		ceph_decode_32_safe(&p, end, return_code, bad);

	if (le16_to_cpu(msg->hdr.version) >= 3)
		ceph_decode_64_safe(&p, end, notifier_id, bad);

	down_read(&osdc->lock);
	lreq = lookup_linger_osdc(&osdc->linger_requests, cookie);
	if (!lreq) {
		dout("%s opcode %d cookie %llu dne\n", __func__, opcode,
		     cookie);
		goto out_unlock_osdc;
	}

	mutex_lock(&lreq->lock);
	dout("%s opcode %d cookie %llu lreq %p is_watch %d\n", __func__,
	     opcode, cookie, lreq, lreq->is_watch);
	if (opcode == CEPH_WATCH_EVENT_DISCONNECT) {
		if (!lreq->last_error) {
			lreq->last_error = -ENOTCONN;
			queue_watch_error(lreq);
		}
	} else if (!lreq->is_watch) {
		/* CEPH_WATCH_EVENT_NOTIFY_COMPLETE */
		if (lreq->notify_id && lreq->notify_id != notify_id) {
			dout("lreq %p notify_id %llu != %llu, ignoring\n", lreq,
			     lreq->notify_id, notify_id);
		} else if (!completion_done(&lreq->notify_finish_wait)) {
			struct ceph_msg_data *data =
			    msg->num_data_items ? &msg->data[0] : NULL;

			if (data) {
				if (lreq->preply_pages) {
					WARN_ON(data->type !=
							CEPH_MSG_DATA_PAGES);
					*lreq->preply_pages = data->pages;
					*lreq->preply_len = data->length;
					data->own_pages = false;
				}
			}
			lreq->notify_finish_error = return_code;
			complete_all(&lreq->notify_finish_wait);
		}
	} else {
		/* CEPH_WATCH_EVENT_NOTIFY */
		lwork = lwork_alloc(lreq, do_watch_notify);
		if (!lwork) {
			pr_err("failed to allocate notify-lwork\n");
			goto out_unlock_lreq;
		}

		lwork->notify.notify_id = notify_id;
		lwork->notify.notifier_id = notifier_id;
		lwork->notify.payload = payload;
		lwork->notify.payload_len = payload_len;
		lwork->notify.msg = ceph_msg_get(msg);
		lwork_queue(lwork);
	}

out_unlock_lreq:
	mutex_unlock(&lreq->lock);
out_unlock_osdc:
	up_read(&osdc->lock);
	return;

bad:
	pr_err("osdc handle_watch_notify corrupt msg\n");
}

/*
 * Register request, send initial attempt.
 */
int ceph_osdc_start_request(struct ceph_osd_client *osdc,
			    struct ceph_osd_request *req,
			    bool nofail)
{
	down_read(&osdc->lock);
	submit_request(req, false);
	up_read(&osdc->lock);

	return 0;
}
EXPORT_SYMBOL(ceph_osdc_start_request);

/*
 * Unregister a registered request.  The request is not completed:
 * ->r_result isn't set and __complete_request() isn't called.
 */
void ceph_osdc_cancel_request(struct ceph_osd_request *req)
{
	struct ceph_osd_client *osdc = req->r_osdc;

	down_write(&osdc->lock);
	if (req->r_osd)
		cancel_request(req);
	up_write(&osdc->lock);
}
EXPORT_SYMBOL(ceph_osdc_cancel_request);

/*
 * @timeout: in jiffies, 0 means "wait forever"
 */
static int wait_request_timeout(struct ceph_osd_request *req,
				unsigned long timeout)
{
	long left;

	dout("%s req %p tid %llu\n", __func__, req, req->r_tid);
	left = wait_for_completion_killable_timeout(&req->r_completion,
						ceph_timeout_jiffies(timeout));
	if (left <= 0) {
		left = left ?: -ETIMEDOUT;
		ceph_osdc_cancel_request(req);
	} else {
		left = req->r_result; /* completed */
	}

	return left;
}

/*
 * wait for a request to complete
 */
int ceph_osdc_wait_request(struct ceph_osd_client *osdc,
			   struct ceph_osd_request *req)
{
	return wait_request_timeout(req, 0);
}
EXPORT_SYMBOL(ceph_osdc_wait_request);

/*
 * sync - wait for all in-flight requests to flush.  avoid starvation.
 */
void ceph_osdc_sync(struct ceph_osd_client *osdc)
{
	struct rb_node *n, *p;
	u64 last_tid = atomic64_read(&osdc->last_tid);

again:
	down_read(&osdc->lock);
	for (n = rb_first(&osdc->osds); n; n = rb_next(n)) {
		struct ceph_osd *osd = rb_entry(n, struct ceph_osd, o_node);

		mutex_lock(&osd->lock);
		for (p = rb_first(&osd->o_requests); p; p = rb_next(p)) {
			struct ceph_osd_request *req =
			    rb_entry(p, struct ceph_osd_request, r_node);

			if (req->r_tid > last_tid)
				break;

			if (!(req->r_flags & CEPH_OSD_FLAG_WRITE))
				continue;

			ceph_osdc_get_request(req);
			mutex_unlock(&osd->lock);
			up_read(&osdc->lock);
			dout("%s waiting on req %p tid %llu last_tid %llu\n",
			     __func__, req, req->r_tid, last_tid);
			wait_for_completion(&req->r_completion);
			ceph_osdc_put_request(req);
			goto again;
		}

		mutex_unlock(&osd->lock);
	}

	up_read(&osdc->lock);
	dout("%s done last_tid %llu\n", __func__, last_tid);
}
EXPORT_SYMBOL(ceph_osdc_sync);

static struct ceph_osd_request *
alloc_linger_request(struct ceph_osd_linger_request *lreq)
{
	struct ceph_osd_request *req;

	req = ceph_osdc_alloc_request(lreq->osdc, NULL, 1, false, GFP_NOIO);
	if (!req)
		return NULL;

	ceph_oid_copy(&req->r_base_oid, &lreq->t.base_oid);
	ceph_oloc_copy(&req->r_base_oloc, &lreq->t.base_oloc);
	return req;
}

static struct ceph_osd_request *
alloc_watch_request(struct ceph_osd_linger_request *lreq, u8 watch_opcode)
{
	struct ceph_osd_request *req;

	req = alloc_linger_request(lreq);
	if (!req)
		return NULL;

	/*
	 * Pass 0 for cookie because we don't know it yet, it will be
	 * filled in by linger_submit().
	 */
	osd_req_op_watch_init(req, 0, 0, watch_opcode);

	if (ceph_osdc_alloc_messages(req, GFP_NOIO)) {
		ceph_osdc_put_request(req);
		return NULL;
	}

	return req;
}

/*
 * Returns a handle, caller owns a ref.
 */
struct ceph_osd_linger_request *
ceph_osdc_watch(struct ceph_osd_client *osdc,
		struct ceph_object_id *oid,
		struct ceph_object_locator *oloc,
		rados_watchcb2_t wcb,
		rados_watcherrcb_t errcb,
		void *data)
{
	struct ceph_osd_linger_request *lreq;
	int ret;

	lreq = linger_alloc(osdc);
	if (!lreq)
		return ERR_PTR(-ENOMEM);

	lreq->is_watch = true;
	lreq->wcb = wcb;
	lreq->errcb = errcb;
	lreq->data = data;
	lreq->watch_valid_thru = jiffies;

	ceph_oid_copy(&lreq->t.base_oid, oid);
	ceph_oloc_copy(&lreq->t.base_oloc, oloc);
	lreq->t.flags = CEPH_OSD_FLAG_WRITE;
	ktime_get_real_ts64(&lreq->mtime);

	lreq->reg_req = alloc_watch_request(lreq, CEPH_OSD_WATCH_OP_WATCH);
	if (!lreq->reg_req) {
		ret = -ENOMEM;
		goto err_put_lreq;
	}

	lreq->ping_req = alloc_watch_request(lreq, CEPH_OSD_WATCH_OP_PING);
	if (!lreq->ping_req) {
		ret = -ENOMEM;
		goto err_put_lreq;
	}

	linger_submit(lreq);
	ret = linger_reg_commit_wait(lreq);
	if (ret) {
		linger_cancel(lreq);
		goto err_put_lreq;
	}

	return lreq;

err_put_lreq:
	linger_put(lreq);
	return ERR_PTR(ret);
}
EXPORT_SYMBOL(ceph_osdc_watch);

/*
 * Releases a ref.
 *
 * Times out after mount_timeout to preserve rbd unmap behaviour
 * introduced in 2894e1d76974 ("rbd: timeout watch teardown on unmap
 * with mount_timeout").
 */
int ceph_osdc_unwatch(struct ceph_osd_client *osdc,
		      struct ceph_osd_linger_request *lreq)
{
	struct ceph_options *opts = osdc->client->options;
	struct ceph_osd_request *req;
	int ret;

	req = ceph_osdc_alloc_request(osdc, NULL, 1, false, GFP_NOIO);
	if (!req)
		return -ENOMEM;

	ceph_oid_copy(&req->r_base_oid, &lreq->t.base_oid);
	ceph_oloc_copy(&req->r_base_oloc, &lreq->t.base_oloc);
	req->r_flags = CEPH_OSD_FLAG_WRITE;
	ktime_get_real_ts64(&req->r_mtime);
	osd_req_op_watch_init(req, 0, lreq->linger_id,
			      CEPH_OSD_WATCH_OP_UNWATCH);

	ret = ceph_osdc_alloc_messages(req, GFP_NOIO);
	if (ret)
		goto out_put_req;

	ceph_osdc_start_request(osdc, req, false);
	linger_cancel(lreq);
	linger_put(lreq);
	ret = wait_request_timeout(req, opts->mount_timeout);

out_put_req:
	ceph_osdc_put_request(req);
	return ret;
}
EXPORT_SYMBOL(ceph_osdc_unwatch);

static int osd_req_op_notify_ack_init(struct ceph_osd_request *req, int which,
				      u64 notify_id, u64 cookie, void *payload,
				      u32 payload_len)
{
	struct ceph_osd_req_op *op;
	struct ceph_pagelist *pl;
	int ret;

	op = osd_req_op_init(req, which, CEPH_OSD_OP_NOTIFY_ACK, 0);

	pl = ceph_pagelist_alloc(GFP_NOIO);
	if (!pl)
		return -ENOMEM;

	ret = ceph_pagelist_encode_64(pl, notify_id);
	ret |= ceph_pagelist_encode_64(pl, cookie);
	if (payload) {
		ret |= ceph_pagelist_encode_32(pl, payload_len);
		ret |= ceph_pagelist_append(pl, payload, payload_len);
	} else {
		ret |= ceph_pagelist_encode_32(pl, 0);
	}
	if (ret) {
		ceph_pagelist_release(pl);
		return -ENOMEM;
	}

	ceph_osd_data_pagelist_init(&op->notify_ack.request_data, pl);
	op->indata_len = pl->length;
	return 0;
}

int ceph_osdc_notify_ack(struct ceph_osd_client *osdc,
			 struct ceph_object_id *oid,
			 struct ceph_object_locator *oloc,
			 u64 notify_id,
			 u64 cookie,
			 void *payload,
			 u32 payload_len)
{
	struct ceph_osd_request *req;
	int ret;

	req = ceph_osdc_alloc_request(osdc, NULL, 1, false, GFP_NOIO);
	if (!req)
		return -ENOMEM;

	ceph_oid_copy(&req->r_base_oid, oid);
	ceph_oloc_copy(&req->r_base_oloc, oloc);
	req->r_flags = CEPH_OSD_FLAG_READ;

	ret = osd_req_op_notify_ack_init(req, 0, notify_id, cookie, payload,
					 payload_len);
	if (ret)
		goto out_put_req;

	ret = ceph_osdc_alloc_messages(req, GFP_NOIO);
	if (ret)
		goto out_put_req;

	ceph_osdc_start_request(osdc, req, false);
	ret = ceph_osdc_wait_request(osdc, req);

out_put_req:
	ceph_osdc_put_request(req);
	return ret;
}
EXPORT_SYMBOL(ceph_osdc_notify_ack);

static int osd_req_op_notify_init(struct ceph_osd_request *req, int which,
				  u64 cookie, u32 prot_ver, u32 timeout,
				  void *payload, u32 payload_len)
{
	struct ceph_osd_req_op *op;
	struct ceph_pagelist *pl;
	int ret;

	op = osd_req_op_init(req, which, CEPH_OSD_OP_NOTIFY, 0);
	op->notify.cookie = cookie;

	pl = ceph_pagelist_alloc(GFP_NOIO);
	if (!pl)
		return -ENOMEM;

	ret = ceph_pagelist_encode_32(pl, 1); /* prot_ver */
	ret |= ceph_pagelist_encode_32(pl, timeout);
	ret |= ceph_pagelist_encode_32(pl, payload_len);
	ret |= ceph_pagelist_append(pl, payload, payload_len);
	if (ret) {
		ceph_pagelist_release(pl);
		return -ENOMEM;
	}

	ceph_osd_data_pagelist_init(&op->notify.request_data, pl);
	op->indata_len = pl->length;
	return 0;
}

/*
 * @timeout: in seconds
 *
 * @preply_{pages,len} are initialized both on success and error.
 * The caller is responsible for:
 *
 *     ceph_release_page_vector(reply_pages, calc_pages_for(0, reply_len))
 */
int ceph_osdc_notify(struct ceph_osd_client *osdc,
		     struct ceph_object_id *oid,
		     struct ceph_object_locator *oloc,
		     void *payload,
		     u32 payload_len,
		     u32 timeout,
		     struct page ***preply_pages,
		     size_t *preply_len)
{
	struct ceph_osd_linger_request *lreq;
	struct page **pages;
	int ret;

	WARN_ON(!timeout);
	if (preply_pages) {
		*preply_pages = NULL;
		*preply_len = 0;
	}

	lreq = linger_alloc(osdc);
	if (!lreq)
		return -ENOMEM;

	lreq->preply_pages = preply_pages;
	lreq->preply_len = preply_len;

	ceph_oid_copy(&lreq->t.base_oid, oid);
	ceph_oloc_copy(&lreq->t.base_oloc, oloc);
	lreq->t.flags = CEPH_OSD_FLAG_READ;

	lreq->reg_req = alloc_linger_request(lreq);
	if (!lreq->reg_req) {
		ret = -ENOMEM;
		goto out_put_lreq;
	}

	/*
	 * Pass 0 for cookie because we don't know it yet, it will be
	 * filled in by linger_submit().
	 */
	ret = osd_req_op_notify_init(lreq->reg_req, 0, 0, 1, timeout,
				     payload, payload_len);
	if (ret)
		goto out_put_lreq;

	/* for notify_id */
	pages = ceph_alloc_page_vector(1, GFP_NOIO);
	if (IS_ERR(pages)) {
		ret = PTR_ERR(pages);
		goto out_put_lreq;
	}
	ceph_osd_data_pages_init(osd_req_op_data(lreq->reg_req, 0, notify,
						 response_data),
				 pages, PAGE_SIZE, 0, false, true);

	ret = ceph_osdc_alloc_messages(lreq->reg_req, GFP_NOIO);
	if (ret)
		goto out_put_lreq;

	linger_submit(lreq);
	ret = linger_reg_commit_wait(lreq);
	if (!ret)
		ret = linger_notify_finish_wait(lreq);
	else
		dout("lreq %p failed to initiate notify %d\n", lreq, ret);

	linger_cancel(lreq);
out_put_lreq:
	linger_put(lreq);
	return ret;
}
EXPORT_SYMBOL(ceph_osdc_notify);

/*
 * Return the number of milliseconds since the watch was last
 * confirmed, or an error.  If there is an error, the watch is no
 * longer valid, and should be destroyed with ceph_osdc_unwatch().
 */
int ceph_osdc_watch_check(struct ceph_osd_client *osdc,
			  struct ceph_osd_linger_request *lreq)
{
	unsigned long stamp, age;
	int ret;

	down_read(&osdc->lock);
	mutex_lock(&lreq->lock);
	stamp = lreq->watch_valid_thru;
	if (!list_empty(&lreq->pending_lworks)) {
		struct linger_work *lwork =
		    list_first_entry(&lreq->pending_lworks,
				     struct linger_work,
				     pending_item);

		if (time_before(lwork->queued_stamp, stamp))
			stamp = lwork->queued_stamp;
	}
	age = jiffies - stamp;
	dout("%s lreq %p linger_id %llu age %lu last_error %d\n", __func__,
	     lreq, lreq->linger_id, age, lreq->last_error);
	/* we are truncating to msecs, so return a safe upper bound */
	ret = lreq->last_error ?: 1 + jiffies_to_msecs(age);

	mutex_unlock(&lreq->lock);
	up_read(&osdc->lock);
	return ret;
}

static int decode_watcher(void **p, void *end, struct ceph_watch_item *item)
{
	u8 struct_v;
	u32 struct_len;
	int ret;

	ret = ceph_start_decoding(p, end, 2, "watch_item_t",
				  &struct_v, &struct_len);
	if (ret)
		goto bad;

	ret = -EINVAL;
	ceph_decode_copy_safe(p, end, &item->name, sizeof(item->name), bad);
	ceph_decode_64_safe(p, end, item->cookie, bad);
	ceph_decode_skip_32(p, end, bad); /* skip timeout seconds */

	if (struct_v >= 2) {
		ret = ceph_decode_entity_addr(p, end, &item->addr);
		if (ret)
			goto bad;
	} else {
		ret = 0;
	}

	dout("%s %s%llu cookie %llu addr %s\n", __func__,
	     ENTITY_NAME(item->name), item->cookie,
	     ceph_pr_addr(&item->addr));
bad:
	return ret;
}

static int decode_watchers(void **p, void *end,
			   struct ceph_watch_item **watchers,
			   u32 *num_watchers)
{
	u8 struct_v;
	u32 struct_len;
	int i;
	int ret;

	ret = ceph_start_decoding(p, end, 1, "obj_list_watch_response_t",
				  &struct_v, &struct_len);
	if (ret)
		return ret;

	*num_watchers = ceph_decode_32(p);
	*watchers = kcalloc(*num_watchers, sizeof(**watchers), GFP_NOIO);
	if (!*watchers)
		return -ENOMEM;

	for (i = 0; i < *num_watchers; i++) {
		ret = decode_watcher(p, end, *watchers + i);
		if (ret) {
			kfree(*watchers);
			return ret;
		}
	}

	return 0;
}

/*
 * On success, the caller is responsible for:
 *
 *     kfree(watchers);
 */
int ceph_osdc_list_watchers(struct ceph_osd_client *osdc,
			    struct ceph_object_id *oid,
			    struct ceph_object_locator *oloc,
			    struct ceph_watch_item **watchers,
			    u32 *num_watchers)
{
	struct ceph_osd_request *req;
	struct page **pages;
	int ret;

	req = ceph_osdc_alloc_request(osdc, NULL, 1, false, GFP_NOIO);
	if (!req)
		return -ENOMEM;

	ceph_oid_copy(&req->r_base_oid, oid);
	ceph_oloc_copy(&req->r_base_oloc, oloc);
	req->r_flags = CEPH_OSD_FLAG_READ;

	pages = ceph_alloc_page_vector(1, GFP_NOIO);
	if (IS_ERR(pages)) {
		ret = PTR_ERR(pages);
		goto out_put_req;
	}

	osd_req_op_init(req, 0, CEPH_OSD_OP_LIST_WATCHERS, 0);
	ceph_osd_data_pages_init(osd_req_op_data(req, 0, list_watchers,
						 response_data),
				 pages, PAGE_SIZE, 0, false, true);

	ret = ceph_osdc_alloc_messages(req, GFP_NOIO);
	if (ret)
		goto out_put_req;

	ceph_osdc_start_request(osdc, req, false);
	ret = ceph_osdc_wait_request(osdc, req);
	if (ret >= 0) {
		void *p = page_address(pages[0]);
		void *const end = p + req->r_ops[0].outdata_len;

		ret = decode_watchers(&p, end, watchers, num_watchers);
	}

out_put_req:
	ceph_osdc_put_request(req);
	return ret;
}
EXPORT_SYMBOL(ceph_osdc_list_watchers);

/*
 * Call all pending notify callbacks - for use after a watch is
 * unregistered, to make sure no more callbacks for it will be invoked
 */
void ceph_osdc_flush_notifies(struct ceph_osd_client *osdc)
{
	dout("%s osdc %p\n", __func__, osdc);
	flush_workqueue(osdc->notify_wq);
}
EXPORT_SYMBOL(ceph_osdc_flush_notifies);

void ceph_osdc_maybe_request_map(struct ceph_osd_client *osdc)
{
	down_read(&osdc->lock);
	maybe_request_map(osdc);
	up_read(&osdc->lock);
}
EXPORT_SYMBOL(ceph_osdc_maybe_request_map);

/*
 * Execute an OSD class method on an object.
 *
 * @flags: CEPH_OSD_FLAG_*
 * @resp_len: in/out param for reply length
 */
int ceph_osdc_call(struct ceph_osd_client *osdc,
		   struct ceph_object_id *oid,
		   struct ceph_object_locator *oloc,
		   const char *class, const char *method,
		   unsigned int flags,
		   struct page *req_page, size_t req_len,
		   struct page **resp_pages, size_t *resp_len)
{
	struct ceph_osd_request *req;
	int ret;

	if (req_len > PAGE_SIZE)
		return -E2BIG;

	req = ceph_osdc_alloc_request(osdc, NULL, 1, false, GFP_NOIO);
	if (!req)
		return -ENOMEM;

	ceph_oid_copy(&req->r_base_oid, oid);
	ceph_oloc_copy(&req->r_base_oloc, oloc);
	req->r_flags = flags;

	ret = osd_req_op_cls_init(req, 0, class, method);
	if (ret)
		goto out_put_req;

	if (req_page)
		osd_req_op_cls_request_data_pages(req, 0, &req_page, req_len,
						  0, false, false);
	if (resp_pages)
		osd_req_op_cls_response_data_pages(req, 0, resp_pages,
						   *resp_len, 0, false, false);

	ret = ceph_osdc_alloc_messages(req, GFP_NOIO);
	if (ret)
		goto out_put_req;

	ceph_osdc_start_request(osdc, req, false);
	ret = ceph_osdc_wait_request(osdc, req);
	if (ret >= 0) {
		ret = req->r_ops[0].rval;
		if (resp_pages)
			*resp_len = req->r_ops[0].outdata_len;
	}

out_put_req:
	ceph_osdc_put_request(req);
	return ret;
}
EXPORT_SYMBOL(ceph_osdc_call);

/*
 * reset all osd connections
 */
void ceph_osdc_reopen_osds(struct ceph_osd_client *osdc)
{
	struct rb_node *n;

	down_write(&osdc->lock);
	for (n = rb_first(&osdc->osds); n; ) {
		struct ceph_osd *osd = rb_entry(n, struct ceph_osd, o_node);

		n = rb_next(n);
		if (!reopen_osd(osd))
			kick_osd_requests(osd);
	}
	up_write(&osdc->lock);
}

/*
 * init, shutdown
 */
int ceph_osdc_init(struct ceph_osd_client *osdc, struct ceph_client *client)
{
	int err;

	dout("init\n");
	osdc->client = client;
	init_rwsem(&osdc->lock);
	osdc->osds = RB_ROOT;
	INIT_LIST_HEAD(&osdc->osd_lru);
	spin_lock_init(&osdc->osd_lru_lock);
	osd_init(&osdc->homeless_osd);
	osdc->homeless_osd.o_osdc = osdc;
	osdc->homeless_osd.o_osd = CEPH_HOMELESS_OSD;
	osdc->last_linger_id = CEPH_LINGER_ID_START;
	osdc->linger_requests = RB_ROOT;
	osdc->map_checks = RB_ROOT;
	osdc->linger_map_checks = RB_ROOT;
	INIT_DELAYED_WORK(&osdc->timeout_work, handle_timeout);
	INIT_DELAYED_WORK(&osdc->osds_timeout_work, handle_osds_timeout);

	err = -ENOMEM;
	osdc->osdmap = ceph_osdmap_alloc();
	if (!osdc->osdmap)
		goto out;

	osdc->req_mempool = mempool_create_slab_pool(10,
						     ceph_osd_request_cache);
	if (!osdc->req_mempool)
		goto out_map;

	err = ceph_msgpool_init(&osdc->msgpool_op, CEPH_MSG_OSD_OP,
				PAGE_SIZE, CEPH_OSD_SLAB_OPS, 10, "osd_op");
	if (err < 0)
		goto out_mempool;
	err = ceph_msgpool_init(&osdc->msgpool_op_reply, CEPH_MSG_OSD_OPREPLY,
				PAGE_SIZE, CEPH_OSD_SLAB_OPS, 10,
				"osd_op_reply");
	if (err < 0)
		goto out_msgpool;

	err = -ENOMEM;
	osdc->notify_wq = create_singlethread_workqueue("ceph-watch-notify");
	if (!osdc->notify_wq)
		goto out_msgpool_reply;

	osdc->completion_wq = create_singlethread_workqueue("ceph-completion");
	if (!osdc->completion_wq)
		goto out_notify_wq;

	schedule_delayed_work(&osdc->timeout_work,
			      osdc->client->options->osd_keepalive_timeout);
	schedule_delayed_work(&osdc->osds_timeout_work,
	    round_jiffies_relative(osdc->client->options->osd_idle_ttl));

	return 0;

out_notify_wq:
	destroy_workqueue(osdc->notify_wq);
out_msgpool_reply:
	ceph_msgpool_destroy(&osdc->msgpool_op_reply);
out_msgpool:
	ceph_msgpool_destroy(&osdc->msgpool_op);
out_mempool:
	mempool_destroy(osdc->req_mempool);
out_map:
	ceph_osdmap_destroy(osdc->osdmap);
out:
	return err;
}

void ceph_osdc_stop(struct ceph_osd_client *osdc)
{
	destroy_workqueue(osdc->completion_wq);
	destroy_workqueue(osdc->notify_wq);
	cancel_delayed_work_sync(&osdc->timeout_work);
	cancel_delayed_work_sync(&osdc->osds_timeout_work);

	down_write(&osdc->lock);
	while (!RB_EMPTY_ROOT(&osdc->osds)) {
		struct ceph_osd *osd = rb_entry(rb_first(&osdc->osds),
						struct ceph_osd, o_node);
		close_osd(osd);
	}
	up_write(&osdc->lock);
	WARN_ON(refcount_read(&osdc->homeless_osd.o_ref) != 1);
	osd_cleanup(&osdc->homeless_osd);

	WARN_ON(!list_empty(&osdc->osd_lru));
	WARN_ON(!RB_EMPTY_ROOT(&osdc->linger_requests));
	WARN_ON(!RB_EMPTY_ROOT(&osdc->map_checks));
	WARN_ON(!RB_EMPTY_ROOT(&osdc->linger_map_checks));
	WARN_ON(atomic_read(&osdc->num_requests));
	WARN_ON(atomic_read(&osdc->num_homeless));

	ceph_osdmap_destroy(osdc->osdmap);
	mempool_destroy(osdc->req_mempool);
	ceph_msgpool_destroy(&osdc->msgpool_op);
	ceph_msgpool_destroy(&osdc->msgpool_op_reply);
}

static int osd_req_op_copy_from_init(struct ceph_osd_request *req,
				     u64 src_snapid, u64 src_version,
				     struct ceph_object_id *src_oid,
				     struct ceph_object_locator *src_oloc,
				     u32 src_fadvise_flags,
				     u32 dst_fadvise_flags,
				     u32 truncate_seq, u64 truncate_size,
				     u8 copy_from_flags)
{
	struct ceph_osd_req_op *op;
	struct page **pages;
	void *p, *end;

	pages = ceph_alloc_page_vector(1, GFP_KERNEL);
	if (IS_ERR(pages))
		return PTR_ERR(pages);

	op = osd_req_op_init(req, 0, CEPH_OSD_OP_COPY_FROM2,
			     dst_fadvise_flags);
	op->copy_from.snapid = src_snapid;
	op->copy_from.src_version = src_version;
	op->copy_from.flags = copy_from_flags;
	op->copy_from.src_fadvise_flags = src_fadvise_flags;

	p = page_address(pages[0]);
	end = p + PAGE_SIZE;
	ceph_encode_string(&p, end, src_oid->name, src_oid->name_len);
	encode_oloc(&p, end, src_oloc);
	ceph_encode_32(&p, truncate_seq);
	ceph_encode_64(&p, truncate_size);
	op->indata_len = PAGE_SIZE - (end - p);

	ceph_osd_data_pages_init(&op->copy_from.osd_data, pages,
				 op->indata_len, 0, false, true);
	return 0;
}

int ceph_osdc_copy_from(struct ceph_osd_client *osdc,
			u64 src_snapid, u64 src_version,
			struct ceph_object_id *src_oid,
			struct ceph_object_locator *src_oloc,
			u32 src_fadvise_flags,
			struct ceph_object_id *dst_oid,
			struct ceph_object_locator *dst_oloc,
			u32 dst_fadvise_flags,
			u32 truncate_seq, u64 truncate_size,
			u8 copy_from_flags)
{
	struct ceph_osd_request *req;
	int ret;

	req = ceph_osdc_alloc_request(osdc, NULL, 1, false, GFP_KERNEL);
	if (!req)
		return -ENOMEM;

	req->r_flags = CEPH_OSD_FLAG_WRITE;

	ceph_oloc_copy(&req->r_t.base_oloc, dst_oloc);
	ceph_oid_copy(&req->r_t.base_oid, dst_oid);

	ret = osd_req_op_copy_from_init(req, src_snapid, src_version, src_oid,
					src_oloc, src_fadvise_flags,
					dst_fadvise_flags, truncate_seq,
					truncate_size, copy_from_flags);
	if (ret)
		goto out;

	ret = ceph_osdc_alloc_messages(req, GFP_KERNEL);
	if (ret)
		goto out;

	ceph_osdc_start_request(osdc, req, false);
	ret = ceph_osdc_wait_request(osdc, req);

out:
	ceph_osdc_put_request(req);
	return ret;
}
EXPORT_SYMBOL(ceph_osdc_copy_from);

int __init ceph_osdc_setup(void)
{
	size_t size = sizeof(struct ceph_osd_request) +
	    CEPH_OSD_SLAB_OPS * sizeof(struct ceph_osd_req_op);

	BUG_ON(ceph_osd_request_cache);
	ceph_osd_request_cache = kmem_cache_create("ceph_osd_request", size,
						   0, 0, NULL);

	return ceph_osd_request_cache ? 0 : -ENOMEM;
}

void ceph_osdc_cleanup(void)
{
	BUG_ON(!ceph_osd_request_cache);
	kmem_cache_destroy(ceph_osd_request_cache);
	ceph_osd_request_cache = NULL;
}

/*
 * handle incoming message
 */
static void osd_dispatch(struct ceph_connection *con, struct ceph_msg *msg)
{
	struct ceph_osd *osd = con->private;
	struct ceph_osd_client *osdc = osd->o_osdc;
	int type = le16_to_cpu(msg->hdr.type);

	switch (type) {
	case CEPH_MSG_OSD_MAP:
		ceph_osdc_handle_map(osdc, msg);
		break;
	case CEPH_MSG_OSD_OPREPLY:
		handle_reply(osd, msg);
		break;
	case CEPH_MSG_OSD_BACKOFF:
		handle_backoff(osd, msg);
		break;
	case CEPH_MSG_WATCH_NOTIFY:
		handle_watch_notify(osdc, msg);
		break;

	default:
		pr_err("received unknown message type %d %s\n", type,
		       ceph_msg_type_name(type));
	}

	ceph_msg_put(msg);
}

/*
 * Lookup and return message for incoming reply.  Don't try to do
 * anything about a larger than preallocated data portion of the
 * message at the moment - for now, just skip the message.
 */
static struct ceph_msg *get_reply(struct ceph_connection *con,
				  struct ceph_msg_header *hdr,
				  int *skip)
{
	struct ceph_osd *osd = con->private;
	struct ceph_osd_client *osdc = osd->o_osdc;
	struct ceph_msg *m = NULL;
	struct ceph_osd_request *req;
	int front_len = le32_to_cpu(hdr->front_len);
	int data_len = le32_to_cpu(hdr->data_len);
	u64 tid = le64_to_cpu(hdr->tid);

	down_read(&osdc->lock);
	if (!osd_registered(osd)) {
		dout("%s osd%d unknown, skipping\n", __func__, osd->o_osd);
		*skip = 1;
		goto out_unlock_osdc;
	}
	WARN_ON(osd->o_osd != le64_to_cpu(hdr->src.num));

	mutex_lock(&osd->lock);
	req = lookup_request(&osd->o_requests, tid);
	if (!req) {
		dout("%s osd%d tid %llu unknown, skipping\n", __func__,
		     osd->o_osd, tid);
		*skip = 1;
		goto out_unlock_session;
	}

	ceph_msg_revoke_incoming(req->r_reply);

	if (front_len > req->r_reply->front_alloc_len) {
		pr_warn("%s osd%d tid %llu front %d > preallocated %d\n",
			__func__, osd->o_osd, req->r_tid, front_len,
			req->r_reply->front_alloc_len);
		m = ceph_msg_new(CEPH_MSG_OSD_OPREPLY, front_len, GFP_NOFS,
				 false);
		if (!m)
			goto out_unlock_session;
		ceph_msg_put(req->r_reply);
		req->r_reply = m;
	}

	if (data_len > req->r_reply->data_length) {
		pr_warn("%s osd%d tid %llu data %d > preallocated %zu, skipping\n",
			__func__, osd->o_osd, req->r_tid, data_len,
			req->r_reply->data_length);
		m = NULL;
		*skip = 1;
		goto out_unlock_session;
	}

	m = ceph_msg_get(req->r_reply);
	dout("get_reply tid %lld %p\n", tid, m);

out_unlock_session:
	mutex_unlock(&osd->lock);
out_unlock_osdc:
	up_read(&osdc->lock);
	return m;
}

static struct ceph_msg *alloc_msg_with_page_vector(struct ceph_msg_header *hdr)
{
	struct ceph_msg *m;
	int type = le16_to_cpu(hdr->type);
	u32 front_len = le32_to_cpu(hdr->front_len);
	u32 data_len = le32_to_cpu(hdr->data_len);

	m = ceph_msg_new2(type, front_len, 1, GFP_NOIO, false);
	if (!m)
		return NULL;

	if (data_len) {
		struct page **pages;

		pages = ceph_alloc_page_vector(calc_pages_for(0, data_len),
					       GFP_NOIO);
		if (IS_ERR(pages)) {
			ceph_msg_put(m);
			return NULL;
		}

		ceph_msg_data_add_pages(m, pages, data_len, 0, true);
	}

	return m;
}

static struct ceph_msg *osd_alloc_msg(struct ceph_connection *con,
				      struct ceph_msg_header *hdr,
				      int *skip)
{
	struct ceph_osd *osd = con->private;
	int type = le16_to_cpu(hdr->type);

	*skip = 0;
	switch (type) {
	case CEPH_MSG_OSD_MAP:
	case CEPH_MSG_OSD_BACKOFF:
	case CEPH_MSG_WATCH_NOTIFY:
		return alloc_msg_with_page_vector(hdr);
	case CEPH_MSG_OSD_OPREPLY:
		return get_reply(con, hdr, skip);
	default:
		pr_warn("%s osd%d unknown msg type %d, skipping\n", __func__,
			osd->o_osd, type);
		*skip = 1;
		return NULL;
	}
}

/*
 * Wrappers to refcount containing ceph_osd struct
 */
static struct ceph_connection *osd_get_con(struct ceph_connection *con)
{
	struct ceph_osd *osd = con->private;
	if (get_osd(osd))
		return con;
	return NULL;
}

static void osd_put_con(struct ceph_connection *con)
{
	struct ceph_osd *osd = con->private;
	put_osd(osd);
}

/*
 * authentication
 */

/*
 * Note: returned pointer is the address of a structure that's
 * managed separately.  Caller must *not* attempt to free it.
 */
static struct ceph_auth_handshake *
osd_get_authorizer(struct ceph_connection *con, int *proto, int force_new)
{
	struct ceph_osd *o = con->private;
	struct ceph_osd_client *osdc = o->o_osdc;
	struct ceph_auth_client *ac = osdc->client->monc.auth;
	struct ceph_auth_handshake *auth = &o->o_auth;
	int ret;

	ret = __ceph_auth_get_authorizer(ac, auth, CEPH_ENTITY_TYPE_OSD,
					 force_new, proto, NULL, NULL);
	if (ret)
		return ERR_PTR(ret);

	return auth;
}

static int osd_add_authorizer_challenge(struct ceph_connection *con,
				    void *challenge_buf, int challenge_buf_len)
{
	struct ceph_osd *o = con->private;
	struct ceph_osd_client *osdc = o->o_osdc;
	struct ceph_auth_client *ac = osdc->client->monc.auth;

	return ceph_auth_add_authorizer_challenge(ac, o->o_auth.authorizer,
					    challenge_buf, challenge_buf_len);
}

static int osd_verify_authorizer_reply(struct ceph_connection *con)
{
	struct ceph_osd *o = con->private;
	struct ceph_osd_client *osdc = o->o_osdc;
	struct ceph_auth_client *ac = osdc->client->monc.auth;
	struct ceph_auth_handshake *auth = &o->o_auth;

	return ceph_auth_verify_authorizer_reply(ac, auth->authorizer,
		auth->authorizer_reply_buf, auth->authorizer_reply_buf_len,
		NULL, NULL, NULL, NULL);
}

static int osd_invalidate_authorizer(struct ceph_connection *con)
{
	struct ceph_osd *o = con->private;
	struct ceph_osd_client *osdc = o->o_osdc;
	struct ceph_auth_client *ac = osdc->client->monc.auth;

	ceph_auth_invalidate_authorizer(ac, CEPH_ENTITY_TYPE_OSD);
	return ceph_monc_validate_auth(&osdc->client->monc);
}

static int osd_get_auth_request(struct ceph_connection *con,
				void *buf, int *buf_len,
				void **authorizer, int *authorizer_len)
{
	struct ceph_osd *o = con->private;
	struct ceph_auth_client *ac = o->o_osdc->client->monc.auth;
	struct ceph_auth_handshake *auth = &o->o_auth;
	int ret;

	ret = ceph_auth_get_authorizer(ac, auth, CEPH_ENTITY_TYPE_OSD,
				       buf, buf_len);
	if (ret)
		return ret;

	*authorizer = auth->authorizer_buf;
	*authorizer_len = auth->authorizer_buf_len;
	return 0;
}

static int osd_handle_auth_reply_more(struct ceph_connection *con,
				      void *reply, int reply_len,
				      void *buf, int *buf_len,
				      void **authorizer, int *authorizer_len)
{
	struct ceph_osd *o = con->private;
	struct ceph_auth_client *ac = o->o_osdc->client->monc.auth;
	struct ceph_auth_handshake *auth = &o->o_auth;
	int ret;

	ret = ceph_auth_handle_svc_reply_more(ac, auth, reply, reply_len,
					      buf, buf_len);
	if (ret)
		return ret;

	*authorizer = auth->authorizer_buf;
	*authorizer_len = auth->authorizer_buf_len;
	return 0;
}

static int osd_handle_auth_done(struct ceph_connection *con,
				u64 global_id, void *reply, int reply_len,
				u8 *session_key, int *session_key_len,
				u8 *con_secret, int *con_secret_len)
{
	struct ceph_osd *o = con->private;
	struct ceph_auth_client *ac = o->o_osdc->client->monc.auth;
	struct ceph_auth_handshake *auth = &o->o_auth;

	return ceph_auth_handle_svc_reply_done(ac, auth, reply, reply_len,
					       session_key, session_key_len,
					       con_secret, con_secret_len);
}

static int osd_handle_auth_bad_method(struct ceph_connection *con,
				      int used_proto, int result,
				      const int *allowed_protos, int proto_cnt,
				      const int *allowed_modes, int mode_cnt)
{
	struct ceph_osd *o = con->private;
	struct ceph_mon_client *monc = &o->o_osdc->client->monc;
	int ret;

	if (ceph_auth_handle_bad_authorizer(monc->auth, CEPH_ENTITY_TYPE_OSD,
					    used_proto, result,
					    allowed_protos, proto_cnt,
					    allowed_modes, mode_cnt)) {
		ret = ceph_monc_validate_auth(monc);
		if (ret)
			return ret;
	}

	return -EACCES;
}

static void osd_reencode_message(struct ceph_msg *msg)
{
	int type = le16_to_cpu(msg->hdr.type);

	if (type == CEPH_MSG_OSD_OP)
		encode_request_finish(msg);
}

static int osd_sign_message(struct ceph_msg *msg)
{
	struct ceph_osd *o = msg->con->private;
	struct ceph_auth_handshake *auth = &o->o_auth;

	return ceph_auth_sign_message(auth, msg);
}

static int osd_check_message_signature(struct ceph_msg *msg)
{
	struct ceph_osd *o = msg->con->private;
	struct ceph_auth_handshake *auth = &o->o_auth;

	return ceph_auth_check_message_signature(auth, msg);
}

static const struct ceph_connection_operations osd_con_ops = {
	.get = osd_get_con,
	.put = osd_put_con,
	.alloc_msg = osd_alloc_msg,
	.dispatch = osd_dispatch,
	.fault = osd_fault,
	.reencode_message = osd_reencode_message,
	.get_authorizer = osd_get_authorizer,
	.add_authorizer_challenge = osd_add_authorizer_challenge,
	.verify_authorizer_reply = osd_verify_authorizer_reply,
	.invalidate_authorizer = osd_invalidate_authorizer,
	.sign_message = osd_sign_message,
	.check_message_signature = osd_check_message_signature,
	.get_auth_request = osd_get_auth_request,
	.handle_auth_reply_more = osd_handle_auth_reply_more,
	.handle_auth_done = osd_handle_auth_done,
	.handle_auth_bad_method = osd_handle_auth_bad_method,
};
