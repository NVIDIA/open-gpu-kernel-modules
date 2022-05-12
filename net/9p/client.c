// SPDX-License-Identifier: GPL-2.0-only
/*
 * net/9p/clnt.c
 *
 * 9P Client
 *
 *  Copyright (C) 2008 by Eric Van Hensbergen <ericvh@gmail.com>
 *  Copyright (C) 2007 by Latchesar Ionkov <lucho@ionkov.net>
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/module.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/poll.h>
#include <linux/idr.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/sched/signal.h>
#include <linux/uaccess.h>
#include <linux/uio.h>
#include <net/9p/9p.h>
#include <linux/parser.h>
#include <linux/seq_file.h>
#include <net/9p/client.h>
#include <net/9p/transport.h>
#include "protocol.h"

#define CREATE_TRACE_POINTS
#include <trace/events/9p.h>

/*
  * Client Option Parsing (code inspired by NFS code)
  *  - a little lazy - parse all client options
  */

enum {
	Opt_msize,
	Opt_trans,
	Opt_legacy,
	Opt_version,
	Opt_err,
};

static const match_table_t tokens = {
	{Opt_msize, "msize=%u"},
	{Opt_legacy, "noextend"},
	{Opt_trans, "trans=%s"},
	{Opt_version, "version=%s"},
	{Opt_err, NULL},
};

inline int p9_is_proto_dotl(struct p9_client *clnt)
{
	return clnt->proto_version == p9_proto_2000L;
}
EXPORT_SYMBOL(p9_is_proto_dotl);

inline int p9_is_proto_dotu(struct p9_client *clnt)
{
	return clnt->proto_version == p9_proto_2000u;
}
EXPORT_SYMBOL(p9_is_proto_dotu);

int p9_show_client_options(struct seq_file *m, struct p9_client *clnt)
{
	if (clnt->msize != 8192)
		seq_printf(m, ",msize=%u", clnt->msize);
	seq_printf(m, ",trans=%s", clnt->trans_mod->name);

	switch (clnt->proto_version) {
	case p9_proto_legacy:
		seq_puts(m, ",noextend");
		break;
	case p9_proto_2000u:
		seq_puts(m, ",version=9p2000.u");
		break;
	case p9_proto_2000L:
		/* Default */
		break;
	}

	if (clnt->trans_mod->show_options)
		return clnt->trans_mod->show_options(m, clnt);
	return 0;
}
EXPORT_SYMBOL(p9_show_client_options);

/*
 * Some error codes are taken directly from the server replies,
 * make sure they are valid.
 */
static int safe_errno(int err)
{
	if ((err > 0) || (err < -MAX_ERRNO)) {
		p9_debug(P9_DEBUG_ERROR, "Invalid error code %d\n", err);
		return -EPROTO;
	}
	return err;
}


/* Interpret mount option for protocol version */
static int get_protocol_version(char *s)
{
	int version = -EINVAL;

	if (!strcmp(s, "9p2000")) {
		version = p9_proto_legacy;
		p9_debug(P9_DEBUG_9P, "Protocol version: Legacy\n");
	} else if (!strcmp(s, "9p2000.u")) {
		version = p9_proto_2000u;
		p9_debug(P9_DEBUG_9P, "Protocol version: 9P2000.u\n");
	} else if (!strcmp(s, "9p2000.L")) {
		version = p9_proto_2000L;
		p9_debug(P9_DEBUG_9P, "Protocol version: 9P2000.L\n");
	} else
		pr_info("Unknown protocol version %s\n", s);

	return version;
}

/**
 * parse_opts - parse mount options into client structure
 * @opts: options string passed from mount
 * @clnt: existing v9fs client information
 *
 * Return 0 upon success, -ERRNO upon failure
 */

static int parse_opts(char *opts, struct p9_client *clnt)
{
	char *options, *tmp_options;
	char *p;
	substring_t args[MAX_OPT_ARGS];
	int option;
	char *s;
	int ret = 0;

	clnt->proto_version = p9_proto_2000L;
	clnt->msize = 8192;

	if (!opts)
		return 0;

	tmp_options = kstrdup(opts, GFP_KERNEL);
	if (!tmp_options) {
		p9_debug(P9_DEBUG_ERROR,
			 "failed to allocate copy of option string\n");
		return -ENOMEM;
	}
	options = tmp_options;

	while ((p = strsep(&options, ",")) != NULL) {
		int token, r;
		if (!*p)
			continue;
		token = match_token(p, tokens, args);
		switch (token) {
		case Opt_msize:
			r = match_int(&args[0], &option);
			if (r < 0) {
				p9_debug(P9_DEBUG_ERROR,
					 "integer field, but no integer?\n");
				ret = r;
				continue;
			}
			if (option < 4096) {
				p9_debug(P9_DEBUG_ERROR,
					 "msize should be at least 4k\n");
				ret = -EINVAL;
				continue;
			}
			clnt->msize = option;
			break;
		case Opt_trans:
			s = match_strdup(&args[0]);
			if (!s) {
				ret = -ENOMEM;
				p9_debug(P9_DEBUG_ERROR,
					 "problem allocating copy of trans arg\n");
				goto free_and_return;
			}

			v9fs_put_trans(clnt->trans_mod);
			clnt->trans_mod = v9fs_get_trans_by_name(s);
			if (clnt->trans_mod == NULL) {
				pr_info("Could not find request transport: %s\n",
					s);
				ret = -EINVAL;
			}
			kfree(s);
			break;
		case Opt_legacy:
			clnt->proto_version = p9_proto_legacy;
			break;
		case Opt_version:
			s = match_strdup(&args[0]);
			if (!s) {
				ret = -ENOMEM;
				p9_debug(P9_DEBUG_ERROR,
					 "problem allocating copy of version arg\n");
				goto free_and_return;
			}
			r = get_protocol_version(s);
			if (r < 0)
				ret = r;
			else
				clnt->proto_version = r;
			kfree(s);
			break;
		default:
			continue;
		}
	}

free_and_return:
	if (ret)
		v9fs_put_trans(clnt->trans_mod);
	kfree(tmp_options);
	return ret;
}

static int p9_fcall_init(struct p9_client *c, struct p9_fcall *fc,
			 int alloc_msize)
{
	if (likely(c->fcall_cache) && alloc_msize == c->msize) {
		fc->sdata = kmem_cache_alloc(c->fcall_cache, GFP_NOFS);
		fc->cache = c->fcall_cache;
	} else {
		fc->sdata = kmalloc(alloc_msize, GFP_NOFS);
		fc->cache = NULL;
	}
	if (!fc->sdata)
		return -ENOMEM;
	fc->capacity = alloc_msize;
	return 0;
}

void p9_fcall_fini(struct p9_fcall *fc)
{
	/* sdata can be NULL for interrupted requests in trans_rdma,
	 * and kmem_cache_free does not do NULL-check for us
	 */
	if (unlikely(!fc->sdata))
		return;

	if (fc->cache)
		kmem_cache_free(fc->cache, fc->sdata);
	else
		kfree(fc->sdata);
}
EXPORT_SYMBOL(p9_fcall_fini);

static struct kmem_cache *p9_req_cache;

/**
 * p9_tag_alloc - Allocate a new request.
 * @c: Client session.
 * @type: Transaction type.
 * @max_size: Maximum packet size for this request.
 *
 * Context: Process context.
 * Return: Pointer to new request.
 */
static struct p9_req_t *
p9_tag_alloc(struct p9_client *c, int8_t type, unsigned int max_size)
{
	struct p9_req_t *req = kmem_cache_alloc(p9_req_cache, GFP_NOFS);
	int alloc_msize = min(c->msize, max_size);
	int tag;

	if (!req)
		return ERR_PTR(-ENOMEM);

	if (p9_fcall_init(c, &req->tc, alloc_msize))
		goto free_req;
	if (p9_fcall_init(c, &req->rc, alloc_msize))
		goto free;

	p9pdu_reset(&req->tc);
	p9pdu_reset(&req->rc);
	req->t_err = 0;
	req->status = REQ_STATUS_ALLOC;
	init_waitqueue_head(&req->wq);
	INIT_LIST_HEAD(&req->req_list);

	idr_preload(GFP_NOFS);
	spin_lock_irq(&c->lock);
	if (type == P9_TVERSION)
		tag = idr_alloc(&c->reqs, req, P9_NOTAG, P9_NOTAG + 1,
				GFP_NOWAIT);
	else
		tag = idr_alloc(&c->reqs, req, 0, P9_NOTAG, GFP_NOWAIT);
	req->tc.tag = tag;
	spin_unlock_irq(&c->lock);
	idr_preload_end();
	if (tag < 0)
		goto free;

	/* Init ref to two because in the general case there is one ref
	 * that is put asynchronously by a writer thread, one ref
	 * temporarily given by p9_tag_lookup and put by p9_client_cb
	 * in the recv thread, and one ref put by p9_tag_remove in the
	 * main thread. The only exception is virtio that does not use
	 * p9_tag_lookup but does not have a writer thread either
	 * (the write happens synchronously in the request/zc_request
	 * callback), so p9_client_cb eats the second ref there
	 * as the pointer is duplicated directly by virtqueue_add_sgs()
	 */
	refcount_set(&req->refcount.refcount, 2);

	return req;

free:
	p9_fcall_fini(&req->tc);
	p9_fcall_fini(&req->rc);
free_req:
	kmem_cache_free(p9_req_cache, req);
	return ERR_PTR(-ENOMEM);
}

