/*
 * SPDX-FileCopyrightText: Copyright (c) 2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 
#ifndef GSP_FMC_ERROR_CODES_FORMATTING_H
#define GSP_FMC_ERROR_CODES_FORMATTING_H

//================================================================================
// DRF Field Definitions for NV_PBUS_SW_SCRATCH_GSP_FMC_ERROR
// These define the bit ranges for the register fields.
//================================================================================

// Error code layout
#define NV_PBUS_SW_SCRATCH_GSP_FMC_ERROR_VARIANT                       31:28
#define NV_PBUS_SW_SCRATCH_GSP_FMC_ERROR_PARTITION                     27:24
#define NV_PBUS_SW_SCRATCH_GSP_FMC_ERROR_PAYLOAD                       23:0

// Fields within the PAYLOAD for Variant 0 (SK)
#define NV_PBUS_SW_SCRATCH_GSP_FMC_ERROR_SK_ERROR                      15:8
#define NV_PBUS_SW_SCRATCH_GSP_FMC_ERROR_SK_PHASE                       7:0

// Fields within the PAYLOAD for Variant 1 (Generic)
#define NV_PBUS_SW_SCRATCH_GSP_FMC_ERROR_GENERIC_ADDITIONAL_INFO       23:16
#define NV_PBUS_SW_SCRATCH_GSP_FMC_ERROR_GENERIC_ERROR_CODE            15:0

// Error code layout for safety
#define NV_PBUS_SW_SCRATCH_GSP_FMC_ERROR_VARIANT_XMSB                  31
#define NV_PBUS_SW_SCRATCH_GSP_FMC_ERROR_VARIANT_XLSB                  28
#define NV_PBUS_SW_SCRATCH_GSP_FMC_ERROR_PARTITION_XMSB                27
#define NV_PBUS_SW_SCRATCH_GSP_FMC_ERROR_PARTITION_XLSB                24
#define NV_PBUS_SW_SCRATCH_GSP_FMC_ERROR_PAYLOAD_XMSB                  23
#define NV_PBUS_SW_SCRATCH_GSP_FMC_ERROR_PAYLOAD_XLSB                   0

// Fields within the PAYLOAD for Variant 0 (SK) for safety
#define NV_PBUS_SW_SCRATCH_GSP_FMC_ERROR_SK_ERROR_XMSB                 15
#define NV_PBUS_SW_SCRATCH_GSP_FMC_ERROR_SK_ERROR_XLSB                  8

#define NV_PBUS_SW_SCRATCH_GSP_FMC_ERROR_SK_PHASE_XMSB                  7
#define NV_PBUS_SW_SCRATCH_GSP_FMC_ERROR_SK_PHASE_XLSB                  0

// Fields within the PAYLOAD for Variant 1 (Generic) for safety
#define NV_PBUS_SW_SCRATCH_GSP_FMC_ERROR_GENERIC_ADDITIONAL_INFO_XMSB  23
#define NV_PBUS_SW_SCRATCH_GSP_FMC_ERROR_GENERIC_ADDITIONAL_INFO_XLSB  16

#define NV_PBUS_SW_SCRATCH_GSP_FMC_ERROR_GENERIC_ERROR_CODE_XMSB       15
#define NV_PBUS_SW_SCRATCH_GSP_FMC_ERROR_GENERIC_ERROR_CODE_XLSB        0


//================================================================================
// Constant Values
//================================================================================

// Partition ID Encoding Logic
#define GSP_FMC_ERROR_CODES_LATEST_IMPL_PART_ID 2U
#define GSP_FMC_PARTITION_ID_MASK               0xFU
#define GSP_FMC_PARTITION_ID_OFFSET             1U

// Error Variants / Versions
#define GSP_FMC_ERROR_VARIANT_SK                0U
#define GSP_FMC_ERROR_VARIANT_GENERIC           1U

#endif /* GSP_FMC_ERROR_CODES_FORMATTING_H */
