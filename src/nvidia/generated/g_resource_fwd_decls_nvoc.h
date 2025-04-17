
#ifndef _G_RESOURCE_FWD_DECLS_NVOC_H_
#define _G_RESOURCE_FWD_DECLS_NVOC_H_

// Version of generated metadata structures
#ifdef NVOC_METADATA_VERSION
#undef NVOC_METADATA_VERSION
#endif
#define NVOC_METADATA_VERSION 2

#include "nvoc/runtime.h"
#include "nvoc/rtti.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#pragma once
#include "g_resource_fwd_decls_nvoc.h"

#ifndef RESOURCE_FWD_DECLS_H
#define RESOURCE_FWD_DECLS_H

#include "nvtypes.h"
#include "nvoc/prelude.h"
#include "nvoc/object.h"
#include "rmconfig.h"

// Base classes

struct ChannelDescendant;

#ifndef __NVOC_CLASS_ChannelDescendant_TYPEDEF__
#define __NVOC_CLASS_ChannelDescendant_TYPEDEF__
typedef struct ChannelDescendant ChannelDescendant;
#endif /* __NVOC_CLASS_ChannelDescendant_TYPEDEF__ */

#ifndef __nvoc_class_id_ChannelDescendant
#define __nvoc_class_id_ChannelDescendant 0x43d7c4
#endif /* __nvoc_class_id_ChannelDescendant */



struct DispChannel;

#ifndef __NVOC_CLASS_DispChannel_TYPEDEF__
#define __NVOC_CLASS_DispChannel_TYPEDEF__
typedef struct DispChannel DispChannel;
#endif /* __NVOC_CLASS_DispChannel_TYPEDEF__ */

#ifndef __nvoc_class_id_DispChannel
#define __nvoc_class_id_DispChannel 0xbd2ff3
#endif /* __nvoc_class_id_DispChannel */



struct GpuResource;

#ifndef __NVOC_CLASS_GpuResource_TYPEDEF__
#define __NVOC_CLASS_GpuResource_TYPEDEF__
typedef struct GpuResource GpuResource;
#endif /* __NVOC_CLASS_GpuResource_TYPEDEF__ */

#ifndef __nvoc_class_id_GpuResource
#define __nvoc_class_id_GpuResource 0x5d5d9f
#endif /* __nvoc_class_id_GpuResource */



struct INotifier;

#ifndef __NVOC_CLASS_INotifier_TYPEDEF__
#define __NVOC_CLASS_INotifier_TYPEDEF__
typedef struct INotifier INotifier;
#endif /* __NVOC_CLASS_INotifier_TYPEDEF__ */

#ifndef __nvoc_class_id_INotifier
#define __nvoc_class_id_INotifier 0xf8f965
#endif /* __nvoc_class_id_INotifier */



struct Memory;

#ifndef __NVOC_CLASS_Memory_TYPEDEF__
#define __NVOC_CLASS_Memory_TYPEDEF__
typedef struct Memory Memory;
#endif /* __NVOC_CLASS_Memory_TYPEDEF__ */

#ifndef __nvoc_class_id_Memory
#define __nvoc_class_id_Memory 0x4789f2
#endif /* __nvoc_class_id_Memory */



struct Notifier;

#ifndef __NVOC_CLASS_Notifier_TYPEDEF__
#define __NVOC_CLASS_Notifier_TYPEDEF__
typedef struct Notifier Notifier;
#endif /* __NVOC_CLASS_Notifier_TYPEDEF__ */

#ifndef __nvoc_class_id_Notifier
#define __nvoc_class_id_Notifier 0xa8683b
#endif /* __nvoc_class_id_Notifier */



struct NotifShare;

#ifndef __NVOC_CLASS_NotifShare_TYPEDEF__
#define __NVOC_CLASS_NotifShare_TYPEDEF__
typedef struct NotifShare NotifShare;
#endif /* __NVOC_CLASS_NotifShare_TYPEDEF__ */

#ifndef __nvoc_class_id_NotifShare
#define __nvoc_class_id_NotifShare 0xd5f150
#endif /* __nvoc_class_id_NotifShare */



struct Resource;

#ifndef __NVOC_CLASS_Resource_TYPEDEF__
#define __NVOC_CLASS_Resource_TYPEDEF__
typedef struct Resource Resource;
#endif /* __NVOC_CLASS_Resource_TYPEDEF__ */

#ifndef __nvoc_class_id_Resource
#define __nvoc_class_id_Resource 0xbe8545
#endif /* __nvoc_class_id_Resource */



struct RmResource;

#ifndef __NVOC_CLASS_RmResource_TYPEDEF__
#define __NVOC_CLASS_RmResource_TYPEDEF__
typedef struct RmResource RmResource;
#endif /* __NVOC_CLASS_RmResource_TYPEDEF__ */

#ifndef __nvoc_class_id_RmResource
#define __nvoc_class_id_RmResource 0x03610d
#endif /* __nvoc_class_id_RmResource */



struct RmResourceCommon;

#ifndef __NVOC_CLASS_RmResourceCommon_TYPEDEF__
#define __NVOC_CLASS_RmResourceCommon_TYPEDEF__
typedef struct RmResourceCommon RmResourceCommon;
#endif /* __NVOC_CLASS_RmResourceCommon_TYPEDEF__ */

#ifndef __nvoc_class_id_RmResourceCommon
#define __nvoc_class_id_RmResourceCommon 0x8ef259
#endif /* __nvoc_class_id_RmResourceCommon */



struct RsResource;

#ifndef __NVOC_CLASS_RsResource_TYPEDEF__
#define __NVOC_CLASS_RsResource_TYPEDEF__
typedef struct RsResource RsResource;
#endif /* __NVOC_CLASS_RsResource_TYPEDEF__ */

#ifndef __nvoc_class_id_RsResource
#define __nvoc_class_id_RsResource 0xd551cb
#endif /* __nvoc_class_id_RsResource */



struct RsShared;

#ifndef __NVOC_CLASS_RsShared_TYPEDEF__
#define __NVOC_CLASS_RsShared_TYPEDEF__
typedef struct RsShared RsShared;
#endif /* __NVOC_CLASS_RsShared_TYPEDEF__ */

#ifndef __nvoc_class_id_RsShared
#define __nvoc_class_id_RsShared 0x830542
#endif /* __nvoc_class_id_RsShared */



// Classes disabled in orin but required forward declarations to build.

struct HostVgpuDeviceApi;

#ifndef __NVOC_CLASS_HostVgpuDeviceApi_TYPEDEF__
#define __NVOC_CLASS_HostVgpuDeviceApi_TYPEDEF__
typedef struct HostVgpuDeviceApi HostVgpuDeviceApi;
#endif /* __NVOC_CLASS_HostVgpuDeviceApi_TYPEDEF__ */

#ifndef __nvoc_class_id_HostVgpuDeviceApi
#define __nvoc_class_id_HostVgpuDeviceApi 0x4c4173
#endif /* __nvoc_class_id_HostVgpuDeviceApi */

 // also used by open rm

struct MpsApi;

#ifndef __NVOC_CLASS_MpsApi_TYPEDEF__
#define __NVOC_CLASS_MpsApi_TYPEDEF__
typedef struct MpsApi MpsApi;
#endif /* __NVOC_CLASS_MpsApi_TYPEDEF__ */

#ifndef __nvoc_class_id_MpsApi
#define __nvoc_class_id_MpsApi 0x22ce42
#endif /* __nvoc_class_id_MpsApi */



struct MIGConfigSession;

#ifndef __NVOC_CLASS_MIGConfigSession_TYPEDEF__
#define __NVOC_CLASS_MIGConfigSession_TYPEDEF__
typedef struct MIGConfigSession MIGConfigSession;
#endif /* __NVOC_CLASS_MIGConfigSession_TYPEDEF__ */

#ifndef __nvoc_class_id_MIGConfigSession
#define __nvoc_class_id_MIGConfigSession 0x36a941
#endif /* __nvoc_class_id_MIGConfigSession */



struct FmSessionApi;

#ifndef __NVOC_CLASS_FmSessionApi_TYPEDEF__
#define __NVOC_CLASS_FmSessionApi_TYPEDEF__
typedef struct FmSessionApi FmSessionApi;
#endif /* __NVOC_CLASS_FmSessionApi_TYPEDEF__ */

#ifndef __nvoc_class_id_FmSessionApi
#define __nvoc_class_id_FmSessionApi 0xdfbd08
#endif /* __nvoc_class_id_FmSessionApi */



struct MIGMonitorSession;

#ifndef __NVOC_CLASS_MIGMonitorSession_TYPEDEF__
#define __NVOC_CLASS_MIGMonitorSession_TYPEDEF__
typedef struct MIGMonitorSession MIGMonitorSession;
#endif /* __NVOC_CLASS_MIGMonitorSession_TYPEDEF__ */

#ifndef __nvoc_class_id_MIGMonitorSession
#define __nvoc_class_id_MIGMonitorSession 0x29e15c
#endif /* __nvoc_class_id_MIGMonitorSession */



struct TimerApi;

#ifndef __NVOC_CLASS_TimerApi_TYPEDEF__
#define __NVOC_CLASS_TimerApi_TYPEDEF__
typedef struct TimerApi TimerApi;
#endif /* __NVOC_CLASS_TimerApi_TYPEDEF__ */

#ifndef __nvoc_class_id_TimerApi
#define __nvoc_class_id_TimerApi 0xb13ac4
#endif /* __nvoc_class_id_TimerApi */



struct KernelSMDebuggerSession;

#ifndef __NVOC_CLASS_KernelSMDebuggerSession_TYPEDEF__
#define __NVOC_CLASS_KernelSMDebuggerSession_TYPEDEF__
typedef struct KernelSMDebuggerSession KernelSMDebuggerSession;
#endif /* __NVOC_CLASS_KernelSMDebuggerSession_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelSMDebuggerSession
#define __nvoc_class_id_KernelSMDebuggerSession 0x4adc81
#endif /* __nvoc_class_id_KernelSMDebuggerSession */



// NVOC only expand macros inside a class. Use the stub class

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_RESOURCE_FWD_DECLS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI
struct NVOC_METADATA__NVOCFwdDeclHack;

struct RmClientResource;

