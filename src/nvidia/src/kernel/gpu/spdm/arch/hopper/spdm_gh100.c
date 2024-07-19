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

/*!
 * Provides the implementation for all GH100 specific SPDM HALs
 * interfaces.
 */

/* ------------------------ Includes --------------------------------------- */
#include "nvRmReg.h"
#include "gpu/spdm/spdm.h"
#include "spdm/rmspdmtransport.h"
#include "spdm/rmspdmvendordef.h"
#include "gpu/timer/objtmr.h"
#include "gpu/gsp/kernel_gsp.h"
#include "gpu/bus/kern_bus.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "gpu/spdm/libspdm_includes.h"
#include "rmapi/client_resource.h"
#include "ctrl/ctrl2080/ctrl2080spdm.h"
#include "flcnretval.h"
#include "gpu/conf_compute/conf_compute.h"
#include "platform/sli/sli.h"
#include "nvspdm_rmconfig.h"
#include "published/hopper/gh100/dev_falcon_v4.h"
#include "nvmisc.h"
#include "gpu/conf_compute/conf_compute.h"

/* ------------------------ Macros ----------------------------------------- */
//
// List expected capabilties to receive from Responder.
// Regardless of whether Requester is configured to support these,
// we only expect Responder to provide these capabilities.
//

//
// TODO: SPDM_CAPABILITIES_FLAGS_GH100 and g_SpdmAlgoCheckTable_GH100 is expected capabilities flags
//       and attributions what GH100 receive from responder. Currently, we have only 1 responder
//       and return fixed capabilities flags and attributions.
//       If we want to support different return capabilitis and attributions afterwards, we need
//       to refactor spdmCheckConnection_GH100().
//
#define SPDM_CAPABILITIES_FLAGS_GH100 \
        SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_CERT_CAP       | \
        SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_MEAS_CAP_SIG   | \
        SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_MEAS_FRESH_CAP | \
        SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_ENCRYPT_CAP    | \
        SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_MAC_CAP        | \
        SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_KEY_EX_CAP     | \
        SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_KEY_UPD_CAP    | \
        SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_ENCAP_CAP      | \
        SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_MUT_AUTH_CAP   | \
        SPDM_GET_CAPABILITIES_RESPONSE_FLAGS_HBEAT_CAP;

/* ------------------------ Static Variables ------------------------------- */
static SPDM_ALGO_CHECK_ENTRY g_SpdmAlgoCheckTable_GH100[] =
{
    { LIBSPDM_DATA_MEASUREMENT_SPEC,       SPDM_MEASUREMENT_SPECIFICATION_DMTF },
    { LIBSPDM_DATA_MEASUREMENT_HASH_ALGO,  SPDM_ALGORITHMS_MEASUREMENT_HASH_ALGO_TPM_ALG_SHA_384 },
    { LIBSPDM_DATA_BASE_ASYM_ALGO,         SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_ECDSA_ECC_NIST_P384 },
    { LIBSPDM_DATA_BASE_HASH_ALGO,         SPDM_ALGORITHMS_BASE_HASH_ALGO_TPM_ALG_SHA_384 },
    { LIBSPDM_DATA_DHE_NAME_GROUP,         SPDM_ALGORITHMS_DHE_NAMED_GROUP_SECP_384_R1 },
    { LIBSPDM_DATA_AEAD_CIPHER_SUITE,      SPDM_ALGORITHMS_AEAD_CIPHER_SUITE_AES_256_GCM },
    { LIBSPDM_DATA_KEY_SCHEDULE,           SPDM_ALGORITHMS_KEY_SCHEDULE_HMAC_HASH },
    { LIBSPDM_DATA_OTHER_PARAMS_SUPPORT,   0 },
#if LIBSPDM_ENABLE_CAPABILITY_MUT_AUTH_CAP
    { LIBSPDM_DATA_REQ_BASE_ASYM_ALG,      SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSAPSS_3072 }
#endif
};

/* ------------------------ Static Function Prototypes --------------------- */
static void _spdmSendHeartbeat(NvU32 gpuInstance, void *pArgs);
static NV_STATUS _spdmTriggerHeartbeat(OBJGPU *pGpu, OBJTMR *pTmr, TMR_EVENT *pTmrEvent);

//
// Static transport layer functions which we pass to libspdm as function pointers.
// The libspdm library will then use these functions to send and receive SPDM messages.
// Function parameters and types must match those expected by libspdm.
//
static spdm_version_number_t _spdmGetSecuredMessageVersionGsp(spdm_version_number_t secured_message_version);

static uint8_t _spdmGetSecuredMessageSequenceNumberGsp(uint64_t  sequence_number,
                                                       uint8_t  *sequence_number_buffer);

