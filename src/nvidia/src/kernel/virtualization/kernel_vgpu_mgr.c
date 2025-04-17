/*
 * SPDX-FileCopyrightText: Copyright (c) 2012-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "ctrl/ctrla081.h"
#include "class/clc637.h" // for AMPERE_SMC_PARTITION_REF
#include "virtualization/kernel_hostvgpudeviceapi.h"

#include "core/system.h"
#include "os/os.h"
#include "rmapi/control.h"
#include "virtualization/hypervisor/hypervisor.h"
#include "virtualization/kernel_vgpu_mgr.h"
#include "gpu/device/device.h"
#include "vgpu/vgpu_version.h"
#include "vgpu/vgpu_events.h"
#include "rmapi/rmapi_utils.h"
#include "rmapi/rs_utils.h"
#include "gpu/gpu.h"
#include "gpu/mem_sys/kern_mem_sys.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "gpu/mem_mgr/heap.h"
#include "gpu/fifo/kernel_fifo.h"
#include "containers/eheap_old.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"
#include "nvdevid.h"
#include "nvmisc.h"

#define __VGPU_ALIAS_PGPU_LIST__
#include "g_vgpu_resman_specific.h" // _get_chip_id_for_alias_pgpu
#undef __VGPU_ALIAS_PGPU_LIST__

#define MAX_NVU32_TO_CONVERTED_STR_LEN   0x8

#define DEVID_ENCODED_VALUE_SIZE          0x4
#define SUBDEVID_ENCODED_VALUE_SIZE       0x4
#define PGPU_CAP_ENCODED_VALUE_SIZE       0x8

#define PGPU_CAP_ECC_ON                  (1 << 0)
#define PGPU_CAP_SRIOV_ON                (1 << 1)

#define VGPU_LOWER_FB_PROFILE_SIZE_1_GB (1024 * 1024 * 1024)

// Mapping of vGPU MIG profiles types to corresponding RM partiotionFlag
#define GENERATE_vgpuSmcTypeIdMappings
#include "g_vgpu_resman_specific.h"
#undef GENERATE_vgpuSmcTypeIdMappings
//
// ODB functions
//

NV_STATUS
kvgpumgrConstruct_IMPL(KernelVgpuMgr *pKernelVgpuMgr)
{
    NV_PRINTF(LEVEL_INFO, "Enter function\n");

    pKernelVgpuMgr->pHeap = NULL;

    /* Default vGPU version is set to the host supported vGPU version range */
    pKernelVgpuMgr->user_min_supported_version
        = GRIDSW_VERSION_EXTERNAL(NV_VGPU_MIN_SUPPORTED_GRIDSW_VERSION_EXTERNAL_MAJOR,
                                  NV_VGPU_MIN_SUPPORTED_GRIDSW_VERSION_EXTERNAL_MINOR);
    pKernelVgpuMgr->user_max_supported_version
        = GRIDSW_VERSION_EXTERNAL(NV_VGPU_MAX_SUPPORTED_GRIDSW_VERSION_EXTERNAL_MAJOR,
                                  NV_VGPU_MAX_SUPPORTED_GRIDSW_VERSION_EXTERNAL_MINOR);

    listInit(&(pKernelVgpuMgr->listVgpuTypeHead), portMemAllocatorGetGlobalNonPaged());
    listInit(&(pKernelVgpuMgr->listVgpuGuestHead), portMemAllocatorGetGlobalNonPaged());
    listInit(&(pKernelVgpuMgr->listRequestVgpuHead), portMemAllocatorGetGlobalNonPaged());

    return NV_OK;
}

void
kvgpumgrDestruct_IMPL(KernelVgpuMgr *pKernelVgpuMgr)
{
    if (pKernelVgpuMgr->pHeap != NULL)
    {
        pKernelVgpuMgr->pHeap->eheapDestruct(pKernelVgpuMgr->pHeap);
        portMemFree(pKernelVgpuMgr->pHeap);
    }

    listDestroy(&(pKernelVgpuMgr->listRequestVgpuHead));
    listDestroy(&(pKernelVgpuMgr->listVgpuTypeHead));
    listDestroy(&(pKernelVgpuMgr->listVgpuGuestHead));
}

//
// Get max instance for a vgpu profile.
//
NV_STATUS
kvgpumgrGetMaxInstanceOfVgpu(NvU32 vgpuTypeId, NvU32 *maxInstanceVgpu)
{
    VGPU_TYPE  *vgpuTypeInfo;
    NV_STATUS   status;

    if (maxInstanceVgpu == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    status = kvgpumgrGetVgpuTypeInfo(vgpuTypeId, &vgpuTypeInfo);
    if (status == NV_OK)
    {
        *maxInstanceVgpu = vgpuTypeInfo->maxInstance;
    }

    return status;
}

//
// vGPU Manager functions
//


static NV_STATUS
_kvgpumgrClearAssignedSwizzIdMask(OBJGPU *pGpu,
                                  NvU32 swizzId);

static NV_STATUS
_kvgpumgrUpdateHeterogeneousCreatablePlacementIds(OBJGPU *pGpu, NvU16 placementId,
                                                  NvU32 vgpuTypeId, NvBool isAlloc,
                                                  NvU32 swizzId);

static NV_STATUS
_kvgpumgrUpdateHomogeneousCreatablePlacementIds(OBJGPU *pGpu, NvU16 placementId,
                                                NvU32 vgpuTypeId, NvBool isAlloc,
                                                NvU32 swizzId);

static NV_STATUS _kvgpuMgrSetHeterogeneousResourcesPerGI(OBJGPU *pGpu, NvU32 swizzId, KERNEL_PHYS_GPU_INFO *pPgpuInfo);

static NV_STATUS _kvgpumgrSetHomogeneousResourcesPerGI(OBJGPU *pGpu, NvU32 swizzId, KERNEL_PHYS_GPU_INFO *pPgpuInfo);

NV_STATUS
kvgpumgrGetPgpuIndex(KernelVgpuMgr *pKernelVgpuMgr, NvU32 gpuPciId, NvU32* index)
{
    NvU32 i;
    for (i = 0; i < NV_ARRAY_ELEMENTS(pKernelVgpuMgr->pgpuInfo); i++)
    {
        if ((pKernelVgpuMgr->pgpuInfo[i].gpuPciId == gpuPciId) &&
            (pKernelVgpuMgr->pgpuInfo[i].isAttached == NV_TRUE))
        {
            *index = i;
            return NV_OK;
        }
    }
    return NV_ERR_OBJECT_NOT_FOUND;
}

NvBool
kvgpumgrIsHeterogeneousVgpuTypeSupported(void)
{
    /* This support is currently limited to VMware, Device VM and KVM */
    return ((osIsVgpuVfioPresent() == NV_OK) ||
            (osIsVgpuDeviceVmPresent() == NV_OK));
}

NV_STATUS
_kvgpumgrCheckPartitionFlag(OBJGPU *pGpu, NvU32 swizzId, NvU32 vgpuTypeId, NvBool *bMatch)
{
    if (IS_MIG_IN_USE(pGpu) && swizzId != KMIGMGR_SWIZZID_INVALID)
    {
        KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
        KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance;
        NvU32 partitionFlag;

        NV_ASSERT_OK_OR_RETURN(kmigmgrGetGPUInstanceInfo(pGpu, pKernelMIGManager, swizzId, &pKernelMIGGpuInstance));

        kvgpumgrGetPartitionFlag(vgpuTypeId, &partitionFlag);

        *bMatch = (partitionFlag == pKernelMIGGpuInstance->partitionFlag) ? NV_TRUE : NV_FALSE;

        return NV_OK;
    }
    else
    {
        return NV_ERR_NOT_SUPPORTED;
    }
}

/*
 * This function should be called to check at runtime whether homogeneous
 * vGPU placement mode is supported / enabled or not. Homogeneous vGPU placement
 * mode is enabled in mutual exclusion with heterogeneous mode on the Ada+
 * vGPU profiles.
 */
NV_STATUS
kvgpumgrCheckHomogeneousPlacementSupported(OBJGPU *pGpu, NvU32 swizzId)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    KernelVgpuMgr *pKernelVgpuMgr = SYS_GET_KERNEL_VGPUMGR(pSys);
    KERNEL_PHYS_GPU_INFO *pPgpuInfo;
    NvU32 index;
    NvBool bHeterogeneousModeEnabled;

    NV_CHECK_OR_RETURN(LEVEL_ERROR, pGpu != NULL, NV_ERR_INVALID_ARGUMENT);

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            kvgpumgrGetPgpuIndex(pKernelVgpuMgr, pGpu->gpuId, &index));

    pPgpuInfo = &(pKernelVgpuMgr->pgpuInfo[index]);

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, kvgpuMgrGetHeterogeneousMode(pGpu, swizzId, &bHeterogeneousModeEnabled));

    if (!bHeterogeneousModeEnabled &&
        (pPgpuInfo->homogeneousPlacementSupported))
    {
        return NV_OK;
    }

    return NV_ERR_NOT_SUPPORTED;
}

NvBool
kvgpumgrIsVgpuWarmUpdateSupported(void)
{
    /*This support is currently limited to VMware and KVM*/
    return ((osIsVgpuVfioPresent() == NV_OK) ||
            (osIsVgpuDeviceVmPresent() == NV_OK));
}

NV_STATUS
kvgpumgrSetVgpuType(OBJGPU *pGpu, KERNEL_PHYS_GPU_INFO *pPhysGpuInfo, NvU32 vgpuTypeId, NvU32 swizzId)
{
    NvU32 i;

    /*
     * If heterogeneous timesliced mode is enabled, supportedTypeIds is already
     * prepared when creatable placementIds array is updated in
     * _kvgpumgrUpdateHeterogeneousCreatablePlacementIds() function
     */
    if (pGpu != NULL)
    {
        NvBool bHeterogeneousModeEnabled;

        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, kvgpuMgrGetHeterogeneousMode(pGpu, swizzId, &bHeterogeneousModeEnabled));

        if (bHeterogeneousModeEnabled)
            return NV_OK;
    }

    for (i = 0; i < MAX_VGPU_TYPES_PER_PGPU; i++)
        pPhysGpuInfo->supportedTypeIds[i] = NVA081_CTRL_VGPU_CONFIG_INVALID_TYPE;

    if (vgpuTypeId == NVA081_CTRL_VGPU_CONFIG_INVALID_TYPE)
    {
        for (i = 0; i < pPhysGpuInfo->numVgpuTypes; i++)
            pPhysGpuInfo->supportedTypeIds[i] = pPhysGpuInfo->vgpuTypes[i]->vgpuTypeId;

        return NV_OK;
    }

    if (kvgpumgrIsHeterogeneousVgpuTypeSupported())
    {
        VGPU_TYPE *vgpuTypeInfo = NULL;
        NV_STATUS  status       = kvgpumgrGetVgpuTypeInfo(vgpuTypeId, &vgpuTypeInfo);
        NvU32      numSupported = 0;

        if (status != NV_OK)
            return status;

        NvU64 totalFbAllocation = vgpuTypeInfo->profileSize;

        for (i = 0; i < pPhysGpuInfo->numVgpuTypes; i++)
        {
            if (pPhysGpuInfo->vgpuTypes[i]->profileSize == totalFbAllocation)
            {
                pPhysGpuInfo->supportedTypeIds[numSupported] = pPhysGpuInfo->vgpuTypes[i]->vgpuTypeId;
                numSupported++;
            }
        }
    }
    else
    {
        pPhysGpuInfo->supportedTypeIds[0] = vgpuTypeId;
    }

    return NV_OK;
}

// non-MIG vgpu types
static NV_STATUS
_kvgpumgrCheckVgpuTypeSupported(OBJGPU *pGpu, KERNEL_PHYS_GPU_INFO *pPhysGpuInfo,
                                VGPU_TYPE *vgpuTypeInfo, NvU32 existingVgpus)
{
    NvU32 i;

    if (existingVgpus == 0)
        return NV_OK;

    for (i = 0; i < pPhysGpuInfo->numVgpuTypes; i++)
    {
        if (pPhysGpuInfo->supportedTypeIds[i] == vgpuTypeInfo->vgpuTypeId)
            break;
        if (pPhysGpuInfo->supportedTypeIds[i] == NVA081_CTRL_VGPU_CONFIG_INVALID_TYPE)
            return NV_ERR_NOT_SUPPORTED;
    }

    /*
     * In heterogeneous vGPU mode, creatable vGPU type is not dependent on a vGPU types
     * maxInstance. If the type is present in supportedTypeIds (populated based on placementIds),
     * it should suffice
     */
    if (!pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_VGPU_HETEROGENEOUS_MODE))
    {
        if (existingVgpus >= vgpuTypeInfo->maxInstance)
            return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    return NV_OK;
}

NV_STATUS
// non-MIG vgpu types
kvgpumgrCheckVgpuTypeCreatable(OBJGPU *pGpu, KERNEL_PHYS_GPU_INFO *pPhysGpuInfo, VGPU_TYPE *vgpuTypeInfo)
{
    if (osIsVgpuVfioPresent() == NV_OK)
        return _kvgpumgrCheckVgpuTypeSupported(pGpu, pPhysGpuInfo, vgpuTypeInfo, pPhysGpuInfo->numCreatedVgpu);
    else
        return _kvgpumgrCheckVgpuTypeSupported(pGpu, pPhysGpuInfo, vgpuTypeInfo, pPhysGpuInfo->numActiveVgpu);
}

static NV_STATUS
// non-MIG vgpu types
_kvgpumgrCheckVgpuTypeActivatable(OBJGPU *pGpu, KERNEL_PHYS_GPU_INFO *pPhysGpuInfo, VGPU_TYPE *vgpuTypeInfo)
{
    return _kvgpumgrCheckVgpuTypeSupported(pGpu, pPhysGpuInfo, vgpuTypeInfo, pPhysGpuInfo->numActiveVgpu);
}

static NV_STATUS
_addVgpuTypeToCreatableType(NvU32 vgpuTypeId,
                             NvU32 *numVgpuTypes,
                             NvU32 *vgpuTypes,
                             KERNEL_PHYS_GPU_INFO *pgpuInfo,
                             KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance )
{
    NvU32 j = 0;
    NV_STATUS rmStatus = NV_OK;
    // Check if we have already included this vGPU type ID.
    // If not, then add vgpuTypeID to out vgpuTypes array.
    if (vgpuTypeId != NVA081_CTRL_VGPU_CONFIG_INVALID_TYPE)
    {
        for(j = 0; j < *numVgpuTypes; j++)
        {
            if (vgpuTypes[j] == vgpuTypeId)
            {
                break;
            }
        }

        // vgpuTypeId not included in vgputypes array yet, include it.
        if (j == *numVgpuTypes)
        {
            vgpuTypes[*numVgpuTypes] = vgpuTypeId;
            (*numVgpuTypes)++;
        }
    }
    return rmStatus;
}

NV_STATUS
kvgpumgrGetCreatableVgpuTypes(OBJGPU *pGpu, KernelVgpuMgr *pKernelVgpuMgr, NvU32 pgpuIndex,
                              NvU32 gpuInstanceId, NvU32* numVgpuTypes, NvU32* vgpuTypes)
{
    NvU32 i                        = 0;
    KERNEL_PHYS_GPU_INFO *pgpuInfo = &pKernelVgpuMgr->pgpuInfo[pgpuIndex];

    *numVgpuTypes = 0;

    if (IS_MIG_ENABLED(pGpu))
    {
        KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
        NvU64 swizzIdInUseMask = 0;
        NvU32 id;

        // No GPU instances created, none of the profiles are creatable. 
        if (!IS_MIG_IN_USE(pGpu))
            return NV_OK;

        swizzIdInUseMask = kmigmgrGetSwizzIdInUseMask(pGpu, pKernelMIGManager);

        for (id = 0; id < KMIGMGR_MAX_GPU_SWIZZID; id++)
        {
            KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance;
            NvU32 j;
            VGPU_TYPE *pVgpuTypeInfo;
            VGPU_TYPE *existingVgpuTypeInfo = NULL;
            NvBool bExistingVgpuOnGI = NV_FALSE;
            REQUEST_VGPU_INFO_NODE *pRequestVgpu = NULL;
            NvBool bHeterogeneousModeEnabled;
            NvU32 *pSupportedTypeId;

            if (!(NVBIT64(id) & swizzIdInUseMask))
                 continue;

            //If specified GI is present, ignore other GIs
            if ((gpuInstanceId != KMIGMGR_SWIZZID_INVALID) && (gpuInstanceId != id))
                 continue;

            NV_ASSERT_OK_OR_RETURN(
                kmigmgrGetGPUInstanceInfo(pGpu, pKernelMIGManager, id, &pKernelMIGGpuInstance));
            NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                kvgpuMgrGetHeterogeneousMode(pGpu,
                                             id,
                                             &bHeterogeneousModeEnabled));
            if (bHeterogeneousModeEnabled)
            {
                pSupportedTypeId = kvgpuMgrGetVgpuCreatableTypeIdFromSwizzId(pGpu, pgpuInfo, id);

                for (i = 0; i < pgpuInfo->numVgpuTypes; i++)
                {
                    if (pSupportedTypeId[i] == NVA081_CTRL_VGPU_CONFIG_INVALID_TYPE)
                        break;
                    _addVgpuTypeToCreatableType(pSupportedTypeId[i],
                                                numVgpuTypes, vgpuTypes, pgpuInfo, pKernelMIGGpuInstance);
                }
                continue;
            }
            for (j = 0; j < NV_ARRAY_ELEMENTS(vgpuSmcTypeIdMappings); j++)
            {
                bExistingVgpuOnGI = NV_FALSE;

                //Check for Ids with same partition flag on same DevID (GPU) 
                if ((vgpuSmcTypeIdMappings[j].devId == pGpu->idInfo.PCIDeviceID) &&
                    (vgpuSmcTypeIdMappings[j].partitionFlag == pKernelMIGGpuInstance->partitionFlag))
                {
                    if (pgpuInfo->assignedSwizzIdVgpuCount[id] == 0)
                    {
                        // check to make sure the profile is supported
                        for (i = 0; i < pgpuInfo->numVgpuTypes; i++)
                        {
                            if (pgpuInfo->vgpuTypes[i] == NULL)
                                continue;
                            if (pgpuInfo->vgpuTypes[i]->vgpuTypeId == vgpuSmcTypeIdMappings[j].vgpuTypeId)
                            {
                                _addVgpuTypeToCreatableType(vgpuSmcTypeIdMappings[j].vgpuTypeId,
                                                    numVgpuTypes, vgpuTypes, pgpuInfo, pKernelMIGGpuInstance);
                                break;
                            }
                        }

                    }
                    else
                    {
                        /*TODO -> Need to fix this for ESXi for MIG + Timeslice Support*/
                        if (osIsVgpuVfioPresent() == NV_OK)
                        {
                            pVgpuTypeInfo = NULL;
                            // check to make sure the profile is supported
                            for (i = 0; i < pgpuInfo->numVgpuTypes; i++)
                            {
                                if (pgpuInfo->vgpuTypes[i] == NULL)
                                    continue;
                                if (pgpuInfo->vgpuTypes[i]->vgpuTypeId == vgpuSmcTypeIdMappings[j].vgpuTypeId)
                                {
                                    pVgpuTypeInfo = pgpuInfo->vgpuTypes[i];
                                }
                            }
                            // If the vgpu type id is not supported, skip and search the next one
                            if (pVgpuTypeInfo == NULL)
                                continue;
                            for (pRequestVgpu = listHead(&pKernelVgpuMgr->listRequestVgpuHead);
                                 pRequestVgpu != NULL;
                                 pRequestVgpu = listNext(&pKernelVgpuMgr->listRequestVgpuHead, pRequestVgpu))
                            {
                                /* Check for an existing Vgpu on the same physical gpu and same GI  */
                                if ((pRequestVgpu->gpuPciId == pGpu->gpuId) && (pRequestVgpu->swizzId == id))
                                {
                                    bExistingVgpuOnGI = NV_TRUE;
                                    break;
                                }
                            }
                            if (bExistingVgpuOnGI ==  NV_FALSE)
                                continue;
                            
                            NV_ASSERT_OK_OR_RETURN(kvgpumgrGetVgpuTypeInfo(pRequestVgpu->vgpuTypeId, 
                                                    &existingVgpuTypeInfo));
                            
                            // Check if maximum allowed number of vGPUs for this type has already been created
                            if (pgpuInfo->assignedSwizzIdVgpuCount[id] >= existingVgpuTypeInfo->maxInstancePerGI)
                                continue;
                            
                            if(kvgpumgrIsHeterogeneousVgpuTypeSupported() == NV_TRUE)
                            {
                                if (existingVgpuTypeInfo->profileSize == pVgpuTypeInfo->profileSize)
                                    _addVgpuTypeToCreatableType(vgpuSmcTypeIdMappings[j].vgpuTypeId, numVgpuTypes, 
                                                                vgpuTypes, pgpuInfo, pKernelMIGGpuInstance);
                            }
                            else
                            {
                                if (existingVgpuTypeInfo->vgpuTypeId == pVgpuTypeInfo->vgpuTypeId)
                                     _addVgpuTypeToCreatableType(vgpuSmcTypeIdMappings[j].vgpuTypeId, numVgpuTypes, 
                                                                 vgpuTypes, pgpuInfo, pKernelMIGGpuInstance);
                            }
                        }
                    }
                }
            }
        }
    }
    else
    {
        *numVgpuTypes = 0;
        for (i = 0; i < pgpuInfo->numVgpuTypes; i++)
        {
            // Consider only non-MIG (gpuInstanceSize == 0) profiles
            if (!pgpuInfo->vgpuTypes[i]->gpuInstanceSize &&
                (kvgpumgrCheckVgpuTypeCreatable(pGpu, pgpuInfo, pgpuInfo->vgpuTypes[i]) == NV_OK))
            {
                vgpuTypes[*numVgpuTypes] = pgpuInfo->vgpuTypes[i]->vgpuTypeId;
                (*numVgpuTypes)++;
            }
        }
    }

    return NV_OK;
}

static NV_STATUS
_kvgpumgrCreateVgpuType(NVA081_CTRL_VGPU_INFO *pVgpuInfo, VGPU_TYPE **ppVgpuType)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    KernelVgpuMgr *pKernelVgpuMgr = SYS_GET_KERNEL_VGPUMGR(pSys);
    VGPU_TYPE *pVgpuTypeNode;

    pVgpuTypeNode = listAppendNew(&(pKernelVgpuMgr->listVgpuTypeHead));
    if (pVgpuTypeNode == NULL)
        return NV_ERR_NO_MEMORY;

    vgpuMgrFillVgpuType(pVgpuInfo, pVgpuTypeNode);

    *ppVgpuType = pVgpuTypeNode;

    return NV_OK;
}

NV_STATUS
kvgpumgrGetVgpuTypeInfo(NvU32 vgpuTypeId, VGPU_TYPE **vgpuType)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    KernelVgpuMgr *pKernelVgpuMgr = SYS_GET_KERNEL_VGPUMGR(pSys);
    VGPU_TYPE *pVgpuTypeNode;

    for (pVgpuTypeNode = listHead(&(pKernelVgpuMgr->listVgpuTypeHead));
         pVgpuTypeNode != NULL;
         pVgpuTypeNode = listNext(&(pKernelVgpuMgr->listVgpuTypeHead), pVgpuTypeNode))
    {
        if (vgpuTypeId == pVgpuTypeNode->vgpuTypeId)
        {
            *vgpuType = pVgpuTypeNode;
            return NV_OK;
        }
    }
    return NV_ERR_OBJECT_NOT_FOUND;
}

static void
kvgpumgrCopyFromVgpuTypeToVgpuInfo(NVA081_CTRL_VGPU_INFO *pVgpuInfo, VGPU_TYPE *pVgpuType)
{
    pVgpuInfo->vgpuType            = pVgpuType->vgpuTypeId;
    pVgpuInfo->maxInstance         = pVgpuType->maxInstance;
    pVgpuInfo->numHeads            = pVgpuType->numHeads;
    pVgpuInfo->maxResolutionX      = pVgpuType->maxResolutionX;
    pVgpuInfo->maxResolutionY      = pVgpuType->maxResolutionY;
    pVgpuInfo->maxPixels           = pVgpuType->maxPixels;
    pVgpuInfo->frlConfig           = pVgpuType->frlConfig;
    pVgpuInfo->cudaEnabled         = pVgpuType->cudaEnabled;
    pVgpuInfo->eccSupported        = pVgpuType->eccSupported;
    pVgpuInfo->gpuInstanceSize     = pVgpuType->gpuInstanceSize;
    pVgpuInfo->multiVgpuSupported  = pVgpuType->multiVgpuSupported;
    pVgpuInfo->vdevId              = pVgpuType->vdevId;
    pVgpuInfo->pdevId              = pVgpuType->pdevId;
    pVgpuInfo->profileSize         = pVgpuType->profileSize;
    pVgpuInfo->fbLength            = pVgpuType->fbLength;
    pVgpuInfo->gspHeapSize         = pVgpuType->gspHeapSize;
    pVgpuInfo->fbReservation       = pVgpuType->fbReservation;
    pVgpuInfo->mappableVideoSize   = pVgpuType->mappableVideoSize;
    pVgpuInfo->encoderCapacity     = pVgpuType->encoderCapacity;
    pVgpuInfo->bar1Length          = pVgpuType->bar1Length;
    pVgpuInfo->gpuDirectSupported  = pVgpuType->gpuDirectSupported;
    pVgpuInfo->nvlinkP2PSupported  = pVgpuType->nvlinkP2PSupported;
    pVgpuInfo->maxInstancePerGI    = pVgpuType->maxInstancePerGI;
    pVgpuInfo->multiVgpuExclusive  = pVgpuType->multiVgpuExclusive;
    pVgpuInfo->frlEnable           = pVgpuType->frlEnable;

    portStringCopy(
        (char *) pVgpuInfo->vgpuName, sizeof(pVgpuInfo->vgpuName),
        (char *) pVgpuType->vgpuName, sizeof(pVgpuType->vgpuName));
    portStringCopy(
        (char *) pVgpuInfo->vgpuClass, sizeof(pVgpuInfo->vgpuClass),
        (char *) pVgpuType->vgpuClass, sizeof(pVgpuType->vgpuClass));
    portStringCopy(
        (char *) pVgpuInfo->license, sizeof(pVgpuInfo->license),
        (char *) pVgpuType->license, sizeof(pVgpuType->license));
    portStringCopy(
        (char *) pVgpuInfo->vgpuExtraParams, sizeof(pVgpuInfo->vgpuExtraParams),
        (char *) pVgpuType->vgpuExtraParams, sizeof(pVgpuType->vgpuExtraParams));
    portMemCopy(
        (char *) pVgpuInfo->vgpuSignature, sizeof(pVgpuInfo->vgpuSignature),
        (char *) pVgpuType->vgpuSignature, sizeof(pVgpuType->vgpuSignature));
}

NV_STATUS
kvgpumgrSendAllVgpuTypesToGsp(OBJGPU *pGpu)
{
    NV_STATUS             rmStatus       = NV_OK;
    OBJSYS               *pSys           = SYS_GET_INSTANCE();
    KernelVgpuMgr        *pKernelVgpuMgr = SYS_GET_KERNEL_VGPUMGR(pSys);
    RM_API               *pRmApi         = GPU_GET_PHYSICAL_RMAPI(pGpu);
    KERNEL_PHYS_GPU_INFO *pPgpuInfo;
    NV2080_CTRL_VGPU_MGR_INTERNAL_PGPU_ADD_VGPU_TYPE_PARAMS *pAddVgpuTypeParams;
    NvU32                 index, i;

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            kvgpumgrGetPgpuIndex(pKernelVgpuMgr, pGpu->gpuId, &index));

    pPgpuInfo = &(pKernelVgpuMgr->pgpuInfo[index]);

    // Early exit if no vgpu types found
    if (pPgpuInfo->numVgpuTypes == 0)
    {
        return NV_OK;
    }

    pAddVgpuTypeParams = portMemAllocNonPaged(sizeof(*pAddVgpuTypeParams));
    if (pAddVgpuTypeParams == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to alloc memory for add vGPU type params\n");
        return NV_ERR_NO_MEMORY;
    }

    portMemSet(pAddVgpuTypeParams, 0, sizeof(*pAddVgpuTypeParams));

    for (i = 0; i < pPgpuInfo->numVgpuTypes; i++)
    {
        kvgpumgrCopyFromVgpuTypeToVgpuInfo(&pAddVgpuTypeParams->vgpuInfo[i],
                                           pPgpuInfo->vgpuTypes[i]);
    }

    pAddVgpuTypeParams->discardVgpuTypes = NV_TRUE;
    pAddVgpuTypeParams->vgpuInfoCount = pPgpuInfo->numVgpuTypes;

    NV_CHECK_OK_OR_GOTO(rmStatus, LEVEL_ERROR,
            pRmApi->Control(pRmApi, pGpu->hInternalClient, pGpu->hInternalSubdevice,
                            NV2080_CTRL_CMD_VGPU_MGR_INTERNAL_PGPU_ADD_VGPU_TYPE,
                            pAddVgpuTypeParams, sizeof(*pAddVgpuTypeParams)),
            kvgpumgrSendAllVgpuTypesToGsp_exit);

kvgpumgrSendAllVgpuTypesToGsp_exit:
    portMemFree(pAddVgpuTypeParams);
    return rmStatus;
}

