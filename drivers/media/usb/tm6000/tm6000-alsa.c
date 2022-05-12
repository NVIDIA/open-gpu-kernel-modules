// SPDX-License-Identifier: GPL-2.0
// Support for audio capture for tm5600/6000/6010
// Copyright (c) 2007-2008 Mauro Carvalho Chehab <mchehab@kernel.org>
//
// Based on cx88-alsa.c

#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/usb.h>
#include <linux/slab.h>

#include <linux/delay.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/control.h>
#include <sound/initval.h>


#include "tm6000.h"
#include "tm6000-regs.h"

#undef dprintk

#define dprintk(level, fmt, arg...) do {				   \
	if (debug >= level)						   \
		printk(KERN_INFO "%s/1: " fmt, chip->core->name , ## arg); \
	} while (0)

/****************************************************************************
			Module global static vars
 ****************************************************************************/

static int index[SNDRV_CARDS] = SNDRV_DEFAULT_IDX;	/* Index 0-MAX */

static bool enable[SNDRV_CARDS] = SNDRV_DEFAULT_ENABLE_PNP;

module_param_array(enable, bool, NULL, 0444);
MODULE_PARM_DESC(enable, "Enable tm6000x soundcard. default enabled.");

module_param_array(index, int, NULL, 0444);
MODULE_PARM_DESC(index, "Index value for tm6000x capture interface(s).");


/****************************************************************************
				Module macros
 ****************************************************************************/

MODULE_DESCRIPTION("ALSA driver module for tm5600/tm6000/tm6010 based TV cards");
MODULE_AUTHOR("Mauro Carvalho Chehab");
MODULE_LICENSE("GPL v2");
static unsigned int debug;
module_param(debug, int, 0644);
MODULE_PARM_DESC(debug, "enable debug messages");

/****************************************************************************
			Module specific functions
 ****************************************************************************/

/*
 * BOARD Specific: Sets audio DMA
 */

static int _tm6000_start_audio_dma(struct snd_tm6000_card *chip)
{
	struct tm6000_core *core = chip->core;

	dprintk(1, "Starting audio DMA\n");

	/* Enables audio */
	tm6000_set_reg_mask(core, TM6010_REQ07_RCC_ACTIVE_IF, 0x40, 0x40);

	tm6000_set_audio_bitrate(core, 48000);

	return 0;
}

/*
 * BOARD Specific: Resets audio DMA
 */
static int _tm6000_stop_audio_dma(struct snd_tm6000_card *chip)
{
	struct tm6000_core *core = chip->core;

	dprintk(1, "Stopping audio DMA\n");

	/* Disables audio */
	tm6000_set_reg_mask(core, TM6010_REQ07_RCC_ACTIVE_IF, 0x00, 0x40);

	return 0;
}

/****************************************************************************
				ALSA PCM Interface
 ****************************************************************************/

/*
 * Digital hardware definition
 */
#define DEFAULT_FIFO_SIZE	4096

static const struct snd_pcm_hardware snd_tm6000_digital_hw = {
	.info = SNDRV_PCM_INFO_BATCH |
		SNDRV_PCM_INFO_MMAP |
		SNDRV_PCM_INFO_INTERLEAVED |
		SNDRV_PCM_INFO_BLOCK_TRANSFER |
		SNDRV_PCM_INFO_MMAP_VALID,
	.formats = SNDRV_PCM_FMTBIT_S16_LE,

	.rates = SNDRV_PCM_RATE_CONTINUOUS | SNDRV_PCM_RATE_KNOT,
	.rate_min = 48000,
	.rate_max = 48000,
	.channels_min = 2,
	.channels_max = 2,
	.period_bytes_min = 64,
	.period_bytes_max = 12544,
	.periods_min = 2,
	.periods_max = 98,
	.buffer_bytes_max = 62720 * 8,
};

/*
 * audio pcm capture open callback
 */
static int snd_tm6000_pcm_open(struct snd_pcm_substream *substream)
{
	struct snd_tm6000_card *chip = snd_pcm_substream_chip(substream);
	struct snd_pcm_runtime *runtime = substream->runtime;
	int err;

	err = snd_pcm_hw_constraint_pow2(runtime, 0,
					 SNDRV_PCM_HW_PARAM_PERIODS);
	if (err < 0)
		goto _error;

	chip->substream = substream;

	runtime->hw = snd_tm6000_digital_hw;
	snd_pcm_hw_constraint_integer(runtime, SNDRV_PCM_HW_PARAM_PERIODS);

	return 0;
_error:
	dprintk(1, "Error opening PCM!\n");
	return err;
}