#ifndef __NVOC_CLASS_RmClientResource_TYPEDEF__
#define __NVOC_CLASS_RmClientResource_TYPEDEF__
typedef struct RmClientResource RmClientResource;
#endif /* __NVOC_CLASS_RmClientResource_TYPEDEF__ */

#ifndef __nvoc_class_id_RmClientResource
#define __nvoc_class_id_RmClientResource 0x37a701
#endif /* __nvoc_class_id_RmClientResource */

struct ImexSessionApi;

#ifndef __NVOC_CLASS_ImexSessionApi_TYPEDEF__
#define __NVOC_CLASS_ImexSessionApi_TYPEDEF__
typedef struct ImexSessionApi ImexSessionApi;
#endif /* __NVOC_CLASS_ImexSessionApi_TYPEDEF__ */

#ifndef __nvoc_class_id_ImexSessionApi
#define __nvoc_class_id_ImexSessionApi 0xb4748b
#endif /* __nvoc_class_id_ImexSessionApi */

struct GpuManagementApi;

#ifndef __NVOC_CLASS_GpuManagementApi_TYPEDEF__
#define __NVOC_CLASS_GpuManagementApi_TYPEDEF__
typedef struct GpuManagementApi GpuManagementApi;
#endif /* __NVOC_CLASS_GpuManagementApi_TYPEDEF__ */

#ifndef __nvoc_class_id_GpuManagementApi
#define __nvoc_class_id_GpuManagementApi 0x376305
#endif /* __nvoc_class_id_GpuManagementApi */

struct EventBuffer;

#ifndef __NVOC_CLASS_EventBuffer_TYPEDEF__
#define __NVOC_CLASS_EventBuffer_TYPEDEF__
typedef struct EventBuffer EventBuffer;
#endif /* __NVOC_CLASS_EventBuffer_TYPEDEF__ */

#ifndef __nvoc_class_id_EventBuffer
#define __nvoc_class_id_EventBuffer 0x63502b
#endif /* __nvoc_class_id_EventBuffer */

struct SemaphoreSurface;

#ifndef __NVOC_CLASS_SemaphoreSurface_TYPEDEF__
#define __NVOC_CLASS_SemaphoreSurface_TYPEDEF__
typedef struct SemaphoreSurface SemaphoreSurface;
#endif /* __NVOC_CLASS_SemaphoreSurface_TYPEDEF__ */

#ifndef __nvoc_class_id_SemaphoreSurface
#define __nvoc_class_id_SemaphoreSurface 0xeabc69
#endif /* __nvoc_class_id_SemaphoreSurface */

struct P2PApi;

#ifndef __NVOC_CLASS_P2PApi_TYPEDEF__
#define __NVOC_CLASS_P2PApi_TYPEDEF__
typedef struct P2PApi P2PApi;
#endif /* __NVOC_CLASS_P2PApi_TYPEDEF__ */

#ifndef __nvoc_class_id_P2PApi
#define __nvoc_class_id_P2PApi 0x3982b7
#endif /* __nvoc_class_id_P2PApi */

struct SyncGpuBoost;

#ifndef __NVOC_CLASS_SyncGpuBoost_TYPEDEF__
#define __NVOC_CLASS_SyncGpuBoost_TYPEDEF__
typedef struct SyncGpuBoost SyncGpuBoost;
#endif /* __NVOC_CLASS_SyncGpuBoost_TYPEDEF__ */

#ifndef __nvoc_class_id_SyncGpuBoost
#define __nvoc_class_id_SyncGpuBoost 0xc7e30b
#endif /* __nvoc_class_id_SyncGpuBoost */

struct Device;

#ifndef __NVOC_CLASS_Device_TYPEDEF__
#define __NVOC_CLASS_Device_TYPEDEF__
typedef struct Device Device;
#endif /* __NVOC_CLASS_Device_TYPEDEF__ */

#ifndef __nvoc_class_id_Device
#define __nvoc_class_id_Device 0xe0ac20
#endif /* __nvoc_class_id_Device */

struct GSyncApi;

#ifndef __NVOC_CLASS_GSyncApi_TYPEDEF__
#define __NVOC_CLASS_GSyncApi_TYPEDEF__
typedef struct GSyncApi GSyncApi;
#endif /* __NVOC_CLASS_GSyncApi_TYPEDEF__ */

#ifndef __nvoc_class_id_GSyncApi
#define __nvoc_class_id_GSyncApi 0x214628
#endif /* __nvoc_class_id_GSyncApi */

struct Profiler;

#ifndef __NVOC_CLASS_Profiler_TYPEDEF__
#define __NVOC_CLASS_Profiler_TYPEDEF__
typedef struct Profiler Profiler;
#endif /* __NVOC_CLASS_Profiler_TYPEDEF__ */

#ifndef __nvoc_class_id_Profiler
#define __nvoc_class_id_Profiler 0x65b4c7
#endif /* __nvoc_class_id_Profiler */

struct ProfilerCtx;

#ifndef __NVOC_CLASS_ProfilerCtx_TYPEDEF__
#define __NVOC_CLASS_ProfilerCtx_TYPEDEF__
typedef struct ProfilerCtx ProfilerCtx;
#endif /* __NVOC_CLASS_ProfilerCtx_TYPEDEF__ */

#ifndef __nvoc_class_id_ProfilerCtx
#define __nvoc_class_id_ProfilerCtx 0xe99229
#endif /* __nvoc_class_id_ProfilerCtx */

struct ProfilerDev;

#ifndef __NVOC_CLASS_ProfilerDev_TYPEDEF__
#define __NVOC_CLASS_ProfilerDev_TYPEDEF__
typedef struct ProfilerDev ProfilerDev;
#endif /* __NVOC_CLASS_ProfilerDev_TYPEDEF__ */

#ifndef __nvoc_class_id_ProfilerDev
#define __nvoc_class_id_ProfilerDev 0x54d077
#endif /* __nvoc_class_id_ProfilerDev */

struct PerfBuffer;

#ifndef __NVOC_CLASS_PerfBuffer_TYPEDEF__
#define __NVOC_CLASS_PerfBuffer_TYPEDEF__
typedef struct PerfBuffer PerfBuffer;
#endif /* __NVOC_CLASS_PerfBuffer_TYPEDEF__ */

#ifndef __nvoc_class_id_PerfBuffer
#define __nvoc_class_id_PerfBuffer 0x4bc43b
#endif /* __nvoc_class_id_PerfBuffer */

struct VgpuApi;

#ifndef __NVOC_CLASS_VgpuApi_TYPEDEF__
#define __NVOC_CLASS_VgpuApi_TYPEDEF__
typedef struct VgpuApi VgpuApi;
#endif /* __NVOC_CLASS_VgpuApi_TYPEDEF__ */

#ifndef __nvoc_class_id_VgpuApi
#define __nvoc_class_id_VgpuApi 0x7774f5
#endif /* __nvoc_class_id_VgpuApi */

struct Hdacodec;

#ifndef __NVOC_CLASS_Hdacodec_TYPEDEF__
#define __NVOC_CLASS_Hdacodec_TYPEDEF__
typedef struct Hdacodec Hdacodec;
#endif /* __NVOC_CLASS_Hdacodec_TYPEDEF__ */

#ifndef __nvoc_class_id_Hdacodec
#define __nvoc_class_id_Hdacodec 0xf59a20
#endif /* __nvoc_class_id_Hdacodec */

struct KernelChannel;

#ifndef __NVOC_CLASS_KernelChannel_TYPEDEF__
#define __NVOC_CLASS_KernelChannel_TYPEDEF__
typedef struct KernelChannel KernelChannel;
#endif /* __NVOC_CLASS_KernelChannel_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelChannel
#define __nvoc_class_id_KernelChannel 0x5d8d70
#endif /* __nvoc_class_id_KernelChannel */

struct UvmChannelRetainer;

#ifndef __NVOC_CLASS_UvmChannelRetainer_TYPEDEF__
#define __NVOC_CLASS_UvmChannelRetainer_TYPEDEF__
typedef struct UvmChannelRetainer UvmChannelRetainer;
#endif /* __NVOC_CLASS_UvmChannelRetainer_TYPEDEF__ */

#ifndef __nvoc_class_id_UvmChannelRetainer
#define __nvoc_class_id_UvmChannelRetainer 0xa3f03a
#endif /* __nvoc_class_id_UvmChannelRetainer */

struct KernelCtxShareApi;

#ifndef __NVOC_CLASS_KernelCtxShareApi_TYPEDEF__
#define __NVOC_CLASS_KernelCtxShareApi_TYPEDEF__
typedef struct KernelCtxShareApi KernelCtxShareApi;
#endif /* __NVOC_CLASS_KernelCtxShareApi_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelCtxShareApi
#define __nvoc_class_id_KernelCtxShareApi 0x1f9af1
#endif /* __nvoc_class_id_KernelCtxShareApi */

struct KernelGraphicsContext;

#ifndef __NVOC_CLASS_KernelGraphicsContext_TYPEDEF__
#define __NVOC_CLASS_KernelGraphicsContext_TYPEDEF__
typedef struct KernelGraphicsContext KernelGraphicsContext;
#endif /* __NVOC_CLASS_KernelGraphicsContext_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelGraphicsContext
#define __nvoc_class_id_KernelGraphicsContext 0x7ead09
#endif /* __nvoc_class_id_KernelGraphicsContext */

struct Subdevice;

#ifndef __NVOC_CLASS_Subdevice_TYPEDEF__
#define __NVOC_CLASS_Subdevice_TYPEDEF__
typedef struct Subdevice Subdevice;
#endif /* __NVOC_CLASS_Subdevice_TYPEDEF__ */

#ifndef __nvoc_class_id_Subdevice
#define __nvoc_class_id_Subdevice 0x4b01b3
#endif /* __nvoc_class_id_Subdevice */

struct BinaryApi;

#ifndef __NVOC_CLASS_BinaryApi_TYPEDEF__
#define __NVOC_CLASS_BinaryApi_TYPEDEF__
typedef struct BinaryApi BinaryApi;
#endif /* __NVOC_CLASS_BinaryApi_TYPEDEF__ */

#ifndef __nvoc_class_id_BinaryApi
#define __nvoc_class_id_BinaryApi 0xb7a47c
#endif /* __nvoc_class_id_BinaryApi */

struct BinaryApiPrivileged;

