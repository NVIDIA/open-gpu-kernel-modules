/*
 * SPDX-FileCopyrightText: Copyright (c) 2022-2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * @file
 * @brief ENGINE_TYPE controls
 */

#include "kernel/gpu/gpu.h"
#include "kernel/gpu/nvbitmask.h"
#include "gmcapi/gmcapi_engine_types.h"

ct_assert(RM_ENGINE_TYPE_LAST == NV2080_ENGINE_TYPE_LAST);

/* This includes all the entries that have 1:1 mappings across all 3 engine type definitions. */
#define _NV_ENGINE_MAPPINGS(__normal__, __decomp__) \
    __normal__(NV2080_ENGINE_TYPE_NULL,                 RM_ENGINE_TYPE_NULL,    NVGMC_ENGINE_ID_DEF(NONE, 0))   \
    __normal__(NV2080_ENGINE_TYPE_GR0,                  RM_ENGINE_TYPE_GR0,     NVGMC_ENGINE_ID_DEF(GR, 0))     \
    __normal__(NV2080_ENGINE_TYPE_GR1,                  RM_ENGINE_TYPE_GR1,     NVGMC_ENGINE_ID_DEF(GR, 1))     \
    __normal__(NV2080_ENGINE_TYPE_GR2,                  RM_ENGINE_TYPE_GR2,     NVGMC_ENGINE_ID_DEF(GR, 2))     \
    __normal__(NV2080_ENGINE_TYPE_GR3,                  RM_ENGINE_TYPE_GR3,     NVGMC_ENGINE_ID_DEF(GR, 3))     \
    __normal__(NV2080_ENGINE_TYPE_GR4,                  RM_ENGINE_TYPE_GR4,     NVGMC_ENGINE_ID_DEF(GR, 4))     \
    __normal__(NV2080_ENGINE_TYPE_GR5,                  RM_ENGINE_TYPE_GR5,     NVGMC_ENGINE_ID_DEF(GR, 5))     \
    __normal__(NV2080_ENGINE_TYPE_GR6,                  RM_ENGINE_TYPE_GR6,     NVGMC_ENGINE_ID_DEF(GR, 6))     \
    __normal__(NV2080_ENGINE_TYPE_GR7,                  RM_ENGINE_TYPE_GR7,     NVGMC_ENGINE_ID_DEF(GR, 7))     \
    __normal__(NV2080_ENGINE_TYPE_COPY0,                RM_ENGINE_TYPE_COPY0,   NVGMC_ENGINE_ID_DEF(COPY, 0))   \
    __normal__(NV2080_ENGINE_TYPE_COPY1,                RM_ENGINE_TYPE_COPY1,   NVGMC_ENGINE_ID_DEF(COPY, 1))   \
    __normal__(NV2080_ENGINE_TYPE_COPY2,                RM_ENGINE_TYPE_COPY2,   NVGMC_ENGINE_ID_DEF(COPY, 2))   \
    __normal__(NV2080_ENGINE_TYPE_COPY3,                RM_ENGINE_TYPE_COPY3,   NVGMC_ENGINE_ID_DEF(COPY, 3))   \
    __normal__(NV2080_ENGINE_TYPE_COPY4,                RM_ENGINE_TYPE_COPY4,   NVGMC_ENGINE_ID_DEF(COPY, 4))   \
    __normal__(NV2080_ENGINE_TYPE_COPY5,                RM_ENGINE_TYPE_COPY5,   NVGMC_ENGINE_ID_DEF(COPY, 5))   \
    __normal__(NV2080_ENGINE_TYPE_COPY6,                RM_ENGINE_TYPE_COPY6,   NVGMC_ENGINE_ID_DEF(COPY, 6))   \
    __normal__(NV2080_ENGINE_TYPE_COPY7,                RM_ENGINE_TYPE_COPY7,   NVGMC_ENGINE_ID_DEF(COPY, 7))   \
    __normal__(NV2080_ENGINE_TYPE_COPY8,                RM_ENGINE_TYPE_COPY8,   NVGMC_ENGINE_ID_DEF(COPY, 8))   \
    __normal__(NV2080_ENGINE_TYPE_COPY9,                RM_ENGINE_TYPE_COPY9,   NVGMC_ENGINE_ID_DEF(COPY, 9))   \
    __normal__(NV2080_ENGINE_TYPE_COPY10,               RM_ENGINE_TYPE_COPY10,  NVGMC_ENGINE_ID_DEF(COPY, 10))  \
    __normal__(NV2080_ENGINE_TYPE_COPY11,               RM_ENGINE_TYPE_COPY11,  NVGMC_ENGINE_ID_DEF(COPY, 11))  \
    __normal__(NV2080_ENGINE_TYPE_COPY12,               RM_ENGINE_TYPE_COPY12,  NVGMC_ENGINE_ID_DEF(COPY, 12))  \
    __normal__(NV2080_ENGINE_TYPE_COPY13,               RM_ENGINE_TYPE_COPY13,  NVGMC_ENGINE_ID_DEF(COPY, 13))  \
    __normal__(NV2080_ENGINE_TYPE_COPY14,               RM_ENGINE_TYPE_COPY14,  NVGMC_ENGINE_ID_DEF(COPY, 14))  \
    __normal__(NV2080_ENGINE_TYPE_COPY15,               RM_ENGINE_TYPE_COPY15,  NVGMC_ENGINE_ID_DEF(COPY, 15))  \
    __normal__(NV2080_ENGINE_TYPE_COPY16,               RM_ENGINE_TYPE_COPY16,  NVGMC_ENGINE_ID_DEF(COPY, 16))  \
    __normal__(NV2080_ENGINE_TYPE_COPY17,               RM_ENGINE_TYPE_COPY17,  NVGMC_ENGINE_ID_DEF(COPY, 17))  \
    __normal__(NV2080_ENGINE_TYPE_COPY18,               RM_ENGINE_TYPE_COPY18,  NVGMC_ENGINE_ID_DEF(COPY, 18))  \
    __normal__(NV2080_ENGINE_TYPE_COPY19,               RM_ENGINE_TYPE_COPY19,  NVGMC_ENGINE_ID_DEF(COPY, 19))  \
    __normal__(NV2080_ENGINE_TYPE_NVDEC0,               RM_ENGINE_TYPE_NVDEC0,  NVGMC_ENGINE_ID_DEF(NVDEC, 0))  \
    __normal__(NV2080_ENGINE_TYPE_NVDEC1,               RM_ENGINE_TYPE_NVDEC1,  NVGMC_ENGINE_ID_DEF(NVDEC, 1))  \
    __normal__(NV2080_ENGINE_TYPE_NVDEC2,               RM_ENGINE_TYPE_NVDEC2,  NVGMC_ENGINE_ID_DEF(NVDEC, 2))  \
    __normal__(NV2080_ENGINE_TYPE_NVDEC3,               RM_ENGINE_TYPE_NVDEC3,  NVGMC_ENGINE_ID_DEF(NVDEC, 3))  \
    __normal__(NV2080_ENGINE_TYPE_NVDEC4,               RM_ENGINE_TYPE_NVDEC4,  NVGMC_ENGINE_ID_DEF(NVDEC, 4))  \
    __normal__(NV2080_ENGINE_TYPE_NVDEC5,               RM_ENGINE_TYPE_NVDEC5,  NVGMC_ENGINE_ID_DEF(NVDEC, 5))  \
    __normal__(NV2080_ENGINE_TYPE_NVDEC6,               RM_ENGINE_TYPE_NVDEC6,  NVGMC_ENGINE_ID_DEF(NVDEC, 6))  \
    __normal__(NV2080_ENGINE_TYPE_NVDEC7,               RM_ENGINE_TYPE_NVDEC7,  NVGMC_ENGINE_ID_DEF(NVDEC, 7))  \
    __normal__(NV2080_ENGINE_TYPE_NVENC0,               RM_ENGINE_TYPE_NVENC0,  NVGMC_ENGINE_ID_DEF(NVENC, 0))  \
    __normal__(NV2080_ENGINE_TYPE_NVENC1,               RM_ENGINE_TYPE_NVENC1,  NVGMC_ENGINE_ID_DEF(NVENC, 1))  \
    __normal__(NV2080_ENGINE_TYPE_NVENC2,               RM_ENGINE_TYPE_NVENC2,  NVGMC_ENGINE_ID_DEF(NVENC, 2))  \
    __normal__(NV2080_ENGINE_TYPE_NVENC3,               RM_ENGINE_TYPE_NVENC3,  NVGMC_ENGINE_ID_DEF(NVENC, 3))  \
    __normal__(NV2080_ENGINE_TYPE_VP,                   RM_ENGINE_TYPE_VP,      NVGMC_ENGINE_ID_DEF(VP, 0))     \
    __normal__(NV2080_ENGINE_TYPE_ME,                   RM_ENGINE_TYPE_ME,      NVGMC_ENGINE_ID_DEF(ME, 0))     \
    __normal__(NV2080_ENGINE_TYPE_PPP,                  RM_ENGINE_TYPE_PPP,     NVGMC_ENGINE_ID_DEF(PPP, 0))    \
    __normal__(NV2080_ENGINE_TYPE_MPEG,                 RM_ENGINE_TYPE_MPEG,    NVGMC_ENGINE_ID_DEF(MPEG, 0))   \
    __normal__(NV2080_ENGINE_TYPE_SW,                   RM_ENGINE_TYPE_SW,      NVGMC_ENGINE_ID_DEF(SW, 0))     \
    __normal__(NV2080_ENGINE_TYPE_TSEC,                 RM_ENGINE_TYPE_TSEC,    NVGMC_ENGINE_ID_DEF(TSEC, 0))   \
    __normal__(NV2080_ENGINE_TYPE_VIC,                  RM_ENGINE_TYPE_VIC,     NVGMC_ENGINE_ID_DEF(VIC, 0))    \
    __normal__(NV2080_ENGINE_TYPE_MP,                   RM_ENGINE_TYPE_MP,      NVGMC_ENGINE_ID_DEF(MP, 0))     \
    __normal__(NV2080_ENGINE_TYPE_SEC2,                 RM_ENGINE_TYPE_SEC2,    NVGMC_ENGINE_ID_DEF(SEC2, 0))   \
    __normal__(NV2080_ENGINE_TYPE_HOST,                 RM_ENGINE_TYPE_HOST,    NVGMC_ENGINE_ID_DEF(HOST, 0))   \
    __normal__(NV2080_ENGINE_TYPE_DPU,                  RM_ENGINE_TYPE_DPU,     NVGMC_ENGINE_ID_DEF(DPU, 0))    \
    __normal__(NV2080_ENGINE_TYPE_PMU,                  RM_ENGINE_TYPE_PMU,     NVGMC_ENGINE_ID_DEF(PMU, 0))    \
    __normal__(NV2080_ENGINE_TYPE_FBFLCN,               RM_ENGINE_TYPE_FBFLCN,  NVGMC_ENGINE_ID_DEF(FBFLCN, 0)) \
    __normal__(NV2080_ENGINE_TYPE_NVJPEG0,              RM_ENGINE_TYPE_NVJPEG0, NVGMC_ENGINE_ID_DEF(NVJPEG, 0)) \
    __normal__(NV2080_ENGINE_TYPE_NVJPEG1,              RM_ENGINE_TYPE_NVJPEG1, NVGMC_ENGINE_ID_DEF(NVJPEG, 1)) \
    __normal__(NV2080_ENGINE_TYPE_NVJPEG2,              RM_ENGINE_TYPE_NVJPEG2, NVGMC_ENGINE_ID_DEF(NVJPEG, 2)) \
    __normal__(NV2080_ENGINE_TYPE_NVJPEG3,              RM_ENGINE_TYPE_NVJPEG3, NVGMC_ENGINE_ID_DEF(NVJPEG, 3)) \
    __normal__(NV2080_ENGINE_TYPE_NVJPEG4,              RM_ENGINE_TYPE_NVJPEG4, NVGMC_ENGINE_ID_DEF(NVJPEG, 4)) \
    __normal__(NV2080_ENGINE_TYPE_NVJPEG5,              RM_ENGINE_TYPE_NVJPEG5, NVGMC_ENGINE_ID_DEF(NVJPEG, 5)) \
    __normal__(NV2080_ENGINE_TYPE_NVJPEG6,              RM_ENGINE_TYPE_NVJPEG6, NVGMC_ENGINE_ID_DEF(NVJPEG, 6)) \
    __normal__(NV2080_ENGINE_TYPE_NVJPEG7,              RM_ENGINE_TYPE_NVJPEG7, NVGMC_ENGINE_ID_DEF(NVJPEG, 7)) \
    __normal__(NV2080_ENGINE_TYPE_OFA0,                 RM_ENGINE_TYPE_OFA0,    NVGMC_ENGINE_ID_DEF(OFA, 0))    \
    __normal__(NV2080_ENGINE_TYPE_OFA1,                 RM_ENGINE_TYPE_OFA1,    NVGMC_ENGINE_ID_DEF(OFA, 1))    \
    __decomp__(NV2080_ENGINE_TYPE_COMP_DECOMP_COPY0,    RM_ENGINE_TYPE_COPY0,   NVGMC_ENGINE_ID_DEF(COPY, 0))   \
    __decomp__(NV2080_ENGINE_TYPE_COMP_DECOMP_COPY1,    RM_ENGINE_TYPE_COPY1,   NVGMC_ENGINE_ID_DEF(COPY, 1))   \
    __decomp__(NV2080_ENGINE_TYPE_COMP_DECOMP_COPY2,    RM_ENGINE_TYPE_COPY2,   NVGMC_ENGINE_ID_DEF(COPY, 2))   \
    __decomp__(NV2080_ENGINE_TYPE_COMP_DECOMP_COPY3,    RM_ENGINE_TYPE_COPY3,   NVGMC_ENGINE_ID_DEF(COPY, 3))   \
    __decomp__(NV2080_ENGINE_TYPE_COMP_DECOMP_COPY4,    RM_ENGINE_TYPE_COPY4,   NVGMC_ENGINE_ID_DEF(COPY, 4))   \
    __decomp__(NV2080_ENGINE_TYPE_COMP_DECOMP_COPY5,    RM_ENGINE_TYPE_COPY5,   NVGMC_ENGINE_ID_DEF(COPY, 5))   \
    __decomp__(NV2080_ENGINE_TYPE_COMP_DECOMP_COPY6,    RM_ENGINE_TYPE_COPY6,   NVGMC_ENGINE_ID_DEF(COPY, 6))   \
    __decomp__(NV2080_ENGINE_TYPE_COMP_DECOMP_COPY7,    RM_ENGINE_TYPE_COPY7,   NVGMC_ENGINE_ID_DEF(COPY, 7))   \
    __decomp__(NV2080_ENGINE_TYPE_COMP_DECOMP_COPY8,    RM_ENGINE_TYPE_COPY8,   NVGMC_ENGINE_ID_DEF(COPY, 8))   \
    __decomp__(NV2080_ENGINE_TYPE_COMP_DECOMP_COPY9,    RM_ENGINE_TYPE_COPY9,   NVGMC_ENGINE_ID_DEF(COPY, 9))   \
    __decomp__(NV2080_ENGINE_TYPE_COMP_DECOMP_COPY10,   RM_ENGINE_TYPE_COPY10,  NVGMC_ENGINE_ID_DEF(COPY, 10))  \
    __decomp__(NV2080_ENGINE_TYPE_COMP_DECOMP_COPY11,   RM_ENGINE_TYPE_COPY11,  NVGMC_ENGINE_ID_DEF(COPY, 11))  \
    __decomp__(NV2080_ENGINE_TYPE_COMP_DECOMP_COPY12,   RM_ENGINE_TYPE_COPY12,  NVGMC_ENGINE_ID_DEF(COPY, 12))  \
    __decomp__(NV2080_ENGINE_TYPE_COMP_DECOMP_COPY13,   RM_ENGINE_TYPE_COPY13,  NVGMC_ENGINE_ID_DEF(COPY, 13))  \
    __decomp__(NV2080_ENGINE_TYPE_COMP_DECOMP_COPY14,   RM_ENGINE_TYPE_COPY14,  NVGMC_ENGINE_ID_DEF(COPY, 14))  \
    __decomp__(NV2080_ENGINE_TYPE_COMP_DECOMP_COPY15,   RM_ENGINE_TYPE_COPY15,  NVGMC_ENGINE_ID_DEF(COPY, 15))  \
    __decomp__(NV2080_ENGINE_TYPE_COMP_DECOMP_COPY16,   RM_ENGINE_TYPE_COPY16,  NVGMC_ENGINE_ID_DEF(COPY, 16))  \
    __decomp__(NV2080_ENGINE_TYPE_COMP_DECOMP_COPY17,   RM_ENGINE_TYPE_COPY17,  NVGMC_ENGINE_ID_DEF(COPY, 17))  \
    __decomp__(NV2080_ENGINE_TYPE_COMP_DECOMP_COPY18,   RM_ENGINE_TYPE_COPY18,  NVGMC_ENGINE_ID_DEF(COPY, 18))  \
    __decomp__(NV2080_ENGINE_TYPE_COMP_DECOMP_COPY19,   RM_ENGINE_TYPE_COPY19,  NVGMC_ENGINE_ID_DEF(COPY, 19))

