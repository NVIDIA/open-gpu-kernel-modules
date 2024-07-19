/*
 * SPDX-FileCopyrightText: Copyright (c) 2011-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "core/core.h"
#include "core/locks.h"
#include <rmp2pdefines.h>
#include "gpu/gpu.h"
#include "gpu/subdevice/subdevice.h"
#include "gpu/mem_sys/kern_mem_sys.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "kernel/mem_mgr/p2p.h"
#include "os/os.h"
#include "mem_mgr/vaspace.h"
#include "gpu/bus/third_party_p2p.h"
#include "gpu/device/device.h"
#include "rmapi/rs_utils.h"
#include "vgpu/rpc.h"
#include "vgpu/vgpu_events.h"
#include "gpu/bus/kern_bus.h"
#include "class/cl503c.h"


static NvBool _isSpaceAvailableForBar1P2PMapping(OBJGPU *, Subdevice *, RsClient *, NvU64);

static
NV_STATUS RmP2PValidateSubDevice
(
    ThirdPartyP2P *pThirdPartyP2P,
    OBJGPU **ppGpu
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pThirdPartyP2P->pSubdevice);
    if (pGpu == NULL)
    {
        return NV_ERR_INVALID_OBJECT_HANDLE;
    }

    API_GPU_FULL_POWER_SANITY_CHECK(pGpu, NV_TRUE, NV_FALSE);

    *ppGpu = pGpu;
    return NV_OK;
}

/*!
 * @brief frees given third party p2p memory extent
 */
static
void _freeMappingExtentInfo
(
    PCLI_THIRD_PARTY_P2P_MAPPING_EXTENT_INFO pExtentInfo
)
{
    if (pExtentInfo == NULL)
        return;

    memdescDestroy(pExtentInfo->pMemDesc);

    portMemFree(pExtentInfo);
}

/*!
 * @brief Constructs a new third party p2p memory extent
 */