#ifndef __NVOC_CLASS_BinaryApiPrivileged_TYPEDEF__
#define __NVOC_CLASS_BinaryApiPrivileged_TYPEDEF__
typedef struct BinaryApiPrivileged BinaryApiPrivileged;
#endif /* __NVOC_CLASS_BinaryApiPrivileged_TYPEDEF__ */

#ifndef __nvoc_class_id_BinaryApiPrivileged
#define __nvoc_class_id_BinaryApiPrivileged 0x1c0579
#endif /* __nvoc_class_id_BinaryApiPrivileged */

struct KernelChannelGroupApi;

#ifndef __NVOC_CLASS_KernelChannelGroupApi_TYPEDEF__
#define __NVOC_CLASS_KernelChannelGroupApi_TYPEDEF__
typedef struct KernelChannelGroupApi KernelChannelGroupApi;
#endif /* __NVOC_CLASS_KernelChannelGroupApi_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelChannelGroupApi
#define __nvoc_class_id_KernelChannelGroupApi 0x2b5b80
#endif /* __nvoc_class_id_KernelChannelGroupApi */

struct RegisterMemory;

#ifndef __NVOC_CLASS_RegisterMemory_TYPEDEF__
#define __NVOC_CLASS_RegisterMemory_TYPEDEF__
typedef struct RegisterMemory RegisterMemory;
#endif /* __NVOC_CLASS_RegisterMemory_TYPEDEF__ */

#ifndef __nvoc_class_id_RegisterMemory
#define __nvoc_class_id_RegisterMemory 0x40d457
#endif /* __nvoc_class_id_RegisterMemory */

struct VideoMemory;

#ifndef __NVOC_CLASS_VideoMemory_TYPEDEF__
#define __NVOC_CLASS_VideoMemory_TYPEDEF__
typedef struct VideoMemory VideoMemory;
#endif /* __NVOC_CLASS_VideoMemory_TYPEDEF__ */

#ifndef __nvoc_class_id_VideoMemory
#define __nvoc_class_id_VideoMemory 0xed948f
#endif /* __nvoc_class_id_VideoMemory */

struct ExtendedGpuMemory;

#ifndef __NVOC_CLASS_ExtendedGpuMemory_TYPEDEF__
#define __NVOC_CLASS_ExtendedGpuMemory_TYPEDEF__
typedef struct ExtendedGpuMemory ExtendedGpuMemory;
#endif /* __NVOC_CLASS_ExtendedGpuMemory_TYPEDEF__ */

#ifndef __nvoc_class_id_ExtendedGpuMemory
#define __nvoc_class_id_ExtendedGpuMemory 0xeffa5c
#endif /* __nvoc_class_id_ExtendedGpuMemory */

struct PhysicalMemory;

#ifndef __NVOC_CLASS_PhysicalMemory_TYPEDEF__
#define __NVOC_CLASS_PhysicalMemory_TYPEDEF__
typedef struct PhysicalMemory PhysicalMemory;
#endif /* __NVOC_CLASS_PhysicalMemory_TYPEDEF__ */

#ifndef __nvoc_class_id_PhysicalMemory
#define __nvoc_class_id_PhysicalMemory 0x5fccf2
#endif /* __nvoc_class_id_PhysicalMemory */

struct VirtualMemory;

#ifndef __NVOC_CLASS_VirtualMemory_TYPEDEF__
#define __NVOC_CLASS_VirtualMemory_TYPEDEF__
typedef struct VirtualMemory VirtualMemory;
#endif /* __NVOC_CLASS_VirtualMemory_TYPEDEF__ */

#ifndef __nvoc_class_id_VirtualMemory
#define __nvoc_class_id_VirtualMemory 0x2aea5c
#endif /* __nvoc_class_id_VirtualMemory */

struct SystemMemory;

#ifndef __NVOC_CLASS_SystemMemory_TYPEDEF__
#define __NVOC_CLASS_SystemMemory_TYPEDEF__
typedef struct SystemMemory SystemMemory;
#endif /* __NVOC_CLASS_SystemMemory_TYPEDEF__ */

#ifndef __nvoc_class_id_SystemMemory
#define __nvoc_class_id_SystemMemory 0x007a98
#endif /* __nvoc_class_id_SystemMemory */

struct VirtualMemoryRange;

#ifndef __NVOC_CLASS_VirtualMemoryRange_TYPEDEF__
#define __NVOC_CLASS_VirtualMemoryRange_TYPEDEF__
typedef struct VirtualMemoryRange VirtualMemoryRange;
#endif /* __NVOC_CLASS_VirtualMemoryRange_TYPEDEF__ */

#ifndef __nvoc_class_id_VirtualMemoryRange
#define __nvoc_class_id_VirtualMemoryRange 0x7032c6
#endif /* __nvoc_class_id_VirtualMemoryRange */

struct MemoryMapper;

#ifndef __NVOC_CLASS_MemoryMapper_TYPEDEF__
#define __NVOC_CLASS_MemoryMapper_TYPEDEF__
typedef struct MemoryMapper MemoryMapper;
#endif /* __NVOC_CLASS_MemoryMapper_TYPEDEF__ */

#ifndef __nvoc_class_id_MemoryMapper
#define __nvoc_class_id_MemoryMapper 0xb8e4a2
#endif /* __nvoc_class_id_MemoryMapper */

struct OsDescMemory;

#ifndef __NVOC_CLASS_OsDescMemory_TYPEDEF__
#define __NVOC_CLASS_OsDescMemory_TYPEDEF__
typedef struct OsDescMemory OsDescMemory;
#endif /* __NVOC_CLASS_OsDescMemory_TYPEDEF__ */

#ifndef __nvoc_class_id_OsDescMemory
#define __nvoc_class_id_OsDescMemory 0xb3dacd
#endif /* __nvoc_class_id_OsDescMemory */

struct SyncpointMemory;

#ifndef __NVOC_CLASS_SyncpointMemory_TYPEDEF__
#define __NVOC_CLASS_SyncpointMemory_TYPEDEF__
typedef struct SyncpointMemory SyncpointMemory;
#endif /* __NVOC_CLASS_SyncpointMemory_TYPEDEF__ */

#ifndef __nvoc_class_id_SyncpointMemory
#define __nvoc_class_id_SyncpointMemory 0x529def
#endif /* __nvoc_class_id_SyncpointMemory */

struct NoDeviceMemory;

#ifndef __NVOC_CLASS_NoDeviceMemory_TYPEDEF__
#define __NVOC_CLASS_NoDeviceMemory_TYPEDEF__
typedef struct NoDeviceMemory NoDeviceMemory;
#endif /* __NVOC_CLASS_NoDeviceMemory_TYPEDEF__ */

#ifndef __nvoc_class_id_NoDeviceMemory
#define __nvoc_class_id_NoDeviceMemory 0x6c0832
#endif /* __nvoc_class_id_NoDeviceMemory */

struct ConsoleMemory;

#ifndef __NVOC_CLASS_ConsoleMemory_TYPEDEF__
#define __NVOC_CLASS_ConsoleMemory_TYPEDEF__
typedef struct ConsoleMemory ConsoleMemory;
#endif /* __NVOC_CLASS_ConsoleMemory_TYPEDEF__ */

#ifndef __nvoc_class_id_ConsoleMemory
#define __nvoc_class_id_ConsoleMemory 0xaac69e
#endif /* __nvoc_class_id_ConsoleMemory */

struct MemoryHwResources;

#ifndef __NVOC_CLASS_MemoryHwResources_TYPEDEF__
#define __NVOC_CLASS_MemoryHwResources_TYPEDEF__
typedef struct MemoryHwResources MemoryHwResources;
#endif /* __NVOC_CLASS_MemoryHwResources_TYPEDEF__ */

#ifndef __nvoc_class_id_MemoryHwResources
#define __nvoc_class_id_MemoryHwResources 0x9a2a71
#endif /* __nvoc_class_id_MemoryHwResources */

struct MemoryList;

#ifndef __NVOC_CLASS_MemoryList_TYPEDEF__
#define __NVOC_CLASS_MemoryList_TYPEDEF__
typedef struct MemoryList MemoryList;
#endif /* __NVOC_CLASS_MemoryList_TYPEDEF__ */

#ifndef __nvoc_class_id_MemoryList
#define __nvoc_class_id_MemoryList 0x298f78
#endif /* __nvoc_class_id_MemoryList */

struct FlaMemory;

#ifndef __NVOC_CLASS_FlaMemory_TYPEDEF__
#define __NVOC_CLASS_FlaMemory_TYPEDEF__
typedef struct FlaMemory FlaMemory;
#endif /* __NVOC_CLASS_FlaMemory_TYPEDEF__ */

#ifndef __nvoc_class_id_FlaMemory
#define __nvoc_class_id_FlaMemory 0xe61ee1
#endif /* __nvoc_class_id_FlaMemory */

struct MemoryExport;

#ifndef __NVOC_CLASS_MemoryExport_TYPEDEF__
#define __NVOC_CLASS_MemoryExport_TYPEDEF__
typedef struct MemoryExport MemoryExport;
#endif /* __NVOC_CLASS_MemoryExport_TYPEDEF__ */

#ifndef __nvoc_class_id_MemoryExport
#define __nvoc_class_id_MemoryExport 0xe7ac53
#endif /* __nvoc_class_id_MemoryExport */

struct MemoryFabricImportV2;

#ifndef __NVOC_CLASS_MemoryFabricImportV2_TYPEDEF__
#define __NVOC_CLASS_MemoryFabricImportV2_TYPEDEF__
typedef struct MemoryFabricImportV2 MemoryFabricImportV2;
#endif /* __NVOC_CLASS_MemoryFabricImportV2_TYPEDEF__ */

#ifndef __nvoc_class_id_MemoryFabricImportV2
#define __nvoc_class_id_MemoryFabricImportV2 0xf96871
#endif /* __nvoc_class_id_MemoryFabricImportV2 */

struct MemoryFabric;

#ifndef __NVOC_CLASS_MemoryFabric_TYPEDEF__
#define __NVOC_CLASS_MemoryFabric_TYPEDEF__
typedef struct MemoryFabric MemoryFabric;
#endif /* __NVOC_CLASS_MemoryFabric_TYPEDEF__ */

