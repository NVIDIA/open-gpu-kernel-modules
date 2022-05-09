/*
 * SPDX-FileCopyrightText: Copyright (c) 2003-2022 NVIDIA CORPORATION & AFFILIATES
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

#ifndef __tu102_dev_nv_xve_h__
#define __tu102_dev_nv_xve_h__
#define NV_PCFG                             0x00088FFF:0x00088000 /* RW--D */
#define NV_XVE_MSIX_CAP_HDR                                    0x000000C8 /* RW-4R */
#define NV_XVE_MSIX_CAP_HDR_ENABLE                                  31:31 /* RWIVF */
#define NV_XVE_MSIX_CAP_HDR_ENABLE_ENABLED                     0x00000001 /* RW--V */
#define NV_XVE_MSIX_CAP_HDR_ENABLE_DISABLED                    0x00000000 /* RWI-V */
#endif // __tu102_dev_nv_xve_h__
