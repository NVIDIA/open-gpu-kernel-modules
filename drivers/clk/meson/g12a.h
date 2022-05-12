/* SPDX-License-Identifier: (GPL-2.0+ OR MIT) */
/*
 * Copyright (c) 2016 Amlogic, Inc.
 * Author: Michael Turquette <mturquette@baylibre.com>
 *
 * Copyright (c) 2018 Amlogic, inc.
 * Author: Qiufang Dai <qiufang.dai@amlogic.com>
 * Author: Jian Hu <jian.hu@amlogic.com>
 *
 */
#ifndef __G12A_H
#define __G12A_H

/*
 * Clock controller register offsets
 *
 * Register offsets from the data sheet must be multiplied by 4 before
 * adding them to the base address to get the right value.
 */
#define HHI_MIPI_CNTL0			0x000
#define HHI_MIPI_CNTL1			0x004
#define HHI_MIPI_CNTL2			0x008
#define HHI_MIPI_STS			0x00C
#define HHI_GP0_PLL_CNTL0		0x040
#define HHI_GP0_PLL_CNTL1		0x044
#define HHI_GP0_PLL_CNTL2		0x048
#define HHI_GP0_PLL_CNTL3		0x04C
#define HHI_GP0_PLL_CNTL4		0x050
#define HHI_GP0_PLL_CNTL5		0x054
#define HHI_GP0_PLL_CNTL6		0x058
#define HHI_GP0_PLL_STS			0x05C
#define HHI_GP1_PLL_CNTL0		0x060
#define HHI_GP1_PLL_CNTL1		0x064
#define HHI_GP1_PLL_CNTL2		0x068
#define HHI_GP1_PLL_CNTL3		0x06C
#define HHI_GP1_PLL_CNTL4		0x070
#define HHI_GP1_PLL_CNTL5		0x074
#define HHI_GP1_PLL_CNTL6		0x078
#define HHI_GP1_PLL_STS			0x07C
#define HHI_PCIE_PLL_CNTL0		0x098
#define HHI_PCIE_PLL_CNTL1		0x09C
#define HHI_PCIE_PLL_CNTL2		0x0A0
#define HHI_PCIE_PLL_CNTL3		0x0A4
#define HHI_PCIE_PLL_CNTL4		0x0A8
#define HHI_PCIE_PLL_CNTL5		0x0AC
#define HHI_PCIE_PLL_STS		0x0B8
#define HHI_HIFI_PLL_CNTL0		0x0D8
#define HHI_HIFI_PLL_CNTL1		0x0DC
#define HHI_HIFI_PLL_CNTL2		0x0E0
#define HHI_HIFI_PLL_CNTL3		0x0E4
#define HHI_HIFI_PLL_CNTL4		0x0E8
#define HHI_HIFI_PLL_CNTL5		0x0EC
#define HHI_HIFI_PLL_CNTL6		0x0F0
#define HHI_VIID_CLK_DIV		0x128
#define HHI_VIID_CLK_CNTL		0x12C
#define HHI_GCLK_MPEG0			0x140
#define HHI_GCLK_MPEG1			0x144
#define HHI_GCLK_MPEG2			0x148
#define HHI_GCLK_OTHER			0x150
#define HHI_GCLK_OTHER2			0x154
#define HHI_SYS_CPU_CLK_CNTL1		0x15c
#define HHI_VID_CLK_DIV			0x164
#define HHI_MPEG_CLK_CNTL		0x174
#define HHI_AUD_CLK_CNTL		0x178
#define HHI_VID_CLK_CNTL		0x17c
#define HHI_TS_CLK_CNTL			0x190
#define HHI_VID_CLK_CNTL2		0x194
#define HHI_SYS_CPU_CLK_CNTL0		0x19c
#define HHI_VID_PLL_CLK_DIV		0x1A0
#define HHI_MALI_CLK_CNTL		0x1b0
#define HHI_VPU_CLKC_CNTL		0x1b4
#define HHI_VPU_CLK_CNTL		0x1bC
#define HHI_NNA_CLK_CNTL		0x1C8
#define HHI_HDMI_CLK_CNTL		0x1CC
#define HHI_VDEC_CLK_CNTL		0x1E0
#define HHI_VDEC2_CLK_CNTL		0x1E4
#define HHI_VDEC3_CLK_CNTL		0x1E8
#define HHI_VDEC4_CLK_CNTL		0x1EC
#define HHI_HDCP22_CLK_CNTL		0x1F0
#define HHI_VAPBCLK_CNTL		0x1F4
#define HHI_SYS_CPUB_CLK_CNTL1		0x200
#define HHI_SYS_CPUB_CLK_CNTL		0x208
#define HHI_VPU_CLKB_CNTL		0x20C
#define HHI_SYS_CPU_CLK_CNTL2		0x210
#define HHI_SYS_CPU_CLK_CNTL3		0x214
#define HHI_SYS_CPU_CLK_CNTL4		0x218
#define HHI_SYS_CPU_CLK_CNTL5		0x21c
#define HHI_SYS_CPU_CLK_CNTL6		0x220
#define HHI_GEN_CLK_CNTL		0x228
#define HHI_VDIN_MEAS_CLK_CNTL		0x250
#define HHI_MIPIDSI_PHY_CLK_CNTL	0x254
#define HHI_NAND_CLK_CNTL		0x25C
#define HHI_SD_EMMC_CLK_CNTL		0x264
#define HHI_MPLL_CNTL0			0x278
#define HHI_MPLL_CNTL1			0x27C
#define HHI_MPLL_CNTL2			0x280
#define HHI_MPLL_CNTL3			0x284
#define HHI_MPLL_CNTL4			0x288
#define HHI_MPLL_CNTL5			0x28c
#define HHI_MPLL_CNTL6			0x290
#define HHI_MPLL_CNTL7			0x294
#define HHI_MPLL_CNTL8			0x298
#define HHI_FIX_PLL_CNTL0		0x2A0
#define HHI_FIX_PLL_CNTL1		0x2A4
#define HHI_FIX_PLL_CNTL3		0x2AC
#define HHI_SYS_PLL_CNTL0		0x2f4
#define HHI_SYS_PLL_CNTL1		0x2f8
#define HHI_SYS_PLL_CNTL2		0x2fc
#define HHI_SYS_PLL_CNTL3		0x300
#define HHI_SYS_PLL_CNTL4		0x304
#define HHI_SYS_PLL_CNTL5		0x308
#define HHI_SYS_PLL_CNTL6		0x30c
#define HHI_HDMI_PLL_CNTL0		0x320
#define HHI_HDMI_PLL_CNTL1		0x324
#define HHI_HDMI_PLL_CNTL2		0x328
#define HHI_HDMI_PLL_CNTL3		0x32c
#define HHI_HDMI_PLL_CNTL4		0x330
#define HHI_HDMI_PLL_CNTL5		0x334
#define HHI_HDMI_PLL_CNTL6		0x338
#define HHI_SPICC_CLK_CNTL		0x3dc
#define HHI_SYS1_PLL_CNTL0		0x380
#define HHI_SYS1_PLL_CNTL1		0x384
#define HHI_SYS1_PLL_CNTL2		0x388
#define HHI_SYS1_PLL_CNTL3		0x38c
#define HHI_SYS1_PLL_CNTL4		0x390
#define HHI_SYS1_PLL_CNTL5		0x394
#define HHI_SYS1_PLL_CNTL6		0x398

