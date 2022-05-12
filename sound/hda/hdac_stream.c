// SPDX-License-Identifier: GPL-2.0-only
/*
 * HD-audio stream operations
 */

#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/export.h>
#include <linux/clocksource.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/hdaudio.h>
#include <sound/hda_register.h>
#include "trace.h"

/**
 * snd_hdac_get_stream_stripe_ctl - get stripe control value
 * @bus: HD-audio core bus
 * @substream: PCM substream
 */
int snd_hdac_get_stream_stripe_ctl(struct hdac_bus *bus,
				   struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	unsigned int channels = runtime->channels,
		     rate = runtime->rate,
		     bits_per_sample = runtime->sample_bits,
		     max_sdo_lines, value, sdo_line;

	/* T_AZA_GCAP_NSDO is 1:2 bitfields in GCAP */
	max_sdo_lines = snd_hdac_chip_readl(bus, GCAP) & AZX_GCAP_NSDO;

	/* following is from HD audio spec */
	for (sdo_line = max_sdo_lines; sdo_line > 0; sdo_line >>= 1) {
		if (rate > 48000)
			value = (channels * bits_per_sample *
					(rate / 48000)) / sdo_line;
		else
			value = (channels * bits_per_sample) / sdo_line;

		if (value >= bus->sdo_limit)
			break;
	}

	/* stripe value: 0 for 1SDO, 1 for 2SDO, 2 for 4SDO lines */
	return sdo_line >> 1;
}
EXPORT_SYMBOL_GPL(snd_hdac_get_stream_stripe_ctl);

/**
 * snd_hdac_stream_init - initialize each stream (aka device)
 * @bus: HD-audio core bus
 * @azx_dev: HD-audio core stream object to initialize
 * @idx: stream index number
 * @direction: stream direction (SNDRV_PCM_STREAM_PLAYBACK or SNDRV_PCM_STREAM_CAPTURE)
 * @tag: the tag id to assign
 *
 * Assign the starting bdl address to each stream (device) and initialize.
 */
void snd_hdac_stream_init(struct hdac_bus *bus, struct hdac_stream *azx_dev,
			  int idx, int direction, int tag)
{
	azx_dev->bus = bus;
	/* offset: SDI0=0x80, SDI1=0xa0, ... SDO3=0x160 */
	azx_dev->sd_addr = bus->remap_addr + (0x20 * idx + 0x80);
	/* int mask: SDI0=0x01, SDI1=0x02, ... SDO3=0x80 */
	azx_dev->sd_int_sta_mask = 1 << idx;
	azx_dev->index = idx;
	azx_dev->direction = direction;
	azx_dev->stream_tag = tag;
	snd_hdac_dsp_lock_init(azx_dev);
	list_add_tail(&azx_dev->list, &bus->stream_list);
}
EXPORT_SYMBOL_GPL(snd_hdac_stream_init);

/**
 * snd_hdac_stream_start - start a stream
 * @azx_dev: HD-audio core stream to start
 * @fresh_start: false = wallclock timestamp relative to period wallclock
 *
 * Start a stream, set start_wallclk and set the running flag.
 */
void snd_hdac_stream_start(struct hdac_stream *azx_dev, bool fresh_start)
{
	struct hdac_bus *bus = azx_dev->bus;
	int stripe_ctl;

	trace_snd_hdac_stream_start(bus, azx_dev);

	azx_dev->start_wallclk = snd_hdac_chip_readl(bus, WALLCLK);
	if (!fresh_start)
		azx_dev->start_wallclk -= azx_dev->period_wallclk;

	/* enable SIE */
	snd_hdac_chip_updatel(bus, INTCTL,
			      1 << azx_dev->index,
			      1 << azx_dev->index);
	/* set stripe control */
	if (azx_dev->stripe) {
		if (azx_dev->substream)
			stripe_ctl = snd_hdac_get_stream_stripe_ctl(bus, azx_dev->substream);
		else
			stripe_ctl = 0;
		snd_hdac_stream_updateb(azx_dev, SD_CTL_3B, SD_CTL_STRIPE_MASK,
					stripe_ctl);
	}
	/* set DMA start and interrupt mask */
	snd_hdac_stream_updateb(azx_dev, SD_CTL,
				0, SD_CTL_DMA_START | SD_INT_MASK);
	azx_dev->running = true;
}
EXPORT_SYMBOL_GPL(snd_hdac_stream_start);

