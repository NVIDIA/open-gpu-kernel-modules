/*
 * SPDX-FileCopyrightText: Copyright (c) 2009-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrl208f/ctrl208ffb.finn
//

#include "ctrl/ctrl208f/ctrl208fbase.h"

/*
 * NV208F_CTRL_CMD_FB_GET_INFO
 *
 * This command returns fb engine information for the associated GPU.
 * The client specifies what information to query through 'index' param.
 * On success, the information is stored in the 'data' param.
 *
 *   index
 *     Specify what information to query. Please see below for valid values of
 *     indexes for this command.
 *   data
 *     On success, this param will hold the data that the client queried for.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV208F_CTRL_CMD_FB_GET_INFO (0x208f0501) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_DIAG_FB_INTERFACE_ID << 8) | NV208F_CTRL_FB_GET_INFO_PARAMS_MESSAGE_ID" */

#define NV208F_CTRL_FB_GET_INFO_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV208F_CTRL_FB_GET_INFO_PARAMS {
    NvU32 index;
    NV_DECLARE_ALIGNED(NvU64 data, 8);
} NV208F_CTRL_FB_GET_INFO_PARAMS;

/* valid fb info index values */
#define NV208F_CTRL_FB_INFO_INDEX_FREE_CONTIG_COMPRESSION_SIZE (0x00000001)

#define NV208F_CTRL_FB_INFO_INDEX_MAX                          NV208F_CTRL_FB_INFO_INDEX_FREE_CONTIG_COMPRESSION_SIZE

/*
 * NV208F_CTRL_CMD_FB_GET_ZBC_REFCOUNT
 *
 * This command gets the ZBC reference count associated with a given
 * compression tag address.  It is not supported on GPUs which support class
 * GF100_ZBC_CLEAR as it is specific to a different hardware implementation.
 * 
 *   compTagAddress
 *     The input parameter indicating the compression tag address for which the
 *     associated ZBC refcount should be looked up.
 *   zbcRefCount
 *     An array of reference counts for the ZBC clear values associated with
 *     compTagAddress.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV208F_CTRL_CMD_FB_GET_ZBC_REFCOUNT                    (0x208f0505) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_DIAG_FB_INTERFACE_ID << 8) | 0x5" */ // Deprecated, removed form RM

#define NV208F_CTRL_FB_GET_ZBC_REFCOUNT_MAX_REFCOUNTS          2
typedef struct NV208F_CTRL_FB_GET_ZBC_REFCOUNT_PARAMS {
    NvU32 compTagAddress;
    NvU32 zbcRefCount[NV208F_CTRL_FB_GET_ZBC_REFCOUNT_MAX_REFCOUNTS];
} NV208F_CTRL_FB_GET_ZBC_REFCOUNT_PARAMS;

/*
 * NV208F_CTRL_CMD_FB_CTRL_GPU_CACHE
 *
 * This command controls the state of a cache which all GPU memory accesses go
 * through.  If supported, it allows changing of the power state and the write
 * mode.  This is only supported when NV_VERIF_FEATURES is defined.  An error
 * will be returned if the requested combination of settings is not possible.
 *
 *   writeMode
 *     Specifies the write mode of the cache.  Possible values are defined in
 *     NV208F_CTRL_FB_CTRL_GPU_CACHE_WRITE_MODE. Passing _DEFAULT means to
 *     maintain the current write mode.  It is illegal to change the write mode
 *     while the cache is disabled or in the same call as a request to disable
 *     it.
 *   powerState
 *     Specifies the power state of the cache.  Possible values are defined in
 *     NV208F_CTRL_FB_CTRL_GPU_CACHE_POWER_STATE.  Passing _DEFAULT means
 *     to maintain the current power state.
 *   rcmState
 *     Specifies the reduced cache mode of the cache.  Possible values are
 *     defined in NV208F_CTRL_FB_CTRL_GPU_CACHE_RCM_STATE.  Passing _DEFAULT
 *     means to maintain the current RCM state.
 *   vgaCacheMode
 *     Specifies whether or not to enable VGA out-of-cache mode.  Possible
 *     values are defined in NV208F_CTRL_FB_CTRL_GPU_CACHE_VGA_MODE.  Passing
 *     _DEFAULT means to maintain the current VGA caching mode.  
 *   cacheReset
 *     Triggers a hardware reset of the cache.  Possible values are defined in
 *     NV208F_CTRL_FB_CTRL_GPU_CACHE_CACHE_RESET.  Passing _DEFAULT does
 *     nothing while passing _RESET clears all data in the cache.
 *   flags
 *     Contains flags to control the details of how transitions should be
 *     handled.  Possible values are defined in
 *     NV208F_CTRL_FB_CTRL_GPU_CACHE_FLAGS.  Passing _DEFAULT for any of
 *     the fields means to use the defaults specified by the Resource Manager.
 *     Note not all options are available for all transitions.  Flags that are
 *     set but not applicable will be silently ignored.
 *   bypassMode
 *     (Fermi only) Specifies the bypass mode of the L2 cache.  Normal GPU
 *     operation is _DISABLE.  For TEST ONLY, setting _ENABLE enables a debug
 *     mode where all transactions miss in L2 and no writes are combined,
 *     essentially disabling the caching feature of the L2 cache.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_INVALID_STATE
 *   NV_ERR_INVALID_ARGUMENT
 */

