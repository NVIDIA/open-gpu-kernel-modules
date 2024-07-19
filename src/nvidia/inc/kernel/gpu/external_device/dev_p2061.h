/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef DEV_P2061_H
#define DEV_P2061_H

#define NV_P2061_CONTROL4                           0x06 /* RW-1R */
#define NV_P2061_CONTROL4_HOUSE_SYNC_MODE           6:6  /* RWXVF */
#define NV_P2061_CONTROL4_HOUSE_SYNC_MODE_INPUT     0x0  /* RW--V */
#define NV_P2061_CONTROL4_HOUSE_SYNC_MODE_OUTPUT    0x1  /* RW--V */

#define NV_P2061_STATUS6                            0x2E /* RW-1R */
#define NV_P2061_STATUS6_INT_PORT_DIRECTION         7:7  /* RWXVF */
#define NV_P2061_STATUS6_INT_PORT_DIRECTION_INPUT   0    /* RWXVF */
#define NV_P2061_STATUS6_INT_PORT_DIRECTION_OUTPUT  1    /* RWXVF */

#endif //DEV_P2061_H