static uint32_t _spdmGetSecuredMessageMaxRandomNumberCountGsp(void);

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

/* ------------------------ Static Functions ------------------------------- */
//
// Hardcoding check for libspdm secured message callbacks version.
// If libspdm bumps this in a version update, we must update as well.
//
ct_assert(LIBSPDM_SECURED_MESSAGE_CALLBACKS_VERSION == 2);

/*!
  * Callback to convert secured_message_version to DSP0277 version.
  * In our case, secured_message_version will always be DSP0277 version.
  */
spdm_version_number_t
_spdmGetSecuredMessageVersionGsp
(
    spdm_version_number_t secured_message_version
)
{
    return secured_message_version;
}

/*!
 * @brief Work function scheduled by heartbeat callback in order to actually
 *        send the heartbeat to GSP-RM. Checks to ensure whether KEY_UPDATE
 *        is required before sending heartbeat.
 *
 * @param[in] gpuInstance : Instance number of the specific GPU
 * @param[in] pArgs       : Opaque pointer to the SPDM object
 */
static void
_spdmSendHeartbeat
(
    NvU32  gpuInstance,
    void  *pArgs
)
{
    OBJGPU    *pGpu   = gpumgrGetGpu(gpuInstance);
    OBJTMR    *pTmr   = NULL;
    Spdm      *pSpdm  = NULL;
    NV_STATUS  status = NV_OK;

    if (pGpu == NULL || pArgs == NULL)
    {
        status = NV_ERR_INVALID_ARGUMENT;
        goto ErrorExit;
    }

    pTmr  = GPU_GET_TIMER(pGpu);
    pSpdm = (Spdm *)pArgs;
    if (pSpdm == NULL || pSpdm->pLibspdmContext == NULL ||
        pSpdm->sessionId == INVALID_SESSION_ID)
    {
        status = NV_ERR_NOT_READY;
        goto ErrorExit;
    }

    //
    // Check to see if KEY_UPDATE is required before using. As timer resets on
    // every message sent, sending KEY_UPDATE shouldn't incur any timeout risk.
    //
    status = spdmCheckAndExecuteKeyUpdate(pGpu, pSpdm, NV_KEY_UPDATE_TRIGGER_ID_HEARTBEAT);
    if (status != NV_OK)
    {
        goto ErrorExit;
    }

    CHECK_SPDM_STATUS(libspdm_heartbeat(pSpdm->pLibspdmContext, pSpdm->sessionId));

    // Reschedule heartbeat only if successful
    status = tmrEventScheduleRelSec(pTmr, pSpdm->pHeartbeatEvent, pSpdm->heartbeatPeriodSec);

ErrorExit:
    if (status != NV_OK && pGpu != NULL)
    {
        ConfidentialCompute *pConfCompute = GPU_GET_CONF_COMPUTE(pGpu);
        //
        // Set GPU Ready State to false. This will destroy the SPDM session as well.
        // Ideally we don't have dependency on Confidential Compute object here,
        // but that's the best we can do without unnecessary code duplication.
        //
        confComputeSetErrorState(pGpu, pConfCompute);
    }
}

/*!
 * @brief Callback function scheduled when HEARTBEAT is enabled.
 *        Function will queue a work item to actually send the heartbeat,
 *        since we cannot do so in the callback interrupt context.
 *        Upon successful queueing of work item, schedules another heartbeat callback.
 *
 * @param[in] pGpu      : OBJGPU pointer
 * @param[in] pTmr      : OBJTMR pointer
 * @param[in] pTmrEvent : Pointer to the specific heartbeat timer event.
 *
 * @return NV_STATUS representing success or relevant failure.
 */
static NV_STATUS
_spdmTriggerHeartbeat
(
    OBJGPU     *pGpu,
    OBJTMR     *pTmr,
    TMR_EVENT *pTmrEvent
)
{
    NV_STATUS  status = NV_OK;

    if (pGpu == NULL || pTmr == NULL || pTmrEvent == NULL)
    {
        status = NV_ERR_INVALID_ARGUMENT;
        goto ErrorExit;
    }

    // Some RM APIs call SPDM, ensure we do not conflict with them
    status = osQueueWorkItemWithFlags(pGpu, _spdmSendHeartbeat, pTmrEvent->pUserData,
                                        OS_QUEUE_WORKITEM_FLAGS_DONT_FREE_PARAMS |
                                        OS_QUEUE_WORKITEM_FLAGS_LOCK_API_RW      |
                                        OS_QUEUE_WORKITEM_FLAGS_LOCK_GPUS);

    ErrorExit:
    if (status != NV_OK && pGpu != NULL)
    {
        ConfidentialCompute *pConfCompute = GPU_GET_CONF_COMPUTE(pGpu);
        //
        // Set GPU Ready State to false. This will destroy the SPDM session as well.
        // Ideally we don't have dependency on Confidential Compute object here,
        // but that's the best we can do without unnecessary code duplication.
        //
        confComputeSetErrorState(pGpu, pConfCompute);
    }
    return status;
}

