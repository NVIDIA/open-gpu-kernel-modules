/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * AppArmor security module
 *
 * This file contains AppArmor ipc mediation function definitions.
 *
 * Copyright (C) 1998-2008 Novell/SUSE
 * Copyright 2009-2017 Canonical Ltd.
 */

#ifndef __AA_IPC_H
#define __AA_IPC_H

#include <linux/sched.h>

struct aa_profile;

#define AA_PTRACE_TRACE		MAY_WRITE
#define AA_PTRACE_READ		MAY_READ
#define AA_MAY_BE_TRACED	AA_MAY_APPEND
#define AA_MAY_BE_READ		AA_MAY_CREATE
#define PTRACE_PERM_SHIFT	2

#define AA_PTRACE_PERM_MASK (AA_PTRACE_READ | AA_PTRACE_TRACE | \
			     AA_MAY_BE_READ | AA_MAY_BE_TRACED)
#define AA_SIGNAL_PERM_MASK (MAY_READ | MAY_WRITE)

#define AA_SFS_SIG_MASK "hup int quit ill trap abrt bus fpe kill usr1 " \
	"segv usr2 pipe alrm term stkflt chld cont stop stp ttin ttou urg " \
	"xcpu xfsz vtalrm prof winch io pwr sys emt lost"

int aa_may_ptrace(struct aa_label *tracer, struct aa_label *tracee,
		  u32 request);
int aa_may_signal(struct aa_label *sender, struct aa_label *target, int sig);

#endif /* __AA_IPC_H */
