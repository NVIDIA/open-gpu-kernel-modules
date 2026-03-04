/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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


#include "rmapi/control.h"

#include "ctrl/ctrl0000/ctrl0000gpu.h"
#include "ctrl/ctrl0000/ctrl0000nvd.h"
#include "ctrl/ctrl0000/ctrl0000system.h"
#include "ctrl/ctrl0041.h"
#include "ctrl/ctrl0080/ctrl0080dma.h"
#include "ctrl/ctrl0080/ctrl0080fb.h"
#include "ctrl/ctrl0080/ctrl0080fifo.h"
#include "ctrl/ctrl0080/ctrl0080gr.h"
#include "ctrl/ctrl0080/ctrl0080gpu.h"
#include "ctrl/ctrl0080/ctrl0080host.h"
#include "ctrl/ctrl0080/ctrl0080msenc.h"
#include "ctrl/ctrl0080/ctrl0080perf.h"
#include "ctrl/ctrl2080/ctrl2080bus.h"
#include "ctrl/ctrl2080/ctrl2080ce.h"
#include "ctrl/ctrl2080/ctrl2080fb.h"
#include "ctrl/ctrl2080/ctrl2080gpu.h"
#include "ctrl/ctrl2080/ctrl2080i2c.h"
#include "ctrl/ctrl2080/ctrl2080mc.h"
#include "ctrl/ctrl2080/ctrl2080nvd.h"
#include "ctrl/ctrl2080/ctrl2080perf.h"
#include "ctrl/ctrl2080/ctrl2080pmgr.h"
#include "ctrl/ctrl2080/ctrl2080rc.h"
#include "ctrl/ctrl2080/ctrl2080thermal.h"
#include "ctrl/ctrl208f/ctrl208fgpu.h"
#include "ctrl/ctrl402c.h"
#include "ctrl/ctrl0073.h"
#include "ctrl/ctrla0bc.h"
#include "ctrl/ctrlb06f.h"
#include "ctrl/ctrl83de.h"
#ifdef USE_AMAPLIB
#include "amap_v1.h"
#endif

//
// Validates pRmCtrlParams->pParams is non-NULL and user-provided paramsSize is correct
// This check is used in early validation paths outside the resource server lock
//
#define CHECK_PARAMS_OR_RETURN(pRmCtrlParams, paramsType)            \
    do {                                                             \
        NV_CHECK_OR_RETURN(LEVEL_WARNING,                            \
            (((pRmCtrlParams)->pParams != NULL) &&                   \
            ((pRmCtrlParams)->paramsSize) == sizeof(paramsType)),    \
            NV_ERR_INVALID_ARGUMENT);                                \
    } while(0)

static NvBool _i2cTransactionCopyIn(RMAPI_PARAM_COPY *paramCopies, RmCtrlParams *pRmCtrlParams)
{
    NV402C_CTRL_I2C_TRANSACTION_PARAMS *pParams = (NV402C_CTRL_I2C_TRANSACTION_PARAMS*)pRmCtrlParams->pParams;
    NvBool bCopyInitDone = NV_FALSE;

    switch (pParams->transType)
    {
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_I2C_BLOCK_RW:
        {
            RMAPI_PARAM_COPY_INIT(paramCopies[0],
                            pParams->transData.i2cBlockData.pMessage,
                            pParams->transData.i2cBlockData.pMessage,
                            pParams->transData.i2cBlockData.messageLength, 1);

            bCopyInitDone = NV_TRUE;
            if (pParams->transData.i2cBlockData.bWrite == NV_TRUE)
                paramCopies[0].flags |= RMAPI_PARAM_COPY_FLAGS_SKIP_COPYOUT;
            else
                paramCopies[0].flags |= RMAPI_PARAM_COPY_FLAGS_SKIP_COPYIN;

            break;
        }
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_I2C_BUFFER_RW:
        {
            RMAPI_PARAM_COPY_INIT(paramCopies[0],
                            pParams->transData.i2cBufferData.pMessage,
                            pParams->transData.i2cBufferData.pMessage,
                            pParams->transData.i2cBufferData.messageLength, 1);

            bCopyInitDone = NV_TRUE;
            if (pParams->transData.i2cBufferData.bWrite == NV_TRUE)
                paramCopies[0].flags |= RMAPI_PARAM_COPY_FLAGS_SKIP_COPYOUT;
            else
                paramCopies[0].flags |= RMAPI_PARAM_COPY_FLAGS_SKIP_COPYIN;

            break;
        }
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_BLOCK_RW:
        {
            RMAPI_PARAM_COPY_INIT(paramCopies[0],
                            pParams->transData.smbusBlockData.pMessage,
                            pParams->transData.smbusBlockData.pMessage,
                            pParams->transData.smbusBlockData.messageLength, 1);

            bCopyInitDone = NV_TRUE;
            if (pParams->transData.smbusBlockData.bWrite == NV_TRUE)
                paramCopies[0].flags |= RMAPI_PARAM_COPY_FLAGS_SKIP_COPYOUT;
            else
                paramCopies[0].flags |= RMAPI_PARAM_COPY_FLAGS_SKIP_COPYIN;

            break;
        }
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_MULTIBYTE_REGISTER_BLOCK_RW:
        {
            RMAPI_PARAM_COPY_INIT(paramCopies[0],
                            pParams->transData.smbusMultibyteRegisterData.pMessage,
                            pParams->transData.smbusMultibyteRegisterData.pMessage,
                            pParams->transData.smbusMultibyteRegisterData.messageLength, 1);

            bCopyInitDone = NV_TRUE;
            if (pParams->transData.smbusMultibyteRegisterData.bWrite == NV_TRUE)
                paramCopies[0].flags |= RMAPI_PARAM_COPY_FLAGS_SKIP_COPYOUT;
            else
                paramCopies[0].flags |= RMAPI_PARAM_COPY_FLAGS_SKIP_COPYIN;

            break;
        }
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_READ_EDID_DDC:
        {
            RMAPI_PARAM_COPY_INIT(paramCopies[0],
                            pParams->transData.edidData.pMessage,
                            pParams->transData.edidData.pMessage,
                            pParams->transData.edidData.messageLength, 1);

            bCopyInitDone = NV_TRUE;
            paramCopies[0].flags |= RMAPI_PARAM_COPY_FLAGS_SKIP_COPYIN;

            break;
        }

        default:
            break;
    }

    return bCopyInitDone;
}

