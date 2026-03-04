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

#include "rmapi/rmapi_cache_handlers.h"
#include "nvport/nvport.h"
#include "containers/list.h"
#include "containers/map.h"

//
// NV0073_CTRL_CMD_SYSTEM_GET_SUPPORTED is CACHEABLE_BY_INPUT instead of CACHEABLE,
// because parameter subDeviceInstance, although unused in non-SLI setup, is user-provided
// and it's not expected to be changed.
//
NV_STATUS _dispSystemGetSupportedCacheHandler
(
    void *cachedEntry,
    void *pProvidedParams,
    NvBool bSet
)
{
    NV0073_CTRL_SYSTEM_GET_SUPPORTED_PARAMS *pParams     = pProvidedParams;
    DispSystemGetSupportedCacheEntry        *cacheEntry  = cachedEntry;

    if (!bSet && !cacheEntry->valid)
        return NV_ERR_OBJECT_NOT_FOUND;

    if (bSet)
    {
        cacheEntry->displayMask    = pParams->displayMask;
        cacheEntry->displayMaskDDC = pParams->displayMaskDDC;
        cacheEntry->valid          = NV_TRUE;
    }
    else
    {
        pParams->displayMask = cacheEntry->displayMask;
        pParams->displayMaskDDC = cacheEntry->displayMaskDDC;
    }

    return NV_OK;
}

//
// NV0073_CTRL_CMD_SYSTEM_GET_INTERNAL_DISPLAYS is CACHEABLE_BY_INPUT instead of CACHEABLE,
// because parameter subDeviceInstance, although unused in non-SLI setup, is user-provided
// and it's not expected to be changed.
//
NV_STATUS _dispSystemGetInternalDisplaysCacheHandler
(
    void *cachedEntry,
    void *pProvidedParams,
    NvBool bSet
)
{
    NV0073_CTRL_SYSTEM_GET_INTERNAL_DISPLAYS_PARAMS *pParams    = pProvidedParams;
    DispSystemGetInternalDisplaysCacheEntry         *cacheEntry = cachedEntry;

    if (!bSet && !cacheEntry->valid)
        return NV_ERR_OBJECT_NOT_FOUND;

    if (bSet)
    {
        cacheEntry->internalDisplaysMask          = pParams->internalDisplaysMask;
        cacheEntry->availableInternalDisplaysMask = pParams->availableInternalDisplaysMask;
        cacheEntry->valid                         = NV_TRUE;
    }
    else
    {
        pParams->internalDisplaysMask          = cacheEntry->internalDisplaysMask;
        pParams->availableInternalDisplaysMask = cacheEntry->availableInternalDisplaysMask;
    }

    return NV_OK;
}
//
// NV0073_CTRL_CMD_DP_GET_CAPS Cache Handler.
//
NV_STATUS _dispDpGetCapsCacheHandler
(
    void *cachedEntry,
    void *pProvidedParams,
    NvBool bSet
)
{
    NV0073_CTRL_CMD_DP_GET_CAPS_PARAMS *pParams     = pProvidedParams;
    DispDpGetCapsCacheTable            *pCacheTable = cachedEntry;
    NV0073_CTRL_CMD_DP_GET_CAPS_PARAMS *src         = (bSet) ? pParams
                                                             : &pCacheTable->cachedEntries[pParams->sorIndex].params;
    NV0073_CTRL_CMD_DP_GET_CAPS_PARAMS *dst         = (bSet) ? &pCacheTable->cachedEntries[pParams->sorIndex].params
                                                             : pParams;


    if (pParams->sorIndex >= NV_ARRAY_ELEMENTS(pCacheTable->cachedEntries))
        return NV_ERR_INVALID_ARGUMENT;

    if (!bSet && !pCacheTable->cachedEntries[pParams->sorIndex].valid)
        return NV_ERR_OBJECT_NOT_FOUND;

    // Verify mode.
    if (bSet)
    {
        if (pCacheTable->cachedEntries[pParams->sorIndex].valid)
        {
            //
            // Assert that all fields match between src and dst
            //
            // NOTE: subdeviceInstance is not compared, since it's not used in non-SLI systems
            //       and there's no need to compare bIsPC2Disabled, since it's deprecated and unused.
            //
            NV_ASSERT((src->sorIndex == dst->sorIndex)                                         &&
                      (src->maxLinkRate == dst->maxLinkRate)                                   &&
                      (src->dpVersionsSupported == dst->dpVersionsSupported)                   &&
                      (src->UHBRSupportedByGpu == dst->UHBRSupportedByGpu)                     &&
                      (src->bIsMultistreamSupported == dst->bIsMultistreamSupported)           &&
                      (src->bIsSCEnabled == dst->bIsSCEnabled)                                 &&
                      (src->bHasIncreasedWatermarkLimits == dst->bHasIncreasedWatermarkLimits) &&
                      (src->isSingleHeadMSTSupported == dst->isSingleHeadMSTSupported)         &&
                      (src->bFECSupported == dst->bFECSupported)                               &&
                      (src->bIsTrainPhyRepeater == dst->bIsTrainPhyRepeater)                   &&
                      (src->bOverrideLinkBw == dst->bOverrideLinkBw)                           &&
                      (src->bUseRgFlushSequence == dst->bUseRgFlushSequence)                   &&
                      (src->bSupportDPDownSpread == dst->bSupportDPDownSpread)                 &&
                      (src->bAvoidHBR3 == dst->bAvoidHBR3)                                     &&
                      (src->DSC.bDscSupported == dst->DSC.bDscSupported)                       &&
                      (src->DSC.encoderColorFormatMask == dst->DSC.encoderColorFormatMask)     &&
                      (src->DSC.lineBufferSizeKB == dst->DSC.lineBufferSizeKB)                 &&
                      (src->DSC.rateBufferSizeKB == dst->DSC.rateBufferSizeKB)                 &&
                      (src->DSC.bitsPerPixelPrecision == dst->DSC.bitsPerPixelPrecision)       &&
                      (src->DSC.maxNumHztSlices == dst->DSC.maxNumHztSlices)                   &&
                      (src->DSC.lineBufferBitDepth == dst->DSC.lineBufferBitDepth));
        }

        pCacheTable->cachedEntries[pParams->sorIndex].valid = NV_TRUE;
    }

    portMemCopy(dst, sizeof(*pParams), src, sizeof(*pParams));

    return NV_OK;
}

