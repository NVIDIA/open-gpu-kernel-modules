/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/gpu.h"
#include "os/os.h"
#include "gpu/mem_sys/kern_mem_sys.h"
#include "gpu/bus/kern_bus.h"
#include "gpu/bus/p2p_api.h"
#include "gpu/nvlink/kernel_nvlink.h"
#include "published/volta/gv100/dev_mmu.h"

/*!
 * @brief Get physical address of the FB memory on systems where GPU memory
 * is onlined to the OS
 *
 * @param[in] pGpu                OBJGPU pointer
 * @param[in] pKernelMemorySystem pointer to the kernel side KernelMemorySystem instance.
 * @param[in] physAddr            Physical Address of FB memory
 * @param[in] numaNodeId          NUMA node id where FB memory is added to the
 *                                kernel
 *
 * @return  NV_OK on success
 */
NV_STATUS
kmemsysGetFbNumaInfo_GV100
(
    OBJGPU             *pGpu,
    KernelMemorySystem *pKernelMemorySystem,
    NvU64              *physAddr,
    NvU64              *rsvdPhysAddr,
    NvS32              *numaNodeId
)
{
    NV_STATUS     status;

    status = osGetFbNumaInfo(pGpu, physAddr, rsvdPhysAddr, numaNodeId);
    if (status == NV_OK)
    {
        NV_PRINTF(LEVEL_INFO, "NUMA FB Physical address: 0x%llx Node ID: 0x%x\n",
                  *physAddr, *numaNodeId);
    }

    return status;
}

/*!
 * @brief Determine whether RM needs to invalidate GPU L2 cache during map call
 *
 * @param[in] pGpu                OBJGPU pointer
 * @param[in] pKernelMemorySystem pointer to the kernel side KernelMemorySystem instance.
 * @param[in] bIsVolatile         Whether the map call is to create vol mapping
 * @param[in] aperture            Aperture of the memory being mapped
 *
 * @return  NV_OK on success
 */
NvBool
kmemsysNeedInvalidateGpuCacheOnMap_GV100
(
    OBJGPU              *pGpu,
    KernelMemorySystem  *pKernelMemorySystem,
    NvBool               bIsVolatile,
    NvU32                aperture
)
{
    //
    // Only need to invalidate L2 for cached (vol=0) mapping to sys/peer memory
    // because GPU's L2 is not coherent with CPU updates to sysmem
    // See bug 3342220 for more info
    //
    return (!bIsVolatile && (aperture == NV_MMU_PTE_APERTURE_PEER_MEMORY ||
                             aperture == NV_MMU_PTE_APERTURE_SYSTEM_COHERENT_MEMORY ||
                             aperture == NV_MMU_PTE_APERTURE_SYSTEM_NON_COHERENT_MEMORY));
}

/*!
 * @brief Configure local GPU's peer ATS config using peer GPU's local
 * ATS config.
 *
 * @param[in] pLocalGpu                Local GPU OBJGPU pointer
 * @param[in] pLocalKernelMemorySystem Local GPU KernelMemorySystem pointer
 * @param[in] pRemoteGpu               Remote GPU OBJGPU pointer
 * @param[in] peerId                   peer id from local GPU to remote GPU in
 *                                     local GPU
 *
 * @return  NV_OK on success
 */
