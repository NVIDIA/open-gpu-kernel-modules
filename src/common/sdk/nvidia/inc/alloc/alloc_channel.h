/*
 * SPDX-FileCopyrightText: Copyright (c) 2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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


#pragma once

#include <nvtypes.h>

//
// This file was generated with FINN, an NVIDIA coding tool.
// Source file:      alloc/alloc_channel.finn
//

#include "nvlimits.h"
#include "nvcfg_sdk.h"

typedef struct NV_MEMORY_DESC_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 base, 8);
    NV_DECLARE_ALIGNED(NvU64 size, 8);
    NvU32 addressSpace;
    NvU32 cacheAttrib;
} NV_MEMORY_DESC_PARAMS;

/*
 * NV_CHANNEL_ALLOC_PARAMS.flags values.
 *
 * These flags may apply to all channel types: PIO, DMA, and GPFIFO.
 * They are also designed so that zero is always the correct default.
 *
 *   NVOS04_FLAGS_CHANNEL_TYPE:
 *     This flag specifies the type of channel to allocate.  Legal values
 *     for this flag include:
 *
 *       NVOS04_FLAGS_CHANNEL_TYPE_PHYSICAL:
 *         This flag specifies that a physical channel is to be allocated.
 *
 *       NVOS04_FLAGS_CHANNEL_TYPE_VIRTUAL:
 *         OBSOLETE - NOT SUPPORTED
 *
 *       NVOS04_FLAGS_CHANNEL_TYPE_PHYSICAL_FOR_VIRTUAL:
 *         OBSOLETE - NOT SUPPORTED
 */

/* valid NVOS04_FLAGS_CHANNEL_TYPE values */
#define NVOS04_FLAGS_CHANNEL_TYPE                                  1:0
#define NVOS04_FLAGS_CHANNEL_TYPE_PHYSICAL                         0x00000000
#define NVOS04_FLAGS_CHANNEL_TYPE_VIRTUAL                          0x00000001  // OBSOLETE
#define NVOS04_FLAGS_CHANNEL_TYPE_PHYSICAL_FOR_VIRTUAL             0x00000002  // OBSOLETE

/*
 *    NVOS04_FLAGS_VPR:
 *     This flag specifies if channel is intended for work with
 *     Video Protected Regions (VPR)
 *
 *       NVOS04_FLAGS_VPR_TRUE:
 *         The channel will only write to protected memory regions.
 *
 *       NVOS04_FLAGS_VPR_FALSE:
 *         The channel will never read from protected memory regions.
 */
#define NVOS04_FLAGS_VPR                                           2:2
#define NVOS04_FLAGS_VPR_FALSE                                     0x00000000
#define NVOS04_FLAGS_VPR_TRUE                                      0x00000001

/*
 *    NVOS04_FLAGS_CC_SECURE:
 *     This flag specifies if channel is intended to be used for
 *     encryption/decryption of data between SYSMEM <-> VIDMEM. Only CE
 *     & SEC2 Channels are capable of handling encrypted content and this
 *     flag will be ignored when CC is disabled or for chips that are not CC
 *     Capable.
 *     Reusing VPR index since VPR & CC are mutually exclusive.
 *
 *       NVOS04_FLAGS_CC_SECURE_TRUE:
 *         The channel will support CC Encryption/Decryption
 *
 *       NVOS04_FLAGS_CC_SECURE_FALSE:
 *         The channel will not support CC Encryption/Decryption
 */
#define NVOS04_FLAGS_CC_SECURE                                     2:2
#define NVOS04_FLAGS_CC_SECURE_FALSE                               0x00000000
#define NVOS04_FLAGS_CC_SECURE_TRUE                                0x00000001



/*
 *    NVOS04_FLAGS_CHANNEL_SKIP_MAP_REFCOUNTING:
 *     This flag specifies if the channel can skip refcounting of potentially
 *     accessed mappings on job kickoff.  This flag is only meaningful for
 *     kernel drivers which perform refcounting of memory mappings.
 *
 *       NVOS04_FLAGS_CHANNEL_SKIP_MAP_REFCOUNTING_FALSE:
 *         The channel cannot not skip refcounting of memory mappings
 *
 *       NVOS04_FLAGS_CHANNEL_SKIP_MAP_REFCOUNTING_TRUE:
 *         The channel can skip refcounting of memory mappings
 */
