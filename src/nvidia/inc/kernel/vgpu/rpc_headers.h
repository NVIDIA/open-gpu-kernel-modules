/*
 * SPDX-FileCopyrightText: Copyright (c) 2017-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef __vgpu_rpc_nv_headers_h__
#define __vgpu_rpc_nv_headers_h__

#include "ctrl/ctrl0080/ctrl0080perf.h"
#include "ctrl/ctrl2080/ctrl2080perf.h"
#include "ctrl/ctrl2080/ctrl2080internal.h"
#include "nvstatus.h"

#define MAX_GPC_COUNT           32

/*
 * Maximum number of RegOps that can be accommodated within one RPC call
 * due to RPC message buffer size being limited to 4k
 */
#define VGPU_MAX_REGOPS_PER_RPC 100

#define VGPU_RESERVED_HANDLE_BASE   0xCAF3F000
#define VGPU_RESERVED_HANDLE_RANGE  0x1000

#define VGPU_CALC_PARAM_OFFSET(prev_offset, prev_params)    (prev_offset + NV_ALIGN_UP(sizeof(prev_params), sizeof(NvU32)))

/*
 * Message header (in buffer addressed by ring entry)
 *
 * If message is invalid (bad length or signature), signature and length
 * are forced to be valid (if in range of descriptor) and result is set to
 * NV_VGPU_RESULT_INVALID_MESSAGE_FORMAT.  Otherwise, signature, length, and
 * function are always unchanged and result is always set.
 *
 * The function message header, if defined, immediately follows the main message
 * header.
 */
#define NV_VGPU_MSG_HEADER_VERSION_MAJOR                             31:24 /* R---D */
#define NV_VGPU_MSG_HEADER_VERSION_MINOR                             23:16 /* R---D */
#define NV_VGPU_MSG_HEADER_VERSION_MAJOR_TOT                    0x00000003 /* R---D */
#define NV_VGPU_MSG_HEADER_VERSION_MINOR_TOT                    0x00000000 /* R---D */
/* signature must equal valid value */
#define NV_VGPU_MSG_SIGNATURE_VALID                             0x43505256 /* RW--V */

#include "rpc_global_enums.h"

