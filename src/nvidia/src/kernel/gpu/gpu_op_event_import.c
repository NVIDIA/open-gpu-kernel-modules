/*
 * SPDX-FileCopyrightText: Copyright (c) 2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "gpu/gpu_op_event_import.h"

#include "gpu/gpu.h"
#include "nvport/nvport.h"
#include "utils/nvassert.h"
#include "rmapi/rmapi.h"
#include "rmapi/rs_utils.h"
#include "os/workitem.h"                         // osQueueWorkItem + OsQueueWorkItemFlags
#include "gpu/mem_mgr/heap.h"                    // HEAP_OWNER_RM_CLIENT_GENERIC
#include "rmapi/rmapi_utils.h"                   // rmapiutilAllocClientAndDeviceHandles
#include "class/cl0005.h"                        // NV01_EVENT_KERNEL_CALLBACK_EX
#include "class/cl003e.h"                        // NV01_MEMORY_SYSTEM
#include "class/cl90cd.h"                        // NV_EVENT_BUFFER_HEADER, alloc params
#include "class/cl90d0.h"                        // NV_OPERATIONAL_EVENT_BUFFER_BIND, scope/level/severity macros
#include "ctrl/ctrl2080/ctrl2080gpu.h"           // NV2080 GPU GID flags
#include "ctrl/ctrl90cd.h"                       // ENABLE_EVENTS ctrl + KEEP_OLDEST flag
#include "diagnostics/op_event.h"
#include "diagnostics/op_event_log.h"
#include "diagnostics/import_event_group.h"
#include "gpu/gpu_op_event.h"
#include "nvoc/event_group.h"
#include "nvoc/event_bus.h"

#define GOEIMPORT_RECORD_SIZE                                          \
    NV_MAX(sizeof(NV_GPU_OPERATIONAL_EVENT_RECORD),                    \
           sizeof(NV_OPERATIONAL_EVENT_CPER_RECORD))
#define GOEIMPORT_RECORD_COUNT             256U
#define GOEIMPORT_VARDATA_BUFFER_SIZE      (64U * 1024U)
#define GOEIMPORT_RECORDS_FREE_THRESHOLD    (GOEIMPORT_RECORD_COUNT - 1U)
#define GOEIMPORT_VARDATA_FREE_THRESHOLD   (4U  * 1024U)

static void _goeimportPostEventCallback(void *pArg, void *pData,
                                           NvHandle hEvent, NvU32 data,
                                           NvU32 status);
static void _goeimportPublishWorkitem(NvU32 gpuInstance, void *pArg);
static void _goeimportTearDownPartial(OBJGPU *pGpu, GpuOpEventImport *pImporter);
static void _goeimportAbortSpan(GpuOpEventImport *pImporter);

// ---------------- Raw-passthrough wire context machinery -----------------

//
// Wrapper attached to a reconstructed GpuOperationalEvent for each wire
// context. Owns the original bytes verbatim so the renderer can re-emit
// them to consumer-side 0x90d0 binds with no per-contextType logic.
// bOwnedByEvent = NV_TRUE -> opevtDestruct walks pContextList and frees
// the wrapper at live-event teardown.
//
typedef struct
{
    EventContextHeader header;
    NvU16              contextType;
    NvU16              dataFormatVersion;
    NvU32              payloadBytes;
    NvU8               payload[1];   // flex tail of payloadBytes
} GoeImportRawCtx;

static NV_STATUS
_goeimportRawSerializeToEventBufferGoeCtx
(
    EventContextHeader               *pCtxHeader,
    NV_OPERATIONAL_EVENT_CONTEXT     *pOutput,
    NvU32                             outputSize,
    NvU32                            *pBytesWritten
)
{
    GoeImportRawCtx *pRaw = (GoeImportRawCtx *)pCtxHeader;
    NvU32            totalSize;

    if (pRaw->payloadBytes > NV_U32_MAX - sizeof(*pOutput))
        return NV_ERR_INVALID_ARGUMENT;

    totalSize       = sizeof(*pOutput) + pRaw->payloadBytes;
    *pBytesWritten  = totalSize;

    if (pOutput == NULL)
    {
        NV_ASSERT_OR_RETURN(outputSize == 0, NV_ERR_INVALID_ARGUMENT);
        return NV_OK;
    }

    if (outputSize < totalSize)
        return NV_ERR_BUFFER_TOO_SMALL;

    pOutput->contextType       = pRaw->contextType;
    pOutput->dataFormatVersion = pRaw->dataFormatVersion;
    pOutput->dataSize          = pRaw->payloadBytes;

    if (pRaw->payloadBytes != 0)
        portMemCopy((NvU8 *)(pOutput + 1), pRaw->payloadBytes,
                    pRaw->payload, pRaw->payloadBytes);

    return NV_OK;
}

//
// Re-emit the wrapped wire context into a CPER NVIDIA-event section, preserving
// the producer's contextType / dataFormatVersion.
//
static NV_STATUS
_goeimportRawSerializeToCperCtx
(
    EventContextHeader             *pCtxHeader,
    NV_CPER_NV_EVENT_SECTION_STATE *pState
)
{
    GoeImportRawCtx *pRaw  = (GoeImportRawCtx *)pCtxHeader;
    void            *pData = NULL;
    NV_STATUS        status;

    if (pRaw->payloadBytes == 0)
        return NV_OK;

    status = cperNvidiaEventSectionAddContext(pState, pRaw->contextType,
                                              pRaw->dataFormatVersion,
                                              pRaw->payloadBytes, &pData);
    if (status != NV_OK)
        return status;

    portMemCopy(pData, pRaw->payloadBytes, pRaw->payload, pRaw->payloadBytes);
    return NV_OK;
}

//
// Renderer dereferences pCtx->pOutputAdapters directly, so any imported
// contextType is acceptable; both serializers re-emit the stored wire bytes
// verbatim. OsLog emitters are NULL (opaque imported contexts have no XID /
// CPER OS-log representation).
//
static const EventContextOutputAdapters g_goeImportRawAdapters =
{
    .serializeToEventBufferOpEventCtx = _goeimportRawSerializeToEventBufferGoeCtx,
    .serializeToCperEventCtx          = _goeimportRawSerializeToCperCtx,
    .emitToOsLogXid                   = NULL,
    .emitToOsLogCper                  = NULL,
};

//
// Walk a vardata window of back-to-back NV_OPERATIONAL_EVENT_CONTEXT records, wrap each in a heap
// GoeImportRawCtx, attach via opevtAddContext. Partially-attached nodes remain on pContextList on
// error; opevtDestruct frees them when the caller objDeletes pEvent (bOwnedByEvent = NV_TRUE).
//
static NV_STATUS
_goeimportBuildContextChain
(
    GpuOperationalEvent *pEvent,
    const NvU8          *pVardata,
    NvU32                vardataLen
)
{
    NvU32 offset = 0;

    while (offset < vardataLen)
    {
        const NV_OPERATIONAL_EVENT_CONTEXT *pWireCtx;
        NvU32                               recordSize;
        NvU32                               recordSizeAligned;
        NvU32                               wrapperTailBytes;
        NvU32                               allocBytes;
        GoeImportRawCtx                    *pWrap;

        if (vardataLen - offset < sizeof(*pWireCtx))
            return NV_ERR_INVALID_STATE;

        pWireCtx = (const NV_OPERATIONAL_EVENT_CONTEXT *)(pVardata + offset);

        if (pWireCtx->dataSize > vardataLen - offset - sizeof(*pWireCtx))
            return NV_ERR_INVALID_STATE;

        recordSize        = (NvU32)sizeof(*pWireCtx) + pWireCtx->dataSize;
        recordSizeAligned = NV_ALIGN_UP(recordSize, NV_EVENT_VARDATA_GRANULARITY);

        //
        // header.dataSize must be the full wrapper tail so the retained-log deep-copy captures
        // everything past the embedded EventContextHeader; an undercount truncates the imported
        // context. allocBytes uses NV_OFFSETOF(payload) so implicit tail padding can't perturb the
        // size.
        //
        wrapperTailBytes = NV_OFFSETOF(GoeImportRawCtx, payload) -
                           NV_OFFSETOF(GoeImportRawCtx, contextType) +
                           pWireCtx->dataSize;
        allocBytes       = NV_OFFSETOF(GoeImportRawCtx, payload) + pWireCtx->dataSize;

        pWrap = portMemAllocNonPaged(allocBytes);
        if (pWrap == NULL)
            return NV_ERR_NO_MEMORY;

        portMemSet(pWrap, 0, allocBytes);
        pWrap->header.pNext            = NULL;
        pWrap->header.pOutputAdapters  = &g_goeImportRawAdapters;
        pWrap->header.dataSize         = wrapperTailBytes;
        pWrap->header.bOwnedByEvent    = NV_TRUE;
        pWrap->contextType             = pWireCtx->contextType;
        pWrap->dataFormatVersion       = pWireCtx->dataFormatVersion;
        pWrap->payloadBytes            = pWireCtx->dataSize;
        if (pWireCtx->dataSize != 0)
            portMemCopy(pWrap->payload, pWireCtx->dataSize,
                        (const NvU8 *)(pWireCtx + 1), pWireCtx->dataSize);

        opevtAddContext(staticCast(pEvent, OperationalEvent), &pWrap->header);
        offset += recordSizeAligned;
    }

    return NV_OK;
}

// --------------------------- NVOC class lifecycle --------------------------

NV_STATUS
goeimportConstruct_IMPL
(
    GpuOpEventImport *pImporter,
    OBJGPU           *pGpu
)
{
    GoeImportWorkerParams *pParams;

    NV_ASSERT_OR_RETURN(pImporter != NULL, NV_ERR_INVALID_POINTER);
    NV_ASSERT_OR_RETURN(pGpu != NULL, NV_ERR_INVALID_POINTER);

    // NVOC zero-inits storage; only the back-pointer + sentinel need explicit init.
    pImporter->pGpu = pGpu;
    pImporter->spanLastIndex = -1;

    // Worker-params: alloc here, before any callback/workitem fires; owned by and freed with this importer.
    pParams = portMemAllocNonPaged(sizeof(*pParams));
    if (pParams == NULL)
        return NV_ERR_NO_MEMORY;
    portMemSet(pParams, 0, sizeof(*pParams));

    pParams->pLifetimeMutex = portSyncMutexCreate(portMemAllocatorGetGlobalNonPaged());
    if (pParams->pLifetimeMutex == NULL)
    {
        portMemFree(pParams);
        return NV_ERR_NO_MEMORY;
    }
    pParams->pImporter       = pImporter;
    pImporter->pWorkerParams = pParams;

    return NV_OK;
}

void
goeimportDestruct_IMPL
(
    GpuOpEventImport *pImporter
)
{
    GoeImportWorkerParams *pParams;

    NV_ASSERT_OR_RETURN_VOID(pImporter != NULL);

    //
    // NVOC destructor (reached via objDelete). Frees the importer-owned pWorkerParams. In the
    // normal path gpuOpEventImportDestruct already ran teardown under the GPU lock (nulling
    // pParams->pImporter); with the work-queue flush + bDropOnUnloadQueueFlush, no workitem can
    // still dispatch against pParams, so freeing it here is safe. pWorkerParams == NULL means
    // construct never set it up (ctor failed before the alloc) -- nothing to free.
    //
    pParams = pImporter->pWorkerParams;
    if (pParams == NULL)
        return;

    pImporter->pWorkerParams = NULL;

    portSyncMutexDestroy(pParams->pLifetimeMutex);
    portMemFree(pParams);
}

// ------------------------- Setup-sequence helpers --------------------------

static NV_STATUS
_goeimportAllocSysmemRegion
(
    RM_API   *pRmApi,
    NvHandle  hClient,
    NvHandle  hDevice,
    NvU64     size,
    NvHandle *phMemory
)
{
    NV_MEMORY_ALLOCATION_PARAMS memAllocParams;

    portMemSet(&memAllocParams, 0, sizeof(memAllocParams));
    memAllocParams.owner = HEAP_OWNER_RM_CLIENT_GENERIC;
    memAllocParams.type  = NVOS32_TYPE_IMAGE;
    memAllocParams.size  = size;
    memAllocParams.attr  = DRF_DEF(OS32, _ATTR,  _LOCATION,    _PCI)
                         | DRF_DEF(OS32, _ATTR,  _PHYSICALITY, _CONTIGUOUS)
                         | DRF_DEF(OS32, _ATTR,  _COHERENCY,   _UNCACHED);
    //
    // The GSP-side NV_EVENT_BUFFER producer DMAs records into these regions.
    // In a confidential-compute guest, default sysmem is CPU-private
    // (encrypted) and inaccessible to the GPU, so request unprotected memory
    // like the other GSP-shared buffers (message queue, logging buffers).
    // The attribute is honored only when CC is enabled; no-op otherwise.
    //
    memAllocParams.attr2 = DRF_DEF(OS32, _ATTR2, _REGISTER_MEMDESC_TO_PHYS_RM, _TRUE)
                         | DRF_DEF(OS32, _ATTR2, _MEMORY_PROTECTION, _UNPROTECTED);
    memAllocParams.flags = 0;

    return pRmApi->Alloc(pRmApi, hClient, hDevice, phMemory,
                         NV01_MEMORY_SYSTEM,
                         &memAllocParams, sizeof(memAllocParams));
}

//
// Alloc the Kernel-RM callback event under the pre-generated handle-alias value. The same numeric
// handle is reused by the Physical-RM 0x90d0 bind; _kgspRpcPostEvent resolves the callback by this
// handle on the host.
//
static NV_STATUS
_goeimportAllocCallbackEvent
(
    GpuOpEventImport *pImporter,
    RM_API           *pRmApi,
    NvHandle          hClient
)
{
    NV0005_ALLOC_PARAMETERS eventParams;

    //
    // callback descriptor lives on pImporter so its storage outlives the alloc.
    // The callback's arg is pImporter->pWorkerParams, NOT pImporter -- it finds the
    // importer via pParams->pImporter under pLifetimeMutex: either a valid pointer
    // (teardown takes the same mutex to null it) or NULL (teardown already ran).
    // pWorkerParams is owned by the importer and freed with it in destruct.
    //
    pImporter->callback.func = _goeimportPostEventCallback;
    pImporter->callback.arg  = pImporter->pWorkerParams;

    portMemSet(&eventParams, 0, sizeof(eventParams));
    eventParams.hParentClient = hClient;
    eventParams.hClass        = NV01_EVENT_KERNEL_CALLBACK_EX;
    eventParams.data          = NV_PTR_TO_NvP64(&pImporter->callback);
    eventParams.notifyIndex   = NV01_EVENT_CLIENT_RM | NV01_EVENT_WITHOUT_EVENT_DATA;

    //
    // Parent this host-only callback event under the client resource, not the subdevice. EventApi
    // explicitly RPC-allocates events attached to FW-client objects into GSP; doing that here would
    // consume hPhysBind in GSP before the 0x90d0 bind can allocate the same numeric listener
    // identity.
    //
    return pRmApi->AllocWithHandle(pRmApi, hClient, hClient,
                                   pImporter->hPhysBind,
                                   NV01_EVENT_KERNEL_CALLBACK_EX,
                                   &eventParams, sizeof(eventParams));
}

//
// Generate the shared handle-alias value and allocate the Kernel-RM callback event under it. The
// same numeric handle is later reused by the Physical-RM 0x90d0 bind (independent namespaces).
//
static NV_STATUS
_goeimportAllocBindAlias
(
    GpuOpEventImport *pImporter,
    RM_API           *pRmApi,
    NvHandle          hClient
)
{
    NV_STATUS status;

    NV_ASSERT_OR_RETURN(pImporter != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pRmApi != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pImporter->hPhysBind == 0, NV_ERR_INVALID_STATE);

    status = serverutilGenResourceHandle(hClient, &pImporter->hPhysBind);
    if (status != NV_OK)
        return status;

    status = _goeimportAllocCallbackEvent(pImporter, pRmApi, hClient);
    if (status != NV_OK)
    {
        //
        // gen-resource minted a value but no resource owns it; clear so teardown doesn't try to
        // free a phantom handle.
        //
        pImporter->hPhysBind = 0;
    }

    return status;
}

//
// GSP-side NV_EVENT_BUFFER via pPhysicalRmApi (RPCs to GSP). Uses the three cross-registered
// backing handles as external sources. notificationHandle = 0; the wake path is the bClientRM
// listener installed by the 0x90d0 bind (eventbufferTryAddNotify ignores notificationHandle).
//
static NV_STATUS
_goeimportAllocEventBuffer
(
    GpuOpEventImport *pImporter,
    RM_API           *pPhysicalRmApi,
    NvHandle          hClient,
    NvHandle          hSubdevice
)
{
    NV_EVENT_BUFFER_ALLOC_PARAMETERS bufParams;

    portMemSet(&bufParams, 0, sizeof(bufParams));
    bufParams.recordSize           = GOEIMPORT_RECORD_SIZE;
    bufParams.recordCount          = GOEIMPORT_RECORD_COUNT;
    bufParams.vardataBufferSize    = GOEIMPORT_VARDATA_BUFFER_SIZE;
    bufParams.recordsFreeThreshold = GOEIMPORT_RECORDS_FREE_THRESHOLD;
    bufParams.vardataFreeThreshold = GOEIMPORT_VARDATA_FREE_THRESHOLD;
    bufParams.flags                = 0;
    bufParams.notificationHandle   = 0;
    bufParams.hSubDevice           = hSubdevice;
    bufParams.hBufferHeader        = pImporter->hHeaderMemory;
    bufParams.hRecordBuffer        = pImporter->hRecordMemory;
    bufParams.hVardataBuffer       = pImporter->hVardataMemory;
    // UMD VA OUTs unread; the host maps the backing itself.
    return pPhysicalRmApi->AllocWithHandle(pPhysicalRmApi, hClient, hClient,
                                           pImporter->hPhysEventBuffer,
                                           NV_EVENT_BUFFER,
                                           &bufParams, sizeof(bufParams));
}

static NV_STATUS
_goeimportMapSysmemRegions
(
    GpuOpEventImport *pImporter,
    RM_API           *pRmApi,
    NvHandle          hClient,
    NvHandle          hDevice
)
{
    NV_STATUS status;
    void     *pMap;

    pMap = NULL;
    status = pRmApi->MapToCpu(pRmApi, hClient, hDevice,
                              pImporter->hHeaderMemory,
                              0, sizeof(NV_EVENT_BUFFER_HEADER),
                              &pMap, 0);
    if (status != NV_OK)
        return status;
    pImporter->pHeader = (NV_EVENT_BUFFER_HEADER *)pMap;

    pMap = NULL;
    status = pRmApi->MapToCpu(pRmApi, hClient, hDevice,
                              pImporter->hRecordMemory,
                              0,
                              (NvU64)GOEIMPORT_RECORD_SIZE *
                                  (NvU64)GOEIMPORT_RECORD_COUNT,
                              &pMap, 0);
    if (status != NV_OK)
        return status;
    pImporter->pRecords = (NvU8 *)pMap;

    pMap = NULL;
    status = pRmApi->MapToCpu(pRmApi, hClient, hDevice,
                              pImporter->hVardataMemory,
                              0, GOEIMPORT_VARDATA_BUFFER_SIZE,
                              &pMap, 0);
    if (status != NV_OK)
        return status;
    pImporter->pVardata = (NvU8 *)pMap;

    return NV_OK;
}

//
// Enable producer KEEP_OLDEST. Must precede the 0x90d0 bind alloc -- the bind constructor's
// listener install may immediately exercise the producer, and eventBufferProducerAddEvent
// NV_WARN_NOTHING_TO_DO's while disabled.
//
static NV_STATUS
_goeimportEnableProducer
(
    GpuOpEventImport *pImporter,
    RM_API           *pPhysicalRmApi,
    NvHandle          hClient
)
{
    NV_EVENT_BUFFER_CTRL_CMD_ENABLE_EVENTS_PARAMS enableParams;

    portMemSet(&enableParams, 0, sizeof(enableParams));
    enableParams.enable = NV_TRUE;
    enableParams.flags  = NV_EVENT_BUFFER_FLAG_OVERFLOW_POLICY_KEEP_OLDEST;

    return pPhysicalRmApi->Control(pPhysicalRmApi, hClient,
                                   pImporter->hPhysEventBuffer,
                                   NV_EVENT_BUFFER_CTRL_CMD_ENABLE_EVENTS,
                                   &enableParams, sizeof(enableParams));
}

//
// Allocate the Physical-RM 0x90d0 import bind on top of the EventBuffer. Reuses hPhysBind
// (handle-alias trick) so the Physical-RM bind constructor can fetch its own resource handle and
// pass it as the bClientRM listener identity for opEventBufferInstallClientRmNotify.
//
static NV_STATUS
_goeimportAllocImportBind
(
    GpuOpEventImport *pImporter,
    RM_API           *pPhysicalRmApi,
    NvHandle          hClient
)
{
    NV_OPERATIONAL_EVENT_BUFFER_BIND_ALLOC_PARAMETERS bindParams = {0};
    OBJGPU *pGpu = pImporter->pGpu;
    NvU32 gidFlags = DRF_DEF(2080_GPU_CMD, _GPU_GET_GID_FLAGS, _TYPE, _SHA1) |
                     DRF_DEF(2080_GPU_CMD, _GPU_GET_GID_FLAGS, _FORMAT, _BINARY);

    NV_ASSERT_OR_RETURN(pGpu != NULL, NV_ERR_INVALID_STATE);
    NV_ASSERT_OK_OR_RETURN(gpuGetGidInfo(pGpu, NULL, NULL, gidFlags));
    NV_ASSERT_OR_RETURN(pGpu->gpuUuid.isInitialized, NV_ERR_INVALID_STATE);

    bindParams.hEventBuffer   = pImporter->hPhysEventBuffer;
    bindParams.recordFormat   = NV_OPERATIONAL_EVENT_BUFFER_RECORD_FORMAT_GOE;
    bindParams.scope          = NV_OPERATIONAL_EVENT_BUFFER_SCOPE_DEVICE;
    bindParams.minLogLevel    = NV_GPU_OPERATIONAL_EVENT_LOG_LEVEL_ALL;
    bindParams.minSeverity    = NV_OPERATIONAL_EVENT_SEVERITY_ALL;
    bindParams.bindId         = 0;       // assigned by producer state
    bindParams.startingCursor = NV_OPERATIONAL_EVENT_BUFFER_STARTING_CURSOR_AFTER_BIND;

    portMemCopy(bindParams.resourceUuid, sizeof(bindParams.resourceUuid),
                pGpu->gpuUuid.uuid, sizeof(bindParams.resourceUuid));

    return pPhysicalRmApi->AllocWithHandle(pPhysicalRmApi,
                                           hClient, hClient,
                                           pImporter->hPhysBind,
                                           NV_OPERATIONAL_EVENT_BUFFER_BIND,
                                           &bindParams, sizeof(bindParams));
}

//
// Setup sequence. Each successful step leaves a handle/pointer non-zero on pImporter so
// _goeimportTearDownPartial unwinds correctly on any failure.
//
static NV_STATUS
_goeimportSetupTransport
(
    OBJGPU           *pGpu,
    GpuOpEventImport *pImporter
)
{
    NV_STATUS status;

    RM_API   *pRmApi          = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    RM_API   *pPhysicalRmApi  = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV_ASSERT_OR_RETURN(pRmApi != NULL, NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(pPhysicalRmApi != NULL, NV_ERR_INVALID_STATE);

    //
    // Alloc import-owned client trio. NV01_ROOT carries RS_FLAGS_ALLOC_RPC_TO_PHYS_RM, so the trio
    // resolves on both Kernel-RM (pRmApi) and GSP (pPhysicalRmApi via rpcRmApiAlloc_GSP). Freed in
    // teardown.
    //
    NV_PRINTF(LEVEL_INFO, "goeimport: step 2 alloc client trio\n");
    status = rmapiutilAllocClientAndDeviceHandles(pRmApi, pGpu,
                                                  &pImporter->hClient,
                                                  &pImporter->hDevice,
                                                  &pImporter->hSubdevice);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "goeimport: step 2 failed (0x%x)\n", status);
        return status;
    }

    const NvHandle hClient    = pImporter->hClient;
    const NvHandle hDevice    = pImporter->hDevice;
    const NvHandle hSubdevice = pImporter->hSubdevice;

    NV_PRINTF(LEVEL_INFO,
              "goeimport: setup begin; hClient=0x%08x hDevice=0x%08x hSubdevice=0x%08x\n",
              hClient, hDevice, hSubdevice);

    NV_PRINTF(LEVEL_INFO, "goeimport: step 3a alloc header sysmem\n");
    status = _goeimportAllocSysmemRegion(pRmApi, hClient, hDevice,
                                            sizeof(NV_EVENT_BUFFER_HEADER),
                                            &pImporter->hHeaderMemory);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "goeimport: step 3a failed (0x%x)\n", status);
        return status;
    }

    NV_PRINTF(LEVEL_INFO, "goeimport: step 3b alloc records sysmem\n");
    status = _goeimportAllocSysmemRegion(pRmApi, hClient, hDevice,
                                            (NvU64)GOEIMPORT_RECORD_SIZE *
                                                (NvU64)GOEIMPORT_RECORD_COUNT,
                                            &pImporter->hRecordMemory);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "goeimport: step 3b failed (0x%x)\n", status);
        return status;
    }

    NV_PRINTF(LEVEL_INFO, "goeimport: step 3c alloc vardata sysmem\n");
    status = _goeimportAllocSysmemRegion(pRmApi, hClient, hDevice,
                                            GOEIMPORT_VARDATA_BUFFER_SIZE,
                                            &pImporter->hVardataMemory);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "goeimport: step 3c failed (0x%x)\n", status);
        return status;
    }

    NV_PRINTF(LEVEL_INFO,
              "goeimport: step 4 alloc callback event handle alias\n");
    status = _goeimportAllocBindAlias(pImporter, pRmApi, hClient);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "goeimport: step 4 failed (0x%x)\n", status);
        return status;
    }

    NV_PRINTF(LEVEL_INFO, "goeimport: step 5a gen hPhysEventBuffer\n");
    status = serverutilGenResourceHandle(hClient, &pImporter->hPhysEventBuffer);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "goeimport: step 5a failed (0x%x)\n", status);
        return status;
    }

    NV_PRINTF(LEVEL_INFO,
              "goeimport: step 5b alloc EventBuffer (hPhysEventBuffer=0x%08x)\n",
              pImporter->hPhysEventBuffer);
    status = _goeimportAllocEventBuffer(pImporter, pPhysicalRmApi,
                                           hClient, hSubdevice);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "goeimport: step 5b failed (0x%x)\n", status);
        pImporter->hPhysEventBuffer = 0;
        return status;
    }

    NV_PRINTF(LEVEL_INFO, "goeimport: step 6 MapToCpu x3\n");
    status = _goeimportMapSysmemRegions(pImporter, pRmApi, hClient, hDevice);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "goeimport: step 6 failed (0x%x)\n", status);
        return status;
    }

    //
    // Zero the backing before producer enable: external 0x90cd alloc maps but doesn't clear, and
    // the header holds the producer's ring cursors.
    //
    portMemSet(pImporter->pHeader,  0, sizeof(NV_EVENT_BUFFER_HEADER));
    portMemSet(pImporter->pRecords, 0,
               (NvU64)GOEIMPORT_RECORD_SIZE *
                   (NvU64)GOEIMPORT_RECORD_COUNT);
    portMemSet(pImporter->pVardata, 0, GOEIMPORT_VARDATA_BUFFER_SIZE);

    NV_PRINTF(LEVEL_INFO, "goeimport: step 7 ENABLE_EVENTS\n");
    status = _goeimportEnableProducer(pImporter, pPhysicalRmApi, hClient);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "goeimport: step 7 failed (0x%x)\n", status);
        return status;
    }

    NV_PRINTF(LEVEL_INFO,
              "goeimport: step 8 alloc 0x90d0 bind (hPhysBind=0x%08x)\n",
              pImporter->hPhysBind);
    status = _goeimportAllocImportBind(pImporter, pPhysicalRmApi, hClient);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "goeimport: step 8 failed (0x%x)\n", status);
        return status;
    }

    //
    // hPhysBind now also names a Physical-RM resource; teardown's Physical-RM Free is gated on this
    // flag, not the handle value.
    //
    pImporter->bPhysBindAllocated = NV_TRUE;
    NV_PRINTF(LEVEL_INFO, "goeimport: setup complete\n");

    return NV_OK;
}

// ------------------------ Teardown helper (shared) ------------------------

//
// Idempotent partial-teardown. Shared between setup-failure rollback and gpuOpEventImportDestruct.
// Order matters: Free(hPhysEventBuffer) detaches the cross-domain listener and (per RM listener-
// detach semantics) synchronizes with in-flight POST_EVENT callbacks; must precede the sysmem-
// handle teardown so a mid-flight callback can't touch unmapped memory.
//
static void
_goeimportTearDownPartial
(
    OBJGPU           *pGpu,
    GpuOpEventImport *pImporter
)
{
    RM_API *pRmApi;
    RM_API *pPhysicalRmApi;
    NvHandle hClient;
    NvHandle hDevice;

    NV_ASSERT_OR_RETURN_VOID(pGpu != NULL);
    NV_ASSERT_OR_RETURN_VOID(pImporter != NULL);

    // Lockless fence: future dispatches early-exit.
    pImporter->bStreamingDisabled = NV_TRUE;

    //
    // Null pParams->pImporter under pLifetimeMutex so any later dispatch observes it and bails. The
    // GPU lock -- held here via gpuStateDestroy and by the drain workitem (bLockGpuGroupSubdevice) --
    // serializes against an in-flight drain and keeps the mapped transport alive while it runs; this
    // mutex only guards the pImporter handoff, not drain state. Must precede any transport free so a
    // subsequent dispatch can't observe a stale pImporter. pParams itself is freed in the NVOC
    // destructor; the work-queue flush + bDropOnUnloadQueueFlush ensure no workitem dispatches
    // against it after teardown.
    //
    if (pImporter->pWorkerParams != NULL)
    {
        portSyncMutexAcquire(pImporter->pWorkerParams->pLifetimeMutex);
        pImporter->pWorkerParams->pImporter = NULL;
        portSyncMutexRelease(pImporter->pWorkerParams->pLifetimeMutex);
    }

    //
    // Drop any partial span: the ImportEventGroup + contained events + raw-context wrappers would
    // otherwise leak (NVOC destructor only frees worker params). Idempotent if no span is open.
    //
    _goeimportAbortSpan(pImporter);

    pRmApi         = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    pPhysicalRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    hClient        = pImporter->hClient;
    hDevice        = pImporter->hDevice;

    //
    // Free the Physical-RM 0x90d0 bind. Gated on bPhysBindAllocated (set only after the bind alloc
    // succeeds) so we never Free a bind setup didn't create. The Kernel-RM callback event sharing
    // this numeric handle is freed separately below via pRmApi.
    //
    if (pImporter->bPhysBindAllocated && pPhysicalRmApi != NULL)
    {
        (void)pPhysicalRmApi->Free(pPhysicalRmApi, hClient, pImporter->hPhysBind);
        pImporter->bPhysBindAllocated = NV_FALSE;
    }

    //
    // Free EventBuffer: destructor detaches the cross-domain listener and synchronizes with in-
    // flight notifies.
    //
    if (pImporter->hPhysEventBuffer != 0 && pPhysicalRmApi != NULL)
    {
        (void)pPhysicalRmApi->Free(pPhysicalRmApi, hClient,
                                   pImporter->hPhysEventBuffer);
        pImporter->hPhysEventBuffer = 0;
    }

    if (pImporter->pVardata != NULL && pRmApi != NULL)
    {
        (void)pRmApi->UnmapFromCpu(pRmApi, hClient, hDevice,
                                   pImporter->hVardataMemory,
                                   pImporter->pVardata, 0, 0);
        pImporter->pVardata = NULL;
    }
    if (pImporter->pRecords != NULL && pRmApi != NULL)
    {
        (void)pRmApi->UnmapFromCpu(pRmApi, hClient, hDevice,
                                   pImporter->hRecordMemory,
                                   pImporter->pRecords, 0, 0);
        pImporter->pRecords = NULL;
    }
    if (pImporter->pHeader != NULL && pRmApi != NULL)
    {
        (void)pRmApi->UnmapFromCpu(pRmApi, hClient, hDevice,
                                   pImporter->hHeaderMemory,
                                   pImporter->pHeader, 0, 0);
        pImporter->pHeader = NULL;
    }

    // Free the Kernel-RM callback event sharing the handle-alias value.
    if (pImporter->hPhysBind != 0 && pRmApi != NULL)
    {
        (void)pRmApi->Free(pRmApi, hClient, pImporter->hPhysBind);
        pImporter->hPhysBind = 0;
    }

    if (pImporter->hVardataMemory != 0 && pRmApi != NULL)
    {
        (void)pRmApi->Free(pRmApi, hClient, pImporter->hVardataMemory);
        pImporter->hVardataMemory = 0;
    }
    if (pImporter->hRecordMemory != 0 && pRmApi != NULL)
    {
        (void)pRmApi->Free(pRmApi, hClient, pImporter->hRecordMemory);
        pImporter->hRecordMemory = 0;
    }
    if (pImporter->hHeaderMemory != 0 && pRmApi != NULL)
    {
        (void)pRmApi->Free(pRmApi, hClient, pImporter->hHeaderMemory);
        pImporter->hHeaderMemory = 0;
    }

    //
    // Free the import-owned client trio last -- all preceding frees use hClient.
    //
    if (pImporter->hClient != 0 && pRmApi != NULL)
    {
        rmapiutilFreeClientAndDeviceHandles(pRmApi,
                                            &pImporter->hClient,
                                            &pImporter->hDevice,
                                            &pImporter->hSubdevice);
    }

    pImporter->callback.func = NULL;
    pImporter->callback.arg  = NULL;
}

// -------------------------- OBJGPU entry points ---------------------------

void
gpuOpEventImportConstruct_IMPL
(
    OBJGPU *pGpu
)
{
    GpuOpEventImport *pImporter;
    NV_STATUS status;

    NV_ASSERT_OR_RETURN_VOID(pGpu != NULL);
    NV_ASSERT_OR_RETURN_VOID(pGpu->pGpuOpEventImport == NULL);

    status = objCreate(&pGpu->pGpuOpEventImport, pGpu,
                       GpuOpEventImport, pGpu);
    if (status != NV_OK || pGpu->pGpuOpEventImport == NULL)
    {
        NV_PRINTF(LEVEL_WARNING,
                  "objCreate(GpuOpEventImport) failed (0x%x); GSP "
                  "operational-event import disabled for this GPU.\n",
                  status);
        return;
    }

    pImporter = pGpu->pGpuOpEventImport;

    //
    // Soft-fail: any setup error latches bStreamingDisabled and runs the idempotent teardown; the
    // object stays allocated for deinit.
    //
    status = _goeimportSetupTransport(pGpu, pImporter);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_WARNING,
                  "GpuOpEventImport setup failed (0x%x); streaming "
                  "disabled for this GPU.\n", status);
        _goeimportTearDownPartial(pGpu, pImporter);
        pImporter->bStreamingDisabled = NV_TRUE;
    }

}

void
gpuOpEventImportDestruct_IMPL
(
    OBJGPU *pGpu
)
{
    GpuOpEventImport *pImporter;

    NV_ASSERT_OR_RETURN_VOID(pGpu != NULL);

    pImporter = pGpu->pGpuOpEventImport;
    if (pImporter == NULL)
        return;

    _goeimportTearDownPartial(pGpu, pImporter);
    objDelete(pImporter);
    pGpu->pGpuOpEventImport = NULL;
}

// -------------------- Stage 1 callback + queue helper --------------------

//
// Queue the Stage 2 drain workitem. Called from the Stage 1 callback with pParams->pLifetimeMutex
// held (it verified pParams->pImporter != NULL under that mutex).
//
// Why a workitem at all (not inline from the POST_EVENT callback):
//   - Windows IRQL: POST_EVENT can fire at a level above PASSIVE
//     where portMemAllocNonPaged would bugcheck. Stage 2 allocates
//     objects (objCreate, raw-context wrappers) and must run at
//     PASSIVE.
//   - Nested-RPC ban: Stage 2 issues UPDATE_GET via pPhysicalRmApi,
//     which is itself an RPC. The POST_EVENT callback is invoked
//     from the RPC receive path, so a nested RPC there would
//     deadlock.
//
// Workitem flags:
//   apiLock -- Stage 2 does not need API write ownership, so steady state uses
//     NOT_ACQUIRED. But while the GPU is still coming up, the init thread holds the
//     API + GPU locks across all of RmInitAdapter, and the framework's bare
//     subdevice-lock acquire (with no API lock) is skipped -- the queued worker then
//     never runs. So until the GPU is state-loaded, take the API
//     read lock instead: that serializes the worker behind init's API write lock so
//     it dispatches once init releases the locks. gpuIsStateLoaded (not
//     STATE_INITIALIZED, which flips at end of gpuStateInit while gpuStateLoad still
//     holds the locks) is the boundary; erring toward READ_ONLY is safe since it only
//     defers the worker, whereas a wrong NOT_ACQUIRED skips it.
//   bLockGpuGroupSubdevice -- the single per-GPU lock (GPU_LOCK_GRP_SUBDEVICE),
//     not the all-GPU group. The importer is per-OBJGPU; this serializes
//     concurrent drain workitems on the same importer and satisfies the
//     UPDATE_GET RPC's rmDeviceGpuLockIsOwner expectation.
//   bRequiresGpu -- workitem dereferences pGpu.
//   bDontFreeParams -- pParams is owned by the importer (freed in destruct), so the
//     OS worker must NOT portMemFree it.
//   bDropOnUnloadQueueFlush -- pParams is freed at importer teardown, so a workitem
//     still queued at unload is dropped rather than run against freed params.
//
static NV_STATUS
_goeimportQueueLocked
(
    GoeImportWorkerParams *pParams,
    OBJGPU           *pGpu
)
{
    OsQueueWorkItemFlags flags = { 0 };

    flags.apiLock                  = gpuIsStateLoaded(pGpu) ?
                                         WORKITEM_FLAGS_API_LOCK_NOT_ACQUIRED :
                                         WORKITEM_FLAGS_API_LOCK_READ_ONLY;
    flags.bLockGpuGroupSubdevice   = NV_TRUE;
    flags.bRequiresGpu             = NV_TRUE;
    flags.bDontFreeParams          = NV_TRUE;
    flags.bDropOnUnloadQueueFlush  = NV_TRUE;

    return osQueueWorkItem(pGpu, _goeimportPublishWorkitem, pParams, flags);
}

//
// Stage 1: POST_EVENT callback. Invoked from the RPC receive path
// (_kgspRpcPostEvent) with API + GPU locks held by the dispatcher.
// Avoids inline work for two reasons: (a) allocations can't run at
// the dispatcher's IRQL on Windows, and (b) UPDATE_GET would be a
// nested RPC.
//
// pArg is the worker-params object, NOT pImporter. pParams has its own
// lifetime; the callback reads pParams->pImporter under pLifetimeMutex,
// so a mid-flight destructor is serialized via the same mutex.
//
static void
_goeimportPostEventCallback
(
    void     *pArg,
    void     *pData,
    NvHandle  hEvent,
    NvU32     data,
    NvU32     status
)
{
    GoeImportWorkerParams *pParams = (GoeImportWorkerParams *)pArg;

    (void)pData;
    (void)hEvent;
    (void)data;
    (void)status;

    if (pParams == NULL || pParams->pLifetimeMutex == NULL)
        return;

    portSyncMutexAcquire(pParams->pLifetimeMutex);
    if (pParams->pImporter != NULL && !pParams->pImporter->bStreamingDisabled)
    {
        //
        // A queue failure is terminal, not "retry on next wake": the producer latched
        // bNotifyPending on its POST and only the drain's UPDATE_GET clears it, so with no workitem
        // the producer's threshold notifications stay suppressed and no further wake arrives. Log
        // loudly and latch bStreamingDisabled so the dead state is explicit; the next gpuStateLoad
        // re-creates the importer and resyncs. (In-session recovery would need a separate path to
        // clear the producer notify gate.)
        //
        NV_STATUS queueStatus;
        NV_CHECK_OK(queueStatus, LEVEL_ERROR,
                    _goeimportQueueLocked(pParams, pParams->pImporter->pGpu));
        if (queueStatus != NV_OK)
            pParams->pImporter->bStreamingDisabled = NV_TRUE;
    }
    portSyncMutexRelease(pParams->pLifetimeMutex);
}

// ------------------------ Stage 2: span reassembly -----------------------

//
// Close the open span: publish pSpanGroup to the host OpEventLog and reset span state.
// opevtlogAppendEventGroup deep-copies into the log entry and does NOT take ownership; we objDelete
// on every exit path.
//
static void
_goeimportCloseSpan
(
    GpuOpEventImport *pImporter
)
{
    if (pImporter->pSpanGroup == NULL)
        return;

    if (opEventLog != NULL)
    {
        NV_STATUS s = opevtlogAppendEventGroup(opEventLog,
            staticCast(pImporter->pSpanGroup, EventGroup));
        if (s != NV_OK)
        {
            NV_PRINTF(LEVEL_WARNING,
                      "GpuOpEventImport: opevtlogAppendEventGroup "
                      "failed (0x%x) for cursor 0x%llx; group dropped.\n",
                      s, (unsigned long long)pImporter->spanCursor);
        }
    }

    //
    // Drop our ref; EventGroup destructor frees the contained events (raw-context wrappers freed
    // via opevtDestruct's bOwnedByEvent walk).
    //
    objDelete(pImporter->pSpanGroup);
    pImporter->pSpanGroup        = NULL;
    pImporter->spanCursor        = 0;
    pImporter->spanGroupSize     = 0;
    pImporter->spanObservedCount = 0;
    pImporter->spanLastIndex     = -1;
}

// Abort the open span without publishing. Used on fatal error / terminal shutdown.
static void
_goeimportAbortSpan
(
    GpuOpEventImport *pImporter
)
{
    if (pImporter->pSpanGroup == NULL)
        return;

    objDelete(pImporter->pSpanGroup);
    pImporter->pSpanGroup        = NULL;
    pImporter->spanCursor        = 0;
    pImporter->spanGroupSize     = 0;
    pImporter->spanObservedCount = 0;
    pImporter->spanLastIndex     = -1;
}

//
// Reconstruct a GpuOperationalEvent from a wire record. Caller owns the returned event and must
// eventgroupAddEvent or objDelete it.
//
static NV_STATUS
_goeimportBuildGpuOpEvent
(
    GpuOpEventImport                   *pImporter,
    const NV_GPU_OPERATIONAL_EVENT_RECORD *pRec,
    const NvU8                            *pVardataWindow,
    NvU32                                  vardataLen,
    GpuOperationalEvent                  **ppEventOut
)
{
    GpuOperationalEvent  *pEvent = NULL;
    OperationalEvent     *pOp;
    GpuOperationalEventParams params;
    NV_STATUS             status;

    NV_ASSERT_OR_RETURN(pImporter->pGpu != NULL, NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(pImporter->pGpu->pEventBus != NULL, NV_ERR_INVALID_STATE);

    status = objCreate(&pEvent, NVOC_NULL_OBJECT, GpuOperationalEvent,
                       pImporter->pGpu->pEventBus,
                       (NvU32)pRec->event.eventCategory,
                       pRec->event.moduleSignature,
                       (NvU32)pRec->event.moduleEventCode,
                       (NvU32)pRec->event.severity,
                       (NvU32)pRec->event.attributes,
                       (NvU32)pRec->scope,
                       (NvU32)pRec->reportingSource,
                       (NvU32)pRec->logLevel);
    if (status != NV_OK || pEvent == NULL)
        return (status != NV_OK) ? status : NV_ERR_NO_MEMORY;

    // Override the freshly-minted identity scalars with producer values.
    pOp = staticCast(pEvent, OperationalEvent);
    pOp->instanceId = pRec->event.instanceId;
    pOp->timestamp  = pRec->event.timestamp;

    portMemCopy(pEvent->deviceUuid, sizeof(pEvent->deviceUuid),
                pRec->deviceUuid, sizeof(pRec->deviceUuid));
    pEvent->originator = (GPU_OPERATIONAL_EVENT_ORIGINATOR)pRec->originator;
    params.moduleInstance = pRec->moduleInstance;
    params.chipletId      = pRec->chipletId;
    params.migAttribution = pRec->migAttribution;
    gpuopevtInit(pEvent, &params);

    if (vardataLen > 0)
    {
        status = _goeimportBuildContextChain(pEvent,
                                                pVardataWindow, vardataLen);
        if (status != NV_OK)
        {
            objDelete(pEvent);
            return status;
        }
    }

    *ppEventOut = pEvent;

    return NV_OK;
}

//
// Open a fresh span on the record's identity. Caller must subsequently eventgroupAddEvent and bump
// spanObservedCount / spanLastIndex.
//
static NV_STATUS
_goeimportOpenSpan
(
    GpuOpEventImport                   *pImporter,
    const NV_GPU_OPERATIONAL_EVENT_RECORD *pRec
)
{
    ImportEventGroup *pGroup = NULL;
    NV_STATUS         status;

    NV_ASSERT_OR_RETURN(pImporter->pSpanGroup == NULL, NV_ERR_INVALID_STATE);

    //
    // A well-formed group has a nonzero size and an in-range opening index (a producer drop only
    // skips indices forward, so the first observed index is still < groupSize). Anything else is
    // wire corruption; reject before allocating the group.
    //
    if (pRec->event.groupSize == 0 ||
        pRec->event.groupIndex >= pRec->event.groupSize)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "GpuOpEventImport: corrupt span identity (groupSize %u, groupIndex %u) on "
                  "cursor 0x%llx.\n",
                  pRec->event.groupSize, pRec->event.groupIndex,
                  (unsigned long long)pRec->event.cursorValue);
        return NV_ERR_INVALID_STATE;
    }

    status = objCreate(&pGroup, NVOC_NULL_OBJECT, ImportEventGroup,
                       pImporter->pGpu->pEventBus);
    if (status != NV_OK || pGroup == NULL)
        return (status != NV_OK) ? status : NV_ERR_NO_MEMORY;

    //
    // opevtlogAppendEventGroup dynamicCasts to ImportEventGroup and
    // adopts pGroup->cursor into pEntry->groupCursor.
    //
    pGroup->cursor = pRec->event.cursorValue;
    eventgroupSetTraceId(staticCast(pGroup, EventGroup), pRec->event.traceId);
    eventgroupSetAttributes(staticCast(pGroup, EventGroup), pRec->event.groupAttributes);

    pImporter->pSpanGroup        = pGroup;
    pImporter->spanCursor        = pRec->event.cursorValue;
    pImporter->spanGroupSize     = pRec->event.groupSize;
    pImporter->spanObservedCount = 0;
    pImporter->spanLastIndex     = -1;

    return NV_OK;
}

//
// Span reassembly rules for one rebuilt record:
//   cursor/size change                -> close current, open fresh
//   out-of-range/dup/backward index   -> wire corruption (NV_ERR_INVALID_STATE; fatal in drain loop)
//   spanObservedCount == spanGroupSize -> close immediately
//
static NV_STATUS
_goeimportProcessRecord
(
    GpuOpEventImport                   *pImporter,
    const NV_GPU_OPERATIONAL_EVENT_RECORD *pRec,
    const NvU8                            *pVardataWindow,
    NvU32                                  vardataLen
)
{
    GpuOperationalEvent *pEvent = NULL;
    NV_STATUS            status;

    if (pImporter->pSpanGroup != NULL &&
        (pImporter->spanCursor != pRec->event.cursorValue ||
         pImporter->spanGroupSize   != pRec->event.groupSize))
    {
        _goeimportCloseSpan(pImporter);
    }

    if (pImporter->pSpanGroup == NULL)
    {
        status = _goeimportOpenSpan(pImporter, pRec);
        if (status != NV_OK)
            return status;
    }

    //
    // Out-of-range, duplicate, or backward index is wire corruption: a legal producer drop only
    // ever skips indices forward. Surface it (fatal in the drain loop) rather than dropping. Signed
    // compare so the -1 sentinel sorts before groupIndex 0.
    //
    if (pRec->event.groupIndex >= pImporter->spanGroupSize)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "GpuOpEventImport: groupIndex %u >= groupSize %u on cursor 0x%llx; corrupt.\n",
                  pRec->event.groupIndex, pImporter->spanGroupSize,
                  (unsigned long long)pImporter->spanCursor);
        return NV_ERR_INVALID_STATE;
    }
    if ((NvS32)pRec->event.groupIndex <= (NvS32)pImporter->spanLastIndex)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "GpuOpEventImport: dup/backward groupIndex %u (spanLastIndex %d) on cursor "
                  "0x%llx; corrupt.\n",
                  pRec->event.groupIndex,
                  (NvS32)pImporter->spanLastIndex,
                  (unsigned long long)pImporter->spanCursor);
        return NV_ERR_INVALID_STATE;
    }

    status = _goeimportBuildGpuOpEvent(pImporter, pRec,
                                          pVardataWindow, vardataLen,
                                          &pEvent);
    if (status != NV_OK)
        return status;

    eventgroupAddEvent(staticCast(pImporter->pSpanGroup, EventGroup),
                       staticCast(pEvent, Event));
    pImporter->spanLastIndex     = (NvS16)pRec->event.groupIndex;
    pImporter->spanObservedCount += 1U;

    if (pImporter->spanObservedCount >= pImporter->spanGroupSize)
        _goeimportCloseSpan(pImporter);

    return NV_OK;
}

//
// Push the host's record/vardata cursors to GSP. Fires unconditionally from the workitem (even on
// zero-record passes, so the producer's bNotifyPending latch always clears). Any failure is
// terminal.
//
static void
_goeimportIssueUpdateGet
(
    GpuOpEventImport *pImporter,
    NvU32             newVardataGet
)
{
    RM_API *pPhysicalRmApi = GPU_GET_PHYSICAL_RMAPI(pImporter->pGpu);
    NV_EVENT_BUFFER_CTRL_CMD_UPDATE_GET_PARAMS params;
    NV_STATUS status;

    NV_ASSERT_OR_RETURN_VOID(pPhysicalRmApi != NULL);
    NV_ASSERT_OR_RETURN_VOID(pImporter->hPhysEventBuffer != 0);
    NV_ASSERT_OR_RETURN_VOID(pImporter->hClient != 0);

    portMemSet(&params, 0, sizeof(params));
    params.recordBufferGet  = pImporter->pHeader->recordGet;
    params.varDataBufferGet = newVardataGet;

    status = pPhysicalRmApi->Control(pPhysicalRmApi,
                                     pImporter->hClient,
                                     pImporter->hPhysEventBuffer,
                                     NV_EVENT_BUFFER_CTRL_CMD_UPDATE_GET,
                                     &params, sizeof(params));
    if (status == NV_OK)
    {
        pImporter->vardataGet = newVardataGet;
        return;
    }

    NV_PRINTF(LEVEL_ERROR,
              "GpuOpEventImport: UPDATE_GET ctrl failed (0x%x); "
              "terminal shutdown.\n", status);
    _goeimportAbortSpan(pImporter);
    pImporter->bStreamingDisabled = NV_TRUE;
}

//
// Stage 2 entry: drain the shared sysmem ring [recordGet, recordPut), reassemble spans, publish via
// opevtlogAppendEventGroup, push recordGet back to GSP via UPDATE_GET.
//
static void
_goeimportPublishWorkitem
(
    NvU32  gpuInstance,
    void  *pArg
)
{
    GoeImportWorkerParams *pParams = (GoeImportWorkerParams *)pArg;
    GpuOpEventImport      *pImporter;
    NvU32 recordGet;
    NvU32 recordPut;
    NvU32 newVardataGet;

    (void)gpuInstance;

    if (pParams == NULL || pParams->pLifetimeMutex == NULL)
        return;

    //
    // Capture pImporter under pLifetimeMutex (the destructor takes the same mutex to null it). The
    // drain below runs under the GPU lock held by the workitem framework, which is what keeps
    // pImporter and its mapped transport alive against teardown -- so release the mutex before
    // draining; it guards only the handoff, not drain state.
    //
    portSyncMutexAcquire(pParams->pLifetimeMutex);
    pImporter = pParams->pImporter;
    portSyncMutexRelease(pParams->pLifetimeMutex);

    //
    // pImporter == NULL (the destructor ran before this dispatch) or bStreamingDisabled (soft-fail
    // or terminal shutdown latched) are expected: nothing to drain, just exit.
    //
    if (pImporter == NULL || pImporter->bStreamingDisabled)
        return;

    //
    // A live, non-disabled importer always has its three sysmem regions mapped (set in setup,
    // cleared only by teardown, which also nulls pImporter under the mutex). A NULL here is a
    // setup/teardown-ordering bug, so assert loudly rather than silently skipping.
    //
    NV_ASSERT_OR_RETURN_VOID(pImporter->pHeader  != NULL);
    NV_ASSERT_OR_RETURN_VOID(pImporter->pRecords != NULL);
    NV_ASSERT_OR_RETURN_VOID(pImporter->pVardata != NULL);

    //
    // Acquire fence pairs with the producer's release fence (payload stores published before
    // recordPut).
    //
    recordGet = pImporter->pHeader->recordGet;
    recordPut = pImporter->pHeader->recordPut;
    portAtomicMemoryFenceLoad();
    newVardataGet = pImporter->vardataGet;

    while (recordGet != recordPut)
    {
        NvU32 slotIdx;
        const NV_GPU_OPERATIONAL_EVENT_RECORD *pRec;
        NvU32       vardataEndRaw;
        NvU32       vardataStart;
        NvU32       vardataLen;
        const NvU8 *pVardataWindow = NULL;
        NV_STATUS   status;

        slotIdx = recordGet % GOEIMPORT_RECORD_COUNT;
        pRec = (const NV_GPU_OPERATIONAL_EVENT_RECORD *)
               (pImporter->pRecords + (NvU64)slotIdx * GOEIMPORT_RECORD_SIZE);

        //
        // Decode wire vardata window. Layout per NV_EVENT_BUFFER_RECORD_HEADER: high bits = end-
        // offset; low bit 0 = window starts at vardata 0 (the ring wrapped before / at this
        // record).
        //
        vardataEndRaw = pRec->event.header.varData &
                            NV_EVENT_VARDATA_OFFSET_MASK;
        if (pRec->event.header.varData & NV_EVENT_VARDATA_START_OFFSET_ZERO)
            vardataStart = 0;
        else
            vardataStart = newVardataGet;

        vardataLen = pRec->event.totalContextSize;

        if (vardataLen > 0)
        {
            NvU32 expectedEnd;

            if (vardataStart >= GOEIMPORT_VARDATA_BUFFER_SIZE ||
                vardataLen > GOEIMPORT_VARDATA_BUFFER_SIZE - vardataStart)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "GpuOpEventImport: vardata bounds error "
                          "in slot %u (vardataStart=%u, len=%u, "
                          "ringSize=%u); fatal.\n",
                          slotIdx, vardataStart, vardataLen,
                          GOEIMPORT_VARDATA_BUFFER_SIZE);
                pImporter->bFatalErrorRequested = NV_TRUE;
                break;
            }

            //
            // Producer pads to NV_EVENT_VARDATA_GRANULARITY; mismatched end offset means wire
            // framing is corrupt.
            //
            expectedEnd = vardataStart +
                          NV_ALIGN_UP(vardataLen,
                                      NV_EVENT_VARDATA_GRANULARITY);
            if (expectedEnd != vardataEndRaw)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "GpuOpEventImport: vardata framing error "
                          "in slot %u (start=%u, len=%u -> expected "
                          "end=%u, got end=%u); fatal.\n",
                          slotIdx, vardataStart, vardataLen,
                          expectedEnd, vardataEndRaw);
                pImporter->bFatalErrorRequested = NV_TRUE;
                break;
            }

            pVardataWindow = pImporter->pVardata + vardataStart;
        }

        status = _goeimportProcessRecord(pImporter, pRec,
                                            pVardataWindow, vardataLen);
        if (status != NV_OK)
        {
            //
            // Per-record build failure is fatal: publishing a span with gaps the producer didn't
            // leave would corrupt downstream state.
            //
            NV_PRINTF(LEVEL_ERROR,
                      "GpuOpEventImport: record build failed "
                      "(0x%x) in slot %u; fatal.\n", status, slotIdx);
            pImporter->bFatalErrorRequested = NV_TRUE;
            break;
        }

        newVardataGet = vardataEndRaw;
        recordGet = (recordGet + 1) % GOEIMPORT_RECORD_COUNT;
    }

    if (pImporter->bFatalErrorRequested)
    {
        //
        // Abort the open span; UPDATE_GET still fires below so the producer's notify gate clears
        // even on shutdown.
        //
        _goeimportAbortSpan(pImporter);
        pImporter->bStreamingDisabled = NV_TRUE;
    }

    // Publish recordGet; GSP can now reclaim slots up to this cursor.
    pImporter->pHeader->recordGet = recordGet;

    //
    // UPDATE_GET unconditional so producer's bNotifyPending clears
    // even on zero-record workitems. Failure -> terminal shutdown.
    //
    _goeimportIssueUpdateGet(pImporter, newVardataGet);
}

