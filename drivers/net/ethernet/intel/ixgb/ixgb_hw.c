// SPDX-License-Identifier: GPL-2.0
/* Copyright(c) 1999 - 2008 Intel Corporation. */

/* ixgb_hw.c
 * Shared functions for accessing and configuring the adapter
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/pci_ids.h>
#include "ixgb_hw.h"
#include "ixgb_ids.h"

#include <linux/etherdevice.h>

/*  Local function prototypes */

static u32 ixgb_hash_mc_addr(struct ixgb_hw *hw, u8 * mc_addr);

static void ixgb_mta_set(struct ixgb_hw *hw, u32 hash_value);

static void ixgb_get_bus_info(struct ixgb_hw *hw);

static bool ixgb_link_reset(struct ixgb_hw *hw);

static void ixgb_optics_reset(struct ixgb_hw *hw);

static void ixgb_optics_reset_bcm(struct ixgb_hw *hw);

static ixgb_phy_type ixgb_identify_phy(struct ixgb_hw *hw);

static void ixgb_clear_hw_cntrs(struct ixgb_hw *hw);

static void ixgb_clear_vfta(struct ixgb_hw *hw);

static void ixgb_init_rx_addrs(struct ixgb_hw *hw);

static u16 ixgb_read_phy_reg(struct ixgb_hw *hw,
				  u32 reg_address,
				  u32 phy_address,
				  u32 device_type);

static bool ixgb_setup_fc(struct ixgb_hw *hw);

static bool mac_addr_valid(u8 *mac_addr);

static u32 ixgb_mac_reset(struct ixgb_hw *hw)
{
	u32 ctrl_reg;

	ctrl_reg =  IXGB_CTRL0_RST |
				IXGB_CTRL0_SDP3_DIR |   /* All pins are Output=1 */
				IXGB_CTRL0_SDP2_DIR |
				IXGB_CTRL0_SDP1_DIR |
				IXGB_CTRL0_SDP0_DIR |
				IXGB_CTRL0_SDP3	 |   /* Initial value 1101   */
				IXGB_CTRL0_SDP2	 |
				IXGB_CTRL0_SDP0;

#ifdef HP_ZX1
	/* Workaround for 82597EX reset errata */
	IXGB_WRITE_REG_IO(hw, CTRL0, ctrl_reg);
#else
	IXGB_WRITE_REG(hw, CTRL0, ctrl_reg);
#endif

	/* Delay a few ms just to allow the reset to complete */
	msleep(IXGB_DELAY_AFTER_RESET);
	ctrl_reg = IXGB_READ_REG(hw, CTRL0);
#ifdef DBG
	/* Make sure the self-clearing global reset bit did self clear */
	ASSERT(!(ctrl_reg & IXGB_CTRL0_RST));
#endif

	if (hw->subsystem_vendor_id == PCI_VENDOR_ID_SUN) {
		ctrl_reg =  /* Enable interrupt from XFP and SerDes */
			   IXGB_CTRL1_GPI0_EN |
			   IXGB_CTRL1_SDP6_DIR |
			   IXGB_CTRL1_SDP7_DIR |
			   IXGB_CTRL1_SDP6 |
			   IXGB_CTRL1_SDP7;
		IXGB_WRITE_REG(hw, CTRL1, ctrl_reg);
		ixgb_optics_reset_bcm(hw);
	}

	if (hw->phy_type == ixgb_phy_type_txn17401)
		ixgb_optics_reset(hw);

	return ctrl_reg;
}

/******************************************************************************
 * Reset the transmit and receive units; mask and clear all interrupts.
 *
 * hw - Struct containing variables accessed by shared code
 *****************************************************************************/
bool
ixgb_adapter_stop(struct ixgb_hw *hw)
{
	u32 ctrl_reg;

	ENTER();

	/* If we are stopped or resetting exit gracefully and wait to be
	 * started again before accessing the hardware.
	 */
	if (hw->adapter_stopped) {
		pr_debug("Exiting because the adapter is already stopped!!!\n");
		return false;
	}

	/* Set the Adapter Stopped flag so other driver functions stop
	 * touching the Hardware.
	 */
	hw->adapter_stopped = true;

	/* Clear interrupt mask to stop board from generating interrupts */
	pr_debug("Masking off all interrupts\n");
	IXGB_WRITE_REG(hw, IMC, 0xFFFFFFFF);

	/* Disable the Transmit and Receive units.  Then delay to allow
	 * any pending transactions to complete before we hit the MAC with
	 * the global reset.
	 */
	IXGB_WRITE_REG(hw, RCTL, IXGB_READ_REG(hw, RCTL) & ~IXGB_RCTL_RXEN);
	IXGB_WRITE_REG(hw, TCTL, IXGB_READ_REG(hw, TCTL) & ~IXGB_TCTL_TXEN);
	IXGB_WRITE_FLUSH(hw);
	msleep(IXGB_DELAY_BEFORE_RESET);

	/* Issue a global reset to the MAC.  This will reset the chip's
	 * transmit, receive, DMA, and link units.  It will not effect
	 * the current PCI configuration.  The global reset bit is self-
	 * clearing, and should clear within a microsecond.
	 */
	pr_debug("Issuing a global reset to MAC\n");

	ctrl_reg = ixgb_mac_reset(hw);

	/* Clear interrupt mask to stop board from generating interrupts */
	pr_debug("Masking off all interrupts\n");
	IXGB_WRITE_REG(hw, IMC, 0xffffffff);

	/* Clear any pending interrupt events. */
	IXGB_READ_REG(hw, ICR);

	return ctrl_reg & IXGB_CTRL0_RST;
}


