/*
 * SPDX-FileCopyrightText: Copyright (c) 2022-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#define NVLINK_INBAND_MAX_MSG_SIZE     5120
#define NVLINK_INBAND_MSG_MAGIC_ID_FM  0xadbc

/* Nvlink Inband messages types */
#define NVLINK_INBAND_MSG_TYPE_GPU_PROBE_REQ             0
#define NVLINK_INBAND_MSG_TYPE_GPU_PROBE_RSP             1
#define NVLINK_INBAND_MSG_TYPE_MC_TEAM_SETUP_REQ         2
#define NVLINK_INBAND_MSG_TYPE_MC_TEAM_SETUP_RSP         3
#define NVLINK_INBAND_MSG_TYPE_MC_TEAM_RELEASE_REQ       4
#define NVLINK_INBAND_MSG_TYPE_MC_TEAM_SETUP_REQ_V2      5
#define NVLINK_INBAND_MSG_TYPE_GPU_PROBE_UPDATE_REQ      6
#define NVLINK_INBAND_MSG_TYPE_GPU_PROBE_REPLAY_REQ      7
#define NVLINK_INBAND_MSG_TYPE_GPU_PROBE_REPLAY_RSP      NVLINK_INBAND_MSG_TYPE_GPU_PROBE_RSP
#define NVLINK_INBAND_MSG_TYPE_MC_TEAM_SETUP_REPLAY_REQ  8
#define NVLINK_INBAND_MSG_TYPE_MC_TEAM_SETUP_REPLAY_RSP  NVLINK_INBAND_MSG_TYPE_MC_TEAM_SETUP_RSP
#define NVLINK_INBAND_MSG_TYPE_MAX                       9

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
#define NVLINK_INBAND_GPU_PROBE_CAPS_PROBE_UPDATE  NVBIT(1)
#define NVLINK_INBAND_GPU_PROBE_CAPS_EGM_SUPPORT   NVBIT(2)
#define NVLINK_INBAND_GPU_PROBE_CAPS_ATS_SUPPORT   NVBIT(3)

/* Add more caps as need in the future */

#define NVLINK_INBAND_BW_MODE_FULL     0
#define NVLINK_INBAND_BW_MODE_OFF      1
#define NVLINK_INBAND_BW_MODE_MIN      2
#define NVLINK_INBAND_BW_MODE_HALF     3
#define NVLINK_INBAND_BW_MODE_3QUARTER 4

typedef struct
{
    NvU64  pciInfo;              /* Encoded as Domain(63:32):Bus(15:8):Device(0:7). (debug only) */
    NvU8   moduleId;             /* GPIO based physical/module ID of the GPU. (debug only) */
    NvUuid gpuUuid;              /* UUID of the GPU. (debug only) */
    NvU64  discoveredLinkMask;   /* GPU's discovered NVLink mask info. (debug only) */
    NvU64  enabledLinkMask;      /* GPU's currently enabled NvLink mask info. (debug only) */

    NvU32  gpuCapMask;           /* GPU capabilities, one of NVLINK_INBAND_GPU_PROBE_CAPS */
    NvU8   bwMode;               /* NVLink bandwidth mode, one of NVLINK_INBAND_BW_MODE */
    NvU8   reserved[31];         /* For future use. Must be initialized to zero */
} nvlink_inband_gpu_probe_req_t;

typedef struct
{
    nvlink_inband_msg_header_t           msgHdr;
    nvlink_inband_gpu_probe_req_t        probeReq;
} nvlink_inband_gpu_probe_req_msg_t;

#define NVLINK_INBAND_FM_CAPS_MC_TEAM_SETUP_V1   NVBIT64(0)
#define NVLINK_INBAND_FM_CAPS_MC_TEAM_RELEASE_V1 NVBIT64(1)
#define NVLINK_INBAND_FM_CAPS_BW_MODE_MIN        NVBIT64(2)
#define NVLINK_INBAND_FM_CAPS_BW_MODE_HALF       NVBIT64(3)
#define NVLINK_INBAND_FM_CAPS_BW_MODE_3QUARTER   NVBIT64(4)
#define NVLINK_INBAND_FM_CAPS_MC_TEAM_SETUP_V2   NVBIT64(5)
#define NVLINK_INBAND_FM_CAPS_EGM_ENABLED        NVBIT64(6)
#define NVLINK_INBAND_FM_CAPS_ATS_ENABLED        NVBIT64(7)

