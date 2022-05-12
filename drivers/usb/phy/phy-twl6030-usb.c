// SPDX-License-Identifier: GPL-2.0+
/*
 * twl6030_usb - TWL6030 USB transceiver, talking to OMAP OTG driver.
 *
 * Copyright (C) 2010 Texas Instruments Incorporated - https://www.ti.com
 *
 * Author: Hema HK <hemahk@ti.com>
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/usb/musb.h>
#include <linux/usb/phy_companion.h>
#include <linux/phy/omap_usb.h>
#include <linux/mfd/twl.h>
#include <linux/regulator/consumer.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/of.h>

/* usb register definitions */
#define USB_VENDOR_ID_LSB		0x00
#define USB_VENDOR_ID_MSB		0x01
#define USB_PRODUCT_ID_LSB		0x02
#define USB_PRODUCT_ID_MSB		0x03
#define USB_VBUS_CTRL_SET		0x04
#define USB_VBUS_CTRL_CLR		0x05
#define USB_ID_CTRL_SET			0x06
#define USB_ID_CTRL_CLR			0x07
#define USB_VBUS_INT_SRC		0x08
#define USB_VBUS_INT_LATCH_SET		0x09
#define USB_VBUS_INT_LATCH_CLR		0x0A
#define USB_VBUS_INT_EN_LO_SET		0x0B
#define USB_VBUS_INT_EN_LO_CLR		0x0C
#define USB_VBUS_INT_EN_HI_SET		0x0D
#define USB_VBUS_INT_EN_HI_CLR		0x0E
#define USB_ID_INT_SRC			0x0F
#define USB_ID_INT_LATCH_SET		0x10
#define USB_ID_INT_LATCH_CLR		0x11

#define USB_ID_INT_EN_LO_SET		0x12
#define USB_ID_INT_EN_LO_CLR		0x13
#define USB_ID_INT_EN_HI_SET		0x14
#define USB_ID_INT_EN_HI_CLR		0x15
#define USB_OTG_ADP_CTRL		0x16
#define USB_OTG_ADP_HIGH		0x17
#define USB_OTG_ADP_LOW			0x18
#define USB_OTG_ADP_RISE		0x19
#define USB_OTG_REVISION		0x1A

/* to be moved to LDO */
#define TWL6030_MISC2			0xE5
#define TWL6030_CFG_LDO_PD2		0xF5
#define TWL6030_BACKUP_REG		0xFA

#define STS_HW_CONDITIONS		0x21

/* In module TWL6030_MODULE_PM_MASTER */
#define STS_HW_CONDITIONS		0x21
#define STS_USB_ID			BIT(2)

/* In module TWL6030_MODULE_PM_RECEIVER */
#define VUSB_CFG_TRANS			0x71
#define VUSB_CFG_STATE			0x72
#define VUSB_CFG_VOLTAGE		0x73

/* in module TWL6030_MODULE_MAIN_CHARGE */

#define CHARGERUSB_CTRL1		0x8

#define CONTROLLER_STAT1		0x03
#define	VBUS_DET			BIT(2)

struct twl6030_usb {
	struct phy_companion	comparator;
	struct device		*dev;

	/* for vbus reporting with irqs disabled */
	spinlock_t		lock;

	struct regulator		*usb3v3;

	/* used to check initial cable status after probe */
	struct delayed_work	get_status_work;

	/* used to set vbus, in atomic path */
	struct work_struct	set_vbus_work;

	int			irq1;
	int			irq2;
	enum musb_vbus_id_status linkstat;
	u8			asleep;
	bool			vbus_enable;
};

#define	comparator_to_twl(x) container_of((x), struct twl6030_usb, comparator)

/*-------------------------------------------------------------------------*/

static inline int twl6030_writeb(struct twl6030_usb *twl, u8 module,
						u8 data, u8 address)
{
	int ret = 0;

	ret = twl_i2c_write_u8(module, data, address);
	if (ret < 0)
		dev_err(twl->dev,
			"Write[0x%x] Error %d\n", address, ret);
	return ret;
}

