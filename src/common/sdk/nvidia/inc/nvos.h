/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

 /***************************************************************************\
|*                                                                           *|
|*                         NV Architecture Interface                         *|
|*                                                                           *|
|*  <nvos.h>  defines the Operating System function and ioctl interfaces to  *|
|*  NVIDIA's Unified Media Architecture (TM).                                *|
|*                                                                           *|
 \***************************************************************************/

#ifndef NVOS_INCLUDED
#define NVOS_INCLUDED
#ifdef __cplusplus
extern "C" {
#endif

#include "nvstatus.h"

#include "nvgputypes.h"
#include "rs_access.h"
#include "nvcfg_sdk.h"


// Temporary include. Please include this directly instead of nvos.h
#include "alloc/alloc_channel.h"

/* local defines here */
#define FILE_DEVICE_NV      0x00008000
#define NV_IOCTL_FCT_BASE   0x00000800

// This is the maximum number of subdevices supported in an SLI
// configuration.
#define NVOS_MAX_SUBDEVICES 8

/* Define to indicate the use of Unified status codes - bug 200043705*/
#define UNIFIED_NV_STATUS 1

 /***************************************************************************\
|*                              NV OS Functions                              *|
 \***************************************************************************/

/*
   Result codes for RM APIs, shared for all the APIs

   *** IMPORTANT ***

   Ensure that no NVOS_STATUS value has the highest bit set. That bit
   is used to passthrough the NVOS_STATUS on code expecting an RM_STATUS.
*/
#define NVOS_STATUS NV_STATUS

#define NVOS_STATUS_SUCCESS                                     NV_OK
#define NVOS_STATUS_ERROR_CARD_NOT_PRESENT                      NV_ERR_CARD_NOT_PRESENT
#define NVOS_STATUS_ERROR_DUAL_LINK_INUSE                       NV_ERR_DUAL_LINK_INUSE
#define NVOS_STATUS_ERROR_GENERIC                               NV_ERR_GENERIC
#define NVOS_STATUS_ERROR_GPU_NOT_FULL_POWER                    NV_ERR_GPU_NOT_FULL_POWER
#define NVOS_STATUS_ERROR_ILLEGAL_ACTION                        NV_ERR_ILLEGAL_ACTION
#define NVOS_STATUS_ERROR_IN_USE                                NV_ERR_STATE_IN_USE
#define NVOS_STATUS_ERROR_INSUFFICIENT_RESOURCES                NV_ERR_INSUFFICIENT_RESOURCES
#define NVOS_STATUS_ERROR_INVALID_ACCESS_TYPE                   NV_ERR_INVALID_ACCESS_TYPE
#define NVOS_STATUS_ERROR_INVALID_ARGUMENT                      NV_ERR_INVALID_ARGUMENT
#define NVOS_STATUS_ERROR_INVALID_BASE                          NV_ERR_INVALID_BASE
#define NVOS_STATUS_ERROR_INVALID_CHANNEL                       NV_ERR_INVALID_CHANNEL
#define NVOS_STATUS_ERROR_INVALID_CLASS                         NV_ERR_INVALID_CLASS
#define NVOS_STATUS_ERROR_INVALID_CLIENT                        NV_ERR_INVALID_CLIENT
#define NVOS_STATUS_ERROR_INVALID_COMMAND                       NV_ERR_INVALID_COMMAND
#define NVOS_STATUS_ERROR_INVALID_DATA                          NV_ERR_INVALID_DATA
#define NVOS_STATUS_ERROR_INVALID_DEVICE                        NV_ERR_INVALID_DEVICE
#define NVOS_STATUS_ERROR_INVALID_DMA_SPECIFIER                 NV_ERR_INVALID_DMA_SPECIFIER
#define NVOS_STATUS_ERROR_INVALID_EVENT                         NV_ERR_INVALID_EVENT
#define NVOS_STATUS_ERROR_INVALID_FLAGS                         NV_ERR_INVALID_FLAGS
#define NVOS_STATUS_ERROR_INVALID_FUNCTION                      NV_ERR_INVALID_FUNCTION
#define NVOS_STATUS_ERROR_INVALID_HEAP                          NV_ERR_INVALID_HEAP
#define NVOS_STATUS_ERROR_INVALID_INDEX                         NV_ERR_INVALID_INDEX
#define NVOS_STATUS_ERROR_INVALID_LIMIT                         NV_ERR_INVALID_LIMIT
#define NVOS_STATUS_ERROR_INVALID_METHOD                        NV_ERR_INVALID_METHOD
#define NVOS_STATUS_ERROR_INVALID_OBJECT_BUFFER                 NV_ERR_BUFFER_TOO_SMALL
#define NVOS_STATUS_ERROR_INVALID_OBJECT_ERROR                  NV_ERR_INVALID_OBJECT
#define NVOS_STATUS_ERROR_INVALID_OBJECT_HANDLE                 NV_ERR_INVALID_OBJECT_HANDLE
#define NVOS_STATUS_ERROR_INVALID_OBJECT_NEW                    NV_ERR_INVALID_OBJECT_NEW
#define NVOS_STATUS_ERROR_INVALID_OBJECT_OLD                    NV_ERR_INVALID_OBJECT_OLD
#define NVOS_STATUS_ERROR_INVALID_OBJECT_PARENT                 NV_ERR_INVALID_OBJECT_PARENT
#define NVOS_STATUS_ERROR_INVALID_OFFSET                        NV_ERR_INVALID_OFFSET
#define NVOS_STATUS_ERROR_INVALID_OWNER                         NV_ERR_INVALID_OWNER
#define NVOS_STATUS_ERROR_INVALID_PARAM_STRUCT                  NV_ERR_INVALID_PARAM_STRUCT
#define NVOS_STATUS_ERROR_INVALID_PARAMETER                     NV_ERR_INVALID_PARAMETER
#define NVOS_STATUS_ERROR_INVALID_POINTER                       NV_ERR_INVALID_POINTER
#define NVOS_STATUS_ERROR_INVALID_REGISTRY_KEY                  NV_ERR_INVALID_REGISTRY_KEY
#define NVOS_STATUS_ERROR_INVALID_STATE                         NV_ERR_INVALID_STATE
#define NVOS_STATUS_ERROR_INVALID_STRING_LENGTH                 NV_ERR_INVALID_STRING_LENGTH
#define NVOS_STATUS_ERROR_INVALID_XLATE                         NV_ERR_INVALID_XLATE
#define NVOS_STATUS_ERROR_IRQ_NOT_FIRING                        NV_ERR_IRQ_NOT_FIRING
#define NVOS_STATUS_ERROR_MULTIPLE_MEMORY_TYPES                 NV_ERR_MULTIPLE_MEMORY_TYPES
#define NVOS_STATUS_ERROR_NOT_SUPPORTED                         NV_ERR_NOT_SUPPORTED
#define NVOS_STATUS_ERROR_OPERATING_SYSTEM                      NV_ERR_OPERATING_SYSTEM
#define NVOS_STATUS_ERROR_LIB_RM_VERSION_MISMATCH               NV_ERR_LIB_RM_VERSION_MISMATCH
#define NVOS_STATUS_ERROR_PROTECTION_FAULT                      NV_ERR_PROTECTION_FAULT
#define NVOS_STATUS_ERROR_TIMEOUT                               NV_ERR_TIMEOUT
#define NVOS_STATUS_ERROR_TOO_MANY_PRIMARIES                    NV_ERR_TOO_MANY_PRIMARIES
#define NVOS_STATUS_ERROR_IRQ_EDGE_TRIGGERED                    NV_ERR_IRQ_EDGE_TRIGGERED
#define NVOS_STATUS_ERROR_INVALID_OPERATION                     NV_ERR_INVALID_OPERATION
#define NVOS_STATUS_ERROR_NOT_COMPATIBLE                        NV_ERR_NOT_COMPATIBLE
#define NVOS_STATUS_ERROR_MORE_PROCESSING_REQUIRED              NV_WARN_MORE_PROCESSING_REQUIRED
#define NVOS_STATUS_ERROR_INSUFFICIENT_PERMISSIONS              NV_ERR_INSUFFICIENT_PERMISSIONS
#define NVOS_STATUS_ERROR_TIMEOUT_RETRY                         NV_ERR_TIMEOUT_RETRY
#define NVOS_STATUS_ERROR_NOT_READY                             NV_ERR_NOT_READY
#define NVOS_STATUS_ERROR_GPU_IS_LOST                           NV_ERR_GPU_IS_LOST
#define NVOS_STATUS_ERROR_IN_FULLCHIP_RESET                     NV_ERR_GPU_IN_FULLCHIP_RESET
#define NVOS_STATUS_ERROR_INVALID_LOCK_STATE                    NV_ERR_INVALID_LOCK_STATE
#define NVOS_STATUS_ERROR_INVALID_ADDRESS                       NV_ERR_INVALID_ADDRESS
#define NVOS_STATUS_ERROR_INVALID_IRQ_LEVEL                     NV_ERR_INVALID_IRQ_LEVEL
#define NVOS_STATUS_ERROR_MEMORY_TRAINING_FAILED                NV_ERR_MEMORY_TRAINING_FAILED
#define NVOS_STATUS_ERROR_BUSY_RETRY                            NV_ERR_BUSY_RETRY
#define NVOS_STATUS_ERROR_INSUFFICIENT_POWER                    NV_ERR_INSUFFICIENT_POWER
#define NVOS_STATUS_ERROR_OBJECT_NOT_FOUND                      NV_ERR_OBJECT_NOT_FOUND
#define NVOS_STATUS_ERROR_RESOURCE_LOST                         NV_ERR_RESOURCE_LOST
#define NVOS_STATUS_ERROR_BUFFER_TOO_SMALL                      NV_ERR_BUFFER_TOO_SMALL
#define NVOS_STATUS_ERROR_RESET_REQUIRED                        NV_ERR_RESET_REQUIRED
#define NVOS_STATUS_ERROR_INVALID_REQUEST                       NV_ERR_INVALID_REQUEST

#define NVOS_STATUS_ERROR_PRIV_SEC_VIOLATION                    NV_ERR_PRIV_SEC_VIOLATION
#define NVOS_STATUS_ERROR_GPU_IN_DEBUG_MODE                     NV_ERR_GPU_IN_DEBUG_MODE
#define NVOS_STATUS_ERROR_ALREADY_SIGNALLED                     NV_ERR_ALREADY_SIGNALLED

/*
    Note:
        This version of the architecture has been changed to allow the
        RM to return a client handle that will subsequently used to
        identify the client.  NvAllocRoot() returns the handle.  All
        other functions must specify this client handle.

*/
/* macro NV01_FREE */
#define  NV01_FREE                                                 (0x00000000)

/* NT ioctl data structure */
typedef struct
{
  NvHandle  hRoot;
  NvHandle  hObjectParent;
  NvHandle  hObjectOld;
  NvV32     status;
} NVOS00_PARAMETERS;

/* valid hClass values. */
#define  NV01_ROOT                                                 (0x0U)
//
// Redefining it here to maintain consistency with current code
// This is also defined in class cl0001.h
//
#define  NV01_ROOT_NON_PRIV                                        (0x00000001)

// Deprecated, please use NV01_ROOT_CLIENT
#define  NV01_ROOT_USER                                            NV01_ROOT_CLIENT

//
// This will eventually replace NV01_ROOT_USER in RM client code. Please use this
// RM client object type for any new RM client object allocations that are being
// added.
//
#define  NV01_ROOT_CLIENT                                          (0x00000041)

/* macro NV01_ALLOC_MEMORY */
#define  NV01_ALLOC_MEMORY                                         (0x00000002)

/* parameter values */
#define NVOS02_FLAGS_PHYSICALITY                                   7:4
#define NVOS02_FLAGS_PHYSICALITY_CONTIGUOUS                        (0x00000000)
#define NVOS02_FLAGS_PHYSICALITY_NONCONTIGUOUS                     (0x00000001)
#define NVOS02_FLAGS_LOCATION                                      11:8
#define NVOS02_FLAGS_LOCATION_PCI                                  (0x00000000)
#define NVOS02_FLAGS_LOCATION_AGP                                  (0x00000001)
#define NVOS02_FLAGS_LOCATION_VIDMEM                               (0x00000002)
#define NVOS02_FLAGS_COHERENCY                                     15:12
#define NVOS02_FLAGS_COHERENCY_UNCACHED                            (0x00000000)
#define NVOS02_FLAGS_COHERENCY_CACHED                              (0x00000001)
#define NVOS02_FLAGS_COHERENCY_WRITE_COMBINE                       (0x00000002)
#define NVOS02_FLAGS_COHERENCY_WRITE_THROUGH                       (0x00000003)
#define NVOS02_FLAGS_COHERENCY_WRITE_PROTECT                       (0x00000004)
#define NVOS02_FLAGS_COHERENCY_WRITE_BACK                          (0x00000005)
#define NVOS02_FLAGS_ALLOC                                         17:16
#define NVOS02_FLAGS_ALLOC_NONE                                    (0x00000001)
#define NVOS02_FLAGS_GPU_CACHEABLE                                 18:18
#define NVOS02_FLAGS_GPU_CACHEABLE_NO                              (0x00000000)
#define NVOS02_FLAGS_GPU_CACHEABLE_YES                             (0x00000001)
// If requested, RM will create a kernel mapping of this memory.
// Default is no map.
#define NVOS02_FLAGS_KERNEL_MAPPING                                19:19
#define NVOS02_FLAGS_KERNEL_MAPPING_NO_MAP                         (0x00000000)
#define NVOS02_FLAGS_KERNEL_MAPPING_MAP                            (0x00000001)
#define NVOS02_FLAGS_ALLOC_NISO_DISPLAY                            20:20
#define NVOS02_FLAGS_ALLOC_NISO_DISPLAY_NO                         (0x00000000)
#define NVOS02_FLAGS_ALLOC_NISO_DISPLAY_YES                        (0x00000001)

//
// If the flag is set, the RM will only allow read-only CPU user mappings to the
// allocation.
//
#define NVOS02_FLAGS_ALLOC_USER_READ_ONLY                          21:21
#define NVOS02_FLAGS_ALLOC_USER_READ_ONLY_NO                       (0x00000000)
#define NVOS02_FLAGS_ALLOC_USER_READ_ONLY_YES                      (0x00000001)

//
// If the flag is set, the RM will only allow read-only DMA mappings to the
// allocation.
//
#define NVOS02_FLAGS_ALLOC_DEVICE_READ_ONLY                        22:22
#define NVOS02_FLAGS_ALLOC_DEVICE_READ_ONLY_NO                     (0x00000000)
#define NVOS02_FLAGS_ALLOC_DEVICE_READ_ONLY_YES                    (0x00000001)

//
// If the flag is set, the IO memory allocation can be registered with the RM if
// the RM regkey peerMappingOverride is set or the client is privileged.
//
// See Bug 1630288 "[PeerSync] threat related to GPU.." for more details.
//
#define NVOS02_FLAGS_PEER_MAP_OVERRIDE                             23:23
#define NVOS02_FLAGS_PEER_MAP_OVERRIDE_DEFAULT                     (0x00000000)
#define NVOS02_FLAGS_PEER_MAP_OVERRIDE_REQUIRED                    (0x00000001)

// If the flag is set RM will assume the memory pages are of type syncpoint.
#define NVOS02_FLAGS_ALLOC_TYPE_SYNCPOINT                          24:24
#define NVOS02_FLAGS_ALLOC_TYPE_SYNCPOINT_APERTURE                 (0x00000001)

//
// Allow client allocations to go to protected/unprotected video/system memory.
// When Ampere Protected Model aka APM or Confidential Compute is enabled and
// DEFAULT flag is set by client, allocations go to protected memory. When
// protected memory is not enabled, allocations go to unprotected memory.
// If APM or CC is not enabled, it is a bug for a client to set the PROTECTED
// flag to YES
//
#define NVOS02_FLAGS_MEMORY_PROTECTION                             26:25
#define NVOS02_FLAGS_MEMORY_PROTECTION_DEFAULT                     (0x00000000)
#define NVOS02_FLAGS_MEMORY_PROTECTION_PROTECTED                   (0x00000001)
#define NVOS02_FLAGS_MEMORY_PROTECTION_UNPROTECTED                 (0x00000002)

//
// When allocating memory, register the memory descriptor to GSP-RM
// so that GSP-RM is aware of and can access it
//
#define NVOS02_FLAGS_REGISTER_MEMDESC_TO_PHYS_RM                   27:27
#define NVOS02_FLAGS_REGISTER_MEMDESC_TO_PHYS_RM_FALSE             (0x00000000)
#define NVOS02_FLAGS_REGISTER_MEMDESC_TO_PHYS_RM_TRUE              (0x00000001)

//
// If _NO_MAP is requested, the RM in supported platforms will not map the
// allocated system or IO memory into user space. The client can later map
// memory through the RmMapMemory() interface.
// If _NEVER_MAP is requested, the RM will never map the allocated system or
// IO memory into user space
//
#define NVOS02_FLAGS_MAPPING                                       31:30
#define NVOS02_FLAGS_MAPPING_DEFAULT                               (0x00000000)
#define NVOS02_FLAGS_MAPPING_NO_MAP                                (0x00000001)
#define NVOS02_FLAGS_MAPPING_NEVER_MAP                             (0x00000002)

// -------------------------------------------------------------------------------------

/* parameters */
typedef struct
{
    NvHandle    hRoot;
    NvHandle    hObjectParent;
    NvHandle    hObjectNew;
    NvV32       hClass;
    NvV32       flags;
    NvP64       pMemory NV_ALIGN_BYTES(8);
    NvU64       limit NV_ALIGN_BYTES(8);
    NvV32       status;
} NVOS02_PARAMETERS;

/* parameter values */
#define NVOS03_FLAGS_ACCESS                                        1:0
#define NVOS03_FLAGS_ACCESS_READ_WRITE                             (0x00000000)
#define NVOS03_FLAGS_ACCESS_READ_ONLY                              (0x00000001)
#define NVOS03_FLAGS_ACCESS_WRITE_ONLY                             (0x00000002)

#define NVOS03_FLAGS_PREALLOCATE                                   2:2
#define NVOS03_FLAGS_PREALLOCATE_DISABLE                           (0x00000000)
#define NVOS03_FLAGS_PREALLOCATE_ENABLE                            (0x00000001)

#define NVOS03_FLAGS_GPU_MAPPABLE                                  15:15
#define NVOS03_FLAGS_GPU_MAPPABLE_DISABLE                          (0x00000000)
#define NVOS03_FLAGS_GPU_MAPPABLE_ENABLE                           (0x00000001)

// ------------------------------------------------------------------------------------
// This flag is required for a hack to be placed inside DD that allows it to
// access a dummy ctxdma as a block linear surface. Refer bug 1562766 for details.
//
// This flag is deprecated, use NVOS03_FLAGS_PTE_KIND.
//
#define NVOS03_FLAGS_PTE_KIND_BL_OVERRIDE                          16:16
#define NVOS03_FLAGS_PTE_KIND_BL_OVERRIDE_FALSE                    (0x00000000)
#define NVOS03_FLAGS_PTE_KIND_BL_OVERRIDE_TRUE                     (0x00000001)

/*
 * This field allows to specify the page kind. If the page kind
 * is not specified then the page kind associated with the memory will be used.
 *
 * In tegra display driver stack, the page kind remains unknown at the time
 * of memory allocation/import, the page kind can only be known when display
 * driver client creates a framebuffer from allocated/imported memory.
 *
 * This field compatible with NVOS03_FLAGS_PTE_KIND_BL_OVERRIDE flag.
 */
#define NVOS03_FLAGS_PTE_KIND                                      17:16
#define NVOS03_FLAGS_PTE_KIND_NONE                                 (0x00000000)
#define NVOS03_FLAGS_PTE_KIND_BL                                   (0x00000001)
#define NVOS03_FLAGS_PTE_KIND_PITCH                                (0x00000002)

#define NVOS03_FLAGS_TYPE                                          23:20
#define NVOS03_FLAGS_TYPE_NOTIFIER                                 (0x00000001)

/*
 * This is an alias into the LSB of the TYPE field which
 * actually indicates if a Kernel Mapping should be created.
 * If the RM should have access to the memory then Enable this
 * flag.
 *
 * Note that the NV_OS03_FLAGS_MAPPING is an alias to
 * the LSB of the NV_OS03_FLAGS_TYPE. And in fact if
 * type is NV_OS03_FLAGS_TYPE_NOTIFIER (bit 20 set)
 * then it implicitly means that NV_OS03_FLAGS_MAPPING
 * is _MAPPING_KERNEL. If the client wants to have a
 * Kernel Mapping, it should use the _MAPPING_KERNEL
 * flag set and the _TYPE_NOTIFIER should be used only
 * with NOTIFIERS.
 */

#define NVOS03_FLAGS_MAPPING                                       20:20
#define NVOS03_FLAGS_MAPPING_NONE                                  (0x00000000)
#define NVOS03_FLAGS_MAPPING_KERNEL                                (0x00000001)

#define NVOS03_FLAGS_CACHE_SNOOP                                   28:28
#define NVOS03_FLAGS_CACHE_SNOOP_ENABLE                            (0x00000000)
#define NVOS03_FLAGS_CACHE_SNOOP_DISABLE                           (0x00000001)

// HASH_TABLE:ENABLE means that the context DMA is automatically bound into all
// channels in the client.  This can lead to excessive hash table usage.
// HASH_TABLE:DISABLE means that the context DMA must be explicitly bound into
// any channel that needs to use it via NvRmBindContextDma.
// HASH_TABLE:ENABLE is not supported on NV50 and up, and HASH_TABLE:DISABLE should
// be preferred for all new code.
#define NVOS03_FLAGS_HASH_TABLE                                    29:29
#define NVOS03_FLAGS_HASH_TABLE_ENABLE                             (0x00000000)
#define NVOS03_FLAGS_HASH_TABLE_DISABLE                            (0x00000001)

/* macro NV01_ALLOC_OBJECT */
#define  NV01_ALLOC_OBJECT                                         (0x00000005)

/* parameters */
typedef struct
{
    NvHandle hRoot;
    NvHandle hObjectParent;
    NvHandle hObjectNew;
    NvV32    hClass;
    NvV32    status;
} NVOS05_PARAMETERS;

/* Valid values for hClass in Nv01AllocEvent */
/* Note that NV01_EVENT_OS_EVENT is same as NV01_EVENT_WIN32_EVENT */
/* TODO: delete the WIN32 name */
#define  NV01_EVENT_KERNEL_CALLBACK                                (0x00000078)
#define  NV01_EVENT_OS_EVENT                                       (0x00000079)
#define  NV01_EVENT_WIN32_EVENT                             NV01_EVENT_OS_EVENT
#define  NV01_EVENT_KERNEL_CALLBACK_EX                             (0x0000007E)

/* NOTE: NV01_EVENT_KERNEL_CALLBACK is deprecated. Please use NV01_EVENT_KERNEL_CALLBACK_EX. */
/* For use with NV01_EVENT_KERNEL_CALLBACK. */
/* NVOS10_EVENT_KERNEL_CALLBACK data structure storage needs to be retained by the caller. */
typedef void (*Callback1ArgVoidReturn)(void *arg);
typedef void (*Callback5ArgVoidReturn)(void *arg1, void *arg2, NvHandle hEvent, NvU32 data, NvU32 status);

/* NOTE: the 'void* arg' below is ok (but unfortunate) since this interface
   can only be used by other kernel drivers which must share the same ptr-size */
typedef struct
{
    Callback1ArgVoidReturn  func;
    void                   *arg;
} NVOS10_EVENT_KERNEL_CALLBACK;

/* For use with NV01_EVENT_KERNEL_CALLBACK_EX. */
/* NVOS10_EVENT_KERNEL_CALLBACK_EX data structure storage needs to be retained by the caller. */
/* NOTE: the 'void* arg' below is ok (but unfortunate) since this interface
   can only be used by other kernel drivers which must share the same ptr-size */
typedef struct
{
    Callback5ArgVoidReturn  func;
    void                   *arg;
} NVOS10_EVENT_KERNEL_CALLBACK_EX;

/* Setting this bit in index will set the Event to a Broadcast type */
/* i.e. each subdevice under a device needs to see the Event before it's signaled */
#define NV01_EVENT_BROADCAST                                       (0x80000000)

/* allow non-root resman client to create NV01_EVENT_KERNEL_CALLBACK events */
/* -- this works in debug/develop drivers only (for security reasons)*/
#define NV01_EVENT_PERMIT_NON_ROOT_EVENT_KERNEL_CALLBACK_CREATION  (0x40000000)

/* RM event should be triggered only by the specified subdevice; see cl0005.h
 * for details re: how to specify subdevice. */
#define NV01_EVENT_SUBDEVICE_SPECIFIC                              (0x20000000)

/* RM should trigger the event but shouldn't do the book-keeping of data
 * associated with that event */
#define NV01_EVENT_WITHOUT_EVENT_DATA                              (0x10000000)

/* RM event should be triggered only by the non-stall interrupt */
#define NV01_EVENT_NONSTALL_INTR                                   (0x08000000)

/* RM event was allocated from client RM, post events back to client RM */
#define NV01_EVENT_CLIENT_RM                                       (0x04000000)

/* function OS19 */
#define  NV04_I2C_ACCESS                                           (0x00000013)

#define NVOS_I2C_ACCESS_MAX_BUFFER_SIZE  2048

/* parameters */
typedef struct
{
    NvHandle hClient;
    NvHandle hDevice;
    NvU32    paramSize;
    NvP64    paramStructPtr NV_ALIGN_BYTES(8);
    NvV32    status;
} NVOS_I2C_ACCESS_PARAMS;

/* current values for command */
#define NVOS20_COMMAND_unused0001                  0x0001
#define NVOS20_COMMAND_unused0002                  0x0002
#define NVOS20_COMMAND_STRING_PRINT                0x0003

/* function OS21 */
#define  NV04_ALLOC                                                (0x00000015)

/* parameters */
typedef struct
{
    NvHandle hRoot;
    NvHandle hObjectParent;
    NvHandle hObjectNew;
    NvV32    hClass;
    NvP64    pAllocParms NV_ALIGN_BYTES(8);
    NvU32    paramsSize;
    NvV32    status;
} NVOS21_PARAMETERS;

#define NVOS64_FLAGS_NONE             (0x00000000)
#define NVOS64_FLAGS_FINN_SERIALIZED  (0x00000001)

/* New struct with rights requested */
typedef struct
{
    NvHandle hRoot;                               // [IN] client handle
    NvHandle hObjectParent;                       // [IN] parent handle of new object
    NvHandle hObjectNew;                          // [INOUT] new object handle, 0 to generate
    NvV32    hClass;                              // [in] class num of new object
    NvP64    pAllocParms NV_ALIGN_BYTES(8);       // [IN] class-specific alloc parameters
    NvP64    pRightsRequested NV_ALIGN_BYTES(8);  // [IN] RS_ACCESS_MASK to request rights, or NULL
    NvU32    paramsSize;                          // [IN] Size of alloc params
    NvU32    flags;                               // [IN] flags for FINN serialization
    NvV32    status;                              // [OUT] status
} NVOS64_PARAMETERS;

/* RM Alloc header
 *
 * Replacement for NVOS21/64_PARAMETERS where embedded pointers are not allowed.
 * Input layout for RM Alloc user space calls should be
 *
 * +--- NVOS62_PARAMETERS ---+--- RM Alloc parameters ---+
 * +--- NVOS65_PARAMETERS ---+--- Rights Requested ---+--- RM Alloc parameters ---+
 *
 * NVOS62_PARAMETERS::paramsSize is the size of RM Alloc parameters
 * If NVOS65_PARAMETERS::maskSize is 0, Rights Requested will not be present in memory.
 *
 */
typedef struct
{
    NvHandle hRoot;             // [IN]  client handle
    NvHandle hObjectParent;     // [IN]  parent handle of the new object
    NvHandle hObjectNew;        // [IN]  new object handle
    NvV32    hClass;            // [IN]  class num of the new object
    NvU32    paramSize;         // [IN]  size in bytes of the RM alloc parameters
    NvV32    status;            // [OUT] status
} NVOS62_PARAMETERS;

#define NVOS65_PARAMETERS_VERSION_MAGIC 0x77FEF81E

typedef struct
{
    NvHandle hRoot;             // [IN]  client handle
    NvHandle hObjectParent;     // [IN]  parent handle of the new object
    NvHandle hObjectNew;        // [INOUT]  new object handle, 0 to generate
    NvV32    hClass;            // [IN]  class num of the new object
    NvU32    paramSize;         // [IN]  size in bytes of the RM alloc parameters
    NvU32    versionMagic;      // [IN]  NVOS65_PARAMETERS_VERISON_MAGIC
    NvU32    maskSize;          // [IN]  size in bytes of access mask, or 0 if NULL
    NvV32    status;            // [OUT] status
} NVOS65_PARAMETERS;

/* function OS30 */
#define NV04_IDLE_CHANNELS                                         (0x0000001E)

/* parameter values */
#define NVOS30_FLAGS_BEHAVIOR                                      3:0
#define NVOS30_FLAGS_BEHAVIOR_SPIN                                 (0x00000000)
#define NVOS30_FLAGS_BEHAVIOR_SLEEP                                (0x00000001)
#define NVOS30_FLAGS_BEHAVIOR_QUERY                                (0x00000002)
#define NVOS30_FLAGS_BEHAVIOR_FORCE_BUSY_CHECK                     (0x00000003)
#define NVOS30_FLAGS_CHANNEL                                       7:4
#define NVOS30_FLAGS_CHANNEL_LIST                                  (0x00000000)
#define NVOS30_FLAGS_CHANNEL_SINGLE                                (0x00000001)
#define NVOS30_FLAGS_IDLE                                          30:8
#define NVOS30_FLAGS_IDLE_PUSH_BUFFER                              (0x00000001)
#define NVOS30_FLAGS_IDLE_CACHE1                                   (0x00000002)
#define NVOS30_FLAGS_IDLE_GRAPHICS                                 (0x00000004)
#define NVOS30_FLAGS_IDLE_MPEG                                     (0x00000008)
#define NVOS30_FLAGS_IDLE_MOTION_ESTIMATION                        (0x00000010)
#define NVOS30_FLAGS_IDLE_VIDEO_PROCESSOR                          (0x00000020)
#define NVOS30_FLAGS_IDLE_MSPDEC                                   (0x00000020)
#define NVOS30_FLAGS_IDLE_BITSTREAM_PROCESSOR                      (0x00000040)
#define NVOS30_FLAGS_IDLE_MSVLD                                    (0x00000040)
#define NVOS30_FLAGS_IDLE_NVDEC0                                   NVOS30_FLAGS_IDLE_MSVLD
#define NVOS30_FLAGS_IDLE_CIPHER_DMA                               (0x00000080)
#define NVOS30_FLAGS_IDLE_SEC                                      (0x00000080)
#define NVOS30_FLAGS_IDLE_CALLBACKS                                (0x00000100)
#define NVOS30_FLAGS_IDLE_MSPPP                                    (0x00000200)
#define NVOS30_FLAGS_IDLE_CE0                                      (0x00000400)
#define NVOS30_FLAGS_IDLE_CE1                                      (0x00000800)
#define NVOS30_FLAGS_IDLE_CE2                                      (0x00001000)
#define NVOS30_FLAGS_IDLE_CE3                                      (0x00002000)
#define NVOS30_FLAGS_IDLE_CE4                                      (0x00004000)
#define NVOS30_FLAGS_IDLE_CE5                                      (0x00008000)
#define NVOS30_FLAGS_IDLE_VIC                                      (0x00010000)
#define NVOS30_FLAGS_IDLE_MSENC                                    (0x00020000)
#define NVOS30_FLAGS_IDLE_NVENC0                                    NVOS30_FLAGS_IDLE_MSENC
#define NVOS30_FLAGS_IDLE_NVENC1                                   (0x00040000)
#define NVOS30_FLAGS_IDLE_NVENC2                                   (0x00080000)
#define NVOS30_FLAGS_IDLE_NVJPG                                    (0x00100000)
#define NVOS30_FLAGS_IDLE_NVDEC1                                   (0x00200000)
#define NVOS30_FLAGS_IDLE_NVDEC2                                   (0x00400000)
#define NVOS30_FLAGS_IDLE_ACTIVECHANNELS                           (0x00800000)
#define NVOS30_FLAGS_IDLE_ALL_ENGINES (NVOS30_FLAGS_IDLE_GRAPHICS | \
                                       NVOS30_FLAGS_IDLE_MPEG | \
                                       NVOS30_FLAGS_IDLE_MOTION_ESTIMATION | \
                                       NVOS30_FLAGS_IDLE_VIDEO_PROCESSOR | \
                                       NVOS30_FLAGS_IDLE_BITSTREAM_PROCESSOR |  \
                                       NVOS30_FLAGS_IDLE_CIPHER_DMA  | \
                                       NVOS30_FLAGS_IDLE_MSPDEC      | \
                                       NVOS30_FLAGS_IDLE_NVDEC0      | \
                                       NVOS30_FLAGS_IDLE_SEC         | \
                                       NVOS30_FLAGS_IDLE_MSPPP       | \
                                       NVOS30_FLAGS_IDLE_CE0         | \
                                       NVOS30_FLAGS_IDLE_CE1         | \
                                       NVOS30_FLAGS_IDLE_CE2         | \
                                       NVOS30_FLAGS_IDLE_CE3         | \
                                       NVOS30_FLAGS_IDLE_CE4         | \
                                       NVOS30_FLAGS_IDLE_CE5         | \
                                       NVOS30_FLAGS_IDLE_NVENC0      | \
                                       NVOS30_FLAGS_IDLE_NVENC1      | \
                                       NVOS30_FLAGS_IDLE_NVENC2      | \
                                       NVOS30_FLAGS_IDLE_VIC         | \
                                       NVOS30_FLAGS_IDLE_NVJPG       | \
                                       NVOS30_FLAGS_IDLE_NVDEC1      | \
                                       NVOS30_FLAGS_IDLE_NVDEC2)