/* result code */
/* codes below 0xFF000000 must match exactly the NV_STATUS codes in nvos.h */
#define NV_VGPU_MSG_RESULT__RM           NV_ERR_GENERIC:0x00000000 /* RW--D */
#define NV_VGPU_MSG_RESULT_SUCCESS                      NV_OK
#define NV_VGPU_MSG_RESULT_CARD_NOT_PRESENT             NV_ERR_CARD_NOT_PRESENT
#define NV_VGPU_MSG_RESULT_DUAL_LINK_INUSE              NV_ERR_DUAL_LINK_INUSE
#define NV_VGPU_MSG_RESULT_GENERIC                      NV_ERR_GENERIC
#define NV_VGPU_MSG_RESULT_GPU_NOT_FULL_POWER           NV_ERR_GPU_NOT_FULL_POWER
#define NV_VGPU_MSG_RESULT_IN_USE                       NV_ERR_IN_USE
#define NV_VGPU_MSG_RESULT_INSUFFICIENT_RESOURCES       NV_ERR_INSUFFICIENT_RESOURCES
#define NV_VGPU_MSG_RESULT_INVALID_ACCESS_TYPE          NV_ERR_INVALID_ACCESS_TYPE
#define NV_VGPU_MSG_RESULT_INVALID_ARGUMENT             NV_ERR_INVALID_ARGUMENT
#define NV_VGPU_MSG_RESULT_INVALID_BASE                 NV_ERR_INVALID_BASE
#define NV_VGPU_MSG_RESULT_INVALID_CHANNEL              NV_ERR_INVALID_CHANNEL
#define NV_VGPU_MSG_RESULT_INVALID_CLASS                NV_ERR_INVALID_CLASS
#define NV_VGPU_MSG_RESULT_INVALID_CLIENT               NV_ERR_INVALID_CLIENT
#define NV_VGPU_MSG_RESULT_INVALID_COMMAND              NV_ERR_INVALID_COMMAND
#define NV_VGPU_MSG_RESULT_INVALID_DATA                 NV_ERR_INVALID_DATA
#define NV_VGPU_MSG_RESULT_INVALID_DEVICE               NV_ERR_INVALID_DEVICE
#define NV_VGPU_MSG_RESULT_INVALID_DMA_SPECIFIER        NV_ERR_INVALID_DMA_SPECIFIER
#define NV_VGPU_MSG_RESULT_INVALID_EVENT                NV_ERR_INVALID_EVENT
#define NV_VGPU_MSG_RESULT_INVALID_FLAGS                NV_ERR_INVALID_FLAGS
#define NV_VGPU_MSG_RESULT_INVALID_FUNCTION             NV_ERR_INVALID_FUNCTION
#define NV_VGPU_MSG_RESULT_INVALID_HEAP                 NV_ERR_INVALID_HEAP
#define NV_VGPU_MSG_RESULT_INVALID_INDEX                NV_ERR_INVALID_INDEX
#define NV_VGPU_MSG_RESULT_INVALID_LIMIT                NV_ERR_INVALID_LIMIT
#define NV_VGPU_MSG_RESULT_INVALID_METHOD               NV_ERR_INVALID_METHOD
#define NV_VGPU_MSG_RESULT_INVALID_OBJECT_BUFFER        NV_ERR_INVALID_OBJECT_BUFFER
#define NV_VGPU_MSG_RESULT_INVALID_OBJECT_ERROR         NV_ERR_INVALID_OBJECT
#define NV_VGPU_MSG_RESULT_INVALID_OBJECT_HANDLE        NV_ERR_INVALID_OBJECT_HANDLE
#define NV_VGPU_MSG_RESULT_INVALID_OBJECT_NEW           NV_ERR_INVALID_OBJECT_NEW
#define NV_VGPU_MSG_RESULT_INVALID_OBJECT_OLD           NV_ERR_INVALID_OBJECT_OLD
#define NV_VGPU_MSG_RESULT_INVALID_OBJECT_PARENT        NV_ERR_INVALID_OBJECT_PARENT
#define NV_VGPU_MSG_RESULT_INVALID_OFFSET               NV_ERR_INVALID_OFFSET
#define NV_VGPU_MSG_RESULT_INVALID_OWNER                NV_ERR_INVALID_OWNER
#define NV_VGPU_MSG_RESULT_INVALID_PARAM_STRUCT         NV_ERR_INVALID_PARAM_STRUCT
#define NV_VGPU_MSG_RESULT_INVALID_PARAMETER            NV_ERR_INVALID_PARAMETER
#define NV_VGPU_MSG_RESULT_INVALID_POINTER              NV_ERR_INVALID_POINTER
#define NV_VGPU_MSG_RESULT_INVALID_REGISTRY_KEY         NV_ERR_INVALID_REGISTRY_KEY
#define NV_VGPU_MSG_RESULT_INVALID_STATE                NV_ERR_INVALID_STATE
#define NV_VGPU_MSG_RESULT_INVALID_STRING_LENGTH        NV_ERR_INVALID_STRING_LENGTH
#define NV_VGPU_MSG_RESULT_INVALID_XLATE                NV_ERR_INVALID_XLATE
#define NV_VGPU_MSG_RESULT_IRQ_NOT_FIRING               NV_ERR_IRQ_NOT_FIRING
#define NV_VGPU_MSG_RESULT_MULTIPLE_MEMORY_TYPES        NV_ERR_MULTIPLE_MEMORY_TYPES
#define NV_VGPU_MSG_RESULT_NOT_SUPPORTED                NV_ERR_NOT_SUPPORTED
#define NV_VGPU_MSG_RESULT_OPERATING_SYSTEM             NV_ERR_OPERATING_SYSTEM
#define NV_VGPU_MSG_RESULT_PROTECTION_FAULT             NV_ERR_PROTECTION_FAULT
#define NV_VGPU_MSG_RESULT_TIMEOUT                      NV_ERR_TIMEOUT
#define NV_VGPU_MSG_RESULT_TOO_MANY_PRIMARIES           NV_ERR_TOO_MANY_PRIMARIES
#define NV_VGPU_MSG_RESULT_IRQ_EDGE_TRIGGERED           NV_ERR_IRQ_EDGE_TRIGGERED
#define NV_VGPU_MSG_RESULT_GUEST_HOST_DRIVER_MISMATCH   NV_ERR_LIB_RM_VERSION_MISMATCH

/*
 * codes above 0xFF000000 and below 0xFF100000 must match one-for-one
 * the vmiop_error_t codes in vmioplugin.h, with 0xFF000000 added.
 */
