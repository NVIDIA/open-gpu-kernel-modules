/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * linux/can/rx-offload.h
 *
 * Copyright (c) 2014 David Jander, Protonic Holland
 * Copyright (c) 2014-2017 Pengutronix, Marc Kleine-Budde <kernel@pengutronix.de>
 */

#ifndef _CAN_RX_OFFLOAD_H
#define _CAN_RX_OFFLOAD_H

#include <linux/netdevice.h>
#include <linux/can.h>

struct can_rx_offload {
	struct net_device *dev;

	struct sk_buff *(*mailbox_read)(struct can_rx_offload *offload,
					unsigned int mb, u32 *timestamp,
					bool drop);

	struct sk_buff_head skb_queue;
	u32 skb_queue_len_max;

	unsigned int mb_first;
	unsigned int mb_last;

	struct napi_struct napi;

	bool inc;
};

int can_rx_offload_add_timestamp(struct net_device *dev,
				 struct can_rx_offload *offload);
int can_rx_offload_add_fifo(struct net_device *dev,
			    struct can_rx_offload *offload,
			    unsigned int weight);
int can_rx_offload_add_manual(struct net_device *dev,
			      struct can_rx_offload *offload,
			      unsigned int weight);
int can_rx_offload_irq_offload_timestamp(struct can_rx_offload *offload,
					 u64 reg);
int can_rx_offload_irq_offload_fifo(struct can_rx_offload *offload);
int can_rx_offload_queue_sorted(struct can_rx_offload *offload,
				struct sk_buff *skb, u32 timestamp);
unsigned int can_rx_offload_get_echo_skb(struct can_rx_offload *offload,
					 unsigned int idx, u32 timestamp,
					 unsigned int *frame_len_ptr);
int can_rx_offload_queue_tail(struct can_rx_offload *offload,
			      struct sk_buff *skb);
void can_rx_offload_del(struct can_rx_offload *offload);
void can_rx_offload_enable(struct can_rx_offload *offload);

static inline void can_rx_offload_schedule(struct can_rx_offload *offload)
{
	napi_schedule(&offload->napi);
}

static inline void can_rx_offload_disable(struct can_rx_offload *offload)
{
	napi_disable(&offload->napi);
}

#endif /* !_CAN_RX_OFFLOAD_H */
