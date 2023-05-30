/*
 * SPDX-FileCopyrightText: Copyright (c) 2012-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrla080.finn
//





/*
 * NVA080_CTRL_CMD_VGPU_DISPLAY_SET_SURFACE_PROPERTIES
 * 
 * This command sets primary surface properties on a virtual GPU in displayless mode
 *
 * Parameters:
 *   headIndex
 *      This parameter specifies the head for which surface properties are 
 *
 *   isPrimary
 *      This parameter indicates whether surface information is for primary surface. set to 1 if its a primary surface.
 *
 *   hMemory
 *      Memory handle containing the surface (only for RM-managed heaps)
 *
 *   offset
 *      Offset from base of allocation (hMemory for RM-managed heaps; physical
 *      memory otherwise)
 *
 *   surfaceType
 *      This parameter indicates whether surface type is block linear or pitch
 *
 *   surfaceBlockHeight
 *      This parameter indicates block height for the surface
 *
 *   surfacePitch
 *      This parameter indicates pitch value for the surface
 *
 *   surfaceFormat
 *      This parameter indicates surface format (A8R8G8B8/A1R5G5B5)
 *
 *   surfaceWidth
 *      This parameter indicates width value for the surface 
 *
 *   surfaceHeight
 *      This parameter indicates height value for the surface
 *
 *   surfaceSize
 *      This parameter indicates size of the surface
 *
 *   surfaceKind
 *      This parameter indicates surface kind (only for externally-managed
 *      heaps)
 *
 *    rectX [unused]
 *      This parameter indicates X coordinate of the region to be displayed
 *
 *    rectY [unused]
 *      This parameter indicates Y coordinate of the region to be displayed
 *
 *    rectWidth
 *      This parameter indicates width of the region to be displayed
 *
 *    rectHeight
 *      This parameter indicates height of the region to be displayed
 *
 *    hHwResDevice
 *      This parameter indicates the device associated with surface
 *
 *    hHwResHandle
 *      This parameter indicates the handle to hardware resources allocated to surface
 *
 *    effectiveFbPageSize 
 *      This parameter indicates the actual page size used by KMD for the surface
 *
 *   Possible status values returned are:
 *      NV_OK
 *      NV_ERR_INVALID_ARGUMENT
 *      NVOS_STATUS_NOT_SUPPORTED
 */

#define NVA080_CTRL_CMD_VGPU_DISPLAY_SET_SURFACE_PROPERTIES (0xa0800103) /* finn: Evaluated from "(FINN_KEPLER_DEVICE_VGPU_VGPU_DISPLAY_INTERFACE_ID << 8) | NVA080_CTRL_VGPU_DISPLAY_SET_SURFACE_PROPERTIES_MESSAGE_ID" */

#define NVA080_CTRL_VGPU_DISPLAY_SET_SURFACE_PROPERTIES_MESSAGE_ID (0x3U)

typedef struct NVA080_CTRL_VGPU_DISPLAY_SET_SURFACE_PROPERTIES {
    NvU32 headIndex;
    NvU32 isPrimary;
    NvU32 hMemory;
    NvU32 offset;
    NvU32 surfaceType;
    NvU32 surfaceBlockHeight;
    NvU32 surfacePitch;
    NvU32 surfaceFormat;
    NvU32 surfaceWidth;
    NvU32 surfaceHeight;
    NvU32 rectX;
    NvU32 rectY;
    NvU32 rectWidth;
    NvU32 rectHeight;
    NvU32 surfaceSize;
    NvU32 surfaceKind;
    NvU32 hHwResDevice;
    NvU32 hHwResHandle;
    NvU32 effectiveFbPageSize;
} NVA080_CTRL_VGPU_DISPLAY_SET_SURFACE_PROPERTIES;