#define NV_MAP_2080_TO_RM(_nv2080, _rm, _gmc)  case _nv2080: return _rm;
#define NV_MAP_RM_TO_2080(_nv2080, _rm, _gmc)  case _rm:     return _nv2080;
#define NV_MAP_2080_TO_GMC(_nv2080, _rm, _gmc) case _nv2080: return _gmc;
#define NV_MAP_GMC_TO_2080(_nv2080, _rm, _gmc) case _gmc:    return _nv2080;
#define NV_MAP_RM_TO_GMC(_nv2080, _rm, _gmc)   case _rm:     return _gmc;
#define NV_MAP_GMC_TO_RM(_nv2080, _rm, _gmc)   case _gmc:    return _rm;
#define NV_MAP_NOTHING(_nv2080, _rm, _gmc)

/*!
 *  @brief Convert NV2080 engine type to Rm internal engine type
 *
 *  Rm internally uses RM engine type instead of NV2080 engine types.
 *  Some clients, like VGPU and CUDA, need have the cross-branch compatibility, we need to keep
 *  NV2080_ENGINE_TYPE consistent. When we add new ENGINE TYPEs, especially to increase the engine
 *  number on an existing ENGINE groups, we can not insert number in the middle. It will change the number
 *  of the rest of NV2080_ENGINE_TYPEs. But RM need to group the same type of ENGINE_TYPE together.
 *  So the solution is the separate RM and NV2080 engine types. When ENGINE_TYPE cross RM boundary,
 *  through control calls or RPC calls, we will need to convert the engine types.
 *
 *  @param[in] index  NV2080_ENGINE_TYPE number
 *
 *  @returns RM_ENGINE_TYPE number
 *           RM_ENGINE_TYPE_LAST if the index is invalid
 */
