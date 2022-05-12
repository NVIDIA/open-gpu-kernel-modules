// SPDX-License-Identifier: GPL-2.0
//
// Regulator Driver for Freescale MC13783 PMIC
//
// Copyright 2010 Yong Shen <yong.shen@linaro.org>
// Copyright (C) 2008 Sascha Hauer, Pengutronix <s.hauer@pengutronix.de>
// Copyright 2009 Alberto Panizzo <maramaopercheseimorto@gmail.com>

#include <linux/mfd/mc13783.h>
#include <linux/regulator/machine.h>
#include <linux/regulator/driver.h>
#include <linux/platform_device.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/err.h>
#include <linux/module.h>
#include "mc13xxx.h"

#define MC13783_REG_SWITCHERS0			24
/* Enable does not exist for SW1A */
#define MC13783_REG_SWITCHERS0_SW1AEN			0
#define MC13783_REG_SWITCHERS0_SW1AVSEL			0
#define MC13783_REG_SWITCHERS0_SW1AVSEL_M		(63 << 0)

#define MC13783_REG_SWITCHERS1			25
/* Enable does not exist for SW1B */
#define MC13783_REG_SWITCHERS1_SW1BEN			0
#define MC13783_REG_SWITCHERS1_SW1BVSEL			0
#define MC13783_REG_SWITCHERS1_SW1BVSEL_M		(63 << 0)

#define MC13783_REG_SWITCHERS2			26
/* Enable does not exist for SW2A */
#define MC13783_REG_SWITCHERS2_SW2AEN			0
#define MC13783_REG_SWITCHERS2_SW2AVSEL			0
#define MC13783_REG_SWITCHERS2_SW2AVSEL_M		(63 << 0)

#define MC13783_REG_SWITCHERS3			27
/* Enable does not exist for SW2B */
#define MC13783_REG_SWITCHERS3_SW2BEN			0
#define MC13783_REG_SWITCHERS3_SW2BVSEL			0
#define MC13783_REG_SWITCHERS3_SW2BVSEL_M		(63 << 0)

#define MC13783_REG_SWITCHERS5			29
#define MC13783_REG_SWITCHERS5_SW3EN			(1 << 20)
#define MC13783_REG_SWITCHERS5_SW3VSEL			18
#define MC13783_REG_SWITCHERS5_SW3VSEL_M		(3 << 18)

#define MC13783_REG_REGULATORSETTING0		30
#define MC13783_REG_REGULATORSETTING0_VIOLOVSEL		2
#define MC13783_REG_REGULATORSETTING0_VDIGVSEL		4
#define MC13783_REG_REGULATORSETTING0_VGENVSEL		6
#define MC13783_REG_REGULATORSETTING0_VRFDIGVSEL	9
#define MC13783_REG_REGULATORSETTING0_VRFREFVSEL	11
#define MC13783_REG_REGULATORSETTING0_VRFCPVSEL		13
#define MC13783_REG_REGULATORSETTING0_VSIMVSEL		14
#define MC13783_REG_REGULATORSETTING0_VESIMVSEL		15
#define MC13783_REG_REGULATORSETTING0_VCAMVSEL		16

#define MC13783_REG_REGULATORSETTING0_VIOLOVSEL_M	(3 << 2)
#define MC13783_REG_REGULATORSETTING0_VDIGVSEL_M	(3 << 4)
#define MC13783_REG_REGULATORSETTING0_VGENVSEL_M	(7 << 6)
#define MC13783_REG_REGULATORSETTING0_VRFDIGVSEL_M	(3 << 9)
#define MC13783_REG_REGULATORSETTING0_VRFREFVSEL_M	(3 << 11)
#define MC13783_REG_REGULATORSETTING0_VRFCPVSEL_M	(1 << 13)
#define MC13783_REG_REGULATORSETTING0_VSIMVSEL_M	(1 << 14)
#define MC13783_REG_REGULATORSETTING0_VESIMVSEL_M	(1 << 15)
#define MC13783_REG_REGULATORSETTING0_VCAMVSEL_M	(7 << 16)

