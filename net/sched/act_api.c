// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * net/sched/act_api.c	Packet action API.
 *
 * Author:	Jamal Hadi Salim
 */

#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/skbuff.h>
#include <linux/init.h>
#include <linux/kmod.h>
#include <linux/err.h>
#include <linux/module.h>
#include <net/net_namespace.h>
#include <net/sock.h>
#include <net/sch_generic.h>
#include <net/pkt_cls.h>
#include <net/act_api.h>
#include <net/netlink.h>

#ifdef CONFIG_INET
DEFINE_STATIC_KEY_FALSE(tcf_frag_xmit_count);
EXPORT_SYMBOL_GPL(tcf_frag_xmit_count);
#endif

int tcf_dev_queue_xmit(struct sk_buff *skb, int (*xmit)(struct sk_buff *skb))
{
#ifdef CONFIG_INET
	if (static_branch_unlikely(&tcf_frag_xmit_count))
		return sch_frag_xmit_hook(skb, xmit);
#endif

	return xmit(skb);
}
EXPORT_SYMBOL_GPL(tcf_dev_queue_xmit);

static void tcf_action_goto_chain_exec(const struct tc_action *a,
				       struct tcf_result *res)
{
	const struct tcf_chain *chain = rcu_dereference_bh(a->goto_chain);

	res->goto_tp = rcu_dereference_bh(chain->filter_chain);
}

static void tcf_free_cookie_rcu(struct rcu_head *p)
{
	struct tc_cookie *cookie = container_of(p, struct tc_cookie, rcu);

	kfree(cookie->data);
	kfree(cookie);
}

static void tcf_set_action_cookie(struct tc_cookie __rcu **old_cookie,
				  struct tc_cookie *new_cookie)
{
	struct tc_cookie *old;

	old = xchg((__force struct tc_cookie **)old_cookie, new_cookie);
	if (old)
		call_rcu(&old->rcu, tcf_free_cookie_rcu);
}

int tcf_action_check_ctrlact(int action, struct tcf_proto *tp,
			     struct tcf_chain **newchain,
			     struct netlink_ext_ack *extack)
{
	int opcode = TC_ACT_EXT_OPCODE(action), ret = -EINVAL;
	u32 chain_index;

	if (!opcode)
		ret = action > TC_ACT_VALUE_MAX ? -EINVAL : 0;
	else if (opcode <= TC_ACT_EXT_OPCODE_MAX || action == TC_ACT_UNSPEC)
		ret = 0;
	if (ret) {
		NL_SET_ERR_MSG(extack, "invalid control action");
		goto end;
	}

	if (TC_ACT_EXT_CMP(action, TC_ACT_GOTO_CHAIN)) {
		chain_index = action & TC_ACT_EXT_VAL_MASK;
		if (!tp || !newchain) {
			ret = -EINVAL;
			NL_SET_ERR_MSG(extack,
				       "can't goto NULL proto/chain");
			goto end;
		}
		*newchain = tcf_chain_get_by_act(tp->chain->block, chain_index);
		if (!*newchain) {
			ret = -ENOMEM;
			NL_SET_ERR_MSG(extack,
				       "can't allocate goto_chain");
		}
	}
end:
	return ret;
}
EXPORT_SYMBOL(tcf_action_check_ctrlact);

struct tcf_chain *tcf_action_set_ctrlact(struct tc_action *a, int action,
					 struct tcf_chain *goto_chain)
{
	a->tcfa_action = action;
	goto_chain = rcu_replace_pointer(a->goto_chain, goto_chain, 1);
	return goto_chain;
}
EXPORT_SYMBOL(tcf_action_set_ctrlact);

/* XXX: For standalone actions, we don't need a RCU grace period either, because
 * actions are always connected to filters and filters are already destroyed in
 * RCU callbacks, so after a RCU grace period actions are already disconnected
 * from filters. Readers later can not find us.
 */
static void free_tcf(struct tc_action *p)
{
	struct tcf_chain *chain = rcu_dereference_protected(p->goto_chain, 1);

	free_percpu(p->cpu_bstats);
	free_percpu(p->cpu_bstats_hw);
	free_percpu(p->cpu_qstats);

	tcf_set_action_cookie(&p->act_cookie, NULL);
	if (chain)
		tcf_chain_put_by_act(chain);

	kfree(p);
}

static void tcf_action_cleanup(struct tc_action *p)
{
	if (p->ops->cleanup)
		p->ops->cleanup(p);

	gen_kill_estimator(&p->tcfa_rate_est);
	free_tcf(p);
}

static int __tcf_action_put(struct tc_action *p, bool bind)
{
	struct tcf_idrinfo *idrinfo = p->idrinfo;

	if (refcount_dec_and_mutex_lock(&p->tcfa_refcnt, &idrinfo->lock)) {
		if (bind)
			atomic_dec(&p->tcfa_bindcnt);
		idr_remove(&idrinfo->action_idr, p->tcfa_index);
		mutex_unlock(&idrinfo->lock);

		tcf_action_cleanup(p);
		return 1;
	}

	if (bind)
		atomic_dec(&p->tcfa_bindcnt);

	return 0;
}

static int __tcf_idr_release(struct tc_action *p, bool bind, bool strict)
{
	int ret = 0;

	/* Release with strict==1 and bind==0 is only called through act API
	 * interface (classifiers always bind). Only case when action with
	 * positive reference count and zero bind count can exist is when it was
	 * also created with act API (unbinding last classifier will destroy the
	 * action if it was created by classifier). So only case when bind count
	 * can be changed after initial check is when unbound action is
	 * destroyed by act API while classifier binds to action with same id
	 * concurrently. This result either creation of new action(same behavior
	 * as before), or reusing existing action if concurrent process
	 * increments reference count before action is deleted. Both scenarios
	 * are acceptable.
	 */
	if (p) {
		if (!bind && strict && atomic_read(&p->tcfa_bindcnt) > 0)
			return -EPERM;

		if (__tcf_action_put(p, bind))
			ret = ACT_P_DELETED;
	}

	return ret;
}

int tcf_idr_release(struct tc_action *a, bool bind)
{
	const struct tc_action_ops *ops = a->ops;
	int ret;

	ret = __tcf_idr_release(a, bind, false);
	if (ret == ACT_P_DELETED)
		module_put(ops->owner);
	return ret;
}
EXPORT_SYMBOL(tcf_idr_release);

static size_t tcf_action_shared_attrs_size(const struct tc_action *act)
{
	struct tc_cookie *act_cookie;
	u32 cookie_len = 0;

	rcu_read_lock();
	act_cookie = rcu_dereference(act->act_cookie);

	if (act_cookie)
		cookie_len = nla_total_size(act_cookie->len);
	rcu_read_unlock();

	return  nla_total_size(0) /* action number nested */
		+ nla_total_size(IFNAMSIZ) /* TCA_ACT_KIND */
		+ cookie_len /* TCA_ACT_COOKIE */
		+ nla_total_size(sizeof(struct nla_bitfield32)) /* TCA_ACT_HW_STATS */
		+ nla_total_size(0) /* TCA_ACT_STATS nested */
		+ nla_total_size(sizeof(struct nla_bitfield32)) /* TCA_ACT_FLAGS */
		/* TCA_STATS_BASIC */
		+ nla_total_size_64bit(sizeof(struct gnet_stats_basic))
		/* TCA_STATS_PKT64 */
		+ nla_total_size_64bit(sizeof(u64))
		/* TCA_STATS_QUEUE */
		+ nla_total_size_64bit(sizeof(struct gnet_stats_queue))
		+ nla_total_size(0) /* TCA_OPTIONS nested */
		+ nla_total_size(sizeof(struct tcf_t)); /* TCA_GACT_TM */
}