RM_ENGINE_TYPE gpuGetRmEngineType_IMPL(NvU32 index)
{
    //
    // The passed in index generally comes from outside RM itself w/o any checking. To
    // avoid log spam, we no longer assert on the value per the new policy on parameter
    // validation.
    //
    NV_CHECK_OR_RETURN(LEVEL_INFO, index < NV2080_ENGINE_TYPE_LAST, RM_ENGINE_TYPE_LAST);

    switch (index)
    {
        _NV_ENGINE_MAPPINGS(NV_MAP_2080_TO_RM, NV_MAP_2080_TO_RM);
        default: break;
    }

    return RM_ENGINE_TYPE_NULL;
}

/*!
 *  @brief Convert RM engine type to NV2080 engine type
 *
 *  Refer to the comments of gpuGetRmEngineType_IMPL
 *
 *  @param[in] index  RM_ENGINE_TYPE number
 *
 *  @returns NV2080_ENGINE_TYPE number
 *           NV2080_ENGINE_TYPE_LAST if the index is invalid
 */
NvU32 gpuGetNv2080EngineType_IMPL(RM_ENGINE_TYPE index)
{
    NV_CHECK_OR_RETURN(LEVEL_ERROR, index < RM_ENGINE_TYPE_LAST, NV2080_ENGINE_TYPE_LAST);

    switch (index)
    {
        _NV_ENGINE_MAPPINGS(NV_MAP_RM_TO_2080, NV_MAP_NOTHING);
        default: break;
    }

    return NV2080_ENGINE_TYPE_NULL;
}

