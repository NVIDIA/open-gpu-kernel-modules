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
 *      Confidential Compute HALs                                           *
 *                                                                          *
 \**************************************************************************/

#define NVOC_CONF_COMPUTE_H_PRIVATE_ACCESS_ALLOWED

#include "gpu/conf_compute/conf_compute.h"
#include "rmapi/rmapi.h"
#include "conf_compute/cc_keystore.h"

/*!
 * Returns RM engine Id corresponding to a key space
 *
 * @param[in]     pConfCompute             : ConfidentialCompute pointer
 * @param[in]     keySpace                 : value of keyspace from cc_keystore.h
 */
RM_ENGINE_TYPE
confComputeGetEngineIdFromKeySpace_GB100
(
    ConfidentialCompute *pConfCompute,
    NvU32 keySpace
)
{
    if (keySpace == CC_KEYSPACE_GSP)
    {
        return RM_ENGINE_TYPE_NULL;
    }

    if (keySpace == CC_KEYSPACE_SEC2)
    {
        return RM_ENGINE_TYPE_SEC2;
    }

    switch (keySpace)
    {
        case CC_KEYSPACE_LCE10:
            return RM_ENGINE_TYPE_COPY12;

        case CC_KEYSPACE_LCE11:
            return RM_ENGINE_TYPE_COPY13;

        case CC_KEYSPACE_LCE12:
            return RM_ENGINE_TYPE_COPY14;

        case CC_KEYSPACE_LCE13:
            return RM_ENGINE_TYPE_COPY15;

        case CC_KEYSPACE_LCE14:
            return RM_ENGINE_TYPE_COPY16;

        case CC_KEYSPACE_LCE15:
            return RM_ENGINE_TYPE_COPY17;

        case CC_KEYSPACE_LCE16:
            return RM_ENGINE_TYPE_COPY18;

        case CC_KEYSPACE_LCE17:
            return RM_ENGINE_TYPE_COPY19;

        default:
            return confComputeGetEngineIdFromKeySpace_GH100(pConfCompute, keySpace);
    }

    return RM_ENGINE_TYPE_NULL;
}

NV_STATUS
confComputeDeriveSecrets_GB100
(
    ConfidentialCompute *pConfCompute,
    NvU32                engine
)
{
    OBJGPU *pGpu = ENG_GET_GPU(pConfCompute);

    if (!IS_GSP_CLIENT(pGpu))
    {
        return NV_OK;
    }

    switch (engine)
    {
        /*
         * With Blackwell, key derivation has been modified to per-channel based
         * as compared to per-engine based implementation earlier.
         * Hence we need not do any key-derivation here.
         */
        case MC_ENGINE_IDX_CE2:
        case MC_ENGINE_IDX_CE3:
        case MC_ENGINE_IDX_CE4:
        case MC_ENGINE_IDX_CE5:
        case MC_ENGINE_IDX_CE6:
        case MC_ENGINE_IDX_CE7:
        case MC_ENGINE_IDX_CE8:
        case MC_ENGINE_IDX_CE9:
        case MC_ENGINE_IDX_CE12:
        case MC_ENGINE_IDX_CE13:
        case MC_ENGINE_IDX_CE14:
        case MC_ENGINE_IDX_CE15:
        case MC_ENGINE_IDX_CE16:
        case MC_ENGINE_IDX_CE17:
        case MC_ENGINE_IDX_CE18:
        case MC_ENGINE_IDX_CE19:
            return NV_OK;
        default:
            return confComputeDeriveSecrets_GH100(pConfCompute, engine);
    }

    return NV_OK;
}

/*!
 * Returns a key space corresponding to a channel
 *
 * @param[in]   pConfCompute               : ConfidentialCompute pointer
 * @param[in]   pKernelChannel             : KernelChannel pointer
 * @param[out]  keySpace                   : value of keyspace from cc_keystore.h
 */
NV_STATUS
confComputeGetKeySpaceFromKChannel_GB100
(
    ConfidentialCompute *pConfCompute,
    KernelChannel       *pKernelChannel,
    NvU16               *keyspace
)
{
    switch (kchannelGetEngineType(pKernelChannel))
    {
        case RM_ENGINE_TYPE_COPY12:
            *keyspace = CC_KEYSPACE_LCE10;
            break;
        case RM_ENGINE_TYPE_COPY13:
            *keyspace = CC_KEYSPACE_LCE11;
            break;
        case RM_ENGINE_TYPE_COPY14:
            *keyspace = CC_KEYSPACE_LCE12;
            break;
        case RM_ENGINE_TYPE_COPY15:
            *keyspace = CC_KEYSPACE_LCE13;
            break;
        case RM_ENGINE_TYPE_COPY16:
            *keyspace = CC_KEYSPACE_LCE14;
            break;
        case RM_ENGINE_TYPE_COPY17:
            *keyspace = CC_KEYSPACE_LCE15;
            break;
        case RM_ENGINE_TYPE_COPY18:
            *keyspace = CC_KEYSPACE_LCE16;
            break;
        case RM_ENGINE_TYPE_COPY19:
            *keyspace = CC_KEYSPACE_LCE17;
            break;
        default:
            return confComputeGetKeySpaceFromKChannel_GH100(pConfCompute,
                    pKernelChannel, keyspace);
    }

    return NV_OK;
}
