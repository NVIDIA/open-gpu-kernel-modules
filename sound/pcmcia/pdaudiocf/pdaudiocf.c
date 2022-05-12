// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Driver for Sound Core PDAudioCF soundcard
 *
 * Copyright (c) 2003 by Jaroslav Kysela <perex@perex.cz>
 */

#include <sound/core.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <pcmcia/ciscode.h>
#include <pcmcia/cisreg.h>
#include "pdaudiocf.h"
#include <sound/initval.h>
#include <linux/init.h>

/*
 */

#define CARD_NAME	"PDAudio-CF"

MODULE_AUTHOR("Jaroslav Kysela <perex@perex.cz>");
MODULE_DESCRIPTION("Sound Core " CARD_NAME);
MODULE_LICENSE("GPL");

static int index[SNDRV_CARDS] = SNDRV_DEFAULT_IDX;	/* Index 0-MAX */
static char *id[SNDRV_CARDS] = SNDRV_DEFAULT_STR;	/* ID for this card */
static bool enable[SNDRV_CARDS] = SNDRV_DEFAULT_ENABLE_PNP;	/* Enable switches */

module_param_array(index, int, NULL, 0444);
MODULE_PARM_DESC(index, "Index value for " CARD_NAME " soundcard.");
module_param_array(id, charp, NULL, 0444);
MODULE_PARM_DESC(id, "ID string for " CARD_NAME " soundcard.");
module_param_array(enable, bool, NULL, 0444);
MODULE_PARM_DESC(enable, "Enable " CARD_NAME " soundcard.");

/*
 */

static struct snd_card *card_list[SNDRV_CARDS];

/*
 * prototypes
 */
static int pdacf_config(struct pcmcia_device *link);
static void snd_pdacf_detach(struct pcmcia_device *p_dev);

static void pdacf_release(struct pcmcia_device *link)
{
	free_irq(link->irq, link->priv);
	pcmcia_disable_device(link);
}

/*
 * destructor
 */
static int snd_pdacf_free(struct snd_pdacf *pdacf)
{
	struct pcmcia_device *link = pdacf->p_dev;

	pdacf_release(link);

	card_list[pdacf->index] = NULL;
	pdacf->card = NULL;

	kfree(pdacf);
	return 0;
}

static int snd_pdacf_dev_free(struct snd_device *device)
{
	struct snd_pdacf *chip = device->device_data;
	return snd_pdacf_free(chip);
}

/*
 * snd_pdacf_attach - attach callback for cs
 */
static int snd_pdacf_probe(struct pcmcia_device *link)
{
	int i, err;
	struct snd_pdacf *pdacf;
	struct snd_card *card;
	static const struct snd_device_ops ops = {
		.dev_free =	snd_pdacf_dev_free,
	};

	snd_printdd(KERN_DEBUG "pdacf_attach called\n");
	/* find an empty slot from the card list */
	for (i = 0; i < SNDRV_CARDS; i++) {
		if (! card_list[i])
			break;
	}
	if (i >= SNDRV_CARDS) {
		snd_printk(KERN_ERR "pdacf: too many cards found\n");
		return -EINVAL;
	}
	if (! enable[i])
		return -ENODEV; /* disabled explicitly */

	/* ok, create a card instance */
	err = snd_card_new(&link->dev, index[i], id[i], THIS_MODULE,
			   0, &card);
	if (err < 0) {
		snd_printk(KERN_ERR "pdacf: cannot create a card instance\n");
		return err;
	}

	pdacf = snd_pdacf_create(card);
	if (!pdacf) {
		snd_card_free(card);
		return -ENOMEM;
	}

	err = snd_device_new(card, SNDRV_DEV_LOWLEVEL, pdacf, &ops);
	if (err < 0) {
		kfree(pdacf);
		snd_card_free(card);
		return err;
	}

	pdacf->index = i;
	card_list[i] = card;

	pdacf->p_dev = link;
	link->priv = pdacf;

	link->resource[0]->flags |= IO_DATA_PATH_WIDTH_AUTO;
	link->resource[0]->end = 16;

	link->config_flags = CONF_ENABLE_IRQ | CONF_ENABLE_PULSE_IRQ;
	link->config_index = 1;
	link->config_regs = PRESENT_OPTION;

	return pdacf_config(link);
}


/**
 * snd_pdacf_assign_resources - initialize the hardware and card instance.
 * @pdacf: context
 * @port: i/o port for the card
 * @irq: irq number for the card
 *
 * this function assigns the specified port and irq, boot the card,
 * create pcm and control instances, and initialize the rest hardware.
 *
 * returns 0 if successful, or a negative error code.
 */
