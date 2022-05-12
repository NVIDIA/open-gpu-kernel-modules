// SPDX-License-Identifier: GPL-2.0-only
/*
 * Rockchip IO Voltage Domain driver
 *
 * Copyright 2014 MundoReader S.L.
 * Copyright 2014 Google, Inc.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/err.h>
#include <linux/mfd/syscon.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>
#include <linux/regulator/consumer.h>

#define MAX_SUPPLIES		16

/*
 * The max voltage for 1.8V and 3.3V come from the Rockchip datasheet under
 * "Recommended Operating Conditions" for "Digital GPIO".   When the typical
 * is 3.3V the max is 3.6V.  When the typical is 1.8V the max is 1.98V.
 *
 * They are used like this:
 * - If the voltage on a rail is above the "1.8" voltage (1.98V) we'll tell the
 *   SoC we're at 3.3.
 * - If the voltage on a rail is above the "3.3" voltage (3.6V) we'll consider
 *   that to be an error.
 */
#define MAX_VOLTAGE_1_8		1980000
#define MAX_VOLTAGE_3_3		3600000

#define PX30_IO_VSEL			0x180
#define PX30_IO_VSEL_VCCIO6_SRC		BIT(0)
#define PX30_IO_VSEL_VCCIO6_SUPPLY_NUM	1

#define RK3288_SOC_CON2			0x24c
#define RK3288_SOC_CON2_FLASH0		BIT(7)
#define RK3288_SOC_FLASH_SUPPLY_NUM	2

#define RK3328_SOC_CON4			0x410
#define RK3328_SOC_CON4_VCCIO2		BIT(7)
#define RK3328_SOC_VCCIO2_SUPPLY_NUM	1

#define RK3368_SOC_CON15		0x43c
#define RK3368_SOC_CON15_FLASH0		BIT(14)
#define RK3368_SOC_FLASH_SUPPLY_NUM	2

#define RK3399_PMUGRF_CON0		0x180
#define RK3399_PMUGRF_CON0_VSEL		BIT(8)
#define RK3399_PMUGRF_VSEL_SUPPLY_NUM	9

struct rockchip_iodomain;

struct rockchip_iodomain_soc_data {
	int grf_offset;
	const char *supply_names[MAX_SUPPLIES];
	void (*init)(struct rockchip_iodomain *iod);
};

struct rockchip_iodomain_supply {
	struct rockchip_iodomain *iod;
	struct regulator *reg;
	struct notifier_block nb;
	int idx;
};

struct rockchip_iodomain {
	struct device *dev;
	struct regmap *grf;
	const struct rockchip_iodomain_soc_data *soc_data;
	struct rockchip_iodomain_supply supplies[MAX_SUPPLIES];
};

static int rockchip_iodomain_write(struct rockchip_iodomain_supply *supply,
				   int uV)
{
	struct rockchip_iodomain *iod = supply->iod;
	u32 val;
	int ret;

	/* set value bit */
	val = (uV > MAX_VOLTAGE_1_8) ? 0 : 1;
	val <<= supply->idx;

	/* apply hiword-mask */
	val |= (BIT(supply->idx) << 16);

	ret = regmap_write(iod->grf, iod->soc_data->grf_offset, val);
	if (ret)
		dev_err(iod->dev, "Couldn't write to GRF\n");

	return ret;
}

