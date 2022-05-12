/* SPDX-License-Identifier: GPL-2.0 */
#ifndef AMDKCL_MN_H
#define AMDKCL_MN_H

#include <linux/mmu_notifier.h>

/* mmu_notifier_put in the RH DRM backport from 5.6 is broken */
#if DRM_VER == 5 && DRM_PATCH == 6 && \
	LINUX_VERSION_CODE == KERNEL_VERSION(4, 18, 0)
#undef HAVE_MMU_NOTIFIER_PUT
#endif

/* Copied from v3.16-6588-gb972216e27d1 include/linux/mmu_notifier.h */
#if !defined(HAVE_MMU_NOTIFIER_CALL_SRCU) && \
	!defined(HAVE_MMU_NOTIFIER_PUT)
extern void mmu_notifier_call_srcu(struct rcu_head *rcu,
                            void (*func)(struct rcu_head *rcu));
extern void mmu_notifier_unregister_no_release(struct mmu_notifier *mn,
					struct mm_struct *mm);
#endif

#endif /* AMDKCL_MN_H */
