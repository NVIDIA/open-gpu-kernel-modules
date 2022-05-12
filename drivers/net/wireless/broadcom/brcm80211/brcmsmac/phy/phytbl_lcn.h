// SPDX-License-Identifier: ISC
/*
 * Copyright (c) 2010 Broadcom Corporation
 */

#include <types.h>
#include "phy_int.h"

extern const struct phytbl_info dot11lcnphytbl_rx_gain_info_rev0[];
extern const u32 dot11lcnphytbl_rx_gain_info_sz_rev0;
extern const struct phytbl_info dot11lcn_sw_ctrl_tbl_info_4313;
extern const struct phytbl_info dot11lcn_sw_ctrl_tbl_info_4313_bt_ipa;
extern const struct phytbl_info dot11lcn_sw_ctrl_tbl_info_4313_epa;
extern const struct phytbl_info dot11lcn_sw_ctrl_tbl_info_4313_epa_combo;
extern const struct phytbl_info dot11lcn_sw_ctrl_tbl_info_4313_bt_epa;
extern const struct phytbl_info dot11lcn_sw_ctrl_tbl_info_4313_bt_epa_p250;

extern const struct phytbl_info dot11lcnphytbl_info_rev0[];
extern const u32 dot11lcnphytbl_info_sz_rev0;

extern const struct phytbl_info dot11lcnphytbl_rx_gain_info_2G_rev2[];
extern const u32 dot11lcnphytbl_rx_gain_info_2G_rev2_sz;

extern const struct phytbl_info dot11lcnphytbl_rx_gain_info_5G_rev2[];
extern const u32 dot11lcnphytbl_rx_gain_info_5G_rev2_sz;

extern const struct phytbl_info dot11lcnphytbl_rx_gain_info_extlna_2G_rev2[];

extern const struct phytbl_info dot11lcnphytbl_rx_gain_info_extlna_5G_rev2[];

struct lcnphy_tx_gain_tbl_entry {
	unsigned char gm;
	unsigned char pga;
	unsigned char pad;
	unsigned char dac;
	unsigned char bb_mult;
};

extern const struct lcnphy_tx_gain_tbl_entry dot11lcnphy_2GHz_gaintable_rev0[];

extern const struct
lcnphy_tx_gain_tbl_entry dot11lcnphy_2GHz_extPA_gaintable_rev0[];

extern const struct lcnphy_tx_gain_tbl_entry dot11lcnphy_5GHz_gaintable_rev0[];
