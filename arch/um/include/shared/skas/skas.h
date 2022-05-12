/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2002 - 2007 Jeff Dike (jdike@{addtoit,linux.intel}.com)
 */

#ifndef __SKAS_H
#define __SKAS_H

#include <sysdep/ptrace.h>

extern int userspace_pid[];

extern int user_thread(unsigned long stack, int flags);
extern void new_thread_handler(void);
extern void handle_syscall(struct uml_pt_regs *regs);
extern long execute_syscall_skas(void *r);
extern unsigned long current_stub_stack(void);

#endif