/**
 * snd_hdac_stream_clear - stop a stream DMA
 * @azx_dev: HD-audio core stream to stop
 */
void snd_hdac_stream_clear(struct hdac_stream *azx_dev)
{
	snd_hdac_stream_updateb(azx_dev, SD_CTL,
				SD_CTL_DMA_START | SD_INT_MASK, 0);
	snd_hdac_stream_writeb(azx_dev, SD_STS, SD_INT_MASK); /* to be sure */
	if (azx_dev->stripe)
		snd_hdac_stream_updateb(azx_dev, SD_CTL_3B, SD_CTL_STRIPE_MASK, 0);
	azx_dev->running = false;
}
EXPORT_SYMBOL_GPL(snd_hdac_stream_clear);

/**
 * snd_hdac_stream_stop - stop a stream
 * @azx_dev: HD-audio core stream to stop
 *
 * Stop a stream DMA and disable stream interrupt
 */
void snd_hdac_stream_stop(struct hdac_stream *azx_dev)
{
	trace_snd_hdac_stream_stop(azx_dev->bus, azx_dev);

	snd_hdac_stream_clear(azx_dev);
	/* disable SIE */
	snd_hdac_chip_updatel(azx_dev->bus, INTCTL, 1 << azx_dev->index, 0);
}
EXPORT_SYMBOL_GPL(snd_hdac_stream_stop);

/**
 * snd_hdac_stream_reset - reset a stream
 * @azx_dev: HD-audio core stream to reset
 */
void snd_hdac_stream_reset(struct hdac_stream *azx_dev)
{
	unsigned char val;
	int timeout;
	int dma_run_state;

	snd_hdac_stream_clear(azx_dev);

	dma_run_state = snd_hdac_stream_readb(azx_dev, SD_CTL) & SD_CTL_DMA_START;

	snd_hdac_stream_updateb(azx_dev, SD_CTL, 0, SD_CTL_STREAM_RESET);
	udelay(3);
	timeout = 300;
	do {
		val = snd_hdac_stream_readb(azx_dev, SD_CTL) &
			SD_CTL_STREAM_RESET;
		if (val)
			break;
	} while (--timeout);

	if (azx_dev->bus->dma_stop_delay && dma_run_state)
		udelay(azx_dev->bus->dma_stop_delay);

	val &= ~SD_CTL_STREAM_RESET;
	snd_hdac_stream_writeb(azx_dev, SD_CTL, val);
	udelay(3);

	timeout = 300;
	/* waiting for hardware to report that the stream is out of reset */
	do {
		val = snd_hdac_stream_readb(azx_dev, SD_CTL) &
			SD_CTL_STREAM_RESET;
		if (!val)
			break;
	} while (--timeout);

	/* reset first position - may not be synced with hw at this time */
	if (azx_dev->posbuf)
		*azx_dev->posbuf = 0;
}
EXPORT_SYMBOL_GPL(snd_hdac_stream_reset);

/**
 * snd_hdac_stream_setup -  set up the SD for streaming
 * @azx_dev: HD-audio core stream to set up
 */