static
NV_STATUS
_kmemsysConfigureAtsPeers
(
    OBJGPU             *pLocalGpu,
    KernelMemorySystem *pLocalKernelMemorySystem,
    OBJGPU             *pRemoteGpu,
    NvU32               peerId
)
{
    RM_API *pLocalRmApi = GPU_GET_PHYSICAL_RMAPI(pLocalGpu);
    RM_API *pRemoteRmApi = GPU_GET_PHYSICAL_RMAPI(pRemoteGpu);
    NV2080_CTRL_INTERNAL_MEMSYS_GET_LOCAL_ATS_CONFIG_PARAMS getParams = { 0 };
    NV2080_CTRL_INTERNAL_MEMSYS_SET_PEER_ATS_CONFIG_PARAMS setParams = { 0 };

    NV_ASSERT_OK_OR_RETURN(pRemoteRmApi->Control(pRemoteRmApi,
                                           pRemoteGpu->hInternalClient,
                                           pRemoteGpu->hInternalSubdevice,
                                           NV2080_CTRL_CMD_INTERNAL_MEMSYS_GET_LOCAL_ATS_CONFIG,
                                           &getParams,
                                           sizeof(NV2080_CTRL_INTERNAL_MEMSYS_GET_LOCAL_ATS_CONFIG_PARAMS)));
    setParams.peerId = peerId;
    setParams.addrSysPhys = getParams.addrSysPhys;
    setParams.addrWidth = getParams.addrWidth;
    setParams.mask = getParams.mask;
    setParams.maskWidth = getParams.maskWidth;

    NV_ASSERT_OK_OR_RETURN(pLocalRmApi->Control(pLocalRmApi,
                                           pLocalGpu->hInternalClient,
                                           pLocalGpu->hInternalSubdevice,
                                           NV2080_CTRL_CMD_INTERNAL_MEMSYS_SET_PEER_ATS_CONFIG,
                                           &setParams,
                                           sizeof(NV2080_CTRL_INTERNAL_MEMSYS_SET_PEER_ATS_CONFIG_PARAMS)));

    return NV_OK;
}

/*!
 * @brief Remove local GPU's peer ATS config
 *
 * @param[in] pLocalGpu                Local GPU OBJGPU pointer
 * @param[in] pLocalKernelMemorySystem Local GPU KernelMemorySystem pointer
 * @param[in] peerId                   peer id from local GPU to remote GPU in
 *                                     local GPU
 *
 * @return  NV_OK on success
 */
static
NV_STATUS
_kmemsysResetAtsPeerConfiguration
(
    OBJGPU             *pLocalGpu,
    KernelMemorySystem *pLocalKernelMemorySystem,
    NvU32               peerId
)
{
    RM_API *pLocalRmApi = GPU_GET_PHYSICAL_RMAPI(pLocalGpu);
    NV2080_CTRL_INTERNAL_MEMSYS_GET_LOCAL_ATS_CONFIG_PARAMS getParams = { 0 };
    NV2080_CTRL_INTERNAL_MEMSYS_SET_PEER_ATS_CONFIG_PARAMS setParams = { 0 };

    NV_ASSERT_OK_OR_RETURN(pLocalRmApi->Control(pLocalRmApi,
                                           pLocalGpu->hInternalClient,
                                           pLocalGpu->hInternalSubdevice,
                                           NV2080_CTRL_CMD_INTERNAL_MEMSYS_GET_LOCAL_ATS_CONFIG,
                                           &getParams,
                                           sizeof(NV2080_CTRL_INTERNAL_MEMSYS_GET_LOCAL_ATS_CONFIG_PARAMS)));

    setParams.peerId = peerId;
    setParams.addrSysPhys = 0;
    setParams.addrWidth = getParams.addrWidth;
    setParams.mask = 0;
    setParams.maskWidth = getParams.maskWidth;

    NV_ASSERT_OK_OR_RETURN(pLocalRmApi->Control(pLocalRmApi,
                                           pLocalGpu->hInternalClient,
                                           pLocalGpu->hInternalSubdevice,
                                           NV2080_CTRL_CMD_INTERNAL_MEMSYS_SET_PEER_ATS_CONFIG,
                                           &setParams,
                                           sizeof(NV2080_CTRL_INTERNAL_MEMSYS_SET_PEER_ATS_CONFIG_PARAMS)));

    return NV_OK;
}

/**
 * @brief Setup one pair of ATS peers (non-chiplib configs)
 *
 * @param[in] pGpu                OBJGPU pointer
 * @param[in] pKernelMemorySystem Kernel Memory System pointer
 * @param[in] pRemoteGpu          OBJGPU pointer for the ATS peer
 *
 * @return  NV_OK on success
 */
