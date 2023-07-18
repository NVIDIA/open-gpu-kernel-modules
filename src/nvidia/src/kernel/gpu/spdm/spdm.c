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

/***************************************************************************\
 *                                                                          *
 *      SPDM Object Module                                                  *
 *                                                                          *
 \**************************************************************************/

/* ------------------------ Includes --------------------------------------- */
#include "gpu/spdm/spdm.h"
#include "spdm/rmspdmtransport.h"
#include "spdm/rmspdmvendordef.h"
#include "gpu/gpu.h"
#include "gpu/gpu_resource.h"

//
// Libspdm only supported on certain builds,
// take note if you are copying header elsewhere.
//
#include "gpu/spdm/libspdm_includes.h"

/* ------------------------ Static Function Prototypes --------------------- */
static void _spdmClearContext(Spdm *pSpdm);
libspdm_return_t _spdmAcquireTransportBuffer(void *context, size_t *max_msg_size, void **msg_buf_ptr);
void _spdmReleaseTransportBuffer(void *context, const void *msg_buf_ptr);
bool _spdmVerifyCertChain(void *spdm_context, uint8_t slot_id, size_t cert_chain_size,
                          const void *cert_chain, const void **trust_anchor, size_t *trust_anchor_size);

/* ------------------------ Static Functions ------------------------------- */
/*
 * Helper to clear and free any SPDM object context.
 */
void
_spdmClearContext
(
    Spdm *pSpdm
)
{
    NvU32 index = 0;

    if (pSpdm == NULL)
    {
        return;
    }

    //
    // If we haven't deinitialized session, not much we can do now.
    // Make best effort to free any allocated Requester context,
    // ensuring we scrub the libspdm context.
    //
    if (pSpdm->pLibspdmContext != NULL)
    {

        libspdm_reset_message_b(pSpdm->pLibspdmContext);
        libspdm_reset_message_c(pSpdm->pLibspdmContext);
        libspdm_reset_message_mut_b(pSpdm->pLibspdmContext);
        libspdm_reset_message_mut_c(pSpdm->pLibspdmContext);

        libspdm_reset_message_m(pSpdm->pLibspdmContext, NULL);
        for (index = 0; index < LIBSPDM_MAX_SESSION_COUNT; index++)
        {
            libspdm_reset_message_m(pSpdm->pLibspdmContext,  &(((libspdm_context_t *)pSpdm->pLibspdmContext)->session_info[index]));
        }

        libspdm_deinit_context(pSpdm->pLibspdmContext);
        libspdm_reset_context(pSpdm->pLibspdmContext);
        portMemSet((NvU8 *)pSpdm->pLibspdmContext, 0, pSpdm->libspdmContextSize);
    }

    if (pSpdm->pLibspdmScratch != NULL)
    {
        portMemSet((NvU8 *)pSpdm->pLibspdmScratch, 0, pSpdm->libspdmScratchSize);
    }

    // portMemFree handles NULL pointers gracefully.
    portMemFree(pSpdm->pLibspdmContext);
    portMemFree(pSpdm->pLibspdmScratch);
    portMemFree(pSpdm->pAttestationCertChain);
    portMemFree(pSpdm->pDeviceIOContext);
    portMemFree(pSpdm->pLastExchange);

    pSpdm->pLibspdmContext          = NULL;
    pSpdm->pLibspdmScratch          = NULL;
    pSpdm->pAttestationCertChain    = NULL;
    pSpdm->pDeviceIOContext         = NULL;
    pSpdm->pLastExchange            = NULL;

    pSpdm->libspdmContextSize       = 0;
    pSpdm->libspdmScratchSize       = 0;
    pSpdm->attestationCertChainSize = 0;
    pSpdm->lastExchangeSize         = 0;

    pSpdm->guestId                  = 0;
    pSpdm->sessionId                = INVALID_SESSION_ID;
    pSpdm->endpointId               = 0;
}

/*
 * Transport layer helpers for send/receive message buffers.
 */
libspdm_return_t
_spdmAcquireTransportBuffer
(
    void     *context,
    size_t   *max_msg_size,
    void   **msg_buf_ptr
)
{
    if (context == NULL || max_msg_size == NULL || msg_buf_ptr == NULL)
    {
        return LIBSPDM_STATUS_INVALID_PARAMETER;
    }

    *msg_buf_ptr = portMemAllocNonPaged(LIBSPDM_MAX_MESSAGE_BUFFER_SIZE);
    if (*msg_buf_ptr == NULL)
    {
        return LIBSPDM_STATUS_BUFFER_FULL;
    }

    *max_msg_size = LIBSPDM_MAX_MESSAGE_BUFFER_SIZE;

    return LIBSPDM_STATUS_SUCCESS;
}

