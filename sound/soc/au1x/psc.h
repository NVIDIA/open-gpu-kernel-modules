/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Alchemy ALSA ASoC audio support.
 *
 * (c) 2007-2011 MSC Vertriebsges.m.b.H.,
 *	Manuel Lauss <manuel.lauss@gmail.com>
 */

#ifndef _AU1X_PCM_H
#define _AU1X_PCM_H

struct au1xpsc_audio_data {
	void __iomem *mmio;

	unsigned long cfg;
	unsigned long rate;

	struct snd_soc_dai_driver dai_drv;

	unsigned long pm[2];
	struct mutex lock;
	int dmaids[2];
};

/* easy access macros */
#define PSC_CTRL(x)	((x)->mmio + PSC_CTRL_OFFSET)
#define PSC_SEL(x)	((x)->mmio + PSC_SEL_OFFSET)
#define I2S_STAT(x)	((x)->mmio + PSC_I2SSTAT_OFFSET)
#define I2S_CFG(x)	((x)->mmio + PSC_I2SCFG_OFFSET)
#define I2S_PCR(x)	((x)->mmio + PSC_I2SPCR_OFFSET)
#define AC97_CFG(x)	((x)->mmio + PSC_AC97CFG_OFFSET)
#define AC97_CDC(x)	((x)->mmio + PSC_AC97CDC_OFFSET)
#define AC97_EVNT(x)	((x)->mmio + PSC_AC97EVNT_OFFSET)
#define AC97_PCR(x)	((x)->mmio + PSC_AC97PCR_OFFSET)
#define AC97_RST(x)	((x)->mmio + PSC_AC97RST_OFFSET)
#define AC97_STAT(x)	((x)->mmio + PSC_AC97STAT_OFFSET)

#endif
