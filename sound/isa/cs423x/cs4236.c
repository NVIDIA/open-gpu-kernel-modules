// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  Driver for generic CS4232/CS4235/CS4236/CS4236B/CS4237B/CS4238B/CS4239 chips
 *  Copyright (c) by Jaroslav Kysela <perex@perex.cz>
 */

#include <linux/init.h>
#include <linux/err.h>
#include <linux/isa.h>
#include <linux/pnp.h>
#include <linux/module.h>
#include <sound/core.h>
#include <sound/wss.h>
#include <sound/mpu401.h>
#include <sound/opl3.h>
#include <sound/initval.h>

MODULE_AUTHOR("Jaroslav Kysela <perex@perex.cz>");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Cirrus Logic CS4232-9");
MODULE_ALIAS("snd_cs4232");

#define IDENT "CS4232+"
#define DEV_NAME "cs4232+"

static int index[SNDRV_CARDS] = SNDRV_DEFAULT_IDX;	/* Index 0-MAX */
static char *id[SNDRV_CARDS] = SNDRV_DEFAULT_STR;	/* ID for this card */
static bool enable[SNDRV_CARDS] = SNDRV_DEFAULT_ENABLE_ISAPNP; /* Enable this card */
#ifdef CONFIG_PNP
static bool isapnp[SNDRV_CARDS] = {[0 ... (SNDRV_CARDS - 1)] = 1};
#endif
static long port[SNDRV_CARDS] = SNDRV_DEFAULT_PORT;	/* PnP setup */
static long cport[SNDRV_CARDS] = SNDRV_DEFAULT_PORT;	/* PnP setup */
static long mpu_port[SNDRV_CARDS] = SNDRV_DEFAULT_PORT;/* PnP setup */
static long fm_port[SNDRV_CARDS] = SNDRV_DEFAULT_PORT;	/* PnP setup */
static long sb_port[SNDRV_CARDS] = SNDRV_DEFAULT_PORT;	/* PnP setup */
static int irq[SNDRV_CARDS] = SNDRV_DEFAULT_IRQ;	/* 5,7,9,11,12,15 */
static int mpu_irq[SNDRV_CARDS] = SNDRV_DEFAULT_IRQ;	/* 9,11,12,15 */
static int dma1[SNDRV_CARDS] = SNDRV_DEFAULT_DMA;	/* 0,1,3,5,6,7 */
static int dma2[SNDRV_CARDS] = SNDRV_DEFAULT_DMA;	/* 0,1,3,5,6,7 */

module_param_array(index, int, NULL, 0444);
MODULE_PARM_DESC(index, "Index value for " IDENT " soundcard.");
module_param_array(id, charp, NULL, 0444);
MODULE_PARM_DESC(id, "ID string for " IDENT " soundcard.");
module_param_array(enable, bool, NULL, 0444);
MODULE_PARM_DESC(enable, "Enable " IDENT " soundcard.");
#ifdef CONFIG_PNP
module_param_array(isapnp, bool, NULL, 0444);
MODULE_PARM_DESC(isapnp, "ISA PnP detection for specified soundcard.");
#endif
module_param_hw_array(port, long, ioport, NULL, 0444);
MODULE_PARM_DESC(port, "Port # for " IDENT " driver.");
module_param_hw_array(cport, long, ioport, NULL, 0444);
MODULE_PARM_DESC(cport, "Control port # for " IDENT " driver.");
module_param_hw_array(mpu_port, long, ioport, NULL, 0444);
MODULE_PARM_DESC(mpu_port, "MPU-401 port # for " IDENT " driver.");
module_param_hw_array(fm_port, long, ioport, NULL, 0444);
MODULE_PARM_DESC(fm_port, "FM port # for " IDENT " driver.");
module_param_hw_array(sb_port, long, ioport, NULL, 0444);
MODULE_PARM_DESC(sb_port, "SB port # for " IDENT " driver (optional).");
module_param_hw_array(irq, int, irq, NULL, 0444);
MODULE_PARM_DESC(irq, "IRQ # for " IDENT " driver.");
module_param_hw_array(mpu_irq, int, irq, NULL, 0444);
MODULE_PARM_DESC(mpu_irq, "MPU-401 IRQ # for " IDENT " driver.");
module_param_hw_array(dma1, int, dma, NULL, 0444);
MODULE_PARM_DESC(dma1, "DMA1 # for " IDENT " driver.");
module_param_hw_array(dma2, int, dma, NULL, 0444);
MODULE_PARM_DESC(dma2, "DMA2 # for " IDENT " driver.");