static inline u8 twl6030_readb(struct twl6030_usb *twl, u8 module, u8 address)
{
	u8 data;
	int ret;

	ret = twl_i2c_read_u8(module, &data, address);
	if (ret >= 0)
		ret = data;
	else
		dev_err(twl->dev,
			"readb[0x%x,0x%x] Error %d\n",
					module, address, ret);
	return ret;
}

static int twl6030_start_srp(struct phy_companion *comparator)
{
	struct twl6030_usb *twl = comparator_to_twl(comparator);

	twl6030_writeb(twl, TWL_MODULE_USB, 0x24, USB_VBUS_CTRL_SET);
	twl6030_writeb(twl, TWL_MODULE_USB, 0x84, USB_VBUS_CTRL_SET);

	mdelay(100);
	twl6030_writeb(twl, TWL_MODULE_USB, 0xa0, USB_VBUS_CTRL_CLR);

	return 0;
}

static int twl6030_usb_ldo_init(struct twl6030_usb *twl)
{
	/* Set to OTG_REV 1.3 and turn on the ID_WAKEUP_COMP */
	twl6030_writeb(twl, TWL6030_MODULE_ID0, 0x1, TWL6030_BACKUP_REG);

	/* Program CFG_LDO_PD2 register and set VUSB bit */
	twl6030_writeb(twl, TWL6030_MODULE_ID0, 0x1, TWL6030_CFG_LDO_PD2);

	/* Program MISC2 register and set bit VUSB_IN_VBAT */
	twl6030_writeb(twl, TWL6030_MODULE_ID0, 0x10, TWL6030_MISC2);

	twl->usb3v3 = regulator_get(twl->dev, "usb");
	if (IS_ERR(twl->usb3v3))
		return -ENODEV;

	/* Program the USB_VBUS_CTRL_SET and set VBUS_ACT_COMP bit */
	twl6030_writeb(twl, TWL_MODULE_USB, 0x4, USB_VBUS_CTRL_SET);

	/*
	 * Program the USB_ID_CTRL_SET register to enable GND drive
	 * and the ID comparators
	 */
	twl6030_writeb(twl, TWL_MODULE_USB, 0x14, USB_ID_CTRL_SET);

	return 0;
}

static ssize_t vbus_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	struct twl6030_usb *twl = dev_get_drvdata(dev);
	unsigned long flags;
	int ret = -EINVAL;

	spin_lock_irqsave(&twl->lock, flags);

	switch (twl->linkstat) {
	case MUSB_VBUS_VALID:
	       ret = snprintf(buf, PAGE_SIZE, "vbus\n");
	       break;
	case MUSB_ID_GROUND:
	       ret = snprintf(buf, PAGE_SIZE, "id\n");
	       break;
	case MUSB_VBUS_OFF:
	       ret = snprintf(buf, PAGE_SIZE, "none\n");
	       break;
	default:
	       ret = snprintf(buf, PAGE_SIZE, "UNKNOWN\n");
	}
	spin_unlock_irqrestore(&twl->lock, flags);

	return ret;
}
static DEVICE_ATTR_RO(vbus);

static struct attribute *twl6030_attrs[] = {
	&dev_attr_vbus.attr,
	NULL,
};
ATTRIBUTE_GROUPS(twl6030);

static irqreturn_t twl6030_usb_irq(int irq, void *_twl)
{
	struct twl6030_usb *twl = _twl;
	enum musb_vbus_id_status status = MUSB_UNKNOWN;
	u8 vbus_state, hw_state;
	int ret;

	hw_state = twl6030_readb(twl, TWL6030_MODULE_ID0, STS_HW_CONDITIONS);

	vbus_state = twl6030_readb(twl, TWL_MODULE_MAIN_CHARGE,
						CONTROLLER_STAT1);
	if (!(hw_state & STS_USB_ID)) {
		if (vbus_state & VBUS_DET) {
			ret = regulator_enable(twl->usb3v3);
			if (ret)
				dev_err(twl->dev, "Failed to enable usb3v3\n");

			twl->asleep = 1;
			status = MUSB_VBUS_VALID;
			twl->linkstat = status;
			ret = musb_mailbox(status);
			if (ret)
				twl->linkstat = MUSB_UNKNOWN;
		} else {
			if (twl->linkstat != MUSB_UNKNOWN) {
				status = MUSB_VBUS_OFF;
				twl->linkstat = status;
				ret = musb_mailbox(status);
				if (ret)
					twl->linkstat = MUSB_UNKNOWN;
				if (twl->asleep) {
					regulator_disable(twl->usb3v3);
					twl->asleep = 0;
				}
			}
		}
	}
	sysfs_notify(&twl->dev->kobj, NULL, "vbus");

	return IRQ_HANDLED;
}

