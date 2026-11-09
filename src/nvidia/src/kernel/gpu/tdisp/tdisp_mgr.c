/*
 * SPDX-FileCopyrightText: Copyright (c) 2025-2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 *      TDISP_MGR Object Module                                    *
 *                                                                          *
 \**************************************************************************/

#define NVOC_TDISP_MGR_H_PRIVATE_ACCESS_ALLOWED

#include "gpu/tdisp/tdisp_mgr.h"
#include "gpu/gpu.h"
#include "gpu/mem_sys/kern_mem_sys.h"
#include "gsp/gsp_proxy_reg.h"
#include "nvrm_registry.h"
#include "gpu/spdm/spdm.h"
#include "os/os.h"
#include "spdm/rmspdmtransport.h"

static NvBool
_tdispmgrIsTdiInRunState
(
    OBJGPU *pGpu
)
{
    NvU32 dvsecTdiState = 0;

    return (gpuReadBusConfigReg_HAL(pGpu, NV_TDISP_DVSEC0_HEADER_2_AND_GENERAL, &dvsecTdiState) == NV_OK) &&
           (NV_TDISP_DVSEC0_TDI_STATE(dvsecTdiState) == NV_TDISP_DVSEC0_TDI_STATE_RUN);
}

NV_STATUS
tdispmgrConstructEngine_IMPL
(
    OBJGPU        *pGpu,
    TdispMgr      *pTdispMgr,
    ENGDESCRIPTOR  engDesc
)
{
    NvBool bTdispEnabled = NV_FALSE;
    NvBool bTdispOverrideEnabled = NV_FALSE;
    NvBool bTdispQualModeEnabled = NV_FALSE;
    NvBool bTdispIdeTrafficDisabled = NV_FALSE;
    NvU32 data = 0;
    pTdispMgr->gspProxyRegkeys = 0;
    pTdispMgr->bRunState = NV_FALSE;


    if (!pTdispMgr->getProperty(pTdispMgr, PDB_PROP_TDISPMGR_TDISP_SUPPORTED))
    {
        NV_PRINTF(LEVEL_NOTICE, "TDISP is disabled: unsupported platform\n");
        return NV_OK;
    }
    if (!gpuIsCCEnabledInHw_HAL(pGpu))
    {
        NV_PRINTF(LEVEL_NOTICE, "TDISP disabled: GPU does not support TDISP - CC not enabled in HW\n");
        return NV_OK;
    }
    bTdispEnabled = osTdispIsEnabled(pGpu);

    if (!bTdispEnabled)
    {
        NV_PRINTF(LEVEL_NOTICE, "TDISP disabled: GPU is not TDISP supported\n");
        return NV_OK;
    }

    if (bTdispOverrideEnabled)
    {
        pTdispMgr->gspProxyRegkeys |= DRF_DEF(GSP, _PROXY_REG, _TDISP, _ENABLE);
        // Check if QUAL mode is enabled in the TDISP regkey
        if (bTdispQualModeEnabled)
        {
            NV_PRINTF(LEVEL_NOTICE, "TDISP Qual mode enabled via regkey override\n");
            pTdispMgr->setProperty(pTdispMgr, PDB_PROP_TDISPMGR_TDISP_QUAL_MODE_ENABLED, NV_TRUE);
            pTdispMgr->gspProxyRegkeys |= DRF_DEF(GSP, _PROXY_REG, _TDISP_ENABLE_QUAL_MODE, _YES);
        }
        else
        {
            NV_PRINTF(LEVEL_INFO, "TDISP Qual mode is disabled\n");
            pTdispMgr->setProperty(pTdispMgr, PDB_PROP_TDISPMGR_TDISP_QUAL_MODE_ENABLED, NV_FALSE);
            pTdispMgr->gspProxyRegkeys |= DRF_DEF(GSP, _PROXY_REG, _TDISP_ENABLE_QUAL_MODE, _NO);
        }
        // Check if IDE traffic is disabled in TDISP regkey
        if (bTdispIdeTrafficDisabled)
        {
            NV_PRINTF(LEVEL_NOTICE, "TDISP IDE traffic is disabled via regkey override.\n");
            pTdispMgr->setProperty(pTdispMgr, PDB_PROP_TDISPMGR_TDISP_IDE_TRAFFIC_DISABLED, NV_TRUE);
            pTdispMgr->gspProxyRegkeys |= DRF_DEF(GSP, _PROXY_REG, _TDISP_DISABLE_IDE_TRAFFIC, _YES);
        }
        else
        {
            NV_PRINTF(LEVEL_INFO, "TDISP IDE traffic is enabled\n");
            pTdispMgr->setProperty(pTdispMgr, PDB_PROP_TDISPMGR_TDISP_IDE_TRAFFIC_DISABLED, NV_FALSE);
            pTdispMgr->gspProxyRegkeys |= DRF_DEF(GSP, _PROXY_REG, _TDISP_DISABLE_IDE_TRAFFIC, _NO);
        }
    }

    if ((osReadRegistryDword(pGpu, NV_REG_STR_RM_TDISP_SKIP_BAR1_VALIDATION, &data) == NV_OK) &&
        (data == NV_REG_STR_RM_TDISP_SKIP_BAR1_VALIDATION_YES))
    {
        {
            NV_PRINTF(LEVEL_NOTICE, "TDISP BAR1 validation skipped via regkey override\n");
            pTdispMgr->setProperty(pTdispMgr,
                PDB_PROP_TDISPMGR_TDISP_BAR1_VALIDATION_SKIPPED, NV_TRUE);
            pTdispMgr->gspProxyRegkeys |=
                DRF_DEF(GSP, _PROXY_REG, _TDISP_SKIP_BAR1_VALIDATION, _YES);
        }
    }

    NV_PRINTF(LEVEL_NOTICE, "TDISP enabled\n");
    pTdispMgr->setProperty(pTdispMgr, PDB_PROP_TDISPMGR_TDISP_ENABLED, NV_TRUE);

    {
        if (_tdispmgrIsTdiInRunState(pGpu))
        {
            // Driver reload loses TdispMgr state; DVSEC RUN means TSM accept completed.
            pTdispMgr->bRunState = NV_TRUE;
        }
    }

    //
    // Enable SPDM on Kernel-RM when TDISP is enabled.
    // SPDM session is required for TDISP secure boot flow.
    //
    {
        Spdm *pSpdm = GPU_GET_SPDM(pGpu);
        if (pSpdm != NULL)
        {
            pSpdm->setProperty(pSpdm, PDB_PROP_SPDM_ENABLED, NV_TRUE);
        }
        else
        {
            NV_PRINTF(LEVEL_ERROR, "SPDM object is NULL; TDISP cannot be enabled.\n");
            return NV_ERR_OBJECT_NOT_FOUND;
        }
    }

    return NV_OK;
}