static size_t tcf_action_full_attrs_size(size_t sz)
{
	return NLMSG_HDRLEN                     /* struct nlmsghdr */
		+ sizeof(struct tcamsg)
		+ nla_total_size(0)             /* TCA_ACT_TAB nested */
		+ sz;
}

static size_t tcf_action_fill_size(const struct tc_action *act)
{
	size_t sz = tcf_action_shared_attrs_size(act);

	if (act->ops->get_fill_size)
		return act->ops->get_fill_size(act) + sz;
	return sz;
}

static int
tcf_action_dump_terse(struct sk_buff *skb, struct tc_action *a, bool from_act)
{
	unsigned char *b = skb_tail_pointer(skb);
	struct tc_cookie *cookie;

	if (nla_put_string(skb, TCA_KIND, a->ops->kind))
		goto nla_put_failure;
	if (tcf_action_copy_stats(skb, a, 0))
		goto nla_put_failure;
	if (from_act && nla_put_u32(skb, TCA_ACT_INDEX, a->tcfa_index))
		goto nla_put_failure;

	rcu_read_lock();
	cookie = rcu_dereference(a->act_cookie);
	if (cookie) {
		if (nla_put(skb, TCA_ACT_COOKIE, cookie->len, cookie->data)) {
			rcu_read_unlock();
			goto nla_put_failure;
		}
	}
	rcu_read_unlock();

	return 0;

nla_put_failure:
	nlmsg_trim(skb, b);
	return -1;
}

static int tcf_dump_walker(struct tcf_idrinfo *idrinfo, struct sk_buff *skb,
			   struct netlink_callback *cb)
{
	int err = 0, index = -1, s_i = 0, n_i = 0;
	u32 act_flags = cb->args[2];
	unsigned long jiffy_since = cb->args[3];
	struct nlattr *nest;
	struct idr *idr = &idrinfo->action_idr;
	struct tc_action *p;
	unsigned long id = 1;
	unsigned long tmp;

	mutex_lock(&idrinfo->lock);

	s_i = cb->args[0];

	idr_for_each_entry_ul(idr, p, tmp, id) {
		index++;
		if (index < s_i)
			continue;
		if (IS_ERR(p))
			continue;

		if (jiffy_since &&
		    time_after(jiffy_since,
			       (unsigned long)p->tcfa_tm.lastuse))
			continue;

		nest = nla_nest_start_noflag(skb, n_i);
		if (!nest) {
			index--;
			goto nla_put_failure;
		}
		err = (act_flags & TCA_ACT_FLAG_TERSE_DUMP) ?
			tcf_action_dump_terse(skb, p, true) :
			tcf_action_dump_1(skb, p, 0, 0);
		if (err < 0) {
			index--;
			nlmsg_trim(skb, nest);
			goto done;
		}
		nla_nest_end(skb, nest);
		n_i++;
		if (!(act_flags & TCA_ACT_FLAG_LARGE_DUMP_ON) &&
		    n_i >= TCA_ACT_MAX_PRIO)
			goto done;
	}
done:
	if (index >= 0)
		cb->args[0] = index + 1;

	mutex_unlock(&idrinfo->lock);
	if (n_i) {
		if (act_flags & TCA_ACT_FLAG_LARGE_DUMP_ON)
			cb->args[1] = n_i;
	}
	return n_i;

nla_put_failure:
	nla_nest_cancel(skb, nest);
	goto done;
}

static int tcf_idr_release_unsafe(struct tc_action *p)
{
	if (atomic_read(&p->tcfa_bindcnt) > 0)
		return -EPERM;

	if (refcount_dec_and_test(&p->tcfa_refcnt)) {
		idr_remove(&p->idrinfo->action_idr, p->tcfa_index);
		tcf_action_cleanup(p);
		return ACT_P_DELETED;
	}

	return 0;
}

static int tcf_del_walker(struct tcf_idrinfo *idrinfo, struct sk_buff *skb,
			  const struct tc_action_ops *ops)
{
	struct nlattr *nest;
	int n_i = 0;
	int ret = -EINVAL;
	struct idr *idr = &idrinfo->action_idr;
	struct tc_action *p;
	unsigned long id = 1;
	unsigned long tmp;

	nest = nla_nest_start_noflag(skb, 0);
	if (nest == NULL)
		goto nla_put_failure;
	if (nla_put_string(skb, TCA_KIND, ops->kind))
		goto nla_put_failure;

	mutex_lock(&idrinfo->lock);
	idr_for_each_entry_ul(idr, p, tmp, id) {
		if (IS_ERR(p))
			continue;
		ret = tcf_idr_release_unsafe(p);
		if (ret == ACT_P_DELETED) {
			module_put(ops->owner);
			n_i++;
		} else if (ret < 0) {
			mutex_unlock(&idrinfo->lock);
			goto nla_put_failure;
		}
	}
	mutex_unlock(&idrinfo->lock);

	if (nla_put_u32(skb, TCA_FCNT, n_i))
		goto nla_put_failure;
	nla_nest_end(skb, nest);

	return n_i;
nla_put_failure:
	nla_nest_cancel(skb, nest);
	return ret;
}

int tcf_generic_walker(struct tc_action_net *tn, struct sk_buff *skb,
		       struct netlink_callback *cb, int type,
		       const struct tc_action_ops *ops,
		       struct netlink_ext_ack *extack)
{
	struct tcf_idrinfo *idrinfo = tn->idrinfo;

	if (type == RTM_DELACTION) {
		return tcf_del_walker(idrinfo, skb, ops);
	} else if (type == RTM_GETACTION) {
		return tcf_dump_walker(idrinfo, skb, cb);
	} else {
		WARN(1, "tcf_generic_walker: unknown command %d\n", type);
		NL_SET_ERR_MSG(extack, "tcf_generic_walker: unknown command");
		return -EINVAL;
	}
}
EXPORT_SYMBOL(tcf_generic_walker);

int tcf_idr_search(struct tc_action_net *tn, struct tc_action **a, u32 index)
{
	struct tcf_idrinfo *idrinfo = tn->idrinfo;
	struct tc_action *p;

	mutex_lock(&idrinfo->lock);
	p = idr_find(&idrinfo->action_idr, index);
	if (IS_ERR(p))
		p = NULL;
	else if (p)
		refcount_inc(&p->tcfa_refcnt);
	mutex_unlock(&idrinfo->lock);

	if (p) {
		*a = p;
		return true;
	}
	return false;
}
EXPORT_SYMBOL(tcf_idr_search);

