// SPDX-License-Identifier: GPL-2.0
/*
 * gpio-vbus.c - simple GPIO VBUS sensing driver for B peripheral devices
 *
 * Copyright (c) 2008 Philipp Zabel <philipp.zabel@gmail.com>
 */

#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/gpio/consumer.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/usb.h>
#include <linux/workqueue.h>

#include <linux/regulator/consumer.h>

#include <linux/usb/gadget.h>
#include <linux/usb/otg.h>


/*
 * A simple GPIO VBUS sensing driver for B peripheral only devices
 * with internal transceivers. It can control a D+ pullup GPIO and
 * a regulator to limit the current drawn from VBUS.
 *
 * Needs to be loaded before the UDC driver that will use it.
 */
struct gpio_vbus_data {
	struct gpio_desc	*vbus_gpiod;
	struct gpio_desc	*pullup_gpiod;
	struct usb_phy		phy;
	struct device          *dev;
	struct regulator       *vbus_draw;
	int			vbus_draw_enabled;
	unsigned		mA;
	struct delayed_work	work;
	int			vbus;
	int			irq;
};


/*
 * This driver relies on "both edges" triggering.  VBUS has 100 msec to
 * stabilize, so the peripheral controller driver may need to cope with
 * some bouncing due to current surges (e.g. charging local capacitance)
 * and contact chatter.
 *
 * REVISIT in desperate straits, toggling between rising and falling
 * edges might be workable.
 */
#define VBUS_IRQ_FLAGS \
	(IRQF_SHARED | IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING)


/* interface to regulator framework */
static void set_vbus_draw(struct gpio_vbus_data *gpio_vbus, unsigned mA)
{
	struct regulator *vbus_draw = gpio_vbus->vbus_draw;
	int enabled;
	int ret;

	if (!vbus_draw)
		return;

	enabled = gpio_vbus->vbus_draw_enabled;
	if (mA) {
		regulator_set_current_limit(vbus_draw, 0, 1000 * mA);
		if (!enabled) {
			ret = regulator_enable(vbus_draw);
			if (ret < 0)
				return;
			gpio_vbus->vbus_draw_enabled = 1;
		}
	} else {
		if (enabled) {
			ret = regulator_disable(vbus_draw);
			if (ret < 0)
				return;
			gpio_vbus->vbus_draw_enabled = 0;
		}
	}
	gpio_vbus->mA = mA;
}

static int is_vbus_powered(struct gpio_vbus_data *gpio_vbus)
{
	return gpiod_get_value(gpio_vbus->vbus_gpiod);
}

static void gpio_vbus_work(struct work_struct *work)
{
	struct gpio_vbus_data *gpio_vbus =
		container_of(work, struct gpio_vbus_data, work.work);
	int status, vbus;

	if (!gpio_vbus->phy.otg->gadget)
		return;

	vbus = is_vbus_powered(gpio_vbus);
	if ((vbus ^ gpio_vbus->vbus) == 0)
		return;
	gpio_vbus->vbus = vbus;

	/* Peripheral controllers which manage the pullup themselves won't have
	 * a pullup GPIO configured here.  If it's configured here, we'll do
	 * what isp1301_omap::b_peripheral() does and enable the pullup here...
	 * although that may complicate usb_gadget_{,dis}connect() support.
	 */

	if (vbus) {
		status = USB_EVENT_VBUS;
		gpio_vbus->phy.otg->state = OTG_STATE_B_PERIPHERAL;
		gpio_vbus->phy.last_event = status;
		usb_gadget_vbus_connect(gpio_vbus->phy.otg->gadget);

		/* drawing a "unit load" is *always* OK, except for OTG */
		set_vbus_draw(gpio_vbus, 100);

		/* optionally enable D+ pullup */
		if (gpio_vbus->pullup_gpiod)
			gpiod_set_value(gpio_vbus->pullup_gpiod, 1);

		atomic_notifier_call_chain(&gpio_vbus->phy.notifier,
					   status, gpio_vbus->phy.otg->gadget);
		usb_phy_set_event(&gpio_vbus->phy, USB_EVENT_ENUMERATED);
	} else {
		/* optionally disable D+ pullup */
		if (gpio_vbus->pullup_gpiod)
			gpiod_set_value(gpio_vbus->pullup_gpiod, 0);

		set_vbus_draw(gpio_vbus, 0);

		usb_gadget_vbus_disconnect(gpio_vbus->phy.otg->gadget);
		status = USB_EVENT_NONE;
		gpio_vbus->phy.otg->state = OTG_STATE_B_IDLE;
		gpio_vbus->phy.last_event = status;

		atomic_notifier_call_chain(&gpio_vbus->phy.notifier,
					   status, gpio_vbus->phy.otg->gadget);
		usb_phy_set_event(&gpio_vbus->phy, USB_EVENT_NONE);
	}
}

