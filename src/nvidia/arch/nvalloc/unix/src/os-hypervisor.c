/*
 * SPDX-FileCopyrightText: Copyright (c) 2014-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/*!
 *
 * @file os-hypervisor.c
 * @brief OS specific Hypervisor interfaces for RM
 *
 */

#include "os/os.h"
#include "nv.h"
#include "nv-priv.h"
#include <nvrm_registry.h>
#include <virtualization/hypervisor/hypervisor.h>
#include "core/thread_state.h"
#include "core/locks.h"
#include "virtualization/kernel_vgpu_mgr.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"
#include "kernel/gpu/fifo/kernel_fifo.h"
#include "osapi.h"
#include "virtualization/kernel_hostvgpudeviceapi.h"
#include "gpu/timer/objtmr.h"
#include "gpu/bif/kernel_bif.h"
#include "gpu/bus/kern_bus.h"
#include <nv_ref.h>               // NV_PMC_BOOT_1_VGPU
#include "nvdevid.h"

#include "g_vgpu_chip_flags.h"    // vGPU device names

#define NV_VFIO_PCI_BAR0_REGION_INDEX 0
#define NV_VFIO_PCI_BAR1_REGION_INDEX 1
#define NV_VFIO_PCI_BAR2_REGION_INDEX 2
#define NV_VFIO_PCI_BAR3_REGION_INDEX 3

static NV_STATUS nv_parse_config_params(const char *, const char *, const char, NvU32 *);


void hypervisorSetHypervVgpuSupported_IMPL(OBJHYPERVISOR *pHypervisor)
{
    pHypervisor->bIsHypervVgpuSupported = NV_TRUE;
}

NvBool hypervisorIsVgxHyper_IMPL(void)
{
    return os_is_vgx_hyper();
}

NV_STATUS hypervisorInjectInterrupt_IMPL
(
    OBJHYPERVISOR *pHypervisor,
    VGPU_NS_INTR   *pVgpuNsIntr
)
{
    NV_STATUS status = NV_ERR_NOT_SUPPORTED;

    if (osIsVgpuVfioPresent() == NV_TRUE)
        return NV_ERR_NOT_SUPPORTED;
    else
    {
        if (pVgpuNsIntr->guestMSIAddr && pVgpuNsIntr->guestMSIData)
        {
            status = os_inject_vgx_msi((NvU16)pVgpuNsIntr->guestDomainId,
                                       pVgpuNsIntr->guestMSIAddr,
                                       pVgpuNsIntr->guestMSIData);
        }
    }

    return status;
}

HYPERVISOR_TYPE NV_API_CALL nv_get_hypervisor_type(void)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJHYPERVISOR *pHypervisor = SYS_GET_HYPERVISOR(pSys);
    return hypervisorGetHypervisorType(pHypervisor);
}