static int tcf_idr_delete_index(struct tcf_idrinfo *idrinfo, u32 index)
{
	struct tc_action *p;
	int ret = 0;

	mutex_lock(&idrinfo->lock);
	p = idr_find(&idrinfo->action_idr, index);
	if (!p) {
		mutex_unlock(&idrinfo->lock);
		return -ENOENT;
	}

	if (!atomic_read(&p->tcfa_bindcnt)) {
		if (refcount_dec_and_test(&p->tcfa_refcnt)) {
			struct module *owner = p->ops->owner;

			WARN_ON(p != idr_remove(&idrinfo->action_idr,
						p->tcfa_index));
			mutex_unlock(&idrinfo->lock);

			tcf_action_cleanup(p);
			module_put(owner);
			return 0;
		}
		ret = 0;
	} else {
		ret = -EPERM;
	}

	mutex_unlock(&idrinfo->lock);
	return ret;
}

int tcf_idr_create(struct tc_action_net *tn, u32 index, struct nlattr *est,
		   struct tc_action **a, const struct tc_action_ops *ops,
		   int bind, bool cpustats, u32 flags)
{
	struct tc_action *p = kzalloc(ops->size, GFP_KERNEL);
	struct tcf_idrinfo *idrinfo = tn->idrinfo;
	int err = -ENOMEM;

	if (unlikely(!p))
		return -ENOMEM;
	refcount_set(&p->tcfa_refcnt, 1);
	if (bind)
		atomic_set(&p->tcfa_bindcnt, 1);

	if (cpustats) {
		p->cpu_bstats = netdev_alloc_pcpu_stats(struct gnet_stats_basic_cpu);
		if (!p->cpu_bstats)
			goto err1;
		p->cpu_bstats_hw = netdev_alloc_pcpu_stats(struct gnet_stats_basic_cpu);
		if (!p->cpu_bstats_hw)
			goto err2;
		p->cpu_qstats = alloc_percpu(struct gnet_stats_queue);
		if (!p->cpu_qstats)
			goto err3;
	}
	spin_lock_init(&p->tcfa_lock);
	p->tcfa_index = index;
	p->tcfa_tm.install = jiffies;
	p->tcfa_tm.lastuse = jiffies;
	p->tcfa_tm.firstuse = 0;
	p->tcfa_flags = flags;
	if (est) {
		err = gen_new_estimator(&p->tcfa_bstats, p->cpu_bstats,
					&p->tcfa_rate_est,
					&p->tcfa_lock, NULL, est);
		if (err)
			goto err4;
	}

	p->idrinfo = idrinfo;
	__module_get(ops->owner);
	p->ops = ops;
	*a = p;
	return 0;
err4:
	free_percpu(p->cpu_qstats);
err3:
	free_percpu(p->cpu_bstats_hw);
err2:
	free_percpu(p->cpu_bstats);
err1:
	kfree(p);
	return err;
}
EXPORT_SYMBOL(tcf_idr_create);

int tcf_idr_create_from_flags(struct tc_action_net *tn, u32 index,
			      struct nlattr *est, struct tc_action **a,
			      const struct tc_action_ops *ops, int bind,
			      u32 flags)
{
	/* Set cpustats according to actions flags. */
	return tcf_idr_create(tn, index, est, a, ops, bind,
			      !(flags & TCA_ACT_FLAGS_NO_PERCPU_STATS), flags);
}
EXPORT_SYMBOL(tcf_idr_create_from_flags);

/* Cleanup idr index that was allocated but not initialized. */

void tcf_idr_cleanup(struct tc_action_net *tn, u32 index)
{
	struct tcf_idrinfo *idrinfo = tn->idrinfo;

	mutex_lock(&idrinfo->lock);
	/* Remove ERR_PTR(-EBUSY) allocated by tcf_idr_check_alloc */
	WARN_ON(!IS_ERR(idr_remove(&idrinfo->action_idr, index)));
	mutex_unlock(&idrinfo->lock);
}
EXPORT_SYMBOL(tcf_idr_cleanup);

/* Check if action with specified index exists. If actions is found, increments
 * its reference and bind counters, and return 1. Otherwise insert temporary
 * error pointer (to prevent concurrent users from inserting actions with same
 * index) and return 0.
 */

int tcf_idr_check_alloc(struct tc_action_net *tn, u32 *index,
			struct tc_action **a, int bind)
{
	struct tcf_idrinfo *idrinfo = tn->idrinfo;
	struct tc_action *p;
	int ret;

again:
	mutex_lock(&idrinfo->lock);
	if (*index) {
		p = idr_find(&idrinfo->action_idr, *index);
		if (IS_ERR(p)) {
			/* This means that another process allocated
			 * index but did not assign the pointer yet.
			 */
			mutex_unlock(&idrinfo->lock);
			goto again;
		}

		if (p) {
			refcount_inc(&p->tcfa_refcnt);
			if (bind)
				atomic_inc(&p->tcfa_bindcnt);
			*a = p;
			ret = 1;
		} else {
			*a = NULL;
			ret = idr_alloc_u32(&idrinfo->action_idr, NULL, index,
					    *index, GFP_KERNEL);
			if (!ret)
				idr_replace(&idrinfo->action_idr,
					    ERR_PTR(-EBUSY), *index);
		}
	} else {
		*index = 1;
		*a = NULL;
		ret = idr_alloc_u32(&idrinfo->action_idr, NULL, index,
				    UINT_MAX, GFP_KERNEL);
		if (!ret)
			idr_replace(&idrinfo->action_idr, ERR_PTR(-EBUSY),
				    *index);
	}
	mutex_unlock(&idrinfo->lock);
	return ret;
}
EXPORT_SYMBOL(tcf_idr_check_alloc);

void tcf_idrinfo_destroy(const struct tc_action_ops *ops,
			 struct tcf_idrinfo *idrinfo)
{
	struct idr *idr = &idrinfo->action_idr;
	struct tc_action *p;
	int ret;
	unsigned long id = 1;
	unsigned long tmp;

	idr_for_each_entry_ul(idr, p, tmp, id) {
		ret = __tcf_idr_release(p, false, true);
		if (ret == ACT_P_DELETED)
			module_put(ops->owner);
		else if (ret < 0)
			return;
	}
	idr_destroy(&idrinfo->action_idr);
}
EXPORT_SYMBOL(tcf_idrinfo_destroy);

static LIST_HEAD(act_base);
static DEFINE_RWLOCK(act_mod_lock);

int tcf_register_action(struct tc_action_ops *act,
			struct pernet_operations *ops)
{
	struct tc_action_ops *a;
	int ret;

	if (!act->act || !act->dump || !act->init || !act->walk || !act->lookup)
		return -EINVAL;

	/* We have to register pernet ops before making the action ops visible,
	 * otherwise tcf_action_init_1() could get a partially initialized
	 * netns.
	 */
	ret = register_pernet_subsys(ops);
	if (ret)
		return ret;

	write_lock(&act_mod_lock);
	list_for_each_entry(a, &act_base, head) {
		if (act->id == a->id || (strcmp(act->kind, a->kind) == 0)) {
			write_unlock(&act_mod_lock);
			unregister_pernet_subsys(ops);
			return -EEXIST;
		}
	}
	list_add_tail(&act->head, &act_base);
	write_unlock(&act_mod_lock);

	return 0;
}
EXPORT_SYMBOL(tcf_register_action);

