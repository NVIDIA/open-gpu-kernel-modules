/*
 * SPDX-FileCopyrightText: Copyright (c) 2003-2026 NVIDIA CORPORATION & AFFILIATES
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

#ifndef ___dev_ppc_ref_h__
#define ___dev_ppc_ref_h__

#define NV_PPPC_PRI_CBM_BETA_CB_SIZE                                              0x000000C0 /*       */
#define NV_PPPC_PRI_CBM_BETA_CB_SIZE_V                                                  21:0 /*       */
#define NV_PPPC_PRI_CBM_BETA_CB_SIZE_V_GRANULARITY                                0x00000020 /*       */
#define NV_PPPC_PRI_CBM_ALPHA_CB_SIZE                                             0x000000E4 /*       */
#define NV_PPPC_PRI_CBM_ALPHA_CB_SIZE_V                                                 15:0 /*       */
#define NV_PPPC_PRI_CBM_BETA_STEADY_STATE_CB_SIZE                                 0x000000F0 /*       */
#define NV_PPPC_PRI_CBM_BETA_STEADY_STATE_CB_SIZE_V                                     21:0 /*       */
#define NV_PPPC_PRI_CBM_BETA_CB_OFFSET                                            0x000000F4 /*       */
#define NV_PPPC_PRI_CBM_BETA_CB_OFFSET_V                                                21:0 /*       */
#define NV_PPPC_PRI_CBM_ALPHA_CB_OFFSET                                           0x000000F8 /*       */
#define NV_PPPC_PRI_CBM_ALPHA_CB_OFFSET_V                                               21:0 /*       */
#define NV_PPPC_PRI_CBM_BETA_CB_CTRL                                              0x000000FC /*       */
#define NV_PPPC_PRI_CBM_BETA_CB_CTRL_CBES_RESERVE                                       11:0 /*       */
#define NV_PPPC_PRI_CBM_BETA_CB_CTRL_CBES_RESERVE_NON_GFXP                                 0 /*       */

#endif // ___dev_ppc_ref_h__
