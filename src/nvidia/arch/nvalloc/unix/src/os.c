/*
 * SPDX-FileCopyrightText: Copyright (c) 1999-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include <nv.h>                     // NV device driver interface
#include <nv-priv.h>
#include <nv-caps.h>
#include <os/os.h>
#include <nvos.h>
#include <osapi.h>
#include <ctrl/ctrl0000/ctrl0000gpu.h>
#include <ctrl/ctrl0000/ctrl0000unix.h>

#include <nverror.h>
#include <gpu/device/device.h>

#include "gpu/gpu.h"
#include <gpu_mgr/gpu_mgr.h>
#include <platform/chipset/chipset.h>

#include "nverror.h"
#include "kernel/gpu/bif/kernel_bif.h"

#include "gpu/mig_mgr/kernel_mig_manager.h"

#include "gpu/mem_sys/kern_mem_sys.h"

#include "mem_mgr/io_vaspace.h"
#include <diagnostics/journal.h>
#include "gpu/mem_mgr/mem_desc.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "core/thread_state.h"
#include <nvacpitypes.h>
#include <platform/acpi_common.h>
#include <core/locks.h>
#include <ctrl/ctrl2080/ctrl2080gpu.h>
#include "virtualization/hypervisor/hypervisor.h"
#include "rmobjexportimport.h"
#include <nvi2c.h>
#include "rmapi/rs_utils.h"
#include "rmapi/client_resource.h"
#include "os/dce_rm_client_ipc.h"
#include "mem_mgr/mem.h"
#include "gpu/mem_mgr/virt_mem_allocator_common.h"

#include "vgpu/vgpu_util.h"

#include <acpidsmguids.h>
#include <pex.h>
#include "gps.h"
#include "jt.h"




extern const char *ppOsBugCheckBugcodeStr[];


ct_assert(NV_RM_PAGE_SIZE == RM_PAGE_SIZE);
ct_assert(NV_RM_PAGE_MASK == RM_PAGE_MASK);
ct_assert(NV_RM_PAGE_SHIFT == RM_PAGE_SHIFT);

typedef struct
{
    NvU32 euid;
    NvU32 pid;
} TOKEN_USER, *PTOKEN_USER;

struct OS_RM_CAPS
{
    NvU32 count;

    // This should be the last element
    nv_cap_t **caps;
};

NvBool osIsRaisedIRQL(void)
{
    return (!os_semaphore_may_sleep());
}

NvBool osIsISR(void)
{
    return os_is_isr();
}

NV_STATUS osGetDriverBlock
(
    OS_GPU_INFO     *pOsGpuInfo,
    OS_DRIVER_BLOCK *pBlock
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS osGetCurrentTick(NvU64 *pTimeInNs)
{
    *pTimeInNs = os_get_current_tick();
    return NV_OK;
}

NvU64 osGetTickResolution(void)
{
    return os_get_tick_resolution();
}

NV_STATUS osGetPerformanceCounter(NvU64 *pTimeInNs)
{
    *pTimeInNs = os_get_current_tick_hr();
    return NV_OK;
}

NV_STATUS osGetCurrentTime(
    NvU32 *pSeconds,
    NvU32 *pMicroSeconds
)
{
    return os_get_current_time(pSeconds, pMicroSeconds);
}

/*!
 * @brief Get timestamp for logging.
 *
 * Everything that logs a time stamp should use this routine for consistency.
 *
 * The returned value is OS dependent.  We want the time stamp to use
 * KeQueryPerformanceCounter on Windows so it matches the DirectX timestamps.
 * Linux uses microseconds since 1970 (osGetCurrentTime), since matching DirectX
 * is not a priority.
 *
 * osGetTimestampFreq returns the frequency required to decode the time stamps.
 *
 * @returns   system dependent timestamp.
 */
NvU64 osGetTimestamp(void)
{
    NvU32 sec  = 0;
    NvU32 usec = 0;
    osGetCurrentTime(&sec, &usec);
    return (NvU64)sec * 1000000 + usec;
}

/*!
 * @brief Get timestamp frequency.
 *
 * Timestamps are OS dependent.  This call returns the frequency
 * required to decode them.
 *
 * @returns   Timestamp frequency.  For example, 1000000 for MHz.
 */
NvU64 osGetTimestampFreq(void)
{
    return 1000000;
}

NV_STATUS osDelay(NvU32 milliseconds)
{
    return os_delay(milliseconds);
}

NV_STATUS osDelayUs(NvU32 microseconds)
{
    return os_delay_us(microseconds);
}

NV_STATUS osDelayNs(NvU32 nanoseconds)
{
    NvU32 microseconds = NV_MAX(1, (nanoseconds / 1000));
    return os_delay_us(microseconds);
}

NvU32 osGetCpuFrequency(void)
{
    /* convert os_get_cpu_frequency()'s return value from Hz to MHz */
    return ((NvU32)(os_get_cpu_frequency() / 1000000ULL));
}

void* osPciInitHandle(
    NvU32  Domain,
    NvU8   Bus,
    NvU8   Slot,
    NvU8   Function,
    NvU16 *pVendor,
    NvU16 *pDevice
)
{
    //
    // Check if the BDF is for a GPU that's already been attached, for which
    // we should already have a handle cached. This won't catch devices that
    // have been probed but not yet attached, but that shouldn't be a common
    // occurrence.
    //
    // More importantly, having this check here means we don't need to check
    // a global list of devices in the kernel interface layer, which could
    // have the implication of taking another lock, causing hairy lock
    // ordering issues.
    //
    if (Function == 0)
    {
        OBJGPU *pGpu = gpumgrGetGpuFromBusInfo(Domain, Bus, Slot);
        if (pGpu != NULL)
        {
            nv_state_t *nv = NV_GET_NV_STATE(pGpu);
            if (pVendor) *pVendor = nv->pci_info.vendor_id;
            if (pDevice) *pDevice = nv->pci_info.device_id;
            return nv->handle;
        }
    }

    return os_pci_init_handle(Domain, Bus, Slot, Function, pVendor, pDevice);
}

NvU8 osPciReadByte(
    void *pHandle,
    NvU32 Offset
)
{
    NvU8 val;
    os_pci_read_byte(pHandle, Offset, &val);
    return val;
}

NvU16 osPciReadWord(
    void *pHandle,
    NvU32 Offset
)
{
    NvU16 val;
    os_pci_read_word(pHandle, Offset, &val);
    return val;
}

NvU32 osPciReadDword(
    void *pHandle,
    NvU32 Offset
)
{
    NvU32 val;
    os_pci_read_dword(pHandle, Offset, &val);
    return val;
}

void osPciWriteByte(
    void *pHandle,
    NvU32 Offset,
    NvU8  Value
)
{
    os_pci_write_byte(pHandle, Offset, Value);
}

void osPciWriteWord(
    void  *pHandle,
    NvU32 Offset,
    NvU16 Value
)
{
    os_pci_write_word(pHandle, Offset, Value);
}

void osPciWriteDword(
    void  *pHandle,
    NvU32 Offset,
    NvU32 Value
)
{
    os_pci_write_dword(pHandle, Offset, Value);
}

void* osMapKernelSpace(
    RmPhysAddr Start,
    NvU64      Size,
    NvU32      Mode,
    NvU32      Protect
)
{
    NvU64 offset;
    NvU8 *ptr;

    if (0 == Size)
    {
        NV_ASSERT(Size != 0);
        return NULL;
    }

    offset = (Start & ~os_page_mask);
    Start &= os_page_mask;

    if (!portSafeAddU64(Size, offset, &Size) ||
        !portSafeAddU64(Size, ~os_page_mask, &Size))
    {
        return NULL;
    }
    Size &= os_page_mask;

    ptr = os_map_kernel_space(Start, Size, Mode);
    if (ptr != NULL)
        return (ptr + offset);

    return NULL;
}

void osUnmapKernelSpace(
    void *pAddress,
    NvU64 Size
)
{
    NvU64 offset;
    NvUPtr ptr = (NvUPtr)pAddress;

    if (0 == Size)
    {
        NV_ASSERT(Size != 0);
        return;
    }

    offset = (ptr & ~os_page_mask);
    ptr &= os_page_mask;
    Size = ((Size + offset + ~os_page_mask) & os_page_mask);
    os_unmap_kernel_space((void *)ptr, Size);
}

static NV_STATUS setNumaPrivData
(
    KernelMemorySystem      *pKernelMemorySystem,
    nv_state_t              *nv,
    MEMORY_DESCRIPTOR       *pMemDesc
)
{
    NV_STATUS   rmStatus      = NV_OK;
    void       *pAllocPrivate = NULL;
    NvU64      *addrArray     = NULL;
    NvU64       numPages      = pMemDesc->PageCount;
    NvU64       numOsPages    = numPages;
    RmPhysAddr *pteArray      = memdescGetPteArray(pMemDesc, AT_CPU);
    NvU64       i;

    addrArray = portMemAllocNonPaged(numPages * sizeof(NvU64));
    if (addrArray == NULL)
    {
        return NV_ERR_NO_MEMORY;
    }

    if (NV_RM_PAGE_SIZE < os_page_size)
    {
        numOsPages = NV_RM_PAGES_TO_OS_PAGES(numPages);
    }

    if (!memdescGetContiguity(pMemDesc, AT_CPU))
    {
        portMemCopy((void*)addrArray,
            (numPages * sizeof(NvU64)),
            (void*)pteArray,
            (numPages * sizeof(NvU64)));

        if (NV_RM_PAGE_SIZE < os_page_size)
        {
            RmDeflateRmToOsPageArray(addrArray, numPages);
        }

        for (i = 0; i < numOsPages; i++)
        {
            // Update GPA to system physical address
            addrArray[i] += pKernelMemorySystem->coherentCpuFbBase;
        }
    }
    else
    {
        //
        // Original PTE array in contiguous memdesc only holds start address.
        // We need to fill the OS page array with adjacent page addresses to
        // map contiguously.
        //
        NvU64 offset = pteArray[0] + pKernelMemorySystem->coherentCpuFbBase;

        for (i = 0; i < numOsPages; i++, offset += os_page_size)
        {
            addrArray[i] = offset;
        }
    }

    rmStatus = nv_register_phys_pages(nv, addrArray, numOsPages, NV_MEMORY_CACHED, &pAllocPrivate);
    if (rmStatus != NV_OK)
    {
        goto errors;
    }

    memdescSetMemData(pMemDesc, pAllocPrivate, NULL);

errors:
    portMemFree(addrArray);

    return rmStatus;
}

NV_STATUS osGetNumMemoryPages
(
    MEMORY_DESCRIPTOR *pMemDesc,
    NvU32 *pNumPages
)
{
    void *pAllocPrivate = NULL;

    pAllocPrivate = memdescGetMemData(pMemDesc);
    if (pAllocPrivate == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "pAllocPrivate is NULL!\n");
        return NV_ERR_INVALID_STATE;
    }

    return nv_get_num_phys_pages(pAllocPrivate, pNumPages);
}

NV_STATUS osGetMemoryPages
(
    MEMORY_DESCRIPTOR *pMemDesc,
    void *pPages,
    NvU32 *pNumPages
)
{
    void *pAllocPrivate = NULL;

    pAllocPrivate = memdescGetMemData(pMemDesc);
    if (pAllocPrivate == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "pAllocPrivate is NULL!\n");
        return NV_ERR_INVALID_STATE;
    }

    return nv_get_phys_pages(pAllocPrivate, pPages, pNumPages);
}

NV_STATUS osMapSystemMemory
(
    MEMORY_DESCRIPTOR *pMemDesc,
    NvU64 Offset,
    NvU64 Length,
    NvBool Kernel,
    NvU32 Protect,
    NvP64 *ppAddress,
    NvP64 *ppPrivate
)
{
    OBJGPU *pGpu = pMemDesc->pGpu;

    RmPhysAddr userAddress;
    nv_state_t *nv = NV_GET_NV_STATE(pGpu);
    NV_STATUS rmStatus = NV_OK;
    void *pAllocPrivate = NULL;
    void *pAddress;
    void *pPrivate = NULL;
    NvU64 pageIndex;
    NvU32 pageOffset = (Offset & ~os_page_mask);

    if (pGpu != NULL &&
        pGpu->getProperty(pGpu, PDB_PROP_GPU_COHERENT_CPU_MAPPING) &&
        memdescGetAddressSpace(pMemDesc) == ADDR_FBMEM)
    {
        KernelMemorySystem *pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);

        rmStatus = setNumaPrivData(pKernelMemorySystem, nv, pMemDesc);
        if (rmStatus != NV_OK)
            return rmStatus;

        pageOffset = memdescGetPhysAddr(pMemDesc, FORCE_VMMU_TRANSLATION(pMemDesc, AT_GPU), Offset) & ~os_page_mask;
    }

    *ppAddress = NvP64_NULL;
    *ppPrivate = NvP64_NULL;

    if ((Offset + Length) < Length)
        return NV_ERR_INVALID_ARGUMENT;
    if ((Offset + Length) > pMemDesc->Size)
        return NV_ERR_INVALID_ARGUMENT;

    pageIndex = (Offset >> os_page_shift);

    pAllocPrivate = memdescGetMemData(pMemDesc);
    if (!pAllocPrivate)
    {
        NV_PRINTF(LEVEL_ERROR, "pAllocPrivate is NULL!\n");
        return NV_ERR_INVALID_STATE;
    }

    if (Kernel)
    {
        pAddress = nv_alloc_kernel_mapping(nv, pAllocPrivate,
                pageIndex, pageOffset, Length, &pPrivate);
        if (pAddress == NULL)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "failed to create system memory kernel mapping!\n");
            rmStatus = NV_ERR_GENERIC;
        }
        else
        {
            *ppAddress = NV_PTR_TO_NvP64(pAddress);
            *ppPrivate = NV_PTR_TO_NvP64(pPrivate);
        }
    }
    else
    {
        rmStatus = nv_alloc_user_mapping(nv, pAllocPrivate,
                pageIndex, pageOffset, Length, Protect, &userAddress,
                &pPrivate);
        if (rmStatus != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "failed to create system memory user mapping!\n");
        }
        else
        {
            *ppAddress = (NvP64)(userAddress);
            *ppPrivate = NV_PTR_TO_NvP64(pPrivate);
        }
    }

    return rmStatus;
}

void osUnmapSystemMemory
(
    MEMORY_DESCRIPTOR *pMemDesc,
    NvBool Kernel,
    NvU32  ProcessId,
    NvP64  pAddress,
    NvP64  pPrivate
)
{
    NV_STATUS status;
    void *pAllocPrivate = memdescGetMemData(pMemDesc);
    OBJGPU *pGpu = pMemDesc->pGpu;
    nv_state_t *nv = NV_GET_NV_STATE(pGpu);

    if (Kernel)
    {
        status = nv_free_kernel_mapping(nv, pAllocPrivate, NvP64_VALUE(pAddress),
                NvP64_VALUE(pPrivate));
    }
    else
    {
        status = nv_free_user_mapping(nv, pAllocPrivate, (NvU64)pAddress,
                NvP64_VALUE(pPrivate));
    }

    if (pGpu != NULL &&
        pGpu->getProperty(pGpu, PDB_PROP_GPU_COHERENT_CPU_MAPPING) &&
        memdescGetAddressSpace(pMemDesc) == ADDR_FBMEM &&
        pAllocPrivate != NULL)
    {
        nv_unregister_phys_pages(nv, pAllocPrivate);
        memdescSetMemData(pMemDesc, NULL, NULL);
    }

    NV_ASSERT(status == NV_OK);
}

void osIoWriteByte(
    NvU32   Address,
    NvU8    Value
)
{
    os_io_write_byte(Address, Value);
}

NvU16 osIoReadWord(
    NvU32   Address
)
{
    return os_io_read_word(Address);
}

void osIoWriteWord(
    NvU32 Address,
    NvU16 Value
)
{
    os_io_write_word(Address, Value);
}

NvU8 osIoReadByte(
    NvU32   Address
)
{
    return os_io_read_byte(Address);
}

NvBool osIsAdministrator(void)
{
    return os_is_administrator();
}

NvBool osAllowPriorityOverride(void)
{
    return os_allow_priority_override();
}

NvU32 osGetCurrentProcess(void)
{
    return os_get_current_process();
}

void osGetCurrentProcessName(char *ProcName, NvU32 Length)
{
    return os_get_current_process_name(ProcName, Length);
}

