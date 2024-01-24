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

/******************* OS Memory Descriptor APIS *****************************\
*                                                                           *
* This contains routines to create and destroy OS memory descriptor         *
*                                                                           *
****************************************************************************/

#include <nv.h>                     // NV device driver interface
#include <os/os.h>
#include <osapi.h>
#include <nvos.h>
#include <rmapi/client.h>
#include <rmapi/rs_utils.h>
#include <gpu_mgr/gpu_mgr.h>

#include "gpu/bif/kernel_bif.h"

static NV_STATUS osCreateOsDescriptorFromPageArray(OBJGPU *, NvP64, NvHandle, NvU32, NvU64 *, MEMORY_DESCRIPTOR **, void **);
static void osDestroyOsDescriptorPageArray(PMEMORY_DESCRIPTOR);

static NV_STATUS osCreateOsDescriptorFromIoMemory(OBJGPU *, NvP64, NvHandle, NvU32, NvU64 *, MEMORY_DESCRIPTOR **, void**);
static void osDestroyOsDescriptorFromIoMemory(PMEMORY_DESCRIPTOR);

static NV_STATUS osCreateOsDescriptorFromPhysAddr(OBJGPU *, NvP64, NvHandle, NvU32, NvU64 *, MEMORY_DESCRIPTOR **, void**);
static void osDestroyOsDescriptorFromPhysAddr(PMEMORY_DESCRIPTOR);

static NV_STATUS osCreateOsDescriptorFromFileHandle(OBJGPU *, NvP64, NvHandle, NvU32, NvU64 *, MEMORY_DESCRIPTOR **, void**);
static NV_STATUS osCreateOsDescriptorFromDmaBufPtr(OBJGPU *, NvP64, NvHandle, NvU32, NvU64 *, MEMORY_DESCRIPTOR **, void**);
static void osDestroyOsDescriptorFromDmaBuf(PMEMORY_DESCRIPTOR);
static NV_STATUS osCreateOsDescriptorFromSgtPtr(OBJGPU *, NvP64, NvHandle, NvU32, NvU64 *, MEMORY_DESCRIPTOR **, void**);
static void osDestroyOsDescriptorFromSgt(PMEMORY_DESCRIPTOR);

static NV_STATUS osCheckGpuBarsOverlapAddrRange(NvRangeU64 addrRange);

NV_STATUS
osCreateMemFromOsDescriptor
(
    OBJGPU             *pGpu,
    NvP64               pDescriptor,
    NvHandle            hClient,
    NvU32               flags,
    NvU64              *pLimit,
    MEMORY_DESCRIPTOR **ppMemDesc,
    NvU32               descriptorType,
    RS_PRIV_LEVEL       privilegeLevel
)
{
    RmClient* pClient;
    NV_STATUS rmStatus;
    void *pPrivate;

    pClient = serverutilGetClientUnderLock(hClient);
    if ((pDescriptor == NvP64_NULL) ||
        (*pLimit == 0) ||
        (pClient == NULL))
    {
        return NV_ERR_INVALID_PARAM_STRUCT;
    }

    //
    // For the sake of simplicity, unmatched RM and OS page
    // sizes are not currently supported in this path, except for
    // PPC64LE and aarch64.
    //
    // Also, the nvmap handle is sent which can be any random number so
    // the virtual address alignment sanity check can't be done here.
    //
    if (!NVCPU_IS_PPC64LE &&
        !NVCPU_IS_AARCH64 &&
        (NV_RM_PAGE_SIZE != os_page_size))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    //
    // The two checks below use cached privilege because they 
    // concern the privilege level of the client, and not the
    // privilege level of the calling context which may be 
    // overridden to KERNEL at some internal callsites.
    //

    //
    // The RM cannot obtain a table of physical addresses
    // for a kernel virtual address range on all of
    // the supported UNIX platforms. Since this path is
    // not really compelling for kernel allocations on any
    // of those platforms, it is not supported.
    // For UVM, they could have pre-allocated sysmem to register
    // with RM so we put in an exception for that case.
    //
    if ((rmclientGetCachedPrivilege(pClient) >= RS_PRIV_LEVEL_KERNEL) &&
        (descriptorType != NVOS32_DESCRIPTOR_TYPE_OS_PHYS_ADDR) &&
        (descriptorType != NVOS32_DESCRIPTOR_TYPE_OS_FILE_HANDLE) &&
        (descriptorType != NVOS32_DESCRIPTOR_TYPE_OS_DMA_BUF_PTR) &&
        (descriptorType != NVOS32_DESCRIPTOR_TYPE_OS_SGT_PTR))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    //
    // NVOS32_DESCRIPTOR_TYPE_OS_DMA_BUF_PTR and
    // NVOS32_DESCRIPTOR_TYPE_OS_SGT_PTR can only be utilized by kernel space
    // rm-clients.
    //
    if ((rmclientGetCachedPrivilege(pClient) < RS_PRIV_LEVEL_KERNEL) &&
        ((descriptorType == NVOS32_DESCRIPTOR_TYPE_OS_DMA_BUF_PTR) ||
         (descriptorType == NVOS32_DESCRIPTOR_TYPE_OS_SGT_PTR)))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    switch (descriptorType)
    {
        case NVOS32_DESCRIPTOR_TYPE_VIRTUAL_ADDRESS:
            rmStatus = NV_ERR_NOT_SUPPORTED;
            break;
        case NVOS32_DESCRIPTOR_TYPE_OS_PHYS_ADDR:
            if (privilegeLevel < RS_PRIV_LEVEL_KERNEL)
            {
                rmStatus = NV_ERR_NOT_SUPPORTED;
                break;
            }
            rmStatus = osCreateOsDescriptorFromPhysAddr(pGpu, pDescriptor,
                    hClient, flags, pLimit, ppMemDesc, &pPrivate);
            break;
        case NVOS32_DESCRIPTOR_TYPE_OS_IO_MEMORY:
            rmStatus = osCreateOsDescriptorFromIoMemory(pGpu, pDescriptor,
                    hClient, flags, pLimit, ppMemDesc, &pPrivate);
            break;
        case NVOS32_DESCRIPTOR_TYPE_OS_PAGE_ARRAY:
            rmStatus = osCreateOsDescriptorFromPageArray(pGpu, pDescriptor,
                    hClient, flags, pLimit, ppMemDesc, &pPrivate);
            break;
        case NVOS32_DESCRIPTOR_TYPE_OS_FILE_HANDLE:
            rmStatus = osCreateOsDescriptorFromFileHandle(pGpu, pDescriptor,
                     hClient, flags, pLimit, ppMemDesc, &pPrivate);
            break;
        case NVOS32_DESCRIPTOR_TYPE_OS_DMA_BUF_PTR:
            rmStatus = osCreateOsDescriptorFromDmaBufPtr(pGpu, pDescriptor,
                     hClient, flags, pLimit, ppMemDesc, &pPrivate);
            break;
        case NVOS32_DESCRIPTOR_TYPE_OS_SGT_PTR:
            rmStatus = osCreateOsDescriptorFromSgtPtr(pGpu, pDescriptor,
                     hClient, flags, pLimit, ppMemDesc, &pPrivate);
            break;
        default:
            rmStatus = NV_ERR_INVALID_ARGUMENT;
            break;
    }

    return rmStatus;
}

