/*
 * SPDX-FileCopyrightText: Copyright (c) 2014-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include <nvRmReg.h>
#include <virtualization/hypervisor/hypervisor.h>
#include "core/thread_state.h"
#include "core/locks.h"
#include "virtualization/kernel_vgpu_mgr.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"
#include "kernel/gpu/fifo/kernel_fifo.h"
#include "osapi.h"
#include "virtualization/kernel_hostvgpudeviceapi.h"
#include <objtmr.h>
#include "gpu/bif/kernel_bif.h"
#include "gpu/bus/kern_bus.h"
#include <nv_ref.h>               // NV_PMC_BOOT_1_VGPU
#include "nvdevid.h"

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

NvBool hypervisorIsAC_IMPL(void)
{
    return NV_FALSE;
}

void hypervisorSetACSupported_IMPL(OBJHYPERVISOR *pHypervisor)
{
    pHypervisor->bIsACSupported = NV_TRUE;
}

NV_STATUS hypervisorInjectInterrupt_IMPL
(
    OBJHYPERVISOR *pHypervisor,
    VGPU_NS_INTR   *pVgpuNsIntr
)
{
    NV_STATUS status = NV_ERR_NOT_SUPPORTED;

    if (pVgpuNsIntr->pVgpuVfioRef)
        status = osVgpuInjectInterrupt(pVgpuNsIntr->pVgpuVfioRef);
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

static NV_STATUS get_available_instances(
    NvU32 *avail_instances,
    nv_state_t *pNv,
    VGPU_TYPE *vgpuTypeInfo,
    NvU32 pgpuIndex,
    NvU8 devfn
)
{
    NV_STATUS rmStatus = NV_OK;
    OBJGPU *pGpu = NULL;
    OBJSYS *pSys = SYS_GET_INSTANCE();
    KernelVgpuMgr *pKernelVgpuMgr = SYS_GET_KERNEL_VGPUMGR(pSys);
    OBJHYPERVISOR *pHypervisor = SYS_GET_HYPERVISOR(pSys);

    *avail_instances = 0;

    pGpu = NV_GET_NV_PRIV_PGPU(pNv);
    if (pGpu == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "%s GPU handle is not valid \n", __FUNCTION__);
        rmStatus = NV_ERR_INVALID_STATE;
        goto exit;
    }

    /* TODO: Needs to have a proper fix this for DriverVM config */
    if (gpuIsSriovEnabled(pGpu) &&
        !(pHypervisor->getProperty(pHypervisor, PDB_PROP_HYPERVISOR_DRIVERVM_ENABLED)))
    {
        NvU8 fnId = devfn - pGpu->sriovState.firstVFOffset;

        if (fnId > 63)
        {
            NV_ASSERT(0);
            rmStatus = NV_ERR_INVALID_ARGUMENT;
            goto exit;
        }

        if (IS_MIG_ENABLED(pGpu))
        {
            if (IS_MIG_IN_USE(pGpu)) {
                NvU64 swizzIdInUseMask = 0;
                NvU32 partitionFlag = PARTITIONID_INVALID;
                KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
                NvU32 id;

                swizzIdInUseMask = kmigmgrGetSwizzIdInUseMask(pGpu, pKernelMIGManager);

                rmStatus = kvgpumgrGetPartitionFlag(vgpuTypeInfo->vgpuTypeId,
                                                   &partitionFlag);
                if (rmStatus != NV_OK)
                {
                    // Query for a non MIG vgpuType
                    NV_PRINTF(LEVEL_ERROR, "%s Query for a non MIG vGPU type \n",
                              __FUNCTION__);
                    rmStatus = NV_OK;
                    goto exit;
                }

                // Determine valid swizzids not assigned to any vGPU device.
                FOR_EACH_INDEX_IN_MASK(64, id, swizzIdInUseMask)
                {
                    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance;
                    NvU64 mask = 0;

                    rmStatus = kmigmgrGetGPUInstanceInfo(pGpu, pKernelMIGManager,
                                                         id, &pKernelMIGGpuInstance);
                    if (rmStatus != NV_OK)
                    {
                        // Didn't find requested GPU instance
                        NV_PRINTF(LEVEL_ERROR,
                                  "No valid GPU instance with SwizzId - %d found\n", id);
                        goto exit;
                    }

                    mask = NVBIT64(id);

                    if (pKernelMIGGpuInstance->partitionFlag == partitionFlag)
                    {
                        // Validate that same ID is not already set and VF is available
                        if (!(mask & pKernelVgpuMgr->pgpuInfo[pgpuIndex].assignedSwizzIdMask) &&
                            !(pKernelVgpuMgr->pgpuInfo[pgpuIndex].createdVfMask & NVBIT64(fnId)))
                        {
                            *avail_instances = 1;
                            break;
                        }
                    }
                }
                FOR_EACH_INDEX_IN_MASK_END;
            }
        }
        else
        {
            if (pKernelVgpuMgr->pgpuInfo[pgpuIndex].numCreatedVgpu < vgpuTypeInfo->maxInstance)
            {
                if (vgpuTypeInfo->gpuInstanceSize)
                {
                    // Query for a MIG vgpuType
                    NV_PRINTF(LEVEL_ERROR, "%s Query for a MIG vGPU type \n",
                              __FUNCTION__);
                    rmStatus = NV_OK;
                    goto exit;
                }

                if (!(pKernelVgpuMgr->pgpuInfo[pgpuIndex].createdVfMask & NVBIT64(fnId)))
                {
                    if (kvgpumgrCheckVgpuTypeCreatable(&pKernelVgpuMgr->pgpuInfo[pgpuIndex], vgpuTypeInfo) == NV_OK)
                        *avail_instances = 1;
                }
            }
        }
    }
    else
    {
        if (kvgpumgrCheckVgpuTypeCreatable(&pKernelVgpuMgr->pgpuInfo[pgpuIndex], vgpuTypeInfo) == NV_OK)
            *avail_instances = vgpuTypeInfo->maxInstance - pKernelVgpuMgr->pgpuInfo[pgpuIndex].numCreatedVgpu;
    }

