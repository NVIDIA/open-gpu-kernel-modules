/*
 * SPDX-FileCopyrightText: Copyright (c) 2003-2022 NVIDIA CORPORATION & AFFILIATES
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

#ifndef __tu102_dev_gc6_island_addendum_h__
#define __tu102_dev_gc6_island_addendum_h__

#define NV_PGC6_AON_FRTS_INPUT_WPR_SIZE_SECURE_SCRATCH_GROUP_03_0           NV_PGC6_AON_SECURE_SCRATCH_GROUP_03(0)
#define NV_PGC6_AON_FRTS_INPUT_WPR_SIZE_SECURE_SCRATCH_GROUP_03_0_WPR_SIZE                                   15:0
#define NV_PGC6_AON_FRTS_INPUT_WPR_SIZE_SECURE_SCRATCH_GROUP_03_0_WPR_SIZE_1MB_IN_4K                        0x100
#define NV_PGC6_AON_SECURE_SCRATCH_GROUP_05_0_GFW_BOOT                                                NV_PGC6_AON_SECURE_SCRATCH_GROUP_05(0)
#define NV_PGC6_AON_SECURE_SCRATCH_GROUP_05_0_GFW_BOOT_PROGRESS                                                                          7:0
#define NV_PGC6_AON_SECURE_SCRATCH_GROUP_05_0_GFW_BOOT_PROGRESS_COMPLETED                                                         0x000000FF
#define NV_PGC6_BSI_SECURE_SCRATCH_14_BOOT_STAGE_3_HANDOFF                                  26:26
#define NV_PGC6_BSI_SECURE_SCRATCH_14_BOOT_STAGE_3_HANDOFF_VALUE_INIT                         0x0
#define NV_PGC6_BSI_SECURE_SCRATCH_14_BOOT_STAGE_3_HANDOFF_VALUE_DONE                         0x1

#endif // __tu102_dev_gc6_island_addendum_h__

