// SPDX-License-Identifier: GPL-2.0-only
/*
 * drivers/acpi/device_pm.c - ACPI device power management routines.
 *
 * Copyright (C) 2012, Intel Corp.
 * Author: Rafael J. Wysocki <rafael.j.wysocki@intel.com>
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

#define pr_fmt(fmt) "ACPI: PM: " fmt

#include <linux/acpi.h>
#include <linux/export.h>
#include <linux/mutex.h>
#include <linux/pm_qos.h>
#include <linux/pm_domain.h>
#include <linux/pm_runtime.h>
#include <linux/suspend.h>

#include "internal.h"

/**
 * acpi_power_state_string - String representation of ACPI device power state.
 * @state: ACPI device power state to return the string representation of.
 */
const char *acpi_power_state_string(int state)
{
	switch (state) {
	case ACPI_STATE_D0:
		return "D0";
	case ACPI_STATE_D1:
		return "D1";
	case ACPI_STATE_D2:
		return "D2";
	case ACPI_STATE_D3_HOT:
		return "D3hot";
	case ACPI_STATE_D3_COLD:
		return "D3cold";
	default:
		return "(unknown)";
	}
}

static int acpi_dev_pm_explicit_get(struct acpi_device *device, int *state)
{
	unsigned long long psc;
	acpi_status status;

	status = acpi_evaluate_integer(device->handle, "_PSC", NULL, &psc);
	if (ACPI_FAILURE(status))
		return -ENODEV;

	*state = psc;
	return 0;
}

/**
 * acpi_device_get_power - Get power state of an ACPI device.
 * @device: Device to get the power state of.
 * @state: Place to store the power state of the device.
 *
 * This function does not update the device's power.state field, but it may
 * update its parent's power.state field (when the parent's power state is
 * unknown and the device's power state turns out to be D0).
 *
 * Also, it does not update power resource reference counters to ensure that
 * the power state returned by it will be persistent and it may return a power
 * state shallower than previously set by acpi_device_set_power() for @device
 * (if that power state depends on any power resources).
 */
int acpi_device_get_power(struct acpi_device *device, int *state)
{
	int result = ACPI_STATE_UNKNOWN;
	int error;

	if (!device || !state)
		return -EINVAL;

	if (!device->flags.power_manageable) {
		/* TBD: Non-recursive algorithm for walking up hierarchy. */
		*state = device->parent ?
			device->parent->power.state : ACPI_STATE_D0;
		goto out;
	}

	/*
	 * Get the device's power state from power resources settings and _PSC,
	 * if available.
	 */
	if (device->power.flags.power_resources) {
		error = acpi_power_get_inferred_state(device, &result);
		if (error)
			return error;
	}
	if (device->power.flags.explicit_get) {
		int psc;

		error = acpi_dev_pm_explicit_get(device, &psc);
		if (error)
			return error;

		/*
		 * The power resources settings may indicate a power state
		 * shallower than the actual power state of the device, because
		 * the same power resources may be referenced by other devices.
		 *
		 * For systems predating ACPI 4.0 we assume that D3hot is the
		 * deepest state that can be supported.
		 */
		if (psc > result && psc < ACPI_STATE_D3_COLD)
			result = psc;
		else if (result == ACPI_STATE_UNKNOWN)
			result = psc > ACPI_STATE_D2 ? ACPI_STATE_D3_HOT : psc;
	}

	/*
	 * If we were unsure about the device parent's power state up to this
	 * point, the fact that the device is in D0 implies that the parent has
	 * to be in D0 too, except if ignore_parent is set.
	 */
	if (!device->power.flags.ignore_parent && device->parent
	    && device->parent->power.state == ACPI_STATE_UNKNOWN
	    && result == ACPI_STATE_D0)
		device->parent->power.state = ACPI_STATE_D0;

	*state = result;

 out:
	dev_dbg(&device->dev, "Device power state is %s\n",
		acpi_power_state_string(*state));

	return 0;
}

static int acpi_dev_pm_explicit_set(struct acpi_device *adev, int state)
{
	if (adev->power.states[state].flags.explicit_set) {
		char method[5] = { '_', 'P', 'S', '0' + state, '\0' };
		acpi_status status;

		status = acpi_evaluate_object(adev->handle, method, NULL, NULL);
		if (ACPI_FAILURE(status))
			return -ENODEV;
	}
	return 0;
}

/**
 * acpi_device_set_power - Set power state of an ACPI device.
 * @device: Device to set the power state of.
 * @state: New power state to set.
 *
 * Callers must ensure that the device is power manageable before using this
 * function.
 */
