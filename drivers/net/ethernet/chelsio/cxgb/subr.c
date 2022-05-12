/*****************************************************************************
 *                                                                           *
 * File: subr.c                                                              *
 * $Revision: 1.27 $                                                         *
 * $Date: 2005/06/22 01:08:36 $                                              *
 * Description:                                                              *
 *  Various subroutines (intr,pio,etc.) used by Chelsio 10G Ethernet driver. *
 *  part of the Chelsio 10Gb Ethernet Driver.                                *
 *                                                                           *
 * This program is free software; you can redistribute it and/or modify      *
 * it under the terms of the GNU General Public License, version 2, as       *
 * published by the Free Software Foundation.                                *
 *                                                                           *
 * You should have received a copy of the GNU General Public License along   *
 * with this program; if not, see <http://www.gnu.org/licenses/>.            *
 *                                                                           *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED    *
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF      *
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.                     *
 *                                                                           *
 * http://www.chelsio.com                                                    *
 *                                                                           *
 * Copyright (c) 2003 - 2005 Chelsio Communications, Inc.                    *
 * All rights reserved.                                                      *
 *                                                                           *
 * Maintainers: maintainers@chelsio.com                                      *
 *                                                                           *
 * Authors: Dimitrios Michailidis   <dm@chelsio.com>                         *
 *          Tina Yang               <tainay@chelsio.com>                     *
 *          Felix Marti             <felix@chelsio.com>                      *
 *          Scott Bardone           <sbardone@chelsio.com>                   *
 *          Kurt Ottaway            <kottaway@chelsio.com>                   *
 *          Frank DiMambro          <frank@chelsio.com>                      *
 *                                                                           *
 * History:                                                                  *
 *                                                                           *
 ****************************************************************************/

#include "common.h"
#include "elmer0.h"
#include "regs.h"
#include "gmac.h"
#include "cphy.h"
#include "sge.h"
#include "tp.h"
#include "espi.h"

/**
 *	t1_wait_op_done - wait until an operation is completed
 *	@adapter: the adapter performing the operation
 *	@reg: the register to check for completion
 *	@mask: a single-bit field within @reg that indicates completion
 *	@polarity: the value of the field when the operation is completed
 *	@attempts: number of check iterations
 *      @delay: delay in usecs between iterations
 *
 *	Wait until an operation is completed by checking a bit in a register
 *	up to @attempts times.  Returns %0 if the operation completes and %1
 *	otherwise.
 */
static int t1_wait_op_done(adapter_t *adapter, int reg, u32 mask, int polarity,
			   int attempts, int delay)
{
	while (1) {
		u32 val = readl(adapter->regs + reg) & mask;

		if (!!val == polarity)
			return 0;
		if (--attempts == 0)
			return 1;
		if (delay)
			udelay(delay);
	}
}

#define TPI_ATTEMPTS 50

/*
 * Write a register over the TPI interface (unlocked and locked versions).
 */
int __t1_tpi_write(adapter_t *adapter, u32 addr, u32 value)
{
	int tpi_busy;

	writel(addr, adapter->regs + A_TPI_ADDR);
	writel(value, adapter->regs + A_TPI_WR_DATA);
	writel(F_TPIWR, adapter->regs + A_TPI_CSR);

	tpi_busy = t1_wait_op_done(adapter, A_TPI_CSR, F_TPIRDY, 1,
				   TPI_ATTEMPTS, 3);
	if (tpi_busy)
		pr_alert("%s: TPI write to 0x%x failed\n",
			 adapter->name, addr);
	return tpi_busy;
}

int t1_tpi_write(adapter_t *adapter, u32 addr, u32 value)
{
	int ret;

	spin_lock(&adapter->tpi_lock);
	ret = __t1_tpi_write(adapter, addr, value);
	spin_unlock(&adapter->tpi_lock);
	return ret;
}

/*
 * Read a register over the TPI interface (unlocked and locked versions).
 */
int __t1_tpi_read(adapter_t *adapter, u32 addr, u32 *valp)
{
	int tpi_busy;

	writel(addr, adapter->regs + A_TPI_ADDR);
	writel(0, adapter->regs + A_TPI_CSR);

	tpi_busy = t1_wait_op_done(adapter, A_TPI_CSR, F_TPIRDY, 1,
				   TPI_ATTEMPTS, 3);
	if (tpi_busy)
		pr_alert("%s: TPI read from 0x%x failed\n",
			 adapter->name, addr);
	else
		*valp = readl(adapter->regs + A_TPI_RD_DATA);
	return tpi_busy;
}

int t1_tpi_read(adapter_t *adapter, u32 addr, u32 *valp)
{
	int ret;

	spin_lock(&adapter->tpi_lock);
	ret = __t1_tpi_read(adapter, addr, valp);
	spin_unlock(&adapter->tpi_lock);
	return ret;
}

/*
 * Set a TPI parameter.
 */
static void t1_tpi_par(adapter_t *adapter, u32 value)
{
	writel(V_TPIPAR(value), adapter->regs + A_TPI_PAR);
}

/*
 * Called when a port's link settings change to propagate the new values to the
 * associated PHY and MAC.  After performing the common tasks it invokes an
 * OS-specific handler.
 */
