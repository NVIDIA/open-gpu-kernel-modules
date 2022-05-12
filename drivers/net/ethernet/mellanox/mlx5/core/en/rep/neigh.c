// SPDX-License-Identifier: GPL-2.0 OR Linux-OpenIB
/* Copyright (c) 2020 Mellanox Technologies. */

#include <linux/refcount.h>
#include <linux/list.h>
#include <linux/rculist.h>
#include <linux/rtnetlink.h>
#include <linux/workqueue.h>
#include <linux/spinlock.h>
#include <linux/notifier.h>
#include <net/netevent.h>
#include <net/arp.h>
#include "neigh.h"
#include "tc.h"
#include "en_rep.h"
#include "fs_core.h"
#include "diag/en_rep_tracepoint.h"

static unsigned long mlx5e_rep_ipv6_interval(void)
{
	if (IS_ENABLED(CONFIG_IPV6) && ipv6_stub->nd_tbl)
		return NEIGH_VAR(&ipv6_stub->nd_tbl->parms, DELAY_PROBE_TIME);

	return ~0UL;
}

static void mlx5e_rep_neigh_update_init_interval(struct mlx5e_rep_priv *rpriv)
{
	unsigned long ipv4_interval = NEIGH_VAR(&arp_tbl.parms, DELAY_PROBE_TIME);
	unsigned long ipv6_interval = mlx5e_rep_ipv6_interval();
	struct net_device *netdev = rpriv->netdev;
	struct mlx5e_priv *priv = netdev_priv(netdev);

	rpriv->neigh_update.min_interval = min_t(unsigned long, ipv6_interval, ipv4_interval);
	mlx5_fc_update_sampling_interval(priv->mdev, rpriv->neigh_update.min_interval);
}

void mlx5e_rep_queue_neigh_stats_work(struct mlx5e_priv *priv)
{
	struct mlx5e_rep_priv *rpriv = priv->ppriv;
	struct mlx5e_neigh_update_table *neigh_update = &rpriv->neigh_update;

	mlx5_fc_queue_stats_work(priv->mdev,
				 &neigh_update->neigh_stats_work,
				 neigh_update->min_interval);
}

static bool mlx5e_rep_neigh_entry_hold(struct mlx5e_neigh_hash_entry *nhe)
{
	return refcount_inc_not_zero(&nhe->refcnt);
}

static void mlx5e_rep_neigh_entry_remove(struct mlx5e_neigh_hash_entry *nhe);

void mlx5e_rep_neigh_entry_release(struct mlx5e_neigh_hash_entry *nhe)
{
	if (refcount_dec_and_test(&nhe->refcnt)) {
		mlx5e_rep_neigh_entry_remove(nhe);
		kfree_rcu(nhe, rcu);
	}
}

static struct mlx5e_neigh_hash_entry *
mlx5e_get_next_nhe(struct mlx5e_rep_priv *rpriv,
		   struct mlx5e_neigh_hash_entry *nhe)
{
	struct mlx5e_neigh_hash_entry *next = NULL;

	rcu_read_lock();

	for (next = nhe ?
		     list_next_or_null_rcu(&rpriv->neigh_update.neigh_list,
					   &nhe->neigh_list,
					   struct mlx5e_neigh_hash_entry,
					   neigh_list) :
		     list_first_or_null_rcu(&rpriv->neigh_update.neigh_list,
					    struct mlx5e_neigh_hash_entry,
					    neigh_list);
	     next;
	     next = list_next_or_null_rcu(&rpriv->neigh_update.neigh_list,
					  &next->neigh_list,
					  struct mlx5e_neigh_hash_entry,
					  neigh_list))
		if (mlx5e_rep_neigh_entry_hold(next))
			break;

	rcu_read_unlock();

	if (nhe)
		mlx5e_rep_neigh_entry_release(nhe);

	return next;
}

static void mlx5e_rep_neigh_stats_work(struct work_struct *work)
{
	struct mlx5e_rep_priv *rpriv = container_of(work, struct mlx5e_rep_priv,
						    neigh_update.neigh_stats_work.work);
	struct net_device *netdev = rpriv->netdev;
	struct mlx5e_priv *priv = netdev_priv(netdev);
	struct mlx5e_neigh_hash_entry *nhe = NULL;

	rtnl_lock();
	if (!list_empty(&rpriv->neigh_update.neigh_list))
		mlx5e_rep_queue_neigh_stats_work(priv);

	while ((nhe = mlx5e_get_next_nhe(rpriv, nhe)) != NULL)
		mlx5e_tc_update_neigh_used_value(nhe);

	rtnl_unlock();
}

struct neigh_update_work {
	struct work_struct work;
	struct neighbour *n;
	struct mlx5e_neigh_hash_entry *nhe;
};

static void mlx5e_release_neigh_update_work(struct neigh_update_work *update_work)
{
	neigh_release(update_work->n);
	mlx5e_rep_neigh_entry_release(update_work->nhe);
	kfree(update_work);
}

