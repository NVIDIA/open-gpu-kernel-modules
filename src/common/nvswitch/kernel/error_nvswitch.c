/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2019 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "common_nvswitch.h"
#include "error_nvswitch.h"

#define NVSWITCH_DATE_LEN    64

//
// Error logging
//
static void
_nvswitch_dump_error_entry
(
    nvswitch_device *device,
    NvU32   error_count,
    NVSWITCH_ERROR_TYPE *error_entry
)
{
    if ((error_entry != NULL) &&
        (error_entry->error_src == NVSWITCH_ERROR_SRC_HW))
    {
        NVSWITCH_PRINT_SXID_NO_BBX(device, error_entry->error_type,
            "Severity %d Engine instance %02d Sub-engine instance %02d\n",
            error_entry->severity, error_entry->instance, error_entry->subinstance);

        NVSWITCH_PRINT_SXID_NO_BBX(device, error_entry->error_type,
            "Data {0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x}\n",
            error_entry->data.raw.flags,
            error_entry->data.raw.data[0], error_entry->data.raw.data[1],
            error_entry->data.raw.data[2], error_entry->data.raw.data[3],
            error_entry->data.raw.data[4], error_entry->data.raw.data[5],
            error_entry->data.raw.data[6], error_entry->data.raw.data[7]);

        if ((error_entry->data.raw.data[ 8] != 0) ||
            (error_entry->data.raw.data[ 9] != 0) ||
            (error_entry->data.raw.data[10] != 0) ||
            (error_entry->data.raw.data[11] != 0) ||
            (error_entry->data.raw.data[12] != 0) ||
            (error_entry->data.raw.data[13] != 0) ||
            (error_entry->data.raw.data[14] != 0) ||
            (error_entry->data.raw.data[15] != 0))

        {
            NVSWITCH_PRINT_SXID_NO_BBX(device, error_entry->error_type,
                "Data {0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x}\n",
                error_entry->data.raw.data[ 8], error_entry->data.raw.data[ 9],
                error_entry->data.raw.data[10], error_entry->data.raw.data[11],
                error_entry->data.raw.data[12], error_entry->data.raw.data[13],
                error_entry->data.raw.data[14], error_entry->data.raw.data[15]);
        }
    }
}

//
// Construct an error log
//
// If error_log_size > 0 a circular buffer is created to record errors
//
NvlStatus
nvswitch_construct_error_log
(
    NVSWITCH_ERROR_LOG_TYPE *errors,
    NvU32 error_log_size,
    NvBool overwritable
)
{
    NvlStatus retval = NVL_SUCCESS;

    NVSWITCH_ASSERT(errors != NULL);

    errors->error_start = 0;
    errors->error_count = 0;
    errors->error_total = 0;
    errors->error_log_size = 0;
    errors->error_log = NULL;
    errors->overwritable = overwritable;

    if (error_log_size > 0)
    {
        errors->error_log = nvswitch_os_malloc(error_log_size * sizeof(NVSWITCH_ERROR_TYPE));
    }

    if (errors->error_log != NULL)
    {
        errors->error_log_size = error_log_size;
        nvswitch_os_memset(errors->error_log, 0, errors->error_log_size * sizeof(NVSWITCH_ERROR_TYPE));
    }

    if (error_log_size != errors->error_log_size)
    {
        retval = -NVL_NO_MEM;
    }

    return retval;
}

//
// Destroy an error log
//
void
nvswitch_destroy_error_log
(
    nvswitch_device *device,
    NVSWITCH_ERROR_LOG_TYPE *errors
)
{
    if (errors == NULL)
        return;

    errors->error_start = 0;
    errors->error_count = 0;
    //errors->error_total = 0;       // Don't reset total count of errors logged
    errors->error_log_size = 0;

    if (errors->error_log != NULL)
    {
        nvswitch_os_free(errors->error_log);
        errors->error_log = NULL;
    }
}

