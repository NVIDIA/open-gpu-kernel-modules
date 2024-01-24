/*
 * SPDX-FileCopyrightText: Copyright (c) 2016-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/******************************************************************************
*
*   Description:
*       This implements functions of the base class for gpu resources.
*
******************************************************************************/

#include "core/core.h"
#include "os/os.h"
#include "resserv/resserv.h"
#include "resserv/rs_server.h"
#include "resserv/rs_client.h"
#include "resserv/rs_resource.h"
#include "rmapi/client.h"
#include "rmapi/resource.h"
#include "gpu/gpu.h"
#include "gpu/gpu_resource.h"
#include "gpu/device/device.h"
#include "gpu/subdevice/subdevice.h"
#include "gpu_mgr/gpu_mgr.h"
#include "virtualization/hypervisor/hypervisor.h"

#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"

#include "g_allclasses.h"

NV_STATUS
gpuresConstruct_IMPL
(
    GpuResource *pGpuResource,
    CALL_CONTEXT *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    RsResourceRef *pResourceRef = pCallContext->pResourceRef;
    RsResourceRef *pDeviceRef;
    RsResourceRef *pSubdeviceRef;
    OBJGPU        *pGpu = NULL;
    NvBool         bBcResource = NV_TRUE;
    NV_STATUS      status;

    // Check if instance is a subdevice
    pGpuResource->pSubdevice = dynamicCast(pGpuResource, Subdevice);

    // Else check for ancestor
    if (!pGpuResource->pSubdevice)
    {
        status = refFindAncestorOfType(pResourceRef, classId(Subdevice), &pSubdeviceRef);
        if (status == NV_OK)
            pGpuResource->pSubdevice = dynamicCast(pSubdeviceRef->pResource, Subdevice);
    }

    // Check if instance is a device
    pGpuResource->pDevice = dynamicCast(pGpuResource, Device);

    // Else check for ancestor
    if (!pGpuResource->pDevice)
    {
        status = refFindAncestorOfType(pResourceRef, classId(Device), &pDeviceRef);
        if (status == NV_OK)
            pGpuResource->pDevice = dynamicCast(pDeviceRef->pResource, Device);
    }

    if (RS_IS_COPY_CTOR(pParams))
        return gpuresCopyConstruct(pGpuResource, pCallContext, pParams);

    // Fails during device/subdevice ctor. Subclass ctor calls gpuresSetGpu
    status = gpuGetByRef(pResourceRef, &bBcResource, &pGpu);
    if (status == NV_OK)
        gpuresSetGpu(pGpuResource, pGpu, bBcResource);

    return NV_OK;
}

NV_STATUS
gpuresCopyConstruct_IMPL
(
    GpuResource *pGpuResource,
    CALL_CONTEXT *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    GpuResource *pGpuResourceSrc = dynamicCast(pParams->pSrcRef->pResource, GpuResource);

    if (pGpuResourceSrc == NULL)
        return NV_ERR_INVALID_OBJECT;

    gpuresSetGpu(pGpuResource, pGpuResourceSrc->pGpu, pGpuResourceSrc->bBcResource);

    return NV_OK;
}

NV_STATUS
gpuresMap_IMPL
(
    GpuResource *pGpuResource,
    CALL_CONTEXT *pCallContext,
    RS_CPU_MAP_PARAMS *pParams,
    RsCpuMapping *pCpuMapping
)
{
    OBJGPU *pGpu;
    NvU32 offset, size;
    NV_STATUS rmStatus;
    NvBool bBroadcast;

    pGpu = CliGetGpuFromContext(pCpuMapping->pContextRef, &bBroadcast);
    NV_ASSERT_OR_RETURN(pGpu != NULL, NV_ERR_INVALID_ARGUMENT);
    gpuSetThreadBcState(pGpu, bBroadcast);

    rmStatus = gpuresGetRegBaseOffsetAndSize(pGpuResource, pGpu, &offset, &size);
    if (rmStatus != NV_OK)
        return rmStatus;

    rmStatus = rmapiMapGpuCommon(staticCast(pGpuResource, RsResource),
                                 pCallContext,
                                 pCpuMapping,
                                 pGpu,
                                 offset,
                                 size);
    pCpuMapping->processId = osGetCurrentProcess();

    if (pParams->ppCpuVirtAddr)
        *pParams->ppCpuVirtAddr = pCpuMapping->pLinearAddress;

    return rmStatus;
}

