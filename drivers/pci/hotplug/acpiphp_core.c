// SPDX-License-Identifier: GPL-2.0+
/*
 * ACPI PCI Hot Plug Controller Driver
 *
 * Copyright (C) 1995,2001 Compaq Computer Corporation
 * Copyright (C) 2001 Greg Kroah-Hartman (greg@kroah.com)
 * Copyright (C) 2001 IBM Corp.
 * Copyright (C) 2002 Hiroshi Aono (h-aono@ap.jp.nec.com)
 * Copyright (C) 2002,2003 Takayoshi Kochi (t-kochi@bq.jp.nec.com)
 * Copyright (C) 2002,2003 NEC Corporation
 * Copyright (C) 2003-2005 Matthew Wilcox (willy@infradead.org)
 * Copyright (C) 2003-2005 Hewlett Packard
 *
 * All rights reserved.
 *
 * Send feedback to <kristen.c.accardi@intel.com>
 *
 */

#define pr_fmt(fmt) "acpiphp: " fmt

#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>

#include <linux/kernel.h>
#include <linux/pci.h>
#include <linux/pci-acpi.h>
#include <linux/pci_hotplug.h>
#include <linux/slab.h>
#include <linux/smp.h>
#include "acpiphp.h"

/* name size which is used for entries in pcihpfs */
#define SLOT_NAME_SIZE  21              /* {_SUN} */

bool acpiphp_disabled;

/* local variables */
static struct acpiphp_attention_info *attention_info;

#define DRIVER_VERSION	"0.5"
#define DRIVER_AUTHOR	"Greg Kroah-Hartman <gregkh@us.ibm.com>, Takayoshi Kochi <t-kochi@bq.jp.nec.com>, Matthew Wilcox <willy@infradead.org>"
#define DRIVER_DESC	"ACPI Hot Plug PCI Controller Driver"

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("GPL");
MODULE_PARM_DESC(disable, "disable acpiphp driver");
module_param_named(disable, acpiphp_disabled, bool, 0444);

static int enable_slot(struct hotplug_slot *slot);
static int disable_slot(struct hotplug_slot *slot);
static int set_attention_status(struct hotplug_slot *slot, u8 value);
static int get_power_status(struct hotplug_slot *slot, u8 *value);
static int get_attention_status(struct hotplug_slot *slot, u8 *value);
static int get_latch_status(struct hotplug_slot *slot, u8 *value);
static int get_adapter_status(struct hotplug_slot *slot, u8 *value);

static const struct hotplug_slot_ops acpi_hotplug_slot_ops = {
	.enable_slot		= enable_slot,
	.disable_slot		= disable_slot,
	.set_attention_status	= set_attention_status,
	.get_power_status	= get_power_status,
	.get_attention_status	= get_attention_status,
	.get_latch_status	= get_latch_status,
	.get_adapter_status	= get_adapter_status,
};

/**
 * acpiphp_register_attention - set attention LED callback
 * @info: must be completely filled with LED callbacks
 *
 * Description: This is used to register a hardware specific ACPI
 * driver that manipulates the attention LED.  All the fields in
 * info must be set.
 */
int acpiphp_register_attention(struct acpiphp_attention_info *info)
{
	int retval = -EINVAL;

	if (info && info->owner && info->set_attn &&
			info->get_attn && !attention_info) {
		retval = 0;
		attention_info = info;
	}
	return retval;
}
EXPORT_SYMBOL_GPL(acpiphp_register_attention);


/**
 * acpiphp_unregister_attention - unset attention LED callback
 * @info: must match the pointer used to register
 *
 * Description: This is used to un-register a hardware specific acpi
 * driver that manipulates the attention LED.  The pointer to the
 * info struct must be the same as the one used to set it.
 */
int acpiphp_unregister_attention(struct acpiphp_attention_info *info)
{
	int retval = -EINVAL;

	if (info && attention_info == info) {
		attention_info = NULL;
		retval = 0;
	}
	return retval;
}
EXPORT_SYMBOL_GPL(acpiphp_unregister_attention);


/**
 * enable_slot - power on and enable a slot
 * @hotplug_slot: slot to enable
 *
 * Actual tasks are done in acpiphp_enable_slot()
 */
static int enable_slot(struct hotplug_slot *hotplug_slot)
{
	struct slot *slot = to_slot(hotplug_slot);

	pr_debug("%s - physical_slot = %s\n", __func__, slot_name(slot));

	/* enable the specified slot */
	return acpiphp_enable_slot(slot->acpi_slot);
}


/**
 * disable_slot - disable and power off a slot
 * @hotplug_slot: slot to disable
 *
 * Actual tasks are done in acpiphp_disable_slot()
 */
static int disable_slot(struct hotplug_slot *hotplug_slot)
{
	struct slot *slot = to_slot(hotplug_slot);

	pr_debug("%s - physical_slot = %s\n", __func__, slot_name(slot));

	/* disable the specified slot */
	return acpiphp_disable_slot(slot->acpi_slot);
}


