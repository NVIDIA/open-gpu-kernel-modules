/*
 * SPDX-FileCopyrightText: Copyright (c) 2022-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/*!
 * Provides the implementation for all GH100 specific SPDM HALs
 * interfaces.
 */

/* ------------------------ Includes --------------------------------------- */
#include "nvRmReg.h"
#include "gpu/spdm/spdm.h"
#include "spdm/rmspdmtransport.h"
#include "spdm/rmspdmvendordef.h"
#include "objtmr.h"
#include "gpu/bus/kern_bus.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "gpu/spdm/libspdm_includes.h"
#include "rmapi/client_resource.h"
#include "ctrl/ctrl2080/ctrl2080spdm.h"
#include "flcnretval.h"
#include "gpu/conf_compute/conf_compute.h"

/* ------------------------ Macros ----------------------------------------- */
#define SPDM_MESSAGE_BUFFER_ADDRESS_ALIGNMENT    (256)

#define SPDM_CAPABILITIES_FLAGS_GH100 \
        SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_CERT_CAP       | \
        SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_MEAS_CAP_SIG   | \
        SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_MEAS_FRESH_CAP | \
        SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_ENCRYPT_CAP    | \
        SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_MAC_CAP        | \
        SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_KEY_EX_CAP     | \
        SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_KEY_UPD_CAP

/* ------------------------ Types Definitions ------------------------------ */
/*!
 * Define context structure which tracks all state required for IO between
 * RM and GSP. They communicate via a shared memory surface in which SPDM
 * payloads are stored, and use the RM<->GSP command queue to alert of payloads.
 */
typedef struct _SPDM_GSP_IO_CONTEXT
{
    /*!
     * Buffer to store SPDM response read from memory surface. Required since
     * libspdm splits send and receive message across two message calls.
     */
    NV_DECLARE_ALIGNED(NvU8 *pResponse, 8);

    /*! The memory descriptor that describes the memory surface */
    NV_DECLARE_ALIGNED(MEMORY_DESCRIPTOR *pMemDesc,  8);

    /*!
     * The size of the surface.  This information is also contained in
     * the surfaces's memory descriptor but is still required since
     * surface structures will be allocated and populated before the
     * underlying surfaces are allocated themselves (time at which the
     * memory descriptors will allocated and populated).
     */
    NvU32 surfaceSizeInByte;

    /*!
     * Represents the amount by which the actual size of the surface will
     * aligned when the surface is allocated.
     */
    NvU32 alignSize;

    /*!
     * Stores the address that the GSP will use when accessing the surface.
     */
    NvU64 dmaAddr;

    /*!
     * Address space for memory surface (sysmem or FB).
     */
    NvU32 addrSpace;

    /*!
     * Sequence number for the memory surface.
     */
    NvU32 seqNum;

    /*!
     * Region ID for the payload (i.e. unprotected, protected).
     */
    NvU32 regionId;

    /*!
     * Size allocated for the SPDM message decription header, which
     * describes the payload contained in the surface.
     */
    NvU32 descHdrSizeInByte;

    /*!
     * Size of the surface available to RM for storing SPDM messages.
     */
    NvU32 rmBufferSizeInByte;

    /*!
     * Size of the above response buffer.
     */
    NvU32 responseSize;

} SPDM_GSP_IO_CONTEXT;

/* ------------------------ Static Variables ------------------------------- */
//
// For transport functionality, we require access to the GPU and Spdm objects.
// However, libspdm transport layer currently does not support passing any custom
// parameters, meaning we must use static variables to access these objects.
// If we ever require multiple instances of the Spdm object, this will be an issue.
//
static OBJGPU *g_pGpu  = NULL;
static Spdm   *g_pSpdm = NULL;

static SPDM_ALGO_CHECK_ENTRY g_SpdmAlgoCheckTable_GH100[] =
{
    { LIBSPDM_DATA_MEASUREMENT_SPEC,       SPDM_MEASUREMENT_BLOCK_HEADER_SPECIFICATION_DMTF },
    { LIBSPDM_DATA_MEASUREMENT_HASH_ALGO,  SPDM_ALGORITHMS_MEASUREMENT_HASH_ALGO_TPM_ALG_SHA_384 },
    { LIBSPDM_DATA_BASE_ASYM_ALGO,         SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_ECDSA_ECC_NIST_P384 },
    { LIBSPDM_DATA_BASE_HASH_ALGO,         SPDM_ALGORITHMS_BASE_HASH_ALGO_TPM_ALG_SHA_384 },
    { LIBSPDM_DATA_DHE_NAME_GROUP,         SPDM_ALGORITHMS_DHE_NAMED_GROUP_SECP_384_R1 },
    { LIBSPDM_DATA_AEAD_CIPHER_SUITE,      SPDM_ALGORITHMS_AEAD_CIPHER_SUITE_AES_256_GCM },
    { LIBSPDM_DATA_KEY_SCHEDULE,           SPDM_ALGORITHMS_KEY_SCHEDULE_HMAC_HASH },
    { LIBSPDM_DATA_OTHER_PARAMS_SUPPORT,   0 },
    { LIBSPDM_DATA_REQ_BASE_ASYM_ALG,      0 }
};

/* ------------------------ Static Function Prototypes --------------------- */
static void _spdmReleaseSurfaceGsp(Spdm *pSpdm);

static NV_STATUS _spdmErrorCheck(RM_GSP_SPDM_MSG *pSpdmMsg,
                                 NvU32            guestIdChk,
                                 NvU32            endpointIdChk,
                                 NvU32            cmdType);

//
// Static transport layer functions which we pass to libspdm as function pointers.
// The libspdm library will then use these functions to send and receive SPDM messages.
// Function parameters and types must match those expected by libspdm.
//
static uint32_t _spdmGetTransportHeaderSizeGsp(void *spdm_context);

static libspdm_return_t _spdmEncodeMessageGsp(void *spdm_context, const uint32_t *session_id,
                                              bool is_app_message, bool is_requester,
                                              size_t message_size, void *message,
                                              size_t *transport_message_size,
                                              void **transport_message);

static libspdm_return_t _spdmDecodeMessageGsp(void *spdm_context, uint32_t **session_id,
                                              bool *is_app_message, bool is_requester,
                                              size_t transport_message_size, void *transport_message,
                                              size_t *message_size, void **message);

