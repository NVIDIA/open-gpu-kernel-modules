/*
 * SPDX-FileCopyrightText: Copyright (c) 2022-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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


#include "gpu/gpu.h"
#include "gpu/ce/kernel_ce.h"
#include "gpu/ce/kernel_ce_private.h"
#include "gpu/bif/kernel_bif.h"
#include "published/blackwell/gb100/dev_ce_base.h"
#include "gpu/nvlink/kernel_nvlink.h"

// Defines for PCE-LCE mapping algorithm
#define NV_CE_LCE_MASK_INIT                   0xFFFFFFFF
#define NV_CE_EVEN_ASYNC_LCE_MASK             0x55555550
#define NV_CE_ODD_ASYNC_LCE_MASK              0xAAAAAAA0
#define NV_CE_INVALID_TOPO_IDX                0xFFFF
#define NV_CE_SYSMEM_LCE_START_INDEX          0x2
#define NV_CE_SYSMEM_LCE_END_INDEX            0x4
#define NV_CE_LCE_4                           0x4

/*!
 * @brief Returns the size of the PCE2LCE register array
 *
 *
 * @param[in] pGpu  OBJGPU pointer
 * @param[in] pCe   OBJCE pointer
 *
 * @return  NV_CE_PCE2LCE_CONFIG__SIZE_1
 *
 */
NvU32
kceGetPce2lceConfigSize1_GB100
(
    KernelCE *pKCe
)
{
    return NV_CE_PCE2LCE_CONFIG__SIZE_1;
}

/*!
 * @brief Returns the size of the GRCE_CONFIG register array
 *
 * @param[in] pGpu  OBJGPU pointer
 * @param[in] pKCe   KernelCE pointer
 *
 * @return  NV_CE_GRCE_CONFIG__SIZE_1
 */
NvU32
kceGetGrceConfigSize1_GB100
(
    KernelCE *pKCe
)
{
    return (pKCe->shimInstance == 0) ? NV_CE_GRCE_CONFIG__SIZE_1 : 0;
}

/*!
 * @brief Set the shim instance based on the public ID.
 */
void
kceSetShimInstance_GB100
(
    OBJGPU *pGpu,
    KernelCE *pKCe
)
{
    pKCe->shimInstance = pKCe->publicID / NV_KCE_GROUP_ID_STRIDE;
}

/**
 * @brief This function checks for whether input nv2080 engine type
 *         supports decompression workloads or nort
 *
 * @param[in]   pGpu            OBJGPU pointer
 * @param[in]   pKCe            KernelCE pointer
 * @param[in]   nv2080EngineId  NV2080_ENGINE_TYPE_*
 */
NvBool
kceCheckForDecompCapability_GB100
(
    OBJGPU      *pGpu,
    KernelCE    *pKCe,
    NvU32       nv2080EngineId
)
{
    NvBool decompCapPresent = NV_FALSE;
    NV_STATUS status;
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV2080_CTRL_CE_GET_DECOMP_LCE_MASK_PARAMS params = {0};

    NV2080_CTRL_CE_GET_LCE_SHIM_INFO_PARAMS shimParams = {0};
    shimParams.ceEngineType = nv2080EngineId;
    status = pRmApi->Control(pRmApi,
                    pGpu->hInternalClient,
                    pGpu->hInternalSubdevice,
                    NV2080_CTRL_CMD_CE_GET_LCE_SHIM_INFO,
                    &shimParams,
                    sizeof(shimParams));
    if (status != NV_OK)
        goto done;

    params.shimInstance = shimParams.shimInstance;

    status = pRmApi->Control(pRmApi,
                    pGpu->hInternalClient,
                    pGpu->hInternalSubdevice,
                    NV2080_CTRL_CMD_CE_GET_DECOMP_LCE_MASK,
                    &params,
                    sizeof(params));
    if (status != NV_OK)
        goto done;

    if (params.decompLceMask != 0)
    {
        //
        // shimParams.shimLocalLceIdx will range from LCE 0-9
        // and so would params.decompLceMask since it returns mask for given shim instance
        //
        decompCapPresent = (NvBool)(NVBIT64(shimParams.shimLocalLceIdx) & params.decompLceMask);
    }

done:
    return decompCapPresent;
}

/**
 *  Return the pce-lce mappings and grce config
 *  reg values when nvlink topology is NOT forced
 *
 * @param[in]  pGpu                 OBJGPU pointer
 * @param[in]  pKCe                 KernelCE pointer
 * @param[out] pPceLceMap           Stores the pce-lce mappings
 * @param[out] pGrceConfig          Stores the grce configuration
 * @param[out] pExposedLceMask    Mask of CEs to expose to clients
 *
 * @return  NV_OK on success
 */
NV_STATUS
kceGetNvlinkAutoConfigCeValues_GB100
(
    OBJGPU   *pGpu,
    KernelCE *pKCe,
    NvU32    *pPceLceMap,
    NvU32    *pGrceConfig,
    NvU32    *pExposedLceMask
)
{
    KernelNvlink *pKernelNvlink      = GPU_GET_KERNEL_NVLINK(pGpu);
    NV_STATUS     status             = NV_OK;
    NvU32         pce2lceConfigSize1 = kceGetPce2lceConfigSize1_HAL(pKCe);
    NvU32         grceConfigSize1    = kceGetGrceConfigSize1_HAL(pKCe);
    NvU32        *pLocalPceLceMap    = NULL;
    NvU32        *pLocalGrceConfig   = NULL;
    NvU32         pceIdx;
    NvU32         grceIdx;
    NvU32         i;

    NV_ASSERT_OR_RETURN(gpuGetNumCEs(pGpu) != 0, NV_ERR_NOT_SUPPORTED);

    NV_ASSERT_OR_RETURN(pPceLceMap      != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pGrceConfig     != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pExposedLceMask != NULL, NV_ERR_INVALID_ARGUMENT);

    NV_ASSERT_OR_RETURN(!(pKernelNvlink == NULL && pGpu->getProperty(pGpu, PDB_PROP_GPU_SKIP_CE_MAPPINGS_NO_NVLINK)),
                        NV_ERR_NOT_SUPPORTED);

    NVLINK_TOPOLOGY_PARAMS *pCurrentTopo = portMemAllocNonPaged(sizeof(*pCurrentTopo));
    NV_ASSERT_OR_RETURN(pCurrentTopo != NULL, NV_ERR_NO_MEMORY);

    portMemSet(pCurrentTopo, 0, sizeof(*pCurrentTopo));

    pLocalPceLceMap          = portMemAllocNonPaged(sizeof(NvU32[NV2080_CTRL_MAX_PCES]));
    pLocalGrceConfig         = portMemAllocNonPaged(sizeof(NvU32[NV2080_CTRL_MAX_GRCES]));
    NvU32 localExposedCeMask = 0;

    NV_ASSERT_TRUE_OR_GOTO(status, pLocalPceLceMap  != NULL, NV_ERR_NO_MEMORY, done);
    NV_ASSERT_TRUE_OR_GOTO(status, pLocalGrceConfig != NULL, NV_ERR_NO_MEMORY, done);

    for (i = 0; i < NV2080_CTRL_MAX_PCES; i++)
    {
        pLocalPceLceMap[i] = NV2080_CTRL_CE_UPDATE_PCE_LCE_MAPPINGS_INVALID_LCE;
    }

    for (i = 0; i < NV2080_CTRL_MAX_GRCES; i++)
    {
        pLocalGrceConfig[i] = NV2080_CTRL_CE_UPDATE_PCE_LCE_MAPPINGS_INVALID_LCE;
    }

    status = kceGetMappings_HAL(pGpu,
                                pKCe,
                                pCurrentTopo,
                                pLocalPceLceMap,
                                pLocalGrceConfig,
                                &localExposedCeMask);

    *pExposedLceMask = localExposedCeMask;

    for (pceIdx = 0; pceIdx < pce2lceConfigSize1; pceIdx++)
    {
        pPceLceMap[pceIdx] = pLocalPceLceMap[pceIdx];
    }

    for (grceIdx = 0; grceIdx < grceConfigSize1; grceIdx++)
    {
        pGrceConfig[grceIdx] = pLocalGrceConfig[grceIdx];
    }

done:
    portMemFree(pCurrentTopo);
    portMemFree(pLocalPceLceMap);
    portMemFree(pLocalGrceConfig);

    return status;
}

