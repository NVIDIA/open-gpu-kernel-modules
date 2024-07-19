/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/mem_mgr/mem_mgr.h"
#include "gpu/mem_sys/kern_mem_sys.h"
#include "gpu/mem_mgr/heap.h"
#include "mem_mgr/video_mem.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "gpu/bus/kern_bus.h"
#include "gpu/gpu_resource_desc.h"
#include "gpu/device/device.h"
#include "gpu/subdevice/subdevice.h"
#include "gpu/subdevice/subdevice_diag.h"
#include "ctrl/ctrl0080/ctrl0080fb.h"
#include "core/locks.h"
#include "platform/sli/sli.h"
#include "rmapi/rs_utils.h"
#include "rmapi/mapping_list.h"
#include "platform/chipset/chipset.h"

#include "class/cl0040.h" /* NV01_MEMORY_LOCAL_USER */
#include "class/cl003e.h" /* NV01_MEMORY_SYSTEM */
#include "class/cl84a0.h" /* NV01_MEMORY_LIST_XXX */
#include "class/cl00b1.h" /* NV01_MEMORY_HW_RESOURCES */

//
// memmgrGetDeviceCaps
//
// This routine gets cap bits in unicast. If bCapsInitialized is passed as
// NV_FALSE, the caps will be copied into pFbCaps without OR/ANDing. Otherwise,
// the caps bits for the current GPU will be OR/ANDed together with pFbCaps to
// create a single set of caps that accurately represents the functionality of
// the device.
//
static void
memmgrGetDeviceCaps
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager,
    NvU8          *pFbCaps,
    NvBool         bCapsInitialized
)
{
    NvU8 tempCaps[NV0080_CTRL_FB_CAPS_TBL_SIZE], temp;
    KernelMemorySystem *pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);
    const MEMORY_SYSTEM_STATIC_CONFIG *pMemorySystemConfig =
        kmemsysGetStaticConfig(pGpu, pKernelMemorySystem);

    NV_ASSERT(!gpumgrGetBcEnabledStatus(pGpu));

    portMemSet(tempCaps, 0, NV0080_CTRL_FB_CAPS_TBL_SIZE);

    if (pMemoryManager->bScanoutSysmem)
        RMCTRL_SET_CAP(tempCaps, NV0080_CTRL_FB_CAPS, _SUPPORT_SCANOUT_FROM_SYSMEM);

    RMCTRL_SET_CAP(tempCaps, NV0080_CTRL_FB_CAPS, _SUPPORT_RENDER_TO_SYSMEM);
    RMCTRL_SET_CAP(tempCaps, NV0080_CTRL_FB_CAPS, _BLOCKLINEAR);
    RMCTRL_SET_CAP(tempCaps, NV0080_CTRL_FB_CAPS, _BLOCKLINEAR_GOBS_512);

    if (pKernelMemorySystem->bGpuCacheEnable)
        RMCTRL_SET_CAP(tempCaps, NV0080_CTRL_FB_CAPS, _SUPPORT_CACHED_SYSMEM);

    //
    // OS owned Heap doesn't need to be scrubber from Pascal+ chips, since HW Scrubber
    // takes care of it
    //
    if (pMemorySystemConfig->bEnabledEccFBPA &&
        pMemoryManager->bEccInterleavedVidmemScrub &&
        !IsGP100orBetter(pGpu))
    {
        RMCTRL_SET_CAP(tempCaps, NV0080_CTRL_FB_CAPS, _OS_OWNS_HEAP_NEED_ECC_SCRUB);
    }

    if (memmgrComprSupported(pMemoryManager, ADDR_SYSMEM))
    {
        RMCTRL_SET_CAP(tempCaps, NV0080_CTRL_FB_CAPS, _SUPPORT_SYSMEM_COMPRESSION);
    }

    if (pKernelMemorySystem->bDisableTiledCachingInvalidatesWithEccBug1521641)
    {
        if (pMemorySystemConfig->bEnabledEccFBPA || IS_VIRTUAL(pGpu))
        {
            RMCTRL_SET_CAP(tempCaps, NV0080_CTRL_FB_CAPS, _DISABLE_TILED_CACHING_INVALIDATES_WITH_ECC_BUG_1521641);
        }
    }

    if (pMemoryManager->bGenericKindSupport)
    {
        RMCTRL_SET_CAP(tempCaps, NV0080_CTRL_FB_CAPS, _GENERIC_PAGE_KIND);
    }

    if (memmgrIsScrubOnFreeEnabled(pMemoryManager)) {
        RMCTRL_SET_CAP(tempCaps, NV0080_CTRL_FB_CAPS, _VIDMEM_ALLOCS_ARE_CLEARED);
    }

    if (pMemorySystemConfig->bDisablePostL2Compression)
    {
        RMCTRL_SET_CAP(tempCaps, NV0080_CTRL_FB_CAPS, _DISABLE_PLC_GLOBALLY);
    }

    if (pKernelMemorySystem->bDisablePlcForCertainOffsetsBug3046774)
    {
        RMCTRL_SET_CAP(tempCaps, NV0080_CTRL_FB_CAPS, _PLC_BUG_3046774);
    }

    if (!IS_VIRTUAL(pGpu) || IS_VIRTUAL_WITH_FULL_SRIOV(pGpu))
    {
        // Legacy SRIOV modes lack the partial unmap plumbing
        RMCTRL_SET_CAP(tempCaps, NV0080_CTRL_FB_CAPS, _PARTIAL_UNMAP);
    }

    // If we don't have existing caps with which to reconcile, then just return
    if (!bCapsInitialized)
    {
        portMemCopy(pFbCaps, NV0080_CTRL_FB_CAPS_TBL_SIZE, tempCaps, NV0080_CTRL_FB_CAPS_TBL_SIZE);
        return;
    }

    // factor in this GPUs caps: feature caps use AND, WARS use OR
    RMCTRL_AND_CAP(pFbCaps, tempCaps, temp,
                   NV0080_CTRL_FB_CAPS, _SUPPORT_RENDER_TO_SYSMEM);
    RMCTRL_AND_CAP(pFbCaps, tempCaps, temp,
                   NV0080_CTRL_FB_CAPS, _SUPPORT_SCANOUT_FROM_SYSMEM);
    RMCTRL_AND_CAP(pFbCaps, tempCaps, temp,
                   NV0080_CTRL_FB_CAPS, _BLOCKLINEAR);
    RMCTRL_AND_CAP(pFbCaps, tempCaps, temp,
                   NV0080_CTRL_FB_CAPS, _BLOCKLINEAR_GOBS_512);
    RMCTRL_OR_CAP(pFbCaps, tempCaps, temp,
                   NV0080_CTRL_FB_CAPS, _L2_TAG_BUG_632241);
    RMCTRL_AND_CAP(pFbCaps, tempCaps, temp,
                   NV0080_CTRL_FB_CAPS, _SUPPORT_CACHED_SYSMEM);
    RMCTRL_AND_CAP(pFbCaps, tempCaps, temp,
                   NV0080_CTRL_FB_CAPS, _SUPPORT_C24_COMPRESSION);
    RMCTRL_AND_CAP(pFbCaps, tempCaps, temp,
                   NV0080_CTRL_FB_CAPS, _SUPPORT_SYSMEM_COMPRESSION);
    RMCTRL_OR_CAP(pFbCaps, tempCaps, temp,
                   NV0080_CTRL_FB_CAPS, _NISO_CFG0_BUG_534680);
    RMCTRL_OR_CAP(pFbCaps, tempCaps, temp,
                   NV0080_CTRL_FB_CAPS, _ISO_FETCH_ALIGN_BUG_561630);
    RMCTRL_OR_CAP(pFbCaps, tempCaps, temp,
                   NV0080_CTRL_FB_CAPS, _SINGLE_FB_UNIT);
    RMCTRL_OR_CAP(pFbCaps, tempCaps, temp,
                   NV0080_CTRL_FB_CAPS, _OS_OWNS_HEAP_NEED_ECC_SCRUB);
    RMCTRL_OR_CAP(pFbCaps, tempCaps, temp,
                   NV0080_CTRL_FB_CAPS, _DISABLE_TILED_CACHING_INVALIDATES_WITH_ECC_BUG_1521641);
    RMCTRL_AND_CAP(pFbCaps, tempCaps, temp,
                   NV0080_CTRL_FB_CAPS, _GENERIC_PAGE_KIND);
    RMCTRL_OR_CAP(pFbCaps, tempCaps, temp,
                   NV0080_CTRL_FB_CAPS, _DISABLE_MSCG_WITH_VR_BUG_1681803);
    RMCTRL_AND_CAP(pFbCaps, tempCaps, temp,
                   NV0080_CTRL_FB_CAPS, _VIDMEM_ALLOCS_ARE_CLEARED);
    RMCTRL_OR_CAP(pFbCaps, tempCaps, temp,
                   NV0080_CTRL_FB_CAPS, _DISABLE_PLC_GLOBALLY);
    RMCTRL_OR_CAP(pFbCaps, tempCaps, temp,
                   NV0080_CTRL_FB_CAPS, _PLC_BUG_3046774);
    RMCTRL_AND_CAP(pFbCaps, tempCaps, temp,
                   NV0080_CTRL_FB_CAPS, _PARTIAL_UNMAP);

    return;
}

