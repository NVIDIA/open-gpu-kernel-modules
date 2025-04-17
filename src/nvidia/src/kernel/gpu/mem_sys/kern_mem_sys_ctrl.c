/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/bus/kern_bus.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "gpu/mem_mgr/virt_mem_allocator.h"
#include "gpu/gpu_resource_desc.h"
#include "kernel/gpu/device/device.h"
#include "kernel/gpu/subdevice/subdevice.h"
#include "platform/chipset/chipset.h"
#include "ctrl/ctrl0080/ctrl0080fb.h"
#include "ctrl/ctrl2080/ctrl2080fb.h"
#include "core/locks.h"
#include "vgpu/rpc.h"
#include "rmapi/client.h"
#include "platform/sli/sli.h"

NV_STATUS
kmemsysGetFbInfos_VF(OBJGPU *pGpu, KernelMemorySystem *pKernelMemorySystem, RsClient *pClient, Device *pDevice, NvHandle hObject,
                     NV2080_CTRL_FB_GET_INFO_V2_PARAMS *pParams, NvU64 *pFbInfoListIndicesUnset)
{
    NV2080_CTRL_FB_INFO *pFbInfos = pParams->fbInfoList;
    NvU32 data = 0;
    NvU32 i = 0;

    VGPU_STATIC_INFO *pVSI = GPU_GET_STATIC_INFO(pGpu);
    NV_ASSERT_OR_RETURN(pVSI != NULL, NV_ERR_INVALID_STATE);

    FOR_EACH_INDEX_IN_MASK(64, i, *pFbInfoListIndicesUnset)
    {
        switch (pFbInfos[i].index)
        {
            case NV2080_CTRL_FB_INFO_INDEX_PARTITION_COUNT:
            {
                data = nvPopCount64(pVSI->fbioMask);
                break;
            }
            case NV2080_CTRL_FB_INFO_INDEX_PARTITION_MASK_0:
            {
                data = NvU64_LO32((NvU64)pVSI->fbioMask);
                break;
            }
            case NV2080_CTRL_FB_INFO_INDEX_PARTITION_MASK_1:
            {
                data = NvU64_HI32((NvU64)pVSI->fbioMask);
                break;
            }
            case NV2080_CTRL_FB_INFO_INDEX_FBP_MASK:
            {
                data = pVSI->fbpMask;
                break;
            }
            case NV2080_CTRL_FB_INFO_INDEX_BUS_WIDTH:
            {
                data = pVSI->fbBusWidth;
                break;
            }
            case NV2080_CTRL_FB_INFO_INDEX_FBP_COUNT:
            {
                data = nvPopCount64(pVSI->fbpMask);
                break;
            }
            case NV2080_CTRL_FB_INFO_INDEX_LTC_COUNT:
            {
                data = nvPopCount64(pVSI->ltcMask);
                break;
            }
            case NV2080_CTRL_FB_INFO_INDEX_LTS_COUNT:
            {
                data = pVSI->ltsCount;
                break;
            }
            case NV2080_CTRL_FB_INFO_INDEX_LTC_MASK_0:
            {
                data = NvU64_LO32((NvU64)pVSI->ltcMask);
                break;
            }
            case NV2080_CTRL_FB_INFO_INDEX_LTC_MASK_1:
            {
                data = NvU64_HI32((NvU64)pVSI->ltcMask);
                break;
            }
            case NV2080_CTRL_FB_INFO_INDEX_L2CACHE_SIZE:
            {
                data = pVSI->sizeL2Cache;
                break;
            }

            // Indices unknown to this function are handled downstream, so do not modify them.
            default:
            {
                continue;
            }
        }

        // save off data value
        pFbInfos[i].data = data;
        *pFbInfoListIndicesUnset &= ~NVBIT64(i);
    }
    FOR_EACH_INDEX_IN_MASK_END;

    return NV_OK;
}

