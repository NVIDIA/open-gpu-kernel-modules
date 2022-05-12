// SPDX-License-Identifier: GPL-2.0-only
/*
 * Mediatek MT7530 DSA Switch driver
 * Copyright (C) 2017 Sean Wang <sean.wang@mediatek.com>
 */
#include <linux/etherdevice.h>
#include <linux/if_bridge.h>
#include <linux/iopoll.h>
#include <linux/mdio.h>
#include <linux/mfd/syscon.h>
#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/of_mdio.h>
#include <linux/of_net.h>
#include <linux/of_platform.h>
#include <linux/phylink.h>
#include <linux/regmap.h>
#include <linux/regulator/consumer.h>
#include <linux/reset.h>
#include <linux/gpio/consumer.h>
#include <linux/gpio/driver.h>
#include <net/dsa.h>

#include "mt7530.h"

/* String, offset, and register size in bytes if different from 4 bytes */
static const struct mt7530_mib_desc mt7530_mib[] = {
	MIB_DESC(1, 0x00, "TxDrop"),
	MIB_DESC(1, 0x04, "TxCrcErr"),
	MIB_DESC(1, 0x08, "TxUnicast"),
	MIB_DESC(1, 0x0c, "TxMulticast"),
	MIB_DESC(1, 0x10, "TxBroadcast"),
	MIB_DESC(1, 0x14, "TxCollision"),
	MIB_DESC(1, 0x18, "TxSingleCollision"),
	MIB_DESC(1, 0x1c, "TxMultipleCollision"),
	MIB_DESC(1, 0x20, "TxDeferred"),
	MIB_DESC(1, 0x24, "TxLateCollision"),
	MIB_DESC(1, 0x28, "TxExcessiveCollistion"),
	MIB_DESC(1, 0x2c, "TxPause"),
	MIB_DESC(1, 0x30, "TxPktSz64"),
	MIB_DESC(1, 0x34, "TxPktSz65To127"),
	MIB_DESC(1, 0x38, "TxPktSz128To255"),
	MIB_DESC(1, 0x3c, "TxPktSz256To511"),
	MIB_DESC(1, 0x40, "TxPktSz512To1023"),
	MIB_DESC(1, 0x44, "Tx1024ToMax"),
	MIB_DESC(2, 0x48, "TxBytes"),
	MIB_DESC(1, 0x60, "RxDrop"),
	MIB_DESC(1, 0x64, "RxFiltering"),
	MIB_DESC(1, 0x6c, "RxMulticast"),
	MIB_DESC(1, 0x70, "RxBroadcast"),
	MIB_DESC(1, 0x74, "RxAlignErr"),
	MIB_DESC(1, 0x78, "RxCrcErr"),
	MIB_DESC(1, 0x7c, "RxUnderSizeErr"),
	MIB_DESC(1, 0x80, "RxFragErr"),
	MIB_DESC(1, 0x84, "RxOverSzErr"),
	MIB_DESC(1, 0x88, "RxJabberErr"),
	MIB_DESC(1, 0x8c, "RxPause"),
	MIB_DESC(1, 0x90, "RxPktSz64"),
	MIB_DESC(1, 0x94, "RxPktSz65To127"),
	MIB_DESC(1, 0x98, "RxPktSz128To255"),
	MIB_DESC(1, 0x9c, "RxPktSz256To511"),
	MIB_DESC(1, 0xa0, "RxPktSz512To1023"),
	MIB_DESC(1, 0xa4, "RxPktSz1024ToMax"),
	MIB_DESC(2, 0xa8, "RxBytes"),
	MIB_DESC(1, 0xb0, "RxCtrlDrop"),
	MIB_DESC(1, 0xb4, "RxIngressDrop"),
	MIB_DESC(1, 0xb8, "RxArlDrop"),
};

/* Since phy_device has not yet been created and
 * phy_{read,write}_mmd_indirect is not available, we provide our own
 * core_{read,write}_mmd_indirect with core_{clear,write,set} wrappers
 * to complete this function.
 */
static int
core_read_mmd_indirect(struct mt7530_priv *priv, int prtad, int devad)
{
	struct mii_bus *bus = priv->bus;
	int value, ret;

	/* Write the desired MMD Devad */
	ret = bus->write(bus, 0, MII_MMD_CTRL, devad);
	if (ret < 0)
		goto err;

	/* Write the desired MMD register address */
	ret = bus->write(bus, 0, MII_MMD_DATA, prtad);
	if (ret < 0)
		goto err;

	/* Select the Function : DATA with no post increment */
	ret = bus->write(bus, 0, MII_MMD_CTRL, (devad | MII_MMD_CTRL_NOINCR));
	if (ret < 0)
		goto err;

	/* Read the content of the MMD's selected register */
	value = bus->read(bus, 0, MII_MMD_DATA);

	return value;
err:
	dev_err(&bus->dev,  "failed to read mmd register\n");

	return ret;
}

static int
core_write_mmd_indirect(struct mt7530_priv *priv, int prtad,
			int devad, u32 data)
{
	struct mii_bus *bus = priv->bus;
	int ret;

	/* Write the desired MMD Devad */
	ret = bus->write(bus, 0, MII_MMD_CTRL, devad);
	if (ret < 0)
		goto err;

	/* Write the desired MMD register address */
	ret = bus->write(bus, 0, MII_MMD_DATA, prtad);
	if (ret < 0)
		goto err;

	/* Select the Function : DATA with no post increment */
	ret = bus->write(bus, 0, MII_MMD_CTRL, (devad | MII_MMD_CTRL_NOINCR));
	if (ret < 0)
		goto err;

	/* Write the data into MMD's selected register */
	ret = bus->write(bus, 0, MII_MMD_DATA, data);
err:
	if (ret < 0)
		dev_err(&bus->dev,
			"failed to write mmd register\n");
	return ret;
}

static void
core_write(struct mt7530_priv *priv, u32 reg, u32 val)
{
	struct mii_bus *bus = priv->bus;

	mutex_lock_nested(&bus->mdio_lock, MDIO_MUTEX_NESTED);

	core_write_mmd_indirect(priv, reg, MDIO_MMD_VEND2, val);

	mutex_unlock(&bus->mdio_lock);
}

static void
core_rmw(struct mt7530_priv *priv, u32 reg, u32 mask, u32 set)
{
	struct mii_bus *bus = priv->bus;
	u32 val;

	mutex_lock_nested(&bus->mdio_lock, MDIO_MUTEX_NESTED);

	val = core_read_mmd_indirect(priv, reg, MDIO_MMD_VEND2);
	val &= ~mask;
	val |= set;
	core_write_mmd_indirect(priv, reg, MDIO_MMD_VEND2, val);

	mutex_unlock(&bus->mdio_lock);
}

static void
core_set(struct mt7530_priv *priv, u32 reg, u32 val)
{
	core_rmw(priv, reg, 0, val);
}

static void
core_clear(struct mt7530_priv *priv, u32 reg, u32 val)
{
	core_rmw(priv, reg, val, 0);
}

static int
mt7530_mii_write(struct mt7530_priv *priv, u32 reg, u32 val)
{
	struct mii_bus *bus = priv->bus;
	u16 page, r, lo, hi;
	int ret;

	page = (reg >> 6) & 0x3ff;
	r  = (reg >> 2) & 0xf;
	lo = val & 0xffff;
	hi = val >> 16;

	/* MT7530 uses 31 as the pseudo port */
	ret = bus->write(bus, 0x1f, 0x1f, page);
	if (ret < 0)
		goto err;

	ret = bus->write(bus, 0x1f, r,  lo);
	if (ret < 0)
		goto err;

	ret = bus->write(bus, 0x1f, 0x10, hi);
err:
	if (ret < 0)
		dev_err(&bus->dev,
			"failed to write mt7530 register\n");
	return ret;
}

static u32
mt7530_mii_read(struct mt7530_priv *priv, u32 reg)
{
	struct mii_bus *bus = priv->bus;
	u16 page, r, lo, hi;
	int ret;

	page = (reg >> 6) & 0x3ff;
	r = (reg >> 2) & 0xf;

	/* MT7530 uses 31 as the pseudo port */
	ret = bus->write(bus, 0x1f, 0x1f, page);
	if (ret < 0) {
		dev_err(&bus->dev,
			"failed to read mt7530 register\n");
		return ret;
	}

	lo = bus->read(bus, 0x1f, r);
	hi = bus->read(bus, 0x1f, 0x10);

	return (hi << 16) | (lo & 0xffff);
}

static void
mt7530_write(struct mt7530_priv *priv, u32 reg, u32 val)
{
	struct mii_bus *bus = priv->bus;

	mutex_lock_nested(&bus->mdio_lock, MDIO_MUTEX_NESTED);

	mt7530_mii_write(priv, reg, val);

	mutex_unlock(&bus->mdio_lock);
}

static u32
_mt7530_unlocked_read(struct mt7530_dummy_poll *p)
{
	return mt7530_mii_read(p->priv, p->reg);
}

static u32
_mt7530_read(struct mt7530_dummy_poll *p)
{
	struct mii_bus		*bus = p->priv->bus;
	u32 val;

	mutex_lock_nested(&bus->mdio_lock, MDIO_MUTEX_NESTED);

	val = mt7530_mii_read(p->priv, p->reg);

	mutex_unlock(&bus->mdio_lock);

	return val;
}

static u32
mt7530_read(struct mt7530_priv *priv, u32 reg)
{
	struct mt7530_dummy_poll p;

	INIT_MT7530_DUMMY_POLL(&p, priv, reg);
	return _mt7530_read(&p);
}

static void
mt7530_rmw(struct mt7530_priv *priv, u32 reg,
	   u32 mask, u32 set)
{
	struct mii_bus *bus = priv->bus;
	u32 val;

	mutex_lock_nested(&bus->mdio_lock, MDIO_MUTEX_NESTED);

	val = mt7530_mii_read(priv, reg);
	val &= ~mask;
	val |= set;
	mt7530_mii_write(priv, reg, val);

	mutex_unlock(&bus->mdio_lock);
}

static void
mt7530_set(struct mt7530_priv *priv, u32 reg, u32 val)
{
	mt7530_rmw(priv, reg, 0, val);
}

static void
mt7530_clear(struct mt7530_priv *priv, u32 reg, u32 val)
{
	mt7530_rmw(priv, reg, val, 0);
}

static int
mt7530_fdb_cmd(struct mt7530_priv *priv, enum mt7530_fdb_cmd cmd, u32 *rsp)
{
	u32 val;
	int ret;
	struct mt7530_dummy_poll p;

	/* Set the command operating upon the MAC address entries */
	val = ATC_BUSY | ATC_MAT(0) | cmd;
	mt7530_write(priv, MT7530_ATC, val);

	INIT_MT7530_DUMMY_POLL(&p, priv, MT7530_ATC);
	ret = readx_poll_timeout(_mt7530_read, &p, val,
				 !(val & ATC_BUSY), 20, 20000);
	if (ret < 0) {
		dev_err(priv->dev, "reset timeout\n");
		return ret;
	}

	/* Additional sanity for read command if the specified
	 * entry is invalid
	 */
	val = mt7530_read(priv, MT7530_ATC);
	if ((cmd == MT7530_FDB_READ) && (val & ATC_INVALID))
		return -EINVAL;

	if (rsp)
		*rsp = val;

	return 0;
}

static void
mt7530_fdb_read(struct mt7530_priv *priv, struct mt7530_fdb *fdb)
{
	u32 reg[3];
	int i;

	/* Read from ARL table into an array */
	for (i = 0; i < 3; i++) {
		reg[i] = mt7530_read(priv, MT7530_TSRA1 + (i * 4));

		dev_dbg(priv->dev, "%s(%d) reg[%d]=0x%x\n",
			__func__, __LINE__, i, reg[i]);
	}

	fdb->vid = (reg[1] >> CVID) & CVID_MASK;
	fdb->aging = (reg[2] >> AGE_TIMER) & AGE_TIMER_MASK;
	fdb->port_mask = (reg[2] >> PORT_MAP) & PORT_MAP_MASK;
	fdb->mac[0] = (reg[0] >> MAC_BYTE_0) & MAC_BYTE_MASK;
	fdb->mac[1] = (reg[0] >> MAC_BYTE_1) & MAC_BYTE_MASK;
	fdb->mac[2] = (reg[0] >> MAC_BYTE_2) & MAC_BYTE_MASK;
	fdb->mac[3] = (reg[0] >> MAC_BYTE_3) & MAC_BYTE_MASK;
	fdb->mac[4] = (reg[1] >> MAC_BYTE_4) & MAC_BYTE_MASK;
	fdb->mac[5] = (reg[1] >> MAC_BYTE_5) & MAC_BYTE_MASK;
	fdb->noarp = ((reg[2] >> ENT_STATUS) & ENT_STATUS_MASK) == STATIC_ENT;
}

static void
mt7530_fdb_write(struct mt7530_priv *priv, u16 vid,
		 u8 port_mask, const u8 *mac,
		 u8 aging, u8 type)
{
	u32 reg[3] = { 0 };
	int i;

	reg[1] |= vid & CVID_MASK;
	reg[2] |= (aging & AGE_TIMER_MASK) << AGE_TIMER;
	reg[2] |= (port_mask & PORT_MAP_MASK) << PORT_MAP;
	/* STATIC_ENT indicate that entry is static wouldn't
	 * be aged out and STATIC_EMP specified as erasing an
	 * entry
	 */
	reg[2] |= (type & ENT_STATUS_MASK) << ENT_STATUS;
	reg[1] |= mac[5] << MAC_BYTE_5;
	reg[1] |= mac[4] << MAC_BYTE_4;
	reg[0] |= mac[3] << MAC_BYTE_3;
	reg[0] |= mac[2] << MAC_BYTE_2;
	reg[0] |= mac[1] << MAC_BYTE_1;
	reg[0] |= mac[0] << MAC_BYTE_0;

	/* Write array into the ARL table */
	for (i = 0; i < 3; i++)
		mt7530_write(priv, MT7530_ATA1 + (i * 4), reg[i]);
}

