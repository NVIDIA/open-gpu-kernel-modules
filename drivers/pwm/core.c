// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Generic pwmlib implementation
 *
 * Copyright (C) 2011 Sascha Hauer <s.hauer@pengutronix.de>
 * Copyright (C) 2011-2012 Avionic Design GmbH
 */

#include <linux/acpi.h>
#include <linux/module.h>
#include <linux/pwm.h>
#include <linux/radix-tree.h>
#include <linux/list.h>
#include <linux/mutex.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/debugfs.h>
#include <linux/seq_file.h>

#include <dt-bindings/pwm/pwm.h>

#define CREATE_TRACE_POINTS
#include <trace/events/pwm.h>

#define MAX_PWMS 1024

static DEFINE_MUTEX(pwm_lookup_lock);
static LIST_HEAD(pwm_lookup_list);
static DEFINE_MUTEX(pwm_lock);
static LIST_HEAD(pwm_chips);
static DECLARE_BITMAP(allocated_pwms, MAX_PWMS);
static RADIX_TREE(pwm_tree, GFP_KERNEL);

static struct pwm_device *pwm_to_device(unsigned int pwm)
{
	return radix_tree_lookup(&pwm_tree, pwm);
}

static int alloc_pwms(unsigned int count)
{
	unsigned int start;

	start = bitmap_find_next_zero_area(allocated_pwms, MAX_PWMS, 0,
					   count, 0);

	if (start + count > MAX_PWMS)
		return -ENOSPC;

	return start;
}

static void free_pwms(struct pwm_chip *chip)
{
	unsigned int i;

	for (i = 0; i < chip->npwm; i++) {
		struct pwm_device *pwm = &chip->pwms[i];

		radix_tree_delete(&pwm_tree, pwm->pwm);
	}

	bitmap_clear(allocated_pwms, chip->base, chip->npwm);

	kfree(chip->pwms);
	chip->pwms = NULL;
}

static struct pwm_chip *pwmchip_find_by_name(const char *name)
{
	struct pwm_chip *chip;

	if (!name)
		return NULL;

	mutex_lock(&pwm_lock);

	list_for_each_entry(chip, &pwm_chips, list) {
		const char *chip_name = dev_name(chip->dev);

		if (chip_name && strcmp(chip_name, name) == 0) {
			mutex_unlock(&pwm_lock);
			return chip;
		}
	}

	mutex_unlock(&pwm_lock);

	return NULL;
}

static int pwm_device_request(struct pwm_device *pwm, const char *label)
{
	int err;

	if (test_bit(PWMF_REQUESTED, &pwm->flags))
		return -EBUSY;

	if (!try_module_get(pwm->chip->ops->owner))
		return -ENODEV;

	if (pwm->chip->ops->request) {
		err = pwm->chip->ops->request(pwm->chip, pwm);
		if (err) {
			module_put(pwm->chip->ops->owner);
			return err;
		}
	}

	if (pwm->chip->ops->get_state) {
		pwm->chip->ops->get_state(pwm->chip, pwm, &pwm->state);
		trace_pwm_get(pwm, &pwm->state);

		if (IS_ENABLED(CONFIG_PWM_DEBUG))
			pwm->last = pwm->state;
	}

	set_bit(PWMF_REQUESTED, &pwm->flags);
	pwm->label = label;

	return 0;
}

struct pwm_device *
of_pwm_xlate_with_flags(struct pwm_chip *pc, const struct of_phandle_args *args)
{
	struct pwm_device *pwm;

	/* check, whether the driver supports a third cell for flags */
	if (pc->of_pwm_n_cells < 3)
		return ERR_PTR(-EINVAL);

	/* flags in the third cell are optional */
	if (args->args_count < 2)
		return ERR_PTR(-EINVAL);

	if (args->args[0] >= pc->npwm)
		return ERR_PTR(-EINVAL);

	pwm = pwm_request_from_chip(pc, args->args[0], NULL);
	if (IS_ERR(pwm))
		return pwm;

	pwm->args.period = args->args[1];
	pwm->args.polarity = PWM_POLARITY_NORMAL;

	if (args->args_count > 2 && args->args[2] & PWM_POLARITY_INVERTED)
		pwm->args.polarity = PWM_POLARITY_INVERSED;

	return pwm;
}
EXPORT_SYMBOL_GPL(of_pwm_xlate_with_flags);

static struct pwm_device *
of_pwm_simple_xlate(struct pwm_chip *pc, const struct of_phandle_args *args)
{
	struct pwm_device *pwm;

	/* sanity check driver support */
	if (pc->of_pwm_n_cells < 2)
		return ERR_PTR(-EINVAL);

	/* all cells are required */
	if (args->args_count != pc->of_pwm_n_cells)
		return ERR_PTR(-EINVAL);

	if (args->args[0] >= pc->npwm)
		return ERR_PTR(-EINVAL);

	pwm = pwm_request_from_chip(pc, args->args[0], NULL);
	if (IS_ERR(pwm))
		return pwm;

	pwm->args.period = args->args[1];

	return pwm;
}

