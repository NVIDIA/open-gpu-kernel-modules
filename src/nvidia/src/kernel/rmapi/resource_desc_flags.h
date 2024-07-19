/*
 * SPDX-FileCopyrightText: Copyright (c) 2016-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#ifndef _RESOURCE_DESC_FLAGS_H_
#define _RESOURCE_DESC_FLAGS_H_

// Flags for RS_ENTRY
#define RS_FLAGS_NONE                             0

#define RS_FLAGS_ACQUIRE_GPUS_LOCK_ON_ALLOC       NVBIT(0)   ///< GPUs Lock is acquired on allocation
#define RS_FLAGS_ACQUIRE_GPUS_LOCK_ON_FREE        NVBIT(1)   ///< GPUs Lock is acquired for free

#define RS_FLAGS_ACQUIRE_GPU_GROUP_LOCK_ON_ALLOC  NVBIT(2)   ///< GPU Group Lock is acquired on allocation
#define RS_FLAGS_ACQUIRE_GPU_GROUP_LOCK_ON_FREE   NVBIT(3)   ///< GPU Group Lock is acquired for free

#define RS_FLAGS_ALLOC_RPC_TO_VGPU_HOST           NVBIT(4)   ///< Issue RPC to host to allocate resource for virtual GPUs

#define RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC     NVBIT(5)   ///< Acquire the RO API lock for allocation, default is RW API lock

#define RS_FLAGS_ALLOC_RPC_TO_PHYS_RM             NVBIT(6)   ///< Issue RPC to allocate resource in physical RM

#define RS_FLAGS_ALLOC_RPC_TO_ALL                 (RS_FLAGS_ALLOC_RPC_TO_VGPU_HOST | RS_FLAGS_ALLOC_RPC_TO_PHYS_RM)

#define RS_FLAGS_INTERNAL_ONLY                    NVBIT(7)   ///< Class cannot be allocated outside of RM

#define RS_FLAGS_CHANNEL_DESCENDANT_COMMON        (RS_FLAGS_ACQUIRE_GPUS_LOCK | RS_FLAGS_ALLOC_RPC_TO_ALL)

#define RS_FREE_PRIORITY_DEFAULT                  0
#define RS_FREE_PRIORITY_HIGH                     1          ///< Resources with this priority will be freed ahead of others

#define RS_FLAGS_ALLOC_NON_PRIVILEGED             NVBIT(8)   ///< Class is non privileged

#define RS_FLAGS_ALLOC_PRIVILEGED                 NVBIT(9)   ///< Class requires at least admin privilege

#define RS_FLAGS_ALLOC_KERNEL_PRIVILEGED          NVBIT(10)  ///< Class requires at least kernel privilege

/**
 * CPU_PLUGIN_FOR_* indicates object can be allocated in the respective environment
 *                  if the context is at least cached admin privileged
 *     - Cached-admin Hyper-V may access flagged privileged and kernel privileged objects
 *     - Other, runtime-admin hosts may access flagged kernel privileged objects
 *     - Note that runtime-admin hosts do not need the flag to allocate admin privileged objects
 *
 * GSP_PLUGIN_FOR_* is even stricter; any admin and kernel privileged object allocated in a VF context
 *                  is required to have the flag or it will be rejected.
 */
#define RS_FLAGS_ALLOC_CPU_PLUGIN_FOR_VGPU_GSP    NVBIT(11)  ///< CPU-RM, SRIOV, vGPU-GSP enabled, hypervisor environment

#define RS_FLAGS_ALLOC_CPU_PLUGIN_FOR_SRIOV       NVBIT(12)  ///< CPU-RM, SRIOV, vGPU-GSP disabled, hypervisor environment

#define RS_FLAGS_ALLOC_CPU_PLUGIN_FOR_LEGACY      NVBIT(13)  ///< CPU-RM, non-SRIOV or SRIOV-Heavy, hypervisor environment.

#define RS_FLAGS_ALLOC_GSP_PLUGIN_FOR_VGPU_GSP    NVBIT(14)  ///< GSP-RM, SRIOV, vGPU-GSP enabled, VF context.

#define RS_FLAGS_ALLOC_ALL_VGPU_PLUGINS           (RS_FLAGS_ALLOC_CPU_PLUGIN_FOR_VGPU_GSP | RS_FLAGS_ALLOC_CPU_PLUGIN_FOR_SRIOV | RS_FLAGS_ALLOC_CPU_PLUGIN_FOR_LEGACY | RS_FLAGS_ALLOC_GSP_PLUGIN_FOR_VGPU_GSP)

#define RS_FLAGS_DUAL_CLIENT_LOCK                 NVBIT(15)  ///< Class needs to lock two clients when being allocated, must update serverAllocLookupSecondClient in order to use

#define RS_FLAGS_ACQUIRE_GPUS_LOCK_ON_DUP         NVBIT(16)  ///< GPUs Lock is acquired on dup

#define RS_FLAGS_ACQUIRE_GPU_GROUP_LOCK_ON_DUP    NVBIT(17)  ///< GPU Group Lock is acquired for dup

#define RS_FLAGS_ACQUIRE_GPUS_LOCK                (RS_FLAGS_ACQUIRE_GPUS_LOCK_ON_ALLOC | RS_FLAGS_ACQUIRE_GPUS_LOCK_ON_FREE | RS_FLAGS_ACQUIRE_GPUS_LOCK_ON_DUP)
#define RS_FLAGS_ACQUIRE_GPU_GROUP_LOCK           (RS_FLAGS_ACQUIRE_GPU_GROUP_LOCK_ON_ALLOC | RS_FLAGS_ACQUIRE_GPU_GROUP_LOCK_ON_FREE | RS_FLAGS_ACQUIRE_GPU_GROUP_LOCK_ON_DUP)

#endif // _RESOURCE_DESC_FLAGS_H_
