// SPDX-License-Identifier: GPL-2.0 OR Linux-OpenIB
/* Copyright (c) 2019 Mellanox Technologies. */

#include "setup.h"
#include "en/params.h"
#include "en/txrx.h"

/* It matches XDP_UMEM_MIN_CHUNK_SIZE, but as this constant is private and may
 * change unexpectedly, and mlx5e has a minimum valid stride size for striding
 * RQ, keep this check in the driver.
 */
#define MLX5E_MIN_XSK_CHUNK_SIZE 2048

bool mlx5e_validate_xsk_param(struct mlx5e_params *params,
			      struct mlx5e_xsk_param *xsk,
			      struct mlx5_core_dev *mdev)
{
	/* AF_XDP doesn't support frames larger than PAGE_SIZE. */
	if (xsk->chunk_size > PAGE_SIZE ||
			xsk->chunk_size < MLX5E_MIN_XSK_CHUNK_SIZE)
		return false;

	/* Current MTU and XSK headroom don't allow packets to fit the frames. */
	if (mlx5e_rx_get_min_frag_sz(params, xsk) > xsk->chunk_size)
		return false;

	/* frag_sz is different for regular and XSK RQs, so ensure that linear
	 * SKB mode is possible.
	 */
	switch (params->rq_wq_type) {
	case MLX5_WQ_TYPE_LINKED_LIST_STRIDING_RQ:
		return mlx5e_rx_mpwqe_is_linear_skb(mdev, params, xsk);
	default: /* MLX5_WQ_TYPE_CYCLIC */
		return mlx5e_rx_is_linear_skb(params, xsk);
	}
}

static void mlx5e_build_xsk_cparam(struct mlx5_core_dev *mdev,
				   struct mlx5e_params *params,
				   struct mlx5e_xsk_param *xsk,
				   u16 q_counter,
				   struct mlx5e_channel_param *cparam)
{
	mlx5e_build_rq_param(mdev, params, xsk, q_counter, &cparam->rq);
	mlx5e_build_xdpsq_param(mdev, params, &cparam->xdp_sq);
}

static int mlx5e_init_xsk_rq(struct mlx5e_channel *c,
			     struct mlx5e_params *params,
			     struct xsk_buff_pool *pool,
			     struct mlx5e_xsk_param *xsk,
			     struct mlx5e_rq *rq)
{
	struct mlx5_core_dev *mdev = c->mdev;
	int rq_xdp_ix;
	int err;

	rq->wq_type      = params->rq_wq_type;
	rq->pdev         = c->pdev;
	rq->netdev       = c->netdev;
	rq->priv         = c->priv;
	rq->tstamp       = c->tstamp;
	rq->clock        = &mdev->clock;
	rq->icosq        = &c->icosq;
	rq->ix           = c->ix;
	rq->mdev         = mdev;
	rq->hw_mtu       = MLX5E_SW2HW_MTU(params, params->sw_mtu);
	rq->xdpsq        = &c->rq_xdpsq;
	rq->xsk_pool     = pool;
	rq->stats        = &c->priv->channel_stats[c->ix].xskrq;
	rq->ptp_cyc2time = mlx5_rq_ts_translator(mdev);
	rq_xdp_ix        = c->ix + params->num_channels * MLX5E_RQ_GROUP_XSK;
	err = mlx5e_rq_set_handlers(rq, params, xsk);
	if (err)
		return err;

	return  xdp_rxq_info_reg(&rq->xdp_rxq, rq->netdev, rq_xdp_ix, 0);
}

static int mlx5e_open_xsk_rq(struct mlx5e_channel *c, struct mlx5e_params *params,
			     struct mlx5e_rq_param *rq_params, struct xsk_buff_pool *pool,
			     struct mlx5e_xsk_param *xsk)
{
	int err;

	err = mlx5e_init_xsk_rq(c, params, pool, xsk, &c->xskrq);
	if (err)
		return err;

	return mlx5e_open_rq(params, rq_params, xsk, cpu_to_node(c->cpu), &c->xskrq);
}

int mlx5e_open_xsk(struct mlx5e_priv *priv, struct mlx5e_params *params,
		   struct mlx5e_xsk_param *xsk, struct xsk_buff_pool *pool,
		   struct mlx5e_channel *c)
{
	struct mlx5e_channel_param *cparam;
	struct mlx5e_create_cq_param ccp;
	int err;

	mlx5e_build_create_cq_param(&ccp, c);

	if (!mlx5e_validate_xsk_param(params, xsk, priv->mdev))
		return -EINVAL;

	cparam = kvzalloc(sizeof(*cparam), GFP_KERNEL);
	if (!cparam)
		return -ENOMEM;

	mlx5e_build_xsk_cparam(priv->mdev, params, xsk, priv->q_counter, cparam);