int acpi_device_set_power(struct acpi_device *device, int state)
{
	int target_state = state;
	int result = 0;

	if (!device || !device->flags.power_manageable
	    || (state < ACPI_STATE_D0) || (state > ACPI_STATE_D3_COLD))
		return -EINVAL;

	acpi_handle_debug(device->handle, "Power state change: %s -> %s\n",
			  acpi_power_state_string(device->power.state),
			  acpi_power_state_string(state));

	/* Make sure this is a valid target state */

	/* There is a special case for D0 addressed below. */
	if (state > ACPI_STATE_D0 && state == device->power.state) {
		dev_dbg(&device->dev, "Device already in %s\n",
			acpi_power_state_string(state));
		return 0;
	}

	if (state == ACPI_STATE_D3_COLD) {
		/*
		 * For transitions to D3cold we need to execute _PS3 and then
		 * possibly drop references to the power resources in use.
		 */
		state = ACPI_STATE_D3_HOT;
		/* If D3cold is not supported, use D3hot as the target state. */
		if (!device->power.states[ACPI_STATE_D3_COLD].flags.valid)
			target_state = state;
	} else if (!device->power.states[state].flags.valid) {
		dev_warn(&device->dev, "Power state %s not supported\n",
			 acpi_power_state_string(state));
		return -ENODEV;
	}

	if (!device->power.flags.ignore_parent &&
	    device->parent && (state < device->parent->power.state)) {
		dev_warn(&device->dev,
			 "Cannot transition to power state %s for parent in %s\n",
			 acpi_power_state_string(state),
			 acpi_power_state_string(device->parent->power.state));
		return -ENODEV;
	}

	/*
	 * Transition Power
	 * ----------------
	 * In accordance with ACPI 6, _PSx is executed before manipulating power
	 * resources, unless the target state is D0, in which case _PS0 is
	 * supposed to be executed after turning the power resources on.
	 */
	if (state > ACPI_STATE_D0) {
		/*
		 * According to ACPI 6, devices cannot go from lower-power
		 * (deeper) states to higher-power (shallower) states.
		 */
		if (state < device->power.state) {
			dev_warn(&device->dev, "Cannot transition from %s to %s\n",
				 acpi_power_state_string(device->power.state),
				 acpi_power_state_string(state));
			return -ENODEV;
		}

		/*
		 * If the device goes from D3hot to D3cold, _PS3 has been
		 * evaluated for it already, so skip it in that case.
		 */
		if (device->power.state < ACPI_STATE_D3_HOT) {
			result = acpi_dev_pm_explicit_set(device, state);
			if (result)
				goto end;
		}

		if (device->power.flags.power_resources)
			result = acpi_power_transition(device, target_state);
	} else {
		int cur_state = device->power.state;

		if (device->power.flags.power_resources) {
			result = acpi_power_transition(device, ACPI_STATE_D0);
			if (result)
				goto end;
		}

		if (cur_state == ACPI_STATE_D0) {
			int psc;

			/* Nothing to do here if _PSC is not present. */
			if (!device->power.flags.explicit_get)
				return 0;

			/*
			 * The power state of the device was set to D0 last
			 * time, but that might have happened before a
			 * system-wide transition involving the platform
			 * firmware, so it may be necessary to evaluate _PS0
			 * for the device here.  However, use extra care here
			 * and evaluate _PSC to check the device's current power
			 * state, and only invoke _PS0 if the evaluation of _PSC
			 * is successful and it returns a power state different
			 * from D0.
			 */
			result = acpi_dev_pm_explicit_get(device, &psc);
			if (result || psc == ACPI_STATE_D0)
				return 0;
		}

		result = acpi_dev_pm_explicit_set(device, ACPI_STATE_D0);
	}

 end:
	if (result) {
		dev_warn(&device->dev, "Failed to change power state to %s\n",
			 acpi_power_state_string(target_state));
	} else {
		device->power.state = target_state;
		dev_dbg(&device->dev, "Power state changed to %s\n",
			acpi_power_state_string(target_state));
	}

	return result;
}
EXPORT_SYMBOL(acpi_device_set_power);

int acpi_bus_set_power(acpi_handle handle, int state)
{
	struct acpi_device *device;
	int result;

	result = acpi_bus_get_device(handle, &device);
	if (result)
		return result;

	return acpi_device_set_power(device, state);
}
EXPORT_SYMBOL(acpi_bus_set_power);

int acpi_bus_init_power(struct acpi_device *device)
{
	int state;
	int result;

	if (!device)
		return -EINVAL;

	device->power.state = ACPI_STATE_UNKNOWN;
	if (!acpi_device_is_present(device)) {
		device->flags.initialized = false;
		return -ENXIO;
	}

	result = acpi_device_get_power(device, &state);
	if (result)
		return result;

	if (state < ACPI_STATE_D3_COLD && device->power.flags.power_resources) {
		/* Reference count the power resources. */
		result = acpi_power_on_resources(device, state);
		if (result)
			return result;

		if (state == ACPI_STATE_D0) {
			/*
			 * If _PSC is not present and the state inferred from
			 * power resources appears to be D0, it still may be
			 * necessary to execute _PS0 at this point, because
			 * another device using the same power resources may
			 * have been put into D0 previously and that's why we
			 * see D0 here.
			 */
			result = acpi_dev_pm_explicit_set(device, state);
			if (result)
				return result;
		}
	} else if (state == ACPI_STATE_UNKNOWN) {
		/*
		 * No power resources and missing _PSC?  Cross fingers and make
		 * it D0 in hope that this is what the BIOS put the device into.
		 * [We tried to force D0 here by executing _PS0, but that broke
		 * Toshiba P870-303 in a nasty way.]
		 */
		state = ACPI_STATE_D0;
	}
	device->power.state = state;
	return 0;
}

/**
 * acpi_device_fix_up_power - Force device with missing _PSC into D0.
 * @device: Device object whose power state is to be fixed up.
 *
 * Devices without power resources and _PSC, but having _PS0 and _PS3 defined,
 * are assumed to be put into D0 by the BIOS.  However, in some cases that may
 * not be the case and this function should be used then.
 */
int acpi_device_fix_up_power(struct acpi_device *device)
{
	int ret = 0;

	if (!device->power.flags.power_resources
	    && !device->power.flags.explicit_get
	    && device->power.state == ACPI_STATE_D0)
		ret = acpi_dev_pm_explicit_set(device, ACPI_STATE_D0);

	return ret;
}
EXPORT_SYMBOL_GPL(acpi_device_fix_up_power);

