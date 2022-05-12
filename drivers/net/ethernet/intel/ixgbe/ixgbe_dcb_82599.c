// SPDX-License-Identifier: GPL-2.0
/* Copyright(c) 1999 - 2018 Intel Corporation. */

#include "ixgbe.h"
#include "ixgbe_type.h"
#include "ixgbe_dcb.h"
#include "ixgbe_dcb_82599.h"

/**
 * ixgbe_dcb_config_rx_arbiter_82599 - Config Rx Data arbiter
 * @hw: pointer to hardware structure
 * @refill: refill credits index by traffic class
 * @max: max credits index by traffic class
 * @bwg_id: bandwidth grouping indexed by traffic class
 * @prio_type: priority type indexed by traffic class
 * @prio_tc: priority to tc assignments indexed by priority
 *
 * Configure Rx Packet Arbiter and credits for each traffic class.
 */
s32 ixgbe_dcb_config_rx_arbiter_82599(struct ixgbe_hw *hw,
				      u16 *refill,
				      u16 *max,
				      u8 *bwg_id,
				      u8 *prio_type,
				      u8 *prio_tc)
{
	u32    reg           = 0;
	u32    credit_refill = 0;
	u32    credit_max    = 0;
	u8     i             = 0;

	/*
	 * Disable the arbiter before changing parameters
	 * (always enable recycle mode; WSP)
	 */
	reg = IXGBE_RTRPCS_RRM | IXGBE_RTRPCS_RAC | IXGBE_RTRPCS_ARBDIS;
	IXGBE_WRITE_REG(hw, IXGBE_RTRPCS, reg);

	/* Map all traffic classes to their UP */
	reg = 0;
	for (i = 0; i < MAX_USER_PRIORITY; i++)
		reg |= (prio_tc[i] << (i * IXGBE_RTRUP2TC_UP_SHIFT));
	IXGBE_WRITE_REG(hw, IXGBE_RTRUP2TC, reg);

	/* Configure traffic class credits and priority */
	for (i = 0; i < MAX_TRAFFIC_CLASS; i++) {
		credit_refill = refill[i];
		credit_max    = max[i];
		reg = credit_refill | (credit_max << IXGBE_RTRPT4C_MCL_SHIFT);

		reg |= (u32)(bwg_id[i]) << IXGBE_RTRPT4C_BWG_SHIFT;

		if (prio_type[i] == prio_link)
			reg |= IXGBE_RTRPT4C_LSP;

		IXGBE_WRITE_REG(hw, IXGBE_RTRPT4C(i), reg);
	}

	/*
	 * Configure Rx packet plane (recycle mode; WSP) and
	 * enable arbiter
	 */
	reg = IXGBE_RTRPCS_RRM | IXGBE_RTRPCS_RAC;
	IXGBE_WRITE_REG(hw, IXGBE_RTRPCS, reg);

	return 0;
}

/**
 * ixgbe_dcb_config_tx_desc_arbiter_82599 - Config Tx Desc. arbiter
 * @hw: pointer to hardware structure
 * @refill: refill credits index by traffic class
 * @max: max credits index by traffic class
 * @bwg_id: bandwidth grouping indexed by traffic class
 * @prio_type: priority type indexed by traffic class
 *
 * Configure Tx Descriptor Arbiter and credits for each traffic class.
 */
s32 ixgbe_dcb_config_tx_desc_arbiter_82599(struct ixgbe_hw *hw,
					   u16 *refill,
					   u16 *max,
					   u8 *bwg_id,
					   u8 *prio_type)
{
	u32    reg, max_credits;
	u8     i;

	/* Clear the per-Tx queue credits; we use per-TC instead */
	for (i = 0; i < 128; i++) {
		IXGBE_WRITE_REG(hw, IXGBE_RTTDQSEL, i);
		IXGBE_WRITE_REG(hw, IXGBE_RTTDT1C, 0);
	}

	/* Configure traffic class credits and priority */
	for (i = 0; i < MAX_TRAFFIC_CLASS; i++) {
		max_credits = max[i];
		reg = max_credits << IXGBE_RTTDT2C_MCL_SHIFT;
		reg |= refill[i];
		reg |= (u32)(bwg_id[i]) << IXGBE_RTTDT2C_BWG_SHIFT;

		if (prio_type[i] == prio_group)
			reg |= IXGBE_RTTDT2C_GSP;

		if (prio_type[i] == prio_link)
			reg |= IXGBE_RTTDT2C_LSP;

		IXGBE_WRITE_REG(hw, IXGBE_RTTDT2C(i), reg);
	}

	/*
	 * Configure Tx descriptor plane (recycle mode; WSP) and
	 * enable arbiter
	 */
	reg = IXGBE_RTTDCS_TDPAC | IXGBE_RTTDCS_TDRM;
	IXGBE_WRITE_REG(hw, IXGBE_RTTDCS, reg);

	return 0;
}

