// SPDX-License-Identifier: GPL-2.0-or-later
/* -*- linux-c -*- *
 *
 * ALSA driver for the digigram lx6464es interface
 *
 * Copyright (c) 2008, 2009 Tim Blechmann <tim@klingt.org>
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/pci.h>
#include <linux/delay.h>
#include <linux/slab.h>

#include <sound/initval.h>
#include <sound/control.h>
#include <sound/info.h>

#include "lx6464es.h"

MODULE_AUTHOR("Tim Blechmann");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("digigram lx6464es");

static int index[SNDRV_CARDS] = SNDRV_DEFAULT_IDX;
static char *id[SNDRV_CARDS] = SNDRV_DEFAULT_STR;
static bool enable[SNDRV_CARDS] = SNDRV_DEFAULT_ENABLE_PNP;

module_param_array(index, int, NULL, 0444);
MODULE_PARM_DESC(index, "Index value for Digigram LX6464ES interface.");
module_param_array(id, charp, NULL, 0444);
MODULE_PARM_DESC(id, "ID string for  Digigram LX6464ES interface.");
module_param_array(enable, bool, NULL, 0444);
MODULE_PARM_DESC(enable, "Enable/disable specific Digigram LX6464ES soundcards.");

static const char card_name[] = "LX6464ES";


#define PCI_DEVICE_ID_PLX_LX6464ES		PCI_DEVICE_ID_PLX_9056

static const struct pci_device_id snd_lx6464es_ids[] = {
	{ PCI_DEVICE_SUB(PCI_VENDOR_ID_PLX, PCI_DEVICE_ID_PLX_LX6464ES,
			 PCI_VENDOR_ID_DIGIGRAM,
			 PCI_SUBDEVICE_ID_DIGIGRAM_LX6464ES_SERIAL_SUBSYSTEM),
	},			/* LX6464ES */
	{ PCI_DEVICE_SUB(PCI_VENDOR_ID_PLX, PCI_DEVICE_ID_PLX_LX6464ES,
			 PCI_VENDOR_ID_DIGIGRAM,
			 PCI_SUBDEVICE_ID_DIGIGRAM_LX6464ES_CAE_SERIAL_SUBSYSTEM),
	},			/* LX6464ES-CAE */
	{ PCI_DEVICE_SUB(PCI_VENDOR_ID_PLX, PCI_DEVICE_ID_PLX_LX6464ES,
			 PCI_VENDOR_ID_DIGIGRAM,
			 PCI_SUBDEVICE_ID_DIGIGRAM_LX6464ESE_SERIAL_SUBSYSTEM),
	},			/* LX6464ESe */
	{ PCI_DEVICE_SUB(PCI_VENDOR_ID_PLX, PCI_DEVICE_ID_PLX_LX6464ES,
			 PCI_VENDOR_ID_DIGIGRAM,
			 PCI_SUBDEVICE_ID_DIGIGRAM_LX6464ESE_CAE_SERIAL_SUBSYSTEM),
	},			/* LX6464ESe-CAE */
	{ 0, },
};

MODULE_DEVICE_TABLE(pci, snd_lx6464es_ids);



/* PGO pour USERo dans le registre pci_0x06/loc_0xEC */
#define CHIPSC_RESET_XILINX (1L<<16)


/* alsa callbacks */
static const struct snd_pcm_hardware lx_caps = {
	.info             = (SNDRV_PCM_INFO_MMAP |
			     SNDRV_PCM_INFO_INTERLEAVED |
			     SNDRV_PCM_INFO_MMAP_VALID |
			     SNDRV_PCM_INFO_SYNC_START),
	.formats	  = (SNDRV_PCM_FMTBIT_S16_LE |
			     SNDRV_PCM_FMTBIT_S16_BE |
			     SNDRV_PCM_FMTBIT_S24_3LE |
			     SNDRV_PCM_FMTBIT_S24_3BE),
	.rates            = (SNDRV_PCM_RATE_CONTINUOUS |
			     SNDRV_PCM_RATE_8000_192000),
	.rate_min         = 8000,
	.rate_max         = 192000,
	.channels_min     = 2,
	.channels_max     = 64,
	.buffer_bytes_max = 64*2*3*MICROBLAZE_IBL_MAX*MAX_STREAM_BUFFER,
	.period_bytes_min = (2*2*MICROBLAZE_IBL_MIN*2),
	.period_bytes_max = (4*64*MICROBLAZE_IBL_MAX*MAX_STREAM_BUFFER),
	.periods_min      = 2,
	.periods_max      = MAX_STREAM_BUFFER,
};

static int lx_set_granularity(struct lx6464es *chip, u32 gran);


