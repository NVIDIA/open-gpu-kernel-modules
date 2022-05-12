// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  Copyright (c) 2000-2002 Vojtech Pavlik <vojtech@ucw.cz>
 *  Copyright (c) 2001-2002, 2007 Johann Deneux <johann.deneux@gmail.com>
 *
 *  USB/RS232 I-Force joysticks and wheels.
 */

#include <asm/unaligned.h>
#include "iforce.h"

MODULE_AUTHOR("Vojtech Pavlik <vojtech@ucw.cz>, Johann Deneux <johann.deneux@gmail.com>");
MODULE_DESCRIPTION("Core I-Force joysticks and wheels driver");
MODULE_LICENSE("GPL");

static signed short btn_joystick[] =
{ BTN_TRIGGER, BTN_TOP, BTN_THUMB, BTN_TOP2, BTN_BASE,
  BTN_BASE2, BTN_BASE3, BTN_BASE4, BTN_BASE5, BTN_A,
  BTN_B, BTN_C, BTN_DEAD, -1 };

static signed short btn_joystick_avb[] =
{ BTN_TRIGGER, BTN_THUMB, BTN_TOP, BTN_TOP2, BTN_BASE,
  BTN_BASE2, BTN_BASE3, BTN_BASE4, BTN_DEAD, -1 };

static signed short btn_wheel[] =
{ BTN_GEAR_DOWN, BTN_GEAR_UP, BTN_BASE, BTN_BASE2, BTN_BASE3,
  BTN_BASE4, BTN_BASE5, BTN_BASE6, -1 };

static signed short abs_joystick[] =
{ ABS_X, ABS_Y, ABS_THROTTLE, ABS_HAT0X, ABS_HAT0Y, -1 };

static signed short abs_joystick_rudder[] =
{ ABS_X, ABS_Y, ABS_THROTTLE, ABS_RUDDER, ABS_HAT0X, ABS_HAT0Y, -1 };

static signed short abs_avb_pegasus[] =
{ ABS_X, ABS_Y, ABS_THROTTLE, ABS_RUDDER, ABS_HAT0X, ABS_HAT0Y,
  ABS_HAT1X, ABS_HAT1Y, -1 };

static signed short abs_wheel[] =
{ ABS_WHEEL, ABS_GAS, ABS_BRAKE, ABS_HAT0X, ABS_HAT0Y, -1 };

static signed short ff_iforce[] =
{ FF_PERIODIC, FF_CONSTANT, FF_SPRING, FF_DAMPER,
  FF_SQUARE, FF_TRIANGLE, FF_SINE, FF_SAW_UP, FF_SAW_DOWN, FF_GAIN,
  FF_AUTOCENTER, -1 };

static struct iforce_device iforce_device[] = {
	{ 0x044f, 0xa01c, "Thrustmaster Motor Sport GT",		btn_wheel, abs_wheel, ff_iforce },
	{ 0x046d, 0xc281, "Logitech WingMan Force",			btn_joystick, abs_joystick, ff_iforce },
	{ 0x046d, 0xc291, "Logitech WingMan Formula Force",		btn_wheel, abs_wheel, ff_iforce },
	{ 0x05ef, 0x020a, "AVB Top Shot Pegasus",			btn_joystick_avb, abs_avb_pegasus, ff_iforce },
	{ 0x05ef, 0x8884, "AVB Mag Turbo Force",			btn_wheel, abs_wheel, ff_iforce },
	{ 0x05ef, 0x8888, "AVB Top Shot Force Feedback Racing Wheel",	btn_wheel, abs_wheel, ff_iforce }, //?
	{ 0x061c, 0xc0a4, "ACT LABS Force RS",                          btn_wheel, abs_wheel, ff_iforce }, //?
	{ 0x061c, 0xc084, "ACT LABS Force RS",				btn_wheel, abs_wheel, ff_iforce },
	{ 0x06a3, 0xff04, "Saitek R440 Force Wheel",			btn_wheel, abs_wheel, ff_iforce }, //?
	{ 0x06f8, 0x0001, "Guillemot Race Leader Force Feedback",	btn_wheel, abs_wheel, ff_iforce }, //?
	{ 0x06f8, 0x0001, "Guillemot Jet Leader Force Feedback",	btn_joystick, abs_joystick_rudder, ff_iforce },
	{ 0x06f8, 0x0004, "Guillemot Force Feedback Racing Wheel",	btn_wheel, abs_wheel, ff_iforce }, //?
	{ 0x06f8, 0xa302, "Guillemot Jet Leader 3D",			btn_joystick, abs_joystick, ff_iforce }, //?
	{ 0x06d6, 0x29bc, "Trust Force Feedback Race Master",		btn_wheel, abs_wheel, ff_iforce },
	{ 0x0000, 0x0000, "Unknown I-Force Device [%04x:%04x]",		btn_joystick, abs_joystick, ff_iforce }
};