static NV_STATUS
osCreateMemdescFromPages
(
    OBJGPU *pGpu,
    NvU64 size,
    NvU32 flags,
    NvU32 cacheType,
    MEMORY_DESCRIPTOR **ppMemDesc,
    void  *pImportPriv,
    void **ppPrivate
)
{
    NV_STATUS rmStatus;
    MEMORY_DESCRIPTOR *pMemDesc;
    NvU64 memdescFlags = MEMDESC_FLAGS_NONE;
    NvU32 gpuCachedFlags;

    if (FLD_TEST_DRF(OS02, _FLAGS, _ALLOC_NISO_DISPLAY, _YES, flags))
    {
        memdescFlags |= MEMDESC_FLAGS_MEMORY_TYPE_DISPLAY_NISO;
    }

    rmStatus = memdescCreate(ppMemDesc, pGpu, size, 0,
                             NV_MEMORY_NONCONTIGUOUS, ADDR_SYSMEM,
                             cacheType, memdescFlags);
    if (rmStatus != NV_OK)
    {
        return rmStatus;
    }

    if (FLD_TEST_DRF(OS02, _FLAGS, _GPU_CACHEABLE, _YES, flags))
        gpuCachedFlags = NV_MEMORY_CACHED;
    else
        gpuCachedFlags = NV_MEMORY_UNCACHED;

    pMemDesc = *ppMemDesc;
    rmStatus = nv_register_user_pages(NV_GET_NV_STATE(pGpu),
            NV_RM_PAGES_TO_OS_PAGES(pMemDesc->PageCount),
            memdescGetPteArray(pMemDesc, AT_CPU), pImportPriv,
            ppPrivate);
    if (rmStatus != NV_OK)
    {
        memdescDestroy(pMemDesc);
        return rmStatus;
    }

    memdescSetGpuCacheAttrib(pMemDesc, gpuCachedFlags);
    memdescSetAddress(pMemDesc, NvP64_NULL);
    memdescSetFlag(pMemDesc, MEMDESC_FLAGS_KERNEL_MODE, NV_FALSE);
    memdescSetFlag(pMemDesc, MEMDESC_FLAGS_EXT_PAGE_ARRAY_MEM, NV_TRUE);

    //
    // If the OS layer doesn't think in RM page size, we need to inflate the
    // PTE array into RM pages.
    //
    if ((NV_RM_PAGE_SIZE < os_page_size) &&
        !memdescGetContiguity(pMemDesc, AT_CPU))
    {
        RmInflateOsToRmPageArray(memdescGetPteArray(pMemDesc, AT_CPU),
                                 pMemDesc->PageCount);
    }

    //
    // memdescMapIommu() requires the OS-private data to be set on the memory
    // descriptor, but we don't want to wire up the teardown callback just yet:
    // that callback needs to unpin the pages, but that will already be done
    // as part of failure handling further up the stack if memdescMapIommu()
    // fails. So we only set up the priv-data cleanup callback once we're sure
    // this call will succeed.
    //
    memdescSetMemData(pMemDesc, *ppPrivate, NULL);

    rmStatus = memdescMapIommu(pMemDesc, pGpu->busInfo.iovaspaceId);
    if (rmStatus != NV_OK)
    {
        if ((NV_RM_PAGE_SIZE < os_page_size) &&
            !memdescGetContiguity(pMemDesc, AT_CPU))
        {
            RmDeflateRmToOsPageArray(memdescGetPteArray(pMemDesc, AT_CPU),
                                     pMemDesc->PageCount);
        }

        nv_unregister_user_pages(NV_GET_NV_STATE(pGpu),
                                 NV_RM_PAGES_TO_OS_PAGES(pMemDesc->PageCount),
                                 NULL /* import_priv */, ppPrivate);
        memdescDestroy(pMemDesc);
        return rmStatus;
    }

    return NV_OK;
}