static irqreturn_t twl6030_usbotg_irq(int irq, void *_twl)
{
	struct twl6030_usb *twl = _twl;
	enum musb_vbus_id_status status = MUSB_UNKNOWN;
	u8 hw_state;
	int ret;

	hw_state = twl6030_readb(twl, TWL6030_MODULE_ID0, STS_HW_CONDITIONS);

	if (hw_state & STS_USB_ID) {
		ret = regulator_enable(twl->usb3v3);
		if (ret)
			dev_err(twl->dev, "Failed to enable usb3v3\n");

		twl->asleep = 1;
		twl6030_writeb(twl, TWL_MODULE_USB, 0x1, USB_ID_INT_EN_HI_CLR);
		twl6030_writeb(twl, TWL_MODULE_USB, 0x10, USB_ID_INT_EN_HI_SET);
		status = MUSB_ID_GROUND;
		twl->linkstat = status;
		ret = musb_mailbox(status);
		if (ret)
			twl->linkstat = MUSB_UNKNOWN;
	} else  {
		twl6030_writeb(twl, TWL_MODULE_USB, 0x10, USB_ID_INT_EN_HI_CLR);
		twl6030_writeb(twl, TWL_MODULE_USB, 0x1, USB_ID_INT_EN_HI_SET);
	}
	twl6030_writeb(twl, TWL_MODULE_USB, status, USB_ID_INT_LATCH_CLR);

	return IRQ_HANDLED;
}

static void twl6030_status_work(struct work_struct *work)
{
	struct twl6030_usb *twl = container_of(work, struct twl6030_usb,
					       get_status_work.work);

	twl6030_usb_irq(twl->irq2, twl);
	twl6030_usbotg_irq(twl->irq1, twl);
}

static int twl6030_enable_irq(struct twl6030_usb *twl)
{
	twl6030_writeb(twl, TWL_MODULE_USB, 0x1, USB_ID_INT_EN_HI_SET);
	twl6030_interrupt_unmask(0x05, REG_INT_MSK_LINE_C);
	twl6030_interrupt_unmask(0x05, REG_INT_MSK_STS_C);

	twl6030_interrupt_unmask(TWL6030_CHARGER_CTRL_INT_MASK,
				REG_INT_MSK_LINE_C);
	twl6030_interrupt_unmask(TWL6030_CHARGER_CTRL_INT_MASK,
				REG_INT_MSK_STS_C);

	return 0;
}

static void otg_set_vbus_work(struct work_struct *data)
{
	struct twl6030_usb *twl = container_of(data, struct twl6030_usb,
								set_vbus_work);

	/*
	 * Start driving VBUS. Set OPA_MODE bit in CHARGERUSB_CTRL1
	 * register. This enables boost mode.
	 */

	if (twl->vbus_enable)
		twl6030_writeb(twl, TWL_MODULE_MAIN_CHARGE, 0x40,
							CHARGERUSB_CTRL1);
	else
		twl6030_writeb(twl, TWL_MODULE_MAIN_CHARGE, 0x00,
							CHARGERUSB_CTRL1);
}

static int twl6030_set_vbus(struct phy_companion *comparator, bool enabled)
{
	struct twl6030_usb *twl = comparator_to_twl(comparator);

	twl->vbus_enable = enabled;
	schedule_work(&twl->set_vbus_work);

	return 0;
}

