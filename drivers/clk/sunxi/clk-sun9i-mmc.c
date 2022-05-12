// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright 2015 Chen-Yu Tsai
 *
 * Chen-Yu Tsai	<wens@csie.org>
 */

#include <linux/clk.h>
#include <linux/clk-provider.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/reset.h>
#include <linux/platform_device.h>
#include <linux/reset-controller.h>
#include <linux/slab.h>
#include <linux/spinlock.h>

#define SUN9I_MMC_WIDTH		4

#define SUN9I_MMC_GATE_BIT	16
#define SUN9I_MMC_RESET_BIT	18

struct sun9i_mmc_clk_data {
	spinlock_t			lock;
	void __iomem			*membase;
	struct clk			*clk;
	struct reset_control		*reset;
	struct clk_onecell_data		clk_data;
	struct reset_controller_dev	rcdev;
};

static int sun9i_mmc_reset_assert(struct reset_controller_dev *rcdev,
			      unsigned long id)
{
	struct sun9i_mmc_clk_data *data = container_of(rcdev,
						       struct sun9i_mmc_clk_data,
						       rcdev);
	unsigned long flags;
	void __iomem *reg = data->membase + SUN9I_MMC_WIDTH * id;
	u32 val;

	clk_prepare_enable(data->clk);
	spin_lock_irqsave(&data->lock, flags);

	val = readl(reg);
	writel(val & ~BIT(SUN9I_MMC_RESET_BIT), reg);

	spin_unlock_irqrestore(&data->lock, flags);
	clk_disable_unprepare(data->clk);

	return 0;
}

static int sun9i_mmc_reset_deassert(struct reset_controller_dev *rcdev,
				unsigned long id)
{
	struct sun9i_mmc_clk_data *data = container_of(rcdev,
						       struct sun9i_mmc_clk_data,
						       rcdev);
	unsigned long flags;
	void __iomem *reg = data->membase + SUN9I_MMC_WIDTH * id;
	u32 val;

	clk_prepare_enable(data->clk);
	spin_lock_irqsave(&data->lock, flags);

	val = readl(reg);
	writel(val | BIT(SUN9I_MMC_RESET_BIT), reg);

	spin_unlock_irqrestore(&data->lock, flags);
	clk_disable_unprepare(data->clk);

	return 0;
}

static int sun9i_mmc_reset_reset(struct reset_controller_dev *rcdev,
				 unsigned long id)
{
	sun9i_mmc_reset_assert(rcdev, id);
	udelay(10);
	sun9i_mmc_reset_deassert(rcdev, id);

	return 0;
}

static const struct reset_control_ops sun9i_mmc_reset_ops = {
	.assert		= sun9i_mmc_reset_assert,
	.deassert	= sun9i_mmc_reset_deassert,
	.reset		= sun9i_mmc_reset_reset,
};

static int sun9i_a80_mmc_config_clk_probe(struct platform_device *pdev)
{
	struct device_node *np = pdev->dev.of_node;
	struct sun9i_mmc_clk_data *data;
	struct clk_onecell_data *clk_data;
	const char *clk_name = np->name;
	const char *clk_parent;
	struct resource *r;
	int count, i, ret;

	data = devm_kzalloc(&pdev->dev, sizeof(*data), GFP_KERNEL);
	if (!data)
		return -ENOMEM;

	spin_lock_init(&data->lock);

	r = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	/* one clock/reset pair per word */
	count = DIV_ROUND_UP((resource_size(r)), SUN9I_MMC_WIDTH);
	data->membase = devm_ioremap_resource(&pdev->dev, r);
	if (IS_ERR(data->membase))
		return PTR_ERR(data->membase);

	clk_data = &data->clk_data;
	clk_data->clk_num = count;
	clk_data->clks = devm_kcalloc(&pdev->dev, count, sizeof(struct clk *),
				      GFP_KERNEL);
	if (!clk_data->clks)
		return -ENOMEM;

	data->clk = devm_clk_get(&pdev->dev, NULL);
	if (IS_ERR(data->clk)) {
		dev_err(&pdev->dev, "Could not get clock\n");
		return PTR_ERR(data->clk);
	}

	data->reset = devm_reset_control_get_exclusive(&pdev->dev, NULL);
	if (IS_ERR(data->reset)) {
		dev_err(&pdev->dev, "Could not get reset control\n");
		return PTR_ERR(data->reset);
	}

	ret = reset_control_deassert(data->reset);
	if (ret) {
		dev_err(&pdev->dev, "Reset deassert err %d\n", ret);
		return ret;
	}

	clk_parent = __clk_get_name(data->clk);
	for (i = 0; i < count; i++) {
		of_property_read_string_index(np, "clock-output-names",
					      i, &clk_name);

		clk_data->clks[i] = clk_register_gate(&pdev->dev, clk_name,
						      clk_parent, 0,
						      data->membase + SUN9I_MMC_WIDTH * i,
						      SUN9I_MMC_GATE_BIT, 0,
						      &data->lock);

		if (IS_ERR(clk_data->clks[i])) {
			ret = PTR_ERR(clk_data->clks[i]);
			goto err_clk_register;
		}
	}

	ret = of_clk_add_provider(np, of_clk_src_onecell_get, clk_data);
	if (ret)
		goto err_clk_provider;

	data->rcdev.owner = THIS_MODULE;
	data->rcdev.nr_resets = count;
	data->rcdev.ops = &sun9i_mmc_reset_ops;
	data->rcdev.of_node = pdev->dev.of_node;

	ret = reset_controller_register(&data->rcdev);
	if (ret)
		goto err_rc_reg;

	platform_set_drvdata(pdev, data);

	return 0;

err_rc_reg:
	of_clk_del_provider(np);

err_clk_provider:
	for (i = 0; i < count; i++)
		clk_unregister(clk_data->clks[i]);

err_clk_register:
	reset_control_assert(data->reset);

	return ret;
}

static const struct of_device_id sun9i_a80_mmc_config_clk_dt_ids[] = {
	{ .compatible = "allwinner,sun9i-a80-mmc-config-clk" },
	{ /* sentinel */ }
};

static struct platform_driver sun9i_a80_mmc_config_clk_driver = {
	.driver = {
		.name = "sun9i-a80-mmc-config-clk",
		.suppress_bind_attrs = true,
		.of_match_table = sun9i_a80_mmc_config_clk_dt_ids,
	},
	.probe = sun9i_a80_mmc_config_clk_probe,
};
builtin_platform_driver(sun9i_a80_mmc_config_clk_driver);
