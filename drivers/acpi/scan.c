// SPDX-License-Identifier: GPL-2.0-only
/*
 * scan.c - support for transforming the ACPI namespace into individual objects
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/acpi.h>
#include <linux/acpi_iort.h>
#include <linux/signal.h>
#include <linux/kthread.h>
#include <linux/dmi.h>
#include <linux/nls.h>
#include <linux/dma-map-ops.h>
#include <linux/platform_data/x86/apple.h>
#include <linux/pgtable.h>

#include "internal.h"

extern struct acpi_device *acpi_root;

#define ACPI_BUS_CLASS			"system_bus"
#define ACPI_BUS_HID			"LNXSYBUS"
#define ACPI_BUS_DEVICE_NAME		"System Bus"

#define ACPI_IS_ROOT_DEVICE(device)    (!(device)->parent)

#define INVALID_ACPI_HANDLE	((acpi_handle)empty_zero_page)

static const char *dummy_hid = "device";

static LIST_HEAD(acpi_dep_list);
static DEFINE_MUTEX(acpi_dep_list_lock);
LIST_HEAD(acpi_bus_id_list);
static DEFINE_MUTEX(acpi_scan_lock);
static LIST_HEAD(acpi_scan_handlers_list);
DEFINE_MUTEX(acpi_device_lock);
LIST_HEAD(acpi_wakeup_device_list);
static DEFINE_MUTEX(acpi_hp_context_lock);

/*
 * The UART device described by the SPCR table is the only object which needs
 * special-casing. Everything else is covered by ACPI namespace paths in STAO
 * table.
 */
static u64 spcr_uart_addr;

struct acpi_dep_data {
	struct list_head node;
	acpi_handle supplier;
	acpi_handle consumer;
};

void acpi_scan_lock_acquire(void)
{
	mutex_lock(&acpi_scan_lock);
}
EXPORT_SYMBOL_GPL(acpi_scan_lock_acquire);

void acpi_scan_lock_release(void)
{
	mutex_unlock(&acpi_scan_lock);
}
EXPORT_SYMBOL_GPL(acpi_scan_lock_release);

void acpi_lock_hp_context(void)
{
	mutex_lock(&acpi_hp_context_lock);
}

void acpi_unlock_hp_context(void)
{
	mutex_unlock(&acpi_hp_context_lock);
}

void acpi_initialize_hp_context(struct acpi_device *adev,
				struct acpi_hotplug_context *hp,
				int (*notify)(struct acpi_device *, u32),
				void (*uevent)(struct acpi_device *, u32))
{
	acpi_lock_hp_context();
	hp->notify = notify;
	hp->uevent = uevent;
	acpi_set_hp_context(adev, hp);
	acpi_unlock_hp_context();
}
EXPORT_SYMBOL_GPL(acpi_initialize_hp_context);

int acpi_scan_add_handler(struct acpi_scan_handler *handler)
{
	if (!handler)
		return -EINVAL;

	list_add_tail(&handler->list_node, &acpi_scan_handlers_list);
	return 0;
}

int acpi_scan_add_handler_with_hotplug(struct acpi_scan_handler *handler,
				       const char *hotplug_profile_name)
{
	int error;

	error = acpi_scan_add_handler(handler);
	if (error)
		return error;

	acpi_sysfs_add_hotplug_profile(&handler->hotplug, hotplug_profile_name);
	return 0;
}

bool acpi_scan_is_offline(struct acpi_device *adev, bool uevent)
{
	struct acpi_device_physical_node *pn;
	bool offline = true;
	char *envp[] = { "EVENT=offline", NULL };

	/*
	 * acpi_container_offline() calls this for all of the container's
	 * children under the container's physical_node_lock lock.
	 */
	mutex_lock_nested(&adev->physical_node_lock, SINGLE_DEPTH_NESTING);

	list_for_each_entry(pn, &adev->physical_node_list, node)
		if (device_supports_offline(pn->dev) && !pn->dev->offline) {
			if (uevent)
				kobject_uevent_env(&pn->dev->kobj, KOBJ_CHANGE, envp);

			offline = false;
			break;
		}

	mutex_unlock(&adev->physical_node_lock);
	return offline;
}

static acpi_status acpi_bus_offline(acpi_handle handle, u32 lvl, void *data,
				    void **ret_p)
{
	struct acpi_device *device = NULL;
	struct acpi_device_physical_node *pn;
	bool second_pass = (bool)data;
	acpi_status status = AE_OK;

	if (acpi_bus_get_device(handle, &device))
		return AE_OK;

	if (device->handler && !device->handler->hotplug.enabled) {
		*ret_p = &device->dev;
		return AE_SUPPORT;
	}

	mutex_lock(&device->physical_node_lock);

	list_for_each_entry(pn, &device->physical_node_list, node) {
		int ret;

		if (second_pass) {
			/* Skip devices offlined by the first pass. */
			if (pn->put_online)
				continue;
		} else {
			pn->put_online = false;
		}
		ret = device_offline(pn->dev);
		if (ret >= 0) {
			pn->put_online = !ret;
		} else {
			*ret_p = pn->dev;
			if (second_pass) {
				status = AE_ERROR;
				break;
			}
		}
	}

	mutex_unlock(&device->physical_node_lock);

	return status;
}

static acpi_status acpi_bus_online(acpi_handle handle, u32 lvl, void *data,
				   void **ret_p)
{
	struct acpi_device *device = NULL;
	struct acpi_device_physical_node *pn;

	if (acpi_bus_get_device(handle, &device))
		return AE_OK;

	mutex_lock(&device->physical_node_lock);

	list_for_each_entry(pn, &device->physical_node_list, node)
		if (pn->put_online) {
			device_online(pn->dev);
			pn->put_online = false;
		}

	mutex_unlock(&device->physical_node_lock);

	return AE_OK;
}

static int acpi_scan_try_to_offline(struct acpi_device *device)
{
	acpi_handle handle = device->handle;
	struct device *errdev = NULL;
	acpi_status status;

	/*
	 * Carry out two passes here and ignore errors in the first pass,
	 * because if the devices in question are memory blocks and
	 * CONFIG_MEMCG is set, one of the blocks may hold data structures
	 * that the other blocks depend on, but it is not known in advance which
	 * block holds them.
	 *
	 * If the first pass is successful, the second one isn't needed, though.
	 */
	status = acpi_walk_namespace(ACPI_TYPE_ANY, handle, ACPI_UINT32_MAX,
				     NULL, acpi_bus_offline, (void *)false,
				     (void **)&errdev);
	if (status == AE_SUPPORT) {
		dev_warn(errdev, "Offline disabled.\n");
		acpi_walk_namespace(ACPI_TYPE_ANY, handle, ACPI_UINT32_MAX,
				    acpi_bus_online, NULL, NULL, NULL);
		return -EPERM;
	}
	acpi_bus_offline(handle, 0, (void *)false, (void **)&errdev);
	if (errdev) {
		errdev = NULL;
		acpi_walk_namespace(ACPI_TYPE_ANY, handle, ACPI_UINT32_MAX,
				    NULL, acpi_bus_offline, (void *)true,
				    (void **)&errdev);
		if (!errdev)
			acpi_bus_offline(handle, 0, (void *)true,
					 (void **)&errdev);

		if (errdev) {
			dev_warn(errdev, "Offline failed.\n");
			acpi_bus_online(handle, 0, NULL, NULL);
			acpi_walk_namespace(ACPI_TYPE_ANY, handle,
					    ACPI_UINT32_MAX, acpi_bus_online,
					    NULL, NULL, NULL);
			return -EBUSY;
		}
	}
	return 0;
}

static int acpi_scan_hot_remove(struct acpi_device *device)
{
	acpi_handle handle = device->handle;
	unsigned long long sta;
	acpi_status status;

	if (device->handler && device->handler->hotplug.demand_offline) {
		if (!acpi_scan_is_offline(device, true))
			return -EBUSY;
	} else {
		int error = acpi_scan_try_to_offline(device);
		if (error)
			return error;
	}

	acpi_handle_debug(handle, "Ejecting\n");

	acpi_bus_trim(device);

	acpi_evaluate_lck(handle, 0);
	/*
	 * TBD: _EJD support.
	 */
	status = acpi_evaluate_ej0(handle);
	if (status == AE_NOT_FOUND)
		return -ENODEV;
	else if (ACPI_FAILURE(status))
		return -EIO;

	/*
	 * Verify if eject was indeed successful.  If not, log an error
	 * message.  No need to call _OST since _EJ0 call was made OK.
	 */
	status = acpi_evaluate_integer(handle, "_STA", NULL, &sta);
	if (ACPI_FAILURE(status)) {
		acpi_handle_warn(handle,
			"Status check after eject failed (0x%x)\n", status);
	} else if (sta & ACPI_STA_DEVICE_ENABLED) {
		acpi_handle_warn(handle,
			"Eject incomplete - status 0x%llx\n", sta);
	}

	return 0;
}

static int acpi_scan_device_not_present(struct acpi_device *adev)
{
	if (!acpi_device_enumerated(adev)) {
		dev_warn(&adev->dev, "Still not present\n");
		return -EALREADY;
	}
	acpi_bus_trim(adev);
	return 0;
}

