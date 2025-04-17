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
#include <nvctassert.h>

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

        case GPU_ARCHITECTURE_BLACKWELL_GB1XX:
            return NV_FIRMWARE_CHIP_FAMILY_GB10X;

        case GPU_ARCHITECTURE_BLACKWELL_GB2XX:
            return NV_FIRMWARE_CHIP_FAMILY_GB20X;

    }

    return NV_FIRMWARE_CHIP_FAMILY_NULL;
}

typedef enum
{
    NV_FIRMWARE_TASK_INIT,
    NV_FIRMWARE_TASK_RM,
    NV_FIRMWARE_TASK_INTR,
    NV_FIRMWARE_TASK_VGPU,
    NV_FIRMWARE_TASK_MNOC,
    NV_FIRMWARE_TASK_DEBUG,
    NV_FIRMWARE_TASK_ROOT,

    NV_FIRMWARE_TASK_COUNT
} nv_firmware_task_t;

typedef enum {
    NV_FIRMWARE_LIBOS_NONE_SUPPORTED = 0x0,
    NV_FIRMWARE_LIBOS2_SUPPORTED     = 0x1,
    NV_FIRMWARE_LIBOS3_SUPPORTED     = 0x2,
} libosMappingType;

typedef struct
{
    const char *memory_id;
    const char *prefix;
    const char *elf_section_name;
    const libosMappingType supported_os;
} nv_firmware_task_log_info_t;

static inline
const nv_firmware_task_log_info_t *nv_firmware_get_task_log_infos(unsigned *num_infos)
{
    const static nv_firmware_task_log_info_t infos[] =
    {
        {"LOGINIT", "INIT", ".fwlogging_init",  NV_FIRMWARE_LIBOS2_SUPPORTED | NV_FIRMWARE_LIBOS3_SUPPORTED},
        {"LOGRM",   "RM",   ".fwlogging_rm",    NV_FIRMWARE_LIBOS2_SUPPORTED | NV_FIRMWARE_LIBOS3_SUPPORTED},
        {"LOGINTR", "INTR", ".fwlogging_rm",    NV_FIRMWARE_LIBOS3_SUPPORTED},
        {"LOGVGPU", "VGPU", ".fwlogging_vgpu",  NV_FIRMWARE_LIBOS2_SUPPORTED | NV_FIRMWARE_LIBOS3_SUPPORTED},
        {"LOGMNOC", "MNOC", ".fwlogging_mnoc",  NV_FIRMWARE_LIBOS3_SUPPORTED},
        {"LOGDBG",  "DBG",  ".fwlogging_debug", NV_FIRMWARE_LIBOS3_SUPPORTED},
        {"LOGROOT", "ROOT", ".fwlogging_start", NV_FIRMWARE_LIBOS3_SUPPORTED},
    };

    *num_infos = NV_ARRAY_ELEMENTS(infos);
    ct_assert(NV_ARRAY_ELEMENTS(infos) == NV_FIRMWARE_TASK_COUNT);

    return infos;
}

static inline
const nv_firmware_task_log_info_t* nv_firmware_log_info_for_task(nv_firmware_task_t task_id)
{
    unsigned num_infos = 0;
    const nv_firmware_task_log_info_t* infos = nv_firmware_get_task_log_infos(&num_infos);

    if ((unsigned)task_id >= num_infos)
    {
        return NULL;
    }

    return &infos[task_id];
}

typedef struct
{
    const char *elf_section_name;

    struct
    {
        NvU32 lo, hi;
    } gpuArch;

    struct
    {
        NvU32 lo, hi;
    } gpuImpl;
} nv_firmware_kernel_log_info_t;

#define NV_FIRMWARE_KERNEL_LOG_MEMORY_ID "LOGKRNL"
#define NV_FIRMWARE_KERNEL_LOG_PREFIX    "KRNL"
#define NV_FIRMWARE_GPU_ARCH_SHIFT 0x4

//
// HAL for selecting .fwlogging section. Chip ID is checked to fall within {gpuArch, gpuImpl}::{lo, hi}
// ranges. Optional gpuImpl range can be used to specify sub-range of an architecture.
// If `hi` is not set, the range is treated as open. Ranges are checked in order of specification.
// Range overlaps are allowed and resolved by the order.
//
static inline
const nv_firmware_kernel_log_info_t *nv_firmware_get_kernel_log_infos(unsigned *num_infos)
{
    const static nv_firmware_kernel_log_info_t infos[] =
    {
        {".fwlogging_kernel_gb20x",
         {GPU_ARCHITECTURE_BLACKWELL_GB2XX}, // gb20x...
        },
        {".fwlogging_kernel_gb10y",
         {GPU_ARCHITECTURE_BLACKWELL_GB1XX, GPU_ARCHITECTURE_BLACKWELL_GB1XX}, // gb10y
         {GPU_IMPLEMENTATION_GB10B, 0}
        },

        {".fwlogging_kernel_gb10x",
         {GPU_ARCHITECTURE_BLACKWELL_GB1XX}, // gb10x...
        },

        {".fwlogging_kernel_gh100",
         {GPU_ARCHITECTURE_HOPPER, GPU_ARCHITECTURE_HOPPER}, // gh100
        },

        {".fwlogging_kernel_ga10x",
         {GPU_ARCHITECTURE_AMPERE, GPU_ARCHITECTURE_ADA}, // ga10x...ad10x
        },
    };

    *num_infos = NV_ARRAY_ELEMENTS(infos);

    return infos;
}

static inline
const nv_firmware_kernel_log_info_t *nv_firmware_kernel_log_info_for_gpu(NvU32 gpuArch, NvU32 gpuImpl)
{
    unsigned num_infos = 0;
    const nv_firmware_kernel_log_info_t* infos = nv_firmware_get_kernel_log_infos(&num_infos);

    for (unsigned i = 0; i < num_infos; i++)
    {
        const nv_firmware_kernel_log_info_t* info = &infos[i];

        NvBool bMatchingArch = info->gpuArch.lo <= gpuArch;
        if (info->gpuArch.hi != 0)
            bMatchingArch &= gpuArch <= info->gpuArch.hi;

        NvBool bMatchingImpl = info->gpuImpl.lo <= gpuImpl;
        if (info->gpuImpl.hi != 0)
            bMatchingImpl &= gpuImpl <= info->gpuImpl.hi;

        if (bMatchingArch && bMatchingImpl)
        {
            return info;
        }
    }

    return NULL;
}

#endif  // NV_FIRMWARE_CHIP_FAMILY_SELECT_H
