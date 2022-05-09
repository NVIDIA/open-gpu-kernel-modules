/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#ifndef _SMBPBI_IMPL_H
#define _SMBPBI_IMPL_H
/*!
 * This header file stores implementation dependent parameters of the SMBPBI server.
 */

/*!
 * Maximum number of individual requests in a bundle
 */
#define NV_MSGBOX_PARAM_MAX_BUNDLE_SIZE 4

/*!
 * Maximum number of Result Disposition Rules
 */
#define NV_MSGBOX_PARAM_MAX_DISP_RULES  10

/*!
 * Maximum length of the Driver Event Message text string is 80, including
 * the terminating NUL character.
 */
#define NV_MSGBOX_MAX_DRIVER_EVENT_MSG_TXT_SIZE 80

#endif // _SMBPBI_IMPL_H