#define MC13783_REG_REGULATORSETTING1		31
#define MC13783_REG_REGULATORSETTING1_VVIBVSEL		0
#define MC13783_REG_REGULATORSETTING1_VRF1VSEL		2
#define MC13783_REG_REGULATORSETTING1_VRF2VSEL		4
#define MC13783_REG_REGULATORSETTING1_VMMC1VSEL		6
#define MC13783_REG_REGULATORSETTING1_VMMC2VSEL		9

#define MC13783_REG_REGULATORSETTING1_VVIBVSEL_M	(3 << 0)
#define MC13783_REG_REGULATORSETTING1_VRF1VSEL_M	(3 << 2)
#define MC13783_REG_REGULATORSETTING1_VRF2VSEL_M	(3 << 4)
#define MC13783_REG_REGULATORSETTING1_VMMC1VSEL_M	(7 << 6)
#define MC13783_REG_REGULATORSETTING1_VMMC2VSEL_M	(7 << 9)

#define MC13783_REG_REGULATORMODE0		32
#define MC13783_REG_REGULATORMODE0_VAUDIOEN		(1 << 0)
#define MC13783_REG_REGULATORMODE0_VIOHIEN		(1 << 3)
#define MC13783_REG_REGULATORMODE0_VIOLOEN		(1 << 6)
#define MC13783_REG_REGULATORMODE0_VDIGEN		(1 << 9)
#define MC13783_REG_REGULATORMODE0_VGENEN		(1 << 12)
#define MC13783_REG_REGULATORMODE0_VRFDIGEN		(1 << 15)
#define MC13783_REG_REGULATORMODE0_VRFREFEN		(1 << 18)
#define MC13783_REG_REGULATORMODE0_VRFCPEN		(1 << 21)

#define MC13783_REG_REGULATORMODE1		33
#define MC13783_REG_REGULATORMODE1_VSIMEN		(1 << 0)
#define MC13783_REG_REGULATORMODE1_VESIMEN		(1 << 3)
#define MC13783_REG_REGULATORMODE1_VCAMEN		(1 << 6)
#define MC13783_REG_REGULATORMODE1_VRFBGEN		(1 << 9)
#define MC13783_REG_REGULATORMODE1_VVIBEN		(1 << 11)
#define MC13783_REG_REGULATORMODE1_VRF1EN		(1 << 12)
#define MC13783_REG_REGULATORMODE1_VRF2EN		(1 << 15)
#define MC13783_REG_REGULATORMODE1_VMMC1EN		(1 << 18)
#define MC13783_REG_REGULATORMODE1_VMMC2EN		(1 << 21)

#define MC13783_REG_POWERMISC			34
#define MC13783_REG_POWERMISC_GPO1EN			(1 << 6)
#define MC13783_REG_POWERMISC_GPO2EN			(1 << 8)
#define MC13783_REG_POWERMISC_GPO3EN			(1 << 10)
#define MC13783_REG_POWERMISC_GPO4EN			(1 << 12)
#define MC13783_REG_POWERMISC_PWGT1SPIEN		(1 << 15)
#define MC13783_REG_POWERMISC_PWGT2SPIEN		(1 << 16)

#define MC13783_REG_POWERMISC_PWGTSPI_M			(3 << 15)


/* Voltage Values */
static const int mc13783_sw1x_val[] = {
	900000, 925000, 950000, 975000,
	1000000, 1025000, 1050000, 1075000,
	1100000, 1125000, 1150000, 1175000,
	1200000, 1225000, 1250000, 1275000,
	1300000, 1325000, 1350000, 1375000,
	1400000, 1425000, 1450000, 1475000,
	1500000, 1525000, 1550000, 1575000,
	1600000, 1625000, 1650000, 1675000,
	1700000, 1700000, 1700000, 1700000,
	1800000, 1800000, 1800000, 1800000,
	1850000, 1850000, 1850000, 1850000,
	2000000, 2000000, 2000000, 2000000,
	2100000, 2100000, 2100000, 2100000,
	2200000, 2200000, 2200000, 2200000,
	2200000, 2200000, 2200000, 2200000,
	2200000, 2200000, 2200000, 2200000,
};

