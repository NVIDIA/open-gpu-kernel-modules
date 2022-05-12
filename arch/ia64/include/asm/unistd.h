/* SPDX-License-Identifier: GPL-2.0 */
/*
 * IA-64 Linux syscall numbers and inline-functions.
 *
 * Copyright (C) 1998-2005 Hewlett-Packard Co
 *	David Mosberger-Tang <davidm@hpl.hp.com>
 */
#ifndef _ASM_IA64_UNISTD_H
#define _ASM_IA64_UNISTD_H

#include <uapi/asm/unistd.h>

#define NR_syscalls		__NR_syscalls /* length of syscall table */

#define __ARCH_WANT_NEW_STAT
#define __ARCH_WANT_SYS_UTIME

#if !defined(__ASSEMBLY__) && !defined(ASSEMBLER)

#include <linux/types.h>
#include <linux/linkage.h>
#include <linux/compiler.h>

extern long __ia64_syscall (long a0, long a1, long a2, long a3, long a4, long nr);

asmlinkage unsigned long sys_mmap(
				unsigned long addr, unsigned long len,
				int prot, int flags,
				int fd, long off);
asmlinkage unsigned long sys_mmap2(
				unsigned long addr, unsigned long len,
				int prot, int flags,
				int fd, long pgoff);
struct pt_regs;
asmlinkage long sys_ia64_pipe(void);

#endif /* !__ASSEMBLY__ */
#endif /* _ASM_IA64_UNISTD_H */
