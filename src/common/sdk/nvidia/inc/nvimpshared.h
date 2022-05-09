/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/******************************************************************************\
*                                                                              *
*  Description:                                                                *
*    Accommodates sharing of IMP-related structures between kernel interface   *
*    files and core RM.                                                        *
*                                                                              *
\******************************************************************************/

#pragma once

#include <nvtypes.h>

//
// This file was generated with FINN, an NVIDIA coding tool.
// Source file: nvimpshared.finn
//




//
// There are only a small number of discrete dramclk frequencies available on
// the system.  This structure contains IMP-relevant information associated
// with a specific dramclk frequency.
//
typedef struct DRAM_CLK_INSTANCE {
    NvU32 dram_clk_freq_khz;

    NvU32 mchub_clk_khz;

    NvU32 mc_clk_khz;

    NvU32 max_iso_bw_kbps;

    //
    // switch_latency_ns is the maximum time required to switch the dramclk
    // frequency to the frequency specified in dram_clk_freq_khz.
    //
    NvU32 switch_latency_ns;
} DRAM_CLK_INSTANCE;

//
// This table is used to collect information from other modules that is needed
// for RM IMP calculations.  (Used on Tegra only.)
//
#define TEGRA_IMP_IMPORT_DATA_DRAM_TYPE_UNKNOWN 0U
#define TEGRA_IMP_IMPORT_DATA_DRAM_TYPE_LPDDR4  1U
#define TEGRA_IMP_IMPORT_DATA_DRAM_TYPE_LPDDR5  2U

typedef struct TEGRA_IMP_IMPORT_DATA {
    //
    // max_iso_bw_kbps stores the maximum possible ISO bandwidth available to
    // display, assuming display is the only active ISO client.  (Note that ISO
    // bandwidth will typically be allocated to multiple clients, so display
    // will generally not have access to the maximum possible bandwidth.)
    //
    NvU32             max_iso_bw_kbps;

    NvU32             dram_type;
    // On Orin, each dram channel is 16 bits wide.
    NvU32             num_dram_channels;

    //
    // dram_clk_instance stores entries for all possible dramclk frequencies,
    // sorted by dramclk frequency in increasing order.
    //
    // "24" is expected to be larger than the actual number of required entries
    // (which is provided by a BPMP API), but it can be increased if necessary.
    //
    // num_dram_clk_entries is filled in with the actual number of distinct
    // dramclk entries.
    //
    NvU32             num_dram_clk_entries;
    DRAM_CLK_INSTANCE dram_clk_instance[24];
} TEGRA_IMP_IMPORT_DATA;
