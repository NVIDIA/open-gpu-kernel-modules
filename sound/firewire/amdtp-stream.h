/* SPDX-License-Identifier: GPL-2.0 */
#ifndef SOUND_FIREWIRE_AMDTP_H_INCLUDED
#define SOUND_FIREWIRE_AMDTP_H_INCLUDED

#include <linux/err.h>
#include <linux/interrupt.h>
#include <linux/mutex.h>
#include <linux/sched.h>
#include <sound/asound.h>
#include "packets-buffer.h"

/**
 * enum cip_flags - describes details of the streaming protocol
 * @CIP_NONBLOCKING: In non-blocking mode, each packet contains
 *	sample_rate/8000 samples, with rounding up or down to adjust
 *	for clock skew and left-over fractional samples.  This should
 *	be used if supported by the device.
 * @CIP_BLOCKING: In blocking mode, each packet contains either zero or
 *	SYT_INTERVAL samples, with these two types alternating so that
 *	the overall sample rate comes out right.
 * @CIP_EMPTY_WITH_TAG0: Only for in-stream. Empty in-packets have TAG0.
 * @CIP_DBC_IS_END_EVENT: The value of dbc in an packet corresponds to the end
 * of event in the packet. Out of IEC 61883.
 * @CIP_WRONG_DBS: Only for in-stream. The value of dbs is wrong in in-packets.
 *	The value of data_block_quadlets is used instead of reported value.
 * @CIP_SKIP_DBC_ZERO_CHECK: Only for in-stream.  Packets with zero in dbc is
 *	skipped for detecting discontinuity.
 * @CIP_EMPTY_HAS_WRONG_DBC: Only for in-stream. The value of dbc in empty
 *	packet is wrong but the others are correct.
 * @CIP_JUMBO_PAYLOAD: Only for in-stream. The number of data blocks in an
 *	packet is larger than IEC 61883-6 defines. Current implementation
 *	allows 5 times as large as IEC 61883-6 defines.
 * @CIP_HEADER_WITHOUT_EOH: Only for in-stream. CIP Header doesn't include
 *	valid EOH.
 * @CIP_NO_HEADERS: a lack of headers in packets
 * @CIP_UNALIGHED_DBC: Only for in-stream. The value of dbc is not alighed to
 *	the value of current SYT_INTERVAL; e.g. initial value is not zero.
 */
enum cip_flags {
	CIP_NONBLOCKING		= 0x00,
	CIP_BLOCKING		= 0x01,
	CIP_EMPTY_WITH_TAG0	= 0x02,
	CIP_DBC_IS_END_EVENT	= 0x04,
	CIP_WRONG_DBS		= 0x08,
	CIP_SKIP_DBC_ZERO_CHECK	= 0x10,
	CIP_EMPTY_HAS_WRONG_DBC	= 0x20,
	CIP_JUMBO_PAYLOAD	= 0x40,
	CIP_HEADER_WITHOUT_EOH	= 0x80,
	CIP_NO_HEADER		= 0x100,
	CIP_UNALIGHED_DBC	= 0x200,
};

/**
 * enum cip_sfc - supported Sampling Frequency Codes (SFCs)
 * @CIP_SFC_32000:   32,000 data blocks
 * @CIP_SFC_44100:   44,100 data blocks
 * @CIP_SFC_48000:   48,000 data blocks
 * @CIP_SFC_88200:   88,200 data blocks
 * @CIP_SFC_96000:   96,000 data blocks
 * @CIP_SFC_176400: 176,400 data blocks
 * @CIP_SFC_192000: 192,000 data blocks
 * @CIP_SFC_COUNT: the number of supported SFCs
 *
 * These values are used to show nominal Sampling Frequency Code in
 * Format Dependent Field (FDF) of AMDTP packet header. In IEC 61883-6:2002,
 * this code means the number of events per second. Actually the code
 * represents the number of data blocks transferred per second in an AMDTP
 * stream.
 *
 * In IEC 61883-6:2005, some extensions were added to support more types of
 * data such as 'One Bit LInear Audio', therefore the meaning of SFC became
 * different depending on the types.
 *
 * Currently our implementation is compatible with IEC 61883-6:2002.
 */