void
_spdmReleaseTransportBuffer
(
    void       *context,
    const void *msg_buf_ptr
)
{
    portMemFree((void *)msg_buf_ptr);
}

bool
_spdmVerifyCertChain
(
    void        *spdm_context,
    uint8_t      slot_id,
    size_t       cert_chain_size,
    const void  *cert_chain,
    const void **trust_anchor,
    size_t       *trust_anchor_size
)
{
    return NV_TRUE;
}

/* ------------------------ Public Functions ------------------------------- */
/*!
 * Constructor
 */
NV_STATUS
spdmConstruct_IMPL
(
    Spdm *pSpdm
)
{

    if (pSpdm == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    pSpdm->pLibspdmContext          = NULL;
    pSpdm->pLibspdmScratch          = NULL;
    pSpdm->pAttestationCertChain    = NULL;
    pSpdm->pDeviceIOContext         = NULL;
    pSpdm->pLastExchange            = NULL;

    pSpdm->libspdmContextSize       = 0;
    pSpdm->libspdmScratchSize       = 0;
    pSpdm->attestationCertChainSize = 0;
    pSpdm->lastExchangeSize         = 0;

    pSpdm->guestId                  = 0;
    pSpdm->sessionId                = INVALID_SESSION_ID;
    pSpdm->endpointId               = 0;

    pSpdm->bExportSecretCleared     = NV_FALSE;

    return NV_OK;
}

/*!
 * Destructor
 */
void
spdmDestruct_IMPL
(
    Spdm * pSpdm
)
{
    _spdmClearContext(pSpdm);
}

NV_STATUS
spdmContextInit_IMPL
(
    OBJGPU *pGpu,
    Spdm   *pSpdm
)
{
    NV_STATUS                status = NV_OK;
    libspdm_data_parameter_t parameter;
    uint8_t                  ctExponent;
    uint32_t                 capFlags;
    uint8_t                  measSpec;
    uint32_t                 baseAsymAlgo;
    uint32_t                 baseHashAlgo;
    uint16_t                 dheGroup;
    uint16_t                 aeadSuite;
    uint16_t                 keySched;

    if (pGpu == NULL || pSpdm == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Allocate and initialize all required memory for context and certificates.
    pSpdm->libspdmContextSize = libspdm_get_context_size();
    pSpdm->pLibspdmContext    = portMemAllocNonPaged(pSpdm->libspdmContextSize);

    if (pSpdm->libspdmContextSize == 0 || pSpdm->pLibspdmContext == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        goto ErrorExit;
    }

    portMemSet(pSpdm->pLibspdmContext, 0, pSpdm->libspdmContextSize);
    libspdm_init_context(pSpdm->pLibspdmContext);

    // Allocate scratch space required for libspdm processing.
    pSpdm->libspdmScratchSize = libspdm_get_sizeof_required_scratch_buffer(pSpdm->pLibspdmContext);
    pSpdm->pLibspdmScratch    = portMemAllocNonPaged(pSpdm->libspdmScratchSize);

    if (pSpdm->libspdmScratchSize == 0 || pSpdm->pLibspdmScratch == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        goto ErrorExit;
    }

    portMemSet(pSpdm->pLibspdmScratch, 0, pSpdm->libspdmScratchSize);
    libspdm_set_scratch_buffer(pSpdm->pLibspdmContext, pSpdm->pLibspdmScratch,
                               pSpdm->libspdmScratchSize);

    // Allocate message transcript recording buffer.
    pSpdm->pLastExchange    = portMemAllocNonPaged(SPDM_MAX_EXCHANGE_BUFFER_SIZE);
    pSpdm->lastExchangeSize = 0;

    if (pSpdm->pLastExchange == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        goto ErrorExit;
    }

    //
    // Eventually, owner of Spdm object may want to set their own
    // specific configuration. For now, hardcode the only supported configuration.
    //
    portMemSet(&parameter, 0, sizeof(parameter));
    parameter.location = LIBSPDM_DATA_LOCATION_LOCAL;

    // Requester will not check Responder's timing, set to maximum value.
    ctExponent = SPDM_CAPABILITIES_CT_EXPONENT_MAX;
    CHECK_SPDM_STATUS(libspdm_set_data(pSpdm->pLibspdmContext, LIBSPDM_DATA_CAPABILITY_CT_EXPONENT,
                                       &parameter, &ctExponent, sizeof(ctExponent)));

    capFlags = SPDM_GET_CAPABILITIES_REQUEST_FLAGS_CERT_CAP    |
               SPDM_GET_CAPABILITIES_REQUEST_FLAGS_ENCRYPT_CAP |
               SPDM_GET_CAPABILITIES_REQUEST_FLAGS_MAC_CAP     |
               SPDM_GET_CAPABILITIES_REQUEST_FLAGS_KEY_EX_CAP  |
               SPDM_GET_CAPABILITIES_REQUEST_FLAGS_KEY_UPD_CAP;
    CHECK_SPDM_STATUS(libspdm_set_data(pSpdm->pLibspdmContext,
                                       LIBSPDM_DATA_CAPABILITY_FLAGS, &parameter,
                                       &capFlags, sizeof(capFlags)));

    measSpec = SPDM_MEASUREMENT_BLOCK_HEADER_SPECIFICATION_DMTF;
    CHECK_SPDM_STATUS(libspdm_set_data(pSpdm->pLibspdmContext,
                                       LIBSPDM_DATA_MEASUREMENT_SPEC, &parameter,
                                       &measSpec, sizeof(measSpec)));

    baseAsymAlgo = SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_ECDSA_ECC_NIST_P384;
    CHECK_SPDM_STATUS(libspdm_set_data(pSpdm->pLibspdmContext, LIBSPDM_DATA_BASE_ASYM_ALGO,
                                       &parameter, &baseAsymAlgo,
                                       sizeof(baseAsymAlgo)));

    baseHashAlgo = SPDM_ALGORITHMS_BASE_HASH_ALGO_TPM_ALG_SHA_384;
    CHECK_SPDM_STATUS(libspdm_set_data(pSpdm->pLibspdmContext, LIBSPDM_DATA_BASE_HASH_ALGO,
                                       &parameter, &baseHashAlgo,
                                       sizeof(baseHashAlgo)));

    dheGroup = SPDM_ALGORITHMS_DHE_NAMED_GROUP_SECP_384_R1;
    CHECK_SPDM_STATUS(libspdm_set_data(pSpdm->pLibspdmContext, LIBSPDM_DATA_DHE_NAME_GROUP,
                                       &parameter, &dheGroup, sizeof(dheGroup)));

    aeadSuite = SPDM_ALGORITHMS_AEAD_CIPHER_SUITE_AES_256_GCM;
    CHECK_SPDM_STATUS(libspdm_set_data(pSpdm->pLibspdmContext, LIBSPDM_DATA_AEAD_CIPHER_SUITE,
                                       &parameter, &aeadSuite, sizeof(aeadSuite)));

    keySched = SPDM_ALGORITHMS_KEY_SCHEDULE_HMAC_HASH;
    CHECK_SPDM_STATUS(libspdm_set_data(pSpdm->pLibspdmContext, LIBSPDM_DATA_KEY_SCHEDULE,
                                       &parameter, &keySched, sizeof(keySched)));

    pSpdm->guestId = 0x01;

    //
    // Perform any device-specific initialization. spdmDeviceInit is also
    // responsible for registering transport layer functions with libspdm.
    //
    status = spdmDeviceInit_HAL(pGpu, pSpdm);
    if (status != NV_OK)
    {
        goto ErrorExit;
    }

    libspdm_register_device_buffer_func(pSpdm->pLibspdmContext,
        _spdmAcquireTransportBuffer, _spdmReleaseTransportBuffer,
        _spdmAcquireTransportBuffer, _spdmReleaseTransportBuffer);

    //
    // Verifier is responsible for verifying the certificate chain. To avoid concerns
    // with libspdm compatibility, override certificate validation function with stub.
    //
    libspdm_register_verify_spdm_cert_chain_func(pSpdm->pLibspdmContext, _spdmVerifyCertChain);

ErrorExit:

    // Clear all SPDM state on failure.
    if (status != NV_OK)
    {
        _spdmClearContext(pSpdm);
    }

    return status;
}

NV_STATUS
spdmContextDeinit_IMPL
(
    OBJGPU *pGpu,
    Spdm   *pSpdm,
    NvBool  bForceClear
)
{
    NV_STATUS status = NV_OK;

    if (pGpu == NULL || pSpdm == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    // If no session to end, just wipe state and return.
    if (pSpdm->pLibspdmContext == NULL)
    {
        _spdmClearContext(pSpdm);
        return NV_OK;
    }

    //
    // End the session by deinitializing the Responder.
    // We don't send END_SESSION as Responder will handle teardown.
    //
    NV_PRINTF(LEVEL_INFO, "SPDM: Tearing down session.\n");
    status = spdmDeviceDeinit_HAL(pGpu, pSpdm, NV_TRUE);

    // Regardless of success or failure, clear any context.
    _spdmClearContext(pSpdm);

    // We really shouldn't fail on deinitialization - ASSERT if we do.
    NV_ASSERT_OK(status);
    return status;
}

NV_STATUS
spdmStart_IMPL
(
    OBJGPU *pGpu,
    Spdm   *pSpdm
)
{
    NV_STATUS status = NV_OK;

    if (pGpu == NULL || pSpdm == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (pSpdm->pLibspdmContext == NULL)
    {
        return NV_ERR_NOT_READY;
    }

     // Send GET_VERSION, GET_CAPABILITIES, and NEGOTIATE_ALGORITHMS to Responder.
    NV_PRINTF(LEVEL_INFO, "SPDM: Starting new SPDM connection.\n");
    CHECK_SPDM_STATUS(libspdm_init_connection(pSpdm->pLibspdmContext, NV_FALSE));

    if (!nvspdm_check_and_clear_libspdm_assert())
    {
        NV_PRINTF(LEVEL_ERROR, "SPDM: libspdm_init_connection() assert hit !!!.\n");
        status =  NV_ERR_GENERIC;
        goto ErrorExit;
    }

    // Ensure the connection attributes match expected.
    status = spdmCheckConnection_HAL(pGpu, pSpdm);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "SPDM: Connection attributes did not match expected!\n");
        goto ErrorExit;
    }

    // Fetch the certificates from the responder and validate them
    status = spdmGetCertificates_HAL(pGpu, pSpdm);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "SPDM: Certificate retrieval failed!\n");
        goto ErrorExit;
    }

    if (!nvspdm_check_and_clear_libspdm_assert())
    {
        NV_PRINTF(LEVEL_ERROR, "SPDM: spdmGetCertificates_HAL() assert hit !!!.\n");
        status =  NV_ERR_GENERIC;
        goto ErrorExit;
    }

    //
    // Complete the SPDM handshake and start the secured session.
    // Ensure we match type of sessionId parameter with what libspdm expects.
    //
    if (spdmDeviceSecuredSessionSupported_HAL(pGpu, pSpdm) == NV_OK)
    {
        NV_PRINTF(LEVEL_INFO, "SPDM: Attempting to establish SPDM session.\n");
        CHECK_SPDM_STATUS(libspdm_start_session(pSpdm->pLibspdmContext, NV_FALSE,
                                                SPDM_KEY_EXCHANGE_REQUEST_NO_MEASUREMENT_SUMMARY_HASH,
                                                SPDM_CERT_DEFAULT_SLOT_ID, 0, &pSpdm->sessionId, NULL, NULL));


        if (!nvspdm_check_and_clear_libspdm_assert())
        {
            NV_PRINTF(LEVEL_ERROR, "SPDM: libspdm_start_session() assert hit !!!.\n");
            status =  NV_ERR_GENERIC;
            goto ErrorExit;
        }
        else
        {
            NV_PRINTF(LEVEL_INFO, "SPDM: Session establishment successful: sessionId 0x%x.\n",
                                   pSpdm->sessionId);
        }
    }

ErrorExit:

    //
    // On error, set session as invalid. Don't need to reset context, since
    // restarting SPDM exchange is valid scenario. Responder may not support.
    //
    if (status != NV_OK)
    {
        pSpdm->sessionId = INVALID_SESSION_ID;
        NV_PRINTF(LEVEL_ERROR, "SPDM: Session establishment failed!\n");
        DBG_BREAKPOINT();
    }

    return status;
}

