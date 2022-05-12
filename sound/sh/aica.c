// SPDX-License-Identifier: GPL-2.0-only
/*
*
* Copyright Adrian McMenamin 2005, 2006, 2007
* <adrian@mcmen.demon.co.uk>
* Requires firmware (BSD licenced) available from:
* http://linuxdc.cvs.sourceforge.net/linuxdc/linux-sh-dc/sound/oss/aica/firmware/
* or the maintainer
*/

#include <linux/init.h>
#include <linux/jiffies.h>
#include <linux/slab.h>
#include <linux/time.h>
#include <linux/wait.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/firmware.h>
#include <linux/timer.h>
#include <linux/delay.h>
#include <linux/workqueue.h>
#include <linux/io.h>
#include <sound/core.h>
#include <sound/control.h>
#include <sound/pcm.h>
#include <sound/initval.h>
#include <sound/info.h>
#include <asm/dma.h>
#include <mach/sysasic.h>
#include "aica.h"

MODULE_AUTHOR("Adrian McMenamin <adrian@mcmen.demon.co.uk>");
MODULE_DESCRIPTION("Dreamcast AICA sound (pcm) driver");
MODULE_LICENSE("GPL");
MODULE_FIRMWARE("aica_firmware.bin");

/* module parameters */
#define CARD_NAME "AICA"
static int index = -1;
static char *id;
static bool enable = 1;
module_param(index, int, 0444);
MODULE_PARM_DESC(index, "Index value for " CARD_NAME " soundcard.");
module_param(id, charp, 0444);
MODULE_PARM_DESC(id, "ID string for " CARD_NAME " soundcard.");
module_param(enable, bool, 0644);
MODULE_PARM_DESC(enable, "Enable " CARD_NAME " soundcard.");

/* Simple platform device */
static struct platform_device *pd;
static struct resource aica_memory_space[2] = {
	{
	 .name = "AICA ARM CONTROL",
	 .start = ARM_RESET_REGISTER,
	 .flags = IORESOURCE_MEM,
	 .end = ARM_RESET_REGISTER + 3,
	 },
	{
	 .name = "AICA Sound RAM",
	 .start = SPU_MEMORY_BASE,
	 .flags = IORESOURCE_MEM,
	 .end = SPU_MEMORY_BASE + 0x200000 - 1,
	 },
};

/* SPU specific functions */
/* spu_write_wait - wait for G2-SH FIFO to clear */
static void spu_write_wait(void)
{
	int time_count;
	time_count = 0;
	while (1) {
		if (!(readl(G2_FIFO) & 0x11))
			break;
		/* To ensure hardware failure doesn't wedge kernel */
		time_count++;
		if (time_count > 0x10000) {
			snd_printk
			    ("WARNING: G2 FIFO appears to be blocked.\n");
			break;
		}
	}
}

/* spu_memset - write to memory in SPU address space */
static void spu_memset(u32 toi, u32 what, int length)
{
	int i;
	unsigned long flags;
	if (snd_BUG_ON(length % 4))
		return;
	for (i = 0; i < length; i++) {
		if (!(i % 8))
			spu_write_wait();
		local_irq_save(flags);
		writel(what, toi + SPU_MEMORY_BASE);
		local_irq_restore(flags);
		toi++;
	}
}

/* spu_memload - write to SPU address space */
static void spu_memload(u32 toi, const void *from, int length)
{
	unsigned long flags;
	const u32 *froml = from;
	u32 __iomem *to = (u32 __iomem *) (SPU_MEMORY_BASE + toi);
	int i;
	u32 val;
	length = DIV_ROUND_UP(length, 4);
	spu_write_wait();
	for (i = 0; i < length; i++) {
		if (!(i % 8))
			spu_write_wait();
		val = *froml;
		local_irq_save(flags);
		writel(val, to);
		local_irq_restore(flags);
		froml++;
		to++;
	}
}

/* spu_disable - set spu registers to stop sound output */
static void spu_disable(void)
{
	int i;
	unsigned long flags;
	u32 regval;
	spu_write_wait();
	regval = readl(ARM_RESET_REGISTER);
	regval |= 1;
	spu_write_wait();
	local_irq_save(flags);
	writel(regval, ARM_RESET_REGISTER);
	local_irq_restore(flags);
	for (i = 0; i < 64; i++) {
		spu_write_wait();
		regval = readl(SPU_REGISTER_BASE + (i * 0x80));
		regval = (regval & ~0x4000) | 0x8000;
		spu_write_wait();
		local_irq_save(flags);
		writel(regval, SPU_REGISTER_BASE + (i * 0x80));
		local_irq_restore(flags);
	}
}

