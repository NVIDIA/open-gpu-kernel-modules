/*
 * SPDX-FileCopyrightText: Copyright (c) 2006-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include <class/cl0080.h>
#include <class/cl00fc.h>      // FABRIC_VASPACE_A
#include "rmapi/rs_utils.h"
#include "gpu/mmu/kern_gmmu.h"
#include "gpu/device/device.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "gpu/bus/p2p_api.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"
#include "kernel/gpu/nvlink/kernel_nvlink.h"
#include "rmapi/rmapi.h"
#include "core/locks.h"
#include "vgpu/rpc.h"
#include "virtualization/hypervisor/hypervisor.h"
#include "mem_mgr/vaspace.h"
#include "mem_mgr/fabric_vaspace.h"
#include "mem_mgr/virt_mem_mgr.h"
#include <os/os.h>
#include "platform/sli/sli.h"

#include "published/ampere/ga100/dev_nv_xve.h"
#include "published/ampere/ga100/dev_ram.h"  // NV_RAMIN_ALLOC_SIZE
#include "ctrl/ctrl2080/ctrl2080fla.h" // NV2080_CTRL_CMD_FLA_SETUP_INSTANCE_MEM_BLOCK

#define NVLNK_FABRIC_ADDR_GRANULARITY                                      36

/*!
 * @brief Sets up the Legacy FLA state for the GPU. This function will allocate a RM
 * client, which will allocate the FERMI_VASPACE_A object with LEGACY_FLA flag.
 *
 * @param[in]  base       VASpace base
 * @param[in]  size       VASpace size
 *
 * @return NV_OK if successful
 */
NV_STATUS
kbusAllocateLegacyFlaVaspace_GA100
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    NvU64      base,
    NvU64      size
)
{
    NV0080_ALLOC_PARAMETERS nv0080AllocParams = {0};
    NV2080_ALLOC_PARAMETERS nv2080AllocParams = {0};
    NV_VASPACE_ALLOCATION_PARAMETERS vaParams = {0};
    RM_API   *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    NvBool    bAcquireLock = NV_FALSE;
    RsClient *pClient;
    NV_STATUS    status = NV_OK;

    //Allocate the client in RM which owns the FLAVASpace
    status = pRmApi->AllocWithHandle(pRmApi, NV01_NULL_OBJECT, NV01_NULL_OBJECT, NV01_NULL_OBJECT,
                                     NV01_ROOT, &pKernelBus->flaInfo.hClient, sizeof(pKernelBus->flaInfo.hClient));
    NV_ASSERT_OR_RETURN(status == NV_OK, status);

    status = serverGetClientUnderLock(&g_resServ, pKernelBus->flaInfo.hClient, &pClient);
    NV_ASSERT_OR_GOTO(status == NV_OK, free_client);

    status = serverutilGenResourceHandle(pKernelBus->flaInfo.hClient, &pKernelBus->flaInfo.hDevice);
    NV_ASSERT_OR_GOTO(status == NV_OK, free_client);

    // Allocate a device handle
    nv0080AllocParams.deviceId = gpuGetDeviceInstance(pGpu);
    status = pRmApi->AllocWithHandle(pRmApi, pKernelBus->flaInfo.hClient, pKernelBus->flaInfo.hClient,
                                     pKernelBus->flaInfo.hDevice, NV01_DEVICE_0,
                                     &nv0080AllocParams, sizeof(nv0080AllocParams));

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "failed creating device, status=0x%x\n", status);
        goto free_client;
    }


    status = serverutilGenResourceHandle(pKernelBus->flaInfo.hClient, &pKernelBus->flaInfo.hSubDevice);
    NV_ASSERT_OR_GOTO(status == NV_OK, free_client);

    //Allocate a sub device handle
    nv2080AllocParams.subDeviceId = gpumgrGetSubDeviceInstanceFromGpu(pGpu);

    status = pRmApi->AllocWithHandle(pRmApi, pKernelBus->flaInfo.hClient, pKernelBus->flaInfo.hDevice,
                                     pKernelBus->flaInfo.hSubDevice, NV20_SUBDEVICE_0,
                                     &nv2080AllocParams, sizeof(nv2080AllocParams));

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "failed creating sub-device, status=0x%x\n",
                  status);
        goto free_client;
    }

    // Allocate the FERMI_VASPACE_A FLA VASpace
    vaParams.index  = NV_VASPACE_ALLOCATION_INDEX_GPU_NEW;
    vaParams.vaBase = base;
    vaParams.vaSize = size;
    vaParams.flags  |= NV_VASPACE_ALLOCATION_FLAGS_IS_FLA;

    // Generate a vaspace handle for FERMI_VASPACE_A object allocation
    status = serverutilGenResourceHandle(pKernelBus->flaInfo.hClient, &pKernelBus->flaInfo.hFlaVASpace);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "failed generating vaspace handle, status=0x%x\n", status);
        goto free_client;
    }

    if (rmDeviceGpuLockIsOwner(pGpu->gpuInstance))
    {
        rmGpuLocksRelease(GPUS_LOCK_FLAGS_NONE, NULL);
        bAcquireLock = NV_TRUE;
        pRmApi = rmapiGetInterface(RMAPI_API_LOCK_INTERNAL);
    }

    // Allocate a FERMI_VASPACE_A object and associate it with hFlaVASpace
    status = pRmApi->AllocWithHandle(pRmApi, pKernelBus->flaInfo.hClient, pKernelBus->flaInfo.hDevice,
                                     pKernelBus->flaInfo.hFlaVASpace, FERMI_VASPACE_A,
                                     &vaParams, sizeof(vaParams));
    if (bAcquireLock)
    {
        NV_ASSERT_OK_OR_CAPTURE_FIRST_ERROR(status,
                    rmGpuLocksAcquire(GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_MEM_FLA));
        bAcquireLock = NV_FALSE;
        pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    }

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "failed allocating vaspace, status=0x%x\n",
                  status);
        goto free_client;
    }

    if (!(IS_VIRTUAL(pGpu) && gpuIsWarBug200577889SriovHeavyEnabled(pGpu)))
    {
        // Get the FLA VASpace associated with hFlaVASpace
        status = vaspaceGetByHandleOrDeviceDefault(pClient,
                                                   pKernelBus->flaInfo.hDevice,
                                                   pKernelBus->flaInfo.hFlaVASpace,
                                                   &pKernelBus->flaInfo.pFlaVAS);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                    "failed getting the vaspace from handle, status=0x%x\n",
                    status);
            goto free_client;
        }
    }
    return NV_OK;

free_client:
    pRmApi->Free(pRmApi, pKernelBus->flaInfo.hClient, pKernelBus->flaInfo.hClient);
    portMemSet(&pKernelBus->flaInfo, 0, sizeof(pKernelBus->flaInfo));

    NV_PRINTF(LEVEL_ERROR, "failed allocating FLA VASpace status=0x%x\n",
              status);
    return status;
}

