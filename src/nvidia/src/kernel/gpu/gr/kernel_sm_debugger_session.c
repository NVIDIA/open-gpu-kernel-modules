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

#define NVOC_KERNEL_SM_DEBUGGER_SESSION_H_PRIVATE_ACCESS_ALLOWED

#include "kernel/os/os.h"
#include "kernel/core/locks.h"
#include "kernel/gpu/gr/kernel_sm_debugger_session.h"
#include "kernel/gpu/gr/kernel_graphics_object.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"
#include "kernel/gpu/subdevice/subdevice.h"
#include "kernel/gpu/device/device.h"
#include "libraries/resserv/rs_client.h"
#include "kernel/rmapi/rs_utils.h"
#include "virtualization/hypervisor/hypervisor.h"

#include "ctrl/ctrl83de/ctrl83dedebug.h"

#include "class/cl0080.h"
#include "class/clc637.h"
#include "class/cl2080.h"
#include "class/cl83de.h"

// Macro to validate two clients having the same security tokens
#define VALIDATE_MATCHING_SEC_TOKENS(handle1, handle2, secInfo, status)                         \
    do {                                                                                        \
        OBJSYS *pSys = SYS_GET_INSTANCE();                                                      \
        if (pSys->getProperty(pSys, PDB_PROP_SYS_VALIDATE_CLIENT_HANDLE) &&                     \
            ((secInfo).privLevel < RS_PRIV_LEVEL_USER_ROOT))                                    \
        {                                                                                       \
            status = osValidateClientTokens((void*)rmclientGetSecurityTokenByHandle(handle1),   \
                                            (void*)rmclientGetSecurityTokenByHandle(handle2));  \
            NV_ASSERT_OR_RETURN(status == NV_OK, status);                                       \
        }                                                                                       \
    } while (0);

static NV_STATUS _ShareDebugger(KernelSMDebuggerSession *, RsResourceRef *, RsResourceRef *);

void
dbgSessionRemoveDependant_IMPL
(
    RmDebuggerSession *pDbgSession,
    RsResourceRef     *pResourceRef
)
{
    RsSession *pSession = dynamicCast(pDbgSession, RsSession);

    // Freeing a KernelSMDebuggerSession dependant should just call the destructor normally
    if (pSession->bValid && (pResourceRef->externalClassId == GT200_DEBUGGER))
        ksmdbgssnFreeCallback(dynamicCast(pResourceRef->pResource, KernelSMDebuggerSession));

    sessionRemoveDependant_IMPL(staticCast(pDbgSession, RsSession), pResourceRef);
}

void
dbgSessionRemoveDependency_IMPL
(
    RmDebuggerSession *pDbgSession,
    RsResourceRef     *pResourceRef
)
{
    RsSession *pSession = dynamicCast(pDbgSession, RsSession);

    //
    // Call all registered KernelSMDebuggerSessions' free callbacks (destructor basically)
    // when the underlying KernelGraphicsObject goes away. This invalidates the KernelSMDebuggerSession
    // and causes all control calls on it to fail since the KernelGraphicsObject dependancy has disappeared.
    //
    if (pSession->bValid)
    {
        RsResourceRefListIter it;

        it = listIterAll(&pSession->dependants);

        while (listIterNext(&it))
        {
            RsResourceRef *pDependency = *(it.pValue);

            if (pDependency->externalClassId == GT200_DEBUGGER)
                ksmdbgssnFreeCallback(dynamicCast(pDependency->pResource, KernelSMDebuggerSession));
        }
    }

    // This call will invalidate the RmDebuggerSession
    sessionRemoveDependency_IMPL(staticCast(pDbgSession, RsSession), pResourceRef);
}