NV_STATUS
spdmRetrieveExportSecret_IMPL
(
    OBJGPU *pGpu,
    Spdm   *pSpdm,
    NvU32   keySize,
    NvU8   *pKeyOut
)
{
    size_t                             keySizeSizeT    = 0;
    libspdm_secured_message_context_t *pSessionContext = NULL;

    // Basic parameter validation.
    if (pGpu == NULL || pSpdm == NULL || keySize == 0 || pKeyOut == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Ensure we are in valid state. Note that export master secret can only be retrieved once.
    if (pSpdm->pLibspdmContext == NULL || pSpdm->sessionId == INVALID_SESSION_ID ||
        pSpdm->bExportSecretCleared)
    {
        return NV_ERR_NOT_READY;
    }

    pSessionContext =
        libspdm_get_secured_message_context_via_session_id(pSpdm->pLibspdmContext, pSpdm->sessionId);
    if (pSessionContext == NULL)
    {
        return NV_ERR_INVALID_STATE;
    }

    //
    // Fetch the export master secret.
    // Use temporary size variable to handle type differences and avoid overflow.
    //
    keySizeSizeT = keySize;
    if (!libspdm_secured_message_export_master_secret(pSessionContext, pKeyOut, &keySizeSizeT))
    {
        return NV_ERR_INVALID_STATE;
    }

    // Clear the export master secret from SPDM memory.
    libspdm_secured_message_clear_export_master_secret(pSessionContext);
    pSpdm->bExportSecretCleared = NV_TRUE;

    return NV_OK;
}

