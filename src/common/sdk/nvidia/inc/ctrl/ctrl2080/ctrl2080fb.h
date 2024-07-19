/*
 * SPDX-FileCopyrightText: Copyright (c) 2006-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrl2080/ctrl2080fb.finn
//

#include "ctrl/ctrl2080/ctrl2080base.h"

/* NV20_SUBDEVICE_XX fb control commands and parameters */

#include "nvlimits.h"
#include "nvcfg_sdk.h"

/*
 * NV2080_CTRL_FB_INFO
 *
 * This structure represents a single 32bit fb engine value.  Clients
 * request a particular fb engine value by specifying a unique fb
 * information index.
 *
 * Legal fb information index values are:
 *   NV2080_CTRL_FB_INFO_INDEX_TILE_REGION_COUNT
 *     This index is used to request the number of tiled regions supported
 *     by the associated subdevice.  The return value is GPU
 *     implementation-dependent.  A return value of 0 indicates the GPU
 *     does not support tiling.
 *   NV2080_CTRL_FB_INFO_INDEX_COMPRESSION_SIZE
 *     This index is used to request the amount of compression (in bytes)
 *     supported by the associated subdevice.  The return value is GPU
 *     implementation-dependent.  A return value of 0 indicates the GPU
 *     does not support compression.
 *   Nv2080_CTRL_FB_INFO_INDEX_DRAM_PAGE_STRIDE
 *     This index is used to request the DRAM page stride (in bytes)
 *     supported by the associated subdevice.  The return value is GPU
 *     implementation-dependent.
 *   NV2080_CTRL_FB_INFO_INDEX_TILE_REGION_FREE_COUNT
 *     This index is used to request the number of free tiled regions on
 *     the associated subdevice.  The return value represents the current
 *     number of free tiled regions at the time the command is processed and
 *     is not guaranteed to remain unchanged.  A return value of 0 indicates
 *     that there are no available tiled regions on the associated subdevice.
 *   NV2080_CTRL_FB_INFO_INDEX_PARTITION_COUNT
 *     This index is used to request the number of frame buffer partitions
 *     on the associated subdevice. Starting with Fermi there are now two units
 *     with the name framebuffer partitions. On those chips this index returns
 *     the number of FBPAs. For number of FBPs use
 *     NV2080_CTRL_FB_INFO_INDEX_FBP_COUNT.
 *     This an SMC aware attribute, thus necessary partition subscription is
 *     required if the device is partitioned.
 *   NV2080_CTRL_FB_INFO_INDEX_RAM_SIZE
 *     This index is used to request the amount of framebuffer memory in
 *     kilobytes physically present on the associated subdevice.  This
 *     value will never exceed the value reported by
 *     NV2080_CTRL_FB_INFO_INDEX_TOTAL_RAM_SIZE.
 *     This an SMC aware attribute, so the per-partition framebuffer memory
 *     size will be returned when the client has a partition subscription.
 *   NV2080_CTRL_FB_INFO_INDEX_TOTAL_RAM_SIZE
 *     This index is used to request the total amount of video memory in
 *     kilobytes for use with the associated subdevice.  This value will
 *     reflect both framebuffer memory as well as any system memory dedicated
 *     for use with the subdevice.
 *     This an SMC aware attribute, so the per-partition video memory size
 *     will be returned when the client has a partition subscription.
 *   NV2080_CTRL_FB_INFO_INDEX_HEAP_SIZE
 *     This index is used to request the amount of total RAM in kilobytes
 *     available for user allocations.  This value reflects the total ram
 *     size less the amount of memory reserved for internal use.
 *     This an SMC aware attribute, thus necessary partition subscription is
 *     required if the device is partitioned.
 *   NV2080_CTRL_FB_INFO_INDEX_HEAP_START
 *     This index is used to request the offset for start of heap in
 *     kilobytes.
 *     This an SMC aware attribute, thus necessary partition subscription is
 *     required if the device is partitioned.
 *   NV2080_CTRL_FB_INFO_INDEX_HEAP_FREE
 *     This index is used to request the available amount of video memory in
 *     kilobytes for use with the associated subdevice or the SMC partition.
 *     This an SMC aware attribute, thus necessary partition subscription is
 *     required to query per partition information, if the device is partitioned.
 *     Alternatively, the SMC/MIG monitor capability can be acquired to query
 *     aggregate available memory across all the valid partitions.
 *   NV2080_CTRL_FB_INFO_INDEX_MAPPABLE_HEAP_SIZE
 *     This index reflects the amount of heap memory in kilobytes that
 *     is accessible by the CPU.  On subdevices with video memory sizes that
 *     exceed the amount that can be bus mappable this value will be less
 *     than that reported by NV2080_CTRL_FB_INFO_INDEX_HEAP_SIZE.
 *     This an SMC aware attribute, thus necessary partition subscription is
 *     required if the device is partitioned.
 *   NV2080_CTRL_FB_INFO_INDEX_BUS_WIDTH
 *     This index is used to request the FB bus bandwidth on the associated
 *     subdevice.
 *   NV2080_CTRL_FB_INFO_INDEX_RAM_CFG
 *     This index is used to request the implementation-dependent RAM
 *     configuration value of the associated subdevice.
 *   NV2080_CTRL_FB_INFO_INDEX_RAM_TYPE
 *     This index is used to request the type of RAM used for the framebuffer
 *     on the associated subdevice.  Legal RAM types include:
 *       NV2080_CTRL_FB_INFO_RAM_TYPE_UNKNOWN
 *       NV2080_CTRL_FB_INFO_RAM_TYPE_SDRAM
 *       NV2080_CTRL_FB_INFO_RAM_TYPE_DDR1
 *       NV2080_CTRL_FB_INFO_RAM_TYPE_DDR2
 *       NV2080_CTRL_FB_INFO_RAM_TYPE_GDDR2
 *       NV2080_CTRL_FB_INFO_RAM_TYPE_GDDR3
 *       NV2080_CTRL_FB_INFO_RAM_TYPE_GDDR4
 *       NV2080_CTRL_FB_INFO_RAM_TYPE_DDR3
 *       NV2080_CTRL_FB_INFO_RAM_TYPE_GDDR5
 *       NV2080_CTRL_FB_INFO_RAM_TYPE_GDDR5X
 *       NV2080_CTRL_FB_INFO_RAM_TYPE_GDDR6
 *       NV2080_CTRL_FB_INFO_RAM_TYPE_GDDR6X
 *       NV2080_CTRL_FB_INFO_RAM_TYPE_LPDDR2
 *       NV2080_CTRL_FB_INFO_RAM_TYPE_LPDDR4
 *       NV2080_CTRL_FB_INFO_RAM_TYPE_LPDDR5
 *   NV2080_CTRL_FB_INFO_INDEX_BANK_COUNT
 *     This index is used to request the number of FB banks on the associated
 *     subdevice.
 *   NV2080_CTRL_FB_INFO_INDEX_OVERLAY_OFFSET_ADJUSTMENT
 *     This index is used to request the offset relative to the start of the
 *     overlay surface(s), in bytes, at which scanout should happen if the
 *     primary and the overlay surfaces are all aligned on large page
 *     boundaries.
 *   NV2080_CTRL_FB_INFO_INDEX_GPU_VADDR_SPACE_SIZE_KB
 *     This index is used to request the size of the GPU's virtual address
 *     space in kilobytes.
 *   NV2080_CTRL_FB_INFO_INDEX_GPU_VADDR_HEAP_SIZE_KB
 *     This index is used to request the size of the GPU's virtual address
 *     space heap (minus RM-reserved space) in kilobytes.
 *   NV2080_CTRL_FB_INFO_INDEX_GPU_VADDR_MAPPBLE_SIZE_KB
 *     This index is used to request the size of the GPU's BAR1 mappable
 *     virtual address space in kilobytes.
 *   NV2080_CTRL_FB_INFO_INDEX_EFFECTIVE_BW
 *     This index is deprecated, and returns zero value.
 *   NV2080_CTRL_FB_INFO_INDEX_PARTITION_MASK
 *   NV2080_CTRL_FB_INFO_INDEX_PARTITION_MASK_0
 *   NV2080_CTRL_FB_INFO_INDEX_PARTITION_MASK_1
 *     This index is used to request the mask of currently active partitions.
 *     Each active partition has an ID that's equivalent to the corresponding
 *     bit position in the mask.
 *     This an SMC aware attribute, thus necessary partition subscription is
 *     required if the device is partitioned.
 *     This value is moving from 32bits to 64bits, so PARTITION_MASK
 *     (though kept for backwards compatibility on older chips), on newer chips
 *     will be replaced by:
 *     PARTITION_MASK_0 for the lower 32bits
 *     PARTITION_MASK_1 for the upper 32bits
 *     Note that PARTITION_MASK and PARTITION_MASK_0 are handled the same, and
 *     use the same enum value.
 *   NV2080_CTRL_FB_INFO_INDEX_LTC_MASK
 *   NV2080_CTRL_FB_INFO_INDEX_LTC_MASK_0
 *   NV2080_CTRL_FB_INFO_INDEX_LTC_MASK_1
 *     This index is used to request the mask of currently active LTCs.
 *     Each active LTC has an ID that's equivalent to the corresponding
 *     bit position in the mask.
 *     This an SMC aware attribute, thus necessary partition subscription is
 *     required if the device is partitioned.
 *     This value is moving from 32bits to 64bits, so LTC_MASK
 *     (though kept for backwards compatibility on older chips), on newer chips
 *     will be replaced by:
 *     LTC_MASK_0 for the lower 32bits
 *     LTC_MASK_1 for the upper 32bits
 *     Note that LTC_MASK and LTC_MASK_0 are handled the same, and
 *     use the same enum value.
 *   NV2080_CTRL_FB_INFO_INDEX_VISTA_RESERVED_HEAP_SIZE
 *     This index is used to request the amount of total RAM in kilobytes
 *     reserved for internal RM allocations on Vista.  This will need to
 *     be subtracted from the total heap size to get the amount available to
 *     KMD.
 *     This an SMC aware attribute, thus necessary partition subscription is
 *     required if the device is partitioned.
 *   NV2080_CTRL_FB_INFO_INDEX_RAM_LOCATION
 *     This index is used to distinguish between different memory
 *     configurations.
 *   NV2080_CTRL_FB_INFO_INDEX_FB_IS_BROKEN
 *     This index is used to check if the FB is functional
 *   NV2080_CTRL_FB_INFO_INDEX_FBP_COUNT
 *     This index is used to get the number of FBPs on the subdevice. This
 *     field is not to be confused with
 *     NV2080_CTRL_FB_INFO_INDEX_PARTITION_COUNT (returns number of FBPAs).
 *     Starting with Fermi the term partition is an ambiguous term, both FBP
 *     and FBPA mean FB partitions. The FBPA is the low level DRAM controller,
 *     while a FBP is the aggregation of one or more FBPAs, L2, ROP, and some
 *     other units.
 *     This an SMC aware attribute, thus necessary partition subscription is
 *     required if the device is partitioned.
 *   NV2080_CTRL_FB_INFO_INDEX_L2CACHE_SIZE
 *     This index is used to get the size of the L2 cache in Bytes.
 *     A value of zero indicates that the L2 cache isn't supported on the
 *     associated subdevice.
 *   NV2080_CTRL_FB_INFO_INDEX_MEMORYINFO_VENDOR_ID
 *     This index is used to get the memory vendor ID information from
 *     the Memory Information Table in the VBIOS.  Legal memory Vendor ID
 *     values include:
 *       NV2080_CTRL_FB_INFO_MEMORYINFO_VENDOR_ID_UNKNOWN
 *       NV2080_CTRL_FB_INFO_MEMORYINFO_VENDOR_ID_RESERVED
 *       NV2080_CTRL_FB_INFO_MEMORYINFO_VENDOR_ID_SAMSUNG
 *       NV2080_CTRL_FB_INFO_MEMORYINFO_VENDOR_ID_QIMONDA
 *       NV2080_CTRL_FB_INFO_MEMORYINFO_VENDOR_ID_ELPIDA
 *       NV2080_CTRL_FB_INFO_MEMORYINFO_VENDOR_ID_ETRON
 *       NV2080_CTRL_FB_INFO_MEMORYINFO_VENDOR_ID_NANYA
 *       NV2080_CTRL_FB_INFO_MEMORYINFO_VENDOR_ID_HYNIX
 *       NV2080_CTRL_FB_INFO_MEMORYINFO_VENDOR_ID_MOSEL
 *       NV2080_CTRL_FB_INFO_MEMORYINFO_VENDOR_ID_WINBOND
 *       NV2080_CTRL_FB_INFO_MEMORYINFO_VENDOR_ID_ESMT
 *       NV2080_CTRL_FB_INFO_MEMORYINFO_VENDOR_ID_MICRON
 *   NV2080_CTRL_FB_INFO_INDEX_BAR1_AVAIL_SIZE
 *     This index is used to request the amount of unused bar1 space. The
 *     data returned is a value in KB. It is not guaranteed to be entirely
 *     accurate since it is a snapshot at a particular time and can
 *     change quickly.
 *   NV2080_CTRL_FB_INFO_INDEX_BAR1_MAX_CONTIGUOUS_AVAIL_SIZE
 *     This index is used to request the amount of largest unused contiguous
 *     block in bar1 space.  The data returned is a value in KB. It is not
 *     guaranteed to be entirely accurate since it is a snapshot at a particular
 *     time and can change quickly.
 *   NV2080_CTRL_FB_INFO_INDEX_USABLE_RAM_SIZE
 *     This index is used to request the amount of usable framebuffer memory in
 *     kilobytes physically present on the associated subdevice.  This
 *     value will never exceed the value reported by
 *     NV2080_CTRL_FB_INFO_INDEX_TOTAL_RAM_SIZE.
 *     This an SMC aware attribute, thus necessary partition subscription is
 *     required if the device is partitioned.
 *   NV2080_CTRL_FB_INFO_INDEX_LTC_COUNT
 *     Returns the active LTC count across all active FBPs.
 *     This an SMC aware attribute, thus necessary partition subscription is
 *     required if the device is partitioned.
 *   NV2080_CTRL_FB_INFO_INDEX_LTS_COUNT
 *     Returns the active LTS count across all active LTCs.
 *     This an SMC aware attribute, thus necessary partition subscription is
 *     required if the device is partitioned.
 *   NV2080_CTRL_FB_INFO_INDEX_PSEUDO_CHANNEL_MODE
 *     This is used to identify if pseudo-channel mode is enabled for HBM
 *   NV2080_CTRL_FB_INFO_INDEX_SMOOTHDISP_RSVD_BAR1_SIZE
 *     This is used by WDDM-KMD to determine whether and how much RM reserved BAR1 for smooth transition
 *   NV2080_CTRL_FB_INFO_INDEX_HEAP_OFFLINE_SIZE
 *     Returns the total size of the all dynamically offlined pages in KiB
 *   NV2080_CTRL_FB_INFO_INDEX_1TO1_COMPTAG_ENABLED
 *     Returns true if 1to1 comptag is enabled
 *   NV2080_CTRL_FB_INFO_INDEX_SUSPEND_RESUME_RSVD_SIZE
 *     Returns the total size of the memory(FB) that will saved/restored during save/restore cycle
 *   NV2080_CTRL_FB_INFO_INDEX_ALLOW_PAGE_RETIREMENT
 *     Returns true if page retirement is allowed
 *   NV2080_CTRL_FB_INFO_POISON_FUSE_ENABLED
 *     Returns true if poison fuse is enabled
 *   NV2080_CTRL_FB_INFO_FBPA_ECC_ENABLED
 *     Returns true if ECC is enabled for FBPA
 *   NV2080_CTRL_FB_INFO_DYNAMIC_PAGE_OFFLINING_ENABLED
 *     Returns true if dynamic page blacklisting is enabled
 *   NV2080_CTRL_FB_INFO_INDEX_FORCED_BAR1_64KB_MAPPING_ENABLED
 *     Returns true if 64KB mapping on BAR1 is force-enabled
 *   NV2080_CTRL_FB_INFO_INDEX_P2P_MAILBOX_SIZE
 *     Returns the P2P mailbox size to be allocated by the client. 
 *     Returns 0 if the P2P mailbox is allocated by RM.
 *   NV2080_CTRL_FB_INFO_INDEX_P2P_MAILBOX_ALIGNMENT_SIZE
 *     Returns the P2P mailbox alignment requirement.
 *     Returns 0 if the P2P mailbox is allocated by RM.
 *   NV2080_CTRL_FB_INFO_INDEX_P2P_MAILBOX_BAR1_MAX_OFFSET_64KB
 *     Returns the P2P mailbox max offset requirement.
 *     Returns 0 if the P2P mailbox is allocated by RM.
 *   NV2080_CTRL_FB_INFO_INDEX_PROTECTED_MEM_SIZE_TOTAL_KB
 *     Returns total protected memory when memory protection is enabled
 *     Returns 0 when memory protection is not enabled.
 *   NV2080_CTRL_FB_INFO_INDEX_PROTECTED_MEM_SIZE_FREE_KB
 *     Returns protected memory available for allocation when memory
 *     protection is enabled.
 *     Returns 0 when memory protection is not enabled.
 *   NV2080_CTRL_FB_INFO_INDEX_ECC_STATUS_SIZE
 *     Returns the ECC status size (corresponds to subpartitions or channels
 *     depending on architecture/memory type).
 *   NV2080_CTRL_FB_INFO_INDEX_IS_ZERO_FB
 *      Returns true if FB is not present on this chip
 */
typedef NVXXXX_CTRL_XXX_INFO NV2080_CTRL_FB_INFO;

