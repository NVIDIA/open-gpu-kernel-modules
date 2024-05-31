/*
 * SPDX-FileCopyrightText: Copyright (c) 2017-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _IOCTL_NVSWITCH_H_
#define _IOCTL_NVSWITCH_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "ioctl_common_nvswitch.h"
#include "nvCpuUuid.h"

/* 4 chars for "SWX-" prefix + 36 chars for UUID string + 1 char for '\0' */
#define NVSWITCH_UUID_STRING_LENGTH 41

#define NVSWITCH_NIBBLE_TO_CHAR(nibble) \
    (((nibble) > 9) ? (((nibble) - 10) + 'A') : ((nibble) + '0'))

static NV_INLINE
NvU32 nvswitch_uuid_to_string(NvUuid *uuid, char *str, NvU32 strLen)
{
    int i;
    int j = 0;

    if ((uuid == NULL) || (str == NULL) || (strLen < NVSWITCH_UUID_STRING_LENGTH))
    {
        return 0;
    }

    str[j++] = 'S';
    str[j++] = 'W';
    str[j++] = 'X';
    str[j++] = '-';

    for (i = 0; i < NV_UUID_LEN; i++)
    {
        if ((i == 4) || (i == 6) || (i == 8) || (i == 10))
        {
            str[j++] = '-';
        }

        str[j++] = NVSWITCH_NIBBLE_TO_CHAR((uuid->uuid[i] & 0xF0) >> 4);
        str[j++] = NVSWITCH_NIBBLE_TO_CHAR(uuid->uuid[i] & 0x0F);
    }

    str[j++] = '\0';

    return j;
}

/*
 * This file defines IOCTL calls that work with nvidia-nvswitchctl
 * (device agnostic) node.
 */

#define NVSWITCH_VERSION_STRING_LENGTH  64

/*
 * Version string
 */
typedef struct
{
    char version[NVSWITCH_VERSION_STRING_LENGTH];
} NVSWITCH_VERSION;

/*
 * NVSWITCH_CTL_CHECK_VERSION
 *
 * The interface will check if the client's version is supported by the driver.
 *
 * Parameters:
 * user[in]
 *    Version of the interface that the client is compiled with.
 * kernel[out]
 *    Version of the interface that the kernel driver is compiled with.
 * is_compatible[out]
 *    Set to true, if user and kernel version are compatible.
 */
typedef struct
{
    NVSWITCH_VERSION user;
    NVSWITCH_VERSION kernel;
    NvBool is_compatible;
} NVSWITCH_CHECK_VERSION_PARAMS;

/*
 * Max devices supported by the driver
 *
 * See ctrl_dev_nvswitch.h for preprocessor definition modification guidelines.
 */
#define NVSWITCH_MAX_DEVICES 64

/*
 * NVSWITCH_CTL_GET_DEVICES
 *
 * This control call will be removed soon. Use NVSWITCH_CTL_GET_DEVICES_V2 instead.
 *
 * Provides information about registered NvSwitch devices.
 *
 * Parameters:
 * deviceInstance[out]
 *    Device instance of the device. This is same as the device minor number
 *    for Linux platforms.
 */
typedef struct
{
    NvU32 deviceInstance;
    NvU32 pciDomain;
    NvU32 pciBus;
    NvU32 pciDevice;
    NvU32 pciFunction;
    /* See ctrl_dev_nvswitch.h for struct definition modification guidelines */
} NVSWITCH_DEVICE_INSTANCE_INFO;

typedef struct
{
    NvU32 deviceCount;
    NVSWITCH_DEVICE_INSTANCE_INFO info[NVSWITCH_MAX_DEVICES];
    /* See ctrl_dev_nvswitch.h for struct definition modification guidelines */
} NVSWITCH_GET_DEVICES_PARAMS;

/*
 * NVSWITCH_CTL_GET_DEVICES_V2
 *
 * Provides information about registered NvSwitch devices.
 * V2 adds a UUID field to the device instance info struct
 *
 * Parameters:
 * deviceInstance[out]
 *    Device instance of the device. This is same as the device minor number
 *    for Linux platforms.
 */
