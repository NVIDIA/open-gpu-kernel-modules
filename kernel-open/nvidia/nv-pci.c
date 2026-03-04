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

#include "nv-pci-table.h"
#include "nv-pci-types.h"
#include "nv-pci.h"
#include "nv-msi.h"
#include "nv-hypervisor.h"
#include "nv-reg.h"

#if defined(NV_VGPU_KVM_BUILD)
#include "nv-vgpu-vfio-interface.h"
#endif
#include <linux/iommu.h>

#include <linux/clk.h>
#include <linux/device.h>
#include <linux/jiffies.h>
#include <linux/kernel.h>
#include <linux/workqueue.h>

#if defined(CONFIG_PM_DEVFREQ)
#include <linux/devfreq.h>

#if defined(CONFIG_DEVFREQ_THERMAL) \
    && defined(NV_DEVFREQ_DEV_PROFILE_HAS_IS_COOLING_DEVICE) \
    && defined(NV_THERMAL_ZONE_FOR_EACH_TRIP_PRESENT) \
    && defined(NV_THERMAL_BIND_CDEV_TO_TRIP_PRESENT) \
    && defined(NV_THERMAL_UNBIND_CDEV_FROM_TRIP_PRESENT)
#include <linux/thermal.h>
#define NV_HAS_COOLING_SUPPORTED 1
#else
#define NV_HAS_COOLING_SUPPORTED 0
#endif

#endif

#if defined(CONFIG_INTERCONNECT) \
    && defined(NV_ICC_SET_BW_PRESENT) \
    && defined(NV_DEVM_ICC_GET_PRESENT)
#include <linux/interconnect.h>
#define NV_HAS_ICC_SUPPORTED 1
#else
#define NV_HAS_ICC_SUPPORTED 0
#endif

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

extern int NVreg_GrdmaPciTopoCheckOverride;

