/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>

#if defined NV_LINUX_OF_DEVICE_H_PRESENT
#include <linux/of_device.h>
#endif

#include "nv-platform.h"
#include "nv-linux.h"
#include <linux/reset.h>

static irqreturn_t
nvidia_soc_isr_kthread_bh(
    int irq,
    void *data
)
{
    NV_STATUS status;
    irqreturn_t ret;
    nv_state_t *nv = (nv_state_t *) data;
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);
    NvU32 irq_count;
    unsigned long flags;

    //
    // Synchronize kthreads servicing bottom halves
    //
    status = os_acquire_mutex(nvl->soc_bh_mutex);

    // os_acquire_mutex can only fail if we cannot sleep and we can
    WARN_ON(status != NV_OK);

    ret = nvidia_isr_kthread_bh(irq, data);

    os_release_mutex(nvl->soc_bh_mutex);

    NV_SPIN_LOCK_IRQSAVE(&nvl->soc_isr_lock, flags);
    for (irq_count = 0; irq_count < nv->num_soc_irqs; irq_count++)
    {
        if (nv->soc_irq_info[irq_count].irq_num == irq)
        {
            nv->soc_irq_info[irq_count].bh_pending = NV_FALSE;
        }
        if (nv->soc_irq_info[irq_count].ref_count == 0)
        {
            nv->soc_irq_info[irq_count].ref_count++;
            enable_irq(nv->soc_irq_info[irq_count].irq_num);
        }
    }
    nv->current_soc_irq = -1;
    NV_SPIN_UNLOCK_IRQRESTORE(&nvl->soc_isr_lock, flags);

    return ret;
}

static irqreturn_t nvidia_soc_isr(int irq, void *arg)
{
    unsigned long flags;
    irqreturn_t ret;
    nv_linux_state_t *nvl = (void *) arg;
    nv_state_t *nv = NV_STATE_PTR(nvl);
    NvU32 irq_count;

    NV_SPIN_LOCK_IRQSAVE(&nvl->soc_isr_lock, flags);

    /*
     * > Only 1 interrupt at a time is allowed to be serviced.
     * > So when bh_pending is true, bottom half is scheduled/active
     *   and serving previous interrupt by disabling all interrupts
     *   at interrupt controller level, also here GPU lock is already
     *   taken so this interrupt will anyways be blocked until bottom
     *   half releases GPU lock, so return early for now.
     * > Once bottom half processed earlier interrupt, it will release
     *   GPU lock and re-enable all interrupts and set bh_pending to
     *   false. Upon re-enabling, this interrupt will be serviced
     *   again because all interrupts that we care are level triggered.
     */
    for (irq_count = 0; irq_count < nv->num_soc_irqs; irq_count++)
    {
        if (nv->soc_irq_info[irq_count].bh_pending == NV_TRUE)
        {
            NV_SPIN_UNLOCK_IRQRESTORE(&nvl->soc_isr_lock, flags);
            return IRQ_NONE;
        }
    }
    nv->current_soc_irq = irq;
    for (irq_count = 0; irq_count < nv->num_soc_irqs; irq_count++)
    {
        if (nv->soc_irq_info[irq_count].ref_count == 1)
        {
            nv->soc_irq_info[irq_count].ref_count--;
            disable_irq_nosync(nv->soc_irq_info[irq_count].irq_num);
        }
    }

    ret = nvidia_isr(irq, arg);
    if (ret == IRQ_WAKE_THREAD)
    {
        for (irq_count = 0; irq_count < nv->num_soc_irqs; irq_count++)
        {
            if (nv->soc_irq_info[irq_count].irq_num == irq)
            {
                nv->soc_irq_info[irq_count].bh_pending = NV_TRUE;
            }
        }
    }
    else
    {
        for (irq_count = 0; irq_count < nv->num_soc_irqs; irq_count++)
        {
            if (nv->soc_irq_info[irq_count].ref_count == 0)
            {
                nv->soc_irq_info[irq_count].ref_count++;
                enable_irq(nv->soc_irq_info[irq_count].irq_num);
            }
        }
        nv->current_soc_irq = -1;
    }

    NV_SPIN_UNLOCK_IRQRESTORE(&nvl->soc_isr_lock, flags);

    return ret;
}

NvS32 nv_request_soc_irq(
    nv_linux_state_t *nvl,
    NvU32 irq,
    nv_soc_irq_type_t type,
    NvU32 flags,
    NvU32 priv_data,
    const char *device_name)
{
    nv_state_t *nv = NV_STATE_PTR(nvl);
    NvS32 ret;
    NvU32 irq_index;

    if (nv->num_soc_irqs >= NV_MAX_SOC_IRQS)
    {
        nv_printf(NV_DBG_ERRORS, "Exceeds Maximum SOC interrupts\n");
        return -EINVAL;
    }

    ret = request_threaded_irq(irq, nvidia_soc_isr, nvidia_soc_isr_kthread_bh,
                               flags, device_name, (void *)nvl);
    if (ret != 0)
    {
        nv_printf(NV_DBG_ERRORS, "nv_request_soc_irq for irq %d failed\n", irq);
        return ret;
    }

    disable_irq_nosync(irq);

    irq_index = nv->num_soc_irqs;
    nv->soc_irq_info[irq_index].irq_num = irq;
    nv->soc_irq_info[irq_index].irq_type = type;
    if (type == NV_SOC_IRQ_GPIO_TYPE)
    {
        nv->soc_irq_info[irq_index].irq_data.gpio_num = priv_data;
    }
    else if (type == NV_SOC_IRQ_DPAUX_TYPE)
    {
        nv->soc_irq_info[irq_index].irq_data.dpaux_instance = priv_data;
    }
    nv->num_soc_irqs++;
    nv->soc_irq_info[irq_index].ref_count = 0;

    return ret;
}

