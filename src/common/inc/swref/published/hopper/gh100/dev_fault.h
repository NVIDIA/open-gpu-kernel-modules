/*
 * SPDX-FileCopyrightText: Copyright (c) 2003-2023 NVIDIA CORPORATION & AFFILIATES
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef __gh100_dev_fault_h__
#define __gh100_dev_fault_h__

#define NV_PFAULT_MMU_ENG_ID_GRAPHICS          384 /*       */
#define NV_PFAULT_MMU_ENG_ID_DISPLAY           1 /*       */
#define NV_PFAULT_MMU_ENG_ID_GSP               2 /*       */
#define NV_PFAULT_MMU_ENG_ID_FLA               4 /*       */
#define NV_PFAULT_MMU_ENG_ID_BAR1              256 /*       */
#define NV_PFAULT_MMU_ENG_ID_BAR2              320 /*       */
#define NV_PFAULT_MMU_ENG_ID_FSP               7 /*       */
#define NV_PFAULT_MMU_ENG_ID_PERF              10 /*       */
#define NV_PFAULT_MMU_ENG_ID_PERF0             10 /*        */
#define NV_PFAULT_MMU_ENG_ID_PWR_PMU           5 /*       */
#define NV_PFAULT_MMU_ENG_ID_PTP               3 /*       */
#define NV_PFAULT_MMU_ENG_ID_PHYSICAL          56 /*       */
#define NV_PFAULT_MMU_ENG_ID_CE0               43 /*       */
#define NV_PFAULT_MMU_ENG_ID_CE1               44 /*       */
#define NV_PFAULT_MMU_ENG_ID_CE2               45 /*       */
#define NV_PFAULT_MMU_ENG_ID_CE3               46 /*       */
#define NV_PFAULT_MMU_ENG_ID_CE4               47 /*       */
#define NV_PFAULT_MMU_ENG_ID_CE5               48 /*       */
#define NV_PFAULT_MMU_ENG_ID_CE6               49 /*       */
#define NV_PFAULT_MMU_ENG_ID_CE7               50 /*       */
#define NV_PFAULT_MMU_ENG_ID_CE8               51 /*       */
#define NV_PFAULT_MMU_ENG_ID_CE9               52 /*       */

