#ifndef _G_RESOURCE_FWD_DECLS_NVOC_H_
#define _G_RESOURCE_FWD_DECLS_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "g_resource_fwd_decls_nvoc.h"

//
// This header is a temporary WAR for CORERM-3115
// When that RFE is implemented, we'll be able to generate these forward decls
// from resource_list.h directly
//
#ifndef RESOURCE_FWD_DECLS_H
#define RESOURCE_FWD_DECLS_H

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



// Allocatable resources
struct AccessCounterBuffer;

#ifndef __NVOC_CLASS_AccessCounterBuffer_TYPEDEF__
#define __NVOC_CLASS_AccessCounterBuffer_TYPEDEF__
typedef struct AccessCounterBuffer AccessCounterBuffer;
#endif /* __NVOC_CLASS_AccessCounterBuffer_TYPEDEF__ */

#ifndef __nvoc_class_id_AccessCounterBuffer
#define __nvoc_class_id_AccessCounterBuffer 0x1f0074
#endif /* __nvoc_class_id_AccessCounterBuffer */


struct KernelCeContext;

#ifndef __NVOC_CLASS_KernelCeContext_TYPEDEF__
#define __NVOC_CLASS_KernelCeContext_TYPEDEF__
typedef struct KernelCeContext KernelCeContext;
#endif /* __NVOC_CLASS_KernelCeContext_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelCeContext
#define __nvoc_class_id_KernelCeContext 0x2d0ee9
#endif /* __nvoc_class_id_KernelCeContext */


struct Channel;

#ifndef __NVOC_CLASS_Channel_TYPEDEF__
#define __NVOC_CLASS_Channel_TYPEDEF__
typedef struct Channel Channel;
#endif /* __NVOC_CLASS_Channel_TYPEDEF__ */

#ifndef __nvoc_class_id_Channel
#define __nvoc_class_id_Channel 0x781dc9
#endif /* __nvoc_class_id_Channel */


struct ConsoleMemory;

#ifndef __NVOC_CLASS_ConsoleMemory_TYPEDEF__
#define __NVOC_CLASS_ConsoleMemory_TYPEDEF__
typedef struct ConsoleMemory ConsoleMemory;
#endif /* __NVOC_CLASS_ConsoleMemory_TYPEDEF__ */

#ifndef __nvoc_class_id_ConsoleMemory
#define __nvoc_class_id_ConsoleMemory 0xaac69e
#endif /* __nvoc_class_id_ConsoleMemory */


struct ContextDma;

#ifndef __NVOC_CLASS_ContextDma_TYPEDEF__
#define __NVOC_CLASS_ContextDma_TYPEDEF__
typedef struct ContextDma ContextDma;
#endif /* __NVOC_CLASS_ContextDma_TYPEDEF__ */

#ifndef __nvoc_class_id_ContextDma
#define __nvoc_class_id_ContextDma 0x88441b
#endif /* __nvoc_class_id_ContextDma */


struct DebugBufferApi;

#ifndef __NVOC_CLASS_DebugBufferApi_TYPEDEF__
#define __NVOC_CLASS_DebugBufferApi_TYPEDEF__
typedef struct DebugBufferApi DebugBufferApi;
#endif /* __NVOC_CLASS_DebugBufferApi_TYPEDEF__ */

#ifndef __nvoc_class_id_DebugBufferApi
#define __nvoc_class_id_DebugBufferApi 0x5e7a1b
#endif /* __nvoc_class_id_DebugBufferApi */


struct DeferredApiObject;

#ifndef __NVOC_CLASS_DeferredApiObject_TYPEDEF__
#define __NVOC_CLASS_DeferredApiObject_TYPEDEF__
typedef struct DeferredApiObject DeferredApiObject;
#endif /* __NVOC_CLASS_DeferredApiObject_TYPEDEF__ */

#ifndef __nvoc_class_id_DeferredApiObject
#define __nvoc_class_id_DeferredApiObject 0x8ea933
#endif /* __nvoc_class_id_DeferredApiObject */


struct Device;

#ifndef __NVOC_CLASS_Device_TYPEDEF__
#define __NVOC_CLASS_Device_TYPEDEF__
typedef struct Device Device;
#endif /* __NVOC_CLASS_Device_TYPEDEF__ */

#ifndef __nvoc_class_id_Device
#define __nvoc_class_id_Device 0xe0ac20
#endif /* __nvoc_class_id_Device */


struct DiagApi;

#ifndef __NVOC_CLASS_DiagApi_TYPEDEF__
#define __NVOC_CLASS_DiagApi_TYPEDEF__
typedef struct DiagApi DiagApi;
#endif /* __NVOC_CLASS_DiagApi_TYPEDEF__ */

#ifndef __nvoc_class_id_DiagApi
#define __nvoc_class_id_DiagApi 0xaa3066
#endif /* __nvoc_class_id_DiagApi */


struct DispCapabilities;