static NV_STATUS
memmgrGetFbCaps(OBJGPU *pGpu, NvU8 *pFbCaps)
{
    NV_STATUS  rmStatus         = NV_OK;
    NvBool     bCapsInitialized = NV_FALSE;

    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY)
    {
        MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
        if (pMemoryManager == NULL)
        {
            rmStatus = NV_ERR_INVALID_POINTER;
            SLI_LOOP_BREAK;
        }
        memmgrGetDeviceCaps(pGpu, pMemoryManager, pFbCaps, bCapsInitialized);
        bCapsInitialized = NV_TRUE;
    }
    SLI_LOOP_END

    return rmStatus;
}

//
// deviceCtrlCmdFbGetCaps
//
// Lock Requirements:
//      Assert that API lock held on entry
//
NV_STATUS
deviceCtrlCmdFbGetCaps_IMPL
(
    Device *pDevice,
    NV0080_CTRL_FB_GET_CAPS_PARAMS *pFbCapsParams
)
{
    OBJGPU   *pGpu = GPU_RES_GET_GPU(pDevice);
    NvU8     *pFbCaps = NvP64_VALUE(pFbCapsParams->capsTbl);

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner());

    // sanity check array size
    if (pFbCapsParams->capsTblSize != NV0080_CTRL_FB_CAPS_TBL_SIZE)
    {
        NV_PRINTF(LEVEL_ERROR, "size mismatch: client 0x%x rm 0x%x\n",
                  pFbCapsParams->capsTblSize, NV0080_CTRL_FB_CAPS_TBL_SIZE);
        return NV_ERR_INVALID_ARGUMENT;
    }

    // now accumulate caps for entire device
    return memmgrGetFbCaps(pGpu, pFbCaps);
}