static int acpi_scan_device_check(struct acpi_device *adev)
{
	int error;

	acpi_bus_get_status(adev);
	if (adev->status.present || adev->status.functional) {
		/*
		 * This function is only called for device objects for which
		 * matching scan handlers exist.  The only situation in which
		 * the scan handler is not attached to this device object yet
		 * is when the device has just appeared (either it wasn't
		 * present at all before or it was removed and then added
		 * again).
		 */
		if (adev->handler) {
			dev_warn(&adev->dev, "Already enumerated\n");
			return -EALREADY;
		}
		error = acpi_bus_scan(adev->handle);
		if (error) {
			dev_warn(&adev->dev, "Namespace scan failure\n");
			return error;
		}
		if (!adev->handler) {
			dev_warn(&adev->dev, "Enumeration failure\n");
			error = -ENODEV;
		}
	} else {
		error = acpi_scan_device_not_present(adev);
	}
	return error;
}

static int acpi_scan_bus_check(struct acpi_device *adev)
{
	struct acpi_scan_handler *handler = adev->handler;
	struct acpi_device *child;
	int error;

	acpi_bus_get_status(adev);
	if (!(adev->status.present || adev->status.functional)) {
		acpi_scan_device_not_present(adev);
		return 0;
	}
	if (handler && handler->hotplug.scan_dependent)
		return handler->hotplug.scan_dependent(adev);

	error = acpi_bus_scan(adev->handle);
	if (error) {
		dev_warn(&adev->dev, "Namespace scan failure\n");
		return error;
	}
	list_for_each_entry(child, &adev->children, node) {
		error = acpi_scan_bus_check(child);
		if (error)
			return error;
	}
	return 0;
}

static int acpi_generic_hotplug_event(struct acpi_device *adev, u32 type)
{
	switch (type) {
	case ACPI_NOTIFY_BUS_CHECK:
		return acpi_scan_bus_check(adev);
	case ACPI_NOTIFY_DEVICE_CHECK:
		return acpi_scan_device_check(adev);
	case ACPI_NOTIFY_EJECT_REQUEST:
	case ACPI_OST_EC_OSPM_EJECT:
		if (adev->handler && !adev->handler->hotplug.enabled) {
			dev_info(&adev->dev, "Eject disabled\n");
			return -EPERM;
		}
		acpi_evaluate_ost(adev->handle, ACPI_NOTIFY_EJECT_REQUEST,
				  ACPI_OST_SC_EJECT_IN_PROGRESS, NULL);
		return acpi_scan_hot_remove(adev);
	}
	return -EINVAL;
}

void acpi_device_hotplug(struct acpi_device *adev, u32 src)
{
	u32 ost_code = ACPI_OST_SC_NON_SPECIFIC_FAILURE;
	int error = -ENODEV;

	lock_device_hotplug();
	mutex_lock(&acpi_scan_lock);

	/*
	 * The device object's ACPI handle cannot become invalid as long as we
	 * are holding acpi_scan_lock, but it might have become invalid before
	 * that lock was acquired.
	 */
	if (adev->handle == INVALID_ACPI_HANDLE)
		goto err_out;

	if (adev->flags.is_dock_station) {
		error = dock_notify(adev, src);
	} else if (adev->flags.hotplug_notify) {
		error = acpi_generic_hotplug_event(adev, src);
	} else {
		int (*notify)(struct acpi_device *, u32);

		acpi_lock_hp_context();
		notify = adev->hp ? adev->hp->notify : NULL;
		acpi_unlock_hp_context();
		/*
		 * There may be additional notify handlers for device objects
		 * without the .event() callback, so ignore them here.
		 */
		if (notify)
			error = notify(adev, src);
		else
			goto out;
	}
	switch (error) {
	case 0:
		ost_code = ACPI_OST_SC_SUCCESS;
		break;
	case -EPERM:
		ost_code = ACPI_OST_SC_EJECT_NOT_SUPPORTED;
		break;
	case -EBUSY:
		ost_code = ACPI_OST_SC_DEVICE_BUSY;
		break;
	default:
		ost_code = ACPI_OST_SC_NON_SPECIFIC_FAILURE;
		break;
	}

 err_out:
	acpi_evaluate_ost(adev->handle, src, ost_code, NULL);

 out:
	acpi_bus_put_acpi_device(adev);
	mutex_unlock(&acpi_scan_lock);
	unlock_device_hotplug();
}

static void acpi_free_power_resources_lists(struct acpi_device *device)
{
	int i;

	if (device->wakeup.flags.valid)
		acpi_power_resources_list_free(&device->wakeup.resources);

	if (!device->power.flags.power_resources)
		return;

	for (i = ACPI_STATE_D0; i <= ACPI_STATE_D3_HOT; i++) {
		struct acpi_device_power_state *ps = &device->power.states[i];
		acpi_power_resources_list_free(&ps->resources);
	}
}

static void acpi_device_release(struct device *dev)
{
	struct acpi_device *acpi_dev = to_acpi_device(dev);

	acpi_free_properties(acpi_dev);
	acpi_free_pnp_ids(&acpi_dev->pnp);
	acpi_free_power_resources_lists(acpi_dev);
	kfree(acpi_dev);
}

static void acpi_device_del(struct acpi_device *device)
{
	struct acpi_device_bus_id *acpi_device_bus_id;

	mutex_lock(&acpi_device_lock);
	if (device->parent)
		list_del(&device->node);

	list_for_each_entry(acpi_device_bus_id, &acpi_bus_id_list, node)
		if (!strcmp(acpi_device_bus_id->bus_id,
			    acpi_device_hid(device))) {
			ida_simple_remove(&acpi_device_bus_id->instance_ida, device->pnp.instance_no);
			if (ida_is_empty(&acpi_device_bus_id->instance_ida)) {
				list_del(&acpi_device_bus_id->node);
				kfree_const(acpi_device_bus_id->bus_id);
				kfree(acpi_device_bus_id);
			}
			break;
		}

	list_del(&device->wakeup_list);
	mutex_unlock(&acpi_device_lock);

	acpi_power_add_remove_device(device, false);
	acpi_device_remove_files(device);
	if (device->remove)
		device->remove(device);

	device_del(&device->dev);
}

static BLOCKING_NOTIFIER_HEAD(acpi_reconfig_chain);

static LIST_HEAD(acpi_device_del_list);
static DEFINE_MUTEX(acpi_device_del_lock);

static void acpi_device_del_work_fn(struct work_struct *work_not_used)
{
	for (;;) {
		struct acpi_device *adev;

		mutex_lock(&acpi_device_del_lock);

		if (list_empty(&acpi_device_del_list)) {
			mutex_unlock(&acpi_device_del_lock);
			break;
		}
		adev = list_first_entry(&acpi_device_del_list,
					struct acpi_device, del_list);
		list_del(&adev->del_list);

		mutex_unlock(&acpi_device_del_lock);

		blocking_notifier_call_chain(&acpi_reconfig_chain,
					     ACPI_RECONFIG_DEVICE_REMOVE, adev);

		acpi_device_del(adev);
		/*
		 * Drop references to all power resources that might have been
		 * used by the device.
		 */
		acpi_power_transition(adev, ACPI_STATE_D3_COLD);
		acpi_dev_put(adev);
	}
}

/**
 * acpi_scan_drop_device - Drop an ACPI device object.
 * @handle: Handle of an ACPI namespace node, not used.
 * @context: Address of the ACPI device object to drop.
 *
 * This is invoked by acpi_ns_delete_node() during the removal of the ACPI
 * namespace node the device object pointed to by @context is attached to.
 *
 * The unregistration is carried out asynchronously to avoid running
 * acpi_device_del() under the ACPICA's namespace mutex and the list is used to
 * ensure the correct ordering (the device objects must be unregistered in the
 * same order in which the corresponding namespace nodes are deleted).
 */
static void acpi_scan_drop_device(acpi_handle handle, void *context)
{
	static DECLARE_WORK(work, acpi_device_del_work_fn);
	struct acpi_device *adev = context;

	mutex_lock(&acpi_device_del_lock);

	/*
	 * Use the ACPI hotplug workqueue which is ordered, so this work item
	 * won't run after any hotplug work items submitted subsequently.  That
	 * prevents attempts to register device objects identical to those being
	 * deleted from happening concurrently (such attempts result from
	 * hotplug events handled via the ACPI hotplug workqueue).  It also will
	 * run after all of the work items submitted previously, which helps
	 * those work items to ensure that they are not accessing stale device
	 * objects.
	 */
	if (list_empty(&acpi_device_del_list))
		acpi_queue_hotplug_work(&work);

	list_add_tail(&adev->del_list, &acpi_device_del_list);
	/* Make acpi_ns_validate_handle() return NULL for this handle. */
	adev->handle = INVALID_ACPI_HANDLE;

	mutex_unlock(&acpi_device_del_lock);
}

static struct acpi_device *handle_to_device(acpi_handle handle,
					    void (*callback)(void *))
{
	struct acpi_device *adev = NULL;
	acpi_status status;

	status = acpi_get_data_full(handle, acpi_scan_drop_device,
				    (void **)&adev, callback);
	if (ACPI_FAILURE(status) || !adev) {
		acpi_handle_debug(handle, "No context!\n");
		return NULL;
	}
	return adev;
}

int acpi_bus_get_device(acpi_handle handle, struct acpi_device **device)
{
	if (!device)
		return -EINVAL;

	*device = handle_to_device(handle, NULL);
	if (!*device)
		return -ENODEV;

	return 0;
}
EXPORT_SYMBOL(acpi_bus_get_device);

static void get_acpi_device(void *dev)
{
	acpi_dev_get(dev);
}

struct acpi_device *acpi_bus_get_acpi_device(acpi_handle handle)
{
	return handle_to_device(handle, get_acpi_device);
}

void acpi_bus_put_acpi_device(struct acpi_device *adev)
{
	acpi_dev_put(adev);
}

static struct acpi_device_bus_id *acpi_device_bus_id_match(const char *dev_id)
{
	struct acpi_device_bus_id *acpi_device_bus_id;