#define NV_VGPU_MSG_RESULT__VMIOP             0xFF00000a:0xFF000000 /* RW--D */
#define NV_VGPU_MSG_RESULT_VMIOP_INVAL                   0xFF000001 /* RW--V */
#define NV_VGPU_MSG_RESULT_VMIOP_RESOURCE                0xFF000002 /* RW--V */
#define NV_VGPU_MSG_RESULT_VMIOP_RANGE                   0xFF000003 /* RW--V */
#define NV_VGPU_MSG_RESULT_VMIOP_READ_ONLY               0xFF000004 /* RW--V */
#define NV_VGPU_MSG_RESULT_VMIOP_NOT_FOUND               0xFF000005 /* RW--V */
#define NV_VGPU_MSG_RESULT_VMIOP_NO_ADDRESS_SPACE        0xFF000006 /* RW--V */
#define NV_VGPU_MSG_RESULT_VMIOP_TIMEOUT                 0xFF000007 /* RW--V */
#define NV_VGPU_MSG_RESULT_VMIOP_NOT_ALLOWED_IN_CALLBACK 0xFF000008 /* RW--V */
#define NV_VGPU_MSG_RESULT_VMIOP_ECC_MISMATCH            0xFF000009 /* RW--V */
#define NV_VGPU_MSG_RESULT_VMIOP_NOT_SUPPORTED           0xFF00000a /* RW--V */
/* RPC-specific error codes */
#define NV_VGPU_MSG_RESULT__RPC               0xFF100009:0xFF100000 /* RW--D */
#define NV_VGPU_MSG_RESULT_RPC_UNKNOWN_FUNCTION          0xFF100001 /* RW--V */
#define NV_VGPU_MSG_RESULT_RPC_INVALID_MESSAGE_FORMAT    0xFF100002 /* RW--V */
#define NV_VGPU_MSG_RESULT_RPC_HANDLE_NOT_FOUND          0xFF100003 /* RW--V */
#define NV_VGPU_MSG_RESULT_RPC_HANDLE_EXISTS             0xFF100004 /* RW--V */
#define NV_VGPU_MSG_RESULT_RPC_UNKNOWN_RM_ERROR          0xFF100005 /* RW--V */
#define NV_VGPU_MSG_RESULT_RPC_UNKNOWN_VMIOP_ERROR       0xFF100006 /* RW--V */
#define NV_VGPU_MSG_RESULT_RPC_RESERVED_HANDLE           0xFF100007 /* RW--V */
#define NV_VGPU_MSG_RESULT_RPC_CUDA_PROFILING_DISABLED   0xFF100008 /* RW--V */
// This error code is used by plugin to notify the guest the that API control
// is recognized but not supported. It used by the guest to avoid printing
// error message about a failed API control.
#define NV_VGPU_MSG_RESULT_RPC_API_CONTROL_NOT_SUPPORTED 0xFF100009 /* RW--V */
/* RPC-specific code in result for incomplete request */
#define NV_VGPU_MSG_RESULT_RPC_PENDING                   0xFFFFFFFF /* RW--V */
/* shared union field */
#define NV_VGPU_MSG_UNION_INIT                           0x00000000 /* RW--V */

/*
 * common PTEDESC message defines (used w/ ALLOC_MEMORY, ALLOC_VIDMEM, FILL_PTE_MEM)
 */
#define NV_VGPU_PTEDESC_INIT                             0x00000000 /* RWI-V */
#define NV_VGPU_PTEDESC__PROD                            0x00000000 /* RW--V */
#define NV_VGPU_PTEDESC_IDR_NONE                         0x00000000 /* RW--V */
#define NV_VGPU_PTEDESC_IDR_SINGLE                       0x00000001 /* RW--V */
#define NV_VGPU_PTEDESC_IDR_DOUBLE                       0x00000002 /* RW--V */
#define NV_VGPU_PTEDESC_IDR_TRIPLE                       0x00000003 /* RW--V */

#define NV_VGPU_PTE_PAGE_SIZE                                0x1000 /* R---V */
#define NV_VGPU_PTE_SIZE                                          4 /* R---V */
#define NV_VGPU_PTE_INDEX_SHIFT                                  10 /* R---V */
#define NV_VGPU_PTE_INDEX_MASK                                0x3FF /* R---V */

#define NV_VGPU_PTE_64_PAGE_SIZE                             0x1000 /* R---V */
#define NV_VGPU_PTE_64_SIZE                                       8 /* R---V */
#define NV_VGPU_PTE_64_INDEX_SHIFT                                9 /* R---V */
#define NV_VGPU_PTE_64_INDEX_MASK                             0x1FF /* R---V */

/*
 * LOG message
 */
