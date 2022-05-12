/*
 * Copyright (c) 2004, 2005 Topspin Communications.  All rights reserved.
 * Copyright (c) 2005, 2006, 2007, 2008 Mellanox Technologies. All rights reserved.
 * Copyright (c) 2006, 2007 Cisco Systems.  All rights reserved.
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

#ifndef MLX4_FW_H
#define MLX4_FW_H

#include "mlx4.h"
#include "icm.h"

struct mlx4_mod_stat_cfg {
	u8 log_pg_sz;
	u8 log_pg_sz_m;
};

struct mlx4_port_cap {
	u8  link_state;
	u8  supported_port_types;
	u8  suggested_type;
	u8  default_sense;
	u8  log_max_macs;
	u8  log_max_vlans;
	int ib_mtu;
	int max_port_width;
	int max_vl;
	int max_tc_eth;
	int max_gids;
	int max_pkeys;
	u64 def_mac;
	u16 eth_mtu;
	int trans_type;
	int vendor_oui;
	u16 wavelength;
	u64 trans_code;
	u8 dmfs_optimized_state;
};

struct mlx4_dev_cap {
	int max_srq_sz;
	int max_qp_sz;
	int reserved_qps;
	int max_qps;
	int reserved_srqs;
	int max_srqs;
	int max_cq_sz;
	int reserved_cqs;
	int max_cqs;
	int max_mpts;
	int reserved_eqs;
	int max_eqs;
	int num_sys_eqs;
	int reserved_mtts;
	int reserved_mrws;
	int max_requester_per_qp;
	int max_responder_per_qp;
	int max_rdma_global;
	int local_ca_ack_delay;
	int num_ports;
	u32 max_msg_sz;
	u16 stat_rate_support;
	int fs_log_max_ucast_qp_range_size;
	int fs_max_num_qp_per_entry;
	u64 flags;
	u64 flags2;
	int reserved_uars;
	int uar_size;
	int min_page_sz;
	int bf_reg_size;
	int bf_regs_per_page;
	int max_sq_sg;
	int max_sq_desc_sz;
	int max_rq_sg;
	int max_rq_desc_sz;
	int max_qp_per_mcg;
	int reserved_mgms;
	int max_mcgs;
	int reserved_pds;
	int max_pds;
	int reserved_xrcds;
	int max_xrcds;
	int qpc_entry_sz;
	int rdmarc_entry_sz;
	int altc_entry_sz;
	int aux_entry_sz;
	int srq_entry_sz;
	int cqc_entry_sz;
	int eqc_entry_sz;
	int dmpt_entry_sz;
	int cmpt_entry_sz;
	int mtt_entry_sz;
	int resize_srq;
	u32 bmme_flags;
	u32 reserved_lkey;
	u64 max_icm_sz;
	int max_gso_sz;
	int max_rss_tbl_sz;
	u32 max_counters;
	u32 dmfs_high_rate_qpn_base;
	u32 dmfs_high_rate_qpn_range;
	struct mlx4_rate_limit_caps rl_caps;
	u32 health_buffer_addrs;
	struct mlx4_port_cap port_cap[MLX4_MAX_PORTS + 1];
	bool wol_port[MLX4_MAX_PORTS + 1];
	bool map_clock_to_user;
};

struct mlx4_func_cap {
	u8	num_ports;
	u8	flags;
	u32	pf_context_behaviour;
	int	qp_quota;
	int	cq_quota;
	int	srq_quota;
	int	mpt_quota;
	int	mtt_quota;
	int	max_eq;
	int	reserved_eq;
	int	mcg_quota;
	struct mlx4_spec_qps spec_qps;
	u32	reserved_lkey;
	u8	physical_port;
	u8	flags0;
	u8	flags1;
	u64	phys_port_id;
	u32	extra_flags;
};

struct mlx4_func {
	int	bus;
	int	device;
	int	function;
	int	physical_function;
	int	rsvd_eqs;
	int	max_eq;
	int	rsvd_uars;
};

struct mlx4_adapter {
	char board_id[MLX4_BOARD_ID_LEN];
	u8   inta_pin;
};

struct mlx4_init_hca_param {
	u64 qpc_base;
	u64 rdmarc_base;
	u64 auxc_base;
	u64 altc_base;
	u64 srqc_base;
	u64 cqc_base;
	u64 eqc_base;
	u64 mc_base;
	u64 dmpt_base;
	u64 cmpt_base;
	u64 mtt_base;
	u64 global_caps;
	u8 log_mc_entry_sz;
	u8 log_mc_hash_sz;
	u16 hca_core_clock; /* Internal Clock Frequency (in MHz) */
	u8  log_num_qps;
	u8  log_num_srqs;
	u8  log_num_cqs;
	u8  log_num_eqs;
	u16 num_sys_eqs;
	u8  log_rd_per_qp;
	u8  log_mc_table_sz;
	u8  log_mpt_sz;
	u8  log_uar_sz;
	u8  mw_enabled;  /* Enable memory windows */
	u8  uar_page_sz; /* log pg sz in 4k chunks */
	u8  steering_mode; /* for QUERY_HCA */
	u8  dmfs_high_steer_mode; /* for QUERY_HCA */
	u64 dev_cap_enabled;
	u16 cqe_size; /* For use only when CQE stride feature enabled */
	u16 eqe_size; /* For use only when EQE stride feature enabled */
	u8 rss_ip_frags;
	u8 phv_check_en; /* for QUERY_HCA */
};

