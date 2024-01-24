/*
 * SPDX-FileCopyrightText: Copyright (c) 2012-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "virtualization/common_vgpu_mgr.h"
#include "virtualization/kernel_vgpu_mgr.h"
#include "virtualization/hypervisor/hypervisor.h"

#include "ctrl/ctrl2080/ctrl2080gpu.h"

// Get vGPU license product name from license info
static char * getGridLicenseProductName(const char* licenseInfo)
{
    if (licenseInfo == NULL)
    {
        return NULL;
    }

    if (portStringCompare(licenseInfo, NV_GRID_LICENSE_FEATURE_VPC_EDITION, (portStringLength(NV_GRID_LICENSE_FEATURE_VPC_EDITION) + 1)) == 0)
    {
        return (char *)NV_GRID_LICENSED_PRODUCT_VPC;
    }
    else if (portStringCompare(licenseInfo, NV_GRID_LICENSE_FEATURE_VAPPS_EDITION, (portStringLength(NV_GRID_LICENSE_FEATURE_VAPPS_EDITION) + 1)) == 0)
    {
        return (char *)NV_GRID_LICENSED_PRODUCT_VAPPS;
    }
    else if (portStringCompare(licenseInfo, NV_GRID_LICENSE_FEATURE_VIRTUAL_WORKSTATION_EDITION, (portStringLength(NV_GRID_LICENSE_FEATURE_VIRTUAL_WORKSTATION_EDITION) + 1)) == 0)
    {
        return (char *)NV_GRID_LICENSED_PRODUCT_VWS;
    }
    else if (portStringCompare(licenseInfo, NV_GRID_LICENSE_FEATURE_GAMING_EDITION, (portStringLength(NV_GRID_LICENSE_FEATURE_GAMING_EDITION) + 1)) == 0)
    {
        return (char *)NV_GRID_LICENSED_PRODUCT_GAMING;
    }
    else if (portStringCompare(licenseInfo, NV_GRID_LICENSE_FEATURE_COMPUTE_EDITION, (portStringLength(NV_GRID_LICENSE_FEATURE_COMPUTE_EDITION) + 1)) == 0)
    {
        return (char *)NV_GRID_LICENSED_PRODUCT_COMPUTE;
    }
    else
    {
        return NULL;
    }
}

void
vgpuMgrFillVgpuType(NVA081_CTRL_VGPU_INFO *pVgpuInfo, VGPU_TYPE *pVgpuTypeNode)
{
    char *licenseProductNameBuffer = NULL;

    portMemSet(pVgpuTypeNode, 0, sizeof(VGPU_TYPE));

    pVgpuTypeNode->vgpuTypeId         = pVgpuInfo->vgpuType;
    pVgpuTypeNode->maxInstance        = pVgpuInfo->maxInstance;
    pVgpuTypeNode->numHeads           = pVgpuInfo->numHeads;
    pVgpuTypeNode->maxResolutionX     = pVgpuInfo->maxResolutionX;
    pVgpuTypeNode->maxResolutionY     = pVgpuInfo->maxResolutionY;
    pVgpuTypeNode->maxPixels          = pVgpuInfo->maxPixels;
    pVgpuTypeNode->frlConfig          = pVgpuInfo->frlConfig;
    pVgpuTypeNode->cudaEnabled        = pVgpuInfo->cudaEnabled;
    pVgpuTypeNode->eccSupported       = pVgpuInfo->eccSupported;
    pVgpuTypeNode->gpuInstanceSize    = pVgpuInfo->gpuInstanceSize;
    pVgpuTypeNode->multiVgpuSupported = pVgpuInfo->multiVgpuSupported;
    pVgpuTypeNode->vdevId             = pVgpuInfo->vdevId;
    pVgpuTypeNode->pdevId             = pVgpuInfo->pdevId;
    pVgpuTypeNode->profileSize        = pVgpuInfo->profileSize;
    pVgpuTypeNode->fbLength           = pVgpuInfo->fbLength;
    pVgpuTypeNode->gspHeapSize        = pVgpuInfo->gspHeapSize;
    pVgpuTypeNode->fbReservation      = pVgpuInfo->fbReservation;
    pVgpuTypeNode->mappableVideoSize  = pVgpuInfo->mappableVideoSize;
    pVgpuTypeNode->encoderCapacity    = pVgpuInfo->encoderCapacity;
    pVgpuTypeNode->bar1Length         = pVgpuInfo->bar1Length;
    pVgpuTypeNode->gpuDirectSupported = pVgpuInfo->gpuDirectSupported;
    pVgpuTypeNode->nvlinkP2PSupported = pVgpuInfo->nvlinkP2PSupported;
    pVgpuTypeNode->multiVgpuExclusive = pVgpuInfo->multiVgpuExclusive;
    pVgpuTypeNode->frlEnable          = pVgpuInfo->frlEnable;

    portStringCopy(
        (char *) pVgpuTypeNode->vgpuName, sizeof(pVgpuTypeNode->vgpuName),
        (char *) pVgpuInfo->vgpuName, sizeof(pVgpuInfo->vgpuName));
    portStringCopy(
        (char *) pVgpuTypeNode->vgpuClass, sizeof(pVgpuTypeNode->vgpuClass),
        (char *) pVgpuInfo->vgpuClass, sizeof(pVgpuInfo->vgpuClass));
    portStringCopy(
        (char *) pVgpuTypeNode->license, sizeof(pVgpuTypeNode->license),
        (char *) pVgpuInfo->license, sizeof(pVgpuInfo->license));
    portStringCopy(
        (char *) pVgpuTypeNode->vgpuExtraParams, sizeof(pVgpuTypeNode->vgpuExtraParams),
        (char *) pVgpuInfo->vgpuExtraParams, sizeof(pVgpuInfo->vgpuExtraParams));
    portMemCopy(
        (char *) pVgpuTypeNode->vgpuSignature, sizeof(pVgpuTypeNode->vgpuSignature),
        (char *) pVgpuInfo->vgpuSignature, sizeof(pVgpuInfo->vgpuSignature));

    // Fetch vGPU license product name from license info
    licenseProductNameBuffer = getGridLicenseProductName((const char *)pVgpuInfo->license);
    if (licenseProductNameBuffer != NULL)
    {
        portStringCopy(
            (char *)pVgpuTypeNode->licensedProductName, sizeof(pVgpuTypeNode->licensedProductName),
            (char *)licenseProductNameBuffer, (portStringLength((char *)licenseProductNameBuffer) + 1));
    }
}

NV_STATUS
vgpuMgrReserveSystemChannelIDs
(
    OBJGPU *pGpu,
    VGPU_TYPE *vgpuTypeInfo,
    NvU32 gfid,
    NvU32 *pChidOffset,
    NvU32 *pChannelCount,
    Device *pMigDevice,
    NvU32 numChannels,
    NvU16 placementId,
    NvU32 engineFifoListNumEntries,
    FIFO_ENGINE_LIST *engineFifoList
)
{
    KernelFifo *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);
    NvU64 mask = 0;
    NvU32 i;
    NvU32 flags = NVOS32_ALLOC_FLAGS_FORCE_MEM_GROWS_DOWN; // allocate from the end
    NvU64 heapOffset = 0;

    NV_ASSERT_OR_RETURN(engineFifoListNumEntries != 0, NV_ERR_INVALID_STATE);

    // If we are running with plugin-offload mode and are in GSP-RM, we
    // need to use fixed offset allocation for channel reserve
    if (RMCFG_FEATURE_PLATFORM_GSP && IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu))
        flags |= NVOS32_ALLOC_FLAGS_FIXED_ADDRESS_ALLOCATE;

   if (!RMCFG_FEATURE_PLATFORM_GSP &&
        pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_VGPU_HETEROGENEOUS_MODE) &&
        (placementId != NVA081_PLACEMENT_ID_INVALID))
    {
        /*
         * If heterogeneous mode is enabled, query chidOffset based on placement ID
         * and vGPU typeId.
         */
        NV_ASSERT_OK_OR_RETURN(kvgpumgrHeterogeneousGetChidOffset(vgpuTypeInfo->vgpuTypeId,
                                                                  placementId,
                                                                  numChannels,
                                                                  &heapOffset));

        flags |= NVOS32_ALLOC_FLAGS_FIXED_ADDRESS_ALLOCATE;
    }

    // Last entry is ENG_SW. Don't include that
    for (i = 0; i < engineFifoListNumEntries - 1; i++)
    {
        NvU32       runlistId;
        CHID_MGR   *pChidMgr;
        NvU32       currentNumChannels = numChannels;

        //
        // This code needs the physical runlist ID. ENGINE_INFO_TYPE_FIFO_TAG is not virtualized currently,
        // and hence we can use a trick to translate it to the physical runlist ID using the host's device table.
        // In the future, if we virtualize all of the data in engineData, we'll need a translation from virtual to physical IDs
        //
        NV_ASSERT_OK_OR_RETURN(kfifoEngineInfoXlate_HAL(pGpu, pKernelFifo,
                                                    ENGINE_INFO_TYPE_FIFO_TAG,
                                                    engineFifoList[i].engineData[ENGINE_INFO_TYPE_FIFO_TAG],
                                                    ENGINE_INFO_TYPE_RUNLIST,
                                                    &runlistId));
        pChidMgr = kfifoGetChidMgr(pGpu, pKernelFifo, runlistId);

        //
        // numChannels received as input argument is prevPow2(maxPhysicalChannels/vgpuTypeInfo->maxInstance)
        // So, if vgpuTypeInfo->maxInstance is not pow2, then numChannels is already reduced.
        if (hypervisorIsType(OS_HYPERVISOR_HYPERV) &&
            (RM_ENGINE_TYPE_IS_COPY(engineFifoList[i].engineData[ENGINE_INFO_TYPE_RM_ENGINE_TYPE])) &&
            ONEBITSET(vgpuTypeInfo->maxInstance))
        {
            //
            // On hyperv host, KMD/OS uses a CE channel for some paging oprations.
            // so, we need to reduce the number of channels for guest to prev
            // power of 2 and then reserve the channels.
            //
            currentNumChannels /= 2;
        }

        //
        // Some engines (like GR and GRCE) may share a single runlist.
        // So, don't process any runlist twice.
        //
        if (mask & NVBIT64(runlistId))
            continue;

        NV_ASSERT_OK_OR_RETURN(kfifoChidMgrReserveSystemChids(pGpu, pKernelFifo,
                                                              pChidMgr, currentNumChannels, flags,
                                                              gfid,
                                                              pChidOffset,
                                                              heapOffset,
                                                              pChannelCount,
                                                              pMigDevice,
                                                              engineFifoListNumEntries,
                                                              engineFifoList));

        // Mark as processed
        mask |= NVBIT64(runlistId);

        // Stop looping in case per runlist chidmgr is not enabled
        if (pKernelFifo->numChidMgrs == 1)
            break;
    }

    return NV_OK;
}