#ifdef CONFIG_PNP
static int isa_registered;
static int pnpc_registered;
static int pnp_registered;
#endif /* CONFIG_PNP */

struct snd_card_cs4236 {
	struct snd_wss *chip;
	struct resource *res_sb_port;
#ifdef CONFIG_PNP
	struct pnp_dev *wss;
	struct pnp_dev *ctrl;
	struct pnp_dev *mpu;
#endif
};

#ifdef CONFIG_PNP

/*
 * PNP BIOS
 */
static const struct pnp_device_id snd_cs423x_pnpbiosids[] = {
	{ .id = "CSC0100" },
	{ .id = "CSC0000" },
	/* Guillemot Turtlebeach something appears to be cs4232 compatible
	 * (untested) */
	{ .id = "GIM0100" },
	{ .id = "" }
};
MODULE_DEVICE_TABLE(pnp, snd_cs423x_pnpbiosids);

#define CS423X_ISAPNP_DRIVER	"cs4232_isapnp"
static const struct pnp_card_device_id snd_cs423x_pnpids[] = {
	/* Philips PCA70PS */
	{ .id = "CSC0d32", .devs = { { "CSC0000" }, { "CSC0010" }, { "PNPb006" } } },
	/* TerraTec Maestro 32/96 (CS4232) */
	{ .id = "CSC1a32", .devs = { { "CSC0000" }, { "CSC0010" }, { "CSC0003" } } },
	/* HP Omnibook 5500 onboard */
	{ .id = "CSC4232", .devs = { { "CSC0000" }, { "CSC0002" }, { "CSC0003" } } },
	/* Unnamed CS4236 card (Made in Taiwan) */
	{ .id = "CSC4236", .devs = { { "CSC0000" }, { "CSC0010" }, { "CSC0003" } } },
	/* Turtle Beach TBS-2000 (CS4232) */
	{ .id = "CSC7532", .devs = { { "CSC0000" }, { "CSC0010" }, { "CSCb006" } } },
	/* Turtle Beach Tropez Plus (CS4232) */
	{ .id = "CSC7632", .devs = { { "CSC0000" }, { "CSC0010" }, { "PNPb006" } } },
	/* SIC CrystalWave 32 (CS4232) */
	{ .id = "CSCf032", .devs = { { "CSC0000" }, { "CSC0010" }, { "CSC0003" } } },
	/* Netfinity 3000 on-board soundcard */
	{ .id = "CSCe825", .devs = { { "CSC0100" }, { "CSC0110" }, { "CSC010f" } } },
	/* Intel Marlin Spike Motherboard - CS4235 */
	{ .id = "CSC0225", .devs = { { "CSC0000" }, { "CSC0010" }, { "CSC0003" } } },
	/* Intel Marlin Spike Motherboard (#2) - CS4235 */
	{ .id = "CSC0225", .devs = { { "CSC0100" }, { "CSC0110" }, { "CSC0103" } } },
	/* Unknown Intel mainboard - CS4235 */
	{ .id = "CSC0225", .devs = { { "CSC0100" }, { "CSC0110" } } },
	/* Genius Sound Maker 3DJ - CS4237B */
	{ .id = "CSC0437", .devs = { { "CSC0000" }, { "CSC0010" }, { "CSC0003" } } },
	/* Digital PC 5000 Onboard - CS4236B */
	{ .id = "CSC0735", .devs = { { "CSC0000" }, { "CSC0010" } } },
	/* some unknown CS4236B */
	{ .id = "CSC0b35", .devs = { { "CSC0000" }, { "CSC0010" }, { "CSC0003" } } },
	/* Intel PR440FX Onboard sound */
	{ .id = "CSC0b36", .devs = { { "CSC0000" }, { "CSC0010" }, { "CSC0003" } } },
	/* CS4235 on mainboard without MPU */
	{ .id = "CSC1425", .devs = { { "CSC0100" }, { "CSC0110" } } },
	/* Gateway E1000 Onboard CS4236B */
	{ .id = "CSC1335", .devs = { { "CSC0000" }, { "CSC0010" }, { "CSC0003" } } },
	/* HP 6330 Onboard sound */
	{ .id = "CSC1525", .devs = { { "CSC0100" }, { "CSC0110" }, { "CSC0103" } } },
	/* Crystal Computer TidalWave128 */
	{ .id = "CSC1e37", .devs = { { "CSC0000" }, { "CSC0010" }, { "CSC0003" } } },
	/* ACER AW37 - CS4235 */
	{ .id = "CSC4236", .devs = { { "CSC0000" }, { "CSC0010" }, { "CSC0003" } } },
	/* build-in soundcard in EliteGroup P5TX-LA motherboard - CS4237B */
	{ .id = "CSC4237", .devs = { { "CSC0000" }, { "CSC0010" }, { "CSC0003" } } },
	/* Crystal 3D - CS4237B */
	{ .id = "CSC4336", .devs = { { "CSC0000" }, { "CSC0010" }, { "CSC0003" } } },
	/* Typhoon Soundsystem PnP - CS4236B */
	{ .id = "CSC4536", .devs = { { "CSC0000" }, { "CSC0010" }, { "CSC0003" } } },
	/* Crystal CX4235-XQ3 EP - CS4235 */
	{ .id = "CSC4625", .devs = { { "CSC0100" }, { "CSC0110" }, { "CSC0103" } } },
	/* Crystal Semiconductors CS4237B */
	{ .id = "CSC4637", .devs = { { "CSC0000" }, { "CSC0010" }, { "CSC0003" } } },
	/* NewClear 3D - CX4237B-XQ3 */
	{ .id = "CSC4837", .devs = { { "CSC0000" }, { "CSC0010" }, { "CSC0003" } } },
	/* Dell Optiplex GX1 - CS4236B */
	{ .id = "CSC6835", .devs = { { "CSC0000" }, { "CSC0010" }, { "CSC0003" } } },
	/* Dell P410 motherboard - CS4236B */
	{ .id = "CSC6835", .devs = { { "CSC0000" }, { "CSC0010" } } },
	/* Dell Workstation 400 Onboard - CS4236B */
	{ .id = "CSC6836", .devs = { { "CSC0000" }, { "CSC0010" }, { "CSC0003" } } },
	/* Turtle Beach Malibu - CS4237B */
	{ .id = "CSC7537", .devs = { { "CSC0000" }, { "CSC0010" }, { "CSC0003" } } },
	/* CS4235 - onboard */
	{ .id = "CSC8025", .devs = { { "CSC0100" }, { "CSC0110" }, { "CSC0103" } } },
	/* IBM Aptiva 2137 E24 Onboard - CS4237B */
	{ .id = "CSC8037", .devs = { { "CSC0000" }, { "CSC0010" }, { "CSC0003" } } },
	/* IBM IntelliStation M Pro motherboard */
	{ .id = "CSCc835", .devs = { { "CSC0000" }, { "CSC0010" } } },
	/* Guillemot MaxiSound 16 PnP - CS4236B */
	{ .id = "CSC9836", .devs = { { "CSC0000" }, { "CSC0010" }, { "CSC0003" } } },
	/* Gallant SC-70P */
	{ .id = "CSC9837", .devs = { { "CSC0000" }, { "CSC0010" }, { "CSC0003" } } },
	/* Techmakers MF-4236PW */
	{ .id = "CSCa736", .devs = { { "CSC0000" }, { "CSC0010" }, { "CSC0003" } } },
	/* TerraTec AudioSystem EWS64XL - CS4236B */
	{ .id = "CSCa836", .devs = { { "CSCa800" }, { "CSCa810" }, { "CSCa803" } } },
	/* TerraTec AudioSystem EWS64XL - CS4236B */
	{ .id = "CSCa836", .devs = { { "CSCa800" }, { "CSCa810" } } },
	/* ACER AW37/Pro - CS4235 */
	{ .id = "CSCd925", .devs = { { "CSC0000" }, { "CSC0010" }, { "CSC0003" } } },
	/* ACER AW35/Pro - CS4237B */
	{ .id = "CSCd937", .devs = { { "CSC0000" }, { "CSC0010" }, { "CSC0003" } } },
	/* CS4235 without MPU401 */
	{ .id = "CSCe825", .devs = { { "CSC0100" }, { "CSC0110" } } },
	/* Unknown SiS530 - CS4235 */
	{ .id = "CSC4825", .devs = { { "CSC0100" }, { "CSC0110" } } },
	/* IBM IntelliStation M Pro 6898 11U - CS4236B */
	{ .id = "CSCe835", .devs = { { "CSC0000" }, { "CSC0010" } } },
	/* IBM PC 300PL Onboard - CS4236B */
	{ .id = "CSCe836", .devs = { { "CSC0000" }, { "CSC0010" } } },
	/* Some noname CS4236 based card */
	{ .id = "CSCe936", .devs = { { "CSC0000" }, { "CSC0010" }, { "CSC0003" } } },
	/* CS4236B */
	{ .id = "CSCf235", .devs = { { "CSC0000" }, { "CSC0010" }, { "CSC0003" } } },
	/* CS4236B */
	{ .id = "CSCf238", .devs = { { "CSC0000" }, { "CSC0010" }, { "CSC0003" } } },
	/* --- */
	{ .id = "" }	/* end */
};