	/* Find suitable bus_id and instance number in acpi_bus_id_list. */
	list_for_each_entry(acpi_device_bus_id, &acpi_bus_id_list, node) {
		if (!strcmp(acpi_device_bus_id->bus_id, dev_id))
			return acpi_device_bus_id;
	}
	return NULL;
}

static int acpi_device_set_name(struct acpi_device *device,
				struct acpi_device_bus_id *acpi_device_bus_id)
{
	struct ida *instance_ida = &acpi_device_bus_id->instance_ida;
	int result;

	result = ida_simple_get(instance_ida, 0, ACPI_MAX_DEVICE_INSTANCES, GFP_KERNEL);
	if (result < 0)
		return result;

	device->pnp.instance_no = result;
	dev_set_name(&device->dev, "%s:%02x", acpi_device_bus_id->bus_id, result);
	return 0;
}

int acpi_device_add(struct acpi_device *device,
		    void (*release)(struct device *))
{
	struct acpi_device_bus_id *acpi_device_bus_id;
	int result;

	if (device->handle) {
		acpi_status status;

		status = acpi_attach_data(device->handle, acpi_scan_drop_device,
					  device);
		if (ACPI_FAILURE(status)) {
			acpi_handle_err(device->handle,
					"Unable to attach device data\n");
			return -ENODEV;
		}
	}

	/*
	 * Linkage
	 * -------
	 * Link this device to its parent and siblings.
	 */
	INIT_LIST_HEAD(&device->children);
	INIT_LIST_HEAD(&device->node);
	INIT_LIST_HEAD(&device->wakeup_list);
	INIT_LIST_HEAD(&device->physical_node_list);
	INIT_LIST_HEAD(&device->del_list);
	mutex_init(&device->physical_node_lock);

	mutex_lock(&acpi_device_lock);

	acpi_device_bus_id = acpi_device_bus_id_match(acpi_device_hid(device));
	if (acpi_device_bus_id) {
		result = acpi_device_set_name(device, acpi_device_bus_id);
		if (result)
			goto err_unlock;
	} else {
		acpi_device_bus_id = kzalloc(sizeof(*acpi_device_bus_id),
					     GFP_KERNEL);
		if (!acpi_device_bus_id) {
			result = -ENOMEM;
			goto err_unlock;
		}
		acpi_device_bus_id->bus_id =
			kstrdup_const(acpi_device_hid(device), GFP_KERNEL);
		if (!acpi_device_bus_id->bus_id) {
			kfree(acpi_device_bus_id);
			result = -ENOMEM;
			goto err_unlock;
		}

		ida_init(&acpi_device_bus_id->instance_ida);

		result = acpi_device_set_name(device, acpi_device_bus_id);
		if (result) {
			kfree_const(acpi_device_bus_id->bus_id);
			kfree(acpi_device_bus_id);
			goto err_unlock;
		}

		list_add_tail(&acpi_device_bus_id->node, &acpi_bus_id_list);
	}

	if (device->parent)
		list_add_tail(&device->node, &device->parent->children);

	if (device->wakeup.flags.valid)
		list_add_tail(&device->wakeup_list, &acpi_wakeup_device_list);

	mutex_unlock(&acpi_device_lock);

	if (device->parent)
		device->dev.parent = &device->parent->dev;

	device->dev.bus = &acpi_bus_type;
	device->dev.release = release;
	result = device_add(&device->dev);
	if (result) {
		dev_err(&device->dev, "Error registering device\n");
		goto err;
	}

	result = acpi_device_setup_files(device);
	if (result)
		printk(KERN_ERR PREFIX "Error creating sysfs interface for device %s\n",
		       dev_name(&device->dev));

	return 0;

err:
	mutex_lock(&acpi_device_lock);

	if (device->parent)
		list_del(&device->node);

	list_del(&device->wakeup_list);

err_unlock:
	mutex_unlock(&acpi_device_lock);

	acpi_detach_data(device->handle, acpi_scan_drop_device);

	return result;
}

/* --------------------------------------------------------------------------
                                 Device Enumeration
   -------------------------------------------------------------------------- */
static bool acpi_info_matches_ids(struct acpi_device_info *info,
				  const char * const ids[])
{
	struct acpi_pnp_device_id_list *cid_list = NULL;
	int i, index;

	if (!(info->valid & ACPI_VALID_HID))
		return false;

	index = match_string(ids, -1, info->hardware_id.string);
	if (index >= 0)
		return true;

	if (info->valid & ACPI_VALID_CID)
		cid_list = &info->compatible_id_list;

	if (!cid_list)
		return false;

	for (i = 0; i < cid_list->count; i++) {
		index = match_string(ids, -1, cid_list->ids[i].string);
		if (index >= 0)
			return true;
	}

	return false;
}

/* List of HIDs for which we ignore matching ACPI devices, when checking _DEP lists. */
static const char * const acpi_ignore_dep_ids[] = {
	"PNP0D80", /* Windows-compatible System Power Management Controller */
	"INT33BD", /* Intel Baytrail Mailbox Device */
	NULL
};

static struct acpi_device *acpi_bus_get_parent(acpi_handle handle)
{
	struct acpi_device *device = NULL;
	acpi_status status;

	/*
	 * Fixed hardware devices do not appear in the namespace and do not
	 * have handles, but we fabricate acpi_devices for them, so we have
	 * to deal with them specially.
	 */
	if (!handle)
		return acpi_root;

	do {
		status = acpi_get_parent(handle, &handle);
		if (ACPI_FAILURE(status))
			return status == AE_NULL_ENTRY ? NULL : acpi_root;
	} while (acpi_bus_get_device(handle, &device));
	return device;
}

acpi_status
acpi_bus_get_ejd(acpi_handle handle, acpi_handle *ejd)
{
	acpi_status status;
	acpi_handle tmp;
	struct acpi_buffer buffer = {ACPI_ALLOCATE_BUFFER, NULL};
	union acpi_object *obj;

	status = acpi_get_handle(handle, "_EJD", &tmp);
	if (ACPI_FAILURE(status))
		return status;

	status = acpi_evaluate_object(handle, "_EJD", NULL, &buffer);
	if (ACPI_SUCCESS(status)) {
		obj = buffer.pointer;
		status = acpi_get_handle(ACPI_ROOT_OBJECT, obj->string.pointer,
					 ejd);
		kfree(buffer.pointer);
	}
	return status;
}
EXPORT_SYMBOL_GPL(acpi_bus_get_ejd);

static int acpi_bus_extract_wakeup_device_power_package(struct acpi_device *dev)
{
	acpi_handle handle = dev->handle;
	struct acpi_device_wakeup *wakeup = &dev->wakeup;
	struct acpi_buffer buffer = { ACPI_ALLOCATE_BUFFER, NULL };
	union acpi_object *package = NULL;
	union acpi_object *element = NULL;
	acpi_status status;
	int err = -ENODATA;

	INIT_LIST_HEAD(&wakeup->resources);

	/* _PRW */
	status = acpi_evaluate_object(handle, "_PRW", NULL, &buffer);
	if (ACPI_FAILURE(status)) {
		acpi_handle_info(handle, "_PRW evaluation failed: %s\n",
				 acpi_format_exception(status));
		return err;
	}

	package = (union acpi_object *)buffer.pointer;

	if (!package || package->package.count < 2)
		goto out;

	element = &(package->package.elements[0]);
	if (!element)
		goto out;

	if (element->type == ACPI_TYPE_PACKAGE) {
		if ((element->package.count < 2) ||
		    (element->package.elements[0].type !=
		     ACPI_TYPE_LOCAL_REFERENCE)
		    || (element->package.elements[1].type != ACPI_TYPE_INTEGER))
			goto out;

		wakeup->gpe_device =
		    element->package.elements[0].reference.handle;
		wakeup->gpe_number =
		    (u32) element->package.elements[1].integer.value;
	} else if (element->type == ACPI_TYPE_INTEGER) {
		wakeup->gpe_device = NULL;
		wakeup->gpe_number = element->integer.value;
	} else {
		goto out;
	}

	element = &(package->package.elements[1]);
	if (element->type != ACPI_TYPE_INTEGER)
		goto out;

	wakeup->sleep_state = element->integer.value;

	err = acpi_extract_power_resources(package, 2, &wakeup->resources);
	if (err)
		goto out;

	if (!list_empty(&wakeup->resources)) {
		int sleep_state;

		err = acpi_power_wakeup_list_init(&wakeup->resources,
						  &sleep_state);
		if (err) {
			acpi_handle_warn(handle, "Retrieving current states "
					 "of wakeup power resources failed\n");
			acpi_power_resources_list_free(&wakeup->resources);
			goto out;
		}
		if (sleep_state < wakeup->sleep_state) {
			acpi_handle_warn(handle, "Overriding _PRW sleep state "
					 "(S%d) by S%d from power resources\n",
					 (int)wakeup->sleep_state, sleep_state);
			wakeup->sleep_state = sleep_state;
		}
	}

 out:
	kfree(buffer.pointer);
	return err;
}

