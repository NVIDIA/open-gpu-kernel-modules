/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * net/sched/cls_rsvp.h	Template file for RSVPv[46] classifiers.
 *
 * Authors:	Alexey Kuznetsov, <kuznet@ms2.inr.ac.ru>
 */

/*
   Comparing to general packet classification problem,
   RSVP needs only sevaral relatively simple rules:

   * (dst, protocol) are always specified,
     so that we are able to hash them.
   * src may be exact, or may be wildcard, so that
     we can keep a hash table plus one wildcard entry.
   * source port (or flow label) is important only if src is given.

   IMPLEMENTATION.

   We use a two level hash table: The top level is keyed by
   destination address and protocol ID, every bucket contains a list
   of "rsvp sessions", identified by destination address, protocol and
   DPI(="Destination Port ID"): triple (key, mask, offset).

   Every bucket has a smaller hash table keyed by source address
   (cf. RSVP flowspec) and one wildcard entry for wildcard reservations.
   Every bucket is again a list of "RSVP flows", selected by
   source address and SPI(="Source Port ID" here rather than
   "security parameter index"): triple (key, mask, offset).


   NOTE 1. All the packets with IPv6 extension headers (but AH and ESP)
   and all fragmented packets go to the best-effort traffic class.


   NOTE 2. Two "port id"'s seems to be redundant, rfc2207 requires
   only one "Generalized Port Identifier". So that for classic
   ah, esp (and udp,tcp) both *pi should coincide or one of them
   should be wildcard.

   At first sight, this redundancy is just a waste of CPU
   resources. But DPI and SPI add the possibility to assign different
   priorities to GPIs. Look also at note 4 about tunnels below.


   NOTE 3. One complication is the case of tunneled packets.
   We implement it as following: if the first lookup
   matches a special session with "tunnelhdr" value not zero,
   flowid doesn't contain the true flow ID, but the tunnel ID (1...255).
   In this case, we pull tunnelhdr bytes and restart lookup
   with tunnel ID added to the list of keys. Simple and stupid 8)8)
   It's enough for PIMREG and IPIP.


   NOTE 4. Two GPIs make it possible to parse even GRE packets.
   F.e. DPI can select ETH_P_IP (and necessary flags to make
   tunnelhdr correct) in GRE protocol field and SPI matches
   GRE key. Is it not nice? 8)8)


   Well, as result, despite its simplicity, we get a pretty
   powerful classification engine.  */


struct rsvp_head {
	u32			tmap[256/32];
	u32			hgenerator;
	u8			tgenerator;
	struct rsvp_session __rcu *ht[256];
	struct rcu_head		rcu;
};

struct rsvp_session {
	struct rsvp_session __rcu	*next;
	__be32				dst[RSVP_DST_LEN];
	struct tc_rsvp_gpi		dpi;
	u8				protocol;
	u8				tunnelid;
	/* 16 (src,sport) hash slots, and one wildcard source slot */
	struct rsvp_filter __rcu	*ht[16 + 1];
	struct rcu_head			rcu;
};


struct rsvp_filter {
	struct rsvp_filter __rcu	*next;
	__be32				src[RSVP_DST_LEN];
	struct tc_rsvp_gpi		spi;
	u8				tunnelhdr;

	struct tcf_result		res;
	struct tcf_exts			exts;

	u32				handle;
	struct rsvp_session		*sess;
	struct rcu_work			rwork;
};

static inline unsigned int hash_dst(__be32 *dst, u8 protocol, u8 tunnelid)
{
	unsigned int h = (__force __u32)dst[RSVP_DST_LEN - 1];

	h ^= h>>16;
	h ^= h>>8;
	return (h ^ protocol ^ tunnelid) & 0xFF;
}

static inline unsigned int hash_src(__be32 *src)
{
	unsigned int h = (__force __u32)src[RSVP_DST_LEN-1];

	h ^= h>>16;
	h ^= h>>8;
	h ^= h>>4;
	return h & 0xF;
}

#define RSVP_APPLY_RESULT()				\
{							\
	int r = tcf_exts_exec(skb, &f->exts, res);	\
	if (r < 0)					\
		continue;				\
	else if (r > 0)					\
		return r;				\
}