/* valid fb info index values */
#define NV2080_CTRL_FB_INFO_INDEX_TILE_REGION_COUNT                (0x00000000U) // Deprecated
#define NV2080_CTRL_FB_INFO_INDEX_COMPRESSION_SIZE                 (0x00000001U)
#define NV2080_CTRL_FB_INFO_INDEX_DRAM_PAGE_STRIDE                 (0x00000002U)
#define NV2080_CTRL_FB_INFO_INDEX_TILE_REGION_FREE_COUNT           (0x00000003U)
#define NV2080_CTRL_FB_INFO_INDEX_PARTITION_COUNT                  (0x00000004U)
#define NV2080_CTRL_FB_INFO_INDEX_BAR1_SIZE                        (0x00000005U)
#define NV2080_CTRL_FB_INFO_INDEX_BANK_SWIZZLE_ALIGNMENT           (0x00000006U)
#define NV2080_CTRL_FB_INFO_INDEX_RAM_SIZE                         (0x00000007U)
#define NV2080_CTRL_FB_INFO_INDEX_TOTAL_RAM_SIZE                   (0x00000008U)
#define NV2080_CTRL_FB_INFO_INDEX_HEAP_SIZE                        (0x00000009U)
#define NV2080_CTRL_FB_INFO_INDEX_MAPPABLE_HEAP_SIZE               (0x0000000AU)
#define NV2080_CTRL_FB_INFO_INDEX_BUS_WIDTH                        (0x0000000BU)
#define NV2080_CTRL_FB_INFO_INDEX_RAM_CFG                          (0x0000000CU)
#define NV2080_CTRL_FB_INFO_INDEX_RAM_TYPE                         (0x0000000DU)
#define NV2080_CTRL_FB_INFO_INDEX_BANK_COUNT                       (0x0000000EU)
#define NV2080_CTRL_FB_INFO_INDEX_OVERLAY_OFFSET_ADJUSTMENT        (0x0000000FU) // Deprecated (index reused to return 0)
#define NV2080_CTRL_FB_INFO_INDEX_GPU_VADDR_SPACE_SIZE_KB          (0x0000000FU) // Deprecated (index reused to return 0)
#define NV2080_CTRL_FB_INFO_INDEX_GPU_VADDR_HEAP_SIZE_KB           (0x0000000FU) // Deprecated (index reused to return 0)
#define NV2080_CTRL_FB_INFO_INDEX_GPU_VADDR_MAPPBLE_SIZE_KB        (0x0000000FU) // Deprecated (index reused to return 0)
#define NV2080_CTRL_FB_INFO_INDEX_EFFECTIVE_BW                     (0x0000000FU) // Deprecated (index reused to return 0)
#define NV2080_CTRL_FB_INFO_INDEX_FB_TAX_SIZE_KB                   (0x00000010U)
#define NV2080_CTRL_FB_INFO_INDEX_HEAP_BASE_KB                     (0x00000011U)
#define NV2080_CTRL_FB_INFO_INDEX_LARGEST_FREE_REGION_SIZE_KB      (0x00000012U)
#define NV2080_CTRL_FB_INFO_INDEX_LARGEST_FREE_REGION_BASE_KB      (0x00000013U)
#define NV2080_CTRL_FB_INFO_INDEX_PARTITION_MASK                   (0x00000014U)
#define NV2080_CTRL_FB_INFO_INDEX_VISTA_RESERVED_HEAP_SIZE         (0x00000015U)
#define NV2080_CTRL_FB_INFO_INDEX_HEAP_FREE                        (0x00000016U)
#define NV2080_CTRL_FB_INFO_INDEX_RAM_LOCATION                     (0x00000017U)
#define NV2080_CTRL_FB_INFO_INDEX_FB_IS_BROKEN                     (0x00000018U)
#define NV2080_CTRL_FB_INFO_INDEX_FBP_COUNT                        (0x00000019U)
#define NV2080_CTRL_FB_INFO_INDEX_FBP_MASK                         (0x0000001AU)
#define NV2080_CTRL_FB_INFO_INDEX_L2CACHE_SIZE                     (0x0000001BU)
#define NV2080_CTRL_FB_INFO_INDEX_MEMORYINFO_VENDOR_ID             (0x0000001CU)
#define NV2080_CTRL_FB_INFO_INDEX_BAR1_AVAIL_SIZE                  (0x0000001DU)
#define NV2080_CTRL_FB_INFO_INDEX_HEAP_START                       (0x0000001EU)
#define NV2080_CTRL_FB_INFO_INDEX_BAR1_MAX_CONTIGUOUS_AVAIL_SIZE   (0x0000001FU)
#define NV2080_CTRL_FB_INFO_INDEX_USABLE_RAM_SIZE                  (0x00000020U)
#define NV2080_CTRL_FB_INFO_INDEX_TRAINIG_2T                       (0x00000021U)
#define NV2080_CTRL_FB_INFO_INDEX_LTC_COUNT                        (0x00000022U)
#define NV2080_CTRL_FB_INFO_INDEX_LTS_COUNT                        (0x00000023U)
#define NV2080_CTRL_FB_INFO_INDEX_L2CACHE_ONLY_MODE                (0x00000024U)
#define NV2080_CTRL_FB_INFO_INDEX_PSEUDO_CHANNEL_MODE              (0x00000025U)
#define NV2080_CTRL_FB_INFO_INDEX_SMOOTHDISP_RSVD_BAR1_SIZE        (0x00000026U)
#define NV2080_CTRL_FB_INFO_INDEX_HEAP_OFFLINE_SIZE                (0x00000027U)
#define NV2080_CTRL_FB_INFO_INDEX_1TO1_COMPTAG_ENABLED             (0x00000028U)
#define NV2080_CTRL_FB_INFO_INDEX_SUSPEND_RESUME_RSVD_SIZE         (0x00000029U)
#define NV2080_CTRL_FB_INFO_INDEX_ALLOW_PAGE_RETIREMENT            (0x0000002AU)
#define NV2080_CTRL_FB_INFO_INDEX_LTC_MASK                         (0x0000002BU)
#define NV2080_CTRL_FB_INFO_POISON_FUSE_ENABLED                    (0x0000002CU)
#define NV2080_CTRL_FB_INFO_FBPA_ECC_ENABLED                       (0x0000002DU)
#define NV2080_CTRL_FB_INFO_DYNAMIC_PAGE_OFFLINING_ENABLED         (0x0000002EU)
#define NV2080_CTRL_FB_INFO_INDEX_FORCED_BAR1_64KB_MAPPING_ENABLED (0x0000002FU)
#define NV2080_CTRL_FB_INFO_INDEX_P2P_MAILBOX_SIZE                 (0x00000030U)
#define NV2080_CTRL_FB_INFO_INDEX_P2P_MAILBOX_ALIGNMENT            (0x00000031U)
#define NV2080_CTRL_FB_INFO_INDEX_P2P_MAILBOX_BAR1_MAX_OFFSET_64KB (0x00000032U)
#define NV2080_CTRL_FB_INFO_INDEX_PROTECTED_MEM_SIZE_TOTAL_KB      (0x00000033U)
#define NV2080_CTRL_FB_INFO_INDEX_PROTECTED_MEM_SIZE_FREE_KB       (0x00000034U)
#define NV2080_CTRL_FB_INFO_INDEX_ECC_STATUS_SIZE                  (0x00000035U)
#define NV2080_CTRL_FB_INFO_INDEX_IS_ZERO_FB                       (0x00000036U)
#define NV2080_CTRL_FB_INFO_INDEX_PARTITION_MASK_0                 (NV2080_CTRL_FB_INFO_INDEX_PARTITION_MASK)
#define NV2080_CTRL_FB_INFO_INDEX_PARTITION_MASK_1                 (0x00000037U)
#define NV2080_CTRL_FB_INFO_INDEX_LTC_MASK_0                       (NV2080_CTRL_FB_INFO_INDEX_LTC_MASK)
#define NV2080_CTRL_FB_INFO_INDEX_LTC_MASK_1                       (0x00000038U)
#define NV2080_CTRL_FB_INFO_MAX_LIST_SIZE                          (0x00000039U)

#define NV2080_CTRL_FB_INFO_INDEX_MAX                              (0x38U) /* finn: Evaluated from "(NV2080_CTRL_FB_INFO_MAX_LIST_SIZE - 1)" */

/* valid fb RAM type values */
#define NV2080_CTRL_FB_INFO_RAM_TYPE_UNKNOWN                       (0x00000000U)
#define NV2080_CTRL_FB_INFO_RAM_TYPE_SDRAM                         (0x00000001U)
#define NV2080_CTRL_FB_INFO_RAM_TYPE_DDR1                          (0x00000002U) /* SDDR and GDDR (aka DDR1 and GDDR1) */
#define NV2080_CTRL_FB_INFO_RAM_TYPE_SDDR2                         (0x00000003U) /* SDDR2 Used on NV43 and later */
#define NV2080_CTRL_FB_INFO_RAM_TYPE_DDR2                          NV2080_CTRL_FB_INFO_RAM_TYPE_SDDR2 /* Deprecated alias */
#define NV2080_CTRL_FB_INFO_RAM_TYPE_GDDR2                         (0x00000004U) /* GDDR2 Used on NV30 and some NV36 */
#define NV2080_CTRL_FB_INFO_RAM_TYPE_GDDR3                         (0x00000005U) /* GDDR3 Used on NV40 and later */
#define NV2080_CTRL_FB_INFO_RAM_TYPE_GDDR4                         (0x00000006U) /* GDDR4 Used on G80 and later (deprecated) */
#define NV2080_CTRL_FB_INFO_RAM_TYPE_SDDR3                         (0x00000007U) /* SDDR3 Used on G9x and later */
#define NV2080_CTRL_FB_INFO_RAM_TYPE_DDR3                          NV2080_CTRL_FB_INFO_RAM_TYPE_SDDR3 /* Deprecated alias */
#define NV2080_CTRL_FB_INFO_RAM_TYPE_GDDR5                         (0x00000008U) /* GDDR5 Used on GT21x and later */
#define NV2080_CTRL_FB_INFO_RAM_TYPE_LPDDR2                        (0x00000009U) /* LPDDR (Low Power SDDR) used on T2x and later. */


#define NV2080_CTRL_FB_INFO_RAM_TYPE_SDDR4                         (0x0000000CU) /* SDDR4 Used on Maxwell and later */
#define NV2080_CTRL_FB_INFO_RAM_TYPE_LPDDR4                        (0x0000000DU) /* LPDDR (Low Power SDDR) used on T21x and later.*/
#define NV2080_CTRL_FB_INFO_RAM_TYPE_HBM1                          (0x0000000EU) /* HBM1 (High Bandwidth Memory) used on GP100 */
#define NV2080_CTRL_FB_INFO_RAM_TYPE_HBM2                          (0x0000000FU) /* HBM2 (High Bandwidth Memory-pseudo channel) */
#define NV2080_CTRL_FB_INFO_RAM_TYPE_GDDR5X                        (0x00000010U) /* GDDR5X Used on GP10x */
#define NV2080_CTRL_FB_INFO_RAM_TYPE_GDDR6                         (0x00000011U) /* GDDR6 Used on TU10x */
#define NV2080_CTRL_FB_INFO_RAM_TYPE_GDDR6X                        (0x00000012U) /* GDDR6X Used on GA10x */
#define NV2080_CTRL_FB_INFO_RAM_TYPE_LPDDR5                        (0x00000013U) /* LPDDR (Low Power SDDR) used on T23x and later.*/
#define NV2080_CTRL_FB_INFO_RAM_TYPE_HBM3                          (0x00000014U) /* HBM3 (High Bandwidth Memory) v3 */



/* valid RAM LOCATION types */
#define NV2080_CTRL_FB_INFO_RAM_LOCATION_GPU_DEDICATED             (0x00000000U)
#define NV2080_CTRL_FB_INFO_RAM_LOCATION_SYS_SHARED                (0x00000001U)
#define NV2080_CTRL_FB_INFO_RAM_LOCATION_SYS_DEDICATED             (0x00000002U)

/* valid Memory Vendor ID values */
#define NV2080_CTRL_FB_INFO_MEMORYINFO_VENDOR_ID_SAMSUNG           (0x00000001U)
#define NV2080_CTRL_FB_INFO_MEMORYINFO_VENDOR_ID_QIMONDA           (0x00000002U)
#define NV2080_CTRL_FB_INFO_MEMORYINFO_VENDOR_ID_ELPIDA            (0x00000003U)
#define NV2080_CTRL_FB_INFO_MEMORYINFO_VENDOR_ID_ETRON             (0x00000004U)
#define NV2080_CTRL_FB_INFO_MEMORYINFO_VENDOR_ID_NANYA             (0x00000005U)
#define NV2080_CTRL_FB_INFO_MEMORYINFO_VENDOR_ID_HYNIX             (0x00000006U)
#define NV2080_CTRL_FB_INFO_MEMORYINFO_VENDOR_ID_MOSEL             (0x00000007U)
#define NV2080_CTRL_FB_INFO_MEMORYINFO_VENDOR_ID_WINBOND           (0x00000008U)
#define NV2080_CTRL_FB_INFO_MEMORYINFO_VENDOR_ID_ESMT              (0x00000009U)
#define NV2080_CTRL_FB_INFO_MEMORYINFO_VENDOR_ID_MICRON            (0x0000000FU)
#define NV2080_CTRL_FB_INFO_MEMORYINFO_VENDOR_ID_UNKNOWN           (0xFFFFFFFFU)

#define NV2080_CTRL_FB_INFO_PSEUDO_CHANNEL_MODE_UNSUPPORTED        (0x00000000U)
#define NV2080_CTRL_FB_INFO_PSEUDO_CHANNEL_MODE_DISABLED           (0x00000001U)
#define NV2080_CTRL_FB_INFO_PSEUDO_CHANNEL_MODE_ENABLED            (0x00000002U)

/**
 * NV2080_CTRL_CMD_FB_GET_INFO
 *
 * This command returns fb engine information for the associated GPU.
 * Requests to retrieve fb information use a list of one or more
 * NV2080_CTRL_FB_INFO structures.
 *
 *   fbInfoListSize
 *     This field specifies the number of entries on the caller's
 *     fbInfoList.
 *   fbInfoList
 *     This field specifies a pointer in the caller's address space
 *     to the buffer into which the fb information is to be returned.
 *     This buffer must be at least as big as fbInfoListSize multiplied
 *     by the size of the NV2080_CTRL_FB_INFO structure.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_OPERATING_SYSTEM
 */
#define NV2080_CTRL_CMD_FB_GET_INFO                                (0x20801301U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FB_INTERFACE_ID << 8) | NV2080_CTRL_FB_GET_INFO_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_FB_GET_INFO_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV2080_CTRL_FB_GET_INFO_PARAMS {
    NvU32 fbInfoListSize;
    NV_DECLARE_ALIGNED(NvP64 fbInfoList, 8);
} NV2080_CTRL_FB_GET_INFO_PARAMS;

#define NV2080_CTRL_CMD_FB_GET_INFO_V2 (0x20801303U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FB_INTERFACE_ID << 8) | NV2080_CTRL_FB_GET_INFO_V2_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_FB_GET_INFO_V2_PARAMS_MESSAGE_ID (0x3U)

typedef struct NV2080_CTRL_FB_GET_INFO_V2_PARAMS {
    NvU32               fbInfoListSize;
    NV2080_CTRL_FB_INFO fbInfoList[NV2080_CTRL_FB_INFO_MAX_LIST_SIZE];
} NV2080_CTRL_FB_GET_INFO_V2_PARAMS;

/*
 * NV2080_CTRL_CMD_FB_GET_BAR1_OFFSET
 *
 * This command returns the GPU virtual address of a bar1
 * allocation, given the CPU virtual address.
 *
 *   cpuVirtAddress
 *     This field specifies the associated CPU virtual address of the
 *     memory allocation.
 *   gpuVirtAddress
 *     The GPU virtual address associated with the allocation
 *     is returned in this field.
 *
 * Possible status values returned are
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV2080_CTRL_CMD_FB_GET_BAR1_OFFSET (0x20801310U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FB_INTERFACE_ID << 8) | NV2080_CTRL_FB_GET_BAR1_OFFSET_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_FB_GET_BAR1_OFFSET_PARAMS_MESSAGE_ID (0x10U)

typedef struct NV2080_CTRL_FB_GET_BAR1_OFFSET_PARAMS {
    NV_DECLARE_ALIGNED(NvP64 cpuVirtAddress, 8);
    NV_DECLARE_ALIGNED(NvU64 gpuVirtAddress, 8);
} NV2080_CTRL_FB_GET_BAR1_OFFSET_PARAMS;

/*
 * Note: Returns Zeros if no System carveout address info
 *
 * NV2080_CTRL_CMD_FB_GET_CARVEOUT_ADDRESS_INFO
 *
 * This command returns FB carveout address space information
 *
 *   StartAddr
 *     Returns the system memory address of the start of carveout space.
 *   SpaceSize
 *     Returns the size of carveout space.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_NOT_SUPPORTED
 */

#define NV2080_CTRL_CMD_FB_GET_CARVEOUT_ADDRESS_INFO (0x2080130bU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FB_INTERFACE_ID << 8) | NV2080_CTRL_FB_GET_SYSTEM_CARVEOUT_ADDRESS_SPACE_INFO_MESSAGE_ID" */

#define NV2080_CTRL_FB_GET_SYSTEM_CARVEOUT_ADDRESS_SPACE_INFO_MESSAGE_ID (0xBU)

typedef struct NV2080_CTRL_FB_GET_SYSTEM_CARVEOUT_ADDRESS_SPACE_INFO {
    NV_DECLARE_ALIGNED(NvU64 StartAddr, 8);
    NV_DECLARE_ALIGNED(NvU64 SpaceSize, 8);
} NV2080_CTRL_FB_GET_SYSTEM_CARVEOUT_ADDRESS_SPACE_INFO;

/*
 * NV2080_CTRL_FB_CMD_GET_CALIBRATION_LOCK_FAILED
 *
 * This command returns the failure counts for calibration.
 *
 *   uFlags
 *     Just one for now -- ehether to reset the counts.
 *   driveStrengthRiseCount
 *     This parameter specifies the failure count for drive strength rising.
 *   driveStrengthFallCount
 *     This parameter specifies the failure count for drive strength falling.
 *   driveStrengthTermCount
 *     This parameter specifies the failure count for drive strength
 *     termination.
 *   slewStrengthRiseCount
 *     This parameter specifies the failure count for slew strength rising.
 *   slewStrengthFallCount
 *     This parameter specifies the failure count for slew strength falling.
 *   slewStrengthTermCount
 *     This parameter specifies the failure count for slew strength
 *     termination.
 *
 *   Possible status values returned are:
 *     NV_OK
 *     NVOS_STATUS_INVALID_PARAM_STRUCT
 *     NVOS_STATUS_NOT_SUPPORTED
 *     NV_ERR_INVALID_ARGUMENT
 */
#define NV2080_CTRL_CMD_FB_GET_CALIBRATION_LOCK_FAILED (0x2080130cU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FB_INTERFACE_ID << 8) | NV2080_CTRL_FB_GET_CALIBRATION_LOCK_FAILED_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_FB_GET_CALIBRATION_LOCK_FAILED_PARAMS_MESSAGE_ID (0xCU)

typedef struct NV2080_CTRL_FB_GET_CALIBRATION_LOCK_FAILED_PARAMS {
    NvU32 flags;
    NvU32 driveStrengthRiseCount;
    NvU32 driveStrengthFallCount;
    NvU32 driveStrengthTermCount;
    NvU32 slewStrengthRiseCount;
    NvU32 slewStrengthFallCount;
} NV2080_CTRL_FB_GET_CALIBRATION_LOCK_FAILED_PARAMS;

/* valid flags parameter values */
#define NV2080_CTRL_CMD_FB_GET_CAL_FLAG_NONE    (0x00000000U)
#define NV2080_CTRL_CMD_FB_GET_CAL_FLAG_RESET   (0x00000001U)

/*
 * NV2080_CTRL_CMD_FB_FLUSH_GPU_CACHE_IRQL
 *
 * If supported by hardware and the OS, this command implements a streamlined version of
 * NV2080_CTRL_CMD_FB_FLUSH_GPU_CACHE which can be called at high IRQL and Bypass the
 * RM Lock.
 *
 * Requires the following NVOS54_PARAMETERS to be set for raised IRQ / Lock Bypass operation:
 *   NVOS54_FLAGS_IRQL_RAISED
 *   NVOS54_FLAGS_LOCK_BYPASS
 *
 *   flags
 *     Contains flags to control various aspects of the flush.  Valid values
 *     are defined in NV2080_CTRL_CTRL_CMD_FB_FLUSH_GPU_CACHE_IRQL_FLAGS*.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_INVALID_STATE
 *   NV_ERR_INVALID_ARGUMENT
 *
 * See Also:
 *   NV2080_CTRL_CMD_FB_FLUSH_GPU_CACHE
 *     This is the more generalized version which is not intended to be called at raised IRQ level
 *   NV0080_CTRL_CMD_DMA_FLUSH
 *     Performs flush operations in broadcast for the GPU cache and other hardware
 *     engines.  Use this call if you want to flush all GPU caches in a
 *     broadcast device.
 *    NV0041_CTRL_CMD_SURFACE_FLUSH_GPU_CACHE
 *     Flushes memory associated with a single allocation if the hardware
 *     supports it.  Use this call if you want to flush a single allocation and
 *     you have a memory object describing the physical memory.
 */
#define NV2080_CTRL_CMD_FB_FLUSH_GPU_CACHE_IRQL (0x2080130dU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FB_INTERFACE_ID << 8) | NV2080_CTRL_CMD_FB_FLUSH_GPU_CACHE_IRQL_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CMD_FB_FLUSH_GPU_CACHE_IRQL_PARAMS_MESSAGE_ID (0xDU)

typedef struct NV2080_CTRL_CMD_FB_FLUSH_GPU_CACHE_IRQL_PARAMS {
    NvU32 flags;
} NV2080_CTRL_CMD_FB_FLUSH_GPU_CACHE_IRQL_PARAMS;