/* VBUS change IRQ handler */
static irqreturn_t gpio_vbus_irq(int irq, void *data)
{
	struct platform_device *pdev = data;
	struct gpio_vbus_data *gpio_vbus = platform_get_drvdata(pdev);
	struct usb_otg *otg = gpio_vbus->phy.otg;

	dev_dbg(&pdev->dev, "VBUS %s (gadget: %s)\n",
		is_vbus_powered(gpio_vbus) ? "supplied" : "inactive",
		otg->gadget ? otg->gadget->name : "none");

	if (otg->gadget)
		schedule_delayed_work(&gpio_vbus->work, msecs_to_jiffies(100));

	return IRQ_HANDLED;
}

/* OTG transceiver interface */

/* bind/unbind the peripheral controller */
static int gpio_vbus_set_peripheral(struct usb_otg *otg,
					struct usb_gadget *gadget)
{
	struct gpio_vbus_data *gpio_vbus;
	struct platform_device *pdev;

	gpio_vbus = container_of(otg->usb_phy, struct gpio_vbus_data, phy);
	pdev = to_platform_device(gpio_vbus->dev);

	if (!gadget) {
		dev_dbg(&pdev->dev, "unregistering gadget '%s'\n",
			otg->gadget->name);

		/* optionally disable D+ pullup */
		if (gpio_vbus->pullup_gpiod)
			gpiod_set_value(gpio_vbus->pullup_gpiod, 0);

		set_vbus_draw(gpio_vbus, 0);

		usb_gadget_vbus_disconnect(otg->gadget);
		otg->state = OTG_STATE_UNDEFINED;

		otg->gadget = NULL;
		return 0;
	}

	otg->gadget = gadget;
	dev_dbg(&pdev->dev, "registered gadget '%s'\n", gadget->name);

	/* initialize connection state */
	gpio_vbus->vbus = 0; /* start with disconnected */
	gpio_vbus_irq(gpio_vbus->irq, pdev);
	return 0;
}

/* effective for B devices, ignored for A-peripheral */
static int gpio_vbus_set_power(struct usb_phy *phy, unsigned mA)
{
	struct gpio_vbus_data *gpio_vbus;

	gpio_vbus = container_of(phy, struct gpio_vbus_data, phy);

	if (phy->otg->state == OTG_STATE_B_PERIPHERAL)
		set_vbus_draw(gpio_vbus, mA);
	return 0;
}

/* for non-OTG B devices: set/clear transceiver suspend mode */
static int gpio_vbus_set_suspend(struct usb_phy *phy, int suspend)
{
	struct gpio_vbus_data *gpio_vbus;

	gpio_vbus = container_of(phy, struct gpio_vbus_data, phy);

	/* draw max 0 mA from vbus in suspend mode; or the previously
	 * recorded amount of current if not suspended
	 *
	 * NOTE: high powered configs (mA > 100) may draw up to 2.5 mA
	 * if they're wake-enabled ... we don't handle that yet.
	 */
	return gpio_vbus_set_power(phy, suspend ? 0 : gpio_vbus->mA);
}

/* platform driver interface */

