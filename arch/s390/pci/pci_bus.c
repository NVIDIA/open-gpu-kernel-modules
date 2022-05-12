// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright IBM Corp. 2020
 *
 * Author(s):
 *   Pierre Morel <pmorel@linux.ibm.com>
 *
 */

#define KMSG_COMPONENT "zpci"
#define pr_fmt(fmt) KMSG_COMPONENT ": " fmt

#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/err.h>
#include <linux/export.h>
#include <linux/delay.h>
#include <linux/seq_file.h>
#include <linux/jump_label.h>
#include <linux/pci.h>
#include <linux/printk.h>

#include <asm/pci_clp.h>
#include <asm/pci_dma.h>

#include "pci_bus.h"
#include "pci_iov.h"

static LIST_HEAD(zbus_list);
static DEFINE_MUTEX(zbus_list_lock);
static int zpci_nb_devices;

/* zpci_bus_prepare_device - Prepare a zPCI function for scanning
 * @zdev: the zPCI function to be prepared
 *
 * The PCI resources for the function are set up and added to its zbus and the
 * function is enabled. The function must be added to a zbus which must have
 * a PCI bus created. If an error occurs the zPCI function is not enabled.
 *
 * Return: 0 on success, an error code otherwise
 */
static int zpci_bus_prepare_device(struct zpci_dev *zdev)
{
	struct resource_entry *window, *n;
	struct resource *res;
	int rc;

	if (!zdev_enabled(zdev)) {
		rc = zpci_enable_device(zdev);
		if (rc)
			return rc;
	}

	if (!zdev->has_resources) {
		zpci_setup_bus_resources(zdev, &zdev->zbus->resources);
		resource_list_for_each_entry_safe(window, n, &zdev->zbus->resources) {
			res = window->res;
			pci_bus_add_resource(zdev->zbus->bus, res, 0);
		}
	}

	return 0;
}

/* zpci_bus_scan_device - Scan a single device adding it to the PCI core
 * @zdev: the zdev to be scanned
 *
 * Scans the PCI function making it available to the common PCI code.
 *
 * Return: 0 on success, an error value otherwise
 */
int zpci_bus_scan_device(struct zpci_dev *zdev)
{
	struct pci_dev *pdev;
	int rc;

	rc = zpci_bus_prepare_device(zdev);
	if (rc)
		return rc;

	pdev = pci_scan_single_device(zdev->zbus->bus, zdev->devfn);
	if (!pdev)
		return -ENODEV;

	pci_bus_add_device(pdev);
	pci_lock_rescan_remove();
	pci_bus_add_devices(zdev->zbus->bus);
	pci_unlock_rescan_remove();

	return 0;
}

/* zpci_bus_remove_device - Removes the given zdev from the PCI core
 * @zdev: the zdev to be removed from the PCI core
 * @set_error: if true the device's error state is set to permanent failure
 *
 * Sets a zPCI device to a configured but offline state; the zPCI
 * device is still accessible through its hotplug slot and the zPCI
 * API but is removed from the common code PCI bus, making it
 * no longer available to drivers.
 */
void zpci_bus_remove_device(struct zpci_dev *zdev, bool set_error)
{
	struct zpci_bus *zbus = zdev->zbus;
	struct pci_dev *pdev;

	if (!zdev->zbus->bus)
		return;

	pdev = pci_get_slot(zbus->bus, zdev->devfn);
	if (pdev) {
		if (set_error)
			pdev->error_state = pci_channel_io_perm_failure;
		if (pdev->is_virtfn) {
			zpci_iov_remove_virtfn(pdev, zdev->vfn);
			/* balance pci_get_slot */
			pci_dev_put(pdev);
			return;
		}
		pci_stop_and_remove_bus_device_locked(pdev);
		/* balance pci_get_slot */
		pci_dev_put(pdev);
	}
}

/* zpci_bus_scan_bus - Scan all configured zPCI functions on the bus
 * @zbus: the zbus to be scanned
 *
 * Enables and scans all PCI functions on the bus making them available to the
 * common PCI code. If there is no function 0 on the zbus nothing is scanned. If
 * a function does not have a slot yet because it was added to the zbus before
 * function 0 the slot is created. If a PCI function fails to be initialized
 * an error will be returned but attempts will still be made for all other
 * functions on the bus.
 *
 * Return: 0 on success, an error value otherwise
 */
