/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Landlock LSM - Limits for different components
 *
 * Copyright © 2016-2020 Mickaël Salaün <mic@digikod.net>
 * Copyright © 2018-2020 ANSSI
 */

#ifndef _SECURITY_LANDLOCK_LIMITS_H
#define _SECURITY_LANDLOCK_LIMITS_H

#include <linux/limits.h>
#include <uapi/linux/landlock.h>

#define LANDLOCK_MAX_NUM_LAYERS		64
#define LANDLOCK_MAX_NUM_RULES		U32_MAX

#define LANDLOCK_LAST_ACCESS_FS		LANDLOCK_ACCESS_FS_MAKE_SYM
#define LANDLOCK_MASK_ACCESS_FS		((LANDLOCK_LAST_ACCESS_FS << 1) - 1)

#endif /* _SECURITY_LANDLOCK_LIMITS_H */
