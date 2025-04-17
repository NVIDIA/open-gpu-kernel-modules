/*
 * SPDX-FileCopyrightText: Copyright (c) 2014-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * @file
 * @brief Hypervisor interface for RM
 */

#include "gpu_mgr/gpu_mgr.h"
#include "gpu/gpu.h"
#include "virtualization/hypervisor/hypervisor.h"
#include "os/os.h"
#include "nvRmReg.h"

static HYPERVISOR_OPS _hypervisorOps[OS_HYPERVISOR_UNKNOWN];

static NV_STATUS _hypervisorDetection_HVM(OBJHYPERVISOR *);
static NvBool _hypervisorCheckVirtualPcieP2PApproval(OBJHYPERVISOR *, NvU32);
static NvBool _hypervisorCheckVirtualPcieP2PGeneralApproval (NvU32);

static void _hypervisorLoad(void)
{
    _hypervisorOps[OS_HYPERVISOR_XEN]    = xenHypervisorOps;
    _hypervisorOps[OS_HYPERVISOR_KVM]    = kvmHypervisorOps;
    _hypervisorOps[OS_HYPERVISOR_HYPERV] = hypervHypervisorOps;
    _hypervisorOps[OS_HYPERVISOR_VMWARE] = vmwareHypervisorOps;
}

void hypervisorDestruct_IMPL(OBJHYPERVISOR *pHypervisor)
{
    pHypervisor->type = OS_HYPERVISOR_UNKNOWN;
}

NV_STATUS hypervisorConstruct_IMPL(OBJHYPERVISOR *pHypervisor)
{
    _hypervisorLoad();

    pHypervisor->type = OS_HYPERVISOR_UNKNOWN;

    pHypervisor->bDetected = NV_FALSE;

    return NV_OK;
}

NvBool hypervisorPcieP2pDetection_IMPL
(
    OBJHYPERVISOR *pHypervisor,
    NvU32 gpuMask
)
{
    if (_hypervisorCheckVirtualPcieP2PApproval(pHypervisor, gpuMask))
        return NV_TRUE;
    if (_hypervisorCheckVirtualPcieP2PGeneralApproval(gpuMask))
        return NV_TRUE;
    if (hypervisorIsVgxHyper())
        return NV_FALSE;

    return _hypervisorOps[pHypervisor->type].hypervisorIsPcieP2PSupported(gpuMask);
}

NV_STATUS hypervisorDetection_IMPL
(
    OBJHYPERVISOR *pHypervisor,
    OBJOS *pOS
)
{
    NV_STATUS rmStatus = NV_OK;

    if (hypervisorIsVgxHyper())
        goto found_one;

    if ((rmStatus = _hypervisorDetection_HVM(pHypervisor)) != NV_OK)
        return rmStatus;

    if ((rmStatus = _hypervisorOps[pHypervisor->type].hypervisorPostDetection(pOS, &pHypervisor->bIsHVMGuest)) != NV_OK)
        return rmStatus;

    if (pHypervisor->type == OS_HYPERVISOR_HYPERV && !pHypervisor->bIsHVMGuest)
    {
        pHypervisor->bIsHypervHost = NV_TRUE;
    }

found_one:
    pHypervisor->bDetected = NV_TRUE;

    if (pHypervisor->bIsHVMGuest)
    {
        NV_PRINTF(LEVEL_NOTICE,
                  "Found HVM kernel running on hypervisor: %s.\n",
                  _hypervisorOps[pHypervisor->type].hypervisorName);
    }
    else if (hypervisorIsVgxHyper())
    {
        NV_PRINTF(LEVEL_NOTICE,
                  "Found PV kernel running with vGPU hypervisor.\n");
    }

    return NV_OK;
}

static NV_STATUS _hypervisorDetection_HVM
(
    OBJHYPERVISOR *pHypervisor
)
{
#if defined(NVCPU_X86_64)
    NvU32 i = 0, base, eax = 0;
    NvU32 vmmSignature[3];
    NvU32 leaf = 0;

    for (base = 0x40000000; base < 0x40001000; base += 0x100)
    {
        if (osNv_cpuid(base, 0, &eax,
                &vmmSignature[0], &vmmSignature[1], &vmmSignature[2]) == 0)
        {
            NV_PRINTF(LEVEL_WARNING, "CPUID is NOT supported!\n");

            return NV_ERR_NOT_SUPPORTED;
        }

        //
        // Looking for the highest leaf as the official hypervisor
        // For example: Xen will implement Viridian at 0x40000000 but
        // push himself to 0x40000100
        //
        for (i = 0; i < OS_HYPERVISOR_UNKNOWN; i++)
        {
            if (!portMemCmp(_hypervisorOps[i].hypervisorSig,
                          vmmSignature, sizeof(vmmSignature)))
            {
                if (base > leaf)
                {
                    leaf = base;
                    pHypervisor->type = i;
                }
            }
        }
    }

    return leaf ? NV_OK : NV_ERR_NOT_SUPPORTED;
#elif defined(NVCPU_AARCH64)
    void *pSmbiosTable = NULL;
    NvU8 *tableStart = NULL;
    NvU64 i = 0;
    NvU64 totalLength = 0;
    NvU64 numSubTypes;
    NvU32 version;
    NvBool bIsVM = NV_FALSE;

    if (osGetSmbiosTable(&pSmbiosTable, &totalLength, &numSubTypes, &version) != NV_OK)
    {
        NV_PRINTF(LEVEL_WARNING, "SMBIOS is NOT supported!\n");
        return NV_ERR_NOT_SUPPORTED;
    }

    NV_ASSERT_OR_RETURN(pSmbiosTable, NV_ERR_INVALID_POINTER);
    tableStart = (NvU8 *)pSmbiosTable;

    // Traverse SMBIOS table to locate 'BIOS Information (Type 0)' struct
    while (i < totalLength)
    {
        NvU8 *structStart;
        NvU8 structType;
        NvU8 structLength;

        // SMBIOS Spec: https://www.dmtf.org/sites/default/files/standards/documents/DSP0134_3.4.0.pdf
        // Each SMBIOS struct has a formatted section followed by optional strings section.
        // type of struct is at byte offset 0.
        // length of formatted section is at byte offset 1.
        // optional strings section starts at byte offset structLength.
        // struct end is marked with two null bytes.

        if ((i + 2ULL) <= totalLength)
        {
            structStart  = &tableStart[i];
            structType   = structStart[0];
            structLength = structStart[1];
        }
        else
        {
            break;
        }

        if (structType == 0x7f)
        {
            // 'End-of-Table (Type 127)' struct reached, stop traversing further.
            break;
        }

        // traverse formatted section
        i += structLength;

        // traverse optional strings section until start of two null bytes
        while (((i + 2ULL) <= totalLength) && (tableStart[i] || tableStart[i + 1]))
            i++;

        // ensure that entire struct (including last two null bytes) is within tableLength
        if ((i + 2ULL) <= totalLength)
        {
            if (structType == 0x0)
            {
                // found 'BIOS Information (Type 0)' struct.
                // check Bit 4 of 'BIOS Characteristics Extention Byte 2' - offset 0x13
                if ((structLength > 0x13) && (structStart[0x13] & NVBIT(4)))
                {
                    bIsVM = NV_TRUE;
                }
                break;
            }
        }

        // traverse over two null bytes at end of a struct
        i += 2;
    }

    osPutSmbiosTable(pSmbiosTable, totalLength);

    if (bIsVM)
    {
        pHypervisor->type = OS_HYPERVISOR_KVM;
        return NV_OK;
    }

    return NV_ERR_NOT_SUPPORTED;
#else
    return NV_ERR_NOT_SUPPORTED;
#endif
}

