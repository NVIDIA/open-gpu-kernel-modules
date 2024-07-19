/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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


#include "core/core.h"
#include "gpu/gpu.h"
#include "os/os.h"
#include "lib/base_utils.h"
#include "lib/zlib/inflate.h"
#include "nvrm_registry.h"
#include "virtualization/hypervisor/hypervisor.h"

/**
 * @brief Changes the user-space permissions for a given register address range
 *
 * @param pGpu
 * @param[in] offset  byte address of register address range start
 * @param[in] size  size in bytes of register address range
 * @param[in] bAllow whether or not to allow register access from user space
 *
 * @return NV_OK if success, error otherwise
 */
NV_STATUS
gpuSetUserRegisterAccessPermissions_IMPL(OBJGPU *pGpu, NvU32 offset, NvU32 size, NvBool bAllow)
{
    NvU32 mapSize = pGpu->userRegisterAccessMapSize * 8;  // total number of bits
    NvU32 bitOffset;
    NvU32 bitSize;

    NV_ASSERT_OR_RETURN(pGpu->pUserRegisterAccessMap != NULL, NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN((offset & 3) == 0, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN((size & 3) == 0, NV_ERR_INVALID_ARGUMENT);

    NV_PRINTF(LEVEL_INFO, "%sllowing access to 0x%x-0x%x\n",
              (bAllow ? "A" : "Disa"), offset, (offset + size - 1));

    NV_PRINTF(LEVEL_INFO, "Byte 0x%x Bit 0x%x through Byte 0x%x Bit 0x%x\n",
              offset / 4 / 8, offset / 4 % 8, (offset + size) / 4 / 8,
              (offset + size - 1) / 4 % 8);

    bitOffset = offset/sizeof(NvU32);
    bitSize = size/sizeof(NvU32);

    NV_ASSERT_OR_RETURN(bitOffset < mapSize, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN((bitOffset+bitSize) <= mapSize, NV_ERR_INVALID_ARGUMENT);

    // Deal with bits  up to first byte.
    for (; bitOffset%8 != 0 && bitSize; bitOffset++, bitSize--)
    {
        nvBitFieldSet((NvU32*) pGpu->pUserRegisterAccessMap,
                pGpu->userRegisterAccessMapSize / sizeof(NvU32),
                bitOffset, bAllow);
    }

    if (!bitSize)
        return NV_OK;

    // Deal with any full bytes.
    portMemSet(pGpu->pUserRegisterAccessMap + bitOffset/8, bAllow ? 0xff : 0x0, NV_ALIGN_DOWN(bitSize, 8)/8);

    bitOffset += NV_ALIGN_DOWN(bitSize, 8);
    bitSize -= NV_ALIGN_DOWN(bitSize, 8);

    // Any remaining bits
    for (; bitSize; bitOffset++, bitSize--)
    {
        nvBitFieldSet((NvU32*) pGpu->pUserRegisterAccessMap,
                pGpu->userRegisterAccessMapSize / sizeof(NvU32),
                bitOffset, bAllow);
    }

    return NV_OK;
}

/**
 * @brief Changes the user-space permissions for the given (in bulk) register address ranges
 *
 * @param pGpu
 * @param[in] pOffsetsSizesArr  flat array of (register offset, register size in bytes) pairs
 * @param[in] arrSizeBytes  size in bytes of the pOffsetsSizesArr array
 * @param[in] bAllow  whether or not to allow register access from user space
 *
 * @return NV_OK if success, error otherwise
 */
NV_STATUS
gpuSetUserRegisterAccessPermissionsInBulk_IMPL(OBJGPU *pGpu, const NvU32 *pOffsetsSizesArr,
                                               NvU32 arrSizeBytes, NvBool bAllow)
{
    NV_ASSERT_OR_RETURN((arrSizeBytes & (2 * sizeof(NvU32) - 1)) == 0, NV_ERR_INVALID_ARGUMENT);
    NvU32 numElements = arrSizeBytes / sizeof(NvU32);

    NvU32 i;
    NV_STATUS status;
    for (i = 0; i < numElements; i += 2)
    {
        status = gpuSetUserRegisterAccessPermissions(pGpu,
                    pOffsetsSizesArr[i], pOffsetsSizesArr[i + 1], bAllow);

        if (status != NV_OK)
        {
            return status;
        }
    }

    return NV_OK;
}

/**
 * @brief returns if a given register address can be accessed from userspace.
 *
 * @param pGpu
 * @param[in]  offset Register offset to test, must be dword aligned.
 *
 * @return NV_TRUE if register is accessible, NV_FALSE if not.
 */
NvBool
gpuGetUserRegisterAccessPermissions_IMPL(OBJGPU *pGpu, NvU32 offset)
{
    NvU32 bitOffset = offset / sizeof(NvU32);

    if (!pGpu->pUserRegisterAccessMap)
    {
        //
        // If very early in the init sequence, everything is accessible, since
        // we can't have gotten any user originating accesses yet.
        //
        if (!gpuIsFullyConstructed(pGpu))
            return NV_TRUE;

        NV_ASSERT_FAILED("No user register access map available to read");
        return NV_FALSE;
    }

    if (bitOffset >= (pGpu->userRegisterAccessMapSize * 8))
    {
        NV_PRINTF(LEVEL_ERROR, "Parameter `offset` = %u is out of bounds.\n",
                  offset);
        return NV_FALSE;
    }

    if ((offset % 4) != 0)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Parameter `offset` = %u must be 4-byte aligned.\n", offset);
        return NV_FALSE;
    }

    // pGpu->pUserRegisterAccessMap is pageable, must not be at raised IRQ
    NV_ASSERT_OR_RETURN(!osIsRaisedIRQL(), NV_ERR_INVALID_IRQ_LEVEL);

    return nvBitFieldTest((NvU32*) pGpu->pUserRegisterAccessMap, pGpu->userRegisterAccessMapSize / sizeof(NvU32), bitOffset);
}


static NvBool _getIsProfilingPrivileged(OBJGPU *pGpu)
{
    // On a vGPU Host, RmProfilingAdminOnly is always set to 1
    if (hypervisorIsVgxHyper())
    {
        //
        // Setting the value at this point to make the behavior same for
        // debug/develop/release drivers on vGPU host.
        //
        return NV_TRUE;
    }
#if defined(DEBUG) || defined(DEVELOP)
    return NV_FALSE;
#else
    NvU32 data32;
    if (NV_OK == osReadRegistryDword(pGpu, NV_REG_STR_RM_PROFILING_ADMIN_ONLY, &data32))
    {
        return (data32 == NV_REG_STR_RM_PROFILING_ADMIN_ONLY_TRUE);
    }

    return NV_TRUE;
#endif
}
/**
 * @brief Constructs the bitmap used to control whether a register can be accessed by user space.
 *
 * Bitmap contains a single bit per 32b register.
 *
 * @param pGpu
 *
 * @return NV_OK if success, error otherwise
 */
NV_STATUS
gpuConstructUserRegisterAccessMap_IMPL(OBJGPU *pGpu)
{
    NV2080_CTRL_INTERNAL_GPU_GET_USER_REGISTER_ACCESS_MAP_PARAMS *pParams = NULL;
    NV_STATUS    status              = NV_OK;
    NvU32        compressedSize      = 0;
    NvU32        profilingRangesSize = 0;
    const NvU8  *compressedData      = NULL;
    const NvU32 *profilingRangesArr  = NULL;

    if (IS_VIRTUAL(pGpu))
    {
        // Usermode access maps unused in Guest RM. Initialize this boolean and leave.
        pGpu->bRmProfilingPrivileged = _getIsProfilingPrivileged(pGpu);
        return NV_OK;
    }

    NV_ASSERT(pGpu->userRegisterAccessMapSize == 0);

    {
        RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
        pParams = portMemAllocPaged(sizeof(*pParams));
        NV_ASSERT_OR_RETURN(pParams != NULL, NV_ERR_NO_MEMORY);
        portMemSet(pParams, 0, sizeof(*pParams));

        NV_ASSERT_OK_OR_GOTO(status,
            pRmApi->Control(pRmApi, pGpu->hInternalClient, pGpu->hInternalSubdevice,
                            NV2080_CTRL_CMD_INTERNAL_GPU_GET_USER_REGISTER_ACCESS_MAP,
                            pParams, sizeof(*pParams)), done);

        pGpu->userRegisterAccessMapSize = pParams->userRegisterAccessMapSize;
        compressedSize      = pParams->compressedSize;
        profilingRangesSize = pParams->profilingRangesSize;
        compressedData      = (const NvU8*)pParams->compressedData;
        profilingRangesArr  = (const NvU32*)pParams->profilingRanges;
    }

    //
    // We round up to a 32b multiple to be used with bitfield helpers.
    // (Of course it should already be a 32b multiple, but just to be sure.)
    //
    pGpu->userRegisterAccessMapSize = NV_ALIGN_UP(pGpu->userRegisterAccessMapSize, sizeof(NvU32));
    if (pGpu->userRegisterAccessMapSize == 0)
    {
        NV_PRINTF(LEVEL_INFO,
                  "User Register Access Map unsupported for this chip.\n");
        status = NV_OK;
        goto done;
    }

    pGpu->pUserRegisterAccessMap = portMemAllocPaged(pGpu->userRegisterAccessMapSize);
    if (pGpu->pUserRegisterAccessMap == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        goto done;
    }

    pGpu->pUnrestrictedRegisterAccessMap = portMemAllocPaged(pGpu->userRegisterAccessMapSize);
    if (pGpu->pUnrestrictedRegisterAccessMap == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        goto done;
    }

    NV_PRINTF(LEVEL_INFO, "Allocated User Register Access Map of 0x%xB @%p\n",
              pGpu->userRegisterAccessMapSize, pGpu->pUserRegisterAccessMap);

    if (!pGpu->bUseRegisterAccessMap || compressedSize == 0)
    {
        NV_PRINTF(LEVEL_INFO,
                  "GPU/Platform does not have restricted user register access!  Allowing all registers.\n");
        portMemSet(pGpu->pUserRegisterAccessMap, 0xFF, pGpu->userRegisterAccessMapSize);
    }
    else
    {
        NV_ASSERT_OK_OR_GOTO(status,
            gpuInitRegisterAccessMap(pGpu, pGpu->pUserRegisterAccessMap,
                pGpu->userRegisterAccessMapSize, compressedData, compressedSize), done);
    }

    // copy permissions from user access map
    if (portMemCopy(pGpu->pUnrestrictedRegisterAccessMap, pGpu->userRegisterAccessMapSize,
                    pGpu->pUserRegisterAccessMap, pGpu->userRegisterAccessMapSize) == NULL)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Failed to initialize unrestricted register access map\n");
        status = NV_ERR_INVALID_ADDRESS;
        goto done;
    }

    pGpu->bRmProfilingPrivileged = _getIsProfilingPrivileged(pGpu);
    if (pGpu->bRmProfilingPrivileged && profilingRangesSize > 0)
    {
        // remove profiling registers from user map
        status = gpuSetUserRegisterAccessPermissionsInBulk(
                    pGpu, profilingRangesArr, profilingRangesSize, NV_FALSE);
        if (status != NV_OK)
        {
            pGpu->bRmProfilingPrivileged = NV_FALSE;
            goto done;
        }
    }

done:
    if (status != NV_OK)
    {
        portMemFree(pGpu->pUserRegisterAccessMap);
        pGpu->pUserRegisterAccessMap = NULL;

        portMemFree(pGpu->pUnrestrictedRegisterAccessMap);
        pGpu->pUnrestrictedRegisterAccessMap = NULL;

        pGpu->userRegisterAccessMapSize = 0;
    }
    portMemFree(pParams);

    return status;
}


/**
 * @brief Initializes the register access map
 *
 * Extracts compressed data representing access map.
 *
 * @param pGpu
 *
 * @return NV_OK if success, error otherwise
 */
NV_STATUS
gpuInitRegisterAccessMap_IMPL(OBJGPU *pGpu, NvU8 *pAccessMap, NvU32 accessMapSize, const NvU8 *pComprData, const NvU32 comprDataSize)
{
    PGZ_INFLATE_STATE pGzState = NULL;
    NvU32 inflatedBytes        = 0;

    NV_ASSERT_OR_RETURN(pAccessMap != NULL, NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(accessMapSize != 0, NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(pComprData != NULL, NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(comprDataSize != 0, NV_ERR_INVALID_STATE);

    //
    // Strip off gzlib 10-byte header
    // XXX this really belongs in the RM GZ library
    //
    pComprData += 10;

    NV_ASSERT_OK_OR_RETURN(utilGzAllocate((NvU8*)pComprData, accessMapSize, &pGzState));

    NV_ASSERT(pGzState);

    inflatedBytes = utilGzGetData(pGzState, 0, accessMapSize, pAccessMap);

    utilGzDestroy(pGzState);

    if (inflatedBytes != accessMapSize)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "failed to get inflated data, got %u bytes, expecting %u\n",
                  inflatedBytes, pGpu->userRegisterAccessMapSize);
        DBG_BREAKPOINT();
        return NV_ERR_INFLATE_COMPRESSED_DATA_FAILED;
    }
    return NV_OK;
}