int zpci_bus_scan_bus(struct zpci_bus *zbus)
{
	struct zpci_dev *zdev;
	int devfn, rc, ret = 0;

	if (!zbus->function[0])
		return 0;

	for (devfn = 0; devfn < ZPCI_FUNCTIONS_PER_BUS; devfn++) {
		zdev = zbus->function[devfn];
		if (zdev && zdev->state == ZPCI_FN_STATE_CONFIGURED) {
			rc = zpci_bus_prepare_device(zdev);
			if (rc)
				ret = -EIO;
		}
	}

	pci_lock_rescan_remove();
	pci_scan_child_bus(zbus->bus);
	pci_bus_add_devices(zbus->bus);
	pci_unlock_rescan_remove();

	return ret;
}

/* zpci_bus_scan_busses - Scan all registered busses
 *
 * Scan all available zbusses
 *
 */
void zpci_bus_scan_busses(void)
{
	struct zpci_bus *zbus = NULL;

	mutex_lock(&zbus_list_lock);
	list_for_each_entry(zbus, &zbus_list, bus_next) {
		zpci_bus_scan_bus(zbus);
		cond_resched();
	}
	mutex_unlock(&zbus_list_lock);
}

/* zpci_bus_create_pci_bus - Create the PCI bus associated with this zbus
 * @zbus: the zbus holding the zdevices
 * @f0: function 0 of the bus
 * @ops: the pci operations
 *
 * Function zero is taken as a parameter as this is used to determine the
 * domain, multifunction property and maximum bus speed of the entire bus.
 *
 * Return: 0 on success, an error code otherwise
 */
static int zpci_bus_create_pci_bus(struct zpci_bus *zbus, struct zpci_dev *f0, struct pci_ops *ops)
{
	struct pci_bus *bus;
	int domain;

	domain = zpci_alloc_domain((u16)f0->uid);
	if (domain < 0)
		return domain;

	zbus->domain_nr = domain;
	zbus->multifunction = f0->rid_available;
	zbus->max_bus_speed = f0->max_bus_speed;

	/*
	 * Note that the zbus->resources are taken over and zbus->resources
	 * is empty after a successful call
	 */
	bus = pci_create_root_bus(NULL, ZPCI_BUS_NR, ops, zbus, &zbus->resources);
	if (!bus) {
		zpci_free_domain(zbus->domain_nr);
		return -EFAULT;
	}

	zbus->bus = bus;
	pci_bus_add_devices(bus);

	return 0;
}

static void zpci_bus_release(struct kref *kref)
{
	struct zpci_bus *zbus = container_of(kref, struct zpci_bus, kref);

	if (zbus->bus) {
		pci_lock_rescan_remove();
		pci_stop_root_bus(zbus->bus);

		zpci_free_domain(zbus->domain_nr);
		pci_free_resource_list(&zbus->resources);

		pci_remove_root_bus(zbus->bus);
		pci_unlock_rescan_remove();
	}

	mutex_lock(&zbus_list_lock);
	list_del(&zbus->bus_next);
	mutex_unlock(&zbus_list_lock);
	kfree(zbus);
}

static void zpci_bus_put(struct zpci_bus *zbus)
{
	kref_put(&zbus->kref, zpci_bus_release);
}

static struct zpci_bus *zpci_bus_get(int pchid)
{
	struct zpci_bus *zbus;

	mutex_lock(&zbus_list_lock);
	list_for_each_entry(zbus, &zbus_list, bus_next) {
		if (pchid == zbus->pchid) {
			kref_get(&zbus->kref);
			goto out_unlock;
		}
	}
	zbus = NULL;
out_unlock:
	mutex_unlock(&zbus_list_lock);
	return zbus;
}

static struct zpci_bus *zpci_bus_alloc(int pchid)
{
	struct zpci_bus *zbus;

	zbus = kzalloc(sizeof(*zbus), GFP_KERNEL);
	if (!zbus)
		return NULL;

	zbus->pchid = pchid;
	INIT_LIST_HEAD(&zbus->bus_next);
	mutex_lock(&zbus_list_lock);
	list_add_tail(&zbus->bus_next, &zbus_list);
	mutex_unlock(&zbus_list_lock);

	kref_init(&zbus->kref);
	INIT_LIST_HEAD(&zbus->resources);