static int lx_hardware_open(struct lx6464es *chip,
			    struct snd_pcm_substream *substream)
{
	int err = 0;
	struct snd_pcm_runtime *runtime = substream->runtime;
	int channels = runtime->channels;
	int is_capture = (substream->stream == SNDRV_PCM_STREAM_CAPTURE);

	snd_pcm_uframes_t period_size = runtime->period_size;

	dev_dbg(chip->card->dev, "allocating pipe for %d channels\n", channels);
	err = lx_pipe_allocate(chip, 0, is_capture, channels);
	if (err < 0) {
		dev_err(chip->card->dev, LXP "allocating pipe failed\n");
		return err;
	}

	err = lx_set_granularity(chip, period_size);
	if (err < 0) {
		dev_err(chip->card->dev, "setting granularity to %ld failed\n",
			   period_size);
		return err;
	}

	return 0;
}

static int lx_hardware_start(struct lx6464es *chip,
			     struct snd_pcm_substream *substream)
{
	int err = 0;
	struct snd_pcm_runtime *runtime = substream->runtime;
	int is_capture = (substream->stream == SNDRV_PCM_STREAM_CAPTURE);

	dev_dbg(chip->card->dev, "setting stream format\n");
	err = lx_stream_set_format(chip, runtime, 0, is_capture);
	if (err < 0) {
		dev_err(chip->card->dev, "setting stream format failed\n");
		return err;
	}

	dev_dbg(chip->card->dev, "starting pipe\n");
	err = lx_pipe_start(chip, 0, is_capture);
	if (err < 0) {
		dev_err(chip->card->dev, "starting pipe failed\n");
		return err;
	}

	dev_dbg(chip->card->dev, "waiting for pipe to start\n");
	err = lx_pipe_wait_for_start(chip, 0, is_capture);
	if (err < 0) {
		dev_err(chip->card->dev, "waiting for pipe failed\n");
		return err;
	}

	return err;
}


static int lx_hardware_stop(struct lx6464es *chip,
			    struct snd_pcm_substream *substream)
{
	int err = 0;
	int is_capture = (substream->stream == SNDRV_PCM_STREAM_CAPTURE);

	dev_dbg(chip->card->dev, "pausing pipe\n");
	err = lx_pipe_pause(chip, 0, is_capture);
	if (err < 0) {
		dev_err(chip->card->dev, "pausing pipe failed\n");
		return err;
	}

	dev_dbg(chip->card->dev, "waiting for pipe to become idle\n");
	err = lx_pipe_wait_for_idle(chip, 0, is_capture);
	if (err < 0) {
		dev_err(chip->card->dev, "waiting for pipe failed\n");
		return err;
	}

	dev_dbg(chip->card->dev, "stopping pipe\n");
	err = lx_pipe_stop(chip, 0, is_capture);
	if (err < 0) {
		dev_err(chip->card->dev, "stopping pipe failed\n");
		return err;
	}

	return err;
}


static int lx_hardware_close(struct lx6464es *chip,
			     struct snd_pcm_substream *substream)
{
	int err = 0;
	int is_capture = (substream->stream == SNDRV_PCM_STREAM_CAPTURE);

	dev_dbg(chip->card->dev, "releasing pipe\n");
	err = lx_pipe_release(chip, 0, is_capture);
	if (err < 0) {
		dev_err(chip->card->dev, "releasing pipe failed\n");
		return err;
	}

	return err;
}


static int lx_pcm_open(struct snd_pcm_substream *substream)
{
	struct lx6464es *chip = snd_pcm_substream_chip(substream);
	struct snd_pcm_runtime *runtime = substream->runtime;
	int err = 0;
	int board_rate;

	dev_dbg(chip->card->dev, "->lx_pcm_open\n");
	mutex_lock(&chip->setup_mutex);

	/* copy the struct snd_pcm_hardware struct */
	runtime->hw = lx_caps;

#if 0
	/* buffer-size should better be multiple of period-size */
	err = snd_pcm_hw_constraint_integer(runtime,
					    SNDRV_PCM_HW_PARAM_PERIODS);
	if (err < 0) {
		dev_warn(chip->card->dev, "could not constrain periods\n");
		goto exit;
	}
#endif

	/* the clock rate cannot be changed */
	board_rate = chip->board_sample_rate;
	err = snd_pcm_hw_constraint_single(runtime, SNDRV_PCM_HW_PARAM_RATE,
					   board_rate);

	if (err < 0) {
		dev_warn(chip->card->dev, "could not constrain periods\n");
		goto exit;
	}

	/* constrain period size */
	err = snd_pcm_hw_constraint_minmax(runtime,
					   SNDRV_PCM_HW_PARAM_PERIOD_SIZE,
					   MICROBLAZE_IBL_MIN,
					   MICROBLAZE_IBL_MAX);
	if (err < 0) {
		dev_warn(chip->card->dev,
			   "could not constrain period size\n");
		goto exit;
	}

	snd_pcm_hw_constraint_step(runtime, 0,
				   SNDRV_PCM_HW_PARAM_BUFFER_SIZE, 32);

	snd_pcm_set_sync(substream);
	err = 0;

exit:
	runtime->private_data = chip;

	mutex_unlock(&chip->setup_mutex);
	dev_dbg(chip->card->dev, "<-lx_pcm_open, %d\n", err);
	return err;
}

