/*
 * net/tipc/msg.c: TIPC message header routines
 *
 * Copyright (c) 2000-2006, 2014-2015, Ericsson AB
 * Copyright (c) 2005, 2010-2011, Wind River Systems
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the names of the copyright holders nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * Alternatively, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") version 2 as published by the Free
 * Software Foundation.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <net/sock.h>
#include "core.h"
#include "msg.h"
#include "addr.h"
#include "name_table.h"
#include "crypto.h"

#define MAX_FORWARD_SIZE 1024
#ifdef CONFIG_TIPC_CRYPTO
#define BUF_HEADROOM ALIGN(((LL_MAX_HEADER + 48) + EHDR_MAX_SIZE), 16)
#define BUF_TAILROOM (TIPC_AES_GCM_TAG_SIZE)
#else
#define BUF_HEADROOM (LL_MAX_HEADER + 48)
#define BUF_TAILROOM 16
#endif

static unsigned int align(unsigned int i)
{
	return (i + 3) & ~3u;
}

/**
 * tipc_buf_acquire - creates a TIPC message buffer
 * @size: message size (including TIPC header)
 * @gfp: memory allocation flags
 *
 * Return: a new buffer with data pointers set to the specified size.
 *
 * NOTE:
 * Headroom is reserved to allow prepending of a data link header.
 * There may also be unrequested tailroom present at the buffer's end.
 */
struct sk_buff *tipc_buf_acquire(u32 size, gfp_t gfp)
{
	struct sk_buff *skb;
#ifdef CONFIG_TIPC_CRYPTO
	unsigned int buf_size = (BUF_HEADROOM + size + BUF_TAILROOM + 3) & ~3u;
#else
	unsigned int buf_size = (BUF_HEADROOM + size + 3) & ~3u;
#endif

	skb = alloc_skb_fclone(buf_size, gfp);
	if (skb) {
		skb_reserve(skb, BUF_HEADROOM);
		skb_put(skb, size);
		skb->next = NULL;
	}
	return skb;
}

void tipc_msg_init(u32 own_node, struct tipc_msg *m, u32 user, u32 type,
		   u32 hsize, u32 dnode)
{
	memset(m, 0, hsize);
	msg_set_version(m);
	msg_set_user(m, user);
	msg_set_hdr_sz(m, hsize);
	msg_set_size(m, hsize);
	msg_set_prevnode(m, own_node);
	msg_set_type(m, type);
	if (hsize > SHORT_H_SIZE) {
		msg_set_orignode(m, own_node);
		msg_set_destnode(m, dnode);
	}
}

struct sk_buff *tipc_msg_create(uint user, uint type,
				uint hdr_sz, uint data_sz, u32 dnode,
				u32 onode, u32 dport, u32 oport, int errcode)
{
	struct tipc_msg *msg;
	struct sk_buff *buf;

	buf = tipc_buf_acquire(hdr_sz + data_sz, GFP_ATOMIC);
	if (unlikely(!buf))
		return NULL;

	msg = buf_msg(buf);
	tipc_msg_init(onode, msg, user, type, hdr_sz, dnode);
	msg_set_size(msg, hdr_sz + data_sz);
	msg_set_origport(msg, oport);
	msg_set_destport(msg, dport);
	msg_set_errcode(msg, errcode);
	return buf;
}

/* tipc_buf_append(): Append a buffer to the fragment list of another buffer
 * @*headbuf: in:  NULL for first frag, otherwise value returned from prev call
 *            out: set when successful non-complete reassembly, otherwise NULL
 * @*buf:     in:  the buffer to append. Always defined
 *            out: head buf after successful complete reassembly, otherwise NULL
 * Returns 1 when reassembly complete, otherwise 0
 */
