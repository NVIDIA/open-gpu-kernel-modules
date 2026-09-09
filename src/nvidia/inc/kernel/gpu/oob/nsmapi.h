/*
 * SPDX-FileCopyrightText: Copyright (c) 2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef __NSM_API_H__
#define __NSM_API_H__

/* ------------------------ Includes --------------------------------------- */
/* ------------------------ Macros ----------------------------------------- */
#define NSM_API_MESSAGE_KIND_REQUEST  0
#define NSM_API_MESSAGE_KIND_RESPONSE 1
#define NSM_API_MESSAGE_KIND_EVENT    2

#define NSM_API_HEADER_IC_MSG                           7:0
#define NSM_API_HEADER_IC_MSG_MESSAGE_TYPE              6:0
#define NSM_API_HEADER_IC_MSG_MESSAGE_TYPE_NSM          0x7E
#define NSM_API_HEADER_IC_MSG_IC                        7:7
#define NSM_API_HEADER_IC_MSG_IC_NSM                    0x0

#define NSM_API_HEADER_VENDOR_ID_PCI                    15:0
#define NSM_API_HEADER_VENDOR_ID_PCI_NVIDIA             0x10DE

#define NSM_API_HEADER_REQUEST_DGRAM                    7:6
#define NSM_API_HEADER_REQUEST_DGRAM_RQ                 7:7
#define NSM_API_HEADER_REQUEST_DGRAM_RQ_NO              0x0
#define NSM_API_HEADER_REQUEST_DGRAM_RQ_YES             0x1
#define NSM_API_HEADER_REQUEST_DGRAM_D                  6:6
#define NSM_API_HEADER_REQUEST_DGRAM_D_NO               0x0
#define NSM_API_HEADER_REQUEST_DGRAM_D_YES              0x1
#define NSM_API_HEADER_RESERVED                         5:5
#define NSM_API_HEADER_INSTANCE_ID                      4:0
#define NSM_API_HEADER_INSTANCE_ID_MAX                  0x1F

#define NSM_API_HEADER_OCP                              7:0
#define NSM_API_HEADER_OCP_DESIGNATOR                   7:7
#define NSM_API_HEADER_OCP_DESIGNATOR_VALID             0x1
#define NSM_API_HEADER_OCP_TYPE                         6:3
#define NSM_API_HEADER_OCP_TYPE_NSM                     0x1
#define NSM_API_HEADER_OCP_VERSION                      2:0
#define NSM_API_HEADER_OCP_VERSION_1X                   0x1

typedef NvU8 NSM_API_STATUS;

/**
 * API status codes.
 */
#define NSM_API_STATUS_SUCCESS                          0U
#define NSM_API_STATUS_ERR_INVALID_ARGUMENTS            1U
#define NSM_API_STATUS_ERR_INVALID_DATA_SIZE            2U
#define NSM_API_STATUS_ERR_UNKNOWN_MCTP_MSG_TYPE        3U
#define NSM_API_STATUS_ERR_UNEXPECTED_MCTP_MSG_TYPE     4U
#define NSM_API_STATUS_ERR_UNKNOWN_VENDOR_ID            5U
#define NSM_API_STATUS_ERR_UNEXPECTED_VENDOR_ID         6U
#define NSM_API_STATUS_ERR_INVALID_INSTANCE             7U
#define NSM_API_STATUS_ERR_INVALID_OCP_BYTE             8U
#define NSM_API_STATUS_ERR_UNKNOWN_MSG_KIND             9U
#define NSM_API_STATUS_ERR_UNEXPECTED_MSG_KIND          10U
#define NSM_API_STATUS_ERR_BUFFER_TOO_SMALL             11U
#define NSM_API_STATUS_ERR_AGG_ELEMENT_BUFFER_TOO_SMALL 12U

/**
 * NVIDIA Message Type enumeration
 */
