// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Backlight Driver for the KB3886 Backlight
 *
 *  Copyright (c) 2007-2008 Claudio Nieder
 *
 *  Based on corgi_bl.c by Richard Purdie and kb3886 driver by Robert Woerle
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/mutex.h>
#include <linux/fb.h>
#include <linux/backlight.h>
#include <linux/delay.h>
#include <linux/dmi.h>

#define KB3886_PARENT 0x64
#define KB3886_IO 0x60
#define KB3886_ADC_DAC_PWM 0xC4
#define KB3886_PWM0_WRITE 0x81
#define KB3886_PWM0_READ 0x41

static DEFINE_MUTEX(bl_mutex);

static void kb3886_bl_set_intensity(int intensity)
{
	mutex_lock(&bl_mutex);
	intensity = intensity&0xff;
	outb(KB3886_ADC_DAC_PWM, KB3886_PARENT);
	usleep_range(10000, 11000);
	outb(KB3886_PWM0_WRITE, KB3886_IO);
	usleep_range(10000, 11000);
	outb(intensity, KB3886_IO);
	mutex_unlock(&bl_mutex);
}

struct kb3886bl_machinfo {
	int max_intensity;
	int default_intensity;
	int limit_mask;
	void (*set_bl_intensity)(int intensity);
};

static struct kb3886bl_machinfo kb3886_bl_machinfo = {
	.max_intensity = 0xff,
	.default_intensity = 0xa0,
	.limit_mask = 0x7f,
	.set_bl_intensity = kb3886_bl_set_intensity,
};

static struct platform_device kb3886bl_device = {
	.name		= "kb3886-bl",
	.dev		= {
		.platform_data	= &kb3886_bl_machinfo,
	},
	.id		= -1,
};

static struct platform_device *devices[] __initdata = {
	&kb3886bl_device,
};

/*
 * Back to driver
 */

static int kb3886bl_intensity;
static struct backlight_device *kb3886_backlight_device;
static struct kb3886bl_machinfo *bl_machinfo;

static unsigned long kb3886bl_flags;
#define KB3886BL_SUSPENDED     0x01

static const struct dmi_system_id kb3886bl_device_table[] __initconst = {
	{
		.ident = "Sahara Touch-iT",
		.matches = {
			DMI_MATCH(DMI_SYS_VENDOR, "SDV"),
			DMI_MATCH(DMI_PRODUCT_NAME, "iTouch T201"),
		},
	},
	{ }
};

static int kb3886bl_send_intensity(struct backlight_device *bd)
{
	int intensity = backlight_get_brightness(bd);

	if (kb3886bl_flags & KB3886BL_SUSPENDED)
		intensity = 0;

	bl_machinfo->set_bl_intensity(intensity);

	kb3886bl_intensity = intensity;
	return 0;
}

#ifdef CONFIG_PM_SLEEP
static int kb3886bl_suspend(struct device *dev)
{
	struct backlight_device *bd = dev_get_drvdata(dev);

	kb3886bl_flags |= KB3886BL_SUSPENDED;
	backlight_update_status(bd);
	return 0;
}

static int kb3886bl_resume(struct device *dev)
{
	struct backlight_device *bd = dev_get_drvdata(dev);

	kb3886bl_flags &= ~KB3886BL_SUSPENDED;
	backlight_update_status(bd);
	return 0;
}
#endif

static SIMPLE_DEV_PM_OPS(kb3886bl_pm_ops, kb3886bl_suspend, kb3886bl_resume);

static int kb3886bl_get_intensity(struct backlight_device *bd)
{
	return kb3886bl_intensity;
}

static const struct backlight_ops kb3886bl_ops = {
	.get_brightness = kb3886bl_get_intensity,
	.update_status  = kb3886bl_send_intensity,
};

static int kb3886bl_probe(struct platform_device *pdev)
{
	struct backlight_properties props;
	struct kb3886bl_machinfo *machinfo = dev_get_platdata(&pdev->dev);

	bl_machinfo = machinfo;
	if (!machinfo->limit_mask)
		machinfo->limit_mask = -1;

	memset(&props, 0, sizeof(struct backlight_properties));
	props.type = BACKLIGHT_RAW;
	props.max_brightness = machinfo->max_intensity;
	kb3886_backlight_device = devm_backlight_device_register(&pdev->dev,
							"kb3886-bl", &pdev->dev,
							NULL, &kb3886bl_ops,
							&props);
	if (IS_ERR(kb3886_backlight_device))
		return PTR_ERR(kb3886_backlight_device);

	platform_set_drvdata(pdev, kb3886_backlight_device);

	kb3886_backlight_device->props.power = FB_BLANK_UNBLANK;
	kb3886_backlight_device->props.brightness = machinfo->default_intensity;
	backlight_update_status(kb3886_backlight_device);

	return 0;
}

static struct platform_driver kb3886bl_driver = {
	.probe		= kb3886bl_probe,
	.driver		= {
		.name	= "kb3886-bl",
		.pm	= &kb3886bl_pm_ops,
	},
};

static int __init kb3886_init(void)
{
	if (!dmi_check_system(kb3886bl_device_table))
		return -ENODEV;

	platform_add_devices(devices, ARRAY_SIZE(devices));
	return platform_driver_register(&kb3886bl_driver);
}

static void __exit kb3886_exit(void)
{
	platform_driver_unregister(&kb3886bl_driver);
}

module_init(kb3886_init);
module_exit(kb3886_exit);

MODULE_AUTHOR("Claudio Nieder <private@claudio.ch>");
MODULE_DESCRIPTION("Tabletkiosk Sahara Touch-iT Backlight Driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("dmi:*:svnSDV:pniTouchT201:*");
