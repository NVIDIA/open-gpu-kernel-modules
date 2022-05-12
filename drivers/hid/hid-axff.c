// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Force feedback support for ACRUX game controllers
 *
 * From what I have gathered, these devices are mass produced in China
 * by several vendors. They often share the same design as the original
 * Xbox 360 controller.
 *
 * 1a34:0802 "ACRUX USB GAMEPAD 8116"
 *  - tested with an EXEQ EQ-PCU-02090 game controller.
 *
 * Copyright (c) 2010 Sergei Kolzun <x0r@dv-life.ru>
 */

/*
 */

#include <linux/input.h>
#include <linux/slab.h>
#include <linux/hid.h>
#include <linux/module.h>

#include "hid-ids.h"

#ifdef CONFIG_HID_ACRUX_FF

struct axff_device {
	struct hid_report *report;
};

static int axff_play(struct input_dev *dev, void *data, struct ff_effect *effect)
{
	struct hid_device *hid = input_get_drvdata(dev);
	struct axff_device *axff = data;
	struct hid_report *report = axff->report;
	int field_count = 0;
	int left, right;
	int i, j;

	left = effect->u.rumble.strong_magnitude;
	right = effect->u.rumble.weak_magnitude;

	dbg_hid("called with 0x%04x 0x%04x", left, right);

	left = left * 0xff / 0xffff;
	right = right * 0xff / 0xffff;

	for (i = 0; i < report->maxfield; i++) {
		for (j = 0; j < report->field[i]->report_count; j++) {
			report->field[i]->value[j] =
				field_count % 2 ? right : left;
			field_count++;
		}
	}

	dbg_hid("running with 0x%02x 0x%02x", left, right);
	hid_hw_request(hid, axff->report, HID_REQ_SET_REPORT);

	return 0;
}

static int axff_init(struct hid_device *hid)
{
	struct axff_device *axff;
	struct hid_report *report;
	struct hid_input *hidinput;
	struct list_head *report_list =&hid->report_enum[HID_OUTPUT_REPORT].report_list;
	struct input_dev *dev;
	int field_count = 0;
	int i, j;
	int error;

	if (list_empty(&hid->inputs)) {
		hid_err(hid, "no inputs found\n");
		return -ENODEV;
	}
	hidinput = list_first_entry(&hid->inputs, struct hid_input, list);
	dev = hidinput->input;

	if (list_empty(report_list)) {
		hid_err(hid, "no output reports found\n");
		return -ENODEV;
	}

	report = list_first_entry(report_list, struct hid_report, list);
	for (i = 0; i < report->maxfield; i++) {
		for (j = 0; j < report->field[i]->report_count; j++) {
			report->field[i]->value[j] = 0x00;
			field_count++;
		}
	}

	if (field_count < 4 && hid->product != 0xf705) {
		hid_err(hid, "not enough fields in the report: %d\n",
			field_count);
		return -ENODEV;
	}

	axff = kzalloc(sizeof(struct axff_device), GFP_KERNEL);
	if (!axff)
		return -ENOMEM;

	set_bit(FF_RUMBLE, dev->ffbit);

	error = input_ff_create_memless(dev, axff, axff_play);
	if (error)
		goto err_free_mem;

	axff->report = report;
	hid_hw_request(hid, axff->report, HID_REQ_SET_REPORT);

	hid_info(hid, "Force Feedback for ACRUX game controllers by Sergei Kolzun <x0r@dv-life.ru>\n");

	return 0;

err_free_mem:
	kfree(axff);
	return error;
}
#else
static inline int axff_init(struct hid_device *hid)
{
	return 0;
}
#endif

static int ax_probe(struct hid_device *hdev, const struct hid_device_id *id)
{
	int error;

	dev_dbg(&hdev->dev, "ACRUX HID hardware probe...\n");

	error = hid_parse(hdev);
	if (error) {
		hid_err(hdev, "parse failed\n");
		return error;
	}

	error = hid_hw_start(hdev, HID_CONNECT_DEFAULT & ~HID_CONNECT_FF);
	if (error) {
		hid_err(hdev, "hw start failed\n");
		return error;
	}

	error = axff_init(hdev);
	if (error) {
		/*
		 * Do not fail device initialization completely as device
		 * may still be partially operable, just warn.
		 */
		hid_warn(hdev,
			 "Failed to enable force feedback support, error: %d\n",
			 error);
	}

	/*
	 * We need to start polling device right away, otherwise
	 * it will go into a coma.
	 */
	error = hid_hw_open(hdev);
	if (error) {
		dev_err(&hdev->dev, "hw open failed\n");
		hid_hw_stop(hdev);
		return error;
	}

	return 0;
}

static void ax_remove(struct hid_device *hdev)
{
	hid_hw_close(hdev);
	hid_hw_stop(hdev);
}

static const struct hid_device_id ax_devices[] = {
	{ HID_USB_DEVICE(USB_VENDOR_ID_ACRUX, 0x0802), },
	{ HID_USB_DEVICE(USB_VENDOR_ID_ACRUX, 0xf705), },
	{ }
};
MODULE_DEVICE_TABLE(hid, ax_devices);

static struct hid_driver ax_driver = {
	.name		= "acrux",
	.id_table	= ax_devices,
	.probe		= ax_probe,
	.remove		= ax_remove,
};
module_hid_driver(ax_driver);

MODULE_AUTHOR("Sergei Kolzun");
MODULE_DESCRIPTION("Force feedback support for ACRUX game controllers");
MODULE_LICENSE("GPL");
