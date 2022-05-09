/*
 * SPDX-FileCopyrightText: Copyright (c) 2017 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#define  __NO_VERSION__
#include "nv-linux.h"
#include "os-interface.h"

#include "nv-report-err.h"

nv_report_error_cb_t nv_error_cb_handle = NULL;

int nv_register_error_cb(nv_report_error_cb_t report_error_cb)
{
    if (report_error_cb == NULL)
        return -EINVAL;

    if (nv_error_cb_handle != NULL)
        return -EBUSY;

    nv_error_cb_handle = report_error_cb;
    return 0;
}

EXPORT_SYMBOL(nv_register_error_cb);

int nv_unregister_error_cb(void)
{
    if (nv_error_cb_handle == NULL)
        return -EPERM;

    nv_error_cb_handle = NULL;
    return 0;
}

EXPORT_SYMBOL(nv_unregister_error_cb);

struct pci_dev;

void nv_report_error(
    struct pci_dev *dev,
    NvU32           error_number,
    const char     *format,
    va_list         ap
)
{
    va_list ap_copy;
    char *buffer;
    int length = 0;
    int status = NV_OK;

    if (nv_error_cb_handle != NULL)
    {
        va_copy(ap_copy, ap);
        length = vsnprintf(NULL, 0, format, ap);
        va_end(ap_copy);

        if (length > 0)
        {
            status = os_alloc_mem((void *)&buffer, (length + 1)*sizeof(char));

            if (status == NV_OK)
            {
                vsnprintf(buffer, length, format, ap);
                nv_error_cb_handle(dev, error_number, buffer, length + 1);
                os_free_mem(buffer);
            }
        }
    }
}
