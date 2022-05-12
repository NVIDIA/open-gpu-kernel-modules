// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause
/*
 * PWM controller driver for Amlogic Meson SoCs.
 *
 * This PWM is only a set of Gates, Dividers and Counters:
 * PWM output is achieved by calculating a clock that permits calculating
 * two periods (low and high). The counter then has to be set to switch after
 * N cycles for the first half period.
 * The hardware has no "polarity" setting. This driver reverses the period
 * cycles (the low length is inverted with the high length) for
 * PWM_POLARITY_INVERSED. This means that .get_state cannot read the polarity
 * from the hardware.
 * Setting the duty cycle will disable and re-enable the PWM output.
 * Disabling the PWM stops the output immediately (without waiting for the
 * current period to complete first).
 *
 * The public S912 (GXM) datasheet contains some documentation for this PWM
 * controller starting on page 543:
 * https://dl.khadas.com/Hardware/VIM2/Datasheet/S912_Datasheet_V0.220170314publicversion-Wesion.pdf
 * An updated version of this IP block is found in S922X (G12B) SoCs. The
 * datasheet contains the description for this IP block revision starting at
 * page 1084:
 * https://dn.odroid.com/S922X/ODROID-N2/Datasheet/S922X_Public_Datasheet_V0.2.pdf
 *
 * Copyright (c) 2016 BayLibre, SAS.
 * Author: Neil Armstrong <narmstrong@baylibre.com>
 * Copyright (C) 2014 Amlogic, Inc.
 */

#include <linux/bitfield.h>
#include <linux/bits.h>
#include <linux/clk.h>
#include <linux/clk-provider.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/math64.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/pwm.h>
#include <linux/slab.h>
#include <linux/spinlock.h>

#define REG_PWM_A		0x0
#define REG_PWM_B		0x4
#define PWM_LOW_MASK		GENMASK(15, 0)
#define PWM_HIGH_MASK		GENMASK(31, 16)

#define REG_MISC_AB		0x8
#define MISC_B_CLK_EN		BIT(23)
#define MISC_A_CLK_EN		BIT(15)
#define MISC_CLK_DIV_MASK	0x7f
#define MISC_B_CLK_DIV_SHIFT	16
#define MISC_A_CLK_DIV_SHIFT	8
#define MISC_B_CLK_SEL_SHIFT	6
#define MISC_A_CLK_SEL_SHIFT	4
#define MISC_CLK_SEL_MASK	0x3
#define MISC_B_EN		BIT(1)
#define MISC_A_EN		BIT(0)

#define MESON_NUM_PWMS		2

static struct meson_pwm_channel_data {
	u8		reg_offset;
	u8		clk_sel_shift;
	u8		clk_div_shift;
	u32		clk_en_mask;
	u32		pwm_en_mask;
} meson_pwm_per_channel_data[MESON_NUM_PWMS] = {
	{
		.reg_offset	= REG_PWM_A,
		.clk_sel_shift	= MISC_A_CLK_SEL_SHIFT,
		.clk_div_shift	= MISC_A_CLK_DIV_SHIFT,
		.clk_en_mask	= MISC_A_CLK_EN,
		.pwm_en_mask	= MISC_A_EN,
	},
	{
		.reg_offset	= REG_PWM_B,
		.clk_sel_shift	= MISC_B_CLK_SEL_SHIFT,
		.clk_div_shift	= MISC_B_CLK_DIV_SHIFT,
		.clk_en_mask	= MISC_B_CLK_EN,
		.pwm_en_mask	= MISC_B_EN,
	}
};

struct meson_pwm_channel {
	unsigned int hi;
	unsigned int lo;
	u8 pre_div;

	struct clk *clk_parent;
	struct clk_mux mux;
	struct clk *clk;
};

struct meson_pwm_data {
	const char * const *parent_names;
	unsigned int num_parents;
};

struct meson_pwm {
	struct pwm_chip chip;
	const struct meson_pwm_data *data;
	struct meson_pwm_channel channels[MESON_NUM_PWMS];
	void __iomem *base;
	/*
	 * Protects register (write) access to the REG_MISC_AB register
	 * that is shared between the two PWMs.
	 */
	spinlock_t lock;
};

