// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * ISP1704 USB Charger Detection driver
 *
 * Copyright (C) 2010 Nokia Corporation
 * Copyright (C) 2012 - 2013 Pali Rohár <pali@kernel.org>
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/err.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/device.h>
#include <linux/sysfs.h>
#include <linux/platform_device.h>
#include <linux/power_supply.h>
#include <linux/delay.h>
#include <linux/of.h>

#include <linux/gpio/consumer.h>
#include <linux/usb/otg.h>
#include <linux/usb/ulpi.h>
#include <linux/usb/ch9.h>
#include <linux/usb/gadget.h>

/* Vendor specific Power Control register */
#define ISP1704_PWR_CTRL		0x3d
#define ISP1704_PWR_CTRL_SWCTRL		(1 << 0)
#define ISP1704_PWR_CTRL_DET_COMP	(1 << 1)
#define ISP1704_PWR_CTRL_BVALID_RISE	(1 << 2)
#define ISP1704_PWR_CTRL_BVALID_FALL	(1 << 3)
#define ISP1704_PWR_CTRL_DP_WKPU_EN	(1 << 4)
#define ISP1704_PWR_CTRL_VDAT_DET	(1 << 5)
#define ISP1704_PWR_CTRL_DPVSRC_EN	(1 << 6)
#define ISP1704_PWR_CTRL_HWDETECT	(1 << 7)

#define NXP_VENDOR_ID			0x04cc

static u16 isp170x_id[] = {
	0x1704,
	0x1707,
};

struct isp1704_charger {
	struct device			*dev;
	struct power_supply		*psy;
	struct power_supply_desc	psy_desc;
	struct gpio_desc		*enable_gpio;
	struct usb_phy			*phy;
	struct notifier_block		nb;
	struct work_struct		work;

	/* properties */
	char			model[8];
	unsigned		present:1;
	unsigned		online:1;
	unsigned		current_max;
};

static inline int isp1704_read(struct isp1704_charger *isp, u32 reg)
{
	return usb_phy_io_read(isp->phy, reg);
}

static inline int isp1704_write(struct isp1704_charger *isp, u32 reg, u32 val)
{
	return usb_phy_io_write(isp->phy, val, reg);
}

static void isp1704_charger_set_power(struct isp1704_charger *isp, bool on)
{
	gpiod_set_value(isp->enable_gpio, on);
}

/*
 * Determine is the charging port DCP (dedicated charger) or CDP (Host/HUB
 * chargers).
 *
 * REVISIT: The method is defined in Battery Charging Specification and is
 * applicable to any ULPI transceiver. Nothing isp170x specific here.
 */
static inline int isp1704_charger_type(struct isp1704_charger *isp)
{
	u8 reg;
	u8 func_ctrl;
	u8 otg_ctrl;
	int type = POWER_SUPPLY_TYPE_USB_DCP;

	func_ctrl = isp1704_read(isp, ULPI_FUNC_CTRL);
	otg_ctrl = isp1704_read(isp, ULPI_OTG_CTRL);

	/* disable pulldowns */
	reg = ULPI_OTG_CTRL_DM_PULLDOWN | ULPI_OTG_CTRL_DP_PULLDOWN;
	isp1704_write(isp, ULPI_CLR(ULPI_OTG_CTRL), reg);

	/* full speed */
	isp1704_write(isp, ULPI_CLR(ULPI_FUNC_CTRL),
			ULPI_FUNC_CTRL_XCVRSEL_MASK);
	isp1704_write(isp, ULPI_SET(ULPI_FUNC_CTRL),
			ULPI_FUNC_CTRL_FULL_SPEED);

	/* Enable strong pull-up on DP (1.5K) and reset */
	reg = ULPI_FUNC_CTRL_TERMSELECT | ULPI_FUNC_CTRL_RESET;
	isp1704_write(isp, ULPI_SET(ULPI_FUNC_CTRL), reg);
	usleep_range(1000, 2000);

	reg = isp1704_read(isp, ULPI_DEBUG);
	if ((reg & 3) != 3)
		type = POWER_SUPPLY_TYPE_USB_CDP;

	/* recover original state */
	isp1704_write(isp, ULPI_FUNC_CTRL, func_ctrl);
	isp1704_write(isp, ULPI_OTG_CTRL, otg_ctrl);

	return type;
}

/*
 * ISP1704 detects PS/2 adapters as charger. To make sure the detected charger
 * is actually a dedicated charger, the following steps need to be taken.
 */