//
// deviceCtrlCmdFbGetCapsV2
//
// Lock Requirements:
//      Assert that API lock held on entry
//
NV_STATUS
deviceCtrlCmdFbGetCapsV2_IMPL
(
    Device *pDevice,
    NV0080_CTRL_FB_GET_CAPS_V2_PARAMS *pFbCapsParams
)
{
    OBJGPU   *pGpu     = GPU_RES_GET_GPU(pDevice);
    NvU8     *pFbCaps  = pFbCapsParams->capsTbl;
    NV_STATUS rmStatus;

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner());

    // now accumulate caps for entire device
    rmStatus = memmgrGetFbCaps(pGpu, pFbCaps);

    return rmStatus;
}

//
// deviceCtrlCmdSetDefaultVidmemPhysicality
//
// Lock Requirements:
//      Assert that API lock held on entry
//
NV_STATUS
deviceCtrlCmdSetDefaultVidmemPhysicality_IMPL
(
    Device *pDevice,
    NV0080_CTRL_FB_SET_DEFAULT_VIDMEM_PHYSICALITY_PARAMS *pParams
)
{
    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner());
    NvU32 override;

    switch (pParams->value)
    {
        case NV0080_CTRL_FB_DEFAULT_VIDMEM_PHYSICALITY_DEFAULT:
            override = NVOS32_ATTR_PHYSICALITY_DEFAULT;
            break;
        case NV0080_CTRL_FB_DEFAULT_VIDMEM_PHYSICALITY_CONTIGUOUS:
            override = NVOS32_ATTR_PHYSICALITY_CONTIGUOUS;
            break;
        case NV0080_CTRL_FB_DEFAULT_VIDMEM_PHYSICALITY_NONCONTIGUOUS:
            override = NVOS32_ATTR_PHYSICALITY_NONCONTIGUOUS;
            break;
        case NV0080_CTRL_FB_DEFAULT_VIDMEM_PHYSICALITY_ALLOW_NONCONTIGUOUS:
            override = NVOS32_ATTR_PHYSICALITY_ALLOW_NONCONTIGUOUS;
            break;
        default:
            return NV_ERR_INVALID_ARGUMENT;
    }
    pDevice->defaultVidmemPhysicalityOverride = override;

    return NV_OK;
}

