/*
 * Copyright (c) 2016, Mellanox Technologies. All rights reserved.
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
 */

#ifndef __MLX5_EN_TC_H__
#define __MLX5_EN_TC_H__

#include <net/pkt_cls.h>
#include "en.h"
#include "eswitch.h"
#include "en/tc_ct.h"
#include "en/tc_tun.h"
#include "en_rep.h"

#define MLX5E_TC_FLOW_ID_MASK 0x0000ffff

#ifdef CONFIG_MLX5_ESWITCH

#define NIC_FLOW_ATTR_SZ (sizeof(struct mlx5_flow_attr) +\
			  sizeof(struct mlx5_nic_flow_attr))
#define ESW_FLOW_ATTR_SZ (sizeof(struct mlx5_flow_attr) +\
			  sizeof(struct mlx5_esw_flow_attr))
#define ns_to_attr_sz(ns) (((ns) == MLX5_FLOW_NAMESPACE_FDB) ?\
			    ESW_FLOW_ATTR_SZ :\
			    NIC_FLOW_ATTR_SZ)


int mlx5e_tc_num_filters(struct mlx5e_priv *priv, unsigned long flags);

struct mlx5e_tc_update_priv {
	struct net_device *tun_dev;
};

struct mlx5_nic_flow_attr {
	u32 flow_tag;
	u32 hairpin_tirn;
	struct mlx5_flow_table *hairpin_ft;
};

struct mlx5_flow_attr {
	u32 action;
	struct mlx5_fc *counter;
	struct mlx5_modify_hdr *modify_hdr;
	struct mlx5_ct_attr ct_attr;
	struct mlx5e_tc_flow_parse_attr *parse_attr;
	u32 chain;
	u16 prio;
	u32 dest_chain;
	struct mlx5_flow_table *ft;
	struct mlx5_flow_table *dest_ft;
	u8 inner_match_level;
	u8 outer_match_level;
	u8 ip_version;
	u8 tun_ip_version;
	u32 flags;
	union {
		struct mlx5_esw_flow_attr esw_attr[0];
		struct mlx5_nic_flow_attr nic_attr[0];
	};
};

struct mlx5_rx_tun_attr {
	u16 decap_vport;
	union {
		__be32 v4;
		struct in6_addr v6;
	} src_ip; /* Valid if decap_vport is not zero */
	union {
		__be32 v4;
		struct in6_addr v6;
	} dst_ip; /* Valid if decap_vport is not zero */
	u32 vni;
};

#define MLX5E_TC_TABLE_CHAIN_TAG_BITS 16
#define MLX5E_TC_TABLE_CHAIN_TAG_MASK GENMASK(MLX5E_TC_TABLE_CHAIN_TAG_BITS - 1, 0)

#if IS_ENABLED(CONFIG_MLX5_CLS_ACT)

struct tunnel_match_key {
	struct flow_dissector_key_control enc_control;
	struct flow_dissector_key_keyid enc_key_id;
	struct flow_dissector_key_ports enc_tp;
	struct flow_dissector_key_ip enc_ip;
	union {
		struct flow_dissector_key_ipv4_addrs enc_ipv4;
		struct flow_dissector_key_ipv6_addrs enc_ipv6;
	};

	int filter_ifindex;
};

struct tunnel_match_enc_opts {
	struct flow_dissector_key_enc_opts key;
	struct flow_dissector_key_enc_opts mask;
};

/* Tunnel_id mapping is TUNNEL_INFO_BITS + ENC_OPTS_BITS.
 * Upper TUNNEL_INFO_BITS for general tunnel info.
 * Lower ENC_OPTS_BITS bits for enc_opts.
 */
#define TUNNEL_INFO_BITS 12
#define TUNNEL_INFO_BITS_MASK GENMASK(TUNNEL_INFO_BITS - 1, 0)
#define ENC_OPTS_BITS 12
#define ENC_OPTS_BITS_MASK GENMASK(ENC_OPTS_BITS - 1, 0)
#define TUNNEL_ID_BITS (TUNNEL_INFO_BITS + ENC_OPTS_BITS)
#define TUNNEL_ID_MASK GENMASK(TUNNEL_ID_BITS - 1, 0)

enum {
	MLX5E_TC_FLAG_INGRESS_BIT,
	MLX5E_TC_FLAG_EGRESS_BIT,
	MLX5E_TC_FLAG_NIC_OFFLOAD_BIT,
	MLX5E_TC_FLAG_ESW_OFFLOAD_BIT,
	MLX5E_TC_FLAG_FT_OFFLOAD_BIT,
	MLX5E_TC_FLAG_LAST_EXPORTED_BIT = MLX5E_TC_FLAG_FT_OFFLOAD_BIT,
};