#ifndef __NVOC_CLASS_DispCapabilities_TYPEDEF__
#define __NVOC_CLASS_DispCapabilities_TYPEDEF__
typedef struct DispCapabilities DispCapabilities;
#endif /* __NVOC_CLASS_DispCapabilities_TYPEDEF__ */

#ifndef __nvoc_class_id_DispCapabilities
#define __nvoc_class_id_DispCapabilities 0x99db3e
#endif /* __nvoc_class_id_DispCapabilities */


struct DispChannelDma;

#ifndef __NVOC_CLASS_DispChannelDma_TYPEDEF__
#define __NVOC_CLASS_DispChannelDma_TYPEDEF__
typedef struct DispChannelDma DispChannelDma;
#endif /* __NVOC_CLASS_DispChannelDma_TYPEDEF__ */

#ifndef __nvoc_class_id_DispChannelDma
#define __nvoc_class_id_DispChannelDma 0xfe3d2e
#endif /* __nvoc_class_id_DispChannelDma */


struct DispChannelPio;

#ifndef __NVOC_CLASS_DispChannelPio_TYPEDEF__
#define __NVOC_CLASS_DispChannelPio_TYPEDEF__
typedef struct DispChannelPio DispChannelPio;
#endif /* __NVOC_CLASS_DispChannelPio_TYPEDEF__ */

#ifndef __nvoc_class_id_DispChannelPio
#define __nvoc_class_id_DispChannelPio 0x10dec3
#endif /* __nvoc_class_id_DispChannelPio */


struct DispCommon;

#ifndef __NVOC_CLASS_DispCommon_TYPEDEF__
#define __NVOC_CLASS_DispCommon_TYPEDEF__
typedef struct DispCommon DispCommon;
#endif /* __NVOC_CLASS_DispCommon_TYPEDEF__ */

#ifndef __nvoc_class_id_DispCommon
#define __nvoc_class_id_DispCommon 0x41f4f2
#endif /* __nvoc_class_id_DispCommon */


struct DispSfUser;

#ifndef __NVOC_CLASS_DispSfUser_TYPEDEF__
#define __NVOC_CLASS_DispSfUser_TYPEDEF__
typedef struct DispSfUser DispSfUser;
#endif /* __NVOC_CLASS_DispSfUser_TYPEDEF__ */

#ifndef __nvoc_class_id_DispSfUser
#define __nvoc_class_id_DispSfUser 0xba7439
#endif /* __nvoc_class_id_DispSfUser */


struct DispSwObj;

#ifndef __NVOC_CLASS_DispSwObj_TYPEDEF__
#define __NVOC_CLASS_DispSwObj_TYPEDEF__
typedef struct DispSwObj DispSwObj;
#endif /* __NVOC_CLASS_DispSwObj_TYPEDEF__ */

#ifndef __nvoc_class_id_DispSwObj
#define __nvoc_class_id_DispSwObj 0x6aa5e2
#endif /* __nvoc_class_id_DispSwObj */


struct DispSwObject;

#ifndef __NVOC_CLASS_DispSwObject_TYPEDEF__
#define __NVOC_CLASS_DispSwObject_TYPEDEF__
typedef struct DispSwObject DispSwObject;
#endif /* __NVOC_CLASS_DispSwObject_TYPEDEF__ */

#ifndef __nvoc_class_id_DispSwObject
#define __nvoc_class_id_DispSwObject 0x99ad6d
#endif /* __nvoc_class_id_DispSwObject */


struct Event;

#ifndef __NVOC_CLASS_Event_TYPEDEF__
#define __NVOC_CLASS_Event_TYPEDEF__
typedef struct Event Event;
#endif /* __NVOC_CLASS_Event_TYPEDEF__ */

#ifndef __nvoc_class_id_Event
#define __nvoc_class_id_Event 0xa4ecfc
#endif /* __nvoc_class_id_Event */


struct EventBuffer;

#ifndef __NVOC_CLASS_EventBuffer_TYPEDEF__
#define __NVOC_CLASS_EventBuffer_TYPEDEF__
typedef struct EventBuffer EventBuffer;
#endif /* __NVOC_CLASS_EventBuffer_TYPEDEF__ */

#ifndef __nvoc_class_id_EventBuffer
#define __nvoc_class_id_EventBuffer 0x63502b
#endif /* __nvoc_class_id_EventBuffer */


struct FbSegment;

#ifndef __NVOC_CLASS_FbSegment_TYPEDEF__
#define __NVOC_CLASS_FbSegment_TYPEDEF__
typedef struct FbSegment FbSegment;
#endif /* __NVOC_CLASS_FbSegment_TYPEDEF__ */

#ifndef __nvoc_class_id_FbSegment
#define __nvoc_class_id_FbSegment 0x2d55be
#endif /* __nvoc_class_id_FbSegment */


struct FlaMemory;

#ifndef __NVOC_CLASS_FlaMemory_TYPEDEF__
#define __NVOC_CLASS_FlaMemory_TYPEDEF__
typedef struct FlaMemory FlaMemory;
#endif /* __NVOC_CLASS_FlaMemory_TYPEDEF__ */