/**
 * @brief This function takes care of mappings for Decomp PCEs.
 *        Mapping will assign first 6 Decomp PCEs found on each
 *        shim to the lowest available odd numbered LCE if possible.
 *
 * @param[in]   pGpu                                OBJGPU pointer
 * @param[in]   pKCe                                KernelCE pointer
 * @param[in]   pAvailablePceMaskForConnectingHub   Pointer to PCEs available per HSHUB
 * @param[out]  pExposedLceMask                     Pointer to LCE Mask
 */
void
kceMapPceLceForDecomp_GB100
(
    OBJGPU      *pGpu,
    KernelCE    *pKCe,
    NvU32       *pAvailablePceMaskForConnectingHub,
    NvU32       *pExposedLceMask
)
{
    NvU32       maxLceCount;
    NvU32       numDecompPcesAssigned   = 0;
    KernelCE   *pKCeIter                = NULL;
    NvU32       localDecompPceMask;
    NvU32       hshubIndex;
    NvU32       lceMask;
    NvU32       lceIndex;
    NvU32       pceIndex;
    NvU32       numPcesPerLce;
    NvU32       numLces;
    NvU32       supportedPceMask;
    NvU32       supportedLceMask;
    NvU32       pcesPerHshub;

    kceGetPceConfigForLceType(
        pGpu,
        pKCe,
        NV2080_CTRL_CE_LCE_TYPE_DECOMP,
        &numPcesPerLce,
        &numLces,
        &supportedPceMask,
        &supportedLceMask,
        &pcesPerHshub);

    maxLceCount = supportedLceMask;
    HIGHESTBITIDX_32(maxLceCount); // HIGHESTBITIDX_32 is destructive.
    maxLceCount++;

    KCE_ITER_SHIM_BEGIN(pGpu, pKCeIter)
    {
        lceMask = kceGetLceMaskForShimInstance_HAL(pGpu, pKCeIter) &
                  supportedLceMask &
                 ~(*pExposedLceMask);

        if ((lceMask & NV_CE_ODD_ASYNC_LCE_MASK) != 0)
        {
            lceMask &= NV_CE_ODD_ASYNC_LCE_MASK;
        }

        numDecompPcesAssigned   = 0;
        lceIndex                = CE_GET_LOWEST_AVAILABLE_IDX(lceMask);

        if (lceIndex >= maxLceCount)
        {
            break;
        }

        FOR_EACH_INDEX_IN_MASK(32, hshubIndex, pKCeIter->shimConnectingHubMask)
        {
            if (numDecompPcesAssigned >= numPcesPerLce)
            {
                break;
            }

            localDecompPceMask = pAvailablePceMaskForConnectingHub[hshubIndex] & supportedPceMask;

            if (localDecompPceMask == 0)
            {
                continue;
            }

            FOR_EACH_INDEX_IN_MASK(32, pceIndex, localDecompPceMask)
            {
                if (pceIndex < kceGetPce2lceConfigSize1_HAL(pKCeIter))
                {
                    //
                    // Convert absolute LCE index to shim local index
                    //
                    pKCeIter->pPceLceMap[pceIndex]                  = lceIndex % NV_KCE_GROUP_ID_STRIDE;
                    *pExposedLceMask                               |= NVBIT(lceIndex);
                    pAvailablePceMaskForConnectingHub[hshubIndex]  &= (~NVBIT(pceIndex));
                    numDecompPcesAssigned++;

                    NV_PRINTF(
                        LEVEL_INFO,
                        "Decomp CE Mapping -- PCE Index: %d -> LCE Index: %d\n",
                        pceIndex, lceIndex);
                }
            }
            FOR_EACH_INDEX_IN_MASK_END;
        }
        FOR_EACH_INDEX_IN_MASK_END;
    }
    KCE_ITER_END;
}

/**
 * @brief This function returns the pceIndex for a particular link ID
 *        Must always be called with the hshub ID for the calling link ID.
 *        Assign PCE from selected HSHUB, if not possible move to the
 *        additional HSHUB provided.
 *
 * @param[in]       pGpu                                OBJGPU pointer
 * @param[in]       pKCe                                KernelCE pointer
 * @param[in]       pAvailablePceMaskForConnectingHub   Pointer to CEs available per HSHUB
 * @param[in,out]   pceIndex                            Pointer to caller pceIndex
 * @param[out]      pHshubId                            Pointer to caller HSHUB ID
 * 
 * @return NV_TRUE if there is a free PCE available for assignment. Else returns NV_FALSE.
 */
