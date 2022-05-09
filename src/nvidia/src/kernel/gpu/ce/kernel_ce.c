/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "core/locks.h"
#include "gpu/ce/kernel_ce.h"
#include "gpu/ce/kernel_ce_private.h"
#include "gpu/eng_desc.h"
#include "gpu/subdevice/subdevice.h"
#include "gpu_mgr/gpu_mgr.h"
#include "kernel/gpu/intr/intr_service.h"
#include "kernel/gpu/nvlink/kernel_nvlink.h"
#include "nvRmReg.h"

NV_STATUS kceConstructEngine_IMPL(OBJGPU *pGpu, KernelCE *pKCe, ENGDESCRIPTOR engDesc)
{
    NV_ASSERT_OR_RETURN(!RMCFG_FEATURE_PLATFORM_GSP, NV_ERR_NOT_SUPPORTED);

    NvU32 thisPublicID = GET_CE_IDX(engDesc);

    NV_PRINTF(LEVEL_INFO, "KernelCE: thisPublicID = %d\n", thisPublicID);

    pKCe->publicID = thisPublicID;
    pKCe->bIsAutoConfigEnabled = NV_TRUE;
    pKCe->bUseGen4Mapping = NV_FALSE;

    NvU32 data32 = 0;
    if ((osReadRegistryDword(pGpu, NV_REG_STR_RM_CE_ENABLE_AUTO_CONFIG, &data32) == NV_OK) &&
        (data32 == NV_REG_STR_RM_CE_ENABLE_AUTO_CONFIG_FALSE))
    {
        NV_PRINTF(LEVEL_INFO, "Disable CE Auto PCE-LCE Config\n");
        pKCe->bIsAutoConfigEnabled = NV_FALSE;
    }

    if ((osReadRegistryDword(pGpu, NV_REG_STR_RM_CE_USE_GEN4_MAPPING, &data32) == NV_OK) &&
        (data32 == NV_REG_STR_RM_CE_USE_GEN4_MAPPING_TRUE))
    {
        NV_PRINTF(LEVEL_INFO, "GEN4 mapping will use a HSHUB PCE (if available) for PCIe!\n");
        pKCe->bUseGen4Mapping = NV_TRUE;
    }

    // OBJCE::isPresent would compute this first
    pGpu->numCEs++;

    return NV_OK;
}

NvBool kceIsPresent_IMPL(OBJGPU *pGpu, KernelCE *kce)
{
    // Use bus/fifo to detemine if LCE(i) is present.
    KernelBus *pKernelBus = GPU_GET_KERNEL_BUS(pGpu);
    NvBool present = NV_FALSE;

    NV_ASSERT_OR_RETURN(pKernelBus != NULL, NV_FALSE);
    present = kbusCheckEngine_HAL(pGpu, pKernelBus, ENG_CE(kce->publicID));

    NV_PRINTF(LEVEL_INFO, "KCE %d / %d: present=%d\n", kce->publicID,
        pGpu->numCEs > 0 ? pGpu->numCEs - 1 : pGpu->numCEs, present);

    return present;
}

NvBool kceIsNewMissingEngineRemovalSequenceEnabled_IMPL(OBJGPU *pGpu, KernelCE *pKCe)
{
    return NV_TRUE;
}

static void printCaps(KernelCE *pKCe, NvU32 engineType, const NvU8 *capsTbl)
{
    NV_PRINTF(LEVEL_INFO, "LCE%d caps (engineType = %d)\n", pKCe->publicID, engineType);
#define PRINT_CAP(cap) NV_PRINTF(LEVEL_INFO, #cap ":%d\n", (RMCTRL_GET_CAP(capsTbl, NV2080_CTRL_CE_CAPS, cap) != 0) ? 1 : 0)

    PRINT_CAP(_CE_GRCE);
    PRINT_CAP(_CE_SHARED);
    PRINT_CAP(_CE_SYSMEM_READ);
    PRINT_CAP(_CE_SYSMEM_WRITE);
    PRINT_CAP(_CE_NVLINK_P2P);
    PRINT_CAP(_CE_SYSMEM);
    PRINT_CAP(_CE_P2P);
    PRINT_CAP(_CE_BL_SIZE_GT_64K_SUPPORTED);
    PRINT_CAP(_CE_SUPPORTS_NONPIPELINED_BL);
    PRINT_CAP(_CE_SUPPORTS_PIPELINED_BL);

}

