/*
 * EIM driver for Freescale's i.MX chips
 *
 * Copyright (C) 2013 Freescale Semiconductor, Inc.
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2. This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */
#include <linux/module.h>
#include <linux/clk.h>
#include <linux/io.h>
#include <linux/of_device.h>
#include <linux/mfd/syscon.h>
#include <linux/mfd/syscon/imx6q-iomuxc-gpr.h>
#include <linux/regmap.h>

struct imx_weim_devtype {
	unsigned int	cs_count;
	unsigned int	cs_regs_count;
	unsigned int	cs_stride;
	unsigned int	wcr_offset;
	unsigned int	wcr_bcm;
};

static const struct imx_weim_devtype imx1_weim_devtype = {
	.cs_count	= 6,
	.cs_regs_count	= 2,
	.cs_stride	= 0x08,
};

static const struct imx_weim_devtype imx27_weim_devtype = {
	.cs_count	= 6,
	.cs_regs_count	= 3,
	.cs_stride	= 0x10,
};

static const struct imx_weim_devtype imx50_weim_devtype = {
	.cs_count	= 4,
	.cs_regs_count	= 6,
	.cs_stride	= 0x18,
	.wcr_offset	= 0x90,
	.wcr_bcm	= BIT(0),
};

static const struct imx_weim_devtype imx51_weim_devtype = {
	.cs_count	= 6,
	.cs_regs_count	= 6,
	.cs_stride	= 0x18,
};

#define MAX_CS_REGS_COUNT	6
#define MAX_CS_COUNT		6
#define OF_REG_SIZE		3

struct cs_timing {
	bool is_applied;
	u32 regs[MAX_CS_REGS_COUNT];
};

struct cs_timing_state {
	struct cs_timing cs[MAX_CS_COUNT];
};

static const struct of_device_id weim_id_table[] = {
	/* i.MX1/21 */
	{ .compatible = "fsl,imx1-weim", .data = &imx1_weim_devtype, },
	/* i.MX25/27/31/35 */
	{ .compatible = "fsl,imx27-weim", .data = &imx27_weim_devtype, },
	/* i.MX50/53/6Q */
	{ .compatible = "fsl,imx50-weim", .data = &imx50_weim_devtype, },
	{ .compatible = "fsl,imx6q-weim", .data = &imx50_weim_devtype, },
	/* i.MX51 */
	{ .compatible = "fsl,imx51-weim", .data = &imx51_weim_devtype, },
	{ }
};
MODULE_DEVICE_TABLE(of, weim_id_table);

static int imx_weim_gpr_setup(struct platform_device *pdev)
{
	struct device_node *np = pdev->dev.of_node;
	struct property *prop;
	const __be32 *p;
	struct regmap *gpr;
	u32 gprvals[4] = {
		05,	/* CS0(128M) CS1(0M)  CS2(0M)  CS3(0M)  */
		033,	/* CS0(64M)  CS1(64M) CS2(0M)  CS3(0M)  */
		0113,	/* CS0(64M)  CS1(32M) CS2(32M) CS3(0M)  */
		01111,	/* CS0(32M)  CS1(32M) CS2(32M) CS3(32M) */
	};
	u32 gprval = 0;
	u32 val;
	int cs = 0;
	int i = 0;

	gpr = syscon_regmap_lookup_by_phandle(np, "fsl,weim-cs-gpr");
	if (IS_ERR(gpr)) {
		dev_dbg(&pdev->dev, "failed to find weim-cs-gpr\n");
		return 0;
	}

	of_property_for_each_u32(np, "ranges", prop, p, val) {
		if (i % 4 == 0) {
			cs = val;
		} else if (i % 4 == 3 && val) {
			val = (val / SZ_32M) | 1;
			gprval |= val << cs * 3;
		}
		i++;
	}

	if (i == 0 || i % 4)
		goto err;

	for (i = 0; i < ARRAY_SIZE(gprvals); i++) {
		if (gprval == gprvals[i]) {
			/* Found it. Set up IOMUXC_GPR1[11:0] with it. */
			regmap_update_bits(gpr, IOMUXC_GPR1, 0xfff, gprval);
			return 0;
		}
	}

err:
	dev_err(&pdev->dev, "Invalid 'ranges' configuration\n");
	return -EINVAL;
}

/* Parse and set the timing for this device. */
static int weim_timing_setup(struct device *dev,
			     struct device_node *np, void __iomem *base,
			     const struct imx_weim_devtype *devtype,
			     struct cs_timing_state *ts)
{
	u32 cs_idx, value[MAX_CS_REGS_COUNT];
	int i, ret;
	int reg_idx, num_regs;
	struct cs_timing *cst;

