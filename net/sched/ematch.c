// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * net/sched/ematch.c		Extended Match API
 *
 * Authors:	Thomas Graf <tgraf@suug.ch>
 *
 * ==========================================================================
 *
 * An extended match (ematch) is a small classification tool not worth
 * writing a full classifier for. Ematches can be interconnected to form
 * a logic expression and get attached to classifiers to extend their
 * functionatlity.
 *
 * The userspace part transforms the logic expressions into an array
 * consisting of multiple sequences of interconnected ematches separated
 * by markers. Precedence is implemented by a special ematch kind
 * referencing a sequence beyond the marker of the current sequence
 * causing the current position in the sequence to be pushed onto a stack
 * to allow the current position to be overwritten by the position referenced
 * in the special ematch. Matching continues in the new sequence until a
 * marker is reached causing the position to be restored from the stack.
 *
 * Example:
 *          A AND (B1 OR B2) AND C AND D
 *
 *              ------->-PUSH-------
 *    -->--    /         -->--      \   -->--
 *   /     \  /         /     \      \ /     \
 * +-------+-------+-------+-------+-------+--------+
 * | A AND | B AND | C AND | D END | B1 OR | B2 END |
 * +-------+-------+-------+-------+-------+--------+
 *                    \                      /
 *                     --------<-POP---------
 *
 * where B is a virtual ematch referencing to sequence starting with B1.
 *
 * ==========================================================================
 *
 * How to write an ematch in 60 seconds
 * ------------------------------------
 *
 *   1) Provide a matcher function:
 *      static int my_match(struct sk_buff *skb, struct tcf_ematch *m,
 *                          struct tcf_pkt_info *info)
 *      {
 *      	struct mydata *d = (struct mydata *) m->data;
 *
 *      	if (...matching goes here...)
 *      		return 1;
 *      	else
 *      		return 0;
 *      }
 *
 *   2) Fill out a struct tcf_ematch_ops:
 *      static struct tcf_ematch_ops my_ops = {
 *      	.kind = unique id,
 *      	.datalen = sizeof(struct mydata),
 *      	.match = my_match,
 *      	.owner = THIS_MODULE,
 *      };
 *
 *   3) Register/Unregister your ematch:
 *      static int __init init_my_ematch(void)
 *      {
 *      	return tcf_em_register(&my_ops);
 *      }
 *
 *      static void __exit exit_my_ematch(void)
 *      {
 *      	tcf_em_unregister(&my_ops);
 *      }
 *
 *      module_init(init_my_ematch);
 *      module_exit(exit_my_ematch);
 *
 *   4) By now you should have two more seconds left, barely enough to
 *      open up a beer to watch the compilation going.
 */

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/rtnetlink.h>
#include <linux/skbuff.h>
#include <net/pkt_cls.h>

static LIST_HEAD(ematch_ops);
static DEFINE_RWLOCK(ematch_mod_lock);

static struct tcf_ematch_ops *tcf_em_lookup(u16 kind)
{
	struct tcf_ematch_ops *e = NULL;

	read_lock(&ematch_mod_lock);
	list_for_each_entry(e, &ematch_ops, link) {
		if (kind == e->kind) {
			if (!try_module_get(e->owner))
				e = NULL;
			read_unlock(&ematch_mod_lock);
			return e;
		}
	}
	read_unlock(&ematch_mod_lock);

	return NULL;
}

/**
 * tcf_em_register - register an extended match
 *
 * @ops: ematch operations lookup table
 *
 * This function must be called by ematches to announce their presence.
 * The given @ops must have kind set to a unique identifier and the
 * callback match() must be implemented. All other callbacks are optional
 * and a fallback implementation is used instead.
 *
 * Returns -EEXISTS if an ematch of the same kind has already registered.
 */
int tcf_em_register(struct tcf_ematch_ops *ops)
{
	int err = -EEXIST;
	struct tcf_ematch_ops *e;

	if (ops->match == NULL)
		return -EINVAL;

	write_lock(&ematch_mod_lock);
	list_for_each_entry(e, &ematch_ops, link)
		if (ops->kind == e->kind)
			goto errout;

	list_add_tail(&ops->link, &ematch_ops);
	err = 0;
errout:
	write_unlock(&ematch_mod_lock);
	return err;
}
EXPORT_SYMBOL(tcf_em_register);