static inline int isp1704_charger_verify(struct isp1704_charger *isp)
{
	int	ret = 0;
	u8	r;

	/* Reset the transceiver */
	r = isp1704_read(isp, ULPI_FUNC_CTRL);
	r |= ULPI_FUNC_CTRL_RESET;
	isp1704_write(isp, ULPI_FUNC_CTRL, r);
	usleep_range(1000, 2000);

	/* Set normal mode */
	r &= ~(ULPI_FUNC_CTRL_RESET | ULPI_FUNC_CTRL_OPMODE_MASK);
	isp1704_write(isp, ULPI_FUNC_CTRL, r);

	/* Clear the DP and DM pull-down bits */
	r = ULPI_OTG_CTRL_DP_PULLDOWN | ULPI_OTG_CTRL_DM_PULLDOWN;
	isp1704_write(isp, ULPI_CLR(ULPI_OTG_CTRL), r);

	/* Enable strong pull-up on DP (1.5K) and reset */
	r = ULPI_FUNC_CTRL_TERMSELECT | ULPI_FUNC_CTRL_RESET;
	isp1704_write(isp, ULPI_SET(ULPI_FUNC_CTRL), r);
	usleep_range(1000, 2000);

	/* Read the line state */
	if (!isp1704_read(isp, ULPI_DEBUG)) {
		/* Disable strong pull-up on DP (1.5K) */
		isp1704_write(isp, ULPI_CLR(ULPI_FUNC_CTRL),
				ULPI_FUNC_CTRL_TERMSELECT);
		return 1;
	}

	/* Is it a charger or PS/2 connection */

	/* Enable weak pull-up resistor on DP */
	isp1704_write(isp, ULPI_SET(ISP1704_PWR_CTRL),
			ISP1704_PWR_CTRL_DP_WKPU_EN);

	/* Disable strong pull-up on DP (1.5K) */
	isp1704_write(isp, ULPI_CLR(ULPI_FUNC_CTRL),
			ULPI_FUNC_CTRL_TERMSELECT);

	/* Enable weak pull-down resistor on DM */
	isp1704_write(isp, ULPI_SET(ULPI_OTG_CTRL),
			ULPI_OTG_CTRL_DM_PULLDOWN);

	/* It's a charger if the line states are clear */
	if (!(isp1704_read(isp, ULPI_DEBUG)))
		ret = 1;

	/* Disable weak pull-up resistor on DP */
	isp1704_write(isp, ULPI_CLR(ISP1704_PWR_CTRL),
			ISP1704_PWR_CTRL_DP_WKPU_EN);

	return ret;
}

static inline int isp1704_charger_detect(struct isp1704_charger *isp)
{
	unsigned long	timeout;
	u8		pwr_ctrl;
	int		ret = 0;

	pwr_ctrl = isp1704_read(isp, ISP1704_PWR_CTRL);

	/* set SW control bit in PWR_CTRL register */
	isp1704_write(isp, ISP1704_PWR_CTRL,
			ISP1704_PWR_CTRL_SWCTRL);

	/* enable manual charger detection */
	isp1704_write(isp, ULPI_SET(ISP1704_PWR_CTRL),
			ISP1704_PWR_CTRL_SWCTRL
			| ISP1704_PWR_CTRL_DPVSRC_EN);
	usleep_range(1000, 2000);

	timeout = jiffies + msecs_to_jiffies(300);
	do {
		/* Check if there is a charger */
		if (isp1704_read(isp, ISP1704_PWR_CTRL)
				& ISP1704_PWR_CTRL_VDAT_DET) {
			ret = isp1704_charger_verify(isp);
			break;
		}
	} while (!time_after(jiffies, timeout) && isp->online);

	/* recover original state */
	isp1704_write(isp, ISP1704_PWR_CTRL, pwr_ctrl);

	return ret;
}

static inline int isp1704_charger_detect_dcp(struct isp1704_charger *isp)
{
	if (isp1704_charger_detect(isp) &&
			isp1704_charger_type(isp) == POWER_SUPPLY_TYPE_USB_DCP)
		return true;
	else
		return false;
}