static bool acpi_wakeup_gpe_init(struct acpi_device *device)
{
	static const struct acpi_device_id button_device_ids[] = {
		{"PNP0C0C", 0},		/* Power button */
		{"PNP0C0D", 0},		/* Lid */
		{"PNP0C0E", 0},		/* Sleep button */
		{"", 0},
	};
	struct acpi_device_wakeup *wakeup = &device->wakeup;
	acpi_status status;

	wakeup->flags.notifier_present = 0;

	/* Power button, Lid switch always enable wakeup */
	if (!acpi_match_device_ids(device, button_device_ids)) {
		if (!acpi_match_device_ids(device, &button_device_ids[1])) {
			/* Do not use Lid/sleep button for S5 wakeup */
			if (wakeup->sleep_state == ACPI_STATE_S5)
				wakeup->sleep_state = ACPI_STATE_S4;
		}
		acpi_mark_gpe_for_wake(wakeup->gpe_device, wakeup->gpe_number);
		device_set_wakeup_capable(&device->dev, true);
		return true;
	}

	status = acpi_setup_gpe_for_wake(device->handle, wakeup->gpe_device,
					 wakeup->gpe_number);
	return ACPI_SUCCESS(status);
}

static void acpi_bus_get_wakeup_device_flags(struct acpi_device *device)
{
	int err;

	/* Presence of _PRW indicates wake capable */
	if (!acpi_has_method(device->handle, "_PRW"))
		return;

	err = acpi_bus_extract_wakeup_device_power_package(device);
	if (err) {
		dev_err(&device->dev, "Unable to extract wakeup power resources");
		return;
	}

	device->wakeup.flags.valid = acpi_wakeup_gpe_init(device);
	device->wakeup.prepare_count = 0;
	/*
	 * Call _PSW/_DSW object to disable its ability to wake the sleeping
	 * system for the ACPI device with the _PRW object.
	 * The _PSW object is deprecated in ACPI 3.0 and is replaced by _DSW.
	 * So it is necessary to call _DSW object first. Only when it is not
	 * present will the _PSW object used.
	 */
	err = acpi_device_sleep_wake(device, 0, 0, 0);
	if (err)
		pr_debug("error in _DSW or _PSW evaluation\n");
}

static void acpi_bus_init_power_state(struct acpi_device *device, int state)
{
	struct acpi_device_power_state *ps = &device->power.states[state];
	char pathname[5] = { '_', 'P', 'R', '0' + state, '\0' };
	struct acpi_buffer buffer = { ACPI_ALLOCATE_BUFFER, NULL };
	acpi_status status;

	INIT_LIST_HEAD(&ps->resources);

	/* Evaluate "_PRx" to get referenced power resources */
	status = acpi_evaluate_object(device->handle, pathname, NULL, &buffer);
	if (ACPI_SUCCESS(status)) {
		union acpi_object *package = buffer.pointer;

		if (buffer.length && package
		    && package->type == ACPI_TYPE_PACKAGE
		    && package->package.count)
			acpi_extract_power_resources(package, 0, &ps->resources);

		ACPI_FREE(buffer.pointer);
	}

	/* Evaluate "_PSx" to see if we can do explicit sets */
	pathname[2] = 'S';
	if (acpi_has_method(device->handle, pathname))
		ps->flags.explicit_set = 1;

	/* State is valid if there are means to put the device into it. */
	if (!list_empty(&ps->resources) || ps->flags.explicit_set)
		ps->flags.valid = 1;

	ps->power = -1;		/* Unknown - driver assigned */
	ps->latency = -1;	/* Unknown - driver assigned */
}

static void acpi_bus_get_power_flags(struct acpi_device *device)
{
	u32 i;

	/* Presence of _PS0|_PR0 indicates 'power manageable' */
	if (!acpi_has_method(device->handle, "_PS0") &&
	    !acpi_has_method(device->handle, "_PR0"))
		return;

	device->flags.power_manageable = 1;

	/*
	 * Power Management Flags
	 */
	if (acpi_has_method(device->handle, "_PSC"))
		device->power.flags.explicit_get = 1;

	if (acpi_has_method(device->handle, "_IRC"))
		device->power.flags.inrush_current = 1;

	if (acpi_has_method(device->handle, "_DSW"))
		device->power.flags.dsw_present = 1;

	/*
	 * Enumerate supported power management states
	 */
	for (i = ACPI_STATE_D0; i <= ACPI_STATE_D3_HOT; i++)
		acpi_bus_init_power_state(device, i);

	INIT_LIST_HEAD(&device->power.states[ACPI_STATE_D3_COLD].resources);

	/* Set the defaults for D0 and D3hot (always supported). */
	device->power.states[ACPI_STATE_D0].flags.valid = 1;
	device->power.states[ACPI_STATE_D0].power = 100;
	device->power.states[ACPI_STATE_D3_HOT].flags.valid = 1;

	/*
	 * Use power resources only if the D0 list of them is populated, because
	 * some platforms may provide _PR3 only to indicate D3cold support and
	 * in those cases the power resources list returned by it may be bogus.
	 */
	if (!list_empty(&device->power.states[ACPI_STATE_D0].resources)) {
		device->power.flags.power_resources = 1;
		/*
		 * D3cold is supported if the D3hot list of power resources is
		 * not empty.
		 */
		if (!list_empty(&device->power.states[ACPI_STATE_D3_HOT].resources))
			device->power.states[ACPI_STATE_D3_COLD].flags.valid = 1;
	}

	if (acpi_bus_init_power(device))
		device->flags.power_manageable = 0;
}

static void acpi_bus_get_flags(struct acpi_device *device)
{
	/* Presence of _STA indicates 'dynamic_status' */
	if (acpi_has_method(device->handle, "_STA"))
		device->flags.dynamic_status = 1;

	/* Presence of _RMV indicates 'removable' */
	if (acpi_has_method(device->handle, "_RMV"))
		device->flags.removable = 1;

	/* Presence of _EJD|_EJ0 indicates 'ejectable' */
	if (acpi_has_method(device->handle, "_EJD") ||
	    acpi_has_method(device->handle, "_EJ0"))
		device->flags.ejectable = 1;
}

static void acpi_device_get_busid(struct acpi_device *device)
{
	char bus_id[5] = { '?', 0 };
	struct acpi_buffer buffer = { sizeof(bus_id), bus_id };
	int i = 0;

	/*
	 * Bus ID
	 * ------
	 * The device's Bus ID is simply the object name.
	 * TBD: Shouldn't this value be unique (within the ACPI namespace)?
	 */
	if (ACPI_IS_ROOT_DEVICE(device)) {
		strcpy(device->pnp.bus_id, "ACPI");
		return;
	}

	switch (device->device_type) {
	case ACPI_BUS_TYPE_POWER_BUTTON:
		strcpy(device->pnp.bus_id, "PWRF");
		break;
	case ACPI_BUS_TYPE_SLEEP_BUTTON:
		strcpy(device->pnp.bus_id, "SLPF");
		break;
	case ACPI_BUS_TYPE_ECDT_EC:
		strcpy(device->pnp.bus_id, "ECDT");
		break;
	default:
		acpi_get_name(device->handle, ACPI_SINGLE_NAME, &buffer);
		/* Clean up trailing underscores (if any) */
		for (i = 3; i > 1; i--) {
			if (bus_id[i] == '_')
				bus_id[i] = '\0';
			else
				break;
		}
		strcpy(device->pnp.bus_id, bus_id);
		break;
	}
}

/*
 * acpi_ata_match - see if an acpi object is an ATA device
 *
 * If an acpi object has one of the ACPI ATA methods defined,
 * then we can safely call it an ATA device.
 */
bool acpi_ata_match(acpi_handle handle)
{
	return acpi_has_method(handle, "_GTF") ||
	       acpi_has_method(handle, "_GTM") ||
	       acpi_has_method(handle, "_STM") ||
	       acpi_has_method(handle, "_SDD");
}

/*
 * acpi_bay_match - see if an acpi object is an ejectable driver bay
 *
 * If an acpi object is ejectable and has one of the ACPI ATA methods defined,
 * then we can safely call it an ejectable drive bay
 */
bool acpi_bay_match(acpi_handle handle)
{
	acpi_handle phandle;

	if (!acpi_has_method(handle, "_EJ0"))
		return false;
	if (acpi_ata_match(handle))
		return true;
	if (ACPI_FAILURE(acpi_get_parent(handle, &phandle)))
		return false;

	return acpi_ata_match(phandle);
}

bool acpi_device_is_battery(struct acpi_device *adev)
{
	struct acpi_hardware_id *hwid;

	list_for_each_entry(hwid, &adev->pnp.ids, list)
		if (!strcmp("PNP0C0A", hwid->id))
			return true;

	return false;
}

static bool is_ejectable_bay(struct acpi_device *adev)
{
	acpi_handle handle = adev->handle;

	if (acpi_has_method(handle, "_EJ0") && acpi_device_is_battery(adev))
		return true;

	return acpi_bay_match(handle);
}

/*
 * acpi_dock_match - see if an acpi object has a _DCK method
 */
bool acpi_dock_match(acpi_handle handle)
{
	return acpi_has_method(handle, "_DCK");
}

static acpi_status
acpi_backlight_cap_match(acpi_handle handle, u32 level, void *context,
			  void **return_value)
{
	long *cap = context;

	if (acpi_has_method(handle, "_BCM") &&
	    acpi_has_method(handle, "_BCL")) {
		acpi_handle_debug(handle, "Found generic backlight support\n");
		*cap |= ACPI_VIDEO_BACKLIGHT;
		/* We have backlight support, no need to scan further */
		return AE_CTRL_TERMINATE;
	}
	return 0;
}

/* Returns true if the ACPI object is a video device which can be
 * handled by video.ko.
 * The device will get a Linux specific CID added in scan.c to
 * identify the device as an ACPI graphics device
 * Be aware that the graphics device may not be physically present
 * Use acpi_video_get_capabilities() to detect general ACPI video
 * capabilities of present cards
 */