static inline struct meson_pwm *to_meson_pwm(struct pwm_chip *chip)
{
	return container_of(chip, struct meson_pwm, chip);
}

static int meson_pwm_request(struct pwm_chip *chip, struct pwm_device *pwm)
{
	struct meson_pwm *meson = to_meson_pwm(chip);
	struct meson_pwm_channel *channel;
	struct device *dev = chip->dev;
	int err;

	channel = pwm_get_chip_data(pwm);
	if (channel)
		return 0;

	channel = &meson->channels[pwm->hwpwm];

	if (channel->clk_parent) {
		err = clk_set_parent(channel->clk, channel->clk_parent);
		if (err < 0) {
			dev_err(dev, "failed to set parent %s for %s: %d\n",
				__clk_get_name(channel->clk_parent),
				__clk_get_name(channel->clk), err);
			return err;
		}
	}

	err = clk_prepare_enable(channel->clk);
	if (err < 0) {
		dev_err(dev, "failed to enable clock %s: %d\n",
			__clk_get_name(channel->clk), err);
		return err;
	}

	return pwm_set_chip_data(pwm, channel);
}

static void meson_pwm_free(struct pwm_chip *chip, struct pwm_device *pwm)
{
	struct meson_pwm_channel *channel = pwm_get_chip_data(pwm);

	if (channel)
		clk_disable_unprepare(channel->clk);
}

static int meson_pwm_calc(struct meson_pwm *meson, struct pwm_device *pwm,
			  const struct pwm_state *state)
{
	struct meson_pwm_channel *channel = pwm_get_chip_data(pwm);
	unsigned int duty, period, pre_div, cnt, duty_cnt;
	unsigned long fin_freq;

	duty = state->duty_cycle;
	period = state->period;

	if (state->polarity == PWM_POLARITY_INVERSED)
		duty = period - duty;

	fin_freq = clk_get_rate(channel->clk);
	if (fin_freq == 0) {
		dev_err(meson->chip.dev, "invalid source clock frequency\n");
		return -EINVAL;
	}

	dev_dbg(meson->chip.dev, "fin_freq: %lu Hz\n", fin_freq);

	pre_div = div64_u64(fin_freq * (u64)period, NSEC_PER_SEC * 0xffffLL);
	if (pre_div > MISC_CLK_DIV_MASK) {
		dev_err(meson->chip.dev, "unable to get period pre_div\n");
		return -EINVAL;
	}

	cnt = div64_u64(fin_freq * (u64)period, NSEC_PER_SEC * (pre_div + 1));
	if (cnt > 0xffff) {
		dev_err(meson->chip.dev, "unable to get period cnt\n");
		return -EINVAL;
	}

	dev_dbg(meson->chip.dev, "period=%u pre_div=%u cnt=%u\n", period,
		pre_div, cnt);

	if (duty == period) {
		channel->pre_div = pre_div;
		channel->hi = cnt;
		channel->lo = 0;
	} else if (duty == 0) {
		channel->pre_div = pre_div;
		channel->hi = 0;
		channel->lo = cnt;
	} else {
		/* Then check is we can have the duty with the same pre_div */
		duty_cnt = div64_u64(fin_freq * (u64)duty,
				     NSEC_PER_SEC * (pre_div + 1));
		if (duty_cnt > 0xffff) {
			dev_err(meson->chip.dev, "unable to get duty cycle\n");
			return -EINVAL;
		}

		dev_dbg(meson->chip.dev, "duty=%u pre_div=%u duty_cnt=%u\n",
			duty, pre_div, duty_cnt);

		channel->pre_div = pre_div;
		channel->hi = duty_cnt;
		channel->lo = cnt - duty_cnt;
	}

	return 0;
}