/**
 * tcf_em_unregister - unregster and extended match
 *
 * @ops: ematch operations lookup table
 *
 * This function must be called by ematches to announce their disappearance
 * for examples when the module gets unloaded. The @ops parameter must be
 * the same as the one used for registration.
 *
 * Returns -ENOENT if no matching ematch was found.
 */
void tcf_em_unregister(struct tcf_ematch_ops *ops)
{
	write_lock(&ematch_mod_lock);
	list_del(&ops->link);
	write_unlock(&ematch_mod_lock);
}
EXPORT_SYMBOL(tcf_em_unregister);

static inline struct tcf_ematch *tcf_em_get_match(struct tcf_ematch_tree *tree,
						  int index)
{
	return &tree->matches[index];
}


static int tcf_em_validate(struct tcf_proto *tp,
			   struct tcf_ematch_tree_hdr *tree_hdr,
			   struct tcf_ematch *em, struct nlattr *nla, int idx)
{
	int err = -EINVAL;
	struct tcf_ematch_hdr *em_hdr = nla_data(nla);
	int data_len = nla_len(nla) - sizeof(*em_hdr);
	void *data = (void *) em_hdr + sizeof(*em_hdr);
	struct net *net = tp->chain->block->net;

	if (!TCF_EM_REL_VALID(em_hdr->flags))
		goto errout;

	if (em_hdr->kind == TCF_EM_CONTAINER) {
		/* Special ematch called "container", carries an index
		 * referencing an external ematch sequence.
		 */
		u32 ref;

		if (data_len < sizeof(ref))
			goto errout;
		ref = *(u32 *) data;

		if (ref >= tree_hdr->nmatches)
			goto errout;

		/* We do not allow backward jumps to avoid loops and jumps
		 * to our own position are of course illegal.
		 */
		if (ref <= idx)
			goto errout;


		em->data = ref;
	} else {
		/* Note: This lookup will increase the module refcnt
		 * of the ematch module referenced. In case of a failure,
		 * a destroy function is called by the underlying layer
		 * which automatically releases the reference again, therefore
		 * the module MUST not be given back under any circumstances
		 * here. Be aware, the destroy function assumes that the
		 * module is held if the ops field is non zero.
		 */
		em->ops = tcf_em_lookup(em_hdr->kind);

		if (em->ops == NULL) {
			err = -ENOENT;
#ifdef CONFIG_MODULES
			__rtnl_unlock();
			request_module("ematch-kind-%u", em_hdr->kind);
			rtnl_lock();
			em->ops = tcf_em_lookup(em_hdr->kind);
			if (em->ops) {
				/* We dropped the RTNL mutex in order to
				 * perform the module load. Tell the caller
				 * to replay the request.
				 */
				module_put(em->ops->owner);
				em->ops = NULL;
				err = -EAGAIN;
			}
#endif
			goto errout;
		}

		/* ematch module provides expected length of data, so we
		 * can do a basic sanity check.
		 */
		if (em->ops->datalen && data_len < em->ops->datalen)
			goto errout;

		if (em->ops->change) {
			err = -EINVAL;
			if (em_hdr->flags & TCF_EM_SIMPLE)
				goto errout;
			err = em->ops->change(net, data, data_len, em);
			if (err < 0)
				goto errout;
		} else if (data_len > 0) {
			/* ematch module doesn't provide an own change
			 * procedure and expects us to allocate and copy
			 * the ematch data.
			 *
			 * TCF_EM_SIMPLE may be specified stating that the
			 * data only consists of a u32 integer and the module
			 * does not expected a memory reference but rather
			 * the value carried.
			 */
			if (em_hdr->flags & TCF_EM_SIMPLE) {
				if (data_len < sizeof(u32))
					goto errout;
				em->data = *(u32 *) data;
			} else {
				void *v = kmemdup(data, data_len, GFP_KERNEL);
				if (v == NULL) {
					err = -ENOBUFS;
					goto errout;
				}
				em->data = (unsigned long) v;
			}
			em->datalen = data_len;
		}
	}

	em->matchid = em_hdr->matchid;
	em->flags = em_hdr->flags;
	em->net = net;

	err = 0;
errout:
	return err;
}

static const struct nla_policy em_policy[TCA_EMATCH_TREE_MAX + 1] = {
	[TCA_EMATCH_TREE_HDR]	= { .len = sizeof(struct tcf_ematch_tree_hdr) },
	[TCA_EMATCH_TREE_LIST]	= { .type = NLA_NESTED },
};

