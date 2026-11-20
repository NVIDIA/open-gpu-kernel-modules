/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "nv-pci-table.h"
#include "nv-pci-types.h"
#include "nv-pci.h"
#include "nv-msi.h"
#include "nv-hypervisor.h"
#include "nv-reg.h"
#include "nv-tegra-clk.h"
#include "nvstatuscodes.h"

#define NV_TDISP_AER_DVSEC0_OFFSET          0x0af8U
#define NV_TDISP_AER_TDI_STATE_SHIFT        30U
#define NV_TDISP_AER_TDI_STATE_MASK         0x3U
#define NV_TDISP_AER_TDI_STATE_ERROR        0x3U
#define NV_TDISP_AER_XID                    79U
#define NV_TDISP_AER_TDI_STATE(_dvsec0)                                \
    (((_dvsec0) >> NV_TDISP_AER_TDI_STATE_SHIFT) &                    \
     NV_TDISP_AER_TDI_STATE_MASK)

#if defined(NV_VGPU_KVM_BUILD)
#include "nv-vgpu-vfio-interface.h"
#endif
#include <linux/completion.h>
#include <linux/iommu.h>
#include <linux/kthread.h>

#if defined(NV_SEQ_READ_ITER_PRESENT)
#include <linux/seq_file.h>
#include <linux/kernfs.h>
#endif

#include "detect-self-hosted.h"

#if !defined(NV_BUS_TYPE_HAS_IOMMU_OPS)
#include <linux/iommu.h>
#endif
#if NV_IS_EXPORT_SYMBOL_GPL_pci_ats_supported
#include <linux/pci-ats.h>
#endif

extern int NVreg_ExcludeAllGpus;

static void
nv_check_and_exclude_gpu(
    nvidia_stack_t *sp,
    nv_state_t *nv
)
{
    char *uuid_str;

    if (NVreg_ExcludeAllGpus)
    {
        NV_DEV_PRINTF(NV_DBG_INFO, nv, "Excluding all GPUs as requested\n");
        nv->flags |= NV_FLAG_EXCLUDE;
        return;
    }

    uuid_str = rm_get_gpu_uuid(sp, nv);
    if (uuid_str == NULL)
    {
        NV_DEV_PRINTF(NV_DBG_INFO, nv, "Unable to read UUID");
        return;
    }

    if (nv_is_uuid_in_gpu_exclusion_list(uuid_str))
    {
        NV_STATUS rm_status = rm_exclude_adapter(sp, nv);
        if (rm_status != NV_OK)
        {
            NV_DEV_PRINTF_STATUS(NV_DBG_ERRORS, nv, rm_status,
                          "Failed to exclude GPU %s", uuid_str);
            goto done;
        }
        nv->flags |= NV_FLAG_EXCLUDE;
        NV_DEV_PRINTF(NV_DBG_INFO, nv, "Excluded GPU %s successfully\n",
                      uuid_str);
    }

done:
    os_free_mem(uuid_str);
}

static bool
nv_pcie_is_cxl(struct pci_dev *pdev)
{
#ifdef NV_PCIE_IS_CXL_PRESENT
    return pcie_is_cxl(pdev);
#elif NV_IS_EXPORT_SYMBOL_GPL_pci_find_dvsec_capability
    u16 dvsec, cap;
    dvsec = pci_find_dvsec_capability(pdev, PCI_VENDOR_ID_CXL, PCI_DVSEC_CXL_FLEXBUS_PORT);
    if (!dvsec)
        return false;
    pci_read_config_word(pdev, dvsec + PCI_DVSEC_CXL_FLEXBUS_PORT_STATUS, &cap);
    return (cap & PCI_DVSEC_CXL_FLEXBUS_PORT_STATUS_CACHE) || (cap & PCI_DVSEC_CXL_FLEXBUS_PORT_STATUS_MEM);
#else
    return false;
#endif
}

static int
nv_cxl_get_hdm_base_addr
(
    struct pci_dev *pdev,
    NvU64 *hdm_base_addr,
    NvU64 *hdm_size
)
{
#if NV_IS_EXPORT_SYMBOL_GPL_pci_find_dvsec_capability
    void __iomem *comp_base = NULL, *hdm_base = NULL;
    u32 reg_lo, reg_hi, cap_array, cap_count, hdr, cap_id, offset;
    u32 hdm_cap, base_low, base_high, size_low, size_high, ctrl_reg;
    u32 dvsec_hdr;
    u64 base_addr, size;
    int dvsec_pos, regloc, bar, i;
    bool committed;

    /* Find CXL Register Locator DVSEC */
    dvsec_pos = pci_find_dvsec_capability(pdev, PCI_VENDOR_ID_CXL,
                                          PCI_DVSEC_CXL_REG_LOCATOR);

    if (!dvsec_pos)
        return -ENXIO;

    pci_read_config_dword(pdev, dvsec_pos + PCI_DVSEC_HEADER1, &dvsec_hdr);

    if (((dvsec_hdr >> 16) & 0xFFFF) == PCI_VENDOR_ID_CXL &&
        (dvsec_hdr & 0xFFFF) == PCI_DVSEC_CXL_REG_LOCATOR)
    {
        nv_printf(NV_DBG_INFO, "NVRM: Found CXL Register Locator DVSEC at 0x%x\n", dvsec_pos);
    }

    /* Find component register block in DVSEC */
    regloc = dvsec_pos + PCI_DVSEC_CXL_REG_LOCATOR_BLOCK1;

    /* Read first register block entry */
    pci_read_config_dword(pdev, regloc, &reg_lo);
    pci_read_config_dword(pdev, regloc + 4, &reg_hi);

    /* Check if it's a component register block */
    if (((reg_lo & PCI_DVSEC_CXL_REG_LOCATOR_BLOCK_ID) >> 8) != NV_CXL_REGLOC_RBI_COMPONENT)
    {
        nv_printf(NV_DBG_INFO, "NVRM: First register block is not component type\n");
        return -ENODEV;
    }

    /* Extract BAR and offset */
    bar = reg_lo & PCI_DVSEC_CXL_REG_LOCATOR_BIR;
    offset = ((u64)reg_hi << 32) | (reg_lo & PCI_DVSEC_CXL_REG_LOCATOR_BLOCK_OFF_LOW);

    /* Map component registers */
    comp_base = ioremap(pci_resource_start(pdev, bar) + offset, pci_resource_len(pdev, bar) - offset);

    if (!comp_base)
    {
        nv_printf(NV_DBG_INFO, "NVRM: Failed to map component registers\n");
        return -ENOMEM;
    }

    /* Find HDM capability in component registers */
    cap_array = readl(comp_base + NV_CXL_CM_OFFSET + NV_CXL_CM_CAP_HDR_OFFSET);

    /* Validate capability header */
    if ((cap_array & NV_CXL_CM_CAP_HDR_ID_MASK) != NV_CM_CAP_HDR_CAP_ID)
    {
        nv_printf(NV_DBG_INFO, "NVRM: Invalid component capability header\n");
        goto err_unmap;
    }

    cap_count = (cap_array & NV_CXL_CM_CAP_HDR_ARRAY_SIZE_MASK) >> 24;
    nv_printf(NV_DBG_INFO, "NVRM: Found %u component capabilities\n", cap_count);

    /* Search for HDM capability */
    hdm_base = NULL;
    for (i = 1; i <= cap_count; i++)
    {
        hdr = readl(comp_base + NV_CXL_CM_OFFSET + i * 0x4);
        cap_id = hdr & NV_CXL_CM_CAP_HDR_ID_MASK;
        offset = (hdr & NV_CXL_CM_CAP_PTR_MASK) >> 20;

        if (cap_id == NV_CXL_CM_CAP_CAP_ID_HDM)
        {
            hdm_base = comp_base + NV_CXL_CM_OFFSET + offset;
            nv_printf(NV_DBG_INFO, "NVRM: Found HDM capability at offset 0x%x\n", offset);
            break;
        }
    }

    if (!hdm_base)
    {
        nv_printf(NV_DBG_INFO, "NVRM: HDM capability not found\n");
        goto err_unmap;
    }

    /* Read HDM capability register */
    hdm_cap = readl(hdm_base + NV_CXL_HDM_DECODER_CAP_OFFSET);
    dev_info(&pdev->dev, "HDM capability register: 0x%08x\n", hdm_cap);

    /* Read HDM decoder 0 registers */
    base_low = readl(hdm_base + NV_CXL_HDM_DECODER0_BASE_LOW_OFFSET(0));
    base_high = readl(hdm_base + NV_CXL_HDM_DECODER0_BASE_HIGH_OFFSET(0));
    size_low = readl(hdm_base + NV_CXL_HDM_DECODER0_SIZE_LOW_OFFSET(0));
    size_high = readl(hdm_base + NV_CXL_HDM_DECODER0_SIZE_HIGH_OFFSET(0));
    ctrl_reg = readl(hdm_base + NV_CXL_HDM_DECODER0_CTRL_OFFSET(0));

    base_addr = ((u64)base_high << 32) | base_low;
    size = ((u64)size_high << 32) | size_low;

    nv_printf(NV_DBG_INFO, "HDM Decoder 0 base : 0x%llx\n", base_addr);
    nv_printf(NV_DBG_INFO, "HDM Decoder 0 size : 0x%llx\n", size);

    committed = !!(ctrl_reg & NV_CXL_HDM_DECODER0_CTRL_COMMITTED);

    if (committed)
    {
        nv_printf(NV_DBG_INFO, "NVRM: HDM Decoder 0 is active and configured\n");
        *hdm_base_addr = base_addr;
        *hdm_size = size;
    }
    else
    {
        nv_printf(NV_DBG_INFO, "NVRM: HDM Decoder 0 is not active\n");
        goto err_unmap;
    }

    iounmap(comp_base);
    return 0;

err_unmap:
    iounmap(comp_base);
#endif
    return -ENODEV;
}

static int
nv_cxl_init_core
(
    struct pci_dev *pci_dev,
    nv_linux_state_t *nvl
)
{
#if defined(NV_CXL_INIT_SUPPORTED) && NV_IS_EXPORT_SYMBOL_GPL_pci_find_dvsec_capability
    struct nv_cxl *cxl;
    struct range range;
    u16 dvsec;
    u64 capacity;
    u32 size_hi, size_lo;
    int rc;

    if (!nv_pcie_is_cxl(pci_dev))
        return -ENODEV;

    dvsec = pci_find_dvsec_capability(pci_dev, PCI_VENDOR_ID_CXL,
                                      PCI_DVSEC_CXL_DEVICE);
    if (!dvsec)
        return -ENODEV;

    /* Create a cxl_dev_state embedded in nv_cxl; no mailbox. */
    cxl = devm_cxl_dev_state_create(&pci_dev->dev, CXL_DEVTYPE_DEVMEM,
                                    pci_dev->dev.id, dvsec, struct nv_cxl,
                                    cxlds, false);
    if (IS_ERR_OR_NULL(cxl))
        return IS_ERR(cxl) ? PTR_ERR(cxl) : -ENOMEM;

    rc = cxl_pci_setup_regs(pci_dev, CXL_REGLOC_RBI_COMPONENT,
                            &cxl->cxlds.reg_map);
    if (rc) {
        nv_printf(NV_DBG_INFO, "No component registers\n");
        return rc;
    }

    if (!cxl->cxlds.reg_map.component_map.hdm_decoder.valid) {
        nv_printf(NV_DBG_INFO, "Expected HDM component register not found\n");
        return -ENODEV;
    }

    /*
     * Initialize the DPA resource tree with the device's memory capacity so
     * that cxl_port can call devm_cxl_dpa_reserve() when it probes the
     * endpoint decoder.
     *
     * Read capacity from the first DVSEC Range Size register; a single
     * committed decoder exposes the full capacity in Range 0.
     */
    if (pci_read_config_dword(pci_dev,
                              dvsec + PCI_DVSEC_CXL_RANGE_SIZE_HIGH(0),
                              &size_hi) ||
        pci_read_config_dword(pci_dev,
                              dvsec + PCI_DVSEC_CXL_RANGE_SIZE_LOW(0),
                              &size_lo)) {
        nv_printf(NV_DBG_INFO, "CXL: failed to read DVSEC range size\n");
        return -EIO;
    }
    capacity = ((u64)size_hi << 32) | (size_lo & PCI_DVSEC_CXL_MEM_SIZE_LOW);
    if (!capacity) {
        nv_printf(NV_DBG_INFO, "CXL: DVSEC reports zero capacity\n");
        return -ENODEV;
    }

    if (cxl_set_capacity(&cxl->cxlds, capacity)) {
        nv_printf(NV_DBG_INFO, "CXL: dpa capacity setup failed\n");
        return -ENOSPC;
    }

    /*
     * Set media ready explicitly; there is no mailbox nor a dedicated CXL
     * register for this on a type-2 accelerator.
     */
    cxl->cxlds.media_ready = true;

    cxl->cxlmd = devm_cxl_add_memdev(&cxl->cxlds, NULL);
    if (IS_ERR_OR_NULL(cxl->cxlmd))
    {
        nv_printf(NV_DBG_INFO, "CXL accel memdev creation failed\n");
        return IS_ERR(cxl->cxlmd) ? PTR_ERR(cxl->cxlmd) : -ENOMEM;
    }

    nvl->cxl = cxl;

    /*
     * cxl_get_committed_decoder() drops its device reference before
     * returning, so neither cxled nor nv_region carries a reference of its
     * own. They are only valid while cxl_mem stays bound to this memdev.
     * That is not enforced here - devm_cxl_add_memdev() is called with a
     * NULL attach argument, so cxl_mem attachment is optional and a memdev
     * can be returned before the CXL topology is ready. Treat a failed
     * lookup as "CXL not usable" and fall back, rather than assuming these
     * pointers outlive the probe.
     */
    cxl->cxled = cxl_get_committed_decoder(cxl->cxlmd, &cxl->nv_region);
    if (IS_ERR_OR_NULL(cxl->cxled))
    {
        rc = IS_ERR(cxl->cxled) ? PTR_ERR(cxl->cxled) : -ENODEV;
        nv_printf(NV_DBG_INFO, "CXL committed decoder lookup failed.\n");

        return rc;
    }

    if (cxl->nv_region == NULL)
    {
        nv_printf(NV_DBG_INFO, "CXL found committed decoder without a region\n");
        return -ENODEV;
    }

    rc = cxl_get_region_range(cxl->nv_region, &range);
    if (rc != 0)
    {
        nv_printf(NV_DBG_INFO, "CXL getting region params from committed decoder failed\n");

        return rc;
    }

    nvl->coherent_link_info.gpu_mem_pa   = range.start;
    nvl->coherent_link_info.gpu_mem_size = range_len(&range);
    nv_printf(NV_DBG_INFO, "CXL committed decoder: HPA base=0x%llx size=0x%llx\n",
              range.start, range_len(&range));

    return 0;
#else
    return -ENODEV;
#endif
}

