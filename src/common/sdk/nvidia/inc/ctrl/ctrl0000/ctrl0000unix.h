/*
 * SPDX-FileCopyrightText: Copyright (c) 2009-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#pragma once

#include <nvtypes.h>

//
// This file was generated with FINN, an NVIDIA coding tool.
// Source file:      ctrl/ctrl0000/ctrl0000unix.finn
//

#include "ctrl/ctrl0000/ctrl0000base.h"

#include "ctrl/ctrlxxxx.h"
/* NV01_ROOT (client) Linux control commands and parameters */

/*
 * NV0000_CTRL_CMD_OS_UNIX_FLUSH_USER_CACHE
 *
 * This command may be used to force a cache flush for a range of virtual addresses in 
 * memory. Can be used for either user or kernel addresses.
 *
 *   offset, length
 *     These parameters specify the offset within the memory block
 *     and the number of bytes to flush/invalidate
 *   cacheOps
 *     This parameter flags whether to flush, invalidate or do both.
 *     Possible values are:
 *       NV0000_CTRL_OS_UNIX_FLAGS_USER_CACHE_FLUSH
 *       NV0000_CTRL_OS_UNIX_FLAGS_USER_CACHE_INVALIDATE
 *       NV0000_CTRL_OS_UNIX_FLAGS_USER_CACHE_FLUSH_INVALIDATE
 *   hDevice
 *     This parameter is the handle to the device
 *   hObject
 *     This parameter is the handle to the memory structure being operated on.
 *   internalOnly
 *     Intended for internal use unless client is running in MODS UNIX environment, 
 *     in which case this parameter specify the virtual address of the memory block 
 *     to flush.
 *
 * Possible status values are:
 *   NV_OK
 *   NV_ERR_OBJECT_NOT_FOUND
 *   NV_ERR_INVALID_OBJECT_HANDLE
 *   NV_ERR_INVALID_COMMAND
 *   NV_ERR_INVALID_LIMIT
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV0000_CTRL_CMD_OS_UNIX_FLUSH_USER_CACHE (0x3d02) /* finn: Evaluated from "(FINN_NV01_ROOT_OS_UNIX_INTERFACE_ID << 8) | NV0000_CTRL_OS_UNIX_FLUSH_USER_CACHE_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_OS_UNIX_FLUSH_USER_CACHE_PARAMS_MESSAGE_ID (0x2U)

typedef struct NV0000_CTRL_OS_UNIX_FLUSH_USER_CACHE_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 offset, 8);
    NV_DECLARE_ALIGNED(NvU64 length, 8);
    NvU32    cacheOps;
    NvHandle hDevice;
    NvHandle hObject;
    NV_DECLARE_ALIGNED(NvU64 internalOnly, 8);
} NV0000_CTRL_OS_UNIX_FLUSH_USER_CACHE_PARAMS;

#define NV0000_CTRL_OS_UNIX_FLAGS_USER_CACHE_FLUSH            (0x00000001)
#define NV0000_CTRL_OS_UNIX_FLAGS_USER_CACHE_INVALIDATE       (0x00000002)
#define NV0000_CTRL_OS_UNIX_FLAGS_USER_CACHE_FLUSH_INVALIDATE (0x00000003)


/*
 * NV0000_CTRL_CMD_OS_UNIX_GET_CONTROL_FILE_DESCRIPTOR
 *
 * This command is used to get the control file descriptor.
 *
 * Possible status values returned are:
 *   NV_OK
 *
 */
#define NV0000_CTRL_CMD_OS_UNIX_GET_CONTROL_FILE_DESCRIPTOR   (0x3d04) /* finn: Evaluated from "(FINN_NV01_ROOT_OS_UNIX_INTERFACE_ID << 8) | 0x4" */

typedef struct NV0000_CTRL_OS_UNIX_GET_CONTROL_FILE_DESCRIPTOR_PARAMS {
    NvS32 fd;
} NV0000_CTRL_OS_UNIX_GET_CONTROL_FILE_DESCRIPTOR_PARAMS;

typedef enum NV0000_CTRL_OS_UNIX_EXPORT_OBJECT_TYPE {
    NV0000_CTRL_OS_UNIX_EXPORT_OBJECT_TYPE_NONE = 0,
    NV0000_CTRL_OS_UNIX_EXPORT_OBJECT_TYPE_RM = 1,
} NV0000_CTRL_OS_UNIX_EXPORT_OBJECT_TYPE;