#define MLX5_TC_FLAG(flag) BIT(MLX5E_TC_FLAG_##flag##_BIT)

int mlx5e_tc_esw_init(struct rhashtable *tc_ht);
void mlx5e_tc_esw_cleanup(struct rhashtable *tc_ht);
bool mlx5e_is_eswitch_flow(struct mlx5e_tc_flow *flow);

int mlx5e_configure_flower(struct net_device *dev, struct mlx5e_priv *priv,
			   struct flow_cls_offload *f, unsigned long flags);
int mlx5e_delete_flower(struct net_device *dev, struct mlx5e_priv *priv,
			struct flow_cls_offload *f, unsigned long flags);

int mlx5e_stats_flower(struct net_device *dev, struct mlx5e_priv *priv,
		       struct flow_cls_offload *f, unsigned long flags);

int mlx5e_tc_configure_matchall(struct mlx5e_priv *priv,
				struct tc_cls_matchall_offload *f);
int mlx5e_tc_delete_matchall(struct mlx5e_priv *priv,
			     struct tc_cls_matchall_offload *f);
void mlx5e_tc_stats_matchall(struct mlx5e_priv *priv,
			     struct tc_cls_matchall_offload *ma);

struct mlx5e_encap_entry;
void mlx5e_tc_encap_flows_add(struct mlx5e_priv *priv,
			      struct mlx5e_encap_entry *e,
			      struct list_head *flow_list);
void mlx5e_tc_encap_flows_del(struct mlx5e_priv *priv,
			      struct mlx5e_encap_entry *e,
			      struct list_head *flow_list);
bool mlx5e_encap_take(struct mlx5e_encap_entry *e);
void mlx5e_encap_put(struct mlx5e_priv *priv, struct mlx5e_encap_entry *e);

void mlx5e_take_all_encap_flows(struct mlx5e_encap_entry *e, struct list_head *flow_list);
void mlx5e_put_flow_list(struct mlx5e_priv *priv, struct list_head *flow_list);

struct mlx5e_neigh_hash_entry;
struct mlx5e_encap_entry *
mlx5e_get_next_init_encap(struct mlx5e_neigh_hash_entry *nhe,
			  struct mlx5e_encap_entry *e);
void mlx5e_tc_update_neigh_used_value(struct mlx5e_neigh_hash_entry *nhe);

void mlx5e_tc_reoffload_flows_work(struct work_struct *work);

enum mlx5e_tc_attr_to_reg {
	CHAIN_TO_REG,
	VPORT_TO_REG,
	TUNNEL_TO_REG,
	CTSTATE_TO_REG,
	ZONE_TO_REG,
	ZONE_RESTORE_TO_REG,
	MARK_TO_REG,
	LABELS_TO_REG,
	FTEID_TO_REG,
	NIC_CHAIN_TO_REG,
	NIC_ZONE_RESTORE_TO_REG,
};

struct mlx5e_tc_attr_to_reg_mapping {
	int mfield; /* rewrite field */
	int moffset; /* offset of mfield */
	int mlen; /* bytes to rewrite/match */

	int soffset; /* offset of spec for match */
};

extern struct mlx5e_tc_attr_to_reg_mapping mlx5e_tc_attr_to_reg_mappings[];

bool mlx5e_is_valid_eswitch_fwd_dev(struct mlx5e_priv *priv,
				    struct net_device *out_dev);

int mlx5e_tc_match_to_reg_set(struct mlx5_core_dev *mdev,
			      struct mlx5e_tc_mod_hdr_acts *mod_hdr_acts,
			      enum mlx5_flow_namespace_type ns,
			      enum mlx5e_tc_attr_to_reg type,
			      u32 data);

void mlx5e_tc_match_to_reg_mod_hdr_change(struct mlx5_core_dev *mdev,
					  struct mlx5e_tc_mod_hdr_acts *mod_hdr_acts,
					  enum mlx5e_tc_attr_to_reg type,
					  int act_id, u32 data);

void mlx5e_tc_match_to_reg_match(struct mlx5_flow_spec *spec,
				 enum mlx5e_tc_attr_to_reg type,
				 u32 data,
				 u32 mask);

void mlx5e_tc_match_to_reg_get_match(struct mlx5_flow_spec *spec,
				     enum mlx5e_tc_attr_to_reg type,
				     u32 *data,
				     u32 *mask);

int mlx5e_tc_match_to_reg_set_and_get_id(struct mlx5_core_dev *mdev,
					 struct mlx5e_tc_mod_hdr_acts *mod_hdr_acts,
					 enum mlx5_flow_namespace_type ns,
					 enum mlx5e_tc_attr_to_reg type,
					 u32 data);