NV_STATUS
kvgpumgrPgpuAddVgpuType
(
    OBJGPU *pGpu,
    NvBool discardVgpuTypes,
    NVA081_CTRL_VGPU_INFO *pVgpuInfo
)
{
    NV_STATUS rmStatus = NV_OK;
    NvU32 index;
    OBJSYS *pSys = SYS_GET_INSTANCE();
    KernelVgpuMgr *pKernelVgpuMgr = SYS_GET_KERNEL_VGPUMGR(pSys);
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    VGPU_TYPE *pVgpuTypeInfo;
    KERNEL_PHYS_GPU_INFO *pPgpuInfo;
    NV2080_CTRL_VGPU_MGR_INTERNAL_PGPU_ADD_VGPU_TYPE_PARAMS *pAddVgpuTypeParams;

    // TODO: Eliminate the heap allocation here
    // NV2080_CTRL_VGPU_MGR_INTERNAL_PGPU_ADD_VGPU_TYPE_PARAMS has massive size
    // because it has an embedded struct VGPU_TYPE instead of a pointer.
    // FINN won't be able to figure out serialization for a pointer at this moment.
    pAddVgpuTypeParams = portMemAllocNonPaged(sizeof(*pAddVgpuTypeParams));
    if (pAddVgpuTypeParams == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to alloc add vGPU type ctrl params.\n");
        return NV_ERR_NO_MEMORY;
    }
    portMemSet(pAddVgpuTypeParams, 0, sizeof(*pAddVgpuTypeParams));

    pAddVgpuTypeParams->discardVgpuTypes = discardVgpuTypes;
    pAddVgpuTypeParams->vgpuInfoCount = 1;
    pAddVgpuTypeParams->vgpuInfo[0] = *pVgpuInfo;

    rmStatus = pRmApi->Control(pRmApi, pGpu->hInternalClient, pGpu->hInternalSubdevice,
                               NV2080_CTRL_CMD_VGPU_MGR_INTERNAL_PGPU_ADD_VGPU_TYPE,
                               pAddVgpuTypeParams, sizeof(*pAddVgpuTypeParams));

    portMemFree(pAddVgpuTypeParams);

    if (rmStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to create vGPU type on GSP.\n");
        return rmStatus;
    }

    if (kvgpumgrGetPgpuIndex(pKernelVgpuMgr, pGpu->gpuId, &index) != NV_OK)
    {
        return NV_ERR_OBJECT_NOT_FOUND;
    }

    pPgpuInfo = &(pKernelVgpuMgr->pgpuInfo[index]);

    if (pPgpuInfo->numVgpuTypes == NVA081_MAX_VGPU_TYPES_PER_PGPU && !discardVgpuTypes)
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    // TODO: This is a WAR. Need to do this change in VGPUD.
     if (pPgpuInfo->fractionalMultiVgpu == 0 && pVgpuInfo->maxInstance != 1)
    {
        pVgpuInfo->multiVgpuSupported = 0;
    }

    rmStatus = kvgpumgrGetVgpuTypeInfo(pVgpuInfo->vgpuType, &pVgpuTypeInfo);
    if (rmStatus == NV_ERR_OBJECT_NOT_FOUND)
    {
        rmStatus = _kvgpumgrCreateVgpuType(pVgpuInfo, &pVgpuTypeInfo);
        if (rmStatus != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Failed to create vGPU type.\n");
            return rmStatus;
        }
    }
    else if (rmStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to get vGPU type.\n");
        return rmStatus;
    }

    if (discardVgpuTypes)
    {
        pPgpuInfo->numVgpuTypes = 0;
    }

    if (pPgpuInfo->numVgpuTypes == 0)
    {
        pPgpuInfo->sriovEnabled = gpuIsSriovEnabled(pGpu);
    }

    pPgpuInfo->vgpuTypes[pPgpuInfo->numVgpuTypes] = pVgpuTypeInfo;
    pPgpuInfo->numVgpuTypes++;

    return NV_OK;
}

NV_STATUS
kvgpumgrAttachGpu(NvU32 gpuPciId)
{
    NvU32 index;
    OBJSYS *pSys = SYS_GET_INSTANCE();
    KernelVgpuMgr *pKernelVgpuMgr = SYS_GET_KERNEL_VGPUMGR(pSys);
    KERNEL_PHYS_GPU_INFO *pPhysGpuInfo;

    NV_PRINTF(LEVEL_INFO, "Enter function\n");

    NV_ASSERT_OR_RETURN((pKernelVgpuMgr != NULL), NV_ERR_NOT_SUPPORTED);

    index = 0;
    while ((index < pKernelVgpuMgr->pgpuCount) &&
           ((pKernelVgpuMgr->pgpuInfo[index].isAttached == NV_TRUE) ||
           (pKernelVgpuMgr->pgpuInfo[index].gpuPciId != gpuPciId)))
    {
        index++;
    }
    NV_ASSERT_OR_RETURN((index != NV_MAX_DEVICES), NV_ERR_INSUFFICIENT_RESOURCES);

    pPhysGpuInfo = &(pKernelVgpuMgr->pgpuInfo[index]);

    /* Probe call, RmInit is not done yet, so send pGpu as NULL */
    kvgpumgrSetVgpuType(NULL, pPhysGpuInfo, NVA081_CTRL_VGPU_CONFIG_INVALID_TYPE, KMIGMGR_SWIZZID_INVALID);
    pPhysGpuInfo->numActiveVgpu    = 0;
    pPhysGpuInfo->isAttached       = NV_TRUE;
    pPhysGpuInfo->numCreatedVgpu   = 0;

    listInit(&(pKernelVgpuMgr->pgpuInfo[index].listVgpuConfigEventsHead), portMemAllocatorGetGlobalNonPaged());
    listInit(&(pKernelVgpuMgr->pgpuInfo[index].listHostVgpuDeviceHead), portMemAllocatorGetGlobalNonPaged());

    if (index == pKernelVgpuMgr->pgpuCount)
    {
        pPhysGpuInfo->gpuPciId     = gpuPciId;
        pPhysGpuInfo->numVgpuTypes = 0;
        pPhysGpuInfo->vgpuConfigState = NVA081_CTRL_VGPU_CONFIG_STATE_UNINITIALIZED;
        pKernelVgpuMgr->pgpuCount++;
    }
    // Clear placement info for GI
    portMemSet(pPhysGpuInfo->kernelVgpuTypePlacementInfoGi, 0,
               sizeof(pPhysGpuInfo->kernelVgpuTypePlacementInfoGi));

    return NV_OK;
}

NV_STATUS
kvgpumgrMigTimeslicingModeEnabled(OBJGPU *pGpu)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    KernelVgpuMgr *pKernelVgpuMgr = SYS_GET_KERNEL_VGPUMGR(pSys);
    KERNEL_PHYS_GPU_INFO *pPhysGpuInfo;
    NvU32 i;

    if (hypervisorIsVgxHyper())
    {
        NV2080_CTRL_VGPU_MGR_INTERNAL_SET_VGPU_MIG_TIMESLICE_MODE_PARAMS params = {0};
        RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

        NV_ASSERT_OR_RETURN((pKernelVgpuMgr != NULL), NV_ERR_INVALID_ARGUMENT);

        NV_ASSERT_OR_RETURN((pGpu != NULL), NV_ERR_INVALID_ARGUMENT);

        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, kvgpumgrGetPgpuIndex(pKernelVgpuMgr, pGpu->gpuId, &i));

        pPhysGpuInfo = &(pKernelVgpuMgr->pgpuInfo[i]);
        NV_ASSERT_OR_RETURN((pPhysGpuInfo != NULL), NV_ERR_INVALID_ARGUMENT);

        pPhysGpuInfo->migTimeslicingModeEnabled = NV_FALSE;
        if ((pGpu->getProperty(pGpu, PDB_PROP_GPU_MIG_TIMESLICING_SUPPORTED)))
        {
            // Default disabled on VMWare as older ESXi hypervisor versions don't support
            // the MIG timeslicing. This support will be available with ESXi 9.x or later.
            pPhysGpuInfo->migTimeslicingModeEnabled = NV_TRUE;
        }
        params.bMigTimeslicingModeEnabled = pPhysGpuInfo->migTimeslicingModeEnabled;
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, pRmApi->Control(pRmApi, pGpu->hInternalClient, pGpu->hInternalSubdevice,
                               NV2080_CTRL_CMD_VGPU_MGR_INTERNAL_SET_VGPU_MIG_TIMESLICE_MODE,
                               &params, sizeof(params)));
    }

    return NV_OK;
}
NvBool
kvgpumgrIsMigTimeslicingModeEnabled(OBJGPU *pGpu)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    KernelVgpuMgr *pKernelVgpuMgr = SYS_GET_KERNEL_VGPUMGR(pSys);
    KERNEL_PHYS_GPU_INFO *pPhysGpuInfo;
    NvU32 i;
    NV_STATUS status;

    NV_CHECK_OR_ELSE(LEVEL_ERROR, (pKernelVgpuMgr != NULL), return NV_FALSE);

    NV_CHECK_OR_ELSE(LEVEL_ERROR, (pGpu != NULL), return NV_FALSE);

    NV_CHECK_OK_OR_ELSE(status, LEVEL_ERROR, kvgpumgrGetPgpuIndex(pKernelVgpuMgr, pGpu->gpuId, &i), return NV_FALSE);

    pPhysGpuInfo = &(pKernelVgpuMgr->pgpuInfo[i]);
    NV_CHECK_OR_ELSE(LEVEL_ERROR, (pPhysGpuInfo != NULL), return NV_FALSE);

    return pPhysGpuInfo->migTimeslicingModeEnabled;
}

NV_STATUS
kvgpumgrDetachGpu(NvU32 gpuPciId)
{
    NvU32 i , j;
    OBJSYS *pSys = SYS_GET_INSTANCE();
    KernelVgpuMgr *pKernelVgpuMgr = SYS_GET_KERNEL_VGPUMGR(pSys);
    KERNEL_PHYS_GPU_INFO *pPhysGpuInfo;
    VGPU_TYPE *pVgpuTypeInfo;
    KERNEL_VGPU_TYPE_PLACEMENT_INFO *pKernelVgpuTypePlacementInfo;
    VGPU_TYPE_PLACEMENT_INFO *pVgpuTypePlacementInfo;
    VGPU_INSTANCE_PLACEMENT_INFO *pVgpuInstancePlacementInfo;
    VGPU_TYPE_SUPPORTED_PLACEMENT_INFO *pVgpuTypeSupportedPlacementInfo;
    VGPU_INSTANCE_SUPPORTED_PLACEMENT_INFO *pVgpuInstanceSupportedPlacementInfo;
    REQUEST_VGPU_INFO_NODE *pRequestVgpu = NULL, *pRequestVgpuNext = NULL;

    NV_PRINTF(LEVEL_INFO, "Enter function\n");

    NV_ASSERT_OR_RETURN((pKernelVgpuMgr != NULL), NV_ERR_NOT_SUPPORTED);

    if (kvgpumgrGetPgpuIndex(pKernelVgpuMgr, gpuPciId, &i) != NV_OK)
        return NV_ERR_OBJECT_NOT_FOUND;

    pPhysGpuInfo = &(pKernelVgpuMgr->pgpuInfo[i]);
    pKernelVgpuTypePlacementInfo = &pPhysGpuInfo->kernelVgpuTypePlacementInfo;

    pPhysGpuInfo->createdVfMask = 0;
    pPhysGpuInfo->assignedSwizzIdMask = 0;
    pPhysGpuInfo->isAttached = NV_FALSE;
    pPhysGpuInfo->numCreatedVgpu = 0;

    for (i = 0; i < MAX_VGPU_TYPES_PER_PGPU; i++)
        pPhysGpuInfo->supportedTypeIds[i] = NVA081_CTRL_VGPU_CONFIG_INVALID_TYPE;

    for (i = 0; i < KMIGMGR_MAX_GPU_SWIZZID; i++)
        pPhysGpuInfo->assignedSwizzIdVgpuCount[i] = 0;
    /*
     * When heterogeneous vGPU mode is disabled, if vGPU supports homogeneous
     * placement ID, initially all homogeneous supported placement IDs are
     * creatable placement IDs.
     */
    if (pPhysGpuInfo->homogeneousPlacementSupported == NV_TRUE)
    {
        for (i = 0; i < pPhysGpuInfo->numVgpuTypes; i++)
        {
            pVgpuTypeInfo = pPhysGpuInfo->vgpuTypes[i];

            if (pVgpuTypeInfo == NULL)
                continue;

            pVgpuTypeSupportedPlacementInfo = &pVgpuTypeInfo->vgpuTypeSupportedPlacementInfo;
            pVgpuTypePlacementInfo = &pKernelVgpuTypePlacementInfo->vgpuTypePlacementInfo[i];

            for (j = 0; j < pVgpuTypeSupportedPlacementInfo->homogeneousPlacementCount; j++)
            {
                pVgpuInstanceSupportedPlacementInfo = &pVgpuTypeSupportedPlacementInfo->vgpuInstanceSupportedPlacementInfo[j];
                pVgpuInstancePlacementInfo = &pVgpuTypePlacementInfo->vgpuInstancePlacementInfo[j];
                pVgpuInstancePlacementInfo->creatablePlacementId = pVgpuInstanceSupportedPlacementInfo->homogeneousSupportedPlacementId;
            }
        }
        /* No vGPU instances running, so placement region is empty. */
        bitVectorClrAll(&pKernelVgpuTypePlacementInfo->usedPlacementRegionMap);
    }

    if (listCount(&(pKernelVgpuMgr->listRequestVgpuHead)) > 0)
    {
        for (pRequestVgpu = listHead(&(pKernelVgpuMgr->listRequestVgpuHead));
             pRequestVgpu != NULL;
             pRequestVgpu = pRequestVgpuNext)
        {
            pRequestVgpuNext = listNext(&(pKernelVgpuMgr->listRequestVgpuHead), pRequestVgpu);
            if (pRequestVgpu->gpuPciId == gpuPciId)
            {
                pKernelVgpuMgr->pHeap->eheapFree(pKernelVgpuMgr->pHeap, pRequestVgpu->vgpuId);
                listRemove(&(pKernelVgpuMgr->listRequestVgpuHead), pRequestVgpu);
            }
        }
    }

    listDestroy(&(pPhysGpuInfo->listHostVgpuDeviceHead));
    // Clear placement info for GI
    portMemSet(pPhysGpuInfo->kernelVgpuTypePlacementInfoGi, 0,
               sizeof(pPhysGpuInfo->kernelVgpuTypePlacementInfoGi));
    return NV_OK;
}

/*
 * @brief Sets Guest(VM) ID for the requested hostvgpudevice
 *
 * @param pParams               SET_GUEST_ID_PARAMS Pointer
 * @param pKernelHostVgpuDevice Device for which Vm ID need to be set
 * @param pGpu                  OBJGPU pointer
 *
 * @return NV_STATUS
 */
NV_STATUS
kvgpumgrRegisterGuestId(SET_GUEST_ID_PARAMS *pParams,
                        KERNEL_HOST_VGPU_DEVICE *pKernelHostVgpuDevice, OBJGPU *pGpu)
{
    NV_STATUS rmStatus = NV_OK;
    OBJSYS *pSys = SYS_GET_INSTANCE();
    KernelVgpuMgr *pKernelVgpuMgr = SYS_GET_KERNEL_VGPUMGR(pSys);

    if (pParams->action  == SET_GUEST_ID_ACTION_SET)
    {
        KERNEL_VGPU_GUEST *pVgpuGuest = NULL;

        for (pVgpuGuest = listHead(&(pKernelVgpuMgr->listVgpuGuestHead));
             pVgpuGuest != NULL;
             pVgpuGuest = listNext(&(pKernelVgpuMgr->listVgpuGuestHead), pVgpuGuest))
        {
            if (pParams->vmIdType == pVgpuGuest->guestVmInfo.vmIdType)
            {
                if (pParams->vmIdType == VM_ID_DOMAIN_ID)
                {
                    if (pParams->guestVmId.vmId == pVgpuGuest->guestVmInfo.guestVmId.vmId)
                    {
                        break;
                    }
                }
                else if (pParams->vmIdType == VM_ID_UUID)
                {
                    if (!portMemCmp(pParams->guestVmId.vmUuid,
                                  pVgpuGuest->guestVmInfo.guestVmId.vmUuid,
                                  VM_UUID_SIZE))
                    {
                        break;
                    }
                }
            }
        }

        if (pVgpuGuest == NULL)
        {
            pVgpuGuest = listAppendNew(&(pKernelVgpuMgr->listVgpuGuestHead));
            if (pVgpuGuest == NULL)
                return NV_ERR_NO_MEMORY;

            portMemSet(pVgpuGuest, 0, sizeof(KERNEL_VGPU_GUEST));
        }

        NV_ASSERT_OR_RETURN(pVgpuGuest->numVgpuDevices != MAX_VGPU_DEVICES_PER_VM,
                            NV_ERR_INSUFFICIENT_RESOURCES);

        pVgpuGuest->numVgpuDevices++;

        pKernelHostVgpuDevice->vgpuGuest = pVgpuGuest;

        pVgpuGuest->guestVmInfo.vmIdType = pParams->vmIdType;

        if (pParams->vmIdType == VM_ID_DOMAIN_ID)
        {
            pVgpuGuest->guestVmInfo.guestVmId.vmId = pParams->guestVmId.vmId;
        }
        else if (pParams->vmIdType == VM_ID_UUID)
        {
            portMemCopy(pVgpuGuest->guestVmInfo.guestVmId.vmUuid,
                        VM_UUID_SIZE,
                        pParams->guestVmId.vmUuid,
                        VM_UUID_SIZE);
        }
    }
    else if (pParams->action == SET_GUEST_ID_ACTION_UNSET)
    {
        KERNEL_VGPU_GUEST *pVgpuGuest = pKernelHostVgpuDevice->vgpuGuest;

        if (pVgpuGuest)
        {
            pVgpuGuest->numVgpuDevices--;
            if (pVgpuGuest->numVgpuDevices == 0)
            {
                listRemove(&(pKernelVgpuMgr->listVgpuGuestHead), pVgpuGuest);
            }
            pKernelHostVgpuDevice->vgpuGuest = NULL;
        }
        else
        {
            return NV_WARN_NULL_OBJECT;
        }
    }
    else
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    return rmStatus;
}

static NV_STATUS
_kvgpumgrVgpuAllocGPUInstanceSubscription
(
    OBJGPU *pGpu,
    NvU32 *phClient,
    NvU32 *phDevice,
    NvU32 swizzId
)
{
    NV_STATUS rmStatus;
    NVC637_ALLOCATION_PARAMETERS nvC637AllocParams;
    NvU32 hSubDevice;
    NvU32 hGPUInstanceSubscription;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    NV_ASSERT_OR_RETURN(phClient != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(phDevice != NULL, NV_ERR_INVALID_ARGUMENT);

    NV_ASSERT_OK_OR_RETURN(
        rmapiutilAllocClientAndDeviceHandles(pRmApi, pGpu, phClient, phDevice, &hSubDevice));

    // Allocate GPUInstanceSubscription handle
    NV_ASSERT_OK_OR_GOTO(
        rmStatus,
        serverutilGenResourceHandle(*phClient, &hGPUInstanceSubscription),
        cleanup);
    portMemSet(&nvC637AllocParams, 0, sizeof(nvC637AllocParams));
    nvC637AllocParams.swizzId = swizzId;

    // Allocate GPUInstanceSubscription object
    NV_CHECK_OK_OR_GOTO(
        rmStatus,
        LEVEL_ERROR,
        pRmApi->AllocWithHandle(pRmApi,
                                *phClient,
                                hSubDevice,
                                hGPUInstanceSubscription,
                                AMPERE_SMC_PARTITION_REF,
                                &nvC637AllocParams,
                                sizeof(nvC637AllocParams)),
        cleanup);

    return rmStatus;

cleanup:
    pRmApi->Free(pRmApi, *phClient, *phClient);
    return rmStatus;
}

NV_STATUS
kvgpumgrHeterogeneousGetChidOffset(NvU32 vgpuTypeId, NvU16 placementId,
                                   NvU32 numChannels, NvU64 *pChidOffset)
{
    VGPU_TYPE *pVgpuTypeInfo = NULL;
    NV_STATUS status;
    NvU32 i;
    VGPU_TYPE_SUPPORTED_PLACEMENT_INFO *pVgpuTypeSupportedPlacementInfo;
    VGPU_INSTANCE_SUPPORTED_PLACEMENT_INFO *pVgpuInstanceSupportedPlacementInfo;

    if ((status = kvgpumgrGetVgpuTypeInfo(vgpuTypeId, &pVgpuTypeInfo)) != NV_OK)
        return status;

    if (placementId == NVA081_PLACEMENT_ID_INVALID)
        return NV_ERR_INVALID_ARGUMENT;

    pVgpuTypeSupportedPlacementInfo = &pVgpuTypeInfo->vgpuTypeSupportedPlacementInfo;

    /* Channel count provided by plugin and calculated by RM should be same */
    if (numChannels != pVgpuTypeSupportedPlacementInfo->channelCount)
        return NV_ERR_INVALID_STATE;

    if (pVgpuTypeSupportedPlacementInfo->heterogeneousPlacementCount > MAX_VGPU_DEVICES_PER_PGPU)
        return NV_ERR_INVALID_INDEX;

    for (i = 0; i < pVgpuTypeSupportedPlacementInfo->heterogeneousPlacementCount; i++)
    {
        /*
         * For the placementId of the vGPU instance, find the corresponding
         * channel offset. heterogeneousSupportedPlacementId and
         * heterogeneousSupportedChidOffset share the same array index for a
         * give placement ID.
         */
        pVgpuInstanceSupportedPlacementInfo = &pVgpuTypeSupportedPlacementInfo->vgpuInstanceSupportedPlacementInfo[i];

        if (placementId == pVgpuInstanceSupportedPlacementInfo->heterogeneousSupportedPlacementId)
        {
            *pChidOffset = pVgpuInstanceSupportedPlacementInfo->heterogeneousSupportedChidOffset;
            return NV_OK;
        }
    }

    return NV_ERR_OBJECT_NOT_FOUND;
}

NV_STATUS
kvgpumgrHomogeneousGetChidOffset(NvU32 vgpuTypeId, NvU16 placementId,
                                 NvU32 numChannels, NvU64 *pChidOffset)
{
    VGPU_TYPE *pVgpuTypeInfo = NULL;
    NV_STATUS status;
    NvU32 i;
    VGPU_TYPE_SUPPORTED_PLACEMENT_INFO *pVgpuTypeSupportedPlacementInfo;
    VGPU_INSTANCE_SUPPORTED_PLACEMENT_INFO *pVgpuInstanceSupportedPlacementInfo;

    if ((status = kvgpumgrGetVgpuTypeInfo(vgpuTypeId, &pVgpuTypeInfo)) != NV_OK)
        return status;

    if (placementId == NVA081_PLACEMENT_ID_INVALID)
        return NV_ERR_INVALID_ARGUMENT;

    pVgpuTypeSupportedPlacementInfo = &pVgpuTypeInfo->vgpuTypeSupportedPlacementInfo;

    /* Channel count provided by plugin and calculated by RM should be same */
    if (numChannels != pVgpuTypeSupportedPlacementInfo->channelCount)
        return NV_ERR_INVALID_STATE;

    if (pVgpuTypeSupportedPlacementInfo->homogeneousPlacementCount > MAX_VGPU_DEVICES_PER_PGPU)
        return NV_ERR_INVALID_INDEX;

    for (i = 0; i < pVgpuTypeSupportedPlacementInfo->homogeneousPlacementCount; i++)
    {
        pVgpuInstanceSupportedPlacementInfo = &pVgpuTypeSupportedPlacementInfo->vgpuInstanceSupportedPlacementInfo[i];

        /*
         * For the placementId of the vGPU instance, find the corresponding
         * channel offset. homogeneousSupportedPlacementIds and
         * homogeneousSupportedChidOffsets share the same array index for a
         * give placement ID.
         */
        if (placementId == pVgpuInstanceSupportedPlacementInfo->homogeneousSupportedPlacementId)
        {
            *pChidOffset = pVgpuInstanceSupportedPlacementInfo->homogeneousSupportedChidOffset;
            return NV_OK;
        }
    }

    return NV_ERR_OBJECT_NOT_FOUND;
}


NV_STATUS
kvgpumgrGuestRegister(OBJGPU *pGpu,
                      NvU32 gfid,
                      NvU32 vgpuType,
                      NvU32 vmPid,
                      VM_ID_TYPE vmIdType,
                      VM_ID guestVmId,
                      NvHandle hPluginFBAllocationClient,
                      NvU32 numChannels,
                      NvU32 numPluginChannels,
                      NvU32 swizzId,
                      NvU32 vgpuDeviceInstanceId,
                      NvBool bDisableDefaultSmcExecPartRestore,
                      NvU16 placementId,
                      NvU8 *pVgpuDevName,
                      KERNEL_HOST_VGPU_DEVICE **ppKernelHostVgpuDevice)
{
    NV_STATUS                rmStatus                   = NV_OK;
    OBJSYS                  *pSys                       = SYS_GET_INSTANCE();
    KernelVgpuMgr           *pKernelVgpuMgr             = SYS_GET_KERNEL_VGPUMGR(pSys);
    SET_GUEST_ID_PARAMS      setGuestIDParams           = {0};
    NvU32                    i;
    KERNEL_PHYS_GPU_INFO    *pPhysGpuInfo;
    KERNEL_HOST_VGPU_DEVICE *pKernelHostVgpuDevice      = NULL;
    NvBool                   bMIGInUse                  = IS_MIG_IN_USE(pGpu);

    NvU32                    j;
    NvU32                    vgpuTypeIdx                = 0;
    REQUEST_VGPU_INFO_NODE  *pRequestVgpu               = NULL;

    NV_PRINTF(LEVEL_INFO, "Enter function\n");

    if (numPluginChannels > VGPU_MAX_PLUGIN_CHANNELS)
    {
        NV_PRINTF(LEVEL_ERROR, "failed, wrong number of plugin channels\n");
        return NV_ERR_INVALID_PARAMETER;
    }

    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_VIRTUALIZATION_MODE_HOST_VSGA))
    {
        NV_PRINTF(LEVEL_ERROR, "GPU is previously set in vSGA mode!\n");
        return NV_ERR_INVALID_DEVICE;
    }

    if (gpuIsSriovEnabled(pGpu))
    {
        GFID_ALLOC_STATUS gfidState;
        NV_ASSERT_OK_OR_RETURN(gpuGetGfidState(pGpu, gfid, &gfidState));
        NV_ASSERT_OR_RETURN((gfidState == GFID_FREE), NV_ERR_INSUFFICIENT_RESOURCES);
    }
    else
    {
        // MIG is supported only with SRIOV. Legacy mode doesn't support MIG
        NV_ASSERT_OR_RETURN(!bMIGInUse, NV_ERR_INVALID_STATE);
        numPluginChannels = 0;
    }

    if (kvgpumgrGetPgpuIndex(pKernelVgpuMgr, pGpu->gpuId, &i) != NV_OK)
        return NV_ERR_OBJECT_NOT_FOUND;

    pPhysGpuInfo = &(pKernelVgpuMgr->pgpuInfo[i]);

    // check if this vgpu type is allowed on physical gpu
    for (j = 0; j < pPhysGpuInfo->numVgpuTypes; j++)
    {
        if (vgpuType == pPhysGpuInfo->vgpuTypes[j]->vgpuTypeId)
        {
            vgpuTypeIdx = j;
            break;
        }
    }

    if (j == pPhysGpuInfo->numVgpuTypes)
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    // On KVM, store a link to the corresponding REQUEST_VGPU_INFO_NODE in
    // KERNEL_HOST_VGPU_DEVICE and vice versa, so as to create a mapping between
    // vGPU device and the corresponding mdev device.
    if (osIsVgpuVfioPresent() == NV_OK)
    {
        for (pRequestVgpu = listHead(&pKernelVgpuMgr->listRequestVgpuHead);
             pRequestVgpu != NULL;
             pRequestVgpu = listNext(&pKernelVgpuMgr->listRequestVgpuHead, pRequestVgpu))
        {
            if (portMemCmp(pRequestVgpu->vgpuDevName, pVgpuDevName, VM_UUID_SIZE) == 0)
                break;
        }

        if (pRequestVgpu == NULL)
        {
            return NV_ERR_OBJECT_NOT_FOUND;
        }
    }

    /*
     * For MIG mode, vGPU type is already validated based on swizzid in
     * NVA081_CTRL_CMD_VGPU_CONFIG_[GET_FREE|VALIDATE]_SWIZZID RmCtrl.
     * For heterogeneous vGPU mode, vGPU type is already validated based on placement ID
     * in NVA081_CTRL_CMD_VGPU_CONFIG_UPDATE_HETEROGENEOUS_INFO RmCtrl.
     * Both the RmCtrls are done before allocating the A084 object.
     */
    if (!IS_MIG_ENABLED(pGpu) && !pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_VGPU_HETEROGENEOUS_MODE))
    {
        rmStatus = _kvgpumgrCheckVgpuTypeActivatable(pGpu, pPhysGpuInfo, pPhysGpuInfo->vgpuTypes[vgpuTypeIdx]);
        if (rmStatus != NV_OK)
            return rmStatus;

        rmStatus = kvgpumgrSetVgpuType(pGpu, pPhysGpuInfo, vgpuType, KMIGMGR_SWIZZID_INVALID);
        if (rmStatus != NV_OK)
            return rmStatus;
    }

    if (listCount(&(pPhysGpuInfo->listHostVgpuDeviceHead)) == MAX_VGPU_DEVICES_PER_PGPU)
    {
        rmStatus = NV_ERR_INSUFFICIENT_RESOURCES;
        goto failed;
    }

    // Register guest vgpu device instance
    pKernelHostVgpuDevice = listAppendNew(&(pPhysGpuInfo->listHostVgpuDeviceHead));
    if (pKernelHostVgpuDevice == NULL)
    {
        rmStatus = NV_ERR_NO_MEMORY;
        goto failed;
    }

    portMemSet(pKernelHostVgpuDevice, 0, sizeof(KERNEL_HOST_VGPU_DEVICE));

    pKernelHostVgpuDevice->vgpuType = vgpuType;

    NV_ASSERT_OR_RETURN(((vmIdType == VM_ID_DOMAIN_ID) ||
                         (vmIdType == VM_ID_UUID)),
                         NV_ERR_INVALID_ARGUMENT);

    setGuestIDParams.action   = SET_GUEST_ID_ACTION_SET;
    setGuestIDParams.vmPid    = vmPid;
    setGuestIDParams.vmIdType = vmIdType;
    if (vmIdType == VM_ID_DOMAIN_ID)
    {
        setGuestIDParams.guestVmId.vmId = guestVmId.vmId;
    }
    else if (vmIdType == VM_ID_UUID)
    {
        portMemCopy(setGuestIDParams.guestVmId.vmUuid,
                    VM_UUID_SIZE,
                    guestVmId.vmUuid,
                    VM_UUID_SIZE);
    }
    if (NV_OK != (rmStatus = kvgpumgrRegisterGuestId(&setGuestIDParams, pKernelHostVgpuDevice, pGpu)))
    {
        goto failed;
    }

    pKernelHostVgpuDevice->gfid                              = gfid;
    pKernelHostVgpuDevice->swizzId                           = swizzId;
    pKernelHostVgpuDevice->numPluginChannels                 = numPluginChannels;
    pKernelHostVgpuDevice->bDisableDefaultSmcExecPartRestore = bDisableDefaultSmcExecPartRestore;
    pKernelHostVgpuDevice->placementId                       = NVA081_PLACEMENT_ID_INVALID;

    if (IS_MIG_ENABLED(pGpu))
    {
        VGPU_TYPE *pVgpuTypeInfo;

        NV_ASSERT_OK_OR_RETURN(kvgpumgrGetVgpuTypeInfo(pPhysGpuInfo->vgpuTypes[vgpuTypeIdx]->vgpuTypeId,
                                                       &pVgpuTypeInfo));

        if (pVgpuTypeInfo->maxInstancePerGI > 1)
        {
            pKernelHostVgpuDevice->bDisableDefaultSmcExecPartRestore = NV_TRUE;
        }
    }

    if (osIsVgpuVfioPresent() == NV_OK)
    {
        pKernelHostVgpuDevice->pRequestVgpuInfoNode = pRequestVgpu;
        pRequestVgpu->pKernelHostVgpuDevice = pKernelHostVgpuDevice;
    }

    pKernelHostVgpuDevice->hPluginFBAllocationClient     = hPluginFBAllocationClient;
    pKernelHostVgpuDevice->vgpuDeviceGuestFbInfo.bValid  = NV_FALSE;
    pKernelHostVgpuDevice->vgpuDeviceGuestFbInfo.offset  = 0;
    pKernelHostVgpuDevice->vgpuDeviceGuestFbInfo.length  = 0;
    pKernelHostVgpuDevice->bOfflinedPageInfoValid        = NV_FALSE;

    pPhysGpuInfo->numActiveVgpu++;

    if (gpuIsSriovEnabled(pGpu))
    {
        // Set GFID in use
        gpuSetGfidUsage(pGpu, gfid, NV_TRUE);
    }

    // If MIG is enabled, then we need to set faultIds and per GPU instance VEID
    // offset in MMU_CFG registers. In MIG with memory partitioning, every
    // VM should be associated with a GPU instance, so set these params for
    // GPU instance engines
    //
    if (gpuIsSriovEnabled(pGpu) && bMIGInUse)
    {
        //
        // Alloc GPUInstanceSubscription object inside hostVgpuDeviceObject. This is
        // needed because hostVgpuDevice can be duped under any client and
        // will get freed after subdevice, so during hostVgpuDeviceDestruct
        // to get GPU instance reference, we need to keep the GPU instance alive
        //
        NV_ASSERT_OK_OR_GOTO(rmStatus, _kvgpumgrVgpuAllocGPUInstanceSubscription(pGpu,
                                                                                 &pKernelHostVgpuDevice->hMigClient,
                                                                                 &pKernelHostVgpuDevice->hMigDevice,
                                                                                 swizzId),
                             failed);
    }

    // Save/Restore default compute instance if not disabled during allocation
    if (gpuIsSriovEnabled(pGpu) && bMIGInUse && !pKernelHostVgpuDevice->bDisableDefaultSmcExecPartRestore)
    {
        KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance;
        KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);

        // Get GPU instance
        NV_ASSERT_OK_OR_GOTO(rmStatus,
                             kmigmgrGetGPUInstanceInfo(pGpu, pKernelMIGManager,
                                                       swizzId, &pKernelMIGGpuInstance),
                             failed);
        //
        // Save any default compute instances as we may need to restore
        // them during destruct
        //
        NV_ASSERT_OK_OR_GOTO(rmStatus,
                             kmigmgrSaveComputeInstances(pGpu, pKernelMIGManager, pKernelMIGGpuInstance,
                                                         pKernelHostVgpuDevice->savedExecPartitions),
                             failed);

        // Determine if we have a VEID fragmentation between compute instances
        if (!kmigmgrIsPartitionVeidAllocationContiguous(pGpu, pKernelMIGManager, pKernelMIGGpuInstance))
        {
            // delete all compute instances and recreate then again
            NV_ASSERT_OK_OR_GOTO(rmStatus,
                                 kvgpuMgrRestoreSmcExecPart(pGpu,
                                                           pKernelHostVgpuDevice,
                                                           pKernelMIGGpuInstance),
                                 failed);
        }
    }

    *ppKernelHostVgpuDevice = pKernelHostVgpuDevice;

    return rmStatus;

