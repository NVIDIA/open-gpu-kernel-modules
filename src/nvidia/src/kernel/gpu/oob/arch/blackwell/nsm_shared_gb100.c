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

#include "core/core.h"
#include "kernel/gpu/oob/nsmapi.h"

static inline NvU16BE cpu_to_be16(NvU16 x)
{
    return (NvU16BE){ .value = (NvU16)((x >> 8) | (x << 8)) };
}

static inline NvU16 be16_to_cpu(NvU16BE x)
{
    return (NvU16)((x.value >> 8) | (x.value << 8));
}

NSM_API_STATUS
nsm_api_encode_header(struct nsm_encdec_context *ctx, NvU8 message_kind, NvU8 nvidia_msg_type, NvU8 instance_id)
{
    NvU8 ic_msg_type = REF_DEF(NSM_API_HEADER_IC_MSG_MESSAGE_TYPE, _NSM) | REF_DEF(NSM_API_HEADER_IC_MSG_IC, _NSM);
    NvU16 vendor_id = REF_DEF(NSM_API_HEADER_VENDOR_ID_PCI, _NVIDIA);
    NvU8 rq_d_instance = REF_NUM(NSM_API_HEADER_INSTANCE_ID, instance_id);
    struct nsm_header *hdr = ctx->base;

    /* Make sure the buffer actually has space for the header */
    if (ctx->total_size < sizeof(struct nsm_header))
    {
        return NSM_API_STATUS_ERR_BUFFER_TOO_SMALL;
    }

    if (instance_id > NSM_API_HEADER_INSTANCE_ID_MAX)
    {
        return NSM_API_STATUS_ERR_INVALID_INSTANCE;
    }

    switch (message_kind)
    {
        case NSM_API_MESSAGE_KIND_REQUEST:
        {
            rq_d_instance |=
                REF_DEF(NSM_API_HEADER_REQUEST_DGRAM_RQ, _YES) | REF_DEF(NSM_API_HEADER_REQUEST_DGRAM_D, _NO);
            break;
        }
        case NSM_API_MESSAGE_KIND_RESPONSE:
        {
            rq_d_instance |=
                REF_DEF(NSM_API_HEADER_REQUEST_DGRAM_RQ, _NO) | REF_DEF(NSM_API_HEADER_REQUEST_DGRAM_D, _NO);
            break;
        }
        case NSM_API_MESSAGE_KIND_EVENT:
        {
            rq_d_instance |=
                REF_DEF(NSM_API_HEADER_REQUEST_DGRAM_RQ, _YES) | REF_DEF(NSM_API_HEADER_REQUEST_DGRAM_D, _YES);
            break;
        }
        default:
        {
            return NSM_API_STATUS_ERR_UNKNOWN_MSG_KIND;
        }
    }

    NvU8 ocp_byte = REF_DEF(NSM_API_HEADER_OCP_DESIGNATOR, _VALID) |
                    REF_DEF(NSM_API_HEADER_OCP_TYPE, _NSM) |
                    REF_DEF(NSM_API_HEADER_OCP_VERSION, _1X);

    hdr->ic_msg_type = ic_msg_type;
    hdr->vendor_id = cpu_to_be16(vendor_id);
    hdr->rq_d_instance = rq_d_instance;
    hdr->ocp_byte = ocp_byte;
    hdr->nv_msg_type = nvidia_msg_type;

    return NSM_API_STATUS_SUCCESS;
}