/*!
 * Callback to fill out sequence number in expected format.
 * The sequence number is for secured message format only and defined in DMTF DSP0277.
 * Currently, requester(RM) and responder(GSP-SPDM) doesn't support sequence number.
 */
uint8_t
_spdmGetSecuredMessageSequenceNumberGsp
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
_spdmGetSecuredMessageMaxRandomNumberCountGsp
(
    void
)
{
    return NV_SPDM_MAX_RANDOM_MSG_BYTES;
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
    Spdm                                *pSpdm                  = NULL;
    size_t                               dataSize               = sizeof(void *);

    // Check libspdm parameters.
    if (spdm_context == NULL || message == NULL || message_size == 0 ||
        transport_message == NULL || *transport_message == NULL ||
        transport_message_size == NULL)
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

    status = libspdm_get_data(spdm_context, LIBSPDM_DATA_APP_CONTEXT_DATA,
                              NULL, (void *)&pSpdm, &dataSize);

    if (status != LIBSPDM_STATUS_SUCCESS)
    {
        NV_PRINTF(LEVEL_ERROR, ", spdmStatus != LIBSPDM_STATUS_SUCCESS \n ");
        return status;
    }

    if (pSpdm == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, " pSpdm == NULL, SPDM context probably corrupted !! \n ");
        return LIBSPDM_STATUS_INVALID_STATE_LOCAL;
    }

    // Initialize descriptor header.
    pNvSpdmDescHdr = (NV_SPDM_DESC_HEADER *)*transport_message;
    portMemSet(pNvSpdmDescHdr, 0, sizeof(NV_SPDM_DESC_HEADER));
    pNvSpdmDescHdr->version = NV_SPDM_DESC_HEADER_VERSION_CURRENT;

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
        securedMessageInfo.version                     = LIBSPDM_SECURED_MESSAGE_CALLBACKS_VERSION;
        securedMessageInfo.get_sequence_number         = _spdmGetSecuredMessageSequenceNumberGsp;
        securedMessageInfo.get_max_random_number_count = _spdmGetSecuredMessageMaxRandomNumberCountGsp;
        securedMessageInfo.get_secured_spdm_version    = _spdmGetSecuredMessageVersionGsp;

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
    if (*transport_message_size > pSpdm->payloadBufferSize)
    {
        return LIBSPDM_STATUS_BUFFER_TOO_SMALL;
    }

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
    spdm_secured_message_a_data_header1_t *pSpdmSecuredMsgHdr     = NULL;
    Spdm                                  *pSpdm                  = NULL;
    size_t                                 dataSize               = sizeof(void *);

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

    status = libspdm_get_data(spdm_context, LIBSPDM_DATA_APP_CONTEXT_DATA,
                              NULL, (void *)&pSpdm, &dataSize);

    if (status != LIBSPDM_STATUS_SUCCESS)
    {
        NV_PRINTF(LEVEL_ERROR, " spdmStatus != LIBSPDM_STATUS_SUCCESS \n ");
        return status;
    }

    if (pSpdm == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, " pSpdm == NULL, SPDM context probably corrupted !! \n ");
        return LIBSPDM_STATUS_INVALID_STATE_LOCAL;
    }

    // Retrieve NV-header from message, and perform basic validation.
    pNvSpdmDescHdr = (NV_SPDM_DESC_HEADER *)transport_message;
    if (transport_message_size < sizeof(NV_SPDM_DESC_HEADER) ||
        transport_message_size > pSpdm->payloadBufferSize)
    {
        return LIBSPDM_STATUS_INVALID_MSG_FIELD;
    }

    if (pNvSpdmDescHdr->version != NV_SPDM_DESC_HEADER_VERSION_CURRENT)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "SPDM: Version mismatch: [Check] version = 0x%x, [SpdmRet] version = 0x%x\n",
                  NV_SPDM_DESC_HEADER_VERSION_CURRENT, pNvSpdmDescHdr->version);
        DBG_BREAKPOINT();
        return LIBSPDM_STATUS_INVALID_MSG_FIELD;
    }

    payloadSize = sizeof(NV_SPDM_DESC_HEADER) + pNvSpdmDescHdr->msgSizeByte;

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
            securedMessageInfo.version                     = LIBSPDM_SECURED_MESSAGE_CALLBACKS_VERSION;
            securedMessageInfo.get_sequence_number         = _spdmGetSecuredMessageSequenceNumberGsp;
            securedMessageInfo.get_max_random_number_count = _spdmGetSecuredMessageMaxRandomNumberCountGsp;
            securedMessageInfo.get_secured_spdm_version    = _spdmGetSecuredMessageVersionGsp;

            // Decode and retrieve application payload from secured message.
            // We must copy the message to the scratch buffer.
            status = libspdm_decode_secured_message(pSecuredMessageContext,
                                                    **session_id, is_requester,
                                                    transport_message_size - sizeof(NV_SPDM_DESC_HEADER),
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
    NV_STATUS                   nvStatus   = NV_OK;
    libspdm_return_t            spdmStatus = LIBSPDM_STATUS_SUCCESS;
    Spdm                       *pSpdm      = NULL;
    OBJGPU                     *pGpu       = NULL;
    size_t                      dataSize   = sizeof(void *);

    // Check libspdm parameters.
    if (message_size == 0 || message == NULL)
    {
        return LIBSPDM_STATUS_INVALID_PARAMETER;
    }

    spdmStatus = libspdm_get_data(spdm_context, LIBSPDM_DATA_APP_CONTEXT_DATA,
                              NULL, (void *)&pSpdm, &dataSize);

    if (spdmStatus != LIBSPDM_STATUS_SUCCESS)
    {
        NV_PRINTF(LEVEL_ERROR,"  spdmStatus != LIBSPDM_STATUS_SUCCESS \n ");
        return spdmStatus;
    }

    if (pSpdm == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, " pSpdm == NULL, SPDM context probably corrupted !! \n ");
        return LIBSPDM_STATUS_INVALID_STATE_LOCAL;
    }

    pGpu = ENG_GET_GPU(pSpdm);

    if (pGpu == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, " pGpu == NULL, SPDM context probably corrupted !! \n ");
        return LIBSPDM_STATUS_INVALID_STATE_LOCAL;
    }

    // Ensure size is cleared to indicate no response pending in buffer yet
    pSpdm->pendingResponseSize = 0;

    if (pSpdm->transportBufferSize < message_size)
    {
        return LIBSPDM_STATUS_BUFFER_TOO_SMALL;
    }

    // Fill transport buffer with message and send
    pSpdm->pendingResponseSize = pSpdm->transportBufferSize;
    portMemCopy(pSpdm->pTransportBuffer, pSpdm->transportBufferSize, message, message_size);

    nvStatus = spdmMessageProcess_HAL(pGpu, pSpdm,
                                      pSpdm->pTransportBuffer, message_size,
                                      pSpdm->pTransportBuffer, &pSpdm->pendingResponseSize);
    if (nvStatus != NV_OK)
    {
        spdmStatus = LIBSPDM_STATUS_SEND_FAIL;
    }

    if (spdmStatus != LIBSPDM_STATUS_SUCCESS)
    {
        // If message failed, size is cleared to indicate no response pending
        pSpdm->pendingResponseSize = 0;
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
    libspdm_return_t   spdmStatus = LIBSPDM_STATUS_SUCCESS;
    Spdm              *pSpdm      = NULL;
    size_t             dataSize   = sizeof(void *);

    // Check libspdm parameters.
    if (message_size == NULL || message == NULL || *message == NULL)
    {
        return LIBSPDM_STATUS_INVALID_PARAMETER;
    }

    spdmStatus = libspdm_get_data(spdm_context, LIBSPDM_DATA_APP_CONTEXT_DATA,
                              NULL, (void *)&pSpdm, &dataSize);

    if (spdmStatus != LIBSPDM_STATUS_SUCCESS)
    {
        NV_PRINTF(LEVEL_ERROR, " spdmStatus != LIBSPDM_STATUS_SUCCESS \n ");
        return spdmStatus;
    }

    if (pSpdm == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, " pSpdm  == NULL, SPDM context probably corrupted !! \n ");
        return LIBSPDM_STATUS_INVALID_STATE_LOCAL;
    }
    // Basic validation to ensure we have a real response.
    if (pSpdm->pendingResponseSize == 0 ||
        pSpdm->pendingResponseSize > *message_size)
    {
        spdmStatus = LIBSPDM_STATUS_RECEIVE_FAIL;
        goto ErrorExit;
    }

    portMemCopy(*message, *message_size,
                pSpdm->pTransportBuffer, pSpdm->pendingResponseSize);
    *message_size = pSpdm->pendingResponseSize;

