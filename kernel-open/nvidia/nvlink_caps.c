/*
 * SPDX-FileCopyrightText: Copyright (c) 2019 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "nvlink_os.h"
#include "nvlink_linux.h"
#include "nvlink_caps.h"
#include "nv-caps.h"

#define NVLINK_CAP_FABRIC_MGMT "fabric-mgmt"

typedef struct
{
    nv_cap_t *root;
    nv_cap_t *fabric_mgmt;
} nvlink_caps_t;

static nvlink_caps_t nvlink_caps = {0};

int nvlink_cap_acquire(int fd, NvU32 type)
{
    int dup_fd = -1;

    if (fd < 0)
    {
        return -1;
    }

    switch(type)
    {
        case NVLINK_CAP_FABRIC_MANAGEMENT:
        {
            dup_fd = nv_cap_validate_and_dup_fd(nvlink_caps.fabric_mgmt, fd);
            if (dup_fd < 0)
            {
                nvlink_print(NVLINK_DBG_ERRORS,
                             "Failed to validate the fabric mgmt capability\n");
                return -1;
            }
            break;
        }
        default:
        {
            nvlink_print(NVLINK_DBG_ERRORS, "Unknown capability specified\n");
            return -1;
        }
    }

    return dup_fd;
}

void nvlink_cap_release(int fd)
{
    if (fd < 0)
    {
        return;
    }

    nv_cap_close_fd(fd);
}

void nvlink_cap_exit(void)
{
    if (nvlink_caps.fabric_mgmt != NULL)
    {
        nv_cap_destroy_entry(nvlink_caps.fabric_mgmt);
        nvlink_caps.fabric_mgmt = NULL;
    }

    if (nvlink_caps.root != NULL)
    {
        nv_cap_destroy_entry(nvlink_caps.root);
        nvlink_caps.root = NULL;
    }
}

int nvlink_cap_init(const char *path)
{
    if (path == NULL)
    {
        nvlink_print(NVLINK_DBG_ERRORS, "Invalid path: %s\n", path);
        return -1;
    }

    nvlink_caps.root = nv_cap_init(path);
    if (nvlink_caps.root == NULL)
    {
        nvlink_print(NVLINK_DBG_ERRORS, "Failed to initialize capabilities\n");
        return -1;
    }

    nvlink_caps.fabric_mgmt = nv_cap_create_file_entry(nvlink_caps.root,
                                                       NVLINK_CAP_FABRIC_MGMT,
                                                       S_IRUSR);
    if (nvlink_caps.fabric_mgmt == NULL)
    {
        nvlink_print(NVLINK_DBG_ERRORS, "Failed to create fabric-mgmt entry\n");
        nvlink_cap_exit();
        return -1;
    }

    return 0;
}