/*
 * audio close callback
 */
static int snd_tm6000_close(struct snd_pcm_substream *substream)
{
	struct snd_tm6000_card *chip = snd_pcm_substream_chip(substream);
	struct tm6000_core *core = chip->core;

	if (atomic_read(&core->stream_started) > 0) {
		atomic_set(&core->stream_started, 0);
		schedule_work(&core->wq_trigger);
	}

	return 0;
}

static int tm6000_fillbuf(struct tm6000_core *core, char *buf, int size)
{
	struct snd_tm6000_card *chip = core->adev;
	struct snd_pcm_substream *substream = chip->substream;
	struct snd_pcm_runtime *runtime;
	int period_elapsed = 0;
	unsigned int stride, buf_pos;
	int length;

	if (atomic_read(&core->stream_started) == 0)
		return 0;

	if (!size || !substream) {
		dprintk(1, "substream was NULL\n");
		return -EINVAL;
	}

	runtime = substream->runtime;
	if (!runtime || !runtime->dma_area) {
		dprintk(1, "runtime was NULL\n");
		return -EINVAL;
	}

	buf_pos = chip->buf_pos;
	stride = runtime->frame_bits >> 3;

	if (stride == 0) {
		dprintk(1, "stride is zero\n");
		return -EINVAL;
	}

	length = size / stride;
	if (length == 0) {
		dprintk(1, "%s: length was zero\n", __func__);
		return -EINVAL;
	}

	dprintk(1, "Copying %d bytes at %p[%d] - buf size=%d x %d\n", size,
		runtime->dma_area, buf_pos,
		(unsigned int)runtime->buffer_size, stride);

	if (buf_pos + length >= runtime->buffer_size) {
		unsigned int cnt = runtime->buffer_size - buf_pos;
		memcpy(runtime->dma_area + buf_pos * stride, buf, cnt * stride);
		memcpy(runtime->dma_area, buf + cnt * stride,
			length * stride - cnt * stride);
	} else
		memcpy(runtime->dma_area + buf_pos * stride, buf,
			length * stride);

	snd_pcm_stream_lock(substream);

	chip->buf_pos += length;
	if (chip->buf_pos >= runtime->buffer_size)
		chip->buf_pos -= runtime->buffer_size;

	chip->period_pos += length;
	if (chip->period_pos >= runtime->period_size) {
		chip->period_pos -= runtime->period_size;
		period_elapsed = 1;
	}

	snd_pcm_stream_unlock(substream);

	if (period_elapsed)
		snd_pcm_period_elapsed(substream);

	return 0;
}

/*
 * prepare callback
 */
static int snd_tm6000_prepare(struct snd_pcm_substream *substream)
{
	struct snd_tm6000_card *chip = snd_pcm_substream_chip(substream);

	chip->buf_pos = 0;
	chip->period_pos = 0;

	return 0;
}


/*
 * trigger callback
 */
static void audio_trigger(struct work_struct *work)
{
	struct tm6000_core *core = container_of(work, struct tm6000_core,
						wq_trigger);
	struct snd_tm6000_card *chip = core->adev;

	if (atomic_read(&core->stream_started)) {
		dprintk(1, "starting capture");
		_tm6000_start_audio_dma(chip);
	} else {
		dprintk(1, "stopping capture");
		_tm6000_stop_audio_dma(chip);
	}
}

static int snd_tm6000_card_trigger(struct snd_pcm_substream *substream, int cmd)
{
	struct snd_tm6000_card *chip = snd_pcm_substream_chip(substream);
	struct tm6000_core *core = chip->core;
	int err = 0;

	switch (cmd) {
	case SNDRV_PCM_TRIGGER_PAUSE_RELEASE:
	case SNDRV_PCM_TRIGGER_RESUME:
	case SNDRV_PCM_TRIGGER_START:
		atomic_set(&core->stream_started, 1);
		break;
	case SNDRV_PCM_TRIGGER_PAUSE_PUSH:
	case SNDRV_PCM_TRIGGER_SUSPEND:
	case SNDRV_PCM_TRIGGER_STOP:
		atomic_set(&core->stream_started, 0);
		break;
	default:
		err = -EINVAL;
		break;
	}
	schedule_work(&core->wq_trigger);

	return err;
}
/*
 * pointer callback
 */