NV_STATUS osGetCurrentThread(OS_THREAD_HANDLE *pThreadId)
{
    NV_STATUS rmStatus;
    NvU64 threadId = 0;

    if (pThreadId == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    rmStatus = os_get_current_thread(&threadId);
    if (rmStatus == NV_OK)
    {
        *pThreadId = threadId;
    }
    else
    {
        *pThreadId = 0;
    }

    return rmStatus;
}

void* osGetPidInfo(void)
{
    return os_get_pid_info();
}

void osPutPidInfo(void *pOsPidInfo)
{
    os_put_pid_info(pOsPidInfo);
}

NV_STATUS osFindNsPid(void *pOsPidInfo, NvU32 *pNsPid)
{
    return os_find_ns_pid(pOsPidInfo, pNsPid);
}

NV_STATUS osAttachToProcess(void** ppProcessInfo, NvU32 ProcessId)
{
    //
    // This function is used by RmUnmapMemory() to attach to the
    // process for which a given device memory mapping was
    // created, in order to be able to unmap it. On Linux/UNIX
    // platforms, we can't "attach" to a random process, but
    // since we don't create/destroy user mappings in the RM, we
    // don't need to, either.
    //
    // Report success to the caller to keep RmUnmapMemory() from
    // failing, and memory from being leaked as a result.
    //
    *ppProcessInfo = NULL;
    return NV_OK;
}

void osDetachFromProcess(void* pProcessInfo)
{
    // stub
    return;
}

NV_STATUS osAcquireRmSema(void *pSema)
{
    return NV_OK;
}

NV_STATUS osCondAcquireRmSema(void *pSema)
{
    return NV_OK;
}

NvU32 osReleaseRmSema(void *pSema, OBJGPU *pDpcGpu)
{
    return NV_SEMA_RELEASE_SUCCEED;
}

void osSpinLoop(void)
{
    // Enable this code to get debug prints from Libos.
}

NvU64 osGetMaxUserVa(void)
{
    return os_get_max_user_va();
}

NV_STATUS osSchedule(void)
{
    return os_schedule();
}

NV_STATUS osQueueWorkItemWithFlags(
    OBJGPU *pGpu,
    OSWorkItemFunction pFunction,
    void *pParams,
    NvU32 flags
)
{
    nv_work_item_t *pWi;
    nv_state_t *nv;
    NV_STATUS status;

    pWi = portMemAllocNonPaged(sizeof(nv_work_item_t));

    if (NULL == pWi)
    {
        return NV_ERR_NO_MEMORY;
    }

    pWi->flags = NV_WORK_ITEM_FLAGS_REQUIRES_GPU;
    if (flags & OS_QUEUE_WORKITEM_FLAGS_DONT_FREE_PARAMS)
        pWi->flags |= NV_WORK_ITEM_FLAGS_DONT_FREE_DATA;

    if (flags & OS_QUEUE_WORKITEM_FLAGS_LOCK_SEMA)
        pWi->flags |= OS_QUEUE_WORKITEM_FLAGS_LOCK_SEMA;
    if (flags & OS_QUEUE_WORKITEM_FLAGS_LOCK_API_RW)
        pWi->flags |= OS_QUEUE_WORKITEM_FLAGS_LOCK_API_RW;
    if (flags & OS_QUEUE_WORKITEM_FLAGS_LOCK_GPUS)
        pWi->flags |= OS_QUEUE_WORKITEM_FLAGS_LOCK_GPUS;
    if (flags & OS_QUEUE_WORKITEM_FLAGS_LOCK_GPU_GROUP_DEVICE)
        pWi->flags |= OS_QUEUE_WORKITEM_FLAGS_LOCK_GPU_GROUP_DEVICE;
    if (flags & OS_QUEUE_WORKITEM_FLAGS_LOCK_GPU_GROUP_SUBDEVICE)
        pWi->flags |= OS_QUEUE_WORKITEM_FLAGS_LOCK_GPU_GROUP_SUBDEVICE;

    if (flags & OS_QUEUE_WORKITEM_FLAGS_FULL_GPU_SANITY)
        pWi->flags |= OS_QUEUE_WORKITEM_FLAGS_FULL_GPU_SANITY;

    pWi->gpuInstance = gpuGetInstance(pGpu);
    pWi->func.pGpuFunction = pFunction;
    pWi->pData = pParams;
    nv = NV_GET_NV_STATE(pGpu);

    status = os_queue_work_item(nv ? nv->queue : NULL, pWi);

    if (NV_OK != status)
    {
        portMemFree((void *)pWi);
    }

    return status;
}

NV_STATUS osQueueSystemWorkItem(
    OSSystemWorkItemFunction pFunction,
    void *pParams
)
{
    nv_work_item_t *pWi;
    NV_STATUS status;

    pWi = portMemAllocNonPaged(sizeof(nv_work_item_t));

    if (NULL == pWi)
    {
        return NV_ERR_NO_MEMORY;
    }

    pWi->flags = NV_WORK_ITEM_FLAGS_NONE;
    pWi->func.pSystemFunction = pFunction;
    pWi->pData = pParams;

    status = os_queue_work_item(NULL, pWi);

    if (NV_OK != status)
    {
        portMemFree((void *)pWi);
    }

    return status;
}

void osQueueMMUFaultHandler(OBJGPU *pGpu)
{
    nv_state_t *nv = NV_GET_NV_STATE(pGpu);

    nv_schedule_uvm_isr(nv);
}

static inline nv_dma_device_t* osGetDmaDeviceForMemDesc(
    OS_GPU_INFO *pOsGpuInfo,
    MEMORY_DESCRIPTOR *pMemDesc
)
{
    return (pOsGpuInfo->niso_dma_dev != NULL) &&
           memdescGetFlag(pMemDesc, MEMDESC_FLAGS_MEMORY_TYPE_DISPLAY_NISO) ?
           pOsGpuInfo->niso_dma_dev : pOsGpuInfo->dma_dev;
}

//
// Set the DMA address size for the given GPU
//
// This is a global device setting and care would need to be taken if it was to
// be modified outside of GPU initialization. At least on Linux other drivers,
// like UVM, might be requesting its own DMA mappings for the same GPU after
// the GPU has been initialized.
//
void osDmaSetAddressSize(
    OS_GPU_INFO *pOsGpuInfo,
    NvU32        bits
)
{
    nv_set_dma_address_size(pOsGpuInfo, bits);
}

NV_STATUS osAllocPagesInternal(
    MEMORY_DESCRIPTOR *pMemDesc
)
{
    OBJSYS           *pSys      = SYS_GET_INSTANCE();
    OBJGPU           *pGpu      = pMemDesc->pGpu;
    nv_state_t       *nv        = NV_GET_NV_STATE(pGpu);
    void             *pMemData  = NULL;
    NV_STATUS         status;
    NvS32             nodeId    = NV0000_CTRL_NO_NUMA_NODE;
    NV_ADDRESS_SPACE  addrSpace = memdescGetAddressSpace(pMemDesc);
    NvU64             pageSize  = osGetPageSize();

    memdescSetAddress(pMemDesc, NvP64_NULL);
    memdescSetMemData(pMemDesc, NULL, NULL);

    NV_ASSERT_OR_RETURN(pMemDesc->PageCount > 0, NV_ERR_INVALID_ARGUMENT);

    if (memdescGetFlag(pMemDesc, MEMDESC_FLAGS_GUEST_ALLOCATED))
    {
        if (NV_RM_PAGE_SIZE < os_page_size &&
            !memdescGetContiguity(pMemDesc, AT_CPU))
        {
            RmDeflateRmToOsPageArray(memdescGetPteArray(pMemDesc, AT_CPU),
                                     pMemDesc->PageCount);
        }

        status = nv_alias_pages(
            NV_GET_NV_STATE(pGpu),
            NV_RM_PAGES_TO_OS_PAGES(pMemDesc->PageCount),
            pageSize,
            memdescGetContiguity(pMemDesc, AT_CPU),
            memdescGetCpuCacheAttrib(pMemDesc),
            memdescGetGuestId(pMemDesc),
            memdescGetPteArray(pMemDesc, AT_CPU),
            &pMemData);
    }
    else
    {
        NvBool unencrypted = 0;

        if (nv && (memdescGetFlag(pMemDesc, MEMDESC_FLAGS_ALLOC_32BIT_ADDRESSABLE)))
            nv->force_dma32_alloc = NV_TRUE;

        //
        // If AMD SEV is enabled but APM(Ampere protected) or CC(Confidential
        // Compute) mode is not enabled on the GPU, all RM and client
        // allocations must be an unprotected sysmem.  If APM is enabled and RM
        // is allocating sysmem for its internal use, then it has to be
        // unprotected as protected sysmem is not accessible to GPU.
        //
        unencrypted = memdescGetFlag(pMemDesc, MEMDESC_FLAGS_ALLOC_IN_UNPROTECTED_MEMORY);

        if (addrSpace == ADDR_SYSMEM)
        {
            nodeId = memdescGetNumaNode(pMemDesc);
        }
        else if (addrSpace == ADDR_EGM)
        {
            nodeId = GPU_GET_MEMORY_MANAGER(pGpu)->localEgmNodeId;
        }

        if (NV_RM_PAGES_TO_OS_PAGES(pMemDesc->PageCount) > NV_U32_MAX)
        {
            status = NV_ERR_INVALID_LIMIT;
        }
        else
        {
            //
            // Bug 4270864: Only non-contig EGM memory needs to specify order. Contig memory
            // calculates it within nv_alloc_pages. The long term goal is to expand the ability
            // to request large page size for all of sysmem.
            //
            if (memdescIsEgm(pMemDesc) && !memdescGetContiguity(pMemDesc, AT_CPU))
            {
                pageSize = memdescGetPageSize(pMemDesc, AT_GPU);
            }

            status = nv_alloc_pages(
                NV_GET_NV_STATE(pGpu),
                NV_RM_PAGES_TO_OS_PAGES(pMemDesc->PageCount),
                pageSize,
                memdescGetContiguity(pMemDesc, AT_CPU),
                memdescGetCpuCacheAttrib(pMemDesc),
                pSys->getProperty(pSys,
                    PDB_PROP_SYS_INITIALIZE_SYSTEM_MEMORY_ALLOCATIONS),
                unencrypted,
                nodeId,
                memdescGetPteArray(pMemDesc, AT_CPU),
                &pMemData);
        }

        if (nv && nv->force_dma32_alloc)
            nv->force_dma32_alloc = NV_FALSE;
    }

    if (status != NV_OK)
    {
        return status;
    }

    //
    // If the OS layer doesn't think in RM page size, we need to inflate the
    // PTE array into RM pages.
    //
    if (NV_RM_PAGE_SIZE < os_page_size &&
        !memdescGetContiguity(pMemDesc, AT_CPU))
    {
        RmInflateOsToRmPageArray(memdescGetPteArray(pMemDesc, AT_CPU),
                                 pMemDesc->PageCount);
    }

    memdescSetMemData(pMemDesc, pMemData, NULL);

    if ((pGpu != NULL) && IS_VIRTUAL(pGpu))
        NV_ASSERT_OK_OR_RETURN(vgpuUpdateGuestSysmemPfnBitMap(pGpu, pMemDesc, NV_TRUE));

    return status;
}

void osFreePagesInternal(
    MEMORY_DESCRIPTOR *pMemDesc
)
{
    OBJGPU *pGpu = pMemDesc->pGpu;
    NV_STATUS rmStatus;

    if ((pGpu != NULL) && IS_VIRTUAL(pGpu))
        NV_ASSERT_OR_RETURN_VOID(vgpuUpdateGuestSysmemPfnBitMap(pGpu, pMemDesc, NV_FALSE) == NV_OK);

    if (NV_RM_PAGE_SIZE < os_page_size &&
        !memdescGetContiguity(pMemDesc, AT_CPU))
    {
        RmDeflateRmToOsPageArray(memdescGetPteArray(pMemDesc, AT_CPU),
                                 pMemDesc->PageCount);
    }

    rmStatus = nv_free_pages(NV_GET_NV_STATE(pGpu),
        NV_RM_PAGES_TO_OS_PAGES(pMemDesc->PageCount),
        memdescGetContiguity(pMemDesc, AT_CPU),
        memdescGetCpuCacheAttrib(pMemDesc),
        memdescGetMemData(pMemDesc));
    NV_ASSERT(rmStatus == NV_OK);
}

NV_STATUS osLockMem(
    MEMORY_DESCRIPTOR *pMemDesc
)
{
    // Not supported on this OS.
    DBG_BREAKPOINT();
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS osUnlockMem(
    MEMORY_DESCRIPTOR *pMemDesc
)
{
    // Not supported on this OS.
    DBG_BREAKPOINT();
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS osMapPciMemoryUser(
    OS_GPU_INFO *pOsGpuInfo,
    RmPhysAddr   busAddress,
    NvU64        length,
    NvU32        Protect,
    NvP64       *pVirtualAddress,
    NvP64       *pPriv,
    NvU32        modeFlag
)
{
    void *addr;
    NvU64 offset = 0;

    NV_ASSERT_OR_RETURN(length != 0, NV_ERR_INVALID_ARGUMENT);

    offset = busAddress & (os_page_size - 1llu);
    busAddress = NV_ALIGN_DOWN64(busAddress, os_page_size);
    length = NV_ALIGN_UP64(busAddress + offset + length, os_page_size) - busAddress;

    addr = os_map_user_space(busAddress, length, modeFlag, Protect, (void **) pPriv);

    NV_ASSERT_OR_RETURN(addr != NULL, NV_ERR_INVALID_ADDRESS);

    *pVirtualAddress = (NvP64)(((NvU64) addr) + offset);

    return NV_OK;
}

void osUnmapPciMemoryUser(
    OS_GPU_INFO *pOsGpuInfo,
    NvP64        virtualAddress,
    NvU64        length,
    NvP64        pPriv
)
{
    NvU64 addr;
    void *priv;

    addr = (NvU64)(virtualAddress);
    priv = (void*)(pPriv);

    length = NV_ALIGN_UP64(addr + length, os_page_size);
    addr = NV_ALIGN_DOWN64(addr, os_page_size);
    length -= addr;

    os_unmap_user_space((void *)addr, length, priv);
}

NV_STATUS osMapPciMemoryKernelOld
(
    OBJGPU    *pGpu,
    RmPhysAddr busAddress,
    NvU64      length,
    NvU32      Protect,
    void     **pVirtualAddress,
    NvU32      modeFlag
)
{
    nv_state_t *nv = NV_GET_NV_STATE(pGpu);
    nv_kern_mapping_t *mapping;

    if (pVirtualAddress == NULL)
        return NV_ERR_GENERIC;

    *pVirtualAddress = os_map_kernel_space(busAddress, length, modeFlag);
    if (*pVirtualAddress == NULL)
        return NV_ERR_GENERIC;

    mapping = portMemAllocNonPaged(sizeof(nv_kern_mapping_t));
    if (NULL == mapping)
    {
        os_unmap_kernel_space(*pVirtualAddress, length);
        *pVirtualAddress = 0;
        return NV_ERR_GENERIC;
    }

    mapping->addr = *pVirtualAddress;
    mapping->size = length;
    mapping->modeFlag = modeFlag;

    mapping->next = nv->kern_mappings;
    nv->kern_mappings = mapping;

    return NV_OK;
}

NV_STATUS osMapPciMemoryKernel64
(
    OBJGPU    *pGpu,
    RmPhysAddr busAddress,
    NvU64      length,
    NvU32      Protect,
    NvP64     *pVirtualAddress,
    NvU32      modeFlag
)
{
    void *tmppVirtualAddress = NvP64_VALUE(pVirtualAddress);
    NV_STATUS rc;

    rc = osMapPciMemoryKernelOld(pGpu,
                                 busAddress,
                                 length,
                                 Protect,
                                 &tmppVirtualAddress,
                                 modeFlag);

    *pVirtualAddress = NV_PTR_TO_NvP64(tmppVirtualAddress);

    return rc;
}

void osUnmapPciMemoryKernelOld
(
    OBJGPU *pGpu,
    void*   virtualAddress
)
{
    nv_state_t *nv = NV_GET_NV_STATE(pGpu);
    nv_kern_mapping_t *mapping, *tmp;

    // this can happen, for example, during a call to RmShutdownAdapter()
    // from a failed RmInitAdapter()
    if (virtualAddress == NULL)
    {
        return;
    }

    tmp = mapping = nv->kern_mappings;
    while (mapping)
    {
        if (mapping->addr == virtualAddress)
        {
            if (mapping == nv->kern_mappings)
            {
                nv->kern_mappings = mapping->next;
            }
            else
            {
                tmp->next = mapping->next;
            }

            os_unmap_kernel_space(mapping->addr, mapping->size);

            portMemFree(mapping);
            return;
        }
        tmp = mapping;
        mapping = mapping->next;
    }

    DBG_BREAKPOINT();
}

void osUnmapPciMemoryKernel64
(
    OBJGPU *pGpu,
    NvP64   virtualAddress
)
{
    osUnmapPciMemoryKernelOld(pGpu, NvP64_VALUE(virtualAddress));
}

NV_STATUS osMapGPU(
    OBJGPU        *pGpu,
    RS_PRIV_LEVEL  privLevel,
    NvU64          offset,
    NvU64          length,
    NvU32          Protect,
    NvP64         *pAddress,
    NvP64         *pPriv
)
{
    NV_STATUS rmStatus = NV_OK;

    if (privLevel >= RS_PRIV_LEVEL_KERNEL)
    {
        if (!portSafeAddU64((NvUPtr)pGpu->deviceMappings[0].gpuNvAddr, offset, (NvU64*)pAddress))
        {
            rmStatus = NV_ERR_INVALID_LIMIT;
        }
    }
    else
    {
        RmPhysAddr busAddress;
        if (!portSafeAddU64(pGpu->busInfo.gpuPhysAddr, offset, &busAddress))
        {
            rmStatus = NV_ERR_INVALID_LIMIT;
        }
        else
        {
            rmStatus = osMapPciMemoryUser(pGpu->pOsGpuInfo,
                                          busAddress,
                                          length,
                                          Protect,
                                          pAddress,
                                          pPriv,
                                          NV_FALSE);
        }
    }

    return rmStatus;
}

void osUnmapGPU(
    OS_GPU_INFO   *pOsGpuInfo,
    RS_PRIV_LEVEL  privLevel,
    NvP64          address,
    NvU64          length,
    NvP64          priv
)
{
    if (privLevel < RS_PRIV_LEVEL_KERNEL)
    {
        osUnmapPciMemoryUser(pOsGpuInfo, address, length, priv);
    }
}

static void postEvent(
    nv_event_t *event,
    NvU32 hEvent,
    NvU32 notifyIndex,
    NvU32 info32,
    NvU16 info16,
    NvBool dataValid
)
{
    if (osReferenceObjectCount(event) != NV_OK)
        return;
    nv_post_event(event, hEvent, notifyIndex,
                  info32, info16, dataValid);
    osDereferenceObjectCount(event);
}

NvU32 osSetEvent
(
    OBJGPU   *pGpu,
    NvP64     eventID
)
{
    nv_event_t *event = NvP64_VALUE(eventID);
    postEvent(event, 0, 0, 0, 0, NV_FALSE);
    return 1;
}

NV_STATUS osNotifyEvent(
    OBJGPU             *pGpu,
    PEVENTNOTIFICATION  NotifyEvent,
    NvU32               Method,
    NvU32               Data,
    NV_STATUS           Status
)
{
    NV_STATUS rmStatus = NV_OK;

    // notify the event
    switch (NotifyEvent->NotifyType)
    {
        case NV01_EVENT_OS_EVENT:
        {
            nv_event_t *event = NvP64_VALUE(NotifyEvent->Data);
            postEvent(event,
                      NotifyEvent->hEvent,
                      NotifyEvent->NotifyIndex,
                      0, 0,
                      NotifyEvent->bEventDataRequired);
            break;
        }

        // NOTE: NV01_EVENT_KERNEL_CALLBACK is deprecated. please use NV01_EVENT_KERNEL_CALLBACK_EX.
        case NV01_EVENT_KERNEL_CALLBACK:
        {
            MINIPORT_CALLBACK callBackToMiniport =
                (MINIPORT_CALLBACK)NvP64_VALUE(NotifyEvent->Data);

            // perform a direct callback to the miniport
            if (callBackToMiniport)
                callBackToMiniport(NV_GET_NV_STATE(pGpu));
            break;
        }

        case NV01_EVENT_KERNEL_CALLBACK_EX:
        {
            NVOS10_EVENT_KERNEL_CALLBACK_EX *kc = (NVOS10_EVENT_KERNEL_CALLBACK_EX *)NvP64_VALUE(NotifyEvent->Data);

            // passes two arguments (arg, params) to the kernel callback instead of one (arg).
            if (kc && kc->func)
            {
                kc->func(kc->arg, NULL, NotifyEvent->hEvent, Data, Status);
            }
            break;
        }


        default:
        {
            rmStatus = NV_ERR_GENERIC;
            break;
        }
    }

    return rmStatus;

} // end of osNotifyEvent()

// Allow CPL Events to be callback or events
NV_STATUS osEventNotification
(
    OBJGPU   *pGpu,
    PEVENTNOTIFICATION pNotifyEvent,
    NvU32 notifyIndex,
    void * pEventData,
    NvU32 eventDataSize
)
{
    return osEventNotificationWithInfo(pGpu, pNotifyEvent, notifyIndex, 0, 0,
                                       pEventData, eventDataSize);
}

NV_STATUS osEventNotificationWithInfo
(
    OBJGPU   *pGpu,
    PEVENTNOTIFICATION pNotifyEvent,
    NvU32 notifyIndex,
    NvU32 info32,
    NvU16 info16,
    void * pEventData,
    NvU32 eventDataSize
)
{
    NV_STATUS rmStatus = NV_OK;

    // walk this object's event list and find any matches for this specific notify
    for (; pNotifyEvent; pNotifyEvent = pNotifyEvent->Next)
    {
        // notifyIndex must match if request isn't for all
        if ((notifyIndex != OS_EVENT_NOTIFICATION_INDEX_ALL) &&
            (pNotifyEvent->NotifyIndex != notifyIndex))
        {
            continue;
        }

        switch (pNotifyEvent->NotifyType)
        {
            case NV_EVENT_BUFFER_BIND:
            case NV01_EVENT_WIN32_EVENT:
            {
                nv_event_t *event = NvP64_VALUE(pNotifyEvent->Data);
                postEvent(event,
                          pNotifyEvent->hEvent,
                          pNotifyEvent->NotifyIndex,
                          info32, info16,
                          pNotifyEvent->bEventDataRequired);
                break;
            }

            // NOTE: NV01_EVENT_KERNEL_CALLBACK is deprecated. please use NV01_EVENT_KERNEL_CALLBACK_EX.
            case NV01_EVENT_KERNEL_CALLBACK:
            {
                MINIPORT_CALLBACK callBackToMiniport =
                    (MINIPORT_CALLBACK)NvP64_VALUE(pNotifyEvent->Data);

                // perform a direct callback to the miniport
                if (callBackToMiniport)
                    callBackToMiniport(NV_GET_NV_STATE(pGpu));
                break;
            }

            case NV01_EVENT_KERNEL_CALLBACK_EX:
            {
                NVOS10_EVENT_KERNEL_CALLBACK_EX *kc = (NVOS10_EVENT_KERNEL_CALLBACK_EX *)NvP64_VALUE(pNotifyEvent->Data);

                if (kc && kc->func)
                {
                    kc->func(kc->arg, pEventData, pNotifyEvent->hEvent, 0, NV_OK);
                }
                break;
            }

            default:
                break;
        }
    }

    return rmStatus;
}

// Allow CPL Events to be callback or events
NV_STATUS osObjectEventNotification
(
    NvHandle            hClient,
    NvHandle            hObject,
    NvU32               hClass,
    PEVENTNOTIFICATION  pNotifyEvent,
    NvU32               notifyIndex,
    void                *pEventData,
    NvU32               eventDataSize
)
{
    NV_STATUS rmStatus = NV_OK;

    NV_PRINTF(LEVEL_INFO, "%s()\n", __FUNCTION__);
    // walk this object's event list and find any matches for this specific notify
    for (; pNotifyEvent; pNotifyEvent = pNotifyEvent->Next)
    {
        // notifyIndex must match if request isn't for all
        if ((notifyIndex != OS_EVENT_NOTIFICATION_INDEX_ALL) &&
            (pNotifyEvent->NotifyIndex != notifyIndex))
        {
            continue;
        }

        switch (pNotifyEvent->NotifyType)
        {
            case NV01_EVENT_OS_EVENT:
            {
                nv_event_t *event = NvP64_VALUE(pNotifyEvent->Data);
                postEvent(event,
                          pNotifyEvent->hEvent,
                          pNotifyEvent->NotifyIndex,
                          0, 0,
                          pNotifyEvent->bEventDataRequired);
                break;
            }

            case NV01_EVENT_KERNEL_CALLBACK_EX:
            {
                NVOS10_EVENT_KERNEL_CALLBACK_EX *kc = (NVOS10_EVENT_KERNEL_CALLBACK_EX *)NvP64_VALUE(pNotifyEvent->Data);

                if (kc && kc->func)
                {
                    kc->func(kc->arg, pEventData, pNotifyEvent->hEvent, 0, NV_OK);
                }
                break;
            }

            default:
                break;
        }
    }

    return rmStatus;
}

NV_STATUS osReferenceObjectCount(void *pEvent)
{
    nv_state_t *nv = nv_get_ctl_state();
    nv_event_t *event = pEvent;

    portSyncSpinlockAcquire(nv->event_spinlock);
    // If event->active is false, don't allow any more reference
    if (!event->active)
    {
        portSyncSpinlockRelease(nv->event_spinlock);
        return NV_ERR_INVALID_EVENT;
    }
    ++event->refcount;
    portSyncSpinlockRelease(nv->event_spinlock);
    return NV_OK;
}

NV_STATUS osDereferenceObjectCount(void *pOSEvent)
{
    nv_state_t *nv = nv_get_ctl_state();
    nv_event_t *event = pOSEvent;

    portSyncSpinlockAcquire(nv->event_spinlock);
    NV_ASSERT(event->refcount > 0);
    // If event->refcount == 0 but event->active is true, the client
    // has not yet freed the OS event.  free_os_event will free its
    // memory when they do, or else when the client itself is freed.
    if (--event->refcount == 0 && !event->active)
        portMemFree(event);
    portSyncSpinlockRelease(nv->event_spinlock);

    return NV_OK;
}

NV_STATUS osUserHandleToKernelPtr(NvHandle hClient, NvP64 hEvent, NvP64 *pEvent)
{
    nv_state_t *nv = nv_get_ctl_state();
    NvU32 fd = (NvU64)hEvent;
    NV_STATUS result;

    portSyncSpinlockAcquire(nv->event_spinlock);
    nv_event_t *e = nv->event_list;
    while (e != NULL)
    {
        if (e->fd == fd && e->hParent == hClient)
            break;
        e = e->next;
    }

    if (e != NULL)
    {
        ++e->refcount;
        *pEvent = NV_PTR_TO_NvP64(e);
        result = NV_OK;
    }
    else
        result = NV_ERR_OBJECT_NOT_FOUND;
    portSyncSpinlockRelease(nv->event_spinlock);

    return result;
}

NV_STATUS osFlushCpuCache(void)
{
    return os_flush_cpu_cache_all();
}

void osFlushCpuWriteCombineBuffer(void)
{
    os_flush_cpu_write_combine_buffer();
}


//
// Evict GPU memory range from the CPU caches.
//
// On some platforms (e.g. P9+V100), the CPU can coherently cache GPU memory
// and RM takes advantage of that. Most everything is handled transparently,
// but there are two exceptions that require explicitly flushing any CPU cache
// lines of GPU memory. These are:
//
// 1) Flushing memory backing ACR regions before they get locked.
//
// Otherwise the cache could get flushed while the regions are locked causing a
// region violation physical fault. See more details in
// acrFlushRegionsFromGpuCoherentCpuCache_IMPL().
//
// 2) Flushing all of FB before GPU reset (NVLink going down)
//
// Leaving cache entries on the CPU causes fatal errors when the CPU tries
// flushing them later while the link is down. See more details in
// nvlinkStatePostUnload_IMPL().
//
void osFlushGpuCoherentCpuCacheRange
(
    OS_GPU_INFO *pOsGpuInfo,
    NvU64        cpuVirtual,
    NvU64        size
)
{
    nv_flush_coherent_cpu_cache_range(pOsGpuInfo, cpuVirtual, size);
}

void osErrorLogV(OBJGPU *pGpu, NvU32 num, const char * pFormat, va_list arglist)
{
    NV_STATUS        rmStatus;
    nv_state_t      *nv             = NV_GET_NV_STATE(pGpu);

    if ((pFormat == NULL) || (*pFormat == '\0'))
    {
        return;
    }

    rmStatus = nv_log_error(nv, num, pFormat, arglist);
    NV_ASSERT(rmStatus == NV_OK);
}

void osErrorLog(OBJGPU *pGpu, NvU32 num, const char* pFormat, ...)
{
    va_list arglist;
    va_start(arglist, pFormat);
    osErrorLogV(pGpu, num, pFormat, arglist);
    va_end(arglist);
}

NvU32
osPollHotkeyState
(
    OBJGPU  *pGpu
)
{
    return 0;
}

void osDevWriteReg008(
     OBJGPU            *pGpu,
     DEVICE_MAPPING    *pMapping,
     NvU32              thisAddress,
     NvV8               thisValue
)
{
    if (thisAddress >= pMapping->gpuNvLength)
    {
        NV_ASSERT(thisAddress < pMapping->gpuNvLength);
        return;
    }

    NV_PRIV_REG_WR08(pMapping->gpuNvAddr, thisAddress, thisValue);
}

void osDevWriteReg016(
     OBJGPU            *pGpu,
     DEVICE_MAPPING    *pMapping,
     NvU32              thisAddress,
     NvV16              thisValue
)
{
    if (thisAddress >= pMapping->gpuNvLength)
    {
        NV_ASSERT(thisAddress < pMapping->gpuNvLength);
        return;
    }

    NV_PRIV_REG_WR16(pMapping->gpuNvAddr, thisAddress, thisValue);
}

void osDevWriteReg032(
     OBJGPU            *pGpu,
     DEVICE_MAPPING    *pMapping,
     NvU32              thisAddress,
     NvV32              thisValue
)
{
    NvBool vgpuHandled = NV_FALSE;

    vgpuDevWriteReg032(pGpu, thisAddress, thisValue, &vgpuHandled);
    if (vgpuHandled)
    {
        return;
    }

    if (thisAddress >= pMapping->gpuNvLength)
    {
        NV_ASSERT(thisAddress < pMapping->gpuNvLength);
        return;
    }

    NV_PRIV_REG_WR32(pMapping->gpuNvAddr, thisAddress, thisValue);
}

NvU8 osDevReadReg008(
    OBJGPU             *pGpu,
    DEVICE_MAPPING     *pMapping,
    NvU32               thisAddress
)
{
    NvU8 retval = 0;

    if (thisAddress >= pMapping->gpuNvLength)
    {
        NV_ASSERT(thisAddress < pMapping->gpuNvLength);
    }
    else
        retval = NV_PRIV_REG_RD08(pMapping->gpuNvAddr, thisAddress);

    return retval;
}

NvU16 osDevReadReg016(
    OBJGPU             *pGpu,
    DEVICE_MAPPING     *pMapping,
    NvU32               thisAddress
)
{
    NvU16 retval = 0;

    if (thisAddress >= pMapping->gpuNvLength)
    {
        NV_ASSERT(thisAddress < pMapping->gpuNvLength);
    }
    else
        retval = NV_PRIV_REG_RD16(pMapping->gpuNvAddr, thisAddress);

    return retval;
}

NvU32 osDevReadReg032(
    OBJGPU             *pGpu,
    DEVICE_MAPPING     *pMapping,
    NvU32               thisAddress
)
{
    NvU32 retval = 0;
    NvBool vgpuHandled = NV_FALSE;

    retval = vgpuDevReadReg032(pGpu, thisAddress, &vgpuHandled);
    if (vgpuHandled)
    {
        return retval;
    }

    if (thisAddress >= pMapping->gpuNvLength)
    {
        NV_ASSERT(thisAddress < pMapping->gpuNvLength);
    }
    else
        retval = NV_PRIV_REG_RD32(pMapping->gpuNvAddr, thisAddress);

    return retval;
}

NV_STATUS osReadRegistryDwordBase(
    OBJGPU     *pGpu,
    const char *regParmStr,
    NvU32      *Data
)
{
    nv_state_t *nv = NV_GET_NV_STATE(pGpu);
    return RmReadRegistryDword(nv, regParmStr, Data);
}

NV_STATUS osWriteRegistryDword(
    OBJGPU     *pGpu,
    const char *regParmStr,
    NvU32       Data
)
{
    nv_state_t *nv = NV_GET_NV_STATE(pGpu);
    return RmWriteRegistryDword(nv, regParmStr, Data);
}

NV_STATUS osReadRegistryBinary(
    OBJGPU     *pGpu,
    const char *regParmStr,
    NvU8       *Data,
    NvU32      *cbLen
)
{
    nv_state_t *nv = NV_GET_NV_STATE(pGpu);
    return RmReadRegistryBinary(nv, regParmStr, Data, cbLen);
}

NV_STATUS osWriteRegistryBinary(
    OBJGPU     *pGpu,
    const char *regParmStr,
    NvU8       *Data,
    NvU32       cbLen
)
{
    nv_state_t *nv = NV_GET_NV_STATE(pGpu);
    return RmWriteRegistryBinary(nv, regParmStr, Data, cbLen);
}

NV_STATUS osWriteRegistryVolatile(
    OBJGPU     *pGpu,
    const char *regParmStr,
    NvU8       *Data,
    NvU32       cbLen
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS osReadRegistryVolatile
(
    OBJGPU     *pGpu,
    const char *regParmStr,
    NvU8       *Data,
    NvU32       cbLen
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS osReadRegistryVolatileSize
(
    OBJGPU     *pGpu,
    const char *regParmStr,
    NvU32      *pSize
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS osReadRegistryStringBase(
    OBJGPU     *pGpu,
    const char *regParmStr,
    NvU8       *buffer,
    NvU32      *pBufferLength
)
{
    nv_state_t *nv = NV_GET_NV_STATE(pGpu);
    return RmReadRegistryString(nv, regParmStr, buffer, pBufferLength);
}

NV_STATUS osPackageRegistry(
    OBJGPU                 *pGpu,
    PACKED_REGISTRY_TABLE  *pRegTable,
    NvU32                  *pSize
)
{
    nv_state_t *nv = NV_GET_NV_STATE(pGpu);
    return RmPackageRegistry(nv, pRegTable, pSize);
}

NvU32 osGetCpuCount(void)
{
    return os_get_cpu_count();   // Total number of logical CPUs.
}

NvU32 osGetCurrentProcessorNumber(void)
{
    return os_get_cpu_number();
}

void osGetTimeoutParams(OBJGPU *pGpu, NvU32 *pTimeoutUs, NvU32 *pScale, NvU32 *pFlags)
{
    NvU32 gpuMode   = gpuGetMode(pGpu);

    NV_ASSERT((NV_GPU_MODE_GRAPHICS_MODE == gpuMode) ||
              (NV_GPU_MODE_COMPUTE_MODE  == gpuMode));

    if (hypervisorIsVgxHyper())
    {
        if (IS_GSP_CLIENT(pGpu) && pGpu->getProperty(pGpu, PDB_PROP_GPU_EXTENDED_GSP_RM_INITIALIZATION_TIMEOUT_FOR_VGX) &&
            !pGpu->gspRmInitialized)
        {
            //
            // For Hopper, 1.8 seconds is not enough to boot GSP-RM.
            // To avoid this issue, 4 seconds timeout is set on initialization,
            // and then it's going to be changed 1.8 seconds after GSP initialization.
            //
            *pTimeoutUs = 4 * 1000000;
        }
        else
        {
            //
            // 1.8 seconds is chosen because it is 90% of the overall hard limit of 2.0
            // seconds, imposed by WDDM driver rules.
            // Currently primary use case of VGX is Windows, so setting 1.8 as default
            //
            *pTimeoutUs = 1.8 * 1000000;
        }
    }
    else
    {
        switch (gpuMode)
        {
        default:
        case NV_GPU_MODE_GRAPHICS_MODE:
            *pTimeoutUs = 4 * 1000000;
            break;

        case NV_GPU_MODE_COMPUTE_MODE:
            *pTimeoutUs = 30 * 1000000;
            break;
        }
    }

    *pFlags = GPU_TIMEOUT_FLAGS_OSTIMER;

    *pScale = 1;
    if (IS_EMULATION(pGpu) || IS_SIMULATION(pGpu))
    {
        *pScale = 60;       // 1s -> 1m
    }

    return;
}

void osFlushLog(void)
{
    // Not implemented
}

NvU32 osGetSimulationMode(void)
{
    return NV_SIM_MODE_HARDWARE;
}

NV_STATUS
cliresCtrlCmdOsUnixFlushUserCache_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_OS_UNIX_FLUSH_USER_CACHE_PARAMS *pAddressSpaceParams
)
{
    Memory *pMemory;
    MEMORY_DESCRIPTOR *pMemDesc;
    NvU64 start, end;
    NvBool bInvalidateOnly;

    NV_CHECK_OK_OR_RETURN(LEVEL_SILENT,
        memGetByHandle(RES_GET_CLIENT(pRmCliRes),
                       pAddressSpaceParams->hObject,
                       &pMemory));

    pMemDesc = pMemory->pMemDesc;

    if (memdescGetAddressSpace(pMemDesc) != ADDR_SYSMEM)
    {
        NV_PRINTF(LEVEL_ERROR, "%s: wrong address space %d\n",
                  __FUNCTION__, memdescGetAddressSpace(pMemDesc));
        return NV_ERR_INVALID_COMMAND;
    }

    if (memdescGetCpuCacheAttrib(pMemDesc) != NV_MEMORY_CACHED)
    {
        NV_PRINTF(LEVEL_ERROR, "%s: wrong caching type %d\n",
                  __FUNCTION__, memdescGetCpuCacheAttrib(pMemDesc));
        return NV_ERR_INVALID_COMMAND;
    }

    start = pAddressSpaceParams->offset;
    end = start + pAddressSpaceParams->length;

    switch(pAddressSpaceParams->cacheOps)
    {
        case NV0000_CTRL_OS_UNIX_FLAGS_USER_CACHE_FLUSH_INVALIDATE:
        case NV0000_CTRL_OS_UNIX_FLAGS_USER_CACHE_FLUSH:
            bInvalidateOnly = NV_FALSE;
            break;

        case NV0000_CTRL_OS_UNIX_FLAGS_USER_CACHE_INVALIDATE:
            bInvalidateOnly = NV_TRUE;
            break;

        default:
            NV_PRINTF(LEVEL_ERROR, "%s: cacheOps not specified\n", __FUNCTION__);
            return NV_ERR_INVALID_COMMAND;
    }

    if ((end - start) > pMemory->Length)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "%s: end address 0x%llx exceeded buffer length: 0x%llx\n",
                  __FUNCTION__, end, pMemory->Length);
        return NV_ERR_INVALID_LIMIT;
    }

    if (bInvalidateOnly)
    {
        //
        // XXX: this seems fishy - I'm not sure if invalidating by the kernel
        // VA only as nv_dma_cache_invalidate() does here is sufficient for
        // this control call.
        // pAddressSpaceParams->internalOnly is expected to be the RM client
        // VA for this control call; if we wanted to invalidate the user VA we
        // could do so using that.
        //
        // For I/O coherent platforms this won't actually do anything.
        // On non-I/O-coherent platforms, there's no need to do a second
        // invalidation after the full flush.
        //
        nv_state_t *nv = NV_GET_NV_STATE(pMemDesc->pGpu);
        if (nv->iovaspace_id != NV_IOVA_DOMAIN_NONE)
        {
            PIOVAMAPPING pIovaMapping = memdescGetIommuMap(pMemDesc, nv->iovaspace_id);
            //
            // This should only be called for devices that map memory descriptors
            // through the nv-dma library, where the memory descriptor data
            // contains all the kernel-specific context we need for the
            // invalidation.
            //
            // (These checks match those in osIovaUnmap() leading up to
            // nv_dma_unmap_alloc()).
            //
            if (pIovaMapping == NULL ||
                pIovaMapping->pOsData == NULL ||
                memdescGetFlag(pIovaMapping->pPhysMemDesc, MEMDESC_FLAGS_GUEST_ALLOCATED) ||
                memdescGetFlag(pIovaMapping->pPhysMemDesc, MEMDESC_FLAGS_PEER_IO_MEM))
            {
                return NV_ERR_INVALID_ARGUMENT;
            }

            nv_dma_cache_invalidate(nv->dma_dev, pIovaMapping->pOsData);
        }
        else
        {
            return NV_ERR_INVALID_ARGUMENT;
        }
    }
    else
    {
        return os_flush_user_cache();
    }

    return NV_OK;
}

static NV_STATUS
_initializeExportObjectFd
(
    nv_file_private_t *nvfp,
    NvHandle           hClient,
    NvHandle           hDevice,
    NvU16              maxObjects,
    NvU8              *metadata
)
{
    NV_STATUS      status;
    RsResourceRef *pResourceRef;
    Device        *pDevice;
    NvU32          deviceInstance = NV_MAX_DEVICES;
    NvU32          gpuInstanceId = NV_U32_MAX;

    if (nvfp->handles != NULL)
    {
        return NV_ERR_STATE_IN_USE;
    }

    if (hDevice != 0)
    {
        OBJGPU *pGpu;
        MIG_INSTANCE_REF ref;

        status = serverutilGetResourceRef(hClient, hDevice, &pResourceRef);
        if (status != NV_OK)
        {
            return status;
        }

        pDevice = dynamicCast(pResourceRef->pResource, Device);
        if (pDevice == NULL)
        {
            return NV_ERR_INVALID_PARAMETER;
        }

        deviceInstance = pDevice->deviceInst;
        pGpu = GPU_RES_GET_GPU(pDevice);

        if (IS_MIG_IN_USE(pGpu))
        {
            KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
            status = kmigmgrGetInstanceRefFromDevice(pGpu, pKernelMIGManager,
                                                     pDevice, &ref);
            if (status == NV_OK)
            {
                gpuInstanceId = ref.pKernelMIGGpuInstance->swizzId;
            }
        }
    }

    NV_ASSERT_OK_OR_RETURN(os_alloc_mem((void **)&nvfp->handles,
                           sizeof(nvfp->handles[0]) * maxObjects));

    os_mem_set(nvfp->handles, 0,
               sizeof(nvfp->handles[0]) * maxObjects);

    nvfp->maxHandles        = maxObjects;
    nvfp->deviceInstance    = deviceInstance;
    nvfp->gpuInstanceId     = gpuInstanceId;

    if (metadata != NULL)
    {
        os_mem_copy(nvfp->metadata, metadata, sizeof(nvfp->metadata));
    }

    return NV_OK;
}

NV_STATUS
cliresCtrlCmdOsUnixExportObjectToFd_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_OS_UNIX_EXPORT_OBJECT_TO_FD_PARAMS *pParams
)
{
    NvHandle           hClient = RES_GET_CLIENT_HANDLE(pRmCliRes);
    RmObjExportHandle  hExportHandle = 0;
    nv_file_private_t *nvfp = NULL;
    void              *priv = NULL;
    NV_STATUS          status = NV_OK;

    /*
     * This flag is intended to be implemented entirely in the rmapi library in
     * userspace, we should never encounter it here.
     */
    if (FLD_TEST_DRF(0000_CTRL, _OS_UNIX_EXPORT_OBJECT_TO_FD_FLAGS,
                     _EMPTY_FD, _TRUE, pParams->flags))
    {
        return NV_ERR_INVALID_PARAMETER;
    }

    if (pParams->object.type != NV0000_CTRL_OS_UNIX_EXPORT_OBJECT_TYPE_RM ||
        pParams->fd == -1)
    {
        return NV_ERR_INVALID_PARAMETER;
    }

    status = RmExportObject(hClient,
                            pParams->object.data.rmObject.hObject,
                            &hExportHandle, NULL);
    if (status != NV_OK)
    {
        goto done;
    }
    NV_ASSERT(hExportHandle != 0);

    nvfp = nv_get_file_private(pParams->fd, NV_TRUE, &priv);
    if (nvfp == NULL)
    {
        NV_ASSERT(priv == NULL);
        status = NV_ERR_INVALID_PARAMETER;
        goto done;
    }

    status = _initializeExportObjectFd(nvfp, hClient,
                                       pParams->object.data.rmObject.hDevice,
                                       1, NULL);
    if (status != NV_OK)
    {
        goto done;
    }

    nvfp->handles[0] = hExportHandle;

done:

    if (status != NV_OK && hExportHandle != 0)
    {
        RmFreeObjExportHandle(hExportHandle);
    }

    if (priv != NULL)
    {
        nv_put_file_private(priv);
    }

    return status;
}

// This control call has been deprecated. It will be deleted soon.
NV_STATUS
cliresCtrlCmdOsUnixCreateExportObjectFd_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_OS_UNIX_CREATE_EXPORT_OBJECT_FD_PARAMS *pParams
)
{
    NV_STATUS          status;
    NvHandle           hClient = RES_GET_CLIENT_HANDLE(pRmCliRes);
    nv_file_private_t *nvfp = NULL;
    void              *priv = NULL;

    ct_assert(sizeof(nvfp->metadata) == sizeof(pParams->metadata));

    if (pParams->maxObjects == 0)
    {
        return NV_ERR_INVALID_PARAMETER;
    }

    nvfp = nv_get_file_private(pParams->fd, NV_TRUE, &priv);
    if (nvfp == NULL)
    {
        NV_ASSERT(priv == NULL);
        status = NV_ERR_INVALID_PARAMETER;
        goto done;
    }

    status = _initializeExportObjectFd(nvfp, hClient, pParams->hDevice,
                                       pParams->maxObjects, pParams->metadata);

done:
    if (priv != NULL)
    {
        nv_put_file_private(priv);
    }

    return status;
}

NV_STATUS
cliresCtrlCmdOsUnixExportObjectsToFd_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_OS_UNIX_EXPORT_OBJECTS_TO_FD_PARAMS *pParams
)
{
    NvHandle           hClient = RES_GET_CLIENT_HANDLE(pRmCliRes);
    RmObjExportHandle *pExportHandle;
    nv_file_private_t *nvfp = NULL;
    void              *priv = NULL;
    NV_STATUS          status = NV_OK;
    NvU32              i;
    NvU32              deviceInstance;
    NvU32              result;
    NvHandle          *exportHandles = NULL;
    NvBool             bFdSetup = NV_FALSE;

    nvfp = nv_get_file_private(pParams->fd, NV_TRUE, &priv);
    if (nvfp == NULL)
    {
        NV_ASSERT(priv == NULL);
        status = NV_ERR_INVALID_PARAMETER;
        goto done;
    }

    ct_assert(sizeof(nvfp->metadata) == sizeof(pParams->metadata));

    /* Setup export FD if not done */
    if (nvfp->handles == NULL)
    {
        if (pParams->maxObjects == 0)
        {
            status = NV_ERR_INVALID_PARAMETER;
            goto done;
        }

        status = _initializeExportObjectFd(nvfp, hClient, pParams->hDevice,
                                           pParams->maxObjects,
                                           pParams->metadata);
        if (status != NV_OK)
        {
            goto done;
        }

        bFdSetup = NV_TRUE;
    }

    if ((nvfp->handles == NULL) ||
        (pParams->numObjects >
            NV0000_CTRL_OS_UNIX_EXPORT_OBJECTS_TO_FD_MAX_OBJECTS))
    {
        status = NV_ERR_INVALID_PARAMETER;
        goto done;
    }

    if (!portSafeAddU32(pParams->numObjects, pParams->index, &result) ||
        (result > nvfp->maxHandles))
    {
        status = NV_ERR_OUT_OF_RANGE;
        goto done;
    }

    status = os_alloc_mem((void **)&exportHandles,
                          sizeof(exportHandles[0]) *
                            pParams->numObjects);
    if (status != NV_OK)
    {
        goto done;
    }

    for (i = 0; i < pParams->numObjects; i++)
    {
        exportHandles[i] = 0;

        if (pParams->objects[i] == 0)
        {
            continue;
        }

        status = RmExportObject(hClient,
                                pParams->objects[i],
                                &exportHandles[i],
                                &deviceInstance);
        if (status != NV_OK)
        {
            goto done;
        }

        NV_ASSERT(exportHandles[i] != 0);

        if (deviceInstance != nvfp->deviceInstance)
        {
            status = NV_ERR_INVALID_PARAMETER;
            goto done;
        }
    }

    for (i = 0; i < pParams->numObjects; i++)
    {
        pExportHandle = &nvfp->handles[i + pParams->index];

        // If the handle already exists in this position, free it
        if (*pExportHandle != 0)
        {
            RmFreeObjExportHandle(*pExportHandle);
            *pExportHandle = 0;
        }

        *pExportHandle = exportHandles[i];
    }

done:

    if ((status != NV_OK) && (exportHandles != NULL))
    {
        for (i = 0; i < pParams->numObjects; i++)
        {
            if (exportHandles[i] != 0)
            {
                RmFreeObjExportHandle(exportHandles[i]);
            }
        }
    }

    if (exportHandles != NULL)
    {
        os_free_mem(exportHandles);
    }

    if ((status != NV_OK) && bFdSetup)
    {
        os_free_mem(nvfp->handles);
        nvfp->handles = NULL;
        nvfp->maxHandles = 0;
    }

    if (priv != NULL)
    {
        nv_put_file_private(priv);
    }

    return status;
}