	if (WARN_ON(devtype->cs_regs_count > MAX_CS_REGS_COUNT))
		return -EINVAL;
	if (WARN_ON(devtype->cs_count > MAX_CS_COUNT))
		return -EINVAL;

	ret = of_property_read_u32_array(np, "fsl,weim-cs-timing",
					 value, devtype->cs_regs_count);
	if (ret)
		return ret;

	/*
	 * the child node's "reg" property may contain multiple address ranges,
	 * extract the chip select for each.
	 */
	num_regs = of_property_count_elems_of_size(np, "reg", OF_REG_SIZE);
	if (num_regs < 0)
		return num_regs;
	if (!num_regs)
		return -EINVAL;
	for (reg_idx = 0; reg_idx < num_regs; reg_idx++) {
		/* get the CS index from this child node's "reg" property. */
		ret = of_property_read_u32_index(np, "reg",
					reg_idx * OF_REG_SIZE, &cs_idx);
		if (ret)
			break;

		if (cs_idx >= devtype->cs_count)
			return -EINVAL;

		/* prevent re-configuring a CS that's already been configured */
		cst = &ts->cs[cs_idx];
		if (cst->is_applied && memcmp(value, cst->regs,
					devtype->cs_regs_count * sizeof(u32))) {
			dev_err(dev, "fsl,weim-cs-timing conflict on %pOF", np);
			return -EINVAL;
		}

		/* set the timing for WEIM */
		for (i = 0; i < devtype->cs_regs_count; i++)
			writel(value[i],
				base + cs_idx * devtype->cs_stride + i * 4);
		if (!cst->is_applied) {
			cst->is_applied = true;
			memcpy(cst->regs, value,
				devtype->cs_regs_count * sizeof(u32));
		}
	}

	return 0;
}

static int weim_parse_dt(struct platform_device *pdev, void __iomem *base)
{
	const struct of_device_id *of_id = of_match_device(weim_id_table,
							   &pdev->dev);
	const struct imx_weim_devtype *devtype = of_id->data;
	struct device_node *child;
	int ret, have_child = 0;
	struct cs_timing_state ts = {};
	u32 reg;

	if (devtype == &imx50_weim_devtype) {
		ret = imx_weim_gpr_setup(pdev);
		if (ret)
			return ret;
	}

	if (of_property_read_bool(pdev->dev.of_node, "fsl,burst-clk-enable")) {
		if (devtype->wcr_bcm) {
			reg = readl(base + devtype->wcr_offset);
			writel(reg | devtype->wcr_bcm,
				base + devtype->wcr_offset);
		} else {
			dev_err(&pdev->dev, "burst clk mode not supported.\n");
			return -EINVAL;
		}
	}

	for_each_available_child_of_node(pdev->dev.of_node, child) {
		ret = weim_timing_setup(&pdev->dev, child, base, devtype, &ts);
		if (ret)
			dev_warn(&pdev->dev, "%pOF set timing failed.\n",
				child);
		else
			have_child = 1;
	}

	if (have_child)
		ret = of_platform_default_populate(pdev->dev.of_node,
						   NULL, &pdev->dev);
	if (ret)
		dev_err(&pdev->dev, "%pOF fail to create devices.\n",
			pdev->dev.of_node);
	return ret;
}

static int weim_probe(struct platform_device *pdev)
{
	struct resource *res;
	struct clk *clk;
	void __iomem *base;
	int ret;

	/* get the resource */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(base))
		return PTR_ERR(base);

	/* get the clock */
	clk = devm_clk_get(&pdev->dev, NULL);
	if (IS_ERR(clk))
		return PTR_ERR(clk);

	ret = clk_prepare_enable(clk);
	if (ret)
		return ret;

	/* parse the device node */
	ret = weim_parse_dt(pdev, base);
	if (ret)
		clk_disable_unprepare(clk);
	else
		dev_info(&pdev->dev, "Driver registered.\n");

	return ret;
}

static struct platform_driver weim_driver = {
	.driver = {
		.name		= "imx-weim",
		.of_match_table	= weim_id_table,
	},
	.probe = weim_probe,
};
module_platform_driver(weim_driver);

MODULE_AUTHOR("Freescale Semiconductor Inc.");
MODULE_DESCRIPTION("i.MX EIM Controller Driver");
MODULE_LICENSE("GPL");