static
NV_STATUS
_kmemsysSetupAtsPeers
(
    OBJGPU             *pGpu,
    KernelMemorySystem *pKernelMemorySystem,
    OBJGPU             *pRemoteGpu
)
{
    NvU32         peer1         = BUS_INVALID_PEER;
    NvU32         peer2         = BUS_INVALID_PEER;
    NV_STATUS     status        = NV_OK;
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);
    KernelMemorySystem *pLocalKernelMs      = NULL;
    KernelMemorySystem *pRemoteKernelMs     = NULL;
    NvU32         attributes    = DRF_DEF(_P2PAPI, _ATTRIBUTES, _CONNECTION_TYPE, _NVLINK) |
                                  DRF_DEF(_P2PAPI, _ATTRIBUTES, _LINK_TYPE, _SPA);

    // Set up P2P ATS configuration
    pLocalKernelMs      = pKernelMemorySystem;
    pRemoteKernelMs     = GPU_GET_KERNEL_MEMORY_SYSTEM(pRemoteGpu);

    if (pKernelNvlink != NULL)
    {
        // Trigger P2P link training to HS before releasing credits on P9
        knvlinkTrainP2pLinksToActive(pGpu, pRemoteGpu, pKernelNvlink);
    }

    status = kbusCreateP2PMapping_HAL(pGpu, GPU_GET_KERNEL_BUS(pGpu), pRemoteGpu, GPU_GET_KERNEL_BUS(pRemoteGpu),
                                     &peer1, &peer2, attributes);
    if (status != NV_OK)
    {
        return status;
    }

    if (pLocalKernelMs && pRemoteKernelMs &&
        pGpu->getProperty(pGpu, PDB_PROP_GPU_ATS_SUPPORTED) &&
        pRemoteGpu->getProperty(pRemoteGpu, PDB_PROP_GPU_ATS_SUPPORTED))
    {
        status = _kmemsysConfigureAtsPeers(pGpu, pLocalKernelMs, pRemoteGpu, peer1);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Configuring ATS p2p config between GPU%u and GPU%u "
                      "failed with status %x\n", pGpu->gpuInstance,
                      pRemoteGpu->gpuInstance, status);
            return status;
        }

        status = _kmemsysConfigureAtsPeers(pRemoteGpu, pRemoteKernelMs, pGpu, peer2);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Configuring ATS p2p config between GPU%u and GPU%u "
                      "failed with status %x\n", pRemoteGpu->gpuInstance,
                      pGpu->gpuInstance, status);
            return status;
        }
    }

    return NV_OK;
}

/**
 * @brief Remove one pair of ATS peers (non-chiplib configs)
 *
 * @param[in] pGpu                OBJGPU pointer
 * @param[in] pKernelMemorySystem Kernel Memory System pointer
 * @param[in] pRemoteGpu          OBJGPU pointer for the ATS peer
 *
 * @return  NV_OK on success
 */
static
NV_STATUS
_kmemsysRemoveAtsPeers
(
    OBJGPU             *pGpu,
    KernelMemorySystem *pKernelMemorySystem,
    OBJGPU             *pRemoteGpu
)
{
    NvU32         peer1         = BUS_INVALID_PEER;
    NvU32         peer2         = BUS_INVALID_PEER;
    NV_STATUS     status        = NV_OK;
    KernelMemorySystem *pLocalKernelMs      = NULL;
    KernelMemorySystem *pRemoteKernelMs     = NULL;
    NvU32         attributes    = DRF_DEF(_P2PAPI, _ATTRIBUTES, _CONNECTION_TYPE, _NVLINK) |
                                  DRF_DEF(_P2PAPI, _ATTRIBUTES, _LINK_TYPE, _SPA);

    pLocalKernelMs      = pKernelMemorySystem;
    pRemoteKernelMs     = GPU_GET_KERNEL_MEMORY_SYSTEM(pRemoteGpu);

    peer1 = kbusGetPeerId_HAL(pGpu, GPU_GET_KERNEL_BUS(pGpu), pRemoteGpu);
    peer2 = kbusGetPeerId_HAL(pRemoteGpu, GPU_GET_KERNEL_BUS(pRemoteGpu), pGpu);

    status = kbusRemoveP2PMapping_HAL(pGpu, GPU_GET_KERNEL_BUS(pGpu), pRemoteGpu, GPU_GET_KERNEL_BUS(pRemoteGpu),
                                      peer1, peer2, attributes);
    if (status != NV_OK)
    {
        return status;
    }

    if (pLocalKernelMs && pRemoteKernelMs &&
        pGpu->getProperty(pGpu, PDB_PROP_GPU_ATS_SUPPORTED) &&
        pRemoteGpu->getProperty(pRemoteGpu, PDB_PROP_GPU_ATS_SUPPORTED))
    {
        status = _kmemsysResetAtsPeerConfiguration(pGpu, pLocalKernelMs, peer1);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Removing ATS p2p config between GPU%u and GPU%u "
                      "failed with status %x\n", pGpu->gpuInstance,
                      pRemoteGpu->gpuInstance, status);
        }

        status = _kmemsysResetAtsPeerConfiguration(pRemoteGpu, pRemoteKernelMs, peer2);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Removinging ATS p2p config between GPU%u and GPU%u "
                      "failed with status %x\n", pRemoteGpu->gpuInstance,
                      pGpu->gpuInstance, status);
        }
    }

    return NV_OK;
}