/* Setup TX circuit including relevant PAD and driving */
static int
mt7530_pad_clk_setup(struct dsa_switch *ds, phy_interface_t interface)
{
	struct mt7530_priv *priv = ds->priv;
	u32 ncpo1, ssc_delta, trgint, i, xtal;

	xtal = mt7530_read(priv, MT7530_MHWTRAP) & HWTRAP_XTAL_MASK;

	if (xtal == HWTRAP_XTAL_20MHZ) {
		dev_err(priv->dev,
			"%s: MT7530 with a 20MHz XTAL is not supported!\n",
			__func__);
		return -EINVAL;
	}

	switch (interface) {
	case PHY_INTERFACE_MODE_RGMII:
		trgint = 0;
		/* PLL frequency: 125MHz */
		ncpo1 = 0x0c80;
		break;
	case PHY_INTERFACE_MODE_TRGMII:
		trgint = 1;
		if (priv->id == ID_MT7621) {
			/* PLL frequency: 150MHz: 1.2GBit */
			if (xtal == HWTRAP_XTAL_40MHZ)
				ncpo1 = 0x0780;
			if (xtal == HWTRAP_XTAL_25MHZ)
				ncpo1 = 0x0a00;
		} else { /* PLL frequency: 250MHz: 2.0Gbit */
			if (xtal == HWTRAP_XTAL_40MHZ)
				ncpo1 = 0x0c80;
			if (xtal == HWTRAP_XTAL_25MHZ)
				ncpo1 = 0x1400;
		}
		break;
	default:
		dev_err(priv->dev, "xMII interface %d not supported\n",
			interface);
		return -EINVAL;
	}

	if (xtal == HWTRAP_XTAL_25MHZ)
		ssc_delta = 0x57;
	else
		ssc_delta = 0x87;

	mt7530_rmw(priv, MT7530_P6ECR, P6_INTF_MODE_MASK,
		   P6_INTF_MODE(trgint));

	/* Lower Tx Driving for TRGMII path */
	for (i = 0 ; i < NUM_TRGMII_CTRL ; i++)
		mt7530_write(priv, MT7530_TRGMII_TD_ODT(i),
			     TD_DM_DRVP(8) | TD_DM_DRVN(8));

	/* Disable MT7530 core and TRGMII Tx clocks */
	core_clear(priv, CORE_TRGMII_GSW_CLK_CG,
		   REG_GSWCK_EN | REG_TRGMIICK_EN);

	/* Setup core clock for MT7530 */
	/* Disable PLL */
	core_write(priv, CORE_GSWPLL_GRP1, 0);

	/* Set core clock into 500Mhz */
	core_write(priv, CORE_GSWPLL_GRP2,
		   RG_GSWPLL_POSDIV_500M(1) |
		   RG_GSWPLL_FBKDIV_500M(25));

	/* Enable PLL */
	core_write(priv, CORE_GSWPLL_GRP1,
		   RG_GSWPLL_EN_PRE |
		   RG_GSWPLL_POSDIV_200M(2) |
		   RG_GSWPLL_FBKDIV_200M(32));

	/* Setup the MT7530 TRGMII Tx Clock */
	core_write(priv, CORE_PLL_GROUP5, RG_LCDDS_PCW_NCPO1(ncpo1));
	core_write(priv, CORE_PLL_GROUP6, RG_LCDDS_PCW_NCPO0(0));
	core_write(priv, CORE_PLL_GROUP10, RG_LCDDS_SSC_DELTA(ssc_delta));
	core_write(priv, CORE_PLL_GROUP11, RG_LCDDS_SSC_DELTA1(ssc_delta));
	core_write(priv, CORE_PLL_GROUP4,
		   RG_SYSPLL_DDSFBK_EN | RG_SYSPLL_BIAS_EN |
		   RG_SYSPLL_BIAS_LPF_EN);
	core_write(priv, CORE_PLL_GROUP2,
		   RG_SYSPLL_EN_NORMAL | RG_SYSPLL_VODEN |
		   RG_SYSPLL_POSDIV(1));
	core_write(priv, CORE_PLL_GROUP7,
		   RG_LCDDS_PCW_NCPO_CHG | RG_LCCDS_C(3) |
		   RG_LCDDS_PWDB | RG_LCDDS_ISO_EN);

	/* Enable MT7530 core and TRGMII Tx clocks */
	core_set(priv, CORE_TRGMII_GSW_CLK_CG,
		 REG_GSWCK_EN | REG_TRGMIICK_EN);

	if (!trgint)
		for (i = 0 ; i < NUM_TRGMII_CTRL; i++)
			mt7530_rmw(priv, MT7530_TRGMII_RD(i),
				   RD_TAP_MASK, RD_TAP(16));
	return 0;
}

static bool mt7531_dual_sgmii_supported(struct mt7530_priv *priv)
{
	u32 val;

	val = mt7530_read(priv, MT7531_TOP_SIG_SR);

	return (val & PAD_DUAL_SGMII_EN) != 0;
}

static int
mt7531_pad_setup(struct dsa_switch *ds, phy_interface_t interface)
{
	struct mt7530_priv *priv = ds->priv;
	u32 top_sig;
	u32 hwstrap;
	u32 xtal;
	u32 val;

	if (mt7531_dual_sgmii_supported(priv))
		return 0;

	val = mt7530_read(priv, MT7531_CREV);
	top_sig = mt7530_read(priv, MT7531_TOP_SIG_SR);
	hwstrap = mt7530_read(priv, MT7531_HWTRAP);
	if ((val & CHIP_REV_M) > 0)
		xtal = (top_sig & PAD_MCM_SMI_EN) ? HWTRAP_XTAL_FSEL_40MHZ :
						    HWTRAP_XTAL_FSEL_25MHZ;
	else
		xtal = hwstrap & HWTRAP_XTAL_FSEL_MASK;

	/* Step 1 : Disable MT7531 COREPLL */
	val = mt7530_read(priv, MT7531_PLLGP_EN);
	val &= ~EN_COREPLL;
	mt7530_write(priv, MT7531_PLLGP_EN, val);

	/* Step 2: switch to XTAL output */
	val = mt7530_read(priv, MT7531_PLLGP_EN);
	val |= SW_CLKSW;
	mt7530_write(priv, MT7531_PLLGP_EN, val);

	val = mt7530_read(priv, MT7531_PLLGP_CR0);
	val &= ~RG_COREPLL_EN;
	mt7530_write(priv, MT7531_PLLGP_CR0, val);

	/* Step 3: disable PLLGP and enable program PLLGP */
	val = mt7530_read(priv, MT7531_PLLGP_EN);
	val |= SW_PLLGP;
	mt7530_write(priv, MT7531_PLLGP_EN, val);

	/* Step 4: program COREPLL output frequency to 500MHz */
	val = mt7530_read(priv, MT7531_PLLGP_CR0);
	val &= ~RG_COREPLL_POSDIV_M;
	val |= 2 << RG_COREPLL_POSDIV_S;
	mt7530_write(priv, MT7531_PLLGP_CR0, val);
	usleep_range(25, 35);

	switch (xtal) {
	case HWTRAP_XTAL_FSEL_25MHZ:
		val = mt7530_read(priv, MT7531_PLLGP_CR0);
		val &= ~RG_COREPLL_SDM_PCW_M;
		val |= 0x140000 << RG_COREPLL_SDM_PCW_S;
		mt7530_write(priv, MT7531_PLLGP_CR0, val);
		break;
	case HWTRAP_XTAL_FSEL_40MHZ:
		val = mt7530_read(priv, MT7531_PLLGP_CR0);
		val &= ~RG_COREPLL_SDM_PCW_M;
		val |= 0x190000 << RG_COREPLL_SDM_PCW_S;
		mt7530_write(priv, MT7531_PLLGP_CR0, val);
		break;
	}

	/* Set feedback divide ratio update signal to high */
	val = mt7530_read(priv, MT7531_PLLGP_CR0);
	val |= RG_COREPLL_SDM_PCW_CHG;
	mt7530_write(priv, MT7531_PLLGP_CR0, val);
	/* Wait for at least 16 XTAL clocks */
	usleep_range(10, 20);

	/* Step 5: set feedback divide ratio update signal to low */
	val = mt7530_read(priv, MT7531_PLLGP_CR0);
	val &= ~RG_COREPLL_SDM_PCW_CHG;
	mt7530_write(priv, MT7531_PLLGP_CR0, val);

	/* Enable 325M clock for SGMII */
	mt7530_write(priv, MT7531_ANA_PLLGP_CR5, 0xad0000);

	/* Enable 250SSC clock for RGMII */
	mt7530_write(priv, MT7531_ANA_PLLGP_CR2, 0x4f40000);

	/* Step 6: Enable MT7531 PLL */
	val = mt7530_read(priv, MT7531_PLLGP_CR0);
	val |= RG_COREPLL_EN;
	mt7530_write(priv, MT7531_PLLGP_CR0, val);

	val = mt7530_read(priv, MT7531_PLLGP_EN);
	val |= EN_COREPLL;
	mt7530_write(priv, MT7531_PLLGP_EN, val);
	usleep_range(25, 35);

	return 0;
}

static void
mt7530_mib_reset(struct dsa_switch *ds)
{
	struct mt7530_priv *priv = ds->priv;

	mt7530_write(priv, MT7530_MIB_CCR, CCR_MIB_FLUSH);
	mt7530_write(priv, MT7530_MIB_CCR, CCR_MIB_ACTIVATE);
}

static int mt7530_phy_read(struct dsa_switch *ds, int port, int regnum)
{
	struct mt7530_priv *priv = ds->priv;

	return mdiobus_read_nested(priv->bus, port, regnum);
}

static int mt7530_phy_write(struct dsa_switch *ds, int port, int regnum,
			    u16 val)
{
	struct mt7530_priv *priv = ds->priv;

	return mdiobus_write_nested(priv->bus, port, regnum, val);
}

static int
mt7531_ind_c45_phy_read(struct mt7530_priv *priv, int port, int devad,
			int regnum)
{
	struct mii_bus *bus = priv->bus;
	struct mt7530_dummy_poll p;
	u32 reg, val;
	int ret;

	INIT_MT7530_DUMMY_POLL(&p, priv, MT7531_PHY_IAC);

	mutex_lock_nested(&bus->mdio_lock, MDIO_MUTEX_NESTED);

	ret = readx_poll_timeout(_mt7530_unlocked_read, &p, val,
				 !(val & MT7531_PHY_ACS_ST), 20, 100000);
	if (ret < 0) {
		dev_err(priv->dev, "poll timeout\n");
		goto out;
	}

	reg = MT7531_MDIO_CL45_ADDR | MT7531_MDIO_PHY_ADDR(port) |
	      MT7531_MDIO_DEV_ADDR(devad) | regnum;
	mt7530_mii_write(priv, MT7531_PHY_IAC, reg | MT7531_PHY_ACS_ST);

	ret = readx_poll_timeout(_mt7530_unlocked_read, &p, val,
				 !(val & MT7531_PHY_ACS_ST), 20, 100000);
	if (ret < 0) {
		dev_err(priv->dev, "poll timeout\n");
		goto out;
	}

	reg = MT7531_MDIO_CL45_READ | MT7531_MDIO_PHY_ADDR(port) |
	      MT7531_MDIO_DEV_ADDR(devad);
	mt7530_mii_write(priv, MT7531_PHY_IAC, reg | MT7531_PHY_ACS_ST);

	ret = readx_poll_timeout(_mt7530_unlocked_read, &p, val,
				 !(val & MT7531_PHY_ACS_ST), 20, 100000);
	if (ret < 0) {
		dev_err(priv->dev, "poll timeout\n");
		goto out;
	}

	ret = val & MT7531_MDIO_RW_DATA_MASK;
out:
	mutex_unlock(&bus->mdio_lock);

	return ret;
}

static int
mt7531_ind_c45_phy_write(struct mt7530_priv *priv, int port, int devad,
			 int regnum, u32 data)
{
	struct mii_bus *bus = priv->bus;
	struct mt7530_dummy_poll p;
	u32 val, reg;
	int ret;

	INIT_MT7530_DUMMY_POLL(&p, priv, MT7531_PHY_IAC);

	mutex_lock_nested(&bus->mdio_lock, MDIO_MUTEX_NESTED);

	ret = readx_poll_timeout(_mt7530_unlocked_read, &p, val,
				 !(val & MT7531_PHY_ACS_ST), 20, 100000);
	if (ret < 0) {
		dev_err(priv->dev, "poll timeout\n");
		goto out;
	}

	reg = MT7531_MDIO_CL45_ADDR | MT7531_MDIO_PHY_ADDR(port) |
	      MT7531_MDIO_DEV_ADDR(devad) | regnum;
	mt7530_mii_write(priv, MT7531_PHY_IAC, reg | MT7531_PHY_ACS_ST);

	ret = readx_poll_timeout(_mt7530_unlocked_read, &p, val,
				 !(val & MT7531_PHY_ACS_ST), 20, 100000);
	if (ret < 0) {
		dev_err(priv->dev, "poll timeout\n");
		goto out;
	}

	reg = MT7531_MDIO_CL45_WRITE | MT7531_MDIO_PHY_ADDR(port) |
	      MT7531_MDIO_DEV_ADDR(devad) | data;
	mt7530_mii_write(priv, MT7531_PHY_IAC, reg | MT7531_PHY_ACS_ST);

	ret = readx_poll_timeout(_mt7530_unlocked_read, &p, val,
				 !(val & MT7531_PHY_ACS_ST), 20, 100000);
	if (ret < 0) {
		dev_err(priv->dev, "poll timeout\n");
		goto out;
	}

out:
	mutex_unlock(&bus->mdio_lock);

	return ret;
}