#define NVOS30_FLAGS_WAIT_FOR_ELPG_ON                              31:31
#define NVOS30_FLAGS_WAIT_FOR_ELPG_ON_NO                           (0x00000000)
#define NVOS30_FLAGS_WAIT_FOR_ELPG_ON_YES                          (0x00000001)

/* parameters */
typedef struct
{
    NvHandle hClient;
    NvHandle hDevice;
    NvHandle hChannel;
    NvV32    numChannels;

    NvP64    phClients NV_ALIGN_BYTES(8);
    NvP64    phDevices NV_ALIGN_BYTES(8);
    NvP64    phChannels NV_ALIGN_BYTES(8);

    NvV32    flags;
    NvV32    timeout;
    NvV32    status;
} NVOS30_PARAMETERS;

/* function OS32 */
typedef void (*BindResultFunc)(void * pVoid, NvU32 gpuMask, NvU32 bState, NvU32 bResult);

#define NV04_VID_HEAP_CONTROL                                      (0x00000020)
/*************************************************************************
************************ New Heap Interface ******************************
*************************************************************************/
// NVOS32 Descriptor types
//
// NVOS32_DESCRIPTOR_TYPE_OS_DMA_BUF_PTR: The dma-buf object
// pointer, provided by the linux kernel buffer sharing sub-system.
// This descriptor can only be used by kernel space rm-clients.
//
#define NVOS32_DESCRIPTOR_TYPE_VIRTUAL_ADDRESS          0
#define NVOS32_DESCRIPTOR_TYPE_OS_PAGE_ARRAY            1
#define NVOS32_DESCRIPTOR_TYPE_OS_IO_MEMORY             2
#define NVOS32_DESCRIPTOR_TYPE_OS_PHYS_ADDR             3
#define NVOS32_DESCRIPTOR_TYPE_OS_FILE_HANDLE           4
#define NVOS32_DESCRIPTOR_TYPE_OS_DMA_BUF_PTR           5
#define NVOS32_DESCRIPTOR_TYPE_OS_SGT_PTR               6
#define NVOS32_DESCRIPTOR_TYPE_KERNEL_VIRTUAL_ADDRESS   7
// NVOS32 function
#define NVOS32_FUNCTION_ALLOC_SIZE                      2
#define NVOS32_FUNCTION_FREE                            3
// #define NVOS32_FUNCTION_HEAP_PURGE                   4
#define NVOS32_FUNCTION_INFO                            5
#define NVOS32_FUNCTION_ALLOC_TILED_PITCH_HEIGHT        6
// #define NVOS32_FUNCTION_DESTROY                      7
// #define NVOS32_FUNCTION_RETAIN                       9
// #define NVOS32_FUNCTION_REALLOC                      10
#define NVOS32_FUNCTION_DUMP                            11
// #define NVOS32_FUNCTION_INFO_TYPE_ALLOC_BLOCKS       12
#define NVOS32_FUNCTION_ALLOC_SIZE_RANGE                14
#define NVOS32_FUNCTION_REACQUIRE_COMPR                 15
#define NVOS32_FUNCTION_RELEASE_COMPR                   16
// #define NVOS32_FUNCTION_MODIFY_DEFERRED_TILES        17
#define NVOS32_FUNCTION_GET_MEM_ALIGNMENT               18
#define NVOS32_FUNCTION_HW_ALLOC                        19
#define NVOS32_FUNCTION_HW_FREE                         20
// #define NVOS32_FUNCTION_SET_OFFSET                   21
// #define NVOS32_FUNCTION_IS_TILED                     22
// #define NVOS32_FUNCTION_ENABLE_RESOURCE              23
// #define NVOS32_FUNCTION_BIND_COMPR                   24
#define NVOS32_FUNCTION_ALLOC_OS_DESCRIPTOR             27

typedef struct
{
    NvP64 sgt NV_ALIGN_BYTES(8);
    NvP64 gem NV_ALIGN_BYTES(8);
} NVOS32_DESCRIPTOR_TYPE_OS_SGT_PTR_PARAMETERS;

#define NVOS32_FLAGS_BLOCKINFO_VISIBILITY_CPU (0x00000001)
typedef struct
{
    NvU64 startOffset NV_ALIGN_BYTES(8);
    NvU64 size NV_ALIGN_BYTES(8);
    NvU32 flags;
} NVOS32_BLOCKINFO;

// NVOS32 IVC-heap number delimiting value
#define NVOS32_IVC_HEAP_NUMBER_DONT_ALLOCATE_ON_IVC_HEAP 0 // When IVC heaps are present,
                                                           // IVC-heap number specified
                                                           // as part of 'NVOS32_PARAMETERS'
                                                           // which is less or equal to this
                                                           // constant indicates that allocation
                                                           // should not be done on IVC heap.
                                                           // Explanation of IVC-heap number is
                                                           // under 'AllocSize' structure below.

