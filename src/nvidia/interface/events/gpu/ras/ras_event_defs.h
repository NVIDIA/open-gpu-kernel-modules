/*
 * SPDX-FileCopyrightText: Copyright (c) 2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _RAS_EVENT_DEFS_H_
#define _RAS_EVENT_DEFS_H_

#include "nvtypes.h"

typedef NvU16 RAS_OP_EVENT_CODE;
enum
{
    RAS_OP_EVENT_CODE_ROW_REMAPPING_PENDING                         = 0x0001U,
    
    // Inforom object is full
    RAS_OP_EVENT_CODE_ROW_REMAPPING_FAILURE_TABLE_FULL              = 0x0002U,
    
    // No more spare rows in the bank
    RAS_OP_EVENT_CODE_ROW_REMAPPING_FAILURE_BANK_FULL               = 0x0003U,
    
    RAS_OP_EVENT_CODE_ROW_REMAPPING_FAILURE_INTERNAL_ERROR          = 0x0004U,
    RAS_OP_EVENT_CODE_ROW_REMAPPING_FAILURE_RESERVED_ROW            = 0x0005U,
    RAS_OP_EVENT_CODE_ROW_REMAPPING_FAILURE_PAGE_OFFLINE_FAILURE    = 0x0006U,
    RAS_OP_EVENT_CODE_DRAM_RETIREMENT_FAILURE_INTERNAL_ERROR        = 0x0007U,
    RAS_OP_EVENT_CODE_DRAM_RETIREMENT_FAILURE_INFOROM_FULL          = 0x0008U,
    RAS_OP_EVENT_CODE_DRAM_RETIREMENT_FAILURE_HW_LIMIT              = 0x0009U,
    RAS_OP_EVENT_CODE_DRAM_RETIREMENT_FAILURE_NO_SPARE              = 0x000AU,
    RAS_OP_EVENT_CODE_LTS_REPAIR_PENDING                            = 0x000BU,
    RAS_OP_EVENT_CODE_LTS_REPAIR_FAILURE                            = 0x000CU,
    RAS_OP_EVENT_CODE_MEMORY_CHANNEL_REPAIR_PENDING                 = 0x000DU,
    RAS_OP_EVENT_CODE_MEMORY_CHANNEL_REPAIR_FAILURE                 = 0x000EU,
    RAS_OP_EVENT_CODE_SPARING_REPAIR_PENDING                        = 0x000FU,

    // 0x0010U deprecated
    RAS_OP_EVENT_CODE_SPARING_REPAIR_NOT_POSSIBLE                   = 0x0011U,

    // 0x0012U deprecated
    
    // Residual ECC errors affecting init/runtime
    RAS_OP_EVENT_CODE_ECC_RESIDUAL_UNCORRECTABLE_ERROR              = 0x0013U,
    RAS_OP_EVENT_CODE_DRAM_ECC_INTR_STORM                           = 0x0014U,
    RAS_OP_EVENT_CODE_SM_ECC_INTR_STORM                             = 0x0015U,
    RAS_OP_EVENT_CODE_BANK_REMAPPING_PENDING                        = 0x0016U,
    
    // Fatal LTC errors
    RAS_OP_EVENT_CODE_LTC_FATAL_ERROR                               = 0x0017U,

    // Fatal LRC errors
    RAS_OP_EVENT_CODE_LRC_FATAL_ERROR                               = 0x0018U,

    // DRAM errors
    RAS_OP_EVENT_CODE_DRAM_ERROR                                    = 0x0019U,
    RAS_OP_EVENT_CODE_DRAM_SIDEBAND_ERROR                           = 0x001AU,
    RAS_OP_EVENT_CODE_DRAM_CBC_FATAL_ERROR                          = 0x001BU,
    RAS_OP_EVENT_CODE_DRAM_CBC_NON_FATAL_ERROR                      = 0x001CU,

    // Recoverable LTC errors
    RAS_OP_EVENT_CODE_LTC_ERROR                                     = 0x001DU,
    RAS_OP_EVENT_CODE_LTC_UNSUPPORTED_CLIENT_POISON_UNCONTAINED_ERROR = 0x001EU,
    RAS_OP_EVENT_CODE_LTC_UNSUPPORTED_CLIENT_POISON_CONTAINED_ERROR   = 0x001FU,

    // Poison errors
    RAS_OP_EVENT_CODE_PCIE_INGRESS_POISON_CONTAINED_ERROR           = 0x0020U,
    RAS_OP_EVENT_CODE_PCIE_INGRESS_POISON_UNCONTAINED_ERROR         = 0x0021U,
    RAS_OP_EVENT_CODE_PCIE_EGRESS_POISON_ERROR                      = 0x0022U,
    RAS_OP_EVENT_CODE_FBHUB_FATAL_POISON_ERROR                      = 0x0023U,
    RAS_OP_EVENT_CODE_FBHUB_NON_FATAL_POISON_ERROR                  = 0x0024U,
    RAS_OP_EVENT_CODE_MMU_POISON_ERROR                              = 0x0025U,
    RAS_OP_EVENT_CODE_GSP_POISON_ERROR                              = 0x0026U,
    RAS_OP_EVENT_CODE_COPY_ENGINE_POISON_CONTAINED_ERROR            = 0x0027U,
    RAS_OP_EVENT_CODE_COPY_ENGINE_POISON_UNCONTAINED_ERROR          = 0x0028U,
    RAS_OP_EVENT_CODE_PMU_POISON_ERROR                              = 0x0029U,
    RAS_OP_EVENT_CODE_SEC2_POISON_ERROR                             = 0x002AU,
    RAS_OP_EVENT_CODE_FBFALCON_POISON_ERROR                         = 0x002BU,
    RAS_OP_EVENT_CODE_VIDEO_ENGINE_POISON_ERROR                     = 0x002CU,
    RAS_OP_EVENT_CODE_SM_POISON_ERROR                               = 0x002DU,
    RAS_OP_EVENT_CODE_CONTEXT_SWITCH_POISON_ERROR                   = 0x002EU,
    RAS_OP_EVENT_CODE_GCC_POISON_ERROR                              = 0x002FU,

    // SRAM ECC errors
    RAS_OP_EVENT_CODE_PCIE_UNCORRECTABLE_ERROR                      = 0x0030U,
    RAS_OP_EVENT_CODE_COPY_ENGINE_UNCORRECTABLE_ERROR               = 0x0031U,
    RAS_OP_EVENT_CODE_MMU_UNCORRECTABLE_ERROR                       = 0x0032U,
    RAS_OP_EVENT_CODE_FECS_UNCORRECTABLE_ERROR                      = 0x0033U,
    RAS_OP_EVENT_CODE_SM_CONTAINED_UNCORRECTABLE_ERROR              = 0x0034U,
    RAS_OP_EVENT_CODE_GCC_UNCORRECTABLE_ERROR                       = 0x0035U,
    RAS_OP_EVENT_CODE_CONTEXT_SWITCH_UNCORRECTABLE_ERROR            = 0x0036U,
    RAS_OP_EVENT_CODE_GPCMMU_UNCORRECTABLE_ERROR                    = 0x0037U,
    RAS_OP_EVENT_CODE_GSP_UNCORRECTABLE_ERROR                       = 0x0038U,
    RAS_OP_EVENT_CODE_LPSEC_UNCORRECTABLE_ERROR                     = 0x0039U,
    RAS_OP_EVENT_CODE_NVLINK_UNCORRECTABLE_ERROR                    = 0x003AU,
    RAS_OP_EVENT_CODE_PMU_UNCORRECTABLE_ERROR                       = 0x003BU,
    RAS_OP_EVENT_CODE_SEC2_UNCORRECTABLE_ERROR                      = 0x003CU,
    RAS_OP_EVENT_CODE_SPARING_EVENT                                 = 0x003DU,
};

#endif // _RAS_EVENT_DEFS_H_
