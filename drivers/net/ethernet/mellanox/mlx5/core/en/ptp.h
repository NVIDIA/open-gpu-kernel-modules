/* SPDX-License-Identifier: GPL-2.0 OR Linux-OpenIB */
/* Copyright (c) 2020 Mellanox Technologies. */

#ifndef __MLX5_EN_PTP_H__
#define __MLX5_EN_PTP_H__

#include "en.h"
#include "en_stats.h"
#include <linux/ptp_classify.h>

struct mlx5e_ptpsq {
	struct mlx5e_txqsq       txqsq;
	struct mlx5e_cq          ts_cq;
	u16                      skb_fifo_cc;
	u16                      skb_fifo_pc;
	struct mlx5e_skb_fifo    skb_fifo;
	struct mlx5e_ptp_cq_stats *cq_stats;
};

enum {
	MLX5E_PTP_STATE_TX,
	MLX5E_PTP_STATE_RX,
	MLX5E_PTP_STATE_NUM_STATES,
};

struct mlx5e_ptp {
	/* data path */
	struct mlx5e_ptpsq         ptpsq[MLX5E_MAX_NUM_TC];
	struct mlx5e_rq            rq;
	struct napi_struct         napi;
	struct device             *pdev;
	struct net_device         *netdev;
	__be32                     mkey_be;
	u8                         num_tc;
	u8                         lag_port;

	/* data path - accessed per napi poll */
	struct mlx5e_ch_stats     *stats;

	/* control */
	struct mlx5e_priv         *priv;
	struct mlx5_core_dev      *mdev;
	struct hwtstamp_config    *tstamp;
	DECLARE_BITMAP(state, MLX5E_PTP_STATE_NUM_STATES);
};

static inline bool mlx5e_use_ptpsq(struct sk_buff *skb)
{
	struct flow_keys fk;

	if (!(skb_shinfo(skb)->tx_flags & SKBTX_HW_TSTAMP))
		return false;

	if (!skb_flow_dissect_flow_keys(skb, &fk, 0))
		return false;

	if (fk.basic.n_proto == htons(ETH_P_1588))
		return true;

	if (fk.basic.n_proto != htons(ETH_P_IP) &&
	    fk.basic.n_proto != htons(ETH_P_IPV6))
		return false;

	return (fk.basic.ip_proto == IPPROTO_UDP &&
		fk.ports.dst == htons(PTP_EV_PORT));
}

int mlx5e_ptp_open(struct mlx5e_priv *priv, struct mlx5e_params *params,
		   u8 lag_port, struct mlx5e_ptp **cp);
void mlx5e_ptp_close(struct mlx5e_ptp *c);
void mlx5e_ptp_activate_channel(struct mlx5e_ptp *c);
void mlx5e_ptp_deactivate_channel(struct mlx5e_ptp *c);
int mlx5e_ptp_get_rqn(struct mlx5e_ptp *c, u32 *rqn);
int mlx5e_ptp_alloc_rx_fs(struct mlx5e_priv *priv);
void mlx5e_ptp_free_rx_fs(struct mlx5e_priv *priv);
int mlx5e_ptp_rx_manage_fs(struct mlx5e_priv *priv, bool set);

enum {
	MLX5E_SKB_CB_CQE_HWTSTAMP  = BIT(0),
	MLX5E_SKB_CB_PORT_HWTSTAMP = BIT(1),
};

void mlx5e_skb_cb_hwtstamp_handler(struct sk_buff *skb, int hwtstamp_type,
				   ktime_t hwtstamp,
				   struct mlx5e_ptp_cq_stats *cq_stats);

void mlx5e_skb_cb_hwtstamp_init(struct sk_buff *skb);
#endif /* __MLX5_EN_PTP_H__ */