#define MAX_STR_LEN 256
NV_STATUS  NV_API_CALL nv_vgpu_get_type_info(
    nvidia_stack_t *sp,
    nv_state_t *pNv,
    NvU32 vgpuTypeId,
    char *buffer,
    int type_info,
    NvU8 devfn
)
{
    THREAD_STATE_NODE threadState;
    OBJSYS *pSys = SYS_GET_INSTANCE();
    KernelVgpuMgr *pKernelVgpuMgr = SYS_GET_KERNEL_VGPUMGR(pSys);
    NV_STATUS rmStatus = NV_OK;
    VGPU_TYPE *vgpuTypeInfo;
    NvU32 pgpuIndex, i, avail_instances = 0;
    OBJGPU *pGpu = NULL;
    void *fp;

    NV_ENTER_RM_RUNTIME(sp,fp);
    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    // LOCK: acquire API lock
    if ((rmStatus = rmapiLockAcquire(API_LOCK_FLAGS_NONE, RM_LOCK_MODULES_HYPERVISOR)) == NV_OK)
    {
        if ((rmStatus = kvgpumgrGetPgpuIndex(pKernelVgpuMgr, pNv->gpu_id, &pgpuIndex)) ==
            NV_OK)
        {
            for (i = 0; i < MAX_VGPU_TYPES_PER_PGPU; i++)
            {
                vgpuTypeInfo = pKernelVgpuMgr->pgpuInfo[pgpuIndex].vgpuTypes[i];
                if (vgpuTypeInfo == NULL)
                    break;

                if (vgpuTypeInfo->vgpuTypeId != vgpuTypeId)
                    continue;

                switch (type_info)
                {
                    case VGPU_TYPE_INSTANCES:
                        pGpu = NV_GET_NV_PRIV_PGPU(pNv);
                        if (pGpu == NULL)
                        {
                            NV_PRINTF(LEVEL_ERROR, "%s GPU handle is not valid \n",
                                      __FUNCTION__);
                            rmStatus = NV_ERR_INVALID_STATE;
                            goto exit;
                        }

                        rmStatus = kvgpumgrGetAvailableInstances(&avail_instances, pGpu,
                                                                 vgpuTypeInfo,
                                                                 pgpuIndex, devfn);
                        if (rmStatus != NV_OK)
                            goto exit;

                        os_snprintf(buffer, MAX_STR_LEN, "%d\n", avail_instances);
                        break;
                    default:
                        rmStatus = NV_ERR_INVALID_ARGUMENT;
                }
                break;
            }
        }

exit:
        // UNLOCK: release API lock
        rmapiLockRelease();
    }

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    NV_EXIT_RM_RUNTIME(sp,fp);

    return rmStatus;
}

NV_STATUS NV_API_CALL nv_vgpu_get_type_ids(
    nvidia_stack_t *sp,
    nv_state_t *pNv,
    NvU32 *numVgpuTypes,
    NvU32 *vgpuTypeIds,
    NvBool isVirtfn,
    NvU8 devfn,
    NvBool getCreatableTypes
)
{
    THREAD_STATE_NODE threadState;
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGPU *pGpu = NULL;
    KernelVgpuMgr *pKernelVgpuMgr = SYS_GET_KERNEL_VGPUMGR(pSys);
    NV_STATUS rmStatus = NV_OK;
    NvU32 pgpuIndex, i, avail_instances = 0;
    NvU32 numSupportedVgpuTypes = 0;
    VGPU_TYPE *vgpuTypeInfo;
    void *fp;

    if (!vgpuTypeIds || !numVgpuTypes)
        return NV_ERR_INVALID_ARGUMENT;

    NV_ENTER_RM_RUNTIME(sp,fp);
    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    // LOCK: acquire API lock
    if ((rmStatus = rmapiLockAcquire(API_LOCK_FLAGS_NONE, RM_LOCK_MODULES_HYPERVISOR)) == NV_OK)
    {
        if ((rmStatus = kvgpumgrGetPgpuIndex(pKernelVgpuMgr, pNv->gpu_id, &pgpuIndex)) ==
            NV_OK)
        {
            if (pKernelVgpuMgr->pgpuInfo[pgpuIndex].sriovEnabled && !isVirtfn)
            {
                *numVgpuTypes = 0;
            }
            else
            {
                numSupportedVgpuTypes = pKernelVgpuMgr->pgpuInfo[pgpuIndex].numVgpuTypes;
                *numVgpuTypes = 0;

                for (i = 0; i < numSupportedVgpuTypes; i++)
                {
                    vgpuTypeInfo = pKernelVgpuMgr->pgpuInfo[pgpuIndex].vgpuTypes[i];

                    if (!getCreatableTypes)
                    {
                        // Return all available types
                        vgpuTypeIds[*numVgpuTypes] = vgpuTypeInfo->vgpuTypeId;
                        (*numVgpuTypes)++;
                        continue;
                    }

                    pGpu = NV_GET_NV_PRIV_PGPU(pNv);
                    if (pGpu == NULL)
                    {
                        NV_PRINTF(LEVEL_ERROR, "%s GPU handle is not valid \n",
                                  __FUNCTION__);
                        goto exit;
                    }

                    rmStatus = kvgpumgrGetAvailableInstances(&avail_instances, pGpu,
                                                             vgpuTypeInfo, pgpuIndex,
                                                             devfn);
                    if (rmStatus != NV_OK)
                    {
                        NV_PRINTF(LEVEL_ERROR, "Failed to get available instances for vGPU ID: %d, status: 0x%x\n",
                                  vgpuTypeInfo->vgpuTypeId, rmStatus);
                        continue;
                    }

                    if (avail_instances == 0)
                        continue;

                    vgpuTypeIds[*numVgpuTypes] = vgpuTypeInfo->vgpuTypeId;
                    (*numVgpuTypes)++;
                }
            }
        }

exit:
        // UNLOCK: release API lock
        rmapiLockRelease();
    }

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    NV_EXIT_RM_RUNTIME(sp,fp);

    return rmStatus;
}