#ifndef __nvoc_class_id_FlaMemory
#define __nvoc_class_id_FlaMemory 0xe61ee1
#endif /* __nvoc_class_id_FlaMemory */


struct FmSessionApi;

#ifndef __NVOC_CLASS_FmSessionApi_TYPEDEF__
#define __NVOC_CLASS_FmSessionApi_TYPEDEF__
typedef struct FmSessionApi FmSessionApi;
#endif /* __NVOC_CLASS_FmSessionApi_TYPEDEF__ */

#ifndef __nvoc_class_id_FmSessionApi
#define __nvoc_class_id_FmSessionApi 0xdfbd08
#endif /* __nvoc_class_id_FmSessionApi */


struct GenericEngineApi;

#ifndef __NVOC_CLASS_GenericEngineApi_TYPEDEF__
#define __NVOC_CLASS_GenericEngineApi_TYPEDEF__
typedef struct GenericEngineApi GenericEngineApi;
#endif /* __NVOC_CLASS_GenericEngineApi_TYPEDEF__ */

#ifndef __nvoc_class_id_GenericEngineApi
#define __nvoc_class_id_GenericEngineApi 0x4bc329
#endif /* __nvoc_class_id_GenericEngineApi */


struct GpuManagementApi;

#ifndef __NVOC_CLASS_GpuManagementApi_TYPEDEF__
#define __NVOC_CLASS_GpuManagementApi_TYPEDEF__
typedef struct GpuManagementApi GpuManagementApi;
#endif /* __NVOC_CLASS_GpuManagementApi_TYPEDEF__ */

#ifndef __nvoc_class_id_GpuManagementApi
#define __nvoc_class_id_GpuManagementApi 0x376305
#endif /* __nvoc_class_id_GpuManagementApi */


struct GraphicsContext;

#ifndef __NVOC_CLASS_GraphicsContext_TYPEDEF__
#define __NVOC_CLASS_GraphicsContext_TYPEDEF__
typedef struct GraphicsContext GraphicsContext;
#endif /* __NVOC_CLASS_GraphicsContext_TYPEDEF__ */

#ifndef __nvoc_class_id_GraphicsContext
#define __nvoc_class_id_GraphicsContext 0x954c97
#endif /* __nvoc_class_id_GraphicsContext */


struct GraphicsObject;

#ifndef __NVOC_CLASS_GraphicsObject_TYPEDEF__
#define __NVOC_CLASS_GraphicsObject_TYPEDEF__
typedef struct GraphicsObject GraphicsObject;
#endif /* __NVOC_CLASS_GraphicsObject_TYPEDEF__ */

#ifndef __nvoc_class_id_GraphicsObject
#define __nvoc_class_id_GraphicsObject 0x8cddfd
#endif /* __nvoc_class_id_GraphicsObject */


struct Griddisplayless;

#ifndef __NVOC_CLASS_Griddisplayless_TYPEDEF__
#define __NVOC_CLASS_Griddisplayless_TYPEDEF__
typedef struct Griddisplayless Griddisplayless;
#endif /* __NVOC_CLASS_Griddisplayless_TYPEDEF__ */

#ifndef __nvoc_class_id_Griddisplayless
#define __nvoc_class_id_Griddisplayless 0x3d03b2
#endif /* __nvoc_class_id_Griddisplayless */


struct Hdacodec;

#ifndef __NVOC_CLASS_Hdacodec_TYPEDEF__
#define __NVOC_CLASS_Hdacodec_TYPEDEF__
typedef struct Hdacodec Hdacodec;
#endif /* __NVOC_CLASS_Hdacodec_TYPEDEF__ */

#ifndef __nvoc_class_id_Hdacodec
#define __nvoc_class_id_Hdacodec 0xf59a20
#endif /* __nvoc_class_id_Hdacodec */


struct HostVgpuDeviceApi;

#ifndef __NVOC_CLASS_HostVgpuDeviceApi_TYPEDEF__
#define __NVOC_CLASS_HostVgpuDeviceApi_TYPEDEF__
typedef struct HostVgpuDeviceApi HostVgpuDeviceApi;
#endif /* __NVOC_CLASS_HostVgpuDeviceApi_TYPEDEF__ */

#ifndef __nvoc_class_id_HostVgpuDeviceApi
#define __nvoc_class_id_HostVgpuDeviceApi 0x4c4173
#endif /* __nvoc_class_id_HostVgpuDeviceApi */


struct HostVgpuDeviceApi_KERNEL;

#ifndef __NVOC_CLASS_HostVgpuDeviceApi_KERNEL_TYPEDEF__
#define __NVOC_CLASS_HostVgpuDeviceApi_KERNEL_TYPEDEF__
typedef struct HostVgpuDeviceApi_KERNEL HostVgpuDeviceApi_KERNEL;
#endif /* __NVOC_CLASS_HostVgpuDeviceApi_KERNEL_TYPEDEF__ */

