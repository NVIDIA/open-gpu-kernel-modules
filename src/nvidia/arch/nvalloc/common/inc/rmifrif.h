/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2012 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _RMIFRIF_H_
#define _RMIFRIF_H_

/*!
 * @file   rmifrif.h
 * @brief  Defines structures and interfaces common between RM and
 *         Init-From-Rom (IFR).
 *
 * For systems supporting GC6 that have on-board VBIOS ROMs, IFR is used
 * to expedite several parts of GC6 exit in parallel with PEX init.
 *
 * After running devinit using a PMU ucode image loaded from the ROM itself,
 * parts of RM stateLoad can be done using RM's ucode image. This is
 * achieved by loading RM PMU ucode directly from FB. The primary difficulties
 * are how to find RM's PMU ucode and how to bootstrap it.
 *
 * We use the simple approach of allocating a fixed buffer near the
 * top of FB that contains the information required to bootstrap RM's PMU
 * image. This buffer is called the RM_IFR_GC6_CTX.
 *
 * The buffer is allocated within RM's reserved memory space, directly before
 * the VBIOS workspace (if any is present).  Since the VBIOS workspace is
 * always a multiple of 64K, RM enforces that the offset between top of memory
 * and the end of the buffer is 64K.  This way the IFR code can start
 * from the top of memory and search downwards in 64K decrements.
 *
 * A small header is placed at the end of the buffer which contains a
 * string signature identifying the buffer and other data needed to find the
 * remaining context data.
 *
 *        Top_Of-FB  /---------------------\ <-
 *                   |                     |   \
 *                   | (VBIOS_Workspace)   |   | END_OFFSET
 *                   |                     |   /
 *                   |---------------------| <-
                     |                     |   \
 *                   | GSP FW (if present) |   | pFbHalPvtInfo->gspFwSizeBytes
 *                   |                     |   /
 *                   |---------------------| <-
 *                   | RM_IFR_GC6_CTX_HDR  |   \
 *                   |---------------------|   |
 *                   | (Padding)           |   | RM_IFR_GC6_CTX_HDR.bufferSize
 *                   |---------------------|   |
 *                   | Sequence Data       |   /
 *                   |---------------------| <-
 *                   |                     |
 *                   |                     |
 *                   |                     |
 *                   |                     |
 *        0x00000000 \---------------------/
 *
 * To simplify the RM PMU bootstrap process and decrease IFR maintainence
 * cost, the bootstrap process is encoded as a sequence script, leveraging
 * a small subset of RM's PMU_SEQ_INST interface (see pmuseqinst.h).
 * Register writes are captured during the initial (CPU-driven) RM PMU bootstrap
 * and saved into a sequence for replay during GC6 exit.
 *
 * Only the following opcodes are supported currently:
 *      NV_PMU_SEQ_WRITE_REG_OPC - (multi-)register write
 *      NV_PMU_SEQ_EXIT_OPC      - sequence done
 *
 */

/*!
 * Header structure which identifies the GC6 context buffer.
 */
typedef struct
{
    NvU8  signature[12]; // RM_IFR_GC6_CTX_SIGNATURE
    NvU32 bufferSize;    // Size of the entire context buffer in bytes
    NvU32 seqSizeWords;  // Number of 32-bit words of sequence data
    NvU32 checksum;      // 32-bit chunk checksum of the sequence data
} RM_IFR_GC6_CTX_HDR, *PRM_IFR_GC6_CTX_HDR;

/*!
 * String signature that IFR searches for to find the GC6 context buffer.
 */
#define RM_IFR_GC6_CTX_SIGNATURE            "GC6_CTX_HDR" // 12 bytes

/*!
 * Alignment of the offset between top of memory and the end of the
 * GC6 context buffer (which is also the end of the header).
 */
#define RM_IFR_GC6_CTX_END_OFFSET_ALIGNMENT 0x10000       // 64KB

/*!
 * Maximum offset between top of memory and the end of the
 * GC6 context buffer. This is meant to be a loose upper bound preventing
 * scanning of the whole of memory (e.g. when something goes wrong).
 */
#define RM_IFR_GC6_CTX_END_OFFSET_MAX                0x1000000     // 16MB

/*!
 * Maximum size of the context data in bytes.
 * This is limited by FECS falcon DMEM size (4K on Kepler).
 * The buffer must fit within DMEM together with stack and other global data.
 */
#define RM_IFR_GC6_CTX_DATA_MAX_SIZE        2048          // 2KB

#endif // _RMIFRIF_H_
