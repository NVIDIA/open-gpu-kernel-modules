/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/**
 * @file
 * @brief This module defines several guids.
 */

#include "nvcd.h"

#define DECLSPEC_SELECTANY

// Special GUID definition to get around pre-compiled header definition
// We need to define the GUID value here and not just generate a reference
#define GUID_CONST(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
        const GUID DECLSPEC_SELECTANY name \
                = { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }


// _DSM NVHG GUID {9D95A0A0-0060-4d48-B34D-7E5FEA129FD4}
GUID_CONST (NVHG_DSM_GUID,
        0x9D95A0A0L, 0x0060, 0x4d48, 0xB3, 0x4D, 0x7E, 0x5F, 0xEA, 0x12, 0x9F, 0xD4);
// Buffer(0x10){0xA0, 0xA0, 0x95, 0x9D, 0x60, 0x00, 0x48, 0x4d, 0xB3, 0x4D, 0x7E, 0x5F, 0xEA, 0x12, 0x9F, 0xD4},

// _DSM NBSI NVHG GUID {998669A6-8BE9-49FB-BDDB-51A1EFE19C3D}
GUID_CONST (NBSI_DSM_GUID,
        0x998669A6L, 0x8BE9, 0x49FB, 0xBD, 0xDB, 0x51, 0xA1, 0xEF, 0xE1, 0x9C, 0x3D);
// Buffer(0x10){0xA6, 0x69, 0x86, 0x99, 0xE9, 0x8B, 0xFB, 0x49, 0xBD, 0xDB, 0x51, 0xA1, 0xEF, 0xE1, 0x9C, 0x3D},

// _DSM SPB GUID {95DB88FD-940A-4253-A446-70CE0504AEDF}
GUID_CONST (SPB_DSM_GUID,
        0x95DB88FDL, 0x940A, 0x4253, 0xA4, 0x46, 0x70, 0xCE, 0x05, 0x04, 0xAE, 0xDF);
// Buffer(0x10){0xFD, 0x88, 0xDB, 0x95, 0x0A, 0x94, 0x53, 0x42, 0xA4, 0x46, 0x70, 0xCE, 0x05, 0x04, 0xAE, 0xDF},

// _DSM NBCI GUID {D4A50B75-65C7-46F7-BFB7-41514CEA0244}
GUID_CONST (NBCI_DSM_GUID,
        0xD4A50B75L, 0x65C7, 0x46F7, 0xBF, 0xB7, 0x41, 0x51, 0x4C, 0xEA, 0x02, 0x44);
// Buffer(0x10){0x75, 0x0B, 0xA5, 0xD4, 0xC7, 0x65, 0xF7, 0x46, 0xBF, 0xB7, 0x41, 0x51, 0x4C, 0xEA, 0x02, 0x44},

// _DSM NVOP GUID {A486D8F8-0BDA-471B-A72B-6042A6B5BEE0}
GUID_CONST (NVOP_DSM_GUID,
        0xA486D8F8L, 0x0BDA, 0x471B, 0xA7, 0x2B, 0x60, 0x42, 0xA6, 0xB5, 0xBE, 0xE0);
// Buffer(0x10){0xF8, 0xD8, 0x86, 0xA4, 0xDA, 0x0B, 0x1B, 0x47, 0xA7, 0x2B, 0x60, 0x42, 0xA6, 0xB5, 0xBE, 0xE0},

// _DSM PCFG GUID {81C6147D-735F-42D9-9E41-B002CBC6571D}
GUID_CONST (PCFG_DSM_GUID,
        0x81C6147DL, 0x735F, 0x42D9, 0x9E, 0x41, 0xB0, 0x02, 0xCB, 0xC6, 0x57, 0x1D);
// Buffer(0x10){0x7D, 0x14, 0C6, 0x81, 0x5F, 0x73, 0xD9, 0x42, 0x9E, 0x41, 0xB0, 0x02, 0xCB, 0xC6, 0x57, 0x1D},

// _DSM GPS GUID {A3132D01-8CDA-49BA-A52E-BC9D46DF6B81}
GUID_CONST (GPS_DSM_GUID,
        0xA3132D01L, 0x8CDA, 0x49BA, 0xA5, 0x2E, 0xBC, 0x9D, 0x46, 0xDF, 0x6B, 0x81);

// _DSM_ JT GUID {CBECA351-067B-4924-9CBD-B46B00B86F34}
GUID_CONST (JT_DSM_GUID,
        0xCBECA351L, 0x067B, 0x4924, 0x9C, 0xBD, 0xB4, 0x6B, 0x00, 0xB8, 0x6F, 0x34);

// PCI Express DSM GUID

// _DSM_ PEX GUID {E5C937D0-3553-4D7A-9117-EA4D19C3434D}
GUID_CONST (PEX_DSM_GUID,
        0xE5C937D0L, 0x3553, 0x4D7A, 0x91, 0x17, 0xEA, 0x4D, 0x19, 0xC3, 0x43, 0x4D);

// NVPCF GUID
GUID_CONST (NVPCF_ACPI_DSM_GUID,
        0x36B49710L, 0x2483, 0x11E7, 0x95, 0x98, 0x08, 0x00, 0x20, 0x0C, 0x9A, 0x66);

// UEFI variables
GUID_CONST (NV_GUID_UEFI_VARIABLE,
        0x7a09bc55L, 0x83e8, 0x4d4d, 0x93, 0xb0, 0x89, 0xd8, 0x0e, 0xed, 0xb7, 0x20);

// _DSM MXM GUID {4004A400-917D-4cf2-B89C-79B62FD55665}
GUID_CONST (DSM_MXM_GUID,
        0x4004A400L, 0x917D, 0x4cf2, 0xB8, 0x9C, 0x79, 0xB6, 0x2F, 0xD5, 0x56, 0x65);
// Buffer(0x10){0x00, 0xA4, 0x04, 0x40, 0x7D, 0x91, 0xF2, 0x4C, 0xB8, 0x9C, 0x79, 0xB6, 0x2F, 0xD5, 0x56, 0x65},

