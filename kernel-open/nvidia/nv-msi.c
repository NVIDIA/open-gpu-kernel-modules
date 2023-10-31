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

#include "nv-msi.h"
#include "nv-proto.h"

#if defined(NV_LINUX_PCIE_MSI_SUPPORTED)
void NV_API_CALL nv_init_msi(nv_state_t *nv)
{
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);
    int rc = 0;

    rc = pci_enable_msi(nvl->pci_dev);
    if (rc == 0)
    {
        nv->interrupt_line = nvl->pci_dev->irq;
        nv->flags |= NV_FLAG_USES_MSI;
        nvl->num_intr = 1;
        NV_KZALLOC(nvl->irq_count, sizeof(nv_irq_count_info_t) * nvl->num_intr);

        if (nvl->irq_count == NULL)
        {
            nv->flags &= ~NV_FLAG_USES_MSI;
            NV_DEV_PRINTF(NV_DBG_ERRORS, nv,
                      "Failed to allocate counter for MSI entry; "
                      "falling back to PCIe virtual-wire interrupts.\n");
        }
        else
        {
            nvl->current_num_irq_tracked = 0;
        }
    }
    else
    {
        nv->flags &= ~NV_FLAG_USES_MSI;
        if (nvl->pci_dev->irq != 0)
        {
            NV_DEV_PRINTF(NV_DBG_ERRORS, nv,
                      "Failed to enable MSI; "
                      "falling back to PCIe virtual-wire interrupts.\n");
        }
    }

    return;
}

void NV_API_CALL nv_init_msix(nv_state_t *nv)
{
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);
    int num_intr = 0;
    struct msix_entry *msix_entries;
    int rc = 0;
    int i;

    NV_SPIN_LOCK_INIT(&nvl->msix_isr_lock);

    rc = os_alloc_mutex(&nvl->msix_bh_mutex);
    if (rc != 0)
        goto failed;

    num_intr = nv_get_max_irq(nvl->pci_dev);

    if (num_intr > NV_RM_MAX_MSIX_LINES)
    {
        NV_DEV_PRINTF(NV_DBG_INFO, nv, "Reducing MSI-X count from %d to the "
                               "driver-supported maximum %d.\n", num_intr, NV_RM_MAX_MSIX_LINES);
        num_intr = NV_RM_MAX_MSIX_LINES;
    }

    NV_KMALLOC(nvl->msix_entries, sizeof(struct msix_entry) * num_intr);
    if (nvl->msix_entries == NULL)
    {
        NV_DEV_PRINTF(NV_DBG_ERRORS, nv, "Failed to allocate MSI-X entries.\n");
        goto failed;
    }

    for (i = 0, msix_entries = nvl->msix_entries; i < num_intr; i++, msix_entries++)
    {
        msix_entries->entry = i;
    }

    NV_KZALLOC(nvl->irq_count, sizeof(nv_irq_count_info_t) * num_intr);

    if (nvl->irq_count == NULL)
    {
        NV_DEV_PRINTF(NV_DBG_ERRORS, nv, "Failed to allocate counter for MSI-X entries.\n");
        goto failed;
    }
    else
    {
        nvl->current_num_irq_tracked = 0;
    }
    rc = nv_pci_enable_msix(nvl, num_intr);
    if (rc != NV_OK)
        goto failed;

    nv->flags |= NV_FLAG_USES_MSIX;
    return;

failed:
    nv->flags &= ~NV_FLAG_USES_MSIX;

    if (nvl->msix_entries)
    {
        NV_KFREE(nvl->msix_entries, sizeof(struct msix_entry) * num_intr);
    }

    if (nvl->irq_count)
    {
        NV_KFREE(nvl->irq_count, sizeof(nv_irq_count_info_t) * num_intr);
    }

    if (nvl->msix_bh_mutex)
    {
        os_free_mutex(nvl->msix_bh_mutex);
        nvl->msix_bh_mutex = NULL;
    }
    NV_DEV_PRINTF(NV_DBG_ERRORS, nv, "Failed to enable MSI-X.\n");
}

NvS32 NV_API_CALL nv_request_msix_irq(nv_linux_state_t *nvl)
{
    int i;
    int j;
    struct msix_entry *msix_entries;
    int rc = NV_ERR_INVALID_ARGUMENT;
    nv_state_t *nv = NV_STATE_PTR(nvl);

    for (i = 0, msix_entries = nvl->msix_entries; i < nvl->num_intr;
         i++, msix_entries++)
    {
        rc = request_threaded_irq(msix_entries->vector, nvidia_isr_msix,
                                  nvidia_isr_msix_kthread_bh, nv_default_irq_flags(nv),
                                  nv_device_name, (void *)nvl);
        if (rc)
        {
            for( j = 0; j < i; j++)
            {
                free_irq(nvl->msix_entries[j].vector, (void *)nvl);
            }
            break;
        }
    }

    return rc;
}
#endif