/* valid surfaceType values */
#define NVA080_CTRL_VGPU_DISPLAY_SET_SURFACE_PROPERTIES_MEMORY_LAYOUT                      0:0
#define NVA080_CTRL_VGPU_DISPLAY_SET_SURFACE_PROPERTIES_MEMORY_LAYOUT_BLOCKLINEAR    0x00000000
#define NVA080_CTRL_VGPU_DISPLAY_SET_SURFACE_PROPERTIES_MEMORY_LAYOUT_PITCH          0x00000001
/* valid surfaceBlockHeight values */
#define NVA080_CTRL_VGPU_DISPLAY_SET_SURFACE_PROPERTIES_BLOCK_HEIGHT_ONE_GOB         0x00000000
#define NVA080_CTRL_VGPU_DISPLAY_SET_SURFACE_PROPERTIES_BLOCK_HEIGHT_TWO_GOBS        0x00000001
#define NVA080_CTRL_VGPU_DISPLAY_SET_SURFACE_PROPERTIES_BLOCK_HEIGHT_FOUR_GOBS       0x00000002
#define NVA080_CTRL_VGPU_DISPLAY_SET_SURFACE_PROPERTIES_BLOCK_HEIGHT_EIGHT_GOBS      0x00000003
#define NVA080_CTRL_VGPU_DISPLAY_SET_SURFACE_PROPERTIES_BLOCK_HEIGHT_SIXTEEN_GOBS    0x00000004
#define NVA080_CTRL_VGPU_DISPLAY_SET_SURFACE_PROPERTIES_BLOCK_HEIGHT_THIRTYTWO_GOBS  0x00000005
/* valid surfaceFormat values */
#define NVA080_CTRL_VGPU_DISPLAY_SET_SURFACE_PROPERTIES_FORMAT_I8                    0x0000001E
#define NVA080_CTRL_VGPU_DISPLAY_SET_SURFACE_PROPERTIES_FORMAT_RF16_GF16_BF16_AF16   0x000000CA
#define NVA080_CTRL_VGPU_DISPLAY_SET_SURFACE_PROPERTIES_FORMAT_A8R8G8B8              0x000000CF
#define NVA080_CTRL_VGPU_DISPLAY_SET_SURFACE_PROPERTIES_FORMAT_A2B10G10R10           0x000000D1
#define NVA080_CTRL_VGPU_DISPLAY_SET_SURFACE_PROPERTIES_FORMAT_X2BL10GL10RL10_XRBIAS 0x00000022
#define NVA080_CTRL_VGPU_DISPLAY_SET_SURFACE_PROPERTIES_FORMAT_A8B8G8R8              0x000000D5
#define NVA080_CTRL_VGPU_DISPLAY_SET_SURFACE_PROPERTIES_FORMAT_R5G6B5                0x000000E8
#define NVA080_CTRL_VGPU_DISPLAY_SET_SURFACE_PROPERTIES_FORMAT_A1R5G5B5              0x000000E9
#define NVA080_CTRL_VGPU_DISPLAY_SET_SURFACE_PROPERTIES_FORMAT_R16_G16_B16_A16       0x000000C6

/*
 * NVA080_CTRL_CMD_VGPU_DISPLAY_CLEANUP_SURFACE_PARAMS
 * 
 * This command clears surface information related to head. 
 * It should be called while shutting down head in displayless mode on virtual GPU
 *
 * Parameters:
 *   headIndex
 *     This parameter specifies the head for which cleanup is requested.
 *
 *   blankingEnabled
 *     This parameter must be set to 1 to enable blanking.
 *
 *   Possible status values returned are:
 *     NV_OK
 *     NV_ERR_INVALID_ARGUMENT
 *     NVOS_STATUS_NOT_SUPPORTED
 */
#define NVA080_CTRL_CMD_VGPU_DISPLAY_CLEANUP_SURFACE                                 (0xa0800104) /* finn: Evaluated from "(FINN_KEPLER_DEVICE_VGPU_VGPU_DISPLAY_INTERFACE_ID << 8) | NVA080_CTRL_VGPU_DISPLAY_CLEANUP_SURFACE_PARAMS_MESSAGE_ID" */

#define NVA080_CTRL_VGPU_DISPLAY_CLEANUP_SURFACE_PARAMS_MESSAGE_ID (0x4U)

typedef struct NVA080_CTRL_VGPU_DISPLAY_CLEANUP_SURFACE_PARAMS {
    NvU32 headIndex;
    NvU32 blankingEnabled;
} NVA080_CTRL_VGPU_DISPLAY_CLEANUP_SURFACE_PARAMS;

/*
 * NVA080_CTRL_CMD_VGPU_DISPLAY_GET_POINTER_ADDRESS
 *
 * This command returns CPU virtual address of the mouse pointer mapping for VGPU.
 * The address returned by this command is the pointer address for the head 0.
 * VGPU_POINTER_OFFSET_HEAD(i) should be added to this address to get the address of head i.
 * VGPU mouse pointer is a 32 bit value, X location of the mouse pointer is stored in
 * 15:0 and Y location is stored in 31:16 bits. X location value of the mouse pointer is
 * negative if bit 15 is set. Similarly, Y location value is negative if bit 31 is set.
 *
 * Parameters:
 *   pPointerAddress
 *     CPU virtual address of the mouse pointer mapping for VGPU
 * 
 * Possible status values returned are:
 *      NV_OK
 *      NV_ERR_INVALID_ARGUMENT
 *      NVOS_STATUS_NOT_SUPPORTED
 */

#define NVA080_CTRL_CMD_VGPU_DISPLAY_GET_POINTER_ADDRESS (0xa0800105) /* finn: Evaluated from "(FINN_KEPLER_DEVICE_VGPU_VGPU_DISPLAY_INTERFACE_ID << 8) | NVA080_CTRL_VGPU_DISPLAY_GET_POINTER_ADDRESS_PARAMS_MESSAGE_ID" */