#ifndef __nvoc_class_id_MemoryFabric
#define __nvoc_class_id_MemoryFabric 0x127499
#endif /* __nvoc_class_id_MemoryFabric */

struct MemoryFabricImportedRef;

#ifndef __NVOC_CLASS_MemoryFabricImportedRef_TYPEDEF__
#define __NVOC_CLASS_MemoryFabricImportedRef_TYPEDEF__
typedef struct MemoryFabricImportedRef MemoryFabricImportedRef;
#endif /* __NVOC_CLASS_MemoryFabricImportedRef_TYPEDEF__ */

#ifndef __nvoc_class_id_MemoryFabricImportedRef
#define __nvoc_class_id_MemoryFabricImportedRef 0x189bad
#endif /* __nvoc_class_id_MemoryFabricImportedRef */

struct VaSpaceApi;

#ifndef __NVOC_CLASS_VaSpaceApi_TYPEDEF__
#define __NVOC_CLASS_VaSpaceApi_TYPEDEF__
typedef struct VaSpaceApi VaSpaceApi;
#endif /* __NVOC_CLASS_VaSpaceApi_TYPEDEF__ */

#ifndef __nvoc_class_id_VaSpaceApi
#define __nvoc_class_id_VaSpaceApi 0xcd048b
#endif /* __nvoc_class_id_VaSpaceApi */

struct MemoryMulticastFabric;

#ifndef __NVOC_CLASS_MemoryMulticastFabric_TYPEDEF__
#define __NVOC_CLASS_MemoryMulticastFabric_TYPEDEF__
typedef struct MemoryMulticastFabric MemoryMulticastFabric;
#endif /* __NVOC_CLASS_MemoryMulticastFabric_TYPEDEF__ */

#ifndef __nvoc_class_id_MemoryMulticastFabric
#define __nvoc_class_id_MemoryMulticastFabric 0x130210
#endif /* __nvoc_class_id_MemoryMulticastFabric */

struct NvencSession;

#ifndef __NVOC_CLASS_NvencSession_TYPEDEF__
#define __NVOC_CLASS_NvencSession_TYPEDEF__
typedef struct NvencSession NvencSession;
#endif /* __NVOC_CLASS_NvencSession_TYPEDEF__ */

#ifndef __nvoc_class_id_NvencSession
#define __nvoc_class_id_NvencSession 0x3434af
#endif /* __nvoc_class_id_NvencSession */

struct NvfbcSession;

#ifndef __NVOC_CLASS_NvfbcSession_TYPEDEF__
#define __NVOC_CLASS_NvfbcSession_TYPEDEF__
typedef struct NvfbcSession NvfbcSession;
#endif /* __NVOC_CLASS_NvfbcSession_TYPEDEF__ */

#ifndef __nvoc_class_id_NvfbcSession
#define __nvoc_class_id_NvfbcSession 0xcbde75
#endif /* __nvoc_class_id_NvfbcSession */

struct VgpuConfigApi;

#ifndef __NVOC_CLASS_VgpuConfigApi_TYPEDEF__
#define __NVOC_CLASS_VgpuConfigApi_TYPEDEF__
typedef struct VgpuConfigApi VgpuConfigApi;
#endif /* __NVOC_CLASS_VgpuConfigApi_TYPEDEF__ */

#ifndef __nvoc_class_id_VgpuConfigApi
#define __nvoc_class_id_VgpuConfigApi 0x4d560a
#endif /* __nvoc_class_id_VgpuConfigApi */

struct KernelHostVgpuDeviceApi;

#ifndef __NVOC_CLASS_KernelHostVgpuDeviceApi_TYPEDEF__
#define __NVOC_CLASS_KernelHostVgpuDeviceApi_TYPEDEF__
typedef struct KernelHostVgpuDeviceApi KernelHostVgpuDeviceApi;
#endif /* __NVOC_CLASS_KernelHostVgpuDeviceApi_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelHostVgpuDeviceApi
#define __nvoc_class_id_KernelHostVgpuDeviceApi 0xb12d7d
#endif /* __nvoc_class_id_KernelHostVgpuDeviceApi */

struct ThirdPartyP2P;

#ifndef __NVOC_CLASS_ThirdPartyP2P_TYPEDEF__
#define __NVOC_CLASS_ThirdPartyP2P_TYPEDEF__
typedef struct ThirdPartyP2P ThirdPartyP2P;
#endif /* __NVOC_CLASS_ThirdPartyP2P_TYPEDEF__ */

#ifndef __nvoc_class_id_ThirdPartyP2P
#define __nvoc_class_id_ThirdPartyP2P 0x34d08b
#endif /* __nvoc_class_id_ThirdPartyP2P */

struct GenericEngineApi;

#ifndef __NVOC_CLASS_GenericEngineApi_TYPEDEF__
#define __NVOC_CLASS_GenericEngineApi_TYPEDEF__
typedef struct GenericEngineApi GenericEngineApi;
#endif /* __NVOC_CLASS_GenericEngineApi_TYPEDEF__ */

#ifndef __nvoc_class_id_GenericEngineApi
#define __nvoc_class_id_GenericEngineApi 0x4bc329
#endif /* __nvoc_class_id_GenericEngineApi */

struct I2cApi;

#ifndef __NVOC_CLASS_I2cApi_TYPEDEF__
#define __NVOC_CLASS_I2cApi_TYPEDEF__
typedef struct I2cApi I2cApi;
#endif /* __NVOC_CLASS_I2cApi_TYPEDEF__ */

#ifndef __nvoc_class_id_I2cApi
#define __nvoc_class_id_I2cApi 0xceb8f6
#endif /* __nvoc_class_id_I2cApi */

struct DiagApi;

#ifndef __NVOC_CLASS_DiagApi_TYPEDEF__
#define __NVOC_CLASS_DiagApi_TYPEDEF__
typedef struct DiagApi DiagApi;
#endif /* __NVOC_CLASS_DiagApi_TYPEDEF__ */

#ifndef __nvoc_class_id_DiagApi
#define __nvoc_class_id_DiagApi 0xaa3066
#endif /* __nvoc_class_id_DiagApi */

struct ZbcApi;

#ifndef __NVOC_CLASS_ZbcApi_TYPEDEF__
#define __NVOC_CLASS_ZbcApi_TYPEDEF__
typedef struct ZbcApi ZbcApi;
#endif /* __NVOC_CLASS_ZbcApi_TYPEDEF__ */

#ifndef __nvoc_class_id_ZbcApi
#define __nvoc_class_id_ZbcApi 0x397ee3
#endif /* __nvoc_class_id_ZbcApi */

struct DebugBufferApi;

#ifndef __NVOC_CLASS_DebugBufferApi_TYPEDEF__
#define __NVOC_CLASS_DebugBufferApi_TYPEDEF__
typedef struct DebugBufferApi DebugBufferApi;
#endif /* __NVOC_CLASS_DebugBufferApi_TYPEDEF__ */

#ifndef __nvoc_class_id_DebugBufferApi
#define __nvoc_class_id_DebugBufferApi 0x5e7a1b
#endif /* __nvoc_class_id_DebugBufferApi */

struct GpuUserSharedData;

#ifndef __NVOC_CLASS_GpuUserSharedData_TYPEDEF__
#define __NVOC_CLASS_GpuUserSharedData_TYPEDEF__
typedef struct GpuUserSharedData GpuUserSharedData;
#endif /* __NVOC_CLASS_GpuUserSharedData_TYPEDEF__ */

#ifndef __nvoc_class_id_GpuUserSharedData
#define __nvoc_class_id_GpuUserSharedData 0x5e7d1f
#endif /* __nvoc_class_id_GpuUserSharedData */

struct UserModeApi;

#ifndef __NVOC_CLASS_UserModeApi_TYPEDEF__
#define __NVOC_CLASS_UserModeApi_TYPEDEF__
typedef struct UserModeApi UserModeApi;
#endif /* __NVOC_CLASS_UserModeApi_TYPEDEF__ */

#ifndef __nvoc_class_id_UserModeApi
#define __nvoc_class_id_UserModeApi 0x6f57ec
#endif /* __nvoc_class_id_UserModeApi */

struct DispSfUser;

#ifndef __NVOC_CLASS_DispSfUser_TYPEDEF__
#define __NVOC_CLASS_DispSfUser_TYPEDEF__
typedef struct DispSfUser DispSfUser;
#endif /* __NVOC_CLASS_DispSfUser_TYPEDEF__ */

#ifndef __nvoc_class_id_DispSfUser
#define __nvoc_class_id_DispSfUser 0xba7439
#endif /* __nvoc_class_id_DispSfUser */

struct MmuFaultBuffer;

#ifndef __NVOC_CLASS_MmuFaultBuffer_TYPEDEF__
#define __NVOC_CLASS_MmuFaultBuffer_TYPEDEF__
typedef struct MmuFaultBuffer MmuFaultBuffer;
#endif /* __NVOC_CLASS_MmuFaultBuffer_TYPEDEF__ */

#ifndef __nvoc_class_id_MmuFaultBuffer
#define __nvoc_class_id_MmuFaultBuffer 0x7e1829
#endif /* __nvoc_class_id_MmuFaultBuffer */

struct AccessCounterBuffer;

#ifndef __NVOC_CLASS_AccessCounterBuffer_TYPEDEF__
#define __NVOC_CLASS_AccessCounterBuffer_TYPEDEF__
typedef struct AccessCounterBuffer AccessCounterBuffer;
#endif /* __NVOC_CLASS_AccessCounterBuffer_TYPEDEF__ */

#ifndef __nvoc_class_id_AccessCounterBuffer
#define __nvoc_class_id_AccessCounterBuffer 0x1f0074
#endif /* __nvoc_class_id_AccessCounterBuffer */

struct VidmemAccessBitBuffer;

#ifndef __NVOC_CLASS_VidmemAccessBitBuffer_TYPEDEF__
#define __NVOC_CLASS_VidmemAccessBitBuffer_TYPEDEF__
typedef struct VidmemAccessBitBuffer VidmemAccessBitBuffer;
#endif /* __NVOC_CLASS_VidmemAccessBitBuffer_TYPEDEF__ */

#ifndef __nvoc_class_id_VidmemAccessBitBuffer
#define __nvoc_class_id_VidmemAccessBitBuffer 0xebb6da
#endif /* __nvoc_class_id_VidmemAccessBitBuffer */

