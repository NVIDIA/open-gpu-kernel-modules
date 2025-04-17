/*
 * SPDX-FileCopyrightText: Copyright (c) 2016-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// No include guards - this file is included multiple times, each time with a
// different definition for RS_ENTRY
//
// Some of those definitions of RS_ENTRY may depend on declarations in various
// other header files. Include "resource_list_required_includes.h" to pull them
// in.
//

//
// Table describing all RsResource subclasses.
//
// Internal Class - there is a RM internal class representing all classes
// exported to RM clients. The internal name of the class should be similar to
// the symbolic name used by clients. If there is ambiguity between RM internal
// classes, e.g.: between the PMU engine (OBJPMU) and the exported class, it's
// recommended to use Api as the suffix to disambiguate; for example, OBJPMU
// (the engine) vs PmuApi (the per-client api object). It's also recommended to
// avoid using Object, Resource, etc as those terms don't improve clarity.
// If there is no ambiguity, there is no need to add the Api suffix; for example,
// Channel is preferred over ChannelApi (there is no other Channel object in
// RM).
//
// Multi-Instance - NV_TRUE if there can be multiple instances of this object's
// *internal* class id under a parent.
//
// This list should eventually replace the similar lists in nvapi.c and
// rmctrl.c. The number of fields in the table should be kept minimal, just
// enough to create the object, with as much of the detail being specified
// within the class itself.
//
// In the future we should consider switching to a registration approach or
// generating with NVOC and/or annotating the class definition.
//
// RS-TODO: Rename classes that have 'Object' in their names
//



RS_ENTRY(
    /* External Class         */ NV01_ROOT,
    /* Internal Class         */ RmClientResource,
    /* Multi-Instance         */ NV_FALSE,
    /* Parents                */ RS_ROOT_OBJECT,
    /* Alloc Param Info       */ RS_OPTIONAL(NvHandle),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ACQUIRE_GPUS_LOCK_ON_ALLOC | RS_FLAGS_ACQUIRE_GPUS_LOCK_ON_DUP,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NV01_ROOT_NON_PRIV,
    /* Internal Class         */ RmClientResource,
    /* Multi-Instance         */ NV_FALSE,
    /* Parents                */ RS_ROOT_OBJECT,
    /* Alloc Param Info       */ RS_OPTIONAL(NvHandle),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ACQUIRE_GPUS_LOCK_ON_ALLOC | RS_FLAGS_ACQUIRE_GPUS_LOCK_ON_DUP,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NV01_ROOT_CLIENT,
    /* Internal Class         */ RmClientResource,
    /* Multi-Instance         */ NV_FALSE,
    /* Parents                */ RS_ROOT_OBJECT,
    /* Alloc Param Info       */ RS_OPTIONAL(NvHandle),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ACQUIRE_GPUS_LOCK_ON_ALLOC | RS_FLAGS_ACQUIRE_GPUS_LOCK_ON_DUP,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ MPS_COMPUTE,
    /* Internal Class         */ MpsApi,
    /* Multi-Instance         */ NV_FALSE,
    /* Parents                */ RS_LIST(classId(RmClientResource)),
    /* Alloc Param Info       */ RS_NONE,
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ FABRIC_MANAGER_SESSION,
    /* Internal Class         */ FmSessionApi,
    /* Multi-Instance         */ NV_FALSE,
    /* Parents                */ RS_LIST(classId(RmClientResource)),
    /* Alloc Param Info       */ RS_REQUIRED(NV000F_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NV_IMEX_SESSION,
    /* Internal Class         */ ImexSessionApi,
    /* Multi-Instance         */ NV_FALSE,
    /* Parents                */ RS_LIST(classId(RmClientResource)),
    /* Alloc Param Info       */ RS_REQUIRED(NV00F1_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_HIGH,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NV0020_GPU_MANAGEMENT,
    /* Internal Class         */ GpuManagementApi,
    /* Multi-Instance         */ NV_FALSE,
    /* Parents                */ RS_LIST(classId(RmClientResource)),
    /* Alloc Param Info       */ RS_OPTIONAL(NvHandle),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NV_EVENT_BUFFER,
    /* Internal Class         */ EventBuffer,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(RmClientResource)),
    /* Alloc Param Info       */ RS_REQUIRED(NV_EVENT_BUFFER_ALLOC_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NV_SEMAPHORE_SURFACE,
    /* Internal Class         */ SemaphoreSurface,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(Subdevice)),
    /* Alloc Param Info       */ RS_REQUIRED(NV_SEMAPHORE_SURFACE_ALLOC_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NV50_P2P,
    /* Internal Class         */ P2PApi,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(RmClientResource)),
    /* Alloc Param Info       */ RS_REQUIRED(NV503B_ALLOC_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NV0060_SYNC_GPU_BOOST,
    /* Internal Class         */ SyncGpuBoost,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(RmClientResource)),
    /* Alloc Param Info       */ RS_REQUIRED(NV0060_ALLOC_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NV01_DEVICE_0,
    /* Internal Class         */ Device,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(RmClientResource)),
    /* Alloc Param Info       */ RS_OPTIONAL(NV0080_ALLOC_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK | RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ GT200_DEBUGGER,
    /* Internal Class         */ KernelSMDebuggerSession,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(Device)),
    /* Alloc Param Info       */ RS_REQUIRED(NV83DE_ALLOC_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK | RS_FLAGS_ALLOC_RPC_TO_ALL | RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC | RS_FLAGS_DUAL_CLIENT_LOCK,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NV30_GSYNC,
    /* Internal Class         */ GSyncApi,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(RmClientResource)),
    /* Alloc Param Info       */ RS_REQUIRED(NV30F1_ALLOC_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ GF100_PROFILER,
    /* Internal Class         */ Profiler,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(Subdevice), classId(KernelChannel), classId(KernelChannelGroupApi)),
    /* Alloc Param Info       */ RS_NONE,
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK | RS_FLAGS_ALLOC_RPC_TO_PHYS_RM,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ MAXWELL_PROFILER_CONTEXT,
    /* Internal Class         */ ProfilerCtx,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(KernelChannel), classId(KernelChannelGroupApi)),
    /* Alloc Param Info       */ RS_REQUIRED(NVB1CC_ALLOC_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ MAXWELL_PROFILER_DEVICE,
    /* Internal Class         */ ProfilerDev,
    /* Multi-Instance         */ NV_FALSE,
    /* Parents                */ RS_LIST(classId(Subdevice)),
    /* Alloc Param Info       */ RS_REQUIRED(NVB2CC_ALLOC_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ G84_PERFBUFFER,
    /* Internal Class         */ PerfBuffer,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(Device)),
    /* Alloc Param Info       */ RS_NONE,
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ KEPLER_DEVICE_VGPU,
    /* Internal Class         */ VgpuApi,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(Device)),
    /* Alloc Param Info       */ RS_NONE,
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ GF100_HDACODEC,
    /* Internal Class         */ Hdacodec,
    /* Multi-Instance         */ NV_FALSE,
    /* Parents                */ RS_LIST(classId(Device)),
    /* Alloc Param Info       */ RS_NONE,
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK_ON_FREE | RS_FLAGS_ALLOC_RPC_TO_ALL,
    /* Required Access Rights */ RS_ACCESS_NONE
)
    /* Channels can have a CHANNEL_GROUP, a DEVICE, or a CONTEXT_SHARE (starting in Volta) as parents */
    /* RS-TODO: Update channel parent list when CONTEXT_SHARE is added */