NV_STATUS
cliresCtrlCmdOsUnixImportObjectFromFd_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_OS_UNIX_IMPORT_OBJECT_FROM_FD_PARAMS *pParams
)
{
    NvHandle           hClient = RES_GET_CLIENT_HANDLE(pRmCliRes);
    nv_file_private_t *nvfp = NULL;
    void              *priv = NULL;
    NV_STATUS          status = NV_OK;

    if (pParams->object.type != NV0000_CTRL_OS_UNIX_EXPORT_OBJECT_TYPE_RM ||
        pParams->fd == -1)
    {
        return NV_ERR_INVALID_PARAMETER;
    }

    nvfp = nv_get_file_private(pParams->fd, NV_TRUE, &priv);
    if (nvfp == NULL)
    {
        NV_ASSERT(priv == NULL);
        status = NV_ERR_INVALID_PARAMETER;
        goto done;
    }

    if ((nvfp->handles == NULL) || (nvfp->handles[0] == 0) ||
        (nvfp->maxHandles > 1))
    {
        status = NV_ERR_INVALID_PARAMETER;
        goto done;
    }

    status = RmImportObject(hClient,
                            pParams->object.data.rmObject.hParent,
                            &pParams->object.data.rmObject.hObject,
                            nvfp->handles[0], NULL);

done:
    if (priv != NULL)
    {
        nv_put_file_private(priv);
    }

    return status;
}