static NvBool
_ceGetAlgorithmPceIndex
(
    OBJGPU     *pGpu,
    KernelCE   *pKCe,
    NvU32      *pAvailablePceMaskForConnectingHub,
    NvU32      *pceIndex,
    NvU32      *pHshubId
)
{
    //
    // The requested PCE is available. So, just use it.
    //
    if (NVBIT32(*pceIndex) & pAvailablePceMaskForConnectingHub[*pHshubId] & ~(pKCe->decompPceMask))
    {
        return NV_TRUE;
    }

    //
    // Try to use the unassigned PCE from last HSHUB.
    //
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV2080_CTRL_INTERNAL_HSHUB_GET_MAX_HSHUBS_PER_SHIM_PARAMS maxHshubParams;
    portMemSet(&maxHshubParams, 0, sizeof(maxHshubParams));

    NV_ASSERT_OK(pRmApi->Control(pRmApi,
                                 pGpu->hInternalClient,
                                 pGpu->hInternalSubdevice,
                                 NV2080_CTRL_CMD_INTERNAL_HSHUB_GET_MAX_HSHUBS_PER_SHIM,
                                 &maxHshubParams,
                                 sizeof(maxHshubParams)));

    *pHshubId = maxHshubParams.maxHshubs - 1;
    *pceIndex = CE_GET_LOWEST_AVAILABLE_IDX(pAvailablePceMaskForConnectingHub[*pHshubId]);

    if (*pceIndex <= kceGetPce2lceConfigSize1_HAL(pKCe))
    {
        return NV_TRUE;
    }

    // If we've reached this point, then we have no more available PCEs to assign
    NV_PRINTF(LEVEL_ERROR, "No more available PCEs to assign!\n");
    return NV_FALSE;
}

/**
 * @brief This function assigns PCE-LCE mappings for NVLink peers
 *        Based on HSHUBs that the links associated with a peer connect to,
 *        algorithm will attempt to assign a PCE from associated HSHUB before
 *        moving to additional HSHUB.
 *
 * @param[in]   pGpu                                OBJGPU pointer
 * @param[in]   pKCe                                KernelCE pointer
 * @param[in]   pAvailablePceMaskForConnectingHub   Pointer to CEs available per HSHUB
 * @param[out]  pLocalPceLceMap                     Pointer to PCE-LCE array
 * @param[out]  pExposedLceMask                     Pointer to LCE Mask
 *
 * Returns NV_OK if successful in assigning PCEs and LCEs for each of the NVLink peers
 */
NV_STATUS
kceMapPceLceForNvlinkPeers_GB100
(
    OBJGPU      *pGpu,
    KernelCE    *pKCe,
    NvU32       *pAvailablePceMaskForConnectingHub,
    NvU32       *pLocalPceLceMap,
    NvU32       *pExposedLceMask
)
{
    KernelNvlink   *pKernelNvlink         = GPU_GET_KERNEL_NVLINK(pGpu);
    NV_STATUS       status                = NV_OK;
    NvU32           pceMask               = 0;
    NvU32           peerLinkMask          = 0;
    NvU32           gpuInstance           = 0;
    NvU32           pceIndex              = kceGetPce2lceConfigSize1_HAL(pKCe);
    NvBool          bPeerAssigned         = NV_FALSE;
    NvU32           peerAvailableLceMask  = NV_CE_LCE_MASK_INIT;
    OBJGPU         *pRemoteGpu;
    NvU32           lceIndex;
    NvU32           linkId;
    NvU32           gpuMask;
    NvU32           hshubIndex;
    NvU32           maxLceCount;
    NvU32           numPcesPerLce;
    NvU32           numLces;
    NvU32           supportedPceMask;
    NvU32           supportedLceMask;
    NvU32           pcesPerHshub;
    NvBool          bIsPceAvailable;

    kceGetPceConfigForLceType(pGpu,
                              pKCe,
                              NV2080_CTRL_CE_LCE_TYPE_NVLINK_PEER,
                              &numPcesPerLce,
                              &numLces,
                              &supportedPceMask,
                              &supportedLceMask,
                              &pcesPerHshub);

    maxLceCount = supportedLceMask;
    HIGHESTBITIDX_32(maxLceCount); // HIGHESTBITIDX_32 is destructive.
    maxLceCount++;

    NV2080_CTRL_INTERNAL_HSHUB_GET_HSHUB_ID_FOR_LINKS_PARAMS params;
    portMemSet(&params, 0, sizeof(params));

    peerAvailableLceMask = kceGetNvlinkPeerSupportedLceMask_HAL(pGpu, pKCe, peerAvailableLceMask) &
                                kceGetLceMask(pGpu) &
                                ~(*pExposedLceMask);

    pKCe->nvlinkNumPeers = 0;

    if (knvlinkIsGpuConnectedToNvswitch(pGpu, pKernelNvlink))
    {
        //
        // On NVSwitch systems, we only create 1 aperture for all p2p connections.
        // For PCE2LCE mapping, we should only assign 1 LCE for this connection.
        //
        // Since we mark the loopback connections in peerLinkMasks with the appropriate
        // links (see _nvlinkUpdateSwitchLinkMasks), we can use that to calculate
        // the PCE2LCE config.
        //
        gpuMask = NVBIT32(pGpu->gpuInstance);
    }
    else
    {
        // On direct connected systems, we'll loop over each GPU in the system
        // and assign a peer LCE for each connection
        (void)gpumgrGetGpuAttachInfo(NULL, &gpuMask);
    }

   while ((pRemoteGpu = gpumgrGetNextGpu(gpuMask, &gpuInstance)) != NULL)
   {
        NvU32 numLinksToPeer = knvlinkGetNumLinksToPeer(pGpu, pKernelNvlink, pRemoteGpu);

        if (numLinksToPeer == 0)
        {
            continue;
        }

        pceMask = 0;

        if (peerAvailableLceMask == 0)
        {
            //
            // peerAvailableLceMask is initialized to even async LCEs at the
            // top of the function.
            // As a result, if at any point in the loop, this mask == 0,
            // it implies we have used up all even async LCEs and should move to
            // using odd async LCEs.
            //
            peerAvailableLceMask = kceGetNvlinkPeerSupportedLceMask_HAL(pGpu, pKCe, peerAvailableLceMask) &
                                        kceGetLceMask(pGpu) &
                                        ~(*pExposedLceMask);
        }


        lceIndex = CE_GET_LOWEST_AVAILABLE_IDX(peerAvailableLceMask);

        if (lceIndex >= maxLceCount)
        {
            NV_PRINTF(LEVEL_ERROR,
                        "Invalid LCE Index Request. lceIndex = %d, maxLceCount = %d\n",
                        lceIndex,
                        maxLceCount);
            return NV_OK;
        }

        pKCe->nvlinkNumPeers++;

        peerLinkMask = knvlinkGetLinkMaskToPeer(pGpu, pKernelNvlink, pRemoteGpu);
        if (peerLinkMask == 0)
        {
            NV_PRINTF(LEVEL_INFO, "GPU%d has nvlink disabled. Skip LCE-PCE mapping.\n", pRemoteGpu->gpuInstance);
            continue;
        }

        params.linkMask = peerLinkMask;

        status = knvlinkExecGspRmRpc(pGpu,
                                     pKernelNvlink,
                                     NV2080_CTRL_CMD_INTERNAL_HSHUB_GET_HSHUB_ID_FOR_LINKS,
                                     &params,
                                     sizeof(params));
        NV_ASSERT_OK_OR_RETURN(status);

        FOR_EACH_INDEX_IN_MASK(32, linkId, peerLinkMask)
        {
            hshubIndex  = params.hshubIds[linkId];
            pceIndex    = CE_GET_LOWEST_AVAILABLE_IDX(pAvailablePceMaskForConnectingHub[hshubIndex]);

            bIsPceAvailable = _ceGetAlgorithmPceIndex(pGpu, pKCe, pAvailablePceMaskForConnectingHub, &pceIndex, &hshubIndex);
            if (bIsPceAvailable && (pceIndex < kceGetPce2lceConfigSize1_HAL(pKCe)))
            {
                pceMask                                         |= NVBIT32(pceIndex);
                pAvailablePceMaskForConnectingHub[hshubIndex]   &= ~(NVBIT32(pceIndex));
            }
        }
        FOR_EACH_INDEX_IN_MASK_END;

        if (pceMask != 0)
        {
            peerAvailableLceMask    &= ~(NVBIT32(lceIndex));
            *pExposedLceMask        |= NVBIT32(lceIndex);
            bPeerAssigned            = NV_TRUE;

            FOR_EACH_INDEX_IN_MASK(32, pceIndex, pceMask)
            {
                //
                // Convert absolute LCE index to shim local index
                //
                pKCe->pPceLceMap[pceIndex] = lceIndex % NV_KCE_GROUP_ID_STRIDE;
                NV_PRINTF(LEVEL_INFO, "Nvlink peer CE Mapping for GPU%d <-> GPU%d -- PCE Index: %d -> LCE Index: %d\n",
                          pGpu->gpuInstance,
                          pRemoteGpu->gpuInstance,
                          pceIndex,
                          lceIndex);
            }
            FOR_EACH_INDEX_IN_MASK_END;
        }
    }

    if (!bPeerAssigned)
    {
        status = NV_WARN_NOTHING_TO_DO;
    }

    return status;
}

