// SPDX-License-Identifier: GPL-2.0
/*
 * device_cgroup.c - device cgroup subsystem
 *
 * Copyright 2007 IBM Corp
 */
#include <linux/module.h>
#include <kcl/kcl_device_cgroup.h>

#if defined(CONFIG_CGROUP_DEVICE) && \
	!defined(HAVE_DEVCGROUP_CHECK_PERMISSION)
/*
 * __devcgroup_check_permission is introduced in v3.6-6796-gad676077a2ae
 * as:
 * static int __devcgroup_check_permission(struct dev_cgroup *dev_cgroup,
 *                                     short type, u32 major, u32 minor,
 *                                     short access)
 *
 * prototype change in v3.7-rc2-147-g8c9506d16925 to:
 * static int __devcgroup_check_permission(short type, u32 major, u32 minor,
 *                                      short access)
 *
 * the current amdkcl don't support kernel earilier than v3.7-rc2-147-g8c9506d16925
 */
int (*__kcl_devcgroup_check_permission)(short type, u32 major, u32 minor,
					short access);
EXPORT_SYMBOL(__kcl_devcgroup_check_permission);
#endif
void amdkcl_dev_cgroup_init(void)
{
#if defined(CONFIG_CGROUP_DEVICE) && \
	!defined(HAVE_DEVCGROUP_CHECK_PERMISSION)
	__kcl_devcgroup_check_permission = amdkcl_fp_setup("__devcgroup_check_permission", NULL);
#endif
}