/**
 * p9_tag_lookup - Look up a request by tag.
 * @c: Client session.
 * @tag: Transaction ID.
 *
 * Context: Any context.
 * Return: A request, or %NULL if there is no request with that tag.
 */
struct p9_req_t *p9_tag_lookup(struct p9_client *c, u16 tag)
{
	struct p9_req_t *req;

	rcu_read_lock();
again:
	req = idr_find(&c->reqs, tag);
	if (req) {
		/* We have to be careful with the req found under rcu_read_lock
		 * Thanks to SLAB_TYPESAFE_BY_RCU we can safely try to get the
		 * ref again without corrupting other data, then check again
		 * that the tag matches once we have the ref
		 */
		if (!p9_req_try_get(req))
			goto again;
		if (req->tc.tag != tag) {
			p9_req_put(req);
			goto again;
		}
	}
	rcu_read_unlock();

	return req;
}
EXPORT_SYMBOL(p9_tag_lookup);

/**
 * p9_tag_remove - Remove a tag.
 * @c: Client session.
 * @r: Request of reference.
 *
 * Context: Any context.
 */
static int p9_tag_remove(struct p9_client *c, struct p9_req_t *r)
{
	unsigned long flags;
	u16 tag = r->tc.tag;

	p9_debug(P9_DEBUG_MUX, "clnt %p req %p tag: %d\n", c, r, tag);
	spin_lock_irqsave(&c->lock, flags);
	idr_remove(&c->reqs, tag);
	spin_unlock_irqrestore(&c->lock, flags);
	return p9_req_put(r);
}

static void p9_req_free(struct kref *ref)
{
	struct p9_req_t *r = container_of(ref, struct p9_req_t, refcount);
	p9_fcall_fini(&r->tc);
	p9_fcall_fini(&r->rc);
	kmem_cache_free(p9_req_cache, r);
}

int p9_req_put(struct p9_req_t *r)
{
	return kref_put(&r->refcount, p9_req_free);
}
EXPORT_SYMBOL(p9_req_put);

/**
 * p9_tag_cleanup - cleans up tags structure and reclaims resources
 * @c:  v9fs client struct
 *
 * This frees resources associated with the tags structure
 *
 */
static void p9_tag_cleanup(struct p9_client *c)
{
	struct p9_req_t *req;
	int id;

	rcu_read_lock();
	idr_for_each_entry(&c->reqs, req, id) {
		pr_info("Tag %d still in use\n", id);
		if (p9_tag_remove(c, req) == 0)
			pr_warn("Packet with tag %d has still references",
				req->tc.tag);
	}
	rcu_read_unlock();
}

/**
 * p9_client_cb - call back from transport to client
 * @c: client state
 * @req: request received
 * @status: request status, one of REQ_STATUS_*
 *
 */
void p9_client_cb(struct p9_client *c, struct p9_req_t *req, int status)
{
	p9_debug(P9_DEBUG_MUX, " tag %d\n", req->tc.tag);

	/*
	 * This barrier is needed to make sure any change made to req before
	 * the status change is visible to another thread
	 */
	smp_wmb();
	req->status = status;

	wake_up(&req->wq);
	p9_debug(P9_DEBUG_MUX, "wakeup: %d\n", req->tc.tag);
	p9_req_put(req);
}
EXPORT_SYMBOL(p9_client_cb);

/**
 * p9_parse_header - parse header arguments out of a packet
 * @pdu: packet to parse
 * @size: size of packet
 * @type: type of request
 * @tag: tag of packet
 * @rewind: set if we need to rewind offset afterwards
 */

int
p9_parse_header(struct p9_fcall *pdu, int32_t *size, int8_t *type, int16_t *tag,
								int rewind)
{
	int8_t r_type;
	int16_t r_tag;
	int32_t r_size;
	int offset = pdu->offset;
	int err;

	pdu->offset = 0;

	err = p9pdu_readf(pdu, 0, "dbw", &r_size, &r_type, &r_tag);
	if (err)
		goto rewind_and_exit;

	if (type)
		*type = r_type;
	if (tag)
		*tag = r_tag;
	if (size)
		*size = r_size;

	if (pdu->size != r_size || r_size < 7) {
		err = -EINVAL;
		goto rewind_and_exit;
	}

	pdu->id = r_type;
	pdu->tag = r_tag;

	p9_debug(P9_DEBUG_9P, "<<< size=%d type: %d tag: %d\n",
		 pdu->size, pdu->id, pdu->tag);

rewind_and_exit:
	if (rewind)
		pdu->offset = offset;
	return err;
}
EXPORT_SYMBOL(p9_parse_header);

/**
 * p9_check_errors - check 9p packet for error return and process it
 * @c: current client instance
 * @req: request to parse and check for error conditions
 *
 * returns error code if one is discovered, otherwise returns 0
 *
 * this will have to be more complicated if we have multiple
 * error packet types
 */

static int p9_check_errors(struct p9_client *c, struct p9_req_t *req)
{
	int8_t type;
	int err;
	int ecode;

	err = p9_parse_header(&req->rc, NULL, &type, NULL, 0);
	if (req->rc.size >= c->msize) {
		p9_debug(P9_DEBUG_ERROR,
			 "requested packet size too big: %d\n",
			 req->rc.size);
		return -EIO;
	}
	/*
	 * dump the response from server
	 * This should be after check errors which poplulate pdu_fcall.
	 */
	trace_9p_protocol_dump(c, &req->rc);
	if (err) {
		p9_debug(P9_DEBUG_ERROR, "couldn't parse header %d\n", err);
		return err;
	}
	if (type != P9_RERROR && type != P9_RLERROR)
		return 0;

	if (!p9_is_proto_dotl(c)) {
		char *ename;
		err = p9pdu_readf(&req->rc, c->proto_version, "s?d",
				  &ename, &ecode);
		if (err)
			goto out_err;

		if (p9_is_proto_dotu(c) && ecode < 512)
			err = -ecode;

		if (!err) {
			err = p9_errstr2errno(ename, strlen(ename));

			p9_debug(P9_DEBUG_9P, "<<< RERROR (%d) %s\n",
				 -ecode, ename);
		}
		kfree(ename);
	} else {
		err = p9pdu_readf(&req->rc, c->proto_version, "d", &ecode);
		err = -ecode;

		p9_debug(P9_DEBUG_9P, "<<< RLERROR (%d)\n", -ecode);
	}

	return err;

out_err:
	p9_debug(P9_DEBUG_ERROR, "couldn't parse error%d\n", err);

	return err;
}

/**
 * p9_check_zc_errors - check 9p packet for error return and process it
 * @c: current client instance
 * @req: request to parse and check for error conditions
 * @uidata: external buffer containing error
 * @in_hdrlen: Size of response protocol buffer.
 *
 * returns error code if one is discovered, otherwise returns 0
 *
 * this will have to be more complicated if we have multiple
 * error packet types
 */

static int p9_check_zc_errors(struct p9_client *c, struct p9_req_t *req,
			      struct iov_iter *uidata, int in_hdrlen)
{
	int err;
	int ecode;
	int8_t type;
	char *ename = NULL;

	err = p9_parse_header(&req->rc, NULL, &type, NULL, 0);
	/*
	 * dump the response from server
	 * This should be after parse_header which poplulate pdu_fcall.
	 */
	trace_9p_protocol_dump(c, &req->rc);
	if (err) {
		p9_debug(P9_DEBUG_ERROR, "couldn't parse header %d\n", err);
		return err;
	}

	if (type != P9_RERROR && type != P9_RLERROR)
		return 0;

	if (!p9_is_proto_dotl(c)) {
		/* Error is reported in string format */
		int len;
		/* 7 = header size for RERROR; */
		int inline_len = in_hdrlen - 7;

		len = req->rc.size - req->rc.offset;
		if (len > (P9_ZC_HDR_SZ - 7)) {
			err = -EFAULT;
			goto out_err;
		}

		ename = &req->rc.sdata[req->rc.offset];
		if (len > inline_len) {
			/* We have error in external buffer */
			if (!copy_from_iter_full(ename + inline_len,
					     len - inline_len, uidata)) {
				err = -EFAULT;
				goto out_err;
			}
		}
		ename = NULL;
		err = p9pdu_readf(&req->rc, c->proto_version, "s?d",
				  &ename, &ecode);
		if (err)
			goto out_err;

		if (p9_is_proto_dotu(c) && ecode < 512)
			err = -ecode;

		if (!err) {
			err = p9_errstr2errno(ename, strlen(ename));

			p9_debug(P9_DEBUG_9P, "<<< RERROR (%d) %s\n",
				 -ecode, ename);
		}
		kfree(ename);
	} else {
		err = p9pdu_readf(&req->rc, c->proto_version, "d", &ecode);
		err = -ecode;

		p9_debug(P9_DEBUG_9P, "<<< RLERROR (%d)\n", -ecode);
	}
	return err;

out_err:
	p9_debug(P9_DEBUG_ERROR, "couldn't parse error%d\n", err);
	return err;
}

static struct p9_req_t *
p9_client_rpc(struct p9_client *c, int8_t type, const char *fmt, ...);

