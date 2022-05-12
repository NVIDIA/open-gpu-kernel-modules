/*
 * Linux V4L2 radio driver for the Griffin radioSHARK USB radio receiver
 *
 * Note the radioSHARK offers the audio through a regular USB audio device,
 * this driver only handles the tuning.
 *
 * The info necessary to drive the shark was taken from the small userspace
 * shark.c program by Michael Rolig, which he kindly placed in the Public
 * Domain.
 *
 * Copyright (c) 2012 Hans de Goede <hdegoede@redhat.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
*/

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/leds.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/usb.h>
#include <linux/workqueue.h>
#include <media/v4l2-device.h>
#include <media/drv-intf/tea575x.h>

#if defined(CONFIG_LEDS_CLASS) || \
    (defined(CONFIG_LEDS_CLASS_MODULE) && defined(CONFIG_RADIO_SHARK_MODULE))
#define SHARK_USE_LEDS 1
#endif

/*
 * Version Information
 */
MODULE_AUTHOR("Hans de Goede <hdegoede@redhat.com>");
MODULE_DESCRIPTION("Griffin radioSHARK, USB radio receiver driver");
MODULE_LICENSE("GPL");

#define SHARK_IN_EP		0x83
#define SHARK_OUT_EP		0x05

#define TEA575X_BIT_MONO	(1<<22)		/* 0 = stereo, 1 = mono */
#define TEA575X_BIT_BAND_MASK	(3<<20)
#define TEA575X_BIT_BAND_FM	(0<<20)

#define TB_LEN 6
#define DRV_NAME "radioshark"

#define v4l2_dev_to_shark(d) container_of(d, struct shark_device, v4l2_dev)

/* Note BLUE_IS_PULSE comes after NO_LEDS as it is a status bit, not a LED */
enum { BLUE_LED, BLUE_PULSE_LED, RED_LED, NO_LEDS, BLUE_IS_PULSE };

struct shark_device {
	struct usb_device *usbdev;
	struct v4l2_device v4l2_dev;
	struct snd_tea575x tea;

#ifdef SHARK_USE_LEDS
	struct work_struct led_work;
	struct led_classdev leds[NO_LEDS];
	char led_names[NO_LEDS][32];
	atomic_t brightness[NO_LEDS];
	unsigned long brightness_new;
#endif

	u8 *transfer_buffer;
	u32 last_val;
};

static atomic_t shark_instance = ATOMIC_INIT(0);

static void shark_write_val(struct snd_tea575x *tea, u32 val)
{
	struct shark_device *shark = tea->private_data;
	int i, res, actual_len;

	/* Avoid unnecessary (slow) USB transfers */
	if (shark->last_val == val)
		return;

	memset(shark->transfer_buffer, 0, TB_LEN);
	shark->transfer_buffer[0] = 0xc0; /* Write shift register command */
	for (i = 0; i < 4; i++)
		shark->transfer_buffer[i] |= (val >> (24 - i * 8)) & 0xff;

	res = usb_interrupt_msg(shark->usbdev,
				usb_sndintpipe(shark->usbdev, SHARK_OUT_EP),
				shark->transfer_buffer, TB_LEN,
				&actual_len, 1000);
	if (res >= 0)
		shark->last_val = val;
	else
		v4l2_err(&shark->v4l2_dev, "set-freq error: %d\n", res);
}

static u32 shark_read_val(struct snd_tea575x *tea)
{
	struct shark_device *shark = tea->private_data;
	int i, res, actual_len;
	u32 val = 0;

	memset(shark->transfer_buffer, 0, TB_LEN);
	shark->transfer_buffer[0] = 0x80;
	res = usb_interrupt_msg(shark->usbdev,
				usb_sndintpipe(shark->usbdev, SHARK_OUT_EP),
				shark->transfer_buffer, TB_LEN,
				&actual_len, 1000);
	if (res < 0) {
		v4l2_err(&shark->v4l2_dev, "request-status error: %d\n", res);
		return shark->last_val;
	}

	res = usb_interrupt_msg(shark->usbdev,
				usb_rcvintpipe(shark->usbdev, SHARK_IN_EP),
				shark->transfer_buffer, TB_LEN,
				&actual_len, 1000);
	if (res < 0) {
		v4l2_err(&shark->v4l2_dev, "get-status error: %d\n", res);
		return shark->last_val;
	}

	for (i = 0; i < 4; i++)
		val |= shark->transfer_buffer[i] << (24 - i * 8);

	shark->last_val = val;

	/*
	 * The shark does not allow actually reading the stereo / mono pin :(
	 * So assume that when we're tuned to an FM station and mono has not
	 * been requested, that we're receiving stereo.
	 */
	if (((val & TEA575X_BIT_BAND_MASK) == TEA575X_BIT_BAND_FM) &&
	    !(val & TEA575X_BIT_MONO))
		shark->tea.stereo = true;
	else
		shark->tea.stereo = false;

	return val;
}

