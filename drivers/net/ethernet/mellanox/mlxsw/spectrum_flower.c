// SPDX-License-Identifier: BSD-3-Clause OR GPL-2.0
/* Copyright (c) 2017-2018 Mellanox Technologies. All rights reserved */

#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/netdevice.h>
#include <linux/log2.h>
#include <net/net_namespace.h>
#include <net/flow_dissector.h>
#include <net/pkt_cls.h>
#include <net/tc_act/tc_gact.h>
#include <net/tc_act/tc_mirred.h>
#include <net/tc_act/tc_vlan.h>

#include "spectrum.h"
#include "core_acl_flex_keys.h"

static int mlxsw_sp_flower_parse_actions(struct mlxsw_sp *mlxsw_sp,
					 struct mlxsw_sp_flow_block *block,
					 struct mlxsw_sp_acl_rule_info *rulei,
					 struct flow_action *flow_action,
					 struct netlink_ext_ack *extack)
{
	const struct flow_action_entry *act;
	int mirror_act_count = 0;
	int police_act_count = 0;
	int sample_act_count = 0;
	int err, i;

	if (!flow_action_has_entries(flow_action))
		return 0;
	if (!flow_action_mixed_hw_stats_check(flow_action, extack))
		return -EOPNOTSUPP;

	act = flow_action_first_entry_get(flow_action);
	if (act->hw_stats & FLOW_ACTION_HW_STATS_DISABLED) {
		/* Nothing to do */
	} else if (act->hw_stats & FLOW_ACTION_HW_STATS_IMMEDIATE) {
		/* Count action is inserted first */
		err = mlxsw_sp_acl_rulei_act_count(mlxsw_sp, rulei, extack);
		if (err)
			return err;
	} else {
		NL_SET_ERR_MSG_MOD(extack, "Unsupported action HW stats type");
		return -EOPNOTSUPP;
	}