/**
 * tcf_em_tree_validate - validate ematch config TLV and build ematch tree
 *
 * @tp: classifier kind handle
 * @nla: ematch tree configuration TLV
 * @tree: destination ematch tree variable to store the resulting
 *        ematch tree.
 *
 * This function validates the given configuration TLV @nla and builds an
 * ematch tree in @tree. The resulting tree must later be copied into
 * the private classifier data using tcf_em_tree_change(). You MUST NOT
 * provide the ematch tree variable of the private classifier data directly,
 * the changes would not be locked properly.
 *
 * Returns a negative error code if the configuration TLV contains errors.
 */
int tcf_em_tree_validate(struct tcf_proto *tp, struct nlattr *nla,
			 struct tcf_ematch_tree *tree)
{
	int idx, list_len, matches_len, err;
	struct nlattr *tb[TCA_EMATCH_TREE_MAX + 1];
	struct nlattr *rt_match, *rt_hdr, *rt_list;
	struct tcf_ematch_tree_hdr *tree_hdr;
	struct tcf_ematch *em;

	memset(tree, 0, sizeof(*tree));
	if (!nla)
		return 0;

	err = nla_parse_nested_deprecated(tb, TCA_EMATCH_TREE_MAX, nla,
					  em_policy, NULL);
	if (err < 0)
		goto errout;

	err = -EINVAL;
	rt_hdr = tb[TCA_EMATCH_TREE_HDR];
	rt_list = tb[TCA_EMATCH_TREE_LIST];

	if (rt_hdr == NULL || rt_list == NULL)
		goto errout;

	tree_hdr = nla_data(rt_hdr);
	memcpy(&tree->hdr, tree_hdr, sizeof(*tree_hdr));

	rt_match = nla_data(rt_list);
	list_len = nla_len(rt_list);
	matches_len = tree_hdr->nmatches * sizeof(*em);

	tree->matches = kzalloc(matches_len, GFP_KERNEL);
	if (tree->matches == NULL)
		goto errout;

	/* We do not use nla_parse_nested here because the maximum
	 * number of attributes is unknown. This saves us the allocation
	 * for a tb buffer which would serve no purpose at all.
	 *
	 * The array of rt attributes is parsed in the order as they are
	 * provided, their type must be incremental from 1 to n. Even
	 * if it does not serve any real purpose, a failure of sticking
	 * to this policy will result in parsing failure.
	 */
	for (idx = 0; nla_ok(rt_match, list_len); idx++) {
		err = -EINVAL;

		if (rt_match->nla_type != (idx + 1))
			goto errout_abort;

		if (idx >= tree_hdr->nmatches)
			goto errout_abort;

		if (nla_len(rt_match) < sizeof(struct tcf_ematch_hdr))
			goto errout_abort;

		em = tcf_em_get_match(tree, idx);

		err = tcf_em_validate(tp, tree_hdr, em, rt_match, idx);
		if (err < 0)
			goto errout_abort;

		rt_match = nla_next(rt_match, &list_len);
	}

	/* Check if the number of matches provided by userspace actually
	 * complies with the array of matches. The number was used for
	 * the validation of references and a mismatch could lead to
	 * undefined references during the matching process.
	 */
	if (idx != tree_hdr->nmatches) {
		err = -EINVAL;
		goto errout_abort;
	}

	err = 0;
errout:
	return err;

errout_abort:
	tcf_em_tree_destroy(tree);
	return err;
}
EXPORT_SYMBOL(tcf_em_tree_validate);

/**
 * tcf_em_tree_destroy - destroy an ematch tree
 *
 * @tree: ematch tree to be deleted
 *
 * This functions destroys an ematch tree previously created by
 * tcf_em_tree_validate()/tcf_em_tree_change(). You must ensure that
 * the ematch tree is not in use before calling this function.
 */
void tcf_em_tree_destroy(struct tcf_ematch_tree *tree)
{
	int i;

	if (tree->matches == NULL)
		return;

	for (i = 0; i < tree->hdr.nmatches; i++) {
		struct tcf_ematch *em = tcf_em_get_match(tree, i);

		if (em->ops) {
			if (em->ops->destroy)
				em->ops->destroy(em);
			else if (!tcf_em_is_simple(em))
				kfree((void *) em->data);
			module_put(em->ops->owner);
		}
	}

	tree->hdr.nmatches = 0;
	kfree(tree->matches);
	tree->matches = NULL;
}
EXPORT_SYMBOL(tcf_em_tree_destroy);