typedef struct
{
  NvHandle  hRoot;                      // [IN]  - root object handle
  NvHandle  hObjectParent;              // [IN]  - device handle
  NvU32     function;                   // [IN]  - heap function, see below FUNCTION* defines
  NvHandle  hVASpace;                   // [IN]  - VASpace handle
  NvS16     ivcHeapNumber;              // [IN] - When IVC heaps are present: either 1) number of the IVC heap
                                        //        shared between two VMs or 2) number indicating that allocation
                                        //        should not be done on an IVC heap. Values greater than constant
                                        //        'NVOS32_IVC_HEAP_NUMBER_DONT_ALLOCATE_ON_IVC_HEAP' define set 1)
                                        //        and values less or equal to that constant define set 2).
                                        //        When IVC heaps are present, correct IVC-heap number must be specified.
                                        //        When IVC heaps are absent, IVC-heap number is diregarded.
                                        //        RM provides for each VM a bitmask of heaps with each bit
                                        //        specifying the other peer that can use the partition.
                                        //        Each bit set to one can be enumerated, such that the bit
                                        //        with lowest significance is enumerated with one.
                                        //        'ivcHeapNumber' parameter specifies this enumeration value.
                                        //        This value is used to uniquely identify a heap shared between
                                        //        two particular VMs.
                                        //        Illustration:
                                        //                                bitmask: 1  1  0  1  0 = 0x1A
                                        //        possible 'ivcHeapNumber' values: 3, 2,    1
  NvV32     status;                     // [OUT] - returned NVOS32* status code, see below STATUS* defines
  NvU64     total NV_ALIGN_BYTES(8);    // [OUT] - returned total size of heap
  NvU64     free  NV_ALIGN_BYTES(8);    // [OUT] - returned free space available in heap

  union
  {
      // NVOS32_FUNCTION_ALLOC_SIZE
      struct
      {
          NvU32     owner;              // [IN]  - memory owner ID
          NvHandle  hMemory;            // [IN/OUT]  - unique memory handle - IN only if MEMORY_HANDLE_PROVIDED is set (otherwise generated)
          NvU32     type;               // [IN]  - surface type, see below TYPE* defines
          NvU32     flags;              // [IN]  - allocation modifier flags, see below ALLOC_FLAGS* defines
          NvU32     attr;               // [IN/OUT] - surface attributes requested, and surface attributes allocated
          NvU32     format;             // [IN/OUT] - format requested, and format allocated
          NvU32     comprCovg;          // [IN/OUT] - compr covg requested, and allocated
          NvU32     zcullCovg;          // [OUT] - zcull covg allocated
          NvU32     partitionStride;    // [IN/OUT] - 0 means "RM" chooses
          NvU32     width;              // [IN] - width "hint" used for zcull region allocations
          NvU32     height;             // [IN] - height "hint" used for zcull region allocations
          NvU64     size      NV_ALIGN_BYTES(8); // [IN/OUT]  - size of allocation - also returns the actual size allocated
          NvU64     alignment NV_ALIGN_BYTES(8); // [IN]  - requested alignment - NVOS32_ALLOC_FLAGS_ALIGNMENT* must be on
          NvU64     offset    NV_ALIGN_BYTES(8); // [IN/OUT]  - desired offset if NVOS32_ALLOC_FLAGS_FIXED_ADDRESS_ALLOCATE is on AND returned offset
          NvU64     limit     NV_ALIGN_BYTES(8); // [OUT] - returned surface limit
          NvP64     address NV_ALIGN_BYTES(8);// [OUT] - returned address
          NvU64     rangeBegin NV_ALIGN_BYTES(8); // [IN]  - allocated memory will be limited to the range
          NvU64     rangeEnd   NV_ALIGN_BYTES(8); // [IN]  - from rangeBegin to rangeEnd, inclusive.
          NvU32     attr2;              // [IN/OUT] - surface attributes requested, and surface attributes allocated
          NvU32     ctagOffset;         // [IN] - comptag offset for this surface (see NVOS32_ALLOC_COMPTAG_OFFSET)
          NvS32     numaNode;           // [IN] - NUMA node from which memory should be allocated
      } AllocSize;

      // NVOS32_FUNCTION_ALLOC_TILED_PITCH_HEIGHT
      struct
      {
          NvU32     owner;              // [IN]  - memory owner ID
          NvHandle  hMemory;            // [IN/OUT]  - unique memory handle - IN only if MEMORY_HANDLE_PROVIDED is set (otherwise generated)
          NvU32     type;               // [IN]  - surface type, see below TYPE* defines
          NvU32     flags;              // [IN]  - allocation modifier flags, see below ALLOC_FLAGS* defines
          NvU32     height;             // [IN]  - height of surface in pixels
          NvS32     pitch;              // [IN/OUT] - desired pitch AND returned actual pitch allocated
          NvU32     attr;               // [IN/OUT] - surface attributes requested, and surface attributes allocated
          NvU32     width;               //[IN] - width of surface in pixels
          NvU32     format;             // [IN/OUT] - format requested, and format allocated
          NvU32     comprCovg;          // [IN/OUT] - compr covg requested, and allocated
          NvU32     zcullCovg;          // [OUT] - zcull covg allocated
          NvU32     partitionStride;    // [IN/OUT] - 0 means "RM" chooses
          NvU64     size      NV_ALIGN_BYTES(8); // [IN/OUT]  - size of allocation - also returns the actual size allocated
          NvU64     alignment NV_ALIGN_BYTES(8); // [IN]  - requested alignment - NVOS32_ALLOC_FLAGS_ALIGNMENT* must be on
          NvU64     offset    NV_ALIGN_BYTES(8); // [IN/OUT]  - desired offset if NVOS32_ALLOC_FLAGS_FIXED_ADDRESS_ALLOCATE is on AND returned offset
          NvU64     limit     NV_ALIGN_BYTES(8); // [OUT] - returned surface limit
          NvP64     address NV_ALIGN_BYTES(8);// [OUT] - returned address
          NvU64     rangeBegin NV_ALIGN_BYTES(8); // [IN]  - allocated memory will be limited to the range
          NvU64     rangeEnd   NV_ALIGN_BYTES(8); // [IN]  - from rangeBegin to rangeEnd, inclusive.
          NvU32     attr2;              // [IN/OUT] - surface attributes requested, and surface attributes allocated
          NvU32     ctagOffset;         // [IN] - comptag offset for this surface (see NVOS32_ALLOC_COMPTAG_OFFSET)
          NvS32     numaNode;           // [IN] - NUMA node from which memory should be allocated
      } AllocTiledPitchHeight;

      // NVOS32_FUNCTION_FREE
      struct
      {
          NvU32     owner;              // [IN]  - memory owner ID
          NvHandle  hMemory;            // [IN]  - unique memory handle
          NvU32     flags;              // [IN]  - heap free flags (must be NVOS32_FREE_FLAGS_MEMORY_HANDLE_PROVIDED)
      } Free;

      // NVOS32_FUNCTION_RELEASE_COMPR
      struct
      {
          NvU32     owner;           // [IN]  - memory owner ID
          NvU32     flags;           // [IN]  - must be NVOS32_RELEASE_COMPR_FLAGS_MEMORY_HANDLE_PROVIDED
          NvHandle  hMemory;         // [IN]  - unique memory handle (valid if _RELEASE_COMPR_FLAGS_MEMORY_HANDLE_PROVIDED
      } ReleaseCompr;

      // NVOS32_FUNCTION_REACQUIRE_COMPR
      struct
      {
          NvU32     owner;           // [IN]  - memory owner ID
          NvU32     flags;           // [IN]  - must be NVOS32_REACQUIRE_COMPR_FLAGS_MEMORY_HANDLE_PROVIDED
          NvHandle  hMemory;         // [IN]  - unique memory handle (valid if _REACQUIRE_COMPR_FLAGS_MEMORY_HANDLE_PROVIDED
      } ReacquireCompr;

      // NVOS32_FUNCTION_INFO
      struct
      {
          NvU32 attr;               // [IN] - memory heap attributes requested
          NvU64 offset NV_ALIGN_BYTES(8); // [OUT] - base of largest free block
          NvU64 size   NV_ALIGN_BYTES(8); // [OUT] - size of largest free block
          NvU64 base   NV_ALIGN_BYTES(8);   // [OUT] - returned heap phys base
      } Info;

      // NVOS32_FUNCTION_DUMP
      struct
      {
          NvU32 flags;              // [IN] - see _DUMP_FLAGS
          // [IN]  - if NULL, numBlocks is the returned number of blocks in
          //         heap, else returns all blocks in eHeap
          //         if non-NULL points to a buffer that is at least numBlocks
          //         * sizeof(NVOS32_HEAP_DUMP_BLOCK) bytes.
          NvP64 pBuffer NV_ALIGN_BYTES(8);
          // [IN/OUT] - if pBuffer is NULL, will number of blocks in heap
          //            if pBuffer is non-NULL, is input containing the size of
          //            pBuffer in units of NVOS32_HEAP_DUMP_BLOCK.  This must
          //            be greater than or equal to the number of blocks in the
          //            heap.
          NvU32 numBlocks;
      } Dump;

      // NVOS32_FUNCTION_DESTROY - no extra parameters needed

      // NVOS32_FUNCTION_ALLOC_SIZE_RANGE
      struct
      {
          NvU32     owner;              // [IN]  - memory owner ID
          NvHandle  hMemory;            // [IN]  - unique memory handle
          NvU32     type;               // [IN]  - surface type, see below TYPE* defines
          NvU32     flags;              // [IN]  - allocation modifier flags, see below ALLOC_FLAGS* defines
          NvU32     attr;               // [IN/OUT] - surface attributes requested, and surface attributes allocated
          NvU32     format;             // [IN/OUT] - format requested, and format allocated
          NvU32     comprCovg;          // [IN/OUT] - compr covg requested, and allocated
          NvU32     zcullCovg;          // [OUT] - zcull covg allocated
          NvU32     partitionStride;    // [IN/OUT] - 0 means "RM" chooses
          NvU64     size      NV_ALIGN_BYTES(8);  // [IN/OUT]  - size of allocation - also returns the actual size allocated
          NvU64     alignment NV_ALIGN_BYTES(8);  // [IN]  - requested alignment - NVOS32_ALLOC_FLAGS_ALIGNMENT* must be on
          NvU64     offset    NV_ALIGN_BYTES(8);  // [IN/OUT]  - desired offset if NVOS32_ALLOC_FLAGS_FIXED_ADDRESS_ALLOCATE is on AND returned offset
          NvU64     limit     NV_ALIGN_BYTES(8);  // [OUT] - returned surface limit
          NvU64     rangeBegin NV_ALIGN_BYTES(8); // [IN]  - allocated memory will be limited to the range
          NvU64     rangeEnd   NV_ALIGN_BYTES(8); // [IN]  - from rangeBegin to rangeEnd, inclusive.
          NvP64     address NV_ALIGN_BYTES(8);// [OUT] - returned address
          NvU32     attr2;              // [IN/OUT] - surface attributes requested, and surface attributes allocated
          NvU32     ctagOffset;         // [IN] - comptag offset for this surface (see NVOS32_ALLOC_COMPTAG_OFFSET)
          NvS32     numaNode;           // [IN] - NUMA node from which memory should be allocated
      } AllocSizeRange;

      // additions for Longhorn
#define NVAL_MAX_BANKS (4)
#define NVAL_MAP_DIRECTION             0:0
#define NVAL_MAP_DIRECTION_DOWN 0x00000000
#define NVAL_MAP_DIRECTION_UP   0x00000001

      // NVOS32_FUNCTION_GET_MEM_ALIGNMENT
      struct
      {
          NvU32 alignType;                                 // Input
          NvU32 alignAttr;
          NvU32 alignInputFlags;
          NvU64 alignSize NV_ALIGN_BYTES(8);
          NvU32 alignHeight;
          NvU32 alignWidth;
          NvU32 alignPitch;
          NvU32 alignPad;
          NvU32 alignMask;
          NvU32 alignOutputFlags[NVAL_MAX_BANKS];           // We could compress this information but it is probably not that big of a deal
          NvU32 alignBank[NVAL_MAX_BANKS];
          NvU32 alignKind;
          NvU32 alignAdjust;                                // Output -- If non-zero the amount we need to adjust the offset
          NvU32 alignAttr2;
      } AllocHintAlignment;

      struct
      {
          NvU32     allocOwner;              // [IN]  - memory owner ID
          NvHandle  allochMemory;            // [IN/OUT] - unique memory handle - IN only if MEMORY_HANDLE_PROVIDED is set (otherwise generated)
          NvU32     flags;
          NvU32     allocType;               // Input
          NvU32     allocAttr;
          NvU32     allocInputFlags;
          NvU64     allocSize       NV_ALIGN_BYTES(8);
          NvU32     allocHeight;
          NvU32     allocWidth;
          NvU32     allocPitch;
          NvU32     allocMask;
          NvU32     allocComprCovg;
          NvU32     allocZcullCovg;
          NvP64     bindResultFunc  NV_ALIGN_BYTES(8);         // BindResultFunc
          NvP64     pHandle         NV_ALIGN_BYTES(8);
          NvHandle  hResourceHandle;                          // Handle to RM container
          NvU32     retAttr;                                  // Output Indicates the resources that we allocated
          NvU32     kind;
          NvU64     osDeviceHandle  NV_ALIGN_BYTES(8);
          NvU32     allocAttr2;
          NvU32     retAttr2;                                 // Output Indicates the resources that we allocated
          NvU64     allocAddr       NV_ALIGN_BYTES(8);
          // [out] from GMMU_COMPR_INFO in drivers/common/shared/inc/mmu/gmmu_fmt.h
          struct
          {
              NvU32 compPageShift;
              NvU32 compressedKind;
              NvU32 compTagLineMin;
              NvU32 compPageIndexLo;
              NvU32 compPageIndexHi;
              NvU32 compTagLineMultiplier;
          } comprInfo;
          // [out] fallback uncompressed kind.
          NvU32  uncompressedKind;
      } HwAlloc;

      // NVOS32_FUNCTION_HW_FREE
      struct
      {
          NvHandle  hResourceHandle;                          // Handle to RM Resource Info
          NvU32     flags;                                    // Indicate if HW Resources and/or Memory
      } HwFree;
// Updated interface check.
#define NV_RM_OS32_ALLOC_OS_DESCRIPTOR_WITH_OS32_ATTR   1

      // NVOS32_FUNCTION_ALLOC_OS_DESCRIPTOR
      struct
      {
          NvHandle  hMemory;                      // [IN/OUT] - unique memory handle - IN only if MEMORY_HANDLE_PROVIDED is set (otherwise generated)
          NvU32     type;                         // [IN]  - surface type, see below TYPE* defines
          NvU32     flags;                        // [IN]  - allocation modifier flags, see below ALLOC_FLAGS* defines
          NvU32     attr;                         // [IN]  - attributes for memory placement/properties, see below
          NvU32     attr2;                        // [IN]  - attributes GPU_CACHEABLE
          NvP64     descriptor NV_ALIGN_BYTES(8); // [IN]  - descriptor address
          NvU64     limit      NV_ALIGN_BYTES(8); // [IN]  - allocated size -1
          NvU32     descriptorType;               // [IN]  - descriptor type(Virtual | nvmap Handle)
      } AllocOsDesc;

  } data;
} NVOS32_PARAMETERS;

typedef struct
{
    NvU32 owner;    // owner id - NVOS32_BLOCK_TYPE_FREE or defined by client during heap_alloc
    NvU32 format;   // arch specific format/kind
    NvU64 begin NV_ALIGN_BYTES(8); // start of allocated memory block
    NvU64 align NV_ALIGN_BYTES(8); // actual start of usable memory, aligned to chip specific boundary
    NvU64 end   NV_ALIGN_BYTES(8); // end of usable memory.  end - align + 1 = size of block
} NVOS32_HEAP_DUMP_BLOCK;


#define NVOS32_DELETE_RESOURCES_ALL                     0

// type field
#define NVOS32_TYPE_IMAGE                               0
#define NVOS32_TYPE_DEPTH                               1
#define NVOS32_TYPE_TEXTURE                             2
#define NVOS32_TYPE_VIDEO                               3
#define NVOS32_TYPE_FONT                                4
#define NVOS32_TYPE_CURSOR                              5
#define NVOS32_TYPE_DMA                                 6
#define NVOS32_TYPE_INSTANCE                            7
#define NVOS32_TYPE_PRIMARY                             8
#define NVOS32_TYPE_ZCULL                               9
#define NVOS32_TYPE_UNUSED                              10
#define NVOS32_TYPE_SHADER_PROGRAM                      11
#define NVOS32_TYPE_OWNER_RM                            12
#define NVOS32_TYPE_NOTIFIER                            13
#define NVOS32_TYPE_RESERVED                            14
#define NVOS32_TYPE_PMA                                 15
#define NVOS32_TYPE_STENCIL                             16
#define NVOS32_NUM_MEM_TYPES                            17

// Surface attribute field - bitmask of requested attributes the surface
// should have.
// This value is updated to reflect what was actually allocated, and so this
// field must be checked after every allocation to determine what was
// allocated. Pass in the ANY tags to indicate that RM should fall back but
// still succeed the alloc.
// for example, if tiled_any is passed in, but no tile ranges are available,
// RM will allocate normal memory and indicate that in the returned attr field.
// Each returned attribute will have the REQUIRED field set if that attribute
// applies to the allocated surface.

#define NVOS32_ATTR_NONE                                0x00000000

#define NVOS32_ATTR_DEPTH                                      2:0
#define NVOS32_ATTR_DEPTH_UNKNOWN                       0x00000000
#define NVOS32_ATTR_DEPTH_8                             0x00000001
#define NVOS32_ATTR_DEPTH_16                            0x00000002
#define NVOS32_ATTR_DEPTH_24                            0x00000003
#define NVOS32_ATTR_DEPTH_32                            0x00000004
#define NVOS32_ATTR_DEPTH_64                            0x00000005
#define NVOS32_ATTR_DEPTH_128                           0x00000006

#define NVOS32_ATTR_COMPR_COVG                                 3:3
#define NVOS32_ATTR_COMPR_COVG_DEFAULT                  0x00000000
#define NVOS32_ATTR_COMPR_COVG_PROVIDED                 0x00000001

// Surface description - number of AA samples
// This number should only reflect AA done in hardware, not in software. For
// example, OpenGL's 8x AA mode is a mix of 2x hardware multisample and 2x2
// software supersample.
// OpenGL should specify ATTR_AA_SAMPLES of 2 in this case, not 8, because
// the hardware will be programmed to run in 2x AA mode.
// Note that X_VIRTUAL_Y means X real samples with Y samples total (i.e. Y
// does not indicate the number of virtual samples).  For instance, what
// arch and HW describe as NV_PGRAPH_ANTI_ALIAS_SAMPLES_MODE_2X2_VC_12
// corresponds to NVOS32_ATTR_AA_SAMPLES_4_VIRTUAL_16 here.

#define NVOS32_ATTR_AA_SAMPLES                                 7:4
#define NVOS32_ATTR_AA_SAMPLES_1                        0x00000000
#define NVOS32_ATTR_AA_SAMPLES_2                        0x00000001
#define NVOS32_ATTR_AA_SAMPLES_4                        0x00000002
#define NVOS32_ATTR_AA_SAMPLES_4_ROTATED                0x00000003
#define NVOS32_ATTR_AA_SAMPLES_6                        0x00000004
#define NVOS32_ATTR_AA_SAMPLES_8                        0x00000005
#define NVOS32_ATTR_AA_SAMPLES_16                       0x00000006
#define NVOS32_ATTR_AA_SAMPLES_4_VIRTUAL_8              0x00000007
#define NVOS32_ATTR_AA_SAMPLES_4_VIRTUAL_16             0x00000008
#define NVOS32_ATTR_AA_SAMPLES_8_VIRTUAL_16             0x00000009
#define NVOS32_ATTR_AA_SAMPLES_8_VIRTUAL_32             0x0000000A

// Zcull region (NV40 and up)
// If ATTR_ZCULL is REQUIRED or ANY and ATTR_DEPTH is UNKNOWN, the
// allocation will fail.
// If ATTR_DEPTH or ATTR_AA_SAMPLES is not accurate, erroneous rendering
// may result.
#define NVOS32_ATTR_ZCULL                                    11:10
#define NVOS32_ATTR_ZCULL_NONE                          0x00000000
#define NVOS32_ATTR_ZCULL_REQUIRED                      0x00000001
#define NVOS32_ATTR_ZCULL_ANY                           0x00000002
#define NVOS32_ATTR_ZCULL_SHARED                        0x00000003