int tipc_buf_append(struct sk_buff **headbuf, struct sk_buff **buf)
{
	struct sk_buff *head = *headbuf;
	struct sk_buff *frag = *buf;
	struct sk_buff *tail = NULL;
	struct tipc_msg *msg;
	u32 fragid;
	int delta;
	bool headstolen;

	if (!frag)
		goto err;

	msg = buf_msg(frag);
	fragid = msg_type(msg);
	frag->next = NULL;
	skb_pull(frag, msg_hdr_sz(msg));

	if (fragid == FIRST_FRAGMENT) {
		if (unlikely(head))
			goto err;
		*buf = NULL;
		if (skb_has_frag_list(frag) && __skb_linearize(frag))
			goto err;
		frag = skb_unshare(frag, GFP_ATOMIC);
		if (unlikely(!frag))
			goto err;
		head = *headbuf = frag;
		TIPC_SKB_CB(head)->tail = NULL;
		return 0;
	}

	if (!head)
		goto err;

	if (skb_try_coalesce(head, frag, &headstolen, &delta)) {
		kfree_skb_partial(frag, headstolen);
	} else {
		tail = TIPC_SKB_CB(head)->tail;
		if (!skb_has_frag_list(head))
			skb_shinfo(head)->frag_list = frag;
		else
			tail->next = frag;
		head->truesize += frag->truesize;
		head->data_len += frag->len;
		head->len += frag->len;
		TIPC_SKB_CB(head)->tail = frag;
	}

	if (fragid == LAST_FRAGMENT) {
		TIPC_SKB_CB(head)->validated = 0;
		if (unlikely(!tipc_msg_validate(&head)))
			goto err;
		*buf = head;
		TIPC_SKB_CB(head)->tail = NULL;
		*headbuf = NULL;
		return 1;
	}
	*buf = NULL;
	return 0;
err:
	kfree_skb(*buf);
	kfree_skb(*headbuf);
	*buf = *headbuf = NULL;
	return 0;
}

/**
 * tipc_msg_append(): Append data to tail of an existing buffer queue
 * @_hdr: header to be used
 * @m: the data to be appended
 * @mss: max allowable size of buffer
 * @dlen: size of data to be appended
 * @txq: queue to append to
 *
 * Return: the number of 1k blocks appended or errno value
 */
int tipc_msg_append(struct tipc_msg *_hdr, struct msghdr *m, int dlen,
		    int mss, struct sk_buff_head *txq)
{
	struct sk_buff *skb;
	int accounted, total, curr;
	int mlen, cpy, rem = dlen;
	struct tipc_msg *hdr;

	skb = skb_peek_tail(txq);
	accounted = skb ? msg_blocks(buf_msg(skb)) : 0;
	total = accounted;

	do {
		if (!skb || skb->len >= mss) {
			skb = tipc_buf_acquire(mss, GFP_KERNEL);
			if (unlikely(!skb))
				return -ENOMEM;
			skb_orphan(skb);
			skb_trim(skb, MIN_H_SIZE);
			hdr = buf_msg(skb);
			skb_copy_to_linear_data(skb, _hdr, MIN_H_SIZE);
			msg_set_hdr_sz(hdr, MIN_H_SIZE);
			msg_set_size(hdr, MIN_H_SIZE);
			__skb_queue_tail(txq, skb);
			total += 1;
		}
		hdr = buf_msg(skb);
		curr = msg_blocks(hdr);
		mlen = msg_size(hdr);
		cpy = min_t(size_t, rem, mss - mlen);
		if (cpy != copy_from_iter(skb->data + mlen, cpy, &m->msg_iter))
			return -EFAULT;
		msg_set_size(hdr, mlen + cpy);
		skb_put(skb, cpy);
		rem -= cpy;
		total += msg_blocks(hdr) - curr;
	} while (rem > 0);
	return total - accounted;
}

/* tipc_msg_validate - validate basic format of received message
 *
 * This routine ensures a TIPC message has an acceptable header, and at least
 * as much data as the header indicates it should.  The routine also ensures
 * that the entire message header is stored in the main fragment of the message
 * buffer, to simplify future access to message header fields.
 *
 * Note: Having extra info present in the message header or data areas is OK.
 * TIPC will ignore the excess, under the assumption that it is optional info
 * introduced by a later release of the protocol.
 */