#ifndef __nvoc_class_id_HostVgpuDeviceApi_KERNEL
#define __nvoc_class_id_HostVgpuDeviceApi_KERNEL 0xeb7e48
#endif /* __nvoc_class_id_HostVgpuDeviceApi_KERNEL */


struct I2cApi;

#ifndef __NVOC_CLASS_I2cApi_TYPEDEF__
#define __NVOC_CLASS_I2cApi_TYPEDEF__
typedef struct I2cApi I2cApi;
#endif /* __NVOC_CLASS_I2cApi_TYPEDEF__ */

#ifndef __nvoc_class_id_I2cApi
#define __nvoc_class_id_I2cApi 0xceb8f6
#endif /* __nvoc_class_id_I2cApi */


struct KernelChannel;

#ifndef __NVOC_CLASS_KernelChannel_TYPEDEF__
#define __NVOC_CLASS_KernelChannel_TYPEDEF__
typedef struct KernelChannel KernelChannel;
#endif /* __NVOC_CLASS_KernelChannel_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelChannel
#define __nvoc_class_id_KernelChannel 0x5d8d70
#endif /* __nvoc_class_id_KernelChannel */


struct KernelChannelGroupApi;

#ifndef __NVOC_CLASS_KernelChannelGroupApi_TYPEDEF__
#define __NVOC_CLASS_KernelChannelGroupApi_TYPEDEF__
typedef struct KernelChannelGroupApi KernelChannelGroupApi;
#endif /* __NVOC_CLASS_KernelChannelGroupApi_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelChannelGroupApi
#define __nvoc_class_id_KernelChannelGroupApi 0x2b5b80
#endif /* __nvoc_class_id_KernelChannelGroupApi */


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


struct KernelGraphicsObject;

#ifndef __NVOC_CLASS_KernelGraphicsObject_TYPEDEF__
#define __NVOC_CLASS_KernelGraphicsObject_TYPEDEF__
typedef struct KernelGraphicsObject KernelGraphicsObject;
#endif /* __NVOC_CLASS_KernelGraphicsObject_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelGraphicsObject
#define __nvoc_class_id_KernelGraphicsObject 0x097648
#endif /* __nvoc_class_id_KernelGraphicsObject */


struct KernelSMDebuggerSession;

#ifndef __NVOC_CLASS_KernelSMDebuggerSession_TYPEDEF__
#define __NVOC_CLASS_KernelSMDebuggerSession_TYPEDEF__
typedef struct KernelSMDebuggerSession KernelSMDebuggerSession;
#endif /* __NVOC_CLASS_KernelSMDebuggerSession_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelSMDebuggerSession
#define __nvoc_class_id_KernelSMDebuggerSession 0x4adc81
#endif /* __nvoc_class_id_KernelSMDebuggerSession */


struct MemoryFabric;

#ifndef __NVOC_CLASS_MemoryFabric_TYPEDEF__
#define __NVOC_CLASS_MemoryFabric_TYPEDEF__
typedef struct MemoryFabric MemoryFabric;
#endif /* __NVOC_CLASS_MemoryFabric_TYPEDEF__ */

#ifndef __nvoc_class_id_MemoryFabric
#define __nvoc_class_id_MemoryFabric 0x127499
#endif /* __nvoc_class_id_MemoryFabric */


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


struct MmuFaultBuffer;

#ifndef __NVOC_CLASS_MmuFaultBuffer_TYPEDEF__
#define __NVOC_CLASS_MmuFaultBuffer_TYPEDEF__
typedef struct MmuFaultBuffer MmuFaultBuffer;
#endif /* __NVOC_CLASS_MmuFaultBuffer_TYPEDEF__ */

#ifndef __nvoc_class_id_MmuFaultBuffer
#define __nvoc_class_id_MmuFaultBuffer 0x7e1829
#endif /* __nvoc_class_id_MmuFaultBuffer */


struct MpsApi;

#ifndef __NVOC_CLASS_MpsApi_TYPEDEF__
#define __NVOC_CLASS_MpsApi_TYPEDEF__
typedef struct MpsApi MpsApi;
#endif /* __NVOC_CLASS_MpsApi_TYPEDEF__ */

#ifndef __nvoc_class_id_MpsApi
#define __nvoc_class_id_MpsApi 0x22ce42
#endif /* __nvoc_class_id_MpsApi */


struct MsencContext;

#ifndef __NVOC_CLASS_MsencContext_TYPEDEF__
#define __NVOC_CLASS_MsencContext_TYPEDEF__
typedef struct MsencContext MsencContext;
#endif /* __NVOC_CLASS_MsencContext_TYPEDEF__ */

#ifndef __nvoc_class_id_MsencContext
#define __nvoc_class_id_MsencContext 0x88c92a
#endif /* __nvoc_class_id_MsencContext */


struct NoDeviceMemory;

#ifndef __NVOC_CLASS_NoDeviceMemory_TYPEDEF__
#define __NVOC_CLASS_NoDeviceMemory_TYPEDEF__
typedef struct NoDeviceMemory NoDeviceMemory;
#endif /* __NVOC_CLASS_NoDeviceMemory_TYPEDEF__ */

