/*
 * SPDX-FileCopyrightText: Copyright (c) 2017-2017 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _NV_SECURITYINFO_H_
#define _NV_SECURITYINFO_H_

/// opaque token used for the security model on all OS's (UID/PID for Linux, SID Windows, etc)
typedef void *PSECURITY_TOKEN;

/**
 * Specifies the privilege level of the caller
 * Taken from resserv.h
 */
typedef enum 
{
    RS_PRIV_LEVEL_USER,
    RS_PRIV_LEVEL_USER_ROOT,
    RS_PRIV_LEVEL_KERNEL,
    RS_PRIV_LEVEL_HOST,
    RS_PRIV_LEVEL_CPU_MAX=RS_PRIV_LEVEL_HOST,
    RS_PRIV_LEVEL_GPU1,
    RS_PRIV_LEVEL_GPU2,
    RS_PRIV_LEVEL_GPU3,
} RS_PRIV_LEVEL;

/**
 * Specifies where the parameters are, user or kernel space (decide whether copy_in/out)
 *
 * PARAM_LOCATION_USER   - Parameters in user space
 * PARAM_LOCATION_KERNEL - Parameters in kernel space
 * PARAM_LOCATION_MIXED  - Parameters in kernel space with potential user mode pointers
 */
typedef enum _API_PARAM_LOCATION
{
    PARAM_LOCATION_USER   = 0,
    PARAM_LOCATION_KERNEL = 1,
    PARAM_LOCATION_MIXED  = 2,
} API_PARAM_LOCATION;


typedef struct _API_SECURITY_INFO
{
    RS_PRIV_LEVEL privLevel;
    API_PARAM_LOCATION paramLocation;
    PSECURITY_TOKEN pProcessToken;
    void *clientOSInfo;
    void *gpuOsInfo;
} API_SECURITY_INFO;


#endif // _NV_SECURITYINFO_H_
