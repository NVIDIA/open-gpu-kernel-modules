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

#ifndef OS_STUB_H
#define OS_STUB_H

/**************** Resource Manager Defines and Structures ******************\
*                                                                           *
*       Extern definitions of all public stub function interfaces           *
*                                                                           *
\***************************************************************************/

#include "os/os.h"

//
// Each of these stub functions returns a different type. Used to
// stub out function pointers in OBJOS.
//
OSQADbgRegistryInit             stubOsQADbgRegistryInit;
OSnv_rdcr4                      stubOsnv_rdcr4;
OSnv_rdxcr0                     stubOsnv_rdxcr0;
OSnv_cpuid                      stubOsnv_cpuid;
OSnv_rdmsr                      stubOsnv_rdmsr;
OSnv_wrmsr                      stubOsnv_wrmsr;
OSRobustChannelsDefaultState    stubOsRobustChannelsDefaultState;
OSSpinLoop                      stubOsSpinLoop;
OSDbgBreakpointEnabled          stubOsDbgBreakpointEnabled;
OSQueueWorkItem                 stubOsQueueWorkItem;
OSQueueWorkItemWithFlags        stubOsQueueWorkItemWithFlags;
OSQueueSystemWorkItem           stubOsQueueSystemWorkItem;
OSSimEscapeWrite                stubOsSimEscapeWrite;
OSSimEscapeWriteBuffer          stubOsSimEscapeWriteBuffer;
OSSimEscapeRead                 stubOsSimEscapeRead;
OSSimEscapeReadBuffer           stubOsSimEscapeReadBuffer;
OSCallACPI_MXMX                 stubOsCallACPI_MXMX;
OSCallACPI_DSM                  stubOsCallACPI_DSM;
OSCallACPI_DDC                  stubOsCallACPI_DDC;
OSCallACPI_BCL                  stubOsCallACPI_BCL;
OSCallACPI_ON                   stubOsCallACPI_ON;
OSCallACPI_OFF                  stubOsCallACPI_OFF;
OSCallACPI_NVHG_GPUON           stubOsCallWMI_NVHG_GPUON;
OSCallACPI_NVHG_GPUOFF          stubOsCallWMI_NVHG_GPUOFF;
OSCallACPI_NVHG_GPUSTA          stubOsCallWMI_NVHG_GPUSTA;
OSCallACPI_NVHG_MXDS            stubOsCallWMI_NVHG_MXDS;
OSCallACPI_NVHG_MXMX            stubOsCallWMI_NVHG_MXMX;
OSCallACPI_NVHG_DOS             stubOsCallWMI_NVHG_DOS;
OSCallACPI_NVHG_ROM             stubOsCallWMI_NVHG_ROM;
OSCallACPI_NVHG_DCS             stubOsCallWMI_NVHG_DCS;
OSCallACPI_DOD                  stubOsCallWMI_DOD;
OSCheckCallback                 stubOsCheckCallback;
OSRCCallback                    stubOsRCCallback;

OSCallACPI_NBPS                 stubOsCallACPI_NBPS;
OSCallACPI_NBSL                 stubOsCallACPI_NBSL;
OSCallACPI_OPTM_GPUON           stubOsCallWMI_OPTM_GPUON;
OSSetupVBlank                   stubOsSetupVBlank;
OSObjectEventNotification       stubOsObjectEventNotification;
OSPageArrayGetPhysAddr          stubOsPageArrayGetPhysAddr;
OSInternalReserveFreeCallback   stubOsInternalReserveFreeCallback;
OSInternalReserveAllocCallback  stubOsInternalReserveAllocCallback;
OSGetUefiVariable               stubOsGetUefiVariable;
OSCallACPI_MXDS                 stubOsCallACPI_MXDS;
OSCallACPI_MXDM                 stubOsCallACPI_MXDM;
OSCallACPI_MXID                 stubOsCallACPI_MXID;
OSCallACPI_LRST                 stubOsCallACPI_LRST;

#endif // OS_STUB_H