static libspdm_return_t _spdmSendMessageGsp(void *spdm_context, size_t message_size,
                                            const void *message, uint64_t timeout);

static libspdm_return_t _spdmReceiveMessageGsp(void *spdm_context, size_t *message_size,
                                                void **message, uint64_t timeout);

static NV_STATUS _spdmSendInitRmDataCommand(OBJGPU *pGpu, Spdm *pSpdm);

/* ------------------------ Static Functions ------------------------------- */
/*!
 * Frees all memory and state associated with the GSP SPDM-message surface.
 */
void
_spdmReleaseSurfaceGsp
(
    Spdm *pSpdm
)
{
    SPDM_GSP_IO_CONTEXT *pIOContext = NULL;

    if (pSpdm == NULL || pSpdm->pDeviceIOContext == NULL)
    {
       NV_PRINTF(LEVEL_ERROR, "SPDM: Attempting to release NULL MemDesc, this is BUG!\n");
       DBG_BREAKPOINT();
       return;
    }

    pIOContext = (SPDM_GSP_IO_CONTEXT *)pSpdm->pDeviceIOContext;
    memdescFree(pIOContext->pMemDesc);
    memdescDestroy(pIOContext->pMemDesc);
    portMemFree(pIOContext->pResponse);
    portMemSet(pIOContext, 0, sizeof(SPDM_GSP_IO_CONTEXT));
    portMemFree(pIOContext);

    // Ensure we clear object context pointer.
    pSpdm->pDeviceIOContext = NULL;
}

/*!
 * Performs validation of received RPC SPDM message and
 * by comparing the message against expected values. Used
 * to detect mismatch between attributes of request sent
 * and attributes of response received.
 *
 * @param[in] pSpdmMsg          Pointer to SPDM Message structure.
 * @param[in] guestIdChk        Expected guest ID.
 * @param[in] endpointIdChk     Expected endpoint ID.
 * @param[in] cmdType           SPDM command type.
 */
NV_STATUS
_spdmErrorCheck
(
    RM_GSP_SPDM_MSG     *pSpdmMsg,
    NvU32                guestIdChk,
    NvU32                endpointIdChk,
    NvU32                cmdType
)
{
    // Perform validation of received RPC SPDM message.
    if (pSpdmMsg->status != FLCN_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "SPDM: RPC message error : status = 0x%x.\n",
                                pSpdmMsg->status);

        DBG_BREAKPOINT();
        return NV_ERR_GENERIC;
    }

    // Perform validation of received SPDM message description header only if SPDM RPC message status is FLCN_OK.
    if (pSpdmMsg->version != NV_SPDM_DESC_HEADER_VERSION_CURRENT)
    {
        NV_PRINTF(LEVEL_ERROR,
                "SPDM: Version mismatch: [Check] version = 0x%x, [SpdmRet] version = 0x%x\n",
                NV_SPDM_DESC_HEADER_VERSION_CURRENT, pSpdmMsg->version);
        DBG_BREAKPOINT();
        return NV_ERR_GENERIC;
    }

    if (pSpdmMsg->guestId != guestIdChk)
    {
        NV_PRINTF(LEVEL_ERROR,
                "SPDM: Guest ID mismatch: [Check] guestId = 0x%x, [SpdmRet] guestId = 0x%x\n",
                guestIdChk, pSpdmMsg->guestId);
        DBG_BREAKPOINT();
        return NV_ERR_GENERIC;
    }

    if (cmdType != RM_GSP_SPDM_CMD_ID_CC_INIT)
    {
        if (pSpdmMsg->endpointId != endpointIdChk)
        {
            NV_PRINTF(LEVEL_ERROR,
                    "SPDM: Endpoint ID mismatch: [Check] endpointId = 0x%x, [SpdmRet] endpointId = 0x%x\n",
                    endpointIdChk, pSpdmMsg->endpointId);
            DBG_BREAKPOINT();
            return NV_ERR_GENERIC;
        }
    }

    return NV_OK;
}

/*!
 * Callback to fill out sequence number in expected format.
 * The sequence number is for secured message format only and defined in DMTF DSP0277.
 * Currently, requester(RM) and responder(GSP-SPDM) doesn't support sequence number.
 */
uint8_t
_spdmGetSequenceNumberGsp
(
    uint64_t  sequence_number,
    uint8_t  *sequence_number_buffer
)
{
    // No sequence number included as a part of GSP secured message.
    return 0;
}

/*!
 * Callback to fill RNG blob in secured message.
 * The random number size is for secured message format only and defined in DMTF DSP0277.
 */
uint32_t
_spdmGetMaxRandomNumberCountGsp
(
    void
)
{
    return NV_SPDM_MAX_RANDOM_MSG_BYTES;
}

/*!
 * Callback to get allocated space for transport header that is pre-pended
 * to the SPDM message. Return the size of the NV_SPDM_MESSAGE_HEADER we prepend.
 */
uint32_t
_spdmGetTransportHeaderSizeGsp
(
    void *spdm_context
)
{
    return sizeof(NV_SPDM_DESC_HEADER);
}

/*!
 * Static function libspdm uses as hook to RM<->GSP transport layer.
 * If secured, encodes message into SPDM Secured Message format.
 */
