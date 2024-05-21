/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef __vgpu_dev_nv_vgpu_h__
#define __vgpu_dev_nv_vgpu_h__

#include "nvtypes.h"
#include "nvmisc.h"

#include "nvctassert.h"

/******************************************************************************/
/* EMULATED REGISTERS - START                                                 */
/******************************************************************************/

#define NV_VGPU_GUEST_OS_TYPE_ARCH                              2:0 /* -W-4F */
#define NV_VGPU_GUEST_OS_TYPE_ARCH_UNKNOWN                        0 /* -W--V */
#define NV_VGPU_GUEST_OS_TYPE_ARCH_AARCH64                        1 /* -W--V */
#define NV_VGPU_GUEST_OS_TYPE_ARCH_X86_64                         2 /* -W--V */
#define NV_VGPU_GUEST_OS_TYPE_ARCH_INVALID                        3 /* -W--V */
#define NV_VGPU_GUEST_OS_TYPE_OS                                7:3 /* -W-4F */
#define NV_VGPU_GUEST_OS_TYPE_OS_UNKNOWN                          0 /* -W--V */
#define NV_VGPU_GUEST_OS_TYPE_OS_LINUX                            1 /* -W--V */
#define NV_VGPU_GUEST_OS_TYPE_OS_WINDOWS7                         2 /* -W--V */
#define NV_VGPU_GUEST_OS_TYPE_OS_WINDOWS10                        3 /* -W--V */
#define NV_VGPU_GUEST_OS_TYPE_OS_INVALID                          4 /* -W--V */
// All remaining values for NV_VGPU_GUEST_OS_TYPE_OS are reserved/not supported.
#define NV_VGPU_GUEST_OS_TYPE_PAGE_SIZE                        15:8 /* -W-4F */

#define NV_VGPU_SHARED_MEMORY__SIZE_1                             4 /*       */
#define NV_VGPU_SHARED_MEMORY_TARGET                            1:0 /* RWIVF */
#define NV_VGPU_SHARED_MEMORY_TARGET_PHYS_NVM            0x00000001 /* RW--V */
#define NV_VGPU_SHARED_MEMORY_TARGET_PHYS_PCI_COHERENT   0x00000003 /* RW--V */
#define NV_VGPU_SHARED_MEMORY_STATUS                            3:3 /* RWIVF */
#define NV_VGPU_SHARED_MEMORY_STATUS_INVALID             0x00000000 /* RW--V */
#define NV_VGPU_SHARED_MEMORY_STATUS_VALID               0x00000001 /* RW--V */
#define NV_VGPU_SHARED_MEMORY_SIZE                              5:4 /* RWIVF */
#define NV_VGPU_SHARED_MEMORY_SIZE_4KB                   0x00000000 /* RW--V */
#define NV_VGPU_SHARED_MEMORY_ADDR_LO                         31:12 /* RWIVF */

#define NV_VGPU_SHARED_MEMORY_HI_ADDR                          19:0 /* RWIVF */

/******************************************************************************/
/* EMULATED REGISTERS - END                                                   */
/******************************************************************************/

/******************************************************************************/
/* SHARED MEMORY - START                                                      */
/******************************************************************************/

/* vGPU Current Pstate */
#define NV_VGPU_SHARED_MEMORY_POINTER_CURRENT_PSTATE     0x00000090
#define NV_VGPU_SHARED_MEMORY_POINTER_CURRENT_PSTATE_VALUE     31:0

/* vGPU ECC errors */
#define NV_VGPU_SHARED_MEMORY_POINTER_ECC_TYPE           0x00000094
#define NV_VGPU_SHARED_MEMORY_POINTER_ECC_TYPE_VALUE           31:0
#define NV_VGPU_SHARED_MEMORY_POINTER_ECC_UNIT           0x00000098
#define NV_VGPU_SHARED_MEMORY_POINTER_ECC_UNIT_VALUE           31:0
#define NV_VGPU_SHARED_MEMORY_POINTER_ECC_ERROR_COUNT    0x0000009c
#define NV_VGPU_SHARED_MEMORY_POINTER_ECC_ERROR_COUNT_VALUE    31:0

/* vGPU backdoor VNC state */
#define NV_VGPU_SHARED_MEMORY_POINTER_VNC                0x000000a0
#define NV_VGPU_SHARED_MEMORY_POINTER_VNC_STATE                31:0
#define NV_VGPU_SHARED_MEMORY_POINTER_VNC_STATE_ENABLED  0x00000001
#define NV_VGPU_SHARED_MEMORY_POINTER_VNC_STATE_DISABLED 0x00000000