static int
nv_cxl_init
(
    nv_linux_state_t *nvl
)
{
    struct pci_dev *pci_dev = nvl->pci_dev;
    nv_state_t *nv = NV_STATE_PTR(nvl);
    int rc;

    if (!nv_pcie_is_cxl(pci_dev))
        return 0;

    NV_DEV_PRINTF(NV_DBG_INFO, nv, "Detected as a CXL device\n");
    nv->is_cxl_dev = NV_TRUE;

#if defined(NV_CXL_INIT_SUPPORTED)
    rc = nv_cxl_init_core(pci_dev, nvl);
    if (rc == 0)
        return 0;

    NV_DEV_PRINTF(NV_DBG_INFO, nv,
                  "CXL subsystem init failed, falling back to direct HDM decoder read\n");
#endif

    rc = nv_cxl_get_hdm_base_addr(pci_dev,
                                  &nvl->coherent_link_info.gpu_mem_pa,
                                  &nvl->coherent_link_info.gpu_mem_size);
    if (rc != 0)
    {
        NV_DEV_PRINTF(NV_DBG_INFO, nv, "Cannot get CXL coherent link info.\n");
    }

    return rc;
}

/*
 * Release this device's CXL state.
 *
 * unregister_region selects how the discovered committed region is treated:
 *
 *   NV_TRUE  - device removal. This driver is the last consumer of the
 *              region, so tear it down along with the rest of the state.
 *   NV_FALSE - probe failure. The committed region is discovered, not
 *              created here: cxl_get_committed_decoder() returns a region
 *              that the CXL framework and the platform already own. Tearing
 *              it down because RM failed to probe would destroy topology
 *              that other consumers - and our own subsequent reprobe -
 *              still depend on, and it does not come back on reprobe.
 */
static void
nv_cxl_cleanup
(
    nv_linux_state_t *nvl,
    NvBool unregister_region
)
{
#if defined(NV_CXL_INIT_SUPPORTED)
    struct nv_cxl *cxl = nvl->cxl;

    if (!cxl)
        return;

    if (unregister_region && (cxl->nv_region != NULL))
    {
        cxl_unregister_region(cxl->nv_region);
    }

    cxl->nv_region = NULL;
    nvl->cxl = NULL;
#endif
}

static NvBool nv_treat_missing_irq_as_error(void)
{
#if defined(NV_LINUX_PCIE_MSI_SUPPORTED)
    return (nv_get_hypervisor_type() != OS_HYPERVISOR_HYPERV);
#else
    return NV_TRUE;
#endif
}

static void nv_get_pci_sysfs_config
(
    struct pci_dev *pci_dev,
    nv_linux_state_t *nvl
)
{
#if NV_FILESYSTEM_ACCESS_AVAILABLE
    char filename[50];
    int ret;

    ret = snprintf(filename, sizeof(filename),
                   "/sys/bus/pci/devices/%04x:%02x:%02x.0/config",
                   NV_PCI_DOMAIN_NUMBER(pci_dev),
                   NV_PCI_BUS_NUMBER(pci_dev),
                   NV_PCI_SLOT_NUMBER(pci_dev));
    if (ret > 0 && ret < sizeof(filename))
    {
        struct file *file = filp_open(filename, O_RDONLY, 0);
        if (!IS_ERR(file))
        {
#if defined(NV_SEQ_READ_ITER_PRESENT)
            /*
             * Sanity check for confirming if file path is mounted over
             * sysfs file system.
             */
            if ((file->f_inode != NULL) && (file->f_inode->i_sb != NULL) &&
                (strcmp(file->f_inode->i_sb->s_id, "sysfs") == 0))
            {
                struct seq_file *sf = file->private_data;

                /*
                 * Sanity check for confirming if 'file->private_data'
                 * actually points to 'struct seq_file'.
                 */
                if ((sf != NULL) && (sf->file == file) && (sf->op == NULL))
                {
                    struct kernfs_open_file *of = sf->private;

                    /*
                     * Sanity check for confirming if 'sf->private'
                     * actually points to 'struct kernfs_open_file'.
                     */
                    if ((of != NULL) && (of->file == file) &&
                        (of->seq_file == sf))
                    {
                        nvl->sysfs_config_file = file;
                    }
                }
            }

            if (nvl->sysfs_config_file == NULL)
            {
                filp_close(file, NULL);
            }
#else
            nvl->sysfs_config_file = file;
#endif
        }
    }
#endif
}

static int nv_resize_pcie_bars(struct pci_dev *pci_dev) {
#if defined(NV_PCI_REBAR_GET_POSSIBLE_SIZES_PRESENT)
    u16 cmd;
    int r, old_size, requested_size;
    unsigned long sizes;
    int ret = 0;
#if NV_IS_EXPORT_SYMBOL_PRESENT_pci_find_host_bridge
    struct pci_host_bridge *host;
#endif

    if (NVreg_EnableResizableBar == 0)
    {
        nv_printf(NV_DBG_INFO, "NVRM: resizable BAR disabled by regkey, skipping\n");
        return 0;
    }

    // Check if BAR1 has PCIe rebar capabilities
    sizes = pci_rebar_get_possible_sizes(pci_dev, NV_GPU_BAR1);
    if (sizes == 0) {
        /* ReBAR not available. Nothing to do. */
        return 0;
    }

    /* Try to resize the BAR to the largest supported size */
    requested_size = fls(sizes) - 1;

    /* Save the current size, just in case things go wrong */
    old_size = pci_rebar_bytes_to_size(pci_resource_len(pci_dev, NV_GPU_BAR1));

    if (old_size == requested_size) {
        nv_printf(NV_DBG_INFO, "NVRM: %04x:%02x:%02x.%x: BAR1 already at requested size.\n",
            NV_PCI_DOMAIN_NUMBER(pci_dev), NV_PCI_BUS_NUMBER(pci_dev),
            NV_PCI_SLOT_NUMBER(pci_dev), PCI_FUNC(pci_dev->devfn));
        return 0;
    }
#if NV_IS_EXPORT_SYMBOL_PRESENT_pci_find_host_bridge
    /* If the kernel will refuse us, don't even try to resize,
       but give an informative error */
    host = pci_find_host_bridge(pci_dev->bus);
    if (host->preserve_config) {
        nv_printf(NV_DBG_INFO, "NVRM: Not resizing BAR because the firmware forbids moving windows.\n");
        return 0;
    }
#endif
    nv_printf(NV_DBG_INFO, "NVRM: %04x:%02x:%02x.%x: Attempting to resize BAR1.\n",
        NV_PCI_DOMAIN_NUMBER(pci_dev), NV_PCI_BUS_NUMBER(pci_dev),
        NV_PCI_SLOT_NUMBER(pci_dev), PCI_FUNC(pci_dev->devfn));

    /* Disable memory decoding - required by the kernel APIs */
    pci_read_config_word(pci_dev, PCI_COMMAND, &cmd);
    pci_write_config_word(pci_dev, PCI_COMMAND, cmd & ~PCI_COMMAND_MEMORY);

    /* Release BAR1 */
    pci_release_resource(pci_dev, NV_GPU_BAR1);

    /* Release BAR3 - we don't want to resize it, it's in the same bridge, so we'll want to move it */
    pci_release_resource(pci_dev, NV_GPU_BAR3);

resize:
    /* Attempt to resize BAR1 to the largest supported size */
#if defined(NV_PCI_RESIZE_RESOURCE_HAS_EXCLUDE_BARS_ARG)
    r = pci_resize_resource(pci_dev, NV_GPU_BAR1, requested_size, 0);
#else
    r = pci_resize_resource(pci_dev, NV_GPU_BAR1, requested_size);
#endif

    if (r) {
        if (r == -ENOSPC)
        {
            /* step through smaller sizes down to original size */
            if (requested_size > old_size)
            {
                clear_bit(fls(sizes) - 1, &sizes);
                requested_size = fls(sizes) - 1;
                goto resize;
            }
            else
            {
                nv_printf(NV_DBG_ERRORS, "NVRM: No address space to allocate resized BAR1.\n");
            }
        }
        else if (r == -EOPNOTSUPP)
        {
            nv_printf(NV_DBG_WARNINGS, "NVRM: BAR resize resource not supported.\n");
        }
        else
        {
            nv_printf(NV_DBG_WARNINGS, "NVRM: BAR resizing failed with error `%d`.\n", r);
        }
    }

    /* Re-attempt assignment of PCIe resources */
    pci_assign_unassigned_bus_resources(pci_dev->bus);

    if ((pci_resource_flags(pci_dev, NV_GPU_BAR1) & IORESOURCE_UNSET) ||
        (pci_resource_flags(pci_dev, NV_GPU_BAR3) & IORESOURCE_UNSET)) {
        if (requested_size != old_size) {
            /* Try to get the BAR back with the original size */
            requested_size = old_size;
            goto resize;
        }
        /* Something went horribly wrong and the kernel didn't manage to re-allocate BAR1.
           This is unlikely (because we had space before), but can happen. */
        nv_printf(NV_DBG_ERRORS, "NVRM: FATAL: Failed to re-allocate BAR1.\n");
        ret = -ENODEV;
    }

    /* Re-enable memory decoding */
    pci_write_config_word(pci_dev, PCI_COMMAND, cmd);

    return ret;
#else
    nv_printf(NV_DBG_INFO, "NVRM: Resizable BAR is not supported on this kernel version.\n");
    return 0;
#endif /* NV_PCI_REBAR_GET_POSSIBLE_SIZES_PRESENT */
}

#if defined(CONFIG_ACPI_NUMA) && NV_IS_EXPORT_SYMBOL_PRESENT_pxm_to_node
/*
 * Parse the SRAT table to look for numa node associated with the GPU.
 */
static NvU32 find_gpu_numa_nodes_in_srat(nv_linux_state_t *nvl)
{
    NvU32 gi_dbdf, dev_dbdf, pxm_count = 0;
    struct acpi_table_header *table_header;
    struct acpi_subtable_header *subtable_header;
    unsigned long table_end, subtable_header_length;
    struct acpi_srat_generic_affinity *gi;
    NvU32 numa_node = NUMA_NO_NODE;

    if (NV_PCI_DEVFN(nvl->pci_dev) != 0)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: Failing to parse SRAT GI for %04x:%02x:%02x.%x "
                  "since non-zero device function is not supported.\n",
                  NV_PCI_DOMAIN_NUMBER(nvl->pci_dev), NV_PCI_BUS_NUMBER(nvl->pci_dev),
                  NV_PCI_SLOT_NUMBER(nvl->pci_dev), PCI_FUNC(nvl->pci_dev->devfn));
        return 0;
    }

    if (acpi_get_table(ACPI_SIG_SRAT, 0, &table_header)) {
        nv_printf(NV_DBG_INFO, "NVRM: Failed to parse the SRAT table.\n");
        return 0;
    }

    table_end = (unsigned long)table_header + table_header->length;
    subtable_header = (struct acpi_subtable_header *)
            ((unsigned long)table_header + sizeof(struct acpi_table_srat));
    subtable_header_length = subtable_header->length;

    dev_dbdf = NV_PCI_DOMAIN_NUMBER(nvl->pci_dev) << 16 |
               NV_PCI_BUS_NUMBER(nvl->pci_dev) << 8 |
               NV_PCI_DEVFN(nvl->pci_dev);

    /*
     * On baremetal and passthrough, there could be upto 8 generic initiators.
     * This is not a hack as a device can have any number of initiators hardware
     * supports.
     */
    while (subtable_header_length &&
           (((unsigned long)subtable_header) + subtable_header_length <= table_end)) {

        if (subtable_header->type == ACPI_SRAT_TYPE_GENERIC_AFFINITY) {
            NvU8 busAtByte2, busAtByte3;
            gi = (struct acpi_srat_generic_affinity *) subtable_header;
            busAtByte2 = gi->device_handle[2];
            busAtByte3 = gi->device_handle[3];

            // Device and function should be zero enforced by above check
            gi_dbdf = *((NvU16 *)(&gi->device_handle[0])) << 16 |
                (busAtByte2 != 0 ? busAtByte2 : busAtByte3) << 8;
            
            if (gi_dbdf == dev_dbdf) {
                numa_node = pxm_to_node(gi->proximity_domain);
                if (numa_node < MAX_NUMNODES) {
                    pxm_count++;
                    set_bit(numa_node, nvl->coherent_link_info.free_node_bitmap);
                }
                else {
                    /* We shouldn't be here. This is a mis-configuration. */
                    nv_printf(NV_DBG_INFO, "NVRM: Invalid node-id found.\n");
                    pxm_count = 0;
                    goto exit;
                }
                nv_printf(NV_DBG_INFO,
                          "NVRM: matching SRAT GI entry: 0x%x 0x%x 0x%x 0x%x  PXM: %d\n",
                          gi->device_handle[3],
                          gi->device_handle[2],
                          gi->device_handle[1],
                          gi->device_handle[0],
                          gi->proximity_domain);
                if ((busAtByte2) == 0 &&
                    (busAtByte3) != 0)
                {
                    /*
                     * TODO: Remove this WAR once Hypervisor stack is updated
                     * to fix this bug and after all CSPs have moved to using
                     * the updated Hypervisor stack with fix.
                     */
                    nv_printf(NV_DBG_WARNINGS,
                              "NVRM: PCIe bus value picked from byte 3 offset in SRAT GI entry: 0x%x 0x%x 0x%x 0x%x  PXM: %d\n"
                              "NVRM: Hypervisor stack is old and not following ACPI spec defined offset.\n"
                              "NVRM: Please consider upgrading the Hypervisor stack as this workaround will be removed in future release.\n",
                              gi->device_handle[3],
                              gi->device_handle[2],
                              gi->device_handle[1],
                              gi->device_handle[0],
                              gi->proximity_domain);
                }
            }
        }

        subtable_header = (struct acpi_subtable_header *)
                          ((unsigned long) subtable_header + subtable_header_length);
        subtable_header_length = subtable_header->length;
    }