static NV_STATUS
_ksmdbgssnInitClient
(
    OBJGPU *pGpu,
    KernelSMDebuggerSession *pKernelSMDebuggerSession
)
{
    NV0080_ALLOC_PARAMETERS nv0080AllocParams;
    NV2080_ALLOC_PARAMETERS nv2080AllocParams;
    NV_STATUS status = NV_OK;
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    NvBool bMIGInUse = IS_MIG_IN_USE(pGpu);

    pKernelSMDebuggerSession->hInternalClient = NV01_NULL_OBJECT;

    // Allocate a (kernel-space) client.
    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
        pRmApi->AllocWithHandle(pRmApi,
                                NV01_NULL_OBJECT,
                                NV01_NULL_OBJECT,
                                NV01_NULL_OBJECT,
                                NV01_ROOT,
                                &pKernelSMDebuggerSession->hInternalClient,
                                sizeof(pKernelSMDebuggerSession->hInternalClient)),
        failed);

    // Allocate a device.
    NV_ASSERT_OK_OR_GOTO(status,
        serverutilGenResourceHandle(pKernelSMDebuggerSession->hInternalClient,
                                    &pKernelSMDebuggerSession->hInternalDevice),
        failed);
    portMemSet(&nv0080AllocParams, 0, sizeof(nv0080AllocParams));
    nv0080AllocParams.deviceId = gpuGetDeviceInstance(pGpu);
    nv0080AllocParams.hClientShare = pKernelSMDebuggerSession->hInternalClient;
    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
        pRmApi->AllocWithHandle(pRmApi,
                                pKernelSMDebuggerSession->hInternalClient,
                                pKernelSMDebuggerSession->hInternalClient,
                                pKernelSMDebuggerSession->hInternalDevice,
                                NV01_DEVICE_0,
                                &nv0080AllocParams,
                                sizeof(nv0080AllocParams)),
        failed);

    // Allocate a subdevice.
    NV_ASSERT_OK_OR_GOTO(status,
        serverutilGenResourceHandle(pKernelSMDebuggerSession->hInternalClient,
                                    &pKernelSMDebuggerSession->hInternalSubdevice),
        failed);
    portMemSet(&nv2080AllocParams, 0, sizeof(nv2080AllocParams));
    nv2080AllocParams.subDeviceId = gpumgrGetSubDeviceInstanceFromGpu(pGpu);
    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
        pRmApi->AllocWithHandle(pRmApi,
                                pKernelSMDebuggerSession->hInternalClient,
                                pKernelSMDebuggerSession->hInternalDevice,
                                pKernelSMDebuggerSession->hInternalSubdevice,
                                NV20_SUBDEVICE_0,
                                &nv2080AllocParams,
                                sizeof(nv2080AllocParams)),
        failed);

    if (bMIGInUse)
    {
        NVC637_ALLOCATION_PARAMETERS nvC637AllocParams;
        MIG_INSTANCE_REF ref;
        Device *pDevice = GPU_RES_GET_DEVICE(pKernelSMDebuggerSession);

        portMemSet(&nvC637AllocParams, 0, sizeof(nvC637AllocParams));
        NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
            kmigmgrGetInstanceRefFromDevice(pGpu, pKernelMIGManager, pDevice, &ref),
            failed);

        NV_ASSERT_OK_OR_GOTO(status,
            serverutilGenResourceHandle(pKernelSMDebuggerSession->hInternalClient,
                                        &pKernelSMDebuggerSession->hInternalSubscription),
            failed);
        nvC637AllocParams.swizzId = ref.pKernelMIGGpuInstance->swizzId;
        NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
            pRmApi->AllocWithHandle(pRmApi,
                                    pKernelSMDebuggerSession->hInternalClient,
                                    pKernelSMDebuggerSession->hInternalSubdevice,
                                    pKernelSMDebuggerSession->hInternalSubscription,
                                    AMPERE_SMC_PARTITION_REF,
                                    &nvC637AllocParams,
                                    sizeof(nvC637AllocParams)),
            failed);
    }

    return NV_OK;
failed:
    if (pKernelSMDebuggerSession->hInternalClient != NV01_NULL_OBJECT)
    {
        pRmApi->Free(pRmApi, pKernelSMDebuggerSession->hInternalClient, pKernelSMDebuggerSession->hInternalClient);
        pKernelSMDebuggerSession->hInternalClient = NV01_NULL_OBJECT;
        pKernelSMDebuggerSession->hInternalDevice = NV01_NULL_OBJECT;
        pKernelSMDebuggerSession->hInternalSubdevice = NV01_NULL_OBJECT;
        pKernelSMDebuggerSession->hInternalSubscription = NV01_NULL_OBJECT;
        pKernelSMDebuggerSession->hInternalMemMapping = NV01_NULL_OBJECT;
    }

    return status;
}