static void of_pwmchip_add(struct pwm_chip *chip)
{
	if (!chip->dev || !chip->dev->of_node)
		return;

	if (!chip->of_xlate) {
		chip->of_xlate = of_pwm_simple_xlate;
		chip->of_pwm_n_cells = 2;
	}

	of_node_get(chip->dev->of_node);
}

static void of_pwmchip_remove(struct pwm_chip *chip)
{
	if (chip->dev)
		of_node_put(chip->dev->of_node);
}

/**
 * pwm_set_chip_data() - set private chip data for a PWM
 * @pwm: PWM device
 * @data: pointer to chip-specific data
 *
 * Returns: 0 on success or a negative error code on failure.
 */
int pwm_set_chip_data(struct pwm_device *pwm, void *data)
{
	if (!pwm)
		return -EINVAL;

	pwm->chip_data = data;

	return 0;
}
EXPORT_SYMBOL_GPL(pwm_set_chip_data);

/**
 * pwm_get_chip_data() - get private chip data for a PWM
 * @pwm: PWM device
 *
 * Returns: A pointer to the chip-private data for the PWM device.
 */
void *pwm_get_chip_data(struct pwm_device *pwm)
{
	return pwm ? pwm->chip_data : NULL;
}
EXPORT_SYMBOL_GPL(pwm_get_chip_data);

static bool pwm_ops_check(const struct pwm_chip *chip)
{

	const struct pwm_ops *ops = chip->ops;

	/* driver supports legacy, non-atomic operation */
	if (ops->config && ops->enable && ops->disable) {
		if (IS_ENABLED(CONFIG_PWM_DEBUG))
			dev_warn(chip->dev,
				 "Driver needs updating to atomic API\n");

		return true;
	}

	if (!ops->apply)
		return false;

	if (IS_ENABLED(CONFIG_PWM_DEBUG) && !ops->get_state)
		dev_warn(chip->dev,
			 "Please implement the .get_state() callback\n");

	return true;
}

/**
 * pwmchip_add() - register a new PWM chip
 * @chip: the PWM chip to add
 *
 * Register a new PWM chip.
 *
 * Returns: 0 on success or a negative error code on failure.
 */
int pwmchip_add(struct pwm_chip *chip)
{
	struct pwm_device *pwm;
	unsigned int i;
	int ret;

	if (!chip || !chip->dev || !chip->ops || !chip->npwm)
		return -EINVAL;

	if (!pwm_ops_check(chip))
		return -EINVAL;

	mutex_lock(&pwm_lock);

	ret = alloc_pwms(chip->npwm);
	if (ret < 0)
		goto out;

	chip->base = ret;

	chip->pwms = kcalloc(chip->npwm, sizeof(*pwm), GFP_KERNEL);
	if (!chip->pwms) {
		ret = -ENOMEM;
		goto out;
	}

	for (i = 0; i < chip->npwm; i++) {
		pwm = &chip->pwms[i];

		pwm->chip = chip;
		pwm->pwm = chip->base + i;
		pwm->hwpwm = i;

		radix_tree_insert(&pwm_tree, pwm->pwm, pwm);
	}

	bitmap_set(allocated_pwms, chip->base, chip->npwm);

	INIT_LIST_HEAD(&chip->list);
	list_add(&chip->list, &pwm_chips);

	ret = 0;

	if (IS_ENABLED(CONFIG_OF))
		of_pwmchip_add(chip);

out:
	mutex_unlock(&pwm_lock);

	if (!ret)
		pwmchip_sysfs_export(chip);

	return ret;
}
EXPORT_SYMBOL_GPL(pwmchip_add);

/**
 * pwmchip_remove() - remove a PWM chip
 * @chip: the PWM chip to remove
 *
 * Removes a PWM chip. This function may return busy if the PWM chip provides
 * a PWM device that is still requested.
 *
 * Returns: 0 on success or a negative error code on failure.
 */
int pwmchip_remove(struct pwm_chip *chip)
{
	unsigned int i;
	int ret = 0;

	pwmchip_sysfs_unexport(chip);

	mutex_lock(&pwm_lock);

	for (i = 0; i < chip->npwm; i++) {
		struct pwm_device *pwm = &chip->pwms[i];

		if (test_bit(PWMF_REQUESTED, &pwm->flags)) {
			ret = -EBUSY;
			goto out;
		}
	}

	list_del_init(&chip->list);

	if (IS_ENABLED(CONFIG_OF))
		of_pwmchip_remove(chip);

	free_pwms(chip);

out:
	mutex_unlock(&pwm_lock);
	return ret;
}
EXPORT_SYMBOL_GPL(pwmchip_remove);

