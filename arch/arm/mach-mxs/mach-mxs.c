// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright 2012 Freescale Semiconductor, Inc.
 * Copyright 2012 Linaro Ltd.
 */

#include <linux/clk.h>
#include <linux/clk/mxs.h>
#include <linux/clkdev.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/gpio.h>
#include <linux/init.h>
#include <linux/irqchip/mxs.h>
#include <linux/reboot.h>
#include <linux/micrel_phy.h>
#include <linux/of_address.h>
#include <linux/of_platform.h>
#include <linux/phy.h>
#include <linux/pinctrl/consumer.h>
#include <linux/sys_soc.h>
#include <asm/mach/arch.h>
#include <asm/mach/map.h>
#include <asm/mach/time.h>
#include <asm/system_info.h>
#include <asm/system_misc.h>

#include "pm.h"

/* MXS DIGCTL SAIF CLKMUX */
#define MXS_DIGCTL_SAIF_CLKMUX_DIRECT		0x0
#define MXS_DIGCTL_SAIF_CLKMUX_CROSSINPUT	0x1
#define MXS_DIGCTL_SAIF_CLKMUX_EXTMSTR0		0x2
#define MXS_DIGCTL_SAIF_CLKMUX_EXTMSTR1		0x3

#define HW_DIGCTL_CHIPID	0x310
#define HW_DIGCTL_CHIPID_MASK	(0xffff << 16)
#define HW_DIGCTL_REV_MASK	0xff
#define HW_DIGCTL_CHIPID_MX23	(0x3780 << 16)
#define HW_DIGCTL_CHIPID_MX28	(0x2800 << 16)

#define MXS_CHIP_REVISION_1_0	0x10
#define MXS_CHIP_REVISION_1_1	0x11
#define MXS_CHIP_REVISION_1_2	0x12
#define MXS_CHIP_REVISION_1_3	0x13
#define MXS_CHIP_REVISION_1_4	0x14
#define MXS_CHIP_REV_UNKNOWN	0xff

#define MXS_GPIO_NR(bank, nr)	((bank) * 32 + (nr))

#define MXS_SET_ADDR		0x4
#define MXS_CLR_ADDR		0x8
#define MXS_TOG_ADDR		0xc

#define HW_OCOTP_OPS2		19	/* offset 0x150 */
#define HW_OCOTP_OPS3		20	/* offset 0x160 */

static u32 chipid;
static u32 socid;

static void __iomem *reset_addr;

static inline void __mxs_setl(u32 mask, void __iomem *reg)
{
	__raw_writel(mask, reg + MXS_SET_ADDR);
}

static inline void __mxs_clrl(u32 mask, void __iomem *reg)
{
	__raw_writel(mask, reg + MXS_CLR_ADDR);
}

static inline void __mxs_togl(u32 mask, void __iomem *reg)
{
	__raw_writel(mask, reg + MXS_TOG_ADDR);
}

#define OCOTP_WORD_OFFSET		0x20
#define OCOTP_WORD_COUNT		0x20

#define BM_OCOTP_CTRL_BUSY		(1 << 8)
#define BM_OCOTP_CTRL_ERROR		(1 << 9)
#define BM_OCOTP_CTRL_RD_BANK_OPEN	(1 << 12)

static DEFINE_MUTEX(ocotp_mutex);
static u32 ocotp_words[OCOTP_WORD_COUNT];