/**
 * set_attention_status - set attention LED
 * @hotplug_slot: slot to set attention LED on
 * @status: value to set attention LED to (0 or 1)
 *
 * attention status LED, so we use a callback that
 * was registered with us.  This allows hardware specific
 * ACPI implementations to blink the light for us.
 */
static int set_attention_status(struct hotplug_slot *hotplug_slot, u8 status)
{
	int retval = -ENODEV;

	pr_debug("%s - physical_slot = %s\n", __func__,
		hotplug_slot_name(hotplug_slot));

	if (attention_info && try_module_get(attention_info->owner)) {
		retval = attention_info->set_attn(hotplug_slot, status);
		module_put(attention_info->owner);
	} else
		attention_info = NULL;
	return retval;
}


/**
 * get_power_status - get power status of a slot
 * @hotplug_slot: slot to get status
 * @value: pointer to store status
 *
 * Some platforms may not implement _STA method properly.
 * In that case, the value returned may not be reliable.
 */
static int get_power_status(struct hotplug_slot *hotplug_slot, u8 *value)
{
	struct slot *slot = to_slot(hotplug_slot);

	pr_debug("%s - physical_slot = %s\n", __func__, slot_name(slot));

	*value = acpiphp_get_power_status(slot->acpi_slot);

	return 0;
}


/**
 * get_attention_status - get attention LED status
 * @hotplug_slot: slot to get status from
 * @value: returns with value of attention LED
 *
 * ACPI doesn't have known method to determine the state
 * of the attention status LED, so we use a callback that
 * was registered with us.  This allows hardware specific
 * ACPI implementations to determine its state.
 */
static int get_attention_status(struct hotplug_slot *hotplug_slot, u8 *value)
{
	int retval = -EINVAL;

	pr_debug("%s - physical_slot = %s\n", __func__,
		hotplug_slot_name(hotplug_slot));

	if (attention_info && try_module_get(attention_info->owner)) {
		retval = attention_info->get_attn(hotplug_slot, value);
		module_put(attention_info->owner);
	} else
		attention_info = NULL;
	return retval;
}


/**
 * get_latch_status - get latch status of a slot
 * @hotplug_slot: slot to get status
 * @value: pointer to store status
 *
 * ACPI doesn't provide any formal means to access latch status.
 * Instead, we fake latch status from _STA.
 */
static int get_latch_status(struct hotplug_slot *hotplug_slot, u8 *value)
{
	struct slot *slot = to_slot(hotplug_slot);

	pr_debug("%s - physical_slot = %s\n", __func__, slot_name(slot));

	*value = acpiphp_get_latch_status(slot->acpi_slot);

	return 0;
}


/**
 * get_adapter_status - get adapter status of a slot
 * @hotplug_slot: slot to get status
 * @value: pointer to store status
 *
 * ACPI doesn't provide any formal means to access adapter status.
 * Instead, we fake adapter status from _STA.
 */
static int get_adapter_status(struct hotplug_slot *hotplug_slot, u8 *value)
{
	struct slot *slot = to_slot(hotplug_slot);

	pr_debug("%s - physical_slot = %s\n", __func__, slot_name(slot));

	*value = acpiphp_get_adapter_status(slot->acpi_slot);

	return 0;
}

/* callback routine to initialize 'struct slot' for each slot */
int acpiphp_register_hotplug_slot(struct acpiphp_slot *acpiphp_slot,
				  unsigned int sun)
{
	struct slot *slot;
	int retval = -ENOMEM;
	char name[SLOT_NAME_SIZE];

	slot = kzalloc(sizeof(*slot), GFP_KERNEL);
	if (!slot)
		goto error;

	slot->hotplug_slot.ops = &acpi_hotplug_slot_ops;

	slot->acpi_slot = acpiphp_slot;

	acpiphp_slot->slot = slot;
	slot->sun = sun;
	snprintf(name, SLOT_NAME_SIZE, "%u", sun);

	retval = pci_hp_register(&slot->hotplug_slot, acpiphp_slot->bus,
				 acpiphp_slot->device, name);
	if (retval == -EBUSY)
		goto error_slot;
	if (retval) {
		pr_err("pci_hp_register failed with error %d\n", retval);
		goto error_slot;
	}

	pr_info("Slot [%s] registered\n", slot_name(slot));

	return 0;
error_slot:
	kfree(slot);
error:
	return retval;
}


void acpiphp_unregister_hotplug_slot(struct acpiphp_slot *acpiphp_slot)
{
	struct slot *slot = acpiphp_slot->slot;

	pr_info("Slot [%s] unregistered\n", slot_name(slot));

	pci_hp_deregister(&slot->hotplug_slot);
	kfree(slot);
}


void __init acpiphp_init(void)
{
	pr_info(DRIVER_DESC " version: " DRIVER_VERSION "%s\n",
		acpiphp_disabled ? ", disabled by user; please report a bug"
				 : "");
}
