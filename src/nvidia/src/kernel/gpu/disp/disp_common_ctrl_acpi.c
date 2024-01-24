/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

 /**
  * This file implements rmctrls which
  * (a) are declared in disp_common_ctrl_acpi.h; i.e.
  *    (i) are dispcmnCtrlCmd* functions
  *    (ii) which are not used by Tegra SOC NVDisplay and/or OS layer; and
  * (b) are ACPI feature related
  */

#include "gpu_mgr/gpu_mgr.h"
#include "gpu/disp/disp_objs.h"
#include "mxm_spec.h"
#include "nvacpitypes.h"
#include "nbci.h"
#include "nvhybridacpi.h"
#include "acpigenfuncs.h"
#include "platform/platform.h"
#include "gpu/disp/kern_disp.h"

NV_STATUS
dispcmnCtrlCmdSpecificSetAcpiIdMapping_IMPL
(
    DispCommon *pDispCommon,
    NV0073_CTRL_SPECIFIC_SET_ACPI_ID_MAPPING_PARAMS *pParams
)
{
    OBJGPU     *pGpu = DISPAPI_GET_GPU(pDispCommon);
    OBJGPU     *pOrigGpu = pGpu;
    OBJSYS     *pSys = SYS_GET_INSTANCE();
    OBJPFM     *pPfm = SYS_GET_PFM(pSys);
    NvU32       acpiIdx,index;
    NvBool      bcState = gpumgrGetBcEnabledStatus(pGpu);
    NV_STATUS   status = NV_OK;

    // index into the RM mapping table
    index = 0;
    for (acpiIdx = 0; acpiIdx < NV0073_CTRL_SPECIFIC_MAX_ACPI_DEVICES; ++acpiIdx)
    {
        // client gave us a subdevice number: get right pGpu for it
        status = dispapiSetUnicastAndSynchronize_HAL(
                               staticCast(pDispCommon, DisplayApi),
                               DISPAPI_GET_GPUGRP(pDispCommon),
                               &pGpu,
                               NULL,
                               pParams->mapTable[acpiIdx].subDeviceInstance);
        if (status != NV_OK)
            return status;

        if ((pParams->mapTable[acpiIdx].displayId) &&
                !((pParams->mapTable[acpiIdx].displayId) & (pParams->mapTable[acpiIdx].displayId - 1)))
        {
            // update the mapping table
            pfmUpdateAcpiIdMapping(pPfm,
                                   pGpu,
                                   pParams->mapTable[acpiIdx].acpiId,
                                   pParams->mapTable[acpiIdx].displayId,
                                   pParams->mapTable[acpiIdx].dodIndex,
                                   index);
            // increment the index into the RM-side table only if we have a valid entry
            ++index;
        }
        else
        {
            continue;
        }
    }

    if (gpumgrGetBcEnabledStatus(pGpu))
    {
        // Grab the parent ...
        pGpu  = gpumgrGetDisplayParent(pGpu);
    }

    pGpu = pOrigGpu;
    gpumgrSetBcEnabledStatus(pGpu, bcState);

    return status;
}

NV_STATUS
dispcmnCtrlCmdSystemAcpiSubsystemActivated_IMPL
(
    DispCommon *pDispCommon,
    NV0073_CTRL_SYSTEM_ACPI_SUBSYSTEM_ACTIVATED_PARAMS *pParams
)
{
    OBJGPU    *pGpu   = DISPAPI_GET_GPU(pDispCommon);
    RM_API    *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    OBJOS     *pOs    = NULL;
    NV_STATUS  status = NV_OK;

    // client gave us a subdevice #: get right pGpu/pDisp for it
    status = dispapiSetUnicastAndSynchronize_HAL(
                               staticCast(pDispCommon, DisplayApi),
                               DISPAPI_GET_GPUGRP(pDispCommon),
                               &pGpu,
                               NULL,
                               pParams->subDeviceInstance);
    if (status != NV_OK)
    {
        return status;
    }

    pOs = GPU_GET_OS(pGpu);

    pOs->setProperty(pOs, PDB_PROP_OS_WAIT_FOR_ACPI_SUBSYSTEM, NV_FALSE);

    status = pRmApi->Control(pRmApi, pGpu->hInternalClient, pGpu->hInternalSubdevice,
                             NV2080_CTRL_CMD_INTERNAL_DISPLAY_ACPI_SUBSYSTEM_ACTIVATED,
                             NULL, 0);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_INFO,
                  "ERROR: NV0073_CTRL_SYSTEM_ACPI_SUBSYSTEM_ACTIVATED control call failed. "
                  "Should be rmapi ctrl call which is failed");
    }

    return status;
}

