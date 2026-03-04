/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _INTR_NVSWITCH_H_
#define _INTR_NVSWITCH_H_

#include "error_nvswitch.h"

//
// Wrapper to track interrupt servicing
//
#define NVSWITCH_UNHANDLED_INIT(val) (unhandled = (val))
#define NVSWITCH_HANDLED(mask)       (unhandled &= ~(mask))

#define NVSWITCH_UNHANDLED_CHECK(_device, _unhandled)                      \
    do                                                                     \
    {                                                                      \
        if (_unhandled)                                                    \
        {                                                                  \
            NVSWITCH_PRINT(_device, ERROR,                                 \
                        "%s:%d unhandled interrupt! %x\n",                 \
                        __FUNCTION__, __LINE__, _unhandled);               \
            NVSWITCH_PRINT_SXID(_device,                                   \
                  NVSWITCH_ERR_HW_HOST_UNHANDLED_INTERRUPT,                \
                  "Fatal, unhandled interrupt in %s(%d)\n",                \
                  __FUNCTION__, __LINE__);                                 \
            NVSWITCH_LOG_FATAL_DATA(_device, _HW,                          \
                _HW_HOST_UNHANDLED_INTERRUPT, 0, 0, NV_FALSE, &_unhandled);\
        }                                                                  \
    } while(0)

//
// Wrappers for basic leaf interrupt handling
//
#define NVSWITCH_PENDING(_bit) ((bit = (_bit)) && (pending & (_bit)))
#define NVSWITCH_FIRST()       (bit & report.raw_first) ? " (First)" : ""

//
// Report/log error interrupt helper.
//

//
// Print an intermediate point (non-leaf) in the interrupt tree.
//
#define NVSWITCH_REPORT_TREE(_logenum)                                        \
    do                                                                        \
    {                                                                         \
        NVSWITCH_PRINT(device, ERROR, "Intermediate, Link %02d \n", link);    \
    } while(0)

// Log correctable errors
#define NVSWITCH_REPORT_CORRECTABLE(_logenum, _str)                            \
    do                                                                         \
    {                                                                          \
        NVSWITCH_PRINT_SXID(device, NVSWITCH_ERR ## _logenum,                  \
            "Correctable, Link %02d %s%s\n", link, _str, NVSWITCH_FIRST());    \
        NVSWITCH_LOG_NONFATAL_DATA(device, _HW, _logenum,                      \
             link, 0, NV_TRUE, &report);                                       \
        if (nvswitch_lib_notify_client_events(device,                          \
            NVSWITCH_DEVICE_EVENT_NONFATAL) != NVL_SUCCESS)                    \
        {                                                                      \
            NVSWITCH_PRINT(device, ERROR, "%s: Failed to notify event\n",      \
                           __FUNCTION__);                                      \
        }                                                                      \
    } while(0)

// Log uncorrectable error that is not fatal to the fabric
#define NVSWITCH_REPORT_NONFATAL(_logenum, _str)                             \
    do                                                                       \
    {                                                                        \
        NVSWITCH_PRINT_SXID(device, NVSWITCH_ERR ## _logenum,                \
            "Non-fatal, Link %02d %s%s\n", link, _str, NVSWITCH_FIRST());    \
        NVSWITCH_LOG_NONFATAL_DATA(device, _HW, _logenum,                    \
            link, 0, NV_FALSE, &report, _str);                                     \
        if (nvswitch_lib_notify_client_events(device,                        \
            NVSWITCH_DEVICE_EVENT_NONFATAL) != NVL_SUCCESS)                  \
        {                                                                    \
            NVSWITCH_PRINT(device, ERROR, "%s: Failed to notify event\n",    \
                           __FUNCTION__);                                    \
        }                                                                    \
    } while(0)

// Log uncorrectable error that is fatal to the fabric
#define NVSWITCH_REPORT_FATAL(_logenum, _str, device_fatal)              \
    do                                                                   \
    {                                                                    \
        NVSWITCH_PRINT_SXID(device, NVSWITCH_ERR ## _logenum,            \
            "Fatal, Link %02d %s%s\n", link, _str, NVSWITCH_FIRST());    \
        NVSWITCH_LOG_FATAL_DATA(device, _HW, _logenum,                   \
            link, 0, NV_FALSE, &report, _str);                           \
        nvswitch_set_fatal_error(device, device_fatal, link);            \
        if (nvswitch_lib_notify_client_events(device,                    \
            NVSWITCH_DEVICE_EVENT_FATAL) != NVL_SUCCESS)                 \
        {                                                                \
            NVSWITCH_PRINT(device, ERROR, "%s: Failed to notify event\n",\
                           __FUNCTION__);                                \
        }                                                                \
    } while(0)