NV_STATUS NV_API_CALL nv_vgpu_delete(
    nvidia_stack_t *sp,
    const NvU8 *pMdevUuid,
    NvU16 vgpuId
)
{
    THREAD_STATE_NODE threadState;
    void         *fp = NULL;
    NvU32        rmStatus = NV_OK;

    NV_ENTER_RM_RUNTIME(sp,fp);
    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    // LOCK: acquire API lock
    if ((rmStatus = rmapiLockAcquire(API_LOCK_FLAGS_NONE, RM_LOCK_MODULES_HYPERVISOR)) == NV_OK)
    {
        rmStatus = kvgpumgrDeleteRequestVgpu(pMdevUuid, vgpuId);
        // UNLOCK: release API lock
        rmapiLockRelease();
    }
    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    NV_EXIT_RM_RUNTIME(sp,fp);

    return rmStatus;
}

NV_STATUS  NV_API_CALL nv_vgpu_process_vf_info(
    nvidia_stack_t *sp,
    nv_state_t *pNv,
    NvU8 cmd,
    NvU32 domain,
    NvU8 bus,
    NvU8 slot,
    NvU8 function,
    NvBool isMdevAttached,
    void *vf_pci_info
)
{
    THREAD_STATE_NODE threadState;
    NV_STATUS     rmStatus = NV_OK;
    void         *fp = NULL;

    NV_ENTER_RM_RUNTIME(sp,fp);
    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    // LOCK: acquire API lock
    if ((rmStatus = rmapiLockAcquire(API_LOCK_FLAGS_NONE, RM_LOCK_MODULES_HYPERVISOR)) == NV_OK)
    {
        rmStatus = kvgpumgrProcessVfInfo(pNv->gpu_id, cmd, domain, bus, slot, function, isMdevAttached, (vgpu_vf_pci_info *) vf_pci_info);

        // UNLOCK: release API lock
        rmapiLockRelease();
    }
    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    NV_EXIT_RM_RUNTIME(sp,fp);

    return rmStatus;
}

NV_STATUS  NV_API_CALL nv_vgpu_create_request(
    nvidia_stack_t *sp,
    nv_state_t *pNv,
    const NvU8 *pMdevUuid,
    NvU32 vgpuTypeId,
    NvU16 *vgpuId,
    NvU32 gpuPciBdf
)
{
    THREAD_STATE_NODE threadState;
    void          *fp          = NULL;
    NV_STATUS     rmStatus     = NV_OK;

    NV_ENTER_RM_RUNTIME(sp,fp);
    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    // LOCK: acquire API lock
    if ((rmStatus = rmapiLockAcquire(API_LOCK_FLAGS_NONE, RM_LOCK_MODULES_HYPERVISOR)) == NV_OK)
    {
        rmStatus = kvgpumgrCreateRequestVgpu(pNv->gpu_id, pMdevUuid,
                                             vgpuTypeId, vgpuId, gpuPciBdf);

        // UNLOCK: release API lock
        rmapiLockRelease();
    }

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    NV_EXIT_RM_RUNTIME(sp,fp);

    return rmStatus;
}