static
NV_STATUS _constructMappingExtentInfo
(
    NvU64       address,
    NvU64       offset,
    NvU64       length,
    NvU64       fbApertureOffset,
    MEMORY_DESCRIPTOR *pMemDesc,
    PCLI_THIRD_PARTY_P2P_MAPPING_EXTENT_INFO *ppExtentInfo
)
{
    NV_STATUS status;
    PCLI_THIRD_PARTY_P2P_MAPPING_EXTENT_INFO pExtentInfo;
    MEMORY_DESCRIPTOR *pNewMemDesc;

    NV_ASSERT_OR_RETURN((ppExtentInfo != NULL), NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN((pMemDesc != NULL), NV_ERR_INVALID_ARGUMENT);

    *ppExtentInfo = NULL;

    pExtentInfo = portMemAllocNonPaged(
        sizeof(CLI_THIRD_PARTY_P2P_MAPPING_EXTENT_INFO));
    if (pExtentInfo == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        goto out;
    }

    status = memdescCreateSubMem(&pNewMemDesc, pMemDesc, NULL, offset, length);
    if (status != NV_OK)
    {
        goto out;
    }

    portMemSet(pExtentInfo, 0, sizeof(*pExtentInfo));

    pExtentInfo->address = address;
    pExtentInfo->length = length;
    pExtentInfo->fbApertureOffset = fbApertureOffset;
    pExtentInfo->pMemDesc = pNewMemDesc;
    pExtentInfo->refCount = 1;

    *ppExtentInfo = pExtentInfo;

out:
    if (status != NV_OK)
        _freeMappingExtentInfo(pExtentInfo);

    return status;
}

/*!
 * @brief Creates a new third party p2p memory extent
 */
static
NV_STATUS _createThirdPartyP2PMappingExtent
(
    NvU64       address,
    NvU64       length,
    NvU64       offset,
    RsClient   *pClient,
    PCLI_THIRD_PARTY_P2P_VIDMEM_INFO pVidmemInfo,
    CLI_THIRD_PARTY_P2P_MAPPING_EXTENT_INFO_LIST *pList,
    MEMORY_DESCRIPTOR *pMemDesc,
    OBJGPU     *pGpu,
    Subdevice  *pSubDevice,
    PCLI_THIRD_PARTY_P2P_MAPPING_EXTENT_INFO *ppExtentInfo,
    NvU64      *pMappingStart,
    NvU64      *pMappingLength
)
{
    NvU64 fbApertureOffset = 0;
    NvU64 fbApertureMapLength = RM_ALIGN_UP(length, NVRM_P2P_PAGESIZE_BIG_64K);
    NV_STATUS status;
    KernelBus *pKernelBus;
    PCLI_THIRD_PARTY_P2P_MAPPING_EXTENT_INFO pExtentInfoTmp;
    Device *pDevice = GPU_RES_GET_DEVICE(pSubDevice);
    NvBool bGpuLockTaken = (rmDeviceGpuLockIsOwner(gpuGetInstance(pGpu)) ||
                            rmGpuLockIsOwner());

    NV_PRINTF(LEVEL_INFO, "New allocation for address: 0x%llx\n", address);

    NV_ASSERT_OR_RETURN((pDevice != NULL), NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN((ppExtentInfo != NULL), NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN((pList != NULL), NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN((pMappingStart != NULL), NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN((pMappingLength != NULL), NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN((pMemDesc != NULL), NV_ERR_INVALID_ARGUMENT);

    if (IS_VIRTUAL(pGpu))
    {
        VGPU_STATIC_INFO *pVSI = GPU_GET_STATIC_INFO(pGpu);

        if (FLD_TEST_DRF(A080, _CTRL_CMD_VGPU_GET_CONFIG, _PARAMS_VGPU_DEV_CAPS_GPU_DIRECT_RDMA_ENABLED,
                      _FALSE, pVSI->vgpuConfig.vgpuDeviceCapsBits))
        {
            return NV_ERR_NOT_SUPPORTED;
        }
    }

    *ppExtentInfo = NULL;

    pKernelBus = GPU_GET_KERNEL_BUS(pGpu);

    //
    // By the time the mapping extent is created, the range has been already
    // verified to be correct has to fit in the memdesc.
    //
    NV_ASSERT(offset < memdescGetSize(pMemDesc));

    status = _constructMappingExtentInfo(address, offset,
                fbApertureMapLength, 0, pMemDesc, ppExtentInfo);
    if (status != NV_OK)
    {
        goto out;
    }

    if (IS_VIRTUAL(pGpu) && gpuIsWarBug200577889SriovHeavyEnabled(pGpu))
    {
        NV_RM_RPC_MAP_MEMORY(pGpu, pClient->hClient,
                             RES_GET_HANDLE(pDevice),
                             pVidmemInfo->hMemory,
                             offset,
                             fbApertureMapLength,
                             0,
                             &fbApertureOffset, status);
    }
    else
    {
        if (!bGpuLockTaken)
        {
            status = rmDeviceGpuLocksAcquire(pGpu, GPUS_LOCK_FLAGS_NONE,
                                             RM_LOCK_MODULES_P2P);
            NV_ASSERT_OR_GOTO(status == NV_OK, out);
        }

        status = kbusMapFbApertureSingle(pGpu, pKernelBus,
                                         (*ppExtentInfo)->pMemDesc, 0,
                                         &fbApertureOffset,
                                         &fbApertureMapLength,
                                         BUS_MAP_FB_FLAGS_MAP_UNICAST,
                                         pDevice);

        if (!bGpuLockTaken)
        {
            rmDeviceGpuLocksRelease(pGpu, GPUS_LOCK_FLAGS_NONE, NULL);
        }
    }
    if (status != NV_OK)
    {
        goto out;
    }

    (*ppExtentInfo)->fbApertureOffset = fbApertureOffset;

    for (pExtentInfoTmp = listHead(pList);
         pExtentInfoTmp != NULL;
         pExtentInfoTmp = listNext(pList, pExtentInfoTmp))
    {
       if (pExtentInfoTmp->address > address)
           break;
    }

    if (pExtentInfoTmp == NULL)
        listAppendExisting(pList, *ppExtentInfo);
    else
        listInsertExisting(pList, pExtentInfoTmp, *ppExtentInfo);

    pSubDevice->P2PfbMappedBytes += fbApertureMapLength;
    *pMappingLength = length;
    *pMappingStart = 0; // starts at zero in the current allocation.

out:
    if ((status != NV_OK) && (*ppExtentInfo != NULL))
    {
        NV_STATUS tmpStatus = NV_OK;

        if (fbApertureMapLength != 0)
        {
            if (IS_VIRTUAL(pGpu) && gpuIsWarBug200577889SriovHeavyEnabled(pGpu))
            {
                NV_RM_RPC_UNMAP_MEMORY(pGpu, pClient->hClient,
                                       RES_GET_HANDLE(pDevice),
                                       pVidmemInfo->hMemory,
                                       0,
                                       fbApertureOffset, tmpStatus);
            }
            else
            {
                if (!bGpuLockTaken)
                {
                    tmpStatus = rmDeviceGpuLocksAcquire(pGpu, GPUS_LOCK_FLAGS_NONE,
                                                        RM_LOCK_MODULES_P2P);
                    NV_ASSERT(tmpStatus == NV_OK);

                    if (tmpStatus != NV_OK)
                    {
                        goto cleanup;
                    }
                }

                tmpStatus = kbusUnmapFbApertureSingle(pGpu, pKernelBus,
                                                      (*ppExtentInfo)->pMemDesc,
                                                      fbApertureOffset,
                                                      fbApertureMapLength,
                                                      BUS_MAP_FB_FLAGS_MAP_UNICAST);

                if (!bGpuLockTaken)
                {
                    rmDeviceGpuLocksRelease(pGpu, GPUS_LOCK_FLAGS_NONE, NULL);
                }
            }
            NV_ASSERT(tmpStatus == NV_OK);
        }

cleanup:
        _freeMappingExtentInfo(*ppExtentInfo);
    }
    return status;
}

/*!
 * @brief Reuse an existing third party p2p allocation.
 *
 *  Determines offset in the current allocation and its size that can
 *  be reused in the new mapping.
 */
static
NV_STATUS _reuseThirdPartyP2PMappingExtent
(
    NvU64       address,
    NvU64       length,
    CLI_THIRD_PARTY_P2P_MAPPING_EXTENT_INFO_LIST *pList,
    MEMORY_DESCRIPTOR *pMemDesc,
    OBJGPU     *pGpu,
    Subdevice  *pSubDevice,
    PCLI_THIRD_PARTY_P2P_MAPPING_EXTENT_INFO *ppExtentInfo,
    NvU64      *pMappingStart,
    NvU64      *pMappingLength
)
{
    NvU64 mappingStart;
    NvU64 mappingLength;
    NV_STATUS status = NV_OK;
    PCLI_THIRD_PARTY_P2P_MAPPING_EXTENT_INFO pExtentInfo  = NULL;

    NV_ASSERT_OR_RETURN((ppExtentInfo != NULL), NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN((pList != NULL), NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN((pMappingStart != NULL), NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN((pMappingLength != NULL), NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN((pMemDesc != NULL), NV_ERR_INVALID_ARGUMENT);

    NV_PRINTF(LEVEL_INFO, "Reuse allocation for address: 0x%llx\n", address);

    pExtentInfo = *ppExtentInfo;

    mappingStart = address - pExtentInfo->address;
    mappingLength = NV_MIN((pExtentInfo->length - mappingStart), length);

    *pMappingLength = mappingLength;
    *pMappingStart = mappingStart;

    pExtentInfo->refCount++;

    return status;
}

/*!
 * @brief Frees an existing third party P2P mapping
 *
 *  Iterates over all the p2p allocations that are used in the mapping and
 *  decrements its refcount. If P2p allocation's refcount has reached zero,
 *  it is freed and usage of FB for p2p is appropriately adjusted.
 */
static
NV_STATUS RmThirdPartyP2PMappingFree
(
    RsClient   *pClient,
    OBJGPU     *pGpu,
    PCLI_THIRD_PARTY_P2P_VIDMEM_INFO pVidmemInfo,
    PCLI_THIRD_PARTY_P2P_INFO pThirdPartyP2PInfo,
    Subdevice  *pSubDevice,
    PCLI_THIRD_PARTY_P2P_MAPPING_INFO pMappingInfo
)
{
    NV_STATUS status = NV_OK;
    KernelBus                          *pKernelBus;
    NvU64                               length;
    NvU64                               mappingLength;
    NvU64                               address;
    NvU64                               startOffset;
    PCLI_THIRD_PARTY_P2P_MAPPING_EXTENT_INFO pExtentInfo = NULL;
    PCLI_THIRD_PARTY_P2P_MAPPING_EXTENT_INFO pExtentInfoNext = NULL;
    Device                             *pDevice = GPU_RES_GET_DEVICE(pSubDevice);
    NvBool                              bGpuLockTaken;
    NvBool                              bVgpuRpc;

    bGpuLockTaken = (rmDeviceGpuLockIsOwner(gpuGetInstance(pGpu)) ||
                     rmGpuLockIsOwner());

    NV_ASSERT_OR_RETURN((pGpu != NULL), NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN((pMappingInfo != NULL), NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN((pSubDevice != NULL), NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN((pThirdPartyP2PInfo != NULL), NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN((pDevice != NULL), NV_ERR_INVALID_STATE);

    if (IS_VIRTUAL(pGpu))
    {
        VGPU_STATIC_INFO *pVSI = GPU_GET_STATIC_INFO(pGpu);

        if (FLD_TEST_DRF(A080, _CTRL_CMD_VGPU_GET_CONFIG, _PARAMS_VGPU_DEV_CAPS_GPU_DIRECT_RDMA_ENABLED,
                      _FALSE, pVSI->vgpuConfig.vgpuDeviceCapsBits))
        {
            return NV_ERR_NOT_SUPPORTED;
        }
    }

    pKernelBus = GPU_GET_KERNEL_BUS(pGpu);

    length = pMappingInfo->length;
    address = pMappingInfo->address;

    bVgpuRpc = IS_VIRTUAL(pGpu) && gpuIsWarBug200577889SriovHeavyEnabled(pGpu);

    if (!bGpuLockTaken && !bVgpuRpc)
    {
        status = rmDeviceGpuLocksAcquire(pGpu, GPUS_LOCK_FLAGS_NONE,
                                         RM_LOCK_MODULES_P2P);
        NV_ASSERT_OK_OR_RETURN(status);
    }

    for(pExtentInfo = pMappingInfo->pStart; (pExtentInfo != NULL) && (length != 0);
        pExtentInfo = pExtentInfoNext)
    {
        pExtentInfoNext = listNext(&pVidmemInfo->mappingExtentList, pExtentInfo);
        startOffset = address - pExtentInfo->address;
        mappingLength = NV_MIN(length, (pExtentInfo->length - startOffset));

        address += mappingLength;
        length -= mappingLength;
        pExtentInfo->refCount--;
        if (pExtentInfo->refCount == 0)
        {
            if (bVgpuRpc)
            {
                NV_RM_RPC_UNMAP_MEMORY(pGpu, pClient->hClient,
                                       RES_GET_HANDLE(pDevice),
                                       pVidmemInfo->hMemory,
                                       0,
                                       pExtentInfo->fbApertureOffset, status);
            }
            else
            {
                status = kbusUnmapFbApertureSingle(pGpu, pKernelBus,
                                                   pExtentInfo->pMemDesc,
                                                   pExtentInfo->fbApertureOffset,
                                                   pExtentInfo->length,
                                                   BUS_MAP_FB_FLAGS_MAP_UNICAST);
            }
            NV_ASSERT(status == NV_OK);

            listRemove(&pVidmemInfo->mappingExtentList, pExtentInfo);

            pSubDevice->P2PfbMappedBytes -= pExtentInfo->length;
            _freeMappingExtentInfo(pExtentInfo);
        }
    }

    if (!bGpuLockTaken && !bVgpuRpc)
    {
        rmDeviceGpuLocksRelease(pGpu, GPUS_LOCK_FLAGS_NONE, NULL);
    }

    NV_ASSERT(length == 0);

    pMappingInfo->pStart = NULL;
    pMappingInfo->length = 0;

    return status;
}

/*!
 *  @brief Gets BAR1 mapped pages.
 *
 *  The function creates mappings from BAR1 VASpace for registered third party
 *  P2P allocations, so the pages returned by this function are BAR1 addresses,
 *  BAR1 base + BAR1 VAs returned by RM.
 *  Note that PCLI_THIRD_PARTY_P2P_MAPPING_INFO is also updated to track these
 *  BAR1 addresses in order to reuse them across multiple allocations.
 */
static
NV_STATUS RmThirdPartyP2PBAR1GetPages
(
    NvU64       address,
    NvU64       length,
    NvU64       offset,
    RsClient   *pClient,
    PCLI_THIRD_PARTY_P2P_VIDMEM_INFO pVidmemInfo,
    NvU64     **ppPhysicalAddresses,
    NvU32     **ppWreqMbH,
    NvU32     **ppRreqMbH,
    NvU32      *pEntries,
    OBJGPU     *pGpu,
    Subdevice  *pSubDevice,
    PCLI_THIRD_PARTY_P2P_MAPPING_INFO pMappingInfo,
    PCLI_THIRD_PARTY_P2P_INFO pThirdPartyP2PInfo
)
{
    NV_STATUS status = NV_OK;
    PCLI_THIRD_PARTY_P2P_MAPPING_EXTENT_INFO pExtentInfoLoop = NULL;
    PCLI_THIRD_PARTY_P2P_MAPPING_EXTENT_INFO pExtentInfo     = NULL;
    MEMORY_DESCRIPTOR *pMemDesc;
    NvU64 mappingLength = 0;
    NvU64 mappingOffset = 0;
    NvU64 lengthReq = 0;
    NvU64 lastAddress;
    NvU32 entries = 0;
    NvU64 fbApertureOffset;
    NvU64 physicalFbAddress;
    NvBool bFound;

    NV_ASSERT_OR_RETURN((pGpu != NULL), NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN((pMappingInfo != NULL), NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN((pSubDevice != NULL), NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN((pThirdPartyP2PInfo != NULL), NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN((ppPhysicalAddresses != NULL), NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN((pEntries != NULL), NV_ERR_INVALID_ARGUMENT);

    NV_PRINTF(LEVEL_INFO,
              "Requesting Bar1 mappings for address: 0x%llx, length: 0x%llx\n",
              address, length);
    *pEntries = 0;

    pMappingInfo->length = 0;
    pMappingInfo->address = address;

    pExtentInfoLoop = listHead(&pVidmemInfo->mappingExtentList);

    while (length > 0)
    {
        bFound = NV_FALSE;
        lengthReq = length;
        for(; pExtentInfoLoop != NULL; pExtentInfoLoop = listNext(&pVidmemInfo->mappingExtentList, pExtentInfoLoop))
        {
           if ((address >= pExtentInfoLoop->address) &&
               (address <
                (pExtentInfoLoop->address + pExtentInfoLoop->length)))
           {
               bFound = NV_TRUE;
               break;
           }
           else if (address < pExtentInfoLoop->address)
           {
               //
               // create new allocation for addresses that are not overlapping
               // with the next allocation.
               //
               if ((address + length) > pExtentInfoLoop->address)
               {
                   lengthReq = pExtentInfoLoop->address - address;
               }
               break;
           }
        }

        pExtentInfo = pExtentInfoLoop;

        if (!bFound)
        {
            // Check if there is still space in BAR1 to map this length
            if (!_isSpaceAvailableForBar1P2PMapping(pGpu, pSubDevice, pClient, lengthReq))
            {
                NV_PRINTF(LEVEL_ERROR,
                          "no space for BAR1 mappings, length: 0x%llx \n", lengthReq);

                status = NV_ERR_INSUFFICIENT_RESOURCES;
                goto out;
            }

            pMemDesc = pVidmemInfo->pMemDesc;
            status = _createThirdPartyP2PMappingExtent(
                        address, lengthReq, offset, pClient,
                        pVidmemInfo,
                        &pVidmemInfo->mappingExtentList, pMemDesc, pGpu,
                        pSubDevice, &pExtentInfo,
                        &mappingOffset, &mappingLength);
            if (NV_OK != status)
            {
                goto out;
            }
        }
        else
        {
            pMemDesc = pExtentInfo->pMemDesc;
            status = _reuseThirdPartyP2PMappingExtent(
                        address, lengthReq, &pVidmemInfo->mappingExtentList, pMemDesc,
                        pGpu, pSubDevice, &pExtentInfo, &mappingOffset, &mappingLength);
            if (NV_OK != status)
            {
                goto out;
            }
        }

        if (pMappingInfo->pStart == NULL)
            pMappingInfo->pStart = pExtentInfo;

        // fill page table entries
        fbApertureOffset = pExtentInfo->fbApertureOffset + mappingOffset;
        lastAddress = (address + mappingLength - 1);
        while (address < lastAddress)
        {
            if (ppWreqMbH != NULL && ppRreqMbH != NULL)
            {
                (*ppWreqMbH)[entries] = 0;
                (*ppRreqMbH)[entries] = 0;
            }

            physicalFbAddress = gpumgrGetGpuPhysFbAddr(pGpu);
            (*ppPhysicalAddresses)[entries] = (physicalFbAddress +
                                               fbApertureOffset);
            fbApertureOffset += NVRM_P2P_PAGESIZE_BIG_64K;
            address += NVRM_P2P_PAGESIZE_BIG_64K;
            offset += NVRM_P2P_PAGESIZE_BIG_64K;
            entries++;
        }

        length -= mappingLength;
        pMappingInfo->length += mappingLength;

    }

    *pEntries = entries;

out:
    if (status != NV_OK)
    {
        RmThirdPartyP2PMappingFree(pClient, pGpu, pVidmemInfo, pThirdPartyP2PInfo,
                                   pSubDevice, pMappingInfo);
    }
    return status;
}

/*!
 *  @brief Gets pages adjusted by NVLink aperture base (GPAs).
 */
static
NV_STATUS RmThirdPartyP2PNVLinkGetPages
(
    OBJGPU            *pGpu,
    NvU64              address,
    NvU64              length,
    NvU64              offset,
    MEMORY_DESCRIPTOR *pMemDesc,
    NvU32            **ppWreqMbH,
    NvU32            **ppRreqMbH,
    NvU64            **ppPhysicalAddresses,
    NvU32             *pEntries
)
{
    NvU64 lastAddress;
    NvU32 entries = 0;
    RmPhysAddr physAddr;
    KernelMemorySystem *pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);

    if (memdescGetPageSize(pMemDesc, AT_CPU) < NVRM_P2P_PAGESIZE_BIG_64K)
    {
        return NV_ERR_INVALID_STATE;
    }

    NV_ASSERT_OR_RETURN(NV_IS_ALIGNED64(address, NVRM_P2P_PAGESIZE_BIG_64K),
                        NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(NV_IS_ALIGNED64(length, NVRM_P2P_PAGESIZE_BIG_64K),
                        NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(NV_IS_ALIGNED64(offset, NVRM_P2P_PAGESIZE_BIG_64K),
                        NV_ERR_INVALID_ARGUMENT);

    lastAddress = (address + length - 1);
    while (address < lastAddress)
    {
        physAddr = memdescGetPhysAddr(pMemDesc, AT_CPU, offset);

        if ((ppWreqMbH != NULL) && (ppRreqMbH != NULL))
        {
            (*ppWreqMbH)[entries] = 0;
            (*ppRreqMbH)[entries] = 0;
        }

        (*ppPhysicalAddresses)[entries] = pKernelMemorySystem->coherentCpuFbBase + physAddr;

        address += NVRM_P2P_PAGESIZE_BIG_64K;
        offset += NVRM_P2P_PAGESIZE_BIG_64K;
        entries++;
    }

    *pEntries = entries;

    return NV_OK;
}

/*!
 *  @brief Gets pages for the given VidmemInfo
 */
static
NV_STATUS RmP2PGetPagesUsingVidmemInfo
(
    NvU64                             address,
    NvU64                             length,
    NvU64                             offset,
    ThirdPartyP2P                    *pThirdPartyP2P,
    NvU64                           **ppPhysicalAddresses,
    NvU32                           **ppWreqMbH,
    NvU32                           **ppRreqMbH,
    NvU32                            *pEntries,
    void                             *pPlatformData,
    void                            (*pFreeCallback)(void *pData),
    void                             *pData,
    OBJGPU                           *pGpu,
    Subdevice                        *pSubDevice,
    CLI_THIRD_PARTY_P2P_VASPACE_INFO *pVASpaceInfo,
    ThirdPartyP2P                    *pThirdPartyP2PInfo,
    CLI_THIRD_PARTY_P2P_VIDMEM_INFO  *pVidmemInfo
)
{
    NV_STATUS status;
    MEMORY_DESCRIPTOR *pMemDesc;
    RsClient *pClient = RES_GET_CLIENT(pThirdPartyP2P);
    CLI_THIRD_PARTY_P2P_MAPPING_INFO *pMappingInfo = NULL;

    pMemDesc = pVidmemInfo->pMemDesc;

    status = CliGetThirdPartyP2PMappingInfoFromKey(pThirdPartyP2P,
                pVidmemInfo->hMemory, pPlatformData, &pMappingInfo);
    if (status == NV_ERR_OBJECT_NOT_FOUND)
    {
        status = CliAddThirdPartyP2PMappingInfo(pThirdPartyP2P, pVidmemInfo->hMemory,
                pPlatformData, pFreeCallback, pData, &pMappingInfo);
    }
    if (status != NV_OK)
    {
        return status;
    }

    switch(pThirdPartyP2PInfo->type)
    {
        case CLI_THIRD_PARTY_P2P_TYPE_BAR1:
            status = RmThirdPartyP2PBAR1GetPages(address, length, offset, pClient,
                                                 pVidmemInfo, ppPhysicalAddresses,
                                                 ppWreqMbH, ppRreqMbH, pEntries,
                                                 pGpu, pSubDevice, pMappingInfo,
                                                 pThirdPartyP2PInfo);
            break;
        case CLI_THIRD_PARTY_P2P_TYPE_NVLINK:
            status = RmThirdPartyP2PNVLinkGetPages(pGpu, address, length,
                                                   offset, pMemDesc, ppWreqMbH,
                                                   ppRreqMbH, ppPhysicalAddresses,
                                                   pEntries);
            break;
        default:
            status = NV_ERR_NOT_SUPPORTED;
            break;
    }

    return status;
}

/*!
 *  @brief Gets pages or validates address range.
 *
 *  If the argument "ppPhysicalAddresses" is NULL,
 *  the function just validates the address range.
 */
static
NV_STATUS RmP2PValidateAddressRangeOrGetPages
(
    NvU64          address,
    NvU64          length,
    ThirdPartyP2P *pThirdPartyP2P,
    NvU64        **ppPhysicalAddresses,
    NvU32        **ppWreqMbH,
    NvU32        **ppRreqMbH,
    NvU32         *pEntries,
    void          *pPlatformData,
    void         (*pFreeCallback)(void *pData),
    void          *pData,
    OBJGPU        *pGpu,
    Subdevice     *pSubDevice,
    PCLI_THIRD_PARTY_P2P_VASPACE_INFO pVASpaceInfo,
    PCLI_THIRD_PARTY_P2P_INFO pThirdPartyP2PInfo
)
{
    CLI_THIRD_PARTY_P2P_VIDMEM_INFO *pVidmemInfo;
    NV_STATUS status;
    NvU64 offset;

    status = CliGetThirdPartyP2PVidmemInfoFromAddress(pThirdPartyP2P,
                address, length, &offset, &pVidmemInfo);
    if (status != NV_OK)
    {
        return status;
    }

    // Range validation is done at this point, so return if only validation was requested
    if (ppPhysicalAddresses == NULL)
    {
        return NV_OK;
    }

    status = RmP2PGetPagesUsingVidmemInfo(address, length, offset, pThirdPartyP2P,
                                          ppPhysicalAddresses, ppWreqMbH, ppRreqMbH,
                                          pEntries, pPlatformData, pFreeCallback,
                                          pData, pGpu, pSubDevice, pVASpaceInfo,
                                          pThirdPartyP2PInfo, pVidmemInfo);
    if (status != NV_OK)
    {
        return status;
    }

    return NV_OK;
}

static
NV_STATUS RmP2PGetVASpaceInfoWithoutToken
(
    NvU64 address,
    NvU64 length,
    void  *pPlatformData,
    void  (*pFreeCallback)(void *pData),
    void  *pData,
    ThirdPartyP2P *pThirdPartyP2P,
    PCLI_THIRD_PARTY_P2P_VASPACE_INFO *ppVASpaceInfo
)
{
    NV_STATUS status;
    PCLI_THIRD_PARTY_P2P_VASPACE_INFO pVASpaceInfo = NULL;
    NvBool bFound = NV_FALSE;
    Subdevice *pSubdevice;
    OBJGPU *pGpu;

    pSubdevice = pThirdPartyP2P->pSubdevice;

    status = RmP2PValidateSubDevice(pThirdPartyP2P, &pGpu);
    if (NV_OK != status)
    {
        return status;
    }

    if ((pThirdPartyP2P->type == CLI_THIRD_PARTY_P2P_TYPE_PROPRIETARY) &&
        !(pThirdPartyP2P->flags & CLI_THIRD_PARTY_P2P_FLAGS_INITIALIZED))
    {
        return NV_ERR_INVALID_STATE;
    }

    while (1)
    {
        status = thirdpartyp2pGetNextVASpaceInfo(pThirdPartyP2P, &pVASpaceInfo);
        if (status != NV_OK)
        {
            if (bFound)
            {
                status = NV_OK;
            }
            return status;
        }

        //
        // Passing NULL for arguments to prevent looking up or
        // updating mapping info in range validation.
        //
        status = RmP2PValidateAddressRangeOrGetPages(address, length, pThirdPartyP2P,
                                                     NULL, NULL, NULL, NULL,
                                                     pPlatformData, pFreeCallback,
                                                     pData, pGpu, pSubdevice,
                                                     pVASpaceInfo, pThirdPartyP2P);
        if ((NV_OK == status) && bFound)
        {
            return NV_ERR_GENERIC;
        }
        else if (NV_OK == status)
        {
            bFound = NV_TRUE;
        }

        if (NULL != ppVASpaceInfo)
        {
            *ppVASpaceInfo = pVASpaceInfo;
        }
    }

    return status;
}

static
NV_STATUS RmP2PGetInfoWithoutToken
(
    NvU64 address,
    NvU64 length,
    void  *pPlatformData,
    PCLI_THIRD_PARTY_P2P_INFO *ppThirdPartyP2PInfo,
    PCLI_THIRD_PARTY_P2P_VASPACE_INFO *ppVASpaceInfo,
    OBJGPU *pGpu
)
{
    NV_STATUS status;
    PCLI_THIRD_PARTY_P2P_INFO pThirdPartyP2PInfo = NULL;
    PCLI_THIRD_PARTY_P2P_VASPACE_INFO pVASpaceInfo = NULL;
    NvBool bFound = NV_FALSE;
    NvU32 processId = osGetCurrentProcess();

    while (1)
    {
        RmClient *pClient;
        status = CliNextThirdPartyP2PInfoWithPid(pGpu,
                                                 processId,
                                                 0,
                                                 &pClient,
                                                 &pThirdPartyP2PInfo);
        if (NV_OK != status)
        {
            if (bFound)
            {
                status = NV_OK;
            }
            break;
        }

        if ((pThirdPartyP2PInfo->type == CLI_THIRD_PARTY_P2P_TYPE_PROPRIETARY) &&
            !(pThirdPartyP2PInfo->flags & CLI_THIRD_PARTY_P2P_FLAGS_INITIALIZED))
        {
            status = NV_ERR_INVALID_STATE;
            continue;
        }

        if (0 == length)
        {
            // PutPages
            status = CliGetThirdPartyP2PPlatformData(pThirdPartyP2PInfo,
                                                     pPlatformData);
        }
        else
        {
            // GetPages
            status = RmP2PGetVASpaceInfoWithoutToken(address,
                                              length,
                                              pPlatformData,
                                              NULL,
                                              NULL,
                                              pThirdPartyP2PInfo,
                                              &pVASpaceInfo);
            if (NV_OK == status)
            {
                *ppVASpaceInfo = pVASpaceInfo;
            }
        }

        if (NV_OK == status)
        {
            if (bFound)
            {
                status = NV_ERR_GENERIC;
                break;
            }
            else
            {
                bFound = NV_TRUE;
                if (NULL != ppThirdPartyP2PInfo)
                {
                    *ppThirdPartyP2PInfo = pThirdPartyP2PInfo;
                }
            }
        }
    }

    return status;
}

static NvBool _isSpaceAvailableForBar1P2PMapping(
    OBJGPU    *pGpu,
    Subdevice *pSubDevice,
    RsClient  *pClient,
    NvU64      length
)
{
    NvU64 bar1SizeBytes;
    NvU64 fbAvailableBytes;
    GETBAR1INFO bar1Info;
    NV_STATUS status;
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);

    status = memmgrGetBAR1InfoForDevice(pGpu, pMemoryManager,
                                        GPU_RES_GET_DEVICE(pSubDevice),
                                        &bar1Info);
    if (status != NV_OK)
        return NV_FALSE;

    // Convert Bar1 size to bytes as reported size is in KB.
    bar1SizeBytes = ((NvU64)bar1Info.bar1Size) << 10;

    if (bar1SizeBytes <  pSubDevice->P2PfbMappedBytes)
    {
        DBG_BREAKPOINT();
        return NV_FALSE;
    }

    fbAvailableBytes = (bar1SizeBytes - pSubDevice->P2PfbMappedBytes);
    return (fbAvailableBytes >= (CLI_THIRD_PARTY_P2P_BAR1_RESERVE + length));
}

static NV_STATUS _rmP2PGetPages(
    NvU64       p2pToken,
    NvU32       vaSpaceToken,
    NvU64       address,
    NvU64       length,
    NvU64      *pPhysicalAddresses,
    NvU32      *pWreqMbH,
    NvU32      *pRreqMbH,
    NvU32      *pEntries,
    OBJGPU    **ppGpu,
    void       *pPlatformData,
    void      (*pFreeCallback)(void *pData),
    void       *pData
)
{
    NV_STATUS status;
    OBJGPU *pGpu;
    ThirdPartyP2P *pThirdPartyP2P;
    Subdevice *pSubdevice;
    PCLI_THIRD_PARTY_P2P_VASPACE_INFO pVASpaceInfo = NULL;

    NV_ASSERT_OR_RETURN(NV_IS_ALIGNED64(address, NVRM_P2P_PAGESIZE_BIG_64K),
                        NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(NV_IS_ALIGNED64(length, NVRM_P2P_PAGESIZE_BIG_64K),
                        NV_ERR_INVALID_ARGUMENT);

    if (0 != p2pToken)
    {
        status = CliGetThirdPartyP2PInfoFromToken(p2pToken,
                                                  &pThirdPartyP2P);
    }
    else
    {
        status = RmP2PGetInfoWithoutToken(address,
                                       length,
                                       pPlatformData,
                                       &pThirdPartyP2P,
                                       &pVASpaceInfo,
                                       NULL);
    }

    if (status != NV_OK)
    {
        return status;
    }

    pSubdevice = pThirdPartyP2P->pSubdevice;

    if ((pThirdPartyP2P->type == CLI_THIRD_PARTY_P2P_TYPE_PROPRIETARY) &&
        !(pThirdPartyP2P->flags & CLI_THIRD_PARTY_P2P_FLAGS_INITIALIZED))
    {
        status = NV_ERR_INVALID_STATE;
        goto failed;
    }

    status = RmP2PValidateSubDevice(pThirdPartyP2P, &pGpu);
    if (status != NV_OK)
    {
        goto failed;
    }

    if (0 != vaSpaceToken)
    {
        status = thirdpartyp2pGetVASpaceInfoFromToken(pThirdPartyP2P, vaSpaceToken, &pVASpaceInfo);
        if (status != NV_OK)
        {
            goto failed;
        }
    }

    if (pVASpaceInfo == NULL)
    {
        status = NV_ERR_INVALID_STATE;
        goto failed;
    }

    status = RmP2PValidateAddressRangeOrGetPages(address, length, pThirdPartyP2P,
                                                 &pPhysicalAddresses, &pWreqMbH,
                                                 &pRreqMbH, pEntries, pPlatformData,
                                                 pFreeCallback, pData, pGpu, pSubdevice,
                                                 pVASpaceInfo, pThirdPartyP2P);
    if (status != NV_OK)
    {
        goto failed;
    }

    if (ppGpu != NULL)
    {
        *ppGpu = pGpu;
    }

    return NV_OK;
failed:
    thirdpartyp2pDelMappingInfoByKey(pThirdPartyP2P, pPlatformData, NV_FALSE);

    return status;
}

static
CLI_THIRD_PARTY_P2P_VIDMEM_INFO* _createOrReuseVidmemInfoPersistent
(
    OBJGPU         *pGpu,
    NvU64           address,
    NvU64           length,
    NvU64          *pOffset,
    NvHandle        hClientInternal,
    ThirdPartyP2P  *pThirdPartyP2P,
    ThirdPartyP2P  *pThirdPartyP2PInternal
)
{
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    CLI_THIRD_PARTY_P2P_VIDMEM_INFO *pVidmemInfo = NULL;
    CLI_THIRD_PARTY_P2P_VIDMEM_INFO *pVidmemInfoInternal = NULL;
    Memory *pMemoryInternal;
    RsClient *pClientInternal;
    Device *pDevice;
    Subdevice *pSubdevice;
    NvU64 offset = 0;
    NvHandle hMemoryDuped = 0;
    NV_STATUS status;
    NvBool bMemDuped = NV_FALSE;

    //
    // Note: hMemory is duped(memory is ref-counted) only once for the first time.
    // All subsequent get_pages_persistent() requests reuse the same VidmemInfo.
    // Mappings are ref-counted using ExtentInfo in the MappingInfoList.
    // The duped handle is freed when the MappingInfoList is empty in
    // put_pages_persistent() path.
    //

    //
    // Get user client's ThirdPartyP2P's VidmemInfo
    // Needed to get user's offset and hMemory
    //
    status = CliGetThirdPartyP2PVidmemInfoFromAddress(pThirdPartyP2P,
                                                      address,
                                                      length,
                                                      &offset,
                                                      &pVidmemInfo);
    if (status != NV_OK)
    {
        goto failed;
    }

    *pOffset = offset;

    //
    // Check if an internal VidmemInfo already exists.
    // Every VidmemInfo is assigned a unique ID and the internal ThirdPartyP2P
    // object's AddressRangeTree is keyed at user client's VidmemInfo ID instead
    // of the VA. This is because the VA could have been reassigned to another
    // phys allocation.
    //
    status = CliGetThirdPartyP2PVidmemInfoFromId(pThirdPartyP2PInternal,
                                                 pVidmemInfo->id,
                                                 &pVidmemInfoInternal);
    if (status == NV_OK)
    {
        return pVidmemInfoInternal;
    }
    else if (status != NV_ERR_OBJECT_NOT_FOUND)
    {
        goto failed;
    }

    pClientInternal = RES_GET_CLIENT(pThirdPartyP2PInternal);
    pDevice = GPU_RES_GET_DEVICE(pThirdPartyP2PInternal);
    pSubdevice = GPU_RES_GET_SUBDEVICE(pThirdPartyP2PInternal);

    // Dupe user client's hMemory
    status = pRmApi->DupObject(pRmApi,
                               hClientInternal,
                               RES_GET_HANDLE(pDevice),
                               &hMemoryDuped,
                               pThirdPartyP2P->hClient,
                               pVidmemInfo->hMemory,
                               0);
    if (status == NV_ERR_INVALID_OBJECT_PARENT)
    {
        // If duping under Device fails, try duping under Subdevice
        status = pRmApi->DupObject(pRmApi,
                                   hClientInternal,
                                   RES_GET_HANDLE(pSubdevice),
                                   &hMemoryDuped,
                                   pThirdPartyP2P->hClient,
                                   pVidmemInfo->hMemory,
                                   0);
    }
    if (status != NV_OK)
    {
        goto failed;
    }

    bMemDuped = NV_TRUE;

    status = memGetByHandleAndDevice(pClientInternal,
                                     hMemoryDuped,
                                     RES_GET_HANDLE(pDevice),
                                     &pMemoryInternal);
    if (status != NV_OK)
    {
        goto failed;
    }

    //
    // Add a new VidmemInfo with the address field as user's VidmemInfo ID
    // and length = 1. This is because keyStart and keyEnd for internal
    // AddressRangeTree should be the user's VidmemInfo ID.
    //
    status = CliAddThirdPartyP2PVidmemInfo(pThirdPartyP2PInternal,
                                           hMemoryDuped,
                                           pVidmemInfo->id,
                                           1,
                                           pVidmemInfo->offset,
                                           pMemoryInternal);
    if (status != NV_OK)
    {
        goto failed;
    }

    // Fetch the newly added VidmemInfo to return.
    status = CliGetThirdPartyP2PVidmemInfoFromId(pThirdPartyP2PInternal,
                                                 pVidmemInfo->id,
                                                 &pVidmemInfoInternal);
    if (status != NV_OK)
    {
        goto failed;
    }

    return pVidmemInfoInternal;

failed:
    if (bMemDuped)
    {
        pRmApi->Free(pRmApi, hClientInternal, hMemoryDuped);
    }

    return NULL;
}

static NV_STATUS RmP2PGetMigInfo(
    OBJGPU                   *pGpu,
    NvU64                     address,
    NvU64                     length,
    ThirdPartyP2P            *pThirdPartyP2P,
    KERNEL_MIG_GPU_INSTANCE **ppGpuInstanceInfo
)
{
    NvHandle hClient, hMemory;
    MIG_INSTANCE_REF ref;
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    CLI_THIRD_PARTY_P2P_VIDMEM_INFO *pVidmemInfo = NULL;
    Memory *pMemory;
    RsClient *pClient;
    NvU64 offset;

    // Get hClient and hMemory
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                          CliGetThirdPartyP2PVidmemInfoFromAddress(pThirdPartyP2P,
                                        address, length, &offset, &pVidmemInfo));
    hClient = pVidmemInfo->hClient;
    hMemory = pVidmemInfo->hMemory;

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                          serverGetClientUnderLock(&g_resServ, hClient, &pClient));

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                          memGetByHandle(pClient, hMemory, &pMemory));

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                          kmigmgrGetInstanceRefFromDevice(pGpu, pKernelMIGManager,
                                                          pMemory->pDevice, &ref));

    // Refcount++ MIG instance
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                          kmigmgrIncRefCount(ref.pKernelMIGGpuInstance->pShare));

    *ppGpuInstanceInfo = (void *) ref.pKernelMIGGpuInstance;

    return NV_OK;
}

static void RmP2PPutMigInfo(
    void  *pGpuInstanceInfo
)
{
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance;

    if (pGpuInstanceInfo == NULL)
    {
        return;
    }

    pKernelMIGGpuInstance = (KERNEL_MIG_GPU_INSTANCE *) pGpuInstanceInfo;

    // Refcount-- MIG instance
    NV_ASSERT_OK(kmigmgrDecRefCount(pKernelMIGGpuInstance->pShare));
}

NV_STATUS RmP2PGetPagesPersistent(
    NvU64       address,
    NvU64       length,
    void      **p2pObject,
    NvU64      *pPhysicalAddresses,
    NvU32      *pEntries,
    void       *pPlatformData,
    void       *pGpuInfo,
    void      **ppGpuInstanceInfo
)
{
    RsResourceRef *pResourceRef;
    OBJGPU *pGpu = (OBJGPU *) pGpuInfo;
    ThirdPartyP2P *pThirdPartyP2P = NULL;
    ThirdPartyP2P *pThirdPartyP2PInternal = NULL;
    CLI_THIRD_PARTY_P2P_VASPACE_INFO *pVASpaceInfo = NULL;
    CLI_THIRD_PARTY_P2P_VIDMEM_INFO *pVidmemInfo = NULL;
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance = NULL;
    NvU64 offset = 0;
    NvHandle hClientInternal;
    NvHandle hThirdPartyP2PInternal;
    NV_STATUS status;

    NV_ASSERT_OR_RETURN(NV_IS_ALIGNED64(address, NVRM_P2P_PAGESIZE_BIG_64K),
                        NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(NV_IS_ALIGNED64(length, NVRM_P2P_PAGESIZE_BIG_64K),
                        NV_ERR_INVALID_ARGUMENT);

    if(gpuIsApmFeatureEnabled(pGpu))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    status = RmP2PGetInfoWithoutToken(address, length, NULL,
                                      &pThirdPartyP2P, &pVASpaceInfo, pGpu);
    if (status != NV_OK)
    {
        return status;
    }

    if (IS_MIG_ENABLED(pGpu))
    {
        status = RmP2PGetMigInfo(pGpu, address, length, pThirdPartyP2P,
                                 &pKernelMIGGpuInstance);
        if (status != NV_OK)
        {
            return status;
        }
        *ppGpuInstanceInfo = (void *) pKernelMIGGpuInstance;

        if (pKernelMIGGpuInstance->instanceHandles.hThirdPartyP2P == NV01_NULL_OBJECT)
        {
            status = NV_ERR_NOT_SUPPORTED;

            goto failed;
        }

        hClientInternal = pKernelMIGGpuInstance->instanceHandles.hClient;
        hThirdPartyP2PInternal = pKernelMIGGpuInstance->instanceHandles.hThirdPartyP2P;
    }
    else
    {
        MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);

        if (pMemoryManager->hThirdPartyP2P == NV01_NULL_OBJECT)
        {
            return NV_ERR_NOT_SUPPORTED;
        }

        hClientInternal = pMemoryManager->hClient;
        hThirdPartyP2PInternal = pMemoryManager->hThirdPartyP2P;
        *ppGpuInstanceInfo = NULL;
    }

    status = serverutilGetResourceRef(hClientInternal,
                                      hThirdPartyP2PInternal,
                                      &pResourceRef);
    if (status != NV_OK)
    {
        goto failed;
    }

    pThirdPartyP2PInternal = dynamicCast(pResourceRef->pResource, ThirdPartyP2P);

    pVidmemInfo = _createOrReuseVidmemInfoPersistent(pGpu, address, length, &offset,
                                                     hClientInternal,
                                                     pThirdPartyP2P,
                                                     pThirdPartyP2PInternal);
    if (pVidmemInfo == NULL)
    {
        status = NV_ERR_INVALID_STATE;

        goto failed;
    }

    status = RmP2PGetPagesUsingVidmemInfo(address, length, offset,
                                          pThirdPartyP2PInternal,
                                          &pPhysicalAddresses, NULL, NULL,
                                          pEntries, pPlatformData, NULL, NULL,
                                          pGpu, pThirdPartyP2PInternal->pSubdevice,
                                          NULL, pThirdPartyP2PInternal, pVidmemInfo);
    if (status != NV_OK)
    {
        // Cleanup MappingInfo if it was allocated
        thirdpartyp2pDelMappingInfoByKey(pThirdPartyP2PInternal, pPlatformData, NV_FALSE);

        //
        // The cleanup with thirdpartyp2pDelMappingInfoByKey() above is not enough
        // since creating MappingInfo with pPlatformData could have failed.
        // Cleanup of the internal VidmemInfo is still needed since pPlatformData
        // lookup would fail and the VidmemInfo is not available for cleanup via
        // thirdpartyp2pDelPersistentMappingInfoByKey().
        //
        CliDelThirdPartyP2PVidmemInfoPersistent(pThirdPartyP2PInternal, pVidmemInfo);

        goto failed;
    }

    //
    // Update p2pObject as the internal ThirdPartyP2P object
    // which will be used by nvidia_p2p_put_pages() to look up mappings.
    //
    *p2pObject = (void *) pThirdPartyP2PInternal;

    return NV_OK;

failed:
    RmP2PPutMigInfo(pKernelMIGGpuInstance);

    return status;
}

NV_STATUS RmP2PGetPages(
    NvU64       p2pToken,
    NvU32       vaSpaceToken,
    NvU64       address,
    NvU64       length,
    NvU64      *pPhysicalAddresses,
    NvU32      *pWreqMbH,
    NvU32      *pRreqMbH,
    NvU32      *pEntries,
    OBJGPU    **ppGpu,
    void       *pPlatformData,
    void      (*pFreeCallback)(void *pData),
    void       *pData
)
{
    if (pFreeCallback == NULL || pData == NULL)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "invalid argument(s) in RmP2PGetPages, pFreeCallback=%p pData=%p\n",
                  pFreeCallback, pData);
        return NV_ERR_INVALID_ARGUMENT;
    }

    return _rmP2PGetPages(p2pToken, vaSpaceToken, address, length,
                          pPhysicalAddresses, pWreqMbH, pRreqMbH,
                          pEntries, ppGpu, pPlatformData,
                          pFreeCallback, pData);
}

NV_STATUS RmP2PGetPagesWithoutCallbackRegistration(
    NvU64       p2pToken,
    NvU32       vaSpaceToken,
    NvU64       address,
    NvU64       length,
    NvU64      *pPhysicalAddresses,
    NvU32      *pWreqMbH,
    NvU32      *pRreqMbH,
    NvU32      *pEntries,
    OBJGPU    **ppGpu,
    void       *pPlatformData
)
{
    return _rmP2PGetPages(p2pToken, vaSpaceToken, address, length,
                          pPhysicalAddresses, pWreqMbH, pRreqMbH,
                          pEntries, ppGpu, pPlatformData,
                          NULL, NULL);
}

NV_STATUS RmP2PGetGpuByAddress(
    NvU64      address,
    NvU64      length,
    OBJGPU   **ppGpu
)
{
    ThirdPartyP2P *pThirdPartyP2P = NULL;
    CLI_THIRD_PARTY_P2P_VASPACE_INFO *pVASpaceInfo = NULL;
    OBJGPU *pGpu = NULL;
    NV_STATUS status = NV_OK;

    status = RmP2PGetInfoWithoutToken(address, length, NULL,
                                      &pThirdPartyP2P, &pVASpaceInfo, NULL);
    if (status != NV_OK)
    {
        return status;
    }

    status = RmP2PValidateSubDevice(pThirdPartyP2P, &pGpu);
    if (status != NV_OK)
    {
        return status;
    }

    *ppGpu = pGpu;

    return status;
}

NV_STATUS RmP2PRegisterCallback(
    NvU64       p2pToken,
    NvU64       address,
    NvU64       length,
    void       *pPlatformData,
    void      (*pFreeCallback)(void *pData),
    void       *pData
)
{
    NV_STATUS status;
    ThirdPartyP2P *pThirdPartyP2P;
    PCLI_THIRD_PARTY_P2P_VASPACE_INFO pVASpaceInfo = NULL;
    PCLI_THIRD_PARTY_P2P_VIDMEM_INFO pVidmemInfo;
    NvU64 offset;

    if (0 != p2pToken)
    {
        status = CliGetThirdPartyP2PInfoFromToken(p2pToken,
                                                  &pThirdPartyP2P);
    }
    else
    {
        status = RmP2PGetInfoWithoutToken(address,
                                          0,
                                          pPlatformData,
                                          &pThirdPartyP2P,
                                          &pVASpaceInfo,
                                          NULL);
    }
    if (status != NV_OK)
    {
        return status;
    }

    status = CliGetThirdPartyP2PVidmemInfoFromAddress(pThirdPartyP2P, address,
                                                      length, &offset, &pVidmemInfo);
    if (status != NV_OK)
    {
        return status;
    }

    return CliRegisterThirdPartyP2PMappingCallback(pThirdPartyP2P,
                                                   pVidmemInfo->hMemory,
                                                   pPlatformData, pFreeCallback,
                                                   pData);
}

NV_STATUS RmP2PPutPagesPersistent(
    void       *p2pObject,
    void       *pPlatformData,
    void       *pMigInfo
)
{
    NV_STATUS status;
    ThirdPartyP2P *pThirdPartyP2P = NULL;

    pThirdPartyP2P = (ThirdPartyP2P *)(p2pObject);

    if ((pThirdPartyP2P->type == CLI_THIRD_PARTY_P2P_TYPE_PROPRIETARY) &&
        !(pThirdPartyP2P->flags & CLI_THIRD_PARTY_P2P_FLAGS_INITIALIZED))
    {
        return NV_ERR_INVALID_STATE;
    }

    status = thirdpartyp2pDelPersistentMappingInfoByKey(pThirdPartyP2P, pPlatformData, NV_TRUE);

    NV_ASSERT(status == NV_OK);

    RmP2PPutMigInfo(pMigInfo);

    return status;
}

NV_STATUS RmP2PPutPages(
    NvU64       p2pToken,
    NvU32       vaSpaceToken,
    NvU64       address,
    void       *pPlatformData
)
{
    NV_STATUS status;
    ThirdPartyP2P *pThirdPartyP2P;

    if (0 != p2pToken)
    {
        status = CliGetThirdPartyP2PInfoFromToken(p2pToken,
                                                  &pThirdPartyP2P);
    }
    else
    {
        status = RmP2PGetInfoWithoutToken(address,
                                          0,
                                          pPlatformData,
                                          &pThirdPartyP2P,
                                          NULL, NULL);
    }
    if (status != NV_OK)
    {
        return status;
    }

    if ((pThirdPartyP2P->type == CLI_THIRD_PARTY_P2P_TYPE_PROPRIETARY) &&
        !(pThirdPartyP2P->flags & CLI_THIRD_PARTY_P2P_FLAGS_INITIALIZED))
    {
        return NV_ERR_INVALID_STATE;
    }

    status = thirdpartyp2pDelMappingInfoByKey(pThirdPartyP2P, pPlatformData, NV_TRUE);
    NV_ASSERT(status == NV_OK);

    return status;
}