int snd_hdac_stream_setup(struct hdac_stream *azx_dev)
{
	struct hdac_bus *bus = azx_dev->bus;
	struct snd_pcm_runtime *runtime;
	unsigned int val;

	if (azx_dev->substream)
		runtime = azx_dev->substream->runtime;
	else
		runtime = NULL;
	/* make sure the run bit is zero for SD */
	snd_hdac_stream_clear(azx_dev);
	/* program the stream_tag */
	val = snd_hdac_stream_readl(azx_dev, SD_CTL);
	val = (val & ~SD_CTL_STREAM_TAG_MASK) |
		(azx_dev->stream_tag << SD_CTL_STREAM_TAG_SHIFT);
	if (!bus->snoop)
		val |= SD_CTL_TRAFFIC_PRIO;
	snd_hdac_stream_writel(azx_dev, SD_CTL, val);

	/* program the length of samples in cyclic buffer */
	snd_hdac_stream_writel(azx_dev, SD_CBL, azx_dev->bufsize);

	/* program the stream format */
	/* this value needs to be the same as the one programmed */
	snd_hdac_stream_writew(azx_dev, SD_FORMAT, azx_dev->format_val);

	/* program the stream LVI (last valid index) of the BDL */
	snd_hdac_stream_writew(azx_dev, SD_LVI, azx_dev->frags - 1);

	/* program the BDL address */
	/* lower BDL address */
	snd_hdac_stream_writel(azx_dev, SD_BDLPL, (u32)azx_dev->bdl.addr);
	/* upper BDL address */
	snd_hdac_stream_writel(azx_dev, SD_BDLPU,
			       upper_32_bits(azx_dev->bdl.addr));

	/* enable the position buffer */
	if (bus->use_posbuf && bus->posbuf.addr) {
		if (!(snd_hdac_chip_readl(bus, DPLBASE) & AZX_DPLBASE_ENABLE))
			snd_hdac_chip_writel(bus, DPLBASE,
				(u32)bus->posbuf.addr | AZX_DPLBASE_ENABLE);
	}

	/* set the interrupt enable bits in the descriptor control register */
	snd_hdac_stream_updatel(azx_dev, SD_CTL, 0, SD_INT_MASK);

	azx_dev->fifo_size = snd_hdac_stream_readw(azx_dev, SD_FIFOSIZE) + 1;

	/* when LPIB delay correction gives a small negative value,
	 * we ignore it; currently set the threshold statically to
	 * 64 frames
	 */
	if (runtime && runtime->period_size > 64)
		azx_dev->delay_negative_threshold =
			-frames_to_bytes(runtime, 64);
	else
		azx_dev->delay_negative_threshold = 0;

	/* wallclk has 24Mhz clock source */
	if (runtime)
		azx_dev->period_wallclk = (((runtime->period_size * 24000) /
				    runtime->rate) * 1000);

	return 0;
}
EXPORT_SYMBOL_GPL(snd_hdac_stream_setup);

/**
 * snd_hdac_stream_cleanup - cleanup a stream
 * @azx_dev: HD-audio core stream to clean up
 */
void snd_hdac_stream_cleanup(struct hdac_stream *azx_dev)
{
	snd_hdac_stream_writel(azx_dev, SD_BDLPL, 0);
	snd_hdac_stream_writel(azx_dev, SD_BDLPU, 0);
	snd_hdac_stream_writel(azx_dev, SD_CTL, 0);
	azx_dev->bufsize = 0;
	azx_dev->period_bytes = 0;
	azx_dev->format_val = 0;
}
EXPORT_SYMBOL_GPL(snd_hdac_stream_cleanup);

/**
 * snd_hdac_stream_assign - assign a stream for the PCM
 * @bus: HD-audio core bus
 * @substream: PCM substream to assign
 *
 * Look for an unused stream for the given PCM substream, assign it
 * and return the stream object.  If no stream is free, returns NULL.
 * The function tries to keep using the same stream object when it's used
 * beforehand.  Also, when bus->reverse_assign flag is set, the last free
 * or matching entry is returned.  This is needed for some strange codecs.
 */
struct hdac_stream *snd_hdac_stream_assign(struct hdac_bus *bus,
					   struct snd_pcm_substream *substream)
{
	struct hdac_stream *azx_dev;
	struct hdac_stream *res = NULL;

	/* make a non-zero unique key for the substream */
	int key = (substream->pcm->device << 16) | (substream->number << 2) |
		(substream->stream + 1);

	list_for_each_entry(azx_dev, &bus->stream_list, list) {
		if (azx_dev->direction != substream->stream)
			continue;
		if (azx_dev->opened)
			continue;
		if (azx_dev->assigned_key == key) {
			res = azx_dev;
			break;
		}
		if (!res || bus->reverse_assign)
			res = azx_dev;
	}
	if (res) {
		spin_lock_irq(&bus->reg_lock);
		res->opened = 1;
		res->running = 0;
		res->assigned_key = key;
		res->substream = substream;
		spin_unlock_irq(&bus->reg_lock);
	}
	return res;
}
EXPORT_SYMBOL_GPL(snd_hdac_stream_assign);