static NV_STATUS i2cTransactionCopyOut(RMAPI_PARAM_COPY *paramCopies, RmCtrlParams *pRmCtrlParams)
{
    NV_STATUS status = NV_OK;
    NV402C_CTRL_I2C_TRANSACTION_PARAMS *pParams = (NV402C_CTRL_I2C_TRANSACTION_PARAMS*)pRmCtrlParams->pParams;

    switch (pParams->transType)
    {
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_I2C_BLOCK_RW:
        {
            status = rmapiParamsRelease(&paramCopies[0]);
            pParams->transData.i2cBlockData.pMessage = paramCopies[0].pUserParams;
            break;
        }
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_I2C_BUFFER_RW:
        {
            status = rmapiParamsRelease(&paramCopies[0]);
            pParams->transData.i2cBufferData.pMessage = paramCopies[0].pUserParams;
            break;
        }
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_BLOCK_RW:
        {
            status = rmapiParamsRelease(&paramCopies[0]);
            pParams->transData.smbusBlockData.pMessage = paramCopies[0].pUserParams;
            break;
        }
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_MULTIBYTE_REGISTER_BLOCK_RW:
        {
            status = rmapiParamsRelease(&paramCopies[0]);
            pParams->transData.smbusMultibyteRegisterData.pMessage = paramCopies[0].pUserParams;
            break;
        }
        case NV402C_CTRL_I2C_TRANSACTION_TYPE_READ_EDID_DDC:
        {
            status = rmapiParamsRelease(&paramCopies[0]);
            pParams->transData.edidData.pMessage = paramCopies[0].pUserParams;
            break;
        }

        default:
            break;
    }

    return status;
}

/*
 * Helper routine to handle all embedded pointer user to kernel copies
 * Top level parameters are already copied to kernel (done in rmControlCmdExecute/dispControlSynchronizedCmdExecute)
 * After successful execution each embedded pointer within pRmCtrlParams->pParams will be kernel
 * paramCopies is a 4-element array as we currently have up to 4 embedded pointers in existing RM Controls
 *
 * No new RM Controls with embedded pointers should ever be added!
 *
 * See bug 1867098 for more reference - [RM Linux/UVM] Fix GPU lock/mmap_sem lock inversion
 */
NV_STATUS embeddedParamCopyIn(RMAPI_PARAM_COPY *paramCopies, RmCtrlParams *pRmCtrlParams)
{
    NV_STATUS status = NV_OK;
    void* pParams = pRmCtrlParams->pParams;
    NvU32 paramsCnt = 1;
    NvU32 i, j = 0;

    if ((pRmCtrlParams->secInfo.paramLocation == PARAM_LOCATION_KERNEL) ||
        (pRmCtrlParams->flags & NVOS54_FLAGS_FINN_SERIALIZED))
    {
        return NV_OK;
    }

    switch (pRmCtrlParams->cmd)
    {
        case NV2080_CTRL_GPU_GET_NVENC_SW_SESSION_INFO:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV2080_CTRL_GPU_GET_NVENC_SW_SESSION_INFO_PARAMS);

            RMAPI_PARAM_COPY_INIT(paramCopies[0],
                            ((NV2080_CTRL_GPU_GET_NVENC_SW_SESSION_INFO_PARAMS*)pParams)->sessionInfoTbl,
                            ((NV2080_CTRL_GPU_GET_NVENC_SW_SESSION_INFO_PARAMS*)pParams)->sessionInfoTbl,
                            ((NV2080_CTRL_GPU_GET_NVENC_SW_SESSION_INFO_PARAMS*)pParams)->sessionInfoTblEntry,
                            sizeof(NV2080_CTRL_NVENC_SW_SESSION_INFO));
            paramCopies[0].flags |= RMAPI_PARAM_COPY_FLAGS_SKIP_COPYIN;

            break;
        }
        case NV2080_CTRL_CMD_GPU_GET_ENGINES:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV2080_CTRL_GPU_GET_ENGINES_PARAMS);

            RMAPI_PARAM_COPY_INIT(paramCopies[0],
                            ((NV2080_CTRL_GPU_GET_ENGINES_PARAMS*)pParams)->engineList,
                            ((NV2080_CTRL_GPU_GET_ENGINES_PARAMS*)pParams)->engineList,
                            ((NV2080_CTRL_GPU_GET_ENGINES_PARAMS*)pParams)->engineCount, sizeof(NvU32));
            break;
        }
#ifdef USE_AMAPLIB
        case NV2080_CTRL_CMD_FB_GET_AMAP_CONF:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV2080_CTRL_CMD_FB_GET_AMAP_CONF_PARAMS);

            NV2080_CTRL_CMD_FB_GET_AMAP_CONF_PARAMS *userParams = (NV2080_CTRL_CMD_FB_GET_AMAP_CONF_PARAMS*)pParams;
            NvU32 amapConfParamsSize = (userParams->pAmapConfParams != NvP64_NULL) ?
                                                                    sizeof(ConfParamsV1) : 0;
            NvU32 cbcSwizzleParamsSize = (userParams->pCbcSwizzleParams != NvP64_NULL) ?
                                                                    sizeof(CbcSwizzleParamsV1) : 0;

            RMAPI_PARAM_COPY_INIT(paramCopies[0],
                                  userParams->pAmapConfParams,
                                  userParams->pAmapConfParams,
                                  1, amapConfParamsSize);
            RMAPI_PARAM_COPY_INIT(paramCopies[1],
                                  userParams->pCbcSwizzleParams,
                                  userParams->pCbcSwizzleParams,
                                  1, cbcSwizzleParamsSize);

            paramsCnt++;

            break;
        }