HYPERVISOR_TYPE hypervisorGetHypervisorType_IMPL(OBJHYPERVISOR *pHypervisor)
{
    if (pHypervisor)
        return pHypervisor->type;
    return OS_HYPERVISOR_UNKNOWN;
}

void hypervisorSetHypervisorType_IMPL(OBJHYPERVISOR *pHypervisor, HYPERVISOR_TYPE type)
{
    pHypervisor->type = type;
    pHypervisor->bDetected = type != OS_HYPERVISOR_UNKNOWN;
}

static NvBool _hypervisorCheckVirtualPcieP2PApproval
(
    OBJHYPERVISOR *pHypervisor,
    NvU32 gpuMask
)
{
    OBJGPU *pGpu;
    NvU32 gpuInstance = 0;
    NvU8 peerCliqueId = 0xFF;

    //
    // We provide a way for any hypervisor (either within and without NVIDIA)
    // to indicate peer-to-peer capability among GPUs by specifying a peer
    // "clique" ID for each GPU in its PCI configuration space. This value is
    // read during GPU initialization for virtual GPUs, and GPUs in the same
    // clique are then assumed by the RM to be capable of PCI-E P2P.
    //
    // By specifying a peer clique ID for a GPU, the hypervisor warrants that
    // PCI-E P2P has been tested and works correctly between all GPUs with the
    // same clique ID on the bare-metal platform under the virtual machine we
    // are currently in.
    //
    while ((pGpu = gpumgrGetNextGpu(gpuMask, &gpuInstance)) != NULL)
    {
        if (!pGpu->pciePeerClique.bValid)
        {
            return NV_FALSE;
        }

        if (peerCliqueId == 0xFF)
        {
            peerCliqueId = pGpu->pciePeerClique.id;
        }
        else if (peerCliqueId != pGpu->pciePeerClique.id)
        {
            return NV_FALSE;
        }
    }

    //
    // All GPUs in gpuMask are in the same peer clique as identified by the
    // hypervisor, so allow P2P.
    //
    return NV_TRUE;
}

static NvBool _hypervisorCheckVirtualPcieP2PGeneralApproval
(
    NvU32 gpuMask
)
{
    OBJGPU *pGpu;
    NvU32 gpuInstance = 0;
    NV_STATUS status = NV_OK;

    while ((pGpu = gpumgrGetNextGpu(gpuMask, &gpuInstance)) != NULL)
    {
        if(!(IS_PASSTHRU(pGpu)))
        {
            return NV_FALSE;
        }
        
        RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
        NV2080_CTRL_INTERNAL_GET_PCIE_P2P_CAPS_PARAMS p2pCapsParams = {0};
        
        status = pRmApi->Control(pRmApi, 
                                pGpu->hInternalClient,
                                pGpu->hInternalSubdevice,
                                NV2080_CTRL_CMD_INTERNAL_GET_PCIE_P2P_CAPS,
                                &p2pCapsParams,
                                sizeof(NV2080_CTRL_INTERNAL_GET_PCIE_P2P_CAPS_PARAMS));
            
        if ((status != NV_OK) ||
            (p2pCapsParams.p2pReadCapsStatus != NV0000_P2P_CAPS_STATUS_OK) ||
            (p2pCapsParams.p2pWriteCapsStatus != NV0000_P2P_CAPS_STATUS_OK))
        {
            return NV_FALSE;
        }
    }

    return NV_TRUE;
}

NvBool hypervisorIsType_IMPL(HYPERVISOR_TYPE hyperType)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJHYPERVISOR *pHypervisor = SYS_GET_HYPERVISOR(pSys);

    if (!pHypervisor)
        return NV_FALSE;

    return (pHypervisor->type == hyperType);
}