static int lx_pcm_close(struct snd_pcm_substream *substream)
{
	dev_dbg(substream->pcm->card->dev, "->lx_pcm_close\n");
	return 0;
}

static snd_pcm_uframes_t lx_pcm_stream_pointer(struct snd_pcm_substream
					       *substream)
{
	struct lx6464es *chip = snd_pcm_substream_chip(substream);
	snd_pcm_uframes_t pos;
	int is_capture = (substream->stream == SNDRV_PCM_STREAM_CAPTURE);

	struct lx_stream *lx_stream = is_capture ? &chip->capture_stream :
		&chip->playback_stream;

	dev_dbg(chip->card->dev, "->lx_pcm_stream_pointer\n");

	mutex_lock(&chip->lock);
	pos = lx_stream->frame_pos * substream->runtime->period_size;
	mutex_unlock(&chip->lock);

	dev_dbg(chip->card->dev, "stream_pointer at %ld\n", pos);
	return pos;
}

static int lx_pcm_prepare(struct snd_pcm_substream *substream)
{
	struct lx6464es *chip = snd_pcm_substream_chip(substream);
	int err = 0;
	const int is_capture = (substream->stream == SNDRV_PCM_STREAM_CAPTURE);

	dev_dbg(chip->card->dev, "->lx_pcm_prepare\n");

	mutex_lock(&chip->setup_mutex);

	if (chip->hardware_running[is_capture]) {
		err = lx_hardware_stop(chip, substream);
		if (err < 0) {
			dev_err(chip->card->dev, "failed to stop hardware. "
				   "Error code %d\n", err);
			goto exit;
		}

		err = lx_hardware_close(chip, substream);
		if (err < 0) {
			dev_err(chip->card->dev, "failed to close hardware. "
				   "Error code %d\n", err);
			goto exit;
		}
	}

	dev_dbg(chip->card->dev, "opening hardware\n");
	err = lx_hardware_open(chip, substream);
	if (err < 0) {
		dev_err(chip->card->dev, "failed to open hardware. "
			   "Error code %d\n", err);
		goto exit;
	}

	err = lx_hardware_start(chip, substream);
	if (err < 0) {
		dev_err(chip->card->dev, "failed to start hardware. "
			   "Error code %d\n", err);
		goto exit;
	}

	chip->hardware_running[is_capture] = 1;

	if (chip->board_sample_rate != substream->runtime->rate) {
		if (!err)
			chip->board_sample_rate = substream->runtime->rate;
	}

exit:
	mutex_unlock(&chip->setup_mutex);
	return err;
}

static int lx_pcm_hw_params(struct snd_pcm_substream *substream,
			    struct snd_pcm_hw_params *hw_params, int is_capture)
{
	struct lx6464es *chip = snd_pcm_substream_chip(substream);

	dev_dbg(chip->card->dev, "->lx_pcm_hw_params\n");

	mutex_lock(&chip->setup_mutex);

	if (is_capture)
		chip->capture_stream.stream = substream;
	else
		chip->playback_stream.stream = substream;

	mutex_unlock(&chip->setup_mutex);
	return 0;
}

static int lx_pcm_hw_params_playback(struct snd_pcm_substream *substream,
				 struct snd_pcm_hw_params *hw_params)
{
	return lx_pcm_hw_params(substream, hw_params, 0);
}

static int lx_pcm_hw_params_capture(struct snd_pcm_substream *substream,
				 struct snd_pcm_hw_params *hw_params)
{
	return lx_pcm_hw_params(substream, hw_params, 1);
}