/******************************************************************************
 * Identifies the vendor of the optics module on the adapter.  The SR adapters
 * support two different types of XPAK optics, so it is necessary to determine
 * which optics are present before applying any optics-specific workarounds.
 *
 * hw - Struct containing variables accessed by shared code.
 *
 * Returns: the vendor of the XPAK optics module.
 *****************************************************************************/
static ixgb_xpak_vendor
ixgb_identify_xpak_vendor(struct ixgb_hw *hw)
{
	u32 i;
	u16 vendor_name[5];
	ixgb_xpak_vendor xpak_vendor;

	ENTER();

	/* Read the first few bytes of the vendor string from the XPAK NVR
	 * registers.  These are standard XENPAK/XPAK registers, so all XPAK
	 * devices should implement them. */
	for (i = 0; i < 5; i++) {
		vendor_name[i] = ixgb_read_phy_reg(hw,
						   MDIO_PMA_PMD_XPAK_VENDOR_NAME
						   + i, IXGB_PHY_ADDRESS,
						   MDIO_MMD_PMAPMD);
	}

	/* Determine the actual vendor */
	if (vendor_name[0] == 'I' &&
	    vendor_name[1] == 'N' &&
	    vendor_name[2] == 'T' &&
	    vendor_name[3] == 'E' && vendor_name[4] == 'L') {
		xpak_vendor = ixgb_xpak_vendor_intel;
	} else {
		xpak_vendor = ixgb_xpak_vendor_infineon;
	}

	return xpak_vendor;
}

/******************************************************************************
 * Determine the physical layer module on the adapter.
 *
 * hw - Struct containing variables accessed by shared code.  The device_id
 *      field must be (correctly) populated before calling this routine.
 *
 * Returns: the phy type of the adapter.
 *****************************************************************************/
static ixgb_phy_type
ixgb_identify_phy(struct ixgb_hw *hw)
{
	ixgb_phy_type phy_type;
	ixgb_xpak_vendor xpak_vendor;

	ENTER();

	/* Infer the transceiver/phy type from the device id */
	switch (hw->device_id) {
	case IXGB_DEVICE_ID_82597EX:
		pr_debug("Identified TXN17401 optics\n");
		phy_type = ixgb_phy_type_txn17401;
		break;

	case IXGB_DEVICE_ID_82597EX_SR:
		/* The SR adapters carry two different types of XPAK optics
		 * modules; read the vendor identifier to determine the exact
		 * type of optics. */
		xpak_vendor = ixgb_identify_xpak_vendor(hw);
		if (xpak_vendor == ixgb_xpak_vendor_intel) {
			pr_debug("Identified TXN17201 optics\n");
			phy_type = ixgb_phy_type_txn17201;
		} else {
			pr_debug("Identified G6005 optics\n");
			phy_type = ixgb_phy_type_g6005;
		}
		break;
	case IXGB_DEVICE_ID_82597EX_LR:
		pr_debug("Identified G6104 optics\n");
		phy_type = ixgb_phy_type_g6104;
		break;
	case IXGB_DEVICE_ID_82597EX_CX4:
		pr_debug("Identified CX4\n");
		xpak_vendor = ixgb_identify_xpak_vendor(hw);
		if (xpak_vendor == ixgb_xpak_vendor_intel) {
			pr_debug("Identified TXN17201 optics\n");
			phy_type = ixgb_phy_type_txn17201;
		} else {
			pr_debug("Identified G6005 optics\n");
			phy_type = ixgb_phy_type_g6005;
		}
		break;
	default:
		pr_debug("Unknown physical layer module\n");
		phy_type = ixgb_phy_type_unknown;
		break;
	}

	/* update phy type for sun specific board */
	if (hw->subsystem_vendor_id == PCI_VENDOR_ID_SUN)
		phy_type = ixgb_phy_type_bcm;

	return phy_type;
}

/******************************************************************************
 * Performs basic configuration of the adapter.
 *
 * hw - Struct containing variables accessed by shared code
 *
 * Resets the controller.
 * Reads and validates the EEPROM.
 * Initializes the receive address registers.
 * Initializes the multicast table.
 * Clears all on-chip counters.
 * Calls routine to setup flow control settings.
 * Leaves the transmit and receive units disabled and uninitialized.
 *
 * Returns:
 *      true if successful,
 *      false if unrecoverable problems were encountered.
 *****************************************************************************/
bool
ixgb_init_hw(struct ixgb_hw *hw)
{
	u32 i;
	bool status;

	ENTER();

	/* Issue a global reset to the MAC.  This will reset the chip's
	 * transmit, receive, DMA, and link units.  It will not effect
	 * the current PCI configuration.  The global reset bit is self-
	 * clearing, and should clear within a microsecond.
	 */
	pr_debug("Issuing a global reset to MAC\n");

	ixgb_mac_reset(hw);

	pr_debug("Issuing an EE reset to MAC\n");
#ifdef HP_ZX1
	/* Workaround for 82597EX reset errata */
	IXGB_WRITE_REG_IO(hw, CTRL1, IXGB_CTRL1_EE_RST);
#else
	IXGB_WRITE_REG(hw, CTRL1, IXGB_CTRL1_EE_RST);
#endif

	/* Delay a few ms just to allow the reset to complete */
	msleep(IXGB_DELAY_AFTER_EE_RESET);

	if (!ixgb_get_eeprom_data(hw))
		return false;

	/* Use the device id to determine the type of phy/transceiver. */
	hw->device_id = ixgb_get_ee_device_id(hw);
	hw->phy_type = ixgb_identify_phy(hw);

	/* Setup the receive addresses.
	 * Receive Address Registers (RARs 0 - 15).
	 */
	ixgb_init_rx_addrs(hw);

	/*
	 * Check that a valid MAC address has been set.
	 * If it is not valid, we fail hardware init.
	 */
	if (!mac_addr_valid(hw->curr_mac_addr)) {
		pr_debug("MAC address invalid after ixgb_init_rx_addrs\n");
		return(false);
	}

	/* tell the routines in this file they can access hardware again */
	hw->adapter_stopped = false;

	/* Fill in the bus_info structure */
	ixgb_get_bus_info(hw);

	/* Zero out the Multicast HASH table */
	pr_debug("Zeroing the MTA\n");
	for (i = 0; i < IXGB_MC_TBL_SIZE; i++)
		IXGB_WRITE_REG_ARRAY(hw, MTA, i, 0);

	/* Zero out the VLAN Filter Table Array */
	ixgb_clear_vfta(hw);

	/* Zero all of the hardware counters */
	ixgb_clear_hw_cntrs(hw);

	/* Call a subroutine to setup flow control. */
	status = ixgb_setup_fc(hw);

	/* 82597EX errata: Call check-for-link in case lane deskew is locked */
	ixgb_check_for_link(hw);

	return status;
}