//
// This rmctrl MUST NOT touch hw since it's tagged as NO_GPUS_ACCESS in ctrl0073.def
// RM allow this type of rmctrl to go through when GPU is not available.
//
NV_STATUS
dispcmnCtrlCmdSystemExecuteAcpiMethod_IMPL
(
    DispCommon *pDispCommon,
    NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_PARAMS *pAcpiMethodParams
)
{
    OBJGPU *pGpu          = DISPAPI_GET_GPU(pDispCommon);
    KernelDisplay *pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);
    OBJSYS *pSys          = SYS_GET_INSTANCE();
    OBJPFM *pPfm          = SYS_GET_PFM(pSys);
    NvU32   method        = (NvU32)pAcpiMethodParams->method;
    NvU32   outStatus     = 0;
    NvU16   inDataSize;               // NOTE: must be NvU16, see below
    NvU16   outDataSize;              // NOTE: must be NvU16, see below
    void   *pInOutData    = NULL;
    NvU32   inOutDataSize;
    NvBool  bDoCopyOut    = NV_FALSE;
    NvU32   acpiId        = 0;
    NvU32   displayMask   = 0;
    NV_STATUS status      = NV_OK;

    //
    // Logically a single input/output buffer, but with 2 ptrs & 2 sizes.
    // We have to do our own copyout
    // Messing with output size at every level of the code
    //

    inDataSize      = pAcpiMethodParams->inDataSize;
    outDataSize     = pAcpiMethodParams->outDataSize;
    inOutDataSize   = (NvU32) NV_MAX(inDataSize, outDataSize);

    // Verify size
    if ((outDataSize == 0) ||
        (pAcpiMethodParams->inData == NvP64_NULL) ||
        (pAcpiMethodParams->outData == NvP64_NULL))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "ERROR: NV0073_CTRL_CMD_SYSTEM_EXECUTE_ACPI_METHOD: Parameter "
                  "validation failed: outDataSize=%d  inDataSize=%ud  method = %ud\n",
                  (NvU32)outDataSize, (NvU32)inDataSize, method);

        return NV_ERR_INVALID_ARGUMENT;
    }

    // Allocate memory for the combined in/out buffer
    pInOutData = portMemAllocNonPaged(inOutDataSize);
    if (pInOutData == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        NV_PRINTF(LEVEL_ERROR,
                  "ERROR: NV0073_CTRL_CMD_SYSTEM_EXECUTE_ACPI_METHOD: mem alloc failed\n");
        goto done;
    }

    if (inDataSize)
    {
        portMemCopy(pInOutData, inDataSize, NvP64_VALUE(pAcpiMethodParams->inData), inDataSize);
    }

    // jump to the method to be executed
    switch (method)
    {
        case NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_MXMX:
        {
            if (inOutDataSize < (NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_MXMX_DISP_MASK_OFFSET + sizeof(NvU32)))
            {
                outStatus = NV_ERR_INVALID_ARGUMENT;
                break;
            }
            //
            // get display mask from input buffer
            // display mask is 4 byte long and available at byte 1
            // of the input buffer.
            // byte 0 of the input buffer communicates the function (acquire(0)/release(1))
            //
            portMemCopy((NvU8*) &displayMask,
                        sizeof(NvU32),
                        ((NvU8*) pInOutData) + NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_MXMX_DISP_MASK_OFFSET,
                        sizeof(NvU32));

            // get acpi id
            acpiId = pfmFindAcpiId(pPfm, pGpu, displayMask);

            outDataSize = sizeof(NvU32);
            outStatus = osCallACPI_MXMX(pGpu, acpiId, pInOutData);
            break;

        }
        case NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_GPUON:
        {
            if (inOutDataSize < sizeof(NvU32))
            {
                outStatus = NV_ERR_INVALID_ARGUMENT;
                break;
            }

            outDataSize = sizeof(NvU32);
            outStatus = osCallACPI_NVHG_GPUON(pGpu, (NvU32*) pInOutData);
            break;
        }
        case NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_GPUOFF:
        {
            if (inOutDataSize < sizeof(NvU32))
            {
                outStatus = NV_ERR_INVALID_ARGUMENT;
                break;
            }

            outDataSize = sizeof(NvU32);
            outStatus = osCallACPI_NVHG_GPUOFF(pGpu, (NvU32*) pInOutData);
            break;
        }
        case NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_GPUSTA:
        {
            if (inOutDataSize < sizeof(NvU32))
            {
                outStatus = NV_ERR_INVALID_ARGUMENT;
                break;
            }

            outDataSize = sizeof(NvU32);
            outStatus = osCallACPI_NVHG_GPUSTA(pGpu, (NvU32*) pInOutData);
            break;
        }
        case NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_MXDS:
        {
            if (inOutDataSize < (NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_MXDS_DISP_MASK_OFFSET + sizeof(NvU32)))
            {
                outStatus = NV_ERR_INVALID_ARGUMENT;
                break;
            }

            //
            // get acpi id from input buffer
            // acpi id is 4 byte long and available at byte 4
            // of the input buffer.
            // byte 0 of the input buffer communicates the function (Get Mux state(0)/Set display to active(1))
            //
            portMemCopy(&acpiId,
                        sizeof(NvU32),
                        ((NvU8*) pInOutData) + NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_MXDS_DISP_MASK_OFFSET,
                        sizeof(NvU32));

            outDataSize = sizeof(NvU32);
            outStatus = osCallACPI_NVHG_MXDS(pGpu, acpiId, (NvU32*) pInOutData);
            break;
        }
        case NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_NBCI_MXDS:
        {
            if (inOutDataSize < (NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_NBCI_MXDS_DISP_MASK_OFFSET + sizeof(NvU32)))
            {
                outStatus = NV_ERR_INVALID_ARGUMENT;
                break;
            }

            //
            // get acpi id from input buffer
            // acpi id is 4 byte long and available at byte 4
            // of the input buffer.
            // byte 0 of the input buffer communicates the function
            //
            portMemCopy(&acpiId,
                        sizeof(NvU32),
                        ((NvU8*) pInOutData) + NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_NBCI_MXDS_DISP_MASK_OFFSET,
                        sizeof(NvU32));

            outDataSize = sizeof(NvU32);
            outStatus = osCallACPI_MXDS(pGpu, acpiId, (NvU32*) pInOutData);
            break;
        }
        case NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_NBCI_MXDM:
        {
            if (inOutDataSize < (NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_NBCI_MXDM_DISP_MASK_OFFSET + sizeof(NvU32)))
            {
                outStatus = NV_ERR_INVALID_ARGUMENT;
                break;
            }

            //
            // get acpi id from input buffer
            // acpi id is 4 byte long and available at byte 4
            // of the input buffer.
            // byte 0 of the input buffer communicates the function
            //
            portMemCopy(&acpiId,
                        sizeof(NvU32),
                        ((NvU8*) pInOutData) + NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_NBCI_MXDM_DISP_MASK_OFFSET,
                        sizeof(NvU32));

            outStatus = osCallACPI_MXDM(pGpu, acpiId, (NvU32*) pInOutData);
            outDataSize = sizeof(NvU32);
            break;
        }
        case NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_NBCI_MXID:
        {
            if (inOutDataSize < (NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_NBCI_MXID_DISP_MASK_OFFSET + sizeof(NvU32)))
            {
                outStatus = NV_ERR_INVALID_ARGUMENT;
                break;
            }

            // get acpi id from input buffer
            portMemCopy(&acpiId,
                        sizeof(NvU32),
                        ((NvU8*) pInOutData) + NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_NBCI_MXID_DISP_MASK_OFFSET,
                        sizeof(NvU32));

            outDataSize = sizeof(NvU32);
            outStatus = osCallACPI_MXID(pGpu, acpiId, (NvU32*) pInOutData);
            break;
        }
        case NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_NBCI_LRST:
        {
            if (inOutDataSize < (NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_NBCI_LRST_DISP_MASK_OFFSET + sizeof(NvU32)))
            {
                outStatus = NV_ERR_INVALID_ARGUMENT;
                break;
            }

            portMemCopy(&acpiId,
                        sizeof(NvU32),
                        ((NvU8*) pInOutData) + NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_NBCI_LRST_DISP_MASK_OFFSET,
                        sizeof(NvU32));

            outStatus = osCallACPI_LRST(pGpu, acpiId, (NvU32*) pInOutData);
            outDataSize = sizeof(NvU32);
            break;
        }
        case NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DDC_EDID:
        {
            if (inOutDataSize < (NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DDC_EDID_DISP_MASK_OFFSET + sizeof(NvU32)))
            {
                outStatus = NV_ERR_INVALID_ARGUMENT;
                break;
            }

            portMemCopy(&acpiId,
                        sizeof(NvU32),
                        ((NvU8*) pInOutData) + NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DDC_EDID_DISP_MASK_OFFSET,
                        sizeof(NvU32));

            NvU32 outSize = outDataSize;
            outStatus = osCallACPI_DDC(pGpu, acpiId, (NvU8*) pInOutData,
                                       &outSize, NV_TRUE /*bReadMultiBlock*/);
            outDataSize = (NvU16) outSize;
            break;
        }
        case NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_NVHG_MXMX:
        {
            if (inOutDataSize < (NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_NVHG_MXMX_DISP_MASK_OFFSET + sizeof(NvU32)))
            {
                outStatus = NV_ERR_INVALID_ARGUMENT;
                break;
            }

            //
            // get acpi id from input buffer
            // acpi id is 4 byte long and available at byte 4
            // of the input buffer.
            // byte 0 of the input buffer communicates the function (acquire(0)/release(1)/get mux state(2))
            //
            portMemCopy(&acpiId,
                        sizeof(NvU32),
                        ((NvU8*) pInOutData) + NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_NVHG_MXMX_DISP_MASK_OFFSET,
                        sizeof(NvU32));

            // get acpi id
            acpiId = pfmFindAcpiId(pPfm, pGpu, displayMask);

            outDataSize = sizeof(NvU32);
            outStatus = osCallACPI_NVHG_MXMX(pGpu, acpiId, (NvU32*) pInOutData);
            break;
        }
        case NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DOS:
        {
            if (inOutDataSize < (NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DOS_DISP_MASK_OFFSET + sizeof(NvU32)))
            {
                outStatus = NV_ERR_INVALID_ARGUMENT;
                break;
            }
            //
            // get acpi id from input buffer
            // acpi id is 4 byte long and available at byte 4
            // of the input buffer.
            // byte 0 of the input buffer communicates the function (by ACPI spec)
            //
            portMemCopy(&acpiId,
                        sizeof(NvU32),
                        ((NvU8*) pInOutData) + NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DOS_DISP_MASK_OFFSET,
                        sizeof(NvU32));

            // get acpi id
            acpiId = pfmFindAcpiId(pPfm, pGpu, displayMask);

            outDataSize = sizeof(NvU32);
            outStatus = osCallACPI_NVHG_DOS(pGpu, acpiId, (NvU32*) pInOutData);
            break;
        }
        case NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_ROM:
        {
            NvU32 *pBuffer = (NvU32*) pInOutData;
            if ((inOutDataSize < (2 * sizeof(NvU32))) || (inOutDataSize < pBuffer[1]))
            {
                outStatus = NV_ERR_INVALID_ARGUMENT;
                break;
            }

            outDataSize = pBuffer[1];
            outStatus = osCallACPI_NVHG_ROM(pGpu, (NvU32*) pInOutData, (NvU32*) pInOutData);
            break;
        }
        case NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DCS:
        {
            if (inOutDataSize < sizeof(NvU32))
            {
                outStatus = NV_ERR_INVALID_ARGUMENT;
                break;
            }

            // get display mask from input buffer
            portMemCopy(&acpiId, sizeof(NvU32), pInOutData, sizeof(NvU32));

            outDataSize = sizeof(NvU32);
            outStatus = osCallACPI_NVHG_DCS(pGpu, acpiId, (NvU32*) pInOutData);
            break;
        }
        case NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DOD:
        {
            NvU32 size32 = inOutDataSize;
            outStatus = osCallACPI_DOD(pGpu, (NvU32*) pInOutData, &size32);
            outDataSize = (NvU16) size32;
            break;
        }
        case NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_SUPPORT:
        {
            outStatus = osCallACPI_DSM(pGpu, ACPI_DSM_FUNCTION_NVHG, NVHG_FUNC_SUPPORT, (NvU32*) pInOutData, &outDataSize);
            break;
        }
        case NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_HYBRIDCAPS:
        {
            outStatus = osCallACPI_DSM(pGpu, ACPI_DSM_FUNCTION_NVHG, NVHG_FUNC_HYBRIDCAPS, (NvU32*) pInOutData, &outDataSize);
            break;
        }
        case NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_POLICYSELECT:
        {
            outStatus = osCallACPI_DSM(pGpu, ACPI_DSM_FUNCTION_NVHG, NVHG_FUNC_POLICYSELECT, (NvU32*) pInOutData, &outDataSize);
            break;
        }
        case NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_POWERCONTROL:
        {

            outStatus = osCallACPI_DSM(pGpu, ACPI_DSM_FUNCTION_NVHG, NVHG_FUNC_POWERCONTROL, (NvU32*) pInOutData, &outDataSize);
            break;
        }
        case NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_PLATPOLICY:
        {
            outStatus = osCallACPI_DSM(pGpu, ACPI_DSM_FUNCTION_NVHG, NVHG_FUNC_PLATPOLICY, (NvU32*) pInOutData, &outDataSize);
            break;
        }
        case NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_DISPLAYSTATUS:
        {
            if (pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_MXM_3X))
                outStatus = osCallACPI_DSM(pGpu, ACPI_DSM_FUNCTION_MXM, NV_ACPI_DSM_MXM_FUNC_MXDP, (NvU32*)pInOutData, &outDataSize);
            else
                outStatus = osCallACPI_DSM(pGpu, ACPI_DSM_FUNCTION_NVHG, NVHG_FUNC_DISPLAYSTATUS, (NvU32*)pInOutData, &outDataSize);
            break;
        }
        case NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_MDTL:
        {
            if (pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_MXM_3X))
                outStatus = osCallACPI_DSM(pGpu, ACPI_DSM_FUNCTION_MXM, NV_ACPI_DSM_MXM_FUNC_MDTL, (NvU32*)pInOutData, &outDataSize);
            else
                outStatus = osCallACPI_DSM(pGpu, ACPI_DSM_FUNCTION_NVHG, NVHG_FUNC_MDTL, (NvU32*)pInOutData, &outDataSize);
            break;
        }
        case NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_HCSMBLIST:
        {
            outStatus = osCallACPI_DSM(pGpu, ACPI_DSM_FUNCTION_NVHG, NVHG_FUNC_HCSMBLIST, (NvU32*) pInOutData, &outDataSize);
            break;
        }
        case NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_HCSMBADDR:
        {
            outStatus = osCallACPI_DSM(pGpu, ACPI_DSM_FUNCTION_NVHG, NVHG_FUNC_HCSMBADDR, (NvU32*) pInOutData, &outDataSize);
            break;
        }
        case NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_HCREADBYTE:
        {
            outStatus = osCallACPI_DSM(pGpu, ACPI_DSM_FUNCTION_NVHG, NVHG_FUNC_HCREADBYTE, (NvU32*) pInOutData, &outDataSize);
            break;
        }
        case NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_HCSENDBYTE:
        {
            outStatus = osCallACPI_DSM(pGpu, ACPI_DSM_FUNCTION_NVHG, NVHG_FUNC_HCSENDBYTE, (NvU32*) pInOutData, &outDataSize);
            break;
        }
        case NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_HCGETSTATUS:
        {
            outStatus = osCallACPI_DSM(pGpu, ACPI_DSM_FUNCTION_NVHG, NVHG_FUNC_HCGETSTATUS, (NvU32*) pInOutData, &outDataSize);
            break;
        }
        case NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_HCTRIGDDC:
        {
            outStatus = osCallACPI_DSM(pGpu, ACPI_DSM_FUNCTION_NVHG, NVHG_FUNC_HCTRIGDDC, (NvU32*) pInOutData, &outDataSize);
            break;
        }
        case NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_HCGETDDC:
        {
            outStatus = osCallACPI_DSM(pGpu, ACPI_DSM_FUNCTION_NVHG, NVHG_FUNC_HCGETDDC, (NvU32*) pInOutData, &outDataSize);
            break;
        }

        case NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_MXM_MXSS:
        {
            outStatus = osCallACPI_DSM(pGpu, ACPI_DSM_FUNCTION_MXM, NV_ACPI_DSM_MXM_FUNC_MXSS, (NvU32*) pInOutData, &outDataSize);
            break;
        }
        case NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_MXM_MXMI:
        {
            outStatus = osCallACPI_DSM(pGpu, ACPI_DSM_FUNCTION_MXM, NV_ACPI_DSM_MXM_FUNC_MXMI, (NvU32*) pInOutData, &outDataSize);
            break;
        }
        case NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_MXM_MXMS:
        {
            outStatus = osCallACPI_DSM(pGpu, ACPI_DSM_FUNCTION_MXM, NV_ACPI_DSM_MXM_FUNC_MXMS, (NvU32*) pInOutData, &outDataSize);
            break;
        }
        case NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_MXM_MXPP:
        {
            outStatus = osCallACPI_DSM(pGpu, ACPI_DSM_FUNCTION_MXM, NV_ACPI_DSM_MXM_FUNC_MXPP, (NvU32*) pInOutData, &outDataSize);
            break;
        }
        case NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_MXM_MXDP:
        {
            outStatus = osCallACPI_DSM(pGpu, ACPI_DSM_FUNCTION_MXM, NV_ACPI_DSM_MXM_FUNC_MXDP, (NvU32*) pInOutData, &outDataSize);
            break;
        }
        case NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_MXM_MDTL:
        {
            outStatus = osCallACPI_DSM(pGpu, ACPI_DSM_FUNCTION_MXM, NV_ACPI_DSM_MXM_FUNC_MDTL, (NvU32*) pInOutData, &outDataSize);
            break;
        }
        case NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_MXM_MXCB:
        {
            outStatus = kdispDsmMxmMxcbExecuteAcpi_HAL(pGpu, pKernelDisplay, pInOutData, &outDataSize);
            break;
        }
        case NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_MXM_GETEVENTLIST:
        {
            outStatus = osCallACPI_DSM(pGpu, ACPI_DSM_FUNCTION_MXM, NV_ACPI_DSM_MXM_FUNC_GETEVENTLIST, (NvU32*) pInOutData, &outDataSize);
            break;
        }
        case NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_GETMEMTABLE:
        {
            outStatus = osCallACPI_DSM(pGpu, ACPI_DSM_FUNCTION_NVHG, NVHG_FUNC_GETMEMTABLE, (NvU32*) pInOutData, &outDataSize);
            break;
        }
        case NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_GETMEMCFG:
        {
            outStatus = osCallACPI_DSM(pGpu, ACPI_DSM_FUNCTION_NVHG, NVHG_FUNC_GETMEMCFG, (NvU32*) pInOutData, &outDataSize);
            break;
        }
        case NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_GETOBJBYTYPE:
        {
            outStatus = osCallACPI_DSM(pGpu, ACPI_DSM_FUNCTION_NVHG, NVHG_FUNC_GETOBJBYTYPE, (NvU32*) pInOutData, &outDataSize);
            break;
        }
        case NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_GETALLOBJS:
        {
            outStatus = osCallACPI_DSM(pGpu, ACPI_DSM_FUNCTION_NVHG, NVHG_FUNC_GETALLOBJS, (NvU32*) pInOutData, &outDataSize);
            break;
        }
        case NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_GENERIC_CTL_GETSUPPORTEDFUNC:
        {
            if (outDataSize >= sizeof(pGpu->acpi.dsmCurrentFuncSupport))
            {
                outDataSize = sizeof(pGpu->acpi.dsmCurrentFuncSupport);
                portMemCopy(pInOutData, outDataSize, &pGpu->acpi.dsmCurrentFuncSupport, outDataSize);
                outStatus = NV_OK;
            }
            break;
        }
        case NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_GENERIC_DISPLAYSTATUS:
        {
            outStatus = osCallACPI_DSM(pGpu,
                                       pGpu->acpi.dsmCurrentFunc[NV_ACPI_GENERIC_FUNC_DISPLAYSTATUS-NV_ACPI_GENERIC_FUNC_START],
                                       pGpu->acpi.dsmCurrentSubFunc[NV_ACPI_GENERIC_FUNC_DISPLAYSTATUS-NV_ACPI_GENERIC_FUNC_START],
                                       (NvU32*) pInOutData,
                                       &outDataSize);
            break;
        }
        case NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_GENERIC_MDTL:
        {
            outStatus = osCallACPI_DSM(pGpu,
                                       pGpu->acpi.dsmCurrentFunc[NV_ACPI_GENERIC_FUNC_MDTL-NV_ACPI_GENERIC_FUNC_START],
                                       pGpu->acpi.dsmCurrentSubFunc[NV_ACPI_GENERIC_FUNC_MDTL-NV_ACPI_GENERIC_FUNC_START],
                                       (NvU32*) pInOutData,
                                       &outDataSize);
            break;
        }
        case NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_GENERIC_MSTL:
        {
            outStatus = osCallACPI_DSM(pGpu,
                                       pGpu->acpi.dsmCurrentFunc[NV_ACPI_GENERIC_FUNC_MSTL-NV_ACPI_GENERIC_FUNC_START],
                                       pGpu->acpi.dsmCurrentSubFunc[NV_ACPI_GENERIC_FUNC_MSTL-NV_ACPI_GENERIC_FUNC_START],
                                       (NvU32*) pInOutData,
                                       &outDataSize);
            break;
        }
        case NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_GENERIC_GETOBJBYTYPE:
        {
            outStatus = osCallACPI_DSM(pGpu,
                                       pGpu->acpi.dsmCurrentFunc[NV_ACPI_GENERIC_FUNC_GETOBJBYTYPE-NV_ACPI_GENERIC_FUNC_START],
                                       pGpu->acpi.dsmCurrentSubFunc[NV_ACPI_GENERIC_FUNC_GETOBJBYTYPE-NV_ACPI_GENERIC_FUNC_START],
                                       (NvU32*) pInOutData,
                                       &outDataSize);
            break;
        }
        case NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_GENERIC_GETALLOBJS:
        {
            outStatus = osCallACPI_DSM(pGpu,
                                       pGpu->acpi.dsmCurrentFunc[NV_ACPI_GENERIC_FUNC_GETALLOBJS-NV_ACPI_GENERIC_FUNC_START],
                                       pGpu->acpi.dsmCurrentSubFunc[NV_ACPI_GENERIC_FUNC_GETALLOBJS-NV_ACPI_GENERIC_FUNC_START],
                                       (NvU32*) pInOutData,
                                       &outDataSize);
            break;
        }
        case NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_GENERIC_GETEVENTLIST:
        {
            outStatus = osCallACPI_DSM(pGpu,
                                       pGpu->acpi.dsmCurrentFunc[NV_ACPI_GENERIC_FUNC_GETEVENTLIST-NV_ACPI_GENERIC_FUNC_START],
                                       pGpu->acpi.dsmCurrentSubFunc[NV_ACPI_GENERIC_FUNC_GETEVENTLIST-NV_ACPI_GENERIC_FUNC_START],
                                       (NvU32*) pInOutData,
                                       &outDataSize);
            break;
        }
        case NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_GENERIC_GETBACKLIGHT:
        {
            outStatus = osCallACPI_DSM(pGpu,
                                       pGpu->acpi.dsmCurrentFunc[NV_ACPI_GENERIC_FUNC_GETBACKLIGHT-NV_ACPI_GENERIC_FUNC_START],
                                       pGpu->acpi.dsmCurrentSubFunc[NV_ACPI_GENERIC_FUNC_GETBACKLIGHT-NV_ACPI_GENERIC_FUNC_START],
                                       (NvU32*) pInOutData,
                                       &outDataSize);
            break;
        }
        case NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_GENERIC_GETCALLBACKS:
        {
            outStatus = osCallACPI_DSM(pGpu,
                                       pGpu->acpi.dsmCurrentFunc[NV_ACPI_GENERIC_FUNC_CALLBACKS-NV_ACPI_GENERIC_FUNC_START],
                                       pGpu->acpi.dsmCurrentSubFunc[NV_ACPI_GENERIC_FUNC_CALLBACKS-NV_ACPI_GENERIC_FUNC_START],
                                       (NvU32*) pInOutData,
                                       &outDataSize);
            break;
        }
        case NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_NBCI_SUPPORTFUNCS:
        {
            outStatus = osCallACPI_DSM(pGpu,
                                       ACPI_DSM_FUNCTION_NBCI,
                                       NV_NBCI_FUNC_SUPPORT,
                                       (NvU32*) pInOutData,
                                       &outDataSize);
            break;
        }
        case NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_NBCI_PLATCAPS:
        {
            outStatus = osCallACPI_DSM(pGpu,
                                       ACPI_DSM_FUNCTION_NBCI,
                                       NV_NBCI_FUNC_PLATCAPS,
                                       (NvU32*) pInOutData,
                                       &outDataSize);
            break;
        }
        case NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_DSM_NBCI_PLATPOLICY:
        {
            outStatus = osCallACPI_DSM(pGpu,
                                       ACPI_DSM_FUNCTION_NBCI,
                                       NV_NBCI_FUNC_PLATPOLICY,
                                       (NvU32*) pInOutData,
                                       &outDataSize);
            break;
        }
        case NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_WMMX_NVOP_GPUON:
        {
            outStatus = osCallACPI_OPTM_GPUON(pGpu);
            break;
        }
        default:
        {
            NV_PRINTF(LEVEL_ERROR,
                      "ERROR: NV0073_CTRL_CMD_SYSTEM_EXECUTE_ACPI_METHOD: Unrecognized Api Code: 0x%x\n",
                      method);

            status = NV_ERR_INVALID_ARGUMENT;
            goto done;
        }
    }

    if (outStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_INFO,
                  "ERROR: NV0073_CTRL_CMD_SYSTEM_EXECUTE_ACPI_METHOD: Execution failed "
                  "for method: 0x%x, status=0x%x\n", method, outStatus);

        // if it was a command we tried return the real status.
        if (status == NV_OK)
            status = outStatus;

        // do we need: status = outStatus;  //XXX64 integration question
        pAcpiMethodParams->outStatus = outStatus;
        goto done;
    }

    // NOTE: 'outDataSize' may have changed above.
    if (outDataSize > pAcpiMethodParams->outDataSize)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "ERROR: NV0073_CTRL_CMD_SYSTEM_EXECUTE_ACPI_METHOD: output buffer is "
                  "smaller then expected!\n");

        //pAcpiMethodParams->outStatus = outStatus; //XXX64 check
        //status = outStatus; //XXX64 check
        status = NV_ERR_BUFFER_TOO_SMALL;
        goto done;
    }

    // all ok - so copy 'outdata' back to client
    bDoCopyOut = NV_TRUE;

