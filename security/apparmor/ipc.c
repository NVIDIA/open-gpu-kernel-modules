// SPDX-License-Identifier: GPL-2.0-only
/*
 * AppArmor security module
 *
 * This file contains AppArmor ipc mediation
 *
 * Copyright (C) 1998-2008 Novell/SUSE
 * Copyright 2009-2017 Canonical Ltd.
 */

#include <linux/gfp.h>
#include <linux/ptrace.h>

#include "include/audit.h"
#include "include/capability.h"
#include "include/cred.h"
#include "include/policy.h"
#include "include/ipc.h"
#include "include/sig_names.h"

/**
 * audit_ptrace_mask - convert mask to permission string
 * @mask: permission mask to convert
 *
 * Returns: pointer to static string
 */
static const char *audit_ptrace_mask(u32 mask)
{
	switch (mask) {
	case MAY_READ:
		return "read";
	case MAY_WRITE:
		return "trace";
	case AA_MAY_BE_READ:
		return "readby";
	case AA_MAY_BE_TRACED:
		return "tracedby";
	}
	return "";
}

/* call back to audit ptrace fields */
static void audit_ptrace_cb(struct audit_buffer *ab, void *va)
{
	struct common_audit_data *sa = va;

	if (aad(sa)->request & AA_PTRACE_PERM_MASK) {
		audit_log_format(ab, " requested_mask=\"%s\"",
				 audit_ptrace_mask(aad(sa)->request));

		if (aad(sa)->denied & AA_PTRACE_PERM_MASK) {
			audit_log_format(ab, " denied_mask=\"%s\"",
					 audit_ptrace_mask(aad(sa)->denied));
		}
	}
	audit_log_format(ab, " peer=");
	aa_label_xaudit(ab, labels_ns(aad(sa)->label), aad(sa)->peer,
			FLAGS_NONE, GFP_ATOMIC);
}

/* assumes check for PROFILE_MEDIATES is already done */
/* TODO: conditionals */
static int profile_ptrace_perm(struct aa_profile *profile,
			     struct aa_label *peer, u32 request,
			     struct common_audit_data *sa)
{
	struct aa_perms perms = { };

	aad(sa)->peer = peer;
	aa_profile_match_label(profile, peer, AA_CLASS_PTRACE, request,
			       &perms);
	aa_apply_modes_to_perms(profile, &perms);
	return aa_check_perms(profile, &perms, request, sa, audit_ptrace_cb);
}

static int profile_tracee_perm(struct aa_profile *tracee,
			       struct aa_label *tracer, u32 request,
			       struct common_audit_data *sa)
{
	if (profile_unconfined(tracee) || unconfined(tracer) ||
	    !PROFILE_MEDIATES(tracee, AA_CLASS_PTRACE))
		return 0;

	return profile_ptrace_perm(tracee, tracer, request, sa);
}

static int profile_tracer_perm(struct aa_profile *tracer,
			       struct aa_label *tracee, u32 request,
			       struct common_audit_data *sa)
{
	if (profile_unconfined(tracer))
		return 0;

	if (PROFILE_MEDIATES(tracer, AA_CLASS_PTRACE))
		return profile_ptrace_perm(tracer, tracee, request, sa);

	/* profile uses the old style capability check for ptrace */
	if (&tracer->label == tracee)
		return 0;

	aad(sa)->label = &tracer->label;
	aad(sa)->peer = tracee;
	aad(sa)->request = 0;
	aad(sa)->error = aa_capable(&tracer->label, CAP_SYS_PTRACE,
				    CAP_OPT_NONE);

	return aa_audit(AUDIT_APPARMOR_AUTO, tracer, sa, audit_ptrace_cb);
}

/**
 * aa_may_ptrace - test if tracer task can trace the tracee
 * @tracer: label of the task doing the tracing  (NOT NULL)
 * @tracee: task label to be traced
 * @request: permission request
 *
 * Returns: %0 else error code if permission denied or error
 */
