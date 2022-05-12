// SPDX-License-Identifier: GPL-2.0-or-later

/*
    card-als100.c - driver for Avance Logic ALS100 based soundcards.
    Copyright (C) 1999-2000 by Massimo Piccioni <dafastidio@libero.it>
    Copyright (C) 1999-2002 by Massimo Piccioni <dafastidio@libero.it>

    Thanks to Pierfrancesco 'qM2' Passerini.

    Generalised for soundcards based on DT-0196 and ALS-007 chips
    by Jonathan Woithe <jwoithe@just42.net>: June 2002.

*/

#include <linux/init.h>
#include <linux/wait.h>
#include <linux/time.h>
#include <linux/pnp.h>
#include <linux/module.h>
#include <sound/core.h>
#include <sound/initval.h>
#include <sound/mpu401.h>
#include <sound/opl3.h>
#include <sound/sb.h>

#define PFX "als100: "

MODULE_DESCRIPTION("Avance Logic ALS007/ALS1X0");
MODULE_AUTHOR("Massimo Piccioni <dafastidio@libero.it>");
MODULE_LICENSE("GPL");

static int index[SNDRV_CARDS] = SNDRV_DEFAULT_IDX;	/* Index 0-MAX */
static char *id[SNDRV_CARDS] = SNDRV_DEFAULT_STR;	/* ID for this card */
static bool enable[SNDRV_CARDS] = SNDRV_DEFAULT_ENABLE;	/* Enable this card */
static long port[SNDRV_CARDS] = SNDRV_DEFAULT_PORT;	/* PnP setup */
static long mpu_port[SNDRV_CARDS] = SNDRV_DEFAULT_PORT;	/* PnP setup */
static long fm_port[SNDRV_CARDS] = SNDRV_DEFAULT_PORT;	/* PnP setup */
static int irq[SNDRV_CARDS] = SNDRV_DEFAULT_IRQ;	/* PnP setup */
static int mpu_irq[SNDRV_CARDS] = SNDRV_DEFAULT_IRQ;	/* PnP setup */
static int dma8[SNDRV_CARDS] = SNDRV_DEFAULT_DMA;	/* PnP setup */
static int dma16[SNDRV_CARDS] = SNDRV_DEFAULT_DMA;	/* PnP setup */

module_param_array(index, int, NULL, 0444);
MODULE_PARM_DESC(index, "Index value for Avance Logic based soundcard.");
module_param_array(id, charp, NULL, 0444);
MODULE_PARM_DESC(id, "ID string for Avance Logic based soundcard.");
module_param_array(enable, bool, NULL, 0444);
MODULE_PARM_DESC(enable, "Enable Avance Logic based soundcard.");

MODULE_ALIAS("snd-dt019x");

struct snd_card_als100 {
	struct pnp_dev *dev;
	struct pnp_dev *devmpu;
	struct pnp_dev *devopl;
	struct snd_sb *chip;
};

static const struct pnp_card_device_id snd_als100_pnpids[] = {
	/* DT197A30 */
	{ .id = "RWB1688",
	  .devs = { { "@@@0001" }, { "@X@0001" }, { "@H@0001" } },
	  .driver_data = SB_HW_DT019X },
	/* DT0196 / ALS-007 */
	{ .id = "ALS0007",
	  .devs = { { "@@@0001" }, { "@X@0001" }, { "@H@0001" } },
	  .driver_data = SB_HW_DT019X },
	/* ALS100 - PRO16PNP */
	{ .id = "ALS0001",
	  .devs = { { "@@@0001" }, { "@X@0001" }, { "@H@0001" } },
	  .driver_data = SB_HW_ALS100 },
	/* ALS110 - MF1000 - Digimate 3D Sound */
	{ .id = "ALS0110",
	  .devs = { { "@@@1001" }, { "@X@1001" }, { "@H@1001" } },
	  .driver_data = SB_HW_ALS100 },
	/* ALS120 */
	{ .id = "ALS0120",
	  .devs = { { "@@@2001" }, { "@X@2001" }, { "@H@2001" } },
	  .driver_data = SB_HW_ALS100 },
	/* ALS200 */
	{ .id = "ALS0200",
	  .devs = { { "@@@0020" }, { "@X@0020" }, { "@H@0001" } },
	  .driver_data = SB_HW_ALS100 },
	/* ALS200 OEM */
	{ .id = "ALS0200",
	  .devs = { { "@@@0020" }, { "@X@0020" }, { "@H@0020" } },
	  .driver_data = SB_HW_ALS100 },
	/* RTL3000 */
	{ .id = "RTL3000",
	  .devs = { { "@@@2001" }, { "@X@2001" }, { "@H@2001" } },
	  .driver_data = SB_HW_ALS100 },
	{ .id = "" } /* end */
};

