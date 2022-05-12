// SPDX-License-Identifier: GPL-2.0
/*
 * fs/sysfs/file.c - sysfs regular (text) file implementation
 *
 * Copyright (c) 2001-3 Patrick Mochel
 * Copyright (c) 2007 SUSE Linux Products GmbH
 * Copyright (c) 2007 Tejun Heo <teheo@suse.de>
 *
 * Please see Documentation/filesystems/sysfs.rst for more information.
 */
#include <linux/mm.h>
#include <linux/sysfs.h>

/* Copied from fs/sysfs/file.c */
#ifndef HAVE_SYSFS_EMIT
int sysfs_emit(char *buf, const char *fmt, ...)
{
        va_list args;
        int len;

        if (WARN(!buf || offset_in_page(buf),
                 "invalid sysfs_emit: buf:%p\n", buf))
                return 0;

        va_start(args, fmt);
        len = vscnprintf(buf, PAGE_SIZE, fmt, args);
        va_end(args);

        return len;
}
EXPORT_SYMBOL_GPL(sysfs_emit);

/**
 *      sysfs_emit_at - scnprintf equivalent, aware of PAGE_SIZE buffer.
 *      @buf:   start of PAGE_SIZE buffer.
 *      @at:    offset in @buf to start write in bytes
 *              @at must be >= 0 && < PAGE_SIZE
 *      @fmt:   format
 *      @...:   optional arguments to @fmt
 *
 *
 * Returns number of characters written starting at &@buf[@at].
 */
int sysfs_emit_at(char *buf, int at, const char *fmt, ...)
{
        va_list args;
        int len;

        if (WARN(!buf || offset_in_page(buf) || at < 0 || at >= PAGE_SIZE,
                 "invalid sysfs_emit_at: buf:%p at:%d\n", buf, at))
                return 0;

        va_start(args, fmt);
        len = vscnprintf(buf + at, PAGE_SIZE - at, fmt, args);
        va_end(args);

        return len;
}
EXPORT_SYMBOL_GPL(sysfs_emit_at);

#endif
