/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef LIBSPDM_INCLUDES_H
#define LIBSPDM_INCLUDES_H

//
// Dedicated header file to centralize all libspdm-related includes and defines.
// This allows us to minimize the amount of headers (and sources) that have direct
// libspdm dependency, and allowing shared macros & types for dependent source.
//

/* ------------------------ Includes --------------------------------------- */
#include "internal/libspdm_common_lib.h"
#include "internal/libspdm_secured_message_lib.h"
#include "library/spdm_requester_lib.h"
#include "nvspdm_cryptlib_extensions.h"

/* ------------------------ Macros and Defines ----------------------------- */
//
// As libspdm has its own RETURN_STATUS define, we need to ensure we do not
// accidentally compare it against NV_STATUS. Use macro for consistent libspdm
// error handling.
//
#define CHECK_SPDM_STATUS(expr) do {                                    \
         libspdm_return_t __spdmStatus;                                 \
         __spdmStatus = (expr);                                         \
         if (LIBSPDM_STATUS_IS_ERROR(__spdmStatus))                     \
         {                                                              \
             NV_PRINTF(LEVEL_ERROR, "SPDM failed with status 0x%0x\n",  \
                       __spdmStatus);                                   \
             status = NV_ERR_GENERIC;                                   \
             goto ErrorExit;                                            \
         }                                                              \
     } while (NV_FALSE)

// Check for any critical issues caused by data size mismatches.
ct_assert(sizeof(NvU8)  == sizeof(uint8_t));
ct_assert(sizeof(NvU16) == sizeof(uint16_t));
ct_assert(sizeof(NvU32) == sizeof(uint32_t));
ct_assert(sizeof(NvU64) == sizeof(uint64_t));

typedef struct _SPDM_ALGO_CHECK_ENTRY
{
    libspdm_data_type_t dataType;
    uint32_t            expectedAlgo;
} SPDM_ALGO_CHECK_ENTRY, *PSPDM_ALGO_CHECK_ENTRY;

//
// Check for assert in libspdm code, indicating a fatal condition.
// Returns false if assert was hit.
//
bool nvspdm_check_and_clear_libspdm_assert(void);

#endif // LIBSPDM_INCLUDES_H