/* vGPU backdoor VNC support */
#define NV_VGPU_SHARED_MEMORY_POINTER_VNC_SUPPORT        0x000000a4
#define NV_VGPU_SHARED_MEMORY_POINTER_VNC_SUPPORT_VALUE        31:0
#define NV_VGPU_SHARED_MEMORY_POINTER_VNC_SUPPORT_ENABLED    0x0001
#define NV_VGPU_SHARED_MEMORY_POINTER_VNC_SUPPORT_DISABLED   0x0000

/* ecc fatal poison error*/
#define NV_VGPU_SHARED_MEMORY_POINTER_ECC_POISON_ERROR   0x000000a8
#define NV_VGPU_SHARED_MEMORY_POINTER_ECC_POISON_ERROR_VALUE   31:0

/* NvEnc Stats Reporting State */
#define NV_VGPU_SHARED_MEMORY_POINTER_NVENC_STATS_REPORTING_STATE               0x000000ac
#define NV_VGPU_SHARED_MEMORY_POINTER_NVENC_STATS_REPORTING_STATE_VALUE               31:0
#define NV_VGPU_SHARED_MEMORY_POINTER_NVENC_STATS_REPORTING_STATE_DISABLED      0x00000000
#define NV_VGPU_SHARED_MEMORY_POINTER_NVENC_STATS_REPORTING_STATE_ENABLED       0x00000001
#define NV_VGPU_SHARED_MEMORY_POINTER_NVENC_STATS_REPORTING_STATE_NOT_SUPPORTED 0x00000002

/* Nvlink inband message response available*/
#define NV_VGPU_SHARED_MEMORY_POINTER_NVLINK_INBAND_RESPONSE                  0x000000b0
#define NV_VGPU_SHARED_MEMORY_POINTER_NVLINK_INBAND_RESPONSE_NONE             0x00000000
#define NV_VGPU_SHARED_MEMORY_POINTER_NVLINK_INBAND_RESPONSE_PROBE                   0:0
#define NV_VGPU_SHARED_MEMORY_POINTER_NVLINK_INBAND_RESPONSE_PROBE_CLEAR      0x00000000
#define NV_VGPU_SHARED_MEMORY_POINTER_NVLINK_INBAND_RESPONSE_PROBE_PENDING    0x00000001
#define NV_VGPU_SHARED_MEMORY_POINTER_NVLINK_INBAND_RESPONSE_MC_SETUP                1:1
#define NV_VGPU_SHARED_MEMORY_POINTER_NVLINK_INBAND_RESPONSE_MC_SETUP_CLEAR   0x00000000
#define NV_VGPU_SHARED_MEMORY_POINTER_NVLINK_INBAND_RESPONSE_MC_SETUP_PENDING 0x00000001

/******************************************************************************/
/* SHARED MEMORY - END                                                        */
/******************************************************************************/

/******************************************************************************/
/* EVENT MEMORY - START                                                       */
/******************************************************************************/

