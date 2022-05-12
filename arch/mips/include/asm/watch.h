/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2008 David Daney
 */
#ifndef _ASM_WATCH_H
#define _ASM_WATCH_H

#include <linux/bitops.h>

#include <asm/mipsregs.h>

void mips_install_watch_registers(struct task_struct *t);
void mips_read_watch_registers(void);
void mips_clear_watch_registers(void);
void mips_probe_watch_registers(struct cpuinfo_mips *c);

#ifdef CONFIG_HARDWARE_WATCHPOINTS
#define __restore_watch(task) do {					\
	if (unlikely(test_bit(TIF_LOAD_WATCH,				\
			      &task_thread_info(task)->flags))) {	\
		mips_install_watch_registers(task);			\
	}								\
} while (0)

#else
#define __restore_watch(task) do {} while (0)
#endif

#endif /* _ASM_WATCH_H */
