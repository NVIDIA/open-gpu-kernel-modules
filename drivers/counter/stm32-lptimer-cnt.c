// SPDX-License-Identifier: GPL-2.0
/*
 * STM32 Low-Power Timer Encoder and Counter driver
 *
 * Copyright (C) STMicroelectronics 2017
 *
 * Author: Fabrice Gasnier <fabrice.gasnier@st.com>
 *
 * Inspired by 104-quad-8 and stm32-timer-trigger drivers.
 *
 */

#include <linux/bitfield.h>
#include <linux/counter.h>
#include <linux/mfd/stm32-lptimer.h>
#include <linux/mod_devicetable.h>
#include <linux/module.h>
#include <linux/pinctrl/consumer.h>
#include <linux/platform_device.h>

struct stm32_lptim_cnt {
	struct counter_device counter;
	struct device *dev;
	struct regmap *regmap;
	struct clk *clk;
	u32 ceiling;
	u32 polarity;
	u32 quadrature_mode;
	bool enabled;
};

static int stm32_lptim_is_enabled(struct stm32_lptim_cnt *priv)
{
	u32 val;
	int ret;

	ret = regmap_read(priv->regmap, STM32_LPTIM_CR, &val);
	if (ret)
		return ret;

	return FIELD_GET(STM32_LPTIM_ENABLE, val);
}

static int stm32_lptim_set_enable_state(struct stm32_lptim_cnt *priv,
					int enable)
{
	int ret;
	u32 val;

	val = FIELD_PREP(STM32_LPTIM_ENABLE, enable);
	ret = regmap_write(priv->regmap, STM32_LPTIM_CR, val);
	if (ret)
		return ret;

	if (!enable) {
		clk_disable(priv->clk);
		priv->enabled = false;
		return 0;
	}

	/* LP timer must be enabled before writing CMP & ARR */
	ret = regmap_write(priv->regmap, STM32_LPTIM_ARR, priv->ceiling);
	if (ret)
		return ret;

	ret = regmap_write(priv->regmap, STM32_LPTIM_CMP, 0);
	if (ret)
		return ret;

	/* ensure CMP & ARR registers are properly written */
	ret = regmap_read_poll_timeout(priv->regmap, STM32_LPTIM_ISR, val,
				       (val & STM32_LPTIM_CMPOK_ARROK),
				       100, 1000);
	if (ret)
		return ret;

	ret = regmap_write(priv->regmap, STM32_LPTIM_ICR,
			   STM32_LPTIM_CMPOKCF_ARROKCF);
	if (ret)
		return ret;

	ret = clk_enable(priv->clk);
	if (ret) {
		regmap_write(priv->regmap, STM32_LPTIM_CR, 0);
		return ret;
	}
	priv->enabled = true;

	/* Start LP timer in continuous mode */
	return regmap_update_bits(priv->regmap, STM32_LPTIM_CR,
				  STM32_LPTIM_CNTSTRT, STM32_LPTIM_CNTSTRT);
}

static int stm32_lptim_setup(struct stm32_lptim_cnt *priv, int enable)
{
	u32 mask = STM32_LPTIM_ENC | STM32_LPTIM_COUNTMODE |
		   STM32_LPTIM_CKPOL | STM32_LPTIM_PRESC;
	u32 val;

	/* Setup LP timer encoder/counter and polarity, without prescaler */
	if (priv->quadrature_mode)
		val = enable ? STM32_LPTIM_ENC : 0;
	else
		val = enable ? STM32_LPTIM_COUNTMODE : 0;
	val |= FIELD_PREP(STM32_LPTIM_CKPOL, enable ? priv->polarity : 0);

	return regmap_update_bits(priv->regmap, STM32_LPTIM_CFGR, mask, val);
}

/**
 * enum stm32_lptim_cnt_function - enumerates LPTimer counter & encoder modes
 * @STM32_LPTIM_COUNTER_INCREASE: up count on IN1 rising, falling or both edges
 * @STM32_LPTIM_ENCODER_BOTH_EDGE: count on both edges (IN1 & IN2 quadrature)
 *
 * In non-quadrature mode, device counts up on active edge.
 * In quadrature mode, encoder counting scenarios are as follows:
 * +---------+----------+--------------------+--------------------+
 * | Active  | Level on |      IN1 signal    |     IN2 signal     |
 * | edge    | opposite +----------+---------+----------+---------+
 * |         | signal   |  Rising  | Falling |  Rising  | Falling |
 * +---------+----------+----------+---------+----------+---------+
 * | Rising  | High ->  |   Down   |    -    |   Up     |    -    |
 * | edge    | Low  ->  |   Up     |    -    |   Down   |    -    |
 * +---------+----------+----------+---------+----------+---------+
 * | Falling | High ->  |    -     |   Up    |    -     |   Down  |
 * | edge    | Low  ->  |    -     |   Down  |    -     |   Up    |
 * +---------+----------+----------+---------+----------+---------+
 * | Both    | High ->  |   Down   |   Up    |   Up     |   Down  |
 * | edges   | Low  ->  |   Up     |   Down  |   Down   |   Up    |
 * +---------+----------+----------+---------+----------+---------+
 */
