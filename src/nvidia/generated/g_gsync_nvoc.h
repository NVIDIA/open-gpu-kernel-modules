
#ifndef _G_GSYNC_NVOC_H_
#define _G_GSYNC_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 2006-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_gsync_nvoc.h"

#ifndef GSYNC_H_
#define GSYNC_H_

/* ------------------------ Includes --------------------------------------- */
#include "core/core.h"
#include "core/system.h"
#include "gpu/external_device/external_device.h"
#include "ctrl/ctrl0000/ctrl0000gsync.h"
#include "ctrl/ctrl30f1.h"
#include "class/cl30f1.h"

/* ------------------------ Types definitions ------------------------------ */
typedef enum {
    /* Following value is valid for all gsync devices */
    gsync_Connector_None =
        NV30F1_CTRL_GET_GSYNC_GPU_TOPOLOGY_NONE,

    /* For P2060 four connectors are index based i.e. 0 to 3 */
} GSYNCCONNECTOR;

typedef enum {
    gsync_SyncPolarity_RisingEdge =
        NV30F1_CTRL_GSYNC_SET_CONTROL_SYNC_POLARITY_RISING_EDGE,
    gsync_SyncPolarity_FallingEdge =
        NV30F1_CTRL_GSYNC_SET_CONTROL_SYNC_POLARITY_FALLING_EDGE,
    gsync_SyncPolarity_BothEdges =
        NV30F1_CTRL_GSYNC_SET_CONTROL_SYNC_POLARITY_BOTH_EDGES
} GSYNCSYNCPOLARITY;

typedef enum {
    gsync_VideoMode_None =
        NV30F1_CTRL_GSYNC_SET_CONTROL_VIDEO_MODE_NONE,
    gsync_VideoMode_TTL =
        NV30F1_CTRL_GSYNC_SET_CONTROL_VIDEO_MODE_TTL,
    gsync_VideoMode_NTSCPALSECAM =
        NV30F1_CTRL_GSYNC_SET_CONTROL_VIDEO_MODE_NTSCPALSECAM,
    gsync_VideoMode_HDTV =
        NV30F1_CTRL_GSYNC_SET_CONTROL_VIDEO_MODE_HDTV,

    /* Following value is valid from P2060 only */
    gsync_VideoMode_COMPOSITE =
        NV30F1_CTRL_GSYNC_GET_CONTROL_VIDEO_MODE_COMPOSITE
} GSYNCVIDEOMODE;

typedef enum {
    gsync_Signal_RJ45_0     = 0,
    gsync_Signal_RJ45_1     = 1,
    gsync_Signal_House      = 2,
    gsync_Signal_Supported  = 3
} GSYNCSYNCSIGNAL;

typedef enum {
    gsync_Status_Refresh             = 0,
    gsync_Status_HouseSyncIncoming   = 1,
    gsync_Status_bSyncReady          = 2,
    gsync_Status_bSwapReady          = 3,
    gsync_Status_bTiming             = 4,
    gsync_Status_bStereoSync         = 5,
    gsync_Status_bHouseSync          = 6,
    gsync_Status_bPort0Input         = 7,
    gsync_Status_bPort1Input         = 8,
    gsync_Status_bPort0Ethernet      = 9,
    gsync_Status_bPort1Ethernet      = 10,
    gsync_Status_UniversalFrameCount = 11,
    gsync_Status_bInternalSlave      = 12,
} GSYNCSTATUS;

typedef enum {
    refRead            = 0,
    refFetchGet        = 1,
    refSetCommit       = 2,
} REFTYPE;

// Pre-define to use OBJGSYNC references in API calls
typedef struct _def_gsync OBJGSYNC;

typedef NV30F1_CTRL_GSYNC_GET_OPTIMIZED_TIMING_PARAMS GSYNCTIMINGPARAMS;
typedef NV30F1_CTRL_GSYNC_GET_CAPS_PARAMS             GSYNCCAPSPARAMS;

typedef NV_STATUS GsyncShutdownProvider(NvU32);

