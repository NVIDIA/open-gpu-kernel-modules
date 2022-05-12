// SPDX-License-Identifier: GPL-2.0-only
/*
 * OMAP3/OMAP4 smartreflex device file
 *
 * Author: Thara Gopinath	<thara@ti.com>
 *
 * Based originally on code from smartreflex.c
 * Copyright (C) 2010 Texas Instruments, Inc.
 * Thara Gopinath <thara@ti.com>
 *
 * Copyright (C) 2008 Nokia Corporation
 * Kalle Jokiniemi
 *
 * Copyright (C) 2007 Texas Instruments, Inc.
 * Lesly A M <x0080970@ti.com>
 */
#include <linux/power/smartreflex.h>

#include <linux/err.h>
#include <linux/slab.h>
#include <linux/io.h>

#include "soc.h"
#include "omap_device.h"
#include "voltage.h"
#include "control.h"
#include "pm.h"

static bool sr_enable_on_init;

/* Read EFUSE values from control registers for OMAP3430 */
static void __init sr_set_nvalues(struct omap_volt_data *volt_data,
				struct omap_sr_data *sr_data)
{
	struct omap_sr_nvalue_table *nvalue_table;
	int i, j, count = 0;

	sr_data->nvalue_count = 0;
	sr_data->nvalue_table = NULL;

	while (volt_data[count].volt_nominal)
		count++;

	nvalue_table = kcalloc(count, sizeof(*nvalue_table), GFP_KERNEL);
	if (!nvalue_table)
		return;

	for (i = 0, j = 0; i < count; i++) {
		u32 v;

		/*
		 * In OMAP4 the efuse registers are 24 bit aligned.
		 * A readl_relaxed will fail for non-32 bit aligned address
		 * and hence the 8-bit read and shift.
		 */
		if (cpu_is_omap44xx()) {
			u16 offset = volt_data[i].sr_efuse_offs;

			v = omap_ctrl_readb(offset) |
				omap_ctrl_readb(offset + 1) << 8 |
				omap_ctrl_readb(offset + 2) << 16;
		} else {
			v = omap_ctrl_readl(volt_data[i].sr_efuse_offs);
		}

		/*
		 * Many OMAP SoCs don't have the eFuse values set.
		 * For example, pretty much all OMAP3xxx before
		 * ES3.something.
		 *
		 * XXX There needs to be some way for board files or
		 * userspace to add these in.
		 */
		if (v == 0)
			continue;

		nvalue_table[j].nvalue = v;
		nvalue_table[j].efuse_offs = volt_data[i].sr_efuse_offs;
		nvalue_table[j].errminlimit = volt_data[i].sr_errminlimit;
		nvalue_table[j].volt_nominal = volt_data[i].volt_nominal;

		j++;
	}

	sr_data->nvalue_table = nvalue_table;
	sr_data->nvalue_count = j;
}

extern struct omap_sr_data omap_sr_pdata[];

static int __init sr_init_by_name(const char *name, const char *voltdm)
{
	struct omap_sr_data *sr_data = NULL;
	struct omap_volt_data *volt_data;
	static int i;

	if (!strncmp(name, "smartreflex_mpu_iva", 20) ||
	    !strncmp(name, "smartreflex_mpu", 16))
		sr_data = &omap_sr_pdata[OMAP_SR_MPU];
	else if (!strncmp(name, "smartreflex_core", 17))
		sr_data = &omap_sr_pdata[OMAP_SR_CORE];
	else if (!strncmp(name, "smartreflex_iva", 16))
		sr_data = &omap_sr_pdata[OMAP_SR_IVA];

	if (!sr_data) {
		pr_err("%s: Unknown instance %s\n", __func__, name);
		return -EINVAL;
	}

	sr_data->name = name;
	if (cpu_is_omap343x())
		sr_data->ip_type = 1;
	else
		sr_data->ip_type = 2;
	sr_data->senn_mod = 0x1;
	sr_data->senp_mod = 0x1;

	if (cpu_is_omap34xx() || cpu_is_omap44xx()) {
		sr_data->err_weight = OMAP3430_SR_ERRWEIGHT;
		sr_data->err_maxlimit = OMAP3430_SR_ERRMAXLIMIT;
		sr_data->accum_data = OMAP3430_SR_ACCUMDATA;
		if (!(strcmp(sr_data->name, "smartreflex_mpu"))) {
			sr_data->senn_avgweight = OMAP3430_SR1_SENNAVGWEIGHT;
			sr_data->senp_avgweight = OMAP3430_SR1_SENPAVGWEIGHT;
		} else {
			sr_data->senn_avgweight = OMAP3430_SR2_SENNAVGWEIGHT;
			sr_data->senp_avgweight = OMAP3430_SR2_SENPAVGWEIGHT;
		}
	}

	sr_data->voltdm = voltdm_lookup(voltdm);
	if (!sr_data->voltdm) {
		pr_err("%s: Unable to get voltage domain pointer for VDD %s\n",
			__func__, voltdm);
		goto exit;
	}

	omap_voltage_get_volttable(sr_data->voltdm, &volt_data);
	if (!volt_data) {
		pr_err("%s: No Voltage table registered for VDD%d\n",
		       __func__, i + 1);
		goto exit;
	}

	sr_set_nvalues(volt_data, sr_data);

	sr_data->enable_on_init = sr_enable_on_init;

exit:
	i++;

	return 0;
}

#ifdef CONFIG_OMAP_HWMOD
static int __init sr_dev_init(struct omap_hwmod *oh, void *user)
{
	struct omap_smartreflex_dev_attr *sr_dev_attr;

	sr_dev_attr = (struct omap_smartreflex_dev_attr *)oh->dev_attr;
	if (!sr_dev_attr || !sr_dev_attr->sensor_voltdm_name) {
		pr_err("%s: No voltage domain specified for %s. Cannot initialize\n",
		       __func__, oh->name);
		return 0;
	}

	return sr_init_by_name(oh->name, sr_dev_attr->sensor_voltdm_name);
}
#else
static int __init sr_dev_init(struct omap_hwmod *oh, void *user)
{
	return -EINVAL;
}
#endif

/*
 * API to be called from board files to enable smartreflex
 * autocompensation at init.
 */
void __init omap_enable_smartreflex_on_init(void)
{
	sr_enable_on_init = true;
}

static const char * const omap4_sr_instances[] = {
	"mpu",
	"iva",
	"core",
};

static const char * const dra7_sr_instances[] = {
	"mpu",
	"core",
};

int __init omap_devinit_smartreflex(void)
{
	const char * const *sr_inst = NULL;
	int i, nr_sr = 0;

	if (soc_is_omap44xx()) {
		sr_inst = omap4_sr_instances;
		nr_sr = ARRAY_SIZE(omap4_sr_instances);

	} else if (soc_is_dra7xx()) {
		sr_inst = dra7_sr_instances;
		nr_sr = ARRAY_SIZE(dra7_sr_instances);
	}

	if (nr_sr) {
		const char *name, *voltdm;

		for (i = 0; i < nr_sr; i++) {
			name = kasprintf(GFP_KERNEL, "smartreflex_%s", sr_inst[i]);
			voltdm = sr_inst[i];
			sr_init_by_name(name, voltdm);
		}

		return 0;
	}

	return omap_hwmod_for_each_by_class("smartreflex", sr_dev_init, NULL);
}
