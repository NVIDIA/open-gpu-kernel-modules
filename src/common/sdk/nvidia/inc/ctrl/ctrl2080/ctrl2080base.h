/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#pragma once

//
// This file was generated with FINN, an NVIDIA coding tool.
// Source file: ctrl/ctrl2080/ctrl2080base.finn
//

#include "ctrl/ctrlxxxx.h"
/* NV20_SUBDEVICE_XX control commands and parameters */

#define NV2080_CTRL_CMD(cat,idx)                NVXXXX_CTRL_CMD(0x2080, NV2080_CTRL_##cat, idx)

/* Subdevice command categories (6bits) */
#define NV2080_CTRL_RESERVED                      (0x00)
#define NV2080_CTRL_GPU                           (0x01)
#define NV2080_CTRL_GPU_LEGACY_NON_PRIVILEGED     (0x81) /* finn: Evaluated from "(NV2080_CTRL_GPU | NVxxxx_CTRL_LEGACY_NON_PRIVILEGED)" */
#define NV2080_CTRL_FUSE                          (0x02)
#define NV2080_CTRL_FUSE_LEGACY_NON_PRIVILEGED    (0x82) /* finn: Evaluated from "(NV2080_CTRL_FUSE | NVxxxx_CTRL_LEGACY_NON_PRIVILEGED)" */
#define NV2080_CTRL_EVENT                         (0x03)
#define NV2080_CTRL_TIMER                         (0x04)
#define NV2080_CTRL_THERMAL                       (0x05)
#define NV2080_CTRL_THERMAL_LEGACY_PRIVILEGED     (0xc5) /* finn: Evaluated from "(NV2080_CTRL_THERMAL | NVxxxx_CTRL_LEGACY_PRIVILEGED)" */
#define NV2080_CTRL_THERMAL_LEGACY_NON_PRIVILEGED (0x85) /* finn: Evaluated from "(NV2080_CTRL_THERMAL | NVxxxx_CTRL_LEGACY_NON_PRIVILEGED)" */
#define NV2080_CTRL_I2C                           (0x06)
#define NV2080_CTRL_EXTI2C                        (0x07)
#define NV2080_CTRL_BIOS                          (0x08)
#define NV2080_CTRL_CIPHER                        (0x09)
#define NV2080_CTRL_INTERNAL                      (0x0A)
#define NV2080_CTRL_CLK_LEGACY_PRIVILEGED         (0xd0) /* finn: Evaluated from "(NV2080_CTRL_CLK | NVxxxx_CTRL_LEGACY_PRIVILEGED)" */
#define NV2080_CTRL_CLK_LEGACY_NON_PRIVILEGED     (0x90) /* finn: Evaluated from "(NV2080_CTRL_CLK | NVxxxx_CTRL_LEGACY_NON_PRIVILEGED)" */
#define NV2080_CTRL_CLK                           (0x10)
#define NV2080_CTRL_FIFO                          (0x11)
#define NV2080_CTRL_GR                            (0x12)
#define NV2080_CTRL_FB                            (0x13)
#define NV2080_CTRL_MC                            (0x17)
#define NV2080_CTRL_BUS                           (0x18)
#define NV2080_CTRL_PERF_LEGACY_PRIVILEGED        (0xe0) /* finn: Evaluated from "(NV2080_CTRL_PERF | NVxxxx_CTRL_LEGACY_PRIVILEGED)" */
#define NV2080_CTRL_PERF_LEGACY_NON_PRIVILEGED    (0xa0) /* finn: Evaluated from "(NV2080_CTRL_PERF | NVxxxx_CTRL_LEGACY_NON_PRIVILEGED)" */
#define NV2080_CTRL_PERF                          (0x20)
#define NV2080_CTRL_NVIF                          (0x21)
#define NV2080_CTRL_RC                            (0x22)
#define NV2080_CTRL_GPIO                          (0x23)
#define NV2080_CTRL_GPIO_LEGACY_NON_PRIVILEGED    (0xa3) /* finn: Evaluated from "(NV2080_CTRL_GPIO | NVxxxx_CTRL_LEGACY_NON_PRIVILEGED)" */
#define NV2080_CTRL_NVD                           (0x24)
#define NV2080_CTRL_DMA                           (0x25)
#define NV2080_CTRL_PMGR                          (0x26)
#define NV2080_CTRL_PMGR_LEGACY_PRIVILEGED        (0xe6) /* finn: Evaluated from "(NV2080_CTRL_PMGR | NVxxxx_CTRL_LEGACY_PRIVILEGED)" */
#define NV2080_CTRL_PMGR_LEGACY_NON_PRIVILEGED    (0xa6) /* finn: Evaluated from "(NV2080_CTRL_PMGR | NVxxxx_CTRL_LEGACY_NON_PRIVILEGED)" */
#define NV2080_CTRL_POWER                         (0x27)
#define NV2080_CTRL_POWER_LEGACY_NON_PRIVILEGED   (0xa7) /* finn: Evaluated from "(NV2080_CTRL_POWER | NVxxxx_CTRL_LEGACY_NON_PRIVILEGED)" */
#define NV2080_CTRL_LPWR                          (0x28)
#define NV2080_CTRL_LPWR_LEGACY_NON_PRIVILEGED    (0xa8) /* finn: Evaluated from "(NV2080_CTRL_LPWR | NVxxxx_CTRL_LEGACY_NON_PRIVILEGED)" */
#define NV2080_CTRL_LPWR_LEGACY_PRIVILEGED        (0xe8) /* finn: Evaluated from "(NV2080_CTRL_LPWR | NVxxxx_CTRL_LEGACY_PRIVILEGED)" */
#define NV2080_CTRL_ACR                           (0x29)
#define NV2080_CTRL_CE                            (0x2A)
#define NV2080_CTRL_SPI                           (0x2B)
#define NV2080_CTRL_NVLINK                        (0x30)
#define NV2080_CTRL_FLCN                          (0x31)
#define NV2080_CTRL_VOLT                          (0x32)
#define NV2080_CTRL_VOLT_LEGACY_PRIVILEGED        (0xf2) /* finn: Evaluated from "(NV2080_CTRL_VOLT | NVxxxx_CTRL_LEGACY_PRIVILEGED)" */
#define NV2080_CTRL_VOLT_LEGACY_NON_PRIVILEGED    (0xb2) /* finn: Evaluated from "(NV2080_CTRL_VOLT | NVxxxx_CTRL_LEGACY_NON_PRIVILEGED)" */
#define NV2080_CTRL_FAS                           (0x33)
#define NV2080_CTRL_ECC                           (0x34)
#define NV2080_CTRL_ECC_NON_PRIVILEGED            (0xb4) /* finn: Evaluated from "(NV2080_CTRL_ECC | NVxxxx_CTRL_LEGACY_NON_PRIVILEGED)" */
#define NV2080_CTRL_FLA                           (0x35)
#define NV2080_CTRL_GSP                           (0x36)
#define NV2080_CTRL_NNE                           (0x37)
#define NV2080_CTRL_GRMGR                         (0x38)
#define NV2080_CTRL_UCODE_FUZZER                  (0x39)
#define NV2080_CTRL_DMABUF                        (0x3A)

// per-OS categories start at highest category and work backwards
#define NV2080_CTRL_OS_WINDOWS                    (0x3F)
#define NV2080_CTRL_OS_MACOS                      (0x3E)
#define NV2080_CTRL_OS_UNIX                       (0x3D)


/*
 * NV2080_CTRL_CMD_NULL
 *
 * This command does nothing.
 * This command does not take any parameters.
 *
 * Possible status values returned are:
 *     NV_OK
 */
#define NV2080_CTRL_CMD_NULL                      (0x20800000) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_RESERVED_INTERFACE_ID << 8) | 0x0" */

/* _ctrl2080base_h_ */