struct GPUInstanceSubscription;

#ifndef __NVOC_CLASS_GPUInstanceSubscription_TYPEDEF__
#define __NVOC_CLASS_GPUInstanceSubscription_TYPEDEF__
typedef struct GPUInstanceSubscription GPUInstanceSubscription;
#endif /* __NVOC_CLASS_GPUInstanceSubscription_TYPEDEF__ */

#ifndef __nvoc_class_id_GPUInstanceSubscription
#define __nvoc_class_id_GPUInstanceSubscription 0x91fde7
#endif /* __nvoc_class_id_GPUInstanceSubscription */

struct ComputeInstanceSubscription;

#ifndef __NVOC_CLASS_ComputeInstanceSubscription_TYPEDEF__
#define __NVOC_CLASS_ComputeInstanceSubscription_TYPEDEF__
typedef struct ComputeInstanceSubscription ComputeInstanceSubscription;
#endif /* __NVOC_CLASS_ComputeInstanceSubscription_TYPEDEF__ */

#ifndef __nvoc_class_id_ComputeInstanceSubscription
#define __nvoc_class_id_ComputeInstanceSubscription 0xd1f238
#endif /* __nvoc_class_id_ComputeInstanceSubscription */

struct NvDispApi;

#ifndef __NVOC_CLASS_NvDispApi_TYPEDEF__
#define __NVOC_CLASS_NvDispApi_TYPEDEF__
typedef struct NvDispApi NvDispApi;
#endif /* __NVOC_CLASS_NvDispApi_TYPEDEF__ */

#ifndef __nvoc_class_id_NvDispApi
#define __nvoc_class_id_NvDispApi 0x36aa0b
#endif /* __nvoc_class_id_NvDispApi */

struct DispSwObj;

#ifndef __NVOC_CLASS_DispSwObj_TYPEDEF__
#define __NVOC_CLASS_DispSwObj_TYPEDEF__
typedef struct DispSwObj DispSwObj;
#endif /* __NVOC_CLASS_DispSwObj_TYPEDEF__ */

#ifndef __nvoc_class_id_DispSwObj
#define __nvoc_class_id_DispSwObj 0x6aa5e2
#endif /* __nvoc_class_id_DispSwObj */

struct DispCommon;

#ifndef __NVOC_CLASS_DispCommon_TYPEDEF__
#define __NVOC_CLASS_DispCommon_TYPEDEF__
typedef struct DispCommon DispCommon;
#endif /* __NVOC_CLASS_DispCommon_TYPEDEF__ */

#ifndef __nvoc_class_id_DispCommon
#define __nvoc_class_id_DispCommon 0x41f4f2
#endif /* __nvoc_class_id_DispCommon */

struct VblankCallback;

#ifndef __NVOC_CLASS_VblankCallback_TYPEDEF__
#define __NVOC_CLASS_VblankCallback_TYPEDEF__
typedef struct VblankCallback VblankCallback;
#endif /* __NVOC_CLASS_VblankCallback_TYPEDEF__ */

#ifndef __nvoc_class_id_VblankCallback
#define __nvoc_class_id_VblankCallback 0x4c1997
#endif /* __nvoc_class_id_VblankCallback */

struct RgLineCallback;

#ifndef __NVOC_CLASS_RgLineCallback_TYPEDEF__
#define __NVOC_CLASS_RgLineCallback_TYPEDEF__
typedef struct RgLineCallback RgLineCallback;
#endif /* __NVOC_CLASS_RgLineCallback_TYPEDEF__ */

#ifndef __nvoc_class_id_RgLineCallback
#define __nvoc_class_id_RgLineCallback 0xa3ff1c
#endif /* __nvoc_class_id_RgLineCallback */

struct DispChannelPio;

#ifndef __NVOC_CLASS_DispChannelPio_TYPEDEF__
#define __NVOC_CLASS_DispChannelPio_TYPEDEF__
typedef struct DispChannelPio DispChannelPio;
#endif /* __NVOC_CLASS_DispChannelPio_TYPEDEF__ */

#ifndef __nvoc_class_id_DispChannelPio
#define __nvoc_class_id_DispChannelPio 0x10dec3
#endif /* __nvoc_class_id_DispChannelPio */

struct DispChannelDma;

#ifndef __NVOC_CLASS_DispChannelDma_TYPEDEF__
#define __NVOC_CLASS_DispChannelDma_TYPEDEF__
typedef struct DispChannelDma DispChannelDma;
#endif /* __NVOC_CLASS_DispChannelDma_TYPEDEF__ */

#ifndef __nvoc_class_id_DispChannelDma
#define __nvoc_class_id_DispChannelDma 0xfe3d2e
#endif /* __nvoc_class_id_DispChannelDma */

struct DispCapabilities;

#ifndef __NVOC_CLASS_DispCapabilities_TYPEDEF__
#define __NVOC_CLASS_DispCapabilities_TYPEDEF__
typedef struct DispCapabilities DispCapabilities;
#endif /* __NVOC_CLASS_DispCapabilities_TYPEDEF__ */

#ifndef __nvoc_class_id_DispCapabilities
#define __nvoc_class_id_DispCapabilities 0x99db3e
#endif /* __nvoc_class_id_DispCapabilities */

struct DispSwObject;

#ifndef __NVOC_CLASS_DispSwObject_TYPEDEF__
#define __NVOC_CLASS_DispSwObject_TYPEDEF__
typedef struct DispSwObject DispSwObject;
#endif /* __NVOC_CLASS_DispSwObject_TYPEDEF__ */

#ifndef __nvoc_class_id_DispSwObject
#define __nvoc_class_id_DispSwObject 0x99ad6d
#endif /* __nvoc_class_id_DispSwObject */

struct TimedSemaSwObject;

#ifndef __NVOC_CLASS_TimedSemaSwObject_TYPEDEF__
#define __NVOC_CLASS_TimedSemaSwObject_TYPEDEF__
typedef struct TimedSemaSwObject TimedSemaSwObject;
#endif /* __NVOC_CLASS_TimedSemaSwObject_TYPEDEF__ */

#ifndef __nvoc_class_id_TimedSemaSwObject
#define __nvoc_class_id_TimedSemaSwObject 0x335775
#endif /* __nvoc_class_id_TimedSemaSwObject */

struct DeferredApiObject;

#ifndef __NVOC_CLASS_DeferredApiObject_TYPEDEF__
#define __NVOC_CLASS_DeferredApiObject_TYPEDEF__
typedef struct DeferredApiObject DeferredApiObject;
#endif /* __NVOC_CLASS_DeferredApiObject_TYPEDEF__ */

#ifndef __nvoc_class_id_DeferredApiObject
#define __nvoc_class_id_DeferredApiObject 0x8ea933
#endif /* __nvoc_class_id_DeferredApiObject */

struct UvmSwObject;

#ifndef __NVOC_CLASS_UvmSwObject_TYPEDEF__
#define __NVOC_CLASS_UvmSwObject_TYPEDEF__
typedef struct UvmSwObject UvmSwObject;
#endif /* __NVOC_CLASS_UvmSwObject_TYPEDEF__ */

#ifndef __nvoc_class_id_UvmSwObject
#define __nvoc_class_id_UvmSwObject 0xc35503
#endif /* __nvoc_class_id_UvmSwObject */

struct SoftwareMethodTest;

#ifndef __NVOC_CLASS_SoftwareMethodTest_TYPEDEF__
#define __NVOC_CLASS_SoftwareMethodTest_TYPEDEF__
typedef struct SoftwareMethodTest SoftwareMethodTest;
#endif /* __NVOC_CLASS_SoftwareMethodTest_TYPEDEF__ */

#ifndef __nvoc_class_id_SoftwareMethodTest
#define __nvoc_class_id_SoftwareMethodTest 0xdea092
#endif /* __nvoc_class_id_SoftwareMethodTest */

struct KernelCeContext;

#ifndef __NVOC_CLASS_KernelCeContext_TYPEDEF__
#define __NVOC_CLASS_KernelCeContext_TYPEDEF__
typedef struct KernelCeContext KernelCeContext;
#endif /* __NVOC_CLASS_KernelCeContext_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelCeContext
#define __nvoc_class_id_KernelCeContext 0x2d0ee9
#endif /* __nvoc_class_id_KernelCeContext */

struct NvdecContext;

#ifndef __NVOC_CLASS_NvdecContext_TYPEDEF__
#define __NVOC_CLASS_NvdecContext_TYPEDEF__
typedef struct NvdecContext NvdecContext;
#endif /* __NVOC_CLASS_NvdecContext_TYPEDEF__ */

#ifndef __nvoc_class_id_NvdecContext
#define __nvoc_class_id_NvdecContext 0x70d2be
#endif /* __nvoc_class_id_NvdecContext */

struct NvjpgContext;

#ifndef __NVOC_CLASS_NvjpgContext_TYPEDEF__
#define __NVOC_CLASS_NvjpgContext_TYPEDEF__
typedef struct NvjpgContext NvjpgContext;
#endif /* __NVOC_CLASS_NvjpgContext_TYPEDEF__ */

#ifndef __nvoc_class_id_NvjpgContext
#define __nvoc_class_id_NvjpgContext 0x08c1ce
#endif /* __nvoc_class_id_NvjpgContext */

struct OfaContext;

#ifndef __NVOC_CLASS_OfaContext_TYPEDEF__
#define __NVOC_CLASS_OfaContext_TYPEDEF__
typedef struct OfaContext OfaContext;
#endif /* __NVOC_CLASS_OfaContext_TYPEDEF__ */

#ifndef __nvoc_class_id_OfaContext
#define __nvoc_class_id_OfaContext 0xf63d99
#endif /* __nvoc_class_id_OfaContext */

struct MsencContext;

#ifndef __NVOC_CLASS_MsencContext_TYPEDEF__
#define __NVOC_CLASS_MsencContext_TYPEDEF__
typedef struct MsencContext MsencContext;
#endif /* __NVOC_CLASS_MsencContext_TYPEDEF__ */

#ifndef __nvoc_class_id_MsencContext
#define __nvoc_class_id_MsencContext 0x88c92a
#endif /* __nvoc_class_id_MsencContext */

struct Sec2Context;