static int
mt7531_ind_c22_phy_read(struct mt7530_priv *priv, int port, int regnum)
{
	struct mii_bus *bus = priv->bus;
	struct mt7530_dummy_poll p;
	int ret;
	u32 val;

	INIT_MT7530_DUMMY_POLL(&p, priv, MT7531_PHY_IAC);

	mutex_lock_nested(&bus->mdio_lock, MDIO_MUTEX_NESTED);

	ret = readx_poll_timeout(_mt7530_unlocked_read, &p, val,
				 !(val & MT7531_PHY_ACS_ST), 20, 100000);
	if (ret < 0) {
		dev_err(priv->dev, "poll timeout\n");
		goto out;
	}

	val = MT7531_MDIO_CL22_READ | MT7531_MDIO_PHY_ADDR(port) |
	      MT7531_MDIO_REG_ADDR(regnum);

	mt7530_mii_write(priv, MT7531_PHY_IAC, val | MT7531_PHY_ACS_ST);

	ret = readx_poll_timeout(_mt7530_unlocked_read, &p, val,
				 !(val & MT7531_PHY_ACS_ST), 20, 100000);
	if (ret < 0) {
		dev_err(priv->dev, "poll timeout\n");
		goto out;
	}

	ret = val & MT7531_MDIO_RW_DATA_MASK;
out:
	mutex_unlock(&bus->mdio_lock);

	return ret;
}

static int
mt7531_ind_c22_phy_write(struct mt7530_priv *priv, int port, int regnum,
			 u16 data)
{
	struct mii_bus *bus = priv->bus;
	struct mt7530_dummy_poll p;
	int ret;
	u32 reg;

	INIT_MT7530_DUMMY_POLL(&p, priv, MT7531_PHY_IAC);

	mutex_lock_nested(&bus->mdio_lock, MDIO_MUTEX_NESTED);

	ret = readx_poll_timeout(_mt7530_unlocked_read, &p, reg,
				 !(reg & MT7531_PHY_ACS_ST), 20, 100000);
	if (ret < 0) {
		dev_err(priv->dev, "poll timeout\n");
		goto out;
	}

	reg = MT7531_MDIO_CL22_WRITE | MT7531_MDIO_PHY_ADDR(port) |
	      MT7531_MDIO_REG_ADDR(regnum) | data;

	mt7530_mii_write(priv, MT7531_PHY_IAC, reg | MT7531_PHY_ACS_ST);

	ret = readx_poll_timeout(_mt7530_unlocked_read, &p, reg,
				 !(reg & MT7531_PHY_ACS_ST), 20, 100000);
	if (ret < 0) {
		dev_err(priv->dev, "poll timeout\n");
		goto out;
	}

out:
	mutex_unlock(&bus->mdio_lock);

	return ret;
}

static int
mt7531_ind_phy_read(struct dsa_switch *ds, int port, int regnum)
{
	struct mt7530_priv *priv = ds->priv;
	int devad;
	int ret;

	if (regnum & MII_ADDR_C45) {
		devad = (regnum >> MII_DEVADDR_C45_SHIFT) & 0x1f;
		ret = mt7531_ind_c45_phy_read(priv, port, devad,
					      regnum & MII_REGADDR_C45_MASK);
	} else {
		ret = mt7531_ind_c22_phy_read(priv, port, regnum);
	}

	return ret;
}

static int
mt7531_ind_phy_write(struct dsa_switch *ds, int port, int regnum,
		     u16 data)
{
	struct mt7530_priv *priv = ds->priv;
	int devad;
	int ret;

	if (regnum & MII_ADDR_C45) {
		devad = (regnum >> MII_DEVADDR_C45_SHIFT) & 0x1f;
		ret = mt7531_ind_c45_phy_write(priv, port, devad,
					       regnum & MII_REGADDR_C45_MASK,
					       data);
	} else {
		ret = mt7531_ind_c22_phy_write(priv, port, regnum, data);
	}

	return ret;
}

static void
mt7530_get_strings(struct dsa_switch *ds, int port, u32 stringset,
		   uint8_t *data)
{
	int i;

	if (stringset != ETH_SS_STATS)
		return;

	for (i = 0; i < ARRAY_SIZE(mt7530_mib); i++)
		strncpy(data + i * ETH_GSTRING_LEN, mt7530_mib[i].name,
			ETH_GSTRING_LEN);
}

static void
mt7530_get_ethtool_stats(struct dsa_switch *ds, int port,
			 uint64_t *data)
{
	struct mt7530_priv *priv = ds->priv;
	const struct mt7530_mib_desc *mib;
	u32 reg, i;
	u64 hi;

	for (i = 0; i < ARRAY_SIZE(mt7530_mib); i++) {
		mib = &mt7530_mib[i];
		reg = MT7530_PORT_MIB_COUNTER(port) + mib->offset;

		data[i] = mt7530_read(priv, reg);
		if (mib->size == 2) {
			hi = mt7530_read(priv, reg + 4);
			data[i] |= hi << 32;
		}
	}
}

static int
mt7530_get_sset_count(struct dsa_switch *ds, int port, int sset)
{
	if (sset != ETH_SS_STATS)
		return 0;

	return ARRAY_SIZE(mt7530_mib);
}

static int
mt7530_set_ageing_time(struct dsa_switch *ds, unsigned int msecs)
{
	struct mt7530_priv *priv = ds->priv;
	unsigned int secs = msecs / 1000;
	unsigned int tmp_age_count;
	unsigned int error = -1;
	unsigned int age_count;
	unsigned int age_unit;

	/* Applied timer is (AGE_CNT + 1) * (AGE_UNIT + 1) seconds */
	if (secs < 1 || secs > (AGE_CNT_MAX + 1) * (AGE_UNIT_MAX + 1))
		return -ERANGE;

	/* iterate through all possible age_count to find the closest pair */
	for (tmp_age_count = 0; tmp_age_count <= AGE_CNT_MAX; ++tmp_age_count) {
		unsigned int tmp_age_unit = secs / (tmp_age_count + 1) - 1;

		if (tmp_age_unit <= AGE_UNIT_MAX) {
			unsigned int tmp_error = secs -
				(tmp_age_count + 1) * (tmp_age_unit + 1);

			/* found a closer pair */
			if (error > tmp_error) {
				error = tmp_error;
				age_count = tmp_age_count;
				age_unit = tmp_age_unit;
			}

			/* found the exact match, so break the loop */
			if (!error)
				break;
		}
	}

	mt7530_write(priv, MT7530_AAC, AGE_CNT(age_count) | AGE_UNIT(age_unit));

	return 0;
}

static void mt7530_setup_port5(struct dsa_switch *ds, phy_interface_t interface)
{
	struct mt7530_priv *priv = ds->priv;
	u8 tx_delay = 0;
	int val;

	mutex_lock(&priv->reg_mutex);

	val = mt7530_read(priv, MT7530_MHWTRAP);

	val |= MHWTRAP_MANUAL | MHWTRAP_P5_MAC_SEL | MHWTRAP_P5_DIS;
	val &= ~MHWTRAP_P5_RGMII_MODE & ~MHWTRAP_PHY0_SEL;

	switch (priv->p5_intf_sel) {
	case P5_INTF_SEL_PHY_P0:
		/* MT7530_P5_MODE_GPHY_P0: 2nd GMAC -> P5 -> P0 */
		val |= MHWTRAP_PHY0_SEL;
		fallthrough;
	case P5_INTF_SEL_PHY_P4:
		/* MT7530_P5_MODE_GPHY_P4: 2nd GMAC -> P5 -> P4 */
		val &= ~MHWTRAP_P5_MAC_SEL & ~MHWTRAP_P5_DIS;

		/* Setup the MAC by default for the cpu port */
		mt7530_write(priv, MT7530_PMCR_P(5), 0x56300);
		break;
	case P5_INTF_SEL_GMAC5:
		/* MT7530_P5_MODE_GMAC: P5 -> External phy or 2nd GMAC */
		val &= ~MHWTRAP_P5_DIS;
		break;
	case P5_DISABLED:
		interface = PHY_INTERFACE_MODE_NA;
		break;
	default:
		dev_err(ds->dev, "Unsupported p5_intf_sel %d\n",
			priv->p5_intf_sel);
		goto unlock_exit;
	}

	/* Setup RGMII settings */
	if (phy_interface_mode_is_rgmii(interface)) {
		val |= MHWTRAP_P5_RGMII_MODE;

		/* P5 RGMII RX Clock Control: delay setting for 1000M */
		mt7530_write(priv, MT7530_P5RGMIIRXCR, CSR_RGMII_EDGE_ALIGN);

		/* Don't set delay in DSA mode */
		if (!dsa_is_dsa_port(priv->ds, 5) &&
		    (interface == PHY_INTERFACE_MODE_RGMII_TXID ||
		     interface == PHY_INTERFACE_MODE_RGMII_ID))
			tx_delay = 4; /* n * 0.5 ns */

		/* P5 RGMII TX Clock Control: delay x */
		mt7530_write(priv, MT7530_P5RGMIITXCR,
			     CSR_RGMII_TXC_CFG(0x10 + tx_delay));

		/* reduce P5 RGMII Tx driving, 8mA */
		mt7530_write(priv, MT7530_IO_DRV_CR,
			     P5_IO_CLK_DRV(1) | P5_IO_DATA_DRV(1));
	}

	mt7530_write(priv, MT7530_MHWTRAP, val);

	dev_dbg(ds->dev, "Setup P5, HWTRAP=0x%x, intf_sel=%s, phy-mode=%s\n",
		val, p5_intf_modes(priv->p5_intf_sel), phy_modes(interface));

	priv->p5_interface = interface;

unlock_exit:
	mutex_unlock(&priv->reg_mutex);
}

static int
mt753x_cpu_port_enable(struct dsa_switch *ds, int port)
{
	struct mt7530_priv *priv = ds->priv;
	int ret;

	/* Setup max capability of CPU port at first */
	if (priv->info->cpu_port_config) {
		ret = priv->info->cpu_port_config(ds, port);
		if (ret)
			return ret;
	}

	/* Enable Mediatek header mode on the cpu port */
	mt7530_write(priv, MT7530_PVC_P(port),
		     PORT_SPEC_TAG);

	/* Disable flooding by default */
	mt7530_rmw(priv, MT7530_MFC, BC_FFP_MASK | UNM_FFP_MASK | UNU_FFP_MASK,
		   BC_FFP(BIT(port)) | UNM_FFP(BIT(port)) | UNU_FFP(BIT(port)));

	/* Set CPU port number */
	if (priv->id == ID_MT7621)
		mt7530_rmw(priv, MT7530_MFC, CPU_MASK, CPU_EN | CPU_PORT(port));

	/* CPU port gets connected to all user ports of
	 * the switch.
	 */
	mt7530_write(priv, MT7530_PCR_P(port),
		     PCR_MATRIX(dsa_user_ports(priv->ds)));

	return 0;
}

static int
mt7530_port_enable(struct dsa_switch *ds, int port,
		   struct phy_device *phy)
{
	struct mt7530_priv *priv = ds->priv;

	if (!dsa_is_user_port(ds, port))
		return 0;

	mutex_lock(&priv->reg_mutex);

	/* Allow the user port gets connected to the cpu port and also
	 * restore the port matrix if the port is the member of a certain
	 * bridge.
	 */
	priv->ports[port].pm |= PCR_MATRIX(BIT(MT7530_CPU_PORT));
	priv->ports[port].enable = true;
	mt7530_rmw(priv, MT7530_PCR_P(port), PCR_MATRIX_MASK,
		   priv->ports[port].pm);
	mt7530_clear(priv, MT7530_PMCR_P(port), PMCR_LINK_SETTINGS_MASK);

	mutex_unlock(&priv->reg_mutex);

	return 0;
}

static void
mt7530_port_disable(struct dsa_switch *ds, int port)
{
	struct mt7530_priv *priv = ds->priv;

	if (!dsa_is_user_port(ds, port))
		return;

	mutex_lock(&priv->reg_mutex);

	/* Clear up all port matrix which could be restored in the next
	 * enablement for the port.
	 */
	priv->ports[port].enable = false;
	mt7530_rmw(priv, MT7530_PCR_P(port), PCR_MATRIX_MASK,
		   PCR_MATRIX_CLR);
	mt7530_clear(priv, MT7530_PMCR_P(port), PMCR_LINK_SETTINGS_MASK);

	mutex_unlock(&priv->reg_mutex);
}

static int
mt7530_port_change_mtu(struct dsa_switch *ds, int port, int new_mtu)
{
	struct mt7530_priv *priv = ds->priv;
	struct mii_bus *bus = priv->bus;
	int length;
	u32 val;

	/* When a new MTU is set, DSA always set the CPU port's MTU to the
	 * largest MTU of the slave ports. Because the switch only has a global
	 * RX length register, only allowing CPU port here is enough.
	 */
	if (!dsa_is_cpu_port(ds, port))
		return 0;

	mutex_lock_nested(&bus->mdio_lock, MDIO_MUTEX_NESTED);

	val = mt7530_mii_read(priv, MT7530_GMACCR);
	val &= ~MAX_RX_PKT_LEN_MASK;

	/* RX length also includes Ethernet header, MTK tag, and FCS length */
	length = new_mtu + ETH_HLEN + MTK_HDR_LEN + ETH_FCS_LEN;
	if (length <= 1522) {
		val |= MAX_RX_PKT_LEN_1522;
	} else if (length <= 1536) {
		val |= MAX_RX_PKT_LEN_1536;
	} else if (length <= 1552) {
		val |= MAX_RX_PKT_LEN_1552;
	} else {
		val &= ~MAX_RX_JUMBO_MASK;
		val |= MAX_RX_JUMBO(DIV_ROUND_UP(length, 1024));
		val |= MAX_RX_PKT_LEN_JUMBO;
	}

	mt7530_mii_write(priv, MT7530_GMACCR, val);

	mutex_unlock(&bus->mdio_lock);

	return 0;
}

static int
mt7530_port_max_mtu(struct dsa_switch *ds, int port)
{
	return MT7530_MAX_MTU;
}

