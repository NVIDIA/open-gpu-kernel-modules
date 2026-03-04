/*
 * SPDX-FileCopyrightText: Copyright (c) 2017 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef __NV_XZ_MEM_HOOKS_H__
#define __NV_XZ_MEM_HOOKS_H__

/*
 * This file is included by xz_config.h when NV_XZ_CUSTOM_MEM_HOOKS is defined,
 * allowing us to override xzminidec's standard library use.
 */

#include "nvidia-3d-imports.h"

#define kmalloc(size, flags) nv3dImportAlloc(size)
#define kfree(ptr)           nv3dImportFree(ptr)
#define vmalloc(size)        nv3dImportAlloc(size)
#define vfree(ptr)           nv3dImportFree(ptr)

#define memeq(a, b, size)   (nv3dImportMemCmp(a, b, size) == 0)
#define memzero(buf, size)   nv3dImportMemSet(buf, 0, size)
#define memcpy(a, b, size)   nv3dImportMemCpy(a, b, size)
#define memmove(a, b, size)  nv3dImportMemMove(a, b, size)

#endif /* __NV_XZ_MEM_HOOKS_H__ */