#define NSM_API_NVIDIA_MSG_TYPE_DEVICE_DISCOVERY        0x0
#define NSM_API_NVIDIA_MSG_TYPE_NVLINK                  0x1
#define NSM_API_NVIDIA_MSG_TYPE_PCIE                    0x2
#define NSM_API_NVIDIA_MSG_TYPE_PLAT_ENVIRONMENTALS     0x3
#define NSM_API_NVIDIA_MSG_TYPE_DIAGNOSTICS             0x4
#define NSM_API_NVIDIA_MSG_TYPE_DEVICE_CONFIG           0x5
#define NSM_API_NVIDIA_MSG_TYPE_COUNT                   0x6

#define NSM_API_NVIDIA_MSG_TYPE_INTERNAL                0xFF

/**
 * Completion codes
 */
#define NSM_API_COMPLETION_CODE_SUCCESS                         0x00
#define NSM_API_COMPLETION_CODE_ERROR                           0x01
#define NSM_API_COMPLETION_CODE_ERR_INVALID_DATA                0x02
#define NSM_API_COMPLETION_CODE_ERR_INVALID_DATA_LENGTH         0x03
#define NSM_API_COMPLETION_CODE_ERR_NOT_READY                   0x04
#define NSM_API_COMPLETION_CODE_ERR_UNSUPPORTED_COMMAND_CODE    0x05
#define NSM_API_COMPLETION_CODE_ERR_UNSUPPORTED_MESSAGE_TYPE    0x06
#define NSM_API_COMPLETION_CODE_ERR_BUSY                        0x7E
#define NSM_API_COMPLETION_CODE_ERR_BUS_ACCESS                  0x7F

#define NSM_API_REASON_CODE_NONE                                0x00

/**
 * Event classes
 */
#define NSM_API_EVENT_CLASS_GENERAL                     0x00
#define NSM_API_EVENT_CLASS_ASSERTION                   0x01

#pragma pack(1)
/*
 * The following types are just an indication that a given variable is meant
 * to be encoded as big-endian, regardless of the CPU's native endianness.
 */
typedef struct { NvU16 value; } NvU16BE;
/**
 * NSM Packet Header
 *
 * Common header for all NSM packets, except event ack packets
 *
 * This the the NVIDIA OEM header definition of MCTP packets.
 *
 * The PCI vendor ID (vendor_id) is the only big-endian field in the NSM spec.
 *
 * See section 7.1
 */
struct nsm_header
{
    NvU8 ic_msg_type;   //!< Integrity check and MCTP message type
    NvU16BE vendor_id;  //!< PCI vendor ID, should be 0x10DE
    NvU8 rq_d_instance; //!< Request, datagram, and instance ID
    NvU8 ocp_byte;      //!< OCP type and version
    NvU8 nv_msg_type;   //!< Nvidia message type
};

/**
 * NSM Event packet
 *
 * See section 7.4
 */
struct nsm_event
{
    struct nsm_header hdr;
    NvU8 ackr_version;    //!< ACK required and Event payload version
    NvU8 event_id;        //!< Event ID
    NvU8 event_class;     //!< Category of the reported event. T
    NvU16 event_state;    //!< Additional class-dependent info about the event
    NvU8 data_size_bytes; //!< Size of the trailing payload pointed to by `data`
    NvU8 data[];          //!< Message Payload
};

/**
 * NSM Event packet v2
 *
 * For internal use between OOBHUB and RM.
 */
struct nsm_event_v2
{
    struct nsm_header hdr;
    NvU8 ackr_version;     //!< ACK required and Event payload version
    NvU8 event_id;         //!< Event ID
    NvU8 event_class;      //!< Category of the reported event. T
    NvU16 event_state;     //!< Additional class-dependent info about the event
    NvU32 data_size_bytes; //!< Size of the trailing payload pointed to by `data`
    NvU8 data[];           //!< Message Payload
};

/**
 * NSM Request packet
 *
 * See section 7.2
 */
struct nsm_request
{
    struct nsm_header hdr;
    NvU8 command;         //!< Command code
    NvU8 data_size_bytes; //!< Size of the trailing payload pointed to by `data`
    NvU8 data[];          //!< Message Payload
};

