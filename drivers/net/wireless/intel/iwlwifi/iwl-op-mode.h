/* SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause */
/*
 * Copyright (C) 2005-2014, 2018-2021 Intel Corporation
 * Copyright (C) 2013-2014 Intel Mobile Communications GmbH
 * Copyright (C) 2015 Intel Deutschland GmbH
 */
#ifndef __iwl_op_mode_h__
#define __iwl_op_mode_h__

#include <linux/netdevice.h>
#include <linux/debugfs.h>
#include "iwl-dbg-tlv.h"

struct iwl_op_mode;
struct iwl_trans;
struct sk_buff;
struct iwl_device_cmd;
struct iwl_rx_cmd_buffer;
struct iwl_fw;
struct iwl_cfg;

/**
 * DOC: Operational mode - what is it ?
 *
 * The operational mode (a.k.a. op_mode) is the layer that implements
 * mac80211's handlers. It knows two APIs: mac80211's and the fw's. It uses
 * the transport API to access the HW. The op_mode doesn't need to know how the
 * underlying HW works, since the transport layer takes care of that.
 *
 * There can be several op_mode: i.e. different fw APIs will require two
 * different op_modes. This is why the op_mode is virtualized.
 */

/**
 * DOC: Life cycle of the Operational mode
 *
 * The operational mode has a very simple life cycle.
 *
 *	1) The driver layer (iwl-drv.c) chooses the op_mode based on the
 *	   capabilities advertised by the fw file (in TLV format).
 *	2) The driver layer starts the op_mode (ops->start)
 *	3) The op_mode registers mac80211
 *	4) The op_mode is governed by mac80211
 *	5) The driver layer stops the op_mode
 */

/**
 * struct iwl_op_mode_ops - op_mode specific operations
 *
 * The op_mode exports its ops so that external components can start it and
 * interact with it. The driver layer typically calls the start and stop
 * handlers, the transport layer calls the others.
 *
 * All the handlers MUST be implemented, except @rx_rss which can be left
 * out *iff* the opmode will never run on hardware with multi-queue capability.
 *
 * @start: start the op_mode. The transport layer is already allocated.
 *	May sleep
 * @stop: stop the op_mode. Must free all the memory allocated.
 *	May sleep
 * @rx: Rx notification to the op_mode. rxb is the Rx buffer itself. Cmd is the
 *	HCMD this Rx responds to. Can't sleep.
 * @rx_rss: data queue RX notification to the op_mode, for (data) notifications
 *	received on the RSS queue(s). The queue parameter indicates which of the
 *	RSS queues received this frame; it will always be non-zero.
 *	This method must not sleep.
 * @async_cb: called when an ASYNC command with CMD_WANT_ASYNC_CALLBACK set
 *	completes. Must be atomic.
 * @queue_full: notifies that a HW queue is full.
 *	Must be atomic and called with BH disabled.
 * @queue_not_full: notifies that a HW queue is not full any more.
 *	Must be atomic and called with BH disabled.
 * @hw_rf_kill:notifies of a change in the HW rf kill switch. True means that
 *	the radio is killed. Return %true if the device should be stopped by
 *	the transport immediately after the call. May sleep.
 * @free_skb: allows the transport layer to free skbs that haven't been
 *	reclaimed by the op_mode. This can happen when the driver is freed and
 *	there are Tx packets pending in the transport layer.
 *	Must be atomic
 * @nic_error: error notification. Must be atomic and must be called with BH
 *	disabled.
 * @cmd_queue_full: Called when the command queue gets full. Must be atomic and
 *	called with BH disabled.
 * @nic_config: configure NIC, called before firmware is started.
 *	May sleep
 * @wimax_active: invoked when WiMax becomes active. May sleep
 * @time_point: called when transport layer wants to collect debug data
 */