/* valid fields and values for flags */
#define NV2080_CTRL_CMD_FB_FLUSH_GPU_CACHE_IRQL_FLAGS_WRITE_BACK         0:0
#define NV2080_CTRL_CMD_FB_FLUSH_GPU_CACHE_IRQL_FLAGS_WRITE_BACK_NO  (0x00000000U)
#define NV2080_CTRL_CMD_FB_FLUSH_GPU_CACHE_IRQL_FLAGS_WRITE_BACK_YES (0x00000001U)
#define NV2080_CTRL_CMD_FB_FLUSH_GPU_CACHE_IRQL_FLAGS_INVALIDATE         1:1
#define NV2080_CTRL_CMD_FB_FLUSH_GPU_CACHE_IRQL_FLAGS_INVALIDATE_NO  (0x00000000U)
#define NV2080_CTRL_CMD_FB_FLUSH_GPU_CACHE_IRQL_FLAGS_INVALIDATE_YES (0x00000001U)
#define NV2080_CTRL_CMD_FB_FLUSH_GPU_CACHE_IRQL_FLAGS_FB_FLUSH           2:2
#define NV2080_CTRL_CMD_FB_FLUSH_GPU_CACHE_IRQL_FLAGS_FB_FLUSH_NO    (0x00000000U)
#define NV2080_CTRL_CMD_FB_FLUSH_GPU_CACHE_IRQL_FLAGS_FB_FLUSH_YES   (0x00000001U)

/*
 * NV2080_CTRL_CMD_FB_FLUSH_GPU_CACHE
 *
 * This command flushes a cache on the GPU which all memory accesses go
 * through.  The types of flushes supported by this API may not be supported by
 * all hardware.  Attempting an unsupported flush type will result in an error.
 *
 *   addressArray
 *     An array of physical addresses in the aperture defined by
 *     NV2080_CTRL_FB_FLUSH_GPU_CACHE_FLAGS_APERTURE.  Each entry points to a
 *     contiguous block of memory of size memBlockSizeBytes.  The addresses are
 *     aligned down to addressAlign before coalescing adjacent addresses and
 *     sending flushes to hardware.
 *   addressAlign
 *     Used to align-down addresses held in addressArray.  A value of 0 will be
 *     forced to 1 to avoid a divide by zero.  Value is treated as minimum
 *     alignment and any hardware alignment requirements above this value will
 *     be honored.
 *   addressArraySize
 *     The number of entries in addressArray.
 *   memBlockSizeBytes
 *     The size in bytes of each memory block pointed to by addressArray.
 *   flags
 *     Contains flags to control various aspects of the flush.  Valid values
 *     are defined in NV2080_CTRL_FB_FLUSH_GPU_CACHE_FLAGS*.  Not all flags are
 *     valid for all defined FLUSH_MODEs or all GPUs.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_INVALID_STATE
 *   NV_ERR_INVALID_ARGUMENT
 *
 * See Also:
 *   NV0080_CTRL_CMD_DMA_FLUSH
 *     Performs flush operations in broadcast for the GPU cache and other hardware
 *     engines.  Use this call if you want to flush all GPU caches in a
 *     broadcast device.
 *    NV0041_CTRL_CMD_SURFACE_FLUSH_GPU_CACHE
 *     Flushes memory associated with a single allocation if the hardware
 *     supports it.  Use this call if you want to flush a single allocation and
 *     you have a memory object describing the physical memory.
 */
#define NV2080_CTRL_CMD_FB_FLUSH_GPU_CACHE                           (0x2080130eU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FB_INTERFACE_ID << 8) | NV2080_CTRL_FB_FLUSH_GPU_CACHE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_FB_FLUSH_GPU_CACHE_MAX_ADDRESSES                 500U

#define NV2080_CTRL_FB_FLUSH_GPU_CACHE_PARAMS_MESSAGE_ID (0xEU)

typedef struct NV2080_CTRL_FB_FLUSH_GPU_CACHE_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 addressArray[NV2080_CTRL_FB_FLUSH_GPU_CACHE_MAX_ADDRESSES], 8);
    NvU32 addressArraySize;
    NvU32 addressAlign;
    NV_DECLARE_ALIGNED(NvU64 memBlockSizeBytes, 8);
    NvU32 flags;
} NV2080_CTRL_FB_FLUSH_GPU_CACHE_PARAMS;

/* valid fields and values for flags */
#define NV2080_CTRL_FB_FLUSH_GPU_CACHE_FLAGS_APERTURE              1:0
#define NV2080_CTRL_FB_FLUSH_GPU_CACHE_FLAGS_APERTURE_VIDEO_MEMORY    (0x00000000U)
#define NV2080_CTRL_FB_FLUSH_GPU_CACHE_FLAGS_APERTURE_SYSTEM_MEMORY   (0x00000001U)
#define NV2080_CTRL_FB_FLUSH_GPU_CACHE_FLAGS_APERTURE_PEER_MEMORY     (0x00000002U)
#define NV2080_CTRL_FB_FLUSH_GPU_CACHE_FLAGS_WRITE_BACK            2:2
#define NV2080_CTRL_FB_FLUSH_GPU_CACHE_FLAGS_WRITE_BACK_NO            (0x00000000U)
#define NV2080_CTRL_FB_FLUSH_GPU_CACHE_FLAGS_WRITE_BACK_YES           (0x00000001U)
#define NV2080_CTRL_FB_FLUSH_GPU_CACHE_FLAGS_INVALIDATE            3:3
#define NV2080_CTRL_FB_FLUSH_GPU_CACHE_FLAGS_INVALIDATE_NO            (0x00000000U)
#define NV2080_CTRL_FB_FLUSH_GPU_CACHE_FLAGS_INVALIDATE_YES           (0x00000001U)
#define NV2080_CTRL_FB_FLUSH_GPU_CACHE_FLAGS_FLUSH_MODE            4:4
#define NV2080_CTRL_FB_FLUSH_GPU_CACHE_FLAGS_FLUSH_MODE_ADDRESS_ARRAY (0x00000000U)
#define NV2080_CTRL_FB_FLUSH_GPU_CACHE_FLAGS_FLUSH_MODE_FULL_CACHE    (0x00000001U)
#define NV2080_CTRL_FB_FLUSH_GPU_CACHE_FLAGS_FB_FLUSH              5:5
#define NV2080_CTRL_FB_FLUSH_GPU_CACHE_FLAGS_FB_FLUSH_NO              (0x00000000U)
#define NV2080_CTRL_FB_FLUSH_GPU_CACHE_FLAGS_FB_FLUSH_YES             (0x00000001U)

/*
 * NV2080_CTRL_FB_GPU_CACHE_ALLOC_POLICY (deprecated; use NV2080_CTRL_FB_GPU_CACHE_ALLOC_POLICY_V2 instead)
 *
 * These commands access the cache allocation policy on a specific
 * engine, if supported.
 *
 *   engine
 *     Specifies the target engine.  Possible values are defined in
 *     NV2080_ENGINE_TYPE.
 *   allocPolicy
 *     Specifies the read/write allocation policy of the cache on the specified
 *     engine. Possible values are defined in
 *     NV2080_CTRL_FB_GPU_CACHE_ALLOC_POLICY_READS and
 *     NV2080_CTRL_FB_GPU_CACHE_ALLOC_POLICY_WRITES.
 *
 */
typedef struct NV2080_CTRL_FB_GPU_CACHE_ALLOC_POLICY_PARAMS {
    NvU32 engine;
    NvU32 allocPolicy;
} NV2080_CTRL_FB_GPU_CACHE_ALLOC_POLICY_PARAMS;

/* valid values for allocPolicy */
#define NV2080_CTRL_FB_GPU_CACHE_ALLOC_POLICY_READS                0:0
#define NV2080_CTRL_FB_GPU_CACHE_ALLOC_POLICY_READS_NO   (0x00000000U)
#define NV2080_CTRL_FB_GPU_CACHE_ALLOC_POLICY_READS_YES  (0x00000001U)
#define NV2080_CTRL_FB_GPU_CACHE_ALLOC_POLICY_WRITES               1:1
#define NV2080_CTRL_FB_GPU_CACHE_ALLOC_POLICY_WRITES_NO  (0x00000000U)
#define NV2080_CTRL_FB_GPU_CACHE_ALLOC_POLICY_WRITES_YES (0x00000001U)


/*
 * NV2080_CTRL_CMD_FB_SET_GPU_CACHE_ALLOC_POLICY
 *
 * This command is deprecated.
 * Use NV2080_CTRL_CMD_FB_SET_GPU_CACHE_ALLOC_POLICY_V2 instead.
 *
 * This command sets the state of the cache allocation policy on a specific
 * engine, if supported.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV2080_CTRL_CMD_FB_SET_GPU_CACHE_ALLOC_POLICY    (0x2080130fU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FB_INTERFACE_ID << 8) | NV2080_CTRL_FB_SET_GPU_CACHE_ALLOC_POLICY_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_FB_SET_GPU_CACHE_ALLOC_POLICY_PARAMS_MESSAGE_ID (0xFU)

typedef NV2080_CTRL_FB_GPU_CACHE_ALLOC_POLICY_PARAMS NV2080_CTRL_FB_SET_GPU_CACHE_ALLOC_POLICY_PARAMS;

/*
 * NV2080_CTRL_FB_GPU_CACHE_ALLOC_POLICY_V2_PARAM
 *
 * These commands access the cache allocation policy on a specific
 * client, if supported.
 *
 *   count
 *     Specifies the number of entries in entry.
 *   entry
 *     Specifies an array of allocation policy entries.
 *
 * NV2080_CTRL_FB_GPU_CACHE_ALLOC_POLICY_V2_ENTRY
 *
 *   clients
 *     Specifies the target client.  Possible values are defined in
 *     NV2080_CLIENT_TYPE_*.
 *   allocPolicy
 *     Specifies the read/write allocation policy of the cache on the specified
 *     engine. Possible values are defined in
 *     NV2080_CTRL_FB_GPU_CACHE_ALLOC_POLICY_V2_READS and
 *     NV2080_CTRL_FB_GPU_CACHE_ALLOC_POLICY_V2_WRITES.
 *
 * NV2080_CTRL_FB_GPU_CACHE_ALLOC_POLICY_V2_ENTRY_SIZE
 *
 *     Specifies the maximum number of allocation policy entries allowed
 */
#define NV2080_CTRL_FB_GPU_CACHE_ALLOC_POLICY_V2_ENTRY_SIZE 11U

typedef struct NV2080_CTRL_FB_GPU_CACHE_ALLOC_POLICY_V2_ENTRY {
    NvU32 client;
    NvU32 allocPolicy;
} NV2080_CTRL_FB_GPU_CACHE_ALLOC_POLICY_V2_ENTRY;

typedef struct NV2080_CTRL_FB_GPU_CACHE_ALLOC_POLICY_V2_PARAMS {
    NvU32                                          count;
    NV2080_CTRL_FB_GPU_CACHE_ALLOC_POLICY_V2_ENTRY entry[NV2080_CTRL_FB_GPU_CACHE_ALLOC_POLICY_V2_ENTRY_SIZE];
} NV2080_CTRL_FB_GPU_CACHE_ALLOC_POLICY_V2_PARAMS;

/* valid values for allocPolicy */
#define NV2080_CTRL_FB_GPU_CACHE_ALLOC_POLICY_V2_READS             0:0
#define NV2080_CTRL_FB_GPU_CACHE_ALLOC_POLICY_V2_READS_DISABLE    (0x00000000U)
#define NV2080_CTRL_FB_GPU_CACHE_ALLOC_POLICY_V2_READS_ENABLE     (0x00000001U)
#define NV2080_CTRL_FB_GPU_CACHE_ALLOC_POLICY_V2_READS_ALLOW       1:1
#define NV2080_CTRL_FB_GPU_CACHE_ALLOC_POLICY_V2_READS_ALLOW_NO   (0x00000000U)
#define NV2080_CTRL_FB_GPU_CACHE_ALLOC_POLICY_V2_READS_ALLOW_YES  (0x00000001U)
#define NV2080_CTRL_FB_GPU_CACHE_ALLOC_POLICY_V2_WRITES            2:2
#define NV2080_CTRL_FB_GPU_CACHE_ALLOC_POLICY_V2_WRITES_DISABLE   (0x00000000U)
#define NV2080_CTRL_FB_GPU_CACHE_ALLOC_POLICY_V2_WRITES_ENABLE    (0x00000001U)
#define NV2080_CTRL_FB_GPU_CACHE_ALLOC_POLICY_V2_WRITES_ALLOW      3:3
#define NV2080_CTRL_FB_GPU_CACHE_ALLOC_POLICY_V2_WRITES_ALLOW_NO  (0x00000000U)
#define NV2080_CTRL_FB_GPU_CACHE_ALLOC_POLICY_V2_WRITES_ALLOW_YES (0x00000001U)


/*
 * NV2080_CTRL_CMD_FB_SET_GPU_CACHE_ALLOC_POLICY_V2
 *
 * This command sets the state of the cache allocation policy on a specific
 * engine, if supported.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV2080_CTRL_CMD_FB_SET_GPU_CACHE_ALLOC_POLICY_V2          (0x20801318U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FB_INTERFACE_ID << 8) | NV2080_CTRL_FB_SET_GPU_CACHE_ALLOC_POLICY_V2_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_FB_SET_GPU_CACHE_ALLOC_POLICY_V2_PARAMS_MESSAGE_ID (0x18U)

typedef NV2080_CTRL_FB_GPU_CACHE_ALLOC_POLICY_V2_PARAMS NV2080_CTRL_FB_SET_GPU_CACHE_ALLOC_POLICY_V2_PARAMS;

/*
 * NV2080_CTRL_CMD_FB_GET_GPU_CACHE_ALLOC_POLICY (deprecated; use NV2080_CTRL_CMD_FB_GET_GPU_CACHE_ALLOC_POLICY_V2 instead)
 *
 * This command gets the state of the cache allocation policy on a specific
 * engine, if supported.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV2080_CTRL_CMD_FB_GET_GPU_CACHE_ALLOC_POLICY (0x20801312U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FB_INTERFACE_ID << 8) | NV2080_CTRL_FB_GET_GPU_CACHE_ALLOC_POLICY_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_FB_GET_GPU_CACHE_ALLOC_POLICY_PARAMS_MESSAGE_ID (0x12U)

typedef NV2080_CTRL_FB_GPU_CACHE_ALLOC_POLICY_PARAMS NV2080_CTRL_FB_GET_GPU_CACHE_ALLOC_POLICY_PARAMS;

/*
 * NV2080_CTRL_CMD_FB_GET_GPU_CACHE_ALLOC_POLICY_V2
 *
 * This command gets the state of the cache allocation policy on a specific
 * engine, if supported.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV2080_CTRL_CMD_FB_GET_GPU_CACHE_ALLOC_POLICY_V2 (0x20801319U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FB_INTERFACE_ID << 8) | NV2080_CTRL_FB_GET_GPU_CACHE_ALLOC_POLICY_V2_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_FB_GET_GPU_CACHE_ALLOC_POLICY_V2_PARAMS_MESSAGE_ID (0x19U)

typedef NV2080_CTRL_FB_GPU_CACHE_ALLOC_POLICY_V2_PARAMS NV2080_CTRL_FB_GET_GPU_CACHE_ALLOC_POLICY_V2_PARAMS;


/*
 * NV2080_CTRL_CMD_FB_IS_KIND
 *
 * This command is used to perform various operations like 'IS_KIND_VALID',
 * 'IS_KIND_COMPRESSIBLE'on the kind passed by the caller. The operation to be
 * performed should be passed in the 'operation' parameter of
 * NV2080_CTRL_FB_IS_KIND_PARAMS, the kind on which the operation is to be
 * performed should be passed in the 'kind' parameter. The result of the
 * operation (true/false) will be returned in the 'result' parameter.
 *
 *   operation
 *     Specifies what operation is to be performed on the kind passed by the
 *     caller. The supported operations are
 *       NV2080_CTRL_FB_IS_KIND_OPERATION_SUPPORTED
 *         This operation checks whether the kind passed in the 'kind'
 *         parameter of the 'NV2080_CTRL_FB_IS_KIND_PARAMS' structure is
 *         supported for this GPU. Returns nonzero value in 'result' parameter
 *         if the input kind is supported, else returns zero in the result.
 *       NV2080_CTRL_FB_IS_KIND_OPERATION_COMPRESSIBLE
 *         This operation checks whether the kind passed in the 'kind'
 *         parameter of the 'NV2080_CTRL_FB_IS_KIND_PARAMS' structure is
 *         compressible. Returns nonzero value in 'result' parameter if the
 *         input kind is compressible, else returns zero in the result.
 *       NV2080_CTRL_FB_IS_KIND_OPERATION_COMPRESSIBLE_1
 *         This operation checks whether the kind passed in the 'kind'
 *         parameter of the 'NV2080_CTRL_FB_IS_KIND_PARAMS' structure supports
 *         1 bit compression. Returns nonzero value in 'result' parameter if
 *         kind supports 1 bit compression, else returns zero in the result.
 *       NV2080_CTRL_FB_IS_KIND_OPERATION_COMPRESSIBLE_2
 *         This operation checks whether the kind passed in the 'kind'
 *         parameter of the 'NV2080_CTRL_FB_IS_KIND_PARAMS' structure supports
 *         2 bit compression. Returns nonzero value in 'result' parameter if
 *         kind supports 1 bit compression, else returns zero in the result.
 *       NV2080_CTRL_FB_IS_KIND_OPERATION_COMPRESSIBLE_4
 *         This operation checks whether the kind passed in the 'kind'
 *         parameter of the 'NV2080_CTRL_FB_IS_KIND_PARAMS' structure supports
 *         4 bit compression. Returns nonzero value in 'result' parameter if
 *         kind supports 4 bit compression, else returns zero in the result.
 *       NV2080_CTRL_FB_IS_KIND_OPERATION_ZBC
 *         This operation checks whether the kind passed in the 'kind'
 *         parameter of the 'NV2080_CTRL_FB_IS_KIND_PARAMS' structure
 *         supports ZBC. Returns nonzero value in 'result' parameter if the
 *         input kind supports ZBC, else returns zero in the result.
 *       NV2080_CTRL_FB_IS_KIND_OPERATION_ZBC_ALLOWS_1
 *         This operation checks whether the kind passed in the 'kind'
 *         parameter of the 'NV2080_CTRL_FB_IS_KIND_PARAMS' structure
 *         supports 1 bit ZBC. Returns nonzero value in 'result' parameter if
 *         the input kind supports 1 bit ZBC, else returns zero in the result.
 *       NV2080_CTRL_FB_IS_KIND_OPERATION_ZBC_ALLOWS_2
 *         This operation checks whether the kind passed in the 'kind'
 *         parameter of the 'NV2080_CTRL_FB_IS_KIND_PARAMS' structure
 *         supports 2 bit ZBC. Returns nonzero value in 'result' parameter if
 *         the input kind supports 2 bit ZBC, else returns zero in the result.
 *       NV2080_CTRL_FB_IS_KIND_OPERATION_ZBC_ALLOWS_4
 *         This operation checks whether the kind passed in the 'kind'
 *         parameter of the 'NV2080_CTRL_FB_IS_KIND_PARAMS' structure
 *         supports 4 bit ZBC. Returns nonzero value in 'result' parameter if
 *         the input kind supports 4 bit ZBC, else returns zero in the result.
 *   kind
 *     Specifies the kind on which the operation is to be carried out. The
 *     legal range of values for the kind parameter is different on different
 *     GPUs. For e.g. on Fermi, valid range is 0x00 to 0xfe. Still, some values
 *     inside this legal range can be invalid i.e. not defined.
 *     So its always better to first check if a particular kind is supported on
 *     the current GPU with 'NV2080_CTRL_FB_IS_KIND_SUPPORTED' operation.
 *   result
 *     Upon return, this parameter will hold the result (true/false) of the
 *     operation performed on the kind.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV2080_CTRL_CMD_FB_IS_KIND (0x20801313U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FB_INTERFACE_ID << 8) | NV2080_CTRL_FB_IS_KIND_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_FB_IS_KIND_PARAMS_MESSAGE_ID (0x13U)

typedef struct NV2080_CTRL_FB_IS_KIND_PARAMS {
    NvU32  operation;
    NvU32  kind;
    NvBool result;
} NV2080_CTRL_FB_IS_KIND_PARAMS;

/* valid values for operation */
#define NV2080_CTRL_FB_IS_KIND_OPERATION_SUPPORTED      (0x00000000U)
#define NV2080_CTRL_FB_IS_KIND_OPERATION_COMPRESSIBLE   (0x00000001U)
#define NV2080_CTRL_FB_IS_KIND_OPERATION_COMPRESSIBLE_1 (0x00000002U)
#define NV2080_CTRL_FB_IS_KIND_OPERATION_COMPRESSIBLE_2 (0x00000003U)
#define NV2080_CTRL_FB_IS_KIND_OPERATION_COMPRESSIBLE_4 (0x00000004U)
#define NV2080_CTRL_FB_IS_KIND_OPERATION_ZBC            (0x00000005U)
#define NV2080_CTRL_FB_IS_KIND_OPERATION_ZBC_ALLOWS_1   (0x00000006U)
#define NV2080_CTRL_FB_IS_KIND_OPERATION_ZBC_ALLOWS_2   (0x00000007U)
#define NV2080_CTRL_FB_IS_KIND_OPERATION_ZBC_ALLOWS_4   (0x00000008U)

