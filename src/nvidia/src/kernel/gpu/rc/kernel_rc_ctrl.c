/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "kernel/gpu/rc/kernel_rc.h"

#include "kernel/diagnostics/journal.h"
#include "kernel/gpu/subdevice/subdevice.h"
#include "kernel/rmapi/client.h"
#include "kernel/rmapi/client_resource.h"

#include "g_all_dcl_pb.h"
#include "lib/protobuf/prb_util.h"

NV_STATUS
subdeviceCtrlCmdRcReadVirtualMem_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_RC_READ_VIRTUAL_MEM_PARAMS *pReadVirtMemParam
)
{
    OBJGPU        *pGpu   = GPU_RES_GET_GPU(pSubdevice);
    NV_STATUS      status = NV_OK;
    KernelChannel *pKernelChannel;

    if (pReadVirtMemParam->bufferPtr == NvP64_NULL
        || (((NvU64)pReadVirtMemParam->bufferPtr) & 0x7) != 0
        || pReadVirtMemParam->bufferSize == 0)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (CliGetKernelChannelWithDevice(RES_GET_CLIENT(pSubdevice),
                                      RES_GET_PARENT_HANDLE(pSubdevice),
                                      pReadVirtMemParam->hChannel,
                                      &pKernelChannel) == NV_OK)
    {
        status = krcReadVirtMem(pGpu, GPU_GET_KERNEL_RC(pGpu),
                                pKernelChannel,
                                pReadVirtMemParam->virtAddress,
                                pReadVirtMemParam->bufferPtr,
                                pReadVirtMemParam->bufferSize);
    }
    else
    {
        status = NV_ERR_INVALID_ARGUMENT;
    }

    return status;
}


NV_STATUS
krcSubdeviceCtrlGetErrorInfoCheckPermissions_KERNEL
(
    KernelRc  *pKernelRc,
    Subdevice *pSubdevice
)
{
    OBJGPU       *pGpu         = GPU_RES_GET_GPU(pSubdevice);
    CALL_CONTEXT *pCallContext = resservGetTlsCallContext();

    NV_ASSERT_OR_RETURN(pCallContext != NULL, NV_ERR_INVALID_STATE);

    if (rmclientIsAdminByHandle(RES_GET_CLIENT_HANDLE(pSubdevice),
                                pCallContext->secInfo.privLevel))
    {
        return NV_OK;
    }

    if (IS_MIG_IN_USE(pGpu) &&
        rmclientIsCapableOrAdminByHandle(RES_GET_CLIENT_HANDLE(pSubdevice),
                                         NV_RM_CAP_SYS_SMC_MONITOR,
                                         pCallContext->secInfo.privLevel))
    {
        return NV_OK;
    }

    return NV_ERR_INSUFFICIENT_PERMISSIONS;
}


NV_STATUS
subdeviceCtrlCmdRcGetErrorCount_IMPL
(
    Subdevice                             *pSubdevice,
    NV2080_CTRL_RC_GET_ERROR_COUNT_PARAMS *pParams
)
{
    NV_STATUS  status    = NV_OK;
    OBJGPU    *pGpu      = GPU_RES_GET_GPU(pSubdevice);
    KernelRc  *pKernelRc = GPU_GET_KERNEL_RC(pGpu);

    status = krcSubdeviceCtrlGetErrorInfoCheckPermissions_HAL(pKernelRc,
                                                              pSubdevice);
    if (status != NV_OK)
    {
        return status;
    }

    if (IS_GSP_CLIENT(pGpu))
    {
        RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
        return pRmApi->Control(pRmApi,
                               RES_GET_CLIENT_HANDLE(pSubdevice),
                               RES_GET_HANDLE(pSubdevice),
                               NV2080_CTRL_CMD_RC_GET_ERROR_COUNT,
                               pParams,
                               sizeof *pParams);
    }

    return krcSubdeviceCtrlCmdRcGetErrorCount(pKernelRc, pSubdevice, pParams);
}


NV_STATUS
subdeviceCtrlCmdRcGetErrorV2_IMPL
(
    Subdevice                          *pSubdevice,
    NV2080_CTRL_RC_GET_ERROR_V2_PARAMS *pParams
)
{
    NV_STATUS  status    = NV_OK;
    OBJGPU    *pGpu      = GPU_RES_GET_GPU(pSubdevice);
    KernelRc  *pKernelRc = GPU_GET_KERNEL_RC(pGpu);

    status = krcSubdeviceCtrlGetErrorInfoCheckPermissions_HAL(pKernelRc,
                                                              pSubdevice);
    if (status != NV_OK)
    {
        return status;
    }

    if (IS_GSP_CLIENT(pGpu))
    {
        RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
        return pRmApi->Control(pRmApi,
                               RES_GET_CLIENT_HANDLE(pSubdevice),
                               RES_GET_HANDLE(pSubdevice),
                               NV2080_CTRL_CMD_RC_GET_ERROR_V2,
                               pParams,
                               sizeof *pParams);
    }

    return krcSubdeviceCtrlCmdRcGetErrorV2(pKernelRc, pSubdevice, pParams);
}


NV_STATUS
krcSubdeviceCtrlCmdRcGetErrorCount_IMPL
(
    KernelRc                              *pKernelRc,
    Subdevice                             *pSubdevice,
    NV2080_CTRL_RC_GET_ERROR_COUNT_PARAMS *pErrorCount
)
{
    OBJSYS         *pSys          = SYS_GET_INSTANCE();
    Journal        *pRcDB         = SYS_GET_RCDB(pSys);
    SYS_ERROR_INFO *pSysErrorInfo = &pRcDB->ErrorInfo;

    pErrorCount->errorCount = pSysErrorInfo->ErrorCount;

    return NV_OK;
}