bool tipc_msg_validate(struct sk_buff **_skb)
{
	struct sk_buff *skb = *_skb;
	struct tipc_msg *hdr;
	int msz, hsz;

	/* Ensure that flow control ratio condition is satisfied */
	if (unlikely(skb->truesize / buf_roundup_len(skb) >= 4)) {
		skb = skb_copy_expand(skb, BUF_HEADROOM, 0, GFP_ATOMIC);
		if (!skb)
			return false;
		kfree_skb(*_skb);
		*_skb = skb;
	}

	if (unlikely(TIPC_SKB_CB(skb)->validated))
		return true;

	if (unlikely(!pskb_may_pull(skb, MIN_H_SIZE)))
		return false;

	hsz = msg_hdr_sz(buf_msg(skb));
	if (unlikely(hsz < MIN_H_SIZE) || (hsz > MAX_H_SIZE))
		return false;
	if (unlikely(!pskb_may_pull(skb, hsz)))
		return false;

	hdr = buf_msg(skb);
	if (unlikely(msg_version(hdr) != TIPC_VERSION))
		return false;

	msz = msg_size(hdr);
	if (unlikely(msz < hsz))
		return false;
	if (unlikely((msz - hsz) > TIPC_MAX_USER_MSG_SIZE))
		return false;
	if (unlikely(skb->len < msz))
		return false;

	TIPC_SKB_CB(skb)->validated = 1;
	return true;
}

/**
 * tipc_msg_fragment - build a fragment skb list for TIPC message
 *
 * @skb: TIPC message skb
 * @hdr: internal msg header to be put on the top of the fragments
 * @pktmax: max size of a fragment incl. the header
 * @frags: returned fragment skb list
 *
 * Return: 0 if the fragmentation is successful, otherwise: -EINVAL
 * or -ENOMEM
 */
int tipc_msg_fragment(struct sk_buff *skb, const struct tipc_msg *hdr,
		      int pktmax, struct sk_buff_head *frags)
{
	int pktno, nof_fragms, dsz, dmax, eat;
	struct tipc_msg *_hdr;
	struct sk_buff *_skb;
	u8 *data;

	/* Non-linear buffer? */
	if (skb_linearize(skb))
		return -ENOMEM;

	data = (u8 *)skb->data;
	dsz = msg_size(buf_msg(skb));
	dmax = pktmax - INT_H_SIZE;
	if (dsz <= dmax || !dmax)
		return -EINVAL;

	nof_fragms = dsz / dmax + 1;
	for (pktno = 1; pktno <= nof_fragms; pktno++) {
		if (pktno < nof_fragms)
			eat = dmax;
		else
			eat = dsz % dmax;
		/* Allocate a new fragment */
		_skb = tipc_buf_acquire(INT_H_SIZE + eat, GFP_ATOMIC);
		if (!_skb)
			goto error;
		skb_orphan(_skb);
		__skb_queue_tail(frags, _skb);
		/* Copy header & data to the fragment */
		skb_copy_to_linear_data(_skb, hdr, INT_H_SIZE);
		skb_copy_to_linear_data_offset(_skb, INT_H_SIZE, data, eat);
		data += eat;
		/* Update the fragment's header */
		_hdr = buf_msg(_skb);
		msg_set_fragm_no(_hdr, pktno);
		msg_set_nof_fragms(_hdr, nof_fragms);
		msg_set_size(_hdr, INT_H_SIZE + eat);
	}
	return 0;

error:
	__skb_queue_purge(frags);
	__skb_queue_head_init(frags);
	return -ENOMEM;
}

/**
 * tipc_msg_build - create buffer chain containing specified header and data
 * @mhdr: Message header, to be prepended to data
 * @m: User message
 * @offset: buffer offset for fragmented messages (FIXME)
 * @dsz: Total length of user data
 * @pktmax: Max packet size that can be used
 * @list: Buffer or chain of buffers to be returned to caller
 *
 * Note that the recursive call we are making here is safe, since it can
 * logically go only one further level down.
 *
 * Return: message data size or errno: -ENOMEM, -EFAULT
 */