/**
 * p9_client_flush - flush (cancel) a request
 * @c: client state
 * @oldreq: request to cancel
 *
 * This sents a flush for a particular request and links
 * the flush request to the original request.  The current
 * code only supports a single flush request although the protocol
 * allows for multiple flush requests to be sent for a single request.
 *
 */

static int p9_client_flush(struct p9_client *c, struct p9_req_t *oldreq)
{
	struct p9_req_t *req;
	int16_t oldtag;
	int err;

	err = p9_parse_header(&oldreq->tc, NULL, NULL, &oldtag, 1);
	if (err)
		return err;

	p9_debug(P9_DEBUG_9P, ">>> TFLUSH tag %d\n", oldtag);

	req = p9_client_rpc(c, P9_TFLUSH, "w", oldtag);
	if (IS_ERR(req))
		return PTR_ERR(req);

	/*
	 * if we haven't received a response for oldreq,
	 * remove it from the list
	 */
	if (oldreq->status == REQ_STATUS_SENT) {
		if (c->trans_mod->cancelled)
			c->trans_mod->cancelled(c, oldreq);
	}

	p9_tag_remove(c, req);
	return 0;
}

static struct p9_req_t *p9_client_prepare_req(struct p9_client *c,
					      int8_t type, int req_size,
					      const char *fmt, va_list ap)
{
	int err;
	struct p9_req_t *req;

	p9_debug(P9_DEBUG_MUX, "client %p op %d\n", c, type);

	/* we allow for any status other than disconnected */
	if (c->status == Disconnected)
		return ERR_PTR(-EIO);

	/* if status is begin_disconnected we allow only clunk request */
	if ((c->status == BeginDisconnect) && (type != P9_TCLUNK))
		return ERR_PTR(-EIO);

	req = p9_tag_alloc(c, type, req_size);
	if (IS_ERR(req))
		return req;

	/* marshall the data */
	p9pdu_prepare(&req->tc, req->tc.tag, type);
	err = p9pdu_vwritef(&req->tc, c->proto_version, fmt, ap);
	if (err)
		goto reterr;
	p9pdu_finalize(c, &req->tc);
	trace_9p_client_req(c, type, req->tc.tag);
	return req;
reterr:
	p9_tag_remove(c, req);
	/* We have to put also the 2nd reference as it won't be used */
	p9_req_put(req);
	return ERR_PTR(err);
}

/**
 * p9_client_rpc - issue a request and wait for a response
 * @c: client session
 * @type: type of request
 * @fmt: protocol format string (see protocol.c)
 *
 * Returns request structure (which client must free using p9_tag_remove)
 */

static struct p9_req_t *
p9_client_rpc(struct p9_client *c, int8_t type, const char *fmt, ...)
{
	va_list ap;
	int sigpending, err;
	unsigned long flags;
	struct p9_req_t *req;

	va_start(ap, fmt);
	req = p9_client_prepare_req(c, type, c->msize, fmt, ap);
	va_end(ap);
	if (IS_ERR(req))
		return req;

	if (signal_pending(current)) {
		sigpending = 1;
		clear_thread_flag(TIF_SIGPENDING);
	} else
		sigpending = 0;

	err = c->trans_mod->request(c, req);
	if (err < 0) {
		/* write won't happen */
		p9_req_put(req);
		if (err != -ERESTARTSYS && err != -EFAULT)
			c->status = Disconnected;
		goto recalc_sigpending;
	}
again:
	/* Wait for the response */
	err = wait_event_killable(req->wq, req->status >= REQ_STATUS_RCVD);

	/*
	 * Make sure our req is coherent with regard to updates in other
	 * threads - echoes to wmb() in the callback
	 */
	smp_rmb();

	if ((err == -ERESTARTSYS) && (c->status == Connected)
				  && (type == P9_TFLUSH)) {
		sigpending = 1;
		clear_thread_flag(TIF_SIGPENDING);
		goto again;
	}

	if (req->status == REQ_STATUS_ERROR) {
		p9_debug(P9_DEBUG_ERROR, "req_status error %d\n", req->t_err);
		err = req->t_err;
	}
	if ((err == -ERESTARTSYS) && (c->status == Connected)) {
		p9_debug(P9_DEBUG_MUX, "flushing\n");
		sigpending = 1;
		clear_thread_flag(TIF_SIGPENDING);

		if (c->trans_mod->cancel(c, req))
			p9_client_flush(c, req);

		/* if we received the response anyway, don't signal error */
		if (req->status == REQ_STATUS_RCVD)
			err = 0;
	}
recalc_sigpending:
	if (sigpending) {
		spin_lock_irqsave(&current->sighand->siglock, flags);
		recalc_sigpending();
		spin_unlock_irqrestore(&current->sighand->siglock, flags);
	}
	if (err < 0)
		goto reterr;

	err = p9_check_errors(c, req);
	trace_9p_client_res(c, type, req->rc.tag, err);
	if (!err)
		return req;
reterr:
	p9_tag_remove(c, req);
	return ERR_PTR(safe_errno(err));
}

/**
 * p9_client_zc_rpc - issue a request and wait for a response
 * @c: client session
 * @type: type of request
 * @uidata: destination for zero copy read
 * @uodata: source for zero copy write
 * @inlen: read buffer size
 * @olen: write buffer size
 * @in_hdrlen: reader header size, This is the size of response protocol data
 * @fmt: protocol format string (see protocol.c)
 *
 * Returns request structure (which client must free using p9_tag_remove)
 */
static struct p9_req_t *p9_client_zc_rpc(struct p9_client *c, int8_t type,
					 struct iov_iter *uidata,
					 struct iov_iter *uodata,
					 int inlen, int olen, int in_hdrlen,
					 const char *fmt, ...)
{
	va_list ap;
	int sigpending, err;
	unsigned long flags;
	struct p9_req_t *req;

	va_start(ap, fmt);
	/*
	 * We allocate a inline protocol data of only 4k bytes.
	 * The actual content is passed in zero-copy fashion.
	 */
	req = p9_client_prepare_req(c, type, P9_ZC_HDR_SZ, fmt, ap);
	va_end(ap);
	if (IS_ERR(req))
		return req;

	if (signal_pending(current)) {
		sigpending = 1;
		clear_thread_flag(TIF_SIGPENDING);
	} else
		sigpending = 0;

	err = c->trans_mod->zc_request(c, req, uidata, uodata,
				       inlen, olen, in_hdrlen);
	if (err < 0) {
		if (err == -EIO)
			c->status = Disconnected;
		if (err != -ERESTARTSYS)
			goto recalc_sigpending;
	}
	if (req->status == REQ_STATUS_ERROR) {
		p9_debug(P9_DEBUG_ERROR, "req_status error %d\n", req->t_err);
		err = req->t_err;
	}
	if ((err == -ERESTARTSYS) && (c->status == Connected)) {
		p9_debug(P9_DEBUG_MUX, "flushing\n");
		sigpending = 1;
		clear_thread_flag(TIF_SIGPENDING);

		if (c->trans_mod->cancel(c, req))
			p9_client_flush(c, req);

		/* if we received the response anyway, don't signal error */
		if (req->status == REQ_STATUS_RCVD)
			err = 0;
	}
recalc_sigpending:
	if (sigpending) {
		spin_lock_irqsave(&current->sighand->siglock, flags);
		recalc_sigpending();
		spin_unlock_irqrestore(&current->sighand->siglock, flags);
	}
	if (err < 0)
		goto reterr;

	err = p9_check_zc_errors(c, req, uidata, in_hdrlen);
	trace_9p_client_res(c, type, req->rc.tag, err);
	if (!err)
		return req;
reterr:
	p9_tag_remove(c, req);
	return ERR_PTR(safe_errno(err));
}

static struct p9_fid *p9_fid_create(struct p9_client *clnt)
{
	int ret;
	struct p9_fid *fid;

	p9_debug(P9_DEBUG_FID, "clnt %p\n", clnt);
	fid = kmalloc(sizeof(struct p9_fid), GFP_KERNEL);
	if (!fid)
		return NULL;

	memset(&fid->qid, 0, sizeof(struct p9_qid));
	fid->mode = -1;
	fid->uid = current_fsuid();
	fid->clnt = clnt;
	fid->rdir = NULL;
	fid->fid = 0;
	refcount_set(&fid->count, 1);

	idr_preload(GFP_KERNEL);
	spin_lock_irq(&clnt->lock);
	ret = idr_alloc_u32(&clnt->fids, fid, &fid->fid, P9_NOFID - 1,
			    GFP_NOWAIT);
	spin_unlock_irq(&clnt->lock);
	idr_preload_end();
	if (!ret)
		return fid;

	kfree(fid);
	return NULL;
}

static void p9_fid_destroy(struct p9_fid *fid)
{
	struct p9_client *clnt;
	unsigned long flags;

	p9_debug(P9_DEBUG_FID, "fid %d\n", fid->fid);
	clnt = fid->clnt;
	spin_lock_irqsave(&clnt->lock, flags);
	idr_remove(&clnt->fids, fid->fid);
	spin_unlock_irqrestore(&clnt->lock, flags);
	kfree(fid->rdir);
	kfree(fid);
}