/*!
 *  @brief Convert a list of RM engine type to a list of NV2080 engine type
 *
 *  Refer to the comments of gpuGetRmEngineType_IMPL
 *
 *  @param[in] pRmEngineList         A list in order of RM_ENGINE_TYPE
 *  @param[in] engineCount           Engine numbers
 *  @param[out] pNv2080EngineList    Output list in order of NV2080_ENGINE_TYPE
 *
 *  @returns void
 */
void gpuGetNv2080EngineTypeList_IMPL
(
    RM_ENGINE_TYPE *pRmEngineList,
    NvU32 engineCount,
    NvU32 *pNv2080EngineList
)
{
    NV_ASSERT_OR_RETURN_VOID(engineCount < RM_ENGINE_TYPE_LAST);

    NvU32 i;
    for (i = 0; i < engineCount; i++)
    {
        pNv2080EngineList[i] = gpuGetNv2080EngineType(pRmEngineList[i]);
    }
}

/*!
 *  @brief Convert a list NV2080 engine type of to a list of RM engine type
 *
 *  @param[in] pNv2080EngineList    A list in order of NV2080_ENGINE_TYPE
 *  @param[in] engineCount          Engine numbers
 *  @param[out] pRmEngineList       Output list in order of RM_ENGINE_TYPE
 *
 *  @returns void
 */