nv_soc_irq_type_t NV_API_CALL nv_get_current_irq_type(nv_state_t *nv)
{
    int count;

    for (count = 0; count < nv->num_soc_irqs; count++)
    {
        if (nv->soc_irq_info[count].irq_num == nv->current_soc_irq)
        {
            return nv->soc_irq_info[count].irq_type;
        }
    }

    return NV_SOC_IRQ_INVALID_TYPE;
}

NV_STATUS NV_API_CALL nv_get_current_irq_priv_data(nv_state_t *nv, NvU32 *priv_data)
{
    int count;

    if (nv->current_soc_irq == -1)
    {
        nv_printf(NV_DBG_ERRORS, "%s:No SOC interrupt in progress\n", __func__);
        return NV_ERR_GENERIC;
    }

    for (count = 0; count < nv->num_soc_irqs; count++)
    {
        if (nv->soc_irq_info[count].irq_num == nv->current_soc_irq)
        {
            if (nv->soc_irq_info[count].irq_type == NV_SOC_IRQ_GPIO_TYPE)
            {
                *priv_data = nv->soc_irq_info[count].irq_data.gpio_num;
            }
            else if (nv->soc_irq_info[count].irq_type == NV_SOC_IRQ_DPAUX_TYPE)
            {
                *priv_data = nv->soc_irq_info[count].irq_data.dpaux_instance;
            }
        }
    }

    return NV_OK;
}

static void nv_soc_free_irq_by_type(nv_state_t *nv, nv_soc_irq_type_t type)
{
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);
    int count;

    if ((nv->num_soc_irqs == 0) || (type == 0))
    {
        return;
    }

    for (count = 0; count < NV_MAX_SOC_IRQS; count++)
    {
        if (type == nv->soc_irq_info[count].irq_type)
        {
            free_irq(nv->soc_irq_info[count].irq_num, (void *)nvl);
            nv->soc_irq_info[count].irq_type = 0;
            nv->soc_irq_info[count].irq_num = 0;
            nv->soc_irq_info[count].bh_pending = NV_FALSE;
            nv->soc_irq_info[count].ref_count = 0;
            nv->num_soc_irqs--;
        }
    }
}

int nv_soc_register_irqs(nv_state_t *nv)
{
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);
    int rc;
    int dpauxindex;

    /* Skip registering interrupts for OpenRM */
    if (nv->request_firmware)
        return 0;

    nv->current_soc_irq = -1;

    rc = nv_request_soc_irq(nvl, nv->interrupt_line,
                            NV_SOC_IRQ_DISPLAY_TYPE,
                            nv_default_irq_flags(nv), 0,
                            "nvdisplay");
    if (rc != 0)
    {
        nv_printf(NV_DBG_ERRORS, "failed to request display irq (%d)\n", rc);
        return rc;
    }

    rc = nv_request_soc_irq(nvl, nv->hdacodec_irq, NV_SOC_IRQ_HDACODEC_TYPE,
                            nv_default_irq_flags(nv), 0, "hdacodec");
    if (rc != 0)
    {
        nv_printf(NV_DBG_ERRORS, "failed to request hdacodec irq (%d)\n", rc);
        free_irq(nv->interrupt_line, (void *) nvl);
        return rc;
    }

    if (nv->soc_is_dpalt_mode_supported)
    {
        /* Type-C port controller to display interrupt*/
        rc = nv_request_soc_irq(nvl, nv->tcpc2disp_irq, NV_SOC_IRQ_TCPC2DISP_TYPE,
                nv_default_irq_flags(nv), 0, "tcpc2disp");
        if (rc != 0)
        {
            nv_printf(NV_DBG_ERRORS, "failed to request Tcpc2disp irq (%d)\n", rc);
            free_irq(nv->interrupt_line, (void *) nvl);
            free_irq(nv->hdacodec_irq, (void *)nvl);
            return rc;
        }
    }

    for (dpauxindex = 0; dpauxindex < nv->num_dpaux_instance; dpauxindex++)
    {
        char dpaux_dev_name[10] = {0};
        snprintf(dpaux_dev_name, sizeof(dpaux_dev_name), "%s%d", "dpaux", dpauxindex);

        rc = nv_request_soc_irq(nvl, nv->dpaux_irqs[dpauxindex],
                                NV_SOC_IRQ_DPAUX_TYPE,
                                nv_default_irq_flags(nv),
                                dpauxindex,
                                dpaux_dev_name);
        if (rc != 0)
        {
            nv_printf(NV_DBG_ERRORS, "failed to request dpaux irq (%d)\n", rc);
            free_irq(nv->interrupt_line, (void *)nvl);
            free_irq(nv->hdacodec_irq, (void *)nvl);
            if (nv->soc_is_dpalt_mode_supported)
            {
                free_irq(nv->tcpc2disp_irq, (void *)nvl);
            }
            return rc;
        }
    }

    return 0;
}

void nv_soc_free_irqs(nv_state_t *nv)
{
    nv_soc_free_irq_by_type(nv, NV_SOC_IRQ_DISPLAY_TYPE);
    nv_soc_free_irq_by_type(nv, NV_SOC_IRQ_HDACODEC_TYPE);
    nv_soc_free_irq_by_type(nv, NV_SOC_IRQ_DPAUX_TYPE);
    nv_soc_free_irq_by_type(nv, NV_SOC_IRQ_GPIO_TYPE);

    if (nv->soc_is_dpalt_mode_supported)
    {
        nv_soc_free_irq_by_type(nv, NV_SOC_IRQ_TCPC2DISP_TYPE);
    }

}