#define NV208F_CTRL_CMD_FB_CTRL_GPU_CACHE (0x208f0506) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_DIAG_FB_INTERFACE_ID << 8) | NV208F_CTRL_FB_CTRL_GPU_CACHE_PARAMS_MESSAGE_ID" */

#define NV208F_CTRL_FB_CTRL_GPU_CACHE_PARAMS_MESSAGE_ID (0x6U)

typedef struct NV208F_CTRL_FB_CTRL_GPU_CACHE_PARAMS {
    NvU32 writeMode;
    NvU32 powerState;
    NvU32 rcmState;
    NvU32 vgaCacheMode;
    NvU32 cacheReset;
    NvU32 flags;
    NvU32 bypassMode;
} NV208F_CTRL_FB_CTRL_GPU_CACHE_PARAMS;

/* valid values for writeMode */
#define NV208F_CTRL_FB_CTRL_GPU_CACHE_WRITE_MODE_DEFAULT      (0x00000000)
#define NV208F_CTRL_FB_CTRL_GPU_CACHE_WRITE_MODE_WRITETHROUGH (0x00000001)
#define NV208F_CTRL_FB_CTRL_GPU_CACHE_WRITE_MODE_WRITEBACK    (0x00000002)

/* valid values for powerState */
#define NV208F_CTRL_FB_CTRL_GPU_CACHE_POWER_STATE_DEFAULT     (0x00000000)
#define NV208F_CTRL_FB_CTRL_GPU_CACHE_POWER_STATE_ENABLED     (0x00000001)
#define NV208F_CTRL_FB_CTRL_GPU_CACHE_POWER_STATE_DISABLED    (0x00000002)

/* valid values for rcmState */
#define NV208F_CTRL_FB_CTRL_GPU_CACHE_RCM_STATE_DEFAULT       (0x00000000)
#define NV208F_CTRL_FB_CTRL_GPU_CACHE_RCM_STATE_FULL          (0x00000001)
#define NV208F_CTRL_FB_CTRL_GPU_CACHE_RCM_STATE_REDUCED       (0x00000002)
#define NV208F_CTRL_FB_CTRL_GPU_CACHE_RCM_STATE_ZERO_CACHE    (0x00000003)

/* valid values for vgaCacheMode */
#define NV208F_CTRL_FB_CTRL_GPU_CACHE_VGA_MODE_DEFAULT        (0x00000000)
#define NV208F_CTRL_FB_CTRL_GPU_CACHE_VGA_MODE_ENABLED        (0x00000001)
#define NV208F_CTRL_FB_CTRL_GPU_CACHE_VGA_MODE_DISABLED       (0x00000002)

/* valid values for cacheReset */
#define NV208F_CTRL_FB_CTRL_GPU_CACHE_CACHE_RESET_DEFAULT     (0x00000000)
#define NV208F_CTRL_FB_CTRL_GPU_CACHE_CACHE_RESET_RESET       (0x00000001)

/* valid fields and values for flags */
#define NV208F_CTRL_FB_CTRL_GPU_CACHE_FLAGS_MODE                   1:0
#define NV208F_CTRL_FB_CTRL_GPU_CACHE_FLAGS_MODE_DEFAULT      (0x00000000)
#define NV208F_CTRL_FB_CTRL_GPU_CACHE_FLAGS_MODE_RM           (0x00000001)
#define NV208F_CTRL_FB_CTRL_GPU_CACHE_FLAGS_MODE_PMU          (0x00000002)

/* valid values for bypassMode */
#define NV208F_CTRL_FB_CTRL_GPU_CACHE_BYPASS_MODE_DEFAULT     (0x00000000)
#define NV208F_CTRL_FB_CTRL_GPU_CACHE_BYPASS_MODE_DISABLED    (0x00000001)
#define NV208F_CTRL_FB_CTRL_GPU_CACHE_BYPASS_MODE_ENABLED     (0x00000002)