ErrorExit:

    // Ensure size is cleared to indicate no response pending in buffer
    pSpdm->pendingResponseSize = 0;

    return spdmStatus;
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
    if (pGpu == NULL || pSpdm == NULL || pSpdm->pLibspdmContext == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    pSpdm->pendingResponseSize = 0;
    pSpdm->pTransportBuffer    = portMemAllocNonPaged(pSpdm->payloadBufferSize);
    if (pSpdm->pTransportBuffer == NULL)
    {
        pSpdm->transportBufferSize = 0;
        return NV_ERR_NO_MEMORY;
    }
    pSpdm->transportBufferSize = pSpdm->payloadBufferSize;

    // Register transport layer functionality with library.
    libspdm_register_transport_layer_func(pSpdm->pLibspdmContext,
                                          NV_SPDM_MAX_SPDM_PAYLOAD_SIZE,
                                          sizeof(NV_SPDM_DESC_HEADER),
                                          0,
                                          _spdmEncodeMessageGsp,
                                          _spdmDecodeMessageGsp);

    libspdm_register_device_io_func(pSpdm->pLibspdmContext,
                                    _spdmSendMessageGsp,
                                    _spdmReceiveMessageGsp);

    pSpdm->bUsePolling = IS_GSP_CLIENT(pGpu);

    return NV_OK;
}