#ifndef __nvoc_class_id_NoDeviceMemory
#define __nvoc_class_id_NoDeviceMemory 0x6c0832
#endif /* __nvoc_class_id_NoDeviceMemory */


struct NpuResource;

#ifndef __NVOC_CLASS_NpuResource_TYPEDEF__
#define __NVOC_CLASS_NpuResource_TYPEDEF__
typedef struct NpuResource NpuResource;
#endif /* __NVOC_CLASS_NpuResource_TYPEDEF__ */

#ifndef __nvoc_class_id_NpuResource
#define __nvoc_class_id_NpuResource 0x4d1af2
#endif /* __nvoc_class_id_NpuResource */


struct NvdecContext;

#ifndef __NVOC_CLASS_NvdecContext_TYPEDEF__
#define __NVOC_CLASS_NvdecContext_TYPEDEF__
typedef struct NvdecContext NvdecContext;
#endif /* __NVOC_CLASS_NvdecContext_TYPEDEF__ */

#ifndef __nvoc_class_id_NvdecContext
#define __nvoc_class_id_NvdecContext 0x70d2be
#endif /* __nvoc_class_id_NvdecContext */


struct NvDispApi;

#ifndef __NVOC_CLASS_NvDispApi_TYPEDEF__
#define __NVOC_CLASS_NvDispApi_TYPEDEF__
typedef struct NvDispApi NvDispApi;
#endif /* __NVOC_CLASS_NvDispApi_TYPEDEF__ */

#ifndef __nvoc_class_id_NvDispApi
#define __nvoc_class_id_NvDispApi 0x36aa0b
#endif /* __nvoc_class_id_NvDispApi */


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


struct OsDescMemory;

#ifndef __NVOC_CLASS_OsDescMemory_TYPEDEF__
#define __NVOC_CLASS_OsDescMemory_TYPEDEF__
typedef struct OsDescMemory OsDescMemory;
#endif /* __NVOC_CLASS_OsDescMemory_TYPEDEF__ */

#ifndef __nvoc_class_id_OsDescMemory
#define __nvoc_class_id_OsDescMemory 0xb3dacd
#endif /* __nvoc_class_id_OsDescMemory */


struct UserLocalDescMemory;

#ifndef __NVOC_CLASS_UserLocalDescMemory_TYPEDEF__
#define __NVOC_CLASS_UserLocalDescMemory_TYPEDEF__
typedef struct UserLocalDescMemory UserLocalDescMemory;
#endif /* __NVOC_CLASS_UserLocalDescMemory_TYPEDEF__ */

#ifndef __nvoc_class_id_UserLocalDescMemory
#define __nvoc_class_id_UserLocalDescMemory 0x799456
#endif /* __nvoc_class_id_UserLocalDescMemory */


struct P2PApi;

#ifndef __NVOC_CLASS_P2PApi_TYPEDEF__
#define __NVOC_CLASS_P2PApi_TYPEDEF__
typedef struct P2PApi P2PApi;
#endif /* __NVOC_CLASS_P2PApi_TYPEDEF__ */

#ifndef __nvoc_class_id_P2PApi
#define __nvoc_class_id_P2PApi 0x3982b7
#endif /* __nvoc_class_id_P2PApi */


struct PerfBuffer;

#ifndef __NVOC_CLASS_PerfBuffer_TYPEDEF__
#define __NVOC_CLASS_PerfBuffer_TYPEDEF__
typedef struct PerfBuffer PerfBuffer;
#endif /* __NVOC_CLASS_PerfBuffer_TYPEDEF__ */

#ifndef __nvoc_class_id_PerfBuffer
#define __nvoc_class_id_PerfBuffer 0x4bc43b
#endif /* __nvoc_class_id_PerfBuffer */


struct PhysicalMemory;

#ifndef __NVOC_CLASS_PhysicalMemory_TYPEDEF__
#define __NVOC_CLASS_PhysicalMemory_TYPEDEF__
typedef struct PhysicalMemory PhysicalMemory;
#endif /* __NVOC_CLASS_PhysicalMemory_TYPEDEF__ */

#ifndef __nvoc_class_id_PhysicalMemory
#define __nvoc_class_id_PhysicalMemory 0x5fccf2
#endif /* __nvoc_class_id_PhysicalMemory */


struct PhysMemSubAlloc;

#ifndef __NVOC_CLASS_PhysMemSubAlloc_TYPEDEF__
#define __NVOC_CLASS_PhysMemSubAlloc_TYPEDEF__
typedef struct PhysMemSubAlloc PhysMemSubAlloc;
#endif /* __NVOC_CLASS_PhysMemSubAlloc_TYPEDEF__ */

#ifndef __nvoc_class_id_PhysMemSubAlloc
#define __nvoc_class_id_PhysMemSubAlloc 0x2351fc
#endif /* __nvoc_class_id_PhysMemSubAlloc */


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


struct RegisterMemory;

