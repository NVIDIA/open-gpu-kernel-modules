/*
 * SPDX-FileCopyrightText: Copyright (c) 2011-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/subdevice/subdevice.h"
#include <class/cl90f1.h>  // FERMI_VASPACE_A
#include <rmp2pdefines.h>
#include "gpu/bus/third_party_p2p.h"
#include "mem_mgr/mem.h"
#include "rmapi/rs_utils.h"
#include "vgpu/rpc.h"

NV_STATUS
thirdpartyp2pCtrlCmdRegisterVaSpace_IMPL
(
    ThirdPartyP2P *pThirdPartyP2P,
    NV503C_CTRL_REGISTER_VA_SPACE_PARAMS *pRegisterVaSpaceParams
)
{
    NvU32     vaSpaceToken;
    NV_STATUS status;
    OBJGPU *pGpu;

    pGpu = GPU_RES_GET_GPU(pThirdPartyP2P);
    if (pGpu == NULL)
        return NV_ERR_INVALID_OBJECT_PARENT;


    status = CliAddThirdPartyP2PVASpace(pThirdPartyP2P,
                                        pRegisterVaSpaceParams->hVASpace,
                                        &vaSpaceToken);
    if (status == NV_OK)
    {
        {
            pRegisterVaSpaceParams->vaSpaceToken = vaSpaceToken;
        }
    }

    return status;
}

NV_STATUS
thirdpartyp2pCtrlCmdUnregisterVaSpace_IMPL
(
    ThirdPartyP2P *pThirdPartyP2P,
    NV503C_CTRL_UNREGISTER_VA_SPACE_PARAMS *pUnregisterVaSpaceParams
)
{
    NV_STATUS status = NV_OK;
    OBJGPU *pGpu;

    pGpu = GPU_RES_GET_GPU(pThirdPartyP2P);
    if (pGpu == NULL)
        return NV_ERR_INVALID_OBJECT_PARENT;

    status = CliDelThirdPartyP2PVASpace(pThirdPartyP2P,
                                        pUnregisterVaSpaceParams->hVASpace);

    return status;
}

NV_STATUS
thirdpartyp2pCtrlCmdRegisterVidmem_IMPL
(
    ThirdPartyP2P *pThirdPartyP2P,
    NV503C_CTRL_REGISTER_VIDMEM_PARAMS *pRegisterVidmemParams
)
{
    Memory    *pMemory;
    RsClient  *pClient = RES_GET_CLIENT(pThirdPartyP2P);
    NvHandle   hDevice;
    NvU64      address = pRegisterVidmemParams->address;
    NvU64      size    = pRegisterVidmemParams->size;
    NvU64      offset  = pRegisterVidmemParams->offset;
    NV_STATUS status = NV_OK;
    OBJGPU *pGpu;

    pGpu = GPU_RES_GET_GPU(pThirdPartyP2P);
    if (pGpu == NULL)
        return NV_ERR_INVALID_OBJECT_PARENT;

    hDevice = RES_GET_PARENT_HANDLE(pThirdPartyP2P->pSubdevice);

    status = memGetByHandleAndDevice(pClient,
                                     pRegisterVidmemParams->hMemory,
                                     hDevice,
                                     &pMemory);
    if (status != NV_OK)
    {
        return status;
    }

    if (memdescGetAddressSpace(pMemory->pMemDesc) != ADDR_FBMEM)
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    if (size == 0)
        return NV_ERR_INVALID_ARGUMENT;

    if (size & (NVRM_P2P_PAGESIZE_BIG_64K - 1))
        return NV_ERR_INVALID_ARGUMENT;

    if (address & (NVRM_P2P_PAGESIZE_BIG_64K - 1))
        return NV_ERR_INVALID_ARGUMENT;

    if (offset & (NVRM_P2P_PAGESIZE_BIG_64K - 1))
        return NV_ERR_INVALID_ARGUMENT;

    // Check for overflow
    if (address + size < address || size + offset < size)
        return NV_ERR_INVALID_ARGUMENT;

    if (memdescGetSize(pMemory->pMemDesc) < offset + size)
        return NV_ERR_INVALID_ARGUMENT;

    status = CliAddThirdPartyP2PVidmemInfo(pThirdPartyP2P,
                                           pRegisterVidmemParams->hMemory,
                                           address,
                                           size,
                                           offset,
                                           pMemory);
    if (status != NV_OK)
    {
        return status;
    }

    return status;
}

NV_STATUS
thirdpartyp2pCtrlCmdUnregisterVidmem_IMPL
(
    ThirdPartyP2P *pThirdPartyP2P,
    NV503C_CTRL_UNREGISTER_VIDMEM_PARAMS *pUnregisterVidmemParams
)
{
    NV_STATUS status = NV_OK;
    OBJGPU *pGpu;

    pGpu = GPU_RES_GET_GPU(pThirdPartyP2P);
    if (pGpu == NULL)
        return NV_ERR_INVALID_OBJECT_PARENT;

    status = CliDelThirdPartyP2PVidmemInfo(pThirdPartyP2P,
                                           pUnregisterVidmemParams->hMemory);
    return status;
}

NV_STATUS
thirdpartyp2pCtrlCmdRegisterPid_IMPL
(
    ThirdPartyP2P *pThirdPartyP2P,
    NV503C_CTRL_REGISTER_PID_PARAMS *pRegisterPidParams
)
{
    NvHandle  hClient = RES_GET_CLIENT_HANDLE(pThirdPartyP2P);
    NvHandle  hObject = RES_GET_HANDLE(pThirdPartyP2P);
    RmClient *pClient;
    NvU32     pid;
    NV_STATUS status;

    pClient = serverutilGetClientUnderLock(pRegisterPidParams->hClient);
    NV_ASSERT_OR_RETURN(pClient != NULL, NV_ERR_INVALID_CLIENT);

    pid = pClient->ProcID;

    status = CliAddThirdPartyP2PClientPid(hClient,
                                          hObject,
                                          pid,
                                          pRegisterPidParams->hClient);
    return status;
}