/**
 * NV2080_CTRL_CMD_FB_GET_GPU_CACHE_INFO
 *
 * This command returns the state of a cache which all GPU memory accesess go
 * through.
 *
 *   powerState
 *     Returns the power state of the cache.  Possible values are defined in
 *     NV2080_CTRL_FB_GET_GPU_CACHE_INFO_POWER_STATE.
 *
 *   writeMode
 *     Returns the write mode of the cache.  Possible values are defined in
 *     NV2080_CTRL_FB_GET_GPU_CACHE_INFO_WRITE_MODE.
 *
 *   bypassMode
 *     Returns the bypass mode of the L2 cache.  Possible values are defined in
 *     NV2080_CTRL_FB_GET_GPU_CACHE_INFO_BYPASS_MODE.
 *
 *   rcmState
 *     Returns the RCM state of the cache.  Possible values are defined in
 *     NV2080_CTRL_FB_GET_GPU_CACHE_INFO_RCM_STATE.
 *
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_INVALID_STATE
 *   NV_ERR_INVALID_ARGUMENT
 */

#define NV2080_CTRL_CMD_FB_GET_GPU_CACHE_INFO           (0x20801315U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FB_INTERFACE_ID << 8) | NV2080_CTRL_FB_GET_GPU_CACHE_INFO_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_FB_GET_GPU_CACHE_INFO_PARAMS_MESSAGE_ID (0x15U)

typedef struct NV2080_CTRL_FB_GET_GPU_CACHE_INFO_PARAMS {
    NvU32 powerState;
    NvU32 writeMode;
    NvU32 bypassMode;
    NvU32 rcmState;
} NV2080_CTRL_FB_GET_GPU_CACHE_INFO_PARAMS;

/* valid values for powerState */
#define NV2080_CTRL_FB_GET_GPU_CACHE_INFO_POWER_STATE_ENABLED     (0x00000000U)
#define NV2080_CTRL_FB_GET_GPU_CACHE_INFO_POWER_STATE_DISABLED    (0x00000001U)
/* valid values for writeMode */
#define NV2080_CTRL_FB_GET_GPU_CACHE_INFO_WRITE_MODE_WRITETHROUGH (0x00000000U)
#define NV2080_CTRL_FB_GET_GPU_CACHE_INFO_WRITE_MODE_WRITEBACK    (0x00000001U)
/* valid values for bypassMode */
#define NV2080_CTRL_FB_GET_GPU_CACHE_INFO_BYPASS_MODE_DISABLED    (0x00000000U)
#define NV2080_CTRL_FB_GET_GPU_CACHE_INFO_BYPASS_MODE_ENABLED     (0x00000001U)
/* valid values for rcmState */
#define NV2080_CTRL_FB_GET_GPU_CACHE_INFO_RCM_STATE_FULL          (0x00000000U)
#define NV2080_CTRL_FB_GET_GPU_CACHE_INFO_RCM_STATE_TRANSITIONING (0x00000001U)
#define NV2080_CTRL_FB_GET_GPU_CACHE_INFO_RCM_STATE_REDUCED       (0x00000002U)
#define NV2080_CTRL_FB_GET_GPU_CACHE_INFO_RCM_STATE_ZERO_CACHE    (0x00000003U)

/*
 * NV2080_CTRL_FB_GPU_CACHE_PROMOTION_POLICY
 *
 * These commands access the cache promotion policy on a specific
 * engine, if supported by the hardware.
 *
 * Cache promotion refers to the GPU promoting a memory read to a larger
 * size to preemptively fill the cache so future reads to nearby memory
 * addresses will hit in the cache.
 *
 *   engine
 *     Specifies the target engine.  Possible values are defined in
 *     NV2080_ENGINE_TYPE.
 *   promotionPolicy
 *     Specifies the promotion policy of the cache on the specified
 *     engine. Possible values are defined by
 *     NV2080_CTRL_FB_GPU_CACHE_PROMOTION_POLICY_*.  These values are in terms
 *     of the hardware cache line size.
 *
 */
typedef struct NV2080_CTRL_FB_GPU_CACHE_PROMOTION_POLICY_PARAMS {
    NvU32 engine;
    NvU32 promotionPolicy;
} NV2080_CTRL_FB_GPU_CACHE_PROMOTION_POLICY_PARAMS;

/* valid values for promotionPolicy */
#define NV2080_CTRL_FB_GPU_CACHE_PROMOTION_POLICY_NONE    (0x00000000U)
#define NV2080_CTRL_FB_GPU_CACHE_PROMOTION_POLICY_QUARTER (0x00000001U)
#define NV2080_CTRL_FB_GPU_CACHE_PROMOTION_POLICY_HALF    (0x00000002U)
#define NV2080_CTRL_FB_GPU_CACHE_PROMOTION_POLICY_FULL    (0x00000003U)


/*
 * NV2080_CTRL_CMD_FB_SET_GPU_CACHE_PROMOTION_POLICY
 *
 * This command sets the cache promotion policy on a specific engine, if
 * supported by the hardware.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV2080_CTRL_CMD_FB_SET_GPU_CACHE_PROMOTION_POLICY (0x20801316U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FB_INTERFACE_ID << 8) | 0x16" */ // Deprecated, removed form RM


/*
 * NV2080_CTRL_CMD_FB_GET_GPU_CACHE_PROMOTION_POLICY
 *
 * This command gets the cache promotion policy on a specific engine, if
 * supported by the hardware.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV2080_CTRL_CMD_FB_GET_GPU_CACHE_PROMOTION_POLICY (0x20801317U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FB_INTERFACE_ID << 8) | 0x17" */ // Deprecated, removed form RM

/*
 * NV2080_CTRL_FB_CMD_GET_FB_REGION_INFO
 *
 * This command returns the FB memory region characteristics.
 *
 *   numFBRegions
 *     Number of valid regions returned in fbRegion[]
 *   fbRegion[].base
 *     Base address of region.  The first valid address in the range
 *     [base..limit].
 *   fbRegion[].limit
 *     Last/end address of region.  The last valid address in the range
 *     [base..limit].
 *     (limit - base + 1) = size of the region
 *   fbRegion[].reserved
 *     Amount of memory that RM speculatively needs within the region.  A
 *     client doing its own memory management should leave at least this much
 *     memory available for RM use.  This particularly applies to a driver
 *     model like LDDM.
 *   fbRegion[].performance
 *     Relative performance of this region compared to other regions.
 *     The definition is vague, and only connotes relative bandwidth or
 *     performance.  The higher the value, the higher the performance.
 *   fbRegion[].supportCompressed
 *     TRUE if compressed surfaces/kinds are supported
 *     FALSE if compressed surfaces/kinds are not allowed to be allocated in
 *     this region
 *   fbRegion[].supportISO
 *     TRUE if ISO surfaces/kinds are supported (Display, cursor, video)
 *     FALSE if ISO surfaces/kinds are not allowed to be allocated in this
 *     region
 *   fbRegion[].bProtected
 *     TRUE if this region is a protected memory region.  If true only
 *     allocations marked as protected (NVOS32_ALLOC_FLAGS_PROTECTED) can be
 *     allocated in this region.
 *   fbRegion[].blackList[] - DEPRECATED: Use supportISO
 *     TRUE for each NVOS32_TYPE_IMAGE* that is NOT allowed in this region.
 *
 *   Possible status values returned are:
 *     NV_OK
 *     NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_FB_GET_FB_REGION_INFO             (0x20801320U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FB_INTERFACE_ID << 8) | NV2080_CTRL_CMD_FB_GET_FB_REGION_INFO_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CMD_FB_GET_FB_REGION_INFO_MEM_TYPES   17U

typedef NvBool NV2080_CTRL_CMD_FB_GET_FB_REGION_SURFACE_MEM_TYPE_FLAG[NV2080_CTRL_CMD_FB_GET_FB_REGION_INFO_MEM_TYPES];

typedef struct NV2080_CTRL_CMD_FB_GET_FB_REGION_FB_REGION_INFO {
    NV_DECLARE_ALIGNED(NvU64 base, 8);
    NV_DECLARE_ALIGNED(NvU64 limit, 8);
    NV_DECLARE_ALIGNED(NvU64 reserved, 8);
    NvU32                                                  performance;
    NvBool                                                 supportCompressed;
    NvBool                                                 supportISO;
    NvBool                                                 bProtected;
    NV2080_CTRL_CMD_FB_GET_FB_REGION_SURFACE_MEM_TYPE_FLAG blackList;
} NV2080_CTRL_CMD_FB_GET_FB_REGION_FB_REGION_INFO;

#define NV2080_CTRL_CMD_FB_GET_FB_REGION_INFO_MAX_ENTRIES 16U

#define NV2080_CTRL_CMD_FB_GET_FB_REGION_INFO_PARAMS_MESSAGE_ID (0x20U)

typedef struct NV2080_CTRL_CMD_FB_GET_FB_REGION_INFO_PARAMS {
    NvU32 numFBRegions;
    NV_DECLARE_ALIGNED(NV2080_CTRL_CMD_FB_GET_FB_REGION_FB_REGION_INFO fbRegion[NV2080_CTRL_CMD_FB_GET_FB_REGION_INFO_MAX_ENTRIES], 8);
} NV2080_CTRL_CMD_FB_GET_FB_REGION_INFO_PARAMS;

/*
 * NV2080_CTRL_CMD_FB_OFFLINE_PAGES
 *
 * This command adds video memory page addresses to the list of offlined
 * addresses so that they're not allocated to any client. The newly offlined
 * addresses take effect after a reboot.
 *
 *   offlined
 *     This input parameter is an array of NV2080_CTRL_FB_OFFLINED_ADDRESS_INFO
 *     structures, containing the video memory physical page numbers that
 *     are to be blacklisted. This array can hold a maximum of NV2080_CTRL_FB_
 *     BLACKLIST_PAGES_MAX_PAGES address pairs. Valid entries are adjacent.
 *   pageSize
 *     This input parameter contains the size of the page that is to be
 *     blacklisted.
 *   validEntries
 *     This input parameter specifies the number of valid entries in the
 *     offlined array.
 *   numPagesAdded
 *     This output parameter specifies how many of the validEntries were
 *     actually offlined. If numPagesAdded < validEntries, it
 *     means that only addresses from offlined[0] to offlined[numPagesAdded - 1]
 *     were added to the list of offlined addresses.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV2080_CTRL_CMD_FB_OFFLINE_PAGES              (0x20801321U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FB_INTERFACE_ID << 8) | NV2080_CTRL_FB_OFFLINE_PAGES_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_FB_OFFLINED_PAGES_MAX_PAGES       (0x00000040U)
#define NV2080_CTRL_FB_OFFLINED_PAGES_INVALID_ADDRESS (0xffffffffffffffffULL)
#define NV2080_CTRL_FB_OFFLINED_PAGES_PAGE_SIZE_4K    (0x00000000U)
#define NV2080_CTRL_FB_OFFLINED_PAGES_PAGE_SIZE_64K   (0x00000001U)
#define NV2080_CTRL_FB_OFFLINED_PAGES_PAGE_SIZE_128K  (0x00000002U)

/*
 * NV2080_CTRL_FB_OFFLINED_ADDRESS_INFO
 *
 *   pageAddressWithEccOn
 *     Address of the memory page retired when ECC is enabled on the board.
 *   pageAddressWithEccOff
 *     Address of the memory page retired when ECC is disabled on the board.
 *   rbcAddress
 *     Row/Bank/Column Address of the faulty memory which caused the page to
 *     be retired
 *   source
 *     The reason for the page to be retired
 *   status
 *      Non-exceptional reasons for a page retirement failure
 *         NV2080_CTRL_FB_OFFLINED_PAGES_STATUS_OK
 *            No error
 *         NV2080_CTRL_FB_OFFLINED_PAGES_STATUS_PENDING_RETIREMENT
 *            The given address is already pending retirement or has
 *            been retired during the current driver run. The page
 *            will be offlined during the next driver run.
 *         NV2080_CTRL_FB_OFFLINED_PAGES_STATUS_BLACKLISTING_FAILED
 *            The given page was retired on a previous driver run,
 *            so it should not be accessible unless offlining failed.
 *            Failing to offline a page is strongly indicative of a
 *            driver offlining bug.
 *         NV2080_CTRL_FB_OFFLINED_PAGES_STATUS_TABLE_FULL
 *            The PBL is full and no more pages can be retired
 *         NV2080_CTRL_FB_OFFLINED_PAGES_STATUS_INTERNAL_ERROR
 *            Internal driver error
 *
 */



typedef struct NV2080_CTRL_FB_OFFLINED_ADDRESS_INFO {
    NV_DECLARE_ALIGNED(NvU64 pageAddressWithEccOn, 8);
    NV_DECLARE_ALIGNED(NvU64 pageAddressWithEccOff, 8);
    NvU32 rbcAddress;
    NvU32 source;
    NvU32 status;
    NvU32 timestamp;
} NV2080_CTRL_FB_OFFLINED_ADDRESS_INFO;

/* valid values for source */
#define NV2080_CTRL_FB_OFFLINED_PAGES_SOURCE_DPR_MULTIPLE_SBE    (0x00000002U)
#define NV2080_CTRL_FB_OFFLINED_PAGES_SOURCE_DPR_DBE             (0x00000004U)



/* valid values for status */
#define NV2080_CTRL_FB_OFFLINED_PAGES_STATUS_OK                  (0x00000000U)
#define NV2080_CTRL_FB_OFFLINED_PAGES_STATUS_PENDING_RETIREMENT  (0x00000001U)
#define NV2080_CTRL_FB_OFFLINED_PAGES_STATUS_BLACKLISTING_FAILED (0x00000002U)
#define NV2080_CTRL_FB_OFFLINED_PAGES_STATUS_TABLE_FULL          (0x00000003U)
#define NV2080_CTRL_FB_OFFLINED_PAGES_STATUS_INTERNAL_ERROR      (0x00000004U)

/* deprecated */
#define NV2080_CTRL_FB_OFFLINED_PAGES_SOURCE_MULTIPLE_SBE        NV2080_CTRL_FB_OFFLINED_PAGES_SOURCE_DPR_MULTIPLE_SBE
#define NV2080_CTRL_FB_OFFLINED_PAGES_SOURCE_DBE                 NV2080_CTRL_FB_OFFLINED_PAGES_SOURCE_DPR_DBE


#define NV2080_CTRL_FB_OFFLINE_PAGES_PARAMS_MESSAGE_ID (0x21U)

typedef struct NV2080_CTRL_FB_OFFLINE_PAGES_PARAMS {
    NV_DECLARE_ALIGNED(NV2080_CTRL_FB_OFFLINED_ADDRESS_INFO offlined[NV2080_CTRL_FB_OFFLINED_PAGES_MAX_PAGES], 8);
    NvU32 pageSize;
    NvU32 validEntries;
    NvU32 numPagesAdded;
} NV2080_CTRL_FB_OFFLINE_PAGES_PARAMS;

/*
 * NV2080_CTRL_CMD_FB_GET_OFFLINED_PAGES
 *
 * This command returns the list of video memory page addresses in the
 * Inforom's blacklist.
 *
 *   offlined
 *     This output parameter is an array of NV2080_CTRL_FB_BLACKLIST_ADDRESS_
 *     INFO structures, containing the video memory physical page numbers that
 *     are blacklisted. This array can hold a maximum of NV2080_CTRL_FB_
 *     BLACKLIST_PAGES_MAX_PAGES address pairs. Valid entries are adjacent.
 *     The array also contains the Row/Bank/Column Address and source.
 *   validEntries
 *     This output parameter specifies the number of valid entries in the
 *     offlined array.
 *   bRetirementPending (DEPRECATED, use retirementPending instead)
 *     This output parameter returns if any pages on the list are pending
 *     retirement.
 *   retirementPending
 *     Communicates to the caller whether retirement updates are pending and the
 *     reason for the updates. Possible fields are:
 *     NV2080_CTRL_FB_GET_OFFLINED_PAGES_RETIREMENT_PENDING_*:
 *       NV2080_CTRL_FB_GET_OFFLINED_PAGES_RETIREMENT_PENDING_SBE:
 *         Indicates whether pages are pending retirement due to SBE.
 *       NV2080_CTRL_FB_GET_OFFLINED_PAGES_RETIREMENT_PENDING_DBE:
 *         Indicates whether pages are pending retirement due to DBE. Driver
 *         reload needed to retire bad memory pages and allow compute app runs.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */



#define NV2080_CTRL_CMD_FB_GET_OFFLINED_PAGES                            (0x20801322U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FB_INTERFACE_ID << 8) | NV2080_CTRL_FB_GET_OFFLINED_PAGES_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_FB_GET_OFFLINED_PAGES_RETIREMENT_PENDING_SBE          0:0
#define NV2080_CTRL_FB_GET_OFFLINED_PAGES_RETIREMENT_PENDING_SBE_FALSE   0U
#define NV2080_CTRL_FB_GET_OFFLINED_PAGES_RETIREMENT_PENDING_SBE_TRUE    1U
#define NV2080_CTRL_FB_GET_OFFLINED_PAGES_RETIREMENT_PENDING_DBE          1:1
#define NV2080_CTRL_FB_GET_OFFLINED_PAGES_RETIREMENT_PENDING_DBE_FALSE   0U
#define NV2080_CTRL_FB_GET_OFFLINED_PAGES_RETIREMENT_PENDING_DBE_TRUE    1U



#define NV2080_CTRL_FB_GET_OFFLINED_PAGES_PARAMS_MESSAGE_ID (0x22U)

typedef struct NV2080_CTRL_FB_GET_OFFLINED_PAGES_PARAMS {
    NV_DECLARE_ALIGNED(NV2080_CTRL_FB_OFFLINED_ADDRESS_INFO offlined[NV2080_CTRL_FB_OFFLINED_PAGES_MAX_PAGES], 8);
    NvU32  validEntries;
    NvBool bRetirementPending;
    NvU8   retirementPending;
} NV2080_CTRL_FB_GET_OFFLINED_PAGES_PARAMS;