/**
 * @brief This function takes care of mappings for GRCE Case
 *        If NvLinks are mapped, share GRCE with the lowest
 *        LCE used. If not, mark GRCE as non sharing and assign.
 *
 * @param[in]   pGpu                               OBJGPU pointer
 * @param[in]   pKCe                               KernelCE pointer
 * @param[in]   pAvailablePceMaskForConnectingHub  Pointer to CEs available per HSHUB
 * @param[out]  pLocalPceLceMap                    Pointer to PCE-LCE array
 * @param[out]  pExposedLceMask                    Pointer to LCE Mask
 */
void
kceMapPceLceForGRCE_GB100
(
    OBJGPU   *pGpu,
    KernelCE *pKCe,
    NvU32    *pAvailablePceMaskForConnectingHub,
    NvU32    *pLocalPceLceMap,
    NvU32    *pExposedLceMask,
    NvU32    *pLocalGrceMap,
    NvU32     fbPceMask
)
{
    NvU32         lceIndex         = 0;
    NvU32         grceIdx          = 0;
    NvU32         hshubIndex;
    NvU32         pceIndex;
    KernelCE     *pKCeIter;

    KCE_ITER_SHIM_BEGIN(pGpu, pKCeIter)
    {
        //
        // Assign GRCE PCEs only on the first shim instance.
        //
        if (pKCeIter->shimInstance != 0)
        {
            continue;
        }

        hshubIndex = CE_GET_LOWEST_AVAILABLE_IDX(pKCeIter->shimConnectingHubMask);

        // Some PCEs have not been assigned. Pick one and use that for GRCE.
        if ((pAvailablePceMaskForConnectingHub[hshubIndex] & ~(pKCeIter->decompPceMask)) != 0)
        {
            lceIndex            = 0;
            *pExposedLceMask   |= NVBIT32(lceIndex);
            pceIndex            = CE_GET_LOWEST_AVAILABLE_IDX((pAvailablePceMaskForConnectingHub[hshubIndex] &
                                                              ~(pKCeIter->decompPceMask)));

            if (pceIndex < kceGetPce2lceConfigSize1_HAL(pKCeIter))
            {
                pKCeIter->pPceLceMap[pceIndex]                  = lceIndex;
                pAvailablePceMaskForConnectingHub[hshubIndex]  &= ~(NVBIT(pceIndex));
                pLocalGrceMap[grceIdx]                          = DRF_NUM(_CE, _GRCE_CONFIG, _SHARED, 0) |
                                                                  DRF_DEF(_CE, _GRCE_CONFIG, _SHARED_LCE, _NONE);

                 NV_PRINTF(LEVEL_INFO, "GRCE is not shared and mapped to LCE Index: 0.\n");
            }
        }
        else
        {
            // Find the lowest PCE used which is not Decomp and share with that LCE
            NvU32 size = kceGetPce2lceConfigSize1_HAL(pKCeIter);

            for (pceIndex = 0; pceIndex < size; ++pceIndex)
            {
                if ((NVBIT32(pceIndex) & pKCeIter->decompPceMask) == 0)
                {
                    if (pKCeIter->pPceLceMap[pceIndex] != NV2080_CTRL_CE_UPDATE_PCE_LCE_MAPPINGS_INVALID_LCE)
                    {
                        lceIndex = pKCeIter->pPceLceMap[pceIndex];
                        NV_PRINTF(LEVEL_INFO,
                                  "GRCE is shared and mapped to LCE Index: %d.\n",
                                  lceIndex);
                        break;
                    }
                }
            }

            pLocalGrceMap[grceIdx] = DRF_NUM(_CE, _GRCE_CONFIG, _SHARED, 1) |
                                     DRF_NUM(_CE, _GRCE_CONFIG, _SHARED_LCE, lceIndex);
        }
    }
    KCE_ITER_END;
}