static int p9_client_version(struct p9_client *c)
{
	int err = 0;
	struct p9_req_t *req;
	char *version = NULL;
	int msize;

	p9_debug(P9_DEBUG_9P, ">>> TVERSION msize %d protocol %d\n",
		 c->msize, c->proto_version);

	switch (c->proto_version) {
	case p9_proto_2000L:
		req = p9_client_rpc(c, P9_TVERSION, "ds",
					c->msize, "9P2000.L");
		break;
	case p9_proto_2000u:
		req = p9_client_rpc(c, P9_TVERSION, "ds",
					c->msize, "9P2000.u");
		break;
	case p9_proto_legacy:
		req = p9_client_rpc(c, P9_TVERSION, "ds",
					c->msize, "9P2000");
		break;
	default:
		return -EINVAL;
	}

	if (IS_ERR(req))
		return PTR_ERR(req);

	err = p9pdu_readf(&req->rc, c->proto_version, "ds", &msize, &version);
	if (err) {
		p9_debug(P9_DEBUG_9P, "version error %d\n", err);
		trace_9p_protocol_dump(c, &req->rc);
		goto error;
	}

	p9_debug(P9_DEBUG_9P, "<<< RVERSION msize %d %s\n", msize, version);
	if (!strncmp(version, "9P2000.L", 8))
		c->proto_version = p9_proto_2000L;
	else if (!strncmp(version, "9P2000.u", 8))
		c->proto_version = p9_proto_2000u;
	else if (!strncmp(version, "9P2000", 6))
		c->proto_version = p9_proto_legacy;
	else {
		p9_debug(P9_DEBUG_ERROR,
			 "server returned an unknown version: %s\n", version);
		err = -EREMOTEIO;
		goto error;
	}

	if (msize < 4096) {
		p9_debug(P9_DEBUG_ERROR,
			 "server returned a msize < 4096: %d\n", msize);
		err = -EREMOTEIO;
		goto error;
	}
	if (msize < c->msize)
		c->msize = msize;

error:
	kfree(version);
	p9_tag_remove(c, req);

	return err;
}

struct p9_client *p9_client_create(const char *dev_name, char *options)
{
	int err;
	struct p9_client *clnt;
	char *client_id;

	err = 0;
	clnt = kmalloc(sizeof(struct p9_client), GFP_KERNEL);
	if (!clnt)
		return ERR_PTR(-ENOMEM);

	clnt->trans_mod = NULL;
	clnt->trans = NULL;
	clnt->fcall_cache = NULL;

	client_id = utsname()->nodename;
	memcpy(clnt->name, client_id, strlen(client_id) + 1);

	spin_lock_init(&clnt->lock);
	idr_init(&clnt->fids);
	idr_init(&clnt->reqs);

	err = parse_opts(options, clnt);
	if (err < 0)
		goto free_client;

	if (!clnt->trans_mod)
		clnt->trans_mod = v9fs_get_default_trans();

	if (clnt->trans_mod == NULL) {
		err = -EPROTONOSUPPORT;
		p9_debug(P9_DEBUG_ERROR,
			 "No transport defined or default transport\n");
		goto free_client;
	}

	p9_debug(P9_DEBUG_MUX, "clnt %p trans %p msize %d protocol %d\n",
		 clnt, clnt->trans_mod, clnt->msize, clnt->proto_version);

	err = clnt->trans_mod->create(clnt, dev_name, options);
	if (err)
		goto put_trans;

	if (clnt->msize > clnt->trans_mod->maxsize)
		clnt->msize = clnt->trans_mod->maxsize;

	if (clnt->msize < 4096) {
		p9_debug(P9_DEBUG_ERROR,
			 "Please specify a msize of at least 4k\n");
		err = -EINVAL;
		goto close_trans;
	}

	err = p9_client_version(clnt);
	if (err)
		goto close_trans;

	/* P9_HDRSZ + 4 is the smallest packet header we can have that is
	 * followed by data accessed from userspace by read
	 */
	clnt->fcall_cache =
		kmem_cache_create_usercopy("9p-fcall-cache", clnt->msize,
					   0, 0, P9_HDRSZ + 4,
					   clnt->msize - (P9_HDRSZ + 4),
					   NULL);

	return clnt;

close_trans:
	clnt->trans_mod->close(clnt);
put_trans:
	v9fs_put_trans(clnt->trans_mod);
free_client:
	kfree(clnt);
	return ERR_PTR(err);
}
EXPORT_SYMBOL(p9_client_create);

void p9_client_destroy(struct p9_client *clnt)
{
	struct p9_fid *fid;
	int id;

	p9_debug(P9_DEBUG_MUX, "clnt %p\n", clnt);

	if (clnt->trans_mod)
		clnt->trans_mod->close(clnt);

	v9fs_put_trans(clnt->trans_mod);

	idr_for_each_entry(&clnt->fids, fid, id) {
		pr_info("Found fid %d not clunked\n", fid->fid);
		p9_fid_destroy(fid);
	}

	p9_tag_cleanup(clnt);

	kmem_cache_destroy(clnt->fcall_cache);
	kfree(clnt);
}
EXPORT_SYMBOL(p9_client_destroy);

void p9_client_disconnect(struct p9_client *clnt)
{
	p9_debug(P9_DEBUG_9P, "clnt %p\n", clnt);
	clnt->status = Disconnected;
}
EXPORT_SYMBOL(p9_client_disconnect);

void p9_client_begin_disconnect(struct p9_client *clnt)
{
	p9_debug(P9_DEBUG_9P, "clnt %p\n", clnt);
	clnt->status = BeginDisconnect;
}
EXPORT_SYMBOL(p9_client_begin_disconnect);

struct p9_fid *p9_client_attach(struct p9_client *clnt, struct p9_fid *afid,
	const char *uname, kuid_t n_uname, const char *aname)
{
	int err = 0;
	struct p9_req_t *req;
	struct p9_fid *fid;
	struct p9_qid qid;


	p9_debug(P9_DEBUG_9P, ">>> TATTACH afid %d uname %s aname %s\n",
		 afid ? afid->fid : -1, uname, aname);
	fid = p9_fid_create(clnt);
	if (!fid) {
		err = -ENOMEM;
		goto error;
	}
	fid->uid = n_uname;

	req = p9_client_rpc(clnt, P9_TATTACH, "ddss?u", fid->fid,
			afid ? afid->fid : P9_NOFID, uname, aname, n_uname);
	if (IS_ERR(req)) {
		err = PTR_ERR(req);
		goto error;
	}

	err = p9pdu_readf(&req->rc, clnt->proto_version, "Q", &qid);
	if (err) {
		trace_9p_protocol_dump(clnt, &req->rc);
		p9_tag_remove(clnt, req);
		goto error;
	}

	p9_debug(P9_DEBUG_9P, "<<< RATTACH qid %x.%llx.%x\n",
		 qid.type, (unsigned long long)qid.path, qid.version);

	memmove(&fid->qid, &qid, sizeof(struct p9_qid));

	p9_tag_remove(clnt, req);
	return fid;

error:
	if (fid)
		p9_fid_destroy(fid);
	return ERR_PTR(err);
}
EXPORT_SYMBOL(p9_client_attach);

struct p9_fid *p9_client_walk(struct p9_fid *oldfid, uint16_t nwname,
		const unsigned char * const *wnames, int clone)
{
	int err;
	struct p9_client *clnt;
	struct p9_fid *fid;
	struct p9_qid *wqids;
	struct p9_req_t *req;
	uint16_t nwqids, count;

	err = 0;
	wqids = NULL;
	clnt = oldfid->clnt;
	if (clone) {
		fid = p9_fid_create(clnt);
		if (!fid) {
			err = -ENOMEM;
			goto error;
		}

		fid->uid = oldfid->uid;
	} else
		fid = oldfid;


	p9_debug(P9_DEBUG_9P, ">>> TWALK fids %d,%d nwname %ud wname[0] %s\n",
		 oldfid->fid, fid->fid, nwname, wnames ? wnames[0] : NULL);
	req = p9_client_rpc(clnt, P9_TWALK, "ddT", oldfid->fid, fid->fid,
								nwname, wnames);
	if (IS_ERR(req)) {
		err = PTR_ERR(req);
		goto error;
	}

	err = p9pdu_readf(&req->rc, clnt->proto_version, "R", &nwqids, &wqids);
	if (err) {
		trace_9p_protocol_dump(clnt, &req->rc);
		p9_tag_remove(clnt, req);
		goto clunk_fid;
	}
	p9_tag_remove(clnt, req);

	p9_debug(P9_DEBUG_9P, "<<< RWALK nwqid %d:\n", nwqids);

	if (nwqids != nwname) {
		err = -ENOENT;
		goto clunk_fid;
	}

	for (count = 0; count < nwqids; count++)
		p9_debug(P9_DEBUG_9P, "<<<     [%d] %x.%llx.%x\n",
			count, wqids[count].type,
			(unsigned long long)wqids[count].path,
			wqids[count].version);

	if (nwname)
		memmove(&fid->qid, &wqids[nwqids - 1], sizeof(struct p9_qid));
	else
		memmove(&fid->qid, &oldfid->qid, sizeof(struct p9_qid));

	kfree(wqids);
	return fid;

clunk_fid:
	kfree(wqids);
	p9_client_clunk(fid);
	fid = NULL;

error:
	if (fid && (fid != oldfid))
		p9_fid_destroy(fid);

	return ERR_PTR(err);
}
EXPORT_SYMBOL(p9_client_walk);