#ifndef __NVOC_CLASS_RegisterMemory_TYPEDEF__
#define __NVOC_CLASS_RegisterMemory_TYPEDEF__
typedef struct RegisterMemory RegisterMemory;
#endif /* __NVOC_CLASS_RegisterMemory_TYPEDEF__ */

#ifndef __nvoc_class_id_RegisterMemory
#define __nvoc_class_id_RegisterMemory 0x40d457
#endif /* __nvoc_class_id_RegisterMemory */


struct RemapperObject;

#ifndef __NVOC_CLASS_RemapperObject_TYPEDEF__
#define __NVOC_CLASS_RemapperObject_TYPEDEF__
typedef struct RemapperObject RemapperObject;
#endif /* __NVOC_CLASS_RemapperObject_TYPEDEF__ */

#ifndef __nvoc_class_id_RemapperObject
#define __nvoc_class_id_RemapperObject 0xfc96cb
#endif /* __nvoc_class_id_RemapperObject */


struct RgLineCallback;

#ifndef __NVOC_CLASS_RgLineCallback_TYPEDEF__
#define __NVOC_CLASS_RgLineCallback_TYPEDEF__
typedef struct RgLineCallback RgLineCallback;
#endif /* __NVOC_CLASS_RgLineCallback_TYPEDEF__ */

#ifndef __nvoc_class_id_RgLineCallback
#define __nvoc_class_id_RgLineCallback 0xa3ff1c
#endif /* __nvoc_class_id_RgLineCallback */


struct RmClientResource;

#ifndef __NVOC_CLASS_RmClientResource_TYPEDEF__
#define __NVOC_CLASS_RmClientResource_TYPEDEF__
typedef struct RmClientResource RmClientResource;
#endif /* __NVOC_CLASS_RmClientResource_TYPEDEF__ */

#ifndef __nvoc_class_id_RmClientResource
#define __nvoc_class_id_RmClientResource 0x37a701
#endif /* __nvoc_class_id_RmClientResource */


struct MIGConfigSession;

#ifndef __NVOC_CLASS_MIGConfigSession_TYPEDEF__
#define __NVOC_CLASS_MIGConfigSession_TYPEDEF__
typedef struct MIGConfigSession MIGConfigSession;
#endif /* __NVOC_CLASS_MIGConfigSession_TYPEDEF__ */

#ifndef __nvoc_class_id_MIGConfigSession
#define __nvoc_class_id_MIGConfigSession 0x36a941
#endif /* __nvoc_class_id_MIGConfigSession */


struct ComputeInstanceSubscription;

#ifndef __NVOC_CLASS_ComputeInstanceSubscription_TYPEDEF__
#define __NVOC_CLASS_ComputeInstanceSubscription_TYPEDEF__
typedef struct ComputeInstanceSubscription ComputeInstanceSubscription;
#endif /* __NVOC_CLASS_ComputeInstanceSubscription_TYPEDEF__ */

#ifndef __nvoc_class_id_ComputeInstanceSubscription
#define __nvoc_class_id_ComputeInstanceSubscription 0xd1f238
#endif /* __nvoc_class_id_ComputeInstanceSubscription */


struct MIGMonitorSession;

#ifndef __NVOC_CLASS_MIGMonitorSession_TYPEDEF__
#define __NVOC_CLASS_MIGMonitorSession_TYPEDEF__
typedef struct MIGMonitorSession MIGMonitorSession;
#endif /* __NVOC_CLASS_MIGMonitorSession_TYPEDEF__ */

#ifndef __nvoc_class_id_MIGMonitorSession
#define __nvoc_class_id_MIGMonitorSession 0x29e15c
#endif /* __nvoc_class_id_MIGMonitorSession */


struct GPUInstanceSubscription;

#ifndef __NVOC_CLASS_GPUInstanceSubscription_TYPEDEF__
#define __NVOC_CLASS_GPUInstanceSubscription_TYPEDEF__
typedef struct GPUInstanceSubscription GPUInstanceSubscription;
#endif /* __NVOC_CLASS_GPUInstanceSubscription_TYPEDEF__ */

#ifndef __nvoc_class_id_GPUInstanceSubscription
#define __nvoc_class_id_GPUInstanceSubscription 0x91fde7
#endif /* __nvoc_class_id_GPUInstanceSubscription */


struct SMDebuggerSession;

#ifndef __NVOC_CLASS_SMDebuggerSession_TYPEDEF__
#define __NVOC_CLASS_SMDebuggerSession_TYPEDEF__
typedef struct SMDebuggerSession SMDebuggerSession;
#endif /* __NVOC_CLASS_SMDebuggerSession_TYPEDEF__ */

#ifndef __nvoc_class_id_SMDebuggerSession
#define __nvoc_class_id_SMDebuggerSession 0x9afab7
#endif /* __nvoc_class_id_SMDebuggerSession */


struct SoftwareMethodTest;