	err = mlx5e_open_cq(c->priv, params->rx_cq_moderation, &cparam->rq.cqp, &ccp,
			    &c->xskrq.cq);
	if (unlikely(err))
		goto err_free_cparam;

	err = mlx5e_open_xsk_rq(c, params, &cparam->rq, pool, xsk);
	if (unlikely(err))
		goto err_close_rx_cq;

	err = mlx5e_open_cq(c->priv, params->tx_cq_moderation, &cparam->xdp_sq.cqp, &ccp,
			    &c->xsksq.cq);
	if (unlikely(err))
		goto err_close_rq;

	/* Create a separate SQ, so that when the buff pool is disabled, we could
	 * close this SQ safely and stop receiving CQEs. In other case, e.g., if
	 * the XDPSQ was used instead, we might run into trouble when the buff pool
	 * is disabled and then reenabled, but the SQ continues receiving CQEs
	 * from the old buff pool.
	 */
	err = mlx5e_open_xdpsq(c, params, &cparam->xdp_sq, pool, &c->xsksq, true);
	if (unlikely(err))
		goto err_close_tx_cq;

	kvfree(cparam);

	set_bit(MLX5E_CHANNEL_STATE_XSK, c->state);

	return 0;

err_close_tx_cq:
	mlx5e_close_cq(&c->xsksq.cq);

err_close_rq:
	mlx5e_close_rq(&c->xskrq);

err_close_rx_cq:
	mlx5e_close_cq(&c->xskrq.cq);

err_free_cparam:
	kvfree(cparam);

	return err;
}

void mlx5e_close_xsk(struct mlx5e_channel *c)
{
	clear_bit(MLX5E_CHANNEL_STATE_XSK, c->state);
	synchronize_net(); /* Sync with the XSK wakeup and with NAPI. */

	mlx5e_close_rq(&c->xskrq);
	mlx5e_close_cq(&c->xskrq.cq);
	mlx5e_close_xdpsq(&c->xsksq);
	mlx5e_close_cq(&c->xsksq.cq);

	memset(&c->xskrq, 0, sizeof(c->xskrq));
	memset(&c->xsksq, 0, sizeof(c->xsksq));
}

void mlx5e_activate_xsk(struct mlx5e_channel *c)
{
	set_bit(MLX5E_RQ_STATE_ENABLED, &c->xskrq.state);
	/* TX queue is created active. */

	spin_lock_bh(&c->async_icosq_lock);
	mlx5e_trigger_irq(&c->async_icosq);
	spin_unlock_bh(&c->async_icosq_lock);
}

void mlx5e_deactivate_xsk(struct mlx5e_channel *c)
{
	mlx5e_deactivate_rq(&c->xskrq);
	/* TX queue is disabled on close. */
}

static int mlx5e_redirect_xsk_rqt(struct mlx5e_priv *priv, u16 ix, u32 rqn)
{
	struct mlx5e_redirect_rqt_param direct_rrp = {
		.is_rss = false,
		{
			.rqn = rqn,
		},
	};

	u32 rqtn = priv->xsk_tir[ix].rqt.rqtn;

	return mlx5e_redirect_rqt(priv, rqtn, 1, direct_rrp);
}

int mlx5e_xsk_redirect_rqt_to_channel(struct mlx5e_priv *priv, struct mlx5e_channel *c)
{
	return mlx5e_redirect_xsk_rqt(priv, c->ix, c->xskrq.rqn);
}

int mlx5e_xsk_redirect_rqt_to_drop(struct mlx5e_priv *priv, u16 ix)
{
	return mlx5e_redirect_xsk_rqt(priv, ix, priv->drop_rq.rqn);
}

int mlx5e_xsk_redirect_rqts_to_channels(struct mlx5e_priv *priv, struct mlx5e_channels *chs)
{
	int err, i;

	if (!priv->xsk.refcnt)
		return 0;

	for (i = 0; i < chs->num; i++) {
		struct mlx5e_channel *c = chs->c[i];

		if (!test_bit(MLX5E_CHANNEL_STATE_XSK, c->state))
			continue;

		err = mlx5e_xsk_redirect_rqt_to_channel(priv, c);
		if (unlikely(err))
			goto err_stop;
	}

	return 0;

err_stop:
	for (i--; i >= 0; i--) {
		if (!test_bit(MLX5E_CHANNEL_STATE_XSK, chs->c[i]->state))
			continue;

		mlx5e_xsk_redirect_rqt_to_drop(priv, i);
	}

	return err;
}

void mlx5e_xsk_redirect_rqts_to_drop(struct mlx5e_priv *priv, struct mlx5e_channels *chs)
{
	int i;

	if (!priv->xsk.refcnt)
		return;

	for (i = 0; i < chs->num; i++) {
		if (!test_bit(MLX5E_CHANNEL_STATE_XSK, chs->c[i]->state))
			continue;

		mlx5e_xsk_redirect_rqt_to_drop(priv, i);
	}
}