static const u32 *mxs_get_ocotp(void)
{
	struct device_node *np;
	void __iomem *ocotp_base;
	int timeout = 0x400;
	size_t i;
	static int once;

	if (once)
		return ocotp_words;

	np = of_find_compatible_node(NULL, NULL, "fsl,ocotp");
	ocotp_base = of_iomap(np, 0);
	WARN_ON(!ocotp_base);

	mutex_lock(&ocotp_mutex);

	/*
	 * clk_enable(hbus_clk) for ocotp can be skipped
	 * as it must be on when system is running.
	 */

	/* try to clear ERROR bit */
	__mxs_clrl(BM_OCOTP_CTRL_ERROR, ocotp_base);

	/* check both BUSY and ERROR cleared */
	while ((__raw_readl(ocotp_base) &
		(BM_OCOTP_CTRL_BUSY | BM_OCOTP_CTRL_ERROR)) && --timeout)
		cpu_relax();

	if (unlikely(!timeout))
		goto error_unlock;

	/* open OCOTP banks for read */
	__mxs_setl(BM_OCOTP_CTRL_RD_BANK_OPEN, ocotp_base);

	/* approximately wait 32 hclk cycles */
	udelay(1);

	/* poll BUSY bit becoming cleared */
	timeout = 0x400;
	while ((__raw_readl(ocotp_base) & BM_OCOTP_CTRL_BUSY) && --timeout)
		cpu_relax();

	if (unlikely(!timeout))
		goto error_unlock;

	for (i = 0; i < OCOTP_WORD_COUNT; i++)
		ocotp_words[i] = __raw_readl(ocotp_base + OCOTP_WORD_OFFSET +
						i * 0x10);

	/* close banks for power saving */
	__mxs_clrl(BM_OCOTP_CTRL_RD_BANK_OPEN, ocotp_base);

	once = 1;

	mutex_unlock(&ocotp_mutex);

	return ocotp_words;

error_unlock:
	mutex_unlock(&ocotp_mutex);
	pr_err("%s: timeout in reading OCOTP\n", __func__);
	return NULL;
}

enum mac_oui {
	OUI_FSL,
	OUI_DENX,
	OUI_CRYSTALFONTZ,
	OUI_I2SE,
	OUI_ARMADEUS,
};

static void __init update_fec_mac_prop(enum mac_oui oui)
{
	struct device_node *np, *from = NULL;
	struct property *newmac;
	const u32 *ocotp = mxs_get_ocotp();
	u8 *macaddr;
	u32 val;
	int i;

	for (i = 0; i < 2; i++) {
		np = of_find_compatible_node(from, NULL, "fsl,imx28-fec");
		if (!np)
			return;

		from = np;

		if (of_get_property(np, "local-mac-address", NULL))
			continue;

		newmac = kzalloc(sizeof(*newmac) + 6, GFP_KERNEL);
		if (!newmac)
			return;
		newmac->value = newmac + 1;
		newmac->length = 6;

		newmac->name = kstrdup("local-mac-address", GFP_KERNEL);
		if (!newmac->name) {
			kfree(newmac);
			return;
		}

		/*
		 * OCOTP only stores the last 4 octets for each mac address,
		 * so hard-code OUI here.
		 */
		macaddr = newmac->value;
		switch (oui) {
		case OUI_FSL:
			macaddr[0] = 0x00;
			macaddr[1] = 0x04;
			macaddr[2] = 0x9f;
			break;
		case OUI_DENX:
			macaddr[0] = 0xc0;
			macaddr[1] = 0xe5;
			macaddr[2] = 0x4e;
			break;
		case OUI_CRYSTALFONTZ:
			macaddr[0] = 0x58;
			macaddr[1] = 0xb9;
			macaddr[2] = 0xe1;
			break;
		case OUI_I2SE:
			macaddr[0] = 0x00;
			macaddr[1] = 0x01;
			macaddr[2] = 0x87;
			break;
		case OUI_ARMADEUS:
			macaddr[0] = 0x00;
			macaddr[1] = 0x1e;
			macaddr[2] = 0xac;
			break;
		}
		val = ocotp[i];
		macaddr[3] = (val >> 16) & 0xff;
		macaddr[4] = (val >> 8) & 0xff;
		macaddr[5] = (val >> 0) & 0xff;

		of_update_property(np, newmac);
	}
}

static inline void enable_clk_enet_out(void)
{
	struct clk *clk = clk_get_sys("enet_out", NULL);

	if (!IS_ERR(clk))
		clk_prepare_enable(clk);
}