NV_STATUS
cliresCtrlCmdOsUnixImportObjectsFromFd_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_OS_UNIX_IMPORT_OBJECTS_FROM_FD_PARAMS *pParams
)
{
    NvHandle           hClient = RES_GET_CLIENT_HANDLE(pRmCliRes);
    nv_file_private_t *nvfp = NULL;
    void              *priv = NULL;
    NV_STATUS          status = NV_OK;
    NvU32              i = 0;
    RmObjExportHandle  hImportHandle;
    NvU32              result;
    RM_API            *pRmApi;

    nvfp = nv_get_file_private(pParams->fd, NV_TRUE, &priv);
    if (nvfp == NULL)
    {
        NV_ASSERT(priv == NULL);
        status = NV_ERR_INVALID_PARAMETER;
        goto done;
    }

    if ((nvfp->handles == NULL) ||
        (pParams->numObjects >
           NV0000_CTRL_OS_UNIX_IMPORT_OBJECTS_TO_FD_MAX_OBJECTS))
    {
        status = NV_ERR_INVALID_PARAMETER;
        goto done;
    }

    if (!portSafeAddU32(pParams->numObjects, pParams->index, &result) ||
        (result > nvfp->maxHandles))
    {
        status = NV_ERR_OUT_OF_RANGE;
        goto done;
    }

    for (i = 0; i < pParams->numObjects; i++)
    {
        hImportHandle = nvfp->handles[i + pParams->index];

        /* Nothing to import, just continue */
        if (hImportHandle == 0)
        {
            pParams->objectTypes[i] = \
                NV0000_CTRL_CMD_OS_UNIX_IMPORT_OBJECT_TYPE_NONE;
            continue;
        }

        status = RmImportObject(hClient,
                                pParams->hParent,
                                &pParams->objects[i],
                                hImportHandle,
                                &pParams->objectTypes[i]);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "%s: Unable to import handle (%x, %x, %x)\n",
                __FUNCTION__, pParams->hParent, pParams->objects[i], hImportHandle);
            goto done;
        }
    }

done:

    if (status != NV_OK)
    {
        pRmApi = rmapiGetInterface(RMAPI_API_LOCK_INTERNAL);
        while (i > 0)
        {
            i--;

            if (pParams->objects[i] != 0)
            {
                pRmApi->Free(pRmApi, hClient, pParams->objects[i]);
            }
        }
    }

    if (priv != NULL)
    {
        nv_put_file_private(priv);
    }

    return status;
}

NV_STATUS
cliresCtrlCmdOsUnixGetExportObjectInfo_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_OS_UNIX_GET_EXPORT_OBJECT_INFO_PARAMS *pParams
)
{
    nv_file_private_t *nvfp = NULL;
    void              *priv = NULL;
    NV_STATUS         status = NV_OK;

    if (pParams->fd < 0)
    {
        return NV_ERR_INVALID_PARAMETER;
    }

    nvfp = nv_get_file_private(pParams->fd, NV_TRUE, &priv);
    if (nvfp == NULL)
    {
        NV_ASSERT(priv == NULL);
        status = NV_ERR_INVALID_PARAMETER;
        goto done;
    }

    if (nvfp->handles == NULL)
    {
        status = NV_ERR_INVALID_PARAMETER;
        goto done;
    }

    pParams->maxObjects = nvfp->maxHandles;
    pParams->deviceInstance = nvfp->deviceInstance;
    pParams->gpuInstanceId = nvfp->gpuInstanceId;

    os_mem_copy(pParams->metadata, nvfp->metadata, sizeof(nvfp->metadata));

done:
    if (priv != NULL)
    {
        nv_put_file_private(priv);
    }

    return status;
}

/*!
 * osAcpiDsm
 *
 * @brief Handles os specific _DSM method function calls.
 *
 * Input parameters:
 * @param[in]     pGpu   : OBJGPU pointer
 * @param[in]     acpiDsmFunction    : ACPI DSM function
 * @param[in]     acpiDsmSubFunction : ACPI DSM subfunction
 * @param[in/out] pInOut : in/out buffer, caller should make sure the buffer is large enough.
 * @param[in]     pSize  : when input, size of data that the caller wants to read, in bytes.
 *                         when output, size of valid data in pInOuta in bytes.
 */
NV_STATUS osCallACPI_DSM
(
    OBJGPU            *pGpu,
    ACPI_DSM_FUNCTION  acpiDsmFunction,
    NvU32              acpiDsmSubFunction,
    NvU32             *pInOut,
    NvU16             *pSize
)
{
    NV_STATUS   status;
    NvU8       *pAcpiDsmGuid = NULL;
    NvU32       acpiDsmRev;
    nv_state_t *nv  = NV_GET_NV_STATE(pGpu);
    nv_priv_t  *nvp = NV_GET_NV_PRIV(nv);
    NvU16       acpiDsmInArgSize = 4;
    NvBool      acpiNvpcfDsmFunction = NV_FALSE;

    // do any handling/remapping of guid needed.
    status = checkDsmCall(pGpu,
                          (ACPI_DSM_FUNCTION *) &acpiDsmFunction,
                          &acpiDsmSubFunction,
                          pInOut,
                          pSize);

    // return if subfunction is not supported or we're returning cache data
    if (status != NV_WARN_MORE_PROCESSING_REQUIRED)
    {
        return status;
    }

    switch ((NvU32) acpiDsmFunction)
    {
        case ACPI_DSM_FUNCTION_NBSI:
            pAcpiDsmGuid = (NvU8 *) &NBSI_DSM_GUID;
            acpiDsmRev  = NBSI_REVISION_ID;
            break;
        case ACPI_DSM_FUNCTION_NVHG:
            pAcpiDsmGuid = (NvU8 *) &NVHG_DSM_GUID;
            acpiDsmRev  = NVHG_REVISION_ID;
            break;
        case ACPI_DSM_FUNCTION_MXM:
            pAcpiDsmGuid = (NvU8 *) &DSM_MXM_GUID;
            acpiDsmRev  = ACPI_MXM_REVISION_ID;
            break;
        case ACPI_DSM_FUNCTION_NBCI:
            pAcpiDsmGuid = (NvU8 *) &NBCI_DSM_GUID;
            acpiDsmRev  = NBCI_REVISION_ID;
            break;
        case ACPI_DSM_FUNCTION_NVOP:
            pAcpiDsmGuid = (NvU8 *) &NVOP_DSM_GUID;
            acpiDsmRev  = NVOP_REVISION_ID;
            break;
        case ACPI_DSM_FUNCTION_PCFG:
            pAcpiDsmGuid = (NvU8 *) &PCFG_DSM_GUID;
            acpiDsmRev  = PCFG_REVISION_ID;
            break;
        case ACPI_DSM_FUNCTION_GPS_2X:
            pAcpiDsmGuid = (NvU8 *) &GPS_DSM_GUID;
            acpiDsmRev = GPS_2X_REVISION_ID;
            acpiDsmInArgSize = (*pSize);
            break;
        case ACPI_DSM_FUNCTION_GPS:
            if ((IsTU10X(pGpu)) ||
                ((gpuIsACPIPatchRequiredForBug2473619_HAL(pGpu)) &&
                 ((acpiDsmSubFunction == GPS_FUNC_SUPPORT) ||
                  (acpiDsmSubFunction == GPS_FUNC_GETCALLBACKS))))
            {
                pAcpiDsmGuid = (NvU8 *) &GPS_DSM_GUID;
                acpiDsmRev = GPS_REVISION_ID;
                acpiDsmInArgSize = (*pSize);
            }
            else
            {
                return NV_ERR_NOT_SUPPORTED;
            }
            break;
        case ACPI_DSM_FUNCTION_PEX:
            pAcpiDsmGuid = (NvU8 *) &PEX_DSM_GUID;
            acpiDsmRev   = PEX_REVISION_ID;
            if (acpiDsmSubFunction == PEX_FUNC_SETLTRLATENCY)
            {
                acpiDsmInArgSize = (3 + *pSize);
            }
            break;
        case (ACPI_DSM_FUNCTION_JT):
            pAcpiDsmGuid = (NvU8 *) &JT_DSM_GUID;
            acpiDsmRev = JT_REVISION_ID;
            break;
        case ACPI_DSM_FUNCTION_NVPCF:
            {
                pAcpiDsmGuid = (NvU8 *)&NVPCF_ACPI_DSM_GUID;
                acpiDsmRev = NVPCF_ACPI_DSM_REVISION_ID;
                acpiDsmInArgSize = (*pSize);
                acpiNvpcfDsmFunction = NV_TRUE;
                break;
            }
        case ACPI_DSM_FUNCTION_NVPCF_2X:
            pAcpiDsmGuid = (NvU8 *)&NVPCF_ACPI_DSM_GUID;
            acpiDsmRev = NVPCF_2X_ACPI_DSM_REVISION_ID;
            acpiDsmInArgSize = (*pSize);
            if (!nv->nvpcf_dsm_in_gpu_scope)
            {
                acpiNvpcfDsmFunction = NV_TRUE;
            }
            break;

        default:
            return NV_ERR_NOT_SUPPORTED;
            break;
    }

    status = nv_acpi_dsm_method(nv,
                                pAcpiDsmGuid,
                                acpiDsmRev,
                                acpiNvpcfDsmFunction,
                                acpiDsmSubFunction,
                                pInOut,
                                acpiDsmInArgSize,
                                NULL,
                                pInOut,
                                pSize);

    if (status == NV_OK)
    {
        if (acpiDsmSubFunction == NV_ACPI_ALL_FUNC_SUPPORT)
        {
            // if handling get supported functions list... cache it for later calls
            cacheDsmSupportedFunction(pGpu, acpiDsmFunction, acpiDsmSubFunction, pInOut, *pSize);
        }
    }
    else if (nvp->b_mobile_config_enabled)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "osCallACPI_DSM: Error during 0x%x DSM subfunction 0x%x! status=0x%x\n",
                  acpiDsmFunction, acpiDsmSubFunction, status);
    }

    return status;
}

NV_STATUS osCallACPI_DOD
(
    OBJGPU  *pGpu,
    NvU32   *pOut,
    NvU32   *pSize
)
{
    NV_STATUS rmStatus;
    nv_state_t *nv = NV_GET_NV_STATE(pGpu);

    if ((pOut == NULL) || (pSize == NULL))
    {
        return NV_ERR_INVALID_POINTER;
    }

    rmStatus = nv_acpi_dod_method(nv, pOut, pSize);

    return rmStatus;
}

//
// osAcpiDdc
//
// Handles os specific _DDC method function calls. _DDC is to get EDID from SBIOS.
//
NV_STATUS osCallACPI_DDC
(
    OBJGPU *pGpu,
    NvU32   ulAcpiId,
    NvU8   *pOutData,
    NvU32  *pOutSize,
    NvBool  bReadMultiBlock
)
{
    NV_STATUS rmStatus;

    nv_state_t *nv = NV_GET_NV_STATE(pGpu);

    if ((pOutData == NULL) || (pOutSize == NULL))
    {
        return NV_ERR_INVALID_POINTER;
    }

    portMemSet(pOutData, 0, *pOutSize);

    rmStatus = nv_acpi_ddc_method(nv, pOutData, pOutSize, bReadMultiBlock);

    return rmStatus;
}

// osCallACPI_NVHG_ROM
// Making ACPI Call into SBIOS with ROM method to get display device's ROM data.
//
NV_STATUS  osCallACPI_NVHG_ROM
(
    OBJGPU *pGpu,
    NvU32 *pInData,
    NvU32 *pOutData
)
{
    NV_STATUS rmStatus;
    nv_state_t *nv = NV_GET_NV_STATE(pGpu);

    if ((pOutData == NULL) || (pInData == NULL))
    {
        return NV_ERR_INVALID_POINTER;
    }

    if (pInData[1] > ROM_METHOD_MAX_RETURN_BUFFER_SIZE)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    rmStatus = nv_acpi_rom_method(nv, pInData, pOutData);

    return rmStatus;
}