exit:
    acpi_put_table(table_header);
    return pxm_count;
}
#endif // defined(CONFIG_ACPI_NUMA) && NV_IS_EXPORT_SYMBOL_PRESENT_pxm_to_node

void
nv_init_gpu_numa_info
(
    nv_state_t *nv
)
{
#if defined(CONFIG_ACPI_NUMA) && NV_IS_EXPORT_SYMBOL_PRESENT_pxm_to_node
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);
    NvU64 pxm_start = 0;
    NvU64 pxm_count = 0;
    NvU32 gi_found = 0;
    NvU32 pxm, node;

    if (!NVCPU_IS_AARCH64)
        return;

    if (!dev_is_pci(nvl->dev))
        return;

    if (bitmap_empty(nvl->coherent_link_info.free_node_bitmap, MAX_NUMNODES))
    {
        gi_found = find_gpu_numa_nodes_in_srat(nvl);

        if (!gi_found &&
            (device_property_read_u64(nvl->dev, "nvidia,gpu-mem-pxm-start", &pxm_start) != 0 ||
             device_property_read_u64(nvl->dev, "nvidia,gpu-mem-pxm-count", &pxm_count) != 0))
        {
            NV_DEV_PRINTF(NV_DBG_SETUP, nv, "Cannot get GPU NUMA info.\n");
            return;
        }

        if (!gi_found)
        {
            for (pxm = pxm_start; pxm < (pxm_start + pxm_count); pxm++)
            {
                node = pxm_to_node(pxm);
                if (node != NUMA_NO_NODE)
                {
                    set_bit(node, nvl->coherent_link_info.free_node_bitmap);
                }
            }
        }
    }

    if (bitmap_empty(nvl->coherent_link_info.free_node_bitmap, MAX_NUMNODES))
    {
        NV_DEV_PRINTF(NV_DBG_SETUP, nv, "Cannot get GPU NUMA info.\n");
        return;
    }

    for (node = 0; (node = find_next_bit(nvl->coherent_link_info.free_node_bitmap,
                             MAX_NUMNODES, node)) != MAX_NUMNODES; node++)
    {
        NV_DEV_PRINTF(NV_DBG_INFO, nv, "\tNVRM: GPU memory NUMA node: %u\n", node);
    }

    NV_ATOMIC_SET(nvl->numa_info.status, NV_IOCTL_NUMA_STATUS_OFFLINE);
    nvl->numa_info.use_auto_online = NV_TRUE;
    nvl->numa_info.node_id =
        find_first_bit(nvl->coherent_link_info.free_node_bitmap, MAX_NUMNODES);

    NV_DEV_PRINTF(NV_DBG_SETUP, nv, "GPU NUMA information: node id: %u\n",
                  nvl->numa_info.node_id);

    return;
#endif // defined(CONFIG_ACPI_NUMA) && NV_IS_EXPORT_SYMBOL_PRESENT_pxm_to_node
    return;
}

void
nv_init_coherent_gpu_info
(
    nv_state_t *nv
)
{
#if defined(CONFIG_ACPI_NUMA) && NV_IS_EXPORT_SYMBOL_PRESENT_pxm_to_node
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);
    NvU64 pa = 0;

    if (!NVCPU_IS_AARCH64)
        return;

    if (!dev_is_pci(nvl->dev))
        return;

    if (nv->is_cxl_dev)
        goto coherent_link_info_initialized;

    if (device_property_read_u64(nvl->dev, "nvidia,gpu-mem-base-pa", &pa) == 0)
    {
        NvU64 gpu_mem_size;

        nvl->coherent_link_info.gpu_mem_pa = pa;

        if (device_property_read_u64(nvl->dev, "nvidia,gpu-mem-size", &gpu_mem_size) == 0)
        {
            nvl->coherent_link_info.gpu_mem_size = gpu_mem_size;
        }
    }
    else
    {
        unsigned int gpu_bar1_offset, gpu_bar2_offset;

        if (nv_get_hypervisor_type() == OS_HYPERVISOR_UNKNOWN)
        {
            /* Fail for the baremetal case */
            goto failed;
        }
        
        /*
         * For the virtualization usecase on SHH, the coherent GPU memory
         * PA is exposed as BAR2 to the VM and the "nvidia,gpu-mem-base-pa"
         * is not present. Set the GPU memory PA to the BAR2 start address.
         *
         * In the case of passthrough, reserved memory portion of the coherent
         * GPU memory is exposed as BAR1
         */

        /*
         * Hopper+ uses 64-bit BARs, so GPU BAR2 should be at BAR4/5 and
         * GPU BAR1 is at BAR2/3
         */
        gpu_bar1_offset = 2;
        gpu_bar2_offset = 4;

        /*
        * cannot use nv->bars[] here as it is not populated correctly if BAR1 is
        * not present but BAR2 is, even though PCIe spec allows it. Not fixing
        * nv->bars[] since this is not a valid scenario with the actual HW and
        * possible only with this host emulated BAR scenario.
        */
        if (!((NV_PCI_RESOURCE_VALID(nvl->pci_dev, gpu_bar2_offset)) &&
            (NV_PCI_RESOURCE_FLAGS(nvl->pci_dev, gpu_bar2_offset) & PCI_BASE_ADDRESS_SPACE)
            == PCI_BASE_ADDRESS_SPACE_MEMORY))
        {
            // BAR2 contains the cacheable part of the coherent FB region and must have.
            goto failed;
        }
        nvl->coherent_link_info.gpu_mem_pa =
            NV_PCI_RESOURCE_START(nvl->pci_dev, gpu_bar2_offset);

        if ((pci_devid_is_self_hosted_hopper(nv->pci_info.device_id)) &&
            (NV_PCI_RESOURCE_VALID(nvl->pci_dev, gpu_bar1_offset)) &&
            (NV_PCI_RESOURCE_FLAGS(nvl->pci_dev, gpu_bar1_offset) & PCI_BASE_ADDRESS_SPACE)
            == PCI_BASE_ADDRESS_SPACE_MEMORY)
        {
            // Present only in passthrough case for self-hosted hopper.
            nvl->coherent_link_info.rsvd_mem_pa = NV_PCI_RESOURCE_START(nvl->pci_dev, gpu_bar1_offset);

            //
            // Unset nv->bars[1] only for self-hosted Hopper as BAR1 in virtualization case
            // for hopper is used to convey RM reserved memory information and doesn't contain
            // the traditional GPU BAR2. Starting from Blackwell BAR1 will be the real BAR1.
            //
            memset(&nv->bars[1], 0, sizeof(nv->bars[1]));
        }

        //
        // Unset nv->bars[2] for all self-hosted systems as BAR2 in the virtualization case
        // is used only to convey the coherent GPU memory information and doesn't contain
        // the traditional GPU BAR2. This is to ensure the coherent FB addresses don't 
        // inadvertently pass the IS_FB_OFFSET or IS_IMEM_OFFSET checks.
        //
        memset(&nv->bars[2], 0, sizeof(nv->bars[2]));
    }

coherent_link_info_initialized:
    NV_DEV_PRINTF(NV_DBG_INFO, nv, "DSD properties: \n");
    NV_DEV_PRINTF(NV_DBG_INFO, nv, "\tGPU memory PA: 0x%lx \n",
                  nvl->coherent_link_info.gpu_mem_pa);
    if (pci_devid_is_self_hosted_hopper(nv->pci_info.device_id))
    {
        NV_DEV_PRINTF(NV_DBG_INFO, nv, "\tGPU reserved memory PA: 0x%lx \n",
                      nvl->coherent_link_info.rsvd_mem_pa);
    }

    nvl->coherent_link_info.gpu_mem_info_discovery_attempted = NV_TRUE;
    return;

failed:
    NV_DEV_PRINTF(NV_DBG_SETUP, nv, "Cannot get coherent link info.\n");
#endif // defined(CONFIG_ACPI_NUMA) && NV_IS_EXPORT_SYMBOL_PRESENT_pxm_to_node
    return;
}


static void nv_init_dynamic_power_management
(
    nvidia_stack_t *sp,
    struct pci_dev *pci_dev
)
{
    nv_linux_state_t *nvl = pci_get_drvdata(pci_dev);
    nv_state_t *nv = NV_STATE_PTR(nvl);
    NvBool pr3_acpi_method_present = NV_FALSE;

    nvl->sysfs_config_file = NULL;

    nv_get_pci_sysfs_config(pci_dev, nvl);

    if (nv_get_hypervisor_type() != OS_HYPERVISOR_UNKNOWN)
    {
        pr3_acpi_method_present = nv_acpi_power_resource_method_present(pci_dev);
    }
    else if (pci_dev->bus && pci_dev->bus->self)
    {
        pr3_acpi_method_present = nv_acpi_power_resource_method_present(pci_dev->bus->self);
    }

    rm_init_dynamic_power_management(sp, nv, pr3_acpi_method_present);
}

static void nv_init_tegra_gpu_pg_mask(nvidia_stack_t *sp, struct pci_dev *pci_dev)
{
    nv_linux_state_t *nvl = pci_get_drvdata(pci_dev);
    nv_state_t *nv = NV_STATE_PTR(nvl);
    struct device_node *np = pci_dev->dev.of_node;
    u32 gpu_pg_mask = 0;

    /* Only continue with certain Tegra PCI iGPUs */
    if (!nv->supports_tegra_igpu_rg)
    {
        return;
    }

    nv->tegra_pci_igpu_pg_mask = NV_TEGRA_PCI_IGPU_PG_MASK_DEFAULT;

    of_property_read_u32(np, "nvidia,fuse-overrides", &gpu_pg_mask);
    if (gpu_pg_mask != 0) {
        nv_printf(NV_DBG_INFO,
            "NVRM: nvidia,fuse-overrides parsed from device tree: 0x%x\n", gpu_pg_mask);
        nv->tegra_pci_igpu_pg_mask = gpu_pg_mask;
    }

    nv_set_gpu_pg_mask(nv);

    /*
     * Trigger GPU reset to make new value of static TPC/GPC/FBP
     * power-gating mask effective in BPMP-FW. Otherwise, the BPMP-FW
     * may just save the mask in SW variable until next GPU reset.
     */
    nv_trigger_gpu_flr(nv);
}

static NvBool
nv_pci_validate_bars(const struct pci_dev *pci_dev, NvBool only_bar0)
{
    unsigned int i, j;
    NvBool last_bar_64bit = NV_FALSE;

    for (i = 0, j = 0; i < NVRM_PCICFG_NUM_BARS && j < NV_GPU_NUM_BARS; i++)
    {
        if (NV_PCI_RESOURCE_VALID(pci_dev, i))
        {
            if ((NV_PCI_RESOURCE_FLAGS(pci_dev, i) & PCI_BASE_ADDRESS_MEM_TYPE_64) &&
                (NV_PCI_RESOURCE_FLAGS(pci_dev, i) & PCI_BASE_ADDRESS_MEM_PREFETCH))
            {
                last_bar_64bit = NV_TRUE;
            }

            //
            // If we are here, then we have found a valid BAR -- 32 or 64-bit.
            //
            j++;

            if (only_bar0)
                return NV_TRUE;

            continue;
        }

        //
        // If last_bar_64bit is "true" then, we are looking at the 2nd (upper)
        // half of the 64-bit BAR. This is typically all 0s which looks invalid
        // but it's normal and not a problem and we can ignore it and continue.
        //
        if (last_bar_64bit)
        {
            last_bar_64bit = NV_FALSE;
            continue;
        }

        // Invalid 32 or 64-bit BAR.
        nv_printf(NV_DBG_ERRORS,
            "NVRM: This PCI I/O region assigned to your NVIDIA device is invalid:\n"
            "NVRM: BAR%d is %" NvU64_fmtu "M @ 0x%" NvU64_fmtx " (PCI:%04x:%02x:%02x.%x)\n", i,
            (NvU64)(NV_PCI_RESOURCE_SIZE(pci_dev, i) >> 20),
            (NvU64)NV_PCI_RESOURCE_START(pci_dev, i),
            NV_PCI_DOMAIN_NUMBER(pci_dev), NV_PCI_BUS_NUMBER(pci_dev),
            NV_PCI_SLOT_NUMBER(pci_dev), PCI_FUNC(pci_dev->devfn));

        return NV_FALSE;
    }

    return NV_TRUE;
}