static int rsvp_classify(struct sk_buff *skb, const struct tcf_proto *tp,
			 struct tcf_result *res)
{
	struct rsvp_head *head = rcu_dereference_bh(tp->root);
	struct rsvp_session *s;
	struct rsvp_filter *f;
	unsigned int h1, h2;
	__be32 *dst, *src;
	u8 protocol;
	u8 tunnelid = 0;
	u8 *xprt;
#if RSVP_DST_LEN == 4
	struct ipv6hdr *nhptr;

	if (!pskb_network_may_pull(skb, sizeof(*nhptr)))
		return -1;
	nhptr = ipv6_hdr(skb);
#else
	struct iphdr *nhptr;

	if (!pskb_network_may_pull(skb, sizeof(*nhptr)))
		return -1;
	nhptr = ip_hdr(skb);
#endif
restart:

#if RSVP_DST_LEN == 4
	src = &nhptr->saddr.s6_addr32[0];
	dst = &nhptr->daddr.s6_addr32[0];
	protocol = nhptr->nexthdr;
	xprt = ((u8 *)nhptr) + sizeof(struct ipv6hdr);
#else
	src = &nhptr->saddr;
	dst = &nhptr->daddr;
	protocol = nhptr->protocol;
	xprt = ((u8 *)nhptr) + (nhptr->ihl<<2);
	if (ip_is_fragment(nhptr))
		return -1;
#endif

	h1 = hash_dst(dst, protocol, tunnelid);
	h2 = hash_src(src);

	for (s = rcu_dereference_bh(head->ht[h1]); s;
	     s = rcu_dereference_bh(s->next)) {
		if (dst[RSVP_DST_LEN-1] == s->dst[RSVP_DST_LEN - 1] &&
		    protocol == s->protocol &&
		    !(s->dpi.mask &
		      (*(u32 *)(xprt + s->dpi.offset) ^ s->dpi.key)) &&
#if RSVP_DST_LEN == 4
		    dst[0] == s->dst[0] &&
		    dst[1] == s->dst[1] &&
		    dst[2] == s->dst[2] &&
#endif
		    tunnelid == s->tunnelid) {

			for (f = rcu_dereference_bh(s->ht[h2]); f;
			     f = rcu_dereference_bh(f->next)) {
				if (src[RSVP_DST_LEN-1] == f->src[RSVP_DST_LEN - 1] &&
				    !(f->spi.mask & (*(u32 *)(xprt + f->spi.offset) ^ f->spi.key))
#if RSVP_DST_LEN == 4
				    &&
				    src[0] == f->src[0] &&
				    src[1] == f->src[1] &&
				    src[2] == f->src[2]
#endif
				    ) {
					*res = f->res;
					RSVP_APPLY_RESULT();

matched:
					if (f->tunnelhdr == 0)
						return 0;

					tunnelid = f->res.classid;
					nhptr = (void *)(xprt + f->tunnelhdr - sizeof(*nhptr));
					goto restart;
				}
			}

			/* And wildcard bucket... */
			for (f = rcu_dereference_bh(s->ht[16]); f;
			     f = rcu_dereference_bh(f->next)) {
				*res = f->res;
				RSVP_APPLY_RESULT();
				goto matched;
			}
			return -1;
		}
	}
	return -1;
}

static void rsvp_replace(struct tcf_proto *tp, struct rsvp_filter *n, u32 h)
{
	struct rsvp_head *head = rtnl_dereference(tp->root);
	struct rsvp_session *s;
	struct rsvp_filter __rcu **ins;
	struct rsvp_filter *pins;
	unsigned int h1 = h & 0xFF;
	unsigned int h2 = (h >> 8) & 0xFF;

	for (s = rtnl_dereference(head->ht[h1]); s;
	     s = rtnl_dereference(s->next)) {
		for (ins = &s->ht[h2], pins = rtnl_dereference(*ins); ;
		     ins = &pins->next, pins = rtnl_dereference(*ins)) {
			if (pins->handle == h) {
				RCU_INIT_POINTER(n->next, pins->next);
				rcu_assign_pointer(*ins, n);
				return;
			}
		}
	}

	/* Something went wrong if we are trying to replace a non-existent
	 * node. Mind as well halt instead of silently failing.
	 */
	BUG_ON(1);
}

