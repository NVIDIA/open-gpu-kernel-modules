// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * cs5530.c - Initialisation code for Cyrix/NatSemi VSA1 softaudio
 *
 * 	(C) Copyright 2007 Ash Willis <ashwillis@programmer.net>
 *	(C) Copyright 2003 Red Hat Inc <alan@lxorguk.ukuu.org.uk>
 *
 * This driver was ported (shamelessly ripped ;) from oss/kahlua.c but I did
 * mess with it a bit. The chip seems to have to have trouble with full duplex
 * mode. If we're recording in 8bit 8000kHz, say, and we then attempt to
 * simultaneously play back audio at 16bit 44100kHz, the device actually plays
 * back in the same format in which it is capturing. By forcing the chip to
 * always play/capture in 16/44100, we can let alsa-lib convert the samples and
 * that way we can hack up some full duplex audio. 
 * 
 * XpressAudio(tm) is used on the Cyrix MediaGX (now NatSemi Geode) systems.
 * The older version (VSA1) provides fairly good soundblaster emulation
 * although there are a couple of bugs: large DMA buffers break record,
 * and the MPU event handling seems suspect. VSA2 allows the native driver
 * to control the AC97 audio engine directly and requires a different driver.
 *
 * Thanks to National Semiconductor for providing the needed information
 * on the XpressAudio(tm) internals.
 *
 * TO DO:
 *	Investigate whether we can portably support Cognac (5520) in the
 *	same manner.
 */

#include <linux/delay.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/slab.h>
#include <sound/core.h>
#include <sound/sb.h>
#include <sound/initval.h>

MODULE_AUTHOR("Ash Willis");
MODULE_DESCRIPTION("CS5530 Audio");
MODULE_LICENSE("GPL");

static int index[SNDRV_CARDS] = SNDRV_DEFAULT_IDX;
static char *id[SNDRV_CARDS] = SNDRV_DEFAULT_STR;
static bool enable[SNDRV_CARDS] = SNDRV_DEFAULT_ENABLE_PNP;

module_param_array(index, int, NULL, 0444);
MODULE_PARM_DESC(index, "Index value for CS5530 Audio driver.");
module_param_array(id, charp, NULL, 0444);
MODULE_PARM_DESC(id, "ID string for CS5530 Audio driver.");
module_param_array(enable, bool, NULL, 0444);
MODULE_PARM_DESC(enable, "Enable CS5530 Audio driver.");

struct snd_cs5530 {
	struct snd_card *card;
	struct pci_dev *pci;
	struct snd_sb *sb;
	unsigned long pci_base;
};

static const struct pci_device_id snd_cs5530_ids[] = {
	{PCI_VENDOR_ID_CYRIX, PCI_DEVICE_ID_CYRIX_5530_AUDIO, PCI_ANY_ID,
							PCI_ANY_ID, 0, 0},
	{0,}
};

MODULE_DEVICE_TABLE(pci, snd_cs5530_ids);

static int snd_cs5530_free(struct snd_cs5530 *chip)
{
	pci_release_regions(chip->pci);
	pci_disable_device(chip->pci);
	kfree(chip);
	return 0;
}

static int snd_cs5530_dev_free(struct snd_device *device)
{
	struct snd_cs5530 *chip = device->device_data;
	return snd_cs5530_free(chip);
}

static void snd_cs5530_remove(struct pci_dev *pci)
{
	snd_card_free(pci_get_drvdata(pci));
}

static u8 snd_cs5530_mixer_read(unsigned long io, u8 reg)
{
	outb(reg, io + 4);
	udelay(20);
	reg = inb(io + 5);
	udelay(20);
	return reg;
}

static int snd_cs5530_create(struct snd_card *card,
			     struct pci_dev *pci,
			     struct snd_cs5530 **rchip)
{
	struct snd_cs5530 *chip;
	unsigned long sb_base;
	u8 irq, dma8, dma16 = 0;
	u16 map;
	void __iomem *mem;
	int err;

	static const struct snd_device_ops ops = {
		.dev_free = snd_cs5530_dev_free,
	};
	*rchip = NULL;

	err = pci_enable_device(pci);
 	if (err < 0)
		return err;

	chip = kzalloc(sizeof(*chip), GFP_KERNEL);
	if (chip == NULL) {
		pci_disable_device(pci);
		return -ENOMEM;
	}

	chip->card = card;
	chip->pci = pci;

	err = pci_request_regions(pci, "CS5530");
	if (err < 0) {
		kfree(chip); 
		pci_disable_device(pci);
		return err;
	}
	chip->pci_base = pci_resource_start(pci, 0);

