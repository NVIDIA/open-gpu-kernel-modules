// SPDX-License-Identifier: GPL-2.0
#include <linux/skbuff.h>
#include <linux/slab.h>
#include <linux/netdevice.h>
#include <net/gro_cells.h>

struct gro_cell {
	struct sk_buff_head	napi_skbs;
	struct napi_struct	napi;
};

int gro_cells_receive(struct gro_cells *gcells, struct sk_buff *skb)
{
	struct net_device *dev = skb->dev;
	struct gro_cell *cell;
	int res;

	rcu_read_lock();
	if (unlikely(!(dev->flags & IFF_UP)))
		goto drop;

	if (!gcells->cells || skb_cloned(skb) || netif_elide_gro(dev)) {
		res = netif_rx(skb);
		goto unlock;
	}

	cell = this_cpu_ptr(gcells->cells);

	if (skb_queue_len(&cell->napi_skbs) > netdev_max_backlog) {
drop:
		atomic_long_inc(&dev->rx_dropped);
		kfree_skb(skb);
		res = NET_RX_DROP;
		goto unlock;
	}

	__skb_queue_tail(&cell->napi_skbs, skb);
	if (skb_queue_len(&cell->napi_skbs) == 1)
		napi_schedule(&cell->napi);

	res = NET_RX_SUCCESS;

unlock:
	rcu_read_unlock();
	return res;
}
EXPORT_SYMBOL(gro_cells_receive);

/* called under BH context */
static int gro_cell_poll(struct napi_struct *napi, int budget)
{
	struct gro_cell *cell = container_of(napi, struct gro_cell, napi);
	struct sk_buff *skb;
	int work_done = 0;

	while (work_done < budget) {
		skb = __skb_dequeue(&cell->napi_skbs);
		if (!skb)
			break;
		napi_gro_receive(napi, skb);
		work_done++;
	}

	if (work_done < budget)
		napi_complete_done(napi, work_done);
	return work_done;
}

int gro_cells_init(struct gro_cells *gcells, struct net_device *dev)
{
	int i;

	gcells->cells = alloc_percpu(struct gro_cell);
	if (!gcells->cells)
		return -ENOMEM;

	for_each_possible_cpu(i) {
		struct gro_cell *cell = per_cpu_ptr(gcells->cells, i);

		__skb_queue_head_init(&cell->napi_skbs);

		set_bit(NAPI_STATE_NO_BUSY_POLL, &cell->napi.state);

		netif_napi_add(dev, &cell->napi, gro_cell_poll,
			       NAPI_POLL_WEIGHT);
		napi_enable(&cell->napi);
	}
	return 0;
}
EXPORT_SYMBOL(gro_cells_init);

void gro_cells_destroy(struct gro_cells *gcells)
{
	int i;

	if (!gcells->cells)
		return;
	for_each_possible_cpu(i) {
		struct gro_cell *cell = per_cpu_ptr(gcells->cells, i);

		napi_disable(&cell->napi);
		__netif_napi_del(&cell->napi);
		__skb_queue_purge(&cell->napi_skbs);
	}
	/* This barrier is needed because netpoll could access dev->napi_list
	 * under rcu protection.
	 */
	synchronize_net();

	free_percpu(gcells->cells);
	gcells->cells = NULL;
}
EXPORT_SYMBOL(gro_cells_destroy);