void gpuGetRmEngineTypeList_IMPL
(
    NvU32 *pNv2080EngineList,
    NvU32 engineCount,
    RM_ENGINE_TYPE *pRmEngineList
)
{
    NV_ASSERT_OR_RETURN_VOID(engineCount < RM_ENGINE_TYPE_LAST);

    NvU32 i;
    for (i = 0; i < engineCount; i++)
    {
        pRmEngineList[i] = gpuGetRmEngineType(pNv2080EngineList[i]);
    }
}

/*!
 *  @brief Convert NVGMC engine type to Rm internal engine type
 */
RM_ENGINE_TYPE gpuGetRmEngineTypeFromGMCEngineId_IMPL(NvU32 gmcEngineId)
{
    switch(gmcEngineId)
    {
        _NV_ENGINE_MAPPINGS(NV_MAP_GMC_TO_RM, NV_MAP_NOTHING);
        default: break;
    }
    return RM_ENGINE_TYPE_NULL;
}

/*!
 *  @brief Convert NVGMC engine type masks to Rm internal engine caps
 */
void gpuGetRmEngineTypeCapsFromGMCEngineIdMasks_IMPL(NvU32 *pRmEngineTypeCap, const NvU64 *gmcEngineIdMask, NvU32 gmcEngineTypeStart, NvU32 gmcEngineTypeCount)
{
    for (NvU32 i = 0; i < gmcEngineTypeCount; i++)
    {
        NvU64 engineTypeMask = gmcEngineIdMask[i];
        if (engineTypeMask != 0)
        {
            NvU32 engineType = gmcEngineTypeStart + i;
            for (NvU32 j = 0; j < 64; j++)
            {
                if (engineTypeMask & (1ULL << j))
                {
                    NvU32 engineId = NVGMC_ENGINE_ID_NUM(engineType, j);
                    NvU32 rmEngineType = gpuGetRmEngineTypeFromGMCEngineId(engineId);
                    if (rmEngineType != RM_ENGINE_TYPE_NULL)
                    {
                        NVGPU_SET_ENGINE_CAPS_MASK(pRmEngineTypeCap, rmEngineType);
                    }
                }
            }
        }
    }
}