static void
mt7530_stp_state_set(struct dsa_switch *ds, int port, u8 state)
{
	struct mt7530_priv *priv = ds->priv;
	u32 stp_state;

	switch (state) {
	case BR_STATE_DISABLED:
		stp_state = MT7530_STP_DISABLED;
		break;
	case BR_STATE_BLOCKING:
		stp_state = MT7530_STP_BLOCKING;
		break;
	case BR_STATE_LISTENING:
		stp_state = MT7530_STP_LISTENING;
		break;
	case BR_STATE_LEARNING:
		stp_state = MT7530_STP_LEARNING;
		break;
	case BR_STATE_FORWARDING:
	default:
		stp_state = MT7530_STP_FORWARDING;
		break;
	}

	mt7530_rmw(priv, MT7530_SSP_P(port), FID_PST_MASK, stp_state);
}

static int
mt7530_port_pre_bridge_flags(struct dsa_switch *ds, int port,
			     struct switchdev_brport_flags flags,
			     struct netlink_ext_ack *extack)
{
	if (flags.mask & ~(BR_LEARNING | BR_FLOOD | BR_MCAST_FLOOD |
			   BR_BCAST_FLOOD))
		return -EINVAL;

	return 0;
}

static int
mt7530_port_bridge_flags(struct dsa_switch *ds, int port,
			 struct switchdev_brport_flags flags,
			 struct netlink_ext_ack *extack)
{
	struct mt7530_priv *priv = ds->priv;

	if (flags.mask & BR_LEARNING)
		mt7530_rmw(priv, MT7530_PSC_P(port), SA_DIS,
			   flags.val & BR_LEARNING ? 0 : SA_DIS);

	if (flags.mask & BR_FLOOD)
		mt7530_rmw(priv, MT7530_MFC, UNU_FFP(BIT(port)),
			   flags.val & BR_FLOOD ? UNU_FFP(BIT(port)) : 0);

	if (flags.mask & BR_MCAST_FLOOD)
		mt7530_rmw(priv, MT7530_MFC, UNM_FFP(BIT(port)),
			   flags.val & BR_MCAST_FLOOD ? UNM_FFP(BIT(port)) : 0);

	if (flags.mask & BR_BCAST_FLOOD)
		mt7530_rmw(priv, MT7530_MFC, BC_FFP(BIT(port)),
			   flags.val & BR_BCAST_FLOOD ? BC_FFP(BIT(port)) : 0);

	return 0;
}

static int
mt7530_port_set_mrouter(struct dsa_switch *ds, int port, bool mrouter,
			struct netlink_ext_ack *extack)
{
	struct mt7530_priv *priv = ds->priv;

	mt7530_rmw(priv, MT7530_MFC, UNM_FFP(BIT(port)),
		   mrouter ? UNM_FFP(BIT(port)) : 0);

	return 0;
}

static int
mt7530_port_bridge_join(struct dsa_switch *ds, int port,
			struct net_device *bridge)
{
	struct mt7530_priv *priv = ds->priv;
	u32 port_bitmap = BIT(MT7530_CPU_PORT);
	int i;

	mutex_lock(&priv->reg_mutex);

	for (i = 0; i < MT7530_NUM_PORTS; i++) {
		/* Add this port to the port matrix of the other ports in the
		 * same bridge. If the port is disabled, port matrix is kept
		 * and not being setup until the port becomes enabled.
		 */
		if (dsa_is_user_port(ds, i) && i != port) {
			if (dsa_to_port(ds, i)->bridge_dev != bridge)
				continue;
			if (priv->ports[i].enable)
				mt7530_set(priv, MT7530_PCR_P(i),
					   PCR_MATRIX(BIT(port)));
			priv->ports[i].pm |= PCR_MATRIX(BIT(port));

			port_bitmap |= BIT(i);
		}
	}

	/* Add the all other ports to this port matrix. */
	if (priv->ports[port].enable)
		mt7530_rmw(priv, MT7530_PCR_P(port),
			   PCR_MATRIX_MASK, PCR_MATRIX(port_bitmap));
	priv->ports[port].pm |= PCR_MATRIX(port_bitmap);

	mutex_unlock(&priv->reg_mutex);

	return 0;
}

static void
mt7530_port_set_vlan_unaware(struct dsa_switch *ds, int port)
{
	struct mt7530_priv *priv = ds->priv;
	bool all_user_ports_removed = true;
	int i;

	/* When a port is removed from the bridge, the port would be set up
	 * back to the default as is at initial boot which is a VLAN-unaware
	 * port.
	 */
	mt7530_rmw(priv, MT7530_PCR_P(port), PCR_PORT_VLAN_MASK,
		   MT7530_PORT_MATRIX_MODE);
	mt7530_rmw(priv, MT7530_PVC_P(port), VLAN_ATTR_MASK | PVC_EG_TAG_MASK,
		   VLAN_ATTR(MT7530_VLAN_TRANSPARENT) |
		   PVC_EG_TAG(MT7530_VLAN_EG_CONSISTENT));

	for (i = 0; i < MT7530_NUM_PORTS; i++) {
		if (dsa_is_user_port(ds, i) &&
		    dsa_port_is_vlan_filtering(dsa_to_port(ds, i))) {
			all_user_ports_removed = false;
			break;
		}
	}

	/* CPU port also does the same thing until all user ports belonging to
	 * the CPU port get out of VLAN filtering mode.
	 */
	if (all_user_ports_removed) {
		mt7530_write(priv, MT7530_PCR_P(MT7530_CPU_PORT),
			     PCR_MATRIX(dsa_user_ports(priv->ds)));
		mt7530_write(priv, MT7530_PVC_P(MT7530_CPU_PORT), PORT_SPEC_TAG
			     | PVC_EG_TAG(MT7530_VLAN_EG_CONSISTENT));
	}
}

static void
mt7530_port_set_vlan_aware(struct dsa_switch *ds, int port)
{
	struct mt7530_priv *priv = ds->priv;

	/* Trapped into security mode allows packet forwarding through VLAN
	 * table lookup. CPU port is set to fallback mode to let untagged
	 * frames pass through.
	 */
	if (dsa_is_cpu_port(ds, port))
		mt7530_rmw(priv, MT7530_PCR_P(port), PCR_PORT_VLAN_MASK,
			   MT7530_PORT_FALLBACK_MODE);
	else
		mt7530_rmw(priv, MT7530_PCR_P(port), PCR_PORT_VLAN_MASK,
			   MT7530_PORT_SECURITY_MODE);

	/* Set the port as a user port which is to be able to recognize VID
	 * from incoming packets before fetching entry within the VLAN table.
	 */
	mt7530_rmw(priv, MT7530_PVC_P(port), VLAN_ATTR_MASK | PVC_EG_TAG_MASK,
		   VLAN_ATTR(MT7530_VLAN_USER) |
		   PVC_EG_TAG(MT7530_VLAN_EG_DISABLED));
}

static void
mt7530_port_bridge_leave(struct dsa_switch *ds, int port,
			 struct net_device *bridge)
{
	struct mt7530_priv *priv = ds->priv;
	int i;

	mutex_lock(&priv->reg_mutex);

	for (i = 0; i < MT7530_NUM_PORTS; i++) {
		/* Remove this port from the port matrix of the other ports
		 * in the same bridge. If the port is disabled, port matrix
		 * is kept and not being setup until the port becomes enabled.
		 * And the other port's port matrix cannot be broken when the
		 * other port is still a VLAN-aware port.
		 */
		if (dsa_is_user_port(ds, i) && i != port &&
		   !dsa_port_is_vlan_filtering(dsa_to_port(ds, i))) {
			if (dsa_to_port(ds, i)->bridge_dev != bridge)
				continue;
			if (priv->ports[i].enable)
				mt7530_clear(priv, MT7530_PCR_P(i),
					     PCR_MATRIX(BIT(port)));
			priv->ports[i].pm &= ~PCR_MATRIX(BIT(port));
		}
	}

	/* Set the cpu port to be the only one in the port matrix of
	 * this port.
	 */
	if (priv->ports[port].enable)
		mt7530_rmw(priv, MT7530_PCR_P(port), PCR_MATRIX_MASK,
			   PCR_MATRIX(BIT(MT7530_CPU_PORT)));
	priv->ports[port].pm = PCR_MATRIX(BIT(MT7530_CPU_PORT));

	mutex_unlock(&priv->reg_mutex);
}

static int
mt7530_port_fdb_add(struct dsa_switch *ds, int port,
		    const unsigned char *addr, u16 vid)
{
	struct mt7530_priv *priv = ds->priv;
	int ret;
	u8 port_mask = BIT(port);

	mutex_lock(&priv->reg_mutex);
	mt7530_fdb_write(priv, vid, port_mask, addr, -1, STATIC_ENT);
	ret = mt7530_fdb_cmd(priv, MT7530_FDB_WRITE, NULL);
	mutex_unlock(&priv->reg_mutex);

	return ret;
}

static int
mt7530_port_fdb_del(struct dsa_switch *ds, int port,
		    const unsigned char *addr, u16 vid)
{
	struct mt7530_priv *priv = ds->priv;
	int ret;
	u8 port_mask = BIT(port);

	mutex_lock(&priv->reg_mutex);
	mt7530_fdb_write(priv, vid, port_mask, addr, -1, STATIC_EMP);
	ret = mt7530_fdb_cmd(priv, MT7530_FDB_WRITE, NULL);
	mutex_unlock(&priv->reg_mutex);

	return ret;
}

static int
mt7530_port_fdb_dump(struct dsa_switch *ds, int port,
		     dsa_fdb_dump_cb_t *cb, void *data)
{
	struct mt7530_priv *priv = ds->priv;
	struct mt7530_fdb _fdb = { 0 };
	int cnt = MT7530_NUM_FDB_RECORDS;
	int ret = 0;
	u32 rsp = 0;

	mutex_lock(&priv->reg_mutex);

	ret = mt7530_fdb_cmd(priv, MT7530_FDB_START, &rsp);
	if (ret < 0)
		goto err;

	do {
		if (rsp & ATC_SRCH_HIT) {
			mt7530_fdb_read(priv, &_fdb);
			if (_fdb.port_mask & BIT(port)) {
				ret = cb(_fdb.mac, _fdb.vid, _fdb.noarp,
					 data);
				if (ret < 0)
					break;
			}
		}
	} while (--cnt &&
		 !(rsp & ATC_SRCH_END) &&
		 !mt7530_fdb_cmd(priv, MT7530_FDB_NEXT, &rsp));
err:
	mutex_unlock(&priv->reg_mutex);

	return 0;
}

static int
mt7530_port_mdb_add(struct dsa_switch *ds, int port,
		    const struct switchdev_obj_port_mdb *mdb)
{
	struct mt7530_priv *priv = ds->priv;
	const u8 *addr = mdb->addr;
	u16 vid = mdb->vid;
	u8 port_mask = 0;
	int ret;

	mutex_lock(&priv->reg_mutex);

	mt7530_fdb_write(priv, vid, 0, addr, 0, STATIC_EMP);
	if (!mt7530_fdb_cmd(priv, MT7530_FDB_READ, NULL))
		port_mask = (mt7530_read(priv, MT7530_ATRD) >> PORT_MAP)
			    & PORT_MAP_MASK;

	port_mask |= BIT(port);
	mt7530_fdb_write(priv, vid, port_mask, addr, -1, STATIC_ENT);
	ret = mt7530_fdb_cmd(priv, MT7530_FDB_WRITE, NULL);

	mutex_unlock(&priv->reg_mutex);

	return ret;
}

static int
mt7530_port_mdb_del(struct dsa_switch *ds, int port,
		    const struct switchdev_obj_port_mdb *mdb)
{
	struct mt7530_priv *priv = ds->priv;
	const u8 *addr = mdb->addr;
	u16 vid = mdb->vid;
	u8 port_mask = 0;
	int ret;

	mutex_lock(&priv->reg_mutex);

	mt7530_fdb_write(priv, vid, 0, addr, 0, STATIC_EMP);
	if (!mt7530_fdb_cmd(priv, MT7530_FDB_READ, NULL))
		port_mask = (mt7530_read(priv, MT7530_ATRD) >> PORT_MAP)
			    & PORT_MAP_MASK;

	port_mask &= ~BIT(port);
	mt7530_fdb_write(priv, vid, port_mask, addr, -1,
			 port_mask ? STATIC_ENT : STATIC_EMP);
	ret = mt7530_fdb_cmd(priv, MT7530_FDB_WRITE, NULL);

	mutex_unlock(&priv->reg_mutex);

	return ret;
}

static int
mt7530_vlan_cmd(struct mt7530_priv *priv, enum mt7530_vlan_cmd cmd, u16 vid)
{
	struct mt7530_dummy_poll p;
	u32 val;
	int ret;

	val = VTCR_BUSY | VTCR_FUNC(cmd) | vid;
	mt7530_write(priv, MT7530_VTCR, val);

	INIT_MT7530_DUMMY_POLL(&p, priv, MT7530_VTCR);
	ret = readx_poll_timeout(_mt7530_read, &p, val,
				 !(val & VTCR_BUSY), 20, 20000);
	if (ret < 0) {
		dev_err(priv->dev, "poll timeout\n");
		return ret;
	}

	val = mt7530_read(priv, MT7530_VTCR);
	if (val & VTCR_INVALID) {
		dev_err(priv->dev, "read VTCR invalid\n");
		return -EINVAL;
	}

	return 0;
}

static int
mt7530_port_vlan_filtering(struct dsa_switch *ds, int port, bool vlan_filtering,
			   struct netlink_ext_ack *extack)
{
	if (vlan_filtering) {
		/* The port is being kept as VLAN-unaware port when bridge is
		 * set up with vlan_filtering not being set, Otherwise, the
		 * port and the corresponding CPU port is required the setup
		 * for becoming a VLAN-aware port.
		 */
		mt7530_port_set_vlan_aware(ds, port);
		mt7530_port_set_vlan_aware(ds, MT7530_CPU_PORT);
	} else {
		mt7530_port_set_vlan_unaware(ds, port);
	}

	return 0;
}

