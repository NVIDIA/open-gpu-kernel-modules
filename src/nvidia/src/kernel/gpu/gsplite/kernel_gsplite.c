/*
 * SPDX-FileCopyrightText: Copyright (c) 2024-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/****************************************************************************
*
*   Kernel GSPLITE module
*
****************************************************************************/

#include "kernel/gpu/gsplite/kernel_gsplite.h"
#include "kernel/gpu/mem_mgr/rm_page_size.h"
#include "kernel/os/os.h"
#include "nvrm_registry.h"

// Setup registry based overrides
static void      _kgspliteInitRegistryOverrides(OBJGPU *, KernelGsplite *);

// Logging start/stop calls
static NV_STATUS _kgspliteStartLogPolling(OBJGPU *, KernelGsplite *);
static void      _kgspliteStopLogPolling(OBJGPU *, KernelGsplite *);

NV_STATUS
kgspliteConstructEngine_IMPL(OBJGPU *pGpu, KernelGsplite *pKernelGsplite, ENGDESCRIPTOR engDesc)
{
    // Initialize PublicId
    pKernelGsplite->PublicId = GET_KERNEL_GSPLITE_IDX(engDesc);

    {
        pKernelGsplite->setProperty(pKernelGsplite, PDB_PROP_KGSPLITE_IS_MISSING, NV_TRUE);
    }

    // Initialize based on registry keys
    _kgspliteInitRegistryOverrides(pGpu, pKernelGsplite);

    return NV_OK;
}

NV_STATUS
kgspliteStateInitUnlocked_IMPL
(
    OBJGPU        *pGpu,
    KernelGsplite *pKernelGsplite
)
{
    if (pKernelGsplite->getProperty(pKernelGsplite, PDB_PROP_KGSPLITE_ENABLE_CMC_NVLOG))
    {
        NV_ASSERT_OK_OR_RETURN(kgspliteInitLibosLoggingStructures(pGpu, pKernelGsplite));
    }

    return NV_OK;
}

NV_STATUS
kgspliteStateInitLocked_IMPL
(
    OBJGPU        *pGpu,
    KernelGsplite *pKernelGsplite
)
{
    if (pKernelGsplite->getProperty(pKernelGsplite, PDB_PROP_KGSPLITE_ENABLE_CMC_NVLOG) && (pKernelGsplite->pLogMemDesc != NULL))
    {
        NV_ASSERT_OK_OR_RETURN(kgspliteSendLibosLoggingStructuresInfo(pGpu, pKernelGsplite));

        NV_ASSERT_OK_OR_RETURN(_kgspliteStartLogPolling(pGpu, pKernelGsplite));
    }

    return NV_OK;
}

void kgspliteDestruct_IMPL
(
    KernelGsplite *pKernelGsplite
)
{
    OBJGPU        *pGpu = ENG_GET_GPU(pKernelGsplite);
    if (pKernelGsplite->getProperty(pKernelGsplite, PDB_PROP_KGSPLITE_ENABLE_CMC_NVLOG))
    {
        _kgspliteStopLogPolling(pGpu, pKernelGsplite);

        kgspliteFreeLibosLoggingStructures(pGpu, pKernelGsplite);
    }
}

/*!
 * Initialize all registry overrides for this object
 */
static void
_kgspliteInitRegistryOverrides
(
    OBJGPU         *pGpu,
    KernelGsplite  *pKernelGsplite
)
{
    NvU32  data;

    /* Temporarily use regkey to determine if KernelGsplite should be present or destroyed */
    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_GSPLITE_ENABLE_MASK, &data) == NV_OK)
    {
        if ((data & NVBIT(pKernelGsplite->PublicId)) != 0)
        {
            NV_PRINTF(LEVEL_INFO, "KernelGsplite%d enabled due to regkey override.\n", pKernelGsplite->PublicId);

            pKernelGsplite->setProperty(pKernelGsplite, PDB_PROP_KGSPLITE_IS_MISSING, NV_FALSE);
        }
        else
        {
            NV_PRINTF(LEVEL_INFO, "KernelGsplite%d missing due to lack of regkey override.\n", pKernelGsplite->PublicId);

            pKernelGsplite->setProperty(pKernelGsplite, PDB_PROP_KGSPLITE_IS_MISSING, NV_TRUE);
        }
    }

    NV_PRINTF(LEVEL_INFO, "KernelGsplite%d: %s\n", pKernelGsplite->PublicId,
              pKernelGsplite->getProperty(pKernelGsplite, PDB_PROP_KGSPLITE_IS_MISSING) ? "disabled" : "enabled");

    return;
}