void
nvswitch_record_error
(
    nvswitch_device             *device,
    NVSWITCH_ERROR_LOG_TYPE     *errors,
    NvU32                        error_type, // NVSWITCH_ERR_*
    NvU32                        instance,
    NvU32                        subinstance,
    NVSWITCH_ERROR_SRC_TYPE      error_src,  // NVSWITCH_ERROR_SRC_*
    NVSWITCH_ERROR_SEVERITY_TYPE severity,   // NVSWITCH_ERROR_SEVERITY_*
    NvBool                       error_resolved,
    void                        *data,
    NvU32                        data_size,
    NvU32                        line,
    const char                  *description
)
{
    NvU32 idx_error;
    NvU32 description_len = (NvU32)nvswitch_os_strlen(description) - 2; //take out leading and trailing quotation

    NVSWITCH_ASSERT(errors != NULL);
    NVSWITCH_ASSERT(data_size <= sizeof(errors->error_log[idx_error].data));

    if (description_len > NVSWITCH_ERROR_MAX_DESCRPTION_LEN)
    {
        description_len = NVSWITCH_ERROR_MAX_DESCRPTION_LEN;
    }

    // If no error log has been created, don't log it.
    if ((errors->error_log_size != 0) && (errors->error_log != NULL))
    {
        idx_error = (errors->error_start + errors->error_count) % errors->error_log_size;

        if (errors->error_count == errors->error_log_size)
        {
            // Error ring buffer already full.
            if (errors->overwritable)
            {
                errors->error_start = (errors->error_start + 1) % errors->error_log_size;
            }
            else
            {
                // Return: ring buffer full
                return;
            }
        }
        else
        {
            errors->error_count++;
        }

        // Log error info
        errors->error_log[idx_error].error_type = error_type;
        errors->error_log[idx_error].instance   = instance;
        errors->error_log[idx_error].subinstance = subinstance;
        errors->error_log[idx_error].error_src  = error_src;
        errors->error_log[idx_error].severity   = severity;
        errors->error_log[idx_error].error_resolved = error_resolved;
        errors->error_log[idx_error].line       = line;
        errors->error_log[idx_error].data_size  = data_size;
        nvswitch_os_memcpy(&errors->error_log[idx_error].description, description + 1, description_len);

        // Log tracking info
        errors->error_log[idx_error].timer_count = nvswitch_hw_counter_read_counter(device);
        errors->error_log[idx_error].time = nvswitch_os_get_platform_time();
        errors->error_log[idx_error].local_error_num  = errors->error_total;
        errors->error_log[idx_error].global_error_num = device->error_total;

        // Copy ancillary data blob
        nvswitch_os_memset(&errors->error_log[idx_error].data, 0, sizeof(errors->error_log[idx_error].data));
        if ((data != NULL) && (data_size > 0))
        {
            nvswitch_os_memcpy(&errors->error_log[idx_error].data, data, data_size);
        }

        _nvswitch_dump_error_entry(device, idx_error, &errors->error_log[idx_error]);
    }
    errors->error_total++;
    device->error_total++;
}

//
// Discard N errors from the specified log
//

void
nvswitch_discard_errors
(
    NVSWITCH_ERROR_LOG_TYPE *errors,
    NvU32 error_discard_count
)
{
    error_discard_count = NV_MIN(error_discard_count, errors->error_count);
    errors->error_start = (errors->error_start+error_discard_count) % errors->error_log_size;
    errors->error_count -= error_discard_count;
}

//
// Retrieve an error entry by index.
// 0 = oldest error
// Out-of-range index does not return an error, but does return an error of type "NO_ERROR"
// error_count returns how many errors in the error log
//