int acpi_device_update_power(struct acpi_device *device, int *state_p)
{
	int state;
	int result;

	if (device->power.state == ACPI_STATE_UNKNOWN) {
		result = acpi_bus_init_power(device);
		if (!result && state_p)
			*state_p = device->power.state;

		return result;
	}

	result = acpi_device_get_power(device, &state);
	if (result)
		return result;

	if (state == ACPI_STATE_UNKNOWN) {
		state = ACPI_STATE_D0;
		result = acpi_device_set_power(device, state);
		if (result)
			return result;
	} else {
		if (device->power.flags.power_resources) {
			/*
			 * We don't need to really switch the state, bu we need
			 * to update the power resources' reference counters.
			 */
			result = acpi_power_transition(device, state);
			if (result)
				return result;
		}
		device->power.state = state;
	}
	if (state_p)
		*state_p = state;

	return 0;
}
EXPORT_SYMBOL_GPL(acpi_device_update_power);

int acpi_bus_update_power(acpi_handle handle, int *state_p)
{
	struct acpi_device *device;
	int result;

	result = acpi_bus_get_device(handle, &device);
	return result ? result : acpi_device_update_power(device, state_p);
}
EXPORT_SYMBOL_GPL(acpi_bus_update_power);

bool acpi_bus_power_manageable(acpi_handle handle)
{
	struct acpi_device *device;
	int result;

	result = acpi_bus_get_device(handle, &device);
	return result ? false : device->flags.power_manageable;
}
EXPORT_SYMBOL(acpi_bus_power_manageable);

#ifdef CONFIG_PM
static DEFINE_MUTEX(acpi_pm_notifier_lock);
static DEFINE_MUTEX(acpi_pm_notifier_install_lock);

void acpi_pm_wakeup_event(struct device *dev)
{
	pm_wakeup_dev_event(dev, 0, acpi_s2idle_wakeup());
}
EXPORT_SYMBOL_GPL(acpi_pm_wakeup_event);

static void acpi_pm_notify_handler(acpi_handle handle, u32 val, void *not_used)
{
	struct acpi_device *adev;

	if (val != ACPI_NOTIFY_DEVICE_WAKE)
		return;

	acpi_handle_debug(handle, "Wake notify\n");

	adev = acpi_bus_get_acpi_device(handle);
	if (!adev)
		return;

	mutex_lock(&acpi_pm_notifier_lock);

	if (adev->wakeup.flags.notifier_present) {
		pm_wakeup_ws_event(adev->wakeup.ws, 0, acpi_s2idle_wakeup());
		if (adev->wakeup.context.func) {
			acpi_handle_debug(handle, "Running %pS for %s\n",
					  adev->wakeup.context.func,
					  dev_name(adev->wakeup.context.dev));
			adev->wakeup.context.func(&adev->wakeup.context);
		}
	}

	mutex_unlock(&acpi_pm_notifier_lock);

	acpi_bus_put_acpi_device(adev);
}

/**
 * acpi_add_pm_notifier - Register PM notify handler for given ACPI device.
 * @adev: ACPI device to add the notify handler for.
 * @dev: Device to generate a wakeup event for while handling the notification.
 * @func: Work function to execute when handling the notification.
 *
 * NOTE: @adev need not be a run-wake or wakeup device to be a valid source of
 * PM wakeup events.  For example, wakeup events may be generated for bridges
 * if one of the devices below the bridge is signaling wakeup, even if the
 * bridge itself doesn't have a wakeup GPE associated with it.
 */
acpi_status acpi_add_pm_notifier(struct acpi_device *adev, struct device *dev,
			void (*func)(struct acpi_device_wakeup_context *context))
{
	acpi_status status = AE_ALREADY_EXISTS;

	if (!dev && !func)
		return AE_BAD_PARAMETER;

	mutex_lock(&acpi_pm_notifier_install_lock);

	if (adev->wakeup.flags.notifier_present)
		goto out;

	status = acpi_install_notify_handler(adev->handle, ACPI_SYSTEM_NOTIFY,
					     acpi_pm_notify_handler, NULL);
	if (ACPI_FAILURE(status))
		goto out;

	mutex_lock(&acpi_pm_notifier_lock);
	adev->wakeup.ws = wakeup_source_register(&adev->dev,
						 dev_name(&adev->dev));
	adev->wakeup.context.dev = dev;
	adev->wakeup.context.func = func;
	adev->wakeup.flags.notifier_present = true;
	mutex_unlock(&acpi_pm_notifier_lock);

 out:
	mutex_unlock(&acpi_pm_notifier_install_lock);
	return status;
}

/**
 * acpi_remove_pm_notifier - Unregister PM notifier from given ACPI device.
 * @adev: ACPI device to remove the notifier from.
 */
acpi_status acpi_remove_pm_notifier(struct acpi_device *adev)
{
	acpi_status status = AE_BAD_PARAMETER;

	mutex_lock(&acpi_pm_notifier_install_lock);

	if (!adev->wakeup.flags.notifier_present)
		goto out;

	status = acpi_remove_notify_handler(adev->handle,
					    ACPI_SYSTEM_NOTIFY,
					    acpi_pm_notify_handler);
	if (ACPI_FAILURE(status))
		goto out;

	mutex_lock(&acpi_pm_notifier_lock);
	adev->wakeup.context.func = NULL;
	adev->wakeup.context.dev = NULL;
	wakeup_source_unregister(adev->wakeup.ws);
	adev->wakeup.flags.notifier_present = false;
	mutex_unlock(&acpi_pm_notifier_lock);

 out:
	mutex_unlock(&acpi_pm_notifier_install_lock);
	return status;
}

