/*
 * SPDX-FileCopyrightText: Copyright (c) 2010-2014 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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


#ifndef _cl83de_h_
#define _cl83de_h_

#ifdef __cplusplus
extern "C" {
#endif

#include "nvtypes.h"

#define  GT200_DEBUGGER                                            (0x000083de)

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

typedef struct {
    NvHandle    hDebuggerClient_Obsolete;  // No longer supported (must be zero)
    NvHandle    hAppClient;
    NvHandle    hClass3dObject;
} NV83DE_ALLOC_PARAMETERS;

#ifdef __cplusplus
};     /* extern "C" */
#endif

#endif /* _cl83de_h_ */