failed:
    // Unset gfid usage
    if (gpuIsSriovEnabled(pGpu))
        gpuSetGfidUsage(pGpu, gfid, NV_FALSE);

    if (pKernelHostVgpuDevice != NULL)
    {
        if (pKernelHostVgpuDevice->vgpuGuest != NULL)
        {
            setGuestIDParams.action = SET_GUEST_ID_ACTION_UNSET;
            kvgpumgrRegisterGuestId(&setGuestIDParams, pKernelHostVgpuDevice, pGpu);
        }
        listRemove(&(pPhysGpuInfo->listHostVgpuDeviceHead), pKernelHostVgpuDevice);
    }

    if (osIsVgpuVfioPresent() != NV_OK && // not KVM
        swizzId != KMIGMGR_SWIZZID_INVALID)
        _kvgpumgrClearAssignedSwizzIdMask(pGpu, swizzId);

    return rmStatus;
}

NV_STATUS
kvgpumgrGuestUnregister(OBJGPU *pGpu, KERNEL_HOST_VGPU_DEVICE *pKernelHostVgpuDevice)
{
    NV_STATUS                    rmStatus        = NV_OK;
    OBJSYS                      *pSys            = SYS_GET_INSTANCE();
    KernelVgpuMgr               *pKernelVgpuMgr  = SYS_GET_KERNEL_VGPUMGR(pSys);
    RM_API                      *pRmApi          = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    KERNEL_PHYS_GPU_INFO        *pPhysGpuInfo;
    NvU32                        gfid            = pKernelHostVgpuDevice->gfid;
    SET_GUEST_ID_PARAMS          setGuestIDParams = {0};
    NvU32                        i;

    NV_PRINTF(LEVEL_INFO, "Enter function\n");

    if (gpuIsSriovEnabled(pGpu))
    {
        GFID_ALLOC_STATUS gfidState;

        // Sanity check on GFID
        NV_ASSERT_OK_OR_GOTO(rmStatus, gpuGetGfidState(pGpu, gfid, &gfidState), done);

        // Keep continuing in unregister path after sanity check
        if (gfidState == GFID_FREE)
        {
            NV_PRINTF(LEVEL_ERROR, "Request to unregister Invalid GFID\n");
            rmStatus = NV_ERR_INVALID_STATE;
            goto done;
        }
    }

    if (IS_MIG_IN_USE(pGpu) && !pKernelHostVgpuDevice->bDisableDefaultSmcExecPartRestore)
    {
        KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
        KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance;

        NV_ASSERT_OK_OR_GOTO(rmStatus, kmigmgrGetGPUInstanceInfo(pGpu, pKernelMIGManager,
                                                                pKernelHostVgpuDevice->swizzId,
                                                                &pKernelMIGGpuInstance),
                            done);

        NV_ASSERT_OK_OR_GOTO(rmStatus,
                            kvgpuMgrRestoreSmcExecPart(pGpu,
                                                        pKernelHostVgpuDevice,
                                                        pKernelMIGGpuInstance),
                            done);

        portMemSet(&pKernelHostVgpuDevice->savedExecPartitions, 0,
                sizeof(pKernelHostVgpuDevice->savedExecPartitions));
    }

    if (!gpuIsSriovEnabled(pGpu))
    {
        portMemFree(pKernelHostVgpuDevice->pGuestFbSegment);
        pKernelHostVgpuDevice->pGuestFbSegment = NULL;
    }

    if (pKernelHostVgpuDevice->hbmRegionList != NULL)
    {
        portMemFree(pKernelHostVgpuDevice->hbmRegionList);
        pKernelHostVgpuDevice->hbmRegionList = NULL;
        pKernelHostVgpuDevice->numValidHbmRegions = 0;
    }

    if (gpuIsSriovEnabled(pGpu))
    {
        if (IS_GSP_CLIENT(pGpu))
        {
            KernelFifo *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);
            ENGINE_INFO tmpEngineInfo;
            Device *pMigDevice = NULL;

            portMemSet(&tmpEngineInfo, 0, sizeof(ENGINE_INFO));

            if (IS_MIG_IN_USE(pGpu))
            {
                RsClient *pClient;

                NV_ASSERT_OK_OR_GOTO(rmStatus,
                                     serverGetClientUnderLock(&g_resServ, pKernelHostVgpuDevice->hMigClient, &pClient),
                                     free_fifo_chids_exit);

                NV_ASSERT_OK_OR_GOTO(rmStatus,
                                     deviceGetByHandle(pClient, pKernelHostVgpuDevice->hMigDevice, &pMigDevice),
                                     free_fifo_chids_exit);
            }

            NV_ASSERT_OK_OR_GOTO(rmStatus,
                                 kfifoGetHostDeviceInfoTable_HAL(pGpu, pKernelFifo, &tmpEngineInfo, pMigDevice),
                                 free_fifo_chids_exit);

            vgpuMgrFreeSystemChannelIDs(pGpu,
                                        pKernelHostVgpuDevice->gfid,
                                        pKernelHostVgpuDevice->chidOffset,
                                        pKernelHostVgpuDevice->channelCount,
                                        pMigDevice,
                                        tmpEngineInfo.engineInfoListSize,
                                        tmpEngineInfo.engineInfoList);

free_fifo_chids_exit:
            portMemFree(tmpEngineInfo.engineInfoList);
            tmpEngineInfo.engineInfoList = NULL;
        }

        // Unset GFID usage
        gpuSetGfidUsage(pGpu, gfid, NV_FALSE);
    }

done:
    if (IS_MIG_IN_USE(pGpu))
    {
        pRmApi->Free(pRmApi, pKernelHostVgpuDevice->hMigClient, pKernelHostVgpuDevice->hMigClient);
        pKernelHostVgpuDevice->hMigClient = NV01_NULL_OBJECT;
        pKernelHostVgpuDevice->hMigDevice = NV01_NULL_OBJECT;
    }

    if (kvgpumgrGetPgpuIndex(pKernelVgpuMgr, pGpu->gpuId, &i) != NV_OK)
    {
        return NV_ERR_OBJECT_NOT_FOUND;
    }

    // Unset the guest id for host vGPU device
    setGuestIDParams.action = SET_GUEST_ID_ACTION_UNSET;
    if (NV_OK != (rmStatus = kvgpumgrRegisterGuestId(&setGuestIDParams,
                                                     pKernelHostVgpuDevice,
                                                     pGpu)))
    {
        // In case of Hyper-V, On VM reboot KERNEL_HOST_VGPU_DEVICE is not destroyed.
        // As guest id is unset during VM reboot by plugin explicitly. So we
        // will receive NV_WARN_NULL_OBJECT here and we need to ignore it.
        //
        // In case, plugin doesn't get a chance to unset the guest id e.g. in case
        // of crash or Force shutdown/Turn off then we need to get it unset here.
        if (hypervisorIsType(OS_HYPERVISOR_HYPERV) && rmStatus == NV_WARN_NULL_OBJECT)
        {
            rmStatus = NV_OK;
        }
        else
        {
            return rmStatus;
        }
    }

    pPhysGpuInfo = &(pKernelVgpuMgr->pgpuInfo[i]);

    NV_ASSERT_OR_RETURN((pPhysGpuInfo != NULL), NV_ERR_INVALID_ARGUMENT);

    pPhysGpuInfo->numActiveVgpu--;

    if (pPhysGpuInfo->numActiveVgpu == 0 && pPhysGpuInfo->numCreatedVgpu == 0)
    {
        kvgpumgrSetVgpuType(pGpu, pPhysGpuInfo, NVA081_CTRL_VGPU_CONFIG_INVALID_TYPE, pKernelHostVgpuDevice->swizzId);
    }

    if (pKernelHostVgpuDevice->pRequestVgpuInfoNode != NULL)
    {
        pKernelHostVgpuDevice->pRequestVgpuInfoNode->pKernelHostVgpuDevice = NULL;
    }

    if (osIsVgpuVfioPresent() != NV_OK) // Not KVM
    {
        if (pKernelHostVgpuDevice->swizzId != KMIGMGR_SWIZZID_INVALID)
            rmStatus = _kvgpumgrClearAssignedSwizzIdMask(pGpu, pKernelHostVgpuDevice->swizzId);
    }

    /* 
     * Accumulate the freed placement ID in creatable placement ID array.
     * In case of KVM, it will be happened at the time of mdev device removal. 
     */
    if (osIsVgpuVfioPresent() != NV_OK)
    {
        if (pKernelHostVgpuDevice->placementId != NVA081_PLACEMENT_ID_INVALID)
        {
            NvBool bHeterogeneousModeEnabled;

            NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                kvgpuMgrGetHeterogeneousMode(pGpu,
                                             pKernelHostVgpuDevice->swizzId,
                                             &bHeterogeneousModeEnabled));

            if (bHeterogeneousModeEnabled)
            {
                rmStatus = _kvgpumgrUpdateHeterogeneousCreatablePlacementIds(pGpu,
                                    pKernelHostVgpuDevice->placementId,
                                    pKernelHostVgpuDevice->vgpuType, NV_FALSE,
                                    pKernelHostVgpuDevice->swizzId);
            }

            if (kvgpumgrCheckHomogeneousPlacementSupported(pGpu, pKernelHostVgpuDevice->swizzId) == NV_OK)
            {
                rmStatus = _kvgpumgrUpdateHomogeneousCreatablePlacementIds(pGpu,
                                        pKernelHostVgpuDevice->placementId,
                                        pKernelHostVgpuDevice->vgpuType, NV_FALSE,
                                        pKernelHostVgpuDevice->swizzId);
            }

            if (rmStatus != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR, "Failed to free placement ID: %d, error: 0x%x\n",
                          pKernelHostVgpuDevice->placementId, rmStatus);
            }
        }
    }

    listRemove(&(pPhysGpuInfo->listHostVgpuDeviceHead), pKernelHostVgpuDevice);

    return rmStatus;
}

//
// Helper function to check if pGPU is live migration capable.
//
NvBool
kvgpumgrCheckPgpuMigrationSupport(OBJGPU *pGpu)
{
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV2080_CTRL_VGPU_MGR_INTERNAL_GET_PGPU_MIGRATION_SUPPORT_PARAMS params = {0};
    NV_STATUS status;

    NV_CHECK_OK_OR_ELSE(status,
                        LEVEL_ERROR,
                        pRmApi->Control(pRmApi, pGpu->hInternalClient, pGpu->hInternalSubdevice,
                                        NV2080_CTRL_CMD_VGPU_MGR_INTERNAL_GET_PGPU_MIGRATION_SUPPORT,
                                        &params, sizeof(params)),
                        return NV_FALSE);

    return params.bIsMigrationSupported;
}

//
// Function to convert a NvU32 value to a ascii-encoded string.
// The value will be padded with 0 to use up the totalWidth. The value
// requiring space more that totalWidth will be truncated.
//
static NvU32
NvU32ToAsciiStr(NvU32 uval, NvU32 totalWidth, NvU8 *dest, NvBool invalid)
{
    NvU32 digitCount = 0;
    NvU8 nibble;

    if (totalWidth == 0)
        return (totalWidth);

    // Move the pointer to the end
    dest += (totalWidth - 1);

    // Start copying the data from the end in the reverse order
    while (digitCount < totalWidth)
    {
        digitCount++;
        if (invalid == NV_TRUE)
        {
            *dest-- = 'X';
            continue;
        }

        nibble = uval & 0xF;
        *dest-- = (nibble <= 9) ? (nibble + '0') : (nibble - 10 + 'A');
        uval = uval >> 4;
    }

    return (totalWidth);
}

NvU32 kvgpumgrGetPgpuDevIdEncoding(OBJGPU *pGpu, NvU8 *pgpuString,
                                   NvU32 strSize)
{
    NvU32 chipID = DRF_VAL(_PCI, _DEVID, _DEVICE, pGpu->idInfo.PCIDeviceID);
    NvU32 subID  = DRF_VAL(_PCI, _DEVID, _DEVICE, pGpu->idInfo.PCISubDeviceID);

    _get_chip_id_for_alias_pgpu(&chipID, &subID);

    if ((strSize < MAX_NVU32_TO_CONVERTED_STR_LEN)
        || (pgpuString == NULL))
    {
        return NV_U32_MAX;
    }

    // The encoding of the Dev ID is the value converted to string
    return (NvU32ToAsciiStr(chipID, DEVID_ENCODED_VALUE_SIZE, pgpuString, NV_FALSE));
}

NvU32 kvgpumgrGetPgpuSubdevIdEncoding(OBJGPU *pGpu, NvU8 *pgpuString,
                                      NvU32 strSize)
{
    NvU32 chipID = DRF_VAL(_PCI, _DEVID, _DEVICE, pGpu->idInfo.PCIDeviceID);
    NvU32 subID  = DRF_VAL(_PCI, _DEVID, _DEVICE, pGpu->idInfo.PCISubDeviceID);
    NvU32 bytes = 0;

    _get_chip_id_for_alias_pgpu(&chipID, &subID);

    if ((strSize < MAX_NVU32_TO_CONVERTED_STR_LEN)
        || (pgpuString == NULL))
    {
        return NV_U32_MAX;
    }

    // The encoding of the subdevice ID is its value converted to string
    bytes = NvU32ToAsciiStr(subID, SUBDEVID_ENCODED_VALUE_SIZE,
                                    pgpuString, NV_FALSE);
    return bytes;
}

NvU32 kvgpumgrGetPgpuFSEncoding(OBJGPU *pGpu, NvU8 *pgpuString,
                                NvU32 strSize)
{
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV2080_CTRL_VGPU_MGR_INTERNAL_GET_PGPU_FS_ENCODING_PARAMS params = {0};
    NV_STATUS status;

    params.pgpuStringSize = strSize;

    NV_CHECK_OK_OR_ELSE(status,
                          LEVEL_ERROR,
                          pRmApi->Control(pRmApi, pGpu->hInternalClient, pGpu->hInternalSubdevice,
                                          NV2080_CTRL_CMD_VGPU_MGR_INTERNAL_GET_PGPU_FS_ENCODING,
                                          &params, sizeof(params)),
                          return NV_U32_MAX);

    portMemCopy(pgpuString, strSize, params.pgpuString, params.pgpuStringSize);

    return params.pgpuStringSize;
}

NvU64
kvgpumgrGetEccAndPrReservedFb(OBJGPU *pGpu)
{
    Heap *pHeap = GPU_GET_HEAP(pGpu);
	const MEMORY_SYSTEM_STATIC_CONFIG *pMemorySystemConfig =
            kmemsysGetStaticConfig(pGpu, GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu));
	NvU64 eccReservedFb = 0, pageRetirementReservedFb = 0, usableSizeFb;

    //
    // pageRetirementReservedFb is needed only in case of legacy, to account
    // for the pagepool used during page stitching
    //
    if (!gpuIsSriovEnabled(pGpu))
    {
        if (pGpu->getProperty(pGpu, PDB_PROP_GPU_ALLOW_PAGE_RETIREMENT))
        {
            if (IsPASCALorBetter(pGpu))
            {
                pageRetirementReservedFb = NV2080_CTRL_FB_OFFLINED_PAGES_MAX_PAGES * RM_PAGE_SIZE_HUGE;
            }
            else
            {
                pageRetirementReservedFb = NV2080_CTRL_FB_OFFLINED_PAGES_MAX_PAGES * RM_PAGE_SIZE_64K;
            }
        }
    }

    if (pMemorySystemConfig->bEnabledEccFBPA)
    {
        if ((pMemorySystemConfig->ramType != NV2080_CTRL_FB_INFO_RAM_TYPE_HBM1) &&
            (pMemorySystemConfig->ramType != NV2080_CTRL_FB_INFO_RAM_TYPE_HBM2) &&
            (pMemorySystemConfig->ramType != NV2080_CTRL_FB_INFO_RAM_TYPE_HBM3))
        {
            heapGetUsableSize(pHeap, &usableSizeFb);
            //
            // FB and ECC checksum calculation
            // eccReservedFb = totalFBEccOff / 16
            // i.e. totalFbEccOff = eccReservedFb * 16
            //
            // totalFbEccOff = totalFbEccOn + eccReservedFb
            // eccReservedFb * 16 = totalFbEccOn + eccReservedFb
            // eccReservedFb * 15 = totalFbEccOn
            // eccReservedFb = totalFbEccOn / 15 (totalFbEccOn is same as usableSizeFb)
            //
            eccReservedFb = usableSizeFb / 15;
        }
    }

	return pageRetirementReservedFb + eccReservedFb;
}

//
// A 32-bit variable is used to consolidate various GPU capabilities like
// ECC, SRIOV etc. The function sets the capabilities in the variable and
// converted to an ascii-encoded format.
//
NvU32 kvgpumgrGetPgpuCapEncoding(OBJGPU *pGpu, NvU8 *pgpuString, NvU32 strSize)
{
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NvU32 pgpuCap = 0;

    if ((strSize < MAX_NVU32_TO_CONVERTED_STR_LEN) || (pgpuString == NULL))
    {
        return NV_U32_MAX;
    }

    {
        NV2080_CTRL_GPU_QUERY_ECC_STATUS_PARAMS eccStatus = {0};

        if (pRmApi->Control(pRmApi,
                            pGpu->hInternalClient,
                            pGpu->hInternalSubdevice,
                            NV2080_CTRL_CMD_GPU_QUERY_ECC_STATUS,
                            &eccStatus, sizeof(eccStatus)) == NV_OK)
        {
            NvU32 i;
            for (i = 0; i < NV2080_CTRL_GPU_ECC_UNIT_COUNT; i++)
            {
                if (eccStatus.units[i].enabled)
                {
                    pgpuCap |= PGPU_CAP_ECC_ON;
                    break;
                }
            }
        }
    }

    {
        NV0080_CTRL_GPU_GET_SRIOV_CAPS_PARAMS sriovCaps = {0};

        if (gpuGetSriovCaps_HAL(pGpu, &sriovCaps) == NV_OK)
        {
            if (sriovCaps.bSriovEnabled)
                pgpuCap |= PGPU_CAP_SRIOV_ON;
        }
    }

    return NvU32ToAsciiStr(pgpuCap, PGPU_CAP_ENCODED_VALUE_SIZE,
                           pgpuString, NV_FALSE);
}

/*
 * Get the user provide vGPU version range
 */
NV_STATUS
kvgpumgrGetHostVgpuVersion(NvU32 *user_min_supported_version,
                           NvU32 *user_max_supported_version)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    KernelVgpuMgr *pKernelVgpuMgr = SYS_GET_KERNEL_VGPUMGR(pSys);

    *user_min_supported_version
        = pKernelVgpuMgr->user_min_supported_version;
    *user_max_supported_version
        = pKernelVgpuMgr->user_max_supported_version;
    return NV_OK;
}

/*
 * Set the user provide vGPU version range
 */
NV_STATUS
kvgpumgrSetHostVgpuVersion(NvU32 user_min_supported_version,
                           NvU32 user_max_supported_version)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    KernelVgpuMgr *pKernelVgpuMgr = SYS_GET_KERNEL_VGPUMGR(pSys);
    NvU32 host_min_supported_version
        = GRIDSW_VERSION_EXTERNAL(NV_VGPU_MIN_SUPPORTED_GRIDSW_VERSION_EXTERNAL_MAJOR,
                                  NV_VGPU_MIN_SUPPORTED_GRIDSW_VERSION_EXTERNAL_MINOR);
    NvU32 host_max_supported_version
        = GRIDSW_VERSION_EXTERNAL(NV_VGPU_MAX_SUPPORTED_GRIDSW_VERSION_EXTERNAL_MAJOR,
                                  NV_VGPU_MAX_SUPPORTED_GRIDSW_VERSION_EXTERNAL_MINOR);

    /* Sanity check */
    if (user_min_supported_version > user_max_supported_version) {
        NV_PRINTF(LEVEL_ERROR,
                  "Maximum vGPU version (0x%x) being set is less than minimum version (0x%x)\n",
                  user_max_supported_version, user_min_supported_version);
        return NV_ERR_INVALID_ARGUMENT;
    }

    /* Sanity check: Fail in case the range being set by admin is a subset
     * of the range supported by the host driver
     */
    if ((user_min_supported_version < host_min_supported_version)
        || (user_max_supported_version > host_max_supported_version)) {

        NV_PRINTF(LEVEL_ERROR,
                  "vGPU version range being set (0x%x, 0x%x) is outside the range supported by host (0x%x, 0x%x)\n",
                  user_min_supported_version, user_max_supported_version,
                  host_min_supported_version, host_max_supported_version);

        return NV_ERR_INVALID_ARGUMENT;
    }

    NV_PRINTF(LEVEL_INFO,
              "vGPU version range enforced by user: (0x%x, 0x%x)\n",
              user_min_supported_version, user_max_supported_version);

    /* Save the information of the user specified range in the host */
    pKernelVgpuMgr->user_min_supported_version
        = user_min_supported_version;
    pKernelVgpuMgr->user_max_supported_version
        = user_max_supported_version;

    return NV_OK;
}

/*
 * Function to set swizzId is assigned to a vGPU device.
 */
static NV_STATUS
_kvgpumgrSetAssignedSwizzIdMask(OBJGPU       *pGpu,
                                VGPU_TYPE *vgpuTypeInfo,
                                NvU32         swizzId)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    KernelVgpuMgr *pKernelVgpuMgr = SYS_GET_KERNEL_VGPUMGR(pSys);
    KERNEL_PHYS_GPU_INFO *pPhysGpuInfo;
    NvU32 i;
    NvU64 mask;

    if (swizzId >= KMIGMGR_MAX_GPU_SWIZZID)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (kvgpumgrGetPgpuIndex(pKernelVgpuMgr, pGpu->gpuId, &i) != NV_OK)
    {
        return NV_ERR_OBJECT_NOT_FOUND;
    }

    pPhysGpuInfo = &(pKernelVgpuMgr->pgpuInfo[i]);
    NV_ASSERT_OR_RETURN((pPhysGpuInfo != NULL), NV_ERR_INVALID_ARGUMENT);

    /* Validate that same ID is not already set and then set the ID */
    mask = NVBIT64(swizzId);

    if (pPhysGpuInfo->assignedSwizzIdVgpuCount[swizzId] < vgpuTypeInfo->maxInstancePerGI)
    {
        pPhysGpuInfo->assignedSwizzIdVgpuCount[swizzId]++;
        pPhysGpuInfo->assignedSwizzIdMask |= mask;
    }
    else
    {
        NV_PRINTF(LEVEL_ERROR, "SwizzID - %d max vgpu count is %d\n", swizzId, vgpuTypeInfo->maxInstancePerGI);
        DBG_BREAKPOINT();
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    return NV_OK;
}

/*!
 * Function to mark swizzId is free to be used by other vGPU devices.
 */
static NV_STATUS
_kvgpumgrClearAssignedSwizzIdMask(OBJGPU *pGpu,
                                  NvU32 swizzId)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    KernelVgpuMgr *pKernelVgpuMgr = SYS_GET_KERNEL_VGPUMGR(pSys);
    KERNEL_PHYS_GPU_INFO *pPhysGpuInfo;
    NvU32 i;
    NvU64 mask;

    if (swizzId >= KMIGMGR_MAX_GPU_SWIZZID)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (kvgpumgrGetPgpuIndex(pKernelVgpuMgr, pGpu->gpuId, &i) != NV_OK)
    {
        return NV_ERR_OBJECT_NOT_FOUND;
    }

    pPhysGpuInfo = &(pKernelVgpuMgr->pgpuInfo[i]);
    NV_ASSERT_OR_RETURN((pPhysGpuInfo != NULL), NV_ERR_INVALID_ARGUMENT);

    /* Validate that same ID is not already marked as free and then set the ID */
    mask = NVBIT64(swizzId);

    if (!(mask & pPhysGpuInfo->assignedSwizzIdMask))
    {
        NV_PRINTF(LEVEL_ERROR, "SwizzID - %d not in use\n", swizzId);
        DBG_BREAKPOINT();
        return NV_ERR_INVALID_STATE;
    }

    if (pPhysGpuInfo->assignedSwizzIdVgpuCount[swizzId] >= 1)
    {
        pPhysGpuInfo->assignedSwizzIdVgpuCount[swizzId]--;
    }
    else
    {
        NV_PRINTF(LEVEL_ERROR, "SwizzID - %d already has no assigned vGPU\n", swizzId);
        return NV_ERR_INVALID_STATE;
    }
    if (pPhysGpuInfo->assignedSwizzIdVgpuCount[swizzId] == 0)
    {
        pPhysGpuInfo->assignedSwizzIdMask &= ~mask;
    }

    return NV_OK;
}

NV_STATUS
kvgpumgrGetSwizzId(OBJGPU *pGpu,
                   KERNEL_PHYS_GPU_INFO *pPhysGpuInfo,
                   NvU32 partitionFlag,
                   VGPU_TYPE *vgpuTypeInfo,
                   NvU32 *swizzId)
{
    OBJSYS *pSys                         = SYS_GET_INSTANCE();
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    KernelVgpuMgr *pKernelVgpuMgr        = SYS_GET_KERNEL_VGPUMGR(pSys);
    NvU64 swizzIdInUseMask = 0;
    NvU32 id;
    NV_STATUS rmStatus = NV_OK;
    VGPU_TYPE *existingVgpuTypeInfo = NULL;
    NvBool bExistingVgpuOnGI = NV_FALSE;
    REQUEST_VGPU_INFO_NODE *pRequestVgpu = NULL;

    swizzIdInUseMask = kmigmgrGetSwizzIdInUseMask(pGpu, pKernelMIGManager);

    *swizzId = KMIGMGR_SWIZZID_INVALID;

    // Determine valid swizzids not assigned to any vGPU device.
    for (id = 0; id < KMIGMGR_MAX_GPU_SWIZZID; id++)
    {
        bExistingVgpuOnGI = NV_FALSE;
        if (NVBIT64(id) & swizzIdInUseMask)
        {
            KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance;

            rmStatus = kmigmgrGetGPUInstanceInfo(pGpu, pKernelMIGManager, id, &pKernelMIGGpuInstance);
            if (rmStatus != NV_OK)
            {
                // Didn't find requested GPU instance
                NV_PRINTF(LEVEL_ERROR, "No valid GPU instance with SwizzId - %d found\n", id);
                return rmStatus;
            }

            if (pKernelMIGGpuInstance->partitionFlag == partitionFlag)
            {
                /*TODO -> Need to fix this for ESXi for MIG + Timeslice Support*/
                if (osIsVgpuVfioPresent() == NV_OK)
                {
                    NvBool bHeterogeneousModeEnabled;
                    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                        kvgpuMgrGetHeterogeneousMode(pGpu,
                                                     id,
                                                     &bHeterogeneousModeEnabled));
                    if (bHeterogeneousModeEnabled)
                    {
                        // search creatable heterogeneous type contains the current type
                        NvU32 *pSupportedTypeId = kvgpuMgrGetVgpuCreatableTypeIdFromSwizzId(pGpu,
                                                                                            pPhysGpuInfo, id);
                        NvBool bFoundCreatableHeterogeneousType = NV_FALSE;
                        for (NvU32 i = 0; i < pPhysGpuInfo->numVgpuTypes; i++)
                        {
                            if (pSupportedTypeId[i] == vgpuTypeInfo->vgpuTypeId)
                            {
                                bFoundCreatableHeterogeneousType = NV_TRUE;
                                break;
                            }
                        }
                        if (!bFoundCreatableHeterogeneousType)
                            continue;
                    }
                    else if (pPhysGpuInfo->assignedSwizzIdVgpuCount[id] > 0)
                    {
                        for (pRequestVgpu = listHead(&pKernelVgpuMgr->listRequestVgpuHead);
                             pRequestVgpu != NULL;
                             pRequestVgpu = listNext(&pKernelVgpuMgr->listRequestVgpuHead, pRequestVgpu))
                        {
                            /* Check for an existing Vgpu on the same physical gpu and same GI  */
                            if ((pRequestVgpu->gpuPciId == pGpu->gpuId) && (pRequestVgpu->swizzId == id))
                            {
                                bExistingVgpuOnGI = NV_TRUE;
                                break;
                            }
                        }
                        if (bExistingVgpuOnGI ==  NV_TRUE)
                        {
                            NV_ASSERT_OK_OR_RETURN(kvgpumgrGetVgpuTypeInfo(pRequestVgpu->vgpuTypeId, &existingVgpuTypeInfo));
                            if (kvgpumgrIsHeterogeneousVgpuTypeSupported() == NV_TRUE)
                            {
                                if (existingVgpuTypeInfo->profileSize != vgpuTypeInfo->profileSize)
                                    continue;
                            }
                            else
                            {
                                if (existingVgpuTypeInfo->vgpuTypeId != vgpuTypeInfo->vgpuTypeId)
                                     continue;
                            }
                        }
                    }
                }

                // Validate that same ID is not already set and then set the ID
                if (pPhysGpuInfo->assignedSwizzIdVgpuCount[id] < vgpuTypeInfo->maxInstancePerGI )
                {
                    NV_ASSERT_OK_OR_RETURN(_kvgpumgrSetAssignedSwizzIdMask(pGpu, vgpuTypeInfo, pKernelMIGGpuInstance->swizzId));
                    *swizzId = pKernelMIGGpuInstance->swizzId;
                    break;
                }
            }
        }
    }

    if (*swizzId == KMIGMGR_SWIZZID_INVALID)
    {
        return NV_ERR_INVALID_STATE;
    }

    return NV_OK;
}

