// SPDX-License-Identifier: GPL-2.0 OR Linux-OpenIB
/* Copyright (c) 2019 Mellanox Technologies. */

#include "dr_types.h"

enum dr_action_domain {
	DR_ACTION_DOMAIN_NIC_INGRESS,
	DR_ACTION_DOMAIN_NIC_EGRESS,
	DR_ACTION_DOMAIN_FDB_INGRESS,
	DR_ACTION_DOMAIN_FDB_EGRESS,
	DR_ACTION_DOMAIN_MAX,
};

enum dr_action_valid_state {
	DR_ACTION_STATE_ERR,
	DR_ACTION_STATE_NO_ACTION,
	DR_ACTION_STATE_REFORMAT,
	DR_ACTION_STATE_MODIFY_HDR,
	DR_ACTION_STATE_MODIFY_VLAN,
	DR_ACTION_STATE_NON_TERM,
	DR_ACTION_STATE_TERM,
	DR_ACTION_STATE_MAX,
};

static const enum dr_action_valid_state
next_action_state[DR_ACTION_DOMAIN_MAX][DR_ACTION_STATE_MAX][DR_ACTION_TYP_MAX] = {
	[DR_ACTION_DOMAIN_NIC_INGRESS] = {
		[DR_ACTION_STATE_NO_ACTION] = {
			[DR_ACTION_TYP_DROP]		= DR_ACTION_STATE_TERM,
			[DR_ACTION_TYP_QP]		= DR_ACTION_STATE_TERM,
			[DR_ACTION_TYP_FT]		= DR_ACTION_STATE_TERM,
			[DR_ACTION_TYP_TAG]		= DR_ACTION_STATE_NON_TERM,
			[DR_ACTION_TYP_CTR]		= DR_ACTION_STATE_NON_TERM,
			[DR_ACTION_TYP_TNL_L2_TO_L2]	= DR_ACTION_STATE_REFORMAT,
			[DR_ACTION_TYP_TNL_L3_TO_L2]	= DR_ACTION_STATE_REFORMAT,
			[DR_ACTION_TYP_MODIFY_HDR]	= DR_ACTION_STATE_MODIFY_HDR,
			[DR_ACTION_TYP_POP_VLAN]	= DR_ACTION_STATE_MODIFY_VLAN,
		},
		[DR_ACTION_STATE_REFORMAT] = {
			[DR_ACTION_TYP_DROP]		= DR_ACTION_STATE_TERM,
			[DR_ACTION_TYP_QP]		= DR_ACTION_STATE_TERM,
			[DR_ACTION_TYP_FT]		= DR_ACTION_STATE_TERM,
			[DR_ACTION_TYP_TAG]		= DR_ACTION_STATE_REFORMAT,
			[DR_ACTION_TYP_CTR]		= DR_ACTION_STATE_REFORMAT,
			[DR_ACTION_TYP_MODIFY_HDR]	= DR_ACTION_STATE_MODIFY_HDR,
			[DR_ACTION_TYP_POP_VLAN]	= DR_ACTION_STATE_MODIFY_VLAN,
		},
		[DR_ACTION_STATE_MODIFY_HDR] = {
			[DR_ACTION_TYP_DROP]		= DR_ACTION_STATE_TERM,
			[DR_ACTION_TYP_QP]		= DR_ACTION_STATE_TERM,
			[DR_ACTION_TYP_FT]		= DR_ACTION_STATE_TERM,
			[DR_ACTION_TYP_TAG]		= DR_ACTION_STATE_MODIFY_HDR,
			[DR_ACTION_TYP_CTR]		= DR_ACTION_STATE_MODIFY_HDR,
		},
		[DR_ACTION_STATE_MODIFY_VLAN] = {
			[DR_ACTION_TYP_DROP]		= DR_ACTION_STATE_TERM,
			[DR_ACTION_TYP_QP]		= DR_ACTION_STATE_TERM,
			[DR_ACTION_TYP_FT]		= DR_ACTION_STATE_TERM,
			[DR_ACTION_TYP_TAG]		= DR_ACTION_STATE_MODIFY_VLAN,
			[DR_ACTION_TYP_CTR]		= DR_ACTION_STATE_MODIFY_VLAN,
			[DR_ACTION_TYP_POP_VLAN]	= DR_ACTION_STATE_MODIFY_VLAN,
			[DR_ACTION_TYP_MODIFY_HDR]	= DR_ACTION_STATE_MODIFY_HDR,
		},
		[DR_ACTION_STATE_NON_TERM] = {
			[DR_ACTION_TYP_DROP]		= DR_ACTION_STATE_TERM,
			[DR_ACTION_TYP_QP]		= DR_ACTION_STATE_TERM,
			[DR_ACTION_TYP_FT]		= DR_ACTION_STATE_TERM,
			[DR_ACTION_TYP_TAG]		= DR_ACTION_STATE_NON_TERM,
			[DR_ACTION_TYP_CTR]		= DR_ACTION_STATE_NON_TERM,
			[DR_ACTION_TYP_TNL_L2_TO_L2]	= DR_ACTION_STATE_REFORMAT,
			[DR_ACTION_TYP_TNL_L3_TO_L2]	= DR_ACTION_STATE_REFORMAT,
			[DR_ACTION_TYP_MODIFY_HDR]	= DR_ACTION_STATE_MODIFY_HDR,
			[DR_ACTION_TYP_POP_VLAN]	= DR_ACTION_STATE_MODIFY_VLAN,
		},
		[DR_ACTION_STATE_TERM] = {
			[DR_ACTION_TYP_CTR]		= DR_ACTION_STATE_TERM,
		},
	},
	[DR_ACTION_DOMAIN_NIC_EGRESS] = {
		[DR_ACTION_STATE_NO_ACTION] = {
			[DR_ACTION_TYP_DROP]		= DR_ACTION_STATE_TERM,
			[DR_ACTION_TYP_FT]		= DR_ACTION_STATE_TERM,
			[DR_ACTION_TYP_CTR]		= DR_ACTION_STATE_NON_TERM,
			[DR_ACTION_TYP_L2_TO_TNL_L2]	= DR_ACTION_STATE_REFORMAT,
			[DR_ACTION_TYP_L2_TO_TNL_L3]	= DR_ACTION_STATE_REFORMAT,
			[DR_ACTION_TYP_MODIFY_HDR]	= DR_ACTION_STATE_MODIFY_HDR,
			[DR_ACTION_TYP_PUSH_VLAN]	= DR_ACTION_STATE_MODIFY_VLAN,
		},
		[DR_ACTION_STATE_REFORMAT] = {
			[DR_ACTION_TYP_DROP]		= DR_ACTION_STATE_TERM,
			[DR_ACTION_TYP_FT]		= DR_ACTION_STATE_TERM,
			[DR_ACTION_TYP_CTR]		= DR_ACTION_STATE_REFORMAT,
		},
		[DR_ACTION_STATE_MODIFY_HDR] = {
			[DR_ACTION_TYP_DROP]		= DR_ACTION_STATE_TERM,
			[DR_ACTION_TYP_FT]		= DR_ACTION_STATE_TERM,
			[DR_ACTION_TYP_CTR]		= DR_ACTION_STATE_MODIFY_HDR,
			[DR_ACTION_TYP_L2_TO_TNL_L2]	= DR_ACTION_STATE_REFORMAT,
			[DR_ACTION_TYP_L2_TO_TNL_L3]	= DR_ACTION_STATE_REFORMAT,
			[DR_ACTION_TYP_PUSH_VLAN]	= DR_ACTION_STATE_MODIFY_VLAN,
		},
		[DR_ACTION_STATE_MODIFY_VLAN] = {
			[DR_ACTION_TYP_DROP]		= DR_ACTION_STATE_TERM,
			[DR_ACTION_TYP_FT]		= DR_ACTION_STATE_TERM,
			[DR_ACTION_TYP_CTR]		= DR_ACTION_STATE_MODIFY_VLAN,
			[DR_ACTION_TYP_PUSH_VLAN]	= DR_ACTION_STATE_MODIFY_VLAN,
			[DR_ACTION_TYP_L2_TO_TNL_L2]	= DR_ACTION_STATE_REFORMAT,
			[DR_ACTION_TYP_L2_TO_TNL_L3]	= DR_ACTION_STATE_REFORMAT,
		},
		[DR_ACTION_STATE_NON_TERM] = {
			[DR_ACTION_TYP_DROP]		= DR_ACTION_STATE_TERM,
			[DR_ACTION_TYP_FT]		= DR_ACTION_STATE_TERM,
			[DR_ACTION_TYP_CTR]		= DR_ACTION_STATE_NON_TERM,
			[DR_ACTION_TYP_L2_TO_TNL_L2]	= DR_ACTION_STATE_REFORMAT,
			[DR_ACTION_TYP_L2_TO_TNL_L3]	= DR_ACTION_STATE_REFORMAT,
			[DR_ACTION_TYP_MODIFY_HDR]	= DR_ACTION_STATE_MODIFY_HDR,
			[DR_ACTION_TYP_PUSH_VLAN]	= DR_ACTION_STATE_MODIFY_VLAN,
		},
		[DR_ACTION_STATE_TERM] = {
			[DR_ACTION_TYP_CTR]		= DR_ACTION_STATE_TERM,
		},
	},
	[DR_ACTION_DOMAIN_FDB_INGRESS] = {
		[DR_ACTION_STATE_NO_ACTION] = {
			[DR_ACTION_TYP_DROP]		= DR_ACTION_STATE_TERM,
			[DR_ACTION_TYP_FT]		= DR_ACTION_STATE_TERM,
			[DR_ACTION_TYP_CTR]		= DR_ACTION_STATE_NON_TERM,
			[DR_ACTION_TYP_TNL_L2_TO_L2]	= DR_ACTION_STATE_REFORMAT,
			[DR_ACTION_TYP_TNL_L3_TO_L2]	= DR_ACTION_STATE_REFORMAT,
			[DR_ACTION_TYP_MODIFY_HDR]	= DR_ACTION_STATE_MODIFY_HDR,
			[DR_ACTION_TYP_POP_VLAN]	= DR_ACTION_STATE_MODIFY_VLAN,
			[DR_ACTION_TYP_VPORT]		= DR_ACTION_STATE_TERM,
		},
		[DR_ACTION_STATE_REFORMAT] = {
			[DR_ACTION_TYP_DROP]		= DR_ACTION_STATE_TERM,
			[DR_ACTION_TYP_FT]		= DR_ACTION_STATE_TERM,
			[DR_ACTION_TYP_CTR]		= DR_ACTION_STATE_REFORMAT,
			[DR_ACTION_TYP_MODIFY_HDR]	= DR_ACTION_STATE_MODIFY_HDR,
			[DR_ACTION_TYP_POP_VLAN]	= DR_ACTION_STATE_MODIFY_VLAN,
			[DR_ACTION_TYP_VPORT]		= DR_ACTION_STATE_TERM,
		},
		[DR_ACTION_STATE_MODIFY_HDR] = {
			[DR_ACTION_TYP_DROP]		= DR_ACTION_STATE_TERM,
			[DR_ACTION_TYP_FT]		= DR_ACTION_STATE_TERM,
			[DR_ACTION_TYP_CTR]		= DR_ACTION_STATE_MODIFY_HDR,
			[DR_ACTION_TYP_VPORT]		= DR_ACTION_STATE_TERM,
		},
		[DR_ACTION_STATE_MODIFY_VLAN] = {
			[DR_ACTION_TYP_DROP]		= DR_ACTION_STATE_TERM,
			[DR_ACTION_TYP_FT]		= DR_ACTION_STATE_TERM,
			[DR_ACTION_TYP_POP_VLAN]	= DR_ACTION_STATE_MODIFY_VLAN,
			[DR_ACTION_TYP_CTR]		= DR_ACTION_STATE_MODIFY_VLAN,
			[DR_ACTION_TYP_VPORT]		= DR_ACTION_STATE_TERM,
			[DR_ACTION_TYP_MODIFY_HDR]	= DR_ACTION_STATE_MODIFY_HDR,
		},
		[DR_ACTION_STATE_NON_TERM] = {
			[DR_ACTION_TYP_DROP]		= DR_ACTION_STATE_TERM,
			[DR_ACTION_TYP_FT]		= DR_ACTION_STATE_TERM,
			[DR_ACTION_TYP_CTR]		= DR_ACTION_STATE_NON_TERM,
			[DR_ACTION_TYP_TNL_L2_TO_L2]	= DR_ACTION_STATE_REFORMAT,
			[DR_ACTION_TYP_TNL_L3_TO_L2]	= DR_ACTION_STATE_REFORMAT,
			[DR_ACTION_TYP_MODIFY_HDR]	= DR_ACTION_STATE_MODIFY_HDR,
			[DR_ACTION_TYP_POP_VLAN]	= DR_ACTION_STATE_MODIFY_VLAN,
			[DR_ACTION_TYP_VPORT]		= DR_ACTION_STATE_TERM,
		},
		[DR_ACTION_STATE_TERM] = {
			[DR_ACTION_TYP_CTR]		= DR_ACTION_STATE_TERM,
		},
	},
	[DR_ACTION_DOMAIN_FDB_EGRESS] = {
		[DR_ACTION_STATE_NO_ACTION] = {
			[DR_ACTION_TYP_DROP]		= DR_ACTION_STATE_TERM,
			[DR_ACTION_TYP_FT]		= DR_ACTION_STATE_TERM,
			[DR_ACTION_TYP_CTR]		= DR_ACTION_STATE_NON_TERM,
			[DR_ACTION_TYP_MODIFY_HDR]	= DR_ACTION_STATE_MODIFY_HDR,
			[DR_ACTION_TYP_L2_TO_TNL_L2]	= DR_ACTION_STATE_REFORMAT,
			[DR_ACTION_TYP_L2_TO_TNL_L3]	= DR_ACTION_STATE_REFORMAT,
			[DR_ACTION_TYP_PUSH_VLAN]	= DR_ACTION_STATE_MODIFY_VLAN,
			[DR_ACTION_TYP_VPORT]		= DR_ACTION_STATE_TERM,
		},
		[DR_ACTION_STATE_REFORMAT] = {
			[DR_ACTION_TYP_DROP]		= DR_ACTION_STATE_TERM,
			[DR_ACTION_TYP_FT]		= DR_ACTION_STATE_TERM,
			[DR_ACTION_TYP_CTR]		= DR_ACTION_STATE_REFORMAT,
			[DR_ACTION_TYP_VPORT]		= DR_ACTION_STATE_TERM,
		},
		[DR_ACTION_STATE_MODIFY_HDR] = {
			[DR_ACTION_TYP_DROP]		= DR_ACTION_STATE_TERM,
			[DR_ACTION_TYP_FT]		= DR_ACTION_STATE_TERM,
			[DR_ACTION_TYP_CTR]		= DR_ACTION_STATE_MODIFY_HDR,
			[DR_ACTION_TYP_L2_TO_TNL_L2]	= DR_ACTION_STATE_REFORMAT,
			[DR_ACTION_TYP_L2_TO_TNL_L3]	= DR_ACTION_STATE_REFORMAT,
			[DR_ACTION_TYP_PUSH_VLAN]	= DR_ACTION_STATE_MODIFY_VLAN,
			[DR_ACTION_TYP_VPORT]		= DR_ACTION_STATE_TERM,
		},
		[DR_ACTION_STATE_MODIFY_VLAN] = {
			[DR_ACTION_TYP_DROP]		= DR_ACTION_STATE_TERM,
			[DR_ACTION_TYP_FT]		= DR_ACTION_STATE_TERM,
			[DR_ACTION_TYP_PUSH_VLAN]	= DR_ACTION_STATE_MODIFY_VLAN,
			[DR_ACTION_TYP_CTR]		= DR_ACTION_STATE_MODIFY_VLAN,
			[DR_ACTION_TYP_L2_TO_TNL_L2]	= DR_ACTION_STATE_REFORMAT,
			[DR_ACTION_TYP_L2_TO_TNL_L3]	= DR_ACTION_STATE_REFORMAT,
			[DR_ACTION_TYP_VPORT]		= DR_ACTION_STATE_TERM,
		},
		[DR_ACTION_STATE_NON_TERM] = {
			[DR_ACTION_TYP_DROP]		= DR_ACTION_STATE_TERM,
			[DR_ACTION_TYP_FT]		= DR_ACTION_STATE_TERM,
			[DR_ACTION_TYP_CTR]		= DR_ACTION_STATE_NON_TERM,
			[DR_ACTION_TYP_MODIFY_HDR]	= DR_ACTION_STATE_MODIFY_HDR,
			[DR_ACTION_TYP_L2_TO_TNL_L2]	= DR_ACTION_STATE_REFORMAT,
			[DR_ACTION_TYP_L2_TO_TNL_L3]	= DR_ACTION_STATE_REFORMAT,
			[DR_ACTION_TYP_PUSH_VLAN]	= DR_ACTION_STATE_MODIFY_VLAN,
			[DR_ACTION_TYP_VPORT]		= DR_ACTION_STATE_TERM,
		},
		[DR_ACTION_STATE_TERM] = {
			[DR_ACTION_TYP_CTR]		= DR_ACTION_STATE_TERM,
		},
	},
};