static int lx_pcm_hw_free(struct snd_pcm_substream *substream)
{
	struct lx6464es *chip = snd_pcm_substream_chip(substream);
	int err = 0;
	int is_capture = (substream->stream == SNDRV_PCM_STREAM_CAPTURE);

	dev_dbg(chip->card->dev, "->lx_pcm_hw_free\n");
	mutex_lock(&chip->setup_mutex);

	if (chip->hardware_running[is_capture]) {
		err = lx_hardware_stop(chip, substream);
		if (err < 0) {
			dev_err(chip->card->dev, "failed to stop hardware. "
				   "Error code %d\n", err);
			goto exit;
		}

		err = lx_hardware_close(chip, substream);
		if (err < 0) {
			dev_err(chip->card->dev, "failed to close hardware. "
				   "Error code %d\n", err);
			goto exit;
		}

		chip->hardware_running[is_capture] = 0;
	}

	if (is_capture)
		chip->capture_stream.stream = NULL;
	else
		chip->playback_stream.stream = NULL;

exit:
	mutex_unlock(&chip->setup_mutex);
	return err;
}

static void lx_trigger_start(struct lx6464es *chip, struct lx_stream *lx_stream)
{
	struct snd_pcm_substream *substream = lx_stream->stream;
	const unsigned int is_capture = lx_stream->is_capture;

	int err;

	const u32 channels = substream->runtime->channels;
	const u32 bytes_per_frame = channels * 3;
	const u32 period_size = substream->runtime->period_size;
	const u32 periods = substream->runtime->periods;
	const u32 period_bytes = period_size * bytes_per_frame;

	dma_addr_t buf = substream->dma_buffer.addr;
	int i;

	u32 needed, freed;
	u32 size_array[5];

	for (i = 0; i != periods; ++i) {
		u32 buffer_index = 0;

		err = lx_buffer_ask(chip, 0, is_capture, &needed, &freed,
				    size_array);
		dev_dbg(chip->card->dev, "starting: needed %d, freed %d\n",
			    needed, freed);

		err = lx_buffer_give(chip, 0, is_capture, period_bytes,
				     lower_32_bits(buf), upper_32_bits(buf),
				     &buffer_index);

		dev_dbg(chip->card->dev, "starting: buffer index %x on 0x%lx (%d bytes)\n",
			    buffer_index, (unsigned long)buf, period_bytes);
		buf += period_bytes;
	}

	err = lx_buffer_ask(chip, 0, is_capture, &needed, &freed, size_array);
	dev_dbg(chip->card->dev, "starting: needed %d, freed %d\n", needed, freed);

	dev_dbg(chip->card->dev, "starting: starting stream\n");
	err = lx_stream_start(chip, 0, is_capture);
	if (err < 0)
		dev_err(chip->card->dev, "couldn't start stream\n");
	else
		lx_stream->status = LX_STREAM_STATUS_RUNNING;

	lx_stream->frame_pos = 0;
}

static void lx_trigger_stop(struct lx6464es *chip, struct lx_stream *lx_stream)
{
	const unsigned int is_capture = lx_stream->is_capture;
	int err;

	dev_dbg(chip->card->dev, "stopping: stopping stream\n");
	err = lx_stream_stop(chip, 0, is_capture);
	if (err < 0)
		dev_err(chip->card->dev, "couldn't stop stream\n");
	else
		lx_stream->status = LX_STREAM_STATUS_FREE;

}

static void lx_trigger_dispatch_stream(struct lx6464es *chip,
				       struct lx_stream *lx_stream)
{
	switch (lx_stream->status) {
	case LX_STREAM_STATUS_SCHEDULE_RUN:
		lx_trigger_start(chip, lx_stream);
		break;

	case LX_STREAM_STATUS_SCHEDULE_STOP:
		lx_trigger_stop(chip, lx_stream);
		break;

	default:
		break;
	}
}

static int lx_pcm_trigger_dispatch(struct lx6464es *chip,
				   struct lx_stream *lx_stream, int cmd)
{
	int err = 0;

	mutex_lock(&chip->lock);
	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:
		lx_stream->status = LX_STREAM_STATUS_SCHEDULE_RUN;
		break;

	case SNDRV_PCM_TRIGGER_STOP:
		lx_stream->status = LX_STREAM_STATUS_SCHEDULE_STOP;
		break;

	default:
		err = -EINVAL;
		goto exit;
	}

	lx_trigger_dispatch_stream(chip, &chip->capture_stream);
	lx_trigger_dispatch_stream(chip, &chip->playback_stream);

exit:
	mutex_unlock(&chip->lock);
	return err;
}


static int lx_pcm_trigger(struct snd_pcm_substream *substream, int cmd)
{
	struct lx6464es *chip = snd_pcm_substream_chip(substream);
	const int is_capture = (substream->stream == SNDRV_PCM_STREAM_CAPTURE);
	struct lx_stream *stream = is_capture ? &chip->capture_stream :
		&chip->playback_stream;

	dev_dbg(chip->card->dev, "->lx_pcm_trigger\n");

	return lx_pcm_trigger_dispatch(chip, stream, cmd);
}