static void meson_pwm_enable(struct meson_pwm *meson, struct pwm_device *pwm)
{
	struct meson_pwm_channel *channel = pwm_get_chip_data(pwm);
	struct meson_pwm_channel_data *channel_data;
	unsigned long flags;
	u32 value;

	channel_data = &meson_pwm_per_channel_data[pwm->hwpwm];

	spin_lock_irqsave(&meson->lock, flags);

	value = readl(meson->base + REG_MISC_AB);
	value &= ~(MISC_CLK_DIV_MASK << channel_data->clk_div_shift);
	value |= channel->pre_div << channel_data->clk_div_shift;
	value |= channel_data->clk_en_mask;
	writel(value, meson->base + REG_MISC_AB);

	value = FIELD_PREP(PWM_HIGH_MASK, channel->hi) |
		FIELD_PREP(PWM_LOW_MASK, channel->lo);
	writel(value, meson->base + channel_data->reg_offset);

	value = readl(meson->base + REG_MISC_AB);
	value |= channel_data->pwm_en_mask;
	writel(value, meson->base + REG_MISC_AB);

	spin_unlock_irqrestore(&meson->lock, flags);
}

static void meson_pwm_disable(struct meson_pwm *meson, struct pwm_device *pwm)
{
	unsigned long flags;
	u32 value;

	spin_lock_irqsave(&meson->lock, flags);

	value = readl(meson->base + REG_MISC_AB);
	value &= ~meson_pwm_per_channel_data[pwm->hwpwm].pwm_en_mask;
	writel(value, meson->base + REG_MISC_AB);

	spin_unlock_irqrestore(&meson->lock, flags);
}

static int meson_pwm_apply(struct pwm_chip *chip, struct pwm_device *pwm,
			   const struct pwm_state *state)
{
	struct meson_pwm_channel *channel = pwm_get_chip_data(pwm);
	struct meson_pwm *meson = to_meson_pwm(chip);
	int err = 0;

	if (!state)
		return -EINVAL;

	if (!state->enabled) {
		if (state->polarity == PWM_POLARITY_INVERSED) {
			/*
			 * This IP block revision doesn't have an "always high"
			 * setting which we can use for "inverted disabled".
			 * Instead we achieve this using the same settings
			 * that we use a pre_div of 0 (to get the shortest
			 * possible duration for one "count") and
			 * "period == duty_cycle". This results in a signal
			 * which is LOW for one "count", while being HIGH for
			 * the rest of the (so the signal is HIGH for slightly
			 * less than 100% of the period, but this is the best
			 * we can achieve).
			 */
			channel->pre_div = 0;
			channel->hi = ~0;
			channel->lo = 0;

			meson_pwm_enable(meson, pwm);
		} else {
			meson_pwm_disable(meson, pwm);
		}
	} else {
		err = meson_pwm_calc(meson, pwm, state);
		if (err < 0)
			return err;

		meson_pwm_enable(meson, pwm);
	}

	return 0;
}

static unsigned int meson_pwm_cnt_to_ns(struct pwm_chip *chip,
					struct pwm_device *pwm, u32 cnt)
{
	struct meson_pwm *meson = to_meson_pwm(chip);
	struct meson_pwm_channel *channel;
	unsigned long fin_freq;
	u32 fin_ns;

	/* to_meson_pwm() can only be used after .get_state() is called */
	channel = &meson->channels[pwm->hwpwm];

	fin_freq = clk_get_rate(channel->clk);
	if (fin_freq == 0)
		return 0;

	fin_ns = div_u64(NSEC_PER_SEC, fin_freq);

	return cnt * fin_ns * (channel->pre_div + 1);
}

static void meson_pwm_get_state(struct pwm_chip *chip, struct pwm_device *pwm,
				struct pwm_state *state)
{
	struct meson_pwm *meson = to_meson_pwm(chip);
	struct meson_pwm_channel_data *channel_data;
	struct meson_pwm_channel *channel;
	u32 value, tmp;

	if (!state)
		return;

	channel = &meson->channels[pwm->hwpwm];
	channel_data = &meson_pwm_per_channel_data[pwm->hwpwm];

	value = readl(meson->base + REG_MISC_AB);

	tmp = channel_data->pwm_en_mask | channel_data->clk_en_mask;
	state->enabled = (value & tmp) == tmp;

	tmp = value >> channel_data->clk_div_shift;
	channel->pre_div = FIELD_GET(MISC_CLK_DIV_MASK, tmp);

	value = readl(meson->base + channel_data->reg_offset);

