/*
 * SPDX-FileCopyrightText: Copyright (c) 2000-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * @file
 * @brief  includes defines used by cpu.c to get cpu information
 */

#ifndef CPUID_ARM_DEF_H_
#define CPUID_ARM_DEF_H_

#if defined(__GNUC__)

#define CP_READ_CCSIDR_REGISTER() CP_READ_REGISTER(CP_CCSIDR)
#define CP_READ_MIDR_REGISTER() CP_READ_REGISTER(CP_MIDR)
#define CP_WRITE_CSSELR_REGISTER(val) CP_WRITE_REGISTER(CP_CSSELR, val)

#else

extern NvU32 CP_READ_CCSIDR_REGISTER();
extern NvU32 CP_READ_MIDR_REGISTER();
extern void CP_WRITE_CSSELR_REGISTER(NvU32 val);

#endif // defined(__GNUC__)

#define GET_BITMASK(f, v) ((v) >> DRF_SHIFT(CP_ ## f) & DRF_MASK(CP_ ## f))

#define CP_MIDR_IMPLEMENTER           31:24
#define CP_MIDR_PRIMARY_PART_NUM      15:4

#define CP_MIDR_IMPLEMENTER_BROADCOM  0x42
#define CP_MIDR_IMPLEMENTER_NVIDIA    0x4e
#define CP_MIDR_IMPLEMENTER_AMCC      0x50
#define CP_MIDR_IMPLEMENTER_ARM       0x41
#define CP_MIDR_IMPLEMENTER_MARVELL   0x43
#define CP_MIDR_IMPLEMENTER_HUAWEI    0x48
#define CP_MIDR_IMPLEMENTER_FUJITSU   0x46
#define CP_MIDR_IMPLEMENTER_PHYTIUM   0x70
#define CP_MIDR_IMPLEMENTER_AMPERE    0x81
#define CP_MIDR_IMPLEMENTER_AMPERE_2  0XC0

#define CP_CSSELR_DATA_CACHE          0
#define CP_CSSELR_INSTRUCTION_CACHE   1
#define CP_CSSELR_DATA_CACHE_LEVEL2   2

#define CP_CCSIDR_CACHE_LINE_SIZE     2:0
#define CP_CCSIDR_CACHE_ASSOCIATIVITY 12:3
#define CP_CCSIDR_CACHE_NUM_SETS      27:13

#if defined(NVCPU_AARCH64)

// Main ID Register
#define CP_MIDR                       "midr_el1"

#define CP_MIDR_PRIMARY_PART_NUM_DENVER_1   0x0
#define CP_MIDR_PRIMARY_PART_NUM_DENVER_2   0x3
#define CP_MIDR_PRIMARY_PART_NUM_CARMEL     0x4

#define CP_MIDR_PRIMARY_PART_NUM_XGENE         0x0
#define CP_MIDR_PRIMARY_PART_NUM_CORTEX_A57    0xd07
#define CP_MIDR_PRIMARY_PART_NUM_CORTEX_A76    0xd0b
#define CP_MIDR_PRIMARY_PART_NUM_NEOVERSE_V2   0xd4f
#define CP_MIDR_PRIMARY_PART_NUM_THUNDER_X2    0x0af
#define CP_MIDR_PRIMARY_PART_NUM_KUNPENG_920   0xd01
#define CP_MIDR_PRIMARY_PART_NUM_BLUEFIELD     0xd08
#define CP_MIDR_PRIMARY_PART_NUM_BLUEFIELD3    0xd42
#define CP_MIDR_PRIMARY_PART_NUM_GRAVITRON2    0xd0c
#define CP_MIDR_PRIMARY_PART_NUM_A64FX         0x001
#define CP_MIDR_PRIMARY_PART_NUM_FT2000        0x662
#define CP_MIDR_PRIMARY_PART_NUM_S2500         0x663
#define CP_MIDR_PRIMARY_PART_NUM_ALTRA         0x000
#define CP_MIDR_PRIMARY_PART_NUM_OCTEON_CN96XX 0x0b2
#define CP_MIDR_PRIMARY_PART_NUM_OCTEON_CN98XX 0x0b1
#define CP_MIDR_PRIMARY_PART_NUM_NEOVERSE_N2   0xd49
#define CP_MIDR_PRIMARY_PART_NUM_AMPEREONE     0xac3

// Cache Size Identification Register
#define CP_CCSIDR                     "ccsidr_el1"

// Cache Size Selection Register
#define CP_CSSELR                     "csselr_el1"

#else // NVCPU_AARCH64

// Main ID Register
#define CP_MIDR                       "0"

#define CP_MIDR_PRIMARY_PART_NUM_A9   0xC09
#define CP_MIDR_PRIMARY_PART_NUM_A15  0xC0F

// Cache Size Identification Register
#define CP_CCSIDR                     "1"

#define CP_CCSIDR_CACHE_LINE_SIZE_8W  1

#define CP_CCSIDR_CACHE_NUM_SETS_16KB 0x7F
#define CP_CCSIDR_CACHE_NUM_SETS_32KB 0xFF
#define CP_CCSIDR_CACHE_NUM_SETS_64KB 0x1FF

#define CP_CCSIDR_CACHE_NUM_SETS_A15_32KB   0xFF
#define CP_CCSIDR_CACHE_NUM_SETS_A15_512KB  0x1FF
#define CP_CCSIDR_CACHE_NUM_SETS_A15_1024KB 0x3FF
#define CP_CCSIDR_CACHE_NUM_SETS_A15_2048KB 0x7FF
#define CP_CCSIDR_CACHE_NUM_SETS_A15_4096KB 0xFFF

// Cache Size Selection Register
#define CP_CSSELR                     "2"

#endif // NVCPU_AARCH64

#endif /* CPUID_ARM_DEF_H_ */