void
nvswitch_get_error
(
    nvswitch_device *device,
    NVSWITCH_ERROR_LOG_TYPE *errors,
    NVSWITCH_ERROR_TYPE *error_entry,
    NvU32   error_idx,
    NvU32   *error_count
)
{
    NVSWITCH_ASSERT(errors != NULL);

    if (error_entry != NULL)
    {
        if (error_idx >= errors->error_count)
        {
            // Index out-of-range
            nvswitch_os_memset(error_entry, 0, sizeof(*error_entry));
            error_entry->error_type = 0;
            error_entry->instance   = 0;
            error_entry->subinstance = 0;
            error_entry->local_error_num  = errors->error_total;
            error_entry->global_error_num  = ((device == NULL) ? 0 : device->error_total);
            error_entry->error_src  = NVSWITCH_ERROR_SRC_NONE;
            error_entry->severity   = NVSWITCH_ERROR_SEVERITY_NONFATAL;
            error_entry->error_resolved = NV_TRUE;
            error_entry->line = 0;
            error_entry->timer_count = 
                ((device == NULL) ? 0 : nvswitch_hw_counter_read_counter(device));
            error_entry->time = nvswitch_os_get_platform_time();
            error_entry->data_size = 0;
        }
        else
        {
            *error_entry = errors->error_log[(errors->error_start + error_idx) % errors->error_log_size];
        }
    }

    if (error_count)
    {
        *error_count = errors->error_count;
    }
}


//
// Retrieve the oldest logged error entry.
// Optionally remove the error entry after reading
// error_count returns how many remaining errors in the error log
//

void
nvswitch_get_next_error
(
    nvswitch_device *device,
    NVSWITCH_ERROR_LOG_TYPE *errors,
    NVSWITCH_ERROR_TYPE *error_entry,
    NvU32   *error_count,
    NvBool  remove_from_list
)
{
    nvswitch_get_error(device, errors, error_entry, 0, error_count);

    // Optionally remove the error from the log
    if (remove_from_list)
    {
        nvswitch_discard_errors(errors, 1);
    }
}

