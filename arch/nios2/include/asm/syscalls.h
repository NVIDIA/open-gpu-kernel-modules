/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright Altera Corporation (C) 2013. All rights reserved
 */
#ifndef __ASM_NIOS2_SYSCALLS_H
#define __ASM_NIOS2_SYSCALLS_H

int sys_cacheflush(unsigned long addr, unsigned long len,
				unsigned int op);

#include <asm-generic/syscalls.h>

#endif /* __ASM_NIOS2_SYSCALLS_H */
