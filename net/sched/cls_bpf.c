// SPDX-License-Identifier: GPL-2.0-only
/*
 * Berkeley Packet Filter based traffic classifier
 *
 * Might be used to classify traffic through flexible, user-defined and
 * possibly JIT-ed BPF filters for traffic control as an alternative to
 * ematches.
 *
 * (C) 2013 Daniel Borkmann <dborkman@redhat.com>
 */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/skbuff.h>
#include <linux/filter.h>
#include <linux/bpf.h>
#include <linux/idr.h>

#include <net/rtnetlink.h>
#include <net/pkt_cls.h>
#include <net/sock.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Daniel Borkmann <dborkman@redhat.com>");
MODULE_DESCRIPTION("TC BPF based classifier");

#define CLS_BPF_NAME_LEN	256
#define CLS_BPF_SUPPORTED_GEN_FLAGS		\
	(TCA_CLS_FLAGS_SKIP_HW | TCA_CLS_FLAGS_SKIP_SW)

struct cls_bpf_head {
	struct list_head plist;
	struct idr handle_idr;
	struct rcu_head rcu;
};

struct cls_bpf_prog {
	struct bpf_prog *filter;
	struct list_head link;
	struct tcf_result res;
	bool exts_integrated;
	u32 gen_flags;
	unsigned int in_hw_count;
	struct tcf_exts exts;
	u32 handle;
	u16 bpf_num_ops;
	struct sock_filter *bpf_ops;
	const char *bpf_name;
	struct tcf_proto *tp;
	struct rcu_work rwork;
};

static const struct nla_policy bpf_policy[TCA_BPF_MAX + 1] = {
	[TCA_BPF_CLASSID]	= { .type = NLA_U32 },
	[TCA_BPF_FLAGS]		= { .type = NLA_U32 },
	[TCA_BPF_FLAGS_GEN]	= { .type = NLA_U32 },
	[TCA_BPF_FD]		= { .type = NLA_U32 },
	[TCA_BPF_NAME]		= { .type = NLA_NUL_STRING,
				    .len = CLS_BPF_NAME_LEN },
	[TCA_BPF_OPS_LEN]	= { .type = NLA_U16 },
	[TCA_BPF_OPS]		= { .type = NLA_BINARY,
				    .len = sizeof(struct sock_filter) * BPF_MAXINSNS },
};

static int cls_bpf_exec_opcode(int code)
{
	switch (code) {
	case TC_ACT_OK:
	case TC_ACT_SHOT:
	case TC_ACT_STOLEN:
	case TC_ACT_TRAP:
	case TC_ACT_REDIRECT:
	case TC_ACT_UNSPEC:
		return code;
	default:
		return TC_ACT_UNSPEC;
	}
}

static int cls_bpf_classify(struct sk_buff *skb, const struct tcf_proto *tp,
			    struct tcf_result *res)
{
	struct cls_bpf_head *head = rcu_dereference_bh(tp->root);
	bool at_ingress = skb_at_tc_ingress(skb);
	struct cls_bpf_prog *prog;
	int ret = -1;

	/* Needed here for accessing maps. */
	rcu_read_lock();
	list_for_each_entry_rcu(prog, &head->plist, link) {
		int filter_res;

		qdisc_skb_cb(skb)->tc_classid = prog->res.classid;

		if (tc_skip_sw(prog->gen_flags)) {
			filter_res = prog->exts_integrated ? TC_ACT_UNSPEC : 0;
		} else if (at_ingress) {
			/* It is safe to push/pull even if skb_shared() */
			__skb_push(skb, skb->mac_len);
			bpf_compute_data_pointers(skb);
			filter_res = BPF_PROG_RUN(prog->filter, skb);
			__skb_pull(skb, skb->mac_len);
		} else {
			bpf_compute_data_pointers(skb);
			filter_res = BPF_PROG_RUN(prog->filter, skb);
		}

		if (prog->exts_integrated) {
			res->class   = 0;
			res->classid = TC_H_MAJ(prog->res.classid) |
				       qdisc_skb_cb(skb)->tc_classid;

			ret = cls_bpf_exec_opcode(filter_res);
			if (ret == TC_ACT_UNSPEC)
				continue;
			break;
		}

		if (filter_res == 0)
			continue;
		if (filter_res != -1) {
			res->class   = 0;
			res->classid = filter_res;
		} else {
			*res = prog->res;
		}

		ret = tcf_exts_exec(skb, &prog->exts, res);
		if (ret < 0)
			continue;

		break;
	}
	rcu_read_unlock();