#endif
        case NV2080_CTRL_CMD_CE_GET_CAPS:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV2080_CTRL_CE_GET_CAPS_PARAMS);

            RMAPI_PARAM_COPY_INIT(paramCopies[0],
                            ((NV2080_CTRL_CE_GET_CAPS_PARAMS*)pParams)->capsTbl,
                            ((NV2080_CTRL_CE_GET_CAPS_PARAMS*)pParams)->capsTbl,
                            ((NV2080_CTRL_CE_GET_CAPS_PARAMS*)pParams)->capsTblSize, 1);
            paramCopies[0].flags |= RMAPI_PARAM_COPY_FLAGS_SKIP_COPYIN;
            paramCopies[0].flags |= RMAPI_PARAM_COPY_FLAGS_ZERO_BUFFER;

            break;
        }
        case NV0080_CTRL_CMD_FIFO_GET_CHANNELLIST:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV0080_CTRL_FIFO_GET_CHANNELLIST_PARAMS);

            RMAPI_PARAM_COPY_INIT(paramCopies[0],
                            ((NV0080_CTRL_FIFO_GET_CHANNELLIST_PARAMS*)pParams)->pChannelHandleList,
                            ((NV0080_CTRL_FIFO_GET_CHANNELLIST_PARAMS*)pParams)->pChannelHandleList,
                            ((NV0080_CTRL_FIFO_GET_CHANNELLIST_PARAMS*)pParams)->numChannels, sizeof(NvU32));
            paramCopies[0].flags |= RMAPI_PARAM_COPY_FLAGS_SKIP_COPYOUT;

            RMAPI_PARAM_COPY_INIT(paramCopies[1],
                            ((NV0080_CTRL_FIFO_GET_CHANNELLIST_PARAMS*)pParams)->pChannelList,
                            ((NV0080_CTRL_FIFO_GET_CHANNELLIST_PARAMS*)pParams)->pChannelList,
                            ((NV0080_CTRL_FIFO_GET_CHANNELLIST_PARAMS*)pParams)->numChannels, sizeof(NvU32));
            paramsCnt++;

            break;
        }
        case NV0000_CTRL_CMD_SYSTEM_EXECUTE_ACPI_METHOD:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV0000_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_PARAMS);

            RMAPI_PARAM_COPY_INIT(paramCopies[0],
                            ((NV0000_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_PARAMS*)pParams)->inData,
                            ((NV0000_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_PARAMS*)pParams)->inData,
                            ((NV0000_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_PARAMS*)pParams)->inDataSize, 1);
            paramCopies[0].flags |= RMAPI_PARAM_COPY_FLAGS_SKIP_COPYOUT;

            RMAPI_PARAM_COPY_INIT(paramCopies[1],
                            ((NV0000_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_PARAMS*)pParams)->outData,
                            ((NV0000_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_PARAMS*)pParams)->outData,
                            ((NV0000_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_PARAMS*)pParams)->outDataSize, 1);
            paramCopies[1].flags |= RMAPI_PARAM_COPY_FLAGS_SKIP_COPYIN;
            paramCopies[1].flags |= RMAPI_PARAM_COPY_FLAGS_ZERO_BUFFER;

            paramsCnt++;

            break;
        }
        case NV0073_CTRL_CMD_SYSTEM_EXECUTE_ACPI_METHOD:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_PARAMS);

            RMAPI_PARAM_COPY_INIT(paramCopies[0],
                            ((NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_PARAMS*)pParams)->inData,
                            ((NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_PARAMS*)pParams)->inData,
                            ((NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_PARAMS*)pParams)->inDataSize, 1);
            paramCopies[0].flags |= RMAPI_PARAM_COPY_FLAGS_SKIP_COPYOUT;

            RMAPI_PARAM_COPY_INIT(paramCopies[1],
                            ((NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_PARAMS*)pParams)->outData,
                            ((NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_PARAMS*)pParams)->outData,
                            ((NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_PARAMS*)pParams)->outDataSize, 1);
            paramCopies[1].flags |= RMAPI_PARAM_COPY_FLAGS_SKIP_COPYIN;
            paramCopies[1].flags |= RMAPI_PARAM_COPY_FLAGS_ZERO_BUFFER;

            paramsCnt++;

            break;
        }
        case NV0080_CTRL_CMD_HOST_GET_CAPS:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV0080_CTRL_HOST_GET_CAPS_PARAMS);

            RMAPI_PARAM_COPY_INIT(paramCopies[0],
                            ((NV0080_CTRL_HOST_GET_CAPS_PARAMS*)pParams)->capsTbl,
                            ((NV0080_CTRL_HOST_GET_CAPS_PARAMS*)pParams)->capsTbl,
                            ((NV0080_CTRL_HOST_GET_CAPS_PARAMS*)pParams)->capsTblSize, 1);
            paramCopies[0].flags |= RMAPI_PARAM_COPY_FLAGS_SKIP_COPYIN;
            paramCopies[0].flags |= RMAPI_PARAM_COPY_FLAGS_ZERO_BUFFER;

            break;
        }
        case NV2080_CTRL_CMD_BIOS_GET_INFO:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV2080_CTRL_BIOS_GET_INFO_PARAMS);

            RMAPI_PARAM_COPY_INIT(paramCopies[0],
                            ((NV2080_CTRL_BIOS_GET_INFO_PARAMS*)pParams)->biosInfoList,
                            ((NV2080_CTRL_BIOS_GET_INFO_PARAMS*)pParams)->biosInfoList,
                            ((NV2080_CTRL_BIOS_GET_INFO_PARAMS*)pParams)->biosInfoListSize,
                            sizeof(NV2080_CTRL_BIOS_INFO));
            break;
        }
        case NV2080_CTRL_CMD_BIOS_GET_NBSI:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV2080_CTRL_BIOS_GET_NBSI_PARAMS);

            RMAPI_PARAM_COPY_INIT(paramCopies[0],
                            ((NV2080_CTRL_BIOS_GET_NBSI_PARAMS*)pParams)->retBuf,
                            ((NV2080_CTRL_BIOS_GET_NBSI_PARAMS*)pParams)->retBuf,
                            ((NV2080_CTRL_BIOS_GET_NBSI_PARAMS*)pParams)->retSize, 1);
            paramCopies[0].flags |= RMAPI_PARAM_COPY_FLAGS_SKIP_COPYIN;
            paramCopies[0].flags |= RMAPI_PARAM_COPY_FLAGS_ZERO_BUFFER;

            break;
        }
        case NV2080_CTRL_CMD_BIOS_GET_NBSI_OBJ:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV2080_CTRL_BIOS_GET_NBSI_OBJ_PARAMS);

            RMAPI_PARAM_COPY_INIT(paramCopies[0],
                            ((NV2080_CTRL_BIOS_GET_NBSI_OBJ_PARAMS*)pParams)->retBuf,
                            ((NV2080_CTRL_BIOS_GET_NBSI_OBJ_PARAMS*)pParams)->retBuf,
                            ((NV2080_CTRL_BIOS_GET_NBSI_OBJ_PARAMS*)pParams)->retSize, 1);
            paramCopies[0].flags |= RMAPI_PARAM_COPY_FLAGS_SKIP_COPYIN;
            paramCopies[0].flags |= RMAPI_PARAM_COPY_FLAGS_ZERO_BUFFER;

            break;
        }
        case NV0080_CTRL_CMD_GR_GET_INFO:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV0080_CTRL_GR_GET_INFO_PARAMS);

            RMAPI_PARAM_COPY_INIT(paramCopies[0],
                            ((NV0080_CTRL_GR_GET_INFO_PARAMS*)pParams)->grInfoList,
                            ((NV0080_CTRL_GR_GET_INFO_PARAMS*)pParams)->grInfoList,
                            ((NV0080_CTRL_GR_GET_INFO_PARAMS*)pParams)->grInfoListSize,
                            sizeof(NV0080_CTRL_GR_INFO));
            break;
        }
        case NV0080_CTRL_CMD_FIFO_GET_CAPS:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV0080_CTRL_FIFO_GET_CAPS_PARAMS);

            RMAPI_PARAM_COPY_INIT(paramCopies[0],
                            ((NV0080_CTRL_FIFO_GET_CAPS_PARAMS*)pParams)->capsTbl,
                            ((NV0080_CTRL_FIFO_GET_CAPS_PARAMS*)pParams)->capsTbl,
                            ((NV0080_CTRL_FIFO_GET_CAPS_PARAMS*)pParams)->capsTblSize, 1);
            paramCopies[0].flags |= RMAPI_PARAM_COPY_FLAGS_SKIP_COPYIN;
            paramCopies[0].flags |= RMAPI_PARAM_COPY_FLAGS_ZERO_BUFFER;

            break;
        }
        case NVA0BC_CTRL_CMD_NVENC_SW_SESSION_UPDATE_INFO:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NVA0BC_CTRL_NVENC_SW_SESSION_UPDATE_INFO_PARAMS);

            RMAPI_PARAM_COPY_INIT(paramCopies[0],
                            ((NVA0BC_CTRL_NVENC_SW_SESSION_UPDATE_INFO_PARAMS*)pParams)->timestampBuffer,
                            ((NVA0BC_CTRL_NVENC_SW_SESSION_UPDATE_INFO_PARAMS*)pParams)->timestampBuffer,
                            ((NVA0BC_CTRL_NVENC_SW_SESSION_UPDATE_INFO_PARAMS*)pParams)->timestampBufferSize,
                            sizeof(NVA0BC_CTRL_NVENC_TIMESTAMP));
            break;
        }
        case NV83DE_CTRL_CMD_DEBUG_READ_MEMORY:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV83DE_CTRL_DEBUG_READ_MEMORY_PARAMS);

            RMAPI_PARAM_COPY_INIT(paramCopies[0],
                            ((NV83DE_CTRL_DEBUG_READ_MEMORY_PARAMS*)pParams)->buffer,
                            ((NV83DE_CTRL_DEBUG_READ_MEMORY_PARAMS*)pParams)->buffer,
                            ((NV83DE_CTRL_DEBUG_READ_MEMORY_PARAMS*)pParams)->length, 1);
            paramCopies[0].flags |= RMAPI_PARAM_COPY_FLAGS_SKIP_COPYIN;
            paramCopies[0].flags |= RMAPI_PARAM_COPY_FLAGS_ZERO_BUFFER;
            paramCopies[0].flags |= RMAPI_PARAM_COPY_FLAGS_DISABLE_MAX_SIZE_CHECK;
            break;
        }
        case NV83DE_CTRL_CMD_DEBUG_WRITE_MEMORY:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV83DE_CTRL_DEBUG_WRITE_MEMORY_PARAMS);

            RMAPI_PARAM_COPY_INIT(paramCopies[0],
                            ((NV83DE_CTRL_DEBUG_WRITE_MEMORY_PARAMS*)pParams)->buffer,
                            ((NV83DE_CTRL_DEBUG_WRITE_MEMORY_PARAMS*)pParams)->buffer,
                            ((NV83DE_CTRL_DEBUG_WRITE_MEMORY_PARAMS*)pParams)->length, 1);
            paramCopies[0].flags |= RMAPI_PARAM_COPY_FLAGS_SKIP_COPYOUT;
            paramCopies[0].flags |= RMAPI_PARAM_COPY_FLAGS_DISABLE_MAX_SIZE_CHECK;
            break;
        }
        case NV83DE_CTRL_CMD_DEBUG_READ_BATCH_MEMORY:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV83DE_CTRL_DEBUG_ACCESS_MEMORY_PARAMS);

            RMAPI_PARAM_COPY_INIT(paramCopies[0],
                            ((NV83DE_CTRL_DEBUG_ACCESS_MEMORY_PARAMS*)pParams)->pData,
                            ((NV83DE_CTRL_DEBUG_ACCESS_MEMORY_PARAMS*)pParams)->pData,
                            ((NV83DE_CTRL_DEBUG_ACCESS_MEMORY_PARAMS*)pParams)->dataLength, 1);
            paramCopies[0].flags |= RMAPI_PARAM_COPY_FLAGS_SKIP_COPYIN;
            paramCopies[0].flags |= RMAPI_PARAM_COPY_FLAGS_ZERO_BUFFER;
            paramCopies[0].flags |= RMAPI_PARAM_COPY_FLAGS_DISABLE_MAX_SIZE_CHECK;
            break;
        }
        case NV83DE_CTRL_CMD_DEBUG_WRITE_BATCH_MEMORY:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV83DE_CTRL_DEBUG_ACCESS_MEMORY_PARAMS);

            RMAPI_PARAM_COPY_INIT(paramCopies[0],
                            ((NV83DE_CTRL_DEBUG_ACCESS_MEMORY_PARAMS*)pParams)->pData,
                            ((NV83DE_CTRL_DEBUG_ACCESS_MEMORY_PARAMS*)pParams)->pData,
                            ((NV83DE_CTRL_DEBUG_ACCESS_MEMORY_PARAMS*)pParams)->dataLength, 1);
            paramCopies[0].flags |= RMAPI_PARAM_COPY_FLAGS_SKIP_COPYOUT;
            paramCopies[0].flags |= RMAPI_PARAM_COPY_FLAGS_DISABLE_MAX_SIZE_CHECK;
            break;
        }
        case NV402C_CTRL_CMD_I2C_INDEXED:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV402C_CTRL_I2C_INDEXED_PARAMS);

            RMAPI_PARAM_COPY_INIT(paramCopies[0],
                            ((NV402C_CTRL_I2C_INDEXED_PARAMS*)pParams)->pMessage,
                            ((NV402C_CTRL_I2C_INDEXED_PARAMS*)pParams)->pMessage,
                            ((NV402C_CTRL_I2C_INDEXED_PARAMS*)pParams)->messageLength, 1);
            break;
        }
        case NV402C_CTRL_CMD_I2C_TRANSACTION:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV402C_CTRL_I2C_TRANSACTION_PARAMS);

            if (!_i2cTransactionCopyIn(paramCopies, pRmCtrlParams))
            {
                return status;
            }

            break;
        }
        case NV2080_CTRL_CMD_GPU_EXEC_REG_OPS:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV2080_CTRL_GPU_EXEC_REG_OPS_PARAMS);

            RMAPI_PARAM_COPY_INIT(paramCopies[0],
                            ((NV2080_CTRL_GPU_EXEC_REG_OPS_PARAMS*)pParams)->regOps,
                            ((NV2080_CTRL_GPU_EXEC_REG_OPS_PARAMS*)pParams)->regOps,
                            ((NV2080_CTRL_GPU_EXEC_REG_OPS_PARAMS*)pParams)->regOpCount,
                            sizeof(NV2080_CTRL_GPU_REG_OP));
            break;
        }
        case NV2080_CTRL_CMD_NVD_GET_DUMP:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV2080_CTRL_NVD_GET_DUMP_PARAMS);

            RMAPI_PARAM_COPY_INIT(paramCopies[0],
                            ((NV2080_CTRL_NVD_GET_DUMP_PARAMS*)pParams)->pBuffer,
                            ((NV2080_CTRL_NVD_GET_DUMP_PARAMS*)pParams)->pBuffer,
                            ((NV2080_CTRL_NVD_GET_DUMP_PARAMS*)pParams)->size, 1);
            paramCopies[0].flags |= RMAPI_PARAM_COPY_FLAGS_SKIP_COPYIN;
            paramCopies[0].flags |= RMAPI_PARAM_COPY_FLAGS_ZERO_BUFFER;

            break;
        }
        case NV0000_CTRL_CMD_NVD_GET_DUMP:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV0000_CTRL_NVD_GET_DUMP_PARAMS);

            RMAPI_PARAM_COPY_INIT(paramCopies[0],
                            ((NV0000_CTRL_NVD_GET_DUMP_PARAMS*)pParams)->pBuffer,
                            ((NV0000_CTRL_NVD_GET_DUMP_PARAMS*)pParams)->pBuffer,
                            ((NV0000_CTRL_NVD_GET_DUMP_PARAMS*)pParams)->size, 1);
            paramCopies[0].flags |= RMAPI_PARAM_COPY_FLAGS_SKIP_COPYIN;
            paramCopies[0].flags |= RMAPI_PARAM_COPY_FLAGS_ZERO_BUFFER;

            break;
        }
        case NV0041_CTRL_CMD_GET_SURFACE_INFO:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV0041_CTRL_GET_SURFACE_INFO_PARAMS);

            RMAPI_PARAM_COPY_INIT(paramCopies[0],
                            ((NV0041_CTRL_GET_SURFACE_INFO_PARAMS*)pParams)->surfaceInfoList,
                            ((NV0041_CTRL_GET_SURFACE_INFO_PARAMS*)pParams)->surfaceInfoList,
                            ((NV0041_CTRL_GET_SURFACE_INFO_PARAMS*)pParams)->surfaceInfoListSize,
                            sizeof(NV0041_CTRL_SURFACE_INFO));
            break;
        }