/*!
 * @brief Sets up the Fabric FLA state for the GPU. This function will allocate fabric VASpace,
 *        allocates PDB for both legacy and fabric VAS, allocates instance block and initialize with
 *        legacy VAS and binds the instance block to HW.
 *
 * @param[in]  base       VASpace base
 * @param[in]  size       VASpace size
 *
 * @return NV_OK if successful
 */
NV_STATUS
kbusAllocateFlaVaspace_GA100
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    NvU64      base,
    NvU64      size
)
{
    NV_STATUS    status = NV_OK;
    OBJVMM      *pVmm   = SYS_GET_VMM(SYS_GET_INSTANCE());
    KernelGmmu  *pKernelGmmu  = GPU_GET_KERNEL_GMMU(pGpu);
    INST_BLK_INIT_PARAMS pInstblkParams = {0};
    RM_API   *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    NV_ASSERT_OR_RETURN(pGpu != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(size != 0, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(!pKernelBus->flaInfo.bFlaAllocated, NV_ERR_INVALID_ARGUMENT);

    pKernelBus->flaInfo.base = base;
    pKernelBus->flaInfo.size = size;

    if (gpuIsSriovEnabled(pGpu))
    {
        KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);

        if (pKernelNvlink != NULL &&
            knvlinkIsGpuConnectedToNvswitch(pGpu, pKernelNvlink))
        {
            pKernelBus->flaInfo.bFlaRangeRegistered = NV_TRUE;
            return status;
        }
    }

    NV_ASSERT_OK_OR_RETURN(kbusAllocateLegacyFlaVaspace_HAL(pGpu, pKernelBus, base, size));

    // Allocate a FABRIC_VASPACE_A object
    status = vmmCreateVaspace(pVmm, FABRIC_VASPACE_A, pGpu->gpuId, gpumgrGetGpuMask(pGpu),
                              base, base + size - 1, 0, 0, NULL, 0,
                              &pGpu->pFabricVAS);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "failed allocating fabric vaspace, status=0x%x\n",
                  status);
        goto free_client;
    }

    //
    // For SRIOV Heavy enabled guests, VAS PTs are managed by host
    // Enabling the same path for GSP-RM offload, where VAS is managed in GSP-RM
    //
    if (IS_VIRTUAL(pGpu) && gpuIsWarBug200577889SriovHeavyEnabled(pGpu))
    {
        NV2080_CTRL_FLA_RANGE_PARAMS params = {0};
        RsClient   *pClient;
        params.mode = NV2080_CTRL_FLA_RANGE_PARAMS_MODE_HOST_MANAGED_VAS_INITIALIZE;
        params.base = base;
        params.size = size;
        params.hVASpace = pKernelBus->flaInfo.hFlaVASpace;
        NV_RM_RPC_CONTROL(pGpu, pKernelBus->flaInfo.hClient, pKernelBus->flaInfo.hSubDevice,
                          NV2080_CTRL_CMD_FLA_RANGE,
                          &params, sizeof(params), status);

        if (status != NV_OK)
        {
            goto free_client;
        }

        serverGetClientUnderLock(&g_resServ, pKernelBus->flaInfo.hClient, &pClient);

        status = vaspaceGetByHandleOrDeviceDefault(pClient,
                                                   pKernelBus->flaInfo.hDevice,
                                                   pKernelBus->flaInfo.hFlaVASpace,
                                                   &pKernelBus->flaInfo.pFlaVAS);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                    "failed getting the vaspace from handle, status=0x%x\n",
                    status);
            goto free_rpc;
        }
    }
    else
    {
        // Pin the VASPACE page directory for pFlaVAS before writing the instance block
        status = vaspacePinRootPageDir(pKernelBus->flaInfo.pFlaVAS, pGpu);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "failed pinning down FLAVASpace, status=0x%x\n",
                    status);
            goto unpin_rootpagedir;
        }

        if (pGpu->pFabricVAS != NULL)
        {
            // Pin the VASPACE page directory for pFabricVAS before writing the instance block
            status = vaspacePinRootPageDir(pGpu->pFabricVAS, pGpu);
            if (status != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR, "failed pinning down fabric vaspace, status=0x%x\n",
                          status);
                goto unpin_rootpagedir;
            }
        }

        // Construct instance block
        status = kbusConstructFlaInstBlk_HAL(pGpu, pKernelBus, GPU_GFID_PF);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                    "failed constructing instblk for FLA, status=0x%x\n",
                    status);
            goto free_instblk;
        }

        // Instantiate Inst Blk for pFlaVAS
        status = kgmmuInstBlkInit(pKernelGmmu,
                                 pKernelBus->flaInfo.pInstblkMemDesc,
                                 pKernelBus->flaInfo.pFlaVAS, FIFO_PDB_IDX_BASE,
                                 &pInstblkParams);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                    "failed instantiating instblk for FLA, status=0x%x\n",
                    status);
            goto free_instblk;
        }
    }
    pKernelBus->flaInfo.bFlaAllocated    = NV_TRUE;
    pKernelBus->flaInfo.bToggleBindPoint = NV_TRUE;

    if (pGpu->pFabricVAS != NULL)
    {
        NV_ASSERT_OK_OR_GOTO(status, fabricvaspaceInitUCRange(
                                     dynamicCast(pGpu->pFabricVAS, FABRIC_VASPACE), pGpu,
                                     base, size), free_instblk);
    }

    //
    // For SRIOV PF/VF system, always check for P2P allocation to determine whether
    // this function is allowed to bind FLA
    //
    if (gpuIsSriovEnabled(pGpu) || IS_VIRTUAL(pGpu))
    {
        if (gpuCheckIsP2PAllocated_HAL(pGpu))
        {
            status = kbusSetupBindFla(pGpu, pKernelBus, pGpu->sriovState.pP2PInfo->gfid);
        }
        else
        {
            NV_PRINTF(LEVEL_INFO, "Skipping binding FLA, because no P2P GFID is"
                      " validated yet\n");
        }
    }
    else
    {
        status = kbusSetupBindFla(pGpu, pKernelBus, GPU_GFID_PF);
    }

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "failed binding instblk for FLA, status=0x%x\n", status);
        goto free_instblk;
    }
    return status;

free_rpc:
     {
        NV2080_CTRL_FLA_RANGE_PARAMS params = {0};
        params.mode = NV2080_CTRL_FLA_RANGE_PARAMS_MODE_HOST_MANAGED_VAS_DESTROY;
        NV_RM_RPC_CONTROL(pGpu, pKernelBus->flaInfo.hClient, pKernelBus->flaInfo.hSubDevice,
                          NV2080_CTRL_CMD_FLA_RANGE,
                          &params, sizeof(params), status);
        goto free_client;
     }

free_instblk:
    kbusDestructFlaInstBlk_HAL(pGpu, pKernelBus);