libspdm_return_t
_spdmEncodeMessageGsp
(
    void            *spdm_context,
    const uint32_t  *session_id,
    bool             is_app_message,
    bool             is_requester,
    size_t           message_size,
    void            *message,
    size_t          *transport_message_size,
    void           **transport_message
)
{
    libspdm_secured_message_callbacks_t  securedMessageInfo;
    libspdm_return_t                     status                 = LIBSPDM_STATUS_SUCCESS;
    size_t                               securedMessageSize     = 0;
    void                                *pSecuredMessageContext = NULL;
    NV_SPDM_DESC_HEADER                 *pNvSpdmDescHdr         = NULL;
    NvU32                                payloadSize            = 0;
    SPDM_GSP_IO_CONTEXT                 *pIOContext             = NULL;

    pIOContext = (SPDM_GSP_IO_CONTEXT *)g_pSpdm->pDeviceIOContext;

    // Check libspdm parameters.
    if (spdm_context == NULL || message == NULL || message_size == 0 ||
        transport_message == NULL || *transport_message == NULL ||
        transport_message_size == NULL || pIOContext == NULL)
    {
        return LIBSPDM_STATUS_INVALID_PARAMETER;
    }

    // Only support sending requester SPDM/Secured messages.
    if (!is_requester || is_app_message)
    {
        return LIBSPDM_STATUS_INVALID_PARAMETER;
    }

    // The transport buffer must be large enough to fit the transport header and the message.
    if (*transport_message_size < (sizeof(NV_SPDM_DESC_HEADER) + message_size))
    {
        return LIBSPDM_STATUS_INVALID_MSG_FIELD;
    }

    // Initialize descriptor header.
    pNvSpdmDescHdr = (NV_SPDM_DESC_HEADER *)*transport_message;
    portMemSet(pNvSpdmDescHdr, 0, sizeof(NV_SPDM_DESC_HEADER));

    // Determine whether message is secured.
    if (session_id != NULL)
    {
        pNvSpdmDescHdr->msgType = NV_SPDM_MESSAGE_TYPE_SECURED;

        pSecuredMessageContext =
            libspdm_get_secured_message_context_via_session_id(spdm_context, *session_id);

        if (pSecuredMessageContext == NULL)
        {
            return LIBSPDM_STATUS_SESSION_MSG_ERROR;
        }

        // Calculate max space we have for secured message in transport buffer.
        securedMessageSize = *transport_message_size;
        securedMessageSize -= sizeof(NV_SPDM_DESC_HEADER);

        // Initialize secured message attributes.
        portMemSet(&securedMessageInfo, 0, sizeof(securedMessageInfo));
        securedMessageInfo.version                     = SPDM_SECURED_MESSAGE_CALLBACKS_VERSION;
        securedMessageInfo.get_sequence_number         = _spdmGetSequenceNumberGsp;
        securedMessageInfo.get_max_random_number_count = _spdmGetMaxRandomNumberCountGsp;

        // Encode secured message into output buffer.
        status = libspdm_encode_secured_message(pSecuredMessageContext, *session_id, is_requester,
                                                message_size, message, &securedMessageSize,
                                                (pNvSpdmDescHdr + 1), &securedMessageInfo);
        if (status != LIBSPDM_STATUS_SUCCESS)
        {
            return status;
        }

        // Transport message buffer must be large enough to store secured message + NV_SPDM_DESC_HEADER.
        NV_ASSERT_OR_RETURN(*transport_message_size > (securedMessageSize + sizeof(NV_SPDM_DESC_HEADER)),
                            LIBSPDM_STATUS_INVALID_MSG_FIELD);

        pNvSpdmDescHdr->msgSizeByte = (NvU32)securedMessageSize;
        *transport_message_size     = securedMessageSize + sizeof(NV_SPDM_DESC_HEADER);
    }
    else
    {
        // The normal message is not encrypted, it will be sent as NV_SPDM_DESC_HEADER + Message.
        payloadSize = sizeof(NV_SPDM_DESC_HEADER) + message_size;

        //
        // Check for large enough buffer for payload, as well as for overflow in
        // operation above. If no overflow, we know message_size fits in NvU32.
        //
        if (*transport_message_size < payloadSize || payloadSize < message_size)
        {
            return LIBSPDM_STATUS_BUFFER_TOO_SMALL;
        }

        //
        // Fill in SPDM message header.
        // The SPDM message is already after the SPDM msg header.
        //
        pNvSpdmDescHdr->msgType     = NV_SPDM_MESSAGE_TYPE_NORMAL;
        pNvSpdmDescHdr->msgSizeByte = (NvU32)message_size;
        *transport_message_size     = payloadSize;
    }

    // Check final encrypted message size.
    if (*transport_message_size > NV_SPDM_RM_SURFACE_SIZE_IN_BYTE)
    {
        return LIBSPDM_STATUS_BUFFER_TOO_SMALL;
    }

    // Record the message in the last exchange transcript.
    if (message_size > SPDM_MAX_EXCHANGE_BUFFER_SIZE)
    {
        return LIBSPDM_STATUS_INVALID_PARAMETER;
    }

    portMemCopy(g_pSpdm->pLastExchange, SPDM_MAX_EXCHANGE_BUFFER_SIZE,
                message, message_size);
    g_pSpdm->lastExchangeSize = message_size;

    return LIBSPDM_STATUS_SUCCESS;
}

/*!
 * Static function libspdm uses as hook to RM<->GSP transport layer.
 * If secured, decodes the message from the SPDM Secured Message format.
 */
