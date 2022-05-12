/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Greybus Interface Block code
 *
 * Copyright 2014 Google Inc.
 * Copyright 2014 Linaro Ltd.
 */

#ifndef __INTERFACE_H
#define __INTERFACE_H

#include <linux/types.h>
#include <linux/device.h>

enum gb_interface_type {
	GB_INTERFACE_TYPE_INVALID = 0,
	GB_INTERFACE_TYPE_UNKNOWN,
	GB_INTERFACE_TYPE_DUMMY,
	GB_INTERFACE_TYPE_UNIPRO,
	GB_INTERFACE_TYPE_GREYBUS,
};

#define GB_INTERFACE_QUIRK_NO_CPORT_FEATURES		BIT(0)
#define GB_INTERFACE_QUIRK_NO_INIT_STATUS		BIT(1)
#define GB_INTERFACE_QUIRK_NO_GMP_IDS			BIT(2)
#define GB_INTERFACE_QUIRK_FORCED_DISABLE		BIT(3)
#define GB_INTERFACE_QUIRK_LEGACY_MODE_SWITCH		BIT(4)
#define GB_INTERFACE_QUIRK_NO_BUNDLE_ACTIVATE		BIT(5)
#define GB_INTERFACE_QUIRK_NO_PM			BIT(6)

struct gb_interface {
	struct device dev;
	struct gb_control *control;

	struct list_head bundles;
	struct list_head module_node;
	struct list_head manifest_descs;
	u8 interface_id;	/* Physical location within the Endo */
	u8 device_id;
	u8 features;		/* Feature flags set in the manifest */

	enum gb_interface_type type;

	u32 ddbl1_manufacturer_id;
	u32 ddbl1_product_id;
	u32 vendor_id;
	u32 product_id;
	u64 serial_number;

	struct gb_host_device *hd;
	struct gb_module *module;

	unsigned long quirks;

	struct mutex mutex;

	bool disconnected;

	bool ejected;
	bool removed;
	bool active;
	bool enabled;
	bool mode_switch;
	bool dme_read;

	struct work_struct mode_switch_work;
	struct completion mode_switch_completion;
};
#define to_gb_interface(d) container_of(d, struct gb_interface, dev)

struct gb_interface *gb_interface_create(struct gb_module *module,
					 u8 interface_id);
int gb_interface_activate(struct gb_interface *intf);
void gb_interface_deactivate(struct gb_interface *intf);
int gb_interface_enable(struct gb_interface *intf);
void gb_interface_disable(struct gb_interface *intf);
int gb_interface_add(struct gb_interface *intf);
void gb_interface_del(struct gb_interface *intf);
void gb_interface_put(struct gb_interface *intf);
void gb_interface_mailbox_event(struct gb_interface *intf, u16 result,
								u32 mailbox);

int gb_interface_request_mode_switch(struct gb_interface *intf);

#endif /* __INTERFACE_H */