// Common logic for all runtimes to populate NV2080_CTRL_FB_INFO array
static NV_STATUS
_kmemsysGetFbInfos
(
    OBJGPU *pGpu,
    RsClient *pClient,
    Device *pDevice,
    NvHandle hObject,
    NV2080_CTRL_FB_GET_INFO_V2_PARAMS *pParams
)
{
    NV2080_CTRL_FB_INFO *pFbInfos = pParams->fbInfoList;
    KernelMemorySystem *pKernelMemorySystem  = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);
    MemoryManager      *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    Heap               *pHeap = GPU_GET_HEAP(pGpu);
    Heap               *pMemoryPartitionHeap = NULL;
    KernelMIGManager   *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    NV_STATUS           status = NV_OK;
    NvU32               data = 0;
    NvU32               i = 0;
    NvBool              bIsPmaEnabled = memmgrIsPmaInitialized(pMemoryManager);
    NvBool              bIsMIGInUse = IS_MIG_IN_USE(pGpu);
    NvU64               bytesTotal;
    NvU64               bytesFree;
    NvU64               heapBase;
    NvU64               largestOffset;
    NvU64               largestFree;
    NvBool              bIsClientMIGMonitor = NV_FALSE;
    NvBool              bIsClientMIGProfiler = NV_FALSE;
    NvU64               fbInfoListIndicesUnset = 0;

    if (!RMCFG_FEATURE_PMA)
        return NV_ERR_NOT_SUPPORTED;

    if (bIsMIGInUse)
    {
        RmClient *pRmClient = dynamicCast(pClient, RmClient);
        NV_ASSERT_OR_RETURN(pRmClient != NULL, NV_ERR_INVALID_CLIENT);

        bIsClientMIGMonitor = !RMCFG_FEATURE_PLATFORM_GSP && rmclientIsCapable(pRmClient, NV_RM_CAP_SYS_SMC_MONITOR);
        bIsClientMIGProfiler = kmigmgrIsDeviceUsingDeviceProfiling(pGpu, pKernelMIGManager, pDevice);
    }

    ct_assert(NV2080_CTRL_FB_INFO_INDEX_MAX <= NV_NBITS_IN_TYPE(fbInfoListIndicesUnset));

    // Construct mask of width fbInfoListSize to track which indices have been handled
    fbInfoListIndicesUnset = NV_U64_MAX >> (NV_NBITS_IN_TYPE(fbInfoListIndicesUnset) - pParams->fbInfoListSize);

    //
    // Populate indices with runtime-specific logic
    // Return early if this computed all requested indices
    //
    if (pKernelMemorySystem != NULL)
    {
        status = kmemsysGetFbInfos_HAL(pGpu, pKernelMemorySystem, pClient, pDevice, hObject, pParams, &fbInfoListIndicesUnset);
        if (status == NV_OK && nvPopCount64(fbInfoListIndicesUnset) == 0)
            return NV_OK;
    }

    // Load the per-GPU instance heap if MIG is enabled.
    FOR_EACH_INDEX_IN_MASK(64, i, fbInfoListIndicesUnset)
    {
        switch (pFbInfos[i].index)
        {
            // The cases which aren't affected by MIG
            case NV2080_CTRL_FB_INFO_INDEX_TILE_REGION_COUNT:
            case NV2080_CTRL_FB_INFO_INDEX_TILE_REGION_FREE_COUNT:
            case NV2080_CTRL_FB_INFO_INDEX_BANK_SWIZZLE_ALIGNMENT:
            case NV2080_CTRL_FB_INFO_INDEX_BANK_COUNT:
            case NV2080_CTRL_FB_INFO_INDEX_OVERLAY_OFFSET_ADJUSTMENT:
            case NV2080_CTRL_FB_INFO_INDEX_FB_TAX_SIZE_KB:
            case NV2080_CTRL_FB_INFO_INDEX_RAM_LOCATION:
            case NV2080_CTRL_FB_INFO_INDEX_FB_IS_BROKEN:
            case NV2080_CTRL_FB_INFO_INDEX_L2CACHE_ONLY_MODE:
            case NV2080_CTRL_FB_INFO_INDEX_SMOOTHDISP_RSVD_BAR1_SIZE:
            case NV2080_CTRL_FB_INFO_INDEX_HEAP_OFFLINE_SIZE:
            case NV2080_CTRL_FB_INFO_INDEX_SUSPEND_RESUME_RSVD_SIZE:
            case NV2080_CTRL_FB_INFO_INDEX_ALLOW_PAGE_RETIREMENT:
            case NV2080_CTRL_FB_INFO_POISON_FUSE_ENABLED:
            case NV2080_CTRL_FB_INFO_FBPA_ECC_ENABLED:
            case NV2080_CTRL_FB_INFO_DYNAMIC_PAGE_OFFLINING_ENABLED:
            case NV2080_CTRL_FB_INFO_INDEX_FORCED_BAR1_64KB_MAPPING_ENABLED:
            case NV2080_CTRL_FB_INFO_INDEX_P2P_MAILBOX_SIZE:
            case NV2080_CTRL_FB_INFO_INDEX_P2P_MAILBOX_ALIGNMENT:
            case NV2080_CTRL_FB_INFO_INDEX_P2P_MAILBOX_BAR1_MAX_OFFSET_64KB:
                continue;

            case NV2080_CTRL_FB_INFO_INDEX_TOTAL_RAM_SIZE:
            case NV2080_CTRL_FB_INFO_INDEX_RAM_SIZE:
            {
                //
                // If MIG is enabled and device profiling/monitoring
                // is not in use we check for GPU instance subscription
                // and provide GPU instance local info. Unsubscribed + unprivileged
                // clients may still query global info for the above list of
                // indices.
                //
                if (bIsMIGInUse &&
                    !bIsClientMIGProfiler && !bIsClientMIGMonitor)
                {
                    MIG_INSTANCE_REF ref;
                    status = kmigmgrGetInstanceRefFromDevice(pGpu, pKernelMIGManager,
                                                             pDevice, &ref);

                    if ((status != NV_OK) && !kmigmgrIsMIGReferenceValid(&ref))
                    {
                        status = NV_OK;
                        break;
                    }
                }

                // Fall through to the default case to get the memory partition heap
            }
            default:
            {
                //
                // If MIG is enabled and device profiling/monitoring
                // is not in use we check for GPU instance subscription
                // and provide GPU instance local info
                //
                if (bIsMIGInUse &&
                    !bIsClientMIGProfiler && !bIsClientMIGMonitor)
                {
                    NV_CHECK_OR_RETURN(LEVEL_INFO, (kmigmgrGetMemoryPartitionHeapFromDevice(pGpu,
                                       pKernelMIGManager, pDevice, &pMemoryPartitionHeap) == NV_OK),
                                       NV_ERR_INSUFFICIENT_PERMISSIONS);

                    //
                    // If client is associated with a GPU instance then point pHeap
                    // to client's memory partition heap
                    //
                    if (pMemoryPartitionHeap != NULL)
                        pHeap = pMemoryPartitionHeap;
                }
                break;
            }
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;

    FOR_EACH_INDEX_IN_MASK(64, i, fbInfoListIndicesUnset)
    {
        switch (pFbInfos[i].index)
        {
            case NV2080_CTRL_FB_INFO_INDEX_TILE_REGION_COUNT:
            {
                data = 0;
                break;
            }
            case NV2080_CTRL_FB_INFO_INDEX_TILE_REGION_FREE_COUNT:
            {
                // Obsolete
                data = 0;
                break;
            }
            case NV2080_CTRL_FB_INFO_INDEX_BAR1_SIZE:
            {
                GETBAR1INFO bar1Info = {0};
                status = memmgrGetBAR1InfoForDevice(pGpu, pMemoryManager, pDevice, &bar1Info);
                if (status != NV_OK)
                    data = 0;
                else
                    data = bar1Info.bar1Size;

                break;
            }
            case NV2080_CTRL_FB_INFO_INDEX_BAR1_AVAIL_SIZE:
            {
                GETBAR1INFO bar1Info = {0};
                status = memmgrGetBAR1InfoForDevice(pGpu, pMemoryManager, pDevice, &bar1Info);
                if (status != NV_OK)
                    data = 0;
                else
                    data = bar1Info.bar1AvailSize;

                break;
            }
            case NV2080_CTRL_FB_INFO_INDEX_BAR1_MAX_CONTIGUOUS_AVAIL_SIZE:
            {
                GETBAR1INFO bar1Info = {0};
                status = memmgrGetBAR1InfoForDevice(pGpu, pMemoryManager, pDevice, &bar1Info);
                if (status != NV_OK)
                    data = 0;
                else
                    data = bar1Info.bar1MaxContigAvailSize;

                break;
            }
            case NV2080_CTRL_FB_INFO_INDEX_BANK_SWIZZLE_ALIGNMENT:
            {
                GETBAR1INFO bar1Info = {0};
                status = memmgrGetBAR1InfoForDevice(pGpu, pMemoryManager, pDevice, &bar1Info);
                if (status != NV_OK)
                    data = 0;
                else
                    data = bar1Info.bankSwizzleAlignment;

                break;
            }
            case NV2080_CTRL_FB_INFO_INDEX_TOTAL_RAM_SIZE:
            {
                if (pGpu->getProperty(pGpu, PDB_PROP_GPU_ZERO_FB))
                {
                    data = 0;
                    NV_PRINTF(LEVEL_INFO,
                        "[zero-FB, No local RAM] TOTAL_RAM_SIZE = 0\n");
                    break;
                }

                if (pMemoryPartitionHeap != NULL)
                {
                    NvU32 heapSizeKb;
                    if (bIsPmaEnabled)
                    {
                        pmaGetTotalMemory(&pHeap->pmaObject, &bytesTotal);
                        NV_ASSERT(NvU64_HI32(bytesTotal >> 10) == 0);
                        heapSizeKb = NvU64_LO32(bytesTotal >> 10);
                    }
                    else
                    {
                        NvU64 size;

                        heapGetSize(pHeap, &size);
                        NV_ASSERT(NvU64_HI32(size >> 10) == 0);
                        heapSizeKb = NvU64_LO32(size >> 10);
                    }
                    data = heapSizeKb;
                    break;
                }
                else
                {
                    NV_ASSERT(0 == NvU64_HI32(pMemoryManager->Ram.fbTotalMemSizeMb << 10));
                    data = NvU64_LO32(NV_MIN((pMemoryManager->Ram.fbTotalMemSizeMb << 10), 
                                             (pMemoryManager->Ram.fbOverrideSizeMb << 10))
                                             - pKernelMemorySystem->fbOverrideStartKb);
                    break;
                }
            }
            case NV2080_CTRL_FB_INFO_INDEX_RAM_SIZE:
            {
                if (pGpu->getProperty(pGpu, PDB_PROP_GPU_ZERO_FB))
                {
                    data = 0;
                    NV_PRINTF(LEVEL_INFO,
                        "[zero-FB, No local RAM] RAM_SIZE = 0\n");
                    break;
                }

                if (pMemoryPartitionHeap != NULL)
                {
                    NvU32 heapSizeKb;
                    if (bIsPmaEnabled)
                    {
                        pmaGetTotalMemory(&pHeap->pmaObject, &bytesTotal);
                        NV_ASSERT(NvU64_HI32(bytesTotal >> 10) == 0);
                        heapSizeKb = NvU64_LO32(bytesTotal >> 10);
                    }
                    else
                    {
                        NvU64 size;

                        heapGetSize(pHeap, &size);
                        NV_ASSERT(NvU64_HI32(size >> 10) == 0);
                        heapSizeKb = NvU64_LO32(size >> 10);
                    }
                    data = heapSizeKb;
                    break;
                }
                NV_ASSERT(0 == NvU64_HI32(pMemoryManager->Ram.fbTotalMemSizeMb << 10));
                data = NvU64_LO32(NV_MIN((pMemoryManager->Ram.fbTotalMemSizeMb << 10),
                                         (pMemoryManager->Ram.fbOverrideSizeMb << 10))
                                         - pKernelMemorySystem->fbOverrideStartKb);
                break;
            }
            case NV2080_CTRL_FB_INFO_INDEX_USABLE_RAM_SIZE:
            {
                if (pGpu->getProperty(pGpu, PDB_PROP_GPU_ZERO_FB))
                {
                    data = 0;
                    NV_PRINTF(LEVEL_INFO,
                        "[zero-FB, No local RAM] USABLE_RAM_SIZE = 0\n");
                    break;
                }

                if (pMemoryPartitionHeap != NULL)
                {
                    NvU32 heapSizeKb;
                    if (bIsPmaEnabled)
                    {
                        pmaGetTotalMemory(&pHeap->pmaObject, &bytesTotal);
                        NV_ASSERT(NvU64_HI32(bytesTotal >> 10) == 0);
                        heapSizeKb = NvU64_LO32(bytesTotal >> 10);
                    }
                    else
                    {
                        NvU64 size;

                        heapGetSize(pHeap, &size);
                        NV_ASSERT(NvU64_HI32(size >> 10) == 0);
                        heapSizeKb = NvU64_LO32(size >> 10);
                    }
                    data = heapSizeKb;
                    break;
                }
                NV_ASSERT(0 == NvU64_HI32(pMemoryManager->Ram.fbUsableMemSize >> 10));
                data = NvU64_LO32(NV_MIN((pMemoryManager->Ram.fbUsableMemSize >> 10 ),
                                         (pMemoryManager->Ram.fbOverrideSizeMb << 10))
                                         - pKernelMemorySystem->fbOverrideStartKb);
                break;
            }
            case NV2080_CTRL_FB_INFO_INDEX_HEAP_SIZE:
            {
                if (bIsPmaEnabled)
                {
                    pmaGetTotalMemory(&pHeap->pmaObject, &bytesTotal);
                    NV_ASSERT(NvU64_HI32(bytesTotal >> 10) == 0);
                    data = NvU64_LO32(bytesTotal >> 10);
                }
                else
                {
                    NvU64 size;

                    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_ZERO_FB))
                    {
                        data = 0;
                        NV_PRINTF(LEVEL_INFO,
                            "[zero-FB, No local RAM HEAP] HEAP_SIZE = 0\n");
                        break;
                    }

                    heapGetUsableSize(pHeap, &size);
                    NV_ASSERT(NvU64_HI32(size >> 10) == 0);
                    data = NvU64_LO32(size >> 10);
                }
                data -= pKernelMemorySystem->fbOverrideStartKb;
                break;
            }
            case NV2080_CTRL_FB_INFO_INDEX_HEAP_START:
            {
                if (pMemoryPartitionHeap != NULL)
                {
                    if (bIsPmaEnabled)
                    {
                        pmaGetLargestFree(&pHeap->pmaObject, &largestFree, &heapBase, &largestOffset);
                    }
                    else
                    {
                        status = heapInfo(pHeap, &bytesFree, &bytesTotal, &heapBase,
                                          &largestOffset, &largestFree);
                    }
                    data = NvU64_LO32(heapBase >> 10);
                }
                else
                {
                    status = memmgrCalculateHeapOffsetWithGSP_HAL(pGpu, pMemoryManager, &data);
                    if (status == NV_ERR_NOT_SUPPORTED)
                    {
                        // If GSP is not in use, if neither Console nor CBC
                        // region are present, or if this calculation is done
                        // in hardware, then heapGetBase() will give the correct
                        // value and it just needs to be converted to kbytes.
                        // It will be zero unless VGA display memory is reserved
                        if (pKernelMemorySystem->fbOverrideStartKb != 0)
                        {
                            status = NV_OK;
                            data = NvU64_LO32(pKernelMemorySystem->fbOverrideStartKb);
                            NV_ASSERT_OR_ELSE((NvU64) data == pKernelMemorySystem->fbOverrideStartKb,
                                              status = NV_ERR_INVALID_DATA);
                            
                        }
                        else
                        {
                            //
                            // Returns start of heap in kbytes. This is zero unless
                            // VGA display memory is reserved.
                            //
                            status = heapGetBase(pHeap, &heapBase);
                            data = NvU64_LO32(heapBase >> 10);
                            NV_ASSERT_OR_ELSE(((NvU64) data << 10ULL) == heapBase,
                                              status = NV_ERR_INVALID_DATA);
                        }
                    }
                }
                break;
            }
            case NV2080_CTRL_FB_INFO_INDEX_HEAP_FREE:
            {
                if (bIsClientMIGMonitor || bIsClientMIGProfiler)
                {
                    NvU64 val = 0;
                    bytesFree = 0;

                    if (bIsPmaEnabled)
                        pmaGetFreeMemory(&pHeap->pmaObject, &val);
                    else
                        heapGetFree(pHeap, &val);

                    bytesFree = val;

                    //
                    // Add free memory across the all valid MIG GPU instances and
                    // the global heap.
                    //
                    // As MIG uses the global heap when memory is not
                    // partitioned, skip getting information from it.
                    //
                    if (kmigmgrIsMIGMemPartitioningEnabled(pGpu, pKernelMIGManager))
                    {
                        NvU64 partTotalBytesFree = 0;
                        NvU64 partTotalBytes = 0;
                        NvU32 config = PMA_QUERY_NUMA_ENABLED;

                        memmgrGetFreeMemoryForAllMIGGPUInstances(pGpu, pMemoryManager, &partTotalBytesFree);
                        
                        //
                        // In the case of MIG+NUMA case(self hosted GPUs), NVOS32_ALLOC_FLAGS_FIXED_ADDRESS_ALLOCATE
                        // is not supported and hence the partition's memory is not accounted in the global PMA.
                        // This resulted in more free memory than the total memory resulting in the
                        // used memory(calculated as total - free) showing very large value.
                        // Now calculating the global free memory in the NUMA case as:
                        // partitions' free memory + (global total memory - all created partitions' total memory).
                        //
                        if (bIsPmaEnabled &&
                            (pmaQueryConfigs(&pHeap->pmaObject, &config) == NV_OK) &&
                            (config & PMA_QUERY_NUMA_ENABLED))
                        {
                            memmgrGetTotalMemoryForAllMIGGPUInstances(pGpu, pMemoryManager, &partTotalBytes);
                            pmaGetTotalMemory(&pHeap->pmaObject, &val);
                            bytesFree = partTotalBytesFree + (val - partTotalBytes);
                        }
                        else
                        {
                            bytesFree += partTotalBytesFree;
                        }
                    }

                    NV_ASSERT(NvU64_HI32(bytesFree >> 10) == 0);
                    data = NvU64_LO32(bytesFree >> 10);
                }
                else if (bIsPmaEnabled)
                {
                    pmaGetFreeMemory(&pHeap->pmaObject, &bytesFree);

                    NV_ASSERT(NvU64_HI32(bytesFree >> 10) == 0);
                    data = NvU64_LO32(bytesFree >> 10);
                }
                else
                {
                    NvU64 size;

                    heapGetFree(pHeap, &size);
                    NV_ASSERT(NvU64_HI32(size >> 10) == 0);
                    data = NvU64_LO32(size >> 10);
                }
                break;
            }

            case NV2080_CTRL_FB_INFO_INDEX_VISTA_RESERVED_HEAP_SIZE:
            {
                //
                // If PMA is enabled, ideally we wouldn't have any reserved heap but UVM vidheapctrl
                // allocations are not accounted for by KMD. RM will reserve it.
                //
                if (bIsPmaEnabled)
                {
                    if (pMemoryPartitionHeap != NULL)
                    {
                        data = 0;
                        break;
                    }

                    NvU64 uvmReserveMem = 0;
                    memmgrCalcReservedFbSpaceForUVM_HAL(pGpu, pMemoryManager, &uvmReserveMem);
                    // Ceil bytes and return the # of KB
                    data = NvU64_LO32(NV_CEIL(uvmReserveMem, 1024));
                }
                else
                {
                    // heap size in kbytes
                    data = memmgrGetReservedHeapSizeMb_HAL(pGpu, pMemoryManager) << 10;
                }
                break;
            }

            case NV2080_CTRL_FB_INFO_INDEX_MAPPABLE_HEAP_SIZE:
            {
                if (bIsPmaEnabled)
                {
                    NvU32 heapSizeKb;

                    pmaGetTotalMemory(&pHeap->pmaObject, &bytesTotal);
                    NV_ASSERT(NvU64_HI32(bytesTotal >> 10) == 0);
                    heapSizeKb = NvU64_LO32(bytesTotal >> 10);

                    data = memmgrGetMappableRamSizeMb(pMemoryManager) << 10;
                    if (data > heapSizeKb)
                        data = heapSizeKb;
                }
                else
                {
                    NvU64 size;
                    NvU32 heapSizeKb;

                    heapGetSize(pHeap, &size);
                    NV_ASSERT(NvU64_HI32(size >> 10) == 0);
                    heapSizeKb = NvU64_LO32(size >> 10);

                    data = memmgrGetMappableRamSizeMb(pMemoryManager) << 10;
                    if (data > heapSizeKb)
                        data = heapSizeKb;
                }
                data -= pKernelMemorySystem->fbOverrideStartKb;
                break;
            }
            case NV2080_CTRL_FB_INFO_INDEX_BANK_COUNT:
            {
                data = 1;
                break;
            }
            case NV2080_CTRL_FB_INFO_INDEX_OVERLAY_OFFSET_ADJUSTMENT:
            {
                data = 0;
                break;
            }
            case NV2080_CTRL_FB_INFO_INDEX_FB_TAX_SIZE_KB:
            {
                bytesTotal = memmgrGetFbTaxSize_HAL(pGpu, pMemoryManager);
                data = NvU64_LO32(bytesTotal >> 10);
                break;
            }
            case NV2080_CTRL_FB_INFO_INDEX_HEAP_BASE_KB:
            {
                if (bIsPmaEnabled)
                {
                    pmaGetLargestFree(&pHeap->pmaObject, &largestFree, &heapBase, &largestOffset);
                }
                else
                {
                    status = heapInfo(pHeap, &bytesFree, &bytesTotal, &heapBase,
                                      &largestOffset, &largestFree);
                }

                data = NvU64_LO32(heapBase >> 10);
                break;
            }
            case NV2080_CTRL_FB_INFO_INDEX_LARGEST_FREE_REGION_SIZE_KB:
            {
                if (bIsPmaEnabled)
                {
                    pmaGetLargestFree(&pHeap->pmaObject, &largestFree, &heapBase, &largestOffset);
                }
                else
                {
                    status = heapInfo(pHeap, &bytesFree, &bytesTotal, &heapBase,
                                      &largestOffset, &largestFree);
                }

                data = NvU64_LO32(largestFree >> 10);
                break;
            }
            case NV2080_CTRL_FB_INFO_INDEX_LARGEST_FREE_REGION_BASE_KB:
            {
                if (bIsPmaEnabled)
                {
                    pmaGetLargestFree(&pHeap->pmaObject, &largestFree, &heapBase, &largestOffset);
                }
                else
                {
                    status = heapInfo(pHeap, &bytesFree, &bytesTotal, &heapBase,
                                      &largestOffset, &largestFree);
                }

                data = NvU64_LO32(largestOffset >> 10);
                break;
            }
            case NV2080_CTRL_FB_INFO_INDEX_RAM_LOCATION:
            {
                data = NV2080_CTRL_FB_INFO_RAM_LOCATION_GPU_DEDICATED;
                break;
            }
            case NV2080_CTRL_FB_INFO_INDEX_FB_IS_BROKEN:
            {
                data = (pGpu->getProperty(pGpu, PDB_PROP_GPU_BROKEN_FB) &&
                        !pGpu->getProperty(pGpu, PDB_PROP_GPU_ZERO_FB)) ? 1 : 0;
                break;
            }
            case NV2080_CTRL_FB_INFO_INDEX_L2CACHE_ONLY_MODE:
            {
                data = gpuIsCacheOnlyModeEnabled(pGpu) ? 1 : 0;
                break;
            }

            case NV2080_CTRL_FB_INFO_INDEX_SMOOTHDISP_RSVD_BAR1_SIZE:
            {
                data = pGpu->uefiScanoutSurfaceSizeInMB;
                break;
            }

            case NV2080_CTRL_FB_INFO_INDEX_HEAP_OFFLINE_SIZE:
            {
                data = pHeap->dynamicBlacklistSize;
                break;
            }
            case NV2080_CTRL_FB_INFO_INDEX_1TO1_COMPTAG_ENABLED:
            {
                const MEMORY_SYSTEM_STATIC_CONFIG *pMemorySystemConfig = kmemsysGetStaticConfig(pGpu, pKernelMemorySystem);
                data = (pMemorySystemConfig->bOneToOneComptagLineAllocation) ? 1 : 0;
                break;
            }

            case NV2080_CTRL_FB_INFO_INDEX_SUSPEND_RESUME_RSVD_SIZE:
            {
                NvU64 rsvdSize = memmgrGetRsvdSizeForSr_HAL(pGpu, pMemoryManager);
                NV_ASSERT(NvU64_HI32(rsvdSize) == 0);
                data = NvU64_LO32(rsvdSize);
                break;
            }
            case NV2080_CTRL_FB_INFO_INDEX_ALLOW_PAGE_RETIREMENT:
            {
                data = pGpu->getProperty(pGpu, PDB_PROP_GPU_ALLOW_PAGE_RETIREMENT) ? 1 : 0;
                break;
            }
            case NV2080_CTRL_FB_INFO_POISON_FUSE_ENABLED:
            {
                data = gpuIsGlobalPoisonFuseEnabled(pGpu) ? 1 : 0;
                break;
            }
            case NV2080_CTRL_FB_INFO_FBPA_ECC_ENABLED:
            {
                const MEMORY_SYSTEM_STATIC_CONFIG *pMemorySystemConfig =
                    kmemsysGetStaticConfig(pGpu, GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu));
                data = (pMemorySystemConfig->bEnabledEccFBPA) ? 1: 0;
                break;
            }
            case NV2080_CTRL_FB_INFO_DYNAMIC_PAGE_OFFLINING_ENABLED:
            {
                data = pMemoryManager->bEnableDynamicPageOfflining ? 1 : 0;
                break;
            }
            case NV2080_CTRL_FB_INFO_INDEX_FORCED_BAR1_64KB_MAPPING_ENABLED:
            {
                KernelBus *pKernelBus = GPU_GET_KERNEL_BUS(pGpu);
                data = kbusIsBar1Force64KBMappingEnabled(pKernelBus);
                break;
            }
            case NV2080_CTRL_FB_INFO_INDEX_P2P_MAILBOX_SIZE:
            {
                KernelBus *pKernelBus = GPU_GET_KERNEL_BUS(pGpu);
                data = 0;
                if (kbusIsP2pMailboxClientAllocated(pKernelBus))
                    kbusGetP2PMailboxAttributes_HAL(pGpu, pKernelBus, &data, NULL, NULL);
                break;
            }
            case NV2080_CTRL_FB_INFO_INDEX_P2P_MAILBOX_ALIGNMENT:
            {
                KernelBus *pKernelBus = GPU_GET_KERNEL_BUS(pGpu);
                data = 0;
                if (kbusIsP2pMailboxClientAllocated(pKernelBus))
                    kbusGetP2PMailboxAttributes_HAL(pGpu, pKernelBus, NULL, &data, NULL);
                break;
            }
            case NV2080_CTRL_FB_INFO_INDEX_P2P_MAILBOX_BAR1_MAX_OFFSET_64KB:
            {
                KernelBus *pKernelBus = GPU_GET_KERNEL_BUS(pGpu);
                data = 0;
                if (kbusIsP2pMailboxClientAllocated(pKernelBus))
                    kbusGetP2PMailboxAttributes_HAL(pGpu, pKernelBus, NULL, NULL, &data);
                break;
            }
            case NV2080_CTRL_FB_INFO_INDEX_PROTECTED_MEM_SIZE_TOTAL_KB:
            {
                if (gpuIsCCorApmFeatureEnabled(pGpu))
                {
                    if (bIsPmaEnabled)
                    {
                        pmaGetTotalProtectedMemory(&pHeap->pmaObject, &bytesTotal);
                        NV_ASSERT(NvU64_HI32(bytesTotal >> 10) == 0);
                        data = NvU64_LO32(bytesTotal >> 10);
                    }
                    else
                    {
                        //
                        // We should never reach here given that PMA is enabled
                        // on all platforms where confidential compute is supported
                        //
                        NV_ASSERT(0);
                        data = 0;
                        status = NV_ERR_INVALID_ARGUMENT;
                    }
                }
                else
                {
                    data = 0;
                }
                break;
            }
            case NV2080_CTRL_FB_INFO_INDEX_PROTECTED_MEM_SIZE_FREE_KB:
            {
                if (gpuIsCCorApmFeatureEnabled(pGpu))
                {
                    if (bIsPmaEnabled)
                    {
                        pmaGetFreeProtectedMemory(&pHeap->pmaObject, &bytesFree);
                        NV_ASSERT(NvU64_HI32(bytesFree >> 10) == 0);
                        data = NvU64_LO32(bytesFree >> 10);
                    }
                    else
                    {
                        //
                        // We should never reach here given that PMA is enabled
                        // on all platforms where confidential compute is supported
                        //
                        NV_ASSERT(0);
                        data = 0;
                        status = NV_ERR_INVALID_ARGUMENT;
                    }
                }
                else
                {
                    data = 0;
                }
                break;
            }
            case NV2080_CTRL_FB_INFO_INDEX_IS_ZERO_FB:
            {
                if (pGpu->getProperty(pGpu, PDB_PROP_GPU_ZERO_FB))
                {
                    data = 1;
                }
                else
                {
                    data = 0;
                }
                break;
            }

            default:
            {
                if (status != NV_OK)
                    break;

                continue;
            }
        }

        if (status != NV_OK)
            break;

        // save off data value
        pFbInfos[i].data = data;
        fbInfoListIndicesUnset &= ~NVBIT64(i);
    }
    FOR_EACH_INDEX_IN_MASK_END;

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, status);

    // RPC to host to populate indices that could not be yet computed
    if ((IS_VIRTUAL(pGpu) || IS_GSP_CLIENT(pGpu)) &&
        (nvPopCount64(fbInfoListIndicesUnset) > 0))
    {
        NvU32 physIdx = 0;
        NV2080_CTRL_FB_GET_INFO_V2_PARAMS *pRpcParams =
            portMemAllocNonPaged(sizeof(NV2080_CTRL_FB_GET_INFO_V2_PARAMS));

        portMemSet(pRpcParams, 0, sizeof(*pRpcParams));
        FOR_EACH_INDEX_IN_MASK(64, i, fbInfoListIndicesUnset)
        {
            pRpcParams->fbInfoList[physIdx++].index = pFbInfos[i].index;
        }
        FOR_EACH_INDEX_IN_MASK_END;

        pRpcParams->fbInfoListSize = physIdx;

        NV_RM_RPC_CONTROL(pGpu,
                          pClient->hClient,
                          hObject,
                          NV2080_CTRL_CMD_FB_GET_INFO_V2,
                          pRpcParams,
                          sizeof(*pRpcParams),
                          status);

        physIdx = 0;
        FOR_EACH_INDEX_IN_MASK(64, i, fbInfoListIndicesUnset)
        {
            NV_ASSERT(pFbInfos[i].index == pRpcParams->fbInfoList[physIdx].index);

            pFbInfos[i].data = pRpcParams->fbInfoList[physIdx].data;

            //
            // Assume that the RPC could handle unknown indices. Actual errors will be signaled by
            // returning status.
            //
            fbInfoListIndicesUnset &= ~NVBIT64(i);

            physIdx++;
        }
        FOR_EACH_INDEX_IN_MASK_END;

        portMemFree(pRpcParams);
    }

    NV_CHECK(LEVEL_INFO, nvPopCount64(fbInfoListIndicesUnset) == 0);

    return status;
}

