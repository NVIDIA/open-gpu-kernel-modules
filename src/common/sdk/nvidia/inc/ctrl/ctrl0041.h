/*
 * SPDX-FileCopyrightText: Copyright (c) 2004-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrl0041.finn
//

#include "nvos.h"
#include "ctrl/ctrlxxxx.h"
/* NV04_MEMORY control commands and parameters */

#define NV0041_CTRL_CMD(cat,idx)          NVXXXX_CTRL_CMD(0x0041, NV0041_CTRL_##cat, idx)

/* NV04_MEMORY command categories (6bits) */
#define NV0041_CTRL_RESERVED (0x00)
#define NV0041_CTRL_MEMORY   (0x01)

/*
 * NV0041_CTRL_CMD_NULL
 *
 * This command does nothing.
 * This command does not take any parameters.
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NV0041_CTRL_CMD_NULL (0x410000) /* finn: Evaluated from "(FINN_NV01_ROOT_USER_RESERVED_INTERFACE_ID << 8) | 0x0" */



/*
 * NV0041_CTRL_CMD_GET_SURFACE_PHYS_ATTR
 *
 * This command returns attributes associated with the memory object
 * at the given offset. The architecture dependent return parameter
 * comprFormat determines the meaningfulness (or not) of comprOffset.
 *
 * This call is only currently supported in the MODS environment.
 *
 *   memOffset
 *     This parameter is both an input and an output. As input, this
 *     parameter holds an offset into the memory surface. The return
 *     value is the physical address of the surface at the given offset.
 *   memFormat
 *     This parameter returns the memory kind of the surface.
 *   comprOffset
 *     This parameter returns the compression offset of the surface.
 *   comprFormat
 *     This parameter returns the type of compression of the surface.
 *   memAperture
 *     The aperture of the surface is returned in this field.
 *     Legal return values for this parameter are
 *       NV0041_CTRL_CMD_GET_SURFACE_PHYS_ATTR_APERTURE_VIDMEM
 *       NV0041_CTRL_CMD_GET_SURFACE_PHYS_ATTR_APERTURE_SYSMEM
 *   gpuCacheAttr
 *     gpuCacheAttr returns the gpu cache attribute of the surface.
 *     Legal return values for this field are
 *       NV0041_CTRL_GET_SURFACE_PHYS_ATTR_GPU_CACHED_UNKNOWN
 *       NV0041_CTRL_GET_SURFACE_PHYS_ATTR_GPU_CACHED
 *       NV0041_CTRL_GET_SURFACE_PHYS_ATTR_GPU_UNCACHED
 *   gpuP2PCacheAttr
 *     gpuP2PCacheAttr returns the gpu peer-to-peer cache attribute of the surface.
 *     Legal return values for this field are
 *       NV0041_CTRL_GET_SURFACE_PHYS_ATTR_GPU_CACHED_UNKNOWN
 *       NV0041_CTRL_GET_SURFACE_PHYS_ATTR_GPU_CACHED
 *       NV0041_CTRL_GET_SURFACE_PHYS_ATTR_GPU_UNCACHED
 *   mmuContext
 *     mmuContext indicates the type of physical address to be returned (input parameter).
 *     Legal return values for this field are
 *       TEGRA_VASPACE_A  --  return the device physical address for Tegra (non-GPU) engines. This is the system physical address itself.
 *                            returns the system physical address. This may change to use a class value in future.
 *       FERMI_VASPACE_A  --  return the device physical address for GPU engines. This can be a system physical address or a GPU SMMU virtual address.
 *                     0  --  return the device physical address for GPU engines. This can be a system physical address or a GPU SMMU virtual address.
 *                            use of zero may be deprecated in future.
 *   contigSegmentSize
 *     If the underlying surface is physically contiguous, this parameter
 *     returns the size in bytes of the piece of memory starting from
 *     the offset specified in the memOffset parameter extending to the last
 *     byte of the surface.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NVOS_STATUS_BAD_OBJECT_HANDLE
 *   NVOS_STATUS_BAD_OBJECT_PARENT
 *   NVOS_STATUS_NOT_SUPPORTED
 *
 */
#define NV0041_CTRL_CMD_GET_SURFACE_PHYS_ATTR (0x410103) /* finn: Evaluated from "(FINN_NV01_ROOT_USER_MEMORY_INTERFACE_ID << 8) | NV0041_CTRL_GET_SURFACE_PHYS_ATTR_PARAMS_MESSAGE_ID" */