/**
 * @brief This function takes care of mappings for PCIe Case
 *        H2D and D2H. Map 1 PCE from 3 HSHUBs to on LCE - do
 *        this twice for H2D and D2H case.
 *
 * @param[in]   pGpu                               OBJGPU pointer
 * @param[in]   pKCe                               KernelCE pointer
 * @param[in]   pAvailablePceMaskForConnectingHub  Pointer to CEs available per HSHUB
 * @param[out]  pExposedLceMask                    Pointer to LCE Mask
 */
void
kceMapPceLceForPCIe_GB100
(
    OBJGPU      *pGpu,
    KernelCE    *pKCe,
    NvU32       *pAvailablePceMaskForConnectingHub,
    NvU32       *pExposedLceMask
)
{
    KernelCE   *pKCeIter        = NULL;
    NvU32       maxLceCount;
    NvU32       numPcesAssigned = 0;
    NvU32       pceIndex;
    NvU32       lceMask;
    NvU32       lceIndex;
    NvU32       hshubIndex;
    NvU32       index;
    NvU32       lastShimInstance = 0;
    NvU32       numPcesPerLce;
    NvU32       numLces;
    NvU32       supportedPceMask;
    NvU32       supportedLceMask;
    NvU32       pcesPerHshub;

    kceGetPceConfigForLceType(
        pGpu,
        pKCe,
        NV2080_CTRL_CE_LCE_TYPE_PCIE,
        &numPcesPerLce,
        &numLces,
        &supportedPceMask,
        &supportedLceMask,
        &pcesPerHshub);

    maxLceCount = supportedLceMask;
    HIGHESTBITIDX_32(maxLceCount); // HIGHESTBITIDX_32 is destructive.
    maxLceCount++;

    KCE_ITER_SHIM_BEGIN(pGpu, pKCeIter)
    {
        lastShimInstance = NV_MAX(pKCeIter->shimInstance, lastShimInstance);
    }
    KCE_ITER_END;

    KCE_ITER_SHIM_BEGIN(pGpu, pKCeIter)
    {
        //
        // Assign PCEs for PCIe only on last shim instance.
        //
        if (pKCeIter->shimInstance != lastShimInstance)
        {
            continue;
        }

        lceMask = kceGetLceMaskForShimInstance_HAL(pGpu, pKCeIter) &
                  supportedLceMask &
                  ~(*pExposedLceMask);

        for (index = 0; index < numLces; ++index)
        {
            numPcesAssigned     = 0;
            lceIndex            = CE_GET_LOWEST_AVAILABLE_IDX(lceMask);

            if (lceIndex >= maxLceCount)
            {
                break;
            }

            FOR_EACH_INDEX_IN_MASK(32, hshubIndex, pKCeIter->shimConnectingHubMask)
            {
                if (numPcesAssigned >= numPcesPerLce)
                {
                    break;
                }

                if (pAvailablePceMaskForConnectingHub[hshubIndex] == 0)
                {
                    continue;
                }

                pceIndex = CE_GET_LOWEST_AVAILABLE_IDX((pAvailablePceMaskForConnectingHub[hshubIndex] & supportedPceMask));

                if (pceIndex < kceGetPce2lceConfigSize1_HAL(pKCeIter))
                {
                    pAvailablePceMaskForConnectingHub[hshubIndex]   &= ~(NVBIT32(pceIndex));
                    //
                    // Convert absolute LCE index to shim local index
                    //
                    pKCeIter->pPceLceMap[pceIndex]           = lceIndex % NV_KCE_GROUP_ID_STRIDE;
                    *pExposedLceMask                        |= NVBIT(lceIndex);
                    lceMask                                 &= ~(NVBIT32(lceIndex));

                    NV_PRINTF(LEVEL_INFO,
                              "PCIe CE Mapping -- PCE Index: %d -> LCE Index: %d\n",
                              pceIndex,
                              lceIndex);

                    ++numPcesAssigned;
                }
            }
            FOR_EACH_INDEX_IN_MASK_END;
        }
    }
    KCE_ITER_END;
}

/**
 * @brief This function takes care of mappings for C2C case.
 *        Assign the required number of PCEs by preferring the
 *        even numbered LCE.
 *
 * @param[in]   pGpu                                OBJGPU pointer
 * @param[in]   pKCe                                KernelCE pointer
 * @param[in]   pAvailablePceMaskForConnectingHub   Pointer to CEs available per HSHUB
 * @param[out]  pLocalPceLceMap                     Pointer to PCE-LCE array
 * @param[out]  pExposedLceMask                     Pointer to LCE Mask
 */