//
// NV0073_CTRL_CMD_SPECIFIC_GET_TYPE Cache Handler.
//
NV_STATUS _dispSpecificGetTypeCacheHandler
(
    void *cachedEntry,
    void *pProvidedParams,
    NvBool bSet
)
{
    NV0073_CTRL_SPECIFIC_GET_TYPE_PARAMS *pParams     = pProvidedParams;
    DispSpecificGetTypeCacheTable        *pCacheTable = cachedEntry;
    NvU32 cacheEntryIdx;

    if (!ONEBITSET(pParams->displayId))
        return NV_ERR_INVALID_ARGUMENT;

    cacheEntryIdx = BIT_IDX_32(pParams->displayId);

    if (!bSet && !pCacheTable->cachedEntries[cacheEntryIdx].valid)
        return NV_ERR_OBJECT_NOT_FOUND;

    if (bSet)
    {
        if (pCacheTable->cachedEntries[cacheEntryIdx].valid)
        {
            // Verify mode.
            NV_ASSERT(pCacheTable->cachedEntries[cacheEntryIdx].displayType == pParams->displayType);
        }

        pCacheTable->cachedEntries[cacheEntryIdx].displayType = pParams->displayType;
        pCacheTable->cachedEntries[cacheEntryIdx].valid       = NV_TRUE;
    }
    else
    {
        pParams->displayType = pCacheTable->cachedEntries[cacheEntryIdx].displayType;
    }

    return NV_OK;
}

