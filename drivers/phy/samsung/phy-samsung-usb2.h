/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Samsung SoC USB 1.1/2.0 PHY driver
 *
 * Copyright (C) 2013 Samsung Electronics Co., Ltd.
 * Author: Kamil Debski <k.debski@samsung.com>
 */

#ifndef _PHY_EXYNOS_USB2_H
#define _PHY_EXYNOS_USB2_H

#include <linux/clk.h>
#include <linux/phy/phy.h>
#include <linux/device.h>
#include <linux/regmap.h>
#include <linux/spinlock.h>
#include <linux/regulator/consumer.h>

#define KHZ 1000
#define MHZ (KHZ * KHZ)

struct samsung_usb2_phy_driver;
struct samsung_usb2_phy_instance;
struct samsung_usb2_phy_config;

struct samsung_usb2_phy_instance {
	const struct samsung_usb2_common_phy *cfg;
	struct phy *phy;
	struct samsung_usb2_phy_driver *drv;
	int int_cnt;
	int ext_cnt;
};

struct samsung_usb2_phy_driver {
	const struct samsung_usb2_phy_config *cfg;
	struct clk *clk;
	struct clk *ref_clk;
	struct regulator *vbus;
	unsigned long ref_rate;
	u32 ref_reg_val;
	struct device *dev;
	void __iomem *reg_phy;
	struct regmap *reg_pmu;
	struct regmap *reg_sys;
	spinlock_t lock;
	struct samsung_usb2_phy_instance instances[];
};

struct samsung_usb2_common_phy {
	int (*power_on)(struct samsung_usb2_phy_instance *);
	int (*power_off)(struct samsung_usb2_phy_instance *);
	unsigned int id;
	char *label;
};


struct samsung_usb2_phy_config {
	const struct samsung_usb2_common_phy *phys;
	int (*rate_to_clk)(unsigned long, u32 *);
	unsigned int num_phys;
	bool has_mode_switch;
	bool has_refclk_sel;
};

extern const struct samsung_usb2_phy_config exynos3250_usb2_phy_config;
extern const struct samsung_usb2_phy_config exynos4210_usb2_phy_config;
extern const struct samsung_usb2_phy_config exynos4x12_usb2_phy_config;
extern const struct samsung_usb2_phy_config exynos5250_usb2_phy_config;
extern const struct samsung_usb2_phy_config exynos5420_usb2_phy_config;
extern const struct samsung_usb2_phy_config s5pv210_usb2_phy_config;
#endif
