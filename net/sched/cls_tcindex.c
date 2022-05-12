// SPDX-License-Identifier: GPL-2.0-only
/*
 * net/sched/cls_tcindex.c	Packet classifier for skb->tc_index
 *
 * Written 1998,1999 by Werner Almesberger, EPFL ICA
 */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/skbuff.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/refcount.h>
#include <net/act_api.h>
#include <net/netlink.h>
#include <net/pkt_cls.h>
#include <net/sch_generic.h>

/*
 * Passing parameters to the root seems to be done more awkwardly than really
 * necessary. At least, u32 doesn't seem to use such dirty hacks. To be
 * verified. FIXME.
 */

#define PERFECT_HASH_THRESHOLD	64	/* use perfect hash if not bigger */
#define DEFAULT_HASH_SIZE	64	/* optimized for diffserv */


struct tcindex_data;

struct tcindex_filter_result {
	struct tcf_exts		exts;
	struct tcf_result	res;
	struct tcindex_data	*p;
	struct rcu_work		rwork;
};

struct tcindex_filter {
	u16 key;
	struct tcindex_filter_result result;
	struct tcindex_filter __rcu *next;
	struct rcu_work rwork;
};


struct tcindex_data {
	struct tcindex_filter_result *perfect; /* perfect hash; NULL if none */
	struct tcindex_filter __rcu **h; /* imperfect hash; */
	struct tcf_proto *tp;
	u16 mask;		/* AND key with mask */
	u32 shift;		/* shift ANDed key to the right */
	u32 hash;		/* hash table size; 0 if undefined */
	u32 alloc_hash;		/* allocated size */
	u32 fall_through;	/* 0: only classify if explicit match */
	refcount_t refcnt;	/* a temporary refcnt for perfect hash */
	struct rcu_work rwork;
};

static inline int tcindex_filter_is_set(struct tcindex_filter_result *r)
{
	return tcf_exts_has_actions(&r->exts) || r->res.classid;
}

static void tcindex_data_get(struct tcindex_data *p)
{
	refcount_inc(&p->refcnt);
}

static void tcindex_data_put(struct tcindex_data *p)
{
	if (refcount_dec_and_test(&p->refcnt)) {
		kfree(p->perfect);
		kfree(p->h);
		kfree(p);
	}
}

static struct tcindex_filter_result *tcindex_lookup(struct tcindex_data *p,
						    u16 key)
{
	if (p->perfect) {
		struct tcindex_filter_result *f = p->perfect + key;

		return tcindex_filter_is_set(f) ? f : NULL;
	} else if (p->h) {
		struct tcindex_filter __rcu **fp;
		struct tcindex_filter *f;

		fp = &p->h[key % p->hash];
		for (f = rcu_dereference_bh_rtnl(*fp);
		     f;
		     fp = &f->next, f = rcu_dereference_bh_rtnl(*fp))
			if (f->key == key)
				return &f->result;
	}

	return NULL;
}


static int tcindex_classify(struct sk_buff *skb, const struct tcf_proto *tp,
			    struct tcf_result *res)
{
	struct tcindex_data *p = rcu_dereference_bh(tp->root);
	struct tcindex_filter_result *f;
	int key = (skb->tc_index & p->mask) >> p->shift;

	pr_debug("tcindex_classify(skb %p,tp %p,res %p),p %p\n",
		 skb, tp, res, p);

	f = tcindex_lookup(p, key);
	if (!f) {
		struct Qdisc *q = tcf_block_q(tp->chain->block);

		if (!p->fall_through)
			return -1;
		res->classid = TC_H_MAKE(TC_H_MAJ(q->handle), key);
		res->class = 0;
		pr_debug("alg 0x%x\n", res->classid);
		return 0;
	}
	*res = f->res;
	pr_debug("map 0x%x\n", res->classid);

	return tcf_exts_exec(skb, &f->exts, res);
}