#ifndef __NVOC_CLASS_SoftwareMethodTest_TYPEDEF__
#define __NVOC_CLASS_SoftwareMethodTest_TYPEDEF__
typedef struct SoftwareMethodTest SoftwareMethodTest;
#endif /* __NVOC_CLASS_SoftwareMethodTest_TYPEDEF__ */

#ifndef __nvoc_class_id_SoftwareMethodTest
#define __nvoc_class_id_SoftwareMethodTest 0xdea092
#endif /* __nvoc_class_id_SoftwareMethodTest */


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


struct SyncGpuBoost;

#ifndef __NVOC_CLASS_SyncGpuBoost_TYPEDEF__
#define __NVOC_CLASS_SyncGpuBoost_TYPEDEF__
typedef struct SyncGpuBoost SyncGpuBoost;
#endif /* __NVOC_CLASS_SyncGpuBoost_TYPEDEF__ */

#ifndef __nvoc_class_id_SyncGpuBoost
#define __nvoc_class_id_SyncGpuBoost 0xc7e30b
#endif /* __nvoc_class_id_SyncGpuBoost */


struct SyncpointMemory;

#ifndef __NVOC_CLASS_SyncpointMemory_TYPEDEF__
#define __NVOC_CLASS_SyncpointMemory_TYPEDEF__
typedef struct SyncpointMemory SyncpointMemory;
#endif /* __NVOC_CLASS_SyncpointMemory_TYPEDEF__ */

#ifndef __nvoc_class_id_SyncpointMemory
#define __nvoc_class_id_SyncpointMemory 0x529def
#endif /* __nvoc_class_id_SyncpointMemory */


struct SystemMemory;

#ifndef __NVOC_CLASS_SystemMemory_TYPEDEF__
#define __NVOC_CLASS_SystemMemory_TYPEDEF__
typedef struct SystemMemory SystemMemory;
#endif /* __NVOC_CLASS_SystemMemory_TYPEDEF__ */

#ifndef __nvoc_class_id_SystemMemory
#define __nvoc_class_id_SystemMemory 0x007a98
#endif /* __nvoc_class_id_SystemMemory */


struct ThirdPartyP2P;

#ifndef __NVOC_CLASS_ThirdPartyP2P_TYPEDEF__
#define __NVOC_CLASS_ThirdPartyP2P_TYPEDEF__
typedef struct ThirdPartyP2P ThirdPartyP2P;
#endif /* __NVOC_CLASS_ThirdPartyP2P_TYPEDEF__ */

#ifndef __nvoc_class_id_ThirdPartyP2P
#define __nvoc_class_id_ThirdPartyP2P 0x34d08b
#endif /* __nvoc_class_id_ThirdPartyP2P */


struct TimedSemaSwObject;

#ifndef __NVOC_CLASS_TimedSemaSwObject_TYPEDEF__
#define __NVOC_CLASS_TimedSemaSwObject_TYPEDEF__
typedef struct TimedSemaSwObject TimedSemaSwObject;
#endif /* __NVOC_CLASS_TimedSemaSwObject_TYPEDEF__ */

#ifndef __nvoc_class_id_TimedSemaSwObject
#define __nvoc_class_id_TimedSemaSwObject 0x335775
#endif /* __nvoc_class_id_TimedSemaSwObject */


struct TimerApi;

#ifndef __NVOC_CLASS_TimerApi_TYPEDEF__
#define __NVOC_CLASS_TimerApi_TYPEDEF__
typedef struct TimerApi TimerApi;
#endif /* __NVOC_CLASS_TimerApi_TYPEDEF__ */

#ifndef __nvoc_class_id_TimerApi
#define __nvoc_class_id_TimerApi 0xb13ac4
#endif /* __nvoc_class_id_TimerApi */


struct UserModeApi;

#ifndef __NVOC_CLASS_UserModeApi_TYPEDEF__
#define __NVOC_CLASS_UserModeApi_TYPEDEF__
typedef struct UserModeApi UserModeApi;
#endif /* __NVOC_CLASS_UserModeApi_TYPEDEF__ */

#ifndef __nvoc_class_id_UserModeApi
#define __nvoc_class_id_UserModeApi 0x6f57ec
#endif /* __nvoc_class_id_UserModeApi */


struct UvmChannelRetainer;

#ifndef __NVOC_CLASS_UvmChannelRetainer_TYPEDEF__
#define __NVOC_CLASS_UvmChannelRetainer_TYPEDEF__
typedef struct UvmChannelRetainer UvmChannelRetainer;
#endif /* __NVOC_CLASS_UvmChannelRetainer_TYPEDEF__ */

#ifndef __nvoc_class_id_UvmChannelRetainer
#define __nvoc_class_id_UvmChannelRetainer 0xa3f03a
#endif /* __nvoc_class_id_UvmChannelRetainer */


struct UvmSwObject;

#ifndef __NVOC_CLASS_UvmSwObject_TYPEDEF__
#define __NVOC_CLASS_UvmSwObject_TYPEDEF__
typedef struct UvmSwObject UvmSwObject;
#endif /* __NVOC_CLASS_UvmSwObject_TYPEDEF__ */