/**
 * pwm_request() - request a PWM device
 * @pwm: global PWM device index
 * @label: PWM device label
 *
 * This function is deprecated, use pwm_get() instead.
 *
 * Returns: A pointer to a PWM device or an ERR_PTR()-encoded error code on
 * failure.
 */
struct pwm_device *pwm_request(int pwm, const char *label)
{
	struct pwm_device *dev;
	int err;

	if (pwm < 0 || pwm >= MAX_PWMS)
		return ERR_PTR(-EINVAL);

	mutex_lock(&pwm_lock);

	dev = pwm_to_device(pwm);
	if (!dev) {
		dev = ERR_PTR(-EPROBE_DEFER);
		goto out;
	}

	err = pwm_device_request(dev, label);
	if (err < 0)
		dev = ERR_PTR(err);

out:
	mutex_unlock(&pwm_lock);

	return dev;
}
EXPORT_SYMBOL_GPL(pwm_request);

/**
 * pwm_request_from_chip() - request a PWM device relative to a PWM chip
 * @chip: PWM chip
 * @index: per-chip index of the PWM to request
 * @label: a literal description string of this PWM
 *
 * Returns: A pointer to the PWM device at the given index of the given PWM
 * chip. A negative error code is returned if the index is not valid for the
 * specified PWM chip or if the PWM device cannot be requested.
 */
struct pwm_device *pwm_request_from_chip(struct pwm_chip *chip,
					 unsigned int index,
					 const char *label)
{
	struct pwm_device *pwm;
	int err;

	if (!chip || index >= chip->npwm)
		return ERR_PTR(-EINVAL);

	mutex_lock(&pwm_lock);
	pwm = &chip->pwms[index];

	err = pwm_device_request(pwm, label);
	if (err < 0)
		pwm = ERR_PTR(err);

	mutex_unlock(&pwm_lock);
	return pwm;
}
EXPORT_SYMBOL_GPL(pwm_request_from_chip);

/**
 * pwm_free() - free a PWM device
 * @pwm: PWM device
 *
 * This function is deprecated, use pwm_put() instead.
 */
void pwm_free(struct pwm_device *pwm)
{
	pwm_put(pwm);
}
EXPORT_SYMBOL_GPL(pwm_free);

static void pwm_apply_state_debug(struct pwm_device *pwm,
				  const struct pwm_state *state)
{
	struct pwm_state *last = &pwm->last;
	struct pwm_chip *chip = pwm->chip;
	struct pwm_state s1, s2;
	int err;

	if (!IS_ENABLED(CONFIG_PWM_DEBUG))
		return;

	/* No reasonable diagnosis possible without .get_state() */
	if (!chip->ops->get_state)
		return;

	/*
	 * *state was just applied. Read out the hardware state and do some
	 * checks.
	 */

	chip->ops->get_state(chip, pwm, &s1);
	trace_pwm_get(pwm, &s1);

	/*
	 * The lowlevel driver either ignored .polarity (which is a bug) or as
	 * best effort inverted .polarity and fixed .duty_cycle respectively.
	 * Undo this inversion and fixup for further tests.
	 */
	if (s1.enabled && s1.polarity != state->polarity) {
		s2.polarity = state->polarity;
		s2.duty_cycle = s1.period - s1.duty_cycle;
		s2.period = s1.period;
		s2.enabled = s1.enabled;
	} else {
		s2 = s1;
	}

	if (s2.polarity != state->polarity &&
	    state->duty_cycle < state->period)
		dev_warn(chip->dev, ".apply ignored .polarity\n");

	if (state->enabled &&
	    last->polarity == state->polarity &&
	    last->period > s2.period &&
	    last->period <= state->period)
		dev_warn(chip->dev,
			 ".apply didn't pick the best available period (requested: %llu, applied: %llu, possible: %llu)\n",
			 state->period, s2.period, last->period);

	if (state->enabled && state->period < s2.period)
		dev_warn(chip->dev,
			 ".apply is supposed to round down period (requested: %llu, applied: %llu)\n",
			 state->period, s2.period);

	if (state->enabled &&
	    last->polarity == state->polarity &&
	    last->period == s2.period &&
	    last->duty_cycle > s2.duty_cycle &&
	    last->duty_cycle <= state->duty_cycle)
		dev_warn(chip->dev,
			 ".apply didn't pick the best available duty cycle (requested: %llu/%llu, applied: %llu/%llu, possible: %llu/%llu)\n",
			 state->duty_cycle, state->period,
			 s2.duty_cycle, s2.period,
			 last->duty_cycle, last->period);

	if (state->enabled && state->duty_cycle < s2.duty_cycle)
		dev_warn(chip->dev,
			 ".apply is supposed to round down duty_cycle (requested: %llu/%llu, applied: %llu/%llu)\n",
			 state->duty_cycle, state->period,
			 s2.duty_cycle, s2.period);

	if (!state->enabled && s2.enabled && s2.duty_cycle > 0)
		dev_warn(chip->dev,
			 "requested disabled, but yielded enabled with duty > 0\n");

	/* reapply the state that the driver reported being configured. */
	err = chip->ops->apply(chip, pwm, &s1);
	if (err) {
		*last = s1;
		dev_err(chip->dev, "failed to reapply current setting\n");
		return;
	}

	trace_pwm_apply(pwm, &s1);

	chip->ops->get_state(chip, pwm, last);
	trace_pwm_get(pwm, last);

	/* reapplication of the current state should give an exact match */
	if (s1.enabled != last->enabled ||
	    s1.polarity != last->polarity ||
	    (s1.enabled && s1.period != last->period) ||
	    (s1.enabled && s1.duty_cycle != last->duty_cycle)) {
		dev_err(chip->dev,
			".apply is not idempotent (ena=%d pol=%d %llu/%llu) -> (ena=%d pol=%d %llu/%llu)\n",
			s1.enabled, s1.polarity, s1.duty_cycle, s1.period,
			last->enabled, last->polarity, last->duty_cycle,
			last->period);
	}
}

