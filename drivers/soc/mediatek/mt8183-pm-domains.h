/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_MEDIATEK_MT8183_PM_DOMAINS_H
#define __SOC_MEDIATEK_MT8183_PM_DOMAINS_H

#include "mtk-pm-domains.h"
#include <dt-bindings/power/mt8183-power.h>

/*
 * MT8183 power domain support
 */

static const struct scpsys_domain_data scpsys_domain_data_mt8183[] = {
	[MT8183_POWER_DOMAIN_AUDIO] = {
		.name = "audio",
		.sta_mask = PWR_STATUS_AUDIO,
		.ctl_offs = 0x0314,
		.sram_pdn_bits = GENMASK(11, 8),
		.sram_pdn_ack_bits = GENMASK(15, 12),
	},
	[MT8183_POWER_DOMAIN_CONN] = {
		.name = "conn",
		.sta_mask = PWR_STATUS_CONN,
		.ctl_offs = 0x032c,
		.sram_pdn_bits = 0,
		.sram_pdn_ack_bits = 0,
		.bp_infracfg = {
			BUS_PROT_WR(MT8183_TOP_AXI_PROT_EN_CONN, MT8183_TOP_AXI_PROT_EN_SET,
				    MT8183_TOP_AXI_PROT_EN_CLR, MT8183_TOP_AXI_PROT_EN_STA1),
		},
	},
	[MT8183_POWER_DOMAIN_MFG_ASYNC] = {
		.name = "mfg_async",
		.sta_mask = PWR_STATUS_MFG_ASYNC,
		.ctl_offs = 0x0334,
		.sram_pdn_bits = 0,
		.sram_pdn_ack_bits = 0,
	},
	[MT8183_POWER_DOMAIN_MFG] = {
		.name = "mfg",
		.sta_mask = PWR_STATUS_MFG,
		.ctl_offs = 0x0338,
		.sram_pdn_bits = GENMASK(8, 8),
		.sram_pdn_ack_bits = GENMASK(12, 12),
		.caps = MTK_SCPD_DOMAIN_SUPPLY,
	},
	[MT8183_POWER_DOMAIN_MFG_CORE0] = {
		.name = "mfg_core0",
		.sta_mask = BIT(7),
		.ctl_offs = 0x034c,
		.sram_pdn_bits = GENMASK(8, 8),
		.sram_pdn_ack_bits = GENMASK(12, 12),
	},
	[MT8183_POWER_DOMAIN_MFG_CORE1] = {
		.name = "mfg_core1",
		.sta_mask = BIT(20),
		.ctl_offs = 0x0310,
		.sram_pdn_bits = GENMASK(8, 8),
		.sram_pdn_ack_bits = GENMASK(12, 12),
	},
	[MT8183_POWER_DOMAIN_MFG_2D] = {
		.name = "mfg_2d",
		.sta_mask = PWR_STATUS_MFG_2D,
		.ctl_offs = 0x0348,
		.sram_pdn_bits = GENMASK(8, 8),
		.sram_pdn_ack_bits = GENMASK(12, 12),
		.bp_infracfg = {
			BUS_PROT_WR(MT8183_TOP_AXI_PROT_EN_1_MFG, MT8183_TOP_AXI_PROT_EN_1_SET,
				    MT8183_TOP_AXI_PROT_EN_1_CLR, MT8183_TOP_AXI_PROT_EN_STA1_1),
			BUS_PROT_WR(MT8183_TOP_AXI_PROT_EN_MFG, MT8183_TOP_AXI_PROT_EN_SET,
				    MT8183_TOP_AXI_PROT_EN_CLR, MT8183_TOP_AXI_PROT_EN_STA1),
		},
	},
	[MT8183_POWER_DOMAIN_DISP] = {
		.name = "disp",
		.sta_mask = PWR_STATUS_DISP,
		.ctl_offs = 0x030c,
		.sram_pdn_bits = GENMASK(8, 8),
		.sram_pdn_ack_bits = GENMASK(12, 12),
		.bp_infracfg = {
			BUS_PROT_WR(MT8183_TOP_AXI_PROT_EN_1_DISP, MT8183_TOP_AXI_PROT_EN_1_SET,
				    MT8183_TOP_AXI_PROT_EN_1_CLR, MT8183_TOP_AXI_PROT_EN_STA1_1),
			BUS_PROT_WR(MT8183_TOP_AXI_PROT_EN_DISP, MT8183_TOP_AXI_PROT_EN_SET,
				    MT8183_TOP_AXI_PROT_EN_CLR, MT8183_TOP_AXI_PROT_EN_STA1),
		},
		.bp_smi = {
			BUS_PROT_WR(MT8183_SMI_COMMON_SMI_CLAMP_DISP,
				    MT8183_SMI_COMMON_CLAMP_EN_SET,
				    MT8183_SMI_COMMON_CLAMP_EN_CLR,
				    MT8183_SMI_COMMON_CLAMP_EN),
		},
	},
	[MT8183_POWER_DOMAIN_CAM] = {
		.name = "cam",
		.sta_mask = BIT(25),
		.ctl_offs = 0x0344,
		.sram_pdn_bits = GENMASK(9, 8),
		.sram_pdn_ack_bits = GENMASK(13, 12),
		.bp_infracfg = {
			BUS_PROT_WR(MT8183_TOP_AXI_PROT_EN_MM_CAM, MT8183_TOP_AXI_PROT_EN_MM_SET,
				    MT8183_TOP_AXI_PROT_EN_MM_CLR, MT8183_TOP_AXI_PROT_EN_MM_STA1),
			BUS_PROT_WR(MT8183_TOP_AXI_PROT_EN_CAM, MT8183_TOP_AXI_PROT_EN_SET,
				    MT8183_TOP_AXI_PROT_EN_CLR, MT8183_TOP_AXI_PROT_EN_STA1),
			BUS_PROT_WR_IGN(MT8183_TOP_AXI_PROT_EN_MM_CAM_2ND,
					MT8183_TOP_AXI_PROT_EN_MM_SET,
					MT8183_TOP_AXI_PROT_EN_MM_CLR,
					MT8183_TOP_AXI_PROT_EN_MM_STA1),
		},
		.bp_smi = {
			BUS_PROT_WR(MT8183_SMI_COMMON_SMI_CLAMP_CAM,
				    MT8183_SMI_COMMON_CLAMP_EN_SET,
				    MT8183_SMI_COMMON_CLAMP_EN_CLR,
				    MT8183_SMI_COMMON_CLAMP_EN),
		},
	},
	[MT8183_POWER_DOMAIN_ISP] = {
		.name = "isp",
		.sta_mask = PWR_STATUS_ISP,
		.ctl_offs = 0x0308,
		.sram_pdn_bits = GENMASK(9, 8),
		.sram_pdn_ack_bits = GENMASK(13, 12),
		.bp_infracfg = {
			BUS_PROT_WR(MT8183_TOP_AXI_PROT_EN_MM_ISP,
				    MT8183_TOP_AXI_PROT_EN_MM_SET,
				    MT8183_TOP_AXI_PROT_EN_MM_CLR,
				    MT8183_TOP_AXI_PROT_EN_MM_STA1),
			BUS_PROT_WR_IGN(MT8183_TOP_AXI_PROT_EN_MM_ISP_2ND,
					MT8183_TOP_AXI_PROT_EN_MM_SET,
					MT8183_TOP_AXI_PROT_EN_MM_CLR,
					MT8183_TOP_AXI_PROT_EN_MM_STA1),
		},
		.bp_smi = {
			BUS_PROT_WR(MT8183_SMI_COMMON_SMI_CLAMP_ISP,
				    MT8183_SMI_COMMON_CLAMP_EN_SET,
				    MT8183_SMI_COMMON_CLAMP_EN_CLR,
				    MT8183_SMI_COMMON_CLAMP_EN),
		},
	},
	[MT8183_POWER_DOMAIN_VDEC] = {
		.name = "vdec",
		.sta_mask = BIT(31),
		.ctl_offs = 0x0300,
		.sram_pdn_bits = GENMASK(8, 8),
		.sram_pdn_ack_bits = GENMASK(12, 12),
		.bp_smi = {
			BUS_PROT_WR(MT8183_SMI_COMMON_SMI_CLAMP_VDEC,
				    MT8183_SMI_COMMON_CLAMP_EN_SET,
				    MT8183_SMI_COMMON_CLAMP_EN_CLR,
				    MT8183_SMI_COMMON_CLAMP_EN),
		},
	},
	[MT8183_POWER_DOMAIN_VENC] = {
		.name = "venc",
		.sta_mask = PWR_STATUS_VENC,
		.ctl_offs = 0x0304,
		.sram_pdn_bits = GENMASK(11, 8),
		.sram_pdn_ack_bits = GENMASK(15, 12),
		.bp_smi = {
			BUS_PROT_WR(MT8183_SMI_COMMON_SMI_CLAMP_VENC,
				    MT8183_SMI_COMMON_CLAMP_EN_SET,
				    MT8183_SMI_COMMON_CLAMP_EN_CLR,
				    MT8183_SMI_COMMON_CLAMP_EN),
		},
	},
	[MT8183_POWER_DOMAIN_VPU_TOP] = {
		.name = "vpu_top",
		.sta_mask = BIT(26),
		.ctl_offs = 0x0324,
		.sram_pdn_bits = GENMASK(8, 8),
		.sram_pdn_ack_bits = GENMASK(12, 12),
		.bp_infracfg = {
			BUS_PROT_WR(MT8183_TOP_AXI_PROT_EN_MM_VPU_TOP,
				    MT8183_TOP_AXI_PROT_EN_MM_SET,
				    MT8183_TOP_AXI_PROT_EN_MM_CLR,
				    MT8183_TOP_AXI_PROT_EN_MM_STA1),
			BUS_PROT_WR(MT8183_TOP_AXI_PROT_EN_VPU_TOP,
				    MT8183_TOP_AXI_PROT_EN_SET,
				    MT8183_TOP_AXI_PROT_EN_CLR,
				    MT8183_TOP_AXI_PROT_EN_STA1),
			BUS_PROT_WR(MT8183_TOP_AXI_PROT_EN_MM_VPU_TOP_2ND,
				    MT8183_TOP_AXI_PROT_EN_MM_SET,
				    MT8183_TOP_AXI_PROT_EN_MM_CLR,
				    MT8183_TOP_AXI_PROT_EN_MM_STA1),
		},
		.bp_smi = {
			BUS_PROT_WR(MT8183_SMI_COMMON_SMI_CLAMP_VPU_TOP,
				    MT8183_SMI_COMMON_CLAMP_EN_SET,
				    MT8183_SMI_COMMON_CLAMP_EN_CLR,
				    MT8183_SMI_COMMON_CLAMP_EN),
		},
	},
	[MT8183_POWER_DOMAIN_VPU_CORE0] = {
		.name = "vpu_core0",
		.sta_mask = BIT(27),
		.ctl_offs = 0x33c,
		.sram_pdn_bits = GENMASK(11, 8),
		.sram_pdn_ack_bits = GENMASK(13, 12),
		.bp_infracfg = {
			BUS_PROT_WR(MT8183_TOP_AXI_PROT_EN_MCU_VPU_CORE0,
				    MT8183_TOP_AXI_PROT_EN_MCU_SET,
				    MT8183_TOP_AXI_PROT_EN_MCU_CLR,
				    MT8183_TOP_AXI_PROT_EN_MCU_STA1),
			BUS_PROT_WR(MT8183_TOP_AXI_PROT_EN_MCU_VPU_CORE0_2ND,
				    MT8183_TOP_AXI_PROT_EN_MCU_SET,
				    MT8183_TOP_AXI_PROT_EN_MCU_CLR,
				    MT8183_TOP_AXI_PROT_EN_MCU_STA1),
		},
		.caps = MTK_SCPD_SRAM_ISO,
	},
	[MT8183_POWER_DOMAIN_VPU_CORE1] = {
		.name = "vpu_core1",
		.sta_mask = BIT(28),
		.ctl_offs = 0x0340,
		.sram_pdn_bits = GENMASK(11, 8),
		.sram_pdn_ack_bits = GENMASK(13, 12),
		.bp_infracfg = {
			BUS_PROT_WR(MT8183_TOP_AXI_PROT_EN_MCU_VPU_CORE1,
				    MT8183_TOP_AXI_PROT_EN_MCU_SET,
				    MT8183_TOP_AXI_PROT_EN_MCU_CLR,
				    MT8183_TOP_AXI_PROT_EN_MCU_STA1),
			BUS_PROT_WR(MT8183_TOP_AXI_PROT_EN_MCU_VPU_CORE1_2ND,
				    MT8183_TOP_AXI_PROT_EN_MCU_SET,
				    MT8183_TOP_AXI_PROT_EN_MCU_CLR,
				    MT8183_TOP_AXI_PROT_EN_MCU_STA1),
		},
		.caps = MTK_SCPD_SRAM_ISO,
	},
};

static const struct scpsys_soc_data mt8183_scpsys_data = {
	.domains_data = scpsys_domain_data_mt8183,
	.num_domains = ARRAY_SIZE(scpsys_domain_data_mt8183),
	.pwr_sta_offs = 0x0180,
	.pwr_sta2nd_offs = 0x0184
};

#endif /* __SOC_MEDIATEK_MT8183_PM_DOMAINS_H */