void t1_link_changed(adapter_t *adapter, int port_id)
{
	int link_ok, speed, duplex, fc;
	struct cphy *phy = adapter->port[port_id].phy;
	struct link_config *lc = &adapter->port[port_id].link_config;

	phy->ops->get_link_status(phy, &link_ok, &speed, &duplex, &fc);

	lc->speed = speed < 0 ? SPEED_INVALID : speed;
	lc->duplex = duplex < 0 ? DUPLEX_INVALID : duplex;
	if (!(lc->requested_fc & PAUSE_AUTONEG))
		fc = lc->requested_fc & (PAUSE_RX | PAUSE_TX);

	if (link_ok && speed >= 0 && lc->autoneg == AUTONEG_ENABLE) {
		/* Set MAC speed, duplex, and flow control to match PHY. */
		struct cmac *mac = adapter->port[port_id].mac;

		mac->ops->set_speed_duplex_fc(mac, speed, duplex, fc);
		lc->fc = (unsigned char)fc;
	}
	t1_link_negotiated(adapter, port_id, link_ok, speed, duplex, fc);
}

static bool t1_pci_intr_handler(adapter_t *adapter)
{
	u32 pcix_cause;

	pci_read_config_dword(adapter->pdev, A_PCICFG_INTR_CAUSE, &pcix_cause);

	if (pcix_cause) {
		pci_write_config_dword(adapter->pdev, A_PCICFG_INTR_CAUSE,
				       pcix_cause);
		/* PCI errors are fatal */
		t1_interrupts_disable(adapter);
		adapter->pending_thread_intr |= F_PL_INTR_SGE_ERR;
		pr_alert("%s: PCI error encountered.\n", adapter->name);
		return true;
	}
	return false;
}

#ifdef CONFIG_CHELSIO_T1_1G
#include "fpga_defs.h"

/*
 * PHY interrupt handler for FPGA boards.
 */
static int fpga_phy_intr_handler(adapter_t *adapter)
{
	int p;
	u32 cause = readl(adapter->regs + FPGA_GMAC_ADDR_INTERRUPT_CAUSE);

	for_each_port(adapter, p)
		if (cause & (1 << p)) {
			struct cphy *phy = adapter->port[p].phy;
			int phy_cause = phy->ops->interrupt_handler(phy);

			if (phy_cause & cphy_cause_link_change)
				t1_link_changed(adapter, p);
		}
	writel(cause, adapter->regs + FPGA_GMAC_ADDR_INTERRUPT_CAUSE);
	return 0;
}

/*
 * Slow path interrupt handler for FPGAs.
 */
static irqreturn_t fpga_slow_intr(adapter_t *adapter)
{
	u32 cause = readl(adapter->regs + A_PL_CAUSE);
	irqreturn_t ret = IRQ_NONE;

	cause &= ~F_PL_INTR_SGE_DATA;
	if (cause & F_PL_INTR_SGE_ERR) {
		if (t1_sge_intr_error_handler(adapter->sge))
			ret = IRQ_WAKE_THREAD;
	}

	if (cause & FPGA_PCIX_INTERRUPT_GMAC)
		fpga_phy_intr_handler(adapter);

	if (cause & FPGA_PCIX_INTERRUPT_TP) {
		/*
		 * FPGA doesn't support MC4 interrupts and it requires
		 * this odd layer of indirection for MC5.
		 */
		u32 tp_cause = readl(adapter->regs + FPGA_TP_ADDR_INTERRUPT_CAUSE);

		/* Clear TP interrupt */
		writel(tp_cause, adapter->regs + FPGA_TP_ADDR_INTERRUPT_CAUSE);
	}
	if (cause & FPGA_PCIX_INTERRUPT_PCIX) {
		if (t1_pci_intr_handler(adapter))
			ret = IRQ_WAKE_THREAD;
	}

	/* Clear the interrupts just processed. */
	if (cause)
		writel(cause, adapter->regs + A_PL_CAUSE);

	if (ret != IRQ_NONE)
		return ret;

	return cause == 0 ? IRQ_NONE : IRQ_HANDLED;
}
#endif

/*
 * Wait until Elmer's MI1 interface is ready for new operations.
 */
static int mi1_wait_until_ready(adapter_t *adapter, int mi1_reg)
{
	int attempts = 100, busy;

	do {
		u32 val;

		__t1_tpi_read(adapter, mi1_reg, &val);
		busy = val & F_MI1_OP_BUSY;
		if (busy)
			udelay(10);
	} while (busy && --attempts);
	if (busy)
		pr_alert("%s: MDIO operation timed out\n", adapter->name);
	return busy;
}

/*
 * MI1 MDIO initialization.
 */
static void mi1_mdio_init(adapter_t *adapter, const struct board_info *bi)
{
	u32 clkdiv = bi->clock_elmer0 / (2 * bi->mdio_mdc) - 1;
	u32 val = F_MI1_PREAMBLE_ENABLE | V_MI1_MDI_INVERT(bi->mdio_mdiinv) |
		V_MI1_MDI_ENABLE(bi->mdio_mdien) | V_MI1_CLK_DIV(clkdiv);

	if (!(bi->caps & SUPPORTED_10000baseT_Full))
		val |= V_MI1_SOF(1);
	t1_tpi_write(adapter, A_ELMER0_PORT0_MI1_CFG, val);
}

#if defined(CONFIG_CHELSIO_T1_1G)
/*
 * Elmer MI1 MDIO read/write operations.
 */
