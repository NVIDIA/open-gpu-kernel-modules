/*
 * SPDX-FileCopyrightText: Copyright (c) 2023-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _CHANNEL_UTILS_H_
#define _CHANNEL_UTILS_H_

#include "core/core.h"
#include "gpu/gpu.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "gpu/ce/kernel_ce.h"
#include "gpu/bus/kern_bus.h"
#include "core/prelude.h"
#include "rmapi/rs_utils.h"
#include "nvos.h"

#include "class/cl906f.h"
#include "class/cl906f.h"
#include "class/cl906fsw.h"

#include "class/clb0b5.h"   // MAXWELL_DMA_COPY_A
#include "class/clc0b5.h"   // PASCAL_DMA_COPY_A
#include "class/clc1b5.h"   // PASCAL_DMA_COPY_B
#include "class/clc3b5.h"   // VOLTA_DMA_COPY_A
#include "class/clc5b5.h"   // TURING_DMA_COPY_A
#include "class/clc6b5.h"   // AMPERE_DMA_COPY_A
#include "class/clc7b5.h"   // AMPERE_DMA_COPY_B

#include "class/clc8b5.h"   // HOPPER_DMA_COPY_A

#include "class/clc86f.h"   // HOPPER_CHANNEL_GPFIFO_A

#include "class/clc9b5.h"      // BLACKWELL_DMA_COPY_A

#include "gpu/conf_compute/ccsl.h"

#include "nvctassert.h"
#include "vgpu/vgpu_guest_pma_scrubber.h"

#define RM_SUBCHANNEL 0x0

#define NV_PUSH_METHOD(OpType, SubCh, Method, Count)                    \
    (DRF_DEF(906F, _DMA, _SEC_OP, OpType) |                             \
     DRF_NUM(906F, _DMA, _METHOD_ADDRESS, (Method) >> 2) |              \
     DRF_NUM(906F, _DMA, _METHOD_SUBCHANNEL, (SubCh)) |                 \
     DRF_NUM(906F, _DMA, _METHOD_COUNT, (Count)))

#define _NV_ASSERT_CONTIGUOUS_METHOD(a1, a2) NV_ASSERT((a2) - (a1) == 4)

#define NV_PUSH_DATA(Data) MEM_WR32(pPtr++, (Data))

#define _NV_PUSH_INC_1U(SubCh, a1, d1, Count)                           \
    do                                                                  \
    {                                                                   \
        NV_PUSH_DATA(NV_PUSH_METHOD(_INC_METHOD, SubCh, a1, Count));    \
        NV_PUSH_DATA(d1);                                               \
    } while (0)

#define NV_PUSH_INC_1U(SubCh, a1, d1)                                   \
    do                                                                  \
    {                                                                   \
        _NV_PUSH_INC_1U (SubCh, a1, d1, 1);                             \
    } while (0)

#define NV_PUSH_INC_2U(SubCh, a1, d1, a2, d2)                           \
    do                                                                  \
    {                                                                   \
        _NV_ASSERT_CONTIGUOUS_METHOD(a1, a2);                           \
        _NV_PUSH_INC_1U(SubCh, a1, d1, 2);                              \
        NV_PUSH_DATA(d2);                                               \
    } while (0)

#define NV_PUSH_INC_3U(SubCh, a1, d1, a2, d2, a3, d3)                   \
    do                                                                  \
    {                                                                   \
        _NV_ASSERT_CONTIGUOUS_METHOD(a1, a2);                           \
        _NV_ASSERT_CONTIGUOUS_METHOD(a2, a3);                           \
        _NV_PUSH_INC_1U(SubCh, a1, d1, 3);                              \
        NV_PUSH_DATA(d2);                                               \
        NV_PUSH_DATA(d3);                                               \
    } while (0)

#define NV_PUSH_INC_4U(SubCh, a1, d1, a2, d2, a3, d3, a4, d4)           \
    do                                                                  \
    {                                                                   \
        _NV_ASSERT_CONTIGUOUS_METHOD(a1, a2);                           \
        _NV_ASSERT_CONTIGUOUS_METHOD(a2, a3);                           \
        _NV_ASSERT_CONTIGUOUS_METHOD(a3, a4);                           \
        _NV_PUSH_INC_1U(SubCh, a1, d1, 4);                              \
        NV_PUSH_DATA(d2);                                               \
        NV_PUSH_DATA(d3);                                               \
        NV_PUSH_DATA(d4);                                               \
    } while (0)

#define READ_CHANNEL_PAYLOAD_SEMA(channel)  channelReadChannelMemdesc(channel, channel->finishPayloadOffset)
#define READ_CHANNEL_PB_SEMA(channel)       channelReadChannelMemdesc(channel, channel->semaOffset)

// 
// This struct contains parameters needed to send a pushbuffer for a CE
// operation. This interface only supports contiguous operations. 
//
typedef struct 
{
    NvBool bCeMemcopy;   // Whether this is a CE memcopy; 
                         // If set to false, this will be a memset operation
    NvU64 dstAddr;       // Physical address of the source address
    NvU64 srcAddr;       // Physical address of the source address; only valid for memcopy
    NvU32 size;
    NvU32 pattern;       // Fixed pattern to memset to. Only valid for memset
    NvU32 payload;       // Payload value used to release semaphore
    NvU64 clientSemaAddr;
    NV_ADDRESS_SPACE dstAddressSpace;
    NV_ADDRESS_SPACE srcAddressSpace;
    NvU32 dstCpuCacheAttrib;
    NvU32 srcCpuCacheAttrib;

    NvBool bSecureCopy; // The copy encrypts/decrypts protected memory
    NvBool bEncrypt; // encrypt/decrypt
    NvU64 authTagAddr;
    NvU64 encryptIvAddr;

} CHANNEL_PB_INFO;

NV_STATUS channelSetupIDs(OBJCHANNEL *pChannel, OBJGPU *pGpu, NvBool bUseVasForCeCopy, NvBool bMIGInUse);
NV_STATUS channelAllocSubdevice(OBJGPU *pGpu, OBJCHANNEL *pChannel);
void channelSetupChannelBufferSizes(OBJCHANNEL *pChannel);
NvU32 channelReadChannelMemdesc(OBJCHANNEL *pChannel, NvU32 offset);

// Needed for pushbuffer management
NV_STATUS channelWaitForFreeEntry(OBJCHANNEL *pChannel, NvU32 *pPutIndex);
NV_STATUS channelFillGpFifo(OBJCHANNEL *pChannel, NvU32 putIndex, NvU32 methodsLength);
NvU32 channelFillCePb(OBJCHANNEL *pChannel, NvU32 putIndex, NvBool bPipelined,
                      NvBool bInsertFinishPayload, CHANNEL_PB_INFO *pChannelPbInfo);
NvU32 channelFillPbFastScrub(OBJCHANNEL *pChannel, NvU32 putIndex, NvBool bPipelined,
                    NvBool bInsertFinishPayload, CHANNEL_PB_INFO *pChannelPbInfo);

NV_STATUS channelFillSec2Pb(OBJCHANNEL *pChannel, NvU32 putIndex, NvBool bInsertFinishPayload,
                            CHANNEL_PB_INFO *pChannelPbInfo, CCSL_CONTEXT *pCcslCtx, 
                            MEMORY_DESCRIPTOR *pScrubMemDesc, MEMORY_DESCRIPTOR *pSemaMemDesc,
                            NvU64 scrubMthdAuthTagBufGpuVA, NvU32 scrubAuthTagBufIndex,
                            NvU64 semaMthdAuthTagBufGpuVA, NvU32 semaAuthTagBufIndex, NvU32* methodLength);

// Needed for work tracking
NV_STATUS channelWaitForFinishPayload(OBJCHANNEL *pChannel, NvU64 targetPayload);
NvU64 channelGetFinishPayload(OBJCHANNEL *pChannel);

void channelServiceScrubberInterrupts(OBJCHANNEL *pChannel);

#endif // _CHANNEL_UTILS_H_
