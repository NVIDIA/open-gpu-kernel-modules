// SPDX-License-Identifier: GPL-2.0

/* Copyright (C) 2021 Linaro Ltd. */

#include <linux/log2.h>

#include "gsi.h"
#include "ipa_data.h"
#include "ipa_endpoint.h"
#include "ipa_mem.h"

/** enum ipa_resource_type - IPA resource types for an SoC having IPA v4.11 */
enum ipa_resource_type {
	/* Source resource types; first must have value 0 */
	IPA_RESOURCE_TYPE_SRC_PKT_CONTEXTS		= 0,
	IPA_RESOURCE_TYPE_SRC_DESCRIPTOR_LISTS,
	IPA_RESOURCE_TYPE_SRC_DESCRIPTOR_BUFF,
	IPA_RESOURCE_TYPE_SRC_HPS_DMARS,
	IPA_RESOURCE_TYPE_SRC_ACK_ENTRIES,

	/* Destination resource types; first must have value 0 */
	IPA_RESOURCE_TYPE_DST_DATA_SECTORS		= 0,
	IPA_RESOURCE_TYPE_DST_DPS_DMARS,
};

/* Resource groups used for an SoC having IPA v4.11 */
enum ipa_rsrc_group_id {
	/* Source resource group identifiers */
	IPA_RSRC_GROUP_SRC_UL_DL			= 0,
	IPA_RSRC_GROUP_SRC_UC_RX_Q,
	IPA_RSRC_GROUP_SRC_UNUSED_2,
	IPA_RSRC_GROUP_SRC_COUNT,	/* Last in set; not a source group */

	/* Destination resource group identifiers */
	IPA_RSRC_GROUP_DST_UL_DL_DPL			= 0,
	IPA_RSRC_GROUP_DST_UNUSED_1,
	IPA_RSRC_GROUP_DST_DRB_IP,
	IPA_RSRC_GROUP_DST_COUNT,	/* Last; not a destination group */
};

/* QSB configuration data for an SoC having IPA v4.11 */
static const struct ipa_qsb_data ipa_qsb_data[] = {
	[IPA_QSB_MASTER_DDR] = {
		.max_writes		= 12,
		.max_reads		= 13,
		.max_reads_beats	= 120,
	},
};

/* Endpoint configuration data for an SoC having IPA v4.11 */
static const struct ipa_gsi_endpoint_data ipa_gsi_endpoint_data[] = {
	[IPA_ENDPOINT_AP_COMMAND_TX] = {
		.ee_id		= GSI_EE_AP,
		.channel_id	= 5,
		.endpoint_id	= 7,
		.toward_ipa	= true,
		.channel = {
			.tre_count	= 256,
			.event_count	= 256,
			.tlv_count	= 20,
		},
		.endpoint = {
			.config = {
				.resource_group	= IPA_RSRC_GROUP_SRC_UL_DL,
				.dma_mode	= true,
				.dma_endpoint	= IPA_ENDPOINT_AP_LAN_RX,
				.tx = {
					.seq_type = IPA_SEQ_DMA,
				},
			},
		},
	},
	[IPA_ENDPOINT_AP_LAN_RX] = {
		.ee_id		= GSI_EE_AP,
		.channel_id	= 14,
		.endpoint_id	= 9,
		.toward_ipa	= false,
		.channel = {
			.tre_count	= 256,
			.event_count	= 256,
			.tlv_count	= 9,
		},
		.endpoint = {
			.config = {
				.resource_group	= IPA_RSRC_GROUP_DST_UL_DL_DPL,
				.aggregation	= true,
				.status_enable	= true,
				.rx = {
					.pad_align	= ilog2(sizeof(u32)),
				},
			},
		},
	},
	[IPA_ENDPOINT_AP_MODEM_TX] = {
		.ee_id		= GSI_EE_AP,
		.channel_id	= 2,
		.endpoint_id	= 2,
		.toward_ipa	= true,
		.channel = {
			.tre_count	= 512,
			.event_count	= 512,
			.tlv_count	= 16,
		},
		.endpoint = {
			.filter_support	= true,
			.config = {
				.resource_group	= IPA_RSRC_GROUP_SRC_UL_DL,
				.qmap		= true,
				.status_enable	= true,
				.tx = {
					.seq_type = IPA_SEQ_2_PASS_SKIP_LAST_UC,
					.status_endpoint =
						IPA_ENDPOINT_MODEM_AP_RX,
				},
			},
		},
	},
	[IPA_ENDPOINT_AP_MODEM_RX] = {
		.ee_id		= GSI_EE_AP,
		.channel_id	= 7,
		.endpoint_id	= 16,
		.toward_ipa	= false,
		.channel = {
			.tre_count	= 256,
			.event_count	= 256,
			.tlv_count	= 9,
		},
		.endpoint = {
			.config = {
				.resource_group	= IPA_RSRC_GROUP_DST_UL_DL_DPL,
				.qmap		= true,
				.aggregation	= true,
				.rx = {
					.aggr_close_eof	= true,
				},
			},
		},
	},
	[IPA_ENDPOINT_MODEM_AP_TX] = {
		.ee_id		= GSI_EE_MODEM,
		.channel_id	= 0,
		.endpoint_id	= 5,
		.toward_ipa	= true,
		.endpoint = {
			.filter_support	= true,
		},
	},
	[IPA_ENDPOINT_MODEM_AP_RX] = {
		.ee_id		= GSI_EE_MODEM,
		.channel_id	= 7,
		.endpoint_id	= 14,
		.toward_ipa	= false,
	},
	[IPA_ENDPOINT_MODEM_DL_NLO_TX] = {
		.ee_id		= GSI_EE_MODEM,
		.channel_id	= 2,
		.endpoint_id	= 8,
		.toward_ipa	= true,
		.endpoint = {
			.filter_support	= true,
		},
	},
};

