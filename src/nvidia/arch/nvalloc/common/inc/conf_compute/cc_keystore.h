/*
 * SPDX-FileCopyrightText: Copyright (c) 2023-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef CC_KEYSTORE_H
#define CC_KEYSTORE_H

//
// Keys are typically grouped in two's so that the first key is used for CPU-to-GPU encryption
// and the second key is used for GPU-to-CPU encryption.
//

// Keyspace identifiers.
enum
{
    CC_KEYSPACE_GSP,
    CC_KEYSPACE_SEC2,
    CC_KEYSPACE_LCE0,
    CC_KEYSPACE_LCE1,
    CC_KEYSPACE_LCE2,
    CC_KEYSPACE_LCE3,
    CC_KEYSPACE_LCE4,
    CC_KEYSPACE_LCE5,
    CC_KEYSPACE_LCE6,
    CC_KEYSPACE_LCE7,
    CC_KEYSPACE_SIZE // This is always the last element.
};

enum
{
    CC_LKEYID_CPU_GSP_LOCKED_RPC,
    CC_LKEYID_GSP_CPU_LOCKED_RPC,
    CC_LKEYID_CPU_GSP_DMA,
    CC_LKEYID_GSP_CPU_DMA,
    CC_LKEYID_CPU_GSP_RESERVED1,
    CC_LKEYID_GSP_CPU_REPLAYABLE_FAULT,
    CC_LKEYID_CPU_GSP_RESERVED2,
    CC_LKEYID_GSP_CPU_NON_REPLAYABLE_FAULT,
    CC_LKEYID_SEC2_GSP_LOCKED_RPC,
    CC_LKEYID_GSP_SEC2_LOCKED_RPC,
    CC_KEYSPACE_GSP_SIZE // This is always the last element.
};
// The fault buffers only support GPU-to-CPU encryption, so the CPU-to-GPU encryption slot
// is left reserved.

#define CC_LKEYID_GSP_CPU_LOCKED_RPC_STR           "gsp_cpu_locked_rpc"
#define CC_LKEYID_CPU_GSP_LOCKED_RPC_STR           "cpu_gsp_locked_rpc"
#define CC_LKEYID_GSP_CPU_DMA_STR                  "gsp_cpu_dma"
#define CC_LKEYID_CPU_GSP_DMA_STR                  "cpu_gsp_dma"
#define CC_LKEYID_GSP_CPU_REPLAYABLE_FAULT_STR     "gsp_cpu_replayable_fault"
#define CC_LKEYID_GSP_CPU_NON_REPLAYABLE_FAULT_STR "gsp_cpu_non_replayable_fault"

enum
{
    CC_LKEYID_CPU_SEC2_DATA_USER,
    CC_LKEYID_CPU_SEC2_HMAC_USER,
    CC_LKEYID_CPU_SEC2_DATA_KERN,
    CC_LKEYID_CPU_SEC2_HMAC_KERN,
    CC_LKEYID_CPU_SEC2_DATA_SCRUBBER,
    CC_LKEYID_CPU_SEC2_HMAC_SCRUBBER,
    CC_KEYSPACE_SEC2_SIZE // This is always the last element.
};

#define CC_LKEYID_CPU_SEC2_DATA_USER_STR     "cpu_sec2_data_user"
#define CC_LKEYID_CPU_SEC2_HMAC_USER_STR     "cpu_sec2_hmac_user"
#define CC_LKEYID_CPU_SEC2_DATA_KERN_STR     "cpu_sec2_data_kernel"
#define CC_LKEYID_CPU_SEC2_HMAC_KERN_STR     "cpu_sec2_hmac_kernel"
#define CC_LKEYID_CPU_SEC2_DATA_SCRUBBER_STR "cpu_sec2_data_scrubber"
#define CC_LKEYID_CPU_SEC2_HMAC_SCRUBBER_STR "cpu_sec2_hmac_scrubber"

enum
{
    CC_LKEYID_LCE_H2D_USER,
    CC_LKEYID_LCE_D2H_USER,
    CC_LKEYID_LCE_H2D_KERN,
    CC_LKEYID_LCE_D2H_KERN,
    CC_LKEYID_LCE_H2D_P2P,
    CC_LKEYID_LCE_D2H_P2P,
    CC_KEYSPACE_LCE_SIZE // This is always the last element.
};

#define CC_KEYSPACE_TOTAL_SIZE (CC_KEYSPACE_GSP_SIZE + CC_KEYSPACE_SEC2_SIZE + (8 * CC_KEYSPACE_LCE_SIZE))

#define CC_LKEYID_LCE0_H2D_USER_STR "Lce0_h2d_user"
#define CC_LKEYID_LCE0_D2H_USER_STR "Lce0_d2h_user"
#define CC_LKEYID_LCE0_H2D_KERN_STR "Lce0_h2d_kernel"
#define CC_LKEYID_LCE0_D2H_KERN_STR "Lce0_d2h_kernel"
#define CC_LKEYID_LCE0_H2D_P2P_STR  "Lce0_h2d_p2p"
#define CC_LKEYID_LCE0_D2H_P2P_STR  "Lce0_d2h_p2p"

#define CC_LKEYID_LCE1_H2D_USER_STR "Lce1_h2d_user"
#define CC_LKEYID_LCE1_D2H_USER_STR "Lce1_d2h_user"
#define CC_LKEYID_LCE1_H2D_KERN_STR "Lce1_h2d_kernel"
#define CC_LKEYID_LCE1_D2H_KERN_STR "Lce1_d2h_kernel"
#define CC_LKEYID_LCE1_H2D_P2P_STR  "Lce1_h2d_p2p"
#define CC_LKEYID_LCE1_D2H_P2P_STR  "Lce1_d2h_p2p"

#define CC_LKEYID_LCE2_H2D_USER_STR "Lce2_h2d_user"
#define CC_LKEYID_LCE2_D2H_USER_STR "Lce2_d2h_user"
#define CC_LKEYID_LCE2_H2D_KERN_STR "Lce2_h2d_kernel"
#define CC_LKEYID_LCE2_D2H_KERN_STR "Lce2_d2h_kernel"
#define CC_LKEYID_LCE2_H2D_P2P_STR  "Lce2_h2d_p2p"
#define CC_LKEYID_LCE2_D2H_P2P_STR  "Lce2_d2h_p2p"

#define CC_LKEYID_LCE3_H2D_USER_STR "Lce3_h2d_user"
#define CC_LKEYID_LCE3_D2H_USER_STR "Lce3_d2h_user"
#define CC_LKEYID_LCE3_H2D_KERN_STR "Lce3_h2d_kernel"
#define CC_LKEYID_LCE3_D2H_KERN_STR "Lce3_d2h_kernel"
#define CC_LKEYID_LCE3_H2D_P2P_STR  "Lce3_h2d_p2p"
#define CC_LKEYID_LCE3_D2H_P2P_STR  "Lce3_d2h_p2p"

#define CC_LKEYID_LCE4_H2D_USER_STR "Lce4_h2d_user"
#define CC_LKEYID_LCE4_D2H_USER_STR "Lce4_d2h_user"
#define CC_LKEYID_LCE4_H2D_KERN_STR "Lce4_h2d_kernel"
#define CC_LKEYID_LCE4_D2H_KERN_STR "Lce4_d2h_kernel"
#define CC_LKEYID_LCE4_H2D_P2P_STR  "Lce4_h2d_p2p"
#define CC_LKEYID_LCE4_D2H_P2P_STR  "Lce4_d2h_p2p"

#define CC_LKEYID_LCE5_H2D_USER_STR "Lce5_h2d_user"
#define CC_LKEYID_LCE5_D2H_USER_STR "Lce5_d2h_user"
#define CC_LKEYID_LCE5_H2D_KERN_STR "Lce5_h2d_kernel"
#define CC_LKEYID_LCE5_D2H_KERN_STR "Lce5_d2h_kernel"
#define CC_LKEYID_LCE5_H2D_P2P_STR  "Lce5_h2d_p2p"
#define CC_LKEYID_LCE5_D2H_P2P_STR  "Lce5_d2h_p2p"

#define CC_LKEYID_LCE6_H2D_USER_STR "Lce6_h2d_user"
#define CC_LKEYID_LCE6_D2H_USER_STR "Lce6_d2h_user"
#define CC_LKEYID_LCE6_H2D_KERN_STR "Lce6_h2d_kernel"
#define CC_LKEYID_LCE6_D2H_KERN_STR "Lce6_d2h_kernel"
#define CC_LKEYID_LCE6_H2D_P2P_STR  "Lce6_h2d_p2p"
#define CC_LKEYID_LCE6_D2H_P2P_STR  "Lce6_d2h_p2p"

#define CC_LKEYID_LCE7_H2D_USER_STR "Lce7_h2d_user"
#define CC_LKEYID_LCE7_D2H_USER_STR "Lce7_d2h_user"
#define CC_LKEYID_LCE7_H2D_KERN_STR "Lce7_h2d_kernel"
#define CC_LKEYID_LCE7_D2H_KERN_STR "Lce7_d2h_kernel"
#define CC_LKEYID_LCE7_H2D_P2P_STR  "Lce7_h2d_p2p"
#define CC_LKEYID_LCE7_D2H_P2P_STR  "Lce7_d2h_p2p"

// Generate a global key ID from a keyspace (a) and local key ID (b).
#define CC_GKEYID_GEN(a, b) (NvU32)(((a) << 16) | (b))

// Get the keyspace component from a global key ID.
#define CC_GKEYID_GET_KEYSPACE(a) (NvU16)((a) >> 16)

// Get the local key ID from a global key ID.
#define CC_GKEYID_GET_LKEYID(a) (NvU16)((a) & 0xffff)

// Decrement/increment the local key ID portion of a global key ID.
#define CC_GKEYID_DEC_LKEYID(a) CC_GKEYID_GEN(CC_GKEYID_GET_KEYSPACE((a)), CC_GKEYID_GET_LKEYID((a)) - 1)
#define CC_GKEYID_INC_LKEYID(a) CC_GKEYID_GEN(CC_GKEYID_GET_KEYSPACE((a)), CC_GKEYID_GET_LKEYID((a)) + 1)

// Get the unqiue string from a global key ID.
#define CC_GKEYID_GET_STR(a) \
    (CC_GKEYID_GET_KEYSPACE(a) == CC_KEYSPACE_GSP) ? \
        (CC_GKEYID_GET_LKEYID(a) == CC_LKEYID_GSP_CPU_LOCKED_RPC) ? \
            CC_LKEYID_GSP_CPU_LOCKED_RPC_STR : \
        (CC_GKEYID_GET_LKEYID(a) == CC_LKEYID_CPU_GSP_LOCKED_RPC) ? \
            CC_LKEYID_CPU_GSP_LOCKED_RPC_STR : \
        (CC_GKEYID_GET_LKEYID(a) == CC_LKEYID_GSP_CPU_DMA) ? \
            CC_LKEYID_GSP_CPU_DMA_STR : \
        (CC_GKEYID_GET_LKEYID(a) == CC_LKEYID_CPU_GSP_DMA) ? \
            CC_LKEYID_CPU_GSP_DMA_STR : \
        (CC_GKEYID_GET_LKEYID(a) == CC_LKEYID_GSP_CPU_REPLAYABLE_FAULT) ? \
            CC_LKEYID_GSP_CPU_REPLAYABLE_FAULT_STR : \
        (CC_GKEYID_GET_LKEYID(a) == CC_LKEYID_GSP_CPU_NON_REPLAYABLE_FAULT) ? \
            CC_LKEYID_GSP_CPU_NON_REPLAYABLE_FAULT_STR : NULL : \
    (CC_GKEYID_GET_KEYSPACE(a) == CC_KEYSPACE_SEC2) ? \
        (CC_GKEYID_GET_LKEYID(a) == CC_LKEYID_CPU_SEC2_DATA_USER) ? \
            CC_LKEYID_CPU_SEC2_DATA_USER_STR : \
        (CC_GKEYID_GET_LKEYID(a) == CC_LKEYID_CPU_SEC2_HMAC_USER) ? \
            CC_LKEYID_CPU_SEC2_HMAC_USER_STR : \
        (CC_GKEYID_GET_LKEYID(a) == CC_LKEYID_CPU_SEC2_DATA_KERN) ? \
            CC_LKEYID_CPU_SEC2_DATA_KERN_STR : \
        (CC_GKEYID_GET_LKEYID(a) == CC_LKEYID_CPU_SEC2_HMAC_KERN) ? \
            CC_LKEYID_CPU_SEC2_HMAC_KERN_STR : \
        (CC_GKEYID_GET_LKEYID(a) == CC_LKEYID_CPU_SEC2_DATA_SCRUBBER) ? \
            CC_LKEYID_CPU_SEC2_DATA_SCRUBBER_STR : \
        (CC_GKEYID_GET_LKEYID(a) == CC_LKEYID_CPU_SEC2_HMAC_SCRUBBER) ? \
            CC_LKEYID_CPU_SEC2_HMAC_SCRUBBER_STR : NULL : \
    (CC_GKEYID_GET_KEYSPACE(a) == CC_KEYSPACE_LCE0) ? \
        (CC_GKEYID_GET_LKEYID(a) == CC_LKEYID_LCE_H2D_USER) ? \
            CC_LKEYID_LCE0_H2D_USER_STR : \
        (CC_GKEYID_GET_LKEYID(a) == CC_LKEYID_LCE_D2H_USER) ? \
            CC_LKEYID_LCE0_D2H_USER_STR : \
        (CC_GKEYID_GET_LKEYID(a) == CC_LKEYID_LCE_H2D_KERN) ? \
            CC_LKEYID_LCE0_H2D_KERN_STR : \
        (CC_GKEYID_GET_LKEYID(a) == CC_LKEYID_LCE_D2H_KERN) ? \
            CC_LKEYID_LCE0_D2H_KERN_STR : \
        (CC_GKEYID_GET_LKEYID(a) == CC_LKEYID_LCE_H2D_P2P) ? \
            CC_LKEYID_LCE0_H2D_P2P_STR : \
        (CC_GKEYID_GET_LKEYID(a) == CC_LKEYID_LCE_D2H_P2P) ? \
            CC_LKEYID_LCE0_D2H_P2P_STR : NULL : \
    (CC_GKEYID_GET_KEYSPACE(a) == CC_KEYSPACE_LCE1) ? \
        (CC_GKEYID_GET_LKEYID(a) == CC_LKEYID_LCE_H2D_USER) ? \
            CC_LKEYID_LCE1_H2D_USER_STR : \
        (CC_GKEYID_GET_LKEYID(a) == CC_LKEYID_LCE_D2H_USER) ? \
            CC_LKEYID_LCE1_D2H_USER_STR : \
        (CC_GKEYID_GET_LKEYID(a) == CC_LKEYID_LCE_H2D_KERN) ? \
            CC_LKEYID_LCE1_H2D_KERN_STR : \
        (CC_GKEYID_GET_LKEYID(a) == CC_LKEYID_LCE_D2H_KERN) ? \
            CC_LKEYID_LCE1_D2H_KERN_STR : \
        (CC_GKEYID_GET_LKEYID(a) == CC_LKEYID_LCE_H2D_P2P) ? \
            CC_LKEYID_LCE1_H2D_P2P_STR : \
        (CC_GKEYID_GET_LKEYID(a) == CC_LKEYID_LCE_D2H_P2P) ? \
            CC_LKEYID_LCE1_D2H_P2P_STR : NULL : \
    (CC_GKEYID_GET_KEYSPACE(a) == CC_KEYSPACE_LCE2) ? \
        (CC_GKEYID_GET_LKEYID(a) == CC_LKEYID_LCE_H2D_USER) ? \
            CC_LKEYID_LCE2_H2D_USER_STR : \
        (CC_GKEYID_GET_LKEYID(a) == CC_LKEYID_LCE_D2H_USER) ? \
            CC_LKEYID_LCE2_D2H_USER_STR : \
        (CC_GKEYID_GET_LKEYID(a) == CC_LKEYID_LCE_H2D_KERN) ? \
            CC_LKEYID_LCE2_H2D_KERN_STR : \
        (CC_GKEYID_GET_LKEYID(a) == CC_LKEYID_LCE_D2H_KERN) ? \
            CC_LKEYID_LCE2_D2H_KERN_STR : \
        (CC_GKEYID_GET_LKEYID(a) == CC_LKEYID_LCE_H2D_P2P) ? \
            CC_LKEYID_LCE2_H2D_P2P_STR : \
        (CC_GKEYID_GET_LKEYID(a) == CC_LKEYID_LCE_D2H_P2P) ? \
            CC_LKEYID_LCE2_D2H_P2P_STR : NULL : \
    (CC_GKEYID_GET_KEYSPACE(a) == CC_KEYSPACE_LCE3) ? \
        (CC_GKEYID_GET_LKEYID(a) == CC_LKEYID_LCE_H2D_USER) ? \
            CC_LKEYID_LCE3_H2D_USER_STR : \
        (CC_GKEYID_GET_LKEYID(a) == CC_LKEYID_LCE_D2H_USER) ? \
            CC_LKEYID_LCE3_D2H_USER_STR : \
        (CC_GKEYID_GET_LKEYID(a) == CC_LKEYID_LCE_H2D_KERN) ? \
            CC_LKEYID_LCE3_H2D_KERN_STR : \
        (CC_GKEYID_GET_LKEYID(a) == CC_LKEYID_LCE_D2H_KERN) ? \
            CC_LKEYID_LCE3_D2H_KERN_STR : \
        (CC_GKEYID_GET_LKEYID(a) == CC_LKEYID_LCE_H2D_P2P) ? \
            CC_LKEYID_LCE3_H2D_P2P_STR : \
        (CC_GKEYID_GET_LKEYID(a) == CC_LKEYID_LCE_D2H_P2P) ? \
            CC_LKEYID_LCE3_D2H_P2P_STR : NULL : \
    (CC_GKEYID_GET_KEYSPACE(a) == CC_KEYSPACE_LCE4) ? \
        (CC_GKEYID_GET_LKEYID(a) == CC_LKEYID_LCE_H2D_USER) ? \
            CC_LKEYID_LCE4_H2D_USER_STR : \
        (CC_GKEYID_GET_LKEYID(a) == CC_LKEYID_LCE_D2H_USER) ? \
            CC_LKEYID_LCE4_D2H_USER_STR : \
        (CC_GKEYID_GET_LKEYID(a) == CC_LKEYID_LCE_H2D_KERN) ? \
            CC_LKEYID_LCE4_H2D_KERN_STR : \
        (CC_GKEYID_GET_LKEYID(a) == CC_LKEYID_LCE_D2H_KERN) ? \
            CC_LKEYID_LCE4_D2H_KERN_STR : \
        (CC_GKEYID_GET_LKEYID(a) == CC_LKEYID_LCE_H2D_P2P) ? \
            CC_LKEYID_LCE4_H2D_P2P_STR : \
        (CC_GKEYID_GET_LKEYID(a) == CC_LKEYID_LCE_D2H_P2P) ? \
            CC_LKEYID_LCE4_D2H_P2P_STR : NULL : \
    (CC_GKEYID_GET_KEYSPACE(a) == CC_KEYSPACE_LCE5) ? \
        (CC_GKEYID_GET_LKEYID(a) == CC_LKEYID_LCE_H2D_USER) ? \
            CC_LKEYID_LCE5_H2D_USER_STR : \
        (CC_GKEYID_GET_LKEYID(a) == CC_LKEYID_LCE_D2H_USER) ? \
            CC_LKEYID_LCE5_D2H_USER_STR : \
        (CC_GKEYID_GET_LKEYID(a) == CC_LKEYID_LCE_H2D_KERN) ? \
            CC_LKEYID_LCE5_H2D_KERN_STR : \
        (CC_GKEYID_GET_LKEYID(a) == CC_LKEYID_LCE_D2H_KERN) ? \
            CC_LKEYID_LCE5_D2H_KERN_STR : \
        (CC_GKEYID_GET_LKEYID(a) == CC_LKEYID_LCE_H2D_P2P) ? \
            CC_LKEYID_LCE5_H2D_P2P_STR : \
        (CC_GKEYID_GET_LKEYID(a) == CC_LKEYID_LCE_D2H_P2P) ? \
            CC_LKEYID_LCE5_D2H_P2P_STR : NULL : \
    (CC_GKEYID_GET_KEYSPACE(a) == CC_KEYSPACE_LCE6) ? \
        (CC_GKEYID_GET_LKEYID(a) == CC_LKEYID_LCE_H2D_USER) ? \
            CC_LKEYID_LCE6_H2D_USER_STR : \
        (CC_GKEYID_GET_LKEYID(a) == CC_LKEYID_LCE_D2H_USER) ? \
            CC_LKEYID_LCE6_D2H_USER_STR : \
        (CC_GKEYID_GET_LKEYID(a) == CC_LKEYID_LCE_H2D_KERN) ? \
            CC_LKEYID_LCE6_H2D_KERN_STR : \
        (CC_GKEYID_GET_LKEYID(a) == CC_LKEYID_LCE_D2H_KERN) ? \
            CC_LKEYID_LCE6_D2H_KERN_STR : \
        (CC_GKEYID_GET_LKEYID(a) == CC_LKEYID_LCE_H2D_P2P) ? \
            CC_LKEYID_LCE6_H2D_P2P_STR : \
        (CC_GKEYID_GET_LKEYID(a) == CC_LKEYID_LCE_D2H_P2P) ? \
            CC_LKEYID_LCE6_D2H_P2P_STR : NULL : \
    (CC_GKEYID_GET_KEYSPACE(a) == CC_KEYSPACE_LCE7) ? \
        (CC_GKEYID_GET_LKEYID(a) == CC_LKEYID_LCE_H2D_USER) ? \
            CC_LKEYID_LCE7_H2D_USER_STR : \
        (CC_GKEYID_GET_LKEYID(a) == CC_LKEYID_LCE_D2H_USER) ? \
            CC_LKEYID_LCE7_D2H_USER_STR : \
        (CC_GKEYID_GET_LKEYID(a) == CC_LKEYID_LCE_H2D_KERN) ? \
            CC_LKEYID_LCE7_H2D_KERN_STR : \
        (CC_GKEYID_GET_LKEYID(a) == CC_LKEYID_LCE_D2H_KERN) ? \
            CC_LKEYID_LCE7_D2H_KERN_STR : \
        (CC_GKEYID_GET_LKEYID(a) == CC_LKEYID_LCE_H2D_P2P) ? \
            CC_LKEYID_LCE7_H2D_P2P_STR : \
        (CC_GKEYID_GET_LKEYID(a) == CC_LKEYID_LCE_D2H_P2P) ? \
            CC_LKEYID_LCE7_D2H_P2P_STR : NULL : NULL

#define CC_EXPORT_MASTER_KEY_SIZE_BYTES 32

#endif // CC_KEYSTORE_H