static int iforce_playback(struct input_dev *dev, int effect_id, int value)
{
	struct iforce *iforce = input_get_drvdata(dev);
	struct iforce_core_effect *core_effect = &iforce->core_effects[effect_id];

	if (value > 0)
		set_bit(FF_CORE_SHOULD_PLAY, core_effect->flags);
	else
		clear_bit(FF_CORE_SHOULD_PLAY, core_effect->flags);

	iforce_control_playback(iforce, effect_id, value);
	return 0;
}

static void iforce_set_gain(struct input_dev *dev, u16 gain)
{
	struct iforce *iforce = input_get_drvdata(dev);
	unsigned char data[3];

	data[0] = gain >> 9;
	iforce_send_packet(iforce, FF_CMD_GAIN, data);
}

static void iforce_set_autocenter(struct input_dev *dev, u16 magnitude)
{
	struct iforce *iforce = input_get_drvdata(dev);
	unsigned char data[3];

	data[0] = 0x03;
	data[1] = magnitude >> 9;
	iforce_send_packet(iforce, FF_CMD_AUTOCENTER, data);

	data[0] = 0x04;
	data[1] = 0x01;
	iforce_send_packet(iforce, FF_CMD_AUTOCENTER, data);
}

/*
 * Function called when an ioctl is performed on the event dev entry.
 * It uploads an effect to the device
 */
static int iforce_upload_effect(struct input_dev *dev, struct ff_effect *effect, struct ff_effect *old)
{
	struct iforce *iforce = input_get_drvdata(dev);
	struct iforce_core_effect *core_effect = &iforce->core_effects[effect->id];
	int ret;

	if (__test_and_set_bit(FF_CORE_IS_USED, core_effect->flags)) {
		/* Check the effect is not already being updated */
		if (test_bit(FF_CORE_UPDATE, core_effect->flags))
			return -EAGAIN;
	}

/*
 * Upload the effect
 */
	switch (effect->type) {
	case FF_PERIODIC:
		ret = iforce_upload_periodic(iforce, effect, old);
		break;

	case FF_CONSTANT:
		ret = iforce_upload_constant(iforce, effect, old);
		break;

	case FF_SPRING:
	case FF_DAMPER:
		ret = iforce_upload_condition(iforce, effect, old);
		break;

	default:
		return -EINVAL;
	}

	if (ret == 0) {
		/* A packet was sent, forbid new updates until we are notified
		 * that the packet was updated
		 */
		set_bit(FF_CORE_UPDATE, core_effect->flags);
	}
	return ret;
}

/*
 * Erases an effect: it frees the effect id and mark as unused the memory
 * allocated for the parameters
 */
static int iforce_erase_effect(struct input_dev *dev, int effect_id)
{
	struct iforce *iforce = input_get_drvdata(dev);
	struct iforce_core_effect *core_effect = &iforce->core_effects[effect_id];
	int err = 0;

	if (test_bit(FF_MOD1_IS_USED, core_effect->flags))
		err = release_resource(&core_effect->mod1_chunk);

	if (!err && test_bit(FF_MOD2_IS_USED, core_effect->flags))
		err = release_resource(&core_effect->mod2_chunk);

	/* TODO: remember to change that if more FF_MOD* bits are added */
	core_effect->flags[0] = 0;

	return err;
}

static int iforce_open(struct input_dev *dev)
{
	struct iforce *iforce = input_get_drvdata(dev);

	iforce->xport_ops->start_io(iforce);

	if (test_bit(EV_FF, dev->evbit)) {
		/* Enable force feedback */
		iforce_send_packet(iforce, FF_CMD_ENABLE, "\004");
	}

	return 0;
}

static void iforce_close(struct input_dev *dev)
{
	struct iforce *iforce = input_get_drvdata(dev);
	int i;

	if (test_bit(EV_FF, dev->evbit)) {
		/* Check: no effects should be present in memory */
		for (i = 0; i < dev->ff->max_effects; i++) {
			if (test_bit(FF_CORE_IS_USED, iforce->core_effects[i].flags)) {
				dev_warn(&dev->dev,
					"%s: Device still owns effects\n",
					__func__);
				break;
			}
		}

		/* Disable force feedback playback */
		iforce_send_packet(iforce, FF_CMD_ENABLE, "\001");
		/* Wait for the command to complete */
		wait_event_interruptible(iforce->wait,
			!test_bit(IFORCE_XMIT_RUNNING, iforce->xmit_flags));
	}

	iforce->xport_ops->stop_io(iforce);
}

int iforce_init_device(struct device *parent, u16 bustype,
		       struct iforce *iforce)
{
	struct input_dev *input_dev;
	struct ff_device *ff;
	u8 c[] = "CEOV";
	u8 buf[IFORCE_MAX_LENGTH];
	size_t len;
	int i, error;
	int ff_effects = 0;

	input_dev = input_allocate_device();
	if (!input_dev)
		return -ENOMEM;

	init_waitqueue_head(&iforce->wait);
	spin_lock_init(&iforce->xmit_lock);
	mutex_init(&iforce->mem_mutex);
	iforce->xmit.buf = iforce->xmit_data;
	iforce->dev = input_dev;

/*
 * Input device fields.
 */