/*! Init libos CMC-UCODE logging */
NV_STATUS
kgspliteInitLibosLoggingStructures_IMPL
(
    OBJGPU        *pGpu,
    KernelGsplite *pKernelGsplite
)
{
    NV_STATUS              status                      = NV_OK;

#if LIBOS_LOG_ENABLE
    const BINDATA_STORAGE *pBinStorageRiscvElfFileData = NULL;
    NvU32                  logElfSize                  = 0;
    NvP64                  pVa                         = NvP64_NULL;
    NvP64                  pPriv                       = NvP64_NULL;

    NV_ASSERT_OR_RETURN(pKernelGsplite->pLogBuf == NULL, NV_ERR_INVALID_STATE);

    // Get CMC-UCODE elf from RM bindata

    pKernelGsplite->pLogElf = NULL;
    if (pBinStorageRiscvElfFileData != NULL)
    {
        logElfSize = bindataGetBufferSize(pBinStorageRiscvElfFileData);

        // Load elf into memory
        pKernelGsplite->pLogElf = portMemAllocNonPaged(logElfSize);
        NV_ASSERT_OK_OR_GOTO(status,
                             bindataWriteToBuffer(pBinStorageRiscvElfFileData, pKernelGsplite->pLogElf, logElfSize),
                             cleanup);
    }

    // Set log buffer size
    pKernelGsplite->logBufSize = 0x10000; //CMC_LOG_BUFFER_SIZE;

    // Create log buffer memdesc
    NV_ASSERT_OK_OR_GOTO(status,
                         memdescCreate(&pKernelGsplite->pLogMemDesc,
                                       pGpu,
                                       pKernelGsplite->logBufSize,
                                       RM_PAGE_SIZE,
                                       NV_TRUE,
                                       ADDR_SYSMEM,
                                       NV_MEMORY_CACHED,
                                       MEMDESC_FLAGS_NONE),
                         cleanup);

    // Allocate log buffer
    memdescTagAlloc(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_CMC_LOG_BUFFER, pKernelGsplite->pLogMemDesc);
    NV_ASSERT_OK_OR_GOTO(status, status, cleanup);

    // Map log buffer for RM to dump logs
    NV_ASSERT_OK_OR_GOTO(status,
                         memdescMap(pKernelGsplite->pLogMemDesc, 0,
                                    memdescGetSize(pKernelGsplite->pLogMemDesc),
                                    NV_TRUE,
                                    NV_PROTECT_READ_WRITE,
                                    &pVa,
                                    &pPriv),
                         cleanup);
    pKernelGsplite->pLogBuf = pVa;
    pKernelGsplite->pLogBufPriv = pPriv;
    portMemSet(pKernelGsplite->pLogBuf, 0, pKernelGsplite->logBufSize);

    // Create CMC-UCODE log
    libosLogCreateEx(&pKernelGsplite->logDecode, "CMC");

    // Add CMC-UCODE log buffer
    libosLogAddLogEx(&pKernelGsplite->logDecode,
                     pKernelGsplite->pLogBuf,
                     pKernelGsplite->logBufSize,
                     pGpu->gpuInstance,
                     (gpuGetChipArch(pGpu) >> GPU_ARCH_SHIFT),
                     gpuGetChipImpl(pGpu),
                     "UCODE", NULL, 0, NULL,
                     LIBOS_LOG_NVLOG_BUFFER_VERSION, 0);

    // Finish CMC-UCODE log init (setting the async-print flag and resolve-pointers flag)
    libosLogInitEx(&pKernelGsplite->logDecode, pKernelGsplite->pLogElf, NV_FALSE, NV_TRUE, NV_TRUE, logElfSize);

cleanup:
    if (status != NV_OK)
    {
        kgspliteFreeLibosLoggingStructures(pGpu, pKernelGsplite);
    }
#endif // LIBOS_LOG_ENABLE

    return status;
}