#ifndef __NVOC_CLASS_Sec2Context_TYPEDEF__
#define __NVOC_CLASS_Sec2Context_TYPEDEF__
typedef struct Sec2Context Sec2Context;
#endif /* __NVOC_CLASS_Sec2Context_TYPEDEF__ */

#ifndef __nvoc_class_id_Sec2Context
#define __nvoc_class_id_Sec2Context 0x4c3439
#endif /* __nvoc_class_id_Sec2Context */

struct KernelGraphicsObject;

#ifndef __NVOC_CLASS_KernelGraphicsObject_TYPEDEF__
#define __NVOC_CLASS_KernelGraphicsObject_TYPEDEF__
typedef struct KernelGraphicsObject KernelGraphicsObject;
#endif /* __NVOC_CLASS_KernelGraphicsObject_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelGraphicsObject
#define __nvoc_class_id_KernelGraphicsObject 0x097648
#endif /* __nvoc_class_id_KernelGraphicsObject */

struct ContextDma;

#ifndef __NVOC_CLASS_ContextDma_TYPEDEF__
#define __NVOC_CLASS_ContextDma_TYPEDEF__
typedef struct ContextDma ContextDma;
#endif /* __NVOC_CLASS_ContextDma_TYPEDEF__ */

#ifndef __nvoc_class_id_ContextDma
#define __nvoc_class_id_ContextDma 0x88441b
#endif /* __nvoc_class_id_ContextDma */

struct Event;

#ifndef __NVOC_CLASS_Event_TYPEDEF__
#define __NVOC_CLASS_Event_TYPEDEF__
typedef struct Event Event;
#endif /* __NVOC_CLASS_Event_TYPEDEF__ */

#ifndef __nvoc_class_id_Event
#define __nvoc_class_id_Event 0xa4ecfc
#endif /* __nvoc_class_id_Event */

struct ConfidentialComputeApi;

#ifndef __NVOC_CLASS_ConfidentialComputeApi_TYPEDEF__
#define __NVOC_CLASS_ConfidentialComputeApi_TYPEDEF__
typedef struct ConfidentialComputeApi ConfidentialComputeApi;
#endif /* __NVOC_CLASS_ConfidentialComputeApi_TYPEDEF__ */

#ifndef __nvoc_class_id_ConfidentialComputeApi
#define __nvoc_class_id_ConfidentialComputeApi 0xea5cb0
#endif /* __nvoc_class_id_ConfidentialComputeApi */

struct KernelCcuApi;

#ifndef __NVOC_CLASS_KernelCcuApi_TYPEDEF__
#define __NVOC_CLASS_KernelCcuApi_TYPEDEF__
typedef struct KernelCcuApi KernelCcuApi;
#endif /* __NVOC_CLASS_KernelCcuApi_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelCcuApi
#define __nvoc_class_id_KernelCcuApi 0x3abed3
#endif /* __nvoc_class_id_KernelCcuApi */

struct CeUtilsApi;

#ifndef __NVOC_CLASS_CeUtilsApi_TYPEDEF__
#define __NVOC_CLASS_CeUtilsApi_TYPEDEF__
typedef struct CeUtilsApi CeUtilsApi;
#endif /* __NVOC_CLASS_CeUtilsApi_TYPEDEF__ */

#ifndef __nvoc_class_id_CeUtilsApi
#define __nvoc_class_id_CeUtilsApi 0x2eb528
#endif /* __nvoc_class_id_CeUtilsApi */

struct LockStressObject;

#ifndef __NVOC_CLASS_LockStressObject_TYPEDEF__
#define __NVOC_CLASS_LockStressObject_TYPEDEF__
typedef struct LockStressObject LockStressObject;
#endif /* __NVOC_CLASS_LockStressObject_TYPEDEF__ */

#ifndef __nvoc_class_id_LockStressObject
#define __nvoc_class_id_LockStressObject 0xecce10
#endif /* __nvoc_class_id_LockStressObject */

struct LockTestRelaxedDupObject;

#ifndef __NVOC_CLASS_LockTestRelaxedDupObject_TYPEDEF__
#define __NVOC_CLASS_LockTestRelaxedDupObject_TYPEDEF__
typedef struct LockTestRelaxedDupObject LockTestRelaxedDupObject;
#endif /* __NVOC_CLASS_LockTestRelaxedDupObject_TYPEDEF__ */

#ifndef __nvoc_class_id_LockTestRelaxedDupObject
#define __nvoc_class_id_LockTestRelaxedDupObject 0x19e861
#endif /* __nvoc_class_id_LockTestRelaxedDupObject */



struct NVOCFwdDeclHack {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__NVOCFwdDeclHack *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Ancestor object pointers for `staticCast` feature
    struct NVOCFwdDeclHack *__nvoc_pbase_NVOCFwdDeclHack;    // nvocfwddeclhack

