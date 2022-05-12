// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * processor_driver.c - ACPI Processor Driver
 *
 *  Copyright (C) 2001, 2002 Andy Grover <andrew.grover@intel.com>
 *  Copyright (C) 2001, 2002 Paul Diefenbaugh <paul.s.diefenbaugh@intel.com>
 *  Copyright (C) 2004       Dominik Brodowski <linux@brodo.de>
 *  Copyright (C) 2004  Anil S Keshavamurthy <anil.s.keshavamurthy@intel.com>
 *  			- Added processor hotplug support
 *  Copyright (C) 2013, Intel Corporation
 *                      Rafael J. Wysocki <rafael.j.wysocki@intel.com>
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/cpufreq.h>
#include <linux/cpu.h>
#include <linux/cpuidle.h>
#include <linux/slab.h>
#include <linux/acpi.h>

#include <acpi/processor.h>

#include "internal.h"

#define ACPI_PROCESSOR_NOTIFY_PERFORMANCE 0x80
#define ACPI_PROCESSOR_NOTIFY_POWER	0x81
#define ACPI_PROCESSOR_NOTIFY_THROTTLING	0x82

MODULE_AUTHOR("Paul Diefenbaugh");
MODULE_DESCRIPTION("ACPI Processor Driver");
MODULE_LICENSE("GPL");

static int acpi_processor_start(struct device *dev);
static int acpi_processor_stop(struct device *dev);

static const struct acpi_device_id processor_device_ids[] = {
	{ACPI_PROCESSOR_OBJECT_HID, 0},
	{ACPI_PROCESSOR_DEVICE_HID, 0},
	{"", 0},
};
MODULE_DEVICE_TABLE(acpi, processor_device_ids);

static struct device_driver acpi_processor_driver = {
	.name = "processor",
	.bus = &cpu_subsys,
	.acpi_match_table = processor_device_ids,
	.probe = acpi_processor_start,
	.remove = acpi_processor_stop,
};

static void acpi_processor_notify(acpi_handle handle, u32 event, void *data)
{
	struct acpi_device *device = data;
	struct acpi_processor *pr;
	int saved;

	if (device->handle != handle)
		return;

	pr = acpi_driver_data(device);
	if (!pr)
		return;

	switch (event) {
	case ACPI_PROCESSOR_NOTIFY_PERFORMANCE:
		saved = pr->performance_platform_limit;
		acpi_processor_ppc_has_changed(pr, 1);
		if (saved == pr->performance_platform_limit)
			break;
		acpi_bus_generate_netlink_event(device->pnp.device_class,
						  dev_name(&device->dev), event,
						  pr->performance_platform_limit);
		break;
	case ACPI_PROCESSOR_NOTIFY_POWER:
		acpi_processor_power_state_has_changed(pr);
		acpi_bus_generate_netlink_event(device->pnp.device_class,
						  dev_name(&device->dev), event, 0);
		break;
	case ACPI_PROCESSOR_NOTIFY_THROTTLING:
		acpi_processor_tstate_has_changed(pr);
		acpi_bus_generate_netlink_event(device->pnp.device_class,
						  dev_name(&device->dev), event, 0);
		break;
	default:
		acpi_handle_debug(handle, "Unsupported event [0x%x]\n", event);
		break;
	}

	return;
}

static int __acpi_processor_start(struct acpi_device *device);

static int acpi_soft_cpu_online(unsigned int cpu)
{
	struct acpi_processor *pr = per_cpu(processors, cpu);
	struct acpi_device *device;

	if (!pr || acpi_bus_get_device(pr->handle, &device))
		return 0;
	/*
	 * CPU got physically hotplugged and onlined for the first time:
	 * Initialize missing things.
	 */
	if (pr->flags.need_hotplug_init) {
		int ret;

		pr_info("Will online and init hotplugged CPU: %d\n",
			pr->id);
		pr->flags.need_hotplug_init = 0;
		ret = __acpi_processor_start(device);
		WARN(ret, "Failed to start CPU: %d\n", pr->id);
	} else {
		/* Normal CPU soft online event. */
		acpi_processor_ppc_has_changed(pr, 0);
		acpi_processor_hotplug(pr);
		acpi_processor_reevaluate_tstate(pr, false);
		acpi_processor_tstate_has_changed(pr);
	}
	return 0;
}

static int acpi_soft_cpu_dead(unsigned int cpu)
{
	struct acpi_processor *pr = per_cpu(processors, cpu);
	struct acpi_device *device;

	if (!pr || acpi_bus_get_device(pr->handle, &device))
		return 0;

	acpi_processor_reevaluate_tstate(pr, true);
	return 0;
}

#ifdef CONFIG_ACPI_CPU_FREQ_PSS
static int acpi_pss_perf_init(struct acpi_processor *pr,
		struct acpi_device *device)
{
	int result = 0;

	acpi_processor_ppc_has_changed(pr, 0);

	acpi_processor_get_throttling_info(pr);

	if (pr->flags.throttling)
		pr->flags.limit = 1;

	pr->cdev = thermal_cooling_device_register("Processor", device,
						   &processor_cooling_ops);
	if (IS_ERR(pr->cdev)) {
		result = PTR_ERR(pr->cdev);
		return result;
	}

	dev_dbg(&device->dev, "registered as cooling_device%d\n",
		pr->cdev->id);

	result = sysfs_create_link(&device->dev.kobj,
				   &pr->cdev->device.kobj,
				   "thermal_cooling");
	if (result) {
		dev_err(&device->dev,
			"Failed to create sysfs link 'thermal_cooling'\n");
		goto err_thermal_unregister;
	}

