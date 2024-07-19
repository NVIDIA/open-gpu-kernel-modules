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
#include "nvspdm_rmconfig.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "platform/sli/sli.h"
#include "rmapi/client_resource.h"
#include "gpu/bus/kern_bus.h"
#include "os/os.h"

//
// Libspdm only supported on certain builds,
// take note if you are copying header elsewhere.
//
#include "gpu/spdm/libspdm_includes.h"


/* ------------------------ Static Function Prototypes --------------------- */
static void _spdmClearContext(Spdm *pSpdm);
libspdm_return_t _spdmAcquireTransportBuffer(void *context, void **msg_buf_ptr);
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
        libspdm_deinit_context(pSpdm->pLibspdmContext);
        libspdm_reset_context(pSpdm->pLibspdmContext);
        portMemSet((NvU8 *)pSpdm->pLibspdmContext, 0, pSpdm->libspdmContextSize);
    }

    if (pSpdm->pLibspdmScratch != NULL)
    {
        portMemSet((NvU8 *)pSpdm->pLibspdmScratch, 0, pSpdm->libspdmScratchSize);
    }

    // memdescFree and memdescDestroy handle NULL gracefully.
    memdescFree(pSpdm->pPayloadBufferMemDesc);
    memdescDestroy(pSpdm->pPayloadBufferMemDesc);

    pSpdm->pPayloadBufferMemDesc  = NULL;
    pSpdm->payloadBufferSize      = 0;

    // portMemFree handles NULL pointers gracefully.
    portMemFree(pSpdm->pLibspdmContext);
    portMemFree(pSpdm->pLibspdmScratch);
    portMemFree(pSpdm->pAttestationCertChain);
    portMemFree(pSpdm->pDeviceIOContext);
    portMemFree(pSpdm->pMsgLog);

    pSpdm->pLibspdmContext          = NULL;
    pSpdm->pLibspdmScratch          = NULL;
    pSpdm->pAttestationCertChain    = NULL;
    pSpdm->pDeviceIOContext         = NULL;
    pSpdm->pMsgLog                  = NULL;

    pSpdm->libspdmContextSize       = 0;
    pSpdm->libspdmScratchSize       = 0;
    pSpdm->attestationCertChainSize = 0;
    pSpdm->msgLogMaxSize            = 0;

    pSpdm->sessionId                = INVALID_SESSION_ID;
    pSpdm->bSessionEstablished      = NV_FALSE;
    pSpdm->bUsePolling              = NV_FALSE;
}

/*
 * Transport layer helpers for send/receive message buffers.
 */
