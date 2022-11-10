/*
 * SPDX-FileCopyrightText: Copyright (c) 2014-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "gpu/gpu.h"
#include "virtualization/hypervisor/hypervisor.h"
#include "os/os.h"
#include "nvRmReg.h"

static HYPERVISOR_OPS _hypervisorOps[OS_HYPERVISOR_UNKNOWN];

static NV_STATUS _hypervisorDetection_HVM(OBJHYPERVISOR *, OBJOS *, NvU32 *);
static NvBool _hypervisorCheckVirtualPcieP2PApproval(OBJHYPERVISOR *, NvU32);

// Because M$ compiler doesn't support C99 we have to initialize
// the struct array in this C function, ugly.

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

static NvU32 _leaf;

NvBool hypervisorPcieP2pDetection_IMPL
(
    OBJHYPERVISOR *pHypervisor,
    NvU32 gpuMask
)
{
    if (_hypervisorCheckVirtualPcieP2PApproval(pHypervisor, gpuMask))
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

    if ((rmStatus = _hypervisorDetection_HVM(pHypervisor, pOS, &_leaf)) != NV_OK)
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
        NV_PRINTF(LEVEL_WARNING,
                  "Found HVM kernel running on hypervisor:%s!\n",
                  _hypervisorOps[pHypervisor->type].hypervisorName);
    }
    else if (hypervisorIsVgxHyper())
    {
        NV_PRINTF(LEVEL_WARNING,
                  "Found PV kernel running with vGPU hypervisor!\n");
    }

    return NV_OK;
}

static NV_STATUS _hypervisorDetection_HVM
(
    OBJHYPERVISOR *pHypervisor,
    OBJOS *pOS,
    NvU32 *pLeaf
)
{
    NvU32 i = 0, base, eax = 0;
    NvU32 vmmSignature[3];

    NV_ASSERT_OR_RETURN(pLeaf,  NV_ERR_INVALID_ARGUMENT);
    *pLeaf = 0;

    for (base = 0x40000000; base < 0x40001000; base += 0x100)
    {
        if (pOS->osNv_cpuid(pOS, base, 0, &eax,
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
                if (base > *pLeaf)
                {
                    *pLeaf = base;
                    pHypervisor->type = i;
                }
            }
        }
    }

    return *pLeaf ? NV_OK : NV_ERR_NOT_SUPPORTED;
}

HYPERVISOR_TYPE hypervisorGetHypervisorType_IMPL(OBJHYPERVISOR *pHypervisor)
{
    if (pHypervisor)
        return pHypervisor->type;
    return OS_HYPERVISOR_UNKNOWN;
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

NvBool hypervisorIsType_IMPL(HYPERVISOR_TYPE hyperType)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJHYPERVISOR *pHypervisor = SYS_GET_HYPERVISOR(pSys);

    if (!pHypervisor)
        return NV_FALSE;

    return (pHypervisor->type == hyperType);
}
