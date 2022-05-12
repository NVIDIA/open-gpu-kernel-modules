/* SPDX-License-Identifier: GPL-2.0 */
/* Copyright(c) 1999 - 2018 Intel Corporation. */

#ifndef _DCB_82599_CONFIG_H_
#define _DCB_82599_CONFIG_H_

/* DCB register definitions */
#define IXGBE_RTTDCS_TDPAC      0x00000001 /* 0 Round Robin,
					    * 1 WSP - Weighted Strict Priority
					    */
#define IXGBE_RTTDCS_VMPAC      0x00000002 /* 0 Round Robin,
					    * 1 WRR - Weighted Round Robin
					    */
#define IXGBE_RTTDCS_TDRM       0x00000010 /* Transmit Recycle Mode */
#define IXGBE_RTTDCS_ARBDIS     0x00000040 /* DCB arbiter disable */
#define IXGBE_RTTDCS_BDPM       0x00400000 /* Bypass Data Pipe - must clear! */
#define IXGBE_RTTDCS_BPBFSM     0x00800000 /* Bypass PB Free Space - must
					     * clear!
					     */
#define IXGBE_RTTDCS_SPEED_CHG  0x80000000 /* Link speed change */

/* Receive UP2TC mapping */
#define IXGBE_RTRUP2TC_UP_SHIFT 3
#define IXGBE_RTRUP2TC_UP_MASK	7
/* Transmit UP2TC mapping */
#define IXGBE_RTTUP2TC_UP_SHIFT 3

#define IXGBE_RTRPT4C_MCL_SHIFT 12 /* Offset to Max Credit Limit setting */
#define IXGBE_RTRPT4C_BWG_SHIFT 9  /* Offset to BWG index */
#define IXGBE_RTRPT4C_GSP       0x40000000 /* GSP enable bit */
#define IXGBE_RTRPT4C_LSP       0x80000000 /* LSP enable bit */

#define IXGBE_RDRXCTL_MPBEN     0x00000010 /* DMA config for multiple packet
					    * buffers enable
					    */
#define IXGBE_RDRXCTL_MCEN      0x00000040 /* DMA config for multiple cores
					    * (RSS) enable
					    */

/* RTRPCS Bit Masks */
#define IXGBE_RTRPCS_RRM        0x00000002 /* Receive Recycle Mode enable */
/* Receive Arbitration Control: 0 Round Robin, 1 DFP */
#define IXGBE_RTRPCS_RAC        0x00000004
#define IXGBE_RTRPCS_ARBDIS     0x00000040 /* Arbitration disable bit */

/* RTTDT2C Bit Masks */
#define IXGBE_RTTDT2C_MCL_SHIFT 12
#define IXGBE_RTTDT2C_BWG_SHIFT 9
#define IXGBE_RTTDT2C_GSP       0x40000000
#define IXGBE_RTTDT2C_LSP       0x80000000

#define IXGBE_RTTPT2C_MCL_SHIFT 12
#define IXGBE_RTTPT2C_BWG_SHIFT 9
#define IXGBE_RTTPT2C_GSP       0x40000000
#define IXGBE_RTTPT2C_LSP       0x80000000

/* RTTPCS Bit Masks */
#define IXGBE_RTTPCS_TPPAC      0x00000020 /* 0 Round Robin,
					    * 1 SP - Strict Priority
					    */
#define IXGBE_RTTPCS_ARBDIS     0x00000040 /* Arbiter disable */
#define IXGBE_RTTPCS_TPRM       0x00000100 /* Transmit Recycle Mode enable */
#define IXGBE_RTTPCS_ARBD_SHIFT 22
#define IXGBE_RTTPCS_ARBD_DCB   0x4        /* Arbitration delay in DCB mode */

/* SECTXMINIFG DCB */
#define IXGBE_SECTX_DCB		0x00001F00 /* DCB TX Buffer IFG */


/* DCB hardware-specific driver APIs */

/* DCB PFC functions */
s32 ixgbe_dcb_config_pfc_82599(struct ixgbe_hw *hw, u8 pfc_en, u8 *prio_tc);

/* DCB hw initialization */
s32 ixgbe_dcb_config_rx_arbiter_82599(struct ixgbe_hw *hw,
					u16 *refill,
					u16 *max,
					u8 *bwg_id,
					u8 *prio_type,
					u8 *prio_tc);

s32 ixgbe_dcb_config_tx_desc_arbiter_82599(struct ixgbe_hw *hw,
						u16 *refill,
						u16 *max,
						u8 *bwg_id,
						u8 *prio_type);

s32 ixgbe_dcb_config_tx_data_arbiter_82599(struct ixgbe_hw *hw,
						u16 *refill,
						u16 *max,
						u8 *bwg_id,
						u8 *prio_type,
						u8 *prio_tc);

s32 ixgbe_dcb_hw_config_82599(struct ixgbe_hw *hw, u8 pfc_en, u16 *refill,
			      u16 *max, u8 *bwg_id, u8 *prio_type,
			      u8 *prio_tc);

#endif /* _DCB_82599_CONFIG_H */