static void isp1704_charger_work(struct work_struct *data)
{
	struct isp1704_charger	*isp =
		container_of(data, struct isp1704_charger, work);
	static DEFINE_MUTEX(lock);

	mutex_lock(&lock);

	switch (isp->phy->last_event) {
	case USB_EVENT_VBUS:
		/* do not call wall charger detection more times */
		if (!isp->present) {
			isp->online = true;
			isp->present = 1;
			isp1704_charger_set_power(isp, 1);

			/* detect wall charger */
			if (isp1704_charger_detect_dcp(isp)) {
				isp->psy_desc.type = POWER_SUPPLY_TYPE_USB_DCP;
				isp->current_max = 1800;
			} else {
				isp->psy_desc.type = POWER_SUPPLY_TYPE_USB;
				isp->current_max = 500;
			}

			/* enable data pullups */
			if (isp->phy->otg->gadget)
				usb_gadget_connect(isp->phy->otg->gadget);
		}

		if (isp->psy_desc.type != POWER_SUPPLY_TYPE_USB_DCP) {
			/*
			 * Only 500mA here or high speed chirp
			 * handshaking may break
			 */
			if (isp->current_max > 500)
				isp->current_max = 500;

			if (isp->current_max > 100)
				isp->psy_desc.type = POWER_SUPPLY_TYPE_USB_CDP;
		}
		break;
	case USB_EVENT_NONE:
		isp->online = false;
		isp->present = 0;
		isp->current_max = 0;
		isp->psy_desc.type = POWER_SUPPLY_TYPE_USB;

		/*
		 * Disable data pullups. We need to prevent the controller from
		 * enumerating.
		 *
		 * FIXME: This is here to allow charger detection with Host/HUB
		 * chargers. The pullups may be enabled elsewhere, so this can
		 * not be the final solution.
		 */
		if (isp->phy->otg->gadget)
			usb_gadget_disconnect(isp->phy->otg->gadget);

		isp1704_charger_set_power(isp, 0);
		break;
	default:
		goto out;
	}

	power_supply_changed(isp->psy);
out:
	mutex_unlock(&lock);
}

static int isp1704_notifier_call(struct notifier_block *nb,
		unsigned long val, void *v)
{
	struct isp1704_charger *isp =
		container_of(nb, struct isp1704_charger, nb);

	schedule_work(&isp->work);

	return NOTIFY_OK;
}

static int isp1704_charger_get_property(struct power_supply *psy,
				enum power_supply_property psp,
				union power_supply_propval *val)
{
	struct isp1704_charger *isp = power_supply_get_drvdata(psy);

	switch (psp) {
	case POWER_SUPPLY_PROP_PRESENT:
		val->intval = isp->present;
		break;
	case POWER_SUPPLY_PROP_ONLINE:
		val->intval = isp->online;
		break;
	case POWER_SUPPLY_PROP_CURRENT_MAX:
		val->intval = isp->current_max;
		break;
	case POWER_SUPPLY_PROP_MODEL_NAME:
		val->strval = isp->model;
		break;
	case POWER_SUPPLY_PROP_MANUFACTURER:
		val->strval = "NXP";
		break;
	default:
		return -EINVAL;
	}
	return 0;
}

static enum power_supply_property power_props[] = {
	POWER_SUPPLY_PROP_PRESENT,
	POWER_SUPPLY_PROP_ONLINE,
	POWER_SUPPLY_PROP_CURRENT_MAX,
	POWER_SUPPLY_PROP_MODEL_NAME,
	POWER_SUPPLY_PROP_MANUFACTURER,
};

static inline int isp1704_test_ulpi(struct isp1704_charger *isp)
{
	int vendor;
	int product;
	int i;
	int ret;

	/* Test ULPI interface */
	ret = isp1704_write(isp, ULPI_SCRATCH, 0xaa);
	if (ret < 0)
		return ret;

	ret = isp1704_read(isp, ULPI_SCRATCH);
	if (ret < 0)
		return ret;

	if (ret != 0xaa)
		return -ENODEV;

	/* Verify the product and vendor id matches */
	vendor = isp1704_read(isp, ULPI_VENDOR_ID_LOW);
	vendor |= isp1704_read(isp, ULPI_VENDOR_ID_HIGH) << 8;
	if (vendor != NXP_VENDOR_ID)
		return -ENODEV;

	product = isp1704_read(isp, ULPI_PRODUCT_ID_LOW);
	product |= isp1704_read(isp, ULPI_PRODUCT_ID_HIGH) << 8;

	for (i = 0; i < ARRAY_SIZE(isp170x_id); i++) {
		if (product == isp170x_id[i]) {
			sprintf(isp->model, "isp%x", product);
			return product;
		}
	}

	dev_err(isp->dev, "product id %x not matching known ids", product);

	return -ENODEV;
}