    // Data members
    struct RmClientResource *PRIVATE_FIELD(RmClientResource_NV01_ROOT);
    struct RmClientResource *PRIVATE_FIELD(RmClientResource_NV01_ROOT_NON_PRIV);
    struct RmClientResource *PRIVATE_FIELD(RmClientResource_NV01_ROOT_CLIENT);
    struct MpsApi *PRIVATE_FIELD(MpsApi_MPS_COMPUTE);
    struct FmSessionApi *PRIVATE_FIELD(FmSessionApi_FABRIC_MANAGER_SESSION);
    struct ImexSessionApi *PRIVATE_FIELD(ImexSessionApi_NV_IMEX_SESSION);
    struct GpuManagementApi *PRIVATE_FIELD(GpuManagementApi_NV0020_GPU_MANAGEMENT);
    struct EventBuffer *PRIVATE_FIELD(EventBuffer_NV_EVENT_BUFFER);
    struct SemaphoreSurface *PRIVATE_FIELD(SemaphoreSurface_NV_SEMAPHORE_SURFACE);
    struct P2PApi *PRIVATE_FIELD(P2PApi_NV50_P2P);
    struct SyncGpuBoost *PRIVATE_FIELD(SyncGpuBoost_NV0060_SYNC_GPU_BOOST);
    struct Device *PRIVATE_FIELD(Device_NV01_DEVICE_0);
    struct KernelSMDebuggerSession *PRIVATE_FIELD(KernelSMDebuggerSession_GT200_DEBUGGER);
    struct GSyncApi *PRIVATE_FIELD(GSyncApi_NV30_GSYNC);
    struct Profiler *PRIVATE_FIELD(Profiler_GF100_PROFILER);
    struct ProfilerCtx *PRIVATE_FIELD(ProfilerCtx_MAXWELL_PROFILER_CONTEXT);
    struct ProfilerDev *PRIVATE_FIELD(ProfilerDev_MAXWELL_PROFILER_DEVICE);
    struct PerfBuffer *PRIVATE_FIELD(PerfBuffer_G84_PERFBUFFER);
    struct VgpuApi *PRIVATE_FIELD(VgpuApi_KEPLER_DEVICE_VGPU);
    struct Hdacodec *PRIVATE_FIELD(Hdacodec_GF100_HDACODEC);
    struct KernelChannel *PRIVATE_FIELD(KernelChannel_GF100_CHANNEL_GPFIFO);
    struct KernelChannel *PRIVATE_FIELD(KernelChannel_KEPLER_CHANNEL_GPFIFO_A);
    struct KernelChannel *PRIVATE_FIELD(KernelChannel_KEPLER_CHANNEL_GPFIFO_B);
    struct KernelChannel *PRIVATE_FIELD(KernelChannel_MAXWELL_CHANNEL_GPFIFO_A);
    struct KernelChannel *PRIVATE_FIELD(KernelChannel_PASCAL_CHANNEL_GPFIFO_A);
    struct KernelChannel *PRIVATE_FIELD(KernelChannel_VOLTA_CHANNEL_GPFIFO_A);
    struct KernelChannel *PRIVATE_FIELD(KernelChannel_TURING_CHANNEL_GPFIFO_A);
    struct KernelChannel *PRIVATE_FIELD(KernelChannel_AMPERE_CHANNEL_GPFIFO_A);
    struct KernelChannel *PRIVATE_FIELD(KernelChannel_HOPPER_CHANNEL_GPFIFO_A);
    struct KernelChannel *PRIVATE_FIELD(KernelChannel_BLACKWELL_CHANNEL_GPFIFO_A);
    struct KernelChannel *PRIVATE_FIELD(KernelChannel_BLACKWELL_CHANNEL_GPFIFO_B);
    struct UvmChannelRetainer *PRIVATE_FIELD(UvmChannelRetainer_UVM_CHANNEL_RETAINER);
    struct KernelCtxShareApi *PRIVATE_FIELD(KernelCtxShareApi_FERMI_CONTEXT_SHARE_A);
    struct KernelGraphicsContext *PRIVATE_FIELD(KernelGraphicsContext_KERNEL_GRAPHICS_CONTEXT);
    struct Subdevice *PRIVATE_FIELD(Subdevice_NV20_SUBDEVICE_0);
    struct BinaryApi *PRIVATE_FIELD(BinaryApi_NV2081_BINAPI);
    struct BinaryApiPrivileged *PRIVATE_FIELD(BinaryApiPrivileged_NV2082_BINAPI_PRIVILEGED);
    struct KernelChannelGroupApi *PRIVATE_FIELD(KernelChannelGroupApi_KEPLER_CHANNEL_GROUP_A);
    struct RegisterMemory *PRIVATE_FIELD(RegisterMemory_NV01_MEMORY_LOCAL_PRIVILEGED);
    struct VideoMemory *PRIVATE_FIELD(VideoMemory_NV01_MEMORY_LOCAL_USER);
    struct ExtendedGpuMemory *PRIVATE_FIELD(ExtendedGpuMemory_NV_MEMORY_EXTENDED_USER);
    struct PhysicalMemory *PRIVATE_FIELD(PhysicalMemory_NV01_MEMORY_LOCAL_PHYSICAL);
    struct VirtualMemory *PRIVATE_FIELD(VirtualMemory_NV50_MEMORY_VIRTUAL);
    struct SystemMemory *PRIVATE_FIELD(SystemMemory_NV01_MEMORY_SYSTEM);
    struct VirtualMemoryRange *PRIVATE_FIELD(VirtualMemoryRange_NV01_MEMORY_VIRTUAL);
    struct MemoryMapper *PRIVATE_FIELD(MemoryMapper_NV_MEMORY_MAPPER);
    struct OsDescMemory *PRIVATE_FIELD(OsDescMemory_NV01_MEMORY_SYSTEM_OS_DESCRIPTOR);
    struct SyncpointMemory *PRIVATE_FIELD(SyncpointMemory_NV01_MEMORY_SYNCPOINT);
    struct NoDeviceMemory *PRIVATE_FIELD(NoDeviceMemory_NV01_MEMORY_DEVICELESS);
    struct ConsoleMemory *PRIVATE_FIELD(ConsoleMemory_NV01_MEMORY_FRAMEBUFFER_CONSOLE);
    struct MemoryHwResources *PRIVATE_FIELD(MemoryHwResources_NV01_MEMORY_HW_RESOURCES);
    struct MemoryList *PRIVATE_FIELD(MemoryList_NV01_MEMORY_LIST_SYSTEM);
    struct MemoryList *PRIVATE_FIELD(MemoryList_NV01_MEMORY_LIST_FBMEM);
    struct MemoryList *PRIVATE_FIELD(MemoryList_NV01_MEMORY_LIST_OBJECT);
    struct FlaMemory *PRIVATE_FIELD(FlaMemory_NV01_MEMORY_FLA);
    struct MemoryExport *PRIVATE_FIELD(MemoryExport_NV_MEMORY_EXPORT);
    struct MemoryFabricImportV2 *PRIVATE_FIELD(MemoryFabricImportV2_NV_MEMORY_FABRIC_IMPORT_V2);
    struct MemoryFabric *PRIVATE_FIELD(MemoryFabric_NV_MEMORY_FABRIC);
    struct MemoryFabricImportedRef *PRIVATE_FIELD(MemoryFabricImportedRef_NV_MEMORY_FABRIC_IMPORTED_REF);
    struct VaSpaceApi *PRIVATE_FIELD(VaSpaceApi_FERMI_VASPACE_A);
    struct MemoryMulticastFabric *PRIVATE_FIELD(MemoryMulticastFabric_NV_MEMORY_MULTICAST_FABRIC);
    struct NvencSession *PRIVATE_FIELD(NvencSession_NVENC_SW_SESSION);
    struct NvfbcSession *PRIVATE_FIELD(NvfbcSession_NVFBC_SW_SESSION);
    struct VgpuConfigApi *PRIVATE_FIELD(VgpuConfigApi_NVA081_VGPU_CONFIG);
    struct KernelHostVgpuDeviceApi *PRIVATE_FIELD(KernelHostVgpuDeviceApi_NVA084_KERNEL_HOST_VGPU_DEVICE);
    struct ThirdPartyP2P *PRIVATE_FIELD(ThirdPartyP2P_NV50_THIRD_PARTY_P2P);
    struct GenericEngineApi *PRIVATE_FIELD(GenericEngineApi_GF100_SUBDEVICE_MASTER);
    struct GenericEngineApi *PRIVATE_FIELD(GenericEngineApi_GF100_SUBDEVICE_INFOROM);
    struct TimerApi *PRIVATE_FIELD(TimerApi_NV01_TIMER);
    struct I2cApi *PRIVATE_FIELD(I2cApi_NV40_I2C);
    struct DiagApi *PRIVATE_FIELD(DiagApi_NV20_SUBDEVICE_DIAG);
    struct ZbcApi *PRIVATE_FIELD(ZbcApi_GF100_ZBC_CLEAR);
    struct DebugBufferApi *PRIVATE_FIELD(DebugBufferApi_NV40_DEBUG_BUFFER);
    struct GpuUserSharedData *PRIVATE_FIELD(GpuUserSharedData_RM_USER_SHARED_DATA);
    struct UserModeApi *PRIVATE_FIELD(UserModeApi_VOLTA_USERMODE_A);
    struct UserModeApi *PRIVATE_FIELD(UserModeApi_TURING_USERMODE_A);
    struct UserModeApi *PRIVATE_FIELD(UserModeApi_AMPERE_USERMODE_A);
    struct UserModeApi *PRIVATE_FIELD(UserModeApi_HOPPER_USERMODE_A);
    struct UserModeApi *PRIVATE_FIELD(UserModeApi_BLACKWELL_USERMODE_A);
    struct DispSfUser *PRIVATE_FIELD(DispSfUser_NVC371_DISP_SF_USER);
    struct DispSfUser *PRIVATE_FIELD(DispSfUser_NVC671_DISP_SF_USER);
    struct DispSfUser *PRIVATE_FIELD(DispSfUser_NVC771_DISP_SF_USER);
    struct DispSfUser *PRIVATE_FIELD(DispSfUser_NVC971_DISP_SF_USER);
    struct DispSfUser *PRIVATE_FIELD(DispSfUser_NVCA71_DISP_SF_USER);
    struct DispSfUser *PRIVATE_FIELD(DispSfUser_NVCB71_DISP_SF_USER);
    struct MmuFaultBuffer *PRIVATE_FIELD(MmuFaultBuffer_MMU_FAULT_BUFFER);
    struct AccessCounterBuffer *PRIVATE_FIELD(AccessCounterBuffer_ACCESS_COUNTER_NOTIFY_BUFFER);
    struct VidmemAccessBitBuffer *PRIVATE_FIELD(VidmemAccessBitBuffer_MMU_VIDMEM_ACCESS_BIT_BUFFER);
    struct GPUInstanceSubscription *PRIVATE_FIELD(GPUInstanceSubscription_AMPERE_SMC_PARTITION_REF);
    struct ComputeInstanceSubscription *PRIVATE_FIELD(ComputeInstanceSubscription_AMPERE_SMC_EXEC_PARTITION_REF);
    struct MIGConfigSession *PRIVATE_FIELD(MIGConfigSession_AMPERE_SMC_CONFIG_SESSION);
    struct MIGMonitorSession *PRIVATE_FIELD(MIGMonitorSession_AMPERE_SMC_MONITOR_SESSION);
    struct NvDispApi *PRIVATE_FIELD(NvDispApi_NVC570_DISPLAY);
    struct NvDispApi *PRIVATE_FIELD(NvDispApi_NVC670_DISPLAY);
    struct NvDispApi *PRIVATE_FIELD(NvDispApi_NVC770_DISPLAY);
    struct NvDispApi *PRIVATE_FIELD(NvDispApi_NVC970_DISPLAY);
    struct NvDispApi *PRIVATE_FIELD(NvDispApi_NVCA70_DISPLAY);
    struct NvDispApi *PRIVATE_FIELD(NvDispApi_NVCB70_DISPLAY);
    struct DispSwObj *PRIVATE_FIELD(DispSwObj_NVC372_DISPLAY_SW);
    struct DispCommon *PRIVATE_FIELD(DispCommon_NV04_DISPLAY_COMMON);
    struct VblankCallback *PRIVATE_FIELD(VblankCallback_NV9010_VBLANK_CALLBACK);
    struct RgLineCallback *PRIVATE_FIELD(RgLineCallback_NV0092_RG_LINE_CALLBACK);
    struct DispChannelPio *PRIVATE_FIELD(DispChannelPio_NVC57A_CURSOR_IMM_CHANNEL_PIO);
    struct DispChannelPio *PRIVATE_FIELD(DispChannelPio_NVC67A_CURSOR_IMM_CHANNEL_PIO);
    struct DispChannelPio *PRIVATE_FIELD(DispChannelPio_NVC97A_CURSOR_IMM_CHANNEL_PIO);
    struct DispChannelPio *PRIVATE_FIELD(DispChannelPio_NVCA7A_CURSOR_IMM_CHANNEL_PIO);
    struct DispChannelPio *PRIVATE_FIELD(DispChannelPio_NVCB7A_CURSOR_IMM_CHANNEL_PIO);
    struct DispChannelDma *PRIVATE_FIELD(DispChannelDma_NVC57B_WINDOW_IMM_CHANNEL_DMA);
    struct DispChannelDma *PRIVATE_FIELD(DispChannelDma_NVC57D_CORE_CHANNEL_DMA);
    struct DispChannelDma *PRIVATE_FIELD(DispChannelDma_NVC57E_WINDOW_CHANNEL_DMA);
    struct DispCapabilities *PRIVATE_FIELD(DispCapabilities_NVC573_DISP_CAPABILITIES);
    struct DispChannelDma *PRIVATE_FIELD(DispChannelDma_NVC67B_WINDOW_IMM_CHANNEL_DMA);
    struct DispChannelDma *PRIVATE_FIELD(DispChannelDma_NVC67D_CORE_CHANNEL_DMA);
    struct DispChannelDma *PRIVATE_FIELD(DispChannelDma_NVC77D_CORE_CHANNEL_DMA);
    struct DispChannelDma *PRIVATE_FIELD(DispChannelDma_NVC77F_ANY_CHANNEL_DMA);
    struct DispChannelDma *PRIVATE_FIELD(DispChannelDma_NVC67E_WINDOW_CHANNEL_DMA);
    struct DispCapabilities *PRIVATE_FIELD(DispCapabilities_NVC673_DISP_CAPABILITIES);
    struct DispCapabilities *PRIVATE_FIELD(DispCapabilities_NVC773_DISP_CAPABILITIES);
    struct DispChannelDma *PRIVATE_FIELD(DispChannelDma_NVC97B_WINDOW_IMM_CHANNEL_DMA);
    struct DispChannelDma *PRIVATE_FIELD(DispChannelDma_NVC97D_CORE_CHANNEL_DMA);
    struct DispChannelDma *PRIVATE_FIELD(DispChannelDma_NVC97E_WINDOW_CHANNEL_DMA);
    struct DispCapabilities *PRIVATE_FIELD(DispCapabilities_NVC973_DISP_CAPABILITIES);
    struct DispCapabilities *PRIVATE_FIELD(DispCapabilities_NVCA73_DISP_CAPABILITIES);
    struct DispChannelDma *PRIVATE_FIELD(DispChannelDma_NVCA7B_WINDOW_IMM_CHANNEL_DMA);
    struct DispChannelDma *PRIVATE_FIELD(DispChannelDma_NVCA7D_CORE_CHANNEL_DMA);
    struct DispChannelDma *PRIVATE_FIELD(DispChannelDma_NVCA7E_WINDOW_CHANNEL_DMA);
    struct DispCapabilities *PRIVATE_FIELD(DispCapabilities_NVCB73_DISP_CAPABILITIES);
    struct DispChannelDma *PRIVATE_FIELD(DispChannelDma_NVCB7B_WINDOW_IMM_CHANNEL_DMA);
    struct DispChannelDma *PRIVATE_FIELD(DispChannelDma_NVCB7D_CORE_CHANNEL_DMA);
    struct DispChannelDma *PRIVATE_FIELD(DispChannelDma_NVCB7E_WINDOW_CHANNEL_DMA);
    struct DispSwObject *PRIVATE_FIELD(DispSwObject_GF100_DISP_SW);
    struct TimedSemaSwObject *PRIVATE_FIELD(TimedSemaSwObject_GF100_TIMED_SEMAPHORE_SW);
    struct DeferredApiObject *PRIVATE_FIELD(DeferredApiObject_NV50_DEFERRED_API_CLASS);
    struct UvmSwObject *PRIVATE_FIELD(UvmSwObject_GP100_UVM_SW);
    struct SoftwareMethodTest *PRIVATE_FIELD(SoftwareMethodTest_NV04_SOFTWARE_TEST);
    struct KernelCeContext *PRIVATE_FIELD(KernelCeContext_MAXWELL_DMA_COPY_A);
    struct KernelCeContext *PRIVATE_FIELD(KernelCeContext_PASCAL_DMA_COPY_A);
    struct KernelCeContext *PRIVATE_FIELD(KernelCeContext_TURING_DMA_COPY_A);
    struct KernelCeContext *PRIVATE_FIELD(KernelCeContext_AMPERE_DMA_COPY_A);
    struct KernelCeContext *PRIVATE_FIELD(KernelCeContext_AMPERE_DMA_COPY_B);
    struct KernelCeContext *PRIVATE_FIELD(KernelCeContext_HOPPER_DMA_COPY_A);
    struct KernelCeContext *PRIVATE_FIELD(KernelCeContext_BLACKWELL_DMA_COPY_A);
    struct KernelCeContext *PRIVATE_FIELD(KernelCeContext_BLACKWELL_DMA_COPY_B);
    struct NvdecContext *PRIVATE_FIELD(NvdecContext_NVB8B0_VIDEO_DECODER);
    struct NvdecContext *PRIVATE_FIELD(NvdecContext_NVC4B0_VIDEO_DECODER);
    struct NvdecContext *PRIVATE_FIELD(NvdecContext_NVC6B0_VIDEO_DECODER);
    struct NvdecContext *PRIVATE_FIELD(NvdecContext_NVC7B0_VIDEO_DECODER);
    struct NvdecContext *PRIVATE_FIELD(NvdecContext_NVC9B0_VIDEO_DECODER);
    struct NvdecContext *PRIVATE_FIELD(NvdecContext_NVCDB0_VIDEO_DECODER);
    struct NvdecContext *PRIVATE_FIELD(NvdecContext_NVCFB0_VIDEO_DECODER);
    struct NvjpgContext *PRIVATE_FIELD(NvjpgContext_NVB8D1_VIDEO_NVJPG);
    struct NvjpgContext *PRIVATE_FIELD(NvjpgContext_NVC4D1_VIDEO_NVJPG);
    struct NvjpgContext *PRIVATE_FIELD(NvjpgContext_NVC9D1_VIDEO_NVJPG);
    struct NvjpgContext *PRIVATE_FIELD(NvjpgContext_NVCDD1_VIDEO_NVJPG);
    struct NvjpgContext *PRIVATE_FIELD(NvjpgContext_NVCFD1_VIDEO_NVJPG);
    struct OfaContext *PRIVATE_FIELD(OfaContext_NVB8FA_VIDEO_OFA);
    struct OfaContext *PRIVATE_FIELD(OfaContext_NVC6FA_VIDEO_OFA);
    struct OfaContext *PRIVATE_FIELD(OfaContext_NVC7FA_VIDEO_OFA);
    struct OfaContext *PRIVATE_FIELD(OfaContext_NVC9FA_VIDEO_OFA);
    struct OfaContext *PRIVATE_FIELD(OfaContext_NVCDFA_VIDEO_OFA);
    struct OfaContext *PRIVATE_FIELD(OfaContext_NVCFFA_VIDEO_OFA);
    struct MsencContext *PRIVATE_FIELD(MsencContext_NVC4B7_VIDEO_ENCODER);
    struct MsencContext *PRIVATE_FIELD(MsencContext_NVB4B7_VIDEO_ENCODER);
    struct MsencContext *PRIVATE_FIELD(MsencContext_NVC7B7_VIDEO_ENCODER);
    struct MsencContext *PRIVATE_FIELD(MsencContext_NVC9B7_VIDEO_ENCODER);
    struct MsencContext *PRIVATE_FIELD(MsencContext_NVCFB7_VIDEO_ENCODER);
    struct Sec2Context *PRIVATE_FIELD(Sec2Context_HOPPER_SEC2_WORK_LAUNCH_A);
    struct KernelGraphicsObject *PRIVATE_FIELD(KernelGraphicsObject_AMPERE_A);
    struct KernelGraphicsObject *PRIVATE_FIELD(KernelGraphicsObject_AMPERE_COMPUTE_A);
    struct KernelGraphicsObject *PRIVATE_FIELD(KernelGraphicsObject_AMPERE_B);
    struct KernelGraphicsObject *PRIVATE_FIELD(KernelGraphicsObject_AMPERE_COMPUTE_B);
    struct KernelGraphicsObject *PRIVATE_FIELD(KernelGraphicsObject_ADA_A);
    struct KernelGraphicsObject *PRIVATE_FIELD(KernelGraphicsObject_ADA_COMPUTE_A);
    struct KernelGraphicsObject *PRIVATE_FIELD(KernelGraphicsObject_HOPPER_A);
    struct KernelGraphicsObject *PRIVATE_FIELD(KernelGraphicsObject_HOPPER_COMPUTE_A);
    struct KernelGraphicsObject *PRIVATE_FIELD(KernelGraphicsObject_BLACKWELL_A);
    struct KernelGraphicsObject *PRIVATE_FIELD(KernelGraphicsObject_BLACKWELL_COMPUTE_A);
    struct KernelGraphicsObject *PRIVATE_FIELD(KernelGraphicsObject_BLACKWELL_INLINE_TO_MEMORY_A);
    struct KernelGraphicsObject *PRIVATE_FIELD(KernelGraphicsObject_BLACKWELL_B);
    struct KernelGraphicsObject *PRIVATE_FIELD(KernelGraphicsObject_BLACKWELL_COMPUTE_B);
    struct KernelGraphicsObject *PRIVATE_FIELD(KernelGraphicsObject_FERMI_TWOD_A);
    struct KernelGraphicsObject *PRIVATE_FIELD(KernelGraphicsObject_KEPLER_INLINE_TO_MEMORY_B);
    struct KernelGraphicsObject *PRIVATE_FIELD(KernelGraphicsObject_TURING_A);
    struct KernelGraphicsObject *PRIVATE_FIELD(KernelGraphicsObject_TURING_COMPUTE_A);
    struct ContextDma *PRIVATE_FIELD(ContextDma_NV01_CONTEXT_DMA);
    struct Event *PRIVATE_FIELD(Event_NV01_EVENT);
    struct Event *PRIVATE_FIELD(Event_NV01_EVENT_OS_EVENT);
    struct Event *PRIVATE_FIELD(Event_NV01_EVENT_KERNEL_CALLBACK);
    struct Event *PRIVATE_FIELD(Event_NV01_EVENT_KERNEL_CALLBACK_EX);
    struct ConfidentialComputeApi *PRIVATE_FIELD(ConfidentialComputeApi_NV_CONFIDENTIAL_COMPUTE);
    struct KernelCcuApi *PRIVATE_FIELD(KernelCcuApi_NV_COUNTER_COLLECTION_UNIT);
    struct CeUtilsApi *PRIVATE_FIELD(CeUtilsApi_NV_CE_UTILS);
    struct LockStressObject *PRIVATE_FIELD(LockStressObject_LOCK_STRESS_OBJECT);
    struct LockTestRelaxedDupObject *PRIVATE_FIELD(LockTestRelaxedDupObject_LOCK_TEST_RELAXED_DUP_OBJECT);
};