typedef struct
{
    struct pci_dev *pci_dev;
    const struct pci_device_id *id_table;
    struct completion done;
    int rc;
} nv_pci_probe_deferred_context_t;

/* Used to differentiate probe behavior during driver registration vs later rebinds */
static NvBool nv_pci_driver_registration_in_progress = NV_FALSE;

static NvBool
nv_tdisp_probe_dma_mapping_readiness
(
    nv_linux_state_t *nvl
)
{
    nv_state_t *nv = NV_STATE_PTR(nvl);

#if defined(NV_DMA_ATTR_REQUIRE_COHERENT_PRESENT)
    {
        struct page *page;
        dma_addr_t dma_addr;

        page = alloc_page(GFP_KERNEL | GFP_DMA32 | __GFP_ZERO);
        if (page == NULL)
        {
            NV_DEV_PRINTF(NV_DBG_ERRORS, nv,
                          "TDISP-DMA-READINESS: page allocation failed, dma_ready=0\n");
            return NV_FALSE;
        }

        dma_addr = dma_map_page_attrs(nvl->dev, page, 0, PAGE_SIZE,
                                      DMA_BIDIRECTIONAL,
                                      DMA_ATTR_REQUIRE_COHERENT);
        if (dma_mapping_error(nvl->dev, dma_addr))
        {
            __free_page(page);
            NV_DEV_PRINTF(NV_DBG_ERRORS, nv,
                          "TDISP-DMA-READINESS: protected mapping failed, dma_ready=0\n");
            return NV_FALSE;
        }

        dma_unmap_page_attrs(nvl->dev, dma_addr, PAGE_SIZE,
                             DMA_BIDIRECTIONAL,
                             DMA_ATTR_REQUIRE_COHERENT);
        __free_page(page);

        NV_DEV_PRINTF(NV_DBG_INFO, nv,
                      "TDISP-DMA-READINESS: protected mapping ready, dma_ready=1\n");
        return NV_TRUE;
    }
#else
    NV_DEV_PRINTF(NV_DBG_ERRORS, nv,
                  "TDISP-DMA-READINESS: DMA_ATTR_REQUIRE_COHERENT unavailable, dma_ready=0\n");
    return NV_FALSE;
#endif
}

/* find nvidia devices and set initial state */
static int
nv_pci_probe_body
(
    struct pci_dev *pci_dev,
    const struct pci_device_id *id_table
)
{
    nv_state_t *nv = NULL;
    nv_linux_state_t *nvl = NULL;
    unsigned int i, j;
    int flags = 0;
    nvidia_stack_t *sp = NULL;
    NV_STATUS status;
    NvU8 regs_bar_index = nv_bar_index_to_os_bar_index(pci_dev,
                                                       NV_GPU_BAR_INDEX_REGS);
    NvBool bar0_requested = NV_FALSE;

    nv_printf(NV_DBG_SETUP, "NVRM: probing 0x%x 0x%x, class 0x%x\n",
        pci_dev->vendor, pci_dev->device, pci_dev->class);

#ifdef NV_PCI_SRIOV_SUPPORT
    if (pci_dev->is_virtfn)
    {
#if defined(NV_VGPU_KVM_BUILD)
#if defined(NV_BUS_TYPE_HAS_IOMMU_OPS)
        if (pci_dev->dev.bus->iommu_ops == NULL) 
#else
        if ((pci_dev->dev.iommu != NULL) && (pci_dev->dev.iommu->iommu_dev != NULL) &&
            (pci_dev->dev.iommu->iommu_dev->ops == NULL))
#endif
        {
            nv_printf(NV_DBG_ERRORS, "NVRM: Aborting probe for VF %04x:%02x:%02x.%x "
                      "since IOMMU is not present on the system.\n",
                       NV_PCI_DOMAIN_NUMBER(pci_dev), NV_PCI_BUS_NUMBER(pci_dev),
                       NV_PCI_SLOT_NUMBER(pci_dev), PCI_FUNC(pci_dev->devfn));
            return -1;
        }

        return 0;
#else
        nv_printf(NV_DBG_ERRORS, "NVRM: Ignoring probe for VF %04x:%02x:%02x.%x ",
                  NV_PCI_DOMAIN_NUMBER(pci_dev), NV_PCI_BUS_NUMBER(pci_dev),
                  NV_PCI_SLOT_NUMBER(pci_dev), PCI_FUNC(pci_dev->devfn));

        return -1;
#endif /* NV_VGPU_KVM_BUILD */
    }
#endif /* NV_PCI_SRIOV_SUPPORT */

    if (nv_kmem_cache_alloc_stack(&sp) != 0)
    {
        return -1;
    }

    if (!rm_wait_for_bar_firewall(
                sp,
                NV_PCI_DOMAIN_NUMBER(pci_dev),
                NV_PCI_BUS_NUMBER(pci_dev),
                NV_PCI_SLOT_NUMBER(pci_dev),
                PCI_FUNC(pci_dev->devfn),
                pci_dev->device,
                pci_dev->subsystem_device))
    {
        nv_printf(NV_DBG_ERRORS,
            "NVRM: failed to wait for bar firewall to lower\n");
        goto failed;
    }

    if (!rm_is_supported_pci_device(
                (pci_dev->class >> 16) & 0xFF,
                (pci_dev->class >> 8) & 0xFF,
                pci_dev->vendor,
                pci_dev->device,
                pci_dev->subsystem_vendor,
                pci_dev->subsystem_device,
                NV_FALSE /* print_legacy_warning */))
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: ignoring the legacy GPU %04x:%02x:%02x.%x\n",
                  NV_PCI_DOMAIN_NUMBER(pci_dev), NV_PCI_BUS_NUMBER(pci_dev),
                  NV_PCI_SLOT_NUMBER(pci_dev), PCI_FUNC(pci_dev->devfn));
        goto failed;
    }

    atomic_inc(&num_probed_nv_devices);

    if (pci_enable_device(pci_dev) != 0)
    {
        nv_printf(NV_DBG_ERRORS,
            "NVRM: pci_enable_device failed, aborting\n");
        goto failed;
    }

    if ((pci_dev->irq == 0 && !pci_find_capability(pci_dev, PCI_CAP_ID_MSIX))
        && nv_treat_missing_irq_as_error())
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: Can't find an IRQ for your NVIDIA card!\n");
        nv_printf(NV_DBG_ERRORS, "NVRM: Please check your BIOS settings.\n");
        nv_printf(NV_DBG_ERRORS, "NVRM: [Plug & Play OS] should be set to NO\n");
        nv_printf(NV_DBG_ERRORS, "NVRM: [Assign IRQ to VGA] should be set to YES \n");
        goto failed;
    }

    // Validate if BAR0 is usable
    if (!nv_pci_validate_bars(pci_dev, /* only_bar0 = */ NV_TRUE))
        goto failed;

    /*
     * CXL GPUs: do NOT claim BAR0 here.  The CXL component registers live
     * within BAR0, and cxl_port needs to devm_request_mem_region a sub-range
     * of it.  Linux's resource tree treats a request_mem_region claim as BUSY
     * (IORESOURCE_BUSY), which prevents any child allocation inside it.
     * When the nvidia driver registers (pci_register_driver), the kernel
     * retries deferred CXL endpoint probes; if BAR0 is already BUSY at that
     * moment, cxl_port fails and cxlmd->endpoint is never set, so
     * cxl_get_committed_decoder() returns NULL.
     *
     * For CXL GPUs we skip the claim entirely.  nv_pci_remove() is guarded
     * symmetrically so that release_mem_region(BAR0) is also skipped.
     * Non-CXL GPUs retain the existing claim/release behaviour.
     */

    if (!nv_pcie_is_cxl(pci_dev))
    {
        if (!request_mem_region(NV_PCI_RESOURCE_START(pci_dev, regs_bar_index),
                                NV_PCI_RESOURCE_SIZE(pci_dev, regs_bar_index),
                                nv_device_name))
        {
            nv_printf(NV_DBG_ERRORS,
                "NVRM: request_mem_region failed for %" NvU64_fmtu "M @ 0x%" NvU64_fmtx ". This can\n"
                "NVRM: occur when a driver such as rivatv is loaded and claims\n"
                "NVRM: ownership of the device's registers.\n",
                (NvU64)(NV_PCI_RESOURCE_SIZE(pci_dev, regs_bar_index) >> 20),
                (NvU64)NV_PCI_RESOURCE_START(pci_dev, regs_bar_index));
            goto failed;
        }
        bar0_requested = NV_TRUE;
    }

    NV_KZALLOC(nvl, sizeof(nv_linux_state_t));
    if (nvl == NULL)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: failed to allocate memory\n");
        goto err_not_supported;
    }

    nv  = NV_STATE_PTR(nvl);
    atomic_set(&nvl->tdisp_aer_reported, 0);
    os_mem_copy(nv->cached_gpu_info.vbios_version, "??.??.??.??.??", 15);

    for (i = 0; i < NVRM_PCICFG_NUM_BARS; i++)
    {
        if ((NV_PCI_RESOURCE_VALID(pci_dev, i)) &&
            (NV_PCI_RESOURCE_FLAGS(pci_dev, i) & PCI_BASE_ADDRESS_SPACE)
                == PCI_BASE_ADDRESS_SPACE_MEMORY)
        {
            nv->bars[NV_GPU_BAR_INDEX_REGS].offset = NVRM_PCICFG_BAR_OFFSET(i);
            nv->bars[NV_GPU_BAR_INDEX_REGS].cpu_address = NV_PCI_RESOURCE_START(pci_dev, i);
            nv->bars[NV_GPU_BAR_INDEX_REGS].size = NV_PCI_RESOURCE_SIZE(pci_dev, i);

            break;
        }
    }
    nv->regs = &nv->bars[NV_GPU_BAR_INDEX_REGS];

    pci_set_drvdata(pci_dev, (void *)nvl);

    /* default to 32-bit PCI bus address space */
    pci_dev->dma_mask = 0xffffffffULL;

    dma_set_max_seg_size(&pci_dev->dev, U32_MAX);

    nvl->dev               = &pci_dev->dev;
    nvl->pci_dev           = pci_dev;
    nvl->dma_dev.dev       = nvl->dev;

    nv->pci_info.vendor_id = pci_dev->vendor;
    nv->pci_info.device_id = pci_dev->device;
    nv->subsystem_id       = pci_dev->subsystem_device;
    nv->subsystem_vendor   = pci_dev->subsystem_vendor;
    nv->os_state           = (void *) nvl;
    nv->dma_dev            = &nvl->dma_dev;
    nv->pci_info.domain    = NV_PCI_DOMAIN_NUMBER(pci_dev);
    nv->pci_info.bus       = NV_PCI_BUS_NUMBER(pci_dev);
    nv->pci_info.slot      = NV_PCI_SLOT_NUMBER(pci_dev);
    nv->pci_info.function  = PCI_FUNC(pci_dev->devfn);
    nv->handle             = pci_dev;
    nv->flags             |= flags;

    if (nv_cxl_init(nvl) != 0)
        goto failed;

    if (!nv_lock_init_locks(sp, nv))
    {
        goto err_not_supported;
    }

    if ((rm_is_supported_device(sp, nv)) != NV_OK)
        goto err_not_supported;

    // Wire RM HAL
    if (!rm_init_private_state(sp, nv))
    {
        NV_DEV_PRINTF(NV_DBG_ERRORS, nv, "rm_init_private_state() failed!\n");
        goto err_zero_dev;
    }

    if (!nv->is_tegra_pci_igpu &&
        !pci_devid_is_self_hosted(pci_dev->device) &&
        !nv_pci_validate_bars(pci_dev, /* only_bar0 = */ NV_FALSE))
        goto err_zero_dev;

    if (nv_resize_pcie_bars(pci_dev)) {
        nv_printf(NV_DBG_ERRORS,
            "NVRM: Fatal Error while attempting to resize PCIe BARs.\n");
        goto err_zero_dev;
    }

    nvl->all_mappings_revoked = NV_TRUE;
    nvl->safe_to_mmap = NV_TRUE;
    nvl->gpu_wakeup_callback_needed = NV_TRUE;
    INIT_LIST_HEAD(&nvl->open_files);

    if (!nv->is_tegra_pci_igpu)
    {
        for (i = 0, j = 0; i < NVRM_PCICFG_NUM_BARS && j < NV_GPU_NUM_BARS; i++)
        {
            if (j == NV_GPU_BAR_INDEX_REGS)
            {
                j++;
                continue;
            }

            if ((NV_PCI_RESOURCE_VALID(pci_dev, i)) &&
                (NV_PCI_RESOURCE_FLAGS(pci_dev, i) & PCI_BASE_ADDRESS_SPACE)
                    == PCI_BASE_ADDRESS_SPACE_MEMORY)
            {
                nv->bars[j].offset = NVRM_PCICFG_BAR_OFFSET(i);
                nv->bars[j].cpu_address = NV_PCI_RESOURCE_START(pci_dev, i);
                nv->bars[j].size = NV_PCI_RESOURCE_SIZE(pci_dev, i);
                j++;
            }
        }
    }

    nv->fb   = &nv->bars[NV_GPU_BAR_INDEX_FB];
    nv->interrupt_line = pci_dev->irq;

    NV_ATOMIC_SET(nvl->numa_info.status, NV_IOCTL_NUMA_STATUS_DISABLED);
    nvl->numa_info.node_id = NUMA_NO_NODE;

