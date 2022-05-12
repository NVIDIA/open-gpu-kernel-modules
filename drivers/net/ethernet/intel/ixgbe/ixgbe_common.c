// SPDX-License-Identifier: GPL-2.0
/* Copyright(c) 1999 - 2018 Intel Corporation. */

#include <linux/pci.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/netdevice.h>

#include "ixgbe.h"
#include "ixgbe_common.h"
#include "ixgbe_phy.h"

static s32 ixgbe_acquire_eeprom(struct ixgbe_hw *hw);
static s32 ixgbe_get_eeprom_semaphore(struct ixgbe_hw *hw);
static void ixgbe_release_eeprom_semaphore(struct ixgbe_hw *hw);
static s32 ixgbe_ready_eeprom(struct ixgbe_hw *hw);
static void ixgbe_standby_eeprom(struct ixgbe_hw *hw);
static void ixgbe_shift_out_eeprom_bits(struct ixgbe_hw *hw, u16 data,
					u16 count);
static u16 ixgbe_shift_in_eeprom_bits(struct ixgbe_hw *hw, u16 count);
static void ixgbe_raise_eeprom_clk(struct ixgbe_hw *hw, u32 *eec);
static void ixgbe_lower_eeprom_clk(struct ixgbe_hw *hw, u32 *eec);
static void ixgbe_release_eeprom(struct ixgbe_hw *hw);

static s32 ixgbe_mta_vector(struct ixgbe_hw *hw, u8 *mc_addr);
static s32 ixgbe_poll_eerd_eewr_done(struct ixgbe_hw *hw, u32 ee_reg);
static s32 ixgbe_read_eeprom_buffer_bit_bang(struct ixgbe_hw *hw, u16 offset,
					     u16 words, u16 *data);
static s32 ixgbe_write_eeprom_buffer_bit_bang(struct ixgbe_hw *hw, u16 offset,
					     u16 words, u16 *data);
static s32 ixgbe_detect_eeprom_page_size_generic(struct ixgbe_hw *hw,
						 u16 offset);
static s32 ixgbe_disable_pcie_master(struct ixgbe_hw *hw);

/* Base table for registers values that change by MAC */
const u32 ixgbe_mvals_8259X[IXGBE_MVALS_IDX_LIMIT] = {
	IXGBE_MVALS_INIT(8259X)
};

/**
 *  ixgbe_device_supports_autoneg_fc - Check if phy supports autoneg flow
 *  control
 *  @hw: pointer to hardware structure
 *
 *  There are several phys that do not support autoneg flow control. This
 *  function check the device id to see if the associated phy supports
 *  autoneg flow control.
 **/
