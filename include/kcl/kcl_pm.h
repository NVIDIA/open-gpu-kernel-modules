/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  pm.h - Power management interface
 *
 *  Copyright (C) 2000 Andrew Henroid
 */
#ifndef KCL_KCL_PM_H
#define KCL_KCL_PM_H

#include <linux/bitops.h>
#include <linux/pm.h>

/*
 * v5.7-rc2-7-ge07515563d01
 * PM: sleep: core: Rename DPM_FLAG_NEVER_SKIP
 */
#ifndef DPM_FLAG_NO_DIRECT_COMPLETE
#define DPM_FLAG_NO_DIRECT_COMPLETE DPM_FLAG_NEVER_SKIP
#endif


/*
 * v4.15-rc1-1-g0d4b54c6fee8
 * PM / core: Add LEAVE_SUSPENDED driver flag
 */
#ifndef DPM_FLAG_SMART_SUSPEND
#define DPM_FLAG_SMART_SUSPEND          BIT(2)
#endif

/*
 * v5.7-rc2-8-g2a3f34750b8b
 * PM: sleep: core: Rename DPM_FLAG_LEAVE_SUSPENDED
 */
#ifndef DPM_FLAG_MAY_SKIP_RESUME
#define DPM_FLAG_MAY_SKIP_RESUME        BIT(3)
#endif

#endif