#if NV_IS_EXPORT_SYMBOL_GPL_pci_ats_supported
    nv->ats_support = pci_ats_supported(nvl->pci_dev);
#else
    nv->ats_support = nvl->pci_dev->ats_enabled;
#endif

    if (nv->ats_support)
    {
        int ret __attribute__ ((unused));

        NV_DEV_PRINTF(NV_DBG_INFO, nv, "ATS supported by this GPU!\n");

#if NV_IS_EXPORT_SYMBOL_GPL_iommu_dev_enable_feature
#if defined(CONFIG_IOMMU_SVA) && \
    (defined(NV_IOASID_GET_PRESENT) || defined(NV_MM_PASID_DROP_PRESENT))
        ret = iommu_dev_enable_feature(nvl->dev, IOMMU_DEV_FEAT_SVA);
        if (ret == 0)
        {
            NV_DEV_PRINTF(NV_DBG_INFO, nv, "Enabled SMMU SVA feature! \n");
        }
        else if (ret == -EBUSY)
        {
            NV_DEV_PRINTF(NV_DBG_INFO, nv, "SMMU SVA feature already enabled!\n");
        }
        else
        {
            NV_DEV_PRINTF(NV_DBG_ERRORS, nv,
                          "Enabling SMMU SVA feature failed! ret: %d\n", ret);
            nv->ats_support = NV_FALSE;
        }
#else
        NV_DEV_PRINTF(NV_DBG_ERRORS, nv,
                      "Enabling SMMU SVA feature failed due to lack of necessary kernel configs.\n");
        nv->ats_support = NV_FALSE;
#endif
#endif // NV_IS_EXPORT_SYMBOL_GPL_iommu_dev_enable_feature
    }

    nv_clk_get_handles(nv);

    pci_set_master(pci_dev);

    if (os_tdisp_enabled)
    {
        nv->tdisp_dma_mapping_ready =
            nv_tdisp_probe_dma_mapping_readiness(nvl);
    }

    /*
     * Snapshot config for AER/DPC recovery. A fatal link-down reset leaves the
     * function at post-reset defaults; unlike a reboot, firmware/PCI bring-up
     * does not reprogram BAR registers and related config, so nv_pci_slot_reset
     * must restore a known-good image. Save here (after pci_set_master) so the
     * image matches post-probe config, not an earlier enum-time save. Skip when
     * the feature is disabled so a save failure does not produce a recovery-
     * related error log for opted-out systems.
     */
    if (NVreg_EnablePCIeErrorRecovery != 0)
    {
        if (pci_save_state(pci_dev) != 0)
        {
            NV_DEV_PRINTF(NV_DBG_ERRORS, nv,
                          "Failed to save PCI config state. PCIe error recovery will not be able to restore it\n");
        }
    }

    nv->primary_vga = ((NV_PCI_RESOURCE_FLAGS(pci_dev, PCI_ROM_RESOURCE) &
                        IORESOURCE_ROM_SHADOW) == IORESOURCE_ROM_SHADOW);

#if defined(CONFIG_VGA_ARB)
#if defined(VGA_DEFAULT_DEVICE)
#if defined(NV_VGA_TRYGET_PRESENT)
    vga_tryget(VGA_DEFAULT_DEVICE, VGA_RSRC_LEGACY_MASK);
#endif
#endif
    vga_set_legacy_decoding(pci_dev, VGA_RSRC_NONE);
#endif

    status = nv_check_gpu_state(nv);
    if (status == NV_ERR_GPU_IS_LOST)
    {
        NV_DEV_PRINTF(NV_DBG_INFO, nv, "GPU is lost, skipping nv_pci_probe\n");
        goto err_gpu_lost;
    }

    nv->cpu_numa_node_id = dev_to_node(nvl->dev);

    if (nv_platform_use_auto_online(nvl))
    {
        nv_init_gpu_numa_info(nv);
    }

    /* Resolve per-device init_on_probe from NVreg_GpuInitOnProbe policy. */
    nv_set_init_on_probe(nv);

    if (nv_linux_init_open_q(nvl) != 0)
    {
        NV_DEV_PRINTF(NV_DBG_ERRORS, nv, "nv_linux_init_open_q() failed!\n");
        goto err_gpu_lost;
    }

    nv_printf(NV_DBG_INFO,
              "NVRM: PCI:%04x:%02x:%02x.%x (%04x:%04x): BAR0 @ 0x%llx (%lluMB)\n",
              nv->pci_info.domain, nv->pci_info.bus, nv->pci_info.slot,
              PCI_FUNC(pci_dev->devfn), nv->pci_info.vendor_id, nv->pci_info.device_id,
              nv->regs->cpu_address, (nv->regs->size >> 20));
    nv_printf(NV_DBG_INFO,
              "NVRM: PCI:%04x:%02x:%02x.%x (%04x:%04x): BAR1 @ 0x%llx (%lluMB)\n",
              nv->pci_info.domain, nv->pci_info.bus, nv->pci_info.slot,
              PCI_FUNC(pci_dev->devfn), nv->pci_info.vendor_id, nv->pci_info.device_id,
              nv->fb->cpu_address, (nv->fb->size >> 20));

    atomic_inc(&num_nv_devices);

    pm_vt_switch_required(nvl->dev, NV_TRUE);

#if defined(CONFIG_PM_DEVFREQ)
    // Support dynamic power management if device is a tegra PCI iGPU
    rm_init_tegra_dynamic_power_management(sp, nv);
#endif
    nv_init_dynamic_power_management(sp, pci_dev);

    nv_init_tegra_gpu_pg_mask(sp, pci_dev);

    rm_get_gpu_uuid_raw(sp, nv);

    /* Parse and set any per-GPU dword registry keys specified. */
    nv_parse_per_device_option_string(sp, nv);

    /* Parse and set per-device binary registry key files. */
    {
        NV_STATUS parse_status = nv_parse_per_device_binary_option_string(sp, nv);
        if (parse_status != NV_OK)
        {
            NV_DEV_PRINTF(NV_DBG_WARNINGS, nv,
                          "Per-device binary registry key parsing failed during probe (status=0x%x)\n",
                          parse_status);
        }
    }

    rm_set_rm_firmware_requested(sp, nv);

    nv_check_and_exclude_gpu(sp, nv);

#if defined(DPM_FLAG_NO_DIRECT_COMPLETE)
    dev_pm_set_driver_flags(nvl->dev, DPM_FLAG_NO_DIRECT_COMPLETE);
#elif defined(DPM_FLAG_NEVER_SKIP)
    dev_pm_set_driver_flags(nvl->dev, DPM_FLAG_NEVER_SKIP);
#endif

#if defined(CONFIG_PM_DEVFREQ)
    /*
     * Expose clock control interface via devfreq framework for Tegra iGPU and
     * let the linux kernel itself to support all the clock scaling logic for
     * Tegra iGPU.
     *
     * On Tegra platforms, most of the clocks are managed by the BPMP. PMU inside
     * the iGPU does not have direct communication path to the BPMP, so using
     * existing clock management features (e.g. Pstates, PerfCf, and etc) with
     * PMU will not work.
     */
    if (nv_pci_tegra_register_devfreq(pci_dev) != 0)
    {
        NV_DEV_PRINTF(NV_DBG_ERRORS, nv, "Failed to register linux devfreq");
        goto err_add_device;
    };
#endif

    /*
     * Assign a minor number for the newly created nvl object
     */
    LOCK_NV_LINUX_DEVICES();
    if (nv_linux_assign_minor_locked(nvl) != 0)
    {
        UNLOCK_NV_LINUX_DEVICES();
        goto err_add_device;
    }
    UNLOCK_NV_LINUX_DEVICES();

    if (nvl->init_on_probe && !(nv->flags & NV_FLAG_EXCLUDE))
    {
        int ret;
        nv_printf(NV_DBG_SETUP, "NVRM: Starting device %04x:%02x:%02x.%x\n",
            NV_PCI_DOMAIN_NUMBER(pci_dev), NV_PCI_BUS_NUMBER(pci_dev),
            NV_PCI_SLOT_NUMBER(pci_dev), PCI_FUNC(pci_dev->devfn));

        ret = nv_start_device(nv, sp);
        if (ret)
        {
            /*
             * Log the failure but do not abort probe. The device will
             * remain registered so that it can still be managed even
             * though RM init did not succeed.
             */
            nv_printf(NV_DBG_ERRORS,
                "NVRM: GPU %04x:%02x:%02x.%x init failed during probe (ret=%d).\n",
                NV_PCI_DOMAIN_NUMBER(pci_dev), NV_PCI_BUS_NUMBER(pci_dev),
                NV_PCI_SLOT_NUMBER(pci_dev), PCI_FUNC(pci_dev->devfn), ret);
        }
    }

    /*
     * The newly created nvl object is added to the nv_linux_devices global list
     * only after all the initialization operations for that nvl object are
     * completed, so as to protect against simultaneous lookup operations which
     * may discover a partially initialized nvl object in the list
     */
    LOCK_NV_LINUX_DEVICES();
    nv_linux_add_device_locked(nvl);
    UNLOCK_NV_LINUX_DEVICES();

    /*
     * Create the procfs entry only after all the initialization operations for
     * the nvl object are completed and the device is added to the
     * nv_linux_devices global list
     */
    nv_procfs_add_gpu(nvl);

#if defined(NV_VGPU_KVM_BUILD)
    /*
     * vGPU VFIO probe requires the device to be added to the nv_linux_devices
     * global list since the vgpu mgr daemon invokes a open() call on the device
     * as part of the probe sequence
     */
    if (nvidia_vgpu_vfio_probe(nvl->pci_dev) != NV_OK)
    {
        NV_DEV_PRINTF(NV_DBG_ERRORS, nv, "Failed to register device to vGPU VFIO module");
        goto err_free_all;
    }
#endif

    /*
     * Notification must be sent only after all the initialization operations
     * as part of the probe sequence are complete.
     * Do not add code after this line which can fail nv_pci_probe().
     */
    rm_notify_gpu_addition(sp, nv);

    nvidia_modeset_probe(nvl);

    /*
     * Dynamic power management should be enabled as the last step.
     * Kernel runtime power management framework can put the device
     * into the suspended state. Hardware register access should not be done
     * after enabling dynamic power management.
     */
    rm_enable_dynamic_power_management(sp, nv);

    if (nv->ats_support)
        WRITE_ONCE(nv_ats_supported, NV_TRUE);
    else
        WRITE_ONCE(nv_non_ats_device_present, NV_TRUE);

    nv_kmem_cache_free_stack(sp);

    return 0;

#if defined(NV_VGPU_KVM_BUILD)
err_free_all:
#endif
    nv_procfs_remove_gpu(nvl);
    LOCK_NV_LINUX_DEVICES();
    nv_linux_remove_device_locked(nvl);
    UNLOCK_NV_LINUX_DEVICES();
    nv_stop_device(nv, sp);
#if defined(NV_VGPU_KVM_BUILD)
err_remove_minor:
#endif
    LOCK_NV_LINUX_DEVICES();
    nv_linux_remove_minor_locked(nvl);
    UNLOCK_NV_LINUX_DEVICES();
err_add_device:
    rm_cleanup_dynamic_power_management(sp, nv);
    pm_vt_switch_unregister(nvl->dev);
    nv_linux_stop_open_q(nvl);
err_gpu_lost:
    nv_clk_clear_handles(nv);
err_zero_dev:
    rm_free_private_state(sp, nv);
err_not_supported:
    nv_lock_destroy_locks(sp, nv);
failed:
    if (bar0_requested)
    {
        release_mem_region(NV_PCI_RESOURCE_START(pci_dev, regs_bar_index),
                           NV_PCI_RESOURCE_SIZE(pci_dev, regs_bar_index));
    }
    NV_PCI_DISABLE_DEVICE(pci_dev);
    pci_set_drvdata(pci_dev, NULL);
    if (nvl != NULL)
    {
        /* Probe failed: leave the platform-owned committed region intact. */
        nv_cxl_cleanup(nvl, NV_FALSE);
        NV_KFREE(nvl, sizeof(nv_linux_state_t));
    }
    nv_kmem_cache_free_stack(sp);
    return -1;
}

static int
nv_pci_probe_deferred_worker(void *data)
{
    nv_pci_probe_deferred_context_t *probe_ctx = data;

    probe_ctx->rc = nv_pci_probe_body(probe_ctx->pci_dev,
                                      probe_ctx->id_table);
    complete(&probe_ctx->done);

    return 0;
}