struct iwl_op_mode_ops {
	struct iwl_op_mode *(*start)(struct iwl_trans *trans,
				     const struct iwl_cfg *cfg,
				     const struct iwl_fw *fw,
				     struct dentry *dbgfs_dir);
	void (*stop)(struct iwl_op_mode *op_mode);
	void (*rx)(struct iwl_op_mode *op_mode, struct napi_struct *napi,
		   struct iwl_rx_cmd_buffer *rxb);
	void (*rx_rss)(struct iwl_op_mode *op_mode, struct napi_struct *napi,
		       struct iwl_rx_cmd_buffer *rxb, unsigned int queue);
	void (*async_cb)(struct iwl_op_mode *op_mode,
			 const struct iwl_device_cmd *cmd);
	void (*queue_full)(struct iwl_op_mode *op_mode, int queue);
	void (*queue_not_full)(struct iwl_op_mode *op_mode, int queue);
	bool (*hw_rf_kill)(struct iwl_op_mode *op_mode, bool state);
	void (*free_skb)(struct iwl_op_mode *op_mode, struct sk_buff *skb);
	void (*nic_error)(struct iwl_op_mode *op_mode);
	void (*cmd_queue_full)(struct iwl_op_mode *op_mode);
	void (*nic_config)(struct iwl_op_mode *op_mode);
	void (*wimax_active)(struct iwl_op_mode *op_mode);
	void (*time_point)(struct iwl_op_mode *op_mode,
			   enum iwl_fw_ini_time_point tp_id,
			   union iwl_dbg_tlv_tp_data *tp_data);
};

int iwl_opmode_register(const char *name, const struct iwl_op_mode_ops *ops);
void iwl_opmode_deregister(const char *name);

/**
 * struct iwl_op_mode - operational mode
 * @ops: pointer to its own ops
 *
 * This holds an implementation of the mac80211 / fw API.
 */
struct iwl_op_mode {
	const struct iwl_op_mode_ops *ops;

	char op_mode_specific[] __aligned(sizeof(void *));
};

static inline void iwl_op_mode_stop(struct iwl_op_mode *op_mode)
{
	might_sleep();
	op_mode->ops->stop(op_mode);
}

static inline void iwl_op_mode_rx(struct iwl_op_mode *op_mode,
				  struct napi_struct *napi,
				  struct iwl_rx_cmd_buffer *rxb)
{
	return op_mode->ops->rx(op_mode, napi, rxb);
}

static inline void iwl_op_mode_rx_rss(struct iwl_op_mode *op_mode,
				      struct napi_struct *napi,
				      struct iwl_rx_cmd_buffer *rxb,
				      unsigned int queue)
{
	op_mode->ops->rx_rss(op_mode, napi, rxb, queue);
}

static inline void iwl_op_mode_async_cb(struct iwl_op_mode *op_mode,
					const struct iwl_device_cmd *cmd)
{
	if (op_mode->ops->async_cb)
		op_mode->ops->async_cb(op_mode, cmd);
}

static inline void iwl_op_mode_queue_full(struct iwl_op_mode *op_mode,
					  int queue)
{
	op_mode->ops->queue_full(op_mode, queue);
}

static inline void iwl_op_mode_queue_not_full(struct iwl_op_mode *op_mode,
					      int queue)
{
	op_mode->ops->queue_not_full(op_mode, queue);
}

static inline bool __must_check
iwl_op_mode_hw_rf_kill(struct iwl_op_mode *op_mode, bool state)
{
	might_sleep();
	return op_mode->ops->hw_rf_kill(op_mode, state);
}

static inline void iwl_op_mode_free_skb(struct iwl_op_mode *op_mode,
					struct sk_buff *skb)
{
	if (WARN_ON_ONCE(!op_mode))
		return;
	op_mode->ops->free_skb(op_mode, skb);
}

static inline void iwl_op_mode_nic_error(struct iwl_op_mode *op_mode)
{
	op_mode->ops->nic_error(op_mode);
}

static inline void iwl_op_mode_cmd_queue_full(struct iwl_op_mode *op_mode)
{
	op_mode->ops->cmd_queue_full(op_mode);
}

static inline void iwl_op_mode_nic_config(struct iwl_op_mode *op_mode)
{
	might_sleep();
	op_mode->ops->nic_config(op_mode);
}

static inline void iwl_op_mode_wimax_active(struct iwl_op_mode *op_mode)
{
	might_sleep();
	op_mode->ops->wimax_active(op_mode);
}

static inline void iwl_op_mode_time_point(struct iwl_op_mode *op_mode,
					  enum iwl_fw_ini_time_point tp_id,
					  union iwl_dbg_tlv_tp_data *tp_data)
{
	if (!op_mode || !op_mode->ops || !op_mode->ops->time_point)
		return;
	op_mode->ops->time_point(op_mode, tp_id, tp_data);
}

#endif /* __iwl_op_mode_h__ */