/*
 * NV2080_CTRL_CMD_FB_QUERY_ACR_REGION
 *
 * This control command is used to query the secured region allocated
 *
 * queryType
 *          NV2080_CTRL_CMD_FB_ACR_QUERY_GET_REGION_STATUS: Provides the alloc
 *          status and ACR region ID.
 *          NV2080_CTRL_CMD_FB_QUERY_MAP_REGION : Maps the region on BAR1
 *           it returns the "pCpuAddr" and pPriv to user.
 *          NV2080_CTRL_CMD_FB_QUERY_UNMAP_REGION: Unmaps the mapped region.
 *          it takes the pPriv as input
 *
 * clientReq : struct ACR_REQUEST_PARAMS
 *          It is used to find the allocated ACR region for that client
 *          clientId     : ACR Client ID
 *          reqReadMask  : read mask of ACR region
 *          reqWriteMask : Write mask of ACR region
 *          regionSize   : ACR region Size
 *
 * clientReqStatus : struct ACR_STATUS_PARAMS
 *          This struct is stores the output of requested ACR region.
 *          allocStatus     : Allocated Status of ACR region
 *          regionId        : ACR region ID
 *          physicalAddress : Physical address on FB
 *
 *
 * NV2080_CTRL_CMD_FB_ACR_QUERY_ERROR_CODE
 *          NV2080_CTRL_CMD_FB_ACR_QUERY_ERROR_NONE : Control command executed successfully
 *          NV2080_CTRL_CMD_FB_ACR_QUERY_ERROR_INVALID_CLIENT_REQUEST : Please check the parameter
 *                      for ACR client request
 *          NV2080_CTRL_CMD_FB_ACR_QUERY_ERROR_FAILED_TO_MAP_ON_BAR1 : RM Fails to map ACR region
 *                      on BAR1
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
*/
#define NV2080_CTRL_CMD_FB_QUERY_ACR_REGION (0x20801325U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FB_INTERFACE_ID << 8) | NV2080_CTRL_CMD_FB_QUERY_ACR_REGION_PARAMS_MESSAGE_ID" */

//
// We can create an ACR region by using RMCreateAcrRegion[1|2] regkey or mods -acr[1|2]_size
// Client ID for such region is 2 in RM.
//
#define NV2080_CTRL_CMD_FB_ACR_CLIENT_ID    2U

typedef enum NV2080_CTRL_CMD_FB_ACR_QUERY_TYPE {
    NV2080_CTRL_CMD_FB_ACR_QUERY_GET_CLIENT_REGION_STATUS = 0,
    NV2080_CTRL_CMD_FB_ACR_QUERY_GET_REGION_PROPERTY = 1,
    NV2080_CTRL_CMD_FB_ACR_QUERY_GET_FALCON_STATUS = 2,
} NV2080_CTRL_CMD_FB_ACR_QUERY_TYPE;

typedef enum NV2080_CTRL_CMD_FB_ACR_QUERY_ERROR_CODE {
    NV2080_CTRL_CMD_FB_ACR_QUERY_ERROR_NONE = 0,
    NV2080_CTRL_CMD_FB_ACR_QUERY_ERROR_INVALID_CLIENT_REQUEST = 1,
} NV2080_CTRL_CMD_FB_ACR_QUERY_ERROR_CODE;

typedef struct ACR_REQUEST_PARAMS {
    NvU32 clientId;
    NvU32 reqReadMask;
    NvU32 reqWriteMask;
    NvU32 regionSize;
} ACR_REQUEST_PARAMS;

typedef struct ACR_REGION_ID_PROP {
    NvU32 regionId;
    NvU32 readMask;
    NvU32 writeMask;
    NvU32 regionSize;
    NvU32 clientMask;
    NV_DECLARE_ALIGNED(NvU64 physicalAddress, 8);
} ACR_REGION_ID_PROP;

typedef struct ACR_STATUS_PARAMS {
    NvU32 allocStatus;
    NvU32 regionId;
    NV_DECLARE_ALIGNED(NvU64 physicalAddress, 8);
} ACR_STATUS_PARAMS;

typedef struct ACR_REGION_HANDLE {
    NvHandle hClient;
    NvHandle hParent;
    NvHandle hMemory;
    NvU32    hClass;
    NvHandle hDevice;
} ACR_REGION_HANDLE;

typedef struct ACR_FALCON_LS_STATUS {
    NvU16  falconId;
    NvBool bIsInLs;
} ACR_FALCON_LS_STATUS;

#define NV2080_CTRL_CMD_FB_QUERY_ACR_REGION_PARAMS_MESSAGE_ID (0x25U)

typedef struct NV2080_CTRL_CMD_FB_QUERY_ACR_REGION_PARAMS {
    NV2080_CTRL_CMD_FB_ACR_QUERY_TYPE       queryType;
    NV2080_CTRL_CMD_FB_ACR_QUERY_ERROR_CODE errorCode;
    NV_DECLARE_ALIGNED(ACR_REGION_ID_PROP acrRegionIdProp, 8);
    ACR_REQUEST_PARAMS                      clientReq;
    NV_DECLARE_ALIGNED(ACR_STATUS_PARAMS clientReqStatus, 8);
    ACR_REGION_HANDLE                       handle;
    ACR_FALCON_LS_STATUS                    falconStatus;
} NV2080_CTRL_CMD_FB_QUERY_ACR_REGION_PARAMS;

/*
 * NV2080_CTRL_CMD_FB_CLEAR_OFFLINED_PAGES
 *
 * This command clears offlined video memory page addresses from the Inforom.
 *
 *   sourceMask
 *     This is a bit mask of NV2080_CTRL_FB_OFFLINED_PAGES_SOURCE. Pages
 *     offlined from the specified sources will be cleared/removed from the
 *     Inforom PBL object denylist.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_FB_CLEAR_OFFLINED_PAGES (0x20801326U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FB_INTERFACE_ID << 8) | NV2080_CTRL_FB_CLEAR_OFFLINED_PAGES_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_FB_CLEAR_OFFLINED_PAGES_PARAMS_MESSAGE_ID (0x26U)

typedef struct NV2080_CTRL_FB_CLEAR_OFFLINED_PAGES_PARAMS {
    NvU32 sourceMask;
} NV2080_CTRL_FB_CLEAR_OFFLINED_PAGES_PARAMS;

/*!
 * NV2080_CTRL_CMD_FB_GET_COMPBITCOPY_INFO
 *
 * Gets pointer to then object of class CompBitCopy, which is used for swizzling
 * compression bits in the compression backing store. The caller is expected to
 * have the appropriate headers for class CompBitCopy. Also retrieves values of some
 * parameters needed to call the compbit swizzling method.
 *
 * @params[out] void *pCompBitCopyObj
 *     Opaque pointer to object of class CompBitCopy
 * @params[out] void *pSwizzleParams
 *     Opaque pointer to values needed to call the compbit
 *     swizzle method.
 *
 * Possible status values returned are:
 *   NV_OK NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_FB_GET_COMPBITCOPY_INFO (0x20801327U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FB_INTERFACE_ID << 8) | NV2080_CTRL_CMD_FB_GET_COMPBITCOPY_INFO_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CMD_FB_GET_COMPBITCOPY_INFO_PARAMS_MESSAGE_ID (0x27U)

typedef struct NV2080_CTRL_CMD_FB_GET_COMPBITCOPY_INFO_PARAMS {
    NV_DECLARE_ALIGNED(NvP64 pCompBitCopyObj, 8);
    NV_DECLARE_ALIGNED(NvP64 pSwizzleParams, 8);
} NV2080_CTRL_CMD_FB_GET_COMPBITCOPY_INFO_PARAMS;

/*
 * NV2080_CTRL_CMD_FB_GET_LTC_INFO_FOR_FBP
 *
 * Gets the count and mask of LTCs for a given FBP.
 *
 *   fbpIndex
 *     The physical index of the FBP to get LTC info for.
 *   ltcMask
 *     The mask of active LTCs for the given FBP.
 *   ltcCount
 *     The count of active LTCs for the given FBP.
 *   ltsMask
 *      The mask of active LTSs for the given FBP
 *   ltsCount
 *      The count of active LTSs for the given FBP
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_FB_GET_LTC_INFO_FOR_FBP (0x20801328U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FB_INTERFACE_ID << 8) | NV2080_CTRL_FB_GET_LTC_INFO_FOR_FBP_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_FB_GET_LTC_INFO_FOR_FBP_PARAMS_MESSAGE_ID (0x28U)

typedef struct NV2080_CTRL_FB_GET_LTC_INFO_FOR_FBP_PARAMS {
    NvU8  fbpIndex;
    NvU32 ltcMask;
    NvU32 ltcCount;
    NvU32 ltsMask;
    NvU32 ltsCount;
} NV2080_CTRL_FB_GET_LTC_INFO_FOR_FBP_PARAMS;


/*!
 * NV2080_CTRL_CMD_FB_COMPBITCOPY_SET_CONTEXT               < Deprecated >
 *
 * "set the context" for following CompBitCopy member functions.
 * These are the CompBitCopy member variables that remain connstant
 * over multiple CompBitCopy member function calls, yet stay the same
 * throughout a single surface eviction.
 *
 * @params[in] UINT64  backingStorePA;
 *     Physical Address of the Backing Store
 * @params[in] UINT08 *backingStoreVA;
 *     Virtual Address of the Backing Store
 * @params[in] UINT64  backingStoreChunkPA;
 *     Physical Address of the "Chunk Buffer"
 * @params[in] UINT08 *backingStoreChunkVA;
 *     Virtual Address of the "Chunk Buffer"
 * @params[in] UINT32  backingStoreChunkSize;
 *     Size of the "Chunk Buffer"
 * @params[in] UINT08 *cacheWriteBitMap;
 *     Pointer to the bitmap which parts of the
 *     "Chunk" was updated.
 * @params[in] bool    backingStoreChunkOverfetch;
 *     Overfetch factor.
 * @params[in] UINT32  PageSizeSrc;
 *     Page size of Source Surface.
 * @params[in] UINT32  PageSizeDest;
 *     page size of Destination Surface.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_FB_COMPBITCOPY_SET_CONTEXT (0x20801329U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FB_INTERFACE_ID << 8) | 0x29" */

typedef struct NV2080_CTRL_CMD_FB_COMPBITCOPY_SET_CONTEXT_PARAMS {
    NvU32  CBCBaseAddress;
    NV_DECLARE_ALIGNED(NvU64 backingStorePA, 8);
    NV_DECLARE_ALIGNED(NvU8 *backingStoreVA, 8);
    NV_DECLARE_ALIGNED(NvU64 backingStoreChunkPA, 8);
    NV_DECLARE_ALIGNED(NvU8 *backingStoreChunkVA, 8);
    NvU32  backingStoreChunkSize;
    NV_DECLARE_ALIGNED(NvU8 *cacheWriteBitMap, 8);
    NvBool backingStoreChunkOverfetch;
    NvU32  PageSizeSrc;
    NvU32  PageSizeDest;
} NV2080_CTRL_CMD_FB_COMPBITCOPY_SET_CONTEXT_PARAMS;

/*!
 * NV2080_CTRL_CMD_FB_COMPBITCOPY_GET_COMPBITS              < Deprecated >
 *
 * Retrieves the Compression and Fast Clear bits for the surface+offset given.
 *
 * @params[out] NvU32  *fcbits;
 *     Fast Clear Bits returned
 * @params[out] NvU32  *compbits;
 *     Compression Bits returned
 * @params[in] NvU64  dataPhysicalStart;
 *     Start Address of Data
 * @params[in] NvU64  surfaceOffset;
 *     Offset in the surface
 * @params[in] NvU32  comptagLine;
 *     Compression Tag Number
 * @params[in] NvBool upper64KBCompbitSel;
 *     Selects Upper or Lower 64K
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTEDD
 */
#define NV2080_CTRL_CMD_FB_COMPBITCOPY_GET_COMPBITS (0x2080132aU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FB_INTERFACE_ID << 8) | 0x2A" */

typedef struct NV2080_CTRL_CMD_FB_COMPBITCOPY_GET_COMPBITS_PARAMS {
    NV_DECLARE_ALIGNED(NvU32 *fcbits, 8);
    NV_DECLARE_ALIGNED(NvU32 *compbits, 8);
    NV_DECLARE_ALIGNED(NvU64 dataPhysicalStart, 8);
    NV_DECLARE_ALIGNED(NvU64 surfaceOffset, 8);
    NvU32  comptagLine;
    NvBool upper64KBCompbitSel;
} NV2080_CTRL_CMD_FB_COMPBITCOPY_GET_COMPBITS_PARAMS;

/*!
 * NV2080_CTRL_CMD_FB_COMPBITCOPY_PUT_COMPBITS              < Deprecated >
 *
 * Sets the Compression and Fast Clear bits for the surface+offset given.
 *
 * @params[in] NvU32  fcbits;
 *     Fast Clear Bits to write.
 * @params[in] NvU32  compbits;
 *     Compression Bits to write
 * @params[in] NvBool writeFc;
 *     Indicates if Fast Clear Bits should be written
 * @params[in] NvU64  dataPhysicalStart;
 *     Start Address of Data
 * @params[in] NvU64  surfaceOffset;
 *     Offset in the surface
 * @params[in] NvU32  comptagLine;
 *     Compression Tag Number
 * @params[in] NvBool upper64KBCompbitSel;
 *     Selects Upper or Lower 64K
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_FB_COMPBITCOPY_PUT_COMPBITS (0x2080132bU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FB_INTERFACE_ID << 8) | 0x2B" */

typedef struct NV2080_CTRL_CMD_FB_COMPBITCOPY_PUT_COMPBITS_PARAMS {
    NvU32  fcbits;
    NvU32  compbits;
    NvBool writeFc;
    NV_DECLARE_ALIGNED(NvU64 dataPhysicalStart, 8);
    NV_DECLARE_ALIGNED(NvU64 surfaceOffset, 8);
    NvU32  comptagLine;
    NvBool upper64KBCompbitSel;
} NV2080_CTRL_CMD_FB_COMPBITCOPY_PUT_COMPBITS_PARAMS;

/*!
 * NV2080_CTRL_CMD_FB_COMPBITCOPY_READ_COMPBITS64KB         < Deprecated >
 *
 * Read 64KB chunk of CompBits
 *
 * @params[in] NvU64  SrcDataPhysicalStart;
 *     Start Address of Data
 * @params[in] NvU32  SrcComptagLine;
 *     Compression Tag Number
 * @params[in] NvU32  page64KB;
 *     Which 64K block to read from.
 * @params[out] NvU32  *compbitBuffer;
 *     Buffer for CompBits read,
 * @params[in] NvBool upper64KBCompbitSel;
 *     Selects Upper or Lower 64K
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_FB_COMPBITCOPY_READ_COMPBITS64KB (0x2080132cU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FB_INTERFACE_ID << 8) | 0x2C" */

typedef struct NV2080_CTRL_CMD_FB_COMPBITCOPY_READ_COMPBITS64KB_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 SrcDataPhysicalStart, 8);
    NvU32  SrcComptagLine;
    NvU32  page64KB;
    NV_DECLARE_ALIGNED(NvU32 *compbitBuffer, 8);
    NvBool upper64KBCompbitSel;
} NV2080_CTRL_CMD_FB_COMPBITCOPY_READ_COMPBITS64KB_PARAMS;

/*!
 * NV2080_CTRL_CMD_FB_COMPBITCOPY_WRITE_COMPBITS64KB        < Deprecated >
 *
 * Write 64K chunk of COmpBits.
 *
 * @params[in] NvU64  SrcDataPhysicalStart;
 *     Start Address of Data
 * @params[in] NvU32  SrcComptagLine;
 *     Compression Tag Number
 * @params[in] NvU32  page64KB;
 *     Which 64K block to read from.
 * @params[in] NvU32  *compbitBuffer;
 *     Buffer for CompBits to write.
 * @params[in] NvBool upper64KBCompbitSel
 *     Selects Upper or Lower 64K
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_FB_COMPBITCOPY_WRITE_COMPBITS64KB (0x2080132dU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FB_INTERFACE_ID << 8) | 0x2D" */

typedef struct NV2080_CTRL_CMD_FB_COMPBITCOPY_WRITE_COMPBITS64KB_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 DstDataPhysicalStart, 8);
    NvU32  DstComptagLine;
    NvU32  page64KB;
    NV_DECLARE_ALIGNED(NvU32 *compbitBuffer, 8);
    NvBool upper64KBCompbitSel;
} NV2080_CTRL_CMD_FB_COMPBITCOPY_WRITE_COMPBITS64KB_PARAMS;

/*!
 * NV2080_CTRL_CMD_FB_COMPBITCOPY_SET_FORCE_BAR1                    < Deprecated >
 *
 * Used by MODS (and possibly other clients) to have compbit code write
 * write directly to BAR1, rather than a intermediate buffer.
 *
 * @params[in] NvBool bForceBar1;
 *     Enables or disables direct writes to BAR1.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_FB_COMPBITCOPY_SET_FORCE_BAR1 (0x20801335U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FB_INTERFACE_ID << 8) | 0x35" */

typedef struct NV2080_CTRL_CMD_FB_COMPBITCOPY_SET_FORCE_BAR1_PARAMS {
    NvBool bForceBar1;
} NV2080_CTRL_CMD_FB_COMPBITCOPY_SET_FORCE_BAR1_PARAMS;

/*!
 * NV2080_CTRL_CMD_FB_GET_AMAP_CONF
 *
 * Fills in fields of a structure of class ConfParamsV1, which is used for
 * swizzling compression bits in the compression backing store.
 * The caller is expected to have the appropriate headers for class ConfParamsV1.
 *
 * @params[in|out] void *pAmapConfParms
 *     Opaque pointer to structure of values for ConfParamsV1
 * @params[in|out] void *pCbcSwizzleParms
 *     Opaque pointer to structure of values for CbcSwizzleParamsV1
 *
 * Possible status values returned are:
 *   NV_OK NV_ERR_NOT_SUPPORTED
 *
 * pCbcSwizzleParams will be filled in with certain parameters from
 * @CbcSwizzleParamsV1.  However, the caller is responsible for making sure
 * all parameters are filled in before using it.
 */
#define NV2080_CTRL_CMD_FB_GET_AMAP_CONF (0x20801336U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FB_INTERFACE_ID << 8) | NV2080_CTRL_CMD_FB_GET_AMAP_CONF_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CMD_FB_GET_AMAP_CONF_PARAMS_MESSAGE_ID (0x36U)

typedef struct NV2080_CTRL_CMD_FB_GET_AMAP_CONF_PARAMS {
    NV_DECLARE_ALIGNED(NvP64 pAmapConfParams, 8);
    NV_DECLARE_ALIGNED(NvP64 pCbcSwizzleParams, 8);
} NV2080_CTRL_CMD_FB_GET_AMAP_CONF_PARAMS;

/*!
 * NV2080_CTRL_CMD_FB_CBC_OP
 *
 * Provides a way for clients to request a CBC Operation
 *
 * @params[in] CTRL_CMD_FB_CBC_OP fbCBCOp
 *      CBC Operation requested.
 *      Valid Values:
 *          CTRL_CMD_FB_CBC_OP_CLEAN
 *          CTRL_CMD_FB_CBC_OP_INVALIDATE
 *
 * Possible status values returned are:
 *   NV_OK NV_ERR_NOT_SUPPORTED NV_ERR_INVALID_ARGUMENT NV_ERR_TIMEOUT
 */
#define NV2080_CTRL_CMD_FB_CBC_OP (0x20801337U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FB_INTERFACE_ID << 8) | NV2080_CTRL_CMD_FB_CBC_OP_PARAMS_MESSAGE_ID" */

/*!
 * Permitted CBC Operations
 */
typedef enum CTRL_CMD_FB_CBC_OP {
    CTRL_CMD_FB_CBC_OP_CLEAN = 0,
    CTRL_CMD_FB_CBC_OP_INVALIDATE = 1,
} CTRL_CMD_FB_CBC_OP;

#define NV2080_CTRL_CMD_FB_CBC_OP_PARAMS_MESSAGE_ID (0x37U)