static void kceGetNvlinkCaps(OBJGPU *pGpu, KernelCE *pKCe, NvU8 *pKCeCaps)
{
    if (kceIsCeSysmemRead_HAL(pGpu, pKCe))
            RMCTRL_SET_CAP(pKCeCaps, NV2080_CTRL_CE_CAPS, _CE_SYSMEM_READ);

    if (kceIsCeSysmemWrite_HAL(pGpu, pKCe))
        RMCTRL_SET_CAP(pKCeCaps, NV2080_CTRL_CE_CAPS, _CE_SYSMEM_WRITE);

    if (kceIsCeNvlinkP2P_HAL(pGpu, pKCe))
        RMCTRL_SET_CAP(pKCeCaps, NV2080_CTRL_CE_CAPS, _CE_NVLINK_P2P);
}

NV_STATUS kceGetDeviceCaps_IMPL(OBJGPU *pGpu, KernelCE *pKCe, NvU32 engineType, NvU8 *pKCeCaps)
{
    if (pKCe->bStubbed)
    {
        NV_PRINTF(LEVEL_INFO, "Skipping stubbed CE %d\n", pKCe->publicID);
        return NV_ERR_NOT_SUPPORTED;
    }

    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);

    //
    // Since some CE capabilities depend on the nvlink topology,
    // trigger topology detection before updating the CE caps
    //
    if (pKernelNvlink != NULL && !knvlinkIsForcedConfig(pGpu, pKernelNvlink))
    {
        knvlinkCoreGetRemoteDeviceInfo(pGpu, pKernelNvlink);
    }

    portMemSet(pKCeCaps, 0, NV2080_CTRL_CE_CAPS_TBL_SIZE);

    NV2080_CTRL_CE_GET_CAPS_V2_PARAMS physicalCaps;
    portMemSet(&physicalCaps, 0, sizeof(physicalCaps));

    physicalCaps.ceEngineType = NV2080_ENGINE_TYPE_COPY(pKCe->publicID);
    NV_PRINTF(LEVEL_INFO, "Querying caps for LCE(%d)\n", pKCe->publicID);

    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV_ASSERT_OK_OR_RETURN(pRmApi->Control(pRmApi,
                            pGpu->hInternalClient,
                            pGpu->hInternalSubdevice,
                            NV2080_CTRL_CMD_CE_GET_PHYSICAL_CAPS,
                            &physicalCaps,
                            sizeof(physicalCaps)));

    portMemCopy(pKCeCaps,
                NV2080_CTRL_CE_CAPS_TBL_SIZE,
                physicalCaps.capsTbl,
                NV2080_CTRL_CE_CAPS_TBL_SIZE);

    if (pKernelNvlink != NULL)
        kceGetNvlinkCaps(pGpu, pKCe, pKCeCaps);

    printCaps(pKCe, engineType, pKCeCaps);

    return NV_OK;
}

NV_STATUS
subdeviceCtrlCmdCeGetAllCaps_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_CE_GET_ALL_CAPS_PARAMS *pCeCapsParams
)
{
    OBJGPU      *pGpu = GPU_RES_GET_GPU(pSubdevice);
    KernelMIGManager *pKernelMIGManager = NULL;
    MIG_INSTANCE_REF migRef;

    ct_assert(ENG_CE__SIZE_1 <= sizeof(pCeCapsParams->capsTbl) / sizeof(pCeCapsParams->capsTbl[0]));

    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);

    //
    // Since some CE capabilities depend on the nvlink topology,
    // trigger topology detection before updating the CE caps
    //
    if (pKernelNvlink != NULL && !knvlinkIsForcedConfig(pGpu, pKernelNvlink))
    {
        knvlinkCoreGetRemoteDeviceInfo(pGpu, pKernelNvlink);
    }

    if (IS_MIG_IN_USE(pGpu))
    {
        KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
        NvHandle hClient = RES_GET_CLIENT_HANDLE(pSubdevice);

        NV_CHECK_OK_OR_RETURN(
            LEVEL_ERROR,
            kmigmgrGetInstanceRefFromClient(pGpu, pKernelMIGManager,
                                            hClient, &migRef));
    }

    portMemSet(pCeCapsParams, 0, sizeof(pCeCapsParams));

    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV_ASSERT_OK_OR_RETURN(pRmApi->Control(pRmApi,
                            pGpu->hInternalClient,
                            pGpu->hInternalSubdevice,
                            NV2080_CTRL_CMD_CE_GET_ALL_PHYSICAL_CAPS,
                            pCeCapsParams,
                            sizeof(*pCeCapsParams)));

    for (NvU32 i = 0; i < ENG_CE__SIZE_1; i++)
    {
        KernelCE *pKCe = GPU_GET_KCE(pGpu, i);
        if (pKCe == NULL || pKCe->bStubbed)
        {
            NV_PRINTF(LEVEL_INFO, "Skipping missing or stubbed CE %d\n", i);
            continue;
        }

        if (IS_MIG_IN_USE(pGpu) &&
            !kmigmgrIsEngineInInstance(pGpu, pKernelMIGManager, NV2080_ENGINE_TYPE_COPY(i), migRef))
        {
            NV_PRINTF(LEVEL_INFO, "Skipping CE%d not that is not in the MIG instance\n", i);
            continue;
        }

        pCeCapsParams->present |= BIT(i);

        NvU8 *pKCeCaps = pCeCapsParams->capsTbl[i];

        if (pKernelNvlink != NULL)
            kceGetNvlinkCaps(pGpu, pKCe, pKCeCaps);
    }

    return NV_OK;
}