static int twl6030_usb_probe(struct platform_device *pdev)
{
	u32 ret;
	struct twl6030_usb	*twl;
	int			status, err;
	struct device_node	*np = pdev->dev.of_node;
	struct device		*dev = &pdev->dev;

	if (!np) {
		dev_err(dev, "no DT info\n");
		return -EINVAL;
	}

	twl = devm_kzalloc(dev, sizeof(*twl), GFP_KERNEL);
	if (!twl)
		return -ENOMEM;

	twl->dev		= &pdev->dev;
	twl->irq1		= platform_get_irq(pdev, 0);
	twl->irq2		= platform_get_irq(pdev, 1);
	twl->linkstat		= MUSB_UNKNOWN;

	twl->comparator.set_vbus	= twl6030_set_vbus;
	twl->comparator.start_srp	= twl6030_start_srp;

	ret = omap_usb2_set_comparator(&twl->comparator);
	if (ret == -ENODEV) {
		dev_info(&pdev->dev, "phy not ready, deferring probe");
		return -EPROBE_DEFER;
	}

	/* init spinlock for workqueue */
	spin_lock_init(&twl->lock);

	err = twl6030_usb_ldo_init(twl);
	if (err) {
		dev_err(&pdev->dev, "ldo init failed\n");
		return err;
	}

	platform_set_drvdata(pdev, twl);

	INIT_WORK(&twl->set_vbus_work, otg_set_vbus_work);
	INIT_DELAYED_WORK(&twl->get_status_work, twl6030_status_work);

	status = request_threaded_irq(twl->irq1, NULL, twl6030_usbotg_irq,
			IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING | IRQF_ONESHOT,
			"twl6030_usb", twl);
	if (status < 0) {
		dev_err(&pdev->dev, "can't get IRQ %d, err %d\n",
			twl->irq1, status);
		goto err_put_regulator;
	}

	status = request_threaded_irq(twl->irq2, NULL, twl6030_usb_irq,
			IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING | IRQF_ONESHOT,
			"twl6030_usb", twl);
	if (status < 0) {
		dev_err(&pdev->dev, "can't get IRQ %d, err %d\n",
			twl->irq2, status);
		goto err_free_irq1;
	}

	twl->asleep = 0;
	twl6030_enable_irq(twl);
	schedule_delayed_work(&twl->get_status_work, HZ);
	dev_info(&pdev->dev, "Initialized TWL6030 USB module\n");

	return 0;

err_free_irq1:
	free_irq(twl->irq1, twl);
err_put_regulator:
	regulator_put(twl->usb3v3);

	return status;
}

static int twl6030_usb_remove(struct platform_device *pdev)
{
	struct twl6030_usb *twl = platform_get_drvdata(pdev);

	cancel_delayed_work_sync(&twl->get_status_work);
	twl6030_interrupt_mask(TWL6030_USBOTG_INT_MASK,
		REG_INT_MSK_LINE_C);
	twl6030_interrupt_mask(TWL6030_USBOTG_INT_MASK,
			REG_INT_MSK_STS_C);
	free_irq(twl->irq1, twl);
	free_irq(twl->irq2, twl);
	regulator_put(twl->usb3v3);
	cancel_work_sync(&twl->set_vbus_work);

	return 0;
}

static const struct of_device_id twl6030_usb_id_table[] = {
	{ .compatible = "ti,twl6030-usb" },
	{}
};
MODULE_DEVICE_TABLE(of, twl6030_usb_id_table);

static struct platform_driver twl6030_usb_driver = {
	.probe		= twl6030_usb_probe,
	.remove		= twl6030_usb_remove,
	.driver		= {
		.name	= "twl6030_usb",
		.of_match_table = of_match_ptr(twl6030_usb_id_table),
		.dev_groups = twl6030_groups,
	},
};

static int __init twl6030_usb_init(void)
{
	return platform_driver_register(&twl6030_usb_driver);
}
subsys_initcall(twl6030_usb_init);

static void __exit twl6030_usb_exit(void)
{
	platform_driver_unregister(&twl6030_usb_driver);
}
module_exit(twl6030_usb_exit);

MODULE_ALIAS("platform:twl6030_usb");
MODULE_AUTHOR("Hema HK <hemahk@ti.com>");
MODULE_DESCRIPTION("TWL6030 USB transceiver driver");
MODULE_LICENSE("GPL");
