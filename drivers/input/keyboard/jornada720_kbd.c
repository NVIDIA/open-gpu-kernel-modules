// SPDX-License-Identifier: GPL-2.0-only
/*
 * drivers/input/keyboard/jornada720_kbd.c
 *
 * HP Jornada 720 keyboard platform driver
 *
 * Copyright (C) 2006/2007 Kristoffer Ericson <Kristoffer.Ericson@Gmail.com>
 *
 *    Copyright (C) 2006 jornada 720 kbd driver by
		Filip Zyzniewsk <Filip.Zyzniewski@tefnet.plX
 *     based on (C) 2004 jornada 720 kbd driver by
		Alex Lange <chicken@handhelds.org>
 */
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/interrupt.h>
#include <linux/input.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/slab.h>

#include <mach/jornada720.h>

MODULE_AUTHOR("Kristoffer Ericson <Kristoffer.Ericson@gmail.com>");
MODULE_DESCRIPTION("HP Jornada 710/720/728 keyboard driver");
MODULE_LICENSE("GPL v2");

static unsigned short jornada_std_keymap[128] = {					/* ROW */
	0, KEY_ESC, KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7,		/* #1  */
	KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_VOLUMEUP, KEY_VOLUMEDOWN, KEY_MUTE,	/*  -> */
	0, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9,		/* #2  */
	KEY_0, KEY_MINUS, KEY_EQUAL,0, 0, 0,						/*  -> */
	0, KEY_Q, KEY_W, KEY_E, KEY_R, KEY_T, KEY_Y, KEY_U, KEY_I, KEY_O,		/* #3  */
	KEY_P, KEY_BACKSLASH, KEY_BACKSPACE, 0, 0, 0,					/*  -> */
	0, KEY_A, KEY_S, KEY_D, KEY_F, KEY_G, KEY_H, KEY_J, KEY_K, KEY_L,		/* #4  */
	KEY_SEMICOLON, KEY_LEFTBRACE, KEY_RIGHTBRACE, 0, 0, 0,				/*  -> */
	0, KEY_Z, KEY_X, KEY_C, KEY_V, KEY_B, KEY_N, KEY_M, KEY_COMMA,			/* #5  */
	KEY_DOT, KEY_KPMINUS, KEY_APOSTROPHE, KEY_ENTER, 0, 0,0,			/*  -> */
	0, KEY_TAB, 0, KEY_LEFTSHIFT, 0, KEY_APOSTROPHE, 0, 0, 0, 0,			/* #6  */
	KEY_UP, 0, KEY_RIGHTSHIFT, 0, 0, 0,0, 0, 0, 0, 0, KEY_LEFTALT, KEY_GRAVE,	/*  -> */
	0, 0, KEY_LEFT, KEY_DOWN, KEY_RIGHT, 0, 0, 0, 0,0, KEY_KPASTERISK,		/*  -> */
	KEY_LEFTCTRL, 0, KEY_SPACE, 0, 0, 0, KEY_SLASH, KEY_DELETE, 0, 0,		/*  -> */
	0, 0, 0, KEY_POWER,								/*  -> */
};

struct jornadakbd {
	unsigned short keymap[ARRAY_SIZE(jornada_std_keymap)];
	struct input_dev *input;
};

static irqreturn_t jornada720_kbd_interrupt(int irq, void *dev_id)
{
	struct platform_device *pdev = dev_id;
	struct jornadakbd *jornadakbd = platform_get_drvdata(pdev);
	struct input_dev *input = jornadakbd->input;
	u8 count, kbd_data, scan_code;

	/* startup ssp with spinlock */
	jornada_ssp_start();

	if (jornada_ssp_inout(GETSCANKEYCODE) != TXDUMMY) {
		dev_dbg(&pdev->dev,
			"GetKeycode command failed with ETIMEDOUT, flushed bus\n");
	} else {
		/* How many keycodes are waiting for us? */
		count = jornada_ssp_byte(TXDUMMY);

		/* Lets drag them out one at a time */
		while (count--) {
			/* Exchange TxDummy for location (keymap[kbddata]) */
			kbd_data = jornada_ssp_byte(TXDUMMY);
			scan_code = kbd_data & 0x7f;

			input_event(input, EV_MSC, MSC_SCAN, scan_code);
			input_report_key(input, jornadakbd->keymap[scan_code],
					 !(kbd_data & 0x80));
			input_sync(input);
		}
	}

	/* release spinlock and turn off ssp */
	jornada_ssp_end();

	return IRQ_HANDLED;
};

static int jornada720_kbd_probe(struct platform_device *pdev)
{
	struct jornadakbd *jornadakbd;
	struct input_dev *input_dev;
	int i, err, irq;

	irq = platform_get_irq(pdev, 0);
	if (irq <= 0)
		return irq < 0 ? irq : -EINVAL;

	jornadakbd = devm_kzalloc(&pdev->dev, sizeof(*jornadakbd), GFP_KERNEL);
	input_dev = devm_input_allocate_device(&pdev->dev);
	if (!jornadakbd || !input_dev)
		return -ENOMEM;

	platform_set_drvdata(pdev, jornadakbd);

	memcpy(jornadakbd->keymap, jornada_std_keymap,
		sizeof(jornada_std_keymap));
	jornadakbd->input = input_dev;

	input_dev->evbit[0] = BIT(EV_KEY) | BIT(EV_REP);
	input_dev->name = "HP Jornada 720 keyboard";
	input_dev->phys = "jornadakbd/input0";
	input_dev->keycode = jornadakbd->keymap;
	input_dev->keycodesize = sizeof(unsigned short);
	input_dev->keycodemax = ARRAY_SIZE(jornada_std_keymap);
	input_dev->id.bustype = BUS_HOST;
	input_dev->dev.parent = &pdev->dev;

	for (i = 0; i < ARRAY_SIZE(jornadakbd->keymap); i++)
		__set_bit(jornadakbd->keymap[i], input_dev->keybit);
	__clear_bit(KEY_RESERVED, input_dev->keybit);

	input_set_capability(input_dev, EV_MSC, MSC_SCAN);

	err = devm_request_irq(&pdev->dev, irq, jornada720_kbd_interrupt,
			       IRQF_TRIGGER_FALLING, "jornadakbd", pdev);
	if (err) {
		dev_err(&pdev->dev, "unable to grab IRQ%d: %d\n", irq, err);
		return err;
	}

	return input_register_device(jornadakbd->input);
};

/* work with hotplug and coldplug */
MODULE_ALIAS("platform:jornada720_kbd");

static struct platform_driver jornada720_kbd_driver = {
	.driver  = {
		.name    = "jornada720_kbd",
	 },
	.probe   = jornada720_kbd_probe,
};
module_platform_driver(jornada720_kbd_driver);