static int mi1_mdio_read(struct net_device *dev, int phy_addr, int mmd_addr,
			 u16 reg_addr)
{
	struct adapter *adapter = dev->ml_priv;
	u32 addr = V_MI1_REG_ADDR(reg_addr) | V_MI1_PHY_ADDR(phy_addr);
	unsigned int val;

	spin_lock(&adapter->tpi_lock);
	__t1_tpi_write(adapter, A_ELMER0_PORT0_MI1_ADDR, addr);
	__t1_tpi_write(adapter,
			A_ELMER0_PORT0_MI1_OP, MI1_OP_DIRECT_READ);
	mi1_wait_until_ready(adapter, A_ELMER0_PORT0_MI1_OP);
	__t1_tpi_read(adapter, A_ELMER0_PORT0_MI1_DATA, &val);
	spin_unlock(&adapter->tpi_lock);
	return val;
}

static int mi1_mdio_write(struct net_device *dev, int phy_addr, int mmd_addr,
			  u16 reg_addr, u16 val)
{
	struct adapter *adapter = dev->ml_priv;
	u32 addr = V_MI1_REG_ADDR(reg_addr) | V_MI1_PHY_ADDR(phy_addr);

	spin_lock(&adapter->tpi_lock);
	__t1_tpi_write(adapter, A_ELMER0_PORT0_MI1_ADDR, addr);
	__t1_tpi_write(adapter, A_ELMER0_PORT0_MI1_DATA, val);
	__t1_tpi_write(adapter,
			A_ELMER0_PORT0_MI1_OP, MI1_OP_DIRECT_WRITE);
	mi1_wait_until_ready(adapter, A_ELMER0_PORT0_MI1_OP);
	spin_unlock(&adapter->tpi_lock);
	return 0;
}

static const struct mdio_ops mi1_mdio_ops = {
	.init = mi1_mdio_init,
	.read = mi1_mdio_read,
	.write = mi1_mdio_write,
	.mode_support = MDIO_SUPPORTS_C22
};

#endif

static int mi1_mdio_ext_read(struct net_device *dev, int phy_addr, int mmd_addr,
			     u16 reg_addr)
{
	struct adapter *adapter = dev->ml_priv;
	u32 addr = V_MI1_REG_ADDR(mmd_addr) | V_MI1_PHY_ADDR(phy_addr);
	unsigned int val;

	spin_lock(&adapter->tpi_lock);

	/* Write the address we want. */
	__t1_tpi_write(adapter, A_ELMER0_PORT0_MI1_ADDR, addr);
	__t1_tpi_write(adapter, A_ELMER0_PORT0_MI1_DATA, reg_addr);
	__t1_tpi_write(adapter, A_ELMER0_PORT0_MI1_OP,
		       MI1_OP_INDIRECT_ADDRESS);
	mi1_wait_until_ready(adapter, A_ELMER0_PORT0_MI1_OP);

	/* Write the operation we want. */
	__t1_tpi_write(adapter,
			A_ELMER0_PORT0_MI1_OP, MI1_OP_INDIRECT_READ);
	mi1_wait_until_ready(adapter, A_ELMER0_PORT0_MI1_OP);

	/* Read the data. */
	__t1_tpi_read(adapter, A_ELMER0_PORT0_MI1_DATA, &val);
	spin_unlock(&adapter->tpi_lock);
	return val;
}

static int mi1_mdio_ext_write(struct net_device *dev, int phy_addr,
			      int mmd_addr, u16 reg_addr, u16 val)
{
	struct adapter *adapter = dev->ml_priv;
	u32 addr = V_MI1_REG_ADDR(mmd_addr) | V_MI1_PHY_ADDR(phy_addr);

	spin_lock(&adapter->tpi_lock);

	/* Write the address we want. */
	__t1_tpi_write(adapter, A_ELMER0_PORT0_MI1_ADDR, addr);
	__t1_tpi_write(adapter, A_ELMER0_PORT0_MI1_DATA, reg_addr);
	__t1_tpi_write(adapter, A_ELMER0_PORT0_MI1_OP,
		       MI1_OP_INDIRECT_ADDRESS);
	mi1_wait_until_ready(adapter, A_ELMER0_PORT0_MI1_OP);

	/* Write the data. */
	__t1_tpi_write(adapter, A_ELMER0_PORT0_MI1_DATA, val);
	__t1_tpi_write(adapter, A_ELMER0_PORT0_MI1_OP, MI1_OP_INDIRECT_WRITE);
	mi1_wait_until_ready(adapter, A_ELMER0_PORT0_MI1_OP);
	spin_unlock(&adapter->tpi_lock);
	return 0;
}

static const struct mdio_ops mi1_mdio_ext_ops = {
	.init = mi1_mdio_init,
	.read = mi1_mdio_ext_read,
	.write = mi1_mdio_ext_write,
	.mode_support = MDIO_SUPPORTS_C45 | MDIO_EMULATE_C22
};

enum {
	CH_BRD_T110_1CU,
	CH_BRD_N110_1F,
	CH_BRD_N210_1F,
	CH_BRD_T210_1F,
	CH_BRD_T210_1CU,
	CH_BRD_N204_4CU,
};

