/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * SVM helper functions
 *
 * Copyright 2018 Anshuman Khandual, IBM Corporation.
 */

#ifndef _ASM_POWERPC_SVM_H
#define _ASM_POWERPC_SVM_H

#ifdef CONFIG_PPC_SVM

static inline bool is_secure_guest(void)
{
	return mfmsr() & MSR_S;
}

void __init svm_swiotlb_init(void);

void dtl_cache_ctor(void *addr);
#define get_dtl_cache_ctor()	(is_secure_guest() ? dtl_cache_ctor : NULL)

#else /* CONFIG_PPC_SVM */

static inline bool is_secure_guest(void)
{
	return false;
}

static inline void svm_swiotlb_init(void) {}

#define get_dtl_cache_ctor() NULL

#endif /* CONFIG_PPC_SVM */
#endif /* _ASM_POWERPC_SVM_H */