/**
 * pwm_apply_state() - atomically apply a new state to a PWM device
 * @pwm: PWM device
 * @state: new state to apply
 */
int pwm_apply_state(struct pwm_device *pwm, const struct pwm_state *state)
{
	struct pwm_chip *chip;
	int err;

	if (!pwm || !state || !state->period ||
	    state->duty_cycle > state->period)
		return -EINVAL;

	chip = pwm->chip;

	if (state->period == pwm->state.period &&
	    state->duty_cycle == pwm->state.duty_cycle &&
	    state->polarity == pwm->state.polarity &&
	    state->enabled == pwm->state.enabled)
		return 0;

	if (chip->ops->apply) {
		err = chip->ops->apply(chip, pwm, state);
		if (err)
			return err;

		trace_pwm_apply(pwm, state);

		pwm->state = *state;

		/*
		 * only do this after pwm->state was applied as some
		 * implementations of .get_state depend on this
		 */
		pwm_apply_state_debug(pwm, state);
	} else {
		/*
		 * FIXME: restore the initial state in case of error.
		 */
		if (state->polarity != pwm->state.polarity) {
			if (!chip->ops->set_polarity)
				return -EINVAL;

			/*
			 * Changing the polarity of a running PWM is
			 * only allowed when the PWM driver implements
			 * ->apply().
			 */
			if (pwm->state.enabled) {
				chip->ops->disable(chip, pwm);
				pwm->state.enabled = false;
			}

			err = chip->ops->set_polarity(chip, pwm,
						      state->polarity);
			if (err)
				return err;

			pwm->state.polarity = state->polarity;
		}

		if (state->period != pwm->state.period ||
		    state->duty_cycle != pwm->state.duty_cycle) {
			err = chip->ops->config(pwm->chip, pwm,
						state->duty_cycle,
						state->period);
			if (err)
				return err;

			pwm->state.duty_cycle = state->duty_cycle;
			pwm->state.period = state->period;
		}

		if (state->enabled != pwm->state.enabled) {
			if (state->enabled) {
				err = chip->ops->enable(chip, pwm);
				if (err)
					return err;
			} else {
				chip->ops->disable(chip, pwm);
			}

			pwm->state.enabled = state->enabled;
		}
	}

	return 0;
}
EXPORT_SYMBOL_GPL(pwm_apply_state);

/**
 * pwm_capture() - capture and report a PWM signal
 * @pwm: PWM device
 * @result: structure to fill with capture result
 * @timeout: time to wait, in milliseconds, before giving up on capture
 *
 * Returns: 0 on success or a negative error code on failure.
 */
int pwm_capture(struct pwm_device *pwm, struct pwm_capture *result,
		unsigned long timeout)
{
	int err;

	if (!pwm || !pwm->chip->ops)
		return -EINVAL;

	if (!pwm->chip->ops->capture)
		return -ENOSYS;

	mutex_lock(&pwm_lock);
	err = pwm->chip->ops->capture(pwm->chip, pwm, result, timeout);
	mutex_unlock(&pwm_lock);

	return err;
}
EXPORT_SYMBOL_GPL(pwm_capture);

/**
 * pwm_adjust_config() - adjust the current PWM config to the PWM arguments
 * @pwm: PWM device
 *
 * This function will adjust the PWM config to the PWM arguments provided
 * by the DT or PWM lookup table. This is particularly useful to adapt
 * the bootloader config to the Linux one.
 */
