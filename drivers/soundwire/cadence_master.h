/* SPDX-License-Identifier: (GPL-2.0 OR BSD-3-Clause) */
/* Copyright(c) 2015-17 Intel Corporation. */
#include <sound/soc.h>

#ifndef __SDW_CADENCE_H
#define __SDW_CADENCE_H

#define SDW_CADENCE_GSYNC_KHZ		4 /* 4 kHz */
#define SDW_CADENCE_GSYNC_HZ		(SDW_CADENCE_GSYNC_KHZ * 1000)

/**
 * struct sdw_cdns_pdi: PDI (Physical Data Interface) instance
 *
 * @num: pdi number
 * @intel_alh_id: link identifier
 * @l_ch_num: low channel for PDI
 * @h_ch_num: high channel for PDI
 * @ch_count: total channel count for PDI
 * @dir: data direction
 * @type: stream type, PDM or PCM
 */
struct sdw_cdns_pdi {
	int num;
	int intel_alh_id;
	int l_ch_num;
	int h_ch_num;
	int ch_count;
	enum sdw_data_direction dir;
	enum sdw_stream_type type;
};

/**
 * struct sdw_cdns_streams: Cadence stream data structure
 *
 * @num_bd: number of bidirectional streams
 * @num_in: number of input streams
 * @num_out: number of output streams
 * @num_ch_bd: number of bidirectional stream channels
 * @num_ch_bd: number of input stream channels
 * @num_ch_bd: number of output stream channels
 * @num_pdi: total number of PDIs
 * @bd: bidirectional streams
 * @in: input streams
 * @out: output streams
 */
struct sdw_cdns_streams {
	unsigned int num_bd;
	unsigned int num_in;
	unsigned int num_out;
	unsigned int num_ch_bd;
	unsigned int num_ch_in;
	unsigned int num_ch_out;
	unsigned int num_pdi;
	struct sdw_cdns_pdi *bd;
	struct sdw_cdns_pdi *in;
	struct sdw_cdns_pdi *out;
};

/**
 * struct sdw_cdns_stream_config: stream configuration
 *
 * @pcm_bd: number of bidirectional PCM streams supported
 * @pcm_in: number of input PCM streams supported
 * @pcm_out: number of output PCM streams supported
 * @pdm_bd: number of bidirectional PDM streams supported
 * @pdm_in: number of input PDM streams supported
 * @pdm_out: number of output PDM streams supported
 */
struct sdw_cdns_stream_config {
	unsigned int pcm_bd;
	unsigned int pcm_in;
	unsigned int pcm_out;
	unsigned int pdm_bd;
	unsigned int pdm_in;
	unsigned int pdm_out;
};

/**
 * struct sdw_cdns_dma_data: Cadence DMA data
 *
 * @name: SoundWire stream name
 * @stream: stream runtime
 * @pdi: PDI used for this dai
 * @bus: Bus handle
 * @stream_type: Stream type
 * @link_id: Master link id
 * @hw_params: hw_params to be applied in .prepare step
 * @suspended: status set when suspended, to be used in .prepare
 */
struct sdw_cdns_dma_data {
	char *name;
	struct sdw_stream_runtime *stream;
	struct sdw_cdns_pdi *pdi;
	struct sdw_bus *bus;
	enum sdw_stream_type stream_type;
	int link_id;
	struct snd_pcm_hw_params *hw_params;
	bool suspended;
};

/**
 * struct sdw_cdns - Cadence driver context
 * @dev: Linux device
 * @bus: Bus handle
 * @instance: instance number
 * @response_buf: SoundWire response buffer
 * @tx_complete: Tx completion
 * @defer: Defer pointer
 * @ports: Data ports
 * @num_ports: Total number of data ports
 * @pcm: PCM streams
 * @pdm: PDM streams
 * @registers: Cadence registers
 * @link_up: Link status
 * @msg_count: Messages sent on bus
 */
struct sdw_cdns {
	struct device *dev;
	struct sdw_bus bus;
	unsigned int instance;

	u32 response_buf[0x80];
	struct completion tx_complete;
	struct sdw_defer *defer;

	struct sdw_cdns_port *ports;
	int num_ports;

	struct sdw_cdns_streams pcm;
	struct sdw_cdns_streams pdm;

	void __iomem *registers;

	bool link_up;
	unsigned int msg_count;
	bool interrupt_enabled;

	struct work_struct work;

	struct list_head list;
};

#define bus_to_cdns(_bus) container_of(_bus, struct sdw_cdns, bus)

/* Exported symbols */

int sdw_cdns_probe(struct sdw_cdns *cdns);
extern struct sdw_master_ops sdw_cdns_master_ops;

irqreturn_t sdw_cdns_irq(int irq, void *dev_id);
irqreturn_t sdw_cdns_thread(int irq, void *dev_id);

int sdw_cdns_init(struct sdw_cdns *cdns);
int sdw_cdns_pdi_init(struct sdw_cdns *cdns,
		      struct sdw_cdns_stream_config config);
int sdw_cdns_exit_reset(struct sdw_cdns *cdns);
int sdw_cdns_enable_interrupt(struct sdw_cdns *cdns, bool state);

bool sdw_cdns_is_clock_stop(struct sdw_cdns *cdns);
int sdw_cdns_clock_stop(struct sdw_cdns *cdns, bool block_wake);
int sdw_cdns_clock_restart(struct sdw_cdns *cdns, bool bus_reset);

#ifdef CONFIG_DEBUG_FS
void sdw_cdns_debugfs_init(struct sdw_cdns *cdns, struct dentry *root);
#endif

struct sdw_cdns_pdi *sdw_cdns_alloc_pdi(struct sdw_cdns *cdns,
					struct sdw_cdns_streams *stream,
					u32 ch, u32 dir, int dai_id);
void sdw_cdns_config_stream(struct sdw_cdns *cdns,
			    u32 ch, u32 dir, struct sdw_cdns_pdi *pdi);

enum sdw_command_response
cdns_reset_page_addr(struct sdw_bus *bus, unsigned int dev_num);

enum sdw_command_response
cdns_xfer_msg(struct sdw_bus *bus, struct sdw_msg *msg);

enum sdw_command_response
cdns_xfer_msg_defer(struct sdw_bus *bus,
		    struct sdw_msg *msg, struct sdw_defer *defer);

enum sdw_command_response
cdns_reset_page_addr(struct sdw_bus *bus, unsigned int dev_num);

int cdns_bus_conf(struct sdw_bus *bus, struct sdw_bus_params *params);

int cdns_set_sdw_stream(struct snd_soc_dai *dai,
			void *stream, bool pcm, int direction);
#endif /* __SDW_CADENCE_H */
