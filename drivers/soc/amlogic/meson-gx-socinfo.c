/*
 * Copyright (c) 2017 BayLibre, SAS
 * Author: Neil Armstrong <narmstrong@baylibre.com>
 *
 * SPDX-License-Identifier: GPL-2.0+
 */

#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/sys_soc.h>
#include <linux/bitfield.h>
#include <linux/regmap.h>
#include <linux/mfd/syscon.h>

#define AO_SEC_SD_CFG8		0xe0
#define AO_SEC_SOCINFO_OFFSET	AO_SEC_SD_CFG8

#define SOCINFO_MAJOR	GENMASK(31, 24)
#define SOCINFO_PACK	GENMASK(23, 16)
#define SOCINFO_MINOR	GENMASK(15, 8)
#define SOCINFO_MISC	GENMASK(7, 0)

static const struct meson_gx_soc_id {
	const char *name;
	unsigned int id;
} soc_ids[] = {
	{ "GXBB", 0x1f },
	{ "GXTVBB", 0x20 },
	{ "GXL", 0x21 },
	{ "GXM", 0x22 },
	{ "TXL", 0x23 },
	{ "TXLX", 0x24 },
	{ "AXG", 0x25 },
	{ "GXLX", 0x26 },
	{ "TXHD", 0x27 },
	{ "G12A", 0x28 },
	{ "G12B", 0x29 },
	{ "SM1", 0x2b },
	{ "A1", 0x2c },
};

static const struct meson_gx_package_id {
	const char *name;
	unsigned int major_id;
	unsigned int pack_id;
	unsigned int pack_mask;
} soc_packages[] = {
	{ "S905", 0x1f, 0, 0x20 }, /* pack_id != 0x20 */
	{ "S905H", 0x1f, 0x3, 0xf }, /* pack_id & 0xf == 0x3 */
	{ "S905M", 0x1f, 0x20, 0xf0 }, /* pack_id == 0x20 */
	{ "S905D", 0x21, 0, 0xf0 },
	{ "S905X", 0x21, 0x80, 0xf0 },
	{ "S905W", 0x21, 0xa0, 0xf0 },
	{ "S905L", 0x21, 0xc0, 0xf0 },
	{ "S905M2", 0x21, 0xe0, 0xf0 },
	{ "S805X", 0x21, 0x30, 0xf0 },
	{ "S805Y", 0x21, 0xb0, 0xf0 },
	{ "S912", 0x22, 0, 0x0 }, /* Only S912 is known for GXM */
	{ "962X", 0x24, 0x10, 0xf0 },
	{ "962E", 0x24, 0x20, 0xf0 },
	{ "A113X", 0x25, 0x37, 0xff },
	{ "A113D", 0x25, 0x22, 0xff },
	{ "S905D2", 0x28, 0x10, 0xf0 },
	{ "S905X2", 0x28, 0x40, 0xf0 },
	{ "A311D", 0x29, 0x10, 0xf0 },
	{ "S922X", 0x29, 0x40, 0xf0 },
	{ "S905D3", 0x2b, 0x4, 0xf5 },
	{ "S905X3", 0x2b, 0x5, 0xf5 },
	{ "S905X3", 0x2b, 0x10, 0x3f },
	{ "S905D3", 0x2b, 0x30, 0x3f },
	{ "A113L", 0x2c, 0x0, 0xf8 },
};

static inline unsigned int socinfo_to_major(u32 socinfo)
{
	return FIELD_GET(SOCINFO_MAJOR, socinfo);
}

static inline unsigned int socinfo_to_minor(u32 socinfo)
{
	return FIELD_GET(SOCINFO_MINOR, socinfo);
}

static inline unsigned int socinfo_to_pack(u32 socinfo)
{
	return FIELD_GET(SOCINFO_PACK, socinfo);
}

static inline unsigned int socinfo_to_misc(u32 socinfo)
{
	return FIELD_GET(SOCINFO_MISC, socinfo);
}

static const char *socinfo_to_package_id(u32 socinfo)
{
	unsigned int pack = socinfo_to_pack(socinfo);
	unsigned int major = socinfo_to_major(socinfo);
	int i;

	for (i = 0 ; i < ARRAY_SIZE(soc_packages) ; ++i) {
		if (soc_packages[i].major_id == major &&
		    soc_packages[i].pack_id ==
				(pack & soc_packages[i].pack_mask))
			return soc_packages[i].name;
	}

	return "Unknown";
}

static const char *socinfo_to_soc_id(u32 socinfo)
{
	unsigned int id = socinfo_to_major(socinfo);
	int i;

	for (i = 0 ; i < ARRAY_SIZE(soc_ids) ; ++i) {
		if (soc_ids[i].id == id)
			return soc_ids[i].name;
	}

	return "Unknown";
}

static int __init meson_gx_socinfo_init(void)
{
	struct soc_device_attribute *soc_dev_attr;
	struct soc_device *soc_dev;
	struct device_node *np;
	struct regmap *regmap;
	unsigned int socinfo;
	struct device *dev;
	int ret;

	/* look up for chipid node */
	np = of_find_compatible_node(NULL, NULL, "amlogic,meson-gx-ao-secure");
	if (!np)
		return -ENODEV;

	/* check if interface is enabled */
	if (!of_device_is_available(np)) {
		of_node_put(np);
		return -ENODEV;
	}

	/* check if chip-id is available */
	if (!of_property_read_bool(np, "amlogic,has-chip-id")) {
		of_node_put(np);
		return -ENODEV;
	}

	/* node should be a syscon */
	regmap = syscon_node_to_regmap(np);
	of_node_put(np);
	if (IS_ERR(regmap)) {
		pr_err("%s: failed to get regmap\n", __func__);
		return -ENODEV;
	}

	ret = regmap_read(regmap, AO_SEC_SOCINFO_OFFSET, &socinfo);
	if (ret < 0)
		return ret;

	if (!socinfo) {
		pr_err("%s: invalid chipid value\n", __func__);
		return -EINVAL;
	}

	soc_dev_attr = kzalloc(sizeof(*soc_dev_attr), GFP_KERNEL);
	if (!soc_dev_attr)
		return -ENODEV;

	soc_dev_attr->family = "Amlogic Meson";

	np = of_find_node_by_path("/");
	of_property_read_string(np, "model", &soc_dev_attr->machine);
	of_node_put(np);

	soc_dev_attr->revision = kasprintf(GFP_KERNEL, "%x:%x - %x:%x",
					   socinfo_to_major(socinfo),
					   socinfo_to_minor(socinfo),
					   socinfo_to_pack(socinfo),
					   socinfo_to_misc(socinfo));
	soc_dev_attr->soc_id = kasprintf(GFP_KERNEL, "%s (%s)",
					 socinfo_to_soc_id(socinfo),
					 socinfo_to_package_id(socinfo));

	soc_dev = soc_device_register(soc_dev_attr);
	if (IS_ERR(soc_dev)) {
		kfree(soc_dev_attr->revision);
		kfree_const(soc_dev_attr->soc_id);
		kfree(soc_dev_attr);
		return PTR_ERR(soc_dev);
	}
	dev = soc_device_to_device(soc_dev);

	dev_info(dev, "Amlogic Meson %s Revision %x:%x (%x:%x) Detected\n",
			soc_dev_attr->soc_id,
			socinfo_to_major(socinfo),
			socinfo_to_minor(socinfo),
			socinfo_to_pack(socinfo),
			socinfo_to_misc(socinfo));

	return 0;
}
device_initcall(meson_gx_socinfo_init);
