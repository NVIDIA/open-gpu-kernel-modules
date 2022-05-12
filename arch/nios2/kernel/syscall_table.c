// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright Altera Corporation (C) 2013. All rights reserved
 */

#include <linux/syscalls.h>
#include <linux/signal.h>
#include <linux/unistd.h>

#include <asm/syscalls.h>

#undef __SYSCALL
#define __SYSCALL(nr, call) [nr] = (call),

void *sys_call_table[__NR_syscalls] = {
#include <asm/unistd.h>
};