static void *tcindex_get(struct tcf_proto *tp, u32 handle)
{
	struct tcindex_data *p = rtnl_dereference(tp->root);
	struct tcindex_filter_result *r;

	pr_debug("tcindex_get(tp %p,handle 0x%08x)\n", tp, handle);
	if (p->perfect && handle >= p->alloc_hash)
		return NULL;
	r = tcindex_lookup(p, handle);
	return r && tcindex_filter_is_set(r) ? r : NULL;
}

static int tcindex_init(struct tcf_proto *tp)
{
	struct tcindex_data *p;

	pr_debug("tcindex_init(tp %p)\n", tp);
	p = kzalloc(sizeof(struct tcindex_data), GFP_KERNEL);
	if (!p)
		return -ENOMEM;

	p->mask = 0xffff;
	p->hash = DEFAULT_HASH_SIZE;
	p->fall_through = 1;
	refcount_set(&p->refcnt, 1); /* Paired with tcindex_destroy_work() */

	rcu_assign_pointer(tp->root, p);
	return 0;
}

static void __tcindex_destroy_rexts(struct tcindex_filter_result *r)
{
	tcf_exts_destroy(&r->exts);
	tcf_exts_put_net(&r->exts);
	tcindex_data_put(r->p);
}

static void tcindex_destroy_rexts_work(struct work_struct *work)
{
	struct tcindex_filter_result *r;

	r = container_of(to_rcu_work(work),
			 struct tcindex_filter_result,
			 rwork);
	rtnl_lock();
	__tcindex_destroy_rexts(r);
	rtnl_unlock();
}

static void __tcindex_destroy_fexts(struct tcindex_filter *f)
{
	tcf_exts_destroy(&f->result.exts);
	tcf_exts_put_net(&f->result.exts);
	kfree(f);
}

static void tcindex_destroy_fexts_work(struct work_struct *work)
{
	struct tcindex_filter *f = container_of(to_rcu_work(work),
						struct tcindex_filter,
						rwork);

	rtnl_lock();
	__tcindex_destroy_fexts(f);
	rtnl_unlock();
}

static int tcindex_delete(struct tcf_proto *tp, void *arg, bool *last,
			  bool rtnl_held, struct netlink_ext_ack *extack)
{
	struct tcindex_data *p = rtnl_dereference(tp->root);
	struct tcindex_filter_result *r = arg;
	struct tcindex_filter __rcu **walk;
	struct tcindex_filter *f = NULL;

	pr_debug("tcindex_delete(tp %p,arg %p),p %p\n", tp, arg, p);
	if (p->perfect) {
		if (!r->res.class)
			return -ENOENT;
	} else {
		int i;

		for (i = 0; i < p->hash; i++) {
			walk = p->h + i;
			for (f = rtnl_dereference(*walk); f;
			     walk = &f->next, f = rtnl_dereference(*walk)) {
				if (&f->result == r)
					goto found;
			}
		}
		return -ENOENT;

found:
		rcu_assign_pointer(*walk, rtnl_dereference(f->next));
	}
	tcf_unbind_filter(tp, &r->res);
	/* all classifiers are required to call tcf_exts_destroy() after rcu
	 * grace period, since converted-to-rcu actions are relying on that
	 * in cleanup() callback
	 */
	if (f) {
		if (tcf_exts_get_net(&f->result.exts))
			tcf_queue_work(&f->rwork, tcindex_destroy_fexts_work);
		else
			__tcindex_destroy_fexts(f);
	} else {
		tcindex_data_get(p);

		if (tcf_exts_get_net(&r->exts))
			tcf_queue_work(&r->rwork, tcindex_destroy_rexts_work);
		else
			__tcindex_destroy_rexts(r);
	}

	*last = false;
	return 0;
}

static void tcindex_destroy_work(struct work_struct *work)
{
	struct tcindex_data *p = container_of(to_rcu_work(work),
					      struct tcindex_data,
					      rwork);

	tcindex_data_put(p);
}

static inline int
valid_perfect_hash(struct tcindex_data *p)
{
	return  p->hash > (p->mask >> p->shift);
}