done:

    // pass data out to client's output buffer
    if (bDoCopyOut)
    {
        pAcpiMethodParams->outDataSize = outDataSize;

        portMemCopy(NvP64_VALUE(pAcpiMethodParams->outData), outDataSize, pInOutData, outDataSize);
    }

    // release client's input buffer
    portMemFree(pInOutData);

    return status;
}

static NV_STATUS _dispDfpSwitchExternalMux
(
    OBJGPU *pGpu,
    NvU32  displayId,
    NvBool bSwitchItoD
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJPFM *pPfm = SYS_GET_PFM(pSys);
    NvU32   muxState = 0, acpiId = 0;
    NV_STATUS status = NV_ERR_GENERIC;

    acpiId = pfmFindAcpiId(pPfm, pGpu, displayId);
    if (acpiId != 0)
    {
        // Set the _MXDS acpi method input argument
        muxState = FLD_SET_REF_NUM(MXDS_METHOD_MUX_OP, MXDS_METHOD_MUX_OP_SET, muxState);
        if (bSwitchItoD)
        {
            muxState = FLD_SET_REF_NUM(MXDS_METHOD_MUX_SET_MODE, MXDS_METHOD_MUX_SET_MODE_DGPU, muxState);
        }
        else
        {
            muxState = FLD_SET_REF_NUM(MXDS_METHOD_MUX_SET_MODE, MXDS_METHOD_MUX_SET_MODE_IGPU, muxState);
        }

        status = osCallACPI_MXDS(pGpu, acpiId, &muxState);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "osCallACPI_MXDS failed 0x%x\n", status);
        }
    }

    return status;
}