static const struct board_info t1_board[] = {
	{
		.board		= CHBT_BOARD_CHT110,
		.port_number	= 1,
		.caps		= SUPPORTED_10000baseT_Full,
		.chip_term	= CHBT_TERM_T1,
		.chip_mac	= CHBT_MAC_PM3393,
		.chip_phy	= CHBT_PHY_MY3126,
		.clock_core	= 125000000,
		.clock_mc3	= 150000000,
		.clock_mc4	= 125000000,
		.espi_nports	= 1,
		.clock_elmer0	= 44,
		.mdio_mdien	= 1,
		.mdio_mdiinv	= 1,
		.mdio_mdc	= 1,
		.mdio_phybaseaddr = 1,
		.gmac		= &t1_pm3393_ops,
		.gphy		= &t1_my3126_ops,
		.mdio_ops	= &mi1_mdio_ext_ops,
		.desc		= "Chelsio T110 1x10GBase-CX4 TOE",
	},

	{
		.board		= CHBT_BOARD_N110,
		.port_number	= 1,
		.caps		= SUPPORTED_10000baseT_Full | SUPPORTED_FIBRE,
		.chip_term	= CHBT_TERM_T1,
		.chip_mac	= CHBT_MAC_PM3393,
		.chip_phy	= CHBT_PHY_88X2010,
		.clock_core	= 125000000,
		.espi_nports	= 1,
		.clock_elmer0	= 44,
		.mdio_mdien	= 0,
		.mdio_mdiinv	= 0,
		.mdio_mdc	= 1,
		.mdio_phybaseaddr = 0,
		.gmac		= &t1_pm3393_ops,
		.gphy		= &t1_mv88x201x_ops,
		.mdio_ops	= &mi1_mdio_ext_ops,
		.desc		= "Chelsio N110 1x10GBaseX NIC",
	},

	{
		.board		= CHBT_BOARD_N210,
		.port_number	= 1,
		.caps		= SUPPORTED_10000baseT_Full | SUPPORTED_FIBRE,
		.chip_term	= CHBT_TERM_T2,
		.chip_mac	= CHBT_MAC_PM3393,
		.chip_phy	= CHBT_PHY_88X2010,
		.clock_core	= 125000000,
		.espi_nports	= 1,
		.clock_elmer0	= 44,
		.mdio_mdien	= 0,
		.mdio_mdiinv	= 0,
		.mdio_mdc	= 1,
		.mdio_phybaseaddr = 0,
		.gmac		= &t1_pm3393_ops,
		.gphy		= &t1_mv88x201x_ops,
		.mdio_ops	= &mi1_mdio_ext_ops,
		.desc		= "Chelsio N210 1x10GBaseX NIC",
	},

	{
		.board		= CHBT_BOARD_CHT210,
		.port_number	= 1,
		.caps		= SUPPORTED_10000baseT_Full,
		.chip_term	= CHBT_TERM_T2,
		.chip_mac	= CHBT_MAC_PM3393,
		.chip_phy	= CHBT_PHY_88X2010,
		.clock_core	= 125000000,
		.clock_mc3	= 133000000,
		.clock_mc4	= 125000000,
		.espi_nports	= 1,
		.clock_elmer0	= 44,
		.mdio_mdien	= 0,
		.mdio_mdiinv	= 0,
		.mdio_mdc	= 1,
		.mdio_phybaseaddr = 0,
		.gmac		= &t1_pm3393_ops,
		.gphy		= &t1_mv88x201x_ops,
		.mdio_ops	= &mi1_mdio_ext_ops,
		.desc		= "Chelsio T210 1x10GBaseX TOE",
	},

	{
		.board		= CHBT_BOARD_CHT210,
		.port_number	= 1,
		.caps		= SUPPORTED_10000baseT_Full,
		.chip_term	= CHBT_TERM_T2,
		.chip_mac	= CHBT_MAC_PM3393,
		.chip_phy	= CHBT_PHY_MY3126,
		.clock_core	= 125000000,
		.clock_mc3	= 133000000,
		.clock_mc4	= 125000000,
		.espi_nports	= 1,
		.clock_elmer0	= 44,
		.mdio_mdien	= 1,
		.mdio_mdiinv	= 1,
		.mdio_mdc	= 1,
		.mdio_phybaseaddr = 1,
		.gmac		= &t1_pm3393_ops,
		.gphy		= &t1_my3126_ops,
		.mdio_ops	= &mi1_mdio_ext_ops,
		.desc		= "Chelsio T210 1x10GBase-CX4 TOE",
	},

#ifdef CONFIG_CHELSIO_T1_1G
	{
		.board		= CHBT_BOARD_CHN204,
		.port_number	= 4,
		.caps		= SUPPORTED_10baseT_Half | SUPPORTED_10baseT_Full
				| SUPPORTED_100baseT_Half | SUPPORTED_100baseT_Full
				| SUPPORTED_1000baseT_Full | SUPPORTED_Autoneg |
				  SUPPORTED_PAUSE | SUPPORTED_TP,
		.chip_term	= CHBT_TERM_T2,
		.chip_mac	= CHBT_MAC_VSC7321,
		.chip_phy	= CHBT_PHY_88E1111,
		.clock_core	= 100000000,
		.espi_nports	= 4,
		.clock_elmer0	= 44,
		.mdio_mdien	= 0,
		.mdio_mdiinv	= 0,
		.mdio_mdc	= 0,
		.mdio_phybaseaddr = 4,
		.gmac		= &t1_vsc7326_ops,
		.gphy		= &t1_mv88e1xxx_ops,
		.mdio_ops	= &mi1_mdio_ops,
		.desc		= "Chelsio N204 4x100/1000BaseT NIC",
	},
#endif

};

const struct pci_device_id t1_pci_tbl[] = {
	CH_DEVICE(8, 0, CH_BRD_T110_1CU),
	CH_DEVICE(8, 1, CH_BRD_T110_1CU),
	CH_DEVICE(7, 0, CH_BRD_N110_1F),
	CH_DEVICE(10, 1, CH_BRD_N210_1F),
	CH_DEVICE(11, 1, CH_BRD_T210_1F),
	CH_DEVICE(14, 1, CH_BRD_T210_1CU),
	CH_DEVICE(16, 1, CH_BRD_N204_4CU),
	{ 0 }
};