/**
 * @brief Setup ATS peer access. On GV100 and GH180, ATS peers use NVLINK.
 *
 * @param[in] pGpu                 OBJGPU pointer
 * @param[in] pKernelMemorySystem  Kernel Memory System pointer
 *
 * @return  NV_OK on success
 */
NV_STATUS
kmemsysSetupAllAtsPeers_GV100
(
    OBJGPU *pGpu,
    KernelMemorySystem *pKernelMemorySystem
)
{
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);
    NvU32 gpuAttachCnt, gpuAttachMask, gpuInstance = 0;

    NV_STATUS status     = NV_OK;
    OBJGPU   *pRemoteGpu = NULL;

    NV_CHECK_OR_RETURN(LEVEL_WARNING, pKernelNvlink != NULL, status);

    // loop over all possible GPU pairs and setup the ATS config
    gpumgrGetGpuAttachInfo(&gpuAttachCnt, &gpuAttachMask);
    while ((pRemoteGpu = gpumgrGetNextGpu(gpuAttachMask, &gpuInstance)) != NULL)
    {
        if (pRemoteGpu == pGpu)
            continue;

        if (gpuIsGpuFullPower(pRemoteGpu) == NV_FALSE)
            continue;

        if (!knvlinkIsNvlinkP2pSupported(pGpu, pKernelNvlink, pRemoteGpu))
            continue;

        status = _kmemsysSetupAtsPeers(pGpu, pKernelMemorySystem, pRemoteGpu);
        if (status != NV_OK)
            return status;
    }

    return NV_OK;
}

/**
 * @brief Remove ATS peer access. On GV100 and GH180, ATS peers use NVLINK.
 *
 * @param[in] pGpu                 OBJGPU pointer
 * @param[in] pKernelMemorySystem  Kernel Memory System pointer
 */
void
kmemsysRemoveAllAtsPeers_GV100
(
    OBJGPU *pGpu,
    KernelMemorySystem *pKernelMemorySystem
)
{
    NvU32 gpuAttachCnt, gpuAttachMask, gpuInstance = 0;

    NV_STATUS status     = NV_OK;
    OBJGPU   *pRemoteGpu = NULL;

    // loop over all possible GPU pairs and remove the ATS config
    gpumgrGetGpuAttachInfo(&gpuAttachCnt, &gpuAttachMask);
    while ((pRemoteGpu = gpumgrGetNextGpu(gpuAttachMask, &gpuInstance)) != NULL)
    {
        if (pRemoteGpu == pGpu)
            continue;

        if (gpuIsGpuFullPower(pRemoteGpu) == NV_FALSE)
            continue;

        status = _kmemsysRemoveAtsPeers(pGpu, pKernelMemorySystem, pRemoteGpu);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Failed to remove ATS peer access between GPU%d and GPU%d\n",
                      pGpu->gpuInstance, pRemoteGpu->gpuInstance);
        }
    }
}

