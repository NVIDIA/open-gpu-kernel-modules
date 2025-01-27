/*
 * SPDX-FileCopyrightText: Copyright (c) 2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#ifndef __NV_SMG_H__
#define __NV_SMG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "nvtypes.h"

/*
 * The simplest required abstraction for accessing RM independent of the
 * calling component which may be a kernel module or userspace driver.
 */
typedef NvU32 (*NVSubdevSMGRMControl) (void *ctx, NvU32 object, NvU32 cmd, void *params, NvU32 paramsSize);
typedef NvU32 (*NVSubdevSMGRMAlloc) (void *ctx, NvU32 parent, NvU32 object, NvU32 cls, void *allocParams);
typedef NvU32 (*NVSubdevSMGRMFree) (void *ctx, NvU32 parent, NvU32 object);

NvBool NVSubdevSMGSetPartition(void *ctx,
                               NvU32 subdevHandle,
                               const char *computeInstUuid,
                               NvU32 gpuInstSubscriptionHdl,
                               NvU32 computeInstSubscriptionHdl,
                               NVSubdevSMGRMControl rmControl,
                               NVSubdevSMGRMAlloc rmAlloc,
                               NVSubdevSMGRMFree rmFree);

#ifdef __cplusplus
}
#endif

#endif /* __NV_SMG_H__ */