static void *rsvp_get(struct tcf_proto *tp, u32 handle)
{
	struct rsvp_head *head = rtnl_dereference(tp->root);
	struct rsvp_session *s;
	struct rsvp_filter *f;
	unsigned int h1 = handle & 0xFF;
	unsigned int h2 = (handle >> 8) & 0xFF;

	if (h2 > 16)
		return NULL;

	for (s = rtnl_dereference(head->ht[h1]); s;
	     s = rtnl_dereference(s->next)) {
		for (f = rtnl_dereference(s->ht[h2]); f;
		     f = rtnl_dereference(f->next)) {
			if (f->handle == handle)
				return f;
		}
	}
	return NULL;
}

static int rsvp_init(struct tcf_proto *tp)
{
	struct rsvp_head *data;

	data = kzalloc(sizeof(struct rsvp_head), GFP_KERNEL);
	if (data) {
		rcu_assign_pointer(tp->root, data);
		return 0;
	}
	return -ENOBUFS;
}

static void __rsvp_delete_filter(struct rsvp_filter *f)
{
	tcf_exts_destroy(&f->exts);
	tcf_exts_put_net(&f->exts);
	kfree(f);
}

static void rsvp_delete_filter_work(struct work_struct *work)
{
	struct rsvp_filter *f = container_of(to_rcu_work(work),
					     struct rsvp_filter,
					     rwork);
	rtnl_lock();
	__rsvp_delete_filter(f);
	rtnl_unlock();
}

static void rsvp_delete_filter(struct tcf_proto *tp, struct rsvp_filter *f)
{
	tcf_unbind_filter(tp, &f->res);
	/* all classifiers are required to call tcf_exts_destroy() after rcu
	 * grace period, since converted-to-rcu actions are relying on that
	 * in cleanup() callback
	 */
	if (tcf_exts_get_net(&f->exts))
		tcf_queue_work(&f->rwork, rsvp_delete_filter_work);
	else
		__rsvp_delete_filter(f);
}

static void rsvp_destroy(struct tcf_proto *tp, bool rtnl_held,
			 struct netlink_ext_ack *extack)
{
	struct rsvp_head *data = rtnl_dereference(tp->root);
	int h1, h2;

	if (data == NULL)
		return;

	for (h1 = 0; h1 < 256; h1++) {
		struct rsvp_session *s;

		while ((s = rtnl_dereference(data->ht[h1])) != NULL) {
			RCU_INIT_POINTER(data->ht[h1], s->next);

			for (h2 = 0; h2 <= 16; h2++) {
				struct rsvp_filter *f;

				while ((f = rtnl_dereference(s->ht[h2])) != NULL) {
					rcu_assign_pointer(s->ht[h2], f->next);
					rsvp_delete_filter(tp, f);
				}
			}
			kfree_rcu(s, rcu);
		}
	}
	kfree_rcu(data, rcu);
}

static int rsvp_delete(struct tcf_proto *tp, void *arg, bool *last,
		       bool rtnl_held, struct netlink_ext_ack *extack)
{
	struct rsvp_head *head = rtnl_dereference(tp->root);
	struct rsvp_filter *nfp, *f = arg;
	struct rsvp_filter __rcu **fp;
	unsigned int h = f->handle;
	struct rsvp_session __rcu **sp;
	struct rsvp_session *nsp, *s = f->sess;
	int i, h1;

	fp = &s->ht[(h >> 8) & 0xFF];
	for (nfp = rtnl_dereference(*fp); nfp;
	     fp = &nfp->next, nfp = rtnl_dereference(*fp)) {
		if (nfp == f) {
			RCU_INIT_POINTER(*fp, f->next);
			rsvp_delete_filter(tp, f);

			/* Strip tree */

			for (i = 0; i <= 16; i++)
				if (s->ht[i])
					goto out;

			/* OK, session has no flows */
			sp = &head->ht[h & 0xFF];
			for (nsp = rtnl_dereference(*sp); nsp;
			     sp = &nsp->next, nsp = rtnl_dereference(*sp)) {
				if (nsp == s) {
					RCU_INIT_POINTER(*sp, s->next);
					kfree_rcu(s, rcu);
					goto out;
				}
			}

			break;
		}
	}

out:
	*last = true;
	for (h1 = 0; h1 < 256; h1++) {
		if (rcu_access_pointer(head->ht[h1])) {
			*last = false;
			break;
		}
	}

	return 0;
}