/**
 * snd_hdac_stream_release - release the assigned stream
 * @azx_dev: HD-audio core stream to release
 *
 * Release the stream that has been assigned by snd_hdac_stream_assign().
 */
void snd_hdac_stream_release(struct hdac_stream *azx_dev)
{
	struct hdac_bus *bus = azx_dev->bus;

	spin_lock_irq(&bus->reg_lock);
	azx_dev->opened = 0;
	azx_dev->running = 0;
	azx_dev->substream = NULL;
	spin_unlock_irq(&bus->reg_lock);
}
EXPORT_SYMBOL_GPL(snd_hdac_stream_release);

/**
 * snd_hdac_get_stream - return hdac_stream based on stream_tag and
 * direction
 *
 * @bus: HD-audio core bus
 * @dir: direction for the stream to be found
 * @stream_tag: stream tag for stream to be found
 */
struct hdac_stream *snd_hdac_get_stream(struct hdac_bus *bus,
					int dir, int stream_tag)
{
	struct hdac_stream *s;

	list_for_each_entry(s, &bus->stream_list, list) {
		if (s->direction == dir && s->stream_tag == stream_tag)
			return s;
	}

	return NULL;
}
EXPORT_SYMBOL_GPL(snd_hdac_get_stream);

/*
 * set up a BDL entry
 */
static int setup_bdle(struct hdac_bus *bus,
		      struct snd_dma_buffer *dmab,
		      struct hdac_stream *azx_dev, __le32 **bdlp,
		      int ofs, int size, int with_ioc)
{
	__le32 *bdl = *bdlp;

	while (size > 0) {
		dma_addr_t addr;
		int chunk;

		if (azx_dev->frags >= AZX_MAX_BDL_ENTRIES)
			return -EINVAL;

		addr = snd_sgbuf_get_addr(dmab, ofs);
		/* program the address field of the BDL entry */
		bdl[0] = cpu_to_le32((u32)addr);
		bdl[1] = cpu_to_le32(upper_32_bits(addr));
		/* program the size field of the BDL entry */
		chunk = snd_sgbuf_get_chunk_size(dmab, ofs, size);
		/* one BDLE cannot cross 4K boundary on CTHDA chips */
		if (bus->align_bdle_4k) {
			u32 remain = 0x1000 - (ofs & 0xfff);

			if (chunk > remain)
				chunk = remain;
		}
		bdl[2] = cpu_to_le32(chunk);
		/* program the IOC to enable interrupt
		 * only when the whole fragment is processed
		 */
		size -= chunk;
		bdl[3] = (size || !with_ioc) ? 0 : cpu_to_le32(0x01);
		bdl += 4;
		azx_dev->frags++;
		ofs += chunk;
	}
	*bdlp = bdl;
	return ofs;
}

/**
 * snd_hdac_stream_setup_periods - set up BDL entries
 * @azx_dev: HD-audio core stream to set up
 *
 * Set up the buffer descriptor table of the given stream based on the
 * period and buffer sizes of the assigned PCM substream.
 */