	zbus->bus_resource.start = 0;
	zbus->bus_resource.end = ZPCI_BUS_NR;
	zbus->bus_resource.flags = IORESOURCE_BUS;
	pci_add_resource(&zbus->resources, &zbus->bus_resource);

	return zbus;
}

void pcibios_bus_add_device(struct pci_dev *pdev)
{
	struct zpci_dev *zdev = to_zpci(pdev);

	/*
	 * With pdev->no_vf_scan the common PCI probing code does not
	 * perform PF/VF linking.
	 */
	if (zdev->vfn) {
		zpci_iov_setup_virtfn(zdev->zbus, pdev, zdev->vfn);
		pdev->no_command_memory = 1;
	}
}

/* zpci_bus_create_hotplug_slots - Add hotplug slot(s) for device added to bus
 * @zdev: the zPCI device that was newly added
 *
 * Add the hotplug slot(s) for the newly added PCI function. Normally this is
 * simply the slot for the function itself. If however we are adding the
 * function 0 on a zbus, it might be that we already registered functions on
 * that zbus but could not create their hotplug slots yet so add those now too.
 *
 * Return: 0 on success, an error code otherwise
 */
static int zpci_bus_create_hotplug_slots(struct zpci_dev *zdev)
{
	struct zpci_bus *zbus = zdev->zbus;
	int devfn, rc = 0;

	rc = zpci_init_slot(zdev);
	if (rc)
		return rc;
	zdev->has_hp_slot = 1;

	if (zdev->devfn == 0 && zbus->multifunction) {
		/* Now that function 0 is there we can finally create the
		 * hotplug slots for those functions with devfn != 0 that have
		 * been parked in zbus->function[] waiting for us to be able to
		 * create the PCI bus.
		 */
		for  (devfn = 1; devfn < ZPCI_FUNCTIONS_PER_BUS; devfn++) {
			zdev = zbus->function[devfn];
			if (zdev && !zdev->has_hp_slot) {
				rc = zpci_init_slot(zdev);
				if (rc)
					return rc;
				zdev->has_hp_slot = 1;
			}
		}

	}

	return rc;
}

static int zpci_bus_add_device(struct zpci_bus *zbus, struct zpci_dev *zdev)
{
	int rc = -EINVAL;

	zdev->zbus = zbus;
	if (zbus->function[zdev->devfn]) {
		pr_err("devfn %04x is already assigned\n", zdev->devfn);
		return rc;
	}
	zbus->function[zdev->devfn] = zdev;
	zpci_nb_devices++;

	if (zbus->bus) {
		if (zbus->multifunction && !zdev->rid_available) {
			WARN_ONCE(1, "rid_available not set for multifunction\n");
			goto error;
		}

		zpci_bus_create_hotplug_slots(zdev);
	} else {
		/* Hotplug slot will be created once function 0 appears */
		zbus->multifunction = 1;
	}

	return 0;

error:
	zbus->function[zdev->devfn] = NULL;
	zpci_nb_devices--;
	return rc;
}

int zpci_bus_device_register(struct zpci_dev *zdev, struct pci_ops *ops)
{
	struct zpci_bus *zbus = NULL;
	int rc = -EBADF;

	if (zpci_nb_devices == ZPCI_NR_DEVICES) {
		pr_warn("Adding PCI function %08x failed because the configured limit of %d is reached\n",
			zdev->fid, ZPCI_NR_DEVICES);
		return -ENOSPC;
	}

	if (zdev->devfn >= ZPCI_FUNCTIONS_PER_BUS)
		return -EINVAL;

	if (!s390_pci_no_rid && zdev->rid_available)
		zbus = zpci_bus_get(zdev->pchid);

	if (!zbus) {
		zbus = zpci_bus_alloc(zdev->pchid);
		if (!zbus)
			return -ENOMEM;
	}

	if (zdev->devfn == 0) {
		rc = zpci_bus_create_pci_bus(zbus, zdev, ops);
		if (rc)
			goto error;
	}

	rc = zpci_bus_add_device(zbus, zdev);
	if (rc)
		goto error;

	return 0;

error:
	pr_err("Adding PCI function %08x failed\n", zdev->fid);
	zpci_bus_put(zbus);
	return rc;
}

void zpci_bus_device_unregister(struct zpci_dev *zdev)
{
	struct zpci_bus *zbus = zdev->zbus;

	zpci_nb_devices--;
	zbus->function[zdev->devfn] = NULL;
	zpci_bus_put(zbus);
}
