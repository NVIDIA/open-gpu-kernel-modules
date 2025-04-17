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
#include "nvlimits.h"
#include "nvrmcontext.h"
#include "nv_mig_types.h"

/*
 * In the context of SMG a MIG device description is the global identity or
 * fingerprint for one MIG device partition that the system has available.
 * These are queried through RM and thus they will be the same in
 * kernelspace and userspace, and remain immutable and cached for the
 * lifetime of the process or kernel module.
 *
 * For now, the graphics driver does NOT support SMG if the MIG partitions
 * change on the fly. RM supports reconfiguring partitions that are not in
 * use but, for now, the kernel and userspace graphics drivers expect the
 * topology of all physical and MIG devices to remain unchanged throughout
 * so that they can agree on the same set of known MIG devices. This is not
 * an unreasonable requirement.
 *
 * Each MIG device description is referred to by a semi-opaque MIGDeviceId.
 * The device id is actually the 0-based index to the table of MIG device
 * descriptions but with bits flipped so that null value is an invalid
 * device. This makes boolean interpretation work more naturally and makes
 * structs from calloc() initialize to an invalid device by default.
 */
typedef struct nvMIGDeviceDescriptionRec {
    /* The globally unique MIG device ID */
    MIGDeviceId migDeviceId;

    /* RM sub/device instance of the physical device hosting the MIG device */
    NvU32 deviceInstance;
    NvU32 subDeviceInstance;

    /* These three uniquely identify a particular MIG device */
    NvU32 gpuId;
    NvU32 gpuInstanceId;
    NvU32 computeInstanceId;

    /* Whether this device is accessible to the calling process */
    NvBool migAccessOk;
    /* MIG exec partition UUID string */
    char migUuid[NV_MIG_DEVICE_UUID_STR_LENGTH];
} nvMIGDeviceDescription;

NvBool nvSMGSubscribeSubDevToPartition(nvRMContextPtr rmctx,
                                       NvU32 subdevHandle,
                                       MIGDeviceId migDevice,
                                       NvU32 gpuInstSubscriptionHdl,
                                       NvU32 computeInstSubscriptionHdl);

NvU32 nvSMGGetDeviceByUUID(nvRMContextPtr rmctx,
                           const char *migUuid,
                           const nvMIGDeviceDescription **uniDev);
NvU32 nvSMGGetDeviceById(nvRMContextPtr rmctx,
                         MIGDeviceId migDev,
                         const nvMIGDeviceDescription **uniDev);
NvU32 nvSMGGetDeviceList(nvRMContextPtr rmctx,
                         nvMIGDeviceDescription **devices,
                         NvU32 *deviceCount);
NvU32 nvSMGGetDefaultDeviceForDeviceInstance(nvRMContextPtr rmctx,
                                             NvU32 deviceInstance,
                                             const nvMIGDeviceDescription **uniDev);

#ifdef __cplusplus
}
#endif

#endif /* __NV_SMG_H__ */
