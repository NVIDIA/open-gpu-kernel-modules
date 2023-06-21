/*
 * SPDX-FileCopyrightText: Copyright (c) 2005-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrl0000/ctrl0000nvd.finn
//

#include "ctrl/ctrl0000/ctrl0000base.h"

#include "ctrl/ctrlxxxx.h"
/* NV01_ROOT (client) nvd control commands and parameters */

/*
 * NV0080_CTRL_NVD_DUMP_COMPONENT
 *
 * The following dump components are used to describe legal ranges in
 * commands below:
 *
 *   NV0080_CTRL_CMD_NVD_DUMP_COMPONENT_SYS
 *     This is the system dump component.
 *   NV0080_CTRL_CMD_NVD_DUMP_COMPONENT_NVLOG
 *     This is the nvlog dump component.
 *   NV0080_CTRL_CMD_NVD_DUMP_COMPONENT_RESERVED
 *     This component is reserved.
 *
 * See nvdump.h for more information on dump component values.
 */
#define NV0000_CTRL_NVD_DUMP_COMPONENT_SYS      (0x400)
#define NV0000_CTRL_NVD_DUMP_COMPONENT_NVLOG    (0x800)
#define NV0000_CTRL_NVD_DUMP_COMPONENT_RESERVED (0xB00)

/*
 * NV0000_CTRL_CMD_NVD_GET_DUMP_SIZE
 *
 * This command gets the expected dump size of a particular system
 * dump component.  Note that events that occur between this command
 * and a later NV0000_CTRL_CMD_NVD_GET_DUMP command could alter the size of
 * the buffer required.
 *
 *   component
 *     This parameter specifies the system dump component for which the
 *     dump size is desired.  Legal values for this parameter must
 *     be greater than or equal to NV0000_CTRL_NVD_DUMP_COMPONENT_SYS and
 *     less than NV0000_CTRL_NVD_GET_DUMP_COMPONENT_NVLOG.
 *   size
 *     This parameter returns the expected size in bytes.  The maximum
 *     value of this call is NV0000_CTRL_NVD_MAX_DUMP_SIZE.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT if components are invalid.
 */

#define NV0000_CTRL_CMD_NVD_GET_DUMP_SIZE       (0x601) /* finn: Evaluated from "(FINN_NV01_ROOT_NVD_INTERFACE_ID << 8) | NV0000_CTRL_NVD_GET_DUMP_SIZE_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_NVD_GET_DUMP_SIZE_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV0000_CTRL_NVD_GET_DUMP_SIZE_PARAMS {
    NvU32 component;
    NvU32 size;
} NV0000_CTRL_NVD_GET_DUMP_SIZE_PARAMS;

/* Max size that a GET_DUMP_SIZE_PARAMS call can return */
#define NV0000_CTRL_NVD_MAX_DUMP_SIZE (1000000)

/*
 * NV0000_CTRL_CMD_NVD_GET_DUMP
 *
 * This command gets a dump of a particular system dump component. If triggers
 * is non-zero, the command waits for the trigger to occur before it returns.
 *
 *   pBuffer
 *     This parameter points to the buffer for the data.
 *   component
 *     This parameter specifies the system dump component for which the
 *     dump is to be retrieved.  Legal values for this parameter must
 *     be greater than or equal to NV0000_CTRL_NVD_DUMP_COMPONENT_SYS and
 *     less than NV0000_CTRL_NVD_GET_DUMP_COMPONENT_NVLOG.
 *   size
 *     On entry, this parameter specifies the maximum length for
 *     the returned data. On exit, it specifies the number of bytes
 *     returned.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NVOS_ERROR_INVALID_ARGUMENT if components are invalid.
 *   NVOS_ERROR_INVALID_ADDRESS if pBuffer is invalid
 *   NVOS_ERROR_INVALID_???? if the buffer was too small
 */
#define NV0000_CTRL_CMD_NVD_GET_DUMP  (0x602) /* finn: Evaluated from "(FINN_NV01_ROOT_NVD_INTERFACE_ID << 8) | NV0000_CTRL_NVD_GET_DUMP_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_NVD_GET_DUMP_PARAMS_MESSAGE_ID (0x2U)

typedef struct NV0000_CTRL_NVD_GET_DUMP_PARAMS {
    NV_DECLARE_ALIGNED(NvP64 pBuffer, 8);
    NvU32 component;
    NvU32 size;
} NV0000_CTRL_NVD_GET_DUMP_PARAMS;

