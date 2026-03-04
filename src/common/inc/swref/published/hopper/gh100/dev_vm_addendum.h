/*
 * SPDX-FileCopyrightText: Copyright (c) 2022 NVIDIA CORPORATION & AFFILIATES
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

#ifndef __gh100_dev_vm_addendum_h__
#define __gh100_dev_vm_addendum_h__

//
// Compile time asserts in the source code files will ensure that
// these don't end up exceeding the range of the top level registers.
//

// Subtrees at CPU_INTR top level for UVM owned interrupts
#define NV_CPU_INTR_UVM_SUBTREE_START NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_TOP_SUBTREE(1)
#define NV_CPU_INTR_UVM_SUBTREE_LAST  NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_TOP_SUBTREE(1)

#define NV_CPU_INTR_UVM_SHARED_SUBTREE_START NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_TOP_SUBTREE(2)
#define NV_CPU_INTR_UVM_SHARED_SUBTREE_LAST NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_TOP_SUBTREE(2)

//
// Subtrees at CPU_INTR top level for all stall interrupts from host-driven
// engines
//
#define NV_CPU_INTR_STALL_SUBTREE_START NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_TOP_SUBTREE(3)
#define NV_CPU_INTR_STALL_SUBTREE_LAST_SWRL NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_TOP_SUBTREE(4)
#define NV_CPU_INTR_STALL_SUBTREE_LAST  NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_TOP_SUBTREE(5)

//
// CPU doorbell can be anything on GH100+! Define them the same as
// pre-GH100 for compatibility
//
#define NV_CTRL_CPU_DOORBELL_VECTORID_VALUE_CONSTANT                           0x00000081

#define NV_VIRTUAL_FUNCTION_PRIV_NON_REPLAYABLE_FAULT_SHADOW_BUFFER_PUT       NV_VIRTUAL_FUNCTION_PRIV_ACCESS_COUNTER_NOTIFY_BUFFER_LO
#define NV_VIRTUAL_FUNCTION_PRIV_NON_REPLAYABLE_FAULT_SHADOW_BUFFER_PUT_PTR   NV_VIRTUAL_FUNCTION_PRIV_ACCESS_COUNTER_NOTIFY_BUFFER_LO_BASE
#define NV_VIRTUAL_FUNCTION_PRIV_REPLAYABLE_FAULT_SHADOW_BUFFER_PUT           NV_VIRTUAL_FUNCTION_PRIV_ACCESS_COUNTER_NOTIFY_BUFFER_HI

#endif // __gh100_dev_vm_addendum_h__