NV_STATUS
gpuresUnmap_IMPL
(
    GpuResource *pGpuResource,
    CALL_CONTEXT *pCallContext,
    RsCpuMapping *pCpuMapping
)
{
    RmClient *pClient = dynamicCast(pCallContext->pClient, RmClient);
    OBJGPU *pGpu;
    NvBool bBroadcast;

    pGpu = CliGetGpuFromContext(pCpuMapping->pContextRef, &bBroadcast);
    NV_ASSERT_OR_RETURN(pGpu != NULL, NV_ERR_INVALID_ARGUMENT);
    gpuSetThreadBcState(pGpu, bBroadcast);

    osUnmapGPU(pGpu->pOsGpuInfo,
               rmclientGetCachedPrivilege(pClient),
               pCpuMapping->pLinearAddress,
               pCpuMapping->length,
               pCpuMapping->pPrivate->pPriv);

    return NV_OK;
}

NvBool
gpuresShareCallback_IMPL
(
    GpuResource *pGpuResource,
    RsClient *pInvokingClient,
    RsResourceRef *pParentRef,
    RS_SHARE_POLICY *pSharePolicy
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pGpuResource);
    NvBool bMIGInUse = IS_MIG_IN_USE(pGpu);
    NvU16 shareType = pSharePolicy->type;

    if ((shareType == RS_SHARE_TYPE_SMC_PARTITION) && !bMIGInUse)
    {
        // When MIG is not enabled, ignore Require restrictions
        if (pSharePolicy->action & RS_SHARE_ACTION_FLAG_REQUIRE)
            return NV_TRUE;

        // Fallback if feature is not available
        shareType = RS_SHARE_TYPE_GPU;
    }

    switch (shareType)
    {
        case RS_SHARE_TYPE_SMC_PARTITION:
        {
            if (RS_ACCESS_MASK_TEST(&pSharePolicy->accessMask, RS_ACCESS_DUP_OBJECT))
            {
                // Special exceptions only for Dup
                RsResourceRef *pSrcRef = RES_GET_REF(pGpuResource);

                switch (pSrcRef->externalClassId)
                {
                    //
                    // XXX Bug 2815350: We exempt NV01_MEMORY_LOCAL_USER and NV01_MEMORY_SYSTEM because there is a WAR
                    // in place for these classes in memCopyConstruct. The WAR can only be removed after bug 2815350 is
                    // resolved. Once bug 2815350 is resolved and WAR removed, we can apply the partition check to
                    // NV01_MEMORY_LOCAL_USER and NV01_MEMORY_SYSTEM as well.
                    //
                    case NV01_MEMORY_LOCAL_USER:
                    case NV01_MEMORY_SYSTEM:
                    //
                    // We also exempt this check for cases when KernelHostVgpuDeviceApi(NVA084_KERNEL_HOST_VGPU_DEVICE) is being duped
                    //  by plugins under every client it creates for itself or guest.
                    //
                    case NVA084_KERNEL_HOST_VGPU_DEVICE:
                        return NV_TRUE;
                    //
                    // We exempt this check for cases when a kernel client is trying to dup AMPERE_SMC_PARTITION_REF object.
                    // UVM dups AMPERE_SMC_PARTITION_REF from its user's client. see Bug 2826455 for details
                    //
                    case AMPERE_SMC_PARTITION_REF:
                    {
                        RmClient *pRmClient = dynamicCast(pInvokingClient, RmClient);
                        RS_PRIV_LEVEL privLevel = RS_PRIV_LEVEL_USER;
                        NvBool bOverrideDupAllow = NV_FALSE;

                        if (pRmClient != NULL)
                        {
                            privLevel = rmclientGetCachedPrivilege(pRmClient);
                        }

                        if ((privLevel >= RS_PRIV_LEVEL_KERNEL) || bOverrideDupAllow)
                            return NV_TRUE;

                        break;
                    }
                }

            }

            {
                KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
                MIG_INSTANCE_REF refClient;
                MIG_INSTANCE_REF refResource;
                RsResourceRef *pInvokingDeviceRef;
                Device *pInvokingDevice;

                NV_ASSERT_OR_RETURN(pParentRef != NULL, NV_ERR_INVALID_ARGUMENT);

                if (pParentRef->internalClassId == classId(Device))
                {
                    pInvokingDeviceRef = pParentRef;
                }
                else
                {
                    NV_ASSERT_OK_OR_RETURN(
                        refFindAncestorOfType(pParentRef, classId(Device), &pInvokingDeviceRef));
                }

                pInvokingDevice = dynamicCast(pInvokingDeviceRef->pResource, Device);

                if (bMIGInUse &&
                    (kmigmgrGetInstanceRefFromDevice(pGpu, pKernelMIGManager, pInvokingDevice,
                                                     &refClient) == NV_OK) &&
                    (kmigmgrGetInstanceRefFromDevice(pGpu, pKernelMIGManager, GPU_RES_GET_DEVICE(pGpuResource),
                                                     &refResource) == NV_OK))
                {
                    // Ignore execution partition differences when sharing
                    refClient = kmigmgrMakeGIReference(refClient.pKernelMIGGpuInstance);
                    refResource = kmigmgrMakeGIReference(refResource.pKernelMIGGpuInstance);
                    if (kmigmgrAreMIGReferencesSame(&refClient, &refResource))
                        return NV_TRUE;
                }
            }
            break;
        }
        case RS_SHARE_TYPE_GPU:
        {
            RsResourceRef *pDeviceAncestorRef;
            RsResourceRef *pParentDeviceAncestorRef;

            // This share type only works when called from dup
            if (pParentRef == NULL)
                break;

            if (pParentRef->internalClassId == classId(Device))
            {
                // pParentRef is allowed to itself be the Device ancestor
                pParentDeviceAncestorRef = pParentRef;
            }
            else
            {
                // If pParentRef is not itself the device, try to find a Device ancestor. If none exist, fail.
                if (refFindAncestorOfType(pParentRef, classId(Device), &pParentDeviceAncestorRef) != NV_OK)
                    break;
            }
            // Check that the source resource's ancestor device instance matches the destination parent's device instance
            if (refFindAncestorOfType(RES_GET_REF(pGpuResource), classId(Device), &pDeviceAncestorRef) == NV_OK)
            {
                Device *pDevice = dynamicCast(pDeviceAncestorRef->pResource, Device);
                Device *pParentDevice = dynamicCast(pParentDeviceAncestorRef->pResource, Device);

                if ((pDevice != NULL) && (pParentDevice != NULL) &&
                    (pDevice->deviceInst == pParentDevice->deviceInst))
                {
                    return NV_TRUE;
                }
            }
        }
    }

    // Delegate to superclass
    return rmresShareCallback_IMPL(staticCast(pGpuResource, RmResource), pInvokingClient, pParentRef, pSharePolicy);
}

