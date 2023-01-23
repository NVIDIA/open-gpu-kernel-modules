/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/*!
 * @brief Defines for simplifying SW accesses to the dev_ctrl interrupt tree.
 *        These are generic defines ued in addition to architecure-specific
 *        defines in dev_vm_addendum.h
 *
 */

//
// Notes on the terms used below:
// Subtree: The HW tree is a 64-way tree that consists of 2 TOP level interrupt
// registers, 32 bits each. Each of these 64 is referred to as a subtree.
// Leaf: Each of these 64 subtrees are associated with a pair of LEAF registers
// giving us a total of 128 LEAF registers.
// GPU vector: The 128 LEAF registers give us a total of (128*32) GPU vectors
// giving us a total of 4096 GPU vectors
// 

//
// Given a subtree index, the below macros give us the index of the TOP level
// register and the bit within the TOP level register to program for that
// subtree.
//
#define NV_CTRL_INTR_SUBTREE_TO_TOP_IDX(i)      ((i) / 32)
#define NV_CTRL_INTR_SUBTREE_TO_TOP_BIT(i)      ((i) % 32)

//
// Given a subtree index, the below macros give us the two LEAF register indices
// that correspond to that subtree.
//
#define NV_CTRL_INTR_SUBTREE_TO_LEAF_IDX_START(i)      ((i)*2)
#define NV_CTRL_INTR_SUBTREE_TO_LEAF_IDX_END(i)        (((i)*2) + 1)

#define NV_CTRL_INTR_LEAF_IDX_TO_SUBTREE(i)            ((i)/2)

//
// Given a LEAF register index, the below macros give us the range of GPU
// interrupt vectors that correspond to those leafs.
//
#define NV_CTRL_INTR_LEAF_IDX_TO_GPU_VECTOR_START(i)   ((i)*32)
#define NV_CTRL_INTR_LEAF_IDX_TO_GPU_VECTOR_END(i)     (((i)*32) + 31)

//
// Given a GPU interrupt vector, the below macros give us the index of the
// LEAF register and the bit within the LEAF register to program for that
// GPU interrupt vector.
//
#define NV_CTRL_INTR_GPU_VECTOR_TO_LEAF_REG(i)         ((i) / 32)
#define NV_CTRL_INTR_GPU_VECTOR_TO_LEAF_BIT(i)         ((i) % 32)

//
// Given a GPU interrupt vector, the below macro gives us the subtree in which
// it belongs.
//
#define NV_CTRL_INTR_GPU_VECTOR_TO_SUBTREE(i)          ((NV_CTRL_INTR_GPU_VECTOR_TO_LEAF_REG(i)) / 2)

//
// The max number of leaf registers we expect
// This is enforced to be greater than or equal to
// (NV_CTRL_INTR_SUBTREE_TO_LEAF_IDX_END(NV_CPU_INTR_STALL_SUBTREE_LAST) + 1)
// for the largest NV_CPU_INTR_STALL_SUBTREE_LAST
//
#define NV_MAX_INTR_LEAVES 12

// In SW, this specifies an invalid interrupt vector
#define NV_INTR_VECTOR_INVALID   (NV_U32_MAX)
