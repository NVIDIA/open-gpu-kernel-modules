/*
 * SPDX-FileCopyrightText: Copyright (c) 2016-2018 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file: nv_vgpu_types.finn
//




/* XAPIGEN - this file is not suitable for (nor needed by) xapigen.         */
/*           Rather than #ifdef out every such include in every sdk         */
/*           file, punt here.                                               */
#include "nvtypes.h"
 /* ! XAPIGEN */

#define VM_UUID_SIZE            16
#define INVALID_VGPU_DEV_INST   0xFFFFFFFFU
#define MAX_VGPU_DEVICES_PER_VM 16U

/* This enum represents the current state of guest dependent fields */
typedef enum GUEST_VM_INFO_STATE {
    GUEST_VM_INFO_STATE_UNINITIALIZED = 0,
    GUEST_VM_INFO_STATE_INITIALIZED = 1,
} GUEST_VM_INFO_STATE;

/* This enum represents types of VM identifiers */
typedef enum VM_ID_TYPE {
    VM_ID_DOMAIN_ID = 0,
    VM_ID_UUID = 1,
} VM_ID_TYPE;

/* This structure represents VM identifier */
typedef union VM_ID {
    NvU8 vmUuid[VM_UUID_SIZE];
    NV_DECLARE_ALIGNED(NvU64 vmId, 8);
} VM_ID;
