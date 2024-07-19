/*
 * SPDX-FileCopyrightText: Copyright (c) 2023-2024 NVIDIA CORPORATION & AFFILIATES
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

#ifndef __gb100_dev_hshub_base_h__
#define __gb100_dev_hshub_base_h__
#define NV_PFB_HSHUB                                   0x00000FFF:0x00000000 /* RW--D */
#define NV_PFB_HSHUB_PCIE_FLUSH_SYSMEM_ADDR_LO              0x00000E50 /* RW-4R */
#define NV_PFB_HSHUB_PCIE_FLUSH_SYSMEM_ADDR_LO_ADR                31:0 /* RWIVF */
#define NV_PFB_HSHUB_PCIE_FLUSH_SYSMEM_ADDR_LO_ADR_INIT     0x00000000 /* RWI-V */
#define NV_PFB_HSHUB_PCIE_FLUSH_SYSMEM_ADDR_LO_ADR_MASK     0xFFFFFF00 /* ----V */
#define NV_PFB_HSHUB_PCIE_FLUSH_SYSMEM_ADDR_HI              0x00000E54 /* RW-4R */
#define NV_PFB_HSHUB_PCIE_FLUSH_SYSMEM_ADDR_HI_ADR                31:0 /* RWIVF */
#define NV_PFB_HSHUB_PCIE_FLUSH_SYSMEM_ADDR_HI_ADR_INIT     0x00000000 /* RWI-V */
#define NV_PFB_HSHUB_PCIE_FLUSH_SYSMEM_ADDR_HI_ADR_MASK     0x000FFFFF /* ----V */
#define NV_PFB_HSHUB_EG_PCIE_FLUSH_SYSMEM_ADDR_LO           0x000006C0 /* RW-4R */
#define NV_PFB_HSHUB_EG_PCIE_FLUSH_SYSMEM_ADDR_LO_ADR             31:0 /* RWIVF */
#define NV_PFB_HSHUB_EG_PCIE_FLUSH_SYSMEM_ADDR_LO_ADR_INIT  0x00000000 /* RWI-V */
#define NV_PFB_HSHUB_EG_PCIE_FLUSH_SYSMEM_ADDR_LO_ADR_MASK  0xFFFFFF00 /* ----V */
#define NV_PFB_HSHUB_EG_PCIE_FLUSH_SYSMEM_ADDR_HI           0x000006C4 /* RW-4R */
#define NV_PFB_HSHUB_EG_PCIE_FLUSH_SYSMEM_ADDR_HI_ADR             31:0 /* RWIVF */
#define NV_PFB_HSHUB_EG_PCIE_FLUSH_SYSMEM_ADDR_HI_ADR_INIT  0x00000000 /* RWI-V */
#define NV_PFB_HSHUB_EG_PCIE_FLUSH_SYSMEM_ADDR_HI_ADR_MASK  0x000FFFFF /* ----V */
#endif // __gb100_dev_hshub_base_h__