void osInitSystemStaticConfig(SYS_STATIC_CONFIG *pConfig)
{
    pConfig->bIsNotebook = rm_is_system_notebook();
    pConfig->bOsCCEnabled = os_cc_enabled;
    pConfig->bOsCCTdxEnabled = os_cc_tdx_enabled;
}

NvU32 osApiLockAcquireConfigureFlags(NvU32 flags)
{
    return flags;
}

NV_STATUS osGpuLocksQueueRelease(OBJGPU *pGpu, NvU32 dpcGpuLocksRelease)
{
    return NV_SEMA_RELEASE_FAILED;
}

void osSyncWithRmDestroy(void)
{
}

void osSyncWithGpuDestroy(NvBool bEntry)
{
}

void osModifyGpuSwStatePersistence
(
    OS_GPU_INFO *pOsGpuInfo,
    NvBool       bEnable
)
{
    if (bEnable)
    {
        pOsGpuInfo->flags |= NV_FLAG_PERSISTENT_SW_STATE;
    }
    else
    {
        pOsGpuInfo->flags &= ~NV_FLAG_PERSISTENT_SW_STATE;
    }
}

//
//osCallACPI_MXDS
//
//Handles OS specific MXDS function call.
//
NV_STATUS osCallACPI_MXDS
(
    OBJGPU *pGpu,
    NvU32 acpiId,
    NvU32 *pInOut
)
{
    NV_STATUS rmStatus;

    nv_state_t *nv = NV_GET_NV_STATE(pGpu);

    if (pInOut == NULL)
    {
        return NV_ERR_INVALID_POINTER;
    }

    rmStatus = nv_acpi_mux_method(nv, pInOut, acpiId, "MXDS");

    return rmStatus;
}

//
//osCallACPI_MXDM
//
//Handles OS specific MXDM function call.
//
NV_STATUS osCallACPI_MXDM
(
    OBJGPU *pGpu,
    NvU32 acpiId,
    NvU32 *pInOut
)
{
    NV_STATUS rmStatus;

    nv_state_t *nv = NV_GET_NV_STATE(pGpu);

    if (pInOut == NULL)
    {
        return NV_ERR_INVALID_POINTER;
    }

    rmStatus = nv_acpi_mux_method(nv, pInOut, acpiId, "MXDM");

    return rmStatus;
}

#include "lib/protobuf/prb.h"
#include "lib/protobuf/prb_util.h"
#include "g_nvdebug_pb.h"

NV_STATUS osGetVersionDump(void * pVoid)
{
    PRB_ENCODER * pPrbEnc = (PRB_ENCODER *)pVoid;
    NV_STATUS rmStatus;
    os_version_info * pOsVersionInfo = NULL;
    const char NV_UNKNOWN_BUILD_VERSION[] = "Unknown build version";
    const char NV_UNKNOWN_BUILD_DATE[]    = "Unknown build date";

    NV_ASSERT_OK_OR_RETURN(os_alloc_mem((void**)&pOsVersionInfo,
                                        sizeof(os_version_info)));
    portMemSet(pOsVersionInfo, 0, sizeof(os_version_info));

    prbEncAddUInt32(pPrbEnc,
                    NVDEBUG_SYSTEMINFO_OSINFO_FAMILY,
                    NVDEBUG_OS_UNIX);

    rmStatus = os_get_version_info(pOsVersionInfo);
    if (rmStatus != NV_OK)
    {
        goto cleanup;
    }

    prbEncAddUInt32(pPrbEnc,
                    NVDEBUG_SYSTEMINFO_OSINFO_OSMAJORVERSION,
                    pOsVersionInfo->os_major_version);

    prbEncAddUInt32(pPrbEnc,
                    NVDEBUG_SYSTEMINFO_OSINFO_OSMINORVERSION,
                    pOsVersionInfo->os_minor_version);

    prbEncAddUInt32(pPrbEnc,
                    NVDEBUG_SYSTEMINFO_OSINFO_OSBLDNUM,
                    pOsVersionInfo->os_build_number);

    if (NULL == pOsVersionInfo->os_build_version_str)
    {
        pOsVersionInfo->os_build_version_str = NV_UNKNOWN_BUILD_VERSION;
    }

    prbEncAddString(pPrbEnc,
                    NVDEBUG_SYSTEMINFO_OSINFO_BUILDVERSION,
                    pOsVersionInfo->os_build_version_str);

    if (NULL == pOsVersionInfo->os_build_date_plus_str)
    {
        pOsVersionInfo->os_build_date_plus_str = NV_UNKNOWN_BUILD_DATE;
    }

    prbEncAddString(pPrbEnc,
                    NVDEBUG_SYSTEMINFO_OSINFO_BUILDDATEPLUS,
                    pOsVersionInfo->os_build_date_plus_str);

cleanup:
    os_free_mem(pOsVersionInfo);
    return rmStatus;
}

NV_STATUS osGetVersion(NvU32 *majorVer, NvU32 *minorVer, NvU32 *buildNum, NvU16 *unusedPatchVersion, NvU16 *unusedProductType)
{
    os_version_info osVersionInfo;
    NV_STATUS rmStatus;

    portMemSet(&osVersionInfo, 0, sizeof(osVersionInfo));

    rmStatus = os_get_version_info(&osVersionInfo);
    if (rmStatus == NV_OK)
    {
        if (majorVer)
            *majorVer = osVersionInfo.os_major_version;
        if (minorVer)
            *minorVer = osVersionInfo.os_minor_version;
        if (buildNum)
            *buildNum = osVersionInfo.os_build_number;
    }

    return rmStatus;
}

NV_STATUS osGetIsOpenRM(NvBool *bOpenRm)
{
    return os_get_is_openrm(bOpenRm);
}

NV_STATUS
osGetCarveoutInfo
(
    NvU64 *pAddr,
    NvU64 *pSize
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
osGetVPRInfo
(
    NvU64 *pAddr,
    NvU64 *pSize
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
osAllocInVPR
(
    MEMORY_DESCRIPTOR *pMemDesc
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
osGetGenCarveout
(
    NvU64  *pAddr,
    NvU64  *pSize,
    NvU32   id,
    NvU64   align
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
osI2CClosePorts
(
    OS_GPU_INFO *pOsGpuInfo,
    NvU32        numPorts
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
osI2CTransfer
(
    OBJGPU *pGpu,
    NvU32 Port,
    NvU8 Address,
    nv_i2c_msg_t *nv_i2c_msgs,
    NvU32 count
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
osTegraI2CGetBusState
(
    OS_GPU_INFO *pOsGpuInfo,
    NvU32 port,
    NvS32 *scl,
    NvS32 *sda
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
osReadI2CBufferDirect
(
    OBJGPU *pGpu,
    NvU32   Port,
    NvU8    Address,
    void   *pOutputBuffer,
    NvU32   OutputSize,
    void   *pInputBuffer,
    NvU32   InputSize
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
osWriteI2CBufferDirect
(
    OBJGPU  *pGpu,
    NvU32    Port,
    NvU8     Address,
    void    *pOutputBuffer0,
    NvU32    OutputSize0,
    void    *pOutputBuffer1,
    NvU32    OutputSize1
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
osGC6PowerControl
(
    OBJGPU *pGpu,
    NvU32   cmd,
    NvU32  *pOut
)
{
    NV_STATUS status;
    NvU32 inOut   = cmd;
    NvU16 rtnSize = sizeof(inOut);

    if (FLD_TEST_DRF(_JT_FUNC, _POWERCONTROL, _GPU_POWER_CONTROL, _GSS, inOut))
    {
        if (!pOut)
        {
            return NV_ERR_INVALID_ARGUMENT;
        }
    }

    status = osCallACPI_DSM(pGpu,
                            ACPI_DSM_FUNCTION_JT,
                            JT_FUNC_POWERCONTROL,
                            &inOut,
                            &rtnSize);

    if ((status != NV_OK) || !pOut)
    {
        return status;
    }

    *pOut = inOut;

    return NV_OK;
}

NvBool osTestPcieExtendedConfigAccess(void *handle, NvU32 offset)
{
    OBJGPU    *pGpu;
    KernelBif *pKernelBif;
    NvU32      nvXveId       = 0;
    NvU32      nvXveVccapHdr = 0;
    NvU32      pciStart      = 0;
    NvU32      pcieStart     = 0;

    static NvBool retryAllowed = NV_TRUE;
    static NvBool configAccess = NV_FALSE;

    //
    // Return early for offset within PCI space
    // and does not requires extended config space access
    //
    if (offset < 0x100)
    {
        return NV_TRUE;
    }

    if (!retryAllowed)
    {
        return configAccess;
    }

    pGpu = gpumgrGetSomeGpu();
    if (pGpu == NULL)
    {
        return configAccess;
    }

    retryAllowed = NV_FALSE;

    pKernelBif = GPU_GET_KERNEL_BIF(pGpu);
    if (pKernelBif == NULL || kbifGetBusIntfType_HAL(pKernelBif) !=
                                  NV2080_CTRL_BUS_INFO_TYPE_PCI_EXPRESS)
    {
        return configAccess;
    }

    // Now verify PCI and PCIe config start registers.
    kbifGetPcieConfigAccessTestRegisters_HAL(pGpu, pKernelBif, &pciStart, &pcieStart);
    os_pci_read_dword(handle, pciStart,  &nvXveId);
    os_pci_read_dword(handle, pcieStart, &nvXveVccapHdr);

    if (NV_OK == kbifVerifyPcieConfigAccessTestRegisters_HAL(pGpu,
                                                             pKernelBif,
                                                             nvXveId,
                                                             nvXveVccapHdr))
    {
        configAccess = NV_TRUE;
    }

    return configAccess;
}

/*!
 * @brief Map memory into an IOVA space according to the given mapping info.
 *
 * @param[in]   pIovaMapping    IOVA mapping info
 *
 * @return      NV_ERR_NOT_SUPPORTED
 */
NV_STATUS
osIovaMap
(
    PIOVAMAPPING pIovaMapping
)
{
    OBJGPU *pGpu;
    nv_state_t *nv, *peer;
    NV_STATUS status;
    RmPhysAddr base;
    NvBool bIsBar0;
    PMEMORY_DESCRIPTOR pRootMemDesc;
    NvBool bIsFbOffset = NV_FALSE;
    NvBool bIsIndirectPeerMapping = NV_FALSE;
    NvBool bIsContig;
    NV_ADDRESS_SPACE addressSpace;
    NvU32 osPageCount;

    if (pIovaMapping == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    pGpu = gpumgrGetGpuFromId(pIovaMapping->iovaspaceId);
    if (pGpu == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    pRootMemDesc = memdescGetRootMemDesc(pIovaMapping->pPhysMemDesc, NULL);
    addressSpace = memdescGetAddressSpace(pIovaMapping->pPhysMemDesc);
    if (gpumgrCheckIndirectPeer(pGpu, pRootMemDesc->pGpu) &&
        (addressSpace == ADDR_FBMEM))
    {
        bIsIndirectPeerMapping = NV_TRUE;
    }

    if ((addressSpace != ADDR_SYSMEM) && !bIsIndirectPeerMapping)
    {
        NV_PRINTF(LEVEL_INFO,
                  "%s passed memory descriptor in an unsupported address space (%s)\n",
                  __FUNCTION__,
                  memdescGetApertureString(memdescGetAddressSpace(pIovaMapping->pPhysMemDesc)));
        return NV_ERR_NOT_SUPPORTED;
    }

    //
    // For guest-allocated memory, we don't actually want to do any remapping,
    // since the physical address is already the DMA address to be used by the
    // GPU.
    //
    if (memdescGetFlag(pIovaMapping->pPhysMemDesc, MEMDESC_FLAGS_GUEST_ALLOCATED))
    {
        return NV_OK;
    }

    nv = NV_GET_NV_STATE(pGpu);

    //
    // Intercept peer IO type memory. These are contiguous allocations, so no
    // need to adjust pages.
    //
    if (memdescGetFlag(pIovaMapping->pPhysMemDesc, MEMDESC_FLAGS_PEER_IO_MEM))
    {
        NV_ASSERT(memdescGetContiguity(pIovaMapping->pPhysMemDesc, AT_CPU));

        status = nv_dma_map_mmio(nv->dma_dev,
            NV_RM_PAGES_TO_OS_PAGES(pIovaMapping->pPhysMemDesc->PageCount),
            &pIovaMapping->iovaArray[0]);

        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "%s: failed to map peer IO mem (status = 0x%x)\n",
                      __FUNCTION__, status);
        }

        return status;
    }

    //
    // We need to check against the "root" GPU, e.g., the GPU that owns this
    // allocation. If we're trying to map one of its BARs for a peer, we need
    // to handle it differently because it wouldn't have gone through our system
    // memory page allocation paths, obviously, and wouldn't have alloc private
    // data associated with it.
    //
    peer = NV_GET_NV_STATE(pRootMemDesc->pGpu);
    bIsContig = memdescGetContiguity(pIovaMapping->pPhysMemDesc, AT_CPU);
    if (NV_RM_PAGE_SIZE < os_page_size && !bIsContig)
    {
        RmDeflateRmToOsPageArray(&pIovaMapping->iovaArray[0],
                                 pIovaMapping->pPhysMemDesc->PageCount);
    }

    base = memdescGetPhysAddr(pIovaMapping->pPhysMemDesc, AT_CPU, 0);
    bIsBar0 = IS_REG_OFFSET(peer, base, pIovaMapping->pPhysMemDesc->Size);

    bIsFbOffset = IS_FB_OFFSET(peer, base, pIovaMapping->pPhysMemDesc->Size);

    //
    // For indirect peers bIsFbOffset should be NV_TRUE
    // TODO:IS_FB_OFFSET macro is currently broken for P9 systems
    // Bug 2010857 tracks fixing this
    //
#if defined(NVCPU_PPC64LE)
    KernelMemorySystem *pRootKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pRootMemDesc->pGpu);
    if (bIsIndirectPeerMapping)
    {
        //
        // If the first page from the memdesc is in FB then the remaining pages
        // should also be in FB. If the memdesc is contiguous, check that it is
        // contained within the coherent CPU FB range. memdescGetNvLinkGpa()
        // will check that each page is in FB to handle the discontiguous case.
        //
        NvU64 atsBase = base + pRootKernelMemorySystem->coherentCpuFbBase;
        NvU64 atsEnd = bIsContig ? (atsBase + pIovaMapping->pPhysMemDesc->Size) : atsBase;
        if ((atsBase >= pRootKernelMemorySystem->coherentCpuFbBase) &&
             (atsEnd <= pRootKernelMemorySystem->coherentCpuFbEnd))
        {
            bIsFbOffset = NV_TRUE;
        }
        else
        {
            NV_ASSERT_OR_RETURN(0, NV_ERR_INVALID_STATE);
        }
    }
#endif

    void *pPriv = memdescGetMemData(pIovaMapping->pPhysMemDesc);
    osPageCount = NV_RM_PAGES_TO_OS_PAGES(pIovaMapping->pPhysMemDesc->PageCount);

    if (!bIsBar0 && !bIsFbOffset)
    {
        if (pPriv == NULL)
        {
            return NV_ERR_INVALID_STATE;
        }
    }
    else if(bIsIndirectPeerMapping)
    {
        NV_ASSERT(!bIsBar0 && bIsFbOffset);
        //
        // TODO: Align onlined GPU memory allocation paths with system memory allocation
        //       That way pMemDesc->pMemData is setup correctly when we try to create mapping
        //       to onlined memory of indirect peer. After that we can also get rid of some
        //       extra code in nv_dma_map_alloc. See bug 190324 for details
        //

        status = memdescGetNvLinkGpa(pRootMemDesc->pGpu, (bIsContig ? 1 : osPageCount),
                                    &pIovaMapping->iovaArray[0]);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "%s Failed to get SPA\n", __FUNCTION__);
            return status;
        }
    }

    if (!bIsBar0 && (!bIsFbOffset || bIsIndirectPeerMapping))
    {
        status = nv_dma_map_alloc(
                    osGetDmaDeviceForMemDesc(nv, pIovaMapping->pPhysMemDesc),
                    osPageCount,
                    &pIovaMapping->iovaArray[0],
                    bIsContig, &pPriv);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "%s: failed to map allocation (status = 0x%x)\n",
                      __FUNCTION__, status);
            return status;
        }

        pIovaMapping->pOsData = pPriv;
    }
    else if (peer != nv)
    {
        status = nv_dma_map_peer(nv->dma_dev, peer->dma_dev, bIsBar0 ? 0 : 1,
                                 osPageCount, &pIovaMapping->iovaArray[0]);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_INFO,
                      "%s: failed to map peer (base = 0x%llx, status = 0x%x)\n",
                      __FUNCTION__, base, status);
            return status;
        }

        //
        // pOsData must be NULL to distinguish a peer DMA mapping from a
        // system memory mapping in osIovaUnmap(), so make sure to set it
        // accordingly here.
        //
        pIovaMapping->pOsData = NULL;
    }
    else
    {
        NV_PRINTF(LEVEL_INFO, "cannot map a GPU's BAR to itself\n");
        return NV_ERR_NOT_SUPPORTED;
    }

    //
    // If the OS layer doesn't think in RM page size, we need to inflate the
    // PTE array into RM pages.
    //
    if (NV_RM_PAGE_SIZE < os_page_size && !bIsContig)
    {
        RmInflateOsToRmPageArray(&pIovaMapping->iovaArray[0],
                                 pIovaMapping->pPhysMemDesc->PageCount);
    }

    return NV_OK;
}

/*!
 * @brief Unmap memory from an IOVA space according to the given mapping info.
 *
 * This mapping info must have been previously mapped by osIovaMap().
 *
 * @param[in]   pIovaMapping    IOVA mapping info
 *
 */
void
osIovaUnmap
(
    PIOVAMAPPING pIovaMapping
)
{
    OBJGPU *pGpu;
    nv_state_t *nv;
    void *pPriv;
    NV_STATUS status;

    if (pIovaMapping == NULL)
    {
        return;
    }

    pGpu = gpumgrGetGpuFromId(pIovaMapping->iovaspaceId);
    if (pGpu == NULL)
    {
        return;
    }

    //
    // For guest-allocated memory, we never actually remapped the memory, so we
    // shouldn't try to unmap it here.
    //
    if (memdescGetFlag(pIovaMapping->pPhysMemDesc, MEMDESC_FLAGS_GUEST_ALLOCATED))
    {
        return;
    }

    nv = NV_GET_NV_STATE(pGpu);

    if (memdescGetFlag(pIovaMapping->pPhysMemDesc, MEMDESC_FLAGS_PEER_IO_MEM))
    {
        nv_dma_unmap_mmio(nv->dma_dev,
            NV_RM_PAGES_TO_OS_PAGES(pIovaMapping->pPhysMemDesc->PageCount),
            pIovaMapping->iovaArray[0]);

        return;
    }

    //
    // TODO: Formalize the interface with the OS layers so we can use a common
    // definition of OS_IOVA_MAPPING_DATA.
    //
    pPriv = (void *)pIovaMapping->pOsData;

    if (NV_RM_PAGE_SIZE < os_page_size &&
        !memdescGetContiguity(pIovaMapping->pPhysMemDesc, AT_CPU))
    {
        RmDeflateRmToOsPageArray(&pIovaMapping->iovaArray[0],
                                 pIovaMapping->pPhysMemDesc->PageCount);
    }

    if (pPriv != NULL)
    {
        status = nv_dma_unmap_alloc(nv->dma_dev,
            NV_RM_PAGES_TO_OS_PAGES(pIovaMapping->pPhysMemDesc->PageCount),
            &pIovaMapping->iovaArray[0], &pPriv);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "%s: failed to unmap allocation (status = 0x%x)\n",
                      __FUNCTION__, status);
        }
    }
    else
    {
        nv_dma_unmap_peer(nv->dma_dev,
            NV_RM_PAGES_TO_OS_PAGES(pIovaMapping->pPhysMemDesc->PageCount),
            pIovaMapping->iovaArray[0]);
    }

    //
    // If the OS layer doesn't think in RM page size, we need to fluff out the
    // PTE array into RM pages.
    //
    if (NV_RM_PAGE_SIZE < os_page_size &&
        !memdescGetContiguity(pIovaMapping->pPhysMemDesc, AT_CPU))
    {
        RmInflateOsToRmPageArray(&pIovaMapping->iovaArray[0],
                                 pIovaMapping->pPhysMemDesc->PageCount);
    }

    pIovaMapping->pOsData = NULL;
}