MODULE_DEVICE_TABLE(pnp_card, snd_als100_pnpids);

static int snd_card_als100_pnp(int dev, struct snd_card_als100 *acard,
			       struct pnp_card_link *card,
			       const struct pnp_card_device_id *id)
{
	struct pnp_dev *pdev;
	int err;

	acard->dev = pnp_request_card_device(card, id->devs[0].id, NULL);
	if (acard->dev == NULL)
		return -ENODEV;

	acard->devmpu = pnp_request_card_device(card, id->devs[1].id, acard->dev);
	acard->devopl = pnp_request_card_device(card, id->devs[2].id, acard->dev);

	pdev = acard->dev;

	err = pnp_activate_dev(pdev);
	if (err < 0) {
		snd_printk(KERN_ERR PFX "AUDIO pnp configure failure\n");
		return err;
	}
	port[dev] = pnp_port_start(pdev, 0);
	if (id->driver_data == SB_HW_DT019X)
		dma8[dev] = pnp_dma(pdev, 0);
	else {
		dma8[dev] = pnp_dma(pdev, 1);
		dma16[dev] = pnp_dma(pdev, 0);
	}
	irq[dev] = pnp_irq(pdev, 0);

	pdev = acard->devmpu;
	if (pdev != NULL) {
		err = pnp_activate_dev(pdev);
		if (err < 0)
			goto __mpu_error;
		mpu_port[dev] = pnp_port_start(pdev, 0);
		mpu_irq[dev] = pnp_irq(pdev, 0);
	} else {
	     __mpu_error:
	     	if (pdev) {
		     	pnp_release_card_device(pdev);
	     		snd_printk(KERN_ERR PFX "MPU401 pnp configure failure, skipping\n");
	     	}
	     	acard->devmpu = NULL;
	     	mpu_port[dev] = -1;
	}

	pdev = acard->devopl;
	if (pdev != NULL) {
		err = pnp_activate_dev(pdev);
		if (err < 0)
			goto __fm_error;
		fm_port[dev] = pnp_port_start(pdev, 0);
	} else {
	      __fm_error:
	     	if (pdev) {
		     	pnp_release_card_device(pdev);
	     		snd_printk(KERN_ERR PFX "OPL3 pnp configure failure, skipping\n");
	     	}
	     	acard->devopl = NULL;
	     	fm_port[dev] = -1;
	}

	return 0;
}

static int snd_card_als100_probe(int dev,
				 struct pnp_card_link *pcard,
				 const struct pnp_card_device_id *pid)
{
	int error;
	struct snd_sb *chip;
	struct snd_card *card;
	struct snd_card_als100 *acard;
	struct snd_opl3 *opl3;

	error = snd_card_new(&pcard->card->dev,
			     index[dev], id[dev], THIS_MODULE,
			     sizeof(struct snd_card_als100), &card);
	if (error < 0)
		return error;
	acard = card->private_data;

	if ((error = snd_card_als100_pnp(dev, acard, pcard, pid))) {
		snd_card_free(card);
		return error;
	}

	if (pid->driver_data == SB_HW_DT019X)
		dma16[dev] = -1;

	error = snd_sbdsp_create(card, port[dev], irq[dev],
				  snd_sb16dsp_interrupt,
				  dma8[dev], dma16[dev],
				  pid->driver_data,
				  &chip);
	if (error < 0) {
		snd_card_free(card);
		return error;
	}
	acard->chip = chip;

	if (pid->driver_data == SB_HW_DT019X) {
		strcpy(card->driver, "DT-019X");
		strcpy(card->shortname, "Diamond Tech. DT-019X");
		snprintf(card->longname, sizeof(card->longname),
			 "Diamond Tech. DT-019X, %s at 0x%lx, irq %d, dma %d",
			 chip->name, chip->port, irq[dev], dma8[dev]);
	} else {
		strcpy(card->driver, "ALS100");
		strcpy(card->shortname, "Avance Logic ALS100");
		snprintf(card->longname, sizeof(card->longname),
			 "Avance Logic ALS100, %s at 0x%lx, irq %d, dma %d&%d",
			 chip->name, chip->port, irq[dev], dma8[dev],
			 dma16[dev]);
	}

	if ((error = snd_sb16dsp_pcm(chip, 0)) < 0) {
		snd_card_free(card);
		return error;
	}

	if ((error = snd_sbmixer_new(chip)) < 0) {
		snd_card_free(card);
		return error;
	}

	if (mpu_port[dev] > 0 && mpu_port[dev] != SNDRV_AUTO_PORT) {
		int mpu_type = MPU401_HW_ALS100;

		if (mpu_irq[dev] == SNDRV_AUTO_IRQ)
			mpu_irq[dev] = -1;

		if (pid->driver_data == SB_HW_DT019X)
			mpu_type = MPU401_HW_MPU401;

		if (snd_mpu401_uart_new(card, 0,
					mpu_type,
					mpu_port[dev], 0, 
					mpu_irq[dev],
					NULL) < 0)
			snd_printk(KERN_ERR PFX "no MPU-401 device at 0x%lx\n", mpu_port[dev]);
	}

	if (fm_port[dev] > 0 && fm_port[dev] != SNDRV_AUTO_PORT) {
		if (snd_opl3_create(card,
				    fm_port[dev], fm_port[dev] + 2,
				    OPL3_HW_AUTO, 0, &opl3) < 0) {
			snd_printk(KERN_ERR PFX "no OPL device at 0x%lx-0x%lx\n",
				   fm_port[dev], fm_port[dev] + 2);
		} else {
			if ((error = snd_opl3_timer_new(opl3, 0, 1)) < 0) {
				snd_card_free(card);
				return error;
			}
			if ((error = snd_opl3_hwdep_new(opl3, 0, 1, NULL)) < 0) {
				snd_card_free(card);
				return error;
			}
		}
	}

	if ((error = snd_card_register(card)) < 0) {
		snd_card_free(card);
		return error;
	}
	pnp_set_card_drvdata(pcard, card);
	return 0;
}