bool acpi_bus_can_wakeup(acpi_handle handle)
{
	struct acpi_device *device;
	int result;

	result = acpi_bus_get_device(handle, &device);
	return result ? false : device->wakeup.flags.valid;
}
EXPORT_SYMBOL(acpi_bus_can_wakeup);

bool acpi_pm_device_can_wakeup(struct device *dev)
{
	struct acpi_device *adev = ACPI_COMPANION(dev);

	return adev ? acpi_device_can_wakeup(adev) : false;
}

/**
 * acpi_dev_pm_get_state - Get preferred power state of ACPI device.
 * @dev: Device whose preferred target power state to return.
 * @adev: ACPI device node corresponding to @dev.
 * @target_state: System state to match the resultant device state.
 * @d_min_p: Location to store the highest power state available to the device.
 * @d_max_p: Location to store the lowest power state available to the device.
 *
 * Find the lowest power (highest number) and highest power (lowest number) ACPI
 * device power states that the device can be in while the system is in the
 * state represented by @target_state.  Store the integer numbers representing
 * those stats in the memory locations pointed to by @d_max_p and @d_min_p,
 * respectively.
 *
 * Callers must ensure that @dev and @adev are valid pointers and that @adev
 * actually corresponds to @dev before using this function.
 *
 * Returns 0 on success or -ENODATA when one of the ACPI methods fails or
 * returns a value that doesn't make sense.  The memory locations pointed to by
 * @d_max_p and @d_min_p are only modified on success.
 */
static int acpi_dev_pm_get_state(struct device *dev, struct acpi_device *adev,
				 u32 target_state, int *d_min_p, int *d_max_p)
{
	char method[] = { '_', 'S', '0' + target_state, 'D', '\0' };
	acpi_handle handle = adev->handle;
	unsigned long long ret;
	int d_min, d_max;
	bool wakeup = false;
	bool has_sxd = false;
	acpi_status status;

	/*
	 * If the system state is S0, the lowest power state the device can be
	 * in is D3cold, unless the device has _S0W and is supposed to signal
	 * wakeup, in which case the return value of _S0W has to be used as the
	 * lowest power state available to the device.
	 */
	d_min = ACPI_STATE_D0;
	d_max = ACPI_STATE_D3_COLD;

	/*
	 * If present, _SxD methods return the minimum D-state (highest power
	 * state) we can use for the corresponding S-states.  Otherwise, the
	 * minimum D-state is D0 (ACPI 3.x).
	 */
	if (target_state > ACPI_STATE_S0) {
		/*
		 * We rely on acpi_evaluate_integer() not clobbering the integer
		 * provided if AE_NOT_FOUND is returned.
		 */
		ret = d_min;
		status = acpi_evaluate_integer(handle, method, NULL, &ret);
		if ((ACPI_FAILURE(status) && status != AE_NOT_FOUND)
		    || ret > ACPI_STATE_D3_COLD)
			return -ENODATA;

		/*
		 * We need to handle legacy systems where D3hot and D3cold are
		 * the same and 3 is returned in both cases, so fall back to
		 * D3cold if D3hot is not a valid state.
		 */
		if (!adev->power.states[ret].flags.valid) {
			if (ret == ACPI_STATE_D3_HOT)
				ret = ACPI_STATE_D3_COLD;
			else
				return -ENODATA;
		}

		if (status == AE_OK)
			has_sxd = true;

		d_min = ret;
		wakeup = device_may_wakeup(dev) && adev->wakeup.flags.valid
			&& adev->wakeup.sleep_state >= target_state;
	} else {
		wakeup = adev->wakeup.flags.valid;
	}

	/*
	 * If _PRW says we can wake up the system from the target sleep state,
	 * the D-state returned by _SxD is sufficient for that (we assume a
	 * wakeup-aware driver if wake is set).  Still, if _SxW exists
	 * (ACPI 3.x), it should return the maximum (lowest power) D-state that
	 * can wake the system.  _S0W may be valid, too.
	 */
	if (wakeup) {
		method[3] = 'W';
		status = acpi_evaluate_integer(handle, method, NULL, &ret);
		if (status == AE_NOT_FOUND) {
			/* No _SxW. In this case, the ACPI spec says that we
			 * must not go into any power state deeper than the
			 * value returned from _SxD.
			 */
			if (has_sxd && target_state > ACPI_STATE_S0)
				d_max = d_min;
		} else if (ACPI_SUCCESS(status) && ret <= ACPI_STATE_D3_COLD) {
			/* Fall back to D3cold if ret is not a valid state. */
			if (!adev->power.states[ret].flags.valid)
				ret = ACPI_STATE_D3_COLD;

			d_max = ret > d_min ? ret : d_min;
		} else {
			return -ENODATA;
		}
	}

	if (d_min_p)
		*d_min_p = d_min;

	if (d_max_p)
		*d_max_p = d_max;

	return 0;
}

/**
 * acpi_pm_device_sleep_state - Get preferred power state of ACPI device.
 * @dev: Device whose preferred target power state to return.
 * @d_min_p: Location to store the upper limit of the allowed states range.
 * @d_max_in: Deepest low-power state to take into consideration.
 * Return value: Preferred power state of the device on success, -ENODEV
 * if there's no 'struct acpi_device' for @dev, -EINVAL if @d_max_in is
 * incorrect, or -ENODATA on ACPI method failure.
 *
 * The caller must ensure that @dev is valid before using this function.
 */