int pwm_adjust_config(struct pwm_device *pwm)
{
	struct pwm_state state;
	struct pwm_args pargs;

	pwm_get_args(pwm, &pargs);
	pwm_get_state(pwm, &state);

	/*
	 * If the current period is zero it means that either the PWM driver
	 * does not support initial state retrieval or the PWM has not yet
	 * been configured.
	 *
	 * In either case, we setup the new period and polarity, and assign a
	 * duty cycle of 0.
	 */
	if (!state.period) {
		state.duty_cycle = 0;
		state.period = pargs.period;
		state.polarity = pargs.polarity;

		return pwm_apply_state(pwm, &state);
	}

	/*
	 * Adjust the PWM duty cycle/period based on the period value provided
	 * in PWM args.
	 */
	if (pargs.period != state.period) {
		u64 dutycycle = (u64)state.duty_cycle * pargs.period;

		do_div(dutycycle, state.period);
		state.duty_cycle = dutycycle;
		state.period = pargs.period;
	}

	/*
	 * If the polarity changed, we should also change the duty cycle.
	 */
	if (pargs.polarity != state.polarity) {
		state.polarity = pargs.polarity;
		state.duty_cycle = state.period - state.duty_cycle;
	}

	return pwm_apply_state(pwm, &state);
}
EXPORT_SYMBOL_GPL(pwm_adjust_config);

static struct pwm_chip *of_node_to_pwmchip(struct device_node *np)
{
	struct pwm_chip *chip;

	mutex_lock(&pwm_lock);

	list_for_each_entry(chip, &pwm_chips, list)
		if (chip->dev && chip->dev->of_node == np) {
			mutex_unlock(&pwm_lock);
			return chip;
		}

	mutex_unlock(&pwm_lock);

	return ERR_PTR(-EPROBE_DEFER);
}

static struct device_link *pwm_device_link_add(struct device *dev,
					       struct pwm_device *pwm)
{
	struct device_link *dl;

	if (!dev) {
		/*
		 * No device for the PWM consumer has been provided. It may
		 * impact the PM sequence ordering: the PWM supplier may get
		 * suspended before the consumer.
		 */
		dev_warn(pwm->chip->dev,
			 "No consumer device specified to create a link to\n");
		return NULL;
	}

	dl = device_link_add(dev, pwm->chip->dev, DL_FLAG_AUTOREMOVE_CONSUMER);
	if (!dl) {
		dev_err(dev, "failed to create device link to %s\n",
			dev_name(pwm->chip->dev));
		return ERR_PTR(-EINVAL);
	}

	return dl;
}

/**
 * of_pwm_get() - request a PWM via the PWM framework
 * @dev: device for PWM consumer
 * @np: device node to get the PWM from
 * @con_id: consumer name
 *
 * Returns the PWM device parsed from the phandle and index specified in the
 * "pwms" property of a device tree node or a negative error-code on failure.
 * Values parsed from the device tree are stored in the returned PWM device
 * object.
 *
 * If con_id is NULL, the first PWM device listed in the "pwms" property will
 * be requested. Otherwise the "pwm-names" property is used to do a reverse
 * lookup of the PWM index. This also means that the "pwm-names" property
 * becomes mandatory for devices that look up the PWM device via the con_id
 * parameter.
 *
 * Returns: A pointer to the requested PWM device or an ERR_PTR()-encoded
 * error code on failure.
 */
struct pwm_device *of_pwm_get(struct device *dev, struct device_node *np,
			      const char *con_id)
{
	struct pwm_device *pwm = NULL;
	struct of_phandle_args args;
	struct device_link *dl;
	struct pwm_chip *pc;
	int index = 0;
	int err;

	if (con_id) {
		index = of_property_match_string(np, "pwm-names", con_id);
		if (index < 0)
			return ERR_PTR(index);
	}

	err = of_parse_phandle_with_args(np, "pwms", "#pwm-cells", index,
					 &args);
	if (err) {
		pr_err("%s(): can't parse \"pwms\" property\n", __func__);
		return ERR_PTR(err);
	}

	pc = of_node_to_pwmchip(args.np);
	if (IS_ERR(pc)) {
		if (PTR_ERR(pc) != -EPROBE_DEFER)
			pr_err("%s(): PWM chip not found\n", __func__);

		pwm = ERR_CAST(pc);
		goto put;
	}

	pwm = pc->of_xlate(pc, &args);
	if (IS_ERR(pwm))
		goto put;

	dl = pwm_device_link_add(dev, pwm);
	if (IS_ERR(dl)) {
		/* of_xlate ended up calling pwm_request_from_chip() */
		pwm_free(pwm);
		pwm = ERR_CAST(dl);
		goto put;
	}

	/*
	 * If a consumer name was not given, try to look it up from the
	 * "pwm-names" property if it exists. Otherwise use the name of
	 * the user device node.
	 */
	if (!con_id) {
		err = of_property_read_string_index(np, "pwm-names", index,
						    &con_id);
		if (err < 0)
			con_id = np->name;
	}

	pwm->label = con_id;

put:
	of_node_put(args.np);

	return pwm;
}
EXPORT_SYMBOL_GPL(of_pwm_get);