	channel->lo = FIELD_GET(PWM_LOW_MASK, value);
	channel->hi = FIELD_GET(PWM_HIGH_MASK, value);

	if (channel->lo == 0) {
		state->period = meson_pwm_cnt_to_ns(chip, pwm, channel->hi);
		state->duty_cycle = state->period;
	} else if (channel->lo >= channel->hi) {
		state->period = meson_pwm_cnt_to_ns(chip, pwm,
						    channel->lo + channel->hi);
		state->duty_cycle = meson_pwm_cnt_to_ns(chip, pwm,
							channel->hi);
	} else {
		state->period = 0;
		state->duty_cycle = 0;
	}
}

static const struct pwm_ops meson_pwm_ops = {
	.request = meson_pwm_request,
	.free = meson_pwm_free,
	.apply = meson_pwm_apply,
	.get_state = meson_pwm_get_state,
	.owner = THIS_MODULE,
};

static const char * const pwm_meson8b_parent_names[] = {
	"xtal", "vid_pll", "fclk_div4", "fclk_div3"
};

static const struct meson_pwm_data pwm_meson8b_data = {
	.parent_names = pwm_meson8b_parent_names,
	.num_parents = ARRAY_SIZE(pwm_meson8b_parent_names),
};

static const char * const pwm_gxbb_parent_names[] = {
	"xtal", "hdmi_pll", "fclk_div4", "fclk_div3"
};

static const struct meson_pwm_data pwm_gxbb_data = {
	.parent_names = pwm_gxbb_parent_names,
	.num_parents = ARRAY_SIZE(pwm_gxbb_parent_names),
};

/*
 * Only the 2 first inputs of the GXBB AO PWMs are valid
 * The last 2 are grounded
 */
static const char * const pwm_gxbb_ao_parent_names[] = {
	"xtal", "clk81"
};

static const struct meson_pwm_data pwm_gxbb_ao_data = {
	.parent_names = pwm_gxbb_ao_parent_names,
	.num_parents = ARRAY_SIZE(pwm_gxbb_ao_parent_names),
};

static const char * const pwm_axg_ee_parent_names[] = {
	"xtal", "fclk_div5", "fclk_div4", "fclk_div3"
};

static const struct meson_pwm_data pwm_axg_ee_data = {
	.parent_names = pwm_axg_ee_parent_names,
	.num_parents = ARRAY_SIZE(pwm_axg_ee_parent_names),
};

static const char * const pwm_axg_ao_parent_names[] = {
	"aoclk81", "xtal", "fclk_div4", "fclk_div5"
};

static const struct meson_pwm_data pwm_axg_ao_data = {
	.parent_names = pwm_axg_ao_parent_names,
	.num_parents = ARRAY_SIZE(pwm_axg_ao_parent_names),
};

static const char * const pwm_g12a_ao_ab_parent_names[] = {
	"xtal", "aoclk81", "fclk_div4", "fclk_div5"
};

static const struct meson_pwm_data pwm_g12a_ao_ab_data = {
	.parent_names = pwm_g12a_ao_ab_parent_names,
	.num_parents = ARRAY_SIZE(pwm_g12a_ao_ab_parent_names),
};

static const char * const pwm_g12a_ao_cd_parent_names[] = {
	"xtal", "aoclk81",
};

static const struct meson_pwm_data pwm_g12a_ao_cd_data = {
	.parent_names = pwm_g12a_ao_cd_parent_names,
	.num_parents = ARRAY_SIZE(pwm_g12a_ao_cd_parent_names),
};

static const char * const pwm_g12a_ee_parent_names[] = {
	"xtal", "hdmi_pll", "fclk_div4", "fclk_div3"
};

static const struct meson_pwm_data pwm_g12a_ee_data = {
	.parent_names = pwm_g12a_ee_parent_names,
	.num_parents = ARRAY_SIZE(pwm_g12a_ee_parent_names),
};

