// SPDX-License-Identifier: GPL-2.0-only
/*
 * AppArmor security module
 *
 * This file contains AppArmor capability mediation functions
 *
 * Copyright (C) 1998-2008 Novell/SUSE
 * Copyright 2009-2010 Canonical Ltd.
 */

#include <linux/capability.h>
#include <linux/errno.h>
#include <linux/gfp.h>
#include <linux/security.h>

#include "include/apparmor.h"
#include "include/capability.h"
#include "include/cred.h"
#include "include/policy.h"
#include "include/audit.h"

/*
 * Table of capability names: we generate it from capabilities.h.
 */
#include "capability_names.h"

struct aa_sfs_entry aa_sfs_entry_caps[] = {
	AA_SFS_FILE_STRING("mask", AA_SFS_CAPS_MASK),
	{ }
};

struct audit_cache {
	struct aa_profile *profile;
	kernel_cap_t caps;
};

static DEFINE_PER_CPU(struct audit_cache, audit_cache);

/**
 * audit_cb - call back for capability components of audit struct
 * @ab - audit buffer   (NOT NULL)
 * @va - audit struct to audit data from  (NOT NULL)
 */
static void audit_cb(struct audit_buffer *ab, void *va)
{
	struct common_audit_data *sa = va;

	audit_log_format(ab, " capname=");
	audit_log_untrustedstring(ab, capability_names[sa->u.cap]);
}

/**
 * audit_caps - audit a capability
 * @sa: audit data
 * @profile: profile being tested for confinement (NOT NULL)
 * @cap: capability tested
 * @error: error code returned by test
 *
 * Do auditing of capability and handle, audit/complain/kill modes switching
 * and duplicate message elimination.
 *
 * Returns: 0 or sa->error on success,  error code on failure
 */
static int audit_caps(struct common_audit_data *sa, struct aa_profile *profile,
		      int cap, int error)
{
	struct audit_cache *ent;
	int type = AUDIT_APPARMOR_AUTO;

	aad(sa)->error = error;

	if (likely(!error)) {
		/* test if auditing is being forced */
		if (likely((AUDIT_MODE(profile) != AUDIT_ALL) &&
			   !cap_raised(profile->caps.audit, cap)))
			return 0;
		type = AUDIT_APPARMOR_AUDIT;
	} else if (KILL_MODE(profile) ||
		   cap_raised(profile->caps.kill, cap)) {
		type = AUDIT_APPARMOR_KILL;
	} else if (cap_raised(profile->caps.quiet, cap) &&
		   AUDIT_MODE(profile) != AUDIT_NOQUIET &&
		   AUDIT_MODE(profile) != AUDIT_ALL) {
		/* quiet auditing */
		return error;
	}

	/* Do simple duplicate message elimination */
	ent = &get_cpu_var(audit_cache);
	if (profile == ent->profile && cap_raised(ent->caps, cap)) {
		put_cpu_var(audit_cache);
		if (COMPLAIN_MODE(profile))
			return complain_error(error);
		return error;
	} else {
		aa_put_profile(ent->profile);
		ent->profile = aa_get_profile(profile);
		cap_raise(ent->caps, cap);
	}
	put_cpu_var(audit_cache);

	return aa_audit(type, profile, sa, audit_cb);
}

/**
 * profile_capable - test if profile allows use of capability @cap
 * @profile: profile being enforced    (NOT NULL, NOT unconfined)
 * @cap: capability to test if allowed
 * @opts: CAP_OPT_NOAUDIT bit determines whether audit record is generated
 * @sa: audit data (MAY BE NULL indicating no auditing)
 *
 * Returns: 0 if allowed else -EPERM
 */
static int profile_capable(struct aa_profile *profile, int cap,
			   unsigned int opts, struct common_audit_data *sa)
{
	int error;

	if (cap_raised(profile->caps.allow, cap) &&
	    !cap_raised(profile->caps.denied, cap))
		error = 0;
	else
		error = -EPERM;

	if (opts & CAP_OPT_NOAUDIT) {
		if (!COMPLAIN_MODE(profile))
			return error;
		/* audit the cap request in complain mode but note that it
		 * should be optional.
		 */
		aad(sa)->info = "optional: no audit";
	}

	return audit_caps(sa, profile, cap, error);
}

/**
 * aa_capable - test permission to use capability
 * @label: label being tested for capability (NOT NULL)
 * @cap: capability to be tested
 * @opts: CAP_OPT_NOAUDIT bit determines whether audit record is generated
 *
 * Look up capability in profile capability set.
 *
 * Returns: 0 on success, or else an error code.
 */
int aa_capable(struct aa_label *label, int cap, unsigned int opts)
{
	struct aa_profile *profile;
	int error = 0;
	DEFINE_AUDIT_DATA(sa, LSM_AUDIT_DATA_CAP, OP_CAPABLE);

	sa.u.cap = cap;
	error = fn_for_each_confined(label, profile,
			profile_capable(profile, cap, opts, &sa));

	return error;
}