static int rockchip_iodomain_notify(struct notifier_block *nb,
				    unsigned long event,
				    void *data)
{
	struct rockchip_iodomain_supply *supply =
			container_of(nb, struct rockchip_iodomain_supply, nb);
	int uV;
	int ret;

	/*
	 * According to Rockchip it's important to keep the SoC IO domain
	 * higher than (or equal to) the external voltage.  That means we need
	 * to change it before external voltage changes happen in the case
	 * of an increase.
	 *
	 * Note that in the "pre" change we pick the max possible voltage that
	 * the regulator might end up at (the client requests a range and we
	 * don't know for certain the exact voltage).  Right now we rely on the
	 * slop in MAX_VOLTAGE_1_8 and MAX_VOLTAGE_3_3 to save us if clients
	 * request something like a max of 3.6V when they really want 3.3V.
	 * We could attempt to come up with better rules if this fails.
	 */
	if (event & REGULATOR_EVENT_PRE_VOLTAGE_CHANGE) {
		struct pre_voltage_change_data *pvc_data = data;

		uV = max_t(unsigned long, pvc_data->old_uV, pvc_data->max_uV);
	} else if (event & (REGULATOR_EVENT_VOLTAGE_CHANGE |
			    REGULATOR_EVENT_ABORT_VOLTAGE_CHANGE)) {
		uV = (unsigned long)data;
	} else {
		return NOTIFY_OK;
	}

	dev_dbg(supply->iod->dev, "Setting to %d\n", uV);

	if (uV > MAX_VOLTAGE_3_3) {
		dev_err(supply->iod->dev, "Voltage too high: %d\n", uV);

		if (event == REGULATOR_EVENT_PRE_VOLTAGE_CHANGE)
			return NOTIFY_BAD;
	}

	ret = rockchip_iodomain_write(supply, uV);
	if (ret && event == REGULATOR_EVENT_PRE_VOLTAGE_CHANGE)
		return NOTIFY_BAD;

	dev_dbg(supply->iod->dev, "Setting to %d done\n", uV);
	return NOTIFY_OK;
}

static void px30_iodomain_init(struct rockchip_iodomain *iod)
{
	int ret;
	u32 val;

	/* if no VCCIO6 supply we should leave things alone */
	if (!iod->supplies[PX30_IO_VSEL_VCCIO6_SUPPLY_NUM].reg)
		return;

	/*
	 * set vccio6 iodomain to also use this framework
	 * instead of a special gpio.
	 */
	val = PX30_IO_VSEL_VCCIO6_SRC | (PX30_IO_VSEL_VCCIO6_SRC << 16);
	ret = regmap_write(iod->grf, PX30_IO_VSEL, val);
	if (ret < 0)
		dev_warn(iod->dev, "couldn't update vccio6 ctrl\n");
}

static void rk3288_iodomain_init(struct rockchip_iodomain *iod)
{
	int ret;
	u32 val;

	/* if no flash supply we should leave things alone */
	if (!iod->supplies[RK3288_SOC_FLASH_SUPPLY_NUM].reg)
		return;

	/*
	 * set flash0 iodomain to also use this framework
	 * instead of a special gpio.
	 */
	val = RK3288_SOC_CON2_FLASH0 | (RK3288_SOC_CON2_FLASH0 << 16);
	ret = regmap_write(iod->grf, RK3288_SOC_CON2, val);
	if (ret < 0)
		dev_warn(iod->dev, "couldn't update flash0 ctrl\n");
}

static void rk3328_iodomain_init(struct rockchip_iodomain *iod)
{
	int ret;
	u32 val;

	/* if no vccio2 supply we should leave things alone */
	if (!iod->supplies[RK3328_SOC_VCCIO2_SUPPLY_NUM].reg)
		return;

	/*
	 * set vccio2 iodomain to also use this framework
	 * instead of a special gpio.
	 */
	val = RK3328_SOC_CON4_VCCIO2 | (RK3328_SOC_CON4_VCCIO2 << 16);
	ret = regmap_write(iod->grf, RK3328_SOC_CON4, val);
	if (ret < 0)
		dev_warn(iod->dev, "couldn't update vccio2 vsel ctrl\n");
}