	flow_action_for_each(i, act, flow_action) {
		switch (act->id) {
		case FLOW_ACTION_ACCEPT:
			err = mlxsw_sp_acl_rulei_act_terminate(rulei);
			if (err) {
				NL_SET_ERR_MSG_MOD(extack, "Cannot append terminate action");
				return err;
			}
			break;
		case FLOW_ACTION_DROP: {
			bool ingress;

			if (mlxsw_sp_flow_block_is_mixed_bound(block)) {
				NL_SET_ERR_MSG_MOD(extack, "Drop action is not supported when block is bound to ingress and egress");
				return -EOPNOTSUPP;
			}
			ingress = mlxsw_sp_flow_block_is_ingress_bound(block);
			err = mlxsw_sp_acl_rulei_act_drop(rulei, ingress,
							  act->cookie, extack);
			if (err) {
				NL_SET_ERR_MSG_MOD(extack, "Cannot append drop action");
				return err;
			}

			/* Forbid block with this rulei to be bound
			 * to ingress/egress in future. Ingress rule is
			 * a blocker for egress and vice versa.
			 */
			if (ingress)
				rulei->egress_bind_blocker = 1;
			else
				rulei->ingress_bind_blocker = 1;
			}
			break;
		case FLOW_ACTION_TRAP:
			err = mlxsw_sp_acl_rulei_act_trap(rulei);
			if (err) {
				NL_SET_ERR_MSG_MOD(extack, "Cannot append trap action");
				return err;
			}
			break;
		case FLOW_ACTION_GOTO: {
			u32 chain_index = act->chain_index;
			struct mlxsw_sp_acl_ruleset *ruleset;
			u16 group_id;

			ruleset = mlxsw_sp_acl_ruleset_lookup(mlxsw_sp, block,
							      chain_index,
							      MLXSW_SP_ACL_PROFILE_FLOWER);
			if (IS_ERR(ruleset))
				return PTR_ERR(ruleset);

			group_id = mlxsw_sp_acl_ruleset_group_id(ruleset);
			err = mlxsw_sp_acl_rulei_act_jump(rulei, group_id);
			if (err) {
				NL_SET_ERR_MSG_MOD(extack, "Cannot append jump action");
				return err;
			}
			}
			break;
		case FLOW_ACTION_REDIRECT: {
			struct net_device *out_dev;
			struct mlxsw_sp_fid *fid;
			u16 fid_index;

			if (mlxsw_sp_flow_block_is_egress_bound(block)) {
				NL_SET_ERR_MSG_MOD(extack, "Redirect action is not supported on egress");
				return -EOPNOTSUPP;
			}

			/* Forbid block with this rulei to be bound
			 * to egress in future.
			 */
			rulei->egress_bind_blocker = 1;

			fid = mlxsw_sp_acl_dummy_fid(mlxsw_sp);
			fid_index = mlxsw_sp_fid_index(fid);
			err = mlxsw_sp_acl_rulei_act_fid_set(mlxsw_sp, rulei,
							     fid_index, extack);
			if (err)
				return err;

			out_dev = act->dev;
			err = mlxsw_sp_acl_rulei_act_fwd(mlxsw_sp, rulei,
							 out_dev, extack);
			if (err)
				return err;
			}
			break;
		case FLOW_ACTION_MIRRED: {
			struct net_device *out_dev = act->dev;

			if (mirror_act_count++) {
				NL_SET_ERR_MSG_MOD(extack, "Multiple mirror actions per rule are not supported");
				return -EOPNOTSUPP;
			}

			err = mlxsw_sp_acl_rulei_act_mirror(mlxsw_sp, rulei,
							    block, out_dev,
							    extack);
			if (err)
				return err;
			}
			break;
		case FLOW_ACTION_VLAN_MANGLE: {
			u16 proto = be16_to_cpu(act->vlan.proto);
			u8 prio = act->vlan.prio;
			u16 vid = act->vlan.vid;

			err = mlxsw_sp_acl_rulei_act_vlan(mlxsw_sp, rulei,
							  act->id, vid,
							  proto, prio, extack);
			if (err)
				return err;
			break;
			}
		case FLOW_ACTION_PRIORITY:
			err = mlxsw_sp_acl_rulei_act_priority(mlxsw_sp, rulei,
							      act->priority,
							      extack);
			if (err)
				return err;
			break;
		case FLOW_ACTION_MANGLE: {
			enum flow_action_mangle_base htype = act->mangle.htype;
			__be32 be_mask = (__force __be32) act->mangle.mask;
			__be32 be_val = (__force __be32) act->mangle.val;
			u32 offset = act->mangle.offset;
			u32 mask = be32_to_cpu(be_mask);
			u32 val = be32_to_cpu(be_val);

			err = mlxsw_sp_acl_rulei_act_mangle(mlxsw_sp, rulei,
							    htype, offset,
							    mask, val, extack);
			if (err)
				return err;
			break;
			}
		case FLOW_ACTION_POLICE: {
			u32 burst;

			if (police_act_count++) {
				NL_SET_ERR_MSG_MOD(extack, "Multiple police actions per rule are not supported");
				return -EOPNOTSUPP;
			}

			if (act->police.rate_pkt_ps) {
				NL_SET_ERR_MSG_MOD(extack, "QoS offload not support packets per second");
				return -EOPNOTSUPP;
			}

			/* The kernel might adjust the requested burst size so
			 * that it is not exactly a power of two. Re-adjust it
			 * here since the hardware only supports burst sizes
			 * that are a power of two.
			 */
			burst = roundup_pow_of_two(act->police.burst);
			err = mlxsw_sp_acl_rulei_act_police(mlxsw_sp, rulei,
							    act->police.index,
							    act->police.rate_bytes_ps,
							    burst, extack);
			if (err)
				return err;
			break;
			}
		case FLOW_ACTION_SAMPLE: {
			if (sample_act_count++) {
				NL_SET_ERR_MSG_MOD(extack, "Multiple sample actions per rule are not supported");
				return -EOPNOTSUPP;
			}

			err = mlxsw_sp_acl_rulei_act_sample(mlxsw_sp, rulei,
							    block,
							    act->sample.psample_group,
							    act->sample.rate,
							    act->sample.trunc_size,
							    act->sample.truncate,
							    extack);
			if (err)
				return err;
			break;
			}
		default:
			NL_SET_ERR_MSG_MOD(extack, "Unsupported action");
			dev_err(mlxsw_sp->bus_info->dev, "Unsupported action\n");
			return -EOPNOTSUPP;
		}
	}
	return 0;
}

static int mlxsw_sp_flower_parse_meta(struct mlxsw_sp_acl_rule_info *rulei,
				      struct flow_cls_offload *f,
				      struct mlxsw_sp_flow_block *block)
{
	struct flow_rule *rule = flow_cls_offload_flow_rule(f);
	struct mlxsw_sp_port *mlxsw_sp_port;
	struct net_device *ingress_dev;
	struct flow_match_meta match;