NV_STATUS
kvgpumgrValidateSwizzId(OBJGPU *pGpu,
                        NvU32 vgpuTypeId,
                        NvU32 swizzId)
{
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    NvU32 partitionFlag                 = PARTITIONID_INVALID;
    NV_STATUS rmStatus                  = NV_OK;
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance;
    VGPU_TYPE *vgpuTypeInfo = NULL;

    // Check if swizzId is valid.
    if (!kmigmgrIsSwizzIdInUse(pGpu, pKernelMIGManager, swizzId))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    rmStatus = kmigmgrGetGPUInstanceInfo(pGpu, pKernelMIGManager, swizzId, &pKernelMIGGpuInstance);
    if (rmStatus != NV_OK)
    {
        // Didn't find requested GPU instance
        NV_PRINTF(LEVEL_ERROR, "No valid GPU instance with SwizzId - %d found\n", swizzId);
        return rmStatus;
    }

    NV_ASSERT_OK_OR_RETURN(
        kvgpumgrGetPartitionFlag(vgpuTypeId, &partitionFlag));

    // Check if swizzId is valid for vGPU type.
    if (pKernelMIGGpuInstance->partitionFlag == partitionFlag)
    {
        NV_ASSERT_OK_OR_RETURN(kvgpumgrGetVgpuTypeInfo(vgpuTypeId, &vgpuTypeInfo));
        NV_ASSERT_OK_OR_RETURN(_kvgpumgrSetAssignedSwizzIdMask(pGpu, vgpuTypeInfo, swizzId));
        return NV_OK;
    }

    return NV_ERR_INVALID_STATE;
}

NV_STATUS
kvgpumgrGetPartitionFlag(NvU32 vgpuTypeId, NvU32 *partitionFlag)
{
    NvU32 i;

    *partitionFlag = PARTITIONID_INVALID;

    for (i = 0; i < NV_ARRAY_ELEMENTS(vgpuSmcTypeIdMappings); i++)
    {
        if (vgpuSmcTypeIdMappings[i].vgpuTypeId == vgpuTypeId)
        {
            *partitionFlag = vgpuSmcTypeIdMappings[i].partitionFlag;
            return NV_OK;
        }
    }

    NV_PRINTF(LEVEL_ERROR, "Invalid SMC vGpu TypeId: 0x%x\n", vgpuTypeId);
    return NV_ERR_INVALID_ARGUMENT;
}

/*
 * Add or remove VF info to pgpuInfo of its PF
 * @param[in] gpuPciId          PCI ID of target PF
 * @param[in] cmd
 *      0/VGPU_CMD_PROCESS_VF_INFO.NV_VGPU_SAVE_VF_INFO     = Add VF info to VF list of target PF
 *      1/VGPU_CMD_PROCESS_VF_INFO.NV_VGPU_REMOVE_VF_INFO   = Remove VF info from VF list of target PF
 * @param[in] domain            Domain of VF to be stored
 * @param[in] bus               Bus no. of VF to be stored
 * @param[in] slot              Slot no. of VF to be stored
 * @param[in] function          Function of VF to be stored
 * @param[in] isMdevAttached    Flag to indicate if VF is registered with mdev
 * @param[out]vfPciInfo         Array of PCI information of VFs
 */
NV_STATUS
kvgpumgrProcessVfInfo(NvU32 gpuPciId, NvU8 cmd, NvU32 domain, NvU32 bus, NvU32 slot, NvU32 function, NvBool isMdevAttached, vgpu_vf_pci_info *vfPciInfo)
{
    NvU32 i;
    OBJSYS *pSys = SYS_GET_INSTANCE();
    KernelVgpuMgr *pKernelVgpuMgr = SYS_GET_KERNEL_VGPUMGR(pSys);
    NvU32 pgpuIndex;
    NV_STATUS status = NV_OK;
    KERNEL_PHYS_GPU_INFO *pPhysGpuInfo;
    vgpu_vf_pci_info *pVfInfo = NULL;

    if ((status = kvgpumgrGetPgpuIndex(pKernelVgpuMgr, gpuPciId, &pgpuIndex)) != NV_OK)
        return status;

    pPhysGpuInfo = &(pKernelVgpuMgr->pgpuInfo[pgpuIndex]);

    if (cmd == NV_VGPU_SAVE_VF_INFO)
    {
        /* Find existing entry for VF and set flags if found. */
        for (i = 0; i < MAX_VF_COUNT_PER_GPU; i++)
        {
            pVfInfo = &pPhysGpuInfo->vfPciInfo[i];

            if ((domain   == pVfInfo->domain) &&
                (bus      == pVfInfo->bus)    &&
                (slot     == pVfInfo->slot)   &&
                (function == pVfInfo->function))
            {
                pVfInfo->isNvidiaAttached = NV_TRUE;
                pVfInfo->isMdevAttached   = isMdevAttached;
                break;
            }
        }

        /*
         * If entry doesn't already exist, populate an empty slot and complain
         * if there isn't one.
         */
        if (i == MAX_VF_COUNT_PER_GPU)
        {
            for (i = 0; i < MAX_VF_COUNT_PER_GPU; i++)
            {
                pVfInfo = &pPhysGpuInfo->vfPciInfo[i];
                if ((!pVfInfo->isNvidiaAttached))
                {
                    pVfInfo->domain   = domain;
                    pVfInfo->bus      = bus;
                    pVfInfo->slot     = slot;
                    pVfInfo->function = function;

                    pVfInfo->isNvidiaAttached = NV_TRUE;
                    pVfInfo->isMdevAttached   = isMdevAttached;
                    break;
                }
            }

            if (i == MAX_VF_COUNT_PER_GPU)
            {
                NV_PRINTF(LEVEL_ERROR, "No free free slot to track VF PCI info\n");
                return NV_ERR_INVALID_STATE;
            }
        }
    }
    else if ((cmd == NV_VGPU_REMOVE_VF_PCI_INFO || cmd == NV_VGPU_REMOVE_VF_MDEV_INFO) && pPhysGpuInfo)
    {
        for (i = 0; i < MAX_VF_COUNT_PER_GPU; i++)
        {
            pVfInfo = &pPhysGpuInfo->vfPciInfo[i];

            if ((domain   == pVfInfo->domain) &&
                (bus      == pVfInfo->bus)    &&
                (slot     == pVfInfo->slot)   &&
                (function == pVfInfo->function))
            {
                if (cmd == NV_VGPU_REMOVE_VF_PCI_INFO)
                {
                    pVfInfo->isNvidiaAttached = NV_FALSE;
                }
                pVfInfo->isMdevAttached   = NV_FALSE;
                break;
            }
        }

        if (i == MAX_VF_COUNT_PER_GPU)
        {
            NV_PRINTF(LEVEL_ERROR, "Could not find entry for VF PCI info\n");
            return NV_ERR_MISSING_TABLE_ENTRY;
        }
    }
    else if (cmd == NV_VGPU_GET_VF_INFO && vfPciInfo)
    {
        for (i = 0; i < MAX_VF_COUNT_PER_GPU; i++)
        {
            pVfInfo = &pPhysGpuInfo->vfPciInfo[i];
            portMemCopy(&vfPciInfo[i], sizeof(vgpu_vf_pci_info), pVfInfo, sizeof(vgpu_vf_pci_info));
        }
    }
    else
    {
        NV_PRINTF(LEVEL_ERROR, "Requested invalid operation on VF info\n");
        status = NV_ERR_INVALID_ARGUMENT;
    }

    return status;
}

NV_STATUS
kvgpumgrEnumerateVgpuPerPgpu(OBJGPU *pGpu, NV2080_CTRL_VGPU_MGR_INTERNAL_ENUMERATE_VGPU_PER_PGPU_PARAMS *pParams)
{
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, pRmApi->Control(pRmApi, pGpu->hInternalClient, pGpu->hInternalSubdevice,
                                                       NV2080_CTRL_CMD_VGPU_MGR_INTERNAL_ENUMERATE_VGPU_PER_PGPU,
                                                       pParams, sizeof(*pParams)));

    return NV_OK;
}

NV_STATUS
kvgpumgrClearGuestVmInfo(OBJGPU *pGpu, KERNEL_HOST_VGPU_DEVICE *pKernelHostVgpuDevice)
{
    NV2080_CTRL_VGPU_MGR_INTERNAL_CLEAR_GUEST_VM_INFO_PARAMS params;
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV_STATUS status;

    portMemSet(&params, 0, sizeof(params));
    params.gfid = pKernelHostVgpuDevice->gfid;

    status = pRmApi->Control(pRmApi, pGpu->hInternalClient, pGpu->hInternalSubdevice,
                             NV2080_CTRL_CMD_VGPU_MGR_INTERNAL_CLEAR_GUEST_VM_INFO,
                             &params, sizeof(params));

    if (status != NV_OK)
        NV_PRINTF(LEVEL_ERROR, "Failed to clear guest vm info on GSP\n");

    return status;
}

NV_STATUS
kvgpumgrGetHostVgpuDeviceFromGfid(NvU32 gpuPciId, NvU32 gfid,
                                  KERNEL_HOST_VGPU_DEVICE** ppHostVgpuDevice)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    KernelVgpuMgr *pKernelVgpuMgr = SYS_GET_KERNEL_VGPUMGR(pSys);
    NvU32 pgpuIndex;
    KERNEL_PHYS_GPU_INFO *pPgpuInfo;
    KERNEL_HOST_VGPU_DEVICE *pKernelHostVgpuDevice;

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, kvgpumgrGetPgpuIndex(pKernelVgpuMgr, gpuPciId, &pgpuIndex));
    pPgpuInfo = &pKernelVgpuMgr->pgpuInfo[pgpuIndex];

    if (!pPgpuInfo->sriovEnabled)
        return NV_ERR_NOT_SUPPORTED;

    for (pKernelHostVgpuDevice = listHead(&(pPgpuInfo->listHostVgpuDeviceHead));
         pKernelHostVgpuDevice != NULL;
         pKernelHostVgpuDevice = listNext(&(pPgpuInfo->listHostVgpuDeviceHead), pKernelHostVgpuDevice))
    {
        if (pKernelHostVgpuDevice->gfid == gfid)
        {
            *ppHostVgpuDevice = pKernelHostVgpuDevice;
            return NV_OK;
        }
     }

    return NV_ERR_OBJECT_NOT_FOUND;
}

NV_STATUS
kvgpumgrGetVgpuFbUsage(OBJGPU *pGpu, NVA081_CTRL_VGPU_CONFIG_GET_VGPU_FB_USAGE_PARAMS *pParams)
{
    KERNEL_HOST_VGPU_DEVICE *pKernelHostVgpuDevice;
    NVA081_VGPU_FB_USAGE *pVgpuFbUsage;
    NvU32 i = 0;

    NV_PRINTF(LEVEL_INFO, "%s\n", __FUNCTION__);

    if (gpuIsSriovEnabled(pGpu))
    {
        NV2080_CTRL_VGPU_MGR_INTERNAL_GET_VGPU_FB_USAGE_PARAMS internalParams;
        RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

        portMemSet(&internalParams, 0, sizeof(internalParams));

        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, pRmApi->Control(pRmApi, pGpu->hInternalClient, pGpu->hInternalSubdevice,
                                                           NV2080_CTRL_CMD_VGPU_MGR_INTERNAL_GET_VGPU_FB_USAGE,
                                                           &internalParams, sizeof(internalParams)));

        if (internalParams.vgpuCount > NV_ARRAY_ELEMENTS(pParams->vgpuFbUsage))
            return NV_ERR_INSUFFICIENT_RESOURCES;

        for (i = 0; i < internalParams.vgpuCount; i++)
        {
            NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                 kvgpumgrGetHostVgpuDeviceFromGfid(pGpu->gpuId, internalParams.vgpuFbUsage[i].gfid, &pKernelHostVgpuDevice));

            pVgpuFbUsage = &pParams->vgpuFbUsage[i];

            portMemCopy(pVgpuFbUsage->vgpuUuid, VGPU_UUID_SIZE, pKernelHostVgpuDevice->vgpuUuid, VGPU_UUID_SIZE);

            pVgpuFbUsage->fbUsed = internalParams.vgpuFbUsage[i].fbUsed;
        }

        pParams->vgpuCount = internalParams.vgpuCount;
    }

    return NV_OK;
}

NV_STATUS
kvgpumgrSetVgpuEncoderCapacity(OBJGPU *pGpu, NvU8 *vgpuUuid, NvU32 encoderCapacity)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    KernelVgpuMgr *pKernelVgpuMgr = SYS_GET_KERNEL_VGPUMGR(pSys);
    KERNEL_HOST_VGPU_DEVICE *pKernelHostVgpuDevice;
    KERNEL_PHYS_GPU_INFO *pPhysGpuInfo;
    NvU32 i;

    NV_PRINTF(LEVEL_INFO, "%s\n", __FUNCTION__);

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, kvgpumgrGetPgpuIndex(pKernelVgpuMgr, pGpu->gpuId, &i));

    pPhysGpuInfo = &pKernelVgpuMgr->pgpuInfo[i];

    for (pKernelHostVgpuDevice = listHead(&(pPhysGpuInfo->listHostVgpuDeviceHead));
         pKernelHostVgpuDevice != NULL;
         pKernelHostVgpuDevice = listNext(&(pPhysGpuInfo->listHostVgpuDeviceHead), pKernelHostVgpuDevice))
    {
        if (portMemCmp(pKernelHostVgpuDevice->vgpuUuid, vgpuUuid, VGPU_UUID_SIZE) == 0)
            break;
    }

    if (pKernelHostVgpuDevice == NULL)
        return NV_ERR_OBJECT_NOT_FOUND;

    if (gpuIsSriovEnabled(pGpu))
    {
        NV2080_CTRL_VGPU_MGR_INTERNAL_SET_VGPU_ENCODER_CAPACITY_PARAMS params;
        RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

        params.gfid = pKernelHostVgpuDevice->gfid;
        params.encoderCapacity = encoderCapacity;

        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                              pRmApi->Control(pRmApi, pGpu->hInternalClient, pGpu->hInternalSubdevice,
                                              NV2080_CTRL_CMD_VGPU_MGR_INTERNAL_SET_VGPU_ENCODER_CAPACITY,
                                              &params, sizeof(params)));
    }

    return NV_OK;
}

//
// Add vGPU info received on mdev_create sysfs call to REQUEST_VGPU_INFO_NODE
// list. REQUEST_VGPU_INFO_NODE is currently used only for vGPU on KVM.
//
// This funtion first checks whether the vGPU type is supported or not as
// only homegeneous vGPU types are supported currently. Also, this function
// only creates REQUEST_VGPU_INFO_NODE entry, actual vGPU will be created later
//
NV_STATUS
kvgpumgrCreateRequestVgpu(NvU32 gpuPciId, const NvU8 *pVgpuDevName,
                          NvU32 vgpuTypeId, NvU16 *vgpuId, NvU32 *gpuInstanceId,
                          NvU32 *placementId, NvU32 gpuPciBdf)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGPU *pGpu = gpumgrGetGpuFromId(gpuPciId);
    KernelVgpuMgr *pKernelVgpuMgr = SYS_GET_KERNEL_VGPUMGR(pSys);
    OBJHYPERVISOR *pHypervisor = SYS_GET_HYPERVISOR(pSys);
    NvU32 pgpuIndex;
    NV_STATUS status = NV_OK;
    VGPU_TYPE *vgpuTypeInfo;
    KERNEL_PHYS_GPU_INFO *pPhysGpuInfo;
    REQUEST_VGPU_INFO_NODE *pRequestVgpu = NULL;
    NvU32 allocFlags = NVOS32_ALLOC_FLAGS_FORCE_MEM_GROWS_UP;
    NvU64 vgpuIdSize = 1, tmpVgpuId;
    NvU8 devfn = gpuDecodeDevice(gpuPciBdf);
    NvU32 swizzId = KMIGMGR_SWIZZID_INVALID;
    NvBool bHeterogeneousModeEnabled;

    if (pGpu == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "GPU %u is not initialized yet \n", gpuPciBdf);
        return NV_ERR_TIMEOUT_RETRY;
    }

    if ((status = kvgpumgrGetPgpuIndex(pKernelVgpuMgr, gpuPciId, &pgpuIndex)) != NV_OK)
        return status;

    pPhysGpuInfo = &(pKernelVgpuMgr->pgpuInfo[pgpuIndex]);

    if ((status = kvgpumgrGetVgpuTypeInfo(vgpuTypeId, &vgpuTypeInfo)) != NV_OK)
        return status;

    /*
     * For DriverVM, we will defer the createdVfMask validation later
     * during open call when the params provided.
     *
     */

    if (gpuIsSriovEnabled(pGpu) &&
        !(pHypervisor->getProperty(pHypervisor, PDB_PROP_HYPERVISOR_DRIVERVM_ENABLED)))
    {
        NvU8 fnId = devfn - pGpu->sriovState.firstVFOffset;

        NV_ASSERT_OR_RETURN((fnId < 64), NV_ERR_INVALID_ARGUMENT);

        if (pPhysGpuInfo->createdVfMask & NVBIT64(fnId))
            /* mdev device is already created on VF */
            return NV_ERR_INVALID_OPERATION;
    }

    if (IS_MIG_ENABLED(pGpu))
    {
        NvU32 partitionFlag = PARTITIONID_INVALID;

        NV_CHECK_OR_RETURN(LEVEL_INFO,
            IS_MIG_IN_USE(pGpu),
            NV_ERR_INVALID_OPERATION);

        NV_ASSERT_OK_OR_RETURN(
            kvgpumgrGetPartitionFlag(vgpuTypeInfo->vgpuTypeId, &partitionFlag));

        NV_ASSERT_OK_OR_RETURN(
            kvgpumgrGetSwizzId(pGpu, pPhysGpuInfo, partitionFlag, vgpuTypeInfo, &swizzId));
    }
    else
    {
        // Creation request for a MIG vgpuType
        if (vgpuTypeInfo->gpuInstanceSize)
            return NV_ERR_INVALID_OPERATION;

        status = kvgpumgrCheckVgpuTypeCreatable(pGpu, pPhysGpuInfo, vgpuTypeInfo);

        if (status != NV_OK)
            return status;
    }

    /* Initialize heap on first vGPU device creation */
    if (pKernelVgpuMgr->pHeap == NULL)
    {
        NvU64 tmpSize;
        pKernelVgpuMgr->pHeap = portMemAllocNonPaged(sizeof(OBJEHEAP));
        if (pKernelVgpuMgr->pHeap == NULL)
        {
            status = NV_ERR_NO_MEMORY;
            goto failed;
        }

        constructObjEHeap(pKernelVgpuMgr->pHeap, 1, 0xFFFF + 1, 0, 0);

        /* Verify if pHeap is allocated with required size */
        pKernelVgpuMgr->pHeap->eheapGetSize(pKernelVgpuMgr->pHeap, &tmpSize);
        if (tmpSize != 0xFFFF)
        {
            pKernelVgpuMgr->pHeap->eheapDestruct(pKernelVgpuMgr->pHeap);
            portMemFree(pKernelVgpuMgr->pHeap);
            pKernelVgpuMgr->pHeap = NULL;
            status = NV_ERR_INSUFFICIENT_RESOURCES;
            goto failed;
        }
    }

    status = pKernelVgpuMgr->pHeap->eheapAlloc(pKernelVgpuMgr->pHeap, 0xdeadbeef, &allocFlags,
                                         &tmpVgpuId, &vgpuIdSize, 1, 1,
                                         NULL, NULL, NULL);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_WARNING,
                  "Failed to allocate heap for vGPU ID 0x%x\n",
                  status);
        goto failed;
    }

    *vgpuId = (NvU16) tmpVgpuId;
    pRequestVgpu = listPrependNew(&(pKernelVgpuMgr->listRequestVgpuHead));
    if (pRequestVgpu == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        goto failed;
    }

    portMemSet(pRequestVgpu, 0, sizeof(REQUEST_VGPU_INFO_NODE));

    portMemCopy(pRequestVgpu->vgpuDevName, VGPU_UUID_SIZE, pVgpuDevName, VGPU_UUID_SIZE);
    pRequestVgpu->gpuPciId = gpuPciId; /* For SRIOV, this is PF's gpuPciId */
    pRequestVgpu->gpuPciBdf = gpuPciBdf; /* For SRIOV, this is VF's gpuPciBdf */
    pRequestVgpu->vgpuId = *vgpuId;
    pRequestVgpu->placementId = NVA081_PLACEMENT_ID_INVALID;
    pRequestVgpu->vgpuTypeId = vgpuTypeId;

    if (IS_MIG_IN_USE(pGpu))
    {
        pRequestVgpu->swizzId = swizzId;
    }
    else
    {
        pRequestVgpu->swizzId = KMIGMGR_SWIZZID_INVALID;
    }
    if (!IS_MIG_ENABLED(pGpu) ||
        (IS_MIG_ENABLED(pGpu) && kvgpumgrIsMigTimeslicingModeEnabled(pGpu)))
    {
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                            kvgpuMgrGetHeterogeneousMode(pGpu, swizzId, &bHeterogeneousModeEnabled));

        if (bHeterogeneousModeEnabled ||
            kvgpumgrCheckHomogeneousPlacementSupported(pGpu, pRequestVgpu->swizzId) == NV_OK)
        {
            NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
                kvgpumgrSetPlacementId(pRequestVgpu, pRequestVgpu->placementId), failed);
        }
    }

    kvgpumgrSetVgpuType(pGpu, pPhysGpuInfo, vgpuTypeId, pRequestVgpu->swizzId);
    pPhysGpuInfo->numCreatedVgpu++;

    if (gpuGetDevice(pGpu) != devfn)  /* SRIOV - VF */
        pPhysGpuInfo->createdVfMask |= NVBIT64(devfn - pGpu->sriovState.firstVFOffset);

    *placementId   = pRequestVgpu->placementId;
    *gpuInstanceId = pRequestVgpu->swizzId;

    return NV_OK;

failed:
    if (swizzId != KMIGMGR_SWIZZID_INVALID)
        _kvgpumgrClearAssignedSwizzIdMask(pGpu, swizzId);

    return status;
}

//
// Delete REQUEST_VGPU_INFO_NODE structure from list.
// REQUEST_VGPU_INFO_NODE is currently used only for vGPU on KVM.
//
NV_STATUS
kvgpumgrDeleteRequestVgpu(const NvU8 *pVgpuDevName, NvU16 vgpuId)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGPU *pGpu = NULL;
    NV_STATUS status;
    NvU32 pgpuIndex;
    KernelVgpuMgr *pKernelVgpuMgr = SYS_GET_KERNEL_VGPUMGR(pSys);
    REQUEST_VGPU_INFO_NODE *pRequestVgpu = NULL;
    REQUEST_VGPU_INFO_NODE *pRequestVgpuNext = NULL;
    NvU8 devfn = 0;

    for (pRequestVgpu = listHead(&(pKernelVgpuMgr->listRequestVgpuHead));
         pRequestVgpu != NULL;
         pRequestVgpu = pRequestVgpuNext)
    {
        pRequestVgpuNext = listNext(&(pKernelVgpuMgr->listRequestVgpuHead), pRequestVgpu);
        if (portMemCmp(pVgpuDevName, pRequestVgpu->vgpuDevName, VGPU_UUID_SIZE) == 0)
        {
            if ((status = kvgpumgrGetPgpuIndex(pKernelVgpuMgr, pRequestVgpu->gpuPciId,
                                              &pgpuIndex)) != NV_OK)
                return status;

            pGpu  = gpumgrGetGpuFromId(pRequestVgpu->gpuPciId);
            devfn = gpuDecodeDevice(pRequestVgpu->gpuPciBdf);

            pKernelVgpuMgr->pgpuInfo[pgpuIndex].numCreatedVgpu--;

            if (pGpu && (gpuGetDevice(pGpu) != devfn)) // SRIOV - VF
                pKernelVgpuMgr->pgpuInfo[pgpuIndex].createdVfMask &= ~(NVBIT64(devfn - pGpu->sriovState.firstVFOffset));

            if (IS_MIG_ENABLED(pGpu))
                _kvgpumgrClearAssignedSwizzIdMask(pGpu, pRequestVgpu->swizzId);
            else if (pKernelVgpuMgr->pgpuInfo[pgpuIndex].numCreatedVgpu == 0)
                kvgpumgrSetVgpuType(pGpu, &pKernelVgpuMgr->pgpuInfo[pgpuIndex], NVA081_CTRL_VGPU_CONFIG_INVALID_TYPE,
                                    pRequestVgpu->swizzId);
            if (!IS_MIG_ENABLED(pGpu) ||
                (IS_MIG_ENABLED(pGpu) && kvgpumgrIsMigTimeslicingModeEnabled(pGpu)))
            {
                NvBool bHeterogeneousModeEnabled;

                NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                    kvgpuMgrGetHeterogeneousMode(pGpu, pRequestVgpu->swizzId, &bHeterogeneousModeEnabled));
                if (bHeterogeneousModeEnabled)
                {
                    status = _kvgpumgrUpdateHeterogeneousCreatablePlacementIds(pGpu, pRequestVgpu->placementId,
                                                                            pRequestVgpu->vgpuTypeId, NV_FALSE,
                                                                            pRequestVgpu->swizzId);
                }
                else if (kvgpumgrCheckHomogeneousPlacementSupported(pGpu, pRequestVgpu->swizzId) == NV_OK)
                {
                    status = _kvgpumgrUpdateHomogeneousCreatablePlacementIds(pGpu, pRequestVgpu->placementId,
                                                                            pRequestVgpu->vgpuTypeId, NV_FALSE,
                                                                            pRequestVgpu->swizzId);
                }

                if (status != NV_OK)
                    NV_PRINTF(LEVEL_ERROR, "Failed to free up allocated placementId, status: 0x%x\n", status);
            }

            pKernelVgpuMgr->pHeap->eheapFree(pKernelVgpuMgr->pHeap, vgpuId);

            listRemove(&(pKernelVgpuMgr->listRequestVgpuHead), pRequestVgpu);

            return NV_OK;
        }
    }
    return NV_ERR_OBJECT_NOT_FOUND;
}

