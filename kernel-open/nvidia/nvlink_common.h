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

#ifndef _NVLINK_COMMON_H_
#define _NVLINK_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "nvtypes.h"
#include "nvCpuUuid.h"
#include "nvlink_errors.h"


#ifndef NULL
#define NULL ((void *)0)
#endif

// nvlink pci bar information
struct nvlink_pci_bar_info
{
    NvU64  busAddress;
    NvU64  baseAddr;
    NvU64  barSize;
    NvU32  offset;
    void  *pBar;
};

#define MAX_NVLINK_BARS    2

// nvlink pci information
struct nvlink_pci_info
{
    NvU32   domain;
    NvU8    bus;
    NvU8    device;
    NvU8    function;
    NvU32   pciDeviceId;
    NvU32   irq;
    NvBool  intHooked;
    struct  nvlink_pci_bar_info bars[MAX_NVLINK_BARS];
};

// nvlink detailed device information
struct nvlink_detailed_device_info
{
    char       *deviceName;
    NvU64       deviceType;
    NvU8       *devUuid;
    NvBool      bInitialized;
    NvBool      bEnableALI;
    void       *dev_info;     // Endpoint driver device info opaque
                              // to core lib. Passed from end point
                              // driver to core

    struct nvlink_pci_info  *pciInfo;
};

// nvlink device registration parameters
struct nvlink_device_register_params
{
    //
    // Core lib device info opaque to endpoint driver 
    // Passed from core lib to endpoint driver
    //
    void  **deviceHandle;
    char   *driverName;

    struct nvlink_detailed_device_info *device_params;
};

// nvlink detailed link information
struct nvlink_detailed_link_info
{
    void        *deviceHandle; // Core library device handle passed
                               // to endpoint driver during  device
                               // registration

    void        *link_info;    // End point driver link info  opaque 
                               // to core lib. Passed from end point
                               // driver to core

    char        *linkName;
    NvU32        linkNumber;
    NvU32        version;
    NvBool       bAcCoupled;
    const void  *link_handlers;
};

// nvlink link registration parameters
struct nvlink_link_register_params
{
    //
    // Core lib link info opaque to endpoint driver 
    // Passed from core lib to endpoint driver
    //
    void  **linkHandle;

    struct nvlink_detailed_link_info *link_params;
};

// nvlink client device handle
struct nvlink_device_handle
{
    NvU32                  linkMask;
    struct nvlink_pci_info pciInfo;
};

#define NVLINK_PCI_DEV_FMT              "%04x:%02x:%02x.%x"
#define NVLINK_PCI_DEV_FMT_ARGS(info)   (info)->domain, \
                                        (info)->bus,    \
                                        (info)->device, \
                                        (info)->function

// nvlink connection information
struct nvlink_conn_info
{
    NvU32  domain;
    NvU16  bus;
    NvU16  device;
    NvU16  function;
    NvU32  pciDeviceId;
    NvU8   devUuid[NV_UUID_LEN];
    NvU64  deviceType;
    NvU32  linkNumber;
    NvBool bConnected;
    NvU64  chipSid;
};

// nvlink ioctrl params
struct nvlink_ioctrl_params
{
    void  *osPrivate;
    NvU32  cmd;
    void  *buf;
    NvU32  size;
};

// Typedefs
typedef struct nvlink_pci_bar_info           nvlink_pci_bar_info;
typedef struct nvlink_pci_info               nvlink_pci_info;
typedef struct nvlink_detailed_device_info   nvlink_detailed_device_info;
typedef struct nvlink_detailed_link_info     nvlink_detailed_link_info;
typedef struct nvlink_device_register_params nvlink_device_register_params;
typedef struct nvlink_link_register_params   nvlink_link_register_params;
typedef struct nvlink_conn_info              nvlink_conn_info;
typedef struct nvlink_ioctrl_params          nvlink_ioctrl_params;

#ifdef __cplusplus
}
#endif

#endif //_NVLINK_COMMON_H_
