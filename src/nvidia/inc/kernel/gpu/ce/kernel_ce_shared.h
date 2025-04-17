/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef KERNEL_CE_SHARED_H
#define KERNEL_CE_SHARED_H

#include "gpu/gpu.h"

/**
 * Routines shared between CE and KCE.
 */

NvBool ceIsCeGrce(OBJGPU *pGpu, RM_ENGINE_TYPE ceEngineType);
NvBool ceIsPartneredWithGr(OBJGPU *pGpu, RM_ENGINE_TYPE ceEngineType, RM_ENGINE_TYPE grEngineType);
NvU32  ceCountGrCe(OBJGPU *pGpu);
void cePauseCeUtilsScheduling(OBJGPU *pGpu);
void ceResumeCeUtilsScheduling(OBJGPU *pGpu);

#endif