/*!
 * To deinitialize the GSP SPDM Responder, we need to release the surface for
 * SPDM communication. GSP-RM will handle the rest.
 */
NV_STATUS
spdmDeviceDeinit_GH100
(
    OBJGPU *pGpu,
    Spdm   *pSpdm,
    NvBool  bForceClear
)
{
    // Just-in-case, portMemFree handles NULL.
    portMemFree(pSpdm->pTransportBuffer);
    pSpdm->pTransportBuffer     = NULL;
    pSpdm->transportBufferSize  = 0;
    pSpdm->pendingResponseSize  = 0;

    return NV_OK;
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
    NV_STATUS                                   status                = NV_OK;
    NvU8                                       *pMapMem               = NULL;
    NvU32                                       messagePending        = 0;
    NvU8                                       *pPayloadBuffer        = NULL;
    NvU32                                       transportResponseSize = 0;
    NV2080_CTRL_INTERNAL_SPDM_PARTITION_PARAMS  params;
    MemoryManager                              *pMemoryManager        = NULL;
    TRANSFER_SURFACE                            surf                  = {0};
    NvBool                                      bFreeShadowBuf        = NV_FALSE;

    if (pGpu == NULL || pSpdm == NULL ||pRequest == NULL ||
        pResponse == NULL || pResponseSize == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (requestSize > pSpdm->payloadBufferSize)
    {
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY)

    // Offset is zero for all transfers
    surf.offset    = 0;
    pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);

    if (pSpdm->bUsePolling)
    {
        //
        // If we haven't established the session yet, we need to utilize polling
        // based communication with GSP-SPDM partition, as GSP-RM RPC is not available.
        // Note that there is a short window where session is established but GSP-RM RPC is
        // not active - we don't expect any SPDM messages in this window.
        //
        RMTIMEOUT     timeout;
        KernelGsp    *pKernelGsp    = GPU_GET_KERNEL_GSP(pGpu);
        KernelFalcon *pKernelFalcon = staticCast(pKernelGsp, KernelFalcon);

        pPayloadBuffer = memmgrMemDescBeginTransfer(pMemoryManager, pSpdm->pPayloadBufferMemDesc,
                                                    TRANSFER_FLAGS_SHADOW_ALLOC);
        if (pPayloadBuffer == NULL)
        {
            status = NV_ERR_INSUFFICIENT_RESOURCES;
            goto ErrorExit;
        }

        // First copy payload to shared buffer
        portMemCopy(pPayloadBuffer, requestSize, pRequest, requestSize);
        memdescFlushCpuCaches(pGpu, pSpdm->pPayloadBufferMemDesc);

        // Trigger message pending value, then poll for response from GSP
        kflcnRegWrite_HAL(pGpu, pKernelFalcon, NV_PFALCON_FALCON_MAILBOX0, NV_SPDM_REQUESTER_MESSAGE_PENDING_TOKEN);

        messagePending = kflcnRegRead_HAL(pGpu, pKernelFalcon, NV_PFALCON_FALCON_MAILBOX0);
        gpuSetTimeout(pGpu, GPU_TIMEOUT_DEFAULT, &timeout, 0);
        while (messagePending != NV_SPDM_RESPONDER_MESSAGE_PENDING_TOKEN)
        {
            if (gpuCheckTimeout(pGpu, &timeout) == NV_ERR_TIMEOUT)
            {
                NV_PRINTF(LEVEL_ERROR, "Timeout waiting for response from SPDM Responder!\n");
                DBG_BREAKPOINT();
                status = NV_ERR_TIMEOUT;
                goto ErrorExit;
            }

            messagePending = kflcnRegRead_HAL(pGpu, pKernelFalcon, NV_PFALCON_FALCON_MAILBOX0);
        }
    }
    else
    {
        //
        // At this point, we have abandoned polling and now rely on GSP-RM RPCs.
        //

        // Copy entire SPDM message to shared memory.
        surf.pMemDesc = pSpdm->pPayloadBufferMemDesc;
        status        = memmgrMemWrite(pMemoryManager, &surf, pRequest, requestSize, TRANSFER_FLAGS_NONE);
        if (status != NV_OK)
        {
            goto ErrorExit;
        }

        // Prepare GSP-CMD and send to GSP-SPDM partition
        portMemSet(&params, 0, sizeof(params));
        params.cmd.cmdType = RM_GSP_SPDM_CMD_ID_CC_CTRL;

        status = spdmCtrlSpdmPartition(pGpu, &params);
        if (params.msg.status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "SPDM: RPC failed! RPC status = 0x%x\n",
                      params.msg.status);
            status = params.msg.status;
            DBG_BREAKPOINT();
            goto ErrorExit;
        }
    }

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "SPDM: Send/receive failed! status = 0x%x\n", status);
        DBG_BREAKPOINT();
        goto ErrorExit;
    }

    // Retrieve descriptor header and response message, checking header to ensure valid response.
    surf.pMemDesc = pSpdm->pPayloadBufferMemDesc;
    pMapMem       = portMemAllocNonPaged(pSpdm->payloadBufferSize);
    if (pMapMem == NULL)
    {
        status = NV_ERR_INSUFFICIENT_RESOURCES;
        goto ErrorExit;
    }

    bFreeShadowBuf = NV_TRUE;

    status = memmgrMemRead(pMemoryManager, &surf, pMapMem,
                           pSpdm->payloadBufferSize,
                           TRANSFER_FLAGS_NONE);
    if (status != NV_OK)
    {
        goto ErrorExit;
    }

    transportResponseSize = (((NV_SPDM_DESC_HEADER *)pMapMem)->msgSizeByte + sizeof(NV_SPDM_DESC_HEADER));
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

