/*
 * Copyright (c) 2017 Mellanox Technologies. All rights reserved.
 *
 * This software is available to you under a choice of one of two
 * licenses.  You may choose to be licensed under the terms of the GNU
 * General Public License (GPL) Version 2, available from the file
 * COPYING in the main directory of this source tree, or the
 * OpenIB.org BSD license below:
 *
 *     Redistribution and use in source and binary forms, with or
 *     without modification, are permitted provided that the following
 *     conditions are met:
 *
 *      - Redistributions of source code must retain the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer.
 *
 *      - Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer in the documentation and/or other materials
 *        provided with the distribution.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <linux/ethtool.h>
#include <net/sock.h>

#include "en.h"
#include "accel/ipsec.h"
#include "fpga/sdk.h"
#include "en_accel/ipsec.h"
#include "fpga/ipsec.h"

static const struct counter_desc mlx5e_ipsec_hw_stats_desc[] = {
	{ MLX5E_DECLARE_STAT(struct mlx5e_ipsec_stats, ipsec_dec_in_packets) },
	{ MLX5E_DECLARE_STAT(struct mlx5e_ipsec_stats, ipsec_dec_out_packets) },
	{ MLX5E_DECLARE_STAT(struct mlx5e_ipsec_stats, ipsec_dec_bypass_packets) },
	{ MLX5E_DECLARE_STAT(struct mlx5e_ipsec_stats, ipsec_enc_in_packets) },
	{ MLX5E_DECLARE_STAT(struct mlx5e_ipsec_stats, ipsec_enc_out_packets) },
	{ MLX5E_DECLARE_STAT(struct mlx5e_ipsec_stats, ipsec_enc_bypass_packets) },
	{ MLX5E_DECLARE_STAT(struct mlx5e_ipsec_stats, ipsec_dec_drop_packets) },
	{ MLX5E_DECLARE_STAT(struct mlx5e_ipsec_stats, ipsec_dec_auth_fail_packets) },
	{ MLX5E_DECLARE_STAT(struct mlx5e_ipsec_stats, ipsec_enc_drop_packets) },
	{ MLX5E_DECLARE_STAT(struct mlx5e_ipsec_stats, ipsec_add_sa_success) },
	{ MLX5E_DECLARE_STAT(struct mlx5e_ipsec_stats, ipsec_add_sa_fail) },
	{ MLX5E_DECLARE_STAT(struct mlx5e_ipsec_stats, ipsec_del_sa_success) },
	{ MLX5E_DECLARE_STAT(struct mlx5e_ipsec_stats, ipsec_del_sa_fail) },
	{ MLX5E_DECLARE_STAT(struct mlx5e_ipsec_stats, ipsec_cmd_drop) },
};

static const struct counter_desc mlx5e_ipsec_sw_stats_desc[] = {
	{ MLX5E_DECLARE_STAT(struct mlx5e_ipsec_sw_stats, ipsec_rx_drop_sp_alloc) },
	{ MLX5E_DECLARE_STAT(struct mlx5e_ipsec_sw_stats, ipsec_rx_drop_sadb_miss) },
	{ MLX5E_DECLARE_STAT(struct mlx5e_ipsec_sw_stats, ipsec_rx_drop_syndrome) },
	{ MLX5E_DECLARE_STAT(struct mlx5e_ipsec_sw_stats, ipsec_tx_drop_bundle) },
	{ MLX5E_DECLARE_STAT(struct mlx5e_ipsec_sw_stats, ipsec_tx_drop_no_state) },
	{ MLX5E_DECLARE_STAT(struct mlx5e_ipsec_sw_stats, ipsec_tx_drop_not_ip) },
	{ MLX5E_DECLARE_STAT(struct mlx5e_ipsec_sw_stats, ipsec_tx_drop_trailer) },
	{ MLX5E_DECLARE_STAT(struct mlx5e_ipsec_sw_stats, ipsec_tx_drop_metadata) },
};

#define MLX5E_READ_CTR_ATOMIC64(ptr, dsc, i) \
	atomic64_read((atomic64_t *)((char *)(ptr) + (dsc)[i].offset))

#define NUM_IPSEC_HW_COUNTERS ARRAY_SIZE(mlx5e_ipsec_hw_stats_desc)
#define NUM_IPSEC_SW_COUNTERS ARRAY_SIZE(mlx5e_ipsec_sw_stats_desc)

static MLX5E_DECLARE_STATS_GRP_OP_NUM_STATS(ipsec_sw)
{
	return priv->ipsec ? NUM_IPSEC_SW_COUNTERS : 0;
}

static inline MLX5E_DECLARE_STATS_GRP_OP_UPDATE_STATS(ipsec_sw) {}

static MLX5E_DECLARE_STATS_GRP_OP_FILL_STRS(ipsec_sw)
{
	unsigned int i;

	if (priv->ipsec)
		for (i = 0; i < NUM_IPSEC_SW_COUNTERS; i++)
			strcpy(data + (idx++) * ETH_GSTRING_LEN,
			       mlx5e_ipsec_sw_stats_desc[i].format);
	return idx;
}

static MLX5E_DECLARE_STATS_GRP_OP_FILL_STATS(ipsec_sw)
{
	int i;

	if (priv->ipsec)
		for (i = 0; i < NUM_IPSEC_SW_COUNTERS; i++)
			data[idx++] = MLX5E_READ_CTR_ATOMIC64(&priv->ipsec->sw_stats,
							      mlx5e_ipsec_sw_stats_desc, i);
	return idx;
}

static MLX5E_DECLARE_STATS_GRP_OP_NUM_STATS(ipsec_hw)
{
	return (priv->ipsec && mlx5_fpga_ipsec_device_caps(priv->mdev)) ? NUM_IPSEC_HW_COUNTERS : 0;
}

static MLX5E_DECLARE_STATS_GRP_OP_UPDATE_STATS(ipsec_hw)
{
	int ret = 0;

	if (priv->ipsec)
		ret = mlx5_accel_ipsec_counters_read(priv->mdev, (u64 *)&priv->ipsec->stats,
						     NUM_IPSEC_HW_COUNTERS);
	if (ret)
		memset(&priv->ipsec->stats, 0, sizeof(priv->ipsec->stats));
}

static MLX5E_DECLARE_STATS_GRP_OP_FILL_STRS(ipsec_hw)
{
	unsigned int i;

	if (priv->ipsec && mlx5_fpga_ipsec_device_caps(priv->mdev))
		for (i = 0; i < NUM_IPSEC_HW_COUNTERS; i++)
			strcpy(data + (idx++) * ETH_GSTRING_LEN,
			       mlx5e_ipsec_hw_stats_desc[i].format);

	return idx;
}

static MLX5E_DECLARE_STATS_GRP_OP_FILL_STATS(ipsec_hw)
{
	int i;

	if (priv->ipsec && mlx5_fpga_ipsec_device_caps(priv->mdev))
		for (i = 0; i < NUM_IPSEC_HW_COUNTERS; i++)
			data[idx++] = MLX5E_READ_CTR64_CPU(&priv->ipsec->stats,
							   mlx5e_ipsec_hw_stats_desc,
							   i);
	return idx;
}

MLX5E_DEFINE_STATS_GRP(ipsec_sw, 0);
MLX5E_DEFINE_STATS_GRP(ipsec_hw, 0);