/**
 * NSM Response packet
 *
 * See section 7.3
 */
struct nsm_response
{
    struct nsm_header hdr;
    NvU8 command;          //!< Command code
    NvU8 completion;       //!< Completion code
    NvU16 reserved;        //!< Reserved, must be 0
    NvU16 data_size_bytes; //!< Size of the trailing payload pointed to by `data`
    NvU8 data[];           //!< Message Payload
};

/**
 * NSM Response packet, with reason code
 *
 * See section 7.3.2
 */
struct nsm_response_reason
{
    struct nsm_header hdr;
    NvU8 command;          //!< Command code
    NvU8 completion;       //!< Completion code
    NvU16 reason;          //!< Reason code
};

#pragma pack()

struct nsm_event_info
{
    NvU8 version;         //!< Event data version
    NvU8 event_id;        //!< Event ID
    NvU8 event_class;     //!< Category of the reported event.
    NvU16 event_state;    //!< Additional information about the event depending on class.
    NvU8 data_size_bytes; //!< Size of the payload pointed to by `data`
    NvU8 *data;           //!< Pointer to additional data bytes
};

struct nsm_event_info_v2
{
    NvU8 version;          //!< Event data version
    NvU8 event_id;         //!< Event ID
    NvU8 event_class;      //!< Category of the reported event. T
    NvU16 event_state;     //!< Additional class-dependent info about the event
    NvU8 reserved[1];      //!< Padding to align to 4 byte boundary
    NvU32 data_size_bytes; //!< Size of the trailing payload pointed to by `data`
    NvU8 *data;            //!< Message Payload
 };

struct nsm_encdec_context
{
    void *base;          //!< Start of the input/output buffer
    NvLength total_size; //!< Total size of buffer
};

struct nsm_request_info
{
    NvU8 command;         //!< Command code
    NvU8 data_size_bytes; //!< Size of the trailing payload pointed to by `data`
    NvU8 *data;           //!< Pointer to additional data bytes
};

struct nsm_response_info
{
    NvU8 command;                                             //!< Command code
    NvU8 completion_code;                                     //!< Request completion code
    NvU16 reason_code;                                        //!< Command-specific reason code
    NvU16 data_size_bytes;                                    //!< Size of the payload pointed to by `data`
    NvU16 telemetry_count;                                    //!< Count of the number of samples
                                                              //!< returned by the aggregate request
    NvBool is_aggregate;                                      //!< Determines whether this is an aggregate response.
    union {
        NvU8 *data;                                           //!< Pointer to additional data bytes
        struct nsm_response_aggregate_element_info *agg_data; //!< Pointer to additional data for aggregate response.
    };
};

NSM_API_STATUS nsm_api_encode_header(struct nsm_encdec_context *ctx, NvU8 message_kind, NvU8 nvidia_msg_type, NvU8 instance_id);

NSM_API_STATUS nsm_api_decode_header(struct nsm_encdec_context *ctx, NvU8 *message_kind, NvU8 *nvidia_msg_type, NvU8 *instance_id);

NSM_API_STATUS nsm_api_encode_event(struct nsm_encdec_context *ctx, const struct nsm_event_info *info,
                                    NvU8 nvidia_msg_type, NvU8 instance_id);

NSM_API_STATUS nsm_api_encode_event_v2(struct nsm_encdec_context *ctx, const struct nsm_event_info_v2 *info,
                                       NvU8 nvidia_msg_type, NvU8 instance_id);

NSM_API_STATUS nsm_api_decode_request(struct nsm_encdec_context *ctx, struct nsm_request_info *info,
                                      NvU8 *nvidia_msg_type, NvU8 *instance_id);

NSM_API_STATUS nsm_api_encode_response(struct nsm_encdec_context *ctx, const struct nsm_response_info *info,
                                       NvU8 nvidia_msg_type, NvU8 instance_id);


#endif /* __NSM_API_H__ */