unpin_rootpagedir:
    if (pKernelBus->flaInfo.pFlaVAS != NULL)
    {
        vaspaceUnpinRootPageDir(pKernelBus->flaInfo.pFlaVAS, pGpu);
    }

    if (pGpu->pFabricVAS != NULL)
    {
        vaspaceUnpinRootPageDir(pGpu->pFabricVAS, pGpu);
    }

free_client:
    if (pGpu->pFabricVAS != NULL)
    {
        vmmDestroyVaspace(pVmm, pGpu->pFabricVAS);
        pGpu->pFabricVAS = NULL;
    }

    pRmApi->Free(pRmApi, pKernelBus->flaInfo.hClient, pKernelBus->flaInfo.hClient);
    pKernelBus->flaInfo.bFlaAllocated = NV_FALSE;

    NV_PRINTF(LEVEL_ERROR, "failed allocating FLA VASpace status=0x%x\n",
              status);

    return status;
}

/*!
 * @brief Sets up the Host Managed FLA state for the GPU.
 * This function will manage bare minimum resources in host RM such as
 * allocating PDB, constructing instance memory block in subheap of the vGPU device,
 * binding the PDB with the VASpace.
 *
 * @param[in]  hClient    Client handle which owns the FLA resources
 * @param[in]  hDevice    Device handle associated with FLA VAS
 * @param[in]  hSubdevice SubDevice handle associated with FLA VAS
 * @param[in]  hVASpace   FLA Vaspace handle
 * @param[in]  base       VASpace base
 * @param[in]  size       VASpace size
 * @param[in]  gfid       Calling Context
 *
 * @return NV_OK if successful
 */
NV_STATUS
kbusAllocateHostManagedFlaVaspace_GA100
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    NvHandle   hClient,
    NvHandle   hDevice,
    NvHandle   hSubdevice,
    NvHandle   hVASpace,
    NvU64      base,
    NvU64      size,
    NvU32      gfid
)
{
    KernelGmmu *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);
    OBJVMM     *pVmm        = SYS_GET_VMM(SYS_GET_INSTANCE());
    INST_BLK_INIT_PARAMS pInstblkParams = {0};
    RsClient   *pClient;
    NV_STATUS   status;

    NV_ASSERT_OR_RETURN(pGpu != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(size != 0, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(IS_GFID_VF(gfid), NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(hClient != NV01_NULL_OBJECT, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(hDevice != NV01_NULL_OBJECT, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(hSubdevice != NV01_NULL_OBJECT, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(hVASpace != NV01_NULL_OBJECT, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(!pKernelBus->flaInfo.bFlaAllocated, NV_ERR_INVALID_ARGUMENT);

    pKernelBus->flaInfo.base = base;
    pKernelBus->flaInfo.size = size;
    pKernelBus->flaInfo.hClient = hClient;
    pKernelBus->flaInfo.hDevice = hDevice;
    pKernelBus->flaInfo.hSubDevice = hSubdevice;
    pKernelBus->flaInfo.hFlaVASpace = hVASpace;

    status = serverGetClientUnderLock(&g_resServ, pKernelBus->flaInfo.hClient, &pClient);
    NV_ASSERT_OR_GOTO(status == NV_OK, cleanup);

    status = vaspaceGetByHandleOrDeviceDefault(pClient,
                                               pKernelBus->flaInfo.hDevice,
                                               pKernelBus->flaInfo.hFlaVASpace,
                                               &pKernelBus->flaInfo.pFlaVAS);

    // Allocate a FABRIC_VASPACE_A object
    status = vmmCreateVaspace(pVmm, FABRIC_VASPACE_A, pGpu->gpuId, gpumgrGetGpuMask(pGpu),
                              base, base + size - 1, 0, 0, NULL, 0,
                              &pGpu->pFabricVAS);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "failed allocating fabric vaspace, status=0x%x\n",
                  status);
        goto cleanup;
    }

    // Pin the VASPACE page directory for pFabricVAS before writing the instance block
    status = vaspacePinRootPageDir(pGpu->pFabricVAS, pGpu);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "failed pinning down fabric vaspace, status=0x%x\n",
                    status);
        goto cleanup;
    }

    // Pin the VASPACE page directory for Legacy VAS  before writing the instance block
    status = vaspacePinRootPageDir(pKernelBus->flaInfo.pFlaVAS, pGpu);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "failed pinning down legacy vaspace, status=0x%x\n",
                    status);
        goto unpin_fabric_page_dir;
    }

    // Construct instance block
    status = kbusConstructFlaInstBlk_HAL(pGpu, pKernelBus, gfid);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                 "failed constructing instblk for FLA, status=0x%x\n",
                  status);
        goto unpin_legacy_page_dir;
    }

    // Instantiate Inst Blk for FLA
    status = kgmmuInstBlkInit(pKernelGmmu,
                              pKernelBus->flaInfo.pInstblkMemDesc,
                              pKernelBus->flaInfo.pFlaVAS, FIFO_PDB_IDX_BASE,
                              &pInstblkParams);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "failed instantiating instblk for FLA, status=0x%x\n",
                  status);
        goto free_instblk;
    }

    pKernelBus->flaInfo.bFlaAllocated = NV_TRUE;
    pKernelBus->flaInfo.bToggleBindPoint = NV_TRUE;

    if (pGpu->pFabricVAS != NULL)
    {
        NV_ASSERT_OK_OR_GOTO(status, fabricvaspaceInitUCRange(
                                     dynamicCast(pGpu->pFabricVAS, FABRIC_VASPACE), pGpu,
                                     base, size), free_instblk);
    }

    return status;

free_instblk:
    kbusDestructFlaInstBlk_HAL(pGpu, pKernelBus);

unpin_legacy_page_dir:
    if (pKernelBus->flaInfo.pFlaVAS != NULL)
    {
        vaspaceUnpinRootPageDir(pKernelBus->flaInfo.pFlaVAS, pGpu);
    }

unpin_fabric_page_dir:
    if (pGpu->pFabricVAS != NULL)
    {
        vaspaceUnpinRootPageDir(pGpu->pFabricVAS, pGpu);
    }

cleanup:
    if (pGpu->pFabricVAS != NULL)
    {
        vmmDestroyVaspace(pVmm, pGpu->pFabricVAS);
        pGpu->pFabricVAS = NULL;
    }

    pKernelBus->flaInfo.bFlaAllocated = NV_FALSE;
    return status;
}

/*!
 * Top level function to check if the platform supports FLA, and initialize if
 * supported. This function gets called in all the platforms where Nvlink is enabled.
 *
 * @param[in]  base       VASpace base
 * @param[in]  size       VASpace size
 */