//
// subdeviceCtrlCmdFbGetInfoV2
//
// Lock Requirements:
//      Assert that API and Gpus lock held on entry
//
NV_STATUS
subdeviceCtrlCmdFbGetInfoV2_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_FB_GET_INFO_V2_PARAMS *pFbInfoParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    RsClient *pClient = RES_GET_CLIENT(pSubdevice);
    NvHandle hObject = RES_GET_HANDLE(pSubdevice);
    Device *pDevice = GPU_RES_GET_DEVICE(pSubdevice);

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmDeviceGpuLockIsOwner(pGpu->gpuInstance),
        NV_ERR_INVALID_LOCK_STATE);

    if ((pFbInfoParams->fbInfoListSize > NV2080_CTRL_FB_INFO_MAX_LIST_SIZE) ||
        (pFbInfoParams->fbInfoListSize == 0))
    {
       return NV_ERR_INVALID_ARGUMENT;
    }

    return _kmemsysGetFbInfos(pGpu, pClient, pDevice, hObject, pFbInfoParams);
}

//
// subdeviceCtrlCmdFbGetCliManagedOfflinedPages
//
// Lock Requirements:
//      Assert that API and Gpus lock held on entry
//

NV_STATUS
subdeviceCtrlCmdFbGetCliManagedOfflinedPages_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_FB_GET_CLI_MANAGED_OFFLINED_PAGES_PARAMS *pOsOfflinedParams
)
{
    OBJGPU             *pGpu                = GPU_RES_GET_GPU(pSubdevice);
    Heap               *pHeap               = GPU_GET_HEAP(pGpu);
    MemoryManager      *pMemoryManager      = GPU_GET_MEMORY_MANAGER(pGpu);
    BLACKLIST_CHUNK    *pBlacklistChunks    = pHeap->blackList.pBlacklistChunks;
    NvU64               pageAddress;
    NvU32               i;
    NvU32               osBlackListCount    = 0;
    NvU32               chunk;
    NvU64               chunks[NV2080_CTRL_FB_OFFLINED_PAGES_MAX_PAGES];
    NvU64               pageSize;
    NvU32               numChunks           = 0;

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);
    if (!IsSLIEnabled(pGpu))
    {
        if (memmgrIsPmaInitialized(pMemoryManager))
        {
            // If PMA is enabled Client pages are located here.
            pmaGetClientBlacklistedPages(&pHeap->pmaObject, chunks, &pageSize, &numChunks);

            NV_ASSERT(numChunks <= NV2080_CTRL_FB_OFFLINED_PAGES_MAX_PAGES);

            for (chunk = 0; chunk < numChunks; chunk++)
            {
                pOsOfflinedParams->offlinedPages[chunk] = (NvU32)(chunks[chunk] >> RM_PAGE_SHIFT);
            }
            pOsOfflinedParams->validEntries = numChunks;
            pOsOfflinedParams->pageSize     = (NvU32)pageSize;
        }
        else
        {
            // Iterate over the heap blacklist array to get the OS blacklisted regions
            for (i = 0; i < pHeap->blackList.count; i++)
            {
                // Extract only the globally excluded page offsets
                if (!pBlacklistChunks[i].bIsValid)
                {
                    pageAddress = pBlacklistChunks[i].physOffset;
                    pOsOfflinedParams->offlinedPages[osBlackListCount] = (NvU32) (pageAddress >> RM_PAGE_SHIFT);
                    osBlackListCount++;
                }
            }
            pOsOfflinedParams->validEntries = osBlackListCount;
            pOsOfflinedParams->pageSize = RM_PAGE_SIZE;
        }
        return NV_OK;
    }
    else
        return NV_ERR_NOT_SUPPORTED;
}