bool ixgbe_device_supports_autoneg_fc(struct ixgbe_hw *hw)
{
	bool supported = false;
	ixgbe_link_speed speed;
	bool link_up;

	switch (hw->phy.media_type) {
	case ixgbe_media_type_fiber:
		/* flow control autoneg black list */
		switch (hw->device_id) {
		case IXGBE_DEV_ID_X550EM_A_SFP:
		case IXGBE_DEV_ID_X550EM_A_SFP_N:
			supported = false;
			break;
		default:
			hw->mac.ops.check_link(hw, &speed, &link_up, false);
			/* if link is down, assume supported */
			if (link_up)
				supported = speed == IXGBE_LINK_SPEED_1GB_FULL;
			else
				supported = true;
		}

		break;
	case ixgbe_media_type_backplane:
		if (hw->device_id == IXGBE_DEV_ID_X550EM_X_XFI)
			supported = false;
		else
			supported = true;
		break;
	case ixgbe_media_type_copper:
		/* only some copper devices support flow control autoneg */
		switch (hw->device_id) {
		case IXGBE_DEV_ID_82599_T3_LOM:
		case IXGBE_DEV_ID_X540T:
		case IXGBE_DEV_ID_X540T1:
		case IXGBE_DEV_ID_X550T:
		case IXGBE_DEV_ID_X550T1:
		case IXGBE_DEV_ID_X550EM_X_10G_T:
		case IXGBE_DEV_ID_X550EM_A_10G_T:
		case IXGBE_DEV_ID_X550EM_A_1G_T:
		case IXGBE_DEV_ID_X550EM_A_1G_T_L:
			supported = true;
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}

	if (!supported)
		hw_dbg(hw, "Device %x does not support flow control autoneg\n",
		       hw->device_id);

	return supported;
}

/**
 *  ixgbe_setup_fc_generic - Set up flow control
 *  @hw: pointer to hardware structure
 *
 *  Called at init time to set up flow control.
 **/
s32 ixgbe_setup_fc_generic(struct ixgbe_hw *hw)
{
	s32 ret_val = 0;
	u32 reg = 0, reg_bp = 0;
	u16 reg_cu = 0;
	bool locked = false;

	/*
	 * Validate the requested mode.  Strict IEEE mode does not allow
	 * ixgbe_fc_rx_pause because it will cause us to fail at UNH.
	 */
	if (hw->fc.strict_ieee && hw->fc.requested_mode == ixgbe_fc_rx_pause) {
		hw_dbg(hw, "ixgbe_fc_rx_pause not valid in strict IEEE mode\n");
		return IXGBE_ERR_INVALID_LINK_SETTINGS;
	}

	/*
	 * 10gig parts do not have a word in the EEPROM to determine the
	 * default flow control setting, so we explicitly set it to full.
	 */
	if (hw->fc.requested_mode == ixgbe_fc_default)
		hw->fc.requested_mode = ixgbe_fc_full;

	/*
	 * Set up the 1G and 10G flow control advertisement registers so the
	 * HW will be able to do fc autoneg once the cable is plugged in.  If
	 * we link at 10G, the 1G advertisement is harmless and vice versa.
	 */
	switch (hw->phy.media_type) {
	case ixgbe_media_type_backplane:
		/* some MAC's need RMW protection on AUTOC */
		ret_val = hw->mac.ops.prot_autoc_read(hw, &locked, &reg_bp);
		if (ret_val)
			return ret_val;

		fallthrough; /* only backplane uses autoc */
	case ixgbe_media_type_fiber:
		reg = IXGBE_READ_REG(hw, IXGBE_PCS1GANA);

		break;
	case ixgbe_media_type_copper:
		hw->phy.ops.read_reg(hw, MDIO_AN_ADVERTISE,
					MDIO_MMD_AN, &reg_cu);
		break;
	default:
		break;
	}

	/*
	 * The possible values of fc.requested_mode are:
	 * 0: Flow control is completely disabled
	 * 1: Rx flow control is enabled (we can receive pause frames,
	 *    but not send pause frames).
	 * 2: Tx flow control is enabled (we can send pause frames but
	 *    we do not support receiving pause frames).
	 * 3: Both Rx and Tx flow control (symmetric) are enabled.
	 * other: Invalid.
	 */
	switch (hw->fc.requested_mode) {
	case ixgbe_fc_none:
		/* Flow control completely disabled by software override. */
		reg &= ~(IXGBE_PCS1GANA_SYM_PAUSE | IXGBE_PCS1GANA_ASM_PAUSE);
		if (hw->phy.media_type == ixgbe_media_type_backplane)
			reg_bp &= ~(IXGBE_AUTOC_SYM_PAUSE |
				    IXGBE_AUTOC_ASM_PAUSE);
		else if (hw->phy.media_type == ixgbe_media_type_copper)
			reg_cu &= ~(IXGBE_TAF_SYM_PAUSE | IXGBE_TAF_ASM_PAUSE);
		break;
	case ixgbe_fc_tx_pause:
		/*
		 * Tx Flow control is enabled, and Rx Flow control is
		 * disabled by software override.
		 */
		reg |= IXGBE_PCS1GANA_ASM_PAUSE;
		reg &= ~IXGBE_PCS1GANA_SYM_PAUSE;
		if (hw->phy.media_type == ixgbe_media_type_backplane) {
			reg_bp |= IXGBE_AUTOC_ASM_PAUSE;
			reg_bp &= ~IXGBE_AUTOC_SYM_PAUSE;
		} else if (hw->phy.media_type == ixgbe_media_type_copper) {
			reg_cu |= IXGBE_TAF_ASM_PAUSE;
			reg_cu &= ~IXGBE_TAF_SYM_PAUSE;
		}
		break;
	case ixgbe_fc_rx_pause:
		/*
		 * Rx Flow control is enabled and Tx Flow control is
		 * disabled by software override. Since there really
		 * isn't a way to advertise that we are capable of RX
		 * Pause ONLY, we will advertise that we support both
		 * symmetric and asymmetric Rx PAUSE, as such we fall
		 * through to the fc_full statement.  Later, we will
		 * disable the adapter's ability to send PAUSE frames.
		 */
	case ixgbe_fc_full:
		/* Flow control (both Rx and Tx) is enabled by SW override. */
		reg |= IXGBE_PCS1GANA_SYM_PAUSE | IXGBE_PCS1GANA_ASM_PAUSE;
		if (hw->phy.media_type == ixgbe_media_type_backplane)
			reg_bp |= IXGBE_AUTOC_SYM_PAUSE |
				  IXGBE_AUTOC_ASM_PAUSE;
		else if (hw->phy.media_type == ixgbe_media_type_copper)
			reg_cu |= IXGBE_TAF_SYM_PAUSE | IXGBE_TAF_ASM_PAUSE;
		break;
	default:
		hw_dbg(hw, "Flow control param set incorrectly\n");
		return IXGBE_ERR_CONFIG;
	}

	if (hw->mac.type != ixgbe_mac_X540) {
		/*
		 * Enable auto-negotiation between the MAC & PHY;
		 * the MAC will advertise clause 37 flow control.
		 */
		IXGBE_WRITE_REG(hw, IXGBE_PCS1GANA, reg);
		reg = IXGBE_READ_REG(hw, IXGBE_PCS1GLCTL);

		/* Disable AN timeout */
		if (hw->fc.strict_ieee)
			reg &= ~IXGBE_PCS1GLCTL_AN_1G_TIMEOUT_EN;

		IXGBE_WRITE_REG(hw, IXGBE_PCS1GLCTL, reg);
		hw_dbg(hw, "Set up FC; PCS1GLCTL = 0x%08X\n", reg);
	}

	/*
	 * AUTOC restart handles negotiation of 1G and 10G on backplane
	 * and copper. There is no need to set the PCS1GCTL register.
	 *
	 */
	if (hw->phy.media_type == ixgbe_media_type_backplane) {
		/* Need the SW/FW semaphore around AUTOC writes if 82599 and
		 * LESM is on, likewise reset_pipeline requries the lock as
		 * it also writes AUTOC.
		 */
		ret_val = hw->mac.ops.prot_autoc_write(hw, reg_bp, locked);
		if (ret_val)
			return ret_val;

	} else if ((hw->phy.media_type == ixgbe_media_type_copper) &&
		   ixgbe_device_supports_autoneg_fc(hw)) {
		hw->phy.ops.write_reg(hw, MDIO_AN_ADVERTISE,
				      MDIO_MMD_AN, reg_cu);
	}

	hw_dbg(hw, "Set up FC; IXGBE_AUTOC = 0x%08X\n", reg);
	return ret_val;
}

/**
 *  ixgbe_start_hw_generic - Prepare hardware for Tx/Rx
 *  @hw: pointer to hardware structure
 *
 *  Starts the hardware by filling the bus info structure and media type, clears
 *  all on chip counters, initializes receive address registers, multicast
 *  table, VLAN filter table, calls routine to set up link and flow control
 *  settings, and leaves transmit and receive units disabled and uninitialized
 **/
s32 ixgbe_start_hw_generic(struct ixgbe_hw *hw)
{
	s32 ret_val;
	u32 ctrl_ext;
	u16 device_caps;

	/* Set the media type */
	hw->phy.media_type = hw->mac.ops.get_media_type(hw);

	/* Identify the PHY */
	hw->phy.ops.identify(hw);

	/* Clear the VLAN filter table */
	hw->mac.ops.clear_vfta(hw);

	/* Clear statistics registers */
	hw->mac.ops.clear_hw_cntrs(hw);

	/* Set No Snoop Disable */
	ctrl_ext = IXGBE_READ_REG(hw, IXGBE_CTRL_EXT);
	ctrl_ext |= IXGBE_CTRL_EXT_NS_DIS;
	IXGBE_WRITE_REG(hw, IXGBE_CTRL_EXT, ctrl_ext);
	IXGBE_WRITE_FLUSH(hw);

	/* Setup flow control if method for doing so */
	if (hw->mac.ops.setup_fc) {
		ret_val = hw->mac.ops.setup_fc(hw);
		if (ret_val)
			return ret_val;
	}

	/* Cashe bit indicating need for crosstalk fix */
	switch (hw->mac.type) {
	case ixgbe_mac_82599EB:
	case ixgbe_mac_X550EM_x:
	case ixgbe_mac_x550em_a:
		hw->mac.ops.get_device_caps(hw, &device_caps);
		if (device_caps & IXGBE_DEVICE_CAPS_NO_CROSSTALK_WR)
			hw->need_crosstalk_fix = false;
		else
			hw->need_crosstalk_fix = true;
		break;
	default:
		hw->need_crosstalk_fix = false;
		break;
	}

	/* Clear adapter stopped flag */
	hw->adapter_stopped = false;

	return 0;
}

/**
 *  ixgbe_start_hw_gen2 - Init sequence for common device family
 *  @hw: pointer to hw structure
 *
 * Performs the init sequence common to the second generation
 * of 10 GbE devices.
 * Devices in the second generation:
 *     82599
 *     X540
 **/
s32 ixgbe_start_hw_gen2(struct ixgbe_hw *hw)
{
	u32 i;

	/* Clear the rate limiters */
	for (i = 0; i < hw->mac.max_tx_queues; i++) {
		IXGBE_WRITE_REG(hw, IXGBE_RTTDQSEL, i);
		IXGBE_WRITE_REG(hw, IXGBE_RTTBCNRC, 0);
	}
	IXGBE_WRITE_FLUSH(hw);

	return 0;
}

/**
 *  ixgbe_init_hw_generic - Generic hardware initialization
 *  @hw: pointer to hardware structure
 *
 *  Initialize the hardware by resetting the hardware, filling the bus info
 *  structure and media type, clears all on chip counters, initializes receive
 *  address registers, multicast table, VLAN filter table, calls routine to set
 *  up link and flow control settings, and leaves transmit and receive units
 *  disabled and uninitialized
 **/
s32 ixgbe_init_hw_generic(struct ixgbe_hw *hw)
{
	s32 status;

	/* Reset the hardware */
	status = hw->mac.ops.reset_hw(hw);

	if (status == 0) {
		/* Start the HW */
		status = hw->mac.ops.start_hw(hw);
	}

	/* Initialize the LED link active for LED blink support */
	if (hw->mac.ops.init_led_link_act)
		hw->mac.ops.init_led_link_act(hw);

	return status;
}

/**
 *  ixgbe_clear_hw_cntrs_generic - Generic clear hardware counters
 *  @hw: pointer to hardware structure
 *
 *  Clears all hardware statistics counters by reading them from the hardware
 *  Statistics counters are clear on read.
 **/
s32 ixgbe_clear_hw_cntrs_generic(struct ixgbe_hw *hw)
{
	u16 i = 0;

	IXGBE_READ_REG(hw, IXGBE_CRCERRS);
	IXGBE_READ_REG(hw, IXGBE_ILLERRC);
	IXGBE_READ_REG(hw, IXGBE_ERRBC);
	IXGBE_READ_REG(hw, IXGBE_MSPDC);
	for (i = 0; i < 8; i++)
		IXGBE_READ_REG(hw, IXGBE_MPC(i));

	IXGBE_READ_REG(hw, IXGBE_MLFC);
	IXGBE_READ_REG(hw, IXGBE_MRFC);
	IXGBE_READ_REG(hw, IXGBE_RLEC);
	IXGBE_READ_REG(hw, IXGBE_LXONTXC);
	IXGBE_READ_REG(hw, IXGBE_LXOFFTXC);
	if (hw->mac.type >= ixgbe_mac_82599EB) {
		IXGBE_READ_REG(hw, IXGBE_LXONRXCNT);
		IXGBE_READ_REG(hw, IXGBE_LXOFFRXCNT);
	} else {
		IXGBE_READ_REG(hw, IXGBE_LXONRXC);
		IXGBE_READ_REG(hw, IXGBE_LXOFFRXC);
	}

	for (i = 0; i < 8; i++) {
		IXGBE_READ_REG(hw, IXGBE_PXONTXC(i));
		IXGBE_READ_REG(hw, IXGBE_PXOFFTXC(i));
		if (hw->mac.type >= ixgbe_mac_82599EB) {
			IXGBE_READ_REG(hw, IXGBE_PXONRXCNT(i));
			IXGBE_READ_REG(hw, IXGBE_PXOFFRXCNT(i));
		} else {
			IXGBE_READ_REG(hw, IXGBE_PXONRXC(i));
			IXGBE_READ_REG(hw, IXGBE_PXOFFRXC(i));
		}
	}
	if (hw->mac.type >= ixgbe_mac_82599EB)
		for (i = 0; i < 8; i++)
			IXGBE_READ_REG(hw, IXGBE_PXON2OFFCNT(i));
	IXGBE_READ_REG(hw, IXGBE_PRC64);
	IXGBE_READ_REG(hw, IXGBE_PRC127);
	IXGBE_READ_REG(hw, IXGBE_PRC255);
	IXGBE_READ_REG(hw, IXGBE_PRC511);
	IXGBE_READ_REG(hw, IXGBE_PRC1023);
	IXGBE_READ_REG(hw, IXGBE_PRC1522);
	IXGBE_READ_REG(hw, IXGBE_GPRC);
	IXGBE_READ_REG(hw, IXGBE_BPRC);
	IXGBE_READ_REG(hw, IXGBE_MPRC);
	IXGBE_READ_REG(hw, IXGBE_GPTC);
	IXGBE_READ_REG(hw, IXGBE_GORCL);
	IXGBE_READ_REG(hw, IXGBE_GORCH);
	IXGBE_READ_REG(hw, IXGBE_GOTCL);
	IXGBE_READ_REG(hw, IXGBE_GOTCH);
	if (hw->mac.type == ixgbe_mac_82598EB)
		for (i = 0; i < 8; i++)
			IXGBE_READ_REG(hw, IXGBE_RNBC(i));
	IXGBE_READ_REG(hw, IXGBE_RUC);
	IXGBE_READ_REG(hw, IXGBE_RFC);
	IXGBE_READ_REG(hw, IXGBE_ROC);
	IXGBE_READ_REG(hw, IXGBE_RJC);
	IXGBE_READ_REG(hw, IXGBE_MNGPRC);
	IXGBE_READ_REG(hw, IXGBE_MNGPDC);
	IXGBE_READ_REG(hw, IXGBE_MNGPTC);
	IXGBE_READ_REG(hw, IXGBE_TORL);
	IXGBE_READ_REG(hw, IXGBE_TORH);
	IXGBE_READ_REG(hw, IXGBE_TPR);
	IXGBE_READ_REG(hw, IXGBE_TPT);
	IXGBE_READ_REG(hw, IXGBE_PTC64);
	IXGBE_READ_REG(hw, IXGBE_PTC127);
	IXGBE_READ_REG(hw, IXGBE_PTC255);
	IXGBE_READ_REG(hw, IXGBE_PTC511);
	IXGBE_READ_REG(hw, IXGBE_PTC1023);
	IXGBE_READ_REG(hw, IXGBE_PTC1522);
	IXGBE_READ_REG(hw, IXGBE_MPTC);
	IXGBE_READ_REG(hw, IXGBE_BPTC);
	for (i = 0; i < 16; i++) {
		IXGBE_READ_REG(hw, IXGBE_QPRC(i));
		IXGBE_READ_REG(hw, IXGBE_QPTC(i));
		if (hw->mac.type >= ixgbe_mac_82599EB) {
			IXGBE_READ_REG(hw, IXGBE_QBRC_L(i));
			IXGBE_READ_REG(hw, IXGBE_QBRC_H(i));
			IXGBE_READ_REG(hw, IXGBE_QBTC_L(i));
			IXGBE_READ_REG(hw, IXGBE_QBTC_H(i));
			IXGBE_READ_REG(hw, IXGBE_QPRDC(i));
		} else {
			IXGBE_READ_REG(hw, IXGBE_QBRC(i));
			IXGBE_READ_REG(hw, IXGBE_QBTC(i));
		}
	}

	if (hw->mac.type == ixgbe_mac_X550 || hw->mac.type == ixgbe_mac_X540) {
		if (hw->phy.id == 0)
			hw->phy.ops.identify(hw);
		hw->phy.ops.read_reg(hw, IXGBE_PCRC8ECL, MDIO_MMD_PCS, &i);
		hw->phy.ops.read_reg(hw, IXGBE_PCRC8ECH, MDIO_MMD_PCS, &i);
		hw->phy.ops.read_reg(hw, IXGBE_LDPCECL, MDIO_MMD_PCS, &i);
		hw->phy.ops.read_reg(hw, IXGBE_LDPCECH, MDIO_MMD_PCS, &i);
	}

	return 0;
}

/**
 *  ixgbe_read_pba_string_generic - Reads part number string from EEPROM
 *  @hw: pointer to hardware structure
 *  @pba_num: stores the part number string from the EEPROM
 *  @pba_num_size: part number string buffer length
 *
 *  Reads the part number string from the EEPROM.
 **/
s32 ixgbe_read_pba_string_generic(struct ixgbe_hw *hw, u8 *pba_num,
				  u32 pba_num_size)
{
	s32 ret_val;
	u16 data;
	u16 pba_ptr;
	u16 offset;
	u16 length;

	if (pba_num == NULL) {
		hw_dbg(hw, "PBA string buffer was null\n");
		return IXGBE_ERR_INVALID_ARGUMENT;
	}

	ret_val = hw->eeprom.ops.read(hw, IXGBE_PBANUM0_PTR, &data);
	if (ret_val) {
		hw_dbg(hw, "NVM Read Error\n");
		return ret_val;
	}

	ret_val = hw->eeprom.ops.read(hw, IXGBE_PBANUM1_PTR, &pba_ptr);
	if (ret_val) {
		hw_dbg(hw, "NVM Read Error\n");
		return ret_val;
	}

	/*
	 * if data is not ptr guard the PBA must be in legacy format which
	 * means pba_ptr is actually our second data word for the PBA number
	 * and we can decode it into an ascii string
	 */
	if (data != IXGBE_PBANUM_PTR_GUARD) {
		hw_dbg(hw, "NVM PBA number is not stored as string\n");

		/* we will need 11 characters to store the PBA */
		if (pba_num_size < 11) {
			hw_dbg(hw, "PBA string buffer too small\n");
			return IXGBE_ERR_NO_SPACE;
		}

		/* extract hex string from data and pba_ptr */
		pba_num[0] = (data >> 12) & 0xF;
		pba_num[1] = (data >> 8) & 0xF;
		pba_num[2] = (data >> 4) & 0xF;
		pba_num[3] = data & 0xF;
		pba_num[4] = (pba_ptr >> 12) & 0xF;
		pba_num[5] = (pba_ptr >> 8) & 0xF;
		pba_num[6] = '-';
		pba_num[7] = 0;
		pba_num[8] = (pba_ptr >> 4) & 0xF;
		pba_num[9] = pba_ptr & 0xF;

		/* put a null character on the end of our string */
		pba_num[10] = '\0';

		/* switch all the data but the '-' to hex char */
		for (offset = 0; offset < 10; offset++) {
			if (pba_num[offset] < 0xA)
				pba_num[offset] += '0';
			else if (pba_num[offset] < 0x10)
				pba_num[offset] += 'A' - 0xA;
		}

		return 0;
	}

	ret_val = hw->eeprom.ops.read(hw, pba_ptr, &length);
	if (ret_val) {
		hw_dbg(hw, "NVM Read Error\n");
		return ret_val;
	}

	if (length == 0xFFFF || length == 0) {
		hw_dbg(hw, "NVM PBA number section invalid length\n");
		return IXGBE_ERR_PBA_SECTION;
	}

	/* check if pba_num buffer is big enough */
	if (pba_num_size  < (((u32)length * 2) - 1)) {
		hw_dbg(hw, "PBA string buffer too small\n");
		return IXGBE_ERR_NO_SPACE;
	}

	/* trim pba length from start of string */
	pba_ptr++;
	length--;

	for (offset = 0; offset < length; offset++) {
		ret_val = hw->eeprom.ops.read(hw, pba_ptr + offset, &data);
		if (ret_val) {
			hw_dbg(hw, "NVM Read Error\n");
			return ret_val;
		}
		pba_num[offset * 2] = (u8)(data >> 8);
		pba_num[(offset * 2) + 1] = (u8)(data & 0xFF);
	}
	pba_num[offset * 2] = '\0';

	return 0;
}

/**
 *  ixgbe_get_mac_addr_generic - Generic get MAC address
 *  @hw: pointer to hardware structure
 *  @mac_addr: Adapter MAC address
 *
 *  Reads the adapter's MAC address from first Receive Address Register (RAR0)
 *  A reset of the adapter must be performed prior to calling this function
 *  in order for the MAC address to have been loaded from the EEPROM into RAR0
 **/
s32 ixgbe_get_mac_addr_generic(struct ixgbe_hw *hw, u8 *mac_addr)
{
	u32 rar_high;
	u32 rar_low;
	u16 i;

	rar_high = IXGBE_READ_REG(hw, IXGBE_RAH(0));
	rar_low = IXGBE_READ_REG(hw, IXGBE_RAL(0));

	for (i = 0; i < 4; i++)
		mac_addr[i] = (u8)(rar_low >> (i*8));

	for (i = 0; i < 2; i++)
		mac_addr[i+4] = (u8)(rar_high >> (i*8));

	return 0;
}

enum ixgbe_bus_width ixgbe_convert_bus_width(u16 link_status)
{
	switch (link_status & IXGBE_PCI_LINK_WIDTH) {
	case IXGBE_PCI_LINK_WIDTH_1:
		return ixgbe_bus_width_pcie_x1;
	case IXGBE_PCI_LINK_WIDTH_2:
		return ixgbe_bus_width_pcie_x2;
	case IXGBE_PCI_LINK_WIDTH_4:
		return ixgbe_bus_width_pcie_x4;
	case IXGBE_PCI_LINK_WIDTH_8:
		return ixgbe_bus_width_pcie_x8;
	default:
		return ixgbe_bus_width_unknown;
	}
}

enum ixgbe_bus_speed ixgbe_convert_bus_speed(u16 link_status)
{
	switch (link_status & IXGBE_PCI_LINK_SPEED) {
	case IXGBE_PCI_LINK_SPEED_2500:
		return ixgbe_bus_speed_2500;
	case IXGBE_PCI_LINK_SPEED_5000:
		return ixgbe_bus_speed_5000;
	case IXGBE_PCI_LINK_SPEED_8000:
		return ixgbe_bus_speed_8000;
	default:
		return ixgbe_bus_speed_unknown;
	}
}

/**
 *  ixgbe_get_bus_info_generic - Generic set PCI bus info
 *  @hw: pointer to hardware structure
 *
 *  Sets the PCI bus info (speed, width, type) within the ixgbe_hw structure
 **/
s32 ixgbe_get_bus_info_generic(struct ixgbe_hw *hw)
{
	u16 link_status;

	hw->bus.type = ixgbe_bus_type_pci_express;

	/* Get the negotiated link width and speed from PCI config space */
	link_status = ixgbe_read_pci_cfg_word(hw, IXGBE_PCI_LINK_STATUS);

	hw->bus.width = ixgbe_convert_bus_width(link_status);
	hw->bus.speed = ixgbe_convert_bus_speed(link_status);

	hw->mac.ops.set_lan_id(hw);

	return 0;
}

/**
 *  ixgbe_set_lan_id_multi_port_pcie - Set LAN id for PCIe multiple port devices
 *  @hw: pointer to the HW structure
 *
 *  Determines the LAN function id by reading memory-mapped registers
 *  and swaps the port value if requested.
 **/
void ixgbe_set_lan_id_multi_port_pcie(struct ixgbe_hw *hw)
{
	struct ixgbe_bus_info *bus = &hw->bus;
	u16 ee_ctrl_4;
	u32 reg;

	reg = IXGBE_READ_REG(hw, IXGBE_STATUS);
	bus->func = (reg & IXGBE_STATUS_LAN_ID) >> IXGBE_STATUS_LAN_ID_SHIFT;
	bus->lan_id = bus->func;

	/* check for a port swap */
	reg = IXGBE_READ_REG(hw, IXGBE_FACTPS(hw));
	if (reg & IXGBE_FACTPS_LFS)
		bus->func ^= 0x1;

	/* Get MAC instance from EEPROM for configuring CS4227 */
	if (hw->device_id == IXGBE_DEV_ID_X550EM_A_SFP) {
		hw->eeprom.ops.read(hw, IXGBE_EEPROM_CTRL_4, &ee_ctrl_4);
		bus->instance_id = (ee_ctrl_4 & IXGBE_EE_CTRL_4_INST_ID) >>
				   IXGBE_EE_CTRL_4_INST_ID_SHIFT;
	}
}

/**
 *  ixgbe_stop_adapter_generic - Generic stop Tx/Rx units
 *  @hw: pointer to hardware structure
 *
 *  Sets the adapter_stopped flag within ixgbe_hw struct. Clears interrupts,
 *  disables transmit and receive units. The adapter_stopped flag is used by
 *  the shared code and drivers to determine if the adapter is in a stopped
 *  state and should not touch the hardware.
 **/
s32 ixgbe_stop_adapter_generic(struct ixgbe_hw *hw)
{
	u32 reg_val;
	u16 i;

	/*
	 * Set the adapter_stopped flag so other driver functions stop touching
	 * the hardware
	 */
	hw->adapter_stopped = true;

	/* Disable the receive unit */
	hw->mac.ops.disable_rx(hw);

	/* Clear interrupt mask to stop interrupts from being generated */
	IXGBE_WRITE_REG(hw, IXGBE_EIMC, IXGBE_IRQ_CLEAR_MASK);

	/* Clear any pending interrupts, flush previous writes */
	IXGBE_READ_REG(hw, IXGBE_EICR);

	/* Disable the transmit unit.  Each queue must be disabled. */
	for (i = 0; i < hw->mac.max_tx_queues; i++)
		IXGBE_WRITE_REG(hw, IXGBE_TXDCTL(i), IXGBE_TXDCTL_SWFLSH);

	/* Disable the receive unit by stopping each queue */
	for (i = 0; i < hw->mac.max_rx_queues; i++) {
		reg_val = IXGBE_READ_REG(hw, IXGBE_RXDCTL(i));
		reg_val &= ~IXGBE_RXDCTL_ENABLE;
		reg_val |= IXGBE_RXDCTL_SWFLSH;
		IXGBE_WRITE_REG(hw, IXGBE_RXDCTL(i), reg_val);
	}

	/* flush all queues disables */
	IXGBE_WRITE_FLUSH(hw);
	usleep_range(1000, 2000);

	/*
	 * Prevent the PCI-E bus from from hanging by disabling PCI-E master
	 * access and verify no pending requests
	 */
	return ixgbe_disable_pcie_master(hw);
}

/**
 *  ixgbe_init_led_link_act_generic - Store the LED index link/activity.
 *  @hw: pointer to hardware structure
 *
 *  Store the index for the link active LED. This will be used to support
 *  blinking the LED.
 **/
s32 ixgbe_init_led_link_act_generic(struct ixgbe_hw *hw)
{
	struct ixgbe_mac_info *mac = &hw->mac;
	u32 led_reg, led_mode;
	u16 i;

	led_reg = IXGBE_READ_REG(hw, IXGBE_LEDCTL);

	/* Get LED link active from the LEDCTL register */
	for (i = 0; i < 4; i++) {
		led_mode = led_reg >> IXGBE_LED_MODE_SHIFT(i);

		if ((led_mode & IXGBE_LED_MODE_MASK_BASE) ==
		    IXGBE_LED_LINK_ACTIVE) {
			mac->led_link_act = i;
			return 0;
		}
	}

	/* If LEDCTL register does not have the LED link active set, then use
	 * known MAC defaults.
	 */
	switch (hw->mac.type) {
	case ixgbe_mac_x550em_a:
		mac->led_link_act = 0;
		break;
	case ixgbe_mac_X550EM_x:
		mac->led_link_act = 1;
		break;
	default:
		mac->led_link_act = 2;
	}

	return 0;
}

/**
 *  ixgbe_led_on_generic - Turns on the software controllable LEDs.
 *  @hw: pointer to hardware structure
 *  @index: led number to turn on
 **/
s32 ixgbe_led_on_generic(struct ixgbe_hw *hw, u32 index)
{
	u32 led_reg = IXGBE_READ_REG(hw, IXGBE_LEDCTL);

	if (index > 3)
		return IXGBE_ERR_PARAM;

	/* To turn on the LED, set mode to ON. */
	led_reg &= ~IXGBE_LED_MODE_MASK(index);
	led_reg |= IXGBE_LED_ON << IXGBE_LED_MODE_SHIFT(index);
	IXGBE_WRITE_REG(hw, IXGBE_LEDCTL, led_reg);
	IXGBE_WRITE_FLUSH(hw);

	return 0;
}

/**
 *  ixgbe_led_off_generic - Turns off the software controllable LEDs.
 *  @hw: pointer to hardware structure
 *  @index: led number to turn off
 **/
s32 ixgbe_led_off_generic(struct ixgbe_hw *hw, u32 index)
{
	u32 led_reg = IXGBE_READ_REG(hw, IXGBE_LEDCTL);

	if (index > 3)
		return IXGBE_ERR_PARAM;

	/* To turn off the LED, set mode to OFF. */
	led_reg &= ~IXGBE_LED_MODE_MASK(index);
	led_reg |= IXGBE_LED_OFF << IXGBE_LED_MODE_SHIFT(index);
	IXGBE_WRITE_REG(hw, IXGBE_LEDCTL, led_reg);
	IXGBE_WRITE_FLUSH(hw);

	return 0;
}

/**
 *  ixgbe_init_eeprom_params_generic - Initialize EEPROM params
 *  @hw: pointer to hardware structure
 *
 *  Initializes the EEPROM parameters ixgbe_eeprom_info within the
 *  ixgbe_hw struct in order to set up EEPROM access.
 **/
s32 ixgbe_init_eeprom_params_generic(struct ixgbe_hw *hw)
{
	struct ixgbe_eeprom_info *eeprom = &hw->eeprom;
	u32 eec;
	u16 eeprom_size;

	if (eeprom->type == ixgbe_eeprom_uninitialized) {
		eeprom->type = ixgbe_eeprom_none;
		/* Set default semaphore delay to 10ms which is a well
		 * tested value */
		eeprom->semaphore_delay = 10;
		/* Clear EEPROM page size, it will be initialized as needed */
		eeprom->word_page_size = 0;

		/*
		 * Check for EEPROM present first.
		 * If not present leave as none
		 */
		eec = IXGBE_READ_REG(hw, IXGBE_EEC(hw));
		if (eec & IXGBE_EEC_PRES) {
			eeprom->type = ixgbe_eeprom_spi;

			/*
			 * SPI EEPROM is assumed here.  This code would need to
			 * change if a future EEPROM is not SPI.
			 */
			eeprom_size = (u16)((eec & IXGBE_EEC_SIZE) >>
					    IXGBE_EEC_SIZE_SHIFT);
			eeprom->word_size = BIT(eeprom_size +
						 IXGBE_EEPROM_WORD_SIZE_SHIFT);
		}

		if (eec & IXGBE_EEC_ADDR_SIZE)
			eeprom->address_bits = 16;
		else
			eeprom->address_bits = 8;
		hw_dbg(hw, "Eeprom params: type = %d, size = %d, address bits: %d\n",
		       eeprom->type, eeprom->word_size, eeprom->address_bits);
	}

	return 0;
}

/**
 *  ixgbe_write_eeprom_buffer_bit_bang_generic - Write EEPROM using bit-bang
 *  @hw: pointer to hardware structure
 *  @offset: offset within the EEPROM to write
 *  @words: number of words
 *  @data: 16 bit word(s) to write to EEPROM
 *
 *  Reads 16 bit word(s) from EEPROM through bit-bang method
 **/
s32 ixgbe_write_eeprom_buffer_bit_bang_generic(struct ixgbe_hw *hw, u16 offset,
					       u16 words, u16 *data)
{
	s32 status;
	u16 i, count;

	hw->eeprom.ops.init_params(hw);

	if (words == 0)
		return IXGBE_ERR_INVALID_ARGUMENT;

	if (offset + words > hw->eeprom.word_size)
		return IXGBE_ERR_EEPROM;

	/*
	 * The EEPROM page size cannot be queried from the chip. We do lazy
	 * initialization. It is worth to do that when we write large buffer.
	 */
	if ((hw->eeprom.word_page_size == 0) &&
	    (words > IXGBE_EEPROM_PAGE_SIZE_MAX))
		ixgbe_detect_eeprom_page_size_generic(hw, offset);

	/*
	 * We cannot hold synchronization semaphores for too long
	 * to avoid other entity starvation. However it is more efficient
	 * to read in bursts than synchronizing access for each word.
	 */
	for (i = 0; i < words; i += IXGBE_EEPROM_RD_BUFFER_MAX_COUNT) {
		count = (words - i) / IXGBE_EEPROM_RD_BUFFER_MAX_COUNT > 0 ?
			 IXGBE_EEPROM_RD_BUFFER_MAX_COUNT : (words - i);
		status = ixgbe_write_eeprom_buffer_bit_bang(hw, offset + i,
							    count, &data[i]);

		if (status != 0)
			break;
	}

	return status;
}

/**
 *  ixgbe_write_eeprom_buffer_bit_bang - Writes 16 bit word(s) to EEPROM
 *  @hw: pointer to hardware structure
 *  @offset: offset within the EEPROM to be written to
 *  @words: number of word(s)
 *  @data: 16 bit word(s) to be written to the EEPROM
 *
 *  If ixgbe_eeprom_update_checksum is not called after this function, the
 *  EEPROM will most likely contain an invalid checksum.
 **/
static s32 ixgbe_write_eeprom_buffer_bit_bang(struct ixgbe_hw *hw, u16 offset,
					      u16 words, u16 *data)
{
	s32 status;
	u16 word;
	u16 page_size;
	u16 i;
	u8 write_opcode = IXGBE_EEPROM_WRITE_OPCODE_SPI;

	/* Prepare the EEPROM for writing  */
	status = ixgbe_acquire_eeprom(hw);
	if (status)
		return status;

	if (ixgbe_ready_eeprom(hw) != 0) {
		ixgbe_release_eeprom(hw);
		return IXGBE_ERR_EEPROM;
	}

	for (i = 0; i < words; i++) {
		ixgbe_standby_eeprom(hw);

		/* Send the WRITE ENABLE command (8 bit opcode) */
		ixgbe_shift_out_eeprom_bits(hw,
					    IXGBE_EEPROM_WREN_OPCODE_SPI,
					    IXGBE_EEPROM_OPCODE_BITS);

		ixgbe_standby_eeprom(hw);

		/* Some SPI eeproms use the 8th address bit embedded
		 * in the opcode
		 */
		if ((hw->eeprom.address_bits == 8) &&
		    ((offset + i) >= 128))
			write_opcode |= IXGBE_EEPROM_A8_OPCODE_SPI;

		/* Send the Write command (8-bit opcode + addr) */
		ixgbe_shift_out_eeprom_bits(hw, write_opcode,
					    IXGBE_EEPROM_OPCODE_BITS);
		ixgbe_shift_out_eeprom_bits(hw, (u16)((offset + i) * 2),
					    hw->eeprom.address_bits);

		page_size = hw->eeprom.word_page_size;

		/* Send the data in burst via SPI */
		do {
			word = data[i];
			word = (word >> 8) | (word << 8);
			ixgbe_shift_out_eeprom_bits(hw, word, 16);

			if (page_size == 0)
				break;

			/* do not wrap around page */
			if (((offset + i) & (page_size - 1)) ==
			    (page_size - 1))
				break;
		} while (++i < words);

		ixgbe_standby_eeprom(hw);
		usleep_range(10000, 20000);
	}
	/* Done with writing - release the EEPROM */
	ixgbe_release_eeprom(hw);

	return 0;
}

/**
 *  ixgbe_write_eeprom_generic - Writes 16 bit value to EEPROM
 *  @hw: pointer to hardware structure
 *  @offset: offset within the EEPROM to be written to
 *  @data: 16 bit word to be written to the EEPROM
 *
 *  If ixgbe_eeprom_update_checksum is not called after this function, the
 *  EEPROM will most likely contain an invalid checksum.
 **/
s32 ixgbe_write_eeprom_generic(struct ixgbe_hw *hw, u16 offset, u16 data)
{
	hw->eeprom.ops.init_params(hw);

	if (offset >= hw->eeprom.word_size)
		return IXGBE_ERR_EEPROM;

	return ixgbe_write_eeprom_buffer_bit_bang(hw, offset, 1, &data);
}

/**
 *  ixgbe_read_eeprom_buffer_bit_bang_generic - Read EEPROM using bit-bang
 *  @hw: pointer to hardware structure
 *  @offset: offset within the EEPROM to be read
 *  @words: number of word(s)
 *  @data: read 16 bit words(s) from EEPROM
 *
 *  Reads 16 bit word(s) from EEPROM through bit-bang method
 **/
s32 ixgbe_read_eeprom_buffer_bit_bang_generic(struct ixgbe_hw *hw, u16 offset,
					      u16 words, u16 *data)
{
	s32 status;
	u16 i, count;

	hw->eeprom.ops.init_params(hw);

	if (words == 0)
		return IXGBE_ERR_INVALID_ARGUMENT;

	if (offset + words > hw->eeprom.word_size)
		return IXGBE_ERR_EEPROM;

	/*
	 * We cannot hold synchronization semaphores for too long
	 * to avoid other entity starvation. However it is more efficient
	 * to read in bursts than synchronizing access for each word.
	 */
	for (i = 0; i < words; i += IXGBE_EEPROM_RD_BUFFER_MAX_COUNT) {
		count = (words - i) / IXGBE_EEPROM_RD_BUFFER_MAX_COUNT > 0 ?
			 IXGBE_EEPROM_RD_BUFFER_MAX_COUNT : (words - i);

		status = ixgbe_read_eeprom_buffer_bit_bang(hw, offset + i,
							   count, &data[i]);

		if (status)
			return status;
	}

	return 0;
}

/**
 *  ixgbe_read_eeprom_buffer_bit_bang - Read EEPROM using bit-bang
 *  @hw: pointer to hardware structure
 *  @offset: offset within the EEPROM to be read
 *  @words: number of word(s)
 *  @data: read 16 bit word(s) from EEPROM
 *
 *  Reads 16 bit word(s) from EEPROM through bit-bang method
 **/
static s32 ixgbe_read_eeprom_buffer_bit_bang(struct ixgbe_hw *hw, u16 offset,
					     u16 words, u16 *data)
{
	s32 status;
	u16 word_in;
	u8 read_opcode = IXGBE_EEPROM_READ_OPCODE_SPI;
	u16 i;

	/* Prepare the EEPROM for reading  */
	status = ixgbe_acquire_eeprom(hw);
	if (status)
		return status;

	if (ixgbe_ready_eeprom(hw) != 0) {
		ixgbe_release_eeprom(hw);
		return IXGBE_ERR_EEPROM;
	}

	for (i = 0; i < words; i++) {
		ixgbe_standby_eeprom(hw);
		/* Some SPI eeproms use the 8th address bit embedded
		 * in the opcode
		 */
		if ((hw->eeprom.address_bits == 8) &&
		    ((offset + i) >= 128))
			read_opcode |= IXGBE_EEPROM_A8_OPCODE_SPI;

		/* Send the READ command (opcode + addr) */
		ixgbe_shift_out_eeprom_bits(hw, read_opcode,
					    IXGBE_EEPROM_OPCODE_BITS);
		ixgbe_shift_out_eeprom_bits(hw, (u16)((offset + i) * 2),
					    hw->eeprom.address_bits);

		/* Read the data. */
		word_in = ixgbe_shift_in_eeprom_bits(hw, 16);
		data[i] = (word_in >> 8) | (word_in << 8);
	}

	/* End this read operation */
	ixgbe_release_eeprom(hw);

	return 0;
}

/**
 *  ixgbe_read_eeprom_bit_bang_generic - Read EEPROM word using bit-bang
 *  @hw: pointer to hardware structure
 *  @offset: offset within the EEPROM to be read
 *  @data: read 16 bit value from EEPROM
 *
 *  Reads 16 bit value from EEPROM through bit-bang method
 **/
s32 ixgbe_read_eeprom_bit_bang_generic(struct ixgbe_hw *hw, u16 offset,
				       u16 *data)
{
	hw->eeprom.ops.init_params(hw);

	if (offset >= hw->eeprom.word_size)
		return IXGBE_ERR_EEPROM;

	return ixgbe_read_eeprom_buffer_bit_bang(hw, offset, 1, data);
}

/**
 *  ixgbe_read_eerd_buffer_generic - Read EEPROM word(s) using EERD
 *  @hw: pointer to hardware structure
 *  @offset: offset of word in the EEPROM to read
 *  @words: number of word(s)
 *  @data: 16 bit word(s) from the EEPROM
 *
 *  Reads a 16 bit word(s) from the EEPROM using the EERD register.
 **/
s32 ixgbe_read_eerd_buffer_generic(struct ixgbe_hw *hw, u16 offset,
				   u16 words, u16 *data)
{
	u32 eerd;
	s32 status;
	u32 i;

	hw->eeprom.ops.init_params(hw);

	if (words == 0)
		return IXGBE_ERR_INVALID_ARGUMENT;

	if (offset >= hw->eeprom.word_size)
		return IXGBE_ERR_EEPROM;

	for (i = 0; i < words; i++) {
		eerd = ((offset + i) << IXGBE_EEPROM_RW_ADDR_SHIFT) |
		       IXGBE_EEPROM_RW_REG_START;

		IXGBE_WRITE_REG(hw, IXGBE_EERD, eerd);
		status = ixgbe_poll_eerd_eewr_done(hw, IXGBE_NVM_POLL_READ);

		if (status == 0) {
			data[i] = (IXGBE_READ_REG(hw, IXGBE_EERD) >>
				   IXGBE_EEPROM_RW_REG_DATA);
		} else {
			hw_dbg(hw, "Eeprom read timed out\n");
			return status;
		}
	}

	return 0;
}

/**
 *  ixgbe_detect_eeprom_page_size_generic - Detect EEPROM page size
 *  @hw: pointer to hardware structure
 *  @offset: offset within the EEPROM to be used as a scratch pad
 *
 *  Discover EEPROM page size by writing marching data at given offset.
 *  This function is called only when we are writing a new large buffer
 *  at given offset so the data would be overwritten anyway.
 **/
static s32 ixgbe_detect_eeprom_page_size_generic(struct ixgbe_hw *hw,
						 u16 offset)
{
	u16 data[IXGBE_EEPROM_PAGE_SIZE_MAX];
	s32 status;
	u16 i;

	for (i = 0; i < IXGBE_EEPROM_PAGE_SIZE_MAX; i++)
		data[i] = i;

	hw->eeprom.word_page_size = IXGBE_EEPROM_PAGE_SIZE_MAX;
	status = ixgbe_write_eeprom_buffer_bit_bang(hw, offset,
					     IXGBE_EEPROM_PAGE_SIZE_MAX, data);
	hw->eeprom.word_page_size = 0;
	if (status)
		return status;

	status = ixgbe_read_eeprom_buffer_bit_bang(hw, offset, 1, data);
	if (status)
		return status;

	/*
	 * When writing in burst more than the actual page size
	 * EEPROM address wraps around current page.
	 */
	hw->eeprom.word_page_size = IXGBE_EEPROM_PAGE_SIZE_MAX - data[0];

	hw_dbg(hw, "Detected EEPROM page size = %d words.\n",
	       hw->eeprom.word_page_size);
	return 0;
}

/**
 *  ixgbe_read_eerd_generic - Read EEPROM word using EERD
 *  @hw: pointer to hardware structure
 *  @offset: offset of  word in the EEPROM to read
 *  @data: word read from the EEPROM
 *
 *  Reads a 16 bit word from the EEPROM using the EERD register.
 **/
s32 ixgbe_read_eerd_generic(struct ixgbe_hw *hw, u16 offset, u16 *data)
{
	return ixgbe_read_eerd_buffer_generic(hw, offset, 1, data);
}

/**
 *  ixgbe_write_eewr_buffer_generic - Write EEPROM word(s) using EEWR
 *  @hw: pointer to hardware structure
 *  @offset: offset of  word in the EEPROM to write
 *  @words: number of words
 *  @data: word(s) write to the EEPROM
 *
 *  Write a 16 bit word(s) to the EEPROM using the EEWR register.
 **/
s32 ixgbe_write_eewr_buffer_generic(struct ixgbe_hw *hw, u16 offset,
				    u16 words, u16 *data)
{
	u32 eewr;
	s32 status;
	u16 i;

	hw->eeprom.ops.init_params(hw);

	if (words == 0)
		return IXGBE_ERR_INVALID_ARGUMENT;

	if (offset >= hw->eeprom.word_size)
		return IXGBE_ERR_EEPROM;

	for (i = 0; i < words; i++) {
		eewr = ((offset + i) << IXGBE_EEPROM_RW_ADDR_SHIFT) |
		       (data[i] << IXGBE_EEPROM_RW_REG_DATA) |
		       IXGBE_EEPROM_RW_REG_START;

		status = ixgbe_poll_eerd_eewr_done(hw, IXGBE_NVM_POLL_WRITE);
		if (status) {
			hw_dbg(hw, "Eeprom write EEWR timed out\n");
			return status;
		}

		IXGBE_WRITE_REG(hw, IXGBE_EEWR, eewr);

		status = ixgbe_poll_eerd_eewr_done(hw, IXGBE_NVM_POLL_WRITE);
		if (status) {
			hw_dbg(hw, "Eeprom write EEWR timed out\n");
			return status;
		}
	}

	return 0;
}

/**
 *  ixgbe_write_eewr_generic - Write EEPROM word using EEWR
 *  @hw: pointer to hardware structure
 *  @offset: offset of  word in the EEPROM to write
 *  @data: word write to the EEPROM
 *
 *  Write a 16 bit word to the EEPROM using the EEWR register.
 **/
s32 ixgbe_write_eewr_generic(struct ixgbe_hw *hw, u16 offset, u16 data)
{
	return ixgbe_write_eewr_buffer_generic(hw, offset, 1, &data);
}

/**
 *  ixgbe_poll_eerd_eewr_done - Poll EERD read or EEWR write status
 *  @hw: pointer to hardware structure
 *  @ee_reg: EEPROM flag for polling
 *
 *  Polls the status bit (bit 1) of the EERD or EEWR to determine when the
 *  read or write is done respectively.
 **/
static s32 ixgbe_poll_eerd_eewr_done(struct ixgbe_hw *hw, u32 ee_reg)
{
	u32 i;
	u32 reg;

	for (i = 0; i < IXGBE_EERD_EEWR_ATTEMPTS; i++) {
		if (ee_reg == IXGBE_NVM_POLL_READ)
			reg = IXGBE_READ_REG(hw, IXGBE_EERD);
		else
			reg = IXGBE_READ_REG(hw, IXGBE_EEWR);

		if (reg & IXGBE_EEPROM_RW_REG_DONE) {
			return 0;
		}
		udelay(5);
	}
	return IXGBE_ERR_EEPROM;
}

/**
 *  ixgbe_acquire_eeprom - Acquire EEPROM using bit-bang
 *  @hw: pointer to hardware structure
 *
 *  Prepares EEPROM for access using bit-bang method. This function should
 *  be called before issuing a command to the EEPROM.
 **/
static s32 ixgbe_acquire_eeprom(struct ixgbe_hw *hw)
{
	u32 eec;
	u32 i;

	if (hw->mac.ops.acquire_swfw_sync(hw, IXGBE_GSSR_EEP_SM) != 0)
		return IXGBE_ERR_SWFW_SYNC;

	eec = IXGBE_READ_REG(hw, IXGBE_EEC(hw));

	/* Request EEPROM Access */
	eec |= IXGBE_EEC_REQ;
	IXGBE_WRITE_REG(hw, IXGBE_EEC(hw), eec);

	for (i = 0; i < IXGBE_EEPROM_GRANT_ATTEMPTS; i++) {
		eec = IXGBE_READ_REG(hw, IXGBE_EEC(hw));
		if (eec & IXGBE_EEC_GNT)
			break;
		udelay(5);
	}

	/* Release if grant not acquired */
	if (!(eec & IXGBE_EEC_GNT)) {
		eec &= ~IXGBE_EEC_REQ;
		IXGBE_WRITE_REG(hw, IXGBE_EEC(hw), eec);
		hw_dbg(hw, "Could not acquire EEPROM grant\n");

		hw->mac.ops.release_swfw_sync(hw, IXGBE_GSSR_EEP_SM);
		return IXGBE_ERR_EEPROM;
	}

	/* Setup EEPROM for Read/Write */
	/* Clear CS and SK */
	eec &= ~(IXGBE_EEC_CS | IXGBE_EEC_SK);
	IXGBE_WRITE_REG(hw, IXGBE_EEC(hw), eec);
	IXGBE_WRITE_FLUSH(hw);
	udelay(1);
	return 0;
}

/**
 *  ixgbe_get_eeprom_semaphore - Get hardware semaphore
 *  @hw: pointer to hardware structure
 *
 *  Sets the hardware semaphores so EEPROM access can occur for bit-bang method
 **/
static s32 ixgbe_get_eeprom_semaphore(struct ixgbe_hw *hw)
{
	u32 timeout = 2000;
	u32 i;
	u32 swsm;

	/* Get SMBI software semaphore between device drivers first */
	for (i = 0; i < timeout; i++) {
		/*
		 * If the SMBI bit is 0 when we read it, then the bit will be
		 * set and we have the semaphore
		 */
		swsm = IXGBE_READ_REG(hw, IXGBE_SWSM(hw));
		if (!(swsm & IXGBE_SWSM_SMBI))
			break;
		usleep_range(50, 100);
	}

	if (i == timeout) {
		hw_dbg(hw, "Driver can't access the Eeprom - SMBI Semaphore not granted.\n");
		/* this release is particularly important because our attempts
		 * above to get the semaphore may have succeeded, and if there
		 * was a timeout, we should unconditionally clear the semaphore
		 * bits to free the driver to make progress
		 */
		ixgbe_release_eeprom_semaphore(hw);

		usleep_range(50, 100);
		/* one last try
		 * If the SMBI bit is 0 when we read it, then the bit will be
		 * set and we have the semaphore
		 */
		swsm = IXGBE_READ_REG(hw, IXGBE_SWSM(hw));
		if (swsm & IXGBE_SWSM_SMBI) {
			hw_dbg(hw, "Software semaphore SMBI between device drivers not granted.\n");
			return IXGBE_ERR_EEPROM;
		}
	}

	/* Now get the semaphore between SW/FW through the SWESMBI bit */
	for (i = 0; i < timeout; i++) {
		swsm = IXGBE_READ_REG(hw, IXGBE_SWSM(hw));

		/* Set the SW EEPROM semaphore bit to request access */
		swsm |= IXGBE_SWSM_SWESMBI;
		IXGBE_WRITE_REG(hw, IXGBE_SWSM(hw), swsm);

		/* If we set the bit successfully then we got the
		 * semaphore.
		 */
		swsm = IXGBE_READ_REG(hw, IXGBE_SWSM(hw));
		if (swsm & IXGBE_SWSM_SWESMBI)
			break;

		usleep_range(50, 100);
	}

	/* Release semaphores and return error if SW EEPROM semaphore
	 * was not granted because we don't have access to the EEPROM
	 */
	if (i >= timeout) {
		hw_dbg(hw, "SWESMBI Software EEPROM semaphore not granted.\n");
		ixgbe_release_eeprom_semaphore(hw);
		return IXGBE_ERR_EEPROM;
	}

	return 0;
}

/**
 *  ixgbe_release_eeprom_semaphore - Release hardware semaphore
 *  @hw: pointer to hardware structure
 *
 *  This function clears hardware semaphore bits.
 **/
static void ixgbe_release_eeprom_semaphore(struct ixgbe_hw *hw)
{
	u32 swsm;

	swsm = IXGBE_READ_REG(hw, IXGBE_SWSM(hw));

	/* Release both semaphores by writing 0 to the bits SWESMBI and SMBI */
	swsm &= ~(IXGBE_SWSM_SWESMBI | IXGBE_SWSM_SMBI);
	IXGBE_WRITE_REG(hw, IXGBE_SWSM(hw), swsm);
	IXGBE_WRITE_FLUSH(hw);
}

/**
 *  ixgbe_ready_eeprom - Polls for EEPROM ready
 *  @hw: pointer to hardware structure
 **/
static s32 ixgbe_ready_eeprom(struct ixgbe_hw *hw)
{
	u16 i;
	u8 spi_stat_reg;

	/*
	 * Read "Status Register" repeatedly until the LSB is cleared.  The
	 * EEPROM will signal that the command has been completed by clearing
	 * bit 0 of the internal status register.  If it's not cleared within
	 * 5 milliseconds, then error out.
	 */
	for (i = 0; i < IXGBE_EEPROM_MAX_RETRY_SPI; i += 5) {
		ixgbe_shift_out_eeprom_bits(hw, IXGBE_EEPROM_RDSR_OPCODE_SPI,
					    IXGBE_EEPROM_OPCODE_BITS);
		spi_stat_reg = (u8)ixgbe_shift_in_eeprom_bits(hw, 8);
		if (!(spi_stat_reg & IXGBE_EEPROM_STATUS_RDY_SPI))
			break;

		udelay(5);
		ixgbe_standby_eeprom(hw);
	}

	/*
	 * On some parts, SPI write time could vary from 0-20mSec on 3.3V
	 * devices (and only 0-5mSec on 5V devices)
	 */
	if (i >= IXGBE_EEPROM_MAX_RETRY_SPI) {
		hw_dbg(hw, "SPI EEPROM Status error\n");
		return IXGBE_ERR_EEPROM;
	}

	return 0;
}

/**
 *  ixgbe_standby_eeprom - Returns EEPROM to a "standby" state
 *  @hw: pointer to hardware structure
 **/
static void ixgbe_standby_eeprom(struct ixgbe_hw *hw)
{
	u32 eec;

	eec = IXGBE_READ_REG(hw, IXGBE_EEC(hw));

	/* Toggle CS to flush commands */
	eec |= IXGBE_EEC_CS;
	IXGBE_WRITE_REG(hw, IXGBE_EEC(hw), eec);
	IXGBE_WRITE_FLUSH(hw);
	udelay(1);
	eec &= ~IXGBE_EEC_CS;
	IXGBE_WRITE_REG(hw, IXGBE_EEC(hw), eec);
	IXGBE_WRITE_FLUSH(hw);
	udelay(1);
}

/**
 *  ixgbe_shift_out_eeprom_bits - Shift data bits out to the EEPROM.
 *  @hw: pointer to hardware structure
 *  @data: data to send to the EEPROM
 *  @count: number of bits to shift out
 **/
static void ixgbe_shift_out_eeprom_bits(struct ixgbe_hw *hw, u16 data,
					u16 count)
{
	u32 eec;
	u32 mask;
	u32 i;

	eec = IXGBE_READ_REG(hw, IXGBE_EEC(hw));

	/*
	 * Mask is used to shift "count" bits of "data" out to the EEPROM
	 * one bit at a time.  Determine the starting bit based on count
	 */
	mask = BIT(count - 1);

	for (i = 0; i < count; i++) {
		/*
		 * A "1" is shifted out to the EEPROM by setting bit "DI" to a
		 * "1", and then raising and then lowering the clock (the SK
		 * bit controls the clock input to the EEPROM).  A "0" is
		 * shifted out to the EEPROM by setting "DI" to "0" and then
		 * raising and then lowering the clock.
		 */
		if (data & mask)
			eec |= IXGBE_EEC_DI;
		else
			eec &= ~IXGBE_EEC_DI;

		IXGBE_WRITE_REG(hw, IXGBE_EEC(hw), eec);
		IXGBE_WRITE_FLUSH(hw);

		udelay(1);

		ixgbe_raise_eeprom_clk(hw, &eec);
		ixgbe_lower_eeprom_clk(hw, &eec);

		/*
		 * Shift mask to signify next bit of data to shift in to the
		 * EEPROM
		 */
		mask = mask >> 1;
	}

	/* We leave the "DI" bit set to "0" when we leave this routine. */
	eec &= ~IXGBE_EEC_DI;
	IXGBE_WRITE_REG(hw, IXGBE_EEC(hw), eec);
	IXGBE_WRITE_FLUSH(hw);
}

/**
 *  ixgbe_shift_in_eeprom_bits - Shift data bits in from the EEPROM
 *  @hw: pointer to hardware structure
 *  @count: number of bits to shift
 **/
static u16 ixgbe_shift_in_eeprom_bits(struct ixgbe_hw *hw, u16 count)
{
	u32 eec;
	u32 i;
	u16 data = 0;

	/*
	 * In order to read a register from the EEPROM, we need to shift
	 * 'count' bits in from the EEPROM. Bits are "shifted in" by raising
	 * the clock input to the EEPROM (setting the SK bit), and then reading
	 * the value of the "DO" bit.  During this "shifting in" process the
	 * "DI" bit should always be clear.
	 */
	eec = IXGBE_READ_REG(hw, IXGBE_EEC(hw));

	eec &= ~(IXGBE_EEC_DO | IXGBE_EEC_DI);

	for (i = 0; i < count; i++) {
		data = data << 1;
		ixgbe_raise_eeprom_clk(hw, &eec);

		eec = IXGBE_READ_REG(hw, IXGBE_EEC(hw));

		eec &= ~(IXGBE_EEC_DI);
		if (eec & IXGBE_EEC_DO)
			data |= 1;

		ixgbe_lower_eeprom_clk(hw, &eec);
	}

	return data;
}

/**
 *  ixgbe_raise_eeprom_clk - Raises the EEPROM's clock input.
 *  @hw: pointer to hardware structure
 *  @eec: EEC register's current value
 **/
static void ixgbe_raise_eeprom_clk(struct ixgbe_hw *hw, u32 *eec)
{
	/*
	 * Raise the clock input to the EEPROM
	 * (setting the SK bit), then delay
	 */
	*eec = *eec | IXGBE_EEC_SK;
	IXGBE_WRITE_REG(hw, IXGBE_EEC(hw), *eec);
	IXGBE_WRITE_FLUSH(hw);
	udelay(1);
}

/**
 *  ixgbe_lower_eeprom_clk - Lowers the EEPROM's clock input.
 *  @hw: pointer to hardware structure
 *  @eec: EEC's current value
 **/
static void ixgbe_lower_eeprom_clk(struct ixgbe_hw *hw, u32 *eec)
{
	/*
	 * Lower the clock input to the EEPROM (clearing the SK bit), then
	 * delay
	 */
	*eec = *eec & ~IXGBE_EEC_SK;
	IXGBE_WRITE_REG(hw, IXGBE_EEC(hw), *eec);
	IXGBE_WRITE_FLUSH(hw);
	udelay(1);
}

/**
 *  ixgbe_release_eeprom - Release EEPROM, release semaphores
 *  @hw: pointer to hardware structure
 **/
static void ixgbe_release_eeprom(struct ixgbe_hw *hw)
{
	u32 eec;

	eec = IXGBE_READ_REG(hw, IXGBE_EEC(hw));

	eec |= IXGBE_EEC_CS;  /* Pull CS high */
	eec &= ~IXGBE_EEC_SK; /* Lower SCK */

	IXGBE_WRITE_REG(hw, IXGBE_EEC(hw), eec);
	IXGBE_WRITE_FLUSH(hw);

	udelay(1);

	/* Stop requesting EEPROM access */
	eec &= ~IXGBE_EEC_REQ;
	IXGBE_WRITE_REG(hw, IXGBE_EEC(hw), eec);

	hw->mac.ops.release_swfw_sync(hw, IXGBE_GSSR_EEP_SM);

	/*
	 * Delay before attempt to obtain semaphore again to allow FW
	 * access. semaphore_delay is in ms we need us for usleep_range
	 */
	usleep_range(hw->eeprom.semaphore_delay * 1000,
		     hw->eeprom.semaphore_delay * 2000);
}

/**
 *  ixgbe_calc_eeprom_checksum_generic - Calculates and returns the checksum
 *  @hw: pointer to hardware structure
 **/
s32 ixgbe_calc_eeprom_checksum_generic(struct ixgbe_hw *hw)
{
	u16 i;
	u16 j;
	u16 checksum = 0;
	u16 length = 0;
	u16 pointer = 0;
	u16 word = 0;

	/* Include 0x0-0x3F in the checksum */
	for (i = 0; i < IXGBE_EEPROM_CHECKSUM; i++) {
		if (hw->eeprom.ops.read(hw, i, &word)) {
			hw_dbg(hw, "EEPROM read failed\n");
			break;
		}
		checksum += word;
	}

	/* Include all data from pointers except for the fw pointer */
	for (i = IXGBE_PCIE_ANALOG_PTR; i < IXGBE_FW_PTR; i++) {
		if (hw->eeprom.ops.read(hw, i, &pointer)) {
			hw_dbg(hw, "EEPROM read failed\n");
			return IXGBE_ERR_EEPROM;
		}

		/* If the pointer seems invalid */
		if (pointer == 0xFFFF || pointer == 0)
			continue;

		if (hw->eeprom.ops.read(hw, pointer, &length)) {
			hw_dbg(hw, "EEPROM read failed\n");
			return IXGBE_ERR_EEPROM;
		}

		if (length == 0xFFFF || length == 0)
			continue;

		for (j = pointer + 1; j <= pointer + length; j++) {
			if (hw->eeprom.ops.read(hw, j, &word)) {
				hw_dbg(hw, "EEPROM read failed\n");
				return IXGBE_ERR_EEPROM;
			}
			checksum += word;
		}
	}

	checksum = (u16)IXGBE_EEPROM_SUM - checksum;

	return (s32)checksum;
}

/**
 *  ixgbe_validate_eeprom_checksum_generic - Validate EEPROM checksum
 *  @hw: pointer to hardware structure
 *  @checksum_val: calculated checksum
 *
 *  Performs checksum calculation and validates the EEPROM checksum.  If the
 *  caller does not need checksum_val, the value can be NULL.
 **/
s32 ixgbe_validate_eeprom_checksum_generic(struct ixgbe_hw *hw,
					   u16 *checksum_val)
{
	s32 status;
	u16 checksum;
	u16 read_checksum = 0;

	/*
	 * Read the first word from the EEPROM. If this times out or fails, do
	 * not continue or we could be in for a very long wait while every
	 * EEPROM read fails
	 */
	status = hw->eeprom.ops.read(hw, 0, &checksum);
	if (status) {
		hw_dbg(hw, "EEPROM read failed\n");
		return status;
	}

	status = hw->eeprom.ops.calc_checksum(hw);
	if (status < 0)
		return status;

	checksum = (u16)(status & 0xffff);

	status = hw->eeprom.ops.read(hw, IXGBE_EEPROM_CHECKSUM, &read_checksum);
	if (status) {
		hw_dbg(hw, "EEPROM read failed\n");
		return status;
	}

	/* Verify read checksum from EEPROM is the same as
	 * calculated checksum
	 */
	if (read_checksum != checksum)
		status = IXGBE_ERR_EEPROM_CHECKSUM;

	/* If the user cares, return the calculated checksum */
	if (checksum_val)
		*checksum_val = checksum;

	return status;
}

/**
 *  ixgbe_update_eeprom_checksum_generic - Updates the EEPROM checksum
 *  @hw: pointer to hardware structure
 **/
s32 ixgbe_update_eeprom_checksum_generic(struct ixgbe_hw *hw)
{
	s32 status;
	u16 checksum;

	/*
	 * Read the first word from the EEPROM. If this times out or fails, do
	 * not continue or we could be in for a very long wait while every
	 * EEPROM read fails
	 */
	status = hw->eeprom.ops.read(hw, 0, &checksum);
	if (status) {
		hw_dbg(hw, "EEPROM read failed\n");
		return status;
	}

	status = hw->eeprom.ops.calc_checksum(hw);
	if (status < 0)
		return status;

	checksum = (u16)(status & 0xffff);

	status = hw->eeprom.ops.write(hw, IXGBE_EEPROM_CHECKSUM, checksum);

	return status;
}

/**
 *  ixgbe_set_rar_generic - Set Rx address register
 *  @hw: pointer to hardware structure
 *  @index: Receive address register to write
 *  @addr: Address to put into receive address register
 *  @vmdq: VMDq "set" or "pool" index
 *  @enable_addr: set flag that address is active
 *
 *  Puts an ethernet address into a receive address register.
 **/
s32 ixgbe_set_rar_generic(struct ixgbe_hw *hw, u32 index, u8 *addr, u32 vmdq,
			  u32 enable_addr)
{
	u32 rar_low, rar_high;
	u32 rar_entries = hw->mac.num_rar_entries;

	/* Make sure we are using a valid rar index range */
	if (index >= rar_entries) {
		hw_dbg(hw, "RAR index %d is out of range.\n", index);
		return IXGBE_ERR_INVALID_ARGUMENT;
	}

	/* setup VMDq pool selection before this RAR gets enabled */
	hw->mac.ops.set_vmdq(hw, index, vmdq);

	/*
	 * HW expects these in little endian so we reverse the byte
	 * order from network order (big endian) to little endian
	 */
	rar_low = ((u32)addr[0] |
		   ((u32)addr[1] << 8) |
		   ((u32)addr[2] << 16) |
		   ((u32)addr[3] << 24));
	/*
	 * Some parts put the VMDq setting in the extra RAH bits,
	 * so save everything except the lower 16 bits that hold part
	 * of the address and the address valid bit.
	 */
	rar_high = IXGBE_READ_REG(hw, IXGBE_RAH(index));
	rar_high &= ~(0x0000FFFF | IXGBE_RAH_AV);
	rar_high |= ((u32)addr[4] | ((u32)addr[5] << 8));

	if (enable_addr != 0)
		rar_high |= IXGBE_RAH_AV;

	/* Record lower 32 bits of MAC address and then make
	 * sure that write is flushed to hardware before writing
	 * the upper 16 bits and setting the valid bit.
	 */
	IXGBE_WRITE_REG(hw, IXGBE_RAL(index), rar_low);
	IXGBE_WRITE_FLUSH(hw);
	IXGBE_WRITE_REG(hw, IXGBE_RAH(index), rar_high);

	return 0;
}

/**
 *  ixgbe_clear_rar_generic - Remove Rx address register
 *  @hw: pointer to hardware structure
 *  @index: Receive address register to write
 *
 *  Clears an ethernet address from a receive address register.
 **/
s32 ixgbe_clear_rar_generic(struct ixgbe_hw *hw, u32 index)
{
	u32 rar_high;
	u32 rar_entries = hw->mac.num_rar_entries;

	/* Make sure we are using a valid rar index range */
	if (index >= rar_entries) {
		hw_dbg(hw, "RAR index %d is out of range.\n", index);
		return IXGBE_ERR_INVALID_ARGUMENT;
	}

	/*
	 * Some parts put the VMDq setting in the extra RAH bits,
	 * so save everything except the lower 16 bits that hold part
	 * of the address and the address valid bit.
	 */
	rar_high = IXGBE_READ_REG(hw, IXGBE_RAH(index));
	rar_high &= ~(0x0000FFFF | IXGBE_RAH_AV);

	/* Clear the address valid bit and upper 16 bits of the address
	 * before clearing the lower bits. This way we aren't updating
	 * a live filter.
	 */
	IXGBE_WRITE_REG(hw, IXGBE_RAH(index), rar_high);
	IXGBE_WRITE_FLUSH(hw);
	IXGBE_WRITE_REG(hw, IXGBE_RAL(index), 0);

	/* clear VMDq pool/queue selection for this RAR */
	hw->mac.ops.clear_vmdq(hw, index, IXGBE_CLEAR_VMDQ_ALL);

	return 0;
}

/**
 *  ixgbe_init_rx_addrs_generic - Initializes receive address filters.
 *  @hw: pointer to hardware structure
 *
 *  Places the MAC address in receive address register 0 and clears the rest
 *  of the receive address registers. Clears the multicast table. Assumes
 *  the receiver is in reset when the routine is called.
 **/
s32 ixgbe_init_rx_addrs_generic(struct ixgbe_hw *hw)
{
	u32 i;
	u32 rar_entries = hw->mac.num_rar_entries;

	/*
	 * If the current mac address is valid, assume it is a software override
	 * to the permanent address.
	 * Otherwise, use the permanent address from the eeprom.
	 */
	if (!is_valid_ether_addr(hw->mac.addr)) {
		/* Get the MAC address from the RAR0 for later reference */
		hw->mac.ops.get_mac_addr(hw, hw->mac.addr);

		hw_dbg(hw, " Keeping Current RAR0 Addr =%pM\n", hw->mac.addr);
	} else {
		/* Setup the receive address. */
		hw_dbg(hw, "Overriding MAC Address in RAR[0]\n");
		hw_dbg(hw, " New MAC Addr =%pM\n", hw->mac.addr);

		hw->mac.ops.set_rar(hw, 0, hw->mac.addr, 0, IXGBE_RAH_AV);
	}

	/*  clear VMDq pool/queue selection for RAR 0 */
	hw->mac.ops.clear_vmdq(hw, 0, IXGBE_CLEAR_VMDQ_ALL);

	hw->addr_ctrl.overflow_promisc = 0;

	hw->addr_ctrl.rar_used_count = 1;

	/* Zero out the other receive addresses. */
	hw_dbg(hw, "Clearing RAR[1-%d]\n", rar_entries - 1);
	for (i = 1; i < rar_entries; i++) {
		IXGBE_WRITE_REG(hw, IXGBE_RAL(i), 0);
		IXGBE_WRITE_REG(hw, IXGBE_RAH(i), 0);
	}

	/* Clear the MTA */
	hw->addr_ctrl.mta_in_use = 0;
	IXGBE_WRITE_REG(hw, IXGBE_MCSTCTRL, hw->mac.mc_filter_type);

	hw_dbg(hw, " Clearing MTA\n");
	for (i = 0; i < hw->mac.mcft_size; i++)
		IXGBE_WRITE_REG(hw, IXGBE_MTA(i), 0);

	if (hw->mac.ops.init_uta_tables)
		hw->mac.ops.init_uta_tables(hw);

	return 0;
}

/**
 *  ixgbe_mta_vector - Determines bit-vector in multicast table to set
 *  @hw: pointer to hardware structure
 *  @mc_addr: the multicast address
 *
 *  Extracts the 12 bits, from a multicast address, to determine which
 *  bit-vector to set in the multicast table. The hardware uses 12 bits, from
 *  incoming rx multicast addresses, to determine the bit-vector to check in
 *  the MTA. Which of the 4 combination, of 12-bits, the hardware uses is set
 *  by the MO field of the MCSTCTRL. The MO field is set during initialization
 *  to mc_filter_type.
 **/
static s32 ixgbe_mta_vector(struct ixgbe_hw *hw, u8 *mc_addr)
{
	u32 vector = 0;

	switch (hw->mac.mc_filter_type) {
	case 0:   /* use bits [47:36] of the address */
		vector = ((mc_addr[4] >> 4) | (((u16)mc_addr[5]) << 4));
		break;
	case 1:   /* use bits [46:35] of the address */
		vector = ((mc_addr[4] >> 3) | (((u16)mc_addr[5]) << 5));
		break;
	case 2:   /* use bits [45:34] of the address */
		vector = ((mc_addr[4] >> 2) | (((u16)mc_addr[5]) << 6));
		break;
	case 3:   /* use bits [43:32] of the address */
		vector = ((mc_addr[4]) | (((u16)mc_addr[5]) << 8));
		break;
	default:  /* Invalid mc_filter_type */
		hw_dbg(hw, "MC filter type param set incorrectly\n");
		break;
	}

	/* vector can only be 12-bits or boundary will be exceeded */
	vector &= 0xFFF;
	return vector;
}

/**
 *  ixgbe_set_mta - Set bit-vector in multicast table
 *  @hw: pointer to hardware structure
 *  @mc_addr: Multicast address
 *
 *  Sets the bit-vector in the multicast table.
 **/
static void ixgbe_set_mta(struct ixgbe_hw *hw, u8 *mc_addr)
{
	u32 vector;
	u32 vector_bit;
	u32 vector_reg;

	hw->addr_ctrl.mta_in_use++;

	vector = ixgbe_mta_vector(hw, mc_addr);
	hw_dbg(hw, " bit-vector = 0x%03X\n", vector);

	/*
	 * The MTA is a register array of 128 32-bit registers. It is treated
	 * like an array of 4096 bits.  We want to set bit
	 * BitArray[vector_value]. So we figure out what register the bit is
	 * in, read it, OR in the new bit, then write back the new value.  The
	 * register is determined by the upper 7 bits of the vector value and
	 * the bit within that register are determined by the lower 5 bits of
	 * the value.
	 */
	vector_reg = (vector >> 5) & 0x7F;
	vector_bit = vector & 0x1F;
	hw->mac.mta_shadow[vector_reg] |= BIT(vector_bit);
}

/**
 *  ixgbe_update_mc_addr_list_generic - Updates MAC list of multicast addresses
 *  @hw: pointer to hardware structure
 *  @netdev: pointer to net device structure
 *
 *  The given list replaces any existing list. Clears the MC addrs from receive
 *  address registers and the multicast table. Uses unused receive address
 *  registers for the first multicast addresses, and hashes the rest into the
 *  multicast table.
 **/
s32 ixgbe_update_mc_addr_list_generic(struct ixgbe_hw *hw,
				      struct net_device *netdev)
{
	struct netdev_hw_addr *ha;
	u32 i;

	/*
	 * Set the new number of MC addresses that we are being requested to
	 * use.
	 */
	hw->addr_ctrl.num_mc_addrs = netdev_mc_count(netdev);
	hw->addr_ctrl.mta_in_use = 0;

	/* Clear mta_shadow */
	hw_dbg(hw, " Clearing MTA\n");
	memset(&hw->mac.mta_shadow, 0, sizeof(hw->mac.mta_shadow));

	/* Update mta shadow */
	netdev_for_each_mc_addr(ha, netdev) {
		hw_dbg(hw, " Adding the multicast addresses:\n");
		ixgbe_set_mta(hw, ha->addr);
	}

	/* Enable mta */
	for (i = 0; i < hw->mac.mcft_size; i++)
		IXGBE_WRITE_REG_ARRAY(hw, IXGBE_MTA(0), i,
				      hw->mac.mta_shadow[i]);

	if (hw->addr_ctrl.mta_in_use > 0)
		IXGBE_WRITE_REG(hw, IXGBE_MCSTCTRL,
				IXGBE_MCSTCTRL_MFE | hw->mac.mc_filter_type);

	hw_dbg(hw, "ixgbe_update_mc_addr_list_generic Complete\n");
	return 0;
}

/**
 *  ixgbe_enable_mc_generic - Enable multicast address in RAR
 *  @hw: pointer to hardware structure
 *
 *  Enables multicast address in RAR and the use of the multicast hash table.
 **/
s32 ixgbe_enable_mc_generic(struct ixgbe_hw *hw)
{
	struct ixgbe_addr_filter_info *a = &hw->addr_ctrl;

	if (a->mta_in_use > 0)
		IXGBE_WRITE_REG(hw, IXGBE_MCSTCTRL, IXGBE_MCSTCTRL_MFE |
				hw->mac.mc_filter_type);

	return 0;
}

/**
 *  ixgbe_disable_mc_generic - Disable multicast address in RAR
 *  @hw: pointer to hardware structure
 *
 *  Disables multicast address in RAR and the use of the multicast hash table.
 **/
s32 ixgbe_disable_mc_generic(struct ixgbe_hw *hw)
{
	struct ixgbe_addr_filter_info *a = &hw->addr_ctrl;

	if (a->mta_in_use > 0)
		IXGBE_WRITE_REG(hw, IXGBE_MCSTCTRL, hw->mac.mc_filter_type);

	return 0;
}

/**
 *  ixgbe_fc_enable_generic - Enable flow control
 *  @hw: pointer to hardware structure
 *
 *  Enable flow control according to the current settings.
 **/
s32 ixgbe_fc_enable_generic(struct ixgbe_hw *hw)
{
	u32 mflcn_reg, fccfg_reg;
	u32 reg;
	u32 fcrtl, fcrth;
	int i;

	/* Validate the water mark configuration. */
	if (!hw->fc.pause_time)
		return IXGBE_ERR_INVALID_LINK_SETTINGS;

	/* Low water mark of zero causes XOFF floods */
	for (i = 0; i < MAX_TRAFFIC_CLASS; i++) {
		if ((hw->fc.current_mode & ixgbe_fc_tx_pause) &&
		    hw->fc.high_water[i]) {
			if (!hw->fc.low_water[i] ||
			    hw->fc.low_water[i] >= hw->fc.high_water[i]) {
				hw_dbg(hw, "Invalid water mark configuration\n");
				return IXGBE_ERR_INVALID_LINK_SETTINGS;
			}
		}
	}

	/* Negotiate the fc mode to use */
	hw->mac.ops.fc_autoneg(hw);

	/* Disable any previous flow control settings */
	mflcn_reg = IXGBE_READ_REG(hw, IXGBE_MFLCN);
	mflcn_reg &= ~(IXGBE_MFLCN_RPFCE_MASK | IXGBE_MFLCN_RFCE);

	fccfg_reg = IXGBE_READ_REG(hw, IXGBE_FCCFG);
	fccfg_reg &= ~(IXGBE_FCCFG_TFCE_802_3X | IXGBE_FCCFG_TFCE_PRIORITY);

	/*
	 * The possible values of fc.current_mode are:
	 * 0: Flow control is completely disabled
	 * 1: Rx flow control is enabled (we can receive pause frames,
	 *    but not send pause frames).
	 * 2: Tx flow control is enabled (we can send pause frames but
	 *    we do not support receiving pause frames).
	 * 3: Both Rx and Tx flow control (symmetric) are enabled.
	 * other: Invalid.
	 */
	switch (hw->fc.current_mode) {
	case ixgbe_fc_none:
		/*
		 * Flow control is disabled by software override or autoneg.
		 * The code below will actually disable it in the HW.
		 */
		break;
	case ixgbe_fc_rx_pause:
		/*
		 * Rx Flow control is enabled and Tx Flow control is
		 * disabled by software override. Since there really
		 * isn't a way to advertise that we are capable of RX
		 * Pause ONLY, we will advertise that we support both
		 * symmetric and asymmetric Rx PAUSE.  Later, we will
		 * disable the adapter's ability to send PAUSE frames.
		 */
		mflcn_reg |= IXGBE_MFLCN_RFCE;
		break;
	case ixgbe_fc_tx_pause:
		/*
		 * Tx Flow control is enabled, and Rx Flow control is
		 * disabled by software override.
		 */
		fccfg_reg |= IXGBE_FCCFG_TFCE_802_3X;
		break;
	case ixgbe_fc_full:
		/* Flow control (both Rx and Tx) is enabled by SW override. */
		mflcn_reg |= IXGBE_MFLCN_RFCE;
		fccfg_reg |= IXGBE_FCCFG_TFCE_802_3X;
		break;
	default:
		hw_dbg(hw, "Flow control param set incorrectly\n");
		return IXGBE_ERR_CONFIG;
	}

	/* Set 802.3x based flow control settings. */
	mflcn_reg |= IXGBE_MFLCN_DPF;
	IXGBE_WRITE_REG(hw, IXGBE_MFLCN, mflcn_reg);
	IXGBE_WRITE_REG(hw, IXGBE_FCCFG, fccfg_reg);

	/* Set up and enable Rx high/low water mark thresholds, enable XON. */
	for (i = 0; i < MAX_TRAFFIC_CLASS; i++) {
		if ((hw->fc.current_mode & ixgbe_fc_tx_pause) &&
		    hw->fc.high_water[i]) {
			fcrtl = (hw->fc.low_water[i] << 10) | IXGBE_FCRTL_XONE;
			IXGBE_WRITE_REG(hw, IXGBE_FCRTL_82599(i), fcrtl);
			fcrth = (hw->fc.high_water[i] << 10) | IXGBE_FCRTH_FCEN;
		} else {
			IXGBE_WRITE_REG(hw, IXGBE_FCRTL_82599(i), 0);
			/*
			 * In order to prevent Tx hangs when the internal Tx
			 * switch is enabled we must set the high water mark
			 * to the Rx packet buffer size - 24KB.  This allows
			 * the Tx switch to function even under heavy Rx
			 * workloads.
			 */
			fcrth = IXGBE_READ_REG(hw, IXGBE_RXPBSIZE(i)) - 24576;
		}

		IXGBE_WRITE_REG(hw, IXGBE_FCRTH_82599(i), fcrth);
	}

	/* Configure pause time (2 TCs per register) */
	reg = hw->fc.pause_time * 0x00010001U;
	for (i = 0; i < (MAX_TRAFFIC_CLASS / 2); i++)
		IXGBE_WRITE_REG(hw, IXGBE_FCTTV(i), reg);

	IXGBE_WRITE_REG(hw, IXGBE_FCRTV, hw->fc.pause_time / 2);

	return 0;
}

/**
 *  ixgbe_negotiate_fc - Negotiate flow control
 *  @hw: pointer to hardware structure
 *  @adv_reg: flow control advertised settings
 *  @lp_reg: link partner's flow control settings
 *  @adv_sym: symmetric pause bit in advertisement
 *  @adv_asm: asymmetric pause bit in advertisement
 *  @lp_sym: symmetric pause bit in link partner advertisement
 *  @lp_asm: asymmetric pause bit in link partner advertisement
 *
 *  Find the intersection between advertised settings and link partner's
 *  advertised settings
 **/
s32 ixgbe_negotiate_fc(struct ixgbe_hw *hw, u32 adv_reg, u32 lp_reg,
		       u32 adv_sym, u32 adv_asm, u32 lp_sym, u32 lp_asm)
{
	if ((!(adv_reg)) ||  (!(lp_reg)))
		return IXGBE_ERR_FC_NOT_NEGOTIATED;

	if ((adv_reg & adv_sym) && (lp_reg & lp_sym)) {
		/*
		 * Now we need to check if the user selected Rx ONLY
		 * of pause frames.  In this case, we had to advertise
		 * FULL flow control because we could not advertise RX
		 * ONLY. Hence, we must now check to see if we need to
		 * turn OFF the TRANSMISSION of PAUSE frames.
		 */
		if (hw->fc.requested_mode == ixgbe_fc_full) {
			hw->fc.current_mode = ixgbe_fc_full;
			hw_dbg(hw, "Flow Control = FULL.\n");
		} else {
			hw->fc.current_mode = ixgbe_fc_rx_pause;
			hw_dbg(hw, "Flow Control=RX PAUSE frames only\n");
		}
	} else if (!(adv_reg & adv_sym) && (adv_reg & adv_asm) &&
		   (lp_reg & lp_sym) && (lp_reg & lp_asm)) {
		hw->fc.current_mode = ixgbe_fc_tx_pause;
		hw_dbg(hw, "Flow Control = TX PAUSE frames only.\n");
	} else if ((adv_reg & adv_sym) && (adv_reg & adv_asm) &&
		   !(lp_reg & lp_sym) && (lp_reg & lp_asm)) {
		hw->fc.current_mode = ixgbe_fc_rx_pause;
		hw_dbg(hw, "Flow Control = RX PAUSE frames only.\n");
	} else {
		hw->fc.current_mode = ixgbe_fc_none;
		hw_dbg(hw, "Flow Control = NONE.\n");
	}
	return 0;
}

/**
 *  ixgbe_fc_autoneg_fiber - Enable flow control on 1 gig fiber
 *  @hw: pointer to hardware structure
 *
 *  Enable flow control according on 1 gig fiber.
 **/
static s32 ixgbe_fc_autoneg_fiber(struct ixgbe_hw *hw)
{
	u32 pcs_anadv_reg, pcs_lpab_reg, linkstat;
	s32 ret_val;

	/*
	 * On multispeed fiber at 1g, bail out if
	 * - link is up but AN did not complete, or if
	 * - link is up and AN completed but timed out
	 */

	linkstat = IXGBE_READ_REG(hw, IXGBE_PCS1GLSTA);
	if ((!!(linkstat & IXGBE_PCS1GLSTA_AN_COMPLETE) == 0) ||
	    (!!(linkstat & IXGBE_PCS1GLSTA_AN_TIMED_OUT) == 1))
		return IXGBE_ERR_FC_NOT_NEGOTIATED;

	pcs_anadv_reg = IXGBE_READ_REG(hw, IXGBE_PCS1GANA);
	pcs_lpab_reg = IXGBE_READ_REG(hw, IXGBE_PCS1GANLP);

	ret_val =  ixgbe_negotiate_fc(hw, pcs_anadv_reg,
			       pcs_lpab_reg, IXGBE_PCS1GANA_SYM_PAUSE,
			       IXGBE_PCS1GANA_ASM_PAUSE,
			       IXGBE_PCS1GANA_SYM_PAUSE,
			       IXGBE_PCS1GANA_ASM_PAUSE);

	return ret_val;
}

/**
 *  ixgbe_fc_autoneg_backplane - Enable flow control IEEE clause 37
 *  @hw: pointer to hardware structure
 *
 *  Enable flow control according to IEEE clause 37.
 **/
static s32 ixgbe_fc_autoneg_backplane(struct ixgbe_hw *hw)
{
	u32 links2, anlp1_reg, autoc_reg, links;
	s32 ret_val;

	/*
	 * On backplane, bail out if
	 * - backplane autoneg was not completed, or if
	 * - we are 82599 and link partner is not AN enabled
	 */
	links = IXGBE_READ_REG(hw, IXGBE_LINKS);
	if ((links & IXGBE_LINKS_KX_AN_COMP) == 0)
		return IXGBE_ERR_FC_NOT_NEGOTIATED;

	if (hw->mac.type == ixgbe_mac_82599EB) {
		links2 = IXGBE_READ_REG(hw, IXGBE_LINKS2);
		if ((links2 & IXGBE_LINKS2_AN_SUPPORTED) == 0)
			return IXGBE_ERR_FC_NOT_NEGOTIATED;
	}
	/*
	 * Read the 10g AN autoc and LP ability registers and resolve
	 * local flow control settings accordingly
	 */
	autoc_reg = IXGBE_READ_REG(hw, IXGBE_AUTOC);
	anlp1_reg = IXGBE_READ_REG(hw, IXGBE_ANLP1);

	ret_val = ixgbe_negotiate_fc(hw, autoc_reg,
		anlp1_reg, IXGBE_AUTOC_SYM_PAUSE, IXGBE_AUTOC_ASM_PAUSE,
		IXGBE_ANLP1_SYM_PAUSE, IXGBE_ANLP1_ASM_PAUSE);

	return ret_val;
}

/**
 *  ixgbe_fc_autoneg_copper - Enable flow control IEEE clause 37
 *  @hw: pointer to hardware structure
 *
 *  Enable flow control according to IEEE clause 37.
 **/
static s32 ixgbe_fc_autoneg_copper(struct ixgbe_hw *hw)
{
	u16 technology_ability_reg = 0;
	u16 lp_technology_ability_reg = 0;

	hw->phy.ops.read_reg(hw, MDIO_AN_ADVERTISE,
			     MDIO_MMD_AN,
			     &technology_ability_reg);
	hw->phy.ops.read_reg(hw, MDIO_AN_LPA,
			     MDIO_MMD_AN,
			     &lp_technology_ability_reg);

	return ixgbe_negotiate_fc(hw, (u32)technology_ability_reg,
				  (u32)lp_technology_ability_reg,
				  IXGBE_TAF_SYM_PAUSE, IXGBE_TAF_ASM_PAUSE,
				  IXGBE_TAF_SYM_PAUSE, IXGBE_TAF_ASM_PAUSE);
}

/**
 *  ixgbe_fc_autoneg - Configure flow control
 *  @hw: pointer to hardware structure
 *
 *  Compares our advertised flow control capabilities to those advertised by
 *  our link partner, and determines the proper flow control mode to use.
 **/
void ixgbe_fc_autoneg(struct ixgbe_hw *hw)
{
	s32 ret_val = IXGBE_ERR_FC_NOT_NEGOTIATED;
	ixgbe_link_speed speed;
	bool link_up;

	/*
	 * AN should have completed when the cable was plugged in.
	 * Look for reasons to bail out.  Bail out if:
	 * - FC autoneg is disabled, or if
	 * - link is not up.
	 *
	 * Since we're being called from an LSC, link is already known to be up.
	 * So use link_up_wait_to_complete=false.
	 */
	if (hw->fc.disable_fc_autoneg)
		goto out;

	hw->mac.ops.check_link(hw, &speed, &link_up, false);
	if (!link_up)
		goto out;

	switch (hw->phy.media_type) {
	/* Autoneg flow control on fiber adapters */
	case ixgbe_media_type_fiber:
		if (speed == IXGBE_LINK_SPEED_1GB_FULL)
			ret_val = ixgbe_fc_autoneg_fiber(hw);
		break;

	/* Autoneg flow control on backplane adapters */
	case ixgbe_media_type_backplane:
		ret_val = ixgbe_fc_autoneg_backplane(hw);
		break;

	/* Autoneg flow control on copper adapters */
	case ixgbe_media_type_copper:
		if (ixgbe_device_supports_autoneg_fc(hw))
			ret_val = ixgbe_fc_autoneg_copper(hw);
		break;

	default:
		break;
	}

out:
	if (ret_val == 0) {
		hw->fc.fc_was_autonegged = true;
	} else {
		hw->fc.fc_was_autonegged = false;
		hw->fc.current_mode = hw->fc.requested_mode;
	}
}

/**
 * ixgbe_pcie_timeout_poll - Return number of times to poll for completion
 * @hw: pointer to hardware structure
 *
 * System-wide timeout range is encoded in PCIe Device Control2 register.
 *
 *  Add 10% to specified maximum and return the number of times to poll for
 *  completion timeout, in units of 100 microsec.  Never return less than
 *  800 = 80 millisec.
 **/
static u32 ixgbe_pcie_timeout_poll(struct ixgbe_hw *hw)
{
	s16 devctl2;
	u32 pollcnt;

	devctl2 = ixgbe_read_pci_cfg_word(hw, IXGBE_PCI_DEVICE_CONTROL2);
	devctl2 &= IXGBE_PCIDEVCTRL2_TIMEO_MASK;

	switch (devctl2) {
	case IXGBE_PCIDEVCTRL2_65_130ms:
		 pollcnt = 1300;         /* 130 millisec */
		break;
	case IXGBE_PCIDEVCTRL2_260_520ms:
		pollcnt = 5200;         /* 520 millisec */
		break;
	case IXGBE_PCIDEVCTRL2_1_2s:
		pollcnt = 20000;        /* 2 sec */
		break;
	case IXGBE_PCIDEVCTRL2_4_8s:
		pollcnt = 80000;        /* 8 sec */
		break;
	case IXGBE_PCIDEVCTRL2_17_34s:
		pollcnt = 34000;        /* 34 sec */
		break;
	case IXGBE_PCIDEVCTRL2_50_100us:        /* 100 microsecs */
	case IXGBE_PCIDEVCTRL2_1_2ms:           /* 2 millisecs */
	case IXGBE_PCIDEVCTRL2_16_32ms:         /* 32 millisec */
	case IXGBE_PCIDEVCTRL2_16_32ms_def:     /* 32 millisec default */
	default:
		pollcnt = 800;          /* 80 millisec minimum */
		break;
	}

	/* add 10% to spec maximum */
	return (pollcnt * 11) / 10;
}

/**
 *  ixgbe_disable_pcie_master - Disable PCI-express master access
 *  @hw: pointer to hardware structure
 *
 *  Disables PCI-Express master access and verifies there are no pending
 *  requests. IXGBE_ERR_MASTER_REQUESTS_PENDING is returned if master disable
 *  bit hasn't caused the master requests to be disabled, else 0
 *  is returned signifying master requests disabled.
 **/
static s32 ixgbe_disable_pcie_master(struct ixgbe_hw *hw)
{
	u32 i, poll;
	u16 value;

	/* Always set this bit to ensure any future transactions are blocked */
	IXGBE_WRITE_REG(hw, IXGBE_CTRL, IXGBE_CTRL_GIO_DIS);

	/* Poll for bit to read as set */
	for (i = 0; i < IXGBE_PCI_MASTER_DISABLE_TIMEOUT; i++) {
		if (IXGBE_READ_REG(hw, IXGBE_CTRL) & IXGBE_CTRL_GIO_DIS)
			break;
		usleep_range(100, 120);
	}
	if (i >= IXGBE_PCI_MASTER_DISABLE_TIMEOUT) {
		hw_dbg(hw, "GIO disable did not set - requesting resets\n");
		goto gio_disable_fail;
	}

	/* Exit if master requests are blocked */
	if (!(IXGBE_READ_REG(hw, IXGBE_STATUS) & IXGBE_STATUS_GIO) ||
	    ixgbe_removed(hw->hw_addr))
		return 0;

	/* Poll for master request bit to clear */
	for (i = 0; i < IXGBE_PCI_MASTER_DISABLE_TIMEOUT; i++) {
		udelay(100);
		if (!(IXGBE_READ_REG(hw, IXGBE_STATUS) & IXGBE_STATUS_GIO))
			return 0;
	}

	/*
	 * Two consecutive resets are required via CTRL.RST per datasheet
	 * 5.2.5.3.2 Master Disable.  We set a flag to inform the reset routine
	 * of this need.  The first reset prevents new master requests from
	 * being issued by our device.  We then must wait 1usec or more for any
	 * remaining completions from the PCIe bus to trickle in, and then reset
	 * again to clear out any effects they may have had on our device.
	 */
	hw_dbg(hw, "GIO Master Disable bit didn't clear - requesting resets\n");
gio_disable_fail:
	hw->mac.flags |= IXGBE_FLAGS_DOUBLE_RESET_REQUIRED;

	if (hw->mac.type >= ixgbe_mac_X550)
		return 0;

	/*
	 * Before proceeding, make sure that the PCIe block does not have
	 * transactions pending.
	 */
	poll = ixgbe_pcie_timeout_poll(hw);
	for (i = 0; i < poll; i++) {
		udelay(100);
		value = ixgbe_read_pci_cfg_word(hw, IXGBE_PCI_DEVICE_STATUS);
		if (ixgbe_removed(hw->hw_addr))
			return 0;
		if (!(value & IXGBE_PCI_DEVICE_STATUS_TRANSACTION_PENDING))
			return 0;
	}

	hw_dbg(hw, "PCIe transaction pending bit also did not clear.\n");
	return IXGBE_ERR_MASTER_REQUESTS_PENDING;
}

/**
 *  ixgbe_acquire_swfw_sync - Acquire SWFW semaphore
 *  @hw: pointer to hardware structure
 *  @mask: Mask to specify which semaphore to acquire
 *
 *  Acquires the SWFW semaphore through the GSSR register for the specified
 *  function (CSR, PHY0, PHY1, EEPROM, Flash)
 **/
s32 ixgbe_acquire_swfw_sync(struct ixgbe_hw *hw, u32 mask)
{
	u32 gssr = 0;
	u32 swmask = mask;
	u32 fwmask = mask << 5;
	u32 timeout = 200;
	u32 i;

	for (i = 0; i < timeout; i++) {
		/*
		 * SW NVM semaphore bit is used for access to all
		 * SW_FW_SYNC bits (not just NVM)
		 */
		if (ixgbe_get_eeprom_semaphore(hw))
			return IXGBE_ERR_SWFW_SYNC;

		gssr = IXGBE_READ_REG(hw, IXGBE_GSSR);
		if (!(gssr & (fwmask | swmask))) {
			gssr |= swmask;
			IXGBE_WRITE_REG(hw, IXGBE_GSSR, gssr);
			ixgbe_release_eeprom_semaphore(hw);
			return 0;
		} else {
			/* Resource is currently in use by FW or SW */
			ixgbe_release_eeprom_semaphore(hw);
			usleep_range(5000, 10000);
		}
	}

	/* If time expired clear the bits holding the lock and retry */
	if (gssr & (fwmask | swmask))
		ixgbe_release_swfw_sync(hw, gssr & (fwmask | swmask));

	usleep_range(5000, 10000);
	return IXGBE_ERR_SWFW_SYNC;
}

/**
 *  ixgbe_release_swfw_sync - Release SWFW semaphore
 *  @hw: pointer to hardware structure
 *  @mask: Mask to specify which semaphore to release
 *
 *  Releases the SWFW semaphore through the GSSR register for the specified
 *  function (CSR, PHY0, PHY1, EEPROM, Flash)
 **/
void ixgbe_release_swfw_sync(struct ixgbe_hw *hw, u32 mask)
{
	u32 gssr;
	u32 swmask = mask;

	ixgbe_get_eeprom_semaphore(hw);

	gssr = IXGBE_READ_REG(hw, IXGBE_GSSR);
	gssr &= ~swmask;
	IXGBE_WRITE_REG(hw, IXGBE_GSSR, gssr);

	ixgbe_release_eeprom_semaphore(hw);
}

/**
 * prot_autoc_read_generic - Hides MAC differences needed for AUTOC read
 * @hw: pointer to hardware structure
 * @reg_val: Value we read from AUTOC
 * @locked: bool to indicate whether the SW/FW lock should be taken.  Never
 *	    true in this the generic case.
 *
 * The default case requires no protection so just to the register read.
 **/
s32 prot_autoc_read_generic(struct ixgbe_hw *hw, bool *locked, u32 *reg_val)
{
	*locked = false;
	*reg_val = IXGBE_READ_REG(hw, IXGBE_AUTOC);
	return 0;
}

/**
 * prot_autoc_write_generic - Hides MAC differences needed for AUTOC write
 * @hw: pointer to hardware structure
 * @reg_val: value to write to AUTOC
 * @locked: bool to indicate whether the SW/FW lock was already taken by
 *	    previous read.
 **/
s32 prot_autoc_write_generic(struct ixgbe_hw *hw, u32 reg_val, bool locked)
{
	IXGBE_WRITE_REG(hw, IXGBE_AUTOC, reg_val);
	return 0;
}

/**
 *  ixgbe_disable_rx_buff_generic - Stops the receive data path
 *  @hw: pointer to hardware structure
 *
 *  Stops the receive data path and waits for the HW to internally
 *  empty the Rx security block.
 **/
s32 ixgbe_disable_rx_buff_generic(struct ixgbe_hw *hw)
{
#define IXGBE_MAX_SECRX_POLL 40
	int i;
	int secrxreg;

	secrxreg = IXGBE_READ_REG(hw, IXGBE_SECRXCTRL);
	secrxreg |= IXGBE_SECRXCTRL_RX_DIS;
	IXGBE_WRITE_REG(hw, IXGBE_SECRXCTRL, secrxreg);
	for (i = 0; i < IXGBE_MAX_SECRX_POLL; i++) {
		secrxreg = IXGBE_READ_REG(hw, IXGBE_SECRXSTAT);
		if (secrxreg & IXGBE_SECRXSTAT_SECRX_RDY)
			break;
		else
			/* Use interrupt-safe sleep just in case */
			udelay(1000);
	}

	/* For informational purposes only */
	if (i >= IXGBE_MAX_SECRX_POLL)
		hw_dbg(hw, "Rx unit being enabled before security path fully disabled. Continuing with init.\n");

	return 0;

}

/**
 *  ixgbe_enable_rx_buff_generic - Enables the receive data path
 *  @hw: pointer to hardware structure
 *
 *  Enables the receive data path
 **/
s32 ixgbe_enable_rx_buff_generic(struct ixgbe_hw *hw)
{
	u32 secrxreg;

	secrxreg = IXGBE_READ_REG(hw, IXGBE_SECRXCTRL);
	secrxreg &= ~IXGBE_SECRXCTRL_RX_DIS;
	IXGBE_WRITE_REG(hw, IXGBE_SECRXCTRL, secrxreg);
	IXGBE_WRITE_FLUSH(hw);

	return 0;
}

/**
 *  ixgbe_enable_rx_dma_generic - Enable the Rx DMA unit
 *  @hw: pointer to hardware structure
 *  @regval: register value to write to RXCTRL
 *
 *  Enables the Rx DMA unit
 **/
s32 ixgbe_enable_rx_dma_generic(struct ixgbe_hw *hw, u32 regval)
{
	if (regval & IXGBE_RXCTRL_RXEN)
		hw->mac.ops.enable_rx(hw);
	else
		hw->mac.ops.disable_rx(hw);

	return 0;
}

/**
 *  ixgbe_blink_led_start_generic - Blink LED based on index.
 *  @hw: pointer to hardware structure
 *  @index: led number to blink
 **/
s32 ixgbe_blink_led_start_generic(struct ixgbe_hw *hw, u32 index)
{
	ixgbe_link_speed speed = 0;
	bool link_up = false;
	u32 autoc_reg = IXGBE_READ_REG(hw, IXGBE_AUTOC);
	u32 led_reg = IXGBE_READ_REG(hw, IXGBE_LEDCTL);
	bool locked = false;
	s32 ret_val;

	if (index > 3)
		return IXGBE_ERR_PARAM;

	/*
	 * Link must be up to auto-blink the LEDs;
	 * Force it if link is down.
	 */
	hw->mac.ops.check_link(hw, &speed, &link_up, false);

	if (!link_up) {
		ret_val = hw->mac.ops.prot_autoc_read(hw, &locked, &autoc_reg);
		if (ret_val)
			return ret_val;

		autoc_reg |= IXGBE_AUTOC_AN_RESTART;
		autoc_reg |= IXGBE_AUTOC_FLU;

		ret_val = hw->mac.ops.prot_autoc_write(hw, autoc_reg, locked);
		if (ret_val)
			return ret_val;

		IXGBE_WRITE_FLUSH(hw);

		usleep_range(10000, 20000);
	}

	led_reg &= ~IXGBE_LED_MODE_MASK(index);
	led_reg |= IXGBE_LED_BLINK(index);
	IXGBE_WRITE_REG(hw, IXGBE_LEDCTL, led_reg);
	IXGBE_WRITE_FLUSH(hw);

	return 0;
}

/**
 *  ixgbe_blink_led_stop_generic - Stop blinking LED based on index.
 *  @hw: pointer to hardware structure
 *  @index: led number to stop blinking
 **/
s32 ixgbe_blink_led_stop_generic(struct ixgbe_hw *hw, u32 index)
{
	u32 autoc_reg = 0;
	u32 led_reg = IXGBE_READ_REG(hw, IXGBE_LEDCTL);
	bool locked = false;
	s32 ret_val;

	if (index > 3)
		return IXGBE_ERR_PARAM;

	ret_val = hw->mac.ops.prot_autoc_read(hw, &locked, &autoc_reg);
	if (ret_val)
		return ret_val;

	autoc_reg &= ~IXGBE_AUTOC_FLU;
	autoc_reg |= IXGBE_AUTOC_AN_RESTART;

	ret_val = hw->mac.ops.prot_autoc_write(hw, autoc_reg, locked);
	if (ret_val)
		return ret_val;

	led_reg &= ~IXGBE_LED_MODE_MASK(index);
	led_reg &= ~IXGBE_LED_BLINK(index);
	led_reg |= IXGBE_LED_LINK_ACTIVE << IXGBE_LED_MODE_SHIFT(index);
	IXGBE_WRITE_REG(hw, IXGBE_LEDCTL, led_reg);
	IXGBE_WRITE_FLUSH(hw);

	return 0;
}

/**
 *  ixgbe_get_san_mac_addr_offset - Get SAN MAC address offset from the EEPROM
 *  @hw: pointer to hardware structure
 *  @san_mac_offset: SAN MAC address offset
 *
 *  This function will read the EEPROM location for the SAN MAC address
 *  pointer, and returns the value at that location.  This is used in both
 *  get and set mac_addr routines.
 **/
static s32 ixgbe_get_san_mac_addr_offset(struct ixgbe_hw *hw,
					u16 *san_mac_offset)
{
	s32 ret_val;

	/*
	 * First read the EEPROM pointer to see if the MAC addresses are
	 * available.
	 */
	ret_val = hw->eeprom.ops.read(hw, IXGBE_SAN_MAC_ADDR_PTR,
				      san_mac_offset);
	if (ret_val)
		hw_err(hw, "eeprom read at offset %d failed\n",
		       IXGBE_SAN_MAC_ADDR_PTR);

	return ret_val;
}

/**
 *  ixgbe_get_san_mac_addr_generic - SAN MAC address retrieval from the EEPROM
 *  @hw: pointer to hardware structure
 *  @san_mac_addr: SAN MAC address
 *
 *  Reads the SAN MAC address from the EEPROM, if it's available.  This is
 *  per-port, so set_lan_id() must be called before reading the addresses.
 *  set_lan_id() is called by identify_sfp(), but this cannot be relied
 *  upon for non-SFP connections, so we must call it here.
 **/
s32 ixgbe_get_san_mac_addr_generic(struct ixgbe_hw *hw, u8 *san_mac_addr)
{
	u16 san_mac_data, san_mac_offset;
	u8 i;
	s32 ret_val;

	/*
	 * First read the EEPROM pointer to see if the MAC addresses are
	 * available.  If they're not, no point in calling set_lan_id() here.
	 */
	ret_val = ixgbe_get_san_mac_addr_offset(hw, &san_mac_offset);
	if (ret_val || san_mac_offset == 0 || san_mac_offset == 0xFFFF)

		goto san_mac_addr_clr;

	/* make sure we know which port we need to program */
	hw->mac.ops.set_lan_id(hw);
	/* apply the port offset to the address offset */
	(hw->bus.func) ? (san_mac_offset += IXGBE_SAN_MAC_ADDR_PORT1_OFFSET) :
			 (san_mac_offset += IXGBE_SAN_MAC_ADDR_PORT0_OFFSET);
	for (i = 0; i < 3; i++) {
		ret_val = hw->eeprom.ops.read(hw, san_mac_offset,
					      &san_mac_data);
		if (ret_val) {
			hw_err(hw, "eeprom read at offset %d failed\n",
			       san_mac_offset);
			goto san_mac_addr_clr;
		}
		san_mac_addr[i * 2] = (u8)(san_mac_data);
		san_mac_addr[i * 2 + 1] = (u8)(san_mac_data >> 8);
		san_mac_offset++;
	}
	return 0;

san_mac_addr_clr:
	/* No addresses available in this EEPROM.  It's not necessarily an
	 * error though, so just wipe the local address and return.
	 */
	for (i = 0; i < 6; i++)
		san_mac_addr[i] = 0xFF;
	return ret_val;
}

/**
 *  ixgbe_get_pcie_msix_count_generic - Gets MSI-X vector count
 *  @hw: pointer to hardware structure
 *
 *  Read PCIe configuration space, and get the MSI-X vector count from
 *  the capabilities table.
 **/
u16 ixgbe_get_pcie_msix_count_generic(struct ixgbe_hw *hw)
{
	u16 msix_count;
	u16 max_msix_count;
	u16 pcie_offset;

	switch (hw->mac.type) {
	case ixgbe_mac_82598EB:
		pcie_offset = IXGBE_PCIE_MSIX_82598_CAPS;
		max_msix_count = IXGBE_MAX_MSIX_VECTORS_82598;
		break;
	case ixgbe_mac_82599EB:
	case ixgbe_mac_X540:
	case ixgbe_mac_X550:
	case ixgbe_mac_X550EM_x:
	case ixgbe_mac_x550em_a:
		pcie_offset = IXGBE_PCIE_MSIX_82599_CAPS;
		max_msix_count = IXGBE_MAX_MSIX_VECTORS_82599;
		break;
	default:
		return 1;
	}

	msix_count = ixgbe_read_pci_cfg_word(hw, pcie_offset);
	if (ixgbe_removed(hw->hw_addr))
		msix_count = 0;
	msix_count &= IXGBE_PCIE_MSIX_TBL_SZ_MASK;

	/* MSI-X count is zero-based in HW */
	msix_count++;

	if (msix_count > max_msix_count)
		msix_count = max_msix_count;

	return msix_count;
}

/**
 *  ixgbe_clear_vmdq_generic - Disassociate a VMDq pool index from a rx address
 *  @hw: pointer to hardware struct
 *  @rar: receive address register index to disassociate
 *  @vmdq: VMDq pool index to remove from the rar
 **/
s32 ixgbe_clear_vmdq_generic(struct ixgbe_hw *hw, u32 rar, u32 vmdq)
{
	u32 mpsar_lo, mpsar_hi;
	u32 rar_entries = hw->mac.num_rar_entries;

	/* Make sure we are using a valid rar index range */
	if (rar >= rar_entries) {
		hw_dbg(hw, "RAR index %d is out of range.\n", rar);
		return IXGBE_ERR_INVALID_ARGUMENT;
	}

	mpsar_lo = IXGBE_READ_REG(hw, IXGBE_MPSAR_LO(rar));
	mpsar_hi = IXGBE_READ_REG(hw, IXGBE_MPSAR_HI(rar));

	if (ixgbe_removed(hw->hw_addr))
		return 0;

	if (!mpsar_lo && !mpsar_hi)
		return 0;

	if (vmdq == IXGBE_CLEAR_VMDQ_ALL) {
		if (mpsar_lo) {
			IXGBE_WRITE_REG(hw, IXGBE_MPSAR_LO(rar), 0);
			mpsar_lo = 0;
		}
		if (mpsar_hi) {
			IXGBE_WRITE_REG(hw, IXGBE_MPSAR_HI(rar), 0);
			mpsar_hi = 0;
		}
	} else if (vmdq < 32) {
		mpsar_lo &= ~BIT(vmdq);
		IXGBE_WRITE_REG(hw, IXGBE_MPSAR_LO(rar), mpsar_lo);
	} else {
		mpsar_hi &= ~BIT(vmdq - 32);
		IXGBE_WRITE_REG(hw, IXGBE_MPSAR_HI(rar), mpsar_hi);
	}

	/* was that the last pool using this rar? */
	if (mpsar_lo == 0 && mpsar_hi == 0 &&
	    rar != 0 && rar != hw->mac.san_mac_rar_index)
		hw->mac.ops.clear_rar(hw, rar);

	return 0;
}

/**
 *  ixgbe_set_vmdq_generic - Associate a VMDq pool index with a rx address
 *  @hw: pointer to hardware struct
 *  @rar: receive address register index to associate with a VMDq index
 *  @vmdq: VMDq pool index
 **/
s32 ixgbe_set_vmdq_generic(struct ixgbe_hw *hw, u32 rar, u32 vmdq)
{
	u32 mpsar;
	u32 rar_entries = hw->mac.num_rar_entries;

	/* Make sure we are using a valid rar index range */
	if (rar >= rar_entries) {
		hw_dbg(hw, "RAR index %d is out of range.\n", rar);
		return IXGBE_ERR_INVALID_ARGUMENT;
	}

	if (vmdq < 32) {
		mpsar = IXGBE_READ_REG(hw, IXGBE_MPSAR_LO(rar));
		mpsar |= BIT(vmdq);
		IXGBE_WRITE_REG(hw, IXGBE_MPSAR_LO(rar), mpsar);
	} else {
		mpsar = IXGBE_READ_REG(hw, IXGBE_MPSAR_HI(rar));
		mpsar |= BIT(vmdq - 32);
		IXGBE_WRITE_REG(hw, IXGBE_MPSAR_HI(rar), mpsar);
	}
	return 0;
}

/**
 *  ixgbe_set_vmdq_san_mac_generic - Associate VMDq pool index with a rx address
 *  @hw: pointer to hardware struct
 *  @vmdq: VMDq pool index
 *
 *  This function should only be involved in the IOV mode.
 *  In IOV mode, Default pool is next pool after the number of
 *  VFs advertized and not 0.
 *  MPSAR table needs to be updated for SAN_MAC RAR [hw->mac.san_mac_rar_index]
 **/
s32 ixgbe_set_vmdq_san_mac_generic(struct ixgbe_hw *hw, u32 vmdq)
{
	u32 rar = hw->mac.san_mac_rar_index;

	if (vmdq < 32) {
		IXGBE_WRITE_REG(hw, IXGBE_MPSAR_LO(rar), BIT(vmdq));
		IXGBE_WRITE_REG(hw, IXGBE_MPSAR_HI(rar), 0);
	} else {
		IXGBE_WRITE_REG(hw, IXGBE_MPSAR_LO(rar), 0);
		IXGBE_WRITE_REG(hw, IXGBE_MPSAR_HI(rar), BIT(vmdq - 32));
	}

	return 0;
}

/**
 *  ixgbe_init_uta_tables_generic - Initialize the Unicast Table Array
 *  @hw: pointer to hardware structure
 **/
s32 ixgbe_init_uta_tables_generic(struct ixgbe_hw *hw)
{
	int i;

	for (i = 0; i < 128; i++)
		IXGBE_WRITE_REG(hw, IXGBE_UTA(i), 0);

	return 0;
}

/**
 *  ixgbe_find_vlvf_slot - find the vlanid or the first empty slot
 *  @hw: pointer to hardware structure
 *  @vlan: VLAN id to write to VLAN filter
 *  @vlvf_bypass: true to find vlanid only, false returns first empty slot if
 *		  vlanid not found
 *
 *  return the VLVF index where this VLAN id should be placed
 *
 **/
static s32 ixgbe_find_vlvf_slot(struct ixgbe_hw *hw, u32 vlan, bool vlvf_bypass)
{
	s32 regindex, first_empty_slot;
	u32 bits;

	/* short cut the special case */
	if (vlan == 0)
		return 0;

	/* if vlvf_bypass is set we don't want to use an empty slot, we
	 * will simply bypass the VLVF if there are no entries present in the
	 * VLVF that contain our VLAN
	 */
	first_empty_slot = vlvf_bypass ? IXGBE_ERR_NO_SPACE : 0;

	/* add VLAN enable bit for comparison */
	vlan |= IXGBE_VLVF_VIEN;

	/* Search for the vlan id in the VLVF entries. Save off the first empty
	 * slot found along the way.
	 *
	 * pre-decrement loop covering (IXGBE_VLVF_ENTRIES - 1) .. 1
	 */
	for (regindex = IXGBE_VLVF_ENTRIES; --regindex;) {
		bits = IXGBE_READ_REG(hw, IXGBE_VLVF(regindex));
		if (bits == vlan)
			return regindex;
		if (!first_empty_slot && !bits)
			first_empty_slot = regindex;
	}

	/* If we are here then we didn't find the VLAN.  Return first empty
	 * slot we found during our search, else error.
	 */
	if (!first_empty_slot)
		hw_dbg(hw, "No space in VLVF.\n");

	return first_empty_slot ? : IXGBE_ERR_NO_SPACE;
}

/**
 *  ixgbe_set_vfta_generic - Set VLAN filter table
 *  @hw: pointer to hardware structure
 *  @vlan: VLAN id to write to VLAN filter
 *  @vind: VMDq output index that maps queue to VLAN id in VFVFB
 *  @vlan_on: boolean flag to turn on/off VLAN in VFVF
 *  @vlvf_bypass: boolean flag indicating updating default pool is okay
 *
 *  Turn on/off specified VLAN in the VLAN filter table.
 **/
s32 ixgbe_set_vfta_generic(struct ixgbe_hw *hw, u32 vlan, u32 vind,
			   bool vlan_on, bool vlvf_bypass)
{
	u32 regidx, vfta_delta, vfta, bits;
	s32 vlvf_index;

	if ((vlan > 4095) || (vind > 63))
		return IXGBE_ERR_PARAM;

	/*
	 * this is a 2 part operation - first the VFTA, then the
	 * VLVF and VLVFB if VT Mode is set
	 * We don't write the VFTA until we know the VLVF part succeeded.
	 */

	/* Part 1
	 * The VFTA is a bitstring made up of 128 32-bit registers
	 * that enable the particular VLAN id, much like the MTA:
	 *    bits[11-5]: which register
	 *    bits[4-0]:  which bit in the register
	 */
	regidx = vlan / 32;
	vfta_delta = BIT(vlan % 32);
	vfta = IXGBE_READ_REG(hw, IXGBE_VFTA(regidx));

	/* vfta_delta represents the difference between the current value
	 * of vfta and the value we want in the register.  Since the diff
	 * is an XOR mask we can just update vfta using an XOR.
	 */
	vfta_delta &= vlan_on ? ~vfta : vfta;
	vfta ^= vfta_delta;

	/* Part 2
	 * If VT Mode is set
	 *   Either vlan_on
	 *     make sure the vlan is in VLVF
	 *     set the vind bit in the matching VLVFB
	 *   Or !vlan_on
	 *     clear the pool bit and possibly the vind
	 */
	if (!(IXGBE_READ_REG(hw, IXGBE_VT_CTL) & IXGBE_VT_CTL_VT_ENABLE))
		goto vfta_update;

	vlvf_index = ixgbe_find_vlvf_slot(hw, vlan, vlvf_bypass);
	if (vlvf_index < 0) {
		if (vlvf_bypass)
			goto vfta_update;
		return vlvf_index;
	}

	bits = IXGBE_READ_REG(hw, IXGBE_VLVFB(vlvf_index * 2 + vind / 32));

	/* set the pool bit */
	bits |= BIT(vind % 32);
	if (vlan_on)
		goto vlvf_update;

	/* clear the pool bit */
	bits ^= BIT(vind % 32);

	if (!bits &&
	    !IXGBE_READ_REG(hw, IXGBE_VLVFB(vlvf_index * 2 + 1 - vind / 32))) {
		/* Clear VFTA first, then disable VLVF.  Otherwise
		 * we run the risk of stray packets leaking into
		 * the PF via the default pool
		 */
		if (vfta_delta)
			IXGBE_WRITE_REG(hw, IXGBE_VFTA(regidx), vfta);

		/* disable VLVF and clear remaining bit from pool */
		IXGBE_WRITE_REG(hw, IXGBE_VLVF(vlvf_index), 0);
		IXGBE_WRITE_REG(hw, IXGBE_VLVFB(vlvf_index * 2 + vind / 32), 0);

		return 0;
	}

	/* If there are still bits set in the VLVFB registers
	 * for the VLAN ID indicated we need to see if the
	 * caller is requesting that we clear the VFTA entry bit.
	 * If the caller has requested that we clear the VFTA
	 * entry bit but there are still pools/VFs using this VLAN
	 * ID entry then ignore the request.  We're not worried
	 * about the case where we're turning the VFTA VLAN ID
	 * entry bit on, only when requested to turn it off as
	 * there may be multiple pools and/or VFs using the
	 * VLAN ID entry.  In that case we cannot clear the
	 * VFTA bit until all pools/VFs using that VLAN ID have also
	 * been cleared.  This will be indicated by "bits" being
	 * zero.
	 */
	vfta_delta = 0;

vlvf_update:
	/* record pool change and enable VLAN ID if not already enabled */
	IXGBE_WRITE_REG(hw, IXGBE_VLVFB(vlvf_index * 2 + vind / 32), bits);
	IXGBE_WRITE_REG(hw, IXGBE_VLVF(vlvf_index), IXGBE_VLVF_VIEN | vlan);

vfta_update:
	/* Update VFTA now that we are ready for traffic */
	if (vfta_delta)
		IXGBE_WRITE_REG(hw, IXGBE_VFTA(regidx), vfta);

	return 0;
}

/**
 *  ixgbe_clear_vfta_generic - Clear VLAN filter table
 *  @hw: pointer to hardware structure
 *
 *  Clears the VLAN filer table, and the VMDq index associated with the filter
 **/
s32 ixgbe_clear_vfta_generic(struct ixgbe_hw *hw)
{
	u32 offset;

	for (offset = 0; offset < hw->mac.vft_size; offset++)
		IXGBE_WRITE_REG(hw, IXGBE_VFTA(offset), 0);

	for (offset = 0; offset < IXGBE_VLVF_ENTRIES; offset++) {
		IXGBE_WRITE_REG(hw, IXGBE_VLVF(offset), 0);
		IXGBE_WRITE_REG(hw, IXGBE_VLVFB(offset * 2), 0);
		IXGBE_WRITE_REG(hw, IXGBE_VLVFB(offset * 2 + 1), 0);
	}

	return 0;
}

/**
 *  ixgbe_need_crosstalk_fix - Determine if we need to do cross talk fix
 *  @hw: pointer to hardware structure
 *
 *  Contains the logic to identify if we need to verify link for the
 *  crosstalk fix
 **/
static bool ixgbe_need_crosstalk_fix(struct ixgbe_hw *hw)
{
	/* Does FW say we need the fix */
	if (!hw->need_crosstalk_fix)
		return false;

	/* Only consider SFP+ PHYs i.e. media type fiber */
	switch (hw->mac.ops.get_media_type(hw)) {
	case ixgbe_media_type_fiber:
	case ixgbe_media_type_fiber_qsfp:
		break;
	default:
		return false;
	}

	return true;
}

/**
 *  ixgbe_check_mac_link_generic - Determine link and speed status
 *  @hw: pointer to hardware structure
 *  @speed: pointer to link speed
 *  @link_up: true when link is up
 *  @link_up_wait_to_complete: bool used to wait for link up or not
 *
 *  Reads the links register to determine if link is up and the current speed
 **/
s32 ixgbe_check_mac_link_generic(struct ixgbe_hw *hw, ixgbe_link_speed *speed,
				 bool *link_up, bool link_up_wait_to_complete)
{
	u32 links_reg, links_orig;
	u32 i;

	/* If Crosstalk fix enabled do the sanity check of making sure
	 * the SFP+ cage is full.
	 */
	if (ixgbe_need_crosstalk_fix(hw)) {
		u32 sfp_cage_full;

		switch (hw->mac.type) {
		case ixgbe_mac_82599EB:
			sfp_cage_full = IXGBE_READ_REG(hw, IXGBE_ESDP) &
					IXGBE_ESDP_SDP2;
			break;
		case ixgbe_mac_X550EM_x:
		case ixgbe_mac_x550em_a:
			sfp_cage_full = IXGBE_READ_REG(hw, IXGBE_ESDP) &
					IXGBE_ESDP_SDP0;
			break;
		default:
			/* sanity check - No SFP+ devices here */
			sfp_cage_full = false;
			break;
		}

		if (!sfp_cage_full) {
			*link_up = false;
			*speed = IXGBE_LINK_SPEED_UNKNOWN;
			return 0;
		}
	}

	/* clear the old state */
	links_orig = IXGBE_READ_REG(hw, IXGBE_LINKS);

	links_reg = IXGBE_READ_REG(hw, IXGBE_LINKS);

	if (links_orig != links_reg) {
		hw_dbg(hw, "LINKS changed from %08X to %08X\n",
		       links_orig, links_reg);
	}

	if (link_up_wait_to_complete) {
		for (i = 0; i < IXGBE_LINK_UP_TIME; i++) {
			if (links_reg & IXGBE_LINKS_UP) {
				*link_up = true;
				break;
			} else {
				*link_up = false;
			}
			msleep(100);
			links_reg = IXGBE_READ_REG(hw, IXGBE_LINKS);
		}
	} else {
		if (links_reg & IXGBE_LINKS_UP)
			*link_up = true;
		else
			*link_up = false;
	}

	switch (links_reg & IXGBE_LINKS_SPEED_82599) {
	case IXGBE_LINKS_SPEED_10G_82599:
		if ((hw->mac.type >= ixgbe_mac_X550) &&
		    (links_reg & IXGBE_LINKS_SPEED_NON_STD))
			*speed = IXGBE_LINK_SPEED_2_5GB_FULL;
		else
			*speed = IXGBE_LINK_SPEED_10GB_FULL;
		break;
	case IXGBE_LINKS_SPEED_1G_82599:
		*speed = IXGBE_LINK_SPEED_1GB_FULL;
		break;
	case IXGBE_LINKS_SPEED_100_82599:
		if ((hw->mac.type >= ixgbe_mac_X550) &&
		    (links_reg & IXGBE_LINKS_SPEED_NON_STD))
			*speed = IXGBE_LINK_SPEED_5GB_FULL;
		else
			*speed = IXGBE_LINK_SPEED_100_FULL;
		break;
	case IXGBE_LINKS_SPEED_10_X550EM_A:
		*speed = IXGBE_LINK_SPEED_UNKNOWN;
		if (hw->device_id == IXGBE_DEV_ID_X550EM_A_1G_T ||
		    hw->device_id == IXGBE_DEV_ID_X550EM_A_1G_T_L) {
			*speed = IXGBE_LINK_SPEED_10_FULL;
		}
		break;
	default:
		*speed = IXGBE_LINK_SPEED_UNKNOWN;
	}

	return 0;
}

/**
 *  ixgbe_get_wwn_prefix_generic - Get alternative WWNN/WWPN prefix from
 *  the EEPROM
 *  @hw: pointer to hardware structure
 *  @wwnn_prefix: the alternative WWNN prefix
 *  @wwpn_prefix: the alternative WWPN prefix
 *
 *  This function will read the EEPROM from the alternative SAN MAC address
 *  block to check the support for the alternative WWNN/WWPN prefix support.
 **/
s32 ixgbe_get_wwn_prefix_generic(struct ixgbe_hw *hw, u16 *wwnn_prefix,
					u16 *wwpn_prefix)
{
	u16 offset, caps;
	u16 alt_san_mac_blk_offset;

	/* clear output first */
	*wwnn_prefix = 0xFFFF;
	*wwpn_prefix = 0xFFFF;

	/* check if alternative SAN MAC is supported */
	offset = IXGBE_ALT_SAN_MAC_ADDR_BLK_PTR;
	if (hw->eeprom.ops.read(hw, offset, &alt_san_mac_blk_offset))
		goto wwn_prefix_err;

	if ((alt_san_mac_blk_offset == 0) ||
	    (alt_san_mac_blk_offset == 0xFFFF))
		return 0;

	/* check capability in alternative san mac address block */
	offset = alt_san_mac_blk_offset + IXGBE_ALT_SAN_MAC_ADDR_CAPS_OFFSET;
	if (hw->eeprom.ops.read(hw, offset, &caps))
		goto wwn_prefix_err;
	if (!(caps & IXGBE_ALT_SAN_MAC_ADDR_CAPS_ALTWWN))
		return 0;

	/* get the corresponding prefix for WWNN/WWPN */
	offset = alt_san_mac_blk_offset + IXGBE_ALT_SAN_MAC_ADDR_WWNN_OFFSET;
	if (hw->eeprom.ops.read(hw, offset, wwnn_prefix))
		hw_err(hw, "eeprom read at offset %d failed\n", offset);

	offset = alt_san_mac_blk_offset + IXGBE_ALT_SAN_MAC_ADDR_WWPN_OFFSET;
	if (hw->eeprom.ops.read(hw, offset, wwpn_prefix))
		goto wwn_prefix_err;

	return 0;

wwn_prefix_err:
	hw_err(hw, "eeprom read at offset %d failed\n", offset);
	return 0;
}

/**
 *  ixgbe_set_mac_anti_spoofing - Enable/Disable MAC anti-spoofing
 *  @hw: pointer to hardware structure
 *  @enable: enable or disable switch for MAC anti-spoofing
 *  @vf: Virtual Function pool - VF Pool to set for MAC anti-spoofing
 *
 **/
void ixgbe_set_mac_anti_spoofing(struct ixgbe_hw *hw, bool enable, int vf)
{
	int vf_target_reg = vf >> 3;
	int vf_target_shift = vf % 8;
	u32 pfvfspoof;

	if (hw->mac.type == ixgbe_mac_82598EB)
		return;

	pfvfspoof = IXGBE_READ_REG(hw, IXGBE_PFVFSPOOF(vf_target_reg));
	if (enable)
		pfvfspoof |= BIT(vf_target_shift);
	else
		pfvfspoof &= ~BIT(vf_target_shift);
	IXGBE_WRITE_REG(hw, IXGBE_PFVFSPOOF(vf_target_reg), pfvfspoof);
}

/**
 *  ixgbe_set_vlan_anti_spoofing - Enable/Disable VLAN anti-spoofing
 *  @hw: pointer to hardware structure
 *  @enable: enable or disable switch for VLAN anti-spoofing
 *  @vf: Virtual Function pool - VF Pool to set for VLAN anti-spoofing
 *
 **/
void ixgbe_set_vlan_anti_spoofing(struct ixgbe_hw *hw, bool enable, int vf)
{
	int vf_target_reg = vf >> 3;
	int vf_target_shift = vf % 8 + IXGBE_SPOOF_VLANAS_SHIFT;
	u32 pfvfspoof;

	if (hw->mac.type == ixgbe_mac_82598EB)
		return;

	pfvfspoof = IXGBE_READ_REG(hw, IXGBE_PFVFSPOOF(vf_target_reg));
	if (enable)
		pfvfspoof |= BIT(vf_target_shift);
	else
		pfvfspoof &= ~BIT(vf_target_shift);
	IXGBE_WRITE_REG(hw, IXGBE_PFVFSPOOF(vf_target_reg), pfvfspoof);
}

/**
 *  ixgbe_get_device_caps_generic - Get additional device capabilities
 *  @hw: pointer to hardware structure
 *  @device_caps: the EEPROM word with the extra device capabilities
 *
 *  This function will read the EEPROM location for the device capabilities,
 *  and return the word through device_caps.
 **/
s32 ixgbe_get_device_caps_generic(struct ixgbe_hw *hw, u16 *device_caps)
{
	hw->eeprom.ops.read(hw, IXGBE_DEVICE_CAPS, device_caps);

	return 0;
}

/**
 * ixgbe_set_rxpba_generic - Initialize RX packet buffer
 * @hw: pointer to hardware structure
 * @num_pb: number of packet buffers to allocate
 * @headroom: reserve n KB of headroom
 * @strategy: packet buffer allocation strategy
 **/
void ixgbe_set_rxpba_generic(struct ixgbe_hw *hw,
			     int num_pb,
			     u32 headroom,
			     int strategy)
{
	u32 pbsize = hw->mac.rx_pb_size;
	int i = 0;
	u32 rxpktsize, txpktsize, txpbthresh;

	/* Reserve headroom */
	pbsize -= headroom;

	if (!num_pb)
		num_pb = 1;

	/* Divide remaining packet buffer space amongst the number
	 * of packet buffers requested using supplied strategy.
	 */
	switch (strategy) {
	case (PBA_STRATEGY_WEIGHTED):
		/* pba_80_48 strategy weight first half of packet buffer with
		 * 5/8 of the packet buffer space.
		 */
		rxpktsize = ((pbsize * 5 * 2) / (num_pb * 8));
		pbsize -= rxpktsize * (num_pb / 2);
		rxpktsize <<= IXGBE_RXPBSIZE_SHIFT;
		for (; i < (num_pb / 2); i++)
			IXGBE_WRITE_REG(hw, IXGBE_RXPBSIZE(i), rxpktsize);
		fallthrough; /* configure remaining packet buffers */
	case (PBA_STRATEGY_EQUAL):
		/* Divide the remaining Rx packet buffer evenly among the TCs */
		rxpktsize = (pbsize / (num_pb - i)) << IXGBE_RXPBSIZE_SHIFT;
		for (; i < num_pb; i++)
			IXGBE_WRITE_REG(hw, IXGBE_RXPBSIZE(i), rxpktsize);
		break;
	default:
		break;
	}

	/*
	 * Setup Tx packet buffer and threshold equally for all TCs
	 * TXPBTHRESH register is set in K so divide by 1024 and subtract
	 * 10 since the largest packet we support is just over 9K.
	 */
	txpktsize = IXGBE_TXPBSIZE_MAX / num_pb;
	txpbthresh = (txpktsize / 1024) - IXGBE_TXPKT_SIZE_MAX;
	for (i = 0; i < num_pb; i++) {
		IXGBE_WRITE_REG(hw, IXGBE_TXPBSIZE(i), txpktsize);
		IXGBE_WRITE_REG(hw, IXGBE_TXPBTHRESH(i), txpbthresh);
	}

	/* Clear unused TCs, if any, to zero buffer size*/
	for (; i < IXGBE_MAX_PB; i++) {
		IXGBE_WRITE_REG(hw, IXGBE_RXPBSIZE(i), 0);
		IXGBE_WRITE_REG(hw, IXGBE_TXPBSIZE(i), 0);
		IXGBE_WRITE_REG(hw, IXGBE_TXPBTHRESH(i), 0);
	}
}

/**
 *  ixgbe_calculate_checksum - Calculate checksum for buffer
 *  @buffer: pointer to EEPROM
 *  @length: size of EEPROM to calculate a checksum for
 *
 *  Calculates the checksum for some buffer on a specified length.  The
 *  checksum calculated is returned.
 **/
u8 ixgbe_calculate_checksum(u8 *buffer, u32 length)
{
	u32 i;
	u8 sum = 0;

	if (!buffer)
		return 0;

	for (i = 0; i < length; i++)
		sum += buffer[i];

	return (u8) (0 - sum);
}

/**
 *  ixgbe_hic_unlocked - Issue command to manageability block unlocked
 *  @hw: pointer to the HW structure
 *  @buffer: command to write and where the return status will be placed
 *  @length: length of buffer, must be multiple of 4 bytes
 *  @timeout: time in ms to wait for command completion
 *
 *  Communicates with the manageability block. On success return 0
 *  else returns semaphore error when encountering an error acquiring
 *  semaphore or IXGBE_ERR_HOST_INTERFACE_COMMAND when command fails.
 *
 *  This function assumes that the IXGBE_GSSR_SW_MNG_SM semaphore is held
 *  by the caller.
 **/
s32 ixgbe_hic_unlocked(struct ixgbe_hw *hw, u32 *buffer, u32 length,
		       u32 timeout)
{
	u32 hicr, i, fwsts;
	u16 dword_len;

	if (!length || length > IXGBE_HI_MAX_BLOCK_BYTE_LENGTH) {
		hw_dbg(hw, "Buffer length failure buffersize-%d.\n", length);
		return IXGBE_ERR_HOST_INTERFACE_COMMAND;
	}

	/* Set bit 9 of FWSTS clearing FW reset indication */
	fwsts = IXGBE_READ_REG(hw, IXGBE_FWSTS);
	IXGBE_WRITE_REG(hw, IXGBE_FWSTS, fwsts | IXGBE_FWSTS_FWRI);

	/* Check that the host interface is enabled. */
	hicr = IXGBE_READ_REG(hw, IXGBE_HICR);
	if (!(hicr & IXGBE_HICR_EN)) {
		hw_dbg(hw, "IXGBE_HOST_EN bit disabled.\n");
		return IXGBE_ERR_HOST_INTERFACE_COMMAND;
	}

	/* Calculate length in DWORDs. We must be DWORD aligned */
	if (length % sizeof(u32)) {
		hw_dbg(hw, "Buffer length failure, not aligned to dword");
		return IXGBE_ERR_INVALID_ARGUMENT;
	}

	dword_len = length >> 2;

	/* The device driver writes the relevant command block
	 * into the ram area.
	 */
	for (i = 0; i < dword_len; i++)
		IXGBE_WRITE_REG_ARRAY(hw, IXGBE_FLEX_MNG,
				      i, (__force u32)cpu_to_le32(buffer[i]));

	/* Setting this bit tells the ARC that a new command is pending. */
	IXGBE_WRITE_REG(hw, IXGBE_HICR, hicr | IXGBE_HICR_C);

	for (i = 0; i < timeout; i++) {
		hicr = IXGBE_READ_REG(hw, IXGBE_HICR);
		if (!(hicr & IXGBE_HICR_C))
			break;
		usleep_range(1000, 2000);
	}

	/* Check command successful completion. */
	if ((timeout && i == timeout) ||
	    !(IXGBE_READ_REG(hw, IXGBE_HICR) & IXGBE_HICR_SV))
		return IXGBE_ERR_HOST_INTERFACE_COMMAND;

	return 0;
}

/**
 *  ixgbe_host_interface_command - Issue command to manageability block
 *  @hw: pointer to the HW structure
 *  @buffer: contains the command to write and where the return status will
 *           be placed
 *  @length: length of buffer, must be multiple of 4 bytes
 *  @timeout: time in ms to wait for command completion
 *  @return_data: read and return data from the buffer (true) or not (false)
 *  Needed because FW structures are big endian and decoding of
 *  these fields can be 8 bit or 16 bit based on command. Decoding
 *  is not easily understood without making a table of commands.
 *  So we will leave this up to the caller to read back the data
 *  in these cases.
 *
 *  Communicates with the manageability block.  On success return 0
 *  else return IXGBE_ERR_HOST_INTERFACE_COMMAND.
 **/
s32 ixgbe_host_interface_command(struct ixgbe_hw *hw, void *buffer,
				 u32 length, u32 timeout,
				 bool return_data)
{
	u32 hdr_size = sizeof(struct ixgbe_hic_hdr);
	union {
		struct ixgbe_hic_hdr hdr;
		u32 u32arr[1];
	} *bp = buffer;
	u16 buf_len, dword_len;
	s32 status;
	u32 bi;

	if (!length || length > IXGBE_HI_MAX_BLOCK_BYTE_LENGTH) {
		hw_dbg(hw, "Buffer length failure buffersize-%d.\n", length);
		return IXGBE_ERR_HOST_INTERFACE_COMMAND;
	}
	/* Take management host interface semaphore */
	status = hw->mac.ops.acquire_swfw_sync(hw, IXGBE_GSSR_SW_MNG_SM);
	if (status)
		return status;

	status = ixgbe_hic_unlocked(hw, buffer, length, timeout);
	if (status)
		goto rel_out;

	if (!return_data)
		goto rel_out;

	/* Calculate length in DWORDs */
	dword_len = hdr_size >> 2;

	/* first pull in the header so we know the buffer length */
	for (bi = 0; bi < dword_len; bi++) {
		bp->u32arr[bi] = IXGBE_READ_REG_ARRAY(hw, IXGBE_FLEX_MNG, bi);
		le32_to_cpus(&bp->u32arr[bi]);
	}

	/* If there is any thing in data position pull it in */
	buf_len = bp->hdr.buf_len;
	if (!buf_len)
		goto rel_out;

	if (length < round_up(buf_len, 4) + hdr_size) {
		hw_dbg(hw, "Buffer not large enough for reply message.\n");
		status = IXGBE_ERR_HOST_INTERFACE_COMMAND;
		goto rel_out;
	}

	/* Calculate length in DWORDs, add 3 for odd lengths */
	dword_len = (buf_len + 3) >> 2;

	/* Pull in the rest of the buffer (bi is where we left off) */
	for (; bi <= dword_len; bi++) {
		bp->u32arr[bi] = IXGBE_READ_REG_ARRAY(hw, IXGBE_FLEX_MNG, bi);
		le32_to_cpus(&bp->u32arr[bi]);
	}

rel_out:
	hw->mac.ops.release_swfw_sync(hw, IXGBE_GSSR_SW_MNG_SM);

	return status;
}

/**
 *  ixgbe_set_fw_drv_ver_generic - Sends driver version to firmware
 *  @hw: pointer to the HW structure
 *  @maj: driver version major number
 *  @min: driver version minor number
 *  @build: driver version build number
 *  @sub: driver version sub build number
 *  @len: length of driver_ver string
 *  @driver_ver: driver string
 *
 *  Sends driver version number to firmware through the manageability
 *  block.  On success return 0
 *  else returns IXGBE_ERR_SWFW_SYNC when encountering an error acquiring
 *  semaphore or IXGBE_ERR_HOST_INTERFACE_COMMAND when command fails.
 **/
s32 ixgbe_set_fw_drv_ver_generic(struct ixgbe_hw *hw, u8 maj, u8 min,
				 u8 build, u8 sub, __always_unused u16 len,
				 __always_unused const char *driver_ver)
{
	struct ixgbe_hic_drv_info fw_cmd;
	int i;
	s32 ret_val;

	fw_cmd.hdr.cmd = FW_CEM_CMD_DRIVER_INFO;
	fw_cmd.hdr.buf_len = FW_CEM_CMD_DRIVER_INFO_LEN;
	fw_cmd.hdr.cmd_or_resp.cmd_resv = FW_CEM_CMD_RESERVED;
	fw_cmd.port_num = hw->bus.func;
	fw_cmd.ver_maj = maj;
	fw_cmd.ver_min = min;
	fw_cmd.ver_build = build;
	fw_cmd.ver_sub = sub;
	fw_cmd.hdr.checksum = 0;
	fw_cmd.pad = 0;
	fw_cmd.pad2 = 0;
	fw_cmd.hdr.checksum = ixgbe_calculate_checksum((u8 *)&fw_cmd,
				(FW_CEM_HDR_LEN + fw_cmd.hdr.buf_len));

	for (i = 0; i <= FW_CEM_MAX_RETRIES; i++) {
		ret_val = ixgbe_host_interface_command(hw, &fw_cmd,
						       sizeof(fw_cmd),
						       IXGBE_HI_COMMAND_TIMEOUT,
						       true);
		if (ret_val != 0)
			continue;

		if (fw_cmd.hdr.cmd_or_resp.ret_status ==
		    FW_CEM_RESP_STATUS_SUCCESS)
			ret_val = 0;
		else
			ret_val = IXGBE_ERR_HOST_INTERFACE_COMMAND;

		break;
	}

	return ret_val;
}

/**
 * ixgbe_clear_tx_pending - Clear pending TX work from the PCIe fifo
 * @hw: pointer to the hardware structure
 *
 * The 82599 and x540 MACs can experience issues if TX work is still pending
 * when a reset occurs.  This function prevents this by flushing the PCIe
 * buffers on the system.
 **/
void ixgbe_clear_tx_pending(struct ixgbe_hw *hw)
{
	u32 gcr_ext, hlreg0, i, poll;
	u16 value;

	/*
	 * If double reset is not requested then all transactions should
	 * already be clear and as such there is no work to do
	 */
	if (!(hw->mac.flags & IXGBE_FLAGS_DOUBLE_RESET_REQUIRED))
		return;

	/*
	 * Set loopback enable to prevent any transmits from being sent
	 * should the link come up.  This assumes that the RXCTRL.RXEN bit
	 * has already been cleared.
	 */
	hlreg0 = IXGBE_READ_REG(hw, IXGBE_HLREG0);
	IXGBE_WRITE_REG(hw, IXGBE_HLREG0, hlreg0 | IXGBE_HLREG0_LPBK);

	/* wait for a last completion before clearing buffers */
	IXGBE_WRITE_FLUSH(hw);
	usleep_range(3000, 6000);

	/* Before proceeding, make sure that the PCIe block does not have
	 * transactions pending.
	 */
	poll = ixgbe_pcie_timeout_poll(hw);
	for (i = 0; i < poll; i++) {
		usleep_range(100, 200);
		value = ixgbe_read_pci_cfg_word(hw, IXGBE_PCI_DEVICE_STATUS);
		if (ixgbe_removed(hw->hw_addr))
			break;
		if (!(value & IXGBE_PCI_DEVICE_STATUS_TRANSACTION_PENDING))
			break;
	}

	/* initiate cleaning flow for buffers in the PCIe transaction layer */
	gcr_ext = IXGBE_READ_REG(hw, IXGBE_GCR_EXT);
	IXGBE_WRITE_REG(hw, IXGBE_GCR_EXT,
			gcr_ext | IXGBE_GCR_EXT_BUFFERS_CLEAR);

	/* Flush all writes and allow 20usec for all transactions to clear */
	IXGBE_WRITE_FLUSH(hw);
	udelay(20);

	/* restore previous register values */
	IXGBE_WRITE_REG(hw, IXGBE_GCR_EXT, gcr_ext);
	IXGBE_WRITE_REG(hw, IXGBE_HLREG0, hlreg0);
}

static const u8 ixgbe_emc_temp_data[4] = {
	IXGBE_EMC_INTERNAL_DATA,
	IXGBE_EMC_DIODE1_DATA,
	IXGBE_EMC_DIODE2_DATA,
	IXGBE_EMC_DIODE3_DATA
};
static const u8 ixgbe_emc_therm_limit[4] = {
	IXGBE_EMC_INTERNAL_THERM_LIMIT,
	IXGBE_EMC_DIODE1_THERM_LIMIT,
	IXGBE_EMC_DIODE2_THERM_LIMIT,
	IXGBE_EMC_DIODE3_THERM_LIMIT
};

/**
 *  ixgbe_get_ets_data - Extracts the ETS bit data
 *  @hw: pointer to hardware structure
 *  @ets_cfg: extected ETS data
 *  @ets_offset: offset of ETS data
 *
 *  Returns error code.
 **/
static s32 ixgbe_get_ets_data(struct ixgbe_hw *hw, u16 *ets_cfg,
			      u16 *ets_offset)
{
	s32 status;

	status = hw->eeprom.ops.read(hw, IXGBE_ETS_CFG, ets_offset);
	if (status)
		return status;

	if ((*ets_offset == 0x0000) || (*ets_offset == 0xFFFF))
		return IXGBE_NOT_IMPLEMENTED;

	status = hw->eeprom.ops.read(hw, *ets_offset, ets_cfg);
	if (status)
		return status;

	if ((*ets_cfg & IXGBE_ETS_TYPE_MASK) != IXGBE_ETS_TYPE_EMC_SHIFTED)
		return IXGBE_NOT_IMPLEMENTED;

	return 0;
}

/**
 *  ixgbe_get_thermal_sensor_data_generic - Gathers thermal sensor data
 *  @hw: pointer to hardware structure
 *
 *  Returns the thermal sensor data structure
 **/
s32 ixgbe_get_thermal_sensor_data_generic(struct ixgbe_hw *hw)
{
	s32 status;
	u16 ets_offset;
	u16 ets_cfg;
	u16 ets_sensor;
	u8  num_sensors;
	u8  i;
	struct ixgbe_thermal_sensor_data *data = &hw->mac.thermal_sensor_data;

	/* Only support thermal sensors attached to physical port 0 */
	if ((IXGBE_READ_REG(hw, IXGBE_STATUS) & IXGBE_STATUS_LAN_ID_1))
		return IXGBE_NOT_IMPLEMENTED;

	status = ixgbe_get_ets_data(hw, &ets_cfg, &ets_offset);
	if (status)
		return status;

	num_sensors = (ets_cfg & IXGBE_ETS_NUM_SENSORS_MASK);
	if (num_sensors > IXGBE_MAX_SENSORS)
		num_sensors = IXGBE_MAX_SENSORS;

	for (i = 0; i < num_sensors; i++) {
		u8  sensor_index;
		u8  sensor_location;

		status = hw->eeprom.ops.read(hw, (ets_offset + 1 + i),
					     &ets_sensor);
		if (status)
			return status;

		sensor_index = ((ets_sensor & IXGBE_ETS_DATA_INDEX_MASK) >>
				IXGBE_ETS_DATA_INDEX_SHIFT);
		sensor_location = ((ets_sensor & IXGBE_ETS_DATA_LOC_MASK) >>
				   IXGBE_ETS_DATA_LOC_SHIFT);

		if (sensor_location != 0) {
			status = hw->phy.ops.read_i2c_byte(hw,
					ixgbe_emc_temp_data[sensor_index],
					IXGBE_I2C_THERMAL_SENSOR_ADDR,
					&data->sensor[i].temp);
			if (status)
				return status;
		}
	}

	return 0;
}

/**
 * ixgbe_init_thermal_sensor_thresh_generic - Inits thermal sensor thresholds
 * @hw: pointer to hardware structure
 *
 * Inits the thermal sensor thresholds according to the NVM map
 * and save off the threshold and location values into mac.thermal_sensor_data
 **/
s32 ixgbe_init_thermal_sensor_thresh_generic(struct ixgbe_hw *hw)
{
	s32 status;
	u16 ets_offset;
	u16 ets_cfg;
	u16 ets_sensor;
	u8  low_thresh_delta;
	u8  num_sensors;
	u8  therm_limit;
	u8  i;
	struct ixgbe_thermal_sensor_data *data = &hw->mac.thermal_sensor_data;

	memset(data, 0, sizeof(struct ixgbe_thermal_sensor_data));

	/* Only support thermal sensors attached to physical port 0 */
	if ((IXGBE_READ_REG(hw, IXGBE_STATUS) & IXGBE_STATUS_LAN_ID_1))
		return IXGBE_NOT_IMPLEMENTED;

	status = ixgbe_get_ets_data(hw, &ets_cfg, &ets_offset);
	if (status)
		return status;

	low_thresh_delta = ((ets_cfg & IXGBE_ETS_LTHRES_DELTA_MASK) >>
			     IXGBE_ETS_LTHRES_DELTA_SHIFT);
	num_sensors = (ets_cfg & IXGBE_ETS_NUM_SENSORS_MASK);
	if (num_sensors > IXGBE_MAX_SENSORS)
		num_sensors = IXGBE_MAX_SENSORS;

	for (i = 0; i < num_sensors; i++) {
		u8  sensor_index;
		u8  sensor_location;

		if (hw->eeprom.ops.read(hw, ets_offset + 1 + i, &ets_sensor)) {
			hw_err(hw, "eeprom read at offset %d failed\n",
			       ets_offset + 1 + i);
			continue;
		}
		sensor_index = ((ets_sensor & IXGBE_ETS_DATA_INDEX_MASK) >>
				IXGBE_ETS_DATA_INDEX_SHIFT);
		sensor_location = ((ets_sensor & IXGBE_ETS_DATA_LOC_MASK) >>
				   IXGBE_ETS_DATA_LOC_SHIFT);
		therm_limit = ets_sensor & IXGBE_ETS_DATA_HTHRESH_MASK;

		hw->phy.ops.write_i2c_byte(hw,
			ixgbe_emc_therm_limit[sensor_index],
			IXGBE_I2C_THERMAL_SENSOR_ADDR, therm_limit);

		if (sensor_location == 0)
			continue;

		data->sensor[i].location = sensor_location;
		data->sensor[i].caution_thresh = therm_limit;
		data->sensor[i].max_op_thresh = therm_limit - low_thresh_delta;
	}

	return 0;
}

/**
 *  ixgbe_get_orom_version - Return option ROM from EEPROM
 *
 *  @hw: pointer to hardware structure
 *  @nvm_ver: pointer to output structure
 *
 *  if valid option ROM version, nvm_ver->or_valid set to true
 *  else nvm_ver->or_valid is false.
 **/
void ixgbe_get_orom_version(struct ixgbe_hw *hw,
			    struct ixgbe_nvm_version *nvm_ver)
{
	u16 offset, eeprom_cfg_blkh, eeprom_cfg_blkl;

	nvm_ver->or_valid = false;
	/* Option Rom may or may not be present.  Start with pointer */
	hw->eeprom.ops.read(hw, NVM_OROM_OFFSET, &offset);

	/* make sure offset is valid */
	if (offset == 0x0 || offset == NVM_INVALID_PTR)
		return;

	hw->eeprom.ops.read(hw, offset + NVM_OROM_BLK_HI, &eeprom_cfg_blkh);
	hw->eeprom.ops.read(hw, offset + NVM_OROM_BLK_LOW, &eeprom_cfg_blkl);

	/* option rom exists and is valid */
	if ((eeprom_cfg_blkl | eeprom_cfg_blkh) == 0x0 ||
	    eeprom_cfg_blkl == NVM_VER_INVALID ||
	    eeprom_cfg_blkh == NVM_VER_INVALID)
		return;

	nvm_ver->or_valid = true;
	nvm_ver->or_major = eeprom_cfg_blkl >> NVM_OROM_SHIFT;
	nvm_ver->or_build = (eeprom_cfg_blkl << NVM_OROM_SHIFT) |
			    (eeprom_cfg_blkh >> NVM_OROM_SHIFT);
	nvm_ver->or_patch = eeprom_cfg_blkh & NVM_OROM_PATCH_MASK;
}

/**
 *  ixgbe_get_oem_prod_version - Etrack ID from EEPROM
 *  @hw: pointer to hardware structure
 *  @nvm_ver: pointer to output structure
 *
 *  if valid OEM product version, nvm_ver->oem_valid set to true
 *  else nvm_ver->oem_valid is false.
 **/
void ixgbe_get_oem_prod_version(struct ixgbe_hw *hw,
				struct ixgbe_nvm_version *nvm_ver)
{
	u16 rel_num, prod_ver, mod_len, cap, offset;

	nvm_ver->oem_valid = false;
	hw->eeprom.ops.read(hw, NVM_OEM_PROD_VER_PTR, &offset);

	/* Return is offset to OEM Product Version block is invalid */
	if (offset == 0x0 || offset == NVM_INVALID_PTR)
		return;

	/* Read product version block */
	hw->eeprom.ops.read(hw, offset, &mod_len);
	hw->eeprom.ops.read(hw, offset + NVM_OEM_PROD_VER_CAP_OFF, &cap);

	/* Return if OEM product version block is invalid */
	if (mod_len != NVM_OEM_PROD_VER_MOD_LEN ||
	    (cap & NVM_OEM_PROD_VER_CAP_MASK) != 0x0)
		return;

	hw->eeprom.ops.read(hw, offset + NVM_OEM_PROD_VER_OFF_L, &prod_ver);
	hw->eeprom.ops.read(hw, offset + NVM_OEM_PROD_VER_OFF_H, &rel_num);

	/* Return if version is invalid */
	if ((rel_num | prod_ver) == 0x0 ||
	    rel_num == NVM_VER_INVALID || prod_ver == NVM_VER_INVALID)
		return;

	nvm_ver->oem_major = prod_ver >> NVM_VER_SHIFT;
	nvm_ver->oem_minor = prod_ver & NVM_VER_MASK;
	nvm_ver->oem_release = rel_num;
	nvm_ver->oem_valid = true;
}

/**
 *  ixgbe_get_etk_id - Return Etrack ID from EEPROM
 *
 *  @hw: pointer to hardware structure
 *  @nvm_ver: pointer to output structure
 *
 *  word read errors will return 0xFFFF
 **/
void ixgbe_get_etk_id(struct ixgbe_hw *hw,
		      struct ixgbe_nvm_version *nvm_ver)
{
	u16 etk_id_l, etk_id_h;

	if (hw->eeprom.ops.read(hw, NVM_ETK_OFF_LOW, &etk_id_l))
		etk_id_l = NVM_VER_INVALID;
	if (hw->eeprom.ops.read(hw, NVM_ETK_OFF_HI, &etk_id_h))
		etk_id_h = NVM_VER_INVALID;

	/* The word order for the version format is determined by high order
	 * word bit 15.
	 */
	if ((etk_id_h & NVM_ETK_VALID) == 0) {
		nvm_ver->etk_id = etk_id_h;
		nvm_ver->etk_id |= (etk_id_l << NVM_ETK_SHIFT);
	} else {
		nvm_ver->etk_id = etk_id_l;
		nvm_ver->etk_id |= (etk_id_h << NVM_ETK_SHIFT);
	}
}

void ixgbe_disable_rx_generic(struct ixgbe_hw *hw)
{
	u32 rxctrl;

	rxctrl = IXGBE_READ_REG(hw, IXGBE_RXCTRL);
	if (rxctrl & IXGBE_RXCTRL_RXEN) {
		if (hw->mac.type != ixgbe_mac_82598EB) {
			u32 pfdtxgswc;

			pfdtxgswc = IXGBE_READ_REG(hw, IXGBE_PFDTXGSWC);
			if (pfdtxgswc & IXGBE_PFDTXGSWC_VT_LBEN) {
				pfdtxgswc &= ~IXGBE_PFDTXGSWC_VT_LBEN;
				IXGBE_WRITE_REG(hw, IXGBE_PFDTXGSWC, pfdtxgswc);
				hw->mac.set_lben = true;
			} else {
				hw->mac.set_lben = false;
			}
		}
		rxctrl &= ~IXGBE_RXCTRL_RXEN;
		IXGBE_WRITE_REG(hw, IXGBE_RXCTRL, rxctrl);
	}
}

void ixgbe_enable_rx_generic(struct ixgbe_hw *hw)
{
	u32 rxctrl;

	rxctrl = IXGBE_READ_REG(hw, IXGBE_RXCTRL);
	IXGBE_WRITE_REG(hw, IXGBE_RXCTRL, (rxctrl | IXGBE_RXCTRL_RXEN));

	if (hw->mac.type != ixgbe_mac_82598EB) {
		if (hw->mac.set_lben) {
			u32 pfdtxgswc;

			pfdtxgswc = IXGBE_READ_REG(hw, IXGBE_PFDTXGSWC);
			pfdtxgswc |= IXGBE_PFDTXGSWC_VT_LBEN;
			IXGBE_WRITE_REG(hw, IXGBE_PFDTXGSWC, pfdtxgswc);
			hw->mac.set_lben = false;
		}
	}
}

/** ixgbe_mng_present - returns true when management capability is present
 * @hw: pointer to hardware structure
 **/
bool ixgbe_mng_present(struct ixgbe_hw *hw)
{
	u32 fwsm;

	if (hw->mac.type < ixgbe_mac_82599EB)
		return false;

	fwsm = IXGBE_READ_REG(hw, IXGBE_FWSM(hw));

	return !!(fwsm & IXGBE_FWSM_FW_MODE_PT);
}

/**
 *  ixgbe_setup_mac_link_multispeed_fiber - Set MAC link speed
 *  @hw: pointer to hardware structure
 *  @speed: new link speed
 *  @autoneg_wait_to_complete: true when waiting for completion is needed
 *
 *  Set the link speed in the MAC and/or PHY register and restarts link.
 */
s32 ixgbe_setup_mac_link_multispeed_fiber(struct ixgbe_hw *hw,
					  ixgbe_link_speed speed,
					  bool autoneg_wait_to_complete)
{
	ixgbe_link_speed link_speed = IXGBE_LINK_SPEED_UNKNOWN;
	ixgbe_link_speed highest_link_speed = IXGBE_LINK_SPEED_UNKNOWN;
	s32 status = 0;
	u32 speedcnt = 0;
	u32 i = 0;
	bool autoneg, link_up = false;

	/* Mask off requested but non-supported speeds */
	status = hw->mac.ops.get_link_capabilities(hw, &link_speed, &autoneg);
	if (status)
		return status;

	speed &= link_speed;

	/* Try each speed one by one, highest priority first.  We do this in
	 * software because 10Gb fiber doesn't support speed autonegotiation.
	 */
	if (speed & IXGBE_LINK_SPEED_10GB_FULL) {
		speedcnt++;
		highest_link_speed = IXGBE_LINK_SPEED_10GB_FULL;

		/* Set the module link speed */
		switch (hw->phy.media_type) {
		case ixgbe_media_type_fiber:
			hw->mac.ops.set_rate_select_speed(hw,
						    IXGBE_LINK_SPEED_10GB_FULL);
			break;
		case ixgbe_media_type_fiber_qsfp:
			/* QSFP module automatically detects MAC link speed */
			break;
		default:
			hw_dbg(hw, "Unexpected media type\n");
			break;
		}

		/* Allow module to change analog characteristics (1G->10G) */
		msleep(40);

		status = hw->mac.ops.setup_mac_link(hw,
						    IXGBE_LINK_SPEED_10GB_FULL,
						    autoneg_wait_to_complete);
		if (status)
			return status;

		/* Flap the Tx laser if it has not already been done */
		if (hw->mac.ops.flap_tx_laser)
			hw->mac.ops.flap_tx_laser(hw);

		/* Wait for the controller to acquire link.  Per IEEE 802.3ap,
		 * Section 73.10.2, we may have to wait up to 500ms if KR is
		 * attempted.  82599 uses the same timing for 10g SFI.
		 */
		for (i = 0; i < 5; i++) {
			/* Wait for the link partner to also set speed */
			msleep(100);

			/* If we have link, just jump out */
			status = hw->mac.ops.check_link(hw, &link_speed,
							&link_up, false);
			if (status)
				return status;

			if (link_up)
				goto out;
		}
	}

	if (speed & IXGBE_LINK_SPEED_1GB_FULL) {
		speedcnt++;
		if (highest_link_speed == IXGBE_LINK_SPEED_UNKNOWN)
			highest_link_speed = IXGBE_LINK_SPEED_1GB_FULL;

		/* Set the module link speed */
		switch (hw->phy.media_type) {
		case ixgbe_media_type_fiber:
			hw->mac.ops.set_rate_select_speed(hw,
						     IXGBE_LINK_SPEED_1GB_FULL);
			break;
		case ixgbe_media_type_fiber_qsfp:
			/* QSFP module automatically detects link speed */
			break;
		default:
			hw_dbg(hw, "Unexpected media type\n");
			break;
		}

		/* Allow module to change analog characteristics (10G->1G) */
		msleep(40);

		status = hw->mac.ops.setup_mac_link(hw,
						    IXGBE_LINK_SPEED_1GB_FULL,
						    autoneg_wait_to_complete);
		if (status)
			return status;

		/* Flap the Tx laser if it has not already been done */
		if (hw->mac.ops.flap_tx_laser)
			hw->mac.ops.flap_tx_laser(hw);

		/* Wait for the link partner to also set speed */
		msleep(100);

		/* If we have link, just jump out */
		status = hw->mac.ops.check_link(hw, &link_speed, &link_up,
						false);
		if (status)
			return status;

		if (link_up)
			goto out;
	}

	/* We didn't get link.  Configure back to the highest speed we tried,
	 * (if there was more than one).  We call ourselves back with just the
	 * single highest speed that the user requested.
	 */
	if (speedcnt > 1)
		status = ixgbe_setup_mac_link_multispeed_fiber(hw,
						      highest_link_speed,
						      autoneg_wait_to_complete);

out:
	/* Set autoneg_advertised value based on input link speed */
	hw->phy.autoneg_advertised = 0;

	if (speed & IXGBE_LINK_SPEED_10GB_FULL)
		hw->phy.autoneg_advertised |= IXGBE_LINK_SPEED_10GB_FULL;

	if (speed & IXGBE_LINK_SPEED_1GB_FULL)
		hw->phy.autoneg_advertised |= IXGBE_LINK_SPEED_1GB_FULL;

	return status;
}

/**
 *  ixgbe_set_soft_rate_select_speed - Set module link speed
 *  @hw: pointer to hardware structure
 *  @speed: link speed to set
 *
 *  Set module link speed via the soft rate select.
 */
void ixgbe_set_soft_rate_select_speed(struct ixgbe_hw *hw,
				      ixgbe_link_speed speed)
{
	s32 status;
	u8 rs, eeprom_data;

	switch (speed) {
	case IXGBE_LINK_SPEED_10GB_FULL:
		/* one bit mask same as setting on */
		rs = IXGBE_SFF_SOFT_RS_SELECT_10G;
		break;
	case IXGBE_LINK_SPEED_1GB_FULL:
		rs = IXGBE_SFF_SOFT_RS_SELECT_1G;
		break;
	default:
		hw_dbg(hw, "Invalid fixed module speed\n");
		return;
	}

	/* Set RS0 */
	status = hw->phy.ops.read_i2c_byte(hw, IXGBE_SFF_SFF_8472_OSCB,
					   IXGBE_I2C_EEPROM_DEV_ADDR2,
					   &eeprom_data);
	if (status) {
		hw_dbg(hw, "Failed to read Rx Rate Select RS0\n");
		return;
	}

	eeprom_data = (eeprom_data & ~IXGBE_SFF_SOFT_RS_SELECT_MASK) | rs;

	status = hw->phy.ops.write_i2c_byte(hw, IXGBE_SFF_SFF_8472_OSCB,
					    IXGBE_I2C_EEPROM_DEV_ADDR2,
					    eeprom_data);
	if (status) {
		hw_dbg(hw, "Failed to write Rx Rate Select RS0\n");
		return;
	}

	/* Set RS1 */
	status = hw->phy.ops.read_i2c_byte(hw, IXGBE_SFF_SFF_8472_ESCB,
					   IXGBE_I2C_EEPROM_DEV_ADDR2,
					   &eeprom_data);
	if (status) {
		hw_dbg(hw, "Failed to read Rx Rate Select RS1\n");
		return;
	}

	eeprom_data = (eeprom_data & ~IXGBE_SFF_SOFT_RS_SELECT_MASK) | rs;

	status = hw->phy.ops.write_i2c_byte(hw, IXGBE_SFF_SFF_8472_ESCB,
					    IXGBE_I2C_EEPROM_DEV_ADDR2,
					    eeprom_data);
	if (status) {
		hw_dbg(hw, "Failed to write Rx Rate Select RS1\n");
		return;
	}
}
