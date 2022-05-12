// SPDX-License-Identifier: GPL-2.0-or-later
/* 
 *  Copyright 10/16/2005 Tilman Kranz <tilde@tk-sls.de>
 *  Creative Audio MIDI, for the CA0106 Driver
 *  Version: 0.0.1
 *
 *  Changelog:
 *    Implementation is based on mpu401 and emu10k1x and
 *    tested with ca0106.
 *    mpu401: Copyright (c) by Jaroslav Kysela <perex@perex.cz>
 *    emu10k1x: Copyright (c) by Francisco Moraes <fmoraes@nc.rr.com>
 */

#include <linux/spinlock.h>
#include <sound/core.h>
#include <sound/rawmidi.h>

#include "ca_midi.h"

#define ca_midi_write_data(midi, data)	midi->write(midi, data, 0)
#define ca_midi_write_cmd(midi, data)	midi->write(midi, data, 1)
#define ca_midi_read_data(midi)		midi->read(midi, 0)
#define ca_midi_read_stat(midi)		midi->read(midi, 1)
#define ca_midi_input_avail(midi)	(!(ca_midi_read_stat(midi) & midi->input_avail))
#define ca_midi_output_ready(midi)	(!(ca_midi_read_stat(midi) & midi->output_ready))

static void ca_midi_clear_rx(struct snd_ca_midi *midi)
{
	int timeout = 100000;
	for (; timeout > 0 && ca_midi_input_avail(midi); timeout--)
		ca_midi_read_data(midi);
#ifdef CONFIG_SND_DEBUG
	if (timeout <= 0)
		pr_err("ca_midi_clear_rx: timeout (status = 0x%x)\n",
			   ca_midi_read_stat(midi));
#endif
}

static void ca_midi_interrupt(struct snd_ca_midi *midi, unsigned int status)
{
	unsigned char byte;

	if (midi->rmidi == NULL) {
		midi->interrupt_disable(midi,midi->tx_enable | midi->rx_enable);
		return;
	}

	spin_lock(&midi->input_lock);
	if ((status & midi->ipr_rx) && ca_midi_input_avail(midi)) {
		if (!(midi->midi_mode & CA_MIDI_MODE_INPUT)) {
			ca_midi_clear_rx(midi);
		} else {
			byte = ca_midi_read_data(midi);
			if(midi->substream_input)
				snd_rawmidi_receive(midi->substream_input, &byte, 1);


		}
	}
	spin_unlock(&midi->input_lock);

	spin_lock(&midi->output_lock);
	if ((status & midi->ipr_tx) && ca_midi_output_ready(midi)) {
		if (midi->substream_output &&
		    snd_rawmidi_transmit(midi->substream_output, &byte, 1) == 1) {
			ca_midi_write_data(midi, byte);
		} else {
			midi->interrupt_disable(midi,midi->tx_enable);
		}
	}
	spin_unlock(&midi->output_lock);

}

static void ca_midi_cmd(struct snd_ca_midi *midi, unsigned char cmd, int ack)
{
	unsigned long flags;
	int timeout, ok;

	spin_lock_irqsave(&midi->input_lock, flags);
	ca_midi_write_data(midi, 0x00);
	/* ca_midi_clear_rx(midi); */

	ca_midi_write_cmd(midi, cmd);
	if (ack) {
		ok = 0;
		timeout = 10000;
		while (!ok && timeout-- > 0) {
			if (ca_midi_input_avail(midi)) {
				if (ca_midi_read_data(midi) == midi->ack)
					ok = 1;
			}
		}
		if (!ok && ca_midi_read_data(midi) == midi->ack)
			ok = 1;
	} else {
		ok = 1;
	}
	spin_unlock_irqrestore(&midi->input_lock, flags);
	if (!ok)
		pr_err("ca_midi_cmd: 0x%x failed at 0x%x (status = 0x%x, data = 0x%x)!!!\n",
			   cmd,
			   midi->get_dev_id_port(midi->dev_id),
			   ca_midi_read_stat(midi),
			   ca_midi_read_data(midi));
}

static int ca_midi_input_open(struct snd_rawmidi_substream *substream)
{
	struct snd_ca_midi *midi = substream->rmidi->private_data;
	unsigned long flags;
	
	if (snd_BUG_ON(!midi->dev_id))
		return -ENXIO;
	spin_lock_irqsave(&midi->open_lock, flags);
	midi->midi_mode |= CA_MIDI_MODE_INPUT;
	midi->substream_input = substream;
	if (!(midi->midi_mode & CA_MIDI_MODE_OUTPUT)) {
		spin_unlock_irqrestore(&midi->open_lock, flags);
		ca_midi_cmd(midi, midi->reset, 1);
		ca_midi_cmd(midi, midi->enter_uart, 1);
	} else {
		spin_unlock_irqrestore(&midi->open_lock, flags);
	}
	return 0;
}

static int ca_midi_output_open(struct snd_rawmidi_substream *substream)
{
	struct snd_ca_midi *midi = substream->rmidi->private_data;
	unsigned long flags;

	if (snd_BUG_ON(!midi->dev_id))
		return -ENXIO;
	spin_lock_irqsave(&midi->open_lock, flags);
	midi->midi_mode |= CA_MIDI_MODE_OUTPUT;
	midi->substream_output = substream;
	if (!(midi->midi_mode & CA_MIDI_MODE_INPUT)) {
		spin_unlock_irqrestore(&midi->open_lock, flags);
		ca_midi_cmd(midi, midi->reset, 1);
		ca_midi_cmd(midi, midi->enter_uart, 1);
	} else {
		spin_unlock_irqrestore(&midi->open_lock, flags);
	}
	return 0;
}

