/*
 * SPDX-FileCopyrightText: Copyright (c) 1993 - 2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef NBCI_H_
#define NBCI_H_

/******************************************************************************
 *
 *         Provides the NBCI (NoteBook Common Interface) spec defines for
 *         use by multiple clients.
 *
******************************************************************************/

#define NBCI_REVISION_ID        0x00000102

// NBCI _DSM function codes
#define NV_NBCI_FUNC_SUPPORT                                         0x00000000
#define NV_NBCI_FUNC_PLATCAPS                                        0x00000001
#define NV_NBCI_FUNC_PLATPOLICY                                      0x00000004
#define NV_NBCI_FUNC_DISPLAYSTATUS                                   0x00000005
#define NV_NBCI_FUNC_MDTL                                            0x00000006
#define NV_NBCI_FUNC_GETOBJBYTYPE                                    0x00000010
#define NV_NBCI_FUNC_GETALLOBJS                                      0x00000011
#define NV_NBCI_FUNC_GETEVENTLIST                                    0x00000012
#define NV_NBCI_FUNC_CALLBACKS                                       0x00000013
#define NV_NBCI_FUNC_GETBACKLIGHT                                    0x00000014
#define NV_NBCI_FUNC_MSTL                                            0x00000015
#endif // NBCI_H_