#define NVA080_CTRL_VGPU_DISPLAY_GET_POINTER_ADDRESS_PARAMS_MESSAGE_ID (0x5U)

typedef struct NVA080_CTRL_VGPU_DISPLAY_GET_POINTER_ADDRESS_PARAMS {
    NV_DECLARE_ALIGNED(NvP64 pPointerAddress, 8);
} NVA080_CTRL_VGPU_DISPLAY_GET_POINTER_ADDRESS_PARAMS;

#define VGPU_POINTER_OFFSET_HEAD0_VALUE 0x00000000
#define VGPU_POINTER_OFFSET_HEAD0_FLAG  0x00000004
#define VGPU_POINTER_OFFSET_HEAD1_VALUE 0x00000008
#define VGPU_POINTER_OFFSET_HEAD1_FLAG  0x0000000c
#define VGPU_POINTER_OFFSET_HEAD2_VALUE 0x00000010
#define VGPU_POINTER_OFFSET_HEAD2_FLAG  0x00000014
#define VGPU_POINTER_OFFSET_HEAD3_VALUE 0x00000018
#define VGPU_POINTER_OFFSET_HEAD3_FLAG  0x0000001c
#define VGPU_POINTER_OFFSET_HEAD_VALUE(i)   (i * 8)
#define VGPU_POINTER_OFFSET_HEAD_FLAG(i)    (4 + i * 8)
#define VGPU_POINTER_OFFSET_HEAD_SIZE   4

/*
 * NVA080_CTRL_CMD_GET_MAPPABLE_VIDEO_SIZE
 *
 * This command returns mappable video size to be used by each VM.
 *
 * Parameters:
 *   mappableVideoSize
 *     This parameter returns mappable video size in bytes.
 *
 *  Possible status values returned are:
 *    NV_OK
 *    NV_ERR_INVALID_ARGUMENT
 *    NVOS_STATUS_NOT_SUPPORTED
 */


#define NV_VGPU_POINTER_X_LOCATION    15:0
#define NV_VGPU_POINTER_Y_LOCATION    31:16
#define NVA080_CTRL_CMD_GET_MAPPABLE_VIDEO_SIZE (0xa0800201) /* finn: Evaluated from "(FINN_KEPLER_DEVICE_VGPU_VGPU_MEMORY_INTERFACE_ID << 8) | NVA080_CTRL_GET_MAPPABLE_VIDEO_SIZE_PARAMS_MESSAGE_ID" */

#define NVA080_CTRL_GET_MAPPABLE_VIDEO_SIZE_PARAMS_MESSAGE_ID (0x1U)

typedef struct NVA080_CTRL_GET_MAPPABLE_VIDEO_SIZE_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 mappableVideoSize, 8);
} NVA080_CTRL_GET_MAPPABLE_VIDEO_SIZE_PARAMS;

/*
 *  NVA080_CTRL_CMD_MAP_SEMA_MEM
 *
 *  This command returns GPU VA for the channel with 'hCtxDma' handle
 *  where per VM semaphore memory is mapped which is used for tracking
 *  non-stall interrupt of each VM.
 *
 * Parameters:
 *   hClient [in]
 *     This parameter specifies the handle to the NV01_ROOT object of
 *     the client.  This object should be the parent of the object
 *     specified by hDevice.
 *   hDevice [in]
 *     This parameter specifies the handle of the NV01_DEVICE object
 *     representing the desired GPU.
 *   hMemory [in]
 *     This parameter specifies the handle for semaphore memory
 *   hCtxDma [in]
 *     This parameter specifies the handle of the NV01_CONTEXT_DMA
 *     object through which bufferId is written in semaphore memory for
 *     non-stall interrupt tracking.
 *   semaAddress [out]
 *     This parameter returns the GPU virtual address of the semaphore
 *     memory.
 *
 *  Possible status values returned are:
 *    NV_OK
 *    NVOS_STATUS_INVALID_DATA
 *    NV_ERR_INVALID_CLIENT
 *    NV_ERR_INVALID_OBJECT_HANDLE
 *
 */

#define NVA080_CTRL_CMD_MAP_SEMA_MEM (0xa0800202) /* finn: Evaluated from "(FINN_KEPLER_DEVICE_VGPU_VGPU_MEMORY_INTERFACE_ID << 8) | NVA080_CTRL_MAP_SEMA_MEM_PARAMS_MESSAGE_ID" */

typedef struct NVA080_CTRL_SEMA_MEM_PARAMS {
    NvHandle hClient;
    NvHandle hDevice;
    NvHandle hMemory;
    NvHandle hCtxDma;
    NV_DECLARE_ALIGNED(NvU64 semaAddress, 8);
} NVA080_CTRL_SEMA_MEM_PARAMS;