/* Event ring entry (9 words) */
#define NV_VGPU_EV__SIZE_1                               0x0000000a /*       */
#define NV_VGPU_EV_HCLIENT                               0x00000000 /* RW-4R */
#define NV_VGPU_EV_HOBJECT                               0x00000004 /* RW-4R */
#define NV_VGPU_EV_NOTIFY_INDEX                          0x00000008 /* RW-4R */
#define NV_VGPU_EV_FLAGS                                 0x0000000c /* RW-4R */
#define NV_VGPU_EV_FLAGS_ALLOCATED                              3:0 /* RW-4R */
#define NV_VGPU_EV_FLAGS_ALLOCATED_GUEST                 0x00000000 /* RW--V */
#define NV_VGPU_EV_FLAGS_ALLOCATED_PLUGIN                0x00000001 /* RW--V */
#define NV_VGPU_EV_FLAGS_HAS_NOTIFIER_DATA                      4:4 /* RW-4R */
#define NV_VGPU_EV_FLAGS_HAS_NOTIFIER_DATA_FALSE         0x00000000 /* RW--V */
#define NV_VGPU_EV_FLAGS_HAS_NOTIFIER_DATA_TRUE          0x00000001 /* RW--V */
#define NV_VGPU_EV_FLAGS_TYPE                                 31:16 /* RW-4R */
#define NV_VGPU_EV_FLAGS_TYPE_ROBUST_CHANNEL_ERROR       0x00000000 /* RW--V */
#define NV_VGPU_EV_FLAGS_TYPE_EVENT_INTR_MTHD            0x00000001 /* RW--V */
#define NV_VGPU_EV_FLAGS_TYPE_VBLANK_INTR                0x00000002 /* RW--V */
#define NV_VGPU_EV_FLAGS_TYPE_VNC                        0x00000003 /* RW--V */
#define NV_VGPU_EV_FLAGS_TYPE_PSTATE                     0x00000004 /* RW--V */
#define NV_VGPU_EV_FLAGS_TYPE_ECC                        0x00000005 /* RW--V */
#define NV_VGPU_EV_FLAGS_TYPE_NVENC_REPORTING_STATE      0x00000006 /* RW--V */
#define NV_VGPU_EV_FLAGS_TYPE_INBAND_RESPONSE            0x00000007 /* RW--V */
#define NV_VGPU_EV_NOTIFIER_TIMESTAMP                    0x00000010 /* RW-4R */
#define NV_VGPU_EV_NOTIFIER_TIMESTAMP_HI                 0x00000014 /* RW-4R */
#define NV_VGPU_EV_NOTIFIER_INFO32                       0x00000018 /* RW-4R */
#define NV_VGPU_EV_NOTIFIER_INFO16                       0x0000001c /* RW-4R */
#define NV_VGPU_EV_NOTIFIER_INFO16_VALUE                       15:0 /* RW-4R */
#define NV_VGPU_EV_NOTIFIER_STATUS                       0x00000020 /* RW-4R */
#define NV_VGPU_EV_ROBUST_CHANNEL_ERROR_CHID             0x00000024 /* RW-4R */

typedef struct {
    volatile NvU32 hClient;
    volatile NvU32 hObject;
    volatile NvU32 notifyIndex;
    volatile NvU32 flags;
    volatile NvU32 timestampLo;
    volatile NvU32 timestampHi;
    volatile NvU32 info32;
    volatile NvU32 info16;
    volatile NvU32 status;
    volatile NvU32 rcChid;
} VGPU_EVENT_BUF_ENTRY;

#define VGPU_EVENT_BUF_ENTRY_SIZE   (sizeof(VGPU_EVENT_BUF_ENTRY))
#define VGPU_EVENT_BUF_ENTRY_COUNT  (RM_PAGE_SIZE / VGPU_EVENT_BUF_ENTRY_SIZE)

ct_assert(NV_OFFSETOF(VGPU_EVENT_BUF_ENTRY, hClient    ) == NV_VGPU_EV_HCLIENT);
ct_assert(NV_OFFSETOF(VGPU_EVENT_BUF_ENTRY, hObject    ) == NV_VGPU_EV_HOBJECT);
ct_assert(NV_OFFSETOF(VGPU_EVENT_BUF_ENTRY, notifyIndex) == NV_VGPU_EV_NOTIFY_INDEX);
ct_assert(NV_OFFSETOF(VGPU_EVENT_BUF_ENTRY, flags      ) == NV_VGPU_EV_FLAGS);
ct_assert(NV_OFFSETOF(VGPU_EVENT_BUF_ENTRY, timestampLo) == NV_VGPU_EV_NOTIFIER_TIMESTAMP);
ct_assert(NV_OFFSETOF(VGPU_EVENT_BUF_ENTRY, timestampHi) == NV_VGPU_EV_NOTIFIER_TIMESTAMP_HI);
ct_assert(NV_OFFSETOF(VGPU_EVENT_BUF_ENTRY, info32     ) == NV_VGPU_EV_NOTIFIER_INFO32);
ct_assert(NV_OFFSETOF(VGPU_EVENT_BUF_ENTRY, info16     ) == NV_VGPU_EV_NOTIFIER_INFO16);
ct_assert(NV_OFFSETOF(VGPU_EVENT_BUF_ENTRY, status     ) == NV_VGPU_EV_NOTIFIER_STATUS);
ct_assert(NV_OFFSETOF(VGPU_EVENT_BUF_ENTRY, rcChid     ) == NV_VGPU_EV_ROBUST_CHANNEL_ERROR_CHID);
ct_assert(VGPU_EVENT_BUF_ENTRY_SIZE == (NV_VGPU_EV__SIZE_1 * sizeof (NvU32)));