	return ret;
}

static bool cls_bpf_is_ebpf(const struct cls_bpf_prog *prog)
{
	return !prog->bpf_ops;
}

static int cls_bpf_offload_cmd(struct tcf_proto *tp, struct cls_bpf_prog *prog,
			       struct cls_bpf_prog *oldprog,
			       struct netlink_ext_ack *extack)
{
	struct tcf_block *block = tp->chain->block;
	struct tc_cls_bpf_offload cls_bpf = {};
	struct cls_bpf_prog *obj;
	bool skip_sw;
	int err;

	skip_sw = prog && tc_skip_sw(prog->gen_flags);
	obj = prog ?: oldprog;

	tc_cls_common_offload_init(&cls_bpf.common, tp, obj->gen_flags, extack);
	cls_bpf.command = TC_CLSBPF_OFFLOAD;
	cls_bpf.exts = &obj->exts;
	cls_bpf.prog = prog ? prog->filter : NULL;
	cls_bpf.oldprog = oldprog ? oldprog->filter : NULL;
	cls_bpf.name = obj->bpf_name;
	cls_bpf.exts_integrated = obj->exts_integrated;

	if (oldprog && prog)
		err = tc_setup_cb_replace(block, tp, TC_SETUP_CLSBPF, &cls_bpf,
					  skip_sw, &oldprog->gen_flags,
					  &oldprog->in_hw_count,
					  &prog->gen_flags, &prog->in_hw_count,
					  true);
	else if (prog)
		err = tc_setup_cb_add(block, tp, TC_SETUP_CLSBPF, &cls_bpf,
				      skip_sw, &prog->gen_flags,
				      &prog->in_hw_count, true);
	else
		err = tc_setup_cb_destroy(block, tp, TC_SETUP_CLSBPF, &cls_bpf,
					  skip_sw, &oldprog->gen_flags,
					  &oldprog->in_hw_count, true);

	if (prog && err) {
		cls_bpf_offload_cmd(tp, oldprog, prog, extack);
		return err;
	}

	if (prog && skip_sw && !(prog->gen_flags & TCA_CLS_FLAGS_IN_HW))
		return -EINVAL;

	return 0;
}

static u32 cls_bpf_flags(u32 flags)
{
	return flags & CLS_BPF_SUPPORTED_GEN_FLAGS;
}

static int cls_bpf_offload(struct tcf_proto *tp, struct cls_bpf_prog *prog,
			   struct cls_bpf_prog *oldprog,
			   struct netlink_ext_ack *extack)
{
	if (prog && oldprog &&
	    cls_bpf_flags(prog->gen_flags) !=
	    cls_bpf_flags(oldprog->gen_flags))
		return -EINVAL;

	if (prog && tc_skip_hw(prog->gen_flags))
		prog = NULL;
	if (oldprog && tc_skip_hw(oldprog->gen_flags))
		oldprog = NULL;
	if (!prog && !oldprog)
		return 0;

	return cls_bpf_offload_cmd(tp, prog, oldprog, extack);
}

static void cls_bpf_stop_offload(struct tcf_proto *tp,
				 struct cls_bpf_prog *prog,
				 struct netlink_ext_ack *extack)
{
	int err;

	err = cls_bpf_offload_cmd(tp, NULL, prog, extack);
	if (err)
		pr_err("Stopping hardware offload failed: %d\n", err);
}

static void cls_bpf_offload_update_stats(struct tcf_proto *tp,
					 struct cls_bpf_prog *prog)
{
	struct tcf_block *block = tp->chain->block;
	struct tc_cls_bpf_offload cls_bpf = {};

	tc_cls_common_offload_init(&cls_bpf.common, tp, prog->gen_flags, NULL);
	cls_bpf.command = TC_CLSBPF_STATS;
	cls_bpf.exts = &prog->exts;
	cls_bpf.prog = prog->filter;
	cls_bpf.name = prog->bpf_name;
	cls_bpf.exts_integrated = prog->exts_integrated;

