/*
 * SPDX-FileCopyrightText: Copyright (c) 2023-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * Provides the implementation for all GH100 SPDM certificate HAL interfaces.
 */

/* ------------------------ Includes --------------------------------------- */
#include "nvRmReg.h"
#include "gpu/spdm/spdm.h"
#include "gpu/spdm/libspdm_includes.h"
#include "spdm/rmspdmvendordef.h"
#include "flcnretval.h"

/* ------------------------ Macros ----------------------------------------- */
#define NV_GH100_SPDM_REQUESTER_CERT_COUNT  (3)

//
//TODO : Need to generate individual encapsulated certification chain.
//

static NvU8 SPDM_REQ_ENCAP_CERTIFICATE_DER[NV_SPDM_ENCAP_CERT_SIZE_IN_BYTE] =
{
    0x30, 0x82, 0x02, 0x3e, 0x30, 0x82, 0x01, 0xc4, 0xa0, 0x03, 0x02, 0x01, 0x02, 0x02, 0x09, 0x00,
    0xff, 0xe0, 0xbc, 0xe4, 0x95, 0xe5, 0x9e, 0xe2, 0x30, 0x0a, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce,
    0x3d, 0x04, 0x03, 0x03, 0x30, 0x5d, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x06, 0x13,
    0x02, 0x55, 0x53, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x08, 0x0c, 0x02, 0x43, 0x41,
    0x31, 0x14, 0x30, 0x12, 0x06, 0x03, 0x55, 0x04, 0x07, 0x0c, 0x0b, 0x53, 0x61, 0x6e, 0x74, 0x61,
    0x20, 0x43, 0x6c, 0x61, 0x72, 0x61, 0x31, 0x15, 0x30, 0x13, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x0c,
    0x0c, 0x4e, 0x56, 0x49, 0x44, 0x49, 0x41, 0x20, 0x43, 0x6f, 0x72, 0x70, 0x2e, 0x31, 0x14, 0x30,
    0x12, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0c, 0x0b, 0x4e, 0x56, 0x49, 0x44, 0x49, 0x41, 0x20, 0x43,
    0x6f, 0x72, 0x70, 0x30, 0x1e, 0x17, 0x0d, 0x32, 0x33, 0x30, 0x38, 0x32, 0x35, 0x32, 0x33, 0x31,
    0x33, 0x30, 0x32, 0x5a, 0x17, 0x0d, 0x32, 0x34, 0x30, 0x38, 0x31, 0x39, 0x32, 0x33, 0x31, 0x33,
    0x30, 0x32, 0x5a, 0x30, 0x5d, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x06, 0x13, 0x02,
    0x55, 0x53, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x08, 0x0c, 0x02, 0x43, 0x41, 0x31,
    0x14, 0x30, 0x12, 0x06, 0x03, 0x55, 0x04, 0x07, 0x0c, 0x0b, 0x53, 0x61, 0x6e, 0x74, 0x61, 0x20,
    0x43, 0x6c, 0x61, 0x72, 0x61, 0x31, 0x15, 0x30, 0x13, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x0c, 0x0c,
    0x4e, 0x56, 0x49, 0x44, 0x49, 0x41, 0x20, 0x43, 0x6f, 0x72, 0x70, 0x2e, 0x31, 0x14, 0x30, 0x12,
    0x06, 0x03, 0x55, 0x04, 0x03, 0x0c, 0x0b, 0x4e, 0x56, 0x49, 0x44, 0x49, 0x41, 0x20, 0x43, 0x6f,
    0x72, 0x70, 0x30, 0x76, 0x30, 0x10, 0x06, 0x07, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x02, 0x01, 0x06,
    0x05, 0x2b, 0x81, 0x04, 0x00, 0x22, 0x03, 0x62, 0x00, 0x04, 0x70, 0x6b, 0x5a, 0x92, 0xed, 0x0a,
    0x9c, 0x5a, 0x55, 0xc0, 0x58, 0xf9, 0x81, 0x86, 0xa5, 0x64, 0x3f, 0x44, 0x00, 0xe2, 0x18, 0x4e,
    0x99, 0xc0, 0x4c, 0xd7, 0x31, 0x73, 0x24, 0x23, 0xf4, 0x0a, 0x39, 0x0e, 0x91, 0x52, 0x32, 0x2f,
    0x50, 0x58, 0xe9, 0xc0, 0x83, 0xf4, 0x6d, 0x3c, 0x93, 0x5c, 0x98, 0x18, 0x14, 0x47, 0xc1, 0x0d,
    0x27, 0xa8, 0x36, 0xe8, 0x06, 0xcb, 0xe5, 0x3d, 0x08, 0x85, 0x08, 0x1b, 0x52, 0xcf, 0xa3, 0x82,
    0x42, 0xe8, 0xa7, 0xe7, 0x8b, 0x28, 0x0c, 0xff, 0xb1, 0xc8, 0x8f, 0x55, 0x85, 0xdf, 0x28, 0x5a,
    0xf3, 0xfc, 0x58, 0xed, 0xa6, 0x7a, 0xec, 0x14, 0xff, 0xc8, 0xa3, 0x50, 0x30, 0x4e, 0x30, 0x1d,
    0x06, 0x03, 0x55, 0x1d, 0x0e, 0x04, 0x16, 0x04, 0x14, 0xa9, 0xa5, 0x22, 0xdc, 0x5d, 0xfa, 0x59,
    0xb0, 0xcb, 0x52, 0xad, 0xb1, 0xfa, 0x85, 0x29, 0x88, 0x57, 0xd8, 0xdb, 0x5a, 0x30, 0x1f, 0x06,
    0x03, 0x55, 0x1d, 0x23, 0x04, 0x18, 0x30, 0x16, 0x80, 0x14, 0xa9, 0xa5, 0x22, 0xdc, 0x5d, 0xfa,
    0x59, 0xb0, 0xcb, 0x52, 0xad, 0xb1, 0xfa, 0x85, 0x29, 0x88, 0x57, 0xd8, 0xdb, 0x5a, 0x30, 0x0c,
    0x06, 0x03, 0x55, 0x1d, 0x13, 0x04, 0x05, 0x30, 0x03, 0x01, 0x01, 0xff, 0x30, 0x0a, 0x06, 0x08,
    0x2a, 0x86, 0x48, 0xce, 0x3d, 0x04, 0x03, 0x03, 0x03, 0x68, 0x00, 0x30, 0x65, 0x02, 0x31, 0x00,
    0x84, 0x63, 0x87, 0x79, 0x88, 0x73, 0x70, 0xe9, 0x95, 0x15, 0x55, 0xd1, 0xc6, 0x51, 0x3c, 0xcf,
    0x76, 0x6a, 0x20, 0x60, 0xc4, 0x73, 0xe4, 0xb8, 0xc1, 0x98, 0xc8, 0xf5, 0x08, 0xea, 0x10, 0xe8,
    0xb5, 0xd4, 0x08, 0xda, 0x10, 0xfd, 0x17, 0x75, 0xc4, 0xf5, 0x90, 0xcf, 0x11, 0x5f, 0x53, 0x87,
    0x02, 0x30, 0x07, 0xed, 0x77, 0x63, 0xcf, 0xdd, 0xfb, 0x29, 0x3a, 0xec, 0xf3, 0x3b, 0x00, 0x8e,
    0xbc, 0xdf, 0x9c, 0xc3, 0x64, 0xab, 0x3c, 0x29, 0x7a, 0x32, 0x50, 0xd7, 0xb0, 0x78, 0x74, 0xe4,
    0xd1, 0xea, 0x43, 0xdb, 0xc8, 0x9d, 0xd0, 0xc2, 0xa5, 0x5e, 0x8f, 0x90, 0x4d, 0xb5, 0x1c, 0xe7,
    0xd2, 0x38
};

