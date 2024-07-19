/*
 * SPDX-FileCopyrightText: Copyright (c) 1999-2018 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _OS_CUSTOM_H_
#define _OS_CUSTOM_H_

/*!
 * @file  os_custom.h
 * @brief OS module specific definitions for this OS 
 */

#include <os-interface.h>

// File modes, added for NVIDIA capabilities.
#define OS_RUSR 00400                         // read permission, owner
#define OS_WUSR 00200                         // write permission, owner
#define OS_XUSR 00100                         // execute/search permission, owner
#define OS_RWXU (OS_RUSR | OS_WUSR | OS_XUSR) // read, write, execute/search, owner
#define OS_RGRP 00040                         // read permission, group
#define OS_WGRP 00020                         // write permission, group
#define OS_XGRP 00010                         // execute/search permission, group
#define OS_RWXG (OS_RGRP | OS_WGRP | OS_XGRP) // read, write, execute/search, group
#define OS_ROTH 00004                         // read permission, other
#define OS_WOTH 00002                         // write permission, other
#define OS_XOTH 00001                         // execute/search permission, other
#define OS_RWXO (OS_ROTH | OS_WOTH | OS_XOTH) // read, write, execute/search, other
#define OS_RUGO (OS_RUSR | OS_RGRP | OS_ROTH)
#define OS_WUGO (OS_WUSR | OS_WGRP | OS_WOTH)
#define OS_XUGO (OS_XUSR | OS_XGRP | OS_XOTH)

// Trigger for collecting GPU state for later extraction.
NV_STATUS   RmLogGpuCrash(OBJGPU *);

// This is callback function in the miniport.
// The argument is a device extension, and must be cast as such to be useful.
typedef void (*MINIPORT_CALLBACK)(void*);

NV_STATUS   osPackageRegistry(OBJGPU *pGpu, PACKED_REGISTRY_TABLE *, NvU32 *);

#endif // _OS_CUSTOM_H_
