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

#ifndef _NV_RSYNC_H_
#define _NV_RSYNC_H_

#include "nv-linux.h"

typedef struct nv_rsync_info
{
    struct semaphore lock;
    uint32_t usage_count;
    NvBool relaxed_ordering_mode;
    int (*get_relaxed_ordering_mode)(int *mode, void *data);
    void (*put_relaxed_ordering_mode)(int mode, void *data);
    void (*wait_for_rsync)(struct pci_dev *gpu, void *data);
    void *data;
} nv_rsync_info_t;

void nv_init_rsync_info(void);
void nv_destroy_rsync_info(void);
int nv_get_rsync_info(void);
void nv_put_rsync_info(void);
int nv_register_rsync_driver(
                        int (*get_relaxed_ordering_mode)(int *mode, void *data),
                        void (*put_relaxed_ordering_mode)(int mode, void *data),
                        void (*wait_for_rsync)(struct pci_dev *gpu, void *data),
                        void *data);
void nv_unregister_rsync_driver(
                        int (*get_relaxed_ordering_mode)(int *mode, void *data),
                        void (*put_relaxed_ordering_mode)(int mode, void *data),
                        void (*wait_for_rsync)(struct pci_dev *gpu, void *data),
                        void *data);
NvBool nv_get_rsync_relaxed_ordering_mode(nv_state_t *nv);
void nv_wait_for_rsync(nv_state_t *nv);

#endif
