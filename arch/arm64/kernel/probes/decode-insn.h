/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * arch/arm64/kernel/probes/decode-insn.h
 *
 * Copyright (C) 2013 Linaro Limited.
 */

#ifndef _ARM_KERNEL_KPROBES_ARM64_H
#define _ARM_KERNEL_KPROBES_ARM64_H

#include <asm/kprobes.h>

/*
 * ARM strongly recommends a limit of 128 bytes between LoadExcl and
 * StoreExcl instructions in a single thread of execution. So keep the
 * max atomic context size as 32.
 */
#define MAX_ATOMIC_CONTEXT_SIZE	(128 / sizeof(kprobe_opcode_t))

enum probe_insn {
	INSN_REJECTED,
	INSN_GOOD_NO_SLOT,
	INSN_GOOD,
};

#ifdef CONFIG_KPROBES
enum probe_insn __kprobes
arm_kprobe_decode_insn(kprobe_opcode_t *addr, struct arch_specific_insn *asi);
#endif
enum probe_insn __kprobes
arm_probe_decode_insn(probe_opcode_t insn, struct arch_probe_insn *asi);

#endif /* _ARM_KERNEL_KPROBES_ARM64_H */
