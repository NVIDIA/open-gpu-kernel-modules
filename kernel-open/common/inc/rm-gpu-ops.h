/*
 * SPDX-FileCopyrightText: Copyright (c) 1999-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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


#ifndef _RM_GPU_OPS_H_
#define _RM_GPU_OPS_H_



#include <nvtypes.h>
#include <nvCpuUuid.h>
#include "nv_stdarg.h"
#include <nv-ioctl.h>
#include <nvmisc.h>

NV_STATUS  NV_API_CALL  rm_gpu_ops_create_session (nvidia_stack_t *, nvgpuSessionHandle_t *);
NV_STATUS  NV_API_CALL  rm_gpu_ops_destroy_session (nvidia_stack_t *, nvgpuSessionHandle_t);
NV_STATUS  NV_API_CALL  rm_gpu_ops_device_create (nvidia_stack_t *, nvgpuSessionHandle_t, const nvgpuInfo_t *, const NvProcessorUuid *, nvgpuDeviceHandle_t *, NvBool);
NV_STATUS  NV_API_CALL  rm_gpu_ops_device_destroy (nvidia_stack_t *, nvgpuDeviceHandle_t);
NV_STATUS  NV_API_CALL  rm_gpu_ops_address_space_create(nvidia_stack_t *, nvgpuDeviceHandle_t, unsigned long long, unsigned long long, NvBool, nvgpuAddressSpaceHandle_t *, nvgpuAddressSpaceInfo_t);
NV_STATUS  NV_API_CALL  rm_gpu_ops_dup_address_space(nvidia_stack_t *, nvgpuDeviceHandle_t, NvHandle, NvHandle, nvgpuAddressSpaceHandle_t *, nvgpuAddressSpaceInfo_t);
NV_STATUS  NV_API_CALL  rm_gpu_ops_address_space_destroy(nvidia_stack_t *, nvgpuAddressSpaceHandle_t);
NV_STATUS  NV_API_CALL  rm_gpu_ops_memory_alloc_fb(nvidia_stack_t *, nvgpuAddressSpaceHandle_t, NvLength, NvU64 *, nvgpuAllocInfo_t);

NV_STATUS  NV_API_CALL  rm_gpu_ops_pma_alloc_pages(nvidia_stack_t *, void *, NvLength, NvU32 , nvgpuPmaAllocationOptions_t, NvU64 *);
NV_STATUS  NV_API_CALL  rm_gpu_ops_pma_free_pages(nvidia_stack_t *, void *, NvU64 *, NvLength , NvU32, NvU32);
NV_STATUS  NV_API_CALL  rm_gpu_ops_pma_pin_pages(nvidia_stack_t *, void *, NvU64 *, NvLength , NvU32, NvU32);
NV_STATUS  NV_API_CALL  rm_gpu_ops_get_pma_object(nvidia_stack_t *, nvgpuDeviceHandle_t, void **, const nvgpuPmaStatistics_t *);
NV_STATUS  NV_API_CALL  rm_gpu_ops_pma_register_callbacks(nvidia_stack_t *sp, void *, nvPmaEvictPagesCallback, nvPmaEvictRangeCallback, void *);
void       NV_API_CALL  rm_gpu_ops_pma_unregister_callbacks(nvidia_stack_t *sp, void *);

NV_STATUS  NV_API_CALL  rm_gpu_ops_memory_alloc_sys(nvidia_stack_t *, nvgpuAddressSpaceHandle_t, NvLength, NvU64 *, nvgpuAllocInfo_t);

NV_STATUS  NV_API_CALL  rm_gpu_ops_get_p2p_caps(nvidia_stack_t *, nvgpuDeviceHandle_t, nvgpuDeviceHandle_t, nvgpuP2PCapsParams_t);

NV_STATUS  NV_API_CALL  rm_gpu_ops_memory_cpu_map(nvidia_stack_t *, nvgpuAddressSpaceHandle_t, NvU64, NvLength, void **, NvU32);
NV_STATUS  NV_API_CALL  rm_gpu_ops_memory_cpu_ummap(nvidia_stack_t *, nvgpuAddressSpaceHandle_t, void*);
NV_STATUS  NV_API_CALL  rm_gpu_ops_tsg_allocate(nvidia_stack_t *, nvgpuAddressSpaceHandle_t, const nvgpuTsgAllocParams_t *, nvgpuTsgHandle_t *);
NV_STATUS  NV_API_CALL  rm_gpu_ops_tsg_destroy(nvidia_stack_t *, nvgpuTsgHandle_t);
NV_STATUS  NV_API_CALL  rm_gpu_ops_channel_allocate(nvidia_stack_t *, const nvgpuTsgHandle_t, const nvgpuChannelAllocParams_t *, nvgpuChannelHandle_t *, nvgpuChannelInfo_t);
NV_STATUS  NV_API_CALL  rm_gpu_ops_channel_destroy(nvidia_stack_t *, nvgpuChannelHandle_t);
NV_STATUS  NV_API_CALL rm_gpu_ops_memory_free(nvidia_stack_t *, nvgpuAddressSpaceHandle_t, NvU64);
NV_STATUS  NV_API_CALL rm_gpu_ops_query_caps(nvidia_stack_t *, nvgpuDeviceHandle_t, nvgpuCaps_t);
NV_STATUS  NV_API_CALL rm_gpu_ops_query_ces_caps(nvidia_stack_t *sp, nvgpuDeviceHandle_t, nvgpuCesCaps_t);
NV_STATUS  NV_API_CALL rm_gpu_ops_get_gpu_info(nvidia_stack_t *, const NvProcessorUuid *pUuid, const nvgpuClientInfo_t *, nvgpuInfo_t *);
NV_STATUS  NV_API_CALL rm_gpu_ops_service_device_interrupts_rm(nvidia_stack_t *, nvgpuDeviceHandle_t);
NV_STATUS  NV_API_CALL rm_gpu_ops_dup_allocation(nvidia_stack_t *, nvgpuAddressSpaceHandle_t, NvU64, nvgpuAddressSpaceHandle_t, NvU64, NvU64 *);

NV_STATUS  NV_API_CALL  rm_gpu_ops_dup_memory (nvidia_stack_t *, nvgpuDeviceHandle_t, NvHandle, NvHandle, NvHandle *, nvgpuMemoryInfo_t);

NV_STATUS  NV_API_CALL rm_gpu_ops_free_duped_handle(nvidia_stack_t *, nvgpuDeviceHandle_t, NvHandle);
NV_STATUS  NV_API_CALL rm_gpu_ops_get_fb_info(nvidia_stack_t *, nvgpuDeviceHandle_t, nvgpuFbInfo_t);
NV_STATUS  NV_API_CALL rm_gpu_ops_get_ecc_info(nvidia_stack_t *, nvgpuDeviceHandle_t, nvgpuEccInfo_t);
NV_STATUS NV_API_CALL rm_gpu_ops_own_page_fault_intr(nvidia_stack_t *, nvgpuDeviceHandle_t, NvBool);
NV_STATUS  NV_API_CALL rm_gpu_ops_init_fault_info(nvidia_stack_t *, nvgpuDeviceHandle_t, nvgpuFaultInfo_t);
NV_STATUS  NV_API_CALL rm_gpu_ops_destroy_fault_info(nvidia_stack_t *, nvgpuDeviceHandle_t, nvgpuFaultInfo_t);
NV_STATUS  NV_API_CALL rm_gpu_ops_get_non_replayable_faults(nvidia_stack_t *, nvgpuFaultInfo_t, void *, NvU32 *);
NV_STATUS  NV_API_CALL rm_gpu_ops_flush_replayable_fault_buffer(nvidia_stack_t *, nvgpuFaultInfo_t, NvBool);
NV_STATUS  NV_API_CALL rm_gpu_ops_toggle_prefetch_faults(nvidia_stack_t *, nvgpuFaultInfo_t, NvBool);
NV_STATUS  NV_API_CALL rm_gpu_ops_has_pending_non_replayable_faults(nvidia_stack_t *, nvgpuFaultInfo_t, NvBool *);
NV_STATUS  NV_API_CALL rm_gpu_ops_init_access_cntr_info(nvidia_stack_t *, nvgpuDeviceHandle_t, nvgpuAccessCntrInfo_t, NvU32);
NV_STATUS  NV_API_CALL rm_gpu_ops_destroy_access_cntr_info(nvidia_stack_t *, nvgpuDeviceHandle_t, nvgpuAccessCntrInfo_t);
NV_STATUS  NV_API_CALL rm_gpu_ops_own_access_cntr_intr(nvidia_stack_t *, nvgpuSessionHandle_t, nvgpuAccessCntrInfo_t, NvBool);
NV_STATUS  NV_API_CALL rm_gpu_ops_enable_access_cntr(nvidia_stack_t *, nvgpuDeviceHandle_t, nvgpuAccessCntrInfo_t, nvgpuAccessCntrConfig_t);
NV_STATUS  NV_API_CALL rm_gpu_ops_disable_access_cntr(nvidia_stack_t *, nvgpuDeviceHandle_t, nvgpuAccessCntrInfo_t);
NV_STATUS  NV_API_CALL  rm_gpu_ops_set_page_directory (nvidia_stack_t *, nvgpuAddressSpaceHandle_t, NvU64, unsigned, NvBool, NvU32);
NV_STATUS  NV_API_CALL  rm_gpu_ops_unset_page_directory (nvidia_stack_t *, nvgpuAddressSpaceHandle_t);
NV_STATUS  NV_API_CALL rm_gpu_ops_p2p_object_create(nvidia_stack_t *, nvgpuDeviceHandle_t, nvgpuDeviceHandle_t, NvHandle *);
void       NV_API_CALL rm_gpu_ops_p2p_object_destroy(nvidia_stack_t *, nvgpuSessionHandle_t, NvHandle);
NV_STATUS  NV_API_CALL rm_gpu_ops_get_external_alloc_ptes(nvidia_stack_t*, nvgpuAddressSpaceHandle_t, NvHandle, NvU64, NvU64, nvgpuExternalMappingInfo_t);
NV_STATUS  NV_API_CALL rm_gpu_ops_retain_channel(nvidia_stack_t *, nvgpuAddressSpaceHandle_t, NvHandle, NvHandle, void **, nvgpuChannelInstanceInfo_t);
NV_STATUS  NV_API_CALL rm_gpu_ops_bind_channel_resources(nvidia_stack_t *, void *, nvgpuChannelResourceBindParams_t);
void       NV_API_CALL rm_gpu_ops_release_channel(nvidia_stack_t *, void *);
void       NV_API_CALL rm_gpu_ops_stop_channel(nvidia_stack_t *, void *, NvBool);
NV_STATUS  NV_API_CALL rm_gpu_ops_get_channel_resource_ptes(nvidia_stack_t *, nvgpuAddressSpaceHandle_t, NvP64, NvU64, NvU64, nvgpuExternalMappingInfo_t);
NV_STATUS  NV_API_CALL rm_gpu_ops_report_non_replayable_fault(nvidia_stack_t *, nvgpuDeviceHandle_t, const void *);

NV_STATUS  NV_API_CALL rm_gpu_ops_paging_channel_allocate(nvidia_stack_t *, nvgpuDeviceHandle_t, const nvgpuPagingChannelAllocParams_t *, nvgpuPagingChannelHandle_t *, nvgpuPagingChannelInfo_t);
void       NV_API_CALL rm_gpu_ops_paging_channel_destroy(nvidia_stack_t *, nvgpuPagingChannelHandle_t);
NV_STATUS  NV_API_CALL rm_gpu_ops_paging_channels_map(nvidia_stack_t *, nvgpuAddressSpaceHandle_t, NvU64, nvgpuDeviceHandle_t, NvU64 *);
void       NV_API_CALL rm_gpu_ops_paging_channels_unmap(nvidia_stack_t *, nvgpuAddressSpaceHandle_t, NvU64, nvgpuDeviceHandle_t);
NV_STATUS  NV_API_CALL rm_gpu_ops_paging_channel_push_stream(nvidia_stack_t *, nvgpuPagingChannelHandle_t, char *, NvU32);

NV_STATUS  NV_API_CALL rm_gpu_ops_ccsl_context_init(nvidia_stack_t *, struct ccslContext_t **, nvgpuChannelHandle_t);
NV_STATUS  NV_API_CALL rm_gpu_ops_ccsl_context_clear(nvidia_stack_t *, struct ccslContext_t *);
NV_STATUS  NV_API_CALL rm_gpu_ops_ccsl_rotate_key(nvidia_stack_t *, UvmCslContext *[], NvU32);
NV_STATUS  NV_API_CALL rm_gpu_ops_ccsl_rotate_iv(nvidia_stack_t *, struct ccslContext_t *, NvU8);
NV_STATUS  NV_API_CALL rm_gpu_ops_ccsl_encrypt(nvidia_stack_t *, struct ccslContext_t *, NvU32, NvU8 const *, NvU8 *, NvU8 *);
NV_STATUS  NV_API_CALL rm_gpu_ops_ccsl_encrypt_with_iv(nvidia_stack_t *, struct ccslContext_t *, NvU32, NvU8 const *, NvU8*, NvU8 *, NvU8 *);
NV_STATUS  NV_API_CALL rm_gpu_ops_ccsl_decrypt(nvidia_stack_t *, struct ccslContext_t *, NvU32, NvU8 const *, NvU8 const *, NvU32, NvU8 *, NvU8 const *, NvU32, NvU8 const *);
NV_STATUS  NV_API_CALL rm_gpu_ops_ccsl_sign(nvidia_stack_t *, struct ccslContext_t *, NvU32, NvU8 const *, NvU8 *);
NV_STATUS  NV_API_CALL rm_gpu_ops_ccsl_query_message_pool(nvidia_stack_t *, struct ccslContext_t *, NvU8, NvU64 *);
NV_STATUS  NV_API_CALL rm_gpu_ops_ccsl_increment_iv(nvidia_stack_t *, struct ccslContext_t *, NvU8, NvU64, NvU8 *);
NV_STATUS  NV_API_CALL rm_gpu_ops_ccsl_log_encryption(nvidia_stack_t *, struct ccslContext_t *, NvU8, NvU32);

#endif
