/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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


#ifndef NV_IOCTL_H
#define NV_IOCTL_H

#include <nv-ioctl-numbers.h>
#include <nvtypes.h>

typedef struct {
    NvU32    domain;        /* PCI domain number   */
    NvU8     bus;           /* PCI bus number      */
    NvU8     slot;          /* PCI slot number     */
    NvU8     function;      /* PCI function number */
    NvU16    vendor_id;     /* PCI vendor ID       */
    NvU16    device_id;     /* PCI device ID       */
} nv_pci_info_t;

/*
 * ioctl()'s with parameter structures too large for the
 * _IOC cmd layout use the nv_ioctl_xfer_t structure
 * and the NV_ESC_IOCTL_XFER_CMD ioctl() to pass the actual
 * size and user argument pointer into the RM, which
 * will then copy it to/from kernel space in separate steps.
 */
typedef struct nv_ioctl_xfer
{
    NvU32   cmd;
    NvU32   size;
    NvP64   ptr  NV_ALIGN_BYTES(8);
} nv_ioctl_xfer_t;

typedef struct nv_ioctl_card_info
{
    NvBool        valid;
    nv_pci_info_t pci_info;            /* PCI config information      */
    NvU32         gpu_id;
    NvU16         interrupt_line;
    NvU64         reg_address    NV_ALIGN_BYTES(8);
    NvU64         reg_size       NV_ALIGN_BYTES(8);
    NvU64         fb_address     NV_ALIGN_BYTES(8);
    NvU64         fb_size        NV_ALIGN_BYTES(8);
    NvU32         minor_number;
    NvU8          dev_name[10];  /* device names such as vmgfx[0-32] for vmkernel */
} nv_ioctl_card_info_t;

/* alloc event */
typedef struct nv_ioctl_alloc_os_event
{
    NvHandle hClient;
    NvHandle hDevice;
    NvU32    fd;
    NvU32    Status;
} nv_ioctl_alloc_os_event_t;

/* free event */
typedef struct nv_ioctl_free_os_event
{
    NvHandle hClient;
    NvHandle hDevice;
    NvU32    fd;
    NvU32    Status;
} nv_ioctl_free_os_event_t;

/* status code */
typedef struct nv_ioctl_status_code
{
    NvU32 domain;
    NvU8  bus;
    NvU8  slot;
    NvU32 status;
} nv_ioctl_status_code_t;

/* check version string */
#define NV_RM_API_VERSION_STRING_LENGTH 64

typedef struct nv_ioctl_rm_api_version
{
    NvU32 cmd;
    NvU32 reply;
    char versionString[NV_RM_API_VERSION_STRING_LENGTH];
} nv_ioctl_rm_api_version_t;

#define NV_RM_API_VERSION_CMD_STRICT         0
#define NV_RM_API_VERSION_CMD_RELAXED       '1'
#define NV_RM_API_VERSION_CMD_QUERY         '2'

#define NV_RM_API_VERSION_REPLY_UNRECOGNIZED 0
#define NV_RM_API_VERSION_REPLY_RECOGNIZED   1

typedef struct nv_ioctl_query_device_intr
{
    NvU32 intrStatus NV_ALIGN_BYTES(4);
    NvU32 status;
} nv_ioctl_query_device_intr;

/* system parameters that the kernel driver may use for configuration */
typedef struct nv_ioctl_sys_params
{
    NvU64 memblock_size NV_ALIGN_BYTES(8);
} nv_ioctl_sys_params_t;

typedef struct nv_ioctl_register_fd
{
    int ctl_fd;
} nv_ioctl_register_fd_t;

#define NV_DMABUF_EXPORT_MAX_HANDLES 128

#define NV_DMABUF_EXPORT_MAPPING_TYPE_DEFAULT        0
#define NV_DMABUF_EXPORT_MAPPING_TYPE_FORCE_PCIE     1

typedef struct nv_ioctl_export_to_dma_buf_fd
{
    int         fd;
    NvHandle    hClient;
    NvU32       totalObjects;
    NvU32       numObjects;
    NvU32       index;
    NvU64       totalSize NV_ALIGN_BYTES(8);
    NvU8        mappingType;
    NvHandle    handles[NV_DMABUF_EXPORT_MAX_HANDLES];
    NvU64       offsets[NV_DMABUF_EXPORT_MAX_HANDLES] NV_ALIGN_BYTES(8);
    NvU64       sizes[NV_DMABUF_EXPORT_MAX_HANDLES] NV_ALIGN_BYTES(8);
    NvU32       status;
} nv_ioctl_export_to_dma_buf_fd_t;

typedef struct nv_ioctl_wait_open_complete
{
    int rc;
    NvU32 adapterStatus;
} nv_ioctl_wait_open_complete_t;

#endif