NSM_API_STATUS
nsm_api_decode_header(struct nsm_encdec_context *ctx, NvU8 *message_kind, NvU8 *nvidia_msg_type, NvU8 *instance_id)
{
    struct nsm_header *hdr = ctx->base;
    NvU8 ic_msg_type;
    NvU16 vendor_id;
    NvU8 rq_d_instance;
    NvU8 rq, d;
    NvU8 ocp_byte, ocp_type, ocp_version;

    if (ctx == NULL || message_kind == NULL)
    {
        return NSM_API_STATUS_ERR_INVALID_ARGUMENTS;
    }

    /* Double-check to make sure the buffer actually has the full header */
    if (ctx->total_size < sizeof(struct nsm_header))
    {
        return NSM_API_STATUS_ERR_BUFFER_TOO_SMALL;
    }

    ic_msg_type = hdr->ic_msg_type;
    vendor_id = be16_to_cpu(hdr->vendor_id);
    rq_d_instance = hdr->rq_d_instance;
    ocp_byte = hdr->ocp_byte;

    if ((REF_VAL(NSM_API_HEADER_IC_MSG_IC, ic_msg_type) != NSM_API_HEADER_IC_MSG_IC_NSM) ||
        (REF_VAL(NSM_API_HEADER_IC_MSG_MESSAGE_TYPE, ic_msg_type) != NSM_API_HEADER_IC_MSG_MESSAGE_TYPE_NSM))
    {
        return NSM_API_STATUS_ERR_UNEXPECTED_MCTP_MSG_TYPE;
    }

    if (REF_VAL(NSM_API_HEADER_VENDOR_ID_PCI, vendor_id) != NSM_API_HEADER_VENDOR_ID_PCI_NVIDIA)
    {
        return NSM_API_STATUS_ERR_UNEXPECTED_VENDOR_ID;
    }

    rq = REF_VAL(NSM_API_HEADER_REQUEST_DGRAM_RQ, rq_d_instance);
    d = REF_VAL(NSM_API_HEADER_REQUEST_DGRAM_D, rq_d_instance);

    if ((rq == NSM_API_HEADER_REQUEST_DGRAM_RQ_YES) && (d == NSM_API_HEADER_REQUEST_DGRAM_D_NO))
    {
        *message_kind = NSM_API_MESSAGE_KIND_REQUEST;
    }
    else if ((rq == NSM_API_HEADER_REQUEST_DGRAM_RQ_NO) && (d == NSM_API_HEADER_REQUEST_DGRAM_D_NO))
    {
        *message_kind = NSM_API_MESSAGE_KIND_RESPONSE;
    }
    else if ((rq == NSM_API_HEADER_REQUEST_DGRAM_RQ_YES) && (d == NSM_API_HEADER_REQUEST_DGRAM_D_YES))
    {
        *message_kind = NSM_API_MESSAGE_KIND_EVENT;
    }
    else
    {
        return NSM_API_STATUS_ERR_UNKNOWN_MSG_KIND;
    }

    ocp_type = REF_VAL(NSM_API_HEADER_OCP_TYPE, ocp_byte);
    ocp_version = REF_VAL(NSM_API_HEADER_OCP_VERSION, ocp_byte);

    if ((ocp_type != NSM_API_HEADER_OCP_TYPE_NSM) || (ocp_version != NSM_API_HEADER_OCP_VERSION_1X))
    {
        return NSM_API_STATUS_ERR_INVALID_OCP_BYTE;
    }

    if (nvidia_msg_type)
    {
        *nvidia_msg_type = hdr->nv_msg_type;
    }

    if (instance_id)
    {
        *instance_id = REF_VAL(NSM_API_HEADER_INSTANCE_ID, rq_d_instance);
    }

    return NSM_API_STATUS_SUCCESS;
}

NSM_API_STATUS
nsm_api_encode_event(struct nsm_encdec_context *ctx, const struct nsm_event_info *info,
                     NvU8 nvidia_msg_type, NvU8 instance_id)
{
    struct nsm_event *pkt = ctx->base;
    NSM_API_STATUS rc;

    /* Ensure that the info->data pointer is valid */
    if (info->data_size_bytes && !info->data)
    {
        return NSM_API_STATUS_ERR_INVALID_DATA_SIZE;
    }

    /* Ensure that the packet is large enough to hold the requested data */
    if (info->data_size_bytes > (ctx->total_size - sizeof(*pkt)))
    {
        return NSM_API_STATUS_ERR_INVALID_DATA_SIZE;
    }

    portMemSet(pkt, 0, sizeof(struct nsm_event) + info->data_size_bytes);

    rc = nsm_api_encode_header(ctx, NSM_API_MESSAGE_KIND_EVENT, nvidia_msg_type, instance_id);
    if (rc != NSM_API_STATUS_SUCCESS)
    {
        return rc;
    }

    pkt->ackr_version = info->version;
    pkt->data_size_bytes = info->data_size_bytes;
    pkt->event_id = info->event_id;
    pkt->event_class = info->event_class;
    pkt->event_state = info->event_state;

    if (info->data != NULL)
    {
        portMemCopy(pkt->data, info->data_size_bytes, info->data, info->data_size_bytes);
    }

    return NSM_API_STATUS_SUCCESS;
}

NSM_API_STATUS
nsm_api_encode_event_v2(struct nsm_encdec_context *ctx, const struct nsm_event_info_v2 *info,
                     NvU8 nvidia_msg_type, NvU8 instance_id)
{
    struct nsm_event_v2 *pkt = ctx->base;
    NSM_API_STATUS rc;

    /* Ensure that the info->data pointer is valid */
    if (info->data_size_bytes && !info->data)
    {
        return NSM_API_STATUS_ERR_INVALID_DATA_SIZE;
    }

    /* Ensure that the packet is large enough to hold the requested data */
    if (info->data_size_bytes > (ctx->total_size - sizeof(*pkt)))
    {
        return NSM_API_STATUS_ERR_INVALID_DATA_SIZE;
    }

    portMemSet(pkt, 0, sizeof(struct nsm_event_v2) + info->data_size_bytes);

    rc = nsm_api_encode_header(ctx, NSM_API_MESSAGE_KIND_EVENT, nvidia_msg_type, instance_id);
    if (rc != NSM_API_STATUS_SUCCESS)
    {
        return rc;
    }

    pkt->ackr_version = info->version;
    pkt->data_size_bytes = info->data_size_bytes;
    pkt->event_id = info->event_id;
    pkt->event_class = info->event_class;
    pkt->event_state = info->event_state;

    if (info->data != NULL)
    {
        portMemCopy(pkt->data, info->data_size_bytes, info->data, info->data_size_bytes);
    }

    return NSM_API_STATUS_SUCCESS;
}