static const struct nla_policy tcindex_policy[TCA_TCINDEX_MAX + 1] = {
	[TCA_TCINDEX_HASH]		= { .type = NLA_U32 },
	[TCA_TCINDEX_MASK]		= { .type = NLA_U16 },
	[TCA_TCINDEX_SHIFT]		= { .type = NLA_U32 },
	[TCA_TCINDEX_FALL_THROUGH]	= { .type = NLA_U32 },
	[TCA_TCINDEX_CLASSID]		= { .type = NLA_U32 },
};

static int tcindex_filter_result_init(struct tcindex_filter_result *r,
				      struct tcindex_data *p,
				      struct net *net)
{
	memset(r, 0, sizeof(*r));
	r->p = p;
	return tcf_exts_init(&r->exts, net, TCA_TCINDEX_ACT,
			     TCA_TCINDEX_POLICE);
}

static void tcindex_partial_destroy_work(struct work_struct *work)
{
	struct tcindex_data *p = container_of(to_rcu_work(work),
					      struct tcindex_data,
					      rwork);

	rtnl_lock();
	kfree(p->perfect);
	kfree(p);
	rtnl_unlock();
}

static void tcindex_free_perfect_hash(struct tcindex_data *cp)
{
	int i;

	for (i = 0; i < cp->hash; i++)
		tcf_exts_destroy(&cp->perfect[i].exts);
	kfree(cp->perfect);
}

static int tcindex_alloc_perfect_hash(struct net *net, struct tcindex_data *cp)
{
	int i, err = 0;

	cp->perfect = kcalloc(cp->hash, sizeof(struct tcindex_filter_result),
			      GFP_KERNEL);
	if (!cp->perfect)
		return -ENOMEM;

	for (i = 0; i < cp->hash; i++) {
		err = tcf_exts_init(&cp->perfect[i].exts, net,
				    TCA_TCINDEX_ACT, TCA_TCINDEX_POLICE);
		if (err < 0)
			goto errout;
		cp->perfect[i].p = cp;
	}

	return 0;

errout:
	tcindex_free_perfect_hash(cp);
	return err;
}

static int
tcindex_set_parms(struct net *net, struct tcf_proto *tp, unsigned long base,
		  u32 handle, struct tcindex_data *p,
		  struct tcindex_filter_result *r, struct nlattr **tb,
		  struct nlattr *est, bool ovr, struct netlink_ext_ack *extack)
{
	struct tcindex_filter_result new_filter_result, *old_r = r;
	struct tcindex_data *cp = NULL, *oldp;
	struct tcindex_filter *f = NULL; /* make gcc behave */
	struct tcf_result cr = {};
	int err, balloc = 0;
	struct tcf_exts e;

	err = tcf_exts_init(&e, net, TCA_TCINDEX_ACT, TCA_TCINDEX_POLICE);
	if (err < 0)
		return err;
	err = tcf_exts_validate(net, tp, tb, est, &e, ovr, true, extack);
	if (err < 0)
		goto errout;

	err = -ENOMEM;
	/* tcindex_data attributes must look atomic to classifier/lookup so
	 * allocate new tcindex data and RCU assign it onto root. Keeping
	 * perfect hash and hash pointers from old data.
	 */
	cp = kzalloc(sizeof(*cp), GFP_KERNEL);
	if (!cp)
		goto errout;

	cp->mask = p->mask;
	cp->shift = p->shift;
	cp->hash = p->hash;
	cp->alloc_hash = p->alloc_hash;
	cp->fall_through = p->fall_through;
	cp->tp = tp;
	refcount_set(&cp->refcnt, 1); /* Paired with tcindex_destroy_work() */

	if (tb[TCA_TCINDEX_HASH])
		cp->hash = nla_get_u32(tb[TCA_TCINDEX_HASH]);

	if (tb[TCA_TCINDEX_MASK])
		cp->mask = nla_get_u16(tb[TCA_TCINDEX_MASK]);

