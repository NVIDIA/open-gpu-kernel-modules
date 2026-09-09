/*
 * SPDX-FileCopyrightText: Copyright (c) 2026 NVIDIA CORPORATION & AFFILIATES
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

#ifndef __gr100_dev_nv_pcie_reset_addendum__
#define __gr100_dev_nv_pcie_reset_addendum__

// Do Not Modify this without consulting PCIe HW and SW as this reflects Bug 5197385
#define NV_PCIE_RESET_STATUS_PASS                            0b00110
#define NV_PCIE_RESET_STATUS_FLR_ASSERTION_TIMEOUT           0b10110
#define NV_PCIE_RESET_STATUS_FLR_DEASSERTION_TIMEOUT         0b10010
#define NV_PCIE_RESET_STATUS_CONV_RESET_ASSERTION_TIMEOUT    0b11110
#define NV_PCIE_RESET_STATUS_CONV_RESET_DEASSERTION_TIMEOUT  0b11000
#define NV_PCIE_RESET_STATUS_ILLEGAL_ACK                     0b00111
#define NV_PCIE_RESET_STATUS_UNCLASSIFIED                    0b00000

#define NV_PCIE_RESET_INFO_ILLEGAL_UPHY_ACK                      0:0
#define NV_PCIE_RESET_INFO_CONV_RESET_ALIVE                      1:1
#define NV_PCIE_RESET_INFO_FLR_ALIVE                             2:2
#define NV_PCIE_RESET_INFO_CONV_RESET_PENDING                    3:3
#define NV_PCIE_RESET_INFO_FLR_PENDING                           4:4

#endif // __gr100_dev_nv_pcie_reset_addendum__