int snd_hdac_stream_setup_periods(struct hdac_stream *azx_dev)
{
	struct hdac_bus *bus = azx_dev->bus;
	struct snd_pcm_substream *substream = azx_dev->substream;
	struct snd_pcm_runtime *runtime = substream->runtime;
	__le32 *bdl;
	int i, ofs, periods, period_bytes;
	int pos_adj, pos_align;

	/* reset BDL address */
	snd_hdac_stream_writel(azx_dev, SD_BDLPL, 0);
	snd_hdac_stream_writel(azx_dev, SD_BDLPU, 0);

	period_bytes = azx_dev->period_bytes;
	periods = azx_dev->bufsize / period_bytes;

	/* program the initial BDL entries */
	bdl = (__le32 *)azx_dev->bdl.area;
	ofs = 0;
	azx_dev->frags = 0;

	pos_adj = bus->bdl_pos_adj;
	if (!azx_dev->no_period_wakeup && pos_adj > 0) {
		pos_align = pos_adj;
		pos_adj = DIV_ROUND_UP(pos_adj * runtime->rate, 48000);
		if (!pos_adj)
			pos_adj = pos_align;
		else
			pos_adj = roundup(pos_adj, pos_align);
		pos_adj = frames_to_bytes(runtime, pos_adj);
		if (pos_adj >= period_bytes) {
			dev_warn(bus->dev, "Too big adjustment %d\n",
				 pos_adj);
			pos_adj = 0;
		} else {
			ofs = setup_bdle(bus, snd_pcm_get_dma_buf(substream),
					 azx_dev,
					 &bdl, ofs, pos_adj, true);
			if (ofs < 0)
				goto error;
		}
	} else
		pos_adj = 0;

	for (i = 0; i < periods; i++) {
		if (i == periods - 1 && pos_adj)
			ofs = setup_bdle(bus, snd_pcm_get_dma_buf(substream),
					 azx_dev, &bdl, ofs,
					 period_bytes - pos_adj, 0);
		else
			ofs = setup_bdle(bus, snd_pcm_get_dma_buf(substream),
					 azx_dev, &bdl, ofs,
					 period_bytes,
					 !azx_dev->no_period_wakeup);
		if (ofs < 0)
			goto error;
	}
	return 0;

 error:
	dev_err(bus->dev, "Too many BDL entries: buffer=%d, period=%d\n",
		azx_dev->bufsize, period_bytes);
	return -EINVAL;
}
EXPORT_SYMBOL_GPL(snd_hdac_stream_setup_periods);

/**
 * snd_hdac_stream_set_params - set stream parameters
 * @azx_dev: HD-audio core stream for which parameters are to be set
 * @format_val: format value parameter
 *
 * Setup the HD-audio core stream parameters from substream of the stream
 * and passed format value
 */
int snd_hdac_stream_set_params(struct hdac_stream *azx_dev,
				 unsigned int format_val)
{

	unsigned int bufsize, period_bytes;
	struct snd_pcm_substream *substream = azx_dev->substream;
	struct snd_pcm_runtime *runtime;
	int err;

	if (!substream)
		return -EINVAL;
	runtime = substream->runtime;
	bufsize = snd_pcm_lib_buffer_bytes(substream);
	period_bytes = snd_pcm_lib_period_bytes(substream);

	if (bufsize != azx_dev->bufsize ||
	    period_bytes != azx_dev->period_bytes ||
	    format_val != azx_dev->format_val ||
	    runtime->no_period_wakeup != azx_dev->no_period_wakeup) {
		azx_dev->bufsize = bufsize;
		azx_dev->period_bytes = period_bytes;
		azx_dev->format_val = format_val;
		azx_dev->no_period_wakeup = runtime->no_period_wakeup;
		err = snd_hdac_stream_setup_periods(azx_dev);
		if (err < 0)
			return err;
	}
	return 0;
}
EXPORT_SYMBOL_GPL(snd_hdac_stream_set_params);

static u64 azx_cc_read(const struct cyclecounter *cc)
{
	struct hdac_stream *azx_dev = container_of(cc, struct hdac_stream, cc);

	return snd_hdac_chip_readl(azx_dev->bus, WALLCLK);
}

static void azx_timecounter_init(struct hdac_stream *azx_dev,
				 bool force, u64 last)
{
	struct timecounter *tc = &azx_dev->tc;
	struct cyclecounter *cc = &azx_dev->cc;
	u64 nsec;

	cc->read = azx_cc_read;
	cc->mask = CLOCKSOURCE_MASK(32);

	/*
	 * Converting from 24 MHz to ns means applying a 125/3 factor.
	 * To avoid any saturation issues in intermediate operations,
	 * the 125 factor is applied first. The division is applied
	 * last after reading the timecounter value.
	 * Applying the 1/3 factor as part of the multiplication
	 * requires at least 20 bits for a decent precision, however
	 * overflows occur after about 4 hours or less, not a option.
	 */

	cc->mult = 125; /* saturation after 195 years */
	cc->shift = 0;

	nsec = 0; /* audio time is elapsed time since trigger */
	timecounter_init(tc, cc, nsec);
	if (force) {
		/*
		 * force timecounter to use predefined value,
		 * used for synchronized starts
		 */
		tc->cycle_last = last;
	}
}