static void
nv_check_and_exclude_gpu(
    nvidia_stack_t *sp,
    nv_state_t *nv
)
{
    char *uuid_str;

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
 *
 * find_gpu_numa_nodes_in_srat() is strongly associated with
 * nv_init_coherent_link_info(). Hence matching the conditions wrapping.
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

static void
nv_init_coherent_link_info
(
    nv_state_t *nv
)
{
#if defined(CONFIG_ACPI_NUMA) && NV_IS_EXPORT_SYMBOL_PRESENT_pxm_to_node
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);
    NvU64 pa = 0;
    NvU64 pxm_start = 0;
    NvU64 pxm_count = 0;
    NvU32 pxm;
    NvU32 gi_found = 0, node;

    if (!NVCPU_IS_AARCH64)
        return;

    if (!dev_is_pci(nvl->dev))
        return;

    gi_found = find_gpu_numa_nodes_in_srat(nvl);

    if (!gi_found &&
        (device_property_read_u64(nvl->dev, "nvidia,gpu-mem-pxm-start", &pxm_start) != 0 ||
         device_property_read_u64(nvl->dev, "nvidia,gpu-mem-pxm-count", &pxm_count) != 0))
        goto failed;

    if (device_property_read_u64(nvl->dev, "nvidia,gpu-mem-base-pa", &pa) == 0)
    {
        nvl->coherent_link_info.gpu_mem_pa = pa;
    }
    else
    {
        unsigned int gpu_bar1_offset, gpu_bar2_offset;

        /*
         * This implies that the DSD key for PXM start and count is present
         * while the one for Physical Address (PA) is absent.
         */
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


    NV_DEV_PRINTF(NV_DBG_INFO, nv, "DSD properties: \n");
    NV_DEV_PRINTF(NV_DBG_INFO, nv, "\tGPU memory PA: 0x%lx \n",
                  nvl->coherent_link_info.gpu_mem_pa);
    if (pci_devid_is_self_hosted_hopper(nv->pci_info.device_id))
    {
        NV_DEV_PRINTF(NV_DBG_INFO, nv, "\tGPU reserved memory PA: 0x%lx \n",
                      nvl->coherent_link_info.rsvd_mem_pa);
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

    for (node = 0; (node = find_next_bit(nvl->coherent_link_info.free_node_bitmap,
                             MAX_NUMNODES, node)) != MAX_NUMNODES; node++)
    {
        NV_DEV_PRINTF(NV_DBG_INFO, nv, "\tNVRM: GPU memory NUMA node: %u\n", node);
    }

    if (NVreg_EnableUserNUMAManagement && !os_is_vgx_hyper())
    {
        NV_ATOMIC_SET(nvl->numa_info.status, NV_IOCTL_NUMA_STATUS_OFFLINE);
        nvl->numa_info.use_auto_online = NV_TRUE;

        if (!bitmap_empty(nvl->coherent_link_info.free_node_bitmap, MAX_NUMNODES))
        {
            nvl->numa_info.node_id = find_first_bit(nvl->coherent_link_info.free_node_bitmap, MAX_NUMNODES);
        }
        NV_DEV_PRINTF(NV_DBG_SETUP, nv, "GPU NUMA information: node id: %u PA: 0x%llx\n",
                      nvl->numa_info.node_id, nvl->coherent_link_info.gpu_mem_pa);
    }
    else
    {
        NV_DEV_PRINTF(NV_DBG_SETUP, nv, "User-mode NUMA onlining disabled.\n");
    }

    return;

failed:
    NV_DEV_PRINTF(NV_DBG_SETUP, nv, "Cannot get coherent link info.\n");
#endif // defined(CONFIG_ACPI_NUMA) && NV_IS_EXPORT_SYMBOL_PRESENT_pxm_to_node
    return;
}

#if defined(CONFIG_PM_DEVFREQ)

#define to_tegra_devfreq_dev(x) \
    container_of(x, struct nv_pci_tegra_devfreq_dev, dev)

struct nv_pci_tegra_devfreq_data {
    const char *clk_name;
    const char *icc_name;
    const unsigned int gpc_fuse_field;
    const TEGRASOC_DEVFREQ_CLK devfreq_clk;
};

#if NV_HAS_COOLING_SUPPORTED
struct nv_pci_tegra_thermal_data {
    const char *tz_name;
    const struct thermal_trip *passive_trip;
    struct list_head zones;
};
#endif

struct nv_pci_tegra_devfreq_dev {
    TEGRASOC_DEVFREQ_CLK devfreq_clk;
    int domain;
    struct device dev;
    struct list_head gpc_cluster;
    struct list_head nvd_cluster;
    struct nv_pci_tegra_devfreq_dev *gpc_master;
    struct nv_pci_tegra_devfreq_dev *nvd_master;
    struct clk *clk;
    struct devfreq *devfreq;
    bool boost_enabled;
    struct delayed_work boost_disable;
#if NV_HAS_ICC_SUPPORTED
    struct icc_path *icc_path;
#endif
#if NV_HAS_COOLING_SUPPORTED
    struct list_head therm_zones;
#endif
};

static const struct nv_pci_tegra_devfreq_data gb10b_tegra_devfreq_table[] = {
    {
        .clk_name = "gpc0clk",
        .icc_name = "gpu-write",
        .gpc_fuse_field = BIT(3),
        .devfreq_clk = TEGRASOC_DEVFREQ_CLK_GPC,
    },
    {
        .clk_name = "gpc1clk",
        .icc_name = "gpu-write",
        .gpc_fuse_field = BIT(4),
        .devfreq_clk = TEGRASOC_DEVFREQ_CLK_GPC,
    },
    {
        .clk_name = "gpc2clk",
        .icc_name = "gpu-write",
        .gpc_fuse_field = BIT(5),
        .devfreq_clk = TEGRASOC_DEVFREQ_CLK_GPC,
    },
    {
        .clk_name = "nvdclk",
        .icc_name = "video-write",
        .devfreq_clk = TEGRASOC_DEVFREQ_CLK_NVD,
    },
    {
        .clk_name = "sysclk"
    },
    {
        .clk_name = "uprocclk"
    },
};

static void nv_pci_gb10b_device_release(struct device *dev)
{
    ;
}

static int
nv_pci_gb10b_devfreq_target(struct device *dev, unsigned long *freq, u32 flags)
{
    struct pci_dev *pdev = to_pci_dev(dev->parent);
    nv_linux_state_t *nvl = pci_get_drvdata(pdev);
    struct nv_pci_tegra_devfreq_dev *tdev = to_tegra_devfreq_dev(dev), *tptr;
    unsigned long rate;
#if NV_HAS_ICC_SUPPORTED
    static const unsigned num_mss_ports = 8;
    static const unsigned mss_port_bandwidth = 32;
    static const unsigned gpu_bus_bandwidth = num_mss_ports * mss_port_bandwidth;
    u32 kBps;
#endif

    //
    // When GPU is suspended(railgated), the PM runtime suspend callback should
    // suspend all devfreq devices, and devfreq cycle should not be triggered.
    //
    // However, users are still able to change the devfreq governor from the
    // sysfs interface and indirectly invoke the update_devfreq function, which
    // will further call the target callback function.
    //
    // Early stop the process here before clk_set_rate/clk_get_rate, since these
    // calls served by BPMP will awake the GPU.
    //
    if (pm_runtime_suspended(&pdev->dev))
    {
        return 0;
    }

    clk_set_rate(tdev->clk, *freq);
    *freq = clk_get_rate(tdev->clk);

#if NV_HAS_ICC_SUPPORTED
    if (tdev->icc_path != NULL)
    {
        kBps = Bps_to_icc(*freq * gpu_bus_bandwidth * 400 / 1000);
        if (tdev->boost_enabled)
        {
            kBps = UINT_MAX;
        }
        icc_set_bw(tdev->icc_path, kBps, 0);
    }
#endif

    rate = 0;
    list_for_each_entry(tptr, &tdev->gpc_cluster, gpc_cluster)
    {
        if (tptr->gpc_master != NULL)
        {
            rate = max(rate, clk_get_rate(tptr->gpc_master->clk));
        }

        if (tptr->nvd_master != NULL)
        {
            rate = max(rate, clk_get_rate(tptr->nvd_master->clk));
        }

        if (tdev->boost_enabled
            && (tptr == nvl->sys_devfreq_dev || tptr == nvl->pwr_devfreq_dev))
        {
            clk_set_rate(tptr->clk, ULONG_MAX);
        }
        else
        {
            clk_set_rate(tptr->clk, rate);
        }
    }

    rate = 0;
    list_for_each_entry(tptr, &tdev->nvd_cluster, nvd_cluster)
    {
        if (tptr->gpc_master != NULL)
        {
            rate = max(rate, clk_get_rate(tptr->gpc_master->clk));
        }

        if (tptr->nvd_master != NULL)
        {
            rate = max(rate, clk_get_rate(tptr->nvd_master->clk));
        }

        if (tdev->boost_enabled
            && (tptr == nvl->sys_devfreq_dev || tptr == nvl->pwr_devfreq_dev))
        {
            clk_set_rate(tptr->clk, ULONG_MAX);
        }
        else
        {
            clk_set_rate(tptr->clk, rate);
        }
    }

    return 0;
}

static int
nv_pci_tegra_devfreq_get_cur_freq(struct device *dev, unsigned long *freq)
{
    struct nv_pci_tegra_devfreq_dev *tdev = to_tegra_devfreq_dev(dev);

    *freq = clk_get_rate(tdev->clk);

    return 0;
}

static int
nv_pci_tegra_devfreq_get_dev_status(struct device *dev,
                                    struct devfreq_dev_status *stat)
{
    struct pci_dev *pdev = to_pci_dev(dev->parent);
    nv_linux_state_t *nvl = pci_get_drvdata(pdev);
    nv_state_t *nv = NV_STATE_PTR(nvl);
    nvidia_stack_t *sp = NULL;
    struct nv_pci_tegra_devfreq_dev *tdev = to_tegra_devfreq_dev(dev);
    unsigned int load = 0;
    int retval = 0;
    NV_STATUS status;

    //
    // When GPU is suspended(railgated), the PM runtime suspend callback should
    // suspend all devfreq devices, and devfreq cycle should not be triggered.
    //
    // However, users are still able to change the devfreq governor from the
    // sysfs interface and indirectly invoke the update_devfreq function, which
    // will further call the get_dev_status callback function.
    //
    if (pm_runtime_suspended(&pdev->dev))
    {
        stat->total_time = 100;
        stat->busy_time = 0;
        stat->current_frequency = clk_get_rate(tdev->clk);
        return 0;
    }

    retval = nv_kmem_cache_alloc_stack(&sp);
    if (retval != 0)
    {
        dev_warn(&pdev->dev, "fail to nv_kmem_cache_alloc_stack: %d\n", retval);
        return -ENOMEM;
    }

    //
    // Fetch the load value in percentage from the specified clock domain. If the
    // load information is unavailable, just consider the load as 100% so that the
    // devfreq core will scale the underlying clock to Fmax to prevent any
    // performance drop.
    //
    status = rm_pmu_perfmon_get_load(sp, nv, &load, tdev->devfreq_clk);
    if (status != NV_OK)
    {
        load = 100;
    }

    // Load calculation equals to (busy_time / total_time) in devfreq governors
    // and devfreq governors expect total_time and busy_time in the same unit
    stat->total_time = 100;
    stat->busy_time = load;
    stat->current_frequency = clk_get_rate(tdev->clk);

    nv_kmem_cache_free_stack(sp);

    return retval;
}

static void
populate_opp_table(struct nv_pci_tegra_devfreq_dev *tdev)
{
    unsigned long max_rate, min_rate, step, rate;
    long val;

    /* Get the max rate of the clock */
    val = clk_round_rate(tdev->clk, ULONG_MAX);
    max_rate = (val < 0) ? ULONG_MAX : (unsigned long)val;

    /* Get the min rate of the clock */
    val = clk_round_rate(tdev->clk, 0);
    min_rate = (val < 0) ? ULONG_MAX : (unsigned long)val;

    /* Get the step size of the clock */
    step = (min_rate == ULONG_MAX) ? ULONG_MAX : (min_rate + 1);
    val = clk_round_rate(tdev->clk, step);
    if ((val < 0) || (val < min_rate))
    {
        step = 0;
    }
    else
    {
        step = (unsigned long)val - min_rate;
    }

    /* Create the OPP table */
    rate = min_rate;
    do {
        dev_pm_opp_add(&tdev->dev, rate, 0);
        rate += step;
    } while (rate <= max_rate && step);
}

static void
nv_pci_tegra_devfreq_remove_opps(struct nv_pci_tegra_devfreq_dev *tdev)
{
#if defined(NV_DEVFREQ_HAS_FREQ_TABLE)
    unsigned long *freq_table = tdev->devfreq->freq_table;
    unsigned int max_state = tdev->devfreq->max_state;
#else
    unsigned long *freq_table = tdev->devfreq->profile->freq_table;
    unsigned int max_state = tdev->devfreq->profile->max_state;
#endif
    int i;

    for (i = 0; i < max_state; i++)
    {
        dev_pm_opp_remove(&tdev->dev, freq_table[i]);
    }
}

#if NV_HAS_COOLING_SUPPORTED
static int
nv_pci_tegra_thermal_get_passive_trip_cb(struct thermal_trip *trip, void *arg)
{
    const struct thermal_trip **ptrip = arg;

    /* Return zero to continue the search */
    if (trip->type != THERMAL_TRIP_PASSIVE)
        return 0;

    /* Return nonzero to terminate the search */
    *ptrip = trip;
    return -1;
}

static int
nv_pci_tegra_init_cooling_device(struct nv_pci_tegra_devfreq_dev *tdev)
{
    struct device *pdev = tdev->dev.parent;
    const struct thermal_trip *passive_trip = NULL;
    struct devfreq *devfreq = tdev->devfreq;
    struct nv_pci_tegra_thermal_data *data;
    struct thermal_zone_device *tzdev;
    int i, err, val, n_strings, n_elems;
    u32 temp_min, temp_max;
    const char *tz_name;

    if (!devfreq->cdev)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: devfreq cooling cannot be found\n");
        return -ENODEV;
    }

    if (!pdev->of_node)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: associated OF node cannot be found\n");
        return -ENODEV;
    }

    val = of_property_count_strings(pdev->of_node, "nvidia,thermal-zones");
    if (val == -EINVAL)
    {
        return 0;
    }
    else if (val < 0)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: nvidia,thermal-zones DT property format error\n");
        return val;
    }
    n_strings = val;

    val = of_property_count_u32_elems(pdev->of_node, "nvidia,cooling-device");
    if (val == -EINVAL)
    {
        return 0;
    }
    else if (val < 0)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: nvidia,cooling-device DT property format error\n");
        return val;
    }
    n_elems = val;

    if ((n_elems >> 1) != n_strings)
    {
        nv_printf(NV_DBG_ERRORS,
            "NVRM: number of strings specified in nvidia,thermal-zones needs to"
            "be exact half the number of elements specified nvidia,cooling-device\n");
        return -EINVAL;
    }

    if (((n_elems >> 1) == 0) && ((n_elems & 1) == 1))
    {
        nv_printf(NV_DBG_ERRORS,
            "NVRM: number of elements specified in nvidia,cooling-device needs"
            "to be an even number\n");
        return -EINVAL;
    }

    for (i = 0; i < n_strings; i++)
    {
        data = devm_kzalloc(pdev, sizeof(*data), GFP_KERNEL);
        if (data == NULL)
        {
            err = -ENOMEM;
            goto err_nv_pci_tegra_init_cooling_device;
        }

        of_property_read_string_index(pdev->of_node,
                                      "nvidia,thermal-zones", i, &tz_name);
        of_property_read_u32_index(pdev->of_node,
                                   "nvidia,cooling-device", (i << 1) + 0, &temp_min);
        of_property_read_u32_index(pdev->of_node,
                                   "nvidia,cooling-device", (i << 1) + 1, &temp_max);

        tzdev = thermal_zone_get_zone_by_name(tz_name);
        if (IS_ERR(tzdev))
        {
            nv_printf(NV_DBG_ERRORS,
                "NVRM: fail to get %s thermal_zone_device\n", tz_name);
            err = -ENODEV;
            goto err_nv_pci_tegra_init_cooling_device;
        }

        thermal_zone_for_each_trip(tzdev, nv_pci_tegra_thermal_get_passive_trip_cb, &passive_trip);
        if (passive_trip == NULL)
        {
            nv_printf(NV_DBG_ERRORS,
                "NVRM: fail to find passive_trip in %s thermal_zone_device\n", tz_name);
            err = -ENODEV;
            goto err_nv_pci_tegra_init_cooling_device;
        }

        val = thermal_bind_cdev_to_trip(tzdev,
                                        passive_trip,
                                        devfreq->cdev,
                                        temp_max, temp_min, THERMAL_WEIGHT_DEFAULT);
        if (val != 0)
        {
            nv_printf(NV_DBG_ERRORS,
                "NVRM: fail to bind devfreq cooling device with %s thermal_zone_device\n", tz_name);
            err = -ENODEV;
            goto err_nv_pci_tegra_init_cooling_device;
        }

        data->tz_name = tz_name;
        data->passive_trip = passive_trip;
        list_add_tail(&data->zones, &tdev->therm_zones);
    }

    return 0;