static const int mc13783_sw2x_val[] = {
	900000, 925000, 950000, 975000,
	1000000, 1025000, 1050000, 1075000,
	1100000, 1125000, 1150000, 1175000,
	1200000, 1225000, 1250000, 1275000,
	1300000, 1325000, 1350000, 1375000,
	1400000, 1425000, 1450000, 1475000,
	1500000, 1525000, 1550000, 1575000,
	1600000, 1625000, 1650000, 1675000,
	1700000, 1700000, 1700000, 1700000,
	1800000, 1800000, 1800000, 1800000,
	1900000, 1900000, 1900000, 1900000,
	2000000, 2000000, 2000000, 2000000,
	2100000, 2100000, 2100000, 2100000,
	2200000, 2200000, 2200000, 2200000,
	2200000, 2200000, 2200000, 2200000,
	2200000, 2200000, 2200000, 2200000,
};

static const unsigned int mc13783_sw3_val[] = {
	5000000, 5000000, 5000000, 5500000,
};

static const unsigned int mc13783_vaudio_val[] = {
	2775000,
};

static const unsigned int mc13783_viohi_val[] = {
	2775000,
};

static const unsigned int mc13783_violo_val[] = {
	1200000, 1300000, 1500000, 1800000,
};

static const unsigned int mc13783_vdig_val[] = {
	1200000, 1300000, 1500000, 1800000,
};

static const unsigned int mc13783_vgen_val[] = {
	1200000, 1300000, 1500000, 1800000,
	1100000, 2000000, 2775000, 2400000,
};

static const unsigned int mc13783_vrfdig_val[] = {
	1200000, 1500000, 1800000, 1875000,
};

static const unsigned int mc13783_vrfref_val[] = {
	2475000, 2600000, 2700000, 2775000,
};

static const unsigned int mc13783_vrfcp_val[] = {
	2700000, 2775000,
};

static const unsigned int mc13783_vsim_val[] = {
	1800000, 2900000, 3000000,
};

static const unsigned int mc13783_vesim_val[] = {
	1800000, 2900000,
};

static const unsigned int mc13783_vcam_val[] = {
	1500000, 1800000, 2500000, 2550000,
	2600000, 2750000, 2800000, 3000000,
};

static const unsigned int mc13783_vrfbg_val[] = {
	1250000,
};

static const unsigned int mc13783_vvib_val[] = {
	1300000, 1800000, 2000000, 3000000,
};

static const unsigned int mc13783_vmmc_val[] = {
	1600000, 1800000, 2000000, 2600000,
	2700000, 2800000, 2900000, 3000000,
};

static const unsigned int mc13783_vrf_val[] = {
	1500000, 1875000, 2700000, 2775000,
};

static const unsigned int mc13783_gpo_val[] = {
	3100000,
};

static const unsigned int mc13783_pwgtdrv_val[] = {
	5500000,
};

static const struct regulator_ops mc13783_gpo_regulator_ops;

#define MC13783_DEFINE(prefix, name, node, reg, vsel_reg, voltages)	\
	MC13xxx_DEFINE(MC13783_REG_, name, node, reg, vsel_reg, voltages, \
			mc13xxx_regulator_ops)

#define MC13783_FIXED_DEFINE(prefix, name, node, reg, voltages)		\
	MC13xxx_FIXED_DEFINE(MC13783_REG_, name, node, reg, voltages,	\
			mc13xxx_fixed_regulator_ops)