/******************************************************************************
 * Initializes receive address filters.
 *
 * hw - Struct containing variables accessed by shared code
 *
 * Places the MAC address in receive address register 0 and clears the rest
 * of the receive address registers. Clears the multicast table. Assumes
 * the receiver is in reset when the routine is called.
 *****************************************************************************/
static void
ixgb_init_rx_addrs(struct ixgb_hw *hw)
{
	u32 i;

	ENTER();

	/*
	 * If the current mac address is valid, assume it is a software override
	 * to the permanent address.
	 * Otherwise, use the permanent address from the eeprom.
	 */
	if (!mac_addr_valid(hw->curr_mac_addr)) {

		/* Get the MAC address from the eeprom for later reference */
		ixgb_get_ee_mac_addr(hw, hw->curr_mac_addr);

		pr_debug("Keeping Permanent MAC Addr = %pM\n",
			 hw->curr_mac_addr);
	} else {

		/* Setup the receive address. */
		pr_debug("Overriding MAC Address in RAR[0]\n");
		pr_debug("New MAC Addr = %pM\n", hw->curr_mac_addr);

		ixgb_rar_set(hw, hw->curr_mac_addr, 0);
	}

	/* Zero out the other 15 receive addresses. */
	pr_debug("Clearing RAR[1-15]\n");
	for (i = 1; i < IXGB_RAR_ENTRIES; i++) {
		/* Write high reg first to disable the AV bit first */
		IXGB_WRITE_REG_ARRAY(hw, RA, ((i << 1) + 1), 0);
		IXGB_WRITE_REG_ARRAY(hw, RA, (i << 1), 0);
	}
}

/******************************************************************************
 * Updates the MAC's list of multicast addresses.
 *
 * hw - Struct containing variables accessed by shared code
 * mc_addr_list - the list of new multicast addresses
 * mc_addr_count - number of addresses
 * pad - number of bytes between addresses in the list
 *
 * The given list replaces any existing list. Clears the last 15 receive
 * address registers and the multicast table. Uses receive address registers
 * for the first 15 multicast addresses, and hashes the rest into the
 * multicast table.
 *****************************************************************************/
void
ixgb_mc_addr_list_update(struct ixgb_hw *hw,
			  u8 *mc_addr_list,
			  u32 mc_addr_count,
			  u32 pad)
{
	u32 hash_value;
	u32 i;
	u32 rar_used_count = 1;		/* RAR[0] is used for our MAC address */
	u8 *mca;

	ENTER();

	/* Set the new number of MC addresses that we are being requested to use. */
	hw->num_mc_addrs = mc_addr_count;

	/* Clear RAR[1-15] */
	pr_debug("Clearing RAR[1-15]\n");
	for (i = rar_used_count; i < IXGB_RAR_ENTRIES; i++) {
		IXGB_WRITE_REG_ARRAY(hw, RA, (i << 1), 0);
		IXGB_WRITE_REG_ARRAY(hw, RA, ((i << 1) + 1), 0);
	}

	/* Clear the MTA */
	pr_debug("Clearing MTA\n");
	for (i = 0; i < IXGB_MC_TBL_SIZE; i++)
		IXGB_WRITE_REG_ARRAY(hw, MTA, i, 0);

	/* Add the new addresses */
	mca = mc_addr_list;
	for (i = 0; i < mc_addr_count; i++) {
		pr_debug("Adding the multicast addresses:\n");
		pr_debug("MC Addr #%d = %pM\n", i, mca);

		/* Place this multicast address in the RAR if there is room, *
		 * else put it in the MTA
		 */
		if (rar_used_count < IXGB_RAR_ENTRIES) {
			ixgb_rar_set(hw, mca, rar_used_count);
			pr_debug("Added a multicast address to RAR[%d]\n", i);
			rar_used_count++;
		} else {
			hash_value = ixgb_hash_mc_addr(hw, mca);

			pr_debug("Hash value = 0x%03X\n", hash_value);

			ixgb_mta_set(hw, hash_value);
		}

		mca += ETH_ALEN + pad;
	}

	pr_debug("MC Update Complete\n");
}

/******************************************************************************
 * Hashes an address to determine its location in the multicast table
 *
 * hw - Struct containing variables accessed by shared code
 * mc_addr - the multicast address to hash
 *
 * Returns:
 *      The hash value
 *****************************************************************************/