#define NVSWITCH_REPORT_PRI_ERROR_NONFATAL(_logenum, _str, instance, chiplet, err_data) \
    do                                                                                         \
    {                                                                                          \
        NVSWITCH_PRINT_SXID(device, NVSWITCH_ERR ## _logenum,                                  \
            "Non-fatal, %s, instance=%d, chiplet=%d\n", _str, instance, chiplet);              \
        NVSWITCH_LOG_NONFATAL_DATA(device, _HW, _logenum,                                      \
            instance, chiplet, NV_FALSE, &err_data, _str);                                     \
        if (nvswitch_lib_notify_client_events(device,                                          \
            NVSWITCH_DEVICE_EVENT_NONFATAL) != NVL_SUCCESS)                                    \
        {                                                                                      \
            NVSWITCH_PRINT(device, ERROR, "%s: Failed to notify event\n",                      \
                           __FUNCTION__);                                                      \
        }                                                                                      \
    } while(0)

#define NVSWITCH_REPORT_PRI_ERROR_FATAL(_logenum, _str, device_fatal, instance, chiplet, err_data) \
    do                                                                                      \
    {                                                                                       \
        NVSWITCH_PRINT_SXID(device, NVSWITCH_ERR ## _logenum,                               \
            "Fatal, %s, instance=%d, chiplet=%d\n", _str, instance, chiplet);               \
        NVSWITCH_LOG_FATAL_DATA(device, _HW, _logenum,                                      \
            instance, chiplet, NV_FALSE, &err_data, _str);                                  \
        nvswitch_set_fatal_error(device, device_fatal, 0);                                  \
        if (nvswitch_lib_notify_client_events(device,                                       \
            NVSWITCH_DEVICE_EVENT_FATAL) != NVL_SUCCESS)                                    \
        {                                                                                   \
            NVSWITCH_PRINT(device, ERROR, "%s: Failed to notify event\n",                   \
                           __FUNCTION__);                                                   \
        }                                                                                   \
    } while(0)

/*
 * Automatically determine if error is fatal to the fabric based on
 * if it is contained and will lock the port.
 */
#define NVSWITCH_REPORT_CONTAIN(_logenum, _str, device_fatal)       \
    do                                                              \
    {                                                               \
        if (bit & contain)                                          \
        {                                                           \
            NVSWITCH_REPORT_FATAL(_logenum, _str, device_fatal);    \
        }                                                           \
        else                                                        \
        {                                                           \
            NVSWITCH_REPORT_NONFATAL(_logenum, _str);               \
        }                                                           \
    } while (0)

/*
 * REPORT_*_DATA macros - optionally log data record for additional HW state. This
 * is typically a captured packet, but there are a few other cases.
 *
 * Most interrupt controllers only latch additional data for errors tagged as first.
 * For those cases use _FIRST to only log the data record when it is accurate.  If
 * two errors are detected in the same cycle, they will both be set in first.
 */
#define NVSWITCH_REPORT_DATA(_logenum, _data) \
    NVSWITCH_LOG_NONFATAL_DATA(device, _HW, _logenum, link, 0, NV_TRUE, &_data)

#define NVSWITCH_REPORT_DATA_FIRST(_logenum, _data) \
    do                                              \
    {                                               \
        if (report.raw_first & bit)                 \
        {                                           \
            NVSWITCH_REPORT_DATA(_logenum, _data);  \
        }                                           \
    } while(0)

#define NVSWITCH_REPORT_CONTAIN_DATA(_logenum, _data)               \
    do                                                              \
    {                                                               \
        if (bit & contain)                                          \
        {                                                           \
            NVSWITCH_LOG_FATAL_DATA(device, _HW, _logenum, link,    \
                                     0, NV_FALSE, &_data);          \
        }                                                           \
        else                                                        \
        {                                                           \
            NVSWITCH_LOG_NONFATAL_DATA(device, _HW, _logenum, link, \
                                       0, NV_FALSE, &_data);        \
        }                                                           \
    } while(0)

#define NVSWITCH_REPORT_CONTAIN_DATA_FIRST(_logenum, _data) \
    do                                                      \
    {                                                       \
        if (bit & report.raw_first)                         \
        {                                                   \
            NVSWITCH_REPORT_CONTAIN_DATA(_logenum, _data);  \
        }                                                   \
    } while(0)

#endif //_INTR_NVSWITCH_H_