#define MC13783_GPO_DEFINE(prefix, name, node, reg, voltages)		\
	MC13xxx_GPO_DEFINE(MC13783_REG_, name, node, reg, voltages,	\
			mc13783_gpo_regulator_ops)

#define MC13783_DEFINE_SW(_name, _node, _reg, _vsel_reg, _voltages)	\
	MC13783_DEFINE(REG, _name, _node, _reg, _vsel_reg, _voltages)
#define MC13783_DEFINE_REGU(_name, _node, _reg, _vsel_reg, _voltages)	\
	MC13783_DEFINE(REG, _name, _node, _reg, _vsel_reg, _voltages)

static struct mc13xxx_regulator mc13783_regulators[] = {
	MC13783_DEFINE_SW(SW1A, sw1a, SWITCHERS0, SWITCHERS0, mc13783_sw1x_val),
	MC13783_DEFINE_SW(SW1B, sw1b, SWITCHERS1, SWITCHERS1, mc13783_sw1x_val),
	MC13783_DEFINE_SW(SW2A, sw2a, SWITCHERS2, SWITCHERS2, mc13783_sw2x_val),
	MC13783_DEFINE_SW(SW2B, sw2b, SWITCHERS3, SWITCHERS3, mc13783_sw2x_val),
	MC13783_DEFINE_SW(SW3, sw3, SWITCHERS5, SWITCHERS5, mc13783_sw3_val),

	MC13783_FIXED_DEFINE(REG, VAUDIO, vaudio, REGULATORMODE0, mc13783_vaudio_val),
	MC13783_FIXED_DEFINE(REG, VIOHI, viohi, REGULATORMODE0, mc13783_viohi_val),
	MC13783_DEFINE_REGU(VIOLO, violo, REGULATORMODE0, REGULATORSETTING0,
			    mc13783_violo_val),
	MC13783_DEFINE_REGU(VDIG, vdig, REGULATORMODE0, REGULATORSETTING0,
			    mc13783_vdig_val),
	MC13783_DEFINE_REGU(VGEN, vgen, REGULATORMODE0, REGULATORSETTING0,
			    mc13783_vgen_val),
	MC13783_DEFINE_REGU(VRFDIG, vrfdig, REGULATORMODE0, REGULATORSETTING0,
			    mc13783_vrfdig_val),
	MC13783_DEFINE_REGU(VRFREF, vrfref, REGULATORMODE0, REGULATORSETTING0,
			    mc13783_vrfref_val),
	MC13783_DEFINE_REGU(VRFCP, vrfcp, REGULATORMODE0, REGULATORSETTING0,
			    mc13783_vrfcp_val),
	MC13783_DEFINE_REGU(VSIM, vsim, REGULATORMODE1, REGULATORSETTING0,
			    mc13783_vsim_val),
	MC13783_DEFINE_REGU(VESIM, vesim, REGULATORMODE1, REGULATORSETTING0,
			    mc13783_vesim_val),
	MC13783_DEFINE_REGU(VCAM, vcam, REGULATORMODE1, REGULATORSETTING0,
			    mc13783_vcam_val),
	MC13783_FIXED_DEFINE(REG, VRFBG, vrfbg, REGULATORMODE1, mc13783_vrfbg_val),
	MC13783_DEFINE_REGU(VVIB, vvib, REGULATORMODE1, REGULATORSETTING1,
			    mc13783_vvib_val),
	MC13783_DEFINE_REGU(VRF1, vrf1, REGULATORMODE1, REGULATORSETTING1,
			    mc13783_vrf_val),
	MC13783_DEFINE_REGU(VRF2, vrf2, REGULATORMODE1, REGULATORSETTING1,
			    mc13783_vrf_val),
	MC13783_DEFINE_REGU(VMMC1, vmmc1, REGULATORMODE1, REGULATORSETTING1,
			    mc13783_vmmc_val),
	MC13783_DEFINE_REGU(VMMC2, vmmc2, REGULATORMODE1, REGULATORSETTING1,
			    mc13783_vmmc_val),
	MC13783_GPO_DEFINE(REG, GPO1, gpo1, POWERMISC, mc13783_gpo_val),
	MC13783_GPO_DEFINE(REG, GPO2, gpo1, POWERMISC, mc13783_gpo_val),
	MC13783_GPO_DEFINE(REG, GPO3, gpo1, POWERMISC, mc13783_gpo_val),
	MC13783_GPO_DEFINE(REG, GPO4, gpo1, POWERMISC, mc13783_gpo_val),
	MC13783_GPO_DEFINE(REG, PWGT1SPI, pwgt1spi, POWERMISC, mc13783_pwgtdrv_val),
	MC13783_GPO_DEFINE(REG, PWGT2SPI, pwgt2spi, POWERMISC, mc13783_pwgtdrv_val),
};

