// SPDX-License-Identifier: GPL-2.0
/* 
 * Emagic EMI 2|6 usb audio interface firmware loader.
 * Copyright (C) 2002
 * 	Tapio Laxström (tapio.laxstrom@iptime.fi)
 */
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/usb.h>
#include <linux/delay.h>
#include <linux/firmware.h>
#include <linux/ihex.h>

/* include firmware (variables)*/

/* FIXME: This is quick and dirty solution! */
#define SPDIF	/* if you want SPDIF comment next line */
//#undef SPDIF	/* if you want MIDI uncomment this line */ 

#ifdef SPDIF
#define FIRMWARE_FW "emi62/spdif.fw"
#else
#define FIRMWARE_FW "emi62/midi.fw"
#endif

#define EMI62_VENDOR_ID 		0x086a  /* Emagic Soft-und Hardware GmBH */
#define EMI62_PRODUCT_ID		0x0110	/* EMI 6|2m without firmware */

#define ANCHOR_LOAD_INTERNAL	0xA0	/* Vendor specific request code for Anchor Upload/Download (This one is implemented in the core) */
#define ANCHOR_LOAD_EXTERNAL	0xA3	/* This command is not implemented in the core. Requires firmware */
#define ANCHOR_LOAD_FPGA	0xA5	/* This command is not implemented in the core. Requires firmware. Emagic extension */
#define MAX_INTERNAL_ADDRESS	0x1B3F	/* This is the highest internal RAM address for the AN2131Q */
#define CPUCS_REG		0x7F92  /* EZ-USB Control and Status Register.  Bit 0 controls 8051 reset */ 
#define INTERNAL_RAM(address)   (address <= MAX_INTERNAL_ADDRESS)

static int emi62_writememory(struct usb_device *dev, int address,
			     const unsigned char *data, int length,
			     __u8 bRequest);
static int emi62_set_reset(struct usb_device *dev, unsigned char reset_bit);
static int emi62_load_firmware (struct usb_device *dev);
static int emi62_probe(struct usb_interface *intf, const struct usb_device_id *id);
static void emi62_disconnect(struct usb_interface *intf);

/* thanks to drivers/usb/serial/keyspan_pda.c code */
static int emi62_writememory(struct usb_device *dev, int address,
			     const unsigned char *data, int length,
			     __u8 request)
{
	int result;
	unsigned char *buffer =  kmemdup(data, length, GFP_KERNEL);

	if (!buffer) {
		dev_err(&dev->dev, "kmalloc(%d) failed.\n", length);
		return -ENOMEM;
	}
	/* Note: usb_control_msg returns negative value on error or length of the
	 * 		 data that was written! */
	result = usb_control_msg (dev, usb_sndctrlpipe(dev, 0), request, 0x40, address, 0, buffer, length, 300);
	kfree (buffer);
	return result;
}

/* thanks to drivers/usb/serial/keyspan_pda.c code */
static int emi62_set_reset (struct usb_device *dev, unsigned char reset_bit)
{
	int response;
	dev_info(&dev->dev, "%s - %d\n", __func__, reset_bit);
	
	response = emi62_writememory (dev, CPUCS_REG, &reset_bit, 1, 0xa0);
	if (response < 0)
		dev_err(&dev->dev, "set_reset (%d) failed\n", reset_bit);
	return response;
}

#define FW_LOAD_SIZE		1023