static const struct of_device_id meson_pwm_matches[] = {
	{
		.compatible = "amlogic,meson8b-pwm",
		.data = &pwm_meson8b_data
	},
	{
		.compatible = "amlogic,meson-gxbb-pwm",
		.data = &pwm_gxbb_data
	},
	{
		.compatible = "amlogic,meson-gxbb-ao-pwm",
		.data = &pwm_gxbb_ao_data
	},
	{
		.compatible = "amlogic,meson-axg-ee-pwm",
		.data = &pwm_axg_ee_data
	},
	{
		.compatible = "amlogic,meson-axg-ao-pwm",
		.data = &pwm_axg_ao_data
	},
	{
		.compatible = "amlogic,meson-g12a-ee-pwm",
		.data = &pwm_g12a_ee_data
	},
	{
		.compatible = "amlogic,meson-g12a-ao-pwm-ab",
		.data = &pwm_g12a_ao_ab_data
	},
	{
		.compatible = "amlogic,meson-g12a-ao-pwm-cd",
		.data = &pwm_g12a_ao_cd_data
	},
	{},
};
MODULE_DEVICE_TABLE(of, meson_pwm_matches);

static int meson_pwm_init_channels(struct meson_pwm *meson)
{
	struct device *dev = meson->chip.dev;
	struct clk_init_data init;
	unsigned int i;
	char name[255];
	int err;

	for (i = 0; i < meson->chip.npwm; i++) {
		struct meson_pwm_channel *channel = &meson->channels[i];

		snprintf(name, sizeof(name), "%s#mux%u", dev_name(dev), i);

		init.name = name;
		init.ops = &clk_mux_ops;
		init.flags = 0;
		init.parent_names = meson->data->parent_names;
		init.num_parents = meson->data->num_parents;

		channel->mux.reg = meson->base + REG_MISC_AB;
		channel->mux.shift =
				meson_pwm_per_channel_data[i].clk_sel_shift;
		channel->mux.mask = MISC_CLK_SEL_MASK;
		channel->mux.flags = 0;
		channel->mux.lock = &meson->lock;
		channel->mux.table = NULL;
		channel->mux.hw.init = &init;

		channel->clk = devm_clk_register(dev, &channel->mux.hw);
		if (IS_ERR(channel->clk)) {
			err = PTR_ERR(channel->clk);
			dev_err(dev, "failed to register %s: %d\n", name, err);
			return err;
		}

		snprintf(name, sizeof(name), "clkin%u", i);

		channel->clk_parent = devm_clk_get_optional(dev, name);
		if (IS_ERR(channel->clk_parent))
			return PTR_ERR(channel->clk_parent);
	}

	return 0;
}

static int meson_pwm_probe(struct platform_device *pdev)
{
	struct meson_pwm *meson;
	int err;

	meson = devm_kzalloc(&pdev->dev, sizeof(*meson), GFP_KERNEL);
	if (!meson)
		return -ENOMEM;

	meson->base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(meson->base))
		return PTR_ERR(meson->base);

	spin_lock_init(&meson->lock);
	meson->chip.dev = &pdev->dev;
	meson->chip.ops = &meson_pwm_ops;
	meson->chip.npwm = MESON_NUM_PWMS;
	meson->chip.of_xlate = of_pwm_xlate_with_flags;
	meson->chip.of_pwm_n_cells = 3;

	meson->data = of_device_get_match_data(&pdev->dev);

	err = meson_pwm_init_channels(meson);
	if (err < 0)
		return err;

	err = pwmchip_add(&meson->chip);
	if (err < 0) {
		dev_err(&pdev->dev, "failed to register PWM chip: %d\n", err);
		return err;
	}

	platform_set_drvdata(pdev, meson);

	return 0;
}

static int meson_pwm_remove(struct platform_device *pdev)
{
	struct meson_pwm *meson = platform_get_drvdata(pdev);

	return pwmchip_remove(&meson->chip);
}

static struct platform_driver meson_pwm_driver = {
	.driver = {
		.name = "meson-pwm",
		.of_match_table = meson_pwm_matches,
	},
	.probe = meson_pwm_probe,
	.remove = meson_pwm_remove,
};
module_platform_driver(meson_pwm_driver);

MODULE_DESCRIPTION("Amlogic Meson PWM Generator driver");
MODULE_AUTHOR("Neil Armstrong <narmstrong@baylibre.com>");
MODULE_LICENSE("Dual BSD/GPL");