enum cip_sfc {
	CIP_SFC_32000  = 0,
	CIP_SFC_44100  = 1,
	CIP_SFC_48000  = 2,
	CIP_SFC_88200  = 3,
	CIP_SFC_96000  = 4,
	CIP_SFC_176400 = 5,
	CIP_SFC_192000 = 6,
	CIP_SFC_COUNT
};

struct fw_unit;
struct fw_iso_context;
struct snd_pcm_substream;
struct snd_pcm_runtime;

enum amdtp_stream_direction {
	AMDTP_OUT_STREAM = 0,
	AMDTP_IN_STREAM
};

struct pkt_desc {
	u32 cycle;
	u32 syt;
	unsigned int data_blocks;
	unsigned int data_block_counter;
	__be32 *ctx_payload;
};

struct amdtp_stream;
typedef unsigned int (*amdtp_stream_process_ctx_payloads_t)(
						struct amdtp_stream *s,
						const struct pkt_desc *desc,
						unsigned int packets,
						struct snd_pcm_substream *pcm);

struct amdtp_domain;
struct amdtp_stream {
	struct fw_unit *unit;
	enum cip_flags flags;
	enum amdtp_stream_direction direction;
	struct mutex mutex;

	/* For packet processing. */
	struct fw_iso_context *context;
	struct iso_packets_buffer buffer;
	unsigned int queue_size;
	int packet_index;
	struct pkt_desc *pkt_descs;
	int tag;
	union {
		struct {
			unsigned int ctx_header_size;

			// limit for payload of iso packet.
			unsigned int max_ctx_payload_length;

			// For quirks of CIP headers.
			// Fixed interval of dbc between previos/current
			// packets.
			unsigned int dbc_interval;
		} tx;
		struct {
			// To calculate CIP data blocks and tstamp.
			unsigned int transfer_delay;
			unsigned int seq_index;

			// To generate CIP header.
			unsigned int fdf;
			int syt_override;

			// To generate constant hardware IRQ.
			unsigned int event_count;
			unsigned int events_per_period;
		} rx;
	} ctx_data;

	/* For CIP headers. */
	unsigned int source_node_id_field;
	unsigned int data_block_quadlets;
	unsigned int data_block_counter;
	unsigned int sph;
	unsigned int fmt;

	/* Internal flags. */
	enum cip_sfc sfc;
	unsigned int syt_interval;

	/* For a PCM substream processing. */
	struct snd_pcm_substream *pcm;
	struct work_struct period_work;
	snd_pcm_uframes_t pcm_buffer_pointer;
	unsigned int pcm_period_pointer;

	/* To wait for first packet. */
	bool callbacked;
	wait_queue_head_t callback_wait;
	u32 start_cycle;

	/* For backends to process data blocks. */
	void *protocol;
	amdtp_stream_process_ctx_payloads_t process_ctx_payloads;

	// For domain.
	int channel;
	int speed;
	struct list_head list;
	struct amdtp_domain *domain;
};

int amdtp_stream_init(struct amdtp_stream *s, struct fw_unit *unit,
		      enum amdtp_stream_direction dir, enum cip_flags flags,
		      unsigned int fmt,
		      amdtp_stream_process_ctx_payloads_t process_ctx_payloads,
		      unsigned int protocol_size);
void amdtp_stream_destroy(struct amdtp_stream *s);

int amdtp_stream_set_parameters(struct amdtp_stream *s, unsigned int rate,
				unsigned int data_block_quadlets);
unsigned int amdtp_stream_get_max_payload(struct amdtp_stream *s);

void amdtp_stream_update(struct amdtp_stream *s);