ErrorExit:
    if (pPayloadBuffer != NULL)
    {
        memmgrMemDescEndTransfer(pMemoryManager, pSpdm->pPayloadBufferMemDesc,
                                 TRANSFER_FLAGS_SHADOW_ALLOC);
    }

    SLI_LOOP_END

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

    if (ret != LIBSPDM_STATUS_SUCCESS || ctExponent != LIBSPDM_MAX_CT_EXPONENT)
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
    algoCheckCount = NV_ARRAY_ELEMENTS(g_SpdmAlgoCheckTable_GH100);

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
    if (pSpdm->pLibspdmContext == NULL || !pSpdm->bSessionEstablished)
    {
        return NV_ERR_NOT_READY;
    }

    // Check to see if KEY_UPDATE is required before using.
    status = spdmCheckAndExecuteKeyUpdate(pGpu, pSpdm, NV_KEY_UPDATE_TRIGGER_ID_GET_MEASUREMENTS);
    if (status != NV_OK)
    {
        return status;
    }

    // Retrieve Attestation Report, if requested.
    if (pAttestationReport != NULL && pAttestationReportSize != NULL)
    {
        // Reset the libspdm message log to ensure it only contains the request and response from this call.
        libspdm_reset_msg_log(pSpdm->pLibspdmContext);
        libspdm_set_msg_log_mode(pSpdm->pLibspdmContext, LIBSPDM_MSG_LOG_MODE_ENABLE);

        numBlocks = SPDM_MAX_MEASUREMENT_BLOCK_COUNT;

        //
        // Use Attestation Report buffer temporarily to store measurements, we will replace
        // with the full Attestation Report from message transcripts. If the Attestation Report
        // buffer is too small to get the measurements, it won't be large enough for the report.
        //
        pMeasurementBuffer = pAttestationReport;
        measurementSize    = *pAttestationReportSize;

        // Request the Attestation Report using the provided nonce, signed by the AK cert.
        CHECK_SPDM_STATUS(libspdm_get_measurement_ex(pSpdm->pLibspdmContext, &pSpdm->sessionId,
            SPDM_GET_MEASUREMENTS_REQUEST_ATTRIBUTES_GENERATE_SIGNATURE,
            SPDM_GET_MEASUREMENTS_REQUEST_MEASUREMENT_OPERATION_ALL_MEASUREMENTS,
            SPDM_CERT_DEFAULT_SLOT_ID, NULL, &numBlocks,
            &measurementSize, pMeasurementBuffer, pNonce, NULL, NULL, NULL, NULL));

        if (!nvspdm_check_and_clear_libspdm_assert())
        {
            // libspdm detects assert/error in GET_MEASUREMENT process, need to return error.
            NV_PRINTF(LEVEL_ERROR, "SPDM: spdmCheckAndClearLibspdmAssert() failed \n");
            DBG_BREAKPOINT();

            return NV_ERR_INVALID_STATE;
        }

        //
        // Message log buffer will hold Attestation Report, which is comprised of
        // the GET_MEASUREMENTS request concatenated with the MEASUREMENTS response.
        //
        if (*pAttestationReportSize < libspdm_get_msg_log_size(pSpdm->pLibspdmContext))
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        portMemCopy(pAttestationReport, *pAttestationReportSize,
                    pSpdm->pMsgLog, libspdm_get_msg_log_size(pSpdm->pLibspdmContext));
        *pAttestationReportSize = libspdm_get_msg_log_size(pSpdm->pLibspdmContext);
    }

    // Retrieve CEC Attestation Report, if requested.
    if (pbIsCecAttestationReportPresent != NULL)
    {
        *pbIsCecAttestationReportPresent = NV_FALSE;
    }

