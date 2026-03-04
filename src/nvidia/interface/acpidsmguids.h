/*
 * SPDX-FileCopyrightText: Copyright (c) 2000-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef ACPIDSMGUIDS_H
#define ACPIDSMGUIDS_H

#include "nvcd.h"

//
// These guids are used in linux.
// The same guid values are defined in a windows way in guids.c
// These revision_ids are also defined in platform/nbsi/nbsi_read.h, nvmxm.h, nvhybridacpi.h, nbci.h.
// Those files should be changed and these used.
//
#define NBSI_DSM_GUID_STR { 0xA6, 0x69, 0x86, 0x99, 0xE9, 0x8B, 0xFB, 0x49, \
                            0xBD, 0xDB, 0x51, 0xA1, 0xEF, 0xE1, 0x9C, 0x3D }
#define NBSI_REVISION_ID      0x00000101

#define MXM_DSM_GUID_STR  { 0x00, 0xA4, 0x04, 0x40, 0x7D, 0x91, 0xF2, 0x4C, \
                           0xB8, 0x9C, 0x79, 0xB6, 0x2F, 0xD5, 0x56, 0x65}
#define ACPI_MXM_REVISION_ID  0x00000300

#define NVHG_DSM_GUID_STR { 0xA0, 0xA0, 0x95, 0x9D, 0x60, 0x00, 0x48, 0x4d, \
                            0xB3, 0x4D, 0x7E, 0x5F, 0xEA, 0x12, 0x9F, 0xD4}
#define NVHG_REVISION_ID      0x00000102

#define NBCI_DSM_GUID_STR { 0x75, 0x0B, 0xA5, 0xD4, 0xC7, 0x65, 0xF7, 0x46, \
                            0xBF, 0xB7, 0x41, 0x51, 0x4C, 0xEA, 0x02, 0x44}
#define NBCI_REVISION_ID      0x00000102

#define NVOP_DSM_GUID_STR { 0xF8, 0xD8, 0x86, 0xA4, 0xDA, 0x0B, 0x1B, 0x47, \
                            0xA7, 0x2B, 0x60, 0x42, 0xA6, 0xB5, 0xBE, 0xE0}
#define NVOP_REVISION_ID      0x00000100

#define PCFG_DSM_GUID_STR  { 0x7D, 0x14, 0xC6, 0x81, 0x5F, 0x73, 0xD9, 0x42, \
                            0x9E, 0x41, 0xB0, 0x02, 0xCB, 0xC6, 0x57, 0x1D}
#define PCFG_REVISION_ID       0x00000100

#define GPS_2X_REVISION_ID      0x00000200

#define GPS_DSM_GUID_STR   { 0x01, 0x2D, 0x13, 0xA3, 0xDA, 0x8C, 0xBA, 0x49, \
                             0xA5, 0x2E, 0xBC, 0x9D, 0x46, 0xDF, 0x6B, 0x81},
#define GPS_REVISION_ID       0x00000100

#define JT_DSM_GUID_STR     { 0x51, 0xA3, 0xEC, 0xCB, 0x7B, 0x06, 0x24, 0x49, \
                              0x9C, 0xBD, 0xB4, 0x6B, 0x00, 0xB8, 0x6F, 0x34}
#define JT_REVISION_ID        0x00000103

// PEX_DSM_GUID {E5C937D0-3553-4D7A-9117-EA4D19C3434D}
#define PEX_DSM_GUID_STR     { 0xD0, 0x37, 0xC9, 0xE5, 0x53, 0x35, 0x7A, 0x4D, \
                               0x91, 0x17, 0xEA, 0x4D, 0x19, 0xC3, 0x43, 0x4D}
#define PEX_REVISION_ID        0x00000002

#define NVPCF_ACPI_DSM_REVISION_ID    0x00000100

#define NVPCF_2X_ACPI_DSM_REVISION_ID    0x00000200

extern const GUID NBCI_DSM_GUID;
extern const GUID NBSI_DSM_GUID;
extern const GUID NVHG_DSM_GUID;
extern const GUID NVOP_DSM_GUID;
extern const GUID SPB_DSM_GUID;
extern const GUID DSM_MXM_GUID;
extern const GUID PCFG_DSM_GUID;
extern const GUID GPS_DSM_GUID;
extern const GUID PEX_DSM_GUID;
extern const GUID JT_DSM_GUID;
extern const GUID NVPCF_ACPI_DSM_GUID;

#endif // ACPIDSMGUIDS_H