/* spu_enable - set spu registers to enable sound output */
static void spu_enable(void)
{
	unsigned long flags;
	u32 regval = readl(ARM_RESET_REGISTER);
	regval &= ~1;
	spu_write_wait();
	local_irq_save(flags);
	writel(regval, ARM_RESET_REGISTER);
	local_irq_restore(flags);
}

/* 
 * Halt the sound processor, clear the memory,
 * load some default ARM7 code, and then restart ARM7
*/
static void spu_reset(void)
{
	unsigned long flags;
	spu_disable();
	spu_memset(0, 0, 0x200000 / 4);
	/* Put ARM7 in endless loop */
	local_irq_save(flags);
	__raw_writel(0xea000002, SPU_MEMORY_BASE);
	local_irq_restore(flags);
	spu_enable();
}

/* aica_chn_start - write to spu to start playback */
static void aica_chn_start(void)
{
	unsigned long flags;
	spu_write_wait();
	local_irq_save(flags);
	writel(AICA_CMD_KICK | AICA_CMD_START, (u32 *) AICA_CONTROL_POINT);
	local_irq_restore(flags);
}

/* aica_chn_halt - write to spu to halt playback */
static void aica_chn_halt(void)
{
	unsigned long flags;
	spu_write_wait();
	local_irq_save(flags);
	writel(AICA_CMD_KICK | AICA_CMD_STOP, (u32 *) AICA_CONTROL_POINT);
	local_irq_restore(flags);
}

/* ALSA code below */
static const struct snd_pcm_hardware snd_pcm_aica_playback_hw = {
	.info = (SNDRV_PCM_INFO_NONINTERLEAVED),
	.formats =
	    (SNDRV_PCM_FMTBIT_S8 | SNDRV_PCM_FMTBIT_S16_LE |
	     SNDRV_PCM_FMTBIT_IMA_ADPCM),
	.rates = SNDRV_PCM_RATE_8000_48000,
	.rate_min = 8000,
	.rate_max = 48000,
	.channels_min = 1,
	.channels_max = 2,
	.buffer_bytes_max = AICA_BUFFER_SIZE,
	.period_bytes_min = AICA_PERIOD_SIZE,
	.period_bytes_max = AICA_PERIOD_SIZE,
	.periods_min = AICA_PERIOD_NUMBER,
	.periods_max = AICA_PERIOD_NUMBER,
};

static int aica_dma_transfer(int channels, int buffer_size,
			     struct snd_pcm_substream *substream)
{
	int q, err, period_offset;
	struct snd_card_aica *dreamcastcard;
	struct snd_pcm_runtime *runtime;
	unsigned long flags;
	err = 0;
	dreamcastcard = substream->pcm->private_data;
	period_offset = dreamcastcard->clicks;
	period_offset %= (AICA_PERIOD_NUMBER / channels);
	runtime = substream->runtime;
	for (q = 0; q < channels; q++) {
		local_irq_save(flags);
		err = dma_xfer(AICA_DMA_CHANNEL,
			       (unsigned long) (runtime->dma_area +
						(AICA_BUFFER_SIZE * q) /
						channels +
						AICA_PERIOD_SIZE *
						period_offset),
			       AICA_CHANNEL0_OFFSET + q * CHANNEL_OFFSET +
			       AICA_PERIOD_SIZE * period_offset,
			       buffer_size / channels, AICA_DMA_MODE);
		if (unlikely(err < 0)) {
			local_irq_restore(flags);
			break;
		}
		dma_wait_for_completion(AICA_DMA_CHANNEL);
		local_irq_restore(flags);
	}
	return err;
}

static void startup_aica(struct snd_card_aica *dreamcastcard)
{
	spu_memload(AICA_CHANNEL0_CONTROL_OFFSET,
		    dreamcastcard->channel, sizeof(struct aica_channel));
	aica_chn_start();
}