/**
 * snd_hdac_stream_timecounter_init - initialize time counter
 * @azx_dev: HD-audio core stream (master stream)
 * @streams: bit flags of streams to set up
 *
 * Initializes the time counter of streams marked by the bit flags (each
 * bit corresponds to the stream index).
 * The trigger timestamp of PCM substream assigned to the given stream is
 * updated accordingly, too.
 */
void snd_hdac_stream_timecounter_init(struct hdac_stream *azx_dev,
				      unsigned int streams)
{
	struct hdac_bus *bus = azx_dev->bus;
	struct snd_pcm_runtime *runtime = azx_dev->substream->runtime;
	struct hdac_stream *s;
	bool inited = false;
	u64 cycle_last = 0;
	int i = 0;

	list_for_each_entry(s, &bus->stream_list, list) {
		if (streams & (1 << i)) {
			azx_timecounter_init(s, inited, cycle_last);
			if (!inited) {
				inited = true;
				cycle_last = s->tc.cycle_last;
			}
		}
		i++;
	}

	snd_pcm_gettime(runtime, &runtime->trigger_tstamp);
	runtime->trigger_tstamp_latched = true;
}
EXPORT_SYMBOL_GPL(snd_hdac_stream_timecounter_init);

/**
 * snd_hdac_stream_sync_trigger - turn on/off stream sync register
 * @azx_dev: HD-audio core stream (master stream)
 * @set: true = set, false = clear
 * @streams: bit flags of streams to sync
 * @reg: the stream sync register address
 */
void snd_hdac_stream_sync_trigger(struct hdac_stream *azx_dev, bool set,
				  unsigned int streams, unsigned int reg)
{
	struct hdac_bus *bus = azx_dev->bus;
	unsigned int val;

	if (!reg)
		reg = AZX_REG_SSYNC;
	val = _snd_hdac_chip_readl(bus, reg);
	if (set)
		val |= streams;
	else
		val &= ~streams;
	_snd_hdac_chip_writel(bus, reg, val);
}
EXPORT_SYMBOL_GPL(snd_hdac_stream_sync_trigger);

/**
 * snd_hdac_stream_sync - sync with start/stop trigger operation
 * @azx_dev: HD-audio core stream (master stream)
 * @start: true = start, false = stop
 * @streams: bit flags of streams to sync
 *
 * For @start = true, wait until all FIFOs get ready.
 * For @start = false, wait until all RUN bits are cleared.
 */
void snd_hdac_stream_sync(struct hdac_stream *azx_dev, bool start,
			  unsigned int streams)
{
	struct hdac_bus *bus = azx_dev->bus;
	int i, nwait, timeout;
	struct hdac_stream *s;

	for (timeout = 5000; timeout; timeout--) {
		nwait = 0;
		i = 0;
		list_for_each_entry(s, &bus->stream_list, list) {
			if (!(streams & (1 << i++)))
				continue;

			if (start) {
				/* check FIFO gets ready */
				if (!(snd_hdac_stream_readb(s, SD_STS) &
				      SD_STS_FIFO_READY))
					nwait++;
			} else {
				/* check RUN bit is cleared */
				if (snd_hdac_stream_readb(s, SD_CTL) &
				    SD_CTL_DMA_START) {
					nwait++;
					/*
					 * Perform stream reset if DMA RUN
					 * bit not cleared within given timeout
					 */
					if (timeout == 1)
						snd_hdac_stream_reset(s);
				}
			}
		}
		if (!nwait)
			break;
		cpu_relax();
	}
}
EXPORT_SYMBOL_GPL(snd_hdac_stream_sync);