static NV_STATUS _dispValidateDDSMuxSupport
(
    OBJGPU *pGpu,
    NvU32   displayId,
    NvBool *pIsEmbeddedDisplayPort
)
{
    KernelDisplay *pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);

    // Make sure there is one and only one display ID set
    if (!ONEBITSET(displayId))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (!pKernelDisplay->pStaticInfo->bExternalMuxSupported &&
        !pKernelDisplay->pStaticInfo->bInternalMuxSupported)
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    if (pKernelDisplay->pStaticInfo->embeddedDisplayPortMask & displayId)
    {
        *pIsEmbeddedDisplayPort = NV_TRUE;
    }
    
    return NV_OK;
}

/*!
 * @brief Control call to switch dynamic display MUX between integrated
 *       and discrete GPU
 *
 * @Parameter pDispCommon [In]
 * @Parameter pParams     [In, Out]
 *
 * @return
 *   NV_OK
 *     The request successfully completed.
 *   NV_ERR_NOT_SUPPORTED
 *     The Feature is not supported.
 *   NV_ERR_INVALID_ARGUMENT
 *     Invalid argument is passed.
 *
 */
NV_STATUS
dispcmnCtrlCmdDfpSwitchDispMux_IMPL
(
    DispCommon *pDispCommon,
    NV0073_CTRL_CMD_DFP_SWITCH_DISP_MUX_PARAMS *pParams
)
{
    OBJGPU *pGpu                     = DISPAPI_GET_GPU(pDispCommon);
    KernelDisplay *pKernelDisplay    = NULL;
    NvBool bSwitchItoD               = NV_TRUE;
    NvBool bEmbeddedDisplayPort      = NV_FALSE;
    RM_API *pRmApi                   = NULL;
    NV_STATUS status                 = NV_OK;

    // Get the right pGpu from subdevice instance given by client
    status = dispapiSetUnicastAndSynchronize_HAL(
                               staticCast(pDispCommon, DisplayApi),
                               DISPAPI_GET_GPUGRP(pDispCommon),
                               &pGpu,
                               NULL,
                               pParams->subDeviceInstance);
    if ((status != NV_OK) || (pGpu == NULL))
    {
        return status;
    }

    status = _dispValidateDDSMuxSupport(pGpu, pParams->displayId, &bEmbeddedDisplayPort);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Invalid arguments 0x%x\n", status);
        return status;
    }

    pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);

    if (FLD_TEST_DRF(0073_CTRL_DFP, _DISP_MUX, _SWITCH, _DGPU_TO_IGPU, pParams->flags))
    {
        bSwitchItoD = NV_FALSE;
    }

    // Check if current displayId belongs to external mux
    if (pKernelDisplay->pStaticInfo->bExternalMuxSupported && !bEmbeddedDisplayPort)
    {
        status = _dispDfpSwitchExternalMux(pGpu, pParams->displayId, bSwitchItoD);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "external mux switch failed 0x%x\n", status);
        }
    }
    else
    {
        // switch internal mux
        pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
        status = pRmApi->Control(pRmApi, RES_GET_CLIENT_HANDLE(pDispCommon), 
                                 RES_GET_HANDLE(pDispCommon),
                                 NV0073_CTRL_CMD_INTERNAL_DFP_SWITCH_DISP_MUX,
                                 pParams, sizeof(*pParams));
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "internal mux switch failed 0x%x\n", status);
        }
    }

    return status;
}