static NV_STATUS
osCreateOsDescriptorFromPageArray
(
    OBJGPU  *pGpu,
    NvP64    pDescriptor,
    NvHandle hClient,
    NvU32    flags,
    NvU64   *pLimit,
    MEMORY_DESCRIPTOR **ppMemDesc,
    void **ppPrivate
)
{
    NV_STATUS rmStatus;

    *ppPrivate = NvP64_VALUE(pDescriptor);

    //
    // Since the only type of memory permitted in this path
    // is anonymous user memory, certain restrictions
    // apply for the allocation flags:
    //
    //  1) anonymous memory is write-back cacheable, hence
    //     the _COHERENCY flag must match.
    //
    //  2) the RM has no control over the location of the
    //     associated pages in memory and thus cannot
    //     honor requests for contiguous memory.
    //
    // These restrictions are enforced here to avoid subtle
    // bugs later on.
    //
    if ((!FLD_TEST_DRF(OS02, _FLAGS, _COHERENCY, _CACHED, flags) &&
         !FLD_TEST_DRF(OS02, _FLAGS, _COHERENCY, _WRITE_BACK, flags)) ||
         FLD_TEST_DRF(OS02, _FLAGS, _PHYSICALITY, _CONTIGUOUS, flags))
    {
        return NV_ERR_INVALID_FLAGS;
    }

    rmStatus = osCreateMemdescFromPages(pGpu, (*pLimit + 1), flags,
                                        NV_MEMORY_CACHED, ppMemDesc,
                                        NULL /* pImportPriv */, ppPrivate);
    if (rmStatus != NV_OK)
    {
        return rmStatus;
    }

    // All is well - wire up the cleanup callback now
    memdescSetMemData(*ppMemDesc, memdescGetMemData(*ppMemDesc),
                      osDestroyOsDescriptorPageArray);

    return NV_OK;
}

/*!
 * @brief Checks if the given address range overlaps with the BARs for any of
 * the GPUs.
 */
static NV_STATUS
osCheckGpuBarsOverlapAddrRange
(
    NvRangeU64 addrRange
)
{
    NvRangeU64 gpuPhysAddrRange;
    NvRangeU64 gpuPhysFbAddrRange;
    NvRangeU64 gpuPhysInstAddrRange;
    NvU32 gpuInstance;
    OBJGPU *pGpu;
    NvU32 gpuMask;
    NV_STATUS rmStatus;

    rmStatus = gpumgrGetGpuAttachInfo(NULL, &gpuMask);
    NV_ASSERT_OR_RETURN(rmStatus == NV_OK, rmStatus);

    gpuInstance = 0;
    while ((pGpu = gpumgrGetNextGpu(gpuMask, &gpuInstance)) != NULL)
    {
        NV_INIT_RANGE(gpuPhysFbAddrRange, gpumgrGetGpuPhysFbAddr(pGpu),
            gpumgrGetGpuPhysFbAddr(pGpu) + pGpu->fbLength -1);

        NV_INIT_RANGE(gpuPhysAddrRange, pGpu->busInfo.gpuPhysAddr,
            pGpu->busInfo.gpuPhysAddr +  pGpu->deviceMappings[0].gpuNvLength -1);

        NV_INIT_RANGE(gpuPhysInstAddrRange, pGpu->busInfo.gpuPhysInstAddr,
            pGpu->busInfo.gpuPhysInstAddr + pGpu->instLength -1);

        if (NV_IS_OVERLAPPING_RANGE(gpuPhysFbAddrRange, addrRange) ||
            NV_IS_OVERLAPPING_RANGE(gpuPhysAddrRange, addrRange)   ||
            NV_IS_OVERLAPPING_RANGE(gpuPhysInstAddrRange, addrRange))
        {
            return NV_ERR_INVALID_ADDRESS;
        }
    }

    return NV_OK;
}

static NvU64
_doWarBug4040336
(
    OBJGPU *pGpu,
    NvU64 addr
)
{
    if (gpuIsWarBug4040336Enabled(pGpu))
    {
        if ((addr & 0xffffffff00000000ULL) == 0x7fff00000000ULL)
        {
            addr = addr & 0xffffffffULL;
        }
    }
    return addr;
}