static void nv_platform_free_device_dpaux(nv_state_t *nv)
{
    int dpauxindex;

    for (dpauxindex = 0; dpauxindex < nv->num_dpaux_instance; dpauxindex++)
    {
        /* Note that the memory region is being released only for dpaux0. This is
         * because the memory mapping is done only for dpaux0. Refer to the memory
         * mapping section in nv_platform_alloc_device_dpaux() for details on
         * why this is done, and how the same mapping is reused for other dpaux
         * instances.
         */
        if ((dpauxindex == 0) &&
            (nv->dpaux[dpauxindex] != NULL) &&
            (nv->dpaux[dpauxindex]->size != 0) &&
            (nv->dpaux[dpauxindex]->cpu_address != 0))
        {
            release_mem_region(nv->dpaux[dpauxindex]->cpu_address,
                               nv->dpaux[dpauxindex]->size);
        }

        if (nv->dpaux[dpauxindex] != NULL)
        {
            NV_KFREE(nv->dpaux[dpauxindex], sizeof(*(nv->dpaux[dpauxindex])));
        }
    }
}

static int nv_platform_alloc_device_dpaux(struct platform_device *plat_dev, nv_state_t *nv)
{
#if NV_SUPPORTS_PLATFORM_DEVICE
    static const size_t MAX_LENGTH = 10;
    const char *sdpaux = "dpaux";
    int dpauxindex = 0;
    int irq = 0;
    int rc = 0;
    int num_dpaux_instance = 0;
    const struct resource *res;
    phys_addr_t res_addr = 0;
    resource_size_t res_size = 0;
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);

    nv->num_dpaux_instance = 0;
    if (!of_property_read_u32(nvl->dev->of_node, "nvidia,num-dpaux-instance", &num_dpaux_instance))
    {
        nv->num_dpaux_instance = (unsigned) num_dpaux_instance;
        nv_printf(NV_DBG_INFO, "NVRM: Found %d dpAux instances in device tree.\n",
                  num_dpaux_instance);
    }

    if (nv->num_dpaux_instance > NV_MAX_DPAUX_NUM_DEVICES)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: Number of dpAux instances [%d] in device tree are more than"
                  "that of allowed [%d]. Initilizing %d dpAux instances.\n", nv->num_dpaux_instance,
                  NV_MAX_DPAUX_NUM_DEVICES, NV_MAX_DPAUX_NUM_DEVICES);
        nv->num_dpaux_instance = NV_MAX_DPAUX_NUM_DEVICES;
    }

    /* Memory region is being mapped only for dpaux0 because the size specified
     * in device tree for dpaux0 register space is large enough to accomodate
     * the register spaces of all dpaux instances. The iomapped memory of
     * dpaux0 is reused for all other dpaux instances.
     *
     * This is also required for RM because RM indexes the dpaux register space
     * for all dpaux instances using dpaux0 base address.
     */
    res = platform_get_resource_byname(plat_dev, IORESOURCE_MEM, "dpaux0");
    if (!res)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: failed to get IO memory resource\n");
        rc = -ENXIO;
        goto err_free_dpaux_dev;
    }
    res_addr = res->start;
    res_size = res->end - res->start;

    if (!request_mem_region(res_addr, res_size, nv_device_name))
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: request_mem_region failed for %pa\n",
                &res_addr);
        rc = -ENXIO;
        goto err_free_dpaux_dev;
    }

    for (dpauxindex = 0; dpauxindex < nv->num_dpaux_instance; dpauxindex++)
    {
        char sdpaux_device[MAX_LENGTH];
        snprintf(sdpaux_device, sizeof(sdpaux_device), "%s%d", sdpaux, dpauxindex);

        NV_KMALLOC(nv->dpaux[dpauxindex], sizeof(*(nv->dpaux[dpauxindex])));
        if (nv->dpaux[dpauxindex] == NULL)
        {
            nv_printf(NV_DBG_ERRORS, "NVRM: failed to allocate nv->dpaux[%d] memory\n", dpauxindex);
            rc = -ENOMEM;
            goto err_free_dpaux_dev;
        }

        os_mem_set(nv->dpaux[dpauxindex], 0, sizeof(*(nv->dpaux[dpauxindex])));

        irq = platform_get_irq_byname(plat_dev, sdpaux_device);
        if (irq < 0)
        {
            nv_printf(NV_DBG_ERRORS, "NVRM: failed to get IO irq resource\n");
            rc = irq;
            goto err_free_dpaux_dev;
        }

        nv->dpaux[dpauxindex]->cpu_address = res_addr;
        nv->dpaux[dpauxindex]->size = res_size;
        nv->dpaux_irqs[dpauxindex] = irq;
    }

    return rc;

err_free_dpaux_dev:
    nv_platform_free_device_dpaux(nv);

    return rc;
#else
    return -1;
#endif
}

NV_STATUS NV_API_CALL nv_soc_device_reset(nv_state_t *nv)
{
#if NV_SUPPORTS_PLATFORM_DISPLAY_DEVICE
    NV_STATUS status = NV_OK;
    int rc = 0;
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);

    /*
     * Skip all reset functions if the 'nvidia,skip-clk-rsts' DT property
     * is present. This property is currently present in the System FPGA device
     * tree because the BPMP firmware isn't available on FPGA yet.
     */
    bool skip_clk_rsts = of_property_read_bool(nvl->dev->of_node, "nvidia,skip-clk-rsts");
    if (!skip_clk_rsts)
    {
        if (nvl->nvdisplay_reset != NULL)
        {
            rc = reset_control_reset(nvl->nvdisplay_reset);
            if (rc != 0)
            {
                status = NV_ERR_GENERIC;
                nv_printf(NV_DBG_ERRORS, "NVRM: reset_control_reset failed, rc: %d\n", rc);
                goto out;
            }
        }

        if (nvl->dpaux0_reset != NULL)
        {
            rc = reset_control_reset(nvl->dpaux0_reset);
            if (rc != 0)
            {
                status = NV_ERR_GENERIC;
                nv_printf(NV_DBG_ERRORS, "NVRM: reset_control_reset failed, rc: %d\n", rc);
                goto out;
            }
        }

        if (nvl->dsi_core_reset != NULL)
        {
            rc = reset_control_reset(nvl->dsi_core_reset);
            if (rc != 0)
            {
                status = NV_ERR_GENERIC;
                nv_printf(NV_DBG_ERRORS, "NVRM: reset_control_reset failed, rc: %d\n", rc);
                goto out;
            }
        }

    }