typedef struct NV0000_CTRL_OS_UNIX_EXPORT_OBJECT {
    NV0000_CTRL_OS_UNIX_EXPORT_OBJECT_TYPE type;

    union {
        struct {
            NvHandle hDevice;
            NvHandle hParent;
            NvHandle hObject;
        } rmObject;
    } data;
} NV0000_CTRL_OS_UNIX_EXPORT_OBJECT;

/*
 * NV0000_CTRL_CMD_OS_UNIX_EXPORT_OBJECT_TO_FD
 *
 * This command may be used to export NV0000_CTRL_OS_UNIX_EXPORT_OBJECT_TYPE
 * object to file descriptor.
 *
 * Note that the 'fd' parameter is an input parameter at the kernel level, but
 * an output parameter for usermode RMAPI clients -- the RMAPI library will
 * open a new FD automatically if a usermode RMAPI client exports an object.
 *
 * Kernel-mode RM clients can export an object to an FD in two steps:
 * 1. User client calls this RMControl with the flag 'EMPTY_FD_TRUE' to create
 *    an empty FD to receive the object, then passes that FD to the kernel-mode
 *    RM client.
 * 2. Kernel-mode RM client fills in the rest of the
 *    NV0000_CTRL_OS_UNIX_EXPORT_OBJECT_TO_FD_PARAMS as usual and calls RM to
 *    associate its desired RM object with the empty FD from its usermode
 *    client.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_OBJECT_NOT_FOUND
 *   NV_ERR_INVALID_OBJECT_HANDLE
 *   NV_ERR_INVALID_PARAMETER
 */
#define NV0000_CTRL_CMD_OS_UNIX_EXPORT_OBJECT_TO_FD (0x3d05) /* finn: Evaluated from "(FINN_NV01_ROOT_OS_UNIX_INTERFACE_ID << 8) | NV0000_CTRL_OS_UNIX_EXPORT_OBJECT_TO_FD_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_OS_UNIX_EXPORT_OBJECT_TO_FD_PARAMS_MESSAGE_ID (0x5U)

typedef struct NV0000_CTRL_OS_UNIX_EXPORT_OBJECT_TO_FD_PARAMS {
    NV0000_CTRL_OS_UNIX_EXPORT_OBJECT object; /* IN */
    NvS32                             fd;                                 /* IN/OUT */
    NvU32                             flags;                              /* IN */
} NV0000_CTRL_OS_UNIX_EXPORT_OBJECT_TO_FD_PARAMS;

/*
 * If EMPTY_FD is TRUE, the 'fd' will be created but no object will be
 * associated with it.  The hDevice parameter is still required, to determine
 * the correct device node on which to create the file descriptor.
 * (An empty FD can then be passed to a kernel-mode driver to associate it with
 * an actual object.)
 */
#define NV0000_CTRL_OS_UNIX_EXPORT_OBJECT_TO_FD_FLAGS_EMPTY_FD       0:0
#define NV0000_CTRL_OS_UNIX_EXPORT_OBJECT_TO_FD_FLAGS_EMPTY_FD_FALSE (0x00000000)
#define NV0000_CTRL_OS_UNIX_EXPORT_OBJECT_TO_FD_FLAGS_EMPTY_FD_TRUE  (0x00000001)

/*
 * NV0000_CTRL_CMD_OS_UNIX_IMPORT_OBJECT_FROM_FD
 *
 * This command may be used to import back
 * NV0000_CTRL_OS_UNIX_EXPORT_OBJECT_TYPE object from file descriptor.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_OBJECT_NOT_FOUND
 *   NV_ERR_INVALID_OBJECT_HANDLE
 *   NV_ERR_INVALID_PARAMETER
 */
#define NV0000_CTRL_CMD_OS_UNIX_IMPORT_OBJECT_FROM_FD                (0x3d06) /* finn: Evaluated from "(FINN_NV01_ROOT_OS_UNIX_INTERFACE_ID << 8) | NV0000_CTRL_OS_UNIX_IMPORT_OBJECT_FROM_FD_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_OS_UNIX_IMPORT_OBJECT_FROM_FD_PARAMS_MESSAGE_ID (0x6U)

