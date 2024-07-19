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

#ifndef _GPU_ENGINE_TYPE_H_
#define _GPU_ENGINE_TYPE_H_

#include "class/cl2080.h"
#include "nvrangetypes.h"
#include "utils/nvbitvector.h"
#include "rmconfig.h"

typedef enum
{
    RM_ENGINE_TYPE_NULL                            =       (0x00000000),
    RM_ENGINE_TYPE_GR0                             =       (0x00000001),
    RM_ENGINE_TYPE_GR1                             =       (0x00000002),
    RM_ENGINE_TYPE_GR2                             =       (0x00000003),
    RM_ENGINE_TYPE_GR3                             =       (0x00000004),
    RM_ENGINE_TYPE_GR4                             =       (0x00000005),
    RM_ENGINE_TYPE_GR5                             =       (0x00000006),
    RM_ENGINE_TYPE_GR6                             =       (0x00000007),
    RM_ENGINE_TYPE_GR7                             =       (0x00000008),
    RM_ENGINE_TYPE_COPY0                           =       (0x00000009),
    RM_ENGINE_TYPE_COPY1                           =       (0x0000000a),
    RM_ENGINE_TYPE_COPY2                           =       (0x0000000b),
    RM_ENGINE_TYPE_COPY3                           =       (0x0000000c),
    RM_ENGINE_TYPE_COPY4                           =       (0x0000000d),
    RM_ENGINE_TYPE_COPY5                           =       (0x0000000e),
    RM_ENGINE_TYPE_COPY6                           =       (0x0000000f),
    RM_ENGINE_TYPE_COPY7                           =       (0x00000010),
    RM_ENGINE_TYPE_COPY8                           =       (0x00000011),
    RM_ENGINE_TYPE_COPY9                           =       (0x00000012),
    // removal tracking bug: 3748354
    RM_ENGINE_TYPE_COPY10                          =       (0x00000013),
    RM_ENGINE_TYPE_COPY11                          =       (0x00000014),
    RM_ENGINE_TYPE_COPY12                          =       (0x00000015),
    RM_ENGINE_TYPE_COPY13                          =       (0x00000016),
    RM_ENGINE_TYPE_COPY14                          =       (0x00000017),
    RM_ENGINE_TYPE_COPY15                          =       (0x00000018),
    RM_ENGINE_TYPE_COPY16                          =       (0x00000019),
    RM_ENGINE_TYPE_COPY17                          =       (0x0000001a),
    RM_ENGINE_TYPE_COPY18                          =       (0x0000001b),
    RM_ENGINE_TYPE_COPY19                          =       (0x0000001c),
    RM_ENGINE_TYPE_NVDEC0                          =       (0x0000001d),
    RM_ENGINE_TYPE_NVDEC1                          =       (0x0000001e),
    RM_ENGINE_TYPE_NVDEC2                          =       (0x0000001f),
    RM_ENGINE_TYPE_NVDEC3                          =       (0x00000020),
    RM_ENGINE_TYPE_NVDEC4                          =       (0x00000021),
    RM_ENGINE_TYPE_NVDEC5                          =       (0x00000022),
    RM_ENGINE_TYPE_NVDEC6                          =       (0x00000023),
    RM_ENGINE_TYPE_NVDEC7                          =       (0x00000024),
    RM_ENGINE_TYPE_NVENC0                          =       (0x00000025),
    RM_ENGINE_TYPE_NVENC1                          =       (0x00000026),
    RM_ENGINE_TYPE_NVENC2                          =       (0x00000027),
    RM_ENGINE_TYPE_RESERVED28                      =       (0x00000028),
    RM_ENGINE_TYPE_VP                              =       (0x00000029),
    RM_ENGINE_TYPE_ME                              =       (0x0000002a),
    RM_ENGINE_TYPE_PPP                             =       (0x0000002b),
    RM_ENGINE_TYPE_MPEG                            =       (0x0000002c),
    RM_ENGINE_TYPE_SW                              =       (0x0000002d),
    RM_ENGINE_TYPE_TSEC                            =       (0x0000002e),
    RM_ENGINE_TYPE_VIC                             =       (0x0000002f),
    RM_ENGINE_TYPE_MP                              =       (0x00000030),
    RM_ENGINE_TYPE_SEC2                            =       (0x00000031),
    RM_ENGINE_TYPE_HOST                            =       (0x00000032),
    RM_ENGINE_TYPE_DPU                             =       (0x00000033),
    RM_ENGINE_TYPE_PMU                             =       (0x00000034),
    RM_ENGINE_TYPE_FBFLCN                          =       (0x00000035),
    RM_ENGINE_TYPE_NVJPEG0                         =       (0x00000036),
    RM_ENGINE_TYPE_NVJPEG1                         =       (0x00000037),
    RM_ENGINE_TYPE_NVJPEG2                         =       (0x00000038),
    RM_ENGINE_TYPE_NVJPEG3                         =       (0x00000039),
    RM_ENGINE_TYPE_NVJPEG4                         =       (0x0000003a),
    RM_ENGINE_TYPE_NVJPEG5                         =       (0x0000003b),
    RM_ENGINE_TYPE_NVJPEG6                         =       (0x0000003c),
    RM_ENGINE_TYPE_NVJPEG7                         =       (0x0000003d),
    RM_ENGINE_TYPE_OFA0                            =       (0x0000003e),
    // removal tracking bug: 3748354
    RM_ENGINE_TYPE_OFA1                            =       (0x0000003f),
    RM_ENGINE_TYPE_RESERVED40                      =       (0x00000040),
    RM_ENGINE_TYPE_RESERVED41                      =       (0x00000041),
    RM_ENGINE_TYPE_RESERVED42                      =       (0x00000042),
    RM_ENGINE_TYPE_RESERVED43                      =       (0x00000043),
    RM_ENGINE_TYPE_RESERVED44                      =       (0x00000044),
    RM_ENGINE_TYPE_RESERVED45                      =       (0x00000045),
    RM_ENGINE_TYPE_RESERVED46                      =       (0x00000046),
    RM_ENGINE_TYPE_RESERVED47                      =       (0x00000047),
    RM_ENGINE_TYPE_RESERVED48                      =       (0x00000048),
    RM_ENGINE_TYPE_RESERVED49                      =       (0x00000049),
    RM_ENGINE_TYPE_RESERVED4a                      =       (0x0000004a),
    RM_ENGINE_TYPE_RESERVED4b                      =       (0x0000004b),
    RM_ENGINE_TYPE_RESERVED4c                      =       (0x0000004c),
    RM_ENGINE_TYPE_RESERVED4d                      =       (0x0000004d),
    RM_ENGINE_TYPE_RESERVED4e                      =       (0x0000004e),
    RM_ENGINE_TYPE_RESERVED4f                      =       (0x0000004f),
    RM_ENGINE_TYPE_RESERVED50                      =       (0x00000050),
    RM_ENGINE_TYPE_RESERVED51                      =       (0x00000051),
    RM_ENGINE_TYPE_RESERVED52                      =       (0x00000052),
    RM_ENGINE_TYPE_RESERVED53                      =       (0x00000053),
    RM_ENGINE_TYPE_LAST                            =       (0x00000054),
} RM_ENGINE_TYPE;