int amdtp_stream_add_pcm_hw_constraints(struct amdtp_stream *s,
					struct snd_pcm_runtime *runtime);

void amdtp_stream_pcm_prepare(struct amdtp_stream *s);
void amdtp_stream_pcm_abort(struct amdtp_stream *s);

extern const unsigned int amdtp_syt_intervals[CIP_SFC_COUNT];
extern const unsigned int amdtp_rate_table[CIP_SFC_COUNT];

/**
 * amdtp_stream_running - check stream is running or not
 * @s: the AMDTP stream
 *
 * If this function returns true, the stream is running.
 */
static inline bool amdtp_stream_running(struct amdtp_stream *s)
{
	return !IS_ERR(s->context);
}

/**
 * amdtp_streaming_error - check for streaming error
 * @s: the AMDTP stream
 *
 * If this function returns true, the stream's packet queue has stopped due to
 * an asynchronous error.
 */
static inline bool amdtp_streaming_error(struct amdtp_stream *s)
{
	return s->packet_index < 0;
}

/**
 * amdtp_stream_pcm_running - check PCM substream is running or not
 * @s: the AMDTP stream
 *
 * If this function returns true, PCM substream in the AMDTP stream is running.
 */
static inline bool amdtp_stream_pcm_running(struct amdtp_stream *s)
{
	return !!s->pcm;
}

/**
 * amdtp_stream_pcm_trigger - start/stop playback from a PCM device
 * @s: the AMDTP stream
 * @pcm: the PCM device to be started, or %NULL to stop the current device
 *
 * Call this function on a running isochronous stream to enable the actual
 * transmission of PCM data.  This function should be called from the PCM
 * device's .trigger callback.
 */
static inline void amdtp_stream_pcm_trigger(struct amdtp_stream *s,
					    struct snd_pcm_substream *pcm)
{
	WRITE_ONCE(s->pcm, pcm);
}

static inline bool cip_sfc_is_base_44100(enum cip_sfc sfc)
{
	return sfc & 1;
}

/**
 * amdtp_stream_wait_callback - sleep till callbacked or timeout
 * @s: the AMDTP stream
 * @timeout: msec till timeout
 *
 * If this function return false, the AMDTP stream should be stopped.
 */
static inline bool amdtp_stream_wait_callback(struct amdtp_stream *s,
					      unsigned int timeout)
{
	return wait_event_timeout(s->callback_wait,
				  s->callbacked,
				  msecs_to_jiffies(timeout)) > 0;
}

struct seq_desc {
	unsigned int syt_offset;
	unsigned int data_blocks;
};

struct amdtp_domain {
	struct list_head streams;

	unsigned int events_per_period;
	unsigned int events_per_buffer;

	struct amdtp_stream *irq_target;

	struct seq_desc *seq_descs;
	unsigned int seq_size;
	unsigned int seq_tail;

	unsigned int data_block_state;
	unsigned int syt_offset_state;
	unsigned int last_syt_offset;
};

int amdtp_domain_init(struct amdtp_domain *d);
void amdtp_domain_destroy(struct amdtp_domain *d);

int amdtp_domain_add_stream(struct amdtp_domain *d, struct amdtp_stream *s,
			    int channel, int speed);

int amdtp_domain_start(struct amdtp_domain *d, unsigned int ir_delay_cycle);
void amdtp_domain_stop(struct amdtp_domain *d);

static inline int amdtp_domain_set_events_per_period(struct amdtp_domain *d,
						unsigned int events_per_period,
						unsigned int events_per_buffer)
{
	d->events_per_period = events_per_period;
	d->events_per_buffer = events_per_buffer;

	return 0;
}

unsigned long amdtp_domain_stream_pcm_pointer(struct amdtp_domain *d,
					      struct amdtp_stream *s);
int amdtp_domain_stream_pcm_ack(struct amdtp_domain *d, struct amdtp_stream *s);

#endif