static NV_STATUS _dispDfpGetExternalMuxStatus
(
    OBJGPU *pGpu,
    NvU32  *pMuxStatus,
    NvU32   displayId
)
{
    OBJSYS *pSys      = SYS_GET_INSTANCE();
    OBJPFM *pPfm      = SYS_GET_PFM(pSys);
    NvU32 acpiId      = 0;
    NvU32 muxState    = 0;
    NvU32 mode        = 0;
    NvU32 acpiidIndex = 0;
    NV_STATUS status  = NV_OK;

    acpiId = pfmFindAcpiId(pPfm, pGpu, displayId);
    if (acpiId == 0)
    {
        NV_PRINTF(LEVEL_ERROR, "acpiId not found for displayId 0x%x\n", displayId);
        return NV_ERR_GENERIC;
    }

    // get mux state
    status = osCallACPI_MXDS(pGpu, acpiId, &muxState);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "ACPI call to get mux state failed.\n");
        return status;
    }

    // get mux mode
    status = NV_ERR_GENERIC;
    for (acpiidIndex = 0; acpiidIndex < pGpu->acpiMethodData.muxMethodData.tableLen; acpiidIndex++)
    {
        if (pGpu->acpiMethodData.muxMethodData.acpiIdMuxModeTable[acpiidIndex].acpiId == acpiId)
        {
            mode = pGpu->acpiMethodData.muxMethodData.acpiIdMuxModeTable[acpiidIndex].mode;
            status = pGpu->acpiMethodData.muxMethodData.acpiIdMuxModeTable[acpiidIndex].status;
            break;
        }
    }
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "ACPI lookup to get mux mode failed.\n");
        return status;
    }

    *pMuxStatus = FLD_SET_DRF_NUM(0073_CTRL_DFP, _DISP_MUX, _STATE,
                                         muxState, *pMuxStatus);

    *pMuxStatus = FLD_SET_DRF_NUM(0073_CTRL_DFP, _DISP_MUX, _MODE,
                                         mode, *pMuxStatus);

    return status;
}