long acpi_is_video_device(acpi_handle handle)
{
	long video_caps = 0;

	/* Is this device able to support video switching ? */
	if (acpi_has_method(handle, "_DOD") || acpi_has_method(handle, "_DOS"))
		video_caps |= ACPI_VIDEO_OUTPUT_SWITCHING;

	/* Is this device able to retrieve a video ROM ? */
	if (acpi_has_method(handle, "_ROM"))
		video_caps |= ACPI_VIDEO_ROM_AVAILABLE;

	/* Is this device able to configure which video head to be POSTed ? */
	if (acpi_has_method(handle, "_VPO") &&
	    acpi_has_method(handle, "_GPD") &&
	    acpi_has_method(handle, "_SPD"))
		video_caps |= ACPI_VIDEO_DEVICE_POSTING;

	/* Only check for backlight functionality if one of the above hit. */
	if (video_caps)
		acpi_walk_namespace(ACPI_TYPE_DEVICE, handle,
				    ACPI_UINT32_MAX, acpi_backlight_cap_match, NULL,
				    &video_caps, NULL);

	return video_caps;
}
EXPORT_SYMBOL(acpi_is_video_device);

const char *acpi_device_hid(struct acpi_device *device)
{
	struct acpi_hardware_id *hid;

	if (list_empty(&device->pnp.ids))
		return dummy_hid;

	hid = list_first_entry(&device->pnp.ids, struct acpi_hardware_id, list);
	return hid->id;
}
EXPORT_SYMBOL(acpi_device_hid);

static void acpi_add_id(struct acpi_device_pnp *pnp, const char *dev_id)
{
	struct acpi_hardware_id *id;

	id = kmalloc(sizeof(*id), GFP_KERNEL);
	if (!id)
		return;

	id->id = kstrdup_const(dev_id, GFP_KERNEL);
	if (!id->id) {
		kfree(id);
		return;
	}

	list_add_tail(&id->list, &pnp->ids);
	pnp->type.hardware_id = 1;
}

/*
 * Old IBM workstations have a DSDT bug wherein the SMBus object
 * lacks the SMBUS01 HID and the methods do not have the necessary "_"
 * prefix.  Work around this.
 */
static bool acpi_ibm_smbus_match(acpi_handle handle)
{
	char node_name[ACPI_PATH_SEGMENT_LENGTH];
	struct acpi_buffer path = { sizeof(node_name), node_name };

	if (!dmi_name_in_vendors("IBM"))
		return false;

	/* Look for SMBS object */
	if (ACPI_FAILURE(acpi_get_name(handle, ACPI_SINGLE_NAME, &path)) ||
	    strcmp("SMBS", path.pointer))
		return false;

	/* Does it have the necessary (but misnamed) methods? */
	if (acpi_has_method(handle, "SBI") &&
	    acpi_has_method(handle, "SBR") &&
	    acpi_has_method(handle, "SBW"))
		return true;

	return false;
}

static bool acpi_object_is_system_bus(acpi_handle handle)
{
	acpi_handle tmp;

	if (ACPI_SUCCESS(acpi_get_handle(NULL, "\\_SB", &tmp)) &&
	    tmp == handle)
		return true;
	if (ACPI_SUCCESS(acpi_get_handle(NULL, "\\_TZ", &tmp)) &&
	    tmp == handle)
		return true;

	return false;
}

static void acpi_set_pnp_ids(acpi_handle handle, struct acpi_device_pnp *pnp,
			     int device_type)
{
	struct acpi_device_info *info = NULL;
	struct acpi_pnp_device_id_list *cid_list;
	int i;

	switch (device_type) {
	case ACPI_BUS_TYPE_DEVICE:
		if (handle == ACPI_ROOT_OBJECT) {
			acpi_add_id(pnp, ACPI_SYSTEM_HID);
			break;
		}

		acpi_get_object_info(handle, &info);
		if (!info) {
			pr_err(PREFIX "%s: Error reading device info\n",
					__func__);
			return;
		}

		if (info->valid & ACPI_VALID_HID) {
			acpi_add_id(pnp, info->hardware_id.string);
			pnp->type.platform_id = 1;
		}
		if (info->valid & ACPI_VALID_CID) {
			cid_list = &info->compatible_id_list;
			for (i = 0; i < cid_list->count; i++)
				acpi_add_id(pnp, cid_list->ids[i].string);
		}
		if (info->valid & ACPI_VALID_ADR) {
			pnp->bus_address = info->address;
			pnp->type.bus_address = 1;
		}
		if (info->valid & ACPI_VALID_UID)
			pnp->unique_id = kstrdup(info->unique_id.string,
							GFP_KERNEL);
		if (info->valid & ACPI_VALID_CLS)
			acpi_add_id(pnp, info->class_code.string);

		kfree(info);

		/*
		 * Some devices don't reliably have _HIDs & _CIDs, so add
		 * synthetic HIDs to make sure drivers can find them.
		 */
		if (acpi_is_video_device(handle))
			acpi_add_id(pnp, ACPI_VIDEO_HID);
		else if (acpi_bay_match(handle))
			acpi_add_id(pnp, ACPI_BAY_HID);
		else if (acpi_dock_match(handle))
			acpi_add_id(pnp, ACPI_DOCK_HID);
		else if (acpi_ibm_smbus_match(handle))
			acpi_add_id(pnp, ACPI_SMBUS_IBM_HID);
		else if (list_empty(&pnp->ids) &&
			 acpi_object_is_system_bus(handle)) {
			/* \_SB, \_TZ, LNXSYBUS */
			acpi_add_id(pnp, ACPI_BUS_HID);
			strcpy(pnp->device_name, ACPI_BUS_DEVICE_NAME);
			strcpy(pnp->device_class, ACPI_BUS_CLASS);
		}

		break;
	case ACPI_BUS_TYPE_POWER:
		acpi_add_id(pnp, ACPI_POWER_HID);
		break;
	case ACPI_BUS_TYPE_PROCESSOR:
		acpi_add_id(pnp, ACPI_PROCESSOR_OBJECT_HID);
		break;
	case ACPI_BUS_TYPE_THERMAL:
		acpi_add_id(pnp, ACPI_THERMAL_HID);
		break;
	case ACPI_BUS_TYPE_POWER_BUTTON:
		acpi_add_id(pnp, ACPI_BUTTON_HID_POWERF);
		break;
	case ACPI_BUS_TYPE_SLEEP_BUTTON:
		acpi_add_id(pnp, ACPI_BUTTON_HID_SLEEPF);
		break;
	case ACPI_BUS_TYPE_ECDT_EC:
		acpi_add_id(pnp, ACPI_ECDT_HID);
		break;
	}
}

void acpi_free_pnp_ids(struct acpi_device_pnp *pnp)
{
	struct acpi_hardware_id *id, *tmp;

	list_for_each_entry_safe(id, tmp, &pnp->ids, list) {
		kfree_const(id->id);
		kfree(id);
	}
	kfree(pnp->unique_id);
}

/**
 * acpi_dma_supported - Check DMA support for the specified device.
 * @adev: The pointer to acpi device
 *
 * Return false if DMA is not supported. Otherwise, return true
 */
bool acpi_dma_supported(struct acpi_device *adev)
{
	if (!adev)
		return false;

	if (adev->flags.cca_seen)
		return true;

	/*
	* Per ACPI 6.0 sec 6.2.17, assume devices can do cache-coherent
	* DMA on "Intel platforms".  Presumably that includes all x86 and
	* ia64, and other arches will set CONFIG_ACPI_CCA_REQUIRED=y.
	*/
	if (!IS_ENABLED(CONFIG_ACPI_CCA_REQUIRED))
		return true;

	return false;
}

/**
 * acpi_get_dma_attr - Check the supported DMA attr for the specified device.
 * @adev: The pointer to acpi device
 *
 * Return enum dev_dma_attr.
 */
enum dev_dma_attr acpi_get_dma_attr(struct acpi_device *adev)
{
	if (!acpi_dma_supported(adev))
		return DEV_DMA_NOT_SUPPORTED;

	if (adev->flags.coherent_dma)
		return DEV_DMA_COHERENT;
	else
		return DEV_DMA_NON_COHERENT;
}

/**
 * acpi_dma_get_range() - Get device DMA parameters.
 *
 * @dev: device to configure
 * @dma_addr: pointer device DMA address result
 * @offset: pointer to the DMA offset result
 * @size: pointer to DMA range size result
 *
 * Evaluate DMA regions and return respectively DMA region start, offset
 * and size in dma_addr, offset and size on parsing success; it does not
 * update the passed in values on failure.
 *
 * Return 0 on success, < 0 on failure.
 */
int acpi_dma_get_range(struct device *dev, u64 *dma_addr, u64 *offset,
		       u64 *size)
{
	struct acpi_device *adev;
	LIST_HEAD(list);
	struct resource_entry *rentry;
	int ret;
	struct device *dma_dev = dev;
	u64 len, dma_start = U64_MAX, dma_end = 0, dma_offset = 0;

	/*
	 * Walk the device tree chasing an ACPI companion with a _DMA
	 * object while we go. Stop if we find a device with an ACPI
	 * companion containing a _DMA method.
	 */
	do {
		adev = ACPI_COMPANION(dma_dev);
		if (adev && acpi_has_method(adev->handle, METHOD_NAME__DMA))
			break;

		dma_dev = dma_dev->parent;
	} while (dma_dev);

	if (!dma_dev)
		return -ENODEV;

	if (!acpi_has_method(adev->handle, METHOD_NAME__CRS)) {
		acpi_handle_warn(adev->handle, "_DMA is valid only if _CRS is present\n");
		return -EINVAL;
	}

	ret = acpi_dev_get_dma_resources(adev, &list);
	if (ret > 0) {
		list_for_each_entry(rentry, &list, node) {
			if (dma_offset && rentry->offset != dma_offset) {
				ret = -EINVAL;
				dev_warn(dma_dev, "Can't handle multiple windows with different offsets\n");
				goto out;
			}
			dma_offset = rentry->offset;

			/* Take lower and upper limits */
			if (rentry->res->start < dma_start)
				dma_start = rentry->res->start;
			if (rentry->res->end > dma_end)
				dma_end = rentry->res->end;
		}

		if (dma_start >= dma_end) {
			ret = -EINVAL;
			dev_dbg(dma_dev, "Invalid DMA regions configuration\n");
			goto out;
		}

		*dma_addr = dma_start - dma_offset;
		len = dma_end - dma_start;
		*size = max(len, len + 1);
		*offset = dma_offset;
	}
 out:
	acpi_dev_free_resource_list(&list);

	return ret >= 0 ? 0 : ret;
}