/*!
 * @brief Set the GPU Rail Voltage in Tegra SoC. Currently not supported
 *
 * @param[in]  pGpu            GPU object pointer
 * @param[in]  reqVoltageuV    Rail Voltage requested in uV
 * @param[out] pSetVoltageuV   Rail Voltage set in uV
 *
 * @return    NV_ERR_NOT_SUPPORTED
 */
NV_STATUS
osSetGpuRailVoltage
(
   OBJGPU   *pGpu,
   NvU32     reqVoltageuV,
   NvU32    *pSetVoltageuV
)
{
    return NV_ERR_NOT_SUPPORTED;
}

/*!
 * @brief Get the GPU Rail Voltage in Tegra SoC. Currently not supported
 *
 * @param[in]  pGpu        GPU object pointer
 * @param[out] voltageuV   Rail Voltage in uV
 *
 * @return    NV_ERR_NOT_SUPPORTED
 */
NV_STATUS
osGetGpuRailVoltage
(
    OBJGPU   *pGpu,
    NvU32    *pVoltageuV
)
{
    return NV_ERR_NOT_SUPPORTED;
}

/*!
 * @brief Bring down system in a controlled manner on known error conditions.
 *
 * @bugCode[in] Error code / reason.
 */
void osBugCheck(NvU32 bugCode)
{
    if (bugCode > OS_BUG_CHECK_BUGCODE_LAST)
    {
        bugCode = OS_BUG_CHECK_BUGCODE_UNKNOWN;
    }

    os_bug_check(bugCode, ppOsBugCheckBugcodeStr[bugCode]);
}

/*!
 * @brief Perform an action at assertion failure.
 */
void osAssertFailed(void)
{
    os_dump_stack();
}

/*!
 * @brief Get the GPU Chip Info - Speedo and IDDQ values
 *
 *
 * @param[in]   pGpu           GPU object pointer
 * @param[out]  pGpuSpeedoHv   Pointer to GPU Speedo value at high voltage corner.
 * @param[out]  pGpuSpeedoLv   Pointer to GPU Speedo value at low voltage corner.
 * @param[out]  pGpuIddq       Pointer to GPU Iddq Value
 * @param[out]  pChipSkuId     SKU ID for the chip
 *
 * @return     NV_ERR_NOT_SUPPORTED
 */
NV_STATUS
osGetChipInfo
(
    OBJGPU   *pGpu,
    NvU32    *pGpuSpeedoHv,
    NvU32    *pGpuSpeedoLv,
    NvU32    *pGpuIddq,
    NvU32    *pChipSkuId
)
{
    return NV_ERR_NOT_SUPPORTED;
}

/*
 * @brief Get the GPU Rail Voltage Info (i.e. Min, Max and StepSize) in Tegra SoC.
 *
 * @param[in]  pGpu            GPU object pointer
 * @param[out] pMinVoltageuV   Minimum Voltage supported on the Rail in Micro Volts
 * @param[out] pMaxVoltageuV   Maximum Voltage supported on the Rail in Micro Volts
 * @param[out] pStepVoltageuV  Voltage Step-size supported on the Rail in Micro Volts
 *
 * @return    NV_ERR_NOT_SUPPORTED
 */
NV_STATUS
osGetGpuRailVoltageInfo
(
    OBJGPU    *pGpu,
    NvU32     *pMinVoltageuV,
    NvU32     *pMaxVoltageuV,
    NvU32     *pStepVoltageuV
)
{
    return NV_ERR_NOT_SUPPORTED;
}

/*!
 * @brief Get the current opaque security token.
 *
 * For Linux the security token is the effective UID of a process and process ID
 *
 * Note: This function allocates memory for the token. The onus is on the calling
 * function to free the memory associated with the token once its done with it.
 *
 * @return pointer to the security token.
 */
PSECURITY_TOKEN
osGetSecurityToken(void)
{
    NV_STATUS rmStatus;
    TOKEN_USER *pTokenUser;

    pTokenUser = portMemAllocNonPaged(sizeof(TOKEN_USER));
    if (pTokenUser == NULL)
    {
        return NULL;
    }
    rmStatus = os_get_euid(&pTokenUser->euid);
    if (rmStatus != NV_OK)
    {
        portMemFree(pTokenUser);
        return NULL;
    }

    pTokenUser->pid = os_get_current_process();

    return (PSECURITY_TOKEN)pTokenUser;
}

PUID_TOKEN
osGetCurrentUidToken(void)
{
    NV_STATUS rmStatus;
    NvU32 *pUidToken;

    pUidToken = portMemAllocNonPaged(sizeof(NvU32));
    if (pUidToken == NULL)
    {
        return NULL;
    }

    rmStatus = os_get_euid(pUidToken);
    if (rmStatus != NV_OK)
    {
        portMemFree(pUidToken);
        return NULL;
    }

    return (PUID_TOKEN)pUidToken;
}

/*!
 * @brief Interface function to validate the token for the current client
 *
 * This function takes two tokens as parameters, validates them and checks
 * if either the PID or EUID from client database matches the current PID or EUID.
 *
 * @param[in] pClientSecurityToken   security token cached in the client db
 * @param[in] pCurrentSecurityToken  security token of the current client
 * @return             NV_OK    if the validation is successful
 *                     NV_ERR_INVALID_CLIENT if the tokens do not match
 *                     NV_ERR_INVALID_POINTER if the tokens are invalid
 */
NV_STATUS
osValidateClientTokens
(
   PSECURITY_TOKEN pClientSecurityToken,
   PSECURITY_TOKEN pCurrentSecurityToken
)
{
    PTOKEN_USER  pClientTokenUser  = (PTOKEN_USER)pClientSecurityToken;
    PTOKEN_USER  pCurrentTokenUser = (PTOKEN_USER)pCurrentSecurityToken;

    if (pClientTokenUser == NULL || pCurrentTokenUser == NULL)
        return NV_ERR_INVALID_POINTER;

    if ((pClientTokenUser->euid != pCurrentTokenUser->euid) &&
        (pClientTokenUser->pid != pCurrentTokenUser->pid))
    {
        NV_PRINTF(LEVEL_INFO,
                  "NVRM: %s: Current security token doesn't match the one in the client database. "
                  "Current EUID: %d, PID: %d; Client DB EUID: %d, PID: %d\n",
                  __FUNCTION__, pCurrentTokenUser->euid, pCurrentTokenUser->pid,
                  pClientTokenUser->euid, pClientTokenUser->pid);
        return NV_ERR_INVALID_CLIENT;
    }

    return NV_OK;
}

/*!
 * @brief Interface function to compare the tokens for two client
 *
 * This function takes two tokens as parameters, validates them and checks
 * if the EUIDs of each token match.
 *
 * @param[in] pToken1  Token to compare
 * @param[in] pToken2  Token to compare
 * @return             NV_TRUE if the tokens match
 *                     NV_FALSE if the tokens do not match
 */
NvBool
osUidTokensEqual
(
   PUID_TOKEN pUidToken1,
   PUID_TOKEN pUidToken2
)
{
    NvU32 * pTokenUser1 = (NvU32*)pUidToken1;
    NvU32 * pTokenUser2 = (NvU32*)pUidToken2;

    NV_ASSERT_OR_RETURN((pTokenUser1 != NULL), NV_FALSE);
    NV_ASSERT_OR_RETURN((pTokenUser2 != NULL), NV_FALSE);

    if (*pTokenUser1 != *pTokenUser2)
    {
        return NV_FALSE;
    }

    return NV_TRUE;
}

NvBool
osRemoveGpuSupported
(
    void
)
{
    return os_pci_remove_supported();
}

/*
 * @brief Get the address ranges assigned to local or peer GPUs on a system that
 * supports hardware address translation services (ATS) over NVLink/C2C.
 *
 * @note
 * - All address values are in the System Physical Address (SPA) space
 * - Targets can either be "Local" (bIsPeer=False) or for a specified "Peer"
 *   (bIsPeer=True, peerIndex=#) GPU
 * - Target address and mask values have a specified bit width, and represent
 *   the higher order bits above the target address granularity
 *
 * @param[in]   pGpu                GPU object pointer
 * @param[out]  pAddrSysPhys        Pointer to hold SPA
 * @param[out]  pAddrWidth          Address range width value pointer
 * @param[out]  pMask               Mask value pointer
 * @param[out]  pMaskWidth          Mask width value pointer
 * @param[in]   bIsPeer             NV_TRUE if this is a peer, local GPU otherwise
 * @param[in]   peerIndex           Peer index
 *
 * @return      NV_OK or NV_ERR_NOT_SUPPORTED
 *
 *              A return value of NV_ERR_NOT_SUPPORTED for the local GPU would
 *              indicate that the system does not support ATS over NVLink/C2C
 */
NV_STATUS
osGetAtsTargetAddressRange
(
    OBJGPU *pGpu,
    NvU64   *pAddrSysPhys,
    NvU32   *pAddrWidth,
    NvU32   *pMask,
    NvU32   *pMaskWidth,
    NvBool  bIsPeer,
    NvU32   peerIndex
)
{
#if RMCFG_MODULE_KERNEL_BIF && RMCFG_MODULE_KERNEL_NVLINK && (defined(NVCPU_PPC64LE) || defined(NVCPU_AARCH64))
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);
    KernelBif    *pKernelBif    = GPU_GET_KERNEL_BIF(pGpu);
    nv_state_t   *nv;
    const int addrMaskWidth = 0x10;

    if (!pKernelNvlink && !pKernelBif->getProperty(pKernelBif, PDB_PROP_KBIF_IS_C2C_LINK_UP))
        return NV_ERR_INVALID_ARGUMENT;

    nv = NV_GET_NV_STATE(pGpu);

    //
    // TODO : Bug 1848958 restricts peer device tree parsing. Return early if
    // peer values are requested. This should be fixed by passing correct pGpu
    // pointer of the peer GPU retrieved using peerIds.
    //
    if (bIsPeer)
    {
        const int addrWidth = 0x10;

        *pAddrSysPhys = 0;
        *pAddrWidth = addrWidth;
        *pMask = 0;
        *pMaskWidth = addrMaskWidth;
        return NV_OK;
    }
    else
    {
        NV_STATUS status = nv_get_device_memory_config(nv, pAddrSysPhys, NULL, NULL,
                                                       pAddrWidth, NULL);
        if (status == NV_OK)
        {
            *pMask = NVBIT(*pAddrWidth) - 1U;
            *pMaskWidth = addrMaskWidth;
        }
        return status;
    }

    return NV_OK;
#endif
    return NV_ERR_NOT_SUPPORTED;
}

/*
 * @brief Get the physical address in CPU address map and NUMA node id
 * of the GPU memory.
 *
 * @note
 * - The physical address is System Physical Address (SPA) in baremetal/host
 *   and Intermediate Physical Address(IPA) or Guest Physical Address(GPA)
 *   inside a VM.
 *
 * @param[in]   pGpu             GPU object pointer
 * @param[out]  pAddrPhys        Pointer to hold the physical address of FB in
 *                               CPU address map
 * @param[out]  pNodeId          NUMA nodeID of respective GPU memory
 *
 * @return      NV_OK or NV_ERR_NOT_SUPPORTED
 *
 */
NV_STATUS
osGetFbNumaInfo
(
    OBJGPU *pGpu,
    NvU64  *pAddrPhys,
    NvU64  *pAddrRsvdPhys,
    NvS32  *pNodeId
)
{
#if RMCFG_MODULE_KERNEL_BIF && RMCFG_MODULE_KERNEL_NVLINK && (defined(NVCPU_PPC64LE) || defined(NVCPU_AARCH64))
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);
    KernelBif    *pKernelBif    = GPU_GET_KERNEL_BIF(pGpu);
    nv_state_t   *nv;

    *pNodeId = NV0000_CTRL_NO_NUMA_NODE;

    if (!pKernelNvlink && !pKernelBif->getProperty(pKernelBif, PDB_PROP_KBIF_IS_C2C_LINK_UP))
        return NV_ERR_INVALID_ARGUMENT;

    nv = NV_GET_NV_STATE(pGpu);

    NV_STATUS status = nv_get_device_memory_config(nv, NULL, pAddrPhys,
                                                   pAddrRsvdPhys, NULL, pNodeId);

    return status;
#endif
    return NV_ERR_NOT_SUPPORTED;
}


/*
 * @brief Verif only function to get the chiplib overrides for link connection
 *        state for all C2C links.
 *
 * If chiplib overrides exist, each link can either be enabled (1) or disabled (0)
 *
 * @param[in]   pGpu                GPU object pointer
 * @param[in]   maxLinks            Size of pLinkConnection array
 * @param[out]  pLinkConnection     array of pLinkConnection values to be populated by MODS
 *
 * @return      NV_OK or NV_ERR_NOT_SUPPORTED (no overrides available)
 */