static NV_STATUS
osCreateOsDescriptorFromIoMemory
(
    OBJGPU  *pGpu,
    NvP64    pDescriptor,
    NvHandle hClient,
    NvU32    flags,
    NvU64   *pLimit,
    MEMORY_DESCRIPTOR **ppMemDesc,
    void    **ppPrivate
)
{
    NV_STATUS rmStatus;
    NvU32 gpuCachedFlags;
    MEMORY_DESCRIPTOR *pMemDesc;
    NvU64 *pPteArray;
    NvRangeU64 physAddrRange;
    NvU64 *base = 0;
    NvBool bAllowMmap;

    //
    // Unlike the page array path, this one deals exclusively
    // with I/O memory, which is expected to be contiguous
    // physically, and which may only be accessed with uncached
    // transactions.
    //
    if (!FLD_TEST_DRF(OS02, _FLAGS, _COHERENCY, _UNCACHED, flags) ||
        !FLD_TEST_DRF(OS02, _FLAGS, _PHYSICALITY, _CONTIGUOUS, flags))
    {
        return NV_ERR_INVALID_FLAGS;
    }

    //
    // _PEER_MAP_OVERRIDE flag is controlled by the RM and not the client.
    //
    // RM will set the _PEER_MAP_OVERRIDE_REQUIRED flag itself for IO memory
    // memory imported with RmVidHeapControl.
    //
    if (FLD_TEST_DRF(OS02, _FLAGS, _PEER_MAP_OVERRIDE, _REQUIRED, flags))
    {
        //
        // Don't allow MMIO mappings for unprivileged users
        // This is a temporary WAR for bug 1630288 "[PeerSync] threat related
        // to GPU"
        //
        KernelBif *pKernelBif = GPU_GET_KERNEL_BIF(pGpu);
        if (!pKernelBif->peerMappingOverride && !osIsAdministrator())
        {
            NV_PRINTF(LEVEL_ERROR,
                      "%s(): permission denied, allowPeermapping=%d\n",
                      __FUNCTION__, pKernelBif->peerMappingOverride);
            return NV_ERR_INSUFFICIENT_PERMISSIONS;
        }
    }

    bAllowMmap = !FLD_TEST_DRF(OS02, _FLAGS, _MAPPING, _NEVER_MAP, flags);

    base = (void *)(NvUPtr)pDescriptor;

    //
    // There is an architectural deadlock scenario involved when full-duplex P2P
    // enabled over BAR1. See #3 in the description of bug 1571948 which explains
    // the classic deadlock. So, make sure to error out usermode's memory
    // registration if a memory range falls within any of the available GPU's
    // BAR window.
    //
    physAddrRange.min = *base;
    physAddrRange.max = *base + *pLimit;

    rmStatus = osCheckGpuBarsOverlapAddrRange(physAddrRange);
    if (rmStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "%s(): phys range 0x%016llx-0x%016llx overlaps with GPU BARs",
                  __FUNCTION__, physAddrRange.min, physAddrRange.max);
        return rmStatus;
    }

    //
    // BF3's PCIe MMIO bus address at 0x800000000000(CPU PA 0x7fff00000000) is
    // too high for Ampere to address. As a result, BF3's bus address is
    // moved to < 4GB. Now, the CPU PA and the bus address are no longer 1:1
    // and needs to be adjusted.
    //
    *base = _doWarBug4040336(pGpu, *base);

    rmStatus = memdescCreate(ppMemDesc, pGpu, (*pLimit + 1), 0,
                             NV_MEMORY_CONTIGUOUS, ADDR_SYSMEM,
                             NV_MEMORY_UNCACHED, MEMDESC_FLAGS_NONE);
    if (rmStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "%s(): error %d while attempting to create the MMIO mapping\n",
                  __FUNCTION__, rmStatus);
        return rmStatus;
    }

    pMemDesc = *ppMemDesc;

    if (FLD_TEST_DRF(OS02, _FLAGS, _GPU_CACHEABLE, _YES, flags))
        gpuCachedFlags = NV_MEMORY_CACHED;
    else
        gpuCachedFlags = NV_MEMORY_UNCACHED;

    memdescSetGpuCacheAttrib(pMemDesc, gpuCachedFlags);
    memdescSetAddress(pMemDesc, NvP64_NULL);
    memdescSetMemData(pMemDesc, NULL, NULL);
    memdescSetFlag(pMemDesc, MEMDESC_FLAGS_KERNEL_MODE, NV_FALSE);
    memdescSetFlag(pMemDesc, MEMDESC_FLAGS_PEER_IO_MEM, NV_TRUE);

    pPteArray = memdescGetPteArray(pMemDesc, AT_CPU);
    pPteArray[0] = *base;

    *ppPrivate = NULL;

    if (bAllowMmap)
    {
        rmStatus = nv_register_peer_io_mem(NV_GET_NV_STATE(pGpu), pPteArray,
                                NV_RM_PAGES_TO_OS_PAGES(pMemDesc->PageCount),
                                ppPrivate);
        if (rmStatus != NV_OK)
        {
            memdescDestroy(pMemDesc);
            return rmStatus;
        }
    }

    memdescSetMemData(pMemDesc, *ppPrivate, NULL);

    //
    // memdescMapIommu() requires the OS-private data to be set on the memory
    // descriptor, but we don't want to wire up the teardown callback just yet:
    // that callback needs to unpin the pages, but that will already be done
    // as part of failure handling further up the stack if memdescMapIommu()
    // fails. So we only set up the priv-data cleanup callback once we're sure
    // this call will succeed.
    //
    rmStatus = memdescMapIommu(pMemDesc, pGpu->busInfo.iovaspaceId);
    if (rmStatus != NV_OK)
    {
        if (*ppPrivate != NULL)
        {
            nv_unregister_peer_io_mem(NV_GET_NV_STATE(pGpu), *ppPrivate);
        }
        memdescDestroy(pMemDesc);
        return rmStatus;
    }

    // All is well - wire up the cleanup callback now
    memdescSetMemData(pMemDesc, memdescGetMemData(pMemDesc),
        osDestroyOsDescriptorFromIoMemory);

    return NV_OK;
}