static void
mt7530_hw_vlan_add(struct mt7530_priv *priv,
		   struct mt7530_hw_vlan_entry *entry)
{
	u8 new_members;
	u32 val;

	new_members = entry->old_members | BIT(entry->port) |
		      BIT(MT7530_CPU_PORT);

	/* Validate the entry with independent learning, create egress tag per
	 * VLAN and joining the port as one of the port members.
	 */
	val = IVL_MAC | VTAG_EN | PORT_MEM(new_members) | VLAN_VALID;
	mt7530_write(priv, MT7530_VAWD1, val);

	/* Decide whether adding tag or not for those outgoing packets from the
	 * port inside the VLAN.
	 */
	val = entry->untagged ? MT7530_VLAN_EGRESS_UNTAG :
				MT7530_VLAN_EGRESS_TAG;
	mt7530_rmw(priv, MT7530_VAWD2,
		   ETAG_CTRL_P_MASK(entry->port),
		   ETAG_CTRL_P(entry->port, val));

	/* CPU port is always taken as a tagged port for serving more than one
	 * VLANs across and also being applied with egress type stack mode for
	 * that VLAN tags would be appended after hardware special tag used as
	 * DSA tag.
	 */
	mt7530_rmw(priv, MT7530_VAWD2,
		   ETAG_CTRL_P_MASK(MT7530_CPU_PORT),
		   ETAG_CTRL_P(MT7530_CPU_PORT,
			       MT7530_VLAN_EGRESS_STACK));
}

static void
mt7530_hw_vlan_del(struct mt7530_priv *priv,
		   struct mt7530_hw_vlan_entry *entry)
{
	u8 new_members;
	u32 val;

	new_members = entry->old_members & ~BIT(entry->port);

	val = mt7530_read(priv, MT7530_VAWD1);
	if (!(val & VLAN_VALID)) {
		dev_err(priv->dev,
			"Cannot be deleted due to invalid entry\n");
		return;
	}

	/* If certain member apart from CPU port is still alive in the VLAN,
	 * the entry would be kept valid. Otherwise, the entry is got to be
	 * disabled.
	 */
	if (new_members && new_members != BIT(MT7530_CPU_PORT)) {
		val = IVL_MAC | VTAG_EN | PORT_MEM(new_members) |
		      VLAN_VALID;
		mt7530_write(priv, MT7530_VAWD1, val);
	} else {
		mt7530_write(priv, MT7530_VAWD1, 0);
		mt7530_write(priv, MT7530_VAWD2, 0);
	}
}

static void
mt7530_hw_vlan_update(struct mt7530_priv *priv, u16 vid,
		      struct mt7530_hw_vlan_entry *entry,
		      mt7530_vlan_op vlan_op)
{
	u32 val;

	/* Fetch entry */
	mt7530_vlan_cmd(priv, MT7530_VTCR_RD_VID, vid);

	val = mt7530_read(priv, MT7530_VAWD1);

	entry->old_members = (val >> PORT_MEM_SHFT) & PORT_MEM_MASK;

	/* Manipulate entry */
	vlan_op(priv, entry);

	/* Flush result to hardware */
	mt7530_vlan_cmd(priv, MT7530_VTCR_WR_VID, vid);
}

static int
mt7530_port_vlan_add(struct dsa_switch *ds, int port,
		     const struct switchdev_obj_port_vlan *vlan,
		     struct netlink_ext_ack *extack)
{
	bool untagged = vlan->flags & BRIDGE_VLAN_INFO_UNTAGGED;
	bool pvid = vlan->flags & BRIDGE_VLAN_INFO_PVID;
	struct mt7530_hw_vlan_entry new_entry;
	struct mt7530_priv *priv = ds->priv;

	mutex_lock(&priv->reg_mutex);

	mt7530_hw_vlan_entry_init(&new_entry, port, untagged);
	mt7530_hw_vlan_update(priv, vlan->vid, &new_entry, mt7530_hw_vlan_add);

	if (pvid) {
		mt7530_rmw(priv, MT7530_PPBV1_P(port), G0_PORT_VID_MASK,
			   G0_PORT_VID(vlan->vid));
		priv->ports[port].pvid = vlan->vid;
	}

	mutex_unlock(&priv->reg_mutex);

	return 0;
}

static int
mt7530_port_vlan_del(struct dsa_switch *ds, int port,
		     const struct switchdev_obj_port_vlan *vlan)
{
	struct mt7530_hw_vlan_entry target_entry;
	struct mt7530_priv *priv = ds->priv;
	u16 pvid;

	mutex_lock(&priv->reg_mutex);

	pvid = priv->ports[port].pvid;
	mt7530_hw_vlan_entry_init(&target_entry, port, 0);
	mt7530_hw_vlan_update(priv, vlan->vid, &target_entry,
			      mt7530_hw_vlan_del);

	/* PVID is being restored to the default whenever the PVID port
	 * is being removed from the VLAN.
	 */
	if (pvid == vlan->vid)
		pvid = G0_PORT_VID_DEF;

	mt7530_rmw(priv, MT7530_PPBV1_P(port), G0_PORT_VID_MASK, pvid);
	priv->ports[port].pvid = pvid;

	mutex_unlock(&priv->reg_mutex);

	return 0;
}

static int mt753x_mirror_port_get(unsigned int id, u32 val)
{
	return (id == ID_MT7531) ? MT7531_MIRROR_PORT_GET(val) :
				   MIRROR_PORT(val);
}

static int mt753x_mirror_port_set(unsigned int id, u32 val)
{
	return (id == ID_MT7531) ? MT7531_MIRROR_PORT_SET(val) :
				   MIRROR_PORT(val);
}

static int mt753x_port_mirror_add(struct dsa_switch *ds, int port,
				  struct dsa_mall_mirror_tc_entry *mirror,
				  bool ingress)
{
	struct mt7530_priv *priv = ds->priv;
	int monitor_port;
	u32 val;

	/* Check for existent entry */
	if ((ingress ? priv->mirror_rx : priv->mirror_tx) & BIT(port))
		return -EEXIST;

	val = mt7530_read(priv, MT753X_MIRROR_REG(priv->id));

	/* MT7530 only supports one monitor port */
	monitor_port = mt753x_mirror_port_get(priv->id, val);
	if (val & MT753X_MIRROR_EN(priv->id) &&
	    monitor_port != mirror->to_local_port)
		return -EEXIST;

	val |= MT753X_MIRROR_EN(priv->id);
	val &= ~MT753X_MIRROR_MASK(priv->id);
	val |= mt753x_mirror_port_set(priv->id, mirror->to_local_port);
	mt7530_write(priv, MT753X_MIRROR_REG(priv->id), val);

	val = mt7530_read(priv, MT7530_PCR_P(port));
	if (ingress) {
		val |= PORT_RX_MIR;
		priv->mirror_rx |= BIT(port);
	} else {
		val |= PORT_TX_MIR;
		priv->mirror_tx |= BIT(port);
	}
	mt7530_write(priv, MT7530_PCR_P(port), val);

	return 0;
}

static void mt753x_port_mirror_del(struct dsa_switch *ds, int port,
				   struct dsa_mall_mirror_tc_entry *mirror)
{
	struct mt7530_priv *priv = ds->priv;
	u32 val;

	val = mt7530_read(priv, MT7530_PCR_P(port));
	if (mirror->ingress) {
		val &= ~PORT_RX_MIR;
		priv->mirror_rx &= ~BIT(port);
	} else {
		val &= ~PORT_TX_MIR;
		priv->mirror_tx &= ~BIT(port);
	}
	mt7530_write(priv, MT7530_PCR_P(port), val);

	if (!priv->mirror_rx && !priv->mirror_tx) {
		val = mt7530_read(priv, MT753X_MIRROR_REG(priv->id));
		val &= ~MT753X_MIRROR_EN(priv->id);
		mt7530_write(priv, MT753X_MIRROR_REG(priv->id), val);
	}
}

static enum dsa_tag_protocol
mtk_get_tag_protocol(struct dsa_switch *ds, int port,
		     enum dsa_tag_protocol mp)
{
	struct mt7530_priv *priv = ds->priv;

	if (port != MT7530_CPU_PORT) {
		dev_warn(priv->dev,
			 "port not matched with tagging CPU port\n");
		return DSA_TAG_PROTO_NONE;
	} else {
		return DSA_TAG_PROTO_MTK;
	}
}

#ifdef CONFIG_GPIOLIB
static inline u32
mt7530_gpio_to_bit(unsigned int offset)
{
	/* Map GPIO offset to register bit
	 * [ 2: 0]  port 0 LED 0..2 as GPIO 0..2
	 * [ 6: 4]  port 1 LED 0..2 as GPIO 3..5
	 * [10: 8]  port 2 LED 0..2 as GPIO 6..8
	 * [14:12]  port 3 LED 0..2 as GPIO 9..11
	 * [18:16]  port 4 LED 0..2 as GPIO 12..14
	 */
	return BIT(offset + offset / 3);
}

static int
mt7530_gpio_get(struct gpio_chip *gc, unsigned int offset)
{
	struct mt7530_priv *priv = gpiochip_get_data(gc);
	u32 bit = mt7530_gpio_to_bit(offset);

	return !!(mt7530_read(priv, MT7530_LED_GPIO_DATA) & bit);
}

static void
mt7530_gpio_set(struct gpio_chip *gc, unsigned int offset, int value)
{
	struct mt7530_priv *priv = gpiochip_get_data(gc);
	u32 bit = mt7530_gpio_to_bit(offset);

	if (value)
		mt7530_set(priv, MT7530_LED_GPIO_DATA, bit);
	else
		mt7530_clear(priv, MT7530_LED_GPIO_DATA, bit);
}

static int
mt7530_gpio_get_direction(struct gpio_chip *gc, unsigned int offset)
{
	struct mt7530_priv *priv = gpiochip_get_data(gc);
	u32 bit = mt7530_gpio_to_bit(offset);

	return (mt7530_read(priv, MT7530_LED_GPIO_DIR) & bit) ?
		GPIO_LINE_DIRECTION_OUT : GPIO_LINE_DIRECTION_IN;
}

static int
mt7530_gpio_direction_input(struct gpio_chip *gc, unsigned int offset)
{
	struct mt7530_priv *priv = gpiochip_get_data(gc);
	u32 bit = mt7530_gpio_to_bit(offset);

	mt7530_clear(priv, MT7530_LED_GPIO_OE, bit);
	mt7530_clear(priv, MT7530_LED_GPIO_DIR, bit);

	return 0;
}

static int
mt7530_gpio_direction_output(struct gpio_chip *gc, unsigned int offset, int value)
{
	struct mt7530_priv *priv = gpiochip_get_data(gc);
	u32 bit = mt7530_gpio_to_bit(offset);

	mt7530_set(priv, MT7530_LED_GPIO_DIR, bit);

	if (value)
		mt7530_set(priv, MT7530_LED_GPIO_DATA, bit);
	else
		mt7530_clear(priv, MT7530_LED_GPIO_DATA, bit);

	mt7530_set(priv, MT7530_LED_GPIO_OE, bit);

	return 0;
}

static int
mt7530_setup_gpio(struct mt7530_priv *priv)
{
	struct device *dev = priv->dev;
	struct gpio_chip *gc;

	gc = devm_kzalloc(dev, sizeof(*gc), GFP_KERNEL);
	if (!gc)
		return -ENOMEM;

	mt7530_write(priv, MT7530_LED_GPIO_OE, 0);
	mt7530_write(priv, MT7530_LED_GPIO_DIR, 0);
	mt7530_write(priv, MT7530_LED_IO_MODE, 0);

	gc->label = "mt7530";
	gc->parent = dev;
	gc->owner = THIS_MODULE;
	gc->get_direction = mt7530_gpio_get_direction;
	gc->direction_input = mt7530_gpio_direction_input;
	gc->direction_output = mt7530_gpio_direction_output;
	gc->get = mt7530_gpio_get;
	gc->set = mt7530_gpio_set;
	gc->base = -1;
	gc->ngpio = 15;
	gc->can_sleep = true;

	return devm_gpiochip_add_data(dev, gc, priv);
}
#endif /* CONFIG_GPIOLIB */