MODULE_DEVICE_TABLE(pci, t1_pci_tbl);

/*
 * Return the board_info structure with a given index.  Out-of-range indices
 * return NULL.
 */
const struct board_info *t1_get_board_info(unsigned int board_id)
{
	return board_id < ARRAY_SIZE(t1_board) ? &t1_board[board_id] : NULL;
}

struct chelsio_vpd_t {
	u32 format_version;
	u8 serial_number[16];
	u8 mac_base_address[6];
	u8 pad[2];           /* make multiple-of-4 size requirement explicit */
};

#define EEPROMSIZE        (8 * 1024)
#define EEPROM_MAX_POLL   4

/*
 * Read SEEPROM. A zero is written to the flag register when the address is
 * written to the Control register. The hardware device will set the flag to a
 * one when 4B have been transferred to the Data register.
 */
int t1_seeprom_read(adapter_t *adapter, u32 addr, __le32 *data)
{
	int i = EEPROM_MAX_POLL;
	u16 val;
	u32 v;

	if (addr >= EEPROMSIZE || (addr & 3))
		return -EINVAL;

	pci_write_config_word(adapter->pdev, A_PCICFG_VPD_ADDR, (u16)addr);
	do {
		udelay(50);
		pci_read_config_word(adapter->pdev, A_PCICFG_VPD_ADDR, &val);
	} while (!(val & F_VPD_OP_FLAG) && --i);

	if (!(val & F_VPD_OP_FLAG)) {
		pr_err("%s: reading EEPROM address 0x%x failed\n",
		       adapter->name, addr);
		return -EIO;
	}
	pci_read_config_dword(adapter->pdev, A_PCICFG_VPD_DATA, &v);
	*data = cpu_to_le32(v);
	return 0;
}

static int t1_eeprom_vpd_get(adapter_t *adapter, struct chelsio_vpd_t *vpd)
{
	int addr, ret = 0;

	for (addr = 0; !ret && addr < sizeof(*vpd); addr += sizeof(u32))
		ret = t1_seeprom_read(adapter, addr,
				      (__le32 *)((u8 *)vpd + addr));

	return ret;
}

/*
 * Read a port's MAC address from the VPD ROM.
 */
static int vpd_macaddress_get(adapter_t *adapter, int index, u8 mac_addr[])
{
	struct chelsio_vpd_t vpd;

	if (t1_eeprom_vpd_get(adapter, &vpd))
		return 1;
	memcpy(mac_addr, vpd.mac_base_address, 5);
	mac_addr[5] = vpd.mac_base_address[5] + index;
	return 0;
}

/*
 * Set up the MAC/PHY according to the requested link settings.
 *
 * If the PHY can auto-negotiate first decide what to advertise, then
 * enable/disable auto-negotiation as desired and reset.
 *
 * If the PHY does not auto-negotiate we just reset it.
 *
 * If auto-negotiation is off set the MAC to the proper speed/duplex/FC,
 * otherwise do it later based on the outcome of auto-negotiation.
 */
int t1_link_start(struct cphy *phy, struct cmac *mac, struct link_config *lc)
{
	unsigned int fc = lc->requested_fc & (PAUSE_RX | PAUSE_TX);

	if (lc->supported & SUPPORTED_Autoneg) {
		lc->advertising &= ~(ADVERTISED_ASYM_PAUSE | ADVERTISED_PAUSE);
		if (fc) {
			if (fc == ((PAUSE_RX | PAUSE_TX) &
				   (mac->adapter->params.nports < 2)))
				lc->advertising |= ADVERTISED_PAUSE;
			else {
				lc->advertising |= ADVERTISED_ASYM_PAUSE;
				if (fc == PAUSE_RX)
					lc->advertising |= ADVERTISED_PAUSE;
			}
		}
		phy->ops->advertise(phy, lc->advertising);

		if (lc->autoneg == AUTONEG_DISABLE) {
			lc->speed = lc->requested_speed;
			lc->duplex = lc->requested_duplex;
			lc->fc = (unsigned char)fc;
			mac->ops->set_speed_duplex_fc(mac, lc->speed,
						      lc->duplex, fc);
			/* Also disables autoneg */
			phy->state = PHY_AUTONEG_RDY;
			phy->ops->set_speed_duplex(phy, lc->speed, lc->duplex);
			phy->ops->reset(phy, 0);
		} else {
			phy->state = PHY_AUTONEG_EN;
			phy->ops->autoneg_enable(phy); /* also resets PHY */
		}
	} else {
		phy->state = PHY_AUTONEG_RDY;
		mac->ops->set_speed_duplex_fc(mac, -1, -1, fc);
		lc->fc = (unsigned char)fc;
		phy->ops->reset(phy, 0);
	}
	return 0;
}

/*
 * External interrupt handler for boards using elmer0.
 */