static void mlx5e_rep_neigh_update(struct work_struct *work)
{
	struct neigh_update_work *update_work = container_of(work, struct neigh_update_work,
							     work);
	struct mlx5e_neigh_hash_entry *nhe = update_work->nhe;
	struct neighbour *n = update_work->n;
	struct mlx5e_encap_entry *e = NULL;
	bool neigh_connected, same_dev;
	unsigned char ha[ETH_ALEN];
	u8 nud_state, dead;

	rtnl_lock();

	/* If these parameters are changed after we release the lock,
	 * we'll receive another event letting us know about it.
	 * We use this lock to avoid inconsistency between the neigh validity
	 * and it's hw address.
	 */
	read_lock_bh(&n->lock);
	memcpy(ha, n->ha, ETH_ALEN);
	nud_state = n->nud_state;
	dead = n->dead;
	same_dev = READ_ONCE(nhe->neigh_dev) == n->dev;
	read_unlock_bh(&n->lock);

	neigh_connected = (nud_state & NUD_VALID) && !dead;

	trace_mlx5e_rep_neigh_update(nhe, ha, neigh_connected);

	if (!same_dev)
		goto out;

	/* mlx5e_get_next_init_encap() releases previous encap before returning
	 * the next one.
	 */
	while ((e = mlx5e_get_next_init_encap(nhe, e)) != NULL)
		mlx5e_rep_update_flows(netdev_priv(e->out_dev), e, neigh_connected, ha);

out:
	rtnl_unlock();
	mlx5e_release_neigh_update_work(update_work);
}

static struct neigh_update_work *mlx5e_alloc_neigh_update_work(struct mlx5e_priv *priv,
							       struct neighbour *n)
{
	struct neigh_update_work *update_work;
	struct mlx5e_neigh_hash_entry *nhe;
	struct mlx5e_neigh m_neigh = {};

	update_work = kzalloc(sizeof(*update_work), GFP_ATOMIC);
	if (WARN_ON(!update_work))
		return NULL;

	m_neigh.family = n->ops->family;
	memcpy(&m_neigh.dst_ip, n->primary_key, n->tbl->key_len);

	/* Obtain reference to nhe as last step in order not to release it in
	 * atomic context.
	 */
	rcu_read_lock();
	nhe = mlx5e_rep_neigh_entry_lookup(priv, &m_neigh);
	rcu_read_unlock();
	if (!nhe) {
		kfree(update_work);
		return NULL;
	}

	INIT_WORK(&update_work->work, mlx5e_rep_neigh_update);
	neigh_hold(n);
	update_work->n = n;
	update_work->nhe = nhe;

	return update_work;
}

static int mlx5e_rep_netevent_event(struct notifier_block *nb,
				    unsigned long event, void *ptr)
{
	struct mlx5e_rep_priv *rpriv = container_of(nb, struct mlx5e_rep_priv,
						    neigh_update.netevent_nb);
	struct mlx5e_neigh_update_table *neigh_update = &rpriv->neigh_update;
	struct net_device *netdev = rpriv->netdev;
	struct mlx5e_priv *priv = netdev_priv(netdev);
	struct mlx5e_neigh_hash_entry *nhe = NULL;
	struct neigh_update_work *update_work;
	struct neigh_parms *p;
	struct neighbour *n;
	bool found = false;

	switch (event) {
	case NETEVENT_NEIGH_UPDATE:
		n = ptr;
#if IS_ENABLED(CONFIG_IPV6)
		if (n->tbl != ipv6_stub->nd_tbl && n->tbl != &arp_tbl)
#else
		if (n->tbl != &arp_tbl)
#endif
			return NOTIFY_DONE;

		update_work = mlx5e_alloc_neigh_update_work(priv, n);
		if (!update_work)
			return NOTIFY_DONE;

		queue_work(priv->wq, &update_work->work);
		break;

	case NETEVENT_DELAY_PROBE_TIME_UPDATE:
		p = ptr;

		/* We check the device is present since we don't care about
		 * changes in the default table, we only care about changes
		 * done per device delay prob time parameter.
		 */
#if IS_ENABLED(CONFIG_IPV6)
		if (!p->dev || (p->tbl != ipv6_stub->nd_tbl && p->tbl != &arp_tbl))
#else
		if (!p->dev || p->tbl != &arp_tbl)
#endif
			return NOTIFY_DONE;

		rcu_read_lock();
		list_for_each_entry_rcu(nhe, &neigh_update->neigh_list,
					neigh_list) {
			if (p->dev == READ_ONCE(nhe->neigh_dev)) {
				found = true;
				break;
			}
		}
		rcu_read_unlock();
		if (!found)
			return NOTIFY_DONE;

		neigh_update->min_interval = min_t(unsigned long,
						   NEIGH_VAR(p, DELAY_PROBE_TIME),
						   neigh_update->min_interval);
		mlx5_fc_update_sampling_interval(priv->mdev,
						 neigh_update->min_interval);
		break;
	}
	return NOTIFY_DONE;
}