	tc_setup_cb_call(block, TC_SETUP_CLSBPF, &cls_bpf, false, true);
}

static int cls_bpf_init(struct tcf_proto *tp)
{
	struct cls_bpf_head *head;

	head = kzalloc(sizeof(*head), GFP_KERNEL);
	if (head == NULL)
		return -ENOBUFS;

	INIT_LIST_HEAD_RCU(&head->plist);
	idr_init(&head->handle_idr);
	rcu_assign_pointer(tp->root, head);

	return 0;
}

static void cls_bpf_free_parms(struct cls_bpf_prog *prog)
{
	if (cls_bpf_is_ebpf(prog))
		bpf_prog_put(prog->filter);
	else
		bpf_prog_destroy(prog->filter);

	kfree(prog->bpf_name);
	kfree(prog->bpf_ops);
}

static void __cls_bpf_delete_prog(struct cls_bpf_prog *prog)
{
	tcf_exts_destroy(&prog->exts);
	tcf_exts_put_net(&prog->exts);

	cls_bpf_free_parms(prog);
	kfree(prog);
}

static void cls_bpf_delete_prog_work(struct work_struct *work)
{
	struct cls_bpf_prog *prog = container_of(to_rcu_work(work),
						 struct cls_bpf_prog,
						 rwork);
	rtnl_lock();
	__cls_bpf_delete_prog(prog);
	rtnl_unlock();
}

static void __cls_bpf_delete(struct tcf_proto *tp, struct cls_bpf_prog *prog,
			     struct netlink_ext_ack *extack)
{
	struct cls_bpf_head *head = rtnl_dereference(tp->root);

	idr_remove(&head->handle_idr, prog->handle);
	cls_bpf_stop_offload(tp, prog, extack);
	list_del_rcu(&prog->link);
	tcf_unbind_filter(tp, &prog->res);
	if (tcf_exts_get_net(&prog->exts))
		tcf_queue_work(&prog->rwork, cls_bpf_delete_prog_work);
	else
		__cls_bpf_delete_prog(prog);
}

static int cls_bpf_delete(struct tcf_proto *tp, void *arg, bool *last,
			  bool rtnl_held, struct netlink_ext_ack *extack)
{
	struct cls_bpf_head *head = rtnl_dereference(tp->root);

	__cls_bpf_delete(tp, arg, extack);
	*last = list_empty(&head->plist);
	return 0;
}

static void cls_bpf_destroy(struct tcf_proto *tp, bool rtnl_held,
			    struct netlink_ext_ack *extack)
{
	struct cls_bpf_head *head = rtnl_dereference(tp->root);
	struct cls_bpf_prog *prog, *tmp;

	list_for_each_entry_safe(prog, tmp, &head->plist, link)
		__cls_bpf_delete(tp, prog, extack);

	idr_destroy(&head->handle_idr);
	kfree_rcu(head, rcu);
}

static void *cls_bpf_get(struct tcf_proto *tp, u32 handle)
{
	struct cls_bpf_head *head = rtnl_dereference(tp->root);
	struct cls_bpf_prog *prog;

	list_for_each_entry(prog, &head->plist, link) {
		if (prog->handle == handle)
			return prog;
	}

	return NULL;
}

static int cls_bpf_prog_from_ops(struct nlattr **tb, struct cls_bpf_prog *prog)
{
	struct sock_filter *bpf_ops;
	struct sock_fprog_kern fprog_tmp;
	struct bpf_prog *fp;
	u16 bpf_size, bpf_num_ops;
	int ret;

	bpf_num_ops = nla_get_u16(tb[TCA_BPF_OPS_LEN]);
	if (bpf_num_ops > BPF_MAXINSNS || bpf_num_ops == 0)
		return -EINVAL;

	bpf_size = bpf_num_ops * sizeof(*bpf_ops);
	if (bpf_size != nla_len(tb[TCA_BPF_OPS]))
		return -EINVAL;

	bpf_ops = kmemdup(nla_data(tb[TCA_BPF_OPS]), bpf_size, GFP_KERNEL);
	if (bpf_ops == NULL)
		return -ENOMEM;

	fprog_tmp.len = bpf_num_ops;
	fprog_tmp.filter = bpf_ops;

	ret = bpf_prog_create(&fp, &fprog_tmp);
	if (ret < 0) {
		kfree(bpf_ops);
		return ret;
	}

	prog->bpf_ops = bpf_ops;
	prog->bpf_num_ops = bpf_num_ops;
	prog->bpf_name = NULL;
	prog->filter = fp;

	return 0;
}