NV_STATUS kvgpumgrGetAvailableInstances(
    NvU32     *availInstances,
    OBJGPU    *pGpu,
    VGPU_TYPE *vgpuTypeInfo,
    NvU32      pgpuIndex,
    NvU8       devfn
)
{
    NV_STATUS rmStatus = NV_OK;
    OBJSYS *pSys = SYS_GET_INSTANCE();
    KernelVgpuMgr *pKernelVgpuMgr = SYS_GET_KERNEL_VGPUMGR(pSys);
    OBJHYPERVISOR *pHypervisor = SYS_GET_HYPERVISOR(pSys);

    *availInstances = 0;

    /* TODO: Needs to have a proper fix this for DriverVM config */
    if (gpuIsSriovEnabled(pGpu) &&
        !pHypervisor->getProperty(pHypervisor, PDB_PROP_HYPERVISOR_DRIVERVM_ENABLED))
    {
        NvU8 fnId = devfn - pGpu->sriovState.firstVFOffset;

        NV_ASSERT_OR_RETURN(fnId <= pGpu->sriovState.totalVFs, NV_ERR_INVALID_ARGUMENT);

        
        if ((pKernelVgpuMgr->pgpuInfo[pgpuIndex].createdVfMask) & (NVBIT64(fnId)))
            goto exit;    

        if (IS_MIG_ENABLED(pGpu))
        {
            if (IS_MIG_IN_USE(pGpu))
            {
                NvU64 swizzIdInUseMask = 0;
                NvU32 partitionFlag = PARTITIONID_INVALID;
                KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
                NvU32 id;
                VGPU_TYPE *existingVgpuTypeInfo = NULL;
                NvBool bExistingVgpuOnGI = NV_FALSE;
                REQUEST_VGPU_INFO_NODE *pRequestVgpu = NULL;

                swizzIdInUseMask = kmigmgrGetSwizzIdInUseMask(pGpu, pKernelMIGManager);

                if (!vgpuTypeInfo->gpuInstanceSize)
                {
                    // Query for a non MIG vgpuType
                    NV_PRINTF(LEVEL_INFO, "Query for a non MIG vGPU type \n");
                    rmStatus = NV_OK;
                    goto exit;
                }

                rmStatus = kvgpumgrGetPartitionFlag(vgpuTypeInfo->vgpuTypeId,
                                                    &partitionFlag);
                if (rmStatus != NV_OK)
                {
                    // Query for a non MIG vgpuType
                    NV_PRINTF(LEVEL_ERROR, "Failed to get partition flags.\n");
                    goto exit;
                }
    
                // Determine valid swizzids not assigned to any vGPU device.
                FOR_EACH_INDEX_IN_MASK(64, id, swizzIdInUseMask)
                {
                    bExistingVgpuOnGI = NV_FALSE;
                    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance;

                    rmStatus = kmigmgrGetGPUInstanceInfo(pGpu, pKernelMIGManager,
                                                         id, &pKernelMIGGpuInstance);
                    if (rmStatus != NV_OK)
                    {
                        // Didn't find requested GPU instance
                        NV_PRINTF(LEVEL_ERROR,
                                  "No valid GPU instance with SwizzId - %d found\n", id);
                        goto exit;
                    }

                    if (pKernelMIGGpuInstance->partitionFlag == partitionFlag)
                    {
                        if (pKernelVgpuMgr->pgpuInfo[pgpuIndex].assignedSwizzIdVgpuCount[id] > 0) 
                        {
                            for (pRequestVgpu = listHead(&pKernelVgpuMgr->listRequestVgpuHead);
                                 pRequestVgpu != NULL;
                                 pRequestVgpu = listNext(&pKernelVgpuMgr->listRequestVgpuHead, pRequestVgpu))
                            {
                                /* Check for an existing Vgpu on the same physical gpu and same GI  */
                                if ((pRequestVgpu->gpuPciId == pGpu->gpuId) && (pRequestVgpu->swizzId == id))
                                {
                                    bExistingVgpuOnGI = NV_TRUE;
                                    break;
                                }
                            }
                            if (bExistingVgpuOnGI ==  NV_TRUE)
                            {
                                NV_ASSERT_OK_OR_RETURN(kvgpumgrGetVgpuTypeInfo(pRequestVgpu->vgpuTypeId, &existingVgpuTypeInfo));
                                if (kvgpumgrIsHeterogeneousVgpuTypeSupported() == NV_TRUE)
                                {
                                    if (existingVgpuTypeInfo->profileSize != vgpuTypeInfo->profileSize)
                                        continue;
                                }
                                else
                                {
                                    if (existingVgpuTypeInfo->vgpuTypeId != vgpuTypeInfo->vgpuTypeId)
                                         continue;
                                }
                            }
                        }
                       // Validate that same ID is not already set and VF is availablei
                       if (pKernelVgpuMgr->pgpuInfo[pgpuIndex].assignedSwizzIdVgpuCount[id] < vgpuTypeInfo->maxInstancePerGI)
                        {
                            *availInstances = 1;
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
                    NV_PRINTF(LEVEL_INFO, "Query for a MIG vGPU type\n");
                    rmStatus = NV_OK;
                    goto exit;
                }

                if (kvgpumgrCheckVgpuTypeCreatable(pGpu, &pKernelVgpuMgr->pgpuInfo[pgpuIndex],
                                                    vgpuTypeInfo) == NV_OK)
                    *availInstances = 1;
            }
        }
    }
    else
    {
        if (kvgpumgrCheckVgpuTypeCreatable(pGpu, &pKernelVgpuMgr->pgpuInfo[pgpuIndex], vgpuTypeInfo) == NV_OK)
            *availInstances = vgpuTypeInfo->maxInstance - pKernelVgpuMgr->pgpuInfo[pgpuIndex].numCreatedVgpu;
    }

exit:
    return rmStatus;
}

NV_STATUS
kvgpumgrGetHostVgpuDeviceFromVgpuDevName(NvU32 gpuPciId, const NvU8 *pVgpuDevName,
                                      KERNEL_HOST_VGPU_DEVICE **ppKernelHostVgpuDevice)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    KernelVgpuMgr *pKernelVgpuMgr = SYS_GET_KERNEL_VGPUMGR(pSys);
    NvU32 pgpuIndex , rmStatus;
    KERNEL_PHYS_GPU_INFO *pPgpuInfo;
    KERNEL_HOST_VGPU_DEVICE *pKernelHostVgpuDevice;

    if (osIsVgpuVfioPresent() != NV_OK)
       return NV_ERR_NOT_SUPPORTED;

    if ((rmStatus = kvgpumgrGetPgpuIndex(pKernelVgpuMgr, gpuPciId, &pgpuIndex)) != NV_OK)
        return rmStatus;

    pPgpuInfo = &pKernelVgpuMgr->pgpuInfo[pgpuIndex];

    for (pKernelHostVgpuDevice = listHead(&(pPgpuInfo->listHostVgpuDeviceHead));
         pKernelHostVgpuDevice != NULL;
         pKernelHostVgpuDevice = listNext(&(pPgpuInfo->listHostVgpuDeviceHead), pKernelHostVgpuDevice))
    {
        if (pKernelHostVgpuDevice == NULL || pKernelHostVgpuDevice->pRequestVgpuInfoNode == NULL)
            return NV_ERR_INVALID_POINTER;

        if (portMemCmp(pKernelHostVgpuDevice->pRequestVgpuInfoNode->vgpuDevName,
                       pVgpuDevName, VM_UUID_SIZE) == 0)
        {
            *ppKernelHostVgpuDevice = pKernelHostVgpuDevice;
            return NV_OK;
        }
     }
    NV_PRINTF(LEVEL_ERROR, "Object not found\n");
    return NV_ERR_OBJECT_NOT_FOUND;
}

NV_STATUS
kvgpumgrGetHostVgpuDeviceFromVgpuUuid(NvU32 gpuPciId, NvU8 *vgpuUuid,
                                  KERNEL_HOST_VGPU_DEVICE **ppKernelHostVgpuDevice)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    KernelVgpuMgr *pKernelVgpuMgr = SYS_GET_KERNEL_VGPUMGR(pSys);
    NvU32 pgpuIndex;
    KERNEL_PHYS_GPU_INFO *pPgpuInfo;
    KERNEL_HOST_VGPU_DEVICE *pKernelHostVgpuDevice;

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, kvgpumgrGetPgpuIndex(pKernelVgpuMgr, gpuPciId, &pgpuIndex));

    pPgpuInfo = &pKernelVgpuMgr->pgpuInfo[pgpuIndex];

    for (pKernelHostVgpuDevice = listHead(&(pPgpuInfo->listHostVgpuDeviceHead));
         pKernelHostVgpuDevice != NULL;
         pKernelHostVgpuDevice = listNext(&(pPgpuInfo->listHostVgpuDeviceHead), pKernelHostVgpuDevice))
    {
        if (portMemCmp(pKernelHostVgpuDevice->vgpuUuid,
                             vgpuUuid, VM_UUID_SIZE) == 0)
        {
            *ppKernelHostVgpuDevice = pKernelHostVgpuDevice;
            return NV_OK;
        }
    }

    return NV_ERR_OBJECT_NOT_FOUND;
}

/*
 * On 1GB profiles on 48GB board, we do not deduct any reserved fb from vGPU FB length
 * as maxInstance is only 32, so guest complete 1GB of fb mem. In such cases with
 * heterogeneous vGPUs, for such 1G profiles on 48G board, some combinations with
 * other vGPU types do not work.
 * Due to this, A40-8Q at placement Id 0 cannot work with A40-1Q at placement Id 8 since
 * A40-1Q is occupying larger vGPU FB length and overlaps with A40-8Q's assigned FB.
 * Similar scenario happens for A40-8Q at placement Id 24 and A40-1Q at placement Id 32
 * So, we disable 1GB profiles in such cases at placement Id 8 and placement Id 32.
 */
static NvBool
_kvgpumgrIsPlacementValid(OBJGPU *pGpu, KERNEL_PHYS_GPU_INFO *pPgpuInfo, NvU32 vgpuTypeIndex,
                          NvU32 placementId)
{
    NvU64 vgpuFbLength;
    NvU32 denyListAdaHopper[] = {2, 3, 8, 9, 14, 15, 20, 21, 26, 27, 32, 33, 38, 39, 44, 45};
    NvU32 denyListAmpere[] = {8, 32};
    NvU32 i, length, *invalidPlacements;
    KERNEL_VGPU_TYPE_PLACEMENT_INFO *pKernelVgpuTypePlacementInfo;
    VGPU_TYPE_PLACEMENT_INFO *pVgpuTypePlacementInfo;

    if (IS_GSP_CLIENT(pGpu))
    {
        invalidPlacements = denyListAdaHopper;
        length = NV_ARRAY_ELEMENTS(denyListAdaHopper);
    }
    else
    {
        invalidPlacements = denyListAmpere;
        length = NV_ARRAY_ELEMENTS(denyListAmpere);
    }

    pKernelVgpuTypePlacementInfo = &pPgpuInfo->kernelVgpuTypePlacementInfo;
    pVgpuTypePlacementInfo = &pKernelVgpuTypePlacementInfo->vgpuTypePlacementInfo[vgpuTypeIndex];

    vgpuFbLength = pVgpuTypePlacementInfo->guestVmmuCount * gpuGetVmmuSegmentSize(pGpu);

    if (vgpuFbLength == VGPU_LOWER_FB_PROFILE_SIZE_1_GB)
    {
        for (i = 0; i < length; i++)
        {
           if (invalidPlacements[i] == placementId)
               return NV_FALSE;
        }
    }

    return NV_TRUE;
}

/*
 * This function recursively divides the placement region in 1/2, 1/4, 1/8, 1/16 partitions
 * and then calculates the placement IDs of each vGPU type. It will try to allocate
 * placement ID of 2 vGPU instances for a type in such a way that the placement ID
 * of first vGPU instance starts at the beginning of partition and the placement ID
 * of second vGPU instance ends at the end of partition.
 *
 * It finds the vGPU types whose 2 instances can be allocated in a 1/2, 1/4, 1/8, 1/16
 * partition and then allocates one at start of partition and other at end of partion.
 *
 * It does this recursively by taking input start and end as input alongwith the current
 * partition which specifies whether this is 1/1, 1/2, 1/4, 1/8, 1/16 partion.
 *
 * Similarly it also calculates the channel offsets for each supported placementId
 * for a vGPU type. The algorithm for calculating placementId and channel offset
 * is the same just that for channel offset the range is from 0 to 2048.
 *
 * For vGPU-GSP, we also need to define offsets in FB for GSP plugin heap.
 * For larger vGPU profiles with GSP plugin heap >=1 vMMU segment, the gsp heap is
 * placed at the end of vGPU FB.
 * For smaller profiles with gsp heap < 1 vMMU segment, heap is placed at end of
 * each 1/4th alignment (for 128MB vMMU segment size) OR 1/8th alighment (for 64MB
 * vMMU segment size). For such profiles the vMMU segment at the end of 1/4 or 1/8
 * partition is divided as per profile's heap size.
 *
 */
static void
_kvgpumgrSetHeterogeneousResources(OBJGPU *pGpu, KERNEL_PHYS_GPU_INFO *pPgpuInfo, NvU32 placementIdMin,
                                   NvU32 placementIdMax, NvU32 chidMin, NvU32 chidMax,
                                   NvU32 vmmuSegMin, NvU32 vmmuSegMax, NvU64 gspHeapOffsetMin,
                                   NvU64 gspHeapOffsetMax, NvU32 partitionCount,
                                   NvBool isLeftPartition)
{
    VGPU_TYPE *pVgpuTypeInfo;
    NvU32 heterogeneousMaxInstance, i;
    NvBool isCarveOutGspHeap = NV_FALSE;
    NvU64 newVmmuSegMin, newVmmuSegMax, newGspHeapOffsetMin, newGspHeapOffsetMax, vmmuSegSize;
    NvBool isDefineLeftPlacement, isDefineRightPlacement;
    VGPU_TYPE_SUPPORTED_PLACEMENT_INFO *pVgpuTypeSupportedPlacementInfo;
    VGPU_INSTANCE_SUPPORTED_PLACEMENT_INFO *pVgpuInstanceSupportedPlacementInfo;
    KERNEL_VGPU_TYPE_PLACEMENT_INFO *pKernelVgpuTypePlacementInfo;
    VGPU_TYPE_PLACEMENT_INFO *pVgpuTypePlacementInfo;
    VGPU_INSTANCE_PLACEMENT_INFO *pVgpuInstancePlacementInfo;

    /*
     * As max vGPU per GPU is 32, the minimum partition for 2 vGPUs will be 16.
     * So, if this is a 1/16 partition, don't recurse further
     */
    if (partitionCount > MAX_VGPU_DEVICES_PER_PGPU / 2)
        return;

    pKernelVgpuTypePlacementInfo = &pPgpuInfo->kernelVgpuTypePlacementInfo;

    vmmuSegSize = gpuGetVmmuSegmentSize(pGpu);
    for (i = 0; i < pPgpuInfo->numVgpuTypes; i++)
    {
        pVgpuTypeInfo = pPgpuInfo->vgpuTypes[i];

        pVgpuTypeSupportedPlacementInfo = &pVgpuTypeInfo->vgpuTypeSupportedPlacementInfo;
        pVgpuTypePlacementInfo = &pKernelVgpuTypePlacementInfo->vgpuTypePlacementInfo[i];

        if (pVgpuTypeSupportedPlacementInfo->placementSize == 0)
            continue;

        isDefineLeftPlacement = NV_FALSE;
        isDefineRightPlacement = NV_FALSE;

        heterogeneousMaxInstance = nvPrevPow2_U32(pVgpuTypeInfo->maxInstance);

        /*
         * If homogeneous maxInstances of vGPU type are in power-of-2 and can fit in this partition,
         * then define both the placements in this recursive call
         */
        if (partitionCount * 2 == pVgpuTypeInfo->maxInstance)
        {
            isDefineLeftPlacement = NV_TRUE;
            isDefineRightPlacement = NV_TRUE;

            /*
             * Check if the next recursive partition will be a smaller partition for which
             * we need to carve out GSP heap at the end of smaller partition.
             */
            if (IS_GSP_CLIENT(pGpu) && (isCarveOutGspHeap == NV_FALSE) &&
                (pVgpuTypeInfo->gspHeapSize == vmmuSegSize))
                isCarveOutGspHeap = NV_TRUE;
        }
        else if (((heterogeneousMaxInstance < pVgpuTypeInfo->maxInstance) &&
                  (heterogeneousMaxInstance == partitionCount)) ||
                 ((partitionCount == 1) && (pVgpuTypeInfo->maxInstance == 1)))
        {
            /*
             * If only one instance of vGPU type can fit in this partition, then define
             * placement depending on whether it is a left or right partition
             */
            isDefineLeftPlacement = isLeftPartition;
            isDefineRightPlacement = !isLeftPartition;
        }

        if (isDefineLeftPlacement == NV_TRUE)
        {
            /*
             * Fill placement ID / channel / FB offset of first vGPU in this partition
             * First vGPU is aligned to the starting of partition
             */
            if (_kvgpumgrIsPlacementValid(pGpu, pPgpuInfo, i, placementIdMin))
            {

                pVgpuInstanceSupportedPlacementInfo =
                    &pVgpuTypeSupportedPlacementInfo->vgpuInstanceSupportedPlacementInfo[pVgpuTypeSupportedPlacementInfo->heterogeneousPlacementCount];
                pVgpuInstancePlacementInfo =
                    &pVgpuTypePlacementInfo->vgpuInstancePlacementInfo[pVgpuTypeSupportedPlacementInfo->heterogeneousPlacementCount];

                pVgpuInstanceSupportedPlacementInfo->heterogeneousSupportedPlacementId = placementIdMin;
                pVgpuInstanceSupportedPlacementInfo->heterogeneousSupportedChidOffset = chidMin;

                if (!IS_GSP_CLIENT(pGpu))
                    pVgpuInstancePlacementInfo->heterogeneousSupportedVmmuOffset = vmmuSegMin;
                else
                {
                    /* If profile occupies > 1 vMMU segment for GSP heap, place heap at end of vGPU FB. */
                    if (pVgpuTypeInfo->gspHeapSize >= vmmuSegSize)
                    {
                        pVgpuInstancePlacementInfo->heterogeneousSupportedVmmuOffset = vmmuSegMin;
                        pVgpuInstancePlacementInfo->heterogeneousGspHeapOffset =
                            (vmmuSegMin + pVgpuTypePlacementInfo->guestVmmuCount) * vmmuSegSize;
                    }
                    else
                    {
                        /*
                         * If we're in smaller parition, GSP vMMU segment will be input to the function.
                         * Place the gsp heap of first vGPU at the starting of the GSP vMMU segment.
                         */
                        pVgpuInstancePlacementInfo->heterogeneousSupportedVmmuOffset = vmmuSegMin;
                        pVgpuInstancePlacementInfo->heterogeneousGspHeapOffset = gspHeapOffsetMin;
                    }

                    /* For 32:1 vGPU types, the GSP heap is placed between 2 consecutive guest vMMU segments */
                    if (heterogeneousMaxInstance == 32)
                    {
                        pVgpuInstancePlacementInfo->heterogeneousSupportedVmmuOffset = vmmuSegMin;
                        pVgpuInstancePlacementInfo->heterogeneousGspHeapOffset =
                            (vmmuSegMin + pVgpuTypePlacementInfo->guestVmmuCount) * vmmuSegSize;
                    }
                }

                pVgpuTypeSupportedPlacementInfo->heterogeneousPlacementCount++;
            }
        }

        if (isDefineRightPlacement == NV_TRUE)
        {
            /*
             * Fill placement ID / channel / FB offset of second vGPU in this partition
             * Second vGPU is aligned to the end of partition
             */
            if (_kvgpumgrIsPlacementValid(pGpu, pPgpuInfo, i,
                                          placementIdMax - pVgpuTypeSupportedPlacementInfo->placementSize))
            {

                pVgpuInstanceSupportedPlacementInfo =
                    &pVgpuTypeSupportedPlacementInfo->vgpuInstanceSupportedPlacementInfo[pVgpuTypeSupportedPlacementInfo->heterogeneousPlacementCount];
                pVgpuInstancePlacementInfo =
                    &pVgpuTypePlacementInfo->vgpuInstancePlacementInfo[pVgpuTypeSupportedPlacementInfo->heterogeneousPlacementCount];

                pVgpuInstanceSupportedPlacementInfo->heterogeneousSupportedPlacementId =
                    placementIdMax - pVgpuTypeSupportedPlacementInfo->placementSize;
                pVgpuInstanceSupportedPlacementInfo->heterogeneousSupportedChidOffset =
                    chidMax - pVgpuTypeSupportedPlacementInfo->channelCount;

                if (!IS_GSP_CLIENT(pGpu))
                    pVgpuInstancePlacementInfo->heterogeneousSupportedVmmuOffset = vmmuSegMax - pVgpuTypePlacementInfo->guestVmmuCount;
                else
                {
                    /* If profile occupies > 1 vMMU segment for GSP heap, place heap at end of vGPU FB. */
                    if (pVgpuTypeInfo->gspHeapSize >= vmmuSegSize)
                    {
                        pVgpuInstancePlacementInfo->heterogeneousSupportedVmmuOffset =
                            vmmuSegMax - pVgpuTypePlacementInfo->guestVmmuCount - (pVgpuTypeInfo->gspHeapSize / vmmuSegSize);
                        pVgpuInstancePlacementInfo->heterogeneousGspHeapOffset = (vmmuSegMax * vmmuSegSize) - pVgpuTypeInfo->gspHeapSize;
                    }
                    else
                    {
                        /*
                         * If we're in smaller parition, GSP vMMU segment will be input to the function.
                         * Place the gsp heap of first vGPU at the starting of the GSP vMMU segment.
                         */
                        pVgpuInstancePlacementInfo->heterogeneousSupportedVmmuOffset = vmmuSegMax - pVgpuTypePlacementInfo->guestVmmuCount;
                        pVgpuInstancePlacementInfo->heterogeneousGspHeapOffset = gspHeapOffsetMax - pVgpuTypeInfo->gspHeapSize;
                    }

                    /* For 32:1 vGPU types, the GSP heap is placed between 2 consecutive guest vMMU segments */
                    if (heterogeneousMaxInstance == 32)
                    {
                        pVgpuInstancePlacementInfo->heterogeneousSupportedVmmuOffset = vmmuSegMax - pVgpuTypePlacementInfo->guestVmmuCount;
                        pVgpuInstancePlacementInfo->heterogeneousGspHeapOffset =
                            ((vmmuSegMax - pVgpuTypePlacementInfo->guestVmmuCount) * vmmuSegSize) - pVgpuTypeInfo->gspHeapSize;
                    }
                }

                pVgpuTypeSupportedPlacementInfo->heterogeneousPlacementCount++;
            }
        }
    }

    /*
     * If the next recursive partition is for a left smaller partition which has GSP heap at
     * of start of partition, then update vmmuSegMin to reserve one segment at the
     * start of smaller partition. Also, init gsp min/max value for the reserved vMMU segment
     * at the start.
     */
    newVmmuSegMax = ((vmmuSegMin + vmmuSegMax) / 2);
    if (isCarveOutGspHeap)
    {
        NV_ASSERT((gspHeapOffsetMin == 0));

        newVmmuSegMin = vmmuSegMin + 1;
        newGspHeapOffsetMin = vmmuSegMin * vmmuSegSize;
        newGspHeapOffsetMax = newGspHeapOffsetMin + vmmuSegSize;
    }
    else
    {
        newVmmuSegMin = vmmuSegMin;
        newGspHeapOffsetMin = gspHeapOffsetMin;
        newGspHeapOffsetMax = (gspHeapOffsetMin + gspHeapOffsetMax) / 2;
    }

    /* Recursively call to get placment ID in left half of this partition */
    _kvgpumgrSetHeterogeneousResources(pGpu, pPgpuInfo, placementIdMin,
                             (placementIdMin + placementIdMax) / 2,
                             chidMin, (chidMin + chidMax) / 2, newVmmuSegMin,
                             newVmmuSegMax, newGspHeapOffsetMin, newGspHeapOffsetMax, partitionCount * 2,
                             NV_TRUE);

    /*
     * If the next recursive partition is for a right smaller partition which has GSP heap at
     * of end of partition, then update vmmuSegMax to reserve one segment at the
     * end of right partition. Also, init gsp min/max value for the reserved vMMU segment
     * at the end.
     */
    newVmmuSegMin = (vmmuSegMin + vmmuSegMax) / 2;
    if (isCarveOutGspHeap)
    {
        newVmmuSegMax = vmmuSegMax - 1;
        newGspHeapOffsetMin = newVmmuSegMax * vmmuSegSize;
        newGspHeapOffsetMax = newGspHeapOffsetMin + vmmuSegSize;
    }
    else
    {
        newVmmuSegMax = vmmuSegMax;
        newGspHeapOffsetMin = (gspHeapOffsetMin + gspHeapOffsetMax) / 2;;
        newGspHeapOffsetMax = gspHeapOffsetMax;
    }

    /* Recursively call to get placment ID in right half of this partition */
    _kvgpumgrSetHeterogeneousResources(pGpu, pPgpuInfo, (placementIdMin + placementIdMax) / 2,
                             placementIdMax, (chidMin + chidMax) / 2,
                             chidMax, newVmmuSegMin, newVmmuSegMax,
                             newGspHeapOffsetMin, newGspHeapOffsetMax, partitionCount * 2,
                             NV_FALSE);
}

/*
 * Determine the supported placement IDs for homogeneous vGPU profiles.
 */
static void
_kvgpumgrSetHomogeneousResources(OBJGPU *pGpu, KERNEL_PHYS_GPU_INFO *pPgpuInfo,
                                 NvU32 vmmuSegmentMin, NvU32 hostChannelCount)
{
    VGPU_TYPE *pVgpuTypeInfo;
    NvU32 i, j, chidMax, placementRegionIndexMax;
    VGPU_TYPE_SUPPORTED_PLACEMENT_INFO *pVgpuTypeSupportedPlacementInfo;
    VGPU_INSTANCE_SUPPORTED_PLACEMENT_INFO *pVgpuInstanceSupportedPlacementInfo;
    KERNEL_VGPU_TYPE_PLACEMENT_INFO *pKernelVgpuTypePlacementInfo;
    VGPU_TYPE_PLACEMENT_INFO *pVgpuTypePlacementInfo;
    VGPU_INSTANCE_PLACEMENT_INFO *pVgpuInstancePlacementInfo;

    if (!pGpu || !pPgpuInfo || !pPgpuInfo->homogeneousPlacementSupported) {
        return;
    }

    pKernelVgpuTypePlacementInfo = &pPgpuInfo->kernelVgpuTypePlacementInfo;

    for (i = 0; i < pPgpuInfo->numVgpuTypes; i++)
    {
        pVgpuTypeInfo = pPgpuInfo->vgpuTypes[i];
        chidMax = hostChannelCount;

        pVgpuTypeSupportedPlacementInfo = &pVgpuTypeInfo->vgpuTypeSupportedPlacementInfo;
        pVgpuTypePlacementInfo = &pKernelVgpuTypePlacementInfo->vgpuTypePlacementInfo[i];

        /*
         * Placement ID is not supported for vGPU MIG profiles. Hence, placementSize = 0
         * is already set for them in fn:kvgpumgrSetSupportedPlacementIds(). We use the
         * same below to skip those vGPU MIG profiles.
         */
        if (pVgpuTypeSupportedPlacementInfo->placementSize == 0)
            continue;

        placementRegionIndexMax = (pKernelVgpuTypePlacementInfo->placementRegionSize / pVgpuTypeSupportedPlacementInfo->placementSize);

        for (j = 0; j < placementRegionIndexMax; j++)
        {
            pVgpuInstanceSupportedPlacementInfo = &pVgpuTypeSupportedPlacementInfo->vgpuInstanceSupportedPlacementInfo[j];
            pVgpuInstancePlacementInfo = &pVgpuTypePlacementInfo->vgpuInstancePlacementInfo[j];

            pVgpuInstanceSupportedPlacementInfo->homogeneousSupportedPlacementId = j * pVgpuTypeSupportedPlacementInfo->placementSize;
            pVgpuInstancePlacementInfo->homogeneousSupportedVmmuOffset = vmmuSegmentMin + (j * pVgpuTypePlacementInfo->guestVmmuCount);

            pVgpuInstanceSupportedPlacementInfo->homogeneousSupportedChidOffset = chidMax - pVgpuTypeSupportedPlacementInfo->channelCount;
            chidMax -= pVgpuTypeSupportedPlacementInfo->channelCount;

            pVgpuTypeSupportedPlacementInfo->homogeneousPlacementCount++;

            if (pVgpuTypeSupportedPlacementInfo->homogeneousPlacementCount == pVgpuTypeInfo->maxInstance)
                break;
        }

        /*
         * Since, vGPU homogeneous mode is default, copy the supported placement IDs to
         * creatable placement IDs.
         */
        for (j = 0; j < pVgpuTypeSupportedPlacementInfo->homogeneousPlacementCount; j++)
        {
            pVgpuInstanceSupportedPlacementInfo = &pVgpuTypeSupportedPlacementInfo->vgpuInstanceSupportedPlacementInfo[j];

            pVgpuTypePlacementInfo->vgpuInstancePlacementInfo[j].creatablePlacementId =
                pVgpuInstanceSupportedPlacementInfo->homogeneousSupportedPlacementId;
        }
    }
}

/*
 * Calculate Max (worst case) Total GSP plugin heap size among vGPU types for Blackwell+ vGPUs.
 *
 * For Blackwell+ vGPUs, for 1:1 profile, GSP plugin heap size is 512MB.
 * However for N:1 profiles, total FB consumption for GSP plugin heap of
 * the running vGPUs can be more than 512MB.
 * E.g. 32:1 profile, 32MB heap size per instance = 1024MB total heap size.
 * So find out max or worst case Total GSP plugin heap size.
 */
static NvU64
_kvgpumgrGetMaxTotalGspHeapSize(OBJGPU *pGpu, KERNEL_PHYS_GPU_INFO *pPgpuInfo)
{
    NvU32 i;
    NvU64 totalGspHeapSize;
    NvU64 maxTotalGspHeapSize = 0;
    VGPU_TYPE *pVgpuTypeInfo;
    NvU32 maxInstance;

    /* Calculate Max Total GSP Plugin heap size */
    for (i = 0; i < pPgpuInfo->numVgpuTypes; i++)
    {
        pVgpuTypeInfo = pPgpuInfo->vgpuTypes[i];
        if (pVgpuTypeInfo == NULL)
            break;


        if (IS_MIG_IN_USE(pGpu) && kvgpumgrIsMigTimeslicingModeEnabled(pGpu))
        {
            // Ignore non-MIG vgpu types
            if (pVgpuTypeInfo->gpuInstanceSize == 0)
                continue;
            else
                maxInstance = pVgpuTypeInfo->maxInstancePerGI;
        }
        /* Ignore MIG vGPUs if MIG is not in use or mig timeslicing is not supported */
        else if (pVgpuTypeInfo->gpuInstanceSize != 0)
            continue;
        else
            maxInstance = pVgpuTypeInfo->maxInstance;


        totalGspHeapSize = pVgpuTypeInfo->gspHeapSize * maxInstance;

        if (maxTotalGspHeapSize < totalGspHeapSize)
            maxTotalGspHeapSize = totalGspHeapSize;
    }

    return maxTotalGspHeapSize;
}

static NvU32
_kvgpumgrGetMemDivFromPartitionFlag(NvU32 memSizeFlag)
{
    NvU32 memDiv = 0;

    switch (memSizeFlag)
    {
        case NV2080_CTRL_GPU_PARTITION_FLAG_MEMORY_SIZE_FULL:
            memDiv = 1;
            break;
        case NV2080_CTRL_GPU_PARTITION_FLAG_MEMORY_SIZE_HALF:
            memDiv = 2;
            break;
        case NV2080_CTRL_GPU_PARTITION_FLAG_MEMORY_SIZE_QUARTER:
            memDiv = 4;
            break;
        case NV2080_CTRL_GPU_PARTITION_FLAG_MEMORY_SIZE_EIGHTH:
            memDiv = 8;
            break;
        default:
            NV_ASSERT(0);
            NV_PRINTF(LEVEL_ERROR, "Unrecognized GPU mem partitioning flag 0x%x\n",
                      memSizeFlag);
            break;
    }
    return memDiv;
}