int acpi_pm_device_sleep_state(struct device *dev, int *d_min_p, int d_max_in)
{
	struct acpi_device *adev;
	int ret, d_min, d_max;

	if (d_max_in < ACPI_STATE_D0 || d_max_in > ACPI_STATE_D3_COLD)
		return -EINVAL;

	if (d_max_in > ACPI_STATE_D2) {
		enum pm_qos_flags_status stat;

		stat = dev_pm_qos_flags(dev, PM_QOS_FLAG_NO_POWER_OFF);
		if (stat == PM_QOS_FLAGS_ALL)
			d_max_in = ACPI_STATE_D2;
	}

	adev = ACPI_COMPANION(dev);
	if (!adev) {
		dev_dbg(dev, "ACPI companion missing in %s!\n", __func__);
		return -ENODEV;
	}

	ret = acpi_dev_pm_get_state(dev, adev, acpi_target_system_state(),
				    &d_min, &d_max);
	if (ret)
		return ret;

	if (d_max_in < d_min)
		return -EINVAL;

	if (d_max > d_max_in) {
		for (d_max = d_max_in; d_max > d_min; d_max--) {
			if (adev->power.states[d_max].flags.valid)
				break;
		}
	}

	if (d_min_p)
		*d_min_p = d_min;

	return d_max;
}
EXPORT_SYMBOL(acpi_pm_device_sleep_state);

/**
 * acpi_pm_notify_work_func - ACPI devices wakeup notification work function.
 * @context: Device wakeup context.
 */
static void acpi_pm_notify_work_func(struct acpi_device_wakeup_context *context)
{
	struct device *dev = context->dev;

	if (dev) {
		pm_wakeup_event(dev, 0);
		pm_request_resume(dev);
	}
}

static DEFINE_MUTEX(acpi_wakeup_lock);

static int __acpi_device_wakeup_enable(struct acpi_device *adev,
				       u32 target_state)
{
	struct acpi_device_wakeup *wakeup = &adev->wakeup;
	acpi_status status;
	int error = 0;

	mutex_lock(&acpi_wakeup_lock);

	/*
	 * If the device wakeup power is already enabled, disable it and enable
	 * it again in case it depends on the configuration of subordinate
	 * devices and the conditions have changed since it was enabled last
	 * time.
	 */
	if (wakeup->enable_count > 0)
		acpi_disable_wakeup_device_power(adev);

	error = acpi_enable_wakeup_device_power(adev, target_state);
	if (error) {
		if (wakeup->enable_count > 0) {
			acpi_disable_gpe(wakeup->gpe_device, wakeup->gpe_number);
			wakeup->enable_count = 0;
		}
		goto out;
	}

	if (wakeup->enable_count > 0)
		goto inc;

	status = acpi_enable_gpe(wakeup->gpe_device, wakeup->gpe_number);
	if (ACPI_FAILURE(status)) {
		acpi_disable_wakeup_device_power(adev);
		error = -EIO;
		goto out;
	}

	acpi_handle_debug(adev->handle, "GPE%2X enabled for wakeup\n",
			  (unsigned int)wakeup->gpe_number);

inc:
	if (wakeup->enable_count < INT_MAX)
		wakeup->enable_count++;
	else
		acpi_handle_info(adev->handle, "Wakeup enable count out of bounds!\n");

out:
	mutex_unlock(&acpi_wakeup_lock);
	return error;
}

/**
 * acpi_device_wakeup_enable - Enable wakeup functionality for device.
 * @adev: ACPI device to enable wakeup functionality for.
 * @target_state: State the system is transitioning into.
 *
 * Enable the GPE associated with @adev so that it can generate wakeup signals
 * for the device in response to external (remote) events and enable wakeup
 * power for it.
 *
 * Callers must ensure that @adev is a valid ACPI device node before executing
 * this function.
 */
static int acpi_device_wakeup_enable(struct acpi_device *adev, u32 target_state)
{
	return __acpi_device_wakeup_enable(adev, target_state);
}

/**
 * acpi_device_wakeup_disable - Disable wakeup functionality for device.
 * @adev: ACPI device to disable wakeup functionality for.
 *
 * Disable the GPE associated with @adev and disable wakeup power for it.
 *
 * Callers must ensure that @adev is a valid ACPI device node before executing
 * this function.
 */
static void acpi_device_wakeup_disable(struct acpi_device *adev)
{
	struct acpi_device_wakeup *wakeup = &adev->wakeup;

	mutex_lock(&acpi_wakeup_lock);

	if (!wakeup->enable_count)
		goto out;

	acpi_disable_gpe(wakeup->gpe_device, wakeup->gpe_number);
	acpi_disable_wakeup_device_power(adev);

	wakeup->enable_count--;

out:
	mutex_unlock(&acpi_wakeup_lock);
}

/**
 * acpi_pm_set_device_wakeup - Enable/disable remote wakeup for given device.
 * @dev: Device to enable/disable to generate wakeup events.
 * @enable: Whether to enable or disable the wakeup functionality.
 */
int acpi_pm_set_device_wakeup(struct device *dev, bool enable)
{
	struct acpi_device *adev;
	int error;

	adev = ACPI_COMPANION(dev);
	if (!adev) {
		dev_dbg(dev, "ACPI companion missing in %s!\n", __func__);
		return -ENODEV;
	}

	if (!acpi_device_can_wakeup(adev))
		return -EINVAL;

	if (!enable) {
		acpi_device_wakeup_disable(adev);
		dev_dbg(dev, "Wakeup disabled by ACPI\n");
		return 0;
	}

	error = __acpi_device_wakeup_enable(adev, acpi_target_system_state());
	if (!error)
		dev_dbg(dev, "Wakeup enabled by ACPI\n");

	return error;
}
EXPORT_SYMBOL_GPL(acpi_pm_set_device_wakeup);

