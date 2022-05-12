/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright (C) 2011 Marvell International Ltd. All rights reserved.
 */

#ifndef __MV_PLATFORM_USB_H
#define __MV_PLATFORM_USB_H

enum {
	MV_USB_MODE_OTG,
	MV_USB_MODE_HOST,
};

enum {
	VBUS_LOW	= 0,
	VBUS_HIGH	= 1 << 0,
};

struct mv_usb_addon_irq {
	unsigned int	irq;
	int		(*poll)(void);
};

struct mv_usb_platform_data {
	struct mv_usb_addon_irq	*id;	/* Only valid for OTG. ID pin change*/
	struct mv_usb_addon_irq	*vbus;	/* valid for OTG/UDC. VBUS change*/

	/* only valid for HCD. OTG or Host only*/
	unsigned int		mode;

	/* This flag is used for that needs id pin checked by otg */
	unsigned int    disable_otg_clock_gating:1;
	/* Force a_bus_req to be asserted */
	 unsigned int    otg_force_a_bus_req:1;

	int	(*phy_init)(void __iomem *regbase);
	void	(*phy_deinit)(void __iomem *regbase);
	int	(*set_vbus)(unsigned int vbus);
};
#endif
