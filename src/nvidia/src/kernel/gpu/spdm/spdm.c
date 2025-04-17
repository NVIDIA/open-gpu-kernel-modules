/*
 * SPDX-FileCopyrightText: Copyright (c) 2022-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "ctrl/ctrl2080/ctrl2080spdm.h"
#include "gpu/subdevice/subdevice.h"
#include "core/locks.h"
#include "rmapi/rs_utils.h"

//
// Libspdm only supported on certain builds,
// take note if you are copying header elsewhere.
//
#include "gpu/spdm/libspdm_includes.h"

// Once SPDM is moved out from CC object, remove this dependency.
#include "gpu/conf_compute/conf_compute.h"

/* ------------------------ Defines --------------------- */
#define SPDM_PEM_BEGIN_CERTIFICATE "-----BEGIN CERTIFICATE-----\n"
#define SPDM_PEM_END_CERTIFICATE   "-----END CERTIFICATE-----\n"
#define DER_LONG_FORM_LENGTH_FIELD_BIT   (0x80)
#define DER_CERT_SIZE_FIELD_LENGTH       (0x4)

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
    portMemFree(pSpdm->pTranscriptLog);

    pSpdm->pLibspdmContext          = NULL;
    pSpdm->pLibspdmScratch          = NULL;
    pSpdm->pAttestationCertChain    = NULL;
    pSpdm->pDeviceIOContext         = NULL;
    pSpdm->pMsgLog                  = NULL;
    pSpdm->pTranscriptLog           = NULL;

    pSpdm->libspdmContextSize       = 0;
    pSpdm->libspdmScratchSize       = 0;
    pSpdm->attestationCertChainSize = 0;
    pSpdm->msgLogMaxSize            = 0;
    pSpdm->transcriptLogSize        = 0;

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

