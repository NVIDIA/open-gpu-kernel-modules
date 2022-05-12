// SPDX-License-Identifier: GPL-2.0+
//
// Siano Mobile Silicon, Inc.
// MDTV receiver kernel modules.
// Copyright (C) 2006-2009, Uri Shkolnik
//
// Copyright (c) 2010 - Mauro Carvalho Chehab
//	- Ported the driver to use rc-core
//	- IR raw event decoding is now done at rc-core
//	- Code almost re-written


#include "smscoreapi.h"

#include <linux/types.h>
#include <linux/input.h>

#include "smsir.h"
#include "sms-cards.h"

#define MODULE_NAME "smsmdtv"

void sms_ir_event(struct smscore_device_t *coredev, const char *buf, int len)
{
	int i;
	const s32 *samples = (const void *)buf;

	for (i = 0; i < len >> 2; i++) {
		struct ir_raw_event ev = {
			.duration = abs(samples[i]),
			.pulse = (samples[i] > 0) ? false : true
		};

		ir_raw_event_store(coredev->ir.dev, &ev);
	}
	ir_raw_event_handle(coredev->ir.dev);
}

int sms_ir_init(struct smscore_device_t *coredev)
{
	int err;
	int board_id = smscore_get_board_id(coredev);
	struct rc_dev *dev;

	pr_debug("Allocating rc device\n");
	dev = rc_allocate_device(RC_DRIVER_IR_RAW);
	if (!dev)
		return -ENOMEM;

	coredev->ir.controller = 0;	/* Todo: vega/nova SPI number */
	coredev->ir.timeout = US_TO_NS(IR_DEFAULT_TIMEOUT);
	pr_debug("IR port %d, timeout %d ms\n",
			coredev->ir.controller, coredev->ir.timeout);

	snprintf(coredev->ir.name, sizeof(coredev->ir.name),
		 "SMS IR (%s)", sms_get_board(board_id)->name);

	strscpy(coredev->ir.phys, coredev->devpath, sizeof(coredev->ir.phys));
	strlcat(coredev->ir.phys, "/ir0", sizeof(coredev->ir.phys));

	dev->device_name = coredev->ir.name;
	dev->input_phys = coredev->ir.phys;
	dev->dev.parent = coredev->device;

#if 0
	/* TODO: properly initialize the parameters below */
	dev->input_id.bustype = BUS_USB;
	dev->input_id.version = 1;
	dev->input_id.vendor = le16_to_cpu(dev->udev->descriptor.idVendor);
	dev->input_id.product = le16_to_cpu(dev->udev->descriptor.idProduct);
#endif

	dev->priv = coredev;
	dev->allowed_protocols = RC_PROTO_BIT_ALL_IR_DECODER;
	dev->map_name = sms_get_board(board_id)->rc_codes;
	dev->driver_name = MODULE_NAME;

	pr_debug("Input device (IR) %s is set for key events\n",
		 dev->device_name);

	err = rc_register_device(dev);
	if (err < 0) {
		pr_err("Failed to register device\n");
		rc_free_device(dev);
		return err;
	}

	coredev->ir.dev = dev;
	return 0;
}

void sms_ir_exit(struct smscore_device_t *coredev)
{
	rc_unregister_device(coredev->ir.dev);

	pr_debug("\n");
}