static int ca_midi_input_close(struct snd_rawmidi_substream *substream)
{
	struct snd_ca_midi *midi = substream->rmidi->private_data;
	unsigned long flags;

	if (snd_BUG_ON(!midi->dev_id))
		return -ENXIO;
	spin_lock_irqsave(&midi->open_lock, flags);
	midi->interrupt_disable(midi,midi->rx_enable);
	midi->midi_mode &= ~CA_MIDI_MODE_INPUT;
	midi->substream_input = NULL;
	if (!(midi->midi_mode & CA_MIDI_MODE_OUTPUT)) {
		spin_unlock_irqrestore(&midi->open_lock, flags);
		ca_midi_cmd(midi, midi->reset, 0);
	} else {
		spin_unlock_irqrestore(&midi->open_lock, flags);
	}
	return 0;
}

static int ca_midi_output_close(struct snd_rawmidi_substream *substream)
{
	struct snd_ca_midi *midi = substream->rmidi->private_data;
	unsigned long flags;

	if (snd_BUG_ON(!midi->dev_id))
		return -ENXIO;
	
	spin_lock_irqsave(&midi->open_lock, flags);

	midi->interrupt_disable(midi,midi->tx_enable);
	midi->midi_mode &= ~CA_MIDI_MODE_OUTPUT;
	midi->substream_output = NULL;
	
	if (!(midi->midi_mode & CA_MIDI_MODE_INPUT)) {
		spin_unlock_irqrestore(&midi->open_lock, flags);
		ca_midi_cmd(midi, midi->reset, 0);
	} else {
		spin_unlock_irqrestore(&midi->open_lock, flags);
	}
	return 0;
}

static void ca_midi_input_trigger(struct snd_rawmidi_substream *substream, int up)
{
	struct snd_ca_midi *midi = substream->rmidi->private_data;

	if (snd_BUG_ON(!midi->dev_id))
		return;

	if (up) {
		midi->interrupt_enable(midi,midi->rx_enable);
	} else {
		midi->interrupt_disable(midi, midi->rx_enable);
	}
}

static void ca_midi_output_trigger(struct snd_rawmidi_substream *substream, int up)
{
	struct snd_ca_midi *midi = substream->rmidi->private_data;
	unsigned long flags;

	if (snd_BUG_ON(!midi->dev_id))
		return;

	if (up) {
		int max = 4;
		unsigned char byte;

		spin_lock_irqsave(&midi->output_lock, flags);
	
		/* try to send some amount of bytes here before interrupts */
		while (max > 0) {
			if (ca_midi_output_ready(midi)) {
				if (!(midi->midi_mode & CA_MIDI_MODE_OUTPUT) ||
				    snd_rawmidi_transmit(substream, &byte, 1) != 1) {
					/* no more data */
					spin_unlock_irqrestore(&midi->output_lock, flags);
					return;
				}
				ca_midi_write_data(midi, byte);
				max--;
			} else {
				break;
			}
		}

		spin_unlock_irqrestore(&midi->output_lock, flags);
		midi->interrupt_enable(midi,midi->tx_enable);

	} else {
		midi->interrupt_disable(midi,midi->tx_enable);
	}
}

static const struct snd_rawmidi_ops ca_midi_output =
{
	.open =		ca_midi_output_open,
	.close =	ca_midi_output_close,
	.trigger =	ca_midi_output_trigger,
};

static const struct snd_rawmidi_ops ca_midi_input =
{
	.open =		ca_midi_input_open,
	.close =	ca_midi_input_close,
	.trigger =	ca_midi_input_trigger,
};

static void ca_midi_free(struct snd_ca_midi *midi)
{
	midi->interrupt = NULL;
	midi->interrupt_enable = NULL;
	midi->interrupt_disable = NULL;
	midi->read = NULL;
	midi->write = NULL;
	midi->get_dev_id_card = NULL;
	midi->get_dev_id_port = NULL;
	midi->rmidi = NULL;
}

static void ca_rmidi_free(struct snd_rawmidi *rmidi)
{
	ca_midi_free(rmidi->private_data);
}

int ca_midi_init(void *dev_id, struct snd_ca_midi *midi, int device, char *name)
{
	struct snd_rawmidi *rmidi;
	int err;

	if ((err = snd_rawmidi_new(midi->get_dev_id_card(midi->dev_id), name, device, 1, 1, &rmidi)) < 0)
		return err;

	midi->dev_id = dev_id;
	midi->interrupt = ca_midi_interrupt;

	spin_lock_init(&midi->open_lock);
	spin_lock_init(&midi->input_lock);
	spin_lock_init(&midi->output_lock);

	strcpy(rmidi->name, name);
	snd_rawmidi_set_ops(rmidi, SNDRV_RAWMIDI_STREAM_OUTPUT, &ca_midi_output);
	snd_rawmidi_set_ops(rmidi, SNDRV_RAWMIDI_STREAM_INPUT, &ca_midi_input);
	rmidi->info_flags |= SNDRV_RAWMIDI_INFO_OUTPUT |
	                     SNDRV_RAWMIDI_INFO_INPUT |
	                     SNDRV_RAWMIDI_INFO_DUPLEX;
	rmidi->private_data = midi;
	rmidi->private_free = ca_rmidi_free;
	
	midi->rmidi = rmidi;
	return 0;
}