/*
 * NV0000_CTRL_CMD_NVD_GET_TIMESTAMP
 *
 * This command returns the current value of the timestamp used
 * by the RM in NvDebug dumps. It is provided to keep the RM and NvDebug
 * clients on the same time base.
 *
 *   cpuClkId
 *     See also NV2080_CTRL_CMD_TIMER_GET_GPU_CPU_TIME_CORRELATION_INFO
 *     This parameter specifies the source of the CPU clock. Legal values for
 *     this parameter include:
 *       NV0000_NVD_CPU_TIME_CLK_ID_DEFAULT and NV0000_NVD_CPU_TIME_CLK_ID_OSTIME
 *         This clock id will provide real time in microseconds since 00:00:00 UTC on January 1, 1970.
 *         It is calculated as follows:
 *          (seconds * 1000000) + uSeconds
 *       NV0000_NVD_CPU_TIME_CLK_ID_PLATFORM_API
 *         This clock id will provide time stamp that is constant-rate, high
 *         precision using platform API that is also available in the user mode.
 *       NV0000_NVD_CPU_TIME_CLK_ID_TSC
 *         This clock id will provide time stamp using CPU's time stamp counter.
 *
 *   timestamp
 *     Retrieved timestamp
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_INVALID_ARGUMENT
 */

#define NV0000_NVD_CPU_TIME_CLK_ID_DEFAULT      (0x00000000)
#define NV0000_NVD_CPU_TIME_CLK_ID_OSTIME       (0x00000001)
#define NV0000_NVD_CPU_TIME_CLK_ID_TSC          (0x00000002)
#define NV0000_NVD_CPU_TIME_CLK_ID_PLATFORM_API (0x00000003)

#define NV0000_CTRL_CMD_NVD_GET_TIMESTAMP       (0x603) /* finn: Evaluated from "(FINN_NV01_ROOT_NVD_INTERFACE_ID << 8) | NV0000_CTRL_NVD_GET_TIMESTAMP_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_NVD_GET_TIMESTAMP_PARAMS_MESSAGE_ID (0x3U)

typedef struct NV0000_CTRL_NVD_GET_TIMESTAMP_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 timestamp, 8);
    NvU8 cpuClkId;
} NV0000_CTRL_NVD_GET_TIMESTAMP_PARAMS;

