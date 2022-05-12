/* SPDX-License-Identifier: GPL-2.0 */
#ifndef AMDKCL_UACCESS_BACKPORT_H
#define AMDKCL_UACCESS_BACKPORT_H
#include <linux/uaccess.h>

static inline int kcl_access_ok(unsigned long addr, unsigned long size)
{
#if !defined(HAVE_ACCESS_OK_WITH_TWO_ARGUMENTS)
	return access_ok(VERIFY_WRITE, (addr), (size));
#else
        return access_ok((addr), (size));
#endif
}
#endif