	result = sysfs_create_link(&pr->cdev->device.kobj,
				   &device->dev.kobj,
				   "device");
	if (result) {
		dev_err(&pr->cdev->device,
			"Failed to create sysfs link 'device'\n");
		goto err_remove_sysfs_thermal;
	}

	return 0;

 err_remove_sysfs_thermal:
	sysfs_remove_link(&device->dev.kobj, "thermal_cooling");
 err_thermal_unregister:
	thermal_cooling_device_unregister(pr->cdev);

	return result;
}

static void acpi_pss_perf_exit(struct acpi_processor *pr,
		struct acpi_device *device)
{
	if (pr->cdev) {
		sysfs_remove_link(&device->dev.kobj, "thermal_cooling");
		sysfs_remove_link(&pr->cdev->device.kobj, "device");
		thermal_cooling_device_unregister(pr->cdev);
		pr->cdev = NULL;
	}
}
#else
static inline int acpi_pss_perf_init(struct acpi_processor *pr,
		struct acpi_device *device)
{
	return 0;
}

static inline void acpi_pss_perf_exit(struct acpi_processor *pr,
		struct acpi_device *device) {}
#endif /* CONFIG_ACPI_CPU_FREQ_PSS */

static int __acpi_processor_start(struct acpi_device *device)
{
	struct acpi_processor *pr = acpi_driver_data(device);
	acpi_status status;
	int result = 0;

	if (!pr)
		return -ENODEV;

	if (pr->flags.need_hotplug_init)
		return 0;

	result = acpi_cppc_processor_probe(pr);
	if (result && !IS_ENABLED(CONFIG_ACPI_CPU_FREQ_PSS))
		dev_dbg(&device->dev, "CPPC data invalid or not present\n");

	if (!cpuidle_get_driver() || cpuidle_get_driver() == &acpi_idle_driver)
		acpi_processor_power_init(pr);

	result = acpi_pss_perf_init(pr, device);
	if (result)
		goto err_power_exit;

	status = acpi_install_notify_handler(device->handle, ACPI_DEVICE_NOTIFY,
					     acpi_processor_notify, device);
	if (ACPI_SUCCESS(status))
		return 0;

	result = -ENODEV;
	acpi_pss_perf_exit(pr, device);

err_power_exit:
	acpi_processor_power_exit(pr);
	return result;
}

static int acpi_processor_start(struct device *dev)
{
	struct acpi_device *device = ACPI_COMPANION(dev);
	int ret;

	if (!device)
		return -ENODEV;

	/* Protect against concurrent CPU hotplug operations */
	cpu_hotplug_disable();
	ret = __acpi_processor_start(device);
	cpu_hotplug_enable();
	return ret;
}

static int acpi_processor_stop(struct device *dev)
{
	struct acpi_device *device = ACPI_COMPANION(dev);
	struct acpi_processor *pr;

	if (!device)
		return 0;

	acpi_remove_notify_handler(device->handle, ACPI_DEVICE_NOTIFY,
				   acpi_processor_notify);

	pr = acpi_driver_data(device);
	if (!pr)
		return 0;
	acpi_processor_power_exit(pr);

	acpi_pss_perf_exit(pr, device);

	acpi_cppc_processor_exit(pr);

	return 0;
}

bool acpi_processor_cpufreq_init;

static int acpi_processor_notifier(struct notifier_block *nb,
				   unsigned long event, void *data)
{
	struct cpufreq_policy *policy = data;

	if (event == CPUFREQ_CREATE_POLICY) {
		acpi_thermal_cpufreq_init(policy);
		acpi_processor_ppc_init(policy);
	} else if (event == CPUFREQ_REMOVE_POLICY) {
		acpi_processor_ppc_exit(policy);
		acpi_thermal_cpufreq_exit(policy);
	}

	return 0;
}

static struct notifier_block acpi_processor_notifier_block = {
	.notifier_call = acpi_processor_notifier,
};

/*
 * We keep the driver loaded even when ACPI is not running.
 * This is needed for the powernow-k8 driver, that works even without
 * ACPI, but needs symbols from this driver
 */
static enum cpuhp_state hp_online;
static int __init acpi_processor_driver_init(void)
{
	int result = 0;

	if (acpi_disabled)
		return 0;

	result = driver_register(&acpi_processor_driver);
	if (result < 0)
		return result;

	result = cpuhp_setup_state_nocalls(CPUHP_AP_ONLINE_DYN,
					   "acpi/cpu-drv:online",
					   acpi_soft_cpu_online, NULL);
	if (result < 0)
		goto err;
	hp_online = result;
	cpuhp_setup_state_nocalls(CPUHP_ACPI_CPUDRV_DEAD, "acpi/cpu-drv:dead",
				  NULL, acpi_soft_cpu_dead);

	if (!cpufreq_register_notifier(&acpi_processor_notifier_block,
				       CPUFREQ_POLICY_NOTIFIER)) {
		acpi_processor_cpufreq_init = true;
		acpi_processor_ignore_ppc_init();
	}

	acpi_processor_throttling_init();
	return 0;
err:
	driver_unregister(&acpi_processor_driver);
	return result;
}

static void __exit acpi_processor_driver_exit(void)
{
	if (acpi_disabled)
		return;

	if (acpi_processor_cpufreq_init) {
		cpufreq_unregister_notifier(&acpi_processor_notifier_block,
					    CPUFREQ_POLICY_NOTIFIER);
		acpi_processor_cpufreq_init = false;
	}

	cpuhp_remove_state_nocalls(hp_online);
	cpuhp_remove_state_nocalls(CPUHP_ACPI_CPUDRV_DEAD);
	driver_unregister(&acpi_processor_driver);
}

module_init(acpi_processor_driver_init);
module_exit(acpi_processor_driver_exit);

MODULE_ALIAS("processor");