static NV_STATUS is_bar_64bit(
    OBJGPU *pGpu,
    NvU32 regionIndex,
    NvBool *isBar64bit
)
{
    NV_STATUS   rmStatus = NV_OK;

    if (pGpu == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "%s GPU handle is not valid \n", __FUNCTION__);
        rmStatus = NV_ERR_INVALID_STATE;
        goto exit;
    }

    switch (regionIndex)
    {
        case NV_VFIO_PCI_BAR0_REGION_INDEX:
            *isBar64bit = pGpu->sriovState.b64bitVFBar0;
            break;

        case NV_VFIO_PCI_BAR1_REGION_INDEX:
            *isBar64bit = pGpu->sriovState.b64bitVFBar1;
            break;

        case NV_VFIO_PCI_BAR2_REGION_INDEX:
            *isBar64bit = pGpu->sriovState.b64bitVFBar2;
            break;

        default:
            NV_PRINTF(LEVEL_ERROR, "BAR%d region doesn't exist!\n", regionIndex);
            rmStatus = NV_ERR_INVALID_ARGUMENT;
            goto exit;
            break;
    }

    NV_PRINTF(LEVEL_INFO, "BAR%d region is_64bit: %d\n", regionIndex, *isBar64bit);;

exit:
    return rmStatus;
}

static NV_STATUS
_nv_vgpu_get_bar_size(OBJGPU *pGpu, KERNEL_HOST_VGPU_DEVICE *pKernelHostVgpuDevice,
                      NvU32 regionIndex, NvU64 *size, NvU8 *configParams)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    KernelVgpuMgr *pKernelVgpuMgr = SYS_GET_KERNEL_VGPUMGR(pSys);
    NV_STATUS status;
    KernelBus *pKernelBus;
    NvU32 value = 0;

    pKernelBus = GPU_GET_KERNEL_BUS(pGpu);
    *size = kbusGetPciBarSize(pKernelBus, regionIndex);

    if (regionIndex == NV_VFIO_PCI_BAR1_REGION_INDEX)
    {
        VGPU_TYPE *vgpuTypeInfo;
        NvU32     pgpuIndex = 0;
        NvBool    bOverrideBar1Size = NV_FALSE;

        // Read BAR1 length from vgpuTypeInfo
        NV_ASSERT_OK_OR_RETURN(kvgpumgrGetVgpuTypeInfo(pKernelHostVgpuDevice->vgpuType,
                                                       &vgpuTypeInfo));

        *size = vgpuTypeInfo->bar1Length << 20;
        NV_ASSERT_OK_OR_RETURN(kvgpumgrGetPgpuIndex(pKernelVgpuMgr, pGpu->gpuId, &pgpuIndex));

        /*
         * check for 'override_bar1_size' param in vgpuExtraParams list first,
         * if param is missing there then check it in vgpu_params list
         */
        status = nv_parse_config_params((const char*)vgpuTypeInfo->vgpuExtraParams,
                                        "override_bar1_size", ';', &value);
        if (status == NV_OK && value)
        {
            bOverrideBar1Size = NV_TRUE;
        }
        else if (status == NV_ERR_OBJECT_NOT_FOUND)
        {
            status = nv_parse_config_params((const char *)configParams,
                                            "override_bar1_size", ',', &value);
            if (status == NV_OK && value)
                bOverrideBar1Size = NV_TRUE;
        }

        if (gpuIsVfResizableBAR1Supported(pGpu))
        {
            if ((*size > pGpu->sriovState.vfBarSize[1]) ||
                (!portStringCompare("Compute", (const char *)vgpuTypeInfo->vgpuClass, 7)))
            {
                *size = pGpu->sriovState.vfBarSize[1];
            }
        }

        if (bOverrideBar1Size)
        {
            NvU64 bar1SizeInBytes, guestBar1;
            NvU64 gpuBar1LowerLimit = 256 * 1024 * 1024; // bar1 lower limit for override_bar1_length parameter
            bar1SizeInBytes = kbusGetPciBarSize(pKernelBus, NV_VFIO_PCI_BAR1_REGION_INDEX);

            if (pKernelVgpuMgr->pgpuInfo[pgpuIndex].sriovEnabled)
            {
                *size = pGpu->sriovState.vfBarSize[1];
            }
            else if (bar1SizeInBytes > gpuBar1LowerLimit)
            {
                guestBar1 = bar1SizeInBytes / vgpuTypeInfo->maxInstance;
                *size = nvPrevPow2_U64(guestBar1);
            }
        }
    }
    else if (regionIndex == NV_VFIO_PCI_BAR2_REGION_INDEX ||
             regionIndex == NV_VFIO_PCI_BAR3_REGION_INDEX)
    {
        status = nv_parse_config_params((const char *)configParams,
                                        "address64", ',', &value);

        if ((status != NV_OK) || ((status == NV_OK) && (value != 0)))
        {
            if (regionIndex == NV_VFIO_PCI_BAR2_REGION_INDEX)
                *size = 0;
            else if (regionIndex == NV_VFIO_PCI_BAR3_REGION_INDEX)
                *size = kbusGetPciBarSize(pKernelBus, NV_VFIO_PCI_BAR2_REGION_INDEX);
        }
    }

    return NV_OK;
}