NSM_API_STATUS
nsm_api_decode_request(struct nsm_encdec_context *ctx, struct nsm_request_info *info,
                       NvU8 *nvidia_msg_type, NvU8 *instance_id)
{
    struct nsm_request *pkt = ctx->base;
    NSM_API_STATUS rc;
    NvU8 msg_kind;

    if (ctx->total_size < sizeof(*pkt))
    {
        return NSM_API_STATUS_ERR_BUFFER_TOO_SMALL;
    }

    rc = nsm_api_decode_header(ctx, &msg_kind, nvidia_msg_type, instance_id);
    if (rc != NSM_API_STATUS_SUCCESS)
    {
        return rc;
    }

    if (msg_kind != NSM_API_MESSAGE_KIND_REQUEST)
    {
        return NSM_API_STATUS_ERR_UNEXPECTED_MSG_KIND;
    }

    info->command = pkt->command;

    // Check if the data size will exceed the size of the overall packet
    if (pkt->data_size_bytes > (ctx->total_size - sizeof(*pkt)))
    {
        return NSM_API_STATUS_ERR_INVALID_DATA_SIZE;
    }

    // If there is a payload in the packet, we need to copy it
    if (pkt->data_size_bytes)
    {
        // If a buffer is specified, then use it
        if (info->data)
        {
            if (info->data_size_bytes < pkt->data_size_bytes)
            {
                // Return buffer is too small
                return NSM_API_STATUS_ERR_BUFFER_TOO_SMALL;
            }

            portMemCopy(info->data,info->data_size_bytes, pkt->data, pkt->data_size_bytes);
        }
        else
        {
            // Otherwise, reuse the context buffer
            info->data = pkt->data;
        }
    }
    else
    {
        info->data = NULL;
    }

    info->data_size_bytes = pkt->data_size_bytes;

    return NSM_API_STATUS_SUCCESS;
}

NSM_API_STATUS
nsm_api_encode_response(struct nsm_encdec_context *ctx, const struct nsm_response_info *info,
                        NvU8 nvidia_msg_type, NvU8 instance_id)
{
    NSM_API_STATUS rc;

    /* Ensure that the info->data pointer is valid */
    if (info->data_size_bytes && !info->data)
    {
        return NSM_API_STATUS_ERR_INVALID_DATA_SIZE;
    }

    if (info->is_aggregate)
    {
        return NSM_API_STATUS_ERR_INVALID_ARGUMENTS;
    }

    portMemSet(ctx->base, 0, ctx->total_size);

    rc = nsm_api_encode_header(ctx, NSM_API_MESSAGE_KIND_RESPONSE, nvidia_msg_type, instance_id);
    if (rc != NSM_API_STATUS_SUCCESS)
    {
        return rc;
    }

    if (info->completion_code)
    {
        struct nsm_response_reason *pkt = ctx->base;

        // If there is a non-zero completion code, then this is actually a
        // nsm_response_reason packet, so encode it accordingly.  However,
        // these packets don't have payload, so make sure there isn't any.

        if (info->data_size_bytes)
        {
            return NSM_API_STATUS_ERR_INVALID_DATA_SIZE;
        }

        /* Ensure that the packet is large enough to hold the requested data */
        if (ctx->total_size < sizeof(*pkt))
        {
            return NSM_API_STATUS_ERR_BUFFER_TOO_SMALL;
        }

        pkt->command = info->command;
        pkt->completion = info->completion_code;
        pkt->reason = info->reason_code;

        return NSM_API_STATUS_SUCCESS;
    }

    struct nsm_response *pkt = ctx->base;

    /* Ensure that the packet is large enough to hold the requested data */
    if (info->data_size_bytes > (ctx->total_size - sizeof(*pkt)))
    {
        return NSM_API_STATUS_ERR_INVALID_DATA_SIZE;
    }

    pkt->command = info->command;
    pkt->completion = info->completion_code;
    pkt->data_size_bytes = info->data_size_bytes;

    if (info->data)
    {
        portMemCopy(pkt->data, info->data_size_bytes, info->data, info->data_size_bytes);
    }

    return NSM_API_STATUS_SUCCESS;
}
