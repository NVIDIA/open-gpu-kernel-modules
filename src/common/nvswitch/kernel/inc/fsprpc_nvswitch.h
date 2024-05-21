/*
 * SPDX-FileCopyrightText: Copyright (c) 2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _FSPRPC_NVSWITCH_H_
#define _FSPRPC_NVSWITCH_H_

#include "fsp/fsp_emem_channels.h"
#include "fsp/nvdm_payload_cmd_response.h"
#include "fsp/fsp_nvdm_format.h"
#include "fsp/fsp_mctp_format.h"
#include "fsp/fsp_tnvl_rpc.h"

#define FSP_OK                          (0x00U)
#define FSP_ERR_IFS_ERR_INVALID_STATE   (0x9EU)
#define FSP_ERR_IFR_FILE_NOT_FOUND      (0x9FU)
#define FSP_ERR_IFS_ERR_NOT_SUPPORTED   (0xA0U)
#define FSP_ERR_IFS_ERR_INVALID_DATA    (0xA1U)

#pragma pack(1)
typedef struct mctp_header
{
    NvU32 constBlob;
    NvU8 msgType;
    NvU16 vendorId;
} MCTP_HEADER;

// Needed to remove unnecessary padding
#pragma pack(1)
typedef struct nvdm_payload_cot
{
    NvU16 version;
    NvU16 size;
    NvU64 gspFmcSysmemOffset;
    NvU64 frtsSysmemOffset;
    NvU32 frtsSysmemSize;

    // Note this is an offset from the end of FB
    NvU64 frtsVidmemOffset;
    NvU32 frtsVidmemSize;

    // Authentication related fields
    NvU32 hash384[12];
    NvU32 publicKey[96];
    NvU32 signature[96];

    NvU64 gspBootArgsSysmemOffset;
} NVDM_PAYLOAD_COT;
#pragma pack()

typedef struct nvdm_packet
{
    NvU8 nvdmType;

    // We can make this a union when adding more NVDM payloads
    NVDM_PAYLOAD_COT cotPayload;
} NVDM_PACKET;

// The structure cannot have embedded pointers to send as byte stream
typedef struct mctp_packet
{
    MCTP_HEADER header;
    NVDM_PACKET nvdmPacket;
} MCTP_PACKET, *PMCTP_PACKET;

// Type of packet, can either be SOM, EOM, neither, or both (1-packet messages)
typedef enum mctp_packet_state
{
    MCTP_PACKET_STATE_START,
    MCTP_PACKET_STATE_INTERMEDIATE,
    MCTP_PACKET_STATE_END,
    MCTP_PACKET_STATE_SINGLE_PACKET
} MCTP_PACKET_STATE, *PMCTP_PACKET_STATE;

NvlStatus nvswitch_fsp_read_message(nvswitch_device *device, NvU8 *pPayloadBuffer, NvU32 payloadBufferSize);
NvlStatus nvswitch_fsp_send_packet(nvswitch_device  *device, NvU8 *pPacket, NvU32 packetSize);
NvlStatus nvswitch_fsp_send_and_read_message(nvswitch_device  *device, NvU8 *pPayload, NvU32 size, NvU32 nvdmType, NvU8 *pResponsePayload, NvU32 responseBufferSize);

#endif //_FSPRPC_NVSWITCH_H_
