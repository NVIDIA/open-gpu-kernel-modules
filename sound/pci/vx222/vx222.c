// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Driver for Digigram VX222 V2/Mic PCI soundcards
 *
 * Copyright (c) 2002 by Takashi Iwai <tiwai@suse.de>
 */

#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/pci.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <sound/core.h>
#include <sound/initval.h>
#include <sound/tlv.h>
#include "vx222.h"

#define CARD_NAME "VX222"

MODULE_AUTHOR("Takashi Iwai <tiwai@suse.de>");
MODULE_DESCRIPTION("Digigram VX222 V2/Mic");
MODULE_LICENSE("GPL");

static int index[SNDRV_CARDS] = SNDRV_DEFAULT_IDX;	/* Index 0-MAX */
static char *id[SNDRV_CARDS] = SNDRV_DEFAULT_STR;	/* ID for this card */
static bool enable[SNDRV_CARDS] = SNDRV_DEFAULT_ENABLE_PNP;	/* Enable this card */
static bool mic[SNDRV_CARDS]; /* microphone */
static int ibl[SNDRV_CARDS]; /* microphone */

module_param_array(index, int, NULL, 0444);
MODULE_PARM_DESC(index, "Index value for Digigram " CARD_NAME " soundcard.");
module_param_array(id, charp, NULL, 0444);
MODULE_PARM_DESC(id, "ID string for Digigram " CARD_NAME " soundcard.");
module_param_array(enable, bool, NULL, 0444);
MODULE_PARM_DESC(enable, "Enable Digigram " CARD_NAME " soundcard.");
module_param_array(mic, bool, NULL, 0444);
MODULE_PARM_DESC(mic, "Enable Microphone.");
module_param_array(ibl, int, NULL, 0444);
MODULE_PARM_DESC(ibl, "Capture IBL size.");

/*
 */

enum {
	VX_PCI_VX222_OLD,
	VX_PCI_VX222_NEW
};

static const struct pci_device_id snd_vx222_ids[] = {
	{ 0x10b5, 0x9050, 0x1369, PCI_ANY_ID, 0, 0, VX_PCI_VX222_OLD, },   /* PLX */
	{ 0x10b5, 0x9030, 0x1369, PCI_ANY_ID, 0, 0, VX_PCI_VX222_NEW, },   /* PLX */
	{ 0, }
};

MODULE_DEVICE_TABLE(pci, snd_vx222_ids);


/*
 */

static const DECLARE_TLV_DB_SCALE(db_scale_old_vol, -11350, 50, 0);
static const DECLARE_TLV_DB_SCALE(db_scale_akm, -7350, 50, 0);

static const struct snd_vx_hardware vx222_old_hw = {

	.name = "VX222/Old",
	.type = VX_TYPE_BOARD,
	/* hw specs */
	.num_codecs = 1,
	.num_ins = 1,
	.num_outs = 1,
	.output_level_max = VX_ANALOG_OUT_LEVEL_MAX,
	.output_level_db_scale = db_scale_old_vol,
};

static const struct snd_vx_hardware vx222_v2_hw = {

	.name = "VX222/v2",
	.type = VX_TYPE_V2,
	/* hw specs */
	.num_codecs = 1,
	.num_ins = 1,
	.num_outs = 1,
	.output_level_max = VX2_AKM_LEVEL_MAX,
	.output_level_db_scale = db_scale_akm,
};

static const struct snd_vx_hardware vx222_mic_hw = {

	.name = "VX222/Mic",
	.type = VX_TYPE_MIC,
	/* hw specs */
	.num_codecs = 1,
	.num_ins = 1,
	.num_outs = 1,
	.output_level_max = VX2_AKM_LEVEL_MAX,
	.output_level_db_scale = db_scale_akm,
};


/*
 */
static int snd_vx222_free(struct vx_core *chip)
{
	struct snd_vx222 *vx = to_vx222(chip);

	if (chip->irq >= 0)
		free_irq(chip->irq, (void*)chip);
	if (vx->port[0])
		pci_release_regions(vx->pci);
	pci_disable_device(vx->pci);
	kfree(chip);
	return 0;
}

static int snd_vx222_dev_free(struct snd_device *device)
{
	struct vx_core *chip = device->device_data;
	return snd_vx222_free(chip);
}