/* ------------------------ Static Functions ------------------------------- */

/* ------------------------ Public Functions ------------------------------- */
/*!
 *  Return requester certificate count
 *
 * @param[in]  pGpu          Pointer to GPU object.
 * @param[in]  pSpdm         Pointer to SPDM object.
 * @param[out] pCertCount    The pointer to store requester certificate count
 *
 * @return     Return NV_OK if no error; otherwise return NV_ERR_XXX
 */
NV_STATUS
spdmGetRequesterCertificateCount_GH100
(
    OBJGPU *pGpu,
    Spdm   *pSpdm,
    NvU32  *pCertCount
)
{
    if (pCertCount == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    *pCertCount = NV_GH100_SPDM_REQUESTER_CERT_COUNT;

    return NV_OK;
}

/*!
 *  To return individual certificate.
 *
 * @param[in]     pGpu        Pointer to GPU object.
 * @param[in]     pSpdm       Pointer to SPDM object.
 * @param[in]     certId      The certificate id
 * @param[in]     bDerFormat  To indicate return cert format, DER or PEM.
 * @param[out]    pCert       Pointer to return certificate
 * @param[in/out] pCertSize   As input, this pointer represent the size of pCert buffer;
 *                            as output, this pointer contain the size of return certificate.
 *
 * @return     Return NV_OK if no error; otherwise return NV_ERR_XXX
 */
NV_STATUS
spdmGetIndividualCertificate_GH100
(
    OBJGPU *pGpu,
    Spdm   *pSpdm,
    NvU32   certId,
    NvBool  bDerFormat,
    void   *pCert,
    NvU64  *pCertSize
)
{
    NV_STATUS              status;
    NvU64                  certSize;
    NvBool                 bNeedCopy   = NV_FALSE;
    const BINDATA_ARCHIVE *pBinArchive = NULL;
    const BINDATA_STORAGE *pBinStorage = NULL;

    if (pCertSize == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    bNeedCopy = (pCert == NULL ? NV_FALSE : NV_TRUE);

    switch(certId)
    {
       case NV_SPDM_REQ_L1_CERTIFICATE_ID:
           pBinArchive = spdmGetBinArchiveL1Certificate_IMPL(pSpdm);
       break;

       case NV_SPDM_REQ_L2_CERTIFICATE_ID:
            pBinArchive = spdmGetBinArchiveIndividualL2Certificate_HAL(pSpdm);
       break;

       case NV_SPDM_REQ_L3_CERTIFICATE_ID:
            pBinArchive = spdmGetBinArchiveIndividualL3Certificate_HAL(pSpdm);
       break;

       default:
           *pCertSize = 0;
           return NV_ERR_INVALID_ARGUMENT;
    }

    if (pBinArchive == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    pBinStorage = bDerFormat ?
                  (BINDATA_STORAGE *)bindataArchiveGetStorage(pBinArchive, BINDATA_LABEL_CERTIFICATE_DER) :
                  (BINDATA_STORAGE *)bindataArchiveGetStorage(pBinArchive, BINDATA_LABEL_CERTIFICATE_PEM);

    if (pBinStorage == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    certSize = bindataGetBufferSize(pBinStorage);

    if (bNeedCopy)
    {
        if (*pCertSize < certSize)
        {
            return NV_ERR_BUFFER_TOO_SMALL;
        }

        status = bindataWriteToBuffer(pBinStorage, pCert, certSize);

        if (status != NV_OK)
        {
            *pCertSize = 0;
            return status;
        }
    }

    *pCertSize = certSize;
    return NV_OK;
}

/*!
 *  To get responder certificate.
 *
 * @param[in]     pGpu        Pointer to GPU object.
 * @param[in]     pSpdm       Pointer to SPDM object.
 *
 * @return     Return NV_OK if no error; otherwise return NV_ERR_XXX
 */
NV_STATUS
spdmGetCertificates_GH100
(
    OBJGPU *pGpu,
    Spdm   *pSpdm
)
{
    NV_STATUS             status = NV_OK;
    NvU8                 *pGpuCerts               = NULL;
    size_t                gpuCertsSize            = 0;
    NvU8                 *pDerCertChain           = NULL;
    size_t                derCertChainSize        = 0;
    NvU32                 responderCertCount;
    NV_SPDM_CERT_CONTEXT  responderCertCtx[NV_SPDM_RESPONDER_CERT_COUNT_MAX];

    // Don't allow certificate retrieval after session is established.
    if (pSpdm->pLibspdmContext == NULL || pSpdm->sessionId != INVALID_SESSION_ID)
    {
        return NV_ERR_INVALID_STATE;
    }

    // Allocate buffer for certificates.
    gpuCertsSize                    = LIBSPDM_MAX_CERT_CHAIN_SIZE;
    pGpuCerts                       = portMemAllocNonPaged(gpuCertsSize);
    derCertChainSize                = SPDM_MAX_ENCODED_CERT_CHAIN_SIZE;
    pDerCertChain                   = portMemAllocNonPaged(derCertChainSize);
    pSpdm->attestationCertChainSize = SPDM_MAX_ENCODED_CERT_CHAIN_SIZE;
    pSpdm->pAttestationCertChain    = portMemAllocNonPaged(pSpdm->attestationCertChainSize);

    // Ensure data was properly allocated.
    if (pGpuCerts == NULL || pDerCertChain == NULL || pSpdm->pAttestationCertChain == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        goto ErrorExit;
    }

    portMemSet(pGpuCerts, 0, gpuCertsSize);
    portMemSet(pDerCertChain, 0, derCertChainSize);
    portMemSet(pSpdm->pAttestationCertChain, 0, pSpdm->attestationCertChainSize);

    // We fetch Attestation cert chain on Hopper and Blackwell.
    CHECK_SPDM_STATUS(libspdm_get_certificate(pSpdm->pLibspdmContext, NULL,
                                              SPDM_CERT_DEFAULT_SLOT_ID,
                                              &gpuCertsSize, pGpuCerts));

    status = spdmSetupResponderCertCtx(pGpu, pSpdm, pGpuCerts, gpuCertsSize,
                                      (void *)responderCertCtx, &responderCertCount);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "spdmRetrieveResponderCert() failed !!! \n");
        goto ErrorExit;
    }

    //
    // Skip over the certificate chain size, reserved size and the root hash
    // pSpdmCertChainBufferEnd represents last valid byte for cert buffer.
    //
    status = spdmBuildCertChainDer(pGpu, pSpdm, (void *)responderCertCtx,
                                   responderCertCount, pDerCertChain, &derCertChainSize);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "spdmBuildCertChainDer() failed !!! \n");
        goto ErrorExit;
    }

    //
    // Now that the cert chain is valid, retrieve the cert chain in PEM format,
    // as the Verifier can only parse PEM format.
    //
    status = spdmBuildCertChainPem(pGpu, pSpdm, (void *)responderCertCtx, responderCertCount,
                                   pSpdm->pAttestationCertChain,
                                   &pSpdm->attestationCertChainSize);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "spdmBuildCertChainPem() failed !!! \n");
        goto ErrorExit;
    }