/**
 * acpi_dev_pm_low_power - Put ACPI device into a low-power state.
 * @dev: Device to put into a low-power state.
 * @adev: ACPI device node corresponding to @dev.
 * @system_state: System state to choose the device state for.
 */
static int acpi_dev_pm_low_power(struct device *dev, struct acpi_device *adev,
				 u32 system_state)
{
	int ret, state;

	if (!acpi_device_power_manageable(adev))
		return 0;

	ret = acpi_dev_pm_get_state(dev, adev, system_state, NULL, &state);
	return ret ? ret : acpi_device_set_power(adev, state);
}

/**
 * acpi_dev_pm_full_power - Put ACPI device into the full-power state.
 * @adev: ACPI device node to put into the full-power state.
 */
static int acpi_dev_pm_full_power(struct acpi_device *adev)
{
	return acpi_device_power_manageable(adev) ?
		acpi_device_set_power(adev, ACPI_STATE_D0) : 0;
}

/**
 * acpi_dev_suspend - Put device into a low-power state using ACPI.
 * @dev: Device to put into a low-power state.
 * @wakeup: Whether or not to enable wakeup for the device.
 *
 * Put the given device into a low-power state using the standard ACPI
 * mechanism.  Set up remote wakeup if desired, choose the state to put the
 * device into (this checks if remote wakeup is expected to work too), and set
 * the power state of the device.
 */
int acpi_dev_suspend(struct device *dev, bool wakeup)
{
	struct acpi_device *adev = ACPI_COMPANION(dev);
	u32 target_state = acpi_target_system_state();
	int error;

	if (!adev)
		return 0;

	if (wakeup && acpi_device_can_wakeup(adev)) {
		error = acpi_device_wakeup_enable(adev, target_state);
		if (error)
			return -EAGAIN;
	} else {
		wakeup = false;
	}

	error = acpi_dev_pm_low_power(dev, adev, target_state);
	if (error && wakeup)
		acpi_device_wakeup_disable(adev);

	return error;
}
EXPORT_SYMBOL_GPL(acpi_dev_suspend);

/**
 * acpi_dev_resume - Put device into the full-power state using ACPI.
 * @dev: Device to put into the full-power state.
 *
 * Put the given device into the full-power state using the standard ACPI
 * mechanism.  Set the power state of the device to ACPI D0 and disable wakeup.
 */
int acpi_dev_resume(struct device *dev)
{
	struct acpi_device *adev = ACPI_COMPANION(dev);
	int error;

	if (!adev)
		return 0;

	error = acpi_dev_pm_full_power(adev);
	acpi_device_wakeup_disable(adev);
	return error;
}
EXPORT_SYMBOL_GPL(acpi_dev_resume);

/**
 * acpi_subsys_runtime_suspend - Suspend device using ACPI.
 * @dev: Device to suspend.
 *
 * Carry out the generic runtime suspend procedure for @dev and use ACPI to put
 * it into a runtime low-power state.
 */
int acpi_subsys_runtime_suspend(struct device *dev)
{
	int ret = pm_generic_runtime_suspend(dev);

	return ret ? ret : acpi_dev_suspend(dev, true);
}
EXPORT_SYMBOL_GPL(acpi_subsys_runtime_suspend);

/**
 * acpi_subsys_runtime_resume - Resume device using ACPI.
 * @dev: Device to Resume.
 *
 * Use ACPI to put the given device into the full-power state and carry out the
 * generic runtime resume procedure for it.
 */
int acpi_subsys_runtime_resume(struct device *dev)
{
	int ret = acpi_dev_resume(dev);

	return ret ? ret : pm_generic_runtime_resume(dev);
}
EXPORT_SYMBOL_GPL(acpi_subsys_runtime_resume);

#ifdef CONFIG_PM_SLEEP
static bool acpi_dev_needs_resume(struct device *dev, struct acpi_device *adev)
{
	u32 sys_target = acpi_target_system_state();
	int ret, state;

	if (!pm_runtime_suspended(dev) || !adev || (adev->wakeup.flags.valid &&
	    device_may_wakeup(dev) != !!adev->wakeup.prepare_count))
		return true;

	if (sys_target == ACPI_STATE_S0)
		return false;

	if (adev->power.flags.dsw_present)
		return true;

	ret = acpi_dev_pm_get_state(dev, adev, sys_target, NULL, &state);
	if (ret)
		return true;

	return state != adev->power.state;
}

/**
 * acpi_subsys_prepare - Prepare device for system transition to a sleep state.
 * @dev: Device to prepare.
 */
int acpi_subsys_prepare(struct device *dev)
{
	struct acpi_device *adev = ACPI_COMPANION(dev);

	if (dev->driver && dev->driver->pm && dev->driver->pm->prepare) {
		int ret = dev->driver->pm->prepare(dev);

		if (ret < 0)
			return ret;

		if (!ret && dev_pm_test_driver_flags(dev, DPM_FLAG_SMART_PREPARE))
			return 0;
	}

	return !acpi_dev_needs_resume(dev, adev);
}
EXPORT_SYMBOL_GPL(acpi_subsys_prepare);

/**
 * acpi_subsys_complete - Finalize device's resume during system resume.
 * @dev: Device to handle.
 */
void acpi_subsys_complete(struct device *dev)
{
	pm_generic_complete(dev);
	/*
	 * If the device had been runtime-suspended before the system went into
	 * the sleep state it is going out of and it has never been resumed till
	 * now, resume it in case the firmware powered it up.
	 */
	if (pm_runtime_suspended(dev) && pm_resume_via_firmware())
		pm_request_resume(dev);
}
EXPORT_SYMBOL_GPL(acpi_subsys_complete);