static int snd_lx6464es_free(struct lx6464es *chip)
{
	dev_dbg(chip->card->dev, "->snd_lx6464es_free\n");

	lx_irq_disable(chip);

	if (chip->irq >= 0)
		free_irq(chip->irq, chip);

	iounmap(chip->port_dsp_bar);
	ioport_unmap(chip->port_plx_remapped);

	pci_release_regions(chip->pci);
	pci_disable_device(chip->pci);

	kfree(chip);

	return 0;
}

static int snd_lx6464es_dev_free(struct snd_device *device)
{
	return snd_lx6464es_free(device->device_data);
}

/* reset the dsp during initialization */
static int lx_init_xilinx_reset(struct lx6464es *chip)
{
	int i;
	u32 plx_reg = lx_plx_reg_read(chip, ePLX_CHIPSC);

	dev_dbg(chip->card->dev, "->lx_init_xilinx_reset\n");

	/* activate reset of xilinx */
	plx_reg &= ~CHIPSC_RESET_XILINX;

	lx_plx_reg_write(chip, ePLX_CHIPSC, plx_reg);
	msleep(1);

	lx_plx_reg_write(chip, ePLX_MBOX3, 0);
	msleep(1);

	plx_reg |= CHIPSC_RESET_XILINX;
	lx_plx_reg_write(chip, ePLX_CHIPSC, plx_reg);

	/* deactivate reset of xilinx */
	for (i = 0; i != 100; ++i) {
		u32 reg_mbox3;
		msleep(10);
		reg_mbox3 = lx_plx_reg_read(chip, ePLX_MBOX3);
		if (reg_mbox3) {
			dev_dbg(chip->card->dev, "xilinx reset done\n");
			dev_dbg(chip->card->dev, "xilinx took %d loops\n", i);
			break;
		}
	}

	/* todo: add some error handling? */

	/* clear mr */
	lx_dsp_reg_write(chip, eReg_CSM, 0);

	/* le xilinx ES peut ne pas etre encore pret, on attend. */
	msleep(600);

	return 0;
}

static int lx_init_xilinx_test(struct lx6464es *chip)
{
	u32 reg;

	dev_dbg(chip->card->dev, "->lx_init_xilinx_test\n");

	/* TEST if we have access to Xilinx/MicroBlaze */
	lx_dsp_reg_write(chip, eReg_CSM, 0);

	reg = lx_dsp_reg_read(chip, eReg_CSM);

	if (reg) {
		dev_err(chip->card->dev, "Problem: Reg_CSM %x.\n", reg);

		/* PCI9056_SPACE0_REMAP */
		lx_plx_reg_write(chip, ePLX_PCICR, 1);

		reg = lx_dsp_reg_read(chip, eReg_CSM);
		if (reg) {
			dev_err(chip->card->dev, "Error: Reg_CSM %x.\n", reg);
			return -EAGAIN; /* seems to be appropriate */
		}
	}

	dev_dbg(chip->card->dev, "Xilinx/MicroBlaze access test successful\n");

	return 0;
}

/* initialize ethersound */
static int lx_init_ethersound_config(struct lx6464es *chip)
{
	int i;
	u32 orig_conf_es = lx_dsp_reg_read(chip, eReg_CONFES);

	/* configure 64 io channels */
	u32 conf_es = (orig_conf_es & CONFES_READ_PART_MASK) |
		(64 << IOCR_INPUTS_OFFSET) |
		(64 << IOCR_OUTPUTS_OFFSET) |
		(FREQ_RATIO_SINGLE_MODE << FREQ_RATIO_OFFSET);

	dev_dbg(chip->card->dev, "->lx_init_ethersound\n");

	chip->freq_ratio = FREQ_RATIO_SINGLE_MODE;

	/*
	 * write it to the card !
	 * this actually kicks the ES xilinx, the first time since poweron.
	 * the MAC address in the Reg_ADMACESMSB Reg_ADMACESLSB registers
	 * is not ready before this is done, and the bit 2 in Reg_CSES is set.
	 * */
	lx_dsp_reg_write(chip, eReg_CONFES, conf_es);

	for (i = 0; i != 1000; ++i) {
		if (lx_dsp_reg_read(chip, eReg_CSES) & 4) {
			dev_dbg(chip->card->dev, "ethersound initialized after %dms\n",
				   i);
			goto ethersound_initialized;
		}
		msleep(1);
	}
	dev_warn(chip->card->dev,
		   "ethersound could not be initialized after %dms\n", i);
	return -ETIMEDOUT;

 ethersound_initialized:
	dev_dbg(chip->card->dev, "ethersound initialized\n");
	return 0;
}