NvBool
tdispmgrIsGpuTrusted_KERNEL
(
    OBJGPU *pGpu,
    TdispMgr *pTdispMgr
)
{
    return pTdispMgr->bRunState;
}

static NvBool
_tdispmgrIsTsmFlow
(
    OBJGPU *pGpu,
    TdispMgr *pTdispMgr
)
{
    if ((pTdispMgr == NULL) ||
        !pTdispMgr->getProperty(pTdispMgr, PDB_PROP_TDISPMGR_TDISP_ENABLED))
    {
        return NV_FALSE;
    }

    PORT_UNREFERENCED_VARIABLE(pGpu);

    return NV_TRUE;
}

NvBool
tdispmgrIsTsmFspOnlyBoot_KERNEL
(
    OBJGPU *pGpu,
    TdispMgr *pTdispMgr
)
{
    return _tdispmgrIsTsmFlow(pGpu, pTdispMgr) && !pTdispMgr->bRunState;
}

NvBool
tdispmgrIsTsmGspRmBoot_KERNEL
(
    OBJGPU *pGpu,
    TdispMgr *pTdispMgr
)
{
    return _tdispmgrIsTsmFlow(pGpu, pTdispMgr) && pTdispMgr->bRunState;
}

/*!
 * @brief Check if TDISP IDE traffic is disabled
 */