#define NVA080_CTRL_MAP_SEMA_MEM_PARAMS_MESSAGE_ID (0x2U)

typedef NVA080_CTRL_SEMA_MEM_PARAMS NVA080_CTRL_MAP_SEMA_MEM_PARAMS;

/*
 *  NVA080_CTRL_CMD_UNMAP_SEMA_MEM
 *
 *  This command unmaps per VM semaphore memory from GPU VA space, mapped by
 *  NVA080_CTRL_CMD_MAP_SEMA_MEM command.
 *
 * Parameters:
 *  Same as NVA080_CTRL_MAP_SEMA_MEM_PARAMS, except semaAddress is input
 *  parameter here.
 *
 *  Possible status values returned are:
 *    NV_OK
 *    NVOS_STATUS_INVALID_DATA
 *    NV_ERR_INVALID_CLIENT
 *    NV_ERR_INVALID_OBJECT_HANDLE
 *
 */

#define NVA080_CTRL_CMD_UNMAP_SEMA_MEM (0xa0800203) /* finn: Evaluated from "(FINN_KEPLER_DEVICE_VGPU_VGPU_MEMORY_INTERFACE_ID << 8) | NVA080_CTRL_UNMAP_SEMA_MEM_PARAMS_MESSAGE_ID" */

#define NVA080_CTRL_UNMAP_SEMA_MEM_PARAMS_MESSAGE_ID (0x3U)

typedef NVA080_CTRL_SEMA_MEM_PARAMS NVA080_CTRL_UNMAP_SEMA_MEM_PARAMS;

/*!
 * NVA080_CTRL_CMD_SET_FB_USAGE
 *
 *  This command sets the current framebuffer usage value in the plugin.
 *
 *  Parameters:
 *   fbUsed [in]
 *     This parameter holds the current FB usage value in bytes.
 *
 *   Possible status values returned are:
 *      NV_OK
 */
#define NVA080_CTRL_CMD_SET_FB_USAGE (0xa0800204) /* finn: Evaluated from "(FINN_KEPLER_DEVICE_VGPU_VGPU_MEMORY_INTERFACE_ID << 8) | NVA080_CTRL_SET_FB_USAGE_PARAMS_MESSAGE_ID" */

#define NVA080_CTRL_SET_FB_USAGE_PARAMS_MESSAGE_ID (0x4U)

typedef struct NVA080_CTRL_SET_FB_USAGE_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 fbUsed, 8);
} NVA080_CTRL_SET_FB_USAGE_PARAMS;

/*!
* NVA080_CTRL_CMD_MAP_PER_ENGINE_SEMA_MEM
*
* This command allocates the per engine vGPU semaphore memory and map it to
* GPU/CPU VA.
*
* Calculate engine's semaphore GPU VA =
*               semaAddress + semaStride * NV2080_ENGINE_TYPE_ of that engine
*
* Parameters:
*    hClient [in]
*       This parameter specifies the handle to the NV01_ROOT object of
*       the client.  This object should be the parent of the object
*       specified by hDevice.
*   hDevice [in]
*       This parameter specifies the handle of the NV01_DEVICE object
*       representing the desired GPU.
*   hMemory [in]
*       This parameter specifies the handle for semaphore memory
*   hCtxDma [in]
*       This parameter specifies the handle of the NV01_CONTEXT_DMA
*       object through which bufferId is written in semaphore memory for
*       non-stall interrupt tracking.
*   semaAddress [out]
*       This parameter returns the GPU VA of the per engine semaphore memory.
*   semaStride [out]
*       This parameter specifies the stride of each engine's semaphore offset within this memory.
*
* Possible status values returned are:
*   NV_OK
*/

#define NVA080_CTRL_CMD_MAP_PER_ENGINE_SEMA_MEM (0xa0800205) /* finn: Evaluated from "(FINN_KEPLER_DEVICE_VGPU_VGPU_MEMORY_INTERFACE_ID << 8) | NVA080_CTRL_MAP_PER_ENGINE_SEMA_MEM_PARAMS_MESSAGE_ID" */

typedef struct NVA080_CTRL_PER_ENGINE_SEMA_MEM_PARAMS {
    NvU32    hClient;
    NvU32    hDevice;
    NvHandle hMemory;
    NvU32    hCtxDma;
    NV_DECLARE_ALIGNED(NvU64 semaAddress, 8);
    NvU32    semaStride;
} NVA080_CTRL_PER_ENGINE_SEMA_MEM_PARAMS;

#define NVA080_CTRL_MAP_PER_ENGINE_SEMA_MEM_PARAMS_MESSAGE_ID (0x5U)