#if IS_ENABLED(CONFIG_ACPI)
static struct pwm_chip *device_to_pwmchip(struct device *dev)
{
	struct pwm_chip *chip;

	mutex_lock(&pwm_lock);

	list_for_each_entry(chip, &pwm_chips, list) {
		struct acpi_device *adev = ACPI_COMPANION(chip->dev);

		if ((chip->dev == dev) || (adev && &adev->dev == dev)) {
			mutex_unlock(&pwm_lock);
			return chip;
		}
	}

	mutex_unlock(&pwm_lock);

	return ERR_PTR(-EPROBE_DEFER);
}
#endif

/**
 * acpi_pwm_get() - request a PWM via parsing "pwms" property in ACPI
 * @fwnode: firmware node to get the "pwm" property from
 *
 * Returns the PWM device parsed from the fwnode and index specified in the
 * "pwms" property or a negative error-code on failure.
 * Values parsed from the device tree are stored in the returned PWM device
 * object.
 *
 * This is analogous to of_pwm_get() except con_id is not yet supported.
 * ACPI entries must look like
 * Package () {"pwms", Package ()
 *     { <PWM device reference>, <PWM index>, <PWM period> [, <PWM flags>]}}
 *
 * Returns: A pointer to the requested PWM device or an ERR_PTR()-encoded
 * error code on failure.
 */
static struct pwm_device *acpi_pwm_get(struct fwnode_handle *fwnode)
{
	struct pwm_device *pwm = ERR_PTR(-ENODEV);
#if IS_ENABLED(CONFIG_ACPI)
	struct fwnode_reference_args args;
	struct acpi_device *acpi;
	struct pwm_chip *chip;
	int ret;

	memset(&args, 0, sizeof(args));

	ret = __acpi_node_get_property_reference(fwnode, "pwms", 0, 3, &args);
	if (ret < 0)
		return ERR_PTR(ret);

	acpi = to_acpi_device_node(args.fwnode);
	if (!acpi)
		return ERR_PTR(-EINVAL);

	if (args.nargs < 2)
		return ERR_PTR(-EPROTO);

	chip = device_to_pwmchip(&acpi->dev);
	if (IS_ERR(chip))
		return ERR_CAST(chip);

	pwm = pwm_request_from_chip(chip, args.args[0], NULL);
	if (IS_ERR(pwm))
		return pwm;

	pwm->args.period = args.args[1];
	pwm->args.polarity = PWM_POLARITY_NORMAL;

	if (args.nargs > 2 && args.args[2] & PWM_POLARITY_INVERTED)
		pwm->args.polarity = PWM_POLARITY_INVERSED;
#endif

	return pwm;
}

/**
 * pwm_add_table() - register PWM device consumers
 * @table: array of consumers to register
 * @num: number of consumers in table
 */
void pwm_add_table(struct pwm_lookup *table, size_t num)
{
	mutex_lock(&pwm_lookup_lock);

	while (num--) {
		list_add_tail(&table->list, &pwm_lookup_list);
		table++;
	}

	mutex_unlock(&pwm_lookup_lock);
}

/**
 * pwm_remove_table() - unregister PWM device consumers
 * @table: array of consumers to unregister
 * @num: number of consumers in table
 */
void pwm_remove_table(struct pwm_lookup *table, size_t num)
{
	mutex_lock(&pwm_lookup_lock);

	while (num--) {
		list_del(&table->list);
		table++;
	}

	mutex_unlock(&pwm_lookup_lock);
}

/**
 * pwm_get() - look up and request a PWM device
 * @dev: device for PWM consumer
 * @con_id: consumer name
 *
 * Lookup is first attempted using DT. If the device was not instantiated from
 * a device tree, a PWM chip and a relative index is looked up via a table
 * supplied by board setup code (see pwm_add_table()).
 *
 * Once a PWM chip has been found the specified PWM device will be requested
 * and is ready to be used.
 *
 * Returns: A pointer to the requested PWM device or an ERR_PTR()-encoded
 * error code on failure.
 */
struct pwm_device *pwm_get(struct device *dev, const char *con_id)
{
	const char *dev_id = dev ? dev_name(dev) : NULL;
	struct pwm_device *pwm;
	struct pwm_chip *chip;
	struct device_link *dl;
	unsigned int best = 0;
	struct pwm_lookup *p, *chosen = NULL;
	unsigned int match;
	int err;

	/* look up via DT first */
	if (IS_ENABLED(CONFIG_OF) && dev && dev->of_node)
		return of_pwm_get(dev, dev->of_node, con_id);

	/* then lookup via ACPI */
	if (dev && is_acpi_node(dev->fwnode)) {
		pwm = acpi_pwm_get(dev->fwnode);
		if (!IS_ERR(pwm) || PTR_ERR(pwm) != -ENOENT)
			return pwm;
	}