int tipc_msg_build(struct tipc_msg *mhdr, struct msghdr *m, int offset,
		   int dsz, int pktmax, struct sk_buff_head *list)
{
	int mhsz = msg_hdr_sz(mhdr);
	struct tipc_msg pkthdr;
	int msz = mhsz + dsz;
	int pktrem = pktmax;
	struct sk_buff *skb;
	int drem = dsz;
	int pktno = 1;
	char *pktpos;
	int pktsz;
	int rc;

	msg_set_size(mhdr, msz);

	/* No fragmentation needed? */
	if (likely(msz <= pktmax)) {
		skb = tipc_buf_acquire(msz, GFP_KERNEL);

		/* Fall back to smaller MTU if node local message */
		if (unlikely(!skb)) {
			if (pktmax != MAX_MSG_SIZE)
				return -ENOMEM;
			rc = tipc_msg_build(mhdr, m, offset, dsz, FB_MTU, list);
			if (rc != dsz)
				return rc;
			if (tipc_msg_assemble(list))
				return dsz;
			return -ENOMEM;
		}
		skb_orphan(skb);
		__skb_queue_tail(list, skb);
		skb_copy_to_linear_data(skb, mhdr, mhsz);
		pktpos = skb->data + mhsz;
		if (copy_from_iter_full(pktpos, dsz, &m->msg_iter))
			return dsz;
		rc = -EFAULT;
		goto error;
	}

	/* Prepare reusable fragment header */
	tipc_msg_init(msg_prevnode(mhdr), &pkthdr, MSG_FRAGMENTER,
		      FIRST_FRAGMENT, INT_H_SIZE, msg_destnode(mhdr));
	msg_set_size(&pkthdr, pktmax);
	msg_set_fragm_no(&pkthdr, pktno);
	msg_set_importance(&pkthdr, msg_importance(mhdr));

	/* Prepare first fragment */
	skb = tipc_buf_acquire(pktmax, GFP_KERNEL);
	if (!skb)
		return -ENOMEM;
	skb_orphan(skb);
	__skb_queue_tail(list, skb);
	pktpos = skb->data;
	skb_copy_to_linear_data(skb, &pkthdr, INT_H_SIZE);
	pktpos += INT_H_SIZE;
	pktrem -= INT_H_SIZE;
	skb_copy_to_linear_data_offset(skb, INT_H_SIZE, mhdr, mhsz);
	pktpos += mhsz;
	pktrem -= mhsz;

	do {
		if (drem < pktrem)
			pktrem = drem;

		if (!copy_from_iter_full(pktpos, pktrem, &m->msg_iter)) {
			rc = -EFAULT;
			goto error;
		}
		drem -= pktrem;

		if (!drem)
			break;

		/* Prepare new fragment: */
		if (drem < (pktmax - INT_H_SIZE))
			pktsz = drem + INT_H_SIZE;
		else
			pktsz = pktmax;
		skb = tipc_buf_acquire(pktsz, GFP_KERNEL);
		if (!skb) {
			rc = -ENOMEM;
			goto error;
		}
		skb_orphan(skb);
		__skb_queue_tail(list, skb);
		msg_set_type(&pkthdr, FRAGMENT);
		msg_set_size(&pkthdr, pktsz);
		msg_set_fragm_no(&pkthdr, ++pktno);
		skb_copy_to_linear_data(skb, &pkthdr, INT_H_SIZE);
		pktpos = skb->data + INT_H_SIZE;
		pktrem = pktsz - INT_H_SIZE;

	} while (1);
	msg_set_type(buf_msg(skb), LAST_FRAGMENT);
	return dsz;
error:
	__skb_queue_purge(list);
	__skb_queue_head_init(list);
	return rc;
}

/**
 * tipc_msg_bundle - Append contents of a buffer to tail of an existing one
 * @bskb: the bundle buffer to append to
 * @msg: message to be appended
 * @max: max allowable size for the bundle buffer
 *
 * Return: "true" if bundling has been performed, otherwise "false"
 */
static bool tipc_msg_bundle(struct sk_buff *bskb, struct tipc_msg *msg,
			    u32 max)
{
	struct tipc_msg *bmsg = buf_msg(bskb);
	u32 msz, bsz, offset, pad;

	msz = msg_size(msg);
	bsz = msg_size(bmsg);
	offset = align(bsz);
	pad = offset - bsz;

	if (unlikely(skb_tailroom(bskb) < (pad + msz)))
		return false;
	if (unlikely(max < (offset + msz)))
		return false;

	skb_put(bskb, pad + msz);
	skb_copy_to_linear_data_offset(bskb, offset, msg, msz);
	msg_set_size(bmsg, offset + msz);
	msg_set_msgcnt(bmsg, msg_msgcnt(bmsg) + 1);
	return true;
}