err_nv_pci_tegra_init_cooling_device:
    list_for_each_entry(data, &tdev->therm_zones, zones)
    {
        tzdev = thermal_zone_get_zone_by_name(data->tz_name);
        if (IS_ERR(tzdev))
        {
            continue;
        }

        thermal_unbind_cdev_from_trip(tzdev, data->passive_trip, devfreq->cdev);
    }

    return err;
}
#endif

static int
nv_pci_gb10b_add_devfreq_device(struct nv_pci_tegra_devfreq_dev *tdev)
{
    struct devfreq_dev_profile *profile;
    int err;

    populate_opp_table(tdev);

    profile = devm_kzalloc(&tdev->dev, sizeof(*profile), GFP_KERNEL);
    if (profile == NULL)
    {
        err = -ENOMEM;
        goto err_nv_pci_gb10b_add_devfreq_device_opp;
    }

    profile->target = nv_pci_gb10b_devfreq_target;
    profile->get_cur_freq = nv_pci_tegra_devfreq_get_cur_freq;
    profile->get_dev_status = nv_pci_tegra_devfreq_get_dev_status;
    profile->initial_freq = clk_get_rate(tdev->clk);
    profile->polling_ms = 25;
#if NV_HAS_COOLING_SUPPORTED
    profile->is_cooling_device = true;
#endif

    tdev->devfreq = devm_devfreq_add_device(&tdev->dev,
                                            profile,
                                            DEVFREQ_GOV_PERFORMANCE,
                                            NULL);
    if (IS_ERR(tdev->devfreq))
    {
        err = PTR_ERR(tdev->devfreq);
        goto err_nv_pci_gb10b_add_devfreq_device_opp;
    }

#if defined(NV_DEVFREQ_HAS_SUSPEND_FREQ)
    tdev->devfreq->suspend_freq = tdev->devfreq->scaling_max_freq;
#endif

#if NV_HAS_COOLING_SUPPORTED
    err = nv_pci_tegra_init_cooling_device(tdev);
    if (err)
    {
        goto err_nv_pci_gb10b_add_devfreq_device;
    }
#endif

    return 0;

#if NV_HAS_COOLING_SUPPORTED
err_nv_pci_gb10b_add_devfreq_device:
    devm_devfreq_remove_device(&tdev->dev, tdev->devfreq);
#endif
err_nv_pci_gb10b_add_devfreq_device_opp:
    nv_pci_tegra_devfreq_remove_opps(tdev);

    return err;
}