#define NV0041_CTRL_GET_SURFACE_PHYS_ATTR_PARAMS_MESSAGE_ID (0x3U)

typedef struct NV0041_CTRL_GET_SURFACE_PHYS_ATTR_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 memOffset, 8);
    NvU32 memFormat;
    NvU32 comprOffset;
    NvU32 comprFormat;
    NvU32 memAperture;
    NvU32 gpuCacheAttr;
    NvU32 gpuP2PCacheAttr;
    NvU32 mmuContext;
    NV_DECLARE_ALIGNED(NvU64 contigSegmentSize, 8);
} NV0041_CTRL_GET_SURFACE_PHYS_ATTR_PARAMS;

/* valid memAperture return values */
#define NV0041_CTRL_CMD_GET_SURFACE_PHYS_ATTR_APERTURE_VIDMEM (0x00000000)
#define NV0041_CTRL_CMD_GET_SURFACE_PHYS_ATTR_APERTURE_SYSMEM (0x00000001)

/* valid gpuCacheAttr return values */
#define NV0041_CTRL_GET_SURFACE_PHYS_ATTR_GPU_CACHED_UNKNOWN  (0x00000000)
#define NV0041_CTRL_GET_SURFACE_PHYS_ATTR_GPU_CACHED          (0x00000001)
#define NV0041_CTRL_GET_SURFACE_PHYS_ATTR_GPU_UNCACHED        (0x00000002)

/*
 * NV0041_CTRL_CMD_GET_SURFACE_ZCULL_ID
 *
 * This command returns the Z-cull identifier for a surface.
 * The value of ~0 is returned if there is none associated.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NVOS_STATUS_BAD_OBJECT_HANDLE
 *   NVOS_STATUS_BAD_OBJECT_PARENT
 *   NVOS_STATUS_NOT_SUPPORTED
 *
 */
#define NV0041_CTRL_CMD_GET_SURFACE_ZCULL_ID                  (0x410104) /* finn: Evaluated from "(FINN_NV01_ROOT_USER_MEMORY_INTERFACE_ID << 8) | NV0041_CTRL_GET_SURFACE_ZCULL_ID_PARAMS_MESSAGE_ID" */

#define NV0041_CTRL_GET_SURFACE_ZCULL_ID_PARAMS_MESSAGE_ID (0x4U)

typedef struct NV0041_CTRL_GET_SURFACE_ZCULL_ID_PARAMS {
    NvU32 zcullId;
} NV0041_CTRL_GET_SURFACE_ZCULL_ID_PARAMS;


// return values for 'tilingFormat'
// XXX - the names for these are misleading
#define NV0041_CTRL_CMD_GET_SURFACE_TILING_FORMAT_INVALID   (0x00000000)
#define NV0041_CTRL_CMD_GET_SURFACE_TILING_FORMAT_FB        (0x00000001)
#define NV0041_CTRL_CMD_GET_SURFACE_TILING_FORMAT_FB_1HIGH  (0x00000002)
#define NV0041_CTRL_CMD_GET_SURFACE_TILING_FORMAT_FB_4HIGH  (0x00000003)
#define NV0041_CTRL_CMD_GET_SURFACE_TILING_FORMAT_UMA_1HIGH (0x00000004)
#define NV0041_CTRL_CMD_GET_SURFACE_TILING_FORMAT_UMA_4HIGH (0x00000005)