out:
    return status;
#else
    return NV_OK;
#endif
}

NV_STATUS NV_API_CALL nv_soc_mipi_cal_reset(nv_state_t *nv)
{
    NV_STATUS status = NV_OK;
#if NV_SUPPORTS_PLATFORM_DISPLAY_DEVICE
    int rc = 0;
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);
    bool skip_clk_rsts = of_property_read_bool(nvl->dev->of_node, "nvidia,skip-clk-rsts");

    if (skip_clk_rsts)
        return NV_OK;

    if (nvl->mipi_cal_reset != NULL)
    {
        rc = reset_control_reset(nvl->mipi_cal_reset);
        if (rc != 0)
        {
            status = NV_ERR_GENERIC;
            nv_printf(NV_DBG_ERRORS, "NVRM: mipi_cal reset_control_reset failed, rc: %d\n", rc);
        }
    }
    else
    {
        status = NV_ERR_GENERIC;
    }
#endif
    return status;
}

// This function gets called only for Tegra
static NV_STATUS nv_platform_get_iommu_availability(struct platform_device *plat_dev,
                                              nv_state_t *nv)
{
    struct device_node *np = plat_dev->dev.of_node;
    struct device_node *niso_np_with_iommus = NULL;
    struct device_node *niso_np = NULL;
    struct device_node *iso_np = NULL;
    NvU32 value = 0;
    NV_STATUS status = NV_OK;
    bool single_smmu = NV_TRUE;

    nv->iommus.iso_iommu_present = NV_FALSE;
    nv->iommus.niso_iommu_present = NV_FALSE;

    /* NV_U32_MAX is used to indicate that the platform does not support SMMU */
    nv->iommus.dispIsoStreamId = NV_U32_MAX;
    nv->iommus.dispNisoStreamId = NV_U32_MAX;

    iso_np = of_parse_phandle(np, "iommus", 0);
    if (iso_np && of_device_is_available(iso_np)) {

        /* Parse ISO StreamID. Second entry in iommu property has Stream ID */
        if (of_property_read_u32_index(np, "iommus", 1, &value))
        {
            nv_printf(NV_DBG_ERRORS, "NVRM: nv_platform_get_iommu_availability, failed to parse ISO StreamID\n");
            status = NV_ERR_GENERIC;
            goto free_iso_np;
        }
        /* LSB 8 bits represent the Stream ID */
        nv->iommus.dispIsoStreamId = (value & 0xFF);
    }

    single_smmu = of_property_read_bool(np, "single_stage_iso_smmu");
    nv->iommus.iso_iommu_present = !single_smmu;

    niso_np = of_get_child_by_name(np, "nvdisplay-niso");
    if (niso_np) {
        niso_np_with_iommus = of_parse_phandle(niso_np, "iommus", 0);
        if (niso_np_with_iommus && of_device_is_available(niso_np_with_iommus)) {
            nv->iommus.niso_iommu_present = NV_TRUE;

            /* Parse NISO StreamID. Second entry in iommu property has Stream ID */
            if (of_property_read_u32_index(niso_np, "iommus", 1, &value))
            {
                nv_printf(NV_DBG_ERRORS, "NVRM: nv_platform_get_iommu_availability, failed to parse NISO StreamID\n");
                status = NV_ERR_GENERIC;
                goto free_niso_np;
            }
            /* LSB 8 bits represent the Stream ID */
            nv->iommus.dispNisoStreamId = (value & 0xFF);
        }
    }

free_niso_np:
    if (niso_np_with_iommus)
        of_node_put(niso_np_with_iommus);

    if (niso_np)
        of_node_put(niso_np);

free_iso_np:
    if (iso_np)
        of_node_put(iso_np);

    return status;
}

static int nv_platform_register_mapping_devs(struct platform_device *plat_dev,
                                             nv_state_t *nv)
{
    struct device_node *np = plat_dev->dev.of_node;
    struct device_node *niso_np = NULL;
    struct platform_device *niso_plat_dev = NULL;
    int rc = 0;
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);

    nv->niso_dma_dev = NULL;

    niso_np = of_get_child_by_name(np, "nvdisplay-niso");
    if (niso_np == NULL)
    {
        nv_printf(NV_DBG_INFO, "NVRM: no nvdisplay-niso child node\n");
        goto register_mapping_devs_end;
    }

#if defined(NV_DEVM_OF_PLATFORM_POPULATE_PRESENT) && NV_SUPPORTS_PLATFORM_DEVICE
    rc = devm_of_platform_populate(&plat_dev->dev);
#else
    nv_printf(NV_DBG_ERRORS, "NVRM: devm_of_platform_populate not present\n");
    rc = -ENOSYS;
#endif
    if (rc != 0)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: devm_of_platform_populate failed\n");
        goto register_mapping_devs_end;
    }

    niso_plat_dev = of_find_device_by_node(niso_np);
    if (niso_plat_dev == NULL)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: no nvdisplay-niso platform devices\n");
        rc = -ENODEV;
        goto register_mapping_devs_end;
    }