static snd_pcm_uframes_t snd_tm6000_pointer(struct snd_pcm_substream *substream)
{
	struct snd_tm6000_card *chip = snd_pcm_substream_chip(substream);

	return chip->buf_pos;
}

/*
 * operators
 */
static const struct snd_pcm_ops snd_tm6000_pcm_ops = {
	.open = snd_tm6000_pcm_open,
	.close = snd_tm6000_close,
	.prepare = snd_tm6000_prepare,
	.trigger = snd_tm6000_card_trigger,
	.pointer = snd_tm6000_pointer,
};

/*
 * create a PCM device
 */

/* FIXME: Control interface - How to control volume/mute? */

/****************************************************************************
			Basic Flow for Sound Devices
 ****************************************************************************/

/*
 * Alsa Constructor - Component probe
 */
static int tm6000_audio_init(struct tm6000_core *dev)
{
	struct snd_card		*card;
	struct snd_tm6000_card	*chip;
	int			rc;
	static int		devnr;
	char			component[14];
	struct snd_pcm		*pcm;

	if (!dev)
		return 0;

	if (devnr >= SNDRV_CARDS)
		return -ENODEV;

	if (!enable[devnr])
		return -ENOENT;

	rc = snd_card_new(&dev->udev->dev, index[devnr], "tm6000",
			  THIS_MODULE, 0, &card);
	if (rc < 0) {
		snd_printk(KERN_ERR "cannot create card instance %d\n", devnr);
		return rc;
	}
	strscpy(card->driver, "tm6000-alsa", sizeof(card->driver));
	strscpy(card->shortname, "TM5600/60x0", sizeof(card->shortname));
	sprintf(card->longname, "TM5600/60x0 Audio at bus %d device %d",
		dev->udev->bus->busnum, dev->udev->devnum);

	sprintf(component, "USB%04x:%04x",
		le16_to_cpu(dev->udev->descriptor.idVendor),
		le16_to_cpu(dev->udev->descriptor.idProduct));
	snd_component_add(card, component);

	chip = kzalloc(sizeof(struct snd_tm6000_card), GFP_KERNEL);
	if (!chip) {
		rc = -ENOMEM;
		goto error;
	}

	chip->core = dev;
	chip->card = card;
	dev->adev = chip;
	spin_lock_init(&chip->reg_lock);

	rc = snd_pcm_new(card, "TM6000 Audio", 0, 0, 1, &pcm);
	if (rc < 0)
		goto error_chip;

	pcm->info_flags = 0;
	pcm->private_data = chip;
	strscpy(pcm->name, "Trident TM5600/60x0", sizeof(pcm->name));

	snd_pcm_set_ops(pcm, SNDRV_PCM_STREAM_CAPTURE, &snd_tm6000_pcm_ops);
	snd_pcm_set_managed_buffer_all(pcm, SNDRV_DMA_TYPE_VMALLOC, NULL, 0, 0);

	INIT_WORK(&dev->wq_trigger, audio_trigger);
	rc = snd_card_register(card);
	if (rc < 0)
		goto error_chip;

	dprintk(1, "Registered audio driver for %s\n", card->longname);

	return 0;

error_chip:
	kfree(chip);
	dev->adev = NULL;
error:
	snd_card_free(card);
	return rc;
}

static int tm6000_audio_fini(struct tm6000_core *dev)
{
	struct snd_tm6000_card *chip;

	if (!dev)
		return 0;
	chip = dev->adev;

	if (!chip)
		return 0;

	if (!chip->card)
		return 0;

	snd_card_free(chip->card);
	chip->card = NULL;
	kfree(chip);
	dev->adev = NULL;

	return 0;
}

static struct tm6000_ops audio_ops = {
	.type	= TM6000_AUDIO,
	.name	= "TM6000 Audio Extension",
	.init	= tm6000_audio_init,
	.fini	= tm6000_audio_fini,
	.fillbuf = tm6000_fillbuf,
};

static int __init tm6000_alsa_register(void)
{
	return tm6000_register_extension(&audio_ops);
}

static void __exit tm6000_alsa_unregister(void)
{
	tm6000_unregister_extension(&audio_ops);
}

module_init(tm6000_alsa_register);
module_exit(tm6000_alsa_unregister);