//
// subdeviceCtrlCmdFbGetBar1Offset
//
// Lock Requirements:
//      Assert that API and GPUs lock held on entry
//
NV_STATUS
subdeviceCtrlCmdFbGetBar1Offset_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_FB_GET_BAR1_OFFSET_PARAMS *pFbMemParams
)
{
    NvHandle      hClient  = RES_GET_CLIENT_HANDLE(pSubdevice);
    NvHandle      hDevice  = RES_GET_PARENT_HANDLE(pSubdevice);
    NvU64         offset;
    RsCpuMapping *pCpuMapping = NULL;

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner());

    pCpuMapping = CliFindMappingInClient(hClient, hDevice, pFbMemParams->cpuVirtAddress);
    if (pCpuMapping == NULL)
        return NV_ERR_INVALID_ARGUMENT;

    offset = (NvU64)pFbMemParams->cpuVirtAddress - (NvU64)pCpuMapping->pLinearAddress;
    pFbMemParams->gpuVirtAddress = pCpuMapping->pPrivate->gpuAddress + offset;

    return NV_OK;
}

//
// subdeviceCtrlCmdFbIsKind
//
// Lock Requirements:
//      Assert that API and GPUs lock held on entry
//
NV_STATUS
subdeviceCtrlCmdFbIsKind_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_FB_IS_KIND_PARAMS *pIsKindParams
)
{
    OBJGPU        *pGpu           = GPU_RES_GET_GPU(pSubdevice);
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    NV_STATUS      status         = NV_OK;
    NvBool         rmResult;

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner());

    // perform appropriate RM operation based on the supported sdk operations
    switch (pIsKindParams->operation)
    {

        case NV2080_CTRL_FB_IS_KIND_OPERATION_SUPPORTED:
            rmResult = memmgrIsKind_HAL(pMemoryManager, FB_IS_KIND_SUPPORTED, pIsKindParams->kind);
            break;

        case NV2080_CTRL_FB_IS_KIND_OPERATION_COMPRESSIBLE:
            rmResult = memmgrIsKind_HAL(pMemoryManager, FB_IS_KIND_COMPRESSIBLE, pIsKindParams->kind);
            break;
        case NV2080_CTRL_FB_IS_KIND_OPERATION_COMPRESSIBLE_1:
            rmResult = memmgrIsKind_HAL(pMemoryManager, FB_IS_KIND_COMPRESSIBLE_1, pIsKindParams->kind);
            break;
        case NV2080_CTRL_FB_IS_KIND_OPERATION_COMPRESSIBLE_2:
            rmResult = memmgrIsKind_HAL(pMemoryManager, FB_IS_KIND_COMPRESSIBLE_2, pIsKindParams->kind);
            break;
        case NV2080_CTRL_FB_IS_KIND_OPERATION_COMPRESSIBLE_4:
            rmResult = memmgrIsKind_HAL(pMemoryManager, FB_IS_KIND_COMPRESSIBLE_4, pIsKindParams->kind);
            break;
        case NV2080_CTRL_FB_IS_KIND_OPERATION_ZBC:
            rmResult = memmgrIsKind_HAL(pMemoryManager, FB_IS_KIND_ZBC, pIsKindParams->kind);
            break;
        case NV2080_CTRL_FB_IS_KIND_OPERATION_ZBC_ALLOWS_1:
            rmResult = memmgrIsKind_HAL(pMemoryManager, FB_IS_KIND_ZBC_ALLOWS_1, pIsKindParams->kind);
            break;
        case NV2080_CTRL_FB_IS_KIND_OPERATION_ZBC_ALLOWS_2:
            rmResult = memmgrIsKind_HAL(pMemoryManager, FB_IS_KIND_ZBC_ALLOWS_2, pIsKindParams->kind);
            break;
        case NV2080_CTRL_FB_IS_KIND_OPERATION_ZBC_ALLOWS_4:
            rmResult = memmgrIsKind_HAL(pMemoryManager, FB_IS_KIND_ZBC_ALLOWS_4, pIsKindParams->kind);
            break;
        default:
            NV_ASSERT(0);
            status = NV_ERR_INVALID_ARGUMENT;
            return status;
    }

    // save the result in the params struct and return
    pIsKindParams->result = rmResult;
    return status;
}