typedef struct NV2080_CTRL_CMD_FB_CBC_OP_PARAMS {
    CTRL_CMD_FB_CBC_OP fbCBCOp;
} NV2080_CTRL_CMD_FB_CBC_OP_PARAMS;

/*!
 *  NV2080_CTRL_CMD_FB_GET_CTAGS_FOR_CBC_EVICTION
 *
 *  The call will fetch the compression tags reserved for CBC eviction.
 *
 *  Each comptag will correspond to a unique compression cacheline. The usage of
 *  these comptags is to evict the CBC by making accesses to a dummy compressed page,
 *  thereby evicting each CBC line.
 *
 *  @param [in][out] NvU32 pCompTags
 *     Array of reserved compression tags of size @ref NV2080_MAX_CTAGS_FOR_CBC_EVICTION
 *  @param [out] numCompTags
 *     Number of entries returned in @ref pCompTags
 *
 *  @returns
 *      NV_OK
 *      NV_ERR_INVALID_STATE
 *      NV_ERR_OUT_OF_RANGE
 *      NV_ERR_INVALID_PARAMETER
 */
#define NV2080_CTRL_CMD_FB_GET_CTAGS_FOR_CBC_EVICTION (0x20801338U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FB_INTERFACE_ID << 8) | NV2080_CTRL_FB_GET_CTAGS_FOR_CBC_EVICTION_PARAMS_MESSAGE_ID" */

/*!
 * Max size of @ref NV2080_CTRL_FB_GET_CTAGS_FOR_CBC_EVICTION_PARAMS::pCompTags
 * Arbitrary, but sufficiently large number. Should be checked against CBC size.
 */
#define NV2080_MAX_CTAGS_FOR_CBC_EVICTION             0x7FU


#define NV2080_CTRL_FB_GET_CTAGS_FOR_CBC_EVICTION_PARAMS_MESSAGE_ID (0x38U)

typedef struct NV2080_CTRL_FB_GET_CTAGS_FOR_CBC_EVICTION_PARAMS {
    NvU32 pCompTags[NV2080_MAX_CTAGS_FOR_CBC_EVICTION];
    NvU32 numCompTags;
} NV2080_CTRL_FB_GET_CTAGS_FOR_CBC_EVICTION_PARAMS;

/*!
 * NV2080_CTRL_CMD_FB_ALLOC_COMP_RESOURCE
 *
 * This Call will allocate compression tag
 *
 * @params[in] NvU32 attr
 *      Stores the information:
 *          1. NVOS32_ATTR_COMPR_REQUIRED or not
 *          2. NVOS32_ATTR_PAGE_SIZE
 * @params[in] NvU32 attr2
 *      Determine whether to allocate
 *      an entire cache line or allocate by size
 * @params[in] NvU32 size
 *      Specify the size of allocation, in pages not bytes
 * @params[in] NvU32 ctagOffset
 *      Determine the offset usage of the allocation
 * @params[out] NvU32 hwResId
 *      Stores the result of the allocation
 * @params[out] NvU32 RetcompTagLineMin
 *      The resulting min Ctag Number from the allocation
 * @params[out] NvU32 RetcompTagLineMax
 *      The resulting max Ctag Number from the allocation
 * @returns
 *      NV_OK
 *      NV_ERR_INSUFFICIENT_RESOURCES
 *      NV_ERR_INVALID_ARGUMENT
 *      NV_ERR_INVALID_STATE
 */

#define NV2080_CTRL_CMD_FB_ALLOC_COMP_RESOURCE (0x20801339U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FB_INTERFACE_ID << 8) | NV2080_CTRL_CMD_FB_ALLOC_COMP_RESOURCE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CMD_FB_ALLOC_COMP_RESOURCE_PARAMS_MESSAGE_ID (0x39U)

typedef struct NV2080_CTRL_CMD_FB_ALLOC_COMP_RESOURCE_PARAMS {
    NvU32 attr;
    NvU32 attr2;
    NvU32 size;
    NvU32 ctagOffset;
    NvU32 hwResId;
    NvU32 retCompTagLineMin;
    NvU32 retCompTagLineMax;
} NV2080_CTRL_CMD_FB_ALLOC_COMP_RESOURCE_PARAMS;

/*!
 * NV2080_CTRL_CMD_FB_FREE_TILE
 *
 * This control call is used to release tile back to the free pool
 *
 * @params[in] NvU32 hwResId
 *      Stores the information of a previous allocation
 * @returns
 *      NV_OK
 *      NV_ERR_INVALID_ARGUMENT
 *      NV_ERR_INVALID_STATE
 */

#define NV2080_CTRL_CMD_FB_FREE_TILE (0x2080133aU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FB_INTERFACE_ID << 8) | NV2080_CTRL_CMD_FB_FREE_TILE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CMD_FB_FREE_TILE_PARAMS_MESSAGE_ID (0x3AU)

typedef struct NV2080_CTRL_CMD_FB_FREE_TILE_PARAMS {
    NvU32 hwResId;
} NV2080_CTRL_CMD_FB_FREE_TILE_PARAMS;


/*
 * NV2080_CTRL_CMD_FB_SETUP_VPR_REGION
 *
 * This control command is used to request vpr region setup
 *
 * requestType
 *          NV2080_CTRL_CMD_FB_SET_VPR: Request to setup VPR
 *
 * requestParams : struct VPR_REQUEST_PARAMS
 *          It contains the VPR region request details like,
 *          startAddr : FB offset from which we need to setup VPR
 *          size      : required size of the region
 *
 * statusParams  : struct VPR_STATUS_PARAMS
 *          This struct stores the output of requested VPR region
 *          status    : Whether the request was successful
 *
 * NV2080_CTRL_CMD_FB_VPR_ERROR_CODE :
 *          NV2080_CTRL_CMD_FB_VPR_ERROR_GENERIC : Some unknown error occurred
 *          NV2080_CTRL_CMD_FB_VPR_ERROR_INVALID_CLIENT_REQUEST : Request was invalid
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_FB_SETUP_VPR_REGION (0x2080133bU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FB_INTERFACE_ID << 8) | NV2080_CTRL_CMD_FB_SETUP_VPR_REGION_PARAMS_MESSAGE_ID" */

typedef enum NV2080_CTRL_CMD_FB_VPR_REQUEST_TYPE {
    NV2080_CTRL_CMD_FB_SET_VPR = 0,
} NV2080_CTRL_CMD_FB_VPR_REQUEST_TYPE;

typedef enum NV2080_CTRL_CMD_FB_VPR_ERROR_CODE {
    NV2080_CTRL_CMD_FB_VPR_ERROR_GENERIC = 0,
    NV2080_CTRL_CMD_FB_VPR_ERROR_INVALID_CLIENT_REQUEST = 1,
} NV2080_CTRL_CMD_FB_VPR_ERROR_CODE;

typedef struct VPR_REQUEST_PARAMS {
    NvU32 startAddr;
    NvU32 size;
} VPR_REQUEST_PARAMS;

typedef struct VPR_STATUS_PARAMS {
    NvU32 status;
} VPR_STATUS_PARAMS;

#define NV2080_CTRL_CMD_FB_SETUP_VPR_REGION_PARAMS_MESSAGE_ID (0x3BU)

typedef struct NV2080_CTRL_CMD_FB_SETUP_VPR_REGION_PARAMS {
    NV2080_CTRL_CMD_FB_VPR_REQUEST_TYPE requestType;
    VPR_REQUEST_PARAMS                  requestParams;
    VPR_STATUS_PARAMS                   statusParams;
} NV2080_CTRL_CMD_FB_SETUP_VPR_REGION_PARAMS;
typedef struct NV2080_CTRL_CMD_FB_SETUP_VPR_REGION_PARAMS *PNV2080_CTRL_CMD_FB_SETUP_VPR_REGION_PARAMS;

/*
 * NV2080_CTRL_CMD_FB_GET_CLI_MANAGED_OFFLINED_PAGES
 *
 * This command returns the list of offlined video memory page addresses in the
 * region managed by Client
 *
 *   offlinedPages
 *     This output parameter is an array of video memory physical page numbers that
 *     are offlined. This array can hold a maximum of NV2080_CTRL_FB_
 *     OFFLINED_PAGES_MAX_PAGES addresses.
 *   pageSize
 *     This output parameter contains the size of the page that is offlined.
 *   validEntries
 *     This output parameter specifies the number of valid entries in the
 *     offlined array.
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NV2080_CTRL_CMD_FB_GET_CLI_MANAGED_OFFLINED_PAGES (0x2080133cU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FB_INTERFACE_ID << 8) | NV2080_CTRL_FB_GET_CLI_MANAGED_OFFLINED_PAGES_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_FB_GET_CLI_MANAGED_OFFLINED_PAGES_PARAMS_MESSAGE_ID (0x3CU)

typedef struct NV2080_CTRL_FB_GET_CLI_MANAGED_OFFLINED_PAGES_PARAMS {
    NvU32 offlinedPages[NV2080_CTRL_FB_OFFLINED_PAGES_MAX_PAGES];    // A 32B can hold enough.
    NvU32 pageSize;
    NvU32 validEntries;
} NV2080_CTRL_FB_GET_CLI_MANAGED_OFFLINED_PAGES_PARAMS;

/*!
 * NV2080_CTRL_CMD_FB_GET_COMPBITCOPY_CONSTRUCT_INFO
 *
 * This command returns parameters required to initialize compbit copy object
 * used by address mapping library
 *
 *   defaultPageSize
 *     Page size used by @ref CompBitCopy methods
 *   comptagsPerCacheLine
 *     Number of compression tags in a single compression cache line.
 *   unpackedComptagLinesPerCacheLine;
 *     From hw (not adjusted for CompBits code)  Number of compression tags
 *     in a single compression cache line.
 *   compCacheLineSizePerLTC;
 *     Size of compression cache line per L2 slice. Size in Bytes.
 *   unpackedCompCacheLineSizePerLTC;
 *     From hw (not adjusted for CompBits code) size of compression
 *     cache line per L2 slice. Size in Bytes
 *   slicesPerLTC;
 *     Number of L2 slices per L2 cache.
 *   numActiveLTCs;
 *     Number of active L2 caches. (Not floorswept)
 *   familyName;
 *     Family name for the GPU.
 *   chipName;
 *     Chip name for the GPU.
 *   bitsPerRAMEntry;
 *     Bits per RAM entry. (Need better doc)
 *   ramBankWidth;
 *     Width of RAM bank. (Need better doc)
 *   bitsPerComptagLine;
 *     Number of bits per compression tag line.
 *   ramEntriesPerCompCacheLine;
 *     Number of RAM entries spanned by 1 compression cache line.
 *   comptagLineSize;
 *     Size of compression tag line, in Bytes.
 *
 * Possible status values returned are:
 *   NV_OK
 */

#define NV2080_CTRL_CMD_FB_GET_COMPBITCOPY_CONSTRUCT_INFO (0x2080133dU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FB_INTERFACE_ID << 8) | NV2080_CTRL_CMD_FB_GET_COMPBITCOPY_CONSTRUCT_INFO_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CMD_FB_GET_COMPBITCOPY_CONSTRUCT_INFO_PARAMS_MESSAGE_ID (0x3DU)

typedef struct NV2080_CTRL_CMD_FB_GET_COMPBITCOPY_CONSTRUCT_INFO_PARAMS {
    NvU32 defaultPageSize;
    NvU32 comptagsPerCacheLine;
    NvU32 unpackedComptagLinesPerCacheLine;
    NvU32 compCacheLineSizePerLTC;
    NvU32 unpackedCompCacheLineSizePerLTC;
    NvU32 slicesPerLTC;
    NvU32 numActiveLTCs;
    NvU32 familyName;
    NvU32 chipName;
    NvU32 bitsPerRAMEntry;
    NvU32 ramBankWidth;
    NvU32 bitsPerComptagLine;
    NvU32 ramEntriesPerCompCacheLine;
    NvU32 comptagLineSize;
} NV2080_CTRL_CMD_FB_GET_COMPBITCOPY_CONSTRUCT_INFO_PARAMS;

/*
 * NV2080_CTRL_CMD_FB_SET_RRD
 *
 * Sets the row-to-row delay on the GPU's FB
 *
 * Possible status values returned are:
 *  NV_OK
 *  Any error code
 */
#define NV2080_CTRL_CMD_FB_SET_RRD (0x2080133eU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FB_INTERFACE_ID << 8) | NV2080_CTRL_FB_SET_RRD_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_FB_SET_RRD_RESET_VALUE (~((NvU32)0))
#define NV2080_CTRL_FB_SET_RRD_PARAMS_MESSAGE_ID (0x3EU)

typedef struct NV2080_CTRL_FB_SET_RRD_PARAMS {
    NvU32 rrd;
} NV2080_CTRL_FB_SET_RRD_PARAMS;

/*
 * NV2080_CTRL_FB_SET_READ_WRITE_LIMIT_PARAMS
 *
 * This is not a control call of it's own, but there are common definitions for
 * the two NV2080_CTRL_CMD_FB_SET_READ/WRITE_LIMIT control calls.
 */
typedef struct NV2080_CTRL_FB_SET_READ_WRITE_LIMIT_PARAMS {
    NvU8 limit;
} NV2080_CTRL_FB_SET_READ_WRITE_LIMIT_PARAMS;
#define NV2080_CTRL_FB_SET_READ_WRITE_LIMIT_RESET_VALUE (0xffU)

/*
 * NV2080_CTRL_CMD_FB_SET_READ_LIMIT
 *
 * Sets the READ_LIMIT to be used in the NV_PFB_FBPA_DIR_ARB_CFG0 register
 *
 *  limit
 *      The limit value to use
 *
 * Possible status values returned are:
 *  NV_OK
 *  Any error code
 */
#define NV2080_CTRL_CMD_FB_SET_READ_LIMIT               (0x2080133fU) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FB_INTERFACE_ID << 8) | NV2080_CTRL_FB_SET_READ_LIMIT_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_FB_SET_READ_LIMIT_RESET_VALUE       NV2080_CTRL_FB_SET_READ_WRITE_LIMIT_RESET_VALUE
#define NV2080_CTRL_FB_SET_READ_LIMIT_PARAMS_MESSAGE_ID (0x3FU)

typedef NV2080_CTRL_FB_SET_READ_WRITE_LIMIT_PARAMS NV2080_CTRL_FB_SET_READ_LIMIT_PARAMS;

/*
 * NV2080_CTRL_CMD_FB_SET_WRITE_LIMIT
 *
 * Sets the WRITE_LIMIT to be used in the NV_PFB_FBPA_DIR_ARB_CFG0 register
 *
 *  limit
 *      The limit value to us
 *
 * Possible status values returned are:
 *  NV_OK
 *  Any error code
 */
#define NV2080_CTRL_CMD_FB_SET_WRITE_LIMIT         (0x20801340U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FB_INTERFACE_ID << 8) | NV2080_CTRL_FB_SET_WRITE_LIMIT_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_FB_SET_WRITE_LIMIT_RESET_VALUE NV2080_CTRL_FB_SET_READ_WRITE_LIMIT_RESET_VALUE
#define NV2080_CTRL_FB_SET_WRITE_LIMIT_PARAMS_MESSAGE_ID (0x40U)

typedef NV2080_CTRL_FB_SET_READ_WRITE_LIMIT_PARAMS NV2080_CTRL_FB_SET_WRITE_LIMIT_PARAMS;

/*!
 * NV2080_CTRL_CMD_FB_PATCH_PBR_FOR_MINING
 *
 * Patches some VBIOS values related to PBR to better suit mining applications
 *
 *  bEnable
 *      Set the mining-specific values or reset to the original values
 *
 * Possible status values returned are:
 *  NV_OK
 *  Any error code
 */
#define NV2080_CTRL_CMD_FB_PATCH_PBR_FOR_MINING (0x20801341U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FB_INTERFACE_ID << 8) | NV2080_CTRL_FB_PATCH_PBR_FOR_MINING_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_FB_PATCH_PBR_FOR_MINING_PARAMS_MESSAGE_ID (0x41U)

typedef struct NV2080_CTRL_FB_PATCH_PBR_FOR_MINING_PARAMS {
    NvBool bEnable;
} NV2080_CTRL_FB_PATCH_PBR_FOR_MINING_PARAMS;

/*!
 * NV2080_CTRL_CMD_FB_GET_MEM_ALIGNMENT
 *
 * Get memory alignment. Replacement for NVOS32_FUNCTION_GET_MEM_ALIGNMENT
 */
#define NV2080_CTRL_CMD_FB_GET_MEM_ALIGNMENT       (0x20801342U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FB_INTERFACE_ID << 8) | NV2080_CTRL_FB_GET_MEM_ALIGNMENT_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_FB_GET_MEM_ALIGNMENT_MAX_BANKS (4U)
#define NV2080_CTRL_FB_GET_MEM_ALIGNMENT_PARAMS_MESSAGE_ID (0x42U)

typedef struct NV2080_CTRL_FB_GET_MEM_ALIGNMENT_PARAMS {
    NvU32 alignType;                                 // Input
    NvU32 alignAttr;
    NvU32 alignInputFlags;
    NvU32 alignHead;
    NV_DECLARE_ALIGNED(NvU64 alignSize, 8);
    NvU32 alignHeight;
    NvU32 alignWidth;
    NvU32 alignPitch;
    NvU32 alignPad;
    NvU32 alignMask;
    NvU32 alignOutputFlags[NV2080_CTRL_FB_GET_MEM_ALIGNMENT_MAX_BANKS];
    NvU32 alignBank[NV2080_CTRL_FB_GET_MEM_ALIGNMENT_MAX_BANKS];
    NvU32 alignKind;
    NvU32 alignAdjust;                                // Output -- If non-zero the amount we need to adjust the offset
    NvU32 alignAttr2;
} NV2080_CTRL_FB_GET_MEM_ALIGNMENT_PARAMS;

/*!
 * NV2080_CTRL_CMD_FB_GET_CBC_BASEADDR
 *
 * Get the CBC Base physical address
 * This control call is required by error containment tests
 * NV2080_CTRL_CMD_FB_GET_AMAP_CONF can also return CBC base address
 * but it requires kernel privilege, and it not callalble from SRT test
 *
 * @params[out] NvU64 cbcBaseAddr
 *     Base physical address for CBC data.
 *
 * Possible status values returned are:
 *   NV_OK NV_ERR_NOT_SUPPORTED
 *
 */
#define NV2080_CTRL_CMD_FB_GET_CBC_BASE_ADDR (0x20801343U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FB_INTERFACE_ID << 8) | NV2080_CTRL_CMD_FB_GET_CBC_BASE_ADDR_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CMD_FB_GET_CBC_BASE_ADDR_PARAMS_MESSAGE_ID (0x43U)

typedef struct NV2080_CTRL_CMD_FB_GET_CBC_BASE_ADDR_PARAMS {
    NvU32 cbcBaseAddress;
    NvU32 compCacheLineSize;
    NV_DECLARE_ALIGNED(NvU64 backingStoreStartPA, 8);
    NV_DECLARE_ALIGNED(NvU64 backingStoreAllocPA, 8);
    NvU32 backingStoreChunkOverfetch;
} NV2080_CTRL_CMD_FB_GET_CBC_BASE_ADDR_PARAMS;

#define NV2080_CTRL_FB_REMAP_ENTRY_FLAGS_PENDING                             0:0
#define NV2080_CTRL_FB_REMAP_ENTRY_FLAGS_PENDING_FALSE 0U
#define NV2080_CTRL_FB_REMAP_ENTRY_FLAGS_PENDING_TRUE  1U



typedef struct NV2080_CTRL_FB_REMAP_ENTRY {
    NvU32 remapRegVal;
    NvU32 timestamp;
    NvU8  fbpa;
    NvU8  sublocation;
    NvU8  source;
    NvU8  flags;
} NV2080_CTRL_FB_REMAP_ENTRY;

/* valid values for source */