/*
 * NV208F_CTRL_CMD_FB_SET_STATE
 *
 * This command is used to put fb engine in a state requested by the caller. 
 * 
 *   state
 *     This parameter specifies the desired engine state:
 *       NV208F_CTRL_FB_SET_STATE_STOPPED
 *         This value stops/halts the fb engine.
 *       NV208F_CTRL_FB_SET_STATE_RESTART      
 *         This value restarts fb from a stopped state. 
 *   
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_STATE
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV208F_CTRL_CMD_FB_SET_STATE                          (0x208f0508) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_DIAG_FB_INTERFACE_ID << 8) | NV208F_CTRL_FB_SET_STATE_PARAMS_MESSAGE_ID" */

#define NV208F_CTRL_FB_SET_STATE_PARAMS_MESSAGE_ID (0x8U)

typedef struct NV208F_CTRL_FB_SET_STATE_PARAMS {
    NvU32 state;
} NV208F_CTRL_FB_SET_STATE_PARAMS;

/* valid values for state */
#define NV208F_CTRL_FB_SET_STATE_STOPPED  (0x00000000)
#define NV208F_CTRL_FB_SET_STATE_RESTART  (0x00000001)

/*
 * NV208F_CTRL_CMD_GPU_ECC_SCRUB_DIAG
 * 
 * This command reads all the settings internal to scrubbing (both asynchronous
 * and synchronous.
 *
 * Currently implemented: FB offset scrubber has completed, FB offset that scrubber
 * is completing to, whether or not asynchronous scrubbing is enabled.
 * 
 *   fbOffsetCompleted
 *      This is the offset into FB that the scrubber has completed up to at the
 *      time this function is invoked. Note that the scrubber is top-down. Therefore
 *      the memory that remains unscrubbed is from 0x0 to fbOffsetCompleted.
 *
 *   fbEndOffset
 *       This is the offset of the base of the last block that ECC asynchronous
 *       scrubber has been tasked to scrub.
 *
 *   bAsyncScru bDisabled
 *       This is NV_TRUE if asynchronous scrubbing is disabled and NV_FALSE if
 *       asynchronous scrubbing is enabled.
 *
 * Possible status return values are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV208F_CTRL_CMD_FB_ECC_SCRUB_DIAG (0x208f0509) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_DIAG_FB_INTERFACE_ID << 8) | NV208F_CTRL_CMD_FB_ECC_SCRUB_DIAG_PARAMS_MESSAGE_ID" */

#define NV208F_CTRL_CMD_FB_ECC_SCRUB_DIAG_PARAMS_MESSAGE_ID (0x9U)

typedef struct NV208F_CTRL_CMD_FB_ECC_SCRUB_DIAG_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 fbOffsetCompleted, 8);
    NV_DECLARE_ALIGNED(NvU64 fbEndOffset, 8);
    NvBool bAsyncScrubDisabled;
} NV208F_CTRL_CMD_FB_ECC_SCRUB_DIAG_PARAMS;

/*
 * NV208F_CTRL_CMD_GPU_ECC_ASYNCH_SCRUB_REGION
 *
 * This command launches the ECC scrubber in asynchronous mode. The scrubber, as
 * in normal operation, will continue to operate until all of FB (excluding
 * dedicated system memory) has been scrubbed. Like usual operation, scrubbing is
 * only done on silicon.
 *
 * Possible status return values are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV208F_CTRL_CMD_FB_ECC_ASYNC_SCRUB_REGION (0x208f050a) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_DIAG_FB_INTERFACE_ID << 8) | NV208F_CTRL_CMD_FB_ECC_ASYNC_SCRUB_REGION_PARAMS_MESSAGE_ID" */

#define NV208F_CTRL_CMD_FB_ECC_ASYNC_SCRUB_REGION_PARAMS_MESSAGE_ID (0xAU)

typedef struct NV208F_CTRL_CMD_FB_ECC_ASYNC_SCRUB_REGION_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 startBlock, 8);
    NV_DECLARE_ALIGNED(NvU64 endBlock, 8);
} NV208F_CTRL_CMD_FB_ECC_ASYNC_SCRUB_REGION_PARAMS;

/*
 * NV208F_CTRL_CMD_GPU_ECC_ERROR_INFO
 * This is a structure that is defined here for diag/debug purposes in mods.
 * It is used to return the error information as part of the callback to 
 * kernel clients registering for SBE/DBE callbacks.
 */

typedef struct NV208F_CTRL_CMD_FB_ECC_ERROR_INFO {
    NvU32 row;
    NvU32 bank;
    NvU32 col;
    NvU32 extBank;
    NvU32 xbarAddress;
    NV_DECLARE_ALIGNED(NvU64 physAddress, 8);
} NV208F_CTRL_CMD_FB_ECC_ERROR_INFO;