MODULE_DEVICE_TABLE(pnp_card, snd_cs423x_pnpids);

/* WSS initialization */
static int snd_cs423x_pnp_init_wss(int dev, struct pnp_dev *pdev)
{
	if (pnp_activate_dev(pdev) < 0) {
		printk(KERN_ERR IDENT " WSS PnP configure failed for WSS (out of resources?)\n");
		return -EBUSY;
	}
	port[dev] = pnp_port_start(pdev, 0);
	if (fm_port[dev] > 0)
		fm_port[dev] = pnp_port_start(pdev, 1);
	sb_port[dev] = pnp_port_start(pdev, 2);
	irq[dev] = pnp_irq(pdev, 0);
	dma1[dev] = pnp_dma(pdev, 0);
	dma2[dev] = pnp_dma(pdev, 1) == 4 ? -1 : (int)pnp_dma(pdev, 1);
	snd_printdd("isapnp WSS: wss port=0x%lx, fm port=0x%lx, sb port=0x%lx\n",
			port[dev], fm_port[dev], sb_port[dev]);
	snd_printdd("isapnp WSS: irq=%i, dma1=%i, dma2=%i\n",
			irq[dev], dma1[dev], dma2[dev]);
	return 0;
}

/* CTRL initialization */
static int snd_cs423x_pnp_init_ctrl(int dev, struct pnp_dev *pdev)
{
	if (pnp_activate_dev(pdev) < 0) {
		printk(KERN_ERR IDENT " CTRL PnP configure failed for WSS (out of resources?)\n");
		return -EBUSY;
	}
	cport[dev] = pnp_port_start(pdev, 0);
	snd_printdd("isapnp CTRL: control port=0x%lx\n", cport[dev]);
	return 0;
}