#define NV_PFAULT_CLIENT_GPC_T1_0        0x00000000 /*       */
#define NV_PFAULT_CLIENT_GPC_T1_1        0x00000001 /*       */
#define NV_PFAULT_CLIENT_GPC_T1_2        0x00000002 /*       */
#define NV_PFAULT_CLIENT_GPC_T1_3        0x00000003 /*       */
#define NV_PFAULT_CLIENT_GPC_T1_4        0x00000004 /*       */
#define NV_PFAULT_CLIENT_GPC_T1_5        0x00000005 /*       */
#define NV_PFAULT_CLIENT_GPC_T1_6        0x00000006 /*       */
#define NV_PFAULT_CLIENT_GPC_T1_7        0x00000007 /*       */
#define NV_PFAULT_CLIENT_GPC_PE_0        0x00000008 /*       */
#define NV_PFAULT_CLIENT_GPC_PE_1        0x00000009 /*       */
#define NV_PFAULT_CLIENT_GPC_PE_2        0x0000000A /*       */
#define NV_PFAULT_CLIENT_GPC_PE_3        0x0000000B /*       */
#define NV_PFAULT_CLIENT_GPC_PE_4        0x0000000C /*       */
#define NV_PFAULT_CLIENT_GPC_PE_5        0x0000000D /*       */
#define NV_PFAULT_CLIENT_GPC_PE_6        0x0000000E /*       */
#define NV_PFAULT_CLIENT_GPC_PE_7        0x0000000F /*       */
#define NV_PFAULT_CLIENT_GPC_RAST        0x00000010 /*       */
#define NV_PFAULT_CLIENT_GPC_GCC         0x00000011 /*       */
#define NV_PFAULT_CLIENT_GPC_GPCCS       0x00000012 /*       */
#define NV_PFAULT_CLIENT_GPC_PROP_0      0x00000013 /*       */
#define NV_PFAULT_CLIENT_GPC_PROP_1      0x00000014 /*       */
#define NV_PFAULT_CLIENT_GPC_T1_8        0x00000021 /*       */
#define NV_PFAULT_CLIENT_GPC_T1_9        0x00000022 /*       */
#define NV_PFAULT_CLIENT_GPC_T1_10       0x00000023 /*       */
#define NV_PFAULT_CLIENT_GPC_T1_11       0x00000024 /*       */
#define NV_PFAULT_CLIENT_GPC_T1_12       0x00000025 /*       */
#define NV_PFAULT_CLIENT_GPC_T1_13       0x00000026 /*       */
#define NV_PFAULT_CLIENT_GPC_T1_14       0x00000027 /*       */
#define NV_PFAULT_CLIENT_GPC_T1_15       0x00000028 /*       */
#define NV_PFAULT_CLIENT_GPC_TPCCS_0     0x00000029 /*       */
#define NV_PFAULT_CLIENT_GPC_TPCCS_1     0x0000002A /*       */
#define NV_PFAULT_CLIENT_GPC_TPCCS_2     0x0000002B /*       */
#define NV_PFAULT_CLIENT_GPC_TPCCS_3     0x0000002C /*       */
#define NV_PFAULT_CLIENT_GPC_TPCCS_4     0x0000002D /*       */
#define NV_PFAULT_CLIENT_GPC_TPCCS_5     0x0000002E /*       */
#define NV_PFAULT_CLIENT_GPC_TPCCS_6     0x0000002F /*       */
#define NV_PFAULT_CLIENT_GPC_TPCCS_7     0x00000030 /*       */
#define NV_PFAULT_CLIENT_GPC_PE_8        0x00000031 /*       */
#define NV_PFAULT_CLIENT_GPC_TPCCS_8     0x00000033 /*       */
#define NV_PFAULT_CLIENT_GPC_T1_16       0x00000035 /*       */
#define NV_PFAULT_CLIENT_GPC_T1_17       0x00000036 /*       */
#define NV_PFAULT_CLIENT_GPC_ROP_0       0x00000070 /*       */
#define NV_PFAULT_CLIENT_GPC_ROP_1       0x00000071 /*       */
#define NV_PFAULT_CLIENT_GPC_GPM         0x00000017 /*       */

