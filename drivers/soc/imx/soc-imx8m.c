// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright 2019 NXP.
 */

#include <linux/init.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/nvmem-consumer.h>
#include <linux/of_address.h>
#include <linux/slab.h>
#include <linux/sys_soc.h>
#include <linux/platform_device.h>
#include <linux/arm-smccc.h>
#include <linux/of.h>

#define REV_B1				0x21

#define IMX8MQ_SW_INFO_B1		0x40
#define IMX8MQ_SW_MAGIC_B1		0xff0055aa

#define IMX_SIP_GET_SOC_INFO		0xc2000006

#define OCOTP_UID_LOW			0x410
#define OCOTP_UID_HIGH			0x420

#define IMX8MP_OCOTP_UID_OFFSET		0x10

/* Same as ANADIG_DIGPROG_IMX7D */
#define ANADIG_DIGPROG_IMX8MM	0x800

struct imx8_soc_data {
	char *name;
	u32 (*soc_revision)(struct device *dev);
};

static u64 soc_uid;

#ifdef CONFIG_HAVE_ARM_SMCCC
static u32 imx8mq_soc_revision_from_atf(void)
{
	struct arm_smccc_res res;

	arm_smccc_smc(IMX_SIP_GET_SOC_INFO, 0, 0, 0, 0, 0, 0, 0, &res);

	if (res.a0 == SMCCC_RET_NOT_SUPPORTED)
		return 0;
	else
		return res.a0 & 0xff;
}
#else
static inline u32 imx8mq_soc_revision_from_atf(void) { return 0; };
#endif

static u32 __init imx8mq_soc_revision(struct device *dev)
{
	struct device_node *np;
	void __iomem *ocotp_base;
	u32 magic;
	u32 rev;

	np = of_find_compatible_node(NULL, NULL, "fsl,imx8mq-ocotp");
	if (!np)
		return 0;

	ocotp_base = of_iomap(np, 0);
	WARN_ON(!ocotp_base);

	/*
	 * SOC revision on older imx8mq is not available in fuses so query
	 * the value from ATF instead.
	 */
	rev = imx8mq_soc_revision_from_atf();
	if (!rev) {
		magic = readl_relaxed(ocotp_base + IMX8MQ_SW_INFO_B1);
		if (magic == IMX8MQ_SW_MAGIC_B1)
			rev = REV_B1;
	}

	if (dev) {
		int ret;

		ret = nvmem_cell_read_u64(dev, "soc_unique_id", &soc_uid);
		if (ret) {
			iounmap(ocotp_base);
			of_node_put(np);
			return ret;
		}
	} else {
		soc_uid = readl_relaxed(ocotp_base + OCOTP_UID_HIGH);
		soc_uid <<= 32;
		soc_uid |= readl_relaxed(ocotp_base + OCOTP_UID_LOW);
	}

	iounmap(ocotp_base);
	of_node_put(np);

	return rev;
}

static void __init imx8mm_soc_uid(void)
{
	void __iomem *ocotp_base;
	struct device_node *np;
	u32 offset = of_machine_is_compatible("fsl,imx8mp") ?
		     IMX8MP_OCOTP_UID_OFFSET : 0;

	np = of_find_compatible_node(NULL, NULL, "fsl,imx8mm-ocotp");
	if (!np)
		return;

	ocotp_base = of_iomap(np, 0);
	WARN_ON(!ocotp_base);

	soc_uid = readl_relaxed(ocotp_base + OCOTP_UID_HIGH + offset);
	soc_uid <<= 32;
	soc_uid |= readl_relaxed(ocotp_base + OCOTP_UID_LOW + offset);

	iounmap(ocotp_base);
	of_node_put(np);
}

static u32 __init imx8mm_soc_revision(struct device *dev)
{
	struct device_node *np;
	void __iomem *anatop_base;
	u32 rev;

	np = of_find_compatible_node(NULL, NULL, "fsl,imx8mm-anatop");
	if (!np)
		return 0;

	anatop_base = of_iomap(np, 0);
	WARN_ON(!anatop_base);

	rev = readl_relaxed(anatop_base + ANADIG_DIGPROG_IMX8MM);

	iounmap(anatop_base);
	of_node_put(np);

	if (dev) {
		int ret;

		ret = nvmem_cell_read_u64(dev, "soc_unique_id", &soc_uid);
		if (ret)
			return ret;
	} else {
		imx8mm_soc_uid();
	}

	return rev;
}

static const struct imx8_soc_data imx8mq_soc_data = {
	.name = "i.MX8MQ",
	.soc_revision = imx8mq_soc_revision,
};