#ifdef NV0000_CTRL_CMD_OS_GET_CAPS
// Not defined on all platforms
        case NV0000_CTRL_CMD_OS_GET_CAPS:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV0000_CTRL_OS_GET_CAPS_PARAMS);

            RMAPI_PARAM_COPY_INIT(paramCopies[0],
                            ((NV0000_CTRL_OS_GET_CAPS_PARAMS*)pParams)->capsTbl,
                            ((NV0000_CTRL_OS_GET_CAPS_PARAMS*)pParams)->capsTbl,
                            ((NV0000_CTRL_OS_GET_CAPS_PARAMS*)pParams)->capsTblSize, 1);
            paramCopies[0].flags |= RMAPI_PARAM_COPY_FLAGS_SKIP_COPYIN;
            paramCopies[0].flags |= RMAPI_PARAM_COPY_FLAGS_ZERO_BUFFER;

            break;
        }
#endif
        case NV0000_CTRL_CMD_SYSTEM_GET_P2P_CAPS:
        {
            NvU32 numEntries = 0;

            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PARAMS);

            if (NvP64_VALUE(((NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PARAMS*)pParams)->busPeerIds) != NULL)
            {
                // The handler will check gpuCount * gpuCount against overflow
                numEntries = ((NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PARAMS*)pParams)->gpuCount *
                             ((NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PARAMS*)pParams)->gpuCount;
            }

            RMAPI_PARAM_COPY_INIT(paramCopies[0],
                            ((NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PARAMS*)pParams)->busPeerIds,
                            ((NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PARAMS*)pParams)->busPeerIds,
                            numEntries, sizeof(NvU32));
            paramCopies[0].flags |= RMAPI_PARAM_COPY_FLAGS_SKIP_COPYIN;
            numEntries = 0;

            if (NvP64_VALUE(((NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PARAMS*)pParams)->busEgmPeerIds) != NULL)
            {
                // The handler will check gpuCount * gpuCount against overflow
                numEntries = ((NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PARAMS*)pParams)->gpuCount *
                             ((NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PARAMS*)pParams)->gpuCount;
            }

            RMAPI_PARAM_COPY_INIT(paramCopies[1],
                                  ((NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PARAMS*)pParams)->busEgmPeerIds,
                                  ((NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PARAMS*)pParams)->busEgmPeerIds,
                                  numEntries, sizeof(NvU32));
            paramCopies[1].flags |= RMAPI_PARAM_COPY_FLAGS_SKIP_COPYIN;
            paramsCnt++;
            break;
        }
        case NV0080_CTRL_CMD_FB_GET_CAPS:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV0080_CTRL_FB_GET_CAPS_PARAMS);

            RMAPI_PARAM_COPY_INIT(paramCopies[0],
                            ((NV0080_CTRL_FB_GET_CAPS_PARAMS*)pParams)->capsTbl,
                            ((NV0080_CTRL_FB_GET_CAPS_PARAMS*)pParams)->capsTbl,
                            ((NV0080_CTRL_FB_GET_CAPS_PARAMS*)pParams)->capsTblSize, 1);
            paramCopies[0].flags |= RMAPI_PARAM_COPY_FLAGS_SKIP_COPYIN;
            paramCopies[0].flags |= RMAPI_PARAM_COPY_FLAGS_ZERO_BUFFER;

            break;
        }
        case NV0080_CTRL_CMD_GPU_GET_CLASSLIST:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV0080_CTRL_GPU_GET_CLASSLIST_PARAMS);

            RMAPI_PARAM_COPY_INIT(paramCopies[0],
                            ((NV0080_CTRL_GPU_GET_CLASSLIST_PARAMS*)pParams)->classList,
                            ((NV0080_CTRL_GPU_GET_CLASSLIST_PARAMS*)pParams)->classList,
                            ((NV0080_CTRL_GPU_GET_CLASSLIST_PARAMS*)pParams)->numClasses, sizeof(NvU32));
            break;
        }
        case NV2080_CTRL_CMD_GPU_GET_ENGINE_CLASSLIST:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV2080_CTRL_GPU_GET_ENGINE_CLASSLIST_PARAMS);

            RMAPI_PARAM_COPY_INIT(paramCopies[0],
                            ((NV2080_CTRL_GPU_GET_ENGINE_CLASSLIST_PARAMS*)pParams)->classList,
                            ((NV2080_CTRL_GPU_GET_ENGINE_CLASSLIST_PARAMS*)pParams)->classList,
                            ((NV2080_CTRL_GPU_GET_ENGINE_CLASSLIST_PARAMS*)pParams)->numClasses, sizeof(NvU32));
            break;
        }
        case NV0080_CTRL_CMD_GR_GET_CAPS:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV0080_CTRL_GR_GET_CAPS_PARAMS);

            RMAPI_PARAM_COPY_INIT(paramCopies[0],
                            ((NV0080_CTRL_GR_GET_CAPS_PARAMS*)pParams)->capsTbl,
                            ((NV0080_CTRL_GR_GET_CAPS_PARAMS*)pParams)->capsTbl,
                            ((NV0080_CTRL_GR_GET_CAPS_PARAMS*)pParams)->capsTblSize, 1);
            paramCopies[0].flags |= RMAPI_PARAM_COPY_FLAGS_SKIP_COPYIN;
            paramCopies[0].flags |= RMAPI_PARAM_COPY_FLAGS_ZERO_BUFFER;

            break;
        }
        case NV2080_CTRL_CMD_I2C_ACCESS:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV2080_CTRL_I2C_ACCESS_PARAMS);

            RMAPI_PARAM_COPY_INIT(paramCopies[0],
                            ((NV2080_CTRL_I2C_ACCESS_PARAMS*)pParams)->data,
                            ((NV2080_CTRL_I2C_ACCESS_PARAMS*)pParams)->data,
                            ((NV2080_CTRL_I2C_ACCESS_PARAMS*)pParams)->dataBuffSize, 1);
            break;
        }
        case NV2080_CTRL_CMD_GR_GET_INFO:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV2080_CTRL_GR_GET_INFO_PARAMS);

            RMAPI_PARAM_COPY_INIT(paramCopies[0],
                            ((NV2080_CTRL_GR_GET_INFO_PARAMS*)pParams)->grInfoList,
                            ((NV2080_CTRL_GR_GET_INFO_PARAMS*)pParams)->grInfoList,
                            ((NV2080_CTRL_GR_GET_INFO_PARAMS*)pParams)->grInfoListSize,
                            sizeof(NV2080_CTRL_GR_INFO));
            break;
        }
        case NVB06F_CTRL_CMD_MIGRATE_ENGINE_CTX_DATA:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NVB06F_CTRL_MIGRATE_ENGINE_CTX_DATA_PARAMS);

            RMAPI_PARAM_COPY_INIT(paramCopies[0],
                            ((NVB06F_CTRL_MIGRATE_ENGINE_CTX_DATA_PARAMS*)pParams)->pEngineCtxBuff,
                            ((NVB06F_CTRL_MIGRATE_ENGINE_CTX_DATA_PARAMS*)pParams)->pEngineCtxBuff,
                            ((NVB06F_CTRL_MIGRATE_ENGINE_CTX_DATA_PARAMS*)pParams)->size, 1);
            paramCopies[0].flags |= RMAPI_PARAM_COPY_FLAGS_DISABLE_MAX_SIZE_CHECK;

            break;
        }
        case NVB06F_CTRL_CMD_GET_ENGINE_CTX_DATA:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NVB06F_CTRL_GET_ENGINE_CTX_DATA_PARAMS);

            RMAPI_PARAM_COPY_INIT(paramCopies[0],
                            ((NVB06F_CTRL_GET_ENGINE_CTX_DATA_PARAMS*)pParams)->pEngineCtxBuff,
                            ((NVB06F_CTRL_GET_ENGINE_CTX_DATA_PARAMS*)pParams)->pEngineCtxBuff,
                            ((NVB06F_CTRL_GET_ENGINE_CTX_DATA_PARAMS*)pParams)->size, 1);
            paramCopies[0].flags |= RMAPI_PARAM_COPY_FLAGS_SKIP_COPYIN;
            paramCopies[0].flags |= RMAPI_PARAM_COPY_FLAGS_DISABLE_MAX_SIZE_CHECK;

            break;
        }
        case NV2080_CTRL_CMD_RC_READ_VIRTUAL_MEM:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV2080_CTRL_RC_READ_VIRTUAL_MEM_PARAMS);

            RMAPI_PARAM_COPY_INIT(paramCopies[0],
                            ((NV2080_CTRL_RC_READ_VIRTUAL_MEM_PARAMS*)pParams)->bufferPtr,
                            ((NV2080_CTRL_RC_READ_VIRTUAL_MEM_PARAMS*)pParams)->bufferPtr,
                            ((NV2080_CTRL_RC_READ_VIRTUAL_MEM_PARAMS*)pParams)->bufferSize, 1);
            paramCopies[0].flags |= RMAPI_PARAM_COPY_FLAGS_SKIP_COPYIN;
            paramCopies[0].flags |= RMAPI_PARAM_COPY_FLAGS_ZERO_BUFFER;

            break;
        }
        case NV0080_CTRL_CMD_DMA_UPDATE_PDE_2:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV0080_CTRL_DMA_UPDATE_PDE_2_PARAMS);

            RMAPI_PARAM_COPY_INIT(paramCopies[0],
                            ((NV0080_CTRL_DMA_UPDATE_PDE_2_PARAMS*)pParams)->pPdeBuffer,
                            ((NV0080_CTRL_DMA_UPDATE_PDE_2_PARAMS*)pParams)->pPdeBuffer,
                            1, 0x10/*NV_MMU_VER2_DUAL_PDE__SIZE*/);

            paramCopies[0].flags |= RMAPI_PARAM_COPY_FLAGS_SKIP_COPYIN;
            paramCopies[0].flags |= RMAPI_PARAM_COPY_FLAGS_ZERO_BUFFER;

            break;
        }

        case NV2080_CTRL_CMD_GPU_RPC_GSP_TEST:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV2080_CTRL_GPU_RPC_GSP_TEST_PARAMS);
            RMAPI_PARAM_COPY_INIT(paramCopies[0],
                            ((NV2080_CTRL_GPU_RPC_GSP_TEST_PARAMS*)pParams)->data,
                            ((NV2080_CTRL_GPU_RPC_GSP_TEST_PARAMS*)pParams)->data,
                            ((NV2080_CTRL_GPU_RPC_GSP_TEST_PARAMS*)pParams)->dataSize,
                            sizeof(NvU32));
            break;
        }


        default:
        {
            return status;
        }
    }

    for (i = 0; i < paramsCnt; i++)
    {
        status = rmapiParamsAcquire(&paramCopies[i],
                                    (pRmCtrlParams->secInfo.paramLocation != PARAM_LOCATION_KERNEL));
        if (status != NV_OK)
            break;
    }

    // Preserve the original pRmCtrlParams->secInfo.paramLocation value since:
    // - paramLocation should not be used beyond this point except
    // - in embeddedParamCopyOut to skip copy-out in case when all parameters were in kernel space

    // If a single rmapiParamsAcquire() fails release all previous paramCopies
    if (status != NV_OK)
    {
        for (j = 0; j <= i; j++)
        {
            paramCopies[j].flags |= RMAPI_PARAM_COPY_FLAGS_SKIP_COPYOUT;
            rmapiParamsRelease(&paramCopies[j]);
        }
    }

    return status;
}