#define NV_PFAULT_CLIENT_HUB_VIP         0x00000000 /*       */
#define NV_PFAULT_CLIENT_HUB_CE0         0x00000001 /*       */
#define NV_PFAULT_CLIENT_HUB_CE1         0x00000002 /*       */
#define NV_PFAULT_CLIENT_HUB_DNISO       0x00000003 /*       */
#define NV_PFAULT_CLIENT_HUB_DISPNISO    0x00000003 /*       */
#define NV_PFAULT_CLIENT_HUB_FE0         0x00000004 /*       */
#define NV_PFAULT_CLIENT_HUB_FE          0x00000004 /*       */
#define NV_PFAULT_CLIENT_HUB_FECS0       0x00000005 /*       */
#define NV_PFAULT_CLIENT_HUB_FECS        0x00000005 /*       */
#define NV_PFAULT_CLIENT_HUB_HOST        0x00000006 /*       */
#define NV_PFAULT_CLIENT_HUB_HOST_CPU    0x00000007 /*       */
#define NV_PFAULT_CLIENT_HUB_HOST_CPU_NB 0x00000008 /*       */
#define NV_PFAULT_CLIENT_HUB_ISO         0x00000009 /*       */
#define NV_PFAULT_CLIENT_HUB_MMU         0x0000000A /*       */
#define NV_PFAULT_CLIENT_HUB_NVDEC0      0x0000000B /*       */
#define NV_PFAULT_CLIENT_HUB_NVDEC       0x0000000B /*       */
#define NV_PFAULT_CLIENT_HUB_CE3         0x0000000C /*       */
#define NV_PFAULT_CLIENT_HUB_NVENC1      0x0000000D /*       */
#define NV_PFAULT_CLIENT_HUB_NISO        0x0000000E /*       */
#define NV_PFAULT_CLIENT_HUB_ACTRS       0x0000000E /*       */
#define NV_PFAULT_CLIENT_HUB_P2P         0x0000000F /*       */
#define NV_PFAULT_CLIENT_HUB_PD          0x00000010 /*       */
#define NV_PFAULT_CLIENT_HUB_PERF0       0x00000011 /*       */
#define NV_PFAULT_CLIENT_HUB_PERF        0x00000011 /*       */
#define NV_PFAULT_CLIENT_HUB_PMU         0x00000012 /*       */
#define NV_PFAULT_CLIENT_HUB_RASTERTWOD  0x00000013 /*       */
#define NV_PFAULT_CLIENT_HUB_SCC         0x00000014 /*       */
#define NV_PFAULT_CLIENT_HUB_SCC_NB      0x00000015 /*       */
#define NV_PFAULT_CLIENT_HUB_SEC         0x00000016 /*       */
#define NV_PFAULT_CLIENT_HUB_SSYNC       0x00000017 /*       */
#define NV_PFAULT_CLIENT_HUB_GRCOPY      0x00000018 /*       */
#define NV_PFAULT_CLIENT_HUB_CE2         0x00000018 /*       */
#define NV_PFAULT_CLIENT_HUB_XV          0x00000019 /*       */
#define NV_PFAULT_CLIENT_HUB_MMU_NB      0x0000001A /*       */
#define NV_PFAULT_CLIENT_HUB_NVENC0      0x0000001B /*       */
#define NV_PFAULT_CLIENT_HUB_NVENC       0x0000001B /*       */
#define NV_PFAULT_CLIENT_HUB_DFALCON     0x0000001C /*       */
#define NV_PFAULT_CLIENT_HUB_SKED0       0x0000001D /*       */
#define NV_PFAULT_CLIENT_HUB_SKED        0x0000001D /*       */
#define NV_PFAULT_CLIENT_HUB_AFALCON     0x0000001E /*       */
#define NV_PFAULT_CLIENT_HUB_DONT_CARE   0x0000001F /*       */
#define NV_PFAULT_CLIENT_HUB_HSCE0       0x00000020 /*       */
#define NV_PFAULT_CLIENT_HUB_HSCE1       0x00000021 /*       */
#define NV_PFAULT_CLIENT_HUB_HSCE2       0x00000022 /*       */
#define NV_PFAULT_CLIENT_HUB_HSCE3       0x00000023 /*       */
#define NV_PFAULT_CLIENT_HUB_HSCE4       0x00000024 /*       */
#define NV_PFAULT_CLIENT_HUB_HSCE5       0x00000025 /*       */
#define NV_PFAULT_CLIENT_HUB_HSCE6       0x00000026 /*       */
#define NV_PFAULT_CLIENT_HUB_HSCE7       0x00000027 /*       */
#define NV_PFAULT_CLIENT_HUB_HSCE8       0x00000028 /*       */
#define NV_PFAULT_CLIENT_HUB_HSCE9       0x00000029 /*       */
#define NV_PFAULT_CLIENT_HUB_HSHUB       0x0000002A /*       */
#define NV_PFAULT_CLIENT_HUB_PTP_X0      0x0000002B /*       */
#define NV_PFAULT_CLIENT_HUB_PTP_X1      0x0000002C /*       */
#define NV_PFAULT_CLIENT_HUB_PTP_X2      0x0000002D /*       */
#define NV_PFAULT_CLIENT_HUB_PTP_X3      0x0000002E /*       */
#define NV_PFAULT_CLIENT_HUB_PTP_X4      0x0000002F /*       */
#define NV_PFAULT_CLIENT_HUB_PTP_X5      0x00000030 /*       */
#define NV_PFAULT_CLIENT_HUB_PTP_X6      0x00000031 /*       */
#define NV_PFAULT_CLIENT_HUB_PTP_X7      0x00000032 /*       */
#define NV_PFAULT_CLIENT_HUB_NVENC2      0x00000033 /*       */
#define NV_PFAULT_CLIENT_HUB_VPR_SCRUBBER0 0x00000034 /*       */
#define NV_PFAULT_CLIENT_HUB_VPR_SCRUBBER1 0x00000035 /*       */
#define NV_PFAULT_CLIENT_HUB_DWBIF       0x00000036 /*       */
#define NV_PFAULT_CLIENT_HUB_FBFALCON    0x00000037 /*       */
#define NV_PFAULT_CLIENT_HUB_CE_SHIM     0x00000038 /*       */
#define NV_PFAULT_CLIENT_HUB_GSP         0x00000039 /*       */
#define NV_PFAULT_CLIENT_HUB_NVDEC1      0x0000003A /*       */
#define NV_PFAULT_CLIENT_HUB_NVDEC2      0x0000003B /*       */
#define NV_PFAULT_CLIENT_HUB_NVJPG0      0x0000003C /*       */
#define NV_PFAULT_CLIENT_HUB_NVDEC3      0x0000003D /*       */
#define NV_PFAULT_CLIENT_HUB_NVDEC4      0x0000003E /*       */
#define NV_PFAULT_CLIENT_HUB_OFA0        0x0000003F /*       */
#define NV_PFAULT_CLIENT_HUB_HSCE10      0x00000040 /*       */
#define NV_PFAULT_CLIENT_HUB_HSCE11      0x00000041 /*       */
#define NV_PFAULT_CLIENT_HUB_HSCE12      0x00000042 /*       */
#define NV_PFAULT_CLIENT_HUB_HSCE13      0x00000043 /*       */
#define NV_PFAULT_CLIENT_HUB_HSCE14      0x00000044 /*       */
#define NV_PFAULT_CLIENT_HUB_HSCE15      0x00000045 /*       */
#define NV_PFAULT_CLIENT_HUB_FE1         0x0000004E /*       */
#define NV_PFAULT_CLIENT_HUB_FE2         0x0000004F /*       */
#define NV_PFAULT_CLIENT_HUB_FE3         0x00000050 /*       */
#define NV_PFAULT_CLIENT_HUB_FE4         0x00000051 /*       */
#define NV_PFAULT_CLIENT_HUB_FE5         0x00000052 /*       */
#define NV_PFAULT_CLIENT_HUB_FE6         0x00000053 /*       */
#define NV_PFAULT_CLIENT_HUB_FE7         0x00000054 /*       */
#define NV_PFAULT_CLIENT_HUB_FECS1       0x00000055 /*       */
#define NV_PFAULT_CLIENT_HUB_FECS2       0x00000056 /*       */
#define NV_PFAULT_CLIENT_HUB_FECS3       0x00000057 /*       */
#define NV_PFAULT_CLIENT_HUB_FECS4       0x00000058 /*       */
#define NV_PFAULT_CLIENT_HUB_FECS5       0x00000059 /*       */
#define NV_PFAULT_CLIENT_HUB_FECS6       0x0000005A /*       */
#define NV_PFAULT_CLIENT_HUB_FECS7       0x0000005B /*       */
#define NV_PFAULT_CLIENT_HUB_SKED1       0x0000005C /*       */
#define NV_PFAULT_CLIENT_HUB_SKED2       0x0000005D /*       */
#define NV_PFAULT_CLIENT_HUB_SKED3       0x0000005E /*       */
#define NV_PFAULT_CLIENT_HUB_SKED4       0x0000005F /*       */
#define NV_PFAULT_CLIENT_HUB_SKED5       0x00000060 /*       */
#define NV_PFAULT_CLIENT_HUB_SKED6       0x00000061 /*       */
#define NV_PFAULT_CLIENT_HUB_SKED7       0x00000062 /*       */
#define NV_PFAULT_CLIENT_HUB_ESC         0x00000063 /*       */
#define NV_PFAULT_CLIENT_HUB_NVDEC5      0x0000006F /*       */
#define NV_PFAULT_CLIENT_HUB_NVDEC6      0x00000070 /*       */
#define NV_PFAULT_CLIENT_HUB_NVDEC7      0x00000071 /*       */
#define NV_PFAULT_CLIENT_HUB_NVJPG1      0x00000072 /*       */
#define NV_PFAULT_CLIENT_HUB_NVJPG2      0x00000073 /*       */
#define NV_PFAULT_CLIENT_HUB_NVJPG3      0x00000074 /*       */
#define NV_PFAULT_CLIENT_HUB_NVJPG4      0x00000075 /*       */
#define NV_PFAULT_CLIENT_HUB_NVJPG5      0x00000076 /*       */
#define NV_PFAULT_CLIENT_HUB_NVJPG6      0x00000077 /*       */
#define NV_PFAULT_CLIENT_HUB_NVJPG7      0x00000078 /*       */
#define NV_PFAULT_CLIENT_HUB_FSP         0x00000079 /*       */

#endif // __gh100_dev_fault_h__