RS_ENTRY(
    /* External Class         */ GF100_CHANNEL_GPFIFO,
    /* Internal Class         */ KernelChannel,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(Device), classId(KernelChannelGroupApi)),
    /* Alloc Param Info       */ RS_REQUIRED(NV_CHANNEL_ALLOC_PARAMS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_HIGH,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK_ON_FREE | RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ KEPLER_CHANNEL_GPFIFO_A,
    /* Internal Class         */ KernelChannel,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(Device), classId(KernelChannelGroupApi)),
    /* Alloc Param Info       */ RS_REQUIRED(NV_CHANNEL_ALLOC_PARAMS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_HIGH,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK_ON_FREE | RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ KEPLER_CHANNEL_GPFIFO_B,
    /* Internal Class         */ KernelChannel,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(Device), classId(KernelChannelGroupApi)),
    /* Alloc Param Info       */ RS_REQUIRED(NV_CHANNEL_ALLOC_PARAMS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_HIGH,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK_ON_FREE | RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ MAXWELL_CHANNEL_GPFIFO_A,
    /* Internal Class         */ KernelChannel,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(Device), classId(KernelChannelGroupApi)),
    /* Alloc Param Info       */ RS_REQUIRED(NV_CHANNEL_ALLOC_PARAMS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_HIGH,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK_ON_FREE | RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ PASCAL_CHANNEL_GPFIFO_A,
    /* Internal Class         */ KernelChannel,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(Device), classId(KernelChannelGroupApi)),
    /* Alloc Param Info       */ RS_REQUIRED(NV_CHANNEL_ALLOC_PARAMS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_HIGH,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK_ON_FREE | RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ VOLTA_CHANNEL_GPFIFO_A,
    /* Internal Class         */ KernelChannel,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(Device), classId(KernelChannelGroupApi)),
    /* Alloc Param Info       */ RS_REQUIRED(NV_CHANNEL_ALLOC_PARAMS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_HIGH,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK_ON_FREE | RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ TURING_CHANNEL_GPFIFO_A,
    /* Internal Class         */ KernelChannel,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(Device), classId(KernelChannelGroupApi)),
    /* Alloc Param Info       */ RS_REQUIRED(NV_CHANNEL_ALLOC_PARAMS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_HIGH,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK_ON_FREE | RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ AMPERE_CHANNEL_GPFIFO_A,
    /* Internal Class         */ KernelChannel,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(Device), classId(KernelChannelGroupApi)),
    /* Alloc Param Info       */ RS_REQUIRED(NV_CHANNEL_ALLOC_PARAMS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_HIGH,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK_ON_FREE | RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ HOPPER_CHANNEL_GPFIFO_A,
    /* Internal Class         */ KernelChannel,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(Device), classId(KernelChannelGroupApi)),
    /* Alloc Param Info       */ RS_REQUIRED(NV_CHANNEL_ALLOC_PARAMS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_HIGH,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK_ON_FREE | RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ BLACKWELL_CHANNEL_GPFIFO_A,
    /* Internal Class         */ KernelChannel,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(Device), classId(KernelChannelGroupApi)),
    /* Alloc Param Info       */ RS_REQUIRED(NV_CHANNEL_ALLOC_PARAMS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_HIGH,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK_ON_FREE | RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ BLACKWELL_CHANNEL_GPFIFO_B,
    /* Internal Class         */ KernelChannel,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(Device), classId(KernelChannelGroupApi)),
    /* Alloc Param Info       */ RS_REQUIRED(NV_CHANNEL_ALLOC_PARAMS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_HIGH,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK_ON_FREE | RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ UVM_CHANNEL_RETAINER,
    /* Internal Class         */ UvmChannelRetainer,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(Device), classId(KernelChannelGroupApi)),
    /* Alloc Param Info       */ RS_REQUIRED(NV_UVM_CHANNEL_RETAINER_ALLOC_PARAMS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_HIGH,
    /* Flags                  */ RS_FLAGS_ALLOC_KERNEL_PRIVILEGED | RS_FLAGS_ALLOC_ALL_VGPU_PLUGINS | RS_FLAGS_ACQUIRE_GPU_GROUP_LOCK | RS_FLAGS_ALLOC_RPC_TO_ALL | RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC | RS_FLAGS_DUAL_CLIENT_LOCK,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ FERMI_CONTEXT_SHARE_A,
    /* Internal Class         */ KernelCtxShareApi,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(KernelChannelGroupApi)),
    /* Alloc Param Info       */ RS_REQUIRED(NV_CTXSHARE_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPU_GROUP_LOCK | RS_FLAGS_ALLOC_RPC_TO_ALL,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ KERNEL_GRAPHICS_CONTEXT,
    /* Internal Class         */ KernelGraphicsContext,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(KernelChannelGroupApi), classId(KernelChannel)),
    /* Alloc Param Info       */ RS_NONE,
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPU_GROUP_LOCK | RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC | RS_FLAGS_INTERNAL_ONLY,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NV20_SUBDEVICE_0,
    /* Internal Class         */ Subdevice,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(Device)),
    /* Alloc Param Info       */ RS_OPTIONAL(NV2080_ALLOC_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK | RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NV2081_BINAPI,
    /* Internal Class         */ BinaryApi,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(Subdevice)),
    /* Alloc Param Info       */ RS_OPTIONAL(NV2081_ALLOC_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK |
                                 RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC | RS_FLAGS_ALLOC_RPC_TO_PHYS_RM |
                                 RS_FLAGS_ALLOC_RPC_TO_VGPU_HOST,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NV2082_BINAPI_PRIVILEGED,
    /* Internal Class         */ BinaryApiPrivileged,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(Subdevice)),
    /* Alloc Param Info       */ RS_OPTIONAL(NV2082_ALLOC_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK |
                                 RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC | RS_FLAGS_ALLOC_RPC_TO_PHYS_RM |
                                 RS_FLAGS_ALLOC_RPC_TO_VGPU_HOST | RS_FLAGS_ALLOC_GSP_PLUGIN_FOR_VGPU_GSP,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ KEPLER_CHANNEL_GROUP_A,
    /* Internal Class         */ KernelChannelGroupApi,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(Device)),
    /* Alloc Param Info       */ RS_REQUIRED(NV_CHANNEL_GROUP_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_HIGH,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK_ON_FREE | RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NV01_MEMORY_LOCAL_PRIVILEGED,
    /* Internal Class         */ RegisterMemory,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(Device)),
    /* Alloc Param Info       */ RS_NONE,
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NV01_MEMORY_LOCAL_USER,
    /* Internal Class         */ VideoMemory,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(Device), classId(Subdevice)),
    /* Alloc Param Info       */ RS_REQUIRED(NV_MEMORY_ALLOCATION_PARAMS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK_ON_FREE | RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NV_MEMORY_EXTENDED_USER,
    /* Internal Class         */ ExtendedGpuMemory,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(Device), classId(Subdevice)),
    /* Alloc Param Info       */ RS_REQUIRED(NV_MEMORY_ALLOCATION_PARAMS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK_ON_FREE | RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NV01_MEMORY_LOCAL_PHYSICAL,
    /* Internal Class         */ PhysicalMemory,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(Device)),
    /* Alloc Param Info       */ RS_REQUIRED(NV_PHYSICAL_MEMORY_ALLOCATION_PARAMS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_PRIVILEGED | RS_FLAGS_ALLOC_CPU_PLUGIN_FOR_SRIOV | RS_FLAGS_ACQUIRE_GPUS_LOCK,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NV50_MEMORY_VIRTUAL,
    /* Internal Class         */ VirtualMemory,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(Device)),
    /* Alloc Param Info       */ RS_REQUIRED(NV_MEMORY_ALLOCATION_PARAMS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK_ON_FREE | RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NV01_MEMORY_SYSTEM,
    /* Internal Class         */ SystemMemory,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(Device)),
    /* Alloc Param Info       */ RS_REQUIRED(NV_MEMORY_ALLOCATION_PARAMS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK | RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NV01_MEMORY_VIRTUAL,
    /* Internal Class         */ VirtualMemoryRange,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(Device)),
    /* Alloc Param Info       */ RS_REQUIRED(NV_MEMORY_VIRTUAL_ALLOCATION_PARAMS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NV_MEMORY_MAPPER,
    /* Internal Class         */ MemoryMapper,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(Subdevice)),
    /* Alloc Param Info       */ RS_REQUIRED(NV_MEMORY_MAPPER_ALLOCATION_PARAMS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_HIGH,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NV01_MEMORY_SYSTEM_OS_DESCRIPTOR,
    /* Internal Class         */ OsDescMemory,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(Device)),
    /* Alloc Param Info       */ RS_REQUIRED(NV_OS_DESC_MEMORY_ALLOCATION_PARAMS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPU_GROUP_LOCK_ON_ALLOC | RS_FLAGS_ACQUIRE_GPUS_LOCK_ON_DUP | RS_FLAGS_ACQUIRE_GPU_GROUP_LOCK_ON_FREE | RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC | RS_FLAGS_FORCE_ACQUIRE_RO_API_LOCK_ON_ALLOC_FREE,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NV01_MEMORY_SYNCPOINT,
    /* Internal Class         */ SyncpointMemory,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(Device)),
    /* Alloc Param Info       */ RS_REQUIRED(NV_MEMORY_SYNCPOINT_ALLOCATION_PARAMS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPU_GROUP_LOCK,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NV01_MEMORY_DEVICELESS,
    /* Internal Class         */ NoDeviceMemory,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(RmClientResource)),
    /* Alloc Param Info       */ RS_REQUIRED(NV_MEMORY_ALLOCATION_PARAMS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NV01_MEMORY_FRAMEBUFFER_CONSOLE,
    /* Internal Class         */ ConsoleMemory,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(Device)),
    /* Alloc Param Info       */ RS_NONE,
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_KERNEL_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NV01_MEMORY_HW_RESOURCES,
    /* Internal Class         */ MemoryHwResources,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(Device), classId(Subdevice)),
    /* Alloc Param Info       */ RS_REQUIRED(NV_MEMORY_HW_RESOURCES_ALLOCATION_PARAMS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPU_GROUP_LOCK,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NV01_MEMORY_LIST_SYSTEM,
    /* Internal Class         */ MemoryList,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(Device), classId(Subdevice)),
    /* Alloc Param Info       */ RS_REQUIRED(NV_MEMORY_LIST_ALLOCATION_PARAMS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NV01_MEMORY_LIST_FBMEM,
    /* Internal Class         */ MemoryList,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(Device), classId(Subdevice)),
    /* Alloc Param Info       */ RS_REQUIRED(NV_MEMORY_LIST_ALLOCATION_PARAMS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_PRIVILEGED | RS_FLAGS_ALLOC_CPU_PLUGIN_FOR_SRIOV | RS_FLAGS_ACQUIRE_GPUS_LOCK,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NV01_MEMORY_LIST_OBJECT,
    /* Internal Class         */ MemoryList,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(Device), classId(Subdevice)),
    /* Alloc Param Info       */ RS_REQUIRED(NV_MEMORY_LIST_ALLOCATION_PARAMS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_PRIVILEGED | RS_FLAGS_ALLOC_CPU_PLUGIN_FOR_SRIOV | RS_FLAGS_ACQUIRE_GPUS_LOCK,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NV01_MEMORY_FLA,
    /* Internal Class         */ FlaMemory,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(Device)),
    /* Alloc Param Info       */ RS_REQUIRED(NV_FLA_MEMORY_ALLOCATION_PARAMS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NV_MEMORY_EXPORT,
    /* Internal Class         */ MemoryExport,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(RmClientResource)),
    /* Alloc Param Info       */ RS_REQUIRED(NV00E0_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NV_MEMORY_FABRIC_IMPORT_V2,
    /* Internal Class         */ MemoryFabricImportV2,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(RmClientResource)),
    /* Alloc Param Info       */ RS_REQUIRED(NV00F9_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NV_MEMORY_FABRIC,
    /* Internal Class         */ MemoryFabric,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(Subdevice)),
    /* Alloc Param Info       */ RS_REQUIRED(NV00F8_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK | RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NV_MEMORY_FABRIC_IMPORTED_REF,
    /* Internal Class         */ MemoryFabricImportedRef,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(RmClientResource)),
    /* Alloc Param Info       */ RS_REQUIRED(NV00FB_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ FERMI_VASPACE_A,
    /* Internal Class         */ VaSpaceApi,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(Device), classId(Subdevice)),
    /* Alloc Param Info       */ RS_REQUIRED(NV_VASPACE_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK_ON_FREE | RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NV_MEMORY_MULTICAST_FABRIC,
    /* Internal Class         */ MemoryMulticastFabric,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(RmClientResource)),
    /* Alloc Param Info       */ RS_REQUIRED(NV00FD_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK_ON_FREE | RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC,
    /* Required Access Rights */ RS_ACCESS_NONE
)
    /* Subdevice Children: */