typedef NVA080_CTRL_PER_ENGINE_SEMA_MEM_PARAMS NVA080_CTRL_MAP_PER_ENGINE_SEMA_MEM_PARAMS;

/*!
* NVA080_CTRL_CMD_UNMAP_PER_ENGINE_SEMA_MEM
*
* This command unmaps and frees the per engine vGPU semaphore memory.
*
* Parameters:
*   Same as NVA080_CTRL_MAP_PER_ENGINE_SEMA_MEM_PARAMS, except semaAddress is input
*   parameter here.
*
*  Possible status values returned are:
*    NV_OK
*/

#define NVA080_CTRL_CMD_UNMAP_PER_ENGINE_SEMA_MEM (0xa0800206) /* finn: Evaluated from "(FINN_KEPLER_DEVICE_VGPU_VGPU_MEMORY_INTERFACE_ID << 8) | NVA080_CTRL_UNMAP_PER_ENGINE_SEMA_MEM_PARAMS_MESSAGE_ID" */

#define NVA080_CTRL_UNMAP_PER_ENGINE_SEMA_MEM_PARAMS_MESSAGE_ID (0x6U)

typedef NVA080_CTRL_PER_ENGINE_SEMA_MEM_PARAMS NVA080_CTRL_UNMAP_PER_ENGINE_SEMA_MEM_PARAMS;

/*
 *  NVA080_CTRL_CMD_UPDATE_SYSMEM_BITMAP
 *
 *  This command provides the guest RM with PFN information, so that it can
 *  update the shared memory with the plugin, which keeps track of guest sysmem.
 *
 *  Parameters:
 *   destPhysAddr
 *     Start address of the segment to be tracked
 *
 *   pageCount
 *     Number of pages in the segment
 *
 *   pageSize
 *     Size of pages in the segment
 *
 *   isValid:
 *     TRUE : Set bits corresponding to PFNs in bitmap and increase segment refcount
 *     FALSE: Decrease segment refcount and then unset bits if refcount is 0
 *
 *   pfnList
 *     List of PFNs in the segment
 *
 *   flags
 *     FLAGS_DST_PHYS_ADDR_BAR1_OFFSET
 *       Flag set to TRUE if pteMem is CPU VA pointing to BAR1 and
 *       dstPhysAddr contains BAR1 offset.
 *
 * Possible status values returned are:
 *   NVOS_STATUS_SUCCESS
 */

#define NVA080_CTRL_CMD_UPDATE_SYSMEM_BITMAP                                          (0xa0800207) /* finn: Evaluated from "(FINN_KEPLER_DEVICE_VGPU_VGPU_MEMORY_INTERFACE_ID << 8) | NVA080_CTRL_UPDATE_SYSMEM_BITMAP_PARAMS_MESSAGE_ID" */

#define NVA080_CTRL_UPDATE_SYSMEM_BITMAP_PARAMS_FLAGS_DST_PHYS_ADDR_BAR1_OFFSET       0:0
#define NVA080_CTRL_UPDATE_SYSMEM_BITMAP_PARAMS_FLAGS_DST_PHYS_ADDR_BAR1_OFFSET_FALSE (0x00000000)
#define NVA080_CTRL_UPDATE_SYSMEM_BITMAP_PARAMS_FLAGS_DST_PHYS_ADDR_BAR1_OFFSET_TRUE  (0x00000001)

#define NVA080_CTRL_UPDATE_SYSMEM_BITMAP_PARAMS_MESSAGE_ID (0x7U)

typedef struct NVA080_CTRL_UPDATE_SYSMEM_BITMAP_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 destPhysAddr, 8);
    NvU32  pageCount;
    NvU32  pageSize;
    NvBool isValid;
    NV_DECLARE_ALIGNED(NvP64 pfnList, 8);
    NvU32  flags;
} NVA080_CTRL_UPDATE_SYSMEM_BITMAP_PARAMS;

/*
 * Blit semaphore offset location
 */
#define VGPU_BLIT_RESTORE_SEMA_MEM_OFFSET            0x200
#define VGPU_BLIT_RESTORE_SEMA_MEM_ADDR(addr)               (((NvU64)addr) + VGPU_BLIT_RESTORE_SEMA_MEM_OFFSET)

#define VGPU_BLIT_SEMA_MEM_OFFSET                    0x400
#define VGPU_BLIT_SEMA_MEM_ADDR(addr)                       (((NvU64)addr) + VGPU_BLIT_SEMA_MEM_OFFSET)

#define VGPU_FBMEMCE_PUSH_SEMA_MEM_OFFSET            0x800
#define VGPU_FBMEMCE_PUSH_SEMA_MEM_ADDR(addr)               (((NvU64)addr) + VGPU_FBMEMCE_PUSH_SEMA_MEM_OFFSET)