/******************************************************************************/
/* EVENT MEMORY - END                                                         */
/******************************************************************************/

/* virtual GPU */
#ifndef NV_XVE_ID_DEVICE_CHIP_VGPU
#define NV_XVE_ID_DEVICE_CHIP_VGPU                       0x00000f00 /* R---V */
#endif

/******************************************************************************/
/* GSP Control buffer shared between Guest RM and GSP Plugin - START          */
/******************************************************************************/
/******************************************************************************/
/* GSP Control buffer format - Version 1 - START                              */
/******************************************************************************/

#define VGPU_GSP_BUF_ADDR_V1_VALIDITY                                        0:0
#define VGPU_GSP_BUF_ADDR_V1_VALIDITY_INVALID                         0x00000000
#define VGPU_GSP_BUF_ADDR_V1_VALIDITY_VALID                           0x00000001
#define VGPU_GSP_BUF_ADDR_V1_APERTURE                                        2:1
#define VGPU_GSP_BUF_ADDR_V1_APERTURE_INVALID                         0x00000000
#define VGPU_GSP_BUF_ADDR_V1_APERTURE_SYSMEM                          0x00000001
#define VGPU_GSP_BUF_ADDR_V1_APERTURE_FBMEM                           0x00000002
#define VGPU_GSP_BUF_ADDR_V2_SIZE                                            4:3
#define VGPU_GSP_BUF_ADDR_V2_SIZE_4K                                  0x00000000
#define VGPU_GSP_BUF_ADDR_V2_SIZE_128K                                0x00000001
#define VGPU_GSP_BUF_ADDR_V1_PFN                                           63:12

#define VGPU_GSP_CTRL_BUF_V1_VERSION                                           1
#define VGPU_GSP_CTRL_BUF_V2_VERSION                                           2

/****** Control buffer: written by guest RM and read by GSP vGPU plugin *******/

#define VGPU_GSP_CTRL_BUF_SIZE_V1                                           4096

typedef struct {
    NvU64   addr;
    NvU64   bar2Offset;
} VGPU_GSP_BUF_INFO;

typedef union {
    struct {
        volatile NvU32             version;              // Version of control buffer format
        volatile NvU32             requestId;            // Request sequence number
        volatile VGPU_GSP_BUF_INFO responseBuf;          // Response buffer address
        volatile VGPU_GSP_BUF_INFO msgBuf;               // RPC message buffer address
        volatile VGPU_GSP_BUF_INFO sharedMem;            // Shared memory buffer
        volatile VGPU_GSP_BUF_INFO eventBuf;             // Event buffer address
        volatile NvU32             getEventBuf;          // GET index in circular event buffe
        volatile NvU32             guestEccStatus;       // guest ecc status
        volatile NvU64             sysmemBitMapTablePfn; // Root node's pfn value of dirty sysmem tracking table
        volatile NvU32             guestOsType;          // Guest OS type
    } ;
    volatile NvU8 buf[VGPU_GSP_CTRL_BUF_SIZE_V1];
} VGPU_GSP_CTRL_BUF_V1;

// check size
ct_assert(sizeof(VGPU_GSP_CTRL_BUF_V1) == VGPU_GSP_CTRL_BUF_SIZE_V1);

// check field offset
ct_assert(NV_OFFSETOF(VGPU_GSP_CTRL_BUF_V1, version              ) == 0x000);
ct_assert(NV_OFFSETOF(VGPU_GSP_CTRL_BUF_V1, requestId            ) == 0x004);
ct_assert(NV_OFFSETOF(VGPU_GSP_CTRL_BUF_V1, responseBuf          ) == 0x008);
ct_assert(NV_OFFSETOF(VGPU_GSP_CTRL_BUF_V1, msgBuf               ) == 0x018);
ct_assert(NV_OFFSETOF(VGPU_GSP_CTRL_BUF_V1, sharedMem            ) == 0x028);
ct_assert(NV_OFFSETOF(VGPU_GSP_CTRL_BUF_V1, eventBuf             ) == 0x038);
ct_assert(NV_OFFSETOF(VGPU_GSP_CTRL_BUF_V1, getEventBuf          ) == 0x048);
ct_assert(NV_OFFSETOF(VGPU_GSP_CTRL_BUF_V1, guestEccStatus       ) == 0x04C);
ct_assert(NV_OFFSETOF(VGPU_GSP_CTRL_BUF_V1, sysmemBitMapTablePfn ) == 0x050);
ct_assert(NV_OFFSETOF(VGPU_GSP_CTRL_BUF_V1, guestOsType          ) == 0x058);