	/*
	 * We look up the provider in the static table typically provided by
	 * board setup code. We first try to lookup the consumer device by
	 * name. If the consumer device was passed in as NULL or if no match
	 * was found, we try to find the consumer by directly looking it up
	 * by name.
	 *
	 * If a match is found, the provider PWM chip is looked up by name
	 * and a PWM device is requested using the PWM device per-chip index.
	 *
	 * The lookup algorithm was shamelessly taken from the clock
	 * framework:
	 *
	 * We do slightly fuzzy matching here:
	 *  An entry with a NULL ID is assumed to be a wildcard.
	 *  If an entry has a device ID, it must match
	 *  If an entry has a connection ID, it must match
	 * Then we take the most specific entry - with the following order
	 * of precedence: dev+con > dev only > con only.
	 */
	mutex_lock(&pwm_lookup_lock);

	list_for_each_entry(p, &pwm_lookup_list, list) {
		match = 0;

		if (p->dev_id) {
			if (!dev_id || strcmp(p->dev_id, dev_id))
				continue;

			match += 2;
		}

		if (p->con_id) {
			if (!con_id || strcmp(p->con_id, con_id))
				continue;

			match += 1;
		}

		if (match > best) {
			chosen = p;

			if (match != 3)
				best = match;
			else
				break;
		}
	}

	mutex_unlock(&pwm_lookup_lock);

	if (!chosen)
		return ERR_PTR(-ENODEV);

	chip = pwmchip_find_by_name(chosen->provider);

	/*
	 * If the lookup entry specifies a module, load the module and retry
	 * the PWM chip lookup. This can be used to work around driver load
	 * ordering issues if driver's can't be made to properly support the
	 * deferred probe mechanism.
	 */
	if (!chip && chosen->module) {
		err = request_module(chosen->module);
		if (err == 0)
			chip = pwmchip_find_by_name(chosen->provider);
	}

	if (!chip)
		return ERR_PTR(-EPROBE_DEFER);

	pwm = pwm_request_from_chip(chip, chosen->index, con_id ?: dev_id);
	if (IS_ERR(pwm))
		return pwm;

	dl = pwm_device_link_add(dev, pwm);
	if (IS_ERR(dl)) {
		pwm_free(pwm);
		return ERR_CAST(dl);
	}

	pwm->args.period = chosen->period;
	pwm->args.polarity = chosen->polarity;

	return pwm;
}
EXPORT_SYMBOL_GPL(pwm_get);

/**
 * pwm_put() - release a PWM device
 * @pwm: PWM device
 */
void pwm_put(struct pwm_device *pwm)
{
	if (!pwm)
		return;

	mutex_lock(&pwm_lock);

	if (!test_and_clear_bit(PWMF_REQUESTED, &pwm->flags)) {
		pr_warn("PWM device already freed\n");
		goto out;
	}

	if (pwm->chip->ops->free)
		pwm->chip->ops->free(pwm->chip, pwm);

	pwm_set_chip_data(pwm, NULL);
	pwm->label = NULL;

	module_put(pwm->chip->ops->owner);
out:
	mutex_unlock(&pwm_lock);
}
EXPORT_SYMBOL_GPL(pwm_put);

static void devm_pwm_release(struct device *dev, void *res)
{
	pwm_put(*(struct pwm_device **)res);
}

/**
 * devm_pwm_get() - resource managed pwm_get()
 * @dev: device for PWM consumer
 * @con_id: consumer name
 *
 * This function performs like pwm_get() but the acquired PWM device will
 * automatically be released on driver detach.
 *
 * Returns: A pointer to the requested PWM device or an ERR_PTR()-encoded
 * error code on failure.
 */
struct pwm_device *devm_pwm_get(struct device *dev, const char *con_id)
{
	struct pwm_device **ptr, *pwm;

	ptr = devres_alloc(devm_pwm_release, sizeof(*ptr), GFP_KERNEL);
	if (!ptr)
		return ERR_PTR(-ENOMEM);

	pwm = pwm_get(dev, con_id);
	if (!IS_ERR(pwm)) {
		*ptr = pwm;
		devres_add(dev, ptr);
	} else {
		devres_free(ptr);
	}

	return pwm;
}
EXPORT_SYMBOL_GPL(devm_pwm_get);

/**
 * devm_of_pwm_get() - resource managed of_pwm_get()
 * @dev: device for PWM consumer
 * @np: device node to get the PWM from
 * @con_id: consumer name
 *
 * This function performs like of_pwm_get() but the acquired PWM device will
 * automatically be released on driver detach.
 *
 * Returns: A pointer to the requested PWM device or an ERR_PTR()-encoded
 * error code on failure.
 */
struct pwm_device *devm_of_pwm_get(struct device *dev, struct device_node *np,
				   const char *con_id)
{
	struct pwm_device **ptr, *pwm;

	ptr = devres_alloc(devm_pwm_release, sizeof(*ptr), GFP_KERNEL);
	if (!ptr)
		return ERR_PTR(-ENOMEM);

	pwm = of_pwm_get(dev, np, con_id);
	if (!IS_ERR(pwm)) {
		*ptr = pwm;
		devres_add(dev, ptr);
	} else {
		devres_free(ptr);
	}

	return pwm;
}
EXPORT_SYMBOL_GPL(devm_of_pwm_get);