/*!
 * Determine appropriate CEs for sysmem read/write and P2P over NVLINK.
 *
 * @param[in]   pGpu           OBJGPU pointer
 * @param[in]   pKCe            OBJCE pointer
 * @param[in]   gpuMask        Mask of GPUs for determining P2P CEs
 * @param[out]  pSysmemReadCE  Pointer to fill out the LCE for sysmem read
 * @param[out]  pSysmemWriteCE Pointer to fill out the LCE for sysmem write
 * @param[out]  pP2PCE         Pointer to fill out the LCE for P2P
 */
NV_STATUS
kceGetCeFromNvlinkConfig_IMPL
(
    OBJGPU   *pGpu,
    KernelCE *pKCe,
    NvU32     gpuMask,
    NvU32    *pSysmemReadCE,
    NvU32    *pSysmemWriteCE,
    NvU32    *nvlinkP2PCeMask
)
{
    NV_STATUS  rmStatus;
    NvU32      gpuCount;
    NV2080_CTRL_CMD_NVLINK_GET_NVLINK_CAPS_PARAMS nvlinkCapsParams = {0};
    NvU8      *nvlinkCaps;

    gpuCount = gpumgrGetSubDeviceCount(gpuMask);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, !IsGP100(pGpu) || gpuCount <= 2, NV_ERR_INVALID_STATE);

    rmStatus = knvlinkCtrlCmdBusGetNvlinkCaps(pGpu, &nvlinkCapsParams);
    NV_ASSERT_OK_OR_RETURN(rmStatus);

    nvlinkCaps = (NvU8*)&nvlinkCapsParams.capsTbl;

    // Check if GPU supports NVLink
    if (NV2080_CTRL_NVLINK_GET_CAP(nvlinkCaps, NV2080_CTRL_NVLINK_CAPS_SUPPORTED))
    {
        // Check if GPU supports NVLink for SYSMEM
        if (NV2080_CTRL_NVLINK_GET_CAP(nvlinkCaps, NV2080_CTRL_NVLINK_CAPS_SYSMEM_ACCESS))
            kceGetSysmemRWLCEs(pKCe, pSysmemReadCE, pSysmemWriteCE);

        // Check if GPU supports NVLink for P2P
        if (NV2080_CTRL_NVLINK_GET_CAP(nvlinkCaps, NV2080_CTRL_NVLINK_CAPS_P2P_SUPPORTED))
            rmStatus = kceGetP2PCes(pKCe, pGpu, gpuMask, nvlinkP2PCeMask);
    }

    return rmStatus;
}

NV_STATUS kceUpdateClassDB_KERNEL(OBJGPU *pGpu, KernelCE *pKCe)
{
    RM_API *pRmApi     = GPU_GET_PHYSICAL_RMAPI(pGpu);

    NV2080_CTRL_CE_UPDATE_CLASS_DB_PARAMS params;

    NV_STATUS status = pRmApi->Control(pRmApi,
                             pGpu->hInternalClient,
                             pGpu->hInternalSubdevice,
                             NV2080_CTRL_CMD_CE_UPDATE_CLASS_DB,
                             &params,
                             sizeof(params));
    NV_ASSERT_OK_OR_RETURN(status);

    // For each LCE, check if it is stubbed out in GSP-RM
    for (NvU32 i = 0; i < gpuGetNumCEs(pGpu); i++)
    {
        KernelCE *pKCe = GPU_GET_KCE(pGpu, i);

        if (pKCe)
        {
            NvBool stubbed = ((BIT(i) & params.stubbedCeMask)) != 0;
            // If this CE has no PCEs assigned, remove it from classDB
            if (stubbed)
            {
                NV_PRINTF(LEVEL_INFO, "Stubbing CE %d\n", i);
                pKCe->bStubbed = NV_TRUE;

                status = gpuDeleteClassFromClassDBByEngTag(pGpu, ENG_CE(i));
            }
            else
            {
                // If a new CE needs to be added because of the new mappings
                NV_PRINTF(LEVEL_INFO, "Unstubbing CE %d\n", i);
                pKCe->bStubbed = NV_FALSE;

                status = gpuAddClassToClassDBByEngTag(pGpu, ENG_CE(i));
            }
        }
    }

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, gpuUpdateEngineTable(pGpu));

    return NV_OK;
}