static const struct snd_tea575x_ops shark_tea_ops = {
	.write_val = shark_write_val,
	.read_val  = shark_read_val,
};

#ifdef SHARK_USE_LEDS
static void shark_led_work(struct work_struct *work)
{
	struct shark_device *shark =
		container_of(work, struct shark_device, led_work);
	int i, res, brightness, actual_len;

	for (i = 0; i < 3; i++) {
		if (!test_and_clear_bit(i, &shark->brightness_new))
			continue;

		brightness = atomic_read(&shark->brightness[i]);
		memset(shark->transfer_buffer, 0, TB_LEN);
		if (i != RED_LED) {
			shark->transfer_buffer[0] = 0xA0 + i;
			shark->transfer_buffer[1] = brightness;
		} else
			shark->transfer_buffer[0] = brightness ? 0xA9 : 0xA8;
		res = usb_interrupt_msg(shark->usbdev,
					usb_sndintpipe(shark->usbdev, 0x05),
					shark->transfer_buffer, TB_LEN,
					&actual_len, 1000);
		if (res < 0)
			v4l2_err(&shark->v4l2_dev, "set LED %s error: %d\n",
				 shark->led_names[i], res);
	}
}

static void shark_led_set_blue(struct led_classdev *led_cdev,
			       enum led_brightness value)
{
	struct shark_device *shark =
		container_of(led_cdev, struct shark_device, leds[BLUE_LED]);

	atomic_set(&shark->brightness[BLUE_LED], value);
	set_bit(BLUE_LED, &shark->brightness_new);
	clear_bit(BLUE_IS_PULSE, &shark->brightness_new);
	schedule_work(&shark->led_work);
}

static void shark_led_set_blue_pulse(struct led_classdev *led_cdev,
				     enum led_brightness value)
{
	struct shark_device *shark = container_of(led_cdev,
				struct shark_device, leds[BLUE_PULSE_LED]);

	atomic_set(&shark->brightness[BLUE_PULSE_LED], 256 - value);
	set_bit(BLUE_PULSE_LED, &shark->brightness_new);
	set_bit(BLUE_IS_PULSE, &shark->brightness_new);
	schedule_work(&shark->led_work);
}

static void shark_led_set_red(struct led_classdev *led_cdev,
			      enum led_brightness value)
{
	struct shark_device *shark =
		container_of(led_cdev, struct shark_device, leds[RED_LED]);

	atomic_set(&shark->brightness[RED_LED], value);
	set_bit(RED_LED, &shark->brightness_new);
	schedule_work(&shark->led_work);
}

static const struct led_classdev shark_led_templates[NO_LEDS] = {
	[BLUE_LED] = {
		.name		= "%s:blue:",
		.brightness	= LED_OFF,
		.max_brightness = 127,
		.brightness_set = shark_led_set_blue,
	},
	[BLUE_PULSE_LED] = {
		.name		= "%s:blue-pulse:",
		.brightness	= LED_OFF,
		.max_brightness = 255,
		.brightness_set = shark_led_set_blue_pulse,
	},
	[RED_LED] = {
		.name		= "%s:red:",
		.brightness	= LED_OFF,
		.max_brightness = 1,
		.brightness_set = shark_led_set_red,
	},
};

static int shark_register_leds(struct shark_device *shark, struct device *dev)
{
	int i, retval;

	atomic_set(&shark->brightness[BLUE_LED], 127);
	INIT_WORK(&shark->led_work, shark_led_work);
	for (i = 0; i < NO_LEDS; i++) {
		shark->leds[i] = shark_led_templates[i];
		snprintf(shark->led_names[i], sizeof(shark->led_names[0]),
			 shark->leds[i].name, shark->v4l2_dev.name);
		shark->leds[i].name = shark->led_names[i];
		retval = led_classdev_register(dev, &shark->leds[i]);
		if (retval) {
			v4l2_err(&shark->v4l2_dev,
				 "couldn't register led: %s\n",
				 shark->led_names[i]);
			return retval;
		}
	}
	return 0;
}

static void shark_unregister_leds(struct shark_device *shark)
{
	int i;

	for (i = 0; i < NO_LEDS; i++)
		led_classdev_unregister(&shark->leds[i]);

	cancel_work_sync(&shark->led_work);
}

static inline void shark_resume_leds(struct shark_device *shark)
{
	if (test_bit(BLUE_IS_PULSE, &shark->brightness_new))
		set_bit(BLUE_PULSE_LED, &shark->brightness_new);
	else
		set_bit(BLUE_LED, &shark->brightness_new);
	set_bit(RED_LED, &shark->brightness_new);
	schedule_work(&shark->led_work);
}
#else
static int shark_register_leds(struct shark_device *shark, struct device *dev)
{
	v4l2_warn(&shark->v4l2_dev,
		  "CONFIG_LEDS_CLASS not enabled, LED support disabled\n");
	return 0;
}
static inline void shark_unregister_leds(struct shark_device *shark) { }
static inline void shark_resume_leds(struct shark_device *shark) { }
#endif

