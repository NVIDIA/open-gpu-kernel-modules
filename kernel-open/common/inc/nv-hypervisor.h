/*
 * SPDX-FileCopyrightText: Copyright (c) 1999-2018 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _NV_HYPERVISOR_H_
#define _NV_HYPERVISOR_H_

#include <nv-kernel-interface-api.h>

// Enums for supported hypervisor types.
// New hypervisor type should be added before OS_HYPERVISOR_UNKNOWN
typedef enum _HYPERVISOR_TYPE
{
    OS_HYPERVISOR_XEN = 0,
    OS_HYPERVISOR_VMWARE,
    OS_HYPERVISOR_HYPERV,
    OS_HYPERVISOR_KVM,
    OS_HYPERVISOR_UNKNOWN
} HYPERVISOR_TYPE;

#define CMD_VFIO_WAKE_REMOVE_GPU              1
#define CMD_VGPU_VFIO_PRESENT                 2
#define CMD_VFIO_PCI_CORE_PRESENT             3

#define MAX_VF_COUNT_PER_GPU                  64

typedef enum _VGPU_TYPE_INFO
{
    VGPU_TYPE_NAME = 0,
    VGPU_TYPE_DESCRIPTION,
    VGPU_TYPE_INSTANCES,
} VGPU_TYPE_INFO;

typedef struct
{
    void  *nv;
    NvU32 domain;
    NvU32 bus;
    NvU32 device;
    NvU32 return_status;
} vgpu_vfio_info;

typedef struct
{
    NvU32       domain;
    NvU8        bus;
    NvU8        slot;
    NvU8        function;
    NvBool      isNvidiaAttached;
    NvBool      isMdevAttached;
} vgpu_vf_pci_info;

typedef enum VGPU_CMD_PROCESS_VF_INFO_E
{
    NV_VGPU_SAVE_VF_INFO         = 0,
    NV_VGPU_REMOVE_VF_PCI_INFO   = 1,
    NV_VGPU_REMOVE_VF_MDEV_INFO  = 2,
    NV_VGPU_GET_VF_INFO          = 3
} VGPU_CMD_PROCESS_VF_INFO;

typedef enum VGPU_DEVICE_STATE_E
{
    NV_VGPU_DEV_UNUSED = 0,
    NV_VGPU_DEV_OPENED = 1,
    NV_VGPU_DEV_IN_USE = 2
} VGPU_DEVICE_STATE;

/*
 * Function prototypes
 */

HYPERVISOR_TYPE NV_API_CALL nv_get_hypervisor_type(void);

#endif // _NV_HYPERVISOR_H_
