// SPDX-License-Identifier: GPL-2.0
/*
 * drd.c - DesignWare USB2 DRD Controller Dual-role support
 *
 * Copyright (C) 2020 STMicroelectronics
 *
 * Author(s): Amelie Delaunay <amelie.delaunay@st.com>
 */

#include <linux/iopoll.h>
#include <linux/platform_device.h>
#include <linux/usb/role.h>
#include "core.h"

static void dwc2_ovr_init(struct dwc2_hsotg *hsotg)
{
	unsigned long flags;
	u32 gotgctl;

	spin_lock_irqsave(&hsotg->lock, flags);

	gotgctl = dwc2_readl(hsotg, GOTGCTL);
	gotgctl |= GOTGCTL_BVALOEN | GOTGCTL_AVALOEN | GOTGCTL_VBVALOEN;
	gotgctl |= GOTGCTL_DBNCE_FLTR_BYPASS;
	gotgctl &= ~(GOTGCTL_BVALOVAL | GOTGCTL_AVALOVAL | GOTGCTL_VBVALOVAL);
	dwc2_writel(hsotg, gotgctl, GOTGCTL);

	dwc2_force_mode(hsotg, false);

	spin_unlock_irqrestore(&hsotg->lock, flags);
}

static int dwc2_ovr_avalid(struct dwc2_hsotg *hsotg, bool valid)
{
	u32 gotgctl = dwc2_readl(hsotg, GOTGCTL);

	/* Check if A-Session is already in the right state */
	if ((valid && (gotgctl & GOTGCTL_ASESVLD)) ||
	    (!valid && !(gotgctl & GOTGCTL_ASESVLD)))
		return -EALREADY;

	if (valid)
		gotgctl |= GOTGCTL_AVALOVAL | GOTGCTL_VBVALOVAL;
	else
		gotgctl &= ~(GOTGCTL_AVALOVAL | GOTGCTL_VBVALOVAL);
	dwc2_writel(hsotg, gotgctl, GOTGCTL);

	return 0;
}

static int dwc2_ovr_bvalid(struct dwc2_hsotg *hsotg, bool valid)
{
	u32 gotgctl = dwc2_readl(hsotg, GOTGCTL);

	/* Check if B-Session is already in the right state */
	if ((valid && (gotgctl & GOTGCTL_BSESVLD)) ||
	    (!valid && !(gotgctl & GOTGCTL_BSESVLD)))
		return -EALREADY;

	if (valid)
		gotgctl |= GOTGCTL_BVALOVAL | GOTGCTL_VBVALOVAL;
	else
		gotgctl &= ~(GOTGCTL_BVALOVAL | GOTGCTL_VBVALOVAL);
	dwc2_writel(hsotg, gotgctl, GOTGCTL);

	return 0;
}

static int dwc2_drd_role_sw_set(struct usb_role_switch *sw, enum usb_role role)
{
	struct dwc2_hsotg *hsotg = usb_role_switch_get_drvdata(sw);
	unsigned long flags;
	int already = 0;

	/* Skip session not in line with dr_mode */
	if ((role == USB_ROLE_DEVICE && hsotg->dr_mode == USB_DR_MODE_HOST) ||
	    (role == USB_ROLE_HOST && hsotg->dr_mode == USB_DR_MODE_PERIPHERAL))
		return -EINVAL;

#if IS_ENABLED(CONFIG_USB_DWC2_PERIPHERAL) || \
	IS_ENABLED(CONFIG_USB_DWC2_DUAL_ROLE)
	/* Skip session if core is in test mode */
	if (role == USB_ROLE_NONE && hsotg->test_mode) {
		dev_dbg(hsotg->dev, "Core is in test mode\n");
		return -EBUSY;
	}
#endif

	spin_lock_irqsave(&hsotg->lock, flags);

	if (role == USB_ROLE_HOST) {
		already = dwc2_ovr_avalid(hsotg, true);
	} else if (role == USB_ROLE_DEVICE) {
		already = dwc2_ovr_bvalid(hsotg, true);
		/* This clear DCTL.SFTDISCON bit */
		dwc2_hsotg_core_connect(hsotg);
	} else {
		if (dwc2_is_device_mode(hsotg)) {
			if (!dwc2_ovr_bvalid(hsotg, false))
				/* This set DCTL.SFTDISCON bit */
				dwc2_hsotg_core_disconnect(hsotg);
		} else {
			dwc2_ovr_avalid(hsotg, false);
		}
	}

	spin_unlock_irqrestore(&hsotg->lock, flags);

	if (!already && hsotg->dr_mode == USB_DR_MODE_OTG)
		/* This will raise a Connector ID Status Change Interrupt */
		dwc2_force_mode(hsotg, role == USB_ROLE_HOST);

	dev_dbg(hsotg->dev, "%s-session valid\n",
		role == USB_ROLE_NONE ? "No" :
		role == USB_ROLE_HOST ? "A" : "B");

	return 0;
}

int dwc2_drd_init(struct dwc2_hsotg *hsotg)
{
	struct usb_role_switch_desc role_sw_desc = {0};
	struct usb_role_switch *role_sw;
	int ret;

	if (!device_property_read_bool(hsotg->dev, "usb-role-switch"))
		return 0;

	role_sw_desc.driver_data = hsotg;
	role_sw_desc.fwnode = dev_fwnode(hsotg->dev);
	role_sw_desc.set = dwc2_drd_role_sw_set;
	role_sw_desc.allow_userspace_control = true;

	role_sw = usb_role_switch_register(hsotg->dev, &role_sw_desc);
	if (IS_ERR(role_sw)) {
		ret = PTR_ERR(role_sw);
		dev_err(hsotg->dev,
			"failed to register role switch: %d\n", ret);
		return ret;
	}

	hsotg->role_sw = role_sw;

	/* Enable override and initialize values */
	dwc2_ovr_init(hsotg);

	return 0;
}

void dwc2_drd_suspend(struct dwc2_hsotg *hsotg)
{
	u32 gintsts, gintmsk;

	if (hsotg->role_sw && !hsotg->params.external_id_pin_ctl) {
		gintmsk = dwc2_readl(hsotg, GINTMSK);
		gintmsk &= ~GINTSTS_CONIDSTSCHNG;
		dwc2_writel(hsotg, gintmsk, GINTMSK);
		gintsts = dwc2_readl(hsotg, GINTSTS);
		dwc2_writel(hsotg, gintsts | GINTSTS_CONIDSTSCHNG, GINTSTS);
	}
}

void dwc2_drd_resume(struct dwc2_hsotg *hsotg)
{
	u32 gintsts, gintmsk;

	if (hsotg->role_sw && !hsotg->params.external_id_pin_ctl) {
		gintsts = dwc2_readl(hsotg, GINTSTS);
		dwc2_writel(hsotg, gintsts | GINTSTS_CONIDSTSCHNG, GINTSTS);
		gintmsk = dwc2_readl(hsotg, GINTMSK);
		gintmsk |= GINTSTS_CONIDSTSCHNG;
		dwc2_writel(hsotg, gintmsk, GINTMSK);
	}
}

void dwc2_drd_exit(struct dwc2_hsotg *hsotg)
{
	if (hsotg->role_sw)
		usb_role_switch_unregister(hsotg->role_sw);
}
