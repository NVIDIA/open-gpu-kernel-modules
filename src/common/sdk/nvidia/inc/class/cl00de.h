/*
 * SPDX-FileCopyrightText: Copyright (c) 2022-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _cl00de_h_
#define _cl00de_h_

#ifdef __cplusplus
extern "C" {
#endif

#include "nvmisc.h"
#include "nvfixedtypes.h"

#define RM_USER_SHARED_DATA                      (0x000000de)

#define RUSD_TIMESTAMP_WRITE_IN_PROGRESS (NV_U64_MAX)
#define RUSD_TIMESTAMP_INVALID           0

enum {
    RUSD_CLK_PUBLIC_DOMAIN_GRAPHICS = 0,
    RUSD_CLK_PUBLIC_DOMAIN_MEMORY,
    RUSD_CLK_PUBLIC_DOMAIN_VIDEO,

    // Put at the end. See bug 1000230 NVML doesn't report SM frequency on Kepler
    RUSD_CLK_PUBLIC_DOMAIN_SM,
    RUSD_CLK_PUBLIC_DOMAIN_MAX_TYPE,
};

enum {
    RUSD_CLK_THROTTLE_REASON_GPU_IDLE                         = NVBIT(0),
    RUSD_CLK_THROTTLE_REASON_APPLICATION_CLOCK_SETTING        = NVBIT(1), 
    RUSD_CLK_THROTTLE_REASON_SW_POWER_CAP                     = NVBIT(2), 
    RUSD_CLK_THROTTLE_REASON_HW_SLOWDOWN                      = NVBIT(3), 
    RUSD_CLK_THROTTLE_REASON_SYNC_BOOST                       = NVBIT(4), 
    RUSD_CLK_THROTTLE_REASON_SW_THERMAL_SLOWDOWN              = NVBIT(5), 
    RUSD_CLK_THROTTLE_REASON_HW_THERMAL_SLOWDOWN              = NVBIT(6), 
    RUSD_CLK_THROTTLE_REASON_HW_POWER_BRAKES_SLOWDOWN         = NVBIT(7), 
    RUSD_CLK_THROTTLE_REASON_DISPLAY_CLOCK_SETTING            = NVBIT(8), 
};

typedef struct RUSD_CLK_PUBLIC_DOMAIN_INFO {
    NvU32 targetClkMHz;
} RUSD_CLK_PUBLIC_DOMAIN_INFO;

typedef struct RUSD_CLK_PUBLIC_DOMAIN_INFOS {
    volatile NvU64 lastModifiedTimestamp;
    RUSD_CLK_PUBLIC_DOMAIN_INFO info[RUSD_CLK_PUBLIC_DOMAIN_MAX_TYPE];
} RUSD_CLK_PUBLIC_DOMAIN_INFOS;

typedef struct RUSD_PERF_DEVICE_UTILIZATION_INFO {
    NvU8 gpuPercentBusy;
    NvU8 memoryPercentBusy;
} RUSD_PERF_DEVICE_UTILIZATION_INFO;

typedef struct RUSD_PERF_DEVICE_UTILIZATION {
    volatile NvU64 lastModifiedTimestamp;
    RUSD_PERF_DEVICE_UTILIZATION_INFO info;
} RUSD_PERF_DEVICE_UTILIZATION;

typedef struct RUSD_PERF_CURRENT_PSTATE {
    volatile NvU64 lastModifiedTimestamp;
    NvU32 currentPstate;
} RUSD_PERF_CURRENT_PSTATE;

typedef struct RUSD_CLK_THROTTLE_REASON {
    volatile NvU64 lastModifiedTimestamp;
    NvU32 reasonMask;
} RUSD_CLK_THROTTLE_REASON;

typedef struct RUSD_MEM_ERROR_COUNTS {
    NvU64 correctedVolatile;
    NvU64 correctedAggregate;
    NvU64 uncorrectedVolatile;
    NvU64 uncorrectedAggregate;
} RUSD_MEM_ERROR_COUNTS;

#define RUSD_MEMORY_ERROR_TYPE_TOTAL 0
#define RUSD_MEMORY_ERROR_TYPE_DRAM  1
#define RUSD_MEMORY_ERROR_TYPE_SRAM  2
#define RUSD_MEMORY_ERROR_TYPE_COUNT 3

typedef struct RUSD_MEM_ECC {
    volatile NvU64 lastModifiedTimestamp;
    RUSD_MEM_ERROR_COUNTS count[RUSD_MEMORY_ERROR_TYPE_COUNT];
} RUSD_MEM_ECC;

typedef struct RUSD_POWER_LIMIT_INFO {
    NvU32 requestedmW;
    NvU32 enforcedmW;
} RUSD_POWER_LIMIT_INFO;

typedef struct RUSD_ENFORCED_POWER_LIMITS {
    volatile NvU64 lastModifiedTimestamp;
    RUSD_POWER_LIMIT_INFO info;
} RUSD_POWER_LIMITS;

typedef struct RUSD_TEMPERATURE_INFO{
    NvTemp gpuTemperature;
    NvTemp hbmTemperature;
} RUSD_TEMPERATURE_INFO;

typedef struct RUSD_TEMPERATURE {
    volatile NvU64 lastModifiedTimestamp;
    RUSD_TEMPERATURE_INFO info;
} RUSD_TEMPERATURE;

typedef struct RUSD_MEM_ROW_REMAP_INFO {
    NvU32 histogramMax;     // No remapped row is used.
    NvU32 histogramHigh;    // One remapped row is used.
    NvU32 histogramPartial; // More than one remapped rows are used.
    NvU32 histogramLow;     // One remapped row is available.
    NvU32 histogramNone;    // All remapped rows are used.

    NvU32 correctableRows;
    NvU32 uncorrectableRows;
    NvBool isPending;
    NvBool hasFailureOccurred;
} RUSD_MEM_ROW_REMAP_INFO;

typedef struct RUSD_MEM_ROW_REMAP {
    volatile NvU64 lastModifiedTimestamp;
    RUSD_MEM_ROW_REMAP_INFO info;
} RUSD_MEM_ROW_REMAP;

typedef struct RUSD_AVG_POWER_INFO {
    NvU32 averageGpuPower;      // mW
    NvU32 averageModulePower;   // mW
    NvU32 averageMemoryPower;   // mW
} RUSD_AVG_POWER_INFO;

typedef struct RUSD_AVG_POWER_USAGE {
    volatile NvU64 lastModifiedTimestamp;
    RUSD_AVG_POWER_INFO info;
} RUSD_AVG_POWER_USAGE;

typedef struct RUSD_INST_POWER_INFO {
    NvU32 instGpuPower;         // mW
    NvU32 instModulePower;      // mW
} RUSD_INST_POWER_INFO;

typedef struct RUSD_INST_POWER_USAGE {
    volatile NvU64 lastModifiedTimestamp;
    RUSD_INST_POWER_INFO info;
} RUSD_INST_POWER_USAGE;

typedef struct NV00DE_SHARED_DATA {
    volatile NvU32 seq;

    NvU32 bar1Size;
    NvU32 bar1AvailSize;

    // GSP polling data section
    NV_DECLARE_ALIGNED(RUSD_CLK_PUBLIC_DOMAIN_INFOS clkPublicDomainInfos, 8);
    NV_DECLARE_ALIGNED(RUSD_CLK_THROTTLE_REASON clkThrottleReason, 8);
    NV_DECLARE_ALIGNED(RUSD_PERF_DEVICE_UTILIZATION perfDevUtil, 8);
    NV_DECLARE_ALIGNED(RUSD_MEM_ECC memEcc, 8);
    NV_DECLARE_ALIGNED(RUSD_PERF_CURRENT_PSTATE perfCurrentPstate, 8);
    NV_DECLARE_ALIGNED(RUSD_POWER_LIMITS powerLimitGpu, 8); // Module Limit is not supported on Ampere/Hopper
    NV_DECLARE_ALIGNED(RUSD_TEMPERATURE temperature, 8);
    NV_DECLARE_ALIGNED(RUSD_MEM_ROW_REMAP memRowRemap, 8);
    NV_DECLARE_ALIGNED(RUSD_AVG_POWER_USAGE avgPowerUsage, 8);
    NV_DECLARE_ALIGNED(RUSD_INST_POWER_USAGE instPowerUsage, 8);
} NV00DE_SHARED_DATA;

#define NV00DE_RUSD_POLL_CLOCK     0x1
#define NV00DE_RUSD_POLL_PERF      0x2
#define NV00DE_RUSD_POLL_MEMORY    0x4
#define NV00DE_RUSD_POLL_POWER     0x8
#define NV00DE_RUSD_POLL_THERMAL   0x10

typedef struct NV00DE_ALLOC_PARAMETERS {
    NvU64 polledDataMask; // Bitmask of data to request polling at alloc time, 0 if not needed
} NV00DE_ALLOC_PARAMETERS;

#ifdef __cplusplus
};     /* extern "C" */
#endif

#endif /* _cl00de_h_ */
