/*
 * SPDX-FileCopyrightText: Copyright (c) 2012-2021 NVIDIA CORPORATION & AFFILIATES
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
#ifndef _NVLOG_DEFS_H_
#define _NVLOG_DEFS_H_

#include "nvtypes.h"
/******************* Common Debug & Trace Defines ***************************\
*                                                                           *
* Module: NVLOG_DEFS.H                                                      *
*                                                                           *
\****************************************************************************/

#define NVLOG_MAX_DBG_MODULES  256

/********************************/
/*********  Structures  *********/
/********************************/

// Forward declaration, so it can be used in the function type definition.

/**
 * @brief Struct representing a buffer in NvLog
 *
 * All logging (Print, Regtrace, etc) use these buffers.
 */
typedef struct _NVLOG_BUFFER NVLOG_BUFFER;


/**
 * @brief Type of the 'push' function for NvLog buffers
 *
 * Function called whenever pushing something to an NvLog buffer
 */
typedef NvBool (*NVLOG_BUFFER_PUSHFUNC) (NVLOG_BUFFER *, NvU8 *, NvU32);



/**
 * @brief Fields specific to ring buffers
 */
typedef struct _NVLOG_RING_BUFFER_EXTRA_FIELDS
{
    /** How many times the ring buffer has overflown */
    NvU32 overflow;
} NVLOG_RING_BUFFER_EXTRA_FIELDS;


/**
 * @brief Struct representing a buffer in NvLog
 *
 * All logging (Print, Regtrace, etc) use these buffers.
 */
struct _NVLOG_BUFFER
{
    /** Function to call when writing to this buffer */
    union
    {
        NVLOG_BUFFER_PUSHFUNC fn;

        // Pad this union to prevent struct size from varying between 32/64 bit platforms
        NvP64                 padding;
    } push;

    /** Size of the buffer data section */
    NvU32                   size;
    /** Buffer tag, for easier identification in a dump */
    NvU32                   tag;
    /** Flags of the buffer, following NVLOG_BUFFER_FLAGS_* DRF's */
    NvU32                   flags;
    /** Position of the next available byte in the buffer */
    NvU32                   pos;
    /** Number of threads currently writing to this buffer */
    volatile NvS32          threadCount;
    /** Specific buffer types will define their fields here */
    union
    {
        NVLOG_RING_BUFFER_EXTRA_FIELDS ring;
    } extra;
    /** Buffer data. */
    NvU8                    data[1];
};

#define NVLOG_MAX_BUFFERS_v11       16
#define NVLOG_MAX_BUFFERS_v12       256

#if NVOS_IS_UNIX
#define NVLOG_MAX_BUFFERS           NVLOG_MAX_BUFFERS_v12
#define NVLOG_LOGGER_VERSION        12          // v1.2
#else
#define NVLOG_MAX_BUFFERS           NVLOG_MAX_BUFFERS_v11
#define NVLOG_LOGGER_VERSION        11          // v1.1
#endif // NVOS_IS_UNIX


//
// Due to this file's peculiar location, NvPort may or may not be includable
// This hack will go away when NvLog is moved into common/shared
//
#if NVOS_IS_MACINTOSH

#if !PORT_IS_KERNEL_BUILD
typedef struct PORT_SPINLOCK PORT_SPINLOCK;
#else
#include "nvport/nvport.h"
#endif

#elif !defined(PORT_IS_KERNEL_BUILD)
typedef struct PORT_SPINLOCK PORT_SPINLOCK;
#else
#include "nvport/nvport.h"
#endif

/**
 * @brief Information about the entire NvLog system
 */
typedef struct _NVLOG_LOGGER
{
    /** NvLog logger version */
    NvU32           version;
    /** Logging buffers */
    NVLOG_BUFFER *  pBuffers[NVLOG_MAX_BUFFERS];
    /** Index of the first unallocated buffer */
    NvU32           nextFree;
    /** Total number of free buffer slots */
    NvU32           totalFree;
    /** Lock for all buffer oprations */
    PORT_SPINLOCK*  mainLock;
} NVLOG_LOGGER;
extern NVLOG_LOGGER NvLogLogger;

//
// Buffer flags
//

// Logging to this buffer is disabled
#define NVLOG_BUFFER_FLAGS_DISABLED                     0:0
#define NVLOG_BUFFER_FLAGS_DISABLED_NO                   0
#define NVLOG_BUFFER_FLAGS_DISABLED_YES                  1