/*!
 @param pCert       : The pointer to certification chain start
 @param bufferEnd   : The pointer to certification chain end
 @parsm pCertLength : The pointer to store return certification size

 @return Return NV-OK if no error.

* Static function that calculates the length of the X509 certificate in DER/TLV
* format. It assumes that the certificate is valid.
*/
static NV_STATUS
_calcX509CertSize
(
    NvU8 *pCert,
    NvU8 *bufferEnd,
    NvU32 *pCertLength
)
{
    // The cert is in TLV format.
    NvU32 certSize       = pCert[1];

    // Check to make sure that some data exists after SPDM header, and it is enough to check cert size.
    if (pCert + DER_CERT_SIZE_FIELD_LENGTH >= bufferEnd ||
        pCert + DER_CERT_SIZE_FIELD_LENGTH <= pCert)
    {
        NV_PRINTF(LEVEL_ERROR, " %s: pCert + DER_CERT_SIZE_FIELD_LENGTH(0x%x) is not valid value !! \n",
                  __FUNCTION__, DER_CERT_SIZE_FIELD_LENGTH);

        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Check if the length is in DER longform.
    // MSB in the length field is set for long form notation.
    // fields.
    if (certSize & DER_LONG_FORM_LENGTH_FIELD_BIT)
    {
        //
        // The remaining bits in the length field indicate the
        // number of following bytes used to represent the length.
        // in base 256, most significant digit first.
        //
        NvU32 numLenBytes = certSize & 0x3f;
        NvU8 *pStart      = &pCert[2];
        NvU8 *pEnd        = pStart + numLenBytes; // NOTE: Don't need to subtract numLenBytes 1 here.

        // Checking for buffer overflow.
        if (pEnd > bufferEnd)
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        certSize = *pStart;
        while (++pStart < pEnd)
        {
            certSize = (certSize << 8) + *pStart ;
        }
        // Total cert length includes the Tag + length
        // Adding it here.
        certSize += 2 + numLenBytes;
    }

    //
    // Check to make sure we have not hit end of buffer, and there is space for AK cert.
    // Check for underflow as well. This makes sure we haven't missed the calculation to
    // go past the end of the buffer
    //
    if (pCert + (certSize - 1) > bufferEnd ||
        pCert + (certSize - 1) <= pCert)
    {
        NV_PRINTF(LEVEL_ERROR, " %s: pCert + (certSize(0x%x) - 1) is not a valid value !! \n",
                  __FUNCTION__, certSize);

        return NV_ERR_BUFFER_TOO_SMALL;
    }

    *pCertLength = certSize;
    return NV_OK;
}

static NV_STATUS
pem_write_buffer
(
    NvU8 const *der,
    NvU64       derLen,
    NvU8       *buffer,
    NvU64       bufferLen,
    NvU64      *bufferUsed
)
{
    static const NvU8 base64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    NvU64 i, tmp, size;
    NvU64 printed = 0;
    NvU8 *ptr = buffer;

    // Base64 encoded size
    size = (derLen + 2) / 3 * 4;

    // Add 1 byte per 64 for newline
    size = size + (size + 63) / 64;

    // Add header excluding the terminating null and footer including the null
    size += sizeof(SPDM_PEM_BEGIN_CERTIFICATE) - 1 +
            sizeof(SPDM_PEM_END_CERTIFICATE);

    if (bufferLen < size)
    {
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    portMemCopy(ptr, bufferLen - (ptr - buffer), SPDM_PEM_BEGIN_CERTIFICATE,
                sizeof(SPDM_PEM_BEGIN_CERTIFICATE) - 1);
    ptr += sizeof(SPDM_PEM_BEGIN_CERTIFICATE) - 1;

    for (i = 0; (i + 2) < derLen; i += 3)
    {
        tmp = (der[i] << 16) | (der[i + 1] << 8) | (der[i + 2]);
        *ptr++ = base64[(tmp >> 18) & 63];
        *ptr++ = base64[(tmp >> 12) & 63];
        *ptr++ = base64[(tmp >> 6) & 63];
        *ptr++ = base64[(tmp >> 0) & 63];

        printed += 4;
        if (printed == 64)
        {
            *ptr++ = '\n';
            printed = 0;
        }
    }

    if ((i == derLen) && (printed != 0))
    {
        *ptr++ = '\n';
    }

    // 1 byte extra
    if (i == (derLen - 1))
    {
        tmp = der[i] << 4;
        *ptr++ = base64[(tmp >> 6) & 63];
        *ptr++ = base64[(tmp >> 0) & 63];
        *ptr++ = '=';
        *ptr++ = '=';
        *ptr++ = '\n';
    }

    // 2 byte extra
    if (i == (derLen - 2))
    {
        tmp = ((der[i] << 8) | (der[i + 1])) << 2;
        *ptr++ = base64[(tmp >> 12) & 63];
        *ptr++ = base64[(tmp >> 6) & 63];
        *ptr++ = base64[(tmp >> 0) & 63];
        *ptr++ = '=';
        *ptr++ = '\n';
    }

    portMemCopy(ptr, bufferLen - (ptr - buffer), SPDM_PEM_END_CERTIFICATE,
                sizeof(SPDM_PEM_END_CERTIFICATE));
    ptr += sizeof(SPDM_PEM_END_CERTIFICATE);

    *bufferUsed = size;
    return NV_OK;
}

/* ------------------------ Public Functions ------------------------------- */

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
    uint16_t                 reqAsymAlgo;
    NvU8                    *pEncapCertChain;
    NvU32                    encapCertChainSize;

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

    // Get requester cert chain for mutual authentication process.
    if (spdmMutualAuthSupported(pGpu, pSpdm))
    {
        pEncapCertChain = NULL;
        encapCertChainSize = 0;
        status = spdmGetReqEncapCertificates_HAL(pGpu, pSpdm, &pEncapCertChain, &encapCertChainSize);

        if (status != NV_OK || pEncapCertChain == NULL || encapCertChainSize == 0)
        {
            status = NV_ERR_NOT_SUPPORTED;
            goto ErrorExit;
        }
    }

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

    capFlags = SPDM_GET_CAPABILITIES_REQUEST_FLAGS_CERT_CAP     |
                SPDM_GET_CAPABILITIES_REQUEST_FLAGS_ENCRYPT_CAP  |
                SPDM_GET_CAPABILITIES_REQUEST_FLAGS_MAC_CAP      |
                SPDM_GET_CAPABILITIES_REQUEST_FLAGS_KEY_EX_CAP   |
                SPDM_GET_CAPABILITIES_REQUEST_FLAGS_KEY_UPD_CAP  |
                SPDM_GET_CAPABILITIES_REQUEST_FLAGS_ENCAP_CAP    |
                SPDM_GET_CAPABILITIES_REQUEST_FLAGS_HBEAT_CAP;

    if (spdmMutualAuthSupported(pGpu, pSpdm))
    {
        capFlags |= SPDM_GET_CAPABILITIES_REQUEST_FLAGS_MUT_AUTH_CAP;
    }

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

    if (spdmMutualAuthSupported(pGpu, pSpdm))
    {
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
    }

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
    libspdm_set_msg_log_mode(pSpdm->pLibspdmContext, LIBSPDM_MSG_LOG_MODE_ENABLE);

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
        status = NV_ERR_GENERIC;
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

    // Now that the session has been properly established, cache a log of the entire transcript.
    pSpdm->transcriptLogSize = libspdm_get_msg_log_size(pSpdm->pLibspdmContext);
    if (pSpdm->transcriptLogSize > pSpdm->msgLogMaxSize ||
        (size_t)pSpdm->transcriptLogSize < libspdm_get_msg_log_size(pSpdm->pLibspdmContext))
    {
        // Something has gone quite wrong
        pSpdm->transcriptLogSize = 0;
        status                   = NV_ERR_INVALID_STATE;
        goto ErrorExit;
    }

    pSpdm->pTranscriptLog = (NvU8 *)portMemAllocNonPaged(pSpdm->transcriptLogSize);
    if (pSpdm->pTranscriptLog == NULL)
    {
        pSpdm->transcriptLogSize = 0;
        status                   = NV_ERR_NO_MEMORY;
        goto ErrorExit;
    }

    portMemCopy(pSpdm->pTranscriptLog, pSpdm->transcriptLogSize, pSpdm->pMsgLog, pSpdm->transcriptLogSize);

    // Clear the existing log, as it's no longer needed.
    libspdm_reset_msg_log(pSpdm->pLibspdmContext);

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

/*!
 * Forwards an application message to GPU via the encrypted SPDM messaging channel.
 * Application message must be a RM_SPDM_NV_CMD_TYPE message in proper RM_SPDM_NV_CMD format,
 * else Responder will reject the message.
 *
 * @param[in]      pGpu            GPU object pointer.
 * @param[in]      pSpdm           SPDM object pointer.
 * @param[in]      pRequest        Pointer to a buffer which stores application request message.
 * @param[in]      requestSize     The request buffer size.
 * @param[out]     pResponse       Pointer to a buffer which stores response message.
 * @param[in, out] pResponseSize   Pointer which holds size of response buffer on input, and
 *                                 stores size of received response on output.
 *
 * @return NV_OK if success, Error otherwise.
 */
NV_STATUS
spdmSendApplicationMessage_IMPL
(
    OBJGPU *pGpu,
    Spdm   *pSpdm,
    NvU8   *pRequest,
    NvU32   requestSize,
    NvU8   *pResponse,
    NvU32  *pResponseSize
)
{
    NV_STATUS status        = NV_OK;
    size_t    responseSizeT = 0;

    if (pRequest == NULL || pResponse == NULL || pResponseSize == NULL ||
        requestSize < sizeof(RM_SPDM_NV_CMD_HDR) || *pResponseSize < sizeof(RM_SPDM_NV_CMD_HDR))
    {
        return NV_ERR_INVALID_PARAMETER;
    }

    // Use a temporary size_t passed to libspdm to avoid size mismatch issues
    responseSizeT = *pResponseSize;
    CHECK_SPDM_STATUS(libspdm_send_receive_data(pSpdm->pLibspdmContext, &(pSpdm->sessionId), NV_TRUE,
                                                pRequest, requestSize, pResponse, &responseSizeT));

    // Check for truncation on conversion back to NvU32
    *pResponseSize = responseSizeT;
    if (*pResponseSize < responseSizeT)
    {
        return NV_ERR_OUT_OF_RANGE;
    }

ErrorExit:
    return status;
}

/*!
 * @brief  Control call function to retrieve the SPDM session establishment transcript.
 *
 * @return NV_OK if success, Error otherwise.
 */
NV_STATUS
subdeviceSpdmRetrieveTranscript_IMPL
(
    Subdevice                                            *pSubdevice,
    NV2080_CTRL_INTERNAL_SPDM_RETRIEVE_TRANSCRIPT_PARAMS *pSpdmRetrieveSessionTranscriptParams
)
{

    OBJGPU *pGpu  = GPU_RES_GET_GPU(pSubdevice);
    Spdm   *pSpdm = GPU_GET_SPDM(pGpu);

    if (pSpdm->pTranscriptLog == NULL || pSpdm->transcriptLogSize == 0)
    {
        return NV_ERR_INVALID_STATE;
    }

    if (sizeof(pSpdmRetrieveSessionTranscriptParams->transcript) < pSpdm->transcriptLogSize)
    {
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    portMemCopy(pSpdmRetrieveSessionTranscriptParams->transcript,
                sizeof(pSpdmRetrieveSessionTranscriptParams->transcript),
                pSpdm->pTranscriptLog, pSpdm->transcriptLogSize);

    pSpdmRetrieveSessionTranscriptParams->transcriptSize = pSpdm->transcriptLogSize;

    return NV_OK;
}

/*!
 * Setup responder certificate context
 * RM(requester) send GET_CERTIFICATE command to responer, responder return
 * all certificates in one buffer. This functions is used to parse buffer and
 * setup certificate context to manage individual certificate.
 *
 * @param[in]  pGpu               Pointer to GPU object.
 * @param[in]  pSpdm              Pointer to SPDM object.
 * @param[in]  pCertResponder     Pointer to certificate buffer
 * @param[in]  certResponderSize  The size of certificate buffer
 * @param[out] pCertCtx           Pointer to certificate context array
 * @param[out] pCertCount         Pointer to store responder certificate count
 *
 * @return  NV_OK if no error; otherwise return NV_ERR_XXXX
 */
NV_STATUS
spdmSetupResponderCertCtx_IMPL
(
    OBJGPU               *pGpu,
    Spdm                 *pSpdm,
    NvU8                 *pCertResponder,
    NvU64                 certResponderSize,
    void                 *pCertRespCtx,
    NvU32                *pCertCount
)
{
    NvU8                  *pCertStartSrc  = NULL;
    NvU8                  *pCertEndSrc    = NULL;
    uint32_t               base_hash_algo = 0;
    libspdm_context_t     *pContext       = NULL;
    NvU32                  certSize;
    NvU32                  certCount      = 0;
    NV_SPDM_CERT_CONTEXT  *pCertCtx       = NULL;
    NV_STATUS              status;

    if (pGpu == NULL || pSpdm == NULL || pCertResponder == NULL ||
        pCertRespCtx == NULL || pCertCount == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    pCertCtx = (NV_SPDM_CERT_CONTEXT *)pCertRespCtx;

    // Now, append the responder certificates to create the entire chain.
    pCertEndSrc = pCertResponder + certResponderSize - 1;

    pContext = (libspdm_context_t *)pSpdm->pLibspdmContext;
    base_hash_algo = pContext->connection_info.algorithm.base_hash_algo;

    // calculate each responder certification size and write to buffer
    pCertStartSrc = pCertResponder + sizeof(spdm_cert_chain_t) + libspdm_get_hash_size(base_hash_algo);

    while (pCertStartSrc < pCertEndSrc)
    {
        // Retrieve each responder cert and append to cert chain buffer.
        status = _calcX509CertSize(pCertStartSrc, pCertEndSrc, &certSize);

        if (status != NV_OK)
        {
            return NV_ERR_INVALID_DATA;
        }

        pCertCtx->pCert = pCertStartSrc;
        pCertCtx->certSize = certSize;
        certCount ++;

        if (certCount > NV_SPDM_RESPONDER_CERT_COUNT_MAX)
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        pCertStartSrc += certSize;
        pCertCtx++;
    }

    // validate final address
    if (pCertStartSrc - 1 != pCertEndSrc)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    *pCertCount = certCount;

    return NV_OK;
}

/*!
* This function builds the cert chain in DER format. It is assumed that
* the all the certificates are valid. Also it is assumed that there is a valid
* spdm session already established.
*
* @param [in]   pGpu               The pointer to GPUOBJ.
* @param [in]   pSpdm              The pointer to Spdm object.
* @param [in]   pCertRespCtx       The pointer to certificate context.
* @param [in]   certCountResp      The responder certificate count.
* @param [out]  pCertChainOut      The buffer to store cert chain in der format.
* @param [out]  pCertChainOutSize  The pointer to store cert chain size in byte.
*/
NV_STATUS
spdmBuildCertChainDer_IMPL
(
    OBJGPU               *pGpu,
    Spdm                 *pSpdm,
    void                 *pCertRespCtx,
    NvU32                 certCountResp,
    NvU8                 *pCertChainOut,
    size_t               *pCertChainOutSize
)
{
    NV_STATUS             status;
    NvU32                 certCountReq;
    NvU32                 certId;
    NvU64                 remainingOutBufferSize = 0;
    NvU64                 certSize;
    NvU64                 certTotalSize          = 0;
    NvU8                 *pCertStartDest         = NULL;
    NV_SPDM_CERT_CONTEXT *pCertCtx               = NULL;

    if (pGpu == NULL || pSpdm == NULL || pCertRespCtx == NULL ||
        pCertChainOut == NULL || pCertChainOutSize == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (certCountResp == 0)
    {
        return NV_OK;
    }

    pCertCtx = (NV_SPDM_CERT_CONTEXT *)pCertRespCtx;

    remainingOutBufferSize = *pCertChainOutSize;
    // Get each requester cert and write to buffer
    status = spdmGetRequesterCertificateCount_HAL(pGpu, pSpdm, &certCountReq);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "spdmGetRequesterCertificateCount failed !!! \n");
        return NV_ERR_NOT_SUPPORTED;
    }

    //
    // Note : Need to write cert to buffer in order.
    // L1_CERT -> L2_CERT -> .... -> LEAF_END_CERT
    // Currently for GH100, we have L1 cert(root)->L2 cert->L3(GH100) cert->IK cert-> AK cert(leaf).
    // Requester has L1 cert(root)->L2 cert->L3(GH100) cert
    // Responder has IK cert-> AK cert(leaf)
    // We assume the responder certification in buffer is placed as above order
    //
    certId = NV_SPDM_REQ_L1_CERTIFICATE_ID;
    pCertStartDest = pCertChainOut;

    while(certCountReq--)
    {
        certSize = remainingOutBufferSize;
        status = spdmGetIndividualCertificate_HAL(pGpu, pSpdm, certId, NV_TRUE, pCertStartDest, &certSize);

        if (status != NV_OK || certSize > remainingOutBufferSize)
        {
            NV_PRINTF(LEVEL_ERROR, "spdmGetIndividualCertificate() failed !!! \n");
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        certId ++;
        remainingOutBufferSize -= certSize;
        pCertStartDest += certSize;
        certTotalSize += certSize;
    }

    // Now, append the responder certificates to create the entire chain.
    while (certCountResp--)
    {
        portMemCopy(pCertStartDest, remainingOutBufferSize, pCertCtx->pCert, pCertCtx->certSize);

        pCertStartDest += pCertCtx->certSize;
        remainingOutBufferSize -= pCertCtx->certSize;
        certTotalSize += pCertCtx->certSize;

        pCertCtx++;
    }

    status = spdmGetIndividualCertificate_HAL(pGpu, pSpdm, NV_SPDM_REQ_L1_CERTIFICATE_ID, NV_TRUE, NULL, &certSize);

    // Now, validate that the certificate chain is correctly signed.
    if (!libspdm_x509_verify_cert_chain(pCertChainOut, (size_t)certSize,
                                        pCertChainOut + certSize,
                                        certTotalSize - certSize))
    {
        NV_PRINTF(LEVEL_ERROR, "libspdm_x509_verify_cert_chain() failed !!! \n");

        return NV_ERR_INVALID_DATA;
    }

    return NV_OK;
}

/*!
* The function that first converts the IK and AK certificates from DER to
* PEM format. Then it builds the cert chain in PEM format. It is assumed that
* the all the certificates are valid. Also it is assumed that there is a valid
* spdm session already established.
*
*  Currently The responder context only has 2 certificates from all chips, => [IK , AK]
*  And RM needs to store PEM cert ==> [AK -> IK -> L3 -> L2 ->L1]
*
* @param [in]   pGpu               The pointer to GPUOBJ.
* @param [in]   pSpdm              The pointer to Spdm object.
* @param [in]   pCertRespCtx       The pointer to certificate context.
* @param [in]   certCountResp      The responder certificate count.
* @param [out]  pCertChainOut      The buffer to store cert chain in pem format.
* @param [out]  pCertChainOutSize  The pointer to store cert chain size in byte.
*/
NV_STATUS
spdmBuildCertChainPem_IMPL
(
    OBJGPU               *pGpu,
    Spdm                 *pSpdm,
    void                 *pCertRespCtx,
    NvU32                 certCountResp,
    NvU8                 *pCertChainOut,
    size_t               *pCertChainOutSize
)
{
    NvU64                 remainingOutBufferSize;
    NvU64                 certOutSize;
    NvU32                 i;
    NvU32                 certCountReq;
    NvU64                 certTotalSize           = 0;
    NvU32                 certId;
    NvU32                 certCtxIdx;
    NV_SPDM_CERT_CONTEXT *pCertCtx                = NULL;
    NV_STATUS             status;

    if (pGpu == NULL || pSpdm == NULL || pCertRespCtx == NULL ||
        pCertChainOut == NULL || pCertChainOutSize == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (certCountResp == 0)
    {
        return NV_OK;
    }

    pCertCtx = (NV_SPDM_CERT_CONTEXT *)pCertRespCtx;

    remainingOutBufferSize = *pCertChainOutSize;

    //
    // Note : Need to write cert to buffer in revert order.
    // LEAF_END_CERT -> ... -> L2_CERT -> L1_CERT
    // Currently for GH100/GB100, we have L1 cert(root)->L2 cert->L3(GH100) cert->IK cert-> AK cert(leaf).
    // Requester has L1 cert(root)->L2 cert->L3(GH100) cert
    // The cert in pCertCtx is [IK cert, AK cert]
    //

    // store revert order to buffer
    certCtxIdx = certCountResp - 1;
    for (i = 0; i < certCountResp; i++)
    {
        //
        // Convert DER to PEM and write certificate to the output buffer
        //
        status = pem_write_buffer(pCertCtx[certCtxIdx].pCert, pCertCtx[certCtxIdx].certSize, pCertChainOut,
                                  remainingOutBufferSize, &certOutSize);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "pem_write_buffer() failed \n");
            return status;
        }

        //
        // Keep track how much space we have left in the output buffer
        // and where the next certificate should start.
        // Clear the last byte (NULL).
        //
        certOutSize -= 1;
        remainingOutBufferSize -= certOutSize;
        pCertChainOut          += certOutSize;
        certTotalSize          += certOutSize;
        certCtxIdx--;
    }

    // Get each requester cert and write to buffer
    status = spdmGetRequesterCertificateCount_HAL(pGpu, pSpdm, &certCountReq);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "spdmGetRequesterCertificateCount_HAL() failed \n");
        return NV_ERR_NOT_SUPPORTED;
    }

    certId = NV_SPDM_REQ_L3_CERTIFICATE_ID;

    for (i = 0 ; i < certCountReq; i++)
    {
        certOutSize = remainingOutBufferSize;
        status = spdmGetIndividualCertificate_HAL(pGpu, pSpdm, certId, NV_FALSE, pCertChainOut, &certOutSize);

        if (status != NV_OK || remainingOutBufferSize < certOutSize)
        {
            NV_PRINTF(LEVEL_ERROR, "spdmGetIndividualCertificate_HAL() failed \n");
            return NV_ERR_INVALID_DATA;
        }

        // No need to clear the last byte (NULL) in RM while using BINDATA.
        remainingOutBufferSize -= certOutSize;
        pCertChainOut          += certOutSize;
        certTotalSize          += certOutSize;
        certId--;
    }

    *pCertChainOutSize = certTotalSize;

    return NV_OK;
}

/*!
  * The function is used to verify SPDM session requester.
  *
  * @param [in]   pGpu              The pointer to GPUOBJ.
  * @param [in]   pSpdm             The pointer to Spdm object.
  * @param [in]   requesterId       The id represents the session establishment requester.
  *
  * @return  NV_OK if requester id is valid; otherwise return NV_ERR_XXXX
  */
NV_STATUS
spdmCheckRequesterIdValid_IMPL
(
    OBJGPU   *pGpu,
    Spdm     *pSpdm,
    NvU32     requesterId
)
{
    switch (requesterId)
    {
       case NV_SPDM_REQUESTER_ID_CONF_COMPUTE:
           return NV_OK;
    }

    NV_PRINTF(LEVEL_ERROR, "Error, invalid NV SPDM requester id(0x%x) !!!!\n", requesterId);
    return NV_ERR_NOT_SUPPORTED;
}

/*!
 * The function is used to establish SPDM session
 *
 * @param [in]   pGpu              The pointer to GPUOBJ.
 * @param [in]   pSpdm             The pointer to Spdm object.
 * @param [in]   requesterId       The id represents the session establishment requester.
 *
 * @return  NV_OK if no error; otherwise return NV_ERR_XXXX
 */
NV_STATUS
spdmEstablishSession_IMPL
(
    OBJGPU   *pGpu,
    Spdm     *pSpdm,
    NvU32     requesterId
)
{
    NV_STATUS status = NV_OK;

    status = spdmCheckRequesterIdValid(pGpu, pSpdm, requesterId);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, status == NV_OK, status);

    //
    // Initialize SPDM session between Guest RM and SPDM Responder on GPU.
    // The session lifetime will track Confidential Compute object state lifetime.
    //
    status = spdmContextInit(pGpu, pSpdm);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, status == NV_OK, status);

    status = spdmStart(pGpu, pSpdm);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, status == NV_OK, status);

    pSpdm->nvSpdmRequesterId = requesterId;

    return NV_OK;
}