NV_STATUS
gpuresGetRegBaseOffsetAndSize_IMPL
(
    GpuResource *pGpuResource,
    OBJGPU *pGpu,
    NvU32 *pOffset,
    NvU32 *pSize
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
gpuresGetMapAddrSpace_IMPL
(
    GpuResource *pGpuResource,
    CALL_CONTEXT *pCallContext,
    NvU32 mapFlags,
    NV_ADDRESS_SPACE *pAddrSpace
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pGpuResource);
    NV_STATUS status;
    NvU32 offset;
    NvU32 size;

    // Default to REGMEM if the GPU resource has a register base and offset defined
    status = gpuresGetRegBaseOffsetAndSize(pGpuResource, pGpu, &offset, &size);
    if (status != NV_OK)
        return status;

    if (pAddrSpace)
        *pAddrSpace = ADDR_REGMEM;

    return NV_OK;
}

/*!
 * @brief Forward a control call to the Physical RM portion of this API.
 */
NV_STATUS
gpuresInternalControlForward_IMPL
(
    GpuResource *pGpuResource,
    NvU32 command,
    void *pParams,
    NvU32 size
)
{
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(GPU_RES_GET_GPU(pGpuResource));
    return pRmApi->Control(pRmApi,
                           RES_GET_CLIENT_HANDLE(pGpuResource),
                           gpuresGetInternalObjectHandle(pGpuResource),
                           command,
                           pParams,
                           size);
}

