// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2015 Atmel
 *
 * Alexandre Belloni <alexandre.belloni@free-electrons.com
 * Boris Brezillon <boris.brezillon@free-electrons.com
 */

#define pr_fmt(fmt)	"AT91: " fmt

#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_platform.h>
#include <linux/slab.h>
#include <linux/sys_soc.h>

#include "soc.h"

#define AT91_DBGU_CIDR			0x40
#define AT91_DBGU_EXID			0x44
#define AT91_CHIPID_CIDR		0x00
#define AT91_CHIPID_EXID		0x04
#define AT91_CIDR_VERSION(x, m)		((x) & (m))
#define AT91_CIDR_VERSION_MASK		GENMASK(4, 0)
#define AT91_CIDR_VERSION_MASK_SAMA7G5	GENMASK(3, 0)
#define AT91_CIDR_EXT			BIT(31)
#define AT91_CIDR_MATCH_MASK		GENMASK(30, 5)
#define AT91_CIDR_MASK_SAMA7G5		GENMASK(27, 5)

static const struct at91_soc socs[] __initconst = {
#ifdef CONFIG_SOC_AT91RM9200
	AT91_SOC(AT91RM9200_CIDR_MATCH, AT91_CIDR_MATCH_MASK,
		 AT91_CIDR_VERSION_MASK, 0, "at91rm9200 BGA", "at91rm9200"),
#endif
#ifdef CONFIG_SOC_AT91SAM9
	AT91_SOC(AT91SAM9260_CIDR_MATCH, AT91_CIDR_MATCH_MASK,
		 AT91_CIDR_VERSION_MASK, 0, "at91sam9260", NULL),
	AT91_SOC(AT91SAM9261_CIDR_MATCH, AT91_CIDR_MATCH_MASK,
		 AT91_CIDR_VERSION_MASK, 0, "at91sam9261", NULL),
	AT91_SOC(AT91SAM9263_CIDR_MATCH, AT91_CIDR_MATCH_MASK,
		 AT91_CIDR_VERSION_MASK, 0, "at91sam9263", NULL),
	AT91_SOC(AT91SAM9G20_CIDR_MATCH, AT91_CIDR_MATCH_MASK,
		 AT91_CIDR_VERSION_MASK, 0, "at91sam9g20", NULL),
	AT91_SOC(AT91SAM9RL64_CIDR_MATCH, AT91_CIDR_MATCH_MASK,
		 AT91_CIDR_VERSION_MASK, 0, "at91sam9rl64", NULL),
	AT91_SOC(AT91SAM9G45_CIDR_MATCH, AT91_CIDR_MATCH_MASK,
		 AT91_CIDR_VERSION_MASK, AT91SAM9M11_EXID_MATCH,
		 "at91sam9m11", "at91sam9g45"),
	AT91_SOC(AT91SAM9G45_CIDR_MATCH, AT91_CIDR_MATCH_MASK,
		 AT91_CIDR_VERSION_MASK, AT91SAM9M10_EXID_MATCH,
		 "at91sam9m10", "at91sam9g45"),
	AT91_SOC(AT91SAM9G45_CIDR_MATCH, AT91_CIDR_MATCH_MASK,
		 AT91_CIDR_VERSION_MASK, AT91SAM9G46_EXID_MATCH,
		 "at91sam9g46", "at91sam9g45"),
	AT91_SOC(AT91SAM9G45_CIDR_MATCH, AT91_CIDR_MATCH_MASK,
		 AT91_CIDR_VERSION_MASK, AT91SAM9G45_EXID_MATCH,
		 "at91sam9g45", "at91sam9g45"),
	AT91_SOC(AT91SAM9X5_CIDR_MATCH, AT91_CIDR_MATCH_MASK,
		 AT91_CIDR_VERSION_MASK, AT91SAM9G15_EXID_MATCH,
		 "at91sam9g15", "at91sam9x5"),
	AT91_SOC(AT91SAM9X5_CIDR_MATCH, AT91_CIDR_MATCH_MASK,
		 AT91_CIDR_VERSION_MASK, AT91SAM9G35_EXID_MATCH,
		 "at91sam9g35", "at91sam9x5"),
	AT91_SOC(AT91SAM9X5_CIDR_MATCH, AT91_CIDR_MATCH_MASK,
		 AT91_CIDR_VERSION_MASK, AT91SAM9X35_EXID_MATCH,
		 "at91sam9x35", "at91sam9x5"),
	AT91_SOC(AT91SAM9X5_CIDR_MATCH, AT91_CIDR_MATCH_MASK,
		 AT91_CIDR_VERSION_MASK, AT91SAM9G25_EXID_MATCH,
		 "at91sam9g25", "at91sam9x5"),
	AT91_SOC(AT91SAM9X5_CIDR_MATCH, AT91_CIDR_MATCH_MASK,
		 AT91_CIDR_VERSION_MASK, AT91SAM9X25_EXID_MATCH,
		 "at91sam9x25", "at91sam9x5"),
	AT91_SOC(AT91SAM9N12_CIDR_MATCH, AT91_CIDR_MATCH_MASK,
		 AT91_CIDR_VERSION_MASK, AT91SAM9CN12_EXID_MATCH,
		 "at91sam9cn12", "at91sam9n12"),
	AT91_SOC(AT91SAM9N12_CIDR_MATCH, AT91_CIDR_MATCH_MASK,
		 AT91_CIDR_VERSION_MASK, AT91SAM9N12_EXID_MATCH,
		 "at91sam9n12", "at91sam9n12"),
	AT91_SOC(AT91SAM9N12_CIDR_MATCH, AT91_CIDR_MATCH_MASK,
		 AT91_CIDR_VERSION_MASK, AT91SAM9CN11_EXID_MATCH,
		 "at91sam9cn11", "at91sam9n12"),
	AT91_SOC(AT91SAM9XE128_CIDR_MATCH, AT91_CIDR_MATCH_MASK,
		 AT91_CIDR_VERSION_MASK, 0, "at91sam9xe128", "at91sam9xe128"),
	AT91_SOC(AT91SAM9XE256_CIDR_MATCH, AT91_CIDR_MATCH_MASK,
		 AT91_CIDR_VERSION_MASK, 0, "at91sam9xe256", "at91sam9xe256"),
	AT91_SOC(AT91SAM9XE512_CIDR_MATCH, AT91_CIDR_MATCH_MASK,
		 AT91_CIDR_VERSION_MASK, 0, "at91sam9xe512", "at91sam9xe512"),
#endif
#ifdef CONFIG_SOC_SAM9X60
	AT91_SOC(SAM9X60_CIDR_MATCH, AT91_CIDR_MATCH_MASK,
		 AT91_CIDR_VERSION_MASK, SAM9X60_EXID_MATCH,
		 "sam9x60", "sam9x60"),
	AT91_SOC(SAM9X60_CIDR_MATCH, SAM9X60_D5M_EXID_MATCH,
		 AT91_CIDR_VERSION_MASK, SAM9X60_EXID_MATCH,
		 "sam9x60 64MiB DDR2 SiP", "sam9x60"),
	AT91_SOC(SAM9X60_CIDR_MATCH, SAM9X60_D1G_EXID_MATCH,
		 AT91_CIDR_VERSION_MASK, SAM9X60_EXID_MATCH,
		 "sam9x60 128MiB DDR2 SiP", "sam9x60"),
	AT91_SOC(SAM9X60_CIDR_MATCH, SAM9X60_D6K_EXID_MATCH,
		 AT91_CIDR_VERSION_MASK, SAM9X60_EXID_MATCH,
		 "sam9x60 8MiB SDRAM SiP", "sam9x60"),
#endif
#ifdef CONFIG_SOC_SAMA5
	AT91_SOC(SAMA5D2_CIDR_MATCH, AT91_CIDR_MATCH_MASK,
		 AT91_CIDR_VERSION_MASK, SAMA5D21CU_EXID_MATCH,
		 "sama5d21", "sama5d2"),
	AT91_SOC(SAMA5D2_CIDR_MATCH, AT91_CIDR_MATCH_MASK,
		 AT91_CIDR_VERSION_MASK, SAMA5D22CU_EXID_MATCH,
		 "sama5d22", "sama5d2"),
	AT91_SOC(SAMA5D2_CIDR_MATCH, AT91_CIDR_MATCH_MASK,
		 AT91_CIDR_VERSION_MASK, SAMA5D225C_D1M_EXID_MATCH,
		 "sama5d225c 16MiB SiP", "sama5d2"),
	AT91_SOC(SAMA5D2_CIDR_MATCH, AT91_CIDR_MATCH_MASK,
		 AT91_CIDR_VERSION_MASK, SAMA5D23CU_EXID_MATCH,
		 "sama5d23", "sama5d2"),
	AT91_SOC(SAMA5D2_CIDR_MATCH, AT91_CIDR_MATCH_MASK,
		 AT91_CIDR_VERSION_MASK, SAMA5D24CX_EXID_MATCH,
		 "sama5d24", "sama5d2"),
	AT91_SOC(SAMA5D2_CIDR_MATCH, AT91_CIDR_MATCH_MASK,
		 AT91_CIDR_VERSION_MASK, SAMA5D24CU_EXID_MATCH,
		 "sama5d24", "sama5d2"),
	AT91_SOC(SAMA5D2_CIDR_MATCH, AT91_CIDR_MATCH_MASK,
		 AT91_CIDR_VERSION_MASK, SAMA5D26CU_EXID_MATCH,
		 "sama5d26", "sama5d2"),
	AT91_SOC(SAMA5D2_CIDR_MATCH, AT91_CIDR_MATCH_MASK,
		 AT91_CIDR_VERSION_MASK, SAMA5D27CU_EXID_MATCH,
		 "sama5d27", "sama5d2"),
	AT91_SOC(SAMA5D2_CIDR_MATCH, AT91_CIDR_MATCH_MASK,
		 AT91_CIDR_VERSION_MASK, SAMA5D27CN_EXID_MATCH,
		 "sama5d27", "sama5d2"),
	AT91_SOC(SAMA5D2_CIDR_MATCH, AT91_CIDR_MATCH_MASK,
		 AT91_CIDR_VERSION_MASK, SAMA5D27C_D1G_EXID_MATCH,
		 "sama5d27c 128MiB SiP", "sama5d2"),
	AT91_SOC(SAMA5D2_CIDR_MATCH, AT91_CIDR_MATCH_MASK,
		 AT91_CIDR_VERSION_MASK, SAMA5D27C_D5M_EXID_MATCH,
		 "sama5d27c 64MiB SiP", "sama5d2"),
	AT91_SOC(SAMA5D2_CIDR_MATCH, AT91_CIDR_MATCH_MASK,
		 AT91_CIDR_VERSION_MASK, SAMA5D27C_LD1G_EXID_MATCH,
		 "sama5d27c 128MiB LPDDR2 SiP", "sama5d2"),
	AT91_SOC(SAMA5D2_CIDR_MATCH, AT91_CIDR_MATCH_MASK,
		 AT91_CIDR_VERSION_MASK, SAMA5D27C_LD2G_EXID_MATCH,
		 "sama5d27c 256MiB LPDDR2 SiP", "sama5d2"),
	AT91_SOC(SAMA5D2_CIDR_MATCH, AT91_CIDR_MATCH_MASK,
		 AT91_CIDR_VERSION_MASK, SAMA5D28CU_EXID_MATCH,
		 "sama5d28", "sama5d2"),
	AT91_SOC(SAMA5D2_CIDR_MATCH, AT91_CIDR_MATCH_MASK,
		 AT91_CIDR_VERSION_MASK, SAMA5D28CN_EXID_MATCH,
		 "sama5d28", "sama5d2"),
	AT91_SOC(SAMA5D2_CIDR_MATCH, AT91_CIDR_MATCH_MASK,
		 AT91_CIDR_VERSION_MASK, SAMA5D28C_D1G_EXID_MATCH,
		 "sama5d28c 128MiB SiP", "sama5d2"),
	AT91_SOC(SAMA5D2_CIDR_MATCH, AT91_CIDR_MATCH_MASK,
		 AT91_CIDR_VERSION_MASK, SAMA5D28C_LD1G_EXID_MATCH,
		 "sama5d28c 128MiB LPDDR2 SiP", "sama5d2"),
	AT91_SOC(SAMA5D2_CIDR_MATCH, AT91_CIDR_MATCH_MASK,
		 AT91_CIDR_VERSION_MASK, SAMA5D28C_LD2G_EXID_MATCH,
		 "sama5d28c 256MiB LPDDR2 SiP", "sama5d2"),
	AT91_SOC(SAMA5D3_CIDR_MATCH, AT91_CIDR_MATCH_MASK,
		 AT91_CIDR_VERSION_MASK, SAMA5D31_EXID_MATCH,
		 "sama5d31", "sama5d3"),
	AT91_SOC(SAMA5D3_CIDR_MATCH, AT91_CIDR_MATCH_MASK,
		 AT91_CIDR_VERSION_MASK, SAMA5D33_EXID_MATCH,
		 "sama5d33", "sama5d3"),
	AT91_SOC(SAMA5D3_CIDR_MATCH, AT91_CIDR_MATCH_MASK,
		 AT91_CIDR_VERSION_MASK, SAMA5D34_EXID_MATCH,
		 "sama5d34", "sama5d3"),
	AT91_SOC(SAMA5D3_CIDR_MATCH, AT91_CIDR_MATCH_MASK,
		 AT91_CIDR_VERSION_MASK, SAMA5D35_EXID_MATCH,
		 "sama5d35", "sama5d3"),
	AT91_SOC(SAMA5D3_CIDR_MATCH, AT91_CIDR_MATCH_MASK,
		 AT91_CIDR_VERSION_MASK, SAMA5D36_EXID_MATCH,
		 "sama5d36", "sama5d3"),
	AT91_SOC(SAMA5D4_CIDR_MATCH, AT91_CIDR_MATCH_MASK,
		 AT91_CIDR_VERSION_MASK, SAMA5D41_EXID_MATCH,
		 "sama5d41", "sama5d4"),
	AT91_SOC(SAMA5D4_CIDR_MATCH, AT91_CIDR_MATCH_MASK,
		 AT91_CIDR_VERSION_MASK, SAMA5D42_EXID_MATCH,
		 "sama5d42", "sama5d4"),
	AT91_SOC(SAMA5D4_CIDR_MATCH, AT91_CIDR_MATCH_MASK,
		 AT91_CIDR_VERSION_MASK, SAMA5D43_EXID_MATCH,
		 "sama5d43", "sama5d4"),
	AT91_SOC(SAMA5D4_CIDR_MATCH, AT91_CIDR_MATCH_MASK,
		 AT91_CIDR_VERSION_MASK, SAMA5D44_EXID_MATCH,
		 "sama5d44", "sama5d4"),
#endif
#ifdef CONFIG_SOC_SAMV7
	AT91_SOC(SAME70Q21_CIDR_MATCH, AT91_CIDR_MATCH_MASK,
		 AT91_CIDR_VERSION_MASK, SAME70Q21_EXID_MATCH,
		 "same70q21", "same7"),
	AT91_SOC(SAME70Q20_CIDR_MATCH, AT91_CIDR_MATCH_MASK,
		 AT91_CIDR_VERSION_MASK, SAME70Q20_EXID_MATCH,
		 "same70q20", "same7"),
	AT91_SOC(SAME70Q19_CIDR_MATCH, AT91_CIDR_MATCH_MASK,
		 AT91_CIDR_VERSION_MASK, SAME70Q19_EXID_MATCH,
		 "same70q19", "same7"),
	AT91_SOC(SAMS70Q21_CIDR_MATCH, AT91_CIDR_MATCH_MASK,
		 AT91_CIDR_VERSION_MASK, SAMS70Q21_EXID_MATCH,
		 "sams70q21", "sams7"),
	AT91_SOC(SAMS70Q20_CIDR_MATCH, AT91_CIDR_MATCH_MASK,
		 AT91_CIDR_VERSION_MASK, SAMS70Q20_EXID_MATCH,
		 "sams70q20", "sams7"),
	AT91_SOC(SAMS70Q19_CIDR_MATCH, AT91_CIDR_MATCH_MASK,
		 AT91_CIDR_VERSION_MASK, SAMS70Q19_EXID_MATCH,
		 "sams70q19", "sams7"),
	AT91_SOC(SAMV71Q21_CIDR_MATCH, AT91_CIDR_MATCH_MASK,
		 AT91_CIDR_VERSION_MASK, SAMV71Q21_EXID_MATCH,
		 "samv71q21", "samv7"),
	AT91_SOC(SAMV71Q20_CIDR_MATCH, AT91_CIDR_MATCH_MASK,
		 AT91_CIDR_VERSION_MASK, SAMV71Q20_EXID_MATCH,
		 "samv71q20", "samv7"),
	AT91_SOC(SAMV71Q19_CIDR_MATCH, AT91_CIDR_MATCH_MASK,
		 AT91_CIDR_VERSION_MASK, SAMV71Q19_EXID_MATCH,
		 "samv71q19", "samv7"),
	AT91_SOC(SAMV70Q20_CIDR_MATCH, AT91_CIDR_MATCH_MASK,
		 AT91_CIDR_VERSION_MASK, SAMV70Q20_EXID_MATCH,
		 "samv70q20", "samv7"),
	AT91_SOC(SAMV70Q19_CIDR_MATCH, AT91_CIDR_MATCH_MASK,
		 AT91_CIDR_VERSION_MASK, SAMV70Q19_EXID_MATCH,
		 "samv70q19", "samv7"),
#endif
#ifdef CONFIG_SOC_SAMA7
	AT91_SOC(SAMA7G5_CIDR_MATCH, AT91_CIDR_MATCH_MASK,
		 AT91_CIDR_VERSION_MASK_SAMA7G5, SAMA7G51_EXID_MATCH,
		 "sama7g51", "sama7g5"),
	AT91_SOC(SAMA7G5_CIDR_MATCH, AT91_CIDR_MATCH_MASK,
		 AT91_CIDR_VERSION_MASK_SAMA7G5, SAMA7G52_EXID_MATCH,
		 "sama7g52", "sama7g5"),
	AT91_SOC(SAMA7G5_CIDR_MATCH, AT91_CIDR_MATCH_MASK,
		 AT91_CIDR_VERSION_MASK_SAMA7G5, SAMA7G53_EXID_MATCH,
		 "sama7g53", "sama7g5"),
	AT91_SOC(SAMA7G5_CIDR_MATCH, AT91_CIDR_MATCH_MASK,
		 AT91_CIDR_VERSION_MASK_SAMA7G5, SAMA7G54_EXID_MATCH,
		 "sama7g54", "sama7g5"),
#endif
	{ /* sentinel */ },
};