#define NV2080_CTRL_FB_REMAPPED_ROW_SOURCE_SBE_FIELD         (0x00000002U)
#define NV2080_CTRL_FB_REMAPPED_ROW_SOURCE_DBE_FIELD         (0x00000003U)

#define NV2080_CTRL_FB_REMAPPED_ROWS_MAX_ROWS                (0x00000200U)

/*
 * NV2080_CTRL_CMD_FB_GET_REMAPPED_ROWS
 *
 * This command returns the list of remapped rows stored in the Inforom.
 *
 *   entryCount
 *     This output parameter specifies the number of remapped rows
 *   flags
 *     This output parameter contains info on whether or not there are pending
 *     remappings and whether or not a remapping failed
 *   entries
 *     This output parameter is an array of NV2080_CTRL_FB_REMAP_ENTRY
 *     containing inforomation on the remapping that occurred. This array can
 *     hold a maximum of NV2080_CTRL_FB_REMAPPED_ROWS_MAX_ROWS
 *
 *  Possible status values returned are:
 *    NV_OK
 *    NV_ERR_INVALID_ARGUMENT
 *    NV_ERR_INVALID_POINTER
 *    NV_ERR_OBJECT_NOT_FOUND
 *    NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_FB_GET_REMAPPED_ROWS                 (0x20801344U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FB_INTERFACE_ID << 8) | NV2080_CTRL_FB_GET_REMAPPED_ROWS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_FB_GET_REMAPPED_ROWS_FLAGS_PENDING                       \
    NV2080_CTRL_FB_REMAP_ENTRY_FLAGS_PENDING
#define NV2080_CTRL_FB_GET_REMAPPED_ROWS_FLAGS_PENDING_FALSE NV2080_CTRL_FB_REMAP_ENTRY_FLAGS_PENDING_FALSE
#define NV2080_CTRL_FB_GET_REMAPPED_ROWS_FLAGS_PENDING_TRUE  NV2080_CTRL_FB_REMAP_ENTRY_FLAGS_PENDING_TRUE
#define NV2080_CTRL_FB_GET_REMAPPED_ROWS_FLAGS_FAILURE                       1:1
#define NV2080_CTRL_FB_GET_REMAPPED_ROWS_FLAGS_FAILURE_FALSE 0U
#define NV2080_CTRL_FB_GET_REMAPPED_ROWS_FLAGS_FAILURE_TRUE  1U

#define NV2080_CTRL_FB_GET_REMAPPED_ROWS_PARAMS_MESSAGE_ID (0x44U)

typedef struct NV2080_CTRL_FB_GET_REMAPPED_ROWS_PARAMS {
    NvU32                      entryCount;
    NvU8                       flags;
    NV2080_CTRL_FB_REMAP_ENTRY entries[NV2080_CTRL_FB_REMAPPED_ROWS_MAX_ROWS];
} NV2080_CTRL_FB_GET_REMAPPED_ROWS_PARAMS;

// Max size of the queryParams in Bytes, so that the NV2080_CTRL_FB_FS_INFO_QUERY struct is still 32B
#define NV2080_CTRL_FB_FS_INFO_MAX_QUERY_SIZE 24U

/*!
 * Structure holding the out params for NV2080_CTRL_FB_FS_INFO_INVALID_QUERY.
 */
typedef struct NV2080_CTRL_FB_FS_INFO_INVALID_QUERY_PARAMS {
    // Unused param, will ensure the size of NV2080_CTRL_FB_FS_INFO_QUERY struct to be 32B
    NvU8 data[NV2080_CTRL_FB_FS_INFO_MAX_QUERY_SIZE];
} NV2080_CTRL_FB_FS_INFO_INVALID_QUERY_PARAMS;

/*!
 * Structure holding the in/out params for NV2080_CTRL_FB_FS_INFO_FBP_MASK.
 */
typedef struct NV2080_CTRL_FB_FS_INFO_FBP_MASK_PARAMS {
    /*!
     * [in]: swizzId
     * PartitionID associated with a created smc partition. Currently used only for a
     * device monitoring client to get the physical values of the FB. The client needs to pass
     * 'NV2080_CTRL_GPU_PARTITION_ID_INVALID' explicitly if it wants RM to ignore the swizzId.
     * RM will consider this request similar to a legacy case.
     * The client's subscription is used only as a capability check and not as an input swizzId.
     */
    NvU32 swizzId;
    /*!
     * [out]: physical/local fbp mask.
     */
    NV_DECLARE_ALIGNED(NvU64 fbpEnMask, 8);
} NV2080_CTRL_FB_FS_INFO_FBP_MASK_PARAMS;

/*!
 * Structure holding the in/out params for NV2080_CTRL_FB_FS_INFO_LTC_MASK.
 */
typedef struct NV2080_CTRL_FB_FS_INFO_LTC_MASK_PARAMS {
    /*!
     * [in]: physical/local FB partition index.
     */
    NvU32 fbpIndex;
    /*!
     * [out]: physical/local ltc mask.
     */
    NvU32 ltcEnMask;
} NV2080_CTRL_FB_FS_INFO_LTC_MASK_PARAMS;

/*!
 * Structure holding the in/out params for NV2080_CTRL_FB_FS_INFO_LTS_MASK.
 */
typedef struct NV2080_CTRL_FB_FS_INFO_LTS_MASK_PARAMS {
    /*!
     * [in]: physical/local FB partition index.
     */
    NvU32 fbpIndex;
    /*!
     * [out]: physical/local lts mask.
     * Note that lts bits are flattened out for all ltc with in a fbp.
     */
    NvU32 ltsEnMask;
} NV2080_CTRL_FB_FS_INFO_LTS_MASK_PARAMS;

/*!
 * Structure holding the in/out params for NV2080_CTRL_FB_FS_INFO_FBPA_MASK.
 */
typedef struct NV2080_CTRL_FB_FS_INFO_FBPA_MASK_PARAMS {
    /*!
     * [in]: physical/local FB partition index.
     */
    NvU32 fbpIndex;
    /*!
     * [out]: physical/local FBPA mask.
     */
    NvU32 fbpaEnMask;
} NV2080_CTRL_FB_FS_INFO_FBPA_MASK_PARAMS;

/*!
 * Structure holding the in/out params for NV2080_CTRL_FB_FS_INFO_FBPA_SUBP_MASK.
 */
typedef struct NV2080_CTRL_FB_FS_INFO_FBPA_SUBP_MASK_PARAMS {
    /*!
     * [in]: physical/local FB partition index.
     */
    NvU32 fbpIndex;
    /*!
     * [out]: physical/local FBPA-SubPartition mask.
     */
    NvU32 fbpaSubpEnMask;
} NV2080_CTRL_FB_FS_INFO_FBPA_SUBP_MASK_PARAMS;

/*!
 * Structure holding the in/out params for NV2080_CTRL_FB_FS_INFO_FBP_LOGICAL_MAP
 */
typedef struct NV2080_CTRL_FB_FS_INFO_FBP_LOGICAL_MAP_PARAMS {
    /*!
     * [in]: physical/local FB partition index.
     */
    NvU32 fbpIndex;
    /*!
     * [out]: Logical/local FBP index
     */
    NvU32 fbpLogicalIndex;
} NV2080_CTRL_FB_FS_INFO_FBP_LOGICAL_MAP_PARAMS;

/*!
 * Structure holding the in/out params for NV2080_CTRL_FB_FS_INFO_ROP_MASK.
 */
typedef struct NV2080_CTRL_FB_FS_INFO_ROP_MASK_PARAMS {
    /*!
     * [in]: physical/local FB partition index.
     */
    NvU32 fbpIndex;
    /*!
     * [out]: physical/local ROP mask.
     */
    NvU32 ropEnMask;
} NV2080_CTRL_FB_FS_INFO_ROP_MASK_PARAMS;

/*!
 * Structure holding the in/out params for NV2080_CTRL_FB_FS_INFO_PROFILER_MON_LTC_MASK.
 */
typedef struct NV2080_CTRL_FB_FS_INFO_PROFILER_MON_LTC_MASK_PARAMS {
    /*!
     * [in]: Physical FB partition index.
     */
    NvU32 fbpIndex;
    /*!
     * [in]: swizzId
     * PartitionID associated with a created smc partition.
     */
    NvU32 swizzId;
    /*!
     * [out]: physical ltc mask.
     */
    NvU32 ltcEnMask;
} NV2080_CTRL_FB_FS_INFO_PROFILER_MON_LTC_MASK_PARAMS;

/*!
 * Structure holding the in/out params for NV2080_CTRL_FB_FS_INFO_PROFILER_MON_LTS_MASK.
 */
typedef struct NV2080_CTRL_FB_FS_INFO_PROFILER_MON_LTS_MASK_PARAMS {
    /*!
     * [in]: Physical FB partition index.
     */
    NvU32 fbpIndex;
    /*!
     * [in]: swizzId
     * PartitionID associated with a created smc partition.
     */
    NvU32 swizzId;
    /*!
     * [out]: physical lts mask.
     */
    NvU32 ltsEnMask;
} NV2080_CTRL_FB_FS_INFO_PROFILER_MON_LTS_MASK_PARAMS;

/*!
 * Structure holding the in/out params for NV2080_CTRL_FB_FS_INFO_PROFILER_MON_FBPA_MASK.
 */
typedef struct NV2080_CTRL_FB_FS_INFO_PROFILER_MON_FBPA_MASK_PARAMS {
    /*!
     * [in]: Physical FB partition index.
     */
    NvU32 fbpIndex;
    /*!
     * [in]: swizzId
     * PartitionID associated with a created smc partition.
     */
    NvU32 swizzId;
    /*!
     * [out]: physical fbpa mask.
     */
    NvU32 fbpaEnMask;
} NV2080_CTRL_FB_FS_INFO_PROFILER_MON_FBPA_MASK_PARAMS;

/*!
 * Structure holding the in/out params for NV2080_CTRL_FB_FS_INFO_PROFILER_MON_ROP_MASK.
 */
typedef struct NV2080_CTRL_FB_FS_INFO_PROFILER_MON_ROP_MASK_PARAMS {
    /*!
     * [in]: Physical FB partition index.
     */
    NvU32 fbpIndex;
    /*!
     * [in]: swizzId
     * PartitionID associated with a created smc partition.
     */
    NvU32 swizzId;
    /*!
     * [out]: physical rop mask.
     */
    NvU32 ropEnMask;
} NV2080_CTRL_FB_FS_INFO_PROFILER_MON_ROP_MASK_PARAMS;

/*!
 * Structure holding the in/out params for NV2080_CTRL_FB_FS_INFO_PROFILER_MON_FBPA_SUBP_MASK.
 */
typedef struct NV2080_CTRL_FB_FS_INFO_PROFILER_MON_FBPA_SUBP_MASK_PARAMS {
    /*!
     * [in]: Physical FB partition index.
     */
    NvU32 fbpIndex;
    /*!
     * [in]: swizzId
     * PartitionID associated with a created smc partition. Currently used only for a
     * device monitoring client to get the physical values of the FB. The client needs to pass
     * 'NV2080_CTRL_GPU_PARTITION_ID_INVALID' explicitly if it wants RM to ignore the swizzId.
     * RM will consider this request similar to a legacy case.
     * The client's subscription is used only as a capability check and not as an input swizzId.
     */
    NvU32 swizzId;
    /*!
     * [out]: physical FBPA_SubPartition mask associated with requested partition.
     */
    NV_DECLARE_ALIGNED(NvU64 fbpaSubpEnMask, 8);
} NV2080_CTRL_FB_FS_INFO_PROFILER_MON_FBPA_SUBP_MASK_PARAMS;

/*!
 * Structure holding the in/out params for NV2080_CTRL_SYSL2_FS_INFO_SYSLTC_MASK.
 */
typedef struct NV2080_CTRL_SYSL2_FS_INFO_SYSLTC_MASK_PARAMS {
    /*!
     * [in]: physical/local sys Id.
     */
    NvU32 sysIdx;
    /*!
     * [out]: physical/local sysltc mask.
     */
    NvU32 sysl2LtcEnMask;
} NV2080_CTRL_SYSL2_FS_INFO_SYSLTC_MASK_PARAMS;

/*!
 * Structure holding the in/out params for NV2080_CTRL_FB_FS_INFO_PAC_MASK.
 */
typedef struct NV2080_CTRL_FB_FS_INFO_PAC_MASK_PARAMS {
    /*!
     * [in]: physical/local FB partition index.
     */
    NvU32 fbpIndex;
    /*!
     * [out]: physical/local PAC mask.
     */
    NvU32 pacEnMask;
} NV2080_CTRL_FB_FS_INFO_PAC_MASK_PARAMS;

/*!
 * Structure holding the in/out params for NV2080_CTRL_FB_FS_INFO_LOGICAL_LTC_MASK.
 */
typedef struct NV2080_CTRL_FB_FS_INFO_LOGICAL_LTC_MASK_PARAMS {
    /*!
     * [in]: physical/local FB partition index.
     */
    NvU32 fbpIndex;
    /*!
     * [out]: logical/local ltc mask.
     */
    NV_DECLARE_ALIGNED(NvU64 logicalLtcEnMask, 8);
} NV2080_CTRL_FB_FS_INFO_LOGICAL_LTC_MASK_PARAMS;

/*!
 * Structure holding the in/out params for NV2080_CTRL_FB_FS_INFO_PROFILER_MON_LOGICAL_LTC_MASK.
 */
typedef struct NV2080_CTRL_FB_FS_INFO_PROFILER_MON_LOGICAL_LTC_MASK_PARAMS {
    /*!
     * [in]: Physical FB partition index.
     */
    NvU32 fbpIndex;
    /*!
     * [in]: swizzId
     * PartitionID associated with a created smc partition.
     */
    NvU32 swizzId;
    /*!
     * [out]: logical ltc mask.
     */
    NV_DECLARE_ALIGNED(NvU64 logicalLtcEnMask, 8);
} NV2080_CTRL_FB_FS_INFO_PROFILER_MON_LOGICAL_LTC_MASK_PARAMS;

// Possible values for queryType
#define NV2080_CTRL_FB_FS_INFO_INVALID_QUERY                 0x0U
#define NV2080_CTRL_FB_FS_INFO_FBP_MASK                      0x1U
#define NV2080_CTRL_FB_FS_INFO_LTC_MASK                      0x2U
#define NV2080_CTRL_FB_FS_INFO_LTS_MASK                      0x3U
#define NV2080_CTRL_FB_FS_INFO_FBPA_MASK                     0x4U
#define NV2080_CTRL_FB_FS_INFO_ROP_MASK                      0x5U
#define NV2080_CTRL_FB_FS_INFO_PROFILER_MON_LTC_MASK         0x6U
#define NV2080_CTRL_FB_FS_INFO_PROFILER_MON_LTS_MASK         0x7U
#define NV2080_CTRL_FB_FS_INFO_PROFILER_MON_FBPA_MASK        0x8U
#define NV2080_CTRL_FB_FS_INFO_PROFILER_MON_ROP_MASK         0x9U
#define NV2080_CTRL_FB_FS_INFO_FBPA_SUBP_MASK                0xAU
#define NV2080_CTRL_FB_FS_INFO_PROFILER_MON_FBPA_SUBP_MASK   0xBU
#define NV2080_CTRL_FB_FS_INFO_FBP_LOGICAL_MAP               0xCU
#define NV2080_CTRL_SYSL2_FS_INFO_SYSLTC_MASK                0xDU
#define NV2080_CTRL_FB_FS_INFO_PAC_MASK                      0xEU
#define NV2080_CTRL_FB_FS_INFO_LOGICAL_LTC_MASK              0xFU
#define NV2080_CTRL_FB_FS_INFO_PROFILER_MON_LOGICAL_LTC_MASK 0x10U

typedef struct NV2080_CTRL_FB_FS_INFO_QUERY {
    NvU16 queryType;
    NvU8  reserved[2];
    NvU32 status;
    union {
        NV2080_CTRL_FB_FS_INFO_INVALID_QUERY_PARAMS          inv;
        NV_DECLARE_ALIGNED(NV2080_CTRL_FB_FS_INFO_FBP_MASK_PARAMS fbp, 8);
        NV2080_CTRL_FB_FS_INFO_LTC_MASK_PARAMS               ltc;
        NV2080_CTRL_FB_FS_INFO_LTS_MASK_PARAMS               lts;
        NV2080_CTRL_FB_FS_INFO_FBPA_MASK_PARAMS              fbpa;
        NV2080_CTRL_FB_FS_INFO_ROP_MASK_PARAMS               rop;
        NV2080_CTRL_FB_FS_INFO_PROFILER_MON_LTC_MASK_PARAMS  dmLtc;
        NV2080_CTRL_FB_FS_INFO_PROFILER_MON_LTS_MASK_PARAMS  dmLts;
        NV2080_CTRL_FB_FS_INFO_PROFILER_MON_FBPA_MASK_PARAMS dmFbpa;
        NV2080_CTRL_FB_FS_INFO_PROFILER_MON_ROP_MASK_PARAMS  dmRop;
        NV_DECLARE_ALIGNED(NV2080_CTRL_FB_FS_INFO_PROFILER_MON_FBPA_SUBP_MASK_PARAMS dmFbpaSubp, 8);
        NV2080_CTRL_FB_FS_INFO_FBPA_SUBP_MASK_PARAMS         fbpaSubp;
        NV2080_CTRL_FB_FS_INFO_FBP_LOGICAL_MAP_PARAMS        fbpLogicalMap;
        NV2080_CTRL_SYSL2_FS_INFO_SYSLTC_MASK_PARAMS         sysl2Ltc;
        NV2080_CTRL_FB_FS_INFO_PAC_MASK_PARAMS               pac;
        NV_DECLARE_ALIGNED(NV2080_CTRL_FB_FS_INFO_LOGICAL_LTC_MASK_PARAMS logicalLtc, 8);
        NV_DECLARE_ALIGNED(NV2080_CTRL_FB_FS_INFO_PROFILER_MON_LOGICAL_LTC_MASK_PARAMS dmLogicalLtc, 8);
    } queryParams;
} NV2080_CTRL_FB_FS_INFO_QUERY;

// Max number of queries that can be batched in a single call to NV2080_CTRL_CMD_FB_GET_FS_INFO
#define NV2080_CTRL_FB_FS_INFO_MAX_QUERIES 120U

#define NV2080_CTRL_FB_GET_FS_INFO_PARAMS_MESSAGE_ID (0x46U)

typedef struct NV2080_CTRL_FB_GET_FS_INFO_PARAMS {
    NvU16 numQueries;
    NvU8  reserved[6];
    NV_DECLARE_ALIGNED(NV2080_CTRL_FB_FS_INFO_QUERY queries[NV2080_CTRL_FB_FS_INFO_MAX_QUERIES], 8);
} NV2080_CTRL_FB_GET_FS_INFO_PARAMS;

