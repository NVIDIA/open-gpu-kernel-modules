/*
 * SPDX-FileCopyrightText: Copyright (c) 2018 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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


#ifndef _ERROR_NVSWITCH_H_
#define _ERROR_NVSWITCH_H_

#include "nvtypes.h"

#include "ctrl_dev_nvswitch.h"

//
// Error logging
//

typedef struct
{
    NvU32 addr;
    NvU32 data;
    NvU32 info;
    NvU32 code;
} NVSWITCH_PRI_ERROR_LOG_TYPE;

typedef struct
{
    NvU32 addr;
    NvU32 data;
    NvU32 write;
    NvU32 dest;
    NvU32 subId;
    NvU32 errCode;
    NvU32 raw_data[4];
} NVSWITCH_PRI_TIMEOUT_ERROR_LOG_TYPE;

typedef struct
{
    NvU32 raw_pending;          // raw pending interrupt status
    NvU32 mask;                 // localized mask for current handler
    NvU32 raw_first;            // raw first register
    NvU32 raw_enable;           // raw mask/enable register
    NvU32 data[4];              // record of interrupt specific data
} NVSWITCH_INTERRUPT_LOG_TYPE;

#define NVSWITCH_RAW_ERROR_LOG_DATA_SIZE    16

#define NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_ROUTE_TIME     BIT(0)
#define NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_ROUTE_MISC     BIT(1)
#define NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_ROUTE_HDR      BIT(2)
#define NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_INGRESS_TIME   BIT(3)
#define NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_INGRESS_MISC   BIT(4)
#define NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_INGRESS_HDR    BIT(5)
#define NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_EGRESS_TIME    BIT(6)
#define NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_EGRESS_MISC    BIT(7)
#define NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_EGRESS_HDR     BIT(8)
#define NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_EGRESS_MC_TIME BIT(9)
#define NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_EGRESS_MC_MISC BIT(10)
#define NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_EGRESS_MC_HDR  BIT(11)
#define NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_MC_TIME        BIT(12)
#define NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_RED_TIME       BIT(13)

typedef struct
{
    NvU32 flags;
    NvU32 data[NVSWITCH_RAW_ERROR_LOG_DATA_SIZE];
} NVSWITCH_RAW_ERROR_LOG_TYPE;

#define NVSWITCH_ERROR_NEXT_LOCAL_NUMBER(log) (log->error_total)

typedef struct
{
    NvU32   error_type;                     // NVSWITCH_ERR_*
    NvU64   local_error_num;                // Count of preceding errors (local error log)
    NvU64   global_error_num;               // Count of preceding errors (globally)
    NVSWITCH_ERROR_SRC_TYPE error_src;      // NVSWITCH_ERROR_SRC_*
    NVSWITCH_ERROR_SEVERITY_TYPE severity;  // NVSWITCH_ERROR_SEVERITY_*
    NvU32   instance;                       // Used for link# or subengine instance
    NvU32   subinstance;                    // Used for lane# or similar
    NvBool  error_resolved;
    NvU64   timer_count;                    // NvSwitch timer count
    NvU64   time;                           // Platform time, in ns
    NvU32   line;
    NvU32   data_size;                      // Size of data
    NvU8    description[NVSWITCH_ERROR_MAX_DESCRPTION_LEN]; // Short description of error type    

    union
    {
        NvU64   address;
        NVSWITCH_PRI_ERROR_LOG_TYPE pri_error;
        NVSWITCH_PRI_TIMEOUT_ERROR_LOG_TYPE pri_timeout;
        NVSWITCH_INTERRUPT_LOG_TYPE intr;
        NVSWITCH_RAW_ERROR_LOG_TYPE raw;
    } data;
} NVSWITCH_ERROR_TYPE;

typedef struct
{
    NvU32               error_start;    // Start index within CB
    NvU32               error_count;    // Count of current errors in CB
    NvU64               error_total;    // Count of total errors logged
    NvU32               error_log_size; // CB size
    NVSWITCH_ERROR_TYPE *error_log;
    NvBool              overwritable;   // Old CB entries can be overwritten

} NVSWITCH_ERROR_LOG_TYPE;

//
// Helpful error logging wrappers
//

#define NVSWITCH_LOG_FATAL(_device, _errsrc, _errtype, _instance, _subinstance, _errresolved)\
    nvswitch_record_error(                                                              \
        _device,                                                                        \
        &(_device->log_FATAL_ERRORS),                                                   \
        NVSWITCH_ERR ## _errtype,                                                       \
        _instance, _subinstance,                                                        \
        NVSWITCH_ERROR_SRC ## _errsrc,                                                  \
        NVSWITCH_ERROR_SEVERITY_FATAL,                                                  \
        _errresolved,                                                                   \
        NULL, 0,                                                                        \
        __LINE__, NULL)

#define NVSWITCH_LOG_FATAL_DATA(_device, _errsrc, _errtype, _instance, _subinstance, _errresolved, _errdata, ...)   \
    nvswitch_record_error(                                                              \
        _device,                                                                        \
        &(_device->log_FATAL_ERRORS),                                                   \
        NVSWITCH_ERR ## _errtype,                                                       \
        _instance, _subinstance,                                                        \
        NVSWITCH_ERROR_SRC ## _errsrc,                                                  \
        NVSWITCH_ERROR_SEVERITY_FATAL,                                                  \
        _errresolved,                                                                   \
        _errdata, sizeof(*_errdata),                                                    \
        __LINE__, #__VA_ARGS__)

#define NVSWITCH_LOG_NONFATAL(_device, _errsrc, _errtype, _instance, _subinstance, _errresolved) \
    nvswitch_record_error(                                                              \
        _device,                                                                        \
        &(_device->log_NONFATAL_ERRORS),                                                \
        NVSWITCH_ERR ## _errtype,                                                       \
        _instance, _subinstance,                                                        \
        NVSWITCH_ERROR_SRC ## _errsrc,                                                  \
        NVSWITCH_ERROR_SEVERITY_NONFATAL,                                               \
        _errresolved,                                                                   \
        NULL, 0,                                                                        \
        __LINE__, NULL)

#define NVSWITCH_LOG_NONFATAL_DATA(_device, _errsrc, _errtype, _instance, _subinstance, _errresolved, _errdata, ...)   \
    nvswitch_record_error(                                                              \
        _device,                                                                        \
        &(_device->log_NONFATAL_ERRORS),                                                \
        NVSWITCH_ERR ## _errtype,                                                       \
        _instance, _subinstance,                                                        \
        NVSWITCH_ERROR_SRC ## _errsrc,                                                  \
        NVSWITCH_ERROR_SEVERITY_NONFATAL,                                               \
        _errresolved,                                                                   \
        _errdata, sizeof(*_errdata),                                                    \
        __LINE__, #__VA_ARGS__)

#define NVSWITCH_NVLINK_ARCH_ERROR_NONE             0
#define NVSWITCH_NVLINK_ARCH_ERROR_GENERIC          1
#define NVSWITCH_NVLINK_ARCH_ERROR_HW_FATAL         2
#define NVSWITCH_NVLINK_ARCH_ERROR_HW_CORRECTABLE   3
#define NVSWITCH_NVLINK_ARCH_ERROR_HW_UNCORRECTABLE 4

#define NVSWITCH_NVLINK_HW_ERROR_NONE               0x0
#define NVSWITCH_NVLINK_HW_GENERIC                  0x1
#define NVSWITCH_NVLINK_HW_INGRESS                  0x2
#define NVSWITCH_NVLINK_HW_EGRESS                   0x3
#define NVSWITCH_NVLINK_HW_FSTATE                   0x4
#define NVSWITCH_NVLINK_HW_TSTATE                   0x5
#define NVSWITCH_NVLINK_HW_ROUTE                    0x6
#define NVSWITCH_NVLINK_HW_NPORT                    0x7
#define NVSWITCH_NVLINK_HW_NVLCTRL                  0x8
#define NVSWITCH_NVLINK_HW_NVLIPT                   0x9
#define NVSWITCH_NVLINK_HW_NVLTLC                   0xA
#define NVSWITCH_NVLINK_HW_DLPL                     0xB
#define NVSWITCH_NVLINK_HW_AFS                      0xC
#define NVSWITCH_NVLINK_HW_MINION                   0xD
#define NVSWITCH_NVLINK_HW_HOST                     0xE
#define NVSWITCH_NVLINK_HW_NXBAR                    0XF
#define NVSWITCH_NVLINK_HW_SOURCETRACK              0x10

typedef NvU32 NVSWITCH_NVLINK_ARCH_ERROR;
typedef NvU32 NVSWITCH_NVLINK_HW_ERROR;

NVSWITCH_NVLINK_HW_ERROR nvswitch_translate_hw_error(NVSWITCH_ERR_TYPE type);
void nvswitch_translate_error(NVSWITCH_ERROR_TYPE *error_entry,
                              NVSWITCH_NVLINK_ARCH_ERROR *arch_error,
                              NVSWITCH_NVLINK_HW_ERROR *hw_error);
NvlStatus nvswitch_ctrl_get_errors(nvswitch_device *device,
                                   NVSWITCH_GET_ERRORS_PARAMS *p);

// Log correctable per-device error with data
#define NVSWITCH_REPORT_CORRECTABLE_DEVICE_DATA(_device, _logenum, _data, _fmt, ...)    \
    do                                                                                  \
    {                                                                                   \
        NVSWITCH_PRINT_SXID(_device, NVSWITCH_ERR ## _logenum,                          \
            "Correctable, " _fmt "\n", ## __VA_ARGS__ );                                \
        NVSWITCH_LOG_NONFATAL_DATA(_device, _HW, _logenum,                              \
            0, 0, NV_TRUE, _data);                                                      \
    } while(0)

// Log correctable per-link error with data
#define NVSWITCH_REPORT_CORRECTABLE_LINK_DATA(_device, _link, _logenum, _data, _fmt, ...) \
    do                                                                                    \
    {                                                                                     \
        NVSWITCH_PRINT_SXID(_device, NVSWITCH_ERR ## _logenum,                            \
            "Correctable, Link %02d " _fmt "\n", _link, ## __VA_ARGS__ );                 \
        NVSWITCH_LOG_NONFATAL_DATA(_device, _HW, _logenum,                                \
            _link, 0, NV_TRUE, _data);                                                    \
    } while(0)

// Log nonfatal per-link error
#define NVSWITCH_REPORT_NONFATAL_LINK(_device, _link, _logenum, _fmt, ...)              \
    do                                                                                  \
    {                                                                                   \
        NVSWITCH_PRINT_SXID(_device, NVSWITCH_ERR ## _logenum,                          \
            "Non-fatal, Link %02d " _fmt "\n", _link, ## __VA_ARGS__ );                 \
        NVSWITCH_LOG_NONFATAL(_device, _HW, _logenum,                                   \
            _link, 0, NV_FALSE);                                                        \
    } while(0)

#endif //_ERROR_NVSWITCH_H_