enum stm32_lptim_cnt_function {
	STM32_LPTIM_COUNTER_INCREASE,
	STM32_LPTIM_ENCODER_BOTH_EDGE,
};

static enum counter_count_function stm32_lptim_cnt_functions[] = {
	[STM32_LPTIM_COUNTER_INCREASE] = COUNTER_COUNT_FUNCTION_INCREASE,
	[STM32_LPTIM_ENCODER_BOTH_EDGE] = COUNTER_COUNT_FUNCTION_QUADRATURE_X4,
};

enum stm32_lptim_synapse_action {
	STM32_LPTIM_SYNAPSE_ACTION_RISING_EDGE,
	STM32_LPTIM_SYNAPSE_ACTION_FALLING_EDGE,
	STM32_LPTIM_SYNAPSE_ACTION_BOTH_EDGES,
	STM32_LPTIM_SYNAPSE_ACTION_NONE,
};

static enum counter_synapse_action stm32_lptim_cnt_synapse_actions[] = {
	/* Index must match with stm32_lptim_cnt_polarity[] (priv->polarity) */
	[STM32_LPTIM_SYNAPSE_ACTION_RISING_EDGE] = COUNTER_SYNAPSE_ACTION_RISING_EDGE,
	[STM32_LPTIM_SYNAPSE_ACTION_FALLING_EDGE] = COUNTER_SYNAPSE_ACTION_FALLING_EDGE,
	[STM32_LPTIM_SYNAPSE_ACTION_BOTH_EDGES] = COUNTER_SYNAPSE_ACTION_BOTH_EDGES,
	[STM32_LPTIM_SYNAPSE_ACTION_NONE] = COUNTER_SYNAPSE_ACTION_NONE,
};

static int stm32_lptim_cnt_read(struct counter_device *counter,
				struct counter_count *count, unsigned long *val)
{
	struct stm32_lptim_cnt *const priv = counter->priv;
	u32 cnt;
	int ret;

	ret = regmap_read(priv->regmap, STM32_LPTIM_CNT, &cnt);
	if (ret)
		return ret;

	*val = cnt;

	return 0;
}

static int stm32_lptim_cnt_function_get(struct counter_device *counter,
					struct counter_count *count,
					size_t *function)
{
	struct stm32_lptim_cnt *const priv = counter->priv;

	if (!priv->quadrature_mode) {
		*function = STM32_LPTIM_COUNTER_INCREASE;
		return 0;
	}

	if (priv->polarity == STM32_LPTIM_SYNAPSE_ACTION_BOTH_EDGES) {
		*function = STM32_LPTIM_ENCODER_BOTH_EDGE;
		return 0;
	}

	return -EINVAL;
}

static int stm32_lptim_cnt_function_set(struct counter_device *counter,
					struct counter_count *count,
					size_t function)
{
	struct stm32_lptim_cnt *const priv = counter->priv;

	if (stm32_lptim_is_enabled(priv))
		return -EBUSY;

	switch (function) {
	case STM32_LPTIM_COUNTER_INCREASE:
		priv->quadrature_mode = 0;
		return 0;
	case STM32_LPTIM_ENCODER_BOTH_EDGE:
		priv->quadrature_mode = 1;
		priv->polarity = STM32_LPTIM_SYNAPSE_ACTION_BOTH_EDGES;
		return 0;
	}

	return -EINVAL;
}

static ssize_t stm32_lptim_cnt_enable_read(struct counter_device *counter,
					   struct counter_count *count,
					   void *private, char *buf)
{
	struct stm32_lptim_cnt *const priv = counter->priv;
	int ret;

	ret = stm32_lptim_is_enabled(priv);
	if (ret < 0)
		return ret;

	return scnprintf(buf, PAGE_SIZE, "%u\n", ret);
}

static ssize_t stm32_lptim_cnt_enable_write(struct counter_device *counter,
					    struct counter_count *count,
					    void *private,
					    const char *buf, size_t len)
{
	struct stm32_lptim_cnt *const priv = counter->priv;
	bool enable;
	int ret;

	ret = kstrtobool(buf, &enable);
	if (ret)
		return ret;

	/* Check nobody uses the timer, or already disabled/enabled */
	ret = stm32_lptim_is_enabled(priv);
	if ((ret < 0) || (!ret && !enable))
		return ret;
	if (enable && ret)
		return -EBUSY;

	ret = stm32_lptim_setup(priv, enable);
	if (ret)
		return ret;

	ret = stm32_lptim_set_enable_state(priv, enable);
	if (ret)
		return ret;

	return len;
}

