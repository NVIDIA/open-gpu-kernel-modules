/* SPDX-License-Identifier: GPL-2.0 */
#ifndef AMDKCL_MMU_NOTIFIER_H
#define AMDKCL_MMU_NOTIFIER_H

#include <linux/mmu_notifier.h>

#if !defined(HAVE_MMU_NOTIFIER_RANGE_BLOCKABLE)
/* Copied from v5.1-10225-g4a83bfe916f3 include/linux/mmu_notifier.h */
#if defined(CONFIG_MMU_NOTIFIER) && \
	defined(HAVE_2ARGS_INVALIDATE_RANGE_START)
static inline bool
mmu_notifier_range_blockable(const struct mmu_notifier_range *range)
{
/*
 * It's for rhel8.5 which has the latest struct mmu_notifier_range
 * and no mmu_notifier_range_blockable
 */
#ifdef MMU_NOTIFIER_RANGE_BLOCKABLE
	return (range->flags & MMU_NOTIFIER_RANGE_BLOCKABLE);
#else
	return range->blockable;
#endif
}
#else
struct mmu_notifier_range;
static inline bool
mmu_notifier_range_blockable(const struct mmu_notifier_range *range)
{
	return true;
}
#endif
#endif /* HAVE_MMU_NOTIFIER_RANGE_BLOCKABLE */

#endif /* AMDKCL_MMU_NOTIFIER_H */