/* Source resource configuration data for an SoC having IPA v4.11 */
static const struct ipa_resource ipa_resource_src[] = {
	[IPA_RESOURCE_TYPE_SRC_PKT_CONTEXTS] = {
		.limits[IPA_RSRC_GROUP_SRC_UL_DL] = {
			.min = 6,	.max = 6,
		},
	},
	[IPA_RESOURCE_TYPE_SRC_DESCRIPTOR_LISTS] = {
		.limits[IPA_RSRC_GROUP_SRC_UL_DL] = {
			.min = 8,	.max = 8,
		},
	},
	[IPA_RESOURCE_TYPE_SRC_DESCRIPTOR_BUFF] = {
		.limits[IPA_RSRC_GROUP_SRC_UL_DL] = {
			.min = 18,	.max = 18,
		},
	},
	[IPA_RESOURCE_TYPE_SRC_HPS_DMARS] = {
		.limits[IPA_RSRC_GROUP_SRC_UL_DL] = {
			.min = 2,	.max = 2,
		},
	},
	[IPA_RESOURCE_TYPE_SRC_ACK_ENTRIES] = {
		.limits[IPA_RSRC_GROUP_SRC_UL_DL] = {
			.min = 15,	.max = 15,
		},
	},
};

/* Destination resource configuration data for an SoC having IPA v4.11 */
static const struct ipa_resource ipa_resource_dst[] = {
	[IPA_RESOURCE_TYPE_DST_DATA_SECTORS] = {
		.limits[IPA_RSRC_GROUP_DST_UL_DL_DPL] = {
			.min = 3,	.max = 3,
		},
		.limits[IPA_RSRC_GROUP_DST_DRB_IP] = {
			.min = 25,	.max = 25,
		},
	},
	[IPA_RESOURCE_TYPE_DST_DPS_DMARS] = {
		.limits[IPA_RSRC_GROUP_DST_UL_DL_DPL] = {
			.min = 2,	.max = 2,
		},
	},
};

/* Resource configuration data for an SoC having IPA v4.11 */
static const struct ipa_resource_data ipa_resource_data = {
	.rsrc_group_src_count	= IPA_RSRC_GROUP_SRC_COUNT,
	.rsrc_group_dst_count	= IPA_RSRC_GROUP_DST_COUNT,
	.resource_src_count	= ARRAY_SIZE(ipa_resource_src),
	.resource_src		= ipa_resource_src,
	.resource_dst_count	= ARRAY_SIZE(ipa_resource_dst),
	.resource_dst		= ipa_resource_dst,
};