/* MPU initialization */
static int snd_cs423x_pnp_init_mpu(int dev, struct pnp_dev *pdev)
{
	if (pnp_activate_dev(pdev) < 0) {
		printk(KERN_ERR IDENT " MPU401 PnP configure failed for WSS (out of resources?)\n");
		mpu_port[dev] = SNDRV_AUTO_PORT;
		mpu_irq[dev] = SNDRV_AUTO_IRQ;
	} else {
		mpu_port[dev] = pnp_port_start(pdev, 0);
		if (mpu_irq[dev] >= 0 &&
		    pnp_irq_valid(pdev, 0) &&
		    pnp_irq(pdev, 0) != (resource_size_t)-1) {
			mpu_irq[dev] = pnp_irq(pdev, 0);
		} else {
			mpu_irq[dev] = -1;	/* disable interrupt */
		}
	}
	snd_printdd("isapnp MPU: port=0x%lx, irq=%i\n", mpu_port[dev], mpu_irq[dev]);
	return 0;
}

static int snd_card_cs423x_pnp(int dev, struct snd_card_cs4236 *acard,
			       struct pnp_dev *pdev,
			       struct pnp_dev *cdev)
{
	acard->wss = pdev;
	if (snd_cs423x_pnp_init_wss(dev, acard->wss) < 0)
		return -EBUSY;
	if (cdev)
		cport[dev] = pnp_port_start(cdev, 0);
	else
		cport[dev] = -1;
	return 0;
}

