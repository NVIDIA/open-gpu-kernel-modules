/*
 * SPDX-FileCopyrightText: Copyright (c) 2026 NVIDIA CORPORATION & AFFILIATES
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

#ifndef MCTP_FORMAT_H
#define MCTP_FORMAT_H

/**
 * MCTP (Management Component Transport Protocol) format definitions
 *
 * Based on DMTF DSP0236 - MCTP Base Specification.
 * This is a common header that will be used across multiple RM subsystems
 * like FSP, GSP, SEC2, etc. for standardized inter-processor communication.
 *
 * MCTP provides a transport-agnostic protocol for message exchange between
 * platform management components. RM uses MCTP's basic transport header
 * with vendor-defined message types (NVDM) for internal communication.
 */

// MCTP Transport Header Fields (First DWORD)
#define MCTP_HEADER_VERSION          3:0
#define MCTP_HEADER_RSVD             7:4
#define MCTP_HEADER_DEID            15:8
#define MCTP_HEADER_SEID            23:16
#define MCTP_HEADER_TAG             26:24
#define MCTP_HEADER_TO              27:27
#define MCTP_HEADER_SEQ             29:28
#define MCTP_HEADER_EOM             30:30
#define MCTP_HEADER_SOM             31:31

// MCTP Message Header Fields (Second DWORD)
#define MCTP_MSG_HEADER_TYPE         6:0
#define MCTP_MSG_HEADER_IC           7:7
#define MCTP_MSG_HEADER_VENDOR_ID   23:8
#define MCTP_MSG_HEADER_NVDM_TYPE   31:24

// MCTP Constants from specification
#define MCTP_MSG_HEADER_TYPE_VENDOR_PCI   0x7e
#define MCTP_MSG_HEADER_TYPE_VENDOR_IANA  0x7f

#define MCTP_MSG_HEADER_VENDOR_ID_NV      0x10de
#define MCTP_MSG_HEADER_VENDOR_ID_IANA_NV 0x1647

/**
 * @brief Create MCTP transport header
 *
 * Generic helper function to construct the MCTP transport header (first DWORD).
 * Can be used by any subsystem (FSP, GSP, SEC2, etc.).
 *
 * Note: TAG and TO fields are set to 0. These are only needed for complex
 * scenarios with concurrent interleaved messages or request/response correlation,
 * which are not currently used. Header version is hard-coded to 0x1 (MCTP 1.0).
 *
 * @param[in] som        Start of Message flag (1 for first packet)
 * @param[in] eom        End of Message flag (1 for last packet)
 * @param[in] seid       Source Endpoint ID
 * @param[in] deid       Destination Endpoint ID
 * @param[in] seq        Packet sequence number (0-3, modulo 4)
 *
 * @return Constructed MCTP transport header (32-bit DWORD)
 */
static NV_INLINE NvU32
mctpCreateTransportHeader
(
    NvU8 som,
    NvU8 eom,
    NvU8 seid,
    NvU8 deid,
    NvU8 seq
)
{
    return REF_NUM(MCTP_HEADER_VERSION, 0x1) |
           REF_NUM(MCTP_HEADER_SEID, seid) |
           REF_NUM(MCTP_HEADER_DEID, deid) |
           REF_NUM(MCTP_HEADER_SEQ, seq) |
           REF_NUM(MCTP_HEADER_EOM, eom) |
           REF_NUM(MCTP_HEADER_SOM, som);
}

/**
 * @brief Create NVDM over MCTP header
 *
 * Generic helper function to construct the NVDM message header (second DWORD).
 * Sets the MCTP message type to Vendor Defined PCI (0x7E) and includes
 * NVIDIA's vendor ID (0x10DE) along with the subsystem-specific NVDM type.
 * Can be used by any subsystem (FSP, GSP, SEC2, etc.).
 *
 * @param[in] nvdmType  NVDM message type (subsystem/communication-path specific)
 *
 * @return Constructed NVDM header (32-bit DWORD)
 */
static NV_INLINE NvU32
mctpCreateNvdmHeader
(
    NvU8 nvdmType
)
{
    return REF_DEF(MCTP_MSG_HEADER_TYPE, _VENDOR_PCI) |
           REF_NUM(MCTP_MSG_HEADER_IC, 0)             |
           REF_DEF(MCTP_MSG_HEADER_VENDOR_ID, _NV)    |
           REF_NUM(MCTP_MSG_HEADER_NVDM_TYPE, nvdmType);
}

#endif // MCTP_FORMAT_H