int tcf_unregister_action(struct tc_action_ops *act,
			  struct pernet_operations *ops)
{
	struct tc_action_ops *a;
	int err = -ENOENT;

	write_lock(&act_mod_lock);
	list_for_each_entry(a, &act_base, head) {
		if (a == act) {
			list_del(&act->head);
			err = 0;
			break;
		}
	}
	write_unlock(&act_mod_lock);
	if (!err)
		unregister_pernet_subsys(ops);
	return err;
}
EXPORT_SYMBOL(tcf_unregister_action);

/* lookup by name */
static struct tc_action_ops *tc_lookup_action_n(char *kind)
{
	struct tc_action_ops *a, *res = NULL;

	if (kind) {
		read_lock(&act_mod_lock);
		list_for_each_entry(a, &act_base, head) {
			if (strcmp(kind, a->kind) == 0) {
				if (try_module_get(a->owner))
					res = a;
				break;
			}
		}
		read_unlock(&act_mod_lock);
	}
	return res;
}

/* lookup by nlattr */
static struct tc_action_ops *tc_lookup_action(struct nlattr *kind)
{
	struct tc_action_ops *a, *res = NULL;

	if (kind) {
		read_lock(&act_mod_lock);
		list_for_each_entry(a, &act_base, head) {
			if (nla_strcmp(kind, a->kind) == 0) {
				if (try_module_get(a->owner))
					res = a;
				break;
			}
		}
		read_unlock(&act_mod_lock);
	}
	return res;
}

/*TCA_ACT_MAX_PRIO is 32, there count up to 32 */
#define TCA_ACT_MAX_PRIO_MASK 0x1FF
int tcf_action_exec(struct sk_buff *skb, struct tc_action **actions,
		    int nr_actions, struct tcf_result *res)
{
	u32 jmp_prgcnt = 0;
	u32 jmp_ttl = TCA_ACT_MAX_PRIO; /*matches actions per filter */
	int i;
	int ret = TC_ACT_OK;

	if (skb_skip_tc_classify(skb))
		return TC_ACT_OK;

restart_act_graph:
	for (i = 0; i < nr_actions; i++) {
		const struct tc_action *a = actions[i];

		if (jmp_prgcnt > 0) {
			jmp_prgcnt -= 1;
			continue;
		}
repeat:
		ret = a->ops->act(skb, a, res);
		if (ret == TC_ACT_REPEAT)
			goto repeat;	/* we need a ttl - JHS */

		if (TC_ACT_EXT_CMP(ret, TC_ACT_JUMP)) {
			jmp_prgcnt = ret & TCA_ACT_MAX_PRIO_MASK;
			if (!jmp_prgcnt || (jmp_prgcnt > nr_actions)) {
				/* faulty opcode, stop pipeline */
				return TC_ACT_OK;
			} else {
				jmp_ttl -= 1;
				if (jmp_ttl > 0)
					goto restart_act_graph;
				else /* faulty graph, stop pipeline */
					return TC_ACT_OK;
			}
		} else if (TC_ACT_EXT_CMP(ret, TC_ACT_GOTO_CHAIN)) {
			if (unlikely(!rcu_access_pointer(a->goto_chain))) {
				net_warn_ratelimited("can't go to NULL chain!\n");
				return TC_ACT_SHOT;
			}
			tcf_action_goto_chain_exec(a, res);
		}

		if (ret != TC_ACT_PIPE)
			break;
	}

	return ret;
}
EXPORT_SYMBOL(tcf_action_exec);

int tcf_action_destroy(struct tc_action *actions[], int bind)
{
	const struct tc_action_ops *ops;
	struct tc_action *a;
	int ret = 0, i;

	for (i = 0; i < TCA_ACT_MAX_PRIO && actions[i]; i++) {
		a = actions[i];
		actions[i] = NULL;
		ops = a->ops;
		ret = __tcf_idr_release(a, bind, true);
		if (ret == ACT_P_DELETED)
			module_put(ops->owner);
		else if (ret < 0)
			return ret;
	}
	return ret;
}

static int tcf_action_put(struct tc_action *p)
{
	return __tcf_action_put(p, false);
}

/* Put all actions in this array, skip those NULL's. */
static void tcf_action_put_many(struct tc_action *actions[])
{
	int i;

	for (i = 0; i < TCA_ACT_MAX_PRIO; i++) {
		struct tc_action *a = actions[i];
		const struct tc_action_ops *ops;

		if (!a)
			continue;
		ops = a->ops;
		if (tcf_action_put(a))
			module_put(ops->owner);
	}
}

int
tcf_action_dump_old(struct sk_buff *skb, struct tc_action *a, int bind, int ref)
{
	return a->ops->dump(skb, a, bind, ref);
}

int
tcf_action_dump_1(struct sk_buff *skb, struct tc_action *a, int bind, int ref)
{
	int err = -EINVAL;
	unsigned char *b = skb_tail_pointer(skb);
	struct nlattr *nest;

	if (tcf_action_dump_terse(skb, a, false))
		goto nla_put_failure;

	if (a->hw_stats != TCA_ACT_HW_STATS_ANY &&
	    nla_put_bitfield32(skb, TCA_ACT_HW_STATS,
			       a->hw_stats, TCA_ACT_HW_STATS_ANY))
		goto nla_put_failure;

	if (a->used_hw_stats_valid &&
	    nla_put_bitfield32(skb, TCA_ACT_USED_HW_STATS,
			       a->used_hw_stats, TCA_ACT_HW_STATS_ANY))
		goto nla_put_failure;

	if (a->tcfa_flags &&
	    nla_put_bitfield32(skb, TCA_ACT_FLAGS,
			       a->tcfa_flags, a->tcfa_flags))
		goto nla_put_failure;

	nest = nla_nest_start_noflag(skb, TCA_OPTIONS);
	if (nest == NULL)
		goto nla_put_failure;
	err = tcf_action_dump_old(skb, a, bind, ref);
	if (err > 0) {
		nla_nest_end(skb, nest);
		return err;
	}

nla_put_failure:
	nlmsg_trim(skb, b);
	return -1;
}
EXPORT_SYMBOL(tcf_action_dump_1);

int tcf_action_dump(struct sk_buff *skb, struct tc_action *actions[],
		    int bind, int ref, bool terse)
{
	struct tc_action *a;
	int err = -EINVAL, i;
	struct nlattr *nest;

	for (i = 0; i < TCA_ACT_MAX_PRIO && actions[i]; i++) {
		a = actions[i];
		nest = nla_nest_start_noflag(skb, i + 1);
		if (nest == NULL)
			goto nla_put_failure;
		err = terse ? tcf_action_dump_terse(skb, a, false) :
			tcf_action_dump_1(skb, a, bind, ref);
		if (err < 0)
			goto errout;
		nla_nest_end(skb, nest);
	}

	return 0;

nla_put_failure:
	err = -EINVAL;
errout:
	nla_nest_cancel(skb, nest);
	return err;
}

static struct tc_cookie *nla_memdup_cookie(struct nlattr **tb)
{
	struct tc_cookie *c = kzalloc(sizeof(*c), GFP_KERNEL);
	if (!c)
		return NULL;