static u32
ixgb_hash_mc_addr(struct ixgb_hw *hw,
		   u8 *mc_addr)
{
	u32 hash_value = 0;

	ENTER();

	/* The portion of the address that is used for the hash table is
	 * determined by the mc_filter_type setting.
	 */
	switch (hw->mc_filter_type) {
		/* [0] [1] [2] [3] [4] [5]
		 * 01  AA  00  12  34  56
		 * LSB                 MSB - According to H/W docs */
	case 0:
		/* [47:36] i.e. 0x563 for above example address */
		hash_value =
		    ((mc_addr[4] >> 4) | (((u16) mc_addr[5]) << 4));
		break;
	case 1:		/* [46:35] i.e. 0xAC6 for above example address */
		hash_value =
		    ((mc_addr[4] >> 3) | (((u16) mc_addr[5]) << 5));
		break;
	case 2:		/* [45:34] i.e. 0x5D8 for above example address */
		hash_value =
		    ((mc_addr[4] >> 2) | (((u16) mc_addr[5]) << 6));
		break;
	case 3:		/* [43:32] i.e. 0x634 for above example address */
		hash_value = ((mc_addr[4]) | (((u16) mc_addr[5]) << 8));
		break;
	default:
		/* Invalid mc_filter_type, what should we do? */
		pr_debug("MC filter type param set incorrectly\n");
		ASSERT(0);
		break;
	}

	hash_value &= 0xFFF;
	return hash_value;
}

/******************************************************************************
 * Sets the bit in the multicast table corresponding to the hash value.
 *
 * hw - Struct containing variables accessed by shared code
 * hash_value - Multicast address hash value
 *****************************************************************************/
static void
ixgb_mta_set(struct ixgb_hw *hw,
		  u32 hash_value)
{
	u32 hash_bit, hash_reg;
	u32 mta_reg;

	/* The MTA is a register array of 128 32-bit registers.
	 * It is treated like an array of 4096 bits.  We want to set
	 * bit BitArray[hash_value]. So we figure out what register
	 * the bit is in, read it, OR in the new bit, then write
	 * back the new value.  The register is determined by the
	 * upper 7 bits of the hash value and the bit within that
	 * register are determined by the lower 5 bits of the value.
	 */
	hash_reg = (hash_value >> 5) & 0x7F;
	hash_bit = hash_value & 0x1F;

	mta_reg = IXGB_READ_REG_ARRAY(hw, MTA, hash_reg);

	mta_reg |= (1 << hash_bit);

	IXGB_WRITE_REG_ARRAY(hw, MTA, hash_reg, mta_reg);
}

/******************************************************************************
 * Puts an ethernet address into a receive address register.
 *
 * hw - Struct containing variables accessed by shared code
 * addr - Address to put into receive address register
 * index - Receive address register to write
 *****************************************************************************/
void
ixgb_rar_set(struct ixgb_hw *hw,
		  u8 *addr,
		  u32 index)
{
	u32 rar_low, rar_high;

	ENTER();

	/* HW expects these in little endian so we reverse the byte order
	 * from network order (big endian) to little endian
	 */
	rar_low = ((u32) addr[0] |
		   ((u32)addr[1] << 8) |
		   ((u32)addr[2] << 16) |
		   ((u32)addr[3] << 24));

	rar_high = ((u32) addr[4] |
			((u32)addr[5] << 8) |
			IXGB_RAH_AV);

	IXGB_WRITE_REG_ARRAY(hw, RA, (index << 1), rar_low);
	IXGB_WRITE_REG_ARRAY(hw, RA, ((index << 1) + 1), rar_high);
}

/******************************************************************************
 * Writes a value to the specified offset in the VLAN filter table.
 *
 * hw - Struct containing variables accessed by shared code
 * offset - Offset in VLAN filer table to write
 * value - Value to write into VLAN filter table
 *****************************************************************************/
void
ixgb_write_vfta(struct ixgb_hw *hw,
		 u32 offset,
		 u32 value)
{
	IXGB_WRITE_REG_ARRAY(hw, VFTA, offset, value);
}

/******************************************************************************
 * Clears the VLAN filer table
 *
 * hw - Struct containing variables accessed by shared code
 *****************************************************************************/
static void
ixgb_clear_vfta(struct ixgb_hw *hw)
{
	u32 offset;

	for (offset = 0; offset < IXGB_VLAN_FILTER_TBL_SIZE; offset++)
		IXGB_WRITE_REG_ARRAY(hw, VFTA, offset, 0);
}

/******************************************************************************
 * Configures the flow control settings based on SW configuration.
 *
 * hw - Struct containing variables accessed by shared code
 *****************************************************************************/