static int
nv_pci_gb10b_register_devfreq(struct pci_dev *pdev)
{
    nv_linux_state_t *nvl = pci_get_drvdata(pdev);
    nv_state_t *nv = NV_STATE_PTR(nvl);
    struct pci_bus *pbus = pdev->bus;
    const struct nv_pci_tegra_devfreq_data *tdata;
    struct nv_pci_tegra_devfreq_dev *tdev;
#if NV_HAS_ICC_SUPPORTED
    struct icc_path *icc_path;
#endif
    struct clk *clk;
    int i, err, node;
    u32 gpu_pg_mask;

    while (pbus->parent != NULL)
    {
        pbus = pbus->parent;
    }

    node = max(0, dev_to_node(to_pci_host_bridge(pbus->bridge)->dev.parent));

    if (nv->tegra_pci_igpu_pg_mask == NV_TEGRA_PCI_IGPU_PG_MASK_DEFAULT)
    {
        gpu_pg_mask = 0;
    }
    else
    {
        gpu_pg_mask = nv->tegra_pci_igpu_pg_mask;
        nv_printf(NV_DBG_INFO, "NVRM: devfreq register receives gpu_pg_mask = %u\n", gpu_pg_mask);
    }

    for (i = 0; i < nvl->devfreq_table_size; i++)
    {
        tdata = &nvl->devfreq_table[i];

        if (gpu_pg_mask && (gpu_pg_mask & tdata->gpc_fuse_field))
        {
            continue;
        }

#if NV_HAS_ICC_SUPPORTED
        if (tdata->icc_name != NULL)
        {
            icc_path = devm_of_icc_get(&pdev->dev, tdata->icc_name);
            if (IS_ERR_OR_NULL(icc_path))
            {
                icc_path = NULL;
            }
        }
#endif

        clk = devm_clk_get(&pdev->dev, tdata->clk_name);
        if (IS_ERR_OR_NULL(clk))
        {
            continue;
        }

        tdev = devm_kzalloc(&pdev->dev, sizeof(*tdev), GFP_KERNEL);
        if (tdev == NULL)
        {
            return -ENOMEM;
        }

        INIT_LIST_HEAD(&tdev->gpc_cluster);
        INIT_LIST_HEAD(&tdev->nvd_cluster);
#if NV_HAS_COOLING_SUPPORTED
        INIT_LIST_HEAD(&tdev->therm_zones);
#endif
#if NV_HAS_ICC_SUPPORTED
        tdev->icc_path = icc_path;
#endif
        tdev->clk = clk;
        tdev->domain = node;
        tdev->devfreq_clk = tdata->devfreq_clk;

        tdev->dev.parent = &pdev->dev;
        tdev->dev.release = nv_pci_gb10b_device_release;
        dev_set_name(&tdev->dev, "%s-%d", tdata->clk_name, node);

        if (strstr(tdata->clk_name, "gpc"))
        {
            if (nvl->gpc_devfreq_dev != NULL)
            {
                list_add_tail(&tdev->gpc_cluster, &nvl->gpc_devfreq_dev->gpc_cluster);
                tdev->gpc_master = nvl->gpc_devfreq_dev;
            }
            else
            {
                nvl->gpc_devfreq_dev = tdev;
                dev_set_name(&tdev->dev, "gpu-gpc-%d", node);
            }
        }
        else if (strstr(tdata->clk_name, "nvd"))
        {
            nvl->nvd_devfreq_dev = tdev;
            dev_set_name(&tdev->dev, "gpu-nvd-%d", node);
        }
        else if (strstr(tdata->clk_name, "sys"))
        {
            nvl->sys_devfreq_dev = tdev;
            dev_set_name(&tdev->dev, "gpu-sys-%d", node);
        }
        else if (strstr(tdata->clk_name, "uproc"))
        {
            nvl->pwr_devfreq_dev = tdev;
            dev_set_name(&tdev->dev, "gpu-pwr-%d", node);
        }

        err = device_register(&tdev->dev);
        if (err != 0)
        {
            goto error_return;
        }
    }

    if (nvl->gpc_devfreq_dev != NULL)
    {
        err = nv_pci_gb10b_add_devfreq_device(nvl->gpc_devfreq_dev);
        if (err != 0)
        {
            nvl->gpc_devfreq_dev->devfreq = NULL;
            goto error_slave_teardown;
        }

        if (nvl->sys_devfreq_dev != NULL)
        {
            list_add_tail(&nvl->sys_devfreq_dev->gpc_cluster, &nvl->gpc_devfreq_dev->gpc_cluster);
            nvl->sys_devfreq_dev->gpc_master = nvl->gpc_devfreq_dev;
        }

        if (nvl->pwr_devfreq_dev != NULL)
        {
            list_add_tail(&nvl->pwr_devfreq_dev->gpc_cluster, &nvl->gpc_devfreq_dev->gpc_cluster);
            nvl->pwr_devfreq_dev->gpc_master = nvl->gpc_devfreq_dev;
        }
    }

    if (nvl->nvd_devfreq_dev != NULL)
    {
        err = nv_pci_gb10b_add_devfreq_device(nvl->nvd_devfreq_dev);
        if (err != 0)
        {
            nvl->nvd_devfreq_dev->devfreq = NULL;
            goto error_slave_teardown;
        }

        if (nvl->sys_devfreq_dev != NULL)
        {
            list_add_tail(&nvl->sys_devfreq_dev->nvd_cluster, &nvl->nvd_devfreq_dev->nvd_cluster);
            nvl->sys_devfreq_dev->nvd_master = nvl->nvd_devfreq_dev;
        }

        if (nvl->pwr_devfreq_dev != NULL)
        {
            list_add_tail(&nvl->pwr_devfreq_dev->nvd_cluster, &nvl->nvd_devfreq_dev->nvd_cluster);
            nvl->pwr_devfreq_dev->nvd_master = nvl->nvd_devfreq_dev;
        }
    }

    return 0;

error_slave_teardown:
    if (nvl->sys_devfreq_dev != NULL)
    {
        if (nvl->sys_devfreq_dev->gpc_master != NULL)
        {
            list_del(&nvl->sys_devfreq_dev->gpc_cluster);
            nvl->sys_devfreq_dev->gpc_master = NULL;
        }

        device_unregister(&nvl->sys_devfreq_dev->dev);
        nvl->sys_devfreq_dev = NULL;
    }
    if (nvl->pwr_devfreq_dev != NULL)
    {
        if (nvl->pwr_devfreq_dev->gpc_master != NULL)
        {
            list_del(&nvl->pwr_devfreq_dev->gpc_cluster);
            nvl->pwr_devfreq_dev->gpc_master = NULL;
        }

        device_unregister(&nvl->pwr_devfreq_dev->dev);
        nvl->pwr_devfreq_dev = NULL;
    }
error_return:
    /* The caller will call unregister to unwind on failure */
    return err;
}