NV_STATUS
kbusCheckFlaSupportedAndInit_GA100
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    NvU64      base,
    NvU64      size
)
{

    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);

    portMemSet(&pKernelBus->flaInfo, 0, sizeof(pKernelBus->flaInfo));

    // Initialize FLA State Info if possible
    //  1. FLA is by default enabled for GA100,
    //  2. Disable FLA when MIG is enabled
    //     Currently MIG is persistent state, so GPU reboot will happpen, when MIG is being enabled/disabled
    //     so when the GPU reboot happens with a modified state, don't enable FLA. This is decent WAR for bug: 2568634
    //   3. Disable FLA when SLI is enabled
    //       Bug: 2985556, re-enable once we fix this bug.
    //
    if (((NULL != pKernelMIGManager) && !kmigmgrIsMIGNvlinkP2PSupported(pGpu, pKernelMIGManager)) ||
        (IsSLIEnabled(pGpu)) || pGpu->getProperty(pGpu, PDB_PROP_GPU_BROKEN_FB) ||
        pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_ALL_INST_IN_SYSMEM))
    {
        NV_PRINTF(LEVEL_INFO, "FLA is disabled, gpu %x is in MIG/SLI mode \n", pGpu->gpuInstance);
        pKernelBus->bFlaSupported = NV_FALSE;
        return NV_OK;
    }
    else // for all non-MIG configs, FLA is supported
    {
        NV_PRINTF(LEVEL_INFO, "Enabling FLA_SUPPORTED to TRUE, gpu: %x ...\n", pGpu->gpuInstance);
        pKernelBus->bFlaSupported = NV_TRUE;
    }

    //
    // FLA VAspace is allocated from CPU, so no need to do anything
    // in GSP except setting the property
    //
    if (RMCFG_FEATURE_PLATFORM_GSP)
        return NV_OK;

    NV_ASSERT_OK_OR_RETURN(kbusDetermineFlaRangeAndAllocate_HAL(pGpu, pKernelBus, base, size));

    return NV_OK;
}

/*!
 * @brief Determine FLA Base and Size for NvSwitch Virtualization systems
 *        from reading the scratch registers. This determines FLA base and size
 *        GA100 direct connected systems as well as skip allocation of FLA Vaspace
 *        for vgpu host.
 *
 * @param[in]  base       VASpace base
 * @param[in]  size       VASpace size
 *
 * @return NV_OK if successful
 */
NV_STATUS
kbusDetermineFlaRangeAndAllocate_GA100
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    NvU64      base,
    NvU64      size
)
{
    OBJSYS           *pSys              = SYS_GET_INSTANCE();
    NV_STATUS         status            = NV_OK;
    KernelNvlink     *pKernelNvlink     = GPU_GET_KERNEL_NVLINK(pGpu);

    NV2080_CTRL_NVLINK_GET_SET_NVSWITCH_FLA_ADDR_PARAMS params;

    if (pSys->getProperty(pSys, PDB_PROP_SYS_FABRIC_IS_EXTERNALLY_MANAGED))
    {
        // Nvswitch virtualization enabled
        if (pKernelNvlink != NULL && knvlinkIsNvswitchProxyPresent(pGpu, pKernelNvlink))
        {
            portMemSet(&params, 0, sizeof(params));
            params.bGet = NV_TRUE;

            status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                         NV2080_CTRL_CMD_NVLINK_GET_SET_NVSWITCH_FLA_ADDR,
                                         (void *)&params, sizeof(params));
            if (status != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "Failed to get the NVSwitch FLA address\n");
                return status;
            }

            size = gpuGetFlaVasSize_HAL(pGpu, NV_TRUE);

            status = knvlinkSetUniqueFlaBaseAddress(pGpu, pKernelNvlink, params.addr);
            if (status != NV_OK)
            {
                NV_PRINTF(LEVEL_INFO, "Failed to enable FLA for GPU: %x\n", pGpu->gpuInstance);
                return status;
            }

            base = params.addr;
        }
        else
        {
             return status;
        }
    }
    else // direct connected systems
    {
        if (hypervisorIsVgxHyper())
        {
            NV_PRINTF(LEVEL_INFO, "Skipping the FLA initialization in Host vGPU \n");
            return NV_OK;
        }
        if (!size)
        {
            size = gpuGetFlaVasSize_HAL(pGpu, NV_FALSE);
            base = pGpu->gpuInstance * size;
        }
    }
    NV_ASSERT_OK_OR_RETURN(kbusAllocateFlaVaspace_HAL(pGpu, pKernelBus, base, size));
    return status;
}

/*!
 * @brief Destruct the FLA data structure and associated resources.
 *        Since all the resources are associated with the RM client,
 *        all resources will be destroyed by Resource server.
 *        Note: kbusDestroyFla can be called from different places
 *               1. For direct connected systems, RM unload will call this function.
 */
void
kbusDestroyFla_GA100
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus
)
{
    OBJSYS *pSys   = SYS_GET_INSTANCE();
    OBJVMM *pVmm   = SYS_GET_VMM(pSys);
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    // TODO: if there are dangling resources, cleanup here
    if ((pKernelBus->flaInfo.pFlaVAS != NULL) || (pGpu->pFabricVAS != NULL))
    {
        if (pKernelBus->flaInfo.bFlaBind)
        {
            if (IS_VIRTUAL(pGpu) || IS_GSP_CLIENT(pGpu))
            {
                kbusSetupUnbindFla_HAL(pGpu, pKernelBus);
            }
        }

        if (pKernelBus->flaInfo.bFlaAllocated)
        {
            if (IS_VIRTUAL(pGpu) && gpuIsWarBug200577889SriovHeavyEnabled(pGpu))
            {
                NV2080_CTRL_FLA_RANGE_PARAMS params = {0};
                NV_STATUS status = NV_OK;
                params.mode = NV2080_CTRL_FLA_RANGE_PARAMS_MODE_HOST_MANAGED_VAS_DESTROY;
                NV_RM_RPC_CONTROL(pGpu, pKernelBus->flaInfo.hClient, pKernelBus->flaInfo.hSubDevice,
                                NV2080_CTRL_CMD_FLA_RANGE,
                                &params, sizeof(params), status);

                if (status != NV_OK)
                {
                    NV_PRINTF(LEVEL_WARNING, "RPC to host failed with status: 0x%x\n", status);
                }

                //
                // For SRIOV-Heavy, Instance block is allocated in host, so only destroying the
                // vaspace
                //
                if (pGpu->pFabricVAS != NULL)
                {
                    vmmDestroyVaspace(pVmm, pGpu->pFabricVAS);
                    pGpu->pFabricVAS = NULL;
                }

                pRmApi->Free(pRmApi, pKernelBus->flaInfo.hClient, pKernelBus->flaInfo.hClient);
                portMemSet(&pKernelBus->flaInfo, 0, sizeof(pKernelBus->flaInfo));
            }
            else
            {
                if (pKernelBus->flaInfo.pFlaVAS != NULL)
                {
                    vaspaceUnpinRootPageDir(pKernelBus->flaInfo.pFlaVAS, pGpu);
                }

                if (pGpu->pFabricVAS != NULL)
                {
                    vaspaceUnpinRootPageDir(pGpu->pFabricVAS, pGpu);
                }

                kbusDestructFlaInstBlk_HAL(pGpu, pKernelBus);

                if (pGpu->pFabricVAS != NULL)
                {
                    vmmDestroyVaspace(pVmm, pGpu->pFabricVAS);
                    pGpu->pFabricVAS = NULL;
                }

                pRmApi->Free(pRmApi, pKernelBus->flaInfo.hClient, pKernelBus->flaInfo.hClient);
                portMemSet(&pKernelBus->flaInfo, 0, sizeof(pKernelBus->flaInfo));
            }

            pKernelBus->flaInfo.bFlaAllocated = NV_FALSE;
        }
    }
}