RS_ENTRY(
    /* External Class         */ NVENC_SW_SESSION,
    /* Internal Class         */ NvencSession,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(Subdevice)),
    /* Alloc Param Info       */ RS_REQUIRED(NVA0BC_ALLOC_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NVFBC_SW_SESSION,
    /* Internal Class         */ NvfbcSession,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(Subdevice)),
    /* Alloc Param Info       */ RS_REQUIRED(NVA0BD_ALLOC_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NVA081_VGPU_CONFIG,
    /* Internal Class         */ VgpuConfigApi,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(Subdevice)),
    /* Alloc Param Info       */ RS_NONE,
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NVA084_KERNEL_HOST_VGPU_DEVICE,
    /* Internal Class         */ KernelHostVgpuDeviceApi,
    /* Multi-Instance         */ NV_FALSE,
    /* Parents                */ RS_LIST(classId(Subdevice)),
    /* Alloc Param Info       */ RS_REQUIRED(NVA084_ALLOC_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NV50_THIRD_PARTY_P2P,
    /* Internal Class         */ ThirdPartyP2P,
    /* Multi-Instance         */ NV_FALSE,
    /* Parents                */ RS_LIST(classId(Subdevice)),
    /* Alloc Param Info       */ RS_OPTIONAL(NV503C_ALLOC_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK | RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ GF100_SUBDEVICE_MASTER,
    /* Internal Class         */ GenericEngineApi,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(Subdevice)),
    /* Alloc Param Info       */ RS_NONE,
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_CHANNEL_DESCENDANT_COMMON | RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ GF100_SUBDEVICE_INFOROM,
    /* Internal Class         */ GenericEngineApi,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(Subdevice)),
    /* Alloc Param Info       */ RS_NONE,
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK | RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC | RS_FLAGS_ALLOC_RPC_TO_PHYS_RM,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NV01_TIMER,
    /* Internal Class         */ TimerApi,
    /* Multi-Instance         */ NV_FALSE,
    /* Parents                */ RS_LIST(classId(Subdevice)),
    /* Alloc Param Info       */ RS_NONE,
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ALLOC_RPC_TO_PHYS_RM | RS_FLAGS_ACQUIRE_GPUS_LOCK,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NV40_I2C,
    /* Internal Class         */ I2cApi,
    /* Multi-Instance         */ NV_FALSE,
    /* Parents                */ RS_LIST(classId(Subdevice)),
    /* Alloc Param Info       */ RS_NONE,
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK | RS_FLAGS_ALLOC_RPC_TO_PHYS_RM,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NV20_SUBDEVICE_DIAG,
    /* Internal Class         */ DiagApi,
    /* Multi-Instance         */ NV_FALSE,
    /* Parents                */ RS_LIST(classId(Subdevice)),
    /* Alloc Param Info       */ RS_NONE,
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK | RS_FLAGS_ALLOC_RPC_TO_ALL,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ GF100_ZBC_CLEAR,
    /* Internal Class         */ ZbcApi,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(Subdevice)),
    /* Alloc Param Info       */ RS_NONE,
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK | RS_FLAGS_ALLOC_RPC_TO_ALL,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NV40_DEBUG_BUFFER,
    /* Internal Class         */ DebugBufferApi,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(Subdevice)),
    /* Alloc Param Info       */ RS_REQUIRED(NV00DB_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ RM_USER_SHARED_DATA,
    /* Internal Class         */ GpuUserSharedData,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(Subdevice)),
    /* Alloc Param Info       */ RS_REQUIRED(NV00DE_ALLOC_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPU_GROUP_LOCK,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ VOLTA_USERMODE_A,
    /* Internal Class         */ UserModeApi,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(Subdevice)),
    /* Alloc Param Info       */ RS_NONE,
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ TURING_USERMODE_A,
    /* Internal Class         */ UserModeApi,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(Subdevice)),
    /* Alloc Param Info       */ RS_NONE,
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ AMPERE_USERMODE_A,
    /* Internal Class         */ UserModeApi,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(Subdevice)),
    /* Alloc Param Info       */ RS_NONE,
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ HOPPER_USERMODE_A,
    /* Internal Class         */ UserModeApi,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(Subdevice)),
    /* Alloc Param Info       */ RS_OPTIONAL(NV_HOPPER_USERMODE_A_PARAMS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ BLACKWELL_USERMODE_A,
    /* Internal Class         */ UserModeApi,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(Subdevice)),
    /* Alloc Param Info       */ RS_OPTIONAL(NV_HOPPER_USERMODE_A_PARAMS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NVC371_DISP_SF_USER,
    /* Internal Class         */ DispSfUser,
    /* Multi-Instance         */ NV_FALSE,
    /* Parents                */ RS_LIST(classId(Subdevice)),
    /* Alloc Param Info       */ RS_NONE,
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NVC671_DISP_SF_USER,
    /* Internal Class         */ DispSfUser,
    /* Multi-Instance         */ NV_FALSE,
    /* Parents                */ RS_LIST(classId(Subdevice)),
    /* Alloc Param Info       */ RS_NONE,
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NVC771_DISP_SF_USER,
    /* Internal Class         */ DispSfUser,
    /* Multi-Instance         */ NV_FALSE,
    /* Parents                */ RS_LIST(classId(Subdevice)),
    /* Alloc Param Info       */ RS_NONE,
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NVC971_DISP_SF_USER,
    /* Internal Class         */ DispSfUser,
    /* Multi-Instance         */ NV_FALSE,
    /* Parents                */ RS_LIST(classId(Subdevice)),
    /* Alloc Param Info       */ RS_NONE,
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NVCA71_DISP_SF_USER,
    /* Internal Class         */ DispSfUser,
    /* Multi-Instance         */ NV_FALSE,
    /* Parents                */ RS_LIST(classId(Subdevice)),
    /* Alloc Param Info       */ RS_NONE,
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NVCB71_DISP_SF_USER,
    /* Internal Class         */ DispSfUser,
    /* Multi-Instance         */ NV_FALSE,
    /* Parents                */ RS_LIST(classId(Subdevice)),
    /* Alloc Param Info       */ RS_NONE,
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ MMU_FAULT_BUFFER,
    /* Internal Class         */ MmuFaultBuffer,
    /* Multi-Instance         */ NV_FALSE,
    /* Parents                */ RS_LIST(classId(Subdevice)),
    /* Alloc Param Info       */ RS_NONE,
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_KERNEL_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK | RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ ACCESS_COUNTER_NOTIFY_BUFFER,
    /* Internal Class         */ AccessCounterBuffer,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(Subdevice)),
    /* Alloc Param Info       */ RS_REQUIRED(NV_ACCESS_COUNTER_NOTIFY_BUFFER_ALLOC_PARAMS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ MMU_VIDMEM_ACCESS_BIT_BUFFER,
    /* Internal Class         */ VidmemAccessBitBuffer,
    /* Multi-Instance         */ NV_FALSE,
    /* Parents                */ RS_LIST(classId(Subdevice)),
    /* Alloc Param Info       */ RS_REQUIRED(NV_VIDMEM_ACCESS_BIT_ALLOCATION_PARAMS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK | RS_FLAGS_ALLOC_RPC_TO_PHYS_RM,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ AMPERE_SMC_PARTITION_REF,
    /* Internal Class         */ GPUInstanceSubscription,
    /* Multi-Instance         */ NV_FALSE,
    /* Parents                */ RS_LIST(classId(Subdevice)),
    /* Alloc Param Info       */ RS_REQUIRED(NVC637_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK | RS_FLAGS_ALLOC_RPC_TO_ALL,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ AMPERE_SMC_EXEC_PARTITION_REF,
    /* Internal Class         */ ComputeInstanceSubscription,
    /* Multi-Instance         */ NV_FALSE,
    /* Parents                */ RS_LIST(classId(GPUInstanceSubscription)),
    /* Alloc Param Info       */ RS_REQUIRED(NVC638_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK | RS_FLAGS_ALLOC_RPC_TO_ALL,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ AMPERE_SMC_CONFIG_SESSION,
    /* Internal Class         */ MIGConfigSession,
    /* Multi-Instance         */ NV_FALSE,
    /* Parents                */ RS_LIST(classId(RmClientResource)),
    /* Alloc Param Info       */ RS_REQUIRED(NVC639_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ AMPERE_SMC_MONITOR_SESSION,
    /* Internal Class         */ MIGMonitorSession,
    /* Multi-Instance         */ NV_FALSE,
    /* Parents                */ RS_LIST(classId(RmClientResource)),
    /* Alloc Param Info       */ RS_REQUIRED(NVC640_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED,
    /* Required Access Rights */ RS_ACCESS_NONE
)
    /* Display classes: */
RS_ENTRY(
    /* External Class         */ NVC570_DISPLAY,
    /* Internal Class         */ NvDispApi,
    /* Multi-Instance         */ NV_FALSE,
    /* Parents                */ RS_LIST(classId(Device)),
    /* Alloc Param Info       */ RS_NONE,
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK | RS_FLAGS_ALLOC_RPC_TO_ALL,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NVC670_DISPLAY,
    /* Internal Class         */ NvDispApi,
    /* Multi-Instance         */ NV_FALSE,
    /* Parents                */ RS_LIST(classId(Device)),
    /* Alloc Param Info       */ RS_NONE,
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK | RS_FLAGS_ALLOC_RPC_TO_ALL,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NVC770_DISPLAY,
    /* Internal Class         */ NvDispApi,
    /* Multi-Instance         */ NV_FALSE,
    /* Parents                */ RS_LIST(classId(Device)),
    /* Alloc Param Info       */ RS_NONE,
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK | RS_FLAGS_ALLOC_RPC_TO_ALL,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NVC970_DISPLAY,
    /* Internal Class         */ NvDispApi,
    /* Multi-Instance         */ NV_FALSE,
    /* Parents                */ RS_LIST(classId(Device)),
    /* Alloc Param Info       */ RS_NONE,
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK | RS_FLAGS_ALLOC_RPC_TO_ALL,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NVCA70_DISPLAY,
    /* Internal Class         */ NvDispApi,
    /* Multi-Instance         */ NV_FALSE,
    /* Parents                */ RS_LIST(classId(Device)),
    /* Alloc Param Info       */ RS_NONE,
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK | RS_FLAGS_ALLOC_RPC_TO_ALL,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NVCB70_DISPLAY,
    /* Internal Class         */ NvDispApi,
    /* Multi-Instance         */ NV_FALSE,
    /* Parents                */ RS_LIST(classId(Device)),
    /* Alloc Param Info       */ RS_NONE,
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK | RS_FLAGS_ALLOC_RPC_TO_ALL,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NVC372_DISPLAY_SW,
    /* Internal Class         */ DispSwObj,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(Device)),
    /* Alloc Param Info       */ RS_NONE,
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_PRIVILEGED | RS_FLAGS_ALLOC_RPC_TO_ALL,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NV04_DISPLAY_COMMON,
    /* Internal Class         */ DispCommon,
    /* Multi-Instance         */ NV_FALSE,
    /* Parents                */ RS_LIST(classId(Device)),
    /* Alloc Param Info       */ RS_NONE,
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK_ON_FREE | RS_FLAGS_ALLOC_RPC_TO_ALL,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NV9010_VBLANK_CALLBACK,
    /* Internal Class         */ VblankCallback,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(Device), classId(Subdevice)),
    /* Alloc Param Info       */ RS_REQUIRED(NV_VBLANK_CALLBACK_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_KERNEL_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NV0092_RG_LINE_CALLBACK,
    /* Internal Class         */ RgLineCallback,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(DispCommon)),
    /* Alloc Param Info       */ RS_REQUIRED(NV0092_RG_LINE_CALLBACK_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_KERNEL_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NVC57A_CURSOR_IMM_CHANNEL_PIO,
    /* Internal Class         */ DispChannelPio,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(NvDispApi)),
    /* Alloc Param Info       */ RS_REQUIRED(NV50VAIO_CHANNELPIO_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK | RS_FLAGS_ALLOC_RPC_TO_ALL,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NVC67A_CURSOR_IMM_CHANNEL_PIO,
    /* Internal Class         */ DispChannelPio,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(NvDispApi)),
    /* Alloc Param Info       */ RS_REQUIRED(NV50VAIO_CHANNELPIO_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK | RS_FLAGS_ALLOC_RPC_TO_ALL,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NVC97A_CURSOR_IMM_CHANNEL_PIO,
    /* Internal Class         */ DispChannelPio,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(NvDispApi)),
    /* Alloc Param Info       */ RS_REQUIRED(NV50VAIO_CHANNELPIO_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK | RS_FLAGS_ALLOC_RPC_TO_ALL,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NVCA7A_CURSOR_IMM_CHANNEL_PIO,
    /* Internal Class         */ DispChannelPio,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(NvDispApi)),
    /* Alloc Param Info       */ RS_REQUIRED(NV50VAIO_CHANNELPIO_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK | RS_FLAGS_ALLOC_RPC_TO_ALL,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NVCB7A_CURSOR_IMM_CHANNEL_PIO,
    /* Internal Class         */ DispChannelPio,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(NvDispApi)),
    /* Alloc Param Info       */ RS_REQUIRED(NV50VAIO_CHANNELPIO_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK | RS_FLAGS_ALLOC_RPC_TO_ALL,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NVC57B_WINDOW_IMM_CHANNEL_DMA,
    /* Internal Class         */ DispChannelDma,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(NvDispApi)),
    /* Alloc Param Info       */ RS_REQUIRED(NV50VAIO_CHANNELDMA_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK | RS_FLAGS_ALLOC_RPC_TO_ALL,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NVC57D_CORE_CHANNEL_DMA,
    /* Internal Class         */ DispChannelDma,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(NvDispApi)),
    /* Alloc Param Info       */ RS_REQUIRED(NV50VAIO_CHANNELDMA_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK | RS_FLAGS_ALLOC_RPC_TO_ALL,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NVC57E_WINDOW_CHANNEL_DMA,
    /* Internal Class         */ DispChannelDma,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(NvDispApi)),
    /* Alloc Param Info       */ RS_REQUIRED(NV50VAIO_CHANNELDMA_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK | RS_FLAGS_ALLOC_RPC_TO_ALL,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NVC573_DISP_CAPABILITIES,
    /* Internal Class         */ DispCapabilities,
    /* Multi-Instance         */ NV_FALSE,
    /* Parents                */ RS_LIST(classId(NvDispApi)),
    /* Alloc Param Info       */ RS_NONE,
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NVC67B_WINDOW_IMM_CHANNEL_DMA,
    /* Internal Class         */ DispChannelDma,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(NvDispApi)),
    /* Alloc Param Info       */ RS_REQUIRED(NV50VAIO_CHANNELDMA_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK | RS_FLAGS_ALLOC_RPC_TO_ALL,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NVC67D_CORE_CHANNEL_DMA,
    /* Internal Class         */ DispChannelDma,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(NvDispApi)),
    /* Alloc Param Info       */ RS_REQUIRED(NV50VAIO_CHANNELDMA_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK | RS_FLAGS_ALLOC_RPC_TO_ALL,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NVC77D_CORE_CHANNEL_DMA,
    /* Internal Class         */ DispChannelDma,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(NvDispApi)),
    /* Alloc Param Info       */ RS_REQUIRED(NV50VAIO_CHANNELDMA_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK | RS_FLAGS_ALLOC_RPC_TO_ALL,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NVC77F_ANY_CHANNEL_DMA,
    /* Internal Class         */ DispChannelDma,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(NvDispApi)),
    /* Alloc Param Info       */ RS_REQUIRED(NV50VAIO_CHANNELDMA_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NVC67E_WINDOW_CHANNEL_DMA,
    /* Internal Class         */ DispChannelDma,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(NvDispApi)),
    /* Alloc Param Info       */ RS_REQUIRED(NV50VAIO_CHANNELDMA_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK | RS_FLAGS_ALLOC_RPC_TO_ALL,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NVC673_DISP_CAPABILITIES,
    /* Internal Class         */ DispCapabilities,
    /* Multi-Instance         */ NV_FALSE,
    /* Parents                */ RS_LIST(classId(NvDispApi)),
    /* Alloc Param Info       */ RS_NONE,
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NVC773_DISP_CAPABILITIES,
    /* Internal Class         */ DispCapabilities,
    /* Multi-Instance         */ NV_FALSE,
    /* Parents                */ RS_LIST(classId(NvDispApi)),
    /* Alloc Param Info       */ RS_NONE,
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NVC97B_WINDOW_IMM_CHANNEL_DMA,
    /* Internal Class         */ DispChannelDma,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(NvDispApi)),
    /* Alloc Param Info       */ RS_REQUIRED(NV50VAIO_CHANNELDMA_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK | RS_FLAGS_ALLOC_RPC_TO_ALL,
    /* Required Access Right  */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NVC97D_CORE_CHANNEL_DMA,
    /* Internal Class         */ DispChannelDma,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(NvDispApi)),
    /* Alloc Param Info       */ RS_REQUIRED(NV50VAIO_CHANNELDMA_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK | RS_FLAGS_ALLOC_RPC_TO_ALL,
    /* Required Access Right  */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NVC97E_WINDOW_CHANNEL_DMA,
    /* Internal Class         */ DispChannelDma,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(NvDispApi)),
    /* Alloc Param Info       */ RS_REQUIRED(NV50VAIO_CHANNELDMA_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK | RS_FLAGS_ALLOC_RPC_TO_ALL,
    /* Required Access Right  */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NVC973_DISP_CAPABILITIES,
    /* Internal Class         */ DispCapabilities,
    /* Multi-Instance         */ NV_FALSE,
    /* Parents                */ RS_LIST(classId(NvDispApi)),
    /* Alloc Param Info       */ RS_NONE,
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NVCA73_DISP_CAPABILITIES,
    /* Internal Class         */ DispCapabilities,
    /* Multi-Instance         */ NV_FALSE,
    /* Parents                */ RS_LIST(classId(NvDispApi)),
    /* Alloc Param Info       */ RS_NONE,
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NVCA7B_WINDOW_IMM_CHANNEL_DMA,
    /* Internal Class         */ DispChannelDma,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(NvDispApi)),
    /* Alloc Param Info       */ RS_REQUIRED(NV50VAIO_CHANNELDMA_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK | RS_FLAGS_ALLOC_RPC_TO_ALL,
    /* Required Access Right  */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NVCA7D_CORE_CHANNEL_DMA,
    /* Internal Class         */ DispChannelDma,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(NvDispApi)),
    /* Alloc Param Info       */ RS_REQUIRED(NV50VAIO_CHANNELDMA_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK | RS_FLAGS_ALLOC_RPC_TO_ALL,
    /* Required Access Right  */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NVCA7E_WINDOW_CHANNEL_DMA,
    /* Internal Class         */ DispChannelDma,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(NvDispApi)),
    /* Alloc Param Info       */ RS_REQUIRED(NV50VAIO_CHANNELDMA_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK | RS_FLAGS_ALLOC_RPC_TO_ALL,
    /* Required Access Right  */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NVCB73_DISP_CAPABILITIES,
    /* Internal Class         */ DispCapabilities,
    /* Multi-Instance         */ NV_FALSE,
    /* Parents                */ RS_LIST(classId(NvDispApi)),
    /* Alloc Param Info       */ RS_NONE,
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NVCB7B_WINDOW_IMM_CHANNEL_DMA,
    /* Internal Class         */ DispChannelDma,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(NvDispApi)),
    /* Alloc Param Info       */ RS_REQUIRED(NV50VAIO_CHANNELDMA_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK | RS_FLAGS_ALLOC_RPC_TO_ALL,
    /* Required Access Right  */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NVCB7D_CORE_CHANNEL_DMA,
    /* Internal Class         */ DispChannelDma,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(NvDispApi)),
    /* Alloc Param Info       */ RS_REQUIRED(NV50VAIO_CHANNELDMA_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK | RS_FLAGS_ALLOC_RPC_TO_ALL,
    /* Required Access Right  */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NVCB7E_WINDOW_CHANNEL_DMA,
    /* Internal Class         */ DispChannelDma,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(NvDispApi)),
    /* Alloc Param Info       */ RS_REQUIRED(NV50VAIO_CHANNELDMA_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK | RS_FLAGS_ALLOC_RPC_TO_ALL,
    /* Required Access Right  */ RS_ACCESS_NONE
)
    /* Classes allocated under channel: */
RS_ENTRY(
    /* External Class         */ GF100_DISP_SW,
    /* Internal Class         */ DispSwObject,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(KernelChannel)),
    /* Alloc Param Info       */ RS_REQUIRED(NV9072_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPU_GROUP_LOCK | RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC | RS_FLAGS_ALLOC_RPC_TO_ALL,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ GF100_TIMED_SEMAPHORE_SW,
    /* Internal Class         */ TimedSemaSwObject,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(KernelChannel)),
    /* Alloc Param Info       */ RS_NONE,
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_CHANNEL_DESCENDANT_COMMON,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NV50_DEFERRED_API_CLASS,
    /* Internal Class         */ DeferredApiObject,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(KernelChannel)),
    /* Alloc Param Info       */ RS_OPTIONAL(NV5080_ALLOC_PARAMS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_CHANNEL_DESCENDANT_COMMON,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ GP100_UVM_SW,
    /* Internal Class         */ UvmSwObject,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(KernelChannel)),
    /* Alloc Param Info       */ RS_NONE,
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_PRIVILEGED | RS_FLAGS_ALLOC_CPU_PLUGIN_FOR_SRIOV | RS_FLAGS_ALLOC_GSP_PLUGIN_FOR_VGPU_GSP | RS_FLAGS_CHANNEL_DESCENDANT_COMMON,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NV04_SOFTWARE_TEST,
    /* Internal Class         */ SoftwareMethodTest,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(KernelChannel)),
    /* Alloc Param Info       */ RS_NONE,
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_CHANNEL_DESCENDANT_COMMON,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ MAXWELL_DMA_COPY_A,
    /* Internal Class         */ KernelCeContext,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(KernelChannel)),
    /* Alloc Param Info       */ RS_OPTIONAL(NVB0B5_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_CHANNEL_DESCENDANT_COMMON | RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ PASCAL_DMA_COPY_A,
    /* Internal Class         */ KernelCeContext,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(KernelChannel)),
    /* Alloc Param Info       */ RS_OPTIONAL(NVB0B5_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_CHANNEL_DESCENDANT_COMMON | RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ TURING_DMA_COPY_A,
    /* Internal Class         */ KernelCeContext,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(KernelChannel)),
    /* Alloc Param Info       */ RS_OPTIONAL(NVB0B5_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_CHANNEL_DESCENDANT_COMMON | RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ AMPERE_DMA_COPY_A,
    /* Internal Class         */ KernelCeContext,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(KernelChannel)),
    /* Alloc Param Info       */ RS_OPTIONAL(NVB0B5_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_CHANNEL_DESCENDANT_COMMON | RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ AMPERE_DMA_COPY_B,
    /* Internal Class         */ KernelCeContext,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(KernelChannel)),
    /* Alloc Param Info       */ RS_OPTIONAL(NVB0B5_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_CHANNEL_DESCENDANT_COMMON | RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ HOPPER_DMA_COPY_A,
    /* Internal Class         */ KernelCeContext,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(KernelChannel)),
    /* Alloc Param Info       */ RS_OPTIONAL(NVB0B5_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_CHANNEL_DESCENDANT_COMMON | RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ BLACKWELL_DMA_COPY_A,
    /* Internal Class         */ KernelCeContext,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(KernelChannel)),
    /* Alloc Param Info       */ RS_OPTIONAL(NVB0B5_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_CHANNEL_DESCENDANT_COMMON | RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ BLACKWELL_DMA_COPY_B,
    /* Internal Class         */ KernelCeContext,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(KernelChannel)),
    /* Alloc Param Info       */ RS_OPTIONAL(NVB0B5_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_CHANNEL_DESCENDANT_COMMON | RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NVB8B0_VIDEO_DECODER,
    /* Internal Class         */ NvdecContext,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(KernelChannel)),
    /* Alloc Param Info       */ RS_OPTIONAL(NV_BSP_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_CHANNEL_DESCENDANT_COMMON,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NVC4B0_VIDEO_DECODER,
    /* Internal Class         */ NvdecContext,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(KernelChannel)),
    /* Alloc Param Info       */ RS_OPTIONAL(NV_BSP_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_CHANNEL_DESCENDANT_COMMON,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NVC6B0_VIDEO_DECODER,
    /* Internal Class         */ NvdecContext,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(KernelChannel)),
    /* Alloc Param Info       */ RS_OPTIONAL(NV_BSP_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_CHANNEL_DESCENDANT_COMMON,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NVC7B0_VIDEO_DECODER,
    /* Internal Class         */ NvdecContext,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(KernelChannel)),
    /* Alloc Param Info       */ RS_OPTIONAL(NV_BSP_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_CHANNEL_DESCENDANT_COMMON,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NVC9B0_VIDEO_DECODER,
    /* Internal Class         */ NvdecContext,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(KernelChannel)),
    /* Alloc Param Info       */ RS_OPTIONAL(NV_BSP_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_CHANNEL_DESCENDANT_COMMON,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NVCDB0_VIDEO_DECODER,
    /* Internal Class         */ NvdecContext,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(KernelChannel)),
    /* Alloc Param Info       */ RS_OPTIONAL(NV_BSP_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_CHANNEL_DESCENDANT_COMMON,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NVCFB0_VIDEO_DECODER,
    /* Internal Class         */ NvdecContext,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(KernelChannel)),
    /* Alloc Param Info       */ RS_OPTIONAL(NV_BSP_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_CHANNEL_DESCENDANT_COMMON,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NVB8D1_VIDEO_NVJPG,
    /* Internal Class         */ NvjpgContext,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(KernelChannel)),
    /* Alloc Param Info       */ RS_OPTIONAL(NV_NVJPG_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_CHANNEL_DESCENDANT_COMMON,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NVC4D1_VIDEO_NVJPG,
    /* Internal Class         */ NvjpgContext,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(KernelChannel)),
    /* Alloc Param Info       */ RS_OPTIONAL(NV_NVJPG_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_CHANNEL_DESCENDANT_COMMON,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NVC9D1_VIDEO_NVJPG,
    /* Internal Class         */ NvjpgContext,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(KernelChannel)),
    /* Alloc Param Info       */ RS_OPTIONAL(NV_NVJPG_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_CHANNEL_DESCENDANT_COMMON,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NVCDD1_VIDEO_NVJPG,
    /* Internal Class         */ NvjpgContext,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(KernelChannel)),
    /* Alloc Param Info       */ RS_OPTIONAL(NV_NVJPG_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_CHANNEL_DESCENDANT_COMMON,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NVCFD1_VIDEO_NVJPG,
    /* Internal Class         */ NvjpgContext,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(KernelChannel)),
    /* Alloc Param Info       */ RS_OPTIONAL(NV_NVJPG_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_CHANNEL_DESCENDANT_COMMON,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NVB8FA_VIDEO_OFA,
    /* Internal Class         */ OfaContext,
    /* Multi-Instance         */ NV_FALSE,
    /* Parents                */ RS_LIST(classId(KernelChannel)),
    /* Alloc Param Info       */ RS_OPTIONAL(NV_OFA_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_CHANNEL_DESCENDANT_COMMON,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NVC6FA_VIDEO_OFA,
    /* Internal Class         */ OfaContext,
    /* Multi-Instance         */ NV_FALSE,
    /* Parents                */ RS_LIST(classId(KernelChannel)),
    /* Alloc Param Info       */ RS_OPTIONAL(NV_OFA_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_CHANNEL_DESCENDANT_COMMON,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NVC7FA_VIDEO_OFA,
    /* Internal Class         */ OfaContext,
    /* Multi-Instance         */ NV_FALSE,
    /* Parents                */ RS_LIST(classId(KernelChannel)),
    /* Alloc Param Info       */ RS_OPTIONAL(NV_OFA_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_CHANNEL_DESCENDANT_COMMON,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NVC9FA_VIDEO_OFA,
    /* Internal Class         */ OfaContext,
    /* Multi-Instance         */ NV_FALSE,
    /* Parents                */ RS_LIST(classId(KernelChannel)),
    /* Alloc Param Info       */ RS_OPTIONAL(NV_OFA_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_CHANNEL_DESCENDANT_COMMON,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NVCDFA_VIDEO_OFA,
    /* Internal Class         */ OfaContext,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(KernelChannel)),
    /* Alloc Param Info       */ RS_OPTIONAL(NV_OFA_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_CHANNEL_DESCENDANT_COMMON,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NVCFFA_VIDEO_OFA,
    /* Internal Class         */ OfaContext,
    /* Multi-Instance         */ NV_FALSE,
    /* Parents                */ RS_LIST(classId(KernelChannel)),
    /* Alloc Param Info       */ RS_OPTIONAL(NV_OFA_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_CHANNEL_DESCENDANT_COMMON,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NVC4B7_VIDEO_ENCODER,
    /* Internal Class         */ MsencContext,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(KernelChannel)),
    /* Alloc Param Info       */ RS_OPTIONAL(NV_MSENC_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_CHANNEL_DESCENDANT_COMMON,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NVB4B7_VIDEO_ENCODER,
    /* Internal Class         */ MsencContext,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(KernelChannel)),
    /* Alloc Param Info       */ RS_OPTIONAL(NV_MSENC_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_CHANNEL_DESCENDANT_COMMON,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NVC7B7_VIDEO_ENCODER,
    /* Internal Class         */ MsencContext,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(KernelChannel)),
    /* Alloc Param Info       */ RS_OPTIONAL(NV_MSENC_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_CHANNEL_DESCENDANT_COMMON,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NVC9B7_VIDEO_ENCODER,
    /* Internal Class         */ MsencContext,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(KernelChannel)),
    /* Alloc Param Info       */ RS_OPTIONAL(NV_MSENC_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_CHANNEL_DESCENDANT_COMMON,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NVCFB7_VIDEO_ENCODER,
    /* Internal Class         */ MsencContext,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(KernelChannel)),
    /* Alloc Param Info       */ RS_OPTIONAL(NV_MSENC_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_CHANNEL_DESCENDANT_COMMON,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ HOPPER_SEC2_WORK_LAUNCH_A,
    /* Internal Class         */ Sec2Context,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(KernelChannel)),
    /* Alloc Param Info       */ RS_NONE,
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_CHANNEL_DESCENDANT_COMMON,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ AMPERE_A,
    /* Internal Class         */ KernelGraphicsObject,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(KernelChannel)),
    /* Alloc Param Info       */ RS_OPTIONAL(NV_GR_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_CHANNEL_DESCENDANT_COMMON | RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ AMPERE_COMPUTE_A,
    /* Internal Class         */ KernelGraphicsObject,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(KernelChannel)),
    /* Alloc Param Info       */ RS_OPTIONAL(NV_GR_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_CHANNEL_DESCENDANT_COMMON | RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ AMPERE_B,
    /* Internal Class         */ KernelGraphicsObject,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(KernelChannel)),
    /* Alloc Param Info       */ RS_OPTIONAL(NV_GR_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_CHANNEL_DESCENDANT_COMMON | RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ AMPERE_COMPUTE_B,
    /* Internal Class         */ KernelGraphicsObject,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(KernelChannel)),
    /* Alloc Param Info       */ RS_OPTIONAL(NV_GR_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_CHANNEL_DESCENDANT_COMMON | RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ ADA_A,
    /* Internal Class         */ KernelGraphicsObject,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(KernelChannel)),
    /* Alloc Param Info       */ RS_OPTIONAL(NV_GR_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_CHANNEL_DESCENDANT_COMMON | RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ ADA_COMPUTE_A,
    /* Internal Class         */ KernelGraphicsObject,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(KernelChannel)),
    /* Alloc Param Info       */ RS_OPTIONAL(NV_GR_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_CHANNEL_DESCENDANT_COMMON | RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ HOPPER_A,
    /* Internal Class         */ KernelGraphicsObject,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(KernelChannel)),
    /* Alloc Param Info       */ RS_OPTIONAL(NV_GR_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_CHANNEL_DESCENDANT_COMMON | RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ HOPPER_COMPUTE_A,
    /* Internal Class         */ KernelGraphicsObject,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(KernelChannel)),
    /* Alloc Param Info       */ RS_OPTIONAL(NV_GR_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_CHANNEL_DESCENDANT_COMMON | RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ BLACKWELL_A,
    /* Internal Class         */ KernelGraphicsObject,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(KernelChannel)),
    /* Alloc Param Info       */ RS_OPTIONAL(NV_GR_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_CHANNEL_DESCENDANT_COMMON | RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ BLACKWELL_COMPUTE_A,
    /* Internal Class         */ KernelGraphicsObject,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(KernelChannel)),
    /* Alloc Param Info       */ RS_OPTIONAL(NV_GR_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_CHANNEL_DESCENDANT_COMMON | RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ BLACKWELL_INLINE_TO_MEMORY_A,
    /* Internal Class         */ KernelGraphicsObject,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(KernelChannel)),
    /* Alloc Param Info       */ RS_OPTIONAL(NV_GR_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_CHANNEL_DESCENDANT_COMMON | RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ BLACKWELL_B,
    /* Internal Class         */ KernelGraphicsObject,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(KernelChannel)),
    /* Alloc Param Info       */ RS_OPTIONAL(NV_GR_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_CHANNEL_DESCENDANT_COMMON | RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ BLACKWELL_COMPUTE_B,
    /* Internal Class         */ KernelGraphicsObject,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(KernelChannel)),
    /* Alloc Param Info       */ RS_OPTIONAL(NV_GR_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_CHANNEL_DESCENDANT_COMMON | RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ FERMI_TWOD_A,
    /* Internal Class         */ KernelGraphicsObject,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(KernelChannel)),
    /* Alloc Param Info       */ RS_OPTIONAL(NV_GR_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_CHANNEL_DESCENDANT_COMMON | RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ KEPLER_INLINE_TO_MEMORY_B,
    /* Internal Class         */ KernelGraphicsObject,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(KernelChannel)),
    /* Alloc Param Info       */ RS_OPTIONAL(NV_GR_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_CHANNEL_DESCENDANT_COMMON | RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ TURING_A,
    /* Internal Class         */ KernelGraphicsObject,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(KernelChannel)),
    /* Alloc Param Info       */ RS_OPTIONAL(NV_GR_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_CHANNEL_DESCENDANT_COMMON | RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ TURING_COMPUTE_A,
    /* Internal Class         */ KernelGraphicsObject,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(KernelChannel)),
    /* Alloc Param Info       */ RS_OPTIONAL(NV_GR_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_CHANNEL_DESCENDANT_COMMON | RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NV01_CONTEXT_DMA,
    /* Internal Class         */ ContextDma,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_ANY_PARENT,
    /* Alloc Param Info       */ RS_REQUIRED(NV_CONTEXT_DMA_ALLOCATION_PARAMS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NV01_EVENT,
    /* Internal Class         */ Event,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_ANY_PARENT,
    /* Alloc Param Info       */ RS_REQUIRED(NV0005_ALLOC_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK | RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC | RS_FLAGS_DUAL_CLIENT_LOCK,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NV01_EVENT_OS_EVENT,
    /* Internal Class         */ Event,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_ANY_PARENT,
    /* Alloc Param Info       */ RS_REQUIRED(NV0005_ALLOC_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK | RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC | RS_FLAGS_DUAL_CLIENT_LOCK,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NV01_EVENT_KERNEL_CALLBACK,
    /* Internal Class         */ Event,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_ANY_PARENT,
    /* Alloc Param Info       */ RS_REQUIRED(NV0005_ALLOC_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK | RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC | RS_FLAGS_DUAL_CLIENT_LOCK,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NV01_EVENT_KERNEL_CALLBACK_EX,
    /* Internal Class         */ Event,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_ANY_PARENT,
    /* Alloc Param Info       */ RS_REQUIRED(NV0005_ALLOC_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK | RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC | RS_FLAGS_DUAL_CLIENT_LOCK,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NV_CONFIDENTIAL_COMPUTE,
    /* Internal Class         */ ConfidentialComputeApi,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(RmClientResource)),
    /* Alloc Param Info       */ RS_OPTIONAL(NV_CONFIDENTIAL_COMPUTE_ALLOC_PARAMS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK,
    /* Required Access Rights */ RS_ACCESS_NONE
)
RS_ENTRY(
    /* External Class         */ NV_COUNTER_COLLECTION_UNIT,
    /* Internal Class         */ KernelCcuApi,
    /* Multi-Instance         */ NV_FALSE,
    /* Parents                */ RS_LIST(classId(Subdevice)),
    /* Alloc Param Info       */ RS_NONE,
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_GPUS_LOCK,
    /* Required Access Rights */ RS_ACCESS_NONE
)
#if RMCFG_CLASS_NV_CE_UTILS && (defined(DEBUG) || defined(DEVELOP))
RS_ENTRY(
    /* External Class         */ NV_CE_UTILS,
    /* Internal Class         */ CeUtilsApi,
    /* Multi-Instance         */ NV_TRUE,
    /* Parents                */ RS_LIST(classId(Subdevice)),
    /* Alloc Param Info       */ RS_REQUIRED(NV0050_ALLOCATION_PARAMETERS),
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ACQUIRE_GPUS_LOCK | RS_FLAGS_ALLOC_PRIVILEGED,
    /* Required Access Rights */ RS_ACCESS_NONE
)
#endif

RS_ENTRY(
    /* External Class         */ LOCK_STRESS_OBJECT,
    /* Internal Class         */ LockStressObject,
    /* Multi-Instance         */ NV_FALSE,
    /* Parents                */ RS_LIST(classId(Subdevice)),
    /* Alloc Param Info       */ RS_NONE,
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED,
    /* Required Access Rights */ RS_ACCESS_NONE
)

RS_ENTRY(
    /* External Class         */ LOCK_TEST_RELAXED_DUP_OBJECT,
    /* Internal Class         */ LockTestRelaxedDupObject,
    /* Multi-Instance         */ NV_FALSE,
    /* Parents                */ RS_LIST(classId(Subdevice), classId(Device)),
    /* Alloc Param Info       */ RS_NONE,
    /* Resource Free Priority */ RS_FREE_PRIORITY_DEFAULT,
    /* Flags                  */ RS_FLAGS_ALLOC_NON_PRIVILEGED | RS_FLAGS_ACQUIRE_RELAXED_GPUS_LOCK_ON_DUP | RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC | RS_FLAGS_ACQUIRE_GPU_GROUP_LOCK_ON_ALLOC | RS_FLAGS_ACQUIRE_GPU_GROUP_LOCK_ON_FREE,
    /* Required Access Rights */ RS_ACCESS_NONE
)

// Undefine the entry macro to simplify call sites
#undef RS_ENTRY
