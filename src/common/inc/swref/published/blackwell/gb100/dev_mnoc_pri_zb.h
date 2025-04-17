/*
 * SPDX-FileCopyrightText: Copyright (c) 2024 NVIDIA CORPORATION & AFFILIATES
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

#ifndef __gb100_dev_mnoc_pri_zb_h__
#define __gb100_dev_mnoc_pri_zb_h__

#define NV_MNOC_ZB_PRI_MESSAGE_INFO_0_SENDMBOX(i)                             (0x00000104 + (i)*0xC)                        /* RW-4A */
#define NV_MNOC_ZB_PRI_MESSAGE_INFO_0_SENDMBOX__SIZE_1                        4              /*       */
#define NV_MNOC_ZB_PRI_MESSAGE_INFO_0_SENDMBOX__PRIV_LEVEL_MASK               NV_MNOC_ZB_PRI_SENDMBOX_PRIV_LEVEL_MASK          /*       */
#define NV_MNOC_ZB_PRI_MESSAGE_INFO_0_SENDMBOX_MESSAGE_SIZE                   19:0                                          /* R-IVF */
#define NV_MNOC_ZB_PRI_MESSAGE_INFO_0_SENDMBOX_MESSAGE_SIZE_INIT              0x00000000                                    /* R-I-V */
#define NV_MNOC_ZB_PRI_MESSAGE_INFO_0_SENDMBOX_MESSAGE_READY                  24:24                                         /* R-IVF */
#define NV_MNOC_ZB_PRI_MESSAGE_INFO_0_SENDMBOX_MESSAGE_READY_FALSE            0x00000000                                    /* R-I-V */
#define NV_MNOC_ZB_PRI_MESSAGE_INFO_0_SENDMBOX_MESSAGE_READY_TRUE             0x00000001                                    /* R---V */
#define NV_MNOC_ZB_PRI_MESSAGE_INFO_0_SENDMBOX_ERR                            25:25                                         /* R-IVF */
#define NV_MNOC_ZB_PRI_MESSAGE_INFO_0_SENDMBOX_ERR_FALSE                      0x00000000                                    /* R-I-V */
#define NV_MNOC_ZB_PRI_MESSAGE_INFO_0_SENDMBOX_ERR_TRUE                       0x00000001                                    /* R---V */
#define NV_MNOC_ZB_PRI_MESSAGE_INFO_0_SENDMBOX_INTR_ENABLE                    30:30                                         /* RWIVF */
#define NV_MNOC_ZB_PRI_MESSAGE_INFO_0_SENDMBOX_INTR_ENABLE_DISABLED           0x00000000                                    /* RWI-V */
#define NV_MNOC_ZB_PRI_MESSAGE_INFO_0_SENDMBOX_INTR_ENABLE_ENABLED            0x00000001                                    /* RW--V */
#define NV_MNOC_ZB_PRI_MESSAGE_INFO_0_SENDMBOX_INTR_STATUS                    31:31                                         /* RWIVF */
#define NV_MNOC_ZB_PRI_MESSAGE_INFO_0_SENDMBOX_INTR_STATUS__ONWRITE           "oneToClear"                                  /*       */
#define NV_MNOC_ZB_PRI_MESSAGE_INFO_0_SENDMBOX_INTR_STATUS_CLEARED            0x00000000                                    /* R-I-V */
#define NV_MNOC_ZB_PRI_MESSAGE_INFO_0_SENDMBOX_INTR_STATUS_SET                0x00000001                                    /* R---V */
#define NV_MNOC_ZB_PRI_MESSAGE_INFO_0_SENDMBOX_INTR_STATUS_W1CLR              0x00000001                                    /* -W--V */
#define NV_MNOC_ZB_PRI_RDATA_0_SENDMBOX(i)                                    (0x00000108 + (i)*0xC)                        /* R--4A */
#define NV_MNOC_ZB_PRI_RDATA_0_SENDMBOX__SIZE_1                               4              /*       */
#define NV_MNOC_ZB_PRI_RDATA_0_SENDMBOX__PRIV_LEVEL_MASK                      NV_MNOC_ZB_PRI_SENDMBOX_PRIV_LEVEL_MASK          /*       */
#define NV_MNOC_ZB_PRI_RDATA_0_SENDMBOX_RDATA                                 31:0                                          /* R-IVF */
#define NV_MNOC_ZB_PRI_RDATA_0_SENDMBOX_RDATA_INIT                            0x00000000                                    /* R-I-V */
#define NV_MNOC_ZB_PRI_MESSAGE_INFO_0_RECEIVEMBOX(i)                            (0x00000184 + (i)*0xC)                           /* RW-4A */
#define NV_MNOC_ZB_PRI_MESSAGE_INFO_0_RECEIVEMBOX__SIZE_1                       4              /*       */
#define NV_MNOC_ZB_PRI_MESSAGE_INFO_0_RECEIVEMBOX__PRIV_LEVEL_MASK              NV_MNOC_ZB_PRI_RECEIVEMBOX_PRIV_LEVEL_MASK          /*       */
#define NV_MNOC_ZB_PRI_MESSAGE_INFO_0_RECEIVEMBOX_MESSAGE_SIZE                  19:0                                             /* RWIVF */
#define NV_MNOC_ZB_PRI_MESSAGE_INFO_0_RECEIVEMBOX_MESSAGE_SIZE_INIT             0x00000000                                       /* RWI-V */
#define NV_MNOC_ZB_PRI_MESSAGE_INFO_0_RECEIVEMBOX_MESSAGE_TRIGGER               20:20                                            /* -WXVF */
#define NV_MNOC_ZB_PRI_MESSAGE_INFO_0_RECEIVEMBOX_MESSAGE_TRIGGER_SET           0x1                                              /* -W--V */
#define NV_MNOC_ZB_PRI_MESSAGE_INFO_0_RECEIVEMBOX_RECEIVE_READY                 24:24                                            /* R-IVF */
#define NV_MNOC_ZB_PRI_MESSAGE_INFO_0_RECEIVEMBOX_RECEIVE_READY_FALSE           0x00000000                                       /* R-I-V */
#define NV_MNOC_ZB_PRI_MESSAGE_INFO_0_RECEIVEMBOX_RECEIVE_READY_TRUE            0x00000001                                       /* R---V */
#define NV_MNOC_ZB_PRI_MESSAGE_INFO_0_RECEIVEMBOX_ERR                           25:25                                            /* R-IVF */
#define NV_MNOC_ZB_PRI_MESSAGE_INFO_0_RECEIVEMBOX_ERR_FALSE                     0x00000000                                       /* R-I-V */
#define NV_MNOC_ZB_PRI_MESSAGE_INFO_0_RECEIVEMBOX_ERR_TRUE                      0x00000001                                       /* R---V */
#define NV_MNOC_ZB_PRI_MESSAGE_INFO_0_RECEIVEMBOX_CREDIT_AVAILABLE              26:26                                            /* R-IVF */
#define NV_MNOC_ZB_PRI_MESSAGE_INFO_0_RECEIVEMBOX_CREDIT_AVAILABLE_FALSE        0x00000000                                       /* R-I-V */
#define NV_MNOC_ZB_PRI_MESSAGE_INFO_0_RECEIVEMBOX_CREDIT_AVAILABLE_TRUE         0x00000001                                       /* R---V */
#define NV_MNOC_ZB_PRI_MESSAGE_INFO_0_RECEIVEMBOX_INTR_ENABLE                   30:30                                            /* RWIVF */
#define NV_MNOC_ZB_PRI_MESSAGE_INFO_0_RECEIVEMBOX_INTR_ENABLE_DISABLED          0x00000000                                       /* RWI-V */
#define NV_MNOC_ZB_PRI_MESSAGE_INFO_0_RECEIVEMBOX_INTR_ENABLE_ENABLED           0x00000001                                       /* RW--V */
#define NV_MNOC_ZB_PRI_MESSAGE_INFO_0_RECEIVEMBOX_INTR_STATUS                   31:31                                            /* RWIVF */
#define NV_MNOC_ZB_PRI_MESSAGE_INFO_0_RECEIVEMBOX_INTR_STATUS__ONWRITE          "oneToClear"                                     /*       */
#define NV_MNOC_ZB_PRI_MESSAGE_INFO_0_RECEIVEMBOX_INTR_STATUS_CLEARED           0x00000000                                       /* R-I-V */
#define NV_MNOC_ZB_PRI_MESSAGE_INFO_0_RECEIVEMBOX_INTR_STATUS_SET               0x00000001                                       /* R---V */
#define NV_MNOC_ZB_PRI_MESSAGE_INFO_0_RECEIVEMBOX_INTR_STATUS_W1CLR             0x00000001                                       /* -W--V */
#define NV_MNOC_ZB_PRI_WDATA_0_RECEIVEMBOX(i)                                   (0x00000188 + (i)*0xC)                           /* -W-4A */
#define NV_MNOC_ZB_PRI_WDATA_0_RECEIVEMBOX__SIZE_1                              4              /*       */
#define NV_MNOC_ZB_PRI_WDATA_0_RECEIVEMBOX__PRIV_LEVEL_MASK                     NV_MNOC_ZB_PRI_RECEIVEMBOX_PRIV_LEVEL_MASK          /*       */
#define NV_MNOC_ZB_PRI_WDATA_0_RECEIVEMBOX_WDATA                                31:0                                             /* -WXVF */
#endif // __gb100_dev_mnoc_pri_zb_h__