static int cls_bpf_prog_from_efd(struct nlattr **tb, struct cls_bpf_prog *prog,
				 u32 gen_flags, const struct tcf_proto *tp)
{
	struct bpf_prog *fp;
	char *name = NULL;
	bool skip_sw;
	u32 bpf_fd;

	bpf_fd = nla_get_u32(tb[TCA_BPF_FD]);
	skip_sw = gen_flags & TCA_CLS_FLAGS_SKIP_SW;

	fp = bpf_prog_get_type_dev(bpf_fd, BPF_PROG_TYPE_SCHED_CLS, skip_sw);
	if (IS_ERR(fp))
		return PTR_ERR(fp);

	if (tb[TCA_BPF_NAME]) {
		name = nla_memdup(tb[TCA_BPF_NAME], GFP_KERNEL);
		if (!name) {
			bpf_prog_put(fp);
			return -ENOMEM;
		}
	}

	prog->bpf_ops = NULL;
	prog->bpf_name = name;
	prog->filter = fp;

	if (fp->dst_needed)
		tcf_block_netif_keep_dst(tp->chain->block);

	return 0;
}

static int cls_bpf_set_parms(struct net *net, struct tcf_proto *tp,
			     struct cls_bpf_prog *prog, unsigned long base,
			     struct nlattr **tb, struct nlattr *est, bool ovr,
			     struct netlink_ext_ack *extack)
{
	bool is_bpf, is_ebpf, have_exts = false;
	u32 gen_flags = 0;
	int ret;

	is_bpf = tb[TCA_BPF_OPS_LEN] && tb[TCA_BPF_OPS];
	is_ebpf = tb[TCA_BPF_FD];
	if ((!is_bpf && !is_ebpf) || (is_bpf && is_ebpf))
		return -EINVAL;

	ret = tcf_exts_validate(net, tp, tb, est, &prog->exts, ovr, true,
				extack);
	if (ret < 0)
		return ret;

	if (tb[TCA_BPF_FLAGS]) {
		u32 bpf_flags = nla_get_u32(tb[TCA_BPF_FLAGS]);

		if (bpf_flags & ~TCA_BPF_FLAG_ACT_DIRECT)
			return -EINVAL;

		have_exts = bpf_flags & TCA_BPF_FLAG_ACT_DIRECT;
	}
	if (tb[TCA_BPF_FLAGS_GEN]) {
		gen_flags = nla_get_u32(tb[TCA_BPF_FLAGS_GEN]);
		if (gen_flags & ~CLS_BPF_SUPPORTED_GEN_FLAGS ||
		    !tc_flags_valid(gen_flags))
			return -EINVAL;
	}

	prog->exts_integrated = have_exts;
	prog->gen_flags = gen_flags;

	ret = is_bpf ? cls_bpf_prog_from_ops(tb, prog) :
		       cls_bpf_prog_from_efd(tb, prog, gen_flags, tp);
	if (ret < 0)
		return ret;

	if (tb[TCA_BPF_CLASSID]) {
		prog->res.classid = nla_get_u32(tb[TCA_BPF_CLASSID]);
		tcf_bind_filter(tp, &prog->res, base);
	}

	return 0;
}

static int cls_bpf_change(struct net *net, struct sk_buff *in_skb,
			  struct tcf_proto *tp, unsigned long base,
			  u32 handle, struct nlattr **tca,
			  void **arg, bool ovr, bool rtnl_held,
			  struct netlink_ext_ack *extack)
{
	struct cls_bpf_head *head = rtnl_dereference(tp->root);
	struct cls_bpf_prog *oldprog = *arg;
	struct nlattr *tb[TCA_BPF_MAX + 1];
	struct cls_bpf_prog *prog;
	int ret;

	if (tca[TCA_OPTIONS] == NULL)
		return -EINVAL;