/*!
 * NV2080_CTRL_CMD_FB_GET_FS_INFO
 *
 * This control call returns the fb engine information for a partition/GPU.
 * Supports an interface so that the caller can issue multiple queries by batching them
 * in a single call. Returns the first error it encounters.
 *
 * numQueries[IN]
 *     - Specifies the number of valid queries.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_FB_GET_FS_INFO                             (0x20801346U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FB_INTERFACE_ID << 8) | NV2080_CTRL_FB_GET_FS_INFO_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_FB_HISTOGRAM_IDX_NO_REMAPPED_ROWS              (0x0U)
#define NV2080_CTRL_FB_HISTOGRAM_IDX_SINGLE_REMAPPED_ROW           (0x1U)
#define NV2080_CTRL_FB_HISTOGRAM_IDX_MIXED_REMAPPED_REMAINING_ROWS (0x2U)
#define NV2080_CTRL_FB_HISTOGRAM_IDX_SINGLE_REMAINING_ROW          (0x3U)
#define NV2080_CTRL_FB_HISTOGRAM_IDX_MAX_REMAPPED_ROWS             (0x4U)

#define NV2080_CTRL_FB_GET_ROW_REMAPPER_HISTOGRAM_PARAMS_MESSAGE_ID (0x47U)

typedef struct NV2080_CTRL_FB_GET_ROW_REMAPPER_HISTOGRAM_PARAMS {
    NvU32 histogram[5];
} NV2080_CTRL_FB_GET_ROW_REMAPPER_HISTOGRAM_PARAMS;

/*!
 * NV2080_CTRL_CMD_FB_GET_ROW_REMAPPER_HISTOGRAM
 *
 * This control call returns stats on the number of banks that have a certain
 * number of rows remapped in the bank. Specifically the number of banks that
 * have 0, 1, 2 through (max-2), max-1 and max number of rows remapped in the
 * bank. Values will be returned in an array.
 *
 * Index values are:
 *
 *   NV2080_CTRL_FB_HISTOGRAM_IDX_NO_REMAPPED_ROWS
 *     Number of banks with zero rows remapped
     NV2080_CTRL_FB_HISTOGRAM_IDX_SINGLE_REMAPPED_ROW
 *     Number of banks with one row remapped
     NV2080_CTRL_FB_HISTOGRAM_IDX_MIXED_REMAPPED_REMAINING_ROWS
 *     Number of banks with 2 through (max-2) rows remapped
     NV2080_CTRL_FB_HISTOGRAM_IDX_SINGLE_REMAINING_ROW
 *     Number of banks with (max-1) rows remapped
     NV2080_CTRL_FB_HISTOGRAM_IDX_MAX_REMAPPED_ROWS
 *     Number of banks with max rows remapped
 *
 *   Possible status values returned are:
 *     NV_OK
 *     NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_FB_GET_ROW_REMAPPER_HISTOGRAM (0x20801347U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FB_INTERFACE_ID << 8) | NV2080_CTRL_FB_GET_ROW_REMAPPER_HISTOGRAM_PARAMS_MESSAGE_ID" */

/*
 * NV2080_CTRL_CMD_FB_GET_DYNAMICALLY_BLACKLISTED_PAGES
 *
 * This command returns the list of dynamically blacklisted video memory page addresses
 * after last driver load.
 *
 *   blackList
 *     This output parameter is an array of NV2080_CTRL_FB_DYNAMIC_BLACKLIST_ADDRESS_INFO
 *     This array can hold a maximum of NV2080_CTRL_FB_DYNAMIC_BLACKLIST_MAX_ENTRIES.
 *   validEntries
 *     This output parameter specifies the number of valid entries in the
 *     blackList array.
 *   baseIndex
 *     With the limit of up to 512 blacklisted pages, the size of this array
 *     exceeds the rpc buffer limit. This control call will collect the data
 *     in multiple passes. This parameter indicates the start index of the
 *     data to be passed back to the caller
 *     This cannot be greater than NV2080_CTRL_FB_DYNAMIC_BLACKLIST_MAX_PAGES
 *   bMore
 *     This parameter indicates whether there are more valid elements to be
 *     fetched.
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NV2080_CTRL_CMD_FB_GET_DYNAMIC_OFFLINED_PAGES (0x20801348U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FB_INTERFACE_ID << 8) | NV2080_CTRL_FB_GET_DYNAMIC_OFFLINED_PAGES_PARAMS_MESSAGE_ID" */

/* Maximum pages that can be dynamically blacklisted */
#define NV2080_CTRL_FB_DYNAMIC_BLACKLIST_MAX_PAGES    512U

/*
 * Maximum entries that can be sent in a single pass of 
 * NV2080_CTRL_CMD_FB_GET_DYNAMIC_OFFLINED_PAGES
 */
#define NV2080_CTRL_FB_DYNAMIC_BLACKLIST_MAX_ENTRIES  64U

/**
 * NV2080_CTRL_FB_DYNAMIC_OFFLINED_ADDRESS_INFO
 *   pageNumber
 *     This output parameter specifies the dynamically blacklisted page number.
 *   source
 *     The reason for the page to be retired. Valid values for
 *     this parameter include:
 *        NV2080_CTRL_FB_OFFLINED_PAGES_SOURCE_INVALID
 *           Invalid source.
 *        NV2080_CTRL_FB_OFFLINED_PAGES_SOURCE_DPR_DBE
 *           Page retired by dynamic page retirement due to a double bit
 *           error seen.
 */
typedef struct NV2080_CTRL_FB_DYNAMIC_OFFLINED_ADDRESS_INFO {
    NV_DECLARE_ALIGNED(NvU64 pageNumber, 8);
    NvU8 source;
} NV2080_CTRL_FB_DYNAMIC_OFFLINED_ADDRESS_INFO;

#define NV2080_CTRL_FB_GET_DYNAMIC_OFFLINED_PAGES_PARAMS_MESSAGE_ID (0x48U)

typedef struct NV2080_CTRL_FB_GET_DYNAMIC_OFFLINED_PAGES_PARAMS {
    NV_DECLARE_ALIGNED(NV2080_CTRL_FB_DYNAMIC_OFFLINED_ADDRESS_INFO offlined[NV2080_CTRL_FB_DYNAMIC_BLACKLIST_MAX_ENTRIES], 8);
    NvU32  validEntries;
    NvU32  baseIndex;
    NvBool bMore;
} NV2080_CTRL_FB_GET_DYNAMIC_OFFLINED_PAGES_PARAMS;

/* valid values for source */

#define NV2080_CTRL_FB_DYNAMIC_BLACKLISTED_PAGES_SOURCE_INVALID (0x00000000U)
#define NV2080_CTRL_FB_DYNAMIC_BLACKLISTED_PAGES_SOURCE_DPR_DBE (0x00000001U)

/*
 * NV2080_CTRL_CMD_FB_GET_CLIENT_ALLOCATION_INFO
 *
 * This control command is used by clients to query information pertaining to client allocations.
 *
 *
 * @params [IN/OUT] NvU64 allocCount:
 *        Client specifies the allocation count that it received using the
 *        previous NV2080_CTRL_CMD_FB_GET_CLIENT_ALLOCATION_INFO control call.
 *        RM will get the total number of allocations known by RM and fill
 *        allocCount with it.
 *
 * @params [IN] NvP64 pAllocInfo:
 *        Pointer to the buffer allocated by client of size NV2080_CTRL_CMD_FB_ALLOCATION_INFO *
 *        allocCount. RM returns the info pertaining to each  of the contiguous client
 *        allocation chunks in pAllocInfo. The format of the allocation information is given by
 *        NV2080_CTRL_CMD_FB_ALLOCATION_INFO. The client has to sort the returned information if
 *        it wants to retain the legacy behavior of SORTED BY OFFSET. Information is only returned
 *        if and only if allocCount[IN]>=allocCount[OUT] and clientCount[IN]>=clientCount[OUT].
 *
 * @params [IN/OUT] NvP64 clientCount:
 *        Client specifies the client count that it received using the
 *        previous NV2080_CTRL_CMD_FB_GET_CLIENT_ALLOCATION_INFO control call.
 *        RM will get the total number of clients that have allocations with RM
 *        and fill clientCount with it.
 *
 * @params [IN] NvP64 pClientInfo:
 *        Pointer to the buffer allocated by client of size NV2080_CTRL_CMD_FB_CLIENT_INFO *
 *        clientCount. RM returns the info pertaining to each of the clients that have allocations
 *        known about by RM in pClientInfo. The format of the allocation information is given by
 *        NV2080_CTRL_CMD_FB_CLIENT_INFO. Information is only returned if and only if
 *        allocCount[IN]>=allocCount[OUT] and clientCount[IN]>=clientCount[OUT].
 *
 * @returns Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_POINTER
 *   NV_ERR_NO_MEMORY
 *
 * @Usage: All privileged RM clients for debugging only. Initially, call this with allocCount =
 *         clientCount = 0 to get client count, and then call again with allocated memory and sizes.
 *         Client can repeat with the new count-sized allocations until a maximum try count is
 *         reached or client is out of memory.
 */

#define NV2080_CTRL_CMD_FB_GET_CLIENT_ALLOCATION_INFO           (0x20801349U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FB_INTERFACE_ID << 8) | NV2080_CTRL_CMD_FB_GET_CLIENT_ALLOCATION_INFO_PARAMS_MESSAGE_ID" */

/*
 * These work with the FLD_SET_REF_NUM and FLD_TEST_REF macros and describe the 'flags' member
 * of the NV2080_CTRL_CMD_FB_ALLOCATION_INFO struct.
 */

// Address space of the allocation
#define NV2080_CTRL_CMD_FB_ALLOCATION_FLAGS_TYPE 4:0
#define NV2080_CTRL_CMD_FB_ALLOCATION_FLAGS_TYPE_SYSMEM         0U
#define NV2080_CTRL_CMD_FB_ALLOCATION_FLAGS_TYPE_VIDMEM         1U

// Whether the allocation is shared
#define NV2080_CTRL_CMD_FB_ALLOCATION_FLAGS_SHARED 5:5
#define NV2080_CTRL_CMD_FB_ALLOCATION_FLAGS_SHARED_FALSE        0U
#define NV2080_CTRL_CMD_FB_ALLOCATION_FLAGS_SHARED_TRUE         1U

// Whether this client owns this allocation
#define NV2080_CTRL_CMD_FB_ALLOCATION_FLAGS_OWNER 6:6
#define NV2080_CTRL_CMD_FB_ALLOCATION_FLAGS_OWNER_FALSE         0U
#define NV2080_CTRL_CMD_FB_ALLOCATION_FLAGS_OWNER_TRUE          1U

typedef struct NV2080_CTRL_CMD_FB_ALLOCATION_INFO {
    NvU32 client;                        /* [OUT] Identifies the client that made or shares the allocation (index into pClientInfo)*/
    NvU32 flags;                         /* [OUT] Flags associated with the allocation (see previous defines) */
    NV_DECLARE_ALIGNED(NvU64 beginAddr, 8);   /* [OUT] Starting physical address of the chunk */
    NV_DECLARE_ALIGNED(NvU64 size, 8);   /* [OUT] Size of the allocated contiguous chunk in bytes */
} NV2080_CTRL_CMD_FB_ALLOCATION_INFO;

typedef struct NV2080_CTRL_CMD_FB_CLIENT_INFO {
    NvHandle handle;                                    /* [OUT] Handle of the client that made or shares the allocation */
    NvU32    pid;                                       /* [OUT] PID of the client that made or shares the allocation */

    /* For the definition of the subprocessID and subprocessName params, see NV0000_CTRL_CMD_SET_SUB_PROCESS_ID */
    NvU32    subProcessID;                              /* [OUT] Subprocess ID of the client that made or shares the allocation */
    char     subProcessName[NV_PROC_NAME_MAX_LENGTH];   /* [OUT] Subprocess Name of the client that made or shares the allocation */
} NV2080_CTRL_CMD_FB_CLIENT_INFO;

#define NV2080_CTRL_CMD_FB_GET_CLIENT_ALLOCATION_INFO_PARAMS_MESSAGE_ID (0x49U)

typedef struct NV2080_CTRL_CMD_FB_GET_CLIENT_ALLOCATION_INFO_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 allocCount, 8);
    NV_DECLARE_ALIGNED(NvP64 pAllocInfo, 8);
    NV_DECLARE_ALIGNED(NvU64 clientCount, 8);
    NV_DECLARE_ALIGNED(NvP64 pClientInfo, 8);
} NV2080_CTRL_CMD_FB_GET_CLIENT_ALLOCATION_INFO_PARAMS;

/*
 * NV2080_CTRL_CMD_FB_UPDATE_NUMA_STATUS
 *
 * This control command is used by clients to update the NUMA status.
 *
 * @params [IN] NvBool bOnline:
 *
 * @returns Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_STATE
 *
 */
#define NV2080_CTRL_CMD_FB_UPDATE_NUMA_STATUS (0x20801350U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FB_INTERFACE_ID << 8) | NV2080_CTRL_FB_UPDATE_NUMA_STATUS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_FB_UPDATE_NUMA_STATUS_PARAMS_MESSAGE_ID (0x50U)

typedef struct NV2080_CTRL_FB_UPDATE_NUMA_STATUS_PARAMS {
    NvBool bOnline;
} NV2080_CTRL_FB_UPDATE_NUMA_STATUS_PARAMS;

/*
 * NV2080_CTRL_CMD_FB_GET_NUMA_INFO
 *
 * This control command is used by clients to get per-subdevice or
 * subscribed MIG partition(when MIG is enabled) NUMA memory information as
 * assigned by the system.
 *
 * numaNodeId[OUT]
 *     - Specifies the NUMA node ID.
 *
 * numaMemAddr[OUT]
 *     - Specifies the NUMA memory address.
 *
 * numaMemSize[OUT]
 *     - Specifies the NUMA memory size.
 *
 * numaOfflineAddressesCount[IN/OUT]
 *     - If non-zero, then it specifies the maximum number of entries in
 *       numaOfflineAddresses[] for which the information is required.
 *       It will be updated with the actual number of entries present in
 *       the numaOfflineAddresses[].
 *
 * numaOfflineAddresses[OUT]
 *      - If numaOfflineAddressesCount is non-zero, it contains the addresses
 *        of offline pages in the NUMA region.
 *
 * @returns Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV2080_CTRL_CMD_FB_GET_NUMA_INFO               (0x20801351U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FB_INTERFACE_ID << 8) | NV2080_CTRL_FB_GET_NUMA_INFO_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_FB_NUMA_INFO_MAX_OFFLINE_ADDRESSES 64U

#define NV2080_CTRL_FB_GET_NUMA_INFO_PARAMS_MESSAGE_ID (0x51U)

typedef struct NV2080_CTRL_FB_GET_NUMA_INFO_PARAMS {
    NvS32 numaNodeId;
    NV_DECLARE_ALIGNED(NvU64 numaMemAddr, 8);
    NV_DECLARE_ALIGNED(NvU64 numaMemSize, 8);
    NvU32 numaOfflineAddressesCount;
    NV_DECLARE_ALIGNED(NvU64 numaOfflineAddresses[NV2080_CTRL_FB_NUMA_INFO_MAX_OFFLINE_ADDRESSES], 8);
} NV2080_CTRL_FB_GET_NUMA_INFO_PARAMS;

/*
 * NV2080_CTRL_CMD_FB_GET_SEMAPHORE_SURFACE_LAYOUT
 *
 * This control command is used by clients to get NV_SEMAPHORE_SURFACE layout/caps before allocation.
 * A semaphore surface can be viewed as an array of independent semaphore entries.
 *
 * maxSubmittedSemaphoreValueOffset[OUT]
 *     - An offset of the max submitted value, relative to the semaphore surface entry start, if used.
 *       Used to emulate 64-bit semaphore values on chips where 64-bit semaphores are not supported.
 *
 * monitoredFenceThresholdOffset[OUT]
 *     - An offset of the monitored fence memory, relative to the semaphore surface entry start, if supported.
 *
 * size[OUT]
 *     - A size of a single semaphore surface entry.
 *
 * caps[OUT]
 *     - A mask of NV2080_CTRL_FB_GET_SEMAPHORE_SURFACE_LAYOUT_CAPS_* values.
 */
#define NV2080_CTRL_CMD_FB_GET_SEMAPHORE_SURFACE_LAYOUT                             (0x20801352U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FB_INTERFACE_ID << 8) | NV2080_CTRL_FB_GET_SEMAPHORE_SURFACE_LAYOUT_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_FB_GET_SEMAPHORE_SURFACE_LAYOUT_CAPS_MONITORED_FENCE_SUPPORTED  (0x00000001U)
#define NV2080_CTRL_FB_GET_SEMAPHORE_SURFACE_LAYOUT_CAPS_64BIT_SEMAPHORES_SUPPORTED (0x00000002U)

#define NV2080_CTRL_FB_GET_SEMAPHORE_SURFACE_LAYOUT_PARAMS_MESSAGE_ID (0x52U)

typedef struct NV2080_CTRL_FB_GET_SEMAPHORE_SURFACE_LAYOUT_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 maxSubmittedSemaphoreValueOffset, 8);
    NV_DECLARE_ALIGNED(NvU64 monitoredFenceThresholdOffset, 8);
    NV_DECLARE_ALIGNED(NvU64 size, 8);
    NvU32 caps;
} NV2080_CTRL_FB_GET_SEMAPHORE_SURFACE_LAYOUT_PARAMS;

typedef struct NV2080_CTRL_CMD_FB_STATS_ENTRY {
    //! Total physical memory available (accounts row-remapping)
    NV_DECLARE_ALIGNED(NvU64 totalSize, 8);

    //! Total reserved memory (includes both Region 1 and region 2)
    NV_DECLARE_ALIGNED(NvU64 rsvdSize, 8);

    //! Total usable memory (Region 0) for OS/KMD
    NV_DECLARE_ALIGNED(NvU64 osSize, 8);

    //! Region 1 (RM Internal) memory
    NV_DECLARE_ALIGNED(NvU64 r1Size, 8);

    //! Region 2 (Reserved) memory
    NV_DECLARE_ALIGNED(NvU64 r2Size, 8);

    //! Free memory (reserved but not allocated)
    NV_DECLARE_ALIGNED(NvU64 freeSize, 8);
} NV2080_CTRL_CMD_FB_STATS_ENTRY;

/*
 * NV2080_CTRL_CMD_GMMU_COMMIT_TLB_INVALIDATE
 *
 * This control command is used by clients to commit TLB invalidates
 *
 * gfid[OUT]
 *     - Specifices GPU function ID.
 *
 * invalidateAll[OUT]
 *     - Specifies whether to invalidate all using boolean
 *
 * @returns Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 *
 */
#define NV2080_CTRL_CMD_GMMU_COMMIT_TLB_INVALIDATE (0x20801353U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FB_INTERFACE_ID << 8) | NV2080_CTRL_GMMU_COMMIT_TLB_INVALIDATE_PARAMS_MESSAGE_ID" */
#define NV2080_CTRL_GMMU_COMMIT_TLB_INVALIDATE_PARAMS_MESSAGE_ID (0x53U)

typedef struct NV2080_CTRL_GMMU_COMMIT_TLB_INVALIDATE_PARAMS {
    NvU32  gfid;
    NvBool invalidateAll;
} NV2080_CTRL_GMMU_COMMIT_TLB_INVALIDATE_PARAMS;

typedef struct NV2080_CTRL_CMD_FB_STATS_OWNER_INFO {
    //! Total allocated size for this owner
    NV_DECLARE_ALIGNED(NvU64 allocSize, 8);

    //! Total memory blocks belonging this owner
    NvU32 numBlocks;

    //! Total reserved size for this owner
    NV_DECLARE_ALIGNED(NvU64 rsvdSize, 8);
} NV2080_CTRL_CMD_FB_STATS_OWNER_INFO;

#define NV2080_CTRL_CMD_FB_STATS_MAX_OWNER 200U

/*
 * NV2080_CTRL_CMD_FB_STATS_GET
 *
 * Get the FB allocations info.
 */
#define NV2080_CTRL_CMD_FB_STATS_GET       (0x2080132a) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FB_INTERFACE_ID << 8) | NV2080_CTRL_CMD_FB_STATS_GET_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CMD_FB_STATS_GET_PARAMS_MESSAGE_ID (0x2AU)

typedef struct NV2080_CTRL_CMD_FB_STATS_GET_PARAMS {

    //! Version id for driver and tool matching
    NV_DECLARE_ALIGNED(NvU64 version, 8);

    //! All sizes info
    NV_DECLARE_ALIGNED(NV2080_CTRL_CMD_FB_STATS_ENTRY fbSizeInfo, 8);

    //! Level 2 owner info table
    NV_DECLARE_ALIGNED(NV2080_CTRL_CMD_FB_STATS_OWNER_INFO fbBlockInfo[NV2080_CTRL_CMD_FB_STATS_MAX_OWNER], 8);
} NV2080_CTRL_CMD_FB_STATS_GET_PARAMS;

/* _ctrl2080fb_h_ */