#define VGPU_FBMEMCE_SEMA_MEM_OFFSET                 0x810
#define VGPU_FBMEMCE_SEMA_MEM_ADDR(addr)                    (((NvU64)addr) + VGPU_FBMEMCE_SEMA_MEM_OFFSET)

#define VGPU_FBMEMCE_PIPELINED_SEMA_MEM_LOWER_OFFSET 0x820
#define VGPU_FBMEMCE_PIPELINED_SEMA_MEM_LOWER_ADDR(addr)    (((NvU64)addr) + VGPU_FBMEMCE_PIPELINED_SEMA_MEM_LOWER_OFFSET)

#define VGPU_FBMEMCE_PIPELINED_SEMA_MEM_UPPER_OFFSET 0x824

/*
 *  NVA080_CTRL_CMD_VGPU_GET_CONFIG
 *
 *  This command returns VGPU configuration information for the associated GPU.
 *
 *  Parameters:
 *   frameRateLimiter
 *     This parameter returns value of frame rate limiter
 *   swVSyncEnabled
 *     This parameter returns value of SW VSync flag (zero for disabled,
 *     non-zero for enabled)
 *   cudaEnabled
 *     This parameter returns whether CUDA is enabled or not
 *   pluginPteBlitEnabled
 *     This parameter returns whether to use plugin pte blit path
 *   disableWddm1xPreemption
 *     This parameter returns whether to disable WDDM 1.x Preemption or not
 *   debugBuffer
 *     This parameter specifies a pointer to memory which is filled with
 *     debugging information.
 *   debugBufferSize
 *     This parameter specifies the size of the debugging buffer in bytes.
 *   guestFbOffset
 *     This parameter returns FB offset start address for VM
 *   mappableCpuHostAperture
 *     This parameter returns mappable CPU host aperture size
 *   linuxInterruptOptimization
 *     This parameter returns whether stall interrupts are enabled/disabled for
 *     Linux VM
 *   vgpuDeviceCapsBits
 *      This parameter specifies CAP bits to ON/OFF features from guest OS.
 *      CAPS_SW_VSYNC_ENABLED
 *          cap bit to indicate if SW VSync flag enabled/disabled.
 *          Please note, currently, guest doesn't honour this bit.
 *      CAPS_CUDA_ENABLED
 *          cap bit to indicate if CUDA enabled/disabled.
 *          Please note, currently, guest doesn't honour this bit.
 *      CAPS_WDDM1_PREEMPTION_DISABLED
 *          cap bit to indicate if WDDM 1.x Preemption disabled/enabled.
 *          Please note, currently, guest doesn't honour this bit.
 *      CAPS_LINUX_INTERRUPT_OPTIMIZATION_ENABLED
 *          cap bit to indicate if stall interrupts are enabled/disabled for
 *          Linux VM. Please note, currently, guest doesn't honour this bit.
 *      CAPS_PTE_BLIT_ENABLED
 *          cap bit to indicate if PTE blit is enabled/disabled.
 *          Please note, currently, guest doesn't honour this bit.
 *      CAPS_PDE_BLIT_ENABLED
 *          cap bit to indicate if PDE blit is enabled/disabled.
 *      CAPS_GET_PDE_INFO_CTRL_DISABLED
 *          cap bit to indicate if GET_PDE_INFO RM Ctrl is disabled/enabled.
 *      CAPS_GUEST_FB_OFFSET_DISABLED
 *          cap bit to indicate if FB Offset is exposed to guest or not.
 *          If set, FB Offset is not exposed to guest.
 *      CAPS_CILP_DISABLED_ON_WDDM
 *          cap bit to indicate if CILP on WDDM disabled/enabled.
 *      CAPS_UPDATE_DOORBELL_TOKEN_ENABLED
 *          cap bit to indicate if guest needs to use doorbell token value updated
 *          dynamically by host after migration.
 *      CAPS_SRIOV_ENABLED
 *          Cap bit to indicate if the vGPU is running in SRIOV mode or not.
 *      CAPS_GUEST_MANAGED_VA_ENABLED
 *          Cap bit to indicate if the Guest is managing the VA.
 *      CAPS_VGPU_1TO1_COMPTAG_ENABLED
 *          Cap bit to indicate if the 1to1 comptag enabled. This is always TRUE
 *          when SR-IOV is enabled.
 *      CAPS_MBP_ENABLED
 *          Cap bit to indicate if the Mid Buffer Preemption  enabled.
 *      CAPS_ASYNC_MBP_ENABLED
 *          Cap bit to indicate if the asynchronus Mid buffer Preemption enabled.
 *      CAPS_TLB_INVALIDATE_ENABLED
 *          Cap bit to indicate if the vGPU supports TLB Invalidation operation or not.
 *      CAPS_PTE_BLIT_FOR_BAR1_PT_UPDATE_ENABLED
 *          Cap bit to indicate if the vGPU supports PTE blit for page table updates using BAR1
 *      CAPS_SRIOV_HEAVY_ENABLED
 *          Cap bit to indicate if vGPU is running in SRIOV Heavy mode or not.
 *          When set true SRIOV Heavy is enabled.
 *          When set false and CAPS_SRIOV_ENABLED is set true, SRIOV Standard is enabled.
 *      CAPS_TIMESLICE_OVERRIDE_ENABLED
 *          Cap bit to indicate whether TSG timeslice override is enabled or not.
 *          When set true, TSG timeslice override is enabled.
 *          When false, TSG timeslice override is disabled.
 *      CAPS_GUEST_HIBERNATION_ENABLED
 *          Cap bit to indicate whether Guest OS Hibernation is supported or not.
 *   uvmEnabledFeatures
 *      This parameter returns mask of UVM enabled features on vGPU. It comprises of
 *      UVM managed APIs and replayable faults that are enabled or disabled based on
 *      vGPU version.
 *   enableKmdSysmemScratch
 *      This parameter is used to overwrite guest regkey PreferSystemMemoryScratch.
 *      Setting vgpu parameter "vgpu_enable_kmd_sysmem_scratch" in plugin will
 *      set this parameter. If the parameter is set, guest moves shader buffer
 *      allocation from FB to sysmem.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */
