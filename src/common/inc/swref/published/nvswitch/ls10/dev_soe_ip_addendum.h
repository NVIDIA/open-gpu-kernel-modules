/*
 * SPDX-FileCopyrightText: Copyright (c) 2003-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the Software),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED AS IS, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef __ls10_dev_soe_ip_addendum_h__
#define __ls10_dev_soe_ip_addendum_h__

#define NV_SOE_MUTEX_DEFINES                              \
    NV_MUTEX_ID_SOE_EMEM_ACCESS,                          \

#define NV_SOE_EMEM_ACCESS_PORT_NVSWITCH                           (0)
#define NV_SOE_EMEM_ACCESS_PORT_NVWATCH                            (1)
#define UNUSED_EMEM_ACCESS_PORT_2                                  (2)
#define UNUSED_EMEM_ACCESS_PORT_3                                  (3)

#define NUM_SAW_ENGINE                                              1
#define NUM_NVLINK_ENGINE                                           9

#define NUM_TLC_ENGINE                                              64
#define NUM_NVLIPT_LNK_ENGINE                                       64

#define NUM_NPG_ENGINE                                               16
#define NUM_NPG_BCAST_ENGINE                                         1
#define NUM_NPORT_ENGINE                                             64
#define NUM_NPORT_MULTICAST_BCAST_ENGINE                             1
#define NUM_NXBAR_ENGINE                                             3
#define NUM_NXBAR_BCAST_ENGINE                                       1
#define NUM_TILE_ENGINE                                              36
#define NUM_TILE_MULTICAST_BCAST_ENGINE                              1
#define NUM_NVLW_ENGINE                                              16
#define NUM_BUS_ENGINE                                               1
#define NUM_GIN_ENGINE                                               1
#define NUM_MINION_ENGINE                                            NUM_NVLW_ENGINE
#define NUM_MINION_BCAST_ENGINE                                      1
#define NUM_NVLIPT_ENGINE                                            NUM_NVLW_ENGINE
#define NUM_NVLIPT_BCAST_ENGINE                                      1
#define NUM_SYS_PRI_HUB                                              1
#define NUM_PRI_MASTER_RS                                            1
#define NUM_LINKS_PER_MINION                                         (NUM_NPORT_ENGINE / NUM_MINION_ENGINE)

#endif // __ls10_dev_soe_ip_addendum_h__
