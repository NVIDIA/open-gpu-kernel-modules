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

#ifndef __NV_KMS_H__
#define __NV_KMS_H__

#include "nvtypes.h"
#if defined(NV_KERNEL_INTERFACE_LAYER) && defined(NV_LINUX)
#include <linux/stddef.h> /* size_t */
#else
#include <stddef.h>       /* size_t */
#endif

#include "nvkms-kapi.h"

typedef struct nvkms_per_open nvkms_per_open_handle_t;

typedef void nvkms_procfs_out_string_func_t(void *data,
                                            const char *str);

typedef void nvkms_procfs_proc_t(void *data,
                                 char *buffer, size_t size,
                                 nvkms_procfs_out_string_func_t *outString);

/* max number of loops to prevent hanging the kernel if an edge case is hit */
#define NVKMS_READ_FILE_MAX_LOOPS 1000
/* max size for any file read by the config system */
#define NVKMS_READ_FILE_MAX_SIZE  8192

/*
 * The read file callback should allocate a buffer pointed to by *buff, fill it
 * with the contents of fname, and return the size of the buffer. Buffer is not
 * guaranteed to be null-terminated. The caller is responsible for freeing the
 * buffer with nvkms_free, not nvFree.
 */
typedef size_t nvkms_config_read_file_func_t(char *fname,
                                             char ** const buff);

typedef struct {
    const char *name;
    nvkms_procfs_proc_t *func;
} nvkms_procfs_file_t;

enum NvKmsClientType {
    NVKMS_CLIENT_USER_SPACE,
    NVKMS_CLIENT_KERNEL_SPACE,
};

struct NvKmsPerOpenDev;

NvBool nvKmsIoctl(
    void *pOpenVoid,
    NvU32 cmd,
    NvU64 paramsAddress,
    const size_t paramSize);

void nvKmsClose(void *pOpenVoid);

void* nvKmsOpen(
    NvU32 pid,
    enum NvKmsClientType clientType,
    nvkms_per_open_handle_t *pOpenKernel);

NvBool nvKmsModuleLoad(void);

void nvKmsModuleUnload(void);

void nvKmsSuspend(NvU32 gpuId);
void nvKmsResume(NvU32 gpuId);

void nvKmsGetProcFiles(const nvkms_procfs_file_t **ppProcFiles);

NvBool nvKmsReadConf(const char *buff, size_t size,
                     nvkms_config_read_file_func_t readfile);

void nvKmsKapiHandleEventQueueChange
(
    struct NvKmsKapiDevice *device
);

NvBool nvKmsKapiGetFunctionsTableInternal
(
    struct NvKmsKapiFunctionsTable *funcsTable
);

void nvKmsKapiSuspendResume(NvBool suspend);

NvBool nvKmsGetBacklight(NvU32 display_id, void *drv_priv, NvU32 *brightness);
NvBool nvKmsSetBacklight(NvU32 display_id, void *drv_priv, NvU32 brightness);

NvBool nvKmsOpenDevHasSubOwnerPermissionOrBetter(const struct NvKmsPerOpenDev *pOpenDev);

#endif /* __NV_KMS_H__ */
