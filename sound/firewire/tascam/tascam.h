/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * tascam.h - a part of driver for TASCAM FireWire series
 *
 * Copyright (c) 2015 Takashi Sakamoto
 */

#ifndef SOUND_TASCAM_H_INCLUDED
#define SOUND_TASCAM_H_INCLUDED

#include <linux/device.h>
#include <linux/firewire.h>
#include <linux/firewire-constants.h>
#include <linux/module.h>
#include <linux/mod_devicetable.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/compat.h>
#include <linux/sched/signal.h>

#include <sound/core.h>
#include <sound/initval.h>
#include <sound/info.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/firewire.h>
#include <sound/hwdep.h>
#include <sound/rawmidi.h>

#include "../lib.h"
#include "../amdtp-stream.h"
#include "../iso-resources.h"

struct snd_tscm_spec {
	const char *const name;
	bool has_adat;
	bool has_spdif;
	unsigned int pcm_capture_analog_channels;
	unsigned int pcm_playback_analog_channels;
	unsigned int midi_capture_ports;
	unsigned int midi_playback_ports;
};

#define TSCM_MIDI_IN_PORT_MAX	4
#define TSCM_MIDI_OUT_PORT_MAX	4

struct snd_fw_async_midi_port {
	struct fw_device *parent;
	struct work_struct work;
	bool idling;
	ktime_t next_ktime;
	bool error;

	struct fw_transaction transaction;

	u8 buf[4];
	u8 running_status;
	bool on_sysex;

	struct snd_rawmidi_substream *substream;
	int consume_bytes;
};

#define SND_TSCM_QUEUE_COUNT	16

struct snd_tscm {
	struct snd_card *card;
	struct fw_unit *unit;

	struct mutex mutex;
	spinlock_t lock;

	bool registered;
	struct delayed_work dwork;
	const struct snd_tscm_spec *spec;

	struct fw_iso_resources tx_resources;
	struct fw_iso_resources rx_resources;
	struct amdtp_stream tx_stream;
	struct amdtp_stream rx_stream;
	unsigned int substreams_counter;

	int dev_lock_count;
	bool dev_lock_changed;
	wait_queue_head_t hwdep_wait;

	/* For MIDI message incoming transactions. */
	struct fw_address_handler async_handler;
	struct snd_rawmidi_substream *tx_midi_substreams[TSCM_MIDI_IN_PORT_MAX];

	/* For MIDI message outgoing transactions. */
	struct snd_fw_async_midi_port out_ports[TSCM_MIDI_OUT_PORT_MAX];

	// A cache of status information in tx isoc packets.
	__be32 state[SNDRV_FIREWIRE_TASCAM_STATE_COUNT];
	struct snd_hwdep *hwdep;
	struct snd_firewire_tascam_change queue[SND_TSCM_QUEUE_COUNT];
	unsigned int pull_pos;
	unsigned int push_pos;

	struct amdtp_domain domain;
};

#define TSCM_ADDR_BASE			0xffff00000000ull

#define TSCM_OFFSET_FIRMWARE_REGISTER	0x0000
#define TSCM_OFFSET_FIRMWARE_FPGA	0x0004
#define TSCM_OFFSET_FIRMWARE_ARM	0x0008
#define TSCM_OFFSET_FIRMWARE_HW		0x000c

#define TSCM_OFFSET_ISOC_TX_CH		0x0200
#define TSCM_OFFSET_UNKNOWN		0x0204
#define TSCM_OFFSET_START_STREAMING	0x0208
#define TSCM_OFFSET_ISOC_RX_CH		0x020c
#define TSCM_OFFSET_ISOC_RX_ON		0x0210	/* Little conviction. */
#define TSCM_OFFSET_TX_PCM_CHANNELS	0x0214
#define TSCM_OFFSET_RX_PCM_CHANNELS	0x0218
#define TSCM_OFFSET_MULTIPLEX_MODE	0x021c
#define TSCM_OFFSET_ISOC_TX_ON		0x0220
/* Unknown				0x0224 */
#define TSCM_OFFSET_CLOCK_STATUS	0x0228
#define TSCM_OFFSET_SET_OPTION		0x022c