/* Set Resources for Heterogeneous Blackwell+ vGPUs */
static void
_kvgpumgrSetHeterogeneousResources_GB100(OBJGPU *pGpu,
                                         KERNEL_PHYS_GPU_INFO *pPgpuInfo,
                                         NvU32 vmmuSegmentMin,
                                         NvU32 totalVmmuCount,
                                         NvU32 hostChannelCount)
{
    NvU32 i, j;
    NvU64 vmmuSegSize;
    NvU32 placementRegionSize;
    NvU32 placementRegionSizePerGi;
    NvU32 guestMaxVmmuCount;
    NvU64 maxTotalGspHeapSize;
    NvU64 gspHeapStartOffset;
    NvU64 gspHeapOffset;
    VGPU_TYPE *pVgpuTypeInfo;
    KERNEL_VGPU_TYPE_PLACEMENT_INFO *pKernelVgpuTypePlacementInfo;
    VGPU_TYPE_PLACEMENT_INFO *pVgpuTypePlacementInfo;
    VGPU_INSTANCE_PLACEMENT_INFO *pVgpuInstancePlacementInfo;
    VGPU_TYPE_SUPPORTED_PLACEMENT_INFO *pVgpuTypeSupportedPlacementInfo;
    VGPU_INSTANCE_SUPPORTED_PLACEMENT_INFO *pVgpuInstanceSupportedPlacementInfo;

    /*
     * For Blackwell+ GPUs, for 1:1 profile, plugin GSP heap size is 512MB.
     * However for N:1 profiles, total FB consumption for GSP plugin heap of
     * the running vGPUs can be more than 512MB.
     * So for resource offset calculations, we consider max available FB for guests
     * after accounting for max total GSP heap size.
     */
    maxTotalGspHeapSize = _kvgpumgrGetMaxTotalGspHeapSize(pGpu, pPgpuInfo);
    vmmuSegSize = gpuGetVmmuSegmentSize(pGpu);
    guestMaxVmmuCount = totalVmmuCount - ((maxTotalGspHeapSize + vmmuSegSize - 1) / vmmuSegSize);

    /* GSP Heap Start offset for N:1 */
    gspHeapStartOffset = (vmmuSegmentMin + guestMaxVmmuCount) * vmmuSegSize;

    pKernelVgpuTypePlacementInfo = &pPgpuInfo->kernelVgpuTypePlacementInfo;
    placementRegionSize = pKernelVgpuTypePlacementInfo->placementRegionSize;
    placementRegionSizePerGi = placementRegionSize;

    for (i = 0; i < pPgpuInfo->numVgpuTypes; i++)
    {
        NvU32 maxInstance;
        NvU32 memDiv = 0;

        pVgpuTypeInfo = pPgpuInfo->vgpuTypes[i];
        if (pVgpuTypeInfo == NULL)
            break;

        pVgpuTypeSupportedPlacementInfo = &pVgpuTypeInfo->vgpuTypeSupportedPlacementInfo;
        pVgpuTypePlacementInfo = &pKernelVgpuTypePlacementInfo->vgpuTypePlacementInfo[i];

        /* Skip MIG vGPUs profiles (pre-GB20x, MIG placementSize is set to 0) */
        if (pVgpuTypeSupportedPlacementInfo->placementSize == 0)
            continue;

        /* Set placementIds and resource offsets for each instance of this vGPU type */
        pVgpuTypeSupportedPlacementInfo->heterogeneousPlacementCount = 0;
        if (pGpu->getProperty(pGpu, PDB_PROP_GPU_MIG_TIMESLICING_SUPPORTED) && pVgpuTypeInfo->gpuInstanceSize != 0)
        {
            NvU32 partitionFlag                 = PARTITIONID_INVALID;

            NV_ASSERT_OR_RETURN_VOID(kvgpumgrGetPartitionFlag(pVgpuTypeInfo->vgpuTypeId, &partitionFlag) == NV_OK);

            memDiv = _kvgpumgrGetMemDivFromPartitionFlag(
                (DRF_VAL(2080_CTRL_GPU, _PARTITION_FLAG, _MEMORY_SIZE, partitionFlag)));
            if (memDiv != 0)
                placementRegionSizePerGi = placementRegionSize / memDiv;
            else
                placementRegionSizePerGi = placementRegionSize;
            maxInstance = pVgpuTypeInfo->maxInstancePerGI;
        }
        else
        {
            memDiv = 0;
            maxInstance = pVgpuTypeInfo->maxInstance;
        }


        for (j = 0; j < maxInstance; j++)
        {
            pVgpuInstanceSupportedPlacementInfo =
                                &pVgpuTypeSupportedPlacementInfo->vgpuInstanceSupportedPlacementInfo[j];
            pVgpuInstancePlacementInfo = &pVgpuTypePlacementInfo->vgpuInstancePlacementInfo[j];

            if (pGpu->getProperty(pGpu, PDB_PROP_GPU_MIG_TIMESLICING_SUPPORTED) &&
                pVgpuTypeInfo->gpuInstanceSize != 0 &&
                memDiv != 0)
            {
                pVgpuInstanceSupportedPlacementInfo->heterogeneousSupportedPlacementId =
                    (placementRegionSizePerGi * j) / maxInstance;
            }
            else
            {
                pVgpuInstanceSupportedPlacementInfo->heterogeneousSupportedPlacementId =
                    (placementRegionSize * j) / maxInstance;
            }

            pVgpuInstanceSupportedPlacementInfo->heterogeneousSupportedChidOffset =
                                (hostChannelCount * j) / maxInstance;

            if (maxInstance == 1)
            {
                pVgpuInstancePlacementInfo->heterogeneousSupportedVmmuOffset = vmmuSegmentMin;

                pVgpuInstancePlacementInfo->heterogeneousGspHeapOffset =
                                (vmmuSegmentMin + pVgpuTypePlacementInfo->guestVmmuCount) * vmmuSegSize;
            }
            else
            {
                pVgpuInstancePlacementInfo->heterogeneousSupportedVmmuOffset =
                                vmmuSegmentMin + ((guestMaxVmmuCount * j) / maxInstance);

                gspHeapOffset = (maxTotalGspHeapSize * j) / maxInstance;
                pVgpuInstancePlacementInfo->heterogeneousGspHeapOffset =
                                gspHeapStartOffset + NV_ALIGN_DOWN(gspHeapOffset, RM_PAGE_SIZE_HUGE);
            }

            pVgpuTypeSupportedPlacementInfo->heterogeneousPlacementCount++;
        }
    }

    return;
}

/* Set Resources for Homogeneous Blackwell+ vGPUs */
static void
_kvgpumgrSetHomogeneousResources_GB100(OBJGPU *pGpu,
                                       KERNEL_PHYS_GPU_INFO *pPgpuInfo)
{
    NvU32 i, j;
    VGPU_TYPE *pVgpuTypeInfo;
    KERNEL_VGPU_TYPE_PLACEMENT_INFO *pKernelVgpuTypePlacementInfo;
    VGPU_TYPE_PLACEMENT_INFO *pVgpuTypePlacementInfo;
    VGPU_INSTANCE_PLACEMENT_INFO *pVgpuInstancePlacementInfo;
    VGPU_TYPE_SUPPORTED_PLACEMENT_INFO *pVgpuTypeSupportedPlacementInfo;
    VGPU_INSTANCE_SUPPORTED_PLACEMENT_INFO *pVgpuInstanceSupportedPlacementInfo;

    /*
     * For Blackwell+ GPUs, placementIds, max supported instances and resource offsets are
     * same for Homogeneous and Heterogeneous vGPUs.
     * So copy placementIds and resource offsets from Heterogeneous.
     */
    if (!pPgpuInfo->heterogeneousTimesliceSizesSupported)
    {
        pPgpuInfo->homogeneousPlacementSupported = NV_FALSE;
        NV_ASSERT_OR_RETURN_VOID(pPgpuInfo->homogeneousPlacementSupported);
    }

    pKernelVgpuTypePlacementInfo = &pPgpuInfo->kernelVgpuTypePlacementInfo;

    for (i = 0; i < pPgpuInfo->numVgpuTypes; i++)
    {
        pVgpuTypeInfo = pPgpuInfo->vgpuTypes[i];
        if (pVgpuTypeInfo == NULL)
            break;

        pVgpuTypeSupportedPlacementInfo = &pVgpuTypeInfo->vgpuTypeSupportedPlacementInfo;
        pVgpuTypePlacementInfo = &pKernelVgpuTypePlacementInfo->vgpuTypePlacementInfo[i];

        /* Skip MIG vGPUs profiles (pre-GB20x, MIG placementSize is set to 0) */
        if (pVgpuTypeSupportedPlacementInfo->placementSize == 0)
            continue;

        /* Copy placementIds and resource offsets for each instance of this vGPU type from Heterogeneous */
        pVgpuTypeSupportedPlacementInfo->homogeneousPlacementCount = 0;
        for (j = 0; j < pVgpuTypeSupportedPlacementInfo->heterogeneousPlacementCount; j++)
        {
            pVgpuInstanceSupportedPlacementInfo =
                                &pVgpuTypeSupportedPlacementInfo->vgpuInstanceSupportedPlacementInfo[j];
            pVgpuInstancePlacementInfo = &pVgpuTypePlacementInfo->vgpuInstancePlacementInfo[j];

            pVgpuInstanceSupportedPlacementInfo->homogeneousSupportedPlacementId =
                                pVgpuInstanceSupportedPlacementInfo->heterogeneousSupportedPlacementId;

            pVgpuInstanceSupportedPlacementInfo->homogeneousSupportedChidOffset =
                                pVgpuInstanceSupportedPlacementInfo->heterogeneousSupportedChidOffset;

            pVgpuInstancePlacementInfo->homogeneousSupportedVmmuOffset =
                                pVgpuInstancePlacementInfo->heterogeneousSupportedVmmuOffset;

            pVgpuTypeSupportedPlacementInfo->homogeneousPlacementCount++;
        }

        /*
         * Since, vGPU homogeneous mode is default, copy the supported placement IDs to
         * creatable placement IDs.
         */
        for (j = 0; j < pVgpuTypeSupportedPlacementInfo->homogeneousPlacementCount; j++)
        {
            pVgpuInstanceSupportedPlacementInfo =
                                &pVgpuTypeSupportedPlacementInfo->vgpuInstanceSupportedPlacementInfo[j];

            pVgpuTypePlacementInfo->vgpuInstancePlacementInfo[j].creatablePlacementId =
                                pVgpuInstanceSupportedPlacementInfo->homogeneousSupportedPlacementId;
        }
    }

    return;
}

NV_STATUS
kvgpumgrSetSupportedPlacementIds(OBJGPU *pGpu)
{
    OBJSYS     *pSys     = SYS_GET_INSTANCE();
    KernelVgpuMgr *pKernelVgpuMgr = SYS_GET_KERNEL_VGPUMGR(pSys);
    KernelFifo *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    NV_STATUS rmStatus = NV_OK;
    KERNEL_PHYS_GPU_INFO *pPgpuInfo;
    NvU32 index, i, j;
    VGPU_TYPE *pVgpuTypeInfo;
    NvU32 hostChannelCount = 0, totalVmmuCount = 0;
    NvU64 totalReservedFb, vgpuReservedFb, guestFbLength, totalAvailableFb, totalRequiredFb;
    NvU64 gspHeapOffsetMax, gspHeapOffsetMin, pmaRegionLength, pmaBaseOffset, largestOffset;
    NvU64 vmmuOffsetMin, vmmuOffsetMax, vmmuSegmentMin, vmmuSegmentMax;
    Heap *pHeap = GPU_GET_HEAP(pGpu);
    VGPU_TYPE_SUPPORTED_PLACEMENT_INFO *pVgpuTypeSupportedPlacementInfo;
    VGPU_INSTANCE_SUPPORTED_PLACEMENT_INFO *pVgpuInstanceSupportedPlacementInfo;
    KERNEL_VGPU_TYPE_PLACEMENT_INFO *pKernelVgpuTypePlacementInfo;
    VGPU_TYPE_PLACEMENT_INFO *pVgpuTypePlacementInfo;

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            kvgpumgrGetPgpuIndex(pKernelVgpuMgr, pGpu->gpuId, &index));

    pPgpuInfo = &(pKernelVgpuMgr->pgpuInfo[index]);

    pPgpuInfo->heterogeneousTimesliceSizesSupported = NV_FALSE;
    pPgpuInfo->homogeneousPlacementSupported = NV_FALSE;

    /* Heterogeneous vgpus enabled for SRIOV vGPUs on VMware, Device VM and KVM */
    if (gpuIsSriovEnabled(pGpu)
        )
    {
        if ((osIsVgpuVfioPresent() == NV_OK) ||
            (osIsVgpuDeviceVmPresent() == NV_OK))
        {
            pPgpuInfo->heterogeneousTimesliceSizesSupported = NV_TRUE;
        }
    }

    /*
     * We shall support Homogeneous vGPU placement ID feature only for SR-IOV vGPUs
     * with GSP. ADA vGPU qualifies this requirement. Hence, enable Homoogeneous vGPU
     * placement ID support for only ADA+.
     */
    if (IsADAorBetter(pGpu))
    {
        /*
         * Enable Homogeneous vgpu only on vGPUs on VMware, Device VM and KVM.
         * This will align it with Heterogeneous vgpu support.
         */
        if ((osIsVgpuVfioPresent() == NV_OK) ||
            (osIsVgpuDeviceVmPresent() == NV_OK))
        {
            pPgpuInfo->homogeneousPlacementSupported = NV_TRUE;
        }
    }

    if (!pPgpuInfo->heterogeneousTimesliceSizesSupported && !pPgpuInfo->homogeneousPlacementSupported)
        return rmStatus;

    hostChannelCount = kfifoChidMgrGetNumChannels(pGpu, pKernelFifo, pKernelFifo->ppChidMgr[0]);

    pKernelVgpuTypePlacementInfo = &pPgpuInfo->kernelVgpuTypePlacementInfo;

    for (i = 0; i < pPgpuInfo->numVgpuTypes; i++)
    {
        pVgpuTypeInfo = pPgpuInfo->vgpuTypes[i];
        if (pVgpuTypeInfo == NULL)
            break;

        pVgpuTypeSupportedPlacementInfo = &pVgpuTypeInfo->vgpuTypeSupportedPlacementInfo;
        pVgpuTypePlacementInfo = &pKernelVgpuTypePlacementInfo->vgpuTypePlacementInfo[i];


        for (j = 0; j < MAX_VGPU_DEVICES_PER_PGPU; j++)
        {
            /* Set to NVA081_PLACEMENT_ID_INVALID initially */
            pVgpuInstanceSupportedPlacementInfo = &pVgpuTypeSupportedPlacementInfo->vgpuInstanceSupportedPlacementInfo[j];

            pVgpuInstanceSupportedPlacementInfo->heterogeneousSupportedPlacementId = NVA081_PLACEMENT_ID_INVALID;
            pVgpuInstanceSupportedPlacementInfo->homogeneousSupportedPlacementId   = NVA081_PLACEMENT_ID_INVALID;

        }

        /* Ignore MIG vGPUs if timeslice is not supported */
        if (pVgpuTypeInfo->gpuInstanceSize != 0 && !pGpu->getProperty(pGpu, PDB_PROP_GPU_MIG_TIMESLICING_SUPPORTED))
        {
            pVgpuTypeSupportedPlacementInfo->placementSize = 0;
            continue;
        }

        pVgpuTypeSupportedPlacementInfo->heterogeneousPlacementCount = 0;
        pVgpuTypeSupportedPlacementInfo->homogeneousPlacementCount = 0;
        pVgpuTypePlacementInfo->guestVmmuCount = 0;

        pVgpuTypeSupportedPlacementInfo->placementSize = pVgpuTypeInfo->profileSize / (1024 * 1024 * 1024);

        if (gpuIsSriovEnabled(pGpu))
        {
            if (gpuIsNonPowerOf2ChannelCountSupported(pGpu))
                pVgpuTypeSupportedPlacementInfo->channelCount = hostChannelCount / pVgpuTypeInfo->maxInstance;
            else
                pVgpuTypeSupportedPlacementInfo->channelCount = nvPrevPow2_U32(hostChannelCount /
                                                             pVgpuTypeInfo->maxInstance);

            /*
             * For sriov vGPUs with GSP disabled, guest FB length is only dependent on
             * profile fb length and RM reserved fb (including ECC). Calculate the
             * guest fb length here by reducing reserved FB from profile fb length
             * depending on max instance and aligning it to VMMU segment size.
             */
            if (!IS_GSP_CLIENT(pGpu))
            {
                totalReservedFb = memmgrGetVgpuHostRmReservedFb_HAL(pGpu,
                                    pMemoryManager, pVgpuTypeInfo->vgpuTypeId) +
                                  kvgpumgrGetEccAndPrReservedFb(pGpu);

                vgpuReservedFb = NV_ALIGN_UP((totalReservedFb / pVgpuTypeInfo->maxInstance),
                                     gpuGetVmmuSegmentSize(pGpu));

                /*
                 * For 1GB profiles on 48GB board, maxInstances is limited to 32. For such
                 * profiles, there is no per vgpu reserved fb carved out from guest fb as
                 * host has space for reserved fb.
                 */
                if (pVgpuTypeInfo->profileSize == VGPU_LOWER_FB_PROFILE_SIZE_1_GB)
                {
                    heapGetUsableSize(pHeap, &totalAvailableFb);

                    /* Total FB needed if vgpu reserved fb is 0 and not carved from guest fb */
                    totalRequiredFb = pVgpuTypeInfo->maxInstance *
                                      (VGPU_LOWER_FB_PROFILE_SIZE_1_GB + vgpuReservedFb);

                    if (totalRequiredFb < totalAvailableFb)
                        vgpuReservedFb = 0;
                }

                guestFbLength = pVgpuTypeInfo->profileSize - vgpuReservedFb;

                pVgpuTypePlacementInfo->guestVmmuCount = guestFbLength / gpuGetVmmuSegmentSize(pGpu);
            }
            else
            {
                if (IsGB100orBetter(pGpu))
                {
                    /* Get the guest FB from vGPU type */
                    /* TODO: Handle ECC enabled scenario */
                    pVgpuTypePlacementInfo->guestVmmuCount = pVgpuTypeInfo->fbLength / gpuGetVmmuSegmentSize(pGpu);
                }
                else
                {
                    /* Calculate the guest FB using similar calculation as done in vGPU plugin */
                    totalAvailableFb = pMemoryManager->Ram.fbTotalMemSizeMb << 20;
                    totalAvailableFb = NV_ALIGN_UP(totalAvailableFb, 8 * gpuGetVmmuSegmentSize(pGpu));

                    guestFbLength = (totalAvailableFb / pVgpuTypeInfo->maxInstance) -
                                    pVgpuTypeInfo->fbReservation - pVgpuTypeInfo->gspHeapSize;
                    guestFbLength = NV_MIN(guestFbLength, pVgpuTypeInfo->fbLength);
                    guestFbLength = NV_ALIGN_DOWN(guestFbLength, gpuGetVmmuSegmentSize(pGpu));

                    pVgpuTypePlacementInfo->guestVmmuCount = guestFbLength / gpuGetVmmuSegmentSize(pGpu);
                }
            }
        }

        if (pVgpuTypeInfo->maxInstance == 1)
        {
            pKernelVgpuTypePlacementInfo->placementRegionSize = pVgpuTypeSupportedPlacementInfo->placementSize;

            if (gpuIsSriovEnabled(pGpu))
            {
                if (pVgpuTypePlacementInfo->guestVmmuCount)
                {
                    totalVmmuCount = pVgpuTypePlacementInfo->guestVmmuCount;

                    if (IS_GSP_CLIENT(pGpu))
                        totalVmmuCount += (pVgpuTypeInfo->gspHeapSize / gpuGetVmmuSegmentSize(pGpu));
                }
            }
        }
    }

    /*
     * For SRIOV, the placement IDs are aligned to 1/2, 1/4, 1/8, 1/16 partitions
     * due to restrictions on channels assigned to VF being in power-of-2.
     */
    if (gpuIsSriovEnabled(pGpu))
    {
        pmaGetLargestFree(&pHeap->pmaObject, &pmaRegionLength,
                          &pmaBaseOffset, &largestOffset);

        vmmuOffsetMin = NV_ALIGN_UP(pmaBaseOffset, gpuGetVmmuSegmentSize(pGpu));
        vmmuOffsetMax = pmaBaseOffset + pmaRegionLength;

        totalRequiredFb = (totalVmmuCount * gpuGetVmmuSegmentSize(pGpu));
        if ((vmmuOffsetMax - vmmuOffsetMin) < totalRequiredFb)
        {
            NV_PRINTF(LEVEL_ERROR, "Required FB for heterogeneous vGPU (%llu) less "
                      "than available FB (%llu)\n", totalRequiredFb, pmaRegionLength);
            NV_ASSERT(0);

            // Disable heterogeneous vGPU support, but allow homogeneous placement ID vGPU support.
            pPgpuInfo->heterogeneousTimesliceSizesSupported = NV_FALSE;
        }

        vmmuSegmentMin = vmmuOffsetMin / gpuGetVmmuSegmentSize(pGpu);
        vmmuSegmentMax = vmmuSegmentMin + totalVmmuCount;

        gspHeapOffsetMin = 0;
        gspHeapOffsetMax = 0;

        if (pPgpuInfo->heterogeneousTimesliceSizesSupported)
        {
            if (IsGB100orBetter(pGpu))
                _kvgpumgrSetHeterogeneousResources_GB100(pGpu, pPgpuInfo, vmmuSegmentMin,
                                                         totalVmmuCount, hostChannelCount);
            else
                _kvgpumgrSetHeterogeneousResources(pGpu, pPgpuInfo, 0,
                                                   pKernelVgpuTypePlacementInfo->placementRegionSize,
                                                   0, hostChannelCount, vmmuSegmentMin, vmmuSegmentMax,
                                                   gspHeapOffsetMin, gspHeapOffsetMax, 1, NV_TRUE);
        }

        if (pPgpuInfo->homogeneousPlacementSupported)
        {
            if (IsGB100orBetter(pGpu))
                _kvgpumgrSetHomogeneousResources_GB100(pGpu, pPgpuInfo);
            else
                _kvgpumgrSetHomogeneousResources(pGpu, pPgpuInfo, vmmuSegmentMin, hostChannelCount);
        }
    }

    return rmStatus;
}

static NvBool
isPlacementOverlapping(NvU16 minId1, NvU16 maxId1, NvU16 id2, NvU16 size2)
{
    NvU16 max, min, size1;

    min = NV_MIN(minId1, id2);
    max = NV_MAX(maxId1, (id2 + size2));
    size1 = maxId1 - minId1;

    if ((size1 + size2) > (max - min))
        return NV_TRUE;

    return NV_FALSE;
}

static NvBool
isPlacementSubset(NvU16 min, NvU16 max, NvU16 id, NvU16 size)
{
    if ((min <= id) && (max >= (id + size - 1)))
        return NV_TRUE;

    return NV_FALSE;
}

static NV_STATUS
_kvgpumgrUpdateHomogeneousCreatablePlacementIds(OBJGPU *pGpu, NvU16 placementId,
                                                NvU32 vgpuTypeId, NvBool isAlloc,
                                                NvU32 swizzId)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    KernelVgpuMgr *pKernelVgpuMgr = SYS_GET_KERNEL_VGPUMGR(pSys);
    KERNEL_PHYS_GPU_INFO *pPgpuInfo;
    VGPU_TYPE *pVgpuTypeInfo;
    NvU32 index, i , j, placementSize;
    NvU32 vgpuCount = 0;
    NvBool bIdFound = NV_FALSE, bPartitionMatch;;
    KERNEL_VGPU_TYPE_PLACEMENT_INFO *pKernelVgpuTypePlacementInfo;
    VGPU_TYPE_PLACEMENT_INFO *pVgpuTypePlacementInfo;
    VGPU_INSTANCE_PLACEMENT_INFO *pVgpuInstancePlacementInfo;
    VGPU_TYPE_SUPPORTED_PLACEMENT_INFO *pVgpuTypeSupportedPlacementInfo;
    VGPU_INSTANCE_SUPPORTED_PLACEMENT_INFO *pVgpuInstanceSupportedPlacementInfo;

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            kvgpumgrGetPgpuIndex(pKernelVgpuMgr, pGpu->gpuId, &index));

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            kvgpumgrGetVgpuTypeInfo(vgpuTypeId, &pVgpuTypeInfo));

    pPgpuInfo = &(pKernelVgpuMgr->pgpuInfo[index]);
    pKernelVgpuTypePlacementInfo = kvgpuMgrGetVgpuPlacementInfo(pGpu, pPgpuInfo, swizzId);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, pKernelVgpuTypePlacementInfo != NULL, NV_ERR_OBJECT_NOT_FOUND);

    pVgpuTypeSupportedPlacementInfo = &pVgpuTypeInfo->vgpuTypeSupportedPlacementInfo;
    placementSize = pVgpuTypeSupportedPlacementInfo->placementSize;

    if (osIsVgpuVfioPresent() == NV_OK)
        vgpuCount = pPgpuInfo->numCreatedVgpu;
    else
        vgpuCount = pPgpuInfo->numActiveVgpu;

    /* In Allocation path, check whether input placement ID is available to use. */
    if (isAlloc)
    {
        for (i = 0; i < pPgpuInfo->numVgpuTypes; i++)
        {
            pVgpuTypeInfo = pPgpuInfo->vgpuTypes[i];

            if (pVgpuTypeInfo == NULL)
                continue;

            pVgpuTypeSupportedPlacementInfo = &pVgpuTypeInfo->vgpuTypeSupportedPlacementInfo;
            pVgpuTypePlacementInfo = &pKernelVgpuTypePlacementInfo->vgpuTypePlacementInfo[i];

            if (pVgpuTypeSupportedPlacementInfo->placementSize != placementSize)
                continue;
            if (IS_MIG_IN_USE(pGpu))
            {
                if (!pVgpuTypeInfo->gpuInstanceSize) // Ignore non MIG vgpu types
                    continue;
                if (_kvgpumgrCheckPartitionFlag(pGpu, swizzId, pVgpuTypeInfo->vgpuTypeId, &bPartitionMatch) == NV_OK)
                {
                    if (!bPartitionMatch)
                        continue;
                }
            }
            else
            {
                if (pVgpuTypeInfo->gpuInstanceSize) // Ignore MIG vgpu types
                    continue;
            }
            for (j = 0; j < pVgpuTypeSupportedPlacementInfo->homogeneousPlacementCount; j++)
            {
                pVgpuInstancePlacementInfo = &pVgpuTypePlacementInfo->vgpuInstancePlacementInfo[j];

                if (pVgpuInstancePlacementInfo->creatablePlacementId == placementId)
                {
                    pVgpuInstancePlacementInfo->creatablePlacementId = NVA081_PLACEMENT_ID_INVALID;
                    bIdFound = NV_TRUE;
                    break;
                }
            }

            /* No creatable placement ID found */
            if (!bIdFound)
            {
                NV_PRINTF(LEVEL_ERROR, "Failed to find creatable placement id for vGPU type %d with placement id %d\n",
                    pVgpuTypeInfo->vgpuTypeId, placementId);
                return NV_ERR_INVALID_STATE;
            }
        }

        /*
         * If creatable placement ID found then, mark creatable placement IDs for other
         * vGPU Types with profile size not equal placement size of input to vgpuTypeId
         * as invalid.
         */
        for (i = 0; i < pPgpuInfo->numVgpuTypes; i++)
        {
            pVgpuTypeInfo = pPgpuInfo->vgpuTypes[i];

            if (pVgpuTypeInfo == NULL)
                continue;

            pVgpuTypeSupportedPlacementInfo = &pVgpuTypeInfo->vgpuTypeSupportedPlacementInfo;
            pVgpuTypePlacementInfo = &pKernelVgpuTypePlacementInfo->vgpuTypePlacementInfo[i];

            if (pVgpuTypeSupportedPlacementInfo->placementSize == placementSize)
                continue;

            if (IS_MIG_IN_USE(pGpu))
            {
                if (!pVgpuTypeInfo->gpuInstanceSize) // Ignore non MIG vgpu types
                    continue;
                if (_kvgpumgrCheckPartitionFlag(pGpu, swizzId, pVgpuTypeInfo->vgpuTypeId, &bPartitionMatch) == NV_OK)
                {
                    if (!bPartitionMatch)
                        continue;
                }
            }
            for (j = 0; j < pVgpuTypeSupportedPlacementInfo->homogeneousPlacementCount; j++)
            {
                pVgpuInstancePlacementInfo = &pVgpuTypePlacementInfo->vgpuInstancePlacementInfo[j];

                pVgpuInstancePlacementInfo->creatablePlacementId = NVA081_PLACEMENT_ID_INVALID;
            }
        }
    }
    /* In Free path, accumulate the freed placement ID */
    else
    {
        for (i = 0; i < pPgpuInfo->numVgpuTypes; i++)
        {
            pVgpuTypeInfo = pPgpuInfo->vgpuTypes[i];

            if (pVgpuTypeInfo == NULL)
                continue;

            pVgpuTypeSupportedPlacementInfo = &pVgpuTypeInfo->vgpuTypeSupportedPlacementInfo;
            pVgpuTypePlacementInfo = &pKernelVgpuTypePlacementInfo->vgpuTypePlacementInfo[i];

            /*
             * Restore all the creatable placement IDs with the supported placement IDs when
             * the last vGPU instance is freed.
             */
            if (vgpuCount == 0)
            {
                for (j = 0; j < pVgpuTypeSupportedPlacementInfo->homogeneousPlacementCount; j++)
                {
                    pVgpuInstancePlacementInfo = &pVgpuTypePlacementInfo->vgpuInstancePlacementInfo[j];
                    pVgpuInstanceSupportedPlacementInfo = &pVgpuTypeSupportedPlacementInfo->vgpuInstanceSupportedPlacementInfo[j];

                    pVgpuInstancePlacementInfo->creatablePlacementId = pVgpuInstanceSupportedPlacementInfo->homogeneousSupportedPlacementId;
                }
            }
            else
            {
                if (pVgpuTypeSupportedPlacementInfo->placementSize != placementSize)
                    continue;

                if (IS_MIG_IN_USE(pGpu))
                {
                    if (!pVgpuTypeInfo->gpuInstanceSize) // Ignore non MIG vgpu types
                        continue;
                    if (_kvgpumgrCheckPartitionFlag(pGpu, swizzId, pVgpuTypeInfo->vgpuTypeId, &bPartitionMatch) == NV_OK)
                    {
                        if (!bPartitionMatch)
                            continue;
                    }
                }
                /*
                 * Accumulate the freed placement ID for vGPU instance at its respective
                 * place where creatablePlacementIds slot is invalid.
                 */
                for (j = 0; j < pVgpuTypeSupportedPlacementInfo->homogeneousPlacementCount; j++)
                {
                    pVgpuInstancePlacementInfo = &pVgpuTypePlacementInfo->vgpuInstancePlacementInfo[j];
                    pVgpuInstanceSupportedPlacementInfo =
                        &pVgpuTypeSupportedPlacementInfo->vgpuInstanceSupportedPlacementInfo[j];

                    if ((pVgpuInstanceSupportedPlacementInfo->homogeneousSupportedPlacementId == placementId) &&
                        (pVgpuInstancePlacementInfo->creatablePlacementId == NVA081_PLACEMENT_ID_INVALID))
                    {
                        pVgpuInstancePlacementInfo->creatablePlacementId = placementId;
                        bIdFound = NV_TRUE;
                        break;
                    }
                }

                /* No creatable invalid placement ID found */
                if (!bIdFound)
                    return NV_ERR_INVALID_STATE;
            }
        }
    }

    return NV_OK;
}