typedef NvBool    GsyncGpuCanBeMaster    (struct OBJGPU *, PDACEXTERNALDEVICE);
typedef NV_STATUS GsyncGetSyncPolarity   (struct OBJGPU *, PDACEXTERNALDEVICE, GSYNCSYNCPOLARITY *);
typedef NV_STATUS GsyncSetSyncPolarity   (struct OBJGPU *, PDACEXTERNALDEVICE, GSYNCSYNCPOLARITY);
typedef NV_STATUS GsyncGetVideoMode      (struct OBJGPU *, PDACEXTERNALDEVICE, GSYNCVIDEOMODE*);
typedef NV_STATUS GsyncSetVideoMode      (struct OBJGPU *, PDACEXTERNALDEVICE, GSYNCVIDEOMODE);
typedef NV_STATUS GsyncGetNSync          (struct OBJGPU *, PDACEXTERNALDEVICE, NvU32 *);
typedef NV_STATUS GsyncSetNSync          (struct OBJGPU *, PDACEXTERNALDEVICE, NvU32);
typedef NV_STATUS GsyncGetSyncSkew       (struct OBJGPU *, PDACEXTERNALDEVICE, NvU32 *);
typedef NV_STATUS GsyncSetSyncSkew       (struct OBJGPU *, PDACEXTERNALDEVICE, NvU32);
typedef NV_STATUS GsyncGetUseHouse       (struct OBJGPU *, PDACEXTERNALDEVICE, NvU32 *);
typedef NV_STATUS GsyncSetUseHouse       (struct OBJGPU *, PDACEXTERNALDEVICE, NvU32);
typedef NV_STATUS GsyncGetSyncStartDelay (struct OBJGPU *, PDACEXTERNALDEVICE, NvU32 *);
typedef NV_STATUS GsyncSetSyncStartDelay (struct OBJGPU *, PDACEXTERNALDEVICE, NvU32);
typedef NV_STATUS GsyncGetEmitTestSignal (struct OBJGPU *, PDACEXTERNALDEVICE, NvU32 *);
typedef NV_STATUS GsyncSetEmitTestSignal (struct OBJGPU *, PDACEXTERNALDEVICE, NvU32);
typedef NV_STATUS GsyncGetInterlaceMode  (struct OBJGPU *, PDACEXTERNALDEVICE, NvU32 *);
typedef NV_STATUS GsyncSetInterlaceMode  (struct OBJGPU *, PDACEXTERNALDEVICE, NvU32);
typedef NV_STATUS GsyncRefSwapBarrier    (struct OBJGPU *, PDACEXTERNALDEVICE, REFTYPE, NvBool *);
typedef NV_STATUS GsyncRefSignal         (struct OBJGPU *, PDACEXTERNALDEVICE, REFTYPE, GSYNCSYNCSIGNAL, NvBool TestRate, NvU32 *);
typedef NV_STATUS GsyncRefMaster         (struct OBJGPU *, OBJGSYNC *, REFTYPE, NvU32 *, NvU32 *, NvBool, NvBool);
typedef NV_STATUS GsyncRefSlaves         (struct OBJGPU *, PDACEXTERNALDEVICE, REFTYPE, NvU32 *, NvU32 *);
typedef NV_STATUS GsyncGetCplStatus      (struct OBJGPU *, PDACEXTERNALDEVICE, GSYNCSTATUS, NvU32 *);
typedef NV_STATUS GsyncSetWatchdog       (struct OBJGPU *, PDACEXTERNALDEVICE, NvU32);
typedef NV_STATUS GsyncGetRevision       (struct OBJGPU *, OBJGSYNC *, GSYNCCAPSPARAMS *);
typedef NV_STATUS GsyncRefMasterable     (struct OBJGPU *, PDACEXTERNALDEVICE, REFTYPE, NvU32 *, NvU32 *);
typedef NV_STATUS GsyncGetStereoLockMode (struct OBJGPU *, PDACEXTERNALDEVICE, NvU32 *);
typedef NV_STATUS GsyncSetStereoLockMode (struct OBJGPU *, PDACEXTERNALDEVICE, NvU32);
typedef NV_STATUS GsyncOptimizeTiming    (struct OBJGPU *, GSYNCTIMINGPARAMS *);
typedef NV_STATUS GsyncSetMosaic         (struct OBJGPU *, PDACEXTERNALDEVICE, NV30F1_CTRL_GSYNC_SET_LOCAL_SYNC_PARAMS *);
typedef NV_STATUS GsyncConfigFlashGsync  (struct OBJGPU *, PDACEXTERNALDEVICE, NvU32);
typedef NV_STATUS GsyncGetHouseSyncMode  (struct OBJGPU *, PDACEXTERNALDEVICE, NvU8*);
typedef NV_STATUS GsyncSetHouseSyncMode  (struct OBJGPU *, PDACEXTERNALDEVICE, NvU8);
typedef NV_STATUS GsyncGetMulDiv         (struct OBJGPU *, DACEXTERNALDEVICE *, NV30F1_CTRL_GSYNC_MULTIPLY_DIVIDE_SETTINGS *);
typedef NV_STATUS GsyncSetMulDiv         (struct OBJGPU *, DACEXTERNALDEVICE *, NV30F1_CTRL_GSYNC_MULTIPLY_DIVIDE_SETTINGS *);
typedef NV_STATUS GsyncGetVRR            (struct OBJGPU *, DACEXTERNALDEVICE *, NvU32 *);
typedef NV_STATUS GsyncSetVRR            (struct OBJGPU *, DACEXTERNALDEVICE *, NvU32);
typedef NV_STATUS GsyncSetRasterSyncDecodeMode (struct OBJGPU *, DACEXTERNALDEVICE *);

