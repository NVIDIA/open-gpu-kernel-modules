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

#ifndef KERNEL_SM_DEBUGGER_EXCEPTION_H
#define KERNEL_SM_DEBUGGER_EXCEPTION_H

#include "utils/nv_enum.h"
#include "nvctassert.h"
#include "nvmisc.h"

#include "ctrl/ctrl83de.h"

#define SMDBG_EXCEPTION_TYPE_DEF(x)                                             \
    NV_ENUM_ENTRY(x, SMDBG_EXCEPTION_TYPE_FATAL,                0)              \
    NV_ENUM_ENTRY(x, SMDBG_EXCEPTION_TYPE_TRAP,                 1)              \
    NV_ENUM_ENTRY(x, SMDBG_EXCEPTION_TYPE_SINGLE_STEP,          2)              \
    NV_ENUM_ENTRY(x, SMDBG_EXCEPTION_TYPE_INT,                  3)              \
    NV_ENUM_ENTRY(x, SMDBG_EXCEPTION_TYPE_CILP,                 4)              \
    NV_ENUM_ENTRY(x, SMDBG_EXCEPTION_TYPE_PREEMPTION_STARTED,   5)

NV_ENUM_DEF(SMDBG_EXCEPTION_TYPE, SMDBG_EXCEPTION_TYPE_DEF);
ct_assert(NV_ENUM_IS_CONTIGUOUS(SMDBG_EXCEPTION_TYPE));

ct_assert(NVBIT32(SMDBG_EXCEPTION_TYPE_FATAL) ==
          NV83DE_CTRL_DEBUG_SET_EXCEPTION_MASK_FATAL);
ct_assert(NVBIT32(SMDBG_EXCEPTION_TYPE_TRAP) ==
          NV83DE_CTRL_DEBUG_SET_EXCEPTION_MASK_TRAP);
ct_assert(NVBIT32(SMDBG_EXCEPTION_TYPE_SINGLE_STEP) ==
          NV83DE_CTRL_DEBUG_SET_EXCEPTION_MASK_SINGLE_STEP);
ct_assert(NVBIT32(SMDBG_EXCEPTION_TYPE_INT) ==
          NV83DE_CTRL_DEBUG_SET_EXCEPTION_MASK_INT);
ct_assert(NVBIT32(SMDBG_EXCEPTION_TYPE_CILP) ==
          NV83DE_CTRL_DEBUG_SET_EXCEPTION_MASK_CILP);
ct_assert(NVBIT32(SMDBG_EXCEPTION_TYPE_PREEMPTION_STARTED) ==
          NV83DE_CTRL_DEBUG_SET_EXCEPTION_MASK_PREEMPTION_STARTED);

#endif // KERNEL_SM_DEBUGGER_EXCEPTION_H