	mem = pci_ioremap_bar(pci, 0);
	if (mem == NULL) {
		snd_cs5530_free(chip);
		return -EBUSY;
	}

	map = readw(mem + 0x18);
	iounmap(mem);

	/* Map bits
		0:1	* 0x20 + 0x200 = sb base
		2	sb enable
		3	adlib enable
		5	MPU enable 0x330
		6	MPU enable 0x300

	   The other bits may be used internally so must be masked */

	sb_base = 0x220 + 0x20 * (map & 3);

	if (map & (1<<2))
		dev_info(card->dev, "XpressAudio at 0x%lx\n", sb_base);
	else {
		dev_err(card->dev, "Could not find XpressAudio!\n");
		snd_cs5530_free(chip);
		return -ENODEV;
	}

	if (map & (1<<5))
		dev_info(card->dev, "MPU at 0x300\n");
	else if (map & (1<<6))
		dev_info(card->dev, "MPU at 0x330\n");

	irq = snd_cs5530_mixer_read(sb_base, 0x80) & 0x0F;
	dma8 = snd_cs5530_mixer_read(sb_base, 0x81);

	if (dma8 & 0x20)
		dma16 = 5;
	else if (dma8 & 0x40)
		dma16 = 6;
	else if (dma8 & 0x80)
		dma16 = 7;
	else {
		dev_err(card->dev, "No 16bit DMA enabled\n");
		snd_cs5530_free(chip);
		return -ENODEV;
	}

	if (dma8 & 0x01)
		dma8 = 0;
	else if (dma8 & 02)
		dma8 = 1;
	else if (dma8 & 0x08)
		dma8 = 3;
	else {
		dev_err(card->dev, "No 8bit DMA enabled\n");
		snd_cs5530_free(chip);
		return -ENODEV;
	}

	if (irq & 1)
		irq = 9;
	else if (irq & 2)
		irq = 5;
	else if (irq & 4)
		irq = 7;
	else if (irq & 8)
		irq = 10;
	else {
		dev_err(card->dev, "SoundBlaster IRQ not set\n");
		snd_cs5530_free(chip);
		return -ENODEV;
	}

	dev_info(card->dev, "IRQ: %d DMA8: %d DMA16: %d\n", irq, dma8, dma16);

	err = snd_sbdsp_create(card, sb_base, irq, snd_sb16dsp_interrupt, dma8,
						dma16, SB_HW_CS5530, &chip->sb);
	if (err < 0) {
		dev_err(card->dev, "Could not create SoundBlaster\n");
		snd_cs5530_free(chip);
		return err;
	}

	err = snd_sb16dsp_pcm(chip->sb, 0);
	if (err < 0) {
		dev_err(card->dev, "Could not create PCM\n");
		snd_cs5530_free(chip);
		return err;
	}

	err = snd_sbmixer_new(chip->sb);
	if (err < 0) {
		dev_err(card->dev, "Could not create Mixer\n");
		snd_cs5530_free(chip);
		return err;
	}

	err = snd_device_new(card, SNDRV_DEV_LOWLEVEL, chip, &ops);
	if (err < 0) {
		snd_cs5530_free(chip);
		return err;
	}

	*rchip = chip;
	return 0;
}

static int snd_cs5530_probe(struct pci_dev *pci,
			    const struct pci_device_id *pci_id)
{
	static int dev;
	struct snd_card *card;
	struct snd_cs5530 *chip = NULL;
	int err;

	if (dev >= SNDRV_CARDS)
		return -ENODEV;
	if (!enable[dev]) {
		dev++;
		return -ENOENT;
	}

	err = snd_card_new(&pci->dev, index[dev], id[dev], THIS_MODULE,
			   0, &card);

	if (err < 0)
		return err;

	err = snd_cs5530_create(card, pci, &chip);
	if (err < 0) {
		snd_card_free(card);
		return err;
	}

	strcpy(card->driver, "CS5530");
	strcpy(card->shortname, "CS5530 Audio");
	sprintf(card->longname, "%s at 0x%lx", card->shortname, chip->pci_base);

	err = snd_card_register(card);
	if (err < 0) {
		snd_card_free(card);
		return err;
	}
	pci_set_drvdata(pci, card);
	dev++;
	return 0;
}

static struct pci_driver cs5530_driver = {
	.name = KBUILD_MODNAME,
	.id_table = snd_cs5530_ids,
	.probe = snd_cs5530_probe,
	.remove = snd_cs5530_remove,
};

module_pci_driver(cs5530_driver);