static void run_spu_dma(struct work_struct *work)
{
	int buffer_size;
	struct snd_pcm_runtime *runtime;
	struct snd_card_aica *dreamcastcard;
	dreamcastcard =
	    container_of(work, struct snd_card_aica, spu_dma_work);
	runtime = dreamcastcard->substream->runtime;
	if (unlikely(dreamcastcard->dma_check == 0)) {
		buffer_size =
		    frames_to_bytes(runtime, runtime->buffer_size);
		if (runtime->channels > 1)
			dreamcastcard->channel->flags |= 0x01;
		aica_dma_transfer(runtime->channels, buffer_size,
				  dreamcastcard->substream);
		startup_aica(dreamcastcard);
		dreamcastcard->clicks =
		    buffer_size / (AICA_PERIOD_SIZE * runtime->channels);
		return;
	} else {
		aica_dma_transfer(runtime->channels,
				  AICA_PERIOD_SIZE * runtime->channels,
				  dreamcastcard->substream);
		snd_pcm_period_elapsed(dreamcastcard->substream);
		dreamcastcard->clicks++;
		if (unlikely(dreamcastcard->clicks >= AICA_PERIOD_NUMBER))
			dreamcastcard->clicks %= AICA_PERIOD_NUMBER;
		mod_timer(&dreamcastcard->timer, jiffies + 1);
	}
}

static void aica_period_elapsed(struct timer_list *t)
{
	struct snd_card_aica *dreamcastcard = from_timer(dreamcastcard,
							      t, timer);
	struct snd_pcm_substream *substream = dreamcastcard->substream;
	/*timer function - so cannot sleep */
	int play_period;
	struct snd_pcm_runtime *runtime;
	runtime = substream->runtime;
	dreamcastcard = substream->pcm->private_data;
	/* Have we played out an additional period? */
	play_period =
	    frames_to_bytes(runtime,
			    readl
			    (AICA_CONTROL_CHANNEL_SAMPLE_NUMBER)) /
	    AICA_PERIOD_SIZE;
	if (play_period == dreamcastcard->current_period) {
		/* reschedule the timer */
		mod_timer(&(dreamcastcard->timer), jiffies + 1);
		return;
	}
	if (runtime->channels > 1)
		dreamcastcard->current_period = play_period;
	if (unlikely(dreamcastcard->dma_check == 0))
		dreamcastcard->dma_check = 1;
	schedule_work(&(dreamcastcard->spu_dma_work));
}

static void spu_begin_dma(struct snd_pcm_substream *substream)
{
	struct snd_card_aica *dreamcastcard;
	struct snd_pcm_runtime *runtime;
	runtime = substream->runtime;
	dreamcastcard = substream->pcm->private_data;
	/*get the queue to do the work */
	schedule_work(&(dreamcastcard->spu_dma_work));
	mod_timer(&dreamcastcard->timer, jiffies + 4);
}

static int snd_aicapcm_pcm_open(struct snd_pcm_substream
				*substream)
{
	struct snd_pcm_runtime *runtime;
	struct aica_channel *channel;
	struct snd_card_aica *dreamcastcard;
	if (!enable)
		return -ENOENT;
	dreamcastcard = substream->pcm->private_data;
	channel = kmalloc(sizeof(struct aica_channel), GFP_KERNEL);
	if (!channel)
		return -ENOMEM;
	/* set defaults for channel */
	channel->sfmt = SM_8BIT;
	channel->cmd = AICA_CMD_START;
	channel->vol = dreamcastcard->master_volume;
	channel->pan = 0x80;
	channel->pos = 0;
	channel->flags = 0;	/* default to mono */
	dreamcastcard->channel = channel;
	runtime = substream->runtime;
	runtime->hw = snd_pcm_aica_playback_hw;
	spu_enable();
	dreamcastcard->clicks = 0;
	dreamcastcard->current_period = 0;
	dreamcastcard->dma_check = 0;
	return 0;
}

static int snd_aicapcm_pcm_close(struct snd_pcm_substream
				 *substream)
{
	struct snd_card_aica *dreamcastcard = substream->pcm->private_data;
	flush_work(&(dreamcastcard->spu_dma_work));
	del_timer(&dreamcastcard->timer);
	dreamcastcard->substream = NULL;
	kfree(dreamcastcard->channel);
	spu_disable();
	return 0;
}

static int snd_aicapcm_pcm_prepare(struct snd_pcm_substream
				   *substream)
{
	struct snd_card_aica *dreamcastcard = substream->pcm->private_data;
	if ((substream->runtime)->format == SNDRV_PCM_FORMAT_S16_LE)
		dreamcastcard->channel->sfmt = SM_16BIT;
	dreamcastcard->channel->freq = substream->runtime->rate;
	dreamcastcard->substream = substream;
	return 0;
}

