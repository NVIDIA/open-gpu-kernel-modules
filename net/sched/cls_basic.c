// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * net/sched/cls_basic.c	Basic Packet Classifier.
 *
 * Authors:	Thomas Graf <tgraf@suug.ch>
 */

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <linux/rtnetlink.h>
#include <linux/skbuff.h>
#include <linux/idr.h>
#include <linux/percpu.h>
#include <net/netlink.h>
#include <net/act_api.h>
#include <net/pkt_cls.h>

struct basic_head {
	struct list_head	flist;
	struct idr		handle_idr;
	struct rcu_head		rcu;
};

struct basic_filter {
	u32			handle;
	struct tcf_exts		exts;
	struct tcf_ematch_tree	ematches;
	struct tcf_result	res;
	struct tcf_proto	*tp;
	struct list_head	link;
	struct tc_basic_pcnt __percpu *pf;
	struct rcu_work		rwork;
};

static int basic_classify(struct sk_buff *skb, const struct tcf_proto *tp,
			  struct tcf_result *res)
{
	int r;
	struct basic_head *head = rcu_dereference_bh(tp->root);
	struct basic_filter *f;

	list_for_each_entry_rcu(f, &head->flist, link) {
		__this_cpu_inc(f->pf->rcnt);
		if (!tcf_em_tree_match(skb, &f->ematches, NULL))
			continue;
		__this_cpu_inc(f->pf->rhit);
		*res = f->res;
		r = tcf_exts_exec(skb, &f->exts, res);
		if (r < 0)
			continue;
		return r;
	}
	return -1;
}

static void *basic_get(struct tcf_proto *tp, u32 handle)
{
	struct basic_head *head = rtnl_dereference(tp->root);
	struct basic_filter *f;

	list_for_each_entry(f, &head->flist, link) {
		if (f->handle == handle) {
			return f;
		}
	}

	return NULL;
}

static int basic_init(struct tcf_proto *tp)
{
	struct basic_head *head;

	head = kzalloc(sizeof(*head), GFP_KERNEL);
	if (head == NULL)
		return -ENOBUFS;
	INIT_LIST_HEAD(&head->flist);
	idr_init(&head->handle_idr);
	rcu_assign_pointer(tp->root, head);
	return 0;
}

static void __basic_delete_filter(struct basic_filter *f)
{
	tcf_exts_destroy(&f->exts);
	tcf_em_tree_destroy(&f->ematches);
	tcf_exts_put_net(&f->exts);
	free_percpu(f->pf);
	kfree(f);
}

static void basic_delete_filter_work(struct work_struct *work)
{
	struct basic_filter *f = container_of(to_rcu_work(work),
					      struct basic_filter,
					      rwork);
	rtnl_lock();
	__basic_delete_filter(f);
	rtnl_unlock();
}

static void basic_destroy(struct tcf_proto *tp, bool rtnl_held,
			  struct netlink_ext_ack *extack)
{
	struct basic_head *head = rtnl_dereference(tp->root);
	struct basic_filter *f, *n;

	list_for_each_entry_safe(f, n, &head->flist, link) {
		list_del_rcu(&f->link);
		tcf_unbind_filter(tp, &f->res);
		idr_remove(&head->handle_idr, f->handle);
		if (tcf_exts_get_net(&f->exts))
			tcf_queue_work(&f->rwork, basic_delete_filter_work);
		else
			__basic_delete_filter(f);
	}
	idr_destroy(&head->handle_idr);
	kfree_rcu(head, rcu);
}

static int basic_delete(struct tcf_proto *tp, void *arg, bool *last,
			bool rtnl_held, struct netlink_ext_ack *extack)
{
	struct basic_head *head = rtnl_dereference(tp->root);
	struct basic_filter *f = arg;

	list_del_rcu(&f->link);
	tcf_unbind_filter(tp, &f->res);
	idr_remove(&head->handle_idr, f->handle);
	tcf_exts_get_net(&f->exts);
	tcf_queue_work(&f->rwork, basic_delete_filter_work);
	*last = list_empty(&head->flist);
	return 0;
}

static const struct nla_policy basic_policy[TCA_BASIC_MAX + 1] = {
	[TCA_BASIC_CLASSID]	= { .type = NLA_U32 },
	[TCA_BASIC_EMATCHES]	= { .type = NLA_NESTED },
};

static int basic_set_parms(struct net *net, struct tcf_proto *tp,
			   struct basic_filter *f, unsigned long base,
			   struct nlattr **tb,
			   struct nlattr *est, bool ovr,
			   struct netlink_ext_ack *extack)
{
	int err;

	err = tcf_exts_validate(net, tp, tb, est, &f->exts, ovr, true, extack);
	if (err < 0)
		return err;

	err = tcf_em_tree_validate(tp, tb[TCA_BASIC_EMATCHES], &f->ematches);
	if (err < 0)
		return err;

	if (tb[TCA_BASIC_CLASSID]) {
		f->res.classid = nla_get_u32(tb[TCA_BASIC_CLASSID]);
		tcf_bind_filter(tp, &f->res, base);
	}

	f->tp = tp;
	return 0;
}

