// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2008-2009 Patrick McHardy <kaber@trash.net>
 *
 * Development of this code funded by Astaro AG (http://www.astaro.com/)
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/list.h>
#include <linux/rbtree.h>
#include <linux/netlink.h>
#include <linux/netfilter.h>
#include <linux/netfilter/nf_tables.h>
#include <net/netfilter/nf_tables_core.h>

struct nft_rbtree {
	struct rb_root		root;
	rwlock_t		lock;
	seqcount_rwlock_t	count;
	struct delayed_work	gc_work;
};

struct nft_rbtree_elem {
	struct rb_node		node;
	struct nft_set_ext	ext;
};

static bool nft_rbtree_interval_end(const struct nft_rbtree_elem *rbe)
{
	return nft_set_ext_exists(&rbe->ext, NFT_SET_EXT_FLAGS) &&
	       (*nft_set_ext_flags(&rbe->ext) & NFT_SET_ELEM_INTERVAL_END);
}

static bool nft_rbtree_interval_start(const struct nft_rbtree_elem *rbe)
{
	return !nft_rbtree_interval_end(rbe);
}

static bool nft_rbtree_equal(const struct nft_set *set, const void *this,
			     const struct nft_rbtree_elem *interval)
{
	return memcmp(this, nft_set_ext_key(&interval->ext), set->klen) == 0;
}

static bool __nft_rbtree_lookup(const struct net *net, const struct nft_set *set,
				const u32 *key, const struct nft_set_ext **ext,
				unsigned int seq)
{
	struct nft_rbtree *priv = nft_set_priv(set);
	const struct nft_rbtree_elem *rbe, *interval = NULL;
	u8 genmask = nft_genmask_cur(net);
	const struct rb_node *parent;
	const void *this;
	int d;

	parent = rcu_dereference_raw(priv->root.rb_node);
	while (parent != NULL) {
		if (read_seqcount_retry(&priv->count, seq))
			return false;

		rbe = rb_entry(parent, struct nft_rbtree_elem, node);

		this = nft_set_ext_key(&rbe->ext);
		d = memcmp(this, key, set->klen);
		if (d < 0) {
			parent = rcu_dereference_raw(parent->rb_left);
			if (interval &&
			    nft_rbtree_equal(set, this, interval) &&
			    nft_rbtree_interval_end(rbe) &&
			    nft_rbtree_interval_start(interval))
				continue;
			interval = rbe;
		} else if (d > 0)
			parent = rcu_dereference_raw(parent->rb_right);
		else {
			if (!nft_set_elem_active(&rbe->ext, genmask)) {
				parent = rcu_dereference_raw(parent->rb_left);
				continue;
			}

			if (nft_set_elem_expired(&rbe->ext))
				return false;

			if (nft_rbtree_interval_end(rbe)) {
				if (nft_set_is_anonymous(set))
					return false;
				parent = rcu_dereference_raw(parent->rb_left);
				interval = NULL;
				continue;
			}

			*ext = &rbe->ext;
			return true;
		}
	}

	if (set->flags & NFT_SET_INTERVAL && interval != NULL &&
	    nft_set_elem_active(&interval->ext, genmask) &&
	    !nft_set_elem_expired(&interval->ext) &&
	    nft_rbtree_interval_start(interval)) {
		*ext = &interval->ext;
		return true;
	}

	return false;
}

static bool nft_rbtree_lookup(const struct net *net, const struct nft_set *set,
			      const u32 *key, const struct nft_set_ext **ext)
{
	struct nft_rbtree *priv = nft_set_priv(set);
	unsigned int seq = read_seqcount_begin(&priv->count);
	bool ret;

	ret = __nft_rbtree_lookup(net, set, key, ext, seq);
	if (ret || !read_seqcount_retry(&priv->count, seq))
		return ret;

	read_lock_bh(&priv->lock);
	seq = read_seqcount_begin(&priv->count);
	ret = __nft_rbtree_lookup(net, set, key, ext, seq);
	read_unlock_bh(&priv->lock);

	return ret;
}

static bool __nft_rbtree_get(const struct net *net, const struct nft_set *set,
			     const u32 *key, struct nft_rbtree_elem **elem,
			     unsigned int seq, unsigned int flags, u8 genmask)
{
	struct nft_rbtree_elem *rbe, *interval = NULL;
	struct nft_rbtree *priv = nft_set_priv(set);
	const struct rb_node *parent;
	const void *this;
	int d;

	parent = rcu_dereference_raw(priv->root.rb_node);
	while (parent != NULL) {
		if (read_seqcount_retry(&priv->count, seq))
			return false;

		rbe = rb_entry(parent, struct nft_rbtree_elem, node);

		this = nft_set_ext_key(&rbe->ext);
		d = memcmp(this, key, set->klen);
		if (d < 0) {
			parent = rcu_dereference_raw(parent->rb_left);
			if (!(flags & NFT_SET_ELEM_INTERVAL_END))
				interval = rbe;
		} else if (d > 0) {
			parent = rcu_dereference_raw(parent->rb_right);
			if (flags & NFT_SET_ELEM_INTERVAL_END)
				interval = rbe;
		} else {
			if (!nft_set_elem_active(&rbe->ext, genmask)) {
				parent = rcu_dereference_raw(parent->rb_left);
				continue;
			}

			if (nft_set_elem_expired(&rbe->ext))
				return false;

			if (!nft_set_ext_exists(&rbe->ext, NFT_SET_EXT_FLAGS) ||
			    (*nft_set_ext_flags(&rbe->ext) & NFT_SET_ELEM_INTERVAL_END) ==
			    (flags & NFT_SET_ELEM_INTERVAL_END)) {
				*elem = rbe;
				return true;
			}

			if (nft_rbtree_interval_end(rbe))
				interval = NULL;

			parent = rcu_dereference_raw(parent->rb_left);
		}
	}

	if (set->flags & NFT_SET_INTERVAL && interval != NULL &&
	    nft_set_elem_active(&interval->ext, genmask) &&
	    !nft_set_elem_expired(&interval->ext) &&
	    ((!nft_rbtree_interval_end(interval) &&
	      !(flags & NFT_SET_ELEM_INTERVAL_END)) ||
	     (nft_rbtree_interval_end(interval) &&
	      (flags & NFT_SET_ELEM_INTERVAL_END)))) {
		*elem = interval;
		return true;
	}

	return false;
}

static void *nft_rbtree_get(const struct net *net, const struct nft_set *set,
			    const struct nft_set_elem *elem, unsigned int flags)
{
	struct nft_rbtree *priv = nft_set_priv(set);
	unsigned int seq = read_seqcount_begin(&priv->count);
	struct nft_rbtree_elem *rbe = ERR_PTR(-ENOENT);
	const u32 *key = (const u32 *)&elem->key.val;
	u8 genmask = nft_genmask_cur(net);
	bool ret;

	ret = __nft_rbtree_get(net, set, key, &rbe, seq, flags, genmask);
	if (ret || !read_seqcount_retry(&priv->count, seq))
		return rbe;

	read_lock_bh(&priv->lock);
	seq = read_seqcount_begin(&priv->count);
	ret = __nft_rbtree_get(net, set, key, &rbe, seq, flags, genmask);
	if (!ret)
		rbe = ERR_PTR(-ENOENT);
	read_unlock_bh(&priv->lock);

	return rbe;
}

static int __nft_rbtree_insert(const struct net *net, const struct nft_set *set,
			       struct nft_rbtree_elem *new,
			       struct nft_set_ext **ext)
{
	bool overlap = false, dup_end_left = false, dup_end_right = false;
	struct nft_rbtree *priv = nft_set_priv(set);
	u8 genmask = nft_genmask_next(net);
	struct nft_rbtree_elem *rbe;
	struct rb_node *parent, **p;
	int d;

	/* Detect overlaps as we descend the tree. Set the flag in these cases:
	 *
	 * a1. _ _ __>|  ?_ _ __|  (insert end before existing end)
	 * a2. _ _ ___|  ?_ _ _>|  (insert end after existing end)
	 * a3. _ _ ___? >|_ _ __|  (insert start before existing end)
	 *
	 * and clear it later on, as we eventually reach the points indicated by
	 * '?' above, in the cases described below. We'll always meet these
	 * later, locally, due to tree ordering, and overlaps for the intervals
	 * that are the closest together are always evaluated last.
	 *
	 * b1. _ _ __>|  !_ _ __|  (insert end before existing start)
	 * b2. _ _ ___|  !_ _ _>|  (insert end after existing start)
	 * b3. _ _ ___! >|_ _ __|  (insert start after existing end, as a leaf)
	 *            '--' no nodes falling in this range
	 * b4.          >|_ _   !  (insert start before existing start)
	 *
	 * Case a3. resolves to b3.:
	 * - if the inserted start element is the leftmost, because the '0'
	 *   element in the tree serves as end element
	 * - otherwise, if an existing end is found immediately to the left. If
	 *   there are existing nodes in between, we need to further descend the
	 *   tree before we can conclude the new start isn't causing an overlap
	 *
	 * or to b4., which, preceded by a3., means we already traversed one or
	 * more existing intervals entirely, from the right.
	 *
	 * For a new, rightmost pair of elements, we'll hit cases b3. and b2.,
	 * in that order.
	 *
	 * The flag is also cleared in two special cases:
	 *
	 * b5. |__ _ _!|<_ _ _   (insert start right before existing end)
	 * b6. |__ _ >|!__ _ _   (insert end right after existing start)
	 *
	 * which always happen as last step and imply that no further
	 * overlapping is possible.
	 *
	 * Another special case comes from the fact that start elements matching
	 * an already existing start element are allowed: insertion is not
	 * performed but we return -EEXIST in that case, and the error will be
	 * cleared by the caller if NLM_F_EXCL is not present in the request.
	 * This way, request for insertion of an exact overlap isn't reported as
	 * error to userspace if not desired.
	 *
	 * However, if the existing start matches a pre-existing start, but the
	 * end element doesn't match the corresponding pre-existing end element,
	 * we need to report a partial overlap. This is a local condition that
	 * can be noticed without need for a tracking flag, by checking for a
	 * local duplicated end for a corresponding start, from left and right,
	 * separately.
	 */

	parent = NULL;
	p = &priv->root.rb_node;
	while (*p != NULL) {
		parent = *p;
		rbe = rb_entry(parent, struct nft_rbtree_elem, node);
		d = memcmp(nft_set_ext_key(&rbe->ext),
			   nft_set_ext_key(&new->ext),
			   set->klen);
		if (d < 0) {
			p = &parent->rb_left;

			if (nft_rbtree_interval_start(new)) {
				if (nft_rbtree_interval_end(rbe) &&
				    nft_set_elem_active(&rbe->ext, genmask) &&
				    !nft_set_elem_expired(&rbe->ext) && !*p)
					overlap = false;
			} else {
				if (dup_end_left && !*p)
					return -ENOTEMPTY;

				overlap = nft_rbtree_interval_end(rbe) &&
					  nft_set_elem_active(&rbe->ext,
							      genmask) &&
					  !nft_set_elem_expired(&rbe->ext);

				if (overlap) {
					dup_end_right = true;
					continue;
				}
			}
		} else if (d > 0) {
			p = &parent->rb_right;

			if (nft_rbtree_interval_end(new)) {
				if (dup_end_right && !*p)
					return -ENOTEMPTY;

				overlap = nft_rbtree_interval_end(rbe) &&
					  nft_set_elem_active(&rbe->ext,
							      genmask) &&
					  !nft_set_elem_expired(&rbe->ext);

				if (overlap) {
					dup_end_left = true;
					continue;
				}
			} else if (nft_set_elem_active(&rbe->ext, genmask) &&
				   !nft_set_elem_expired(&rbe->ext)) {
				overlap = nft_rbtree_interval_end(rbe);
			}
		} else {
			if (nft_rbtree_interval_end(rbe) &&
			    nft_rbtree_interval_start(new)) {
				p = &parent->rb_left;

				if (nft_set_elem_active(&rbe->ext, genmask) &&
				    !nft_set_elem_expired(&rbe->ext))
					overlap = false;
			} else if (nft_rbtree_interval_start(rbe) &&
				   nft_rbtree_interval_end(new)) {
				p = &parent->rb_right;

				if (nft_set_elem_active(&rbe->ext, genmask) &&
				    !nft_set_elem_expired(&rbe->ext))
					overlap = false;
			} else if (nft_set_elem_active(&rbe->ext, genmask) &&
				   !nft_set_elem_expired(&rbe->ext)) {
				*ext = &rbe->ext;
				return -EEXIST;
			} else {
				p = &parent->rb_left;
			}
		}

		dup_end_left = dup_end_right = false;
	}

	if (overlap)
		return -ENOTEMPTY;

	rb_link_node_rcu(&new->node, parent, p);
	rb_insert_color(&new->node, &priv->root);
	return 0;
}

static int nft_rbtree_insert(const struct net *net, const struct nft_set *set,
			     const struct nft_set_elem *elem,
			     struct nft_set_ext **ext)
{
	struct nft_rbtree *priv = nft_set_priv(set);
	struct nft_rbtree_elem *rbe = elem->priv;
	int err;

	write_lock_bh(&priv->lock);
	write_seqcount_begin(&priv->count);
	err = __nft_rbtree_insert(net, set, rbe, ext);
	write_seqcount_end(&priv->count);
	write_unlock_bh(&priv->lock);

	return err;
}

static void nft_rbtree_remove(const struct net *net,
			      const struct nft_set *set,
			      const struct nft_set_elem *elem)
{
	struct nft_rbtree *priv = nft_set_priv(set);
	struct nft_rbtree_elem *rbe = elem->priv;

	write_lock_bh(&priv->lock);
	write_seqcount_begin(&priv->count);
	rb_erase(&rbe->node, &priv->root);
	write_seqcount_end(&priv->count);
	write_unlock_bh(&priv->lock);
}

static void nft_rbtree_activate(const struct net *net,
				const struct nft_set *set,
				const struct nft_set_elem *elem)
{
	struct nft_rbtree_elem *rbe = elem->priv;

	nft_set_elem_change_active(net, set, &rbe->ext);
	nft_set_elem_clear_busy(&rbe->ext);
}

static bool nft_rbtree_flush(const struct net *net,
			     const struct nft_set *set, void *priv)
{
	struct nft_rbtree_elem *rbe = priv;

	if (!nft_set_elem_mark_busy(&rbe->ext) ||
	    !nft_is_active(net, &rbe->ext)) {
		nft_set_elem_change_active(net, set, &rbe->ext);
		return true;
	}
	return false;
}

static void *nft_rbtree_deactivate(const struct net *net,
				   const struct nft_set *set,
				   const struct nft_set_elem *elem)
{
	const struct nft_rbtree *priv = nft_set_priv(set);
	const struct rb_node *parent = priv->root.rb_node;
	struct nft_rbtree_elem *rbe, *this = elem->priv;
	u8 genmask = nft_genmask_next(net);
	int d;

	while (parent != NULL) {
		rbe = rb_entry(parent, struct nft_rbtree_elem, node);

		d = memcmp(nft_set_ext_key(&rbe->ext), &elem->key.val,
					   set->klen);
		if (d < 0)
			parent = parent->rb_left;
		else if (d > 0)
			parent = parent->rb_right;
		else {
			if (nft_rbtree_interval_end(rbe) &&
			    nft_rbtree_interval_start(this)) {
				parent = parent->rb_left;
				continue;
			} else if (nft_rbtree_interval_start(rbe) &&
				   nft_rbtree_interval_end(this)) {
				parent = parent->rb_right;
				continue;
			} else if (!nft_set_elem_active(&rbe->ext, genmask)) {
				parent = parent->rb_left;
				continue;
			}
			nft_rbtree_flush(net, set, rbe);
			return rbe;
		}
	}
	return NULL;
}

static void nft_rbtree_walk(const struct nft_ctx *ctx,
			    struct nft_set *set,
			    struct nft_set_iter *iter)
{
	struct nft_rbtree *priv = nft_set_priv(set);
	struct nft_rbtree_elem *rbe;
	struct nft_set_elem elem;
	struct rb_node *node;

	read_lock_bh(&priv->lock);
	for (node = rb_first(&priv->root); node != NULL; node = rb_next(node)) {
		rbe = rb_entry(node, struct nft_rbtree_elem, node);

		if (iter->count < iter->skip)
			goto cont;
		if (nft_set_elem_expired(&rbe->ext))
			goto cont;
		if (!nft_set_elem_active(&rbe->ext, iter->genmask))
			goto cont;

		elem.priv = rbe;

		iter->err = iter->fn(ctx, set, iter, &elem);
		if (iter->err < 0) {
			read_unlock_bh(&priv->lock);
			return;
		}
cont:
		iter->count++;
	}
	read_unlock_bh(&priv->lock);
}

static void nft_rbtree_gc(struct work_struct *work)
{
	struct nft_rbtree_elem *rbe, *rbe_end = NULL, *rbe_prev = NULL;
	struct nft_set_gc_batch *gcb = NULL;
	struct nft_rbtree *priv;
	struct rb_node *node;
	struct nft_set *set;

	priv = container_of(work, struct nft_rbtree, gc_work.work);
	set  = nft_set_container_of(priv);

	write_lock_bh(&priv->lock);
	write_seqcount_begin(&priv->count);
	for (node = rb_first(&priv->root); node != NULL; node = rb_next(node)) {
		rbe = rb_entry(node, struct nft_rbtree_elem, node);

		if (nft_rbtree_interval_end(rbe)) {
			rbe_end = rbe;
			continue;
		}
		if (!nft_set_elem_expired(&rbe->ext))
			continue;
		if (nft_set_elem_mark_busy(&rbe->ext))
			continue;

		if (rbe_prev) {
			rb_erase(&rbe_prev->node, &priv->root);
			rbe_prev = NULL;
		}
		gcb = nft_set_gc_batch_check(set, gcb, GFP_ATOMIC);
		if (!gcb)
			break;

		atomic_dec(&set->nelems);
		nft_set_gc_batch_add(gcb, rbe);
		rbe_prev = rbe;

		if (rbe_end) {
			atomic_dec(&set->nelems);
			nft_set_gc_batch_add(gcb, rbe_end);
			rb_erase(&rbe_end->node, &priv->root);
			rbe_end = NULL;
		}
		node = rb_next(node);
		if (!node)
			break;
	}
	if (rbe_prev)
		rb_erase(&rbe_prev->node, &priv->root);
	write_seqcount_end(&priv->count);
	write_unlock_bh(&priv->lock);

	rbe = nft_set_catchall_gc(set);
	if (rbe) {
		gcb = nft_set_gc_batch_check(set, gcb, GFP_ATOMIC);
		if (gcb)
			nft_set_gc_batch_add(gcb, rbe);
	}
	nft_set_gc_batch_complete(gcb);

	queue_delayed_work(system_power_efficient_wq, &priv->gc_work,
			   nft_set_gc_interval(set));
}

static u64 nft_rbtree_privsize(const struct nlattr * const nla[],
			       const struct nft_set_desc *desc)
{
	return sizeof(struct nft_rbtree);
}

static int nft_rbtree_init(const struct nft_set *set,
			   const struct nft_set_desc *desc,
			   const struct nlattr * const nla[])
{
	struct nft_rbtree *priv = nft_set_priv(set);

	rwlock_init(&priv->lock);
	seqcount_rwlock_init(&priv->count, &priv->lock);
	priv->root = RB_ROOT;

	INIT_DEFERRABLE_WORK(&priv->gc_work, nft_rbtree_gc);
	if (set->flags & NFT_SET_TIMEOUT)
		queue_delayed_work(system_power_efficient_wq, &priv->gc_work,
				   nft_set_gc_interval(set));

	return 0;
}

static void nft_rbtree_destroy(const struct nft_set *set)
{
	struct nft_rbtree *priv = nft_set_priv(set);
	struct nft_rbtree_elem *rbe;
	struct rb_node *node;

	cancel_delayed_work_sync(&priv->gc_work);
	rcu_barrier();
	while ((node = priv->root.rb_node) != NULL) {
		rb_erase(node, &priv->root);
		rbe = rb_entry(node, struct nft_rbtree_elem, node);
		nft_set_elem_destroy(set, rbe, true);
	}
}

static bool nft_rbtree_estimate(const struct nft_set_desc *desc, u32 features,
				struct nft_set_estimate *est)
{
	if (desc->field_count > 1)
		return false;

	if (desc->size)
		est->size = sizeof(struct nft_rbtree) +
			    desc->size * sizeof(struct nft_rbtree_elem);
	else
		est->size = ~0;

	est->lookup = NFT_SET_CLASS_O_LOG_N;
	est->space  = NFT_SET_CLASS_O_N;

	return true;
}

const struct nft_set_type nft_set_rbtree_type = {
	.features	= NFT_SET_INTERVAL | NFT_SET_MAP | NFT_SET_OBJECT | NFT_SET_TIMEOUT,
	.ops		= {
		.privsize	= nft_rbtree_privsize,
		.elemsize	= offsetof(struct nft_rbtree_elem, ext),
		.estimate	= nft_rbtree_estimate,
		.init		= nft_rbtree_init,
		.destroy	= nft_rbtree_destroy,
		.insert		= nft_rbtree_insert,
		.remove		= nft_rbtree_remove,
		.deactivate	= nft_rbtree_deactivate,
		.flush		= nft_rbtree_flush,
		.activate	= nft_rbtree_activate,
		.lookup		= nft_rbtree_lookup,
		.walk		= nft_rbtree_walk,
		.get		= nft_rbtree_get,
	},
};