static NV_STATUS
_kvgpumgrUpdateHeterogeneousCreatablePlacementIds(OBJGPU *pGpu, NvU16 placementId,
                                                  NvU32 vgpuTypeId, NvBool isAlloc,
                                                  NvU32 swizzId)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    KernelVgpuMgr *pKernelVgpuMgr = SYS_GET_KERNEL_VGPUMGR(pSys);
    KERNEL_PHYS_GPU_INFO *pPgpuInfo;
    VGPU_TYPE *pVgpuTypeInfo;
    NvU32 index, i , j, placementSize, min = 0, max = 0, numCreatable = 0;
    NV_RANGE range;
    NvBool isVgpuTypeCreatable, bPartitionMatch;
    KERNEL_VGPU_TYPE_PLACEMENT_INFO *pKernelVgpuTypePlacementInfo;
    VGPU_TYPE_PLACEMENT_INFO *pVgpuTypePlacementInfo;
    VGPU_INSTANCE_PLACEMENT_INFO *pVgpuInstancePlacementInfo;
    VGPU_TYPE_SUPPORTED_PLACEMENT_INFO *pVgpuTypeSupportedPlacementInfo;
    VGPU_INSTANCE_SUPPORTED_PLACEMENT_INFO *pVgpuInstanceSupportedPlacementInfo;
    NvU32 *pSupportedTypeId;

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            kvgpumgrGetPgpuIndex(pKernelVgpuMgr, pGpu->gpuId, &index));

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            kvgpumgrGetVgpuTypeInfo(vgpuTypeId, &pVgpuTypeInfo));

    pPgpuInfo = &(pKernelVgpuMgr->pgpuInfo[index]);

    pKernelVgpuTypePlacementInfo = kvgpuMgrGetVgpuPlacementInfo(pGpu, pPgpuInfo, swizzId);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, pKernelVgpuTypePlacementInfo != NULL, NV_ERR_OBJECT_NOT_FOUND);
    pSupportedTypeId = kvgpuMgrGetVgpuCreatableTypeIdFromSwizzId(pGpu, pPgpuInfo, swizzId);

    pVgpuTypeSupportedPlacementInfo = &pVgpuTypeInfo->vgpuTypeSupportedPlacementInfo;
    placementSize = pVgpuTypeSupportedPlacementInfo->placementSize;

    range = rangeMake(placementId, placementId + placementSize - 1);

    if (isAlloc)
    {
        /* Update the placment region bitmap to set (id + size) slots */
        bitVectorSetRange(&pKernelVgpuTypePlacementInfo->usedPlacementRegionMap, range);
        min = placementId;
        max = placementId + placementSize;
    }
    else
    {
        /* Update the placment region bitmap to unset (id + size) slots */
        bitVectorClrRange(&pKernelVgpuTypePlacementInfo->usedPlacementRegionMap, range);

        /* find the minimum slot before placement ID which is not occupied */
        min = placementId;
        for (j = placementId; j > 0;)
        {
            j--;

            if (bitVectorTest(&pKernelVgpuTypePlacementInfo->usedPlacementRegionMap, j))
                break;

            min = j;
        }

        /* find the maximum slot after placement ID + placementSize which is not occupied */
        max = placementId + placementSize - 1;
        for (j = placementId + placementSize; j < pKernelVgpuTypePlacementInfo->placementRegionSize; j++)
        {
            if (bitVectorTest(&pKernelVgpuTypePlacementInfo->usedPlacementRegionMap, j))
                break;

             max = j;
        }
    }

    for (i = 0; i < MAX_VGPU_TYPES_PER_PGPU; i++)
        pSupportedTypeId[i] = NVA081_CTRL_VGPU_CONFIG_INVALID_TYPE;

    /* Update creatable placement Ids of all vGPU type IDs */
    for (i = 0; i < pPgpuInfo->numVgpuTypes; i++)
    {
        pVgpuTypeInfo = pPgpuInfo->vgpuTypes[i];
        if (pVgpuTypeInfo == NULL)
            break;

        pVgpuTypeSupportedPlacementInfo = &pVgpuTypeInfo->vgpuTypeSupportedPlacementInfo;
        pVgpuTypePlacementInfo = &pKernelVgpuTypePlacementInfo->vgpuTypePlacementInfo[i];

        if (pVgpuTypeSupportedPlacementInfo->placementSize == 0)
            continue;

        if (IS_MIG_IN_USE(pGpu))
        {
            if (!pVgpuTypeInfo->gpuInstanceSize) // Ignore non MIG vgpu types
                continue;
            if (_kvgpumgrCheckPartitionFlag(pGpu, swizzId, pVgpuTypeInfo->vgpuTypeId, &bPartitionMatch) == NV_OK)
            {
                if (!bPartitionMatch)
                    continue;
            }
        }
        else
        {
            if (pVgpuTypeInfo->gpuInstanceSize) // Ignore MIG vgpu types
                continue;
        }
        isVgpuTypeCreatable = NV_FALSE;

        for (j = 0; j < pVgpuTypeSupportedPlacementInfo->heterogeneousPlacementCount; j++)
        {
            pVgpuInstanceSupportedPlacementInfo = &pVgpuTypeSupportedPlacementInfo->vgpuInstanceSupportedPlacementInfo[j];
            pVgpuInstancePlacementInfo = &pVgpuTypePlacementInfo->vgpuInstancePlacementInfo[j];

            /*
             * Creatable Placement IDs will be updated either on VM boot or VM shutdown.
             * 1. VM boot.
             *      Here, we compare every (id + size) of each vGPU type if it's partial/full
             *      overlapping with the (id + size) of the vGPU VM that is booting up.
             *      If yes, then mark that placement ID as INVALID.
             * 2. VM shutdown
             *      Here, we compare every INVALID id of each vGPU type and
             *      see if the corresponding supported (id + size) is a complete subset
             *      of (max - min) range of available placement slots.
             *      If it is subset, then the INVALID id is overwritten with
             *      corresponding supported id as it is now creatable.
             */
            if (pVgpuInstancePlacementInfo->creatablePlacementId != NVA081_PLACEMENT_ID_INVALID)
            {
                if (isAlloc && isPlacementOverlapping(min, max,
                                                      pVgpuInstancePlacementInfo->creatablePlacementId,
                                                      pVgpuTypeSupportedPlacementInfo->placementSize))
                {
                    pVgpuInstancePlacementInfo->creatablePlacementId = NVA081_PLACEMENT_ID_INVALID;
                }
            }
            else
            {
                if (!isAlloc && isPlacementSubset(min, max,
                                                  pVgpuInstanceSupportedPlacementInfo->heterogeneousSupportedPlacementId,
                                                  pVgpuTypeSupportedPlacementInfo->placementSize))
                {
                    pVgpuInstancePlacementInfo->creatablePlacementId =
                        pVgpuInstanceSupportedPlacementInfo->heterogeneousSupportedPlacementId;
                }
            }

            if (pVgpuInstancePlacementInfo->creatablePlacementId != NVA081_PLACEMENT_ID_INVALID)
                isVgpuTypeCreatable = NV_TRUE;
        }

        if (isVgpuTypeCreatable)
            pSupportedTypeId[numCreatable++] = pVgpuTypeInfo->vgpuTypeId;
    }

    return NV_OK;
}

NV_STATUS
kvgpumgrUpdateHeterogeneousInfo(OBJGPU *pGpu, Device *pDevice,
                                NvU32 vgpuTypeId, NvU16 *placementId,
                                NvU64 *guestFbLength, NvU64 *guestFbOffset,
                                NvU64 *gspHeapOffset, NvU64 *guestBar1PFOffset)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    KernelVgpuMgr *pKernelVgpuMgr = SYS_GET_KERNEL_VGPUMGR(pSys);
    KERNEL_PHYS_GPU_INFO *pPgpuInfo;
    VGPU_TYPE *pVgpuTypeInfo;
    NvU32 index, i , j, swizzId = KMIGMGR_SWIZZID_INVALID;
    NvBool bIdFound = NV_FALSE;
    VGPU_TYPE_SUPPORTED_PLACEMENT_INFO *pVgpuTypeSupportedPlacementInfo;
    KERNEL_VGPU_TYPE_PLACEMENT_INFO *pKernelVgpuTypePlacementInfo;
    VGPU_TYPE_PLACEMENT_INFO *pVgpuTypePlacementInfo;
    VGPU_INSTANCE_PLACEMENT_INFO *pVgpuInstancePlacementInfo;

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            kvgpumgrGetPgpuIndex(pKernelVgpuMgr, pGpu->gpuId, &index));

    pPgpuInfo = &(pKernelVgpuMgr->pgpuInfo[index]);

    if (IS_MIG_IN_USE(pGpu) && kvgpumgrIsMigTimeslicingModeEnabled(pGpu))
    {
        swizzId = kvgpuMgrGetSwizzIdFromDevice(pGpu, pDevice);
        pKernelVgpuTypePlacementInfo = kvgpuMgrGetVgpuPlacementInfo(pGpu, pPgpuInfo, swizzId);
        NV_CHECK_OR_RETURN(LEVEL_ERROR, pKernelVgpuTypePlacementInfo != NULL, NV_ERR_OBJECT_NOT_FOUND);
    }
    else
    {
        pKernelVgpuTypePlacementInfo = &pPgpuInfo->kernelVgpuTypePlacementInfo;
    }

    for (i = 0; i < pPgpuInfo->numVgpuTypes; i++)
    {
        pVgpuTypeInfo = pPgpuInfo->vgpuTypes[i];
        if (pVgpuTypeInfo == NULL)
            break;

        pVgpuTypeSupportedPlacementInfo = &pVgpuTypeInfo->vgpuTypeSupportedPlacementInfo;
        pVgpuTypePlacementInfo = &pKernelVgpuTypePlacementInfo->vgpuTypePlacementInfo[i];

        if (pVgpuTypeInfo->vgpuTypeId == vgpuTypeId)
        {
            for (j = 0; j < pVgpuTypeSupportedPlacementInfo->heterogeneousPlacementCount; j++)
            {
                pVgpuInstancePlacementInfo = &pVgpuTypePlacementInfo->vgpuInstancePlacementInfo[j];

                /*
                 * If hypervisor hasn't provided the placement ID as input, look
                 * for the first available placement ID for the input vGPU type.
                 * If hypervisor has provided the placement ID as input, just
                 * validate if it is a creatable placement ID.
                 */
                if (*placementId == NVA081_PLACEMENT_ID_INVALID)
                {
                    if (pVgpuInstancePlacementInfo->creatablePlacementId != NVA081_PLACEMENT_ID_INVALID)
                    {
                        *placementId = pVgpuInstancePlacementInfo->creatablePlacementId;
                        if (gpuIsSriovEnabled(pGpu))
                        {
                            *guestFbOffset = pVgpuInstancePlacementInfo->heterogeneousSupportedVmmuOffset *
                                             gpuGetVmmuSegmentSize(pGpu);
                            *guestFbLength = pVgpuTypePlacementInfo->guestVmmuCount *
                                             gpuGetVmmuSegmentSize(pGpu);
                            *gspHeapOffset = pVgpuInstancePlacementInfo->heterogeneousGspHeapOffset;
                            *guestBar1PFOffset = 0;
                        }
                        bIdFound = NV_TRUE;
                        break;
                    }
                }
                else
                {
                    if (pVgpuInstancePlacementInfo->creatablePlacementId == *placementId)
                    {
                        if (gpuIsSriovEnabled(pGpu))
                        {
                            *guestFbOffset = pVgpuInstancePlacementInfo->heterogeneousSupportedVmmuOffset *
                                             gpuGetVmmuSegmentSize(pGpu);
                            *guestFbLength = pVgpuTypePlacementInfo->guestVmmuCount *
                                             gpuGetVmmuSegmentSize(pGpu);
                            *gspHeapOffset = pVgpuInstancePlacementInfo->heterogeneousGspHeapOffset;
                            *guestBar1PFOffset    = 0;
                        }
                        bIdFound = NV_TRUE;
                        break;
                    }
                }
            }
        }
    }

    /* No creatable placement ID found */
    if (!bIdFound)
        return NV_ERR_INVALID_STATE;

    return _kvgpumgrUpdateHeterogeneousCreatablePlacementIds(pGpu, *placementId, vgpuTypeId, NV_TRUE, swizzId);
}

NV_STATUS
kvgpumgrUpdateHomogeneousInfo(OBJGPU *pGpu, Device *pDevice,
                              NvU32 vgpuTypeId, NvU16 *placementId,
                              NvU64 *guestFbLength, NvU64 *guestFbOffset)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    KernelVgpuMgr *pKernelVgpuMgr = SYS_GET_KERNEL_VGPUMGR(pSys);
    KERNEL_PHYS_GPU_INFO *pPgpuInfo;
    VGPU_TYPE *pVgpuTypeInfo;
    NvU32 index, i , j, swizzId = KMIGMGR_SWIZZID_INVALID;
    NvBool bIdFound = NV_FALSE;
    KERNEL_VGPU_TYPE_PLACEMENT_INFO *pKernelVgpuTypePlacementInfo;
    VGPU_TYPE_PLACEMENT_INFO *pVgpuTypePlacementInfo;
    VGPU_INSTANCE_PLACEMENT_INFO *pVgpuInstancePlacementInfo;
    VGPU_TYPE_SUPPORTED_PLACEMENT_INFO *pVgpuTypeSupportedPlacementInfo;

    NV_CHECK_OR_RETURN(LEVEL_ERROR, pGpu != NULL, NV_ERR_NOT_SUPPORTED);

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            kvgpumgrGetPgpuIndex(pKernelVgpuMgr, pGpu->gpuId, &index));

    pPgpuInfo = &(pKernelVgpuMgr->pgpuInfo[index]);

    if (IS_MIG_IN_USE(pGpu) && kvgpumgrIsMigTimeslicingModeEnabled(pGpu))
    {
        swizzId = kvgpuMgrGetSwizzIdFromDevice(pGpu, pDevice);
        pKernelVgpuTypePlacementInfo = kvgpuMgrGetVgpuPlacementInfo(pGpu, pPgpuInfo, swizzId);
        NV_CHECK_OR_RETURN(LEVEL_ERROR, pKernelVgpuTypePlacementInfo != NULL, NV_ERR_OBJECT_NOT_FOUND);
    }
    else
    {
        pKernelVgpuTypePlacementInfo = &pPgpuInfo->kernelVgpuTypePlacementInfo;
    }

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, kvgpumgrCheckHomogeneousPlacementSupported(pGpu, swizzId));

    for (i = 0; i < pPgpuInfo->numVgpuTypes; i++)
    {
        pVgpuTypeInfo = pPgpuInfo->vgpuTypes[i];
        if (pVgpuTypeInfo == NULL)
            break;

        if (pVgpuTypeInfo->vgpuTypeId == vgpuTypeId)
        {
            pVgpuTypeSupportedPlacementInfo = &pVgpuTypeInfo->vgpuTypeSupportedPlacementInfo;
            pVgpuTypePlacementInfo = &pKernelVgpuTypePlacementInfo->vgpuTypePlacementInfo[i];

            for (j = 0; j < pVgpuTypeSupportedPlacementInfo->homogeneousPlacementCount; j++)
            {
                pVgpuInstancePlacementInfo = &pVgpuTypePlacementInfo->vgpuInstancePlacementInfo[j];

                /*
                 * If hypervisor hasn't provided the placement ID as input, look
                 * for the first available placement ID for the input vGPU type.
                 * If hypervisor has provided the placement ID as input, just
                 * validate if it is a creatable placement ID.
                 */
                if (*placementId == NVA081_PLACEMENT_ID_INVALID)
                {
                    if (pVgpuInstancePlacementInfo->creatablePlacementId != NVA081_PLACEMENT_ID_INVALID)
                    {
                        *placementId = pVgpuInstancePlacementInfo->creatablePlacementId;
                        *guestFbOffset = pVgpuInstancePlacementInfo->homogeneousSupportedVmmuOffset *
                                         gpuGetVmmuSegmentSize(pGpu);
                        *guestFbLength = pVgpuTypePlacementInfo->guestVmmuCount *
                                         gpuGetVmmuSegmentSize(pGpu);
                        bIdFound = NV_TRUE;
                        break;
                    }
                }
                else
                {
                    if (pVgpuInstancePlacementInfo->creatablePlacementId == *placementId)
                    {
                        *guestFbOffset = pVgpuInstancePlacementInfo->homogeneousSupportedVmmuOffset *
                                         gpuGetVmmuSegmentSize(pGpu);
                        *guestFbLength = pVgpuTypePlacementInfo->guestVmmuCount *
                                         gpuGetVmmuSegmentSize(pGpu);
                        bIdFound = NV_TRUE;
                        break;
                    }
                }
            }
        }
    }

    /* No creatable placement ID found */
    if (!bIdFound)
        return NV_ERR_INVALID_STATE;

    return _kvgpumgrUpdateHomogeneousCreatablePlacementIds(pGpu, *placementId, vgpuTypeId, NV_TRUE, swizzId);
}

NV_STATUS
kvgpumgrGetConfigEventInfoFromDb(NvHandle hClient,
                                 NvHandle hVgpuConfig,
                                 VGPU_CONFIG_EVENT_INFO_NODE **ppVgpuConfigEventInfoNode,
                                 NvU32 pgpuIndex)
{
    OBJSYS     *pSys     = SYS_GET_INSTANCE();
    KernelVgpuMgr *pKernelVgpuMgr = SYS_GET_KERNEL_VGPUMGR(pSys);
    VGPU_CONFIG_EVENT_INFO_NODE *pVgpuConfigEventInfoNodeTemp;

    if (!ppVgpuConfigEventInfoNode)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    for (pVgpuConfigEventInfoNodeTemp = listHead(&(pKernelVgpuMgr->pgpuInfo[pgpuIndex].listVgpuConfigEventsHead));
         pVgpuConfigEventInfoNodeTemp != NULL;
         pVgpuConfigEventInfoNodeTemp = listNext(&(pKernelVgpuMgr->pgpuInfo[pgpuIndex].listVgpuConfigEventsHead), pVgpuConfigEventInfoNodeTemp))
    {
        if ((pVgpuConfigEventInfoNodeTemp->hClient == hClient) &&
            (pVgpuConfigEventInfoNodeTemp->hVgpuConfig == hVgpuConfig))
        {
            *ppVgpuConfigEventInfoNode = pVgpuConfigEventInfoNodeTemp;
            return NV_OK;
        }
    }

    *ppVgpuConfigEventInfoNode = NULL;
    return NV_ERR_OBJECT_NOT_FOUND;
}

NV_STATUS
kvgpuMgrRestoreSmcExecPart
(
    OBJGPU *pGpu,
    KERNEL_HOST_VGPU_DEVICE *pKernelHostVgpuDevice,
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance
)
{
    NV_STATUS rmStatus = NV_OK;
    NvU32 CIIdx;
    NVC637_CTRL_EXEC_PARTITIONS_IMPORT_EXPORT_PARAMS *pExecPartImportParams = NULL;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    //
    // Delete any compute instances which are alive in system. This may delete
    // instances existing in system even if they are same as default instance,
    // as we are not comparing instances
    //
    for (CIIdx = 0; CIIdx < NV_ARRAY_ELEMENTS(pKernelMIGGpuInstance->MIGComputeInstance); ++CIIdx)
    {
        NVC637_CTRL_EXEC_PARTITIONS_DELETE_PARAMS params;
        MIG_COMPUTE_INSTANCE *pMIGComputeInstance =
            &pKernelMIGGpuInstance->MIGComputeInstance[CIIdx];

        if (!pMIGComputeInstance->bValid)
            continue;

        portMemSet(&params, 0, sizeof(params));
        params.execPartCount = 1;
        params.execPartId[0] = CIIdx;

        //
        // There is no revert or failure in this call as we have to do best case
        // effort to delete and restore default compute instances
        //
        NV_ASSERT_OK(
            pRmApi->Control(pRmApi,
                            pKernelMIGGpuInstance->instanceHandles.hClient,
                            pKernelMIGGpuInstance->instanceHandles.hSubscription,
                            NVC637_CTRL_CMD_EXEC_PARTITIONS_DELETE,
                            &params,
                            sizeof(params)));
    }

    // Restore all saved compute instances
    pExecPartImportParams = portMemAllocNonPaged(sizeof(*pExecPartImportParams));
    NV_ASSERT_OR_RETURN(pExecPartImportParams != NULL, NV_ERR_NO_MEMORY);

    for (CIIdx = 0; CIIdx < NV_ARRAY_ELEMENTS(pKernelMIGGpuInstance->MIGComputeInstance); ++CIIdx)
    {
        GPUMGR_SAVE_COMPUTE_INSTANCE *pComputeInstanceSave = &pKernelHostVgpuDevice->savedExecPartitions[CIIdx];
        if (!pComputeInstanceSave->bValid)
            continue;

        portMemSet(pExecPartImportParams, 0, sizeof(*pExecPartImportParams));
        pExecPartImportParams->id = pComputeInstanceSave->id;
        pExecPartImportParams->bCreateCap = NV_TRUE;
        portMemCopy(&pExecPartImportParams->info, sizeof(pExecPartImportParams->info),
                          &pComputeInstanceSave->ciInfo, sizeof(pComputeInstanceSave->ciInfo));

        NV_ASSERT_OK_OR_GOTO(rmStatus,
                      pRmApi->Control(pRmApi,
                                      pKernelMIGGpuInstance->instanceHandles.hClient,
                                      pKernelMIGGpuInstance->instanceHandles.hSubscription,
                                      NVC637_CTRL_CMD_EXEC_PARTITIONS_IMPORT,
                                      pExecPartImportParams,
                                      sizeof(*pExecPartImportParams)),
                done);
    }

    //
    // Generate a subdevice event stating something has changed in GPU partition
    // config.
    //
    gpuNotifySubDeviceEvent(pGpu, NV2080_NOTIFIERS_SMC_CONFIG_UPDATE, NULL, 0, 0, 0);

done:
    portMemFree(pExecPartImportParams);
    return rmStatus;
}

/*
 * This function is currently supported only on Device VM.
 * Device VM use VM ID as identifier to set the VM Name.
 */
NV_STATUS
kvgpumgrGetHostVgpuDeviceFromVmId(NvU32 gpuPciId, NvU64 vmId,
                                  KERNEL_HOST_VGPU_DEVICE** ppHostVgpuDevice)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    KernelVgpuMgr *pKernelVgpuMgr = SYS_GET_KERNEL_VGPUMGR(pSys);
    NvU32 pgpuIndex;
    KERNEL_PHYS_GPU_INFO *pPgpuInfo;
    KERNEL_HOST_VGPU_DEVICE *pKernelHostVgpuDevice;

    if (osIsVgpuDeviceVmPresent() != NV_OK)
        return NV_ERR_NOT_SUPPORTED;

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, kvgpumgrGetPgpuIndex(pKernelVgpuMgr, gpuPciId, &pgpuIndex));
    pPgpuInfo = &pKernelVgpuMgr->pgpuInfo[pgpuIndex];

    if (!pPgpuInfo->sriovEnabled)
        return NV_ERR_NOT_SUPPORTED;

    for (pKernelHostVgpuDevice = listHead(&(pPgpuInfo->listHostVgpuDeviceHead));
         pKernelHostVgpuDevice != NULL;
         pKernelHostVgpuDevice = listNext(&(pPgpuInfo->listHostVgpuDeviceHead), pKernelHostVgpuDevice))
    {
        if (pKernelHostVgpuDevice->vgpuGuest->guestVmInfo.guestVmId.vmId == vmId)
        {
            *ppHostVgpuDevice = pKernelHostVgpuDevice;
            return NV_OK;
        }
     }

    return NV_ERR_OBJECT_NOT_FOUND;
}

NV_STATUS
kvgpumgrSetPlacementId(REQUEST_VGPU_INFO_NODE *pRequestVgpu, NvU32 placementId)
{
    OBJSYS                              *pSys           = SYS_GET_INSTANCE();
    KernelVgpuMgr                       *pKernelVgpuMgr = SYS_GET_KERNEL_VGPUMGR(pSys);
    OBJGPU                              *pGpu           = NULL;
    NV_STATUS                            rmStatus       = NV_OK;
    NvU32                                index, i , j, assignedPlacementId;
    KERNEL_PHYS_GPU_INFO                *pPgpuInfo;
    VGPU_TYPE                           *pVgpuTypeInfo;
    KERNEL_VGPU_TYPE_PLACEMENT_INFO     *pKernelVgpuTypePlacementInfo;
    NvU16                                placementCount;
    VGPU_TYPE_PLACEMENT_INFO            *pVgpuTypePlacementInfo;
    VGPU_INSTANCE_PLACEMENT_INFO        *pVgpuInstancePlacementInfo;
    VGPU_TYPE_SUPPORTED_PLACEMENT_INFO  *pVgpuTypeSupportedPlacementInfo;
    NvBool                               bIdFound = NV_FALSE, bHeterogeneousModeEnabled;

    // If the call is not coming from mdev-creation, verify the existing placement Id
    // against the placement Id to be assigned.
    if ((pRequestVgpu->placementId != NVA081_PLACEMENT_ID_INVALID) &&
        (pRequestVgpu->placementId == placementId))
        return NV_OK;

    pGpu = gpumgrGetGpuFromId(pRequestVgpu->gpuPciId);
    if (pGpu == NULL)
        return NV_ERR_NOT_SUPPORTED;

    // Check for placementId unsupported config.

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            kvgpumgrGetPgpuIndex(pKernelVgpuMgr, pGpu->gpuId, &index));
 
    pPgpuInfo = &(pKernelVgpuMgr->pgpuInfo[index]);
    if (IS_MIG_IN_USE(pGpu) && kvgpumgrIsMigTimeslicingModeEnabled(pGpu))
    {
        pKernelVgpuTypePlacementInfo = kvgpuMgrGetVgpuPlacementInfo(pGpu, pPgpuInfo, pRequestVgpu->swizzId);
        NV_CHECK_OR_RETURN(LEVEL_ERROR, pKernelVgpuTypePlacementInfo != NULL, NV_ERR_OBJECT_NOT_FOUND);
    }
    else
    {
        pKernelVgpuTypePlacementInfo = &pPgpuInfo->kernelVgpuTypePlacementInfo;
    }
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        kvgpuMgrGetHeterogeneousMode(pGpu, pRequestVgpu->swizzId, &bHeterogeneousModeEnabled));

    if (!bHeterogeneousModeEnabled &&
        (kvgpumgrCheckHomogeneousPlacementSupported(pGpu, pRequestVgpu->swizzId) != NV_OK))
    {
        pRequestVgpu->placementId = NVA081_PLACEMENT_ID_INVALID;
        return NV_ERR_NOT_SUPPORTED;
    }

    // Allocate either the first available placementId or the placementId specified by the user.
    for (i = 0; i < pPgpuInfo->numVgpuTypes; i++)
    {
        pVgpuTypeInfo = pPgpuInfo->vgpuTypes[i];
        if (pVgpuTypeInfo == NULL)
            break;
 
        pVgpuTypeSupportedPlacementInfo = &pVgpuTypeInfo->vgpuTypeSupportedPlacementInfo;
        pVgpuTypePlacementInfo = &pKernelVgpuTypePlacementInfo->vgpuTypePlacementInfo[i];
 
        if (pVgpuTypeInfo->vgpuTypeId == pRequestVgpu->vgpuTypeId)
        {
            placementCount = (bHeterogeneousModeEnabled) ?
                            pVgpuTypeSupportedPlacementInfo->heterogeneousPlacementCount :
                            pVgpuTypeSupportedPlacementInfo->homogeneousPlacementCount;

            for (j = 0; j < placementCount; j++)
            {
                pVgpuInstancePlacementInfo = &pVgpuTypePlacementInfo->vgpuInstancePlacementInfo[j];

                if (placementId == NVA081_PLACEMENT_ID_INVALID)
                {
                    if (pVgpuInstancePlacementInfo->creatablePlacementId != NVA081_PLACEMENT_ID_INVALID)
                    {
                        assignedPlacementId = pVgpuInstancePlacementInfo->creatablePlacementId;
                        bIdFound = NV_TRUE;
                        break;
                    }
                }
                else
                {
                    if (pVgpuInstancePlacementInfo->creatablePlacementId == placementId)
                    {
                        assignedPlacementId = placementId;
                        bIdFound = NV_TRUE;
                        break;
                    }
                }
            }
        }
    }

    // No creatable placement ID found
    if (!bIdFound)
        return NV_ERR_INVALID_STATE;

    // Reserve the new placementId then free up the pre-existing placementId if there is any
    if (bHeterogeneousModeEnabled)
    {
        rmStatus = _kvgpumgrUpdateHeterogeneousCreatablePlacementIds(pGpu,
                                    assignedPlacementId, pRequestVgpu->vgpuTypeId, NV_TRUE, pRequestVgpu->swizzId);
        if (rmStatus != NV_OK)
            return rmStatus;

        if (placementId != NVA081_PLACEMENT_ID_INVALID)
        {
            rmStatus = _kvgpumgrUpdateHeterogeneousCreatablePlacementIds(pGpu,
                                         pRequestVgpu->placementId,
                                         pRequestVgpu->vgpuTypeId, NV_FALSE,
                                         pRequestVgpu->swizzId);
        }
    }
    else
    {
        rmStatus = _kvgpumgrUpdateHomogeneousCreatablePlacementIds(pGpu,
                                    assignedPlacementId, pRequestVgpu->vgpuTypeId, NV_TRUE, pRequestVgpu->swizzId);
        if (rmStatus != NV_OK)
            return rmStatus;

        if (placementId != NVA081_PLACEMENT_ID_INVALID)
        {
            rmStatus = _kvgpumgrUpdateHomogeneousCreatablePlacementIds(pGpu,
                                         pRequestVgpu->placementId,
                                         pRequestVgpu->vgpuTypeId, NV_FALSE,
                                         pRequestVgpu->swizzId);
        }
    }

    if (rmStatus != NV_OK)
        return rmStatus;

    pRequestVgpu->placementId = assignedPlacementId;

    return rmStatus;
}