NV_STATUS
subdeviceCtrlCmdFbGetMemAlignment_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_FB_GET_MEM_ALIGNMENT_PARAMS *pParams
)
{
    OBJGPU                 *pGpu       = GPU_RES_GET_GPU(pSubdevice);
    NvHandle                hClient    = RES_GET_CLIENT_HANDLE(pSubdevice);
    Device                 *pDevice    = GPU_RES_GET_DEVICE(pSubdevice);
    NvHandle                hObject    = RES_GET_HANDLE(pSubdevice);
    Heap                   *pHeap      = vidmemGetHeap(pGpu, pDevice, NV_FALSE, NV_FALSE);
    HEAP_ALLOC_HINT_PARAMS  AllocHint  = {0};
    NvU32                   i;
    NvU64                   _size, _alignment;
    NV_STATUS               status = NV_OK;

    _size = pParams->alignSize;
    _alignment = pParams->alignMask;
    AllocHint.pSize = &_size;
    AllocHint.pAlignment = &_alignment;

    // Type, Attributes, Flags
    AllocHint.type = pParams->alignType;
    AllocHint.pAttr = &pParams->alignAttr;
    AllocHint.pAttr2 = &pParams->alignAttr2;
    AllocHint.flags = pParams->alignInputFlags;

    // Size Information
    AllocHint.pHeight = &pParams->alignHeight;
    AllocHint.pWidth = &pParams->alignWidth;
    AllocHint.pPitch = &pParams->alignPitch;
    AllocHint.pKind = &pParams->alignKind;
    AllocHint.alignAdjust = 0x0;

    status = heapAllocHint(pGpu, pHeap, hClient, hObject, &AllocHint);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "heapAllocHint failed\n");
        return status;
    }

    //XXX64b torque converter
    pParams->alignMask = (NvU32)_alignment;
    pParams->alignSize = _size;

    if (NV_FALSE == AllocHint.ignoreBankPlacement)
    {
        for (i=0; i<MEM_NUM_BANKS_TO_TRY; i++)
        {
            if (MEM_NO_BANK_SELECTION == (AllocHint.bankPlacement & MEM_NO_BANK_SELECTION))
            {
                pParams->alignBank[i] = 0x0;
                pParams->alignOutputFlags[i] = NVAL_MAP_DIRECTION_UP;
            }
            else
            {
                pParams->alignBank[i] = (AllocHint.bankPlacement & MEM_BANK_MASK) + 1;
                pParams->alignOutputFlags[i] = (BANK_MEM_GROW_DOWN == (AllocHint.bankPlacement & BANK_MEM_GROW_MASK)) ? NVAL_MAP_DIRECTION_DOWN: NVAL_MAP_DIRECTION_UP;
            }
            AllocHint.bankPlacement >>= MEM_BANK_DATA_SIZE;
        }

    }
    else
    {
        for (i=0; i<NVAL_MAX_BANKS; i++)
        {
            pParams->alignBank[i] = 0x0;
            pParams->alignOutputFlags[i] = NVAL_MAP_DIRECTION_UP;
        }
    }

    // Keep Track of resources that we have allocated
    pParams->alignPad = (NvU32)AllocHint.pad;//XXX64b
    pParams->alignAdjust = (NvU32)AllocHint.alignAdjust;//XXX64b

    return NV_OK;
}

#if defined(DEBUG) || defined(DEVELOP) || defined(NV_VERIF_FEATURES) || defined(NV_MODS)
/*
 * Currently returns information for **all** clients;
 * returns actual sizes allocated from the heap,
 * not the sizes requested by the client
 */