static int __init at91_get_cidr_exid_from_dbgu(u32 *cidr, u32 *exid)
{
	struct device_node *np;
	void __iomem *regs;

	np = of_find_compatible_node(NULL, NULL, "atmel,at91rm9200-dbgu");
	if (!np)
		np = of_find_compatible_node(NULL, NULL,
					     "atmel,at91sam9260-dbgu");
	if (!np)
		return -ENODEV;

	regs = of_iomap(np, 0);
	of_node_put(np);

	if (!regs) {
		pr_warn("Could not map DBGU iomem range");
		return -ENXIO;
	}

	*cidr = readl(regs + AT91_DBGU_CIDR);
	*exid = readl(regs + AT91_DBGU_EXID);

	iounmap(regs);

	return 0;
}

static int __init at91_get_cidr_exid_from_chipid(u32 *cidr, u32 *exid)
{
	struct device_node *np;
	void __iomem *regs;
	static const struct of_device_id chipids[] = {
		{ .compatible = "atmel,sama5d2-chipid" },
		{ .compatible = "microchip,sama7g5-chipid" },
		{ },
	};

	np = of_find_matching_node(NULL, chipids);
	if (!np)
		return -ENODEV;

	regs = of_iomap(np, 0);
	of_node_put(np);

	if (!regs) {
		pr_warn("Could not map DBGU iomem range");
		return -ENXIO;
	}

	*cidr = readl(regs + AT91_CHIPID_CIDR);
	*exid = readl(regs + AT91_CHIPID_EXID);

	iounmap(regs);

	return 0;
}