static int gpio_vbus_probe(struct platform_device *pdev)
{
	struct gpio_vbus_data *gpio_vbus;
	struct resource *res;
	struct device *dev = &pdev->dev;
	int err, irq;
	unsigned long irqflags;

	gpio_vbus = devm_kzalloc(&pdev->dev, sizeof(struct gpio_vbus_data),
				 GFP_KERNEL);
	if (!gpio_vbus)
		return -ENOMEM;

	gpio_vbus->phy.otg = devm_kzalloc(&pdev->dev, sizeof(struct usb_otg),
					  GFP_KERNEL);
	if (!gpio_vbus->phy.otg)
		return -ENOMEM;

	platform_set_drvdata(pdev, gpio_vbus);
	gpio_vbus->dev = &pdev->dev;
	gpio_vbus->phy.label = "gpio-vbus";
	gpio_vbus->phy.dev = gpio_vbus->dev;
	gpio_vbus->phy.set_power = gpio_vbus_set_power;
	gpio_vbus->phy.set_suspend = gpio_vbus_set_suspend;

	gpio_vbus->phy.otg->state = OTG_STATE_UNDEFINED;
	gpio_vbus->phy.otg->usb_phy = &gpio_vbus->phy;
	gpio_vbus->phy.otg->set_peripheral = gpio_vbus_set_peripheral;

	/* Look up the VBUS sensing GPIO */
	gpio_vbus->vbus_gpiod = devm_gpiod_get(dev, "vbus", GPIOD_IN);
	if (IS_ERR(gpio_vbus->vbus_gpiod)) {
		err = PTR_ERR(gpio_vbus->vbus_gpiod);
		dev_err(&pdev->dev, "can't request vbus gpio, err: %d\n", err);
		return err;
	}
	gpiod_set_consumer_name(gpio_vbus->vbus_gpiod, "vbus_detect");

	res = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	if (res) {
		irq = res->start;
		irqflags = (res->flags & IRQF_TRIGGER_MASK) | IRQF_SHARED;
	} else {
		irq = gpiod_to_irq(gpio_vbus->vbus_gpiod);
		irqflags = VBUS_IRQ_FLAGS;
	}

	gpio_vbus->irq = irq;

	/*
	 * The VBUS sensing GPIO should have a pulldown, which will normally be
	 * part of a resistor ladder turning a 4.0V-5.25V level on VBUS into a
	 * value the GPIO detects as active. Some systems will use comparators.
	 * Get the optional D+ or D- pullup GPIO. If the data line pullup is
	 * in use, initialize it to "not pulling up"
	 */
	gpio_vbus->pullup_gpiod = devm_gpiod_get_optional(dev, "pullup",
							  GPIOD_OUT_LOW);
	if (IS_ERR(gpio_vbus->pullup_gpiod)) {
		err = PTR_ERR(gpio_vbus->pullup_gpiod);
		dev_err(&pdev->dev, "can't request pullup gpio, err: %d\n",
			err);
		return err;
	}
	if (gpio_vbus->pullup_gpiod)
		gpiod_set_consumer_name(gpio_vbus->pullup_gpiod, "udc_pullup");

	err = devm_request_irq(&pdev->dev, irq, gpio_vbus_irq, irqflags,
			       "vbus_detect", pdev);
	if (err) {
		dev_err(&pdev->dev, "can't request irq %i, err: %d\n",
			irq, err);
		return err;
	}

	INIT_DELAYED_WORK(&gpio_vbus->work, gpio_vbus_work);

	gpio_vbus->vbus_draw = devm_regulator_get(&pdev->dev, "vbus_draw");
	if (IS_ERR(gpio_vbus->vbus_draw)) {
		dev_dbg(&pdev->dev, "can't get vbus_draw regulator, err: %ld\n",
			PTR_ERR(gpio_vbus->vbus_draw));
		gpio_vbus->vbus_draw = NULL;
	}

	/* only active when a gadget is registered */
	err = usb_add_phy(&gpio_vbus->phy, USB_PHY_TYPE_USB2);
	if (err) {
		dev_err(&pdev->dev, "can't register transceiver, err: %d\n",
			err);
		return err;
	}

	/* TODO: wakeup could be enabled here with device_init_wakeup(dev, 1) */

	return 0;
}

static int gpio_vbus_remove(struct platform_device *pdev)
{
	struct gpio_vbus_data *gpio_vbus = platform_get_drvdata(pdev);

	device_init_wakeup(&pdev->dev, 0);
	cancel_delayed_work_sync(&gpio_vbus->work);

	usb_remove_phy(&gpio_vbus->phy);

	return 0;
}

#ifdef CONFIG_PM
static int gpio_vbus_pm_suspend(struct device *dev)
{
	struct gpio_vbus_data *gpio_vbus = dev_get_drvdata(dev);

	if (device_may_wakeup(dev))
		enable_irq_wake(gpio_vbus->irq);

	return 0;
}

static int gpio_vbus_pm_resume(struct device *dev)
{
	struct gpio_vbus_data *gpio_vbus = dev_get_drvdata(dev);

	if (device_may_wakeup(dev))
		disable_irq_wake(gpio_vbus->irq);

	return 0;
}

static const struct dev_pm_ops gpio_vbus_dev_pm_ops = {
	.suspend	= gpio_vbus_pm_suspend,
	.resume		= gpio_vbus_pm_resume,
};
#endif

MODULE_ALIAS("platform:gpio-vbus");

static struct platform_driver gpio_vbus_driver = {
	.driver = {
		.name  = "gpio-vbus",
#ifdef CONFIG_PM
		.pm = &gpio_vbus_dev_pm_ops,
#endif
	},
	.probe		= gpio_vbus_probe,
	.remove		= gpio_vbus_remove,
};

module_platform_driver(gpio_vbus_driver);

MODULE_DESCRIPTION("simple GPIO controlled OTG transceiver driver");
MODULE_AUTHOR("Philipp Zabel");
MODULE_LICENSE("GPL");