static int mc13783_powermisc_rmw(struct mc13xxx_regulator_priv *priv, u32 mask,
		u32 val)
{
	struct mc13xxx *mc13783 = priv->mc13xxx;
	int ret;
	u32 valread;

	BUG_ON(val & ~mask);

	mc13xxx_lock(priv->mc13xxx);
	ret = mc13xxx_reg_read(mc13783, MC13783_REG_POWERMISC, &valread);
	if (ret)
		goto out;

	/* Update the stored state for Power Gates. */
	priv->powermisc_pwgt_state =
				(priv->powermisc_pwgt_state & ~mask) | val;
	priv->powermisc_pwgt_state &= MC13783_REG_POWERMISC_PWGTSPI_M;

	/* Construct the new register value */
	valread = (valread & ~mask) | val;
	/* Overwrite the PWGTxEN with the stored version */
	valread = (valread & ~MC13783_REG_POWERMISC_PWGTSPI_M) |
						priv->powermisc_pwgt_state;

	ret = mc13xxx_reg_write(mc13783, MC13783_REG_POWERMISC, valread);
out:
	mc13xxx_unlock(priv->mc13xxx);
	return ret;
}

static int mc13783_gpo_regulator_enable(struct regulator_dev *rdev)
{
	struct mc13xxx_regulator_priv *priv = rdev_get_drvdata(rdev);
	struct mc13xxx_regulator *mc13xxx_regulators = priv->mc13xxx_regulators;
	int id = rdev_get_id(rdev);
	u32 en_val = mc13xxx_regulators[id].enable_bit;

	dev_dbg(rdev_get_dev(rdev), "%s id: %d\n", __func__, id);

	/* Power Gate enable value is 0 */
	if (id == MC13783_REG_PWGT1SPI ||
	    id == MC13783_REG_PWGT2SPI)
		en_val = 0;

	return mc13783_powermisc_rmw(priv, mc13xxx_regulators[id].enable_bit,
					en_val);
}

static int mc13783_gpo_regulator_disable(struct regulator_dev *rdev)
{
	struct mc13xxx_regulator_priv *priv = rdev_get_drvdata(rdev);
	struct mc13xxx_regulator *mc13xxx_regulators = priv->mc13xxx_regulators;
	int id = rdev_get_id(rdev);
	u32 dis_val = 0;

	dev_dbg(rdev_get_dev(rdev), "%s id: %d\n", __func__, id);

	/* Power Gate disable value is 1 */
	if (id == MC13783_REG_PWGT1SPI ||
	    id == MC13783_REG_PWGT2SPI)
		dis_val = mc13xxx_regulators[id].enable_bit;

	return mc13783_powermisc_rmw(priv, mc13xxx_regulators[id].enable_bit,
					dis_val);
}

