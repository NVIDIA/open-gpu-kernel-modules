/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * include/linux/idr.h
 *
 * 2002-10-18  written by Jim Houston jim.houston@ccur.com
 *	Copyright (C) 2002 by Concurrent Computer Corporation
 *
 * Small id to pointer translation service avoiding fixed sized
 * tables.
 */
#ifndef AMDKCL_IDR_H
#define AMDKCL_IDR_H

#include <linux/idr.h>

/* Copied from v4.4-rc2-61-ga55bbd375d18 include/linux/idr.h */
#ifndef idr_for_each_entry_continue
#define idr_for_each_entry_continue(idr, entry, id)                    \
       for ((entry) = idr_get_next((idr), &(id));                      \
            entry;                                                     \
            ++id, (entry) = idr_get_next((idr), &(id)))
#endif

#ifndef HAVE_IDR_REMOVE_RETURN_VOID_POINTER
static inline void *_kcl_idr_remove(struct idr *idr, int id)
{
	void *ptr;

	ptr = idr_find(idr, id);
	if (ptr)
		idr_remove(idr, id);

	return ptr;
}
#define idr_remove _kcl_idr_remove
#endif /* HAVE_IDR_REMOVE_RETURN_VOID_POINTER */

#endif /* AMDKCL_IDR_H */
