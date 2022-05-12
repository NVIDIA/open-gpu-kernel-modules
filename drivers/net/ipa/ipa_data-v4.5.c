// SPDX-License-Identifier: GPL-2.0

/* Copyright (C) 2021 Linaro Ltd. */

#include <linux/log2.h>

#include "gsi.h"
#include "ipa_data.h"
#include "ipa_endpoint.h"
#include "ipa_mem.h"

/** enum ipa_resource_type - IPA resource types for an SoC having IPA v4.5 */
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

/* Resource groups used for an SoC having IPA v4.5 */
enum ipa_rsrc_group_id {
	/* Source resource group identifiers */
	IPA_RSRC_GROUP_SRC_UNUSED_0		= 0,
	IPA_RSRC_GROUP_SRC_UL_DL,
	IPA_RSRC_GROUP_SRC_UNUSED_2,
	IPA_RSRC_GROUP_SRC_UNUSED_3,
	IPA_RSRC_GROUP_SRC_UC_RX_Q,
	IPA_RSRC_GROUP_SRC_COUNT,	/* Last in set; not a source group */

	/* Destination resource group identifiers */
	IPA_RSRC_GROUP_DST_UNUSED_0		= 0,
	IPA_RSRC_GROUP_DST_UL_DL_DPL,
	IPA_RSRC_GROUP_DST_UNUSED_2,
	IPA_RSRC_GROUP_DST_UNUSED_3,
	IPA_RSRC_GROUP_DST_UC,
	IPA_RSRC_GROUP_DST_COUNT,	/* Last; not a destination group */
};

/* QSB configuration data for an SoC having IPA v4.5 */
static const struct ipa_qsb_data ipa_qsb_data[] = {
	[IPA_QSB_MASTER_DDR] = {
		.max_writes		= 8,
		.max_reads		= 0,	/* no limit (hardware max) */
		.max_reads_beats	= 120,
	},
	[IPA_QSB_MASTER_PCIE] = {
		.max_writes		= 8,
		.max_reads		= 12,
		/* no outstanding read byte (beat) limit */
	},
};

/* Endpoint configuration data for an SoC having IPA v4.5 */
static const struct ipa_gsi_endpoint_data ipa_gsi_endpoint_data[] = {
	[IPA_ENDPOINT_AP_COMMAND_TX] = {
		.ee_id		= GSI_EE_AP,
		.channel_id	= 9,
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
		.channel_id	= 10,
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
		.channel_id	= 7,
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
		.channel_id	= 1,
		.endpoint_id	= 14,
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
		.endpoint_id	= 21,
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

/* Source resource configuration data for an SoC having IPA v4.5 */
static const struct ipa_resource ipa_resource_src[] = {
	[IPA_RESOURCE_TYPE_SRC_PKT_CONTEXTS] = {
		.limits[IPA_RSRC_GROUP_SRC_UL_DL] = {
			.min = 1,	.max = 11,
		},
		.limits[IPA_RSRC_GROUP_SRC_UC_RX_Q] = {
			.min = 1,	.max = 63,
		},
	},
	[IPA_RESOURCE_TYPE_SRC_DESCRIPTOR_LISTS] = {
		.limits[IPA_RSRC_GROUP_SRC_UL_DL] = {
			.min = 14,	.max = 14,
		},
		.limits[IPA_RSRC_GROUP_SRC_UC_RX_Q] = {
			.min = 3,	.max = 3,
		},
	},
	[IPA_RESOURCE_TYPE_SRC_DESCRIPTOR_BUFF] = {
		.limits[IPA_RSRC_GROUP_SRC_UL_DL] = {
			.min = 18,	.max = 18,
		},
		.limits[IPA_RSRC_GROUP_SRC_UC_RX_Q] = {
			.min = 8,	.max = 8,
		},
	},
	[IPA_RESOURCE_TYPE_SRC_HPS_DMARS] = {
		.limits[IPA_RSRC_GROUP_SRC_UNUSED_0] = {
			.min = 0,	.max = 63,
		},
		.limits[IPA_RSRC_GROUP_SRC_UL_DL] = {
			.min = 0,	.max = 63,
		},
		.limits[IPA_RSRC_GROUP_SRC_UNUSED_2] = {
			.min = 0,	.max = 63,
		},
		.limits[IPA_RSRC_GROUP_SRC_UNUSED_3] = {
			.min = 0,	.max = 63,
		},
		.limits[IPA_RSRC_GROUP_SRC_UC_RX_Q] = {
			.min = 0,	.max = 63,
		},
	},
	[IPA_RESOURCE_TYPE_SRC_ACK_ENTRIES] = {
		.limits[IPA_RSRC_GROUP_SRC_UL_DL] = {
			.min = 24,	.max = 24,
		},
		.limits[IPA_RSRC_GROUP_SRC_UC_RX_Q] = {
			.min = 8,	.max = 8,
		},
	},
};

/* Destination resource configuration data for an SoC having IPA v4.5 */
static const struct ipa_resource ipa_resource_dst[] = {
	[IPA_RESOURCE_TYPE_DST_DATA_SECTORS] = {
		.limits[IPA_RSRC_GROUP_DST_UL_DL_DPL] = {
			.min = 16,	.max = 16,
		},
		.limits[IPA_RSRC_GROUP_DST_UNUSED_2] = {
			.min = 2,	.max = 2,
		},
		.limits[IPA_RSRC_GROUP_DST_UNUSED_3] = {
			.min = 2,	.max = 2,
		},
	},
	[IPA_RESOURCE_TYPE_DST_DPS_DMARS] = {
		.limits[IPA_RSRC_GROUP_DST_UL_DL_DPL] = {
			.min = 2,	.max = 63,
		},
		.limits[IPA_RSRC_GROUP_DST_UNUSED_2] = {
			.min = 1,	.max = 2,
		},
		.limits[IPA_RSRC_GROUP_DST_UNUSED_3] = {
			.min = 1,	.max = 2,
		},
		.limits[IPA_RSRC_GROUP_DST_UC] = {
			.min = 0,	.max = 2,
		},
	},
};

/* Resource configuration data for an SoC having IPA v4.5 */
static const struct ipa_resource_data ipa_resource_data = {
	.rsrc_group_src_count	= IPA_RSRC_GROUP_SRC_COUNT,
	.rsrc_group_dst_count	= IPA_RSRC_GROUP_DST_COUNT,
	.resource_src_count	= ARRAY_SIZE(ipa_resource_src),
	.resource_src		= ipa_resource_src,
	.resource_dst_count	= ARRAY_SIZE(ipa_resource_dst),
	.resource_dst		= ipa_resource_dst,
};

/* IPA-resident memory region data for an SoC having IPA v4.5 */
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
		.size		= 0x0b20,
		.canary_count	= 2,
	},
	[IPA_MEM_AP_PROC_CTX] = {
		.offset		= 0x15f0,
		.size		= 0x0200,
		.canary_count	= 0,
	},
	[IPA_MEM_NAT_TABLE] = {
		.offset		= 0x1800,
		.size		= 0x0d00,
		.canary_count	= 4,
	},
	[IPA_MEM_STATS_QUOTA_MODEM] = {
		.offset		= 0x2510,
		.size		= 0x0030,
		.canary_count	= 4,
	},
	[IPA_MEM_STATS_QUOTA_AP] = {
		.offset		= 0x2540,
		.size		= 0x0048,
		.canary_count	= 0,
	},
	[IPA_MEM_STATS_TETHERING] = {
		.offset		= 0x2588,
		.size		= 0x0238,
		.canary_count	= 0,
	},
	[IPA_MEM_STATS_FILTER_ROUTE] = {
		.offset		= 0x27c0,
		.size		= 0x0800,
		.canary_count	= 0,
	},
	[IPA_MEM_STATS_DROP] = {
		.offset		= 0x2fc0,
		.size		= 0x0020,
		.canary_count	= 0,
	},
	[IPA_MEM_MODEM] = {
		.offset		= 0x2fe8,
		.size		= 0x0800,
		.canary_count	= 2,
	},
	[IPA_MEM_UC_EVENT_RING] = {
		.offset		= 0x3800,
		.size		= 0x1000,
		.canary_count	= 1,
	},
	[IPA_MEM_PDN_CONFIG] = {
		.offset		= 0x4800,
		.size		= 0x0050,
		.canary_count	= 0,
	},
};

