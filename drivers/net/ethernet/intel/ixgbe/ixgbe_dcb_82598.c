// SPDX-License-Identifier: GPL-2.0
/* Copyright(c) 1999 - 2018 Intel Corporation. */

#include "ixgbe.h"
#include "ixgbe_type.h"
#include "ixgbe_dcb.h"
#include "ixgbe_dcb_82598.h"

/**
 * ixgbe_dcb_config_rx_arbiter_82598 - Config Rx data arbiter
 * @hw: pointer to hardware structure
 * @refill: refill credits index by traffic class
 * @max: max credits index by traffic class
 * @prio_type: priority type indexed by traffic class
 *
 * Configure Rx Data Arbiter and credits for each traffic class.
 */
s32 ixgbe_dcb_config_rx_arbiter_82598(struct ixgbe_hw *hw,
					u16 *refill,
					u16 *max,
					u8 *prio_type)
{
	u32    reg           = 0;
	u32    credit_refill = 0;
	u32    credit_max    = 0;
	u8     i             = 0;

	reg = IXGBE_READ_REG(hw, IXGBE_RUPPBMR) | IXGBE_RUPPBMR_MQA;
	IXGBE_WRITE_REG(hw, IXGBE_RUPPBMR, reg);

	reg = IXGBE_READ_REG(hw, IXGBE_RMCS);
	/* Enable Arbiter */
	reg &= ~IXGBE_RMCS_ARBDIS;
	/* Enable Receive Recycle within the BWG */
	reg |= IXGBE_RMCS_RRM;
	/* Enable Deficit Fixed Priority arbitration*/
	reg |= IXGBE_RMCS_DFP;

	IXGBE_WRITE_REG(hw, IXGBE_RMCS, reg);

	/* Configure traffic class credits and priority */
	for (i = 0; i < MAX_TRAFFIC_CLASS; i++) {
		credit_refill = refill[i];
		credit_max    = max[i];

		reg = credit_refill | (credit_max << IXGBE_RT2CR_MCL_SHIFT);

		if (prio_type[i] == prio_link)
			reg |= IXGBE_RT2CR_LSP;

		IXGBE_WRITE_REG(hw, IXGBE_RT2CR(i), reg);
	}

	reg = IXGBE_READ_REG(hw, IXGBE_RDRXCTL);
	reg |= IXGBE_RDRXCTL_RDMTS_1_2;
	reg |= IXGBE_RDRXCTL_MPBEN;
	reg |= IXGBE_RDRXCTL_MCEN;
	IXGBE_WRITE_REG(hw, IXGBE_RDRXCTL, reg);

	reg = IXGBE_READ_REG(hw, IXGBE_RXCTRL);
	/* Make sure there is enough descriptors before arbitration */
	reg &= ~IXGBE_RXCTRL_DMBYPS;
	IXGBE_WRITE_REG(hw, IXGBE_RXCTRL, reg);

	return 0;
}

/**
 * ixgbe_dcb_config_tx_desc_arbiter_82598 - Config Tx Desc. arbiter
 * @hw: pointer to hardware structure
 * @refill: refill credits index by traffic class
 * @max: max credits index by traffic class
 * @bwg_id: bandwidth grouping indexed by traffic class
 * @prio_type: priority type indexed by traffic class
 *
 * Configure Tx Descriptor Arbiter and credits for each traffic class.
 */
