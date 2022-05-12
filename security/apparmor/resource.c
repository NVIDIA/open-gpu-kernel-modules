// SPDX-License-Identifier: GPL-2.0-only
/*
 * AppArmor security module
 *
 * This file contains AppArmor resource mediation and attachment
 *
 * Copyright (C) 1998-2008 Novell/SUSE
 * Copyright 2009-2010 Canonical Ltd.
 */

#include <linux/audit.h>
#include <linux/security.h>

#include "include/audit.h"
#include "include/cred.h"
#include "include/resource.h"
#include "include/policy.h"

/*
 * Table of rlimit names: we generate it from resource.h.
 */
#include "rlim_names.h"

struct aa_sfs_entry aa_sfs_entry_rlimit[] = {
	AA_SFS_FILE_STRING("mask", AA_SFS_RLIMIT_MASK),
	{ }
};

/* audit callback for resource specific fields */
static void audit_cb(struct audit_buffer *ab, void *va)
{
	struct common_audit_data *sa = va;

	audit_log_format(ab, " rlimit=%s value=%lu",
			 rlim_names[aad(sa)->rlim.rlim], aad(sa)->rlim.max);
	if (aad(sa)->peer) {
		audit_log_format(ab, " peer=");
		aa_label_xaudit(ab, labels_ns(aad(sa)->label), aad(sa)->peer,
				FLAGS_NONE, GFP_ATOMIC);
	}
}

/**
 * audit_resource - audit setting resource limit
 * @profile: profile being enforced  (NOT NULL)
 * @resource: rlimit being auditing
 * @value: value being set
 * @error: error value
 *
 * Returns: 0 or sa->error else other error code on failure
 */
static int audit_resource(struct aa_profile *profile, unsigned int resource,
			  unsigned long value, struct aa_label *peer,
			  const char *info, int error)
{
	DEFINE_AUDIT_DATA(sa, LSM_AUDIT_DATA_NONE, OP_SETRLIMIT);

	aad(&sa)->rlim.rlim = resource;
	aad(&sa)->rlim.max = value;
	aad(&sa)->peer = peer;
	aad(&sa)->info = info;
	aad(&sa)->error = error;

	return aa_audit(AUDIT_APPARMOR_AUTO, profile, &sa, audit_cb);
}

/**
 * aa_map_resouce - map compiled policy resource to internal #
 * @resource: flattened policy resource number
 *
 * Returns: resource # for the current architecture.
 *
 * rlimit resource can vary based on architecture, map the compiled policy
 * resource # to the internal representation for the architecture.
 */
int aa_map_resource(int resource)
{
	return rlim_map[resource];
}

static int profile_setrlimit(struct aa_profile *profile, unsigned int resource,
			     struct rlimit *new_rlim)
{
	int e = 0;

	if (profile->rlimits.mask & (1 << resource) && new_rlim->rlim_max >
	    profile->rlimits.limits[resource].rlim_max)
		e = -EACCES;
	return audit_resource(profile, resource, new_rlim->rlim_max, NULL, NULL,
			      e);
}

/**
 * aa_task_setrlimit - test permission to set an rlimit
 * @label - label confining the task  (NOT NULL)
 * @task - task the resource is being set on
 * @resource - the resource being set
 * @new_rlim - the new resource limit  (NOT NULL)
 *
 * Control raising the processes hard limit.
 *
 * Returns: 0 or error code if setting resource failed
 */
int aa_task_setrlimit(struct aa_label *label, struct task_struct *task,
		      unsigned int resource, struct rlimit *new_rlim)
{
	struct aa_profile *profile;
	struct aa_label *peer;
	int error = 0;

	rcu_read_lock();
	peer = aa_get_newest_cred_label(__task_cred(task));
	rcu_read_unlock();

	/* TODO: extend resource control to handle other (non current)
	 * profiles.  AppArmor rules currently have the implicit assumption
	 * that the task is setting the resource of a task confined with
	 * the same profile or that the task setting the resource of another
	 * task has CAP_SYS_RESOURCE.
	 */

	if (label != peer &&
	    aa_capable(label, CAP_SYS_RESOURCE, CAP_OPT_NOAUDIT) != 0)
		error = fn_for_each(label, profile,
				audit_resource(profile, resource,
					       new_rlim->rlim_max, peer,
					       "cap_sys_resource", -EACCES));
	else
		error = fn_for_each_confined(label, profile,
				profile_setrlimit(profile, resource, new_rlim));
	aa_put_label(peer);

	return error;
}

/**
 * __aa_transition_rlimits - apply new profile rlimits
 * @old_l: old label on task  (NOT NULL)
 * @new_l: new label with rlimits to apply  (NOT NULL)
 */
void __aa_transition_rlimits(struct aa_label *old_l, struct aa_label *new_l)
{
	unsigned int mask = 0;
	struct rlimit *rlim, *initrlim;
	struct aa_profile *old, *new;
	struct label_it i;

	old = labels_profile(old_l);
	new = labels_profile(new_l);

	/* for any rlimits the profile controlled, reset the soft limit
	 * to the lesser of the tasks hard limit and the init tasks soft limit
	 */
	label_for_each_confined(i, old_l, old) {
		if (old->rlimits.mask) {
			int j;

			for (j = 0, mask = 1; j < RLIM_NLIMITS; j++,
				     mask <<= 1) {
				if (old->rlimits.mask & mask) {
					rlim = current->signal->rlim + j;
					initrlim = init_task.signal->rlim + j;
					rlim->rlim_cur = min(rlim->rlim_max,
							    initrlim->rlim_cur);
				}
			}
		}
	}

	/* set any new hard limits as dictated by the new profile */
	label_for_each_confined(i, new_l, new) {
		int j;

		if (!new->rlimits.mask)
			continue;
		for (j = 0, mask = 1; j < RLIM_NLIMITS; j++, mask <<= 1) {
			if (!(new->rlimits.mask & mask))
				continue;

			rlim = current->signal->rlim + j;
			rlim->rlim_max = min(rlim->rlim_max,
					     new->rlimits.limits[j].rlim_max);
			/* soft limit should not exceed hard limit */
			rlim->rlim_cur = min(rlim->rlim_cur, rlim->rlim_max);
		}
	}
}