// Metadata with per-class RTTI
struct NVOC_METADATA__NVOCFwdDeclHack {
    const struct NVOC_RTTI rtti;
};

#ifndef __NVOC_CLASS_NVOCFwdDeclHack_TYPEDEF__
#define __NVOC_CLASS_NVOCFwdDeclHack_TYPEDEF__
typedef struct NVOCFwdDeclHack NVOCFwdDeclHack;
#endif /* __NVOC_CLASS_NVOCFwdDeclHack_TYPEDEF__ */

#ifndef __nvoc_class_id_NVOCFwdDeclHack
#define __nvoc_class_id_NVOCFwdDeclHack 0x0d01f5
#endif /* __nvoc_class_id_NVOCFwdDeclHack */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_NVOCFwdDeclHack;

#define __staticCast_NVOCFwdDeclHack(pThis) \
    ((pThis)->__nvoc_pbase_NVOCFwdDeclHack)

#ifdef __nvoc_resource_fwd_decls_h_disabled
#define __dynamicCast_NVOCFwdDeclHack(pThis) ((NVOCFwdDeclHack*) NULL)
#else //__nvoc_resource_fwd_decls_h_disabled
#define __dynamicCast_NVOCFwdDeclHack(pThis) \
    ((NVOCFwdDeclHack*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(NVOCFwdDeclHack)))
#endif //__nvoc_resource_fwd_decls_h_disabled

NV_STATUS __nvoc_objCreateDynamic_NVOCFwdDeclHack(NVOCFwdDeclHack**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_NVOCFwdDeclHack(NVOCFwdDeclHack**, Dynamic*, NvU32);
#define __objCreate_NVOCFwdDeclHack(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_NVOCFwdDeclHack((ppNewObj), staticCast((pParent), Dynamic), (createFlags))

#undef PRIVATE_FIELD


#endif // RESOURCE_FWD_DECLS_H


#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_RESOURCE_FWD_DECLS_NVOC_H_