libspdm_return_t
_spdmDecodeMessageGsp
(
    void      *spdm_context,
    uint32_t **session_id,
    bool      *is_app_message,
    bool       is_requester,
    size_t     transport_message_size,
    void      *transport_message,
    size_t    *message_size,
    void     **message
)
{
    libspdm_secured_message_callbacks_t    securedMessageInfo;
    NV_SPDM_DESC_HEADER                   *pNvSpdmDescHdr         = NULL;
    NvU32                                  payloadSize            = 0;
    void                                  *pSecuredMessageContext = NULL;
    libspdm_return_t                       status                 = LIBSPDM_STATUS_SUCCESS;
    NvU32                                  lastExchangeSizeLeft   = 0;
    spdm_secured_message_a_data_header1_t *pSpdmSecuredMsgHdr     = NULL;

    // Check libspdm parameters.
    if (spdm_context == NULL || session_id == NULL || is_app_message == NULL ||
        transport_message_size == 0 || transport_message == NULL ||
        message_size == NULL || message == NULL || *message == NULL)
    {
        return LIBSPDM_STATUS_INVALID_PARAMETER;
    }

    // Only support receiving Responder SPDM/Secured messages.
    if (is_requester)
    {
        return LIBSPDM_STATUS_INVALID_PARAMETER;
    }

    // Retrieve NV-header from message, and perform basic validation.
    pNvSpdmDescHdr = (NV_SPDM_DESC_HEADER *)transport_message;
    if (transport_message_size < sizeof(NV_SPDM_DESC_HEADER) ||
        transport_message_size > NV_SPDM_RM_SURFACE_SIZE_IN_BYTE)
    {
        return LIBSPDM_STATUS_INVALID_MSG_FIELD;
    }

    payloadSize = NV_SPDM_DESC_HEADER_SIZE_IN_BYTE + pNvSpdmDescHdr->msgSizeByte;

    if (transport_message_size  != payloadSize)
    {
        return LIBSPDM_STATUS_INVALID_MSG_FIELD;
    }

    // Decode message, based on type.
    switch (pNvSpdmDescHdr->msgType)
    {
        case NV_SPDM_MESSAGE_TYPE_SECURED:
        {
            //
            // Double-check the payload fits a secured message header.
            // Our implementation of a secure message header only includes
            // session ID - no sequence number.
            //
            if (pNvSpdmDescHdr->msgSizeByte < sizeof(spdm_secured_message_a_data_header1_t))
            {
                return LIBSPDM_STATUS_INVALID_MSG_FIELD;
            }

            // Secured message header begins immediately after general NV header.
            pSpdmSecuredMsgHdr = (spdm_secured_message_a_data_header1_t *)(pNvSpdmDescHdr + 1);
            *session_id = &pSpdmSecuredMsgHdr->session_id;

            pSecuredMessageContext =
                libspdm_get_secured_message_context_via_session_id(spdm_context, **session_id);

            if (pSecuredMessageContext == NULL)
            {
                return LIBSPDM_STATUS_SESSION_MSG_ERROR;
            }

            // Initialize secured message attributes.
            portMemSet(&securedMessageInfo, 0, sizeof(securedMessageInfo));
            securedMessageInfo.version                     = SPDM_SECURED_MESSAGE_CALLBACKS_VERSION;
            securedMessageInfo.get_sequence_number         = _spdmGetSequenceNumberGsp;
            securedMessageInfo.get_max_random_number_count = _spdmGetMaxRandomNumberCountGsp;

            // Decode and retrieve application payload from secured message.
            // We must copy the message to the scratch buffer.
            status = libspdm_decode_secured_message(pSecuredMessageContext,
                                                    **session_id, is_requester,
                                                    transport_message_size - NV_SPDM_DESC_HEADER_SIZE_IN_BYTE,
                                                    (pNvSpdmDescHdr + 1), message_size,
                                                    message, &securedMessageInfo);
            if (status != LIBSPDM_STATUS_SUCCESS)
            {
                return status;
            }
        }
        break;

        case NV_SPDM_MESSAGE_TYPE_NORMAL:
        {
            // Indicate the message is unsecured.
            *session_id = NULL;

            //
            // We both check that the buffer is large enough, and that the
            // size_t type is large enough to hold the size of the message.
            //
            if (*message_size < pNvSpdmDescHdr->msgSizeByte)
            {
                return LIBSPDM_STATUS_BUFFER_TOO_SMALL;
            }

            // The message is already present in the receiver buffer.
            // Just use that.
            *message      = (uint8_t *)(pNvSpdmDescHdr + 1);
            *message_size = pNvSpdmDescHdr->msgSizeByte;
        }
        break;

        default:
        {
            return LIBSPDM_STATUS_INVALID_MSG_FIELD;
        }
    }

    // We don't expect app message for any scenario.
    *is_app_message = NV_FALSE;

    // Record the message in the last exchange transcript.
    lastExchangeSizeLeft = SPDM_MAX_EXCHANGE_BUFFER_SIZE - g_pSpdm->lastExchangeSize;
    if (*message_size > lastExchangeSizeLeft || SPDM_MAX_EXCHANGE_BUFFER_SIZE < g_pSpdm->lastExchangeSize)
    {
        return LIBSPDM_STATUS_INVALID_PARAMETER;
    }

    portMemCopy((NvU8 *)g_pSpdm->pLastExchange + g_pSpdm->lastExchangeSize,
                lastExchangeSizeLeft, *message, *message_size);
    g_pSpdm->lastExchangeSize += *message_size;

    return LIBSPDM_STATUS_SUCCESS;
}

/*!
 * Static function libspdm uses as hook to RM<->GSP transport layer.
 * Sends SPDM request message to GSP, and stores received response to buffer.
 * Response buffer must be freed by corresponding _spdmReceiveMessageGsp().
 */
libspdm_return_t
_spdmSendMessageGsp
(
    void       *spdm_context,
    size_t      message_size,
    const void *message,
    uint64_t    timeout
)
{
    NvU8                    *pRequest    = NULL;
    NvU32                    requestSize = 0;
    NV_STATUS                nvStatus    = NV_OK;
    libspdm_return_t         spdmStatus  = LIBSPDM_STATUS_SUCCESS;
    SPDM_GSP_IO_CONTEXT     *pIOContext  = NULL;

    // Check libspdm parameters.
    if (message_size == 0 || message == NULL)
    {
        return LIBSPDM_STATUS_INVALID_PARAMETER;
    }

    if (g_pGpu == NULL || g_pSpdm == NULL || g_pSpdm->pDeviceIOContext == NULL)
    {
        return LIBSPDM_STATUS_INVALID_STATE_LOCAL;
    }

    pIOContext = (SPDM_GSP_IO_CONTEXT *)g_pSpdm->pDeviceIOContext;

    // Initialize space for SPDM request and response buffers.
    requestSize              = message_size;
    pRequest                 = (NvU8 *)portMemAllocNonPaged(requestSize);
    pIOContext->responseSize = LIBSPDM_MAX_MESSAGE_BUFFER_SIZE;
    pIOContext->pResponse    = (NvU8 *)portMemAllocNonPaged(pIOContext->responseSize);

    if (pRequest == NULL || pIOContext->pResponse == NULL)
    {
        spdmStatus = LIBSPDM_STATUS_BUFFER_FULL;
        goto ErrorExit;
    }

    portMemSet(pRequest, 0, requestSize);
    portMemSet(pIOContext->pResponse, 0, pIOContext->responseSize);
    portMemCopy(pRequest, requestSize, message, message_size);

    nvStatus = spdmMessageProcess_HAL(g_pGpu, g_pSpdm,
                                      pRequest, requestSize,
                                      pIOContext->pResponse,
                                      &pIOContext->responseSize);

ErrorExit:

    portMemFree(pRequest);
    if (spdmStatus != LIBSPDM_STATUS_SUCCESS || nvStatus != NV_OK)
    {
        portMemFree(pIOContext->pResponse);
        pIOContext->pResponse    = NULL;
        pIOContext->responseSize = 0;
        spdmStatus               = LIBSPDM_STATUS_SEND_FAIL;
    }

    return spdmStatus;
}

/*!
 * Static function libspdm uses as hook to RM<->GSP transport layer.
 * Copies stored response message back to libspdm buffer. Cannot be retried,
 * as must free response message regardless of success or failure.
 */