static int
nv_pci_probe
(
    struct pci_dev *pci_dev,
    const struct pci_device_id *id_table
)
{
    int rc;
    nv_pci_probe_deferred_context_t *probe_ctx = NULL;
    struct task_struct *thread = NULL;

    /*
     * Kthread deferral only helps during module-init when multiple probes
     * run concurrently via the async worker pool. Later bind/unbind from
     * sysfs or hotplug is synchronous, so the kthread overhead is not
     * worthwhile.
     */
    if (!smp_load_acquire(&nv_pci_driver_registration_in_progress))
    {
        return nv_pci_probe_body(pci_dev, id_table);
    }

    NV_KZALLOC(probe_ctx, sizeof(*probe_ctx));
    if (probe_ctx == NULL)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: failed to allocate probe deferral context for %s, falling back to inline probe\n", dev_name(&pci_dev->dev));
        return nv_pci_probe_body(pci_dev, id_table);
    }

    probe_ctx->pci_dev = pci_dev;
    probe_ctx->id_table = id_table;
    init_completion(&probe_ctx->done);

    /*
     * PROBE_PREFER_ASYNCHRONOUS only moves probe() onto the PCI core's async
     * worker pool, which is shared with other unbound kernel workqueues.
     * Under contention, running full nv_pci_probe_body() in that shared pool
     * hurts parallel initialization throughput.
     *
     * During module-init deferral window only, offload probe body to a
     * dedicated per-GPU kthread. We still join below so this probe callback
     * does not return before its own probe body finishes.
     */
    thread = kthread_run(nv_pci_probe_deferred_worker,
                         probe_ctx,
                         "nv-probe-%s",
                         dev_name(&pci_dev->dev));
    if (IS_ERR(thread))
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: failed to create probe deferral kthread for %s, falling back to inline probe\n", dev_name(&pci_dev->dev));
        NV_KFREE(probe_ctx, sizeof(*probe_ctx));
        return nv_pci_probe_body(pci_dev, id_table);
    }

    wait_for_completion(&probe_ctx->done);
    rc = probe_ctx->rc;

    NV_KFREE(probe_ctx, sizeof(*probe_ctx));
    return rc;
}

static void nv_pci_remove_helper(struct pci_dev *pci_dev, bool block_if_gpu_in_use)
{
    nv_linux_state_t *nvl = NULL;
    nv_state_t *nv;
    nvidia_stack_t *sp = NULL;
    NvU8 regs_bar_index = nv_bar_index_to_os_bar_index(pci_dev,
                                                       NV_GPU_BAR_INDEX_REGS);


#ifdef NV_PCI_SRIOV_SUPPORT
    if (pci_dev->is_virtfn)
    {
#if defined(NV_VGPU_KVM_BUILD)
        /* Arg 2 == NV_TRUE means that the PCI device should be removed */
        nvidia_vgpu_vfio_remove(pci_dev, NV_TRUE);
#endif /* NV_VGPU_KVM_BUILD */
        return;
    }
#endif /* NV_PCI_SRIOV_SUPPORT */

    nvl = pci_get_drvdata(pci_dev);
    if (!nvl || (nvl->pci_dev != pci_dev))
    {
        return;
    }

    nv = NV_STATE_PTR(nvl);

#if NV_IS_EXPORT_SYMBOL_GPL_iommu_dev_disable_feature
#if defined(CONFIG_IOMMU_SVA) && \
    (defined(NV_IOASID_GET_PRESENT) || defined(NV_MM_PASID_DROP_PRESENT))
    if (nv->ats_support)
    {
        int ret;

        ret = iommu_dev_disable_feature(nvl->dev, IOMMU_DEV_FEAT_SVA);
        if (ret == 0)
        {
            NV_DEV_PRINTF(NV_DBG_INFO, nv, "Disabled SMMU SVA feature! \n");
        }
        else
        {
            NV_DEV_PRINTF(NV_DBG_ERRORS, nv,
                          "Disabling SMMU SVA feature failed! ret: %d\n", ret);
        }
    }
#endif
#endif // NV_IS_EXPORT_SYMBOL_GPL_iommu_dev_disable_feature

    /*
     * Flush and stop open_q before proceeding with removal to ensure nvl
     * outlives all enqueued work items.
     */
    nv_linux_stop_open_q(nvl);

    nvidia_modeset_remove(nv->gpu_id);

    LOCK_NV_LINUX_DEVICES();
    down(&nvl->ldata_lock);

    if (!block_if_gpu_in_use && (atomic64_read(&nvl->usage_count) != 0))
    {
        up(&nvl->ldata_lock);
        UNLOCK_NV_LINUX_DEVICES();
        return;
    }

    if (nv_kmem_cache_alloc_stack(&sp) != 0)
    {
        up(&nvl->ldata_lock);
        UNLOCK_NV_LINUX_DEVICES();
        return;
    }

    nv_linux_remove_device_locked(nvl);

    rm_notify_gpu_removal(sp, nv);

    /*
     * Sanity check: A removed device shouldn't have a non-zero usage_count.
     * For eGPU, fall off the bus along with clients active is a valid scenario.
     * Hence skipping the sanity check for eGPU.
     */
    if ((atomic64_read(&nvl->usage_count) != 0) && !(nv->is_external_gpu))
    {
        nv_printf(NV_DBG_ERRORS,
                  "NVRM: Attempting to remove device %04x:%02x:%02x.%x with non-zero usage count!\n",
                  NV_PCI_DOMAIN_NUMBER(pci_dev), NV_PCI_BUS_NUMBER(pci_dev),
                  NV_PCI_SLOT_NUMBER(pci_dev), PCI_FUNC(pci_dev->devfn));

        /*
         * We can't return from this function without corrupting state, so we wait for
         * the usage count to go to zero.
         */
        while (atomic64_read(&nvl->usage_count) != 0)
        {
            /*
             * While waiting, release the locks so that other threads can make
             * forward progress.
             */
            up(&nvl->ldata_lock);
            UNLOCK_NV_LINUX_DEVICES();

            os_delay(500);

            /* Re-acquire the locks before checking again */
            LOCK_NV_LINUX_DEVICES();
            nvl = pci_get_drvdata(pci_dev);
            if (!nvl)
            {
                /* The device was not found, which should not happen */
                nv_printf(NV_DBG_ERRORS,
                          "NVRM: Failed removal of device %04x:%02x:%02x.%x!\n",
                          NV_PCI_DOMAIN_NUMBER(pci_dev), NV_PCI_BUS_NUMBER(pci_dev),
                          NV_PCI_SLOT_NUMBER(pci_dev), PCI_FUNC(pci_dev->devfn));
                WARN_ON(1);
                goto done;
            }
            nv = NV_STATE_PTR(nvl);
            down(&nvl->ldata_lock);
        }

        nv_printf(NV_DBG_ERRORS,
                  "NVRM: Continuing with GPU removal for device %04x:%02x:%02x.%x\n",
                  NV_PCI_DOMAIN_NUMBER(pci_dev), NV_PCI_BUS_NUMBER(pci_dev),
                  NV_PCI_SLOT_NUMBER(pci_dev), PCI_FUNC(pci_dev->devfn));
    }

    nvl->coherent_link_info.gpu_mem_info_discovery_attempted = NV_FALSE;

    rm_check_for_gpu_surprise_removal(sp, nv);

    /* Remove proc entry for this GPU */
    nv_procfs_remove_gpu(nvl);

#if defined(CONFIG_PM_DEVFREQ)
    nv_pci_tegra_unregister_devfreq(pci_dev);
#endif

    nv_clk_clear_handles(nv);

    nv_stop_device(nv, sp);

    rm_cleanup_dynamic_power_management(sp, nv);

    nv_linux_remove_minor_locked(nvl);
    nv->removed = NV_TRUE;

    UNLOCK_NV_LINUX_DEVICES();

    pm_vt_switch_unregister(&pci_dev->dev);

#if defined(NV_VGPU_KVM_BUILD)
    /* Arg 2 == NV_TRUE means that the PCI device should be removed */
    nvidia_vgpu_vfio_remove(pci_dev, NV_TRUE);
#endif

    if ((nv->flags & NV_FLAG_PERSISTENT_SW_STATE) || (nv->flags & NV_FLAG_INITIALIZED))
    {
        nv_acpi_unregister_notifier(nvl);
        if (nv->flags & NV_FLAG_PERSISTENT_SW_STATE)
        {
            rm_disable_gpu_state_persistence(sp, nv);
        }
        nv_shutdown_adapter(sp, nv, nvl);
        nv_dev_free_stacks(nvl);
    }

    if (nvl->sysfs_config_file != NULL)
    {
        filp_close(nvl->sysfs_config_file, NULL);
        nvl->sysfs_config_file = NULL;
    }

    if (atomic64_read(&nvl->usage_count) == 0)
    {
        nv_lock_destroy_locks(sp, nv);
    }

    atomic_dec(&num_probed_nv_devices);

    pci_set_drvdata(pci_dev, NULL);

    rm_i2c_remove_adapters(sp, nv);
    rm_free_private_state(sp, nv);

    nv_cxl_cleanup(nvl, NV_TRUE);

    /* BAR0 was never claimed for CXL devices; skip the matching release. */
    if (!nv_pcie_is_cxl(pci_dev))
    {
        release_mem_region(NV_PCI_RESOURCE_START(pci_dev, regs_bar_index),
                           NV_PCI_RESOURCE_SIZE(pci_dev, regs_bar_index));
    }

    atomic_dec(&num_nv_devices);

    if (atomic64_read(&nvl->usage_count) == 0)
    {
        NV_PCI_DISABLE_DEVICE(pci_dev);
        NV_KFREE(nvl, sizeof(nv_linux_state_t));
    }
    else
    {
        up(&nvl->ldata_lock);
    }

    nv_kmem_cache_free_stack(sp);
    return;

done:
    UNLOCK_NV_LINUX_DEVICES();
    nv_kmem_cache_free_stack(sp);
}

static void
nv_pci_remove(struct pci_dev *pci_dev)
{

    nv_printf(NV_DBG_SETUP, "NVRM: removing GPU %04x:%02x:%02x.%x\n",
              NV_PCI_DOMAIN_NUMBER(pci_dev), NV_PCI_BUS_NUMBER(pci_dev),
              NV_PCI_SLOT_NUMBER(pci_dev), PCI_FUNC(pci_dev->devfn));

    nv_pci_remove_helper(pci_dev, true);
}

static void
nv_pci_shutdown(struct pci_dev *pci_dev)
{
    nv_linux_state_t *nvl = pci_get_drvdata(pci_dev);
    
    if (!nvl || (nvl->pci_dev != pci_dev))
    {
        return;
    }

    if (nvl->is_forced_shutdown)
    {
        nvl->is_forced_shutdown = NV_FALSE;
        return;
    }

#if defined(CONFIG_PM_DEVFREQ)
    nv_pci_tegra_unregister_devfreq(pci_dev);
#endif

    nv_pci_remove_helper(pci_dev, false);

    if (pci_get_drvdata(pci_dev) != NULL)
    {
        nvl->nv_state.is_shutdown = NV_TRUE;
    }

    /* pci_clear_master is not defined for !CONFIG_PCI */
#ifdef CONFIG_PCI
    pci_clear_master(pci_dev);
#endif

    /* SHH HW mandates 1us delay to realise the effects of
     * Bus Mater Enable(BME) disable. Adding 1us delay for
     * all the chips as the delay is not in the data path
     * and not big. Creating HAL for this would be a overkill.
     */
    udelay(1);
}

int nvidia_dev_get_pci_info(const NvU8 *uuid, struct pci_dev **pci_dev_out,
    NvU64 *dma_start, NvU64 *dma_limit)
{
    nv_linux_state_t *nvl;

    /* Takes nvl->ldata_lock */
    nvl = find_uuid(uuid);
    if (!nvl)
        return -ENODEV;

    *pci_dev_out = nvl->pci_dev;
    *dma_start = nvl->dma_dev.addressable_range.start;
    *dma_limit = nvl->dma_dev.addressable_range.limit;

    up(&nvl->ldata_lock);

    return 0;
}

NvU8 nv_find_pci_capability(struct pci_dev *pci_dev, NvU8 capability)
{
    u16 status = 0;
    u8  cap_ptr = 0, cap_id = 0xff;

    pci_read_config_word(pci_dev, PCI_STATUS, &status);
    status &= PCI_STATUS_CAP_LIST;
    if (!status)
        return 0;

    switch (pci_dev->hdr_type) {
        case PCI_HEADER_TYPE_NORMAL:
        case PCI_HEADER_TYPE_BRIDGE:
            pci_read_config_byte(pci_dev, PCI_CAPABILITY_LIST, &cap_ptr);
            break;
        default:
            return 0;
    }

    do {
        cap_ptr &= 0xfc;
        pci_read_config_byte(pci_dev, cap_ptr + PCI_CAP_LIST_ID, &cap_id);
        if (cap_id == capability)
            return cap_ptr;
        pci_read_config_byte(pci_dev, cap_ptr + PCI_CAP_LIST_NEXT, &cap_ptr);
    } while (cap_ptr && cap_id != 0xff);

    return 0;
}