/*
 * NV0000_CTRL_CMD_NVD_GET_NVLOG_INFO
 *
 * This command gets the current state of the NvLog buffer system.
 *
 *   component (in)
 *     This parameter specifies the system dump component for which the
 *     NvLog info is desired.  Legal values for this parameter must
 *     be greater than or equal to NV0000_CTRL_NVD_DUMP_COMPONENT_NVLOG and
 *     less than NV0000_CTRL_NVD_DUMP_COMPONENT_RESERVED.
 *   version (out)
 *     This parameter returns the version of the Nvlog subsystem.
 *   runtimeSizes (out)
 *     This parameter returns the array of sizes for all supported printf
 *     specifiers.  This information is necessary to know how many bytes
 *     to decode when given a certain specifier (such as %d).
 *     The following describes the contents of each array entry:
 *       NV0000_CTRL_NVD_RUNTIME_SIZE_UNUSED
 *         This array entry has special meaning and is unused in the
 *         runtimeSizes array.
 *       NV0000_CTRL_NVD_RUNTIME_SIZE_INT
 *         This array entry returns the size of integer types for use in
 *         interpreting the %d, %u, %x, %X, %i, %o specifiers.
 *       NV0000_CTRL_NVD_RUNTIME_SIZE_LONG_LONG
 *         This array entry returns the size of long long integer types for
 *         using in interpreting the %lld, %llu, %llx, %llX, %lli, %llo
 *         specifiers.
 *       NV0000_CTRL_NVD_RUNTIME_SIZE_STRING
 *         This array entry returns zero as strings are not allowed.
 *       NV0000_CTRL_NVD_RUNTIME_SIZE_PTR
 *         This array entry returns the size of the pointer type for use
 *         in interpreting the %p specifier.
 *       NV0000_CTRL_NVD_RUNTIME_SIZE_CHAR
 *         This array entry returns the size of the char type for use in
 *         intpreting the %c specifier.
 *       NV0000_CTRL_NVD_RUNTIME_SIZE_FLOAT
 *         This array entry returns the size of the float types for use in
 *         in interpreting the %f, %g, %e, %F, %G, %E specifiers.
 *     All remaining entries are reserved and return 0.
 *   printFlags (out)
 *     This parameter returns the flags of the NvLog system.
 *       NV0000_CTRL_NVD_NVLOG_PRINT_FLAGS_BUFFER_FLAGS
 *         See NV0000_CTRL_CMD_NVD_GET_NVLOG_BUF_INFO for more details.
 *       NV0000_CTRL_NVD_NVLOG_PRINT_FLAGS_BUFFER_SIZE
 *         This field returns the buffer size in KBytes.  A value of zero
 *         is returned when logging is disabled.
 *       NV0000_CTRL_NVD_NVLOG_PRINT_FLAGS_TIMESTAMP
 *         This field returns the format of the timestamp.  Legal values
 *         for this parameter include:
 *           NV0000_CTRL_NVD_NVLOG_PRINT_FLAGS_TIMESTAMP_NONE
 *             This value indicates no timestamp.
 *           NV0000_CTRL_NVD_NVLOG_PRINT_FLAGS_TIMESTAMP_32BIT
 *             This value indicates a 32-bit timestamp.
 *           NV0000_CTRL_NVD_NVLOG_PRINT_FLAGS_TIMESTAMP_64BIT
 *             This value indicates a 64-bit timestamp.
 *           NV0000_CTRL_NVD_NVLOG_PRINT_FLAGS_TIMESTAMP_32BIT_DIFF
 *             This value indicates a 32-bit differential timestamp.
 *       NV0000_CTRL_NVD_NVLOG_PRINT_FLAGS_RESERVED
 *          This field is reserved.
 *       NV0000_CTRL_NVD_NVLOG_PRINT_FLAGS_RUNTIME_LEVEL
 *          This field returns the lowest debug level for which logging
 *          is enabled by default.
 *       NV0000_CTRL_NVD_NVLOG_PRINT_FLAGS_INIT
 *          This field indicates if logging for the specified component has
 *          been initialized. Legal values for this parameter include:
 *            NV0000_CTRL_NVD_NVLOG_PRINT_FLAGS_INIT_NO
 *              This value indicates NvLog is uninitialized.
 *            NV0000_CTRL_NVD_NVLOG_PRINT_FLAGS_INIT_YES
 *              This value indicates NvLog has been initialized.
 *   signature (out)
 *     This parameter is the signature of the database
 *     required to decode these logs, autogenerated at buildtime.
 *   bufferTags (out)
 *     This parameter identifies the buffer tag used during allocation
 *     or a value of '0' if buffer is unallocated for each possible
 *     buffer.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT if components are invalid.
 */
#define NV0000_CTRL_CMD_NVD_GET_NVLOG_INFO (0x604) /* finn: Evaluated from "(FINN_NV01_ROOT_NVD_INTERFACE_ID << 8) | NV0000_CTRL_NVD_GET_NVLOG_INFO_PARAMS_MESSAGE_ID" */

/* maximum size of the runtimeSizes array */
#define NV0000_CTRL_NVD_MAX_RUNTIME_SIZES  (16)

/* size of signature parameter */
#define NV0000_CTRL_NVD_SIGNATURE_SIZE     (4)

/* Maximum number of buffers */
#define NV0000_CTRL_NVD_MAX_BUFFERS        (256)

#define NV0000_CTRL_NVD_GET_NVLOG_INFO_PARAMS_MESSAGE_ID (0x4U)

typedef struct NV0000_CTRL_NVD_GET_NVLOG_INFO_PARAMS {
    NvU32 component;
    NvU32 version;
    NvU8  runtimeSizes[NV0000_CTRL_NVD_MAX_RUNTIME_SIZES];
    NvU32 printFlags;
    NvU32 signature[NV0000_CTRL_NVD_SIGNATURE_SIZE];
    NvU32 bufferTags[NV0000_CTRL_NVD_MAX_BUFFERS];
} NV0000_CTRL_NVD_GET_NVLOG_INFO_PARAMS;

/* runtimeSize array indices */
#define NV0000_CTRL_NVD_RUNTIME_SIZE_UNUSED                       (0)
#define NV0000_CTRL_NVD_RUNTIME_SIZE_INT                          (1)
#define NV0000_CTRL_NVD_RUNTIME_SIZE_LONG_LONG                    (2)
#define NV0000_CTRL_NVD_RUNTIME_SIZE_STRING                       (3)
#define NV0000_CTRL_NVD_RUNTIME_SIZE_PTR                          (4)
#define NV0000_CTRL_NVD_RUNTIME_SIZE_CHAR                         (5)
#define NV0000_CTRL_NVD_RUNTIME_SIZE_FLOAT                        (6)

