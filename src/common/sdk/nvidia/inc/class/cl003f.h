/*
 * Copyright (c) 2001-2001, NVIDIA CORPORATION. All rights reserved.
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
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef _cl003f_h_
#define _cl003f_h_

#ifdef __cplusplus
extern "C" {
#endif

#include "nvtypes.h"

#define  NV01_MEMORY_LOCAL_PRIVILEGED                              (0x0000003F)
/* NvNotification[] fields and values */
#define NV03F_NOTIFICATION_STATUS_ERROR_PROTECTION_FAULT           (0x4000)
/* pio method data structure */
#ifndef AMD64
typedef volatile struct _cl003f_tag0 {
#else
typedef struct {
#endif
 NvV32 Reserved00[0x7c0];
} Nv01MemoryLocalPrivileged;
#define NV03F_TYPEDEF                                 Nv01MemoryLocalPrivileged
typedef Nv01MemoryLocalPrivileged Nv03fTypedef;
/* obsolete stuff */
#define NV01_MEMORY_PRIVILEGED                                     (0x0000003F)
#define NV1_MEMORY_PRIVILEGED                                      (0x0000003F)
#define Nv01MemoryPrivileged                          Nv01MemoryLocalPrivileged
#define nv01MemoryPrivileged                          Nv01MemoryLocalPrivileged
#define Nv1MemoryPrivileged                           Nv01MemoryLocalPrivileged
#define nv1MemoryPrivileged                           Nv01MemoryLocalPrivileged
#define nv01MemoryLocalPrivileged                     Nv01MemoryLocalPrivileged

#ifdef __cplusplus
};     /* extern "C" */
#endif

#endif /* _cl003f_h_ */
