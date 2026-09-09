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
    RAS_OP_EVENT_CODE_TPC_REPAIR_PENDING_SAME_GPC                   = 0x000FU,
    RAS_OP_EVENT_CODE_TPC_REPAIR_PENDING_DIFFERENT_GPC              = 0x0010U,
    RAS_OP_EVENT_CODE_TPC_REPAIR_FAILURE_NO_SPARE                   = 0x0011U,
    RAS_OP_EVENT_CODE_TPC_REPAIR_FAILURE_NO_SPARE_MIG               = 0x0012U,
    
    // Residual ECC errors affecting init/runtime
    RAS_OP_EVENT_CODE_ECC_RESIDUAL_UNCORRECTABLE_ERROR              = 0x0013U,
    RAS_OP_EVENT_CODE_DRAM_ECC_INTR_STORM                           = 0x0014U,
    RAS_OP_EVENT_CODE_SM_ECC_INTR_STORM                             = 0x0015U,
    RAS_OP_EVENT_CODE_BANK_REMAPPING_PENDING                        = 0x0016U,
};

#endif // _RAS_EVENT_DEFS_H_
