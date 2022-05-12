// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Linux driver for TerraTec DMX 6Fire USB
 *
 * Main routines and module definitions.
 *
 * Author:	Torsten Schenk <torsten.schenk@zoho.com>
 * Created:	Jan 01, 2011
 * Copyright:	(C) Torsten Schenk
 */

#include "chip.h"
#include "firmware.h"
#include "pcm.h"
#include "control.h"
#include "comm.h"
#include "midi.h"

#include <linux/moduleparam.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/gfp.h>
#include <sound/initval.h>

MODULE_AUTHOR("Torsten Schenk <torsten.schenk@zoho.com>");
MODULE_DESCRIPTION("TerraTec DMX 6Fire USB audio driver");
MODULE_LICENSE("GPL v2");

static int index[SNDRV_CARDS] = SNDRV_DEFAULT_IDX; /* Index 0-max */
static char *id[SNDRV_CARDS] = SNDRV_DEFAULT_STR; /* Id for card */
static bool enable[SNDRV_CARDS] = SNDRV_DEFAULT_ENABLE_PNP; /* Enable card */
static struct sfire_chip *chips[SNDRV_CARDS] = SNDRV_DEFAULT_PTR;
static struct usb_device *devices[SNDRV_CARDS] = SNDRV_DEFAULT_PTR;

module_param_array(index, int, NULL, 0444);
MODULE_PARM_DESC(index, "Index value for the 6fire sound device");
module_param_array(id, charp, NULL, 0444);
MODULE_PARM_DESC(id, "ID string for the 6fire sound device.");
module_param_array(enable, bool, NULL, 0444);
MODULE_PARM_DESC(enable, "Enable the 6fire sound device.");

static DEFINE_MUTEX(register_mutex);

static void usb6fire_chip_abort(struct sfire_chip *chip)
{
	if (chip) {
		if (chip->pcm)
			usb6fire_pcm_abort(chip);
		if (chip->midi)
			usb6fire_midi_abort(chip);
		if (chip->comm)
			usb6fire_comm_abort(chip);
		if (chip->control)
			usb6fire_control_abort(chip);
		if (chip->card) {
			snd_card_disconnect(chip->card);
			snd_card_free_when_closed(chip->card);
			chip->card = NULL;
		}
	}
}

static void usb6fire_chip_destroy(struct sfire_chip *chip)
{
	if (chip) {
		if (chip->pcm)
			usb6fire_pcm_destroy(chip);
		if (chip->midi)
			usb6fire_midi_destroy(chip);
		if (chip->comm)
			usb6fire_comm_destroy(chip);
		if (chip->control)
			usb6fire_control_destroy(chip);
		if (chip->card)
			snd_card_free(chip->card);
	}
}

static int usb6fire_chip_probe(struct usb_interface *intf,
			       const struct usb_device_id *usb_id)
{
	int ret;
	int i;
	struct sfire_chip *chip = NULL;
	struct usb_device *device = interface_to_usbdev(intf);
	int regidx = -1; /* index in module parameter array */
	struct snd_card *card = NULL;

	/* look if we already serve this card and return if so */
	mutex_lock(&register_mutex);
	for (i = 0; i < SNDRV_CARDS; i++) {
		if (devices[i] == device) {
			if (chips[i])
				chips[i]->intf_count++;
			usb_set_intfdata(intf, chips[i]);
			mutex_unlock(&register_mutex);
			return 0;
		} else if (!devices[i] && regidx < 0)
			regidx = i;
	}
	if (regidx < 0) {
		mutex_unlock(&register_mutex);
		dev_err(&intf->dev, "too many cards registered.\n");
		return -ENODEV;
	}
	devices[regidx] = device;
	mutex_unlock(&register_mutex);

	/* check, if firmware is present on device, upload it if not */
	ret = usb6fire_fw_init(intf);
	if (ret < 0)
		return ret;
	else if (ret == FW_NOT_READY) /* firmware update performed */
		return 0;

	/* if we are here, card can be registered in alsa. */
	if (usb_set_interface(device, 0, 0) != 0) {
		dev_err(&intf->dev, "can't set first interface.\n");
		return -EIO;
	}
	ret = snd_card_new(&intf->dev, index[regidx], id[regidx],
			   THIS_MODULE, sizeof(struct sfire_chip), &card);
	if (ret < 0) {
		dev_err(&intf->dev, "cannot create alsa card.\n");
		return ret;
	}
	strcpy(card->driver, "6FireUSB");
	strcpy(card->shortname, "TerraTec DMX6FireUSB");
	sprintf(card->longname, "%s at %d:%d", card->shortname,
			device->bus->busnum, device->devnum);

	chip = card->private_data;
	chips[regidx] = chip;
	chip->dev = device;
	chip->regidx = regidx;
	chip->intf_count = 1;
	chip->card = card;

	ret = usb6fire_comm_init(chip);
	if (ret < 0)
		goto destroy_chip;

	ret = usb6fire_midi_init(chip);
	if (ret < 0)
		goto destroy_chip;

	ret = usb6fire_pcm_init(chip);
	if (ret < 0)
		goto destroy_chip;

	ret = usb6fire_control_init(chip);
	if (ret < 0)
		goto destroy_chip;

	ret = snd_card_register(card);
	if (ret < 0) {
		dev_err(&intf->dev, "cannot register card.");
		goto destroy_chip;
	}
	usb_set_intfdata(intf, chip);
	return 0;

destroy_chip:
	usb6fire_chip_destroy(chip);
	return ret;
}

static void usb6fire_chip_disconnect(struct usb_interface *intf)
{
	struct sfire_chip *chip;

	chip = usb_get_intfdata(intf);
	if (chip) { /* if !chip, fw upload has been performed */
		chip->intf_count--;
		if (!chip->intf_count) {
			mutex_lock(&register_mutex);
			devices[chip->regidx] = NULL;
			chips[chip->regidx] = NULL;
			mutex_unlock(&register_mutex);

			chip->shutdown = true;
			usb6fire_chip_abort(chip);
			usb6fire_chip_destroy(chip);
		}
	}
}

static const struct usb_device_id device_table[] = {
	{
		.match_flags = USB_DEVICE_ID_MATCH_DEVICE,
		.idVendor = 0x0ccd,
		.idProduct = 0x0080
	},
	{}
};

MODULE_DEVICE_TABLE(usb, device_table);

static struct usb_driver usb_driver = {
	.name = "snd-usb-6fire",
	.probe = usb6fire_chip_probe,
	.disconnect = usb6fire_chip_disconnect,
	.id_table = device_table,
};

module_usb_driver(usb_driver);
