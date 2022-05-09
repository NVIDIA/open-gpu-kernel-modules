/*
 * SPDX-FileCopyrightText: Copyright (c) 2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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


#ifndef NV_IOCTL_NUMA_H
#define NV_IOCTL_NUMA_H

#if defined(NV_LINUX)

#include <nv-ioctl-numbers.h>

#if defined(NV_KERNEL_INTERFACE_LAYER)

#include <linux/types.h>

#else

#include <stdint.h>

#if !defined(__aligned)
#define __aligned(n) __attribute__((aligned(n)))
#endif

#endif

#define NV_ESC_NUMA_INFO         (NV_IOCTL_BASE + 15)
#define NV_ESC_SET_NUMA_STATUS   (NV_IOCTL_BASE + 16)

#define NV_IOCTL_NUMA_INFO_MAX_OFFLINE_ADDRESSES 64
typedef struct offline_addresses
{
    uint64_t addresses[NV_IOCTL_NUMA_INFO_MAX_OFFLINE_ADDRESSES] __aligned(8);
    uint32_t numEntries;
} nv_offline_addresses_t;


/* per-device NUMA memory info as assigned by the system */
typedef struct nv_ioctl_numa_info
{
    int32_t  nid;
    int32_t  status;
    uint64_t memblock_size __aligned(8);
    uint64_t numa_mem_addr __aligned(8);
    uint64_t numa_mem_size __aligned(8);
    nv_offline_addresses_t offline_addresses __aligned(8);
} nv_ioctl_numa_info_t;

/* set the status of the device NUMA memory */
typedef struct nv_ioctl_set_numa_status
{
    int32_t status;
} nv_ioctl_set_numa_status_t;

#define NV_IOCTL_NUMA_STATUS_DISABLED               0
#define NV_IOCTL_NUMA_STATUS_OFFLINE                1
#define NV_IOCTL_NUMA_STATUS_ONLINE_IN_PROGRESS     2
#define NV_IOCTL_NUMA_STATUS_ONLINE                 3
#define NV_IOCTL_NUMA_STATUS_ONLINE_FAILED          4
#define NV_IOCTL_NUMA_STATUS_OFFLINE_IN_PROGRESS    5
#define NV_IOCTL_NUMA_STATUS_OFFLINE_FAILED         6

#endif

#endif