// Compression (NV20 and up)
// If ATTR_COMPR is REQUIRED or ANY and ATTR_DEPTH is UNKNOWN, the
// allocation will fail.
// If ATTR_DEPTH or ATTR_AA_SAMPLES is not accurate, performance will
// suffer heavily
#define NVOS32_ATTR_COMPR                                    13:12
#define NVOS32_ATTR_COMPR_NONE                          0x00000000
#define NVOS32_ATTR_COMPR_REQUIRED                      0x00000001
#define NVOS32_ATTR_COMPR_ANY                           0x00000002
#define NVOS32_ATTR_COMPR_PLC_REQUIRED                  NVOS32_ATTR_COMPR_REQUIRED
#define NVOS32_ATTR_COMPR_PLC_ANY                       NVOS32_ATTR_COMPR_ANY
#define NVOS32_ATTR_COMPR_DISABLE_PLC_ANY               0x00000003

//
// Force the allocation to go to the reserved heap.
// This flag is used for KMD allocations when MIG is enabled.
//
#define NVOS32_ATTR_ALLOCATE_FROM_RESERVED_HEAP              14:14
#define NVOS32_ATTR_ALLOCATE_FROM_RESERVED_HEAP_NO      0x00000000
#define NVOS32_ATTR_ALLOCATE_FROM_RESERVED_HEAP_YES     0x00000001

// Format
// _BLOCK_LINEAR is only available for nv50+.
#define NVOS32_ATTR_FORMAT                                   17:16
// Macros representing the low/high bits of NVOS32_ATTR_FORMAT
// bit range. These provide direct access to the range limits
// without needing to split the low:high representation via
// ternary operator, thereby avoiding MISRA 14.3 violation.
#define NVOS32_ATTR_FORMAT_LOW_FIELD                            16
#define NVOS32_ATTR_FORMAT_HIGH_FIELD                           17
#define NVOS32_ATTR_FORMAT_PITCH                        0x00000000
#define NVOS32_ATTR_FORMAT_SWIZZLED                     0x00000001
#define NVOS32_ATTR_FORMAT_BLOCK_LINEAR                 0x00000002

#define NVOS32_ATTR_Z_TYPE                                   18:18
#define NVOS32_ATTR_Z_TYPE_FIXED                        0x00000000
#define NVOS32_ATTR_Z_TYPE_FLOAT                        0x00000001

#define NVOS32_ATTR_ZS_PACKING                               21:19
#define NVOS32_ATTR_ZS_PACKING_S8                       0x00000000 // Z24S8 and S8 share definition
#define NVOS32_ATTR_ZS_PACKING_Z24S8                    0x00000000
#define NVOS32_ATTR_ZS_PACKING_S8Z24                    0x00000001
#define NVOS32_ATTR_ZS_PACKING_Z32                      0x00000002
#define NVOS32_ATTR_ZS_PACKING_Z24X8                    0x00000003
#define NVOS32_ATTR_ZS_PACKING_X8Z24                    0x00000004
#define NVOS32_ATTR_ZS_PACKING_Z32_X24S8                0x00000005
#define NVOS32_ATTR_ZS_PACKING_X8Z24_X24S8              0x00000006
#define NVOS32_ATTR_ZS_PACKING_Z16                      0x00000007
// NOTE: ZS packing and color packing fields are overlaid
#define NVOS32_ATTR_COLOR_PACKING                       NVOS32_ATTR_ZS_PACKING
#define NVOS32_ATTR_COLOR_PACKING_A8R8G8B8              0x00000000
#define NVOS32_ATTR_COLOR_PACKING_X8R8G8B8              0x00000001



//
// For virtual allocs to choose page size for the region. Specifying
// _DEFAULT will select a virtual page size that allows for a surface
// to be mixed between video and system memory and allow the surface
// to be migrated between video and system memory. For tesla chips,
// 4KB will be used. For fermi chips with dual page tables, a virtual
// address with both page tables will be used.
//
// For physical allocation on chips with page swizzle this field is
// used to select the page swizzle.  This later also sets the virtual
// page size, but does not have influence over selecting a migratable
// virtual address. That must be selected when mapping the physical
// memory.
//
// BIG_PAGE  = 64 KB on PASCAL
//           = 64 KB or 128 KB on pre_PASCAL chips
//
// HUGE_PAGE = 2 MB on PASCAL+
//           = 2 MB or 512 MB on AMPERE+
//           = not supported on pre_PASCAL chips.
//
// To request for a HUGE page size,
// set NVOS32_ATTR_PAGE_SIZE to _HUGE and NVOS32_ATTR2_PAGE_SIZE_HUGE to
// the desired size.
//
#define NVOS32_ATTR_PAGE_SIZE                                24:23
#define NVOS32_ATTR_PAGE_SIZE_DEFAULT                   0x00000000
#define NVOS32_ATTR_PAGE_SIZE_4KB                       0x00000001
#define NVOS32_ATTR_PAGE_SIZE_BIG                       0x00000002
#define NVOS32_ATTR_PAGE_SIZE_HUGE                      0x00000003

#define NVOS32_ATTR_LOCATION                                 26:25
#define NVOS32_ATTR_LOCATION_VIDMEM                     0x00000000
#define NVOS32_ATTR_LOCATION_PCI                        0x00000001
#define NVOS32_ATTR_LOCATION_AGP                        0x00000002
#define NVOS32_ATTR_LOCATION_ANY                        0x00000003

//
// _DEFAULT implies _CONTIGUOUS for video memory currently, but
// may be changed to imply _NONCONTIGUOUS in the future.
// _ALLOW_NONCONTIGUOUS enables falling back to the noncontiguous
// vidmem allocator if contig allocation fails.
//
#define NVOS32_ATTR_PHYSICALITY                              28:27
#define NVOS32_ATTR_PHYSICALITY_DEFAULT                 0x00000000
#define NVOS32_ATTR_PHYSICALITY_NONCONTIGUOUS           0x00000001
#define NVOS32_ATTR_PHYSICALITY_CONTIGUOUS              0x00000002
#define NVOS32_ATTR_PHYSICALITY_ALLOW_NONCONTIGUOUS     0x00000003

#define NVOS32_ATTR_COHERENCY                                31:29
#define NVOS32_ATTR_COHERENCY_UNCACHED                  0x00000000
#define NVOS32_ATTR_COHERENCY_CACHED                    0x00000001
#define NVOS32_ATTR_COHERENCY_WRITE_COMBINE             0x00000002
#define NVOS32_ATTR_COHERENCY_WRITE_THROUGH             0x00000003
#define NVOS32_ATTR_COHERENCY_WRITE_PROTECT             0x00000004
#define NVOS32_ATTR_COHERENCY_WRITE_BACK                0x00000005

// ATTR2 fields
#define NVOS32_ATTR2_NONE                               0x00000000

//
// DEFAULT          - Let lower level drivers pick optimal page kind.
// PREFER_NO_ZBC    - Prefer other types of compression over ZBC when
//                    selecting page kind.
// PREFER_ZBC       - Prefer ZBC over other types of compression when
//                    selecting page kind.
// REQUIRE_ONLY_ZBC - Require a page kind that enables ZBC but disables
//                    other types of compression (i.e. 2C page kind).
// INVALID          - Aliases REQUIRE_ONLY_ZBC, which is not supported
//                    by all RM implementations.
//
#define NVOS32_ATTR2_ZBC                                       1:0
#define NVOS32_ATTR2_ZBC_DEFAULT                        0x00000000
#define NVOS32_ATTR2_ZBC_PREFER_NO_ZBC                  0x00000001
#define NVOS32_ATTR2_ZBC_PREFER_ZBC                     0x00000002
#define NVOS32_ATTR2_ZBC_REQUIRE_ONLY_ZBC               0x00000003
#define NVOS32_ATTR2_ZBC_INVALID                        0x00000003

//
// DEFAULT  - Highest performance cache policy that is coherent with the highest
//            performance CPU mapping.  Typically this is gpu cached for video
//            memory and gpu uncached for system memory.
// YES      - Enable gpu caching if supported on this surface type.  For system
//            memory this will not be coherent with direct CPU mappings.
// NO       - Disable gpu caching if supported on this surface type.
// INVALID  - Clients should never set YES and NO simultaneously.
//
#define NVOS32_ATTR2_GPU_CACHEABLE                             3:2
#define NVOS32_ATTR2_GPU_CACHEABLE_DEFAULT              0x00000000
#define NVOS32_ATTR2_GPU_CACHEABLE_YES                  0x00000001
#define NVOS32_ATTR2_GPU_CACHEABLE_NO                   0x00000002
#define NVOS32_ATTR2_GPU_CACHEABLE_INVALID              0x00000003

//
// DEFAULT  - GPU-dependent cache policy
// YES      - Enable gpu caching for p2p mem
// NO       - Disable gpu caching for p2p mem
//
#define NVOS32_ATTR2_P2P_GPU_CACHEABLE                         5:4
#define NVOS32_ATTR2_P2P_GPU_CACHEABLE_DEFAULT          0x00000000
#define NVOS32_ATTR2_P2P_GPU_CACHEABLE_YES              0x00000001
#define NVOS32_ATTR2_P2P_GPU_CACHEABLE_NO               0x00000002

// This applies to virtual allocs only.  See NVOS46_FLAGS_32BIT_POINTER.
#define NVOS32_ATTR2_32BIT_POINTER                             6:6
#define NVOS32_ATTR2_32BIT_POINTER_DISABLE              0x00000000
#define NVOS32_ATTR2_32BIT_POINTER_ENABLE               0x00000001

//
// Whether or not a NUMA Node ID has been specified.
// If yes, the NUMA node ID specified in numaNode will be used.
// If no, memory can be allocated from any socket (numaNode will be ignored).
// Specified numaNode must be of a CPU's memory
//

#define NVOS32_ATTR2_FIXED_NUMA_NODE_ID                        7:7
#define NVOS32_ATTR2_FIXED_NUMA_NODE_ID_NO              0x00000000
#define NVOS32_ATTR2_FIXED_NUMA_NODE_ID_YES             0x00000001

//
// Force SMMU mapping on GPU physical allocation in Tegra
// SMMU mapping for GPU physical allocation decided internally by RM
// This attribute provide an override to RM policy for verification purposes.
//
#define NVOS32_ATTR2_SMMU_ON_GPU                               10:8
#define NVOS32_ATTR2_SMMU_ON_GPU_DEFAULT                 0x00000000
#define NVOS32_ATTR2_SMMU_ON_GPU_DISABLE                 0x00000001
#define NVOS32_ATTR2_SMMU_ON_GPU_ENABLE                  0x00000002

//
// Make comptag allocation aligned to compression cacheline size.
// Specifying this attribute will make RM allocate comptags worth an entire
// comp cacheline. The allocation will be offset aligned to number of comptags/comp cacheline.
//
#define NVOS32_ATTR2_ALLOC_COMPCACHELINE_ALIGN                11:11
#define NVOS32_ATTR2_ALLOC_COMPCACHELINE_ALIGN_OFF              0x0
#define NVOS32_ATTR2_ALLOC_COMPCACHELINE_ALIGN_ON               0x1
#define NVOS32_ATTR2_ALLOC_COMPCACHELINE_ALIGN_DEFAULT            \
                                   NVOS32_ATTR2_ALLOC_COMPCACHELINE_ALIGN_OFF

// Allocation preferred in high or low priority memory
#define NVOS32_ATTR2_PRIORITY                                 13:12
#define NVOS32_ATTR2_PRIORITY_DEFAULT                           0x0
#define NVOS32_ATTR2_PRIORITY_HIGH                              0x1
#define NVOS32_ATTR2_PRIORITY_LOW                               0x2

// PMA: Allocation is an RM internal allocation (RM-only)
#define NVOS32_ATTR2_INTERNAL                                 14:14
#define NVOS32_ATTR2_INTERNAL_NO                                0x0
#define NVOS32_ATTR2_INTERNAL_YES                               0x1

// Allocate 2C instead of 2CZ
#define NVOS32_ATTR2_PREFER_2C                                15:15
#define NVOS32_ATTR2_PREFER_2C_NO                        0x00000000
#define NVOS32_ATTR2_PREFER_2C_YES                       0x00000001

// Allocation used by display engine; RM verifies display engine has enough
// address bits or remapper available.
#define NVOS32_ATTR2_NISO_DISPLAY                             16:16
#define NVOS32_ATTR2_NISO_DISPLAY_NO                     0x00000000
#define NVOS32_ATTR2_NISO_DISPLAY_YES                    0x00000001

//
// !!WARNING!!!
//
// This flag is introduced as a temporary WAR to enable color compression
// without ZBC.
//
// This dangerous flag can be used by UMDs to instruct RM to skip the zbc
// table refcounting that RM does today, when the chosen PTE kind has ZBC
// support.
//
// Currently we do not have a safe per process zbc slot management and
// refcounting mechanism between RM and UMD and hence, any process can
// access any other process's zbc entry in the global zbc table (without mask)
// Inorder to flush the ZBC table for slot reuse RM cannot track which
// process is using which zbc slot. Hence RM has a global refcount for the
// zbc table to flush and reuse the entries if the PTE kind supports zbc.
//
// This scheme poses a problem if there are apps that are persistent such as
// the desktop components that can have color compression enabled which will
// always keep the refcount active. Since these apps can live without
// ZBC, UMD can disable ZBC using masks.
//
// In such a case, if UMD so chooses to disable ZBC, this flag should be used
// to skip refcounting as by default RM would refcount the ZBC table.
//
// NOTE: There is no way for RM to enforce/police this, and we totally rely
// on UMD to use a zbc mask in the pushbuffer method to prevent apps from
// accessing the ZBC table.
//
#define NVOS32_ATTR2_ZBC_SKIP_ZBCREFCOUNT                     17:17
#define NVOS32_ATTR2_ZBC_SKIP_ZBCREFCOUNT_NO             0x00000000
#define NVOS32_ATTR2_ZBC_SKIP_ZBCREFCOUNT_YES            0x00000001

// Allocation requires ISO bandwidth guarantees
#define NVOS32_ATTR2_ISO                                      18:18
#define NVOS32_ATTR2_ISO_NO                              0x00000000
#define NVOS32_ATTR2_ISO_YES                             0x00000001

//
// Turn off blacklist feature for video memory allocation
// This attribute should be used only by Kernel client (KMD), to mask
// the blacklisted pages for the allocation. This is done so that the clients
// will manage the above masked blacklisted pages after the allocation. It will
// return to RM's pool after the allocation was free-d.RmVidHeapCtrl returns
// NV_ERR_INSUFFICIENT_PERMISSIONS if it is being called by non-kernel clients.
//

// TODO: Project ReLingo - This term is marked for deletion. Use PAGE_OFFLINING.
#define NVOS32_ATTR2_BLACKLIST                                19:19
#define NVOS32_ATTR2_BLACKLIST_ON                        0x00000000
#define NVOS32_ATTR2_BLACKLIST_OFF                       0x00000001
#define NVOS32_ATTR2_PAGE_OFFLINING                           19:19
#define NVOS32_ATTR2_PAGE_OFFLINING_ON                   0x00000000
#define NVOS32_ATTR2_PAGE_OFFLINING_OFF                  0x00000001

//
// For virtual allocs to choose the HUGE page size for the region.
// NVOS32_ATTR_PAGE_SIZE must be set to _HUGE to use this.
// Currently, the default huge page is 2MB, so a request with _DEFAULT
// set will always be interpreted as 2MB.
// Not supported on pre_AMPERE chips.
//
#define NVOS32_ATTR2_PAGE_SIZE_HUGE                           21:20
#define NVOS32_ATTR2_PAGE_SIZE_HUGE_DEFAULT              0x00000000
#define NVOS32_ATTR2_PAGE_SIZE_HUGE_2MB                  0x00000001
#define NVOS32_ATTR2_PAGE_SIZE_HUGE_512MB                0x00000002
#define NVOS32_ATTR2_PAGE_SIZE_HUGE_256GB                0x00000003

// Allow read-only or read-write user CPU mappings
#define NVOS32_ATTR2_PROTECTION_USER                          22:22
#define NVOS32_ATTR2_PROTECTION_USER_READ_WRITE          0x00000000
#define NVOS32_ATTR2_PROTECTION_USER_READ_ONLY           0x00000001

// Allow read-only or read-write device mappings
#define NVOS32_ATTR2_PROTECTION_DEVICE                        23:23
#define NVOS32_ATTR2_PROTECTION_DEVICE_READ_WRITE        0x00000000
#define NVOS32_ATTR2_PROTECTION_DEVICE_READ_ONLY         0x00000001

// Deprecated. To be deleted once client code has removed references.
#define NVOS32_ATTR2_USE_EGM                                 24:24
#define NVOS32_ATTR2_USE_EGM_FALSE                      0x00000000
#define NVOS32_ATTR2_USE_EGM_TRUE                       0x00000001

//
// Allow client allocations to go to protected/unprotected video/system memory.
// When Ampere Protected Model aka APM or Confidential Compute is enabled and
// DEFAULT flag is set by client, allocations go to protected memory. When
// protected memory is not enabled, allocations go to unprotected memory.
// If APM or CC is not enabled, it is a bug for a client to set the PROTECTED
// flag to YES
//
#define NVOS32_ATTR2_MEMORY_PROTECTION                       26:25
#define NVOS32_ATTR2_MEMORY_PROTECTION_DEFAULT          0x00000000
#define NVOS32_ATTR2_MEMORY_PROTECTION_PROTECTED        0x00000001
#define NVOS32_ATTR2_MEMORY_PROTECTION_UNPROTECTED      0x00000002
//
// Force the allocation to go to guest subheap.
// This flag is used by vmiop plugin to allocate from GPA
//
#define NVOS32_ATTR2_ALLOCATE_FROM_SUBHEAP                   27:27
#define NVOS32_ATTR2_ALLOCATE_FROM_SUBHEAP_NO           0x00000000
#define NVOS32_ATTR2_ALLOCATE_FROM_SUBHEAP_YES          0x00000001

//
// When allocating memory, register the memory descriptor to GSP-RM
// so that GSP-RM is aware of and can access it
//
#define NVOS32_ATTR2_REGISTER_MEMDESC_TO_PHYS_RM            31:31
#define NVOS32_ATTR2_REGISTER_MEMDESC_TO_PHYS_RM_FALSE      0x00000000
#define NVOS32_ATTR2_REGISTER_MEMDESC_TO_PHYS_RM_TRUE       0x00000001