static NV_STATUS
osCreateOsDescriptorFromPhysAddr
(
    OBJGPU  *pGpu,
    NvP64    pDescriptor,
    NvHandle hClient,
    NvU32    flags,
    NvU64   *pLimit,
    MEMORY_DESCRIPTOR **ppMemDesc,
    void    **ppPrivate
)
{
    NV_STATUS rmStatus;
    nv_state_t *nv = NV_GET_NV_STATE(pGpu);
    MEMORY_DESCRIPTOR *pMemDesc;
    NvU64 *pPteArray;
    NvU64  base = 0;
    NvU32  cache_type = NV_MEMORY_CACHED;
    NvU64  memdescFlags = MEMDESC_FLAGS_NONE;
    NvU64 *pPhys_addrs;
    NvU64  num_os_pages;
    NvU32  idx;

    // Currently only work with contiguous sysmem allocations
    if (!FLD_TEST_DRF(OS02, _FLAGS, _PHYSICALITY, _CONTIGUOUS, flags))
    {
        return NV_ERR_INVALID_FLAGS;
    }

    if (FLD_TEST_DRF(OS02, _FLAGS, _ALLOC_TYPE_SYNCPOINT, _APERTURE, flags))
    {
        // Syncpoint memory is uncached, DMA mapping needs to skip CPU sync.
        cache_type = NV_MEMORY_UNCACHED;

        //
        // Syncpoint memory is NISO. Don't attempt to IOMMU map if the NISO
        // IOMMU isn't enabled.
        //
        if (!NV_SOC_IS_NISO_IOMMU_PRESENT(nv))
        {
            memdescFlags |= MEMDESC_FLAGS_SKIP_IOMMU_MAPPING;
        }
    }

    if (FLD_TEST_DRF(OS02, _FLAGS, _ALLOC_NISO_DISPLAY, _YES, flags))
    {
        memdescFlags |= MEMDESC_FLAGS_MEMORY_TYPE_DISPLAY_NISO;
    }

    base = (NvU64)pDescriptor;
    rmStatus = memdescCreate(ppMemDesc, pGpu, (*pLimit + 1), 0,
                             NV_MEMORY_CONTIGUOUS, ADDR_SYSMEM,
                             cache_type, memdescFlags);
    if (rmStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "%s(): error %d while creating memdesc for kernel memory\n",
                  __FUNCTION__, rmStatus);
        return rmStatus;
    }

    pMemDesc = *ppMemDesc;

    memdescSetAddress(pMemDesc, NvP64_NULL);
    memdescSetMemData(pMemDesc, NULL, NULL);
    memdescSetFlag(pMemDesc, MEMDESC_FLAGS_EXT_PAGE_ARRAY_MEM, NV_TRUE);

    pPteArray = memdescGetPteArray(pMemDesc, AT_CPU);
    pPteArray[0] = base;

    num_os_pages = NV_RM_PAGES_TO_OS_PAGES(pMemDesc->PageCount);
    pPhys_addrs  = portMemAllocNonPaged(sizeof(NvU64) * num_os_pages);
    if (pPhys_addrs == NULL)
        goto cleanup_memdesc;

    for (idx = 0; idx < num_os_pages; idx++) 
    {
        pPhys_addrs[idx] =  base + (idx * os_page_size); 
    }

    *ppPrivate = NULL;
    rmStatus = nv_register_phys_pages(nv, pPhys_addrs, num_os_pages,
                                      memdescGetCpuCacheAttrib(pMemDesc),
                                      ppPrivate);
    if (rmStatus != NV_OK)
        goto cleanup_memdesc;

    //
    // For syncpoint memory, if IOMMU skip flag wasn't set earlier,
    // create IOVA mapping.
    //
    if (FLD_TEST_DRF(OS02, _FLAGS, _ALLOC_TYPE_SYNCPOINT, _APERTURE, flags) &&
        !memdescGetFlag(pMemDesc, MEMDESC_FLAGS_SKIP_IOMMU_MAPPING))
    {
        //
        // memdescMapIommu() requires the OS-private data to be set on the memory
        // descriptor, but we don't want to wire up the teardown callback just yet:
        // that callback needs to unpin the pages, but that will already be done
        // as part of failure handling further up the stack if memdescMapIommu()
        // fails. So we only set up the priv-data cleanup callback once we're sure
        // this call will succeed.
        //
        memdescSetMemData(pMemDesc, *ppPrivate, NULL);

        rmStatus = memdescMapIommu(pMemDesc, pGpu->busInfo.iovaspaceId);
        if (rmStatus != NV_OK)
            goto cleanup_pages;
    }

    // All is well - wire up the cleanup callback now
    memdescSetMemData(pMemDesc, *ppPrivate,
        osDestroyOsDescriptorFromPhysAddr);

    portMemFree(pPhys_addrs);

    return NV_OK;

cleanup_pages:
    if (*ppPrivate != NULL)
    {
        nv_unregister_phys_pages(NV_GET_NV_STATE(pGpu), *ppPrivate);
    }

cleanup_memdesc:
    memdescDestroy(pMemDesc);

    portMemFree(pPhys_addrs);

    return rmStatus;
}

