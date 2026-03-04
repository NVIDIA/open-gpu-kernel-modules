/*
 * SPDX-FileCopyrightText: Copyright (c) 2017-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#pragma once

#include <nvtypes.h>

//
// This file was generated with FINN, an NVIDIA coding tool.
// Source file:      class/clc58b.finn
//

#define TURING_VMMU_A (0xc58bU) /* finn: Evaluated from "TURING_VMMU_A_ALLOCATION_PARAMETERS_MESSAGE_ID" */

/*
 * @brief NvAlloc parameters for TuringVmmuA class
 *
 * This class represents mapping between guest physical and system physical.
 * Will also be used to represent VF specific state for a given guest.
 *
 * gfid [in]
 *       GFID of VF
 */

#define TURING_VMMU_A_ALLOCATION_PARAMETERS_MESSAGE_ID (0xc58bU)

typedef struct TURING_VMMU_A_ALLOCATION_PARAMETERS {
    NvHandle hHostVgpuDevice;
} TURING_VMMU_A_ALLOCATION_PARAMETERS;