static int
mt7530_setup(struct dsa_switch *ds)
{
	struct mt7530_priv *priv = ds->priv;
	struct device_node *phy_node;
	struct device_node *mac_np;
	struct mt7530_dummy_poll p;
	phy_interface_t interface;
	struct device_node *dn;
	u32 id, val;
	int ret, i;

	/* The parent node of master netdev which holds the common system
	 * controller also is the container for two GMACs nodes representing
	 * as two netdev instances.
	 */
	dn = dsa_to_port(ds, MT7530_CPU_PORT)->master->dev.of_node->parent;
	ds->mtu_enforcement_ingress = true;

	if (priv->id == ID_MT7530) {
		regulator_set_voltage(priv->core_pwr, 1000000, 1000000);
		ret = regulator_enable(priv->core_pwr);
		if (ret < 0) {
			dev_err(priv->dev,
				"Failed to enable core power: %d\n", ret);
			return ret;
		}

		regulator_set_voltage(priv->io_pwr, 3300000, 3300000);
		ret = regulator_enable(priv->io_pwr);
		if (ret < 0) {
			dev_err(priv->dev, "Failed to enable io pwr: %d\n",
				ret);
			return ret;
		}
	}

	/* Reset whole chip through gpio pin or memory-mapped registers for
	 * different type of hardware
	 */
	if (priv->mcm) {
		reset_control_assert(priv->rstc);
		usleep_range(1000, 1100);
		reset_control_deassert(priv->rstc);
	} else {
		gpiod_set_value_cansleep(priv->reset, 0);
		usleep_range(1000, 1100);
		gpiod_set_value_cansleep(priv->reset, 1);
	}

	/* Waiting for MT7530 got to stable */
	INIT_MT7530_DUMMY_POLL(&p, priv, MT7530_HWTRAP);
	ret = readx_poll_timeout(_mt7530_read, &p, val, val != 0,
				 20, 1000000);
	if (ret < 0) {
		dev_err(priv->dev, "reset timeout\n");
		return ret;
	}

	id = mt7530_read(priv, MT7530_CREV);
	id >>= CHIP_NAME_SHIFT;
	if (id != MT7530_ID) {
		dev_err(priv->dev, "chip %x can't be supported\n", id);
		return -ENODEV;
	}

	/* Reset the switch through internal reset */
	mt7530_write(priv, MT7530_SYS_CTRL,
		     SYS_CTRL_PHY_RST | SYS_CTRL_SW_RST |
		     SYS_CTRL_REG_RST);

	/* Enable Port 6 only; P5 as GMAC5 which currently is not supported */
	val = mt7530_read(priv, MT7530_MHWTRAP);
	val &= ~MHWTRAP_P6_DIS & ~MHWTRAP_PHY_ACCESS;
	val |= MHWTRAP_MANUAL;
	mt7530_write(priv, MT7530_MHWTRAP, val);

	priv->p6_interface = PHY_INTERFACE_MODE_NA;

	/* Enable and reset MIB counters */
	mt7530_mib_reset(ds);

	for (i = 0; i < MT7530_NUM_PORTS; i++) {
		/* Disable forwarding by default on all ports */
		mt7530_rmw(priv, MT7530_PCR_P(i), PCR_MATRIX_MASK,
			   PCR_MATRIX_CLR);

		if (dsa_is_cpu_port(ds, i)) {
			ret = mt753x_cpu_port_enable(ds, i);
			if (ret)
				return ret;
		} else {
			mt7530_port_disable(ds, i);

			/* Disable learning by default on all user ports */
			mt7530_set(priv, MT7530_PSC_P(i), SA_DIS);
		}
		/* Enable consistent egress tag */
		mt7530_rmw(priv, MT7530_PVC_P(i), PVC_EG_TAG_MASK,
			   PVC_EG_TAG(MT7530_VLAN_EG_CONSISTENT));
	}

	/* Setup port 5 */
	priv->p5_intf_sel = P5_DISABLED;
	interface = PHY_INTERFACE_MODE_NA;

	if (!dsa_is_unused_port(ds, 5)) {
		priv->p5_intf_sel = P5_INTF_SEL_GMAC5;
		ret = of_get_phy_mode(dsa_to_port(ds, 5)->dn, &interface);
		if (ret && ret != -ENODEV)
			return ret;
	} else {
		/* Scan the ethernet nodes. look for GMAC1, lookup used phy */
		for_each_child_of_node(dn, mac_np) {
			if (!of_device_is_compatible(mac_np,
						     "mediatek,eth-mac"))
				continue;

			ret = of_property_read_u32(mac_np, "reg", &id);
			if (ret < 0 || id != 1)
				continue;

			phy_node = of_parse_phandle(mac_np, "phy-handle", 0);
			if (!phy_node)
				continue;

			if (phy_node->parent == priv->dev->of_node->parent) {
				ret = of_get_phy_mode(mac_np, &interface);
				if (ret && ret != -ENODEV) {
					of_node_put(mac_np);
					return ret;
				}
				id = of_mdio_parse_addr(ds->dev, phy_node);
				if (id == 0)
					priv->p5_intf_sel = P5_INTF_SEL_PHY_P0;
				if (id == 4)
					priv->p5_intf_sel = P5_INTF_SEL_PHY_P4;
			}
			of_node_put(mac_np);
			of_node_put(phy_node);
			break;
		}
	}

#ifdef CONFIG_GPIOLIB
	if (of_property_read_bool(priv->dev->of_node, "gpio-controller")) {
		ret = mt7530_setup_gpio(priv);
		if (ret)
			return ret;
	}
#endif /* CONFIG_GPIOLIB */

	mt7530_setup_port5(ds, interface);

	/* Flush the FDB table */
	ret = mt7530_fdb_cmd(priv, MT7530_FDB_FLUSH, NULL);
	if (ret < 0)
		return ret;

	return 0;
}

static int
mt7531_setup(struct dsa_switch *ds)
{
	struct mt7530_priv *priv = ds->priv;
	struct mt7530_dummy_poll p;
	u32 val, id;
	int ret, i;

	/* Reset whole chip through gpio pin or memory-mapped registers for
	 * different type of hardware
	 */
	if (priv->mcm) {
		reset_control_assert(priv->rstc);
		usleep_range(1000, 1100);
		reset_control_deassert(priv->rstc);
	} else {
		gpiod_set_value_cansleep(priv->reset, 0);
		usleep_range(1000, 1100);
		gpiod_set_value_cansleep(priv->reset, 1);
	}

	/* Waiting for MT7530 got to stable */
	INIT_MT7530_DUMMY_POLL(&p, priv, MT7530_HWTRAP);
	ret = readx_poll_timeout(_mt7530_read, &p, val, val != 0,
				 20, 1000000);
	if (ret < 0) {
		dev_err(priv->dev, "reset timeout\n");
		return ret;
	}

	id = mt7530_read(priv, MT7531_CREV);
	id >>= CHIP_NAME_SHIFT;

	if (id != MT7531_ID) {
		dev_err(priv->dev, "chip %x can't be supported\n", id);
		return -ENODEV;
	}

	/* Reset the switch through internal reset */
	mt7530_write(priv, MT7530_SYS_CTRL,
		     SYS_CTRL_PHY_RST | SYS_CTRL_SW_RST |
		     SYS_CTRL_REG_RST);

	if (mt7531_dual_sgmii_supported(priv)) {
		priv->p5_intf_sel = P5_INTF_SEL_GMAC5_SGMII;

		/* Let ds->slave_mii_bus be able to access external phy. */
		mt7530_rmw(priv, MT7531_GPIO_MODE1, MT7531_GPIO11_RG_RXD2_MASK,
			   MT7531_EXT_P_MDC_11);
		mt7530_rmw(priv, MT7531_GPIO_MODE1, MT7531_GPIO12_RG_RXD3_MASK,
			   MT7531_EXT_P_MDIO_12);
	} else {
		priv->p5_intf_sel = P5_INTF_SEL_GMAC5;
	}
	dev_dbg(ds->dev, "P5 support %s interface\n",
		p5_intf_modes(priv->p5_intf_sel));

	mt7530_rmw(priv, MT7531_GPIO_MODE0, MT7531_GPIO0_MASK,
		   MT7531_GPIO0_INTERRUPT);

	/* Let phylink decide the interface later. */
	priv->p5_interface = PHY_INTERFACE_MODE_NA;
	priv->p6_interface = PHY_INTERFACE_MODE_NA;

	/* Enable PHY core PLL, since phy_device has not yet been created
	 * provided for phy_[read,write]_mmd_indirect is called, we provide
	 * our own mt7531_ind_mmd_phy_[read,write] to complete this
	 * function.
	 */
	val = mt7531_ind_c45_phy_read(priv, MT753X_CTRL_PHY_ADDR,
				      MDIO_MMD_VEND2, CORE_PLL_GROUP4);
	val |= MT7531_PHY_PLL_BYPASS_MODE;
	val &= ~MT7531_PHY_PLL_OFF;
	mt7531_ind_c45_phy_write(priv, MT753X_CTRL_PHY_ADDR, MDIO_MMD_VEND2,
				 CORE_PLL_GROUP4, val);

	/* BPDU to CPU port */
	mt7530_rmw(priv, MT7531_CFC, MT7531_CPU_PMAP_MASK,
		   BIT(MT7530_CPU_PORT));
	mt7530_rmw(priv, MT753X_BPC, MT753X_BPDU_PORT_FW_MASK,
		   MT753X_BPDU_CPU_ONLY);

	/* Enable and reset MIB counters */
	mt7530_mib_reset(ds);

	for (i = 0; i < MT7530_NUM_PORTS; i++) {
		/* Disable forwarding by default on all ports */
		mt7530_rmw(priv, MT7530_PCR_P(i), PCR_MATRIX_MASK,
			   PCR_MATRIX_CLR);

		mt7530_set(priv, MT7531_DBG_CNT(i), MT7531_DIS_CLR);

		if (dsa_is_cpu_port(ds, i)) {
			ret = mt753x_cpu_port_enable(ds, i);
			if (ret)
				return ret;
		} else {
			mt7530_port_disable(ds, i);

			/* Disable learning by default on all user ports */
			mt7530_set(priv, MT7530_PSC_P(i), SA_DIS);
		}

		/* Enable consistent egress tag */
		mt7530_rmw(priv, MT7530_PVC_P(i), PVC_EG_TAG_MASK,
			   PVC_EG_TAG(MT7530_VLAN_EG_CONSISTENT));
	}

	ds->mtu_enforcement_ingress = true;

	/* Flush the FDB table */
	ret = mt7530_fdb_cmd(priv, MT7530_FDB_FLUSH, NULL);
	if (ret < 0)
		return ret;

	return 0;
}

static bool
mt7530_phy_mode_supported(struct dsa_switch *ds, int port,
			  const struct phylink_link_state *state)
{
	struct mt7530_priv *priv = ds->priv;

	switch (port) {
	case 0 ... 4: /* Internal phy */
		if (state->interface != PHY_INTERFACE_MODE_GMII)
			return false;
		break;
	case 5: /* 2nd cpu port with phy of port 0 or 4 / external phy */
		if (!phy_interface_mode_is_rgmii(state->interface) &&
		    state->interface != PHY_INTERFACE_MODE_MII &&
		    state->interface != PHY_INTERFACE_MODE_GMII)
			return false;
		break;
	case 6: /* 1st cpu port */
		if (state->interface != PHY_INTERFACE_MODE_RGMII &&
		    state->interface != PHY_INTERFACE_MODE_TRGMII)
			return false;
		break;
	default:
		dev_err(priv->dev, "%s: unsupported port: %i\n", __func__,
			port);
		return false;
	}

	return true;
}

static bool mt7531_is_rgmii_port(struct mt7530_priv *priv, u32 port)
{
	return (port == 5) && (priv->p5_intf_sel != P5_INTF_SEL_GMAC5_SGMII);
}

static bool
mt7531_phy_mode_supported(struct dsa_switch *ds, int port,
			  const struct phylink_link_state *state)
{
	struct mt7530_priv *priv = ds->priv;

	switch (port) {
	case 0 ... 4: /* Internal phy */
		if (state->interface != PHY_INTERFACE_MODE_GMII)
			return false;
		break;
	case 5: /* 2nd cpu port supports either rgmii or sgmii/8023z */
		if (mt7531_is_rgmii_port(priv, port))
			return phy_interface_mode_is_rgmii(state->interface);
		fallthrough;
	case 6: /* 1st cpu port supports sgmii/8023z only */
		if (state->interface != PHY_INTERFACE_MODE_SGMII &&
		    !phy_interface_mode_is_8023z(state->interface))
			return false;
		break;
	default:
		dev_err(priv->dev, "%s: unsupported port: %i\n", __func__,
			port);
		return false;
	}

	return true;
}

static bool
mt753x_phy_mode_supported(struct dsa_switch *ds, int port,
			  const struct phylink_link_state *state)
{
	struct mt7530_priv *priv = ds->priv;

	return priv->info->phy_mode_supported(ds, port, state);
}

static int
mt753x_pad_setup(struct dsa_switch *ds, const struct phylink_link_state *state)
{
	struct mt7530_priv *priv = ds->priv;

	return priv->info->pad_setup(ds, state->interface);
}

static int
mt7530_mac_config(struct dsa_switch *ds, int port, unsigned int mode,
		  phy_interface_t interface)
{
	struct mt7530_priv *priv = ds->priv;

	/* Only need to setup port5. */
	if (port != 5)
		return 0;

	mt7530_setup_port5(priv->ds, interface);

	return 0;
}

static int mt7531_rgmii_setup(struct mt7530_priv *priv, u32 port,
			      phy_interface_t interface,
			      struct phy_device *phydev)
{
	u32 val;

	if (!mt7531_is_rgmii_port(priv, port)) {
		dev_err(priv->dev, "RGMII mode is not available for port %d\n",
			port);
		return -EINVAL;
	}

	val = mt7530_read(priv, MT7531_CLKGEN_CTRL);
	val |= GP_CLK_EN;
	val &= ~GP_MODE_MASK;
	val |= GP_MODE(MT7531_GP_MODE_RGMII);
	val &= ~CLK_SKEW_IN_MASK;
	val |= CLK_SKEW_IN(MT7531_CLK_SKEW_NO_CHG);
	val &= ~CLK_SKEW_OUT_MASK;
	val |= CLK_SKEW_OUT(MT7531_CLK_SKEW_NO_CHG);
	val |= TXCLK_NO_REVERSE | RXCLK_NO_DELAY;

	/* Do not adjust rgmii delay when vendor phy driver presents. */
	if (!phydev || phy_driver_is_genphy(phydev)) {
		val &= ~(TXCLK_NO_REVERSE | RXCLK_NO_DELAY);
		switch (interface) {
		case PHY_INTERFACE_MODE_RGMII:
			val |= TXCLK_NO_REVERSE;
			val |= RXCLK_NO_DELAY;
			break;
		case PHY_INTERFACE_MODE_RGMII_RXID:
			val |= TXCLK_NO_REVERSE;
			break;
		case PHY_INTERFACE_MODE_RGMII_TXID:
			val |= RXCLK_NO_DELAY;
			break;
		case PHY_INTERFACE_MODE_RGMII_ID:
			break;
		default:
			return -EINVAL;
		}
	}
	mt7530_write(priv, MT7531_CLKGEN_CTRL, val);

	return 0;
}

static void mt7531_sgmii_validate(struct mt7530_priv *priv, int port,
				  unsigned long *supported)
{
	/* Port5 supports ethier RGMII or SGMII.
	 * Port6 supports SGMII only.
	 */
	switch (port) {
	case 5:
		if (mt7531_is_rgmii_port(priv, port))
			break;
		fallthrough;
	case 6:
		phylink_set(supported, 1000baseX_Full);
		phylink_set(supported, 2500baseX_Full);
		phylink_set(supported, 2500baseT_Full);
	}
}

