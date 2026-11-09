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

#ifndef __gh100_dev_xtl_ep_pcfg_vf_h__
#define __gh100_dev_xtl_ep_pcfg_vf_h__

/* Published subset of GH100 VF PCIe configuration-space definitions. */
#define NV_EP_PCFG_VF_PCI_EXPRESS_CAPABILITY_HEADER                 0x00000040 /* R--4R */
#define NV_EP_PCFG_VF_PCI_EXPRESS_CAPABILITY_HEADER_CAP_ID                 7:0 /* R-IVF */
#define NV_EP_PCFG_VF_PCI_EXPRESS_CAPABILITY_HEADER_CAP_ID_DEFAULT  0x00000010 /* R-I-V */
#define NV_EP_PCFG_VF_PCI_EXPRESS_CAPABILITY_HEADER_NEXT_POINTER          15:8 /* R-IVF */
#define NV_EP_PCFG_VF_LINK_CAPABILITIES                             0x0000004C /* R--4R */
#define NV_EP_PCFG_VF_LINK_CAPABILITIES_MAX_LINK_SPEED                     3:0 /* R-IVF */
#define NV_EP_PCFG_VF_LINK_CAPABILITIES_MAX_LINK_WIDTH                     9:4 /* R-IVF */

#endif // __gh100_dev_xtl_ep_pcfg_vf_h__