	if (!flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_META))
		return 0;

	flow_rule_match_meta(rule, &match);
	if (match.mask->ingress_ifindex != 0xFFFFFFFF) {
		NL_SET_ERR_MSG_MOD(f->common.extack, "Unsupported ingress ifindex mask");
		return -EINVAL;
	}

	ingress_dev = __dev_get_by_index(block->net,
					 match.key->ingress_ifindex);
	if (!ingress_dev) {
		NL_SET_ERR_MSG_MOD(f->common.extack, "Can't find specified ingress port to match on");
		return -EINVAL;
	}

	if (!mlxsw_sp_port_dev_check(ingress_dev)) {
		NL_SET_ERR_MSG_MOD(f->common.extack, "Can't match on non-mlxsw ingress port");
		return -EINVAL;
	}

	mlxsw_sp_port = netdev_priv(ingress_dev);
	if (mlxsw_sp_port->mlxsw_sp != block->mlxsw_sp) {
		NL_SET_ERR_MSG_MOD(f->common.extack, "Can't match on a port from different device");
		return -EINVAL;
	}

	mlxsw_sp_acl_rulei_keymask_u32(rulei,
				       MLXSW_AFK_ELEMENT_SRC_SYS_PORT,
				       mlxsw_sp_port->local_port,
				       0xFFFFFFFF);
	return 0;
}

static void mlxsw_sp_flower_parse_ipv4(struct mlxsw_sp_acl_rule_info *rulei,
				       struct flow_cls_offload *f)
{
	struct flow_match_ipv4_addrs match;

	flow_rule_match_ipv4_addrs(f->rule, &match);

	mlxsw_sp_acl_rulei_keymask_buf(rulei, MLXSW_AFK_ELEMENT_SRC_IP_0_31,
				       (char *) &match.key->src,
				       (char *) &match.mask->src, 4);
	mlxsw_sp_acl_rulei_keymask_buf(rulei, MLXSW_AFK_ELEMENT_DST_IP_0_31,
				       (char *) &match.key->dst,
				       (char *) &match.mask->dst, 4);
}

static void mlxsw_sp_flower_parse_ipv6(struct mlxsw_sp_acl_rule_info *rulei,
				       struct flow_cls_offload *f)
{
	struct flow_match_ipv6_addrs match;

	flow_rule_match_ipv6_addrs(f->rule, &match);

	mlxsw_sp_acl_rulei_keymask_buf(rulei, MLXSW_AFK_ELEMENT_SRC_IP_96_127,
				       &match.key->src.s6_addr[0x0],
				       &match.mask->src.s6_addr[0x0], 4);
	mlxsw_sp_acl_rulei_keymask_buf(rulei, MLXSW_AFK_ELEMENT_SRC_IP_64_95,
				       &match.key->src.s6_addr[0x4],
				       &match.mask->src.s6_addr[0x4], 4);
	mlxsw_sp_acl_rulei_keymask_buf(rulei, MLXSW_AFK_ELEMENT_SRC_IP_32_63,
				       &match.key->src.s6_addr[0x8],
				       &match.mask->src.s6_addr[0x8], 4);
	mlxsw_sp_acl_rulei_keymask_buf(rulei, MLXSW_AFK_ELEMENT_SRC_IP_0_31,
				       &match.key->src.s6_addr[0xC],
				       &match.mask->src.s6_addr[0xC], 4);
	mlxsw_sp_acl_rulei_keymask_buf(rulei, MLXSW_AFK_ELEMENT_DST_IP_96_127,
				       &match.key->dst.s6_addr[0x0],
				       &match.mask->dst.s6_addr[0x0], 4);
	mlxsw_sp_acl_rulei_keymask_buf(rulei, MLXSW_AFK_ELEMENT_DST_IP_64_95,
				       &match.key->dst.s6_addr[0x4],
				       &match.mask->dst.s6_addr[0x4], 4);
	mlxsw_sp_acl_rulei_keymask_buf(rulei, MLXSW_AFK_ELEMENT_DST_IP_32_63,
				       &match.key->dst.s6_addr[0x8],
				       &match.mask->dst.s6_addr[0x8], 4);
	mlxsw_sp_acl_rulei_keymask_buf(rulei, MLXSW_AFK_ELEMENT_DST_IP_0_31,
				       &match.key->dst.s6_addr[0xC],
				       &match.mask->dst.s6_addr[0xC], 4);
}

