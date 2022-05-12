// SPDX-License-Identifier: GPL-2.0
#include <linux/version.h>
#include <linux/sched.h>
#include <linux/sched/mm.h>
#include <linux/mmu_notifier.h>
#include <kcl/kcl_mn.h>

/* Copied from v3.16-6588-gb972216e27d1 mm/mmu_notifier.c */
#if !defined(HAVE_MMU_NOTIFIER_CALL_SRCU) && \
	!defined(HAVE_MMU_NOTIFIER_PUT)
/*
 * Modifications [2017-03-14] (c) [2017]
 */

/*
 * This function allows mmu_notifier::release callback to delay a call to
 * a function that will free appropriate resources. The function must be
 * quick and must not block.
 */
void mmu_notifier_call_srcu(struct rcu_head *rcu,
			    void (*func)(struct rcu_head *rcu))
{
	/* changed from call_srcu to call_rcu */
	call_rcu(rcu, func);
}
EXPORT_SYMBOL_GPL(mmu_notifier_call_srcu);

void mmu_notifier_unregister_no_release(struct mmu_notifier *mn,
					struct mm_struct *mm)
{
	spin_lock(&mm->mmu_notifier_mm->lock);
	/*
	 * Can not use list_del_rcu() since __mmu_notifier_release
	 * can delete it before we hold the lock.
	 */
	hlist_del_init_rcu(&mn->hlist);
	spin_unlock(&mm->mmu_notifier_mm->lock);

	BUG_ON(atomic_read(&mm->mm_count) <= 0);
	mmdrop(mm);
}
EXPORT_SYMBOL_GPL(mmu_notifier_unregister_no_release);
#endif
