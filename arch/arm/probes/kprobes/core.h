/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * arch/arm/kernel/kprobes.h
 *
 * Copyright (C) 2011 Jon Medhurst <tixy@yxit.co.uk>.
 *
 * Some contents moved here from arch/arm/include/asm/kprobes.h which is
 * Copyright (C) 2006, 2007 Motorola Inc.
 */

#ifndef _ARM_KERNEL_KPROBES_H
#define _ARM_KERNEL_KPROBES_H

#include <asm/kprobes.h>
#include "../decode.h"

/*
 * These undefined instructions must be unique and
 * reserved solely for kprobes' use.
 */
#define KPROBE_ARM_BREAKPOINT_INSTRUCTION	0x07f001f8
#define KPROBE_THUMB16_BREAKPOINT_INSTRUCTION	0xde18
#define KPROBE_THUMB32_BREAKPOINT_INSTRUCTION	0xf7f0a018

extern void kprobes_remove_breakpoint(void *addr, unsigned int insn);

enum probes_insn __kprobes
kprobe_decode_ldmstm(kprobe_opcode_t insn, struct arch_probes_insn *asi,
		const struct decode_header *h);

typedef enum probes_insn (kprobe_decode_insn_t)(probes_opcode_t,
						struct arch_probes_insn *,
						bool,
						const union decode_action *,
						const struct decode_checker *[]);

#ifdef CONFIG_THUMB2_KERNEL

extern const union decode_action kprobes_t32_actions[];
extern const union decode_action kprobes_t16_actions[];
extern const struct decode_checker *kprobes_t32_checkers[];
extern const struct decode_checker *kprobes_t16_checkers[];
#else /* !CONFIG_THUMB2_KERNEL */

extern const union decode_action kprobes_arm_actions[];
extern const struct decode_checker *kprobes_arm_checkers[];

#endif

#endif /* _ARM_KERNEL_KPROBES_H */
