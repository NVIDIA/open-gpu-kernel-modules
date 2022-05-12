// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  atlas_btns.c - Atlas Wallmount Touchscreen ACPI Extras
 *
 *  Copyright (C) 2006 Jaya Kumar
 *  Based on Toshiba ACPI by John Belmonte and ASUS ACPI
 *  This work was sponsored by CIS(M) Sdn Bhd.
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/input.h>
#include <linux/types.h>
#include <linux/acpi.h>
#include <linux/uaccess.h>

#define ACPI_ATLAS_NAME		"Atlas ACPI"
#define ACPI_ATLAS_CLASS	"Atlas"

static unsigned short atlas_keymap[16];
static struct input_dev *input_dev;

/* button handling code */
static acpi_status acpi_atlas_button_setup(acpi_handle region_handle,
		    u32 function, void *handler_context, void **return_context)
{
	*return_context =
		(function != ACPI_REGION_DEACTIVATE) ? handler_context : NULL;

	return AE_OK;
}

static acpi_status acpi_atlas_button_handler(u32 function,
		      acpi_physical_address address,
		      u32 bit_width, u64 *value,
		      void *handler_context, void *region_context)
{
	acpi_status status;

	if (function == ACPI_WRITE) {
		int code = address & 0x0f;
		int key_down = !(address & 0x10);

		input_event(input_dev, EV_MSC, MSC_SCAN, code);
		input_report_key(input_dev, atlas_keymap[code], key_down);
		input_sync(input_dev);

		status = AE_OK;
	} else {
		pr_warn("shrugged on unexpected function: function=%x,address=%lx,value=%x\n",
			function, (unsigned long)address, (u32)*value);
		status = AE_BAD_PARAMETER;
	}

	return status;
}

static int atlas_acpi_button_add(struct acpi_device *device)
{
	acpi_status status;
	int i;
	int err;

	input_dev = input_allocate_device();
	if (!input_dev) {
		pr_err("unable to allocate input device\n");
		return -ENOMEM;
	}

	input_dev->name = "Atlas ACPI button driver";
	input_dev->phys = "ASIM0000/atlas/input0";
	input_dev->id.bustype = BUS_HOST;
	input_dev->keycode = atlas_keymap;
	input_dev->keycodesize = sizeof(unsigned short);
	input_dev->keycodemax = ARRAY_SIZE(atlas_keymap);

	input_set_capability(input_dev, EV_MSC, MSC_SCAN);
	__set_bit(EV_KEY, input_dev->evbit);
	for (i = 0; i < ARRAY_SIZE(atlas_keymap); i++) {
		if (i < 9) {
			atlas_keymap[i] = KEY_F1 + i;
			__set_bit(KEY_F1 + i, input_dev->keybit);
		} else
			atlas_keymap[i] = KEY_RESERVED;
	}

	err = input_register_device(input_dev);
	if (err) {
		pr_err("couldn't register input device\n");
		input_free_device(input_dev);
		return err;
	}

	/* hookup button handler */
	status = acpi_install_address_space_handler(device->handle,
				0x81, &acpi_atlas_button_handler,
				&acpi_atlas_button_setup, device);
	if (ACPI_FAILURE(status)) {
		pr_err("error installing addr spc handler\n");
		input_unregister_device(input_dev);
		err = -EINVAL;
	}

	return err;
}

static int atlas_acpi_button_remove(struct acpi_device *device)
{
	acpi_status status;

	status = acpi_remove_address_space_handler(device->handle,
				0x81, &acpi_atlas_button_handler);
	if (ACPI_FAILURE(status))
		pr_err("error removing addr spc handler\n");

	input_unregister_device(input_dev);

	return 0;
}

static const struct acpi_device_id atlas_device_ids[] = {
	{"ASIM0000", 0},
	{"", 0},
};
MODULE_DEVICE_TABLE(acpi, atlas_device_ids);

static struct acpi_driver atlas_acpi_driver = {
	.name	= ACPI_ATLAS_NAME,
	.class	= ACPI_ATLAS_CLASS,
	.owner	= THIS_MODULE,
	.ids	= atlas_device_ids,
	.ops	= {
		.add	= atlas_acpi_button_add,
		.remove	= atlas_acpi_button_remove,
	},
};
module_acpi_driver(atlas_acpi_driver);

MODULE_AUTHOR("Jaya Kumar");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Atlas button driver");