/**
 * acpi_dma_configure_id - Set-up DMA configuration for the device.
 * @dev: The pointer to the device
 * @attr: device dma attributes
 * @input_id: input device id const value pointer
 */
int acpi_dma_configure_id(struct device *dev, enum dev_dma_attr attr,
			  const u32 *input_id)
{
	const struct iommu_ops *iommu;
	u64 dma_addr = 0, size = 0;

	if (attr == DEV_DMA_NOT_SUPPORTED) {
		set_dma_ops(dev, &dma_dummy_ops);
		return 0;
	}

	iort_dma_setup(dev, &dma_addr, &size);

	iommu = iort_iommu_configure_id(dev, input_id);
	if (PTR_ERR(iommu) == -EPROBE_DEFER)
		return -EPROBE_DEFER;

	arch_setup_dma_ops(dev, dma_addr, size,
				iommu, attr == DEV_DMA_COHERENT);

	return 0;
}
EXPORT_SYMBOL_GPL(acpi_dma_configure_id);

static void acpi_init_coherency(struct acpi_device *adev)
{
	unsigned long long cca = 0;
	acpi_status status;
	struct acpi_device *parent = adev->parent;

	if (parent && parent->flags.cca_seen) {
		/*
		 * From ACPI spec, OSPM will ignore _CCA if an ancestor
		 * already saw one.
		 */
		adev->flags.cca_seen = 1;
		cca = parent->flags.coherent_dma;
	} else {
		status = acpi_evaluate_integer(adev->handle, "_CCA",
					       NULL, &cca);
		if (ACPI_SUCCESS(status))
			adev->flags.cca_seen = 1;
		else if (!IS_ENABLED(CONFIG_ACPI_CCA_REQUIRED))
			/*
			 * If architecture does not specify that _CCA is
			 * required for DMA-able devices (e.g. x86),
			 * we default to _CCA=1.
			 */
			cca = 1;
		else
			acpi_handle_debug(adev->handle,
					  "ACPI device is missing _CCA.\n");
	}

	adev->flags.coherent_dma = cca;
}

static int acpi_check_serial_bus_slave(struct acpi_resource *ares, void *data)
{
	bool *is_serial_bus_slave_p = data;

	if (ares->type != ACPI_RESOURCE_TYPE_SERIAL_BUS)
		return 1;

	*is_serial_bus_slave_p = true;

	 /* no need to do more checking */
	return -1;
}

static bool acpi_is_indirect_io_slave(struct acpi_device *device)
{
	struct acpi_device *parent = device->parent;
	static const struct acpi_device_id indirect_io_hosts[] = {
		{"HISI0191", 0},
		{}
	};

	return parent && !acpi_match_device_ids(parent, indirect_io_hosts);
}

static bool acpi_device_enumeration_by_parent(struct acpi_device *device)
{
	struct list_head resource_list;
	bool is_serial_bus_slave = false;
	/*
	 * These devices have multiple I2cSerialBus resources and an i2c-client
	 * must be instantiated for each, each with its own i2c_device_id.
	 * Normally we only instantiate an i2c-client for the first resource,
	 * using the ACPI HID as id. These special cases are handled by the
	 * drivers/platform/x86/i2c-multi-instantiate.c driver, which knows
	 * which i2c_device_id to use for each resource.
	 */
	static const struct acpi_device_id i2c_multi_instantiate_ids[] = {
		{"BSG1160", },
		{"BSG2150", },
		{"INT33FE", },
		{"INT3515", },
		{}
	};

	if (acpi_is_indirect_io_slave(device))
		return true;

	/* Macs use device properties in lieu of _CRS resources */
	if (x86_apple_machine &&
	    (fwnode_property_present(&device->fwnode, "spiSclkPeriod") ||
	     fwnode_property_present(&device->fwnode, "i2cAddress") ||
	     fwnode_property_present(&device->fwnode, "baud")))
		return true;

	/* Instantiate a pdev for the i2c-multi-instantiate drv to bind to */
	if (!acpi_match_device_ids(device, i2c_multi_instantiate_ids))
		return false;

	INIT_LIST_HEAD(&resource_list);
	acpi_dev_get_resources(device, &resource_list,
			       acpi_check_serial_bus_slave,
			       &is_serial_bus_slave);
	acpi_dev_free_resource_list(&resource_list);

	return is_serial_bus_slave;
}

void acpi_init_device_object(struct acpi_device *device, acpi_handle handle,
			     int type)
{
	INIT_LIST_HEAD(&device->pnp.ids);
	device->device_type = type;
	device->handle = handle;
	device->parent = acpi_bus_get_parent(handle);
	fwnode_init(&device->fwnode, &acpi_device_fwnode_ops);
	acpi_set_device_status(device, ACPI_STA_DEFAULT);
	acpi_device_get_busid(device);
	acpi_set_pnp_ids(handle, &device->pnp, type);
	acpi_init_properties(device);
	acpi_bus_get_flags(device);
	device->flags.match_driver = false;
	device->flags.initialized = true;
	device->flags.enumeration_by_parent =
		acpi_device_enumeration_by_parent(device);
	acpi_device_clear_enumerated(device);
	device_initialize(&device->dev);
	dev_set_uevent_suppress(&device->dev, true);
	acpi_init_coherency(device);
	/* Assume there are unmet deps to start with. */
	device->dep_unmet = 1;
}

void acpi_device_add_finalize(struct acpi_device *device)
{
	dev_set_uevent_suppress(&device->dev, false);
	kobject_uevent(&device->dev.kobj, KOBJ_ADD);
}

static void acpi_scan_init_status(struct acpi_device *adev)
{
	if (acpi_bus_get_status(adev))
		acpi_set_device_status(adev, 0);
}

static int acpi_add_single_object(struct acpi_device **child,
				  acpi_handle handle, int type)
{
	struct acpi_device *device;
	int result;

	device = kzalloc(sizeof(struct acpi_device), GFP_KERNEL);
	if (!device)
		return -ENOMEM;

	acpi_init_device_object(device, handle, type);
	/*
	 * Getting the status is delayed till here so that we can call
	 * acpi_bus_get_status() and use its quirk handling.  Note that
	 * this must be done before the get power-/wakeup_dev-flags calls.
	 */
	if (type == ACPI_BUS_TYPE_DEVICE || type == ACPI_BUS_TYPE_PROCESSOR)
		acpi_scan_init_status(device);

	acpi_bus_get_power_flags(device);
	acpi_bus_get_wakeup_device_flags(device);

	result = acpi_device_add(device, acpi_device_release);
	if (result) {
		acpi_device_release(&device->dev);
		return result;
	}

	acpi_power_add_remove_device(device, true);
	acpi_device_add_finalize(device);

	acpi_handle_debug(handle, "Added as %s, parent %s\n",
			  dev_name(&device->dev), device->parent ?
				dev_name(&device->parent->dev) : "(null)");

	*child = device;
	return 0;
}

static acpi_status acpi_get_resource_memory(struct acpi_resource *ares,
					    void *context)
{
	struct resource *res = context;

	if (acpi_dev_resource_memory(ares, res))
		return AE_CTRL_TERMINATE;

	return AE_OK;
}

static bool acpi_device_should_be_hidden(acpi_handle handle)
{
	acpi_status status;
	struct resource res;

	/* Check if it should ignore the UART device */
	if (!(spcr_uart_addr && acpi_has_method(handle, METHOD_NAME__CRS)))
		return false;

	/*
	 * The UART device described in SPCR table is assumed to have only one
	 * memory resource present. So we only look for the first one here.
	 */
	status = acpi_walk_resources(handle, METHOD_NAME__CRS,
				     acpi_get_resource_memory, &res);
	if (ACPI_FAILURE(status) || res.start != spcr_uart_addr)
		return false;

	acpi_handle_info(handle, "The UART device @%pa in SPCR table will be hidden\n",
			 &res.start);

	return true;
}

bool acpi_device_is_present(const struct acpi_device *adev)
{
	return adev->status.present || adev->status.functional;
}

static bool acpi_scan_handler_matching(struct acpi_scan_handler *handler,
				       const char *idstr,
				       const struct acpi_device_id **matchid)
{
	const struct acpi_device_id *devid;

	if (handler->match)
		return handler->match(idstr, matchid);

	for (devid = handler->ids; devid->id[0]; devid++)
		if (!strcmp((char *)devid->id, idstr)) {
			if (matchid)
				*matchid = devid;

			return true;
		}

	return false;
}

static struct acpi_scan_handler *acpi_scan_match_handler(const char *idstr,
					const struct acpi_device_id **matchid)
{
	struct acpi_scan_handler *handler;

	list_for_each_entry(handler, &acpi_scan_handlers_list, list_node)
		if (acpi_scan_handler_matching(handler, idstr, matchid))
			return handler;

	return NULL;
}