/* printFlags fields and values */
#define NV0000_CTRL_NVD_NVLOG_INFO_PRINTFLAGS_BUFFER_INFO          7:0
#define NV0000_CTRL_NVD_NVLOG_INFO_PRINTFLAGS_BUFFER_SIZE          23:8
#define NV0000_CTRL_NVD_NVLOG_INFO_PRINTFLAGS_BUFFER_SIZE_DISABLE (0x00000000)
#define NV0000_CTRL_NVD_NVLOG_INFO_PRINTFLAGS_BUFFER_SIZE_DEFAULT (0x00000004)
#define NV0000_CTRL_NVD_NVLOG_INFO_PRINTFLAGS_RUNTIME_LEVEL        28:25
#define NV0000_CTRL_NVD_NVLOG_INFO_PRINTFLAGS_TIMESTAMP            30:29
#define NV0000_CTRL_NVD_NVLOG_INFO_PRINTFLAGS_TIMESTAMP_NONE      (0x00000000)
#define NV0000_CTRL_NVD_NVLOG_INFO_PRINTFLAGS_TIMESTAMP_32        (0x00000001)
#define NV0000_CTRL_NVD_NVLOG_INFO_PRINTFLAGS_TIMESTAMP_64        (0x00000002)
#define NV0000_CTRL_NVD_NVLOG_INFO_PRINTFLAGS_TIMESTAMP_32_DIFF   (0x00000003)
#define NV0000_CTRL_NVD_NVLOG_INFO_PRINTFLAGS_INITED               31:31
#define NV0000_CTRL_NVD_NVLOG_INFO_PRINTFLAGS_INITED_NO           (0x00000000)
#define NV0000_CTRL_NVD_NVLOG_INFO_PRINTFLAGS_INITED_YES          (0x00000001)

/*
 * NV0000_CTRL_CMD_NVD_GET_NVLOG_BUFFER_INFO
 *
 * This command gets the current state of a specific buffer in the NvLog
 * buffer system.
 *
 *   component (in)
 *     This parameter specifies the system dump component for which the
 *     NvLog info is desired.  Legal values for this parameter must
 *     be greater than or equal to NV0000_CTRL_NVD_DUMP_COMPONENT_NVLOG and
 *     less than NV0000_CTRL_NVD_DUMP_COMPONENT_RESERVED.
 *   buffer (in/out)
 *     This parameter specifies the buffer number from which to retrieve the
 *     buffer information. Valid values are 0 to (NV0000_CTRL_NVD_MAX_BUFFERS - 1).
 *     If the buffer is specified using the 'tag' parameter, the buffer
 *     number is returned through this one.
 *   tag (in/out)
 *     If this parameter is non-zero, it will be used to specify the buffer,
 *     instead of 'buffer' parameter. It returns the tag of the specified buffer
 *   size (out)
 *     This parameter returns the size of the specified buffer.
 *   flags (in/out)
 *     On input, this parameter sets the following behavior:
 *       NV0000_CTRL_NVD_NVLOG_BUFFER_INFO_FLAGS_PAUSE
 *         This flag controls if the nvlog system should pause output
 *         to this buffer.
 *           NV0000_CTRL_NVD_NVLOG_BUFFER_INFO_FLAGS_PAUSE_YES
 *             The buffer should be paused until another command 
 *             unpauses this buffer.
 *           NV0000_CTRL_NVD_NVLOG_BUFFER_INFO_FLAGS_PAUSE_NO
 *             The buffer should not be paused.
 *     On output, this parameter returns the flags of a specified buffer:
 *       NV0000_CTRL_NVD_NVLOG_BUFFER_INFO_FLAGS_DISABLED
 *         This flag indicates if logging to the specified buffer is
 *         disabled or not.
 *       NV0000_CTRL_NVD_NVLOG_BUFFER_INFO_FLAGS_TYPE
 *         This flag indicates the buffer logging type:
 *           NV0000_CTRL_NVD_NVLOG_BUFFER_INFO_FLAGS_TYPE_RING
 *             This type value indicates logging to the buffer wraps.
 *           NV0000_CTRL_NVD_NVLOG_BUFFER_INFO_FLAGS_TYPE_NOWRAP
 *             This type value indicates logging to the buffer does not wrap.
 *       NV0000_CTRL_NVD_NVLOG_BUFFER_INFO_FLAGS_EXPANDABLE
 *         This flag indicates if the buffer size is expandable.
 *           NV0000_CTRL_NVD_NVLOG_BUFFER_INFO_FLAGS_EXPANDABLE_NO
 *             The buffer is not expandable.
 *           NV0000_CTRL_NVD_NVLOG_BUFFER_INFO_FLAGS_EXPANDABLE_YES
 *             The buffer is expandable.
 *       NV0000_CTRL_NVD_NVLOG_BUFFER_INFO_FLAGS_NON_PAGED
 *         This flag indicates if the buffer occupies non-paged or pageable
 *         memory.
 *           NV0000_CTRL_NVD_NVLOG_BUFFER_INFO_FLAGS_NON_PAGED_NO
 *             The buffer is in pageable memory.
 *           NV0000_CTRL_NVD_NVLOG_BUFFER_INFO_FLAGS_NON_PAGES_YES
 *             The buffer is in non-paged memory.
 *       NV0000_CTRL_NVD_NVLOG_BUFFER_INFO_FLAGS_LOCKING
 *         This flag indicates the locking mode for the specified buffer.
 *           NV0000_CTRL_NVD_NVLOG_BUFFER_INFO_FLAGS_LOCKING_NONE
 *             This locking value indicates that no locking is performed.  This
 *             locking mode is typically used for inherently single-threaded
 *             buffers.
 *           NV0000_CTRL_NVD_NVLOG_BUFFER_INFO_FLAGS_LOCKING_STATE
 *             This locking value indicates that the buffer is locked only
 *             during state changes and that memory copying is unlocked.  This
 *             mode should not be used tiny buffers that overflow every write
 *             or two.
 *           NV0000_CTRL_NVD_NVLOG_BUFFER_INFO_FLAGS_LOCKING_FULL
 *             This locking value indicates the buffer is locked for the full
 *             duration of the write.
 *       NV0000_CTRL_NVD_NVLOG_BUFFER_INFO_FLAGS_OCA
 *         This flag indicates if the buffer is stored in OCA dumps.
 *           NV0000_CTRL_NVD_NVLOG_BUFFER_INFO_FLAGS_OCA_NO
 *             The buffer is not included in OCA dumps.
 *           NV0000_CTRL_NVD_NVLOG_BUFFER_INFO_FLAGS_OCA_YES
 *             The buffer is included in OCA dumps.
 *   pos (out)
 *      This parameter is the current position of the tracker/cursor in the
 *      buffer.
 *   overflow (out)
 *     This parameter is the number of times the buffer has overflowed.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT if components are invalid.
 */