#define NV208F_CTRL_CMD_FB_ECC_GET_FORWARD_MAP_ADDRESS (0x208f050c) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_DIAG_FB_INTERFACE_ID << 8) | NV208F_CTRL_CMD_FB_ECC_GET_FORWARD_MAP_ADDRESS_PARAMS_MESSAGE_ID" */

#define NV208F_CTRL_CMD_FB_ECC_GET_FORWARD_MAP_ADDRESS_PARAMS_MESSAGE_ID (0xCU)

typedef struct NV208F_CTRL_CMD_FB_ECC_GET_FORWARD_MAP_ADDRESS_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 pAddr, 8);
    NvU32 row;
    NvU32 bank;
    NvU32 col;
    NvU32 extBank;
    NvU32 rank;
    NvU32 physicalSublocation;
    NvU32 physicalPartition;
    NvU32 logicalSublocation;
    NvU32 logicalPartition;
    NvU32 writeKillPtr0;
    NvU32 injectionAddr;
    NvU32 injectionAddrExt;
    NvU32 rbcAddress;
    NvU32 rbcAddressExt;
    NvU32 rbcAddressExt2;
    NvU32 eccCol;
} NV208F_CTRL_CMD_FB_ECC_GET_FORWARD_MAP_ADDRESS_PARAMS;

/*
 * NV208F_CTRL_CMD_FB_ECC_SET_KILL_PTR
 *
 * This command sets the kill pointer for the specified DRAM address.  If
 * the kill pointer is set to NV208F_CTRL_FB_ERROR_TYPE_CORRECTABLE
 * or NV208F_CTRL_FB_ERROR_TYPE_UNCORRECTABLE, accesses to the specified
 * address will result in ECC errors until all kill pointers are unset
 * using NV208F_CTRL_FB_ERROR_TYPE_NONE.
 *
 * Only one kill pointer can be set at a time.  Setting a kill pointer will
 * clear all currently set kill pointers and set the new kill pointer.
 * Calling NV208F_CTRL_FB_ERROR_TYPE_NONE simply clears all currently set
 * kill pointers.
 *
 *   errorType
 *      The type of kill pointer to set.  NV208F_CTRL_FB_ERROR_TYPE_CORRECTABLE
 *      will set a single kill pointer resulting in a correctable error.
 *      NV208F_CTRL_FB_ERROR_TYPE_UNCORRECTABLE will set both kill pointers
 *      resulting in an uncorrectable error.  NV208F_CTRL_FB_ERROR_TYPE_NONE
 *      will clear all kill pointers, which stops the associated addresses
 *      from generating ECC errors if NV208F_CTRL_FB_ERROR_TYPE_CORRECTABLE
 *      or NV208F_CTRL_FB_ERROR_TYPE_UNCORRECTABLE was previously set.
 *      Only one kill pointer can be set at a time and setting a new
 *      kill pointer will clear the previous kill pointer.
 *
 *    address
 *      The physical DRAM address to be targeted by the kill pointer
 *
 *    bProdInjection
 *      Whether the kill pointer is set through the production injection flow or not
 */
#define NV208F_CTRL_CMD_FB_ECC_SET_KILL_PTR (0x208f050e) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_DIAG_FB_INTERFACE_ID << 8) | NV208F_CTRL_FB_ECC_SET_KILL_PTR_PARAMS_MESSAGE_ID" */

typedef enum NV208F_CTRL_FB_ERROR_TYPE {
    NV208F_CTRL_FB_ERROR_TYPE_CORRECTABLE = 0,
    NV208F_CTRL_FB_ERROR_TYPE_UNCORRECTABLE = 1,
    NV208F_CTRL_FB_ERROR_TYPE_NONE = 2,
} NV208F_CTRL_FB_ERROR_TYPE;

#define NV208F_CTRL_FB_ECC_SET_KILL_PTR_PARAMS_MESSAGE_ID (0xEU)

typedef struct NV208F_CTRL_FB_ECC_SET_KILL_PTR_PARAMS {
    NV208F_CTRL_FB_ERROR_TYPE errorType;
    NV_DECLARE_ALIGNED(NvU64 address, 8);
    NvBool                    bProdInjection;
} NV208F_CTRL_FB_ECC_SET_KILL_PTR_PARAMS;



/*
 * NV208F_CTRL_CMD_FB_INJECT_LTC_ECC_ERROR
 *
 * This API allows a client to inject ECC errors in the L2.
 *
 *   ltc:
 *      The physical LTC number to inject the error into.
 *   slice:
 *      THe physical slice number within the LTC to inject the error into.
 *   locationMask
 *      LTC location subtype(s) where error is to be injected. (Valid on Ampere and later)
 *   errorType
 *      Type of error to inject
 *      NV208F_CTRL_FB_ERROR_TYPE_CORRECTABLE for SBE.
 *      NV208F_CTRL_FB_ERROR_TYPE_UNCORRECTABLE for DBE.
 *
 */