void
kbusDestroyHostManagedFlaVaspace_GA100
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    NvU32      gfid
)
{
    OBJSYS *pSys   = SYS_GET_INSTANCE();
    OBJVMM *pVmm   = SYS_GET_VMM(pSys);

    NV_PRINTF(LEVEL_INFO, "Freeing the FLA client: 0x%x FLAVASpace:%x, gpu:%x \n",
             pKernelBus->flaInfo.hClient, pKernelBus->flaInfo.hFlaVASpace, pGpu->gpuInstance);

    if (pKernelBus->flaInfo.bFlaAllocated)
    {
        KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);

        if (pKernelBus->flaInfo.pFlaVAS != NULL)
        {
            vaspaceUnpinRootPageDir(pKernelBus->flaInfo.pFlaVAS, pGpu);
            pKernelBus->flaInfo.hFlaVASpace = NV01_NULL_OBJECT;
            pKernelBus->flaInfo.pFlaVAS = NULL;
        }

        if (pGpu->pFabricVAS != NULL)
        {
            vaspaceUnpinRootPageDir(pGpu->pFabricVAS, pGpu);
            vmmDestroyVaspace(pVmm, pGpu->pFabricVAS);
            pGpu->pFabricVAS = NULL;
        }

        kbusDestructFlaInstBlk_HAL(pGpu, pKernelBus);
        pKernelBus->flaInfo.hClient = NV01_NULL_OBJECT;
        pKernelBus->flaInfo.hDevice = NV01_NULL_OBJECT;
        pKernelBus->flaInfo.hSubDevice = NV01_NULL_OBJECT;
        pKernelBus->flaInfo.bFlaAllocated = NV_FALSE;
        if (pKernelNvlink == NULL || !knvlinkIsGpuConnectedToNvswitch(pGpu, pKernelNvlink))
        {
            pKernelBus->flaInfo.bFlaRangeRegistered = NV_FALSE;
            pKernelBus->flaInfo.base = 0;
            pKernelBus->flaInfo.size = 0;
        }
    }
}

/*!
 * @brief This function will return the OBJVASPACE for the FLA VAS.
 *
 * @param[in/out]  ppVAS    OBJVASPACE double pointer
 *
 * @return NV_ERR_NOT_SUPPORTED, if FLA is not supported,
 *         else NV_OK
 */
NV_STATUS
kbusGetFlaVaspace_GA100
(
    OBJGPU      *pGpu,
    KernelBus   *pKernelBus,
    OBJVASPACE **ppVAS
)
{
    NV_STATUS         status  = NV_OK;
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    KernelNvlink     *pKernelNvlink     = GPU_GET_KERNEL_NVLINK(pGpu);

    *ppVAS = NULL;

    // Return NV_ERR_NOT_SUPPORTED if nvlink is force disabled using cmd line args
    if (!IS_VIRTUAL(pGpu) && pKernelNvlink == NULL)
    {
        NV_PRINTF(LEVEL_WARNING, "Nvlink is not supported in this GPU: %x \n", pGpu->gpuInstance);
        return  NV_ERR_NOT_SUPPORTED;
    }

    // Return NV_ERR_NOT_SUPPORTED when we are in MIG mode
    if ((pKernelMIGManager != NULL) && !kmigmgrIsMIGNvlinkP2PSupported(pGpu, pKernelMIGManager))
    {
        NV_PRINTF(LEVEL_WARNING, "FLA is not supported with MIG enabled, GPU: %x \n", pGpu->gpuInstance);
        return NV_ERR_NOT_SUPPORTED;
    }

    if (!kbusIsFlaSupported(pKernelBus))
    {
        NV_PRINTF(LEVEL_WARNING, "FLA is not supported, GPU: %x\n", pGpu->gpuInstance);
        return NV_ERR_NOT_SUPPORTED;
    }

    if (!IS_VIRTUAL(pGpu) && !kbusIsFlaEnabled(pKernelBus))
    {
        if (!gpuIsSriovEnabled(pGpu) && !IS_VIRTUAL(pGpu))
        {
            NV_PRINTF(LEVEL_WARNING, "FLA is not enabled, GPU: %x\n", pGpu->gpuInstance);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
    //
    // when FLA init moves to P2P object creation time, any client trying to get
    // FLA VAS reference, needs to be returned NV_ERR_NOT_SUPPORTED. In that case, only
    // way to determine is to check if links are trained in the system. Since we dont have an easy
    // way to do the checks, currently we can assume that Nvlinks will not be disabled outside of MIG
    //

    *ppVAS = pKernelBus->flaInfo.pFlaVAS;

    NV_PRINTF(LEVEL_INFO, "returning the vas: %p for GPU: %x start: 0x%llx, limit:0x%llx \n",
              pKernelBus->flaInfo.pFlaVAS, pGpu->gpuInstance, pKernelBus->flaInfo.pFlaVAS->vasStart,
              pKernelBus->flaInfo.pFlaVAS->vasLimit);

    return status;
}

/*!
 * @brief Constructor for the Instance Memory block for FLA VASpace. This will
 *        allocate the memory descriptor for the IMB.
 *
 *
 * @return NV_OK, if successful
 */
NV_STATUS
kbusConstructFlaInstBlk_GA100
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    NvU32      gfid
)
{
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    NV_STATUS      status = NV_OK;
    NvU32          aperture;
    NvU64          size;
    NvU32          cpuCacheAttrib;
    NvU32          flags = MEMDESC_FLAGS_NONE;

    // Inst Blocks are by default in FB
    aperture = ADDR_FBMEM;
    cpuCacheAttrib = NV_MEMORY_UNCACHED;
    size = NV_RAMIN_ALLOC_SIZE;

    if (gpuIsWarBug200577889SriovHeavyEnabled(pGpu) && IS_GFID_PF(gfid))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (IS_GFID_VF(gfid))
        flags |= MEMDESC_FLAGS_OWNED_BY_CURRENT_DEVICE;

    // create the memdesc for instance block
    status = memdescCreate(&pKernelBus->flaInfo.pInstblkMemDesc, pGpu,
                           size, 0, NV_TRUE,
                           aperture, cpuCacheAttrib, flags);

    NV_ASSERT(status == NV_OK);

    memdescTagAlloc(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_89, 
                        pKernelBus->flaInfo.pInstblkMemDesc);

    NV_ASSERT(status == NV_OK);

    // Initialize the memdesc to zero
    status = memmgrMemDescMemSet(pMemoryManager,
                                 pKernelBus->flaInfo.pInstblkMemDesc,
                                 0,
                                 TRANSFER_FLAGS_NONE);
    NV_ASSERT(status == NV_OK);

    return status;
}

/*!
 * @brief Destruct the Instance memory block allocated for FLA VAS
 *
 */
void
kbusDestructFlaInstBlk_GA100
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus
)
{
    // Free the FLA Inst Blk MemDesc
   if (pKernelBus->flaInfo.pInstblkMemDesc != NULL)
   {
       memdescFree(pKernelBus->flaInfo.pInstblkMemDesc);
       memdescDestroy(pKernelBus->flaInfo.pInstblkMemDesc);
       pKernelBus->flaInfo.pInstblkMemDesc = NULL;
   }
}

