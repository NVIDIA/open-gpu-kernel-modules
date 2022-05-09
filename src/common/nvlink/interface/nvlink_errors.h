/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2016 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _NVLINK_ERRORS_H_
#define _NVLINK_ERRORS_H_

typedef int NvlStatus;

#define NVL_SUCCESS                             (NvlStatus) 0
#define NVL_BAD_ARGS                            (NvlStatus) 1
#define NVL_NO_MEM                              (NvlStatus) 2
#define NVL_NOT_FOUND                           (NvlStatus) 3
#define NVL_INITIALIZATION_PARTIAL_FAILURE      (NvlStatus) 4
#define NVL_INITIALIZATION_TOTAL_FAILURE        (NvlStatus) 5
#define NVL_PCI_ERROR                           (NvlStatus) 6
#define NVL_ERR_GENERIC                         (NvlStatus) 7
#define NVL_ERR_INVALID_STATE                   (NvlStatus) 8
#define NVL_UNBOUND_DEVICE                      (NvlStatus) 9
#define NVL_MORE_PROCESSING_REQUIRED            (NvlStatus)10
#define NVL_IO_ERROR                            (NvlStatus)11
#define NVL_ERR_STATE_IN_USE                    (NvlStatus)12
#define NVL_ERR_NOT_SUPPORTED                   (NvlStatus)13
#define NVL_ERR_NOT_IMPLEMENTED                 (NvlStatus)14
#define NVL_ERR_INSUFFICIENT_PERMISSIONS        (NvlStatus)15
#define NVL_ERR_OPERATING_SYSTEM                (NvlStatus)16

#endif // _NVLINK_ERRORS_H_
