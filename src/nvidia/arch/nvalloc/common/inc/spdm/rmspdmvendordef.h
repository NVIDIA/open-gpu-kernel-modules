/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _RMSPDMVENDORDEF_H_
#define _RMSPDMVENDORDEF_H_
#include <nvtypes.h>

/*!
 * In NVIDIA's implementation of SPDM, we will support application messages
 * which are payloads delivered encrypted in SPDM secured messages,
 * sent across the SPDM channel, but do not contain SPDM messages.
 * Instead, these messages are for NVIDIA-application purposes.
 * These messages and relevant structs are defined here.
 */

/* ------------------------- Application Payloads ------------------------ */
/*!
 * All application messages will follow a simple header format in order to
 * simplify the parsing and forwarding of application messages to the
 * relevant recipient endpoint. This format is defined as follows:
 *
 * +----------------------------------+
 * |          RM_SPDM_NV_CMD          |
 * | Command Type |  Command Payload  |
 * |  (4 bytes)     (n - 4 - 4 bytes) |
 * +----------------------------------+
 *
 * The Command Type value defines the expected structure of the payload,
 * and how it should be interpreted.
 *
 * There are two major categories of commands - requests and responses.
 * The values of 0x00000000-0x7FFFFFFF are reserved for requests.
 * The values of 0x80000000-0xFFFFFFFF are reserved for responses.
 */

typedef enum
{
    // Request commands.
    RM_SPDM_NV_CMD_TYPE_REQ_KEYMGR_NVLE          = 0x00000000,
    // 0x00000001 - 0x7FFFFFFF reserved.

    // Response commands.
    RM_SPDM_NV_CMD_TYPE_RSP_SUCCESS              = 0x80000000,
    // 0x80000001 - 0xFFFFFFFF reserved.
    RM_SPDM_NV_CMD_TYPE_RSP_ERROR                = 0xFFFFFFFF,
} RM_SPDM_NV_CMD_TYPE;

typedef struct
{
    NvU32 cmdType;
} RM_SPDM_NV_CMD_HDR;

/* ------------------------- NVIDIA Requests -------------------------------- */
/*!
 * Request Command: Key Manager NVLE Command
 * Command Type:    0x00000000
 *
 * Fields:
 *      Payload -  Variable size opaque data blob containing command meant for Key manager
 *
 * This command is a generic wrapper around all commands that must be forwarded to Key Manager for NvLink Encryption.
 * The command structure itself is opaque, as SPDM need not process any of the message itself. This limits coupling
 * between the two command interfaces.
 */
typedef struct
{
    RM_SPDM_NV_CMD_HDR hdr;

    NvU8               payload[];
} RM_SPDM_NV_CMD_REQ_KEYMGR_NVLE;

/*!
 * Union of all known requests, so responder can allocate enough space for all possible requests.
 */
typedef union {
    RM_SPDM_NV_CMD_HDR             hdr;

    RM_SPDM_NV_CMD_REQ_KEYMGR_NVLE keyMgrNvle;
} RM_SPDM_NV_CMD_REQ;

/* ------------------------- NVIDIA Responses ------------------------------- */
/*!
 * Response Command: Success
 * Command Type:     0x80000000
 *
 * This command is a simple payload for the responding endpoint to
 * return success. It has no Command Payload.
 */
typedef struct
{
    RM_SPDM_NV_CMD_HDR hdr;
} RM_SPDM_NV_CMD_RSP_SUCCESS;

/*!
 * Response Command: Error
 * Command Type:     0xFFFFFFFF
 *
 * This command is a generic placeholder error response to return specific
 * failure code to application endpoint. This is used to return an error
 * back to the original application which sent the message, rather than
 * suggesting that there was some SPDM or transport layer error.
 */
typedef struct
{
    RM_SPDM_NV_CMD_HDR hdr;
    NV_STATUS          status;
} RM_SPDM_NV_CMD_RSP_ERROR;

/*!
 * Union of all known responses, so requester can allocate enough space for all possible responses.
 */
typedef union {
    RM_SPDM_NV_CMD_HDR         hdr;

    RM_SPDM_NV_CMD_RSP_SUCCESS success;
    RM_SPDM_NV_CMD_RSP_ERROR   error;
} RM_SPDM_NV_CMD_RSP;

#pragma pack()

/* ------------------------- Macros ----------------------------------------- */
#define SPDM_MAX_MEASUREMENT_BLOCK_COUNT     (64)
#define SPDM_CERT_DEFAULT_SLOT_ID            (0)
#define SPDM_CAPABILITIES_CT_EXPONENT_MAX    (0x1F)
#define SPDM_DEFAULT_HEARTBEAT_PERIOD_IN_SEC (4)

#define NV_BYTE_TO_BIT_OVERFLOW_MASK_UINT32 (0xE0000000)
#define IS_BYTE_TO_BIT_OVERFLOW_UINT32(a) \
        ((a & NV_BYTE_TO_BIT_OVERFLOW_MASK_UINT32) != 0)

/* ---------------------- Public defines ------------------------------------- */
#define NV_KEY_UPDATE_TRIGGER_THRESHOLD                 (0x10000000)

#define NV_KEY_UPDATE_TRIGGER_ID_INVALID                (0)
#define NV_KEY_UPDATE_TRIGGER_ID_GET_MEASUREMENTS       (1)
#define NV_KEY_UPDATE_TRIGGER_ID_HEARTBEAT              (2)
#define NV_KEY_UPDATE_TRIGGER_ID_LAST                   (3)

#define NV_SPDM_ENCAP_CERT_SIZE_IN_BYTE                 (578)

#endif // _RMSPDMVENDORDEF_H_
