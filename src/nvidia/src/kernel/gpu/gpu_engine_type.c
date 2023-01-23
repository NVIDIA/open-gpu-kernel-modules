/*
 * SPDX-FileCopyrightText: Copyright (c) 2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

ct_assert(RM_ENGINE_TYPE_LAST == NV2080_ENGINE_TYPE_LAST);

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
    NV_ASSERT_OR_RETURN(index < NV2080_ENGINE_TYPE_LAST, RM_ENGINE_TYPE_LAST);

    switch (index)
    {
        case NV2080_ENGINE_TYPE_NULL:       return RM_ENGINE_TYPE_NULL;
        case NV2080_ENGINE_TYPE_GR0:        return RM_ENGINE_TYPE_GR0;
        case NV2080_ENGINE_TYPE_GR1:        return RM_ENGINE_TYPE_GR1;
        case NV2080_ENGINE_TYPE_GR2:        return RM_ENGINE_TYPE_GR2;
        case NV2080_ENGINE_TYPE_GR3:        return RM_ENGINE_TYPE_GR3;
        case NV2080_ENGINE_TYPE_GR4:        return RM_ENGINE_TYPE_GR4;
        case NV2080_ENGINE_TYPE_GR5:        return RM_ENGINE_TYPE_GR5;
        case NV2080_ENGINE_TYPE_GR6:        return RM_ENGINE_TYPE_GR6;
        case NV2080_ENGINE_TYPE_GR7:        return RM_ENGINE_TYPE_GR7;
        case NV2080_ENGINE_TYPE_COPY0:      return RM_ENGINE_TYPE_COPY0;
        case NV2080_ENGINE_TYPE_COPY1:      return RM_ENGINE_TYPE_COPY1;
        case NV2080_ENGINE_TYPE_COPY2:      return RM_ENGINE_TYPE_COPY2;
        case NV2080_ENGINE_TYPE_COPY3:      return RM_ENGINE_TYPE_COPY3;
        case NV2080_ENGINE_TYPE_COPY4:      return RM_ENGINE_TYPE_COPY4;
        case NV2080_ENGINE_TYPE_COPY5:      return RM_ENGINE_TYPE_COPY5;
        case NV2080_ENGINE_TYPE_COPY6:      return RM_ENGINE_TYPE_COPY6;
        case NV2080_ENGINE_TYPE_COPY7:      return RM_ENGINE_TYPE_COPY7;
        case NV2080_ENGINE_TYPE_COPY8:      return RM_ENGINE_TYPE_COPY8;
        case NV2080_ENGINE_TYPE_COPY9:      return RM_ENGINE_TYPE_COPY9;
        case NV2080_ENGINE_TYPE_NVDEC0:     return RM_ENGINE_TYPE_NVDEC0;
        case NV2080_ENGINE_TYPE_NVDEC1:     return RM_ENGINE_TYPE_NVDEC1;
        case NV2080_ENGINE_TYPE_NVDEC2:     return RM_ENGINE_TYPE_NVDEC2;
        case NV2080_ENGINE_TYPE_NVDEC3:     return RM_ENGINE_TYPE_NVDEC3;
        case NV2080_ENGINE_TYPE_NVDEC4:     return RM_ENGINE_TYPE_NVDEC4;
        case NV2080_ENGINE_TYPE_NVDEC5:     return RM_ENGINE_TYPE_NVDEC5;
        case NV2080_ENGINE_TYPE_NVDEC6:     return RM_ENGINE_TYPE_NVDEC6;
        case NV2080_ENGINE_TYPE_NVDEC7:     return RM_ENGINE_TYPE_NVDEC7;
        case NV2080_ENGINE_TYPE_NVENC0:     return RM_ENGINE_TYPE_NVENC0;
        case NV2080_ENGINE_TYPE_NVENC1:     return RM_ENGINE_TYPE_NVENC1;
        case NV2080_ENGINE_TYPE_NVENC2:     return RM_ENGINE_TYPE_NVENC2;
        case NV2080_ENGINE_TYPE_VP:         return RM_ENGINE_TYPE_VP;
        case NV2080_ENGINE_TYPE_ME:         return RM_ENGINE_TYPE_ME;
        case NV2080_ENGINE_TYPE_PPP:        return RM_ENGINE_TYPE_PPP;
        case NV2080_ENGINE_TYPE_MPEG:       return RM_ENGINE_TYPE_MPEG;
        case NV2080_ENGINE_TYPE_SW:         return RM_ENGINE_TYPE_SW;
        case NV2080_ENGINE_TYPE_TSEC:       return RM_ENGINE_TYPE_TSEC;
        case NV2080_ENGINE_TYPE_VIC:        return RM_ENGINE_TYPE_VIC;
        case NV2080_ENGINE_TYPE_MP:         return RM_ENGINE_TYPE_MP;
        case NV2080_ENGINE_TYPE_SEC2:       return RM_ENGINE_TYPE_SEC2;
        case NV2080_ENGINE_TYPE_HOST:       return RM_ENGINE_TYPE_HOST;
        case NV2080_ENGINE_TYPE_DPU:        return RM_ENGINE_TYPE_DPU;
        case NV2080_ENGINE_TYPE_PMU:        return RM_ENGINE_TYPE_PMU;
        case NV2080_ENGINE_TYPE_FBFLCN:     return RM_ENGINE_TYPE_FBFLCN;
        case NV2080_ENGINE_TYPE_NVJPEG0:    return RM_ENGINE_TYPE_NVJPEG0;
        case NV2080_ENGINE_TYPE_NVJPEG1:    return RM_ENGINE_TYPE_NVJPEG1;
        case NV2080_ENGINE_TYPE_NVJPEG2:    return RM_ENGINE_TYPE_NVJPEG2;
        case NV2080_ENGINE_TYPE_NVJPEG3:    return RM_ENGINE_TYPE_NVJPEG3;
        case NV2080_ENGINE_TYPE_NVJPEG4:    return RM_ENGINE_TYPE_NVJPEG4;
        case NV2080_ENGINE_TYPE_NVJPEG5:    return RM_ENGINE_TYPE_NVJPEG5;
        case NV2080_ENGINE_TYPE_NVJPEG6:    return RM_ENGINE_TYPE_NVJPEG6;
        case NV2080_ENGINE_TYPE_NVJPEG7:    return RM_ENGINE_TYPE_NVJPEG7;
        case NV2080_ENGINE_TYPE_OFA:        return RM_ENGINE_TYPE_OFA;
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
    NV_ASSERT_OR_RETURN(index < RM_ENGINE_TYPE_LAST, NV2080_ENGINE_TYPE_LAST);

    switch (index)
    {
        case RM_ENGINE_TYPE_NULL:       return NV2080_ENGINE_TYPE_NULL;
        case RM_ENGINE_TYPE_GR0:        return NV2080_ENGINE_TYPE_GR0;
        case RM_ENGINE_TYPE_GR1:        return NV2080_ENGINE_TYPE_GR1;
        case RM_ENGINE_TYPE_GR2:        return NV2080_ENGINE_TYPE_GR2;
        case RM_ENGINE_TYPE_GR3:        return NV2080_ENGINE_TYPE_GR3;
        case RM_ENGINE_TYPE_GR4:        return NV2080_ENGINE_TYPE_GR4;
        case RM_ENGINE_TYPE_GR5:        return NV2080_ENGINE_TYPE_GR5;
        case RM_ENGINE_TYPE_GR6:        return NV2080_ENGINE_TYPE_GR6;
        case RM_ENGINE_TYPE_GR7:        return NV2080_ENGINE_TYPE_GR7;
        case RM_ENGINE_TYPE_COPY0:      return NV2080_ENGINE_TYPE_COPY0;
        case RM_ENGINE_TYPE_COPY1:      return NV2080_ENGINE_TYPE_COPY1;
        case RM_ENGINE_TYPE_COPY2:      return NV2080_ENGINE_TYPE_COPY2;
        case RM_ENGINE_TYPE_COPY3:      return NV2080_ENGINE_TYPE_COPY3;
        case RM_ENGINE_TYPE_COPY4:      return NV2080_ENGINE_TYPE_COPY4;
        case RM_ENGINE_TYPE_COPY5:      return NV2080_ENGINE_TYPE_COPY5;
        case RM_ENGINE_TYPE_COPY6:      return NV2080_ENGINE_TYPE_COPY6;
        case RM_ENGINE_TYPE_COPY7:      return NV2080_ENGINE_TYPE_COPY7;
        case RM_ENGINE_TYPE_COPY8:      return NV2080_ENGINE_TYPE_COPY8;
        case RM_ENGINE_TYPE_COPY9:      return NV2080_ENGINE_TYPE_COPY9;
        case RM_ENGINE_TYPE_NVDEC0:     return NV2080_ENGINE_TYPE_NVDEC0;
        case RM_ENGINE_TYPE_NVDEC1:     return NV2080_ENGINE_TYPE_NVDEC1;
        case RM_ENGINE_TYPE_NVDEC2:     return NV2080_ENGINE_TYPE_NVDEC2;
        case RM_ENGINE_TYPE_NVDEC3:     return NV2080_ENGINE_TYPE_NVDEC3;
        case RM_ENGINE_TYPE_NVDEC4:     return NV2080_ENGINE_TYPE_NVDEC4;
        case RM_ENGINE_TYPE_NVDEC5:     return NV2080_ENGINE_TYPE_NVDEC5;
        case RM_ENGINE_TYPE_NVDEC6:     return NV2080_ENGINE_TYPE_NVDEC6;
        case RM_ENGINE_TYPE_NVDEC7:     return NV2080_ENGINE_TYPE_NVDEC7;
        case RM_ENGINE_TYPE_NVENC0:     return NV2080_ENGINE_TYPE_NVENC0;
        case RM_ENGINE_TYPE_NVENC1:     return NV2080_ENGINE_TYPE_NVENC1;
        case RM_ENGINE_TYPE_NVENC2:     return NV2080_ENGINE_TYPE_NVENC2;
        case RM_ENGINE_TYPE_VP:         return NV2080_ENGINE_TYPE_VP;
        case RM_ENGINE_TYPE_ME:         return NV2080_ENGINE_TYPE_ME;
        case RM_ENGINE_TYPE_PPP:        return NV2080_ENGINE_TYPE_PPP;
        case RM_ENGINE_TYPE_MPEG:       return NV2080_ENGINE_TYPE_MPEG;
        case RM_ENGINE_TYPE_SW:         return NV2080_ENGINE_TYPE_SW;
        case RM_ENGINE_TYPE_TSEC:       return NV2080_ENGINE_TYPE_TSEC;
        case RM_ENGINE_TYPE_VIC:        return NV2080_ENGINE_TYPE_VIC;
        case RM_ENGINE_TYPE_MP:         return NV2080_ENGINE_TYPE_MP;
        case RM_ENGINE_TYPE_SEC2:       return NV2080_ENGINE_TYPE_SEC2;
        case RM_ENGINE_TYPE_HOST:       return NV2080_ENGINE_TYPE_HOST;
        case RM_ENGINE_TYPE_DPU:        return NV2080_ENGINE_TYPE_DPU;
        case RM_ENGINE_TYPE_PMU:        return NV2080_ENGINE_TYPE_PMU;
        case RM_ENGINE_TYPE_FBFLCN:     return NV2080_ENGINE_TYPE_FBFLCN;
        case RM_ENGINE_TYPE_NVJPEG0:    return NV2080_ENGINE_TYPE_NVJPEG0;
        case RM_ENGINE_TYPE_NVJPEG1:    return NV2080_ENGINE_TYPE_NVJPEG1;
        case RM_ENGINE_TYPE_NVJPEG2:    return NV2080_ENGINE_TYPE_NVJPEG2;
        case RM_ENGINE_TYPE_NVJPEG3:    return NV2080_ENGINE_TYPE_NVJPEG3;
        case RM_ENGINE_TYPE_NVJPEG4:    return NV2080_ENGINE_TYPE_NVJPEG4;
        case RM_ENGINE_TYPE_NVJPEG5:    return NV2080_ENGINE_TYPE_NVJPEG5;
        case RM_ENGINE_TYPE_NVJPEG6:    return NV2080_ENGINE_TYPE_NVJPEG6;
        case RM_ENGINE_TYPE_NVJPEG7:    return NV2080_ENGINE_TYPE_NVJPEG7;
        case RM_ENGINE_TYPE_OFA:        return NV2080_ENGINE_TYPE_OFA;
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