static bool
ixgb_setup_fc(struct ixgb_hw *hw)
{
	u32 ctrl_reg;
	u32 pap_reg = 0;   /* by default, assume no pause time */
	bool status = true;

	ENTER();

	/* Get the current control reg 0 settings */
	ctrl_reg = IXGB_READ_REG(hw, CTRL0);

	/* Clear the Receive Pause Enable and Transmit Pause Enable bits */
	ctrl_reg &= ~(IXGB_CTRL0_RPE | IXGB_CTRL0_TPE);

	/* The possible values of the "flow_control" parameter are:
	 *      0:  Flow control is completely disabled
	 *      1:  Rx flow control is enabled (we can receive pause frames
	 *          but not send pause frames).
	 *      2:  Tx flow control is enabled (we can send pause frames
	 *          but we do not support receiving pause frames).
	 *      3:  Both Rx and TX flow control (symmetric) are enabled.
	 *  other:  Invalid.
	 */
	switch (hw->fc.type) {
	case ixgb_fc_none:	/* 0 */
		/* Set CMDC bit to disable Rx Flow control */
		ctrl_reg |= (IXGB_CTRL0_CMDC);
		break;
	case ixgb_fc_rx_pause:	/* 1 */
		/* RX Flow control is enabled, and TX Flow control is
		 * disabled.
		 */
		ctrl_reg |= (IXGB_CTRL0_RPE);
		break;
	case ixgb_fc_tx_pause:	/* 2 */
		/* TX Flow control is enabled, and RX Flow control is
		 * disabled, by a software over-ride.
		 */
		ctrl_reg |= (IXGB_CTRL0_TPE);
		pap_reg = hw->fc.pause_time;
		break;
	case ixgb_fc_full:	/* 3 */
		/* Flow control (both RX and TX) is enabled by a software
		 * over-ride.
		 */
		ctrl_reg |= (IXGB_CTRL0_RPE | IXGB_CTRL0_TPE);
		pap_reg = hw->fc.pause_time;
		break;
	default:
		/* We should never get here.  The value should be 0-3. */
		pr_debug("Flow control param set incorrectly\n");
		ASSERT(0);
		break;
	}

	/* Write the new settings */
	IXGB_WRITE_REG(hw, CTRL0, ctrl_reg);

	if (pap_reg != 0)
		IXGB_WRITE_REG(hw, PAP, pap_reg);

	/* Set the flow control receive threshold registers.  Normally,
	 * these registers will be set to a default threshold that may be
	 * adjusted later by the driver's runtime code.  However, if the
	 * ability to transmit pause frames in not enabled, then these
	 * registers will be set to 0.
	 */
	if (!(hw->fc.type & ixgb_fc_tx_pause)) {
		IXGB_WRITE_REG(hw, FCRTL, 0);
		IXGB_WRITE_REG(hw, FCRTH, 0);
	} else {
	   /* We need to set up the Receive Threshold high and low water
	    * marks as well as (optionally) enabling the transmission of XON
	    * frames. */
		if (hw->fc.send_xon) {
			IXGB_WRITE_REG(hw, FCRTL,
				(hw->fc.low_water | IXGB_FCRTL_XONE));
		} else {
			IXGB_WRITE_REG(hw, FCRTL, hw->fc.low_water);
		}
		IXGB_WRITE_REG(hw, FCRTH, hw->fc.high_water);
	}
	return status;
}

/******************************************************************************
 * Reads a word from a device over the Management Data Interface (MDI) bus.
 * This interface is used to manage Physical layer devices.
 *
 * hw          - Struct containing variables accessed by hw code
 * reg_address - Offset of device register being read.
 * phy_address - Address of device on MDI.
 *
 * Returns:  Data word (16 bits) from MDI device.
 *
 * The 82597EX has support for several MDI access methods.  This routine
 * uses the new protocol MDI Single Command and Address Operation.
 * This requires that first an address cycle command is sent, followed by a
 * read command.
 *****************************************************************************/
static u16
ixgb_read_phy_reg(struct ixgb_hw *hw,
		u32 reg_address,
		u32 phy_address,
		u32 device_type)
{
	u32 i;
	u32 data;
	u32 command = 0;

	ASSERT(reg_address <= IXGB_MAX_PHY_REG_ADDRESS);
	ASSERT(phy_address <= IXGB_MAX_PHY_ADDRESS);
	ASSERT(device_type <= IXGB_MAX_PHY_DEV_TYPE);

	/* Setup and write the address cycle command */
	command = ((reg_address << IXGB_MSCA_NP_ADDR_SHIFT) |
		   (device_type << IXGB_MSCA_DEV_TYPE_SHIFT) |
		   (phy_address << IXGB_MSCA_PHY_ADDR_SHIFT) |
		   (IXGB_MSCA_ADDR_CYCLE | IXGB_MSCA_MDI_COMMAND));

	IXGB_WRITE_REG(hw, MSCA, command);

    /**************************************************************
    ** Check every 10 usec to see if the address cycle completed
    ** The COMMAND bit will clear when the operation is complete.
    ** This may take as long as 64 usecs (we'll wait 100 usecs max)
    ** from the CPU Write to the Ready bit assertion.
    **************************************************************/

	for (i = 0; i < 10; i++)
	{
		udelay(10);

		command = IXGB_READ_REG(hw, MSCA);

		if ((command & IXGB_MSCA_MDI_COMMAND) == 0)
			break;
	}

	ASSERT((command & IXGB_MSCA_MDI_COMMAND) == 0);

	/* Address cycle complete, setup and write the read command */
	command = ((reg_address << IXGB_MSCA_NP_ADDR_SHIFT) |
		   (device_type << IXGB_MSCA_DEV_TYPE_SHIFT) |
		   (phy_address << IXGB_MSCA_PHY_ADDR_SHIFT) |
		   (IXGB_MSCA_READ | IXGB_MSCA_MDI_COMMAND));

	IXGB_WRITE_REG(hw, MSCA, command);

    /**************************************************************
    ** Check every 10 usec to see if the read command completed
    ** The COMMAND bit will clear when the operation is complete.
    ** The read may take as long as 64 usecs (we'll wait 100 usecs max)
    ** from the CPU Write to the Ready bit assertion.
    **************************************************************/

	for (i = 0; i < 10; i++)
	{
		udelay(10);

		command = IXGB_READ_REG(hw, MSCA);

		if ((command & IXGB_MSCA_MDI_COMMAND) == 0)
			break;
	}

	ASSERT((command & IXGB_MSCA_MDI_COMMAND) == 0);

	/* Operation is complete, get the data from the MDIO Read/Write Data
	 * register and return.
	 */
	data = IXGB_READ_REG(hw, MSRWD);
	data >>= IXGB_MSRWD_READ_DATA_SHIFT;
	return((u16) data);
}

/******************************************************************************
 * Writes a word to a device over the Management Data Interface (MDI) bus.
 * This interface is used to manage Physical layer devices.
 *
 * hw          - Struct containing variables accessed by hw code
 * reg_address - Offset of device register being read.
 * phy_address - Address of device on MDI.
 * device_type - Also known as the Device ID or DID.
 * data        - 16-bit value to be written
 *
 * Returns:  void.
 *
 * The 82597EX has support for several MDI access methods.  This routine
 * uses the new protocol MDI Single Command and Address Operation.
 * This requires that first an address cycle command is sent, followed by a
 * write command.
 *****************************************************************************/
