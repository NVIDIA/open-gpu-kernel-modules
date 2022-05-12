// SPDX-License-Identifier: GPL-2.0+
/*
 * drivers/net/phy/ste10Xp.c
 *
 * Driver for STMicroelectronics STe10Xp PHYs
 *
 * Author: Giuseppe Cavallaro <peppe.cavallaro@st.com>
 *
 * Copyright (c) 2008 STMicroelectronics Limited
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>
#include <linux/interrupt.h>
#include <linux/netdevice.h>
#include <linux/ethtool.h>
#include <linux/mii.h>
#include <linux/phy.h>

#define MII_XCIIS   	0x11	/* Configuration Info IRQ & Status Reg */
#define MII_XIE     	0x12	/* Interrupt Enable Register */
#define MII_XIE_DEFAULT_MASK 0x0070 /* ANE complete, Remote Fault, Link Down */

#define STE101P_PHY_ID		0x00061c50
#define STE100P_PHY_ID       	0x1c040011

static int ste10Xp_config_init(struct phy_device *phydev)
{
	int value, err;

	/* Software Reset PHY */
	value = phy_read(phydev, MII_BMCR);
	if (value < 0)
		return value;

	value |= BMCR_RESET;
	err = phy_write(phydev, MII_BMCR, value);
	if (err < 0)
		return err;

	do {
		value = phy_read(phydev, MII_BMCR);
	} while (value & BMCR_RESET);

	return 0;
}

static int ste10Xp_ack_interrupt(struct phy_device *phydev)
{
	int err = phy_read(phydev, MII_XCIIS);

	if (err < 0)
		return err;

	return 0;
}

static int ste10Xp_config_intr(struct phy_device *phydev)
{
	int err;

	if (phydev->interrupts == PHY_INTERRUPT_ENABLED) {
		/* clear any pending interrupts */
		err = ste10Xp_ack_interrupt(phydev);
		if (err)
			return err;

		/* Enable all STe101P interrupts (PR12) */
		err = phy_write(phydev, MII_XIE, MII_XIE_DEFAULT_MASK);
	} else {
		err = phy_write(phydev, MII_XIE, 0);
		if (err)
			return err;

		err = ste10Xp_ack_interrupt(phydev);
	}

	return err;
}

static irqreturn_t ste10Xp_handle_interrupt(struct phy_device *phydev)
{
	int irq_status;

	irq_status = phy_read(phydev, MII_XCIIS);
	if (irq_status < 0) {
		phy_error(phydev);
		return IRQ_NONE;
	}

	if (!(irq_status & MII_XIE_DEFAULT_MASK))
		return IRQ_NONE;

	phy_trigger_machine(phydev);

	return IRQ_HANDLED;
}

static struct phy_driver ste10xp_pdriver[] = {
{
	.phy_id = STE101P_PHY_ID,
	.phy_id_mask = 0xfffffff0,
	.name = "STe101p",
	/* PHY_BASIC_FEATURES */
	.config_init = ste10Xp_config_init,
	.config_intr = ste10Xp_config_intr,
	.handle_interrupt = ste10Xp_handle_interrupt,
	.suspend = genphy_suspend,
	.resume = genphy_resume,
}, {
	.phy_id = STE100P_PHY_ID,
	.phy_id_mask = 0xffffffff,
	.name = "STe100p",
	/* PHY_BASIC_FEATURES */
	.config_init = ste10Xp_config_init,
	.config_intr = ste10Xp_config_intr,
	.handle_interrupt = ste10Xp_handle_interrupt,
	.suspend = genphy_suspend,
	.resume = genphy_resume,
} };

module_phy_driver(ste10xp_pdriver);

static struct mdio_device_id __maybe_unused ste10Xp_tbl[] = {
	{ STE101P_PHY_ID, 0xfffffff0 },
	{ STE100P_PHY_ID, 0xffffffff },
	{ }
};

MODULE_DEVICE_TABLE(mdio, ste10Xp_tbl);

MODULE_DESCRIPTION("STMicroelectronics STe10Xp PHY driver");
MODULE_AUTHOR("Giuseppe Cavallaro <peppe.cavallaro@st.com>");
MODULE_LICENSE("GPL");