#if defined(NV_OF_DMA_CONFIGURE_PRESENT)
#if defined(NV_OF_DMA_CONFIGURE_HAS_INT_RETURN_TYPE)
    rc = of_dma_configure(
#else
    rc = 0;
    of_dma_configure(
#endif
        &niso_plat_dev->dev,
        niso_np
#if NV_OF_DMA_CONFIGURE_ARGUMENT_COUNT > 2
        , true
#endif
    );
#else
    nv_printf(NV_DBG_ERRORS, "NVRM: of_dma_configure not present\n");
    rc = -ENOSYS;
#endif
    if (rc != 0)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: nv_of_dma_configure failed for niso\n");
        goto register_mapping_devs_end;
    }

    nvl->niso_dma_dev.dev = &niso_plat_dev->dev;
    nvl->niso_dma_dev.addressable_range.start = 0;
    nvl->niso_dma_dev.addressable_range.limit = NV_U64_MAX;
    nv->niso_dma_dev = &nvl->niso_dma_dev;

register_mapping_devs_end:
    of_node_put(niso_np);
    return rc;
}

static int nv_platform_parse_dcb(struct platform_device *plat_dev,
        nv_state_t *nv)
{
    int ret;

#if defined(NV_OF_PROPERTY_COUNT_ELEMS_OF_SIZE_PRESENT)
    struct device_node *np = plat_dev->dev.of_node;
    ret = of_property_count_elems_of_size(np, "nvidia,dcb-image", sizeof(u8));
#else
    nv_printf(NV_DBG_ERRORS, "of_property_count_elems_of_size not present\n");
    return -ENOSYS;
#endif
    if (ret > 0)
    {
        nv->soc_dcb_size = ret;
        /* Allocate dcb array */
        NV_KMALLOC(nv->soc_dcb_blob, nv->soc_dcb_size);
        if (nv->soc_dcb_blob == NULL)
        {
            nv_printf(NV_DBG_ERRORS, "failed to allocate dcb array");
            return -ENOMEM;
        }
    }

#if defined(NV_OF_PROPERTY_READ_VARIABLE_U8_ARRAY_PRESENT)
    ret = of_property_read_variable_u8_array(np, "nvidia,dcb-image",
            nv->soc_dcb_blob, 0, nv->soc_dcb_size);
#else
    nv_printf(NV_DBG_ERRORS, "of_property_read_variable_u8_array not present\n");
    ret = -ENOSYS;
#endif
    if (IS_ERR(&ret))
    {
        nv_printf(NV_DBG_ERRORS, "failed to read dcb blob");
        NV_KFREE(nv->soc_dcb_blob, nv->soc_dcb_size);
        nv->soc_dcb_blob = NULL;
        nv->soc_dcb_size = 0;
        return ret;
    }

    return 0;
}