static int mlxsw_sp_flower_parse_ports(struct mlxsw_sp *mlxsw_sp,
				       struct mlxsw_sp_acl_rule_info *rulei,
				       struct flow_cls_offload *f,
				       u8 ip_proto)
{
	const struct flow_rule *rule = flow_cls_offload_flow_rule(f);
	struct flow_match_ports match;

	if (!flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_PORTS))
		return 0;

	if (ip_proto != IPPROTO_TCP && ip_proto != IPPROTO_UDP) {
		NL_SET_ERR_MSG_MOD(f->common.extack, "Only UDP and TCP keys are supported");
		dev_err(mlxsw_sp->bus_info->dev, "Only UDP and TCP keys are supported\n");
		return -EINVAL;
	}

	flow_rule_match_ports(rule, &match);
	mlxsw_sp_acl_rulei_keymask_u32(rulei, MLXSW_AFK_ELEMENT_DST_L4_PORT,
				       ntohs(match.key->dst),
				       ntohs(match.mask->dst));
	mlxsw_sp_acl_rulei_keymask_u32(rulei, MLXSW_AFK_ELEMENT_SRC_L4_PORT,
				       ntohs(match.key->src),
				       ntohs(match.mask->src));
	return 0;
}

static int mlxsw_sp_flower_parse_tcp(struct mlxsw_sp *mlxsw_sp,
				     struct mlxsw_sp_acl_rule_info *rulei,
				     struct flow_cls_offload *f,
				     u8 ip_proto)
{
	const struct flow_rule *rule = flow_cls_offload_flow_rule(f);
	struct flow_match_tcp match;

	if (!flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_TCP))
		return 0;

	if (ip_proto != IPPROTO_TCP) {
		NL_SET_ERR_MSG_MOD(f->common.extack, "TCP keys supported only for TCP");
		dev_err(mlxsw_sp->bus_info->dev, "TCP keys supported only for TCP\n");
		return -EINVAL;
	}

	flow_rule_match_tcp(rule, &match);

	if (match.mask->flags & htons(0x0E00)) {
		NL_SET_ERR_MSG_MOD(f->common.extack, "TCP flags match not supported on reserved bits");
		dev_err(mlxsw_sp->bus_info->dev, "TCP flags match not supported on reserved bits\n");
		return -EINVAL;
	}

	mlxsw_sp_acl_rulei_keymask_u32(rulei, MLXSW_AFK_ELEMENT_TCP_FLAGS,
				       ntohs(match.key->flags),
				       ntohs(match.mask->flags));
	return 0;
}

static int mlxsw_sp_flower_parse_ip(struct mlxsw_sp *mlxsw_sp,
				    struct mlxsw_sp_acl_rule_info *rulei,
				    struct flow_cls_offload *f,
				    u16 n_proto)
{
	const struct flow_rule *rule = flow_cls_offload_flow_rule(f);
	struct flow_match_ip match;

	if (!flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_IP))
		return 0;

	if (n_proto != ETH_P_IP && n_proto != ETH_P_IPV6) {
		NL_SET_ERR_MSG_MOD(f->common.extack, "IP keys supported only for IPv4/6");
		dev_err(mlxsw_sp->bus_info->dev, "IP keys supported only for IPv4/6\n");
		return -EINVAL;
	}

	flow_rule_match_ip(rule, &match);

	mlxsw_sp_acl_rulei_keymask_u32(rulei, MLXSW_AFK_ELEMENT_IP_TTL_,
				       match.key->ttl, match.mask->ttl);

	mlxsw_sp_acl_rulei_keymask_u32(rulei, MLXSW_AFK_ELEMENT_IP_ECN,
				       match.key->tos & 0x3,
				       match.mask->tos & 0x3);

	mlxsw_sp_acl_rulei_keymask_u32(rulei, MLXSW_AFK_ELEMENT_IP_DSCP,
				       match.key->tos >> 2,
				       match.mask->tos >> 2);

	return 0;
}

static int mlxsw_sp_flower_parse(struct mlxsw_sp *mlxsw_sp,
				 struct mlxsw_sp_flow_block *block,
				 struct mlxsw_sp_acl_rule_info *rulei,
				 struct flow_cls_offload *f)
{
	struct flow_rule *rule = flow_cls_offload_flow_rule(f);
	struct flow_dissector *dissector = rule->match.dissector;
	u16 n_proto_mask = 0;
	u16 n_proto_key = 0;
	u16 addr_type = 0;
	u8 ip_proto = 0;
	int err;