NvBool
tdispmgrIsTdispIdeTrafficDisabled_IMPL
(
    OBJGPU *pGpu,
    TdispMgr *pTdispMgr
)
{
    if (pTdispMgr != NULL)
    {
        return pTdispMgr->getProperty(pTdispMgr, PDB_PROP_TDISPMGR_TDISP_IDE_TRAFFIC_DISABLED);
    }
    return NV_FALSE;
}

/*!
 * @brief Check if TDISP BAR1 validation is skipped
 */
NvBool
tdispmgrIsTdispBar1ValidationSkipped_IMPL
(
    OBJGPU *pGpu,
    TdispMgr *pTdispMgr
)
{
    if (pTdispMgr != NULL)
    {
        return pTdispMgr->getProperty(pTdispMgr,
            PDB_PROP_TDISPMGR_TDISP_BAR1_VALIDATION_SKIPPED);
    }

    return NV_FALSE;
}

/*!
 * @brief Check if TDISP QUAL mode is enabled
 */
NvBool
tdispmgrIsTdispQualModeEnabled_IMPL
(
    OBJGPU *pGpu,
    TdispMgr *pTdispMgr
)
{
    if (pTdispMgr != NULL)
    {
        return pTdispMgr->getProperty(pTdispMgr, PDB_PROP_TDISPMGR_TDISP_QUAL_MODE_ENABLED);
    }
    return NV_FALSE;
}

// Chain is ordered L1(root) -> L2 -> L3 -> L4(IK) -> L5(AK, leaf).
#define NV_TDISP_DEVICE_CERT_L4_INDEX  3

//
// The certificate serial number carries the PDI. Byte 0 differs between the two
// chains and is not part of the PDI, so it is excluded from the comparison.
//
#define NV_TDISP_PDI_OFFSET       1
#define NV_TDISP_PDI_SIZE         8

#define NV_TDISP_DEVICE_CERT_CHAIN_SIZE_MAX   4096
#define NV_TDISP_DEVICE_CERT_SERIAL_SIZE_MAX  64

// Large enough for the SHA-512 the TSM may report.
#define NV_TDISP_DEVICE_CERT_DIGEST_SIZE_MAX  64

/*!
 * @brief Verify the realm-cached device certificate chain against the FSP root
 *        CA, then confirm it and the GSP certificate chain belong to the same
 *        board by comparing the PDI carried in their L4 certificate serial
 *        numbers.
 */