int t1_elmer0_ext_intr_handler(adapter_t *adapter)
{
	struct cphy *phy;
	int phy_cause;
	u32 cause;

	t1_tpi_read(adapter, A_ELMER0_INT_CAUSE, &cause);

	switch (board_info(adapter)->board) {
#ifdef CONFIG_CHELSIO_T1_1G
	case CHBT_BOARD_CHT204:
	case CHBT_BOARD_CHT204E:
	case CHBT_BOARD_CHN204:
	case CHBT_BOARD_CHT204V: {
		int i, port_bit;
		for_each_port(adapter, i) {
			port_bit = i + 1;
			if (!(cause & (1 << port_bit)))
				continue;

			phy = adapter->port[i].phy;
			phy_cause = phy->ops->interrupt_handler(phy);
			if (phy_cause & cphy_cause_link_change)
				t1_link_changed(adapter, i);
		}
		break;
	}
	case CHBT_BOARD_CHT101:
		if (cause & ELMER0_GP_BIT1) { /* Marvell 88E1111 interrupt */
			phy = adapter->port[0].phy;
			phy_cause = phy->ops->interrupt_handler(phy);
			if (phy_cause & cphy_cause_link_change)
				t1_link_changed(adapter, 0);
		}
		break;
	case CHBT_BOARD_7500: {
		int p;
		/*
		 * Elmer0's interrupt cause isn't useful here because there is
		 * only one bit that can be set for all 4 ports.  This means
		 * we are forced to check every PHY's interrupt status
		 * register to see who initiated the interrupt.
		 */
		for_each_port(adapter, p) {
			phy = adapter->port[p].phy;
			phy_cause = phy->ops->interrupt_handler(phy);
			if (phy_cause & cphy_cause_link_change)
			    t1_link_changed(adapter, p);
		}
		break;
	}
#endif
	case CHBT_BOARD_CHT210:
	case CHBT_BOARD_N210:
	case CHBT_BOARD_N110:
		if (cause & ELMER0_GP_BIT6) { /* Marvell 88x2010 interrupt */
			phy = adapter->port[0].phy;
			phy_cause = phy->ops->interrupt_handler(phy);
			if (phy_cause & cphy_cause_link_change)
				t1_link_changed(adapter, 0);
		}
		break;
	case CHBT_BOARD_8000:
	case CHBT_BOARD_CHT110:
		if (netif_msg_intr(adapter))
			dev_dbg(&adapter->pdev->dev,
				"External interrupt cause 0x%x\n", cause);
		if (cause & ELMER0_GP_BIT1) {        /* PMC3393 INTB */
			struct cmac *mac = adapter->port[0].mac;

			mac->ops->interrupt_handler(mac);
		}
		if (cause & ELMER0_GP_BIT5) {        /* XPAK MOD_DETECT */
			u32 mod_detect;

			t1_tpi_read(adapter,
					A_ELMER0_GPI_STAT, &mod_detect);
			if (netif_msg_link(adapter))
				dev_info(&adapter->pdev->dev, "XPAK %s\n",
					 mod_detect ? "removed" : "inserted");
		}
		break;
	}
	t1_tpi_write(adapter, A_ELMER0_INT_CAUSE, cause);
	return 0;
}

/* Enables all interrupts. */
void t1_interrupts_enable(adapter_t *adapter)
{
	unsigned int i;

	adapter->slow_intr_mask = F_PL_INTR_SGE_ERR | F_PL_INTR_TP;

	t1_sge_intr_enable(adapter->sge);
	t1_tp_intr_enable(adapter->tp);
	if (adapter->espi) {
		adapter->slow_intr_mask |= F_PL_INTR_ESPI;
		t1_espi_intr_enable(adapter->espi);
	}

	/* Enable MAC/PHY interrupts for each port. */
	for_each_port(adapter, i) {
		adapter->port[i].mac->ops->interrupt_enable(adapter->port[i].mac);
		adapter->port[i].phy->ops->interrupt_enable(adapter->port[i].phy);
	}

	/* Enable PCIX & external chip interrupts on ASIC boards. */
	if (t1_is_asic(adapter)) {
		u32 pl_intr = readl(adapter->regs + A_PL_ENABLE);

		/* PCI-X interrupts */
		pci_write_config_dword(adapter->pdev, A_PCICFG_INTR_ENABLE,
				       0xffffffff);

		adapter->slow_intr_mask |= F_PL_INTR_EXT | F_PL_INTR_PCIX;
		pl_intr |= F_PL_INTR_EXT | F_PL_INTR_PCIX;
		writel(pl_intr, adapter->regs + A_PL_ENABLE);
	}
}

/* Disables all interrupts. */
void t1_interrupts_disable(adapter_t* adapter)
{
	unsigned int i;

	t1_sge_intr_disable(adapter->sge);
	t1_tp_intr_disable(adapter->tp);
	if (adapter->espi)
		t1_espi_intr_disable(adapter->espi);

	/* Disable MAC/PHY interrupts for each port. */
	for_each_port(adapter, i) {
		adapter->port[i].mac->ops->interrupt_disable(adapter->port[i].mac);
		adapter->port[i].phy->ops->interrupt_disable(adapter->port[i].phy);
	}

	/* Disable PCIX & external chip interrupts. */
	if (t1_is_asic(adapter))
		writel(0, adapter->regs + A_PL_ENABLE);

	/* PCI-X interrupts */
	pci_write_config_dword(adapter->pdev, A_PCICFG_INTR_ENABLE, 0);

	adapter->slow_intr_mask = 0;
}

/* Clears all interrupts */
void t1_interrupts_clear(adapter_t* adapter)
{
	unsigned int i;

	t1_sge_intr_clear(adapter->sge);
	t1_tp_intr_clear(adapter->tp);
	if (adapter->espi)
		t1_espi_intr_clear(adapter->espi);

	/* Clear MAC/PHY interrupts for each port. */
	for_each_port(adapter, i) {
		adapter->port[i].mac->ops->interrupt_clear(adapter->port[i].mac);
		adapter->port[i].phy->ops->interrupt_clear(adapter->port[i].phy);
	}

	/* Enable interrupts for external devices. */
	if (t1_is_asic(adapter)) {
		u32 pl_intr = readl(adapter->regs + A_PL_CAUSE);

		writel(pl_intr | F_PL_INTR_EXT | F_PL_INTR_PCIX,
		       adapter->regs + A_PL_CAUSE);
	}

	/* PCI-X interrupts */
	pci_write_config_dword(adapter->pdev, A_PCICFG_INTR_CAUSE, 0xffffffff);
}