static unsigned int gen_handle(struct tcf_proto *tp, unsigned salt)
{
	struct rsvp_head *data = rtnl_dereference(tp->root);
	int i = 0xFFFF;

	while (i-- > 0) {
		u32 h;

		if ((data->hgenerator += 0x10000) == 0)
			data->hgenerator = 0x10000;
		h = data->hgenerator|salt;
		if (!rsvp_get(tp, h))
			return h;
	}
	return 0;
}

static int tunnel_bts(struct rsvp_head *data)
{
	int n = data->tgenerator >> 5;
	u32 b = 1 << (data->tgenerator & 0x1F);

	if (data->tmap[n] & b)
		return 0;
	data->tmap[n] |= b;
	return 1;
}

static void tunnel_recycle(struct rsvp_head *data)
{
	struct rsvp_session __rcu **sht = data->ht;
	u32 tmap[256/32];
	int h1, h2;

	memset(tmap, 0, sizeof(tmap));

	for (h1 = 0; h1 < 256; h1++) {
		struct rsvp_session *s;
		for (s = rtnl_dereference(sht[h1]); s;
		     s = rtnl_dereference(s->next)) {
			for (h2 = 0; h2 <= 16; h2++) {
				struct rsvp_filter *f;

				for (f = rtnl_dereference(s->ht[h2]); f;
				     f = rtnl_dereference(f->next)) {
					if (f->tunnelhdr == 0)
						continue;
					data->tgenerator = f->res.classid;
					tunnel_bts(data);
				}
			}
		}
	}

	memcpy(data->tmap, tmap, sizeof(tmap));
}

static u32 gen_tunnel(struct rsvp_head *data)
{
	int i, k;

	for (k = 0; k < 2; k++) {
		for (i = 255; i > 0; i--) {
			if (++data->tgenerator == 0)
				data->tgenerator = 1;
			if (tunnel_bts(data))
				return data->tgenerator;
		}
		tunnel_recycle(data);
	}
	return 0;
}

static const struct nla_policy rsvp_policy[TCA_RSVP_MAX + 1] = {
	[TCA_RSVP_CLASSID]	= { .type = NLA_U32 },
	[TCA_RSVP_DST]		= { .len = RSVP_DST_LEN * sizeof(u32) },
	[TCA_RSVP_SRC]		= { .len = RSVP_DST_LEN * sizeof(u32) },
	[TCA_RSVP_PINFO]	= { .len = sizeof(struct tc_rsvp_pinfo) },
};

static int rsvp_change(struct net *net, struct sk_buff *in_skb,
		       struct tcf_proto *tp, unsigned long base,
		       u32 handle,
		       struct nlattr **tca,
		       void **arg, bool ovr, bool rtnl_held,
		       struct netlink_ext_ack *extack)
{
	struct rsvp_head *data = rtnl_dereference(tp->root);
	struct rsvp_filter *f, *nfp;
	struct rsvp_filter __rcu **fp;
	struct rsvp_session *nsp, *s;
	struct rsvp_session __rcu **sp;
	struct tc_rsvp_pinfo *pinfo = NULL;
	struct nlattr *opt = tca[TCA_OPTIONS];
	struct nlattr *tb[TCA_RSVP_MAX + 1];
	struct tcf_exts e;
	unsigned int h1, h2;
	__be32 *dst;
	int err;

	if (opt == NULL)
		return handle ? -EINVAL : 0;

	err = nla_parse_nested_deprecated(tb, TCA_RSVP_MAX, opt, rsvp_policy,
					  NULL);
	if (err < 0)
		return err;

	err = tcf_exts_init(&e, net, TCA_RSVP_ACT, TCA_RSVP_POLICE);
	if (err < 0)
		return err;
	err = tcf_exts_validate(net, tp, tb, tca[TCA_RATE], &e, ovr, true,
				extack);
	if (err < 0)
		goto errout2;

