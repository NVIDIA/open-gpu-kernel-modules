/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef OSFUNCS_H
#define OSFUNCS_H

/**************** Resource Manager Defines and Structures ******************\
*                                                                           *
*       Declarations for the Operating System Specific Functions.           *
*                                                                           *
\***************************************************************************/

#include <os/os.h>

OSQueueWorkItem                 osQueueWorkItem;
OSQueueWorkItemWithFlags        osQueueWorkItemWithFlags;
OSQueueSystemWorkItem           osQueueSystemWorkItem;
OSDbgBreakpointEnabled          osDbgBreakpointEnabled;

void*                           osGetStereoDongleInterface(void);

OSCallACPI_DSM                  osCallACPI_DSM;
OSCallACPI_DDC                  osCallACPI_DDC;
OSCallACPI_NVHG_ROM             osCallACPI_NVHG_ROM;
OSCallACPI_DOD                  osCallACPI_DOD;
OSCallACPI_MXDS                 osCallACPI_MXDS;
OSCallACPI_MXDM                 osCallACPI_MXDM;

#if defined(NVCPU_X86_64)
OSnv_rdcr4                      nv_rdcr4;
NvU64                           nv_rdcr3(OBJOS *);
OSnv_cpuid                      nv_cpuid;
#endif

#endif // OSFUNCS_H