/*!
 * @brief Function to determine if the mapping can be direct mapped or BAR mapped
 *
 * @param[in]   pMemDesc    Memory Descriptor pointer
 * @param[in]   mapFlags    Flags used for mapping
 * @param[in]   bDirectSysMappingAllowed boolean to return the result
 *
 * returns NV_ERR_INVALID_ARGUMENT, if the reflected mapping is requested
 *         NV_OK, otherwise
 */
NV_STATUS
kbusIsDirectMappingAllowed_GA100
(
    OBJGPU            *pGpu,
    KernelBus         *pKernelBus,
    MEMORY_DESCRIPTOR *pMemDesc,
    NvU32              mapFlags,
    NvBool            *bDirectSysMappingAllowed
)
{
    *bDirectSysMappingAllowed = NV_FALSE;

    if (DRF_VAL(OS33, _FLAGS, _MAPPING, mapFlags) == NVOS33_FLAGS_MAPPING_REFLECTED)
    {
        NV_PRINTF(LEVEL_WARNING, "BAR allocation trying to request reflected mapping, "
                   "by passing the map flags, failing the request \n");
    }

    if (memdescGetFlag(pMemDesc, MEMDESC_FLAGS_ENCRYPTED))
    {
        NV_PRINTF(LEVEL_WARNING, "BAR allocation trying to request reflected mapping, "
                   "by setting ENCRYPTED flag in memdesc, failing the request \n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    *bDirectSysMappingAllowed = NV_TRUE;
    return NV_OK;
}

/*!
 * @brief Returns the Nvlink peer ID from pGpu0 to pGpu1
 *
 * @param[in]   pGpu0          (local GPU)
 * @param[in]   pKernelBus0    (local GPU)
 * @param[in]   pGpu1          (remote GPU)
 * @param[in]   pKernelBus1    (remote GPU)
 * @param[out]  nvlinkPeer     NvU32  pointer
 *
 * return NV_OK on success
 */
NV_STATUS
kbusGetNvlinkP2PPeerId_GA100
(
    OBJGPU    *pGpu0,
    KernelBus *pKernelBus0,
    OBJGPU    *pGpu1,
    KernelBus *pKernelBus1,
    NvU32     *nvlinkPeer,
    NvU32      attributes
)
{
    KernelNvlink *pKernelNvlink0 = GPU_GET_KERNEL_NVLINK(pGpu0);
    NV_STATUS     status         = NV_OK;
    NvBool        bEgmPeer = FLD_TEST_DRF(_P2PAPI, _ATTRIBUTES, _REMOTE_EGM, _YES, attributes);

    if (nvlinkPeer == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    *nvlinkPeer = BUS_INVALID_PEER;

    // If the chip does not support NVLink, then return
    if (pKernelNvlink0 == NULL)
    {
        return NV_OK;
    }

    //
    // If NVLINK topology is forced and the forced configuration has peer links,
    // get the peer ID from the table
    //
    if ((knvlinkIsForcedConfig(pGpu0, pKernelNvlink0) ||
        knvlinkAreLinksRegistryOverriden(pGpu0, pKernelNvlink0)) && !bEgmPeer)
    {
        if (knvlinkGetPeersNvlinkMaskFromHshub(pGpu0, pKernelNvlink0) != 0)
        {
            *nvlinkPeer = kbusGetPeerIdFromTable_HAL(pGpu0, pKernelBus0,
                                                     pGpu0->gpuInstance,
                                                     pGpu1->gpuInstance);

            if (*nvlinkPeer == BUS_INVALID_PEER)
            {
                return NV_ERR_INVALID_REQUEST;
            }
        }
        return NV_OK;
    }

    //
    // NVLINK topology is not forced. Get the NVLink P2P peer ID for NVLink
    // auto-config.
    //

    //
    // Return if there are no NVLink connections to the remote GPU
    //
    // skipping this check for knvlinkIsForcedConfig case since
    // peerLinkMasks is not set for knvlinkIsForcedConfig
    // this will be skipped only for reserving EGM peerId (bEgmPeer is true)
    // when client has not provided EGM peerId mask in forced config
    // tests.
    //
    if (!(knvlinkIsForcedConfig(pGpu0, pKernelNvlink0) ||
          knvlinkAreLinksRegistryOverriden(pGpu0, pKernelNvlink0)) &&
         (knvlinkGetPeerLinkMask(pGpu0, pKernelNvlink0, gpuGetInstance(pGpu1)) == 0))
    {
        return NV_OK;
    }

    // Return if a peer ID is already allocated for P2P from pGpu0 to pGpu1
    if (bEgmPeer)
    {
        *nvlinkPeer = kbusGetEgmPeerId_HAL(pGpu0, pKernelBus0, pGpu1);
    }
    else
    {
        *nvlinkPeer = kbusGetPeerId_HAL(pGpu0, pKernelBus0, pGpu1);
    }

    if (*nvlinkPeer != BUS_INVALID_PEER)
    {
        return NV_OK;
    }

    //
    // Peer ID 0 is used for the following use-cases:
    //     1. If the GPU is connected to itself through NVLink (loopback)
    //     2. If the GPU is connected to the other GPU through NVSwitch
    //
    // On NVSwitch systems, peer ID 0 might not be available only if:
    //     1. PCIe P2P is allowed along with NVLink P2P on NVSWitch systems
    //     2. Mix of direct NVLink and NVSwitch connections is supported
    //   None of the above hold true currently
    //
    if (((pGpu0 == pGpu1) && !bEgmPeer) || knvlinkIsGpuConnectedToNvswitch(pGpu0, pKernelNvlink0))
    {
        *nvlinkPeer = 0;

        goto kbusGetNvlinkP2PPeerId_end;
    }

    // If no peer ID has been assigned yet, find the first unused peer ID
    if (*nvlinkPeer == BUS_INVALID_PEER)
    {
        *nvlinkPeer = kbusGetUnusedPeerId_HAL(pGpu0, pKernelBus0);

        // If could not find a free peer ID, return error
        if (*nvlinkPeer == BUS_INVALID_PEER)
        {
            NV_PRINTF(LEVEL_WARNING,
                      "GPU%d: peerID not available for NVLink P2P\n",
                      pGpu0->gpuInstance);
            return NV_ERR_GENERIC;
        }

        goto kbusGetNvlinkP2PPeerId_end;
    }

kbusGetNvlinkP2PPeerId_end:

    // Reserve the peer ID for NVLink use
    status = kbusReserveP2PPeerIds_HAL(pGpu0, pKernelBus0, NVBIT(*nvlinkPeer));

    if ((status == NV_OK) &&
        !knvlinkIsGpuConnectedToNvswitch(pGpu0, pKernelNvlink0))
    {
        pKernelBus0->p2p.bEgmPeer[*nvlinkPeer] = bEgmPeer;
    }

    return status;
}

/**
 *@brief Select whether RM needs to use direct mapping or BAR mapping
 *       This function is a WAR for bug: 2494500, where FB hangs if SW issues
 *       reflected accesses. RM should select direct mapping for any accesses
 *       other than FB
 *
 * @param[in]     pMemDesc           MEMORY_DESCRIPTOR pointer
 * @param[in/out] pbAllowDirectMap   NvBool pointer
 *
 *@returns NV_OK, if supported
 *         NV_ERR_NOT_SUPPORTED, otherwise
 */
NV_STATUS
kbusUseDirectSysmemMap_GA100
(
    OBJGPU            *pGpu,
    KernelBus         *pKernelBus,
    MEMORY_DESCRIPTOR *pMemDesc,
    NvBool            *pbAllowDirectMap
)
{
    *pbAllowDirectMap = NV_FALSE;

    if((memdescGetAddressSpace(pMemDesc) != ADDR_FBMEM))
    {
        *pbAllowDirectMap = NV_TRUE;
    }

    return NV_OK;
}

/*!
 * @brief   Validates FLA base address.
 *
 * @param         flaBaseAddr
 *
 * @returns On success, NV_OK.
 *          On failure, returns NV_ERR_XXX.
 */
NV_STATUS
kbusValidateFlaBaseAddress_GA100
(
    OBJGPU            *pGpu,
    KernelBus         *pKernelBus,
    NvU64              flaBaseAddr
)
{
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    NvU64 fbSizeBytes;

    fbSizeBytes = pMemoryManager->Ram.fbTotalMemSizeMb << 20;

    //
    // Ampere SKUs will be paired with NVSwitches (Limerock) supporting 2K
    // mapslots that can cover 64GB each. Make sure that the fabric base
    // address being used is valid to cover whole frame buffer.
    //

    // Check if fabric address is aligned to mapslot size.
    if (flaBaseAddr & (NVBIT64(NVLNK_FABRIC_ADDR_GRANULARITY) - 1))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Align fbSize to mapslot size.
    fbSizeBytes = RM_ALIGN_UP(fbSizeBytes, NVBIT64(NVLNK_FABRIC_ADDR_GRANULARITY));

    // Make sure the address range doesn't go beyond the limit, (2K * 64GB).
    if ((flaBaseAddr + fbSizeBytes) > NVBIT64(NV_BUS_FLA_VASPACE_ADDR_HI))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    return NV_OK;
}

/*!
 * @brief Validates FLA Range allocated in the GPU when FM registers itself to RM
 *         This is useful when FM gets killed/crashed during the app runtime and can
 *         re-spawn at any point later. We don't do any client validation, since FM is
 *         a privileged process managed by sysadmin.
 *
 * @param[in]    flaBaseAddr NvU64 address
 * @param[in]    flaSize     NvU64 Size
 *
 *  @returns NV_TRUE, if flaBaseAddr & flaSize matches the existing FLA VAS allocation
 *           else, NV_FALSE
 *
 */
 NvBool
 kbusVerifyFlaRange_GA100
 (
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    NvU64      flaBaseAddr,
    NvU64      flaSize
 )
 {
     if ((pKernelBus->flaInfo.base != flaBaseAddr) || (pKernelBus->flaInfo.size != flaSize))
           return NV_FALSE;

    NV_PRINTF(LEVEL_INFO, "FLA base: %llx, size: %llx is verified \n", flaBaseAddr, flaSize);
    return NV_TRUE;
 }

/*!
 * @brief Returns the NvSwitch peer ID
 *
 *
 * @returns NvU32 bus peer number
 */
NvU32
kbusGetNvSwitchPeerId_GA100
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus
)
{
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);
    NvU32 peerId = BUS_INVALID_PEER;
    NvU32 i;

    if (IS_VIRTUAL(pGpu) && GPU_IS_NVSWITCH_DETECTED(pGpu))
        goto nvswitch_detected;

    if (pKernelNvlink == NULL)
        return BUS_INVALID_PEER;

    if (!knvlinkIsGpuConnectedToNvswitch(pGpu, pKernelNvlink))
        return BUS_INVALID_PEER;

nvswitch_detected:

    for (i = 0; i < NV_MAX_DEVICES; i++)
    {
        peerId = pKernelBus->p2p.busNvlinkPeerNumberMask[i];

        if (peerId == 0)
            continue;

        LOWESTBITIDX_32(peerId);

        break;
    }

    return peerId;
}

/*!
 * @brief Helper function to extract information from FLA data structure and
 *        to trigger RPC to Physical RM to BIND FLA VASpace
 *
 * @param[in]  gfid     GFID
 *
 * @return NV_OK if successful
 */
NV_STATUS
kbusSetupBindFla_GA100
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    NvU32      gfid
)
{
    NV_STATUS status = NV_OK;
    NV2080_CTRL_FLA_SETUP_INSTANCE_MEM_BLOCK_PARAMS params = {0};

    if (!gpuIsWarBug200577889SriovHeavyEnabled(pGpu))
    {
        MEMORY_DESCRIPTOR  *pMemDesc;
        RmPhysAddr          imbPhysAddr;
        NvU32               addrSpace;

        pMemDesc     = pKernelBus->flaInfo.pInstblkMemDesc;
        imbPhysAddr  = memdescGetPhysAddr(pMemDesc, AT_GPU, 0);
        addrSpace    = memdescGetAddressSpace(pMemDesc);
        NV2080_CTRL_FLA_ADDRSPACE paramAddrSpace = NV2080_CTRL_FLA_ADDRSPACE_FBMEM;

        switch(addrSpace)
        {
            case ADDR_FBMEM:
                paramAddrSpace = NV2080_CTRL_FLA_ADDRSPACE_FBMEM;
                break;
            case ADDR_SYSMEM:
                paramAddrSpace = NV2080_CTRL_FLA_ADDRSPACE_SYSMEM;
                break;
        }
        params.imbPhysAddr = imbPhysAddr;
        params.addrSpace   = paramAddrSpace;
    }
    params.flaAction   = NV2080_CTRL_FLA_ACTION_BIND;

    NV_RM_RPC_CONTROL(pGpu, pKernelBus->flaInfo.hClient,
                        pKernelBus->flaInfo.hSubDevice,
                        NV2080_CTRL_CMD_FLA_SETUP_INSTANCE_MEM_BLOCK,
                        &params, sizeof(params), status);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "FLA bind failed, status: %x \n", status);
        return status;
    }

    // Since FLA state is tracked in the Guest, Guest RM needs to set it here
    pKernelBus->flaInfo.bFlaBind = NV_TRUE;
    pKernelBus->bFlaEnabled      = NV_TRUE;

    return NV_OK;
}