	if (tb[TCA_TCINDEX_SHIFT]) {
		cp->shift = nla_get_u32(tb[TCA_TCINDEX_SHIFT]);
		if (cp->shift > 16) {
			err = -EINVAL;
			goto errout;
		}
	}
	if (!cp->hash) {
		/* Hash not specified, use perfect hash if the upper limit
		 * of the hashing index is below the threshold.
		 */
		if ((cp->mask >> cp->shift) < PERFECT_HASH_THRESHOLD)
			cp->hash = (cp->mask >> cp->shift) + 1;
		else
			cp->hash = DEFAULT_HASH_SIZE;
	}

	if (p->perfect) {
		int i;

		if (tcindex_alloc_perfect_hash(net, cp) < 0)
			goto errout;
		cp->alloc_hash = cp->hash;
		for (i = 0; i < min(cp->hash, p->hash); i++)
			cp->perfect[i].res = p->perfect[i].res;
		balloc = 1;
	}
	cp->h = p->h;

	err = tcindex_filter_result_init(&new_filter_result, cp, net);
	if (err < 0)
		goto errout_alloc;
	if (old_r)
		cr = r->res;

	err = -EBUSY;

	/* Hash already allocated, make sure that we still meet the
	 * requirements for the allocated hash.
	 */
	if (cp->perfect) {
		if (!valid_perfect_hash(cp) ||
		    cp->hash > cp->alloc_hash)
			goto errout_alloc;
	} else if (cp->h && cp->hash != cp->alloc_hash) {
		goto errout_alloc;
	}

	err = -EINVAL;
	if (tb[TCA_TCINDEX_FALL_THROUGH])
		cp->fall_through = nla_get_u32(tb[TCA_TCINDEX_FALL_THROUGH]);

	if (!cp->perfect && !cp->h)
		cp->alloc_hash = cp->hash;

	/* Note: this could be as restrictive as if (handle & ~(mask >> shift))
	 * but then, we'd fail handles that may become valid after some future
	 * mask change. While this is extremely unlikely to ever matter,
	 * the check below is safer (and also more backwards-compatible).
	 */
	if (cp->perfect || valid_perfect_hash(cp))
		if (handle >= cp->alloc_hash)
			goto errout_alloc;


	err = -ENOMEM;
	if (!cp->perfect && !cp->h) {
		if (valid_perfect_hash(cp)) {
			if (tcindex_alloc_perfect_hash(net, cp) < 0)
				goto errout_alloc;
			balloc = 1;
		} else {
			struct tcindex_filter __rcu **hash;

			hash = kcalloc(cp->hash,
				       sizeof(struct tcindex_filter *),
				       GFP_KERNEL);

			if (!hash)
				goto errout_alloc;

			cp->h = hash;
			balloc = 2;
		}
	}

	if (cp->perfect)
		r = cp->perfect + handle;
	else
		r = tcindex_lookup(cp, handle) ? : &new_filter_result;

	if (r == &new_filter_result) {
		f = kzalloc(sizeof(*f), GFP_KERNEL);
		if (!f)
			goto errout_alloc;
		f->key = handle;
		f->next = NULL;
		err = tcindex_filter_result_init(&f->result, cp, net);
		if (err < 0) {
			kfree(f);
			goto errout_alloc;
		}
	}

	if (tb[TCA_TCINDEX_CLASSID]) {
		cr.classid = nla_get_u32(tb[TCA_TCINDEX_CLASSID]);
		tcf_bind_filter(tp, &cr, base);
	}

	if (old_r && old_r != r) {
		err = tcindex_filter_result_init(old_r, cp, net);
		if (err < 0) {
			kfree(f);
			goto errout_alloc;
		}
	}

	oldp = p;
	r->res = cr;
	tcf_exts_change(&r->exts, &e);

	rcu_assign_pointer(tp->root, cp);