typedef struct
{
    NvU32 deviceInstance;
    NvUuid uuid;
    NvU32 pciDomain;
    NvU32 pciBus;
    NvU32 pciDevice;
    NvU32 pciFunction;
    NVSWITCH_DRIVER_FABRIC_STATE driverState;
    NVSWITCH_DEVICE_FABRIC_STATE deviceState;
    NVSWITCH_DEVICE_BLACKLIST_REASON deviceReason;
    NvU32 physId;
    NvBool bTnvlEnabled;

    /* See ctrl_dev_nvswitch.h for struct definition modification guidelines */
} NVSWITCH_DEVICE_INSTANCE_INFO_V2;

#define NVSWITCH_INVALID_PHYS_ID        NV_U32_MAX

typedef struct
{
    NvU32 deviceCount;
    NVSWITCH_DEVICE_INSTANCE_INFO_V2 info[NVSWITCH_MAX_DEVICES];
    /* See ctrl_dev_nvswitch.h for struct definition modification guidelines */
} NVSWITCH_GET_DEVICES_V2_PARAMS;

#define NVSWITCH_DEVICE_NAME_STRING_LENGTH 10

/*
 * CTRL_NVSWITCH_GET_DEVICE_NODES
 *
 * Provides a mapping of the VMWare kernel device names (vmfgx[N]) and registered
 * NVSwitch devices (nvidia-nvswitch[N]).
 * 
 * This IOCTL is only implemented for VMWare.
 *
 * Parameters:
 * deviceInstance[out]
 *    Device instance of the device. This is same as the device minor number
 *    for VMWare platforms.
 * dev_name[out]
 *    VMWare kernel device name of the nvswitch device (vmfgx[N])
 */
typedef struct
{
    NvU32  deviceInstance;
    NvUuid uuid;
    NvU8   dev_name[NVSWITCH_DEVICE_NAME_STRING_LENGTH];
    /* See ctrl_dev_nvswitch.h for struct definition modification guidelines */
} NVSWITCH_DEVICE_NODE_INFO;

typedef struct
{
    NvU32 deviceCount;
    NVSWITCH_DEVICE_NODE_INFO info[NVSWITCH_MAX_DEVICES];
    /* See ctrl_dev_nvswitch.h for struct definition modification guidelines */
} NVSWITCH_GET_DEVICE_NODES_PARAMS;

#define CTRL_NVSWITCH_GET_DEVICES         0x01
#define CTRL_NVSWITCH_CHECK_VERSION       0x02
#define CTRL_NVSWITCH_GET_DEVICES_V2      0x03
#define CTRL_NVSWITCH_GET_DEVICE_NODES    0x04

/*
 * Nvswitchctl (device agnostic) IOCTLs
 */

#define IOCTL_NVSWITCH_GET_DEVICES \
    NVSWITCH_IOCTL_CODE(NVSWITCH_CTL_IO_TYPE, CTRL_NVSWITCH_GET_DEVICES, NVSWITCH_GET_DEVICES_PARAMS, \
                        NVSWITCH_IO_READ_ONLY)
#define IOCTL_NVSWITCH_CHECK_VERSION \
    NVSWITCH_IOCTL_CODE(NVSWITCH_CTL_IO_TYPE, CTRL_NVSWITCH_CHECK_VERSION, NVSWITCH_CHECK_VERSION_PARAMS, \
                        NVSWITCH_IO_WRITE_READ)
#define IOCTL_NVSWITCH_GET_DEVICES_V2 \
    NVSWITCH_IOCTL_CODE(NVSWITCH_CTL_IO_TYPE, CTRL_NVSWITCH_GET_DEVICES_V2, NVSWITCH_GET_DEVICES_V2_PARAMS, \
                        NVSWITCH_IO_READ_ONLY)
#define IOCTL_NVSWITCH_GET_DEVICE_NODES \
    NVSWITCH_IOCTL_CODE(NVSWITCH_CTL_IO_TYPE, CTRL_NVSWITCH_GET_DEVICE_NODES, NVSWITCH_GET_DEVICE_NODES_PARAMS, \
                        NVSWITCH_IO_READ_ONLY)

#ifdef __cplusplus
}
#endif

#endif //_IOCTL_NVSWITCH_H_