static int emi62_load_firmware (struct usb_device *dev)
{
	const struct firmware *loader_fw = NULL;
	const struct firmware *bitstream_fw = NULL;
	const struct firmware *firmware_fw = NULL;
	const struct ihex_binrec *rec;
	int err = -ENOMEM;
	int i;
	__u32 addr;	/* Address to write */
	__u8 *buf;

	dev_dbg(&dev->dev, "load_firmware\n");
	buf = kmalloc(FW_LOAD_SIZE, GFP_KERNEL);
	if (!buf)
		goto wraperr;

	err = request_ihex_firmware(&loader_fw, "emi62/loader.fw", &dev->dev);
	if (err)
		goto nofw;

	err = request_ihex_firmware(&bitstream_fw, "emi62/bitstream.fw",
				    &dev->dev);
	if (err)
		goto nofw;

	err = request_ihex_firmware(&firmware_fw, FIRMWARE_FW, &dev->dev);
	if (err) {
	nofw:
		goto wraperr;
	}

	/* Assert reset (stop the CPU in the EMI) */
	err = emi62_set_reset(dev,1);
	if (err < 0)
		goto wraperr;

	rec = (const struct ihex_binrec *)loader_fw->data;

	/* 1. We need to put the loader for the FPGA into the EZ-USB */
	while (rec) {
		err = emi62_writememory(dev, be32_to_cpu(rec->addr),
					rec->data, be16_to_cpu(rec->len),
					ANCHOR_LOAD_INTERNAL);
		if (err < 0)
			goto wraperr;
		rec = ihex_next_binrec(rec);
	}

	/* De-assert reset (let the CPU run) */
	err = emi62_set_reset(dev,0);
	if (err < 0)
		goto wraperr;
	msleep(250);	/* let device settle */

	/* 2. We upload the FPGA firmware into the EMI
	 * Note: collect up to 1023 (yes!) bytes and send them with
	 * a single request. This is _much_ faster! */
	rec = (const struct ihex_binrec *)bitstream_fw->data;
	do {
		i = 0;
		addr = be32_to_cpu(rec->addr);

		/* intel hex records are terminated with type 0 element */
		while (rec && (i + be16_to_cpu(rec->len) < FW_LOAD_SIZE)) {
			memcpy(buf + i, rec->data, be16_to_cpu(rec->len));
			i += be16_to_cpu(rec->len);
			rec = ihex_next_binrec(rec);
		}
		err = emi62_writememory(dev, addr, buf, i, ANCHOR_LOAD_FPGA);
		if (err < 0)
			goto wraperr;
	} while (rec);

	/* Assert reset (stop the CPU in the EMI) */
	err = emi62_set_reset(dev,1);
	if (err < 0)
		goto wraperr;

	/* 3. We need to put the loader for the firmware into the EZ-USB (again...) */
	for (rec = (const struct ihex_binrec *)loader_fw->data;
	     rec; rec = ihex_next_binrec(rec)) {
		err = emi62_writememory(dev, be32_to_cpu(rec->addr),
					rec->data, be16_to_cpu(rec->len),
					ANCHOR_LOAD_INTERNAL);
		if (err < 0)
			goto wraperr;
	}

	/* De-assert reset (let the CPU run) */
	err = emi62_set_reset(dev,0);
	if (err < 0)
		goto wraperr;
	msleep(250);	/* let device settle */

	/* 4. We put the part of the firmware that lies in the external RAM into the EZ-USB */

	for (rec = (const struct ihex_binrec *)firmware_fw->data;
	     rec; rec = ihex_next_binrec(rec)) {
		if (!INTERNAL_RAM(be32_to_cpu(rec->addr))) {
			err = emi62_writememory(dev, be32_to_cpu(rec->addr),
						rec->data, be16_to_cpu(rec->len),
						ANCHOR_LOAD_EXTERNAL);
			if (err < 0)
				goto wraperr;
		}
	}

	/* Assert reset (stop the CPU in the EMI) */
	err = emi62_set_reset(dev,1);
	if (err < 0)
		goto wraperr;

	for (rec = (const struct ihex_binrec *)firmware_fw->data;
	     rec; rec = ihex_next_binrec(rec)) {
		if (INTERNAL_RAM(be32_to_cpu(rec->addr))) {
			err = emi62_writememory(dev, be32_to_cpu(rec->addr),
						rec->data, be16_to_cpu(rec->len),
						ANCHOR_LOAD_EXTERNAL);
			if (err < 0)
				goto wraperr;
		}
	}

	/* De-assert reset (let the CPU run) */
	err = emi62_set_reset(dev,0);
	if (err < 0)
		goto wraperr;
	msleep(250);	/* let device settle */

	release_firmware(loader_fw);
	release_firmware(bitstream_fw);
	release_firmware(firmware_fw);

	kfree(buf);

	/* return 1 to fail the driver inialization
	 * and give real driver change to load */
	return 1;

wraperr:
	if (err < 0)
		dev_err(&dev->dev,"%s - error loading firmware: error = %d\n",
			__func__, err);
	release_firmware(loader_fw);
	release_firmware(bitstream_fw);
	release_firmware(firmware_fw);

	kfree(buf);
	dev_err(&dev->dev, "Error\n");
	return err;
}

static const struct usb_device_id id_table[] = {
	{ USB_DEVICE(EMI62_VENDOR_ID, EMI62_PRODUCT_ID) },
	{ }                                             /* Terminating entry */
};

MODULE_DEVICE_TABLE (usb, id_table);

static int emi62_probe(struct usb_interface *intf, const struct usb_device_id *id)
{
	struct usb_device *dev = interface_to_usbdev(intf);
	dev_dbg(&intf->dev, "emi62_probe\n");

	dev_info(&intf->dev, "%s start\n", __func__);

	emi62_load_firmware(dev);

	/* do not return the driver context, let real audio driver do that */
	return -EIO;
}

static void emi62_disconnect(struct usb_interface *intf)
{
}

static struct usb_driver emi62_driver = {
	.name		= "emi62 - firmware loader",
	.probe		= emi62_probe,
	.disconnect	= emi62_disconnect,
	.id_table	= id_table,
};

module_usb_driver(emi62_driver);

MODULE_AUTHOR("Tapio Laxström");
MODULE_DESCRIPTION("Emagic EMI 6|2m firmware loader.");
MODULE_LICENSE("GPL");

MODULE_FIRMWARE("emi62/loader.fw");
MODULE_FIRMWARE("emi62/bitstream.fw");
MODULE_FIRMWARE(FIRMWARE_FW);
/* vi:ai:syntax=c:sw=8:ts=8:tw=80
 */
