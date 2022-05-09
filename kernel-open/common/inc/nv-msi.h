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

#ifndef _NV_MSI_H_
#define _NV_MSI_H_

#include "nv-linux.h"

#if (defined(CONFIG_X86_LOCAL_APIC) || defined(NVCPU_AARCH64) || \
     defined(NVCPU_PPC64LE)) && \
    (defined(CONFIG_PCI_MSI) || defined(CONFIG_PCI_USE_VECTOR))
#define NV_LINUX_PCIE_MSI_SUPPORTED
#endif

#if !defined(NV_LINUX_PCIE_MSI_SUPPORTED) || !defined(CONFIG_PCI_MSI)
#define NV_PCI_DISABLE_MSI(pci_dev)
#else
#define NV_PCI_DISABLE_MSI(pci_dev)     pci_disable_msi(pci_dev)
#endif

irqreturn_t     nvidia_isr              (int, void *);
irqreturn_t     nvidia_isr_msix         (int, void *);
irqreturn_t     nvidia_isr_kthread_bh   (int, void *);
irqreturn_t     nvidia_isr_msix_kthread_bh(int, void *);

#if defined(NV_LINUX_PCIE_MSI_SUPPORTED)
void    NV_API_CALL nv_init_msi         (nv_state_t *);
void    NV_API_CALL nv_init_msix        (nv_state_t *);
NvS32   NV_API_CALL nv_request_msix_irq (nv_linux_state_t *);

#define NV_PCI_MSIX_FLAGS         2
#define NV_PCI_MSIX_FLAGS_QSIZE   0x7FF

static inline void nv_free_msix_irq(nv_linux_state_t *nvl)
{
    int i;

    for (i = 0; i < nvl->num_intr; i++)
    {
        free_irq(nvl->msix_entries[i].vector, (void *)nvl);
    }
}

static inline int nv_get_max_irq(struct pci_dev *pci_dev)
{
    int nvec;
    int cap_ptr;
    NvU16 ctrl;

    cap_ptr = pci_find_capability(pci_dev, PCI_CAP_ID_MSIX);
    /*
     * The 'PCI_MSIX_FLAGS' was added in 2.6.21-rc3 by:
     * 2007-03-05 f5f2b13129a6541debf8851bae843cbbf48298b7
     */
#if defined(PCI_MSIX_FLAGS)
    pci_read_config_word(pci_dev, cap_ptr + PCI_MSIX_FLAGS, &ctrl);
    nvec = (ctrl & PCI_MSIX_FLAGS_QSIZE) + 1;
#else
    pci_read_config_word(pci_dev, cap_ptr + NV_PCI_MSIX_FLAGS, &ctrl);
    nvec = (ctrl & NV_PCI_MSIX_FLAGS_QSIZE) + 1;
#endif

    return nvec;
}

static inline int nv_pci_enable_msix(nv_linux_state_t *nvl, int nvec)
{
    int rc = 0;

    /*
     * pci_enable_msix_range() replaced pci_enable_msix() in 3.14-rc1:
     * 2014-01-03  302a2523c277bea0bbe8340312b09507905849ed
     */

#if defined(NV_PCI_ENABLE_MSIX_RANGE_PRESENT)
    // We require all the vectors we are requesting so use the same min and max
    rc = pci_enable_msix_range(nvl->pci_dev, nvl->msix_entries, nvec, nvec);
    if (rc < 0)
    {
        return NV_ERR_OPERATING_SYSTEM;
    }
    WARN_ON(nvec != rc);
#else
    rc = pci_enable_msix(nvl->pci_dev, nvl->msix_entries, nvec);
    if (rc != 0)
    {
        return NV_ERR_OPERATING_SYSTEM;
    }
#endif

    nvl->num_intr = nvec;
    return NV_OK;
}
#endif
#endif  /* _NV_MSI_H_ */
