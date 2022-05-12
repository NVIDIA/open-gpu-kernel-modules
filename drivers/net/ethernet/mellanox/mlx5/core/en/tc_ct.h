/* SPDX-License-Identifier: GPL-2.0 OR Linux-OpenIB */
/* Copyright (c) 2018 Mellanox Technologies. */

#ifndef __MLX5_EN_TC_CT_H__
#define __MLX5_EN_TC_CT_H__

#include <net/pkt_cls.h>
#include <linux/mlx5/fs.h>
#include <net/tc_act/tc_ct.h>

#include "en.h"

struct mlx5_flow_attr;
struct mlx5e_tc_mod_hdr_acts;
struct mlx5_rep_uplink_priv;
struct mlx5e_tc_flow;
struct mlx5e_priv;

struct mlx5_fs_chains;
struct mlx5_tc_ct_priv;
struct mlx5_ct_flow;

struct nf_flowtable;

struct mlx5_ct_attr {
	u16 zone;
	u16 ct_action;
	struct mlx5_ct_flow *ct_flow;
	struct nf_flowtable *nf_ft;
	u32 ct_labels_id;
};

#define zone_to_reg_ct {\
	.mfield = MLX5_ACTION_IN_FIELD_METADATA_REG_C_2,\
	.moffset = 0,\
	.mlen = 2,\
	.soffset = MLX5_BYTE_OFF(fte_match_param,\
				 misc_parameters_2.metadata_reg_c_2) + 2,\
}

#define ctstate_to_reg_ct {\
	.mfield = MLX5_ACTION_IN_FIELD_METADATA_REG_C_2,\
	.moffset = 2,\
	.mlen = 2,\
	.soffset = MLX5_BYTE_OFF(fte_match_param,\
				 misc_parameters_2.metadata_reg_c_2),\
}

#define mark_to_reg_ct {\
	.mfield = MLX5_ACTION_IN_FIELD_METADATA_REG_C_3,\
	.moffset = 0,\
	.mlen = 4,\
	.soffset = MLX5_BYTE_OFF(fte_match_param,\
				 misc_parameters_2.metadata_reg_c_3),\
}

#define labels_to_reg_ct {\
	.mfield = MLX5_ACTION_IN_FIELD_METADATA_REG_C_4,\
	.moffset = 0,\
	.mlen = 4,\
	.soffset = MLX5_BYTE_OFF(fte_match_param,\
				 misc_parameters_2.metadata_reg_c_4),\
}

#define fteid_to_reg_ct {\
	.mfield = MLX5_ACTION_IN_FIELD_METADATA_REG_C_5,\
	.moffset = 0,\
	.mlen = 4,\
	.soffset = MLX5_BYTE_OFF(fte_match_param,\
				 misc_parameters_2.metadata_reg_c_5),\
}

#define zone_restore_to_reg_ct {\
	.mfield = MLX5_ACTION_IN_FIELD_METADATA_REG_C_1,\
	.moffset = 0,\
	.mlen = (ESW_ZONE_ID_BITS / 8),\
	.soffset = MLX5_BYTE_OFF(fte_match_param,\
				 misc_parameters_2.metadata_reg_c_1) + 3,\
}

#define nic_zone_restore_to_reg_ct {\
	.mfield = MLX5_ACTION_IN_FIELD_METADATA_REG_B,\
	.moffset = 2,\
	.mlen = (ESW_ZONE_ID_BITS / 8),\
}

#define REG_MAPPING_MLEN(reg) (mlx5e_tc_attr_to_reg_mappings[reg].mlen)
#define REG_MAPPING_MOFFSET(reg) (mlx5e_tc_attr_to_reg_mappings[reg].moffset)
#define REG_MAPPING_SHIFT(reg) (REG_MAPPING_MOFFSET(reg) * 8)

#if IS_ENABLED(CONFIG_MLX5_TC_CT)

struct mlx5_tc_ct_priv *
mlx5_tc_ct_init(struct mlx5e_priv *priv, struct mlx5_fs_chains *chains,
		struct mod_hdr_tbl *mod_hdr,
		enum mlx5_flow_namespace_type ns_type);
void
mlx5_tc_ct_clean(struct mlx5_tc_ct_priv *ct_priv);