static int snd_card_cs423x_pnpc(int dev, struct snd_card_cs4236 *acard,
				struct pnp_card_link *card,
				const struct pnp_card_device_id *id)
{
	acard->wss = pnp_request_card_device(card, id->devs[0].id, NULL);
	if (acard->wss == NULL)
		return -EBUSY;
	acard->ctrl = pnp_request_card_device(card, id->devs[1].id, NULL);
	if (acard->ctrl == NULL)
		return -EBUSY;
	if (id->devs[2].id[0]) {
		acard->mpu = pnp_request_card_device(card, id->devs[2].id, NULL);
		if (acard->mpu == NULL)
			return -EBUSY;
	}

	/* WSS initialization */
	if (snd_cs423x_pnp_init_wss(dev, acard->wss) < 0)
		return -EBUSY;

	/* CTRL initialization */
	if (acard->ctrl && cport[dev] > 0) {
		if (snd_cs423x_pnp_init_ctrl(dev, acard->ctrl) < 0)
			return -EBUSY;
	}
	/* MPU initialization */
	if (acard->mpu && mpu_port[dev] > 0) {
		if (snd_cs423x_pnp_init_mpu(dev, acard->mpu) < 0)
			return -EBUSY;
	}
	return 0;
}
#endif /* CONFIG_PNP */

#ifdef CONFIG_PNP
#define is_isapnp_selected(dev)		isapnp[dev]
#else
#define is_isapnp_selected(dev)		0
#endif

static void snd_card_cs4236_free(struct snd_card *card)
{
	struct snd_card_cs4236 *acard = card->private_data;

	release_and_free_resource(acard->res_sb_port);
}

static int snd_cs423x_card_new(struct device *pdev, int dev,
			       struct snd_card **cardp)
{
	struct snd_card *card;
	int err;

	err = snd_card_new(pdev, index[dev], id[dev], THIS_MODULE,
			   sizeof(struct snd_card_cs4236), &card);
	if (err < 0)
		return err;
	card->private_free = snd_card_cs4236_free;
	*cardp = card;
	return 0;
}

static int snd_cs423x_probe(struct snd_card *card, int dev)
{
	struct snd_card_cs4236 *acard;
	struct snd_wss *chip;
	struct snd_opl3 *opl3;
	int err;

	acard = card->private_data;
	if (sb_port[dev] > 0 && sb_port[dev] != SNDRV_AUTO_PORT)
		if ((acard->res_sb_port = request_region(sb_port[dev], 16, IDENT " SB")) == NULL) {
			printk(KERN_ERR IDENT ": unable to register SB port at 0x%lx\n", sb_port[dev]);
			return -EBUSY;
		}

	err = snd_cs4236_create(card, port[dev], cport[dev],
			     irq[dev],
			     dma1[dev], dma2[dev],
			     WSS_HW_DETECT3, 0, &chip);
	if (err < 0)
		return err;

	acard->chip = chip;
	if (chip->hardware & WSS_HW_CS4236B_MASK) {

		err = snd_cs4236_pcm(chip, 0);
		if (err < 0)
			return err;

		err = snd_cs4236_mixer(chip);
		if (err < 0)
			return err;
	} else {
		err = snd_wss_pcm(chip, 0);
		if (err < 0)
			return err;

		err = snd_wss_mixer(chip);
		if (err < 0)
			return err;
	}
	strscpy(card->driver, chip->pcm->name, sizeof(card->driver));
	strscpy(card->shortname, chip->pcm->name, sizeof(card->shortname));
	if (dma2[dev] < 0)
		snprintf(card->longname, sizeof(card->longname),
			 "%s at 0x%lx, irq %i, dma %i",
			 chip->pcm->name, chip->port, irq[dev], dma1[dev]);
	else
		snprintf(card->longname, sizeof(card->longname),
			 "%s at 0x%lx, irq %i, dma %i&%d",
			 chip->pcm->name, chip->port, irq[dev], dma1[dev],
			 dma2[dev]);

	err = snd_wss_timer(chip, 0);
	if (err < 0)
		return err;

	if (fm_port[dev] > 0 && fm_port[dev] != SNDRV_AUTO_PORT) {
		if (snd_opl3_create(card,
				    fm_port[dev], fm_port[dev] + 2,
				    OPL3_HW_OPL3_CS, 0, &opl3) < 0) {
			printk(KERN_WARNING IDENT ": OPL3 not detected\n");
		} else {
			if ((err = snd_opl3_hwdep_new(opl3, 0, 1, NULL)) < 0)
				return err;
		}
	}

	if (mpu_port[dev] > 0 && mpu_port[dev] != SNDRV_AUTO_PORT) {
		if (mpu_irq[dev] == SNDRV_AUTO_IRQ)
			mpu_irq[dev] = -1;
		if (snd_mpu401_uart_new(card, 0, MPU401_HW_CS4232,
					mpu_port[dev], 0,
					mpu_irq[dev], NULL) < 0)
			printk(KERN_WARNING IDENT ": MPU401 not detected\n");
	}

	return snd_card_register(card);
}