#if NV_SUPPORTS_PLATFORM_DISPLAY_DEVICE
static int nv_platform_device_display_probe(struct platform_device *plat_dev)
{
    nv_state_t *nv = NULL;
    nv_linux_state_t *nvl = NULL;
    nvidia_stack_t *sp = NULL;
    phys_addr_t res_addr = 0;
    resource_size_t res_size = 0;
    int irq = 0;
    int rc = 0;
    const struct resource *res;
    bool skip_clk_rsts;
    NV_STATUS   status;

    rc = nv_kmem_cache_alloc_stack(&sp);
    if (rc < 0)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: %s: failed to allocate stack!\n",
                  __FUNCTION__);
        return rc;
    }

    NV_KMALLOC(nvl, sizeof(*nvl));
    if (nvl == NULL)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: failed to allocate nvl memory\n");
        rc = -ENOMEM;
        goto err_free_stack;
    }
    os_mem_set(nvl, 0, sizeof(*nvl));

    nv = NV_STATE_PTR(nvl);

    platform_set_drvdata(plat_dev, (void *)nvl);

    nvl->dev = &plat_dev->dev;

    /*
     * fill SOC dma device information
     */
    nvl->dma_dev.dev = nvl->dev;
    nvl->dma_dev.addressable_range.start = 0;
    nvl->dma_dev.addressable_range.limit = NV_U64_MAX;
    nv->dma_dev = &nvl->dma_dev;

    nvl->tce_bypass_enabled = NV_TRUE;

    NV_KMALLOC(nv->regs, sizeof(*(nv->regs)));
    if (nv->regs == NULL)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: failed to allocate nv->regs memory\n");
        rc = -ENOMEM;
        goto err_free_nvl;
    }
    os_mem_set(nv->regs, 0, sizeof(*(nv->regs)));

    res = platform_get_resource_byname(plat_dev, IORESOURCE_MEM, "nvdisplay");
    if (!res)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: failed to get IO memory resource\n");
        rc = -ENODEV;
        goto err_free_nv_regs;
    }
    res_addr = res->start;
    res_size = res->end - res->start;

    irq = platform_get_irq_byname(plat_dev, "nvdisplay");
    if (irq < 0)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: failed to get IO irq resource\n");
        rc = -ENODEV;
        goto err_free_nv_regs;
    }

    if (!request_mem_region(res_addr, res_size, nv_device_name))
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: request_mem_region failed for %pa\n",
                  &res_addr);
        rc = -ENOMEM;
        goto err_free_nv_regs;
    }

    nv->regs->cpu_address = res_addr;
    nv->regs->size = res_size;
    nv->interrupt_line = irq;
    nv->flags = NV_FLAG_SOC_DISPLAY;

    nv->os_state = (void *) nvl;

    // Check ISO/NISO SMMU status and parse StreamIDs
    status = nv_platform_get_iommu_availability(plat_dev, nv);
    if (status != NV_OK)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: nv_platform_device_display_probe: parsing iommu node failed\n");
        goto err_release_mem_region_regs;
    }

    rc = nv_platform_register_mapping_devs(plat_dev, nv);
    if (rc != 0)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: failed to allocate niso platform device\n");
        goto err_release_mem_region_regs;
    }

    rc = nv_platform_alloc_device_dpaux(plat_dev, nv);
    if (rc < 0)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: failed to alloc DpAux device\n");
        goto err_release_mem_region_regs;
    }

    NV_KMALLOC(nv->hdacodec_regs, sizeof(*(nv->hdacodec_regs)));
    if (nv->hdacodec_regs == NULL)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: failed to allocate hdacodecregs memory\n");
        rc = -ENOMEM;
        goto err_release_mem_region_regs;
    }
    os_mem_set(nv->hdacodec_regs, 0, sizeof(*(nv->hdacodec_regs)));

    res = platform_get_resource_byname(plat_dev, IORESOURCE_MEM, "hdacodec");
    if (!res)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: failed to get hdacodec IO memory resource\n");
        rc = -ENODEV;
        goto err_free_nv_codec_regs;
    }
    res_addr = res->start;
    res_size = res->end - res->start;

    if (!request_mem_region(res_addr, res_size, nv_device_name))
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: request_mem_region of hdacodec failed for %pa\n",
                  &res_addr);
        rc = -ENOMEM;
        goto err_free_nv_codec_regs;
    }

    nv->hdacodec_regs->cpu_address = res_addr;
    nv->hdacodec_regs->size = res_size;
    nv->soc_is_dpalt_mode_supported = false;

    nv->hdacodec_irq = platform_get_irq_byname(plat_dev, "hdacodec");
    if (nv->hdacodec_irq < 0)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: failed to get HDACODEC IO irq resource\n");
        rc = -ENODEV;
        goto err_release_mem_hdacodec_region_regs;
    }

    rc = of_property_read_bool(nvl->dev->of_node, "nvidia,dpalt-supported");
    if (rc == true)
    {
        irq = platform_get_irq_byname(plat_dev, "tcpc2disp");
        if (irq < 0)
        {
            nv->soc_is_dpalt_mode_supported = false;
        }
        else
        {
            nv->tcpc2disp_irq = irq;
            nv->soc_is_dpalt_mode_supported = true;
        }
    }

    NV_KMALLOC(nv->mipical_regs, sizeof(*(nv->mipical_regs)));
    if (nv->mipical_regs == NULL)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: failed to allocate mipical registers memory\n");
        rc = -ENOMEM;
        goto err_release_mem_hdacodec_region_regs;
    }
    os_mem_set(nv->mipical_regs, 0, sizeof(*(nv->mipical_regs)));

    res = platform_get_resource_byname(plat_dev, IORESOURCE_MEM, "mipical");
    if (!res)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: failed to get mipical IO memory resource\n");
        rc = -ENODEV;
        goto err_free_mipical_regs;
    }
    res_addr = res->start;
    res_size = res->end - res->start;

    if (!request_mem_region(res_addr, res_size, nv_device_name))
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: request_mem_region of mipical failed for %pa\n",
                  &res_addr);
        rc = -ENOMEM;
        goto err_free_mipical_regs;
    }

    nv->mipical_regs->cpu_address = res_addr;
    nv->mipical_regs->size = res_size;

    pm_vt_switch_required(&plat_dev->dev, NV_TRUE);

    // Enabling power management for the device.
    pm_runtime_enable(&plat_dev->dev);

    /*
     * Skip all clock/reset functions if the 'nvidia,skip-clk-rsts' DT property
     * is present. This property is currently present in the System FPGA device
     * tree because the BPMP firmware isn't available on FPGA yet.
     */
    skip_clk_rsts = of_property_read_bool(nvl->dev->of_node, "nvidia,skip-clk-rsts");
    if (!skip_clk_rsts)
    {
        /*
         * Getting all the display-clock handles
         * from BPMP FW at the time of probe.
         */
        status = nv_clk_get_handles(nv);
        if (status != NV_OK)
        {
            nv_printf(NV_DBG_ERRORS, "NVRM: failed to get clock handles\n");
            rc = -EPERM;
            goto err_release_mem_mipical_region_regs;
        }

        /*
         * Getting dpaux-reset handles
         * from device tree at the time of probe.
         */
        nvl->dpaux0_reset = devm_reset_control_get(nvl->dev, "dpaux0_reset");
        if (IS_ERR(nvl->dpaux0_reset))
        {
            nv_printf(NV_DBG_ERRORS, "NVRM: devm_reset_control_get failed, err: %ld\n", PTR_ERR(nvl->dpaux0_reset));
            nvl->dpaux0_reset = NULL;
        }

        /*
         * Getting display-reset handles
         * from device tree at the time of probe.
         */
        nvl->nvdisplay_reset = devm_reset_control_get(nvl->dev, "nvdisplay_reset");
        if (IS_ERR(nvl->nvdisplay_reset))
        {
            nv_printf(NV_DBG_ERRORS, "NVRM: devm_reset_control_get failed, err: %ld\n", PTR_ERR(nvl->nvdisplay_reset));
            nvl->nvdisplay_reset = NULL;
        }

        /*
         * Getting dsi-core reset handles
         * from device tree at the time of probe.
         */
        nvl->dsi_core_reset = devm_reset_control_get(nvl->dev, "dsi_core_reset");
        if (IS_ERR(nvl->dsi_core_reset))
        {
            nv_printf(NV_DBG_ERRORS, "NVRM: devm_reset_control_get failed, err: %ld\n",  PTR_ERR(nvl->dsi_core_reset));
            nvl->dsi_core_reset = NULL;
        }

        /*
         * Getting mipi_cal reset handle
         * from device tree at the time of probe.
         */
        nvl->mipi_cal_reset = devm_reset_control_get(nvl->dev, "mipi_cal_reset");
        if (IS_ERR(nvl->mipi_cal_reset))
        {
            nv_printf(NV_DBG_ERRORS, "NVRM: mipi_cal devm_reset_control_get failed, err: %ld\n",  PTR_ERR(nvl->mipi_cal_reset));
            nvl->mipi_cal_reset = NULL;
        }

    }

    status = nv_imp_icc_get(nv);
    if (status != NV_OK)
    {
        //
        // nv_imp_icc_get errors are normally treated as fatal, but ICC is
        // expected to be disabled on AV + L (causing NV_ERR_NOT_SUPPORTED to
        // be returned), so this is not treated as fatal.
        //
        if (status != NV_ERR_NOT_SUPPORTED)
        {
            nv_printf(NV_DBG_ERRORS, "NVRM: failed to get icc handle\n");
            rc = -EPERM;
            goto err_destroy_clk_handles;
        }
    }
    /*
     * Get the backlight device name
     */
    of_property_read_string(nvl->dev->of_node, "nvidia,backlight-name",
                            &nvl->backlight.device_name);

    /*
     * TODO bug 2100708: the fake domain is used to opt out of some RM paths
     *                   that cause issues otherwise, see the bug for details.
     */
    nv->pci_info.domain    = 2;
    nv->pci_info.bus       = 0;
    nv->pci_info.slot      = 0;

    num_probed_nv_devices++;

    if (!nv_lock_init_locks(sp, nv))
    {
        rc = -EPERM;
        goto err_put_icc_handle;
    }

    nvl->safe_to_mmap = NV_TRUE;
    INIT_LIST_HEAD(&nvl->open_files);
    NV_SPIN_LOCK_INIT(&nvl->soc_isr_lock);

    if (!rm_init_private_state(sp, nv))
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: rm_init_private_state() failed!\n");
        rc = -EPERM;
        goto err_destroy_lock;
    }

    if (nv_linux_init_open_q(nvl) != 0)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: nv_linux_init_open_q() failed!\n");
        goto err_free_private_state;
    }

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
        nv_printf(NV_DBG_ERRORS, "NVRM: failed to add device\n");
        rc = -ENODEV;
        goto err_stop_open_q;
    }

    UNLOCK_NV_LINUX_DEVICES();

    rm_set_rm_firmware_requested(sp, nv);

    /*
     * Parse DCB blob
     */
    rc = nv_platform_parse_dcb(plat_dev, nv);
    if (rc != 0)
    {
        goto err_remove_device;
    }

    /*
     * Parse display rm sw-soc-chip-id
     */
    rc = of_property_read_u32(nvl->dev->of_node, "nvidia,disp-sw-soc-chip-id",
                              &nv->disp_sw_soc_chip_id);
    if (rc != 0)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: Unable to read disp_sw_soc_chip_id\n");
        goto err_remove_device;
    }

    /*
     * TODO: procfs, vt_switch, dynamic_power_management
     */

    nv_kmem_cache_free_stack(sp);

    dma_set_mask(nv->dma_dev->dev, DMA_BIT_MASK(39));