/**
 * @brief Provides an opportunity to register some IntrService during intrStateInit.
 */
void
kceRegisterIntrService_IMPL
(
    OBJGPU            *pGpu,
    KernelCE          *pKCe,
    IntrServiceRecord pRecords[MC_ENGINE_IDX_MAX]
)
{
    NvU16 engineIdx = MC_ENGINE_IDX_CE(pKCe->publicID);

    NV_ASSERT(pRecords[engineIdx].pNotificationService == NULL);
    pRecords[engineIdx].bFifoWaiveNotify = NV_FALSE;
    pRecords[engineIdx].pNotificationService = staticCast(pKCe, IntrService);
}

/**
 * @brief Services the nonstall interrupt.
 *
 * @param[in] pGpu
 * @param[in] pKCe    The IntrService object registered to handle the engineIdx nonstall interrupt.
 * @param[in] pParams
 *
 */
NV_STATUS
kceServiceNotificationInterrupt_IMPL
(
    OBJGPU            *pGpu,
    KernelCE          *pKCe,
    IntrServiceServiceNotificationInterruptArguments *pParams
)
{
    NV_ASSERT_OR_RETURN(pParams != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pParams->engineIdx == MC_ENGINE_IDX_CE(pKCe->publicID), NV_ERR_GENERIC);

    NV_PRINTF(LEVEL_INFO, "for CE%d\n", pKCe->publicID);

    MODS_ARCH_REPORT(NV_ARCH_EVENT_NONSTALL_CE, "%s", "processing CE nonstall interrupt\n");

    NV_ASSERT(NV2080_NOTIFIERS_CE(pKCe->publicID));

    kceNonstallIntrCheckAndClear_HAL(pGpu, pKCe, pParams->pThreadState);

    // Wake up channels waiting on this event
    engineNonStallIntrNotify(pGpu,
        NV2080_ENGINE_TYPE_COPY0 + NV2080_NOTIFIERS_CE(pKCe->publicID) - NV2080_NOTIFIERS_CE0);

    return NV_OK;
}