NV_STATUS
osGetForcedC2CConnection
(
    OBJGPU *pGpu,
    NvU32   maxLinks,
    NvU32   *pLinkConnection
)
{
    int i, ret;
    NV_STATUS status;
    char path[64];

    NV_ASSERT_OR_RETURN((pLinkConnection != NULL), NV_ERR_INVALID_POINTER);
    NV_ASSERT_OR_RETURN((maxLinks > 0), NV_ERR_NOT_SUPPORTED);
    NV_ASSERT_OR_RETURN((pGpu != NULL), NV_ERR_INVALID_ARGUMENT);

    for (i = 0; i < maxLinks; i++)
    {
        ret = os_snprintf(path, sizeof(path), "CPU_MODEL|CM_ATS_ADDRESS|C2C%u", i);
        NV_ASSERT((ret > 0) && (ret < (sizeof(path) - 1)));

        status = gpuSimEscapeRead(pGpu, path, 0, 4, &pLinkConnection[i]);
        if (status == NV_OK)
        {
            NV_PRINTF(LEVEL_INFO, "%s: %s=0x%X\n", __FUNCTION__,
                      path, pLinkConnection[i]);
        }
        else
        {
            NV_PRINTF(LEVEL_INFO, "%s: gpuSimEscapeRead for '%s' failed (%u)\n",
                      __FUNCTION__, path, status);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
    return NV_OK;
}

static NV_STATUS
osGetSmbiosTableInfo
(
    const NvU8 *pMappedAddr,
    NvU64 *pBaseAddr,
    NvU64 *pLength,
    NvU64 *pNumSubTypes,
    NvU32 *pVersion
)
{
    *pBaseAddr = 0;
    *pLength = 0;
    *pNumSubTypes = 0;
    *pVersion = 0;

    if (portMemCmp(pMappedAddr, "_SM3_", 5) == 0)
    {
        *pVersion = (pMappedAddr[7] << 8) | pMappedAddr[8];
        portMemCopy(pLength, 4, pMappedAddr + 12, 4);
        portMemCopy(pBaseAddr, 8, pMappedAddr + 16, 8);

        *pNumSubTypes = *pLength / 4;

        return NV_OK;
    }

    if (portMemCmp(pMappedAddr, "_SM_", 4) == 0)
    {
        *pVersion = (pMappedAddr[6] << 8) | pMappedAddr[7];

        pMappedAddr += 16;

        if (portMemCmp(pMappedAddr, "_DMI_", 5) == 0)
        {
            portMemCopy(pLength, 2, pMappedAddr + 6, 2);
            portMemCopy(pBaseAddr, 4, pMappedAddr + 8, 4);
            portMemCopy(pNumSubTypes, 2, pMappedAddr + 12, 2);

            if (!*pVersion)
                *pVersion = (pMappedAddr[14] & 0xF0) << 4 |
                            (pMappedAddr[14] & 0x0F);

            return NV_OK;
        }
    }

    return NV_ERR_INVALID_ADDRESS;
}


/*
 * @brief Function to export SMBIOS table. Also, maps table in kernel-space.
 *
 * @param[out]   ppBaseVAddr         Base virtual address of SMBIOS table.
 * @param[out]   pLength             Size of SMBIOS table.
 * @param[out]   pNumSubTypes        Count of structures (types) embedded in
 *                                   the SMBIOS table.
 * @param[out]   pVersion            SMBIOS version
 *
 * @return NV_OK, NV_ERR_INSUFFICIENT_RESOURCES or NV_ERR_INVALID_ADDRESS
 *          or errors from OS layer
 */
NV_STATUS
osGetSmbiosTable
(
    void **ppBaseVAddr,
    NvU64 *pLength,
    NvU64 *pNumSubTypes,
    NvU32 *pVersion
)
{
    NV_STATUS status = NV_OK;
    NvU64 physSmbiosAddr = ~0ull;
    void *pMappedAddr = NULL;
    NvU64 basePAddr = 0;

    if (!NVCPU_IS_X86_64 && !NVCPU_IS_AARCH64)
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    status = os_get_smbios_header(&physSmbiosAddr);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_INFO, "%s: Failed query SMBIOS table with error: %x \n",
                  __FUNCTION__, status);
        return status;
    }

    NV_ASSERT(physSmbiosAddr != ~0ull);

    pMappedAddr = osMapKernelSpace(physSmbiosAddr,
                                   os_page_size,
                                   NV_MEMORY_CACHED,
                                   NV_PROTECT_READ_WRITE);
    if (!pMappedAddr)
    {
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    status = osGetSmbiosTableInfo(pMappedAddr,
                                  &basePAddr,
                                  pLength,
                                  pNumSubTypes,
                                  pVersion);

    osUnmapKernelSpace(pMappedAddr, os_page_size);

    if (status != NV_OK)
    {
        return status;
    }

    *ppBaseVAddr = osMapKernelSpace(basePAddr,
                                    *pLength,
                                    NV_MEMORY_CACHED,
                                    NV_PROTECT_READ_WRITE);
    if (!*ppBaseVAddr)
    {
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    return NV_OK;
}

/*
 * @brief Function to free SMBIOS table mappings
 *
 * @param[in]   pBaseVAddr        Base virtual address of SMBIOS table.
 * @param[in]   length            Size of SMBIOS table.
 *
 */
void
osPutSmbiosTable
(
    void *pBaseVAddr,
    NvU64 length
)
{
    osUnmapKernelSpace(pBaseVAddr, length);
}

NV_STATUS
osGetAcpiRsdpFromUefi
(
    NvU32  *pRsdpAddr
)
{
    return os_get_acpi_rsdp_from_uefi(pRsdpAddr);
}

/*
 *  @brief Returns NV_TRUE if NvSwitch device is present in the system.
 */
NvBool
osIsNvswitchPresent
(
    void
)
{
    return os_is_nvswitch_present();
}

/*
 *  @brief Function to add crashlog buffer entry.
 *
 *  @param[in] pBuffer              virt_addr of nvlog buffer
 *  @param[in] length               size of nvlog buffer
 */
void
osAddRecordForCrashLog
(
    void *pBuffer,
    NvU32 length
)
{
    os_add_record_for_crashLog(pBuffer, length);
}

/*
 *  @brief Function to delete crashlog buffer entry.
 *
 *  @param[in] pBuffer              virt_addr of nvlog buffer
 */
void
osDeleteRecordForCrashLog
(
    void *pBuffer
)
{
    os_delete_record_for_crashLog(pBuffer);
}

/*
 *  @brief Queries the sysfs interface to get memblock size
 *  @param[out] memblock_size Pointer to the memblock_size
 */
NV_STATUS
osNumaMemblockSize
(
    NvU64 *memblock_size
)
{
    return os_numa_memblock_size(memblock_size);
}

NvBool
osNumaOnliningEnabled
(
    OS_GPU_INFO *pOsGpuInfo
)
{
    NvS32 numaNodeId = NV0000_CTRL_NO_NUMA_NODE;

    //
    // Note that this numaNodeId value fetched from Linux layer might not be
    // accurate since it is possible to overwrite it with regkey on some configs
    //
    if (nv_get_device_memory_config(pOsGpuInfo, NULL, NULL, NULL, NULL,
                                    &numaNodeId) != NV_OK)
    {
        return NV_FALSE;
    }

    return (numaNodeId != NV0000_CTRL_NO_NUMA_NODE);
}

/*
 *  @brief Function to call NUMA allocation entry.
 *
 *  @param[in]  nid       NUMA node id
 *  @param[in]  size      Allocation size
 *  @param[in]  flag      Allocation flags
 *  @param[out] pAddress  Ptr to the allocated physical address
 */
NV_STATUS
osAllocPagesNode
(
    NvS32       nid,
    NvLength    size,
    NvU32       flag,
    NvU64      *pAddress
)
{
    NV_STATUS  status    = NV_OK;
    NvU32      localFlag = NV_ALLOC_PAGES_NODE_NONE;

    if (pAddress == NULL || nid < 0 || size > NV_U32_MAX)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Translate the flags
    if (flag & OS_ALLOC_PAGES_NODE_SKIP_RECLAIM)
    {
        localFlag |= NV_ALLOC_PAGES_NODE_SKIP_RECLAIM;
    }

    status = os_alloc_pages_node(nid, (NvU32)size, localFlag, pAddress);
    return status;
}

void
osAllocAcquirePage
(
    NvU64 pAddress,
    NvU32 pageCount
)
{
    NvU32 i;

    for (i = 0; i < pageCount; i++)
    {
        os_get_page(pAddress + (i << os_page_shift));
    }
}

void
osAllocReleasePage
(
    NvU64 pAddress,
    NvU32 pageCount
)
{
    NvU32 i;

    for (i = 0; i < pageCount; i++)
    {
        os_put_page(pAddress + (i << os_page_shift));
    }
}

/*
 *  @brief Function to return refcount on a page
 *  @param[in] address  The physical address of the page
 */
NvU32
osGetPageRefcount
(
    NvU64       pAddress
)
{
    return os_get_page_refcount(pAddress);
}

/*
 *  @brief Function to return the number of tail pages if the address is
 *  referring to a compound page; For non-compound pages, 1 is returned.
 *  @param[in] address  The physical address of the page
 */
NvU32
osCountTailPages
(
    NvU64       pAddress
)
{
    return os_count_tail_pages(pAddress);
}

/*
 *  @brief Upon success, gets NPU register address range.
 *
 *  @param[in]  pOsGpuInfo       OS specific GPU information pointer
 *  @param[out] pBase            base (physical) of NPU register address range
 *  @param[out] pSize            size of NPU register address range
 */
NV_STATUS
osGetIbmnpuGenregInfo
(
    OS_GPU_INFO *pOsGpuInfo,
    NvU64       *pBase,
    NvU64       *pSize
)
{
    return nv_get_ibmnpu_genreg_info(pOsGpuInfo, pBase, pSize, NULL);
}

/*
 *  @brief Upon success, gets NPU's relaxed ordering mode.
 *
 *  @param[in]  pOsGpuInfo       OS specific GPU information pointer
 *  @param[out] pMode            relaxed ordering mode
 */
NV_STATUS
osGetIbmnpuRelaxedOrderingMode
(
    OS_GPU_INFO *pOsGpuInfo,
    NvBool      *pMode
)
{
    return nv_get_ibmnpu_relaxed_ordering_mode(pOsGpuInfo, pMode);
}

/*
 *  @brief Waits for NVLink HW flush on an NPU associated with a GPU.
 *
 *  @param[in]  pOsGpuInfo       OS specific GPU information pointer
 */
void
osWaitForIbmnpuRsync
(
    OS_GPU_INFO *pOsGpuInfo
)
{
    nv_wait_for_ibmnpu_rsync(pOsGpuInfo);
}

NvU64
osGetPageSize(void)
{
    return os_page_size;
}

NvU8
osGetPageShift(void)
{
    return os_page_shift;
}



/*
 * @brief Opens a new temporary file for reading and writing
 *
 * @param[in]  ppFile      void double pointer
 *
 * @returns NV_STATUS, NV_OK if success,
                       NV_ERR_GENERIC, if error
                       NV_ERR_NOT_SUPPORTED, for unsupported platforms
 */
NV_STATUS
osOpenTemporaryFile
(
    void **ppFile
)
{
    return os_open_temporary_file(ppFile);
}

/*
 * @brief Closes the specified temporary file
 *
 * @param[in]  pFile      Pointer to file
 *
 * @returns void
 */
void
osCloseFile
(
    void *pFile
)
{
    os_close_file(pFile);
}

/*
 * @brief Writes the buffer to the specified file at the given offset
 *
 * @param[in]  pFile            Pointer to file (void)
 * @param[in]  pBuffer          Pointer to buffer from which to copy
 * @param[in]  size             Size of the copy
 * @parma[in]  offset           offset in the file
 *
 * @returns NV_STATUS, NV_OK if success,
                       NV_ERR_GENERIC, if error
                       NV_ERR_NOT_SUPPORTED, for unsupported platforms
 */
NV_STATUS
osWriteToFile
(
    void *pFile,
    NvU8 *pBuffer,
    NvU64 size,
    NvU64 offset
)
{
    return os_write_file(pFile, pBuffer, size, offset);
}

/*
 * @brief Reads from the specified file at the given offset
 *
 * @param[in]  pFile            Pointer to file (void *)
 * @param[in]  pBuffer          Pointer to buffer to which the data is copied
 * @param[in]  size             Size of the copy
 * @parma[in]  offset           offset in the file
 *
 * @returns NV_STATUS, NV_OK if success,
                       NV_ERR_GENERIC, if error
                       NV_ERR_NOT_SUPPORTED, for unsupported platforms
 */
NV_STATUS
osReadFromFile
(
    void *pFile,
    NvU8 *pBuffer,
    NvU64 size,
    NvU64 offset
)
{
    return os_read_file(pFile, pBuffer, size, offset);
}

/*
 * @brief Unregisters caps from the capability framework.
 *        The function assumes that the caps are allocated and stored in the
 *        hierarchical order. If they aren't, OS (Linux kernel) would warn and
 *        leak the caps.
 *
 * @param[in]  pOsRmCaps           caps of interest
 */
void
osRmCapUnregister
(
    OS_RM_CAPS **ppOsRmCaps
)
{
    OS_RM_CAPS *pOsRmCaps = *ppOsRmCaps;
    NvS32 i;

    if (pOsRmCaps == NULL)
    {
        return;
    }

    for (i = pOsRmCaps->count - 1; i >= 0; i--)
    {
        if (pOsRmCaps->caps[i] != NULL)
        {
            os_nv_cap_destroy_entry(pOsRmCaps->caps[i]);
        }
    }

    os_free_mem(pOsRmCaps->caps);
    os_free_mem(pOsRmCaps);

    *ppOsRmCaps = NULL;
}

static NV_STATUS
_allocOsRmCaps
(
    OS_RM_CAPS **ppOsRmCaps,
    NvU32        count
)
{
    NV_STATUS   status;
    OS_RM_CAPS *pOsRmCaps;

    *ppOsRmCaps = NULL;

    status = os_alloc_mem((void**)&pOsRmCaps, sizeof(OS_RM_CAPS));
    if (status != NV_OK)
        return status;

    pOsRmCaps->count = count;

    status = os_alloc_mem((void**)&pOsRmCaps->caps, sizeof(pOsRmCaps->caps[0]) * count);
    if (status != NV_OK)
    {
        os_free_mem(pOsRmCaps);
        return status;
    }

    os_mem_set(pOsRmCaps->caps, 0, sizeof(pOsRmCaps->caps[0]) * count);

    *ppOsRmCaps = pOsRmCaps;
    return NV_OK;
}

#define OS_RM_CAP_GPU_DIR 0
#define OS_RM_CAP_GPU_MIG_DIR 1
#define OS_RM_CAP_GPU_COUNT 2

/*
 * @brief Registers OBJGPU with the capability framework.
 *
 * @param[in]  pOsGpuInfo  OS specific GPU information pointer
 * @param[out] ppOsRmCaps  GPU OS specific capabilities pointer
 */
NV_STATUS
osRmCapRegisterGpu
(
    OS_GPU_INFO   *pOsGpuInfo,
    OS_RM_CAPS   **ppOsRmCaps
)
{
    NvU32 minor = nv_get_dev_minor(pOsGpuInfo);
    char name[16];
    NV_STATUS   status;
    OS_RM_CAPS *pOsRmCaps;
    nv_cap_t   *parent;
    nv_cap_t   *cap;

    // Return success on the unsupported platforms.
    if (nvidia_caps_root == NULL)
    {
        return NV_OK;
    }

    if (*ppOsRmCaps != NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    status = _allocOsRmCaps(&pOsRmCaps, OS_RM_CAP_GPU_COUNT);
    if (status != NV_OK)
        return status;

    *ppOsRmCaps = pOsRmCaps;

    os_snprintf(name, sizeof(name), "gpu%u", minor);
    name[sizeof(name) - 1] = '\0';
    parent = nvidia_caps_root;

    cap = os_nv_cap_create_dir_entry(parent, name, (OS_RUGO | OS_XUGO));
    if (cap == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to setup gpu%u directory\n", minor);
        status = NV_ERR_OPERATING_SYSTEM;
        goto failed;
    }

    pOsRmCaps->caps[OS_RM_CAP_GPU_DIR] = cap;
    parent = cap;

    // TODO: Bug 2679591: Add MIG directory only if SMC is enabled.
    // For now, always add "mig" directory.
    cap = os_nv_cap_create_dir_entry(parent, "mig", (OS_RUGO | OS_XUGO));
    if (cap == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to setup mig directory\n");
        status = NV_ERR_OPERATING_SYSTEM;
        goto failed;
    }

    pOsRmCaps->caps[OS_RM_CAP_GPU_MIG_DIR] = cap;

    return NV_OK;

failed:
    osRmCapUnregister(ppOsRmCaps);

    return status;
}

#define OS_RM_CAP_SMC_PART_DIR 0
#define OS_RM_CAP_SMC_PART_ACCESS_FILE 1
#define OS_RM_CAP_SMC_PART_COUNT 2

/*
 * @brief Registers SMC partition (a.k.a. GPU instance) with the capability
 *        framework
 *
 * @param[in]  pGpuOsRmCaps         GPU OS specific capabilities pointer
 * @param[out] ppPartitionOsRmCaps  OS specific capabilities pointer for SMC partition
 * @param[in]  swizzId              SMC partition swizz ID
 */
NV_STATUS
osRmCapRegisterSmcPartition
(
    OS_RM_CAPS  *pGpuOsRmCaps,
    OS_RM_CAPS **ppPartitionOsRmCaps,
    NvU32        swizzId
)
{
    char        name[16];
    NV_STATUS   status;
    nv_cap_t   *parent;
    nv_cap_t   *cap;
    OS_RM_CAPS *pOsRmCaps;

    // Return success as there is nothing to do.
    if (pGpuOsRmCaps == NULL)
    {
        return NV_OK;
    }

    if (*ppPartitionOsRmCaps != NULL || swizzId >= NV_U32_MAX)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    parent = pGpuOsRmCaps->caps[OS_RM_CAP_GPU_MIG_DIR];
    if (parent == NULL)
    {
        return NV_ERR_INVALID_STATE;
    }

    status = _allocOsRmCaps(&pOsRmCaps, OS_RM_CAP_SMC_PART_COUNT);
    if (status != NV_OK)
        return status;

    *ppPartitionOsRmCaps = pOsRmCaps;

    os_snprintf(name, sizeof(name), "gi%u", swizzId);
    name[sizeof(name) - 1] = '\0';

    cap = os_nv_cap_create_dir_entry(parent, name, OS_RUGO | OS_XUGO);
    if (cap == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to setup gi%u directory\n",
                  swizzId);
        status = NV_ERR_OPERATING_SYSTEM;
        goto failed;
    }

    pOsRmCaps->caps[OS_RM_CAP_SMC_PART_DIR] = cap;
    parent = cap;

    cap = os_nv_cap_create_file_entry(parent, "access", OS_RUGO);
    if (cap == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to setup access file for ID:%u\n",
                  swizzId);
        status = NV_ERR_OPERATING_SYSTEM;
        goto failed;
    }

    pOsRmCaps->caps[OS_RM_CAP_SMC_PART_ACCESS_FILE] = cap;

    return NV_OK;

failed:
    osRmCapUnregister(ppPartitionOsRmCaps);

    return status;
}

#define OS_RM_CAP_SMC_EXEC_PART_DIR 0
#define OS_RM_CAP_SMC_EXEC_PART_ACCESS_FILE 1
#define OS_RM_CAP_SMC_EXEC_PART_COUNT 2

/*
 * @brief Registers SMC execution partition (a.k.a. compute instance) with the
 *        capability framework
 *
 * @param[in]  pPartitionOsRmCaps       OS specific capabilities pointer for SMC partition
 * @param[out] ppExecPartitionOsRmCaps  OS specific capabilities pointer for SMC execution partition
 * @param[in]  execPartitionId          SMC execution partition ID
 */
NV_STATUS
osRmCapRegisterSmcExecutionPartition
(
    OS_RM_CAPS  *pPartitionOsRmCaps,
    OS_RM_CAPS **ppExecPartitionOsRmCaps,
    NvU32        execPartitionId
)
{
    char        name[16];
    NV_STATUS   status;
    nv_cap_t   *parent;
    nv_cap_t   *cap;
    OS_RM_CAPS *pOsRmCaps;

    // Return success as there is nothing to do.
    if (pPartitionOsRmCaps == NULL)
    {
        return NV_OK;
    }

    if ((*ppExecPartitionOsRmCaps != NULL) || (execPartitionId >= NV_U32_MAX))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    parent = pPartitionOsRmCaps->caps[OS_RM_CAP_SMC_PART_DIR];
    if (parent == NULL)
    {
        return NV_ERR_INVALID_STATE;
    }

    status = _allocOsRmCaps(&pOsRmCaps, OS_RM_CAP_SMC_EXEC_PART_COUNT);
    if (status != NV_OK)
    {
        return status;
    }

    *ppExecPartitionOsRmCaps = pOsRmCaps;

    os_snprintf(name, sizeof(name), "ci%u", execPartitionId);
    name[sizeof(name) - 1] = '\0';

    cap = os_nv_cap_create_dir_entry(parent, name, OS_RUGO | OS_XUGO);
    if (cap == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to setup ci%u directory\n",
                  execPartitionId);
        status = NV_ERR_OPERATING_SYSTEM;
        goto failed;
    }

    pOsRmCaps->caps[OS_RM_CAP_SMC_EXEC_PART_DIR] = cap;
    parent = cap;

    cap = os_nv_cap_create_file_entry(parent, "access", OS_RUGO);
    if (cap == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to setup access file for ID:%u\n",
                  execPartitionId);
        status = NV_ERR_OPERATING_SYSTEM;
        goto failed;
    }

    pOsRmCaps->caps[OS_RM_CAP_SMC_EXEC_PART_ACCESS_FILE] = cap;

    return NV_OK;

failed:
    osRmCapUnregister(ppExecPartitionOsRmCaps);

    return status;
}

/*
 * @brief Release the acquired capability
 *
 * @param[in]  dupedCapDescriptor  descriptor to be released
 */
void
osRmCapRelease
(
    NvU64 dupedCapDescriptor
)
{
    if (dupedCapDescriptor == NV_U64_MAX)
    {
        return;
    }

    os_nv_cap_close_fd((int)dupedCapDescriptor);
}

#define OS_RM_CAP_SYS_MIG_DIR                   0
#define OS_RM_CAP_SYS_SMC_CONFIG_FILE           1
#define OS_RM_CAP_SYS_SMC_MONITOR_FILE          2
#define OS_RM_CAP_SYS_FABRIC_IMEX_MGMT_FILE     3
#define OS_RM_CAP_SYS_COUNT                     4

NV_STATUS
osRmCapRegisterSys
(
    OS_RM_CAPS **ppOsRmCaps
)
{
    nv_cap_t  **ppCaps;
    nv_cap_t   *parent;
    nv_cap_t   *cap;
    NV_STATUS   status;
    OS_RM_CAPS *pOsRmCaps;

    if (nvidia_caps_root == NULL)
        return NV_ERR_NOT_SUPPORTED;

    status = _allocOsRmCaps(&pOsRmCaps, OS_RM_CAP_SYS_COUNT);
    if (status != NV_OK)
        return status;

    *ppOsRmCaps = pOsRmCaps;

    ppCaps = pOsRmCaps->caps;

    parent = os_nv_cap_create_dir_entry(nvidia_caps_root, "mig", OS_RUGO | OS_XUGO);
    if (parent == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to create mig directory\n");
        status = NV_ERR_OPERATING_SYSTEM;
        goto failed;
    }
    ppCaps[OS_RM_CAP_SYS_MIG_DIR] = parent;

    cap = os_nv_cap_create_file_entry(parent, "config", OS_RUSR);
    if (cap == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to create mig config file\n");
        status = NV_ERR_OPERATING_SYSTEM;
        goto failed;
    }
    ppCaps[OS_RM_CAP_SYS_SMC_CONFIG_FILE] = cap;

    cap = os_nv_cap_create_file_entry(parent, "monitor", OS_RUGO);
    if (cap == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to create mig monitor file\n");
        status = NV_ERR_OPERATING_SYSTEM;
        goto failed;
    }
    ppCaps[OS_RM_CAP_SYS_SMC_MONITOR_FILE] = cap;

    cap = os_nv_cap_create_file_entry(nvidia_caps_root, "fabric-imex-mgmt", OS_RUSR);
    if (cap == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to create imex file\n");
        status = NV_ERR_OPERATING_SYSTEM;
        goto failed;
    }
    ppCaps[OS_RM_CAP_SYS_FABRIC_IMEX_MGMT_FILE] = cap;

    return NV_OK;

failed:
    osRmCapUnregister(ppOsRmCaps);
    return status;
}

/*
 * @brief Acquire the requested capability
 *
 * @param[in]  pOsRmCaps           opaque pointer to the caps.
 * @param[in]  rmCap               the capability to be acquired.
 * @param[in]  capDescriptor       descriptor to be used for validation
 * @param[out] dupedCapDescriptor  returns duplicated descriptor if validation
 *                                 is successful
 *
 * Note: On Linux, duplicating fd is helpful to let administrators know about
 * the capability users. See https://linux.die.net/man/8/lsof usage.
 */
NV_STATUS
osRmCapAcquire
(
    OS_RM_CAPS *pOsRmCaps,
    NvU32       rmCap,
    NvU64       capDescriptor,
    NvU64      *dupedCapDescriptor
)
{
    nv_cap_t *cap;
    int fd = (int)capDescriptor;
    int duped_fd;
    NvU32 index;
    NV_STATUS status;

    *dupedCapDescriptor = NV_U64_MAX;

    switch (rmCap)
    {
        case NV_RM_CAP_SMC_PARTITION_ACCESS:
        {
            index = OS_RM_CAP_SMC_PART_ACCESS_FILE;
            break;
        }
        case NV_RM_CAP_EXT_FABRIC_MGMT:
        {
            status = nv_acquire_fabric_mgmt_cap(fd, &duped_fd);
            if (status != NV_OK)
            {
                return status;
            }

            goto done;
        }
        case NV_RM_CAP_SMC_EXEC_PARTITION_ACCESS:
        {
            index = OS_RM_CAP_SMC_EXEC_PART_ACCESS_FILE;
            break;
        }
        case NV_RM_CAP_SYS_SMC_CONFIG:
        {
            index = OS_RM_CAP_SYS_SMC_CONFIG_FILE;
            break;
        }
        case NV_RM_CAP_SYS_SMC_MONITOR:
        {
            index = OS_RM_CAP_SYS_SMC_MONITOR_FILE;
            break;
        }
        case NV_RM_CAP_SYS_FABRIC_IMEX_MGMT:
        {
            index = OS_RM_CAP_SYS_FABRIC_IMEX_MGMT_FILE;
            break;
        }
        default:
        {
            return NV_ERR_INVALID_ARGUMENT;
        }
    }

    if (pOsRmCaps == NULL)
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    if (index >= pOsRmCaps->count)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    cap = pOsRmCaps->caps[index];

    duped_fd = os_nv_cap_validate_and_dup_fd(cap, fd);
    if (duped_fd < 0)
    {
        return NV_ERR_INSUFFICIENT_PERMISSIONS;
    }

done:
    *dupedCapDescriptor = duped_fd;

    return NV_OK;
}