ErrorExit:
    //
    // In both success and failure we need to free these allocated buffers.
    // portMemFree() will handle if they are NULL. On success, keep
    // the local pAttestationCertChain buffer.
    //
    portMemFree(pGpuCerts);
    portMemFree(pDerCertChain);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "SPDM failure most likely due to missing crypto implementation.\n");
        NV_PRINTF(LEVEL_ERROR, "Are the LKCA modules properly loaded?\n");

        // portMemFree() handles NULL.
        portMemFree(pSpdm->pAttestationCertChain);
        pSpdm->pAttestationCertChain    = NULL;
        pSpdm->attestationCertChainSize = 0;
    }

    return status;
}

NV_STATUS
spdmGetCertChains_GH100
(
    OBJGPU *pGpu,
    Spdm   *pSpdm,
    void   *pKeyExCertChain,
    NvU32  *pKeyExCertChainSize,
    void   *pAttestationCertChain,
    NvU32  *pAttestationCertChainSize
)
{
    if (pAttestationCertChain == NULL ||
        pAttestationCertChainSize == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Check that we're in a valid state.
    if (pSpdm->pLibspdmContext == NULL || pSpdm->pAttestationCertChain == NULL ||
        pSpdm->attestationCertChainSize == 0)
    {
        return NV_ERR_NOT_READY;
    }

    // We only support Attestation certificates on Hopper.
    if (pKeyExCertChainSize != NULL)
    {
        pKeyExCertChainSize = 0;
    }

    if (*pAttestationCertChainSize < pSpdm->attestationCertChainSize)
    {
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    portMemCopy(pAttestationCertChain, *pAttestationCertChainSize,
                pSpdm->pAttestationCertChain, pSpdm->attestationCertChainSize);
    *pAttestationCertChainSize = pSpdm->attestationCertChainSize;

    return NV_OK;
}

/*!
 @param pGpu                : The pointer to GPU object
 @param pSpdm               : The pointer to SPDM object
 @parsm pEncapCertChain     : The pointer to store return certification address
 @param pEncapCertChainSize : The pointer to store rrtun certification size

 @return Return NV-OK if no error.
*/
NV_STATUS
spdmGetReqEncapCertificates_GH100
(
    OBJGPU *pGpu,
    Spdm   *pSpdm,
    NvU8  **pEncapCertChain,
    NvU32  *pEncapCertChainSize
)
{
    if (*pEncapCertChain != NULL || pEncapCertChainSize == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    *pEncapCertChain = (NvU8 *)SPDM_REQ_ENCAP_CERTIFICATE_DER;
    *pEncapCertChainSize = sizeof(SPDM_REQ_ENCAP_CERTIFICATE_DER);

    return NV_OK;
}