#define NV208F_CTRL_CMD_FB_INJECT_LTC_ECC_ERROR     (0x208f050f) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_DIAG_FB_INTERFACE_ID << 8) | NV208F_CTRL_FB_INJECT_LTC_ECC_ERROR_PARAMS_MESSAGE_ID" */

#define NV208F_CTRL_FB_INJECT_LTC_ECC_ERROR_PARAMS_MESSAGE_ID (0xFU)

typedef struct NV208F_CTRL_FB_INJECT_LTC_ECC_ERROR_PARAMS {
    NvU8                      ltc;
    NvU8                      slice;
    NvU8                      locationMask;
    NV208F_CTRL_FB_ERROR_TYPE errorType;
} NV208F_CTRL_FB_INJECT_LTC_ECC_ERROR_PARAMS;

/*
 * NV208F_CTRL_CMD_FB_ECC_INJECTION_SUPPORTED
 *
 * Reports if error injection is supported for a given HW unit
 *
 * location [in]:
 *      The ECC protected unit for which ECC injection support is being checked.
 *      The location type is defined by NV208F_CTRL_FB_ECC_INJECTION_SUPPORTED_LOC.
 *
 * bCorrectableSupported [out]:
 *      Boolean value that shows if correcatable errors can be injected.
 *
 * bUncorrectableSupported [out]:
 *      Boolean value that shows if uncorrecatable errors can be injected.
 *
 * Return values:
 *      NV_OK on success
 *      NV_ERR_INVALID_ARGUMENT if the requested location is invalid.
 *      NV_ERR_INSUFFICIENT_PERMISSIONS if priv write not enabled.
 *      NV_ERR_NOT_SUPPORTED otherwise
 *
 *
 */
#define NV208F_CTRL_CMD_FB_ECC_INJECTION_SUPPORTED (0x208f0510) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_DIAG_FB_INTERFACE_ID << 8) | NV208F_CTRL_FB_ECC_INJECTION_SUPPORTED_PARAMS_MESSAGE_ID" */

#define NV208F_CTRL_FB_ECC_INJECTION_SUPPORTED_PARAMS_MESSAGE_ID (0x10U)

typedef struct NV208F_CTRL_FB_ECC_INJECTION_SUPPORTED_PARAMS {
    NvU8   location;
    NvBool bCorrectableSupported;
    NvBool bUncorrectableSupported;
} NV208F_CTRL_FB_ECC_INJECTION_SUPPORTED_PARAMS;

#define NV208F_CTRL_FB_ECC_INJECTION_SUPPORTED_LOC          1:0
#define NV208F_CTRL_FB_ECC_INJECTION_SUPPORTED_LOC_LTC   (0x00000000)
#define NV208F_CTRL_FB_ECC_INJECTION_SUPPORTED_LOC_DRAM  (0x00000001)
#define NV208F_CTRL_FB_ECC_INJECTION_SUPPORTED_LOC_LRC   (0x00000002)
#define NV208F_CTRL_FB_ECC_INJECTION_SUPPORTED_LOC_SYSL2 (0x00000003)

/*
 * NV208F_CTRL_CMD_FB_ECC_SET_WRITE_KILL
 *
 * This command sets the write kill for the specified DRAM address.  If set,
 * writes to the specified address won't update the ECC checkbits.  When unset,
 * writes the specified address will update the ECC checkbits.
 *
 * Only one write kill register can be set at a time.  Setting a write kill
 * will clear all currently set write kills and set the new write kill.
 * Calling this ctrl call with setWriteKill = false simply clears all currently
 * set write kills.
 *
 *   setWriteKill
 *      When true, the ECC checkbits for the specified address won't update on
 *      writes. When false, the ECC checkbits for the specified address will
 *      revert to normal behavior and update on all writes.
 *
 *   address
 *      The physical DRAM address to be targeted by the write kill
 */
#define NV208F_CTRL_CMD_FB_ECC_SET_WRITE_KILL            (0x208f0511) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_DIAG_FB_INTERFACE_ID << 8) | NV208F_CTRL_FB_ECC_SET_WRITE_KILL_PARAMS_MESSAGE_ID" */

#define NV208F_CTRL_FB_ECC_SET_WRITE_KILL_PARAMS_MESSAGE_ID (0x11U)

typedef struct NV208F_CTRL_FB_ECC_SET_WRITE_KILL_PARAMS {
    NvBool setWriteKill;
    NV_DECLARE_ALIGNED(NvU64 address, 8);
} NV208F_CTRL_FB_ECC_SET_WRITE_KILL_PARAMS;