static void rk3368_iodomain_init(struct rockchip_iodomain *iod)
{
	int ret;
	u32 val;

	/* if no flash supply we should leave things alone */
	if (!iod->supplies[RK3368_SOC_FLASH_SUPPLY_NUM].reg)
		return;

	/*
	 * set flash0 iodomain to also use this framework
	 * instead of a special gpio.
	 */
	val = RK3368_SOC_CON15_FLASH0 | (RK3368_SOC_CON15_FLASH0 << 16);
	ret = regmap_write(iod->grf, RK3368_SOC_CON15, val);
	if (ret < 0)
		dev_warn(iod->dev, "couldn't update flash0 ctrl\n");
}

static void rk3399_pmu_iodomain_init(struct rockchip_iodomain *iod)
{
	int ret;
	u32 val;

	/* if no pmu io supply we should leave things alone */
	if (!iod->supplies[RK3399_PMUGRF_VSEL_SUPPLY_NUM].reg)
		return;

	/*
	 * set pmu io iodomain to also use this framework
	 * instead of a special gpio.
	 */
	val = RK3399_PMUGRF_CON0_VSEL | (RK3399_PMUGRF_CON0_VSEL << 16);
	ret = regmap_write(iod->grf, RK3399_PMUGRF_CON0, val);
	if (ret < 0)
		dev_warn(iod->dev, "couldn't update pmu io iodomain ctrl\n");
}

static const struct rockchip_iodomain_soc_data soc_data_px30 = {
	.grf_offset = 0x180,
	.supply_names = {
		NULL,
		"vccio6",
		"vccio1",
		"vccio2",
		"vccio3",
		"vccio4",
		"vccio5",
		"vccio-oscgpi",
	},
	.init = px30_iodomain_init,
};

static const struct rockchip_iodomain_soc_data soc_data_px30_pmu = {
	.grf_offset = 0x100,
	.supply_names = {
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		"pmuio1",
		"pmuio2",
	},
};

/*
 * On the rk3188 the io-domains are handled by a shared register with the
 * lower 8 bits being still being continuing drive-strength settings.
 */
static const struct rockchip_iodomain_soc_data soc_data_rk3188 = {
	.grf_offset = 0x104,
	.supply_names = {
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		"ap0",
		"ap1",
		"cif",
		"flash",
		"vccio0",
		"vccio1",
		"lcdc0",
		"lcdc1",
	},
};

static const struct rockchip_iodomain_soc_data soc_data_rk3228 = {
	.grf_offset = 0x418,
	.supply_names = {
		"vccio1",
		"vccio2",
		"vccio3",
		"vccio4",
	},
};

static const struct rockchip_iodomain_soc_data soc_data_rk3288 = {
	.grf_offset = 0x380,
	.supply_names = {
		"lcdc",		/* LCDC_VDD */
		"dvp",		/* DVPIO_VDD */
		"flash0",	/* FLASH0_VDD (emmc) */
		"flash1",	/* FLASH1_VDD (sdio1) */
		"wifi",		/* APIO3_VDD  (sdio0) */
		"bb",		/* APIO5_VDD */
		"audio",	/* APIO4_VDD */
		"sdcard",	/* SDMMC0_VDD (sdmmc) */
		"gpio30",	/* APIO1_VDD */
		"gpio1830",	/* APIO2_VDD */
	},
	.init = rk3288_iodomain_init,
};

static const struct rockchip_iodomain_soc_data soc_data_rk3328 = {
	.grf_offset = 0x410,
	.supply_names = {
		"vccio1",
		"vccio2",
		"vccio3",
		"vccio4",
		"vccio5",
		"vccio6",
		"pmuio",
	},
	.init = rk3328_iodomain_init,
};

static const struct rockchip_iodomain_soc_data soc_data_rk3368 = {
	.grf_offset = 0x900,
	.supply_names = {
		NULL,		/* reserved */
		"dvp",		/* DVPIO_VDD */
		"flash0",	/* FLASH0_VDD (emmc) */
		"wifi",		/* APIO2_VDD (sdio0) */
		NULL,
		"audio",	/* APIO3_VDD */
		"sdcard",	/* SDMMC0_VDD (sdmmc) */
		"gpio30",	/* APIO1_VDD */
		"gpio1830",	/* APIO4_VDD (gpujtag) */
	},
	.init = rk3368_iodomain_init,
};