	if (r == &new_filter_result) {
		struct tcindex_filter *nfp;
		struct tcindex_filter __rcu **fp;

		f->result.res = r->res;
		tcf_exts_change(&f->result.exts, &r->exts);

		fp = cp->h + (handle % cp->hash);
		for (nfp = rtnl_dereference(*fp);
		     nfp;
		     fp = &nfp->next, nfp = rtnl_dereference(*fp))
				; /* nothing */

		rcu_assign_pointer(*fp, f);
	} else {
		tcf_exts_destroy(&new_filter_result.exts);
	}

	if (oldp)
		tcf_queue_work(&oldp->rwork, tcindex_partial_destroy_work);
	return 0;

errout_alloc:
	if (balloc == 1)
		tcindex_free_perfect_hash(cp);
	else if (balloc == 2)
		kfree(cp->h);
	tcf_exts_destroy(&new_filter_result.exts);
errout:
	kfree(cp);
	tcf_exts_destroy(&e);
	return err;
}

static int
tcindex_change(struct net *net, struct sk_buff *in_skb,
	       struct tcf_proto *tp, unsigned long base, u32 handle,
	       struct nlattr **tca, void **arg, bool ovr,
	       bool rtnl_held, struct netlink_ext_ack *extack)
{
	struct nlattr *opt = tca[TCA_OPTIONS];
	struct nlattr *tb[TCA_TCINDEX_MAX + 1];
	struct tcindex_data *p = rtnl_dereference(tp->root);
	struct tcindex_filter_result *r = *arg;
	int err;

	pr_debug("tcindex_change(tp %p,handle 0x%08x,tca %p,arg %p),opt %p,"
	    "p %p,r %p,*arg %p\n",
	    tp, handle, tca, arg, opt, p, r, *arg);

	if (!opt)
		return 0;

	err = nla_parse_nested_deprecated(tb, TCA_TCINDEX_MAX, opt,
					  tcindex_policy, NULL);
	if (err < 0)
		return err;

	return tcindex_set_parms(net, tp, base, handle, p, r, tb,
				 tca[TCA_RATE], ovr, extack);
}

static void tcindex_walk(struct tcf_proto *tp, struct tcf_walker *walker,
			 bool rtnl_held)
{
	struct tcindex_data *p = rtnl_dereference(tp->root);
	struct tcindex_filter *f, *next;
	int i;

	pr_debug("tcindex_walk(tp %p,walker %p),p %p\n", tp, walker, p);
	if (p->perfect) {
		for (i = 0; i < p->hash; i++) {
			if (!p->perfect[i].res.class)
				continue;
			if (walker->count >= walker->skip) {
				if (walker->fn(tp, p->perfect + i, walker) < 0) {
					walker->stop = 1;
					return;
				}
			}
			walker->count++;
		}
	}
	if (!p->h)
		return;
	for (i = 0; i < p->hash; i++) {
		for (f = rtnl_dereference(p->h[i]); f; f = next) {
			next = rtnl_dereference(f->next);
			if (walker->count >= walker->skip) {
				if (walker->fn(tp, &f->result, walker) < 0) {
					walker->stop = 1;
					return;
				}
			}
			walker->count++;
		}
	}
}

static void tcindex_destroy(struct tcf_proto *tp, bool rtnl_held,
			    struct netlink_ext_ack *extack)
{
	struct tcindex_data *p = rtnl_dereference(tp->root);
	int i;

	pr_debug("tcindex_destroy(tp %p),p %p\n", tp, p);

	if (p->perfect) {
		for (i = 0; i < p->hash; i++) {
			struct tcindex_filter_result *r = p->perfect + i;

			/* tcf_queue_work() does not guarantee the ordering we
			 * want, so we have to take this refcnt temporarily to
			 * ensure 'p' is freed after all tcindex_filter_result
			 * here. Imperfect hash does not need this, because it
			 * uses linked lists rather than an array.
			 */
			tcindex_data_get(p);

			tcf_unbind_filter(tp, &r->res);
			if (tcf_exts_get_net(&r->exts))
				tcf_queue_work(&r->rwork,
					       tcindex_destroy_rexts_work);
			else
				__tcindex_destroy_rexts(r);
		}
	}

