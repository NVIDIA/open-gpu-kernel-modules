/*
 * SPDX-FileCopyrightText: Copyright (c) 2006-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrl0000/ctrl0000client.finn
//

#include "ctrl/ctrl0000/ctrl0000base.h"

#include "ctrl/ctrlxxxx.h"
#include "class/cl0000.h"
#include "rs_access.h"

/*
 * NV0000_CTRL_CMD_CLIENT_GET_ADDR_SPACE_TYPE
 *
 * This command may be used to query memory address space type associated with an object 
 *
 * Parameters:
 *    hObject[IN]
 *     handle of the object to look up
 *    addrSpaceType[OUT]
 *     addrSpaceType with associated memory descriptor
 *
 * Possible status values are:
 *   NV_OK
 *   NV_ERR_INVALID_OBJECT_HANDLE
 *   NV_ERR_INVALID_OBJECT
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV0000_CTRL_CMD_CLIENT_GET_ADDR_SPACE_TYPE (0xd01) /* finn: Evaluated from "(FINN_NV01_ROOT_CLIENT_INTERFACE_ID << 8) | NV0000_CTRL_CLIENT_GET_ADDR_SPACE_TYPE_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_CLIENT_GET_ADDR_SPACE_TYPE_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV0000_CTRL_CLIENT_GET_ADDR_SPACE_TYPE_PARAMS {
    NvHandle hObject;                /* [in]  - Handle of object to look up */
    NvU32    mapFlags;                  /* [in]  - Flags that will be used when mapping the object */
    NvU32    addrSpaceType;             /* [out] - Memory Address Space Type */
} NV0000_CTRL_CLIENT_GET_ADDR_SPACE_TYPE_PARAMS;

#define NV0000_CTRL_CMD_CLIENT_GET_ADDR_SPACE_TYPE_INVALID   0x00000000
#define NV0000_CTRL_CMD_CLIENT_GET_ADDR_SPACE_TYPE_SYSMEM    0x00000001
#define NV0000_CTRL_CMD_CLIENT_GET_ADDR_SPACE_TYPE_VIDMEM    0x00000002
#define NV0000_CTRL_CMD_CLIENT_GET_ADDR_SPACE_TYPE_REGMEM    0x00000003
#define NV0000_CTRL_CMD_CLIENT_GET_ADDR_SPACE_TYPE_FABRIC    0x00000004



/*
 * NV0000_CTRL_CMD_CLIENT_GET_HANDLE_INFO
 *
 * This command may be used to query information on a handle
 */
#define NV0000_CTRL_CMD_CLIENT_GET_HANDLE_INFO               (0xd02) /* finn: Evaluated from "(FINN_NV01_ROOT_CLIENT_INTERFACE_ID << 8) | NV0000_CTRL_CLIENT_GET_HANDLE_INFO_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_CLIENT_GET_HANDLE_INFO_PARAMS_MESSAGE_ID (0x2U)

typedef struct NV0000_CTRL_CLIENT_GET_HANDLE_INFO_PARAMS {
    NvHandle hObject;         /* [in]  - Handle of object to look up */
    NvU32    index;           /* [in]  - Type of lookup */

    union {
        NvHandle hResult; /* [out] - Result of lookup when result is a handle type */
        NV_DECLARE_ALIGNED(NvU64 iResult, 8); /* [out] - Result of lookup when result is a integer */
    } data;
} NV0000_CTRL_CLIENT_GET_HANDLE_INFO_PARAMS;

#define NV0000_CTRL_CMD_CLIENT_GET_HANDLE_INFO_INDEX_INVALID 0x00000000
#define NV0000_CTRL_CMD_CLIENT_GET_HANDLE_INFO_INDEX_PARENT  0x00000001
#define NV0000_CTRL_CMD_CLIENT_GET_HANDLE_INFO_INDEX_CLASSID 0x00000002

/*
 * NV0000_CTRL_CMD_CLIENT_GET_ACCESS_RIGHTS
 *
 * This command may be used to get this client's access rights for an object
 * The object to which access rights are checked does not have to be owned by
 * the client calling the command, it is owned by the hClient parameter
 */
#define NV0000_CTRL_CMD_CLIENT_GET_ACCESS_RIGHTS             (0xd03) /* finn: Evaluated from "(FINN_NV01_ROOT_CLIENT_INTERFACE_ID << 8) | NV0000_CTRL_CLIENT_GET_ACCESS_RIGHTS_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_CLIENT_GET_ACCESS_RIGHTS_PARAMS_MESSAGE_ID (0x3U)

typedef struct NV0000_CTRL_CLIENT_GET_ACCESS_RIGHTS_PARAMS {
    NvHandle       hObject;                /* [in]  - Handle of object to look up */
    NvHandle       hClient;                /* [in]  - Handle of client which owns hObject */
    RS_ACCESS_MASK maskResult;       /* [out] - Result of lookup */
} NV0000_CTRL_CLIENT_GET_ACCESS_RIGHTS_PARAMS;

/*
 * NV0000_CTRL_CMD_CLIENT_SET_INHERITED_SHARE_POLICY
 *
 * DEPRECATED: Calls NV0000_CTRL_CMD_CLIENT_SHARE_OBJECT with hObject=hClient
 *
 * This command will modify a client's inherited share policy list
 * The policy is applied in the same way that NvRmShare applies policies,
 * except to the client's inherited policy list instead of an object's policy list
 */