NV_STATUS
tdispmgrVerifyDeviceCert_KERNEL
(
    OBJGPU   *pGpu,
    TdispMgr *pTdispMgr
)
{
    NvU8       *pCertChain   = NULL;
    NvU8       *pL4Serial    = NULL;
    NvU8       *pTsmDigest   = NULL;
    NvU8       *pLocalDigest = NULL;
    NvU32       certChainSize = NV_TDISP_DEVICE_CERT_CHAIN_SIZE_MAX;
    NV_STATUS   status;
    Spdm       *pSpdm = GPU_GET_SPDM(pGpu);
    const NvU8 *pL4Cert = NULL;
    NvU32       l4CertSize;
    NvU32       l4SerialSize;

    PORT_UNREFERENCED_VARIABLE(pTdispMgr);

    pCertChain   = portMemAllocNonPaged(NV_TDISP_DEVICE_CERT_CHAIN_SIZE_MAX);
    pL4Serial    = portMemAllocNonPaged(NV_TDISP_DEVICE_CERT_SERIAL_SIZE_MAX);
    pTsmDigest   = portMemAllocNonPaged(NV_TDISP_DEVICE_CERT_DIGEST_SIZE_MAX);
    pLocalDigest = portMemAllocNonPaged(NV_TDISP_DEVICE_CERT_DIGEST_SIZE_MAX);

    if (pCertChain == NULL || pL4Serial == NULL ||
        pTsmDigest == NULL || pLocalDigest == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        goto done;
    }

    status = osReadDeviceCertificate(pGpu, pCertChain, &certChainSize);
    if (status == NV_ERR_BUFFER_TOO_SMALL)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "device certificate is %u bytes, buffer too small\n", certChainSize);
    }
    else if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "osReadDeviceCertificate failed (0x%08x)\n", status);
    }

    //
    // The chain above came from the untrusted host, which is free to hand back
    // a different GPU's certificate. The TSM is the trusted one and reports
    // its own digest of the certificate belonging to the device actually
    // assigned to this guest, so comparing the two closes that substitution.
    //
    // A host that can suppress the TSM query must not be able to downgrade this
    // back to the PDI comparison alone, so a failed query is as fatal as a
    // mismatch.
    //
    if (status == NV_OK)
    {
        NvU32 digestSize = NV_TDISP_DEVICE_CERT_DIGEST_SIZE_MAX;

        status = osReadDeviceCertDigests(pGpu, pCertChain, certChainSize,
                                         pTsmDigest, pLocalDigest, &digestSize);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "osReadDeviceCertDigests failed (0x%08x)\n", status);
        }
        else if (portMemCmp(pTsmDigest, pLocalDigest, digestSize) != 0)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "device certificate does not match the TSM digest\n");
            status = NV_ERR_INVALID_DATA;
        }
    }

    if (status == NV_OK)
    {
        status = spdmVerifyDeviceCertChain(pGpu, pSpdm, pCertChain, certChainSize);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "device certificate chain failed verification (0x%08x)\n", status);
        }
    }

    if (status == NV_OK)
    {
        status = spdmGetCertFromChain(pGpu, pSpdm, pCertChain, certChainSize,
                                      NV_TDISP_DEVICE_CERT_L4_INDEX,
                                      &pL4Cert, &l4CertSize);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "spdmGetCertFromChain failed for L4 cert (0x%08x)\n", status);
        }
    }

    if (status == NV_OK)
    {
        l4SerialSize = NV_TDISP_DEVICE_CERT_SERIAL_SIZE_MAX;
        status = spdmGetCertSerialNumber(pGpu, pSpdm, pL4Cert, l4CertSize,
                                         pL4Serial, &l4SerialSize);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "spdmGetCertSerialNumber failed for L4 cert (0x%08x)\n", status);
        }
    }

    if (status == NV_OK)
    {
        if (pSpdm == NULL || pSpdm->pGspCertSerialNumber == NULL)
        {
            NV_PRINTF(LEVEL_ERROR, "GSP certificate serial number is not cached\n");
            status = NV_ERR_INVALID_STATE;
        }
        else if (pSpdm->gspCertSerialNumberSize < NV_TDISP_PDI_OFFSET + NV_TDISP_PDI_SIZE ||
                 l4SerialSize < NV_TDISP_PDI_OFFSET + NV_TDISP_PDI_SIZE)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "serial number too short to carry a PDI (GSP %u, device %u)\n",
                      pSpdm->gspCertSerialNumberSize, l4SerialSize);
            status = NV_ERR_INVALID_DATA;
        }
        else if (portMemCmp(pSpdm->pGspCertSerialNumber + NV_TDISP_PDI_OFFSET,
                            pL4Serial + NV_TDISP_PDI_OFFSET,
                            NV_TDISP_PDI_SIZE) != 0)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "PDI mismatch between GSP and device certificate chains\n");
            status = NV_ERR_INVALID_DATA;
        }
    }

done:
    portMemFree(pCertChain);
    portMemFree(pL4Serial);
    portMemFree(pTsmDigest);
    portMemFree(pLocalDigest);

    return status;
}
