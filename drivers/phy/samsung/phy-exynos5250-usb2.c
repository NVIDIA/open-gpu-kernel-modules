// SPDX-License-Identifier: GPL-2.0-only
/*
 * Samsung SoC USB 1.1/2.0 PHY driver - Exynos 5250 support
 *
 * Copyright (C) 2013 Samsung Electronics Co., Ltd.
 * Author: Kamil Debski <k.debski@samsung.com>
 */

#include <linux/delay.h>
#include <linux/io.h>
#include <linux/phy/phy.h>
#include <linux/regmap.h>
#include "phy-samsung-usb2.h"

/* Exynos USB PHY registers */
#define EXYNOS_5250_REFCLKSEL_CRYSTAL	0x0
#define EXYNOS_5250_REFCLKSEL_XO	0x1
#define EXYNOS_5250_REFCLKSEL_CLKCORE	0x2

#define EXYNOS_5250_FSEL_9MHZ6		0x0
#define EXYNOS_5250_FSEL_10MHZ		0x1
#define EXYNOS_5250_FSEL_12MHZ		0x2
#define EXYNOS_5250_FSEL_19MHZ2		0x3
#define EXYNOS_5250_FSEL_20MHZ		0x4
#define EXYNOS_5250_FSEL_24MHZ		0x5
#define EXYNOS_5250_FSEL_50MHZ		0x7

/* Normal host */
#define EXYNOS_5250_HOSTPHYCTRL0			0x0

#define EXYNOS_5250_HOSTPHYCTRL0_PHYSWRSTALL		BIT(31)
#define EXYNOS_5250_HOSTPHYCTRL0_REFCLKSEL_SHIFT	19
#define EXYNOS_5250_HOSTPHYCTRL0_REFCLKSEL_MASK	\
		(0x3 << EXYNOS_5250_HOSTPHYCTRL0_REFCLKSEL_SHIFT)
#define EXYNOS_5250_HOSTPHYCTRL0_FSEL_SHIFT		16
#define EXYNOS_5250_HOSTPHYCTRL0_FSEL_MASK \
		(0x7 << EXYNOS_5250_HOSTPHYCTRL0_FSEL_SHIFT)
#define EXYNOS_5250_HOSTPHYCTRL0_TESTBURNIN		BIT(11)
#define EXYNOS_5250_HOSTPHYCTRL0_RETENABLE		BIT(10)
#define EXYNOS_5250_HOSTPHYCTRL0_COMMON_ON_N		BIT(9)
#define EXYNOS_5250_HOSTPHYCTRL0_VATESTENB_MASK		(0x3 << 7)
#define EXYNOS_5250_HOSTPHYCTRL0_VATESTENB_DUAL		(0x0 << 7)
#define EXYNOS_5250_HOSTPHYCTRL0_VATESTENB_ID0		(0x1 << 7)
#define EXYNOS_5250_HOSTPHYCTRL0_VATESTENB_ANALOGTEST	(0x2 << 7)
#define EXYNOS_5250_HOSTPHYCTRL0_SIDDQ			BIT(6)
#define EXYNOS_5250_HOSTPHYCTRL0_FORCESLEEP		BIT(5)
#define EXYNOS_5250_HOSTPHYCTRL0_FORCESUSPEND		BIT(4)
#define EXYNOS_5250_HOSTPHYCTRL0_WORDINTERFACE		BIT(3)
#define EXYNOS_5250_HOSTPHYCTRL0_UTMISWRST		BIT(2)
#define EXYNOS_5250_HOSTPHYCTRL0_LINKSWRST		BIT(1)
#define EXYNOS_5250_HOSTPHYCTRL0_PHYSWRST		BIT(0)

/* HSIC0 & HSIC1 */
#define EXYNOS_5250_HSICPHYCTRL1			0x10
#define EXYNOS_5250_HSICPHYCTRL2			0x20

