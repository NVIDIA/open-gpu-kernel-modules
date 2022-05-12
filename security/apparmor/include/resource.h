/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * AppArmor security module
 *
 * This file contains AppArmor resource limits function definitions.
 *
 * Copyright (C) 1998-2008 Novell/SUSE
 * Copyright 2009-2010 Canonical Ltd.
 */

#ifndef __AA_RESOURCE_H
#define __AA_RESOURCE_H

#include <linux/resource.h>
#include <linux/sched.h>

#include "apparmorfs.h"

struct aa_profile;

/* struct aa_rlimit - rlimit settings for the profile
 * @mask: which hard limits to set
 * @limits: rlimit values that override task limits
 *
 * AppArmor rlimits are used to set confined task rlimits.  Only the
 * limits specified in @mask will be controlled by apparmor.
 */
struct aa_rlimit {
	unsigned int mask;
	struct rlimit limits[RLIM_NLIMITS];
};

extern struct aa_sfs_entry aa_sfs_entry_rlimit[];

int aa_map_resource(int resource);
int aa_task_setrlimit(struct aa_label *label, struct task_struct *task,
		      unsigned int resource, struct rlimit *new_rlim);

void __aa_transition_rlimits(struct aa_label *old, struct aa_label *new);

static inline void aa_free_rlimit_rules(struct aa_rlimit *rlims)
{
	/* NOP */
}

#endif /* __AA_RESOURCE_H */