static int snd_vx222_create(struct snd_card *card, struct pci_dev *pci,
			    const struct snd_vx_hardware *hw,
			    struct snd_vx222 **rchip)
{
	struct vx_core *chip;
	struct snd_vx222 *vx;
	int i, err;
	static const struct snd_device_ops ops = {
		.dev_free =	snd_vx222_dev_free,
	};
	const struct snd_vx_ops *vx_ops;

	/* enable PCI device */
	if ((err = pci_enable_device(pci)) < 0)
		return err;
	pci_set_master(pci);

	vx_ops = hw->type == VX_TYPE_BOARD ? &vx222_old_ops : &vx222_ops;
	chip = snd_vx_create(card, hw, vx_ops,
			     sizeof(struct snd_vx222) - sizeof(struct vx_core));
	if (! chip) {
		pci_disable_device(pci);
		return -ENOMEM;
	}
	vx = to_vx222(chip);
	vx->pci = pci;

	if ((err = pci_request_regions(pci, CARD_NAME)) < 0) {
		snd_vx222_free(chip);
		return err;
	}
	for (i = 0; i < 2; i++)
		vx->port[i] = pci_resource_start(pci, i + 1);

	if (request_threaded_irq(pci->irq, snd_vx_irq_handler,
				 snd_vx_threaded_irq_handler, IRQF_SHARED,
				 KBUILD_MODNAME, chip)) {
		dev_err(card->dev, "unable to grab IRQ %d\n", pci->irq);
		snd_vx222_free(chip);
		return -EBUSY;
	}
	chip->irq = pci->irq;
	card->sync_irq = chip->irq;

	if ((err = snd_device_new(card, SNDRV_DEV_LOWLEVEL, chip, &ops)) < 0) {
		snd_vx222_free(chip);
		return err;
	}

	*rchip = vx;
	return 0;
}


static int snd_vx222_probe(struct pci_dev *pci,
			   const struct pci_device_id *pci_id)
{
	static int dev;
	struct snd_card *card;
	const struct snd_vx_hardware *hw;
	struct snd_vx222 *vx;
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

	switch ((int)pci_id->driver_data) {
	case VX_PCI_VX222_OLD:
		hw = &vx222_old_hw;
		break;
	case VX_PCI_VX222_NEW:
	default:
		if (mic[dev])
			hw = &vx222_mic_hw;
		else
			hw = &vx222_v2_hw;
		break;
	}
	if ((err = snd_vx222_create(card, pci, hw, &vx)) < 0) {
		snd_card_free(card);
		return err;
	}
	card->private_data = vx;
	vx->core.ibl.size = ibl[dev];

	sprintf(card->longname, "%s at 0x%lx & 0x%lx, irq %i",
		card->shortname, vx->port[0], vx->port[1], vx->core.irq);
	dev_dbg(card->dev, "%s at 0x%lx & 0x%lx, irq %i\n",
		    card->shortname, vx->port[0], vx->port[1], vx->core.irq);

#ifdef SND_VX_FW_LOADER
	vx->core.dev = &pci->dev;
#endif

	if ((err = snd_vx_setup_firmware(&vx->core)) < 0) {
		snd_card_free(card);
		return err;
	}

	if ((err = snd_card_register(card)) < 0) {
		snd_card_free(card);
		return err;
	}

	pci_set_drvdata(pci, card);
	dev++;
	return 0;
}

static void snd_vx222_remove(struct pci_dev *pci)
{
	snd_card_free(pci_get_drvdata(pci));
}

#ifdef CONFIG_PM_SLEEP
static int snd_vx222_suspend(struct device *dev)
{
	struct snd_card *card = dev_get_drvdata(dev);
	struct snd_vx222 *vx = card->private_data;

	return snd_vx_suspend(&vx->core);
}

static int snd_vx222_resume(struct device *dev)
{
	struct snd_card *card = dev_get_drvdata(dev);
	struct snd_vx222 *vx = card->private_data;

	return snd_vx_resume(&vx->core);
}

static SIMPLE_DEV_PM_OPS(snd_vx222_pm, snd_vx222_suspend, snd_vx222_resume);
#define SND_VX222_PM_OPS	&snd_vx222_pm
#else
#define SND_VX222_PM_OPS	NULL
#endif

static struct pci_driver vx222_driver = {
	.name = KBUILD_MODNAME,
	.id_table = snd_vx222_ids,
	.probe = snd_vx222_probe,
	.remove = snd_vx222_remove,
	.driver = {
		.pm = SND_VX222_PM_OPS,
	},
};

module_pci_driver(vx222_driver);