static void
mt7531_sgmii_link_up_force(struct dsa_switch *ds, int port,
			   unsigned int mode, phy_interface_t interface,
			   int speed, int duplex)
{
	struct mt7530_priv *priv = ds->priv;
	unsigned int val;

	/* For adjusting speed and duplex of SGMII force mode. */
	if (interface != PHY_INTERFACE_MODE_SGMII ||
	    phylink_autoneg_inband(mode))
		return;

	/* SGMII force mode setting */
	val = mt7530_read(priv, MT7531_SGMII_MODE(port));
	val &= ~MT7531_SGMII_IF_MODE_MASK;

	switch (speed) {
	case SPEED_10:
		val |= MT7531_SGMII_FORCE_SPEED_10;
		break;
	case SPEED_100:
		val |= MT7531_SGMII_FORCE_SPEED_100;
		break;
	case SPEED_1000:
		val |= MT7531_SGMII_FORCE_SPEED_1000;
		break;
	}

	/* MT7531 SGMII 1G force mode can only work in full duplex mode,
	 * no matter MT7531_SGMII_FORCE_HALF_DUPLEX is set or not.
	 */
	if ((speed == SPEED_10 || speed == SPEED_100) &&
	    duplex != DUPLEX_FULL)
		val |= MT7531_SGMII_FORCE_HALF_DUPLEX;

	mt7530_write(priv, MT7531_SGMII_MODE(port), val);
}

static bool mt753x_is_mac_port(u32 port)
{
	return (port == 5 || port == 6);
}

static int mt7531_sgmii_setup_mode_force(struct mt7530_priv *priv, u32 port,
					 phy_interface_t interface)
{
	u32 val;

	if (!mt753x_is_mac_port(port))
		return -EINVAL;

	mt7530_set(priv, MT7531_QPHY_PWR_STATE_CTRL(port),
		   MT7531_SGMII_PHYA_PWD);

	val = mt7530_read(priv, MT7531_PHYA_CTRL_SIGNAL3(port));
	val &= ~MT7531_RG_TPHY_SPEED_MASK;
	/* Setup 2.5 times faster clock for 2.5Gbps data speeds with 10B/8B
	 * encoding.
	 */
	val |= (interface == PHY_INTERFACE_MODE_2500BASEX) ?
		MT7531_RG_TPHY_SPEED_3_125G : MT7531_RG_TPHY_SPEED_1_25G;
	mt7530_write(priv, MT7531_PHYA_CTRL_SIGNAL3(port), val);

	mt7530_clear(priv, MT7531_PCS_CONTROL_1(port), MT7531_SGMII_AN_ENABLE);

	/* MT7531 SGMII 1G and 2.5G force mode can only work in full duplex
	 * mode, no matter MT7531_SGMII_FORCE_HALF_DUPLEX is set or not.
	 */
	mt7530_rmw(priv, MT7531_SGMII_MODE(port),
		   MT7531_SGMII_IF_MODE_MASK | MT7531_SGMII_REMOTE_FAULT_DIS,
		   MT7531_SGMII_FORCE_SPEED_1000);

	mt7530_write(priv, MT7531_QPHY_PWR_STATE_CTRL(port), 0);

	return 0;
}

static int mt7531_sgmii_setup_mode_an(struct mt7530_priv *priv, int port,
				      phy_interface_t interface)
{
	if (!mt753x_is_mac_port(port))
		return -EINVAL;

	mt7530_set(priv, MT7531_QPHY_PWR_STATE_CTRL(port),
		   MT7531_SGMII_PHYA_PWD);

	mt7530_rmw(priv, MT7531_PHYA_CTRL_SIGNAL3(port),
		   MT7531_RG_TPHY_SPEED_MASK, MT7531_RG_TPHY_SPEED_1_25G);

	mt7530_set(priv, MT7531_SGMII_MODE(port),
		   MT7531_SGMII_REMOTE_FAULT_DIS |
		   MT7531_SGMII_SPEED_DUPLEX_AN);

	mt7530_rmw(priv, MT7531_PCS_SPEED_ABILITY(port),
		   MT7531_SGMII_TX_CONFIG_MASK, 1);

	mt7530_set(priv, MT7531_PCS_CONTROL_1(port), MT7531_SGMII_AN_ENABLE);

	mt7530_set(priv, MT7531_PCS_CONTROL_1(port), MT7531_SGMII_AN_RESTART);

	mt7530_write(priv, MT7531_QPHY_PWR_STATE_CTRL(port), 0);

	return 0;
}

static void mt7531_sgmii_restart_an(struct dsa_switch *ds, int port)
{
	struct mt7530_priv *priv = ds->priv;
	u32 val;

	/* Only restart AN when AN is enabled */
	val = mt7530_read(priv, MT7531_PCS_CONTROL_1(port));
	if (val & MT7531_SGMII_AN_ENABLE) {
		val |= MT7531_SGMII_AN_RESTART;
		mt7530_write(priv, MT7531_PCS_CONTROL_1(port), val);
	}
}

static int
mt7531_mac_config(struct dsa_switch *ds, int port, unsigned int mode,
		  phy_interface_t interface)
{
	struct mt7530_priv *priv = ds->priv;
	struct phy_device *phydev;
	struct dsa_port *dp;

	if (!mt753x_is_mac_port(port)) {
		dev_err(priv->dev, "port %d is not a MAC port\n", port);
		return -EINVAL;
	}

	switch (interface) {
	case PHY_INTERFACE_MODE_RGMII:
	case PHY_INTERFACE_MODE_RGMII_ID:
	case PHY_INTERFACE_MODE_RGMII_RXID:
	case PHY_INTERFACE_MODE_RGMII_TXID:
		dp = dsa_to_port(ds, port);
		phydev = dp->slave->phydev;
		return mt7531_rgmii_setup(priv, port, interface, phydev);
	case PHY_INTERFACE_MODE_SGMII:
		return mt7531_sgmii_setup_mode_an(priv, port, interface);
	case PHY_INTERFACE_MODE_NA:
	case PHY_INTERFACE_MODE_1000BASEX:
	case PHY_INTERFACE_MODE_2500BASEX:
		if (phylink_autoneg_inband(mode))
			return -EINVAL;

		return mt7531_sgmii_setup_mode_force(priv, port, interface);
	default:
		return -EINVAL;
	}

	return -EINVAL;
}

static int
mt753x_mac_config(struct dsa_switch *ds, int port, unsigned int mode,
		  const struct phylink_link_state *state)
{
	struct mt7530_priv *priv = ds->priv;

	return priv->info->mac_port_config(ds, port, mode, state->interface);
}

static void
mt753x_phylink_mac_config(struct dsa_switch *ds, int port, unsigned int mode,
			  const struct phylink_link_state *state)
{
	struct mt7530_priv *priv = ds->priv;
	u32 mcr_cur, mcr_new;

	if (!mt753x_phy_mode_supported(ds, port, state))
		goto unsupported;

	switch (port) {
	case 0 ... 4: /* Internal phy */
		if (state->interface != PHY_INTERFACE_MODE_GMII)
			goto unsupported;
		break;
	case 5: /* 2nd cpu port with phy of port 0 or 4 / external phy */
		if (priv->p5_interface == state->interface)
			break;

		if (mt753x_mac_config(ds, port, mode, state) < 0)
			goto unsupported;

		if (priv->p5_intf_sel != P5_DISABLED)
			priv->p5_interface = state->interface;
		break;
	case 6: /* 1st cpu port */
		if (priv->p6_interface == state->interface)
			break;

		mt753x_pad_setup(ds, state);

		if (mt753x_mac_config(ds, port, mode, state) < 0)
			goto unsupported;

		priv->p6_interface = state->interface;
		break;
	default:
unsupported:
		dev_err(ds->dev, "%s: unsupported %s port: %i\n",
			__func__, phy_modes(state->interface), port);
		return;
	}

	if (phylink_autoneg_inband(mode) &&
	    state->interface != PHY_INTERFACE_MODE_SGMII) {
		dev_err(ds->dev, "%s: in-band negotiation unsupported\n",
			__func__);
		return;
	}

	mcr_cur = mt7530_read(priv, MT7530_PMCR_P(port));
	mcr_new = mcr_cur;
	mcr_new &= ~PMCR_LINK_SETTINGS_MASK;
	mcr_new |= PMCR_IFG_XMIT(1) | PMCR_MAC_MODE | PMCR_BACKOFF_EN |
		   PMCR_BACKPR_EN | PMCR_FORCE_MODE_ID(priv->id);

	/* Are we connected to external phy */
	if (port == 5 && dsa_is_user_port(ds, 5))
		mcr_new |= PMCR_EXT_PHY;

	if (mcr_new != mcr_cur)
		mt7530_write(priv, MT7530_PMCR_P(port), mcr_new);
}

static void
mt753x_phylink_mac_an_restart(struct dsa_switch *ds, int port)
{
	struct mt7530_priv *priv = ds->priv;

	if (!priv->info->mac_pcs_an_restart)
		return;

	priv->info->mac_pcs_an_restart(ds, port);
}

static void mt753x_phylink_mac_link_down(struct dsa_switch *ds, int port,
					 unsigned int mode,
					 phy_interface_t interface)
{
	struct mt7530_priv *priv = ds->priv;

	mt7530_clear(priv, MT7530_PMCR_P(port), PMCR_LINK_SETTINGS_MASK);
}

static void mt753x_mac_pcs_link_up(struct dsa_switch *ds, int port,
				   unsigned int mode, phy_interface_t interface,
				   int speed, int duplex)
{
	struct mt7530_priv *priv = ds->priv;

	if (!priv->info->mac_pcs_link_up)
		return;

	priv->info->mac_pcs_link_up(ds, port, mode, interface, speed, duplex);
}

static void mt753x_phylink_mac_link_up(struct dsa_switch *ds, int port,
				       unsigned int mode,
				       phy_interface_t interface,
				       struct phy_device *phydev,
				       int speed, int duplex,
				       bool tx_pause, bool rx_pause)
{
	struct mt7530_priv *priv = ds->priv;
	u32 mcr;

	mt753x_mac_pcs_link_up(ds, port, mode, interface, speed, duplex);

	mcr = PMCR_RX_EN | PMCR_TX_EN | PMCR_FORCE_LNK;

	/* MT753x MAC works in 1G full duplex mode for all up-clocked
	 * variants.
	 */
	if (interface == PHY_INTERFACE_MODE_TRGMII ||
	    (phy_interface_mode_is_8023z(interface))) {
		speed = SPEED_1000;
		duplex = DUPLEX_FULL;
	}

	switch (speed) {
	case SPEED_1000:
		mcr |= PMCR_FORCE_SPEED_1000;
		break;
	case SPEED_100:
		mcr |= PMCR_FORCE_SPEED_100;
		break;
	}
	if (duplex == DUPLEX_FULL) {
		mcr |= PMCR_FORCE_FDX;
		if (tx_pause)
			mcr |= PMCR_TX_FC_EN;
		if (rx_pause)
			mcr |= PMCR_RX_FC_EN;
	}

	if (mode == MLO_AN_PHY && phydev && phy_init_eee(phydev, 0) >= 0) {
		switch (speed) {
		case SPEED_1000:
			mcr |= PMCR_FORCE_EEE1G;
			break;
		case SPEED_100:
			mcr |= PMCR_FORCE_EEE100;
			break;
		}
	}

	mt7530_set(priv, MT7530_PMCR_P(port), mcr);
}

static int
mt7531_cpu_port_config(struct dsa_switch *ds, int port)
{
	struct mt7530_priv *priv = ds->priv;
	phy_interface_t interface;
	int speed;
	int ret;

	switch (port) {
	case 5:
		if (mt7531_is_rgmii_port(priv, port))
			interface = PHY_INTERFACE_MODE_RGMII;
		else
			interface = PHY_INTERFACE_MODE_2500BASEX;

		priv->p5_interface = interface;
		break;
	case 6:
		interface = PHY_INTERFACE_MODE_2500BASEX;

		mt7531_pad_setup(ds, interface);

		priv->p6_interface = interface;
		break;
	default:
		return -EINVAL;
	}

	if (interface == PHY_INTERFACE_MODE_2500BASEX)
		speed = SPEED_2500;
	else
		speed = SPEED_1000;

	ret = mt7531_mac_config(ds, port, MLO_AN_FIXED, interface);
	if (ret)
		return ret;
	mt7530_write(priv, MT7530_PMCR_P(port),
		     PMCR_CPU_PORT_SETTING(priv->id));
	mt753x_phylink_mac_link_up(ds, port, MLO_AN_FIXED, interface, NULL,
				   speed, DUPLEX_FULL, true, true);

	return 0;
}

static void
mt7530_mac_port_validate(struct dsa_switch *ds, int port,
			 unsigned long *supported)
{
	if (port == 5)
		phylink_set(supported, 1000baseX_Full);
}

static void mt7531_mac_port_validate(struct dsa_switch *ds, int port,
				     unsigned long *supported)
{
	struct mt7530_priv *priv = ds->priv;

	mt7531_sgmii_validate(priv, port, supported);
}

static void
mt753x_phylink_validate(struct dsa_switch *ds, int port,
			unsigned long *supported,
			struct phylink_link_state *state)
{
	__ETHTOOL_DECLARE_LINK_MODE_MASK(mask) = { 0, };
	struct mt7530_priv *priv = ds->priv;

	if (state->interface != PHY_INTERFACE_MODE_NA &&
	    !mt753x_phy_mode_supported(ds, port, state)) {
		linkmode_zero(supported);
		return;
	}

	phylink_set_port_modes(mask);

	if (state->interface != PHY_INTERFACE_MODE_TRGMII ||
	    !phy_interface_mode_is_8023z(state->interface)) {
		phylink_set(mask, 10baseT_Half);
		phylink_set(mask, 10baseT_Full);
		phylink_set(mask, 100baseT_Half);
		phylink_set(mask, 100baseT_Full);
		phylink_set(mask, Autoneg);
	}