static unsigned int als100_devices;

static int snd_als100_pnp_detect(struct pnp_card_link *card,
				 const struct pnp_card_device_id *id)
{
	static int dev;
	int res;

	for ( ; dev < SNDRV_CARDS; dev++) {
		if (!enable[dev])
			continue;
		res = snd_card_als100_probe(dev, card, id);
		if (res < 0)
			return res;
		dev++;
		als100_devices++;
		return 0;
	}
	return -ENODEV;
}

static void snd_als100_pnp_remove(struct pnp_card_link *pcard)
{
	snd_card_free(pnp_get_card_drvdata(pcard));
	pnp_set_card_drvdata(pcard, NULL);
}

#ifdef CONFIG_PM
static int snd_als100_pnp_suspend(struct pnp_card_link *pcard, pm_message_t state)
{
	struct snd_card *card = pnp_get_card_drvdata(pcard);
	struct snd_card_als100 *acard = card->private_data;
	struct snd_sb *chip = acard->chip;

	snd_power_change_state(card, SNDRV_CTL_POWER_D3hot);
	snd_sbmixer_suspend(chip);
	return 0;
}

static int snd_als100_pnp_resume(struct pnp_card_link *pcard)
{
	struct snd_card *card = pnp_get_card_drvdata(pcard);
	struct snd_card_als100 *acard = card->private_data;
	struct snd_sb *chip = acard->chip;

	snd_sbdsp_reset(chip);
	snd_sbmixer_resume(chip);
	snd_power_change_state(card, SNDRV_CTL_POWER_D0);
	return 0;
}
#endif

static struct pnp_card_driver als100_pnpc_driver = {
	.flags          = PNP_DRIVER_RES_DISABLE,
	.name		= "als100",
        .id_table       = snd_als100_pnpids,
        .probe          = snd_als100_pnp_detect,
	.remove		= snd_als100_pnp_remove,
#ifdef CONFIG_PM
	.suspend	= snd_als100_pnp_suspend,
	.resume		= snd_als100_pnp_resume,
#endif
};

static int __init alsa_card_als100_init(void)
{
	int err;

	err = pnp_register_card_driver(&als100_pnpc_driver);
	if (err)
		return err;

	if (!als100_devices) {
		pnp_unregister_card_driver(&als100_pnpc_driver);
#ifdef MODULE
		snd_printk(KERN_ERR "no Avance Logic based soundcards found\n");
#endif
		return -ENODEV;
	}
	return 0;
}

static void __exit alsa_card_als100_exit(void)
{
	pnp_unregister_card_driver(&als100_pnpc_driver);
}

module_init(alsa_card_als100_init)
module_exit(alsa_card_als100_exit)
