/* SPDX-License-Identifier: GPL-2.0 OR MIT */
/**************************************************************************
 *
 * Copyright (c) 2006-2009 VMware, Inc., Palo Alto, CA., USA
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sub license, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * THE COPYRIGHT HOLDERS, AUTHORS AND/OR ITS SUPPLIERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
 * USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 **************************************************************************/
/*
 * Authors: Thomas Hellstrom <thellstrom-at-vmware-dot-com>
 * 	    Jerome Glisse
 */
#include <linux/module.h>
#include <linux/device.h>
#ifdef HAVE_LINUX_PGTABLE_H
#include <linux/pgtable.h>
#endif
#include <linux/sched.h>
#include <linux/debugfs.h>
#include <drm/drm_sysfs.h>
#include <drm/ttm/ttm_caching.h>

#include "ttm_module.h"

/**
 * ttm_prot_from_caching - Modify the page protection according to the
 * ttm cacing mode
 * @caching: The ttm caching mode
 * @tmp: The original page protection
 *
 * Return: The modified page protection
 */
pgprot_t ttm_prot_from_caching(enum ttm_caching caching, pgprot_t tmp)
{
	/* Cached mappings need no adjustment */
	if (caching == ttm_cached)
		return tmp;

#if defined(__i386__) || defined(__x86_64__)
	if (caching == ttm_write_combined)
		tmp = pgprot_writecombine(tmp);
	else if (boot_cpu_data.x86 > 3)
		tmp = pgprot_noncached(tmp);
#endif
#if defined(__ia64__) || defined(__arm__) || defined(__aarch64__) || \
	defined(__powerpc__) || defined(__mips__)
	if (caching == ttm_write_combined)
		tmp = pgprot_writecombine(tmp);
	else
		tmp = pgprot_noncached(tmp);
#endif
#if defined(__sparc__)
	tmp = pgprot_noncached(tmp);
#endif
	return tmp;
}

struct dentry *ttm_debugfs_root;

static int __init ttm_init(void)
{
	ttm_debugfs_root = debugfs_create_dir(TTM_NAME, NULL);
	return 0;
}

static void __exit ttm_exit(void)
{
	debugfs_remove(ttm_debugfs_root);
}

module_init(ttm_init);
module_exit(ttm_exit);

MODULE_AUTHOR("Thomas Hellstrom, Jerome Glisse");
MODULE_DESCRIPTION("TTM memory manager subsystem (for DRM device)");
MODULE_LICENSE("GPL and additional rights");