libspdm_return_t
_spdmReceiveMessageGsp
(
    void      *spdm_context,
    size_t    *message_size,
    void     **message,
    uint64_t   timeout
)
{
    libspdm_return_t     spdmStatus = LIBSPDM_STATUS_SUCCESS;
    SPDM_GSP_IO_CONTEXT *pIOContext = NULL;

    // Check libspdm parameters.
    if (message_size == NULL || message == NULL || *message == NULL)
    {
        return LIBSPDM_STATUS_INVALID_PARAMETER;
    }

    if (g_pGpu == NULL || g_pSpdm == NULL || g_pSpdm->pDeviceIOContext == NULL)
    {
        return LIBSPDM_STATUS_INVALID_STATE_LOCAL;
    }

    // Basic validation to ensure we have a real response.
    pIOContext = (SPDM_GSP_IO_CONTEXT *)g_pSpdm->pDeviceIOContext;
    if (pIOContext->pResponse == NULL || pIOContext->responseSize == 0)
    {
        spdmStatus = LIBSPDM_STATUS_RECEIVE_FAIL;
        goto ErrorExit;
    }

    if (pIOContext->responseSize > *message_size)
    {
        spdmStatus = LIBSPDM_STATUS_BUFFER_TOO_SMALL;
        goto ErrorExit;
    }

    portMemCopy(*message, *message_size, pIOContext->pResponse, pIOContext->responseSize);
    *message_size = pIOContext->responseSize;

ErrorExit:

    // Invalidate response message, regardless of success or failure.
    portMemFree(pIOContext->pResponse);
    pIOContext->pResponse    = NULL;
    pIOContext->responseSize = 0;

    return spdmStatus;
}

/*!
* Static function that sends the opaque data from RM to GSP.
*/
NV_STATUS
_spdmSendInitRmDataCommand
(
    OBJGPU *pGpu,
    Spdm   *pSpdm
)
{
    NV_STATUS                                  status     = NV_OK;
    RMTIMEOUT                                  timeout;
    NV2080_CTRL_INTERNAL_SPDM_PARTITION_PARAMS params;

    if (pGpu == NULL || pSpdm == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    portMemSet(&params, 0, sizeof(params));
    params.cmd.cmdType = RM_GSP_SPDM_CMD_ID_CC_INIT_RM_DATA;

    gpuSetTimeout(pGpu, GPU_TIMEOUT_DEFAULT, &timeout, 0);

    status = spdmCtrlSpdmPartition(pGpu, &params);
    if (status != NV_OK)
    {
        return NV_ERR_FLCN_ERROR;
    }

    // Perform basic validation of header returned, ensuring it matches expected values.
    status = _spdmErrorCheck(&params.msg, pSpdm->guestId, pSpdm->endpointId, RM_GSP_SPDM_CMD_ID_CC_INIT_RM_DATA);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "SPDM: _spdmErrorCheck() failed in INIT_RM_DATA command \n");
        DBG_BREAKPOINT();
        return status;
    }

    return NV_OK;
}

/* ------------------------ Public Functions ------------------------------- */
/*!
 * On Hopper, we use GSP as SPDM Responder. To initialize, we must allocate
 * a surface for SPDM communication, send initialization command to GSP-SPDM
 * partition, and register all transport-layer functionality with library.
 */
