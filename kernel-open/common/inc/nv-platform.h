/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef NV_PLATFORM_H
#define NV_PLATFORM_H

#include "nv-linux.h"

irqreturn_t     nvidia_isr              (int, void *);
irqreturn_t     nvidia_isr_kthread_bh   (int, void *);

int       nv_platform_register_driver(void);
void      nv_platform_unregister_driver(void);
int       nv_platform_count_devices(void);
int       nv_soc_register_irqs(nv_state_t *nv);
void      nv_soc_free_irqs(nv_state_t *nv);

#define NV_SUPPORTS_PLATFORM_DEVICE NV_IS_EXPORT_SYMBOL_PRESENT___platform_driver_register

#if defined(NV_LINUX_PLATFORM_TEGRA_DCE_DCE_CLIENT_IPC_H_PRESENT)
#define NV_SUPPORTS_DCE_CLIENT_IPC 1
#else
#define NV_SUPPORTS_DCE_CLIENT_IPC 0
#endif

#define NV_SUPPORTS_PLATFORM_DISPLAY_DEVICE (NV_SUPPORTS_PLATFORM_DEVICE && NV_SUPPORTS_DCE_CLIENT_IPC)

#endif
