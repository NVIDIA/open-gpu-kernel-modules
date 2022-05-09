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

#ifndef _NV_PCI_H_
#define _NV_PCI_H_

#include <linux/pci.h>
#include "nv-linux.h"

#if defined(NV_DEV_IS_PCI_PRESENT)
#define nv_dev_is_pci(dev) dev_is_pci(dev)
#else
/*
 * Non-PCI devices are only supported on kernels which expose the
 * dev_is_pci() function. For older kernels, we only support PCI
 * devices, hence returning true to take all the PCI code paths.
 */
#define nv_dev_is_pci(dev) (true)
#endif

int nv_pci_register_driver(void);
void nv_pci_unregister_driver(void);
int nv_pci_count_devices(void);
NvU8 nv_find_pci_capability(struct pci_dev *, NvU8);
int nvidia_dev_get_pci_info(const NvU8 *, struct pci_dev **, NvU64 *, NvU64 *);
nv_linux_state_t * find_pci(NvU32, NvU8, NvU8, NvU8);

#endif