	f = *arg;
	if (f) {
		/* Node exists: adjust only classid */
		struct rsvp_filter *n;

		if (f->handle != handle && handle)
			goto errout2;

		n = kmemdup(f, sizeof(*f), GFP_KERNEL);
		if (!n) {
			err = -ENOMEM;
			goto errout2;
		}

		err = tcf_exts_init(&n->exts, net, TCA_RSVP_ACT,
				    TCA_RSVP_POLICE);
		if (err < 0) {
			kfree(n);
			goto errout2;
		}

		if (tb[TCA_RSVP_CLASSID]) {
			n->res.classid = nla_get_u32(tb[TCA_RSVP_CLASSID]);
			tcf_bind_filter(tp, &n->res, base);
		}

		tcf_exts_change(&n->exts, &e);
		rsvp_replace(tp, n, handle);
		return 0;
	}

	/* Now more serious part... */
	err = -EINVAL;
	if (handle)
		goto errout2;
	if (tb[TCA_RSVP_DST] == NULL)
		goto errout2;

	err = -ENOBUFS;
	f = kzalloc(sizeof(struct rsvp_filter), GFP_KERNEL);
	if (f == NULL)
		goto errout2;

	err = tcf_exts_init(&f->exts, net, TCA_RSVP_ACT, TCA_RSVP_POLICE);
	if (err < 0)
		goto errout;
	h2 = 16;
	if (tb[TCA_RSVP_SRC]) {
		memcpy(f->src, nla_data(tb[TCA_RSVP_SRC]), sizeof(f->src));
		h2 = hash_src(f->src);
	}
	if (tb[TCA_RSVP_PINFO]) {
		pinfo = nla_data(tb[TCA_RSVP_PINFO]);
		f->spi = pinfo->spi;
		f->tunnelhdr = pinfo->tunnelhdr;
	}
	if (tb[TCA_RSVP_CLASSID])
		f->res.classid = nla_get_u32(tb[TCA_RSVP_CLASSID]);

	dst = nla_data(tb[TCA_RSVP_DST]);
	h1 = hash_dst(dst, pinfo ? pinfo->protocol : 0, pinfo ? pinfo->tunnelid : 0);

	err = -ENOMEM;
	if ((f->handle = gen_handle(tp, h1 | (h2<<8))) == 0)
		goto errout;

	if (f->tunnelhdr) {
		err = -EINVAL;
		if (f->res.classid > 255)
			goto errout;

		err = -ENOMEM;
		if (f->res.classid == 0 &&
		    (f->res.classid = gen_tunnel(data)) == 0)
			goto errout;
	}

	for (sp = &data->ht[h1];
	     (s = rtnl_dereference(*sp)) != NULL;
	     sp = &s->next) {
		if (dst[RSVP_DST_LEN-1] == s->dst[RSVP_DST_LEN-1] &&
		    pinfo && pinfo->protocol == s->protocol &&
		    memcmp(&pinfo->dpi, &s->dpi, sizeof(s->dpi)) == 0 &&
#if RSVP_DST_LEN == 4
		    dst[0] == s->dst[0] &&
		    dst[1] == s->dst[1] &&
		    dst[2] == s->dst[2] &&
#endif
		    pinfo->tunnelid == s->tunnelid) {

insert:
			/* OK, we found appropriate session */

			fp = &s->ht[h2];

			f->sess = s;
			if (f->tunnelhdr == 0)
				tcf_bind_filter(tp, &f->res, base);

			tcf_exts_change(&f->exts, &e);

			fp = &s->ht[h2];
			for (nfp = rtnl_dereference(*fp); nfp;
			     fp = &nfp->next, nfp = rtnl_dereference(*fp)) {
				__u32 mask = nfp->spi.mask & f->spi.mask;

				if (mask != f->spi.mask)
					break;
			}
			RCU_INIT_POINTER(f->next, nfp);
			rcu_assign_pointer(*fp, f);

			*arg = f;
			return 0;
		}
	}

	/* No session found. Create new one. */

	err = -ENOBUFS;
	s = kzalloc(sizeof(struct rsvp_session), GFP_KERNEL);
	if (s == NULL)
		goto errout;
	memcpy(s->dst, dst, sizeof(s->dst));

	if (pinfo) {
		s->dpi = pinfo->dpi;
		s->protocol = pinfo->protocol;
		s->tunnelid = pinfo->tunnelid;
	}
	sp = &data->ht[h1];
	for (nsp = rtnl_dereference(*sp); nsp;
	     sp = &nsp->next, nsp = rtnl_dereference(*sp)) {
		if ((nsp->dpi.mask & s->dpi.mask) != s->dpi.mask)
			break;
	}
	RCU_INIT_POINTER(s->next, nsp);
	rcu_assign_pointer(*sp, s);

