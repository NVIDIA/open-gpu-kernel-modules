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

#ifndef __gm107_dev_pri_ringstation_sys_h__
#define __gm107_dev_pri_ringstation_sys_h__

#define NV_PPRIV_SYS_PRI_ERROR_CODE                                                     31:8  /* --XVF */
#define NV_PPRIV_SYS_PRI_ERROR_CODE_HOST_FECS_ERR                                   0xBAD00F  /* ----V */
#define NV_PPRIV_SYS_PRI_ERROR_CODE_HOST_PRI_TIMEOUT                                0xBAD001  /* ----V */
#define NV_PPRIV_SYS_PRI_ERROR_CODE_HOST_FB_ACK_TIMEOUT                             0xBAD0B0  /* ----V */
#define NV_PPRIV_SYS_PRI_ERROR_CODE_FECS_PRI_TIMEOUT                                0xBADF10  /* ----V */
#define NV_PPRIV_SYS_PRI_ERROR_CODE_FECS_PRI_DECODE                                 0xBADF11  /* ----V */
#define NV_PPRIV_SYS_PRI_ERROR_CODE_FECS_PRI_RESET                                  0xBADF12  /* ----V */
#define NV_PPRIV_SYS_PRI_ERROR_CODE_FECS_PRI_FLOORSWEEP                             0xBADF13  /* ----V */
#define NV_PPRIV_SYS_PRI_ERROR_CODE_FECS_PRI_STUCK_ACK                              0xBADF14  /* ----V */
#define NV_PPRIV_SYS_PRI_ERROR_CODE_FECS_PRI_0_EXPECTED_ACK                         0xBADF15  /* ----V */
#define NV_PPRIV_SYS_PRI_ERROR_CODE_FECS_PRI_FENCE_ERROR                            0xBADF16  /* ----V */
#define NV_PPRIV_SYS_PRI_ERROR_CODE_FECS_PRI_SUBID_ERROR                            0xBADF17  /* ----V */
#define NV_PPRIV_SYS_PRI_ERROR_CODE_FECS_PRI_ORPHAN                                 0xBADF20  /* ----V */
#define NV_PPRIV_SYS_PRI_ERROR_CODE_FECS_DEAD_RING                                  0xBADF30  /* ----V */
#define NV_PPRIV_SYS_PRI_ERROR_CODE_FECS_TRAP                                       0xBADF40  /* ----V */
#define NV_PPRIV_SYS_PRI_ERROR_CODE_FECS_PRI_CLIENT_ERR                             0xBADF50  /* ----V */

#endif // __gm107_dev_pri_ringstation_sys_h__