#if defined(NV_DMA_SET_MASK_AND_COHERENT_PRESENT)
    if (nv->niso_dma_dev != NULL)
    {
        dma_set_mask_and_coherent(nv->niso_dma_dev->dev, DMA_BIT_MASK(39));
    }
#else
    nv_printf(NV_DBG_INFO, "NVRM: Using default 32-bit DMA mask\n");
#endif

    rc = os_alloc_mutex(&nvl->soc_bh_mutex);

    return rc;

err_remove_device:
    LOCK_NV_LINUX_DEVICES();
    nv_linux_remove_device_locked(nvl);
    UNLOCK_NV_LINUX_DEVICES();
err_stop_open_q:
    nv_linux_stop_open_q(nvl);
err_free_private_state:
    rm_free_private_state(sp, nv);
err_destroy_lock:
    nv_lock_destroy_locks(sp, nv);
err_put_icc_handle:
    nv_imp_icc_put(nv);
err_destroy_clk_handles:
    nv_clk_clear_handles(nv);
err_remove_dpaux_device:
    nv_platform_free_device_dpaux(nv);
err_release_mem_mipical_region_regs:
    release_mem_region(nv->mipical_regs->cpu_address, nv->mipical_regs->size);
err_free_mipical_regs:
    NV_KFREE(nv->mipical_regs, sizeof(*(nv->mipical_regs)));
err_release_mem_hdacodec_region_regs:
    release_mem_region(nv->hdacodec_regs->cpu_address, nv->hdacodec_regs->size);
err_release_mem_region_regs:
    release_mem_region(nv->regs->cpu_address, nv->regs->size);
err_free_nv_codec_regs:
    NV_KFREE(nv->hdacodec_regs, sizeof(*(nv->hdacodec_regs)));
err_free_nv_regs:
    NV_KFREE(nv->regs, sizeof(*(nv->regs)));
err_free_nvl:
    NV_KFREE(nvl, sizeof(*nvl));
    platform_set_drvdata(plat_dev, NULL);
err_free_stack:
    nv_kmem_cache_free_stack(sp);

    return rc;
}
#endif

