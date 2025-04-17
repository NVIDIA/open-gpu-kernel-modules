/*
 * SPDX-FileCopyrightText: Copyright (c) 2024 NVIDIA CORPORATION & AFFILIATES
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

#ifndef __gb102_dev_vm_addendum_h__
#define __gb102_dev_vm_addendum_h__

#define NV_VIRTUAL_FUNCTION_PRIV_NON_REPLAYABLE_FAULT_SHADOW_BUFFER_PUT       NV_VIRTUAL_FUNCTION_PRIV_ACCESS_COUNTER_NOTIFY_BUFFER_LO
#define NV_VIRTUAL_FUNCTION_PRIV_NON_REPLAYABLE_FAULT_SHADOW_BUFFER_PUT_PTR   NV_VIRTUAL_FUNCTION_PRIV_ACCESS_COUNTER_NOTIFY_BUFFER_LO_BASE
#define NV_VIRTUAL_FUNCTION_PRIV_REPLAYABLE_FAULT_SHADOW_BUFFER_PUT           NV_VIRTUAL_FUNCTION_PRIV_ACCESS_COUNTER_NOTIFY_BUFFER_HI

#endif // __gb102_dev_vm_addendum_h__