	/* This switch only supports 1G full-duplex. */
	if (state->interface != PHY_INTERFACE_MODE_MII)
		phylink_set(mask, 1000baseT_Full);

	priv->info->mac_port_validate(ds, port, mask);

	phylink_set(mask, Pause);
	phylink_set(mask, Asym_Pause);

	linkmode_and(supported, supported, mask);
	linkmode_and(state->advertising, state->advertising, mask);

	/* We can only operate at 2500BaseX or 1000BaseX.  If requested
	 * to advertise both, only report advertising at 2500BaseX.
	 */
	phylink_helper_basex_speed(state);
}

static int
mt7530_phylink_mac_link_state(struct dsa_switch *ds, int port,
			      struct phylink_link_state *state)
{
	struct mt7530_priv *priv = ds->priv;
	u32 pmsr;

	if (port < 0 || port >= MT7530_NUM_PORTS)
		return -EINVAL;

	pmsr = mt7530_read(priv, MT7530_PMSR_P(port));

	state->link = (pmsr & PMSR_LINK);
	state->an_complete = state->link;
	state->duplex = !!(pmsr & PMSR_DPX);

	switch (pmsr & PMSR_SPEED_MASK) {
	case PMSR_SPEED_10:
		state->speed = SPEED_10;
		break;
	case PMSR_SPEED_100:
		state->speed = SPEED_100;
		break;
	case PMSR_SPEED_1000:
		state->speed = SPEED_1000;
		break;
	default:
		state->speed = SPEED_UNKNOWN;
		break;
	}

	state->pause &= ~(MLO_PAUSE_RX | MLO_PAUSE_TX);
	if (pmsr & PMSR_RX_FC)
		state->pause |= MLO_PAUSE_RX;
	if (pmsr & PMSR_TX_FC)
		state->pause |= MLO_PAUSE_TX;

	return 1;
}

static int
mt7531_sgmii_pcs_get_state_an(struct mt7530_priv *priv, int port,
			      struct phylink_link_state *state)
{
	u32 status, val;
	u16 config_reg;

	status = mt7530_read(priv, MT7531_PCS_CONTROL_1(port));
	state->link = !!(status & MT7531_SGMII_LINK_STATUS);
	if (state->interface == PHY_INTERFACE_MODE_SGMII &&
	    (status & MT7531_SGMII_AN_ENABLE)) {
		val = mt7530_read(priv, MT7531_PCS_SPEED_ABILITY(port));
		config_reg = val >> 16;

		switch (config_reg & LPA_SGMII_SPD_MASK) {
		case LPA_SGMII_1000:
			state->speed = SPEED_1000;
			break;
		case LPA_SGMII_100:
			state->speed = SPEED_100;
			break;
		case LPA_SGMII_10:
			state->speed = SPEED_10;
			break;
		default:
			dev_err(priv->dev, "invalid sgmii PHY speed\n");
			state->link = false;
			return -EINVAL;
		}

		if (config_reg & LPA_SGMII_FULL_DUPLEX)
			state->duplex = DUPLEX_FULL;
		else
			state->duplex = DUPLEX_HALF;
	}

	return 0;
}

static int
mt7531_phylink_mac_link_state(struct dsa_switch *ds, int port,
			      struct phylink_link_state *state)
{
	struct mt7530_priv *priv = ds->priv;

	if (state->interface == PHY_INTERFACE_MODE_SGMII)
		return mt7531_sgmii_pcs_get_state_an(priv, port, state);

	return -EOPNOTSUPP;
}

static int
mt753x_phylink_mac_link_state(struct dsa_switch *ds, int port,
			      struct phylink_link_state *state)
{
	struct mt7530_priv *priv = ds->priv;

	return priv->info->mac_port_get_state(ds, port, state);
}

static int
mt753x_setup(struct dsa_switch *ds)
{
	struct mt7530_priv *priv = ds->priv;

	return priv->info->sw_setup(ds);
}

static int
mt753x_phy_read(struct dsa_switch *ds, int port, int regnum)
{
	struct mt7530_priv *priv = ds->priv;

	return priv->info->phy_read(ds, port, regnum);
}

static int
mt753x_phy_write(struct dsa_switch *ds, int port, int regnum, u16 val)
{
	struct mt7530_priv *priv = ds->priv;

	return priv->info->phy_write(ds, port, regnum, val);
}

static int mt753x_get_mac_eee(struct dsa_switch *ds, int port,
			      struct ethtool_eee *e)
{
	struct mt7530_priv *priv = ds->priv;
	u32 eeecr = mt7530_read(priv, MT7530_PMEEECR_P(port));

	e->tx_lpi_enabled = !(eeecr & LPI_MODE_EN);
	e->tx_lpi_timer = GET_LPI_THRESH(eeecr);

	return 0;
}

static int mt753x_set_mac_eee(struct dsa_switch *ds, int port,
			      struct ethtool_eee *e)
{
	struct mt7530_priv *priv = ds->priv;
	u32 set, mask = LPI_THRESH_MASK | LPI_MODE_EN;

	if (e->tx_lpi_timer > 0xFFF)
		return -EINVAL;

	set = SET_LPI_THRESH(e->tx_lpi_timer);
	if (!e->tx_lpi_enabled)
		/* Force LPI Mode without a delay */
		set |= LPI_MODE_EN;
	mt7530_rmw(priv, MT7530_PMEEECR_P(port), mask, set);

	return 0;
}

static const struct dsa_switch_ops mt7530_switch_ops = {
	.get_tag_protocol	= mtk_get_tag_protocol,
	.setup			= mt753x_setup,
	.get_strings		= mt7530_get_strings,
	.phy_read		= mt753x_phy_read,
	.phy_write		= mt753x_phy_write,
	.get_ethtool_stats	= mt7530_get_ethtool_stats,
	.get_sset_count		= mt7530_get_sset_count,
	.set_ageing_time	= mt7530_set_ageing_time,
	.port_enable		= mt7530_port_enable,
	.port_disable		= mt7530_port_disable,
	.port_change_mtu	= mt7530_port_change_mtu,
	.port_max_mtu		= mt7530_port_max_mtu,
	.port_stp_state_set	= mt7530_stp_state_set,
	.port_pre_bridge_flags	= mt7530_port_pre_bridge_flags,
	.port_bridge_flags	= mt7530_port_bridge_flags,
	.port_set_mrouter	= mt7530_port_set_mrouter,
	.port_bridge_join	= mt7530_port_bridge_join,
	.port_bridge_leave	= mt7530_port_bridge_leave,
	.port_fdb_add		= mt7530_port_fdb_add,
	.port_fdb_del		= mt7530_port_fdb_del,
	.port_fdb_dump		= mt7530_port_fdb_dump,
	.port_mdb_add		= mt7530_port_mdb_add,
	.port_mdb_del		= mt7530_port_mdb_del,
	.port_vlan_filtering	= mt7530_port_vlan_filtering,
	.port_vlan_add		= mt7530_port_vlan_add,
	.port_vlan_del		= mt7530_port_vlan_del,
	.port_mirror_add	= mt753x_port_mirror_add,
	.port_mirror_del	= mt753x_port_mirror_del,
	.phylink_validate	= mt753x_phylink_validate,
	.phylink_mac_link_state	= mt753x_phylink_mac_link_state,
	.phylink_mac_config	= mt753x_phylink_mac_config,
	.phylink_mac_an_restart	= mt753x_phylink_mac_an_restart,
	.phylink_mac_link_down	= mt753x_phylink_mac_link_down,
	.phylink_mac_link_up	= mt753x_phylink_mac_link_up,
	.get_mac_eee		= mt753x_get_mac_eee,
	.set_mac_eee		= mt753x_set_mac_eee,
};

static const struct mt753x_info mt753x_table[] = {
	[ID_MT7621] = {
		.id = ID_MT7621,
		.sw_setup = mt7530_setup,
		.phy_read = mt7530_phy_read,
		.phy_write = mt7530_phy_write,
		.pad_setup = mt7530_pad_clk_setup,
		.phy_mode_supported = mt7530_phy_mode_supported,
		.mac_port_validate = mt7530_mac_port_validate,
		.mac_port_get_state = mt7530_phylink_mac_link_state,
		.mac_port_config = mt7530_mac_config,
	},
	[ID_MT7530] = {
		.id = ID_MT7530,
		.sw_setup = mt7530_setup,
		.phy_read = mt7530_phy_read,
		.phy_write = mt7530_phy_write,
		.pad_setup = mt7530_pad_clk_setup,
		.phy_mode_supported = mt7530_phy_mode_supported,
		.mac_port_validate = mt7530_mac_port_validate,
		.mac_port_get_state = mt7530_phylink_mac_link_state,
		.mac_port_config = mt7530_mac_config,
	},
	[ID_MT7531] = {
		.id = ID_MT7531,
		.sw_setup = mt7531_setup,
		.phy_read = mt7531_ind_phy_read,
		.phy_write = mt7531_ind_phy_write,
		.pad_setup = mt7531_pad_setup,
		.cpu_port_config = mt7531_cpu_port_config,
		.phy_mode_supported = mt7531_phy_mode_supported,
		.mac_port_validate = mt7531_mac_port_validate,
		.mac_port_get_state = mt7531_phylink_mac_link_state,
		.mac_port_config = mt7531_mac_config,
		.mac_pcs_an_restart = mt7531_sgmii_restart_an,
		.mac_pcs_link_up = mt7531_sgmii_link_up_force,
	},
};

static const struct of_device_id mt7530_of_match[] = {
	{ .compatible = "mediatek,mt7621", .data = &mt753x_table[ID_MT7621], },
	{ .compatible = "mediatek,mt7530", .data = &mt753x_table[ID_MT7530], },
	{ .compatible = "mediatek,mt7531", .data = &mt753x_table[ID_MT7531], },
	{ /* sentinel */ },
};
MODULE_DEVICE_TABLE(of, mt7530_of_match);

static int
mt7530_probe(struct mdio_device *mdiodev)
{
	struct mt7530_priv *priv;
	struct device_node *dn;

	dn = mdiodev->dev.of_node;

	priv = devm_kzalloc(&mdiodev->dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	priv->ds = devm_kzalloc(&mdiodev->dev, sizeof(*priv->ds), GFP_KERNEL);
	if (!priv->ds)
		return -ENOMEM;

	priv->ds->dev = &mdiodev->dev;
	priv->ds->num_ports = DSA_MAX_PORTS;

	/* Use medatek,mcm property to distinguish hardware type that would
	 * casues a little bit differences on power-on sequence.
	 */
	priv->mcm = of_property_read_bool(dn, "mediatek,mcm");
	if (priv->mcm) {
		dev_info(&mdiodev->dev, "MT7530 adapts as multi-chip module\n");

		priv->rstc = devm_reset_control_get(&mdiodev->dev, "mcm");
		if (IS_ERR(priv->rstc)) {
			dev_err(&mdiodev->dev, "Couldn't get our reset line\n");
			return PTR_ERR(priv->rstc);
		}
	}

	/* Get the hardware identifier from the devicetree node.
	 * We will need it for some of the clock and regulator setup.
	 */
	priv->info = of_device_get_match_data(&mdiodev->dev);
	if (!priv->info)
		return -EINVAL;

	/* Sanity check if these required device operations are filled
	 * properly.
	 */
	if (!priv->info->sw_setup || !priv->info->pad_setup ||
	    !priv->info->phy_read || !priv->info->phy_write ||
	    !priv->info->phy_mode_supported ||
	    !priv->info->mac_port_validate ||
	    !priv->info->mac_port_get_state || !priv->info->mac_port_config)
		return -EINVAL;

	priv->id = priv->info->id;

	if (priv->id == ID_MT7530) {
		priv->core_pwr = devm_regulator_get(&mdiodev->dev, "core");
		if (IS_ERR(priv->core_pwr))
			return PTR_ERR(priv->core_pwr);

		priv->io_pwr = devm_regulator_get(&mdiodev->dev, "io");
		if (IS_ERR(priv->io_pwr))
			return PTR_ERR(priv->io_pwr);
	}

	/* Not MCM that indicates switch works as the remote standalone
	 * integrated circuit so the GPIO pin would be used to complete
	 * the reset, otherwise memory-mapped register accessing used
	 * through syscon provides in the case of MCM.
	 */
	if (!priv->mcm) {
		priv->reset = devm_gpiod_get_optional(&mdiodev->dev, "reset",
						      GPIOD_OUT_LOW);
		if (IS_ERR(priv->reset)) {
			dev_err(&mdiodev->dev, "Couldn't get our reset line\n");
			return PTR_ERR(priv->reset);
		}
	}

	priv->bus = mdiodev->bus;
	priv->dev = &mdiodev->dev;
	priv->ds->priv = priv;
	priv->ds->ops = &mt7530_switch_ops;
	mutex_init(&priv->reg_mutex);
	dev_set_drvdata(&mdiodev->dev, priv);

	return dsa_register_switch(priv->ds);
}

static void
mt7530_remove(struct mdio_device *mdiodev)
{
	struct mt7530_priv *priv = dev_get_drvdata(&mdiodev->dev);
	int ret = 0;

	ret = regulator_disable(priv->core_pwr);
	if (ret < 0)
		dev_err(priv->dev,
			"Failed to disable core power: %d\n", ret);

	ret = regulator_disable(priv->io_pwr);
	if (ret < 0)
		dev_err(priv->dev, "Failed to disable io pwr: %d\n",
			ret);

	dsa_unregister_switch(priv->ds);
	mutex_destroy(&priv->reg_mutex);
}

static struct mdio_driver mt7530_mdio_driver = {
	.probe  = mt7530_probe,
	.remove = mt7530_remove,
	.mdiodrv.driver = {
		.name = "mt7530",
		.of_match_table = mt7530_of_match,
	},
};

mdio_module_driver(mt7530_mdio_driver);

MODULE_AUTHOR("Sean Wang <sean.wang@mediatek.com>");
MODULE_DESCRIPTION("Driver for Mediatek MT7530 Switch");
MODULE_LICENSE("GPL");
