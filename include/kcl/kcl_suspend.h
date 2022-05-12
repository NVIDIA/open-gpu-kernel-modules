/* SPDX-License-Identifier: GPL-2.0 */
#ifndef AMDKCL_SUSPEND_H
#define AMDKCL_SUSPEND_H

#ifndef HAVE_KSYS_SYNC_HELPER
#ifdef CONFIG_PM_SLEEP
extern void _kcl_ksys_sync_helper(void);

static inline void ksys_sync_helper(void)
{
	_kcl_ksys_sync_helper();
}
#else
static inline void ksys_sync_helper(void) {}
#endif /* CONFIG_PM_SLEEP */
#endif /* HAVE_KSYS_SYNC_HELPER */

#ifndef HAVE_PM_SUSPEND_VIA_FIRMWARE
static inline bool pm_suspend_via_firmware(void) { return false; }
#endif /* HAVE_PM_SUSPEND_VIA_FIRMWARE */

#endif /* AMDKCL_SUSPEND_H */
