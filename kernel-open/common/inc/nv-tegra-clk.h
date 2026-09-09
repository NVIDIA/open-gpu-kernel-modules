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
 
 #ifndef _NV_TEGRA_CLK_H_
 #define _NV_TEGRA_CLK_H_

 #include "nv.h"

 struct nv_pci_tegra_devfreq_data {
     const char *clk_name;
     const char *icc_name;
     const unsigned int gpc_fuse_field;
     const TEGRASOC_DEVFREQ_CLK devfreq_clk;
 };

static const struct nv_pci_tegra_devfreq_data tegra_devfreq_tables[][6] = {
    {
        {
            .clk_name = "gpc0clk",
            .icc_name = "gpu-write",
            .gpc_fuse_field = BIT(0),
            .devfreq_clk = TEGRASOC_DEVFREQ_CLK_GPC,
        },
        {
            .clk_name = "gpc1clk",
            .icc_name = "gpu-write",
            .gpc_fuse_field = BIT(1),
            .devfreq_clk = TEGRASOC_DEVFREQ_CLK_GPC,
        },
        {
            .clk_name = "gpc2clk",
            .icc_name = "gpu-write",
            .gpc_fuse_field = BIT(2),
            .devfreq_clk = TEGRASOC_DEVFREQ_CLK_GPC,
        },
        {
            .clk_name = "nvdclk",
            .icc_name = "video-write",
            .devfreq_clk = TEGRASOC_DEVFREQ_CLK_NVD,
        },
        {
            .clk_name = "sysclk"
        },
        {
            .clk_name = "uprocclk"
        },
    },
    {
        {
            .clk_name = "gpc0clk",
            .icc_name = "gpu-write",
            .gpc_fuse_field = BIT(0),
            .devfreq_clk = TEGRASOC_DEVFREQ_CLK_GPC,
        },
        {
            .clk_name = "gpc1clk",
            .icc_name = "gpu-write",
            .gpc_fuse_field = BIT(1),
            .devfreq_clk = TEGRASOC_DEVFREQ_CLK_GPC,
        },
        {
            .clk_name = "nvdclk",
            .icc_name = "video-write",
            .devfreq_clk = TEGRASOC_DEVFREQ_CLK_NVD,
        },
        {
            .clk_name = "sysclk"
        },
        {
            .clk_name = "uprocclk"
        },
        {
            .clk_name = NULL
        },
    },
 };

 #endif /* _NV_TEGRA_CLK_H_ */