#define NV0000_CTRL_CMD_NVD_GET_NVLOG_BUFFER_INFO                 (0x605) /* finn: Evaluated from "(FINN_NV01_ROOT_NVD_INTERFACE_ID << 8) | NV0000_CTRL_NVD_GET_NVLOG_BUFFER_INFO_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_NVD_GET_NVLOG_BUFFER_INFO_PARAMS_MESSAGE_ID (0x5U)

typedef struct NV0000_CTRL_NVD_GET_NVLOG_BUFFER_INFO_PARAMS {
    NvU32 component;
    NvU32 buffer;
    NvU32 tag;
    NvU32 size;
    NvU32 flags;
    NvU32 pos;
    NvU32 overflow;
} NV0000_CTRL_NVD_GET_NVLOG_BUFFER_INFO_PARAMS;

/* flags fields and values */
/* input */
#define NV0000_CTRL_NVD_NVLOG_BUFFER_INFO_FLAGS_PAUSE              0:0
#define NV0000_CTRL_NVD_NVLOG_BUFFER_INFO_FLAGS_PAUSE_NO       (0x00000000)
#define NV0000_CTRL_NVD_NVLOG_BUFFER_INFO_FLAGS_PAUSE_YES      (0x00000001)

/* output */
#define NV0000_CTRL_NVD_NVLOG_BUFFER_INFO_FLAGS_DISABLED           0:0
#define NV0000_CTRL_NVD_NVLOG_BUFFER_INFO_FLAGS_DISABLED_NO    (0x00000000)
#define NV0000_CTRL_NVD_NVLOG_BUFFER_INFO_FLAGS_DISABLED_YES   (0x00000001)
#define NV0000_CTRL_NVD_NVLOG_BUFFER_INFO_FLAGS_TYPE               1:1
#define NV0000_CTRL_NVD_NVLOG_BUFFER_INFO_FLAGS_TYPE_RING      (0x00000000)
#define NV0000_CTRL_NVD_NVLOG_BUFFER_INFO_FLAGS_TYPE_NOWRAP    (0x00000001)
#define NV0000_CTRL_NVD_NVLOG_BUFFER_INFO_FLAGS_EXPANDABLE         2:2
#define NV0000_CTRL_NVD_NVLOG_BUFFER_INFO_FLAGS_EXPANDABLE_NO  (0x00000000)
#define NV0000_CTRL_NVD_NVLOG_BUFFER_INFO_FLAGS_EXPANDABLE_YES (0x00000001)
#define NV0000_CTRL_NVD_NVLOG_BUFFER_INFO_FLAGS_NONPAGED           3:3
#define NV0000_CTRL_NVD_NVLOG_BUFFER_INFO_FLAGS_NONPAGED_NO    (0x00000000)
#define NV0000_CTRL_NVD_NVLOG_BUFFER_INFO_FLAGS_NONPAGED_YES   (0x00000001)
#define NV0000_CTRL_NVD_NVLOG_BUFFER_INFO_FLAGS_LOCKING            5:4
#define NV0000_CTRL_NVD_NVLOG_BUFFER_INFO_FLAGS_LOCKING_NONE   (0x00000000)
#define NV0000_CTRL_NVD_NVLOG_BUFFER_INFO_FLAGS_LOCKING_STATE  (0x00000001)
#define NV0000_CTRL_NVD_NVLOG_BUFFER_INFO_FLAGS_LOCKING_FULL   (0x00000002)
#define NV0000_CTRL_NVD_NVLOG_BUFFER_INFO_FLAGS_OCA                6:6
#define NV0000_CTRL_NVD_NVLOG_BUFFER_INFO_FLAGS_OCA_NO         (0x00000000)
#define NV0000_CTRL_NVD_NVLOG_BUFFER_INFO_FLAGS_OCA_YES        (0x00000001)

