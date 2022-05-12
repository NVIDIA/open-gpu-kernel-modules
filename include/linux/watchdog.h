/* SPDX-License-Identifier: GPL-2.0 */
/*
 *	Generic watchdog defines. Derived from..
 *
 * Berkshire PC Watchdog Defines
 * by Ken Hollis <khollis@bitgate.com>
 *
 */
#ifndef _LINUX_WATCHDOG_H
#define _LINUX_WATCHDOG_H


#include <linux/bitops.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/notifier.h>
#include <uapi/linux/watchdog.h>

struct watchdog_ops;
struct watchdog_device;
struct watchdog_core_data;
struct watchdog_governor;

/** struct watchdog_ops - The watchdog-devices operations
 *
 * @owner:	The module owner.
 * @start:	The routine for starting the watchdog device.
 * @stop:	The routine for stopping the watchdog device.
 * @ping:	The routine that sends a keepalive ping to the watchdog device.
 * @status:	The routine that shows the status of the watchdog device.
 * @set_timeout:The routine for setting the watchdog devices timeout value (in seconds).
 * @set_pretimeout:The routine for setting the watchdog devices pretimeout.
 * @get_timeleft:The routine that gets the time left before a reset (in seconds).
 * @restart:	The routine for restarting the machine.
 * @ioctl:	The routines that handles extra ioctl calls.
 *
 * The watchdog_ops structure contains a list of low-level operations
 * that control a watchdog device. It also contains the module that owns
 * these operations. The start function is mandatory, all other
 * functions are optional.
 */
struct watchdog_ops {
	struct module *owner;
	/* mandatory operations */
	int (*start)(struct watchdog_device *);
	/* optional operations */
	int (*stop)(struct watchdog_device *);
	int (*ping)(struct watchdog_device *);
	unsigned int (*status)(struct watchdog_device *);
	int (*set_timeout)(struct watchdog_device *, unsigned int);
	int (*set_pretimeout)(struct watchdog_device *, unsigned int);
	unsigned int (*get_timeleft)(struct watchdog_device *);
	int (*restart)(struct watchdog_device *, unsigned long, void *);
	long (*ioctl)(struct watchdog_device *, unsigned int, unsigned long);
};

/** struct watchdog_device - The structure that defines a watchdog device
 *
 * @id:		The watchdog's ID. (Allocated by watchdog_register_device)
 * @parent:	The parent bus device
 * @groups:	List of sysfs attribute groups to create when creating the
 *		watchdog device.
 * @info:	Pointer to a watchdog_info structure.
 * @ops:	Pointer to the list of watchdog operations.
 * @gov:	Pointer to watchdog pretimeout governor.
 * @bootstatus:	Status of the watchdog device at boot.
 * @timeout:	The watchdog devices timeout value (in seconds).
 * @pretimeout: The watchdog devices pre_timeout value.
 * @min_timeout:The watchdog devices minimum timeout value (in seconds).
 * @max_timeout:The watchdog devices maximum timeout value (in seconds)
 *		as configurable from user space. Only relevant if
 *		max_hw_heartbeat_ms is not provided.
 * @min_hw_heartbeat_ms:
 *		Hardware limit for minimum time between heartbeats,
 *		in milli-seconds.
 * @max_hw_heartbeat_ms:
 *		Hardware limit for maximum timeout, in milli-seconds.
 *		Replaces max_timeout if specified.
 * @reboot_nb:	The notifier block to stop watchdog on reboot.
 * @restart_nb:	The notifier block to register a restart function.
 * @driver_data:Pointer to the drivers private data.
 * @wd_data:	Pointer to watchdog core internal data.
 * @status:	Field that contains the devices internal status bits.
 * @deferred:	Entry in wtd_deferred_reg_list which is used to
 *		register early initialized watchdogs.
 *
 * The watchdog_device structure contains all information about a
 * watchdog timer device.
 *
 * The driver-data field may not be accessed directly. It must be accessed
 * via the watchdog_set_drvdata and watchdog_get_drvdata helpers.
 */