#define NVLOG_BUFFER_FLAGS_TYPE                         2:1
#define NVLOG_BUFFER_FLAGS_TYPE_RING                     0
#define NVLOG_BUFFER_FLAGS_TYPE_NOWRAP                   1
#define NVLOG_BUFFER_FLAGS_TYPE_SYSTEMLOG                2

// Expand buffer when full
#define NVLOG_BUFFER_FLAGS_EXPANDABLE                   3:3
#define NVLOG_BUFFER_FLAGS_EXPANDABLE_NO                 0
#define NVLOG_BUFFER_FLAGS_EXPANDABLE_YES                1

// Allocate buffer in non paged memory
#define NVLOG_BUFFER_FLAGS_NONPAGED                     4:4
#define NVLOG_BUFFER_FLAGS_NONPAGED_NO                   0
#define NVLOG_BUFFER_FLAGS_NONPAGED_YES                  1

//
// Type of buffer locking to use
// NONE  - No locking performed, for buffers that are inherently single threaded
// STATE - Lock only during state change, do memory copying unlocked
//         Don't use with tiny buffers that overflow every write or two.
// FULL  - Keep everything locked for the full duration of the write
//
#define NVLOG_BUFFER_FLAGS_LOCKING                      6:5
#define NVLOG_BUFFER_FLAGS_LOCKING_NONE                  0
#define NVLOG_BUFFER_FLAGS_LOCKING_STATE                 1
#define NVLOG_BUFFER_FLAGS_LOCKING_FULL                  2

// Store this buffer in OCA minidumps
#define NVLOG_BUFFER_FLAGS_OCA                          7:7
#define NVLOG_BUFFER_FLAGS_OCA_NO                        0
#define NVLOG_BUFFER_FLAGS_OCA_YES                       1

// Buffer format (not included in registry key)
#define NVLOG_BUFFER_FLAGS_FORMAT                     10:8
#define NVLOG_BUFFER_FLAGS_FORMAT_PRINTF                 0
#define NVLOG_BUFFER_FLAGS_FORMAT_LIBOS_LOG              1
#define NVLOG_BUFFER_FLAGS_FORMAT_MEMTRACK               2

// Buffer GPU index
#define NVLOG_BUFFER_FLAGS_GPU_INSTANCE              31:24

typedef NvU32 NVLOG_BUFFER_HANDLE;

//
// Utility macros
//
#define NVLOG_IS_RING_BUFFER(pBuffer)                                          \
    FLD_TEST_DRF(LOG_BUFFER, _FLAGS, _TYPE, _RING, pBuffer->flags)
#define NVLOG_IS_NOWRAP_BUFFER(pBuffer)                                        \
    FLD_TEST_DRF(LOG_BUFFER, _FLAGS, _TYPE, _NOWRAP, pBuffer->flags)

#define NVLOG_PRINT_BUFFER_SIZE(pBuffer)               ((pBuffer)->size)
#define NVLOG_BUFFER_SIZE(pBuffer)                                             \
    (NV_OFFSETOF(NVLOG_BUFFER, data) + NVLOG_PRINT_BUFFER_SIZE(pBuffer))

/********************************/
/*********  Filtering  **********/
/********************************/
// TODO - Remove all this once tools are updated

#define NVLOG_FILTER_INVALID                            (~0)

#define NVLOG_FILTER_VALUE_SIMPLE_NO                     0x0
#define NVLOG_FILTER_VALUE_SIMPLE_YES                    0x1
#define NVLOG_FILTER_VALUE_EXPLICIT_NO                   0x2
#define NVLOG_FILTER_VALUE_EXPLICIT_YES                  0x3

#define NVLOG_FILTER_PRINT_LEVEL_REGTRACE                1:0
#define NVLOG_FILTER_PRINT_LEVEL_INFO                    3:2
#define NVLOG_FILTER_PRINT_LEVEL_NOTICE                  5:4
#define NVLOG_FILTER_PRINT_LEVEL_WARNINGS                7:6
#define NVLOG_FILTER_PRINT_LEVEL_ERRORS                  9:8
#define NVLOG_FILTER_PRINT_LEVEL_HW_ERROR               11:10
#define NVLOG_FILTER_PRINT_LEVEL_FATAL                  13:12

#define NVLOG_FILTER_PRINT_BUFFER                       18:14
#define NVLOG_FILTER_REGTRACE_BUFFER                    22:19