/*!
 *  @brief Convert RM internal engine type to NVGMC engine Id
 */
NvU32 gpuGetGMCEngineIdFromRmEngineType_IMPL(RM_ENGINE_TYPE engineType)
{
    switch(engineType)
    {
        _NV_ENGINE_MAPPINGS(NV_MAP_RM_TO_GMC, NV_MAP_NOTHING);
        default: break;
    }
    return NVGMC_ENGINE_ID_DEF(NONE, 0);
}

/*!
 *  @brief Convert RM internal engine type caps to NVGMC engine id masks
 */
void gpuGetGMCEngineIdMasksFromRmEngineTypeCaps_IMPL(NvU64 *pGMCEngineIdMasks, const NvU32 *pRmEngineTypeCap)
{
    // Translate from RM engine type caps bits to GMCAPI engine masks.
    for(NvU32 i = 0; i < RM_ENGINE_TYPE_LAST; i++)
    {
        if (NVGPU_GET_ENGINE_CAPS_MASK(pRmEngineTypeCap, i))
        {
            NvU32 gmcEngineId = gpuGetGMCEngineIdFromRmEngineType(i);
            if (gmcEngineId != NVGMC_ENGINE_ID_DEF(NONE, 0))
            {
                NvU32 gmcEngineType = REF_VAL(NVGMC_ENGINE_ID_TYPE, gmcEngineId);
                NvU32 gmcEngineIndex = REF_VAL(NVGMC_ENGINE_ID_INDEX, gmcEngineId);
                pGMCEngineIdMasks[gmcEngineType] |= 1ULL << gmcEngineIndex;
            }
        }
    }
}