#define NV_VGPU_LOG_LEVEL_FATAL                          0x00000000 /* RW--V */
#define NV_VGPU_LOG_LEVEL_ERROR                          0x00000001 /* RW--V */
#define NV_VGPU_LOG_LEVEL_NOTICE                         0x00000002 /* RW--V */
#define NV_VGPU_LOG_LEVEL_STATUS                         0x00000003 /* RW--V */
#define NV_VGPU_LOG_LEVEL_DEBUG                          0x00000004 /* RW--V */

typedef enum
{
    RPC_GR_BUFFER_TYPE_GRAPHICS                 = 0,
    RPC_GR_BUFFER_TYPE_GRAPHICS_ZCULL           = 1,
    RPC_GR_BUFFER_TYPE_GRAPHICS_GRAPHICS_PM     = 2,
    RPC_GR_BUFFER_TYPE_COMPUTE_PREEMPT          = 3,
    RPC_GR_BUFFER_TYPE_GRAPHICS_PATCH           = 4,
    RPC_GR_BUFFER_TYPE_GRAPHICS_BUNDLE_CB       = 5,
    RPC_GR_BUFFER_TYPE_GRAPHICS_PAGEPOOL_GLOBAL = 6,
    RPC_GR_BUFFER_TYPE_GRAPHICS_ATTRIBUTE_CB    = 7,
    RPC_GR_BUFFER_TYPE_GRAPHICS_RTV_CB_GLOBAL   = 8,
    RPC_GR_BUFFER_TYPE_GRAPHICS_GFXP_POOL       = 9,
    RPC_GR_BUFFER_TYPE_GRAPHICS_GFXP_CTRL_BLK   = 10,
    RPC_GR_BUFFER_TYPE_GRAPHICS_FECS_EVENT      = 11,
    RPC_GR_BUFFER_TYPE_GRAPHICS_PRIV_ACCESS_MAP = 12,
    RPC_GR_BUFFER_TYPE_GRAPHICS_MAX             = 13,
} RPC_GR_BUFFER_TYPE;

typedef enum
{
    FECS_ERROR_EVENT_TYPE_NONE                  = 0,
    FECS_ERROR_EVENT_TYPE_BUFFER_RESET_REQUIRED = 1,
    FECS_ERROR_EVENT_TYPE_BUFFER_FULL           = 2,
    FECS_ERROR_EVENT_TYPE_MAX                   = 3,
} FECS_ERROR_EVENT_TYPE;

/*
 * Maximum entries that can be sent in a single pass of RPC.
 */
#define VGPU_RPC_GET_P2P_CAPS_V2_MAX_GPUS_SQUARED_PER_RPC               512

/* Fetching NV2080_CTRL_GR_MAX_CTX_BUFFER_COUNT in single RPC mesaage
 * causes RPC buffer to overflow. To accommodate, we will have to convert
 * current RPC to multipass. But currently, RM allocates only
 * (3 + GR_GLOBALCTX_BUFFER_COUNT) < 32 buffers and they accommodate in single
 * RPC message size. Hence, not converting current RPC to multipass.
 * and limiting the max buffer count per RPC to 32.
 */
#define GR_MAX_RPC_CTX_BUFFER_COUNT   32

/*
 * Enums specifying the BAR number that we are going to update its PDE
 */
typedef enum
{
    NV_RPC_UPDATE_PDE_BAR_1,
    NV_RPC_UPDATE_PDE_BAR_2,
    NV_RPC_UPDATE_PDE_BAR_INVALID,
} NV_RPC_UPDATE_PDE_BAR_TYPE;

typedef struct VIRTUAL_DISPLAY_GET_MAX_RESOLUTION_PARAMS 
{
    NvU32 headIndex;
    NvU32 maxHResolution;
    NvU32 maxVResolution;
} VIRTUAL_DISPLAY_GET_MAX_RESOLUTION_PARAMS;

typedef struct VIRTUAL_DISPLAY_GET_NUM_HEADS_PARAMS 
{
    NvU32 numHeads;
    NvU32 maxNumHeads;
} VIRTUAL_DISPLAY_GET_NUM_HEADS_PARAMS;


/*
 *  Maximum number of SMs that can be read in one RPC call to get error states
 */

#define VGPU_RPC_CTRL_DEBUG_READ_ALL_SM_ERROR_STATES_PER_RPC_v21_06 80

typedef enum
{
    GPU_RECOVERY_EVENT_TYPE_REFRESH,
    GPU_RECOVERY_EVENT_TYPE_GPU_DRAIN_P2P,
    GPU_RECOVERY_EVENT_TYPE_SYS_REBOOT,
    GPU_RECOVERY_EVENT_TYPE_GPU_REBOOT,
} GPU_RECOVERY_EVENT_TYPE;


#endif // __vgpu_rpc_nv_headers_h__
