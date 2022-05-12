/* SPDX-License-Identifier: GPL-2.0 */
/******************************************************************************
 *
 * Copyright(c) 2007 - 2013 Realtek Corporation. All rights reserved.
 *
 ******************************************************************************/
#ifndef __OSDEP_LINUX_SERVICE_H_
#define __OSDEP_LINUX_SERVICE_H_

	#include <linux/spinlock.h>
	#include <linux/compiler.h>
	#include <linux/kernel.h>
	#include <linux/errno.h>
	#include <linux/init.h>
	#include <linux/slab.h>
	#include <linux/module.h>
	#include <linux/kref.h>
	/* include <linux/smp_lock.h> */
	#include <linux/netdevice.h>
	#include <linux/skbuff.h>
	#include <linux/uaccess.h>
	#include <asm/byteorder.h>
	#include <linux/atomic.h>
	#include <linux/io.h>
	#include <linux/sem.h>
	#include <linux/sched.h>
	#include <linux/etherdevice.h>
	#include <linux/wireless.h>
	#include <net/iw_handler.h>
	#include <linux/if_arp.h>
	#include <linux/rtnetlink.h>
	#include <linux/delay.h>
	#include <linux/interrupt.h>	/*  for struct tasklet_struct */
	#include <linux/ip.h>
	#include <linux/kthread.h>
	#include <linux/list.h>
	#include <linux/vmalloc.h>

/* 	#include <linux/ieee80211.h> */
        #include <net/ieee80211_radiotap.h>
	#include <net/cfg80211.h>

	struct	__queue	{
		struct	list_head	queue;
		spinlock_t	lock;
	};

	#define thread_exit() complete_and_exit(NULL, 0)

static inline struct list_head *get_next(struct list_head	*list)
{
	return list->next;
}

static inline struct list_head	*get_list_head(struct __queue	*queue)
{
	return (&(queue->queue));
}

static inline void _set_timer(struct timer_list *ptimer, u32 delay_time)
{
	mod_timer(ptimer, (jiffies + (delay_time * HZ / 1000)));
}

static inline void _init_workitem(struct work_struct *pwork, void *pfunc, void *cntx)
{
	INIT_WORK(pwork, pfunc);
}

static inline void _set_workitem(struct work_struct *pwork)
{
	schedule_work(pwork);
}

static inline void _cancel_workitem_sync(struct work_struct *pwork)
{
	cancel_work_sync(pwork);
}

static inline int rtw_netif_queue_stopped(struct net_device *pnetdev)
{
	return (netif_tx_queue_stopped(netdev_get_tx_queue(pnetdev, 0)) &&
		netif_tx_queue_stopped(netdev_get_tx_queue(pnetdev, 1)) &&
		netif_tx_queue_stopped(netdev_get_tx_queue(pnetdev, 2)) &&
		netif_tx_queue_stopped(netdev_get_tx_queue(pnetdev, 3)));
}

static inline void rtw_netif_wake_queue(struct net_device *pnetdev)
{
	netif_tx_wake_all_queues(pnetdev);
}

static inline void rtw_netif_start_queue(struct net_device *pnetdev)
{
	netif_tx_start_all_queues(pnetdev);
}

static inline void rtw_netif_stop_queue(struct net_device *pnetdev)
{
	netif_tx_stop_all_queues(pnetdev);
}

#define rtw_signal_process(pid, sig) kill_pid(find_vpid((pid)), (sig), 1)

#define NDEV_ARG(ndev) ndev->name
#define ADPT_ARG(adapter) adapter->pnetdev->name
#define FUNC_NDEV_FMT "%s(%s)"
#define FUNC_NDEV_ARG(ndev) __func__, ndev->name
#define FUNC_ADPT_FMT "%s(%s)"
#define FUNC_ADPT_ARG(adapter) __func__, adapter->pnetdev->name

struct rtw_netdev_priv_indicator {
	void *priv;
	u32 sizeof_priv;
};

static inline struct adapter *rtw_netdev_priv(struct net_device *netdev)
{
	return ((struct rtw_netdev_priv_indicator *)netdev_priv(netdev))->priv;
}

struct net_device *rtw_alloc_etherdev_with_old_priv(int sizeof_priv, void *old_priv);
extern struct net_device *rtw_alloc_etherdev(int sizeof_priv);

#endif