void
vgpuMgrFreeSystemChannelIDs
(
    OBJGPU *pGpu,
    NvU32 gfid,
    NvU32 *pChidOffset,
    NvU32 *pChannelCount,
    Device *pMigDevice,
    NvU32 engineFifoListNumEntries,
    FIFO_ENGINE_LIST *engineFifoList
)
{
    KernelFifo *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);
    NvU64 mask = 0;
    NvU32 i;

    NV_ASSERT_OR_RETURN_VOID(engineFifoListNumEntries != 0);

    // Last entry is ENG_SW. Don't include that
    for (i = 0; i < engineFifoListNumEntries - 1; i++)
    {
        NvU32       runlistId;
        CHID_MGR   *pChidMgr;

        //
        // This code needs the physical runlist ID. ENGINE_INFO_TYPE_FIFO_TAG is not virtualized currently,
        // and hence we can use a trick to translate it to the physical runlist ID using the host's device table.
        // In the future, if we virtualize all of the data in engineData, we'll need a translation from virtual to physical IDs
        //
        NV_ASSERT_OR_RETURN_VOID(kfifoEngineInfoXlate_HAL(pGpu, pKernelFifo,
                                                    ENGINE_INFO_TYPE_FIFO_TAG,
                                                    engineFifoList[i].engineData[ENGINE_INFO_TYPE_FIFO_TAG],
                                                    ENGINE_INFO_TYPE_RUNLIST,
                                                    &runlistId) == NV_OK);

        pChidMgr = kfifoGetChidMgr(pGpu, pKernelFifo, runlistId);

        //
        // Some engines (like GR and GRCE) may share a single runlist.
        // So, don't process any runlist twice.
        //
        if (mask & NVBIT64(runlistId))
            continue;

        NV_ASSERT_OR_RETURN_VOID(kfifoChidMgrFreeSystemChids(pGpu,
                                                            pKernelFifo,
                                                            pChidMgr,
                                                            gfid,
                                                            pChidOffset,
                                                            pChannelCount,
                                                            pMigDevice,
                                                            engineFifoListNumEntries,
                                                            engineFifoList) == NV_OK);

         // Mark the runlist as processed
         mask |= NVBIT64(runlistId);

        // Stop looping in case per runlist chidmgr is not enabled
        if (pKernelFifo->numChidMgrs == 1)
             break;
    }
}
