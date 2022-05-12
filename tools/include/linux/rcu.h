/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LIBLOCKDEP_RCU_H_
#define _LIBLOCKDEP_RCU_H_

int rcu_scheduler_active;

static inline int rcu_lockdep_current_cpu_online(void)
{
	return 1;
}

static inline int rcu_is_cpu_idle(void)
{
	return 1;
}

static inline bool rcu_is_watching(void)
{
	return false;
}

#define rcu_assign_pointer(p, v)	do { (p) = (v); } while (0)
#define RCU_INIT_POINTER(p, v)	do { (p) = (v); } while (0)

#endif
