/*
 * SPDX-FileCopyrightText: Copyright (c) 2003-2021 NVIDIA CORPORATION & AFFILIATES
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

#ifndef TU102_KIND_MACROS_H_INCLUDED
#define TU102_KIND_MACROS_H_INCLUDED

#define KIND_INVALID(k)               (                                      ((k) ==NV_MMU_CLIENT_KIND_INVALID))
#define PTEKIND_PITCH(k)              (                                      ((k) ==NV_MMU_PTE_KIND_PITCH)||                                      ((k) ==NV_MMU_PTE_KIND_SMSKED_MESSAGE))
#define PTEKIND_COMPRESSIBLE(k)       (                                      ((k) >=NV_MMU_PTE_KIND_GENERIC_MEMORY_COMPRESSIBLE && (k) <= NV_MMU_PTE_KIND_Z24S8_COMPRESSIBLE_DISABLE_PLC))
#define PTEKIND_DISALLOWS_PLC(k)      (                                      !((k) ==NV_MMU_PTE_KIND_GENERIC_MEMORY_COMPRESSIBLE))
#define PTEKIND_SUPPORTED(k)          (                                      ((k) ==NV_MMU_PTE_KIND_INVALID)||                                      ((k) ==NV_MMU_PTE_KIND_PITCH)||                                      ((k) ==NV_MMU_PTE_KIND_GENERIC_MEMORY)||                                      ((k) >=NV_MMU_PTE_KIND_Z16 && (k) <= NV_MMU_PTE_KIND_Z24S8)||                                      ((k) >=NV_MMU_PTE_KIND_GENERIC_MEMORY_COMPRESSIBLE && (k) <= NV_MMU_PTE_KIND_SMSKED_MESSAGE))
#define KIND_Z(k)                     (                                      ((k) >=NV_MMU_CLIENT_KIND_Z16 && (k) <= NV_MMU_CLIENT_KIND_Z24S8))
#define PTEKIND_Z(k)                  (                                      ((k) >=NV_MMU_PTE_KIND_Z16 && (k) <= NV_MMU_PTE_KIND_Z24S8)||                                      ((k) >=NV_MMU_PTE_KIND_S8_COMPRESSIBLE_DISABLE_PLC && (k) <= NV_MMU_PTE_KIND_Z24S8_COMPRESSIBLE_DISABLE_PLC))
#define PTEKIND_GENERIC_MEMORY(k)     (                                      ((k) ==NV_MMU_PTE_KIND_GENERIC_MEMORY)||                                      ((k) >=NV_MMU_PTE_KIND_GENERIC_MEMORY_COMPRESSIBLE && (k) <= NV_MMU_PTE_KIND_GENERIC_MEMORY_COMPRESSIBLE_DISABLE_PLC))

#endif // TU102_KIND_MACROS_H_INCLUDED
