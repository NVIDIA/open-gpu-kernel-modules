/*
 * SPDX-FileCopyrightText: Copyright (c) 2003-2023 NVIDIA CORPORATION & AFFILIATES
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

#ifndef __ga100_dev_nv_xve_addendum_h__
#define __ga100_dev_nv_xve_addendum_h__

//
// Extra config bits that can be emulated by the hypervisor for passthrough.
// This offset is unused in HW and HW returns 0x0 on read.
//
#define NV_XVE_PASSTHROUGH_EMULATED_CONFIG                              0xE8

//
// On GA100, we need to be able to detect the case where the GPU is running at
// gen4, but the root port is at gen3. On baremetal, we just check the root
// port directly, but for passthrough root port is commonly completely hidden
// or fake. To handle this case we support the hypervisor explicitly
// communicating the speed to us through emulated config space. The
// ROOT_PORT_SPEED field follows the usual link speed encoding with the
// numerical value matching the gen speed, i.e. gen3 is 0x3.
// See bug 2927491 for more details.
//
#define NV_XVE_PASSTHROUGH_EMULATED_CONFIG_ROOT_PORT_SPEED               3:0

//
// On some platforms it's beneficial to enable relaxed ordering after vetting
// it's safe to do so. To automate this process on virtualized platforms, allow
// RO to be requested through this emulated config space bit.
//
#define NV_XVE_PASSTHROUGH_EMULATED_CONFIG_RELAXED_ORDERING_ENABLE       4:4

//
// While the following defines are not register addresses/bit fields, the BAR
// indices(currently arranged in serial fashion) could be changed for future
// chips and therefore keeping it chip specific
//
#define NV_XVE_BAR0_LO_INDEX 0x0
#define NV_XVE_BAR0_HI_INDEX 0x1
#define NV_XVE_BAR1_LO_INDEX 0x2
#define NV_XVE_BAR1_HI_INDEX 0x3
#define NV_XVE_BAR2_LO_INDEX 0x4
#define NV_XVE_BAR2_HI_INDEX 0x5
#define NV_XVE_BAR3_INDEX    0x6


#define NV_PCFG                             0x00088FFF:0x00088000 /* RW--D */
#define NV_XVE_DEV_CTRL                     0x00000004 /* RW-4R */

#endif
