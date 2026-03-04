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

#include "nv-linux.h"
#include "nv-rsync.h"

nv_rsync_info_t g_rsync_info;

void nv_init_rsync_info(
    void
)
{
    g_rsync_info.relaxed_ordering_mode = NV_FALSE;
    g_rsync_info.usage_count = 0;
    g_rsync_info.data = NULL;
    NV_INIT_MUTEX(&g_rsync_info.lock);
}

void nv_destroy_rsync_info(
    void
)
{
    WARN_ON(g_rsync_info.data);
    WARN_ON(g_rsync_info.usage_count);
    WARN_ON(g_rsync_info.relaxed_ordering_mode);
}

int nv_get_rsync_info(
    void
)
{
    int mode;
    int rc = 0;

    down(&g_rsync_info.lock);

    if (g_rsync_info.usage_count == 0)
    {
        if (g_rsync_info.get_relaxed_ordering_mode)
        {
            rc = g_rsync_info.get_relaxed_ordering_mode(&mode,
                                                        g_rsync_info.data);
            if (rc != 0)
            {
                goto done;
            }

            g_rsync_info.relaxed_ordering_mode = !!mode;
        }
    }

    g_rsync_info.usage_count++;

done:
    up(&g_rsync_info.lock);

    return rc;
}

void nv_put_rsync_info(
    void
)
{
    int mode;

    down(&g_rsync_info.lock);

    g_rsync_info.usage_count--;

    if (g_rsync_info.usage_count == 0)
    {
        if (g_rsync_info.put_relaxed_ordering_mode)
        {
            mode = g_rsync_info.relaxed_ordering_mode;
            g_rsync_info.put_relaxed_ordering_mode(mode, g_rsync_info.data);
            g_rsync_info.relaxed_ordering_mode = NV_FALSE;
        }
    }

    up(&g_rsync_info.lock);
}

int nv_register_rsync_driver(
    int (*get_relaxed_ordering_mode)(int *mode, void *data),
    void (*put_relaxed_ordering_mode)(int mode, void *data),
    void (*wait_for_rsync)(struct pci_dev *gpu, void *data),
    void *data
)
{
    int rc = 0;

    down(&g_rsync_info.lock);

    if (g_rsync_info.get_relaxed_ordering_mode != NULL)
    {
        rc = -EBUSY;
        goto done;
    }

    if (g_rsync_info.usage_count != 0)
    {
        rc = -EBUSY;
        goto done;
    }

    g_rsync_info.get_relaxed_ordering_mode = get_relaxed_ordering_mode;
    g_rsync_info.put_relaxed_ordering_mode = put_relaxed_ordering_mode;
    g_rsync_info.wait_for_rsync = wait_for_rsync;
    g_rsync_info.data = data;

done:
    up(&g_rsync_info.lock);

    return rc;
}

void nv_unregister_rsync_driver(
    int (*get_relaxed_ordering_mode)(int *mode, void *data),
    void (*put_relaxed_ordering_mode)(int mode, void *data),
    void (*wait_for_rsync)(struct pci_dev *gpu, void *data),
    void *data
)
{
    down(&g_rsync_info.lock);

    WARN_ON(g_rsync_info.usage_count != 0);

    WARN_ON(g_rsync_info.get_relaxed_ordering_mode !=
            get_relaxed_ordering_mode);
    WARN_ON(g_rsync_info.put_relaxed_ordering_mode !=
            put_relaxed_ordering_mode);
    WARN_ON(g_rsync_info.wait_for_rsync != wait_for_rsync);
    WARN_ON(g_rsync_info.data != data);

    g_rsync_info.get_relaxed_ordering_mode = NULL;
    g_rsync_info.put_relaxed_ordering_mode = NULL;
    g_rsync_info.wait_for_rsync = NULL;
    g_rsync_info.data = NULL;

    up(&g_rsync_info.lock);
}