struct watchdog_device {
	int id;
	struct device *parent;
	const struct attribute_group **groups;
	const struct watchdog_info *info;
	const struct watchdog_ops *ops;
	const struct watchdog_governor *gov;
	unsigned int bootstatus;
	unsigned int timeout;
	unsigned int pretimeout;
	unsigned int min_timeout;
	unsigned int max_timeout;
	unsigned int min_hw_heartbeat_ms;
	unsigned int max_hw_heartbeat_ms;
	struct notifier_block reboot_nb;
	struct notifier_block restart_nb;
	void *driver_data;
	struct watchdog_core_data *wd_data;
	unsigned long status;
/* Bit numbers for status flags */
#define WDOG_ACTIVE		0	/* Is the watchdog running/active */
#define WDOG_NO_WAY_OUT		1	/* Is 'nowayout' feature set ? */
#define WDOG_STOP_ON_REBOOT	2	/* Should be stopped on reboot */
#define WDOG_HW_RUNNING		3	/* True if HW watchdog running */
#define WDOG_STOP_ON_UNREGISTER	4	/* Should be stopped on unregister */
	struct list_head deferred;
};

#define WATCHDOG_NOWAYOUT		IS_BUILTIN(CONFIG_WATCHDOG_NOWAYOUT)
#define WATCHDOG_NOWAYOUT_INIT_STATUS	(WATCHDOG_NOWAYOUT << WDOG_NO_WAY_OUT)

/* Use the following function to check whether or not the watchdog is active */
static inline bool watchdog_active(struct watchdog_device *wdd)
{
	return test_bit(WDOG_ACTIVE, &wdd->status);
}

/*
 * Use the following function to check whether or not the hardware watchdog
 * is running
 */
static inline bool watchdog_hw_running(struct watchdog_device *wdd)
{
	return test_bit(WDOG_HW_RUNNING, &wdd->status);
}

/* Use the following function to set the nowayout feature */
static inline void watchdog_set_nowayout(struct watchdog_device *wdd, bool nowayout)
{
	if (nowayout)
		set_bit(WDOG_NO_WAY_OUT, &wdd->status);
}

/* Use the following function to stop the watchdog on reboot */
static inline void watchdog_stop_on_reboot(struct watchdog_device *wdd)
{
	set_bit(WDOG_STOP_ON_REBOOT, &wdd->status);
}

/* Use the following function to stop the watchdog when unregistering it */
static inline void watchdog_stop_on_unregister(struct watchdog_device *wdd)
{
	set_bit(WDOG_STOP_ON_UNREGISTER, &wdd->status);
}

/* Use the following function to check if a timeout value is invalid */
static inline bool watchdog_timeout_invalid(struct watchdog_device *wdd, unsigned int t)
{
	/*
	 * The timeout is invalid if
	 * - the requested value is larger than UINT_MAX / 1000
	 *   (since internal calculations are done in milli-seconds),
	 * or
	 * - the requested value is smaller than the configured minimum timeout,
	 * or
	 * - a maximum hardware timeout is not configured, a maximum timeout
	 *   is configured, and the requested value is larger than the
	 *   configured maximum timeout.
	 */
	return t > UINT_MAX / 1000 || t < wdd->min_timeout ||
		(!wdd->max_hw_heartbeat_ms && wdd->max_timeout &&
		 t > wdd->max_timeout);
}

/* Use the following function to check if a pretimeout value is invalid */
static inline bool watchdog_pretimeout_invalid(struct watchdog_device *wdd,
					       unsigned int t)
{
	return t && wdd->timeout && t >= wdd->timeout;
}

/* Use the following functions to manipulate watchdog driver specific data */
static inline void watchdog_set_drvdata(struct watchdog_device *wdd, void *data)
{
	wdd->driver_data = data;
}

static inline void *watchdog_get_drvdata(struct watchdog_device *wdd)
{
	return wdd->driver_data;
}

/* Use the following functions to report watchdog pretimeout event */
#if IS_ENABLED(CONFIG_WATCHDOG_PRETIMEOUT_GOV)
void watchdog_notify_pretimeout(struct watchdog_device *wdd);
#else
static inline void watchdog_notify_pretimeout(struct watchdog_device *wdd)
{
	pr_alert("watchdog%d: pretimeout event\n", wdd->id);
}
#endif

/* drivers/watchdog/watchdog_core.c */
void watchdog_set_restart_priority(struct watchdog_device *wdd, int priority);
extern int watchdog_init_timeout(struct watchdog_device *wdd,
				  unsigned int timeout_parm, struct device *dev);
extern int watchdog_register_device(struct watchdog_device *);
extern void watchdog_unregister_device(struct watchdog_device *);

int watchdog_set_last_hw_keepalive(struct watchdog_device *, unsigned int);

/* devres register variant */
int devm_watchdog_register_device(struct device *dev, struct watchdog_device *);

#endif  /* ifndef _LINUX_WATCHDOG_H */