/**
 * acpi_subsys_suspend - Run the device driver's suspend callback.
 * @dev: Device to handle.
 *
 * Follow PCI and resume devices from runtime suspend before running their
 * system suspend callbacks, unless the driver can cope with runtime-suspended
 * devices during system suspend and there are no ACPI-specific reasons for
 * resuming them.
 */
int acpi_subsys_suspend(struct device *dev)
{
	if (!dev_pm_test_driver_flags(dev, DPM_FLAG_SMART_SUSPEND) ||
	    acpi_dev_needs_resume(dev, ACPI_COMPANION(dev)))
		pm_runtime_resume(dev);

	return pm_generic_suspend(dev);
}
EXPORT_SYMBOL_GPL(acpi_subsys_suspend);

/**
 * acpi_subsys_suspend_late - Suspend device using ACPI.
 * @dev: Device to suspend.
 *
 * Carry out the generic late suspend procedure for @dev and use ACPI to put
 * it into a low-power state during system transition into a sleep state.
 */
int acpi_subsys_suspend_late(struct device *dev)
{
	int ret;

	if (dev_pm_skip_suspend(dev))
		return 0;

	ret = pm_generic_suspend_late(dev);
	return ret ? ret : acpi_dev_suspend(dev, device_may_wakeup(dev));
}
EXPORT_SYMBOL_GPL(acpi_subsys_suspend_late);

/**
 * acpi_subsys_suspend_noirq - Run the device driver's "noirq" suspend callback.
 * @dev: Device to suspend.
 */
int acpi_subsys_suspend_noirq(struct device *dev)
{
	int ret;

	if (dev_pm_skip_suspend(dev))
		return 0;

	ret = pm_generic_suspend_noirq(dev);
	if (ret)
		return ret;

	/*
	 * If the target system sleep state is suspend-to-idle, it is sufficient
	 * to check whether or not the device's wakeup settings are good for
	 * runtime PM.  Otherwise, the pm_resume_via_firmware() check will cause
	 * acpi_subsys_complete() to take care of fixing up the device's state
	 * anyway, if need be.
	 */
	if (device_can_wakeup(dev) && !device_may_wakeup(dev))
		dev->power.may_skip_resume = false;

	return 0;
}
EXPORT_SYMBOL_GPL(acpi_subsys_suspend_noirq);

/**
 * acpi_subsys_resume_noirq - Run the device driver's "noirq" resume callback.
 * @dev: Device to handle.
 */
static int acpi_subsys_resume_noirq(struct device *dev)
{
	if (dev_pm_skip_resume(dev))
		return 0;

	return pm_generic_resume_noirq(dev);
}

/**
 * acpi_subsys_resume_early - Resume device using ACPI.
 * @dev: Device to Resume.
 *
 * Use ACPI to put the given device into the full-power state and carry out the
 * generic early resume procedure for it during system transition into the
 * working state.
 */
static int acpi_subsys_resume_early(struct device *dev)
{
	int ret;

	if (dev_pm_skip_resume(dev))
		return 0;

	ret = acpi_dev_resume(dev);
	return ret ? ret : pm_generic_resume_early(dev);
}

/**
 * acpi_subsys_freeze - Run the device driver's freeze callback.
 * @dev: Device to handle.
 */
int acpi_subsys_freeze(struct device *dev)
{
	/*
	 * Resume all runtime-suspended devices before creating a snapshot
	 * image of system memory, because the restore kernel generally cannot
	 * be expected to always handle them consistently and they need to be
	 * put into the runtime-active metastate during system resume anyway,
	 * so it is better to ensure that the state saved in the image will be
	 * always consistent with that.
	 */
	pm_runtime_resume(dev);

	return pm_generic_freeze(dev);
}
EXPORT_SYMBOL_GPL(acpi_subsys_freeze);

/**
 * acpi_subsys_restore_early - Restore device using ACPI.
 * @dev: Device to restore.
 */
int acpi_subsys_restore_early(struct device *dev)
{
	int ret = acpi_dev_resume(dev);

	return ret ? ret : pm_generic_restore_early(dev);
}
EXPORT_SYMBOL_GPL(acpi_subsys_restore_early);

/**
 * acpi_subsys_poweroff - Run the device driver's poweroff callback.
 * @dev: Device to handle.
 *
 * Follow PCI and resume devices from runtime suspend before running their
 * system poweroff callbacks, unless the driver can cope with runtime-suspended
 * devices during system suspend and there are no ACPI-specific reasons for
 * resuming them.
 */
int acpi_subsys_poweroff(struct device *dev)
{
	if (!dev_pm_test_driver_flags(dev, DPM_FLAG_SMART_SUSPEND) ||
	    acpi_dev_needs_resume(dev, ACPI_COMPANION(dev)))
		pm_runtime_resume(dev);

	return pm_generic_poweroff(dev);
}
EXPORT_SYMBOL_GPL(acpi_subsys_poweroff);

/**
 * acpi_subsys_poweroff_late - Run the device driver's poweroff callback.
 * @dev: Device to handle.
 *
 * Carry out the generic late poweroff procedure for @dev and use ACPI to put
 * it into a low-power state during system transition into a sleep state.
 */
static int acpi_subsys_poweroff_late(struct device *dev)
{
	int ret;

	if (dev_pm_skip_suspend(dev))
		return 0;

	ret = pm_generic_poweroff_late(dev);
	if (ret)
		return ret;

	return acpi_dev_suspend(dev, device_may_wakeup(dev));
}