static int basic_change(struct net *net, struct sk_buff *in_skb,
			struct tcf_proto *tp, unsigned long base, u32 handle,
			struct nlattr **tca, void **arg, bool ovr,
			bool rtnl_held, struct netlink_ext_ack *extack)
{
	int err;
	struct basic_head *head = rtnl_dereference(tp->root);
	struct nlattr *tb[TCA_BASIC_MAX + 1];
	struct basic_filter *fold = (struct basic_filter *) *arg;
	struct basic_filter *fnew;

	if (tca[TCA_OPTIONS] == NULL)
		return -EINVAL;

	err = nla_parse_nested_deprecated(tb, TCA_BASIC_MAX, tca[TCA_OPTIONS],
					  basic_policy, NULL);
	if (err < 0)
		return err;

	if (fold != NULL) {
		if (handle && fold->handle != handle)
			return -EINVAL;
	}

	fnew = kzalloc(sizeof(*fnew), GFP_KERNEL);
	if (!fnew)
		return -ENOBUFS;

	err = tcf_exts_init(&fnew->exts, net, TCA_BASIC_ACT, TCA_BASIC_POLICE);
	if (err < 0)
		goto errout;

	if (!handle) {
		handle = 1;
		err = idr_alloc_u32(&head->handle_idr, fnew, &handle,
				    INT_MAX, GFP_KERNEL);
	} else if (!fold) {
		err = idr_alloc_u32(&head->handle_idr, fnew, &handle,
				    handle, GFP_KERNEL);
	}
	if (err)
		goto errout;
	fnew->handle = handle;
	fnew->pf = alloc_percpu(struct tc_basic_pcnt);
	if (!fnew->pf) {
		err = -ENOMEM;
		goto errout;
	}

	err = basic_set_parms(net, tp, fnew, base, tb, tca[TCA_RATE], ovr,
			      extack);
	if (err < 0) {
		if (!fold)
			idr_remove(&head->handle_idr, fnew->handle);
		goto errout;
	}

	*arg = fnew;

	if (fold) {
		idr_replace(&head->handle_idr, fnew, fnew->handle);
		list_replace_rcu(&fold->link, &fnew->link);
		tcf_unbind_filter(tp, &fold->res);
		tcf_exts_get_net(&fold->exts);
		tcf_queue_work(&fold->rwork, basic_delete_filter_work);
	} else {
		list_add_rcu(&fnew->link, &head->flist);
	}

	return 0;
errout:
	free_percpu(fnew->pf);
	tcf_exts_destroy(&fnew->exts);
	kfree(fnew);
	return err;
}

static void basic_walk(struct tcf_proto *tp, struct tcf_walker *arg,
		       bool rtnl_held)
{
	struct basic_head *head = rtnl_dereference(tp->root);
	struct basic_filter *f;

	list_for_each_entry(f, &head->flist, link) {
		if (arg->count < arg->skip)
			goto skip;

		if (arg->fn(tp, f, arg) < 0) {
			arg->stop = 1;
			break;
		}
skip:
		arg->count++;
	}
}

static void basic_bind_class(void *fh, u32 classid, unsigned long cl, void *q,
			     unsigned long base)
{
	struct basic_filter *f = fh;

	if (f && f->res.classid == classid) {
		if (cl)
			__tcf_bind_filter(q, &f->res, base);
		else
			__tcf_unbind_filter(q, &f->res);
	}
}

static int basic_dump(struct net *net, struct tcf_proto *tp, void *fh,
		      struct sk_buff *skb, struct tcmsg *t, bool rtnl_held)
{
	struct tc_basic_pcnt gpf = {};
	struct basic_filter *f = fh;
	struct nlattr *nest;
	int cpu;

	if (f == NULL)
		return skb->len;

	t->tcm_handle = f->handle;

	nest = nla_nest_start_noflag(skb, TCA_OPTIONS);
	if (nest == NULL)
		goto nla_put_failure;

	if (f->res.classid &&
	    nla_put_u32(skb, TCA_BASIC_CLASSID, f->res.classid))
		goto nla_put_failure;

	for_each_possible_cpu(cpu) {
		struct tc_basic_pcnt *pf = per_cpu_ptr(f->pf, cpu);

		gpf.rcnt += pf->rcnt;
		gpf.rhit += pf->rhit;
	}

	if (nla_put_64bit(skb, TCA_BASIC_PCNT,
			  sizeof(struct tc_basic_pcnt),
			  &gpf, TCA_BASIC_PAD))
		goto nla_put_failure;

	if (tcf_exts_dump(skb, &f->exts) < 0 ||
	    tcf_em_tree_dump(skb, &f->ematches, TCA_BASIC_EMATCHES) < 0)
		goto nla_put_failure;

	nla_nest_end(skb, nest);

	if (tcf_exts_dump_stats(skb, &f->exts) < 0)
		goto nla_put_failure;

	return skb->len;

nla_put_failure:
	nla_nest_cancel(skb, nest);
	return -1;
}

static struct tcf_proto_ops cls_basic_ops __read_mostly = {
	.kind		=	"basic",
	.classify	=	basic_classify,
	.init		=	basic_init,
	.destroy	=	basic_destroy,
	.get		=	basic_get,
	.change		=	basic_change,
	.delete		=	basic_delete,
	.walk		=	basic_walk,
	.dump		=	basic_dump,
	.bind_class	=	basic_bind_class,
	.owner		=	THIS_MODULE,
};

static int __init init_basic(void)
{
	return register_tcf_proto_ops(&cls_basic_ops);
}

static void __exit exit_basic(void)
{
	unregister_tcf_proto_ops(&cls_basic_ops);
}

module_init(init_basic)
module_exit(exit_basic)
MODULE_LICENSE("GPL");