//
// The duplicates in the RM_ENGINE_TYPE. Using define instead of putting them
// in the enum to make sure that each item in the enum has a unique number.
//
#define RM_ENGINE_TYPE_GRAPHICS                 RM_ENGINE_TYPE_GR0
#define RM_ENGINE_TYPE_BSP                      RM_ENGINE_TYPE_NVDEC0
#define RM_ENGINE_TYPE_MSENC                    RM_ENGINE_TYPE_NVENC0
#define RM_ENGINE_TYPE_CIPHER                   RM_ENGINE_TYPE_TSEC
#define RM_ENGINE_TYPE_NVJPG                    RM_ENGINE_TYPE_NVJPEG0

// removal tracking bug: 3748354
#define RM_ENGINE_TYPE_COPY_SIZE 20
#define RM_ENGINE_TYPE_NVENC_SIZE 3
#define RM_ENGINE_TYPE_NVJPEG_SIZE 8
#define RM_ENGINE_TYPE_NVDEC_SIZE 8
// removal tracking bug: 3748354
#define RM_ENGINE_TYPE_OFA_SIZE 2
#define RM_ENGINE_TYPE_GR_SIZE 8

// Indexed engines
#define RM_ENGINE_TYPE_COPY(i)     (RM_ENGINE_TYPE_COPY0+(i))
#define RM_ENGINE_TYPE_IS_COPY(i)  (((i) >= RM_ENGINE_TYPE_COPY0) && ((i) < RM_ENGINE_TYPE_COPY(RM_ENGINE_TYPE_COPY_SIZE)))
#define RM_ENGINE_TYPE_COPY_IDX(i) ((i) - RM_ENGINE_TYPE_COPY0)