static int snd_pdacf_assign_resources(struct snd_pdacf *pdacf, int port, int irq)
{
	int err;
	struct snd_card *card = pdacf->card;

	snd_printdd(KERN_DEBUG "pdacf assign resources: port = 0x%x, irq = %d\n", port, irq);
	pdacf->port = port;
	pdacf->irq = irq;
	pdacf->chip_status |= PDAUDIOCF_STAT_IS_CONFIGURED;

	err = snd_pdacf_ak4117_create(pdacf);
	if (err < 0)
		return err;	

	strcpy(card->driver, "PDAudio-CF");
	sprintf(card->shortname, "Core Sound %s", card->driver);
	sprintf(card->longname, "%s at 0x%x, irq %i",
		card->shortname, port, irq);

	err = snd_pdacf_pcm_new(pdacf);
	if (err < 0)
		return err;

	if ((err = snd_card_register(card)) < 0)
		return err;

	return 0;
}


/*
 * snd_pdacf_detach - detach callback for cs
 */
static void snd_pdacf_detach(struct pcmcia_device *link)
{
	struct snd_pdacf *chip = link->priv;

	snd_printdd(KERN_DEBUG "pdacf_detach called\n");

	if (chip->chip_status & PDAUDIOCF_STAT_IS_CONFIGURED)
		snd_pdacf_powerdown(chip);
	chip->chip_status |= PDAUDIOCF_STAT_IS_STALE; /* to be sure */
	snd_card_disconnect(chip->card);
	snd_card_free_when_closed(chip->card);
}

/*
 * configuration callback
 */

static int pdacf_config(struct pcmcia_device *link)
{
	struct snd_pdacf *pdacf = link->priv;
	int ret;

	snd_printdd(KERN_DEBUG "pdacf_config called\n");
	link->config_index = 0x5;
	link->config_flags |= CONF_ENABLE_IRQ | CONF_ENABLE_PULSE_IRQ;

	ret = pcmcia_request_io(link);
	if (ret)
		goto failed_preirq;

	ret = request_threaded_irq(link->irq, pdacf_interrupt,
				   pdacf_threaded_irq,
				   IRQF_SHARED, link->devname, link->priv);
	if (ret)
		goto failed_preirq;

	ret = pcmcia_enable_device(link);
	if (ret)
		goto failed;

	if (snd_pdacf_assign_resources(pdacf, link->resource[0]->start,
					link->irq) < 0)
		goto failed;

	pdacf->card->sync_irq = link->irq;
	return 0;

 failed:
	free_irq(link->irq, link->priv);
failed_preirq:
	pcmcia_disable_device(link);
	return -ENODEV;
}

#ifdef CONFIG_PM

static int pdacf_suspend(struct pcmcia_device *link)
{
	struct snd_pdacf *chip = link->priv;

	snd_printdd(KERN_DEBUG "SUSPEND\n");
	if (chip) {
		snd_printdd(KERN_DEBUG "snd_pdacf_suspend calling\n");
		snd_pdacf_suspend(chip);
	}

	return 0;
}

static int pdacf_resume(struct pcmcia_device *link)
{
	struct snd_pdacf *chip = link->priv;

	snd_printdd(KERN_DEBUG "RESUME\n");
	if (pcmcia_dev_present(link)) {
		if (chip) {
			snd_printdd(KERN_DEBUG "calling snd_pdacf_resume\n");
			snd_pdacf_resume(chip);
		}
	}
	snd_printdd(KERN_DEBUG "resume done!\n");

	return 0;
}

#endif

/*
 * Module entry points
 */
static const struct pcmcia_device_id snd_pdacf_ids[] = {
	/* this is too general PCMCIA_DEVICE_MANF_CARD(0x015d, 0x4c45), */
	PCMCIA_DEVICE_PROD_ID12("Core Sound","PDAudio-CF",0x396d19d2,0x71717b49),
	PCMCIA_DEVICE_NULL
};
MODULE_DEVICE_TABLE(pcmcia, snd_pdacf_ids);

static struct pcmcia_driver pdacf_cs_driver = {
	.owner		= THIS_MODULE,
	.name		= "snd-pdaudiocf",
	.probe		= snd_pdacf_probe,
	.remove		= snd_pdacf_detach,
	.id_table	= snd_pdacf_ids,
#ifdef CONFIG_PM
	.suspend	= pdacf_suspend,
	.resume		= pdacf_resume,
#endif
};
module_pcmcia_driver(pdacf_cs_driver);