#define EXYNOS_5250_HSICPHYCTRLX_REFCLKSEL_MASK		(0x3 << 23)
#define EXYNOS_5250_HSICPHYCTRLX_REFCLKSEL_DEFAULT	(0x2 << 23)
#define EXYNOS_5250_HSICPHYCTRLX_REFCLKDIV_MASK		(0x7f << 16)
#define EXYNOS_5250_HSICPHYCTRLX_REFCLKDIV_12		(0x24 << 16)
#define EXYNOS_5250_HSICPHYCTRLX_REFCLKDIV_15		(0x1c << 16)
#define EXYNOS_5250_HSICPHYCTRLX_REFCLKDIV_16		(0x1a << 16)
#define EXYNOS_5250_HSICPHYCTRLX_REFCLKDIV_19_2		(0x15 << 16)
#define EXYNOS_5250_HSICPHYCTRLX_REFCLKDIV_20		(0x14 << 16)
#define EXYNOS_5250_HSICPHYCTRLX_SIDDQ			BIT(6)
#define EXYNOS_5250_HSICPHYCTRLX_FORCESLEEP		BIT(5)
#define EXYNOS_5250_HSICPHYCTRLX_FORCESUSPEND		BIT(4)
#define EXYNOS_5250_HSICPHYCTRLX_WORDINTERFACE		BIT(3)
#define EXYNOS_5250_HSICPHYCTRLX_UTMISWRST		BIT(2)
#define EXYNOS_5250_HSICPHYCTRLX_PHYSWRST		BIT(0)

/* EHCI control */
#define EXYNOS_5250_HOSTEHCICTRL			0x30
#define EXYNOS_5250_HOSTEHCICTRL_ENAINCRXALIGN		BIT(29)
#define EXYNOS_5250_HOSTEHCICTRL_ENAINCR4		BIT(28)
#define EXYNOS_5250_HOSTEHCICTRL_ENAINCR8		BIT(27)
#define EXYNOS_5250_HOSTEHCICTRL_ENAINCR16		BIT(26)
#define EXYNOS_5250_HOSTEHCICTRL_AUTOPPDONOVRCUREN	BIT(25)
#define EXYNOS_5250_HOSTEHCICTRL_FLADJVAL0_SHIFT	19
#define EXYNOS_5250_HOSTEHCICTRL_FLADJVAL0_MASK	\
		(0x3f << EXYNOS_5250_HOSTEHCICTRL_FLADJVAL0_SHIFT)
#define EXYNOS_5250_HOSTEHCICTRL_FLADJVAL1_SHIFT	13
#define EXYNOS_5250_HOSTEHCICTRL_FLADJVAL1_MASK	\
		(0x3f << EXYNOS_5250_HOSTEHCICTRL_FLADJVAL1_SHIFT)
#define EXYNOS_5250_HOSTEHCICTRL_FLADJVAL2_SHIFT	7
#define EXYNOS_5250_HOSTEHCICTRL_FLADJVAL0_MASK	\
		(0x3f << EXYNOS_5250_HOSTEHCICTRL_FLADJVAL0_SHIFT)
#define EXYNOS_5250_HOSTEHCICTRL_FLADJVALHOST_SHIFT	1
#define EXYNOS_5250_HOSTEHCICTRL_FLADJVALHOST_MASK \
		(0x1 << EXYNOS_5250_HOSTEHCICTRL_FLADJVALHOST_SHIFT)
#define EXYNOS_5250_HOSTEHCICTRL_SIMULATIONMODE		BIT(0)

/* OHCI control */
#define EXYNOS_5250_HOSTOHCICTRL                        0x34
#define EXYNOS_5250_HOSTOHCICTRL_FRAMELENVAL_SHIFT	1
#define EXYNOS_5250_HOSTOHCICTRL_FRAMELENVAL_MASK \
		(0x3ff << EXYNOS_5250_HOSTOHCICTRL_FRAMELENVAL_SHIFT)
#define EXYNOS_5250_HOSTOHCICTRL_FRAMELENVALEN		BIT(0)