NV_STATUS NV_API_CALL nv_vgpu_get_bar_info
(
    nvidia_stack_t *sp,
    nv_state_t *pNv,
    const NvU8 *pMdevUuid,
    NvU64 *barSizes,
    NvU64 *sparseOffsets,
    NvU64 *sparseSizes,
    NvU32 *sparseCount,
    NvBool *isBar064bit,
    NvU8 *configParams
)
{
    THREAD_STATE_NODE threadState;
    NV_STATUS rmStatus = NV_OK;
    OBJGPU *pGpu = NULL;
    void *fp = NULL;

    NV_ENTER_RM_RUNTIME(sp,fp);
    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    // LOCK: acquire API lock
    NV_CHECK_OK_OR_GOTO(rmStatus, LEVEL_SILENT,
                        rmapiLockAcquire(API_LOCK_FLAGS_NONE, RM_LOCK_MODULES_HYPERVISOR), exit);

    pGpu = NV_GET_NV_PRIV_PGPU(pNv);
    if (pGpu == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "%s GPU handle is not valid \n", __FUNCTION__);
        rmStatus = NV_ERR_INVALID_STATE;
        goto release_lock;
    }

    NV_CHECK_OK_OR_GOTO(rmStatus, LEVEL_SILENT,
                        nv_vgpu_rm_get_bar_info(pGpu, pMdevUuid, barSizes,
                                                sparseOffsets, sparseSizes,
                                                sparseCount, isBar064bit,
                                                configParams),
                        release_lock);
release_lock:
    // UNLOCK: release API lock
    rmapiLockRelease();

exit:
    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    NV_EXIT_RM_RUNTIME(sp,fp);

    return rmStatus;
}

NV_STATUS NV_API_CALL nv_vgpu_get_hbm_info(
    nvidia_stack_t *sp,
    nv_state_t *pNv,
    const NvU8 *pMdevUuid,
    NvU64 *hbmAddr,
    NvU64 *size
)
{
    NV_STATUS    rmStatus = NV_OK;
    THREAD_STATE_NODE threadState;
    OBJGPU      *pGpu = NULL;
    KERNEL_HOST_VGPU_DEVICE *pKernelHostVgpuDevice;
    void         *fp = NULL;

    NV_ENTER_RM_RUNTIME(sp,fp);
    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    if ((size == NULL) || (hbmAddr == NULL))
    {
        rmStatus = NV_ERR_INVALID_ARGUMENT;
        goto exit;
    }

    // LOCK: acquire API lock
    NV_CHECK_OK_OR_GOTO(rmStatus, LEVEL_SILENT, rmapiLockAcquire(API_LOCK_FLAGS_NONE, RM_LOCK_MODULES_HYPERVISOR), exit);

    pGpu = NV_GET_NV_PRIV_PGPU(pNv);
    if (pGpu == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "%s GPU handle is not valid \n", __FUNCTION__);
        rmStatus = NV_ERR_INVALID_STATE;
        goto release_lock;
    }

    NV_CHECK_OK_OR_GOTO(rmStatus, LEVEL_SILENT,
                        kvgpumgrGetHostVgpuDeviceFromMdevUuid(pNv->gpu_id,
                                                              pMdevUuid,
                                                              &pKernelHostVgpuDevice), release_lock);
    if (pKernelHostVgpuDevice->numValidHbmRegions > 1)
    {
        NV_PRINTF(LEVEL_NOTICE, "non contiguous HBM region is not supported\n");
        NV_ASSERT(0);
    }

    if (pKernelHostVgpuDevice->hbmRegionList != NULL)
    {
        *hbmAddr = pKernelHostVgpuDevice->hbmRegionList[0].hbmBaseAddr;
        *size = pKernelHostVgpuDevice->hbmRegionList[0].size;
    }
    else
    {
        *hbmAddr = 0;
        *size = 0;
    }