NV_STATUS kceTopLevelPceLceMappingsUpdate_IMPL(OBJGPU *pGpu, KernelCE *pKCe)
{
    NvU32        pceLceMap[NV2080_CTRL_MAX_PCES]    = {0};
    NvU32        grceConfig[NV2080_CTRL_MAX_GRCES]  = {0};
    NvU32        exposeCeMask        = 0;
    NvBool       bUpdateNvlinkPceLce = NV_FALSE;
    NV_STATUS    status              = NV_OK;
    KernelNvlink *pKernelNvlink      = GPU_GET_KERNEL_NVLINK(pGpu);

    //
    // Sync class DB before proceeding with the algorithm.
    // This is needed if mapping update previously originated in physical RM
    //
    NV_ASSERT_OK_OR_RETURN(kceUpdateClassDB_HAL(pGpu, pKCe));

    if (pKernelNvlink && !knvlinkIsForcedConfig(pGpu, pKernelNvlink))
    {
        //
        // If not GSP-RM, get the auto-config PCE-LCE mappings for NVLink topology.
        // This should work fine on CPU-RM and monolithic RM.
        //

        // Set bUpdateNvlinkPceLce to auto-config status
        bUpdateNvlinkPceLce = pKCe->bIsAutoConfigEnabled;

        if (bUpdateNvlinkPceLce)
        {
            status = kceGetNvlinkAutoConfigCeValues_HAL(pGpu, pKCe, pceLceMap,
                                                   grceConfig, &exposeCeMask);
            if (status == NV_ERR_NOT_SUPPORTED)
            {
                NV_PRINTF(LEVEL_INFO,
                    "CE AutoConfig is not supported. Skipping PCE2LCE update\n");

                bUpdateNvlinkPceLce = NV_FALSE;
            }
            else
            {
                if (status != NV_OK)
                {
                    NV_PRINTF(LEVEL_ERROR,
                        "Failed to get auto-config PCE-LCE mappings. Return\n");
                    return status;
                }
            }
        }
    }

    //
    // Pass these values to the ceUpdatePceLceMappings_HAL.
    //
    // bUpdateNvlinkPceLce will have the following values:
    //     a. NV_FALSE when auto-config is disabled through regkey.
    //     b. NV_FALSE when NVLink does not exist or NVLink topology is forced.
    //        ceUpdatePceLceMappings_HAL still have to be called because PCE-LCE
    //        mappings can be updated for reasons other than NVLink topology.
    //     c. NV_TRUE when (a) and (b) does not hold true and platform is CPU-RM
    //        or monolithic RM. For GSP-RM, value is NV_FALSE because GSP-RM does
    //        not store the NVLink topology.
    //
    // exposeCeMask will be 0x0 when bUpdateNvlinkPceLce is NV_FALSE.
    //
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV2080_CTRL_CE_UPDATE_PCE_LCE_MAPPINGS_PARAMS params;

    if (bUpdateNvlinkPceLce)
    {
        portMemCopy(params.pceLceMap,
                    sizeof(params.pceLceMap),
                    pceLceMap,
                    sizeof(pceLceMap));

        portMemCopy(params.grceConfig,
                    sizeof(params.grceConfig),
                    grceConfig,
                    sizeof(grceConfig));
    }

    params.exposeCeMask = exposeCeMask;
    params.bUpdateNvlinkPceLce = bUpdateNvlinkPceLce;

    // For GSP clients, the update needs to be routed through ctrl call
    status = pRmApi->Control(pRmApi,
                             pGpu->hInternalClient,
                             pGpu->hInternalSubdevice,
                             NV2080_CTRL_CMD_CE_UPDATE_PCE_LCE_MAPPINGS,
                             &params,
                             sizeof(params));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
            "Failed to update PCE-LCE mappings. Return\n");
        return status;
    }

    //
    // After the mappings are updated, LCEs which do not have PCEs mapped need
    // to be stubbed out and LCEs which have PCEs need to be un-stubbed. This
    // happens as a part of ceUpdatePceLceMappings_HAL which gets executed in
    // GSP/monolithic RM. For CPU-RM, have to call this function explicitly.
    //
    status = kceUpdateClassDB_HAL(pGpu, pKCe);

    return status;
}

NV_STATUS kceGetFaultMethodBufferSize_IMPL(OBJGPU *pGpu, NvU32 *size)
{
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV2080_CTRL_CE_GET_FAULT_METHOD_BUFFER_SIZE_PARAMS params;

    NV_ASSERT_OK_OR_RETURN(pRmApi->Control(pRmApi, pGpu->hInternalClient, pGpu->hInternalSubdevice,
            NV2080_CTRL_CMD_CE_GET_FAULT_METHOD_BUFFER_SIZE, &params, sizeof(params)));

    *size = params.size;
    return NV_OK;
}

/*!
 * Gets the HSHUB/FBHUB PCE masks and updates NVLINK_TOPOLOGY_PARAMS.
 *
 * @param[in]  pGpu         OBJGPU pointer
 * @param[in]  pTopoParams  Pointer to NVLINK_TOPOLOGY_PARAMS
 *
 * Returns NV_OK if array is filled successfully.
 *         NV_ERR_INVALID_ARGUMENT if pTopoParams is NULL or aray size is not equal.
 */
NV_STATUS
kceGetAvailableHubPceMask_IMPL
(
    OBJGPU *pGpu,
    NVLINK_TOPOLOGY_PARAMS *pTopoParams
)
{
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV2080_CTRL_CE_GET_HUB_PCE_MASK_PARAMS params;

    NV_ASSERT_OR_RETURN(pTopoParams != NULL, NV_ERR_INVALID_ARGUMENT);
    ct_assert(NV_ARRAY_ELEMENTS(pTopoParams->pceAvailableMaskPerHshub) == NV_ARRAY_ELEMENTS(params.hshubPceMasks));

    NV_ASSERT_OK_OR_RETURN(
        pRmApi->Control(pRmApi,
                        pGpu->hInternalClient,
                        pGpu->hInternalSubdevice,
                        NV2080_CTRL_CMD_CE_GET_HUB_PCE_MASK,
                        &params,
                        sizeof(params))
    );

    portMemCopy(pTopoParams->pceAvailableMaskPerHshub,
                sizeof(pTopoParams->pceAvailableMaskPerHshub),
                params.hshubPceMasks,
                sizeof(pTopoParams->pceAvailableMaskPerHshub));
    pTopoParams->fbhubPceMask = params.fbhubPceMask;

    return NV_OK;
}