	if (dissector->used_keys &
	    ~(BIT(FLOW_DISSECTOR_KEY_META) |
	      BIT(FLOW_DISSECTOR_KEY_CONTROL) |
	      BIT(FLOW_DISSECTOR_KEY_BASIC) |
	      BIT(FLOW_DISSECTOR_KEY_ETH_ADDRS) |
	      BIT(FLOW_DISSECTOR_KEY_IPV4_ADDRS) |
	      BIT(FLOW_DISSECTOR_KEY_IPV6_ADDRS) |
	      BIT(FLOW_DISSECTOR_KEY_PORTS) |
	      BIT(FLOW_DISSECTOR_KEY_TCP) |
	      BIT(FLOW_DISSECTOR_KEY_IP) |
	      BIT(FLOW_DISSECTOR_KEY_VLAN))) {
		dev_err(mlxsw_sp->bus_info->dev, "Unsupported key\n");
		NL_SET_ERR_MSG_MOD(f->common.extack, "Unsupported key");
		return -EOPNOTSUPP;
	}

	mlxsw_sp_acl_rulei_priority(rulei, f->common.prio);

	err = mlxsw_sp_flower_parse_meta(rulei, f, block);
	if (err)
		return err;

	if (flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_CONTROL)) {
		struct flow_match_control match;

		flow_rule_match_control(rule, &match);
		addr_type = match.key->addr_type;
	}

	if (flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_BASIC)) {
		struct flow_match_basic match;

		flow_rule_match_basic(rule, &match);
		n_proto_key = ntohs(match.key->n_proto);
		n_proto_mask = ntohs(match.mask->n_proto);

		if (n_proto_key == ETH_P_ALL) {
			n_proto_key = 0;
			n_proto_mask = 0;
		}
		mlxsw_sp_acl_rulei_keymask_u32(rulei,
					       MLXSW_AFK_ELEMENT_ETHERTYPE,
					       n_proto_key, n_proto_mask);

		ip_proto = match.key->ip_proto;
		mlxsw_sp_acl_rulei_keymask_u32(rulei,
					       MLXSW_AFK_ELEMENT_IP_PROTO,
					       match.key->ip_proto,
					       match.mask->ip_proto);
	}

	if (flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_ETH_ADDRS)) {
		struct flow_match_eth_addrs match;

		flow_rule_match_eth_addrs(rule, &match);
		mlxsw_sp_acl_rulei_keymask_buf(rulei,
					       MLXSW_AFK_ELEMENT_DMAC_32_47,
					       match.key->dst,
					       match.mask->dst, 2);
		mlxsw_sp_acl_rulei_keymask_buf(rulei,
					       MLXSW_AFK_ELEMENT_DMAC_0_31,
					       match.key->dst + 2,
					       match.mask->dst + 2, 4);
		mlxsw_sp_acl_rulei_keymask_buf(rulei,
					       MLXSW_AFK_ELEMENT_SMAC_32_47,
					       match.key->src,
					       match.mask->src, 2);
		mlxsw_sp_acl_rulei_keymask_buf(rulei,
					       MLXSW_AFK_ELEMENT_SMAC_0_31,
					       match.key->src + 2,
					       match.mask->src + 2, 4);
	}

	if (flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_VLAN)) {
		struct flow_match_vlan match;

		flow_rule_match_vlan(rule, &match);
		if (mlxsw_sp_flow_block_is_egress_bound(block)) {
			NL_SET_ERR_MSG_MOD(f->common.extack, "vlan_id key is not supported on egress");
			return -EOPNOTSUPP;
		}

		/* Forbid block with this rulei to be bound
		 * to egress in future.
		 */
		rulei->egress_bind_blocker = 1;

		if (match.mask->vlan_id != 0)
			mlxsw_sp_acl_rulei_keymask_u32(rulei,
						       MLXSW_AFK_ELEMENT_VID,
						       match.key->vlan_id,
						       match.mask->vlan_id);
		if (match.mask->vlan_priority != 0)
			mlxsw_sp_acl_rulei_keymask_u32(rulei,
						       MLXSW_AFK_ELEMENT_PCP,
						       match.key->vlan_priority,
						       match.mask->vlan_priority);
	}

	if (addr_type == FLOW_DISSECTOR_KEY_IPV4_ADDRS)
		mlxsw_sp_flower_parse_ipv4(rulei, f);

	if (addr_type == FLOW_DISSECTOR_KEY_IPV6_ADDRS)
		mlxsw_sp_flower_parse_ipv6(rulei, f);

	err = mlxsw_sp_flower_parse_ports(mlxsw_sp, rulei, f, ip_proto);
	if (err)
		return err;
	err = mlxsw_sp_flower_parse_tcp(mlxsw_sp, rulei, f, ip_proto);
	if (err)
		return err;

	err = mlxsw_sp_flower_parse_ip(mlxsw_sp, rulei, f, n_proto_key & n_proto_mask);
	if (err)
		return err;

	return mlxsw_sp_flower_parse_actions(mlxsw_sp, block, rulei,
					     &f->rule->action,
					     f->common.extack);
}

