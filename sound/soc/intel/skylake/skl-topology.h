/* SPDX-License-Identifier: GPL-2.0-only */
/*
 *  skl_topology.h - Intel HDA Platform topology header file
 *
 *  Copyright (C) 2014-15 Intel Corp
 *  Author: Jeeja KP <jeeja.kp@intel.com>
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

#ifndef __SKL_TOPOLOGY_H__
#define __SKL_TOPOLOGY_H__

#include <linux/types.h>

#include <sound/hdaudio_ext.h>
#include <sound/soc.h>
#include <uapi/sound/skl-tplg-interface.h>
#include "skl.h"

#define BITS_PER_BYTE 8
#define MAX_TS_GROUPS 8
#define MAX_DMIC_TS_GROUPS 4
#define MAX_FIXED_DMIC_PARAMS_SIZE 727

/* Maximum number of coefficients up down mixer module */
#define UP_DOWN_MIXER_MAX_COEFF		8

#define MODULE_MAX_IN_PINS	8
#define MODULE_MAX_OUT_PINS	8

#define SKL_MIC_CH_SUPPORT	4
#define SKL_MIC_MAX_CH_SUPPORT	8
#define SKL_DEFAULT_MIC_SEL_GAIN	0x3FF
#define SKL_MIC_SEL_SWITCH	0x3

#define SKL_OUTPUT_PIN		0
#define SKL_INPUT_PIN		1
#define SKL_MAX_PATH_CONFIGS	8
#define SKL_MAX_MODULES_IN_PIPE	8
#define SKL_MAX_MODULE_FORMATS		32
#define SKL_MAX_MODULE_RESOURCES	32

enum skl_channel_index {
	SKL_CHANNEL_LEFT = 0,
	SKL_CHANNEL_RIGHT = 1,
	SKL_CHANNEL_CENTER = 2,
	SKL_CHANNEL_LEFT_SURROUND = 3,
	SKL_CHANNEL_CENTER_SURROUND = 3,
	SKL_CHANNEL_RIGHT_SURROUND = 4,
	SKL_CHANNEL_LFE = 7,
	SKL_CHANNEL_INVALID = 0xF,
};

enum skl_bitdepth {
	SKL_DEPTH_8BIT = 8,
	SKL_DEPTH_16BIT = 16,
	SKL_DEPTH_24BIT = 24,
	SKL_DEPTH_32BIT = 32,
	SKL_DEPTH_INVALID
};


enum skl_s_freq {
	SKL_FS_8000 = 8000,
	SKL_FS_11025 = 11025,
	SKL_FS_12000 = 12000,
	SKL_FS_16000 = 16000,
	SKL_FS_22050 = 22050,
	SKL_FS_24000 = 24000,
	SKL_FS_32000 = 32000,
	SKL_FS_44100 = 44100,
	SKL_FS_48000 = 48000,
	SKL_FS_64000 = 64000,
	SKL_FS_88200 = 88200,
	SKL_FS_96000 = 96000,
	SKL_FS_128000 = 128000,
	SKL_FS_176400 = 176400,
	SKL_FS_192000 = 192000,
	SKL_FS_INVALID
};

enum skl_widget_type {
	SKL_WIDGET_VMIXER = 1,
	SKL_WIDGET_MIXER = 2,
	SKL_WIDGET_PGA = 3,
	SKL_WIDGET_MUX = 4
};

struct skl_audio_data_format {
	enum skl_s_freq s_freq;
	enum skl_bitdepth bit_depth;
	u32 channel_map;
	enum skl_ch_cfg ch_cfg;
	enum skl_interleaving interleaving;
	u8 number_of_channels;
	u8 valid_bit_depth;
	u8 sample_type;
	u8 reserved;
} __packed;

struct skl_base_cfg {
	u32 cpc;
	u32 ibs;
	u32 obs;
	u32 is_pages;
	struct skl_audio_data_format audio_fmt;
};

struct skl_cpr_gtw_cfg {
	u32 node_id;
	u32 dma_buffer_size;
	u32 config_length;
	/* not mandatory; required only for DMIC/I2S */
	u32 config_data[1];
} __packed;