#define NV0000_CTRL_CMD_CLIENT_SET_INHERITED_SHARE_POLICY (0xd04) /* finn: Evaluated from "(FINN_NV01_ROOT_CLIENT_INTERFACE_ID << 8) | NV0000_CTRL_CLIENT_SET_INHERITED_SHARE_POLICY_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_CLIENT_SET_INHERITED_SHARE_POLICY_PARAMS_MESSAGE_ID (0x4U)

typedef struct NV0000_CTRL_CLIENT_SET_INHERITED_SHARE_POLICY_PARAMS {
    RS_SHARE_POLICY sharePolicy;       /* [in] - Share Policy to apply */
} NV0000_CTRL_CLIENT_SET_INHERITED_SHARE_POLICY_PARAMS;

/*
 * NV0000_CTRL_CMD_CLIENT_GET_CHILD_HANDLE
 *
 * This command may be used to get a handle of a child of a given type
 */
#define NV0000_CTRL_CMD_CLIENT_GET_CHILD_HANDLE (0xd05) /* finn: Evaluated from "(FINN_NV01_ROOT_CLIENT_INTERFACE_ID << 8) | NV0000_CTRL_CMD_CLIENT_GET_CHILD_HANDLE_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_CMD_CLIENT_GET_CHILD_HANDLE_PARAMS_MESSAGE_ID (0x5U)

typedef struct NV0000_CTRL_CMD_CLIENT_GET_CHILD_HANDLE_PARAMS {
    NvHandle hParent;                /* [in]  - Handle of parent object */
    NvU32    classId;                /* [in]  - Class ID of the child object */
    NvHandle hObject;                /* [out] - Handle of the child object (0 if not found) */
} NV0000_CTRL_CMD_CLIENT_GET_CHILD_HANDLE_PARAMS;

/*
 * NV0000_CTRL_CMD_CLIENT_SHARE_OBJECT
 *
 * This command is meant to imitate the NvRmShare API.
 * Applies a share policy to an object, which should be owned by the caller's client.
 * The policy is applied in the same way that NvRmShare applies policies.
 *
 * This ctrl command is only meant to be used in older branches. For releases after R450,
 * use NvRmShare directly instead.
 */
#define NV0000_CTRL_CMD_CLIENT_SHARE_OBJECT (0xd06) /* finn: Evaluated from "(FINN_NV01_ROOT_CLIENT_INTERFACE_ID << 8) | NV0000_CTRL_CLIENT_SHARE_OBJECT_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_CLIENT_SHARE_OBJECT_PARAMS_MESSAGE_ID (0x6U)

typedef struct NV0000_CTRL_CLIENT_SHARE_OBJECT_PARAMS {
    NvHandle        hObject;                /* [in]  - Handle of object to share */
    RS_SHARE_POLICY sharePolicy;     /* [in]  - Share Policy to apply */
} NV0000_CTRL_CLIENT_SHARE_OBJECT_PARAMS;

/*
 * NV0000_CTRL_CMD_CLIENT_OBJECTS_ARE_DUPLICATES
 *
 * This command returns true if the objects are duplicates.
 *
 * Currently supported only for memory objects.
 */
#define NV0000_CTRL_CMD_CLIENT_OBJECTS_ARE_DUPLICATES (0xd07) /* finn: Evaluated from "(FINN_NV01_ROOT_CLIENT_INTERFACE_ID << 8) | NV0000_CTRL_CLIENT_OBJECTS_ARE_DUPLICATES_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_CLIENT_OBJECTS_ARE_DUPLICATES_PARAMS_MESSAGE_ID (0x7U)

typedef struct NV0000_CTRL_CLIENT_OBJECTS_ARE_DUPLICATES_PARAMS {
    NvHandle hObject1;   /* [in]  - Handle of object to be checked */
    NvHandle hObject2;   /* [in]  - Handle of object to be checked */
    NvBool   bDuplicates;     /* [out] - Returns true if duplicates     */
} NV0000_CTRL_CLIENT_OBJECTS_ARE_DUPLICATES_PARAMS;

/*
 * NV0000_CTRL_CMD_CLIENT_SUBSCRIBE_TO_IMEX_CHANNEL:
 *
 * Subscribes to the first accessible or provided channel.
 * Subscription is valid throughout the lifetime of a client.
 *
 * devDescriptor [OPAQUE IN]
 *   devDescriptor is an IMEX file descriptor for UNIX RM clients, but a void
 *   pointer for windows RM clients. It is transparent to RM clients i.e. RM's
 *   user-mode shim populates this field on behalf of clients.
 *
 * channel [IN/OUT]
 *   Channel number to which the client is subscribed
 *   If -1 is passed, RM picks the first accessible channel.
 */
#define NV0000_CTRL_CMD_CLIENT_SUBSCRIBE_TO_IMEX_CHANNEL (0xd08) /* finn: Evaluated from "(FINN_NV01_ROOT_CLIENT_INTERFACE_ID << 8) | NV0000_CTRL_CLIENT_SUBSCRIBE_TO_IMEX_CHANNEL_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_CLIENT_SUBSCRIBE_TO_IMEX_CHANNEL_PARAMS_MESSAGE_ID (0x8U)

typedef struct NV0000_CTRL_CLIENT_SUBSCRIBE_TO_IMEX_CHANNEL_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 devDescriptor, 8);
    NvU32 channel;
} NV0000_CTRL_CLIENT_SUBSCRIBE_TO_IMEX_CHANNEL_PARAMS;



/* _ctrl0000client_h_ */

