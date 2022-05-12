/* SPDX-License-Identifier: GPL-2.0 */
#ifndef S390_CCWGROUP_H
#define S390_CCWGROUP_H

struct ccw_device;
struct ccw_driver;

/**
 * struct ccwgroup_device - ccw group device
 * @state: online/offline state
 * @count: number of attached slave devices
 * @dev: embedded device structure
 * @cdev: variable number of slave devices, allocated as needed
 * @ungroup_work: work to be done when a ccwgroup notifier has action
 *	type %BUS_NOTIFY_UNBIND_DRIVER
 */
struct ccwgroup_device {
	enum {
		CCWGROUP_OFFLINE,
		CCWGROUP_ONLINE,
	} state;
/* private: */
	atomic_t onoff;
	struct mutex reg_mutex;
/* public: */
	unsigned int count;
	struct device	dev;
	struct work_struct ungroup_work;
	struct ccw_device *cdev[0];
};

/**
 * struct ccwgroup_driver - driver for ccw group devices
 * @setup: function called during device creation to setup the device
 * @remove: function called on remove
 * @set_online: function called when device is set online
 * @set_offline: function called when device is set offline
 * @shutdown: function called when device is shut down
 * @driver: embedded driver structure
 * @ccw_driver: supported ccw_driver (optional)
 */
struct ccwgroup_driver {
	int (*setup) (struct ccwgroup_device *);
	void (*remove) (struct ccwgroup_device *);
	int (*set_online) (struct ccwgroup_device *);
	int (*set_offline) (struct ccwgroup_device *);
	void (*shutdown)(struct ccwgroup_device *);

	struct device_driver driver;
	struct ccw_driver *ccw_driver;
};

extern int  ccwgroup_driver_register   (struct ccwgroup_driver *cdriver);
extern void ccwgroup_driver_unregister (struct ccwgroup_driver *cdriver);
int ccwgroup_create_dev(struct device *root, struct ccwgroup_driver *gdrv,
			int num_devices, const char *buf);
struct ccwgroup_device *get_ccwgroupdev_by_busid(struct ccwgroup_driver *gdrv,
						 char *bus_id);

extern int ccwgroup_set_online(struct ccwgroup_device *gdev);
extern int ccwgroup_set_offline(struct ccwgroup_device *gdev);

extern int ccwgroup_probe_ccwdev(struct ccw_device *cdev);
extern void ccwgroup_remove_ccwdev(struct ccw_device *cdev);

#define to_ccwgroupdev(x) container_of((x), struct ccwgroup_device, dev)
#define to_ccwgroupdrv(x) container_of((x), struct ccwgroup_driver, driver)

#if IS_ENABLED(CONFIG_CCWGROUP)
bool dev_is_ccwgroup(struct device *dev);
#else /* CONFIG_CCWGROUP */
static inline bool dev_is_ccwgroup(struct device *dev)
{
	return false;
}
#endif /* CONFIG_CCWGROUP */

#endif