exit:
    return rmStatus;
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
                    case VGPU_TYPE_NAME:
                        os_snprintf(buffer, VGPU_STRING_BUFFER_SIZE, "%s\n",
                                    vgpuTypeInfo->vgpuName);
                        break;
                    case VGPU_TYPE_DESCRIPTION:
                         os_snprintf(buffer, MAX_STR_LEN,
                                     "num_heads=%d, frl_config=%d, "
                                     "framebuffer=%lluM, max_resolution=%dx%d, max_instance=%d\n",
                                     vgpuTypeInfo->numHeads, vgpuTypeInfo->frlConfig,
                                     vgpuTypeInfo->profileSize >> 20,
                                     vgpuTypeInfo->maxResolutionX,
                                     vgpuTypeInfo->maxResolutionY,
                                     vgpuTypeInfo->maxInstance);
                        break;
                    case VGPU_TYPE_INSTANCES:
                        rmStatus = get_available_instances(&avail_instances, pNv,
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

                    rmStatus = get_available_instances(&avail_instances, pNv,
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

NV_STATUS NV_API_CALL nv_vgpu_get_bar_info(
    nvidia_stack_t *sp,
    nv_state_t *pNv,
    const NvU8 *pMdevUuid,
    NvU64 *size,
    NvU32 regionIndex,
    void *pVgpuVfioRef,
    NvBool *isBar64bit
)
{
    REQUEST_VGPU_INFO_NODE *pRequestVgpu = NULL;
    THREAD_STATE_NODE threadState;
    NV_STATUS    rmStatus = NV_OK, status;
    OBJGPU      *pGpu = NULL;
    KernelBus   *pKernelBus;
    KERNEL_HOST_VGPU_DEVICE *pKernelHostVgpuDevice;
    void         *fp = NULL;
    NvU32        value = 0;
    OBJSYS      *pSys = SYS_GET_INSTANCE();
    KernelVgpuMgr * pKernelVgpuMgr = SYS_GET_KERNEL_VGPUMGR(pSys);

    NV_ENTER_RM_RUNTIME(sp,fp);
    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    /*
     * This function can be used to query both BAR 64bit state and/or BAR size
     * If neither is queried, return with error.
     */
    if ((size == NULL) && (isBar64bit == NULL))
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

    /* Get input BAR index 64bit state */
    if (isBar64bit != NULL)
    {
        NV_CHECK_OK_OR_GOTO(rmStatus, LEVEL_SILENT,
            is_bar_64bit(pGpu, regionIndex, isBar64bit), release_lock);

        /* Query is only for BAR index 64bit state*/
        if (size == NULL)
            goto release_lock;
    }

    pKernelBus = GPU_GET_KERNEL_BUS(pGpu);
    *size = kbusGetPciBarSize(pKernelBus, regionIndex);

    NV_CHECK_OK_OR_GOTO(rmStatus, LEVEL_SILENT,
        kvgpumgrGetHostVgpuDeviceFromMdevUuid(pNv->gpu_id,
                                             pMdevUuid,
                                             &pKernelHostVgpuDevice), release_lock);

    pRequestVgpu = pKernelHostVgpuDevice->pRequestVgpuInfoNode;
    if (pRequestVgpu == NULL)
    {
        rmStatus = NV_ERR_INVALID_POINTER;
        goto release_lock;
    }

    pKernelHostVgpuDevice->pVgpuVfioRef = pVgpuVfioRef;

    if (regionIndex == NV_VFIO_PCI_BAR1_REGION_INDEX)
    {
        VGPU_TYPE *vgpuTypeInfo;
        NvU32     pgpuIndex = 0;
        NvBool    bOverrideBar1Size = NV_FALSE;

        // Read BAR1 length from vgpuTypeInfo
        NV_CHECK_OK_OR_GOTO(rmStatus, LEVEL_SILENT,
            kvgpumgrGetVgpuTypeInfo(pKernelHostVgpuDevice->vgpuType, &vgpuTypeInfo), release_lock);
        
        *size = vgpuTypeInfo->bar1Length << 20;

        NV_CHECK_OK_OR_GOTO(rmStatus, LEVEL_SILENT,
            kvgpumgrGetPgpuIndex(pKernelVgpuMgr, pNv->gpu_id, &pgpuIndex), release_lock);

        /* 
         * check for 'override_bar1_size' param in vgpuExtraParams list first,
         * if param is missing there then check it in vgpu_params list
         */
        status = nv_parse_config_params((const char*)vgpuTypeInfo->vgpuExtraParams,
                                        "override_bar1_size", ';', &value);

        if (status == NV_OK && value) {
            bOverrideBar1Size = NV_TRUE;
        } else if (status == NV_ERR_OBJECT_NOT_FOUND) {
            status = nv_parse_config_params(pRequestVgpu->configParams,
                                            "override_bar1_size", ',', &value);
            if (status == NV_OK && value)
                bOverrideBar1Size = NV_TRUE;
        }
        if (bOverrideBar1Size) {
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
        status = nv_parse_config_params(pRequestVgpu->configParams,
                                        "address64", ',', &value);

        if ((status != NV_OK) || ((status == NV_OK) && (value != 0)))
        {
            if (regionIndex == NV_VFIO_PCI_BAR2_REGION_INDEX)
                *size = 0;
            else if (regionIndex == NV_VFIO_PCI_BAR3_REGION_INDEX)
                *size = kbusGetPciBarSize(pKernelBus, NV_VFIO_PCI_BAR2_REGION_INDEX);
        }
    }

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

NV_STATUS osVgpuVfioWake(
    void *waitQueue
)
{
    vgpu_vfio_info vgpu_info;

    vgpu_info.waitQueue = waitQueue;

    return os_call_vgpu_vfio((void *) &vgpu_info, CMD_VGPU_VFIO_WAKE_WAIT_QUEUE);
}

NV_STATUS NV_API_CALL nv_vgpu_start(
    nvidia_stack_t *sp,
    const NvU8 *pMdevUuid,
    void *waitQueue,
    NvS32 *returnStatus,
    NvU8 *vmName,
    NvU32 qemuPid
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
        rmStatus = kvgpumgrStart(pMdevUuid, waitQueue, returnStatus,
                                vmName, qemuPid);

        // UNLOCK: release API lock
        rmapiLockRelease();
    }
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

NV_STATUS  NV_API_CALL  nv_vgpu_get_sparse_mmap(
    nvidia_stack_t *sp ,
    nv_state_t *pNv,
    const NvU8 *pMdevUuid,
    NvU64 **offsets,
    NvU64 **sizes,
    NvU32 *numAreas
)
{
    THREAD_STATE_NODE threadState;
    NV_STATUS rmStatus = NV_ERR_INVALID_STATE, status;
    OBJGPU *pGpu = NULL;
    OBJTMR *pTmr = NULL;
    KernelFifo *pKernelFifo = NULL;
    void *fp = NULL;
    REQUEST_VGPU_INFO_NODE *pRequestVgpu = NULL;
    KERNEL_HOST_VGPU_DEVICE *pKernelHostVgpuDevice;
    NvU32 bar0TmrMapSize = 0, bar0FifoMapSize = 0, value = 0;
    NvU64 bar0TmrMapOffset = 0, bar0FifoMapOffset = 0;
    NvU64 *vfRegionSizes = NULL;
    NvU64 *vfRegionOffsets = NULL;
    KernelBif *pKernelBif = NULL;


    NV_ENTER_RM_RUNTIME(sp,fp);
    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    // LOCK: acquire API lock
    if ((rmStatus = rmapiLockAcquire(API_LOCK_FLAGS_NONE, RM_LOCK_MODULES_HYPERVISOR)) == NV_OK)
    {
        pGpu = NV_GET_NV_PRIV_PGPU(pNv);
    
        if (pGpu == NULL)
        {
            rmStatus = NV_ERR_INVALID_STATE;
            goto cleanup;
        }
        pTmr = GPU_GET_TIMER(pGpu);
        pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);
        pKernelBif = GPU_GET_KERNEL_BIF(pGpu);
        *numAreas = 0;
        rmStatus = kvgpumgrGetHostVgpuDeviceFromMdevUuid(pNv->gpu_id, pMdevUuid,
                                                        &pKernelHostVgpuDevice);
        if (rmStatus == NV_OK)
        {
            if (pKernelHostVgpuDevice->gfid != 0)
            {
                rmStatus = kbifGetVFSparseMmapRegions_HAL(pGpu, pKernelBif, pKernelHostVgpuDevice, os_page_size,
                                                          numAreas, NULL, NULL);
                if (rmStatus == NV_OK)
                {
                    os_alloc_mem((void **)&vfRegionOffsets, sizeof(NvU64) * (*numAreas));
                    os_alloc_mem((void **)&vfRegionSizes, sizeof (NvU64) * (*numAreas));
                    if (vfRegionOffsets && vfRegionSizes)
                    {
                        rmStatus = kbifGetVFSparseMmapRegions_HAL(pGpu, pKernelBif, pKernelHostVgpuDevice, os_page_size,
                                                                  numAreas, vfRegionOffsets, vfRegionSizes);
                        if (rmStatus == NV_OK)
                        {
                            *offsets = vfRegionOffsets;
                            *sizes   = vfRegionSizes;
                        }
                    }
                    else
                    {
                        if (vfRegionOffsets != NULL)
                            os_free_mem(vfRegionOffsets);

                        if (vfRegionSizes != NULL)
                            os_free_mem(vfRegionSizes);

                        rmStatus = NV_ERR_INSUFFICIENT_RESOURCES;
                    }
                }
            }
            else
            {
                pRequestVgpu = pKernelHostVgpuDevice->pRequestVgpuInfoNode;
                if (pRequestVgpu == NULL)
                {
                    rmStatus = NV_ERR_INVALID_POINTER;
                    goto cleanup;
                }

                status = nv_parse_config_params(pRequestVgpu->configParams, "direct_gpu_timer_access", ',', &value);
                if ((status == NV_OK) && (value != 0))
                {
                    rmStatus = tmrGetTimerBar0MapInfo_HAL(pGpu, pTmr,
                                                          &bar0TmrMapOffset,
                                                          &bar0TmrMapSize);
                    if (rmStatus == NV_OK)
                        (*numAreas)++;
                    else
                        NV_PRINTF(LEVEL_ERROR,
                                  "%s Failed to get NV_PTIMER region \n",
                                  __FUNCTION__);
                }

                value = 0;
                {
                    status = kfifoGetUsermodeMapInfo_HAL(pGpu, pKernelFifo,
                                                         &bar0FifoMapOffset,
                                                         &bar0FifoMapSize);
                    if (status == NV_OK)
                        (*numAreas)++;
                }

                if (*numAreas != 0)
                {
                    NvU32 i = 0;
                    NvU64 *tmpOffset, *tmpSize;
                    os_alloc_mem((void **)offsets, sizeof(NvU64) * (*numAreas));
                    os_alloc_mem((void **)sizes, sizeof (NvU64) * (*numAreas));

                    tmpOffset = *offsets;
                    tmpSize   = *sizes;

                    if (bar0TmrMapSize != 0)
                    {
                        tmpOffset[i] = bar0TmrMapOffset;
                        tmpSize[i] = bar0TmrMapSize;
                        i++;
                    }

                    if (bar0FifoMapSize != 0)
                    {
                        tmpOffset[i] = bar0FifoMapOffset;
                        tmpSize[i] = bar0FifoMapSize;
                    }
                }
            }
        }

cleanup:
        // UNLOCK: release API lock
        rmapiLockRelease();
    }

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    NV_EXIT_RM_RUNTIME(sp,fp);

    return rmStatus;
}

NV_STATUS  NV_API_CALL nv_vgpu_update_request(
    nvidia_stack_t *sp ,
    const NvU8 *pMdevUuid,
    VGPU_DEVICE_STATE deviceState,
    NvU64 *offsets,
    NvU64 *sizes,
    const char *configParams
)
{
    THREAD_STATE_NODE threadState;
    NV_STATUS rmStatus = NV_ERR_OBJECT_NOT_FOUND;
    void *fp = NULL;
    REQUEST_VGPU_INFO_NODE *pRequestVgpu = NULL;
    OBJSYS *pSys = SYS_GET_INSTANCE();
    KernelVgpuMgr *pKernelVgpuMgr = SYS_GET_KERNEL_VGPUMGR(pSys);

    NV_ENTER_RM_RUNTIME(sp,fp);
    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    if (offsets != NULL)
        os_free_mem(offsets);

    if (sizes != NULL)
        os_free_mem(sizes);

    // LOCK: acquire API lock
    if ((rmStatus = rmapiLockAcquire(API_LOCK_FLAGS_NONE, RM_LOCK_MODULES_HYPERVISOR)) == NV_OK)
    {
        for (pRequestVgpu = listHead(&pKernelVgpuMgr->listRequestVgpuHead);
             pRequestVgpu != NULL;
             pRequestVgpu = listNext(&pKernelVgpuMgr->listRequestVgpuHead, pRequestVgpu))
        {
            if (portMemCmp(pRequestVgpu->mdevUuid, pMdevUuid, VGPU_UUID_SIZE) == 0)
            {

                if (configParams != NULL)
                    portStringCopy(pRequestVgpu->configParams,
                                   sizeof(pRequestVgpu->configParams),
                                   configParams, (portStringLength(configParams) + 1));

                pRequestVgpu->deviceState = deviceState;
                rmStatus = NV_OK;
            }
        }

        // UNLOCK: release API lock
        rmapiLockRelease();
    }

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    NV_EXIT_RM_RUNTIME(sp,fp);

    return rmStatus;
}

NV_STATUS NV_API_CALL nv_gpu_bind_event(
    nvidia_stack_t *sp
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
        CliAddSystemEvent(NV0000_NOTIFIERS_GPU_BIND_EVENT, 0);

        // UNLOCK: release API lock
        rmapiLockRelease();
    }

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    NV_EXIT_RM_RUNTIME(sp,fp);

    return rmStatus;
}

NV_STATUS osVgpuInjectInterrupt(void *vgpuVfioRef)
{
    vgpu_vfio_info vgpu_info;

    vgpu_info.vgpuVfioRef = vgpuVfioRef;

    return os_call_vgpu_vfio((void *) &vgpu_info, CMD_VGPU_VFIO_INJECT_INTERRUPT);
}

NV_STATUS osVgpuRegisterMdev
(
    OS_GPU_INFO *pOsGpuInfo
)
{
    NV_STATUS status = NV_OK;
    vgpu_vfio_info vgpu_info = {0};
    OBJSYS *pSys = SYS_GET_INSTANCE();
    KernelVgpuMgr *pKernelVgpuMgr = SYS_GET_KERNEL_VGPUMGR(pSys);
    KERNEL_PHYS_GPU_INFO *pPhysGpuInfo;
    NvU32 pgpuIndex, i;
    OBJHYPERVISOR *pHypervisor = SYS_GET_HYPERVISOR(pSys);

    status = kvgpumgrGetPgpuIndex(pKernelVgpuMgr, pOsGpuInfo->gpu_id, &pgpuIndex);
    if (status != NV_OK)
        return status;

    pPhysGpuInfo = &(pKernelVgpuMgr->pgpuInfo[pgpuIndex]);

    vgpu_info.numVgpuTypes = pKernelVgpuMgr->pgpuInfo[pgpuIndex].numVgpuTypes;

    status = os_alloc_mem((void **)&vgpu_info.vgpuTypeIds,
                          ((vgpu_info.numVgpuTypes) * sizeof(NvU32)));
    if (status != NV_OK)
        goto free_mem;

    status = os_alloc_mem((void **)&vgpu_info.vgpuNames,
                          ((vgpu_info.numVgpuTypes) * sizeof(char *)));
    if (status != NV_OK)
        goto free_mem;

    vgpu_info.nv = pOsGpuInfo;
    for (i = 0; i < pPhysGpuInfo->numVgpuTypes; i++)
    {
        status = os_alloc_mem((void *)&vgpu_info.vgpuNames[i], (VGPU_STRING_BUFFER_SIZE * sizeof(char)));
        if (status != NV_OK)
            goto free_mem;

        vgpu_info.vgpuTypeIds[i] = pPhysGpuInfo->vgpuTypes[i]->vgpuTypeId;
        os_snprintf((char *) vgpu_info.vgpuNames[i], VGPU_STRING_BUFFER_SIZE, "%s\n", pPhysGpuInfo->vgpuTypes[i]->vgpuName);
    }

    if ((!pPhysGpuInfo->sriovEnabled) || 
        (pHypervisor->getProperty(pHypervisor, PDB_PROP_HYPERVISOR_DRIVERVM_ENABLED)))
    {
        vgpu_info.is_virtfn = NV_FALSE;
        status = os_call_vgpu_vfio((void *)&vgpu_info, CMD_VGPU_VFIO_REGISTER_MDEV);
    }
    else
    {
        for (i = 0; i < MAX_VF_COUNT_PER_GPU; i++)
        {
            if (pPhysGpuInfo->vfPciInfo[i].isNvidiaAttached)
            {
                vgpu_info.is_virtfn =   NV_TRUE;
                vgpu_info.domain    =   pPhysGpuInfo->vfPciInfo[i].domain;
                vgpu_info.bus       =   pPhysGpuInfo->vfPciInfo[i].bus;
                vgpu_info.slot      =   pPhysGpuInfo->vfPciInfo[i].slot;
                vgpu_info.function  =   pPhysGpuInfo->vfPciInfo[i].function;

                status = os_call_vgpu_vfio((void *)&vgpu_info, CMD_VGPU_VFIO_REGISTER_MDEV);
                if (status == NV_OK)
                {
                    pPhysGpuInfo->vfPciInfo[i].isMdevAttached = NV_TRUE;
                }
            }
        }
    }

free_mem:
    if (vgpu_info.vgpuTypeIds)
        os_free_mem(vgpu_info.vgpuTypeIds);

    if (vgpu_info.vgpuNames)
    {
        for (i = 0; i < pPhysGpuInfo->numVgpuTypes; i++)
        {
            if (vgpu_info.vgpuNames[i])
            {
                os_free_mem(vgpu_info.vgpuNames[i]);
            }
        }
        os_free_mem(vgpu_info.vgpuNames);
    }

    return status;
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
    NvU32       pmc_boot_1
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJHYPERVISOR *pHypervisor = SYS_GET_HYPERVISOR(pSys);
    NvBool is_sriov_enabled = FLD_TEST_DRF(_PMC, _BOOT_1, _VGPU, _VF, pmc_boot_1);

    // if not running in vGPU mode (guest VM) return NV_OK
    if (!(pHypervisor && pHypervisor->bIsHVMGuest &&
          (FLD_TEST_DRF(_PMC, _BOOT_1, _VGPU, _PV, pmc_boot_1) ||
           is_sriov_enabled)))
    {
        return NV_OK;
    }

    return NV_OK;
}
