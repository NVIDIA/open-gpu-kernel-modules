/*
 * SPDX-FileCopyrightText: Copyright (c) 2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

//
// This file provides common types for the UVM kernel driver, UVM user layer,
// and RM's UVM interface.
//

#ifndef _NV_UVM_USER_TYPES_H_
#define _NV_UVM_USER_TYPES_H_

#include "nvtypes.h"

//
// Default Page Size if left "0" because in RM BIG page size is default & there
// are multiple BIG page sizes in RM. These defines are used as flags to "0"
// should be OK when user is not sure which pagesize allocation it wants
//
#define UVM_PAGE_SIZE_DEFAULT    0x0ULL
#define UVM_PAGE_SIZE_4K         0x1000ULL
#define UVM_PAGE_SIZE_64K        0x10000ULL
#define UVM_PAGE_SIZE_128K       0x20000ULL
#define UVM_PAGE_SIZE_2M         0x200000ULL
#define UVM_PAGE_SIZE_512M       0x20000000ULL
#define UVM_PAGE_SIZE_256G       0x4000000000ULL

typedef enum
{
    UVM_VIRT_MODE_NONE = 0,             // Baremetal or passthrough virtualization
    UVM_VIRT_MODE_LEGACY = 1,           // Virtualization without SRIOV support
    UVM_VIRT_MODE_SRIOV_HEAVY = 2,      // Virtualization with SRIOV Heavy configured
    UVM_VIRT_MODE_SRIOV_STANDARD = 3,   // Virtualization with SRIOV Standard configured
    UVM_VIRT_MODE_COUNT = 4,
} UVM_VIRT_MODE;

//------------------------------------------------------------------------------
// UVM GPU mapping types
//
// These types indicate the kinds of accesses allowed from a given GPU at the
// specified virtual address range. There are 3 basic kinds of accesses: read,
// write and atomics. Each type indicates what kinds of accesses are allowed.
// Accesses of any disallowed kind are fatal. The "Default" type specifies that
// the UVM driver should decide on the types of accesses allowed.
//------------------------------------------------------------------------------
typedef enum
{
    UvmGpuMappingTypeDefault = 0,
    UvmGpuMappingTypeReadWriteAtomic = 1,
    UvmGpuMappingTypeReadWrite = 2,
    UvmGpuMappingTypeReadOnly = 3,
    UvmGpuMappingTypeCount = 4
} UvmGpuMappingType;

//------------------------------------------------------------------------------
// UVM GPU caching types
//
// These types indicate the cacheability of the specified virtual address range
// from a given GPU. The "Default" type specifies that the UVM driver should
// set caching on or off as required to follow the UVM coherence model. The
// "ForceUncached" and "ForceCached" types will always turn caching off or on
// respectively. These two types override the cacheability specified by the UVM
// coherence model.
//------------------------------------------------------------------------------
typedef enum
{
    UvmGpuCachingTypeDefault = 0,
    UvmGpuCachingTypeForceUncached = 1,
    UvmGpuCachingTypeForceCached = 2,
    UvmGpuCachingTypeCount = 3
} UvmGpuCachingType;

//------------------------------------------------------------------------------
// UVM GPU format types
//
// These types indicate the memory format of the specified virtual address
// range for a given GPU. The "Default" type specifies that the UVM driver will
// detect the format based on the allocation and is mutually inclusive with
// UvmGpuFormatElementBitsDefault.
//------------------------------------------------------------------------------
typedef enum {
   UvmGpuFormatTypeDefault = 0,
   UvmGpuFormatTypeBlockLinear = 1,
   UvmGpuFormatTypeCount = 2
} UvmGpuFormatType;

//------------------------------------------------------------------------------
// UVM GPU Element bits types
//
// These types indicate the element size of the specified virtual address range
// for a given GPU. The "Default" type specifies that the UVM driver will
// detect the element size based on the allocation and is mutually inclusive
// with UvmGpuFormatTypeDefault. The element size is specified in bits:
// UvmGpuFormatElementBits8 uses the 8-bits format.
//------------------------------------------------------------------------------
typedef enum {
   UvmGpuFormatElementBitsDefault = 0,
   UvmGpuFormatElementBits8 = 1,
   UvmGpuFormatElementBits16 = 2,
   // Cuda does not support 24-bit width
   UvmGpuFormatElementBits32 = 4,
   UvmGpuFormatElementBits64 = 5,
   UvmGpuFormatElementBits128 = 6,
   UvmGpuFormatElementBitsCount = 7
} UvmGpuFormatElementBits;

//------------------------------------------------------------------------------
// UVM GPU Compression types
//
// These types indicate the compression type of the specified virtual address
// range for a given GPU. The "Default" type specifies that the UVM driver will
// detect the compression attributes based on the allocation. Any type other
// than the default will override the compression behavior of the physical
// allocation. UvmGpuCompressionTypeEnabledNoPlc will disable PLC but enables
// generic compression. UvmGpuCompressionTypeEnabledNoPlc type is only supported
// on Turing plus GPUs. Since UvmGpuCompressionTypeEnabledNoPlc type enables
// generic compression, it can only be used when the compression attribute of
// the underlying physical allocation is enabled.
//------------------------------------------------------------------------------
typedef enum {
    UvmGpuCompressionTypeDefault = 0,
    UvmGpuCompressionTypeEnabledNoPlc = 1,
    UvmGpuCompressionTypeCount = 2
} UvmGpuCompressionType;

//
// Mirrored in PMA (PMA_STATS)
//
typedef struct UvmPmaStatistics_tag
{
    volatile NvU64 numPages2m;                // PMA-wide 2MB pages count across all regions
    volatile NvU64 numFreePages64k;           // PMA-wide free 64KB page count across all regions
    volatile NvU64 numFreePages2m;            // PMA-wide free 2MB pages count across all regions
    volatile NvU64 numPages2mProtected;       // PMA-wide 2MB pages count in protected memory
    volatile NvU64 numFreePages64kProtected;  // PMA-wide free 64KB page count in protected memory
    volatile NvU64 numFreePages2mProtected;   // PMA-wide free 2MB pages count in protected memory
} UvmPmaStatistics;

typedef enum
{
    UVM_ACCESS_COUNTER_GRANULARITY_64K = 1,
    UVM_ACCESS_COUNTER_GRANULARITY_2M  = 2,
    UVM_ACCESS_COUNTER_GRANULARITY_16M = 3,
    UVM_ACCESS_COUNTER_GRANULARITY_16G = 4,
} UVM_ACCESS_COUNTER_GRANULARITY;

#endif // _NV_UVM_USER_TYPES_H_