/**
 * ixgbe_dcb_config_tx_data_arbiter_82599 - Config Tx Data arbiter
 * @hw: pointer to hardware structure
 * @refill: refill credits index by traffic class
 * @max: max credits index by traffic class
 * @bwg_id: bandwidth grouping indexed by traffic class
 * @prio_type: priority type indexed by traffic class
 * @prio_tc: priority to tc assignments indexed by priority
 *
 * Configure Tx Packet Arbiter and credits for each traffic class.
 */
s32 ixgbe_dcb_config_tx_data_arbiter_82599(struct ixgbe_hw *hw,
					   u16 *refill,
					   u16 *max,
					   u8 *bwg_id,
					   u8 *prio_type,
					   u8 *prio_tc)
{
	u32 reg;
	u8 i;

	/*
	 * Disable the arbiter before changing parameters
	 * (always enable recycle mode; SP; arb delay)
	 */
	reg = IXGBE_RTTPCS_TPPAC | IXGBE_RTTPCS_TPRM |
	      (IXGBE_RTTPCS_ARBD_DCB << IXGBE_RTTPCS_ARBD_SHIFT) |
	      IXGBE_RTTPCS_ARBDIS;
	IXGBE_WRITE_REG(hw, IXGBE_RTTPCS, reg);

	/* Map all traffic classes to their UP */
	reg = 0;
	for (i = 0; i < MAX_USER_PRIORITY; i++)
		reg |= (prio_tc[i] << (i * IXGBE_RTTUP2TC_UP_SHIFT));
	IXGBE_WRITE_REG(hw, IXGBE_RTTUP2TC, reg);

	/* Configure traffic class credits and priority */
	for (i = 0; i < MAX_TRAFFIC_CLASS; i++) {
		reg = refill[i];
		reg |= (u32)(max[i]) << IXGBE_RTTPT2C_MCL_SHIFT;
		reg |= (u32)(bwg_id[i]) << IXGBE_RTTPT2C_BWG_SHIFT;

		if (prio_type[i] == prio_group)
			reg |= IXGBE_RTTPT2C_GSP;

		if (prio_type[i] == prio_link)
			reg |= IXGBE_RTTPT2C_LSP;

		IXGBE_WRITE_REG(hw, IXGBE_RTTPT2C(i), reg);
	}

	/*
	 * Configure Tx packet plane (recycle mode; SP; arb delay) and
	 * enable arbiter
	 */
	reg = IXGBE_RTTPCS_TPPAC | IXGBE_RTTPCS_TPRM |
	      (IXGBE_RTTPCS_ARBD_DCB << IXGBE_RTTPCS_ARBD_SHIFT);
	IXGBE_WRITE_REG(hw, IXGBE_RTTPCS, reg);

	return 0;
}

/**
 * ixgbe_dcb_config_pfc_82599 - Configure priority flow control
 * @hw: pointer to hardware structure
 * @pfc_en: enabled pfc bitmask
 * @prio_tc: priority to tc assignments indexed by priority
 *
 * Configure Priority Flow Control (PFC) for each traffic class.
 */
