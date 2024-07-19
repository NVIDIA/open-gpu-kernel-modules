/*
 * SPDX-FileCopyrightText: Copyright (c) 2022-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef NV_FIRMWARE_CHIP_FAMILY_SELECT_H
#define NV_FIRMWARE_CHIP_FAMILY_SELECT_H



#include <published/nv_arch.h>
#include <nv-firmware.h>

static inline nv_firmware_chip_family_t nv_firmware_get_chip_family(
    NvU32 gpuArch,
    NvU32 gpuImpl
)
{
    switch (gpuArch)
    {
        case GPU_ARCHITECTURE_TURING:
            if (gpuImpl <= GPU_IMPLEMENTATION_TU106)
                return NV_FIRMWARE_CHIP_FAMILY_TU10X;
            else
                return NV_FIRMWARE_CHIP_FAMILY_TU11X;

        case GPU_ARCHITECTURE_AMPERE:
            if (gpuImpl == GPU_IMPLEMENTATION_GA100)
                return NV_FIRMWARE_CHIP_FAMILY_GA100;
            else
                return NV_FIRMWARE_CHIP_FAMILY_GA10X;

        case GPU_ARCHITECTURE_ADA:
            return NV_FIRMWARE_CHIP_FAMILY_AD10X;

        case GPU_ARCHITECTURE_HOPPER:
            return NV_FIRMWARE_CHIP_FAMILY_GH100;

        case GPU_ARCHITECTURE_BLACKWELL:
            return NV_FIRMWARE_CHIP_FAMILY_GB10X;

    }

    return NV_FIRMWARE_CHIP_FAMILY_NULL;
}

#endif  // NV_FIRMWARE_CHIP_FAMILY_SELECT_H