/*
 * NV0041_CTRL_SURFACE_INFO
 *
 * This structure represents a single 32bit surface value.  Clients
 * request a particular surface value by specifying a unique surface
 * information index.
 *
 * Legal surface information index values are:
 *   NV0041_CTRL_SURFACE_INFO_INDEX_ATTRS
 *     This index is used to request the set of hw attributes associated
 *     with the surface.  Each distinct attribute is represented by a
 *     single bit flag in the returned value.
 *     Legal flags values for this index are:
 *       NV0041_CTRL_SURFACE_INFO_ATTRS_COMPR
 *         This surface has compression resources bound to it.
 *       NV0041_CTRL_SURFACE_INFO_ATTRS_ZCULL
 *         This surface has zcull resources bound to it.
 *   NV0041_CTRL_SURFACE_INFO_INDEX_COMPR_COVERAGE
 *     This index is used to request the compression coverage (if any)
 *     in units of 64K for the associated surface.  A value of zero indicates
 *     there are no compression resources associated with the surface.
 *     Legal return values range from zero to a maximum number of 64K units
 *     that is GPU implementation dependent.
 *   NV0041_CTRL_SURFACE_INFO_INDEX_PHYS_SIZE
 *     This index is used to request the physically allocated size in units
 *     of 4K(NV0041_CTRL_SURFACE_INFO_PHYS_SIZE_SCALE_FACTOR) for the associated
 *     surface.
 *   NV0041_CTRL_SURFACE_INFO_INDEX_PHYS_ATTR
 *     This index is used to request the surface attribute field. The returned
 *     field value can be decoded using the NV0041_CTRL_SURFACE_INFO_PHYS_ATTR_*
 *     DRF-style macros provided below.
 *   NV0041_CTRL_SURFACE_INFO_INDEX_ADDR_SPACE_TYPE
 *     This index is used to request the surface address space type.
 *     Returned values are described by NV0000_CTRL_CMD_CLIENT_GET_ADDR_SPACE_TYPE.
 */
typedef NVXXXX_CTRL_XXX_INFO NV0041_CTRL_SURFACE_INFO;

/* valid surface info index values */
#define NV0041_CTRL_SURFACE_INFO_INDEX_ATTRS                           (0x00000001)
#define NV0041_CTRL_SURFACE_INFO_INDEX_COMPR_COVERAGE                  (0x00000005)
#define NV0041_CTRL_SURFACE_INFO_INDEX_PHYS_SIZE                       (0x00000007)
#define NV0041_CTRL_SURFACE_INFO_INDEX_PHYS_ATTR                       (0x00000008)
#define NV0041_CTRL_SURFACE_INFO_INDEX_ADDR_SPACE_TYPE                 (0x00000009)

/*
 * This define indicates the scale factor of the reported physical size to the
 * actual size in bytes. We use the scale factor to save space from the
 * interface and account for large surfaces. To get the actual size,
 * use `(NvU64)reported_size * NV0041_CTRL_SURFACE_INFO_PHYS_SIZE_SCALE_FACTOR`.
 */
#define NV0041_CTRL_SURFACE_INFO_PHYS_SIZE_SCALE_FACTOR                (0x1000)

/* valid surface info attr flags */
#define NV0041_CTRL_SURFACE_INFO_ATTRS_COMPR                           (0x00000002)
#define NV0041_CTRL_SURFACE_INFO_ATTRS_ZCULL                           (0x00000004)

/* Valid surface info page size */
#define NV0041_CTRL_SURFACE_INFO_PHYS_ATTR_PAGE_SIZE                   NVOS32_ATTR_PAGE_SIZE
#define NV0041_CTRL_SURFACE_INFO_PHYS_ATTR_PAGE_SIZE_DEFAULT           NVOS32_ATTR_PAGE_SIZE_DEFAULT
#define NV0041_CTRL_SURFACE_INFO_PHYS_ATTR_PAGE_SIZE_4KB               NVOS32_ATTR_PAGE_SIZE_4KB
#define NV0041_CTRL_SURFACE_INFO_PHYS_ATTR_PAGE_SIZE_BIG               NVOS32_ATTR_PAGE_SIZE_BIG
#define NV0041_CTRL_SURFACE_INFO_PHYS_ATTR_PAGE_SIZE_HUGE              NVOS32_ATTR_PAGE_SIZE_HUGE

/* Valid surface info CPU coherency */
#define NV0041_CTRL_SURFACE_INFO_PHYS_ATTR_CPU_COHERENCY               NVOS32_ATTR_COHERENCY
#define NV0041_CTRL_SURFACE_INFO_PHYS_ATTR_CPU_COHERENCY_UNCACHED      NVOS32_ATTR_COHERENCY_UNCACHED
#define NV0041_CTRL_SURFACE_INFO_PHYS_ATTR_CPU_COHERENCY_CACHED        NVOS32_ATTR_COHERENCY_CACHED
#define NV0041_CTRL_SURFACE_INFO_PHYS_ATTR_CPU_COHERENCY_WRITE_COMBINE NVOS32_ATTR_COHERENCY_WRITE_COMBINE
#define NV0041_CTRL_SURFACE_INFO_PHYS_ATTR_CPU_COHERENCY_WRITE_THROUGH NVOS32_ATTR_COHERENCY_WRITE_THROUGH
#define NV0041_CTRL_SURFACE_INFO_PHYS_ATTR_CPU_COHERENCY_WRITE_PROTECT NVOS32_ATTR_COHERENCY_WRITE_PROTECT
#define NV0041_CTRL_SURFACE_INFO_PHYS_ATTR_CPU_COHERENCY_WRITE_BACK    NVOS32_ATTR_COHERENCY_WRITE_BACK

