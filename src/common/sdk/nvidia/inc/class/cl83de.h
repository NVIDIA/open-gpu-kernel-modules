/*
 * SPDX-FileCopyrightText: Copyright (c) 2010-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      class/cl83de.finn
//

#define GT200_DEBUGGER (0x83deU) /* finn: Evaluated from "NV83DE_ALLOC_PARAMETERS_MESSAGE_ID" */

/*
 * Creating the GT200_DEBUGGER object:
 * - The debug object is instantiated as a child of either the compute or the
 *   3D-class object.
 * - The Cuda/GR debugger uses the NV83DE_ALLOC_PARAMETERS to fill in the Client
 *   and 3D-Class handles of the debuggee and passes this to the NvRmAlloc.
 *   e.g:
        NV83DE_ALLOC_PARAMETERS params;
 *      memset (&params, 0, sizeof (NV83DE_ALLOC_PARAMETERS));
 *      params.hAppClient = DebuggeeClient;
 *      params.hClass3dObject = 3DClassHandle;
 *      NvRmAlloc(hDebuggerClient, hDebuggerClient, hDebugger, GT200_DEBUGGER, &params);
 */

#define NV83DE_ALLOC_PARAMETERS_MESSAGE_ID (0x83deU)

typedef struct NV83DE_ALLOC_PARAMETERS {
    NvHandle hDebuggerClient_Obsolete;  // No longer supported (must be zero)
    NvHandle hAppClient;
    NvHandle hClass3dObject;
} NV83DE_ALLOC_PARAMETERS;