/*!
 * @brief Retrieve the handle associated with the Physical RM portion of the API.
 *        For non-split object, this is the same as the handle of the object.
 */
NvHandle
gpuresGetInternalObjectHandle_IMPL(GpuResource *pGpuResource)
{
    return RES_GET_HANDLE(pGpuResource);
}

NV_STATUS
gpuresControl_IMPL
(
    GpuResource                    *pGpuResource,
    CALL_CONTEXT                   *pCallContext,
    RS_RES_CONTROL_PARAMS_INTERNAL *pParams
)
{
    NV_ASSERT_OR_RETURN(pGpuResource->pGpu != NULL, NV_ERR_INVALID_STATE);
    gpuresControlSetup(pParams, pGpuResource);

    return resControl_IMPL(staticCast(pGpuResource, RsResource),
                           pCallContext, pParams);
}

void
gpuresControlSetup_IMPL
(
    RS_RES_CONTROL_PARAMS_INTERNAL *pParams,
    GpuResource *pGpuResource
)
{
    RmCtrlParams *pRmCtrlParams = pParams->pLegacyParams;
    pRmCtrlParams->pGpu = pGpuResource->pGpu;

    GPU_RES_SET_THREAD_BC_STATE(pGpuResource);
}

void
gpuresSetGpu_IMPL
(
    GpuResource *pGpuResource,
    OBJGPU *pGpu,
    NvBool bBcResource
)
{
    if (pGpu != NULL)
    {
        RmResource *pResource = staticCast(pGpuResource, RmResource);
        pResource->rpcGpuInstance = gpuGetInstance(pGpu);
        pGpuResource->pGpu = pGpu;
        pGpuResource->pGpuGrp = gpumgrGetGpuGrpFromGpu(pGpuResource->pGpu);
        pGpuResource->bBcResource = bBcResource;
        gpuSetThreadBcState(pGpu, bBcResource);
    }
}

NV_STATUS
gpuresGetByHandle_IMPL
(
    RsClient     *pClient,
    NvHandle      hResource,
    GpuResource **ppGpuResource
)
{
    RsResourceRef  *pResourceRef;
    NV_STATUS       status;

    *ppGpuResource = NULL;

    status = clientGetResourceRef(pClient, hResource, &pResourceRef);
    if (status != NV_OK)
        return status;

    *ppGpuResource = dynamicCast(pResourceRef->pResource, GpuResource);

    return (*ppGpuResource) ? NV_OK : NV_ERR_INVALID_OBJECT_HANDLE;
}

NV_STATUS
gpuresGetByDeviceOrSubdeviceHandle
(
    RsClient     *pClient,
    NvHandle      hResource,
    GpuResource **ppGpuResource
)
{
    NV_STATUS status;

    status = gpuresGetByHandle(pClient, hResource, ppGpuResource);

    if (status != NV_OK)
        return status;

    // Must be device or subdevice
    if (!dynamicCast(*ppGpuResource, Device) &&
        !dynamicCast(*ppGpuResource, Subdevice))
    {
        return NV_ERR_INVALID_OBJECT_HANDLE;
    }

    return NV_OK;
}