#define NVLOG_FILTER_REGTRACE_LOG_READ                  25:23
#define NVLOG_FILTER_REGTRACE_LOG_WRITE                 27:26
#define NVLOG_FILTER_REGTRACE_BREAK_READ                29:28
#define NVLOG_FILTER_REGTRACE_BREAK_WRITE               31:30

#define NVLOG_FILTER_VALUE_IS_NO(val)               ((val & 0x1) == 0)
#define NVLOG_FILTER_VALUE_IS_YES(val)               (val & 0x1)
#define NVLOG_FILTER_PRINT_GET_VALUE(level, num)    ((num >> (level*2)) & 0x3)

/**
 * @brief Type representing a value of a given 16bit range.
 */
typedef struct _NVLOG_RANGE_16
{
    NvU16 low;
    NvU16 high;
    NvU32 value;
} NVLOG_RANGE_16;


/**
 * @brief Type representing a value of a given 32bit range.
 */
typedef struct _NVLOG_RANGE_32
{
    NvU32 low;
    NvU32 high;
    NvU32 value;
} NVLOG_RANGE_32;

//
// Maximum number of files that have a filter assigned to them.
//
#define NVLOG_MAX_FILES                         1
//
// Maximum number of line rules (both single line and range) allowed per file
//
#define NVLOG_FILELINE_FILTER_MAX_RANGES        1

/**
 * @brief Internal type for NVLOG_FILELINE_FILTER.
 *
 * Contains filtering info for a single file.
 */
typedef struct _NVLOG_FILELINE_FILTER_FILEHASH
{
    /** ID of the file (24bit MD5) */
    NvU32 fileId;
    /** Number of elements in the array 'ranges' */
    NvU32 numElems;
    /** Value to use if the given value isn't found in the range array */
    NvU32 defaultValue;
    /** Array of ranges representing lines in the file */
    NVLOG_RANGE_16 ranges[NVLOG_FILELINE_FILTER_MAX_RANGES];
} NVLOG_FILELINE_FILTER_FILEHASH;

/**
 * @brief Filter that contains rules that depend on the file and line number.
 */
typedef struct _NVLOG_FILELINE_FILTER
{
    /** Number of elements in the fileHash array */
    NvU32 numFiles;
    /** Value to use if a given file isn't found */
    NvU32 defaultValue;
    /** Array of file entries, ordered as a hash table */
    NVLOG_FILELINE_FILTER_FILEHASH fileHash[NVLOG_MAX_FILES];
} NVLOG_FILELINE_FILTER;

/********************************/
/********* Print Logger *********/
/********************************/

#define NVLOG_PRINT_LOGGER_VERSION                 11     // v1.1
// Max buffers cannot be over 32.
#define NVLOG_PRINT_MAX_BUFFERS                    8

#define NVLOG_PRINT_BUFFER_PRIMARY                 1
#define NVLOG_PRINT_BUFFER_SECONDARY               2
#define NVLOG_PRINT_BUFFER_SYSTEMLOG               3

#define NVLOG_PRINT_DESC1_FILEID                 23:0
#define NVLOG_PRINT_DESC1_GPUID                  28:24    // 2^5 = 32 possible
#define NVLOG_PRINT_DESC1_MAGIC                  31:29
#define NVLOG_PRINT_DESC1_MAGIC_VALUE              5

#define NVLOG_PRINT_DESC2_LINEID                 15:0
#define NVLOG_PRINT_DESC2_GROUPID                17:16
#define NVLOG_PRINT_DESC2_GROUPID_RM               0
#define NVLOG_PRINT_DESC2_GROUPID_PMU              1
#define NVLOG_PRINT_DESC2_OPT_DATA_COUNT         24:18    // number of dwords
#define NVLOG_PRINT_DESC2_OPT_DATA_COUNT_MAX      0x7F
#define NVLOG_PRINT_DESC2_RESERVED               28:25
#define NVLOG_PRINT_DESC2_MAGIC                  31:29
#define NVLOG_PRINT_DESC2_MAGIC_VALUE              6

#define NVLOG_UNKNOWN_GPU_INSTANCE                0x1f

#define NVLOG_PRINT_MODULE_FILTER_VALUE           1:0
#define NVLOG_PRINT_MODULE_FILTER_BUFFER          6:2
#define NVLOG_PRINT_MODULE_FILTER_ENABLED         7:7