/*!
 * @brief This call can be used to update the NUMA status.
 *
 * Lock Requirements:
 *      Assert that API and GPUs lock held on entry
 *
 * @param[in] pSubdevice Subdevice
 * @param[in] pParams    pointer to control parameters
 *
 * @return NV_OK When successful
 *         NV_ERR_INVALID_STATE Otherwise
 */
NV_STATUS
subdeviceCtrlCmdFbUpdateNumaStatus_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_FB_UPDATE_NUMA_STATUS_PARAMS *pParams
)
{
    OBJGPU             *pGpu                = GPU_RES_GET_GPU(pSubdevice);
    KernelMemorySystem *pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);
    Heap               *pHeap               = GPU_GET_HEAP(pGpu);
    PMA                *pPma                = &pHeap->pmaObject;
    NV_STATUS          status               = NV_OK;

    if (!RMCFG_FEATURE_PMA)
        return NV_ERR_NOT_SUPPORTED;

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);

    if (pParams->bOnline)
    {
        status = pmaNumaOnlined(pPma, pGpu->numaNodeId,
                                pKernelMemorySystem->coherentCpuFbBase,
                                pKernelMemorySystem->numaOnlineSize);
    }
    else
    {
        pmaNumaOfflined(pPma);
    }

    return status;
}