static int mlxsw_sp_flower_mall_prio_check(struct mlxsw_sp_flow_block *block,
					   struct flow_cls_offload *f)
{
	bool ingress = mlxsw_sp_flow_block_is_ingress_bound(block);
	unsigned int mall_min_prio;
	unsigned int mall_max_prio;
	int err;

	err = mlxsw_sp_mall_prio_get(block, f->common.chain_index,
				     &mall_min_prio, &mall_max_prio);
	if (err) {
		if (err == -ENOENT)
			/* No matchall filters installed on this chain. */
			return 0;
		NL_SET_ERR_MSG(f->common.extack, "Failed to get matchall priorities");
		return err;
	}
	if (ingress && f->common.prio <= mall_min_prio) {
		NL_SET_ERR_MSG(f->common.extack, "Failed to add in front of existing matchall rules");
		return -EOPNOTSUPP;
	}
	if (!ingress && f->common.prio >= mall_max_prio) {
		NL_SET_ERR_MSG(f->common.extack, "Failed to add behind of existing matchall rules");
		return -EOPNOTSUPP;
	}
	return 0;
}

int mlxsw_sp_flower_replace(struct mlxsw_sp *mlxsw_sp,
			    struct mlxsw_sp_flow_block *block,
			    struct flow_cls_offload *f)
{
	struct mlxsw_sp_acl_rule_info *rulei;
	struct mlxsw_sp_acl_ruleset *ruleset;
	struct mlxsw_sp_acl_rule *rule;
	int err;

	err = mlxsw_sp_flower_mall_prio_check(block, f);
	if (err)
		return err;

	ruleset = mlxsw_sp_acl_ruleset_get(mlxsw_sp, block,
					   f->common.chain_index,
					   MLXSW_SP_ACL_PROFILE_FLOWER, NULL);
	if (IS_ERR(ruleset))
		return PTR_ERR(ruleset);

	rule = mlxsw_sp_acl_rule_create(mlxsw_sp, ruleset, f->cookie, NULL,
					f->common.extack);
	if (IS_ERR(rule)) {
		err = PTR_ERR(rule);
		goto err_rule_create;
	}

	rulei = mlxsw_sp_acl_rule_rulei(rule);
	err = mlxsw_sp_flower_parse(mlxsw_sp, block, rulei, f);
	if (err)
		goto err_flower_parse;

	err = mlxsw_sp_acl_rulei_commit(rulei);
	if (err)
		goto err_rulei_commit;

	err = mlxsw_sp_acl_rule_add(mlxsw_sp, rule);
	if (err)
		goto err_rule_add;

	mlxsw_sp_acl_ruleset_put(mlxsw_sp, ruleset);
	return 0;

err_rule_add:
err_rulei_commit:
err_flower_parse:
	mlxsw_sp_acl_rule_destroy(mlxsw_sp, rule);
err_rule_create:
	mlxsw_sp_acl_ruleset_put(mlxsw_sp, ruleset);
	return err;
}

void mlxsw_sp_flower_destroy(struct mlxsw_sp *mlxsw_sp,
			     struct mlxsw_sp_flow_block *block,
			     struct flow_cls_offload *f)
{
	struct mlxsw_sp_acl_ruleset *ruleset;
	struct mlxsw_sp_acl_rule *rule;

	ruleset = mlxsw_sp_acl_ruleset_get(mlxsw_sp, block,
					   f->common.chain_index,
					   MLXSW_SP_ACL_PROFILE_FLOWER, NULL);
	if (IS_ERR(ruleset))
		return;

