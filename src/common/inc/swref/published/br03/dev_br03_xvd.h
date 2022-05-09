/*
 * SPDX-FileCopyrightText: Copyright (c) 2022 NVIDIA CORPORATION & AFFILIATES
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

#ifndef DEV_BR03_XVD_H
#define DEV_BR03_XVD_H

#define NV_BR03_XVD_LINK_CTRLSTAT                         0x00000070 /* RWI4R */
#define NV_BR03_XVD_XP_0                                  0x00000B00 /* RW-4R */
#define NV_BR03_XVD_XP_0_UPDATE_FC_THRESHOLD                     9:1 /* RWIVF */
#define NV_BR03_XVD_XP_0_UPDATE_FC_THRESHOLD_INIT         0x00000000 /* RWI-V */
#define NV_BR03_XVD_XP_0_OPPORTUNISTIC_ACK                     29:29 /* RWIVF */
#define NV_BR03_XVD_XP_0_OPPORTUNISTIC_ACK_INIT           0x00000000 /* RWI-V */
#define NV_BR03_XVD_XP_0_OPPORTUNISTIC_UPDATE_FC               30:30 /* RWIVF */
#define NV_BR03_XVD_XP_0_OPPORTUNISTIC_UPDATE_FC_INIT     0x00000000 /* RWI-V */

#endif /* DEV_BR03_XVD_H */