NV_STATUS
krcSubdeviceCtrlCmdRcGetErrorV2_IMPL
(
    KernelRc                           *pKernelRc,
    Subdevice                          *pSubdevice,
    NV2080_CTRL_RC_GET_ERROR_V2_PARAMS *pErrorParams
)
{
    OBJSYS            *pSys          = SYS_GET_INSTANCE();
    Journal           *pRcDB         = SYS_GET_RCDB(pSys);
    SYS_ERROR_INFO    *pSysErrorInfo = &pRcDB->ErrorInfo;
    RmProtoBuf_RECORD *pProtoBuf     = (RmProtoBuf_RECORD *)
                                       pErrorParams->recordBuffer;
    NvU8 *pBuffer = pErrorParams->recordBuffer + sizeof(RmProtoBuf_RECORD);
    RMPRBERRORELEMENT_V2 *pPrbErrorElement;
    RMCD_ERROR_BLOCK     *pErrorBlock;
    PRB_ENCODER           prbEnc;
    NV_STATUS             status = NV_OK;
    NvU32                 i;

    // Set Output Size to zero
    pErrorParams->outputRecordSize = 0;

    ct_assert(sizeof(pErrorParams->recordBuffer) > sizeof(RmProtoBuf_RECORD));

    // Check error list
    if (pSysErrorInfo->pErrorList == NULL)
    {
        return status;
    }

    // Skip over records up to whichBuffer
    pPrbErrorElement = (RMPRBERRORELEMENT_V2 *)pSysErrorInfo->pErrorList;
    for (i = 0; (i < pErrorParams->whichBuffer) && (pPrbErrorElement != NULL);
         i++)
    {
        pPrbErrorElement = (RMPRBERRORELEMENT_V2 *)
                               pPrbErrorElement->ErrorHeader.pNextError;
    }
    if (pPrbErrorElement == NULL)
    {
        return status;
    }

    //
    // There's no data in a protobuf error element - just combine the error
    // blocks into a Dcl.DclMsg.
    //
    if (pPrbErrorElement->RmPrbErrorData.common.Header.cRecordType !=
        RmPrbErrorInfo_V2)
    {
        // Can only handle protobuf formatted messages
        NV_PRINTF(LEVEL_ERROR,
                  "NVRM-RC: unknown error element type: %d\n",
                  pPrbErrorElement->RmPrbErrorData.common.Header.cRecordType);
        return NV_ERR_NOT_SUPPORTED;
    }

    {
        NvU32 dwSize = sizeof(pErrorParams->recordBuffer) -
                       sizeof(RmProtoBuf_RECORD);
        prbEncStart(&prbEnc, DCL_ERRORBLOCK, pBuffer, dwSize, NULL);
    }

    // Add each Error Block
    for (pErrorBlock = pPrbErrorElement->ErrorHeader.pErrorBlock;
         pErrorBlock != NULL;
         pErrorBlock = pErrorBlock->pNext)
    {
        NV_STATUS status1;
        status = prbEncNestedStart(&prbEnc, DCL_ERRORBLOCK_DATA);
        if (status != NV_OK)
            break;

        status  = prbEncCatMsg(&prbEnc,
                              pErrorBlock->pBlock,
                              pErrorBlock->blockSize);
        status1 = prbEncNestedEnd(&prbEnc);
        if (status != NV_OK)
            break;

        if (status1 != NV_OK)
        {
            status = status1;
            break;
        }
    }

    {
        NvU8 *prbBuffer = NULL;
        NvU32 prbLen = prbEncFinish(&prbEnc, (void **)&prbBuffer);

        if (status == NV_OK)
        {
            pProtoBuf->Header.wRecordSize  = sizeof(RmProtoBuf_RECORD);
            pProtoBuf->Header.cRecordType  = RmProtoBuf;
            pProtoBuf->Header.cRecordGroup = RmGroup;
            pProtoBuf->dwSize              = prbLen;
        }

        pErrorParams->outputRecordSize = prbLen + sizeof(RmProtoBuf_RECORD);
    }

    return status;
}


NV_STATUS
krcCliresCtrlNvdGetRcerrRptCheckPermissions_KERNEL
(
    KernelRc                                 *pKernelRc,
    RmClientResource                         *pRmCliRes,
    NV0000_CTRL_CMD_NVD_GET_RCERR_RPT_PARAMS *pReportParams
)
{
    CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
    RmClient     *pClient = dynamicCast(RES_GET_CLIENT(pRmCliRes), RmClient);

    if (pClient == NULL)
    {
        return NV_ERR_OBJECT_NOT_FOUND;
    }

    //
    // Only a kernel client is allowed to query reports without filter, record
    // owners are assigned only on Windows.
    //
    if (pCallContext->secInfo.privLevel < RS_PRIV_LEVEL_KERNEL &&
        pReportParams->owner == RCDB_RCDIAG_DEFAULT_OWNER)
    {
        return NV_ERR_INSUFFICIENT_PERMISSIONS;
    }

    return NV_OK;
}

NV_STATUS subdeviceCtrlCmdGetRcRecovery_VF
(
    Subdevice *pSubdevice,
    NV2080_CTRL_CMD_RC_RECOVERY_PARAMS *pRcRecovery
)
{
    pRcRecovery->rcEnable = NV2080_CTRL_CMD_RC_RECOVERY_DISABLED;

    return NV_OK;
}