NV_STATUS
spdmDeviceInit_GH100
(
    OBJGPU *pGpu,
    Spdm   *pSpdm
)
{
    NV_STATUS                                  status         = NV_OK;
    NV_STATUS                                  descHdrStatus  = NV_OK;
    SPDM_GSP_IO_CONTEXT                       *pIOContext     = NULL;
    RmPhysAddr                                 physAddr       = 0;
    NvU64                                      memDescFlags   = MEMDESC_FLAGS_NONE;
    NV2080_CTRL_INTERNAL_SPDM_PARTITION_PARAMS params;
    RMTIMEOUT                                  timeout;
    TRANSFER_SURFACE                           surf           = {0};
    MemoryManager                             *pMemoryManager = NULL;

    if (pGpu == NULL || pSpdm == NULL || pSpdm->pLibspdmContext == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    g_pGpu  = pGpu;
    g_pSpdm = pSpdm;

    // Register transport layer functionality with library.
    libspdm_register_transport_layer_func(pSpdm->pLibspdmContext,
                                          _spdmEncodeMessageGsp,
                                          _spdmDecodeMessageGsp,
                                          _spdmGetTransportHeaderSizeGsp);

    libspdm_register_device_io_func(pSpdm->pLibspdmContext,
                                 _spdmSendMessageGsp,
                                 _spdmReceiveMessageGsp);


    pSpdm->pDeviceIOContext = portMemAllocNonPaged(sizeof(SPDM_GSP_IO_CONTEXT));
    if (pSpdm->pDeviceIOContext == NULL)
    {
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    pIOContext = (SPDM_GSP_IO_CONTEXT *)pSpdm->pDeviceIOContext;
    portMemSet(pIOContext, 0, sizeof(SPDM_GSP_IO_CONTEXT));

    //
    // Set default values for IO context.
    // TODO CONFCOMP-1463: Generalize command structures.
    //
    pIOContext->surfaceSizeInByte  = NV_SPDM_RM_SURFACE_SIZE_IN_BYTE;
    pIOContext->descHdrSizeInByte  = NV_SPDM_DESC_HEADER_SIZE_IN_BYTE;
    pIOContext->rmBufferSizeInByte = NV_RM_BUFFER_SIZE_IN_BYTE;
    pIOContext->alignSize          = SPDM_MESSAGE_BUFFER_ADDRESS_ALIGNMENT;
    pIOContext->addrSpace          = NV_SPDM_DMA_ADDR_SPACE_DEFAULT;
    pIOContext->regionId           = NV_SPDM_DMA_REGION_ID_DEFAULT;
    pIOContext->pResponse          = NULL;
    pIOContext->responseSize       = 0;

    // Be sure to allocate in unprotected memory, so GPU can read.
    memDescFlags = MEMDESC_FLAGS_ALLOC_IN_UNPROTECTED_MEMORY;

    // Create memory descriptor.
    status = memdescCreate(&pIOContext->pMemDesc, pGpu,
                           pIOContext->surfaceSizeInByte,
                           pIOContext->alignSize,
                           NV_TRUE, pIOContext->addrSpace,
                           NV_MEMORY_UNCACHED,
                           memDescFlags);
    if (status != NV_OK || pIOContext->pMemDesc == NULL)
    {
        portMemFree(pIOContext);
        pSpdm->pDeviceIOContext = NULL;
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    memdescTagAlloc(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_82, 
                    pIOContext->pMemDesc);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "SPDM: Cannot allocate SPDM surface!\n");
        memdescDestroy(pIOContext->pMemDesc);
        portMemFree(pIOContext);
        pSpdm->pDeviceIOContext = NULL;
        return status;
    }

    // Physical mode with GSP.
    if ((pIOContext->addrSpace == ADDR_SYSMEM) || (pIOContext->addrSpace == ADDR_FBMEM))
    {
        physAddr = memdescGetPhysAddr(pIOContext->pMemDesc, AT_GPU, 0);
    }
    else
    {
        NV_PRINTF(LEVEL_ERROR, "SPDM: Message surface addr space is invalid: (0x%x)\n",
                  pIOContext->addrSpace);
        status = NV_ERR_INSUFFICIENT_RESOURCES;
        goto ErrorExit;
    }

    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY)

    pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);

    surf.pMemDesc = pIOContext->pMemDesc;
    surf.offset = 0;

    // Scrub surface
    NV_ASSERT_OK_OR_GOTO(status,
        memmgrMemSet(pMemoryManager, &surf, 0, pIOContext->surfaceSizeInByte,
                     TRANSFER_FLAGS_NONE),
        ErrorExit);

    pIOContext->dmaAddr = (NvU64)physAddr;

    // Prepare GSP-CMD and send to GSP-SPDM partition.
    portMemSet(&params, 0, sizeof(params));

    params.cmd.cmdType = RM_GSP_SPDM_CMD_ID_CC_INIT;

    RM_GSP_SPDM_CMD_CC_INIT *pCcInit    = &params.cmd.ccInit;
    RM_GSP_SPDM_CC_INIT_CTX *pCcInitCtx = &pCcInit->ccInitCtx;

    // Ucode is responsible for setting DMA index.
    pCcInitCtx->guestId            = pSpdm->guestId;
    pCcInitCtx->rmBufferSizeInByte = NV_RM_BUFFER_SIZE_IN_BYTE;
    NvU64_ALIGN32_PACK(&pCcInitCtx->dmaAddr, &pIOContext->dmaAddr);

    gpuSetTimeout(pGpu, GPU_TIMEOUT_DEFAULT, &timeout, 0);

    status = spdmCtrlSpdmPartition(pGpu, &params);

    //
    // If successful, copy status received from GSP-SPDM. In case of failure, we will
    // unmap memory first before breaking to error handling routine.
    //
    if (status == NV_OK)
    {
        descHdrStatus = _spdmErrorCheck(&params.msg, pSpdm->guestId, pSpdm->endpointId, RM_GSP_SPDM_CMD_ID_CC_INIT);
    }

    // Now we check for command success.
    if (status != NV_OK || descHdrStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "SPDM: gspCommandPostBlocking() failed in INIT command, status = 0x%x, descHdrStatus = 0x%x \n",
                  status, descHdrStatus);
        DBG_BREAKPOINT();

        status = (status == NV_OK ? descHdrStatus : status);
        goto ErrorExit;
    }

    // If successful, save returned endpoint ID to context.
    pSpdm->endpointId = params.msg.endpointId;

    // Sending Rm data for opaque data to GSP.
    status = _spdmSendInitRmDataCommand(pGpu, pSpdm);

    SLI_LOOP_END

ErrorExit:
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "SPDM: INIT command failed: status 0x%x\n", status);
        _spdmReleaseSurfaceGsp(pSpdm);
    }

    return status;
}


/*!
 * To deinitialize the GSP SPDM Responder, we need to release the surface for
 * SPDM communcation, and send deinitialization command to GSP-SPDM.
 */
NV_STATUS
spdmDeviceDeinit_GH100
(
    OBJGPU *pGpu,
    Spdm   *pSpdm,
    NvBool  bForceClear
)
{
    NV_STATUS                                  status         = NV_OK;
    NV_STATUS                                  descHdrStatus  = NV_OK;
    RM_GSP_SPDM_CMD_CC_DEINIT                 *pCcDeinit      = NULL;
    RM_GSP_SPDM_CC_DEINIT_CTX                 *pCcDeinitCtx   = NULL;
    NV2080_CTRL_INTERNAL_SPDM_PARTITION_PARAMS params;
    RMTIMEOUT                                  timeout;

    if (pGpu == NULL || pSpdm == NULL || pSpdm->pDeviceIOContext == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY)

    // Prepare GSP-CMD and send to GSP-SPDM partition.
    portMemSet(&params, 0, sizeof(params));

    params.cmd.cmdType = RM_GSP_SPDM_CMD_ID_CC_DEINIT;

    // TODO CONFCOMP-1463: Generalize command structures.
    pCcDeinit                = &params.cmd.ccDeinit;
    pCcDeinitCtx             = &pCcDeinit->ccDeinitCtx;
    pCcDeinitCtx->guestId    = pSpdm->guestId;
    pCcDeinitCtx->endpointId = pSpdm->endpointId;

    if (bForceClear)
    {
        pCcDeinitCtx->flags |= DEINIT_FLAGS_FORCE_CLEAR;
    }

    gpuSetTimeout(pGpu, GPU_TIMEOUT_DEFAULT, &timeout, 0);

    status = spdmCtrlSpdmPartition(pGpu, &params);

    //
    // If successful, copy status received from GSP-SPDM. In case of failure, we will
    // unmap memory first before breaking to error handling routine.
    //
    if (status == NV_OK)
    {
        descHdrStatus = _spdmErrorCheck(&params.msg, pSpdm->guestId, pSpdm->endpointId, RM_GSP_SPDM_CMD_ID_CC_DEINIT);
    }

    SLI_LOOP_END

    if (status != NV_OK || descHdrStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "SPDM: DEINIT command failed: status = 0x%x, descHdrStatus = 0x%x\n",
                  status, descHdrStatus);
        DBG_BREAKPOINT();

        status = (status == NV_OK ? descHdrStatus : status);
        goto ErrorExit;
    }

    // Only release surface on success, as we may want to retry deinit on failure.
    _spdmReleaseSurfaceGsp(pSpdm);

ErrorExit:

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "SPDM: DEINIT command failed: status 0x%x\n", status);
    }

    return status;
}

