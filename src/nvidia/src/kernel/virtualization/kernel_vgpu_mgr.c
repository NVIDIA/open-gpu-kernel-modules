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

#include "ctrl/ctrla081.h"
#include "class/clc637.h" // for AMPERE_SMC_PARTITION_REF
#include "virtualization/kernel_hostvgpudeviceapi.h"

//
// ODB functions
//

NV_STATUS
kvgpumgrConstruct_IMPL(KernelVgpuMgr *pKernelVgpuMgr)
{
    return NV_OK;
}

void
kvgpumgrDestruct_IMPL(KernelVgpuMgr *pKernelVgpuMgr)
{
}

//
// Get max instance for a vgpu profile.
//
NV_STATUS
kvgpumgrGetMaxInstanceOfVgpu(NvU32 vgpuTypeId, NvU32 *maxInstanceVgpu)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
kvgpumgrGetPgpuIndex(KernelVgpuMgr *pKernelVgpuMgr, NvU32 gpuPciId, NvU32* index)
{
    return NV_ERR_OBJECT_NOT_FOUND;
}

NvBool
kvgpumgrIsHeterogeneousVgpuSupported(void)
{
    return NV_FALSE;
}

NV_STATUS
kvgpumgrCheckVgpuTypeCreatable(KERNEL_PHYS_GPU_INFO *pPhysGpuInfo, VGPU_TYPE *vgpuTypeInfo)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
kvgpumgrGetCreatableVgpuTypes(OBJGPU *pGpu, KernelVgpuMgr *pKernelVgpuMgr, NvU32 pgpuIndex, NvU32* numVgpuTypes, NvU32* vgpuTypes)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
kvgpumgrGetVgpuTypeInfo(NvU32 vgpuTypeId, VGPU_TYPE **vgpuType)
{
    return NV_ERR_OBJECT_NOT_FOUND;
}

NV_STATUS
kvgpumgrSendAllVgpuTypesToGsp(OBJGPU *pGpu)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
kvgpumgrPgpuAddVgpuType
(
    OBJGPU *pGpu,
    NvBool discardVgpuTypes,
    NVA081_CTRL_VGPU_INFO *pVgpuInfo
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
kvgpumgrAttachGpu(NvU32 gpuPciId)
{
    return NV_OK;
}

NV_STATUS
kvgpumgrDetachGpu(NvU32 gpuPciId)
{
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
    return NV_ERR_NOT_SUPPORTED;
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
                      KERNEL_HOST_VGPU_DEVICE **ppKernelHostVgpuDevice)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
kvgpumgrGuestUnregister(OBJGPU *pGpu, KERNEL_HOST_VGPU_DEVICE *pKernelHostVgpuDevice)
{
    return NV_ERR_NOT_SUPPORTED;
}

//
// Helper function to check if pGPU is live migration capable.
//
NvBool
kvgpumgrCheckPgpuMigrationSupport(OBJGPU *pGpu)
{
    return NV_FALSE;
}

NvU32 kvgpumgrGetPgpuDevIdEncoding(OBJGPU *pGpu, NvU8 *pgpuString,
                                   NvU32 strSize)
{
    return NV_U32_MAX;
}

NvU32 kvgpumgrGetPgpuSubdevIdEncoding(OBJGPU *pGpu, NvU8 *pgpuString,
                                      NvU32 strSize)
{
    return NV_U32_MAX;
}

NvU32 kvgpumgrGetPgpuFSEncoding(OBJGPU *pGpu, NvU8 *pgpuString,
                                NvU32 strSize)
{
    return NV_U32_MAX;
}

//
// A 32-bit variable is used to consolidate various GPU capabilities like
// ECC, SRIOV etc. The function sets the capabilities in the variable and
// converted to an ascii-encoded format.
//
NvU32 kvgpumgrGetPgpuCapEncoding(OBJGPU *pGpu, NvU8 *pgpuString, NvU32 strSize)
{
    return NV_U32_MAX;
}

/*
 * Get the user provide vGPU version range
 */
NV_STATUS
kvgpumgrGetHostVgpuVersion(NvU32 *user_min_supported_version,
                           NvU32 *user_max_supported_version)
{
    return NV_ERR_NOT_SUPPORTED;
}

/*
 * Set the user provide vGPU version range
 */
NV_STATUS
kvgpumgrSetHostVgpuVersion(NvU32 user_min_supported_version,
                           NvU32 user_max_supported_version)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
kvgpumgrGetSwizzId(OBJGPU *pGpu,
                   KERNEL_PHYS_GPU_INFO *pPhysGpuInfo,
                   NvU32 partitionFlag,
                   NvU32 *swizzId)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
kvgpumgrValidateSwizzId(OBJGPU *pGpu,
                        NvU32 vgpuTypeId,
                        NvU32 swizzId)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
kvgpumgrGetPartitionFlag(NvU32 vgpuTypeId, NvU32 *partitionFlag)
{
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
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
kvgpumgrEnumerateVgpuPerPgpu(OBJGPU *pGpu, NV2080_CTRL_VGPU_MGR_INTERNAL_ENUMERATE_VGPU_PER_PGPU_PARAMS *pParams)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
kvgpumgrClearGuestVmInfo(OBJGPU *pGpu, KERNEL_HOST_VGPU_DEVICE *pKernelHostVgpuDevice)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
kvgpumgrGetVgpuFbUsage(OBJGPU *pGpu, NVA081_CTRL_VGPU_CONFIG_GET_VGPU_FB_USAGE_PARAMS *pParams)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
kvgpumgrSetVgpuEncoderCapacity(OBJGPU *pGpu, NvU8 *vgpuUuid, NvU32 encoderCapacity)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
kvgpumgrStart(const NvU8 *pMdevUuid, void *waitQueue, NvS32 *returnStatus,
              NvU8 *vmName, NvU32 qemuPid)
{
    return NV_ERR_OBJECT_NOT_FOUND;
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
kvgpumgrCreateRequestVgpu(NvU32 gpuPciId, const NvU8 *pMdevUuid,
                          NvU32 vgpuTypeId, NvU16 *vgpuId, NvU32 gpuPciBdf)
{
    return NV_ERR_NOT_SUPPORTED;
}

//
// Delete REQUEST_VGPU_INFO_NODE structure from list.
// REQUEST_VGPU_INFO_NODE is currently used only for vGPU on KVM.
//
NV_STATUS
kvgpumgrDeleteRequestVgpu(const NvU8 *pMdevUuid, NvU16 vgpuId)
{
    return NV_ERR_OBJECT_NOT_FOUND;
}

NV_STATUS
kvgpumgrGetHostVgpuDeviceFromMdevUuid(NvU32 gpuPciId, const NvU8 *pMdevUuid,
                                      KERNEL_HOST_VGPU_DEVICE **ppKernelHostVgpuDevice)
{
    return NV_ERR_OBJECT_NOT_FOUND;
}

NV_STATUS
kvgpumgrGetHostVgpuDeviceFromVgpuUuid(NvU32 gpuPciId, NvU8 *vgpuUuid,
                                  KERNEL_HOST_VGPU_DEVICE **ppKernelHostVgpuDevice)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
kvgpumgrGetConfigEventInfoFromDb(NvHandle hClient,
                                 NvHandle hVgpuConfig,
                                 VGPU_CONFIG_EVENT_INFO_NODE **ppVgpuConfigEventInfoNode,
                                 NvU32 pgpuIndex)
{
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
    return NV_ERR_NOT_SUPPORTED;
}