/**
 * NVOS32 ALLOC_FLAGS
 *
 *      NVOS32_ALLOC_FLAGS_IGNORE_BANK_PLACEMENT
 *
 *      NVOS32_ALLOC_FLAGS_FORCE_MEM_GROWS_UP
 *
 *      NVOS32_ALLOC_FLAGS_FORCE_MEM_GROWS_DOWN
 *
 *      NVOS32_ALLOC_FLAGS_FORCE_ALIGN_HOST_PAGE
 *
 *      NVOS32_ALLOC_FLAGS_FIXED_ADDRESS_ALLOCATE
 *
 *      NVOS32_ALLOC_FLAGS_BANK_HINT
 *
 *      NVOS32_ALLOC_FLAGS_BANK_FORCE
 *
 *      NVOS32_ALLOC_FLAGS_ALIGNMENT_HINT
 *
 *      NVOS32_ALLOC_FLAGS_ALIGNMENT_FORCE
 *
 *      NVOS32_ALLOC_FLAGS_BANK_GROW_UP
 *          Only relevant if bank_hint or bank_force are set
 *
 *      NVOS32_ALLOC_FLAGS_BANK_GROW_DOWN
 *          Only relevant if bank_hint or bank_force are set
 *
 *      NVOS32_ALLOC_FLAGS_LAZY
 *          Lazy allocation (deferred pde, pagetable creation)
 *
 *      NVOS32_ALLOC_FLAGS_NO_SCANOUT
 *          Set if surface will never be scanned out
 *
 *      NVOS32_ALLOC_FLAGS_PITCH_FORCE
 *          Fail alloc if supplied pitch is not aligned
 *
 *      NVOS32_ALLOC_FLAGS_MEMORY_HANDLE_PROVIDED
 *          Memory handle provided to be associated with this allocation
 *
 *      NVOS32_ALLOC_FLAGS_MAP_NOT_REQUIRED
 *          By default memory is mapped into the CPU address space
 *
 *      NVOS32_ALLOC_FLAGS_PERSISTENT_VIDMEM
 *          Allocate persistent video memory
 *
 *      NVOS32_ALLOC_FLAGS_USE_BEGIN_END
 *          Use rangeBegin & rangeEnd fields in allocs other than size/range
 *
 *      NVOS32_ALLOC_FLAGS_TURBO_CIPHER_ENCRYPTED
 *          Allocate TurboCipher encrypted region
 *
 *      NVOS32_ALLOC_FLAGS_VIRTUAL
 *          Allocate virtual memory address space
 *
 *      NVOS32_ALLOC_FLAGS_FORCE_INTERNAL_INDEX
 *          Force allocation internal index
 *
 *      NVOS32_ALLOC_FLAGS_ZCULL_COVG_SPECIFIED
 *          This flag is depreciated and allocations will fail.
 *
 *      NVOS32_ALLOC_FLAGS_EXTERNALLY_MANAGED
 *          Must be used with NVOS32_ALLOC_FLAGS_VIRTUAL.
 *          Page tables for this allocation will be managed outside of RM.
 *
 *      NVOS32_ALLOC_FLAGS_FORCE_DEDICATED_PDE
 *
 *      NVOS32_ALLOC_FLAGS_PROTECTED
 *          Allocate in a protected memory region if available
 *
 *      NVOS32_ALLOC_FLAGS_KERNEL_MAPPING_MAP
 *          Map kernel os descriptor
 *
 *      NVOS32_ALLOC_FLAGS_MAXIMIZE_ADDRESS_SPACE
 *          On WDDM all address spaces are created with MINIMIZE_PTETABLE_SIZE
 *          to reduce the overhead of private address spaces per application,
 *          at the cost of holes in the virtual address space.
 *
 *          Shaders have short pointers that are required to be within a
 *          GPU dependent 32b range.
 *
 *          MAXIMIZE_ADDRESS_SPACE will reverse the MINIMIZE_PTE_TABLE_SIZE
 *          flag with certain restrictions:
 *          - This flag only has an effect when the allocation has the side
 *            effect of creating a new PDE.  It does not affect existing PDEs.
 *          - The first few PDEs of the address space are kept minimum to allow
 *            small applications to use fewer resources.
 *          - By default this operations on the 0-4GB address range.
 *          - If USE_BEGIN_END is specified the setting will apply to the
 *            specified range instead of the first 4GB.
 *
 *      NVOS32_ALLOC_FLAGS_SPARSE
 *          Denote that a virtual address range is "sparse". Must be used with
 *          NVOS32_ALLOC_FLAGS_VIRTUAL. Creation of a "sparse" virtual address range
 *          denotes that an unmapped virtual address range should "not" fault but simply
 *          return 0's.
 *
 *      NVOS32_ALLOC_FLAGS_ALLOCATE_KERNEL_PRIVILEGED
 *          This a special flag that can be used only by kernel(root) clients
 *          to allocate memory out of a protected region of the address space
 *          If this flag is set by non kernel clients then the allocation will
 *          fail.
 *
 *      NVOS32_ALLOC_FLAGS_SKIP_RESOURCE_ALLOC
 *
 *      NVOS32_ALLOC_FLAGS_PREFER_PTES_IN_SYSMEMORY
 *          If new pagetable need to be allocated prefer them in sysmem (if supported by the gpu)
 *
 *      NVOS32_ALLOC_FLAGS_SKIP_ALIGN_PAD
 *          As per KMD request to eliminate extra allocation
 *
 *      NVOS32_ALLOC_FLAGS_WPR1
 *          Allocate in a WPR1 region if available
 *
 *      NVOS32_ALLOC_FLAGS_ZCULL_DONT_ALLOCATE_SHARED_1X
 *          If using zcull sharing and this surface is fsaa, then don't allocate an additional non-FSAA region.
 *
 *      NVOS32_ALLOC_FLAGS_WPR2
 *          Allocate in a WPR1 region if available
 */
#define NVOS32_ALLOC_FLAGS_IGNORE_BANK_PLACEMENT        0x00000001
#define NVOS32_ALLOC_FLAGS_FORCE_MEM_GROWS_UP           0x00000002
#define NVOS32_ALLOC_FLAGS_FORCE_MEM_GROWS_DOWN         0x00000004
#define NVOS32_ALLOC_FLAGS_FORCE_ALIGN_HOST_PAGE        0x00000008
#define NVOS32_ALLOC_FLAGS_FIXED_ADDRESS_ALLOCATE       0x00000010
#define NVOS32_ALLOC_FLAGS_BANK_HINT                    0x00000020
#define NVOS32_ALLOC_FLAGS_BANK_FORCE                   0x00000040
#define NVOS32_ALLOC_FLAGS_ALIGNMENT_HINT               0x00000080
#define NVOS32_ALLOC_FLAGS_ALIGNMENT_FORCE              0x00000100
#define NVOS32_ALLOC_FLAGS_BANK_GROW_UP                 0x00000000
#define NVOS32_ALLOC_FLAGS_BANK_GROW_DOWN               0x00000200
#define NVOS32_ALLOC_FLAGS_LAZY                         0x00000400
#define NVOS32_ALLOC_FLAGS_FORCE_REVERSE_ALLOC          0x00000800
#define NVOS32_ALLOC_FLAGS_NO_SCANOUT                   0x00001000
#define NVOS32_ALLOC_FLAGS_PITCH_FORCE                  0x00002000
#define NVOS32_ALLOC_FLAGS_MEMORY_HANDLE_PROVIDED       0x00004000
#define NVOS32_ALLOC_FLAGS_MAP_NOT_REQUIRED             0x00008000
#define NVOS32_ALLOC_FLAGS_PERSISTENT_VIDMEM            0x00010000
#define NVOS32_ALLOC_FLAGS_USE_BEGIN_END                0x00020000
#define NVOS32_ALLOC_FLAGS_TURBO_CIPHER_ENCRYPTED       0x00040000
#define NVOS32_ALLOC_FLAGS_VIRTUAL                      0x00080000
#define NVOS32_ALLOC_FLAGS_FORCE_INTERNAL_INDEX         0x00100000
#define NVOS32_ALLOC_FLAGS_ZCULL_COVG_SPECIFIED         0x00200000
#define NVOS32_ALLOC_FLAGS_EXTERNALLY_MANAGED           0x00400000
#define NVOS32_ALLOC_FLAGS_FORCE_DEDICATED_PDE          0x00800000
#define NVOS32_ALLOC_FLAGS_PROTECTED                    0x01000000
#define NVOS32_ALLOC_FLAGS_KERNEL_MAPPING_MAP           0x02000000 // TODO BUG 2488679: fix alloc flag aliasing
#define NVOS32_ALLOC_FLAGS_MAXIMIZE_ADDRESS_SPACE       0x02000000
#define NVOS32_ALLOC_FLAGS_SPARSE                       0x04000000
#define NVOS32_ALLOC_FLAGS_USER_READ_ONLY               0x04000000 // TODO BUG 2488682: remove this after KMD transition
#define NVOS32_ALLOC_FLAGS_DEVICE_READ_ONLY             0x08000000 // TODO BUG 2488682: remove this after KMD transition
#define NVOS32_ALLOC_FLAGS_ALLOCATE_KERNEL_PRIVILEGED   0x08000000
#define NVOS32_ALLOC_FLAGS_SKIP_RESOURCE_ALLOC          0x10000000
#define NVOS32_ALLOC_FLAGS_PREFER_PTES_IN_SYSMEMORY     0x20000000
#define NVOS32_ALLOC_FLAGS_SKIP_ALIGN_PAD               0x40000000
#define NVOS32_ALLOC_FLAGS_WPR1                         0x40000000 // TODO BUG 2488672: fix alloc flag aliasing
#define NVOS32_ALLOC_FLAGS_ZCULL_DONT_ALLOCATE_SHARED_1X 0x80000000
#define NVOS32_ALLOC_FLAGS_WPR2                         0x80000000 // TODO BUG 2488672: fix alloc flag aliasing

// Internal flags used for RM's allocation paths
#define NVOS32_ALLOC_INTERNAL_FLAGS_CLIENTALLOC         0x00000001 // RM internal flags - not sure if this should be exposed even. Keeping it here.
#define NVOS32_ALLOC_INTERNAL_FLAGS_SKIP_SCRUB          0x00000004 // RM internal flags - not sure if this should be exposed even. Keeping it here.
#define NVOS32_ALLOC_FLAGS_MAXIMIZE_4GB_ADDRESS_SPACE NVOS32_ALLOC_FLAGS_MAXIMIZE_ADDRESS_SPACE // Legacy name

//
// Bitmask of flags that are only valid for virtual allocations.
//
#define NVOS32_ALLOC_FLAGS_VIRTUAL_ONLY         ( \
    NVOS32_ALLOC_FLAGS_VIRTUAL                  | \
    NVOS32_ALLOC_FLAGS_LAZY                     | \
    NVOS32_ALLOC_FLAGS_EXTERNALLY_MANAGED       | \
    NVOS32_ALLOC_FLAGS_SPARSE                   | \
    NVOS32_ALLOC_FLAGS_MAXIMIZE_ADDRESS_SPACE   | \
    NVOS32_ALLOC_FLAGS_PREFER_PTES_IN_SYSMEMORY )

// COMPR_COVG_* allows for specification of what compression resources
// are required (_MIN) and necessary (_MAX).  Default behavior is for
// RM to provide as much as possible, including none if _ANY is allowed.
// Values for min/max are (0-100, a %) * _COVG_SCALE (so max value is
// 100*100==10000).  _START is used to specify the % offset into the
// region to begin the requested coverage.
// _COVG_BITS allows specification of the number of comptags per ROP tile.
// A value of 0 is default and allows RM to choose based upon MMU/FB rules.
// All other values for _COVG_BITS are arch-specific.
// Note: NVOS32_ATTR_COMPR_COVG_PROVIDED must be set for this feature
// to be available (verif-only).
#define NVOS32_ALLOC_COMPR_COVG_SCALE                           10
#define NVOS32_ALLOC_COMPR_COVG_BITS                           1:0
#define NVOS32_ALLOC_COMPR_COVG_BITS_DEFAULT            0x00000000
#define NVOS32_ALLOC_COMPR_COVG_BITS_1                  0x00000001
#define NVOS32_ALLOC_COMPR_COVG_BITS_2                  0x00000002
#define NVOS32_ALLOC_COMPR_COVG_BITS_4                  0x00000003
#define NVOS32_ALLOC_COMPR_COVG_MAX                           11:2
#define NVOS32_ALLOC_COMPR_COVG_MIN                          21:12
#define NVOS32_ALLOC_COMPR_COVG_START                        31:22


// Note: NVOS32_ALLOC_FLAGS_ZCULL_COVG_SPECIFIED must be set for this feature
// to be enabled.
// If FALLBACK_ALLOW is set, a fallback from LOW_RES_Z or LOW_RES_ZS
// to HIGH_RES_Z is allowed if the surface can't be fully covered.
#define NVOS32_ALLOC_ZCULL_COVG_FORMAT                         3:0
#define NVOS32_ALLOC_ZCULL_COVG_FORMAT_LOW_RES_Z        0x00000000
#define NVOS32_ALLOC_ZCULL_COVG_FORMAT_HIGH_RES_Z       0x00000002
#define NVOS32_ALLOC_ZCULL_COVG_FORMAT_LOW_RES_ZS       0x00000003
#define NVOS32_ALLOC_ZCULL_COVG_FALLBACK                       4:4
#define NVOS32_ALLOC_ZCULL_COVG_FALLBACK_DISALLOW       0x00000000
#define NVOS32_ALLOC_ZCULL_COVG_FALLBACK_ALLOW          0x00000001


// _ALLOC_COMPTAG_OFFSET allows the caller to specify the starting
// offset for the comptags for a given surface, primarily for test only.
// To specify an offset, set _USAGE_FIXED or _USAGE_MIN in conjunction
// with _START.
//
// _USAGE_FIXED sets a surface's comptagline to start at the given
// starting value.  If the offset has already been assigned, then
// the alloc call fails.
//
// _USAGE_MIN sets a surface's comptagline to start at the given
// starting value or higher, depending on comptagline availability.
// In this case, if the offset has already been assigned, the next
// available comptagline (in increasing order) will be assigned.
//
// For Fermi, up to 2^17 comptags may be allowed, but the actual,
// usable limit depends on the size of the compbit backing store.
//
// For Pascal, up to 2 ^ 18 comptags may be allowed
// From Turing. up to 2 ^ 20 comptags may be allowed
//
// See also field ctagOffset in struct NVOS32_PARAMETERS.
#define NVOS32_ALLOC_COMPTAG_OFFSET_START                     19:0
#define NVOS32_ALLOC_COMPTAG_OFFSET_START_DEFAULT       0x00000000
#define NVOS32_ALLOC_COMPTAG_OFFSET_USAGE                    31:30
#define NVOS32_ALLOC_COMPTAG_OFFSET_USAGE_DEFAULT       0x00000000
#define NVOS32_ALLOC_COMPTAG_OFFSET_USAGE_OFF           0x00000000
#define NVOS32_ALLOC_COMPTAG_OFFSET_USAGE_FIXED         0x00000001
#define NVOS32_ALLOC_COMPTAG_OFFSET_USAGE_MIN           0x00000002


// REALLOC flags field
#define NVOS32_REALLOC_FLAGS_GROW_ALLOCATION            0x00000000
#define NVOS32_REALLOC_FLAGS_SHRINK_ALLOCATION          0x00000001
#define NVOS32_REALLOC_FLAGS_REALLOC_UP                 0x00000000 // towards/from high memory addresses
#define NVOS32_REALLOC_FLAGS_REALLOC_DOWN               0x00000002 // towards/from memory address 0

// RELEASE_COMPR, REACQUIRE_COMPR flags field
#define NVOS32_RELEASE_COMPR_FLAGS_MEMORY_HANDLE_PROVIDED   0x000000001

#define NVOS32_REACQUIRE_COMPR_FLAGS_MEMORY_HANDLE_PROVIDED 0x000000001


// FREE flags field
#define NVOS32_FREE_FLAGS_MEMORY_HANDLE_PROVIDED        0x00000001

// DUMP flags field
#define NVOS32_DUMP_FLAGS_TYPE                                 1:0
#define NVOS32_DUMP_FLAGS_TYPE_FB                       0x00000000
#define NVOS32_DUMP_FLAGS_TYPE_CLIENT_PD                0x00000001
#define NVOS32_DUMP_FLAGS_TYPE_CLIENT_VA                0x00000002
#define NVOS32_DUMP_FLAGS_TYPE_CLIENT_VAPTE             0x00000003

#define NVOS32_BLOCK_TYPE_FREE                          0xFFFFFFFF
#define NVOS32_INVALID_BLOCK_FREE_OFFSET                0xFFFFFFFF

#define NVOS32_MEM_TAG_NONE                             0x00000000

/*
 * NV_CONTEXT_DMA_ALLOCATION_PARAMS - Allocation params to create context dma
   through NvRmAlloc.
 */
typedef struct
{
    NvHandle hSubDevice;
    NvV32    flags;
    NvHandle hMemory;
    NvU64    offset NV_ALIGN_BYTES(8);
    NvU64    limit NV_ALIGN_BYTES(8);
} NV_CONTEXT_DMA_ALLOCATION_PARAMS;

/*
 * NV_MEMORY_ALLOCATION_PARAMS - Allocation params to create memory through
 * NvRmAlloc. Flags are populated with NVOS32_ defines.
 */
typedef struct
{
    NvU32     owner;                        // [IN]  - memory owner ID
    NvU32     type;                         // [IN]  - surface type, see below TYPE* defines
    NvU32     flags;                        // [IN]  - allocation modifier flags, see below ALLOC_FLAGS* defines

    NvU32     width;                        // [IN]  - width of surface in pixels
    NvU32     height;                       // [IN]  - height of surface in pixels
    NvS32     pitch;                        // [IN/OUT] - desired pitch AND returned actual pitch allocated

    NvU32     attr;                         // [IN/OUT] - surface attributes requested, and surface attributes allocated
    NvU32     attr2;                        // [IN/OUT] - surface attributes requested, and surface attributes allocated

    NvU32     format;                       // [IN/OUT] - format requested, and format allocated
    NvU32     comprCovg;                    // [IN/OUT] - compr covg requested, and allocated
    NvU32     zcullCovg;                    // [OUT] - zcull covg allocated

    NvU64     rangeLo   NV_ALIGN_BYTES(8);  // [IN]  - allocated memory will be limited to the range
    NvU64     rangeHi   NV_ALIGN_BYTES(8);  // [IN]  - from rangeBegin to rangeEnd, inclusive.

    NvU64     size      NV_ALIGN_BYTES(8);  // [IN/OUT]  - size of allocation - also returns the actual size allocated
    NvU64     alignment NV_ALIGN_BYTES(8);  // [IN]  - requested alignment - NVOS32_ALLOC_FLAGS_ALIGNMENT* must be on
    NvU64     offset    NV_ALIGN_BYTES(8);  // [IN/OUT]  - desired offset if NVOS32_ALLOC_FLAGS_FIXED_ADDRESS_ALLOCATE is on AND returned offset
    NvU64     limit     NV_ALIGN_BYTES(8);  // [OUT] - returned surface limit
    NvP64     address   NV_ALIGN_BYTES(8);  // [OUT] - returned address

    NvU32     ctagOffset;                   // [IN] - comptag offset for this surface (see NVOS32_ALLOC_COMPTAG_OFFSET)
    NvHandle  hVASpace;                     // [IN]  - VASpace handle. Used when flag is VIRTUAL.

    NvU32     internalflags;                // [IN]  - internal flags to change allocation behaviors from internal paths

    NvU32     tag;                          // [IN] - memory tag used for debugging

    NvS32     numaNode;                     // [IN] - CPU NUMA node from which memory should be allocated
} NV_MEMORY_ALLOCATION_PARAMS;

/*
 * NV_OS_DESC_MEMORY_ALLOCATION_PARAMS - Allocation params to create OS
 * described memory through NvRmAlloc. Flags are populated with NVOS32_ defines.
 */
typedef struct
{
    NvU32     type;                         // [IN]  - surface type, see below TYPE* defines
    NvU32     flags;                        // [IN]  - allocation modifier flags, see below ALLOC_FLAGS* defines
    NvU32     attr;                         // [IN]  - attributes for memory placement/properties, see below
    NvU32     attr2;                        // [IN]  - attributes GPU_CACHEABLE
    NvP64     descriptor NV_ALIGN_BYTES(8); // [IN]  - descriptor address
    NvU64     limit      NV_ALIGN_BYTES(8); // [IN]  - allocated size -1
    NvU32     descriptorType;               // [IN]  - descriptor type(Virtual | nvmap Handle)
    NvU32     tag;                          // [IN]  - memory tag used for debugging
} NV_OS_DESC_MEMORY_ALLOCATION_PARAMS;

/*
 * NV_USER_LOCAL_DESC_MEMORY_ALLOCATION_PARAMS - Allocation params to create a memory
 * object from user allocated video memory. Flags are populated with NVOS32_*
 * defines.
 */
typedef struct
{
    NvU32     flags;                        // [IN]  - allocation modifier flags, see NVOS02_FLAGS* defines
    NvU64     physAddr   NV_ALIGN_BYTES(8); // [IN]  - physical address
    NvU64     size       NV_ALIGN_BYTES(8); // [IN]  - mem size
    NvU32     tag;                          // [IN]  - memory tag used for debugging
    NvBool    bGuestAllocated;              // [IN]  - Set if memory is guest allocated (mapped by VMMU)
} NV_USER_LOCAL_DESC_MEMORY_ALLOCATION_PARAMS;

/*
 * NV_MEMORY_HW_RESOURCES_ALLOCATION_PARAMS - Allocation params to create
 * memory HW resources through NvRmAlloc. Flags are populated with NVOS32_
 * defines.
 */