int p9_client_open(struct p9_fid *fid, int mode)
{
	int err;
	struct p9_client *clnt;
	struct p9_req_t *req;
	struct p9_qid qid;
	int iounit;

	clnt = fid->clnt;
	p9_debug(P9_DEBUG_9P, ">>> %s fid %d mode %d\n",
		p9_is_proto_dotl(clnt) ? "TLOPEN" : "TOPEN", fid->fid, mode);
	err = 0;

	if (fid->mode != -1)
		return -EINVAL;

	if (p9_is_proto_dotl(clnt))
		req = p9_client_rpc(clnt, P9_TLOPEN, "dd", fid->fid, mode);
	else
		req = p9_client_rpc(clnt, P9_TOPEN, "db", fid->fid, mode);
	if (IS_ERR(req)) {
		err = PTR_ERR(req);
		goto error;
	}

	err = p9pdu_readf(&req->rc, clnt->proto_version, "Qd", &qid, &iounit);
	if (err) {
		trace_9p_protocol_dump(clnt, &req->rc);
		goto free_and_error;
	}

	p9_debug(P9_DEBUG_9P, "<<< %s qid %x.%llx.%x iounit %x\n",
		p9_is_proto_dotl(clnt) ? "RLOPEN" : "ROPEN",  qid.type,
		(unsigned long long)qid.path, qid.version, iounit);

	memmove(&fid->qid, &qid, sizeof(struct p9_qid));
	fid->mode = mode;
	fid->iounit = iounit;

free_and_error:
	p9_tag_remove(clnt, req);
error:
	return err;
}
EXPORT_SYMBOL(p9_client_open);

int p9_client_create_dotl(struct p9_fid *ofid, const char *name, u32 flags, u32 mode,
		kgid_t gid, struct p9_qid *qid)
{
	int err = 0;
	struct p9_client *clnt;
	struct p9_req_t *req;
	int iounit;

	p9_debug(P9_DEBUG_9P,
			">>> TLCREATE fid %d name %s flags %d mode %d gid %d\n",
			ofid->fid, name, flags, mode,
		 	from_kgid(&init_user_ns, gid));
	clnt = ofid->clnt;

	if (ofid->mode != -1)
		return -EINVAL;

	req = p9_client_rpc(clnt, P9_TLCREATE, "dsddg", ofid->fid, name, flags,
			mode, gid);
	if (IS_ERR(req)) {
		err = PTR_ERR(req);
		goto error;
	}

	err = p9pdu_readf(&req->rc, clnt->proto_version, "Qd", qid, &iounit);
	if (err) {
		trace_9p_protocol_dump(clnt, &req->rc);
		goto free_and_error;
	}

	p9_debug(P9_DEBUG_9P, "<<< RLCREATE qid %x.%llx.%x iounit %x\n",
			qid->type,
			(unsigned long long)qid->path,
			qid->version, iounit);

	memmove(&ofid->qid, qid, sizeof(struct p9_qid));
	ofid->mode = mode;
	ofid->iounit = iounit;

free_and_error:
	p9_tag_remove(clnt, req);
error:
	return err;
}
EXPORT_SYMBOL(p9_client_create_dotl);

int p9_client_fcreate(struct p9_fid *fid, const char *name, u32 perm, int mode,
		     char *extension)
{
	int err;
	struct p9_client *clnt;
	struct p9_req_t *req;
	struct p9_qid qid;
	int iounit;

	p9_debug(P9_DEBUG_9P, ">>> TCREATE fid %d name %s perm %d mode %d\n",
						fid->fid, name, perm, mode);
	err = 0;
	clnt = fid->clnt;

	if (fid->mode != -1)
		return -EINVAL;

	req = p9_client_rpc(clnt, P9_TCREATE, "dsdb?s", fid->fid, name, perm,
				mode, extension);
	if (IS_ERR(req)) {
		err = PTR_ERR(req);
		goto error;
	}

	err = p9pdu_readf(&req->rc, clnt->proto_version, "Qd", &qid, &iounit);
	if (err) {
		trace_9p_protocol_dump(clnt, &req->rc);
		goto free_and_error;
	}

	p9_debug(P9_DEBUG_9P, "<<< RCREATE qid %x.%llx.%x iounit %x\n",
				qid.type,
				(unsigned long long)qid.path,
				qid.version, iounit);

	memmove(&fid->qid, &qid, sizeof(struct p9_qid));
	fid->mode = mode;
	fid->iounit = iounit;

free_and_error:
	p9_tag_remove(clnt, req);
error:
	return err;
}
EXPORT_SYMBOL(p9_client_fcreate);

int p9_client_symlink(struct p9_fid *dfid, const char *name,
		const char *symtgt, kgid_t gid, struct p9_qid *qid)
{
	int err = 0;
	struct p9_client *clnt;
	struct p9_req_t *req;

	p9_debug(P9_DEBUG_9P, ">>> TSYMLINK dfid %d name %s  symtgt %s\n",
			dfid->fid, name, symtgt);
	clnt = dfid->clnt;

	req = p9_client_rpc(clnt, P9_TSYMLINK, "dssg", dfid->fid, name, symtgt,
			gid);
	if (IS_ERR(req)) {
		err = PTR_ERR(req);
		goto error;
	}

	err = p9pdu_readf(&req->rc, clnt->proto_version, "Q", qid);
	if (err) {
		trace_9p_protocol_dump(clnt, &req->rc);
		goto free_and_error;
	}

	p9_debug(P9_DEBUG_9P, "<<< RSYMLINK qid %x.%llx.%x\n",
			qid->type, (unsigned long long)qid->path, qid->version);

free_and_error:
	p9_tag_remove(clnt, req);
error:
	return err;
}
EXPORT_SYMBOL(p9_client_symlink);

int p9_client_link(struct p9_fid *dfid, struct p9_fid *oldfid, const char *newname)
{
	struct p9_client *clnt;
	struct p9_req_t *req;

	p9_debug(P9_DEBUG_9P, ">>> TLINK dfid %d oldfid %d newname %s\n",
			dfid->fid, oldfid->fid, newname);
	clnt = dfid->clnt;
	req = p9_client_rpc(clnt, P9_TLINK, "dds", dfid->fid, oldfid->fid,
			newname);
	if (IS_ERR(req))
		return PTR_ERR(req);

	p9_debug(P9_DEBUG_9P, "<<< RLINK\n");
	p9_tag_remove(clnt, req);
	return 0;
}
EXPORT_SYMBOL(p9_client_link);

int p9_client_fsync(struct p9_fid *fid, int datasync)
{
	int err;
	struct p9_client *clnt;
	struct p9_req_t *req;

	p9_debug(P9_DEBUG_9P, ">>> TFSYNC fid %d datasync:%d\n",
			fid->fid, datasync);
	err = 0;
	clnt = fid->clnt;

	req = p9_client_rpc(clnt, P9_TFSYNC, "dd", fid->fid, datasync);
	if (IS_ERR(req)) {
		err = PTR_ERR(req);
		goto error;
	}

	p9_debug(P9_DEBUG_9P, "<<< RFSYNC fid %d\n", fid->fid);

	p9_tag_remove(clnt, req);

error:
	return err;
}
EXPORT_SYMBOL(p9_client_fsync);

int p9_client_clunk(struct p9_fid *fid)
{
	int err;
	struct p9_client *clnt;
	struct p9_req_t *req;
	int retries = 0;

	if (!fid || IS_ERR(fid)) {
		pr_warn("%s (%d): Trying to clunk with invalid fid\n",
			__func__, task_pid_nr(current));
		dump_stack();
		return 0;
	}
	if (!refcount_dec_and_test(&fid->count))
		return 0;

again:
	p9_debug(P9_DEBUG_9P, ">>> TCLUNK fid %d (try %d)\n", fid->fid,
								retries);
	err = 0;
	clnt = fid->clnt;

	req = p9_client_rpc(clnt, P9_TCLUNK, "d", fid->fid);
	if (IS_ERR(req)) {
		err = PTR_ERR(req);
		goto error;
	}

	p9_debug(P9_DEBUG_9P, "<<< RCLUNK fid %d\n", fid->fid);

	p9_tag_remove(clnt, req);
error:
	/*
	 * Fid is not valid even after a failed clunk
	 * If interrupted, retry once then give up and
	 * leak fid until umount.
	 */
	if (err == -ERESTARTSYS) {
		if (retries++ == 0)
			goto again;
	} else
		p9_fid_destroy(fid);
	return err;
}
EXPORT_SYMBOL(p9_client_clunk);

int p9_client_remove(struct p9_fid *fid)
{
	int err;
	struct p9_client *clnt;
	struct p9_req_t *req;

	p9_debug(P9_DEBUG_9P, ">>> TREMOVE fid %d\n", fid->fid);
	err = 0;
	clnt = fid->clnt;

	req = p9_client_rpc(clnt, P9_TREMOVE, "d", fid->fid);
	if (IS_ERR(req)) {
		err = PTR_ERR(req);
		goto error;
	}

	p9_debug(P9_DEBUG_9P, "<<< RREMOVE fid %d\n", fid->fid);

	p9_tag_remove(clnt, req);
error:
	if (err == -ERESTARTSYS)
		p9_client_clunk(fid);
	else
		p9_fid_destroy(fid);
	return err;
}
EXPORT_SYMBOL(p9_client_remove);