struct soc_device * __init at91_soc_init(const struct at91_soc *socs)
{
	struct soc_device_attribute *soc_dev_attr;
	const struct at91_soc *soc;
	struct soc_device *soc_dev;
	u32 cidr, exid;
	int ret;

	/*
	 * With SAMA5D2 and later SoCs, CIDR and EXID registers are no more
	 * in the dbgu device but in the chipid device whose purpose is only
	 * to expose these two registers.
	 */
	ret = at91_get_cidr_exid_from_dbgu(&cidr, &exid);
	if (ret)
		ret = at91_get_cidr_exid_from_chipid(&cidr, &exid);
	if (ret) {
		if (ret == -ENODEV)
			pr_warn("Could not find identification node");
		return NULL;
	}

	for (soc = socs; soc->name; soc++) {
		if (soc->cidr_match != (cidr & soc->cidr_mask))
			continue;

		if (!(cidr & AT91_CIDR_EXT) || soc->exid_match == exid)
			break;
	}

	if (!soc->name) {
		pr_warn("Could not find matching SoC description\n");
		return NULL;
	}

	soc_dev_attr = kzalloc(sizeof(*soc_dev_attr), GFP_KERNEL);
	if (!soc_dev_attr)
		return NULL;

	soc_dev_attr->family = soc->family;
	soc_dev_attr->soc_id = soc->name;
	soc_dev_attr->revision = kasprintf(GFP_KERNEL, "%X",
					   AT91_CIDR_VERSION(cidr, soc->version_mask));
	soc_dev = soc_device_register(soc_dev_attr);
	if (IS_ERR(soc_dev)) {
		kfree(soc_dev_attr->revision);
		kfree(soc_dev_attr);
		pr_warn("Could not register SoC device\n");
		return NULL;
	}

	if (soc->family)
		pr_info("Detected SoC family: %s\n", soc->family);
	pr_info("Detected SoC: %s, revision %X\n", soc->name,
		AT91_CIDR_VERSION(cidr, soc->version_mask));

	return soc_dev;
}

static const struct of_device_id at91_soc_allowed_list[] __initconst = {
	{ .compatible = "atmel,at91rm9200", },
	{ .compatible = "atmel,at91sam9", },
	{ .compatible = "atmel,sama5", },
	{ .compatible = "atmel,samv7", },
	{ .compatible = "microchip,sama7g5", },
	{ }
};

static int __init atmel_soc_device_init(void)
{
	struct device_node *np = of_find_node_by_path("/");

	if (!of_match_node(at91_soc_allowed_list, np))
		return 0;

	at91_soc_init(socs);

	return 0;
}
subsys_initcall(atmel_soc_device_init);