/*
 * @brief This call can be used to get NUMA related information.
 *
 * Lock Requirements:
 *      Assert that API and GPUs lock held on entry
 *
 * @param[in] pSubdevice Subdevice
 * @param[in] pParams    pointer to control parameters
 *
 * @return NV_OK When successful
 *         NV_ERR_INVALID_STATE Otherwise
 */
NV_STATUS
subdeviceCtrlCmdFbGetNumaInfo_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_FB_GET_NUMA_INFO_PARAMS *pParams
)
{
    OBJGPU              *pGpu                 = GPU_RES_GET_GPU(pSubdevice);
    KernelMemorySystem  *pKernelMemorySystem  = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);
    MemoryManager       *pMemoryManager       = GPU_GET_MEMORY_MANAGER(pGpu);
    BLACKLIST_ADDRESS   *pBlAddrs;
    NvU32                numaOfflineIdx       = 0;
    NvU32                idx;
    NV_STATUS            status;
    NvU32                count;
    NvU64                numaMemOffset = 0;
    NvU64                numaMemSize = 0;

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner(),
        NV_ERR_INVALID_LOCK_STATE);

    if (pParams->numaOfflineAddressesCount >
        NV_ARRAY_ELEMENTS(pParams->numaOfflineAddresses))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (gpuIsSelfHosted(pGpu) && IS_MIG_IN_USE(pGpu))
    {
        KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
        Device           *pDevice = GPU_RES_GET_DEVICE(pSubdevice);
        MIG_INSTANCE_REF  ref;
        NvU32             swizzId;

        pParams->numaNodeId = NV0000_CTRL_NO_NUMA_NODE;

        if (kmigmgrGetInstanceRefFromDevice(pGpu, pKernelMIGManager, pDevice, &ref) == NV_OK)
        {
            swizzId = ref.pKernelMIGGpuInstance->swizzId;

            if (pKernelMemorySystem->memPartitionNumaInfo[swizzId].bInUse)
            {
                pParams->numaNodeId = pKernelMemorySystem->memPartitionNumaInfo[swizzId].numaNodeId;
                pParams->numaMemAddr = pKernelMemorySystem->coherentCpuFbBase + pKernelMemorySystem->memPartitionNumaInfo[swizzId].offset;
                pParams->numaMemSize = pKernelMemorySystem->memPartitionNumaInfo[swizzId].size;
                numaMemOffset  = pKernelMemorySystem->memPartitionNumaInfo[swizzId].offset;
                numaMemSize = pKernelMemorySystem->memPartitionNumaInfo[swizzId].size;
            }
        }
    }
    else
    {
        pParams->numaNodeId = pGpu->numaNodeId;
        pParams->numaMemAddr = pKernelMemorySystem->coherentCpuFbBase + pKernelMemorySystem->numaOnlineBase;
        pParams->numaMemSize = pKernelMemorySystem->numaOnlineSize;
        numaMemOffset = pKernelMemorySystem->numaOnlineBase;
        numaMemSize = pKernelMemorySystem->numaOnlineSize;
    }

    if (pParams->numaOfflineAddressesCount == 0)
    {
        return NV_OK;
    }

    if (!(pGpu->getProperty(pGpu, PDB_PROP_GPU_ALLOW_PAGE_RETIREMENT) &&
          gpuCheckPageRetirementSupport_HAL(pGpu)))
    {
        pParams->numaOfflineAddressesCount = 0;
        return NV_OK;
    }

    count = kmemsysGetMaximumBlacklistPages(pGpu, pKernelMemorySystem);
    pBlAddrs = portMemAllocNonPaged(sizeof(BLACKLIST_ADDRESS) * count);
    if (pBlAddrs == NULL)
    {
        return NV_ERR_NO_MEMORY;
    }

    status = memmgrGetBlackListPages_HAL(pGpu, pMemoryManager, pBlAddrs, &count);
    NV_ASSERT(status != NV_ERR_BUFFER_TOO_SMALL);

    if(status == NV_OK)
    {
        for (idx = 0; idx < count; idx++)
        {
            NvU64 offset = pBlAddrs[idx].address;

            if (numaOfflineIdx >= pParams->numaOfflineAddressesCount)
            {
                status = NV_ERR_BUFFER_TOO_SMALL;
                break;
            }

            // Only tell the caller about Offline pages in the NUMA region.
            if ((offset >= numaMemOffset) && (offset < (numaMemOffset + numaMemSize)))
            {
                pParams->numaOfflineAddresses[numaOfflineIdx++] = pKernelMemorySystem->coherentCpuFbBase + offset;
            }
            else
            {
                NV_PRINTF(LEVEL_INFO, "retired page address 0x%llx not in NUMA region\n",
                          offset);
            }
        }
    }
    else
    {
        // No offlined pages or failure to read offlined addresses.
        status = NV_OK;
    }

    pParams->numaOfflineAddressesCount = numaOfflineIdx;

    portMemFree(pBlAddrs);

    return status;
}