void acpi_scan_hotplug_enabled(struct acpi_hotplug_profile *hotplug, bool val)
{
	if (!!hotplug->enabled == !!val)
		return;

	mutex_lock(&acpi_scan_lock);

	hotplug->enabled = val;

	mutex_unlock(&acpi_scan_lock);
}

static void acpi_scan_init_hotplug(struct acpi_device *adev)
{
	struct acpi_hardware_id *hwid;

	if (acpi_dock_match(adev->handle) || is_ejectable_bay(adev)) {
		acpi_dock_add(adev);
		return;
	}
	list_for_each_entry(hwid, &adev->pnp.ids, list) {
		struct acpi_scan_handler *handler;

		handler = acpi_scan_match_handler(hwid->id, NULL);
		if (handler) {
			adev->flags.hotplug_notify = true;
			break;
		}
	}
}

static u32 acpi_scan_check_dep(acpi_handle handle, bool check_dep)
{
	struct acpi_handle_list dep_devices;
	acpi_status status;
	u32 count;
	int i;

	/*
	 * Check for _HID here to avoid deferring the enumeration of:
	 * 1. PCI devices.
	 * 2. ACPI nodes describing USB ports.
	 * Still, checking for _HID catches more then just these cases ...
	 */
	if (!check_dep || !acpi_has_method(handle, "_DEP") ||
	    !acpi_has_method(handle, "_HID"))
		return 0;

	status = acpi_evaluate_reference(handle, "_DEP", NULL, &dep_devices);
	if (ACPI_FAILURE(status)) {
		acpi_handle_debug(handle, "Failed to evaluate _DEP.\n");
		return 0;
	}

	for (count = 0, i = 0; i < dep_devices.count; i++) {
		struct acpi_device_info *info;
		struct acpi_dep_data *dep;
		bool skip;

		status = acpi_get_object_info(dep_devices.handles[i], &info);
		if (ACPI_FAILURE(status)) {
			acpi_handle_debug(handle, "Error reading _DEP device info\n");
			continue;
		}

		skip = acpi_info_matches_ids(info, acpi_ignore_dep_ids);
		kfree(info);

		if (skip)
			continue;

		dep = kzalloc(sizeof(*dep), GFP_KERNEL);
		if (!dep)
			continue;

		count++;

		dep->supplier = dep_devices.handles[i];
		dep->consumer = handle;

		mutex_lock(&acpi_dep_list_lock);
		list_add_tail(&dep->node , &acpi_dep_list);
		mutex_unlock(&acpi_dep_list_lock);
	}

	return count;
}

static void acpi_scan_dep_init(struct acpi_device *adev)
{
	struct acpi_dep_data *dep;

	adev->dep_unmet = 0;

	mutex_lock(&acpi_dep_list_lock);

	list_for_each_entry(dep, &acpi_dep_list, node) {
		if (dep->consumer == adev->handle)
			adev->dep_unmet++;
	}

	mutex_unlock(&acpi_dep_list_lock);
}

static bool acpi_bus_scan_second_pass;

static acpi_status acpi_bus_check_add(acpi_handle handle, bool check_dep,
				      struct acpi_device **adev_p)
{
	struct acpi_device *device = NULL;
	acpi_object_type acpi_type;
	int type;

	acpi_bus_get_device(handle, &device);
	if (device)
		goto out;

	if (ACPI_FAILURE(acpi_get_type(handle, &acpi_type)))
		return AE_OK;

	switch (acpi_type) {
	case ACPI_TYPE_DEVICE:
		if (acpi_device_should_be_hidden(handle))
			return AE_OK;

		/* Bail out if there are dependencies. */
		if (acpi_scan_check_dep(handle, check_dep) > 0) {
			acpi_bus_scan_second_pass = true;
			return AE_CTRL_DEPTH;
		}

		fallthrough;
	case ACPI_TYPE_ANY:	/* for ACPI_ROOT_OBJECT */
		type = ACPI_BUS_TYPE_DEVICE;
		break;

	case ACPI_TYPE_PROCESSOR:
		type = ACPI_BUS_TYPE_PROCESSOR;
		break;

	case ACPI_TYPE_THERMAL:
		type = ACPI_BUS_TYPE_THERMAL;
		break;

	case ACPI_TYPE_POWER:
		acpi_add_power_resource(handle);
		fallthrough;
	default:
		return AE_OK;
	}

	acpi_add_single_object(&device, handle, type);
	if (!device)
		return AE_CTRL_DEPTH;

	acpi_scan_init_hotplug(device);
	/*
	 * If check_dep is true at this point, the device has no dependencies,
	 * or the creation of the device object would have been postponed above.
	 */
	if (check_dep)
		device->dep_unmet = 0;
	else
		acpi_scan_dep_init(device);

out:
	if (!*adev_p)
		*adev_p = device;

	return AE_OK;
}

static acpi_status acpi_bus_check_add_1(acpi_handle handle, u32 lvl_not_used,
					void *not_used, void **ret_p)
{
	return acpi_bus_check_add(handle, true, (struct acpi_device **)ret_p);
}

static acpi_status acpi_bus_check_add_2(acpi_handle handle, u32 lvl_not_used,
					void *not_used, void **ret_p)
{
	return acpi_bus_check_add(handle, false, (struct acpi_device **)ret_p);
}

static void acpi_default_enumeration(struct acpi_device *device)
{
	/*
	 * Do not enumerate devices with enumeration_by_parent flag set as
	 * they will be enumerated by their respective parents.
	 */
	if (!device->flags.enumeration_by_parent) {
		acpi_create_platform_device(device, NULL);
		acpi_device_set_enumerated(device);
	} else {
		blocking_notifier_call_chain(&acpi_reconfig_chain,
					     ACPI_RECONFIG_DEVICE_ADD, device);
	}
}

static const struct acpi_device_id generic_device_ids[] = {
	{ACPI_DT_NAMESPACE_HID, },
	{"", },
};

static int acpi_generic_device_attach(struct acpi_device *adev,
				      const struct acpi_device_id *not_used)
{
	/*
	 * Since ACPI_DT_NAMESPACE_HID is the only ID handled here, the test
	 * below can be unconditional.
	 */
	if (adev->data.of_compatible)
		acpi_default_enumeration(adev);

	return 1;
}

static struct acpi_scan_handler generic_device_handler = {
	.ids = generic_device_ids,
	.attach = acpi_generic_device_attach,
};

static int acpi_scan_attach_handler(struct acpi_device *device)
{
	struct acpi_hardware_id *hwid;
	int ret = 0;

	list_for_each_entry(hwid, &device->pnp.ids, list) {
		const struct acpi_device_id *devid;
		struct acpi_scan_handler *handler;

		handler = acpi_scan_match_handler(hwid->id, &devid);
		if (handler) {
			if (!handler->attach) {
				device->pnp.type.platform_id = 0;
				continue;
			}
			device->handler = handler;
			ret = handler->attach(device, devid);
			if (ret > 0)
				break;

			device->handler = NULL;
			if (ret < 0)
				break;
		}
	}

	return ret;
}

static void acpi_bus_attach(struct acpi_device *device, bool first_pass)
{
	struct acpi_device *child;
	bool skip = !first_pass && device->flags.visited;
	acpi_handle ejd;
	int ret;

	if (skip)
		goto ok;

	if (ACPI_SUCCESS(acpi_bus_get_ejd(device->handle, &ejd)))
		register_dock_dependent_device(device, ejd);

	acpi_bus_get_status(device);
	/* Skip devices that are not present. */
	if (!acpi_device_is_present(device)) {
		device->flags.initialized = false;
		acpi_device_clear_enumerated(device);
		device->flags.power_manageable = 0;
		return;
	}
	if (device->handler)
		goto ok;

	if (!device->flags.initialized) {
		device->flags.power_manageable =
			device->power.states[ACPI_STATE_D0].flags.valid;
		if (acpi_bus_init_power(device))
			device->flags.power_manageable = 0;

		device->flags.initialized = true;
	} else if (device->flags.visited) {
		goto ok;
	}

	ret = acpi_scan_attach_handler(device);
	if (ret < 0)
		return;

	device->flags.match_driver = true;
	if (ret > 0 && !device->flags.enumeration_by_parent) {
		acpi_device_set_enumerated(device);
		goto ok;
	}

	ret = device_attach(&device->dev);
	if (ret < 0)
		return;

	if (device->pnp.type.platform_id || device->flags.enumeration_by_parent)
		acpi_default_enumeration(device);
	else
		acpi_device_set_enumerated(device);

 ok:
	list_for_each_entry(child, &device->children, node)
		acpi_bus_attach(child, first_pass);

	if (!skip && device->handler && device->handler->hotplug.notify_online)
		device->handler->hotplug.notify_online(device);
}

void acpi_walk_dep_device_list(acpi_handle handle)
{
	struct acpi_dep_data *dep, *tmp;
	struct acpi_device *adev;

	mutex_lock(&acpi_dep_list_lock);
	list_for_each_entry_safe(dep, tmp, &acpi_dep_list, node) {
		if (dep->supplier == handle) {
			acpi_bus_get_device(dep->consumer, &adev);

			if (adev) {
				adev->dep_unmet--;
				if (!adev->dep_unmet)
					acpi_bus_attach(adev, true);
			}

			list_del(&dep->node);
			kfree(dep);
		}
	}
	mutex_unlock(&acpi_dep_list_lock);
}
EXPORT_SYMBOL_GPL(acpi_walk_dep_device_list);