NV_STATUS
nv_pci_find_nvidia_dvsec_capability
(
    struct pci_dev *dev,
    NvU16 target_dvsec_id,
    NvU32 *dvsec_offset,
    NvU32 *dvsec_length
)
{
    u32 cap_offset = NV_PCI_EXT_CAP_START_OFFSET;
    u32 hdr0;
    u32 hdr1;
    u32 hdr2;
    u32 i;
    int rc;

    if ((dev == NULL) || (dvsec_offset == NULL) || (dvsec_length == NULL))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    /*
     * Iterate over the capabilities linked list until the target DVSEC is found
     * or we reach the end of the list (offset of 0).
     */

    for (i = 0; i < NV_PCI_EXT_CAP_MAX_ITERATIONS; i++)
    {
        /*
         * The capability offset should always be within the extended capability
         * space.
         */
        if ((cap_offset < NV_PCI_EXT_CAP_START_OFFSET) ||
            (cap_offset >= NV_PCI_EXT_CAP_END_OFFSET))
        {
            nv_printf(NV_DBG_ERRORS,
                      "NVRM: Malformed PCI extended capability chain while searching for NVIDIA DVSEC%u: offset 0x%x out of range\n",
                      target_dvsec_id, cap_offset);
            return NV_ERR_INVALID_OFFSET;
        }

        /*
         * The capability offset should always be aligned to DWORD boundary.
         */
        if (!NV_IS_ALIGNED(cap_offset, sizeof(NvU32)))
        {
            nv_printf(NV_DBG_ERRORS,
                      "NVRM: Malformed PCI extended capability chain while searching for NVIDIA DVSEC%u: offset 0x%x is not DWORD aligned\n",
                      target_dvsec_id, cap_offset);
            return NV_ERR_INVALID_OFFSET;
        }

        rc = pci_read_config_dword(dev, cap_offset, &hdr0);
        if (rc != 0)
        {
            nv_printf(NV_DBG_ERRORS,
                      "NVRM: Failed to read PCI extended capability header at offset 0x%x while searching for NVIDIA DVSEC%u, rc 0x%x\n",
                      cap_offset, target_dvsec_id, rc);
            return NV_ERR_INVALID_READ;
        }

        if (NV_PCI_EXT_CAP_ID(hdr0) == NV_PCI_EXT_CAP_ID_DVSEC)
        {
            rc = pci_read_config_dword(dev,
                                       (cap_offset + NV_PCI_DVSEC_HEADER_1_OFFSET),
                                       &hdr1);
            if (rc != 0)
            {
                nv_printf(NV_DBG_ERRORS,
                          "NVRM: Failed to read DVSEC header 1 at offset 0x%x while searching for NVIDIA DVSEC%u, rc 0x%x\n",
                          cap_offset, target_dvsec_id, rc);
                return NV_ERR_INVALID_READ;
            }

            rc = pci_read_config_dword(dev,
                                       (cap_offset + NV_PCI_DVSEC_HEADER_2_OFFSET),
                                       &hdr2);
            if (rc != 0)
            {
                nv_printf(NV_DBG_ERRORS,
                          "NVRM: Failed to read DVSEC header 2 at offset 0x%x while searching for NVIDIA DVSEC%u, rc 0x%x\n",
                          cap_offset, target_dvsec_id, rc);
                return NV_ERR_INVALID_READ;
            }

            if ((NV_PCI_DVSEC_VENDOR_ID(hdr1) == NV_PCI_DVSEC_NVIDIA_VENDOR_ID) &&
                (NV_PCI_DVSEC_DESIGNATED_ID(hdr2) == target_dvsec_id))
            {
                *dvsec_offset = cap_offset;
                *dvsec_length = NV_PCI_DVSEC_LEN(hdr1);
                return NV_OK;
            }
        }

        cap_offset = NV_PCI_EXT_CAP_NEXT_OFFSET(hdr0);
        if (cap_offset == 0)
        {
            nv_printf(NV_DBG_INFO, "NVRM: Failed to find NVIDIA DVSEC%u\n",
                      target_dvsec_id);
            return NV_ERR_NOT_SUPPORTED;
        }
    }

    /*
     * If we reach here, the loop maximum number of iterations was reached,
     * which likely indicates a malformed/cyclic PCI extended capability chain.
     */
    nv_printf(NV_DBG_ERRORS,
              "NVRM: Malformed PCI extended capability chain while searching for NVIDIA DVSEC%u: traversal limit reached\n",
              target_dvsec_id);
    return NV_ERR_CYCLE_DETECTED;
}

NV_STATUS NV_API_CALL
nv_pci_read_gpu_pdi_from_dvsec
(
    nv_state_t *nv,
    NvU64 *pdi
)
{
    NV_STATUS status;
    struct pci_dev *pdev;
    NvU32 dvsec_offset;
    NvU32 dvsec_length;
    u32 pdi_low;
    u32 pdi_high;
    u64 pdi_value;
    int rc;

    if ((nv == NULL) || (pdi == NULL))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    pdev = (struct pci_dev *)nv->handle;
    if (pdev == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    status = nv_pci_find_nvidia_dvsec_capability(pdev,
                                                 NV_PCI_DVSEC_GPU_PDI_DESIGNATED_ID,
                                                 &dvsec_offset,
                                                 &dvsec_length);
    if (status != NV_OK)
    {
        return status;
    }

    /* Verify the DVSEC length. */
    if (dvsec_length < NV_PCI_DVSEC_GPU_PDI_CAP_LEN)
    {
        nv_printf(NV_DBG_ERRORS,
                  "NVRM: NVIDIA DVSEC3 PDI capability length 0x%x is smaller than expected 0x%x\n",
                  dvsec_length, NV_PCI_DVSEC_GPU_PDI_CAP_LEN);
        return NV_ERR_INVALID_DATA;
    }

    /* Verify that the DVSEC offset is within the extended capability space. */
    if (dvsec_offset > (NV_PCI_EXT_CAP_END_OFFSET - NV_PCI_DVSEC_GPU_PDI_CAP_LEN))
    {
        nv_printf(NV_DBG_ERRORS,
                  "NVRM: NVIDIA DVSEC3 PDI capability offset 0x%x cannot contain expected length 0x%x\n",
                  dvsec_offset, NV_PCI_DVSEC_GPU_PDI_CAP_LEN);
        return NV_ERR_INVALID_OFFSET;
    }

    rc = pci_read_config_dword(pdev,
                               (dvsec_offset + NV_PCI_DVSEC_GPU_PDI_LOW_OFFSET),
                               &pdi_low);
    if (rc != 0)
    {
        nv_printf(NV_DBG_ERRORS,
                  "NVRM: Failed to read PDI low DWORD from NVIDIA DVSEC3 at offset 0x%x, rc 0x%x\n",
                  dvsec_offset, rc);
        return NV_ERR_INVALID_READ;
    }

    rc = pci_read_config_dword(pdev,
                               (dvsec_offset + NV_PCI_DVSEC_GPU_PDI_HIGH_OFFSET),
                               &pdi_high);
    if (rc != 0)
    {
        nv_printf(NV_DBG_ERRORS,
                  "NVRM: Failed to read PDI high DWORD from NVIDIA DVSEC3 at offset 0x%x, rc 0x%x\n",
                  dvsec_offset, rc);
        return NV_ERR_INVALID_READ;
    }

    pdi_value = (((u64)pdi_high) << 32) | (u64)pdi_low;

    /* Validate the PDI. */
    if ((pdi_value == 0) || (pdi_value == NV_U64_MAX))
    {
        nv_printf(NV_DBG_ERRORS,
                  "NVRM: Invalid PDI value 0x%llx found in NVIDIA DVSEC3\n",
                  pdi_value);
        return NV_ERR_NOT_SUPPORTED;
    }

    *pdi = pdi_value;
    return NV_OK;
}

static void check_for_bound_driver(struct pci_dev *pci_dev)
{
    if (pci_dev->dev.driver)
    {
        const char *driver_name = pci_dev->dev.driver->name;

        nv_printf(NV_DBG_WARNINGS, "NVRM: GPU %04x:%02x:%02x.%x is already "
            "bound to %s.\n",
            NV_PCI_DOMAIN_NUMBER(pci_dev), NV_PCI_BUS_NUMBER(pci_dev),
            NV_PCI_SLOT_NUMBER(pci_dev), PCI_FUNC(pci_dev->devfn),
            driver_name ? driver_name : "another driver"
        );
    }
}

void NV_API_CALL nv_pci_cxl_set_caching(nv_state_t *nv, NvBool cache_enable)
{
#if defined(PCI_DVSEC_CXL_DEVCAP_CACHE_CAPABLE)
    //
    // TODO: Bug 4921794, add proper conftest once these macros are added to upstream kernel
    // currently CXL headers are not present in upstream
    //
    u16 dvsec = 0;
    u16 cap;
    u16 reg;
    u16 rc;
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);
    struct pci_dev *pci_dev = nvl->pci_dev;

#if NV_IS_EXPORT_SYMBOL_GPL_pci_find_dvsec_capability
    dvsec = pci_find_dvsec_capability(pci_dev, PCI_VENDOR_ID_CXL,
                                      PCI_DVSEC_CXL_DEVICE);
#endif

    if (!dvsec)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: Invalid CXL DVSEC\n");
        return;
    }

    rc = pci_read_config_word(pci_dev, dvsec + PCI_DVSEC_CXL_DEVCAP, &cap);

    if (rc)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: failed to read PCI_DVSEC_CXL_DEVCAP rc 0x%x\n", rc);
        return;
    }

    if (!(cap & PCI_DVSEC_CXL_DEVCAP_CACHE_CAPABLE))
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: CXL device not CACHE capable\n");
        return;
    }

    rc = pci_read_config_word(pci_dev, dvsec + PCI_DVSEC_CXL_DEVCTL2, &reg);

    if (rc)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: failed to read PCI_DVSEC_CXL_DEVCTL2 rc 0x%x\n", rc);
        return;
    }

    // Disable caching and do WB &I
    if (!cache_enable)
    {
        nv_printf(NV_DBG_INFO, "NVRM: WB+I and Disabling CXL caching\n");

        reg |= PCI_DVSEC_CXL_DEVCTL2_DISABLE_CACHING;
        pci_write_config_word(pci_dev, dvsec + PCI_DVSEC_CXL_DEVCTL2, reg);

        if (cap & PCI_DVSEC_CXL_DEVCAP_CACHE_WB_INVALIDATE)
        {
            reg |= PCI_DVSEC_CXL_DEVCTL2_INIT_CACHE_WB_INVALIDATE;
            pci_write_config_word(pci_dev, dvsec + PCI_DVSEC_CXL_DEVCTL2, reg);
        }
    }
    // Enable caching
    else
    {
        nv_printf(NV_DBG_INFO, "NVRM: Enabling CXL caching\n");

        reg &= ~PCI_DVSEC_CXL_DEVCTL2_DISABLE_CACHING;
        pci_write_config_word(pci_dev, dvsec + PCI_DVSEC_CXL_DEVCTL2, reg);
    }

#endif
    return;
}

/* make sure the pci_driver called probe for all of our devices.
 * we've seen cases where rivafb claims the device first and our driver
 * doesn't get called.
 */
int
nv_pci_count_devices(void)
{
    struct pci_dev *pci_dev;
    int count = 0;

    if (NVreg_RegisterPCIDriver == 0)
    {
        return 0;
    }

    pci_dev = pci_get_class(PCI_CLASS_DISPLAY_VGA << 8, NULL);
    while (pci_dev)
    {
        if (rm_is_supported_pci_device(
                PCI_BASE_CLASS_DISPLAY,
                PCI_CLASS_DISPLAY_VGA & 0xFF,
                pci_dev->vendor,
                pci_dev->device,
                pci_dev->subsystem_vendor,
                pci_dev->subsystem_device,
                NV_TRUE /* print_legacy_warning */))
        {
            check_for_bound_driver(pci_dev);
            count++;
        }
        pci_dev = pci_get_class(PCI_CLASS_DISPLAY_VGA << 8, pci_dev);
    }

    pci_dev = pci_get_class(PCI_CLASS_DISPLAY_3D << 8, NULL);
    while (pci_dev)
    {
        if (rm_is_supported_pci_device(
                (pci_dev->class >> 16) & 0xFF,
                (pci_dev->class >> 8) & 0xFF,
                pci_dev->vendor,
                pci_dev->device,
                pci_dev->subsystem_vendor,
                pci_dev->subsystem_device,
                NV_TRUE /* print_legacy_warning */))
        {
            check_for_bound_driver(pci_dev);
            count++;
        }
        pci_dev = pci_get_class(PCI_CLASS_DISPLAY_3D << 8, pci_dev);
    }

    return count;
}

/*
 * On coherent platforms that support BAR1 mappings for GPUDirect RDMA,
 * dma-buf and nv-p2p subsystems need to ensure the 2 devices belong to
 * the same IOMMU group. If the importer device doesn't have an IOMMU
 * group assigned, at least check if the GPU and importer are under the same
 * root port.
 */