//
// Regkey fields - These are copied directly from nvRmReg.h
// A copy is necessary as these might be needed on systems that don't
// have nvRmReg.h, such as DVS builds for NvWatch
//
#ifndef NV_REG_STR_RM_NVLOG
#define NV_REG_STR_RM_NVLOG                          "RMNvLog"
#define NV_REG_STR_RM_NVLOG_BUFFER_FLAGS                7:0
#define NV_REG_STR_RM_NVLOG_BUFFER_SIZE                23:8
#define NV_REG_STR_RM_NVLOG_BUFFER_SIZE_DEFAULT  ((NVOS_IS_WINDOWS||NVOS_IS_MACINTOSH)?8:250)
#define NV_REG_STR_RM_NVLOG_BUFFER_SIZE_DISABLE          0
#define NV_REG_STR_RM_NVLOG_RUNTIME_LEVEL              28:25
#define NV_REG_STR_RM_NVLOG_TIMESTAMP                  30:29
#define NV_REG_STR_RM_NVLOG_TIMESTAMP_NONE               0
#define NV_REG_STR_RM_NVLOG_TIMESTAMP_32                 1
#define NV_REG_STR_RM_NVLOG_TIMESTAMP_64                 2
#define NV_REG_STR_RM_NVLOG_TIMESTAMP_32_DIFF            3
#define NV_REG_STR_RM_NVLOG_INITED                     31:31
#define NV_REG_STR_RM_NVLOG_INITED_NO                    0
#define NV_REG_STR_RM_NVLOG_INITED_YES                   1
#endif // NV_REG_STR_RM_NVLOG


//
// Arg types:
//    0:    Special meaning. End of argument list.
//    1:    d, u, x, X, i, o             - Integer type
//    2:    lld, llu, llx, llX, lli, llo - Long long integer type
//    3:    s                            - string type (size is 0)
//    4:    p                            - pointer type
//    5:    c                            - char type
//    6:    f, g, e, F, G, E             - floating point type
//    7-14: Unused at the moment, default value is 0
//    15:   Special meaning. Error value - unsupported type.
//
#define NVLOG_PRINT_MAX_ARG_TYPES                 0x10
#define NVLOG_PRINT_ARG_TYPE_ARGLIST_END          0x0
#define NVLOG_PRINT_ARG_TYPE_INT                  0x1
#define NVLOG_PRINT_ARG_TYPE_LONGLONG             0x2
#define NVLOG_PRINT_ARG_TYPE_STRING               0x3
#define NVLOG_PRINT_ARG_TYPE_POINTER              0x4
#define NVLOG_PRINT_ARG_TYPE_CHAR                 0x5
#define NVLOG_PRINT_ARG_TYPE_FLOAT                0x6
#define NVLOG_PRINT_ARG_TYPE_ERROR                0xf


/**
 * @brief Signature of the database required to decode the print logs
 *
 * The sig1-sig3 values are generated randomly at compile time.
 */
typedef struct _NVLOG_DB_SIGNATURE
{
    NvU32 timestamp;
    NvU32 sig1;
    NvU32 sig2;
    NvU32 sig3;
} NVLOG_DB_SIGNATURE;

/**
 * @brief Filter that contains all rules used to filter DBG_PRINTF calls
 */
typedef struct _NVLOG_PRINT_FILTER
{
    /** Same file:line filter is shared with the Regtrace system */
    NVLOG_FILELINE_FILTER *pFileLineFilter;
    /** Filter based on debug levels. Uses NVLOG_FILTER_PRINT_LEVEL_* DRF's */
    NvU32 runtimePrintLevelFilter;
    /** Filter based on debug modules. Uses NVLOG_PRINT_MODULE_FILTER_* DRF's */
    NvU8  runtimePrintModuleFilter[NVLOG_MAX_DBG_MODULES];
} NVLOG_PRINT_FILTER;


/**
 * @brief Enum representing all possible argument types to DBG_PRINTF
 */
typedef enum _NVLOG_ARGTYPE
{
    NVLOG_ARGTYPE_NONE,
    NVLOG_ARGTYPE_INT,
    NVLOG_ARGTYPE_LONG_LONG_INT,
    NVLOG_ARGTYPE_STRING,
    NVLOG_ARGTYPE_POINTER,
    NVLOG_ARGTYPE_FLOAT,
    NVLOG_ARGTYPE__COUNT
} NVLOG_ARGTYPE;

/**
 * @brief General info about the NvLog Print system
 */