/****** Response buffer: written by GSP vGPU plugin and read by guest RM ******/

#define VGPU_GSP_RESPONSE_BUF_SIZE_V1                                       4096

typedef union {
    struct {
        volatile NvU32 responseId;          // Response sequence number
        volatile NvU32 putEventBuf;         // PUT index in circular event buffer
        volatile NvU32 hostEccStatus;       // host ecc status
        volatile NvU32 usmType;             // Host USM Type
    };
    volatile NvU8 buf[VGPU_GSP_RESPONSE_BUF_SIZE_V1];
} VGPU_GSP_RESPONSE_BUF_V1;

// check size
ct_assert(sizeof(VGPU_GSP_RESPONSE_BUF_V1) == VGPU_GSP_RESPONSE_BUF_SIZE_V1);

// check field offset
ct_assert(NV_OFFSETOF(VGPU_GSP_RESPONSE_BUF_V1, responseId          ) == 0x000);
ct_assert(NV_OFFSETOF(VGPU_GSP_RESPONSE_BUF_V1, putEventBuf         ) == 0x004);
ct_assert(NV_OFFSETOF(VGPU_GSP_RESPONSE_BUF_V1, hostEccStatus       ) == 0x008);
ct_assert(NV_OFFSETOF(VGPU_GSP_RESPONSE_BUF_V1, usmType             ) == 0x00C);

/******************************************************************************/
/* GSP Control buffer format - Version 1 - END                                */
/******************************************************************************/

// Control buffer format for Guest RM
typedef union {
    volatile       VGPU_GSP_CTRL_BUF_V1     v1;
} VGPU_GSP_CTRL_BUF_RM;

// Control buffer format for GSP vGPU Plugin
typedef union {
    volatile const VGPU_GSP_CTRL_BUF_V1     v1;
} VGPU_GSP_CTRL_BUF_PLUGIN;

// Response buffer format for Guest RM
typedef union {
    volatile const VGPU_GSP_RESPONSE_BUF_V1 v1;
} VGPU_GSP_RESPONSE_BUF_RM;

// Response buffer format for GSP vGPU Plugin
typedef union {
    volatile       VGPU_GSP_RESPONSE_BUF_V1 v1;
} VGPU_GSP_RESPONSE_BUF_PLUGIN;

/******************************************************************************/
/* GSP Control buffer shared between Guest RM and GSP Plugin - END            */
/******************************************************************************/

// VGPU GSP dirty sysmem tracking pfn format
#define VGPU_GSP_SYSMEM_PFN_BITMAP_BUF_ADDR_VALIDITY                         0:0
#define VGPU_GSP_SYSMEM_PFN_BITMAP_BUF_ADDR_VALIDITY_INVALID          0x00000000
#define VGPU_GSP_SYSMEM_PFN_BITMAP_BUF_ADDR_VALIDITY_VALID            0x00000001
#define VGPU_GSP_SYSMEM_PFN_BITMAP_BUF_ADDR_APERTURE                         2:1
#define VGPU_GSP_SYSMEM_PFN_BITMAP_BUF_ADDR_APERTURE_INVALID          0x00000000
#define VGPU_GSP_SYSMEM_PFN_BITMAP_BUF_ADDR_APERTURE_SYSMEM           0x00000001
#define VGPU_GSP_SYSMEM_PFN_BITMAP_BUF_ADDR_APERTURE_FBMEM            0x00000002
#define VGPU_GSP_SYSMEM_PFN_BITMAP_BUF_ADDR_PFN                            63:12

#define MAX_PFNS_PER_4K_PAGE                512

// VGPU GSP dirty sysmem tracking root node format
typedef struct {
    NvU16   nodeCount;                          // count of allocated bitmap nodes
    NvU16   padding1;
    NvU32   padding2;
    NvU64   nodePfns[MAX_PFNS_PER_4K_PAGE - 1];
} VGPU_GSP_SYSMEM_BITMAP_ROOT_NODE;

ct_assert(sizeof(VGPU_GSP_SYSMEM_BITMAP_ROOT_NODE) == 0x1000);
#endif // __vgpu_dev_nv_vgpu_h__