static void
ixgb_write_phy_reg(struct ixgb_hw *hw,
			u32 reg_address,
			u32 phy_address,
			u32 device_type,
			u16 data)
{
	u32 i;
	u32 command = 0;

	ASSERT(reg_address <= IXGB_MAX_PHY_REG_ADDRESS);
	ASSERT(phy_address <= IXGB_MAX_PHY_ADDRESS);
	ASSERT(device_type <= IXGB_MAX_PHY_DEV_TYPE);

	/* Put the data in the MDIO Read/Write Data register */
	IXGB_WRITE_REG(hw, MSRWD, (u32)data);

	/* Setup and write the address cycle command */
	command = ((reg_address << IXGB_MSCA_NP_ADDR_SHIFT)  |
			   (device_type << IXGB_MSCA_DEV_TYPE_SHIFT) |
			   (phy_address << IXGB_MSCA_PHY_ADDR_SHIFT) |
			   (IXGB_MSCA_ADDR_CYCLE | IXGB_MSCA_MDI_COMMAND));

	IXGB_WRITE_REG(hw, MSCA, command);

	/**************************************************************
	** Check every 10 usec to see if the address cycle completed
	** The COMMAND bit will clear when the operation is complete.
	** This may take as long as 64 usecs (we'll wait 100 usecs max)
	** from the CPU Write to the Ready bit assertion.
	**************************************************************/

	for (i = 0; i < 10; i++)
	{
		udelay(10);

		command = IXGB_READ_REG(hw, MSCA);

		if ((command & IXGB_MSCA_MDI_COMMAND) == 0)
			break;
	}

	ASSERT((command & IXGB_MSCA_MDI_COMMAND) == 0);

	/* Address cycle complete, setup and write the write command */
	command = ((reg_address << IXGB_MSCA_NP_ADDR_SHIFT)  |
			   (device_type << IXGB_MSCA_DEV_TYPE_SHIFT) |
			   (phy_address << IXGB_MSCA_PHY_ADDR_SHIFT) |
			   (IXGB_MSCA_WRITE | IXGB_MSCA_MDI_COMMAND));

	IXGB_WRITE_REG(hw, MSCA, command);

	/**************************************************************
	** Check every 10 usec to see if the read command completed
	** The COMMAND bit will clear when the operation is complete.
	** The write may take as long as 64 usecs (we'll wait 100 usecs max)
	** from the CPU Write to the Ready bit assertion.
	**************************************************************/

	for (i = 0; i < 10; i++)
	{
		udelay(10);

		command = IXGB_READ_REG(hw, MSCA);

		if ((command & IXGB_MSCA_MDI_COMMAND) == 0)
			break;
	}

	ASSERT((command & IXGB_MSCA_MDI_COMMAND) == 0);

	/* Operation is complete, return. */
}

/******************************************************************************
 * Checks to see if the link status of the hardware has changed.
 *
 * hw - Struct containing variables accessed by hw code
 *
 * Called by any function that needs to check the link status of the adapter.
 *****************************************************************************/
void
ixgb_check_for_link(struct ixgb_hw *hw)
{
	u32 status_reg;
	u32 xpcss_reg;

	ENTER();

	xpcss_reg = IXGB_READ_REG(hw, XPCSS);
	status_reg = IXGB_READ_REG(hw, STATUS);

	if ((xpcss_reg & IXGB_XPCSS_ALIGN_STATUS) &&
	    (status_reg & IXGB_STATUS_LU)) {
		hw->link_up = true;
	} else if (!(xpcss_reg & IXGB_XPCSS_ALIGN_STATUS) &&
		   (status_reg & IXGB_STATUS_LU)) {
		pr_debug("XPCSS Not Aligned while Status:LU is set\n");
		hw->link_up = ixgb_link_reset(hw);
	} else {
		/*
		 * 82597EX errata.  Since the lane deskew problem may prevent
		 * link, reset the link before reporting link down.
		 */
		hw->link_up = ixgb_link_reset(hw);
	}
	/*  Anything else for 10 Gig?? */
}

/******************************************************************************
 * Check for a bad link condition that may have occurred.
 * The indication is that the RFC / LFC registers may be incrementing
 * continually.  A full adapter reset is required to recover.
 *
 * hw - Struct containing variables accessed by hw code
 *
 * Called by any function that needs to check the link status of the adapter.
 *****************************************************************************/
bool ixgb_check_for_bad_link(struct ixgb_hw *hw)
{
	u32 newLFC, newRFC;
	bool bad_link_returncode = false;

	if (hw->phy_type == ixgb_phy_type_txn17401) {
		newLFC = IXGB_READ_REG(hw, LFC);
		newRFC = IXGB_READ_REG(hw, RFC);
		if ((hw->lastLFC + 250 < newLFC)
		    || (hw->lastRFC + 250 < newRFC)) {
			pr_debug("BAD LINK! too many LFC/RFC since last check\n");
			bad_link_returncode = true;
		}
		hw->lastLFC = newLFC;
		hw->lastRFC = newRFC;
	}

	return bad_link_returncode;
}

/******************************************************************************
 * Clears all hardware statistics counters.
 *
 * hw - Struct containing variables accessed by shared code
 *****************************************************************************/