#if NV_SUPPORTS_PLATFORM_DISPLAY_DEVICE
static void nv_platform_device_display_remove(struct platform_device *plat_dev)
{
    nv_linux_state_t *nvl = NULL;
    nv_state_t *nv;
    nvidia_stack_t *sp = NULL;

    nv_printf(NV_DBG_SETUP, "NVRM: removing SOC Display device\n");

    nvl = platform_get_drvdata(plat_dev);
    if (WARN_ON(!nvl || (nvl->dev != &plat_dev->dev)))
    {
        return;
    }

    if (WARN_ON(nv_kmem_cache_alloc_stack(&sp) < 0))
    {
        return;
    }

    /*
     * Flush and stop open_q before proceeding with removal to ensure nvl
     * outlives all enqueued work items.
     */
    nv_linux_stop_open_q(nvl);

    LOCK_NV_LINUX_DEVICES();

    nv_linux_remove_device_locked(nvl);

    /*
     * TODO: procfs
     */

    down(&nvl->ldata_lock);
    UNLOCK_NV_LINUX_DEVICES();

    /*
     * TODO: vt_switch, dynamic_power_management
     */

    nv = NV_STATE_PTR(nvl);

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

    nv_lock_destroy_locks(sp, nv);

    num_probed_nv_devices--;

    rm_free_private_state(sp, nv);

    release_mem_region(nv->mipical_regs->cpu_address, nv->mipical_regs->size);

    NV_KFREE(nv->mipical_regs, sizeof(*(nv->mipical_regs)));

    release_mem_region(nv->hdacodec_regs->cpu_address, nv->hdacodec_regs->size);

    NV_KFREE(nv->hdacodec_regs, sizeof(*(nv->hdacodec_regs)));

    release_mem_region(nv->regs->cpu_address, nv->regs->size);

    NV_KFREE(nv->regs, sizeof(*(nv->regs)));

    nv_imp_icc_put(nv);

    nv_platform_free_device_dpaux(nv);

    /*
     * Clearing all the display-clock handles
     * at the time of device remove.
     */
    nv_clk_clear_handles(nv);

    // Disabling power management for the device.
    pm_runtime_disable(&plat_dev->dev);

    num_nv_devices--;

    NV_KFREE(nv->soc_dcb_blob, nv->soc_dcb_size);

    os_free_mutex(nvl->soc_bh_mutex);

    NV_KFREE(nvl, sizeof(*nvl));

    nv_kmem_cache_free_stack(sp);
}
#endif

static int nv_platform_device_probe(struct platform_device *plat_dev)
{
    int rc = 0;

    if (plat_dev->dev.of_node)
    {
        {
#if NV_SUPPORTS_PLATFORM_DISPLAY_DEVICE
            rc = nv_platform_device_display_probe(plat_dev);
#endif
        }
    }
    else
    {
#if NV_SUPPORTS_PLATFORM_DISPLAY_DEVICE
        rc = nv_platform_device_display_probe(plat_dev);
#endif
    }

    return rc;
}

static void nv_platform_device_remove(struct platform_device *plat_dev)
{
    if (plat_dev->dev.of_node)
    {
        {
#if NV_SUPPORTS_PLATFORM_DISPLAY_DEVICE
            nv_platform_device_display_remove(plat_dev);
#endif
        }
    }
    else
    {
#if NV_SUPPORTS_PLATFORM_DISPLAY_DEVICE
        nv_platform_device_display_remove(plat_dev);
#endif
    }
}

#if defined(NV_PLATFORM_DRIVER_STRUCT_REMOVE_RETURNS_VOID) /* Linux v6.11 */
static void nv_platform_device_remove_wrapper(struct platform_device *pdev)
{
    nv_platform_device_remove(pdev);
}
#else
static int nv_platform_device_remove_wrapper(struct platform_device *pdev)
{
    nv_platform_device_remove(pdev);

    return 0;
}
#endif

const struct of_device_id nv_platform_device_table[] =
{
    { .compatible = "nvidia,tegra234-display",},
    {},
};
MODULE_DEVICE_TABLE(of, nv_platform_device_table);

#if defined(CONFIG_PM)
extern struct dev_pm_ops nv_pm_ops;
#endif

struct platform_driver nv_platform_driver = {
    .driver = {
        .name  = "nv_platform",
        .of_match_table = nv_platform_device_table,
        .owner = THIS_MODULE,
#if defined(CONFIG_PM)
        .pm = &nv_pm_ops,
#endif
    },
    .probe     = nv_platform_device_probe,
    .remove    = nv_platform_device_remove_wrapper,
};

int nv_platform_count_devices(void)
{
    int count = 0;
    struct device_node *np = NULL;

    while ((np = of_find_matching_node(np, nv_platform_device_table)))
    {
        count++;
    }

    return count;
}

int nv_platform_register_driver(void)
{
#if NV_SUPPORTS_PLATFORM_DEVICE
    return platform_driver_register(&nv_platform_driver);
#else
    nv_printf(NV_DBG_ERRORS, "NVRM: Not registering platform driver\n");
    return -1;
#endif
}

void nv_platform_unregister_driver(void)
{
#if NV_SUPPORTS_PLATFORM_DEVICE
    platform_driver_unregister(&nv_platform_driver);
#endif
}

extern int tegra_fuse_control_read(unsigned long addr, unsigned int *data);

unsigned int NV_API_CALL nv_soc_fuse_register_read (unsigned int addr)
{
   unsigned int data = 0;

#if NV_IS_EXPORT_SYMBOL_PRESENT_tegra_fuse_control_read
   tegra_fuse_control_read ((unsigned long)(addr), &data);
#endif

   return data;
}

NV_STATUS nv_get_valid_window_head_mask(nv_state_t *nv, NvU64 *window_head_mask)
{
#if NV_SUPPORTS_PLATFORM_DEVICE
    int ret = 0;
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);

    if ((nvl == NULL) || (window_head_mask == NULL))
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: Wrong input arguments \n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    ret = of_property_read_u64(nvl->dev->of_node, "nvidia,window-head-mask", window_head_mask);
    if (ret == -EINVAL)
    {
        // Property does not exist.
        return NV_ERR_NOT_SUPPORTED;
    }
    else if (ret)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: failed to read device node window-head-mask ret=%d\n", ret);
        return NV_ERR_GENERIC;
    }
    return NV_OK;
#else
    return NV_ERR_NOT_SUPPORTED;
#endif
}

NV_STATUS NV_API_CALL nv_soc_i2c_hsp_semaphore_acquire(NvU32 ownerId, NvBool bAcquire, NvU64 timeout)
{
    // TODO: This needs to be updated once HSP side implementation is done.
    return NV_OK;
}

NvBool nv_get_hdcp_enabled(nv_state_t *nv)
{
    bool rc = false;
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);

    rc = of_property_read_bool(nvl->dev->of_node, "hdcp_enabled");
    if (rc == true)
    {
        return NV_TRUE;
    }

    return NV_FALSE;
}