#ifdef CONFIG_SND_HDA_DSP_LOADER
/**
 * snd_hdac_dsp_prepare - prepare for DSP loading
 * @azx_dev: HD-audio core stream used for DSP loading
 * @format: HD-audio stream format
 * @byte_size: data chunk byte size
 * @bufp: allocated buffer
 *
 * Allocate the buffer for the given size and set up the given stream for
 * DSP loading.  Returns the stream tag (>= 0), or a negative error code.
 */
int snd_hdac_dsp_prepare(struct hdac_stream *azx_dev, unsigned int format,
			 unsigned int byte_size, struct snd_dma_buffer *bufp)
{
	struct hdac_bus *bus = azx_dev->bus;
	__le32 *bdl;
	int err;

	snd_hdac_dsp_lock(azx_dev);
	spin_lock_irq(&bus->reg_lock);
	if (azx_dev->running || azx_dev->locked) {
		spin_unlock_irq(&bus->reg_lock);
		err = -EBUSY;
		goto unlock;
	}
	azx_dev->locked = true;
	spin_unlock_irq(&bus->reg_lock);

	err = snd_dma_alloc_pages(SNDRV_DMA_TYPE_DEV_SG, bus->dev,
				  byte_size, bufp);
	if (err < 0)
		goto err_alloc;

	azx_dev->substream = NULL;
	azx_dev->bufsize = byte_size;
	azx_dev->period_bytes = byte_size;
	azx_dev->format_val = format;

	snd_hdac_stream_reset(azx_dev);

	/* reset BDL address */
	snd_hdac_stream_writel(azx_dev, SD_BDLPL, 0);
	snd_hdac_stream_writel(azx_dev, SD_BDLPU, 0);

	azx_dev->frags = 0;
	bdl = (__le32 *)azx_dev->bdl.area;
	err = setup_bdle(bus, bufp, azx_dev, &bdl, 0, byte_size, 0);
	if (err < 0)
		goto error;

	snd_hdac_stream_setup(azx_dev);
	snd_hdac_dsp_unlock(azx_dev);
	return azx_dev->stream_tag;

 error:
	snd_dma_free_pages(bufp);
 err_alloc:
	spin_lock_irq(&bus->reg_lock);
	azx_dev->locked = false;
	spin_unlock_irq(&bus->reg_lock);
 unlock:
	snd_hdac_dsp_unlock(azx_dev);
	return err;
}
EXPORT_SYMBOL_GPL(snd_hdac_dsp_prepare);

/**
 * snd_hdac_dsp_trigger - start / stop DSP loading
 * @azx_dev: HD-audio core stream used for DSP loading
 * @start: trigger start or stop
 */
void snd_hdac_dsp_trigger(struct hdac_stream *azx_dev, bool start)
{
	if (start)
		snd_hdac_stream_start(azx_dev, true);
	else
		snd_hdac_stream_stop(azx_dev);
}
EXPORT_SYMBOL_GPL(snd_hdac_dsp_trigger);

/**
 * snd_hdac_dsp_cleanup - clean up the stream from DSP loading to normal
 * @azx_dev: HD-audio core stream used for DSP loading
 * @dmab: buffer used by DSP loading
 */
void snd_hdac_dsp_cleanup(struct hdac_stream *azx_dev,
			  struct snd_dma_buffer *dmab)
{
	struct hdac_bus *bus = azx_dev->bus;

	if (!dmab->area || !azx_dev->locked)
		return;

	snd_hdac_dsp_lock(azx_dev);
	/* reset BDL address */
	snd_hdac_stream_writel(azx_dev, SD_BDLPL, 0);
	snd_hdac_stream_writel(azx_dev, SD_BDLPU, 0);
	snd_hdac_stream_writel(azx_dev, SD_CTL, 0);
	azx_dev->bufsize = 0;
	azx_dev->period_bytes = 0;
	azx_dev->format_val = 0;

	snd_dma_free_pages(dmab);
	dmab->area = NULL;

	spin_lock_irq(&bus->reg_lock);
	azx_dev->locked = false;
	spin_unlock_irq(&bus->reg_lock);
	snd_hdac_dsp_unlock(azx_dev);
}
EXPORT_SYMBOL_GPL(snd_hdac_dsp_cleanup);
#endif /* CONFIG_SND_HDA_DSP_LOADER */