/**
 * tipc_msg_try_bundle - Try to bundle a new message to the last one
 * @tskb: the last/target message to which the new one will be appended
 * @skb: the new message skb pointer
 * @mss: max message size (header inclusive)
 * @dnode: destination node for the message
 * @new_bundle: if this call made a new bundle or not
 *
 * Return: "true" if the new message skb is potential for bundling this time or
 * later, in the case a bundling has been done this time, the skb is consumed
 * (the skb pointer = NULL).
 * Otherwise, "false" if the skb cannot be bundled at all.
 */
bool tipc_msg_try_bundle(struct sk_buff *tskb, struct sk_buff **skb, u32 mss,
			 u32 dnode, bool *new_bundle)
{
	struct tipc_msg *msg, *inner, *outer;
	u32 tsz;

	/* First, check if the new buffer is suitable for bundling */
	msg = buf_msg(*skb);
	if (msg_user(msg) == MSG_FRAGMENTER)
		return false;
	if (msg_user(msg) == TUNNEL_PROTOCOL)
		return false;
	if (msg_user(msg) == BCAST_PROTOCOL)
		return false;
	if (mss <= INT_H_SIZE + msg_size(msg))
		return false;

	/* Ok, but the last/target buffer can be empty? */
	if (unlikely(!tskb))
		return true;

	/* Is it a bundle already? Try to bundle the new message to it */
	if (msg_user(buf_msg(tskb)) == MSG_BUNDLER) {
		*new_bundle = false;
		goto bundle;
	}

	/* Make a new bundle of the two messages if possible */
	tsz = msg_size(buf_msg(tskb));
	if (unlikely(mss < align(INT_H_SIZE + tsz) + msg_size(msg)))
		return true;
	if (unlikely(pskb_expand_head(tskb, INT_H_SIZE, mss - tsz - INT_H_SIZE,
				      GFP_ATOMIC)))
		return true;
	inner = buf_msg(tskb);
	skb_push(tskb, INT_H_SIZE);
	outer = buf_msg(tskb);
	tipc_msg_init(msg_prevnode(inner), outer, MSG_BUNDLER, 0, INT_H_SIZE,
		      dnode);
	msg_set_importance(outer, msg_importance(inner));
	msg_set_size(outer, INT_H_SIZE + tsz);
	msg_set_msgcnt(outer, 1);
	*new_bundle = true;

bundle:
	if (likely(tipc_msg_bundle(tskb, msg, mss))) {
		consume_skb(*skb);
		*skb = NULL;
	}
	return true;
}

/**
 *  tipc_msg_extract(): extract bundled inner packet from buffer
 *  @skb: buffer to be extracted from.
 *  @iskb: extracted inner buffer, to be returned
 *  @pos: position in outer message of msg to be extracted.
 *  Returns position of next msg.
 *  Consumes outer buffer when last packet extracted
 *  Return: true when there is an extracted buffer, otherwise false
 */
bool tipc_msg_extract(struct sk_buff *skb, struct sk_buff **iskb, int *pos)
{
	struct tipc_msg *hdr, *ihdr;
	int imsz;

	*iskb = NULL;
	if (unlikely(skb_linearize(skb)))
		goto none;

	hdr = buf_msg(skb);
	if (unlikely(*pos > (msg_data_sz(hdr) - MIN_H_SIZE)))
		goto none;

	ihdr = (struct tipc_msg *)(msg_data(hdr) + *pos);
	imsz = msg_size(ihdr);

	if ((*pos + imsz) > msg_data_sz(hdr))
		goto none;

	*iskb = tipc_buf_acquire(imsz, GFP_ATOMIC);
	if (!*iskb)
		goto none;

	skb_copy_to_linear_data(*iskb, ihdr, imsz);
	if (unlikely(!tipc_msg_validate(iskb)))
		goto none;

	*pos += align(imsz);
	return true;
none:
	kfree_skb(skb);
	kfree_skb(*iskb);
	*iskb = NULL;
	return false;
}