/*
 * NV0000_CTRL_CMD_NVD_GET_NVLOG
 *
 * This command retrieves the specified dump block from the specified
 * NvLog buffer.  To retrieve the entire buffer, the caller should start
 * with blockNum set to 0 and continue issuing calls with an incremented
 * blockNum until the returned size value is less than
 * NV0000_CTRL_NVD_NVLOG_MAX_BLOCK_SIZE.
 *
 *   component (in)
 *     This parameter specifies the system dump component for which the NvLog
 *     dump operation is to be directed.  Legal values for this parameter
 *     must be greater than or equal to NV0000_CTRL_NVD_DUMP_COMPONENT_NVLOG
 *     and less than NV0000_CTRL_NVD_DUMP_COMPONENT_RESERVED.
 *   buffer (in)
 *     This parameter specifies the NvLog buffer to dump.
 *   blockNum (in)
 *     This parameter specifies the block number for which data is to be
 *     dumped.
 *   size (in/out)
 *     On entry, this parameter specifies the maximum length in bytes for
 *     the returned data (should be set to NV0000_CTRL_NVLOG_MAX_BLOCK_SIZE).
 *     On exit, it specifies the number of bytes returned.
 *   data (out)
 *     This parameter returns the data for the specified block.  The size
 *     patameter values indicates the number of valid bytes returned.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NVOS_ERROR_INVALID_ARGUMENT if components are invalid.
 */
#define NV0000_CTRL_CMD_NVD_GET_NVLOG                          (0x606) /* finn: Evaluated from "(FINN_NV01_ROOT_NVD_INTERFACE_ID << 8) | NV0000_CTRL_NVD_GET_NVLOG_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_NVLOG_MAX_BLOCK_SIZE                       (4000)

#define NV0000_CTRL_NVD_GET_NVLOG_PARAMS_MESSAGE_ID (0x6U)

typedef struct NV0000_CTRL_NVD_GET_NVLOG_PARAMS {
    NvU32 component;
    NvU32 buffer;
    NvU32 blockNum;
    NvU32 size;
    NvU8  data[NV0000_CTRL_NVLOG_MAX_BLOCK_SIZE];
} NV0000_CTRL_NVD_GET_NVLOG_PARAMS;