int aa_may_ptrace(struct aa_label *tracer, struct aa_label *tracee,
		  u32 request)
{
	struct aa_profile *profile;
	u32 xrequest = request << PTRACE_PERM_SHIFT;
	DEFINE_AUDIT_DATA(sa, LSM_AUDIT_DATA_NONE, OP_PTRACE);

	return xcheck_labels(tracer, tracee, profile,
			profile_tracer_perm(profile, tracee, request, &sa),
			profile_tracee_perm(profile, tracer, xrequest, &sa));
}


static inline int map_signal_num(int sig)
{
	if (sig > SIGRTMAX)
		return SIGUNKNOWN;
	else if (sig >= SIGRTMIN)
		return sig - SIGRTMIN + SIGRT_BASE;
	else if (sig < MAXMAPPED_SIG)
		return sig_map[sig];
	return SIGUNKNOWN;
}

/**
 * audit_signal_mask - convert mask to permission string
 * @mask: permission mask to convert
 *
 * Returns: pointer to static string
 */
static const char *audit_signal_mask(u32 mask)
{
	if (mask & MAY_READ)
		return "receive";
	if (mask & MAY_WRITE)
		return "send";
	return "";
}

/**
 * audit_cb - call back for signal specific audit fields
 * @ab: audit_buffer  (NOT NULL)
 * @va: audit struct to audit values of  (NOT NULL)
 */
static void audit_signal_cb(struct audit_buffer *ab, void *va)
{
	struct common_audit_data *sa = va;

	if (aad(sa)->request & AA_SIGNAL_PERM_MASK) {
		audit_log_format(ab, " requested_mask=\"%s\"",
				 audit_signal_mask(aad(sa)->request));
		if (aad(sa)->denied & AA_SIGNAL_PERM_MASK) {
			audit_log_format(ab, " denied_mask=\"%s\"",
					 audit_signal_mask(aad(sa)->denied));
		}
	}
	if (aad(sa)->signal == SIGUNKNOWN)
		audit_log_format(ab, "signal=unknown(%d)",
				 aad(sa)->unmappedsig);
	else if (aad(sa)->signal < MAXMAPPED_SIGNAME)
		audit_log_format(ab, " signal=%s", sig_names[aad(sa)->signal]);
	else
		audit_log_format(ab, " signal=rtmin+%d",
				 aad(sa)->signal - SIGRT_BASE);
	audit_log_format(ab, " peer=");
	aa_label_xaudit(ab, labels_ns(aad(sa)->label), aad(sa)->peer,
			FLAGS_NONE, GFP_ATOMIC);
}

static int profile_signal_perm(struct aa_profile *profile,
			       struct aa_label *peer, u32 request,
			       struct common_audit_data *sa)
{
	struct aa_perms perms;
	unsigned int state;

	if (profile_unconfined(profile) ||
	    !PROFILE_MEDIATES(profile, AA_CLASS_SIGNAL))
		return 0;

	aad(sa)->peer = peer;
	/* TODO: secondary cache check <profile, profile, perm> */
	state = aa_dfa_next(profile->policy.dfa,
			    profile->policy.start[AA_CLASS_SIGNAL],
			    aad(sa)->signal);
	aa_label_match(profile, peer, state, false, request, &perms);
	aa_apply_modes_to_perms(profile, &perms);
	return aa_check_perms(profile, &perms, request, sa, audit_signal_cb);
}

int aa_may_signal(struct aa_label *sender, struct aa_label *target, int sig)
{
	struct aa_profile *profile;
	DEFINE_AUDIT_DATA(sa, LSM_AUDIT_DATA_NONE, OP_SIGNAL);

	aad(&sa)->signal = map_signal_num(sig);
	aad(&sa)->unmappedsig = sig;
	return xcheck_labels(sender, target, profile,
			profile_signal_perm(profile, target, MAY_WRITE, &sa),
			profile_signal_perm(profile, sender, MAY_READ, &sa));
}
