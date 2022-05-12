/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef _ASM_SECCOMP_H
#define _ASM_SECCOMP_H

#include <asm-generic/seccomp.h>

#define SECCOMP_ARCH_NATIVE		AUDIT_ARCH_CSKY
#define SECCOMP_ARCH_NATIVE_NR		NR_syscalls
#define SECCOMP_ARCH_NATIVE_NAME	"csky"

#endif /* _ASM_SECCOMP_H */