static const struct rockchip_iodomain_soc_data soc_data_rk3368_pmu = {
	.grf_offset = 0x100,
	.supply_names = {
		NULL,
		NULL,
		NULL,
		NULL,
		"pmu",	        /*PMU IO domain*/
		"vop",	        /*LCDC IO domain*/
	},
};

static const struct rockchip_iodomain_soc_data soc_data_rk3399 = {
	.grf_offset = 0xe640,
	.supply_names = {
		"bt656",		/* APIO2_VDD */
		"audio",		/* APIO5_VDD */
		"sdmmc",		/* SDMMC0_VDD */
		"gpio1830",		/* APIO4_VDD */
	},
};

static const struct rockchip_iodomain_soc_data soc_data_rk3399_pmu = {
	.grf_offset = 0x180,
	.supply_names = {
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		"pmu1830",		/* PMUIO2_VDD */
	},
	.init = rk3399_pmu_iodomain_init,
};

static const struct rockchip_iodomain_soc_data soc_data_rv1108 = {
	.grf_offset = 0x404,
	.supply_names = {
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		"vccio1",
		"vccio2",
		"vccio3",
		"vccio5",
		"vccio6",
	},

};

static const struct rockchip_iodomain_soc_data soc_data_rv1108_pmu = {
	.grf_offset = 0x104,
	.supply_names = {
		"pmu",
	},
};

static const struct of_device_id rockchip_iodomain_match[] = {
	{
		.compatible = "rockchip,px30-io-voltage-domain",
		.data = (void *)&soc_data_px30
	},
	{
		.compatible = "rockchip,px30-pmu-io-voltage-domain",
		.data = (void *)&soc_data_px30_pmu
	},
	{
		.compatible = "rockchip,rk3188-io-voltage-domain",
		.data = &soc_data_rk3188
	},
	{
		.compatible = "rockchip,rk3228-io-voltage-domain",
		.data = &soc_data_rk3228
	},
	{
		.compatible = "rockchip,rk3288-io-voltage-domain",
		.data = &soc_data_rk3288
	},
	{
		.compatible = "rockchip,rk3328-io-voltage-domain",
		.data = &soc_data_rk3328
	},
	{
		.compatible = "rockchip,rk3368-io-voltage-domain",
		.data = &soc_data_rk3368
	},
	{
		.compatible = "rockchip,rk3368-pmu-io-voltage-domain",
		.data = &soc_data_rk3368_pmu
	},
	{
		.compatible = "rockchip,rk3399-io-voltage-domain",
		.data = &soc_data_rk3399
	},
	{
		.compatible = "rockchip,rk3399-pmu-io-voltage-domain",
		.data = &soc_data_rk3399_pmu
	},
	{
		.compatible = "rockchip,rv1108-io-voltage-domain",
		.data = &soc_data_rv1108
	},
	{
		.compatible = "rockchip,rv1108-pmu-io-voltage-domain",
		.data = &soc_data_rv1108_pmu
	},
	{ /* sentinel */ },
};
MODULE_DEVICE_TABLE(of, rockchip_iodomain_match);