/* Valid surface info format */
#define NV0041_CTRL_SURFACE_INFO_PHYS_ATTR_FORMAT                      NVOS32_ATTR_FORMAT
#define NV0041_CTRL_SURFACE_INFO_PHYS_ATTR_FORMAT_PITCH                NVOS32_ATTR_FORMAT_PITCH
#define NV0041_CTRL_SURFACE_INFO_PHYS_ATTR_FORMAT_SWIZZLED             NVOS32_ATTR_FORMAT_SWIZZLED
#define NV0041_CTRL_SURFACE_INFO_PHYS_ATTR_FORMAT_BLOCK_LINEAR         NVOS32_ATTR_FORMAT_BLOCK_LINEAR

/*
 * NV0041_CTRL_CMD_GET_SURFACE_INFO
 *
 * This command returns surface information for the associated memory object.
 * Requests to retrieve surface information use a list of one or more
 * NV0041_CTRL_SURFACE_INFO structures.
 *
 *   surfaceInfoListSize
 *     This field specifies the number of entries on the caller's
 *     surfaceInfoList.
 *   surfaceInfoList
 *     This field specifies a pointer in the caller's address space
 *     to the buffer into which the surface information is to be returned.
 *     This buffer must be at least as big as surfaceInfoListSize multiplied
 *     by the size of the NV0041_CTRL_SURFACE_INFO structure.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_OPERATING_SYSTEM
 */
#define NV0041_CTRL_CMD_GET_SURFACE_INFO                               (0x410110) /* finn: Evaluated from "(FINN_NV01_ROOT_USER_MEMORY_INTERFACE_ID << 8) | NV0041_CTRL_GET_SURFACE_INFO_PARAMS_MESSAGE_ID" */

#define NV0041_CTRL_GET_SURFACE_INFO_PARAMS_MESSAGE_ID (0x10U)

typedef struct NV0041_CTRL_GET_SURFACE_INFO_PARAMS {
    NvU32 surfaceInfoListSize;
    NV_DECLARE_ALIGNED(NvP64 surfaceInfoList, 8);
} NV0041_CTRL_GET_SURFACE_INFO_PARAMS;

/*
 * NV0041_CTRL_CMD_GET_SURFACE_COMPRESSION_COVERAGE
 *
 * This command returns the percentage of surface compression tag coverage.
 * The value of 0 is returned if there are no tags associated.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NVOS_STATUS_BAD_OBJECT_HANDLE
 *   NVOS_STATUS_BAD_OBJECT_PARENT
 *   NVOS_STATUS_NOT_SUPPORTED
 *
 */
#define NV0041_CTRL_CMD_GET_SURFACE_COMPRESSION_COVERAGE (0x410112) /* finn: Evaluated from "(FINN_NV01_ROOT_USER_MEMORY_INTERFACE_ID << 8) | NV0041_CTRL_GET_SURFACE_COMPRESSION_COVERAGE_PARAMS_MESSAGE_ID" */

#define NV0041_CTRL_GET_SURFACE_COMPRESSION_COVERAGE_PARAMS_MESSAGE_ID (0x12U)

typedef struct NV0041_CTRL_GET_SURFACE_COMPRESSION_COVERAGE_PARAMS {
    NvHandle hSubDevice; /* if non zero subDevice handle of local GPU */
    NvU32    lineMin;
    NvU32    lineMax;
    NvU32    format;
} NV0041_CTRL_GET_SURFACE_COMPRESSION_COVERAGE_PARAMS;

/*
 * NV0041_CTRL_CMD_GET_FBMEM_BUS_ADDR
 *
 * This command returns the BAR1 physical address of a
 * Memory mapping made using NvRmMapMemory()
 *
 * Possible status values returned are:
 *   NV_OK
 *   NVOS_STATUS_INVALID_DATA
 *   NV_ERR_INVALID_CLIENT
 *   NV_ERR_INVALID_OBJECT_HANDLE
 *
 */