#define VGPU_FBMEMCE_PIPELINED_SEMA_MEM_UPPER_ADDR(addr)    (((NvU64)addr) + VGPU_FBMEMCE_PIPELINED_SEMA_MEM_UPPER_OFFSET)
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG                                                                 (0xa0800301) /* finn: Evaluated from "(FINN_KEPLER_DEVICE_VGPU_VGPU_OTHERS_INTERFACE_ID << 8) | NVA080_CTRL_VGPU_GET_CONFIG_PARAMS_MESSAGE_ID" */

#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_SW_VSYNC_ENABLED                            0:0
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_SW_VSYNC_ENABLED_FALSE                     (0x00000000)
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_SW_VSYNC_ENABLED_TRUE                      (0x00000001)
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_CUDA_ENABLED                                1:1
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_CUDA_ENABLED_FALSE                         (0x00000000)
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_CUDA_ENABLED_TRUE                          (0x00000001)
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_WDDM1_PREEMPTION_DISABLED                   2:2
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_WDDM1_PREEMPTION_DISABLED_FALSE            (0x00000000)
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_WDDM1_PREEMPTION_DISABLED_TRUE             (0x00000001)
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_LINUX_INTERRUPT_OPTIMIZATION_ENABLED        3:3
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_LINUX_INTERRUPT_OPTIMIZATION_ENABLED_FALSE (0x00000000)
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_LINUX_INTERRUPT_OPTIMIZATION_ENABLED_TRUE  (0x00000001)
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_PTE_BLIT_ENABLED                            4:4
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_PTE_BLIT_ENABLED_FALSE                     (0x00000000)
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_PTE_BLIT_ENABLED_TRUE                      (0x00000001)
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_PDE_BLIT_ENABLED                            5:5
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_PDE_BLIT_ENABLED_FALSE                     (0x00000000)
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_PDE_BLIT_ENABLED_TRUE                      (0x00000001)
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_GET_PDE_INFO_CTRL_DISABLED                  6:6
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_GET_PDE_INFO_CTRL_DISABLED_FALSE           (0x00000000)
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_GET_PDE_INFO_CTRL_DISABLED_TRUE            (0x00000001)
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_GUEST_FB_OFFSET_DISABLED                    7:7
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_GUEST_FB_OFFSET_DISABLED_FALSE             (0x00000000)
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_GUEST_FB_OFFSET_DISABLED_TRUE              (0x00000001)
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_CILP_DISABLED_ON_WDDM                       8:8
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_CILP_DISABLED_ON_WDDM_FALSE                (0x00000000)
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_CILP_DISABLED_ON_WDDM_TRUE                 (0x00000001)
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_VGPU_SEMAPHORE_DISABLED                     9:9
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_VGPU_SEMAPHORE_DISABLED_FALSE              (0x00000000)
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_VGPU_SEMAPHORE_DISABLED_TRUE               (0x00000001)
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_UPDATE_DOORBELL_TOKEN_ENABLED               10:10
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_UPDATE_DOORBELL_TOKEN_ENABLED_FALSE        (0x00000000)
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_UPDATE_DOORBELL_TOKEN_ENABLED_TRUE         (0x00000001)
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_SRIOV_ENABLED                               11:11
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_SRIOV_ENABLED_FALSE                        (0x00000000)
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_SRIOV_ENABLED_TRUE                         (0x00000001)
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_GUEST_MANAGED_VA_ENABLED                    12:12
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_GUEST_MANAGED_VA_ENABLED_FALSE             (0x00000000)
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_GUEST_MANAGED_VA_ENABLED_TRUE              (0x00000001)
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_VGPU_1TO1_COMPTAG_ENABLED                   13:13
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_VGPU_1TO1_COMPTAG_ENABLED_FALSE            (0x00000000)
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_VGPU_1TO1_COMPTAG_ENABLED_TRUE             (0x00000001)
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_MBP_ENABLED                                 14:14
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_MBP_ENABLED_FALSE                          (0x00000000)
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_MBP_ENABLED_TRUE                           (0x00000001)
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_ASYNC_MBP_ENABLED                           15:15
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_ASYNC_MBP_ENABLED_FALSE                    (0x00000000)
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_ASYNC_MBP_ENABLED_TRUE                     (0x00000001)
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_TLB_INVALIDATE_ENABLED                      16:16
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_TLB_INVALIDATE_ENABLED_FALSE               (0x00000000)
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_TLB_INVALIDATE_ENABLED_TRUE                (0x00000001)
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_PTE_BLIT_FOR_BAR1_PT_UPDATE_ENABLED         17:17
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_PTE_BLIT_FOR_BAR1_PT_UPDATE_ENABLED_FALSE  (0x00000000)
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_PTE_BLIT_FOR_BAR1_PT_UPDATE_ENABLED_TRUE   (0x00000001)
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_GPU_DIRECT_RDMA_ENABLED                     18:18
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_GPU_DIRECT_RDMA_ENABLED_FALSE              (0x00000000)
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_GPU_DIRECT_RDMA_ENABLED_TRUE               (0x00000001)
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_SRIOV_HEAVY_ENABLED                         19:19
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_SRIOV_HEAVY_ENABLED_FALSE                  (0x00000000)
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_SRIOV_HEAVY_ENABLED_TRUE                   (0x00000001)
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_TIMESLICE_OVERRIDE_ENABLED                  20:20
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_TIMESLICE_OVERRIDE_ENABLED_FALSE           (0x00000000)
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_TIMESLICE_OVERRIDE_ENABLED_TRUE            (0x00000001)
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_USE_NON_STALL_LINUX_EVENTS                  21:21
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_USE_NON_STALL_LINUX_EVENTS_FALSE           (0x00000000)
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_USE_NON_STALL_LINUX_EVENTS_TRUE            (0x00000001)
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_GUEST_HIBERNATION_ENABLED                  22:22
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_GUEST_HIBERNATION_ENABLED_FALSE            (0x00000000)
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_GUEST_HIBERNATION_ENABLED_TRUE             (0x00000001)
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_VF_INVALIDATE_TLB_TRAP_ENABLED            23:23
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_VF_INVALIDATE_TLB_TRAP_ENABLED_FALSE       (0x00000000)
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_VGPU_DEV_CAPS_VF_INVALIDATE_TLB_TRAP_ENABLED_TRUE        (0x00000001)