	c->data = nla_memdup(tb[TCA_ACT_COOKIE], GFP_KERNEL);
	if (!c->data) {
		kfree(c);
		return NULL;
	}
	c->len = nla_len(tb[TCA_ACT_COOKIE]);

	return c;
}

static u8 tcf_action_hw_stats_get(struct nlattr *hw_stats_attr)
{
	struct nla_bitfield32 hw_stats_bf;

	/* If the user did not pass the attr, that means he does
	 * not care about the type. Return "any" in that case
	 * which is setting on all supported types.
	 */
	if (!hw_stats_attr)
		return TCA_ACT_HW_STATS_ANY;
	hw_stats_bf = nla_get_bitfield32(hw_stats_attr);
	return hw_stats_bf.value;
}

static const struct nla_policy tcf_action_policy[TCA_ACT_MAX + 1] = {
	[TCA_ACT_KIND]		= { .type = NLA_STRING },
	[TCA_ACT_INDEX]		= { .type = NLA_U32 },
	[TCA_ACT_COOKIE]	= { .type = NLA_BINARY,
				    .len = TC_COOKIE_MAX_SIZE },
	[TCA_ACT_OPTIONS]	= { .type = NLA_NESTED },
	[TCA_ACT_FLAGS]		= NLA_POLICY_BITFIELD32(TCA_ACT_FLAGS_NO_PERCPU_STATS),
	[TCA_ACT_HW_STATS]	= NLA_POLICY_BITFIELD32(TCA_ACT_HW_STATS_ANY),
};

void tcf_idr_insert_many(struct tc_action *actions[])
{
	int i;

	for (i = 0; i < TCA_ACT_MAX_PRIO; i++) {
		struct tc_action *a = actions[i];
		struct tcf_idrinfo *idrinfo;

		if (!a)
			continue;
		idrinfo = a->idrinfo;
		mutex_lock(&idrinfo->lock);
		/* Replace ERR_PTR(-EBUSY) allocated by tcf_idr_check_alloc if
		 * it is just created, otherwise this is just a nop.
		 */
		idr_replace(&idrinfo->action_idr, a, a->tcfa_index);
		mutex_unlock(&idrinfo->lock);
	}
}

struct tc_action_ops *tc_action_load_ops(char *name, struct nlattr *nla,
					 bool rtnl_held,
					 struct netlink_ext_ack *extack)
{
	struct nlattr *tb[TCA_ACT_MAX + 1];
	struct tc_action_ops *a_o;
	char act_name[IFNAMSIZ];
	struct nlattr *kind;
	int err;

	if (name == NULL) {
		err = nla_parse_nested_deprecated(tb, TCA_ACT_MAX, nla,
						  tcf_action_policy, extack);
		if (err < 0)
			return ERR_PTR(err);
		err = -EINVAL;
		kind = tb[TCA_ACT_KIND];
		if (!kind) {
			NL_SET_ERR_MSG(extack, "TC action kind must be specified");
			return ERR_PTR(err);
		}
		if (nla_strscpy(act_name, kind, IFNAMSIZ) < 0) {
			NL_SET_ERR_MSG(extack, "TC action name too long");
			return ERR_PTR(err);
		}
	} else {
		if (strlcpy(act_name, name, IFNAMSIZ) >= IFNAMSIZ) {
			NL_SET_ERR_MSG(extack, "TC action name too long");
			return ERR_PTR(-EINVAL);
		}
	}

	a_o = tc_lookup_action_n(act_name);
	if (a_o == NULL) {
#ifdef CONFIG_MODULES
		if (rtnl_held)
			rtnl_unlock();
		request_module("act_%s", act_name);
		if (rtnl_held)
			rtnl_lock();

		a_o = tc_lookup_action_n(act_name);

		/* We dropped the RTNL semaphore in order to
		 * perform the module load.  So, even if we
		 * succeeded in loading the module we have to
		 * tell the caller to replay the request.  We
		 * indicate this using -EAGAIN.
		 */
		if (a_o != NULL) {
			module_put(a_o->owner);
			return ERR_PTR(-EAGAIN);
		}
#endif
		NL_SET_ERR_MSG(extack, "Failed to load TC action module");
		return ERR_PTR(-ENOENT);
	}

	return a_o;
}

struct tc_action *tcf_action_init_1(struct net *net, struct tcf_proto *tp,
				    struct nlattr *nla, struct nlattr *est,
				    char *name, int ovr, int bind,
				    struct tc_action_ops *a_o, int *init_res,
				    bool rtnl_held,
				    struct netlink_ext_ack *extack)
{
	struct nla_bitfield32 flags = { 0, 0 };
	u8 hw_stats = TCA_ACT_HW_STATS_ANY;
	struct nlattr *tb[TCA_ACT_MAX + 1];
	struct tc_cookie *cookie = NULL;
	struct tc_action *a;
	int err;

	/* backward compatibility for policer */
	if (name == NULL) {
		err = nla_parse_nested_deprecated(tb, TCA_ACT_MAX, nla,
						  tcf_action_policy, extack);
		if (err < 0)
			return ERR_PTR(err);
		if (tb[TCA_ACT_COOKIE]) {
			cookie = nla_memdup_cookie(tb);
			if (!cookie) {
				NL_SET_ERR_MSG(extack, "No memory to generate TC cookie");
				err = -ENOMEM;
				goto err_out;
			}
		}
		hw_stats = tcf_action_hw_stats_get(tb[TCA_ACT_HW_STATS]);
		if (tb[TCA_ACT_FLAGS])
			flags = nla_get_bitfield32(tb[TCA_ACT_FLAGS]);

		err = a_o->init(net, tb[TCA_ACT_OPTIONS], est, &a, ovr, bind,
				rtnl_held, tp, flags.value, extack);
	} else {
		err = a_o->init(net, nla, est, &a, ovr, bind, rtnl_held,
				tp, flags.value, extack);
	}
	if (err < 0)
		goto err_out;
	*init_res = err;

	if (!name && tb[TCA_ACT_COOKIE])
		tcf_set_action_cookie(&a->act_cookie, cookie);

	if (!name)
		a->hw_stats = hw_stats;

	return a;

err_out:
	if (cookie) {
		kfree(cookie->data);
		kfree(cookie);
	}
	return ERR_PTR(err);
}

/* Returns numbers of initialized actions or negative error. */

