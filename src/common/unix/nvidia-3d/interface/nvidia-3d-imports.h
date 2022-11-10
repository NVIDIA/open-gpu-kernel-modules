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
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/*
 * nvidia-3d-imports.h declares functions with nvidia-3d host drivers must
 * provide.
 */

#ifndef __NVIDIA_3D_IMPORTS_H__
#define __NVIDIA_3D_IMPORTS_H__

#include <stddef.h> /* size_t */

void *nv3dImportAlloc(size_t size);
void nv3dImportFree(void *ptr);
int nv3dImportMemCmp(const void *a, const void *b, size_t size);
void nv3dImportMemSet(void *s, int c, size_t size);
void nv3dImportMemCpy(void *dest, const void *src, size_t size);
void nv3dImportMemMove(void *dest, const void *src, size_t size);

#endif /* __NVIDIA_3D_IMPORTS_H__ */