/* UVM supported features */
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_UVM_FEATURES_REPLAYABLE_FAULTS_ENABLED                    0:0
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_UVM_FEATURES_REPLAYABLE_FAULTS_ENABLED_FALSE             (0x00000000)
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_UVM_FEATURES_REPLAYABLE_FAULTS_ENABLED_TRUE              (0x00000001)
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_UVM_FEATURES_API_ENABLED                                  1:1
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_UVM_FEATURES_API_ENABLED_FALSE                           (0x00000000)
#define NVA080_CTRL_CMD_VGPU_GET_CONFIG_PARAMS_UVM_FEATURES_API_ENABLED_TRUE                            (0x00000001)

#define NVA080_CTRL_VGPU_GET_CONFIG_PARAMS_MESSAGE_ID (0x1U)

typedef struct NVA080_CTRL_VGPU_GET_CONFIG_PARAMS {
    NvU32  frameRateLimiter;
    NvU32  swVSyncEnabled;
    NvU32  cudaEnabled;
    NvU32  pluginPteBlitEnabled;
    NvU32  disableWddm1xPreemption;
    NvU32  debugBufferSize;
    NV_DECLARE_ALIGNED(NvP64 debugBuffer, 8);
    NV_DECLARE_ALIGNED(NvU64 guestFbOffset, 8);
    NV_DECLARE_ALIGNED(NvU64 mappableCpuHostAperture, 8);
    NvU32  linuxInterruptOptimization;
    NvU32  vgpuDeviceCapsBits;
    NvU32  maxPixels;
    NvU32  uvmEnabledFeatures;
    NvBool enableKmdSysmemScratch;
} NVA080_CTRL_VGPU_GET_CONFIG_PARAMS;



/* _ctrla080_h_ */