static const struct rhashtable_params mlx5e_neigh_ht_params = {
	.head_offset = offsetof(struct mlx5e_neigh_hash_entry, rhash_node),
	.key_offset = offsetof(struct mlx5e_neigh_hash_entry, m_neigh),
	.key_len = sizeof(struct mlx5e_neigh),
	.automatic_shrinking = true,
};

int mlx5e_rep_neigh_init(struct mlx5e_rep_priv *rpriv)
{
	struct mlx5e_neigh_update_table *neigh_update = &rpriv->neigh_update;
	int err;

	err = rhashtable_init(&neigh_update->neigh_ht, &mlx5e_neigh_ht_params);
	if (err)
		goto out_err;

	INIT_LIST_HEAD(&neigh_update->neigh_list);
	mutex_init(&neigh_update->encap_lock);
	INIT_DELAYED_WORK(&neigh_update->neigh_stats_work,
			  mlx5e_rep_neigh_stats_work);
	mlx5e_rep_neigh_update_init_interval(rpriv);

	neigh_update->netevent_nb.notifier_call = mlx5e_rep_netevent_event;
	err = register_netevent_notifier(&neigh_update->netevent_nb);
	if (err)
		goto out_notifier;
	return 0;

out_notifier:
	neigh_update->netevent_nb.notifier_call = NULL;
	rhashtable_destroy(&neigh_update->neigh_ht);
out_err:
	netdev_warn(rpriv->netdev,
		    "Failed to initialize neighbours handling for vport %d\n",
		    rpriv->rep->vport);
	return err;
}

void mlx5e_rep_neigh_cleanup(struct mlx5e_rep_priv *rpriv)
{
	struct mlx5e_neigh_update_table *neigh_update = &rpriv->neigh_update;
	struct mlx5e_priv *priv = netdev_priv(rpriv->netdev);

	if (!rpriv->neigh_update.netevent_nb.notifier_call)
		return;

	unregister_netevent_notifier(&neigh_update->netevent_nb);

	flush_workqueue(priv->wq); /* flush neigh update works */

	cancel_delayed_work_sync(&rpriv->neigh_update.neigh_stats_work);

	mutex_destroy(&neigh_update->encap_lock);
	rhashtable_destroy(&neigh_update->neigh_ht);
}

static int mlx5e_rep_neigh_entry_insert(struct mlx5e_priv *priv,
					struct mlx5e_neigh_hash_entry *nhe)
{
	struct mlx5e_rep_priv *rpriv = priv->ppriv;
	int err;

	err = rhashtable_insert_fast(&rpriv->neigh_update.neigh_ht,
				     &nhe->rhash_node,
				     mlx5e_neigh_ht_params);
	if (err)
		return err;

	list_add_rcu(&nhe->neigh_list, &rpriv->neigh_update.neigh_list);

	return err;
}

static void mlx5e_rep_neigh_entry_remove(struct mlx5e_neigh_hash_entry *nhe)
{
	struct mlx5e_rep_priv *rpriv = nhe->priv->ppriv;

	mutex_lock(&rpriv->neigh_update.encap_lock);

	list_del_rcu(&nhe->neigh_list);

	rhashtable_remove_fast(&rpriv->neigh_update.neigh_ht,
			       &nhe->rhash_node,
			       mlx5e_neigh_ht_params);
	mutex_unlock(&rpriv->neigh_update.encap_lock);
}

/* This function must only be called under the representor's encap_lock or
 * inside rcu read lock section.
 */
struct mlx5e_neigh_hash_entry *
mlx5e_rep_neigh_entry_lookup(struct mlx5e_priv *priv,
			     struct mlx5e_neigh *m_neigh)
{
	struct mlx5e_rep_priv *rpriv = priv->ppriv;
	struct mlx5e_neigh_update_table *neigh_update = &rpriv->neigh_update;
	struct mlx5e_neigh_hash_entry *nhe;

	nhe = rhashtable_lookup_fast(&neigh_update->neigh_ht, m_neigh,
				     mlx5e_neigh_ht_params);
	return nhe && mlx5e_rep_neigh_entry_hold(nhe) ? nhe : NULL;
}

int mlx5e_rep_neigh_entry_create(struct mlx5e_priv *priv,
				 struct mlx5e_neigh *m_neigh,
				 struct net_device *neigh_dev,
				 struct mlx5e_neigh_hash_entry **nhe)
{
	int err;

	*nhe = kzalloc(sizeof(**nhe), GFP_KERNEL);
	if (!*nhe)
		return -ENOMEM;

	(*nhe)->priv = priv;
	memcpy(&(*nhe)->m_neigh, m_neigh, sizeof(*m_neigh));
	spin_lock_init(&(*nhe)->encap_list_lock);
	INIT_LIST_HEAD(&(*nhe)->encap_list);
	refcount_set(&(*nhe)->refcnt, 1);
	WRITE_ONCE((*nhe)->neigh_dev, neigh_dev);

	err = mlx5e_rep_neigh_entry_insert(priv, *nhe);
	if (err)
		goto out_free;
	return 0;

out_free:
	kfree(*nhe);
	return err;
}