static int snd_aicapcm_pcm_trigger(struct snd_pcm_substream
				   *substream, int cmd)
{
	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:
		spu_begin_dma(substream);
		break;
	case SNDRV_PCM_TRIGGER_STOP:
		aica_chn_halt();
		break;
	default:
		return -EINVAL;
	}
	return 0;
}

static unsigned long snd_aicapcm_pcm_pointer(struct snd_pcm_substream
					     *substream)
{
	return readl(AICA_CONTROL_CHANNEL_SAMPLE_NUMBER);
}

static const struct snd_pcm_ops snd_aicapcm_playback_ops = {
	.open = snd_aicapcm_pcm_open,
	.close = snd_aicapcm_pcm_close,
	.prepare = snd_aicapcm_pcm_prepare,
	.trigger = snd_aicapcm_pcm_trigger,
	.pointer = snd_aicapcm_pcm_pointer,
};

/* TO DO: set up to handle more than one pcm instance */
static int __init snd_aicapcmchip(struct snd_card_aica
				  *dreamcastcard, int pcm_index)
{
	struct snd_pcm *pcm;
	int err;
	/* AICA has no capture ability */
	err =
	    snd_pcm_new(dreamcastcard->card, "AICA PCM", pcm_index, 1, 0,
			&pcm);
	if (unlikely(err < 0))
		return err;
	pcm->private_data = dreamcastcard;
	strcpy(pcm->name, "AICA PCM");
	snd_pcm_set_ops(pcm, SNDRV_PCM_STREAM_PLAYBACK,
			&snd_aicapcm_playback_ops);
	/* Allocate the DMA buffers */
	snd_pcm_set_managed_buffer_all(pcm,
				       SNDRV_DMA_TYPE_CONTINUOUS,
				       NULL,
				       AICA_BUFFER_SIZE,
				       AICA_BUFFER_SIZE);
	return 0;
}

/* Mixer controls */
#define aica_pcmswitch_info		snd_ctl_boolean_mono_info

static int aica_pcmswitch_get(struct snd_kcontrol *kcontrol,
			      struct snd_ctl_elem_value *ucontrol)
{
	ucontrol->value.integer.value[0] = 1;	/* TO DO: Fix me */
	return 0;
}

static int aica_pcmswitch_put(struct snd_kcontrol *kcontrol,
			      struct snd_ctl_elem_value *ucontrol)
{
	if (ucontrol->value.integer.value[0] == 1)
		return 0;	/* TO DO: Fix me */
	else
		aica_chn_halt();
	return 0;
}

static int aica_pcmvolume_info(struct snd_kcontrol *kcontrol,
			       struct snd_ctl_elem_info *uinfo)
{
	uinfo->type = SNDRV_CTL_ELEM_TYPE_INTEGER;
	uinfo->count = 1;
	uinfo->value.integer.min = 0;
	uinfo->value.integer.max = 0xFF;
	return 0;
}

static int aica_pcmvolume_get(struct snd_kcontrol *kcontrol,
			      struct snd_ctl_elem_value *ucontrol)
{
	struct snd_card_aica *dreamcastcard;
	dreamcastcard = kcontrol->private_data;
	if (unlikely(!dreamcastcard->channel))
		return -ETXTBSY;	/* we've not yet been set up */
	ucontrol->value.integer.value[0] = dreamcastcard->channel->vol;
	return 0;
}

static int aica_pcmvolume_put(struct snd_kcontrol *kcontrol,
			      struct snd_ctl_elem_value *ucontrol)
{
	struct snd_card_aica *dreamcastcard;
	unsigned int vol;
	dreamcastcard = kcontrol->private_data;
	if (unlikely(!dreamcastcard->channel))
		return -ETXTBSY;
	vol = ucontrol->value.integer.value[0];
	if (vol > 0xff)
		return -EINVAL;
	if (unlikely(dreamcastcard->channel->vol == vol))
		return 0;
	dreamcastcard->channel->vol = ucontrol->value.integer.value[0];
	dreamcastcard->master_volume = ucontrol->value.integer.value[0];
	spu_memload(AICA_CHANNEL0_CONTROL_OFFSET,
		    dreamcastcard->channel, sizeof(struct aica_channel));
	return 1;
}

static const struct snd_kcontrol_new snd_aica_pcmswitch_control = {
	.iface = SNDRV_CTL_ELEM_IFACE_MIXER,
	.name = "PCM Playback Switch",
	.index = 0,
	.info = aica_pcmswitch_info,
	.get = aica_pcmswitch_get,
	.put = aica_pcmswitch_put
};