struct mlx4_init_ib_param {
	int port_width;
	int vl_cap;
	int mtu_cap;
	u16 gid_cap;
	u16 pkey_cap;
	int set_guid0;
	u64 guid0;
	int set_node_guid;
	u64 node_guid;
	int set_si_guid;
	u64 si_guid;
};

struct mlx4_set_ib_param {
	int set_si_guid;
	int reset_qkey_viol;
	u64 si_guid;
	u32 cap_mask;
};

void mlx4_dev_cap_dump(struct mlx4_dev *dev, struct mlx4_dev_cap *dev_cap);
int mlx4_QUERY_DEV_CAP(struct mlx4_dev *dev, struct mlx4_dev_cap *dev_cap);
int mlx4_QUERY_PORT(struct mlx4_dev *dev, int port, struct mlx4_port_cap *port_cap);
int mlx4_QUERY_FUNC_CAP(struct mlx4_dev *dev, u8 gen_or_port,
			struct mlx4_func_cap *func_cap);
int mlx4_QUERY_FUNC_CAP_wrapper(struct mlx4_dev *dev, int slave,
				struct mlx4_vhcr *vhcr,
				struct mlx4_cmd_mailbox *inbox,
				struct mlx4_cmd_mailbox *outbox,
				struct mlx4_cmd_info *cmd);
int mlx4_QUERY_FUNC(struct mlx4_dev *dev, struct mlx4_func *func, int slave);
int mlx4_MAP_FA(struct mlx4_dev *dev, struct mlx4_icm *icm);
int mlx4_UNMAP_FA(struct mlx4_dev *dev);
int mlx4_RUN_FW(struct mlx4_dev *dev);
int mlx4_QUERY_FW(struct mlx4_dev *dev);
int mlx4_QUERY_ADAPTER(struct mlx4_dev *dev, struct mlx4_adapter *adapter);
int mlx4_INIT_HCA(struct mlx4_dev *dev, struct mlx4_init_hca_param *param);
int mlx4_QUERY_HCA(struct mlx4_dev *dev, struct mlx4_init_hca_param *param);
int mlx4_CLOSE_HCA(struct mlx4_dev *dev, int panic);
int mlx4_map_cmd(struct mlx4_dev *dev, u16 op, struct mlx4_icm *icm, u64 virt);
int mlx4_SET_ICM_SIZE(struct mlx4_dev *dev, u64 icm_size, u64 *aux_pages);
int mlx4_MAP_ICM_AUX(struct mlx4_dev *dev, struct mlx4_icm *icm);
int mlx4_UNMAP_ICM_AUX(struct mlx4_dev *dev);
int mlx4_NOP(struct mlx4_dev *dev);
int mlx4_MOD_STAT_CFG(struct mlx4_dev *dev, struct mlx4_mod_stat_cfg *cfg);
void mlx4_opreq_action(struct work_struct *work);

#endif /* MLX4_FW_H */