static int snd_cs423x_isa_match(struct device *pdev,
				unsigned int dev)
{
	if (!enable[dev] || is_isapnp_selected(dev))
		return 0;

	if (port[dev] == SNDRV_AUTO_PORT) {
		dev_err(pdev, "please specify port\n");
		return 0;
	}
	if (cport[dev] == SNDRV_AUTO_PORT) {
		dev_err(pdev, "please specify cport\n");
		return 0;
	}
	if (irq[dev] == SNDRV_AUTO_IRQ) {
		dev_err(pdev, "please specify irq\n");
		return 0;
	}
	if (dma1[dev] == SNDRV_AUTO_DMA) {
		dev_err(pdev, "please specify dma1\n");
		return 0;
	}
	return 1;
}

static int snd_cs423x_isa_probe(struct device *pdev,
				unsigned int dev)
{
	struct snd_card *card;
	int err;

	err = snd_cs423x_card_new(pdev, dev, &card);
	if (err < 0)
		return err;
	if ((err = snd_cs423x_probe(card, dev)) < 0) {
		snd_card_free(card);
		return err;
	}

	dev_set_drvdata(pdev, card);
	return 0;
}

static void snd_cs423x_isa_remove(struct device *pdev,
				 unsigned int dev)
{
	snd_card_free(dev_get_drvdata(pdev));
}

#ifdef CONFIG_PM
static int snd_cs423x_suspend(struct snd_card *card)
{
	struct snd_card_cs4236 *acard = card->private_data;
	snd_power_change_state(card, SNDRV_CTL_POWER_D3hot);
	acard->chip->suspend(acard->chip);
	return 0;
}

static int snd_cs423x_resume(struct snd_card *card)
{
	struct snd_card_cs4236 *acard = card->private_data;
	acard->chip->resume(acard->chip);
	snd_power_change_state(card, SNDRV_CTL_POWER_D0);
	return 0;
}

static int snd_cs423x_isa_suspend(struct device *dev, unsigned int n,
				  pm_message_t state)
{
	return snd_cs423x_suspend(dev_get_drvdata(dev));
}

static int snd_cs423x_isa_resume(struct device *dev, unsigned int n)
{
	return snd_cs423x_resume(dev_get_drvdata(dev));
}
#endif

static struct isa_driver cs423x_isa_driver = {
	.match		= snd_cs423x_isa_match,
	.probe		= snd_cs423x_isa_probe,
	.remove		= snd_cs423x_isa_remove,
#ifdef CONFIG_PM
	.suspend	= snd_cs423x_isa_suspend,
	.resume		= snd_cs423x_isa_resume,
#endif
	.driver		= {
		.name	= DEV_NAME
	},
};


#ifdef CONFIG_PNP
static int snd_cs423x_pnpbios_detect(struct pnp_dev *pdev,
				     const struct pnp_device_id *id)
{
	static int dev;
	int err;
	struct snd_card *card;
	struct pnp_dev *cdev;
	char cid[PNP_ID_LEN];

	if (pnp_device_is_isapnp(pdev))
		return -ENOENT;	/* we have another procedure - card */
	for (; dev < SNDRV_CARDS; dev++) {
		if (enable[dev] && isapnp[dev])
			break;
	}
	if (dev >= SNDRV_CARDS)
		return -ENODEV;

	/* prepare second id */
	strcpy(cid, pdev->id[0].id);
	cid[5] = '1';
	cdev = NULL;
	list_for_each_entry(cdev, &(pdev->protocol->devices), protocol_list) {
		if (!strcmp(cdev->id[0].id, cid))
			break;
	}
	err = snd_cs423x_card_new(&pdev->dev, dev, &card);
	if (err < 0)
		return err;
	err = snd_card_cs423x_pnp(dev, card->private_data, pdev, cdev);
	if (err < 0) {
		printk(KERN_ERR "PnP BIOS detection failed for " IDENT "\n");
		snd_card_free(card);
		return err;
	}
	if ((err = snd_cs423x_probe(card, dev)) < 0) {
		snd_card_free(card);
		return err;
	}
	pnp_set_drvdata(pdev, card);
	dev++;
	return 0;
}