/*
 * Slow path interrupt handler for ASICs.
 */
static irqreturn_t asic_slow_intr(adapter_t *adapter)
{
	u32 cause = readl(adapter->regs + A_PL_CAUSE);
	irqreturn_t ret = IRQ_HANDLED;

	cause &= adapter->slow_intr_mask;
	if (!cause)
		return IRQ_NONE;
	if (cause & F_PL_INTR_SGE_ERR) {
		if (t1_sge_intr_error_handler(adapter->sge))
			ret = IRQ_WAKE_THREAD;
	}
	if (cause & F_PL_INTR_TP)
		t1_tp_intr_handler(adapter->tp);
	if (cause & F_PL_INTR_ESPI)
		t1_espi_intr_handler(adapter->espi);
	if (cause & F_PL_INTR_PCIX) {
		if (t1_pci_intr_handler(adapter))
			ret = IRQ_WAKE_THREAD;
	}
	if (cause & F_PL_INTR_EXT) {
		/* Wake the threaded interrupt to handle external interrupts as
		 * we require a process context. We disable EXT interrupts in
		 * the interim and let the thread reenable them when it's done.
		 */
		adapter->pending_thread_intr |= F_PL_INTR_EXT;
		adapter->slow_intr_mask &= ~F_PL_INTR_EXT;
		writel(adapter->slow_intr_mask | F_PL_INTR_SGE_DATA,
		       adapter->regs + A_PL_ENABLE);
		ret = IRQ_WAKE_THREAD;
	}

	/* Clear the interrupts just processed. */
	writel(cause, adapter->regs + A_PL_CAUSE);
	readl(adapter->regs + A_PL_CAUSE); /* flush writes */
	return ret;
}

irqreturn_t t1_slow_intr_handler(adapter_t *adapter)
{
#ifdef CONFIG_CHELSIO_T1_1G
	if (!t1_is_asic(adapter))
		return fpga_slow_intr(adapter);
#endif
	return asic_slow_intr(adapter);
}

/* Power sequencing is a work-around for Intel's XPAKs. */
static void power_sequence_xpak(adapter_t* adapter)
{
	u32 mod_detect;
	u32 gpo;

	/* Check for XPAK */
	t1_tpi_read(adapter, A_ELMER0_GPI_STAT, &mod_detect);
	if (!(ELMER0_GP_BIT5 & mod_detect)) {
		/* XPAK is present */
		t1_tpi_read(adapter, A_ELMER0_GPO, &gpo);
		gpo |= ELMER0_GP_BIT18;
		t1_tpi_write(adapter, A_ELMER0_GPO, gpo);
	}
}

int t1_get_board_rev(adapter_t *adapter, const struct board_info *bi,
		     struct adapter_params *p)
{
	p->chip_version = bi->chip_term;
	p->is_asic = (p->chip_version != CHBT_TERM_FPGA);
	if (p->chip_version == CHBT_TERM_T1 ||
	    p->chip_version == CHBT_TERM_T2 ||
	    p->chip_version == CHBT_TERM_FPGA) {
		u32 val = readl(adapter->regs + A_TP_PC_CONFIG);

		val = G_TP_PC_REV(val);
		if (val == 2)
			p->chip_revision = TERM_T1B;
		else if (val == 3)
			p->chip_revision = TERM_T2;
		else
			return -1;
	} else
		return -1;
	return 0;
}

/*
 * Enable board components other than the Chelsio chip, such as external MAC
 * and PHY.
 */
static int board_init(adapter_t *adapter, const struct board_info *bi)
{
	switch (bi->board) {
	case CHBT_BOARD_8000:
	case CHBT_BOARD_N110:
	case CHBT_BOARD_N210:
	case CHBT_BOARD_CHT210:
		t1_tpi_par(adapter, 0xf);
		t1_tpi_write(adapter, A_ELMER0_GPO, 0x800);
		break;
	case CHBT_BOARD_CHT110:
		t1_tpi_par(adapter, 0xf);
		t1_tpi_write(adapter, A_ELMER0_GPO, 0x1800);

		/* TBD XXX Might not need.  This fixes a problem
		 *         described in the Intel SR XPAK errata.
		 */
		power_sequence_xpak(adapter);
		break;
#ifdef CONFIG_CHELSIO_T1_1G
	case CHBT_BOARD_CHT204E:
		/* add config space write here */
	case CHBT_BOARD_CHT204:
	case CHBT_BOARD_CHT204V:
	case CHBT_BOARD_CHN204:
		t1_tpi_par(adapter, 0xf);
		t1_tpi_write(adapter, A_ELMER0_GPO, 0x804);
		break;
	case CHBT_BOARD_CHT101:
	case CHBT_BOARD_7500:
		t1_tpi_par(adapter, 0xf);
		t1_tpi_write(adapter, A_ELMER0_GPO, 0x1804);
		break;
#endif
	}
	return 0;
}

/*
 * Initialize and configure the Terminator HW modules.  Note that external
 * MAC and PHYs are initialized separately.
 */