#define NVLINK_INBAND_FABRIC_HEALTH_MASK_DEGRADED_BW 1:0
#define NVLINK_INBAND_FABRIC_HEALTH_MASK_DEGRADED_BW_NOT_SUPPORTED 0
#define NVLINK_INBAND_FABRIC_HEALTH_MASK_DEGRADED_BW_TRUE          1
#define NVLINK_INBAND_FABRIC_HEALTH_MASK_DEGRADED_BW_FALSE         2

typedef struct
{
    NvU64  gpuHandle;             /* Unique handle assigned by initialization entity for this GPU */
    NvU32  gfId;                  /* GFID which supports NVLink */
    NvU64  fmCaps;                /* Capability of FM e.g. what features FM support. */
    NvUuid clusterUuid;           /* Cluster UUID to which this node belongs */
    NvU16  fabricPartitionId;     /* Partition ID if the GPU belongs to a fabric partition */
    NvU64  gpaAddress;            /* GPA starting address for the GPU */
    NvU64  gpaAddressRange;       /* GPU GPA address range */
    NvU64  flaAddress;            /* FLA starting address for the GPU */
    NvU64  flaAddressRange;       /* GPU FLA address range */
    NvU32  linkMaskToBeReduced;   /* bit mask of unused NVLink ports for P2P */
    NvU32  cliqueId;              /* Fabric Clique Id */
    NvU32  fabricHealthMask;      /* Mask containing bits indicating various fabric health parameters */
    NvU32  gpaAddressEGMHi;       /* GPA Address for EGM. Don't use if EGM support is not present in GFM */
    NvU8   reserved[16];          /* For future use. Must be initialized to zero */
} nvlink_inband_gpu_probe_rsp_t;

typedef struct
{
    nvlink_inband_msg_header_t           msgHdr;
    nvlink_inband_gpu_probe_rsp_t        probeRsp;
} nvlink_inband_gpu_probe_rsp_msg_t;

typedef struct
{
    NvU64  gpuHandle;             /* Unique handle assigned by initialization entity for this GPU */
    NvU32  cliqueId;              /* Fabric Clique Id*/
    NvU32  fabricHealthMask;      /* Mask containing bits indicating various fabric health parameters */
    NvU8   reserved[32];          /* For future use. Must be initialized to zero */
} nvlink_inband_gpu_probe_update_req_t;

typedef struct
{
    nvlink_inband_msg_header_t               msgHdr;
    nvlink_inband_gpu_probe_update_req_t     probeUpdate;
} nvlink_inband_gpu_probe_update_req_msg_t;

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
    NvU64 mcAllocSize;           /* Multicast allocation size requested */
    NvU32 flags;                 /* For future use. Must be initialized to zero */
    NvU8  reserved[8];           /* For future use. Must be initialized to zero */
    NvU16 numGpuHandles;         /* Number of GPUs in this team */
    NvU16 numKeys;               /* Number of keys (a.k.a request ID) used by FM to send response */
    NvU64 gpuHandlesAndKeys[];   /* Array of probed handles and keys, should be last */

    /*
     * The array will be grouped and ordered as: <allGpuHandlesOfNodeA, allGpuHandlesOfNodeB,...
     * keyForNodeA, keyForNodeB>. The first group of gpuHandles will belong to the exporter node,
     * which will be followed by the importer nodes.
     *
     * Test case: If the exporter and importer nodes are same, then the message will
     * have multiple keys belonging to the same node as: <allGpuHandlesOfNodeA,...
     * key1ForNodeA, key2ForNodeA>. Even though all gpuHandles belong to the same node, the
     * first key should be considered from the exporter node and the rest from the importer
     * nodes.
     */
} nvlink_inband_mc_team_setup_req_v2_t;

typedef struct
{
    nvlink_inband_msg_header_t           msgHdr;
    nvlink_inband_mc_team_setup_req_v2_t mcTeamSetupReq;
} nvlink_inband_mc_team_setup_req_v2_msg_t;