s32 ixgbe_dcb_config_tx_desc_arbiter_82598(struct ixgbe_hw *hw,
						u16 *refill,
						u16 *max,
						u8 *bwg_id,
						u8 *prio_type)
{
	u32    reg, max_credits;
	u8     i;

	reg = IXGBE_READ_REG(hw, IXGBE_DPMCS);

	/* Enable arbiter */
	reg &= ~IXGBE_DPMCS_ARBDIS;
	reg |= IXGBE_DPMCS_TSOEF;

	/* Configure Max TSO packet size 34KB including payload and headers */
	reg |= (0x4 << IXGBE_DPMCS_MTSOS_SHIFT);

	IXGBE_WRITE_REG(hw, IXGBE_DPMCS, reg);

	/* Configure traffic class credits and priority */
	for (i = 0; i < MAX_TRAFFIC_CLASS; i++) {
		max_credits = max[i];
		reg = max_credits << IXGBE_TDTQ2TCCR_MCL_SHIFT;
		reg |= refill[i];
		reg |= (u32)(bwg_id[i]) << IXGBE_TDTQ2TCCR_BWG_SHIFT;

		if (prio_type[i] == prio_group)
			reg |= IXGBE_TDTQ2TCCR_GSP;

		if (prio_type[i] == prio_link)
			reg |= IXGBE_TDTQ2TCCR_LSP;

		IXGBE_WRITE_REG(hw, IXGBE_TDTQ2TCCR(i), reg);
	}

	return 0;
}

/**
 * ixgbe_dcb_config_tx_data_arbiter_82598 - Config Tx data arbiter
 * @hw: pointer to hardware structure
 * @refill: refill credits index by traffic class
 * @max: max credits index by traffic class
 * @bwg_id: bandwidth grouping indexed by traffic class
 * @prio_type: priority type indexed by traffic class
 *
 * Configure Tx Data Arbiter and credits for each traffic class.
 */
s32 ixgbe_dcb_config_tx_data_arbiter_82598(struct ixgbe_hw *hw,
						u16 *refill,
						u16 *max,
						u8 *bwg_id,
						u8 *prio_type)
{
	u32 reg;
	u8 i;

	reg = IXGBE_READ_REG(hw, IXGBE_PDPMCS);
	/* Enable Data Plane Arbiter */
	reg &= ~IXGBE_PDPMCS_ARBDIS;
	/* Enable DFP and Transmit Recycle Mode */
	reg |= (IXGBE_PDPMCS_TPPAC | IXGBE_PDPMCS_TRM);

	IXGBE_WRITE_REG(hw, IXGBE_PDPMCS, reg);

	/* Configure traffic class credits and priority */
	for (i = 0; i < MAX_TRAFFIC_CLASS; i++) {
		reg = refill[i];
		reg |= (u32)(max[i]) << IXGBE_TDPT2TCCR_MCL_SHIFT;
		reg |= (u32)(bwg_id[i]) << IXGBE_TDPT2TCCR_BWG_SHIFT;

		if (prio_type[i] == prio_group)
			reg |= IXGBE_TDPT2TCCR_GSP;

		if (prio_type[i] == prio_link)
			reg |= IXGBE_TDPT2TCCR_LSP;

		IXGBE_WRITE_REG(hw, IXGBE_TDPT2TCCR(i), reg);
	}

	/* Enable Tx packet buffer division */
	reg = IXGBE_READ_REG(hw, IXGBE_DTXCTL);
	reg |= IXGBE_DTXCTL_ENDBUBD;
	IXGBE_WRITE_REG(hw, IXGBE_DTXCTL, reg);

	return 0;
}

/**
 * ixgbe_dcb_config_pfc_82598 - Config priority flow control
 * @hw: pointer to hardware structure
 * @pfc_en: enabled pfc bitmask
 *
 * Configure Priority Flow Control for each traffic class.
 */