libspdm_return_t
_spdmAcquireTransportBuffer
(
    void     *context,
    void   **msg_buf_ptr
)
{
    if (context == NULL || msg_buf_ptr == NULL)
    {
        return LIBSPDM_STATUS_INVALID_PARAMETER;
    }

    *msg_buf_ptr = portMemAllocNonPaged(NV_SPDM_SYSMEM_SURFACE_SIZE_IN_BYTES);
    if (*msg_buf_ptr == NULL)
    {
        return LIBSPDM_STATUS_BUFFER_FULL;
    }

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
    pSpdm->pMsgLog                  = NULL;

    pSpdm->libspdmContextSize       = 0;
    pSpdm->libspdmScratchSize       = 0;
    pSpdm->attestationCertChainSize = 0;
    pSpdm->msgLogMaxSize            = 0;

    pSpdm->sessionId                = INVALID_SESSION_ID;
    pSpdm->bSessionEstablished      = NV_FALSE;
    pSpdm->bUsePolling              = NV_FALSE;
    pSpdm->bExportSecretCleared     = NV_FALSE;

    pSpdm->pPayloadBufferMemDesc    = NULL;
    pSpdm->payloadBufferSize        = 0;

    pSpdm->pHeartbeatEvent          = NULL;
    pSpdm->heartbeatPeriodSec       = 0;

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
spdmSetupCommunicationBuffers_IMPL
(
    OBJGPU *pGpu,
    Spdm   *pSpdm
)
{
    MemoryManager    *pMemoryManager = NULL;
    TRANSFER_SURFACE  surf           = {0};
    NV_STATUS         status         = NV_OK;

    // Create memory descriptor for payload buffer
    status = memdescCreate(&pSpdm->pPayloadBufferMemDesc, pGpu, NV_SPDM_SYSMEM_SURFACE_SIZE_PAGE_ALIGNED,
                           NV_SPDM_SYSMEM_SURFACE_ALIGNMENT_IN_BYTES, NV_TRUE, ADDR_SYSMEM,
                           NV_MEMORY_CACHED, MEMDESC_FLAGS_ALLOC_IN_UNPROTECTED_MEMORY);
    if (status != NV_OK || pSpdm->pPayloadBufferMemDesc == NULL)
    {
        status = NV_ERR_INSUFFICIENT_RESOURCES;
        goto ErrorExit;
    }

    memdescTagAlloc(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_82, pSpdm->pPayloadBufferMemDesc);
    if (status != NV_OK)
    {
        goto ErrorExit;
    }

    // We over-allocate since we must allocate page-aligned. Set size only to what we will use.
    pSpdm->payloadBufferSize = NV_SPDM_SYSMEM_SURFACE_SIZE_IN_BYTES;

    // Scrub surface
    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY)
    pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    surf.offset    = 0;

    surf.pMemDesc = pSpdm->pPayloadBufferMemDesc;
    status = memmgrMemSet(pMemoryManager, &surf, 0, pSpdm->payloadBufferSize, TRANSFER_FLAGS_NONE);
    if (status != NV_OK)
    {
        SLI_LOOP_GOTO(ErrorExit);
    }

    SLI_LOOP_END

ErrorExit:

    if (status != NV_OK)
    {
        _spdmClearContext(pSpdm);
    }

    return status;
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
    uint32_t                 maxSessionCount;
    uint8_t                  maxRetries;

#if LIBSPDM_ENABLE_CAPABILITY_MUT_AUTH_CAP
    uint16_t                 reqAsymAlgo;
    NvU8                    *pEncapCertChain;
    NvU32                    encapCertChainSize;
#endif

    if (pGpu == NULL || pSpdm == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (!libspdm_check_crypto_backend())
    {
        NV_PRINTF(LEVEL_ERROR, "SPDM cannot boot without proper crypto backend!\n");
        return NV_ERR_INVALID_STATE;
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


    // Allocate message transcript recording buffer.
    pSpdm->pMsgLog       = portMemAllocNonPaged(NV_SPDM_MAX_TRANSCRIPT_BUFFER_SIZE);
    pSpdm->msgLogMaxSize = NV_SPDM_MAX_TRANSCRIPT_BUFFER_SIZE;

    if (pSpdm->pMsgLog == NULL)
    {
        pSpdm->msgLogMaxSize = 0;
        status               = NV_ERR_NO_MEMORY;
        goto ErrorExit;
    }

#if LIBSPDM_ENABLE_CAPABILITY_MUT_AUTH_CAP
    // Get requester cert chain for mutual authentication process.
    pEncapCertChain = NULL;
    encapCertChainSize = 0;
    status = spdmGetReqEncapCertificates_HAL(pGpu, pSpdm, &pEncapCertChain, &encapCertChainSize);

    if (status != NV_OK || pEncapCertChain == NULL || encapCertChainSize == 0)
    {
        status = NV_ERR_NOT_SUPPORTED;
        goto ErrorExit;
    }
#endif

    //
    // Eventually, owner of Spdm object may want to set their own
    // specific configuration. For now, hardcode the only supported configuration.
    //
    portMemSet(&parameter, 0, sizeof(parameter));
    parameter.location = LIBSPDM_DATA_LOCATION_LOCAL;

    // Requester will not check Responder's timing, set to maximum value.
    ctExponent = LIBSPDM_MAX_CT_EXPONENT;
    CHECK_SPDM_STATUS(libspdm_set_data(pSpdm->pLibspdmContext, LIBSPDM_DATA_CAPABILITY_CT_EXPONENT,
                                       &parameter, &ctExponent, sizeof(ctExponent)));

#if LIBSPDM_ENABLE_CAPABILITY_MUT_AUTH_CAP
    capFlags = SPDM_GET_CAPABILITIES_REQUEST_FLAGS_CERT_CAP     |
               SPDM_GET_CAPABILITIES_REQUEST_FLAGS_ENCRYPT_CAP  |
               SPDM_GET_CAPABILITIES_REQUEST_FLAGS_MAC_CAP      |
               SPDM_GET_CAPABILITIES_REQUEST_FLAGS_KEY_EX_CAP   |
               SPDM_GET_CAPABILITIES_REQUEST_FLAGS_KEY_UPD_CAP  |
               SPDM_GET_CAPABILITIES_REQUEST_FLAGS_ENCAP_CAP    |
               SPDM_GET_CAPABILITIES_REQUEST_FLAGS_MUT_AUTH_CAP |
               SPDM_GET_CAPABILITIES_REQUEST_FLAGS_HBEAT_CAP;
#else
    capFlags = SPDM_GET_CAPABILITIES_REQUEST_FLAGS_CERT_CAP    |
               SPDM_GET_CAPABILITIES_REQUEST_FLAGS_ENCRYPT_CAP |
               SPDM_GET_CAPABILITIES_REQUEST_FLAGS_MAC_CAP     |
               SPDM_GET_CAPABILITIES_REQUEST_FLAGS_KEY_EX_CAP  |
               SPDM_GET_CAPABILITIES_REQUEST_FLAGS_KEY_UPD_CAP |
               SPDM_GET_CAPABILITIES_REQUEST_FLAGS_HBEAT_CAP;
#endif
    CHECK_SPDM_STATUS(libspdm_set_data(pSpdm->pLibspdmContext,
                                       LIBSPDM_DATA_CAPABILITY_FLAGS, &parameter,
                                       &capFlags, sizeof(capFlags)));

    measSpec = SPDM_MEASUREMENT_SPECIFICATION_DMTF;
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

#if LIBSPDM_ENABLE_CAPABILITY_MUT_AUTH_CAP
    reqAsymAlgo = SPDM_ALGORITHMS_BASE_ASYM_ALGO_TPM_ALG_RSAPSS_3072;
    CHECK_SPDM_STATUS(libspdm_set_data(pSpdm->pLibspdmContext, LIBSPDM_DATA_REQ_BASE_ASYM_ALG,
                                       &parameter, &reqAsymAlgo,
                                       sizeof(reqAsymAlgo)));

    //
    // Set certification for encapsulated command process.
    // Specify certificate location, passing slot number as well.
    //
    parameter.additional_data[0] = SPDM_CERT_DEFAULT_SLOT_ID;
    CHECK_SPDM_STATUS(libspdm_set_data(pSpdm->pLibspdmContext, LIBSPDM_DATA_LOCAL_PUBLIC_CERT_CHAIN,
                                       &parameter, pEncapCertChain,
                                       encapCertChainSize));
#endif

    // Ensure that we set only DHE sessions as allowed, not PSK sessions.
    maxSessionCount = 1;
    CHECK_SPDM_STATUS(libspdm_set_data(pSpdm->pLibspdmContext, LIBSPDM_DATA_MAX_DHE_SESSION_COUNT,
                                       &parameter, &maxSessionCount, sizeof(maxSessionCount)));

    maxSessionCount = 0;
    CHECK_SPDM_STATUS(libspdm_set_data(pSpdm->pLibspdmContext, LIBSPDM_DATA_MAX_PSK_SESSION_COUNT,
                                       &parameter, &maxSessionCount, sizeof(maxSessionCount)));


    // We don't allow SPDM message retries.
    maxRetries = 0;
    CHECK_SPDM_STATUS(libspdm_set_data(pSpdm->pLibspdmContext, LIBSPDM_DATA_REQUEST_RETRY_TIMES,
                                       &parameter, &maxRetries, sizeof(maxRetries)));

    libspdm_init_msg_log(pSpdm->pLibspdmContext, pSpdm->pMsgLog, pSpdm->msgLogMaxSize);


    // Store SPDM object pointer to libspdm context
    CHECK_SPDM_STATUS(libspdm_set_data(pSpdm->pLibspdmContext, LIBSPDM_DATA_APP_CONTEXT_DATA,
                                       NULL, (void *)&pSpdm, sizeof(void *)));

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
        NV_SPDM_SYSMEM_SURFACE_SIZE_IN_BYTES, NV_SPDM_SYSMEM_SURFACE_SIZE_IN_BYTES,
        _spdmAcquireTransportBuffer, _spdmReleaseTransportBuffer,
        _spdmAcquireTransportBuffer, _spdmReleaseTransportBuffer);

    //
    // Allocate scratch space required for libspdm processing.
    // We need to wait for transport layer initialization (i.e. after device init)
    // in order to properly calculate the required scratch size.
    //
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

    //
    // Verifier is responsible for verifying the certificate chain. To avoid concerns
    // with libspdm compatibility, override certificate validation function with stub.
    //
    libspdm_register_verify_spdm_cert_chain_func(pSpdm->pLibspdmContext, _spdmVerifyCertChain);

    // Initialize session message count to zero.
    pSpdm->sessionMsgCount = 0;

    // Libspdm provides basic functionality to check the context
    if (!libspdm_check_context(pSpdm->pLibspdmContext))
    {
        NV_PRINTF(LEVEL_ERROR, "Failed libspdm context selftest!\n");
        status = NV_ERR_INVALID_STATE;
        goto ErrorExit;
    }

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
    NV_STATUS  status = NV_OK;

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
    // Make sure to unregister heartbeats in case we didn't
    // hit normal teardown path. Best effort attempt.
    //
    status = spdmUnregisterFromHeartbeats(pGpu, pSpdm);
    NV_ASSERT_OK(status);

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
        NvU8 heartbeatPeriodInSec = 0;

        NV_PRINTF(LEVEL_INFO, "SPDM: Attempting to establish SPDM session.\n");
        CHECK_SPDM_STATUS(libspdm_start_session(pSpdm->pLibspdmContext, NV_FALSE, NULL, 0,
                                                SPDM_KEY_EXCHANGE_REQUEST_NO_MEASUREMENT_SUMMARY_HASH,
                                                SPDM_CERT_DEFAULT_SLOT_ID, 0, &pSpdm->sessionId,
                                                &heartbeatPeriodInSec, NULL));
        if (!nvspdm_check_and_clear_libspdm_assert())
        {
            NV_PRINTF(LEVEL_ERROR, "SPDM: libspdm_start_session() assert hit !!!.\n");
            status = NV_ERR_GENERIC;
            goto ErrorExit;
        }
        else if (heartbeatPeriodInSec != SPDM_DEFAULT_HEARTBEAT_PERIOD_IN_SEC)
        {
            //
            // Do a basic check to make sure the SPDM heartbeat period agreed between
            // Requester and Responder is expected, even if it is overridden via regkey.
            //
            NV_PRINTF(LEVEL_ERROR, "SPDM: Responder returned unexpected heartbeat 0x%x\n",
                      heartbeatPeriodInSec);
            status = NV_ERR_NOT_SUPPORTED;
            goto ErrorExit;
        }

        NV_PRINTF(LEVEL_INFO, "SPDM: Session establishment successful: sessionId 0x%x.\n",
                  pSpdm->sessionId);
        pSpdm->bSessionEstablished = NV_TRUE;
        pSpdm->bUsePolling         = NV_FALSE;
    }

ErrorExit:

    //
    // On error, set session as invalid. Don't need to reset context, since
    // restarting SPDM exchange is valid scenario. Responder may not support.
    //
    if (status != NV_OK)
    {
        pSpdm->sessionId           = INVALID_SESSION_ID;
        pSpdm->bSessionEstablished = NV_FALSE;
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
    if (pSpdm->pLibspdmContext == NULL || !pSpdm->bSessionEstablished || pSpdm->bExportSecretCleared)
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