#define RM_ENGINE_TYPE_NVENC(i)    (RM_ENGINE_TYPE_NVENC0+(i))
#define RM_ENGINE_TYPE_IS_NVENC(i)  (((i) >= RM_ENGINE_TYPE_NVENC0) && ((i) < RM_ENGINE_TYPE_NVENC(RM_ENGINE_TYPE_NVENC_SIZE)))
#define RM_ENGINE_TYPE_NVENC_IDX(i) ((i) - RM_ENGINE_TYPE_NVENC0)

#define RM_ENGINE_TYPE_NVDEC(i)    (RM_ENGINE_TYPE_NVDEC0+(i))
#define RM_ENGINE_TYPE_IS_NVDEC(i)  (((i) >= RM_ENGINE_TYPE_NVDEC0) && ((i) < RM_ENGINE_TYPE_NVDEC(RM_ENGINE_TYPE_NVDEC_SIZE)))
#define RM_ENGINE_TYPE_NVDEC_IDX(i) ((i) - RM_ENGINE_TYPE_NVDEC0)

#define RM_ENGINE_TYPE_NVJPEG(i)    (RM_ENGINE_TYPE_NVJPEG0+(i))
#define RM_ENGINE_TYPE_IS_NVJPEG(i)  (((i) >= RM_ENGINE_TYPE_NVJPEG0) && ((i) < RM_ENGINE_TYPE_NVJPEG(RM_ENGINE_TYPE_NVJPEG_SIZE)))
#define RM_ENGINE_TYPE_NVJPEG_IDX(i) ((i) - RM_ENGINE_TYPE_NVJPEG0)

#define RM_ENGINE_TYPE_OFA(i)      (RM_ENGINE_TYPE_OFA0+(i))
#define RM_ENGINE_TYPE_IS_OFA(i)   (((i) >= RM_ENGINE_TYPE_OFA0) && ((i) < RM_ENGINE_TYPE_OFA(RM_ENGINE_TYPE_OFA_SIZE)))
#define RM_ENGINE_TYPE_OFA_IDX(i)  ((i) - RM_ENGINE_TYPE_OFA0)

#define RM_ENGINE_TYPE_IS_VIDEO(i) (RM_ENGINE_TYPE_IS_NVENC(i)  | \
                                    RM_ENGINE_TYPE_IS_NVDEC(i)  | \
                                    RM_ENGINE_TYPE_IS_NVJPEG(i) | \
                                    RM_ENGINE_TYPE_IS_OFA(i))

#define RM_ENGINE_TYPE_GR(i)       (RM_ENGINE_TYPE_GR0 + (i))
#define RM_ENGINE_TYPE_IS_GR(i)    (((i) >= RM_ENGINE_TYPE_GR0) && ((i) < RM_ENGINE_TYPE_GR(RM_ENGINE_TYPE_GR_SIZE)))
#define RM_ENGINE_TYPE_GR_IDX(i)   ((i) - RM_ENGINE_TYPE_GR0)

#define RM_ENGINE_TYPE_IS_VALID(i) (((i) > (RM_ENGINE_TYPE_NULL)) && ((i) < (RM_ENGINE_TYPE_LAST)))

// Engine Range defines
#define RM_ENGINE_RANGE_GR()    rangeMake(RM_ENGINE_TYPE_GR(0), RM_ENGINE_TYPE_GR(RM_ENGINE_TYPE_GR_SIZE - 1))
#define RM_ENGINE_RANGE_COPY()  rangeMake(RM_ENGINE_TYPE_COPY(0), RM_ENGINE_TYPE_COPY(RM_ENGINE_TYPE_COPY_SIZE - 1))
#define RM_ENGINE_RANGE_NVDEC() rangeMake(RM_ENGINE_TYPE_NVDEC(0), RM_ENGINE_TYPE_NVDEC(RM_ENGINE_TYPE_NVDEC_SIZE - 1))
#define RM_ENGINE_RANGE_NVENC() rangeMake(RM_ENGINE_TYPE_NVENC(0), RM_ENGINE_TYPE_NVENC(RM_ENGINE_TYPE_NVENC_SIZE - 1))
#define RM_ENGINE_RANGE_NVJPEG() rangeMake(RM_ENGINE_TYPE_NVJPEG(0), RM_ENGINE_TYPE_NVJPEG(RM_ENGINE_TYPE_NVJPEG_SIZE - 1))
#define RM_ENGINE_RANGE_OFA()    rangeMake(RM_ENGINE_TYPE_OFA(0), RM_ENGINE_TYPE_OFA(RM_ENGINE_TYPE_OFA_SIZE - 1))

// Bit Vectors
MAKE_BITVECTOR(ENGTYPE_BIT_VECTOR, RM_ENGINE_TYPE_LAST);

#endif //_GPU_ENGINE_TYPE_H_
