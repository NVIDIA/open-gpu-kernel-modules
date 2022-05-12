// SPDX-License-Identifier: GPL-2.0
/*
 *  This files contains card eeprom (93c46 or 93c56) programming routines,
 *  memory is addressed by 16 bits words.
 *
 *  This is part of rtl8180 OpenSource driver.
 *  Copyright (C) Andrea Merello 2004  <andrea.merello@gmail.com>
 *
 *  Parts of this driver are based on the GPL part of the
 *  official realtek driver.
 *
 *  Parts of this driver are based on the rtl8180 driver skeleton
 *  from Patric Schenke & Andres Salomon.
 *
 *  Parts of this driver are based on the Intel Pro Wireless 2100 GPL driver.
 *
 *  We want to thank the Authors of those projects and the Ndiswrapper
 *  project Authors.
 */

#include "r8180_93cx6.h"

static void eprom_cs(struct net_device *dev, short bit)
{
	u8 cmdreg;
	int err;

	err = read_nic_byte_E(dev, EPROM_CMD, &cmdreg);
	if (err)
		return;
	if (bit)
		/* enable EPROM */
		write_nic_byte_E(dev, EPROM_CMD, cmdreg | EPROM_CS_BIT);
	else
		/* disable EPROM */
		write_nic_byte_E(dev, EPROM_CMD, cmdreg & ~EPROM_CS_BIT);

	force_pci_posting(dev);
	udelay(EPROM_DELAY);
}

static void eprom_ck_cycle(struct net_device *dev)
{
	u8 cmdreg;
	int err;

	err = read_nic_byte_E(dev, EPROM_CMD, &cmdreg);
	if (err)
		return;
	write_nic_byte_E(dev, EPROM_CMD, cmdreg | EPROM_CK_BIT);
	force_pci_posting(dev);
	udelay(EPROM_DELAY);

	read_nic_byte_E(dev, EPROM_CMD, &cmdreg);
	write_nic_byte_E(dev, EPROM_CMD, cmdreg & ~EPROM_CK_BIT);
	force_pci_posting(dev);
	udelay(EPROM_DELAY);
}

static void eprom_w(struct net_device *dev, short bit)
{
	u8 cmdreg;
	int err;

	err = read_nic_byte_E(dev, EPROM_CMD, &cmdreg);
	if (err)
		return;
	if (bit)
		write_nic_byte_E(dev, EPROM_CMD, cmdreg | EPROM_W_BIT);
	else
		write_nic_byte_E(dev, EPROM_CMD, cmdreg & ~EPROM_W_BIT);

	force_pci_posting(dev);
	udelay(EPROM_DELAY);
}

static short eprom_r(struct net_device *dev)
{
	u8 bit;
	int err;

	err = read_nic_byte_E(dev, EPROM_CMD, &bit);
	if (err)
		return err;

	udelay(EPROM_DELAY);

	if (bit & EPROM_R_BIT)
		return 1;

	return 0;
}

static void eprom_send_bits_string(struct net_device *dev, short b[], int len)
{
	int i;

	for (i = 0; i < len; i++) {
		eprom_w(dev, b[i]);
		eprom_ck_cycle(dev);
	}
}

int eprom_read(struct net_device *dev, u32 addr)
{
	struct r8192_priv *priv = ieee80211_priv(dev);
	short read_cmd[] = {1, 1, 0};
	short addr_str[8];
	int i;
	int addr_len;
	u32 ret;
	int err;

	ret = 0;
	/* enable EPROM programming */
	write_nic_byte_E(dev, EPROM_CMD,
		       (EPROM_CMD_PROGRAM << EPROM_CMD_OPERATING_MODE_SHIFT));
	force_pci_posting(dev);
	udelay(EPROM_DELAY);

	if (priv->epromtype == EPROM_93c56) {
		addr_str[7] = addr & 1;
		addr_str[6] = addr & BIT(1);
		addr_str[5] = addr & BIT(2);
		addr_str[4] = addr & BIT(3);
		addr_str[3] = addr & BIT(4);
		addr_str[2] = addr & BIT(5);
		addr_str[1] = addr & BIT(6);
		addr_str[0] = addr & BIT(7);
		addr_len = 8;
	} else {
		addr_str[5] = addr & 1;
		addr_str[4] = addr & BIT(1);
		addr_str[3] = addr & BIT(2);
		addr_str[2] = addr & BIT(3);
		addr_str[1] = addr & BIT(4);
		addr_str[0] = addr & BIT(5);
		addr_len = 6;
	}
	eprom_cs(dev, 1);
	eprom_ck_cycle(dev);
	eprom_send_bits_string(dev, read_cmd, 3);
	eprom_send_bits_string(dev, addr_str, addr_len);

	/*
	 * keep chip pin D to low state while reading.
	 * I'm unsure if it is necessary, but anyway shouldn't hurt
	 */
	eprom_w(dev, 0);

	for (i = 0; i < 16; i++) {
		/* eeprom needs a clk cycle between writing opcode&adr
		 * and reading data. (eeprom outs a dummy 0)
		 */
		eprom_ck_cycle(dev);
		err = eprom_r(dev);
		if (err < 0)
			return err;

		ret |= err << (15 - i);
	}

	eprom_cs(dev, 0);
	eprom_ck_cycle(dev);

	/* disable EPROM programming */
	write_nic_byte_E(dev, EPROM_CMD,
		       (EPROM_CMD_NORMAL << EPROM_CMD_OPERATING_MODE_SHIFT));
	return ret;
}
