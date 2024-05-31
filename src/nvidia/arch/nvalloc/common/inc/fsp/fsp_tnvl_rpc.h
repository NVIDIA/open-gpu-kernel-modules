/*
 * SPDX-FileCopyrightText: Copyright (c) 2023 NVIDIA CORPORATION & AFFILIATES
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

#ifndef _FSP_TNVL_RPC_H_
#define _FSP_TNVL_RPC_H_

#define TNVL_CAPS_SUBMESSAGE_ID 0xFF
#define TNVL_GET_ATT_CERTS_SUBMESSAGE_ID  0x0
#define TNVL_GET_ATT_REPORT_SUBMESSAGE_ID 0x1
#define TNVL_LOCK_CONFIG_SUBMESSAGE_ID    0x2

#pragma pack(1)

 /*!
  * @brief TNVL payload to query FSP capabilities
  */
typedef struct
{
    NvU8 subMessageId;
} TNVL_RPC_CAPS_PAYLOAD;

/*!
  * @brief TNVL response payload for FSP capability query
  */
typedef struct
{
    NvU8 nvdmType;
    NVDM_PAYLOAD_COMMAND_RESPONSE cmdResponse;
    NvU8 rspPayload[40];
} TNVL_RPC_CAPS_RSP_PAYLOAD;

 /*!
  * @brief TNVL payload to query attestation cert chain
  */
typedef struct
{
    NvU8 subMessageId;
    NvU8 rsvd;
    NvU8 minorVersion;
    NvU8 majorVersion;
} TNVL_GET_ATT_CERTS_CMD_PAYLOAD;

/*!
  * @brief TNVL response payload for attestation cert chain
  */
typedef struct
{
    NVDM_PAYLOAD_COMMAND_RESPONSE cmdResponse;
    NvU8 subMessageId;
    NvU8 rsvd0;
    NvU8 minorVersion;
    NvU8 majorVersion;
    NvU16 certChainLength;
    NvU16 rsvd1;
    NvU8 certChain[NVSWITCH_ATTESTATION_CERT_CHAIN_MAX_SIZE];
} TNVL_GET_ATT_CERTS_RSP_PAYLOAD;

 /*!
  * @brief TNVL payload to query attestation report
  */
typedef struct
{
    NvU8 subMessageId;
    NvU8 rsvd;
    NvU8 minorVersion;
    NvU8 majorVersion;
    NvU8 nonce[NVSWITCH_NONCE_SIZE];
} TNVL_GET_ATT_REPORT_CMD_PAYLOAD;

/*!
 * @brief TNVL response payload for attestation report
 */
typedef struct
{
    NVDM_PAYLOAD_COMMAND_RESPONSE cmdResponse;
    NvU8 subMessageId;
    NvU8 rsvd0;
    NvU8 minorVersion;
    NvU8 majorVersion;
    NvU32 measurementSize;
    NvU8 measurementBuffer[NVSWITCH_ATTESTATION_REPORT_MAX_SIZE];
} TNVL_GET_ATT_REPORT_RSP_PAYLOAD;

/*!
 * @brief TNVL payload to send lock config
 */
typedef struct
{
    NvU8 subMessageId;
    NvU8 rsvd;
    NvU8 minorVersion;
    NvU8 majorVersion;
} TNVL_LOCK_CONFIG_CMD_PAYLOAD;

/*!
 * @brief TNVL response payload for lock config
 */
typedef struct
{
    NVDM_PAYLOAD_COMMAND_RESPONSE cmdResponse;
    NvU8 subMessageId;
    NvU8 rsvd0;
    NvU8 minorVersion;
    NvU8 majorVersion;
} TNVL_LOCK_CONFIG_RSP_PAYLOAD;

#pragma pack()

#endif // _FSP_TNVL_RPC_H_