release_lock:
    // UNLOCK: release API lock
    rmapiLockRelease();
exit:

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    NV_EXIT_RM_RUNTIME(sp,fp);
    return rmStatus;
}

static NV_STATUS nv_parse_config_params(
    const char *config_params,
    const char *key,
    const char delim,
    NvU32 *config_value
)
{
    char *ptr, *configParams = rm_remove_spaces(config_params);
    char *token, *value, *name;
    NvU32 data;
    NV_STATUS rmStatus = NV_ERR_OBJECT_NOT_FOUND;

    ptr = configParams;
    while ((token = rm_string_token(&ptr, delim)) != NULL)
    {
        if (!(name = rm_string_token(&token, '=')) || !os_string_length(name))
            continue;

        if (!(value = rm_string_token(&token, '=')) || !os_string_length(value))
            continue;

        data = os_strtoul(value, NULL, 0);

        if (os_string_compare(name, key) == 0)
        {
            rmStatus = NV_OK;
            *config_value = data;
        }
    }

   // Free the memory allocated by rm_remove_spaces()
   os_free_mem(configParams);

   return rmStatus;
}

static NV_STATUS _nv_vgpu_get_sparse_mmap(
    OBJGPU *pGpu,
    KERNEL_HOST_VGPU_DEVICE *pKernelHostVgpuDevice,
    NvU64 *offsets,
    NvU64 *sizes,
    NvU32 *numAreas,
    NvU8 *configParams
)
{
    NV_STATUS rmStatus = NV_OK, status;
    OBJTMR *pTmr = GPU_GET_TIMER(pGpu);;
    KernelFifo *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);;
    KernelBif *pKernelBif = GPU_GET_KERNEL_BIF(pGpu);
    NvU32 value = 0;

    *numAreas = 0;
    if (pKernelHostVgpuDevice->gfid != 0)
    {
        rmStatus = kbifGetVFSparseMmapRegions_HAL(pGpu, pKernelBif, pKernelHostVgpuDevice,
                                                  os_page_size, numAreas, NULL, NULL);
        if (rmStatus == NV_OK)
        {
            if (*numAreas > NVA081_MAX_SPARSE_REGION_COUNT)
            {
                NV_PRINTF(LEVEL_ERROR, "Not enough space for sparse mmap region info\n");
                return NV_ERR_INSUFFICIENT_RESOURCES;
            }


            rmStatus = kbifGetVFSparseMmapRegions_HAL(pGpu, pKernelBif, pKernelHostVgpuDevice, os_page_size,
                                                      numAreas, offsets, sizes);
            if (rmStatus != NV_OK)
                return rmStatus;
        }
    }
    else
    {
        status = nv_parse_config_params((const char *)configParams,
                                        "direct_gpu_timer_access", ',', &value);
        if ((status == NV_OK) && (value != 0))
        {
            NvU64 offset = 0;
            NvU32 size = 0;

            rmStatus = tmrGetTimerBar0MapInfo_HAL(pGpu, pTmr, &offset, &size);

            if (rmStatus == NV_OK)
            {
                offsets[*numAreas] = offset;
                sizes[*numAreas] = size;
                (*numAreas)++;
            }
        }

        value = 0;
        {
            NvU64 offset = 0;
            NvU32 size = 0;

            status = kfifoGetUsermodeMapInfo_HAL(pGpu, pKernelFifo, &offset, &size);

            if (status == NV_OK)
            {
                offsets[*numAreas] = offset;
                sizes[*numAreas] = size;
                (*numAreas)++;
            }
        }
    }

    return rmStatus;
}

