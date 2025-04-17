/*
 * SPDX-FileCopyrightText: Copyright (c) 2024-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * Provides the implementation for all GB100 SPDM certificate HAL interfaces.
 */

/* ------------------------ Includes --------------------------------------- */
#include "nvRmReg.h"
#include "gpu/spdm/spdm.h"
#include "gpu/spdm/libspdm_includes.h"
#include "spdm/rmspdmvendordef.h"
#include "flcnretval.h"

/* ------------------------ Macros ----------------------------------------- */
#define NV_GB100_SPDM_REQUESTER_CERT_COUNT  (3)

// TODO: CONFCOMP-1965 - Add GB100 Mutual Auth Cert

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
spdmGetRequesterCertificateCount_GB100
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

    *pCertCount = NV_GB100_SPDM_REQUESTER_CERT_COUNT;

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
spdmGetIndividualCertificate_GB100
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