static void __init imx28_evk_init(void)
{
	update_fec_mac_prop(OUI_FSL);

	mxs_saif_clkmux_select(MXS_DIGCTL_SAIF_CLKMUX_EXTMSTR0);
}

static void __init imx28_apf28_init(void)
{
	update_fec_mac_prop(OUI_ARMADEUS);
}

static int apx4devkit_phy_fixup(struct phy_device *phy)
{
	phy->dev_flags |= MICREL_PHY_50MHZ_CLK;
	return 0;
}

static void __init apx4devkit_init(void)
{
	enable_clk_enet_out();

	if (IS_BUILTIN(CONFIG_PHYLIB))
		phy_register_fixup_for_uid(PHY_ID_KSZ8051, MICREL_PHY_ID_MASK,
					   apx4devkit_phy_fixup);
}

static void __init crystalfontz_init(void)
{
	update_fec_mac_prop(OUI_CRYSTALFONTZ);
}

static void __init duckbill_init(void)
{
	update_fec_mac_prop(OUI_I2SE);
}

static void __init m28cu3_init(void)
{
	update_fec_mac_prop(OUI_DENX);
}

static const char __init *mxs_get_soc_id(void)
{
	struct device_node *np;
	void __iomem *digctl_base;

	np = of_find_compatible_node(NULL, NULL, "fsl,imx23-digctl");
	digctl_base = of_iomap(np, 0);
	WARN_ON(!digctl_base);

	chipid = readl(digctl_base + HW_DIGCTL_CHIPID);
	socid = chipid & HW_DIGCTL_CHIPID_MASK;

	iounmap(digctl_base);
	of_node_put(np);

	switch (socid) {
	case HW_DIGCTL_CHIPID_MX23:
		return "i.MX23";
	case HW_DIGCTL_CHIPID_MX28:
		return "i.MX28";
	default:
		return "Unknown";
	}
}

static u32 __init mxs_get_cpu_rev(void)
{
	u32 rev = chipid & HW_DIGCTL_REV_MASK;

	switch (socid) {
	case HW_DIGCTL_CHIPID_MX23:
		switch (rev) {
		case 0x0:
			return MXS_CHIP_REVISION_1_0;
		case 0x1:
			return MXS_CHIP_REVISION_1_1;
		case 0x2:
			return MXS_CHIP_REVISION_1_2;
		case 0x3:
			return MXS_CHIP_REVISION_1_3;
		case 0x4:
			return MXS_CHIP_REVISION_1_4;
		default:
			return MXS_CHIP_REV_UNKNOWN;
		}
	case HW_DIGCTL_CHIPID_MX28:
		switch (rev) {
		case 0x0:
			return MXS_CHIP_REVISION_1_1;
		case 0x1:
			return MXS_CHIP_REVISION_1_2;
		default:
			return MXS_CHIP_REV_UNKNOWN;
		}
	default:
		return MXS_CHIP_REV_UNKNOWN;
	}
}

static const char __init *mxs_get_revision(void)
{
	u32 rev = mxs_get_cpu_rev();

	if (rev != MXS_CHIP_REV_UNKNOWN)
		return kasprintf(GFP_KERNEL, "%d.%d", (rev >> 4) & 0xf,
				rev & 0xf);
	else
		return kasprintf(GFP_KERNEL, "%s", "Unknown");
}

#define MX23_CLKCTRL_RESET_OFFSET	0x120
#define MX28_CLKCTRL_RESET_OFFSET	0x1e0

static int __init mxs_restart_init(void)
{
	struct device_node *np;

	np = of_find_compatible_node(NULL, NULL, "fsl,clkctrl");
	reset_addr = of_iomap(np, 0);
	if (!reset_addr)
		return -ENODEV;

	if (of_device_is_compatible(np, "fsl,imx23-clkctrl"))
		reset_addr += MX23_CLKCTRL_RESET_OFFSET;
	else
		reset_addr += MX28_CLKCTRL_RESET_OFFSET;
	of_node_put(np);

	return 0;
}