NV_STATUS embeddedParamCopyOut(RMAPI_PARAM_COPY *paramCopies, RmCtrlParams *pRmCtrlParams)
{
    NV_STATUS status = NV_OK;
    void* pParams = pRmCtrlParams->pParams;

    if ((pRmCtrlParams->secInfo.paramLocation == PARAM_LOCATION_KERNEL) ||
        (pRmCtrlParams->flags & NVOS54_FLAGS_FINN_SERIALIZED))
    {
        return NV_OK;
    }

    switch (pRmCtrlParams->cmd)
    {
        case NV2080_CTRL_GPU_GET_NVENC_SW_SESSION_INFO:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV2080_CTRL_GPU_GET_NVENC_SW_SESSION_INFO_PARAMS);

            status = rmapiParamsRelease(&paramCopies[0]);
            ((NV2080_CTRL_GPU_GET_NVENC_SW_SESSION_INFO_PARAMS*)pParams)->sessionInfoTbl = paramCopies[0].pUserParams;
            break;
        }
        case NV2080_CTRL_CMD_GPU_GET_ENGINES:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV2080_CTRL_GPU_GET_ENGINES_PARAMS);

            status = rmapiParamsRelease(&paramCopies[0]);
            ((NV2080_CTRL_GPU_GET_ENGINES_PARAMS*)pParams)->engineList = paramCopies[0].pUserParams;
            break;
        }