	goto insert;

errout:
	tcf_exts_destroy(&f->exts);
	kfree(f);
errout2:
	tcf_exts_destroy(&e);
	return err;
}

static void rsvp_walk(struct tcf_proto *tp, struct tcf_walker *arg,
		      bool rtnl_held)
{
	struct rsvp_head *head = rtnl_dereference(tp->root);
	unsigned int h, h1;

	if (arg->stop)
		return;

	for (h = 0; h < 256; h++) {
		struct rsvp_session *s;

		for (s = rtnl_dereference(head->ht[h]); s;
		     s = rtnl_dereference(s->next)) {
			for (h1 = 0; h1 <= 16; h1++) {
				struct rsvp_filter *f;

				for (f = rtnl_dereference(s->ht[h1]); f;
				     f = rtnl_dereference(f->next)) {
					if (arg->count < arg->skip) {
						arg->count++;
						continue;
					}
					if (arg->fn(tp, f, arg) < 0) {
						arg->stop = 1;
						return;
					}
					arg->count++;
				}
			}
		}
	}
}

static int rsvp_dump(struct net *net, struct tcf_proto *tp, void *fh,
		     struct sk_buff *skb, struct tcmsg *t, bool rtnl_held)
{
	struct rsvp_filter *f = fh;
	struct rsvp_session *s;
	struct nlattr *nest;
	struct tc_rsvp_pinfo pinfo;

	if (f == NULL)
		return skb->len;
	s = f->sess;

	t->tcm_handle = f->handle;

	nest = nla_nest_start_noflag(skb, TCA_OPTIONS);
	if (nest == NULL)
		goto nla_put_failure;

	if (nla_put(skb, TCA_RSVP_DST, sizeof(s->dst), &s->dst))
		goto nla_put_failure;
	pinfo.dpi = s->dpi;
	pinfo.spi = f->spi;
	pinfo.protocol = s->protocol;
	pinfo.tunnelid = s->tunnelid;
	pinfo.tunnelhdr = f->tunnelhdr;
	pinfo.pad = 0;
	if (nla_put(skb, TCA_RSVP_PINFO, sizeof(pinfo), &pinfo))
		goto nla_put_failure;
	if (f->res.classid &&
	    nla_put_u32(skb, TCA_RSVP_CLASSID, f->res.classid))
		goto nla_put_failure;
	if (((f->handle >> 8) & 0xFF) != 16 &&
	    nla_put(skb, TCA_RSVP_SRC, sizeof(f->src), f->src))
		goto nla_put_failure;

	if (tcf_exts_dump(skb, &f->exts) < 0)
		goto nla_put_failure;

	nla_nest_end(skb, nest);

	if (tcf_exts_dump_stats(skb, &f->exts) < 0)
		goto nla_put_failure;
	return skb->len;

nla_put_failure:
	nla_nest_cancel(skb, nest);
	return -1;
}

static void rsvp_bind_class(void *fh, u32 classid, unsigned long cl, void *q,
			    unsigned long base)
{
	struct rsvp_filter *f = fh;

	if (f && f->res.classid == classid) {
		if (cl)
			__tcf_bind_filter(q, &f->res, base);
		else
			__tcf_unbind_filter(q, &f->res);
	}
}

static struct tcf_proto_ops RSVP_OPS __read_mostly = {
	.kind		=	RSVP_ID,
	.classify	=	rsvp_classify,
	.init		=	rsvp_init,
	.destroy	=	rsvp_destroy,
	.get		=	rsvp_get,
	.change		=	rsvp_change,
	.delete		=	rsvp_delete,
	.walk		=	rsvp_walk,
	.dump		=	rsvp_dump,
	.bind_class	=	rsvp_bind_class,
	.owner		=	THIS_MODULE,
};

static int __init init_rsvp(void)
{
	return register_tcf_proto_ops(&RSVP_OPS);
}

static void __exit exit_rsvp(void)
{
	unregister_tcf_proto_ops(&RSVP_OPS);
}

module_init(init_rsvp)
module_exit(exit_rsvp)