struct skl_dma_control {
	u32 node_id;
	u32 config_length;
	u32 config_data[];
} __packed;

struct skl_cpr_cfg {
	struct skl_base_cfg base_cfg;
	struct skl_audio_data_format out_fmt;
	u32 cpr_feature_mask;
	struct skl_cpr_gtw_cfg gtw_cfg;
} __packed;

struct skl_cpr_pin_fmt {
	u32 sink_id;
	struct skl_audio_data_format src_fmt;
	struct skl_audio_data_format dst_fmt;
} __packed;

struct skl_src_module_cfg {
	struct skl_base_cfg base_cfg;
	enum skl_s_freq src_cfg;
} __packed;

struct skl_up_down_mixer_cfg {
	struct skl_base_cfg base_cfg;
	enum skl_ch_cfg out_ch_cfg;
	/* This should be set to 1 if user coefficients are required */
	u32 coeff_sel;
	/* Pass the user coeff in this array */
	s32 coeff[UP_DOWN_MIXER_MAX_COEFF];
	u32 ch_map;
} __packed;

struct skl_algo_cfg {
	struct skl_base_cfg  base_cfg;
	char params[];
} __packed;

struct skl_base_outfmt_cfg {
	struct skl_base_cfg base_cfg;
	struct skl_audio_data_format out_fmt;
} __packed;

enum skl_dma_type {
	SKL_DMA_HDA_HOST_OUTPUT_CLASS = 0,
	SKL_DMA_HDA_HOST_INPUT_CLASS = 1,
	SKL_DMA_HDA_HOST_INOUT_CLASS = 2,
	SKL_DMA_HDA_LINK_OUTPUT_CLASS = 8,
	SKL_DMA_HDA_LINK_INPUT_CLASS = 9,
	SKL_DMA_HDA_LINK_INOUT_CLASS = 0xA,
	SKL_DMA_DMIC_LINK_INPUT_CLASS = 0xB,
	SKL_DMA_I2S_LINK_OUTPUT_CLASS = 0xC,
	SKL_DMA_I2S_LINK_INPUT_CLASS = 0xD,
};

union skl_ssp_dma_node {
	u8 val;
	struct {
		u8 time_slot_index:4;
		u8 i2s_instance:4;
	} dma_node;
};

union skl_connector_node_id {
	u32 val;
	struct {
		u32 vindex:8;
		u32 dma_type:4;
		u32 rsvd:20;
	} node;
};

struct skl_module_fmt {
	u32 channels;
	u32 s_freq;
	u32 bit_depth;
	u32 valid_bit_depth;
	u32 ch_cfg;
	u32 interleaving_style;
	u32 sample_type;
	u32 ch_map;
};

struct skl_module_cfg;

struct skl_mod_inst_map {
	u16 mod_id;
	u16 inst_id;
};

struct skl_uuid_inst_map {
	u16 inst_id;
	u16 reserved;
	guid_t mod_uuid;
} __packed;

struct skl_kpb_params {
	u32 num_modules;
	union {
		struct skl_mod_inst_map map[0];
		struct skl_uuid_inst_map map_uuid[0];
	} u;
};

struct skl_module_inst_id {
	guid_t mod_uuid;
	int module_id;
	u32 instance_id;
	int pvt_id;
};

enum skl_module_pin_state {
	SKL_PIN_UNBIND = 0,
	SKL_PIN_BIND_DONE = 1,
};

struct skl_module_pin {
	struct skl_module_inst_id id;
	bool is_dynamic;
	bool in_use;
	enum skl_module_pin_state pin_state;
	struct skl_module_cfg *tgt_mcfg;
};

struct skl_specific_cfg {
	u32 set_params;
	u32 param_id;
	u32 caps_size;
	u32 *caps;
};

enum skl_pipe_state {
	SKL_PIPE_INVALID = 0,
	SKL_PIPE_CREATED = 1,
	SKL_PIPE_PAUSED = 2,
	SKL_PIPE_STARTED = 3,
	SKL_PIPE_RESET = 4
};

struct skl_pipe_module {
	struct snd_soc_dapm_widget *w;
	struct list_head node;
};

