/*
 * SPDX-FileCopyrightText: Copyright (c) 2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/gpu.h"
#include "gpu/gpu_resource.h"
#include "gpu/conf_compute/conf_compute.h"
#include "os/os.h"

#include "nvspdm_rmconfig.h"
#include "internal/libspdm_common_lib.h"

/*!
 * Constructor
 */
NV_STATUS
spdmConstructEngine_IMPL
(
    OBJGPU        *pGpu,
    Spdm          *pSpdm,
    ENGDESCRIPTOR  engDesc
)
{
    pSpdm->pLibspdmContext             = NULL;
    pSpdm->pLibspdmScratch             = NULL;
    pSpdm->pAttestationCertChain       = NULL;
    pSpdm->pDeviceIOContext            = NULL;
    pSpdm->pMsgLog                     = NULL;
    pSpdm->pTranscriptLog              = NULL;

    pSpdm->libspdmContextSize          = 0;
    pSpdm->libspdmScratchSize          = 0;
    pSpdm->attestationCertChainSize    = 0;
    pSpdm->msgLogMaxSize               = 0;
    pSpdm->transcriptLogSize           = 0;
    pSpdm->sessionId                   = INVALID_SESSION_ID;
    pSpdm->bSessionEstablished         = NV_FALSE;
    pSpdm->bUsePolling                 = NV_FALSE;
    pSpdm->bExportSecretCleared        = NV_FALSE;

    pSpdm->pPayloadBufferMemDesc       = NULL;
    pSpdm->payloadBufferSize           = 0;

    pSpdm->pHeartbeatEvent             = NULL;
    pSpdm->heartbeatPeriodSec          = 0;
    pSpdm->heartbeatPeriodSec          = 0;
    pSpdm->nvSpdmRequesterId           = NV_SPDM_REQUESTER_ID_NULL;

    return NV_OK;
}

NV_STATUS
spdmStatePostLoad_KERNEL
(
    OBJGPU *pGpu,
    Spdm   *pSpdm,
    NvU32   flags
)
{
    NV_STATUS status = NV_OK;
    return status;
}

NV_STATUS
spdmStatePreUnload_KERNEL
(
    OBJGPU *pGpu,
    Spdm   *pSpdm,
    NvU32   flags
)
{
    NV_STATUS status = NV_OK;
    return status;
}


/*!
 * Deinitialize the SPDM context
 *
 * Note: Must occur in destructor, rather than spdmStateDestroy
 * as engine state is set to destroy before GSP-RM teardown. Since we
 * still need SPDM to be valid until after GSP-RM teardown, we wait until
 * object destruction.
 *
 * @param[in]  pSpdm  Spdm pointer
 */
void
spdmDestruct_IMPL
(
    Spdm *pSpdm
)
{
    NV_STATUS  status = NV_OK;
    OBJGPU    *pGpu   = ENG_GET_GPU(pSpdm);

    if (IS_GSP_CLIENT(pGpu) && pSpdm->getProperty(pSpdm, PDB_PROP_SPDM_ENABLED))
    {
        status = spdmContextDeinit(pGpu, pSpdm, NV_TRUE);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Failed deinitializing SPDM: 0x%x!\n", status);
        }
    }

    return;
}
