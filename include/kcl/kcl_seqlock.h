/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _KCL_KCL_SEQLOCK_H
#define _KCL_KCL_SEQLOCK_H

#include <linux/seqlock.h>

#ifndef write_seqcount_begin
struct ww_mutex;
static __always_inline void
seqcount_ww_mutex_init(seqcount_t *s, struct ww_mutex *lock)
{
	seqcount_init(s);
}

static inline void _kcl_write_seqcount_begin(seqcount_t *s)
{
	preempt_disable();
	write_seqcount_begin(s);
}
#define write_seqcount_begin _kcl_write_seqcount_begin

static inline void _kcl_write_seqcount_end(seqcount_t *s)
{
	write_seqcount_end(s);
	preempt_enable();
}
#define write_seqcount_end _kcl_write_seqcount_end
#endif /* write_seqcount_begin */

#endif
