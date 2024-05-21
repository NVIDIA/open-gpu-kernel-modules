/*
 * SPDX-FileCopyrightText: Copyright (c) 2023-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

extern int NVreg_ImexChannelCount;
extern int NVreg_CreateImexChannel0;

static int nv_caps_imex_open(struct inode *inode, struct file *file)
{
    return 0;
}

static int nv_caps_imex_release(struct inode *inode, struct file *file)
{
    return 0;
}

static struct file_operations g_nv_caps_imex_fops =
{
    .owner = THIS_MODULE,
    .open    = nv_caps_imex_open,
    .release = nv_caps_imex_release
};

struct
{
    NvBool initialized;
    struct cdev cdev;
    dev_t devno;
} g_nv_caps_imex;

int NV_API_CALL nv_caps_imex_channel_get(int fd)
{
#if NV_FILESYSTEM_ACCESS_AVAILABLE
    struct file *file;
    struct inode *inode;
    int channel = -1;

    file = fget(fd);
    if (file == NULL)
    {
        return channel;
    }

    inode = NV_FILE_INODE(file);
    if (inode == NULL)
    {
        goto out;
    }

    /* Make sure the fd belongs to the nv-caps-imex-drv */
    if (file->f_op != &g_nv_caps_imex_fops)
    {
        goto out;
    }

    /* minor number is same as channel */
    channel = MINOR(inode->i_rdev);

out:
    fput(file);

    return channel;
#else
    return -1;
#endif
}

int NV_API_CALL nv_caps_imex_channel_count(void)
{
    return NVreg_ImexChannelCount;
}

int NV_API_CALL nv_caps_imex_init(void)
{
    int rc;

    if (g_nv_caps_imex.initialized)
    {
        nv_printf(NV_DBG_ERRORS, "nv-caps-imex is already initialized.\n");
        return -EBUSY;
    }

    if (NVreg_ImexChannelCount == 0)
    {
        nv_printf(NV_DBG_INFO, "nv-caps-imex is disabled.\n");

        // Disable channel creation as well
        NVreg_CreateImexChannel0 = 0;

        return 0;
    }

    rc = alloc_chrdev_region(&g_nv_caps_imex.devno, 0,
                             NVreg_ImexChannelCount,
                             "nvidia-caps-imex-channels");
    if (rc < 0)
    {
        nv_printf(NV_DBG_ERRORS, "nv-caps-imex failed to create cdev.\n");
        return rc;
    }

    cdev_init(&g_nv_caps_imex.cdev, &g_nv_caps_imex_fops);

    g_nv_caps_imex.cdev.owner = THIS_MODULE;

    rc = cdev_add(&g_nv_caps_imex.cdev, g_nv_caps_imex.devno,
                  NVreg_ImexChannelCount);
    if (rc < 0)
    {
        nv_printf(NV_DBG_ERRORS, "nv-caps-imex failed to add cdev.\n");
        goto cdev_add_fail;
    }

    g_nv_caps_imex.initialized = NV_TRUE;

    return 0;

cdev_add_fail:
    unregister_chrdev_region(g_nv_caps_imex.devno, NVreg_ImexChannelCount);

    return rc;
}

void NV_API_CALL nv_caps_imex_exit(void)
{
    if (!g_nv_caps_imex.initialized)
    {
        return;
    }

    cdev_del(&g_nv_caps_imex.cdev);

    unregister_chrdev_region(g_nv_caps_imex.devno, NVreg_ImexChannelCount);

    g_nv_caps_imex.initialized = NV_FALSE;
}