static int
dr_action_reformat_to_action_type(enum mlx5dr_action_reformat_type reformat_type,
				  enum mlx5dr_action_type *action_type)
{
	switch (reformat_type) {
	case DR_ACTION_REFORMAT_TYP_TNL_L2_TO_L2:
		*action_type = DR_ACTION_TYP_TNL_L2_TO_L2;
		break;
	case DR_ACTION_REFORMAT_TYP_L2_TO_TNL_L2:
		*action_type = DR_ACTION_TYP_L2_TO_TNL_L2;
		break;
	case DR_ACTION_REFORMAT_TYP_TNL_L3_TO_L2:
		*action_type = DR_ACTION_TYP_TNL_L3_TO_L2;
		break;
	case DR_ACTION_REFORMAT_TYP_L2_TO_TNL_L3:
		*action_type = DR_ACTION_TYP_L2_TO_TNL_L3;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

/* Apply the actions on the rule STE array starting from the last_ste.
 * Actions might require more than one STE, new_num_stes will return
 * the new size of the STEs array, rule with actions.
 */
static void dr_actions_apply(struct mlx5dr_domain *dmn,
			     enum mlx5dr_ste_entry_type ste_type,
			     u8 *action_type_set,
			     u8 *last_ste,
			     struct mlx5dr_ste_actions_attr *attr,
			     u32 *new_num_stes)
{
	struct mlx5dr_ste_ctx *ste_ctx = dmn->ste_ctx;
	u32 added_stes = 0;

	if (ste_type == MLX5DR_STE_TYPE_RX)
		mlx5dr_ste_set_actions_rx(ste_ctx, dmn, action_type_set,
					  last_ste, attr, &added_stes);
	else
		mlx5dr_ste_set_actions_tx(ste_ctx, dmn, action_type_set,
					  last_ste, attr, &added_stes);

	*new_num_stes += added_stes;
}

static enum dr_action_domain
dr_action_get_action_domain(enum mlx5dr_domain_type domain,
			    enum mlx5dr_ste_entry_type ste_type)
{
	switch (domain) {
	case MLX5DR_DOMAIN_TYPE_NIC_RX:
		return DR_ACTION_DOMAIN_NIC_INGRESS;
	case MLX5DR_DOMAIN_TYPE_NIC_TX:
		return DR_ACTION_DOMAIN_NIC_EGRESS;
	case MLX5DR_DOMAIN_TYPE_FDB:
		if (ste_type == MLX5DR_STE_TYPE_RX)
			return DR_ACTION_DOMAIN_FDB_INGRESS;
		return DR_ACTION_DOMAIN_FDB_EGRESS;
	default:
		WARN_ON(true);
		return DR_ACTION_DOMAIN_MAX;
	}
}

static
int dr_action_validate_and_get_next_state(enum dr_action_domain action_domain,
					  u32 action_type,
					  u32 *state)
{
	u32 cur_state = *state;

	/* Check action state machine is valid */
	*state = next_action_state[action_domain][cur_state][action_type];

	if (*state == DR_ACTION_STATE_ERR)
		return -EOPNOTSUPP;

	return 0;
}

static int dr_action_handle_cs_recalc(struct mlx5dr_domain *dmn,
				      struct mlx5dr_action *dest_action,
				      u64 *final_icm_addr)
{
	int ret;

	switch (dest_action->action_type) {
	case DR_ACTION_TYP_FT:
		/* Allow destination flow table only if table is a terminating
		 * table, since there is an *assumption* that in such case FW
		 * will recalculate the CS.
		 */
		if (dest_action->dest_tbl->is_fw_tbl) {
			*final_icm_addr = dest_action->dest_tbl->fw_tbl.rx_icm_addr;
		} else {
			mlx5dr_dbg(dmn,
				   "Destination FT should be terminating when modify TTL is used\n");
			return -EINVAL;
		}
		break;

	case DR_ACTION_TYP_VPORT:
		/* If destination is vport we will get the FW flow table
		 * that recalculates the CS and forwards to the vport.
		 */
		ret = mlx5dr_domain_cache_get_recalc_cs_ft_addr(dest_action->vport->dmn,
								dest_action->vport->caps->num,
								final_icm_addr);
		if (ret) {
			mlx5dr_err(dmn, "Failed to get FW cs recalc flow table\n");
			return ret;
		}
		break;

	default:
		break;
	}

	return 0;
}

#define WITH_VLAN_NUM_HW_ACTIONS 6

int mlx5dr_actions_build_ste_arr(struct mlx5dr_matcher *matcher,
				 struct mlx5dr_matcher_rx_tx *nic_matcher,
				 struct mlx5dr_action *actions[],
				 u32 num_actions,
				 u8 *ste_arr,
				 u32 *new_hw_ste_arr_sz)
{
	struct mlx5dr_domain_rx_tx *nic_dmn = nic_matcher->nic_tbl->nic_dmn;
	bool rx_rule = nic_dmn->ste_type == MLX5DR_STE_TYPE_RX;
	struct mlx5dr_domain *dmn = matcher->tbl->dmn;
	u8 action_type_set[DR_ACTION_TYP_MAX] = {};
	struct mlx5dr_ste_actions_attr attr = {};
	struct mlx5dr_action *dest_action = NULL;
	u32 state = DR_ACTION_STATE_NO_ACTION;
	enum dr_action_domain action_domain;
	bool recalc_cs_required = false;
	u8 *last_ste;
	int i, ret;

	attr.gvmi = dmn->info.caps.gvmi;
	attr.hit_gvmi = dmn->info.caps.gvmi;
	attr.final_icm_addr = nic_dmn->default_icm_addr;
	action_domain = dr_action_get_action_domain(dmn->type, nic_dmn->ste_type);

	for (i = 0; i < num_actions; i++) {
		struct mlx5dr_action_dest_tbl *dest_tbl;
		struct mlx5dr_action *action;
		int max_actions_type = 1;
		u32 action_type;

		action = actions[i];
		action_type = action->action_type;

		switch (action_type) {
		case DR_ACTION_TYP_DROP:
			attr.final_icm_addr = nic_dmn->drop_icm_addr;
			break;
		case DR_ACTION_TYP_FT:
			dest_action = action;
			dest_tbl = action->dest_tbl;
			if (!dest_tbl->is_fw_tbl) {
				if (dest_tbl->tbl->dmn != dmn) {
					mlx5dr_err(dmn,
						   "Destination table belongs to a different domain\n");
					goto out_invalid_arg;
				}
				if (dest_tbl->tbl->level <= matcher->tbl->level) {
					mlx5_core_warn_once(dmn->mdev,
							    "Connecting table to a lower/same level destination table\n");
					mlx5dr_dbg(dmn,
						   "Connecting table at level %d to a destination table at level %d\n",
						   matcher->tbl->level,
						   dest_tbl->tbl->level);
				}
				attr.final_icm_addr = rx_rule ?
					dest_tbl->tbl->rx.s_anchor->chunk->icm_addr :
					dest_tbl->tbl->tx.s_anchor->chunk->icm_addr;
			} else {
				struct mlx5dr_cmd_query_flow_table_details output;
				int ret;

				/* get the relevant addresses */
				if (!action->dest_tbl->fw_tbl.rx_icm_addr) {
					ret = mlx5dr_cmd_query_flow_table(dmn->mdev,
									  dest_tbl->fw_tbl.type,
									  dest_tbl->fw_tbl.id,
									  &output);
					if (!ret) {
						dest_tbl->fw_tbl.tx_icm_addr =
							output.sw_owner_icm_root_1;
						dest_tbl->fw_tbl.rx_icm_addr =
							output.sw_owner_icm_root_0;
					} else {
						mlx5dr_err(dmn,
							   "Failed mlx5_cmd_query_flow_table ret: %d\n",
							   ret);
						return ret;
					}
				}
				attr.final_icm_addr = rx_rule ?
					dest_tbl->fw_tbl.rx_icm_addr :
					dest_tbl->fw_tbl.tx_icm_addr;
			}
			break;
		case DR_ACTION_TYP_QP:
			mlx5dr_info(dmn, "Domain doesn't support QP\n");
			goto out_invalid_arg;
		case DR_ACTION_TYP_CTR:
			attr.ctr_id = action->ctr->ctr_id +
				action->ctr->offeset;
			break;
		case DR_ACTION_TYP_TAG:
			attr.flow_tag = action->flow_tag->flow_tag;
			break;
		case DR_ACTION_TYP_TNL_L2_TO_L2:
			break;
		case DR_ACTION_TYP_TNL_L3_TO_L2:
			attr.decap_index = action->rewrite->index;
			attr.decap_actions = action->rewrite->num_of_actions;
			attr.decap_with_vlan =
				attr.decap_actions == WITH_VLAN_NUM_HW_ACTIONS;
			break;
		case DR_ACTION_TYP_MODIFY_HDR:
			attr.modify_index = action->rewrite->index;
			attr.modify_actions = action->rewrite->num_of_actions;
			recalc_cs_required = action->rewrite->modify_ttl &&
					     !mlx5dr_ste_supp_ttl_cs_recalc(&dmn->info.caps);
			break;
		case DR_ACTION_TYP_L2_TO_TNL_L2:
		case DR_ACTION_TYP_L2_TO_TNL_L3:
			attr.reformat_size = action->reformat->reformat_size;
			attr.reformat_id = action->reformat->reformat_id;
			break;
		case DR_ACTION_TYP_VPORT:
			attr.hit_gvmi = action->vport->caps->vhca_gvmi;
			dest_action = action;
			if (rx_rule) {
				/* Loopback on WIRE vport is not supported */
				if (action->vport->caps->num == WIRE_PORT)
					goto out_invalid_arg;

				attr.final_icm_addr = action->vport->caps->icm_address_rx;
			} else {
				attr.final_icm_addr = action->vport->caps->icm_address_tx;
			}
			break;
		case DR_ACTION_TYP_POP_VLAN:
			max_actions_type = MLX5DR_MAX_VLANS;
			attr.vlans.count++;
			break;
		case DR_ACTION_TYP_PUSH_VLAN:
			max_actions_type = MLX5DR_MAX_VLANS;
			if (attr.vlans.count == MLX5DR_MAX_VLANS)
				return -EINVAL;

			attr.vlans.headers[attr.vlans.count++] = action->push_vlan->vlan_hdr;
			break;
		default:
			goto out_invalid_arg;
		}

		/* Check action duplication */
		if (++action_type_set[action_type] > max_actions_type) {
			mlx5dr_err(dmn, "Action type %d supports only max %d time(s)\n",
				   action_type, max_actions_type);
			goto out_invalid_arg;
		}

		/* Check action state machine is valid */
		if (dr_action_validate_and_get_next_state(action_domain,
							  action_type,
							  &state)) {
			mlx5dr_err(dmn, "Invalid action sequence provided\n");
			return -EOPNOTSUPP;
		}
	}

	*new_hw_ste_arr_sz = nic_matcher->num_of_builders;
	last_ste = ste_arr + DR_STE_SIZE * (nic_matcher->num_of_builders - 1);

	/* Due to a HW bug in some devices, modifying TTL on RX flows will
	 * cause an incorrect checksum calculation. In this case we will
	 * use a FW table to recalculate.
	 */
	if (dmn->type == MLX5DR_DOMAIN_TYPE_FDB &&
	    rx_rule && recalc_cs_required && dest_action) {
		ret = dr_action_handle_cs_recalc(dmn, dest_action, &attr.final_icm_addr);
		if (ret) {
			mlx5dr_err(dmn,
				   "Failed to handle checksum recalculation err %d\n",
				   ret);
			return ret;
		}
	}

	dr_actions_apply(dmn,
			 nic_dmn->ste_type,
			 action_type_set,
			 last_ste,
			 &attr,
			 new_hw_ste_arr_sz);

	return 0;

out_invalid_arg:
	return -EINVAL;
}

static unsigned int action_size[DR_ACTION_TYP_MAX] = {
	[DR_ACTION_TYP_TNL_L2_TO_L2] = sizeof(struct mlx5dr_action_reformat),
	[DR_ACTION_TYP_L2_TO_TNL_L2] = sizeof(struct mlx5dr_action_reformat),
	[DR_ACTION_TYP_TNL_L3_TO_L2] = sizeof(struct mlx5dr_action_rewrite),
	[DR_ACTION_TYP_L2_TO_TNL_L3] = sizeof(struct mlx5dr_action_reformat),
	[DR_ACTION_TYP_FT]           = sizeof(struct mlx5dr_action_dest_tbl),
	[DR_ACTION_TYP_CTR]          = sizeof(struct mlx5dr_action_ctr),
	[DR_ACTION_TYP_TAG]          = sizeof(struct mlx5dr_action_flow_tag),
	[DR_ACTION_TYP_MODIFY_HDR]   = sizeof(struct mlx5dr_action_rewrite),
	[DR_ACTION_TYP_VPORT]        = sizeof(struct mlx5dr_action_vport),
	[DR_ACTION_TYP_PUSH_VLAN]    = sizeof(struct mlx5dr_action_push_vlan),
};

static struct mlx5dr_action *
dr_action_create_generic(enum mlx5dr_action_type action_type)
{
	struct mlx5dr_action *action;
	int extra_size;

	if (action_type < DR_ACTION_TYP_MAX)
		extra_size = action_size[action_type];
	else
		return NULL;

	action = kzalloc(sizeof(*action) + extra_size, GFP_KERNEL);
	if (!action)
		return NULL;

	action->action_type = action_type;
	refcount_set(&action->refcount, 1);
	action->data = action + 1;

	return action;
}

struct mlx5dr_action *mlx5dr_action_create_drop(void)
{
	return dr_action_create_generic(DR_ACTION_TYP_DROP);
}

struct mlx5dr_action *
mlx5dr_action_create_dest_table_num(struct mlx5dr_domain *dmn, u32 table_num)
{
	struct mlx5dr_action *action;

	action = dr_action_create_generic(DR_ACTION_TYP_FT);
	if (!action)
		return NULL;

	action->dest_tbl->is_fw_tbl = true;
	action->dest_tbl->fw_tbl.dmn = dmn;
	action->dest_tbl->fw_tbl.id = table_num;
	action->dest_tbl->fw_tbl.type = FS_FT_FDB;
	refcount_inc(&dmn->refcount);

	return action;
}

struct mlx5dr_action *
mlx5dr_action_create_dest_table(struct mlx5dr_table *tbl)
{
	struct mlx5dr_action *action;

	refcount_inc(&tbl->refcount);

	action = dr_action_create_generic(DR_ACTION_TYP_FT);
	if (!action)
		goto dec_ref;

	action->dest_tbl->tbl = tbl;

	return action;

dec_ref:
	refcount_dec(&tbl->refcount);
	return NULL;
}

struct mlx5dr_action *
mlx5dr_action_create_mult_dest_tbl(struct mlx5dr_domain *dmn,
				   struct mlx5dr_action_dest *dests,
				   u32 num_of_dests)
{
	struct mlx5dr_cmd_flow_destination_hw_info *hw_dests;
	struct mlx5dr_action **ref_actions;
	struct mlx5dr_action *action;
	bool reformat_req = false;
	u32 num_of_ref = 0;
	int ret;
	int i;

	if (dmn->type != MLX5DR_DOMAIN_TYPE_FDB) {
		mlx5dr_err(dmn, "Multiple destination support is for FDB only\n");
		return NULL;
	}

	hw_dests = kzalloc(sizeof(*hw_dests) * num_of_dests, GFP_KERNEL);
	if (!hw_dests)
		return NULL;

	ref_actions = kzalloc(sizeof(*ref_actions) * num_of_dests * 2, GFP_KERNEL);
	if (!ref_actions)
		goto free_hw_dests;

	for (i = 0; i < num_of_dests; i++) {
		struct mlx5dr_action *reformat_action = dests[i].reformat;
		struct mlx5dr_action *dest_action = dests[i].dest;

		ref_actions[num_of_ref++] = dest_action;

		switch (dest_action->action_type) {
		case DR_ACTION_TYP_VPORT:
			hw_dests[i].vport.flags = MLX5_FLOW_DEST_VPORT_VHCA_ID;
			hw_dests[i].type = MLX5_FLOW_DESTINATION_TYPE_VPORT;
			hw_dests[i].vport.num = dest_action->vport->caps->num;
			hw_dests[i].vport.vhca_id = dest_action->vport->caps->vhca_gvmi;
			if (reformat_action) {
				reformat_req = true;
				hw_dests[i].vport.reformat_id =
					reformat_action->reformat->reformat_id;
				ref_actions[num_of_ref++] = reformat_action;
				hw_dests[i].vport.flags |= MLX5_FLOW_DEST_VPORT_REFORMAT_ID;
			}
			break;

		case DR_ACTION_TYP_FT:
			hw_dests[i].type = MLX5_FLOW_DESTINATION_TYPE_FLOW_TABLE;
			if (dest_action->dest_tbl->is_fw_tbl)
				hw_dests[i].ft_id = dest_action->dest_tbl->fw_tbl.id;
			else
				hw_dests[i].ft_id = dest_action->dest_tbl->tbl->table_id;
			break;

		default:
			mlx5dr_dbg(dmn, "Invalid multiple destinations action\n");
			goto free_ref_actions;
		}
	}

	action = dr_action_create_generic(DR_ACTION_TYP_FT);
	if (!action)
		goto free_ref_actions;

	ret = mlx5dr_fw_create_md_tbl(dmn,
				      hw_dests,
				      num_of_dests,
				      reformat_req,
				      &action->dest_tbl->fw_tbl.id,
				      &action->dest_tbl->fw_tbl.group_id);
	if (ret)
		goto free_action;

	refcount_inc(&dmn->refcount);

	for (i = 0; i < num_of_ref; i++)
		refcount_inc(&ref_actions[i]->refcount);

	action->dest_tbl->is_fw_tbl = true;
	action->dest_tbl->fw_tbl.dmn = dmn;
	action->dest_tbl->fw_tbl.type = FS_FT_FDB;
	action->dest_tbl->fw_tbl.ref_actions = ref_actions;
	action->dest_tbl->fw_tbl.num_of_ref_actions = num_of_ref;

	kfree(hw_dests);

	return action;

free_action:
	kfree(action);
free_ref_actions:
	kfree(ref_actions);
free_hw_dests:
	kfree(hw_dests);
	return NULL;
}

struct mlx5dr_action *
mlx5dr_action_create_dest_flow_fw_table(struct mlx5dr_domain *dmn,
					struct mlx5_flow_table *ft)
{
	struct mlx5dr_action *action;

	action = dr_action_create_generic(DR_ACTION_TYP_FT);
	if (!action)
		return NULL;

	action->dest_tbl->is_fw_tbl = 1;
	action->dest_tbl->fw_tbl.type = ft->type;
	action->dest_tbl->fw_tbl.id = ft->id;
	action->dest_tbl->fw_tbl.dmn = dmn;

	refcount_inc(&dmn->refcount);

	return action;
}

struct mlx5dr_action *
mlx5dr_action_create_flow_counter(u32 counter_id)
{
	struct mlx5dr_action *action;

	action = dr_action_create_generic(DR_ACTION_TYP_CTR);
	if (!action)
		return NULL;

	action->ctr->ctr_id = counter_id;

	return action;
}

struct mlx5dr_action *mlx5dr_action_create_tag(u32 tag_value)
{
	struct mlx5dr_action *action;

	action = dr_action_create_generic(DR_ACTION_TYP_TAG);
	if (!action)
		return NULL;

	action->flow_tag->flow_tag = tag_value & 0xffffff;

	return action;
}

static int
dr_action_verify_reformat_params(enum mlx5dr_action_type reformat_type,
				 struct mlx5dr_domain *dmn,
				 size_t data_sz,
				 void *data)
{
	if ((!data && data_sz) || (data && !data_sz) || reformat_type >
		DR_ACTION_TYP_L2_TO_TNL_L3) {
		mlx5dr_dbg(dmn, "Invalid reformat parameter!\n");
		goto out_err;
	}

	if (dmn->type == MLX5DR_DOMAIN_TYPE_FDB)
		return 0;

	if (dmn->type == MLX5DR_DOMAIN_TYPE_NIC_RX) {
		if (reformat_type != DR_ACTION_TYP_TNL_L2_TO_L2 &&
		    reformat_type != DR_ACTION_TYP_TNL_L3_TO_L2) {
			mlx5dr_dbg(dmn, "Action reformat type not support on RX domain\n");
			goto out_err;
		}
	} else if (dmn->type == MLX5DR_DOMAIN_TYPE_NIC_TX) {
		if (reformat_type != DR_ACTION_TYP_L2_TO_TNL_L2 &&
		    reformat_type != DR_ACTION_TYP_L2_TO_TNL_L3) {
			mlx5dr_dbg(dmn, "Action reformat type not support on TX domain\n");
			goto out_err;
		}
	}

	return 0;

out_err:
	return -EINVAL;
}

#define ACTION_CACHE_LINE_SIZE 64

static int
dr_action_create_reformat_action(struct mlx5dr_domain *dmn,
				 size_t data_sz, void *data,
				 struct mlx5dr_action *action)
{
	u32 reformat_id;
	int ret;

	switch (action->action_type) {
	case DR_ACTION_TYP_L2_TO_TNL_L2:
	case DR_ACTION_TYP_L2_TO_TNL_L3:
	{
		enum mlx5_reformat_ctx_type rt;

		if (action->action_type == DR_ACTION_TYP_L2_TO_TNL_L2)
			rt = MLX5_REFORMAT_TYPE_L2_TO_L2_TUNNEL;
		else
			rt = MLX5_REFORMAT_TYPE_L2_TO_L3_TUNNEL;

		ret = mlx5dr_cmd_create_reformat_ctx(dmn->mdev, rt, data_sz, data,
						     &reformat_id);
		if (ret)
			return ret;

		action->reformat->reformat_id = reformat_id;
		action->reformat->reformat_size = data_sz;
		return 0;
	}
	case DR_ACTION_TYP_TNL_L2_TO_L2:
	{
		return 0;
	}
	case DR_ACTION_TYP_TNL_L3_TO_L2:
	{
		u8 hw_actions[ACTION_CACHE_LINE_SIZE] = {};
		int ret;

		ret = mlx5dr_ste_set_action_decap_l3_list(dmn->ste_ctx,
							  data, data_sz,
							  hw_actions,
							  ACTION_CACHE_LINE_SIZE,
							  &action->rewrite->num_of_actions);
		if (ret) {
			mlx5dr_dbg(dmn, "Failed creating decap l3 action list\n");
			return ret;
		}

		action->rewrite->chunk = mlx5dr_icm_alloc_chunk(dmn->action_icm_pool,
								DR_CHUNK_SIZE_8);
		if (!action->rewrite->chunk) {
			mlx5dr_dbg(dmn, "Failed allocating modify header chunk\n");
			return -ENOMEM;
		}

		action->rewrite->data = (void *)hw_actions;
		action->rewrite->index = (action->rewrite->chunk->icm_addr -
					 dmn->info.caps.hdr_modify_icm_addr) /
					 ACTION_CACHE_LINE_SIZE;

		ret = mlx5dr_send_postsend_action(dmn, action);
		if (ret) {
			mlx5dr_dbg(dmn, "Writing decap l3 actions to ICM failed\n");
			mlx5dr_icm_free_chunk(action->rewrite->chunk);
			return ret;
		}
		return 0;
	}
	default:
		mlx5dr_info(dmn, "Reformat type is not supported %d\n", action->action_type);
		return -EINVAL;
	}
}

#define CVLAN_ETHERTYPE 0x8100
#define SVLAN_ETHERTYPE 0x88a8

struct mlx5dr_action *mlx5dr_action_create_pop_vlan(void)
{
	return dr_action_create_generic(DR_ACTION_TYP_POP_VLAN);
}

struct mlx5dr_action *mlx5dr_action_create_push_vlan(struct mlx5dr_domain *dmn,
						     __be32 vlan_hdr)
{
	u32 vlan_hdr_h = ntohl(vlan_hdr);
	u16 ethertype = vlan_hdr_h >> 16;
	struct mlx5dr_action *action;

	if (ethertype != SVLAN_ETHERTYPE && ethertype != CVLAN_ETHERTYPE) {
		mlx5dr_dbg(dmn, "Invalid vlan ethertype\n");
		return NULL;
	}

	action = dr_action_create_generic(DR_ACTION_TYP_PUSH_VLAN);
	if (!action)
		return NULL;

	action->push_vlan->vlan_hdr = vlan_hdr_h;
	return action;
}

struct mlx5dr_action *
mlx5dr_action_create_packet_reformat(struct mlx5dr_domain *dmn,
				     enum mlx5dr_action_reformat_type reformat_type,
				     size_t data_sz,
				     void *data)
{
	enum mlx5dr_action_type action_type;
	struct mlx5dr_action *action;
	int ret;

	refcount_inc(&dmn->refcount);

	/* General checks */
	ret = dr_action_reformat_to_action_type(reformat_type, &action_type);
	if (ret) {
		mlx5dr_dbg(dmn, "Invalid reformat_type provided\n");
		goto dec_ref;
	}

	ret = dr_action_verify_reformat_params(action_type, dmn, data_sz, data);
	if (ret)
		goto dec_ref;

	action = dr_action_create_generic(action_type);
	if (!action)
		goto dec_ref;

	action->reformat->dmn = dmn;

	ret = dr_action_create_reformat_action(dmn,
					       data_sz,
					       data,
					       action);
	if (ret) {
		mlx5dr_dbg(dmn, "Failed creating reformat action %d\n", ret);
		goto free_action;
	}

	return action;

free_action:
	kfree(action);
dec_ref:
	refcount_dec(&dmn->refcount);
	return NULL;
}

static int
dr_action_modify_sw_to_hw_add(struct mlx5dr_domain *dmn,
			      __be64 *sw_action,
			      __be64 *hw_action,
			      const struct mlx5dr_ste_action_modify_field **ret_hw_info)
{
	const struct mlx5dr_ste_action_modify_field *hw_action_info;
	u8 max_length;
	u16 sw_field;
	u32 data;

	/* Get SW modify action data */
	sw_field = MLX5_GET(set_action_in, sw_action, field);
	data = MLX5_GET(set_action_in, sw_action, data);

	/* Convert SW data to HW modify action format */
	hw_action_info = mlx5dr_ste_conv_modify_hdr_sw_field(dmn->ste_ctx, sw_field);
	if (!hw_action_info) {
		mlx5dr_dbg(dmn, "Modify add action invalid field given\n");
		return -EINVAL;
	}

	max_length = hw_action_info->end - hw_action_info->start + 1;

	mlx5dr_ste_set_action_add(dmn->ste_ctx,
				  hw_action,
				  hw_action_info->hw_field,
				  hw_action_info->start,
				  max_length,
				  data);

	*ret_hw_info = hw_action_info;

	return 0;
}

static int
dr_action_modify_sw_to_hw_set(struct mlx5dr_domain *dmn,
			      __be64 *sw_action,
			      __be64 *hw_action,
			      const struct mlx5dr_ste_action_modify_field **ret_hw_info)
{
	const struct mlx5dr_ste_action_modify_field *hw_action_info;
	u8 offset, length, max_length;
	u16 sw_field;
	u32 data;

	/* Get SW modify action data */
	length = MLX5_GET(set_action_in, sw_action, length);
	offset = MLX5_GET(set_action_in, sw_action, offset);
	sw_field = MLX5_GET(set_action_in, sw_action, field);
	data = MLX5_GET(set_action_in, sw_action, data);

	/* Convert SW data to HW modify action format */
	hw_action_info = mlx5dr_ste_conv_modify_hdr_sw_field(dmn->ste_ctx, sw_field);
	if (!hw_action_info) {
		mlx5dr_dbg(dmn, "Modify set action invalid field given\n");
		return -EINVAL;
	}

	/* PRM defines that length zero specific length of 32bits */
	length = length ? length : 32;

	max_length = hw_action_info->end - hw_action_info->start + 1;

	if (length + offset > max_length) {
		mlx5dr_dbg(dmn, "Modify action length + offset exceeds limit\n");
		return -EINVAL;
	}

	mlx5dr_ste_set_action_set(dmn->ste_ctx,
				  hw_action,
				  hw_action_info->hw_field,
				  hw_action_info->start + offset,
				  length,
				  data);

	*ret_hw_info = hw_action_info;

	return 0;
}

static int
dr_action_modify_sw_to_hw_copy(struct mlx5dr_domain *dmn,
			       __be64 *sw_action,
			       __be64 *hw_action,
			       const struct mlx5dr_ste_action_modify_field **ret_dst_hw_info,
			       const struct mlx5dr_ste_action_modify_field **ret_src_hw_info)
{
	u8 src_offset, dst_offset, src_max_length, dst_max_length, length;
	const struct mlx5dr_ste_action_modify_field *hw_dst_action_info;
	const struct mlx5dr_ste_action_modify_field *hw_src_action_info;
	u16 src_field, dst_field;

	/* Get SW modify action data */
	src_field = MLX5_GET(copy_action_in, sw_action, src_field);
	dst_field = MLX5_GET(copy_action_in, sw_action, dst_field);
	src_offset = MLX5_GET(copy_action_in, sw_action, src_offset);
	dst_offset = MLX5_GET(copy_action_in, sw_action, dst_offset);
	length = MLX5_GET(copy_action_in, sw_action, length);

	/* Convert SW data to HW modify action format */
	hw_src_action_info = mlx5dr_ste_conv_modify_hdr_sw_field(dmn->ste_ctx, src_field);
	hw_dst_action_info = mlx5dr_ste_conv_modify_hdr_sw_field(dmn->ste_ctx, dst_field);
	if (!hw_src_action_info || !hw_dst_action_info) {
		mlx5dr_dbg(dmn, "Modify copy action invalid field given\n");
		return -EINVAL;
	}

	/* PRM defines that length zero specific length of 32bits */
	length = length ? length : 32;

	src_max_length = hw_src_action_info->end -
			 hw_src_action_info->start + 1;
	dst_max_length = hw_dst_action_info->end -
			 hw_dst_action_info->start + 1;

	if (length + src_offset > src_max_length ||
	    length + dst_offset > dst_max_length) {
		mlx5dr_dbg(dmn, "Modify action length + offset exceeds limit\n");
		return -EINVAL;
	}

	mlx5dr_ste_set_action_copy(dmn->ste_ctx,
				   hw_action,
				   hw_dst_action_info->hw_field,
				   hw_dst_action_info->start + dst_offset,
				   length,
				   hw_src_action_info->hw_field,
				   hw_src_action_info->start + src_offset);

	*ret_dst_hw_info = hw_dst_action_info;
	*ret_src_hw_info = hw_src_action_info;

	return 0;
}

static int
dr_action_modify_sw_to_hw(struct mlx5dr_domain *dmn,
			  __be64 *sw_action,
			  __be64 *hw_action,
			  const struct mlx5dr_ste_action_modify_field **ret_dst_hw_info,
			  const struct mlx5dr_ste_action_modify_field **ret_src_hw_info)
{
	u8 action;
	int ret;

	*hw_action = 0;
	*ret_src_hw_info = NULL;

	/* Get SW modify action type */
	action = MLX5_GET(set_action_in, sw_action, action_type);

	switch (action) {
	case MLX5_ACTION_TYPE_SET:
		ret = dr_action_modify_sw_to_hw_set(dmn, sw_action,
						    hw_action,
						    ret_dst_hw_info);
		break;

	case MLX5_ACTION_TYPE_ADD:
		ret = dr_action_modify_sw_to_hw_add(dmn, sw_action,
						    hw_action,
						    ret_dst_hw_info);
		break;

	case MLX5_ACTION_TYPE_COPY:
		ret = dr_action_modify_sw_to_hw_copy(dmn, sw_action,
						     hw_action,
						     ret_dst_hw_info,
						     ret_src_hw_info);
		break;

	default:
		mlx5dr_info(dmn, "Unsupported action_type for modify action\n");
		ret = -EOPNOTSUPP;
	}

	return ret;
}

static int
dr_action_modify_check_set_field_limitation(struct mlx5dr_action *action,
					    const __be64 *sw_action)
{
	u16 sw_field = MLX5_GET(set_action_in, sw_action, field);
	struct mlx5dr_domain *dmn = action->rewrite->dmn;

	if (sw_field == MLX5_ACTION_IN_FIELD_METADATA_REG_A) {
		action->rewrite->allow_rx = 0;
		if (dmn->type != MLX5DR_DOMAIN_TYPE_NIC_TX) {
			mlx5dr_dbg(dmn, "Unsupported field %d for RX/FDB set action\n",
				   sw_field);
			return -EINVAL;
		}
	} else if (sw_field == MLX5_ACTION_IN_FIELD_METADATA_REG_B) {
		action->rewrite->allow_tx = 0;
		if (dmn->type != MLX5DR_DOMAIN_TYPE_NIC_RX) {
			mlx5dr_dbg(dmn, "Unsupported field %d for TX/FDB set action\n",
				   sw_field);
			return -EINVAL;
		}
	}

	if (!action->rewrite->allow_rx && !action->rewrite->allow_tx) {
		mlx5dr_dbg(dmn, "Modify SET actions not supported on both RX and TX\n");
		return -EINVAL;
	}

	return 0;
}

static int
dr_action_modify_check_add_field_limitation(struct mlx5dr_action *action,
					    const __be64 *sw_action)
{
	u16 sw_field = MLX5_GET(set_action_in, sw_action, field);
	struct mlx5dr_domain *dmn = action->rewrite->dmn;

	if (sw_field != MLX5_ACTION_IN_FIELD_OUT_IP_TTL &&
	    sw_field != MLX5_ACTION_IN_FIELD_OUT_IPV6_HOPLIMIT &&
	    sw_field != MLX5_ACTION_IN_FIELD_OUT_TCP_SEQ_NUM &&
	    sw_field != MLX5_ACTION_IN_FIELD_OUT_TCP_ACK_NUM) {
		mlx5dr_dbg(dmn, "Unsupported field %d for add action\n",
			   sw_field);
		return -EINVAL;
	}

	return 0;
}

static int
dr_action_modify_check_copy_field_limitation(struct mlx5dr_action *action,
					     const __be64 *sw_action)
{
	struct mlx5dr_domain *dmn = action->rewrite->dmn;
	u16 sw_fields[2];
	int i;

	sw_fields[0] = MLX5_GET(copy_action_in, sw_action, src_field);
	sw_fields[1] = MLX5_GET(copy_action_in, sw_action, dst_field);

	for (i = 0; i < 2; i++) {
		if (sw_fields[i] == MLX5_ACTION_IN_FIELD_METADATA_REG_A) {
			action->rewrite->allow_rx = 0;
			if (dmn->type != MLX5DR_DOMAIN_TYPE_NIC_TX) {
				mlx5dr_dbg(dmn, "Unsupported field %d for RX/FDB set action\n",
					   sw_fields[i]);
				return -EINVAL;
			}
		} else if (sw_fields[i] == MLX5_ACTION_IN_FIELD_METADATA_REG_B) {
			action->rewrite->allow_tx = 0;
			if (dmn->type != MLX5DR_DOMAIN_TYPE_NIC_RX) {
				mlx5dr_dbg(dmn, "Unsupported field %d for TX/FDB set action\n",
					   sw_fields[i]);
				return -EINVAL;
			}
		}
	}

	if (!action->rewrite->allow_rx && !action->rewrite->allow_tx) {
		mlx5dr_dbg(dmn, "Modify copy actions not supported on both RX and TX\n");
		return -EINVAL;
	}

	return 0;
}

static int
dr_action_modify_check_field_limitation(struct mlx5dr_action *action,
					const __be64 *sw_action)
{
	struct mlx5dr_domain *dmn = action->rewrite->dmn;
	u8 action_type;
	int ret;

	action_type = MLX5_GET(set_action_in, sw_action, action_type);

	switch (action_type) {
	case MLX5_ACTION_TYPE_SET:
		ret = dr_action_modify_check_set_field_limitation(action,
								  sw_action);
		break;

	case MLX5_ACTION_TYPE_ADD:
		ret = dr_action_modify_check_add_field_limitation(action,
								  sw_action);
		break;

	case MLX5_ACTION_TYPE_COPY:
		ret = dr_action_modify_check_copy_field_limitation(action,
								   sw_action);
		break;

	default:
		mlx5dr_info(dmn, "Unsupported action %d modify action\n",
			    action_type);
		ret = -EOPNOTSUPP;
	}

	return ret;
}

static bool
dr_action_modify_check_is_ttl_modify(const void *sw_action)
{
	u16 sw_field = MLX5_GET(set_action_in, sw_action, field);

	return sw_field == MLX5_ACTION_IN_FIELD_OUT_IP_TTL;
}

static int dr_actions_convert_modify_header(struct mlx5dr_action *action,
					    u32 max_hw_actions,
					    u32 num_sw_actions,
					    __be64 sw_actions[],
					    __be64 hw_actions[],
					    u32 *num_hw_actions,
					    bool *modify_ttl)
{
	const struct mlx5dr_ste_action_modify_field *hw_dst_action_info;
	const struct mlx5dr_ste_action_modify_field *hw_src_action_info;
	struct mlx5dr_domain *dmn = action->rewrite->dmn;
	int ret, i, hw_idx = 0;
	__be64 *sw_action;
	__be64 hw_action;
	u16 hw_field = 0;
	u32 l3_type = 0;
	u32 l4_type = 0;

	*modify_ttl = false;

	action->rewrite->allow_rx = 1;
	action->rewrite->allow_tx = 1;

	for (i = 0; i < num_sw_actions; i++) {
		sw_action = &sw_actions[i];

		ret = dr_action_modify_check_field_limitation(action,
							      sw_action);
		if (ret)
			return ret;

		if (!(*modify_ttl))
			*modify_ttl = dr_action_modify_check_is_ttl_modify(sw_action);

		/* Convert SW action to HW action */
		ret = dr_action_modify_sw_to_hw(dmn,
						sw_action,
						&hw_action,
						&hw_dst_action_info,
						&hw_src_action_info);
		if (ret)
			return ret;

		/* Due to a HW limitation we cannot modify 2 different L3 types */
		if (l3_type && hw_dst_action_info->l3_type &&
		    hw_dst_action_info->l3_type != l3_type) {
			mlx5dr_dbg(dmn, "Action list can't support two different L3 types\n");
			return -EINVAL;
		}
		if (hw_dst_action_info->l3_type)
			l3_type = hw_dst_action_info->l3_type;

		/* Due to a HW limitation we cannot modify two different L4 types */
		if (l4_type && hw_dst_action_info->l4_type &&
		    hw_dst_action_info->l4_type != l4_type) {
			mlx5dr_dbg(dmn, "Action list can't support two different L4 types\n");
			return -EINVAL;
		}
		if (hw_dst_action_info->l4_type)
			l4_type = hw_dst_action_info->l4_type;

		/* HW reads and executes two actions at once this means we
		 * need to create a gap if two actions access the same field
		 */
		if ((hw_idx % 2) && (hw_field == hw_dst_action_info->hw_field ||
				     (hw_src_action_info &&
				      hw_field == hw_src_action_info->hw_field))) {
			/* Check if after gap insertion the total number of HW
			 * modify actions doesn't exceeds the limit
			 */
			hw_idx++;
			if ((num_sw_actions + hw_idx - i) >= max_hw_actions) {
				mlx5dr_dbg(dmn, "Modify header action number exceeds HW limit\n");
				return -EINVAL;
			}
		}
		hw_field = hw_dst_action_info->hw_field;

		hw_actions[hw_idx] = hw_action;
		hw_idx++;
	}

	*num_hw_actions = hw_idx;

	return 0;
}

static int dr_action_create_modify_action(struct mlx5dr_domain *dmn,
					  size_t actions_sz,
					  __be64 actions[],
					  struct mlx5dr_action *action)
{
	struct mlx5dr_icm_chunk *chunk;
	u32 max_hw_actions;
	u32 num_hw_actions;
	u32 num_sw_actions;
	__be64 *hw_actions;
	bool modify_ttl;
	int ret;

	num_sw_actions = actions_sz / DR_MODIFY_ACTION_SIZE;
	max_hw_actions = mlx5dr_icm_pool_chunk_size_to_entries(DR_CHUNK_SIZE_16);

	if (num_sw_actions > max_hw_actions) {
		mlx5dr_dbg(dmn, "Max number of actions %d exceeds limit %d\n",
			   num_sw_actions, max_hw_actions);
		return -EINVAL;
	}

	chunk = mlx5dr_icm_alloc_chunk(dmn->action_icm_pool, DR_CHUNK_SIZE_16);
	if (!chunk)
		return -ENOMEM;

	hw_actions = kcalloc(1, max_hw_actions * DR_MODIFY_ACTION_SIZE, GFP_KERNEL);
	if (!hw_actions) {
		ret = -ENOMEM;
		goto free_chunk;
	}

	ret = dr_actions_convert_modify_header(action,
					       max_hw_actions,
					       num_sw_actions,
					       actions,
					       hw_actions,
					       &num_hw_actions,
					       &modify_ttl);
	if (ret)
		goto free_hw_actions;

	action->rewrite->chunk = chunk;
	action->rewrite->modify_ttl = modify_ttl;
	action->rewrite->data = (u8 *)hw_actions;
	action->rewrite->num_of_actions = num_hw_actions;
	action->rewrite->index = (chunk->icm_addr -
				  dmn->info.caps.hdr_modify_icm_addr) /
				  ACTION_CACHE_LINE_SIZE;

	ret = mlx5dr_send_postsend_action(dmn, action);
	if (ret)
		goto free_hw_actions;

	return 0;

free_hw_actions:
	kfree(hw_actions);
free_chunk:
	mlx5dr_icm_free_chunk(chunk);
	return ret;
}

struct mlx5dr_action *
mlx5dr_action_create_modify_header(struct mlx5dr_domain *dmn,
				   u32 flags,
				   size_t actions_sz,
				   __be64 actions[])
{
	struct mlx5dr_action *action;
	int ret = 0;

	refcount_inc(&dmn->refcount);

	if (actions_sz % DR_MODIFY_ACTION_SIZE) {
		mlx5dr_dbg(dmn, "Invalid modify actions size provided\n");
		goto dec_ref;
	}

	action = dr_action_create_generic(DR_ACTION_TYP_MODIFY_HDR);
	if (!action)
		goto dec_ref;

	action->rewrite->dmn = dmn;

	ret = dr_action_create_modify_action(dmn,
					     actions_sz,
					     actions,
					     action);
	if (ret) {
		mlx5dr_dbg(dmn, "Failed creating modify header action %d\n", ret);
		goto free_action;
	}

	return action;

free_action:
	kfree(action);
dec_ref:
	refcount_dec(&dmn->refcount);
	return NULL;
}

struct mlx5dr_action *
mlx5dr_action_create_dest_vport(struct mlx5dr_domain *dmn,
				u32 vport, u8 vhca_id_valid,
				u16 vhca_id)
{
	struct mlx5dr_cmd_vport_cap *vport_cap;
	struct mlx5dr_domain *vport_dmn;
	struct mlx5dr_action *action;
	u8 peer_vport;

	peer_vport = vhca_id_valid && (vhca_id != dmn->info.caps.gvmi);
	vport_dmn = peer_vport ? dmn->peer_dmn : dmn;
	if (!vport_dmn) {
		mlx5dr_dbg(dmn, "No peer vport domain for given vhca_id\n");
		return NULL;
	}

	if (vport_dmn->type != MLX5DR_DOMAIN_TYPE_FDB) {
		mlx5dr_dbg(dmn, "Domain doesn't support vport actions\n");
		return NULL;
	}

	vport_cap = mlx5dr_get_vport_cap(&vport_dmn->info.caps, vport);
	if (!vport_cap) {
		mlx5dr_dbg(dmn, "Failed to get vport %d caps\n", vport);
		return NULL;
	}

	action = dr_action_create_generic(DR_ACTION_TYP_VPORT);
	if (!action)
		return NULL;

	action->vport->dmn = vport_dmn;
	action->vport->caps = vport_cap;

	return action;
}

int mlx5dr_action_destroy(struct mlx5dr_action *action)
{
	if (refcount_read(&action->refcount) > 1)
		return -EBUSY;

	switch (action->action_type) {
	case DR_ACTION_TYP_FT:
		if (action->dest_tbl->is_fw_tbl)
			refcount_dec(&action->dest_tbl->fw_tbl.dmn->refcount);
		else
			refcount_dec(&action->dest_tbl->tbl->refcount);

		if (action->dest_tbl->is_fw_tbl &&
		    action->dest_tbl->fw_tbl.num_of_ref_actions) {
			struct mlx5dr_action **ref_actions;
			int i;

			ref_actions = action->dest_tbl->fw_tbl.ref_actions;
			for (i = 0; i < action->dest_tbl->fw_tbl.num_of_ref_actions; i++)
				refcount_dec(&ref_actions[i]->refcount);

			kfree(ref_actions);

			mlx5dr_fw_destroy_md_tbl(action->dest_tbl->fw_tbl.dmn,
						 action->dest_tbl->fw_tbl.id,
						 action->dest_tbl->fw_tbl.group_id);
		}
		break;
	case DR_ACTION_TYP_TNL_L2_TO_L2:
		refcount_dec(&action->reformat->dmn->refcount);
		break;
	case DR_ACTION_TYP_TNL_L3_TO_L2:
		mlx5dr_icm_free_chunk(action->rewrite->chunk);
		refcount_dec(&action->rewrite->dmn->refcount);
		break;
	case DR_ACTION_TYP_L2_TO_TNL_L2:
	case DR_ACTION_TYP_L2_TO_TNL_L3:
		mlx5dr_cmd_destroy_reformat_ctx((action->reformat->dmn)->mdev,
						action->reformat->reformat_id);
		refcount_dec(&action->reformat->dmn->refcount);
		break;
	case DR_ACTION_TYP_MODIFY_HDR:
		mlx5dr_icm_free_chunk(action->rewrite->chunk);
		kfree(action->rewrite->data);
		refcount_dec(&action->rewrite->dmn->refcount);
		break;
	default:
		break;
	}

	kfree(action);
	return 0;
}