int p9_client_unlinkat(struct p9_fid *dfid, const char *name, int flags)
{
	int err = 0;
	struct p9_req_t *req;
	struct p9_client *clnt;

	p9_debug(P9_DEBUG_9P, ">>> TUNLINKAT fid %d %s %d\n",
		   dfid->fid, name, flags);

	clnt = dfid->clnt;
	req = p9_client_rpc(clnt, P9_TUNLINKAT, "dsd", dfid->fid, name, flags);
	if (IS_ERR(req)) {
		err = PTR_ERR(req);
		goto error;
	}
	p9_debug(P9_DEBUG_9P, "<<< RUNLINKAT fid %d %s\n", dfid->fid, name);

	p9_tag_remove(clnt, req);
error:
	return err;
}
EXPORT_SYMBOL(p9_client_unlinkat);

int
p9_client_read(struct p9_fid *fid, u64 offset, struct iov_iter *to, int *err)
{
	int total = 0;
	*err = 0;

	while (iov_iter_count(to)) {
		int count;

		count = p9_client_read_once(fid, offset, to, err);
		if (!count || *err)
			break;
		offset += count;
		total += count;
	}
	return total;
}
EXPORT_SYMBOL(p9_client_read);

int
p9_client_read_once(struct p9_fid *fid, u64 offset, struct iov_iter *to,
		    int *err)
{
	struct p9_client *clnt = fid->clnt;
	struct p9_req_t *req;
	int count = iov_iter_count(to);
	int rsize, non_zc = 0;
	char *dataptr;

	*err = 0;
	p9_debug(P9_DEBUG_9P, ">>> TREAD fid %d offset %llu %d\n",
		   fid->fid, (unsigned long long) offset, (int)iov_iter_count(to));

	rsize = fid->iounit;
	if (!rsize || rsize > clnt->msize - P9_IOHDRSZ)
		rsize = clnt->msize - P9_IOHDRSZ;

	if (count < rsize)
		rsize = count;

	/* Don't bother zerocopy for small IO (< 1024) */
	if (clnt->trans_mod->zc_request && rsize > 1024) {
		/* response header len is 11
		 * PDU Header(7) + IO Size (4)
		 */
		req = p9_client_zc_rpc(clnt, P9_TREAD, to, NULL, rsize,
				       0, 11, "dqd", fid->fid,
				       offset, rsize);
	} else {
		non_zc = 1;
		req = p9_client_rpc(clnt, P9_TREAD, "dqd", fid->fid, offset,
				    rsize);
	}
	if (IS_ERR(req)) {
		*err = PTR_ERR(req);
		return 0;
	}

	*err = p9pdu_readf(&req->rc, clnt->proto_version,
			   "D", &count, &dataptr);
	if (*err) {
		trace_9p_protocol_dump(clnt, &req->rc);
		p9_tag_remove(clnt, req);
		return 0;
	}
	if (rsize < count) {
		pr_err("bogus RREAD count (%d > %d)\n", count, rsize);
		count = rsize;
	}

	p9_debug(P9_DEBUG_9P, "<<< RREAD count %d\n", count);

	if (non_zc) {
		int n = copy_to_iter(dataptr, count, to);

		if (n != count) {
			*err = -EFAULT;
			p9_tag_remove(clnt, req);
			return n;
		}
	} else {
		iov_iter_advance(to, count);
	}
	p9_tag_remove(clnt, req);
	return count;
}
EXPORT_SYMBOL(p9_client_read_once);

int
p9_client_write(struct p9_fid *fid, u64 offset, struct iov_iter *from, int *err)
{
	struct p9_client *clnt = fid->clnt;
	struct p9_req_t *req;
	int total = 0;
	*err = 0;

	p9_debug(P9_DEBUG_9P, ">>> TWRITE fid %d offset %llu count %zd\n",
				fid->fid, (unsigned long long) offset,
				iov_iter_count(from));

	while (iov_iter_count(from)) {
		int count = iov_iter_count(from);
		int rsize = fid->iounit;
		if (!rsize || rsize > clnt->msize-P9_IOHDRSZ)
			rsize = clnt->msize - P9_IOHDRSZ;

		if (count < rsize)
			rsize = count;

		/* Don't bother zerocopy for small IO (< 1024) */
		if (clnt->trans_mod->zc_request && rsize > 1024) {
			req = p9_client_zc_rpc(clnt, P9_TWRITE, NULL, from, 0,
					       rsize, P9_ZC_HDR_SZ, "dqd",
					       fid->fid, offset, rsize);
		} else {
			req = p9_client_rpc(clnt, P9_TWRITE, "dqV", fid->fid,
						    offset, rsize, from);
		}
		if (IS_ERR(req)) {
			*err = PTR_ERR(req);
			break;
		}

		*err = p9pdu_readf(&req->rc, clnt->proto_version, "d", &count);
		if (*err) {
			trace_9p_protocol_dump(clnt, &req->rc);
			p9_tag_remove(clnt, req);
			break;
		}
		if (rsize < count) {
			pr_err("bogus RWRITE count (%d > %d)\n", count, rsize);
			count = rsize;
		}

		p9_debug(P9_DEBUG_9P, "<<< RWRITE count %d\n", count);

		p9_tag_remove(clnt, req);
		iov_iter_advance(from, count);
		total += count;
		offset += count;
	}
	return total;
}
EXPORT_SYMBOL(p9_client_write);

struct p9_wstat *p9_client_stat(struct p9_fid *fid)
{
	int err;
	struct p9_client *clnt;
	struct p9_wstat *ret = kmalloc(sizeof(struct p9_wstat), GFP_KERNEL);
	struct p9_req_t *req;
	u16 ignored;

	p9_debug(P9_DEBUG_9P, ">>> TSTAT fid %d\n", fid->fid);

	if (!ret)
		return ERR_PTR(-ENOMEM);

	err = 0;
	clnt = fid->clnt;

	req = p9_client_rpc(clnt, P9_TSTAT, "d", fid->fid);
	if (IS_ERR(req)) {
		err = PTR_ERR(req);
		goto error;
	}

	err = p9pdu_readf(&req->rc, clnt->proto_version, "wS", &ignored, ret);
	if (err) {
		trace_9p_protocol_dump(clnt, &req->rc);
		p9_tag_remove(clnt, req);
		goto error;
	}

	p9_debug(P9_DEBUG_9P,
		"<<< RSTAT sz=%x type=%x dev=%x qid=%x.%llx.%x\n"
		"<<<    mode=%8.8x atime=%8.8x mtime=%8.8x length=%llx\n"
		"<<<    name=%s uid=%s gid=%s muid=%s extension=(%s)\n"
		"<<<    uid=%d gid=%d n_muid=%d\n",
		ret->size, ret->type, ret->dev, ret->qid.type,
		(unsigned long long)ret->qid.path, ret->qid.version, ret->mode,
		ret->atime, ret->mtime, (unsigned long long)ret->length,
		ret->name, ret->uid, ret->gid, ret->muid, ret->extension,
		from_kuid(&init_user_ns, ret->n_uid),
		from_kgid(&init_user_ns, ret->n_gid),
		from_kuid(&init_user_ns, ret->n_muid));

	p9_tag_remove(clnt, req);
	return ret;

error:
	kfree(ret);
	return ERR_PTR(err);
}
EXPORT_SYMBOL(p9_client_stat);

struct p9_stat_dotl *p9_client_getattr_dotl(struct p9_fid *fid,
							u64 request_mask)
{
	int err;
	struct p9_client *clnt;
	struct p9_stat_dotl *ret = kmalloc(sizeof(struct p9_stat_dotl),
								GFP_KERNEL);
	struct p9_req_t *req;

	p9_debug(P9_DEBUG_9P, ">>> TGETATTR fid %d, request_mask %lld\n",
							fid->fid, request_mask);

	if (!ret)
		return ERR_PTR(-ENOMEM);

	err = 0;
	clnt = fid->clnt;

	req = p9_client_rpc(clnt, P9_TGETATTR, "dq", fid->fid, request_mask);
	if (IS_ERR(req)) {
		err = PTR_ERR(req);
		goto error;
	}

	err = p9pdu_readf(&req->rc, clnt->proto_version, "A", ret);
	if (err) {
		trace_9p_protocol_dump(clnt, &req->rc);
		p9_tag_remove(clnt, req);
		goto error;
	}

	p9_debug(P9_DEBUG_9P,
		"<<< RGETATTR st_result_mask=%lld\n"
		"<<< qid=%x.%llx.%x\n"
		"<<< st_mode=%8.8x st_nlink=%llu\n"
		"<<< st_uid=%d st_gid=%d\n"
		"<<< st_rdev=%llx st_size=%llx st_blksize=%llu st_blocks=%llu\n"
		"<<< st_atime_sec=%lld st_atime_nsec=%lld\n"
		"<<< st_mtime_sec=%lld st_mtime_nsec=%lld\n"
		"<<< st_ctime_sec=%lld st_ctime_nsec=%lld\n"
		"<<< st_btime_sec=%lld st_btime_nsec=%lld\n"
		"<<< st_gen=%lld st_data_version=%lld\n",
		ret->st_result_mask, ret->qid.type, ret->qid.path,
		ret->qid.version, ret->st_mode, ret->st_nlink,
		from_kuid(&init_user_ns, ret->st_uid),
		from_kgid(&init_user_ns, ret->st_gid),
		ret->st_rdev, ret->st_size, ret->st_blksize,
		ret->st_blocks, ret->st_atime_sec, ret->st_atime_nsec,
		ret->st_mtime_sec, ret->st_mtime_nsec, ret->st_ctime_sec,
		ret->st_ctime_nsec, ret->st_btime_sec, ret->st_btime_nsec,
		ret->st_gen, ret->st_data_version);

	p9_tag_remove(clnt, req);
	return ret;

error:
	kfree(ret);
	return ERR_PTR(err);
}
EXPORT_SYMBOL(p9_client_getattr_dotl);