static int lx_init_get_version_features(struct lx6464es *chip)
{
	u32 dsp_version;

	int err;

	dev_dbg(chip->card->dev, "->lx_init_get_version_features\n");

	err = lx_dsp_get_version(chip, &dsp_version);

	if (err == 0) {
		u32 freq;

		dev_info(chip->card->dev, "DSP version: V%02d.%02d #%d\n",
			   (dsp_version>>16) & 0xff, (dsp_version>>8) & 0xff,
			   dsp_version & 0xff);

		/* later: what firmware version do we expect? */

		/* retrieve Play/Rec features */
		/* done here because we may have to handle alternate
		 * DSP files. */
		/* later */

		/* init the EtherSound sample rate */
		err = lx_dsp_get_clock_frequency(chip, &freq);
		if (err == 0)
			chip->board_sample_rate = freq;
		dev_dbg(chip->card->dev, "actual clock frequency %d\n", freq);
	} else {
		dev_err(chip->card->dev, "DSP corrupted \n");
		err = -EAGAIN;
	}

	return err;
}

static int lx_set_granularity(struct lx6464es *chip, u32 gran)
{
	int err = 0;
	u32 snapped_gran = MICROBLAZE_IBL_MIN;

	dev_dbg(chip->card->dev, "->lx_set_granularity\n");

	/* blocksize is a power of 2 */
	while ((snapped_gran < gran) &&
	       (snapped_gran < MICROBLAZE_IBL_MAX)) {
		snapped_gran *= 2;
	}

	if (snapped_gran == chip->pcm_granularity)
		return 0;

	err = lx_dsp_set_granularity(chip, snapped_gran);
	if (err < 0) {
		dev_warn(chip->card->dev, "could not set granularity\n");
		err = -EAGAIN;
	}

	if (snapped_gran != gran)
		dev_err(chip->card->dev, "snapped blocksize to %d\n", snapped_gran);

	dev_dbg(chip->card->dev, "set blocksize on board %d\n", snapped_gran);
	chip->pcm_granularity = snapped_gran;

	return err;
}

/* initialize and test the xilinx dsp chip */
static int lx_init_dsp(struct lx6464es *chip)
{
	int err;
	int i;

	dev_dbg(chip->card->dev, "->lx_init_dsp\n");

	dev_dbg(chip->card->dev, "initialize board\n");
	err = lx_init_xilinx_reset(chip);
	if (err)
		return err;

	dev_dbg(chip->card->dev, "testing board\n");
	err = lx_init_xilinx_test(chip);
	if (err)
		return err;

	dev_dbg(chip->card->dev, "initialize ethersound configuration\n");
	err = lx_init_ethersound_config(chip);
	if (err)
		return err;

	lx_irq_enable(chip);

	/** \todo the mac address should be ready by not, but it isn't,
	 *  so we wait for it */
	for (i = 0; i != 1000; ++i) {
		err = lx_dsp_get_mac(chip);
		if (err)
			return err;
		if (chip->mac_address[0] || chip->mac_address[1] || chip->mac_address[2] ||
		    chip->mac_address[3] || chip->mac_address[4] || chip->mac_address[5])
			goto mac_ready;
		msleep(1);
	}
	return -ETIMEDOUT;

mac_ready:
	dev_dbg(chip->card->dev, "mac address ready read after: %dms\n", i);
	dev_info(chip->card->dev,
		 "mac address: %02X.%02X.%02X.%02X.%02X.%02X\n",
		   chip->mac_address[0], chip->mac_address[1], chip->mac_address[2],
		   chip->mac_address[3], chip->mac_address[4], chip->mac_address[5]);

	err = lx_init_get_version_features(chip);
	if (err)
		return err;

	lx_set_granularity(chip, MICROBLAZE_IBL_DEFAULT);

	chip->playback_mute = 0;

	return err;
}

static const struct snd_pcm_ops lx_ops_playback = {
	.open      = lx_pcm_open,
	.close     = lx_pcm_close,
	.prepare   = lx_pcm_prepare,
	.hw_params = lx_pcm_hw_params_playback,
	.hw_free   = lx_pcm_hw_free,
	.trigger   = lx_pcm_trigger,
	.pointer   = lx_pcm_stream_pointer,
};