static void
ixgb_clear_hw_cntrs(struct ixgb_hw *hw)
{
	ENTER();

	/* if we are stopped or resetting exit gracefully */
	if (hw->adapter_stopped) {
		pr_debug("Exiting because the adapter is stopped!!!\n");
		return;
	}

	IXGB_READ_REG(hw, TPRL);
	IXGB_READ_REG(hw, TPRH);
	IXGB_READ_REG(hw, GPRCL);
	IXGB_READ_REG(hw, GPRCH);
	IXGB_READ_REG(hw, BPRCL);
	IXGB_READ_REG(hw, BPRCH);
	IXGB_READ_REG(hw, MPRCL);
	IXGB_READ_REG(hw, MPRCH);
	IXGB_READ_REG(hw, UPRCL);
	IXGB_READ_REG(hw, UPRCH);
	IXGB_READ_REG(hw, VPRCL);
	IXGB_READ_REG(hw, VPRCH);
	IXGB_READ_REG(hw, JPRCL);
	IXGB_READ_REG(hw, JPRCH);
	IXGB_READ_REG(hw, GORCL);
	IXGB_READ_REG(hw, GORCH);
	IXGB_READ_REG(hw, TORL);
	IXGB_READ_REG(hw, TORH);
	IXGB_READ_REG(hw, RNBC);
	IXGB_READ_REG(hw, RUC);
	IXGB_READ_REG(hw, ROC);
	IXGB_READ_REG(hw, RLEC);
	IXGB_READ_REG(hw, CRCERRS);
	IXGB_READ_REG(hw, ICBC);
	IXGB_READ_REG(hw, ECBC);
	IXGB_READ_REG(hw, MPC);
	IXGB_READ_REG(hw, TPTL);
	IXGB_READ_REG(hw, TPTH);
	IXGB_READ_REG(hw, GPTCL);
	IXGB_READ_REG(hw, GPTCH);
	IXGB_READ_REG(hw, BPTCL);
	IXGB_READ_REG(hw, BPTCH);
	IXGB_READ_REG(hw, MPTCL);
	IXGB_READ_REG(hw, MPTCH);
	IXGB_READ_REG(hw, UPTCL);
	IXGB_READ_REG(hw, UPTCH);
	IXGB_READ_REG(hw, VPTCL);
	IXGB_READ_REG(hw, VPTCH);
	IXGB_READ_REG(hw, JPTCL);
	IXGB_READ_REG(hw, JPTCH);
	IXGB_READ_REG(hw, GOTCL);
	IXGB_READ_REG(hw, GOTCH);
	IXGB_READ_REG(hw, TOTL);
	IXGB_READ_REG(hw, TOTH);
	IXGB_READ_REG(hw, DC);
	IXGB_READ_REG(hw, PLT64C);
	IXGB_READ_REG(hw, TSCTC);
	IXGB_READ_REG(hw, TSCTFC);
	IXGB_READ_REG(hw, IBIC);
	IXGB_READ_REG(hw, RFC);
	IXGB_READ_REG(hw, LFC);
	IXGB_READ_REG(hw, PFRC);
	IXGB_READ_REG(hw, PFTC);
	IXGB_READ_REG(hw, MCFRC);
	IXGB_READ_REG(hw, MCFTC);
	IXGB_READ_REG(hw, XONRXC);
	IXGB_READ_REG(hw, XONTXC);
	IXGB_READ_REG(hw, XOFFRXC);
	IXGB_READ_REG(hw, XOFFTXC);
	IXGB_READ_REG(hw, RJC);
}

/******************************************************************************
 * Turns on the software controllable LED
 *
 * hw - Struct containing variables accessed by shared code
 *****************************************************************************/
void
ixgb_led_on(struct ixgb_hw *hw)
{
	u32 ctrl0_reg = IXGB_READ_REG(hw, CTRL0);

	/* To turn on the LED, clear software-definable pin 0 (SDP0). */
	ctrl0_reg &= ~IXGB_CTRL0_SDP0;
	IXGB_WRITE_REG(hw, CTRL0, ctrl0_reg);
}

/******************************************************************************
 * Turns off the software controllable LED
 *
 * hw - Struct containing variables accessed by shared code
 *****************************************************************************/
void
ixgb_led_off(struct ixgb_hw *hw)
{
	u32 ctrl0_reg = IXGB_READ_REG(hw, CTRL0);

	/* To turn off the LED, set software-definable pin 0 (SDP0). */
	ctrl0_reg |= IXGB_CTRL0_SDP0;
	IXGB_WRITE_REG(hw, CTRL0, ctrl0_reg);
}

/******************************************************************************
 * Gets the current PCI bus type, speed, and width of the hardware
 *
 * hw - Struct containing variables accessed by shared code
 *****************************************************************************/
static void
ixgb_get_bus_info(struct ixgb_hw *hw)
{
	u32 status_reg;

	status_reg = IXGB_READ_REG(hw, STATUS);

	hw->bus.type = (status_reg & IXGB_STATUS_PCIX_MODE) ?
		ixgb_bus_type_pcix : ixgb_bus_type_pci;

	if (hw->bus.type == ixgb_bus_type_pci) {
		hw->bus.speed = (status_reg & IXGB_STATUS_PCI_SPD) ?
			ixgb_bus_speed_66 : ixgb_bus_speed_33;
	} else {
		switch (status_reg & IXGB_STATUS_PCIX_SPD_MASK) {
		case IXGB_STATUS_PCIX_SPD_66:
			hw->bus.speed = ixgb_bus_speed_66;
			break;
		case IXGB_STATUS_PCIX_SPD_100:
			hw->bus.speed = ixgb_bus_speed_100;
			break;
		case IXGB_STATUS_PCIX_SPD_133:
			hw->bus.speed = ixgb_bus_speed_133;
			break;
		default:
			hw->bus.speed = ixgb_bus_speed_reserved;
			break;
		}
	}

	hw->bus.width = (status_reg & IXGB_STATUS_BUS64) ?
		ixgb_bus_width_64 : ixgb_bus_width_32;
}