typedef struct NV208F_CTRL_FB_REMAPPING_RBC_ADDRESS_INFO {
    NvU32 bank;
    NvU32 stackId;
    NvU32 row;
    NvU32 partition;
    NvU32 sublocation;
} NV208F_CTRL_FB_REMAPPING_RBC_ADDRESS_INFO;

#define NV208F_CTRL_FB_REMAP_ROW_ADDRESS_TYPE_PHYSICAL 0x0
#define NV208F_CTRL_FB_REMAP_ROW_ADDRESS_TYPE_RBC      0x1

/*
 * NV208F_CTRL_FB_REMAPPING_ADDRESS_INFO
 *
 *   source
 *     The reason for retirement. Valid values for this parameter are
 *     from NV2080_CTRL_FB_REMAPPED_ROW_SOURCE_*
 *   status
 *     Reason for row remapping failure. Valid values are:
 *       NV208F_CTRL_FB_REMAP_ROW_STATUS_OK
 *         No error
 *       NV208F_CTRL_FB_REMAP_ROW_STATUS_REMAPPING_PENDING
 *         The remapping is pending
 *       NV208F_CTRL_FB_REMAP_ROW_STATUS_TABLE_FULL
 *         Table full
 *       NV208F_CTRL_FB_REMAP_ROW_STATUS_ALREADY_REMAPPED
 *         Attempting to remap a reserved row
 *       NV208F_CTRL_FB_REMAP_ROW_STATUS_INTERNAL_ERROR
 *         Some other RM failure
 *   addressType
 *     Type of address passed. Valid values are:
 *       NV208F_CTRL_FB_REMAP_ROW_ADDRESS_TYPE_PHYSICAL
 *         The specified address is physical address.
 *       NV208F_CTRL_FB_REMAP_ROW_ADDRESS_TYPE_RBC
 *         The specified address is DRAM Row Bank Column address.
 *   address
 *     Union of physicalAddress and rbcAddress. Set the appropriate one based on the address type.
 */
typedef struct NV208F_CTRL_FB_REMAPPING_ADDRESS_INFO {
    NvU8  source;
    NvU32 status;
    NvU8  addressType;
    union {
        NV_DECLARE_ALIGNED(NvU64 physicalAddress, 8);
        NV208F_CTRL_FB_REMAPPING_RBC_ADDRESS_INFO rbcAddress;
    } address;
} NV208F_CTRL_FB_REMAPPING_ADDRESS_INFO;

/* valid values for status */
#define NV208F_CTRL_FB_REMAP_ROW_STATUS_OK                (0x00000000)
#define NV208F_CTRL_FB_REMAP_ROW_STATUS_REMAPPING_PENDING (0x00000001)
#define NV208F_CTRL_FB_REMAP_ROW_STATUS_TABLE_FULL        (0x00000002)
#define NV208F_CTRL_FB_REMAP_ROW_STATUS_ALREADY_REMAPPED  (0x00000003)
#define NV208F_CTRL_FB_REMAP_ROW_STATUS_INTERNAL_ERROR    (0x00000004)

#define NV208F_CTRL_FB_REMAPPED_ROWS_MAX_ROWS             (0x00000200)
/*
 * NV208F_CTRL_CMD_FB_REMAP_ROW
 *
 * This command will write entries to Inforom. During init the entries will be
 * read and used to remap a row.
 *
 *   addressList
 *     This input parameter is an array of NV208F_CTRL_FB_REMAPPING_ADDRESS_INFO
 *     structures containing information used for row remapping. Valid entries
 *     are adjacent
 *   validEntries
 *     This input parameter specifies the number of valid entries in the
 *     address array
 *   numEntriesAdded
 *     This output parameter specifies how many validEntries were successfully
 *     added to the Inforom
 *
 *   Possible status values returned are:
 *     NV_OK
 *     NV_ERR_INVALID_ARGUMENT
 */
#define NV208F_CTRL_CMD_FB_REMAP_ROW                      (0x208f0512) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_DIAG_FB_INTERFACE_ID << 8) | NV208F_CTRL_FB_REMAP_ROW_PARAMS_MESSAGE_ID" */

#define NV208F_CTRL_FB_REMAP_ROW_PARAMS_MESSAGE_ID (0x12U)

typedef struct NV208F_CTRL_FB_REMAP_ROW_PARAMS {
    NV_DECLARE_ALIGNED(NV208F_CTRL_FB_REMAPPING_ADDRESS_INFO addressList[NV208F_CTRL_FB_REMAPPED_ROWS_MAX_ROWS], 8);
    NvU32 validEntries;
    NvU32 numEntriesAdded;
} NV208F_CTRL_FB_REMAP_ROW_PARAMS;