typedef struct GSYNC_HAL_IFACES {

    GsyncGpuCanBeMaster     *gsyncGpuCanBeMaster;
    GsyncGetSyncPolarity    *gsyncGetSyncPolarity;
    GsyncSetSyncPolarity    *gsyncSetSyncPolarity;
    GsyncGetVideoMode       *gsyncGetVideoMode;
    GsyncSetVideoMode       *gsyncSetVideoMode;
    GsyncGetNSync           *gsyncGetNSync;
    GsyncSetNSync           *gsyncSetNSync;
    GsyncGetSyncSkew        *gsyncGetSyncSkew;
    GsyncSetSyncSkew        *gsyncSetSyncSkew;
    GsyncGetUseHouse        *gsyncGetUseHouse;
    GsyncSetUseHouse        *gsyncSetUseHouse;
    GsyncGetSyncStartDelay  *gsyncGetSyncStartDelay;
    GsyncSetSyncStartDelay  *gsyncSetSyncStartDelay;
    GsyncGetEmitTestSignal  *gsyncGetEmitTestSignal;
    GsyncSetEmitTestSignal  *gsyncSetEmitTestSignal;
    GsyncGetInterlaceMode   *gsyncGetInterlaceMode;
    GsyncSetInterlaceMode   *gsyncSetInterlaceMode;
    GsyncRefSwapBarrier     *gsyncRefSwapBarrier;
    GsyncRefSignal          *gsyncRefSignal;
    GsyncRefMaster          *gsyncRefMaster;
    GsyncRefSlaves          *gsyncRefSlaves;
    GsyncGetCplStatus       *gsyncGetCplStatus;
    GsyncSetWatchdog        *gsyncSetWatchdog;
    GsyncGetRevision        *gsyncGetRevision;
    GsyncRefMasterable      *gsyncRefMasterable;
    GsyncOptimizeTiming     *gsyncOptimizeTiming;
    GsyncGetStereoLockMode  *gsyncGetStereoLockMode;
    GsyncSetStereoLockMode  *gsyncSetStereoLockMode;
    GsyncSetMosaic          *gsyncSetMosaic;
    GsyncConfigFlashGsync   *gsyncConfigFlashGsync;
    GsyncGetHouseSyncMode   *gsyncGetHouseSyncMode;
    GsyncSetHouseSyncMode   *gsyncSetHouseSyncMode;
    GsyncGetMulDiv          *gsyncGetMulDiv;
    GsyncSetMulDiv          *gsyncSetMulDiv;
    GsyncGetVRR             *gsyncGetVRR;
    GsyncSetVRR             *gsyncSetVRR;
    GsyncSetRasterSyncDecodeMode *gsyncSetRasterSyncDecodeMode;

} GSYNC_HAL_IFACES;

typedef struct _def_gsync {
    NvU32 gsyncId;
    NvU32 gpuCount;
    NvU32 connectorCount;
    struct {
        NvU32 gpuId;
        NvU32 connector;
        NvU32 proxyGpuId;
    } gpus[NV30F1_CTRL_MAX_GPUS_PER_GSYNC];

    PDACEXTERNALDEVICE pExtDev;

    //
    // Set for the gsync objects lifetime until
    // any gsyncSetControlWatchdog has come in
    // as this indicated client's are taking care
    // and don't want any automatic solution.
    //
    NvBool bAutomaticWatchdogScheduling;
    NvBool bDoEventFiltering;

    // Bitmask of Masterable GPU connectors.
    NvU8 masterableGpuConnectors;

    // gsync hal
    GSYNC_HAL_IFACES gsyncHal;

} OBJGSYNC;

/* ------------------------ Macros & Defines ------------------------------- */
#define FLIPLOCK_LSR_MIN_TIME_FOR_SAWP_BARRIER_NV50  0x3FF //max LSR_MIN_TIME value for nv50
#define FLIPLOCK_LSR_MIN_TIME_FOR_SAWP_BARRIER_GF100 0x3FF //max LSR_MIN_TIME value for gf100
#define FLIPLOCK_LSR_MIN_TIME_FOR_SAWP_BARRIER_V02   0x3FF //max LSR_MIN_TIME value for gf11x+


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_GSYNC_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__OBJGSYNCMGR;
struct NVOC_METADATA__Object;


struct OBJGSYNCMGR {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__OBJGSYNCMGR *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct Object __nvoc_base_Object;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super
    struct OBJGSYNCMGR *__nvoc_pbase_OBJGSYNCMGR;    // gsyncmgr