/**
 * devm_fwnode_pwm_get() - request a resource managed PWM from firmware node
 * @dev: device for PWM consumer
 * @fwnode: firmware node to get the PWM from
 * @con_id: consumer name
 *
 * Returns the PWM device parsed from the firmware node. See of_pwm_get() and
 * acpi_pwm_get() for a detailed description.
 *
 * Returns: A pointer to the requested PWM device or an ERR_PTR()-encoded
 * error code on failure.
 */
struct pwm_device *devm_fwnode_pwm_get(struct device *dev,
				       struct fwnode_handle *fwnode,
				       const char *con_id)
{
	struct pwm_device **ptr, *pwm = ERR_PTR(-ENODEV);

	ptr = devres_alloc(devm_pwm_release, sizeof(*ptr), GFP_KERNEL);
	if (!ptr)
		return ERR_PTR(-ENOMEM);

	if (is_of_node(fwnode))
		pwm = of_pwm_get(dev, to_of_node(fwnode), con_id);
	else if (is_acpi_node(fwnode))
		pwm = acpi_pwm_get(fwnode);

	if (!IS_ERR(pwm)) {
		*ptr = pwm;
		devres_add(dev, ptr);
	} else {
		devres_free(ptr);
	}

	return pwm;
}
EXPORT_SYMBOL_GPL(devm_fwnode_pwm_get);

static int devm_pwm_match(struct device *dev, void *res, void *data)
{
	struct pwm_device **p = res;

	if (WARN_ON(!p || !*p))
		return 0;

	return *p == data;
}

/**
 * devm_pwm_put() - resource managed pwm_put()
 * @dev: device for PWM consumer
 * @pwm: PWM device
 *
 * Release a PWM previously allocated using devm_pwm_get(). Calling this
 * function is usually not needed because devm-allocated resources are
 * automatically released on driver detach.
 */
void devm_pwm_put(struct device *dev, struct pwm_device *pwm)
{
	WARN_ON(devres_release(dev, devm_pwm_release, devm_pwm_match, pwm));
}
EXPORT_SYMBOL_GPL(devm_pwm_put);

#ifdef CONFIG_DEBUG_FS
static void pwm_dbg_show(struct pwm_chip *chip, struct seq_file *s)
{
	unsigned int i;

	for (i = 0; i < chip->npwm; i++) {
		struct pwm_device *pwm = &chip->pwms[i];
		struct pwm_state state;

		pwm_get_state(pwm, &state);

		seq_printf(s, " pwm-%-3d (%-20.20s):", i, pwm->label);

		if (test_bit(PWMF_REQUESTED, &pwm->flags))
			seq_puts(s, " requested");

		if (state.enabled)
			seq_puts(s, " enabled");

		seq_printf(s, " period: %llu ns", state.period);
		seq_printf(s, " duty: %llu ns", state.duty_cycle);
		seq_printf(s, " polarity: %s",
			   state.polarity ? "inverse" : "normal");

		seq_puts(s, "\n");
	}
}

static void *pwm_seq_start(struct seq_file *s, loff_t *pos)
{
	mutex_lock(&pwm_lock);
	s->private = "";

	return seq_list_start(&pwm_chips, *pos);
}

static void *pwm_seq_next(struct seq_file *s, void *v, loff_t *pos)
{
	s->private = "\n";

	return seq_list_next(v, &pwm_chips, pos);
}

static void pwm_seq_stop(struct seq_file *s, void *v)
{
	mutex_unlock(&pwm_lock);
}

static int pwm_seq_show(struct seq_file *s, void *v)
{
	struct pwm_chip *chip = list_entry(v, struct pwm_chip, list);

	seq_printf(s, "%s%s/%s, %d PWM device%s\n", (char *)s->private,
		   chip->dev->bus ? chip->dev->bus->name : "no-bus",
		   dev_name(chip->dev), chip->npwm,
		   (chip->npwm != 1) ? "s" : "");

	pwm_dbg_show(chip, s);

	return 0;
}

static const struct seq_operations pwm_debugfs_sops = {
	.start = pwm_seq_start,
	.next = pwm_seq_next,
	.stop = pwm_seq_stop,
	.show = pwm_seq_show,
};

DEFINE_SEQ_ATTRIBUTE(pwm_debugfs);

static int __init pwm_debugfs_init(void)
{
	debugfs_create_file("pwm", S_IFREG | 0444, NULL, NULL,
			    &pwm_debugfs_fops);

	return 0;
}
subsys_initcall(pwm_debugfs_init);
#endif /* CONFIG_DEBUG_FS */