static ssize_t stm32_lptim_cnt_ceiling_read(struct counter_device *counter,
					    struct counter_count *count,
					    void *private, char *buf)
{
	struct stm32_lptim_cnt *const priv = counter->priv;

	return snprintf(buf, PAGE_SIZE, "%u\n", priv->ceiling);
}

static ssize_t stm32_lptim_cnt_ceiling_write(struct counter_device *counter,
					     struct counter_count *count,
					     void *private,
					     const char *buf, size_t len)
{
	struct stm32_lptim_cnt *const priv = counter->priv;
	unsigned int ceiling;
	int ret;

	if (stm32_lptim_is_enabled(priv))
		return -EBUSY;

	ret = kstrtouint(buf, 0, &ceiling);
	if (ret)
		return ret;

	if (ceiling > STM32_LPTIM_MAX_ARR)
		return -EINVAL;

	priv->ceiling = ceiling;

	return len;
}

static const struct counter_count_ext stm32_lptim_cnt_ext[] = {
	{
		.name = "enable",
		.read = stm32_lptim_cnt_enable_read,
		.write = stm32_lptim_cnt_enable_write
	},
	{
		.name = "ceiling",
		.read = stm32_lptim_cnt_ceiling_read,
		.write = stm32_lptim_cnt_ceiling_write
	},
};

static int stm32_lptim_cnt_action_get(struct counter_device *counter,
				      struct counter_count *count,
				      struct counter_synapse *synapse,
				      size_t *action)
{
	struct stm32_lptim_cnt *const priv = counter->priv;
	size_t function;
	int err;

	err = stm32_lptim_cnt_function_get(counter, count, &function);
	if (err)
		return err;

	switch (function) {
	case STM32_LPTIM_COUNTER_INCREASE:
		/* LP Timer acts as up-counter on input 1 */
		if (synapse->signal->id == count->synapses[0].signal->id)
			*action = priv->polarity;
		else
			*action = STM32_LPTIM_SYNAPSE_ACTION_NONE;
		return 0;
	case STM32_LPTIM_ENCODER_BOTH_EDGE:
		*action = priv->polarity;
		return 0;
	}

	return -EINVAL;
}

static int stm32_lptim_cnt_action_set(struct counter_device *counter,
				      struct counter_count *count,
				      struct counter_synapse *synapse,
				      size_t action)
{
	struct stm32_lptim_cnt *const priv = counter->priv;
	size_t function;
	int err;

	if (stm32_lptim_is_enabled(priv))
		return -EBUSY;

	err = stm32_lptim_cnt_function_get(counter, count, &function);
	if (err)
		return err;

	/* only set polarity when in counter mode (on input 1) */
	if (function == STM32_LPTIM_COUNTER_INCREASE
	    && synapse->signal->id == count->synapses[0].signal->id) {
		switch (action) {
		case STM32_LPTIM_SYNAPSE_ACTION_RISING_EDGE:
		case STM32_LPTIM_SYNAPSE_ACTION_FALLING_EDGE:
		case STM32_LPTIM_SYNAPSE_ACTION_BOTH_EDGES:
			priv->polarity = action;
			return 0;
		}
	}

	return -EINVAL;
}

static const struct counter_ops stm32_lptim_cnt_ops = {
	.count_read = stm32_lptim_cnt_read,
	.function_get = stm32_lptim_cnt_function_get,
	.function_set = stm32_lptim_cnt_function_set,
	.action_get = stm32_lptim_cnt_action_get,
	.action_set = stm32_lptim_cnt_action_set,
};

static struct counter_signal stm32_lptim_cnt_signals[] = {
	{
		.id = 0,
		.name = "Channel 1 Quadrature A"
	},
	{
		.id = 1,
		.name = "Channel 1 Quadrature B"
	}
};

static struct counter_synapse stm32_lptim_cnt_synapses[] = {
	{
		.actions_list = stm32_lptim_cnt_synapse_actions,
		.num_actions = ARRAY_SIZE(stm32_lptim_cnt_synapse_actions),
		.signal = &stm32_lptim_cnt_signals[0]
	},
	{
		.actions_list = stm32_lptim_cnt_synapse_actions,
		.num_actions = ARRAY_SIZE(stm32_lptim_cnt_synapse_actions),
		.signal = &stm32_lptim_cnt_signals[1]
	}
};

