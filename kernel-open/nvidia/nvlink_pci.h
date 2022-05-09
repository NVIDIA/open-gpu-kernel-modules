/*
 * SPDX-FileCopyrightText: Copyright (c) 2015 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _NVLINK_PCI_H_
#define _NVLINK_PCI_H_

#include <linux/pci.h>
#include "nvlink_common.h"

#define NV_PCI_RESOURCE_START(dev, bar) pci_resource_start(dev, (bar))
#define NV_PCI_RESOURCE_SIZE(dev, bar)  pci_resource_len(dev, (bar))
#define NV_PCI_RESOURCE_FLAGS(dev, bar) pci_resource_flags(dev, (bar))

#if defined(NVCPU_X86)
#define NV_PCI_RESOURCE_VALID(dev, bar)                                      \
    ((NV_PCI_RESOURCE_START(dev, bar) != 0) &&                               \
     (NV_PCI_RESOURCE_SIZE(dev, bar) != 0) &&                                \
     (!((NV_PCI_RESOURCE_FLAGS(dev, bar) & PCI_BASE_ADDRESS_MEM_TYPE_64) &&  \
       ((NV_PCI_RESOURCE_START(dev, bar) >> PAGE_SHIFT) > 0xfffffULL))))
#else
#define NV_PCI_RESOURCE_VALID(dev, bar)                                      \
    ((NV_PCI_RESOURCE_START(dev, bar) != 0) &&                               \
     (NV_PCI_RESOURCE_SIZE(dev, bar) != 0))
#endif

#define NV_PCI_DOMAIN_NUMBER(dev)     (NvU32)pci_domain_nr(dev->bus)
#define NV_PCI_BUS_NUMBER(dev)        (dev)->bus->number
#define NV_PCI_DEVFN(dev)             (dev)->devfn
#define NV_PCI_SLOT_NUMBER(dev)       PCI_SLOT(NV_PCI_DEVFN(dev))

#define NV_PCI_DEV_FMT                NVLINK_PCI_DEV_FMT
#define NV_PCI_DEV_FMT_ARGS(dev)                        \
    NV_PCI_DOMAIN_NUMBER(dev), NV_PCI_BUS_NUMBER(dev),  \
    NV_PCI_SLOT_NUMBER(dev), PCI_FUNC((dev)->devfn)

#define NVRM_PCICFG_NUM_BARS            6
#define NVRM_PCICFG_BAR_OFFSET(i)       (0x10 + (i) * 4)

#define NV_PCIE_CFG_MAX_OFFSET 0x1000

#endif // _NVLINK_PCI_H_