static int rockchip_iodomain_probe(struct platform_device *pdev)
{
	struct device_node *np = pdev->dev.of_node;
	const struct of_device_id *match;
	struct rockchip_iodomain *iod;
	struct device *parent;
	int i, ret = 0;

	if (!np)
		return -ENODEV;

	iod = devm_kzalloc(&pdev->dev, sizeof(*iod), GFP_KERNEL);
	if (!iod)
		return -ENOMEM;

	iod->dev = &pdev->dev;
	platform_set_drvdata(pdev, iod);

	match = of_match_node(rockchip_iodomain_match, np);
	iod->soc_data = match->data;

	parent = pdev->dev.parent;
	if (parent && parent->of_node) {
		iod->grf = syscon_node_to_regmap(parent->of_node);
	} else {
		dev_dbg(&pdev->dev, "falling back to old binding\n");
		iod->grf = syscon_regmap_lookup_by_phandle(np, "rockchip,grf");
	}

	if (IS_ERR(iod->grf)) {
		dev_err(&pdev->dev, "couldn't find grf regmap\n");
		return PTR_ERR(iod->grf);
	}

	for (i = 0; i < MAX_SUPPLIES; i++) {
		const char *supply_name = iod->soc_data->supply_names[i];
		struct rockchip_iodomain_supply *supply = &iod->supplies[i];
		struct regulator *reg;
		int uV;

		if (!supply_name)
			continue;

		reg = devm_regulator_get_optional(iod->dev, supply_name);
		if (IS_ERR(reg)) {
			ret = PTR_ERR(reg);

			/* If a supply wasn't specified, that's OK */
			if (ret == -ENODEV)
				continue;
			else if (ret != -EPROBE_DEFER)
				dev_err(iod->dev, "couldn't get regulator %s\n",
					supply_name);
			goto unreg_notify;
		}

		/* set initial correct value */
		uV = regulator_get_voltage(reg);

		/* must be a regulator we can get the voltage of */
		if (uV < 0) {
			dev_err(iod->dev, "Can't determine voltage: %s\n",
				supply_name);
			ret = uV;
			goto unreg_notify;
		}

		if (uV > MAX_VOLTAGE_3_3) {
			dev_crit(iod->dev,
				 "%d uV is too high. May damage SoC!\n",
				 uV);
			ret = -EINVAL;
			goto unreg_notify;
		}

		/* setup our supply */
		supply->idx = i;
		supply->iod = iod;
		supply->reg = reg;
		supply->nb.notifier_call = rockchip_iodomain_notify;

		ret = rockchip_iodomain_write(supply, uV);
		if (ret) {
			supply->reg = NULL;
			goto unreg_notify;
		}

		/* register regulator notifier */
		ret = regulator_register_notifier(reg, &supply->nb);
		if (ret) {
			dev_err(&pdev->dev,
				"regulator notifier request failed\n");
			supply->reg = NULL;
			goto unreg_notify;
		}
	}

	if (iod->soc_data->init)
		iod->soc_data->init(iod);

	return 0;

unreg_notify:
	for (i = MAX_SUPPLIES - 1; i >= 0; i--) {
		struct rockchip_iodomain_supply *io_supply = &iod->supplies[i];

		if (io_supply->reg)
			regulator_unregister_notifier(io_supply->reg,
						      &io_supply->nb);
	}

	return ret;
}

static int rockchip_iodomain_remove(struct platform_device *pdev)
{
	struct rockchip_iodomain *iod = platform_get_drvdata(pdev);
	int i;

	for (i = MAX_SUPPLIES - 1; i >= 0; i--) {
		struct rockchip_iodomain_supply *io_supply = &iod->supplies[i];

		if (io_supply->reg)
			regulator_unregister_notifier(io_supply->reg,
						      &io_supply->nb);
	}

	return 0;
}

static struct platform_driver rockchip_iodomain_driver = {
	.probe   = rockchip_iodomain_probe,
	.remove  = rockchip_iodomain_remove,
	.driver  = {
		.name  = "rockchip-iodomain",
		.of_match_table = rockchip_iodomain_match,
	},
};

module_platform_driver(rockchip_iodomain_driver);

MODULE_DESCRIPTION("Rockchip IO-domain driver");
MODULE_AUTHOR("Heiko Stuebner <heiko@sntech.de>");
MODULE_AUTHOR("Doug Anderson <dianders@chromium.org>");
MODULE_LICENSE("GPL v2");
