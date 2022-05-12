// SPDX-License-Identifier: GPL-2.0-only
/*
 * drivers/pwm/pwm-vt8500.c
 *
 * Copyright (C) 2012 Tony Prisk <linux@prisktech.co.nz>
 * Copyright (C) 2010 Alexey Charkov <alchark@gmail.com>
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/pwm.h>
#include <linux/delay.h>
#include <linux/clk.h>

#include <asm/div64.h>

#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_address.h>

/*
 * SoC architecture allocates register space for 4 PWMs but only
 * 2 are currently implemented.
 */
#define VT8500_NR_PWMS	2

#define REG_CTRL(pwm)		(((pwm) << 4) + 0x00)
#define REG_SCALAR(pwm)		(((pwm) << 4) + 0x04)
#define REG_PERIOD(pwm)		(((pwm) << 4) + 0x08)
#define REG_DUTY(pwm)		(((pwm) << 4) + 0x0C)
#define REG_STATUS		0x40

#define CTRL_ENABLE		BIT(0)
#define CTRL_INVERT		BIT(1)
#define CTRL_AUTOLOAD		BIT(2)
#define CTRL_STOP_IMM		BIT(3)
#define CTRL_LOAD_PRESCALE	BIT(4)
#define CTRL_LOAD_PERIOD	BIT(5)

#define STATUS_CTRL_UPDATE	BIT(0)
#define STATUS_SCALAR_UPDATE	BIT(1)
#define STATUS_PERIOD_UPDATE	BIT(2)
#define STATUS_DUTY_UPDATE	BIT(3)
#define STATUS_ALL_UPDATE	0x0F

struct vt8500_chip {
	struct pwm_chip chip;
	void __iomem *base;
	struct clk *clk;
};

#define to_vt8500_chip(chip)	container_of(chip, struct vt8500_chip, chip)

#define msecs_to_loops(t) (loops_per_jiffy / 1000 * HZ * t)
static inline void pwm_busy_wait(struct vt8500_chip *vt8500, int nr, u8 bitmask)
{
	int loops = msecs_to_loops(10);
	u32 mask = bitmask << (nr << 8);

	while ((readl(vt8500->base + REG_STATUS) & mask) && --loops)
		cpu_relax();

	if (unlikely(!loops))
		dev_warn(vt8500->chip.dev, "Waiting for status bits 0x%x to clear timed out\n",
			 mask);
}

static int vt8500_pwm_config(struct pwm_chip *chip, struct pwm_device *pwm,
		int duty_ns, int period_ns)
{
	struct vt8500_chip *vt8500 = to_vt8500_chip(chip);
	unsigned long long c;
	unsigned long period_cycles, prescale, pv, dc;
	int err;
	u32 val;

	err = clk_enable(vt8500->clk);
	if (err < 0) {
		dev_err(chip->dev, "failed to enable clock\n");
		return err;
	}

	c = clk_get_rate(vt8500->clk);
	c = c * period_ns;
	do_div(c, 1000000000);
	period_cycles = c;

	if (period_cycles < 1)
		period_cycles = 1;
	prescale = (period_cycles - 1) / 4096;
	pv = period_cycles / (prescale + 1) - 1;
	if (pv > 4095)
		pv = 4095;

	if (prescale > 1023) {
		clk_disable(vt8500->clk);
		return -EINVAL;
	}

	c = (unsigned long long)pv * duty_ns;
	do_div(c, period_ns);
	dc = c;

	writel(prescale, vt8500->base + REG_SCALAR(pwm->hwpwm));
	pwm_busy_wait(vt8500, pwm->hwpwm, STATUS_SCALAR_UPDATE);

	writel(pv, vt8500->base + REG_PERIOD(pwm->hwpwm));
	pwm_busy_wait(vt8500, pwm->hwpwm, STATUS_PERIOD_UPDATE);

	writel(dc, vt8500->base + REG_DUTY(pwm->hwpwm));
	pwm_busy_wait(vt8500, pwm->hwpwm, STATUS_DUTY_UPDATE);

	val = readl(vt8500->base + REG_CTRL(pwm->hwpwm));
	val |= CTRL_AUTOLOAD;
	writel(val, vt8500->base + REG_CTRL(pwm->hwpwm));
	pwm_busy_wait(vt8500, pwm->hwpwm, STATUS_CTRL_UPDATE);

	clk_disable(vt8500->clk);
	return 0;
}