/*
 * CLKID index values
 *
 * These indices are entirely contrived and do not map onto the hardware.
 * It has now been decided to expose everything by default in the DT header:
 * include/dt-bindings/clock/g12a-clkc.h. Only the clocks ids we don't want
 * to expose, such as the internal muxes and dividers of composite clocks,
 * will remain defined here.
 */
#define CLKID_MPEG_SEL				8
#define CLKID_MPEG_DIV				9
#define CLKID_SD_EMMC_A_CLK0_SEL		63
#define CLKID_SD_EMMC_A_CLK0_DIV		64
#define CLKID_SD_EMMC_B_CLK0_SEL		65
#define CLKID_SD_EMMC_B_CLK0_DIV		66
#define CLKID_SD_EMMC_C_CLK0_SEL		67
#define CLKID_SD_EMMC_C_CLK0_DIV		68
#define CLKID_MPLL0_DIV				69
#define CLKID_MPLL1_DIV				70
#define CLKID_MPLL2_DIV				71
#define CLKID_MPLL3_DIV				72
#define CLKID_MPLL_PREDIV			73
#define CLKID_FCLK_DIV2_DIV			75
#define CLKID_FCLK_DIV3_DIV			76
#define CLKID_FCLK_DIV4_DIV			77
#define CLKID_FCLK_DIV5_DIV			78
#define CLKID_FCLK_DIV7_DIV			79
#define CLKID_FCLK_DIV2P5_DIV			100
#define CLKID_FIXED_PLL_DCO			101
#define CLKID_SYS_PLL_DCO			102
#define CLKID_GP0_PLL_DCO			103
#define CLKID_HIFI_PLL_DCO			104
#define CLKID_VPU_0_DIV				111
#define CLKID_VPU_1_DIV				114
#define CLKID_VAPB_0_DIV			118
#define CLKID_VAPB_1_DIV			121
#define CLKID_HDMI_PLL_DCO			125
#define CLKID_HDMI_PLL_OD			126
#define CLKID_HDMI_PLL_OD2			127
#define CLKID_VID_PLL_SEL			130
#define CLKID_VID_PLL_DIV			131
#define CLKID_VCLK_SEL				132
#define CLKID_VCLK2_SEL				133
#define CLKID_VCLK_INPUT			134
#define CLKID_VCLK2_INPUT			135
#define CLKID_VCLK_DIV				136
#define CLKID_VCLK2_DIV				137
#define CLKID_VCLK_DIV2_EN			140
#define CLKID_VCLK_DIV4_EN			141
#define CLKID_VCLK_DIV6_EN			142
#define CLKID_VCLK_DIV12_EN			143
#define CLKID_VCLK2_DIV2_EN			144
#define CLKID_VCLK2_DIV4_EN			145
#define CLKID_VCLK2_DIV6_EN			146
#define CLKID_VCLK2_DIV12_EN			147
#define CLKID_CTS_ENCI_SEL			158
#define CLKID_CTS_ENCP_SEL			159
#define CLKID_CTS_VDAC_SEL			160
#define CLKID_HDMI_TX_SEL			161
#define CLKID_HDMI_SEL				166
#define CLKID_HDMI_DIV				167
#define CLKID_MALI_0_DIV			170
#define CLKID_MALI_1_DIV			173
#define CLKID_MPLL_50M_DIV			176
#define CLKID_SYS_PLL_DIV16_EN			178
#define CLKID_SYS_PLL_DIV16			179
#define CLKID_CPU_CLK_DYN0_SEL			180
#define CLKID_CPU_CLK_DYN0_DIV			181
#define CLKID_CPU_CLK_DYN0			182
#define CLKID_CPU_CLK_DYN1_SEL			183
#define CLKID_CPU_CLK_DYN1_DIV			184
#define CLKID_CPU_CLK_DYN1			185
#define CLKID_CPU_CLK_DYN			186
#define CLKID_CPU_CLK_DIV16_EN			188
#define CLKID_CPU_CLK_DIV16			189
#define CLKID_CPU_CLK_APB_DIV			190
#define CLKID_CPU_CLK_APB			191
#define CLKID_CPU_CLK_ATB_DIV			192
#define CLKID_CPU_CLK_ATB			193
#define CLKID_CPU_CLK_AXI_DIV			194
#define CLKID_CPU_CLK_AXI			195
#define CLKID_CPU_CLK_TRACE_DIV			196
#define CLKID_CPU_CLK_TRACE			197
#define CLKID_PCIE_PLL_DCO			198
#define CLKID_PCIE_PLL_DCO_DIV2			199
#define CLKID_PCIE_PLL_OD			200
#define CLKID_VDEC_1_SEL			202
#define CLKID_VDEC_1_DIV			203
#define CLKID_VDEC_HEVC_SEL			205
#define CLKID_VDEC_HEVC_DIV			206
#define CLKID_VDEC_HEVCF_SEL			208
#define CLKID_VDEC_HEVCF_DIV			209
#define CLKID_TS_DIV				211
#define CLKID_SYS1_PLL_DCO			213
#define CLKID_SYS1_PLL				214
#define CLKID_SYS1_PLL_DIV16_EN			215
#define CLKID_SYS1_PLL_DIV16			216
#define CLKID_CPUB_CLK_DYN0_SEL			217
#define CLKID_CPUB_CLK_DYN0_DIV			218
#define CLKID_CPUB_CLK_DYN0			219
#define CLKID_CPUB_CLK_DYN1_SEL			220
#define CLKID_CPUB_CLK_DYN1_DIV			221
#define CLKID_CPUB_CLK_DYN1			222
#define CLKID_CPUB_CLK_DYN			223
#define CLKID_CPUB_CLK_DIV16_EN			225
#define CLKID_CPUB_CLK_DIV16			226
#define CLKID_CPUB_CLK_DIV2			227
#define CLKID_CPUB_CLK_DIV3			228
#define CLKID_CPUB_CLK_DIV4			229
#define CLKID_CPUB_CLK_DIV5			230
#define CLKID_CPUB_CLK_DIV6			231
#define CLKID_CPUB_CLK_DIV7			232
#define CLKID_CPUB_CLK_DIV8			233
#define CLKID_CPUB_CLK_APB_SEL			234
#define CLKID_CPUB_CLK_APB			235
#define CLKID_CPUB_CLK_ATB_SEL			236
#define CLKID_CPUB_CLK_ATB			237
#define CLKID_CPUB_CLK_AXI_SEL			238
#define CLKID_CPUB_CLK_AXI			239
#define CLKID_CPUB_CLK_TRACE_SEL		240
#define CLKID_CPUB_CLK_TRACE			241
#define CLKID_GP1_PLL_DCO			242
#define CLKID_DSU_CLK_DYN0_SEL			244
#define CLKID_DSU_CLK_DYN0_DIV			245
#define CLKID_DSU_CLK_DYN0			246
#define CLKID_DSU_CLK_DYN1_SEL			247
#define CLKID_DSU_CLK_DYN1_DIV			248
#define CLKID_DSU_CLK_DYN1			249
#define CLKID_DSU_CLK_DYN			250
#define CLKID_DSU_CLK_FINAL			251
#define CLKID_SPICC0_SCLK_SEL			256
#define CLKID_SPICC0_SCLK_DIV			257
#define CLKID_SPICC1_SCLK_SEL			259
#define CLKID_SPICC1_SCLK_DIV			260
#define CLKID_NNA_AXI_CLK_SEL			262
#define CLKID_NNA_AXI_CLK_DIV			263
#define CLKID_NNA_CORE_CLK_SEL			265
#define CLKID_NNA_CORE_CLK_DIV			266
#define CLKID_MIPI_DSI_PXCLK_DIV		268

#define NR_CLKS					271

/* include the CLKIDs that have been made part of the DT binding */
#include <dt-bindings/clock/g12a-clkc.h>

#endif /* __G12A_H */