/* USBOTG */
#define EXYNOS_5250_USBOTGSYS				0x38
#define EXYNOS_5250_USBOTGSYS_PHYLINK_SW_RESET		BIT(14)
#define EXYNOS_5250_USBOTGSYS_LINK_SW_RST_UOTG		BIT(13)
#define EXYNOS_5250_USBOTGSYS_PHY_SW_RST		BIT(12)
#define EXYNOS_5250_USBOTGSYS_REFCLKSEL_SHIFT		9
#define EXYNOS_5250_USBOTGSYS_REFCLKSEL_MASK \
		(0x3 << EXYNOS_5250_USBOTGSYS_REFCLKSEL_SHIFT)
#define EXYNOS_5250_USBOTGSYS_ID_PULLUP			BIT(8)
#define EXYNOS_5250_USBOTGSYS_COMMON_ON			BIT(7)
#define EXYNOS_5250_USBOTGSYS_FSEL_SHIFT		4
#define EXYNOS_5250_USBOTGSYS_FSEL_MASK \
		(0x3 << EXYNOS_5250_USBOTGSYS_FSEL_SHIFT)
#define EXYNOS_5250_USBOTGSYS_FORCE_SLEEP		BIT(3)
#define EXYNOS_5250_USBOTGSYS_OTGDISABLE		BIT(2)
#define EXYNOS_5250_USBOTGSYS_SIDDQ_UOTG		BIT(1)
#define EXYNOS_5250_USBOTGSYS_FORCE_SUSPEND		BIT(0)

/* Isolation, configured in the power management unit */
#define EXYNOS_5250_USB_ISOL_OTG_OFFSET		0x704
#define EXYNOS_5250_USB_ISOL_HOST_OFFSET	0x708
#define EXYNOS_5420_USB_ISOL_HOST_OFFSET	0x70C
#define EXYNOS_5250_USB_ISOL_ENABLE		BIT(0)

/* Mode swtich register */
#define EXYNOS_5250_MODE_SWITCH_OFFSET		0x230
#define EXYNOS_5250_MODE_SWITCH_MASK		1
#define EXYNOS_5250_MODE_SWITCH_DEVICE		0
#define EXYNOS_5250_MODE_SWITCH_HOST		1

enum exynos4x12_phy_id {
	EXYNOS5250_DEVICE,
	EXYNOS5250_HOST,
	EXYNOS5250_HSIC0,
	EXYNOS5250_HSIC1,
};

/*
 * exynos5250_rate_to_clk() converts the supplied clock rate to the value that
 * can be written to the phy register.
 */