#define NV0041_CTRL_CMD_GET_FBMEM_BUS_ADDR (0x410114) /* finn: Evaluated from "(FINN_NV01_ROOT_USER_MEMORY_INTERFACE_ID << 8) | NV0041_CTRL_GET_FBMEM_BUS_ADDR_PARAMS_MESSAGE_ID" */

#define NV0041_CTRL_GET_FBMEM_BUS_ADDR_PARAMS_MESSAGE_ID (0x14U)

typedef struct NV0041_CTRL_GET_FBMEM_BUS_ADDR_PARAMS {
    NV_DECLARE_ALIGNED(NvP64 pLinearAddress, 8);  /* [in] Linear address of CPU mapping */
    NV_DECLARE_ALIGNED(NvU64 busAddress, 8);  /* [out] BAR1 address */
} NV0041_CTRL_GET_FBMEM_BUS_ADDR_PARAMS;

/*
 * NV0041_CTRL_CMD_SURFACE_FLUSH_GPU_CACHE
 * 
 * This command flushes a cache on the GPU which all memory accesses go
 * through.  The types of flushes supported by this API may not be supported by
 * all hardware.  Attempting an unsupported flush type will result in an error.
 *
 *   flags
 *     Contains flags to control various aspects of the flush.  Valid values
 *     are defined in NV0041_CTRL_SURFACE_FLUSH_GPU_CACHE_FLAGS*.  Not all
 *     flags are valid for all GPUs.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 *   NVOS_STATUS_INVALID_ARGUMENT
 *   NVOS_STATUS_INVALID_STATE
 *
 * See Also:
 *   NV0080_CTRL_CMD_DMA_FLUSH 
 *     Performs flush operations in broadcast for the GPU cache and other hardware
 *     engines.  Use this call if you want to flush all GPU caches in a
 *     broadcast device.
 *   NV2080_CTRL_CMD_FB_FLUSH_GPU_CACHE
 *     Flushes the entire GPU cache or a set of physical addresses (if the
 *     hardware supports it).  Use this call if you want to flush a set of
 *     addresses or the entire GPU cache in unicast mode.
 *
 */
#define NV0041_CTRL_CMD_SURFACE_FLUSH_GPU_CACHE (0x410116) /* finn: Evaluated from "(FINN_NV01_ROOT_USER_MEMORY_INTERFACE_ID << 8) | NV0041_CTRL_SURFACE_FLUSH_GPU_CACHE_PARAMS_MESSAGE_ID" */

#define NV0041_CTRL_SURFACE_FLUSH_GPU_CACHE_PARAMS_MESSAGE_ID (0x16U)

typedef struct NV0041_CTRL_SURFACE_FLUSH_GPU_CACHE_PARAMS {
    NvU32 flags;
} NV0041_CTRL_SURFACE_FLUSH_GPU_CACHE_PARAMS;

#define NV0041_CTRL_SURFACE_FLUSH_GPU_CACHE_FLAGS_WRITE_BACK                0:0
#define NV0041_CTRL_SURFACE_FLUSH_GPU_CACHE_FLAGS_WRITE_BACK_NO  (0x00000000)
#define NV0041_CTRL_SURFACE_FLUSH_GPU_CACHE_FLAGS_WRITE_BACK_YES (0x00000001)
#define NV0041_CTRL_SURFACE_FLUSH_GPU_CACHE_FLAGS_INVALIDATE                1:1
#define NV0041_CTRL_SURFACE_FLUSH_GPU_CACHE_FLAGS_INVALIDATE_NO  (0x00000000)
#define NV0041_CTRL_SURFACE_FLUSH_GPU_CACHE_FLAGS_INVALIDATE_YES (0x00000001)

/*
 * NV0041_CTRL_CMD_GET_EME_PAGE_SIZE
 *
 * This command may be used to get the memory page size 
 *
 * Parameters:
 *   pageSize [OUT]
 *     pageSize with associated memory descriptor
 *
 * Possible status values are:
 *   NV_OK
 *   NV_ERR_INVALID_OBJECT_HANDLE
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV0041_CTRL_CMD_GET_MEM_PAGE_SIZE                        (0x410118) /* finn: Evaluated from "(FINN_NV01_ROOT_USER_MEMORY_INTERFACE_ID << 8) | NV0041_CTRL_GET_MEM_PAGE_SIZE_PARAMS_MESSAGE_ID" */

#define NV0041_CTRL_GET_MEM_PAGE_SIZE_PARAMS_MESSAGE_ID (0x18U)