static int isp1704_charger_probe(struct platform_device *pdev)
{
	struct isp1704_charger	*isp;
	int			ret = -ENODEV;
	struct power_supply_config psy_cfg = {};

	isp = devm_kzalloc(&pdev->dev, sizeof(*isp), GFP_KERNEL);
	if (!isp)
		return -ENOMEM;

	isp->enable_gpio = devm_gpiod_get(&pdev->dev, "nxp,enable",
					  GPIOD_OUT_HIGH);
	if (IS_ERR(isp->enable_gpio)) {
		ret = PTR_ERR(isp->enable_gpio);
		dev_err(&pdev->dev, "Could not get reset gpio: %d\n", ret);
		return ret;
	}

	if (pdev->dev.of_node)
		isp->phy = devm_usb_get_phy_by_phandle(&pdev->dev, "usb-phy", 0);
	else
		isp->phy = devm_usb_get_phy(&pdev->dev, USB_PHY_TYPE_USB2);

	if (IS_ERR(isp->phy)) {
		ret = PTR_ERR(isp->phy);
		dev_err(&pdev->dev, "usb_get_phy failed\n");
		goto fail0;
	}

	isp->dev = &pdev->dev;
	platform_set_drvdata(pdev, isp);

	isp1704_charger_set_power(isp, 1);

	ret = isp1704_test_ulpi(isp);
	if (ret < 0) {
		dev_err(&pdev->dev, "isp1704_test_ulpi failed\n");
		goto fail1;
	}

	isp->psy_desc.name		= "isp1704";
	isp->psy_desc.type		= POWER_SUPPLY_TYPE_USB;
	isp->psy_desc.properties	= power_props;
	isp->psy_desc.num_properties	= ARRAY_SIZE(power_props);
	isp->psy_desc.get_property	= isp1704_charger_get_property;

	psy_cfg.drv_data		= isp;

	isp->psy = power_supply_register(isp->dev, &isp->psy_desc, &psy_cfg);
	if (IS_ERR(isp->psy)) {
		ret = PTR_ERR(isp->psy);
		dev_err(&pdev->dev, "power_supply_register failed\n");
		goto fail1;
	}

	/*
	 * REVISIT: using work in order to allow the usb notifications to be
	 * made atomically in the future.
	 */
	INIT_WORK(&isp->work, isp1704_charger_work);

	isp->nb.notifier_call = isp1704_notifier_call;

	ret = usb_register_notifier(isp->phy, &isp->nb);
	if (ret) {
		dev_err(&pdev->dev, "usb_register_notifier failed\n");
		goto fail2;
	}

	dev_info(isp->dev, "registered with product id %s\n", isp->model);

	/*
	 * Taking over the D+ pullup.
	 *
	 * FIXME: The device will be disconnected if it was already
	 * enumerated. The charger driver should be always loaded before any
	 * gadget is loaded.
	 */
	if (isp->phy->otg->gadget)
		usb_gadget_disconnect(isp->phy->otg->gadget);

	if (isp->phy->last_event == USB_EVENT_NONE)
		isp1704_charger_set_power(isp, 0);

	/* Detect charger if VBUS is valid (the cable was already plugged). */
	if (isp->phy->last_event == USB_EVENT_VBUS &&
			!isp->phy->otg->default_a)
		schedule_work(&isp->work);

	return 0;
fail2:
	power_supply_unregister(isp->psy);
fail1:
	isp1704_charger_set_power(isp, 0);
fail0:
	dev_err(&pdev->dev, "failed to register isp1704 with error %d\n", ret);

	return ret;
}

static int isp1704_charger_remove(struct platform_device *pdev)
{
	struct isp1704_charger *isp = platform_get_drvdata(pdev);

	usb_unregister_notifier(isp->phy, &isp->nb);
	power_supply_unregister(isp->psy);
	isp1704_charger_set_power(isp, 0);

	return 0;
}

#ifdef CONFIG_OF
static const struct of_device_id omap_isp1704_of_match[] = {
	{ .compatible = "nxp,isp1704", },
	{ .compatible = "nxp,isp1707", },
	{},
};
MODULE_DEVICE_TABLE(of, omap_isp1704_of_match);
#endif

static struct platform_driver isp1704_charger_driver = {
	.driver = {
		.name = "isp1704_charger",
		.of_match_table = of_match_ptr(omap_isp1704_of_match),
	},
	.probe = isp1704_charger_probe,
	.remove = isp1704_charger_remove,
};

module_platform_driver(isp1704_charger_driver);

MODULE_ALIAS("platform:isp1704_charger");
MODULE_AUTHOR("Nokia Corporation");
MODULE_DESCRIPTION("ISP170x USB Charger driver");
MODULE_LICENSE("GPL");