NV_STATUS
ksmdbgssnConstruct_IMPL
(
    KernelSMDebuggerSession      *pKernelSMDebuggerSession,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    NV83DE_ALLOC_PARAMETERS  *pNv83deAllocParams = pParams->pAllocParams;
    OBJGPU                   *pGpu;
    NvHandle                  hAppChannel;
    NvHandle                  hAppClient;
    NvHandle                  hClass3dObject;
    NvHandle                  hKernelSMDebuggerSession;
    NvHandle                  hSubdevice;
    NV_STATUS                 status = NV_OK;
    RsClient                 *pAppClient;
    Device                   *pAppDevice;
    Subdevice                *pSubdevice;
    RsResourceRef            *pGrResourceRef;
    RsResourceRef            *pParentRef;

    // The app using the new interface should initialize this to 0.
    if (pNv83deAllocParams->hDebuggerClient_Obsolete)
    {
        NV_ASSERT_FAILED("Old Nv83deAllocParams interface not supported");
        return NV_ERR_INVALID_ARGUMENT;
    }

    hAppClient = pNv83deAllocParams->hAppClient;
    hClass3dObject = pNv83deAllocParams->hClass3dObject;
    hKernelSMDebuggerSession = pParams->hResource;

    // If given a zero hAppClient, assume the client meant to target the calling hClient.
    if (hAppClient == NV01_NULL_OBJECT)
    {
        hAppClient = pParams->hClient;
    }

    // Validate + lookup the application client
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        serverGetClientUnderLock(&g_resServ, hAppClient, &pAppClient));

    status = clientGetResourceRef(pAppClient, hClass3dObject, &pGrResourceRef);

    if (status != NV_OK)
    {
       NV_PRINTF(LEVEL_ERROR, "hObject 0x%x not found for client 0x%x\n",
                 pParams->hParent, pParams->hClient);
       return NV_ERR_INVALID_OBJECT_PARENT;
    }

    //
    // On GSP, the security token is either the GFID or NULL. When the security token is
    // set to the GFID this will properly constrain debugger access to wtihin a single
    // Guest on GHV environments. When it is NULL, this allows access to any client in
    // the system but in order to take advantage of this CPU-RM would already have
    // to have been compromised anyway.
    //
    // On legacy vGPU systems, the security token will match the Guest's vGPU plugin. So you'd
    // only be able to access other resources allocated by the same Guest.
    //
    if (RMCFG_FEATURE_PLATFORM_GSP || hypervisorIsVgxHyper())
    {
        API_SECURITY_INFO *pSecInfo = pParams->pSecInfo;

        VALIDATE_MATCHING_SEC_TOKENS((pCallContext->pClient->hClient), hAppClient,
                                      *pSecInfo, status);
    }
    else
    {
        RS_ACCESS_MASK debugAccessMask;

        //
        // On CPU-RM and Guest RM systems check that debugging rights were shared.
        //
        // Check that the application client allowed debugging rights for the debugger
        // client on the compute object (i.e. the current client allocating this object).
        //
        //
        RS_ACCESS_MASK_CLEAR(&debugAccessMask);
        RS_ACCESS_MASK_ADD(&debugAccessMask, RS_ACCESS_DEBUG);

        status = rsAccessCheckRights(pGrResourceRef, pCallContext->pClient,
                                     &debugAccessMask);

        NV_CHECK_OR_ELSE(LEVEL_ERROR,
                status == NV_OK,
                NV_PRINTF(LEVEL_ERROR, "Current user does not have debugging rights on the compute object. Status = 0x%x\n", status);
                return NV_ERR_INSUFFICIENT_PERMISSIONS;);
    }

    pKernelSMDebuggerSession->pObject = dynamicCast(pGrResourceRef->pResource, KernelGraphicsObject);
    if (pKernelSMDebuggerSession->pObject == NULL)
    {
        return NV_ERR_INVALID_OBJECT;
    }

    pParentRef  = pGrResourceRef->pParentRef;
    hAppChannel = pParentRef->hResource;

    // Ensure that debugger session is created under same device as the object under debug
    pGpu = GPU_RES_GET_GPU(pKernelSMDebuggerSession);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, pGpu == GPU_RES_GET_GPU(pKernelSMDebuggerSession->pObject),
                           NV_ERR_INVALID_ARGUMENT);

    pAppDevice = GPU_RES_GET_DEVICE(pKernelSMDebuggerSession->pObject);

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        subdeviceGetByInstance(pAppClient,
                               RES_GET_HANDLE(pAppDevice),
                               gpumgrGetSubDeviceInstanceFromGpu(pGpu),
                               &pSubdevice));

    GPU_RES_SET_THREAD_BC_STATE(pSubdevice);

    hSubdevice = RES_GET_HANDLE(pSubdevice);

    // Initialize the object info
    pKernelSMDebuggerSession->hChannelClient  = pAppClient->hClient;
    pKernelSMDebuggerSession->hDebugger       = hKernelSMDebuggerSession;
    pKernelSMDebuggerSession->hDebuggerClient = pCallContext->pClient->hClient;
    pKernelSMDebuggerSession->hChannel        = hAppChannel;
    pKernelSMDebuggerSession->hSubdevice      = hSubdevice;

    // Insert it into this Object's debugger list
    if (!kgrctxRegisterKernelSMDebuggerSession(pGpu, kgrobjGetKernelGraphicsContext(pGpu, pKernelSMDebuggerSession->pObject), pKernelSMDebuggerSession))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Failed to insert Debugger into channel list, handle = 0x%x\n",
                  pKernelSMDebuggerSession->hDebugger);
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        _ksmdbgssnInitClient(pGpu, pKernelSMDebuggerSession));

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        _ShareDebugger(pKernelSMDebuggerSession, pCallContext->pResourceRef, pGrResourceRef));

    return status;
}