int tcf_action_init(struct net *net, struct tcf_proto *tp, struct nlattr *nla,
		    struct nlattr *est, char *name, int ovr, int bind,
		    struct tc_action *actions[], int init_res[], size_t *attr_size,
		    bool rtnl_held, struct netlink_ext_ack *extack)
{
	struct tc_action_ops *ops[TCA_ACT_MAX_PRIO] = {};
	struct nlattr *tb[TCA_ACT_MAX_PRIO + 1];
	struct tc_action *act;
	size_t sz = 0;
	int err;
	int i;

	err = nla_parse_nested_deprecated(tb, TCA_ACT_MAX_PRIO, nla, NULL,
					  extack);
	if (err < 0)
		return err;

	for (i = 1; i <= TCA_ACT_MAX_PRIO && tb[i]; i++) {
		struct tc_action_ops *a_o;

		a_o = tc_action_load_ops(name, tb[i], rtnl_held, extack);
		if (IS_ERR(a_o)) {
			err = PTR_ERR(a_o);
			goto err_mod;
		}
		ops[i - 1] = a_o;
	}

	for (i = 1; i <= TCA_ACT_MAX_PRIO && tb[i]; i++) {
		act = tcf_action_init_1(net, tp, tb[i], est, name, ovr, bind,
					ops[i - 1], &init_res[i - 1], rtnl_held,
					extack);
		if (IS_ERR(act)) {
			err = PTR_ERR(act);
			goto err;
		}
		sz += tcf_action_fill_size(act);
		/* Start from index 0 */
		actions[i - 1] = act;
	}

	/* We have to commit them all together, because if any error happened in
	 * between, we could not handle the failure gracefully.
	 */
	tcf_idr_insert_many(actions);

	*attr_size = tcf_action_full_attrs_size(sz);
	err = i - 1;
	goto err_mod;

err:
	tcf_action_destroy(actions, bind);
err_mod:
	for (i = 0; i < TCA_ACT_MAX_PRIO; i++) {
		if (ops[i])
			module_put(ops[i]->owner);
	}
	return err;
}

void tcf_action_update_stats(struct tc_action *a, u64 bytes, u64 packets,
			     u64 drops, bool hw)
{
	if (a->cpu_bstats) {
		_bstats_cpu_update(this_cpu_ptr(a->cpu_bstats), bytes, packets);

		this_cpu_ptr(a->cpu_qstats)->drops += drops;

		if (hw)
			_bstats_cpu_update(this_cpu_ptr(a->cpu_bstats_hw),
					   bytes, packets);
		return;
	}

	_bstats_update(&a->tcfa_bstats, bytes, packets);
	a->tcfa_qstats.drops += drops;
	if (hw)
		_bstats_update(&a->tcfa_bstats_hw, bytes, packets);
}
EXPORT_SYMBOL(tcf_action_update_stats);

int tcf_action_copy_stats(struct sk_buff *skb, struct tc_action *p,
			  int compat_mode)
{
	int err = 0;
	struct gnet_dump d;

	if (p == NULL)
		goto errout;

	/* compat_mode being true specifies a call that is supposed
	 * to add additional backward compatibility statistic TLVs.
	 */
	if (compat_mode) {
		if (p->type == TCA_OLD_COMPAT)
			err = gnet_stats_start_copy_compat(skb, 0,
							   TCA_STATS,
							   TCA_XSTATS,
							   &p->tcfa_lock, &d,
							   TCA_PAD);
		else
			return 0;
	} else
		err = gnet_stats_start_copy(skb, TCA_ACT_STATS,
					    &p->tcfa_lock, &d, TCA_ACT_PAD);

	if (err < 0)
		goto errout;

	if (gnet_stats_copy_basic(NULL, &d, p->cpu_bstats, &p->tcfa_bstats) < 0 ||
	    gnet_stats_copy_basic_hw(NULL, &d, p->cpu_bstats_hw,
				     &p->tcfa_bstats_hw) < 0 ||
	    gnet_stats_copy_rate_est(&d, &p->tcfa_rate_est) < 0 ||
	    gnet_stats_copy_queue(&d, p->cpu_qstats,
				  &p->tcfa_qstats,
				  p->tcfa_qstats.qlen) < 0)
		goto errout;

	if (gnet_stats_finish_copy(&d) < 0)
		goto errout;

	return 0;

errout:
	return -1;
}

static int tca_get_fill(struct sk_buff *skb, struct tc_action *actions[],
			u32 portid, u32 seq, u16 flags, int event, int bind,
			int ref)
{
	struct tcamsg *t;
	struct nlmsghdr *nlh;
	unsigned char *b = skb_tail_pointer(skb);
	struct nlattr *nest;

	nlh = nlmsg_put(skb, portid, seq, event, sizeof(*t), flags);
	if (!nlh)
		goto out_nlmsg_trim;
	t = nlmsg_data(nlh);
	t->tca_family = AF_UNSPEC;
	t->tca__pad1 = 0;
	t->tca__pad2 = 0;

	nest = nla_nest_start_noflag(skb, TCA_ACT_TAB);
	if (!nest)
		goto out_nlmsg_trim;

	if (tcf_action_dump(skb, actions, bind, ref, false) < 0)
		goto out_nlmsg_trim;

	nla_nest_end(skb, nest);

	nlh->nlmsg_len = skb_tail_pointer(skb) - b;
	return skb->len;

out_nlmsg_trim:
	nlmsg_trim(skb, b);
	return -1;
}

static int
tcf_get_notify(struct net *net, u32 portid, struct nlmsghdr *n,
	       struct tc_action *actions[], int event,
	       struct netlink_ext_ack *extack)
{
	struct sk_buff *skb;

	skb = alloc_skb(NLMSG_GOODSIZE, GFP_KERNEL);
	if (!skb)
		return -ENOBUFS;
	if (tca_get_fill(skb, actions, portid, n->nlmsg_seq, 0, event,
			 0, 1) <= 0) {
		NL_SET_ERR_MSG(extack, "Failed to fill netlink attributes while adding TC action");
		kfree_skb(skb);
		return -EINVAL;
	}

	return rtnl_unicast(skb, net, portid);
}

static struct tc_action *tcf_action_get_1(struct net *net, struct nlattr *nla,
					  struct nlmsghdr *n, u32 portid,
					  struct netlink_ext_ack *extack)
{
	struct nlattr *tb[TCA_ACT_MAX + 1];
	const struct tc_action_ops *ops;
	struct tc_action *a;
	int index;
	int err;

	err = nla_parse_nested_deprecated(tb, TCA_ACT_MAX, nla,
					  tcf_action_policy, extack);
	if (err < 0)
		goto err_out;

	err = -EINVAL;
	if (tb[TCA_ACT_INDEX] == NULL ||
	    nla_len(tb[TCA_ACT_INDEX]) < sizeof(index)) {
		NL_SET_ERR_MSG(extack, "Invalid TC action index value");
		goto err_out;
	}
	index = nla_get_u32(tb[TCA_ACT_INDEX]);

	err = -EINVAL;
	ops = tc_lookup_action(tb[TCA_ACT_KIND]);
	if (!ops) { /* could happen in batch of actions */
		NL_SET_ERR_MSG(extack, "Specified TC action kind not found");
		goto err_out;
	}
	err = -ENOENT;
	if (ops->lookup(net, &a, index) == 0) {
		NL_SET_ERR_MSG(extack, "TC action with specified index not found");
		goto err_mod;
	}

	module_put(ops->owner);
	return a;

err_mod:
	module_put(ops->owner);
err_out:
	return ERR_PTR(err);
}

static int tca_action_flush(struct net *net, struct nlattr *nla,
			    struct nlmsghdr *n, u32 portid,
			    struct netlink_ext_ack *extack)
{
	struct sk_buff *skb;
	unsigned char *b;
	struct nlmsghdr *nlh;
	struct tcamsg *t;
	struct netlink_callback dcb;
	struct nlattr *nest;
	struct nlattr *tb[TCA_ACT_MAX + 1];
	const struct tc_action_ops *ops;
	struct nlattr *kind;
	int err = -ENOMEM;

