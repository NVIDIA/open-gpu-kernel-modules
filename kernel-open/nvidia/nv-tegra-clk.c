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

#define  __NO_VERSION__

#include "nv-pci-types.h"
#include "nv-pci.h"
#include "nv-tegra-clk.h"

#include <linux/clk.h>
#include <linux/device.h>
#include <linux/jiffies.h>
#include <linux/kernel.h>
#include <linux/pm_runtime.h>
#include <linux/workqueue.h>

#if defined(NV_PM_QOS_READ_VALUE_SUPPORTED)
#include <linux/pm_qos.h>
#endif

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

#if defined(CONFIG_PM_DEVFREQ)

struct nv_pci_tegra_devfreq_data {
    const char *clk_name;
    const char *icc_name;
    const unsigned int gpc_fuse_field;
    const TEGRASOC_DEVFREQ_CLK devfreq_clk;
};

static const struct nv_pci_tegra_devfreq_data tegra_devfreq_tables[][6] = {
    {
        {
            .clk_name = "gpc0clk",
            .icc_name = "gpu-write",
            .gpc_fuse_field = BIT(0),
            .devfreq_clk = TEGRASOC_DEVFREQ_CLK_GPC,
        },
        {
            .clk_name = "gpc1clk",
            .icc_name = "gpu-write",
            .gpc_fuse_field = BIT(1),
            .devfreq_clk = TEGRASOC_DEVFREQ_CLK_GPC,
        },
        {
            .clk_name = "gpc2clk",
            .icc_name = "gpu-write",
            .gpc_fuse_field = BIT(2),
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
    },
    {
        {
            .clk_name = "gpc0clk",
            .icc_name = "gpu-write",
            .gpc_fuse_field = BIT(0),
            .devfreq_clk = TEGRASOC_DEVFREQ_CLK_GPC,
        },
        {
            .clk_name = "gpc1clk",
            .icc_name = "gpu-write",
            .gpc_fuse_field = BIT(1),
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
        {
            .clk_name = NULL
        },
    },
};

#define to_tegra_devfreq_dev(x) \
    container_of(x, struct nv_pci_tegra_devfreq_dev, dev)

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
    //
    // Tracks whether this devfreq object successfully entered the devfreq core
    // suspend path, so devfreq_resume_device() stays paired with a real
    // devfreq_suspend_device() call.
    //
    bool is_devfreq_suspended;
    unsigned long devfreq_locked_freq;
    TEGRASOC_DEVFREQ_CLK_BOOST_TYPE devfreq_boost_type;
    struct delayed_work boost_disable;
#if NV_HAS_ICC_SUPPORTED
    struct icc_path *icc_path;
#endif
#if NV_HAS_COOLING_SUPPORTED
    struct list_head therm_zones;
#endif
};

static bool
nv_pci_tegra_has_devfreq_governor_private_data(struct devfreq *df)
{
#if defined(NV_DEVFREQ_HAS_GOVERNOR_DATA)
    return READ_ONCE(df->governor_data) != NULL;
#else
    return READ_ONCE(df->data) != NULL;
#endif
}

static int
nv_pci_tegra_try_suspend_devfreq(struct nv_pci_tegra_devfreq_dev *tdev)
{
    struct devfreq *df = READ_ONCE(tdev->devfreq);
    int err;

    WRITE_ONCE(tdev->is_devfreq_suspended, false);

    if (df == NULL)
    {
        return 0;
    }

    //
    // Some governors dereference their private state from the suspend event.
    // Avoid entering a governor suspend callback when that state was not
    // created or has already been torn down.
    //
    if (!nv_pci_tegra_has_devfreq_governor_private_data(df))
    {
        return 0;
    }

#if defined(NV_DEVFREQ_HAS_SUSPEND_FREQ)
    mutex_lock(&df->lock);
    df->suspend_freq = df->previous_freq;
    mutex_unlock(&df->lock);
#endif

    err = devfreq_suspend_device(df);
    if (err == 0)
    {
        WRITE_ONCE(tdev->is_devfreq_suspended, true);
    }

    return err;
}

static int
nv_pci_tegra_try_resume_devfreq(struct nv_pci_tegra_devfreq_dev *tdev)
{
    struct devfreq *df = READ_ONCE(tdev->devfreq);
    int err;

    if ((df == NULL) || !READ_ONCE(tdev->is_devfreq_suspended))
    {
        return 0;
    }

    err = devfreq_resume_device(df);
    if (err == 0)
    {
        WRITE_ONCE(tdev->is_devfreq_suspended, false);
    }

    return err;
}

static void
nv_pci_tegra_update_devfreq(struct devfreq *df)
{
#if defined(NV_UPDATE_DEVFREQ_PRESENT)
    mutex_lock(&df->lock);
    update_devfreq(df);
    mutex_unlock(&df->lock);
#else
    (void)df;
#endif
}

static void
nv_pci_tegra_unwind_devfreq_suspend(struct nv_pci_tegra_devfreq_dev *tdev)
{
    struct devfreq *df;
    int err;

    if (tdev == NULL)
    {
        return;
    }

    df = READ_ONCE(tdev->devfreq);
    if (df == NULL)
    {
        return;
    }

    err = nv_pci_tegra_try_resume_devfreq(tdev);
    if (err)
    {
        nv_printf(NV_DBG_WARNINGS,
                  "NVRM: failed to unwind devfreq suspend: %d\n", err);
        return;
    }

    //
    // Runtime suspend is being aborted. Refresh the active devfreq request so
    // frequency and ICC state are not left at suspend values.
    //
    nv_pci_tegra_update_devfreq(df);
}

static void nv_pci_gb10b_device_release(struct device *dev)
{
    ;
}

#if defined(NV_PM_QOS_READ_VALUE_SUPPORTED)
static s32 nv_pci_tegra_devfreq_pm_qos_read_value(struct devfreq *devfreq, enum dev_pm_qos_req_type type)
{
    struct device *dev = devfreq->dev.parent;
    struct dev_pm_qos *pm_qos = dev->power.qos;
    s32 ret = 0;
    unsigned long flags;

    spin_lock_irqsave(&dev->power.lock, flags);

    switch (type) {
        case DEV_PM_QOS_MIN_FREQUENCY:
                ret = IS_ERR_OR_NULL(pm_qos) ? PM_QOS_MIN_FREQUENCY_DEFAULT_VALUE
                        : READ_ONCE(pm_qos->freq.min_freq.target_value);
                break;
        case DEV_PM_QOS_MAX_FREQUENCY:
                ret = IS_ERR_OR_NULL(pm_qos) ? PM_QOS_MAX_FREQUENCY_DEFAULT_VALUE
                        : READ_ONCE(pm_qos->freq.max_freq.target_value);
                break;
        default:
                break;
    }

    spin_unlock_irqrestore(&dev->power.lock, flags);

    return ret;
}

static void nv_pci_tegra_devfreq_get_freq_range(struct devfreq *devfreq,
                                   unsigned long *min_freq,
                                   unsigned long *max_freq)
{
    s32 qos_min_freq, qos_max_freq;

#if defined(NV_DEVFREQ_HAS_FREQ_TABLE)
    if (devfreq->max_state == 0U) {
        *min_freq = ULONG_MAX;
        *max_freq = ULONG_MAX;
        return;
    }

    *min_freq = devfreq->freq_table[0];
    *max_freq = devfreq->freq_table[devfreq->max_state - 1];
#else
    if (devfreq->profile->max_state == 0U) {
        *min_freq = ULONG_MAX;
        *max_freq = ULONG_MAX;
        return;
    }

    *min_freq = devfreq->profile->freq_table[0];
    *max_freq = devfreq->profile->freq_table[devfreq->profile->max_state - 1];
#endif

    qos_min_freq = nv_pci_tegra_devfreq_pm_qos_read_value(devfreq, DEV_PM_QOS_MIN_FREQUENCY);
    qos_max_freq = nv_pci_tegra_devfreq_pm_qos_read_value(devfreq, DEV_PM_QOS_MAX_FREQUENCY);

    /* Apply constraints from PM QoS */
    *min_freq = max(*min_freq, (unsigned long)qos_min_freq * 1000);
    if (qos_max_freq != PM_QOS_MAX_FREQUENCY_DEFAULT_VALUE)
        *max_freq = min(*max_freq, (unsigned long)qos_max_freq * 1000);

    /* Apply constraints from OPP framework */
    *min_freq = max(*min_freq, devfreq->scaling_min_freq);
    *max_freq = min(*max_freq, devfreq->scaling_max_freq);
}
#endif // defined(NV_PM_QOS_READ_VALUE_SUPPORTED)

static unsigned long nv_pci_gb10b_devfreq_get_locked_freq(struct nv_pci_tegra_devfreq_dev *tdev)
{
#if defined(NV_PM_QOS_READ_VALUE_SUPPORTED)
    unsigned long min_freq, max_freq;

    if (tdev->devfreq_boost_type == TEGRASOC_DEVFREQ_CLK_BOOST_TYPE_RATED_TDP) {
        nv_pci_tegra_devfreq_get_freq_range(tdev->devfreq, &min_freq, &max_freq);
        return max_freq;
    }
#endif // defined(NV_PM_QOS_READ_VALUE_SUPPORTED)

#if defined(CONFIG_PM_DEVFREQ)
    return tdev->devfreq->scaling_max_freq;
#endif // defined(CONFIG_PM_DEVFREQ)

    return ULONG_MAX;
}

static int
nv_pci_gb10b_devfreq_target(struct device *dev, unsigned long *freq, u32 flags)
{
    struct pci_dev *pdev = to_pci_dev(dev->parent);
    struct nv_pci_tegra_devfreq_dev *tdev = to_tegra_devfreq_dev(dev), *tptr;
    unsigned long rate;
#if NV_HAS_ICC_SUPPORTED
    static const unsigned num_mss_ports = 8;
    static const unsigned mss_port_bandwidth = 32;
    static const unsigned gpu_bus_bandwidth = num_mss_ports * mss_port_bandwidth;
    u32 kBps;
#endif

    /* Skip clock scaling until runtime PM has fully reached RPM_ACTIVE. */
    if (!pm_runtime_active(&pdev->dev))
    {
        return 0;
    }

    if (tdev->devfreq_boost_type != TEGRASOC_DEVFREQ_CLK_BOOST_TYPE_DEFAULT) {
        if (tdev->devfreq_locked_freq == 0) {
            // Get target frequency based on the boost type
            tdev->devfreq_locked_freq = nv_pci_gb10b_devfreq_get_locked_freq(tdev);
        }

        *freq = tdev->devfreq_locked_freq;
    }

    clk_set_rate(tdev->clk, *freq);
    *freq = clk_get_rate(tdev->clk);

#if NV_HAS_ICC_SUPPORTED
    if (tdev->icc_path != NULL)
    {
        kBps = Bps_to_icc(*freq * gpu_bus_bandwidth * 400 / 1000);

        // Boost ICC request to max only if GPU is boosted FMAX
        if (tdev->devfreq_boost_type == TEGRASOC_DEVFREQ_CLK_BOOST_TYPE_FMAX)
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

        clk_set_rate(tptr->clk, rate);
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

        clk_set_rate(tptr->clk, rate);

    }

    return 0;
}

static int
nv_pci_tegra_devfreq_get_cur_freq(struct device *dev, unsigned long *freq)
{
    struct nv_pci_tegra_devfreq_dev *tdev = to_tegra_devfreq_dev(dev);

    //
    // Clock frequency reported from CCF is only valid when GPU is active.
    // If GPU is suspended, return 0 frequency because whole GPU is powered off.
    // Otherwise, return the last frequency step saved in devfreq.
    //
    if (pm_runtime_active(dev->parent))
        *freq = clk_get_rate(tdev->clk);
    else if (pm_runtime_suspended(dev->parent))
        *freq = 0;
    else
        *freq = tdev->devfreq->previous_freq;

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
    // GPU can be under suspending/suspended/resuming state defined the runtime
    // PM (RPM) framework. When device is under either of these states, DVFS based
    // on GPU load information should be disabled.
    //
    // Complete load-based DVFS cycle involve GPU load query through rmapi and
    // clock scaling through BPMP MRQ_CLK mailbox request, which will awake the
    // GPU and contradict the suspended state.
    //
    if (!pm_runtime_active(&pdev->dev))
    {
        stat->total_time = 100;
        stat->busy_time = 0;
        stat->current_frequency = tdev->devfreq->previous_freq;
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
    resource_size_t bar0_addr, bar0_size;
    void *bar0_map;
    u32 gpc_fuse_mask;

    while (pbus->parent != NULL)
    {
        pbus = pbus->parent;
    }

    node = max(0, dev_to_node(to_pci_host_bridge(pbus->bridge)->dev.parent));

    bar0_addr = (resource_size_t)nv->bars[NV_GPU_BAR_INDEX_REGS].cpu_address;
    bar0_size = (resource_size_t)nv->bars[NV_GPU_BAR_INDEX_REGS].size;
    bar0_map = devm_ioremap(&pdev->dev, bar0_addr, bar0_size);

    if (bar0_map == NULL)
    {
        gpc_fuse_mask = 0;
    }
    else
    {
        gpc_fuse_mask = readl(bar0_map + nv->gpc_fuse_status_offset);
    }

    nv_printf(NV_DBG_INFO, "NVRM: devfreq registration detects gpc_fuse_mask = %u\n", gpc_fuse_mask);

    for (i = 0; i < nvl->devfreq_table_size; i++)
    {
        tdata = &nvl->devfreq_table[i];

        /* skip redundant clk entry */
        if (tdata->clk_name == NULL)
        {
            continue;
        }

        /* skip clk entry if gpc is floor swept */
        if (gpc_fuse_mask && (gpc_fuse_mask & tdata->gpc_fuse_field))
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
        tdev->devfreq_boost_type = TEGRASOC_DEVFREQ_CLK_BOOST_TYPE_DEFAULT;
        tdev->devfreq_locked_freq = 0;

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
    struct nv_pci_tegra_devfreq_dev *gpc_tdev;
    struct nv_pci_tegra_devfreq_dev *nvd_tdev;
    struct devfreq *df;
    int err = 0;

    gpc_tdev = READ_ONCE(nvl->gpc_devfreq_dev);
    if (gpc_tdev != NULL)
    {
        df = READ_ONCE(gpc_tdev->devfreq);
        if (df != NULL)
        {
            err = nv_pci_tegra_try_suspend_devfreq(gpc_tdev);
            if (err)
            {
                return err;
            }

#if NV_HAS_ICC_SUPPORTED
            if (gpc_tdev->icc_path != NULL)
            {
                icc_set_bw(gpc_tdev->icc_path, 0, 0);
            }
#endif
        }
    }

    nvd_tdev = READ_ONCE(nvl->nvd_devfreq_dev);
    if (nvd_tdev != NULL)
    {
        df = READ_ONCE(nvd_tdev->devfreq);
        if (df != NULL)
        {
            err = nv_pci_tegra_try_suspend_devfreq(nvd_tdev);
            if (err)
            {
                nv_pci_tegra_unwind_devfreq_suspend(gpc_tdev);
                return err;
            }

#if NV_HAS_ICC_SUPPORTED
            if (nvd_tdev->icc_path != NULL)
            {
                icc_set_bw(nvd_tdev->icc_path, 0, 0);
            }
#endif
        }
    }

    return err;
}

static int
nv_pci_gb10b_resume_devfreq(struct device *dev)
{
    struct pci_dev *pci_dev = to_pci_dev(dev);
    nv_linux_state_t *nvl = pci_get_drvdata(pci_dev);
    struct nv_pci_tegra_devfreq_dev *gpc_tdev;
    struct nv_pci_tegra_devfreq_dev *nvd_tdev;
    struct devfreq *df;
    int err = 0;

    gpc_tdev = READ_ONCE(nvl->gpc_devfreq_dev);
    if (gpc_tdev != NULL)
    {
        df = READ_ONCE(gpc_tdev->devfreq);
        if (df != NULL)
        {
            err = nv_pci_tegra_try_resume_devfreq(gpc_tdev);
            if (err)
            {
                return err;
            }
#if defined(NV_UPDATE_DEVFREQ_PRESENT)
            /*
             * During GPU runtime suspended state, switching the devfreq governor
             * which doesn't poll for GPU utilization could lead to devfreq
             * frequency not being updated after runtime resume.
             *
             * Manually trigger the devfreq update here to ensure the
             * frequency is compliant with the devfreq governor.
             */
            nv_pci_tegra_update_devfreq(df);
#endif
        }
    }

    nvd_tdev = READ_ONCE(nvl->nvd_devfreq_dev);
    if (nvd_tdev != NULL)
    {
        df = READ_ONCE(nvd_tdev->devfreq);
        if (df != NULL)
        {
            err = nv_pci_tegra_try_resume_devfreq(nvd_tdev);
            if (err)
            {
                return err;
            }
#if defined(NV_UPDATE_DEVFREQ_PRESENT)
            nv_pci_tegra_update_devfreq(df);
#endif
        }
    }

    return err;
}

static void nv_pci_tegra_disable_devfreq_boost_work(struct work_struct *work)
{
#if defined(NV_UPDATE_DEVFREQ_PRESENT)
    struct nv_pci_tegra_devfreq_dev *tdev;

    tdev = container_of(work, struct nv_pci_tegra_devfreq_dev, boost_disable.work);
    tdev->devfreq_boost_type = TEGRASOC_DEVFREQ_CLK_BOOST_TYPE_DEFAULT;
    tdev->devfreq_locked_freq = 0;
#endif
}

static int
nv_pci_gb10b_enable_devfreq_boost(struct device *dev, unsigned int duration, int boost_type)
{
#if defined(NV_UPDATE_DEVFREQ_PRESENT)
    struct pci_dev *pci_dev = to_pci_dev(dev);
    nv_linux_state_t *nvl = pci_get_drvdata(pci_dev);
    struct nv_pci_tegra_devfreq_dev *gpc_tdev;
    struct nv_pci_tegra_devfreq_dev *nvd_tdev;
    unsigned long delay;

    delay = msecs_to_jiffies(duration * 1000);

    gpc_tdev = nvl->gpc_devfreq_dev;
    if ((gpc_tdev != NULL) &&
        (gpc_tdev->devfreq != NULL) &&
        (gpc_tdev->devfreq_boost_type == TEGRASOC_DEVFREQ_CLK_BOOST_TYPE_DEFAULT))
    {
        gpc_tdev->devfreq_boost_type = boost_type;

        // If duration is 0, do not set timer to disable boost state.
        if (duration != 0)
        {
            INIT_DELAYED_WORK(&gpc_tdev->boost_disable,
                              nv_pci_tegra_disable_devfreq_boost_work);
            schedule_delayed_work(&gpc_tdev->boost_disable, delay);
        }
    }

    nvd_tdev = nvl->nvd_devfreq_dev;
    if ((nvd_tdev != NULL) &&
        (nvd_tdev->devfreq != NULL) &&
        (nvd_tdev->devfreq_boost_type == TEGRASOC_DEVFREQ_CLK_BOOST_TYPE_DEFAULT))
    {
        nvd_tdev->devfreq_boost_type = boost_type;

        if (duration != 0)
        {
            INIT_DELAYED_WORK(&nvd_tdev->boost_disable,
                              nv_pci_tegra_disable_devfreq_boost_work);
            schedule_delayed_work(&nvd_tdev->boost_disable, delay);
        }
    }

    return 0;
#else // !defined(NV_UPDATE_DEVFREQ_PRESENT)
    return -1;
#endif
}

static int
nv_pci_gb10b_disable_devfreq_boost(struct device *dev)
{
#if defined(NV_UPDATE_DEVFREQ_PRESENT)
    struct pci_dev *pci_dev = to_pci_dev(dev);
    nv_linux_state_t *nvl = pci_get_drvdata(pci_dev);
    struct nv_pci_tegra_devfreq_dev *gpc_tdev;
    struct nv_pci_tegra_devfreq_dev *nvd_tdev;

    gpc_tdev = nvl->gpc_devfreq_dev;
    if ((gpc_tdev != NULL) &&
        (gpc_tdev->devfreq != NULL) &&
        (gpc_tdev->devfreq_boost_type != TEGRASOC_DEVFREQ_CLK_BOOST_TYPE_DEFAULT))
    {
        gpc_tdev->devfreq_boost_type = TEGRASOC_DEVFREQ_CLK_BOOST_TYPE_DEFAULT;
        gpc_tdev->devfreq_locked_freq = 0;
        cancel_delayed_work_sync(&gpc_tdev->boost_disable);
    }

    nvd_tdev = nvl->nvd_devfreq_dev;
    if ((nvd_tdev != NULL) &&
        (nvd_tdev->devfreq != NULL) &&
        (nvd_tdev->devfreq_boost_type != TEGRASOC_DEVFREQ_CLK_BOOST_TYPE_DEFAULT))
    {
        nvd_tdev->devfreq_boost_type = TEGRASOC_DEVFREQ_CLK_BOOST_TYPE_DEFAULT;
        nvd_tdev->devfreq_locked_freq = 0;
        cancel_delayed_work_sync(&nvd_tdev->boost_disable);
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
    int (*register_devfreq_cb)(struct pci_dev*);
    int (*suspend_devfreq_cb)(struct device*);
    int (*resume_devfreq_cb)(struct device*);
    int (*enable_devfreq_boost_cb)(struct device*, unsigned int, int);
    int (*disable_devfreq_boost_cb)(struct device*);
};

static const struct nv_pci_tegra_data nv_pci_tegra_table[] = {
    {
        .vendor = 0x10de,
        .device = 0x2b00,
        .devfreq_table = tegra_devfreq_tables[0],
        .devfreq_table_size = ARRAY_SIZE(tegra_devfreq_tables[0]),
        .register_devfreq_cb = nv_pci_gb10b_register_devfreq,
        .suspend_devfreq_cb = nv_pci_gb10b_suspend_devfreq,
        .resume_devfreq_cb = nv_pci_gb10b_resume_devfreq,
        .enable_devfreq_boost_cb = nv_pci_gb10b_enable_devfreq_boost,
        .disable_devfreq_boost_cb = nv_pci_gb10b_disable_devfreq_boost,
    },
    {
        .vendor = 0x10de,
        .device = 0x3400,
        .devfreq_table = tegra_devfreq_tables[1],
        .devfreq_table_size = ARRAY_SIZE(tegra_devfreq_tables[1]),
        .register_devfreq_cb = nv_pci_gb10b_register_devfreq,
        .suspend_devfreq_cb = nv_pci_gb10b_suspend_devfreq,
        .resume_devfreq_cb = nv_pci_gb10b_resume_devfreq,
        .enable_devfreq_boost_cb = nv_pci_gb10b_enable_devfreq_boost,
        .disable_devfreq_boost_cb = nv_pci_gb10b_disable_devfreq_boost,
    },
};

static void
nv_pci_tegra_devfreq_remove(struct nv_pci_tegra_devfreq_dev *tdev)
{
    struct nv_pci_tegra_devfreq_dev *tptr, *next;
    struct devfreq *df = READ_ONCE(tdev->devfreq);
#if NV_HAS_COOLING_SUPPORTED
    struct nv_pci_tegra_thermal_data *data;
    struct thermal_zone_device *tzdev;
#endif

#if defined(NV_UPDATE_DEVFREQ_PRESENT)
    if (tdev->devfreq_boost_type != TEGRASOC_DEVFREQ_CLK_BOOST_TYPE_DEFAULT)
    {
        tdev->devfreq_boost_type = TEGRASOC_DEVFREQ_CLK_BOOST_TYPE_DEFAULT;
        cancel_delayed_work_sync(&tdev->boost_disable);
    }
#endif

    if (df != NULL)
    {
#if NV_HAS_COOLING_SUPPORTED
        list_for_each_entry(data, &tdev->therm_zones, zones)
        {
            tzdev = thermal_zone_get_zone_by_name(data->tz_name);
            if (IS_ERR(tzdev))
            {
                continue;
            }

            thermal_unbind_cdev_from_trip(tzdev, data->passive_trip, df->cdev);
        }
#endif
        nv_pci_tegra_devfreq_remove_opps(tdev);
        //
        // Clear the cached devfreq pointer before entering the
        // devfreq core remove path. A runtime suspend racing teardown then sees
        // tdev->devfreq == NULL and skips devfreq_suspend_device() instead of
        // dereferencing a devfreq object whose teardown has already started.
        //
        WRITE_ONCE(tdev->devfreq, NULL);
        WRITE_ONCE(tdev->is_devfreq_suspended, false);
        devm_devfreq_remove_device(&tdev->dev, df);
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
        devm_clk_put(tdev->dev.parent, tdev->clk);
        tdev->clk = NULL;
        device_unregister(&tdev->dev);
    }
}

void
nv_pci_tegra_unregister_devfreq(struct pci_dev *pdev)
{
    nv_linux_state_t *nvl = pci_get_drvdata(pdev);

    //
    // Bug 6307421: block new runtime-PM callbacks, disarm the devfreq hooks,
    // then drain any callback that already entered nv_pmops_runtime_suspend()
    // before tearing down the devfreq devices. Otherwise pm_runtime_work can
    // race nv_pci_tegra_devfreq_remove() and call devfreq_suspend_device() on a
    // devfreq object whose teardown has already started.
    //
    pm_runtime_get_noresume(&pdev->dev);
    WRITE_ONCE(nvl->suspend_devfreq_cb, NULL);
    WRITE_ONCE(nvl->resume_devfreq_cb, NULL);
    WRITE_ONCE(nvl->enable_devfreq_boost_cb, NULL);
    WRITE_ONCE(nvl->disable_devfreq_boost_cb, NULL);
    pm_runtime_barrier(&pdev->dev);

    if (nvl->gpc_devfreq_dev != NULL)
    {
        nv_pci_tegra_devfreq_remove(nvl->gpc_devfreq_dev);
        WRITE_ONCE(nvl->gpc_devfreq_dev, NULL);
    }

    if (nvl->nvd_devfreq_dev != NULL)
    {
        nv_pci_tegra_devfreq_remove(nvl->nvd_devfreq_dev);
        WRITE_ONCE(nvl->nvd_devfreq_dev, NULL);
    }

    pm_runtime_put_noidle(&pdev->dev);
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

void nv_pci_tegra_boost_clocks(struct device *dev)
{
#if defined(CONFIG_PM_DEVFREQ)
    nv_linux_state_t *nvl = pci_get_drvdata(to_pci_dev(dev));
    const struct nv_pci_tegra_devfreq_data *tdata;
    struct clk *clk = NULL;
    long max_freq;
    int i;

    for (i = 0; i < nvl->devfreq_table_size; i++)
    {
        tdata = &nvl->devfreq_table[i];

        /* skip redundant clk entry */
        if (tdata->clk_name == NULL)
        {
            continue;
        }

        /*
         * Don't boost the GPC, NVD clocks but only the uprocclk
         * and sysclk. This saves power consumption of GPU rail
         * and reduces impact to GPU suspend/resume latency.
         */
        if (strstr(tdata->clk_name, "uproc") || strstr(tdata->clk_name, "sys"))
        {
            clk = devm_clk_get(dev, tdata->clk_name);
            if (IS_ERR_OR_NULL(clk))
            {
                continue;
            }
        }
        else
        {
            continue;
        }

        max_freq = clk_round_rate(clk, ULONG_MAX);
        if (max_freq >= 0)
        {
            clk_set_rate(clk, max_freq);
        }
        devm_clk_put(dev, clk);
    }
#endif
}

int
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
    err = tegra_data->register_devfreq_cb(pdev);
    if (err != 0)
    {
        nv_pci_tegra_unregister_devfreq(pdev);
        return err;
    }

    WRITE_ONCE(nvl->suspend_devfreq_cb, tegra_data->suspend_devfreq_cb);
    WRITE_ONCE(nvl->resume_devfreq_cb, tegra_data->resume_devfreq_cb);
    WRITE_ONCE(nvl->enable_devfreq_boost_cb, tegra_data->enable_devfreq_boost_cb);
    WRITE_ONCE(nvl->disable_devfreq_boost_cb, tegra_data->disable_devfreq_boost_cb);

    return 0;
}
#endif // defined(CONFIG_PM_DEVFREQ)