NV_STATUS
spdmMessageProcess_GH100
(
    OBJGPU *pGpu,
    Spdm   *pSpdm,
    NvU8   *pRequest,
    NvU32   requestSize,
    NvU8   *pResponse,
    NvU32  *pResponseSize
)
{
    NV_STATUS                                  status                = NV_OK;
    NV_STATUS                                  descHdrStatus         = NV_OK;
    NvU8                                      *pMapMem               = NULL;
    NvU32                                      transportResponseSize = 0;
    SPDM_GSP_IO_CONTEXT                       *pIOContext            = NULL;
    RM_GSP_SPDM_CMD_CC_CTRL                   *pCcCtrl               = NULL;
    RM_GSP_SPDM_CC_CTRL_CTX                   *pCcCtrlCtx            = NULL;
    NV2080_CTRL_INTERNAL_SPDM_PARTITION_PARAMS params;
    RMTIMEOUT                                  timeout;
    MemoryManager                             *pMemoryManager        = NULL;
    TRANSFER_SURFACE                           surf                  = {0};
    NvBool                                     bFreeShadowBuf        = NV_FALSE;

    if (pGpu == NULL || pSpdm == NULL || pSpdm->pDeviceIOContext == NULL ||
        pRequest == NULL || pResponse == NULL || pResponseSize == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    pIOContext = (SPDM_GSP_IO_CONTEXT *)pSpdm->pDeviceIOContext;
    if (requestSize > pIOContext->rmBufferSizeInByte)
    {
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    //
    // Map memory for CPU access to copy SPDM message - ensure we unmap
    // before returning for any reason.
    //
    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY)

    pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);

    pMapMem = memmgrMemDescBeginTransfer(pMemoryManager, pIOContext->pMemDesc,
                                         TRANSFER_FLAGS_SHADOW_ALLOC);
    if (pMapMem == NULL)
    {
        status = NV_ERR_INSUFFICIENT_RESOURCES;
        goto ErrorExit;
    }

    // Copy entire SPDM message to shared memory.
    portMemCopy(pMapMem, requestSize, pRequest, requestSize);

    memmgrMemDescEndTransfer(pMemoryManager, pIOContext->pMemDesc,
                             TRANSFER_FLAGS_SHADOW_ALLOC);

    // Prepare GSP-CMD and send to GSP-SPDM partition
    portMemSet(&params, 0, sizeof(params));
    params.cmd.cmdType      = RM_GSP_SPDM_CMD_ID_CC_CTRL;
    pCcCtrl                 = &params.cmd.ccCtrl;
    pCcCtrlCtx              = &pCcCtrl->ccCtrlCtx;
    pCcCtrlCtx->version     = NV_SPDM_DESC_HEADER_VERSION_CURRENT;
    pCcCtrlCtx->guestId     = pSpdm->guestId;
    pCcCtrlCtx->endpointId  = pSpdm->endpointId;

    gpuSetTimeout(pGpu, GPU_TIMEOUT_DEFAULT, &timeout, 0);

    status = spdmCtrlSpdmPartition(pGpu, &params);

    // Retrieve descriptor header and response message, checking header to ensure valid response.
    if (status == NV_OK)
    {
        surf.pMemDesc = pIOContext->pMemDesc;
        surf.offset   = 0;

        pMapMem = portMemAllocNonPaged(pIOContext->surfaceSizeInByte);

        if (pMapMem == NULL)
        {
            status = NV_ERR_INSUFFICIENT_RESOURCES;
            goto ErrorExit;
        }

        bFreeShadowBuf = NV_TRUE;

        NV_ASSERT_OK_OR_GOTO(status,
            memmgrMemRead(pMemoryManager, &surf, pMapMem,
                          pIOContext->surfaceSizeInByte,
                          TRANSFER_FLAGS_NONE),
            ErrorExit);

        descHdrStatus = _spdmErrorCheck(&params.msg, pSpdm->guestId, pSpdm->endpointId, RM_GSP_SPDM_CMD_ID_CC_CTRL);
    }

    if (status != NV_OK || descHdrStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "SPDM: Process message failed: status = 0x%x, descHdrStatus = 0x%x\n",
                  status, descHdrStatus);
        DBG_BREAKPOINT();

        status = (status == NV_OK ? descHdrStatus : status);
        goto ErrorExit;
    }

    transportResponseSize = (((NV_SPDM_DESC_HEADER *)pMapMem)->msgSizeByte + NV_SPDM_DESC_HEADER_SIZE_IN_BYTE);

    if (transportResponseSize > *pResponseSize)
    {
        status =  NV_ERR_INSUFFICIENT_RESOURCES;
        NV_PRINTF(LEVEL_ERROR, "SPDM: Error: transportResponseSize = 0x%x, responseSize = 0x%x\n",
                  transportResponseSize, *pResponseSize);
        DBG_BREAKPOINT();
        goto ErrorExit;
    }

    portMemCopy(pResponse, *pResponseSize, pMapMem, transportResponseSize);
    *pResponseSize = transportResponseSize;

    SLI_LOOP_END

ErrorExit:
    if (bFreeShadowBuf)
    {
        portMemFree(pMapMem);
        bFreeShadowBuf = NV_FALSE;
    }
    return status;
}

NV_STATUS
spdmDeviceSecuredSessionSupported_GH100
(
    OBJGPU *pGpu,
    Spdm   *pSpdm
)
{

    return NV_OK;
}