/**
 * acpi_subsys_poweroff_noirq - Run the driver's "noirq" poweroff callback.
 * @dev: Device to suspend.
 */
static int acpi_subsys_poweroff_noirq(struct device *dev)
{
	if (dev_pm_skip_suspend(dev))
		return 0;

	return pm_generic_poweroff_noirq(dev);
}
#endif /* CONFIG_PM_SLEEP */

static struct dev_pm_domain acpi_general_pm_domain = {
	.ops = {
		.runtime_suspend = acpi_subsys_runtime_suspend,
		.runtime_resume = acpi_subsys_runtime_resume,
#ifdef CONFIG_PM_SLEEP
		.prepare = acpi_subsys_prepare,
		.complete = acpi_subsys_complete,
		.suspend = acpi_subsys_suspend,
		.suspend_late = acpi_subsys_suspend_late,
		.suspend_noirq = acpi_subsys_suspend_noirq,
		.resume_noirq = acpi_subsys_resume_noirq,
		.resume_early = acpi_subsys_resume_early,
		.freeze = acpi_subsys_freeze,
		.poweroff = acpi_subsys_poweroff,
		.poweroff_late = acpi_subsys_poweroff_late,
		.poweroff_noirq = acpi_subsys_poweroff_noirq,
		.restore_early = acpi_subsys_restore_early,
#endif
	},
};

/**
 * acpi_dev_pm_detach - Remove ACPI power management from the device.
 * @dev: Device to take care of.
 * @power_off: Whether or not to try to remove power from the device.
 *
 * Remove the device from the general ACPI PM domain and remove its wakeup
 * notifier.  If @power_off is set, additionally remove power from the device if
 * possible.
 *
 * Callers must ensure proper synchronization of this function with power
 * management callbacks.
 */
static void acpi_dev_pm_detach(struct device *dev, bool power_off)
{
	struct acpi_device *adev = ACPI_COMPANION(dev);

	if (adev && dev->pm_domain == &acpi_general_pm_domain) {
		dev_pm_domain_set(dev, NULL);
		acpi_remove_pm_notifier(adev);
		if (power_off) {
			/*
			 * If the device's PM QoS resume latency limit or flags
			 * have been exposed to user space, they have to be
			 * hidden at this point, so that they don't affect the
			 * choice of the low-power state to put the device into.
			 */
			dev_pm_qos_hide_latency_limit(dev);
			dev_pm_qos_hide_flags(dev);
			acpi_device_wakeup_disable(adev);
			acpi_dev_pm_low_power(dev, adev, ACPI_STATE_S0);
		}
	}
}

/**
 * acpi_dev_pm_attach - Prepare device for ACPI power management.
 * @dev: Device to prepare.
 * @power_on: Whether or not to power on the device.
 *
 * If @dev has a valid ACPI handle that has a valid struct acpi_device object
 * attached to it, install a wakeup notification handler for the device and
 * add it to the general ACPI PM domain.  If @power_on is set, the device will
 * be put into the ACPI D0 state before the function returns.
 *
 * This assumes that the @dev's bus type uses generic power management callbacks
 * (or doesn't use any power management callbacks at all).
 *
 * Callers must ensure proper synchronization of this function with power
 * management callbacks.
 */
int acpi_dev_pm_attach(struct device *dev, bool power_on)
{
	/*
	 * Skip devices whose ACPI companions match the device IDs below,
	 * because they require special power management handling incompatible
	 * with the generic ACPI PM domain.
	 */
	static const struct acpi_device_id special_pm_ids[] = {
		{"PNP0C0B", }, /* Generic ACPI fan */
		{"INT3404", }, /* Fan */
		{"INTC1044", }, /* Fan for Tiger Lake generation */
		{"INTC1048", }, /* Fan for Alder Lake generation */
		{}
	};
	struct acpi_device *adev = ACPI_COMPANION(dev);

	if (!adev || !acpi_match_device_ids(adev, special_pm_ids))
		return 0;

	/*
	 * Only attach the power domain to the first device if the
	 * companion is shared by multiple. This is to prevent doing power
	 * management twice.
	 */
	if (!acpi_device_is_first_physical_node(adev, dev))
		return 0;

	acpi_add_pm_notifier(adev, dev, acpi_pm_notify_work_func);
	dev_pm_domain_set(dev, &acpi_general_pm_domain);
	if (power_on) {
		acpi_dev_pm_full_power(adev);
		acpi_device_wakeup_disable(adev);
	}

	dev->pm_domain->detach = acpi_dev_pm_detach;
	return 1;
}
EXPORT_SYMBOL_GPL(acpi_dev_pm_attach);

/**
 * acpi_storage_d3 - Check if a storage device should use D3.
 * @dev: Device to check
 *
 * Returns %true if @dev should be put into D3 when the ->suspend method is
 * called, else %false.  The name of this function is somewhat misleading
 * as it has nothing to do with storage except for the name of the ACPI
 * property.  On some platforms resume will not work if this hint is ignored.
 *
 */
bool acpi_storage_d3(struct device *dev)
{
	struct acpi_device *adev = ACPI_COMPANION(dev);
	u8 val;

	if (force_storage_d3())
		return true;

	if (!adev)
		return false;
	if (fwnode_property_read_u8(acpi_fwnode_handle(adev), "StorageD3Enable",
			&val))
		return false;
	return val == 1;
}
EXPORT_SYMBOL_GPL(acpi_storage_d3);

#endif /* CONFIG_PM */