s32 ixgbe_dcb_config_pfc_82598(struct ixgbe_hw *hw, u8 pfc_en)
{
	u32 fcrtl, reg;
	u8  i;

	/* Enable Transmit Priority Flow Control */
	reg = IXGBE_READ_REG(hw, IXGBE_RMCS);
	reg &= ~IXGBE_RMCS_TFCE_802_3X;
	reg |= IXGBE_RMCS_TFCE_PRIORITY;
	IXGBE_WRITE_REG(hw, IXGBE_RMCS, reg);

	/* Enable Receive Priority Flow Control */
	reg = IXGBE_READ_REG(hw, IXGBE_FCTRL);
	reg &= ~(IXGBE_FCTRL_RPFCE | IXGBE_FCTRL_RFCE);

	if (pfc_en)
		reg |= IXGBE_FCTRL_RPFCE;

	IXGBE_WRITE_REG(hw, IXGBE_FCTRL, reg);

	/* Configure PFC Tx thresholds per TC */
	for (i = 0; i < MAX_TRAFFIC_CLASS; i++) {
		if (!(pfc_en & BIT(i))) {
			IXGBE_WRITE_REG(hw, IXGBE_FCRTL(i), 0);
			IXGBE_WRITE_REG(hw, IXGBE_FCRTH(i), 0);
			continue;
		}

		fcrtl = (hw->fc.low_water[i] << 10) | IXGBE_FCRTL_XONE;
		reg = (hw->fc.high_water[i] << 10) | IXGBE_FCRTH_FCEN;
		IXGBE_WRITE_REG(hw, IXGBE_FCRTL(i), fcrtl);
		IXGBE_WRITE_REG(hw, IXGBE_FCRTH(i), reg);
	}

	/* Configure pause time */
	reg = hw->fc.pause_time * 0x00010001;
	for (i = 0; i < (MAX_TRAFFIC_CLASS / 2); i++)
		IXGBE_WRITE_REG(hw, IXGBE_FCTTV(i), reg);

	/* Configure flow control refresh threshold value */
	IXGBE_WRITE_REG(hw, IXGBE_FCRTV, hw->fc.pause_time / 2);


	return 0;
}

/**
 * ixgbe_dcb_config_tc_stats_82598 - Configure traffic class statistics
 * @hw: pointer to hardware structure
 *
 * Configure queue statistics registers, all queues belonging to same traffic
 * class uses a single set of queue statistics counters.
 */
static s32 ixgbe_dcb_config_tc_stats_82598(struct ixgbe_hw *hw)
{
	u32 reg = 0;
	u8  i   = 0;
	u8  j   = 0;

	/* Receive Queues stats setting -  8 queues per statistics reg */
	for (i = 0, j = 0; i < 15 && j < 8; i = i + 2, j++) {
		reg = IXGBE_READ_REG(hw, IXGBE_RQSMR(i));
		reg |= ((0x1010101) * j);
		IXGBE_WRITE_REG(hw, IXGBE_RQSMR(i), reg);
		reg = IXGBE_READ_REG(hw, IXGBE_RQSMR(i + 1));
		reg |= ((0x1010101) * j);
		IXGBE_WRITE_REG(hw, IXGBE_RQSMR(i + 1), reg);
	}
	/* Transmit Queues stats setting -  4 queues per statistics reg */
	for (i = 0; i < 8; i++) {
		reg = IXGBE_READ_REG(hw, IXGBE_TQSMR(i));
		reg |= ((0x1010101) * i);
		IXGBE_WRITE_REG(hw, IXGBE_TQSMR(i), reg);
	}

	return 0;
}

/**
 * ixgbe_dcb_hw_config_82598 - Config and enable DCB
 * @hw: pointer to hardware structure
 * @pfc_en: enabled pfc bitmask
 * @refill: refill credits index by traffic class
 * @max: max credits index by traffic class
 * @bwg_id: bandwidth grouping indexed by traffic class
 * @prio_type: priority type indexed by traffic class
 *
 * Configure dcb settings and enable dcb mode.
 */
s32 ixgbe_dcb_hw_config_82598(struct ixgbe_hw *hw, u8 pfc_en, u16 *refill,
			      u16 *max, u8 *bwg_id, u8 *prio_type)
{
	ixgbe_dcb_config_rx_arbiter_82598(hw, refill, max, prio_type);
	ixgbe_dcb_config_tx_desc_arbiter_82598(hw, refill, max,
					       bwg_id, prio_type);
	ixgbe_dcb_config_tx_data_arbiter_82598(hw, refill, max,
					       bwg_id, prio_type);
	ixgbe_dcb_config_pfc_82598(hw, pfc_en);
	ixgbe_dcb_config_tc_stats_82598(hw);

	return 0;
}