/*!
 * @brief Helper function to trigger RPC to Physical RM to unbind FLA VASpace
 *
 * @return NV_OK if successful
 */
NV_STATUS
kbusSetupUnbindFla_GA100
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus
)
{
    NV_STATUS status = NV_OK;
    NV2080_CTRL_FLA_SETUP_INSTANCE_MEM_BLOCK_PARAMS params = { 0 };

    if (!pKernelBus->flaInfo.bFlaBind)
        return NV_OK;

    params.flaAction = NV2080_CTRL_FLA_ACTION_UNBIND;

    NV_RM_RPC_CONTROL(pGpu, pKernelBus->flaInfo.hClient,
                      pKernelBus->flaInfo.hSubDevice,
                      NV2080_CTRL_CMD_FLA_SETUP_INSTANCE_MEM_BLOCK,
                      &params, sizeof(params), status);

    pKernelBus->flaInfo.bFlaBind = NV_FALSE;
    pKernelBus->bFlaEnabled      = NV_FALSE;

    return status;
}

NV_STATUS
kbusGetFlaRange_GA100
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    NvU64     *ucFlaBase,
    NvU64     *ucFlaSize,
    NvBool     bIsConntectedToNvswitch
)
{
    if (gpuIsSriovEnabled(pGpu) && bIsConntectedToNvswitch)
    {
        if (pKernelBus->flaInfo.bFlaRangeRegistered)
        {
            *ucFlaBase = pKernelBus->flaInfo.base;
            *ucFlaSize = pKernelBus->flaInfo.size;
        }
    }
    else // direct connected system
    {
        *ucFlaSize = gpuGetFlaVasSize_HAL(pGpu, NV_FALSE);
        *ucFlaBase = pGpu->gpuInstance * (*ucFlaSize);
    }

    return NV_OK;
}