NV_STATUS
kvgpumgrSetGpuInstanceId(REQUEST_VGPU_INFO_NODE *pRequestVgpu, NvU32 swizzId)
{
    OBJSYS                  *pSys                   = SYS_GET_INSTANCE();
    OBJGPU                  *pGpu                   = NULL;
    KernelMIGManager        *pKernelMIGManager      = NULL;
    KernelVgpuMgr           *pKernelVgpuMgr         = SYS_GET_KERNEL_VGPUMGR(pSys);
    KERNEL_PHYS_GPU_INFO    *pPhysGpuInfo           = NULL;
    NvU64                    swizzIdInUseMask       = 0;
    NV_STATUS                rmStatus               = NV_OK;
    VGPU_TYPE               *existingVgpuTypeInfo   = NULL;
    VGPU_TYPE               *vgpuTypeInfo           = NULL;
    NvU32                    partitionFlag          = PARTITIONID_INVALID;
    REQUEST_VGPU_INFO_NODE  *pRequestVgpuTemp       = NULL;
    NvU32                    pgpuIndex;

    if (pRequestVgpu->swizzId == swizzId)
        return NV_OK;

    pGpu = gpumgrGetGpuFromId(pRequestVgpu->gpuPciId);
    if (pGpu == NULL)
        return NV_ERR_NOT_SUPPORTED;

    pKernelMIGManager  = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    NV_CHECK_OR_RETURN(LEVEL_INFO, IS_MIG_ENABLED(pGpu), NV_ERR_INVALID_OPERATION);
    NV_CHECK_OR_RETURN(LEVEL_INFO, IS_MIG_IN_USE(pGpu), NV_ERR_INVALID_OPERATION);

    if ((rmStatus = kvgpumgrGetPgpuIndex(pKernelVgpuMgr, pRequestVgpu->gpuPciId, &pgpuIndex)) != NV_OK)
        return rmStatus;

    pPhysGpuInfo = &(pKernelVgpuMgr->pgpuInfo[pgpuIndex]);

    if ((rmStatus = kvgpumgrGetVgpuTypeInfo(pRequestVgpu->vgpuTypeId, &vgpuTypeInfo)) != NV_OK)
        return rmStatus;

    NV_ASSERT_OK_OR_RETURN(kvgpumgrGetPartitionFlag(vgpuTypeInfo->vgpuTypeId, &partitionFlag));

    swizzIdInUseMask = kmigmgrGetSwizzIdInUseMask(pGpu, pKernelMIGManager);

    // Determine valid swizzids not assigned to any vGPU device.
    if (NVBIT64(swizzId) & swizzIdInUseMask)
    {
        KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance;

        rmStatus = kmigmgrGetGPUInstanceInfo(pGpu, pKernelMIGManager, swizzId, &pKernelMIGGpuInstance);
        if (rmStatus != NV_OK)
        {
            // Didn't find requested GPU instance
            NV_PRINTF(LEVEL_ERROR, "No valid GPU instance with SwizzId - %d found\n", swizzId);
            return rmStatus;
        }

        if (pKernelMIGGpuInstance->partitionFlag == partitionFlag)
        {
            if (pPhysGpuInfo->assignedSwizzIdVgpuCount[swizzId] > 0)
            {
                for (pRequestVgpuTemp = listHead(&pKernelVgpuMgr->listRequestVgpuHead);
                     pRequestVgpuTemp != NULL;
                     pRequestVgpuTemp = listNext(&pKernelVgpuMgr->listRequestVgpuHead, pRequestVgpuTemp))
                {
                    /* Check for an existing Vgpu on the same physical gpu and same GI  */
                    if ((pRequestVgpuTemp->gpuPciId == pGpu->gpuId) && (pRequestVgpuTemp->swizzId == swizzId))
                    {
                        NV_ASSERT_OK_OR_RETURN(kvgpumgrGetVgpuTypeInfo(pRequestVgpuTemp->vgpuTypeId,
                                               &existingVgpuTypeInfo));
                        if (kvgpumgrIsHeterogeneousVgpuTypeSupported() == NV_TRUE)
                        {
                            if (existingVgpuTypeInfo->profileSize != vgpuTypeInfo->profileSize)
                                goto error_exit;
                        }
                        else
                        {
                            if (existingVgpuTypeInfo->vgpuTypeId != vgpuTypeInfo->vgpuTypeId)
                                goto error_exit;
                        }
                    }
                }
            }

            // Validate that same ID is not already set and then set the ID
            if (pPhysGpuInfo->assignedSwizzIdVgpuCount[swizzId] < vgpuTypeInfo->maxInstancePerGI )
            {
                // Clear the assigned swizzId
                NV_ASSERT_OK_OR_RETURN(_kvgpumgrClearAssignedSwizzIdMask(pGpu, pRequestVgpu->swizzId));

                // Assign the swizzId to the vGPU device.
                NV_ASSERT_OK_OR_RETURN(_kvgpumgrSetAssignedSwizzIdMask(pGpu, vgpuTypeInfo,
                                        pKernelMIGGpuInstance->swizzId));
            }
            else
                goto error_exit;
        }
        else
            goto error_exit;

        // Assign swizzId to the REQUEST_VGPU_INFO_NODE
        pRequestVgpu->swizzId = pKernelMIGGpuInstance->swizzId;

        return NV_OK;
    }

error_exit:
    return NV_ERR_INVALID_STATE;
}

NV_STATUS
kvgpuMgrGetPlacementResource(OBJGPU *pGpu, Device *pDevice,
                             const NvU8 *pVgpuDevName, NvU16 *placementId,
                             NvU64 *guestFbLength, NvU64 *guestFbOffset,
                             NvU64 *gspHeapOffset, NvU64 *guestBar1PFOffset)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    KernelVgpuMgr *pKernelVgpuMgr = SYS_GET_KERNEL_VGPUMGR(pSys);
    REQUEST_VGPU_INFO_NODE *pRequestVgpu = NULL;
    NvU32 index, i , j;
    KERNEL_PHYS_GPU_INFO *pPgpuInfo;
    VGPU_TYPE *pVgpuTypeInfo;
    KERNEL_VGPU_TYPE_PLACEMENT_INFO *pKernelVgpuTypePlacementInfo;
    VGPU_INSTANCE_PLACEMENT_INFO *pVgpuInstancePlacementInfo;
    VGPU_TYPE_PLACEMENT_INFO *pVgpuTypePlacementInfo;
    VGPU_TYPE_SUPPORTED_PLACEMENT_INFO *pVgpuTypeSupportedPlacementInfo;
    NvBool bHeterogeneousModeEnabled;

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, kvgpumgrGetPgpuIndex(pKernelVgpuMgr, pGpu->gpuId, &index));

    pPgpuInfo = &(pKernelVgpuMgr->pgpuInfo[index]);
    if (IS_MIG_IN_USE(pGpu) && kvgpumgrIsMigTimeslicingModeEnabled(pGpu))
    {
        NvU32 swizzId = kvgpuMgrGetSwizzIdFromDevice(pGpu, pDevice);
        pKernelVgpuTypePlacementInfo = kvgpuMgrGetVgpuPlacementInfo(pGpu, pPgpuInfo, swizzId);
        NV_CHECK_OR_RETURN(LEVEL_ERROR, pKernelVgpuTypePlacementInfo != NULL, NV_ERR_OBJECT_NOT_FOUND);
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            kvgpuMgrGetHeterogeneousModePerGI(pGpu, swizzId, &bHeterogeneousModeEnabled));
    }
    else
    {
        pKernelVgpuTypePlacementInfo = &pPgpuInfo->kernelVgpuTypePlacementInfo;
        bHeterogeneousModeEnabled = pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_VGPU_HETEROGENEOUS_MODE);
    }
    // Check the corresponding REQUEST_VGPU_INFO_NODE
    for (pRequestVgpu = listHead(&pKernelVgpuMgr->listRequestVgpuHead);
         pRequestVgpu != NULL;
         pRequestVgpu = listNext(&pKernelVgpuMgr->listRequestVgpuHead, pRequestVgpu))
    {
        if (portMemCmp(pRequestVgpu->vgpuDevName, pVgpuDevName, VM_UUID_SIZE) == 0)
            break;
    }

    if (pRequestVgpu == NULL)
        return NV_ERR_OBJECT_NOT_FOUND;

    // The placement resources corresponding to the placementId are obtained by iteration over vgpuTypes
    for (i = 0; i < pPgpuInfo->numVgpuTypes; i++)
    {
        pVgpuTypeInfo = pPgpuInfo->vgpuTypes[i];
        if (pVgpuTypeInfo == NULL)
            continue;

        pVgpuTypeSupportedPlacementInfo = &pVgpuTypeInfo->vgpuTypeSupportedPlacementInfo;
        pVgpuTypePlacementInfo = &pKernelVgpuTypePlacementInfo->vgpuTypePlacementInfo[i];

        if (pVgpuTypeInfo->vgpuTypeId != pRequestVgpu->vgpuTypeId)
            continue;

        if (pVgpuTypeSupportedPlacementInfo->placementSize == 0)
            continue;

        if (bHeterogeneousModeEnabled)
        {
            for (j = 0; j < pVgpuTypeSupportedPlacementInfo->heterogeneousPlacementCount; j++)
            {
                if (pVgpuTypeSupportedPlacementInfo->vgpuInstanceSupportedPlacementInfo[j].heterogeneousSupportedPlacementId ==
                    pRequestVgpu->placementId)
                {
                    pVgpuInstancePlacementInfo = &pVgpuTypePlacementInfo->vgpuInstancePlacementInfo[j];
                    *placementId        = pRequestVgpu->placementId;
                    if (gpuIsSriovEnabled(pGpu))
                    {
                        *guestFbOffset = pVgpuInstancePlacementInfo->heterogeneousSupportedVmmuOffset *
                                         gpuGetVmmuSegmentSize(pGpu);
                        *guestFbLength = pVgpuTypePlacementInfo->guestVmmuCount *
                                         gpuGetVmmuSegmentSize(pGpu);
                        *gspHeapOffset = pVgpuInstancePlacementInfo->heterogeneousGspHeapOffset;
                        *guestBar1PFOffset    = 0;
                    }

                    return NV_OK;
                }
            }
        }
        else
        {
            for (j = 0; j < pVgpuTypeSupportedPlacementInfo->homogeneousPlacementCount; j++)
            {
                if (pVgpuTypeSupportedPlacementInfo->vgpuInstanceSupportedPlacementInfo[j].homogeneousSupportedPlacementId ==
                    pRequestVgpu->placementId)
                {
                     pVgpuInstancePlacementInfo = &pVgpuTypePlacementInfo->vgpuInstancePlacementInfo[j];
                    *placementId        = pRequestVgpu->placementId;
                    *guestFbOffset      = pVgpuInstancePlacementInfo->homogeneousSupportedVmmuOffset *
                                                gpuGetVmmuSegmentSize(pGpu);
                    *guestFbLength      = pVgpuTypePlacementInfo->guestVmmuCount *
                                                gpuGetVmmuSegmentSize(pGpu);
                    *gspHeapOffset      = 0;
                    *guestBar1PFOffset  = 0;

                    return NV_OK;
                }
            }
        }
    }

    return NV_ERR_INVALID_STATE;
}
NV_STATUS
kvgpuMgrGetHeterogeneousMode(OBJGPU *pGpu, NvU32 swizzId, NvBool *bHeterogeneousModeEnabled)
{
    if (IS_MIG_IN_USE(pGpu) && kvgpumgrIsMigTimeslicingModeEnabled(pGpu))
    {
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            kvgpuMgrGetHeterogeneousModePerGI(pGpu, swizzId, bHeterogeneousModeEnabled));
    }
    else
    {
        *bHeterogeneousModeEnabled = pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_VGPU_HETEROGENEOUS_MODE);
    }
    return NV_OK;
}

KERNEL_VGPU_TYPE_PLACEMENT_INFO*
kvgpuMgrGetVgpuPlacementInfo
(
    OBJGPU *pGpu,
    KERNEL_PHYS_GPU_INFO *pPgpuInfo,
    NvU32 swizzId
)
{
    KERNEL_VGPU_TYPE_PLACEMENT_INFO *pKernelVgpuTypePlacementInfo = NULL;

    if (IS_MIG_IN_USE(pGpu) && kvgpumgrIsMigTimeslicingModeEnabled(pGpu))
    {
        for (NvU32 i = 0; i < MAX_GI_PARTITIONS_PER_GPU; i++)
        {
            if (pPgpuInfo->kernelVgpuTypePlacementInfoGi[i].swizzId == swizzId &&
                pPgpuInfo->kernelVgpuTypePlacementInfoGi[i].bUsed)
            {
                pKernelVgpuTypePlacementInfo = &pPgpuInfo->kernelVgpuTypePlacementInfoGi[i].kernelVgpuTypePlacementInfo;
                break;
            }
        }
    }
    else
    {
        pKernelVgpuTypePlacementInfo = &pPgpuInfo->kernelVgpuTypePlacementInfo;
    }
    return pKernelVgpuTypePlacementInfo;
}

NvU32*
kvgpuMgrGetVgpuCreatableTypeIdFromSwizzId
(
    OBJGPU *pGpu,
    KERNEL_PHYS_GPU_INFO *pPgpuInfo,
    NvU32 swizzId
)
{
    NvU32 *pSupportedTypeId = NULL;

    if (IS_MIG_IN_USE(pGpu) && kvgpumgrIsMigTimeslicingModeEnabled(pGpu))
    {
        for (NvU32 i = 0; i < MAX_GI_PARTITIONS_PER_GPU; i++)
        {
            if (pPgpuInfo->kernelVgpuTypePlacementInfoGi[i].swizzId == swizzId &&
                pPgpuInfo->kernelVgpuTypePlacementInfoGi[i].bUsed)
            {
                pSupportedTypeId = pPgpuInfo->kernelVgpuTypePlacementInfoGi[i].supportedTypeIds;
                break;
            }
        }
    }
    else
    {
        pSupportedTypeId = pPgpuInfo->supportedTypeIds;
    }
    return pSupportedTypeId;
}

NvU32
kvgpuMgrGetSwizzIdFromDevice
(
    OBJGPU *pGpu,
    Device *pDevice
)
{
    NvU32 swizzId = KMIGMGR_SWIZZID_INVALID;

    if (IS_MIG_IN_USE(pGpu))
    {
        KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
        MIG_INSTANCE_REF ref;

        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, kmigmgrGetInstanceRefFromDevice(pGpu, pKernelMIGManager, pDevice, &ref));
        swizzId = ref.pKernelMIGGpuInstance->swizzId;
    }

    return swizzId;
}

NV_STATUS
kvgpuMgrReserveVgpuPlacementInfoPerGI
(
    OBJGPU  *pGpu,
    NvU32    swizzId
)
{
    OBJSYS     *pSys     = SYS_GET_INSTANCE();
    KernelVgpuMgr *pKernelVgpuMgr = SYS_GET_KERNEL_VGPUMGR(pSys);
    KERNEL_PHYS_GPU_INFO *pPgpuInfo;
    NvU32 index, i;

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            kvgpumgrGetPgpuIndex(pKernelVgpuMgr, pGpu->gpuId, &index));

    pPgpuInfo = &(pKernelVgpuMgr->pgpuInfo[index]);

    for (i = 0; i < MAX_GI_PARTITIONS_PER_GPU; i++)
    {
        if (!pPgpuInfo->kernelVgpuTypePlacementInfoGi[i].bUsed)
        {
            pPgpuInfo->kernelVgpuTypePlacementInfoGi[i].swizzId = swizzId;
            pPgpuInfo->kernelVgpuTypePlacementInfoGi[i].bUsed = NV_TRUE;
            break;
        }
    }
    if (i == MAX_GI_PARTITIONS_PER_GPU)
    {
        return NV_ERR_OUT_OF_RANGE;
    }

    if (pPgpuInfo->heterogeneousTimesliceSizesSupported)
    {
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            _kvgpuMgrSetHeterogeneousResourcesPerGI(pGpu, swizzId, pPgpuInfo));
    }
    if (pPgpuInfo->homogeneousPlacementSupported)
    {
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            _kvgpumgrSetHomogeneousResourcesPerGI(pGpu, swizzId, pPgpuInfo));
    }
    return NV_OK;
}

NV_STATUS
kvgpuMgrClearVgpuPlacementInfoPerGI
(
    OBJGPU    *pGpu,
    NvU32      swizzId
)
{
    OBJSYS     *pSys     = SYS_GET_INSTANCE();
    KernelVgpuMgr *pKernelVgpuMgr = SYS_GET_KERNEL_VGPUMGR(pSys);
    KERNEL_PHYS_GPU_INFO *pPgpuInfo;
    NvU32 index, i;

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            kvgpumgrGetPgpuIndex(pKernelVgpuMgr, pGpu->gpuId, &index));

    pPgpuInfo = &(pKernelVgpuMgr->pgpuInfo[index]);

    for (i = 0; i < MAX_GI_PARTITIONS_PER_GPU; i++)
    {
        if (pPgpuInfo->kernelVgpuTypePlacementInfoGi[i].swizzId == swizzId &&
            pPgpuInfo->kernelVgpuTypePlacementInfoGi[i].bUsed)
        {
            portMemSet(&pPgpuInfo->kernelVgpuTypePlacementInfoGi[i], 0,
                        sizeof(pPgpuInfo->kernelVgpuTypePlacementInfoGi[i]));
            break;
        }
    }
    if (i == MAX_GI_PARTITIONS_PER_GPU)
    {
        return NV_ERR_OUT_OF_RANGE;
    }

    return NV_OK;
}

NV_STATUS
kvgpuMgrSetHeterogeneousModePerGI
(
    OBJGPU    *pGpu,
    NvU32      swizzId,
    NvBool     bHeterogeneousModeEnabled
)
{
    OBJSYS     *pSys     = SYS_GET_INSTANCE();
    KernelVgpuMgr *pKernelVgpuMgr = SYS_GET_KERNEL_VGPUMGR(pSys);
    KERNEL_PHYS_GPU_INFO *pPgpuInfo;
    NvU32 index, i;

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            kvgpumgrGetPgpuIndex(pKernelVgpuMgr, pGpu->gpuId, &index));

    pPgpuInfo = &(pKernelVgpuMgr->pgpuInfo[index]);

    for (i = 0; i < MAX_GI_PARTITIONS_PER_GPU; i++)
    {
        if (pPgpuInfo->kernelVgpuTypePlacementInfoGi[i].swizzId == swizzId &&
            pPgpuInfo->kernelVgpuTypePlacementInfoGi[i].bUsed)
        {
            pPgpuInfo->kernelVgpuTypePlacementInfoGi[i].bHeterogeneousModeEnabled = bHeterogeneousModeEnabled;
            break;
        }
    }
    if (i == MAX_GI_PARTITIONS_PER_GPU)
    {
        return NV_ERR_OUT_OF_RANGE;
    }

    return NV_OK;
}

NV_STATUS
kvgpuMgrGetHeterogeneousModePerGI
(
    OBJGPU    *pGpu,
    NvU32      swizzId,
    NvBool     *bHeterogeneousModeEnabled
)
{
    OBJSYS     *pSys     = SYS_GET_INSTANCE();
    KernelVgpuMgr *pKernelVgpuMgr = SYS_GET_KERNEL_VGPUMGR(pSys);
    KERNEL_PHYS_GPU_INFO *pPgpuInfo;
    NvU32 index, i;

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            kvgpumgrGetPgpuIndex(pKernelVgpuMgr, pGpu->gpuId, &index));

    pPgpuInfo = &(pKernelVgpuMgr->pgpuInfo[index]);

    for (i = 0; i < MAX_GI_PARTITIONS_PER_GPU; i++)
    {
        if (pPgpuInfo->kernelVgpuTypePlacementInfoGi[i].swizzId == swizzId &&
            pPgpuInfo->kernelVgpuTypePlacementInfoGi[i].bUsed)
        {
            *bHeterogeneousModeEnabled = pPgpuInfo->kernelVgpuTypePlacementInfoGi[i].bHeterogeneousModeEnabled;
            break;
        }
    }
    if (i == MAX_GI_PARTITIONS_PER_GPU)
    {
        return NV_ERR_OUT_OF_RANGE;
    }

    return NV_OK;
}

static NV_STATUS
_kvgpuMgrSetHeterogeneousResourcesPerGI(OBJGPU *pGpu, NvU32 swizzId, KERNEL_PHYS_GPU_INFO *pPgpuInfo)
{
    OBJSYS     *pSys     = SYS_GET_INSTANCE();
    KernelVgpuMgr *pKernelVgpuMgr = SYS_GET_KERNEL_VGPUMGR(pSys);
    KERNEL_VGPU_TYPE_PLACEMENT_INFO *pKernelVgpuTypePlacementInfo = NULL, *pDefaultKernelVgpuTypePlacementInfo;
    NvU32 index, i, j, guestMaxVmmuCount, totalVmmuCount = 0, partitionFlag, numCreatable = 0;
    NvU64 vmmuOffsetMin, vmmuSegmentMin, vmmuSegSize,
          maxTotalGspHeapSize, gspHeapStartOffset, gspHeapOffset,
          pmaRegionLength, pmaBaseOffset, largestOffset;
    VGPU_TYPE *pVgpuTypeInfo;
    VGPU_TYPE_PLACEMENT_INFO *pVgpuTypePlacementInfo, *pDefaultVgpuTypePlacementInfo;
    VGPU_TYPE_SUPPORTED_PLACEMENT_INFO *pVgpuTypeSupportedPlacementInfo;
    VGPU_INSTANCE_PLACEMENT_INFO *pVgpuInstancePlacementInfo;
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance;
    Heap *pMemoryPartitionHeap  = NULL;
    Device *pDevice;
    RsClient *pClient;
    NvU32 *pSupportedTypeId;
    NvBool bFoundOne2OneProfile = 0;

    NV_CHECK_OR_RETURN(LEVEL_ERROR, pPgpuInfo != NULL, NV_ERR_INVALID_ARGUMENT);

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            kvgpumgrGetPgpuIndex(pKernelVgpuMgr, pGpu->gpuId, &index));
    pPgpuInfo = &(pKernelVgpuMgr->pgpuInfo[index]);

    if (!gpuIsSriovEnabled(pGpu) || !IS_MIG_IN_USE(pGpu))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    NV_ASSERT_OK_OR_RETURN(kmigmgrGetGPUInstanceInfo(pGpu, pKernelMIGManager, swizzId, &pKernelMIGGpuInstance));

    /*
     * Get the heap for the MIG partition and find memory range
     * vmmuOffsetMin = start of the GI memory aligned to VMMU segment size
     */
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                    serverGetClientUnderLock(&g_resServ, pKernelMIGGpuInstance->instanceHandles.hClient, &pClient));

    NV_CHECK_OR_RETURN(LEVEL_ERROR,
                      (deviceGetByHandle(pClient, pKernelMIGGpuInstance->instanceHandles.hDevice, &pDevice) == NV_OK),
                      NV_ERR_INSUFFICIENT_PERMISSIONS);

    NV_CHECK_OR_RETURN(LEVEL_ERROR, (kmigmgrGetMemoryPartitionHeapFromDevice(pGpu,
                                       pKernelMIGManager, pDevice, &pMemoryPartitionHeap) == NV_OK),
                                       NV_ERR_INSUFFICIENT_PERMISSIONS);
    pmaGetLargestFree(&pMemoryPartitionHeap->pmaObject, &pmaRegionLength, &pmaBaseOffset, &largestOffset);
    vmmuOffsetMin = NV_ALIGN_UP(pmaBaseOffset, gpuGetVmmuSegmentSize(pGpu));
    vmmuSegmentMin = vmmuOffsetMin / gpuGetVmmuSegmentSize(pGpu);
    vmmuSegSize = gpuGetVmmuSegmentSize(pGpu);

    maxTotalGspHeapSize = _kvgpumgrGetMaxTotalGspHeapSize(pGpu, pPgpuInfo);

    // Find GI placement info
    for (i = 0; i < MAX_GI_PARTITIONS_PER_GPU; i++)
    {
        if (pPgpuInfo->kernelVgpuTypePlacementInfoGi[i].bUsed &&
            pPgpuInfo->kernelVgpuTypePlacementInfoGi[i].swizzId == swizzId)
        {
            pKernelVgpuTypePlacementInfo =
                &pPgpuInfo->kernelVgpuTypePlacementInfoGi[i].kernelVgpuTypePlacementInfo;
            break;
        }
    }
    NV_ASSERT_OR_RETURN(pKernelVgpuTypePlacementInfo != NULL, NV_ERR_OBJECT_NOT_FOUND);

    // Set initial creatable vgpu types for this GI
    pSupportedTypeId = kvgpuMgrGetVgpuCreatableTypeIdFromSwizzId(pGpu, pPgpuInfo, swizzId);

    pDefaultKernelVgpuTypePlacementInfo = &pPgpuInfo->kernelVgpuTypePlacementInfo;

    // Find the 1:1 profile of the current GI, calculate totalVmmuCount
    for (i = 0; i < pPgpuInfo->numVgpuTypes; i++)
    {
        pVgpuTypeInfo = pPgpuInfo->vgpuTypes[i];
        if (pVgpuTypeInfo == NULL)
            break;

        // Ignore non-MIG vgpu types
        if (pVgpuTypeInfo->gpuInstanceSize == 0)
            continue;

        kvgpumgrGetPartitionFlag(pVgpuTypeInfo->vgpuTypeId, &partitionFlag);

        // Ignore vgpu types that does not belong to the current GI
        if (partitionFlag != pKernelMIGGpuInstance->partitionFlag)
            continue;

        // Set initial creatable vgpu types for this GI
        pSupportedTypeId[numCreatable++] = pVgpuTypeInfo->vgpuTypeId;

        // Use 1:1 profile to get the totalVmmuCount
        if (!bFoundOne2OneProfile && pVgpuTypeInfo->maxInstancePerGI == 1)
        {
            /*
             * Global(Default) placement info will be aligned to the per GI placement info index
             * use its guestVmmuCount calculated from kvgpumgrSetSupportedPlacementIds, per GI placement info
             * is calcualted in the next loop
             */
            pDefaultVgpuTypePlacementInfo = &pDefaultKernelVgpuTypePlacementInfo->vgpuTypePlacementInfo[i];
            totalVmmuCount = pDefaultVgpuTypePlacementInfo->guestVmmuCount
                           + pVgpuTypeInfo->gspHeapSize / gpuGetVmmuSegmentSize(pGpu);
            bFoundOne2OneProfile = NV_TRUE;
        }
    }
    NV_ASSERT_OR_RETURN(bFoundOne2OneProfile, NV_ERR_INVALID_ARGUMENT);

    guestMaxVmmuCount = totalVmmuCount - ((maxTotalGspHeapSize + vmmuSegSize - 1) / vmmuSegSize);
    gspHeapStartOffset = (vmmuSegmentMin + guestMaxVmmuCount) * vmmuSegSize;

    // Iterate vgpu types belongs to current GI, then calculate placement resources
    for (i = 0; i < pPgpuInfo->numVgpuTypes; i++)
    {
        pVgpuTypeInfo = pPgpuInfo->vgpuTypes[i];
        if (pVgpuTypeInfo == NULL)
            break;

        // Ignore non-MIG vgpu types
        if (pVgpuTypeInfo->gpuInstanceSize == 0)
            continue;

        kvgpumgrGetPartitionFlag(pVgpuTypeInfo->vgpuTypeId, &partitionFlag);

        // Ignore vgpu types that does not belong to the current GI
        if (partitionFlag != pKernelMIGGpuInstance->partitionFlag)
            continue;

        pVgpuTypeSupportedPlacementInfo = &pVgpuTypeInfo->vgpuTypeSupportedPlacementInfo;
        pVgpuTypePlacementInfo = &pKernelVgpuTypePlacementInfo->vgpuTypePlacementInfo[i];
        pDefaultVgpuTypePlacementInfo = &pDefaultKernelVgpuTypePlacementInfo->vgpuTypePlacementInfo[i];

        // Copy from Global(default) placement info calculated in kvgpumgrSetSupportedPlacementIds
        pVgpuTypePlacementInfo->guestVmmuCount = pDefaultVgpuTypePlacementInfo->guestVmmuCount;

        for (j = 0; j < pVgpuTypeSupportedPlacementInfo->heterogeneousPlacementCount; j++)
        {
            pVgpuInstancePlacementInfo = &pVgpuTypePlacementInfo->vgpuInstancePlacementInfo[j];
            if (pVgpuTypeInfo->maxInstancePerGI == 1)
            {
                pVgpuInstancePlacementInfo->heterogeneousSupportedVmmuOffset = vmmuSegmentMin;
                pVgpuInstancePlacementInfo->heterogeneousGspHeapOffset =
                            (vmmuSegmentMin + pVgpuTypePlacementInfo->guestVmmuCount) * vmmuSegSize;
            }
            else
            {
                pVgpuInstancePlacementInfo->heterogeneousSupportedVmmuOffset =
                            vmmuSegmentMin + ((guestMaxVmmuCount * j) / pVgpuTypeInfo->maxInstancePerGI);
                gspHeapOffset = (maxTotalGspHeapSize * j) / pVgpuTypeInfo->maxInstancePerGI;
                pVgpuInstancePlacementInfo->heterogeneousGspHeapOffset =
                            gspHeapStartOffset + NV_ALIGN_DOWN(gspHeapOffset, RM_PAGE_SIZE_HUGE);
            }
        }
    }
    return NV_OK;
}

static NV_STATUS
_kvgpumgrSetHomogeneousResourcesPerGI(OBJGPU *pGpu, NvU32 swizzId, KERNEL_PHYS_GPU_INFO *pPgpuInfo)
{
    KERNEL_VGPU_TYPE_PLACEMENT_INFO *pKernelVgpuTypePlacementInfo = NULL;
    VGPU_TYPE_SUPPORTED_PLACEMENT_INFO *pVgpuTypeSupportedPlacementInfo;
    VGPU_TYPE_PLACEMENT_INFO *pVgpuTypePlacementInfo;
    VGPU_TYPE *pVgpuTypeInfo;
    NvU32 i, j, partitionFlag;
    VGPU_INSTANCE_PLACEMENT_INFO *pVgpuInstancePlacementInfo;
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance;
    VGPU_INSTANCE_SUPPORTED_PLACEMENT_INFO *pVgpuInstanceSupportedPlacementInfo;

    NV_CHECK_OR_RETURN(LEVEL_ERROR, pPgpuInfo != NULL, NV_ERR_INVALID_ARGUMENT);

    NV_ASSERT_OK_OR_RETURN(kmigmgrGetGPUInstanceInfo(pGpu, pKernelMIGManager, swizzId, &pKernelMIGGpuInstance));

    if (!pPgpuInfo->heterogeneousTimesliceSizesSupported)
    {
        pPgpuInfo->homogeneousPlacementSupported = NV_FALSE;
        NV_CHECK_OR_RETURN(LEVEL_ERROR, pPgpuInfo->homogeneousPlacementSupported, NV_ERR_NOT_SUPPORTED);
    }

    for (i = 0; i < MAX_GI_PARTITIONS_PER_GPU; i++)
    {
        if (pPgpuInfo->kernelVgpuTypePlacementInfoGi[i].bUsed &&
            pPgpuInfo->kernelVgpuTypePlacementInfoGi[i].swizzId == swizzId)
        {
            pKernelVgpuTypePlacementInfo =
                &pPgpuInfo->kernelVgpuTypePlacementInfoGi[i].kernelVgpuTypePlacementInfo;
            break;
        }
    }
    NV_ASSERT_OR_RETURN(pKernelVgpuTypePlacementInfo != NULL, NV_ERR_OBJECT_NOT_FOUND);

    for (i = 0; i < pPgpuInfo->numVgpuTypes; i++)
    {
        pVgpuTypeInfo = pPgpuInfo->vgpuTypes[i];
        if (pVgpuTypeInfo == NULL)
            break;

        // Ignore non-MIG vgpu types
        if (pVgpuTypeInfo->gpuInstanceSize == 0)
            continue;

        kvgpumgrGetPartitionFlag(pVgpuTypeInfo->vgpuTypeId, &partitionFlag);

        // Ignore vgpu types that does not belong to the current GI
        if (partitionFlag != pKernelMIGGpuInstance->partitionFlag)
            continue;

        pVgpuTypeSupportedPlacementInfo = &pVgpuTypeInfo->vgpuTypeSupportedPlacementInfo;
        pVgpuTypePlacementInfo = &pKernelVgpuTypePlacementInfo->vgpuTypePlacementInfo[i];

        /*
         * For GB20x on MIG, heterogeneous and homogeneous placement count is the same, hence copy it over using
         * heterogenous placement count
         */
        for (j = 0; j < pVgpuTypeSupportedPlacementInfo->heterogeneousPlacementCount; j++)
        {
            pVgpuInstancePlacementInfo = &pVgpuTypePlacementInfo->vgpuInstancePlacementInfo[j];
            pVgpuInstanceSupportedPlacementInfo =
                &pVgpuTypeSupportedPlacementInfo->vgpuInstanceSupportedPlacementInfo[j];
            // Copy from heterogeneous vmmu offset
            pVgpuInstancePlacementInfo->homogeneousSupportedVmmuOffset =
                                pVgpuInstancePlacementInfo->heterogeneousSupportedVmmuOffset;
            /*
             * Initially, all supported placements are creatable
             * Copy from homogeneous placement count, homogeneous and heterogeneous placements are same
             */
            pVgpuInstancePlacementInfo->creatablePlacementId =
                pVgpuInstanceSupportedPlacementInfo->homogeneousSupportedPlacementId;
        }
    }
    return NV_OK;
}