/*!
 * @brief Control call to query display MUX status
 *
 * @Parameter pDispCommon [In]
 * @Parameter pParams     [In, Out]
 *
 * @return
 *   NV_OK
 *     The request successfully completed.
 *   NV_ERR_NOT_SUPPORTED
 *     The Feature is not supported.
 *   NV_ERR_INVALID_ARGUMENT
 *     Invalid argument is passed.
 *
 */
NV_STATUS
dispcmnCtrlCmdDfpGetDispMuxStatus_IMPL
(
    DispCommon *pDispCommon,
    NV0073_CTRL_CMD_DFP_GET_DISP_MUX_STATUS_PARAMS *pParams
)
{
    OBJGPU        *pGpu;
    KernelDisplay *pKernelDisplay    = NULL;
    NV_STATUS      status   = NV_OK;
    NvBool         bEmbeddedDisplayPort = NV_FALSE;
    RM_API         *pRmApi  = NULL;

    // Get the right pGpu from subdevice instance given by client
    status = dispapiSetUnicastAndSynchronize_HAL(
                               staticCast(pDispCommon, DisplayApi),
                               DISPAPI_GET_GPUGRP(pDispCommon),
                               &pGpu,
                               NULL,
                               pParams->subDeviceInstance);
    if (status != NV_OK || pGpu == NULL)
    {
        return status;
    }

    status = _dispValidateDDSMuxSupport(pGpu, pParams->displayId, &bEmbeddedDisplayPort);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Invalid arguments 0x%x\n", status);
        return status;
    }

    pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);

    // Check if current displayId belongs to external mux
    if (pKernelDisplay->pStaticInfo->bExternalMuxSupported && !bEmbeddedDisplayPort)
    {
        status = _dispDfpGetExternalMuxStatus(pGpu, &pParams->muxStatus, pParams->displayId);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "failed to get external mux status\n");
        }
    }
    else
    {
        // get internal mux status
        pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
        status = pRmApi->Control(pRmApi, RES_GET_CLIENT_HANDLE(pDispCommon), 
                                 RES_GET_HANDLE(pDispCommon),
                                 NV0073_CTRL_CMD_INTERNAL_DFP_GET_DISP_MUX_STATUS,
                                 pParams, sizeof(*pParams));
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "failed to get internal mux status\n");
        }
    }

    return status;
}