typedef struct NV0041_CTRL_GET_MEM_PAGE_SIZE_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 pageSize, 8);             /* [out] - page size */
} NV0041_CTRL_GET_MEM_PAGE_SIZE_PARAMS;

/*
 * NV0041_CTRL_CMD_UPDATE_SURFACE_COMPRESSION
 *
 * Acquire/release compression for surface
 *
 * Parameters:
 *   bRelease [IN]
 *     true = release compression; false = acquire compression
 */
#define NV0041_CTRL_CMD_UPDATE_SURFACE_COMPRESSION (0x410119) /* finn: Evaluated from "(FINN_NV01_ROOT_USER_MEMORY_INTERFACE_ID << 8) | NV0041_CTRL_UPDATE_SURFACE_COMPRESSION_PARAMS_MESSAGE_ID" */

#define NV0041_CTRL_UPDATE_SURFACE_COMPRESSION_PARAMS_MESSAGE_ID (0x19U)

typedef struct NV0041_CTRL_UPDATE_SURFACE_COMPRESSION_PARAMS {
    NvBool bRelease;             /* [in] - acquire/release setting */
} NV0041_CTRL_UPDATE_SURFACE_COMPRESSION_PARAMS;

#define NV0041_CTRL_CMD_PRINT_LABELS_PARAMS_MESSAGE_ID (0x50U)

typedef struct NV0041_CTRL_CMD_PRINT_LABELS_PARAMS {
    NvU32 tag; /* [in] */
} NV0041_CTRL_CMD_PRINT_LABELS_PARAMS;
#define NV0041_CTRL_CMD_SET_LABEL_PARAMS_MESSAGE_ID (0x51U)

typedef struct NV0041_CTRL_CMD_SET_LABEL_PARAMS {
    NvU32 tag; /* [in] */
} NV0041_CTRL_CMD_SET_LABEL_PARAMS;
#define NV0041_CTRL_CMD_SET_LABEL (0x410151) /* finn: Evaluated from "(FINN_NV01_ROOT_USER_MEMORY_INTERFACE_ID << 8) | NV0041_CTRL_CMD_SET_LABEL_PARAMS_MESSAGE_ID" */
#define NV0041_CTRL_CMD_GET_LABEL (0x410152) /* finn: Evaluated from "(FINN_NV01_ROOT_USER_MEMORY_INTERFACE_ID << 8) | NV0041_CTRL_CMD_GET_LABEL_PARAMS_MESSAGE_ID" */
#define NV0041_CTRL_CMD_GET_LABEL_PARAMS_MESSAGE_ID (0x52U)

typedef struct NV0041_CTRL_CMD_GET_LABEL_PARAMS {
    NvU32 tag; /* [in] */
} NV0041_CTRL_CMD_GET_LABEL_PARAMS;

/*
 * NV0041_CTRL_CMD_SET_TAG
 *
 * This command sets memory allocation tag used for debugging.
* Every client has it's own memory allocation tag and tag is copying when object is duping.
 * This control can be used for shared allocations to change it's tag.
 */
#define NV0041_CTRL_CMD_SET_TAG (0x410120) /* finn: Evaluated from "(FINN_NV01_ROOT_USER_MEMORY_INTERFACE_ID << 8) | NV0041_CTRL_CMD_SET_TAG_PARAMS_MESSAGE_ID" */

#define NV0041_CTRL_CMD_SET_TAG_PARAMS_MESSAGE_ID (0x20U)

typedef struct NV0041_CTRL_CMD_SET_TAG_PARAMS {
    NvU32 tag; /* [in] */
} NV0041_CTRL_CMD_SET_TAG_PARAMS;

/*
 * NV0041_CTRL_CMD_GET_TAG
 *
 * This command returns memory allocation tag used for debugging.
 */
#define NV0041_CTRL_CMD_GET_TAG (0x410121) /* finn: Evaluated from "(FINN_NV01_ROOT_USER_MEMORY_INTERFACE_ID << 8) | NV0041_CTRL_CMD_GET_TAG_PARAMS_MESSAGE_ID" */

#define NV0041_CTRL_CMD_GET_TAG_PARAMS_MESSAGE_ID (0x21U)

typedef struct NV0041_CTRL_CMD_GET_TAG_PARAMS {
    NvU32 tag; /* [out] */
} NV0041_CTRL_CMD_GET_TAG_PARAMS;

/* _ctrl0041_h_ */