/*
 * @brief Initializes capability descriptor
 *
 * @param[out] pCapDescriptor        descriptor to be used
 *
 */
void
osRmCapInitDescriptor
(
    NvU64 *pCapDescriptor
)
{
    *pCapDescriptor = NV_U64_MAX;
}

/*
 * @brief Checks if IMEX channel support is present.
 */
NvBool
osImexChannelIsSupported(void)
{
    return os_imex_channel_is_supported;
}

/*
 * @brief Returns IMEX channel count.
 */
NvS32
osImexChannelCount
(
    void
)
{
    return os_imex_channel_count();
}

/*
 * @brief Returns IMEX channel number.
 *
 * @param[in] descriptor   OS specific descriptor to query channel number.
 *
 */
NvS32
osImexChannelGet(NvU64 descriptor)
{
    return os_imex_channel_get(descriptor);
}

/*
 * @brief Generates random bytes which can be used as a universally unique
 *        identifier.
 *
 * This function may sleep (interruptible).
 *
 * @param[out] pBytes        Array of random bytes
 * @param[in]  numBytes      Size of the array
 */
NV_STATUS
osGetRandomBytes
(
    NvU8 *pBytes,
    NvU16 numBytes
)
{
    return os_get_random_bytes(pBytes, numBytes);
}

/*
 * @brief Allocate wait queue
 *
 * @param[out] ppWq        Wait queue
 */
NV_STATUS
osAllocWaitQueue
(
    OS_WAIT_QUEUE **ppWq
)
{
    return os_alloc_wait_queue(ppWq);
}

/*
 * @brief Free wait queue
 *
 * @param[in] pWq        Wait queue
 */
void
osFreeWaitQueue
(
    OS_WAIT_QUEUE *pWq
)
{
    os_free_wait_queue(pWq);
}

/*
 * @brief Put thread to uninterruptible sleep
 *
 * @param[in] pWq        Wait queue
 */
void
osWaitUninterruptible
(
    OS_WAIT_QUEUE *pWq
)
{
    os_wait_uninterruptible(pWq);
}

/*
 * @brief Put thread to interruptible sleep
 *
 * @param[in] pWq        Wait queue
 */
void
osWaitInterruptible
(
    OS_WAIT_QUEUE *pWq
)
{
    os_wait_interruptible(pWq);
}

/*
 * @brief Wake up thread from uninterruptible sleep
 *
 * @param[in] pWq        Wait queue
 */
void
osWakeUp
(
    OS_WAIT_QUEUE *pWq
)
{
    os_wake_up(pWq);
}

NV_STATUS
osReadPFPciConfigInVF
(
    NvU32 addr,
    NvU32 *data
)
{
    return NV_ERR_NOT_SUPPORTED;
}

/*!
 * @brief Sends an MRQ (message-request) to BPMP
 *
 * The request, response, and ret parameters of this function correspond to the
 * components of the tegra_bpmp_message struct, which BPMP uses to receive
 * MRQs.
 *
 * @param[in]  pOsGpuInfo         OS specific GPU information pointer
 * @param[in]  mrq                MRQ_xxx ID specifying what is requested
 * @param[in]  pRequestData       Pointer to request input data
 * @param[in]  requestDataSize    Size of structure pointed to by pRequestData
 * @param[out] pResponseData      Pointer to response output data
 * @param[in]  responseDataSize   Size of structure pointed to by pResponseData
 * @param[out] ret                MRQ return code (from "ret" element of
 *                                  tegra_bpmp_message struct)
 * @param[out] apiRet             Return code from tegra_bpmp_transfer call
 *
 * @returns NV_OK if successful,
 *          NV_ERR_NOT_SUPPORTED if the functionality is not available,
 *          NV_ERR_INVALID_POINTER if the tegra_bpmp struct pointer could not
 *            be obtained from nv, or
 *          NV_ERR_GENERIC if the tegra_bpmp_transfer call failed (see apiRet
 *            for Linux error code).
 */
NV_STATUS
osTegraSocBpmpSendMrq
(
    OS_GPU_INFO *pOsGpuInfo,
    NvU32        mrq,
    const void  *pRequestData,
    NvU32        requestDataSize,
    void        *pResponseData,
    NvU32        responseDataSize,
    NvS32       *pRet,
    NvS32       *pApiRet
)
{
    return NV_ERR_NOT_SUPPORTED;
}

/*!
 * @brief Returns IMP-relevant data collected from other modules
 *
 * This function is basically a wrapper to call the unix/linux layer.
 *
 * @param[out]  pTegraImpImportData  Structure to receive the data
 *
 * @returns NV_OK if successful,
 *          NV_ERR_BUFFER_TOO_SMALL if the array in TEGRA_IMP_IMPORT_DATA is
 *            too small,
 *          NV_ERR_NOT_SUPPORTED if the functionality is not available, or
 *          other errors as may be returned by subfunctions.
 */
NV_STATUS
osTegraSocGetImpImportData
(
    TEGRA_IMP_IMPORT_DATA *pTegraImpImportData
)
{
    return NV_ERR_NOT_SUPPORTED;
}

/*!
 * @brief Tells BPMP whether or not RFL is valid
 *
 * Display HW generates an ok_to_switch signal which asserts when mempool
 * occupancy is high enough to be able to turn off memory long enough to
 * execute a dramclk frequency switch without underflowing display output.
 * ok_to_switch drives the RFL ("request for latency") signal in the memory
 * unit, and the switch sequencer waits for this signal to go active before
 * starting a dramclk switch.  However, if the signal is not valid (e.g., if
 * display HW or SW has not been initialized yet), the switch sequencer ignores
 * the signal.  This API tells BPMP whether or not the signal is valid.
 *
 * @param[in] pOsGpuInfo    Per GPU Linux state
 * @param[in] bEnable       True if RFL will be valid; false if invalid
 *
 * @returns NV_OK if successful,
 *          NV_ERR_NOT_SUPPORTED if the functionality is not available, or
 *          NV_ERR_GENERIC if some other kind of error occurred.
 */
NV_STATUS
osTegraSocEnableDisableRfl
(
    OS_GPU_INFO *pOsGpuInfo,
    NvBool       bEnable
)
{
    return NV_ERR_NOT_SUPPORTED;
}

/*!
 * @brief Allocates a specified amount of ISO memory bandwidth for display
 *
 * floorBandwidthKBPS is the minimum required (i.e., floor) dramclk frequency
 * multiplied by the width of the pipe over which the display data will travel.
 * (It is understood that the bandwidth calculated by multiplying the clock
 * frequency by the pipe width will not be realistically achievable, due to
 * overhead in the memory subsystem.  The infrastructure will not actually use
 * the bandwidth value, except to reverse the calculation to get the required
 * dramclk frequency.)
 *
 * This function is basically a wrapper to call the unix/linux layer.
 *
 * @param[in]   pOsGpuInfo            OS specific GPU information pointer
 * @param[in]   averageBandwidthKBPS  Amount of ISO memory bandwidth requested
 * @param[in]   floorBandwidhtKBPS    Min required dramclk freq * pipe width
 *
 * @returns NV_OK if successful,
 *          NV_ERR_INSUFFICIENT_RESOURCES if one of the bandwidth values is too
 *            high, and bandwidth cannot be allocated,
 *          NV_ERR_NOT_SUPPORTED if the functionality is not available, or
 *          NV_ERR_GENERIC if some other kind of error occurred.
 */
NV_STATUS
osTegraAllocateDisplayBandwidth
(
    OS_GPU_INFO *pOsGpuInfo,
    NvU32        averageBandwidthKBPS,
    NvU32        floorBandwidthKBPS
)
{
    return NV_ERR_NOT_SUPPORTED;
}

/*!
 * @brief Creates or sets up platform specific nano second resolution timer
 *
 * @param[in]     pOsGpuInfo     OS specific GPU information pointer
 * @param[in]     pTmrEvent      Pointer to timer event information
 * @param[in/out] pTimer         pointer to hold high resolution timer object
 */
NV_STATUS
osCreateNanoTimer
(
    OS_GPU_INFO *pOsGpuInfo,
    void *pTmrEvent,
    void **pTimer
)
{
    nv_create_nano_timer(pOsGpuInfo, pTmrEvent, (nv_nano_timer_t **)pTimer);
    return NV_OK;
}

/*!
 * @brief Starts platform specific nano second resolution timer
 *
 * @param[in] pOsGpuInfo     OS specific GPU information pointer
 * @param[in] pTimer         pointer to high resolution timer object
 * @param[in] timeNs         Relative time in nano seconds
 */
NV_STATUS
osStartNanoTimer
(
    OS_GPU_INFO *pOsGpuInfo,
    void *pTimer,
    NvU64 timeNs
)
{
    nv_start_nano_timer(pOsGpuInfo, (nv_nano_timer_t *)pTimer, timeNs);
    return NV_OK;
}

/*!
 * @brief Cancels platform specific nano second resolution timer
 *
 * @param[in] pOsGpuInfo     OS specific GPU information pointer
 * @param[in] pTimer       pointer to timer object
 */
NV_STATUS
osCancelNanoTimer
(
    OS_GPU_INFO *pOsGpuInfo,
    void *pTimer
)
{
    nv_cancel_nano_timer(pOsGpuInfo, (nv_nano_timer_t *)pTimer);
    return NV_OK;
}

/*!
 * @brief Destroys & cancels platform specific nano second resolution timer
 *
 *
 * @param[in] pGpu         Device of interest
 * @param[in] pTimer       pointer to timer object
 */
NV_STATUS
osDestroyNanoTimer
(
    OS_GPU_INFO *pOsGpuInfo,
    void *pTimer
)
{
    nv_destroy_nano_timer(pOsGpuInfo, (nv_nano_timer_t *)pTimer);
    return NV_OK;
}

/*!
 * @brief Get number of dpAux instances.
 * It is wrapper function to call unix/linux layer.
 *
 * @param[in]  pGpu           GPU object pointer
 * @param[out]  pNumIntances  Number of dpAux instances.
 *
 * @returns NV_STATUS, NV_OK if success,
 *                     NV_ERR_GENERIC, if error
 *                     NV_ERR_NOT_SUPPORTED, for unsupported platforms
 */

NV_STATUS
osGetTegraNumDpAuxInstances
(
    OS_GPU_INFO *pOsGpuInfo,
    NvU32  *pNumIntances
)
{
    return NV_ERR_NOT_SUPPORTED;
}

/*
 * @brief Return the priv Data of current IRQ.
 * It is wrapper function to call unix/linux layer.
 *
 * @param[in]  pGpu           Device of interest
 * @param[out] pPrivData      privData of current IRQ
 *
 * @returns NV_STATUS, NV_OK if success,
 *                     NV_ERR_GENERIC, if error
 *                     NV_ERR_NOT_SUPPORTED, for unsupported platforms
 */
NV_STATUS
osGetCurrentIrqPrivData
(
    OS_GPU_INFO *pOsGpuInfo,
    NvU32  *pPrivData
)
{
    return NV_ERR_NOT_SUPPORTED;
}

/*!
 * @brief Get the brightness level
 * It is wrapper function to call unix/linux layer.
 *
 * @param[in]  pGpu           GPU object pointer
 * @param[out] brightness     Pointer to brightness level
 *
 * @returns NV_STATUS, NV_OK if success,
 *                     NV_ERR_GENERIC, if error
 *                     NV_ERR_NOT_SUPPORTED, for unsupported platforms
 */
NV_STATUS
osGetTegraBrightnessLevel
(
    OS_GPU_INFO *pOsGpuInfo,
    NvU32 *brightness
)
{
    return NV_ERR_NOT_SUPPORTED;
}

/*!
 * @brief Set the brightness level
 * It is wrapper function to call unix/linux layer.
 *
 * @param[in]  pGpu           GPU object pointer
 * @param[out] brightness     brightness level
 *
 * @returns NV_STATUS, NV_OK if success,
 *                     NV_ERR_GENERIC, if error
 *                     NV_ERR_NOT_SUPPORTED, for unsupported platforms
 */
NV_STATUS
osSetTegraBrightnessLevel
(
    OS_GPU_INFO *pOsGpuInfo,
    NvU32 brightness
)
{
    return NV_ERR_NOT_SUPPORTED;
}

/* @brief Gets syncpoint aperture information
 *
 * @param[in] OS_GPU_INFO OS specific GPU information pointer
 * @param[in]  syncpointId
 * @param[out] *physAddr
 * @param[out] *limit
 * @param[out] *offset
 */
NV_STATUS
osGetSyncpointAperture
(
    OS_GPU_INFO *pOsGpuInfo,
    NvU32 syncpointId,
    NvU64 *physAddr,
    NvU64 *limit,
    NvU32 *offset
)
{
    return nv_get_syncpoint_aperture(syncpointId, physAddr, limit, offset);
}

/*!
 * @brief Enable PCIe AtomicOp Requester Enable and return
 * the completer side capabilities that the requester can send.
 *
 * @param[in]    pOsGpuInfo   OS_GPU_INFO OS specific GPU information pointer
 * @param[out]   pMask        mask of supported atomic size, including one or more of:
 *                            OS_PCIE_CAP_MASK_REQ_ATOMICS_32
 *                            OS_PCIE_CAP_MASK_REQ_ATOMICS_64
 *                            OS_PCIE_CAP_MASK_REQ_ATOMICS_128
 *
 * @returns NV_STATUS, NV_OK if success
 *                     NV_ERR_NOT_SUPPORTED if platform doesn't support this
 *                     feature.
 *                     NV_ERR_GENERIC for any other error
 */

NV_STATUS
osConfigurePcieReqAtomics
(
    OS_GPU_INFO *pOsGpuInfo,
    NvU32       *pMask
)
{
    if (pMask)
    {
        *pMask = 0U;
        if (pOsGpuInfo)
        {
            if (os_enable_pci_req_atomics(pOsGpuInfo->handle,
                                          OS_INTF_PCIE_REQ_ATOMICS_32BIT) == NV_OK)
                *pMask |= OS_PCIE_CAP_MASK_REQ_ATOMICS_32;
            if (os_enable_pci_req_atomics(pOsGpuInfo->handle,
                                          OS_INTF_PCIE_REQ_ATOMICS_64BIT) == NV_OK)
                *pMask |= OS_PCIE_CAP_MASK_REQ_ATOMICS_64;
            if (os_enable_pci_req_atomics(pOsGpuInfo->handle,
                                          OS_INTF_PCIE_REQ_ATOMICS_128BIT) == NV_OK)
                *pMask |= OS_PCIE_CAP_MASK_REQ_ATOMICS_128;

            if (*pMask != 0)
                return NV_OK;
        }
    }
    return NV_ERR_NOT_SUPPORTED;
}

/*!
 * @brief Check GPU is accessible or not
 *
 * @param[in]  pGpu           GPU object pointer
 *
 * @returns NVBool, Returns TRUE if the GPU is accessible,
 *                  FALSE, if error
 */
NvBool
osIsGpuAccessible
(
    OBJGPU *pGpu
)
{
    return nv_is_gpu_accessible(NV_GET_NV_STATE(pGpu));
}

/*!
 * @brief Check whether GPU has received a shutdown notification from the OS
 */
NvBool
osIsGpuShutdown
(
    OBJGPU *pGpu
)
{
    nv_state_t *nv = NV_GET_NV_STATE(pGpu);
    return nv ? nv->is_shutdown : NV_TRUE;
}

/*!
 * @brief Check GPU OS info matches
 *
 * @param[in]  pGpu           GPU object pointer
 *
 * @returns NVBool, Returns TRUE if matched.
 */
NvBool
osMatchGpuOsInfo
(
    OBJGPU *pGpu,
    void   *pOsInfo
)
{
    return nv_match_gpu_os_info(NV_GET_NV_STATE(pGpu), pOsInfo);
}

/*!
 * @brief Release GPU OS info.
 *
 * @param[in]  pOsInfo        GPU OS info pointer
 *
 * @returns void
 */
void
osReleaseGpuOsInfo
(
    void   *pOsInfo
)
{
    nv_put_file_private(pOsInfo);
}

/*!
 * @brief Get free, total memory of a NUMA node by NUMA node ID from kernel.
 *
 * @param[in]      numaId              NUMA node ID.
 * @param[out]     free_memory_bytes   free memory in bytes.
 * @param[out]     total_memory_bytes  total memory in bytes.
 *
 */
void
osGetNumaMemoryUsage
(
    NvS32 numaId,
    NvU64 *free_memory_bytes,
    NvU64 *total_memory_bytes
)
{
    NV_STATUS status = os_get_numa_node_memory_usage(numaId,
                                                     free_memory_bytes,
                                                     total_memory_bytes);
    NV_ASSERT(status == NV_OK);
}

/*!
 * @brief Add GPU memory as a NUMA node.
 *
 * @param[in/out]  pOsGpuInfo   OS specific GPU information pointer
 * @param[in]      offset       start offset of the partition within FB
 * @param[in]      size         size of the partition
 * @param[out]     pNumaNodeId  OS NUMA node id for the added memory.
 *
 * @returns NV_OK if all is okay.  Otherwise an error-specific value.
 *
 */
NV_STATUS
osNumaAddGpuMemory
(
    OS_GPU_INFO *pOsGpuInfo,
    NvU64       offset,
    NvU64       size,
    NvU32       *pNumaNodeId
)
{
    nv_state_t *nv = pOsGpuInfo;

    return os_numa_add_gpu_memory(nv->handle, offset, size, pNumaNodeId);
}

/*!
 * @brief Remove a particular SMC partition's GPU memory from OS kernel.
 *
 * Remove GPU memory from the OS kernel that is earlier added as a NUMA node
 * to the kernel in platforms where GPU is coherently connected to the CPU.
 *
 * @param[in/out]  pOsGpuInfo   OS_GPU_INFO OS specific GPU information pointer
 * @param[in]      offset       start offset of the partition within FB
 * @param[in]      size         size of the partition
 * @param[in]      numaNodeId   OS NUMA node id of the memory to be removed.
 *
 */
void
osNumaRemoveGpuMemory
(
    OS_GPU_INFO *pOsGpuInfo,
    NvU64       offset,
    NvU64       size,
    NvU32       numaNodeId
)
{
    nv_state_t *nv = pOsGpuInfo;

    NV_STATUS status = os_numa_remove_gpu_memory(nv->handle, offset, size, numaNodeId);
    NV_ASSERT(status == NV_OK);

    return;
}

NvBool
osDmabufIsSupported(void)
{
    return os_dma_buf_enabled;
}

NV_STATUS
osGetEgmInfo
(
    OBJGPU *pGpu,
    NvU64  *pPhysAddr,
    NvU64  *pSize,
    NvS32  *pNodeId
)
{
    return nv_get_egm_info(NV_GET_NV_STATE(pGpu), pPhysAddr, pSize, pNodeId);
}

/*!
 * @brief Offline (i.e., blacklist) the page containing a given address from OS kernel.
 *
 * @param[in]      address   Address (SPA) of the page to be offlined
 *
 */
NV_STATUS
osOfflinePageAtAddress
(
    NvU64       address
)
{
    return os_offline_page_at_address(address);
}

void osAllocatedRmClient(void *pOsInfo)
{
    nv_file_private_t* nvfp = (nv_file_private_t*)pOsInfo;

    if (nvfp != NULL)
        nvfp->bCleanupRmapi = NV_TRUE;
}