static int
nv_pci_gb10b_suspend_devfreq(struct device *dev)
{
    struct pci_dev *pci_dev = to_pci_dev(dev);
    nv_linux_state_t *nvl = pci_get_drvdata(pci_dev);
    int err = 0;

    if (nvl->gpc_devfreq_dev != NULL && nvl->gpc_devfreq_dev->devfreq != NULL)
    {
        err = devfreq_suspend_device(nvl->gpc_devfreq_dev->devfreq);
        if (err)
        {
            return err;
        }

#if NV_HAS_ICC_SUPPORTED
        if (nvl->gpc_devfreq_dev->icc_path != NULL)
        {
            icc_set_bw(nvl->gpc_devfreq_dev->icc_path, 0, 0);
        }
#endif
    }

    if (nvl->nvd_devfreq_dev != NULL && nvl->nvd_devfreq_dev->devfreq != NULL)
    {
        err = devfreq_suspend_device(nvl->nvd_devfreq_dev->devfreq);
        if (err)
        {
            return err;
        }

#if NV_HAS_ICC_SUPPORTED
        if (nvl->nvd_devfreq_dev->icc_path != NULL)
        {
            icc_set_bw(nvl->nvd_devfreq_dev->icc_path, 0, 0);
        }
#endif
    }

    return err;
}

static int
nv_pci_gb10b_resume_devfreq(struct device *dev)
{
    struct pci_dev *pci_dev = to_pci_dev(dev);
    nv_linux_state_t *nvl = pci_get_drvdata(pci_dev);
    int err = 0;

    if (nvl->gpc_devfreq_dev != NULL && nvl->gpc_devfreq_dev->devfreq != NULL)
    {
        err = devfreq_resume_device(nvl->gpc_devfreq_dev->devfreq);
        if (err)
        {
            return err;
        }
    }

    if (nvl->nvd_devfreq_dev != NULL && nvl->nvd_devfreq_dev->devfreq != NULL)
    {
        err = devfreq_resume_device(nvl->nvd_devfreq_dev->devfreq);
        if (err)
        {
            return err;
        }
    }

    return err;
}