static int p9_client_statsize(struct p9_wstat *wst, int proto_version)
{
	int ret;

	/* NOTE: size shouldn't include its own length */
	/* size[2] type[2] dev[4] qid[13] */
	/* mode[4] atime[4] mtime[4] length[8]*/
	/* name[s] uid[s] gid[s] muid[s] */
	ret = 2+4+13+4+4+4+8+2+2+2+2;

	if (wst->name)
		ret += strlen(wst->name);
	if (wst->uid)
		ret += strlen(wst->uid);
	if (wst->gid)
		ret += strlen(wst->gid);
	if (wst->muid)
		ret += strlen(wst->muid);

	if ((proto_version == p9_proto_2000u) ||
		(proto_version == p9_proto_2000L)) {
		ret += 2+4+4+4;	/* extension[s] n_uid[4] n_gid[4] n_muid[4] */
		if (wst->extension)
			ret += strlen(wst->extension);
	}

	return ret;
}

int p9_client_wstat(struct p9_fid *fid, struct p9_wstat *wst)
{
	int err;
	struct p9_req_t *req;
	struct p9_client *clnt;

	err = 0;
	clnt = fid->clnt;
	wst->size = p9_client_statsize(wst, clnt->proto_version);
	p9_debug(P9_DEBUG_9P, ">>> TWSTAT fid %d\n", fid->fid);
	p9_debug(P9_DEBUG_9P,
		"     sz=%x type=%x dev=%x qid=%x.%llx.%x\n"
		"     mode=%8.8x atime=%8.8x mtime=%8.8x length=%llx\n"
		"     name=%s uid=%s gid=%s muid=%s extension=(%s)\n"
		"     uid=%d gid=%d n_muid=%d\n",
		wst->size, wst->type, wst->dev, wst->qid.type,
		(unsigned long long)wst->qid.path, wst->qid.version, wst->mode,
		wst->atime, wst->mtime, (unsigned long long)wst->length,
		wst->name, wst->uid, wst->gid, wst->muid, wst->extension,
		from_kuid(&init_user_ns, wst->n_uid),
		from_kgid(&init_user_ns, wst->n_gid),
		from_kuid(&init_user_ns, wst->n_muid));

	req = p9_client_rpc(clnt, P9_TWSTAT, "dwS", fid->fid, wst->size+2, wst);
	if (IS_ERR(req)) {
		err = PTR_ERR(req);
		goto error;
	}

	p9_debug(P9_DEBUG_9P, "<<< RWSTAT fid %d\n", fid->fid);

	p9_tag_remove(clnt, req);
error:
	return err;
}
EXPORT_SYMBOL(p9_client_wstat);

int p9_client_setattr(struct p9_fid *fid, struct p9_iattr_dotl *p9attr)
{
	int err;
	struct p9_req_t *req;
	struct p9_client *clnt;

	err = 0;
	clnt = fid->clnt;
	p9_debug(P9_DEBUG_9P, ">>> TSETATTR fid %d\n", fid->fid);
	p9_debug(P9_DEBUG_9P,
		"    valid=%x mode=%x uid=%d gid=%d size=%lld\n"
		"    atime_sec=%lld atime_nsec=%lld\n"
		"    mtime_sec=%lld mtime_nsec=%lld\n",
		p9attr->valid, p9attr->mode,
		from_kuid(&init_user_ns, p9attr->uid),
		from_kgid(&init_user_ns, p9attr->gid),
		p9attr->size, p9attr->atime_sec, p9attr->atime_nsec,
		p9attr->mtime_sec, p9attr->mtime_nsec);

	req = p9_client_rpc(clnt, P9_TSETATTR, "dI", fid->fid, p9attr);

	if (IS_ERR(req)) {
		err = PTR_ERR(req);
		goto error;
	}
	p9_debug(P9_DEBUG_9P, "<<< RSETATTR fid %d\n", fid->fid);
	p9_tag_remove(clnt, req);
error:
	return err;
}
EXPORT_SYMBOL(p9_client_setattr);

int p9_client_statfs(struct p9_fid *fid, struct p9_rstatfs *sb)
{
	int err;
	struct p9_req_t *req;
	struct p9_client *clnt;

	err = 0;
	clnt = fid->clnt;

	p9_debug(P9_DEBUG_9P, ">>> TSTATFS fid %d\n", fid->fid);

	req = p9_client_rpc(clnt, P9_TSTATFS, "d", fid->fid);
	if (IS_ERR(req)) {
		err = PTR_ERR(req);
		goto error;
	}

	err = p9pdu_readf(&req->rc, clnt->proto_version, "ddqqqqqqd", &sb->type,
			  &sb->bsize, &sb->blocks, &sb->bfree, &sb->bavail,
			  &sb->files, &sb->ffree, &sb->fsid, &sb->namelen);
	if (err) {
		trace_9p_protocol_dump(clnt, &req->rc);
		p9_tag_remove(clnt, req);
		goto error;
	}

	p9_debug(P9_DEBUG_9P, "<<< RSTATFS fid %d type 0x%lx bsize %ld "
		"blocks %llu bfree %llu bavail %llu files %llu ffree %llu "
		"fsid %llu namelen %ld\n",
		fid->fid, (long unsigned int)sb->type, (long int)sb->bsize,
		sb->blocks, sb->bfree, sb->bavail, sb->files,  sb->ffree,
		sb->fsid, (long int)sb->namelen);

	p9_tag_remove(clnt, req);
error:
	return err;
}
EXPORT_SYMBOL(p9_client_statfs);

int p9_client_rename(struct p9_fid *fid,
		     struct p9_fid *newdirfid, const char *name)
{
	int err;
	struct p9_req_t *req;
	struct p9_client *clnt;

	err = 0;
	clnt = fid->clnt;

	p9_debug(P9_DEBUG_9P, ">>> TRENAME fid %d newdirfid %d name %s\n",
			fid->fid, newdirfid->fid, name);

	req = p9_client_rpc(clnt, P9_TRENAME, "dds", fid->fid,
			newdirfid->fid, name);
	if (IS_ERR(req)) {
		err = PTR_ERR(req);
		goto error;
	}

	p9_debug(P9_DEBUG_9P, "<<< RRENAME fid %d\n", fid->fid);

	p9_tag_remove(clnt, req);
error:
	return err;
}
EXPORT_SYMBOL(p9_client_rename);

int p9_client_renameat(struct p9_fid *olddirfid, const char *old_name,
		       struct p9_fid *newdirfid, const char *new_name)
{
	int err;
	struct p9_req_t *req;
	struct p9_client *clnt;

	err = 0;
	clnt = olddirfid->clnt;

	p9_debug(P9_DEBUG_9P, ">>> TRENAMEAT olddirfid %d old name %s"
		   " newdirfid %d new name %s\n", olddirfid->fid, old_name,
		   newdirfid->fid, new_name);

	req = p9_client_rpc(clnt, P9_TRENAMEAT, "dsds", olddirfid->fid,
			    old_name, newdirfid->fid, new_name);
	if (IS_ERR(req)) {
		err = PTR_ERR(req);
		goto error;
	}

	p9_debug(P9_DEBUG_9P, "<<< RRENAMEAT newdirfid %d new name %s\n",
		   newdirfid->fid, new_name);

	p9_tag_remove(clnt, req);
error:
	return err;
}
EXPORT_SYMBOL(p9_client_renameat);

/*
 * An xattrwalk without @attr_name gives the fid for the lisxattr namespace
 */
struct p9_fid *p9_client_xattrwalk(struct p9_fid *file_fid,
				const char *attr_name, u64 *attr_size)
{
	int err;
	struct p9_req_t *req;
	struct p9_client *clnt;
	struct p9_fid *attr_fid;

	err = 0;
	clnt = file_fid->clnt;
	attr_fid = p9_fid_create(clnt);
	if (!attr_fid) {
		err = -ENOMEM;
		goto error;
	}
	p9_debug(P9_DEBUG_9P,
		">>> TXATTRWALK file_fid %d, attr_fid %d name %s\n",
		file_fid->fid, attr_fid->fid, attr_name);

	req = p9_client_rpc(clnt, P9_TXATTRWALK, "dds",
			file_fid->fid, attr_fid->fid, attr_name);
	if (IS_ERR(req)) {
		err = PTR_ERR(req);
		goto error;
	}
	err = p9pdu_readf(&req->rc, clnt->proto_version, "q", attr_size);
	if (err) {
		trace_9p_protocol_dump(clnt, &req->rc);
		p9_tag_remove(clnt, req);
		goto clunk_fid;
	}
	p9_tag_remove(clnt, req);
	p9_debug(P9_DEBUG_9P, "<<<  RXATTRWALK fid %d size %llu\n",
		attr_fid->fid, *attr_size);
	return attr_fid;
clunk_fid:
	p9_client_clunk(attr_fid);
	attr_fid = NULL;
error:
	if (attr_fid && (attr_fid != file_fid))
		p9_fid_destroy(attr_fid);

	return ERR_PTR(err);
}
EXPORT_SYMBOL_GPL(p9_client_xattrwalk);