static void usb_shark_disconnect(struct usb_interface *intf)
{
	struct v4l2_device *v4l2_dev = usb_get_intfdata(intf);
	struct shark_device *shark = v4l2_dev_to_shark(v4l2_dev);

	mutex_lock(&shark->tea.mutex);
	v4l2_device_disconnect(&shark->v4l2_dev);
	snd_tea575x_exit(&shark->tea);
	mutex_unlock(&shark->tea.mutex);

	shark_unregister_leds(shark);

	v4l2_device_put(&shark->v4l2_dev);
}

static void usb_shark_release(struct v4l2_device *v4l2_dev)
{
	struct shark_device *shark = v4l2_dev_to_shark(v4l2_dev);

	v4l2_device_unregister(&shark->v4l2_dev);
	kfree(shark->transfer_buffer);
	kfree(shark);
}

static int usb_shark_probe(struct usb_interface *intf,
			   const struct usb_device_id *id)
{
	struct shark_device *shark;
	int retval = -ENOMEM;

	shark = kzalloc(sizeof(struct shark_device), GFP_KERNEL);
	if (!shark)
		return retval;

	shark->transfer_buffer = kmalloc(TB_LEN, GFP_KERNEL);
	if (!shark->transfer_buffer)
		goto err_alloc_buffer;

	v4l2_device_set_name(&shark->v4l2_dev, DRV_NAME, &shark_instance);

	retval = shark_register_leds(shark, &intf->dev);
	if (retval)
		goto err_reg_leds;

	shark->v4l2_dev.release = usb_shark_release;
	retval = v4l2_device_register(&intf->dev, &shark->v4l2_dev);
	if (retval) {
		v4l2_err(&shark->v4l2_dev, "couldn't register v4l2_device\n");
		goto err_reg_dev;
	}

	shark->usbdev = interface_to_usbdev(intf);
	shark->tea.v4l2_dev = &shark->v4l2_dev;
	shark->tea.private_data = shark;
	shark->tea.radio_nr = -1;
	shark->tea.ops = &shark_tea_ops;
	shark->tea.cannot_mute = true;
	shark->tea.has_am = true;
	strscpy(shark->tea.card, "Griffin radioSHARK",
		sizeof(shark->tea.card));
	usb_make_path(shark->usbdev, shark->tea.bus_info,
		sizeof(shark->tea.bus_info));

	retval = snd_tea575x_init(&shark->tea, THIS_MODULE);
	if (retval) {
		v4l2_err(&shark->v4l2_dev, "couldn't init tea5757\n");
		goto err_init_tea;
	}

	return 0;

err_init_tea:
	v4l2_device_unregister(&shark->v4l2_dev);
err_reg_dev:
	shark_unregister_leds(shark);
err_reg_leds:
	kfree(shark->transfer_buffer);
err_alloc_buffer:
	kfree(shark);

	return retval;
}

#ifdef CONFIG_PM
static int usb_shark_suspend(struct usb_interface *intf, pm_message_t message)
{
	return 0;
}

static int usb_shark_resume(struct usb_interface *intf)
{
	struct v4l2_device *v4l2_dev = usb_get_intfdata(intf);
	struct shark_device *shark = v4l2_dev_to_shark(v4l2_dev);

	mutex_lock(&shark->tea.mutex);
	snd_tea575x_set_freq(&shark->tea);
	mutex_unlock(&shark->tea.mutex);

	shark_resume_leds(shark);

	return 0;
}
#endif

/* Specify the bcdDevice value, as the radioSHARK and radioSHARK2 share ids */
static const struct usb_device_id usb_shark_device_table[] = {
	{ .match_flags = USB_DEVICE_ID_MATCH_DEVICE_AND_VERSION |
			 USB_DEVICE_ID_MATCH_INT_CLASS,
	  .idVendor     = 0x077d,
	  .idProduct    = 0x627a,
	  .bcdDevice_lo = 0x0001,
	  .bcdDevice_hi = 0x0001,
	  .bInterfaceClass = 3,
	},
	{ }
};
MODULE_DEVICE_TABLE(usb, usb_shark_device_table);

static struct usb_driver usb_shark_driver = {
	.name			= DRV_NAME,
	.probe			= usb_shark_probe,
	.disconnect		= usb_shark_disconnect,
	.id_table		= usb_shark_device_table,
#ifdef CONFIG_PM
	.suspend		= usb_shark_suspend,
	.resume			= usb_shark_resume,
	.reset_resume		= usb_shark_resume,
#endif
};
module_usb_driver(usb_shark_driver);