#define NVOS04_FLAGS_CHANNEL_SKIP_MAP_REFCOUNTING                  3:3
#define NVOS04_FLAGS_CHANNEL_SKIP_MAP_REFCOUNTING_FALSE            0x00000000
#define NVOS04_FLAGS_CHANNEL_SKIP_MAP_REFCOUNTING_TRUE             0x00000001

/*
 *     NVOS04_FLAGS_GROUP_CHANNEL_RUNQUEUE:
 *       This flag specifies which "runqueue" the allocated channel will be
 *       executed on in a TSG.  Channels on different runqueues within a TSG
 *       may be able to feed methods into the engine simultaneously.
 *       Non-default values are only supported on GP10x and later and only for
 *       channels within a TSG.
 */
#define NVOS04_FLAGS_GROUP_CHANNEL_RUNQUEUE                       4:4
#define NVOS04_FLAGS_GROUP_CHANNEL_RUNQUEUE_DEFAULT               0x00000000
#define NVOS04_FLAGS_GROUP_CHANNEL_RUNQUEUE_ONE                   0x00000001

/*
 *     NVOS04_FLAGS_PRIVILEGED_CHANNEL:
 *       This flag tells RM whether to give the channel admin privilege. This
 *       flag will only take effect if the client is GSP-vGPU plugin. It is
 *       needed so that guest can update page tables in physical mode and do
 *       scrubbing.
 */
#define NVOS04_FLAGS_PRIVILEGED_CHANNEL                           5:5
#define NVOS04_FLAGS_PRIVILEGED_CHANNEL_FALSE                     0x00000000
#define NVOS04_FLAGS_PRIVILEGED_CHANNEL_TRUE                      0x00000001

/*
 *     NVOS04_FLAGS_DELAY_CHANNEL_SCHEDULING:
 *       This flags tells RM not to schedule a newly created channel within a
 *       channel group immediately even if channel group is currently scheduled.
 *       Channel will not be scheduled until NVA06F_CTRL_GPFIFO_SCHEDULE is
 *       invoked. This is used eg. for CUDA which needs to do additional
 *       initialization before starting up a channel.
 *       Default is FALSE.
 */
#define NVOS04_FLAGS_DELAY_CHANNEL_SCHEDULING                     6:6
#define NVOS04_FLAGS_DELAY_CHANNEL_SCHEDULING_FALSE               0x00000000
#define NVOS04_FLAGS_DELAY_CHANNEL_SCHEDULING_TRUE                0x00000001

/*
 *     NVOS04_FLAGS_DENY_PHYSICAL_MODE_CE:
 *       This flag specifies whether or not to deny access to the physical
 *       mode of CopyEngine regardless of whether or not the client handle
 *       is admin. If set to true, this channel allocation will always result
 *       in an unprivileged channel. If set to false, the privilege of the channel
 *       will depend on the privilege level of the client handle.
 *       This is primarily meant for vGPU since all client handles
 *       granted to guests are admin.
 */
#define NVOS04_FLAGS_CHANNEL_DENY_PHYSICAL_MODE_CE                7:7
#define NVOS04_FLAGS_CHANNEL_DENY_PHYSICAL_MODE_CE_FALSE          0x00000000
#define NVOS04_FLAGS_CHANNEL_DENY_PHYSICAL_MODE_CE_TRUE           0x00000001

/*
 *     NVOS04_FLAGS_CHANNEL_USERD_INDEX_VALUE
 *
 *        This flag specifies the channel offset in terms of within a page of
 *        USERD. For example, value 3 means the 4th channel within a USERD page.
 *        Given the USERD size is 512B, we will have 8 channels total, so 3 bits
 *        are reserved.
 *
 *        When _USERD_INDEX_FIXED_TRUE is set but INDEX_PAGE_FIXED_FALSE is set,
 *        it will ask for a new USERD page.
 *
 */
#define NVOS04_FLAGS_CHANNEL_USERD_INDEX_VALUE                    10:8

#define NVOS04_FLAGS_CHANNEL_USERD_INDEX_FIXED                    11:11
#define NVOS04_FLAGS_CHANNEL_USERD_INDEX_FIXED_FALSE              0x00000000
#define NVOS04_FLAGS_CHANNEL_USERD_INDEX_FIXED_TRUE               0x00000001

/*
 *     NVOS04_FLAGS_CHANNEL_USERD_INDEX_PAGE_VALUE
 *
 *        This flag specifies the channel offset in terms of USERD page. When
 *        this PAGE_FIXED_TRUE is set, the INDEX_FIXED_FALSE bit should also
 *        be set, otherwise INVALID_STATE will be returned.
 *
 *        And the field _USERD_INDEX_VALUE will be used to request the specific
 *        offset within a USERD page.
 */