typedef struct
{
    NvU32     owner;                        // [IN]  - memory owner ID
    NvU32     flags;                        // [IN]  - allocation modifier flags, see below ALLOC_FLAGS* defines
    NvU32     type;                         // [IN]  - surface type, see below TYPE* defines

    NvU32     attr;                         // [IN/OUT] - surface attributes requested, and surface attributes allocated
    NvU32     attr2;                        // [IN/OUT] - surface attributes requested, and surface attributes allocated

    NvU32     height;
    NvU32     width;
    NvU32     pitch;
    NvU32     alignment;
    NvU32     comprCovg;
    NvU32     zcullCovg;

    NvU32     kind;

    NvP64     bindResultFunc  NV_ALIGN_BYTES(8);  // BindResultFunc
    NvP64     pHandle         NV_ALIGN_BYTES(8);
    NvU64     osDeviceHandle  NV_ALIGN_BYTES(8);
    NvU64     size            NV_ALIGN_BYTES(8);
    NvU64     allocAddr       NV_ALIGN_BYTES(8);

    // [out] from GMMU_COMPR_INFO in drivers/common/shared/inc/mmu/gmmu_fmt.h
    NvU32 compPageShift;
    NvU32 compressedKind;
    NvU32 compTagLineMin;
    NvU32 compPageIndexLo;
    NvU32 compPageIndexHi;
    NvU32 compTagLineMultiplier;

    // [out] fallback uncompressed kind.
    NvU32  uncompressedKind;

    NvU32     tag;                          // [IN] - memory tag used for debugging
} NV_MEMORY_HW_RESOURCES_ALLOCATION_PARAMS;

/* function OS33 */
#define NV04_MAP_MEMORY                                 (0x00000021)

// Legacy map and unmap memory flags that don't use DRF_DEF scheme
#define NV04_MAP_MEMORY_FLAGS_NONE                      (0x00000000)
#define NV04_MAP_MEMORY_FLAGS_USER                      (0x00004000)

// New map and unmap memory flags.  These flags are used for both NvRmMapMemory
// and for NvRmUnmapMemory.

// Mappings can have restricted permissions (read-only, write-only).  Some
// RM implementations may choose to ignore these flags, or they may work
// only for certain memory spaces (system, AGP, video memory); in such cases,
// you may get a read/write mapping even if you asked for a read-only or
// write-only mapping.
#define NVOS33_FLAGS_ACCESS                                        1:0
#define NVOS33_FLAGS_ACCESS_READ_WRITE                             (0x00000000)
#define NVOS33_FLAGS_ACCESS_READ_ONLY                              (0x00000001)
#define NVOS33_FLAGS_ACCESS_WRITE_ONLY                             (0x00000002)

// Persistent mappings are no longer supported
#define NVOS33_FLAGS_PERSISTENT                                    4:4
#define NVOS33_FLAGS_PERSISTENT_DISABLE                            (0x00000000)
#define NVOS33_FLAGS_PERSISTENT_ENABLE                             (0x00000001)

// This flag is a hack to work around bug 150889.  It disables the error
// checking in the RM that verifies that the client is not trying to map
// memory past the end of the memory object.  This error checking needs to
// be shut off in some cases for a PAE bug workaround in certain kernels.
#define NVOS33_FLAGS_SKIP_SIZE_CHECK                               8:8
#define NVOS33_FLAGS_SKIP_SIZE_CHECK_DISABLE                       (0x00000000)
#define NVOS33_FLAGS_SKIP_SIZE_CHECK_ENABLE                        (0x00000001)

// Normally, a mapping is created in the same memory space as the client -- in
// kernel space for a kernel RM client, or in user space for a user RM client.
// However, a kernel RM client can specify MEM_SPACE:USER to create a user-space
// mapping in the current RM client.
#define NVOS33_FLAGS_MEM_SPACE                                     14:14
#define NVOS33_FLAGS_MEM_SPACE_CLIENT                              (0x00000000)
#define NVOS33_FLAGS_MEM_SPACE_USER                                (0x00000001)

// The client can ask for direct memory mapping (i.e. no BAR1) if remappers and
// blocklinear are not required. RM can do direct mapping in this case if
// carveout is available.
// DEFAULT:   Use direct mapping if available and no address/data translation
//            is necessary; reflected otherwise
// DIRECT:    Use direct mapping if available, even if some translation is
//            necessary (the client is responsible for translation)
// REFLECTED: Always use reflected mapping
#define NVOS33_FLAGS_MAPPING                                       16:15
#define NVOS33_FLAGS_MAPPING_DEFAULT                               (0x00000000)
#define NVOS33_FLAGS_MAPPING_DIRECT                                (0x00000001)
#define NVOS33_FLAGS_MAPPING_REFLECTED                             (0x00000002)

// The client requests a fifo mapping but doesn't know the offset or length
// DEFAULT:   Do error check length and offset
// ENABLE:    Don't error check length and offset but have the RM fill them in
#define NVOS33_FLAGS_FIFO_MAPPING                                  17:17
#define NVOS33_FLAGS_FIFO_MAPPING_DEFAULT                          (0x00000000)
#define NVOS33_FLAGS_FIFO_MAPPING_ENABLE                           (0x00000001)

// The client can require that the CPU mapping be to a specific CPU address
// (akin to MAP_FIXED for mmap).
// DISABLED: RM will map the allocation at a CPU VA that RM selects.
// ENABLED:  RM will map the allocation at the CPU VA specified by the address
//           pass-back parameter to NvRmMapMemory
// NOTES:
// - Used for controlling CPU addresses in CUDA's unified CPU+GPU virtual
//   address space
// - Only valid on NvRmMapMemory
// - Implemented on Unix but not VMware
#define NVOS33_FLAGS_MAP_FIXED                                     18:18
#define NVOS33_FLAGS_MAP_FIXED_DISABLE                             (0x00000000)
#define NVOS33_FLAGS_MAP_FIXED_ENABLE                              (0x00000001)

// The client can specify to the RM that the CPU virtual address range for an
// allocation should remain reserved after the allocation is unmapped.
// DISABLE:   When this mapping is destroyed, RM will unmap the CPU virtual
//            address space used by this allocation.  On Linux this corresponds
//            to calling munmap on the CPU VA region.
// ENABLE:    When the map object is freed, RM will leave the CPU virtual
//            address space used by allocation reserved.  On Linux this means
//            that RM will overwrite the previous mapping with an anonymous
//            mapping of instead calling munmap.
// NOTES:
// - When combined with MAP_FIXED, this allows the client to exert
//   significant control over the CPU heap
// - Used in CUDA's unified CPU+GPU virtual address space
// - Valid in nvRmUnmapMemory
// - Valid on NvRmMapMemory (specifies RM's behavior whenever the
//   mapping is destroyed, regardless of mechanism)
// - Implemented on Unix but not VMware
#define NVOS33_FLAGS_RESERVE_ON_UNMAP                              19:19
#define NVOS33_FLAGS_RESERVE_ON_UNMAP_DISABLE                      (0x00000000)
#define NVOS33_FLAGS_RESERVE_ON_UNMAP_ENABLE                       (0x00000001)

// Internal use only
#define NVOS33_FLAGS_OS_DESCRIPTOR                                 22:22
#define NVOS33_FLAGS_OS_DESCRIPTOR_DISABLE                         (0x00000000)
#define NVOS33_FLAGS_OS_DESCRIPTOR_ENABLE                          (0x00000001)

//
// For use in the linux mapping path. This flag sets the
// caching mode for pcie BAR mappings (from nv_memory_type.h).
// Internal use only.
//
#define NVOS33_FLAGS_CACHING_TYPE                                  25:23
#define NVOS33_FLAGS_CACHING_TYPE_CACHED                           0
#define NVOS33_FLAGS_CACHING_TYPE_UNCACHED                         1
#define NVOS33_FLAGS_CACHING_TYPE_WRITECOMBINED                    2
#define NVOS33_FLAGS_CACHING_TYPE_WRITEBACK                        5
#define NVOS33_FLAGS_CACHING_TYPE_DEFAULT                          6
#define NVOS33_FLAGS_CACHING_TYPE_UNCACHED_WEAK                    7

//
// For use when Hopper Confidential Compute is operating in devtools mode
// BAR1 access to CPR vidmem is blocked to CPU-RM by default when HCC is
// enabled in both devtools and prod modes. However, certain mappings are
// allowed to go through successfully only in devtools mode. For example,
// CPU mappings made on behalf of devtools, event buffer mappings are allowed
// to happen in devtools mode
//
#define NVOS33_FLAGS_ALLOW_MAPPING_ON_HCC                     26:26
#define NVOS33_FLAGS_ALLOW_MAPPING_ON_HCC_NO                  (0x00000000)
#define NVOS33_FLAGS_ALLOW_MAPPING_ON_HCC_YES                 (0x00000001)

/* parameters */
typedef struct
{
    NvHandle hClient;
    NvHandle hDevice;            // device or sub-device handle
    NvHandle hMemory;            // handle to memory object if provided -- NULL if not
    NvU64    offset NV_ALIGN_BYTES(8);
    NvU64    length NV_ALIGN_BYTES(8);
    NvP64    pLinearAddress NV_ALIGN_BYTES(8);     // pointer for returned address
    NvU32    status;
    NvU32    flags;
} NVOS33_PARAMETERS;


/* function OS34 */
#define NV04_UNMAP_MEMORY                                          (0x00000022)

/* parameters */
typedef struct
{
    NvHandle hClient;
    NvHandle hDevice;
    NvHandle hMemory;
    NvP64    pLinearAddress NV_ALIGN_BYTES(8);     // ptr to virtual address of mapped memory
    NvU32    status;
    NvU32    flags;
} NVOS34_PARAMETERS;

/* function OS38 */
#define NV04_ACCESS_REGISTRY                                       (0x00000026)

/* parameter values */
#define NVOS38_ACCESS_TYPE_READ_DWORD                                        1
#define NVOS38_ACCESS_TYPE_WRITE_DWORD                                       2
#define NVOS38_ACCESS_TYPE_READ_BINARY                                       6
#define NVOS38_ACCESS_TYPE_WRITE_BINARY                                      7

#define NVOS38_MAX_REGISTRY_STRING_LENGTH                                  256
#define NVOS38_MAX_REGISTRY_BINARY_LENGTH                                  256

/* parameters */
typedef struct
{
    NvHandle hClient;
    NvHandle hObject;
    NvV32    AccessType;

    NvV32    DevNodeLength;
    NvP64    pDevNode NV_ALIGN_BYTES(8);

    NvV32    ParmStrLength;
    NvP64    pParmStr NV_ALIGN_BYTES(8);

    NvV32    BinaryDataLength;
    NvP64    pBinaryData NV_ALIGN_BYTES(8);

    NvV32    Data;
    NvV32    Entry;
    NvV32    status;
} NVOS38_PARAMETERS;

#define  NV04_ALLOC_CONTEXT_DMA                                    (0x00000027)

/* parameter values are the same as NVOS03 -- not repeated here */

/* parameters */
typedef struct
{
    NvHandle hObjectParent;
    NvHandle hSubDevice;
    NvHandle hObjectNew;
    NvV32    hClass;
    NvV32    flags;
    NvU32    selector;
    NvHandle hMemory;
    NvU64    offset NV_ALIGN_BYTES(8);
    NvU64    limit NV_ALIGN_BYTES(8);
    NvV32    status;
} NVOS39_PARAMETERS;


#define NV04_GET_EVENT_DATA                                        (0x00000028)

typedef struct
{
    NvHandle hObject;
    NvV32    NotifyIndex;

    //
    // Holds same information as that of nvgputypes.h::NvNotification's
    // info32 and info16.
    //
    NvV32    info32;
    NvU16    info16;
} NvUnixEvent;

/* parameters */
typedef struct
{
    NvP64 pEvent NV_ALIGN_BYTES(8);
    NvV32 MoreEvents;
    NvV32 status;
} NVOS41_PARAMETERS;

/* function NVOS43        -- deleted 4/09 */
/* #define NV04_UNIFIED_FREE                                       (0x0000002B)  */


#define  NVSIM01_BUS_XACT                                          (0x0000002C)

/* parameters */
typedef struct
{
    NvHandle hClient; // n/a currently
    NvHandle hDevice; // n/a currently
    NvU32    offset;  // phy bus offset
    NvU32    bar;     // ~0 := phy addr, {0..2} specify gpu bar
    NvU32    bytes;   // # of bytes
    NvU32    write;   // 0 := read request
    NvU32    data;    // in/out based upon 'write'
    NvU32    status;
} NVOS2C_PARAMETERS;

/* function NVOS2D        -- deleted 4/09 */
/* #define  NVSIM01_BUS_GET_IFACES                                 (0x0000002D)  */


/* function OS46 */
#define NV04_MAP_MEMORY_DMA                                        (0x0000002E)

/* parameter values */
#define NVOS46_FLAGS_ACCESS                                        1:0
#define NVOS46_FLAGS_ACCESS_READ_WRITE                             (0x00000000)
#define NVOS46_FLAGS_ACCESS_READ_ONLY                              (0x00000001)
#define NVOS46_FLAGS_ACCESS_WRITE_ONLY                             (0x00000002)

//
// Compute shaders support both 32b and 64b pointers. This allows mappings
// to be restricted to the bottom 4GB of the address space. How _DISABLE
// is handled is chip specific and may force a pointer above 4GB.
//
#define NVOS46_FLAGS_32BIT_POINTER                                 2:2
#define NVOS46_FLAGS_32BIT_POINTER_DISABLE                         (0x00000000)
#define NVOS46_FLAGS_32BIT_POINTER_ENABLE                          (0x00000001)

#define NVOS46_FLAGS_PAGE_KIND                                     3:3
#define NVOS46_FLAGS_PAGE_KIND_PHYSICAL                            (0x00000000)
#define NVOS46_FLAGS_PAGE_KIND_VIRTUAL                             (0x00000001)

#define NVOS46_FLAGS_CACHE_SNOOP                                   4:4
#define NVOS46_FLAGS_CACHE_SNOOP_DISABLE                           (0x00000000)
#define NVOS46_FLAGS_CACHE_SNOOP_ENABLE                            (0x00000001)

// The client requests a CPU kernel mapping so that SW class could use it
// DEFAULT: Don't map CPU address
// ENABLE:  Map CPU address
#define NVOS46_FLAGS_KERNEL_MAPPING                                5:5
#define NVOS46_FLAGS_KERNEL_MAPPING_NONE                           (0x00000000)
#define NVOS46_FLAGS_KERNEL_MAPPING_ENABLE                         (0x00000001)

//
// Compute shader access control.
// GPUs that support this feature set the NV0080_CTRL_DMA_CAPS_SHADER_ACCESS_SUPPORTED
// property. These were first supported in Kepler. _DEFAULT will match the ACCESS field.
//
#define NVOS46_FLAGS_SHADER_ACCESS                                 7:6
#define NVOS46_FLAGS_SHADER_ACCESS_DEFAULT                         (0x00000000)
#define NVOS46_FLAGS_SHADER_ACCESS_READ_ONLY                       (0x00000001)
#define NVOS46_FLAGS_SHADER_ACCESS_WRITE_ONLY                      (0x00000002)
#define NVOS46_FLAGS_SHADER_ACCESS_READ_WRITE                      (0x00000003)

//
// How the PAGE_SIZE field is interpreted is architecture specific.
//
// On Curie chips it is ignored.
//
// On Tesla it is used to guide is used to select which type PDE
// to use. By default the RM will select 4KB for system memory
// and BIG (64KB) for video memory. BOTH is not supported.
//
// Likewise on Fermi this used to select the PDE type. Fermi cannot
// mix page sizes to a single mapping so the page size is determined
// at surface alloation time. 4KB or BIG may be specified but they
// must match the page size selected at allocation time.  DEFAULT
// allows the RM to select either a single page size or both PDE,
// while BOTH forces the RM to select a dual page size PDE.
//
// BIG_PAGE  = 64 KB on PASCAL
//           = 64 KB or 128 KB on pre_PASCAL chips
//
// HUGE_PAGE = 2 MB on PASCAL
//           = not supported on pre_PASCAL chips.
//
#define NVOS46_FLAGS_PAGE_SIZE                                     11:8
#define NVOS46_FLAGS_PAGE_SIZE_DEFAULT                             (0x00000000)
#define NVOS46_FLAGS_PAGE_SIZE_4KB                                 (0x00000001)
#define NVOS46_FLAGS_PAGE_SIZE_BIG                                 (0x00000002)
#define NVOS46_FLAGS_PAGE_SIZE_BOTH                                (0x00000003)
#define NVOS46_FLAGS_PAGE_SIZE_HUGE                                (0x00000004)
#define NVOS46_FLAGS_PAGE_SIZE_512M                                (0x00000005)

// Some systems allow the device to use the system L3 cache when accessing the
// system memory. For example, the iGPU on T19X can allocate from the system L3
// provided the SoC L3 cache is configured for device allocation.
//
// NVOS46_FLAGS_SYSTEM_L3_ALLOC_DEFAULT - Use the default L3 allocation
// policy. When using this policy, device memory access will be coherent with
// non-snooping devices such as the display on Tegra.
//
// NVOS46_FLAGS_SYSTEM_L3_ALLOC_ENABLE_HINT - Enable L3 allocation if possible.
// When L3 allocation is enabled, device memory access may be cached, and the
// memory access will be coherent only with other snoop-enabled access. This
// flag is a hint and will be ignored if the system does not support L3
// allocation for the device. NVOS46_FLAGS_CACHE_SNOOP_ENABLE must also be set
// for this flag to be effective.
//
// Note: This flag is implemented only by rmapi_tegra. It is not implemented by
// Resman.
//
#define NVOS46_FLAGS_SYSTEM_L3_ALLOC                               13:13
#define NVOS46_FLAGS_SYSTEM_L3_ALLOC_DEFAULT                       (0x00000000)
#define NVOS46_FLAGS_SYSTEM_L3_ALLOC_ENABLE_HINT                   (0x00000001)

#define NVOS46_FLAGS_DMA_OFFSET_GROWS                              14:14
#define NVOS46_FLAGS_DMA_OFFSET_GROWS_UP                           (0x00000000)
#define NVOS46_FLAGS_DMA_OFFSET_GROWS_DOWN                         (0x00000001)

//
// DMA_OFFSET_FIXED is overloaded for two purposes.
//
// 1. For CTXDMA mappings that use DMA_UNICAST_REUSE_ALLOC_FALSE,
//    DMA_OFFSET_FIXED_TRUE indicates to use the dmaOffset parameter
//    for a fixed address allocation out of the VA space heap.
//    DMA_OFFSET_FIXED_FALSE indicates dmaOffset input will be ignored.
//
// 2. For CTXDMA mappings that use DMA_UNICAST_REUSE_ALLOC_TRUE and
//    for *ALL* non-CTXDMA mappings, DMA_OFFSET_FIXED_TRUE indicates
//    to treat the input dmaOffset as an absolute virtual address
//    instead of an offset relative to the virtual allocation being
//    mapped into. Whether relative or absolute, the resulting
//    virtual address *must* be contained within the specified
//    virtual allocation.
//
//    Internally, it is also required that the virtual address be aligned
//    to the page size of the mapping (obviously cannot map sub-pages).
//    For client flexibility the physical offset does not require page alignment.
//    This is handled by adding the physical misalignment
//    (internally called pteAdjust) to the returned virtual address.
//    The *input* dmaOffset can account for this pteAdjust (or not),
//    but the returned virtual address always will.
//
#define NVOS46_FLAGS_DMA_OFFSET_FIXED                              15:15
#define NVOS46_FLAGS_DMA_OFFSET_FIXED_FALSE                        (0x00000000)
#define NVOS46_FLAGS_DMA_OFFSET_FIXED_TRUE                         (0x00000001)

