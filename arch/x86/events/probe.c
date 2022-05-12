// SPDX-License-Identifier: GPL-2.0
#include <linux/export.h>
#include <linux/types.h>
#include <linux/bits.h>
#include "probe.h"

static umode_t
not_visible(struct kobject *kobj, struct attribute *attr, int i)
{
	return 0;
}

/*
 * Accepts msr[] array with non populated entries as long as either
 * msr[i].msr is 0 or msr[i].grp is NULL. Note that the default sysfs
 * visibility is visible when group->is_visible callback is set.
 */
unsigned long
perf_msr_probe(struct perf_msr *msr, int cnt, bool zero, void *data)
{
	unsigned long avail = 0;
	unsigned int bit;
	u64 val;

	if (cnt >= BITS_PER_LONG)
		return 0;

	for (bit = 0; bit < cnt; bit++) {
		if (!msr[bit].no_check) {
			struct attribute_group *grp = msr[bit].grp;
			u64 mask;

			/* skip entry with no group */
			if (!grp)
				continue;

			grp->is_visible = not_visible;

			/* skip unpopulated entry */
			if (!msr[bit].msr)
				continue;

			if (msr[bit].test && !msr[bit].test(bit, data))
				continue;
			/* Virt sucks; you cannot tell if a R/O MSR is present :/ */
			if (rdmsrl_safe(msr[bit].msr, &val))
				continue;

			mask = msr[bit].mask;
			if (!mask)
				mask = ~0ULL;
			/* Disable zero counters if requested. */
			if (!zero && !(val & mask))
				continue;

			grp->is_visible = NULL;
		}
		avail |= BIT(bit);
	}

	return avail;
}
EXPORT_SYMBOL_GPL(perf_msr_probe);