	ret = nla_parse_nested_deprecated(tb, TCA_BPF_MAX, tca[TCA_OPTIONS],
					  bpf_policy, NULL);
	if (ret < 0)
		return ret;

	prog = kzalloc(sizeof(*prog), GFP_KERNEL);
	if (!prog)
		return -ENOBUFS;

	ret = tcf_exts_init(&prog->exts, net, TCA_BPF_ACT, TCA_BPF_POLICE);
	if (ret < 0)
		goto errout;

	if (oldprog) {
		if (handle && oldprog->handle != handle) {
			ret = -EINVAL;
			goto errout;
		}
	}

	if (handle == 0) {
		handle = 1;
		ret = idr_alloc_u32(&head->handle_idr, prog, &handle,
				    INT_MAX, GFP_KERNEL);
	} else if (!oldprog) {
		ret = idr_alloc_u32(&head->handle_idr, prog, &handle,
				    handle, GFP_KERNEL);
	}

	if (ret)
		goto errout;
	prog->handle = handle;

	ret = cls_bpf_set_parms(net, tp, prog, base, tb, tca[TCA_RATE], ovr,
				extack);
	if (ret < 0)
		goto errout_idr;

	ret = cls_bpf_offload(tp, prog, oldprog, extack);
	if (ret)
		goto errout_parms;

	if (!tc_in_hw(prog->gen_flags))
		prog->gen_flags |= TCA_CLS_FLAGS_NOT_IN_HW;

	if (oldprog) {
		idr_replace(&head->handle_idr, prog, handle);
		list_replace_rcu(&oldprog->link, &prog->link);
		tcf_unbind_filter(tp, &oldprog->res);
		tcf_exts_get_net(&oldprog->exts);
		tcf_queue_work(&oldprog->rwork, cls_bpf_delete_prog_work);
	} else {
		list_add_rcu(&prog->link, &head->plist);
	}

	*arg = prog;
	return 0;

errout_parms:
	cls_bpf_free_parms(prog);
errout_idr:
	if (!oldprog)
		idr_remove(&head->handle_idr, prog->handle);
errout:
	tcf_exts_destroy(&prog->exts);
	kfree(prog);
	return ret;
}

static int cls_bpf_dump_bpf_info(const struct cls_bpf_prog *prog,
				 struct sk_buff *skb)
{
	struct nlattr *nla;

	if (nla_put_u16(skb, TCA_BPF_OPS_LEN, prog->bpf_num_ops))
		return -EMSGSIZE;

	nla = nla_reserve(skb, TCA_BPF_OPS, prog->bpf_num_ops *
			  sizeof(struct sock_filter));
	if (nla == NULL)
		return -EMSGSIZE;

	memcpy(nla_data(nla), prog->bpf_ops, nla_len(nla));

	return 0;
}

static int cls_bpf_dump_ebpf_info(const struct cls_bpf_prog *prog,
				  struct sk_buff *skb)
{
	struct nlattr *nla;

	if (prog->bpf_name &&
	    nla_put_string(skb, TCA_BPF_NAME, prog->bpf_name))
		return -EMSGSIZE;

	if (nla_put_u32(skb, TCA_BPF_ID, prog->filter->aux->id))
		return -EMSGSIZE;

	nla = nla_reserve(skb, TCA_BPF_TAG, sizeof(prog->filter->tag));
	if (nla == NULL)
		return -EMSGSIZE;

	memcpy(nla_data(nla), prog->filter->tag, nla_len(nla));

	return 0;
}

static int cls_bpf_dump(struct net *net, struct tcf_proto *tp, void *fh,
			struct sk_buff *skb, struct tcmsg *tm, bool rtnl_held)
{
	struct cls_bpf_prog *prog = fh;
	struct nlattr *nest;
	u32 bpf_flags = 0;
	int ret;

	if (prog == NULL)
		return skb->len;

	tm->tcm_handle = prog->handle;

	cls_bpf_offload_update_stats(tp, prog);

	nest = nla_nest_start_noflag(skb, TCA_OPTIONS);
	if (nest == NULL)
		goto nla_put_failure;

	if (prog->res.classid &&
	    nla_put_u32(skb, TCA_BPF_CLASSID, prog->res.classid))
		goto nla_put_failure;