/*
 * NV0000_CTRL_CMD_NVD_GET_RCERR_RPT
 *
 * This command returns block of registers that were recorded at the time
 * of an RC error for the current process.
 *
 *   reqIdx:
 *      [IN] the index of the report being requested.
 *      index rolls over to 0.
 *      if the requested index is not in the circular buffer, then no data is
 *      transferred & either NV_ERR_INVALID_INDEX (indicating the specified
 *      index is not in the table) is returned.
 *
 *   rptIdx:
 *      [OUT] the index of the report being returned.
 *      if the requested index is not in the circular buffer, then the value is
 *      undefined, no data istransferred & NV_ERR_INVALID_INDEX is returned.
 *      if the the specified index is present, but does not meet the requested
 *      criteria (refer to the owner & processId fields). the rptIdx will be
 *      set to a value that does not match the reqIdx, and no data will be
 *      transferred. NV_ERR_INSUFFICIENT_PERMISSIONS is still returned.
 *
 *   gpuTag:
 *      [OUT] id of the GPU whose data was collected.
 *
 *   rptTimeInNs:
 *      [OUT] the timestamp for when the report was created.
 *
 *   startIdx:
 *      [OUT] the index of the oldest start record for the first report that
 *      matches the specified criteria (refer to the owner & processId
 *      fields). If no records match the specified criteria, this value is
 *      undefined, the failure code NV_ERR_MISSING_TABLE_ENTRY will
 *      be returned, and no data will be transferred.
 *
 *   endIdx:
 *      [OUT] the index of the newest end record for the most recent report that
 *      matches the specified criteria (refer to the owner & processId
 *      fields). If no records match the specified criteria, this value is
 *      undefined, the failure code NV_ERR_MISSING_TABLE_ENTRY will
 *      be returned, and no data will be transferred.
 *
 *   rptType:
 *      [OUT] indicator of what data is in the report.
 *
 *  flags
 *      [OUT] a set odf flags indicating attributes of the record
 *          NV0000_CTRL_CMD_NVD_RCERR_RPT_FLAGS_POS_FIRST --    indicates this is the first record of a report.
 *          NV0000_CTRL_CMD_NVD_RCERR_RPT_FLAGS_POS_LAST --     indicates this is the last record of the report.
 *          NV0000_CTRL_CMD_NVD_RCERR_RPT_FLAGS_RANGE_VALID --  indicates this is the response contains a valid
*               index range.
 *              Note, this may be set when an error is returned indicating a valid range was found, but event of
 *              the requested index was not.
 *          NV0000_CTRL_CMD_NVD_RCERR_RPT_FLAGS_DATA_VALID --   indicates this is the response contains valid data.
 *
 *   rptCount:
 *      [OUT] number of entries returned in report.
 *
 *   owner:
 *      [IN] Entries are only returned if they have the same owner as the specified owner or the specified 
 *      owner Id is NV0000_CTRL_CMD_NVD_RPT_ANY_OWNER_ID.
 *      if the requested index is not owned by the specified owner, the rptIdx
 *      will be set to a value that does not match the reqIdx, and no data will
 *      be transferred.  NV_ERR_INSUFFICIENT_PERMISSIONS is returned.
 *
 *   processId:
 *      [IN] Deprecated
 *   report:
 *      [OUT] array of rptCount enum/value pair entries containing the data from the report.
 *      entries beyond rptCount are undefined.
 *
 *
 * Possible status values returned are:
 *  NV_OK -- we found & transferred the requested record.
 *  NV_ERR_MISSING_TABLE_ENTRY -- we don't find any records that meet the criteria.
 *  NV_ERR_INVALID_INDEX -- the requested index was not found in the buffer.
 *  NV_ERR_INSUFFICIENT_PERMISSIONS -- the requested record was found, but it did not meet the criteria.
 *  NV_ERR_BUSY_RETRY -- We could not access the circular buffer.
 *
 */

#define NV0000_CTRL_CMD_NVD_GET_RCERR_RPT                                            (0x607) /* finn: Evaluated from "(FINN_NV01_ROOT_NVD_INTERFACE_ID << 8) | NV0000_CTRL_CMD_NVD_GET_RCERR_RPT_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_CMD_NVD_RCERR_RPT_MAX_ENTRIES                                    200

// report types
#define NV0000_CTRL_CMD_NVD_RCERR_RPT_TYPE_TEST                                      0
#define NV0000_CTRL_CMD_NVD_RCERR_RPT_TYPE_GRSTATUS                                  1
#define NV0000_CTRL_CMD_NVD_RCERR_RPT_TYPE_GPCSTATUS                                 2
#define NV0000_CTRL_CMD_NVD_RCERR_RPT_TYPE_MMU_FAULT_STATUS                          3

