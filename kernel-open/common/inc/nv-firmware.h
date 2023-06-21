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

#ifndef NV_FIRMWARE_H
#define NV_FIRMWARE_H



#include <nvtypes.h>
#include <nvmisc.h>

typedef enum
{
    NV_FIRMWARE_TYPE_GSP,
    NV_FIRMWARE_TYPE_GSP_LOG
} nv_firmware_type_t;

typedef enum
{
    NV_FIRMWARE_CHIP_FAMILY_NULL = 0,
    NV_FIRMWARE_CHIP_FAMILY_TU10X = 1,
    NV_FIRMWARE_CHIP_FAMILY_TU11X = 2,
    NV_FIRMWARE_CHIP_FAMILY_GA100 = 3,
    NV_FIRMWARE_CHIP_FAMILY_GA10X = 4,
    NV_FIRMWARE_CHIP_FAMILY_AD10X = 5,
    NV_FIRMWARE_CHIP_FAMILY_GH100 = 6,
    NV_FIRMWARE_CHIP_FAMILY_END,
} nv_firmware_chip_family_t;

static inline const char *nv_firmware_chip_family_to_string(
    nv_firmware_chip_family_t fw_chip_family
)
{
    switch (fw_chip_family) {
        case NV_FIRMWARE_CHIP_FAMILY_GH100: return "gh100";
        case NV_FIRMWARE_CHIP_FAMILY_AD10X: return "ad10x";
        case NV_FIRMWARE_CHIP_FAMILY_GA10X: return "ga10x";
        case NV_FIRMWARE_CHIP_FAMILY_GA100: return "ga100";
        case NV_FIRMWARE_CHIP_FAMILY_TU11X: return "tu11x";
        case NV_FIRMWARE_CHIP_FAMILY_TU10X: return "tu10x";

        case NV_FIRMWARE_CHIP_FAMILY_END:  // fall through
        case NV_FIRMWARE_CHIP_FAMILY_NULL:
            return NULL;
    }
    return NULL;
}

// The includer (presumably nv.c) may optionally define
// NV_FIRMWARE_PATH_FOR_FILENAME(filename)
// to return a string "path" given a gsp_*.bin or gsp_log_*.bin filename.
//
// The function nv_firmware_path will then be available.
#if defined(NV_FIRMWARE_PATH_FOR_FILENAME)
static inline const char *nv_firmware_path(
    nv_firmware_type_t fw_type,
    nv_firmware_chip_family_t fw_chip_family
)
{
    if (fw_type == NV_FIRMWARE_TYPE_GSP)
    {
        switch (fw_chip_family)
        {
            case NV_FIRMWARE_CHIP_FAMILY_GH100:  // fall through
            case NV_FIRMWARE_CHIP_FAMILY_AD10X:  // fall through
            case NV_FIRMWARE_CHIP_FAMILY_GA10X:
                return NV_FIRMWARE_PATH_FOR_FILENAME("gsp_ga10x.bin");

            case NV_FIRMWARE_CHIP_FAMILY_GA100:  // fall through
            case NV_FIRMWARE_CHIP_FAMILY_TU11X:  // fall through
            case NV_FIRMWARE_CHIP_FAMILY_TU10X:
                return NV_FIRMWARE_PATH_FOR_FILENAME("gsp_tu10x.bin");

            case NV_FIRMWARE_CHIP_FAMILY_END:  // fall through
            case NV_FIRMWARE_CHIP_FAMILY_NULL:
                return "";
        }
    }
    else if (fw_type == NV_FIRMWARE_TYPE_GSP_LOG)
    {
        switch (fw_chip_family)
        {
            case NV_FIRMWARE_CHIP_FAMILY_GH100:  // fall through
            case NV_FIRMWARE_CHIP_FAMILY_AD10X:  // fall through
            case NV_FIRMWARE_CHIP_FAMILY_GA10X:
                return NV_FIRMWARE_PATH_FOR_FILENAME("gsp_log_ga10x.bin");

            case NV_FIRMWARE_CHIP_FAMILY_GA100:  // fall through
            case NV_FIRMWARE_CHIP_FAMILY_TU11X:  // fall through
            case NV_FIRMWARE_CHIP_FAMILY_TU10X:
                return NV_FIRMWARE_PATH_FOR_FILENAME("gsp_log_tu10x.bin");

            case NV_FIRMWARE_CHIP_FAMILY_END:  // fall through
            case NV_FIRMWARE_CHIP_FAMILY_NULL:
                return "";
        }
    }

    return "";
}
#endif  // defined(NV_FIRMWARE_PATH_FOR_FILENAME)

// The includer (presumably nv.c) may optionally define
// NV_FIRMWARE_DECLARE_GSP_FILENAME(filename)
// which will then be invoked (at the top-level) for each
// gsp_*.bin (but not gsp_log_*.bin)
#if defined(NV_FIRMWARE_DECLARE_GSP_FILENAME)
NV_FIRMWARE_DECLARE_GSP_FILENAME("gsp_ga10x.bin")
NV_FIRMWARE_DECLARE_GSP_FILENAME("gsp_tu10x.bin")
#endif  // defined(NV_FIRMWARE_DECLARE_GSP_FILENAME)

#endif  // NV_FIRMWARE_DECLARE_GSP_FILENAME