/*!
 *  @brief Convert NV2080 engine type to NVGMC engine id
 */
NvU32 gpuGetGMCEngineIdFromNv2080EngineType_IMPL(NvU32 nv2080EngineType)
{
    switch(nv2080EngineType)
    {
        _NV_ENGINE_MAPPINGS(NV_MAP_2080_TO_GMC, NV_MAP_2080_TO_GMC);
        default: break;
    }
    return NVGMC_ENGINE_ID_DEF(NONE, 0);
}


/*!
 *  @brief Convert NVGMC engine id to NV2080 engine type
 */
NvU32 gpuGetNv2080EngineTypeFromGMCEngineId_IMPL(NvU32 gmcEngineId)
{
    switch(gmcEngineId)
    {
        _NV_ENGINE_MAPPINGS(NV_MAP_GMC_TO_2080, NV_MAP_NOTHING);
        default: break;
    }
    return NV2080_ENGINE_TYPE_NULL;
}

/*!
 *  @brief Convert a capability mask of NV2080 engine type to the RM engine type capability mask.
 *
 *  Refer to the comments of gpuGetRmEngineType_IMPL
 *
 *  @param[in]  pNV2080EngineTypeCap        NV2080 engine type capability mask
 *  @param[in]  capSize                     Cap size in dword
 *  @param[out] pRmEngineTypeCap            RM engine type capability mask
 *
 *  @returns NV_OK
 *           NV_ERR_INVALID_ARGUMENT
 */
NV_STATUS gpuGetRmEngineTypeCapMask_IMPL
(
    NvU32 *pNV2080EngineTypeCap,
    NvU32 capSize,
    NvU32 *pRmEngineTypeCap
)
{
    NvU32 i;

    NV_ASSERT_OR_RETURN(capSize == NVGPU_ENGINE_CAPS_MASK_ARRAY_MAX, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pRmEngineTypeCap != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pNV2080EngineTypeCap != NULL, NV_ERR_INVALID_ARGUMENT);

    for (i = 0; i < capSize; i++)
    {
        pRmEngineTypeCap[i] = 0;
    }

    for (i = 0; i < NV2080_ENGINE_TYPE_LAST; i++)
    {
        if (NVGPU_GET_ENGINE_CAPS_MASK(pNV2080EngineTypeCap, i))
        {
            NVGPU_SET_ENGINE_CAPS_MASK(pRmEngineTypeCap, gpuGetRmEngineType(i));
        }
    }

    return NV_OK;
}

