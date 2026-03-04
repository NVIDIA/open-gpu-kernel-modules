/*
 * SPDX-FileCopyrightText: Copyright (c) 2023 NVIDIA CORPORATION & AFFILIATES
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

#ifndef __tu102_dev_ltc_h_
#define __tu102_dev_ltc_h_

#define NV_PLTCG_LTC0_LTS0_L2_CACHE_ECC_UNCORRECTED_ERR_COUNT                  0x001404f8 /* RW-4R */
#define NV_PLTCG_LTC0_LTS0_L2_CACHE_ECC_UNCORRECTED_ERR_COUNT_TOTAL                  15:0 /* RWIVF */
#define NV_PLTCG_LTC0_LTS0_L2_CACHE_ECC_UNCORRECTED_ERR_COUNT_TOTAL_INIT           0x0000 /* RWI-V */
#define NV_PLTCG_LTC0_LTS0_L2_CACHE_ECC_UNCORRECTED_ERR_COUNT_UNIQUE                31:16 /* RWIVF */
#define NV_PLTCG_LTC0_LTS0_L2_CACHE_ECC_UNCORRECTED_ERR_COUNT_UNIQUE_INIT          0x0000 /* RWI-V */

#endif // __tu102_dev_ltc_h_
