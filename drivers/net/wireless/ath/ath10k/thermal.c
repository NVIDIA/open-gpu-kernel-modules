// SPDX-License-Identifier: ISC
/*
 * Copyright (c) 2014-2015 Qualcomm Atheros, Inc.
 */

#include <linux/device.h>
#include <linux/sysfs.h>
#include <linux/thermal.h>
#include <linux/hwmon.h>
#include <linux/hwmon-sysfs.h>
#include "core.h"
#include "debug.h"
#include "wmi-ops.h"

static int
ath10k_thermal_get_max_throttle_state(struct thermal_cooling_device *cdev,
				      unsigned long *state)
{
	*state = ATH10K_THERMAL_THROTTLE_MAX;

	return 0;
}

static int
ath10k_thermal_get_cur_throttle_state(struct thermal_cooling_device *cdev,
				      unsigned long *state)
{
	struct ath10k *ar = cdev->devdata;

	mutex_lock(&ar->conf_mutex);
	*state = ar->thermal.throttle_state;
	mutex_unlock(&ar->conf_mutex);

	return 0;
}

static int
ath10k_thermal_set_cur_throttle_state(struct thermal_cooling_device *cdev,
				      unsigned long throttle_state)
{
	struct ath10k *ar = cdev->devdata;

	if (throttle_state > ATH10K_THERMAL_THROTTLE_MAX) {
		ath10k_warn(ar, "throttle state %ld is exceeding the limit %d\n",
			    throttle_state, ATH10K_THERMAL_THROTTLE_MAX);
		return -EINVAL;
	}
	mutex_lock(&ar->conf_mutex);
	ar->thermal.throttle_state = throttle_state;
	ath10k_thermal_set_throttling(ar);
	mutex_unlock(&ar->conf_mutex);
	return 0;
}

static const struct thermal_cooling_device_ops ath10k_thermal_ops = {
	.get_max_state = ath10k_thermal_get_max_throttle_state,
	.get_cur_state = ath10k_thermal_get_cur_throttle_state,
	.set_cur_state = ath10k_thermal_set_cur_throttle_state,
};

static ssize_t ath10k_thermal_show_temp(struct device *dev,
					struct device_attribute *attr,
					char *buf)
{
	struct ath10k *ar = dev_get_drvdata(dev);
	int ret, temperature;
	unsigned long time_left;

	mutex_lock(&ar->conf_mutex);

	/* Can't get temperature when the card is off */
	if (ar->state != ATH10K_STATE_ON) {
		ret = -ENETDOWN;
		goto out;
	}

	reinit_completion(&ar->thermal.wmi_sync);
	ret = ath10k_wmi_pdev_get_temperature(ar);
	if (ret) {
		ath10k_warn(ar, "failed to read temperature %d\n", ret);
		goto out;
	}

	if (test_bit(ATH10K_FLAG_CRASH_FLUSH, &ar->dev_flags)) {
		ret = -ESHUTDOWN;
		goto out;
	}

	time_left = wait_for_completion_timeout(&ar->thermal.wmi_sync,
						ATH10K_THERMAL_SYNC_TIMEOUT_HZ);
	if (!time_left) {
		ath10k_warn(ar, "failed to synchronize thermal read\n");
		ret = -ETIMEDOUT;
		goto out;
	}

	spin_lock_bh(&ar->data_lock);
	temperature = ar->thermal.temperature;
	spin_unlock_bh(&ar->data_lock);

	/* display in millidegree celcius */
	ret = snprintf(buf, PAGE_SIZE, "%d\n", temperature * 1000);
out:
	mutex_unlock(&ar->conf_mutex);
	return ret;
}

void ath10k_thermal_event_temperature(struct ath10k *ar, int temperature)
{
	spin_lock_bh(&ar->data_lock);
	ar->thermal.temperature = temperature;
	spin_unlock_bh(&ar->data_lock);
	complete(&ar->thermal.wmi_sync);
}

static SENSOR_DEVICE_ATTR(temp1_input, 0444, ath10k_thermal_show_temp,
			  NULL, 0);

static struct attribute *ath10k_hwmon_attrs[] = {
	&sensor_dev_attr_temp1_input.dev_attr.attr,
	NULL,
};
ATTRIBUTE_GROUPS(ath10k_hwmon);

void ath10k_thermal_set_throttling(struct ath10k *ar)
{
	u32 period, duration, enabled;
	int ret;

	lockdep_assert_held(&ar->conf_mutex);

	if (!test_bit(WMI_SERVICE_THERM_THROT, ar->wmi.svc_map))
		return;

	if (!ar->wmi.ops->gen_pdev_set_quiet_mode)
		return;

	if (ar->state != ATH10K_STATE_ON)
		return;

	period = ar->thermal.quiet_period;
	duration = (period * ar->thermal.throttle_state) / 100;
	enabled = duration ? 1 : 0;

	ret = ath10k_wmi_pdev_set_quiet_mode(ar, period, duration,
					     ATH10K_QUIET_START_OFFSET,
					     enabled);
	if (ret) {
		ath10k_warn(ar, "failed to set quiet mode period %u duarion %u enabled %u ret %d\n",
			    period, duration, enabled, ret);
	}
}

int ath10k_thermal_register(struct ath10k *ar)
{
	struct thermal_cooling_device *cdev;
	struct device *hwmon_dev;
	int ret;

	if (!test_bit(WMI_SERVICE_THERM_THROT, ar->wmi.svc_map))
		return 0;

	cdev = thermal_cooling_device_register("ath10k_thermal", ar,
					       &ath10k_thermal_ops);

	if (IS_ERR(cdev)) {
		ath10k_err(ar, "failed to setup thermal device result: %ld\n",
			   PTR_ERR(cdev));
		return -EINVAL;
	}

	ret = sysfs_create_link(&ar->dev->kobj, &cdev->device.kobj,
				"cooling_device");
	if (ret) {
		ath10k_err(ar, "failed to create cooling device symlink\n");
		goto err_cooling_destroy;
	}

	ar->thermal.cdev = cdev;
	ar->thermal.quiet_period = ATH10K_QUIET_PERIOD_DEFAULT;

	/* Do not register hwmon device when temperature reading is not
	 * supported by firmware
	 */
	if (!(ar->wmi.ops->gen_pdev_get_temperature))
		return 0;

	/* Avoid linking error on devm_hwmon_device_register_with_groups, I
	 * guess linux/hwmon.h is missing proper stubs.
	 */
	if (!IS_REACHABLE(CONFIG_HWMON))
		return 0;

	hwmon_dev = devm_hwmon_device_register_with_groups(ar->dev,
							   "ath10k_hwmon", ar,
							   ath10k_hwmon_groups);
	if (IS_ERR(hwmon_dev)) {
		ath10k_err(ar, "failed to register hwmon device: %ld\n",
			   PTR_ERR(hwmon_dev));
		ret = -EINVAL;
		goto err_remove_link;
	}
	return 0;

err_remove_link:
	sysfs_remove_link(&ar->dev->kobj, "cooling_device");
err_cooling_destroy:
	thermal_cooling_device_unregister(cdev);
	return ret;
}

void ath10k_thermal_unregister(struct ath10k *ar)
{
	if (!test_bit(WMI_SERVICE_THERM_THROT, ar->wmi.svc_map))
		return;

	sysfs_remove_link(&ar->dev->kobj, "cooling_device");
	thermal_cooling_device_unregister(ar->thermal.cdev);
}