ErrorExit:
    // Regardless of what happens, reset the message log so we don't track anything past this.
    libspdm_reset_msg_log(pSpdm->pLibspdmContext);

    return status;
}

/*!
* Function that sends the opaque data from RM to GSP.
*/
NV_STATUS
spdmSendInitRmDataCommand_GH100
(
    OBJGPU *pGpu,
    Spdm   *pSpdm
)
{
    NV_STATUS                                  status     = NV_OK;
    RMTIMEOUT                                  timeout;
    NV2080_CTRL_INTERNAL_SPDM_PARTITION_PARAMS params;

    if (pGpu == NULL || pSpdm == NULL || !pSpdm->bSessionEstablished)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    portMemSet(&params, 0, sizeof(params));
    params.cmd.cmdType = RM_GSP_SPDM_CMD_ID_CC_INIT_RM_DATA;

    gpuSetTimeout(pGpu, GPU_TIMEOUT_DEFAULT, &timeout, 0);

    status = spdmCtrlSpdmPartition(pGpu, &params);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "SPDM: Send/receive error in INIT_RM_DATA command\n");
        return NV_ERR_FLCN_ERROR;
    }

    // Perform basic validation of header returned.
    status = params.msg.status;
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "SPDM: RPC returned failure in INIT_RM_DATA command! status = 0x%0x\n",
                  status);
        DBG_BREAKPOINT();
        return status;
    }

    return NV_OK;
}

/*!
 * @brief spdmCheckAndExecuteKeyUpdate_GH100
 *        This function is used check scenario and perform key_exchange process if needed.
 *
 * @param[in]     pGpu                : OBJGPU Pointer
 * @param[in]     pSpdm               : SPDM pointer
 * @param[in]     keyUpdateTriggerId  : The id to identify the client, which trigger key update.
 *
 * @return NV_OK                      : Return NV_OK if no error
 *
 */
