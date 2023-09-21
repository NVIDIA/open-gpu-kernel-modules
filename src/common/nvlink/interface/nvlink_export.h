/*
 * SPDX-FileCopyrightText: Copyright (c) 2015 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _NVLINK_EXPORT_H_
#define _NVLINK_EXPORT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "nvlink_common.h"

/*
 * Initializes core lib and does all that is needed
 * to access NVLINK functionality on the current platform.
 */
NvlStatus nvlink_lib_initialize(void);

/*
 * Frees any related resources and then unloads core lib. 
 */
NvlStatus nvlink_lib_unload(void);

/*
* Entry point for nvlink ioctl calls.
*/
NvlStatus nvlink_lib_ioctl_ctrl(nvlink_ioctrl_params *ctrl_params);

/*
* Gets number of devices with type deviceType
*/
NvlStatus nvlink_lib_return_device_count_by_type(NvU32 deviceType, NvU32 *numDevices);

#ifdef __cplusplus
}
#endif

#endif //_NVLINK_EXPORT_H_