/* Memory configuration data for an SoC having IPA v4.5 */
static const struct ipa_mem_data ipa_mem_data = {
	.local_count	= ARRAY_SIZE(ipa_mem_local_data),
	.local		= ipa_mem_local_data,
	.imem_addr	= 0x14688000,
	.imem_size	= 0x00003000,
	.smem_id	= 497,
	.smem_size	= 0x00009000,
};

/* Interconnect rates are in 1000 byte/second units */
static const struct ipa_interconnect_data ipa_interconnect_data[] = {
	{
		.name			= "memory-a",
		.peak_bandwidth		= 600000,	/* 600 MBps */
		.average_bandwidth	= 150000,	/* 150 MBps */
	},
	{
		.name			= "memory-b",
		.peak_bandwidth		= 1804000,	/* 1.804 GBps */
		.average_bandwidth	= 150000,	/* 150 MBps */
	},
	/* Average rate is unused for the next two interconnects */
	{
		.name			= "imem",
		.peak_bandwidth		= 450000,	/* 450 MBps */
		.average_bandwidth	= 75000,	/* 75 MBps (unused?) */
	},
	{
		.name			= "config",
		.peak_bandwidth		= 171400,	/* 171.4 MBps */
		.average_bandwidth	= 0,		/* unused */
	},
};

/* Clock and interconnect configuration data for an SoC having IPA v4.5 */
static const struct ipa_clock_data ipa_clock_data = {
	.core_clock_rate	= 150 * 1000 * 1000,	/* Hz (150?  60?) */
	.interconnect_count	= ARRAY_SIZE(ipa_interconnect_data),
	.interconnect_data	= ipa_interconnect_data,
};

/* Configuration data for an SoC having IPA v4.5 */
const struct ipa_data ipa_data_v4_5 = {
	.version	= IPA_VERSION_4_5,
	.qsb_count	= ARRAY_SIZE(ipa_qsb_data),
	.qsb_data	= ipa_qsb_data,
	.endpoint_count	= ARRAY_SIZE(ipa_gsi_endpoint_data),
	.endpoint_data	= ipa_gsi_endpoint_data,
	.resource_data	= &ipa_resource_data,
	.mem_data	= &ipa_mem_data,
	.clock_data	= &ipa_clock_data,
};
