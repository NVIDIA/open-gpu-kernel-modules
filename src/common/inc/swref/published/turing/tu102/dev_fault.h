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

#ifndef __ad102_dev_fault_h__
#define __ad102_dev_fault_h__

#define NV_PFAULT_MMU_ENG_ID_DISPLAY           1 /*       */
#define NV_PFAULT_MMU_ENG_ID_IFB               9 /*       */
#define NV_PFAULT_MMU_ENG_ID_BAR1              128 /*       */
#define NV_PFAULT_MMU_ENG_ID_BAR2              192 /*       */
#define NV_PFAULT_MMU_ENG_ID_SEC               14 /*       */
#define NV_PFAULT_MMU_ENG_ID_PERF              8 /*       */
#define NV_PFAULT_MMU_ENG_ID_NVDEC             10 /*       */
#define NV_PFAULT_MMU_ENG_ID_NVDEC0            10 /*       */
#define NV_PFAULT_MMU_ENG_ID_NVDEC1            25 /*       */
#define NV_PFAULT_MMU_ENG_ID_NVDEC2            26 /*       */
#define NV_PFAULT_MMU_ENG_ID_NVJPG0            24 /*       */
#define NV_PFAULT_MMU_ENG_ID_GRCOPY            15 /*       */
#define NV_PFAULT_MMU_ENG_ID_CE0               15 /*       */
#define NV_PFAULT_MMU_ENG_ID_CE1               16 /*       */
#define NV_PFAULT_MMU_ENG_ID_CE2               17 /*       */
#define NV_PFAULT_MMU_ENG_ID_CE3               18 /*       */
#define NV_PFAULT_MMU_ENG_ID_CE4               19 /*       */
#define NV_PFAULT_MMU_ENG_ID_CE5               20 /*       */
#define NV_PFAULT_MMU_ENG_ID_CE6               21 /*       */
#define NV_PFAULT_MMU_ENG_ID_CE7               22 /*       */
#define NV_PFAULT_MMU_ENG_ID_CE8               23 /*       */
#define NV_PFAULT_MMU_ENG_ID_PWR_PMU           6 /*       */
#define NV_PFAULT_MMU_ENG_ID_PTP               3 /*       */
#define NV_PFAULT_MMU_ENG_ID_NVENC0            11 /*       */
#define NV_PFAULT_MMU_ENG_ID_NVENC1            12 /*       */
#define NV_PFAULT_MMU_ENG_ID_NVENC2            13 /*       */
#define NV_PFAULT_MMU_ENG_ID_PHYSICAL          31 /*       */

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

#define NV_PFAULT_CLIENT_HUB_CE0         0x00000001 /*       */
#define NV_PFAULT_CLIENT_HUB_CE1         0x00000002 /*       */
#define NV_PFAULT_CLIENT_HUB_DNISO       0x00000003 /*       */
#define NV_PFAULT_CLIENT_HUB_FE          0x00000004 /*       */
#define NV_PFAULT_CLIENT_HUB_FECS        0x00000005 /*       */
#define NV_PFAULT_CLIENT_HUB_HOST        0x00000006 /*       */
#define NV_PFAULT_CLIENT_HUB_HOST_CPU    0x00000007 /*       */
#define NV_PFAULT_CLIENT_HUB_HOST_CPU_NB 0x00000008 /*       */
#define NV_PFAULT_CLIENT_HUB_ISO         0x00000009 /*       */
#define NV_PFAULT_CLIENT_HUB_MMU         0x0000000A /*       */
#define NV_PFAULT_CLIENT_HUB_NVDEC       0x0000000B /*       */
#define NV_PFAULT_CLIENT_HUB_NVDEC0      0x0000000B /*       */
#define NV_PFAULT_CLIENT_HUB_NVENC1      0x0000000D /*       */
#define NV_PFAULT_CLIENT_HUB_NVENC2      0x00000033 /*       */
#define NV_PFAULT_CLIENT_HUB_NISO        0x0000000E /*       */
#define NV_PFAULT_CLIENT_HUB_P2P         0x0000000F /*       */
#define NV_PFAULT_CLIENT_HUB_PD          0x00000010 /*       */
#define NV_PFAULT_CLIENT_HUB_PERF        0x00000011 /*       */
#define NV_PFAULT_CLIENT_HUB_PMU         0x00000012 /*       */
#define NV_PFAULT_CLIENT_HUB_RASTERTWOD  0x00000013 /*       */
#define NV_PFAULT_CLIENT_HUB_SCC         0x00000014 /*       */
#define NV_PFAULT_CLIENT_HUB_SCC_NB      0x00000015 /*       */
#define NV_PFAULT_CLIENT_HUB_SEC         0x00000016 /*       */
#define NV_PFAULT_CLIENT_HUB_SSYNC       0x00000017 /*       */
#define NV_PFAULT_CLIENT_HUB_NVDEC1      0x0000003A /*       */
#define NV_PFAULT_CLIENT_HUB_NVDEC2      0x0000003B /*       */
#define NV_PFAULT_CLIENT_HUB_NVJPG0      0x0000003C /*       */
#define NV_PFAULT_CLIENT_HUB_VIP         0x00000000 /*       */
#define NV_PFAULT_CLIENT_HUB_GRCOPY      0x00000018 /*       */
#define NV_PFAULT_CLIENT_HUB_CE2         0x00000018 /*       */
#define NV_PFAULT_CLIENT_HUB_XV          0x00000019 /*       */
#define NV_PFAULT_CLIENT_HUB_MMU_NB      0x0000001A /*       */
#define NV_PFAULT_CLIENT_HUB_NVENC       0x0000001B /*       */
#define NV_PFAULT_CLIENT_HUB_NVENC0      0x0000001B /*       */
#define NV_PFAULT_CLIENT_HUB_DFALCON     0x0000001C /*       */
#define NV_PFAULT_CLIENT_HUB_SKED        0x0000001D /*       */
#define NV_PFAULT_CLIENT_HUB_AFALCON     0x0000001E /*       */
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
#define NV_PFAULT_CLIENT_HUB_DWBIF       0x00000036 /*       */
#define NV_PFAULT_CLIENT_HUB_FBFALCON    0x00000037 /*       */
#define NV_PFAULT_CLIENT_HUB_GSP         0x00000039 /*       */
#define NV_PFAULT_CLIENT_HUB_DONT_CARE   0x0000001F /*       */

#endif // _ad102_dev_fault_h__
