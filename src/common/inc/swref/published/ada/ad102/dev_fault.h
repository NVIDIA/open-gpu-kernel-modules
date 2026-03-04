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
#define NV_PFAULT_MMU_ENG_ID_FLA               4 /*       */
#define NV_PFAULT_MMU_ENG_ID_BAR1              128 /*       */
#define NV_PFAULT_MMU_ENG_ID_BAR2              192 /*       */
#define NV_PFAULT_MMU_ENG_ID_SEC               14 /*       */
#define NV_PFAULT_MMU_ENG_ID_PERF              8 /*       */
#define NV_PFAULT_MMU_ENG_ID_NVDEC             25 /*       */
#define NV_PFAULT_MMU_ENG_ID_NVDEC0            25 /*       */
#define NV_PFAULT_MMU_ENG_ID_NVDEC1            26 /*       */
#define NV_PFAULT_MMU_ENG_ID_NVDEC2            27 /*       */
#define NV_PFAULT_MMU_ENG_ID_NVDEC3            28 /*       */
#define NV_PFAULT_MMU_ENG_ID_NVDEC4            29 /*       */
#define NV_PFAULT_MMU_ENG_ID_NVJPG0            21 /*       */
#define NV_PFAULT_MMU_ENG_ID_NVJPG1            22 /*       */
#define NV_PFAULT_MMU_ENG_ID_NVJPG2            23 /*       */
#define NV_PFAULT_MMU_ENG_ID_NVJPG3            24 /*       */
#define NV_PFAULT_MMU_ENG_ID_GRCOPY            15 /*       */
#define NV_PFAULT_MMU_ENG_ID_CE0               15 /*       */
#define NV_PFAULT_MMU_ENG_ID_CE1               16 /*       */
#define NV_PFAULT_MMU_ENG_ID_CE2               17 /*       */
#define NV_PFAULT_MMU_ENG_ID_CE3               18 /*       */
#define NV_PFAULT_MMU_ENG_ID_CE4               19 /*       */
#define NV_PFAULT_MMU_ENG_ID_CE5               20 /*       */
#define NV_PFAULT_MMU_ENG_ID_PWR_PMU           6 /*       */
#define NV_PFAULT_MMU_ENG_ID_PTP               3 /*       */
#define NV_PFAULT_MMU_ENG_ID_NVENC0            11 /*       */
#define NV_PFAULT_MMU_ENG_ID_NVENC1            12 /*       */
#define NV_PFAULT_MMU_ENG_ID_NVENC2            13 /*       */
#define NV_PFAULT_MMU_ENG_ID_OFA0              10 /*       */
#define NV_PFAULT_MMU_ENG_ID_PHYSICAL          31 /*       */

#define NV_PFAULT_CLIENT_HUB_NVJPG1      0x00000072 /*       */
#define NV_PFAULT_CLIENT_HUB_NVJPG2      0x00000073 /*       */
#define NV_PFAULT_CLIENT_HUB_NVJPG3      0x00000074 /*       */

#endif // __ad102_dev_fault_h__