/**
 * @brief Binds a debugger object to the given KernelGraphicsObject's RsSession object
 *        or allocates a new one if it's not currently referencing one.
 *
 * @param[in]     pKernelSMDebuggerSession Underlying debugger object
 * @param[in,out] pGrResourceRef  RsResourceRef for the channel that will be
 *                                bound to an RsSession if one isn't already
 *                                there.
 * @param[in,out] pDebuggerRef    RsResourceRef for the debugger object that will
 *                                be bound to a new RsSession or the channel's
 *                                existing one.
 *
 * @return NV_OK on success, error code on failure
 */
static NV_STATUS
_ShareDebugger
(
    KernelSMDebuggerSession *pKernelSMDebuggerSession,
    RsResourceRef           *pDebuggerRef,
    RsResourceRef           *pGrResourceRef
)
{
    NV_STATUS  status = NV_OK;
    RsSession *pRsSession;

    // Allocate a new RsSession if the KernelGraphicsObject doesn't reference one already
    if (pGrResourceRef->pDependantSession == NULL)
    {
        RsShared *pShared = NULL;

        status = serverAllocShare(&g_resServ, classInfo(RmDebuggerSession), &pShared);
        if (status != NV_OK)
            return status;

        pKernelSMDebuggerSession->pDebugSession = dynamicCast(pShared, RmDebuggerSession);
        pRsSession = staticCast(pKernelSMDebuggerSession->pDebugSession, RsSession);

        // Add KernelGraphicsObject as a dependency
        sessionAddDependency(pRsSession, pGrResourceRef);

        // Add debugger object as a dependant of the new RsSession object
        sessionAddDependant(pRsSession, pDebuggerRef);

        //
        // Decrease ref count if newly allocated, we only want the ref
        // count for the RsSession object to be 2 in this case
        //
        serverFreeShare(&g_resServ, pShared);
    }
    else
    {
        pKernelSMDebuggerSession->pDebugSession = dynamicCast(pGrResourceRef->pDependantSession,
                                                     RmDebuggerSession);

        if (pKernelSMDebuggerSession->pDebugSession == NULL)
        {
            NV_PRINTF(LEVEL_ERROR, "KernelGraphicsObject already a dependent of a non-debugger session\n");
            return NV_ERR_INVALID_STATE;
        }

        // Add debugger object as a dependant of the existing RsSession object
        pRsSession = staticCast(pKernelSMDebuggerSession->pDebugSession, RsSession);
        sessionAddDependant(pRsSession, pDebuggerRef);
    }

    return NV_OK;
}

//
// Empty destructor since the destruction is done in the free callback which is invoked
// by Resource Server when the RmDebuggerSession shared object is invalidated due to either
// the KernelSMDebuggerSession being freed or the underlying KernelGraphicsObject dependancy being freed.
//
void
ksmdbgssnDestruct_IMPL
(
    KernelSMDebuggerSession *pKernelSMDebuggerSession
)
{}

//
// The free callback will always be invoked before the destructor for either the KernelSMDebuggerSession
// (empty since we clean up here) and before the KernelGraphicsObject dependancy's destructor. This is a bit
// different from how other Resource Server classes clean up since there is a dependancy owned
// by a different RM client.
//
void
ksmdbgssnFreeCallback_IMPL
(
    KernelSMDebuggerSession *pKernelSMDebuggerSession
)
{
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    OBJGPU *pGpu = GPU_RES_GET_GPU(pKernelSMDebuggerSession);

    // This should free the entire hierarchy of objects.
    pRmApi->Free(pRmApi, pKernelSMDebuggerSession->hInternalClient, pKernelSMDebuggerSession->hInternalClient);

    // Remove it from the pObject debugger list
    kgrctxDeregisterKernelSMDebuggerSession(pGpu, kgrobjGetKernelGraphicsContext(pGpu, pKernelSMDebuggerSession->pObject), pKernelSMDebuggerSession);

}

NV_STATUS
ksmdbgssnInternalControlForward_IMPL
(
    KernelSMDebuggerSession *pKernelSMDebuggerSession,
    NvU32 command,
    void *pParams,
    NvU32 size
)
{
    return gpuresInternalControlForward_IMPL(staticCast(pKernelSMDebuggerSession, GpuResource), command, pParams, size);
}

NvHandle
ksmdbgssnGetInternalObjectHandle_IMPL(KernelSMDebuggerSession *pKernelSMDebuggerSession)
{
    return NV01_NULL_OBJECT;
}