NV_STATUS
spdmCheckAndExecuteKeyUpdate_GH100
(
    OBJGPU *pGpu,
    Spdm   *pSpdm,
    NvU32   keyUpdateTriggerId
)
{
    libspdm_return_t ret;
    bool    bSingleDirection = false;

    if (pGpu == NULL || pSpdm == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Ensure we have a valid session, and have retrieved the certificates.
    if (pSpdm->pLibspdmContext == NULL || pSpdm->sessionId == INVALID_SESSION_ID)
    {
        return NV_ERR_NOT_READY;
    }

    if (keyUpdateTriggerId == NV_KEY_UPDATE_TRIGGER_ID_GET_MEASUREMENTS ||
        keyUpdateTriggerId == NV_KEY_UPDATE_TRIGGER_ID_HEARTBEAT)
    {
        pSpdm->sessionMsgCount++;
        if (pSpdm->sessionMsgCount >= NV_KEY_UPDATE_TRIGGER_THRESHOLD)
        {
            ret = libspdm_key_update(pSpdm->pLibspdmContext, pSpdm->sessionId,
                                     bSingleDirection);

            if (ret != LIBSPDM_STATUS_SUCCESS)
            {
                NV_PRINTF(LEVEL_ERROR, "Key Update (single direction(0x%x)) failed, ret(0x%x), triggerId = (0x%x).\n",
                          bSingleDirection, ret, keyUpdateTriggerId);
                return NV_ERR_GENERIC;
            }

            if (!nvspdm_check_and_clear_libspdm_assert())
            {
                // libspdm detects assert/error in key update process, need to return error.
                NV_PRINTF(LEVEL_ERROR, "SPDM: spdmCheckAndExecuteKeyUpdate() assert !! \n");
                DBG_BREAKPOINT();
                return NV_ERR_INVALID_STATE;
            }

            NV_PRINTF(LEVEL_INFO, "SPDM: Key update successfully, triggerId = (0x%x)!\n", keyUpdateTriggerId);
            pSpdm->sessionMsgCount = 0;
        }
    }
    else
    {
        return NV_ERR_INVALID_ARGUMENT;
    }


    return NV_OK;
}

NV_STATUS
spdmRegisterForHeartbeats_GH100
(
    OBJGPU *pGpu,
    Spdm   *pSpdm,
    NvU32   heartbeatPeriodSec
)
{
    NV_STATUS                                   status = NV_OK;
    OBJTMR                                     *pTmr;
    RMTIMEOUT                                   timeout;
    NV2080_CTRL_INTERNAL_SPDM_PARTITION_PARAMS  params;

    // Basic parameter validation, make sure we are in a session
    if (pGpu == NULL || pSpdm == NULL || pSpdm->sessionId == INVALID_SESSION_ID)
    {
        return NV_ERR_NOT_READY;
    }
    // Set minimum value to ensure we don't trigger unexpected behavior with small timer values.
    else if (heartbeatPeriodSec < SPDM_DEFAULT_HEARTBEAT_PERIOD_IN_SEC)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    pTmr = GPU_GET_TIMER(pGpu);

    // Create the timer event and schedule the first heartbeat callback
    status = tmrEventCreate(pTmr, &pSpdm->pHeartbeatEvent, _spdmTriggerHeartbeat, pSpdm, TMR_FLAGS_NONE);
    if (status != NV_OK)
    {
        return status;
    }

    pSpdm->heartbeatPeriodSec = heartbeatPeriodSec;
    status = tmrEventScheduleRelSec(pTmr, pSpdm->pHeartbeatEvent, pSpdm->heartbeatPeriodSec);

    // Tell GSP-RM to start expecting heartbeats.
    portMemSet(&params, 0, sizeof(params));
    params.cmd.cmdType                 = RM_GSP_SPDM_CMD_ID_CC_HEARTBEAT_CTRL;
    params.cmd.ccHeartbeatCtrl.bEnable = NV_TRUE;

    gpuSetTimeout(pGpu, GPU_TIMEOUT_DEFAULT, &timeout, 0);
    status = spdmCtrlSpdmPartition(pGpu, &params);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "SPDM: Send/receive error in CC_HEARTBEAT_CTRL command! Status = 0x%0x\n", status);
        return status;
    }

    // Perform basic validation of header returned.
    status = params.msg.status;
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "SPDM: RPC returned failure in CC_HEARTBEAT_CTRL command! status = 0x%0x\n",
                  status);
        DBG_BREAKPOINT();
        return status;
    }

    return status;
}

NV_STATUS
spdmUnregisterFromHeartbeats_GH100
(
    OBJGPU *pGpu,
    Spdm   *pSpdm
)
{
    NV_STATUS                                   status = NV_OK;

    OBJTMR                                     *pTmr = GPU_GET_TIMER(pGpu);
    RMTIMEOUT                                   timeout;
    NV2080_CTRL_INTERNAL_SPDM_PARTITION_PARAMS  params;

    if (pSpdm->pHeartbeatEvent == NULL)
    {
        // No timer exists, we never started sending heartbeats.
        return NV_OK;
    }

    // Tell GSP-RM to stop expecting heartbeats.
    portMemSet(&params, 0, sizeof(params));
    params.cmd.cmdType                 = RM_GSP_SPDM_CMD_ID_CC_HEARTBEAT_CTRL;
    params.cmd.ccHeartbeatCtrl.bEnable = NV_FALSE;

    gpuSetTimeout(pGpu, GPU_TIMEOUT_DEFAULT, &timeout, 0);
    status = spdmCtrlSpdmPartition(pGpu, &params);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "SPDM: Send/receive error in CC_HEARTBEAT_CTRL command! Status = 0x%0x\n", status);
        goto ErrorExit;
    }

    // Perform basic validation of header returned.
    status = params.msg.status;
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "SPDM: RPC returned failure in CC_HEARTBEAT_CTRL command! status = 0x%0x\n",
                  status);
        DBG_BREAKPOINT();
        goto ErrorExit;
    }

ErrorExit:
    // In any case, cancel any further heartbeats that might occur. Handles NULL gracefully.
    tmrEventDestroy(pTmr, pSpdm->pHeartbeatEvent);
    pSpdm->pHeartbeatEvent    = NULL;
    pSpdm->heartbeatPeriodSec = 0;

    return status;
}