void
mlx5_tc_ct_match_del(struct mlx5_tc_ct_priv *priv, struct mlx5_ct_attr *ct_attr);

int
mlx5_tc_ct_match_add(struct mlx5_tc_ct_priv *priv,
		     struct mlx5_flow_spec *spec,
		     struct flow_cls_offload *f,
		     struct mlx5_ct_attr *ct_attr,
		     struct netlink_ext_ack *extack);
int mlx5_tc_ct_add_no_trk_match(struct mlx5_flow_spec *spec);
int
mlx5_tc_ct_parse_action(struct mlx5_tc_ct_priv *priv,
			struct mlx5_flow_attr *attr,
			const struct flow_action_entry *act,
			struct netlink_ext_ack *extack);

struct mlx5_flow_handle *
mlx5_tc_ct_flow_offload(struct mlx5_tc_ct_priv *priv,
			struct mlx5e_tc_flow *flow,
			struct mlx5_flow_spec *spec,
			struct mlx5_flow_attr *attr,
			struct mlx5e_tc_mod_hdr_acts *mod_hdr_acts);
void
mlx5_tc_ct_delete_flow(struct mlx5_tc_ct_priv *priv,
		       struct mlx5e_tc_flow *flow,
		       struct mlx5_flow_attr *attr);

bool
mlx5e_tc_ct_restore_flow(struct mlx5_tc_ct_priv *ct_priv,
			 struct sk_buff *skb, u8 zone_restore_id);

#else /* CONFIG_MLX5_TC_CT */

static inline struct mlx5_tc_ct_priv *
mlx5_tc_ct_init(struct mlx5e_priv *priv, struct mlx5_fs_chains *chains,
		struct mod_hdr_tbl *mod_hdr,
		enum mlx5_flow_namespace_type ns_type)
{
	return NULL;
}

static inline void
mlx5_tc_ct_clean(struct mlx5_tc_ct_priv *ct_priv)
{
}

static inline void
mlx5_tc_ct_match_del(struct mlx5_tc_ct_priv *priv, struct mlx5_ct_attr *ct_attr) {}

static inline int
mlx5_tc_ct_match_add(struct mlx5_tc_ct_priv *priv,
		     struct mlx5_flow_spec *spec,
		     struct flow_cls_offload *f,
		     struct mlx5_ct_attr *ct_attr,
		     struct netlink_ext_ack *extack)
{
	struct flow_rule *rule = flow_cls_offload_flow_rule(f);

	if (!flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_CT))
		return 0;

	NL_SET_ERR_MSG_MOD(extack, "mlx5 tc ct offload isn't enabled.");
	return -EOPNOTSUPP;
}

static inline int
mlx5_tc_ct_add_no_trk_match(struct mlx5_flow_spec *spec)
{
	return 0;
}

static inline int
mlx5_tc_ct_parse_action(struct mlx5_tc_ct_priv *priv,
			struct mlx5_flow_attr *attr,
			const struct flow_action_entry *act,
			struct netlink_ext_ack *extack)
{
	NL_SET_ERR_MSG_MOD(extack, "mlx5 tc ct offload isn't enabled.");
	return -EOPNOTSUPP;
}

static inline struct mlx5_flow_handle *
mlx5_tc_ct_flow_offload(struct mlx5_tc_ct_priv *priv,
			struct mlx5e_tc_flow *flow,
			struct mlx5_flow_spec *spec,
			struct mlx5_flow_attr *attr,
			struct mlx5e_tc_mod_hdr_acts *mod_hdr_acts)
{
	return ERR_PTR(-EOPNOTSUPP);
}

static inline void
mlx5_tc_ct_delete_flow(struct mlx5_tc_ct_priv *priv,
		       struct mlx5e_tc_flow *flow,
		       struct mlx5_flow_attr *attr)
{
}

static inline bool
mlx5e_tc_ct_restore_flow(struct mlx5_tc_ct_priv *ct_priv,
			 struct sk_buff *skb, u8 zone_restore_id)
{
	if (!zone_restore_id)
		return true;

	return false;
}

#endif /* !IS_ENABLED(CONFIG_MLX5_TC_CT) */
#endif /* __MLX5_EN_TC_CT_H__ */