static NV_STATUS
_createMemdescFromDmaBufSgtHelper
(
    OBJGPU  *pGpu,
    NvU32    flags,
    void *pImportPriv,
    struct sg_table *pImportSgt,
    NvU32 size,
    MEMORY_DESCRIPTOR **ppMemDesc,
    void **ppPrivate,
    MEM_DATA_RELEASE_CALL_BACK *pMemDataReleaseCallback
)
{
    NV_STATUS rmStatus = NV_OK;
    NvU32 cacheType = NV_MEMORY_UNCACHED;
    MEMORY_DESCRIPTOR *pMemDesc;
    NvU64 memdescFlags = MEMDESC_FLAGS_NONE;
    NvU32 gpuCachedFlags;

    NV_ASSERT((pMemDataReleaseCallback == osDestroyOsDescriptorFromDmaBuf) ||
              (pMemDataReleaseCallback == osDestroyOsDescriptorFromSgt));

    if (FLD_TEST_DRF(OS02, _FLAGS, _COHERENCY, _WRITE_COMBINE, flags))
    {
        cacheType = NV_MEMORY_WRITECOMBINED;
    }
    else if (!FLD_TEST_DRF(OS02, _FLAGS, _COHERENCY, _UNCACHED, flags))
    {
        cacheType = NV_MEMORY_CACHED;
    }

    if (FLD_TEST_DRF(OS02, _FLAGS, _ALLOC_NISO_DISPLAY, _YES, flags))
    {
        memdescFlags |= MEMDESC_FLAGS_MEMORY_TYPE_DISPLAY_NISO;
    }

    rmStatus = memdescCreate(ppMemDesc, pGpu, size, 0,
                             NV_MEMORY_NONCONTIGUOUS, ADDR_SYSMEM,
                             cacheType, memdescFlags);
    if (rmStatus != NV_OK)
    {
        return rmStatus;
    }

    if (FLD_TEST_DRF(OS02, _FLAGS, _GPU_CACHEABLE, _YES, flags))
    {
        gpuCachedFlags = NV_MEMORY_CACHED;
    }
    else
    {
        gpuCachedFlags = NV_MEMORY_UNCACHED;
    }

    pMemDesc = *ppMemDesc;

    memdescSetGpuCacheAttrib(pMemDesc, gpuCachedFlags);
    memdescSetAddress(pMemDesc, NvP64_NULL);
    memdescSetMemData(pMemDesc, NULL, NULL);
    memdescSetFlag(pMemDesc, MEMDESC_FLAGS_KERNEL_MODE, NV_FALSE);
    memdescSetFlag(pMemDesc, MEMDESC_FLAGS_EXT_PAGE_ARRAY_MEM, NV_TRUE);

    *ppPrivate = NULL;
    rmStatus = nv_register_sgt(NV_GET_NV_STATE(pGpu),
                               memdescGetPteArray(pMemDesc, AT_CPU),
                               NV_RM_PAGES_TO_OS_PAGES(pMemDesc->PageCount),
                               memdescGetCpuCacheAttrib(pMemDesc),
                               ppPrivate,
                               pImportSgt,
                               pImportPriv);
    if (rmStatus != NV_OK)
    {
        memdescDestroy(pMemDesc);
        return rmStatus;
    }

    //
    // If the OS layer doesn't think in RM page size, we need to inflate the
    // PTE array into RM pages.
    //
    if ((NV_RM_PAGE_SIZE < os_page_size) &&
        !memdescGetContiguity(pMemDesc, AT_CPU))
    {
        RmInflateOsToRmPageArray(memdescGetPteArray(pMemDesc, AT_CPU),
                                 pMemDesc->PageCount);
    }

    memdescSetMemData(*ppMemDesc, *ppPrivate, NULL);

    //
    // memdescMapIommu() requires the OS-private data to be set on the memory
    // descriptor, but we don't want to wire up the teardown callback just yet:
    // that callback does teardown that will already be done as part of failure
    // handling further up the stack if memdescMapIommu() fails. So we only
    // setup the priv-data cleanup callback once we're sure this call will
    // succeed.
    //
    rmStatus = memdescMapIommu(*ppMemDesc, pGpu->busInfo.iovaspaceId);
    if (rmStatus != NV_OK)
    {
        if ((NV_RM_PAGE_SIZE < os_page_size) &&
            !memdescGetContiguity(pMemDesc, AT_CPU))
        {
            RmDeflateRmToOsPageArray(memdescGetPteArray(pMemDesc, AT_CPU),
                                     pMemDesc->PageCount);
        }
        if (*ppPrivate != NULL)
        {
            nv_unregister_sgt(NV_GET_NV_STATE(pGpu), &pImportSgt,
                              (void **) &pImportPriv, *ppPrivate);
        }
        memdescDestroy(pMemDesc);
        return rmStatus;
    }

    // All is well - wire up the cleanup callback now
    memdescSetMemData(*ppMemDesc, *ppPrivate, pMemDataReleaseCallback);

    return rmStatus;
}

static NV_STATUS
_createMemdescFromDmaBuf
(
    OBJGPU  *pGpu,
    NvU32    flags,
    nv_dma_buf_t *pImportPriv,
    struct sg_table *pImportSgt,
    NvU32 size,
    MEMORY_DESCRIPTOR **ppMemDesc,
    void **ppPrivate
)
{
    NV_STATUS rmStatus =
        _createMemdescFromDmaBufSgtHelper(pGpu, flags, pImportPriv, pImportSgt,
                                          size, ppMemDesc, ppPrivate,
                                          osDestroyOsDescriptorFromDmaBuf);
    if (rmStatus != NV_OK)
    {
        nv_dma_release_dma_buf(pImportPriv);
    }

    return rmStatus;
}