static void __init eukrea_mbmx283lc_init(void)
{
	mxs_saif_clkmux_select(MXS_DIGCTL_SAIF_CLKMUX_EXTMSTR0);
}

static void __init mxs_machine_init(void)
{
	struct device_node *root;
	struct device *parent;
	struct soc_device *soc_dev;
	struct soc_device_attribute *soc_dev_attr;
	u64 soc_uid = 0;
	const u32 *ocotp = mxs_get_ocotp();
	int ret;

	soc_dev_attr = kzalloc(sizeof(*soc_dev_attr), GFP_KERNEL);
	if (!soc_dev_attr)
		return;

	root = of_find_node_by_path("/");
	ret = of_property_read_string(root, "model", &soc_dev_attr->machine);
	if (ret)
		return;

	soc_dev_attr->family = "Freescale MXS Family";
	soc_dev_attr->soc_id = mxs_get_soc_id();
	soc_dev_attr->revision = mxs_get_revision();

	if (socid == HW_DIGCTL_CHIPID_MX23) {
		soc_uid = system_serial_low = ocotp[HW_OCOTP_OPS3];
	} else if (socid == HW_DIGCTL_CHIPID_MX28) {
		soc_uid = system_serial_high = ocotp[HW_OCOTP_OPS2];
		soc_uid <<= 32;
		system_serial_low = ocotp[HW_OCOTP_OPS3];
		soc_uid |= system_serial_low;
	}

	if (soc_uid)
		soc_dev_attr->serial_number = kasprintf(GFP_KERNEL, "%016llX", soc_uid);

	soc_dev = soc_device_register(soc_dev_attr);
	if (IS_ERR(soc_dev)) {
		kfree(soc_dev_attr->serial_number);
		kfree(soc_dev_attr->revision);
		kfree(soc_dev_attr);
		return;
	}

	parent = soc_device_to_device(soc_dev);

	if (of_machine_is_compatible("fsl,imx28-evk"))
		imx28_evk_init();
	if (of_machine_is_compatible("armadeus,imx28-apf28"))
		imx28_apf28_init();
	else if (of_machine_is_compatible("bluegiga,apx4devkit"))
		apx4devkit_init();
	else if (of_machine_is_compatible("crystalfontz,cfa10036"))
		crystalfontz_init();
	else if (of_machine_is_compatible("eukrea,mbmx283lc"))
		eukrea_mbmx283lc_init();
	else if (of_machine_is_compatible("i2se,duckbill") ||
		 of_machine_is_compatible("i2se,duckbill-2"))
		duckbill_init();
	else if (of_machine_is_compatible("msr,m28cu3"))
		m28cu3_init();

	of_platform_default_populate(NULL, NULL, parent);

	mxs_restart_init();
}

#define MXS_CLKCTRL_RESET_CHIP		(1 << 1)

/*
 * Reset the system. It is called by machine_restart().
 */
static void mxs_restart(enum reboot_mode mode, const char *cmd)
{
	if (reset_addr) {
		/* reset the chip */
		__mxs_setl(MXS_CLKCTRL_RESET_CHIP, reset_addr);

		pr_err("Failed to assert the chip reset\n");

		/* Delay to allow the serial port to show the message */
		mdelay(50);
	}

	/* We'll take a jump through zero as a poor second */
	soft_restart(0);
}

static const char *const mxs_dt_compat[] __initconst = {
	"fsl,imx28",
	"fsl,imx23",
	NULL,
};

DT_MACHINE_START(MXS, "Freescale MXS (Device Tree)")
	.handle_irq	= icoll_handle_irq,
	.init_machine	= mxs_machine_init,
	.init_late      = mxs_pm_init,
	.dt_compat	= mxs_dt_compat,
	.restart	= mxs_restart,
MACHINE_END