int mlx5e_tc_add_flow_mod_hdr(struct mlx5e_priv *priv,
			      struct mlx5e_tc_flow_parse_attr *parse_attr,
			      struct mlx5e_tc_flow *flow);

int alloc_mod_hdr_actions(struct mlx5_core_dev *mdev,
			  int namespace,
			  struct mlx5e_tc_mod_hdr_acts *mod_hdr_acts);
void dealloc_mod_hdr_actions(struct mlx5e_tc_mod_hdr_acts *mod_hdr_acts);

struct mlx5e_tc_flow;
u32 mlx5e_tc_get_flow_tun_id(struct mlx5e_tc_flow *flow);

void mlx5e_tc_set_ethertype(struct mlx5_core_dev *mdev,
			    struct flow_match_basic *match, bool outer,
			    void *headers_c, void *headers_v);

int mlx5e_tc_nic_init(struct mlx5e_priv *priv);
void mlx5e_tc_nic_cleanup(struct mlx5e_priv *priv);

int mlx5e_setup_tc_block_cb(enum tc_setup_type type, void *type_data,
			    void *cb_priv);

struct mlx5_flow_handle *
mlx5e_add_offloaded_nic_rule(struct mlx5e_priv *priv,
			     struct mlx5_flow_spec *spec,
			     struct mlx5_flow_attr *attr);
void mlx5e_del_offloaded_nic_rule(struct mlx5e_priv *priv,
				  struct mlx5_flow_handle *rule,
				  struct mlx5_flow_attr *attr);

struct mlx5_flow_handle *
mlx5_tc_rule_insert(struct mlx5e_priv *priv,
		    struct mlx5_flow_spec *spec,
		    struct mlx5_flow_attr *attr);
void
mlx5_tc_rule_delete(struct mlx5e_priv *priv,
		    struct mlx5_flow_handle *rule,
		    struct mlx5_flow_attr *attr);

bool mlx5e_tc_is_vf_tunnel(struct net_device *out_dev, struct net_device *route_dev);
int mlx5e_tc_query_route_vport(struct net_device *out_dev, struct net_device *route_dev,
			       u16 *vport);

#else /* CONFIG_MLX5_CLS_ACT */
static inline int  mlx5e_tc_nic_init(struct mlx5e_priv *priv) { return 0; }
static inline void mlx5e_tc_nic_cleanup(struct mlx5e_priv *priv) {}
static inline int
mlx5e_setup_tc_block_cb(enum tc_setup_type type, void *type_data, void *cb_priv)
{ return -EOPNOTSUPP; }

#endif /* CONFIG_MLX5_CLS_ACT */

struct mlx5_flow_attr *mlx5_alloc_flow_attr(enum mlx5_flow_namespace_type type);

struct mlx5_flow_handle *
mlx5e_add_offloaded_nic_rule(struct mlx5e_priv *priv,
			     struct mlx5_flow_spec *spec,
			     struct mlx5_flow_attr *attr);
void mlx5e_del_offloaded_nic_rule(struct mlx5e_priv *priv,
				  struct mlx5_flow_handle *rule,
				  struct mlx5_flow_attr *attr);

#else /* CONFIG_MLX5_ESWITCH */
static inline int  mlx5e_tc_nic_init(struct mlx5e_priv *priv) { return 0; }
static inline void mlx5e_tc_nic_cleanup(struct mlx5e_priv *priv) {}
static inline int  mlx5e_tc_num_filters(struct mlx5e_priv *priv,
					unsigned long flags)
{
	return 0;
}

static inline int
mlx5e_setup_tc_block_cb(enum tc_setup_type type, void *type_data, void *cb_priv)
{ return -EOPNOTSUPP; }
#endif

#if IS_ENABLED(CONFIG_MLX5_CLS_ACT)
static inline bool mlx5e_cqe_regb_chain(struct mlx5_cqe64 *cqe)
{
#if IS_ENABLED(CONFIG_NET_TC_SKB_EXT)
	u32 chain, reg_b;

	reg_b = be32_to_cpu(cqe->ft_metadata);

	if (reg_b >> (MLX5E_TC_TABLE_CHAIN_TAG_BITS + ESW_ZONE_ID_BITS))
		return false;

	chain = reg_b & MLX5E_TC_TABLE_CHAIN_TAG_MASK;
	if (chain)
		return true;
#endif

	return false;
}

bool mlx5e_tc_update_skb(struct mlx5_cqe64 *cqe, struct sk_buff *skb);
#else /* CONFIG_MLX5_CLS_ACT */
static inline bool mlx5e_cqe_regb_chain(struct mlx5_cqe64 *cqe)
{ return false; }
static inline bool
mlx5e_tc_update_skb(struct mlx5_cqe64 *cqe, struct sk_buff *skb)
{ return true; }
#endif

#endif /* __MLX5_EN_TC_H__ */