static const struct imx8_soc_data imx8mm_soc_data = {
	.name = "i.MX8MM",
	.soc_revision = imx8mm_soc_revision,
};

static const struct imx8_soc_data imx8mn_soc_data = {
	.name = "i.MX8MN",
	.soc_revision = imx8mm_soc_revision,
};

static const struct imx8_soc_data imx8mp_soc_data = {
	.name = "i.MX8MP",
	.soc_revision = imx8mm_soc_revision,
};

static __maybe_unused const struct of_device_id imx8_machine_match[] = {
	{ .compatible = "fsl,imx8mq", .data = &imx8mq_soc_data, },
	{ .compatible = "fsl,imx8mm", .data = &imx8mm_soc_data, },
	{ .compatible = "fsl,imx8mn", .data = &imx8mn_soc_data, },
	{ .compatible = "fsl,imx8mp", .data = &imx8mp_soc_data, },
	{ }
};

static __maybe_unused const struct of_device_id imx8_soc_match[] = {
	{ .compatible = "fsl,imx8mq-soc", .data = &imx8mq_soc_data, },
	{ .compatible = "fsl,imx8mm-soc", .data = &imx8mm_soc_data, },
	{ .compatible = "fsl,imx8mn-soc", .data = &imx8mn_soc_data, },
	{ .compatible = "fsl,imx8mp-soc", .data = &imx8mp_soc_data, },
	{ }
};

#define imx8_revision(soc_rev) \
	soc_rev ? \
	kasprintf(GFP_KERNEL, "%d.%d", (soc_rev >> 4) & 0xf,  soc_rev & 0xf) : \
	"unknown"

static int imx8_soc_info(struct platform_device *pdev)
{
	struct soc_device_attribute *soc_dev_attr;
	struct soc_device *soc_dev;
	const struct of_device_id *id;
	u32 soc_rev = 0;
	const struct imx8_soc_data *data;
	int ret;

	soc_dev_attr = kzalloc(sizeof(*soc_dev_attr), GFP_KERNEL);
	if (!soc_dev_attr)
		return -ENOMEM;

	soc_dev_attr->family = "Freescale i.MX";

	ret = of_property_read_string(of_root, "model", &soc_dev_attr->machine);
	if (ret)
		goto free_soc;

	if (pdev)
		id = of_match_node(imx8_soc_match, pdev->dev.of_node);
	else
		id = of_match_node(imx8_machine_match, of_root);
	if (!id) {
		ret = -ENODEV;
		goto free_soc;
	}

	data = id->data;
	if (data) {
		soc_dev_attr->soc_id = data->name;
		if (data->soc_revision) {
			if (pdev) {
				soc_rev = data->soc_revision(&pdev->dev);
				ret = soc_rev;
				if (ret < 0)
					goto free_soc;
			} else {
				soc_rev = data->soc_revision(NULL);
			}
		}
	}

	soc_dev_attr->revision = imx8_revision(soc_rev);
	if (!soc_dev_attr->revision) {
		ret = -ENOMEM;
		goto free_soc;
	}

	soc_dev_attr->serial_number = kasprintf(GFP_KERNEL, "%016llX", soc_uid);
	if (!soc_dev_attr->serial_number) {
		ret = -ENOMEM;
		goto free_rev;
	}

	soc_dev = soc_device_register(soc_dev_attr);
	if (IS_ERR(soc_dev)) {
		ret = PTR_ERR(soc_dev);
		goto free_serial_number;
	}

	pr_info("SoC: %s revision %s\n", soc_dev_attr->soc_id,
		soc_dev_attr->revision);

	if (IS_ENABLED(CONFIG_ARM_IMX_CPUFREQ_DT))
		platform_device_register_simple("imx-cpufreq-dt", -1, NULL, 0);

	return 0;

free_serial_number:
	kfree(soc_dev_attr->serial_number);
free_rev:
	if (strcmp(soc_dev_attr->revision, "unknown"))
		kfree(soc_dev_attr->revision);
free_soc:
	kfree(soc_dev_attr);
	return ret;
}

/* Retain device_initcall is for backward compatibility with DTS. */
static int __init imx8_soc_init(void)
{
	if (of_find_matching_node_and_match(NULL, imx8_soc_match, NULL))
		return 0;

	return imx8_soc_info(NULL);
}
device_initcall(imx8_soc_init);

static struct platform_driver imx8_soc_info_driver = {
	.probe = imx8_soc_info,
	.driver = {
		.name = "imx8_soc_info",
		.of_match_table = imx8_soc_match,
	},
};

module_platform_driver(imx8_soc_info_driver);
MODULE_LICENSE("GPL v2");