/*!
 * @brief This call can be used to get static Bar1 related information.
 *
 * Lock Requirements:
 *      Assert that API and GPUs lock held on entry
 *
 * @param[in] pSubdevice Subdevice
 * @param[in] pParams    pointer to control parameters
 *
 * @return NV_OK When successful
 *         NV_ERR_INVALID_STATE Otherwise
 */
NV_STATUS
subdeviceCtrlCmdFbGetStaticBar1Info_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_FB_GET_STATIC_BAR1_INFO_PARAMS *pParams
)
{
    OBJGPU             *pGpu        = GPU_RES_GET_GPU(pSubdevice);
    KernelBus          *pKernelBus  = GPU_GET_KERNEL_BUS(pGpu);
    NV_STATUS           status      = NV_OK;
    NvU32               gfid;

    NV_ASSERT_OK_OR_RETURN(vgpuGetCallingContextGfid(pGpu, &gfid));

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmDeviceGpuLockIsOwner(pGpu->gpuInstance),
        NV_ERR_INVALID_LOCK_STATE);

    pParams->bStaticBar1Enabled = pKernelBus->bar1[gfid].bStaticBar1Enabled;

    if (pParams->bStaticBar1Enabled)
    {
        pParams->staticBar1StartOffset = pKernelBus->bar1[gfid].staticBar1.startOffset;
        pParams->staticBar1Size        = pKernelBus->bar1[gfid].staticBar1.size;
    }
    else
    {
        pParams->staticBar1StartOffset = 0;
        pParams->staticBar1Size = 0;
    }

    return status;
}