struct skl_pipe_params {
	u8 host_dma_id;
	u8 link_dma_id;
	u32 ch;
	u32 s_freq;
	u32 s_fmt;
	u8 linktype;
	snd_pcm_format_t format;
	int link_index;
	int stream;
	unsigned int host_bps;
	unsigned int link_bps;
};

struct skl_pipe_fmt {
	u32 freq;
	u8 channels;
	u8 bps;
};

struct skl_pipe_mcfg {
	u8 res_idx;
	u8 fmt_idx;
};

struct skl_path_config {
	u8 mem_pages;
	struct skl_pipe_fmt in_fmt;
	struct skl_pipe_fmt out_fmt;
};

struct skl_pipe {
	u8 ppl_id;
	u8 pipe_priority;
	u16 conn_type;
	u32 memory_pages;
	u8 lp_mode;
	struct skl_pipe_params *p_params;
	enum skl_pipe_state state;
	u8 direction;
	u8 cur_config_idx;
	u8 nr_cfgs;
	struct skl_path_config configs[SKL_MAX_PATH_CONFIGS];
	struct list_head w_list;
	bool passthru;
	u32 pipe_config_idx;
};

enum skl_module_state {
	SKL_MODULE_UNINIT = 0,
	SKL_MODULE_LOADED = 1,
	SKL_MODULE_INIT_DONE = 2,
	SKL_MODULE_BIND_DONE = 3,
	SKL_MODULE_UNLOADED = 4,
};

enum d0i3_capability {
	SKL_D0I3_NONE = 0,
	SKL_D0I3_STREAMING = 1,
	SKL_D0I3_NON_STREAMING = 2,
};

struct skl_module_pin_fmt {
	u8 id;
	struct skl_module_fmt fmt;
};

struct skl_module_iface {
	u8 fmt_idx;
	u8 nr_in_fmt;
	u8 nr_out_fmt;
	struct skl_module_pin_fmt inputs[MAX_IN_QUEUE];
	struct skl_module_pin_fmt outputs[MAX_OUT_QUEUE];
};

struct skl_module_pin_resources {
	u8 pin_index;
	u32 buf_size;
};

struct skl_module_res {
	u8 id;
	u32 is_pages;
	u32 ibs;
	u32 obs;
	u32 dma_buffer_size;
	u32 cpc;
	u8 nr_input_pins;
	u8 nr_output_pins;
	struct skl_module_pin_resources input[MAX_IN_QUEUE];
	struct skl_module_pin_resources output[MAX_OUT_QUEUE];
};

struct skl_module {
	guid_t uuid;
	u8 loadable;
	u8 input_pin_type;
	u8 output_pin_type;
	u8 max_input_pins;
	u8 max_output_pins;
	u8 nr_resources;
	u8 nr_interfaces;
	struct skl_module_res resources[SKL_MAX_MODULE_RESOURCES];
	struct skl_module_iface formats[SKL_MAX_MODULE_FORMATS];
};

struct skl_module_cfg {
	u8 guid[16];
	struct skl_module_inst_id id;
	struct skl_module *module;
	int res_idx;
	int fmt_idx;
	u8 domain;
	bool homogenous_inputs;
	bool homogenous_outputs;
	struct skl_module_fmt in_fmt[MODULE_MAX_IN_PINS];
	struct skl_module_fmt out_fmt[MODULE_MAX_OUT_PINS];
	u8 max_in_queue;
	u8 max_out_queue;
	u8 in_queue_mask;
	u8 out_queue_mask;
	u8 in_queue;
	u8 out_queue;
	u8 is_loadable;
	u8 core_id;
	u8 dev_type;
	u8 dma_id;
	u8 time_slot;
	u8 dmic_ch_combo_index;
	u32 dmic_ch_type;
	u32 params_fixup;
	u32 converter;
	u32 vbus_id;
	u32 mem_pages;
	enum d0i3_capability d0i3_caps;
	u32 dma_buffer_size; /* in milli seconds */
	struct skl_module_pin *m_in_pin;
	struct skl_module_pin *m_out_pin;
	enum skl_module_type m_type;
	enum skl_hw_conn_type  hw_conn_type;
	enum skl_module_state m_state;
	struct skl_pipe *pipe;
	struct skl_specific_cfg formats_config;
	struct skl_pipe_mcfg mod_cfg[SKL_MAX_MODULES_IN_PIPE];
};