NV_STATUS nv_vgpu_rm_get_bar_info
(
    OBJGPU     *pGpu,
    const NvU8 *pMdevUuid,
    NvU64      *barSizes,
    NvU64      *sparseOffsets,
    NvU64      *sparseSizes,
    NvU32      *sparseCount,
    NvBool     *isBar064bit,
    NvU8       *configParams
)
{
    KERNEL_HOST_VGPU_DEVICE *pKernelHostVgpuDevice;
    NV_STATUS rmStatus;
    NvU32 i = 0;

    NV_CHECK_OK_OR_GOTO(rmStatus, LEVEL_SILENT,
                        is_bar_64bit(pGpu, NV_VFIO_PCI_BAR0_REGION_INDEX, isBar064bit),
                        exit);

    NV_CHECK_OK_OR_GOTO(rmStatus, LEVEL_SILENT,
                        kvgpumgrGetHostVgpuDeviceFromMdevUuid(pGpu->gpuId,
                                                              pMdevUuid,
                                                              &pKernelHostVgpuDevice),
                        exit);

    for (i = 0; i < NVA081_MAX_BAR_REGION_COUNT; i++)
    {
        /*
         * For SRIOV, only VF BAR1 is queried via RM, others BARs are directly
         * queried via VF config space in vgpu-vfio
         */
        if (gpuIsSriovEnabled(pGpu) && (i != NV_VFIO_PCI_BAR1_REGION_INDEX))
        {
            barSizes[i] = 0;
            continue;
        }

        rmStatus = _nv_vgpu_get_bar_size(pGpu, pKernelHostVgpuDevice, i,
                                         &barSizes[i], configParams);
        if (rmStatus != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Failed to query BAR size for index %u 0x%x\n",
                      i, rmStatus);
            goto exit;
        }
    }

    NV_CHECK_OK_OR_GOTO(rmStatus, LEVEL_SILENT,
                        _nv_vgpu_get_sparse_mmap(pGpu, pKernelHostVgpuDevice,
                                                 sparseOffsets, sparseSizes,
                                                 sparseCount, configParams),
                        exit);

exit:
    return rmStatus;
}

NV_STATUS NV_API_CALL nv_gpu_unbind_event
(
    nvidia_stack_t *sp,
    NvU32 gpuId,
    NvBool *isEventNotified
)
{
    THREAD_STATE_NODE threadState;
    NV_STATUS rmStatus = NV_OK;
    void *fp = NULL;

    NV_ENTER_RM_RUNTIME(sp,fp);
    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    // LOCK: acquire API lock
    if ((rmStatus = rmapiLockAcquire(API_LOCK_FLAGS_NONE, RM_LOCK_MODULES_HYPERVISOR)) == NV_OK)
    {
        /*
         * Send gpu_id in "status" field of the event so that nvidia-vgpu-mgr
         * daemon knows which GPU is being unbound
         */
        CliAddSystemEvent(NV0000_NOTIFIERS_GPU_UNBIND_EVENT, gpuId, isEventNotified);

        // UNLOCK: release API lock
        rmapiLockRelease();
    }

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    NV_EXIT_RM_RUNTIME(sp,fp);

    return rmStatus;
}

NV_STATUS NV_API_CALL nv_gpu_bind_event(
    nvidia_stack_t *sp,
    NvU32 gpuId,
    NvBool *isEventNotified
)
{
    THREAD_STATE_NODE threadState;
    NV_STATUS rmStatus = NV_OK;
    void *fp = NULL;

    NV_ENTER_RM_RUNTIME(sp,fp);
    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    // LOCK: acquire API lock
    if ((rmStatus = rmapiLockAcquire(API_LOCK_FLAGS_NONE, RM_LOCK_MODULES_HYPERVISOR)) == NV_OK)
    {
        CliAddSystemEvent(NV0000_NOTIFIERS_GPU_BIND_EVENT, gpuId, isEventNotified);

        // UNLOCK: release API lock
        rmapiLockRelease();
    }

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    NV_EXIT_RM_RUNTIME(sp,fp);

    return rmStatus;
}