NV_STATUS
deviceCtrlCmdFbSetZbcReferenced_IMPL
(
    Device *pDevice,
    NV0080_CTRL_INTERNAL_MEMSYS_SET_ZBC_REFERENCED_PARAMS *pParams
)
{
    OBJGPU       *pGpu = GPU_RES_GET_GPU(pDevice);
    CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
    NvU32         gfid;
    NV_STATUS     status = NV_OK;

    NV_ASSERT_OK_OR_RETURN(vgpuGetCallingContextGfid(pGpu, &gfid));

    NV_CHECK_OR_RETURN(LEVEL_ERROR, IS_GFID_VF(gfid) || pCallContext->secInfo.privLevel >= RS_PRIV_LEVEL_KERNEL, NV_ERR_INSUFFICIENT_PERMISSIONS);

    if (IS_VIRTUAL(pGpu) || IS_GSP_CLIENT(pGpu))
    {
        RmCtrlParams *pRmCtrlParams = pCallContext->pControlParams;

        NV_RM_RPC_CONTROL(pGpu,
                          pRmCtrlParams->hClient,
                          pRmCtrlParams->hObject,
                          pRmCtrlParams->cmd,
                          pRmCtrlParams->pParams,
                          pRmCtrlParams->paramsSize,
                          status);

        return status;
    }

    return status;
}

NV_STATUS
subdeviceCtrlCmdFbSetZbcReferenced_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_INTERNAL_MEMSYS_SET_ZBC_REFERENCED_PARAMS *pParams
)
{
    OBJGPU       *pGpu = GPU_RES_GET_GPU(pSubdevice);
    CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
    NvU32         gfid;
    NV_STATUS     status = NV_OK;

    NV_ASSERT_OK_OR_RETURN(vgpuGetCallingContextGfid(pGpu, &gfid));

    NV_CHECK_OR_RETURN(LEVEL_ERROR, IS_GFID_VF(gfid) || pCallContext->secInfo.privLevel >= RS_PRIV_LEVEL_KERNEL, NV_ERR_INSUFFICIENT_PERMISSIONS);

    if (IS_VIRTUAL(pGpu) || IS_GSP_CLIENT(pGpu))
    {
        RmCtrlParams *pRmCtrlParams = pCallContext->pControlParams;

        NV_RM_RPC_CONTROL(pGpu,
                          pRmCtrlParams->hClient,
                          pRmCtrlParams->hObject,
                          pRmCtrlParams->cmd,
                          pRmCtrlParams->pParams,
                          pRmCtrlParams->paramsSize,
                          status);

        return status;
    }

    return status;
}

NV_STATUS
kmemsysFlushGpuCache_IMPL
(
    OBJGPU *pGpu,
    KernelMemorySystem *pKernelMemorySystem,
    KernelBus *pKernelBus,
    NV2080_CTRL_FB_FLUSH_GPU_CACHE_PARAMS *pCacheFlushParams
)
{
    NV_STATUS           status = NV_OK;
    FB_CACHE_MEMTYPE    memType = FB_CACHE_MEM_UNDEFINED;
    FB_CACHE_OP         cacheOp = FB_CACHE_OP_UNDEFINED;
    NvBool              bWriteback = NV_FALSE;
    NvBool              bInvalidate = NV_FALSE;

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmDeviceGpuLockIsOwner(pGpu->gpuInstance),
        NV_ERR_INVALID_LOCK_STATE);

    // Either WriteBack or Invalidate are required for Cache Ops
    if (FLD_TEST_DRF(2080, _CTRL_FB_FLUSH_GPU_CACHE_FLAGS, _WRITE_BACK,
                     _YES, pCacheFlushParams->flags))
    {
        bWriteback = NV_TRUE;
    }
    if (FLD_TEST_DRF(2080, _CTRL_FB_FLUSH_GPU_CACHE_FLAGS, _INVALIDATE,
                _YES, pCacheFlushParams->flags))
    {
        bInvalidate = NV_TRUE;
    }

    if (bWriteback || bInvalidate )
    {
        // Cache Ops Path

        switch (DRF_VAL(2080, _CTRL_FB_FLUSH_GPU_CACHE_FLAGS, _APERTURE,
                    pCacheFlushParams->flags))
        {
            case NV2080_CTRL_FB_FLUSH_GPU_CACHE_FLAGS_APERTURE_VIDEO_MEMORY:
                memType = FB_CACHE_VIDEO_MEMORY;
                break;
            case NV2080_CTRL_FB_FLUSH_GPU_CACHE_FLAGS_APERTURE_SYSTEM_MEMORY:
                memType = FB_CACHE_SYSTEM_MEMORY;
                break;
            case NV2080_CTRL_FB_FLUSH_GPU_CACHE_FLAGS_APERTURE_PEER_MEMORY:
                memType = FB_CACHE_PEER_MEMORY;
                break;
            default:
                NV_PRINTF(LEVEL_ERROR, "Invalid aperture.\n");
                return NV_ERR_INVALID_ARGUMENT;
        }

        if (bWriteback && bInvalidate)
        {
            cacheOp = FB_CACHE_EVICT;
        }
        else if (!bWriteback && bInvalidate)
        {
            cacheOp = FB_CACHE_INVALIDATE;
        }
        else if (bWriteback && !bInvalidate)
        {
            cacheOp = FB_CACHE_WRITEBACK;
        }
        else
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Must specify at least one of WRITE_BACK or INVALIDATE\n");
            return NV_ERR_INVALID_ARGUMENT;
        }

        switch (DRF_VAL(2080, _CTRL_FB_FLUSH_GPU_CACHE_FLAGS, _FLUSH_MODE,
                    pCacheFlushParams->flags))
        {
            case NV2080_CTRL_FB_FLUSH_GPU_CACHE_FLAGS_FLUSH_MODE_ADDRESS_ARRAY:
                if ((pCacheFlushParams->addressArraySize == 0) ||
                        (pCacheFlushParams->addressArraySize >
                        NV2080_CTRL_FB_FLUSH_GPU_CACHE_MAX_ADDRESSES))
                {
                    NV_PRINTF(LEVEL_ERROR, "Invalid array size (0x%x)\n",
                              pCacheFlushParams->addressArraySize);
                    status = NV_ERR_INVALID_ARGUMENT;
                    break;
                }

                if(pCacheFlushParams->memBlockSizeBytes == 0)
                {
                    NV_PRINTF(LEVEL_ERROR, "Invalid memBlock size (0x%llx)\n",
                              pCacheFlushParams->memBlockSizeBytes);
                    status = NV_ERR_INVALID_ARGUMENT;
                    break;
                }

                status = NV_ERR_GENERIC;
                break;
            case NV2080_CTRL_FB_FLUSH_GPU_CACHE_FLAGS_FLUSH_MODE_FULL_CACHE:
                status = kmemsysCacheOp_HAL(pGpu, pKernelMemorySystem, NULL, memType, cacheOp);
                break;
            default:
                NV_PRINTF(LEVEL_ERROR, "Invalid FLUSH_MODE 0x%x\n",
                          DRF_VAL(2080, _CTRL_FB_FLUSH_GPU_CACHE_FLAGS, _FLUSH_MODE, pCacheFlushParams->flags));
                return NV_ERR_INVALID_ARGUMENT;
        }
    }

    // FB Flush
    if (FLD_TEST_DRF(2080, _CTRL_FB_FLUSH_GPU_CACHE_FLAGS, _FB_FLUSH,
                     _YES, pCacheFlushParams->flags))
    {
        NV_CHECK_OK_OR_CAPTURE_FIRST_ERROR(status, LEVEL_ERROR,
                        kbusSendSysmembar(pGpu, pKernelBus));
    }

    return status;
}