	skb = alloc_skb(NLMSG_GOODSIZE, GFP_KERNEL);
	if (!skb)
		return err;

	b = skb_tail_pointer(skb);

	err = nla_parse_nested_deprecated(tb, TCA_ACT_MAX, nla,
					  tcf_action_policy, extack);
	if (err < 0)
		goto err_out;

	err = -EINVAL;
	kind = tb[TCA_ACT_KIND];
	ops = tc_lookup_action(kind);
	if (!ops) { /*some idjot trying to flush unknown action */
		NL_SET_ERR_MSG(extack, "Cannot flush unknown TC action");
		goto err_out;
	}

	nlh = nlmsg_put(skb, portid, n->nlmsg_seq, RTM_DELACTION,
			sizeof(*t), 0);
	if (!nlh) {
		NL_SET_ERR_MSG(extack, "Failed to create TC action flush notification");
		goto out_module_put;
	}
	t = nlmsg_data(nlh);
	t->tca_family = AF_UNSPEC;
	t->tca__pad1 = 0;
	t->tca__pad2 = 0;

	nest = nla_nest_start_noflag(skb, TCA_ACT_TAB);
	if (!nest) {
		NL_SET_ERR_MSG(extack, "Failed to add new netlink message");
		goto out_module_put;
	}

	err = ops->walk(net, skb, &dcb, RTM_DELACTION, ops, extack);
	if (err <= 0) {
		nla_nest_cancel(skb, nest);
		goto out_module_put;
	}

	nla_nest_end(skb, nest);

	nlh->nlmsg_len = skb_tail_pointer(skb) - b;
	nlh->nlmsg_flags |= NLM_F_ROOT;
	module_put(ops->owner);
	err = rtnetlink_send(skb, net, portid, RTNLGRP_TC,
			     n->nlmsg_flags & NLM_F_ECHO);
	if (err > 0)
		return 0;
	if (err < 0)
		NL_SET_ERR_MSG(extack, "Failed to send TC action flush notification");

	return err;

out_module_put:
	module_put(ops->owner);
err_out:
	kfree_skb(skb);
	return err;
}

static int tcf_action_delete(struct net *net, struct tc_action *actions[])
{
	int i;

	for (i = 0; i < TCA_ACT_MAX_PRIO && actions[i]; i++) {
		struct tc_action *a = actions[i];
		const struct tc_action_ops *ops = a->ops;
		/* Actions can be deleted concurrently so we must save their
		 * type and id to search again after reference is released.
		 */
		struct tcf_idrinfo *idrinfo = a->idrinfo;
		u32 act_index = a->tcfa_index;

		actions[i] = NULL;
		if (tcf_action_put(a)) {
			/* last reference, action was deleted concurrently */
			module_put(ops->owner);
		} else  {
			int ret;

			/* now do the delete */
			ret = tcf_idr_delete_index(idrinfo, act_index);
			if (ret < 0)
				return ret;
		}
	}
	return 0;
}

static int
tcf_del_notify(struct net *net, struct nlmsghdr *n, struct tc_action *actions[],
	       u32 portid, size_t attr_size, struct netlink_ext_ack *extack)
{
	int ret;
	struct sk_buff *skb;

	skb = alloc_skb(attr_size <= NLMSG_GOODSIZE ? NLMSG_GOODSIZE : attr_size,
			GFP_KERNEL);
	if (!skb)
		return -ENOBUFS;

	if (tca_get_fill(skb, actions, portid, n->nlmsg_seq, 0, RTM_DELACTION,
			 0, 2) <= 0) {
		NL_SET_ERR_MSG(extack, "Failed to fill netlink TC action attributes");
		kfree_skb(skb);
		return -EINVAL;
	}

	/* now do the delete */
	ret = tcf_action_delete(net, actions);
	if (ret < 0) {
		NL_SET_ERR_MSG(extack, "Failed to delete TC action");
		kfree_skb(skb);
		return ret;
	}

	ret = rtnetlink_send(skb, net, portid, RTNLGRP_TC,
			     n->nlmsg_flags & NLM_F_ECHO);
	if (ret > 0)
		return 0;
	return ret;
}

static int
tca_action_gd(struct net *net, struct nlattr *nla, struct nlmsghdr *n,
	      u32 portid, int event, struct netlink_ext_ack *extack)
{
	int i, ret;
	struct nlattr *tb[TCA_ACT_MAX_PRIO + 1];
	struct tc_action *act;
	size_t attr_size = 0;
	struct tc_action *actions[TCA_ACT_MAX_PRIO] = {};

	ret = nla_parse_nested_deprecated(tb, TCA_ACT_MAX_PRIO, nla, NULL,
					  extack);
	if (ret < 0)
		return ret;

	if (event == RTM_DELACTION && n->nlmsg_flags & NLM_F_ROOT) {
		if (tb[1])
			return tca_action_flush(net, tb[1], n, portid, extack);

		NL_SET_ERR_MSG(extack, "Invalid netlink attributes while flushing TC action");
		return -EINVAL;
	}

	for (i = 1; i <= TCA_ACT_MAX_PRIO && tb[i]; i++) {
		act = tcf_action_get_1(net, tb[i], n, portid, extack);
		if (IS_ERR(act)) {
			ret = PTR_ERR(act);
			goto err;
		}
		attr_size += tcf_action_fill_size(act);
		actions[i - 1] = act;
	}

	attr_size = tcf_action_full_attrs_size(attr_size);

	if (event == RTM_GETACTION)
		ret = tcf_get_notify(net, portid, n, actions, event, extack);
	else { /* delete */
		ret = tcf_del_notify(net, n, actions, portid, attr_size, extack);
		if (ret)
			goto err;
		return 0;
	}
err:
	tcf_action_put_many(actions);
	return ret;
}

static int
tcf_add_notify(struct net *net, struct nlmsghdr *n, struct tc_action *actions[],
	       u32 portid, size_t attr_size, struct netlink_ext_ack *extack)
{
	struct sk_buff *skb;
	int err = 0;

	skb = alloc_skb(attr_size <= NLMSG_GOODSIZE ? NLMSG_GOODSIZE : attr_size,
			GFP_KERNEL);
	if (!skb)
		return -ENOBUFS;

	if (tca_get_fill(skb, actions, portid, n->nlmsg_seq, n->nlmsg_flags,
			 RTM_NEWACTION, 0, 0) <= 0) {
		NL_SET_ERR_MSG(extack, "Failed to fill netlink attributes while adding TC action");
		kfree_skb(skb);
		return -EINVAL;
	}

	err = rtnetlink_send(skb, net, portid, RTNLGRP_TC,
			     n->nlmsg_flags & NLM_F_ECHO);
	if (err > 0)
		err = 0;
	return err;
}

static int tcf_action_add(struct net *net, struct nlattr *nla,
			  struct nlmsghdr *n, u32 portid, int ovr,
			  struct netlink_ext_ack *extack)
{
	size_t attr_size = 0;
	int loop, ret, i;
	struct tc_action *actions[TCA_ACT_MAX_PRIO] = {};
	int init_res[TCA_ACT_MAX_PRIO] = {};

	for (loop = 0; loop < 10; loop++) {
		ret = tcf_action_init(net, NULL, nla, NULL, NULL, ovr, 0,
				      actions, init_res, &attr_size, true, extack);
		if (ret != -EAGAIN)
			break;
	}