/* IPA-resident memory region data for an SoC having IPA v4.11 */
static const struct ipa_mem ipa_mem_local_data[] = {
	[IPA_MEM_UC_SHARED] = {
		.offset		= 0x0000,
		.size		= 0x0080,
		.canary_count	= 0,
	},
	[IPA_MEM_UC_INFO] = {
		.offset		= 0x0080,
		.size		= 0x0200,
		.canary_count	= 0,
	},
	[IPA_MEM_V4_FILTER_HASHED] = {
		.offset		= 0x0288,
		.size		= 0x0078,
		.canary_count	= 2,
	},
	[IPA_MEM_V4_FILTER] = {
		.offset		= 0x0308,
		.size		= 0x0078,
		.canary_count	= 2,
	},
	[IPA_MEM_V6_FILTER_HASHED] = {
		.offset		= 0x0388,
		.size		= 0x0078,
		.canary_count	= 2,
	},
	[IPA_MEM_V6_FILTER] = {
		.offset		= 0x0408,
		.size		= 0x0078,
		.canary_count	= 2,
	},
	[IPA_MEM_V4_ROUTE_HASHED] = {
		.offset		= 0x0488,
		.size		= 0x0078,
		.canary_count	= 2,
	},
	[IPA_MEM_V4_ROUTE] = {
		.offset		= 0x0508,
		.size		= 0x0078,
		.canary_count	= 2,
	},
	[IPA_MEM_V6_ROUTE_HASHED] = {
		.offset		= 0x0588,
		.size		= 0x0078,
		.canary_count	= 2,
	},
	[IPA_MEM_V6_ROUTE] = {
		.offset		= 0x0608,
		.size		= 0x0078,
		.canary_count	= 2,
	},
	[IPA_MEM_MODEM_HEADER] = {
		.offset		= 0x0688,
		.size		= 0x0240,
		.canary_count	= 2,
	},
	[IPA_MEM_AP_HEADER] = {
		.offset		= 0x08c8,
		.size		= 0x0200,
		.canary_count	= 0,
	},
	[IPA_MEM_MODEM_PROC_CTX] = {
		.offset		= 0x0ad0,
		.size		= 0x0200,
		.canary_count	= 2,
	},
	[IPA_MEM_AP_PROC_CTX] = {
		.offset		= 0x0cd0,
		.size		= 0x0200,
		.canary_count	= 0,
	},
	[IPA_MEM_NAT_TABLE] = {
		.offset		= 0x0ee0,
		.size		= 0x0d00,
		.canary_count	= 4,
	},
	[IPA_MEM_PDN_CONFIG] = {
		.offset		= 0x1be8,
		.size		= 0x0050,
		.canary_count	= 0,
	},
	[IPA_MEM_STATS_QUOTA_MODEM] = {
		.offset		= 0x1c40,
		.size		= 0x0030,
		.canary_count	= 4,
	},
	[IPA_MEM_STATS_QUOTA_AP] = {
		.offset		= 0x1c70,
		.size		= 0x0048,
		.canary_count	= 0,
	},
	[IPA_MEM_STATS_TETHERING] = {
		.offset		= 0x1cb8,
		.size		= 0x0238,
		.canary_count	= 0,
	},
	[IPA_MEM_STATS_DROP] = {
		.offset		= 0x1ef0,
		.size		= 0x0020,
		.canary_count	= 0,
	},
	[IPA_MEM_MODEM] = {
		.offset		= 0x1f18,
		.size		= 0x100c,
		.canary_count	= 2,
	},
	[IPA_MEM_UC_EVENT_RING] = {
		.offset		= 0x3000,
		.size		= 0x0000,
		.canary_count	= 1,
	},
};

/* Memory configuration data for an SoC having IPA v4.11 */
static const struct ipa_mem_data ipa_mem_data = {
	.local_count	= ARRAY_SIZE(ipa_mem_local_data),
	.local		= ipa_mem_local_data,
	.imem_addr	= 0x146a8000,
	.imem_size	= 0x00002000,
	.smem_id	= 497,
	.smem_size	= 0x00009000,
};

/* Interconnect rates are in 1000 byte/second units */
static const struct ipa_interconnect_data ipa_interconnect_data[] = {
	{
		.name			= "memory",
		.peak_bandwidth		= 465000,	/* 465 MBps */
		.average_bandwidth	= 80000,	/* 80 MBps */
	},
	/* Average rate is unused for the next two interconnects */
	{
		.name			= "imem",
		.peak_bandwidth		= 68570,	/* 68.57 MBps */
		.average_bandwidth	= 80000,	/* 80 MBps (unused?) */
	},
	{
		.name			= "config",
		.peak_bandwidth		= 30000,	/* 30 MBps */
		.average_bandwidth	= 0,		/* unused */
	},
};

/* Clock and interconnect configuration data for an SoC having IPA v4.11 */
static const struct ipa_clock_data ipa_clock_data = {
	.core_clock_rate	= 60 * 1000 * 1000,	/* Hz */
	.interconnect_count	= ARRAY_SIZE(ipa_interconnect_data),
	.interconnect_data	= ipa_interconnect_data,
};

/* Configuration data for an SoC having IPA v4.11 */
const struct ipa_data ipa_data_v4_11 = {
	.version	= IPA_VERSION_4_11,
	.qsb_count	= ARRAY_SIZE(ipa_qsb_data),
	.qsb_data	= ipa_qsb_data,
	.endpoint_count	= ARRAY_SIZE(ipa_gsi_endpoint_data),
	.endpoint_data	= ipa_gsi_endpoint_data,
	.resource_data	= &ipa_resource_data,
	.mem_data	= &ipa_mem_data,
	.clock_data	= &ipa_clock_data,
};
