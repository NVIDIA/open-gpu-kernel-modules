// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Linux driver for TerraTec DMX 6Fire USB
 *
 * Device communications
 *
 * Author:	Torsten Schenk <torsten.schenk@zoho.com>
 * Created:	Jan 01, 2011
 * Copyright:	(C) Torsten Schenk
 */

#include "comm.h"
#include "chip.h"
#include "midi.h"

enum {
	COMM_EP = 1,
	COMM_FPGA_EP = 2
};

static void usb6fire_comm_init_urb(struct comm_runtime *rt, struct urb *urb,
		u8 *buffer, void *context, void(*handler)(struct urb *urb))
{
	usb_init_urb(urb);
	urb->transfer_buffer = buffer;
	urb->pipe = usb_sndintpipe(rt->chip->dev, COMM_EP);
	urb->complete = handler;
	urb->context = context;
	urb->interval = 1;
	urb->dev = rt->chip->dev;
}

static void usb6fire_comm_receiver_handler(struct urb *urb)
{
	struct comm_runtime *rt = urb->context;
	struct midi_runtime *midi_rt = rt->chip->midi;

	if (!urb->status) {
		if (rt->receiver_buffer[0] == 0x10) /* midi in event */
			if (midi_rt)
				midi_rt->in_received(midi_rt,
						rt->receiver_buffer + 2,
						rt->receiver_buffer[1]);
	}

	if (!rt->chip->shutdown) {
		urb->status = 0;
		urb->actual_length = 0;
		if (usb_submit_urb(urb, GFP_ATOMIC) < 0)
			dev_warn(&urb->dev->dev,
					"comm data receiver aborted.\n");
	}
}

static void usb6fire_comm_init_buffer(u8 *buffer, u8 id, u8 request,
		u8 reg, u8 vl, u8 vh)
{
	buffer[0] = 0x01;
	buffer[2] = request;
	buffer[3] = id;
	switch (request) {
	case 0x02:
		buffer[1] = 0x05; /* length (starting at buffer[2]) */
		buffer[4] = reg;
		buffer[5] = vl;
		buffer[6] = vh;
		break;

	case 0x12:
		buffer[1] = 0x0b; /* length (starting at buffer[2]) */
		buffer[4] = 0x00;
		buffer[5] = 0x18;
		buffer[6] = 0x05;
		buffer[7] = 0x00;
		buffer[8] = 0x01;
		buffer[9] = 0x00;
		buffer[10] = 0x9e;
		buffer[11] = reg;
		buffer[12] = vl;
		break;

	case 0x20:
	case 0x21:
	case 0x22:
		buffer[1] = 0x04;
		buffer[4] = reg;
		buffer[5] = vl;
		break;
	}
}

static int usb6fire_comm_send_buffer(u8 *buffer, struct usb_device *dev)
{
	int ret;
	int actual_len;

	ret = usb_interrupt_msg(dev, usb_sndintpipe(dev, COMM_EP),
			buffer, buffer[1] + 2, &actual_len, HZ);
	if (ret < 0)
		return ret;
	else if (actual_len != buffer[1] + 2)
		return -EIO;
	return 0;
}

static int usb6fire_comm_write8(struct comm_runtime *rt, u8 request,
		u8 reg, u8 value)
{
	u8 *buffer;
	int ret;

	/* 13: maximum length of message */
	buffer = kmalloc(13, GFP_KERNEL);
	if (!buffer)
		return -ENOMEM;

	usb6fire_comm_init_buffer(buffer, 0x00, request, reg, value, 0x00);
	ret = usb6fire_comm_send_buffer(buffer, rt->chip->dev);

	kfree(buffer);
	return ret;
}

static int usb6fire_comm_write16(struct comm_runtime *rt, u8 request,
		u8 reg, u8 vl, u8 vh)
{
	u8 *buffer;
	int ret;

	/* 13: maximum length of message */
	buffer = kmalloc(13, GFP_KERNEL);
	if (!buffer)
		return -ENOMEM;

	usb6fire_comm_init_buffer(buffer, 0x00, request, reg, vl, vh);
	ret = usb6fire_comm_send_buffer(buffer, rt->chip->dev);

	kfree(buffer);
	return ret;
}

int usb6fire_comm_init(struct sfire_chip *chip)
{
	struct comm_runtime *rt = kzalloc(sizeof(struct comm_runtime),
			GFP_KERNEL);
	struct urb *urb;
	int ret;

	if (!rt)
		return -ENOMEM;

	rt->receiver_buffer = kzalloc(COMM_RECEIVER_BUFSIZE, GFP_KERNEL);
	if (!rt->receiver_buffer) {
		kfree(rt);
		return -ENOMEM;
	}

	urb = &rt->receiver;
	rt->serial = 1;
	rt->chip = chip;
	usb_init_urb(urb);
	rt->init_urb = usb6fire_comm_init_urb;
	rt->write8 = usb6fire_comm_write8;
	rt->write16 = usb6fire_comm_write16;

	/* submit an urb that receives communication data from device */
	urb->transfer_buffer = rt->receiver_buffer;
	urb->transfer_buffer_length = COMM_RECEIVER_BUFSIZE;
	urb->pipe = usb_rcvintpipe(chip->dev, COMM_EP);
	urb->dev = chip->dev;
	urb->complete = usb6fire_comm_receiver_handler;
	urb->context = rt;
	urb->interval = 1;
	ret = usb_submit_urb(urb, GFP_KERNEL);
	if (ret < 0) {
		kfree(rt->receiver_buffer);
		kfree(rt);
		dev_err(&chip->dev->dev, "cannot create comm data receiver.");
		return ret;
	}
	chip->comm = rt;
	return 0;
}

void usb6fire_comm_abort(struct sfire_chip *chip)
{
	struct comm_runtime *rt = chip->comm;

	if (rt)
		usb_poison_urb(&rt->receiver);
}

void usb6fire_comm_destroy(struct sfire_chip *chip)
{
	struct comm_runtime *rt = chip->comm;

	kfree(rt->receiver_buffer);
	kfree(rt);
	chip->comm = NULL;
}