/**
 * tipc_msg_reverse(): swap source and destination addresses and add error code
 * @own_node: originating node id for reversed message
 * @skb:  buffer containing message to be reversed; will be consumed
 * @err:  error code to be set in message, if any
 * Replaces consumed buffer with new one when successful
 * Return: true if success, otherwise false
 */
bool tipc_msg_reverse(u32 own_node,  struct sk_buff **skb, int err)
{
	struct sk_buff *_skb = *skb;
	struct tipc_msg *_hdr, *hdr;
	int hlen, dlen;

	if (skb_linearize(_skb))
		goto exit;
	_hdr = buf_msg(_skb);
	dlen = min_t(uint, msg_data_sz(_hdr), MAX_FORWARD_SIZE);
	hlen = msg_hdr_sz(_hdr);

	if (msg_dest_droppable(_hdr))
		goto exit;
	if (msg_errcode(_hdr))
		goto exit;

	/* Never return SHORT header */
	if (hlen == SHORT_H_SIZE)
		hlen = BASIC_H_SIZE;

	/* Don't return data along with SYN+, - sender has a clone */
	if (msg_is_syn(_hdr) && err == TIPC_ERR_OVERLOAD)
		dlen = 0;

	/* Allocate new buffer to return */
	*skb = tipc_buf_acquire(hlen + dlen, GFP_ATOMIC);
	if (!*skb)
		goto exit;
	memcpy((*skb)->data, _skb->data, msg_hdr_sz(_hdr));
	memcpy((*skb)->data + hlen, msg_data(_hdr), dlen);

	/* Build reverse header in new buffer */
	hdr = buf_msg(*skb);
	msg_set_hdr_sz(hdr, hlen);
	msg_set_errcode(hdr, err);
	msg_set_non_seq(hdr, 0);
	msg_set_origport(hdr, msg_destport(_hdr));
	msg_set_destport(hdr, msg_origport(_hdr));
	msg_set_destnode(hdr, msg_prevnode(_hdr));
	msg_set_prevnode(hdr, own_node);
	msg_set_orignode(hdr, own_node);
	msg_set_size(hdr, hlen + dlen);
	skb_orphan(_skb);
	kfree_skb(_skb);
	return true;
exit:
	kfree_skb(_skb);
	*skb = NULL;
	return false;
}

bool tipc_msg_skb_clone(struct sk_buff_head *msg, struct sk_buff_head *cpy)
{
	struct sk_buff *skb, *_skb;

	skb_queue_walk(msg, skb) {
		_skb = skb_clone(skb, GFP_ATOMIC);
		if (!_skb) {
			__skb_queue_purge(cpy);
			pr_err_ratelimited("Failed to clone buffer chain\n");
			return false;
		}
		__skb_queue_tail(cpy, _skb);
	}
	return true;
}

/**
 * tipc_msg_lookup_dest(): try to find new destination for named message
 * @net: pointer to associated network namespace
 * @skb: the buffer containing the message.
 * @err: error code to be used by caller if lookup fails
 * Does not consume buffer
 * Return: true if a destination is found, false otherwise
 */
bool tipc_msg_lookup_dest(struct net *net, struct sk_buff *skb, int *err)
{
	struct tipc_msg *msg = buf_msg(skb);
	u32 scope = msg_lookup_scope(msg);
	u32 self = tipc_own_addr(net);
	u32 inst = msg_nameinst(msg);
	struct tipc_socket_addr sk;
	struct tipc_uaddr ua;

	if (!msg_isdata(msg))
		return false;
	if (!msg_named(msg))
		return false;
	if (msg_errcode(msg))
		return false;
	*err = TIPC_ERR_NO_NAME;
	if (skb_linearize(skb))
		return false;
	msg = buf_msg(skb);
	if (msg_reroute_cnt(msg))
		return false;
	tipc_uaddr(&ua, TIPC_SERVICE_RANGE, scope,
		   msg_nametype(msg), inst, inst);
	sk.node = tipc_scope2node(net, scope);
	if (!tipc_nametbl_lookup_anycast(net, &ua, &sk))
		return false;
	msg_incr_reroute_cnt(msg);
	if (sk.node != self)
		msg_set_prevnode(msg, self);
	msg_set_destnode(msg, sk.node);
	msg_set_destport(msg, sk.ref);
	*err = TIPC_OK;

	return true;
}

