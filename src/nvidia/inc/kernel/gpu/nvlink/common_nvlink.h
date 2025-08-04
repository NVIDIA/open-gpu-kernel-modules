/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef COMMON_NVLINK_H
#define COMMON_NVLINK_H

#include "core/core.h"
#include "kernel/gpu/nvlink/kernel_nvlink.h"
#include "kernel/gpu/nvlink/kernel_ioctrl.h"
#include "utils/nvbitvector.h"

#include "ctrl/ctrl2080/ctrl2080nvlink.h" // rmcontrol params

MAKE_BITVECTOR(NV2080_NVLINK_BIT_VECTOR, NV2080_CTRL_NVLINK_MAX_LINKS);

NV_STATUS nvlinkCtrlCmdBusGetNvlinkCaps(OBJGPU *pGpu, NV2080_CTRL_CMD_NVLINK_GET_NVLINK_CAPS_PARAMS *pParams);

NV_STATUS
convertMaskToBitVector(NvU64 inputLinkMask, NV2080_NVLINK_BIT_VECTOR *pLocalLinkMask);

NV_STATUS
convertBitVectorToLinkMask32(NV2080_NVLINK_BIT_VECTOR *pBitVector, NvU32 *linkMask);

NV_STATUS
convertBitVectorToLinkMasks(NV2080_NVLINK_BIT_VECTOR *pLocalLinkMask,
                            void *pOutputLinkMask1, NvU32 outputLinkMask1Size,
                            NV2080_CTRL_NVLINK_LINK_MASK *pOutputLinkMask2);

NV_STATUS
convertLinkMasksToBitVector(const void *pLinkMask1, NvU32 linkMask1Size,
                            const NV2080_CTRL_NVLINK_LINK_MASK *pLinkMask2,
                            NV2080_NVLINK_BIT_VECTOR *pOutputBitVector);

#endif // COMMON_NVLINK_H
