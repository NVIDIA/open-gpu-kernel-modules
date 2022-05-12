/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * aQuantia Corporation Network Driver
 * Copyright (C) 2014-2017 aQuantia Corporation. All rights reserved
 */

/* File aq_utils.h: Useful macro and structures used in all layers of driver. */

#ifndef AQ_UTILS_H
#define AQ_UTILS_H

#include "aq_common.h"

static inline void aq_utils_obj_set(atomic_t *flags, u32 mask)
{
	unsigned long flags_old, flags_new;

	do {
		flags_old = atomic_read(flags);
		flags_new = flags_old | (mask);
	} while (atomic_cmpxchg(flags, flags_old, flags_new) != flags_old);
}

static inline void aq_utils_obj_clear(atomic_t *flags, u32 mask)
{
	unsigned long flags_old, flags_new;

	do {
		flags_old = atomic_read(flags);
		flags_new = flags_old & ~(mask);
	} while (atomic_cmpxchg(flags, flags_old, flags_new) != flags_old);
}

static inline bool aq_utils_obj_test(atomic_t *flags, u32 mask)
{
	return atomic_read(flags) & mask;
}

#endif /* AQ_UTILS_H */