/* LP timer with encoder */
static struct counter_count stm32_lptim_enc_counts = {
	.id = 0,
	.name = "LPTimer Count",
	.functions_list = stm32_lptim_cnt_functions,
	.num_functions = ARRAY_SIZE(stm32_lptim_cnt_functions),
	.synapses = stm32_lptim_cnt_synapses,
	.num_synapses = ARRAY_SIZE(stm32_lptim_cnt_synapses),
	.ext = stm32_lptim_cnt_ext,
	.num_ext = ARRAY_SIZE(stm32_lptim_cnt_ext)
};

/* LP timer without encoder (counter only) */
static struct counter_count stm32_lptim_in1_counts = {
	.id = 0,
	.name = "LPTimer Count",
	.functions_list = stm32_lptim_cnt_functions,
	.num_functions = 1,
	.synapses = stm32_lptim_cnt_synapses,
	.num_synapses = 1,
	.ext = stm32_lptim_cnt_ext,
	.num_ext = ARRAY_SIZE(stm32_lptim_cnt_ext)
};

static int stm32_lptim_cnt_probe(struct platform_device *pdev)
{
	struct stm32_lptimer *ddata = dev_get_drvdata(pdev->dev.parent);
	struct stm32_lptim_cnt *priv;

	if (IS_ERR_OR_NULL(ddata))
		return -EINVAL;

	priv = devm_kzalloc(&pdev->dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	priv->dev = &pdev->dev;
	priv->regmap = ddata->regmap;
	priv->clk = ddata->clk;
	priv->ceiling = STM32_LPTIM_MAX_ARR;

	/* Initialize Counter device */
	priv->counter.name = dev_name(&pdev->dev);
	priv->counter.parent = &pdev->dev;
	priv->counter.ops = &stm32_lptim_cnt_ops;
	if (ddata->has_encoder) {
		priv->counter.counts = &stm32_lptim_enc_counts;
		priv->counter.num_signals = ARRAY_SIZE(stm32_lptim_cnt_signals);
	} else {
		priv->counter.counts = &stm32_lptim_in1_counts;
		priv->counter.num_signals = 1;
	}
	priv->counter.num_counts = 1;
	priv->counter.signals = stm32_lptim_cnt_signals;
	priv->counter.priv = priv;

	platform_set_drvdata(pdev, priv);

	return devm_counter_register(&pdev->dev, &priv->counter);
}

#ifdef CONFIG_PM_SLEEP
static int stm32_lptim_cnt_suspend(struct device *dev)
{
	struct stm32_lptim_cnt *priv = dev_get_drvdata(dev);
	int ret;

	/* Only take care of enabled counter: don't disturb other MFD child */
	if (priv->enabled) {
		ret = stm32_lptim_setup(priv, 0);
		if (ret)
			return ret;

		ret = stm32_lptim_set_enable_state(priv, 0);
		if (ret)
			return ret;

		/* Force enable state for later resume */
		priv->enabled = true;
	}

	return pinctrl_pm_select_sleep_state(dev);
}

static int stm32_lptim_cnt_resume(struct device *dev)
{
	struct stm32_lptim_cnt *priv = dev_get_drvdata(dev);
	int ret;

	ret = pinctrl_pm_select_default_state(dev);
	if (ret)
		return ret;

	if (priv->enabled) {
		priv->enabled = false;
		ret = stm32_lptim_setup(priv, 1);
		if (ret)
			return ret;

		ret = stm32_lptim_set_enable_state(priv, 1);
		if (ret)
			return ret;
	}

	return 0;
}
#endif

static SIMPLE_DEV_PM_OPS(stm32_lptim_cnt_pm_ops, stm32_lptim_cnt_suspend,
			 stm32_lptim_cnt_resume);

static const struct of_device_id stm32_lptim_cnt_of_match[] = {
	{ .compatible = "st,stm32-lptimer-counter", },
	{},
};
MODULE_DEVICE_TABLE(of, stm32_lptim_cnt_of_match);

static struct platform_driver stm32_lptim_cnt_driver = {
	.probe = stm32_lptim_cnt_probe,
	.driver = {
		.name = "stm32-lptimer-counter",
		.of_match_table = stm32_lptim_cnt_of_match,
		.pm = &stm32_lptim_cnt_pm_ops,
	},
};
module_platform_driver(stm32_lptim_cnt_driver);

MODULE_AUTHOR("Fabrice Gasnier <fabrice.gasnier@st.com>");
MODULE_ALIAS("platform:stm32-lptimer-counter");
MODULE_DESCRIPTION("STMicroelectronics STM32 LPTIM counter driver");
MODULE_LICENSE("GPL v2");