static int exynos5250_rate_to_clk(unsigned long rate, u32 *reg)
{
	/* EXYNOS_5250_FSEL_MASK */

	switch (rate) {
	case 9600 * KHZ:
		*reg = EXYNOS_5250_FSEL_9MHZ6;
		break;
	case 10 * MHZ:
		*reg = EXYNOS_5250_FSEL_10MHZ;
		break;
	case 12 * MHZ:
		*reg = EXYNOS_5250_FSEL_12MHZ;
		break;
	case 19200 * KHZ:
		*reg = EXYNOS_5250_FSEL_19MHZ2;
		break;
	case 20 * MHZ:
		*reg = EXYNOS_5250_FSEL_20MHZ;
		break;
	case 24 * MHZ:
		*reg = EXYNOS_5250_FSEL_24MHZ;
		break;
	case 50 * MHZ:
		*reg = EXYNOS_5250_FSEL_50MHZ;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static void exynos5250_isol(struct samsung_usb2_phy_instance *inst, bool on)
{
	struct samsung_usb2_phy_driver *drv = inst->drv;
	u32 offset;
	u32 mask = EXYNOS_5250_USB_ISOL_ENABLE;

	if (drv->cfg == &exynos5250_usb2_phy_config &&
	    inst->cfg->id == EXYNOS5250_DEVICE)
		offset = EXYNOS_5250_USB_ISOL_OTG_OFFSET;
	else if (drv->cfg == &exynos5250_usb2_phy_config &&
		 inst->cfg->id == EXYNOS5250_HOST)
		offset = EXYNOS_5250_USB_ISOL_HOST_OFFSET;
	else if (drv->cfg == &exynos5420_usb2_phy_config &&
		 inst->cfg->id == EXYNOS5250_HOST)
		offset = EXYNOS_5420_USB_ISOL_HOST_OFFSET;
	else
		return;

	regmap_update_bits(drv->reg_pmu, offset, mask, on ? 0 : mask);
}

static int exynos5250_power_on(struct samsung_usb2_phy_instance *inst)
{
	struct samsung_usb2_phy_driver *drv = inst->drv;
	u32 ctrl0;
	u32 otg;
	u32 ehci;
	u32 ohci;
	u32 hsic;

	switch (inst->cfg->id) {
	case EXYNOS5250_DEVICE:
		regmap_update_bits(drv->reg_sys,
				   EXYNOS_5250_MODE_SWITCH_OFFSET,
				   EXYNOS_5250_MODE_SWITCH_MASK,
				   EXYNOS_5250_MODE_SWITCH_DEVICE);

		/* OTG configuration */
		otg = readl(drv->reg_phy + EXYNOS_5250_USBOTGSYS);
		/* The clock */
		otg &= ~EXYNOS_5250_USBOTGSYS_FSEL_MASK;
		otg |= drv->ref_reg_val << EXYNOS_5250_USBOTGSYS_FSEL_SHIFT;
		/* Reset */
		otg &= ~(EXYNOS_5250_USBOTGSYS_FORCE_SUSPEND |
			EXYNOS_5250_USBOTGSYS_FORCE_SLEEP |
			EXYNOS_5250_USBOTGSYS_SIDDQ_UOTG);
		otg |=	EXYNOS_5250_USBOTGSYS_PHY_SW_RST |
			EXYNOS_5250_USBOTGSYS_PHYLINK_SW_RESET |
			EXYNOS_5250_USBOTGSYS_LINK_SW_RST_UOTG |
			EXYNOS_5250_USBOTGSYS_OTGDISABLE;
		/* Ref clock */
		otg &=	~EXYNOS_5250_USBOTGSYS_REFCLKSEL_MASK;
		otg |=  EXYNOS_5250_REFCLKSEL_CLKCORE <<
					EXYNOS_5250_USBOTGSYS_REFCLKSEL_SHIFT;
		writel(otg, drv->reg_phy + EXYNOS_5250_USBOTGSYS);
		udelay(100);
		otg &= ~(EXYNOS_5250_USBOTGSYS_PHY_SW_RST |
			EXYNOS_5250_USBOTGSYS_LINK_SW_RST_UOTG |
			EXYNOS_5250_USBOTGSYS_PHYLINK_SW_RESET |
			EXYNOS_5250_USBOTGSYS_OTGDISABLE);
		writel(otg, drv->reg_phy + EXYNOS_5250_USBOTGSYS);


		break;
	case EXYNOS5250_HOST:
	case EXYNOS5250_HSIC0:
	case EXYNOS5250_HSIC1:
		/* Host registers configuration */
		ctrl0 = readl(drv->reg_phy + EXYNOS_5250_HOSTPHYCTRL0);
		/* The clock */
		ctrl0 &= ~EXYNOS_5250_HOSTPHYCTRL0_FSEL_MASK;
		ctrl0 |= drv->ref_reg_val <<
					EXYNOS_5250_HOSTPHYCTRL0_FSEL_SHIFT;

		/* Reset */
		ctrl0 &=	~(EXYNOS_5250_HOSTPHYCTRL0_PHYSWRST |
				EXYNOS_5250_HOSTPHYCTRL0_PHYSWRSTALL |
				EXYNOS_5250_HOSTPHYCTRL0_SIDDQ |
				EXYNOS_5250_HOSTPHYCTRL0_FORCESUSPEND |
				EXYNOS_5250_HOSTPHYCTRL0_FORCESLEEP);
		ctrl0 |=	EXYNOS_5250_HOSTPHYCTRL0_LINKSWRST |
				EXYNOS_5250_HOSTPHYCTRL0_UTMISWRST |
				EXYNOS_5250_HOSTPHYCTRL0_COMMON_ON_N;
		writel(ctrl0, drv->reg_phy + EXYNOS_5250_HOSTPHYCTRL0);
		udelay(10);
		ctrl0 &=	~(EXYNOS_5250_HOSTPHYCTRL0_LINKSWRST |
				EXYNOS_5250_HOSTPHYCTRL0_UTMISWRST);
		writel(ctrl0, drv->reg_phy + EXYNOS_5250_HOSTPHYCTRL0);

		/* OTG configuration */
		otg = readl(drv->reg_phy + EXYNOS_5250_USBOTGSYS);
		/* The clock */
		otg &= ~EXYNOS_5250_USBOTGSYS_FSEL_MASK;
		otg |= drv->ref_reg_val << EXYNOS_5250_USBOTGSYS_FSEL_SHIFT;
		/* Reset */
		otg &= ~(EXYNOS_5250_USBOTGSYS_FORCE_SUSPEND |
			EXYNOS_5250_USBOTGSYS_FORCE_SLEEP |
			EXYNOS_5250_USBOTGSYS_SIDDQ_UOTG);
		otg |=	EXYNOS_5250_USBOTGSYS_PHY_SW_RST |
			EXYNOS_5250_USBOTGSYS_PHYLINK_SW_RESET |
			EXYNOS_5250_USBOTGSYS_LINK_SW_RST_UOTG |
			EXYNOS_5250_USBOTGSYS_OTGDISABLE;
		/* Ref clock */
		otg &=	~EXYNOS_5250_USBOTGSYS_REFCLKSEL_MASK;
		otg |=  EXYNOS_5250_REFCLKSEL_CLKCORE <<
					EXYNOS_5250_USBOTGSYS_REFCLKSEL_SHIFT;
		writel(otg, drv->reg_phy + EXYNOS_5250_USBOTGSYS);
		udelay(10);
		otg &= ~(EXYNOS_5250_USBOTGSYS_PHY_SW_RST |
			EXYNOS_5250_USBOTGSYS_LINK_SW_RST_UOTG |
			EXYNOS_5250_USBOTGSYS_PHYLINK_SW_RESET);

		/* HSIC phy configuration */
		hsic = (EXYNOS_5250_HSICPHYCTRLX_REFCLKDIV_12 |
				EXYNOS_5250_HSICPHYCTRLX_REFCLKSEL_DEFAULT |
				EXYNOS_5250_HSICPHYCTRLX_PHYSWRST);
		writel(hsic, drv->reg_phy + EXYNOS_5250_HSICPHYCTRL1);
		writel(hsic, drv->reg_phy + EXYNOS_5250_HSICPHYCTRL2);
		udelay(10);
		hsic &= ~EXYNOS_5250_HSICPHYCTRLX_PHYSWRST;
		writel(hsic, drv->reg_phy + EXYNOS_5250_HSICPHYCTRL1);
		writel(hsic, drv->reg_phy + EXYNOS_5250_HSICPHYCTRL2);
		/* The following delay is necessary for the reset sequence to be
		 * completed */
		udelay(80);

		/* Enable EHCI DMA burst */
		ehci = readl(drv->reg_phy + EXYNOS_5250_HOSTEHCICTRL);
		ehci |=	EXYNOS_5250_HOSTEHCICTRL_ENAINCRXALIGN |
			EXYNOS_5250_HOSTEHCICTRL_ENAINCR4 |
			EXYNOS_5250_HOSTEHCICTRL_ENAINCR8 |
			EXYNOS_5250_HOSTEHCICTRL_ENAINCR16;
		writel(ehci, drv->reg_phy + EXYNOS_5250_HOSTEHCICTRL);

		/* OHCI settings */
		ohci = readl(drv->reg_phy + EXYNOS_5250_HOSTOHCICTRL);
		/* Following code is based on the old driver */
		ohci |=	0x1 << 3;
		writel(ohci, drv->reg_phy + EXYNOS_5250_HOSTOHCICTRL);

		break;
	}
	exynos5250_isol(inst, 0);

	return 0;
}

static int exynos5250_power_off(struct samsung_usb2_phy_instance *inst)
{
	struct samsung_usb2_phy_driver *drv = inst->drv;
	u32 ctrl0;
	u32 otg;
	u32 hsic;

	exynos5250_isol(inst, 1);

	switch (inst->cfg->id) {
	case EXYNOS5250_DEVICE:
		otg = readl(drv->reg_phy + EXYNOS_5250_USBOTGSYS);
		otg |= (EXYNOS_5250_USBOTGSYS_FORCE_SUSPEND |
			EXYNOS_5250_USBOTGSYS_SIDDQ_UOTG |
			EXYNOS_5250_USBOTGSYS_FORCE_SLEEP);
		writel(otg, drv->reg_phy + EXYNOS_5250_USBOTGSYS);
		break;
	case EXYNOS5250_HOST:
		ctrl0 = readl(drv->reg_phy + EXYNOS_5250_HOSTPHYCTRL0);
		ctrl0 |= (EXYNOS_5250_HOSTPHYCTRL0_SIDDQ |
				EXYNOS_5250_HOSTPHYCTRL0_FORCESUSPEND |
				EXYNOS_5250_HOSTPHYCTRL0_FORCESLEEP |
				EXYNOS_5250_HOSTPHYCTRL0_PHYSWRST |
				EXYNOS_5250_HOSTPHYCTRL0_PHYSWRSTALL);
		writel(ctrl0, drv->reg_phy + EXYNOS_5250_HOSTPHYCTRL0);
		break;
	case EXYNOS5250_HSIC0:
	case EXYNOS5250_HSIC1:
		hsic = (EXYNOS_5250_HSICPHYCTRLX_REFCLKDIV_12 |
				EXYNOS_5250_HSICPHYCTRLX_REFCLKSEL_DEFAULT |
				EXYNOS_5250_HSICPHYCTRLX_SIDDQ |
				EXYNOS_5250_HSICPHYCTRLX_FORCESLEEP |
				EXYNOS_5250_HSICPHYCTRLX_FORCESUSPEND
				);
		writel(hsic, drv->reg_phy + EXYNOS_5250_HSICPHYCTRL1);
		writel(hsic, drv->reg_phy + EXYNOS_5250_HSICPHYCTRL2);
		break;
	}

	return 0;
}


static const struct samsung_usb2_common_phy exynos5250_phys[] = {
	{
		.label		= "device",
		.id		= EXYNOS5250_DEVICE,
		.power_on	= exynos5250_power_on,
		.power_off	= exynos5250_power_off,
	},
	{
		.label		= "host",
		.id		= EXYNOS5250_HOST,
		.power_on	= exynos5250_power_on,
		.power_off	= exynos5250_power_off,
	},
	{
		.label		= "hsic0",
		.id		= EXYNOS5250_HSIC0,
		.power_on	= exynos5250_power_on,
		.power_off	= exynos5250_power_off,
	},
	{
		.label		= "hsic1",
		.id		= EXYNOS5250_HSIC1,
		.power_on	= exynos5250_power_on,
		.power_off	= exynos5250_power_off,
	},
};

static const struct samsung_usb2_common_phy exynos5420_phys[] = {
	{
		.label		= "host",
		.id		= EXYNOS5250_HOST,
		.power_on	= exynos5250_power_on,
		.power_off	= exynos5250_power_off,
	},
	{
		.label		= "hsic",
		.id		= EXYNOS5250_HSIC0,
		.power_on	= exynos5250_power_on,
		.power_off	= exynos5250_power_off,
	},
};

const struct samsung_usb2_phy_config exynos5250_usb2_phy_config = {
	.has_mode_switch	= 1,
	.num_phys		= ARRAY_SIZE(exynos5250_phys),
	.phys			= exynos5250_phys,
	.rate_to_clk		= exynos5250_rate_to_clk,
};

const struct samsung_usb2_phy_config exynos5420_usb2_phy_config = {
	.has_mode_switch	= 1,
	.num_phys		= ARRAY_SIZE(exynos5420_phys),
	.phys			= exynos5420_phys,
	.rate_to_clk		= exynos5250_rate_to_clk,
};