NV_STATUS
kceMapPceLceForC2C_GB100
(
    OBJGPU      *pGpu,
    KernelCE    *pKCe,
    NvU32       *pAvailablePceMaskForConnectingHub,
    NvU32       *pLocalPceLceMap,
    NvU32       *pExposedLceMask
)
{
    KernelCE   *pKCeIter         = NULL;
    NvU32       maxLceCount;
    NvU32       pceIndex;
    NvU32       lceIndex;
    NvU32       lceMask;
    NvU32       hshubIndex;
    NvU32       index;
    NvU32       lastShimInstance = 0;
    NvU32       numPcesPerLce;
    NvU32       numLces;
    NvU32       supportedPceMask;
    NvU32       supportedLceMask;
    NvU32       pcesPerHshub;
    NvU32       numC2CLcesToAssign;
    NvBool      bAssignedAtleastOneLce;

    kceGetPceConfigForLceType(pGpu,
                              pKCe,
                              NV2080_CTRL_CE_LCE_TYPE_C2C,
                              &numPcesPerLce,
                              &numLces,
                              &supportedPceMask,
                              &supportedLceMask,
                              &pcesPerHshub);

    maxLceCount = supportedLceMask;
    HIGHESTBITIDX_32(maxLceCount); // HIGHESTBITIDX_32 is destructive.
    maxLceCount++;

    // Choose the last shim instance that has PCEs
    KCE_ITER_SHIM_BEGIN(pGpu, pKCeIter)
    {
        // Skip any shims that don't have any PCEs connected to the hubs
        if (pKCeIter->shimConnectingHubMask == 0)
        {
            continue;
        }

        hshubIndex = CE_GET_LOWEST_AVAILABLE_IDX(pKCeIter->shimConnectingHubMask);
        pceIndex   = CE_GET_LOWEST_AVAILABLE_IDX((pAvailablePceMaskForConnectingHub[hshubIndex] &
                                                  supportedPceMask));

        if (pceIndex < kceGetPce2lceConfigSize1_HAL(pKCeIter))
        {
            lastShimInstance = NV_MAX(pKCeIter->shimInstance, lastShimInstance);
        }
    }
    KCE_ITER_END;

    KCE_ITER_SHIM_BEGIN(pGpu, pKCeIter)
    {
        // Assign PCEs for C2C only on last shim instance.
        if (pKCeIter->shimInstance != lastShimInstance)
        {
            continue;
        }

        lceMask = kceGetLceMaskForShimInstance_HAL(pGpu, pKCeIter) &
                  supportedLceMask &
                  ~(*pExposedLceMask);

        if ((lceMask & NV_CE_EVEN_ASYNC_LCE_MASK) != 0)
        {
            lceMask &= NV_CE_EVEN_ASYNC_LCE_MASK;
        }

        numC2CLcesToAssign = numLces;

        while (numC2CLcesToAssign > 0)
        {
            bAssignedAtleastOneLce  = NV_FALSE;
            lceIndex                = CE_GET_LOWEST_AVAILABLE_IDX(lceMask);
            if (lceIndex >= maxLceCount)
            {
                break;
            }

            FOR_EACH_INDEX_IN_MASK(32, hshubIndex, pKCeIter->shimConnectingHubMask)
            {
                if (pAvailablePceMaskForConnectingHub[hshubIndex] == 0)
                {
                    // floorswept HSHUB with no PCEs available
                    continue;
                }

                for (index = 0; index < pcesPerHshub; ++index)
                {
                    pceIndex = CE_GET_LOWEST_AVAILABLE_IDX((pAvailablePceMaskForConnectingHub[hshubIndex] & supportedPceMask));

                    if (pceIndex >= kceGetPce2lceConfigSize1_HAL(pKCeIter))
                    {
                        break;
                    }

                    pAvailablePceMaskForConnectingHub[hshubIndex] &= ~(NVBIT32(pceIndex));

                    // Convert absolute LCE index to shim local index
                    pKCeIter->pPceLceMap[pceIndex]  = lceIndex % NV_KCE_GROUP_ID_STRIDE;
                    lceMask                        &= ~NVBIT32(lceIndex);

                    *pExposedLceMask               |= NVBIT32(lceIndex);
                    bAssignedAtleastOneLce          = NV_TRUE;

                    NV_PRINTF(LEVEL_INFO,
                              "C2C CE Mapping -- PCE Index: %d -> LCE Index: %d\n",
                              pceIndex,
                              lceIndex);
                }
            }
            FOR_EACH_INDEX_IN_MASK_END;

            if (bAssignedAtleastOneLce)
            {
                --numC2CLcesToAssign;
            }
            else
            {
                NV_PRINTF(LEVEL_ERROR, "Unable to assign the required number of LCEs for C2C.\n");
                return NV_ERR_INSUFFICIENT_RESOURCES;
            }
        }
    }
    KCE_ITER_END;

    return NV_OK;
}

/**
 * @brief This function takes care of mappings for Scrub Case.
 *        Map 2 PCEs from each HSHUB and map to lowest even
 *        numbered LCE if possible. If no even numbered LCE
 *        is free, choose an odd numbered LCE. (Bug 4656188)
 *
 * @param[in]   pGpu                               OBJGPU pointer
 * @param[in]   pKCe                               KernelCE pointer
 * @param[in]   pAvailablePceMaskForConnectingHub  Pointer to CEs available per HSHUB
 * @param[out]  pExposedLceMask                    Pointer to LCE Mask
 */
NV_STATUS
kceMapPceLceForScrub_GB100
(
    OBJGPU      *pGpu,
    KernelCE    *pKCe,
    NvU32       *pAvailablePceMaskForConnectingHub,
    NvU32       *pExposedLceMask
)
{
    KernelCE   *pKCeIter         = NULL;
    NvU32       maxLceCount;
    NvU32       status           = NV_WARN_NOTHING_TO_DO;
    NvU32       pceIndex;
    NvU32       lceIndex;
    NvU32       lceMask;
    NvU32       hshubIndex;
    NvU32       index;
    NvU32       lastShimInstance = 0;
    NvU32       numPcesPerLce;
    NvU32       numLces;
    NvU32       supportedPceMask;
    NvU32       supportedLceMask;
    NvU32       pcesPerHshub;

    kceGetPceConfigForLceType(pGpu,
                              pKCe,
                              NV2080_CTRL_CE_LCE_TYPE_SCRUB,
                              &numPcesPerLce,
                              &numLces,
                              &supportedPceMask,
                              &supportedLceMask,
                              &pcesPerHshub);

    maxLceCount = supportedLceMask;
    HIGHESTBITIDX_32(maxLceCount); // HIGHESTBITIDX_32 is destructive.
    maxLceCount++;

    KCE_ITER_SHIM_BEGIN(pGpu, pKCeIter)
    {
        lastShimInstance = NV_MAX(pKCeIter->shimInstance, lastShimInstance);
    }
    KCE_ITER_END;

    KCE_ITER_SHIM_BEGIN(pGpu, pKCeIter)
    {
        //
        // Assign PCEs for PCIe only on last shim instance.
        //
        if (pKCeIter->shimInstance != lastShimInstance)
        {
            continue;
        }

        lceMask = kceGetLceMaskForShimInstance_HAL(pGpu, pKCeIter) &
                  supportedLceMask &
                  ~(*pExposedLceMask);

        if ((lceMask & NV_CE_EVEN_ASYNC_LCE_MASK) != 0)
        {
            lceMask &= NV_CE_EVEN_ASYNC_LCE_MASK;
        }

        lceIndex = CE_GET_LOWEST_AVAILABLE_IDX(lceMask);
        if (lceIndex >= maxLceCount)
        {
            break;
        }

        FOR_EACH_INDEX_IN_MASK(32, hshubIndex, pKCeIter->shimConnectingHubMask)
        {
            if (pAvailablePceMaskForConnectingHub[hshubIndex] == 0)
            {
                // floorswept HSHUB with no PCEs available
                continue;
            }

            for (index = 0; index < pcesPerHshub; ++index)
            {
                pceIndex = CE_GET_LOWEST_AVAILABLE_IDX((pAvailablePceMaskForConnectingHub[hshubIndex] & supportedPceMask));

                if (pceIndex < kceGetPce2lceConfigSize1_HAL(pKCeIter))
                {
                    pAvailablePceMaskForConnectingHub[hshubIndex]   &= (~(NVBIT32(pceIndex)));

                    //
                    // Convert absolute LCE index to shim local index
                    //
                    pKCeIter->pPceLceMap[pceIndex]           = lceIndex % NV_KCE_GROUP_ID_STRIDE;

                    *pExposedLceMask                        |= NVBIT32(lceIndex);

                    NV_PRINTF(LEVEL_INFO,
                             "Scrub CE Mapping -- PCE Index: %d -> LCE Index: %d\n",
                              pceIndex,
                              lceIndex);

                    status = NV_OK;
                }
            }
        }
        FOR_EACH_INDEX_IN_MASK_END;
    }
    KCE_ITER_END;

    return status;
}