static void nv_pci_devfreq_disable_boost(struct work_struct *work)
{
#if defined(NV_UPDATE_DEVFREQ_PRESENT)
    struct nv_pci_tegra_devfreq_dev *tdev;

    tdev = container_of(work, struct nv_pci_tegra_devfreq_dev, boost_disable.work);
    tdev->boost_enabled = 0;
#endif
}

static int
nv_pci_gb10b_devfreq_enable_boost(struct device *dev, unsigned int duration)
{
#if defined(NV_UPDATE_DEVFREQ_PRESENT)
    struct pci_dev *pci_dev = to_pci_dev(dev);
    nv_linux_state_t *nvl = pci_get_drvdata(pci_dev);
    struct nv_pci_tegra_devfreq_dev *tdev;
    unsigned long delay;

    if (duration == 0)
        return 0;

    delay = msecs_to_jiffies(duration * 1000);

    tdev = nvl->gpc_devfreq_dev;
    if (tdev != NULL && tdev->devfreq != NULL && tdev->boost_enabled == 0)
    {
        tdev->boost_enabled = 1;

        INIT_DELAYED_WORK(&tdev->boost_disable, nv_pci_devfreq_disable_boost);
        schedule_delayed_work(&tdev->boost_disable, delay);
    }

    tdev = nvl->nvd_devfreq_dev;
    if (tdev != NULL && tdev->devfreq != NULL && tdev->boost_enabled == 0)
    {
        tdev->boost_enabled = 1;

        INIT_DELAYED_WORK(&tdev->boost_disable, nv_pci_devfreq_disable_boost);
        schedule_delayed_work(&tdev->boost_disable, delay);
    }

    return 0;
#else // !defined(NV_UPDATE_DEVFREQ_PRESENT)
    return -1;
#endif
}

static int
nv_pci_gb10b_devfreq_disable_boost(struct device *dev)
{
#if defined(NV_UPDATE_DEVFREQ_PRESENT)
    struct pci_dev *pci_dev = to_pci_dev(dev);
    nv_linux_state_t *nvl = pci_get_drvdata(pci_dev);
    struct nv_pci_tegra_devfreq_dev *tdev;

    tdev = nvl->gpc_devfreq_dev;
    if (tdev != NULL && tdev->devfreq != NULL && tdev->boost_enabled)
    {
        tdev->boost_enabled = 0;
        cancel_delayed_work_sync(&tdev->boost_disable);
    }

    tdev = nvl->nvd_devfreq_dev;
    if (tdev != NULL && tdev->devfreq != NULL && tdev->boost_enabled)
    {
        tdev->boost_enabled = 0;
        cancel_delayed_work_sync(&tdev->boost_disable);
    }

    return 0;
#else // !defined(NV_UPDATE_DEVFREQ_PRESENT)
    return -1;
#endif
}

struct nv_pci_tegra_data {
    unsigned short vendor;
    unsigned short device;
    const struct nv_pci_tegra_devfreq_data *devfreq_table;
    unsigned int devfreq_table_size;
    int (*devfreq_register)(struct pci_dev*);
    int (*devfreq_suspend)(struct device*);
    int (*devfreq_resume)(struct device*);
    int (*devfreq_enable_boost)(struct device*, unsigned int);
    int (*devfreq_disable_boost)(struct device*);
};

static const struct nv_pci_tegra_data nv_pci_tegra_table[] = {
    {
        .vendor = 0x10de,
        .device = 0x2b00,
        .devfreq_table = gb10b_tegra_devfreq_table,
        .devfreq_table_size = ARRAY_SIZE(gb10b_tegra_devfreq_table),
        .devfreq_register = nv_pci_gb10b_register_devfreq,
        .devfreq_suspend = nv_pci_gb10b_suspend_devfreq,
        .devfreq_resume = nv_pci_gb10b_resume_devfreq,
        .devfreq_enable_boost = nv_pci_gb10b_devfreq_enable_boost,
        .devfreq_disable_boost = nv_pci_gb10b_devfreq_disable_boost,
    },
};

static void
nv_pci_tegra_devfreq_remove(struct nv_pci_tegra_devfreq_dev *tdev)
{
    struct nv_pci_tegra_devfreq_dev *tptr, *next;
#if NV_HAS_COOLING_SUPPORTED
    struct nv_pci_tegra_thermal_data *data;
    struct thermal_zone_device *tzdev;
#endif

    if (tdev->devfreq != NULL)
    {
#if NV_HAS_COOLING_SUPPORTED
        list_for_each_entry(data, &tdev->therm_zones, zones)
        {
            tzdev = thermal_zone_get_zone_by_name(data->tz_name);
            if (IS_ERR(tzdev))
            {
                continue;
            }

            thermal_unbind_cdev_from_trip(tzdev, data->passive_trip, tdev->devfreq->cdev);
        }
#endif
        devm_devfreq_remove_device(&tdev->dev, tdev->devfreq);
        nv_pci_tegra_devfreq_remove_opps(tdev);
        tdev->devfreq = NULL;
    }

#if NV_HAS_ICC_SUPPORTED
    if (tdev->icc_path != NULL)
    {
        icc_set_bw(tdev->icc_path, 0, 0);
    }
#endif

    list_for_each_entry_safe(tptr, next, &tdev->gpc_cluster, gpc_cluster)
    {
        if (tptr->clk != NULL)
        {
            devm_clk_put(tdev->dev.parent, tptr->clk);
            tptr->clk = NULL;
            device_unregister(&tptr->dev);
        }

        list_del(&tptr->gpc_cluster);
        tptr->gpc_master = NULL;
    }

    list_for_each_entry_safe(tptr, next, &tdev->nvd_cluster, nvd_cluster)
    {
        if (tptr->clk != NULL)
        {
            devm_clk_put(tdev->dev.parent, tptr->clk);
            tptr->clk = NULL;
            device_unregister(&tptr->dev);
        }

        list_del(&tptr->nvd_cluster);
        tptr->nvd_master = NULL;
    }

    if (tdev->clk != NULL)
    {
        devm_clk_put(tdev->dev.parent, tptr->clk);
        tdev->clk = NULL;
        device_unregister(&tdev->dev);
    }
}

