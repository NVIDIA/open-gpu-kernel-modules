/* SPDX-License-Identifier: GPL-2.0 OR Linux-OpenIB */
/* Copyright (c) 2021 Mellanox Technologies. */

#ifndef __MLX5_EN_TC_SAMPLE_H__
#define __MLX5_EN_TC_SAMPLE_H__

#include "en.h"
#include "eswitch.h"

struct mlx5e_priv;
struct mlx5_flow_attr;
struct mlx5_esw_psample;

struct mlx5_sample_attr {
	u32 group_num;
	u32 rate;
	u32 trunc_size;
	u32 restore_obj_id;
	u32 sampler_id;
	struct mlx5_flow_table *sample_default_tbl;
	struct mlx5_sample_flow *sample_flow;
};

void mlx5_esw_sample_skb(struct sk_buff *skb, struct mlx5_mapped_obj *mapped_obj);

struct mlx5_flow_handle *
mlx5_esw_sample_offload(struct mlx5_esw_psample *sample_priv,
			struct mlx5_flow_spec *spec,
			struct mlx5_flow_attr *attr);

void
mlx5_esw_sample_unoffload(struct mlx5_esw_psample *sample_priv,
			  struct mlx5_flow_handle *rule,
			  struct mlx5_flow_attr *attr);

struct mlx5_esw_psample *
mlx5_esw_sample_init(struct mlx5e_priv *priv);

void
mlx5_esw_sample_cleanup(struct mlx5_esw_psample *esw_psample);

#endif /* __MLX5_EN_TC_SAMPLE_H__ */