static int mc13783_gpo_regulator_is_enabled(struct regulator_dev *rdev)
{
	struct mc13xxx_regulator_priv *priv = rdev_get_drvdata(rdev);
	struct mc13xxx_regulator *mc13xxx_regulators = priv->mc13xxx_regulators;
	int ret, id = rdev_get_id(rdev);
	unsigned int val;

	mc13xxx_lock(priv->mc13xxx);
	ret = mc13xxx_reg_read(priv->mc13xxx, mc13xxx_regulators[id].reg, &val);
	mc13xxx_unlock(priv->mc13xxx);

	if (ret)
		return ret;

	/* Power Gates state is stored in powermisc_pwgt_state
	 * where the meaning of bits is negated */
	val = (val & ~MC13783_REG_POWERMISC_PWGTSPI_M) |
	      (priv->powermisc_pwgt_state ^ MC13783_REG_POWERMISC_PWGTSPI_M);

	return (val & mc13xxx_regulators[id].enable_bit) != 0;
}

static const struct regulator_ops mc13783_gpo_regulator_ops = {
	.enable = mc13783_gpo_regulator_enable,
	.disable = mc13783_gpo_regulator_disable,
	.is_enabled = mc13783_gpo_regulator_is_enabled,
	.list_voltage = regulator_list_voltage_table,
	.set_voltage = mc13xxx_fixed_regulator_set_voltage,
};

static int mc13783_regulator_probe(struct platform_device *pdev)
{
	struct mc13xxx_regulator_priv *priv;
	struct mc13xxx *mc13783 = dev_get_drvdata(pdev->dev.parent);
	struct mc13xxx_regulator_platform_data *pdata =
		dev_get_platdata(&pdev->dev);
	struct mc13xxx_regulator_init_data *mc13xxx_data;
	struct regulator_config config = { };
	int i, num_regulators;

	num_regulators = mc13xxx_get_num_regulators_dt(pdev);

	if (num_regulators <= 0 && pdata)
		num_regulators = pdata->num_regulators;
	if (num_regulators <= 0)
		return -EINVAL;

	priv = devm_kzalloc(&pdev->dev,
			    struct_size(priv, regulators, num_regulators),
			    GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	priv->num_regulators = num_regulators;
	priv->mc13xxx_regulators = mc13783_regulators;
	priv->mc13xxx = mc13783;
	platform_set_drvdata(pdev, priv);

	mc13xxx_data = mc13xxx_parse_regulators_dt(pdev, mc13783_regulators,
					ARRAY_SIZE(mc13783_regulators));

	for (i = 0; i < priv->num_regulators; i++) {
		struct regulator_init_data *init_data;
		struct regulator_desc *desc;
		struct device_node *node = NULL;
		int id;

		if (mc13xxx_data) {
			id = mc13xxx_data[i].id;
			init_data = mc13xxx_data[i].init_data;
			node = mc13xxx_data[i].node;
		} else {
			id = pdata->regulators[i].id;
			init_data = pdata->regulators[i].init_data;
		}
		desc = &mc13783_regulators[id].desc;

		config.dev = &pdev->dev;
		config.init_data = init_data;
		config.driver_data = priv;
		config.of_node = node;

		priv->regulators[i] = devm_regulator_register(&pdev->dev, desc,
							      &config);
		if (IS_ERR(priv->regulators[i])) {
			dev_err(&pdev->dev, "failed to register regulator %s\n",
				mc13783_regulators[i].desc.name);
			return PTR_ERR(priv->regulators[i]);
		}
	}

	return 0;
}

static struct platform_driver mc13783_regulator_driver = {
	.driver	= {
		.name	= "mc13783-regulator",
	},
	.probe		= mc13783_regulator_probe,
};

static int __init mc13783_regulator_init(void)
{
	return platform_driver_register(&mc13783_regulator_driver);
}
subsys_initcall(mc13783_regulator_init);

static void __exit mc13783_regulator_exit(void)
{
	platform_driver_unregister(&mc13783_regulator_driver);
}
module_exit(mc13783_regulator_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Sascha Hauer <s.hauer@pengutronix.de>");
MODULE_DESCRIPTION("Regulator Driver for Freescale MC13783 PMIC");
MODULE_ALIAS("platform:mc13783-regulator");