int p9_client_xattrcreate(struct p9_fid *fid, const char *name,
			u64 attr_size, int flags)
{
	int err;
	struct p9_req_t *req;
	struct p9_client *clnt;

	p9_debug(P9_DEBUG_9P,
		">>> TXATTRCREATE fid %d name  %s size %lld flag %d\n",
		fid->fid, name, (long long)attr_size, flags);
	err = 0;
	clnt = fid->clnt;
	req = p9_client_rpc(clnt, P9_TXATTRCREATE, "dsqd",
			fid->fid, name, attr_size, flags);
	if (IS_ERR(req)) {
		err = PTR_ERR(req);
		goto error;
	}
	p9_debug(P9_DEBUG_9P, "<<< RXATTRCREATE fid %d\n", fid->fid);
	p9_tag_remove(clnt, req);
error:
	return err;
}
EXPORT_SYMBOL_GPL(p9_client_xattrcreate);

int p9_client_readdir(struct p9_fid *fid, char *data, u32 count, u64 offset)
{
	int err, rsize, non_zc = 0;
	struct p9_client *clnt;
	struct p9_req_t *req;
	char *dataptr;
	struct kvec kv = {.iov_base = data, .iov_len = count};
	struct iov_iter to;

	iov_iter_kvec(&to, READ, &kv, 1, count);

	p9_debug(P9_DEBUG_9P, ">>> TREADDIR fid %d offset %llu count %d\n",
				fid->fid, (unsigned long long) offset, count);

	err = 0;
	clnt = fid->clnt;

	rsize = fid->iounit;
	if (!rsize || rsize > clnt->msize-P9_READDIRHDRSZ)
		rsize = clnt->msize - P9_READDIRHDRSZ;

	if (count < rsize)
		rsize = count;

	/* Don't bother zerocopy for small IO (< 1024) */
	if (clnt->trans_mod->zc_request && rsize > 1024) {
		/*
		 * response header len is 11
		 * PDU Header(7) + IO Size (4)
		 */
		req = p9_client_zc_rpc(clnt, P9_TREADDIR, &to, NULL, rsize, 0,
				       11, "dqd", fid->fid, offset, rsize);
	} else {
		non_zc = 1;
		req = p9_client_rpc(clnt, P9_TREADDIR, "dqd", fid->fid,
				    offset, rsize);
	}
	if (IS_ERR(req)) {
		err = PTR_ERR(req);
		goto error;
	}

	err = p9pdu_readf(&req->rc, clnt->proto_version, "D", &count, &dataptr);
	if (err) {
		trace_9p_protocol_dump(clnt, &req->rc);
		goto free_and_error;
	}
	if (rsize < count) {
		pr_err("bogus RREADDIR count (%d > %d)\n", count, rsize);
		count = rsize;
	}

	p9_debug(P9_DEBUG_9P, "<<< RREADDIR count %d\n", count);

	if (non_zc)
		memmove(data, dataptr, count);

	p9_tag_remove(clnt, req);
	return count;

free_and_error:
	p9_tag_remove(clnt, req);
error:
	return err;
}
EXPORT_SYMBOL(p9_client_readdir);

int p9_client_mknod_dotl(struct p9_fid *fid, const char *name, int mode,
			dev_t rdev, kgid_t gid, struct p9_qid *qid)
{
	int err;
	struct p9_client *clnt;
	struct p9_req_t *req;

	err = 0;
	clnt = fid->clnt;
	p9_debug(P9_DEBUG_9P, ">>> TMKNOD fid %d name %s mode %d major %d "
		"minor %d\n", fid->fid, name, mode, MAJOR(rdev), MINOR(rdev));
	req = p9_client_rpc(clnt, P9_TMKNOD, "dsdddg", fid->fid, name, mode,
		MAJOR(rdev), MINOR(rdev), gid);
	if (IS_ERR(req))
		return PTR_ERR(req);

	err = p9pdu_readf(&req->rc, clnt->proto_version, "Q", qid);
	if (err) {
		trace_9p_protocol_dump(clnt, &req->rc);
		goto error;
	}
	p9_debug(P9_DEBUG_9P, "<<< RMKNOD qid %x.%llx.%x\n", qid->type,
				(unsigned long long)qid->path, qid->version);

error:
	p9_tag_remove(clnt, req);
	return err;

}
EXPORT_SYMBOL(p9_client_mknod_dotl);

int p9_client_mkdir_dotl(struct p9_fid *fid, const char *name, int mode,
				kgid_t gid, struct p9_qid *qid)
{
	int err;
	struct p9_client *clnt;
	struct p9_req_t *req;

	err = 0;
	clnt = fid->clnt;
	p9_debug(P9_DEBUG_9P, ">>> TMKDIR fid %d name %s mode %d gid %d\n",
		 fid->fid, name, mode, from_kgid(&init_user_ns, gid));
	req = p9_client_rpc(clnt, P9_TMKDIR, "dsdg", fid->fid, name, mode,
		gid);
	if (IS_ERR(req))
		return PTR_ERR(req);

	err = p9pdu_readf(&req->rc, clnt->proto_version, "Q", qid);
	if (err) {
		trace_9p_protocol_dump(clnt, &req->rc);
		goto error;
	}
	p9_debug(P9_DEBUG_9P, "<<< RMKDIR qid %x.%llx.%x\n", qid->type,
				(unsigned long long)qid->path, qid->version);

error:
	p9_tag_remove(clnt, req);
	return err;

}
EXPORT_SYMBOL(p9_client_mkdir_dotl);

int p9_client_lock_dotl(struct p9_fid *fid, struct p9_flock *flock, u8 *status)
{
	int err;
	struct p9_client *clnt;
	struct p9_req_t *req;

	err = 0;
	clnt = fid->clnt;
	p9_debug(P9_DEBUG_9P, ">>> TLOCK fid %d type %i flags %d "
			"start %lld length %lld proc_id %d client_id %s\n",
			fid->fid, flock->type, flock->flags, flock->start,
			flock->length, flock->proc_id, flock->client_id);

	req = p9_client_rpc(clnt, P9_TLOCK, "dbdqqds", fid->fid, flock->type,
				flock->flags, flock->start, flock->length,
					flock->proc_id, flock->client_id);

	if (IS_ERR(req))
		return PTR_ERR(req);

	err = p9pdu_readf(&req->rc, clnt->proto_version, "b", status);
	if (err) {
		trace_9p_protocol_dump(clnt, &req->rc);
		goto error;
	}
	p9_debug(P9_DEBUG_9P, "<<< RLOCK status %i\n", *status);
error:
	p9_tag_remove(clnt, req);
	return err;

}
EXPORT_SYMBOL(p9_client_lock_dotl);

int p9_client_getlock_dotl(struct p9_fid *fid, struct p9_getlock *glock)
{
	int err;
	struct p9_client *clnt;
	struct p9_req_t *req;

	err = 0;
	clnt = fid->clnt;
	p9_debug(P9_DEBUG_9P, ">>> TGETLOCK fid %d, type %i start %lld "
		"length %lld proc_id %d client_id %s\n", fid->fid, glock->type,
		glock->start, glock->length, glock->proc_id, glock->client_id);

	req = p9_client_rpc(clnt, P9_TGETLOCK, "dbqqds", fid->fid,  glock->type,
		glock->start, glock->length, glock->proc_id, glock->client_id);

	if (IS_ERR(req))
		return PTR_ERR(req);

	err = p9pdu_readf(&req->rc, clnt->proto_version, "bqqds", &glock->type,
			  &glock->start, &glock->length, &glock->proc_id,
			  &glock->client_id);
	if (err) {
		trace_9p_protocol_dump(clnt, &req->rc);
		goto error;
	}
	p9_debug(P9_DEBUG_9P, "<<< RGETLOCK type %i start %lld length %lld "
		"proc_id %d client_id %s\n", glock->type, glock->start,
		glock->length, glock->proc_id, glock->client_id);
error:
	p9_tag_remove(clnt, req);
	return err;
}
EXPORT_SYMBOL(p9_client_getlock_dotl);

int p9_client_readlink(struct p9_fid *fid, char **target)
{
	int err;
	struct p9_client *clnt;
	struct p9_req_t *req;

	err = 0;
	clnt = fid->clnt;
	p9_debug(P9_DEBUG_9P, ">>> TREADLINK fid %d\n", fid->fid);

	req = p9_client_rpc(clnt, P9_TREADLINK, "d", fid->fid);
	if (IS_ERR(req))
		return PTR_ERR(req);

	err = p9pdu_readf(&req->rc, clnt->proto_version, "s", target);
	if (err) {
		trace_9p_protocol_dump(clnt, &req->rc);
		goto error;
	}
	p9_debug(P9_DEBUG_9P, "<<< RREADLINK target %s\n", *target);
error:
	p9_tag_remove(clnt, req);
	return err;
}
EXPORT_SYMBOL(p9_client_readlink);

int __init p9_client_init(void)
{
	p9_req_cache = KMEM_CACHE(p9_req_t, SLAB_TYPESAFE_BY_RCU);
	return p9_req_cache ? 0 : -ENOMEM;
}

void __exit p9_client_exit(void)
{
	kmem_cache_destroy(p9_req_cache);
}