/*!
 * @brief This call can be used to flush L2 followed by FB or
 * just the FB.
 *
 * If L2 ops are needed, either _INVALIDATE or _WRITE_BACK
 * or both flags set to _YES is required. Specifying both
 * to _YES implies EVICT.
 *
 * If only the FB flush is needed, only the
 * _APERTURE and _FB_FLUSH_YES are needed.
 *
 * If only L2 ops are needed (i.e. no FB flush following
 * it), _FB_FLUSH_NO is needed in addition to the other
 * L2 ops flags.
 *
 * Lock Requirements:
 *      Assert that API and GPUs lock held on entry
 *
 * @param[in] pSubdevice Subdevice
 * @param[in] pCacheFlushParams Various flush flags
 *
 */
NV_STATUS
subdeviceCtrlCmdFbFlushGpuCache_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_FB_FLUSH_GPU_CACHE_PARAMS *pCacheFlushParams
)
{
    OBJGPU             *pGpu = GPU_RES_GET_GPU(pSubdevice);
    KernelMemorySystem *pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);
    KernelBus *pKernelBus = GPU_GET_KERNEL_BUS(pGpu);
    return kmemsysFlushGpuCache(pGpu, pKernelMemorySystem, pKernelBus, pCacheFlushParams);
}

//
// subdeviceCtrlCmdFbGetGpuCacheInfo
//
// Lock Requirements:
//      Assert that API and GPUs lock held on entry
//
NV_STATUS
subdeviceCtrlCmdFbGetGpuCacheInfo_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_FB_GET_GPU_CACHE_INFO_PARAMS *pGpuCacheParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    NV_STATUS status = NV_OK;
    KernelMemorySystem *pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);
    FB_CACHE_WRITE_MODE writeMode = pKernelMemorySystem->l2WriteMode;
    FB_CACHE_BYPASS_MODE bypassMode = FB_CACHE_BYPASS_MODE_DISABLED;

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);

    switch (writeMode)
    {
        case FB_CACHE_WRITE_MODE_WRITETHROUGH:
            pGpuCacheParams->writeMode = NV2080_CTRL_FB_GET_GPU_CACHE_INFO_WRITE_MODE_WRITETHROUGH;
            break;
        case FB_CACHE_WRITE_MODE_WRITEBACK:
            pGpuCacheParams->writeMode = NV2080_CTRL_FB_GET_GPU_CACHE_INFO_WRITE_MODE_WRITEBACK;
            break;
        default:
            status = NV_ERR_INVALID_STATE;
            NV_PRINTF(LEVEL_ERROR, "Invalid write mode : %d\n", writeMode);
            break;
    }

    switch (bypassMode)
    {
        case FB_CACHE_BYPASS_MODE_DISABLED:
            pGpuCacheParams->bypassMode = NV2080_CTRL_FB_GET_GPU_CACHE_INFO_BYPASS_MODE_DISABLED;
            break;
        case FB_CACHE_BYPASS_MODE_ENABLED:
            pGpuCacheParams->bypassMode = NV2080_CTRL_FB_GET_GPU_CACHE_INFO_BYPASS_MODE_ENABLED;
            break;
        default:
            status = NV_ERR_INVALID_STATE;
            NV_PRINTF(LEVEL_ERROR, "Invalid bypass mode : %d\n",
                      bypassMode);
            break;
    }

    pGpuCacheParams->powerState = NV2080_CTRL_FB_GET_GPU_CACHE_INFO_POWER_STATE_ENABLED;
    pGpuCacheParams->rcmState = NV2080_CTRL_FB_GET_GPU_CACHE_INFO_RCM_STATE_FULL;

    return status;
}

NV_STATUS subdeviceCtrlCmdMemSysGetStaticConfig_VF
(
    Subdevice *pSubdevice,
    NV2080_CTRL_INTERNAL_MEMSYS_GET_STATIC_CONFIG_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    VGPU_STATIC_INFO *pVSI = GPU_GET_STATIC_INFO(pGpu);

    NV_ASSERT_OR_RETURN(pVSI != NULL, NV_ERR_INVALID_STATE);

    portMemCopy(pParams, sizeof(*pParams),
                &pVSI->memsysStaticConfig, sizeof(pVSI->memsysStaticConfig));

    return NV_OK;
}

NV_STATUS
subdeviceCtrlCmdFbGetDynamicOfflinedPages_VF
(
    Subdevice *pSubdevice,
    NV2080_CTRL_FB_GET_DYNAMIC_OFFLINED_PAGES_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    VGPU_STATIC_INFO *pVSI = GPU_GET_STATIC_INFO(pGpu);
    NvU32 i = pParams->baseIndex / NV2080_CTRL_FB_DYNAMIC_BLACKLIST_MAX_ENTRIES;

    NV_ASSERT_OR_RETURN(pVSI != NULL, NV_ERR_INVALID_STATE);

    if (i >= MAX_ITERATIONS_DYNAMIC_BLACKLIST)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    pParams->validEntries = pVSI->fbDynamicBlacklistedPages[i].validEntries;
    if (pParams->validEntries > NV2080_CTRL_FB_DYNAMIC_BLACKLIST_MAX_ENTRIES)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    portMemCopy(&pParams->offlined,
                pParams->validEntries * sizeof(NV2080_CTRL_FB_DYNAMIC_OFFLINED_ADDRESS_INFO),
                &pVSI->fbDynamicBlacklistedPages[i].offlined,
                pParams->validEntries * sizeof(NV2080_CTRL_FB_DYNAMIC_OFFLINED_ADDRESS_INFO));
    pParams->bMore = pVSI->fbDynamicBlacklistedPages[i].bMore;

    return NV_OK;
}

NV_STATUS
subdeviceCtrlCmdFbGetLTCInfoForFBP_VF
(
    Subdevice *pSubdevice,
    NV2080_CTRL_FB_GET_LTC_INFO_FOR_FBP_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    VGPU_STATIC_INFO *pVSI = GPU_GET_STATIC_INFO(pGpu);

    NV_ASSERT_OR_RETURN(pVSI != NULL, NV_ERR_INVALID_STATE);

    if (pParams->fbpIndex >= MAX_FBPS)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    pParams->ltcCount = pVSI->fbLtcInfoForFbp[pParams->fbpIndex].ltcCount;
    pParams->ltcMask = pVSI->fbLtcInfoForFbp[pParams->fbpIndex].ltcMask;

    return NV_OK;
}

NV_STATUS
subdeviceCtrlCmdFbGetOfflinedPages_VF
(
    Subdevice *pSubdevice,
    NV2080_CTRL_FB_GET_OFFLINED_PAGES_PARAMS *pOfflinedParams
)
{
    return NV_ERR_NOT_SUPPORTED;
}