struct skl_algo_data {
	u32 param_id;
	u32 set_params;
	u32 max;
	u32 size;
	char *params;
};

struct skl_pipeline {
	struct skl_pipe *pipe;
	struct list_head node;
};

struct skl_module_deferred_bind {
	struct skl_module_cfg *src;
	struct skl_module_cfg *dst;
	struct list_head node;
};

struct skl_mic_sel_config {
	u16 mic_switch;
	u16 flags;
	u16 blob[SKL_MIC_MAX_CH_SUPPORT][SKL_MIC_MAX_CH_SUPPORT];
} __packed;

enum skl_channel {
	SKL_CH_MONO = 1,
	SKL_CH_STEREO = 2,
	SKL_CH_TRIO = 3,
	SKL_CH_QUATRO = 4,
};

static inline struct skl_dev *get_skl_ctx(struct device *dev)
{
	struct hdac_bus *bus = dev_get_drvdata(dev);

	return bus_to_skl(bus);
}

int skl_tplg_be_update_params(struct snd_soc_dai *dai,
	struct skl_pipe_params *params);
int skl_dsp_set_dma_control(struct skl_dev *skl, u32 *caps,
			u32 caps_size, u32 node_id);
void skl_tplg_set_be_dmic_config(struct snd_soc_dai *dai,
	struct skl_pipe_params *params, int stream);
int skl_tplg_init(struct snd_soc_component *component,
				struct hdac_bus *bus);
void skl_tplg_exit(struct snd_soc_component *component,
				struct hdac_bus *bus);
struct skl_module_cfg *skl_tplg_fe_get_cpr_module(
		struct snd_soc_dai *dai, int stream);
int skl_tplg_update_pipe_params(struct device *dev,
		struct skl_module_cfg *mconfig, struct skl_pipe_params *params);

void skl_tplg_d0i3_get(struct skl_dev *skl, enum d0i3_capability caps);
void skl_tplg_d0i3_put(struct skl_dev *skl, enum d0i3_capability caps);

int skl_create_pipeline(struct skl_dev *skl, struct skl_pipe *pipe);

int skl_run_pipe(struct skl_dev *skl, struct skl_pipe *pipe);

int skl_pause_pipe(struct skl_dev *skl, struct skl_pipe *pipe);

int skl_delete_pipe(struct skl_dev *skl, struct skl_pipe *pipe);

int skl_stop_pipe(struct skl_dev *skl, struct skl_pipe *pipe);

int skl_reset_pipe(struct skl_dev *skl, struct skl_pipe *pipe);

int skl_init_module(struct skl_dev *skl, struct skl_module_cfg *mconfig);

int skl_bind_modules(struct skl_dev *skl, struct skl_module_cfg
	*src_mcfg, struct skl_module_cfg *dst_mcfg);

int skl_unbind_modules(struct skl_dev *skl, struct skl_module_cfg
	*src_mcfg, struct skl_module_cfg *dst_mcfg);

int skl_set_module_params(struct skl_dev *skl, u32 *params, int size,
			u32 param_id, struct skl_module_cfg *mcfg);
int skl_get_module_params(struct skl_dev *skl, u32 *params, int size,
			  u32 param_id, struct skl_module_cfg *mcfg);

struct skl_module_cfg *skl_tplg_be_get_cpr_module(struct snd_soc_dai *dai,
								int stream);
enum skl_bitdepth skl_get_bit_depth(int params);
int skl_pcm_host_dma_prepare(struct device *dev,
			struct skl_pipe_params *params);
int skl_pcm_link_dma_prepare(struct device *dev,
			struct skl_pipe_params *params);

int skl_dai_load(struct snd_soc_component *cmp, int index,
		struct snd_soc_dai_driver *dai_drv,
		struct snd_soc_tplg_pcm *pcm, struct snd_soc_dai *dai);
void skl_tplg_add_moduleid_in_bind_params(struct skl_dev *skl,
				struct snd_soc_dapm_widget *w);
#endif
