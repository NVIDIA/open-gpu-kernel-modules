// SPDX-License-Identifier: GPL-2.0+
/*
 * drivers/net/phy/cicada.c
 *
 * Driver for Cicada PHYs
 *
 * Author: Andy Fleming
 *
 * Copyright (c) 2004 Freescale Semiconductor, Inc.
 */
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <linux/unistd.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/spinlock.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/mii.h>
#include <linux/ethtool.h>
#include <linux/phy.h>

#include <linux/io.h>
#include <asm/irq.h>
#include <linux/uaccess.h>

/* Cicada Extended Control Register 1 */
#define MII_CIS8201_EXT_CON1           0x17
#define MII_CIS8201_EXTCON1_INIT       0x0000

/* Cicada Interrupt Mask Register */
#define MII_CIS8201_IMASK		0x19
#define MII_CIS8201_IMASK_IEN		0x8000
#define MII_CIS8201_IMASK_SPEED	0x4000
#define MII_CIS8201_IMASK_LINK		0x2000
#define MII_CIS8201_IMASK_DUPLEX	0x1000
#define MII_CIS8201_IMASK_MASK		0xf000

/* Cicada Interrupt Status Register */
#define MII_CIS8201_ISTAT		0x1a
#define MII_CIS8201_ISTAT_STATUS	0x8000
#define MII_CIS8201_ISTAT_SPEED	0x4000
#define MII_CIS8201_ISTAT_LINK		0x2000
#define MII_CIS8201_ISTAT_DUPLEX	0x1000

/* Cicada Auxiliary Control/Status Register */
#define MII_CIS8201_AUX_CONSTAT        0x1c
#define MII_CIS8201_AUXCONSTAT_INIT    0x0004
#define MII_CIS8201_AUXCONSTAT_DUPLEX  0x0020
#define MII_CIS8201_AUXCONSTAT_SPEED   0x0018
#define MII_CIS8201_AUXCONSTAT_GBIT    0x0010
#define MII_CIS8201_AUXCONSTAT_100     0x0008

MODULE_DESCRIPTION("Cicadia PHY driver");
MODULE_AUTHOR("Andy Fleming");
MODULE_LICENSE("GPL");

static int cis820x_config_init(struct phy_device *phydev)
{
	int err;

	err = phy_write(phydev, MII_CIS8201_AUX_CONSTAT,
			MII_CIS8201_AUXCONSTAT_INIT);

	if (err < 0)
		return err;

	err = phy_write(phydev, MII_CIS8201_EXT_CON1,
			MII_CIS8201_EXTCON1_INIT);

	return err;
}

static int cis820x_ack_interrupt(struct phy_device *phydev)
{
	int err = phy_read(phydev, MII_CIS8201_ISTAT);

	return (err < 0) ? err : 0;
}

static int cis820x_config_intr(struct phy_device *phydev)
{
	int err;

	if (phydev->interrupts == PHY_INTERRUPT_ENABLED) {
		err = cis820x_ack_interrupt(phydev);
		if (err)
			return err;

		err = phy_write(phydev, MII_CIS8201_IMASK,
				MII_CIS8201_IMASK_MASK);
	} else {
		err = phy_write(phydev, MII_CIS8201_IMASK, 0);
		if (err)
			return err;

		err = cis820x_ack_interrupt(phydev);
	}

	return err;
}

static irqreturn_t cis820x_handle_interrupt(struct phy_device *phydev)
{
	int irq_status;

	irq_status = phy_read(phydev, MII_CIS8201_ISTAT);
	if (irq_status < 0) {
		phy_error(phydev);
		return IRQ_NONE;
	}

	if (!(irq_status & MII_CIS8201_IMASK_MASK))
		return IRQ_NONE;

	phy_trigger_machine(phydev);

	return IRQ_HANDLED;
}

/* Cicada 8201, a.k.a Vitesse VSC8201 */
static struct phy_driver cis820x_driver[] = {
{
	.phy_id		= 0x000fc410,
	.name		= "Cicada Cis8201",
	.phy_id_mask	= 0x000ffff0,
	/* PHY_GBIT_FEATURES */
	.config_init	= &cis820x_config_init,
	.config_intr	= &cis820x_config_intr,
	.handle_interrupt = &cis820x_handle_interrupt,
}, {
	.phy_id		= 0x000fc440,
	.name		= "Cicada Cis8204",
	.phy_id_mask	= 0x000fffc0,
	/* PHY_GBIT_FEATURES */
	.config_init	= &cis820x_config_init,
	.config_intr	= &cis820x_config_intr,
	.handle_interrupt = &cis820x_handle_interrupt,
} };

module_phy_driver(cis820x_driver);

static struct mdio_device_id __maybe_unused cicada_tbl[] = {
	{ 0x000fc410, 0x000ffff0 },
	{ 0x000fc440, 0x000fffc0 },
	{ }
};

MODULE_DEVICE_TABLE(mdio, cicada_tbl);