#define NV208F_CTRL_CMD_FB_REVERSE_MAP_RBC_ADDR_TO_PA (0x208f0513) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_DIAG_FB_INTERFACE_ID << 8) | NV208F_CTRL_FB_REVERSE_MAP_RBC_ADDR_TO_PA_PARAMS_MESSAGE_ID" */

#define NV208F_CTRL_FB_REVERSE_MAP_RBC_ADDR_TO_PA_PARAMS_MESSAGE_ID (0x13U)

typedef struct NV208F_CTRL_FB_REVERSE_MAP_RBC_ADDR_TO_PA_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 address, 8);
    NvU32 rbcAddress;
    NvU32 rbcAddressExt;
    NvU32 rbcAddressExt2;
    NvU32 partition;
    NvU32 sublocation;
} NV208F_CTRL_FB_REVERSE_MAP_RBC_ADDR_TO_PA_PARAMS;

/**
 * NV208F_CTRL_CMD_FB_TOGGLE_PHYSICAL_ADDRESS_ECC_ON_OFF
 *
 * This command will convert a physical address when ECC is on to the physical
 * address when ECC is off or vice versa
 *
 * @params[in] NvU64 inputAddress
 *     Input physical address
 *
 * @params[in] NvBool eccOn
 *     Whether or not input physical address is with ECC on or off
 *
 * @params[out] NvU64 outputAddress
 *     Output physical address
 */
#define NV208F_CTRL_CMD_FB_TOGGLE_PHYSICAL_ADDRESS_ECC_ON_OFF (0x208f0514) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_DIAG_FB_INTERFACE_ID << 8) | NV208F_CTRL_FB_TOGGLE_PHYSICAL_ADDRESS_ECC_ON_OFF_PARAMS_MESSAGE_ID" */

#define NV208F_CTRL_FB_TOGGLE_PHYSICAL_ADDRESS_ECC_ON_OFF_PARAMS_MESSAGE_ID (0x14U)

typedef struct NV208F_CTRL_FB_TOGGLE_PHYSICAL_ADDRESS_ECC_ON_OFF_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 inputAddress, 8);
    NvBool eccOn;
    NV_DECLARE_ALIGNED(NvU64 outputAddress, 8);
} NV208F_CTRL_FB_TOGGLE_PHYSICAL_ADDRESS_ECC_ON_OFF_PARAMS;

/*
 * NV208F_CTRL_CMD_FB_CLEAR_REMAPPED_ROWS
 *
 * This command clears remapping entries from the Inforom's row remapping table.
 *
 *   sourceMask
 *     This is a bit mask of NV2080_CTRL_FB_REMAPPED_ROW_SOURCE. Rows
 *     remapped from the specified sources will be cleared/removed from the
 *     Inforom RRL object entries list.
 *
 *   bForcePurge
 *     This flag will force purge the RRL object and associated data structures
 *
 *   Possbile status values returned are:
 *     NV_OK
 *     NV_ERR_NOT_SUPPORTED
 */
#define NV208F_CTRL_CMD_FB_CLEAR_REMAPPED_ROWS (0x208f0515) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_DIAG_FB_INTERFACE_ID << 8) | NV208F_CTRL_FB_CLEAR_REMAPPED_ROWS_PARAMS_MESSAGE_ID" */

#define NV208F_CTRL_FB_CLEAR_REMAPPED_ROWS_PARAMS_MESSAGE_ID (0x15U)

typedef struct NV208F_CTRL_FB_CLEAR_REMAPPED_ROWS_PARAMS {
    NvU32  sourceMask;
    NvBool bForcePurge;
} NV208F_CTRL_FB_CLEAR_REMAPPED_ROWS_PARAMS;



/*
 * NV208F_CTRL_CMD_FB_INJECT_LRC_ECC_ERROR
 *
 * This API allows a client to inject ECC errors in the LRC.
 *
 *   lrcc:
 *      The physical LRCC number to inject the error into.
 *   lrc:
 *      THe physical LRC number within the LRCC to inject the error into.
 *   locationMask
 *      LTC location subtype(s) where error is to be injected. (Valid on Ampere and later)
 *   errorType
 *      Type of error to inject
 *      NV208F_CTRL_FB_ERROR_TYPE_CORRECTABLE for SBE.
 *      NV208F_CTRL_FB_ERROR_TYPE_UNCORRECTABLE for DBE.
 *
 */
#define NV208F_CTRL_CMD_FB_INJECT_LRC_ECC_ERROR (0x208f0516) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_DIAG_FB_INTERFACE_ID << 8) | NV208F_CTRL_FB_INJECT_LRC_ECC_ERROR_PARAMS_MESSAGE_ID" */

#define NV208F_CTRL_FB_INJECT_LRC_ECC_ERROR_PARAMS_MESSAGE_ID (0x16U)