#ifdef USE_AMAPLIB
        case NV2080_CTRL_CMD_FB_GET_AMAP_CONF:
        {
            NV_STATUS status2;

            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV2080_CTRL_CMD_FB_GET_AMAP_CONF_PARAMS);

            NV2080_CTRL_CMD_FB_GET_AMAP_CONF_PARAMS *pParamsUser =
                (NV2080_CTRL_CMD_FB_GET_AMAP_CONF_PARAMS *) pParams;

            status = rmapiParamsRelease(&paramCopies[0]);
            status2 = rmapiParamsRelease(&paramCopies[1]);
            pParamsUser->pAmapConfParams = paramCopies[0].pUserParams;
            pParamsUser->pCbcSwizzleParams = paramCopies[1].pUserParams;

            status = (status == NV_OK) ? status2 : status;

            break;
        }
#endif
        case NV2080_CTRL_CMD_CE_GET_CAPS:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV2080_CTRL_CE_GET_CAPS_PARAMS);

            status = rmapiParamsRelease(&paramCopies[0]);
            ((NV2080_CTRL_CE_GET_CAPS_PARAMS*)pParams)->capsTbl = paramCopies[0].pUserParams;
            break;
        }
        case NV0080_CTRL_CMD_FIFO_GET_CHANNELLIST:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV0080_CTRL_FIFO_GET_CHANNELLIST_PARAMS);

            NV_STATUS handleListParamStatus = rmapiParamsRelease(&paramCopies[0]);
            ((NV0080_CTRL_FIFO_GET_CHANNELLIST_PARAMS*)pParams)->pChannelHandleList = paramCopies[0].pUserParams;

            status = rmapiParamsRelease(&paramCopies[1]);
            ((NV0080_CTRL_FIFO_GET_CHANNELLIST_PARAMS*)pParams)->pChannelList = paramCopies[1].pUserParams;

            if (handleListParamStatus != NV_OK)
                status = handleListParamStatus;

            break;
        }
        case NV0000_CTRL_CMD_SYSTEM_EXECUTE_ACPI_METHOD:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV0000_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_PARAMS);

            NV_STATUS inParamsStatus = rmapiParamsRelease(&paramCopies[0]);
            ((NV0000_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_PARAMS*)pParams)->inData = paramCopies[0].pUserParams;

            status = rmapiParamsRelease(&paramCopies[1]);
            ((NV0000_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_PARAMS*)pParams)->outData = paramCopies[1].pUserParams;

            if (inParamsStatus != NV_OK)
                status = inParamsStatus;
            break;
        }
        case NV0073_CTRL_CMD_SYSTEM_EXECUTE_ACPI_METHOD:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_PARAMS);

            NV_STATUS inParamsStatus = rmapiParamsRelease(&paramCopies[0]);
            ((NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_PARAMS*)pParams)->inData = paramCopies[0].pUserParams;

            status = rmapiParamsRelease(&paramCopies[1]);
            ((NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_PARAMS*)pParams)->outData = paramCopies[1].pUserParams;

            if (inParamsStatus != NV_OK)
                status = inParamsStatus;
            break;
        }
        case NV83DE_CTRL_CMD_DEBUG_READ_MEMORY:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV83DE_CTRL_DEBUG_READ_MEMORY_PARAMS);

            status = rmapiParamsRelease(&paramCopies[0]);
            ((NV83DE_CTRL_DEBUG_READ_MEMORY_PARAMS*)pRmCtrlParams->pParams)->buffer = paramCopies[0].pUserParams;
            break;
        }
        case NV83DE_CTRL_CMD_DEBUG_WRITE_MEMORY:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV83DE_CTRL_DEBUG_WRITE_MEMORY_PARAMS);

            status = rmapiParamsRelease(&paramCopies[0]);
            ((NV83DE_CTRL_DEBUG_WRITE_MEMORY_PARAMS*)pRmCtrlParams->pParams)->buffer = paramCopies[0].pUserParams;
            break;
        }
        case NV83DE_CTRL_CMD_DEBUG_READ_BATCH_MEMORY:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV83DE_CTRL_DEBUG_ACCESS_MEMORY_PARAMS);

            status = rmapiParamsRelease(&paramCopies[0]);
            ((NV83DE_CTRL_DEBUG_ACCESS_MEMORY_PARAMS*)pRmCtrlParams->pParams)->pData = paramCopies[0].pUserParams;
            break;
        }
        case NV83DE_CTRL_CMD_DEBUG_WRITE_BATCH_MEMORY:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV83DE_CTRL_DEBUG_ACCESS_MEMORY_PARAMS);

            status = rmapiParamsRelease(&paramCopies[0]);
            ((NV83DE_CTRL_DEBUG_ACCESS_MEMORY_PARAMS*)pRmCtrlParams->pParams)->pData = paramCopies[0].pUserParams;
            break;
        }
        case NV0080_CTRL_CMD_HOST_GET_CAPS:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV0080_CTRL_HOST_GET_CAPS_PARAMS);

            status = rmapiParamsRelease(&paramCopies[0]);
            ((NV0080_CTRL_HOST_GET_CAPS_PARAMS*)pParams)->capsTbl = paramCopies[0].pUserParams;
            break;
        }
        case NV2080_CTRL_CMD_BIOS_GET_INFO:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV2080_CTRL_BIOS_GET_INFO_PARAMS);

            status = rmapiParamsRelease(&paramCopies[0]);
            ((NV2080_CTRL_BIOS_GET_INFO_PARAMS*)pParams)->biosInfoList = paramCopies[0].pUserParams;
            break;
        }
        case NV2080_CTRL_CMD_BIOS_GET_NBSI:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV2080_CTRL_BIOS_GET_NBSI_PARAMS);

            status = rmapiParamsRelease(&paramCopies[0]);
            ((NV2080_CTRL_BIOS_GET_NBSI_PARAMS*)pParams)->retBuf = paramCopies[0].pUserParams;
            break;
        }
        case NV2080_CTRL_CMD_BIOS_GET_NBSI_OBJ:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV2080_CTRL_BIOS_GET_NBSI_OBJ_PARAMS);

            if (((NV2080_CTRL_BIOS_GET_NBSI_OBJ_PARAMS*)pParams)->retSize == 0)
            {
                paramCopies[0].flags |= RMAPI_PARAM_COPY_FLAGS_SKIP_COPYOUT;
            }

            status = rmapiParamsRelease(&paramCopies[0]);
            ((NV2080_CTRL_BIOS_GET_NBSI_OBJ_PARAMS*)pParams)->retBuf = paramCopies[0].pUserParams;
            break;
        }
        case NV0080_CTRL_CMD_GR_GET_INFO:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV0080_CTRL_GR_GET_INFO_PARAMS);

            status = rmapiParamsRelease(&paramCopies[0]);
            ((NV0080_CTRL_GR_GET_INFO_PARAMS*)pParams)->grInfoList = paramCopies[0].pUserParams;
            break;
        }
        case NV0080_CTRL_CMD_FIFO_GET_CAPS:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV0080_CTRL_FIFO_GET_CAPS_PARAMS);

            status = rmapiParamsRelease(&paramCopies[0]);
            ((NV0080_CTRL_FIFO_GET_CAPS_PARAMS*)pParams)->capsTbl = paramCopies[0].pUserParams;
            break;
        }
        case NVA0BC_CTRL_CMD_NVENC_SW_SESSION_UPDATE_INFO:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NVA0BC_CTRL_NVENC_SW_SESSION_UPDATE_INFO_PARAMS);

            status = rmapiParamsRelease(&paramCopies[0]);
            ((NVA0BC_CTRL_NVENC_SW_SESSION_UPDATE_INFO_PARAMS*)pParams)->timestampBuffer = paramCopies[0].pUserParams;
            break;
        }
        case NV402C_CTRL_CMD_I2C_INDEXED:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV402C_CTRL_I2C_INDEXED_PARAMS);

            status = rmapiParamsRelease(&paramCopies[0]);
            ((NV402C_CTRL_I2C_INDEXED_PARAMS*)pParams)->pMessage = paramCopies[0].pUserParams;
            break;
        }
        case NV402C_CTRL_CMD_I2C_TRANSACTION:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV402C_CTRL_I2C_TRANSACTION_PARAMS);

            return i2cTransactionCopyOut(paramCopies, pRmCtrlParams);
        }
        case NV2080_CTRL_CMD_GPU_EXEC_REG_OPS:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV2080_CTRL_GPU_EXEC_REG_OPS_PARAMS);

            status = rmapiParamsRelease(&paramCopies[0]);
            ((NV2080_CTRL_GPU_EXEC_REG_OPS_PARAMS*)pParams)->regOps = paramCopies[0].pUserParams;
            break;
        }
        case NV2080_CTRL_CMD_NVD_GET_DUMP:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV2080_CTRL_NVD_GET_DUMP_PARAMS);

            status = rmapiParamsRelease(&paramCopies[0]);
            ((NV2080_CTRL_NVD_GET_DUMP_PARAMS*)pParams)->pBuffer = paramCopies[0].pUserParams;
            break;
        }
        case NV0000_CTRL_CMD_NVD_GET_DUMP:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV0000_CTRL_NVD_GET_DUMP_PARAMS);

            status = rmapiParamsRelease(&paramCopies[0]);
            ((NV0000_CTRL_NVD_GET_DUMP_PARAMS*)pParams)->pBuffer = paramCopies[0].pUserParams;
            break;
        }
        case NV0041_CTRL_CMD_GET_SURFACE_INFO:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV0041_CTRL_GET_SURFACE_INFO_PARAMS);

            status = rmapiParamsRelease(&paramCopies[0]);
            ((NV0041_CTRL_GET_SURFACE_INFO_PARAMS*)pParams)->surfaceInfoList = paramCopies[0].pUserParams;
            break;
        }