int t1_init_hw_modules(adapter_t *adapter)
{
	int err = -EIO;
	const struct board_info *bi = board_info(adapter);

	if (!bi->clock_mc4) {
		u32 val = readl(adapter->regs + A_MC4_CFG);

		writel(val | F_READY | F_MC4_SLOW, adapter->regs + A_MC4_CFG);
		writel(F_M_BUS_ENABLE | F_TCAM_RESET,
		       adapter->regs + A_MC5_CONFIG);
	}

	if (adapter->espi && t1_espi_init(adapter->espi, bi->chip_mac,
					  bi->espi_nports))
		goto out_err;

	if (t1_tp_reset(adapter->tp, &adapter->params.tp, bi->clock_core))
		goto out_err;

	err = t1_sge_configure(adapter->sge, &adapter->params.sge);
	if (err)
		goto out_err;

	err = 0;
out_err:
	return err;
}

/*
 * Determine a card's PCI mode.
 */
static void get_pci_mode(adapter_t *adapter, struct chelsio_pci_params *p)
{
	static const unsigned short speed_map[] = { 33, 66, 100, 133 };
	u32 pci_mode;

	pci_read_config_dword(adapter->pdev, A_PCICFG_MODE, &pci_mode);
	p->speed = speed_map[G_PCI_MODE_CLK(pci_mode)];
	p->width = (pci_mode & F_PCI_MODE_64BIT) ? 64 : 32;
	p->is_pcix = (pci_mode & F_PCI_MODE_PCIX) != 0;
}

/*
 * Release the structures holding the SW per-Terminator-HW-module state.
 */
void t1_free_sw_modules(adapter_t *adapter)
{
	unsigned int i;

	for_each_port(adapter, i) {
		struct cmac *mac = adapter->port[i].mac;
		struct cphy *phy = adapter->port[i].phy;

		if (mac)
			mac->ops->destroy(mac);
		if (phy)
			phy->ops->destroy(phy);
	}

	if (adapter->sge)
		t1_sge_destroy(adapter->sge);
	if (adapter->tp)
		t1_tp_destroy(adapter->tp);
	if (adapter->espi)
		t1_espi_destroy(adapter->espi);
}

static void init_link_config(struct link_config *lc,
			     const struct board_info *bi)
{
	lc->supported = bi->caps;
	lc->requested_speed = lc->speed = SPEED_INVALID;
	lc->requested_duplex = lc->duplex = DUPLEX_INVALID;
	lc->requested_fc = lc->fc = PAUSE_RX | PAUSE_TX;
	if (lc->supported & SUPPORTED_Autoneg) {
		lc->advertising = lc->supported;
		lc->autoneg = AUTONEG_ENABLE;
		lc->requested_fc |= PAUSE_AUTONEG;
	} else {
		lc->advertising = 0;
		lc->autoneg = AUTONEG_DISABLE;
	}
}

/*
 * Allocate and initialize the data structures that hold the SW state of
 * the Terminator HW modules.
 */
int t1_init_sw_modules(adapter_t *adapter, const struct board_info *bi)
{
	unsigned int i;

	adapter->params.brd_info = bi;
	adapter->params.nports = bi->port_number;
	adapter->params.stats_update_period = bi->gmac->stats_update_period;

	adapter->sge = t1_sge_create(adapter, &adapter->params.sge);
	if (!adapter->sge) {
		pr_err("%s: SGE initialization failed\n",
		       adapter->name);
		goto error;
	}

	if (bi->espi_nports && !(adapter->espi = t1_espi_create(adapter))) {
		pr_err("%s: ESPI initialization failed\n",
		       adapter->name);
		goto error;
	}

	adapter->tp = t1_tp_create(adapter, &adapter->params.tp);
	if (!adapter->tp) {
		pr_err("%s: TP initialization failed\n",
		       adapter->name);
		goto error;
	}

	board_init(adapter, bi);
	bi->mdio_ops->init(adapter, bi);
	if (bi->gphy->reset)
		bi->gphy->reset(adapter);
	if (bi->gmac->reset)
		bi->gmac->reset(adapter);

	for_each_port(adapter, i) {
		u8 hw_addr[6];
		struct cmac *mac;
		int phy_addr = bi->mdio_phybaseaddr + i;

		adapter->port[i].phy = bi->gphy->create(adapter->port[i].dev,
							phy_addr, bi->mdio_ops);
		if (!adapter->port[i].phy) {
			pr_err("%s: PHY %d initialization failed\n",
			       adapter->name, i);
			goto error;
		}

		adapter->port[i].mac = mac = bi->gmac->create(adapter, i);
		if (!mac) {
			pr_err("%s: MAC %d initialization failed\n",
			       adapter->name, i);
			goto error;
		}

		/*
		 * Get the port's MAC addresses either from the EEPROM if one
		 * exists or the one hardcoded in the MAC.
		 */
		if (!t1_is_asic(adapter) || bi->chip_mac == CHBT_MAC_DUMMY)
			mac->ops->macaddress_get(mac, hw_addr);
		else if (vpd_macaddress_get(adapter, i, hw_addr)) {
			pr_err("%s: could not read MAC address from VPD ROM\n",
			       adapter->port[i].dev->name);
			goto error;
		}
		memcpy(adapter->port[i].dev->dev_addr, hw_addr, ETH_ALEN);
		init_link_config(&adapter->port[i].link_config, bi);
	}

	get_pci_mode(adapter, &adapter->params.pci);
	t1_interrupts_clear(adapter);
	return 0;

error:
	t1_free_sw_modules(adapter);
	return -1;
}