static void snd_cs423x_pnp_remove(struct pnp_dev *pdev)
{
	snd_card_free(pnp_get_drvdata(pdev));
}

#ifdef CONFIG_PM
static int snd_cs423x_pnp_suspend(struct pnp_dev *pdev, pm_message_t state)
{
	return snd_cs423x_suspend(pnp_get_drvdata(pdev));
}

static int snd_cs423x_pnp_resume(struct pnp_dev *pdev)
{
	return snd_cs423x_resume(pnp_get_drvdata(pdev));
}
#endif

static struct pnp_driver cs423x_pnp_driver = {
	.name = "cs423x-pnpbios",
	.id_table = snd_cs423x_pnpbiosids,
	.probe = snd_cs423x_pnpbios_detect,
	.remove = snd_cs423x_pnp_remove,
#ifdef CONFIG_PM
	.suspend	= snd_cs423x_pnp_suspend,
	.resume		= snd_cs423x_pnp_resume,
#endif
};

static int snd_cs423x_pnpc_detect(struct pnp_card_link *pcard,
				  const struct pnp_card_device_id *pid)
{
	static int dev;
	struct snd_card *card;
	int res;

	for ( ; dev < SNDRV_CARDS; dev++) {
		if (enable[dev] && isapnp[dev])
			break;
	}
	if (dev >= SNDRV_CARDS)
		return -ENODEV;

	res = snd_cs423x_card_new(&pcard->card->dev, dev, &card);
	if (res < 0)
		return res;
	if ((res = snd_card_cs423x_pnpc(dev, card->private_data, pcard, pid)) < 0) {
		printk(KERN_ERR "isapnp detection failed and probing for " IDENT
		       " is not supported\n");
		snd_card_free(card);
		return res;
	}
	if ((res = snd_cs423x_probe(card, dev)) < 0) {
		snd_card_free(card);
		return res;
	}
	pnp_set_card_drvdata(pcard, card);
	dev++;
	return 0;
}

static void snd_cs423x_pnpc_remove(struct pnp_card_link *pcard)
{
	snd_card_free(pnp_get_card_drvdata(pcard));
	pnp_set_card_drvdata(pcard, NULL);
}

#ifdef CONFIG_PM
static int snd_cs423x_pnpc_suspend(struct pnp_card_link *pcard, pm_message_t state)
{
	return snd_cs423x_suspend(pnp_get_card_drvdata(pcard));
}

static int snd_cs423x_pnpc_resume(struct pnp_card_link *pcard)
{
	return snd_cs423x_resume(pnp_get_card_drvdata(pcard));
}
#endif

static struct pnp_card_driver cs423x_pnpc_driver = {
	.flags = PNP_DRIVER_RES_DISABLE,
	.name = CS423X_ISAPNP_DRIVER,
	.id_table = snd_cs423x_pnpids,
	.probe = snd_cs423x_pnpc_detect,
	.remove = snd_cs423x_pnpc_remove,
#ifdef CONFIG_PM
	.suspend	= snd_cs423x_pnpc_suspend,
	.resume		= snd_cs423x_pnpc_resume,
#endif
};
#endif /* CONFIG_PNP */

static int __init alsa_card_cs423x_init(void)
{
	int err;

	err = isa_register_driver(&cs423x_isa_driver, SNDRV_CARDS);
#ifdef CONFIG_PNP
	if (!err)
		isa_registered = 1;
	err = pnp_register_driver(&cs423x_pnp_driver);
	if (!err)
		pnp_registered = 1;
	err = pnp_register_card_driver(&cs423x_pnpc_driver);
	if (!err)
		pnpc_registered = 1;
	if (pnp_registered)
		err = 0;
	if (isa_registered)
		err = 0;
#endif
	return err;
}

static void __exit alsa_card_cs423x_exit(void)
{
#ifdef CONFIG_PNP
	if (pnpc_registered)
		pnp_unregister_card_driver(&cs423x_pnpc_driver);
	if (pnp_registered)
		pnp_unregister_driver(&cs423x_pnp_driver);
	if (isa_registered)
#endif
		isa_unregister_driver(&cs423x_isa_driver);
}

module_init(alsa_card_cs423x_init)
module_exit(alsa_card_cs423x_exit)