NVSWITCH_NVLINK_HW_ERROR
nvswitch_translate_hw_error
(
    NVSWITCH_ERR_TYPE type
)
{
    if ((type >= NVSWITCH_ERR_HW_NPORT_INGRESS) &&
        (type <  NVSWITCH_ERR_HW_NPORT_INGRESS_LAST))
    {
        return NVSWITCH_NVLINK_HW_INGRESS;
    }
    else if ((type >= NVSWITCH_ERR_HW_NPORT_EGRESS) &&
             (type <  NVSWITCH_ERR_HW_NPORT_EGRESS_LAST))
    {
        return NVSWITCH_NVLINK_HW_EGRESS;
    }
    else if ((type >= NVSWITCH_ERR_HW_NPORT_FSTATE) &&
             (type <  NVSWITCH_ERR_HW_NPORT_FSTATE_LAST))
    {
        return NVSWITCH_NVLINK_HW_FSTATE;
    }
    else if ((type >= NVSWITCH_ERR_HW_NPORT_TSTATE) &&
             (type <  NVSWITCH_ERR_HW_NPORT_TSTATE_LAST))
    {
        return NVSWITCH_NVLINK_HW_TSTATE;
    }
    else if ((type >= NVSWITCH_ERR_HW_NPORT_ROUTE) &&
             (type <  NVSWITCH_ERR_HW_NPORT_ROUTE_LAST))
    {
        return NVSWITCH_NVLINK_HW_ROUTE;
    }
    else if ((type >= NVSWITCH_ERR_HW_NPORT) &&
             (type <  NVSWITCH_ERR_HW_NPORT_LAST))
    {
        return NVSWITCH_NVLINK_HW_NPORT;
    }
    else if ((type >= NVSWITCH_ERR_HW_NVLCTRL) &&
             (type <  NVSWITCH_ERR_HW_NVLCTRL_LAST))
    {
        return NVSWITCH_NVLINK_HW_NVLCTRL;
    }
    else if ((type >= NVSWITCH_ERR_HW_NVLIPT) &&
             (type <  NVSWITCH_ERR_HW_NVLIPT_LAST))
    {
        return NVSWITCH_NVLINK_HW_NVLIPT;
    }
    else if ((type >= NVSWITCH_ERR_HW_NVLTLC) &&
             (type <  NVSWITCH_ERR_HW_NVLTLC_LAST))
    {
        return NVSWITCH_NVLINK_HW_NVLTLC;
    }
    else if ((type >= NVSWITCH_ERR_HW_DLPL) &&
            (type <  NVSWITCH_ERR_HW_DLPL_LAST))
    {
        return NVSWITCH_NVLINK_HW_DLPL;
    }
    else if ((type >= NVSWITCH_ERR_HW_AFS) &&
             (type <  NVSWITCH_ERR_HW_AFS_LAST))
    {
        return NVSWITCH_NVLINK_HW_AFS;
    }
    else if ((type >= NVSWITCH_ERR_HW_HOST) &&
             (type <  NVSWITCH_ERR_HW_HOST_LAST))
    {
        return NVSWITCH_NVLINK_HW_HOST;
    }
    else if ((type >= NVSWITCH_ERR_HW_MINION) &&
             (type <  NVSWITCH_ERR_HW_MINION_LAST))
    {
        return NVSWITCH_NVLINK_HW_MINION;
    }
    else if ((type >= NVSWITCH_ERR_HW_NXBAR) &&
             (type <  NVSWITCH_ERR_HW_NXBAR_LAST))
    {
        return NVSWITCH_NVLINK_HW_NXBAR;
    }
    else if ((type >= NVSWITCH_ERR_HW_NPORT_SOURCETRACK) &&
             (type < NVSWITCH_ERR_HW_NPORT_SOURCETRACK_LAST))
    {
        return NVSWITCH_NVLINK_HW_SOURCETRACK;
    }
    else if ((type >= NVSWITCH_ERR_HW_NVLIPT_LNK) &&
             (type < NVSWITCH_ERR_HW_NVLIPT_LNK_LAST))
    {
        return NVSWITCH_ERR_HW_NVLIPT_LNK;
    }
    else if ((type >= NVSWITCH_ERR_HW_SOE) &&
             (type < NVSWITCH_ERR_HW_SOE_LAST))
    {
        return NVSWITCH_ERR_HW_SOE;
    }
    else if ((type >= NVSWITCH_ERR_HW_CCI) &&
             (type < NVSWITCH_ERR_HW_CCI_LAST))
    {
        return NVSWITCH_ERR_HW_CCI;
    }
    else if ((type >= NVSWITCH_ERR_HW_OSFP_THERM) &&
             (type < NVSWITCH_ERR_HW_OSFP_THERM_LAST))
    {
        return NVSWITCH_ERR_HW_OSFP_THERM;
    }
    else if ((type >= NVSWITCH_ERR_HW_NPORT_MULTICASTTSTATE) &&
             (type < NVSWITCH_ERR_HW_NPORT_MULTICASTTSTATE_LAST))
    {
        return NVSWITCH_ERR_HW_NPORT_MULTICASTTSTATE;
    }
    else if ((type >= NVSWITCH_ERR_HW_NPORT_REDUCTIONTSTATE) &&
             (type < NVSWITCH_ERR_HW_NPORT_REDUCTIONTSTATE_LAST))
    {
        return NVSWITCH_ERR_HW_NPORT_REDUCTIONTSTATE;
    }
    else
    {
        // Update this assert after adding a new translation entry above
        ct_assert(NVSWITCH_ERR_HW_NPORT_REDUCTIONTSTATE_LAST == (NVSWITCH_ERR_LAST - 1));

        NVSWITCH_PRINT(NULL, ERROR,
            "%s: Undefined error type\n", __FUNCTION__);
        NVSWITCH_ASSERT(0);
        return NVSWITCH_NVLINK_HW_GENERIC;
    }
}

static NVSWITCH_NVLINK_ARCH_ERROR
_nvswitch_translate_arch_error
(
    NVSWITCH_ERROR_TYPE *error_entry
)
{
    if (error_entry->severity == NVSWITCH_ERROR_SEVERITY_FATAL)
    {
        return NVSWITCH_NVLINK_ARCH_ERROR_HW_FATAL;
    }
    else if (error_entry->severity == NVSWITCH_ERROR_SEVERITY_NONFATAL)
    {
        if (error_entry->error_resolved)
        {
            return NVSWITCH_NVLINK_ARCH_ERROR_HW_CORRECTABLE;
        }
        else
        {
            return NVSWITCH_NVLINK_ARCH_ERROR_HW_UNCORRECTABLE;
        }
    }

    return NVSWITCH_NVLINK_ARCH_ERROR_GENERIC;
}