/**
 * @brief Some clients rely on LCE4 also being turned on when there
 *        are no NVLink peers. This function sets up LCE4 if not already
 *        assigned by mapping it to one PCE.
 *
 * @param[in]   pGpu                               OBJGPU pointer
 * @param[in]   pKCe                               KernelCE pointer
 * @param[in]   pAvailablePceMaskForConnectingHub  Pointer to CEs available per HSHUB
 * @param[out]  pLocalPceLceMap                    Pointer to PCE-LCE array
 * @param[out]  pExposedLceMask                    Pointer to LCE Mask
 *
 * Returns NV_OK
 */
NV_STATUS
kceMapAsyncLceDefault_GB100
(
    OBJGPU     *pGpu,
    KernelCE   *pKCe,
    NvU32      *pAvailablePceMaskForConnectingHub,
    NvU32      *pLocalPceLceMap,
    NvU32      *pExposedLceMask,
    NvU32       numDefaultPces
)
{
    KernelCE   *pKCeIter    = NULL;
    NvU32       maxLceCount;
    NvU32       pceIndex;
    NvU32       lceIndex;
    NvU32       lceMask;
    NvU32       availablePceMask;
    NvU32       hshubIndex;
    NvU32       numPcesPerLce;
    NvU32       numLces;
    NvU32       supportedPceMask;
    NvU32       supportedLceMask;
    NvU32       pcesPerHshub;

    //
    // Return early if LCE4 is already assigned.
    //
    if (*pExposedLceMask & NVBIT32(NV_CE_LCE_4))
    {
        return NV_OK;
    }

    kceGetPceConfigForLceType(
        pGpu,
        pKCe,
        NV2080_CTRL_CE_LCE_TYPE_SCRUB,
        &numPcesPerLce,
        &numLces,
        &supportedPceMask,
        &supportedLceMask,
        &pcesPerHshub);

    maxLceCount = supportedLceMask;
    HIGHESTBITIDX_32(maxLceCount); // HIGHESTBITIDX_32 is destructive.
    maxLceCount++;

    // Assign all available non Decomp PCEs to lowest LCE
    KCE_ITER_SHIM_BEGIN(pGpu, pKCeIter)
    {
        lceMask = kceGetLceMaskForShimInstance_HAL(pGpu, pKCeIter)
                & NV_CE_EVEN_ASYNC_LCE_MASK
                & ~(*pExposedLceMask);

        lceIndex = CE_GET_LOWEST_AVAILABLE_IDX(lceMask);
        if (lceIndex >= maxLceCount)
        {
            break;
        }

        FOR_EACH_INDEX_IN_MASK(32, hshubIndex, pKCeIter->shimConnectingHubMask)
        {
            availablePceMask = pAvailablePceMaskForConnectingHub[hshubIndex] & supportedPceMask;
            FOR_EACH_INDEX_IN_MASK(32, pceIndex, availablePceMask)
            {
                //
                // Convert absolute LCE index to shim local index
                //
                pKCeIter->pPceLceMap[pceIndex]                  = lceIndex % NV_KCE_GROUP_ID_STRIDE;
                *pExposedLceMask                               |= NVBIT32(lceIndex);
                pAvailablePceMaskForConnectingHub[hshubIndex]  &= (~NVBIT32(pceIndex));
                NV_PRINTF(LEVEL_INFO,
                          "Async LCE Mapping -- PCE Index: %d -> LCE Index: %d\n",
                          pceIndex,
                          lceIndex);

                return NV_OK;
            }
            FOR_EACH_INDEX_IN_MASK_END;
        }
        FOR_EACH_INDEX_IN_MASK_END;
    }
    KCE_ITER_END;

    return NV_OK;
}

/**
 * @brief Use the algorithm to determine all the mappings for
 *        the given GPU.
 *
 * @param[in]   pGpu                   OBJGPU pointer
 * @param[in]   pKCe                   KernelCE pointer
 * @param[out]  pLocalPceLceMap        Pointer to PCE-LCE array
 * @param[out]  pLocalGrceMap          Pointer to GRCE array
 * @param[out]  plocalExposedCeMask     Pointer to LCE Mask
 *
 * Returns NV_TRUE if algorithm ran to completion with no errors
 */