static NV_STATUS
_createMemdescFromSgt
(
    OBJGPU  *pGpu,
    NvU32    flags,
    struct drm_gem_object *pImportPrivGem,
    struct sg_table *pImportSgt,
    NvU32 size,
    MEMORY_DESCRIPTOR **ppMemDesc,
    void **ppPrivate
)
{
    NV_STATUS rmStatus =
        _createMemdescFromDmaBufSgtHelper(pGpu, flags, pImportPrivGem,
                                          pImportSgt, size, ppMemDesc,
                                          ppPrivate,
                                          osDestroyOsDescriptorFromSgt);
    if (rmStatus != NV_OK)
    {
        nv_dma_release_sgt(pImportSgt, pImportPrivGem);
    }

    return rmStatus;
}

static nv_dma_device_t *GetDmaDeviceForImport
(
    nv_state_t *nv,
    NvU32 flags
)
{
    if (FLD_TEST_DRF(OS02, _FLAGS, _ALLOC_NISO_DISPLAY, _YES, flags) &&
        (nv->niso_dma_dev != NULL))
    {
        return nv->niso_dma_dev;
    }
    else
    {
        return nv->dma_dev;
    }
}

static NV_STATUS
osCreateOsDescriptorFromFileHandle
(
    OBJGPU  *pGpu,
    NvP64    pDescriptor,
    NvHandle hClient,
    NvU32    flags,
    NvU64   *pLimit,
    MEMORY_DESCRIPTOR **ppMemDesc,
    void    **ppPrivate
)
{
    NV_STATUS rmStatus = NV_OK;
    nv_state_t *nv = NV_GET_NV_STATE(pGpu);
    nv_dma_device_t *dma_dev = NULL;
    NvU32 size = 0;
    nv_dma_buf_t *pImportPriv = NULL;
    struct sg_table *pImportSgt = NULL;
    NvS32 fd;

    fd = (NvS32)((NvU64)pDescriptor);
    if ((NvU64)fd != (NvU64)pDescriptor)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "%s(): fd must fit within a signed 32-bit integer!\n",
                  __FUNCTION__);
        return NV_ERR_INVALID_ARGUMENT;
    }

    dma_dev = GetDmaDeviceForImport(nv, flags);
    rmStatus = nv_dma_import_from_fd(dma_dev, fd, &size,
                                     &pImportSgt, &pImportPriv);
    if (rmStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "%s(): Error (%d) while trying to import fd!\n",
                  __FUNCTION__, rmStatus);
        return rmStatus;
    }

    return _createMemdescFromDmaBuf(pGpu, flags, pImportPriv,
                                    pImportSgt,
                                    size, ppMemDesc, ppPrivate);
}

static NV_STATUS
osCreateOsDescriptorFromSgtPtr
(
    OBJGPU  *pGpu,
    NvP64    pDescriptor,
    NvHandle hClient,
    NvU32    flags,
    NvU64   *pLimit,
    MEMORY_DESCRIPTOR **ppMemDesc,
    void    **ppPrivate
)
{
    NV_STATUS rmStatus = NV_OK;
    nv_state_t *nv = NV_GET_NV_STATE(pGpu);
    NVOS32_DESCRIPTOR_TYPE_OS_SGT_PTR_PARAMETERS *params =
        (NVOS32_DESCRIPTOR_TYPE_OS_SGT_PTR_PARAMETERS*)((NvUPtr) pDescriptor);

    struct sg_table *sgt = params->sgt;
    struct drm_gem_object *gem = params->gem;

    rmStatus = nv_dma_import_sgt(nv->dma_dev, sgt, gem);
    if (rmStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "%s(): Error (%d) while trying to import sgt!\n",
                  __FUNCTION__, rmStatus);
        return rmStatus;
    }

    return _createMemdescFromSgt(pGpu, flags, gem, sgt,
                                 (*pLimit + 1), ppMemDesc, ppPrivate);
}

static NV_STATUS
osCreateOsDescriptorFromDmaBufPtr
(
    OBJGPU  *pGpu,
    NvP64    pDescriptor,
    NvHandle hClient,
    NvU32    flags,
    NvU64   *pLimit,
    MEMORY_DESCRIPTOR **ppMemDesc,
    void    **ppPrivate
)
{
    NV_STATUS rmStatus = NV_OK;
    nv_state_t *nv = NV_GET_NV_STATE(pGpu);
    nv_dma_device_t *dma_dev = NULL;
    NvU32 size = 0;
    nv_dma_buf_t *pImportPriv = NULL;
    struct sg_table *pImportSgt = NULL;
    void *dmaBuf = (void*)((NvUPtr)pDescriptor);

    dma_dev = GetDmaDeviceForImport(nv, flags);
    rmStatus = nv_dma_import_dma_buf(dma_dev, dmaBuf, &size,
                                     &pImportSgt, &pImportPriv);
    if (rmStatus != NV_OK)
    {
        NV_PRINTF_COND(rmStatus == NV_ERR_NOT_SUPPORTED, LEVEL_INFO, LEVEL_ERROR,
                       "Error (%d) while trying to import dma_buf!\n", rmStatus);
        return rmStatus;
    }

    return _createMemdescFromDmaBuf(pGpu, flags, pImportPriv,
                                    pImportSgt,
                                    size, ppMemDesc, ppPrivate);
}