typedef struct NV0000_CTRL_OS_UNIX_IMPORT_OBJECT_FROM_FD_PARAMS {
    NvS32                             fd;                                 /* IN */
    NV0000_CTRL_OS_UNIX_EXPORT_OBJECT object; /* IN */
} NV0000_CTRL_OS_UNIX_IMPORT_OBJECT_FROM_FD_PARAMS;

/*
 * NV0000_CTRL_CMD_OS_GET_GPU_INFO
 *
 * This command will query the OS specific info for the specified GPU.
 *
 *  gpuId
 *    This parameter should specify a valid GPU ID value.  If there
 *    is no GPU present with the specified ID, a status of
 *    NV_ERR_INVALID_ARGUMENT is returned.
 *  minorNum
 *    This parameter returns minor number of device node.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 */

#define NV0000_CTRL_CMD_OS_GET_GPU_INFO (0x3d07) /* finn: Evaluated from "(FINN_NV01_ROOT_OS_UNIX_INTERFACE_ID << 8) | 0x7" */

typedef struct NV0000_CTRL_OS_GET_GPU_INFO_PARAMS {
    NvU32 gpuId;    /* IN */
    NvU32 minorNum; /* OUT */
} NV0000_CTRL_OS_GET_GPU_INFO_PARAMS;

/*
 * NV0000_CTRL_CMD_OS_UNIX_GET_EXPORT_OBJECT_INFO
 *
 * This command will query the deviceInstance for the specified FD
 * which is referencing an exported object.
 *
 *  fd
 *    File descriptor parameter is referencing an exported object on a Unix system.
 *
 *  deviceInstatnce
 *    This parameter returns a deviceInstance on which the object is located.
 *
 *    NV_MAX_DEVICES is returned if the object is parented by a client instead
 *    of a device.
 *
 *  gpuInstanceId
 *    For objects parented by device this parameter returns MIG GPU instance
 *    id the device is subscribed to or NV_U32_MAX if no subscription was made.
 *
 *  maxObjects
 *    This parameter returns the maximum number of object handles that may be
 *    contained in the file descriptor.
 *
 *  metadata
 *    This parameter returns the user metadata passed into the
 *    _EXPORT_OBJECTS_TO_FD control call.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_OBJECT_NOT_FOUND
 */

#define NV0000_CTRL_CMD_OS_UNIX_GET_EXPORT_OBJECT_INFO (0x3d08) /* finn: Evaluated from "(FINN_NV01_ROOT_OS_UNIX_INTERFACE_ID << 8) | NV0000_CTRL_OS_UNIX_GET_EXPORT_OBJECT_INFO_PARAMS_MESSAGE_ID" */

#define NV0000_OS_UNIX_EXPORT_OBJECT_FD_BUFFER_SIZE    64

#define NV0000_CTRL_OS_UNIX_GET_EXPORT_OBJECT_INFO_PARAMS_MESSAGE_ID (0x8U)

typedef struct NV0000_CTRL_OS_UNIX_GET_EXPORT_OBJECT_INFO_PARAMS {
    NvS32 fd;   /* IN  */
    NvU32 deviceInstance;   /* OUT */
    NvU32 gpuInstanceId;   /* OUT */
    NvU16 maxObjects;   /* OUT */
    NvU8  metadata[NV0000_OS_UNIX_EXPORT_OBJECT_FD_BUFFER_SIZE]; /* OUT */
} NV0000_CTRL_OS_UNIX_GET_EXPORT_OBJECT_INFO_PARAMS;

/*
 * NV0000_CTRL_CMD_OS_UNIX_REFRESH_RMAPI_DEVICE_LIST
 *
 * This command will re-fetch probed GPUs information and update RMAPI library's
 * internal detected GPU context information accordingly. Without this, GPUs
 * attached to RM after RMAPI client initialization will not be accessible and
 * all RMAPI library calls will fail on them.
 * Currently this is used by NVSwitch Fabric Manager in conjunction with NVSwitch
 * Shared Virtualization feature where GPUs are hot-plugged to OS/RM (by Hypervisor)
 * and Fabric Manager is signaled externally by the Hypervisor to initialize those GPUs.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_STATE
 *   NV_ERR_OPERATING_SYSTEM
 */

#define NV0000_CTRL_CMD_OS_UNIX_REFRESH_RMAPI_DEVICE_LIST       (0x3d09) /* finn: Evaluated from "(FINN_NV01_ROOT_OS_UNIX_INTERFACE_ID << 8) | 0x9" */

