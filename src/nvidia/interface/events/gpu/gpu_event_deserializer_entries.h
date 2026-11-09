/*
 * SPDX-FileCopyrightText: Copyright (c) 2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

GPU_EVENT_DESERIALIZER(
    "GPU",
    GPU_OP_EVENT_CODE_DRIVER_INIT,
    gpudrvinitDeserialize)

GPU_EVENT_DESERIALIZER(
    "GPU-BUS",
    BUS_OP_EVENT_CODE_C2C_CONTAINMENT,
    busc2ccontnmtDeserialize)

GPU_EVENT_DESERIALIZER(
    "GPU-BUS",
    BUS_OP_EVENT_CODE_GPU_FALLEN_OFF_BUS,
    busgpufobDeserialize)

GPU_EVENT_DESERIALIZER(
    "GPU-MEMSYS",
    MEM_SYS_OP_EVENT_CODE_MAINT_OP_ERROR,
    memsystoDeserialize)

GPU_EVENT_DESERIALIZER(
    "GPU-FSP",
    FSP_OP_EVENT_CODE_BOOT_TIMEOUT,
    fspboottoDeserialize)

GPU_EVENT_DESERIALIZER(
    "GPU-FSP",
    FSP_OP_EVENT_CODE_FUSE_ERROR,
    fspfuseerrDeserialize)

GPU_EVENT_DESERIALIZER(
    "GPU-RAS",
    RAS_OP_EVENT_CODE_ROW_REMAPPING_PENDING,
    rrpendingDeserialize)

GPU_EVENT_DESERIALIZER(
    "GPU-RAS",
    RAS_OP_EVENT_CODE_ROW_REMAPPING_FAILURE_TABLE_FULL,
    rrtablefullDeserialize)