NV_STATUS
subdeviceCtrlCmdFbGetClientAllocationInfo_IMPL(Subdevice *pSubdevice,
                                               NV2080_CTRL_CMD_FB_GET_CLIENT_ALLOCATION_INFO_PARAMS *pParams)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    RmClient **ppClient;
    NvU64 reservedAllocCount = pParams->allocCount;
    NvU64 reservedClientCount = pParams->clientCount;
    NvU64 curClientCount = 0;
    NvU64 curAllocCount = 0;

    NV2080_CTRL_CMD_FB_ALLOCATION_INFO* pLocalAllocInfo = NULL;
    NV2080_CTRL_CMD_FB_CLIENT_INFO* pLocalCliInfo = NULL;

    NV_STATUS status = NV_OK;

    if (reservedAllocCount != 0 && reservedClientCount != 0)
    {
        if(reservedAllocCount > (NV_U64_MAX/sizeof(NV2080_CTRL_CMD_FB_ALLOCATION_INFO)) ||
           reservedClientCount > (NV_U64_MAX/sizeof(NV2080_CTRL_CMD_FB_CLIENT_INFO)))
        {
            return NV_ERR_NO_MEMORY;
        }
        pLocalAllocInfo = (NV2080_CTRL_CMD_FB_ALLOCATION_INFO*) portMemAllocNonPaged(
                sizeof(NV2080_CTRL_CMD_FB_ALLOCATION_INFO) * reservedAllocCount);
        NV_ASSERT_OR_RETURN(pLocalAllocInfo != NULL, NV_ERR_NO_MEMORY);

        pLocalCliInfo = (NV2080_CTRL_CMD_FB_CLIENT_INFO*) portMemAllocNonPaged(
                sizeof(NV2080_CTRL_CMD_FB_CLIENT_INFO) * reservedClientCount);

        NV_ASSERT_OR_ELSE(pLocalCliInfo != NULL,
            portMemFree(pLocalAllocInfo);
            return NV_ERR_NO_MEMORY;
        );
    }

    for (ppClient = serverutilGetFirstClientUnderLock();
         ppClient;
         ppClient = serverutilGetNextClientUnderLock(ppClient))
    {
        NvBool bFirstPass = NV_TRUE;
        RmClient *pClient = *ppClient;
        RsClient  *pRsClient = staticCast(pClient, RsClient);

        RS_ITERATOR it = clientRefIter(pRsClient, 0, 0, RS_ITERATE_DESCENDANTS, NV_TRUE);

        while (clientRefIterNext(it.pClient, &it))
        {
            NvU64 nPages;
            NvU64 allocSz;
            NvU64 index;
            NvU32 memFlags;
            NvU32 addrSpaceResult;
            NV_ADDRESS_SPACE addrSpace;

            Memory *pMemoryInfo = dynamicCast(it.pResourceRef->pResource, Memory);

            if (pMemoryInfo == NULL)
                continue;

            addrSpace = pMemoryInfo->pMemDesc->_addressSpace;
            nPages = pMemoryInfo->pMemDesc->PageCount;
            allocSz = pMemoryInfo->pMemDesc->ActualSize;

            // Ignore memdescs not in SYSMEM or FBMEM
            if ((it.pResourceRef->externalClassId != NV01_MEMORY_LOCAL_USER &&
                    it.pResourceRef->externalClassId != NV01_MEMORY_SYSTEM &&
                    it.pResourceRef->externalClassId != NV01_MEMORY_LIST_SYSTEM &&
                    it.pResourceRef->externalClassId != NV01_MEMORY_LIST_FBMEM &&
                    it.pResourceRef->externalClassId != NV01_MEMORY_LIST_OBJECT &&
                    it.pResourceRef->externalClassId != NV01_MEMORY_HW_RESOURCES) ||
                pGpu != pMemoryInfo->pGpu)
            {
                continue;
            }

            if (pMemoryInfo->pMemDesc->_flags & MEMDESC_FLAGS_PHYSICALLY_CONTIGUOUS)
            {
                nPages = 1;
            }
            else
            {
                allocSz = pMemoryInfo->pMemDesc->_pageSize;
            }

            curAllocCount += nPages;

            // Only do client setup on first addded memdesc
            if (bFirstPass)
            {
                curClientCount++;
                bFirstPass = NV_FALSE;

                // Check if we have enough space for new client
                if (pLocalAllocInfo == NULL)
                    continue;

                if (curClientCount > reservedClientCount)
                    goto free_mem;

                pLocalCliInfo[curClientCount-1].handle = pRsClient->hClient;
                pLocalCliInfo[curClientCount-1].pid = pClient->ProcID;
                pLocalCliInfo[curClientCount-1].subProcessID = pClient->SubProcessID;
                portMemCopy(pLocalCliInfo[curClientCount-1].subProcessName, NV_PROC_NAME_MAX_LENGTH,
                    pClient->SubProcessName, NV_PROC_NAME_MAX_LENGTH);

            }

            // Check if we have enough space for memdesc info
            if (pLocalAllocInfo == NULL)
                continue;

            if (curAllocCount > reservedAllocCount)
                goto free_mem;

            memFlags = 0;

            addrSpaceResult = addrSpace == ADDR_SYSMEM ?
                NV2080_CTRL_CMD_FB_ALLOCATION_FLAGS_TYPE_SYSMEM:
                NV2080_CTRL_CMD_FB_ALLOCATION_FLAGS_TYPE_VIDMEM;

            memFlags = FLD_SET_REF_NUM(
                NV2080_CTRL_CMD_FB_ALLOCATION_FLAGS_TYPE,
                addrSpaceResult,
                memFlags);

            memFlags = FLD_SET_REF_NUM(
                NV2080_CTRL_CMD_FB_ALLOCATION_FLAGS_OWNER,
                pMemoryInfo->isMemDescOwner ? 1 : 0,
                memFlags);

            memFlags = FLD_SET_REF_NUM(
                NV2080_CTRL_CMD_FB_ALLOCATION_FLAGS_SHARED,
                pMemoryInfo->RefCount != 1 ? 1 : 0,
                memFlags);

            for (index = 0; index < nPages; index++)
            {
                NvU64 curIndex = curAllocCount + index - nPages;

                pLocalAllocInfo[curIndex].flags      = memFlags;
                pLocalAllocInfo[curIndex].client     = curClientCount - 1;
                pLocalAllocInfo[curIndex].beginAddr  = (NvU64) pMemoryInfo->pMemDesc->_pteArray[index];
                pLocalAllocInfo[curIndex].size       = allocSz;

            }

            continue;

free_mem:

            portMemFree(pLocalCliInfo);
            portMemFree(pLocalAllocInfo);
            pLocalAllocInfo = NULL;
            pLocalCliInfo = NULL;

        }
    }

    if (pLocalAllocInfo != NULL)
    {

        if ((portMemExCopyToUser(pLocalAllocInfo, (NvP64)pParams->pAllocInfo,
                                 sizeof(NV2080_CTRL_CMD_FB_ALLOCATION_INFO) * curAllocCount) != NV_OK) ||
            (portMemExCopyToUser(pLocalCliInfo, (NvP64)pParams->pClientInfo,
                                 sizeof(NV2080_CTRL_CMD_FB_CLIENT_INFO) * curClientCount) != NV_OK))
        {
            status = NV_ERR_INVALID_POINTER;
        }

        portMemFree(pLocalCliInfo);
        portMemFree(pLocalAllocInfo);
    }

    pParams->clientCount = curClientCount;
    pParams->allocCount = curAllocCount;
    return status;
}
#endif // defined(DEBUG) || defined(DEVELOP) || defined(NV_VERIF_FEATURES) || defined(NV_MODS)