typedef struct
{
    NvU64 mcTeamHandle;          /* Unique handle assigned for this Multicast team */
    NvU32 flags;                 /* For future use. Must be initialized to zero */
    NvU8  reserved[8];           /* For future use. Must be initialized to zero */
    NvU64 mcAddressBase;         /* FLA starting address assigned for the Multicast slot */
    NvU64 mcAddressSize;         /* Should be same as mcAllocSize */
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

typedef struct
{
    /* Fields to be replayed */
    NvU64  gpuHandle;            /* Unique handle that was provided by FM pre-migration. */

    /* Other fields from the request */
    NvU64  pciInfo;              /* Encoded as Domain(63:32):Bus(15:8):Device(0:7). (debug only) */
    NvU8   moduleId;             /* GPIO based physical/module ID of the GPU. (debug only) */
    NvUuid gpuUuid;              /* UUID of the GPU. (debug only) */
    NvU64  discoveredLinkMask;   /* GPU's discovered NVLink mask info. (debug only) */
    NvU64  enabledLinkMask;      /* GPU's currently enabled NvLink mask info. (debug only) */

    NvU32  gpuCapMask;           /* GPU capabilities, one of NVLINK_INBAND_GPU_PROBE_CAPS */
    NvU8   bwMode;               /* NVLink bandwidth mode, one of NVLINK_INBAND_BW_MODE */
    NvU8   reserved[31];         /* For future use. Must be initialized to zero */
} nvlink_inband_gpu_probe_replay_req_t;

typedef struct
{
    nvlink_inband_msg_header_t           msgHdr;
    nvlink_inband_gpu_probe_replay_req_t probeReplayReq;
} nvlink_inband_gpu_probe_replay_req_msg_t;

typedef nvlink_inband_gpu_probe_rsp_t nvlink_inband_gpu_probe_replay_rsp_t;
typedef nvlink_inband_gpu_probe_rsp_msg_t nvlink_inband_gpu_probe_replay_rsp_msg_t;

typedef struct
{
    /* Fields to be replayed */
    NvU64 mcTeamHandle;          /* Unique handle assigned for this Multicast team */
    NvU64 mcAddressBase;         /* FLA starting address assigned for the Multicast slot */
    NvU64 mcAddressSize;         /* Size of FLA assigned to the Multicast slot */

    /* Other fields from the request */
    NvU64 mcAllocSize;           /* Multicast allocation size requested */
    NvU32 flags;                 /* For future use. Must be initialized to zero */
    NvU8  reserved[8];           /* For future use. Must be initialized to zero */
    NvU16 numGpuHandles;         /* Number of GPUs in this team */
    NvU16 numKeys;               /* Number of keys (a.k.a request ID) used by FM to send response */
    NvU64 gpuHandlesAndKeys[];   /* Array of probed handles and keys, should be last */
} nvlink_inband_mc_team_setup_replay_req_t;


typedef struct
{
    nvlink_inband_msg_header_t               msgHdr;
    nvlink_inband_mc_team_setup_replay_req_t mcTeamSetupReplayReq;
} nvlink_inband_mc_team_setup_replay_req_msg_t;

typedef nvlink_inband_mc_team_setup_rsp_t nvlink_inband_mc_team_setup_replay_rsp_t;
typedef nvlink_inband_mc_team_setup_rsp_msg_t nvlink_inband_mc_team_setup_replay_rsp_msg_t;

#pragma pack(pop)

/********************* Don't add any message structs after this line ******************************/

/* Helpers */
static NV_INLINE void nvlinkInitInbandMsgHdr
(
    nvlink_inband_msg_header_t *pMsgHdr,
    NvU16                       type,
    NvU32                       len,
    NvU64                       requestId
)
{
    NvU8 i;

    pMsgHdr->requestId = requestId;
    pMsgHdr->magicId = NVLINK_INBAND_MSG_MAGIC_ID_FM;
    pMsgHdr->type = type;
    pMsgHdr->length = len;
    pMsgHdr->status = NV_OK;

    for (i = 0; i < sizeof(pMsgHdr->reserved); i++)
        pMsgHdr->reserved[i] = 0;
}

#endif
