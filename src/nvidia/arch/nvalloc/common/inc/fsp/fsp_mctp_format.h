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

#ifndef _FSP_MCTP_FORMAT_H_
#define _FSP_MCTP_FORMAT_H_

//
// NVDM (NVIDIA Data Model) overlayed on MCTP (Management Component Transport 
// Protocol) and sent over EMEM is the communication mechanism used between FSP 
// management partition and CPU-RM/other uprocs.
//


#define MCTP_HEADER_VERSION          3:0
#define MCTP_HEADER_RSVD             7:4

#define MCTP_HEADER_DEID            15:8
#define MCTP_HEADER_SEID            23:16

#define MCTP_HEADER_TAG             26:24
#define MCTP_HEADER_TO              27:27
#define MCTP_HEADER_SEQ             29:28
#define MCTP_HEADER_EOM             30:30
#define MCTP_HEADER_SOM             31:31

#define MCTP_MSG_HEADER_TYPE         6:0
#define MCTP_MSG_HEADER_IC           7:7

#define MCTP_MSG_HEADER_VENDOR_ID   23:8
#define MCTP_MSG_HEADER_NVDM_TYPE   31:24

#define MCTP_MSG_HEADER_TYPE_VENDOR_PCI 0x7e
#define MCTP_MSG_HEADER_VENDOR_ID_NV    0x10de

#endif // _FSP_MCTP_FORMAT_H_