static int vt8500_pwm_enable(struct pwm_chip *chip, struct pwm_device *pwm)
{
	struct vt8500_chip *vt8500 = to_vt8500_chip(chip);
	int err;
	u32 val;

	err = clk_enable(vt8500->clk);
	if (err < 0) {
		dev_err(chip->dev, "failed to enable clock\n");
		return err;
	}

	val = readl(vt8500->base + REG_CTRL(pwm->hwpwm));
	val |= CTRL_ENABLE;
	writel(val, vt8500->base + REG_CTRL(pwm->hwpwm));
	pwm_busy_wait(vt8500, pwm->hwpwm, STATUS_CTRL_UPDATE);

	return 0;
}

static void vt8500_pwm_disable(struct pwm_chip *chip, struct pwm_device *pwm)
{
	struct vt8500_chip *vt8500 = to_vt8500_chip(chip);
	u32 val;

	val = readl(vt8500->base + REG_CTRL(pwm->hwpwm));
	val &= ~CTRL_ENABLE;
	writel(val, vt8500->base + REG_CTRL(pwm->hwpwm));
	pwm_busy_wait(vt8500, pwm->hwpwm, STATUS_CTRL_UPDATE);

	clk_disable(vt8500->clk);
}

static int vt8500_pwm_set_polarity(struct pwm_chip *chip,
				   struct pwm_device *pwm,
				   enum pwm_polarity polarity)
{
	struct vt8500_chip *vt8500 = to_vt8500_chip(chip);
	u32 val;

	val = readl(vt8500->base + REG_CTRL(pwm->hwpwm));

	if (polarity == PWM_POLARITY_INVERSED)
		val |= CTRL_INVERT;
	else
		val &= ~CTRL_INVERT;

	writel(val, vt8500->base + REG_CTRL(pwm->hwpwm));
	pwm_busy_wait(vt8500, pwm->hwpwm, STATUS_CTRL_UPDATE);

	return 0;
}

static const struct pwm_ops vt8500_pwm_ops = {
	.enable = vt8500_pwm_enable,
	.disable = vt8500_pwm_disable,
	.config = vt8500_pwm_config,
	.set_polarity = vt8500_pwm_set_polarity,
	.owner = THIS_MODULE,
};

static const struct of_device_id vt8500_pwm_dt_ids[] = {
	{ .compatible = "via,vt8500-pwm", },
	{ /* Sentinel */ }
};
MODULE_DEVICE_TABLE(of, vt8500_pwm_dt_ids);

static int vt8500_pwm_probe(struct platform_device *pdev)
{
	struct vt8500_chip *chip;
	struct device_node *np = pdev->dev.of_node;
	int ret;

	if (!np) {
		dev_err(&pdev->dev, "invalid devicetree node\n");
		return -EINVAL;
	}

	chip = devm_kzalloc(&pdev->dev, sizeof(*chip), GFP_KERNEL);
	if (chip == NULL)
		return -ENOMEM;

	chip->chip.dev = &pdev->dev;
	chip->chip.ops = &vt8500_pwm_ops;
	chip->chip.of_xlate = of_pwm_xlate_with_flags;
	chip->chip.of_pwm_n_cells = 3;
	chip->chip.npwm = VT8500_NR_PWMS;

	chip->clk = devm_clk_get(&pdev->dev, NULL);
	if (IS_ERR(chip->clk)) {
		dev_err(&pdev->dev, "clock source not specified\n");
		return PTR_ERR(chip->clk);
	}

	chip->base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(chip->base))
		return PTR_ERR(chip->base);

	ret = clk_prepare(chip->clk);
	if (ret < 0) {
		dev_err(&pdev->dev, "failed to prepare clock\n");
		return ret;
	}

	ret = pwmchip_add(&chip->chip);
	if (ret < 0) {
		dev_err(&pdev->dev, "failed to add PWM chip\n");
		clk_unprepare(chip->clk);
		return ret;
	}

	platform_set_drvdata(pdev, chip);
	return ret;
}

static int vt8500_pwm_remove(struct platform_device *pdev)
{
	struct vt8500_chip *chip;

	chip = platform_get_drvdata(pdev);
	if (chip == NULL)
		return -ENODEV;

	clk_unprepare(chip->clk);

	return pwmchip_remove(&chip->chip);
}

static struct platform_driver vt8500_pwm_driver = {
	.probe		= vt8500_pwm_probe,
	.remove		= vt8500_pwm_remove,
	.driver		= {
		.name	= "vt8500-pwm",
		.of_match_table = vt8500_pwm_dt_ids,
	},
};
module_platform_driver(vt8500_pwm_driver);

MODULE_DESCRIPTION("VT8500 PWM Driver");
MODULE_AUTHOR("Tony Prisk <linux@prisktech.co.nz>");
MODULE_LICENSE("GPL v2");