/*! Send alloc'd logging buffer info to GSP-RM */
NV_STATUS kgspliteSendLibosLoggingStructuresInfo_IMPL
(
    OBJGPU        *pGpu,
    KernelGsplite *pKernelGsplite
)
{
    NV_STATUS status = NV_OK;

#if LIBOS_LOG_ENABLE

    RM_API                                                     *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    RmPhysAddr                                                  logBufferAddr;
    NV2080_CTRL_INTERNAL_SEND_CMC_LIBOS_BUFFER_INFO_PARAMS      params;

    if (IS_GSP_CLIENT(pGpu))
    {
        // Perform control call to send the LibOS log buffer info to CMC via GSP
        logBufferAddr           = memdescGetPhysAddr(pKernelGsplite->pLogMemDesc, AT_GPU, 0);

        params.PublicId         = pKernelGsplite->PublicId;
        params.logBufferAddr    = logBufferAddr;
        params.logBufferSize    = pKernelGsplite->pLogMemDesc->Size;
        status = pRmApi->Control(pRmApi,
                                 pGpu->hInternalClient,
                                 pGpu->hInternalSubdevice,
                                 NV2080_CTRL_CMD_INTERNAL_SEND_CMC_LIBOS_BUFFER_INFO,
                                 &params,
                                 sizeof(params));

        // Free memory alloc'd for LibOS logging structs if CMC is not supported & return NV_OK to allow RM to continue
        if (status == NV_ERR_NOT_SUPPORTED)
        {
            kgspliteFreeLibosLoggingStructures(pGpu, pKernelGsplite);
            status = NV_OK;
        }

        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Sending LibOS Log Buffer Info to CMC failed!\n");
        }
    }

    if (status != NV_OK)
    {
        kgspliteFreeLibosLoggingStructures(pGpu, pKernelGsplite);
    }

#endif // LIBOS_LOG_ENABLE

    return status;
}

/*! Free libos CMC-UCODE logging */
void
kgspliteFreeLibosLoggingStructures_IMPL
(
    OBJGPU        *pGpu,
    KernelGsplite *pKernelGsplite
)
{
#if LIBOS_LOG_ENABLE
    // Dump all logs before destroying buffer
    kgspliteDumpLibosLogs(pGpu, pKernelGsplite);

    // Destroy GSP log
    libosLogDestroy(&pKernelGsplite->logDecode);

    // Unmap log buffer
    if (pKernelGsplite->pLogBuf != NULL)
    {
        memdescUnmap(pKernelGsplite->pLogMemDesc,
                     NV_TRUE,
                     pKernelGsplite->pLogBuf,
                     pKernelGsplite->pLogBufPriv);
        pKernelGsplite->pLogBuf = NULL;
        pKernelGsplite->pLogBufPriv = NULL;
    }

    // Free log buffer
    if (pKernelGsplite->pLogMemDesc != NULL)
    {
        memdescFree(pKernelGsplite->pLogMemDesc);
        memdescDestroy(pKernelGsplite->pLogMemDesc);
        pKernelGsplite->pLogMemDesc = NULL;
    }

    // Free log elf memory
    if (pKernelGsplite->pLogElf != NULL)
    {
        portMemFree(pKernelGsplite->pLogElf);
        pKernelGsplite->pLogElf = NULL;
    }
#endif // LIBOS_LOG_ENABLE

    return;
}

/*! Dump CMC-UCODE logs */
void
kgspliteDumpLibosLogs_IMPL
(
    OBJGPU        *pGpu,
    KernelGsplite *pKernelGsplite
)
{
#if LIBOS_LOG_ENABLE
    if (pKernelGsplite->getProperty(pKernelGsplite, PDB_PROP_KGSPLITE_ENABLE_CMC_NVLOG))
    {
        libosExtractLogs(&pKernelGsplite->logDecode, NV_FALSE);
    }
#endif // LIBOS_LOG_ENABLE

    return;
}

// Currently enable polling option only for Unix
#if LIBOS_LOG_ENABLE && RMCFG_FEATURE_PLATFORM_UNIX
static void
_kgspliteLogPollingCallback
(
    OBJGPU *pGpu,
    void   *data
)
{
    KernelGsplite *pKernelGsplite = (KernelGsplite *)data;
    kgspliteDumpLibosLogs(pGpu, pKernelGsplite);
}

static NV_STATUS
_kgspliteStartLogPolling
(
    OBJGPU        *pGpu,
    KernelGsplite *pKernelGsplite
)
{
    return osSchedule1HzCallback(pGpu,
                                 _kgspliteLogPollingCallback,
                                 pKernelGsplite,
                                 NV_OS_1HZ_REPEAT);
}

static void
_kgspliteStopLogPolling
(
    OBJGPU        *pGpu,
    KernelGsplite *pKernelGsplite
)
{
    osRemove1HzCallback(pGpu, _kgspliteLogPollingCallback, pKernelGsplite);
}

#else // LIBOS_LOG_ENABLE && RMCFG_FEATURE_PLATFORM_UNIX

static NV_STATUS
_kgspliteStartLogPolling
(
    OBJGPU        *pGpu,
    KernelGsplite *pKernelGsplite
)
{
    return NV_OK;
}

static void
_kgspliteStopLogPolling
(
    OBJGPU        *pGpu,
    KernelGsplite *pKernelGsplite
)
{
    return;
}
#endif // LIBOS_LOG_ENABLE && RMCFG_FEATURE_PLATFORM_UNIX