#define NVOS04_FLAGS_CHANNEL_USERD_INDEX_PAGE_VALUE               20:12

#define NVOS04_FLAGS_CHANNEL_USERD_INDEX_PAGE_FIXED               21:21
#define NVOS04_FLAGS_CHANNEL_USERD_INDEX_PAGE_FIXED_FALSE         0x00000000
#define NVOS04_FLAGS_CHANNEL_USERD_INDEX_PAGE_FIXED_TRUE          0x00000001

/*
 *     NVOS04_FLAGS_DENY_AUTH_LEVEL_PRIV
 *       This flag specifies whether or not to deny access to the privileged
 *       host methods TLB_INVALIDATE and ACCESS_COUNTER_CLR
 */
#define NVOS04_FLAGS_CHANNEL_DENY_AUTH_LEVEL_PRIV                 22:22
#define NVOS04_FLAGS_CHANNEL_DENY_AUTH_LEVEL_PRIV_FALSE           0x00000000
#define NVOS04_FLAGS_CHANNEL_DENY_AUTH_LEVEL_PRIV_TRUE            0x00000001

/*
 *    NVOS04_FLAGS_CHANNEL_SKIP_SCRUBBER
 *
 *       This flag specifies scrubbing should be skipped for any internal
 *       allocations made for this channel from PMA using ctx buf pools.
 *       Only kernel clients are allowed to use this setting.
 */
#define NVOS04_FLAGS_CHANNEL_SKIP_SCRUBBER                        23:23
#define NVOS04_FLAGS_CHANNEL_SKIP_SCRUBBER_FALSE                  0x00000000
#define NVOS04_FLAGS_CHANNEL_SKIP_SCRUBBER_TRUE                   0x00000001

/*
 *    NVOS04_FLAGS_CHANNEL_CLIENT_MAP_FIFO
 *
 *       This flag specifies that the client is expected to map USERD themselves
 *       and RM need not do so.
 */
#define NVOS04_FLAGS_CHANNEL_CLIENT_MAP_FIFO                      24:24
#define NVOS04_FLAGS_CHANNEL_CLIENT_MAP_FIFO_FALSE                0x00000000
#define NVOS04_FLAGS_CHANNEL_CLIENT_MAP_FIFO_TRUE                 0x00000001

/*
 *    NVOS04_FLAGS_SET_EVICT_LAST_CE_PREFETCH_CHANNEL
 */
#define NVOS04_FLAGS_SET_EVICT_LAST_CE_PREFETCH_CHANNEL           25:25
#define NVOS04_FLAGS_SET_EVICT_LAST_CE_PREFETCH_CHANNEL_FALSE     0x00000000
#define NVOS04_FLAGS_SET_EVICT_LAST_CE_PREFETCH_CHANNEL_TRUE      0x00000001

/*
 *    NVOS04_FLAGS_CHANNEL_VGPU_PLUGIN_CONTEXT
 *
 *       This flag specifies whether the channel calling context is from CPU
 *       VGPU plugin.
 */
#define NVOS04_FLAGS_CHANNEL_VGPU_PLUGIN_CONTEXT                  26:26
#define NVOS04_FLAGS_CHANNEL_VGPU_PLUGIN_CONTEXT_FALSE            0x00000000
#define NVOS04_FLAGS_CHANNEL_VGPU_PLUGIN_CONTEXT_TRUE             0x00000001

 /*
  *     NVOS04_FLAGS_CHANNEL_PBDMA_ACQUIRE_TIMEOUT
  *
  *        This flag specifies the channel PBDMA ACQUIRE timeout option.
  *        _FALSE to disable it, _TRUE to enable it.
  *        When this flag is enabled, if a host semaphore acquire does not
  *        complete in about 2 sec, it will time out and trigger a RC error.
  */
#define NVOS04_FLAGS_CHANNEL_PBDMA_ACQUIRE_TIMEOUT                 27:27
#define NVOS04_FLAGS_CHANNEL_PBDMA_ACQUIRE_TIMEOUT_FALSE           0x00000000
#define NVOS04_FLAGS_CHANNEL_PBDMA_ACQUIRE_TIMEOUT_TRUE            0x00000001

