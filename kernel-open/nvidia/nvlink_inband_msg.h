/*******************************************************************************
    Copyright (c) 2022 NVidia Corporation

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
    deal in the Software without restriction, including without limitation the
    rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
    sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

        The above copyright notice and this permission notice shall be
        included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*******************************************************************************/

#ifndef NVLINK_INBAND_MSG_HDR_H
#define NVLINK_INBAND_MSG_HDR_H

/*
 * Messages do not have individual versioning, instead a strict ABI is maintained. When a change is
 * required on existing message, instead of modifying corresponding message structure, a completely
 * new message type (like INBAND_MSG_TYPE_XXX_V1, INBAND_MSG_TYPE_XXX_V2) and corresponding message
 * definition structure needs to be added. Do not modify existing structs in any way.
 *
 * Messages may contain fields which are debug only and must be used for logging purpose. Such
 * fields shouldn't be trusted.
 *
 * - Avoid use of enums or bitfields. Always use fixed types.
 * - Avoid conditional fields in the structs.
 * - Avoid nested and complex structs. Keep them simple and flat for ease of encoding and decoding.
 * - Avoid embedded pointers. Flexible arrays at the end of the struct are allowed.
 * - Always use the packed struct to typecast inband messages. More details: 
 * - Always have reserved flags or fields to CYA given the stable ABI conditions.
 */

/* Align to byte boundaries */
#pragma pack(push, 1)

#include "nvtypes.h"
#include "nvmisc.h"
#include "nvCpuUuid.h"
#include "nvstatus.h"
#include "nvstatuscodes.h"

#define NVLINK_INBAND_MAX_MSG_SIZE     4096
#define NVLINK_INBAND_MSG_MAGIC_ID_FM  0xadbc

/* Nvlink Inband messages types */
#define NVLINK_INBAND_MSG_TYPE_GPU_PROBE_REQ         0
#define NVLINK_INBAND_MSG_TYPE_GPU_PROBE_RSP         1
#define NVLINK_INBAND_MSG_TYPE_MC_TEAM_SETUP_REQ     2
#define NVLINK_INBAND_MSG_TYPE_MC_TEAM_SETUP_RSP     3
#define NVLINK_INBAND_MSG_TYPE_MC_TEAM_RELEASE_REQ   4

/* Nvlink Inband message packet header */
typedef struct
{
    NvU16     magicId;           /* Identifier to represent in-band msg, will be NVLINK_INBAND_MSG_MAGIC_ID */
    NvU64     requestId;         /* Unique Id for a request and response will carry same id */
    NV_STATUS status;            /* High level status of the message/request */
    NvU16     type;              /* Type of encoded message. One of NVLINK_INBAND_MSG_TYPE_xxx */
    NvU32     length;            /* Length of encoded message */
    NvU8      reserved[8];       /* For future use. Must be initialized to zero */
} nvlink_inband_msg_header_t;

#define NVLINK_INBAND_GPU_PROBE_CAPS_SRIOV_ENABLED NVBIT(0)

/* Add more caps as need in the future */

typedef struct
{
    NvU32  pciInfo;              /* Encoded as Bus:Device:Function.(debug only) */
    NvU8   moduleId;             /* GPIO based physical/module ID of the GPU. (debug only) */
    NvUuid uuid;                 /* UUID of the GPU. (debug only) */
    NvU64  discoveredLinkMask;   /* GPU's discovered NVLink mask info. (debug only) */
    NvU64  enabledLinkMask;      /* GPU's currently enabled NvLink mask info. (debug only) */

    NvU32  gpuCapMask;           /* GPU capabilities, one of NVLINK_INBAND_GPU_PROBE_CAPS */
    NvU8   reserved[16];         /* For future use. Must be initialized to zero */
} nvlink_inband_gpu_probe_req_t;

typedef struct
{
    nvlink_inband_msg_header_t           msgHdr;
    nvlink_inband_gpu_probe_req_t        probeReq;
} nvlink_inband_gpu_probe_req_msg_t;

#define NVLINK_INBAND_FM_CAPS_MC_TEAM_SETUP_V1   NVBIT64(0)
#define NVLINK_INBAND_FM_CAPS_MC_TEAM_RELEASE_V1 NVBIT64(1)

typedef struct
{
    NvU64 gpuHandle;             /* Unique handle assigned by initialization entity for this GPU */
    NvU32 gfId;                  /* GFID which supports NVLink */
    NvU64 fmCaps;                /* Capability of FM e.g. what features FM support. */
    NvU16 nodeId;                /* Node ID of the system where this GPU belongs */
    NvU16 fabricPartitionId;     /* Partition ID if the GPU belongs to a fabric partition */
    NvU16 clusterId;             /* Cluster ID to which this node belongs */
    NvU64 gpaAddress;            /* GPA starting address for the GPU */
    NvU64 gpaAddressRange;       /* GPU GPA address range */
    NvU64 flaAddress;            /* FLA starting address for the GPU */
    NvU64 flaAddressRange;       /* GPU FLA address range */
    NvU8  reserved[32];          /* For future use. Must be initialized to zero */
} nvlink_inband_gpu_probe_rsp_t;

typedef struct
{
    nvlink_inband_msg_header_t           msgHdr;
    nvlink_inband_gpu_probe_rsp_t        probeRsp;
} nvlink_inband_gpu_probe_rsp_msg_t;

typedef struct
{
    NvU64 mcAllocSize;           /* Multicast allocation size requested */
    NvU32 flags;                 /* For future use. Must be initialized to zero */
    NvU8  reserved[8];           /* For future use. Must be initialized to zero */
    NvU16 numGpuHandles;         /* Number of GPUs in this team */
    NvU64 gpuHandles[];          /* Array of probed handles, should be last */
} nvlink_inband_mc_team_setup_req_t;

typedef struct
{
    nvlink_inband_msg_header_t           msgHdr;
    nvlink_inband_mc_team_setup_req_t    mcTeamSetupReq;
} nvlink_inband_mc_team_setup_req_msg_t;

typedef struct
{
    NvU64 mcTeamHandle;          /* Unique handle assigned for this Multicast team */
    NvU32 flags;                 /* For future use. Must be initialized to zero */
    NvU8  reserved[8];           /* For future use. Must be initialized to zero */
    NvU64 mcAddressBase;         /* FLA starting address assigned for the Multicast slot */
    NvU64 mcAddressSize;         /* Size of FLA assigned to the Multicast slot */
} nvlink_inband_mc_team_setup_rsp_t;

typedef struct
{
    nvlink_inband_msg_header_t           msgHdr;
    nvlink_inband_mc_team_setup_rsp_t    mcTeamSetupRsp;
} nvlink_inband_mc_team_setup_rsp_msg_t;

typedef struct
{
    NvU64 mcTeamHandle;          /* Unique handle assigned for the Multicast team */
    NvU32 flags;                 /* For future use. Must be initialized to zero */
    NvU8  reserved[8];           /* For future use. Must be initialized to zero */
} nvlink_inband_mc_team_release_req_t;

typedef struct
{
    nvlink_inband_msg_header_t           msgHdr;
    nvlink_inband_mc_team_release_req_t  mcTeamReleaseReq;
} nvlink_inband_mc_team_release_req_msg_t;

#pragma pack(pop)

/* Don't add any code after this line */

#endif
