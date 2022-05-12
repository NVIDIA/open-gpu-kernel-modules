/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Definitions for the NVM Express interface
 * Copyright (c) 2011-2014, Intel Corporation.
 */
#ifndef AMDKCL_DEVICE_H
#define AMDKCL_DEVICE_H

#include <linux/ratelimit.h>
#include <linux/device.h>

/* Copied from include/linux/dev_printk.h */
#if !defined(dev_err_once)
#ifdef CONFIG_PRINTK
#define dev_level_once(dev_level, dev, fmt, ...)			\
do {									\
	static bool __print_once __read_mostly;				\
									\
	if (!__print_once) {						\
		__print_once = true;					\
		dev_level(dev, fmt, ##__VA_ARGS__);			\
	}								\
} while (0)
#else
#define dev_level_once(dev_level, dev, fmt, ...)			\
do {									\
	if (0)								\
		dev_level(dev, fmt, ##__VA_ARGS__);			\
} while (0)
#endif

#define dev_err_once(dev, fmt, ...)					\
	dev_level_once(dev_err, dev, fmt, ##__VA_ARGS__)
#endif

#if !defined(dev_err_ratelimited)
#define dev_level_ratelimited(dev_level, dev, fmt, ...)			\
do {									\
	static DEFINE_RATELIMIT_STATE(_rs,				\
				      DEFAULT_RATELIMIT_INTERVAL,	\
				      DEFAULT_RATELIMIT_BURST);		\
	if (__ratelimit(&_rs))						\
		dev_level(dev, fmt, ##__VA_ARGS__);			\
} while (0)

#define dev_err_ratelimited(dev, fmt, ...)				\
	dev_level_ratelimited(dev_err, dev, fmt, ##__VA_ARGS__)
#endif

#if !defined(HAVE_DEV_PM_SET_DRIVER_FLAGS)
/* rhel7.7 wrap macro dev_pm_set_driver_flags in drm/drm_backport.h */
#ifdef dev_pm_set_driver_flags
#undef dev_pm_set_driver_flags
#endif
#define DPM_FLAG_NEVER_SKIP    BIT(0)
#define DPM_FLAG_SMART_PREPARE BIT(1)
static inline void dev_pm_set_driver_flags(struct device *dev, u32 flags)
{
	pr_warn_once("%s is not available\n", __func__);
}
#endif

#endif /* AMDKCL_DEVICE_H */
