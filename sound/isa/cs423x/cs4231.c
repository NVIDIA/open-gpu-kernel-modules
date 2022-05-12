// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  Generic driver for CS4231 chips
 *  Copyright (c) by Jaroslav Kysela <perex@perex.cz>
 *  Originally the CS4232/CS4232A driver, modified for use on CS4231 by
 *  Tugrul Galatali <galatalt@stuy.edu>
 */

#include <linux/init.h>
#include <linux/err.h>
#include <linux/isa.h>
#include <linux/time.h>
#include <linux/wait.h>
#include <linux/module.h>
#include <sound/core.h>
#include <sound/wss.h>
#include <sound/mpu401.h>
#include <sound/initval.h>

#define CRD_NAME "Generic CS4231"
#define DEV_NAME "cs4231"

MODULE_DESCRIPTION(CRD_NAME);
MODULE_AUTHOR("Jaroslav Kysela <perex@perex.cz>");
MODULE_LICENSE("GPL");

static int index[SNDRV_CARDS] = SNDRV_DEFAULT_IDX;	/* Index 0-MAX */
static char *id[SNDRV_CARDS] = SNDRV_DEFAULT_STR;	/* ID for this card */
static bool enable[SNDRV_CARDS] = SNDRV_DEFAULT_ENABLE;	/* Enable this card */
static long port[SNDRV_CARDS] = SNDRV_DEFAULT_PORT;	/* PnP setup */
static long mpu_port[SNDRV_CARDS] = SNDRV_DEFAULT_PORT;	/* PnP setup */
static int irq[SNDRV_CARDS] = SNDRV_DEFAULT_IRQ;	/* 5,7,9,11,12,15 */
static int mpu_irq[SNDRV_CARDS] = SNDRV_DEFAULT_IRQ;	/* 9,11,12,15 */
static int dma1[SNDRV_CARDS] = SNDRV_DEFAULT_DMA;	/* 0,1,3,5,6,7 */
static int dma2[SNDRV_CARDS] = SNDRV_DEFAULT_DMA;	/* 0,1,3,5,6,7 */

module_param_array(index, int, NULL, 0444);
MODULE_PARM_DESC(index, "Index value for " CRD_NAME " soundcard.");
module_param_array(id, charp, NULL, 0444);
MODULE_PARM_DESC(id, "ID string for " CRD_NAME " soundcard.");
module_param_array(enable, bool, NULL, 0444);
MODULE_PARM_DESC(enable, "Enable " CRD_NAME " soundcard.");
module_param_hw_array(port, long, ioport, NULL, 0444);
MODULE_PARM_DESC(port, "Port # for " CRD_NAME " driver.");
module_param_hw_array(mpu_port, long, ioport, NULL, 0444);
MODULE_PARM_DESC(mpu_port, "MPU-401 port # for " CRD_NAME " driver.");
module_param_hw_array(irq, int, irq, NULL, 0444);
MODULE_PARM_DESC(irq, "IRQ # for " CRD_NAME " driver.");
module_param_hw_array(mpu_irq, int, irq, NULL, 0444);
MODULE_PARM_DESC(mpu_irq, "MPU-401 IRQ # for " CRD_NAME " driver.");
module_param_hw_array(dma1, int, dma, NULL, 0444);
MODULE_PARM_DESC(dma1, "DMA1 # for " CRD_NAME " driver.");
module_param_hw_array(dma2, int, dma, NULL, 0444);
MODULE_PARM_DESC(dma2, "DMA2 # for " CRD_NAME " driver.");

static int snd_cs4231_match(struct device *dev, unsigned int n)
{
	if (!enable[n])
		return 0;

	if (port[n] == SNDRV_AUTO_PORT) {
		dev_err(dev, "please specify port\n");
		return 0;
	}
	if (irq[n] == SNDRV_AUTO_IRQ) {
		dev_err(dev, "please specify irq\n");
		return 0;
	}
	if (dma1[n] == SNDRV_AUTO_DMA) {
		dev_err(dev, "please specify dma1\n");
		return 0;
	}
	return 1;
}

static int snd_cs4231_probe(struct device *dev, unsigned int n)
{
	struct snd_card *card;
	struct snd_wss *chip;
	int error;

	error = snd_card_new(dev, index[n], id[n], THIS_MODULE, 0, &card);
	if (error < 0)
		return error;

	error = snd_wss_create(card, port[n], -1, irq[n], dma1[n], dma2[n],
			WSS_HW_DETECT, 0, &chip);
	if (error < 0)
		goto out;

	card->private_data = chip;

	error = snd_wss_pcm(chip, 0);
	if (error < 0)
		goto out;

	strscpy(card->driver, "CS4231", sizeof(card->driver));
	strscpy(card->shortname, chip->pcm->name, sizeof(card->shortname));

	if (dma2[n] < 0)
		snprintf(card->longname, sizeof(card->longname),
			 "%s at 0x%lx, irq %d, dma %d",
			 chip->pcm->name, chip->port, irq[n], dma1[n]);
	else
		snprintf(card->longname, sizeof(card->longname),
			 "%s at 0x%lx, irq %d, dma %d&%d",
			 chip->pcm->name, chip->port, irq[n], dma1[n], dma2[n]);

	error = snd_wss_mixer(chip);
	if (error < 0)
		goto out;

	error = snd_wss_timer(chip, 0);
	if (error < 0)
		goto out;

	if (mpu_port[n] > 0 && mpu_port[n] != SNDRV_AUTO_PORT) {
		if (mpu_irq[n] == SNDRV_AUTO_IRQ)
			mpu_irq[n] = -1;
		if (snd_mpu401_uart_new(card, 0, MPU401_HW_CS4232,
					mpu_port[n], 0, mpu_irq[n],
					NULL) < 0)
			dev_warn(dev, "MPU401 not detected\n");
	}

	error = snd_card_register(card);
	if (error < 0)
		goto out;

	dev_set_drvdata(dev, card);
	return 0;

out:	snd_card_free(card);
	return error;
}

static void snd_cs4231_remove(struct device *dev, unsigned int n)
{
	snd_card_free(dev_get_drvdata(dev));
}

#ifdef CONFIG_PM
static int snd_cs4231_suspend(struct device *dev, unsigned int n, pm_message_t state)
{
	struct snd_card *card = dev_get_drvdata(dev);
	struct snd_wss *chip = card->private_data;

	snd_power_change_state(card, SNDRV_CTL_POWER_D3hot);
	chip->suspend(chip);
	return 0;
}

static int snd_cs4231_resume(struct device *dev, unsigned int n)
{
	struct snd_card *card = dev_get_drvdata(dev);
	struct snd_wss *chip = card->private_data;

	chip->resume(chip);
	snd_power_change_state(card, SNDRV_CTL_POWER_D0);
	return 0;
}
#endif

static struct isa_driver snd_cs4231_driver = {
	.match		= snd_cs4231_match,
	.probe		= snd_cs4231_probe,
	.remove		= snd_cs4231_remove,
#ifdef CONFIG_PM
	.suspend	= snd_cs4231_suspend,
	.resume		= snd_cs4231_resume,
#endif
	.driver		= {
		.name	= DEV_NAME
	}
};

module_isa_driver(snd_cs4231_driver, SNDRV_CARDS);