	rule = mlxsw_sp_acl_rule_lookup(mlxsw_sp, ruleset, f->cookie);
	if (rule) {
		mlxsw_sp_acl_rule_del(mlxsw_sp, rule);
		mlxsw_sp_acl_rule_destroy(mlxsw_sp, rule);
	}

	mlxsw_sp_acl_ruleset_put(mlxsw_sp, ruleset);
}

int mlxsw_sp_flower_stats(struct mlxsw_sp *mlxsw_sp,
			  struct mlxsw_sp_flow_block *block,
			  struct flow_cls_offload *f)
{
	enum flow_action_hw_stats used_hw_stats = FLOW_ACTION_HW_STATS_DISABLED;
	struct mlxsw_sp_acl_ruleset *ruleset;
	struct mlxsw_sp_acl_rule *rule;
	u64 packets;
	u64 lastuse;
	u64 bytes;
	u64 drops;
	int err;

	ruleset = mlxsw_sp_acl_ruleset_get(mlxsw_sp, block,
					   f->common.chain_index,
					   MLXSW_SP_ACL_PROFILE_FLOWER, NULL);
	if (WARN_ON(IS_ERR(ruleset)))
		return -EINVAL;

	rule = mlxsw_sp_acl_rule_lookup(mlxsw_sp, ruleset, f->cookie);
	if (!rule)
		return -EINVAL;

	err = mlxsw_sp_acl_rule_get_stats(mlxsw_sp, rule, &packets, &bytes,
					  &drops, &lastuse, &used_hw_stats);
	if (err)
		goto err_rule_get_stats;

	flow_stats_update(&f->stats, bytes, packets, drops, lastuse,
			  used_hw_stats);

	mlxsw_sp_acl_ruleset_put(mlxsw_sp, ruleset);
	return 0;

err_rule_get_stats:
	mlxsw_sp_acl_ruleset_put(mlxsw_sp, ruleset);
	return err;
}

int mlxsw_sp_flower_tmplt_create(struct mlxsw_sp *mlxsw_sp,
				 struct mlxsw_sp_flow_block *block,
				 struct flow_cls_offload *f)
{
	struct mlxsw_sp_acl_ruleset *ruleset;
	struct mlxsw_sp_acl_rule_info rulei;
	int err;

	memset(&rulei, 0, sizeof(rulei));
	err = mlxsw_sp_flower_parse(mlxsw_sp, block, &rulei, f);
	if (err)
		return err;
	ruleset = mlxsw_sp_acl_ruleset_get(mlxsw_sp, block,
					   f->common.chain_index,
					   MLXSW_SP_ACL_PROFILE_FLOWER,
					   &rulei.values.elusage);

	/* keep the reference to the ruleset */
	return PTR_ERR_OR_ZERO(ruleset);
}

void mlxsw_sp_flower_tmplt_destroy(struct mlxsw_sp *mlxsw_sp,
				   struct mlxsw_sp_flow_block *block,
				   struct flow_cls_offload *f)
{
	struct mlxsw_sp_acl_ruleset *ruleset;

	ruleset = mlxsw_sp_acl_ruleset_get(mlxsw_sp, block,
					   f->common.chain_index,
					   MLXSW_SP_ACL_PROFILE_FLOWER, NULL);
	if (IS_ERR(ruleset))
		return;
	/* put the reference to the ruleset kept in create */
	mlxsw_sp_acl_ruleset_put(mlxsw_sp, ruleset);
	mlxsw_sp_acl_ruleset_put(mlxsw_sp, ruleset);
}

int mlxsw_sp_flower_prio_get(struct mlxsw_sp *mlxsw_sp,
			     struct mlxsw_sp_flow_block *block,
			     u32 chain_index, unsigned int *p_min_prio,
			     unsigned int *p_max_prio)
{
	struct mlxsw_sp_acl_ruleset *ruleset;

	ruleset = mlxsw_sp_acl_ruleset_lookup(mlxsw_sp, block,
					      chain_index,
					      MLXSW_SP_ACL_PROFILE_FLOWER);
	if (IS_ERR(ruleset))
		/* In case there are no flower rules, the caller
		 * receives -ENOENT to indicate there is no need
		 * to check the priorities.
		 */
		return PTR_ERR(ruleset);
	mlxsw_sp_acl_ruleset_prio_get(ruleset, p_min_prio, p_max_prio);
	return 0;
}