	if (cls_bpf_is_ebpf(prog))
		ret = cls_bpf_dump_ebpf_info(prog, skb);
	else
		ret = cls_bpf_dump_bpf_info(prog, skb);
	if (ret)
		goto nla_put_failure;

	if (tcf_exts_dump(skb, &prog->exts) < 0)
		goto nla_put_failure;

	if (prog->exts_integrated)
		bpf_flags |= TCA_BPF_FLAG_ACT_DIRECT;
	if (bpf_flags && nla_put_u32(skb, TCA_BPF_FLAGS, bpf_flags))
		goto nla_put_failure;
	if (prog->gen_flags &&
	    nla_put_u32(skb, TCA_BPF_FLAGS_GEN, prog->gen_flags))
		goto nla_put_failure;

	nla_nest_end(skb, nest);

	if (tcf_exts_dump_stats(skb, &prog->exts) < 0)
		goto nla_put_failure;

	return skb->len;

nla_put_failure:
	nla_nest_cancel(skb, nest);
	return -1;
}

static void cls_bpf_bind_class(void *fh, u32 classid, unsigned long cl,
			       void *q, unsigned long base)
{
	struct cls_bpf_prog *prog = fh;

	if (prog && prog->res.classid == classid) {
		if (cl)
			__tcf_bind_filter(q, &prog->res, base);
		else
			__tcf_unbind_filter(q, &prog->res);
	}
}

static void cls_bpf_walk(struct tcf_proto *tp, struct tcf_walker *arg,
			 bool rtnl_held)
{
	struct cls_bpf_head *head = rtnl_dereference(tp->root);
	struct cls_bpf_prog *prog;

	list_for_each_entry(prog, &head->plist, link) {
		if (arg->count < arg->skip)
			goto skip;
		if (arg->fn(tp, prog, arg) < 0) {
			arg->stop = 1;
			break;
		}
skip:
		arg->count++;
	}
}

static int cls_bpf_reoffload(struct tcf_proto *tp, bool add, flow_setup_cb_t *cb,
			     void *cb_priv, struct netlink_ext_ack *extack)
{
	struct cls_bpf_head *head = rtnl_dereference(tp->root);
	struct tcf_block *block = tp->chain->block;
	struct tc_cls_bpf_offload cls_bpf = {};
	struct cls_bpf_prog *prog;
	int err;

	list_for_each_entry(prog, &head->plist, link) {
		if (tc_skip_hw(prog->gen_flags))
			continue;

		tc_cls_common_offload_init(&cls_bpf.common, tp, prog->gen_flags,
					   extack);
		cls_bpf.command = TC_CLSBPF_OFFLOAD;
		cls_bpf.exts = &prog->exts;
		cls_bpf.prog = add ? prog->filter : NULL;
		cls_bpf.oldprog = add ? NULL : prog->filter;
		cls_bpf.name = prog->bpf_name;
		cls_bpf.exts_integrated = prog->exts_integrated;

		err = tc_setup_cb_reoffload(block, tp, add, cb, TC_SETUP_CLSBPF,
					    &cls_bpf, cb_priv, &prog->gen_flags,
					    &prog->in_hw_count);
		if (err)
			return err;
	}

	return 0;
}

static struct tcf_proto_ops cls_bpf_ops __read_mostly = {
	.kind		=	"bpf",
	.owner		=	THIS_MODULE,
	.classify	=	cls_bpf_classify,
	.init		=	cls_bpf_init,
	.destroy	=	cls_bpf_destroy,
	.get		=	cls_bpf_get,
	.change		=	cls_bpf_change,
	.delete		=	cls_bpf_delete,
	.walk		=	cls_bpf_walk,
	.reoffload	=	cls_bpf_reoffload,
	.dump		=	cls_bpf_dump,
	.bind_class	=	cls_bpf_bind_class,
};

static int __init cls_bpf_init_mod(void)
{
	return register_tcf_proto_ops(&cls_bpf_ops);
}

static void __exit cls_bpf_exit_mod(void)
{
	unregister_tcf_proto_ops(&cls_bpf_ops);
}

module_init(cls_bpf_init_mod);
module_exit(cls_bpf_exit_mod);