// pseudo register enums                                                                                         attribute content
#define NV0000_CTRL_CMD_NVD_RCERR_RPT_REG_EMPTY                                      0x00000000
#define NV0000_CTRL_CMD_NVD_RCERR_RPT_REG_OVERFLOWED                                 0x00000001                  // number of missed entries.
#define NV0000_CTRL_CMD_NVD_RCERR_RPT_REG_MAX_PSEDO_REG                              0x0000000f



// Flags Definitions
#define NV0000_CTRL_CMD_NVD_RCERR_RPT_FLAGS_POS_FIRST                                0x00000001                  // indicates this is the first record of a report.
#define NV0000_CTRL_CMD_NVD_RCERR_RPT_FLAGS_POS_LAST                                 0x00000002                  // indicates this is the last record of the report.
#define NV0000_CTRL_CMD_NVD_RCERR_RPT_FLAGS_RANGE_VALID                              0x00000004                  // indicates this is the response contains a valid range
#define NV0000_CTRL_CMD_NVD_RCERR_RPT_FLAGS_DATA_VALID                               0x00000008                  // indicates this is the response contains valid data


// Attribute Definitions
#define TPC_REG_ATTR(gpcId, tpcId)                                                  ((gpcId << 8) | (tpcId))
#define ROP_REG_ATTR(gpcId, ropId)                                                  ((gpcId << 8) | (ropId))
#define SM_REG_ATTR(gpcId, tpcId, smId)                                             ((((gpcId) << 16) | ((tpcId) << 8)) | (smId))

// Process Id Pseudo values
#define NV0000_CTRL_CMD_NVD_RCERR_RPT_ANY_PROCESS_ID                                 0x00000000                  // get report for any process ID

#define NV0000_CTRL_CMD_NVD_RCERR_RPT_ANY_OWNER_ID                                   0xFFFFFFFF                  // get report for any owner ID


typedef struct NV0000_CTRL_CMD_NVD_RCERR_RPT_REG_ENTRY {
    NvU32 tag;
    NvU32 value;
    NvU32 attribute;
} NV0000_CTRL_CMD_NVD_RCERR_RPT_REG_ENTRY;

#define NV0000_CTRL_CMD_NVD_GET_RCERR_RPT_PARAMS_MESSAGE_ID (0x7U)

typedef struct NV0000_CTRL_CMD_NVD_GET_RCERR_RPT_PARAMS {
    NvU16                                   reqIdx;
    NvU16                                   rptIdx;
    NvU32                                   GPUTag;
    NvU32                                   rptTime;     // time in seconds since 1/1/1970
    NvU16                                   startIdx;
    NvU16                                   endIdx;
    NvU16                                   rptType;
    NvU32                                   flags;
    NvU16                                   rptCount;
    NvU32                                   owner;       // indicating whose reports to get
    NvU32                                   processId;       // deprecated field

    NV0000_CTRL_CMD_NVD_RCERR_RPT_REG_ENTRY report[NV0000_CTRL_CMD_NVD_RCERR_RPT_MAX_ENTRIES];
} NV0000_CTRL_CMD_NVD_GET_RCERR_RPT_PARAMS;

/*
 * NV0000_CTRL_CMD_NVD_GET_DPC_ISR_TS
 *
 * This command returns the time stamp information that are collected from
 * the execution of various DPCs/ISRs. This time stamp information is for
 * debugging purposes only and would help with analyzing regressions and
 * latencies for DPC/ISR execution times.
 *
 *   tsBufferSize
 *     This field specifies the size of the buffer that the caller allocates.
 *   tsBuffer
 *     THis field specifies a pointer in the callers address space to the
 *     buffer into which the timestamp info on DPC/ISR is to be returned.
 *     This buffer must at least be as big as tsBufferSize.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_OPERATING_SYSTEM
 */
#define NV0000_CTRL_CMD_NVD_GET_DPC_ISR_TS (0x608) /* finn: Evaluated from "(FINN_NV01_ROOT_NVD_INTERFACE_ID << 8) | NV0000_CTRL_NVD_GET_DPC_ISR_TS_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_NVD_GET_DPC_ISR_TS_PARAMS_MESSAGE_ID (0x8U)

typedef struct NV0000_CTRL_NVD_GET_DPC_ISR_TS_PARAMS {
    NvU32 tsBufferSize;
    NV_DECLARE_ALIGNED(NvP64 pTSBuffer, 8);
} NV0000_CTRL_NVD_GET_DPC_ISR_TS_PARAMS;

/* _ctrl0000nvd_h_ */