void
nvswitch_translate_error
(
    NVSWITCH_ERROR_TYPE         *error_entry,
    NVSWITCH_NVLINK_ARCH_ERROR  *arch_error,
    NVSWITCH_NVLINK_HW_ERROR    *hw_error
)
{
    NVSWITCH_ASSERT(error_entry != NULL);

    if (arch_error)
    {
        *arch_error = NVSWITCH_NVLINK_ARCH_ERROR_NONE;
    }

    if (hw_error)
    {
        *hw_error = NVSWITCH_NVLINK_HW_ERROR_NONE;
    }

    if (error_entry->error_src == NVSWITCH_ERROR_SRC_HW)
    {
        if (arch_error)
        {
            *arch_error = _nvswitch_translate_arch_error(error_entry);
        }

        if (hw_error)
        {
            *hw_error = nvswitch_translate_hw_error(error_entry->error_type);
        }
    }
    else
    {
        NVSWITCH_PRINT(NULL, ERROR,
            "%s: Undefined error source\n", __FUNCTION__);
        NVSWITCH_ASSERT(0);
    }
}

NvlStatus
nvswitch_ctrl_get_errors
(
    nvswitch_device *device,
    NVSWITCH_GET_ERRORS_PARAMS *p
)
{
    NvU32 index = 0;
    NvU32 count = 0;
    NVSWITCH_ERROR_LOG_TYPE *error_log;
    NVSWITCH_ERROR_TYPE error;

    switch (p->errorType)
    {
        case NVSWITCH_ERROR_SEVERITY_FATAL:
            error_log = &device->log_FATAL_ERRORS;
            break;
        case NVSWITCH_ERROR_SEVERITY_NONFATAL:
            error_log = &device->log_NONFATAL_ERRORS;
            break;
        default:
            return -NVL_BAD_ARGS;
    }

    nvswitch_os_memset(p->error, 0, sizeof(NVSWITCH_ERROR) *
                       NVSWITCH_ERROR_COUNT_SIZE);
    p->nextErrorIndex = NVSWITCH_ERROR_NEXT_LOCAL_NUMBER(error_log);
    p->errorCount = 0;

    // If there is nothing to do, return.
    nvswitch_get_error(device, error_log, &error, index, &count);
    if (count == 0)
    {
        return NVL_SUCCESS;
    }

    //
    // If the error's local_error_num is smaller than the errorIndex
    // passed in by the client, fast-forward index by the difference.
    // This will skip over errors that were previously read by the client.
    //
    if (error.local_error_num < p->errorIndex)
    {
        index = (NvU32) (p->errorIndex - error.local_error_num);
    }

   // If there is nothing to do after fast-forwarding, return.
   if (index >= count)
   {
      return NVL_SUCCESS;
   }

    while ((p->errorCount < NVSWITCH_ERROR_COUNT_SIZE) && (index < count))
    {
        // Get the next error to consider from the log
        nvswitch_get_error(device, error_log, &error, index, NULL);

        p->error[p->errorCount].error_value = error.error_type;
        p->error[p->errorCount].error_src = error.error_src;
        p->error[p->errorCount].instance = error.instance;
        p->error[p->errorCount].subinstance = error.subinstance;
        p->error[p->errorCount].time = error.time;
        p->error[p->errorCount].error_resolved = error.error_resolved;
        p->error[p->errorCount].error_data_size = error.data_size;
        if (error.data_size > 0)
        {
            nvswitch_os_memcpy(p->error[p->errorCount].error_data, error.data.raw.data, error.data_size);
        }
        nvswitch_os_memcpy(p->error[p->errorCount].error_description, error.description, sizeof(error.description));
        
        p->errorCount++;
        index++;
    }

    p->errorIndex = error.local_error_num + 1;

    return NVL_SUCCESS;
}