#define NVOS46_FLAGS_PTE_COALESCE_LEVEL_CAP                        19:16
#define NVOS46_FLAGS_PTE_COALESCE_LEVEL_CAP_DEFAULT                (0x00000000)
#define NVOS46_FLAGS_PTE_COALESCE_LEVEL_CAP_1                      (0x00000001)
#define NVOS46_FLAGS_PTE_COALESCE_LEVEL_CAP_2                      (0x00000002)
#define NVOS46_FLAGS_PTE_COALESCE_LEVEL_CAP_4                      (0x00000003)
#define NVOS46_FLAGS_PTE_COALESCE_LEVEL_CAP_8                      (0x00000004)
#define NVOS46_FLAGS_PTE_COALESCE_LEVEL_CAP_16                     (0x00000005)
#define NVOS46_FLAGS_PTE_COALESCE_LEVEL_CAP_32                     (0x00000006)
#define NVOS46_FLAGS_PTE_COALESCE_LEVEL_CAP_64                     (0x00000007)
#define NVOS46_FLAGS_PTE_COALESCE_LEVEL_CAP_128                    (0x00000008)
#define NVOS46_FLAGS_P2P                                           27:20

#define NVOS46_FLAGS_P2P_ENABLE                                    21:20
#define NVOS46_FLAGS_P2P_ENABLE_NO                                 (0x00000000)
#define NVOS46_FLAGS_P2P_ENABLE_YES                                (0x00000001)
#define NVOS46_FLAGS_P2P_ENABLE_NONE                               NVOS46_FLAGS_P2P_ENABLE_NO
#define NVOS46_FLAGS_P2P_ENABLE_SLI                                NVOS46_FLAGS_P2P_ENABLE_YES
#define NVOS46_FLAGS_P2P_ENABLE_NOSLI                              (0x00000002)
// Subdevice ID. Reserved 3 bits for the possibility of 8-way SLI
#define NVOS46_FLAGS_P2P_SUBDEVICE_ID                              24:22
#define NVOS46_FLAGS_P2P_SUBDEV_ID_SRC                             NVOS46_FLAGS_P2P_SUBDEVICE_ID
#define NVOS46_FLAGS_P2P_SUBDEV_ID_TGT                             27:25
#define NVOS46_FLAGS_TLB_LOCK                                      28:28
#define NVOS46_FLAGS_TLB_LOCK_DISABLE                              (0x00000000)
#define NVOS46_FLAGS_TLB_LOCK_ENABLE                               (0x00000001)
#define NVOS46_FLAGS_DMA_UNICAST_REUSE_ALLOC                       29:29
#define NVOS46_FLAGS_DMA_UNICAST_REUSE_ALLOC_FALSE                 (0x00000000)
#define NVOS46_FLAGS_DMA_UNICAST_REUSE_ALLOC_TRUE                  (0x00000001)
//
// This flag must be used with caution. Improper use can leave stale entries in the TLB,
// and allow access to memory no longer owned by the RM client or cause page faults.
// Also see corresponding flag for NvUnmapMemoryDma.
//
#define NVOS46_FLAGS_DEFER_TLB_INVALIDATION                        31:31
#define NVOS46_FLAGS_DEFER_TLB_INVALIDATION_FALSE                  (0x00000000)
#define NVOS46_FLAGS_DEFER_TLB_INVALIDATION_TRUE                   (0x00000001)

/* parameters */
typedef struct
{
    NvHandle hClient;                // [IN] client handle
    NvHandle hDevice;                // [IN] device handle for mapping
    NvHandle hDma;                   // [IN] dma handle for mapping
    NvHandle hMemory;                // [IN] memory handle for mapping
    NvU64    offset NV_ALIGN_BYTES(8);     // [IN] offset of region
    NvU64    length NV_ALIGN_BYTES(8);     // [IN] limit of region
    NvV32    flags;                  // [IN] flags
    NvU64    dmaOffset NV_ALIGN_BYTES(8);  // [OUT] offset of mapping
                                           // [IN] if FLAGS_DMA_OFFSET_FIXED_TRUE
                                           //      *OR* hDma is NOT a CTXDMA handle
                                           //      (see NVOS46_FLAGS_DMA_OFFSET_FIXED)
    NvV32    status;                 // [OUT] status
} NVOS46_PARAMETERS;


/* function OS47 */
#define NV04_UNMAP_MEMORY_DMA                                      (0x0000002F)

#define NVOS47_FLAGS_DEFER_TLB_INVALIDATION                        0:0
#define NVOS47_FLAGS_DEFER_TLB_INVALIDATION_FALSE                  (0x00000000)
#define NVOS47_FLAGS_DEFER_TLB_INVALIDATION_TRUE                   (0x00000001)

/* parameters */
typedef struct
{
    NvHandle hClient;                // [IN] client handle
    NvHandle hDevice;                // [IN] device handle for mapping
    NvHandle hDma;                   // [IN] dma handle for mapping
    NvHandle hMemory;                // [IN] memory handle for mapping
    NvV32    flags;                  // [IN] flags
    NvU64    dmaOffset NV_ALIGN_BYTES(8);  // [IN] dma offset from NV04_MAP_MEMORY_DMA
    NvU64    size NV_ALIGN_BYTES(8);       // [IN] size to unmap, 0 to unmap entire mapping
    NvV32    status;                 // [OUT] status
} NVOS47_PARAMETERS;


#define NV04_BIND_CONTEXT_DMA                                      (0x00000031)
/* parameters */
typedef struct
{
    NvHandle    hClient;                // [IN] client handle
    NvHandle    hChannel;               // [IN] channel handle for binding
    NvHandle    hCtxDma;                // [IN] ctx dma handle for binding
    NvV32       status;                 // [OUT] status
} NVOS49_PARAMETERS;


/* function OS54 */
#define NV04_CONTROL                                               (0x00000036)

#define NVOS54_FLAGS_NONE                                          (0x00000000)
#define NVOS54_FLAGS_IRQL_RAISED                                   (0x00000001)
#define NVOS54_FLAGS_LOCK_BYPASS                                   (0x00000002)
#define NVOS54_FLAGS_FINN_SERIALIZED                               (0x00000004)

/* parameters */
typedef struct
{
    NvHandle hClient;
    NvHandle hObject;
    NvV32    cmd;
    NvU32    flags;
    NvP64    params NV_ALIGN_BYTES(8);
    NvU32    paramsSize;
    NvV32    status;
} NVOS54_PARAMETERS;

/* RM Control header
 *
 * Replacement for NVOS54_PARAMETERS where embedded pointers are not allowed.
 * Input layout for user space RM Control calls should be:
 *
 * +--- NVOS63_PARAMETERS ---+--- RM Control parameters ---+
 *
 * NVOS63_PARAMETERS::paramsSize is the size of RM Control parameters
 *
 */
typedef struct
{
    NvHandle hClient;       // [IN]  client handle
    NvHandle hObject;       // [IN]  object handle
    NvV32    cmd;           // [IN]  control command ID
    NvU32    paramsSize;    // [IN]  size in bytes of the RM Control parameters
    NvV32    status;        // [OUT] status
} NVOS63_PARAMETERS;


/* function OS55 */
#define NV04_DUP_OBJECT                                             (0x00000037)

/* parameters */
typedef struct
{
  NvHandle  hClient;                // [IN]  destination client handle
  NvHandle  hParent;                // [IN]  parent of new object
  NvHandle  hObject;                // [INOUT] destination (new) object handle
  NvHandle  hClientSrc;             // [IN]  source client handle
  NvHandle  hObjectSrc;             // [IN]  source (old) object handle
  NvU32     flags;                  // [IN]  flags
  NvU32     status;                 // [OUT] status
} NVOS55_PARAMETERS;

#define NV04_DUP_HANDLE_FLAGS_NONE                                  (0x00000000)
#define NV04_DUP_HANDLE_FLAGS_REJECT_KERNEL_DUP_PRIVILEGE           (0x00000001) // If set, prevents an RM kernel client from duping unconditionally
                                                                                 // NOTE: Do not declare a NV04_DUP_HANDLE_FLAGS_* value of 0x00000008
                                                                                 // until Bug 2859347 is resolved! This is due to conflicting usage
                                                                                 // of RS_RES_DUP_PARAMS_INTERNAL.flags to pass
                                                                                 // NVOS32_ALLOC_INTERNAL_FLAGS_FLA_MEMORY to an object constructor.

/* function OS56 */
#define NV04_UPDATE_DEVICE_MAPPING_INFO                             (0x00000038)

/* parameters */
typedef struct
{
    NvHandle hClient;
    NvHandle hDevice;
    NvHandle hMemory;
    NvP64    pOldCpuAddress NV_ALIGN_BYTES(8);
    NvP64    pNewCpuAddress NV_ALIGN_BYTES(8);
    NvV32    status;
} NVOS56_PARAMETERS;

/* function OS57 */
#define NV04_SHARE                                             (0x0000003E)

/* parameters */
typedef struct
{
  NvHandle        hClient;        // [IN]  owner client handle
  NvHandle        hObject;        // [IN]  resource to share
  RS_SHARE_POLICY sharePolicy;    // [IN]  share policy entry
  NvU32           status;         // [OUT] status
} NVOS57_PARAMETERS;

/* parameters */
typedef struct
{
    NvU32 deviceReference;
    NvU32 head;
    NvU32 state;
    NvU8  forceMonitorState;
    NvU8  bForcePerfBiosLevel;
    NvU8  bIsD3HotTransition;    // [OUT] To tell client if it's a D3Hot transition
    NvU8  bForcePowerStateFail;
    NvU32 errorStatus;           // [OUT] To tell client if there is bubble up errors
    NvU32 fastBootPowerState;
} NVPOWERSTATE_PARAMETERS, *PNVPOWERSTATE_PARAMETERS;

 /***************************************************************************\
|*                          Object Allocation Parameters                     *|
 \***************************************************************************/

// GR engine creation parameters
typedef struct {
    NvU32   version;    // set to 0x2
    NvU32   flags;      // input param from a rm client (no flags are currently defined)
    NvU32   size;       // sizeof(NV_GR_ALLOCATION_PARAMETERS)
    NvU32   caps;       // output param for a rm client - class dependent
} NV_GR_ALLOCATION_PARAMETERS;

//
// NvAlloc parameters for NV03_DEVICE_XX class
//    hClientShare
//      For NV50+ this can be set to virtual address space for this
//      device. On previous chips this field is ignored. There are
//      three possible settings
//          NV01_NULL_OBJECT - Use the default global VA space
//          Handle to current client - Create a new private address space
//          Handle to another client - Attach to other clients address space
//    flags
//          MAP_PTE_GLOBALLY           Deprecated.
//          MINIMIZE_PTETABLE_SIZE     Pass hint to DMA HAL to use partial page
//                                     tables. Depending on allocation pattern
//                                     this may actually use more instance memory.
//          RETRY_PTE_ALLOC_IN_SYS     Fallback to PTEs allocation in sysmem. This
//                                     is now enabled by default.
//          VASPACE_SIZE               Honor vaSpaceSize field.
//
//          MAP_PTE                    Deprecated.
//
//          VASPACE_IS_MIRRORED        This flag will tell RM to create a mirrored
//                                     kernel PDB for the address space associated
//                                     with this device. When this flag is set
//                                     the address space covered by the top PDE
//                                     is restricted and cannot be allocated out of.
//
//
//          VASPACE_BIG_PAGE_SIZE_64k  ***Warning this flag will be deprecated do not use*****
//          VASPACE_BIG_PAGE_SIZE_128k This flag will choose the big page size of the VASPace
//                                     to 64K/128k if the system supports a configurable size.
//                                     If the system does not support a configurable size then
//                                     defaults will be chosen.
//                                     If the user sets both these bits then this API will fail.
//
//          SHARED_MANAGEMENT
//              *** Warning: This will be deprecated - see NV_VASPACE_ALLOCATION_PARAMETERS. ***
//
//
//    hTargetClient/hTargetDevice
//      Deprecated. Can be deleted once client code has removed references.
//
//    vaBase
//        *** Warning: This will be deprecated - see NV_VASPACE_ALLOCATION_PARAMETERS. ***
//
//    vaSpaceSize
//      Set the size of the VA space used for this client if allocating
//      a new private address space. Is expressed as a size such as
//      (1<<32) for a 32b address space. Reducing the size of the address
//      space allows the dma chip specific code to reduce the instance memory
//      used for page tables.
//
//    vaMode
//      The vaspace allocation mode. There are three modes supported:
//      1. SINGLE_VASPACE
//      An old abstraction that provides a single VA space under a
//      device and it's allocated implicityly when an object requires a VA
//      space. Typically, this VA space is also shared across clients.
//
//      2. OPTIONAL_MULTIPLE_VASPACES
//      Global + multiple private va spaces. In this mode, the old abstraction,
//      a single vaspace under a device that is allocated implicitly is still
//      being supported. A private VA space is an entity under a device, which/
//      cannot be shared with other clients, but multiple channels under the
//      same device can still share a private VA space.
//      Private VA spaces (class:90f1,FERMI_VASPACE_A) can be allocated as
//      objects through RM APIs. This mode requires the users to know what they
//      are doing in terms of using VA spaces. Page fault can easily occur if
//      one is not careful with a mixed of an implicit VA space and multiple
//      VA spaces.
//
//      3. MULTIPLE_VASPACES
//      In this mode, all VA spaces have to be allocated explicitly through RM
//      APIs and users have to specify which VA space to use for each object.
//      This case prevents users to use context dma, which is not supported and
//      can be misleading if used. Therefore, it's more a safeguard mode to
//      prevent people making mistakes that are hard to debug.
//
//      DEFAULT MODE: 2. OPTIONAL_MULTIPLE_VASPACES
//
// See NV0080_ALLOC_PARAMETERS for allocation parameter structure.
//

#define NV_DEVICE_ALLOCATION_SZNAME_MAXLEN    128
#define NV_DEVICE_ALLOCATION_FLAGS_NONE                            (0x00000000)
#define NV_DEVICE_ALLOCATION_FLAGS_MAP_PTE_GLOBALLY                (0x00000001)
#define NV_DEVICE_ALLOCATION_FLAGS_MINIMIZE_PTETABLE_SIZE          (0x00000002)
#define NV_DEVICE_ALLOCATION_FLAGS_RETRY_PTE_ALLOC_IN_SYS          (0x00000004)
#define NV_DEVICE_ALLOCATION_FLAGS_VASPACE_SIZE                    (0x00000008)
#define NV_DEVICE_ALLOCATION_FLAGS_MAP_PTE                         (0x00000010)
#define NV_DEVICE_ALLOCATION_FLAGS_VASPACE_IS_TARGET               (0x00000020)
#define NV_DEVICE_ALLOCATION_FLAGS_VASPACE_SHARED_MANAGEMENT       (0x00000100)
#define NV_DEVICE_ALLOCATION_FLAGS_VASPACE_BIG_PAGE_SIZE_64k       (0x00000200)
#define NV_DEVICE_ALLOCATION_FLAGS_VASPACE_BIG_PAGE_SIZE_128k      (0x00000400)
#define NV_DEVICE_ALLOCATION_FLAGS_RESTRICT_RESERVED_VALIMITS      (0x00000800)

/*
 *TODO: Delete this flag once CUDA moves to the ctrl call
 */
#define NV_DEVICE_ALLOCATION_FLAGS_VASPACE_IS_MIRRORED             (0x00000040)

// XXX NV_DEVICE_ALLOCATION_FLAGS_VASPACE_PTABLE_PMA_MANAGED should not
//     should not be exposed to clients. It should be the default RM
//     behavior.
//
//     Until it is made the default, certain clients such as OpenGL
//     might still need PTABLE allocations to go through PMA, so this
//     flag has been temporary exposed.
//
//     See bug 1880192
#define NV_DEVICE_ALLOCATION_FLAGS_VASPACE_PTABLE_PMA_MANAGED      (0x00001000)

//
// Indicates this device is being created by guest and requires a
// KernelHostVgpuDeviceApi creation in client.
//
#define NV_DEVICE_ALLOCATION_FLAGS_HOST_VGPU_DEVICE                (0x00002000)

//
// Indicates this device is being created for VGPU plugin use.
// Requires a HostVgpuDevice handle to indicate the guest on which
// this plugin operates.
//
#define NV_DEVICE_ALLOCATION_FLAGS_PLUGIN_CONTEXT                  (0x00004000)

//
// For clients using unlinked SLI to catch allocations attempts on secondary GPUs
// not accompanied by a fixed offset.
//
#define NV_DEVICE_ALLOCATION_FLAGS_VASPACE_REQUIRE_FIXED_OFFSET    (0x00008000)

#define NV_DEVICE_ALLOCATION_VAMODE_OPTIONAL_MULTIPLE_VASPACES     (0x00000000)
#define NV_DEVICE_ALLOCATION_VAMODE_SINGLE_VASPACE                 (0x00000001)
#define NV_DEVICE_ALLOCATION_VAMODE_MULTIPLE_VASPACES              (0x00000002)


#define NV_CHANNELGPFIFO_NOTIFICATION_TYPE_ERROR                0x00000000
#define NV_CHANNELGPFIFO_NOTIFICATION_TYPE_WORK_SUBMIT_TOKEN    0x00000001
#define NV_CHANNELGPFIFO_NOTIFICATION_TYPE_KEY_ROTATION_STATUS  0x00000002
#define NV_CHANNELGPFIFO_NOTIFICATION_TYPE__SIZE_1              3
#define NV_CHANNELGPFIFO_NOTIFICATION_STATUS_VALUE              14:0
#define NV_CHANNELGPFIFO_NOTIFICATION_STATUS_IN_PROGRESS        15:15
#define NV_CHANNELGPFIFO_NOTIFICATION_STATUS_IN_PROGRESS_TRUE   0x1
#define NV_CHANNELGPFIFO_NOTIFICATION_STATUS_IN_PROGRESS_FALSE  0x0

typedef struct
{
    NvV32    channelInstance;            // One of the n channel instances of a given channel type.
                                         // Note that core channel has only one instance
                                         // while all others have two (one per head).
    NvHandle hObjectBuffer;              // ctx dma handle for DMA push buffer
    NvHandle hObjectNotify;              // ctx dma handle for an area (of type NvNotification defined in sdk/nvidia/inc/nvtypes.h) where RM can write errors/notifications
    NvU32    offset;                     // Initial offset for put/get, usually zero.
    NvP64    pControl NV_ALIGN_BYTES(8); // pControl gives virt addr of UDISP GET/PUT regs

    NvU32    flags;
#define NV50VAIO_CHANNELDMA_ALLOCATION_FLAGS_CONNECT_PB_AT_GRAB                1:1
#define NV50VAIO_CHANNELDMA_ALLOCATION_FLAGS_CONNECT_PB_AT_GRAB_YES            0x00000000
#define NV50VAIO_CHANNELDMA_ALLOCATION_FLAGS_CONNECT_PB_AT_GRAB_NO             0x00000001

} NV50VAIO_CHANNELDMA_ALLOCATION_PARAMETERS;

typedef struct
{
    NvV32    channelInstance;            // One of the n channel instances of a given channel type.
                                         // All PIO channels have two instances (one per head).
    NvHandle hObjectNotify;              // ctx dma handle for an area (of type NvNotification defined in sdk/nvidia/inc/nvtypes.h) where RM can write errors.
    NvP64    pControl NV_ALIGN_BYTES(8); // pControl gives virt addr of control region for PIO channel
} NV50VAIO_CHANNELPIO_ALLOCATION_PARAMETERS;

// Used for allocating a channel group
typedef struct
{
    NvHandle hObjectError;               // Error notifier for TSG
    NvHandle hObjectEccError;            // ECC Error notifier for TSG
    NvHandle hVASpace;                   // VA space handle for TSG
    NvU32    engineType;                 // Engine to which all channels in this TSG are associated with
    NvBool   bIsCallingContextVgpuPlugin;
} NV_CHANNEL_GROUP_ALLOCATION_PARAMETERS;

