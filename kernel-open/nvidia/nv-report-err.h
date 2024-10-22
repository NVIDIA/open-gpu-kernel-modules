/*
 * SPDX-FileCopyrightText: Copyright (c) 2017-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _NV_REPORT_ERR_H_
#define _NV_REPORT_ERR_H_

/*
 * @brief
 *   Callback definition for obtaining XID error string and data.
 *
 * @param[in]    pci_dev *
 *   Structure describring GPU PCI device.
 * @param[in]    uint32_t
 *   XID number
 * @param[in]    char *
 *   Error string with HWERR info.
 * @param[in]    int
 *   Length of error string.
 */
typedef void (*nv_report_error_cb_t)(struct pci_dev *, uint32_t, char *, size_t);

/*
 * @brief
 *   Register callback function to obtain XID error string and data.
 *
 * @param[in]    report_error_cb
 *   A function pointer to recieve callback.
 *
 * @return
 *   0           upon successful completion.
 *   -EINVAL     callback handle is NULL.
 *   -EBUSY      callback handle is already registered.
 */
int nvidia_register_error_cb(nv_report_error_cb_t report_error_cb);

/*
 * @brief
 *   Unregisters callback function handle.
 *
 * @return
 *   0           upon successful completion.
 *   -EPERM      unregister not permitted on NULL callback handle.
 */
int nvidia_unregister_error_cb(void);

#endif /* _NV_REPORT_ERR_H_ */