//
// subdeviceCtrlCmdFbGetFBRegionInfo
//
// Lock Requirements:
//      Assert that API and GPUs lock held on entry
//
NV_STATUS
subdeviceCtrlCmdFbGetFBRegionInfo_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_CMD_FB_GET_FB_REGION_INFO_PARAMS *pGFBRIParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    NvU32 regionIndex, i;
    NV_STATUS status = NV_OK;

    // Make sure the exported structure can accomodate the necessary informtion
    ct_assert(NV2080_CTRL_CMD_FB_GET_FB_REGION_INFO_MEM_TYPES >= NVOS32_NUM_MEM_TYPES);
    ct_assert(NV2080_CTRL_CMD_FB_GET_FB_REGION_INFO_MAX_ENTRIES >= MAX_FB_REGIONS);

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner());

    pGFBRIParams->numFBRegions = 0;

    //
    // Copy the internal information into the exported structure
    //
    if (pMemoryManager->Ram.numFBRegions > 0)
    {
        // Copy the region information
        for (regionIndex = 0; (regionIndex < pMemoryManager->Ram.numFBRegions) &&
            (pGFBRIParams->numFBRegions < NV2080_CTRL_CMD_FB_GET_FB_REGION_INFO_MAX_ENTRIES); regionIndex++)
        {
            // Do not expose reserved or internal-use-only regions to callers
            if ((!pMemoryManager->Ram.fbRegion[regionIndex].bRsvdRegion) && (!pMemoryManager->Ram.fbRegion[regionIndex].bInternalHeap))
            {
                pGFBRIParams->fbRegion[pGFBRIParams->numFBRegions].base          = pMemoryManager->Ram.fbRegion[regionIndex].base;
                pGFBRIParams->fbRegion[pGFBRIParams->numFBRegions].limit         = pMemoryManager->Ram.fbRegion[regionIndex].limit;
                pGFBRIParams->fbRegion[pGFBRIParams->numFBRegions].reserved      = pMemoryManager->Ram.fbRegion[regionIndex].rsvdSize;
                pGFBRIParams->fbRegion[pGFBRIParams->numFBRegions].performance   = pMemoryManager->Ram.fbRegion[regionIndex].performance;
                pGFBRIParams->fbRegion[pGFBRIParams->numFBRegions].supportCompressed = pMemoryManager->Ram.fbRegion[regionIndex].bSupportCompressed;
                pGFBRIParams->fbRegion[pGFBRIParams->numFBRegions].supportISO    = pMemoryManager->Ram.fbRegion[regionIndex].bSupportISO;
                pGFBRIParams->fbRegion[pGFBRIParams->numFBRegions].bProtected    = pMemoryManager->Ram.fbRegion[regionIndex].bProtected;

                // Init the blacklist to allow all surface types
                for(i = 0; i < NV2080_CTRL_CMD_FB_GET_FB_REGION_INFO_MEM_TYPES; i++)
                {
                    pGFBRIParams->fbRegion[pGFBRIParams->numFBRegions].blackList[i] = NV_FALSE;
                }

                // If the region does not support ISO then blacklist the ISO surface types (display, cursor, video)
                if (!pMemoryManager->Ram.fbRegion[regionIndex].bSupportISO)
                {
                    pGFBRIParams->fbRegion[pGFBRIParams->numFBRegions].blackList[NVOS32_TYPE_PRIMARY] = NV_TRUE;
                    pGFBRIParams->fbRegion[pGFBRIParams->numFBRegions].blackList[NVOS32_TYPE_CURSOR]  = NV_TRUE;
                    pGFBRIParams->fbRegion[pGFBRIParams->numFBRegions].blackList[NVOS32_TYPE_VIDEO]   = NV_TRUE;
                }

                pGFBRIParams->numFBRegions++;
            }
        }

        regionIndex = pGFBRIParams->numFBRegions;
        // Pad out the excess information
        for ( ; regionIndex < NV2080_CTRL_CMD_FB_GET_FB_REGION_INFO_MAX_ENTRIES; regionIndex++)
        {
            pGFBRIParams->fbRegion[regionIndex].base          = 0;
            pGFBRIParams->fbRegion[regionIndex].limit         = 0;
            pGFBRIParams->fbRegion[regionIndex].reserved      = 0;
            pGFBRIParams->fbRegion[regionIndex].performance   = 0;
            pGFBRIParams->fbRegion[regionIndex].supportCompressed = NV_FALSE;
            pGFBRIParams->fbRegion[regionIndex].supportISO   = NV_FALSE;
            pGFBRIParams->fbRegion[regionIndex].bProtected    = NV_FALSE;

            for (i=0; i<NV2080_CTRL_CMD_FB_GET_FB_REGION_INFO_MEM_TYPES; i++)
            {
                pGFBRIParams->fbRegion[regionIndex].blackList[i] = NV_TRUE;
            }
        }

        if (pGFBRIParams->numFBRegions > 0)
        {
            // Special region information successfully reported.
            status = NV_OK;
        }
        else
        {
            // No public region information to export.
            status = NV_ERR_NOT_SUPPORTED;
        }
    }
    else
    {
        // No special region information to export.  Operate as usual.
        status = NV_ERR_NOT_SUPPORTED;
    }

    return status;
}

NV_STATUS
subdeviceCtrlCmdGbGetSemaphoreSurfaceLayout_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_FB_GET_SEMAPHORE_SURFACE_LAYOUT_PARAMS *pParams
)
{
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(GPU_RES_GET_GPU(pSubdevice));

    pParams->caps = 0;

    if (pMemoryManager->bMonitoredFenceSupported)
        pParams->caps |= NV2080_CTRL_FB_GET_SEMAPHORE_SURFACE_LAYOUT_CAPS_MONITORED_FENCE_SUPPORTED;

    if (pMemoryManager->b64BitSemaphoresSupported)
        pParams->caps |= NV2080_CTRL_FB_GET_SEMAPHORE_SURFACE_LAYOUT_CAPS_64BIT_SEMAPHORES_SUPPORTED;

    //
    // Assume semaphore values to always be 64-bit for simplicity
    // It is not possible to pack the surface better due to alignment requirements,
    // so use the static layout here. But keep the offsets interface for flexibility in the future.
    //
    pParams->monitoredFenceThresholdOffset = 16; // payload + timestamp
    pParams->maxSubmittedSemaphoreValueOffset = 24;
    pParams->size = 32;

    return NV_OK;
}