static void
nv_pci_tegra_unregister_devfreq(struct pci_dev *pdev)
{
    nv_linux_state_t *nvl = pci_get_drvdata(pdev);

    if (nvl->gpc_devfreq_dev != NULL)
    {
        nv_pci_tegra_devfreq_remove(nvl->gpc_devfreq_dev);
        nvl->gpc_devfreq_dev = NULL;
    }

    if (nvl->nvd_devfreq_dev != NULL)
    {
        nv_pci_tegra_devfreq_remove(nvl->nvd_devfreq_dev);
        nvl->nvd_devfreq_dev = NULL;
    }
}

static const struct nv_pci_tegra_data*
nv_pci_get_tegra_igpu_data(struct pci_dev *pdev)
{
    const struct nv_pci_tegra_data *tegra_data = NULL;
    int i;

    for (i = 0; i < ARRAY_SIZE(nv_pci_tegra_table); i++)
    {
        tegra_data = &nv_pci_tegra_table[i];

        if ((tegra_data->vendor == pdev->vendor)
            && (tegra_data->device == pdev->device))
        {
            return tegra_data;
        }
    }

    return NULL;
}

static int
nv_pci_tegra_register_devfreq(struct pci_dev *pdev)
{
    nv_linux_state_t *nvl = pci_get_drvdata(pdev);
    const struct nv_pci_tegra_data *tegra_data = NULL;
    int err;

    tegra_data = nv_pci_get_tegra_igpu_data(pdev);

    if (tegra_data == NULL)
    {
        return 0;
    }

    nvl->devfreq_table = tegra_data->devfreq_table;
    nvl->devfreq_table_size = tegra_data->devfreq_table_size;
    nvl->devfreq_suspend = tegra_data->devfreq_suspend;
    nvl->devfreq_resume = tegra_data->devfreq_resume;
    nvl->devfreq_enable_boost = tegra_data->devfreq_enable_boost;
    nvl->devfreq_disable_boost = tegra_data->devfreq_disable_boost;

    err = tegra_data->devfreq_register(pdev);
    if (err != 0)
    {
        nv_pci_tegra_unregister_devfreq(pdev);
        return err;
    }

    return 0;
}
#endif

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

/* find nvidia devices and set initial state */
static int
nv_pci_probe
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
    NvBool prev_nv_ats_supported = nv_ats_supported;
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

    num_probed_nv_devices++;

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

    NV_KZALLOC(nvl, sizeof(nv_linux_state_t));
    if (nvl == NULL)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: failed to allocate memory\n");
        goto err_not_supported;
    }

    nv  = NV_STATE_PTR(nvl);
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
    nv->handle             = pci_dev;
    nv->flags             |= flags;

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

    if (pci_devid_is_self_hosted(pci_dev->device))
    {
        nv_init_coherent_link_info(nv);
    }

    nv_ats_supported |= nv->ats_support;

    nv_clk_get_handles(nv);

    pci_set_master(pci_dev);

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

    num_nv_devices++;

    /*
     * The newly created nvl object is added to the nv_linux_devices global list
     * only after all the initialization operations for that nvl object are
     * completed, so as to protect against simultaneous lookup operations which
     * may discover a partially initialized nvl object in the list
     */
    LOCK_NV_LINUX_DEVICES();

    if (nv_linux_add_device_locked(nvl) != 0)
    {
        UNLOCK_NV_LINUX_DEVICES();
        goto err_add_device;
    }

    UNLOCK_NV_LINUX_DEVICES();

    pm_vt_switch_required(nvl->dev, NV_TRUE);

#if defined(CONFIG_PM_DEVFREQ)
    // Support dynamic power management if device is a tegra PCI iGPU
    rm_init_tegra_dynamic_power_management(sp, nv);
#endif
    nv_init_dynamic_power_management(sp, pci_dev);

    nv_init_tegra_gpu_pg_mask(sp, pci_dev);

    rm_get_gpu_uuid_raw(sp, nv);

    nv_procfs_add_gpu(nvl);

    /* Parse and set any per-GPU registry keys specified. */
    nv_parse_per_device_option_string(sp);

    rm_set_rm_firmware_requested(sp, nv);

#if defined(NV_VGPU_KVM_BUILD)
    if (nvidia_vgpu_vfio_probe(nvl->pci_dev) != NV_OK)
    {
        NV_DEV_PRINTF(NV_DBG_ERRORS, nv, "Failed to register device to vGPU VFIO module");
        goto err_free_all;
    }
#endif

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
        goto err_free_all;
    };
#endif

    /*
     * Dynamic power management should be enabled as the last step.
     * Kernel runtime power management framework can put the device
     * into the suspended state. Hardware register access should not be done
     * after enabling dynamic power management.
     */
    rm_enable_dynamic_power_management(sp, nv);

    /*
     * This must be the last action in nv_pci_probe(). Do not add code after this line.
     */
    rm_notify_gpu_addition(sp, nv);

    nv_kmem_cache_free_stack(sp);

    nvidia_modeset_probe(nvl);

    return 0;

goto err_free_all;
err_free_all:
    nv_procfs_remove_gpu(nvl);
    rm_cleanup_dynamic_power_management(sp, nv);
    pm_vt_switch_unregister(nvl->dev);
    LOCK_NV_LINUX_DEVICES();
    nv_linux_remove_device_locked(nvl);
    UNLOCK_NV_LINUX_DEVICES();
err_add_device:
    nv_linux_stop_open_q(nvl);
err_gpu_lost:
    nv_clk_clear_handles(nv);
    nv_ats_supported = prev_nv_ats_supported;
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
        NV_KFREE(nvl, sizeof(nv_linux_state_t));
    }
    nv_kmem_cache_free_stack(sp);
    return -1;
}