s32 ixgbe_dcb_config_pfc_82599(struct ixgbe_hw *hw, u8 pfc_en, u8 *prio_tc)
{
	u32 i, j, fcrtl, reg;
	u8 max_tc = 0;

	/* Enable Transmit Priority Flow Control */
	IXGBE_WRITE_REG(hw, IXGBE_FCCFG, IXGBE_FCCFG_TFCE_PRIORITY);

	/* Enable Receive Priority Flow Control */
	reg = IXGBE_READ_REG(hw, IXGBE_MFLCN);
	reg |= IXGBE_MFLCN_DPF;

	/*
	 * X540 & X550 supports per TC Rx priority flow control.
	 * So clear all TCs and only enable those that should be
	 * enabled.
	 */
	reg &= ~(IXGBE_MFLCN_RPFCE_MASK | IXGBE_MFLCN_RFCE);

	if (hw->mac.type >= ixgbe_mac_X540)
		reg |= pfc_en << IXGBE_MFLCN_RPFCE_SHIFT;

	if (pfc_en)
		reg |= IXGBE_MFLCN_RPFCE;

	IXGBE_WRITE_REG(hw, IXGBE_MFLCN, reg);

	for (i = 0; i < MAX_USER_PRIORITY; i++) {
		if (prio_tc[i] > max_tc)
			max_tc = prio_tc[i];
	}


	/* Configure PFC Tx thresholds per TC */
	for (i = 0; i <= max_tc; i++) {
		int enabled = 0;

		for (j = 0; j < MAX_USER_PRIORITY; j++) {
			if ((prio_tc[j] == i) && (pfc_en & BIT(j))) {
				enabled = 1;
				break;
			}
		}

		if (enabled) {
			reg = (hw->fc.high_water[i] << 10) | IXGBE_FCRTH_FCEN;
			fcrtl = (hw->fc.low_water[i] << 10) | IXGBE_FCRTL_XONE;
			IXGBE_WRITE_REG(hw, IXGBE_FCRTL_82599(i), fcrtl);
		} else {
			/* In order to prevent Tx hangs when the internal Tx
			 * switch is enabled we must set the high water mark
			 * to the Rx packet buffer size - 24KB.  This allows
			 * the Tx switch to function even under heavy Rx
			 * workloads.
			 */
			reg = IXGBE_READ_REG(hw, IXGBE_RXPBSIZE(i)) - 24576;
			IXGBE_WRITE_REG(hw, IXGBE_FCRTL_82599(i), 0);
		}

		IXGBE_WRITE_REG(hw, IXGBE_FCRTH_82599(i), reg);
	}

	for (; i < MAX_TRAFFIC_CLASS; i++) {
		IXGBE_WRITE_REG(hw, IXGBE_FCRTL_82599(i), 0);
		IXGBE_WRITE_REG(hw, IXGBE_FCRTH_82599(i), 0);
	}

	/* Configure pause time (2 TCs per register) */
	reg = hw->fc.pause_time * 0x00010001;
	for (i = 0; i < (MAX_TRAFFIC_CLASS / 2); i++)
		IXGBE_WRITE_REG(hw, IXGBE_FCTTV(i), reg);

	/* Configure flow control refresh threshold value */
	IXGBE_WRITE_REG(hw, IXGBE_FCRTV, hw->fc.pause_time / 2);

	return 0;
}

/**
 * ixgbe_dcb_config_tc_stats_82599 - Config traffic class statistics
 * @hw: pointer to hardware structure
 *
 * Configure queue statistics registers, all queues belonging to same traffic
 * class uses a single set of queue statistics counters.
 */
static s32 ixgbe_dcb_config_tc_stats_82599(struct ixgbe_hw *hw)
{
	u32 reg = 0;
	u8  i   = 0;

	/*
	 * Receive Queues stats setting
	 * 32 RQSMR registers, each configuring 4 queues.
	 * Set all 16 queues of each TC to the same stat
	 * with TC 'n' going to stat 'n'.
	 */
	for (i = 0; i < 32; i++) {
		reg = 0x01010101 * (i / 4);
		IXGBE_WRITE_REG(hw, IXGBE_RQSMR(i), reg);
	}
	/*
	 * Transmit Queues stats setting
	 * 32 TQSM registers, each controlling 4 queues.
	 * Set all queues of each TC to the same stat
	 * with TC 'n' going to stat 'n'.
	 * Tx queues are allocated non-uniformly to TCs:
	 * 32, 32, 16, 16, 8, 8, 8, 8.
	 */
	for (i = 0; i < 32; i++) {
		if (i < 8)
			reg = 0x00000000;
		else if (i < 16)
			reg = 0x01010101;
		else if (i < 20)
			reg = 0x02020202;
		else if (i < 24)
			reg = 0x03030303;
		else if (i < 26)
			reg = 0x04040404;
		else if (i < 28)
			reg = 0x05050505;
		else if (i < 30)
			reg = 0x06060606;
		else
			reg = 0x07070707;
		IXGBE_WRITE_REG(hw, IXGBE_TQSM(i), reg);
	}

	return 0;
}

/**
 * ixgbe_dcb_hw_config_82599 - Configure and enable DCB
 * @hw: pointer to hardware structure
 * @pfc_en: enabled pfc bitmask
 * @refill: refill credits index by traffic class
 * @max: max credits index by traffic class
 * @bwg_id: bandwidth grouping indexed by traffic class
 * @prio_type: priority type indexed by traffic class
 * @prio_tc: priority to tc assignments indexed by priority
 *
 * Configure dcb settings and enable dcb mode.
 */
s32 ixgbe_dcb_hw_config_82599(struct ixgbe_hw *hw, u8 pfc_en, u16 *refill,
			      u16 *max, u8 *bwg_id, u8 *prio_type, u8 *prio_tc)
{
	ixgbe_dcb_config_rx_arbiter_82599(hw, refill, max, bwg_id,
					  prio_type, prio_tc);
	ixgbe_dcb_config_tx_desc_arbiter_82599(hw, refill, max,
					       bwg_id, prio_type);
	ixgbe_dcb_config_tx_data_arbiter_82599(hw, refill, max,
					       bwg_id, prio_type, prio_tc);
	ixgbe_dcb_config_pfc_82599(hw, pfc_en, prio_tc);
	ixgbe_dcb_config_tc_stats_82599(hw);

	return 0;
}