/*
 * This control call has been deprecated. It will be deleted soon.
 * Use NV0000_CTRL_CMD_OS_UNIX_EXPORT_OBJECT_TO_FD (singular) or
 * NV0000_CTRL_CMD_OS_UNIX_EXPORT_OBJECTS_TO_FD (plural) instead.
 */
#define NV0000_CTRL_CMD_OS_UNIX_CREATE_EXPORT_OBJECT_FD         (0x3d0a) /* finn: Evaluated from "(FINN_NV01_ROOT_OS_UNIX_INTERFACE_ID << 8) | NV0000_CTRL_OS_UNIX_CREATE_EXPORT_OBJECT_FD_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_OS_UNIX_CREATE_EXPORT_OBJECT_FD_BUFFER_SIZE NV0000_OS_UNIX_EXPORT_OBJECT_FD_BUFFER_SIZE

#define NV0000_CTRL_OS_UNIX_CREATE_EXPORT_OBJECT_FD_PARAMS_MESSAGE_ID (0xAU)

typedef struct NV0000_CTRL_OS_UNIX_CREATE_EXPORT_OBJECT_FD_PARAMS {
    NvHandle hDevice;                                                       /* IN */
    NvU16    maxObjects;                                                       /* IN */
    NvU8     metadata[NV0000_CTRL_OS_UNIX_CREATE_EXPORT_OBJECT_FD_BUFFER_SIZE]; /* IN */
    NvS32    fd;                                                               /* IN/OUT */
} NV0000_CTRL_OS_UNIX_CREATE_EXPORT_OBJECT_FD_PARAMS;

/*
 * NV0000_CTRL_CMD_OS_UNIX_EXPORT_OBJECTS_TO_FD
 *
 * Exports RM handles to an fd that was provided, also creates an FD if
 * requested.
 *
 * The objects in the 'handles' array are exported into the fd
 * as the range [index, index + numObjects).
 *
 * If index + numObjects is greater than the maxObjects value used
 * to create the file descriptor, NV_ERR_INVALID_PARAMETER is returned.
 *
 * If 'numObjects and 'index' overlap with a prior call, the newer call's RM object
 * handles will overwrite the previously exported handles from the previous call.
 * This overlapping behavior can also be used to unexport a handle by setting
 * the appropriate object in 'objects' to 0.
 *
 *  fd
 *    A file descriptor. If -1, a new FD will be created.
 *
 *  hDevice
 *    The owning device of the objects to be exported (must be the same for
 *    all objects).
 *
 *  maxObjects
 *    The total number of objects that the client wishes to export to the FD.
 *    This parameter will be honored only when the FD is getting created.
 *
 *  metadata
 *    A buffer for clients to write some metadata to and pass to the importing
 *    client. This parameter will be honored only when the FD is getting
 *    created.
 *
 *  objects
 *    Array of RM object handles to export to the fd.
 *
 *  numObjects
 *    The number of handles the user wishes to export in this call.
 *
 *  index
 *    The index into the export fd at which to start exporting the handles in
 *    'objects' (for use in iterative calls).
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_OUT_OF_RANGE
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_OBJECT_NOT_FOUND
 *   NV_ERR_INVALID_OBJECT_HANDLE
 */
#define NV0000_CTRL_CMD_OS_UNIX_EXPORT_OBJECTS_TO_FD         (0x3d0b) /* finn: Evaluated from "(FINN_NV01_ROOT_OS_UNIX_INTERFACE_ID << 8) | NV0000_CTRL_OS_UNIX_EXPORT_OBJECTS_TO_FD_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_OS_UNIX_EXPORT_OBJECTS_TO_FD_MAX_OBJECTS 512

#define NV0000_CTRL_OS_UNIX_EXPORT_OBJECTS_TO_FD_PARAMS_MESSAGE_ID (0xBU)

typedef struct NV0000_CTRL_OS_UNIX_EXPORT_OBJECTS_TO_FD_PARAMS {
    NvS32    fd;                                                               /* IN/OUT */
    NvHandle hDevice;                                                       /* IN */
    NvU16    maxObjects;                                                       /* IN */
    NvU8     metadata[NV0000_OS_UNIX_EXPORT_OBJECT_FD_BUFFER_SIZE];             /* IN */
    NvHandle objects[NV0000_CTRL_OS_UNIX_EXPORT_OBJECTS_TO_FD_MAX_OBJECTS]; /* IN */
    NvU16    numObjects;                                                       /* IN */
    NvU16    index;                                                            /* IN */
} NV0000_CTRL_OS_UNIX_EXPORT_OBJECTS_TO_FD_PARAMS;