static void
osDestroyOsDescriptorFromPhysAddr
(
    PMEMORY_DESCRIPTOR pMemDesc
)
{
    OBJGPU *pGpu  = pMemDesc->pGpu;
    void *pPrivate;

    pPrivate = memdescGetMemData(pMemDesc);
    NV_ASSERT(pPrivate != NULL);

    nv_unregister_phys_pages(NV_GET_NV_STATE(pGpu), pPrivate);
}

static void
osDestroyOsDescriptorFromIoMemory
(
    PMEMORY_DESCRIPTOR pMemDesc
)
{
    OBJGPU *pGpu  = pMemDesc->pGpu;
    void *pPrivate = memdescGetMemData(pMemDesc);

    if (pPrivate == NULL)
    {
        return;
    }

    nv_unregister_peer_io_mem(NV_GET_NV_STATE(pGpu), pPrivate);
}

static void
osDestroyOsDescriptorPageArray
(
    MEMORY_DESCRIPTOR *pMemDesc
)
{
    OBJGPU   *pGpu        = pMemDesc->pGpu;
    NvU64     osPageCount = NV_RM_PAGES_TO_OS_PAGES(pMemDesc->PageCount);
    NV_STATUS status;
    void     *pPrivate;

    pPrivate = memdescGetMemData(pMemDesc);

    NV_ASSERT(pPrivate != NULL);

    //
    // TODO: Bug 1811006: Notably skip any IOMMU mapping management as the
    // pMemDesc->pGpu might have been torn down already and the pGpu passed in
    // doesn't necessarily have IOMMU mappings. For now just allow memdescDestroy()
    // to clean up whatever is there (this may not work correctly either if any
    // of the IOMMU mappings have outlasted their VASPACEs). This should
    // be cleaned up once the fix for bug 1811006 is known.
    //

    if ((NV_RM_PAGE_SIZE < os_page_size) &&
        !memdescGetContiguity(pMemDesc, AT_CPU))
    {
        RmDeflateRmToOsPageArray(memdescGetPteArray(pMemDesc, AT_CPU),
                                 pMemDesc->PageCount);
    }

    nv_unregister_user_pages(NV_GET_NV_STATE(pGpu), osPageCount,
                             NULL /* import_priv */, &pPrivate);

    if (memdescGetFlag(pMemDesc, MEMDESC_FLAGS_FOREIGN_PAGE) == NV_FALSE)
    {
        status = os_unlock_user_pages(osPageCount, pPrivate);
        NV_ASSERT(status == NV_OK);
    }
    else
    {
        os_free_mem(pPrivate);
    }
}

static void
osDestroyOsDescriptorFromDmaBuf
(
    PMEMORY_DESCRIPTOR pMemDesc
)
{
    OBJGPU *pGpu = pMemDesc->pGpu;
    void *pPrivate = memdescGetMemData(pMemDesc);

    struct sg_table *pImportSgt;
    void *pImportPriv;

    /*
     * Unmap IOMMU now or we will get a kernel crash when it is unmapped after
     * pImportSgt is freed.
     */
    memdescUnmapIommu(pMemDesc, pGpu->busInfo.iovaspaceId);

    if ((NV_RM_PAGE_SIZE < os_page_size) &&
        !memdescGetContiguity(pMemDesc, AT_CPU))
    {
        RmDeflateRmToOsPageArray(memdescGetPteArray(pMemDesc, AT_CPU),
                                 pMemDesc->PageCount);
    }

    nv_unregister_sgt(NV_GET_NV_STATE(pGpu), &pImportSgt,
                      &pImportPriv, pPrivate);

    /*
     * pImportSgt doesn't need to be passed to nv_dma_release_dma_buf() because
     * the DMA-BUF associated with pImportPriv already has a reference to the
     * SGT.
     */

    nv_dma_release_dma_buf(pImportPriv);
}

static void
osDestroyOsDescriptorFromSgt
(
    PMEMORY_DESCRIPTOR pMemDesc
)
{
    OBJGPU *pGpu = pMemDesc->pGpu;
    void *pPrivate = memdescGetMemData(pMemDesc);

    struct sg_table *pImportSgt;
    struct drm_gem_object *pImportPrivGem;

    NV_ASSERT(pPrivate != NULL);

    /*
     * Unmap IOMMU now or we will get a kernel crash when it is unmapped after
     * pImportSgt is freed.
     */
    memdescUnmapIommu(pMemDesc, pGpu->busInfo.iovaspaceId);

    if ((NV_RM_PAGE_SIZE < os_page_size) &&
        !memdescGetContiguity(pMemDesc, AT_CPU))
    {
        RmDeflateRmToOsPageArray(memdescGetPteArray(pMemDesc, AT_CPU),
                                 pMemDesc->PageCount);
    }

    nv_unregister_sgt(NV_GET_NV_STATE(pGpu), &pImportSgt,
                      (void **) &pImportPrivGem, pPrivate);

    nv_dma_release_sgt(pImportSgt, pImportPrivGem);
}
