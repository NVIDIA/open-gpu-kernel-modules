/*
 * SPDX-FileCopyrightText: Copyright (c) 2024-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "g_kernel_hfrp_nvoc.h"

#ifndef _KERNELHFRP_H_
#define _KERNELHFRP_H_

#include "nvtypes.h"
#include "nvstatus.h"
#include "nvmisc.h"
#include "utils/nvprintf.h"

#include "os/os.h"
#include "gpu/eng_state.h"
#include "gpu/gpu.h"
#include "gpu/hfrp/kern_hfrp_common.h"

// Total number of HFRP Mailboxes available for the interface
#define HFRP_NUMBER_OF_MAILBOXES            2U

// Maximum Payload size for a message
#define HFRP_MAX_PAYLOAD_SIZE               50U

#define HFRP_COMMAND_MAILBOX_INDEX          HFRP_COMMAND_MAILBOX_INDEX_DISPLAY
#define HFRP_RESPONSE_MAILBOX_INDEX         HFRP_RESPONSE_MAILBOX_INDEX_DISPLAY

//
// Maximum values of Sequence Id index and Sequence Id Array index (each
// Sequence Id array element has 32 bits that represent 32 Sequence Ids)
//
#define HFRP_NUMBER_OF_SEQUENCEID_INDEX             0x400
#define HFRP_NUMBER_OF_SEQUENCEID_ARRAY_INDEX       (HFRP_NUMBER_OF_SEQUENCEID_INDEX / 32U)
#define HFRP_ASYNC_NOTIFICATION_SEQUENCEID_INDEX    0x3FF

NVOC_PREFIX(khfrp) class KernelHFRP: OBJENGSTATE
{
public:
    /*! HFRP Create Object */
    virtual NV_STATUS khfrpStatePreInitLocked(OBJGPU *pGpu, KernelHFRP *pHfrp);

    virtual NV_STATUS khfrpConstructEngine(OBJGPU *pGpu, KernelHFRP *pHfrp, ENGDESCRIPTOR engDesc);

    /*! HFRP Destructor */
    void khfrpDestruct(KernelHFRP *pHfrp);

    void khfrpCommonConstruct(KernelHFRP *pHfrp);

    NV_STATUS khfrpIoApertureConstruct(OBJGPU *pGpu, KernelHFRP *pHfrp);

    void khfrpIoApertureDestruct(KernelHFRP *pHfrp, NvU32 index);

    NvU32 khfrpReadBit(KernelHFRP *pHfrp, NvU32 virtualAddr, NvU32 bitIndex);

    void khfrpWriteBit(KernelHFRP *pHfrp, NvU32 virtualAddr, NvU32 bitIndex, NvU32 data);

    NV_STATUS khfrpMailboxQueueMessage(KernelHFRP *pHfrp, NvU32 messageHeader, NvU8 *pPayloadArray,
                                      NvU32 payloadSize, NvU32 mailboxFlag);

    void khfrpServiceEvent(KernelHFRP *pHfrp);

    NvU32 khfrpAllocateSequenceId(KernelHFRP *pHfrp, NvU16 *pResponseStatus, void *pResponsePayload,
                                      NvU32 *pResponsePayloadSize, NV_STATUS *pStatus, NvU32 *pSequenceId);

    void khfrpFreeSequenceId(KernelHFRP *pHfrp, NvU32 index);

    NvBool khfrpIsSequenceIdFree(KernelHFRP *pHfrp, NvU32 index);

    NV_STATUS khfrpPollOnIrqWrapper(KernelHFRP *pHfrp, NvU32 irqRegAddr, NvU32 bitIndex, NvBool bData);

    NV_STATUS khfrpPollOnIrqRm(KernelHFRP *pHfrp, NvU32 irqRegAddr, NvU32 bitIndex, NvBool bData);

    NV_STATUS khfrpPostCommandBlocking(KernelHFRP *pHfrp, NvU16 commandIndex, void *pCommandPayload, NvU32 commandPayloadSize,
                                           NvU16 *pResponseStatus, void *pResponsePayload, NvU32 *pResponsePayloadSize,  NV_STATUS *pStatus);

    NV_STATUS khfrpInterfaceReset(KernelHFRP *pHfrp);

    NVOC_PROPERTY NvBool PDB_PROP_KHFRP_IS_ENABLED;

    NvU32       khfrpPrivBase[5];
    NvU32       khfrpIntrCtrlReg[5];
    IoAperture  *pAperture[HFRP_NUMBER_OF_MAILBOXES];
    HFRP_INFO   khfrpInfo;
};

#define HFRP_REG_RD32(pKernelHfrp, virtualAddr)                                \
    REG_RD32(pKernelHfrp->pAperture[virtualAddr / HFRP_MAILBOX_ACCESS_RANGE],  \
             virtualAddr % HFRP_MAILBOX_ACCESS_RANGE)

#define HFRP_REG_WR32(pKernelHfrp, virtualAddr, data32)                        \
    REG_WR32(pKernelHfrp->pAperture[virtualAddr / HFRP_MAILBOX_ACCESS_RANGE],  \
             virtualAddr % HFRP_MAILBOX_ACCESS_RANGE, data32)

#define HFRP_POLL_ON_IRQ(pKernelHfrp, irqRegAddr, bitIndex, bData)             \
    khfrpPollOnIrqRm(pKernelHfrp, irqRegAddr, bitIndex, bData)

#endif  // _KernelHFRP_H_