/*!
 *  @brief Convert RM engine type to the engine class as a string
 *
 *  Use ENGDESC_FIELD(engDesc, _INST) to get the engine instance
 *
 *  @param[in]  engineType      RM_ENGINE_TYPE number
 *  @param[in]  bNvPrintfStr    Make the returned string compatible with NV_PRINTF
 *
 *  @returns engine name as a string
 */
const char* gpuRmEngineTypeToString_IMPL
(
    RM_ENGINE_TYPE engineType,
    NvBool         bNvPrintfStr
)
{
    if      (RM_ENGINE_TYPE_IS_GR(engineType))     return bNvPrintfStr ? MAKE_NV_PRINTF_STR("GR")      : "GR";
    else if (RM_ENGINE_TYPE_IS_COPY(engineType))   return bNvPrintfStr ? MAKE_NV_PRINTF_STR("COPY")    : "COPY";
    else if (RM_ENGINE_TYPE_IS_NVDEC(engineType))  return bNvPrintfStr ? MAKE_NV_PRINTF_STR("NVDEC")   : "NVDEC";
    else if (RM_ENGINE_TYPE_IS_NVENC(engineType))  return bNvPrintfStr ? MAKE_NV_PRINTF_STR("NVENC")   : "NVENC";
    else if (RM_ENGINE_TYPE_IS_NVJPEG(engineType)) return bNvPrintfStr ? MAKE_NV_PRINTF_STR("NVJPEG")  : "NVJPEG";
    else if (RM_ENGINE_TYPE_IS_OFA(engineType))    return bNvPrintfStr ? MAKE_NV_PRINTF_STR("OFA")     : "OFA";
    else if (engineType == RM_ENGINE_TYPE_VP)      return bNvPrintfStr ? MAKE_NV_PRINTF_STR("VP")      : "VP";
    else if (engineType == RM_ENGINE_TYPE_ME)      return bNvPrintfStr ? MAKE_NV_PRINTF_STR("ME")      : "ME";
    else if (engineType == RM_ENGINE_TYPE_PPP)     return bNvPrintfStr ? MAKE_NV_PRINTF_STR("PPP")     : "PPP";
    else if (engineType == RM_ENGINE_TYPE_MPEG)    return bNvPrintfStr ? MAKE_NV_PRINTF_STR("MPEG")    : "MPEG";
    else if (engineType == RM_ENGINE_TYPE_SW)      return bNvPrintfStr ? MAKE_NV_PRINTF_STR("SW")      : "SW";
    else if (engineType == RM_ENGINE_TYPE_TSEC)    return bNvPrintfStr ? MAKE_NV_PRINTF_STR("TSEC")    : "TSEC";
    else if (engineType == RM_ENGINE_TYPE_VIC)     return bNvPrintfStr ? MAKE_NV_PRINTF_STR("VIC")     : "VIC";
    else if (engineType == RM_ENGINE_TYPE_MP)      return bNvPrintfStr ? MAKE_NV_PRINTF_STR("MP")      : "MP";
    else if (engineType == RM_ENGINE_TYPE_SEC2)    return bNvPrintfStr ? MAKE_NV_PRINTF_STR("SEC2")    : "SEC2";
    else if (engineType == RM_ENGINE_TYPE_HOST)    return bNvPrintfStr ? MAKE_NV_PRINTF_STR("HOST")    : "HOST";
    else if (engineType == RM_ENGINE_TYPE_DPU)     return bNvPrintfStr ? MAKE_NV_PRINTF_STR("DPU")     : "DPU";
    else if (engineType == RM_ENGINE_TYPE_PMU)     return bNvPrintfStr ? MAKE_NV_PRINTF_STR("PMU")     : "PMU";
    else if (engineType == RM_ENGINE_TYPE_FBFLCN)  return bNvPrintfStr ? MAKE_NV_PRINTF_STR("FBFLCN")  : "FBFLCN";
    else                                           return bNvPrintfStr ? MAKE_NV_PRINTF_STR("Unknown") : "Unknown";
}
