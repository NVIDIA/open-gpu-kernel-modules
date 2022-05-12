// SPDX-License-Identifier: GPL-2.0
/*
 * sysfs.h - definitions for the device driver filesystem
 *
 * Copyright (c) 2001,2002 Patrick Mochel
 * Copyright (c) 2004 Silicon Graphics, Inc.
 * Copyright (c) 2007 SUSE Linux Products GmbH
 * Copyright (c) 2007 Tejun Heo <teheo@suse.de>
 *
 * Please see Documentation/filesystems/sysfs.rst for more information.
 */
#include <linux/sysfs.h>

#ifndef HAVE_SYSFS_EMIT
#ifdef CONFIG_SYSFS
__printf(2, 3)
int sysfs_emit(char *buf, const char *fmt, ...);

__printf(3, 4)
int sysfs_emit_at(char *buf, int at, const char *fmt, ...);

#else
__printf(2, 3)
static inline int sysfs_emit(char *buf, const char *fmt, ...)
{
        return 0;
}

__printf(3, 4)
static inline int sysfs_emit_at(char *buf, int at, const char *fmt, ...)
{
        return 0;
}
#endif
#endif