NV_STATUS
kceGetMappings_GB100
(
    OBJGPU                  *pGpu,
    KernelCE                *pKCe,
    NVLINK_TOPOLOGY_PARAMS  *pTopoParams,
    NvU32                   *pLocalPceLceMap,
    NvU32                   *pLocalGrceMap,
    NvU32                   *pExposedLceMask
)
{
    NV_STATUS       status = NV_OK;
    KernelNvlink   *pKernelNvlink   = GPU_GET_KERNEL_NVLINK(pGpu);
    KernelBif      *pKernelBif      = GPU_GET_KERNEL_BIF(pGpu);
    KernelCE       *pKCeIter        = NULL;
    NvU32           availablePceMaskForConnectingHub[NV2080_CTRL_CE_MAX_HSHUBS];
    KernelCE       *pKCeShimOwner;
    NvU32           pceIndex;
    NvU32           hubIndex;
    NvBool          bIsC2CEnabled   = pKernelBif->getProperty(pKernelBif, PDB_PROP_KBIF_IS_C2C_LINK_UP);

    //
    // Mapping algorithm will map all Shim Instances, but only
    // needs to be called one time. Any repeated calls for additional
    // shim instances should be ignored and getMappings will return
    // the mappings which have already been calculated. However,
    // if called for initial shim, remap is necessary because NVLink
    // status may have changed and requires remap.
    //
    status = kceFindShimOwner(pGpu, pKCe, &pKCeShimOwner);
    if (pKCeShimOwner == NULL || status != NV_OK)
    {
        return NV_OK;
    }

    // Do not change the mappings if the KernelCE objects are unloaded.
    if (pKCeShimOwner->pPceLceMap == NULL)
    {
        return NV_OK;
    }

    if (pKCeShimOwner->shimInstance != 0 && pKCeShimOwner->bMapComplete)
    {
        goto copyToLocalMap;
    }

    //
    // Initialize the available PCE mask for all the HSHUBs to 0.
    //
    for (hubIndex = 0; hubIndex < NV_ARRAY_ELEMENTS(availablePceMaskForConnectingHub); hubIndex++)
    {
        availablePceMaskForConnectingHub[hubIndex] = 0;
    }

    //
    // Prepare the per-HSHUB available PCE mask for all HSHUBs
    //
    KCE_ITER_SHIM_BEGIN(pGpu, pKCeIter)
    {
        //
        // Get the PCEs available for the current shim and update the local map (availablePceMaskForConnectingHub).
        //
        kceGetAvailableHubPceMask(pGpu, pKCeIter, pTopoParams);

        for (hubIndex = 0; hubIndex < NV_ARRAY_ELEMENTS(availablePceMaskForConnectingHub); hubIndex++)
        {
            availablePceMaskForConnectingHub[hubIndex] |= pTopoParams->pceAvailableMaskPerConnectingHub[hubIndex];
            
            if (pTopoParams->pceAvailableMaskPerConnectingHub[hubIndex])
            {
                pKCeIter->shimConnectingHubMask |= NVBIT32(hubIndex);
            }
        }

        // Do not change the mappings if the KernelCE objects are unloaded.
        if (pKCeIter->pPceLceMap == NULL)
        {
            return NV_OK;
        }

        for (pceIndex = 0; pceIndex < kceGetPce2lceConfigSize1_HAL(pKCeIter); pceIndex++)
        {
            pKCeIter->pPceLceMap[pceIndex] = NV2080_CTRL_CE_UPDATE_PCE_LCE_MAPPINGS_INVALID_LCE;
        }

        pKCeIter->bMapComplete = NV_TRUE;
    }
    KCE_ITER_END;

    //
    // A) Assign Decomp PCEs
    //
    kceMapPceLceForDecomp_HAL(pGpu,
                              pKCeShimOwner,
                              availablePceMaskForConnectingHub,
                              pExposedLceMask);

    //
    // B) Next, assign PCEs for PCIe or C2C case
    //
    if (bIsC2CEnabled)
    {
        kceMapPceLceForC2C_HAL(pGpu,
                               pKCeShimOwner,
                               availablePceMaskForConnectingHub,
                               pLocalPceLceMap,
                               pExposedLceMask);
    }
    else
    {
        kceMapPceLceForPCIe_HAL(pGpu,
                                pKCeShimOwner,
                                availablePceMaskForConnectingHub,
                                pExposedLceMask);
    }

    //
    // C) First, assign PCEs to Peers if nvlink is enabled or NVswitch enabled
    //
    if (pKernelNvlink && !knvlinkIsForcedConfig(pGpu, pKernelNvlink))
    {
        kceMapPceLceForNvlinkPeers_HAL(pGpu,
                                       pKCeShimOwner,
                                       availablePceMaskForConnectingHub,
                                       pLocalPceLceMap,
                                       pExposedLceMask);
    }

    //
    // D) Assign PCEs for Scrub
    //
    kceMapPceLceForScrub_HAL(pGpu,
                             pKCeShimOwner,
                             availablePceMaskForConnectingHub,
                             pExposedLceMask);

    //
    // E) Assign GRCE after PCEs to ensure proper sharing if required
    //
    kceMapPceLceForGRCE_HAL(pGpu,
                            pKCeShimOwner,
                            availablePceMaskForConnectingHub,
                            pLocalPceLceMap,
                            pExposedLceMask,
                            pLocalGrceMap,
                            pTopoParams->fbhubPceMask);

    //
    // At minimum PCEs need to be assigned for LCE4. Assign PCEs for LCE4 if not already assigned.
    //
    kceMapAsyncLceDefault_HAL(pGpu,
                              pKCeShimOwner,
                              availablePceMaskForConnectingHub,
                              pLocalPceLceMap,
                              pExposedLceMask,
                              0);

    NV_PRINTF(LEVEL_INFO, "Assigned LCE mask = 0x%x.\n", *pExposedLceMask);

copyToLocalMap:
    //
    // Return PceLceMap depending whichever shim is requested
    //
    for (pceIndex = 0; pceIndex < kceGetPce2lceConfigSize1_HAL(pKCeShimOwner); pceIndex++)
    {
        pLocalPceLceMap[pceIndex] = pKCeShimOwner->pPceLceMap[pceIndex];
    }

    return NV_OK;
}

/**
 * @brief Returns the LCEs assigned for PCIe/C2C read write traffic.
 *
 * @param[in]      pGpu       OBJGPU pointer
 * @param[in]      pKCe       KernelCE pointer
 * @param[out]     pReadLce   LCE assigned for read
 * @param[out]     pWriteLce  LCE assigned for write
 */
void
kceGetSysmemRWLCEs_GB100
(
    OBJGPU      *pGpu,
    KernelCE    *pKCe,
    NvU32       *pReadLce,
    NvU32       *pWriteLce
)
{
    NvU32       assignedLces[2];
    NvU32       count;
    KernelCE   *pKCeShimIter;
    KernelCE   *pKCeIter;

    KCE_ITER_SHIM_BEGIN(pGpu, pKCeShimIter)
    {
        count = 0;

        assignedLces[0] = NV2080_CTRL_CE_UPDATE_PCE_LCE_MAPPINGS_INVALID_LCE;
        assignedLces[1] = NV2080_CTRL_CE_UPDATE_PCE_LCE_MAPPINGS_INVALID_LCE;

        KCE_ITER_BEGIN(pGpu, pKCeShimIter, pKCeIter, NV_CE_SYSMEM_LCE_START_INDEX)
        {
            if (pKCeIter->publicID >= NV_CE_SYSMEM_LCE_END_INDEX)
            {
                break;
            }

            if (!pKCeIter->bStubbed)
            {
                assignedLces[count++] = pKCeIter->publicID;
            }

            if (count == 2)
            {
                *pReadLce   = assignedLces[0];
                *pWriteLce  = assignedLces[1];
                return;
            }
        }
        KCE_ITER_END;
    }
    KCE_ITER_END;
}

/**
 * @brief Returns the LCE mask for the corresponding shim instance.
 *
 * @param[in]      pGpu       OBJGPU pointer
 * @param[in]      pKCe       KernelCE pointer
 */
NvU32
kceGetLceMaskForShimInstance_GB100
(
    OBJGPU      *pGpu,
    KernelCE    *pKCe
)
{
    NvU32 shimMask;
    NvU32 shimInstance = pKCe->shimInstance;

    shimMask = NVBIT32(NV_KCE_GROUP_ID_STRIDE * (shimInstance + 1)) -
               NVBIT32(NV_KCE_GROUP_ID_STRIDE * shimInstance);

    return kceGetLceMask(pGpu) & shimMask;
}