#define TSCM_OFFSET_MIDI_TX_ON		0x0300
#define TSCM_OFFSET_MIDI_TX_ADDR_HI	0x0304
#define TSCM_OFFSET_MIDI_TX_ADDR_LO	0x0308

#define TSCM_OFFSET_LED_POWER		0x0404

#define TSCM_OFFSET_MIDI_RX_QUAD	0x4000

// Although FE-8 supports the above registers, it has no I/O interfaces for
// audio samples and music messages. Otherwise it supports another notification
// for status and control message as well as LED brightening. The message
// consists of quadlet-aligned data up to 32 quadlets. The first byte of message
// is fixed to 0x40. The second byte is between 0x00 to 0x1f and represent each
// control:
//   fader:	0x00-0x07
//   button:	0x0d, 0x0e
//   knob:	0x14-0x1b
//   sensing:	0x0b
//
// The rest two bytes represent state of the controls; e.g. current value for
// fader and knob, bitmasks for button and sensing.
// Just after turning on, 32 quadlets messages with 0x00-0x1f are immediately
// sent in one transaction. After, several quadlets are sent in one transaction.
//
// TSCM_OFFSET_FE8_CTL_TX_ON		0x0310
// TSCM_OFFSET_FE8_CTL_TX_ADDR_HI	0x0314
// TSCM_OFFSET_FE8_CTL_TX_ADDR_LO	0x0318

enum snd_tscm_clock {
	SND_TSCM_CLOCK_INTERNAL = 0,
	SND_TSCM_CLOCK_WORD	= 1,
	SND_TSCM_CLOCK_SPDIF	= 2,
	SND_TSCM_CLOCK_ADAT	= 3,
};

int amdtp_tscm_init(struct amdtp_stream *s, struct fw_unit *unit,
		  enum amdtp_stream_direction dir, unsigned int pcm_channels);
int amdtp_tscm_set_parameters(struct amdtp_stream *s, unsigned int rate);
int amdtp_tscm_add_pcm_hw_constraints(struct amdtp_stream *s,
				      struct snd_pcm_runtime *runtime);

int snd_tscm_stream_get_rate(struct snd_tscm *tscm, unsigned int *rate);
int snd_tscm_stream_get_clock(struct snd_tscm *tscm,
			      enum snd_tscm_clock *clock);
int snd_tscm_stream_init_duplex(struct snd_tscm *tscm);
void snd_tscm_stream_update_duplex(struct snd_tscm *tscm);
void snd_tscm_stream_destroy_duplex(struct snd_tscm *tscm);
int snd_tscm_stream_reserve_duplex(struct snd_tscm *tscm, unsigned int rate,
				   unsigned int frames_per_period,
				   unsigned int frames_per_buffer);
int snd_tscm_stream_start_duplex(struct snd_tscm *tscm, unsigned int rate);
void snd_tscm_stream_stop_duplex(struct snd_tscm *tscm);

void snd_tscm_stream_lock_changed(struct snd_tscm *tscm);
int snd_tscm_stream_lock_try(struct snd_tscm *tscm);
void snd_tscm_stream_lock_release(struct snd_tscm *tscm);

void snd_fw_async_midi_port_init(struct snd_fw_async_midi_port *port);

static inline void
snd_fw_async_midi_port_run(struct snd_fw_async_midi_port *port,
			   struct snd_rawmidi_substream *substream)
{
	if (!port->error) {
		port->substream = substream;
		schedule_work(&port->work);
	}
}

static inline void
snd_fw_async_midi_port_finish(struct snd_fw_async_midi_port *port)
{
	port->substream = NULL;
	cancel_work_sync(&port->work);
	port->error = false;
}

int snd_tscm_transaction_register(struct snd_tscm *tscm);
int snd_tscm_transaction_reregister(struct snd_tscm *tscm);
void snd_tscm_transaction_unregister(struct snd_tscm *tscm);

void snd_tscm_proc_init(struct snd_tscm *tscm);

int snd_tscm_create_pcm_devices(struct snd_tscm *tscm);

int snd_tscm_create_midi_devices(struct snd_tscm *tscm);

int snd_tscm_create_hwdep_device(struct snd_tscm *tscm);

#endif