	for (i = 0; p->h && i < p->hash; i++) {
		struct tcindex_filter *f, *next;
		bool last;

		for (f = rtnl_dereference(p->h[i]); f; f = next) {
			next = rtnl_dereference(f->next);
			tcindex_delete(tp, &f->result, &last, rtnl_held, NULL);
		}
	}

	tcf_queue_work(&p->rwork, tcindex_destroy_work);
}


static int tcindex_dump(struct net *net, struct tcf_proto *tp, void *fh,
			struct sk_buff *skb, struct tcmsg *t, bool rtnl_held)
{
	struct tcindex_data *p = rtnl_dereference(tp->root);
	struct tcindex_filter_result *r = fh;
	struct nlattr *nest;

	pr_debug("tcindex_dump(tp %p,fh %p,skb %p,t %p),p %p,r %p\n",
		 tp, fh, skb, t, p, r);
	pr_debug("p->perfect %p p->h %p\n", p->perfect, p->h);

	nest = nla_nest_start_noflag(skb, TCA_OPTIONS);
	if (nest == NULL)
		goto nla_put_failure;

	if (!fh) {
		t->tcm_handle = ~0; /* whatever ... */
		if (nla_put_u32(skb, TCA_TCINDEX_HASH, p->hash) ||
		    nla_put_u16(skb, TCA_TCINDEX_MASK, p->mask) ||
		    nla_put_u32(skb, TCA_TCINDEX_SHIFT, p->shift) ||
		    nla_put_u32(skb, TCA_TCINDEX_FALL_THROUGH, p->fall_through))
			goto nla_put_failure;
		nla_nest_end(skb, nest);
	} else {
		if (p->perfect) {
			t->tcm_handle = r - p->perfect;
		} else {
			struct tcindex_filter *f;
			struct tcindex_filter __rcu **fp;
			int i;

			t->tcm_handle = 0;
			for (i = 0; !t->tcm_handle && i < p->hash; i++) {
				fp = &p->h[i];
				for (f = rtnl_dereference(*fp);
				     !t->tcm_handle && f;
				     fp = &f->next, f = rtnl_dereference(*fp)) {
					if (&f->result == r)
						t->tcm_handle = f->key;
				}
			}
		}
		pr_debug("handle = %d\n", t->tcm_handle);
		if (r->res.class &&
		    nla_put_u32(skb, TCA_TCINDEX_CLASSID, r->res.classid))
			goto nla_put_failure;

		if (tcf_exts_dump(skb, &r->exts) < 0)
			goto nla_put_failure;
		nla_nest_end(skb, nest);

		if (tcf_exts_dump_stats(skb, &r->exts) < 0)
			goto nla_put_failure;
	}

	return skb->len;

nla_put_failure:
	nla_nest_cancel(skb, nest);
	return -1;
}

static void tcindex_bind_class(void *fh, u32 classid, unsigned long cl,
			       void *q, unsigned long base)
{
	struct tcindex_filter_result *r = fh;

	if (r && r->res.classid == classid) {
		if (cl)
			__tcf_bind_filter(q, &r->res, base);
		else
			__tcf_unbind_filter(q, &r->res);
	}
}

static struct tcf_proto_ops cls_tcindex_ops __read_mostly = {
	.kind		=	"tcindex",
	.classify	=	tcindex_classify,
	.init		=	tcindex_init,
	.destroy	=	tcindex_destroy,
	.get		=	tcindex_get,
	.change		=	tcindex_change,
	.delete		=	tcindex_delete,
	.walk		=	tcindex_walk,
	.dump		=	tcindex_dump,
	.bind_class	=	tcindex_bind_class,
	.owner		=	THIS_MODULE,
};

static int __init init_tcindex(void)
{
	return register_tcf_proto_ops(&cls_tcindex_ops);
}

static void __exit exit_tcindex(void)
{
	unregister_tcf_proto_ops(&cls_tcindex_ops);
}

module_init(init_tcindex)
module_exit(exit_tcindex)
MODULE_LICENSE("GPL");