/*
* @params:
* @engineId         : Engine to which the software runlist be associated with.
* @maxTSGs          : Maximum number of TSG entries that will be submitted in this software runlist
*                     The size of the runlist buffer will be determined by
*                     2 *                  // double buffer
*                     maxTSGs           *  // determined by KMD
*                     maxChannelPerTSG  *  // Determined by RM
*                     sizeof(RunlistEntry) // Determined by HW format
* @qosIntrEnableMask: QOS Interrupt bitmask that needs to be enabled for the SW runlist defined below.
*/
typedef struct
{
    NvU32    engineId;          //(IN)
    NvU32    maxTSGs;           //(IN)  // Size of the RM could return error if the request cannot be accommodated.
    NvU32    qosIntrEnableMask; //(IN)  // Bitmask for QOS interrupts that needs to be enabled
} NV_SWRUNLIST_ALLOCATION_PARAMS;

#define NV_SWRUNLIST_QOS_INTR_NONE                                   0x00000000
#define NV_SWRUNLIST_QOS_INTR_RUNLIST_AND_ENG_IDLE_ENABLE            NVBIT32(0)
#define NV_SWRUNLIST_QOS_INTR_RUNLIST_IDLE_ENABLE                    NVBIT32(1)
#define NV_SWRUNLIST_QOS_INTR_RUNLIST_ACQUIRE_ENABLE                 NVBIT32(2)
#define NV_SWRUNLIST_QOS_INTR_RUNLIST_ACQUIRE_AND_ENG_IDLE_ENABLE    NVBIT32(3)

typedef struct
{
    NvU32 size;
    NvU32 caps;
} NV_ME_ALLOCATION_PARAMETERS;

typedef struct
{
    NvU32 size;
    NvU32 prohibitMultipleInstances;
    NvU32 engineInstance;               // Select NVDEC0 or NVDEC1 or NVDEC2
} NV_BSP_ALLOCATION_PARAMETERS;

//
// These are referenced by mdiag mods tests, but do not appear to be used during
// in the RM any longer
//
#define  NV_VP_ALLOCATION_FLAGS_STANDARD_UCODE                               (0x00000000)
#define  NV_VP_ALLOCATION_FLAGS_STATIC_UCODE                                 (0x00000001)
#define  NV_VP_ALLOCATION_FLAGS_DYNAMIC_UCODE                                (0x00000002)

//
// NV_VP_ALLOCATION_PARAMETERS.flags
//
// NV_VP_ALLOCATION_FLAGS_AVP_CLIENT are used by Tegra to specify if
// the current allocation with be used by Video or Audio
//
#define  NV_VP_ALLOCATION_FLAGS_AVP_CLIENT_VIDEO            (0x00000000)
#define  NV_VP_ALLOCATION_FLAGS_AVP_CLIENT_AUDIO            (0x00000001)

typedef struct
{
    NvU32       size;
    NvU32       caps;
    NvU32       flags;
    NvU32       altUcode;
    NvP64       rawUcode            NV_ALIGN_BYTES(8);
    NvU32       rawUcodeSize;
    NvU32       numSubClasses;
    NvU32       numSubSets;
    NvP64       subClasses          NV_ALIGN_BYTES(8);
    NvU32       prohibitMultipleInstances;
    NvP64       pControl            NV_ALIGN_BYTES(8);  // Used by Tegra to return a mapping to NvE276Control
    NvHandle    hMemoryCmdBuffer    NV_ALIGN_BYTES(8);  // Used by Tegra to specify cmd buffer
    NvU64       offset              NV_ALIGN_BYTES(8);  // Used by Tegra to specify an offset into the cmd buffer

} NV_VP_ALLOCATION_PARAMETERS;

typedef struct
{
    NvU32 size;
    NvU32 prohibitMultipleInstances;
} NV_PPP_ALLOCATION_PARAMETERS;

typedef struct
{
    NvU32 size;
    NvU32 prohibitMultipleInstances;  // Prohibit multiple allocations of MSENC?
    NvU32 engineInstance;             // Select MSENC/NVENC0 or NVENC1 or NVENC2
} NV_MSENC_ALLOCATION_PARAMETERS;

typedef struct
{
    NvU32 size;
    NvU32 prohibitMultipleInstances;  // Prohibit multiple allocations of SEC2?
} NV_SEC2_ALLOCATION_PARAMETERS;

typedef struct
{
    NvU32 size;
    NvU32 prohibitMultipleInstances;  // Prohibit multiple allocations of NVJPG?
    NvU32 engineInstance;
} NV_NVJPG_ALLOCATION_PARAMETERS;

typedef struct
{
    NvU32 size;
    NvU32 prohibitMultipleInstances;  // Prohibit multiple allocations of OFA?
    NvU32 engineInstance;
} NV_OFA_ALLOCATION_PARAMETERS;

#define NV04_ADD_VBLANK_CALLBACK                          (0x0000003D)

#include "class/cl9010.h" // for OSVBLANKCALLBACKPROC

/* parameters */
/* NOTE: the "void* pParm's" below are ok (but unfortunate) since this interface
   can only be used by other kernel drivers which must share the same ptr-size */
typedef struct
{
    NvHandle             hClient;     // [IN] client handle
    NvHandle             hDevice;     // [IN] device handle for mapping
    NvHandle             hVblank;     // [IN] Vblank handle for control
    OSVBLANKCALLBACKPROC pProc;       // Routine to call at vblank time

    NvV32                LogicalHead; // Logical Head
    void                *pParm1;
    void                *pParm2;
    NvU32                bAdd;        // Add or Delete
    NvV32                status;      // [OUT] status
} NVOS61_PARAMETERS;

/**
 * @brief NvAlloc parameters for VASPACE classes
 *
 * Used to create a new private virtual address space.
 *
 * index
 *       Tegra: With TEGRA_VASPACE_A, index specifies the IOMMU
 *       virtual address space to be created. Based on the
 *       index, RM/NVMEM will decide the HW ASID to be used with
 *       this VA Space. "index" takes values from the
 *       NVMEM_CLIENT_* defines in
 *       "drivers/common/inc/tegra/memory/ioctl.h".
 *
 *       Big GPU: With FERMI_VASPACE_A, see NV_VASPACE_ALLOCATION_INDEX_GPU_*.
 *
 * flags
 *       MINIMIZE_PTETABLE_SIZE Pass hint to DMA HAL to use partial page tables.
 *                              Depending on allocation pattern this may actually
 *                              use more instance memory.
 *
 *       RETRY_PTE_ALLOC_IN_SYS Fallback to PTEs allocation in sysmem. This is now
 *       enabled by default.
 *
 *       SHARED_MANAGEMENT
 *          Indicates management of the VA space is shared with another
 *          component (e.g. driver layer, OS, etc.).
 *
 *          The initial VA range from vaBase (inclusive) through vaSize (exclusive)
 *          is managed by RM. The range must be aligned to a top-level PDE's VA
 *          coverage since backing page table levels for this range are managed by RM.
 *          All normal RM virtual memory management APIs work within this range.
 *
 *          An external component can manage the remaining VA ranges,
 *          from 0 (inclusive) to vaBase (exclusive) and from vaSize (inclusive) up to the
 *          maximum VA limit supported by HW.
 *          Management of these ranges includes VA sub-allocation and the
 *          backing lower page table levels.
 *
 *          The top-level page directory is special since it is a shared resource.
 *          Management of the page directory is as follows:
 *          1. Initially RM allocates a page directory for RM-managed PDEs.
 *          2. The external component may create a full page directory and commit it
 *             with NV0080_CTRL_CMD_DMA_SET_PAGE_DIRECTORY.
 *             This will copy the RM-managed PDEs from the RM-managed page directory
 *             into the external page directory and commit channels to the external page directory.
 *             After this point RM will update the external page directory directly for
 *             operations that modify RM-managed PDEs.
 *          3. The external component may use NV0080_CTRL_CMD_DMA_SET_PAGE_DIRECTORY repeatedly
 *             if it needs to update the page directory again (e.g. to resize or migrate).
 *             This will copy the RM-managed PDEs from the old external page directory
 *             into the new external page directory and commit channels to the new page directory.
 *          4. The external component may restore management of the page directory back to
 *             RM with NV0080_CTRL_CMD_DMA_UNSET_PAGE_DIRECTORY.
 *             This will copy the RM-managed PDEs from the external page directory
 *             into the RM-managed page directory and commit channels to the RM-managed page directory.
 *             After this point RM will update the RM-managed page directory for
 *             operations that modify RM-managed PDEs.
 *          Note that operations (2) and (4) are symmetric - the RM perspective of management is identical
 *          before and after a sequence of SET => ... => UNSET.
 *
 *       IS_MIRRORED      <to be deprecated once CUDA uses EXTERNALLY_MANAGED>
 *                        This flag will tell RM to create a mirrored
 *                        kernel PDB for the address space associated
 *                        with this device. When this flag is set
 *                        the address space covered by the top PDE
 *                        is restricted and cannot be allocated out of.
 *       ENABLE_PAGE_FAULTING
 *                        Enable page faulting if the architecture supports it.
 *                        As of now page faulting is only supported for compute on pascal+.
 *       IS_EXTERNALLY_OWNED
 *                        This vaspace that has been allocated will be managed by
 *                        an external driver. RM will not own the pagetables for this vaspace.
 *
 *       ENABLE_NVLINK_ATS
 *                        Enables VA translation for this address space using NVLINK ATS.
 *                        Note, the GMMU page tables still exist and take priority over NVLINK ATS.
 *                        VA space object creation will fail if:
 *                        - hardware support is not available (NV_ERR_NOT_SUPPORTED)
 *                        - incompatible options IS_MIRRORED or IS_EXTERNALLY_OWNED are set (NV_ERR_INVALID_ARGUMENT)
 *       IS_FLA
 *                        Sets FLA flag for this VASPACE
 *
 *       ALLOW_ZERO_ADDRESS
 *                        Allows VASPACE Range to start from zero
 *       SKIP_SCRUB_MEMPOOL
 *                        Skip scrubbing in MemPool
 *
 * vaBase [in, out]
 *       On input, the lowest usable base address of the VA space.
 *       If 0, RM will pick a default value - 0 is always reserved to respresent NULL pointers.
 *       The value must be aligned to the largest page size of the VA space.
 *       Larger values aid in debug since offsets added to NULL pointers will still fault.
 *
 *       On output, the actual usable base address is returned.
 *
 * vaSize [in,out]
 *       On input, requested size of the virtual address space in bytes.
 *       Requesting a smaller size reduces the memory required for the initial
 *       page directory, but the VAS may be resized later (NV0080_CTRL_DMA_SET_VA_SPACE_SIZE).
 *       If 0, the default VA space size will be used.
 *
 *       On output, the actual size of the VAS in bytes.
 *       NOTE: This corresponds to the VA_LIMIT + 1, so the usable size is (vaSize - vaBase).
 *
 * bigPageSIze
 *       Set the size of the big page in this address space object. Current HW supports
 *       either 64k or 128k as the size of the big page. HW that support multiple big
 *       page size per address space will use this size. Hw that do not support this feature
 *       will override to the default big page size that is supported by the system.
 *       If the big page size value is set to ZERO then we will pick the default page size
 *       of the system.
 **/
typedef struct
{
    NvU32   index;
    NvV32   flags;
    NvU64   vaSize NV_ALIGN_BYTES(8);
    NvU64   vaStartInternal NV_ALIGN_BYTES(8);
    NvU64   vaLimitInternal NV_ALIGN_BYTES(8);
    NvU32   bigPageSize;
    NvU64   vaBase NV_ALIGN_BYTES(8);
} NV_VASPACE_ALLOCATION_PARAMETERS;

#define NV_VASPACE_ALLOCATION_FLAGS_NONE                            (0x00000000)
#define NV_VASPACE_ALLOCATION_FLAGS_MINIMIZE_PTETABLE_SIZE                BIT(0)
#define NV_VASPACE_ALLOCATION_FLAGS_RETRY_PTE_ALLOC_IN_SYS                BIT(1)
#define NV_VASPACE_ALLOCATION_FLAGS_SHARED_MANAGEMENT                     BIT(2)
#define NV_VASPACE_ALLOCATION_FLAGS_IS_EXTERNALLY_OWNED                   BIT(3)
#define NV_VASPACE_ALLOCATION_FLAGS_ENABLE_NVLINK_ATS                     BIT(4)
#define NV_VASPACE_ALLOCATION_FLAGS_IS_MIRRORED                           BIT(5)
#define NV_VASPACE_ALLOCATION_FLAGS_ENABLE_PAGE_FAULTING                  BIT(6)
#define NV_VASPACE_ALLOCATION_FLAGS_VA_INTERNAL_LIMIT                     BIT(7)
#define NV_VASPACE_ALLOCATION_FLAGS_ALLOW_ZERO_ADDRESS                    BIT(8)
#define NV_VASPACE_ALLOCATION_FLAGS_IS_FLA                                BIT(9)
#define NV_VASPACE_ALLOCATION_FLAGS_SKIP_SCRUB_MEMPOOL                    BIT(10)
#define NV_VASPACE_ALLOCATION_FLAGS_OPTIMIZE_PTETABLE_MEMPOOL_USAGE       BIT(11)
#define NV_VASPACE_ALLOCATION_FLAGS_REQUIRE_FIXED_OFFSET                  BIT(12)
#define NV_VASPACE_ALLOCATION_FLAGS_PTETABLE_HEAP_MANAGED                 BIT(13)

#define NV_VASPACE_ALLOCATION_INDEX_GPU_NEW                                 0x00 //<! Create new VASpace, by default
#define NV_VASPACE_ALLOCATION_INDEX_GPU_HOST                                0x01 //<! Acquire reference to BAR1 VAS.
#define NV_VASPACE_ALLOCATION_INDEX_GPU_GLOBAL                              0x02 //<! Acquire reference to global VAS.
#define NV_VASPACE_ALLOCATION_INDEX_GPU_DEVICE                              0x03 //<! Acquire reference to device vaspace
#define NV_VASPACE_ALLOCATION_INDEX_GPU_FLA                                 0x04 //<! Acquire reference to FLA VAS.
#define NV_VASPACE_ALLOCATION_INDEX_GPU_MAX                                 0x05 //<! Increment this on adding index entries


#define NV_VASPACE_BIG_PAGE_SIZE_64K                                (64 * 1024)
#define NV_VASPACE_BIG_PAGE_SIZE_128K                               (128 * 1024)

/**
 * @brief NvAlloc parameters for FERMI_CONTEXT_SHARE_A class
 *
 * Used to create a new context share object for use by a TSG channel.
 * Context share is now used to represent a subcontext within a TSG.
 * Refer subcontexts-rm-design.docx for more details.
 *
 * hVASpace
 *          Handle of VA Space object associated with the context share.
 *          All channels using the same using the context share the same va space.
 *
 * flags
 *          Options for the context share allocation.
 *
 *          NV_CTXSHARE_ALLOCATION_FLAGS_SUBCONTEXT
 *              Used to specify the subcontext slot
 *              SYNC
 *                  Use synchronous graphics & compute subcontext
 *                  In VOLTA+ chips, this represent VEID 0
 *                  In pre-VOLTA chips, this represent SCG type 0
 *              ASYNC
 *                  Use asynchronous compute subcontext
 *                  In VOLTA+ chips, this represent a VEID greater than 0
 *                  In pre-VOLTA chips, this represent SCG type 1
 *              SPECIFIED
 *                  Force the VEID specified in the subctxId parameter.
 *                  This flag is intended for verif. i.e testing VEID reuse etc.
 *
 * subctxId
 *          As input, it is used to specify the subcontext ID, when the _SPECIFIED flag is set.
 *          As output, it is used to return the subcontext ID allocated by RM.
 *          This field is intended for verif.
 **/

typedef struct
{
    NvHandle hVASpace;
    NvU32    flags;
    NvU32    subctxId;
} NV_CTXSHARE_ALLOCATION_PARAMETERS;

#define NV_CTXSHARE_ALLOCATION_FLAGS_SUBCONTEXT                      1:0
#define NV_CTXSHARE_ALLOCATION_FLAGS_SUBCONTEXT_SYNC                 (0x00000000)
#define NV_CTXSHARE_ALLOCATION_FLAGS_SUBCONTEXT_ASYNC                (0x00000001)
#define NV_CTXSHARE_ALLOCATION_FLAGS_SUBCONTEXT_SPECIFIED            (0x00000002)

/**
 * @brief RmTimeoutControl parameters
 *
 * Used to set various timeout-related features in RM.
 *
 * cmd
 *   The timeout-related command to issue to RM.
 *
 * value
 *   Used by command, such as the timeout to be set, in milliseconds.
 **/

typedef struct
{
    NvU32 cmd;
    NvU32 timeoutInMs;
    NvU32 deviceInstance;
} NV_TIMEOUT_CONTROL_PARAMETERS;

#define NV_TIMEOUT_CONTROL_CMD_SET_DEVICE_TIMEOUT                   (0x00000002)
#define NV_TIMEOUT_CONTROL_CMD_RESET_DEVICE_TIMEOUT                 (0x00000003)

// NV_TIMEOUT_CONTROL_CMD_SET_DEVICE_TIMEOUT sets a maximum timeout value for
// any RM call on a specific device on any thread. It uses 'timeoutInMs'
// as the target timeout and 'deviceInstance' as the target device.

// NV_TIMEOUT_CONTROL_CMD_RESET_DEVICE_TIMEOUT resets the device timeout to its
// default value. It uses 'deviceInstance' as the target device.

/*
 * NV_VIDMEM_ACCESS_BIT_BUFFER_ADDR_SPACE_COH
 * NV_VIDMEM_ACCESS_BIT_BUFFER_ADDR_SPACE_DEFAULT
 *           Location is Coherent System memory (also the default option)
 * NV_VIDMEM_ACCESS_BIT_BUFFER_ADDR_SPACE_NCOH
 *           Location is Non-Coherent System memory
 * NV_VIDMEM_ACCESS_BIT_BUFFER_ADDR_SPACE_VID
 *           Location is FB
 *
 * Currently only used by MODS for the V1 VAB interface. To be deleted.
 */
typedef enum
{
    NV_VIDMEM_ACCESS_BIT_BUFFER_ADDR_SPACE_DEFAULT = 0,
    NV_VIDMEM_ACCESS_BIT_BUFFER_ADDR_SPACE_COH,
    NV_VIDMEM_ACCESS_BIT_BUFFER_ADDR_SPACE_NCOH,
    NV_VIDMEM_ACCESS_BIT_BUFFER_ADDR_SPACE_VID
} NV_VIDMEM_ACCESS_BIT_ALLOCATION_PARAMS_ADDR_SPACE;

/**
 * @brief Multiclient vidmem access bit allocation params
 */
typedef struct
{
    /* [OUT] Dirty/Access tracking */
    NvBool bDirtyTracking;
    /* [OUT] Current tracking granularity */
    NvU32 granularity;
    /* [OUT] 512B Access bit mask with 1s set on
       bits that are reserved for this client */
    NV_DECLARE_ALIGNED(NvU64 accessBitMask[64], 8);
    /* Number of entries of vidmem access buffer. Used by VAB v1 - to be deleted */
    NvU32 noOfEntries;
    /* Address space of the vidmem access bit buffer. Used by VAB v1 - to be deleted */
    NV_VIDMEM_ACCESS_BIT_ALLOCATION_PARAMS_ADDR_SPACE addrSpace;
} NV_VIDMEM_ACCESS_BIT_ALLOCATION_PARAMS;

/**
 * @brief HopperUsermodeAParams
 * This set of optionalparameters is passed in on allocation of
 * HOPPER_USERMODE_A object to specify whether a BAR1/GMMU
 * privileged/non-privileged mapping is needed.
 */

typedef struct
{
    /**
     * [IN] Whether to allocate GMMU/BAR1 mapping or BAR0 mapping.
     */
    NvBool bBar1Mapping;
    /* [IN] Whether to allocate the PRIV page or regular VF page */
    NvBool bPriv;
} NV_HOPPER_USERMODE_A_PARAMS;


#ifdef __cplusplus
};
#endif
#endif /* NVOS_INCLUDED */
