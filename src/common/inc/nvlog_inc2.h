/*
 * SPDX-FileCopyrightText: Copyright (c) 2013,2016-2017,2020-2020 NVIDIA CORPORATION & AFFILIATES
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
#ifndef _NVLOG_INC2_H_
#define _NVLOG_INC2_H_
//
// Include the auto-generated g_$(filename)-nvlog.h header. The file contains
// information about the trace statements that was pulled out by the NvLog preprocessor.
// NVLOG_INCLUDE is defined by make at compile time, for every source file.
//
// The four lines of macros is some trickiness needed to make it work.
//
#if (defined(NVLOG_ENABLED) || defined(NV_MODS)) && defined(NVLOG_INCLUDE) && !defined(NVLOG_PARSING)
#if NVLOG_ENABLED || defined(NV_MODS)

#ifndef   NVLOG_FILEID      // Acts as an include guard
#define   NVLOG_INCLUDE3(a) #a
#define   NVLOG_INCLUDE2(a) NVLOG_INCLUDE3 a
#define   NVLOG_INCLUDE1    NVLOG_INCLUDE2((NVLOG_INCLUDE))
#include  NVLOG_INCLUDE1
#endif // NVLOG_FILEID

#endif // NVLOG_ENABLED
#endif // defined(NVLOG_ENABLED) && defined(NVLOG_INCLUDE)


#endif // _NVLOG_INC2_H_