NvBool nv_pci_is_valid_topology_for_direct_pci(
    nv_state_t     *nv,
    struct pci_dev *peer
)
{
    struct pci_dev *pdev0 = to_pci_dev(nv->dma_dev->dev);
    struct pci_dev *pdev1 = peer;

    if (!nv->coherent)
    {
        nvidia_stack_t *sp = NULL;
        NvU32 allow_p2p = 0;

        // Experimental: NVreg_AllowP2PWithoutCoherentMapping=1 bypasses the gate.
        if (nv_kmem_cache_alloc_stack(&sp) != 0)
        {
            return NV_FALSE;
        }
        (void) rm_read_registry_dword(sp, nv,
                  NV_REG_ALLOW_P2P_WITHOUT_COHERENT_MAPPING, &allow_p2p);
        nv_kmem_cache_free_stack(sp);

        if (!allow_p2p)
        {
            return NV_FALSE;
        }
    }

    if (pdev0->dev.iommu_group == pdev1->dev.iommu_group)
        return NV_TRUE;

    if (pdev1->dev.iommu_group == NULL)
        return nv_pci_has_common_pci_switch(nv, peer);

    return NV_FALSE;
}

NvBool nv_pci_has_common_pci_switch(
    nv_state_t     *nv,
    struct pci_dev *peer
)
{
    struct pci_dev *pci_dev0, *pci_dev1;

    pci_dev0 = pci_upstream_bridge(to_pci_dev(nv->dma_dev->dev));

    while (pci_dev0 != NULL)
    {
        pci_dev1 = pci_upstream_bridge(peer);

        while (pci_dev1 != NULL)
        {
            if (pci_dev0 == pci_dev1)
                return NV_TRUE;

            pci_dev1 = pci_upstream_bridge(pci_dev1);
        }

        pci_dev0 = pci_upstream_bridge(pci_dev0);
    }

    return NV_FALSE;
}

NvBool NV_API_CALL nv_grdma_pci_topology_supported(
    nv_state_t      *nv,
    nv_dma_device_t *dma_peer
)
{
    //
    // Skip topo check on coherent platforms since
    // NIC can map over C2C anyway and PCIe topology shouldn't matter.
    //
    if (nv->coherent)
    {
        return NV_TRUE;
    }

    //
    // Allow RDMA by default on passthrough VMs and vGPU guests.
    // Guest RM sees the virtual chipset, not the host, so the P2P table
    // is not a valid topology signal.
    //
    if ((nv->flags & (NV_FLAG_PASSTHRU | NV_FLAG_VGPU_GUEST)) != 0)
        return NV_TRUE;

    //
    // Only allow RDMA on unsupported chipsets if there exists
    // a common PCI switch between the GPU and the other device
    //
    if ((nv->flags & NV_FLAG_PCI_P2P_UNSUPPORTED_CHIPSET) != 0)
        return nv_pci_has_common_pci_switch(nv, to_pci_dev(dma_peer->dev));

    return NV_TRUE;
}

static NvBool
nv_pci_read_tdisp_tdi_state
(
    struct pci_dev *pci_dev,
    NvU32          *dvsec0,
    NvU32          *tdi_state
)
{
    int ret;

    *dvsec0 = 0xffffffffU;
    *tdi_state = 0U;

    ret = pci_read_config_dword(
        pci_dev,
        NV_TDISP_AER_DVSEC0_OFFSET,
        dvsec0);

    if ((ret != PCIBIOS_SUCCESSFUL) || (*dvsec0 == 0xffffffffU))
        return NV_FALSE;

    *tdi_state = NV_TDISP_AER_TDI_STATE(*dvsec0);
    return NV_TRUE;
}

static void
nv_pci_report_error
(
    struct pci_dev *pci_dev,
    NvU32           xid,
    const char     *format,
    ...
)
{
    va_list ap;

    va_start(ap, format);
    nv_report_error(pci_dev, xid, format, ap);
    va_end(ap);
}

static void
nv_pci_report_tdisp_error
(
    struct pci_dev  *pci_dev,
    nv_linux_state_t *nvl
)
{
    nv_state_t *nv = NV_STATE_PTR(nvl);
    NvU32 dvsec0;
    NvU32 tdi_state;

    if (!nv_pci_read_tdisp_tdi_state(pci_dev, &dvsec0, &tdi_state))
    {
        NV_DEV_PRINTF(NV_DBG_ERRORS, nv,
                      "TDISP state unavailable during PCIe DPC containment\n");
        return;
    }

    if (tdi_state != NV_TDISP_AER_TDI_STATE_ERROR)
    {
        NV_DEV_PRINTF(NV_DBG_ERRORS, nv,
                      "PCIe DPC containment with TDI state=%u, DVSEC0=0x%08x\n",
                      tdi_state, dvsec0);
        return;
    }

    NV_DEV_PRINTF(NV_DBG_ERRORS, nv,
                  "TDISP ERROR during PCIe DPC containment: DVSEC0=0x%08x\n",
                  dvsec0);
    nv_pci_report_error(pci_dev,
                        NV_TDISP_AER_XID,
                        "TDISP ERROR detected during PCIe DPC containment, "
                        "DVSEC0=0x%08x, TDI_STATE=ERROR",
                        dvsec0);
}

static pci_ers_result_t
nv_pci_error_detected
(
    struct pci_dev         *pci_dev,
    nv_pci_channel_state_t  state
)
{
    //
    // Called under device_lock from the PCI error-recovery path. .remove and
    // .shutdown take the same lock, so once pci_get_drvdata returns a matching
    // nvl, that nvl and its embedded nv_state_t remain valid for this callback.
    //
    nv_linux_state_t *nvl = pci_get_drvdata(pci_dev);
    nv_state_t *nv;

    if ((nvl == NULL) || (nvl->pci_dev != pci_dev))
        return PCI_ERS_RESULT_DISCONNECT;

    nv = NV_STATE_PTR(nvl);

    if (state == pci_channel_io_normal)
    {
        NV_DEV_PRINTF(NV_DBG_INFO, nv,
                      "Nonfatal PCIe error detected; continuing recovery\n");
        return PCI_ERS_RESULT_CAN_RECOVER;
    }

    //
    // The GPU is off the bus. Fence RM API entry before anything else can
    // touch the device; see NV_IS_DEVICE_INACCESSIBLE().
    //
    // The flag is set so when GPU is unfrozen we still want to prevent accesses
    //
    nv->flags |= NV_FLAG_IN_PCIE_ERROR_RECOVERY;

    /*
     * Review-only PoC: DPC may already have made endpoint configuration
     * space inaccessible. Attempt exactly one DVSEC0 read and fail closed
     * when it fails or returns all ones. Never access BAR0, submit DMA, or
     * call osHandleGpuLost() from this callback.
     */
    if ((state == pci_channel_io_frozen) && os_tdisp_enabled &&
        (atomic_cmpxchg(&nvl->tdisp_aer_reported, 0, 1) == 0))
    {
        nv_pci_report_tdisp_error(pci_dev, nvl);
    }

    if (state == pci_channel_io_frozen)
    {
        NV_DEV_PRINTF(NV_DBG_ERRORS, nv,
                      "Fatal PCIe error detected; GPU marked inaccessible\n");
        //
        // NEED_RESET rather than DISCONNECT: pcie_do_recovery() broadcasts
        // slot_reset only when the merged status is exactly NEED_RESET.
        //
        return PCI_ERS_RESULT_NEED_RESET;
    }

    if (state == pci_channel_io_perm_failure)
    {
        //
        // Also reached as a consequence of our own DISCONNECT vote from
        // slot_reset: the kernel's recovery failure path re-enters
        // error_detected with a permanent failure state.
        //
        NV_DEV_PRINTF(NV_DBG_ERRORS, nv,
                      "PCIe link reported as permanently failed; GPU remains inaccessible\n");
    }
    else
    {
        NV_DEV_PRINTF(NV_DBG_ERRORS, nv,
                      "Unrecognized PCIe channel state %u; treating as fatal and marking GPU inaccessible\n",
                      (unsigned)state);
    }

    return PCI_ERS_RESULT_DISCONNECT;
}

static pci_ers_result_t
nv_pci_slot_reset
(
    struct pci_dev *pci_dev
)
{
    nv_linux_state_t *nvl;
    nv_state_t *nv;
    u32 id;
    u32 expected_id;

    /*
     * Kernels without commit 45bc82563d55 ("PCI/ERR: Update device error_state
     * already after reset", v6.18) restore error_state only from report_resume,
     * which our DISCONNECT vote skips. Perform the frozen -> normal transition
     * here so the device does not read as pci_channel_offline() forever.
     * cmpxchg both matches how the PCI core owns this field and can never
     * clobber pci_channel_io_perm_failure, and it is a harmless no-op where the
     * core has already done the transition.
     */
    cmpxchg(&pci_dev->error_state, pci_channel_io_frozen,
            pci_channel_io_normal);

    //
    // Called under device_lock from the PCI error-recovery path. .remove and
    // .shutdown take the same lock, so once pci_get_drvdata returns a matching
    // nvl, that nvl and its embedded nv_state_t remain valid for this callback.
    //
    nvl = pci_get_drvdata(pci_dev);
    if ((nvl == NULL) || (nvl->pci_dev != pci_dev))
        return PCI_ERS_RESULT_DISCONNECT;

    nv = NV_STATE_PTR(nvl);

    expected_id = (u32)pci_dev->vendor | ((u32)pci_dev->device << 16);
    if (pci_bus_read_config_dword(pci_dev->bus, pci_dev->devfn,
                                  PCI_VENDOR_ID, &id) != 0)
    {
        NV_DEV_PRINTF(NV_DBG_ERRORS, nv,
                      "PCI identity validation failed after slot reset (config read failed, expected=0x%08x)\n",
                      expected_id);
        return PCI_ERS_RESULT_DISCONNECT;
    }

    if (id != expected_id)
    {
        NV_DEV_PRINTF(NV_DBG_ERRORS, nv,
                      "PCI identity validation failed after slot reset (id=0x%08x, expected=0x%08x)\n",
                      id, expected_id);
        return PCI_ERS_RESULT_DISCONNECT;
    }

    /*
     * The link reset left the function at post-reset defaults, and neither the
     * kernel's recovery path nor a later rebind reprograms the BARs, so restore
     * the snapshot taken during probe. Bus mastering stays off so the lost GPU
     * cannot DMA, and the state is resaved that way so a subsequent
     * pci_reset_function() does not bring it back.
     */
    pci_restore_state(pci_dev);
    pci_clear_master(pci_dev);

    if (pci_save_state(pci_dev) != 0)
    {
        NV_DEV_PRINTF(NV_DBG_ERRORS, nv,
                      "Failed to resave PCI config state after slot reset\n");
    }

    NV_DEV_PRINTF(NV_DBG_ERRORS, nv,
                  "PCIe link reset after DPC/AER containment. The GPU is no longer usable and requires a reset before it will accept new work.\n");

    //
    // DISCONNECT so Linux never resumes this RM instance. Recovery is an
    // operator-driven unbind, sysfs reset and rebind.
    //
    return PCI_ERS_RESULT_DISCONNECT;
}

static const struct pci_error_handlers nv_pci_error_handlers = {
    .error_detected = nv_pci_error_detected,
    .slot_reset     = nv_pci_slot_reset,
};

#if defined(CONFIG_PM)
extern struct dev_pm_ops nv_pm_ops;
#endif

struct pci_driver nv_pci_driver = {
    .name      = MODULE_NAME,
    .id_table  = nv_pci_table,
    .probe     = nv_pci_probe,
    .remove    = nv_pci_remove,
    .shutdown  = nv_pci_shutdown,
#if defined(NV_USE_VFIO_PCI_CORE) && \
  defined(NV_PCI_DRIVER_HAS_DRIVER_MANAGED_DMA)
    .driver_managed_dma = NV_TRUE,
#endif
#if defined(CONFIG_PM)
    .driver.pm = &nv_pm_ops,
#endif
#if NV_PCI_ASYNC_PROBE_SUPPORTED
    .driver.probe_type = PROBE_PREFER_ASYNCHRONOUS,
#else
    .driver.probe_type = PROBE_FORCE_SYNCHRONOUS,
#endif
};

void nv_pci_wait_for_probe_complete(void)
{
    if (NVreg_RegisterPCIDriver == 0)
    {
        return;
    }

#if NV_PCI_ASYNC_PROBE_SUPPORTED
    /*
     * With PROBE_PREFER_ASYNCHRONOUS, pci_register_driver() may return
     * before all probes complete. Wait for all pending probes to finish
     * so that num_probed_nv_devices and num_nv_devices are final before
     * post-registration validation in nvidia_init_module().
     *
     * wait_for_device_probe() is system-wide (not NVIDIA-specific) but
     * is the standard kernel API for this purpose.
     */
    wait_for_device_probe();
#endif
}

void nv_pci_unregister_driver(void)
{
    if (NVreg_RegisterPCIDriver == 0)
    {
        return;
    }
    return pci_unregister_driver(&nv_pci_driver);
}

int nv_pci_register_driver(void)
{
    int rc;

    if (NVreg_RegisterPCIDriver == 0)
    {
        return 0;
    }

    //
    // Attach the error handlers here rather than statically, so that with the
    // feature disabled the kernel sees a handler-less driver and behaves exactly
    // as it did before these callbacks existed.
    //
    if (NVreg_EnablePCIeErrorRecovery != 0)
    {
        nv_pci_driver.err_handler = &nv_pci_error_handlers;
    }

    smp_store_release(&nv_pci_driver_registration_in_progress, NV_TRUE);

    rc = pci_register_driver(&nv_pci_driver);

    if (rc == 0)
    {
        nv_pci_wait_for_probe_complete();
    }

    smp_store_release(&nv_pci_driver_registration_in_progress, NV_FALSE);

    return rc;
}