static const struct snd_pcm_ops lx_ops_capture = {
	.open      = lx_pcm_open,
	.close     = lx_pcm_close,
	.prepare   = lx_pcm_prepare,
	.hw_params = lx_pcm_hw_params_capture,
	.hw_free   = lx_pcm_hw_free,
	.trigger   = lx_pcm_trigger,
	.pointer   = lx_pcm_stream_pointer,
};

static int lx_pcm_create(struct lx6464es *chip)
{
	int err;
	struct snd_pcm *pcm;

	u32 size = 64 *		     /* channels */
		3 *		     /* 24 bit samples */
		MAX_STREAM_BUFFER *  /* periods */
		MICROBLAZE_IBL_MAX * /* frames per period */
		2;		     /* duplex */

	size = PAGE_ALIGN(size);

	/* hardcoded device name & channel count */
	err = snd_pcm_new(chip->card, (char *)card_name, 0,
			  1, 1, &pcm);
	if (err < 0)
		return err;

	pcm->private_data = chip;

	snd_pcm_set_ops(pcm, SNDRV_PCM_STREAM_PLAYBACK, &lx_ops_playback);
	snd_pcm_set_ops(pcm, SNDRV_PCM_STREAM_CAPTURE, &lx_ops_capture);

	pcm->info_flags = 0;
	pcm->nonatomic = true;
	strcpy(pcm->name, card_name);

	snd_pcm_set_managed_buffer_all(pcm, SNDRV_DMA_TYPE_DEV,
				       &chip->pci->dev, size, size);

	chip->pcm = pcm;
	chip->capture_stream.is_capture = 1;

	return 0;
}

static int lx_control_playback_info(struct snd_kcontrol *kcontrol,
				    struct snd_ctl_elem_info *uinfo)
{
	uinfo->type = SNDRV_CTL_ELEM_TYPE_BOOLEAN;
	uinfo->count = 1;
	uinfo->value.integer.min = 0;
	uinfo->value.integer.max = 1;
	return 0;
}

static int lx_control_playback_get(struct snd_kcontrol *kcontrol,
				   struct snd_ctl_elem_value *ucontrol)
{
	struct lx6464es *chip = snd_kcontrol_chip(kcontrol);
	ucontrol->value.integer.value[0] = chip->playback_mute;
	return 0;
}

static int lx_control_playback_put(struct snd_kcontrol *kcontrol,
				   struct snd_ctl_elem_value *ucontrol)
{
	struct lx6464es *chip = snd_kcontrol_chip(kcontrol);
	int changed = 0;
	int current_value = chip->playback_mute;

	if (current_value != ucontrol->value.integer.value[0]) {
		lx_level_unmute(chip, 0, !current_value);
		chip->playback_mute = !current_value;
		changed = 1;
	}
	return changed;
}

static const struct snd_kcontrol_new lx_control_playback_switch = {
	.iface = SNDRV_CTL_ELEM_IFACE_MIXER,
	.name = "PCM Playback Switch",
	.index = 0,
	.access = SNDRV_CTL_ELEM_ACCESS_READWRITE,
	.private_value = 0,
	.info = lx_control_playback_info,
	.get = lx_control_playback_get,
	.put = lx_control_playback_put
};



static void lx_proc_levels_read(struct snd_info_entry *entry,
				struct snd_info_buffer *buffer)
{
	u32 levels[64];
	int err;
	int i, j;
	struct lx6464es *chip = entry->private_data;

	snd_iprintf(buffer, "capture levels:\n");
	err = lx_level_peaks(chip, 1, 64, levels);
	if (err < 0)
		return;

	for (i = 0; i != 8; ++i) {
		for (j = 0; j != 8; ++j)
			snd_iprintf(buffer, "%08x ", levels[i*8+j]);
		snd_iprintf(buffer, "\n");
	}

	snd_iprintf(buffer, "\nplayback levels:\n");

	err = lx_level_peaks(chip, 0, 64, levels);
	if (err < 0)
		return;

	for (i = 0; i != 8; ++i) {
		for (j = 0; j != 8; ++j)
			snd_iprintf(buffer, "%08x ", levels[i*8+j]);
		snd_iprintf(buffer, "\n");
	}

	snd_iprintf(buffer, "\n");
}

static int lx_proc_create(struct snd_card *card, struct lx6464es *chip)
{
	return snd_card_ro_proc_new(card, "levels", chip, lx_proc_levels_read);
}


static int snd_lx6464es_create(struct snd_card *card,
			       struct pci_dev *pci,
			       struct lx6464es **rchip)
{
	struct lx6464es *chip;
	int err;

	static const struct snd_device_ops ops = {
		.dev_free = snd_lx6464es_dev_free,
	};

	dev_dbg(card->dev, "->snd_lx6464es_create\n");

	*rchip = NULL;