NV_STATUS
spdmCheckConnection_GH100
(
    OBJGPU *pGpu,
    Spdm   *pSpdm
)
{
    void                       *pContext      = NULL;
    uint32_t                    expectedFlags = 0;
    uint32_t                    capabilitiesFlags = 0;
    libspdm_return_t            ret = LIBSPDM_STATUS_SUCCESS;
    libspdm_data_parameter_t    dataParam;
    libspdm_connection_state_t  connectionState;
    uint8_t                     ctExponent;
    size_t                      dataSize;
    NvU32                       i;
    NvU32                       algoCheckCount;
    NvU32                       actualAlgo;
    PSPDM_ALGO_CHECK_ENTRY      pCheckEntry;

    if (pGpu == NULL || pSpdm == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Ensure we have at least negotiated the parameters of the SPDM connection.
    pContext = (void *)pSpdm->pLibspdmContext;

    if (pContext == NULL)
    {
        return NV_ERR_NOT_READY;
    }

    dataParam.location = LIBSPDM_DATA_LOCATION_CONNECTION;
    dataSize = sizeof(connectionState);
    ret = libspdm_get_data(pContext, LIBSPDM_DATA_CONNECTION_STATE,
                           &dataParam, &connectionState, &dataSize);

    if (ret != LIBSPDM_STATUS_SUCCESS || connectionState < LIBSPDM_CONNECTION_STATE_NEGOTIATED)
    {
        return NV_ERR_NOT_READY;
    }

    // Check version matches expected.
    if (libspdm_get_connection_version(pContext) != SPDM_MESSAGE_VERSION_11)
    {
        return NV_ERR_INVALID_STATE;
    }

    dataSize = sizeof(ctExponent);
    ret = libspdm_get_data(pContext, LIBSPDM_DATA_CAPABILITY_CT_EXPONENT,
                           &dataParam, &ctExponent, &dataSize);

    if (ret != LIBSPDM_STATUS_SUCCESS || ctExponent != SPDM_CAPABILITIES_CT_EXPONENT_MAX)
    {
        NV_PRINTF(LEVEL_ERROR, "SPDM: Invalid Responder CT exponent.\n");
        return NV_ERR_INVALID_STATE;
    }

    // Check all capabilities match expected.
    expectedFlags = SPDM_CAPABILITIES_FLAGS_GH100;

    dataSize = sizeof(capabilitiesFlags);
    ret = libspdm_get_data(pContext, LIBSPDM_DATA_CAPABILITY_FLAGS,
                           &dataParam, &capabilitiesFlags, &dataSize);

    if (ret != LIBSPDM_STATUS_SUCCESS || capabilitiesFlags != expectedFlags)
    {
        NV_PRINTF(LEVEL_ERROR, "SPDM: Invalid Responder capabilities.\n");
        return NV_ERR_INVALID_STATE;
    }

    // Check all crypto algorithms match expected.
    algoCheckCount = sizeof(g_SpdmAlgoCheckTable_GH100)/sizeof(SPDM_ALGO_CHECK_ENTRY);

    for (i = 0; i < algoCheckCount; i++)
    {
        pCheckEntry = &g_SpdmAlgoCheckTable_GH100[i];

        actualAlgo = 0;
        dataSize   = sizeof(actualAlgo);
        ret        = libspdm_get_data(pContext, pCheckEntry->dataType,
                                      &dataParam, &actualAlgo, &dataSize);

        if (ret != LIBSPDM_STATUS_SUCCESS || actualAlgo != pCheckEntry->expectedAlgo)
        {
            NV_PRINTF(LEVEL_ERROR, "SPDM: Invalid crypto algorithms selected.\n");
            NV_PRINTF(LEVEL_ERROR, "SPDM: AlgoCheckCount 0x%0x, i is 0x%0x, status is 0x%0x.\n", (NvU32)algoCheckCount, (NvU32)i, (NvU32)ret);
            NV_PRINTF(LEVEL_ERROR, "SPDM: Expected algo 0x%0x, actual algo 0x%0x\n", (NvU32)pCheckEntry->expectedAlgo, (NvU32)actualAlgo);
            return NV_ERR_INVALID_STATE;
        }
    }

    return NV_OK;
}

NV_STATUS
spdmGetAttestationReport_GH100
(
    OBJGPU *pGpu,
    Spdm   *pSpdm,
    NvU8   *pNonce,
    void   *pAttestationReport,
    NvU32  *pAttestationReportSize,
    NvBool *pbIsCecAttestationReportPresent,
    void   *pCecAttestationReport,
    NvU32  *pCecAttestationReportSize
)
{
    NV_STATUS status             = NV_OK;
    uint8_t   numBlocks          = 0;
    uint32_t  measurementSize    = 0;
    void     *pMeasurementBuffer = NULL;

    if (pGpu == NULL || pSpdm == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Ensure we have a valid session, and have retrieved the certificates.
    if (pSpdm->pLibspdmContext == NULL || pSpdm->sessionId == INVALID_SESSION_ID)
    {
        return NV_ERR_NOT_READY;
    }

    // Retrieve Attestation Report, if requested.
    if (pAttestationReport != NULL && pAttestationReportSize != NULL)
    {
        // Guarantees the latest transcript will fit inside the Attestation Report buffer.
        if (*pAttestationReportSize < SPDM_MAX_EXCHANGE_BUFFER_SIZE)
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        numBlocks       = LIBSPDM_MAX_MEASUREMENT_BLOCK_COUNT;

        //
        // Use Attestation Report buffer temporarily to store measurements, we will replace
        // with the full Attestation Report from message transcripts.
        //
        pMeasurementBuffer = pAttestationReport;
        measurementSize    = *pAttestationReportSize;

        // Request the Attestation Report using the provided nonce, signed by the AK cert.
        CHECK_SPDM_STATUS(libspdm_get_measurement_ex(pSpdm->pLibspdmContext, &pSpdm->sessionId,
            SPDM_GET_MEASUREMENTS_REQUEST_ATTRIBUTES_GENERATE_SIGNATURE,
            SPDM_GET_MEASUREMENTS_REQUEST_MEASUREMENT_OPERATION_ALL_MEASUREMENTS,
            SPDM_CERT_DEFAULT_SLOT_ID, NULL, &numBlocks,
            &measurementSize, pMeasurementBuffer, pNonce, NULL, NULL));

        if (!nvspdm_check_and_clear_libspdm_assert())
        {
            // libspdm detects assert/error in GET_MEASUREMENT process, need to return error.
            NV_PRINTF(LEVEL_ERROR, "SPDM: spdmCheckAndClearLibspdmAssert() failed \n");
            DBG_BREAKPOINT();

            return NV_ERR_INVALID_STATE;
        }

        //
        // Last exchange buffer will hold Attestation Report, which is comprised of
        // the GET_MEASUREMENTS request concatenated with the MEASUREMENTS response.
        // By definition, must be larger than the measurements written above.
        //
        portMemCopy(pAttestationReport, *pAttestationReportSize,
                    pSpdm->pLastExchange, pSpdm->lastExchangeSize);
        *pAttestationReportSize = pSpdm->lastExchangeSize;
    }

    // Retrieve CEC Attestation Report, if requested.
    if (pbIsCecAttestationReportPresent != NULL)
    {
        *pbIsCecAttestationReportPresent = NV_FALSE;
    }

ErrorExit:

    return status;
}
