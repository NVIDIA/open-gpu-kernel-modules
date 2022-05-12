/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Utility functions for parsing Tegra CVB voltage tables
 */

#ifndef __DRIVERS_CLK_TEGRA_CVB_H
#define __DRIVERS_CLK_TEGRA_CVB_H

#include <linux/types.h>

struct device;

#define MAX_DVFS_FREQS	40

struct rail_alignment {
	int offset_uv;
	int step_uv;
};

struct cvb_coefficients {
	int c0;
	int c1;
	int c2;
};

struct cvb_table_freq_entry {
	unsigned long freq;
	struct cvb_coefficients coefficients;
};

struct cvb_cpu_dfll_data {
	u32 tune0_low;
	u32 tune0_high;
	u32 tune1;
	unsigned int tune_high_min_millivolts;
};

struct cvb_table {
	int speedo_id;
	int process_id;

	int min_millivolts;
	int max_millivolts;

	int speedo_scale;
	int voltage_scale;
	struct cvb_table_freq_entry entries[MAX_DVFS_FREQS];
	struct cvb_cpu_dfll_data cpu_dfll_data;
};

const struct cvb_table *
tegra_cvb_add_opp_table(struct device *dev, const struct cvb_table *cvb_tables,
			size_t count, struct rail_alignment *align,
			int process_id, int speedo_id, int speedo_value,
			unsigned long max_freq);
void tegra_cvb_remove_opp_table(struct device *dev,
				const struct cvb_table *table,
				unsigned long max_freq);

#endif