static const struct snd_kcontrol_new snd_aica_pcmvolume_control = {
	.iface = SNDRV_CTL_ELEM_IFACE_MIXER,
	.name = "PCM Playback Volume",
	.index = 0,
	.info = aica_pcmvolume_info,
	.get = aica_pcmvolume_get,
	.put = aica_pcmvolume_put
};

static int load_aica_firmware(void)
{
	int err;
	const struct firmware *fw_entry;
	spu_reset();
	err = request_firmware(&fw_entry, "aica_firmware.bin", &pd->dev);
	if (unlikely(err))
		return err;
	/* write firmware into memory */
	spu_disable();
	spu_memload(0, fw_entry->data, fw_entry->size);
	spu_enable();
	release_firmware(fw_entry);
	return err;
}

static int add_aicamixer_controls(struct snd_card_aica *dreamcastcard)
{
	int err;
	err = snd_ctl_add
	    (dreamcastcard->card,
	     snd_ctl_new1(&snd_aica_pcmvolume_control, dreamcastcard));
	if (unlikely(err < 0))
		return err;
	err = snd_ctl_add
	    (dreamcastcard->card,
	     snd_ctl_new1(&snd_aica_pcmswitch_control, dreamcastcard));
	if (unlikely(err < 0))
		return err;
	return 0;
}

static int snd_aica_remove(struct platform_device *devptr)
{
	struct snd_card_aica *dreamcastcard;
	dreamcastcard = platform_get_drvdata(devptr);
	if (unlikely(!dreamcastcard))
		return -ENODEV;
	snd_card_free(dreamcastcard->card);
	kfree(dreamcastcard);
	return 0;
}

static int snd_aica_probe(struct platform_device *devptr)
{
	int err;
	struct snd_card_aica *dreamcastcard;
	dreamcastcard = kzalloc(sizeof(struct snd_card_aica), GFP_KERNEL);
	if (unlikely(!dreamcastcard))
		return -ENOMEM;
	err = snd_card_new(&devptr->dev, index, SND_AICA_DRIVER,
			   THIS_MODULE, 0, &dreamcastcard->card);
	if (unlikely(err < 0)) {
		kfree(dreamcastcard);
		return err;
	}
	strcpy(dreamcastcard->card->driver, "snd_aica");
	strcpy(dreamcastcard->card->shortname, SND_AICA_DRIVER);
	strcpy(dreamcastcard->card->longname,
	       "Yamaha AICA Super Intelligent Sound Processor for SEGA Dreamcast");
	/* Prepare to use the queue */
	INIT_WORK(&(dreamcastcard->spu_dma_work), run_spu_dma);
	timer_setup(&dreamcastcard->timer, aica_period_elapsed, 0);
	/* Load the PCM 'chip' */
	err = snd_aicapcmchip(dreamcastcard, 0);
	if (unlikely(err < 0))
		goto freedreamcast;
	/* Add basic controls */
	err = add_aicamixer_controls(dreamcastcard);
	if (unlikely(err < 0))
		goto freedreamcast;
	/* Register the card with ALSA subsystem */
	err = snd_card_register(dreamcastcard->card);
	if (unlikely(err < 0))
		goto freedreamcast;
	platform_set_drvdata(devptr, dreamcastcard);
	snd_printk
	    ("ALSA Driver for Yamaha AICA Super Intelligent Sound Processor\n");
	return 0;
      freedreamcast:
	snd_card_free(dreamcastcard->card);
	kfree(dreamcastcard);
	return err;
}

static struct platform_driver snd_aica_driver = {
	.probe = snd_aica_probe,
	.remove = snd_aica_remove,
	.driver = {
		.name = SND_AICA_DRIVER,
	},
};

static int __init aica_init(void)
{
	int err;
	err = platform_driver_register(&snd_aica_driver);
	if (unlikely(err < 0))
		return err;
	pd = platform_device_register_simple(SND_AICA_DRIVER, -1,
					     aica_memory_space, 2);
	if (IS_ERR(pd)) {
		platform_driver_unregister(&snd_aica_driver);
		return PTR_ERR(pd);
	}
	/* Load the firmware */
	return load_aica_firmware();
}

static void __exit aica_exit(void)
{
	platform_device_unregister(pd);
	platform_driver_unregister(&snd_aica_driver);
	/* Kill any sound still playing and reset ARM7 to safe state */
	spu_reset();
}

module_init(aica_init);
module_exit(aica_exit);