/**
 * acpi_bus_scan - Add ACPI device node objects in a given namespace scope.
 * @handle: Root of the namespace scope to scan.
 *
 * Scan a given ACPI tree (probably recently hot-plugged) and create and add
 * found devices.
 *
 * If no devices were found, -ENODEV is returned, but it does not mean that
 * there has been a real error.  There just have been no suitable ACPI objects
 * in the table trunk from which the kernel could create a device and add an
 * appropriate driver.
 *
 * Must be called under acpi_scan_lock.
 */
int acpi_bus_scan(acpi_handle handle)
{
	struct acpi_device *device = NULL;

	acpi_bus_scan_second_pass = false;

	/* Pass 1: Avoid enumerating devices with missing dependencies. */

	if (ACPI_SUCCESS(acpi_bus_check_add(handle, true, &device)))
		acpi_walk_namespace(ACPI_TYPE_ANY, handle, ACPI_UINT32_MAX,
				    acpi_bus_check_add_1, NULL, NULL,
				    (void **)&device);

	if (!device)
		return -ENODEV;

	acpi_bus_attach(device, true);

	if (!acpi_bus_scan_second_pass)
		return 0;

	/* Pass 2: Enumerate all of the remaining devices. */

	device = NULL;

	if (ACPI_SUCCESS(acpi_bus_check_add(handle, false, &device)))
		acpi_walk_namespace(ACPI_TYPE_ANY, handle, ACPI_UINT32_MAX,
				    acpi_bus_check_add_2, NULL, NULL,
				    (void **)&device);

	acpi_bus_attach(device, false);

	return 0;
}
EXPORT_SYMBOL(acpi_bus_scan);

/**
 * acpi_bus_trim - Detach scan handlers and drivers from ACPI device objects.
 * @adev: Root of the ACPI namespace scope to walk.
 *
 * Must be called under acpi_scan_lock.
 */
void acpi_bus_trim(struct acpi_device *adev)
{
	struct acpi_scan_handler *handler = adev->handler;
	struct acpi_device *child;

	list_for_each_entry_reverse(child, &adev->children, node)
		acpi_bus_trim(child);

	adev->flags.match_driver = false;
	if (handler) {
		if (handler->detach)
			handler->detach(adev);

		adev->handler = NULL;
	} else {
		device_release_driver(&adev->dev);
	}
	/*
	 * Most likely, the device is going away, so put it into D3cold before
	 * that.
	 */
	acpi_device_set_power(adev, ACPI_STATE_D3_COLD);
	adev->flags.initialized = false;
	acpi_device_clear_enumerated(adev);
}
EXPORT_SYMBOL_GPL(acpi_bus_trim);

int acpi_bus_register_early_device(int type)
{
	struct acpi_device *device = NULL;
	int result;

	result = acpi_add_single_object(&device, NULL, type);
	if (result)
		return result;

	device->flags.match_driver = true;
	return device_attach(&device->dev);
}
EXPORT_SYMBOL_GPL(acpi_bus_register_early_device);

static int acpi_bus_scan_fixed(void)
{
	int result = 0;

	/*
	 * Enumerate all fixed-feature devices.
	 */
	if (!(acpi_gbl_FADT.flags & ACPI_FADT_POWER_BUTTON)) {
		struct acpi_device *device = NULL;

		result = acpi_add_single_object(&device, NULL,
						ACPI_BUS_TYPE_POWER_BUTTON);
		if (result)
			return result;

		device->flags.match_driver = true;
		result = device_attach(&device->dev);
		if (result < 0)
			return result;

		device_init_wakeup(&device->dev, true);
	}

	if (!(acpi_gbl_FADT.flags & ACPI_FADT_SLEEP_BUTTON)) {
		struct acpi_device *device = NULL;

		result = acpi_add_single_object(&device, NULL,
						ACPI_BUS_TYPE_SLEEP_BUTTON);
		if (result)
			return result;

		device->flags.match_driver = true;
		result = device_attach(&device->dev);
	}

	return result < 0 ? result : 0;
}

static void __init acpi_get_spcr_uart_addr(void)
{
	acpi_status status;
	struct acpi_table_spcr *spcr_ptr;

	status = acpi_get_table(ACPI_SIG_SPCR, 0,
				(struct acpi_table_header **)&spcr_ptr);
	if (ACPI_FAILURE(status)) {
		pr_warn(PREFIX "STAO table present, but SPCR is missing\n");
		return;
	}

	spcr_uart_addr = spcr_ptr->serial_port.address;
	acpi_put_table((struct acpi_table_header *)spcr_ptr);
}

static bool acpi_scan_initialized;

int __init acpi_scan_init(void)
{
	int result;
	acpi_status status;
	struct acpi_table_stao *stao_ptr;

	acpi_pci_root_init();
	acpi_pci_link_init();
	acpi_processor_init();
	acpi_platform_init();
	acpi_lpss_init();
	acpi_apd_init();
	acpi_cmos_rtc_init();
	acpi_container_init();
	acpi_memory_hotplug_init();
	acpi_watchdog_init();
	acpi_pnp_init();
	acpi_int340x_thermal_init();
	acpi_amba_init();
	acpi_init_lpit();

	acpi_scan_add_handler(&generic_device_handler);

	/*
	 * If there is STAO table, check whether it needs to ignore the UART
	 * device in SPCR table.
	 */
	status = acpi_get_table(ACPI_SIG_STAO, 0,
				(struct acpi_table_header **)&stao_ptr);
	if (ACPI_SUCCESS(status)) {
		if (stao_ptr->header.length > sizeof(struct acpi_table_stao))
			pr_info(PREFIX "STAO Name List not yet supported.\n");

		if (stao_ptr->ignore_uart)
			acpi_get_spcr_uart_addr();

		acpi_put_table((struct acpi_table_header *)stao_ptr);
	}

	acpi_gpe_apply_masked_gpes();
	acpi_update_all_gpes();

	/*
	 * Although we call __add_memory() that is documented to require the
	 * device_hotplug_lock, it is not necessary here because this is an
	 * early code when userspace or any other code path cannot trigger
	 * hotplug/hotunplug operations.
	 */
	mutex_lock(&acpi_scan_lock);
	/*
	 * Enumerate devices in the ACPI namespace.
	 */
	result = acpi_bus_scan(ACPI_ROOT_OBJECT);
	if (result)
		goto out;

	result = acpi_bus_get_device(ACPI_ROOT_OBJECT, &acpi_root);
	if (result)
		goto out;

	/* Fixed feature devices do not exist on HW-reduced platform */
	if (!acpi_gbl_reduced_hardware) {
		result = acpi_bus_scan_fixed();
		if (result) {
			acpi_detach_data(acpi_root->handle,
					 acpi_scan_drop_device);
			acpi_device_del(acpi_root);
			acpi_bus_put_acpi_device(acpi_root);
			goto out;
		}
	}

	acpi_turn_off_unused_power_resources(true);

	acpi_scan_initialized = true;

 out:
	mutex_unlock(&acpi_scan_lock);
	return result;
}

static struct acpi_probe_entry *ape;
static int acpi_probe_count;
static DEFINE_MUTEX(acpi_probe_mutex);

static int __init acpi_match_madt(union acpi_subtable_headers *header,
				  const unsigned long end)
{
	if (!ape->subtable_valid || ape->subtable_valid(&header->common, ape))
		if (!ape->probe_subtbl(header, end))
			acpi_probe_count++;

	return 0;
}

int __init __acpi_probe_device_table(struct acpi_probe_entry *ap_head, int nr)
{
	int count = 0;

	if (acpi_disabled)
		return 0;

	mutex_lock(&acpi_probe_mutex);
	for (ape = ap_head; nr; ape++, nr--) {
		if (ACPI_COMPARE_NAMESEG(ACPI_SIG_MADT, ape->id)) {
			acpi_probe_count = 0;
			acpi_table_parse_madt(ape->type, acpi_match_madt, 0);
			count += acpi_probe_count;
		} else {
			int res;
			res = acpi_table_parse(ape->id, ape->probe_table);
			if (!res)
				count++;
		}
	}
	mutex_unlock(&acpi_probe_mutex);

	return count;
}

struct acpi_table_events_work {
	struct work_struct work;
	void *table;
	u32 event;
};

static void acpi_table_events_fn(struct work_struct *work)
{
	struct acpi_table_events_work *tew;

	tew = container_of(work, struct acpi_table_events_work, work);

	if (tew->event == ACPI_TABLE_EVENT_LOAD) {
		acpi_scan_lock_acquire();
		acpi_bus_scan(ACPI_ROOT_OBJECT);
		acpi_scan_lock_release();
	}

	kfree(tew);
}

void acpi_scan_table_handler(u32 event, void *table, void *context)
{
	struct acpi_table_events_work *tew;

	if (!acpi_scan_initialized)
		return;

	if (event != ACPI_TABLE_EVENT_LOAD)
		return;

	tew = kmalloc(sizeof(*tew), GFP_KERNEL);
	if (!tew)
		return;

	INIT_WORK(&tew->work, acpi_table_events_fn);
	tew->table = table;
	tew->event = event;

	schedule_work(&tew->work);
}

int acpi_reconfig_notifier_register(struct notifier_block *nb)
{
	return blocking_notifier_chain_register(&acpi_reconfig_chain, nb);
}
EXPORT_SYMBOL(acpi_reconfig_notifier_register);

int acpi_reconfig_notifier_unregister(struct notifier_block *nb)
{
	return blocking_notifier_chain_unregister(&acpi_reconfig_chain, nb);
}
EXPORT_SYMBOL(acpi_reconfig_notifier_unregister);