static void
nv_pci_remove(struct pci_dev *pci_dev)
{
    nv_linux_state_t *nvl = NULL;
    nv_state_t *nv;
    nvidia_stack_t *sp = NULL;
    NvU8 regs_bar_index = nv_bar_index_to_os_bar_index(pci_dev,
                                                       NV_GPU_BAR_INDEX_REGS);

    nv_printf(NV_DBG_SETUP, "NVRM: removing GPU %04x:%02x:%02x.%x\n",
              NV_PCI_DOMAIN_NUMBER(pci_dev), NV_PCI_BUS_NUMBER(pci_dev),
              NV_PCI_SLOT_NUMBER(pci_dev), PCI_FUNC(pci_dev->devfn));

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

    if (nv_kmem_cache_alloc_stack(&sp) != 0)
    {
        return;
    }

    nvl = pci_get_drvdata(pci_dev);
    if (!nvl || (nvl->pci_dev != pci_dev))
    {
        nv_kmem_cache_free_stack(sp);
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
    nv->flags |= NV_FLAG_PCI_REMOVE_IN_PROGRESS;

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

    rm_check_for_gpu_surprise_removal(sp, nv);

    nv_linux_remove_device_locked(nvl);

    /* Remove proc entry for this GPU */
    nv_procfs_remove_gpu(nvl);

#if defined(CONFIG_PM_DEVFREQ)
    nv_pci_tegra_unregister_devfreq(pci_dev);
#endif

    nv_clk_clear_handles(nv);

    rm_cleanup_dynamic_power_management(sp, nv);

    nv->removed = NV_TRUE;

    UNLOCK_NV_LINUX_DEVICES();

    pm_vt_switch_unregister(&pci_dev->dev);

#if defined(NV_VGPU_KVM_BUILD)
    /* Arg 2 == NV_TRUE means that the PCI device should be removed */
    nvidia_vgpu_vfio_remove(pci_dev, NV_TRUE);
#endif

    if ((nv->flags & NV_FLAG_PERSISTENT_SW_STATE) || (nv->flags & NV_FLAG_OPEN))
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

    num_probed_nv_devices--;

    pci_set_drvdata(pci_dev, NULL);

    rm_i2c_remove_adapters(sp, nv);
    rm_free_private_state(sp, nv);
    release_mem_region(NV_PCI_RESOURCE_START(pci_dev, regs_bar_index),
                       NV_PCI_RESOURCE_SIZE(pci_dev, regs_bar_index));

    num_nv_devices--;

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
nv_pci_shutdown(struct pci_dev *pci_dev)
{
    nv_linux_state_t *nvl = pci_get_drvdata(pci_dev);

    if (nvl != NULL)
    {
        nv_state_t *nv = NV_STATE_PTR(nvl);

        if (nvl->is_forced_shutdown)
        {
            nvl->is_forced_shutdown = NV_FALSE;
            return;
        }

        nvidia_modeset_remove(nv->gpu_id);

        nvl->nv_state.is_shutdown = NV_TRUE;
    }

#if defined(CONFIG_PM_DEVFREQ)
    nv_pci_tegra_unregister_devfreq(pci_dev);
#endif

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

/*!
 * @brief This function accepts pci information corresponding to a GPU
 * and returns a reference to the nv_linux_state_t corresponding to that GPU.
 *
 * @param[in] domain            Pci domain number for the GPU to be found.
 * @param[in] bus               Pci bus number for the GPU to be found.
 * @param[in] slot              Pci slot number for the GPU to be found.
 * @param[in] function          Pci function number for the GPU to be found.
 *
 * @return Pointer to nv_linux_state_t for the GPU if it is found, or NULL otherwise.
 */
nv_linux_state_t * find_pci(NvU32 domain, NvU8 bus, NvU8 slot, NvU8 function)
{
    nv_linux_state_t *nvl = NULL;

    LOCK_NV_LINUX_DEVICES();

    for (nvl = nv_linux_devices; nvl != NULL; nvl = nvl->next)
    {
        nv_state_t *nv = NV_STATE_PTR(nvl);

        if (nv->pci_info.domain == domain &&
            nv->pci_info.bus == bus &&
            nv->pci_info.slot == slot &&
            nv->pci_info.function == function)
        {
            break;
        }
    }

    UNLOCK_NV_LINUX_DEVICES();
    return nvl;
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
 * the same IOMMU group.
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
        return NV_FALSE;
    }

    switch (NVreg_GrdmaPciTopoCheckOverride) {
        case NV_REG_GRDMA_PCI_TOPO_CHECK_OVERRIDE_ALLOW_ACCESS:
            return NV_TRUE;
        case NV_REG_GRDMA_PCI_TOPO_CHECK_OVERRIDE_DENY_ACCESS:
            return NV_FALSE;
        default:
           return (pdev0->dev.iommu_group == pdev1->dev.iommu_group);
    }
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

    switch (NVreg_GrdmaPciTopoCheckOverride)
    {
        case NV_REG_GRDMA_PCI_TOPO_CHECK_OVERRIDE_ALLOW_ACCESS:
            return NV_TRUE;
        case NV_REG_GRDMA_PCI_TOPO_CHECK_OVERRIDE_DENY_ACCESS:
            return NV_FALSE;
        default:
            break;
    }

    // Allow RDMA by default on passthrough VMs.
    if ((nv->flags & NV_FLAG_PASSTHRU) != 0)
        return NV_TRUE;

    //
    // Only allow RDMA on unsupported chipsets if there exists
    // a common PCI switch between the GPU and the other device
    //
    if ((nv->flags & NV_FLAG_PCI_P2P_UNSUPPORTED_CHIPSET) != 0)
        return nv_pci_has_common_pci_switch(nv, to_pci_dev(dma_peer->dev));

    return NV_TRUE;
}

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
    .driver.probe_type = PROBE_FORCE_SYNCHRONOUS,
};

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
    if (NVreg_RegisterPCIDriver == 0)
    {
        return 0;
    }
    return pci_register_driver(&nv_pci_driver);
}