/*!
 * @brief Returns the Nvlink specific peer number from pGpu (Local) to pGpuPeer.
 *        Used only by VF.
 *
 * @param[in] pGpu          Local
 * @param[in] pKernelBus    Local
 * @param[in] pGpuPeer      Remote
 *
 * @returns NvU32 bus peer number
 */
NvU32
kbusGetNvlinkPeerId_GA100
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    OBJGPU    *pGpuPeer
)
{
    NvU32 gpuPeerInst = gpuGetInstance(pGpuPeer);
    NvU32 peerId = pKernelBus->p2p.busNvlinkPeerNumberMask[gpuPeerInst];

    if (peerId == 0)
    {
        NV_PRINTF(LEVEL_INFO,
                  "NVLINK P2P not set up between GPU%u and GPU%u, checking for PCIe P2P...\n",
                  gpuGetInstance(pGpu), gpuPeerInst);
        return BUS_INVALID_PEER;
    }

    LOWESTBITIDX_32(peerId);
    return peerId;
}

/*!
* @brief  Cache the value of NV_XVE_RESIZE_BAR1_CTRL_SIZE
*
* @param[in] pGpu       OBJGPU pointer
* @param[in] pKernelBus KernelBus pointer
*
*/
void
kbusCacheBAR1ResizeSize_WAR_BUG_3249028_GA100
(
   OBJGPU    *pGpu,
   KernelBus *pKernelBus
)
{
   NvU32 regVal = GPU_REG_RD32(pGpu, DEVICE_BASE(NV_PCFG) + NV_XVE_RESIZE_BAR1_CTRL);
   pKernelBus->bar1ResizeSizeIndex = DRF_VAL(_XVE, _RESIZE_BAR1_CTRL, _BAR_SIZE, regVal);
}

/*!
* @brief  Restore the value of NV_XVE_RESIZE_BAR1_CTRL_SIZE if different
*         from the cached value.
*         Windows has a strict requirement that the PCIE config has to stay
*         the same across power transitions.
*         Early SBIOS implementing resize BAR do not restore properly
*         the value of NV_XVE_RESIZE_BAR1_CTRL_SIZE.
*         The reason of this WAR is to not crash the systems that have
*         not been updated - yet.
*
* @param[in] pGpu       OBJGPU pointer
* @param[in] pKernelBus KernelBus pointer
*
* @returns      NV_STATUS
*
*/
NV_STATUS
kbusRestoreBAR1ResizeSize_WAR_BUG_3249028_GA100
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus
)
{
    NvU32 regVal;
    NvU32 bar1ResizeSizeIndex;

    if (!pKernelBus->getProperty(pKernelBus, PDB_PROP_KBUS_RESTORE_BAR1_SIZE_BUG_3249028_WAR))
    {
        return NV_OK;
    }

    NV_ASSERT_OR_RETURN(pKernelBus->bar1ResizeSizeIndex >= NV_XVE_RESIZE_BAR1_CTRL_BAR_SIZE_MIN,
        NV_ERR_INVALID_DATA);

    NV_ASSERT_OR_RETURN(pKernelBus->bar1ResizeSizeIndex <= NV_XVE_RESIZE_BAR1_CTRL_BAR_SIZE_MAX,
        NV_ERR_INVALID_DATA);

    regVal = GPU_REG_RD32(pGpu, DEVICE_BASE(NV_PCFG) + NV_XVE_RESIZE_BAR1_CTRL);
    bar1ResizeSizeIndex = DRF_VAL(_XVE, _RESIZE_BAR1_CTRL, _BAR_SIZE, regVal);

    if (bar1ResizeSizeIndex == pKernelBus->bar1ResizeSizeIndex)
    {
        // BAR1 size match. Nothing to do
        return NV_OK;
    }

    // BAR1 size changed. Warn and update
    NV_PRINTF(LEVEL_WARNING, "BAR1 size mismatch: current: 0x%x, expected: 0x%x\n",
        bar1ResizeSizeIndex, pKernelBus->bar1ResizeSizeIndex);
    NV_PRINTF(LEVEL_WARNING, "Most likely SBIOS did not restore the BAR1 size\n");
    NV_PRINTF(LEVEL_WARNING, "Please update your SBIOS!\n");

    regVal = FLD_SET_DRF_NUM(_XVE, _RESIZE_BAR1_CTRL, _BAR_SIZE, pKernelBus->bar1ResizeSizeIndex, regVal);
    GPU_REG_WR32(pGpu, DEVICE_BASE(NV_PCFG) + NV_XVE_RESIZE_BAR1_CTRL, regVal);

    return NV_OK;
}
