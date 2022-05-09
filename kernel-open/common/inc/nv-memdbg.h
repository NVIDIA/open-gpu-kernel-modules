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

#ifndef _NVMEMDBG_H_
#define _NVMEMDBG_H_

#include <nvtypes.h>

void nv_memdbg_init(void);
void nv_memdbg_add(void *addr, NvU64 size, const char *file, int line);
void nv_memdbg_remove(void *addr, NvU64 size, const char *file, int line);
void nv_memdbg_exit(void);

#if defined(NV_MEM_LOGGER)

#define NV_MEMDBG_ADD(ptr, size) \
    nv_memdbg_add(ptr, size, __FILE__, __LINE__)

#define NV_MEMDBG_REMOVE(ptr, size) \
    nv_memdbg_remove(ptr, size, __FILE__, __LINE__)

#else

#define NV_MEMDBG_ADD(ptr, size)
#define NV_MEMDBG_REMOVE(ptr, size)

#endif /* NV_MEM_LOGGER */

#endif /* _NVMEMDBG_H_ */