/* tipc_msg_assemble() - assemble chain of fragments into one message
 */
bool tipc_msg_assemble(struct sk_buff_head *list)
{
	struct sk_buff *skb, *tmp = NULL;

	if (skb_queue_len(list) == 1)
		return true;

	while ((skb = __skb_dequeue(list))) {
		skb->next = NULL;
		if (tipc_buf_append(&tmp, &skb)) {
			__skb_queue_tail(list, skb);
			return true;
		}
		if (!tmp)
			break;
	}
	__skb_queue_purge(list);
	__skb_queue_head_init(list);
	pr_warn("Failed do assemble buffer\n");
	return false;
}

/* tipc_msg_reassemble() - clone a buffer chain of fragments and
 *                         reassemble the clones into one message
 */
bool tipc_msg_reassemble(struct sk_buff_head *list, struct sk_buff_head *rcvq)
{
	struct sk_buff *skb, *_skb;
	struct sk_buff *frag = NULL;
	struct sk_buff *head = NULL;
	int hdr_len;

	/* Copy header if single buffer */
	if (skb_queue_len(list) == 1) {
		skb = skb_peek(list);
		hdr_len = skb_headroom(skb) + msg_hdr_sz(buf_msg(skb));
		_skb = __pskb_copy(skb, hdr_len, GFP_ATOMIC);
		if (!_skb)
			return false;
		__skb_queue_tail(rcvq, _skb);
		return true;
	}

	/* Clone all fragments and reassemble */
	skb_queue_walk(list, skb) {
		frag = skb_clone(skb, GFP_ATOMIC);
		if (!frag)
			goto error;
		frag->next = NULL;
		if (tipc_buf_append(&head, &frag))
			break;
		if (!head)
			goto error;
	}
	__skb_queue_tail(rcvq, frag);
	return true;
error:
	pr_warn("Failed do clone local mcast rcv buffer\n");
	kfree_skb(head);
	return false;
}

bool tipc_msg_pskb_copy(u32 dst, struct sk_buff_head *msg,
			struct sk_buff_head *cpy)
{
	struct sk_buff *skb, *_skb;

	skb_queue_walk(msg, skb) {
		_skb = pskb_copy(skb, GFP_ATOMIC);
		if (!_skb) {
			__skb_queue_purge(cpy);
			return false;
		}
		msg_set_destnode(buf_msg(_skb), dst);
		__skb_queue_tail(cpy, _skb);
	}
	return true;
}

/* tipc_skb_queue_sorted(); sort pkt into list according to sequence number
 * @list: list to be appended to
 * @seqno: sequence number of buffer to add
 * @skb: buffer to add
 */
bool __tipc_skb_queue_sorted(struct sk_buff_head *list, u16 seqno,
			     struct sk_buff *skb)
{
	struct sk_buff *_skb, *tmp;

	if (skb_queue_empty(list) || less(seqno, buf_seqno(skb_peek(list)))) {
		__skb_queue_head(list, skb);
		return true;
	}

	if (more(seqno, buf_seqno(skb_peek_tail(list)))) {
		__skb_queue_tail(list, skb);
		return true;
	}

	skb_queue_walk_safe(list, _skb, tmp) {
		if (more(seqno, buf_seqno(_skb)))
			continue;
		if (seqno == buf_seqno(_skb))
			break;
		__skb_queue_before(list, _skb, skb);
		return true;
	}
	kfree_skb(skb);
	return false;
}

void tipc_skb_reject(struct net *net, int err, struct sk_buff *skb,
		     struct sk_buff_head *xmitq)
{
	if (tipc_msg_reverse(tipc_own_addr(net), &skb, err))
		__skb_queue_tail(xmitq, skb);
}