typedef struct _NVLOG_PRINT_LOGGER
{
    /** NvLog print logger version */
    NvU32               version;
    /** Runtime argument sizes (16 different arglist values) */
    NvU8                runtimeSizes[NVLOG_PRINT_MAX_ARG_TYPES];
    /** Database signature for decoding */
    NVLOG_DB_SIGNATURE  signature;
    /** Filter buffer for print statements */
    NVLOG_PRINT_FILTER  filter;
    /** Flags for all NvLog print buffers */
    NvU32               flags;
    /** Buffer indices for all nvlog buffers. buffers[1] is default. */
    NvU32               buffers[NVLOG_PRINT_MAX_BUFFERS];
    /** Initialized flag, set to true after nvlogPrintInit has executed */
    NvBool              initialized;
    /** Paused flag, set to true after nvlogPrintInit has executed */
    NvBool              paused;
} NVLOG_PRINT_LOGGER;
extern NVLOG_PRINT_LOGGER NvLogPrintLogger;

#define NVLOG_PRINT_BUFFER_TAG(_i)      NvU32_BUILD('t','r','p','0' + (_i))

/********************************/
/**********  Regtrace  **********/
/********************************/

#define NVLOG_REGTRACE_LOGGER_VERSION          10   // v1.0
#define NVLOG_REGTRACE_MAX_BUFFERS             4

#define NVLOG_REGTRACE_READ                    0
#define NVLOG_REGTRACE_WRITE                   1

#define NVLOG_REGTRACE_DESC1_FILEID            NVLOG_PRINT_DESC1_FILEID
#define NVLOG_REGTRACE_DESC1_GPUID             NVLOG_PRINT_DESC1_GPUID
#define NVLOG_REGTRACE_DESC1_MAGIC             NVLOG_PRINT_DESC1_MAGIC
#define NVLOG_REGTRACE_DESC1_MAGIC_VALUE       (NVLOG_PRINT_DESC1_MAGIC_VALUE-1)

#define NVLOG_REGTRACE_DESC2_LINEID            15:0
#define NVLOG_REGTRACE_DESC2_READWRITE         16:16
#define NVLOG_REGTRACE_DESC2_READWRITE_READ    NVLOG_REGTRACE_READ
#define NVLOG_REGTRACE_DESC2_READWRITE_WRITE   NVLOG_REGTRACE_WRITE
#define NVLOG_REGTRACE_DESC2_REGSIZE           18:17
#define NVLOG_REGTRACE_DESC2_REGSIZE_8         0
#define NVLOG_REGTRACE_DESC2_REGSIZE_16        1
#define NVLOG_REGTRACE_DESC2_REGSIZE_32        2
#define NVLOG_REGTRACE_DESC2_REGSIZE_64        3
#define NVLOG_REGTRACE_DESC2_THREADID          28:19
#define NVLOG_REGTRACE_DESC2_MAGIC             31:29
#define NVLOG_REGTRACE_DESC2_MAGIC_VALUE       3

/**
 * @brief Single entry in an NvLog Regtrace buffer.
 */
typedef struct _NVLOG_REGTRACE_RECORD
{
    /** Uses NVLOG_REGTRACE_DESC1_* DRF's */
    NvU32 desc1;
    /** Uses NVLOG_REGTRACE_DESC1_* DRF's */
    NvU32 desc2;
    /** Address of the register being accessed */
    NvU32 address;
    /** Value that was read/written */
    NvU32 value;
} NVLOG_REGTRACE_RECORD;



#define NVLOG_REGTRACE_FILTER_MAX_RANGES       256

// Regtrace shares the file:line filter with print


/**
 * @brief Filter that contains all rules used to filter register access logging
 */
typedef struct _NVLOG_REGTRACE_FILTER
{
    /** Number of elements in the 'ranges' array */
    NvU32 numRanges;
    /** File:line based filter. Shared with NvLog print system */
    NVLOG_FILELINE_FILTER *pFileLineFilter;
    /** Range array for filtering based on register addresses */
    NVLOG_RANGE_32 ranges[NVLOG_REGTRACE_FILTER_MAX_RANGES];
} NVLOG_REGTRACE_FILTER;

/**
 * @brief General info about the NvLog Regtrace system
 */
typedef struct _NVLOG_REGTRACE_LOGGER
{
    /** NvLog regtrace logger version */
    NvU32 version;
    /** Filter buffer for regtrace statements */
    NVLOG_REGTRACE_FILTER filter;
    /** Buffer indices for all NvLog buffers. First element is default buffer */
    NvU32 buffers[NVLOG_REGTRACE_MAX_BUFFERS];
} NVLOG_REGTRACE_LOGGER;

#endif // _NVLOG_DEFS_H_
