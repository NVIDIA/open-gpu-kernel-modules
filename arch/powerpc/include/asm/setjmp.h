/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright © 2008 Michael Neuling IBM Corporation
 */
#ifndef _ASM_POWERPC_SETJMP_H
#define _ASM_POWERPC_SETJMP_H

#define JMP_BUF_LEN    23

typedef long jmp_buf[JMP_BUF_LEN];

extern int setjmp(jmp_buf env) __attribute__((returns_twice));
extern void longjmp(jmp_buf env, int val) __attribute__((noreturn));

#endif /* _ASM_POWERPC_SETJMP_H */
