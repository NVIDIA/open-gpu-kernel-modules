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

#ifndef _GPU_MODS_ERROR_H_
#define _GPU_MODS_ERROR_H_

/**
 * @file gpu_mods_error.h
 * @brief Kernel GPU MODS error reporting header
 *
 * This header provides MODS error reporting macros for kernel GPU modules.
 * When built with MODS support, these macros report structured errors.
 * When built without MODS (e.g., OpenRM), these macros are no-ops.
 */

// Define dummy error codes and no-op macros when MODS is not enabled

// Dummy BUS error codes
#define MODSDRV_BUS_ERROR_CODE_SEC_FAULT_REGISTER                0
#define MODSDRV_BUS_ERROR_CODE_VSEC_DEBUG_SEC_REGISTER           0

// Dummy FALCON error codes
#define MODSDRV_FALCON_ERROR_CODE_ECC_IMEM                       0
#define MODSDRV_FALCON_ERROR_CODE_ECC_DMEM                       0
#define MODSDRV_FALCON_ERROR_CODE_ECC_ICACHE                     0
#define MODSDRV_FALCON_ERROR_CODE_ECC_DCACHE                     0
#define MODSDRV_FALCON_ERROR_CODE_ECC_MPU_RAM                    0
#define MODSDRV_FALCON_ERROR_CODE_ECC_DCLS                       0
#define MODSDRV_FALCON_ERROR_CODE_ECC_REG                        0
#define MODSDRV_FALCON_ERROR_CODE_ECC_EMEM                       0
#define MODSDRV_FALCON_ERROR_CODE_FATAL_ERROR                    0
#define MODSDRV_FALCON_ERROR_CODE_IRQSTAT_FATAL                  0

/**
 * @brief Report a BUS error (no-op in non-MODS builds)
 */
#define MODS_REPORT_BUS_ERROR(pGpu_, sev_, ecode_, info_, reg_) \
    do { (void)(pGpu_); (void)(sev_); (void)(ecode_); \
         (void)(info_); (void)(reg_); } while(0)

/**
 * @brief Report a GSP error (no-op in non-MODS builds)
 */
#define MODS_REPORT_GSP_ERROR(pGpu_, sev_, ecode_, errStat_) \
    do { (void)(pGpu_); (void)(sev_); (void)(ecode_); \
         (void)(errStat_); } while(0)

/**
 * @brief Report a FALCON error (no-op in non-MODS builds)
 */
#define MODS_REPORT_FALCON_ERROR(pGpu_, sev_, ecode_, engInst_, errStat_) \
    do { (void)(pGpu_); (void)(sev_); (void)(ecode_); \
         (void)(engInst_); (void)(errStat_); } while(0)

#endif // _GPU_MODS_ERROR_H_
