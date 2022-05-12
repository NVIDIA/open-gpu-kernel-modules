/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright 2013 Red Hat Inc.
 *
 * Authors: Jérôme Glisse <jglisse@redhat.com>
 *
 * See Documentation/vm/hmm.rst for reasons and overview of what HMM is.
 */
#ifndef _KCL_BACKPORT_KCL_HMM_H
#define _KCL_BACKPORT_KCL_HMM_H

#ifdef HAVE_AMDKCL_HMM_MIRROR_ENABLED
#include <linux/hmm.h>

#ifndef HAVE_HMM_RANGE_FAULT_1ARG
static inline
int _kcl_hmm_range_fault(struct hmm_range *range)
{
	return hmm_range_fault(range, 0);
}
#define hmm_range_fault _kcl_hmm_range_fault
#endif /* HAVE_HMM_RANGE_FAULT_1ARG */

#endif /* HAVE_AMDKCL_HMM_MIRROR_ENABLED */
#endif
