/* SPDX-License-Identifier: GPL-2.0 */
/* Copyright (c) 2019, Intel Corporation. */

#ifndef _ICE_FLEX_PIPE_H_
#define _ICE_FLEX_PIPE_H_

#include "ice_type.h"

/* Package minimal version supported */
#define ICE_PKG_SUPP_VER_MAJ	1
#define ICE_PKG_SUPP_VER_MNR	3

/* Package format version */
#define ICE_PKG_FMT_VER_MAJ	1
#define ICE_PKG_FMT_VER_MNR	0
#define ICE_PKG_FMT_VER_UPD	0
#define ICE_PKG_FMT_VER_DFT	0

#define ICE_PKG_CNT 4

bool
ice_get_open_tunnel_port(struct ice_hw *hw, u16 *port);
int ice_udp_tunnel_set_port(struct net_device *netdev, unsigned int table,
			    unsigned int idx, struct udp_tunnel_info *ti);
int ice_udp_tunnel_unset_port(struct net_device *netdev, unsigned int table,
			      unsigned int idx, struct udp_tunnel_info *ti);

enum ice_status
ice_add_prof(struct ice_hw *hw, enum ice_block blk, u64 id, u8 ptypes[],
	     const struct ice_ptype_attributes *attr, u16 attr_cnt,
	     struct ice_fv_word *es, u16 *masks);
enum ice_status
ice_add_prof_id_flow(struct ice_hw *hw, enum ice_block blk, u16 vsi, u64 hdl);
enum ice_status
ice_rem_prof_id_flow(struct ice_hw *hw, enum ice_block blk, u16 vsi, u64 hdl);
enum ice_status ice_init_pkg(struct ice_hw *hw, u8 *buff, u32 len);
enum ice_status
ice_copy_and_init_pkg(struct ice_hw *hw, const u8 *buf, u32 len);
enum ice_status ice_init_hw_tbls(struct ice_hw *hw);
void ice_free_seg(struct ice_hw *hw);
void ice_fill_blk_tbls(struct ice_hw *hw);
void ice_clear_hw_tbls(struct ice_hw *hw);
void ice_free_hw_tbls(struct ice_hw *hw);
enum ice_status
ice_rem_prof(struct ice_hw *hw, enum ice_block blk, u64 id);
#endif /* _ICE_FLEX_PIPE_H_ */