	if (ret < 0)
		return ret;
	ret = tcf_add_notify(net, n, actions, portid, attr_size, extack);

	/* only put existing actions */
	for (i = 0; i < TCA_ACT_MAX_PRIO; i++)
		if (init_res[i] == ACT_P_CREATED)
			actions[i] = NULL;
	tcf_action_put_many(actions);

	return ret;
}

static const struct nla_policy tcaa_policy[TCA_ROOT_MAX + 1] = {
	[TCA_ROOT_FLAGS] = NLA_POLICY_BITFIELD32(TCA_ACT_FLAG_LARGE_DUMP_ON |
						 TCA_ACT_FLAG_TERSE_DUMP),
	[TCA_ROOT_TIME_DELTA]      = { .type = NLA_U32 },
};

static int tc_ctl_action(struct sk_buff *skb, struct nlmsghdr *n,
			 struct netlink_ext_ack *extack)
{
	struct net *net = sock_net(skb->sk);
	struct nlattr *tca[TCA_ROOT_MAX + 1];
	u32 portid = NETLINK_CB(skb).portid;
	int ret = 0, ovr = 0;

	if ((n->nlmsg_type != RTM_GETACTION) &&
	    !netlink_capable(skb, CAP_NET_ADMIN))
		return -EPERM;

	ret = nlmsg_parse_deprecated(n, sizeof(struct tcamsg), tca,
				     TCA_ROOT_MAX, NULL, extack);
	if (ret < 0)
		return ret;

	if (tca[TCA_ACT_TAB] == NULL) {
		NL_SET_ERR_MSG(extack, "Netlink action attributes missing");
		return -EINVAL;
	}

	/* n->nlmsg_flags & NLM_F_CREATE */
	switch (n->nlmsg_type) {
	case RTM_NEWACTION:
		/* we are going to assume all other flags
		 * imply create only if it doesn't exist
		 * Note that CREATE | EXCL implies that
		 * but since we want avoid ambiguity (eg when flags
		 * is zero) then just set this
		 */
		if (n->nlmsg_flags & NLM_F_REPLACE)
			ovr = 1;
		ret = tcf_action_add(net, tca[TCA_ACT_TAB], n, portid, ovr,
				     extack);
		break;
	case RTM_DELACTION:
		ret = tca_action_gd(net, tca[TCA_ACT_TAB], n,
				    portid, RTM_DELACTION, extack);
		break;
	case RTM_GETACTION:
		ret = tca_action_gd(net, tca[TCA_ACT_TAB], n,
				    portid, RTM_GETACTION, extack);
		break;
	default:
		BUG();
	}

	return ret;
}

static struct nlattr *find_dump_kind(struct nlattr **nla)
{
	struct nlattr *tb1, *tb2[TCA_ACT_MAX + 1];
	struct nlattr *tb[TCA_ACT_MAX_PRIO + 1];
	struct nlattr *kind;

	tb1 = nla[TCA_ACT_TAB];
	if (tb1 == NULL)
		return NULL;

	if (nla_parse_deprecated(tb, TCA_ACT_MAX_PRIO, nla_data(tb1), NLMSG_ALIGN(nla_len(tb1)), NULL, NULL) < 0)
		return NULL;

	if (tb[1] == NULL)
		return NULL;
	if (nla_parse_nested_deprecated(tb2, TCA_ACT_MAX, tb[1], tcf_action_policy, NULL) < 0)
		return NULL;
	kind = tb2[TCA_ACT_KIND];

	return kind;
}

static int tc_dump_action(struct sk_buff *skb, struct netlink_callback *cb)
{
	struct net *net = sock_net(skb->sk);
	struct nlmsghdr *nlh;
	unsigned char *b = skb_tail_pointer(skb);
	struct nlattr *nest;
	struct tc_action_ops *a_o;
	int ret = 0;
	struct tcamsg *t = (struct tcamsg *) nlmsg_data(cb->nlh);
	struct nlattr *tb[TCA_ROOT_MAX + 1];
	struct nlattr *count_attr = NULL;
	unsigned long jiffy_since = 0;
	struct nlattr *kind = NULL;
	struct nla_bitfield32 bf;
	u32 msecs_since = 0;
	u32 act_count = 0;

	ret = nlmsg_parse_deprecated(cb->nlh, sizeof(struct tcamsg), tb,
				     TCA_ROOT_MAX, tcaa_policy, cb->extack);
	if (ret < 0)
		return ret;

	kind = find_dump_kind(tb);
	if (kind == NULL) {
		pr_info("tc_dump_action: action bad kind\n");
		return 0;
	}

	a_o = tc_lookup_action(kind);
	if (a_o == NULL)
		return 0;

	cb->args[2] = 0;
	if (tb[TCA_ROOT_FLAGS]) {
		bf = nla_get_bitfield32(tb[TCA_ROOT_FLAGS]);
		cb->args[2] = bf.value;
	}

	if (tb[TCA_ROOT_TIME_DELTA]) {
		msecs_since = nla_get_u32(tb[TCA_ROOT_TIME_DELTA]);
	}

	nlh = nlmsg_put(skb, NETLINK_CB(cb->skb).portid, cb->nlh->nlmsg_seq,
			cb->nlh->nlmsg_type, sizeof(*t), 0);
	if (!nlh)
		goto out_module_put;

	if (msecs_since)
		jiffy_since = jiffies - msecs_to_jiffies(msecs_since);

	t = nlmsg_data(nlh);
	t->tca_family = AF_UNSPEC;
	t->tca__pad1 = 0;
	t->tca__pad2 = 0;
	cb->args[3] = jiffy_since;
	count_attr = nla_reserve(skb, TCA_ROOT_COUNT, sizeof(u32));
	if (!count_attr)
		goto out_module_put;

	nest = nla_nest_start_noflag(skb, TCA_ACT_TAB);
	if (nest == NULL)
		goto out_module_put;

	ret = a_o->walk(net, skb, cb, RTM_GETACTION, a_o, NULL);
	if (ret < 0)
		goto out_module_put;

	if (ret > 0) {
		nla_nest_end(skb, nest);
		ret = skb->len;
		act_count = cb->args[1];
		memcpy(nla_data(count_attr), &act_count, sizeof(u32));
		cb->args[1] = 0;
	} else
		nlmsg_trim(skb, b);

	nlh->nlmsg_len = skb_tail_pointer(skb) - b;
	if (NETLINK_CB(cb->skb).portid && ret)
		nlh->nlmsg_flags |= NLM_F_MULTI;
	module_put(a_o->owner);
	return skb->len;

out_module_put:
	module_put(a_o->owner);
	nlmsg_trim(skb, b);
	return skb->len;
}

static int __init tc_action_init(void)
{
	rtnl_register(PF_UNSPEC, RTM_NEWACTION, tc_ctl_action, NULL, 0);
	rtnl_register(PF_UNSPEC, RTM_DELACTION, tc_ctl_action, NULL, 0);
	rtnl_register(PF_UNSPEC, RTM_GETACTION, tc_ctl_action, tc_dump_action,
		      0);

	return 0;
}

subsys_initcall(tc_action_init);