/*
 *     NVOS04_FLAGS_GROUP_CHANNEL_THREAD:
 *       This flags specifies the thread id in which an allocated channel
 *       will be executed in a TSG. The relationship between the thread id
 *       in A TSG and respective definitions are implementation specific.
 *       Also, not all classes will be supported at thread > 0.
 *       This field cannot be used on non-TSG channels and must be set to
 *       the default value (0) in that case. If thread > 0 on a non-TSG
 *       channel, the allocation will fail
 */
#define NVOS04_FLAGS_GROUP_CHANNEL_THREAD                          29:28
#define NVOS04_FLAGS_GROUP_CHANNEL_THREAD_DEFAULT                  0x00000000
#define NVOS04_FLAGS_GROUP_CHANNEL_THREAD_ONE                      0x00000001
#define NVOS04_FLAGS_GROUP_CHANNEL_THREAD_TWO                      0x00000002

#define NVOS04_FLAGS_MAP_CHANNEL                                   30:30
#define NVOS04_FLAGS_MAP_CHANNEL_FALSE                             0x00000000
#define NVOS04_FLAGS_MAP_CHANNEL_TRUE                              0x00000001

#define NVOS04_FLAGS_SKIP_CTXBUFFER_ALLOC                          31:31
#define NVOS04_FLAGS_SKIP_CTXBUFFER_ALLOC_FALSE                    0x00000000
#define NVOS04_FLAGS_SKIP_CTXBUFFER_ALLOC_TRUE                     0x00000001



#define CC_CHAN_ALLOC_IV_SIZE_DWORD    3U
#define CC_CHAN_ALLOC_NONCE_SIZE_DWORD 8U

#define NV_CHANNEL_ALLOC_PARAMS_MESSAGE_ID (0x906fU)

typedef struct NV_CHANNEL_ALLOC_PARAMS {

    NvHandle hObjectError; // error context DMA
    NvHandle hObjectBuffer; // no longer used
    NV_DECLARE_ALIGNED(NvU64 gpFifoOffset, 8);    // offset to beginning of GP FIFO
    NvU32    gpFifoEntries;    // number of GP FIFO entries

    NvU32    flags;


    NvHandle hContextShare; // context share handle
    NvHandle hVASpace; // VASpace for the channel

    // handle to UserD memory object for channel, ignored if hUserdMemory[0]=0
    NvHandle hUserdMemory[NV_MAX_SUBDEVICES];

    // offset to beginning of UserD within hUserdMemory[x]
    NV_DECLARE_ALIGNED(NvU64 userdOffset[NV_MAX_SUBDEVICES], 8);

    // engine type(NV2080_ENGINE_TYPE_*) with which this channel is associated
    NvU32    engineType;
    // Channel identifier that is unique for the duration of a RM session
    NvU32    cid;
    // One-hot encoded bitmask to match SET_SUBDEVICE_MASK methods
    NvU32    subDeviceId;
    NvHandle hObjectEccError; // ECC error context DMA

    NV_DECLARE_ALIGNED(NV_MEMORY_DESC_PARAMS instanceMem, 8);
    NV_DECLARE_ALIGNED(NV_MEMORY_DESC_PARAMS userdMem, 8);
    NV_DECLARE_ALIGNED(NV_MEMORY_DESC_PARAMS ramfcMem, 8);
    NV_DECLARE_ALIGNED(NV_MEMORY_DESC_PARAMS mthdbufMem, 8);

    NvHandle hPhysChannelGroup;              // reserved
    NvU32    internalFlags;                 // reserved
    NV_DECLARE_ALIGNED(NV_MEMORY_DESC_PARAMS errorNotifierMem, 8); // reserved
    NV_DECLARE_ALIGNED(NV_MEMORY_DESC_PARAMS eccErrorNotifierMem, 8); // reserved
    NvU32    ProcessID;                 // reserved
    NvU32    SubProcessID;                 // reserved

    // IV used for CPU-side encryption / GPU-side decryption.
    NvU32    encryptIv[CC_CHAN_ALLOC_IV_SIZE_DWORD];          // reserved
    // IV used for CPU-side decryption / GPU-side encryption.
    NvU32    decryptIv[CC_CHAN_ALLOC_IV_SIZE_DWORD];          // reserved
    // Nonce used CPU-side signing / GPU-side signature verification.
    NvU32    hmacNonce[CC_CHAN_ALLOC_NONCE_SIZE_DWORD];       // reserved
    NvU32    tpcConfigID; // TPC Configuration Id as supported by DTD-PG Feature
} NV_CHANNEL_ALLOC_PARAMS;

typedef NV_CHANNEL_ALLOC_PARAMS NV_CHANNELGPFIFO_ALLOCATION_PARAMETERS;