/*
 * NV0000_CTRL_CMD_OS_UNIX_IMPORT_OBJECTS_FROM_FD
 *
 * This command can be used to import back RM handles
 * that were exported to an fd using the
 * NV0000_CTRL_CMD_OS_UNIX_EXPORT_OBJECTS_TO_FD control call.
 *
 * If index + numObjects is greater than the maxObjects value used
 * to create the file descriptor, NV_ERR_INVALID_PARAMETER is returned
 * and no objects are imported.
 *
 * For each valid handle in the 'objects' array parameter at index 'i',
 * the corresponding object handle at index ('i' + 'index') contained by
 * the fd will be imported. If the object at index ('i' + 'index') has
 * not been exported into the fd, no object will be imported.
 *
 * If any of handles contained in the 'objects' array parameter are invalid
 * and the corresponding export object handle is valid,
 * NV_ERR_INVALID_PARAMETER will be returned and no handles will be imported.
 *
 *  fd
 *    The export fd on which to import handles out of.
 *
 *  hParent
 *    The parent RM handle of which all of the exported objects will
 *    be duped under.
 *
 *  objects
 *    An array of RM handles. The exported objects will be duped under
 *    these handles during the import process.
 *
 * objectTypes
 *    An array of RM handle types. The type _NONE will be returned if
 *    the object was not imported. Other possible object types are
 *    mentioned below.
 *
 *  numObjects
 *    The number of valid object handles in the 'objects' array. This should
 *    be set to the number of objects that the client wishes to import.
 *
 *  index
 *    The index into the fd in which to start importing from. For
 *    use in iterative calls.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_OUT_OF_RANGE
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_OBJECT_NOT_FOUND
 *   NV_ERR_INVALID_OBJECT_HANDLE
 *   NV_ERR_INVALID_PARAMETER
 */
#define NV0000_CTRL_CMD_OS_UNIX_IMPORT_OBJECTS_FROM_FD       (0x3d0c) /* finn: Evaluated from "(FINN_NV01_ROOT_OS_UNIX_INTERFACE_ID << 8) | NV0000_CTRL_OS_UNIX_IMPORT_OBJECTS_FROM_FD_PARAMS_MESSAGE_ID" */

//
// TODO Bump this back up to 512 after the FLA revamp is complete
//
#define NV0000_CTRL_OS_UNIX_IMPORT_OBJECTS_TO_FD_MAX_OBJECTS 128

#define NV0000_CTRL_CMD_OS_UNIX_IMPORT_OBJECT_TYPE_NONE      0
#define NV0000_CTRL_CMD_OS_UNIX_IMPORT_OBJECT_TYPE_VIDMEM    1
#define NV0000_CTRL_CMD_OS_UNIX_IMPORT_OBJECT_TYPE_SYSMEM    2
#define NV0000_CTRL_CMD_OS_UNIX_IMPORT_OBJECT_TYPE_FABRIC    3
#define NV0000_CTRL_CMD_OS_UNIX_IMPORT_OBJECT_TYPE_FABRIC_MC 4

#define NV0000_CTRL_OS_UNIX_IMPORT_OBJECTS_FROM_FD_PARAMS_MESSAGE_ID (0xCU)

typedef struct NV0000_CTRL_OS_UNIX_IMPORT_OBJECTS_FROM_FD_PARAMS {
    NvS32    fd;                                                                   /* IN  */
    NvHandle hParent;                                                           /* IN  */
    NvHandle objects[NV0000_CTRL_OS_UNIX_IMPORT_OBJECTS_TO_FD_MAX_OBJECTS];     /* IN  */
    NvU8     objectTypes[NV0000_CTRL_OS_UNIX_IMPORT_OBJECTS_TO_FD_MAX_OBJECTS];     /* OUT */
    NvU16    numObjects;                                                           /* IN  */
    NvU16    index;                                                                /* IN  */
} NV0000_CTRL_OS_UNIX_IMPORT_OBJECTS_FROM_FD_PARAMS;

/* _ctrl0000unix_h_ */