typedef struct NV208F_CTRL_FB_INJECT_LRC_ECC_ERROR_PARAMS {
    NvU8                      lrcc;
    NvU8                      lrc;
    NvU8                      locationMask;
    NV208F_CTRL_FB_ERROR_TYPE errorType;
} NV208F_CTRL_FB_INJECT_LRC_ECC_ERROR_PARAMS;

/*
 * NV208F_CTRL_CMD_FB_INJECT_SYSLTC_ECC_ERROR
 *
 * This API allows a client to inject ECC errors in the SYSLTC.
 *
 *   group:
 *      The physical group number to inject the error into.
 *   instance:
 *      The physical instance number within the group to inject the error into.
 *   instance:
 *      The physical slice number within the instance to inject the error into.
 *   locationMask
 *      SYSLTC location subtype(s) where error is to be injected. Same as LTC.
 *   errorType
 *      Type of error to inject
 *      NV208F_CTRL_FB_ERROR_TYPE_CORRECTABLE for SBE.
 *      NV208F_CTRL_FB_ERROR_TYPE_UNCORRECTABLE for DBE.
 *
 */
#define NV208F_CTRL_CMD_FB_INJECT_SYSLTC_ECC_ERROR (0x208f0517) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_DIAG_FB_INTERFACE_ID << 8) | NV208F_CTRL_FB_INJECT_SYSLTC_ECC_ERROR_PARAMS_MESSAGE_ID" */

#define NV208F_CTRL_FB_INJECT_SYSLTC_ECC_ERROR_PARAMS_MESSAGE_ID (0x17U)

typedef struct NV208F_CTRL_FB_INJECT_SYSLTC_ECC_ERROR_PARAMS {
    NvU8                      group;
    NvU8                      instance;
    NvU8                      slice;
    NvU8                      locationMask;
    NV208F_CTRL_FB_ERROR_TYPE errorType;
} NV208F_CTRL_FB_INJECT_SYSLTC_ECC_ERROR_PARAMS;

/*
 * NV208F_CTRL_CMD_FB_GET_FBPA_PAC_MASKS
 *
 * This API returns the PAC mask for an FBPA. The format is an array where the
 * index is the physical FBPA value and the value at the index is the channel
 * mask at the corresponding FBPA. At this time there can only be max 4
 * channels per FBPA. A floorswept FBPA will have a value of 0x0, vs a
 * non-floorswept FBPA with no floorswept channels will have a value of 0xf
 *
 */
#define NV208F_CTRL_FB_GET_FBPA_PAC_MASKS_MAX_FBPAS 64

#define NV208F_CTRL_CMD_FB_GET_FBPA_PAC_MASKS       (0x208f0518) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_DIAG_FB_INTERFACE_ID << 8) | NV208F_CTRL_FB_GET_FBPA_PAC_MASKS_PARAMS_MESSAGE_ID" */

#define NV208F_CTRL_FB_GET_FBPA_PAC_MASKS_PARAMS_MESSAGE_ID (0x18U)

typedef struct NV208F_CTRL_FB_GET_FBPA_PAC_MASKS_PARAMS {
    NvU8 fbpas[NV208F_CTRL_FB_GET_FBPA_PAC_MASKS_MAX_FBPAS];
} NV208F_CTRL_FB_GET_FBPA_PAC_MASKS_PARAMS;

/*
 * NV208F_CTRL_CMD_FB_CONVERT_CHANNEL
 *
 * This API converts either a channel from physical to logical or vice-versa
 *
 *   conversionType:
 *      See NV208F_CTRL_FB_CHANNEL_CONVERSION_TYPE
 *   fbpa:
 *      The physical fbpa the channel resides
 *   input:
 *      Input channel
 *   output:
 *      Output channel
 */
#define NV208F_CTRL_CMD_FB_CONVERT_CHANNEL (0x208f0519) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_DIAG_FB_INTERFACE_ID << 8) | NV208F_CTRL_FB_CONVERT_CHANNEL_PARAMS_MESSAGE_ID" */

#define NV208F_CTRL_FB_CONVERT_CHANNEL_PARAMS_MESSAGE_ID (0x19U)

typedef struct NV208F_CTRL_FB_CONVERT_CHANNEL_PARAMS {
    NvU32 conversionType;
    NvU32 fbpa;
    NvU32 input;
    NvU32 output;
} NV208F_CTRL_FB_CONVERT_CHANNEL_PARAMS;

#define NV208F_CTRL_FB_CHANNEL_CONVERSION_TYPE_LOGICAL_TO_PHYSICAL (0x00000000U)
#define NV208F_CTRL_FB_CHANNEL_CONVERSION_TYPE_PHYSICAL_TO_LOGICAL (0x00000001U)
/* _ctrl208ffb_h_ */