    // Data members
    NvU32 gsyncCount;
    OBJGSYNC gsyncTable[4];
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__OBJGSYNCMGR {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__Object metadata__Object;
};

#ifndef __NVOC_CLASS_OBJGSYNCMGR_TYPEDEF__
#define __NVOC_CLASS_OBJGSYNCMGR_TYPEDEF__
typedef struct OBJGSYNCMGR OBJGSYNCMGR;
#endif /* __NVOC_CLASS_OBJGSYNCMGR_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJGSYNCMGR
#define __nvoc_class_id_OBJGSYNCMGR 0xd07fd0
#endif /* __nvoc_class_id_OBJGSYNCMGR */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJGSYNCMGR;

#define __staticCast_OBJGSYNCMGR(pThis) \
    ((pThis)->__nvoc_pbase_OBJGSYNCMGR)

#ifdef __nvoc_gsync_h_disabled
#define __dynamicCast_OBJGSYNCMGR(pThis) ((OBJGSYNCMGR*) NULL)
#else //__nvoc_gsync_h_disabled
#define __dynamicCast_OBJGSYNCMGR(pThis) \
    ((OBJGSYNCMGR*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(OBJGSYNCMGR)))
#endif //__nvoc_gsync_h_disabled

NV_STATUS __nvoc_objCreateDynamic_OBJGSYNCMGR(OBJGSYNCMGR**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_OBJGSYNCMGR(OBJGSYNCMGR**, Dynamic*, NvU32);
#define __objCreate_OBJGSYNCMGR(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_OBJGSYNCMGR((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros
#define gsyncmgrIsFirmwareGPUMismatch(pGpu, pGsync) gsyncmgrIsFirmwareGPUMismatch_STATIC_DISPATCH(pGpu, pGsync)
#define gsyncmgrIsFirmwareGPUMismatch_HAL(pGpu, pGsync) gsyncmgrIsFirmwareGPUMismatch_STATIC_DISPATCH(pGpu, pGsync)

// Dispatch functions
NvBool gsyncmgrIsFirmwareGPUMismatch_GB100(struct OBJGPU *pGpu, OBJGSYNC *pGsync);

static inline NvBool gsyncmgrIsFirmwareGPUMismatch_4a4dee(struct OBJGPU *pGpu, OBJGSYNC *pGsync) {
    return 0;
}

NvBool gsyncmgrIsFirmwareGPUMismatch_STATIC_DISPATCH(struct OBJGPU *pGpu, OBJGSYNC *pGsync);

NV_STATUS gsyncmgrConstruct_IMPL(struct OBJGSYNCMGR *arg_pGsyncmgr);

#define __nvoc_gsyncmgrConstruct(arg_pGsyncmgr) gsyncmgrConstruct_IMPL(arg_pGsyncmgr)
void gsyncmgrDestruct_IMPL(struct OBJGSYNCMGR *pGsyncmgr);

#define __nvoc_gsyncmgrDestruct(pGsyncmgr) gsyncmgrDestruct_IMPL(pGsyncmgr)
#undef PRIVATE_FIELD



NV_STATUS   gsyncGetAttachedIds(NV0000_CTRL_GSYNC_GET_ATTACHED_IDS_PARAMS *);
NV_STATUS   gsyncGetIdInfo(NV0000_CTRL_GSYNC_GET_ID_INFO_PARAMS *);
NV_STATUS   gsyncAttachGpu(PDACEXTERNALDEVICE pExtDev, struct OBJGPU *pGpu,
                           GSYNCCONNECTOR con, struct OBJGPU *pProxyGpu,
                           DAC_EXTERNAL_DEVICES externalDevice);
NV_STATUS   gsyncRemoveGpu(struct OBJGPU *pGpu);
NvBool      gsyncIsInstanceValid(NvU32 gsyncInst);
struct OBJGPU     *gsyncGetMasterableGpuByInstance(NvU32 gsyncInst);
NV_STATUS   gsyncSignalServiceRequested(NvU32 gsyncInst, NvU32 eventFlags, NvU32 iface);
NvU32       gsyncGetGsyncInstance(struct OBJGPU *pGpu);
NvU32       gsyncFilterEvents(NvU32, NvU32);
NvU32       gsyncConvertNewEventToOldEventNum(NvU32);
NvBool      gsyncAreAllGpusInConfigAttachedToSameGsyncBoard(struct OBJGPU **pGpus, NvU32 gpuCount);

OBJGSYNC   *gsyncmgrGetGsync(struct OBJGPU *);

#ifdef DEBUG
void        gsyncDbgPrintGsyncEvents(NvU32 events, NvU32 iface);
#else
#define     gsyncDbgPrintGsyncEvents(events, iface)
#endif

#endif // GSYNC_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_GSYNC_NVOC_H_