NV_STATUS osIsVgpuVfioPresent(void)
{
    vgpu_vfio_info vgpu_info;

    return os_call_vgpu_vfio((void *) &vgpu_info, CMD_VGPU_VFIO_PRESENT);
}

NV_STATUS osIsVfioPciCorePresent(void)
{
    vgpu_vfio_info vgpu_info;

    return os_call_vgpu_vfio((void *) &vgpu_info, CMD_VFIO_PCI_CORE_PRESENT);
}

void osWakeRemoveVgpu(NvU32 gpuId, NvU32 returnStatus)
{
    vgpu_vfio_info vgpu_info;

    vgpu_info.return_status = returnStatus;
    vgpu_info.domain = gpuDecodeDomain(gpuId);
    vgpu_info.bus = gpuDecodeBus(gpuId);
    vgpu_info.device = gpuDecodeDevice(gpuId);

    os_call_vgpu_vfio((void *)&vgpu_info, CMD_VFIO_WAKE_REMOVE_GPU);
}


NvU32 osGetGridCspSupport(void)
{
    return os_get_grid_csp_support();
}

void initVGXSpecificRegistry(OBJGPU *pGpu)
{
    NvU32 data32;
    osWriteRegistryDword(pGpu, NV_REG_STR_RM_POWER_FEATURES, 0x55455555);
    osWriteRegistryDword(pGpu, NV_REG_STR_RM_INFOROM_DISABLE_BBX,
                               NV_REG_STR_RM_INFOROM_DISABLE_BBX_YES);
    osWriteRegistryDword(pGpu, NV_REG_PROCESS_NONSTALL_INTR_IN_LOCKLESS_ISR,
                               NV_REG_PROCESS_NONSTALL_INTR_IN_LOCKLESS_ISR_ENABLE);
    if ((osReadRegistryDword(pGpu, NV_REG_STR_RM_DUMP_NVLOG, &data32) != NV_OK))
    {
        osWriteRegistryDword(pGpu, NV_REG_STR_RM_DUMP_NVLOG,
                             NV_REG_STR_RM_DUMP_NVLOG_ENABLE);
    }
    osWriteRegistryDword(pGpu, NV_REG_STR_RM_RC_WATCHDOG,
                               NV_REG_STR_RM_RC_WATCHDOG_DISABLE);
    osWriteRegistryDword(pGpu, NV_REG_STR_CL_FORCE_P2P,
                         DRF_DEF(_REG_STR, _CL_FORCE_P2P, _READ, _DISABLE) |
                         DRF_DEF(_REG_STR, _CL_FORCE_P2P, _WRITE, _DISABLE));
}


NV_STATUS rm_is_vgpu_supported_device(
    OS_GPU_INFO *pOsGpuInfo,
    NvU32       pmc_boot_1,
    NvU32       pmc_boot_42
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJHYPERVISOR *pHypervisor = SYS_GET_HYPERVISOR(pSys);
    NvBool is_sriov_enabled = FLD_TEST_DRF(_PMC, _BOOT_1, _VGPU, _VF, pmc_boot_1);
    NvU32     i;

    // if not running in vGPU mode (guest VM) return NV_OK
    if (!(pHypervisor && pHypervisor->bIsHVMGuest &&
          (FLD_TEST_DRF(_PMC, _BOOT_1, _VGPU, _PV, pmc_boot_1) ||
           is_sriov_enabled)))
    {
        return NV_OK;
    }

    if (!is_sriov_enabled)
    {
        return NV_OK;
    }

    if (os_is_grid_supported() && !gpumgrIsVgxRmFirmwareCapableChip(pmc_boot_42))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    if (os_is_grid_supported())
    {
        for (i = 0; i < NV_ARRAY_ELEMENTS(sVgpuUsmTypes); i++)
        {
            if (pOsGpuInfo->pci_info.device_id == sVgpuUsmTypes[i].ulDevID &&
                    pOsGpuInfo->subsystem_vendor == sVgpuUsmTypes[i].ulSubSystemVendorID &&
                    pOsGpuInfo->subsystem_id == sVgpuUsmTypes[i].ulSubID)
            {
                return NV_OK;
            }
        }
    }

    return NV_ERR_NOT_SUPPORTED;
}