/**
 * tcf_em_tree_dump - dump ematch tree into a rtnl message
 *
 * @skb: skb holding the rtnl message
 * @tree: ematch tree to be dumped
 * @tlv: TLV type to be used to encapsulate the tree
 *
 * This function dumps a ematch tree into a rtnl message. It is valid to
 * call this function while the ematch tree is in use.
 *
 * Returns -1 if the skb tailroom is insufficient.
 */
int tcf_em_tree_dump(struct sk_buff *skb, struct tcf_ematch_tree *tree, int tlv)
{
	int i;
	u8 *tail;
	struct nlattr *top_start;
	struct nlattr *list_start;

	top_start = nla_nest_start_noflag(skb, tlv);
	if (top_start == NULL)
		goto nla_put_failure;

	if (nla_put(skb, TCA_EMATCH_TREE_HDR, sizeof(tree->hdr), &tree->hdr))
		goto nla_put_failure;

	list_start = nla_nest_start_noflag(skb, TCA_EMATCH_TREE_LIST);
	if (list_start == NULL)
		goto nla_put_failure;

	tail = skb_tail_pointer(skb);
	for (i = 0; i < tree->hdr.nmatches; i++) {
		struct nlattr *match_start = (struct nlattr *)tail;
		struct tcf_ematch *em = tcf_em_get_match(tree, i);
		struct tcf_ematch_hdr em_hdr = {
			.kind = em->ops ? em->ops->kind : TCF_EM_CONTAINER,
			.matchid = em->matchid,
			.flags = em->flags
		};

		if (nla_put(skb, i + 1, sizeof(em_hdr), &em_hdr))
			goto nla_put_failure;

		if (em->ops && em->ops->dump) {
			if (em->ops->dump(skb, em) < 0)
				goto nla_put_failure;
		} else if (tcf_em_is_container(em) || tcf_em_is_simple(em)) {
			u32 u = em->data;
			nla_put_nohdr(skb, sizeof(u), &u);
		} else if (em->datalen > 0)
			nla_put_nohdr(skb, em->datalen, (void *) em->data);

		tail = skb_tail_pointer(skb);
		match_start->nla_len = tail - (u8 *)match_start;
	}

	nla_nest_end(skb, list_start);
	nla_nest_end(skb, top_start);

	return 0;

nla_put_failure:
	return -1;
}
EXPORT_SYMBOL(tcf_em_tree_dump);

static inline int tcf_em_match(struct sk_buff *skb, struct tcf_ematch *em,
			       struct tcf_pkt_info *info)
{
	int r = em->ops->match(skb, em, info);

	return tcf_em_is_inverted(em) ? !r : r;
}

/* Do not use this function directly, use tcf_em_tree_match instead */
int __tcf_em_tree_match(struct sk_buff *skb, struct tcf_ematch_tree *tree,
			struct tcf_pkt_info *info)
{
	int stackp = 0, match_idx = 0, res = 0;
	struct tcf_ematch *cur_match;
	int stack[CONFIG_NET_EMATCH_STACK];

proceed:
	while (match_idx < tree->hdr.nmatches) {
		cur_match = tcf_em_get_match(tree, match_idx);

		if (tcf_em_is_container(cur_match)) {
			if (unlikely(stackp >= CONFIG_NET_EMATCH_STACK))
				goto stack_overflow;

			stack[stackp++] = match_idx;
			match_idx = cur_match->data;
			goto proceed;
		}

		res = tcf_em_match(skb, cur_match, info);

		if (tcf_em_early_end(cur_match, res))
			break;

		match_idx++;
	}

pop_stack:
	if (stackp > 0) {
		match_idx = stack[--stackp];
		cur_match = tcf_em_get_match(tree, match_idx);

		if (tcf_em_is_inverted(cur_match))
			res = !res;

		if (tcf_em_early_end(cur_match, res)) {
			goto pop_stack;
		} else {
			match_idx++;
			goto proceed;
		}
	}

	return res;

stack_overflow:
	net_warn_ratelimited("tc ematch: local stack overflow, increase NET_EMATCH_STACK\n");
	return -1;
}
EXPORT_SYMBOL(__tcf_em_tree_match);