/******************************************************************************
 * Tests a MAC address to ensure it is a valid Individual Address
 *
 * mac_addr - pointer to MAC address.
 *
 *****************************************************************************/
static bool
mac_addr_valid(u8 *mac_addr)
{
	bool is_valid = true;
	ENTER();

	/* Make sure it is not a multicast address */
	if (is_multicast_ether_addr(mac_addr)) {
		pr_debug("MAC address is multicast\n");
		is_valid = false;
	}
	/* Not a broadcast address */
	else if (is_broadcast_ether_addr(mac_addr)) {
		pr_debug("MAC address is broadcast\n");
		is_valid = false;
	}
	/* Reject the zero address */
	else if (is_zero_ether_addr(mac_addr)) {
		pr_debug("MAC address is all zeros\n");
		is_valid = false;
	}
	return is_valid;
}

/******************************************************************************
 * Resets the 10GbE link.  Waits the settle time and returns the state of
 * the link.
 *
 * hw - Struct containing variables accessed by shared code
 *****************************************************************************/
static bool
ixgb_link_reset(struct ixgb_hw *hw)
{
	bool link_status = false;
	u8 wait_retries = MAX_RESET_ITERATIONS;
	u8 lrst_retries = MAX_RESET_ITERATIONS;

	do {
		/* Reset the link */
		IXGB_WRITE_REG(hw, CTRL0,
			       IXGB_READ_REG(hw, CTRL0) | IXGB_CTRL0_LRST);

		/* Wait for link-up and lane re-alignment */
		do {
			udelay(IXGB_DELAY_USECS_AFTER_LINK_RESET);
			link_status =
			    ((IXGB_READ_REG(hw, STATUS) & IXGB_STATUS_LU)
			     && (IXGB_READ_REG(hw, XPCSS) &
				 IXGB_XPCSS_ALIGN_STATUS)) ? true : false;
		} while (!link_status && --wait_retries);

	} while (!link_status && --lrst_retries);

	return link_status;
}

/******************************************************************************
 * Resets the 10GbE optics module.
 *
 * hw - Struct containing variables accessed by shared code
 *****************************************************************************/
static void
ixgb_optics_reset(struct ixgb_hw *hw)
{
	if (hw->phy_type == ixgb_phy_type_txn17401) {
		ixgb_write_phy_reg(hw,
				   MDIO_CTRL1,
				   IXGB_PHY_ADDRESS,
				   MDIO_MMD_PMAPMD,
				   MDIO_CTRL1_RESET);

		ixgb_read_phy_reg(hw, MDIO_CTRL1, IXGB_PHY_ADDRESS, MDIO_MMD_PMAPMD);
	}
}

/******************************************************************************
 * Resets the 10GbE optics module for Sun variant NIC.
 *
 * hw - Struct containing variables accessed by shared code
 *****************************************************************************/

#define   IXGB_BCM8704_USER_PMD_TX_CTRL_REG         0xC803
#define   IXGB_BCM8704_USER_PMD_TX_CTRL_REG_VAL     0x0164
#define   IXGB_BCM8704_USER_CTRL_REG                0xC800
#define   IXGB_BCM8704_USER_CTRL_REG_VAL            0x7FBF
#define   IXGB_BCM8704_USER_DEV3_ADDR               0x0003
#define   IXGB_SUN_PHY_ADDRESS                      0x0000
#define   IXGB_SUN_PHY_RESET_DELAY                     305

static void
ixgb_optics_reset_bcm(struct ixgb_hw *hw)
{
	u32 ctrl = IXGB_READ_REG(hw, CTRL0);
	ctrl &= ~IXGB_CTRL0_SDP2;
	ctrl |= IXGB_CTRL0_SDP3;
	IXGB_WRITE_REG(hw, CTRL0, ctrl);
	IXGB_WRITE_FLUSH(hw);

	/* SerDes needs extra delay */
	msleep(IXGB_SUN_PHY_RESET_DELAY);

	/* Broadcom 7408L configuration */
	/* Reference clock config */
	ixgb_write_phy_reg(hw,
			   IXGB_BCM8704_USER_PMD_TX_CTRL_REG,
			   IXGB_SUN_PHY_ADDRESS,
			   IXGB_BCM8704_USER_DEV3_ADDR,
			   IXGB_BCM8704_USER_PMD_TX_CTRL_REG_VAL);
	/*  we must read the registers twice */
	ixgb_read_phy_reg(hw,
			  IXGB_BCM8704_USER_PMD_TX_CTRL_REG,
			  IXGB_SUN_PHY_ADDRESS,
			  IXGB_BCM8704_USER_DEV3_ADDR);
	ixgb_read_phy_reg(hw,
			  IXGB_BCM8704_USER_PMD_TX_CTRL_REG,
			  IXGB_SUN_PHY_ADDRESS,
			  IXGB_BCM8704_USER_DEV3_ADDR);

	ixgb_write_phy_reg(hw,
			   IXGB_BCM8704_USER_CTRL_REG,
			   IXGB_SUN_PHY_ADDRESS,
			   IXGB_BCM8704_USER_DEV3_ADDR,
			   IXGB_BCM8704_USER_CTRL_REG_VAL);
	ixgb_read_phy_reg(hw,
			  IXGB_BCM8704_USER_CTRL_REG,
			  IXGB_SUN_PHY_ADDRESS,
			  IXGB_BCM8704_USER_DEV3_ADDR);
	ixgb_read_phy_reg(hw,
			  IXGB_BCM8704_USER_CTRL_REG,
			  IXGB_SUN_PHY_ADDRESS,
			  IXGB_BCM8704_USER_DEV3_ADDR);

	/* SerDes needs extra delay */
	msleep(IXGB_SUN_PHY_RESET_DELAY);
}