#ifndef __nvoc_class_id_UvmSwObject
#define __nvoc_class_id_UvmSwObject 0xc35503
#endif /* __nvoc_class_id_UvmSwObject */


struct VaSpaceApi;

#ifndef __NVOC_CLASS_VaSpaceApi_TYPEDEF__
#define __NVOC_CLASS_VaSpaceApi_TYPEDEF__
typedef struct VaSpaceApi VaSpaceApi;
#endif /* __NVOC_CLASS_VaSpaceApi_TYPEDEF__ */

#ifndef __nvoc_class_id_VaSpaceApi
#define __nvoc_class_id_VaSpaceApi 0xcd048b
#endif /* __nvoc_class_id_VaSpaceApi */


struct VblankCallback;

#ifndef __NVOC_CLASS_VblankCallback_TYPEDEF__
#define __NVOC_CLASS_VblankCallback_TYPEDEF__
typedef struct VblankCallback VblankCallback;
#endif /* __NVOC_CLASS_VblankCallback_TYPEDEF__ */

#ifndef __nvoc_class_id_VblankCallback
#define __nvoc_class_id_VblankCallback 0x4c1997
#endif /* __nvoc_class_id_VblankCallback */


struct VgpuApi;

#ifndef __NVOC_CLASS_VgpuApi_TYPEDEF__
#define __NVOC_CLASS_VgpuApi_TYPEDEF__
typedef struct VgpuApi VgpuApi;
#endif /* __NVOC_CLASS_VgpuApi_TYPEDEF__ */

#ifndef __nvoc_class_id_VgpuApi
#define __nvoc_class_id_VgpuApi 0x7774f5
#endif /* __nvoc_class_id_VgpuApi */


struct VgpuConfigApi;

#ifndef __NVOC_CLASS_VgpuConfigApi_TYPEDEF__
#define __NVOC_CLASS_VgpuConfigApi_TYPEDEF__
typedef struct VgpuConfigApi VgpuConfigApi;
#endif /* __NVOC_CLASS_VgpuConfigApi_TYPEDEF__ */

#ifndef __nvoc_class_id_VgpuConfigApi
#define __nvoc_class_id_VgpuConfigApi 0x4d560a
#endif /* __nvoc_class_id_VgpuConfigApi */


struct VideoMemory;

#ifndef __NVOC_CLASS_VideoMemory_TYPEDEF__
#define __NVOC_CLASS_VideoMemory_TYPEDEF__
typedef struct VideoMemory VideoMemory;
#endif /* __NVOC_CLASS_VideoMemory_TYPEDEF__ */

#ifndef __nvoc_class_id_VideoMemory
#define __nvoc_class_id_VideoMemory 0xed948f
#endif /* __nvoc_class_id_VideoMemory */


struct VirtualMemory;

#ifndef __NVOC_CLASS_VirtualMemory_TYPEDEF__
#define __NVOC_CLASS_VirtualMemory_TYPEDEF__
typedef struct VirtualMemory VirtualMemory;
#endif /* __NVOC_CLASS_VirtualMemory_TYPEDEF__ */

#ifndef __nvoc_class_id_VirtualMemory
#define __nvoc_class_id_VirtualMemory 0x2aea5c
#endif /* __nvoc_class_id_VirtualMemory */


struct VirtualMemoryRange;

#ifndef __NVOC_CLASS_VirtualMemoryRange_TYPEDEF__
#define __NVOC_CLASS_VirtualMemoryRange_TYPEDEF__
typedef struct VirtualMemoryRange VirtualMemoryRange;
#endif /* __NVOC_CLASS_VirtualMemoryRange_TYPEDEF__ */

#ifndef __nvoc_class_id_VirtualMemoryRange
#define __nvoc_class_id_VirtualMemoryRange 0x7032c6
#endif /* __nvoc_class_id_VirtualMemoryRange */


struct VmmuApi;

#ifndef __NVOC_CLASS_VmmuApi_TYPEDEF__
#define __NVOC_CLASS_VmmuApi_TYPEDEF__
typedef struct VmmuApi VmmuApi;
#endif /* __NVOC_CLASS_VmmuApi_TYPEDEF__ */

#ifndef __nvoc_class_id_VmmuApi
#define __nvoc_class_id_VmmuApi 0x40d73a
#endif /* __nvoc_class_id_VmmuApi */


struct ZbcApi;

#ifndef __NVOC_CLASS_ZbcApi_TYPEDEF__
#define __NVOC_CLASS_ZbcApi_TYPEDEF__
typedef struct ZbcApi ZbcApi;
#endif /* __NVOC_CLASS_ZbcApi_TYPEDEF__ */

#ifndef __nvoc_class_id_ZbcApi
#define __nvoc_class_id_ZbcApi 0x397ee3
#endif /* __nvoc_class_id_ZbcApi */



#endif // RESOURCE_FWD_DECLS_H


#ifdef __cplusplus
} // extern "C"
#endif
#endif // _G_RESOURCE_FWD_DECLS_NVOC_H_