	input_dev->id.bustype = bustype;
	input_dev->dev.parent = parent;

	input_set_drvdata(input_dev, iforce);

	input_dev->name = "Unknown I-Force device";
	input_dev->open = iforce_open;
	input_dev->close = iforce_close;

/*
 * On-device memory allocation.
 */

	iforce->device_memory.name = "I-Force device effect memory";
	iforce->device_memory.start = 0;
	iforce->device_memory.end = 200;
	iforce->device_memory.flags = IORESOURCE_MEM;
	iforce->device_memory.parent = NULL;
	iforce->device_memory.child = NULL;
	iforce->device_memory.sibling = NULL;

/*
 * Wait until device ready - until it sends its first response.
 */

	for (i = 0; i < 20; i++)
		if (!iforce_get_id_packet(iforce, 'O', buf, &len))
			break;

	if (i == 20) { /* 5 seconds */
		dev_err(&input_dev->dev,
			"Timeout waiting for response from device.\n");
		error = -ENODEV;
		goto fail;
	}

/*
 * Get device info.
 */

	if (!iforce_get_id_packet(iforce, 'M', buf, &len) || len < 3)
		input_dev->id.vendor = get_unaligned_le16(buf + 1);
	else
		dev_warn(&iforce->dev->dev, "Device does not respond to id packet M\n");

	if (!iforce_get_id_packet(iforce, 'P', buf, &len) || len < 3)
		input_dev->id.product = get_unaligned_le16(buf + 1);
	else
		dev_warn(&iforce->dev->dev, "Device does not respond to id packet P\n");

	if (!iforce_get_id_packet(iforce, 'B', buf, &len) || len < 3)
		iforce->device_memory.end = get_unaligned_le16(buf + 1);
	else
		dev_warn(&iforce->dev->dev, "Device does not respond to id packet B\n");

	if (!iforce_get_id_packet(iforce, 'N', buf, &len) || len < 2)
		ff_effects = buf[1];
	else
		dev_warn(&iforce->dev->dev, "Device does not respond to id packet N\n");

	/* Check if the device can store more effects than the driver can really handle */
	if (ff_effects > IFORCE_EFFECTS_MAX) {
		dev_warn(&iforce->dev->dev, "Limiting number of effects to %d (device reports %d)\n",
		       IFORCE_EFFECTS_MAX, ff_effects);
		ff_effects = IFORCE_EFFECTS_MAX;
	}

/*
 * Display additional info.
 */

	for (i = 0; c[i]; i++)
		if (!iforce_get_id_packet(iforce, c[i], buf, &len))
			iforce_dump_packet(iforce, "info",
					   (FF_CMD_QUERY & 0xff00) | len, buf);

/*
 * Disable spring, enable force feedback.
 */
	iforce_set_autocenter(input_dev, 0);

/*
 * Find appropriate device entry
 */

	for (i = 0; iforce_device[i].idvendor; i++)
		if (iforce_device[i].idvendor == input_dev->id.vendor &&
		    iforce_device[i].idproduct == input_dev->id.product)
			break;

	iforce->type = iforce_device + i;
	input_dev->name = iforce->type->name;

/*
 * Set input device bitfields and ranges.
 */

	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS) |
		BIT_MASK(EV_FF_STATUS);

	for (i = 0; iforce->type->btn[i] >= 0; i++)
		set_bit(iforce->type->btn[i], input_dev->keybit);

	for (i = 0; iforce->type->abs[i] >= 0; i++) {

		signed short t = iforce->type->abs[i];

		switch (t) {
		case ABS_X:
		case ABS_Y:
		case ABS_WHEEL:
			input_set_abs_params(input_dev, t, -1920, 1920, 16, 128);
			set_bit(t, input_dev->ffbit);
			break;

		case ABS_THROTTLE:
		case ABS_GAS:
		case ABS_BRAKE:
			input_set_abs_params(input_dev, t, 0, 255, 0, 0);
			break;

		case ABS_RUDDER:
			input_set_abs_params(input_dev, t, -128, 127, 0, 0);
			break;

		case ABS_HAT0X:
		case ABS_HAT0Y:
		case ABS_HAT1X:
		case ABS_HAT1Y:
			input_set_abs_params(input_dev, t, -1, 1, 0, 0);
			break;
		}
	}

	if (ff_effects) {

		for (i = 0; iforce->type->ff[i] >= 0; i++)
			set_bit(iforce->type->ff[i], input_dev->ffbit);

		error = input_ff_create(input_dev, ff_effects);
		if (error)
			goto fail;

		ff = input_dev->ff;
		ff->upload = iforce_upload_effect;
		ff->erase = iforce_erase_effect;
		ff->set_gain = iforce_set_gain;
		ff->set_autocenter = iforce_set_autocenter;
		ff->playback = iforce_playback;
	}
/*
 * Register input device.
 */

	error = input_register_device(iforce->dev);
	if (error)
		goto fail;

	return 0;

 fail:	input_free_device(input_dev);
	return error;
}
EXPORT_SYMBOL(iforce_init_device);