#ifdef NV0000_CTRL_CMD_OS_GET_CAPS
        // Not defined on all platforms
        case NV0000_CTRL_CMD_OS_GET_CAPS:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV0000_CTRL_OS_GET_CAPS_PARAMS);

            status = rmapiParamsRelease(&paramCopies[0]);
            ((NV0000_CTRL_OS_GET_CAPS_PARAMS*)pParams)->capsTbl = paramCopies[0].pUserParams;
            break;
        }
#endif
        case NV0000_CTRL_CMD_SYSTEM_GET_P2P_CAPS:
        {
            NV_STATUS peerIdsStatus;
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PARAMS);

            peerIdsStatus = rmapiParamsRelease(&paramCopies[0]);
            ((NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PARAMS*)pParams)->busPeerIds = paramCopies[0].pUserParams;

            status = rmapiParamsRelease(&paramCopies[1]);
            ((NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PARAMS*)pParams)->busEgmPeerIds = paramCopies[1].pUserParams;

            if (peerIdsStatus != NV_OK)
                status = peerIdsStatus;

            break;
        }
        case NV0080_CTRL_CMD_FB_GET_CAPS:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV0080_CTRL_FB_GET_CAPS_PARAMS);

            status = rmapiParamsRelease(&paramCopies[0]);
            ((NV0080_CTRL_FB_GET_CAPS_PARAMS*)pParams)->capsTbl = paramCopies[0].pUserParams;
            break;
        }
        case NV0080_CTRL_CMD_GPU_GET_CLASSLIST:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV0080_CTRL_GPU_GET_CLASSLIST_PARAMS);

            status = rmapiParamsRelease(&paramCopies[0]);
            ((NV0080_CTRL_GPU_GET_CLASSLIST_PARAMS*)pParams)->classList = paramCopies[0].pUserParams;
            break;
        }
        case NV2080_CTRL_CMD_GPU_GET_ENGINE_CLASSLIST:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV2080_CTRL_GPU_GET_ENGINE_CLASSLIST_PARAMS);

            status = rmapiParamsRelease(&paramCopies[0]);
            ((NV2080_CTRL_GPU_GET_ENGINE_CLASSLIST_PARAMS*)pParams)->classList = paramCopies[0].pUserParams;
            break;
        }
        case NV0080_CTRL_CMD_GR_GET_CAPS:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV0080_CTRL_GR_GET_CAPS_PARAMS);

            status = rmapiParamsRelease(&paramCopies[0]);
            ((NV0080_CTRL_GR_GET_CAPS_PARAMS*)pParams)->capsTbl = paramCopies[0].pUserParams;
            break;
        }
        case NV2080_CTRL_CMD_I2C_ACCESS:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV2080_CTRL_I2C_ACCESS_PARAMS);

            status = rmapiParamsRelease(&paramCopies[0]);
            ((NV2080_CTRL_I2C_ACCESS_PARAMS*)pParams)->data = paramCopies[0].pUserParams;
            break;
        }
        case NV2080_CTRL_CMD_GR_GET_INFO:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV2080_CTRL_GR_GET_INFO_PARAMS);

            status = rmapiParamsRelease(&paramCopies[0]);
            ((NV2080_CTRL_GR_GET_INFO_PARAMS*)pParams)->grInfoList = paramCopies[0].pUserParams;
            break;
        }
        case NVB06F_CTRL_CMD_MIGRATE_ENGINE_CTX_DATA:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NVB06F_CTRL_MIGRATE_ENGINE_CTX_DATA_PARAMS);

            status = rmapiParamsRelease(&paramCopies[0]);
            ((NVB06F_CTRL_MIGRATE_ENGINE_CTX_DATA_PARAMS*)pParams)->pEngineCtxBuff = paramCopies[0].pUserParams;
            break;
        }
        case NVB06F_CTRL_CMD_GET_ENGINE_CTX_DATA:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NVB06F_CTRL_GET_ENGINE_CTX_DATA_PARAMS);

            status = rmapiParamsRelease(&paramCopies[0]);
            ((NVB06F_CTRL_GET_ENGINE_CTX_DATA_PARAMS*)pParams)->pEngineCtxBuff = paramCopies[0].pUserParams;
            break;
        }
        case NV2080_CTRL_CMD_RC_READ_VIRTUAL_MEM:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV2080_CTRL_RC_READ_VIRTUAL_MEM_PARAMS);

            status = rmapiParamsRelease(&paramCopies[0]);
            ((NV2080_CTRL_RC_READ_VIRTUAL_MEM_PARAMS*)pRmCtrlParams->pParams)->bufferPtr = paramCopies[0].pUserParams;
            break;
        }
        case NV0080_CTRL_CMD_DMA_UPDATE_PDE_2:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV0080_CTRL_DMA_UPDATE_PDE_2_PARAMS);

            status = rmapiParamsRelease(&paramCopies[0]);
            ((NV0080_CTRL_DMA_UPDATE_PDE_2_PARAMS*)pParams)->pPdeBuffer = paramCopies[0].pUserParams;

            break;
        }

        case NV2080_CTRL_CMD_GPU_RPC_GSP_TEST:
        {
            CHECK_PARAMS_OR_RETURN(pRmCtrlParams, NV2080_CTRL_GPU_RPC_GSP_TEST_PARAMS);
            status = rmapiParamsRelease(&paramCopies[0]);
            ((NV2080_CTRL_GPU_RPC_GSP_TEST_PARAMS*)pParams)->data = paramCopies[0].pUserParams;
            break;
        }

        default:
        {
            return NV_OK;
        }
    }

    return status;
}