	/* enable PCI device */
	err = pci_enable_device(pci);
	if (err < 0)
		return err;

	pci_set_master(pci);

	/* check if we can restrict PCI DMA transfers to 32 bits */
	err = dma_set_mask(&pci->dev, DMA_BIT_MASK(32));
	if (err < 0) {
		dev_err(card->dev,
			"architecture does not support 32bit PCI busmaster DMA\n");
		pci_disable_device(pci);
		return -ENXIO;
	}

	chip = kzalloc(sizeof(*chip), GFP_KERNEL);
	if (chip == NULL) {
		err = -ENOMEM;
		goto alloc_failed;
	}

	chip->card = card;
	chip->pci = pci;
	chip->irq = -1;

	/* initialize synchronization structs */
	mutex_init(&chip->lock);
	mutex_init(&chip->msg_lock);
	mutex_init(&chip->setup_mutex);

	/* request resources */
	err = pci_request_regions(pci, card_name);
	if (err < 0)
		goto request_regions_failed;

	/* plx port */
	chip->port_plx = pci_resource_start(pci, 1);
	chip->port_plx_remapped = ioport_map(chip->port_plx,
					     pci_resource_len(pci, 1));

	/* dsp port */
	chip->port_dsp_bar = pci_ioremap_bar(pci, 2);
	if (!chip->port_dsp_bar) {
		dev_err(card->dev, "cannot remap PCI memory region\n");
		err = -ENOMEM;
		goto remap_pci_failed;
	}

	err = request_threaded_irq(pci->irq, lx_interrupt, lx_threaded_irq,
				   IRQF_SHARED, KBUILD_MODNAME, chip);
	if (err) {
		dev_err(card->dev, "unable to grab IRQ %d\n", pci->irq);
		goto request_irq_failed;
	}
	chip->irq = pci->irq;
	card->sync_irq = chip->irq;

	err = snd_device_new(card, SNDRV_DEV_LOWLEVEL, chip, &ops);
	if (err < 0)
		goto device_new_failed;

	err = lx_init_dsp(chip);
	if (err < 0) {
		dev_err(card->dev, "error during DSP initialization\n");
		return err;
	}

	err = lx_pcm_create(chip);
	if (err < 0)
		return err;

	err = lx_proc_create(card, chip);
	if (err < 0)
		return err;

	err = snd_ctl_add(card, snd_ctl_new1(&lx_control_playback_switch,
					     chip));
	if (err < 0)
		return err;

	*rchip = chip;
	return 0;

device_new_failed:
	free_irq(pci->irq, chip);

request_irq_failed:
	iounmap(chip->port_dsp_bar);

remap_pci_failed:
	pci_release_regions(pci);

request_regions_failed:
	kfree(chip);

alloc_failed:
	pci_disable_device(pci);

	return err;
}

static int snd_lx6464es_probe(struct pci_dev *pci,
			      const struct pci_device_id *pci_id)
{
	static int dev;
	struct snd_card *card;
	struct lx6464es *chip;
	int err;

	dev_dbg(&pci->dev, "->snd_lx6464es_probe\n");

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

	err = snd_lx6464es_create(card, pci, &chip);
	if (err < 0) {
		dev_err(card->dev, "error during snd_lx6464es_create\n");
		goto out_free;
	}

	strcpy(card->driver, "LX6464ES");
	sprintf(card->id, "LX6464ES_%02X%02X%02X",
		chip->mac_address[3], chip->mac_address[4], chip->mac_address[5]);

	sprintf(card->shortname, "LX6464ES %02X.%02X.%02X.%02X.%02X.%02X",
		chip->mac_address[0], chip->mac_address[1], chip->mac_address[2],
		chip->mac_address[3], chip->mac_address[4], chip->mac_address[5]);

	sprintf(card->longname, "%s at 0x%lx, 0x%p, irq %i",
		card->shortname, chip->port_plx,
		chip->port_dsp_bar, chip->irq);

	err = snd_card_register(card);
	if (err < 0)
		goto out_free;

	dev_dbg(chip->card->dev, "initialization successful\n");
	pci_set_drvdata(pci, card);
	dev++;
	return 0;

out_free:
	snd_card_free(card);
	return err;

}

static void snd_lx6464es_remove(struct pci_dev *pci)
{
	snd_card_free(pci_get_drvdata(pci));
}


static struct pci_driver lx6464es_driver = {
	.name =     KBUILD_MODNAME,
	.id_table = snd_lx6464es_ids,
	.probe =    snd_lx6464es_probe,
	.remove = snd_lx6464es_remove,
};

module_pci_driver(lx6464es_driver);
