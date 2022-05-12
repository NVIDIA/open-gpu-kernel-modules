// SPDX-License-Identifier: GPL-2.0-only
/*
 *
 * arch/arm/probes/decode-arm.c
 *
 * Some code moved here from arch/arm/kernel/kprobes-arm.c
 *
 * Copyright (C) 2006, 2007 Motorola Inc.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/stddef.h>
#include <linux/ptrace.h>

#include "decode.h"
#include "decode-arm.h"

#define sign_extend(x, signbit) ((x) | (0 - ((x) & (1 << (signbit)))))

#define branch_displacement(insn) sign_extend(((insn) & 0xffffff) << 2, 25)

/*
 * To avoid the complications of mimicing single-stepping on a
 * processor without a Next-PC or a single-step mode, and to
 * avoid having to deal with the side-effects of boosting, we
 * simulate or emulate (almost) all ARM instructions.
 *
 * "Simulation" is where the instruction's behavior is duplicated in
 * C code.  "Emulation" is where the original instruction is rewritten
 * and executed, often by altering its registers.
 *
 * By having all behavior of the kprobe'd instruction completed before
 * returning from the kprobe_handler(), all locks (scheduler and
 * interrupt) can safely be released.  There is no need for secondary
 * breakpoints, no race with MP or preemptable kernels, nor having to
 * clean up resources counts at a later time impacting overall system
 * performance.  By rewriting the instruction, only the minimum registers
 * need to be loaded and saved back optimizing performance.
 *
 * Calling the insnslot_*_rwflags version of a function doesn't hurt
 * anything even when the CPSR flags aren't updated by the
 * instruction.  It's just a little slower in return for saving
 * a little space by not having a duplicate function that doesn't
 * update the flags.  (The same optimization can be said for
 * instructions that do or don't perform register writeback)
 * Also, instructions can either read the flags, only write the
 * flags, or read and write the flags.  To save combinations
 * rather than for sheer performance, flag functions just assume
 * read and write of flags.
 */

void __kprobes simulate_bbl(probes_opcode_t insn,
		struct arch_probes_insn *asi, struct pt_regs *regs)
{
	long iaddr = (long) regs->ARM_pc - 4;
	int disp  = branch_displacement(insn);

	if (insn & (1 << 24))
		regs->ARM_lr = iaddr + 4;

	regs->ARM_pc = iaddr + 8 + disp;
}

void __kprobes simulate_blx1(probes_opcode_t insn,
		struct arch_probes_insn *asi, struct pt_regs *regs)
{
	long iaddr = (long) regs->ARM_pc - 4;
	int disp = branch_displacement(insn);

	regs->ARM_lr = iaddr + 4;
	regs->ARM_pc = iaddr + 8 + disp + ((insn >> 23) & 0x2);
	regs->ARM_cpsr |= PSR_T_BIT;
}

void __kprobes simulate_blx2bx(probes_opcode_t insn,
		struct arch_probes_insn *asi, struct pt_regs *regs)
{
	int rm = insn & 0xf;
	long rmv = regs->uregs[rm];

	if (insn & (1 << 5))
		regs->ARM_lr = (long) regs->ARM_pc;

	regs->ARM_pc = rmv & ~0x1;
	regs->ARM_cpsr &= ~PSR_T_BIT;
	if (rmv & 0x1)
		regs->ARM_cpsr |= PSR_T_BIT;
}

void __kprobes simulate_mrs(probes_opcode_t insn,
		struct arch_probes_insn *asi, struct pt_regs *regs)
{
	int rd = (insn >> 12) & 0xf;
	unsigned long mask = 0xf8ff03df; /* Mask out execution state */
	regs->uregs[rd] = regs->ARM_cpsr & mask;
}

void __kprobes simulate_mov_ipsp(probes_opcode_t insn,
		struct arch_probes_insn *asi, struct pt_regs *regs)
{
	regs->uregs[12] = regs->uregs[13];
}

/*
 * For the instruction masking and comparisons in all the "space_*"
 * functions below, Do _not_ rearrange the order of tests unless
 * you're very, very sure of what you are doing.  For the sake of
 * efficiency, the masks for some tests sometimes assume other test
 * have been done prior to them so the number of patterns to test
 * for an instruction set can be as broad as possible to reduce the
 * number of tests needed.
 */

static const union decode_item arm_1111_table[] = {
	/* Unconditional instructions					*/

	/* memory hint		1111 0100 x001 xxxx xxxx xxxx xxxx xxxx */
	/* PLDI (immediate)	1111 0100 x101 xxxx xxxx xxxx xxxx xxxx */
	/* PLDW (immediate)	1111 0101 x001 xxxx xxxx xxxx xxxx xxxx */
	/* PLD (immediate)	1111 0101 x101 xxxx xxxx xxxx xxxx xxxx */
	DECODE_SIMULATE	(0xfe300000, 0xf4100000, PROBES_PRELOAD_IMM),

	/* memory hint		1111 0110 x001 xxxx xxxx xxxx xxx0 xxxx */
	/* PLDI (register)	1111 0110 x101 xxxx xxxx xxxx xxx0 xxxx */
	/* PLDW (register)	1111 0111 x001 xxxx xxxx xxxx xxx0 xxxx */
	/* PLD (register)	1111 0111 x101 xxxx xxxx xxxx xxx0 xxxx */
	DECODE_SIMULATE	(0xfe300010, 0xf6100000, PROBES_PRELOAD_REG),

	/* BLX (immediate)	1111 101x xxxx xxxx xxxx xxxx xxxx xxxx */
	DECODE_SIMULATE	(0xfe000000, 0xfa000000, PROBES_BRANCH_IMM),

	/* CPS			1111 0001 0000 xxx0 xxxx xxxx xx0x xxxx */
	/* SETEND		1111 0001 0000 0001 xxxx xxxx 0000 xxxx */
	/* SRS			1111 100x x1x0 xxxx xxxx xxxx xxxx xxxx */
	/* RFE			1111 100x x0x1 xxxx xxxx xxxx xxxx xxxx */

	/* Coprocessor instructions... */
	/* MCRR2		1111 1100 0100 xxxx xxxx xxxx xxxx xxxx */
	/* MRRC2		1111 1100 0101 xxxx xxxx xxxx xxxx xxxx */
	/* LDC2			1111 110x xxx1 xxxx xxxx xxxx xxxx xxxx */
	/* STC2			1111 110x xxx0 xxxx xxxx xxxx xxxx xxxx */
	/* CDP2			1111 1110 xxxx xxxx xxxx xxxx xxx0 xxxx */
	/* MCR2			1111 1110 xxx0 xxxx xxxx xxxx xxx1 xxxx */
	/* MRC2			1111 1110 xxx1 xxxx xxxx xxxx xxx1 xxxx */

	/* Other unallocated instructions...				*/
	DECODE_END
};

static const union decode_item arm_cccc_0001_0xx0____0xxx_table[] = {
	/* Miscellaneous instructions					*/

	/* MRS cpsr		cccc 0001 0000 xxxx xxxx xxxx 0000 xxxx */
	DECODE_SIMULATEX(0x0ff000f0, 0x01000000, PROBES_MRS,
						 REGS(0, NOPC, 0, 0, 0)),

	/* BX			cccc 0001 0010 xxxx xxxx xxxx 0001 xxxx */
	DECODE_SIMULATE	(0x0ff000f0, 0x01200010, PROBES_BRANCH_REG),

	/* BLX (register)	cccc 0001 0010 xxxx xxxx xxxx 0011 xxxx */
	DECODE_SIMULATEX(0x0ff000f0, 0x01200030, PROBES_BRANCH_REG,
						 REGS(0, 0, 0, 0, NOPC)),

	/* CLZ			cccc 0001 0110 xxxx xxxx xxxx 0001 xxxx */
	DECODE_EMULATEX	(0x0ff000f0, 0x01600010, PROBES_CLZ,
						 REGS(0, NOPC, 0, 0, NOPC)),

	/* QADD			cccc 0001 0000 xxxx xxxx xxxx 0101 xxxx */
	/* QSUB			cccc 0001 0010 xxxx xxxx xxxx 0101 xxxx */
	/* QDADD		cccc 0001 0100 xxxx xxxx xxxx 0101 xxxx */
	/* QDSUB		cccc 0001 0110 xxxx xxxx xxxx 0101 xxxx */
	DECODE_EMULATEX	(0x0f9000f0, 0x01000050, PROBES_SATURATING_ARITHMETIC,
						 REGS(NOPC, NOPC, 0, 0, NOPC)),

	/* BXJ			cccc 0001 0010 xxxx xxxx xxxx 0010 xxxx */
	/* MSR			cccc 0001 0x10 xxxx xxxx xxxx 0000 xxxx */
	/* MRS spsr		cccc 0001 0100 xxxx xxxx xxxx 0000 xxxx */
	/* BKPT			1110 0001 0010 xxxx xxxx xxxx 0111 xxxx */
	/* SMC			cccc 0001 0110 xxxx xxxx xxxx 0111 xxxx */
	/* And unallocated instructions...				*/
	DECODE_END
};

static const union decode_item arm_cccc_0001_0xx0____1xx0_table[] = {
	/* Halfword multiply and multiply-accumulate			*/

	/* SMLALxy		cccc 0001 0100 xxxx xxxx xxxx 1xx0 xxxx */
	DECODE_EMULATEX	(0x0ff00090, 0x01400080, PROBES_MUL1,
						 REGS(NOPC, NOPC, NOPC, 0, NOPC)),

	/* SMULWy		cccc 0001 0010 xxxx xxxx xxxx 1x10 xxxx */
	DECODE_OR	(0x0ff000b0, 0x012000a0),
	/* SMULxy		cccc 0001 0110 xxxx xxxx xxxx 1xx0 xxxx */
	DECODE_EMULATEX	(0x0ff00090, 0x01600080, PROBES_MUL2,
						 REGS(NOPC, 0, NOPC, 0, NOPC)),

	/* SMLAxy		cccc 0001 0000 xxxx xxxx xxxx 1xx0 xxxx */
	DECODE_OR	(0x0ff00090, 0x01000080),
	/* SMLAWy		cccc 0001 0010 xxxx xxxx xxxx 1x00 xxxx */
	DECODE_EMULATEX	(0x0ff000b0, 0x01200080, PROBES_MUL2,
						 REGS(NOPC, NOPC, NOPC, 0, NOPC)),

	DECODE_END
};

static const union decode_item arm_cccc_0000_____1001_table[] = {
	/* Multiply and multiply-accumulate				*/

	/* MUL			cccc 0000 0000 xxxx xxxx xxxx 1001 xxxx */
	/* MULS			cccc 0000 0001 xxxx xxxx xxxx 1001 xxxx */
	DECODE_EMULATEX	(0x0fe000f0, 0x00000090, PROBES_MUL2,
						 REGS(NOPC, 0, NOPC, 0, NOPC)),

	/* MLA			cccc 0000 0010 xxxx xxxx xxxx 1001 xxxx */
	/* MLAS			cccc 0000 0011 xxxx xxxx xxxx 1001 xxxx */
	DECODE_OR	(0x0fe000f0, 0x00200090),
	/* MLS			cccc 0000 0110 xxxx xxxx xxxx 1001 xxxx */
	DECODE_EMULATEX	(0x0ff000f0, 0x00600090, PROBES_MUL2,
						 REGS(NOPC, NOPC, NOPC, 0, NOPC)),

	/* UMAAL		cccc 0000 0100 xxxx xxxx xxxx 1001 xxxx */
	DECODE_OR	(0x0ff000f0, 0x00400090),
	/* UMULL		cccc 0000 1000 xxxx xxxx xxxx 1001 xxxx */
	/* UMULLS		cccc 0000 1001 xxxx xxxx xxxx 1001 xxxx */
	/* UMLAL		cccc 0000 1010 xxxx xxxx xxxx 1001 xxxx */
	/* UMLALS		cccc 0000 1011 xxxx xxxx xxxx 1001 xxxx */
	/* SMULL		cccc 0000 1100 xxxx xxxx xxxx 1001 xxxx */
	/* SMULLS		cccc 0000 1101 xxxx xxxx xxxx 1001 xxxx */
	/* SMLAL		cccc 0000 1110 xxxx xxxx xxxx 1001 xxxx */
	/* SMLALS		cccc 0000 1111 xxxx xxxx xxxx 1001 xxxx */
	DECODE_EMULATEX	(0x0f8000f0, 0x00800090, PROBES_MUL1,
						 REGS(NOPC, NOPC, NOPC, 0, NOPC)),

	DECODE_END
};

static const union decode_item arm_cccc_0001_____1001_table[] = {
	/* Synchronization primitives					*/

#if __LINUX_ARM_ARCH__ < 6
	/* Deprecated on ARMv6 and may be UNDEFINED on v7		*/
	/* SMP/SWPB		cccc 0001 0x00 xxxx xxxx xxxx 1001 xxxx */
	DECODE_EMULATEX	(0x0fb000f0, 0x01000090, PROBES_SWP,
						 REGS(NOPC, NOPC, 0, 0, NOPC)),
#endif
	/* LDREX/STREX{,D,B,H}	cccc 0001 1xxx xxxx xxxx xxxx 1001 xxxx */
	/* And unallocated instructions...				*/
	DECODE_END
};

static const union decode_item arm_cccc_000x_____1xx1_table[] = {
	/* Extra load/store instructions				*/

	/* STRHT		cccc 0000 xx10 xxxx xxxx xxxx 1011 xxxx */
	/* ???			cccc 0000 xx10 xxxx xxxx xxxx 11x1 xxxx */
	/* LDRHT		cccc 0000 xx11 xxxx xxxx xxxx 1011 xxxx */
	/* LDRSBT		cccc 0000 xx11 xxxx xxxx xxxx 1101 xxxx */
	/* LDRSHT		cccc 0000 xx11 xxxx xxxx xxxx 1111 xxxx */
	DECODE_REJECT	(0x0f200090, 0x00200090),

	/* LDRD/STRD lr,pc,{...	cccc 000x x0x0 xxxx 111x xxxx 1101 xxxx */
	DECODE_REJECT	(0x0e10e0d0, 0x0000e0d0),

	/* LDRD (register)	cccc 000x x0x0 xxxx xxxx xxxx 1101 xxxx */
	/* STRD (register)	cccc 000x x0x0 xxxx xxxx xxxx 1111 xxxx */
	DECODE_EMULATEX	(0x0e5000d0, 0x000000d0, PROBES_LDRSTRD,
						 REGS(NOPCWB, NOPCX, 0, 0, NOPC)),

	/* LDRD (immediate)	cccc 000x x1x0 xxxx xxxx xxxx 1101 xxxx */
	/* STRD (immediate)	cccc 000x x1x0 xxxx xxxx xxxx 1111 xxxx */
	DECODE_EMULATEX	(0x0e5000d0, 0x004000d0, PROBES_LDRSTRD,
						 REGS(NOPCWB, NOPCX, 0, 0, 0)),

	/* STRH (register)	cccc 000x x0x0 xxxx xxxx xxxx 1011 xxxx */
	DECODE_EMULATEX	(0x0e5000f0, 0x000000b0, PROBES_STORE_EXTRA,
						 REGS(NOPCWB, NOPC, 0, 0, NOPC)),

	/* LDRH (register)	cccc 000x x0x1 xxxx xxxx xxxx 1011 xxxx */
	/* LDRSB (register)	cccc 000x x0x1 xxxx xxxx xxxx 1101 xxxx */
	/* LDRSH (register)	cccc 000x x0x1 xxxx xxxx xxxx 1111 xxxx */
	DECODE_EMULATEX	(0x0e500090, 0x00100090, PROBES_LOAD_EXTRA,
						 REGS(NOPCWB, NOPC, 0, 0, NOPC)),

	/* STRH (immediate)	cccc 000x x1x0 xxxx xxxx xxxx 1011 xxxx */
	DECODE_EMULATEX	(0x0e5000f0, 0x004000b0, PROBES_STORE_EXTRA,
						 REGS(NOPCWB, NOPC, 0, 0, 0)),

	/* LDRH (immediate)	cccc 000x x1x1 xxxx xxxx xxxx 1011 xxxx */
	/* LDRSB (immediate)	cccc 000x x1x1 xxxx xxxx xxxx 1101 xxxx */
	/* LDRSH (immediate)	cccc 000x x1x1 xxxx xxxx xxxx 1111 xxxx */
	DECODE_EMULATEX	(0x0e500090, 0x00500090, PROBES_LOAD_EXTRA,
						 REGS(NOPCWB, NOPC, 0, 0, 0)),

	DECODE_END
};

static const union decode_item arm_cccc_000x_table[] = {
	/* Data-processing (register)					*/

	/* <op>S PC, ...	cccc 000x xxx1 xxxx 1111 xxxx xxxx xxxx */
	DECODE_REJECT	(0x0e10f000, 0x0010f000),

	/* MOV IP, SP		1110 0001 1010 0000 1100 0000 0000 1101 */
	DECODE_SIMULATE	(0xffffffff, 0xe1a0c00d, PROBES_MOV_IP_SP),

	/* TST (register)	cccc 0001 0001 xxxx xxxx xxxx xxx0 xxxx */
	/* TEQ (register)	cccc 0001 0011 xxxx xxxx xxxx xxx0 xxxx */
	/* CMP (register)	cccc 0001 0101 xxxx xxxx xxxx xxx0 xxxx */
	/* CMN (register)	cccc 0001 0111 xxxx xxxx xxxx xxx0 xxxx */
	DECODE_EMULATEX	(0x0f900010, 0x01100000, PROBES_DATA_PROCESSING_REG,
						 REGS(ANY, 0, 0, 0, ANY)),

	/* MOV (register)	cccc 0001 101x xxxx xxxx xxxx xxx0 xxxx */
	/* MVN (register)	cccc 0001 111x xxxx xxxx xxxx xxx0 xxxx */
	DECODE_EMULATEX	(0x0fa00010, 0x01a00000, PROBES_DATA_PROCESSING_REG,
						 REGS(0, ANY, 0, 0, ANY)),

	/* AND (register)	cccc 0000 000x xxxx xxxx xxxx xxx0 xxxx */
	/* EOR (register)	cccc 0000 001x xxxx xxxx xxxx xxx0 xxxx */
	/* SUB (register)	cccc 0000 010x xxxx xxxx xxxx xxx0 xxxx */
	/* RSB (register)	cccc 0000 011x xxxx xxxx xxxx xxx0 xxxx */
	/* ADD (register)	cccc 0000 100x xxxx xxxx xxxx xxx0 xxxx */
	/* ADC (register)	cccc 0000 101x xxxx xxxx xxxx xxx0 xxxx */
	/* SBC (register)	cccc 0000 110x xxxx xxxx xxxx xxx0 xxxx */
	/* RSC (register)	cccc 0000 111x xxxx xxxx xxxx xxx0 xxxx */
	/* ORR (register)	cccc 0001 100x xxxx xxxx xxxx xxx0 xxxx */
	/* BIC (register)	cccc 0001 110x xxxx xxxx xxxx xxx0 xxxx */
	DECODE_EMULATEX	(0x0e000010, 0x00000000, PROBES_DATA_PROCESSING_REG,
						 REGS(ANY, ANY, 0, 0, ANY)),

	/* TST (reg-shift reg)	cccc 0001 0001 xxxx xxxx xxxx 0xx1 xxxx */
	/* TEQ (reg-shift reg)	cccc 0001 0011 xxxx xxxx xxxx 0xx1 xxxx */
	/* CMP (reg-shift reg)	cccc 0001 0101 xxxx xxxx xxxx 0xx1 xxxx */
	/* CMN (reg-shift reg)	cccc 0001 0111 xxxx xxxx xxxx 0xx1 xxxx */
	DECODE_EMULATEX	(0x0f900090, 0x01100010, PROBES_DATA_PROCESSING_REG,
						 REGS(NOPC, 0, NOPC, 0, NOPC)),

	/* MOV (reg-shift reg)	cccc 0001 101x xxxx xxxx xxxx 0xx1 xxxx */
	/* MVN (reg-shift reg)	cccc 0001 111x xxxx xxxx xxxx 0xx1 xxxx */
	DECODE_EMULATEX	(0x0fa00090, 0x01a00010, PROBES_DATA_PROCESSING_REG,
						 REGS(0, NOPC, NOPC, 0, NOPC)),

	/* AND (reg-shift reg)	cccc 0000 000x xxxx xxxx xxxx 0xx1 xxxx */
	/* EOR (reg-shift reg)	cccc 0000 001x xxxx xxxx xxxx 0xx1 xxxx */
	/* SUB (reg-shift reg)	cccc 0000 010x xxxx xxxx xxxx 0xx1 xxxx */
	/* RSB (reg-shift reg)	cccc 0000 011x xxxx xxxx xxxx 0xx1 xxxx */
	/* ADD (reg-shift reg)	cccc 0000 100x xxxx xxxx xxxx 0xx1 xxxx */
	/* ADC (reg-shift reg)	cccc 0000 101x xxxx xxxx xxxx 0xx1 xxxx */
	/* SBC (reg-shift reg)	cccc 0000 110x xxxx xxxx xxxx 0xx1 xxxx */
	/* RSC (reg-shift reg)	cccc 0000 111x xxxx xxxx xxxx 0xx1 xxxx */
	/* ORR (reg-shift reg)	cccc 0001 100x xxxx xxxx xxxx 0xx1 xxxx */
	/* BIC (reg-shift reg)	cccc 0001 110x xxxx xxxx xxxx 0xx1 xxxx */
	DECODE_EMULATEX	(0x0e000090, 0x00000010, PROBES_DATA_PROCESSING_REG,
						 REGS(NOPC, NOPC, NOPC, 0, NOPC)),

	DECODE_END
};

static const union decode_item arm_cccc_001x_table[] = {
	/* Data-processing (immediate)					*/

	/* MOVW			cccc 0011 0000 xxxx xxxx xxxx xxxx xxxx */
	/* MOVT			cccc 0011 0100 xxxx xxxx xxxx xxxx xxxx */
	DECODE_EMULATEX	(0x0fb00000, 0x03000000, PROBES_MOV_HALFWORD,
						 REGS(0, NOPC, 0, 0, 0)),

	/* YIELD		cccc 0011 0010 0000 xxxx xxxx 0000 0001 */
	DECODE_OR	(0x0fff00ff, 0x03200001),
	/* SEV			cccc 0011 0010 0000 xxxx xxxx 0000 0100 */
	DECODE_EMULATE	(0x0fff00ff, 0x03200004, PROBES_SEV),
	/* NOP			cccc 0011 0010 0000 xxxx xxxx 0000 0000 */
	/* WFE			cccc 0011 0010 0000 xxxx xxxx 0000 0010 */
	/* WFI			cccc 0011 0010 0000 xxxx xxxx 0000 0011 */
	DECODE_SIMULATE	(0x0fff00fc, 0x03200000, PROBES_WFE),
	/* DBG			cccc 0011 0010 0000 xxxx xxxx ffff xxxx */
	/* unallocated hints	cccc 0011 0010 0000 xxxx xxxx xxxx xxxx */
	/* MSR (immediate)	cccc 0011 0x10 xxxx xxxx xxxx xxxx xxxx */
	DECODE_REJECT	(0x0fb00000, 0x03200000),

	/* <op>S PC, ...	cccc 001x xxx1 xxxx 1111 xxxx xxxx xxxx */
	DECODE_REJECT	(0x0e10f000, 0x0210f000),

	/* TST (immediate)	cccc 0011 0001 xxxx xxxx xxxx xxxx xxxx */
	/* TEQ (immediate)	cccc 0011 0011 xxxx xxxx xxxx xxxx xxxx */
	/* CMP (immediate)	cccc 0011 0101 xxxx xxxx xxxx xxxx xxxx */
	/* CMN (immediate)	cccc 0011 0111 xxxx xxxx xxxx xxxx xxxx */
	DECODE_EMULATEX	(0x0f900000, 0x03100000, PROBES_DATA_PROCESSING_IMM,
						 REGS(ANY, 0, 0, 0, 0)),

	/* MOV (immediate)	cccc 0011 101x xxxx xxxx xxxx xxxx xxxx */
	/* MVN (immediate)	cccc 0011 111x xxxx xxxx xxxx xxxx xxxx */
	DECODE_EMULATEX	(0x0fa00000, 0x03a00000, PROBES_DATA_PROCESSING_IMM,
						 REGS(0, ANY, 0, 0, 0)),

	/* AND (immediate)	cccc 0010 000x xxxx xxxx xxxx xxxx xxxx */
	/* EOR (immediate)	cccc 0010 001x xxxx xxxx xxxx xxxx xxxx */
	/* SUB (immediate)	cccc 0010 010x xxxx xxxx xxxx xxxx xxxx */
	/* RSB (immediate)	cccc 0010 011x xxxx xxxx xxxx xxxx xxxx */
	/* ADD (immediate)	cccc 0010 100x xxxx xxxx xxxx xxxx xxxx */
	/* ADC (immediate)	cccc 0010 101x xxxx xxxx xxxx xxxx xxxx */
	/* SBC (immediate)	cccc 0010 110x xxxx xxxx xxxx xxxx xxxx */
	/* RSC (immediate)	cccc 0010 111x xxxx xxxx xxxx xxxx xxxx */
	/* ORR (immediate)	cccc 0011 100x xxxx xxxx xxxx xxxx xxxx */
	/* BIC (immediate)	cccc 0011 110x xxxx xxxx xxxx xxxx xxxx */
	DECODE_EMULATEX	(0x0e000000, 0x02000000, PROBES_DATA_PROCESSING_IMM,
						 REGS(ANY, ANY, 0, 0, 0)),

	DECODE_END
};

static const union decode_item arm_cccc_0110_____xxx1_table[] = {
	/* Media instructions						*/

	/* SEL			cccc 0110 1000 xxxx xxxx xxxx 1011 xxxx */
	DECODE_EMULATEX	(0x0ff000f0, 0x068000b0, PROBES_SATURATE,
						 REGS(NOPC, NOPC, 0, 0, NOPC)),

	/* SSAT			cccc 0110 101x xxxx xxxx xxxx xx01 xxxx */
	/* USAT			cccc 0110 111x xxxx xxxx xxxx xx01 xxxx */
	DECODE_OR(0x0fa00030, 0x06a00010),
	/* SSAT16		cccc 0110 1010 xxxx xxxx xxxx 0011 xxxx */
	/* USAT16		cccc 0110 1110 xxxx xxxx xxxx 0011 xxxx */
	DECODE_EMULATEX	(0x0fb000f0, 0x06a00030, PROBES_SATURATE,
						 REGS(0, NOPC, 0, 0, NOPC)),

	/* REV			cccc 0110 1011 xxxx xxxx xxxx 0011 xxxx */
	/* REV16		cccc 0110 1011 xxxx xxxx xxxx 1011 xxxx */
	/* RBIT			cccc 0110 1111 xxxx xxxx xxxx 0011 xxxx */
	/* REVSH		cccc 0110 1111 xxxx xxxx xxxx 1011 xxxx */
	DECODE_EMULATEX	(0x0fb00070, 0x06b00030, PROBES_REV,
						 REGS(0, NOPC, 0, 0, NOPC)),

	/* ???			cccc 0110 0x00 xxxx xxxx xxxx xxx1 xxxx */
	DECODE_REJECT	(0x0fb00010, 0x06000010),
	/* ???			cccc 0110 0xxx xxxx xxxx xxxx 1011 xxxx */
	DECODE_REJECT	(0x0f8000f0, 0x060000b0),
	/* ???			cccc 0110 0xxx xxxx xxxx xxxx 1101 xxxx */
	DECODE_REJECT	(0x0f8000f0, 0x060000d0),
	/* SADD16		cccc 0110 0001 xxxx xxxx xxxx 0001 xxxx */
	/* SADDSUBX		cccc 0110 0001 xxxx xxxx xxxx 0011 xxxx */
	/* SSUBADDX		cccc 0110 0001 xxxx xxxx xxxx 0101 xxxx */
	/* SSUB16		cccc 0110 0001 xxxx xxxx xxxx 0111 xxxx */
	/* SADD8		cccc 0110 0001 xxxx xxxx xxxx 1001 xxxx */
	/* SSUB8		cccc 0110 0001 xxxx xxxx xxxx 1111 xxxx */
	/* QADD16		cccc 0110 0010 xxxx xxxx xxxx 0001 xxxx */
	/* QADDSUBX		cccc 0110 0010 xxxx xxxx xxxx 0011 xxxx */
	/* QSUBADDX		cccc 0110 0010 xxxx xxxx xxxx 0101 xxxx */
	/* QSUB16		cccc 0110 0010 xxxx xxxx xxxx 0111 xxxx */
	/* QADD8		cccc 0110 0010 xxxx xxxx xxxx 1001 xxxx */
	/* QSUB8		cccc 0110 0010 xxxx xxxx xxxx 1111 xxxx */
	/* SHADD16		cccc 0110 0011 xxxx xxxx xxxx 0001 xxxx */
	/* SHADDSUBX		cccc 0110 0011 xxxx xxxx xxxx 0011 xxxx */
	/* SHSUBADDX		cccc 0110 0011 xxxx xxxx xxxx 0101 xxxx */
	/* SHSUB16		cccc 0110 0011 xxxx xxxx xxxx 0111 xxxx */
	/* SHADD8		cccc 0110 0011 xxxx xxxx xxxx 1001 xxxx */
	/* SHSUB8		cccc 0110 0011 xxxx xxxx xxxx 1111 xxxx */
	/* UADD16		cccc 0110 0101 xxxx xxxx xxxx 0001 xxxx */
	/* UADDSUBX		cccc 0110 0101 xxxx xxxx xxxx 0011 xxxx */
	/* USUBADDX		cccc 0110 0101 xxxx xxxx xxxx 0101 xxxx */
	/* USUB16		cccc 0110 0101 xxxx xxxx xxxx 0111 xxxx */
	/* UADD8		cccc 0110 0101 xxxx xxxx xxxx 1001 xxxx */
	/* USUB8		cccc 0110 0101 xxxx xxxx xxxx 1111 xxxx */
	/* UQADD16		cccc 0110 0110 xxxx xxxx xxxx 0001 xxxx */
	/* UQADDSUBX		cccc 0110 0110 xxxx xxxx xxxx 0011 xxxx */
	/* UQSUBADDX		cccc 0110 0110 xxxx xxxx xxxx 0101 xxxx */
	/* UQSUB16		cccc 0110 0110 xxxx xxxx xxxx 0111 xxxx */
	/* UQADD8		cccc 0110 0110 xxxx xxxx xxxx 1001 xxxx */
	/* UQSUB8		cccc 0110 0110 xxxx xxxx xxxx 1111 xxxx */
	/* UHADD16		cccc 0110 0111 xxxx xxxx xxxx 0001 xxxx */
	/* UHADDSUBX		cccc 0110 0111 xxxx xxxx xxxx 0011 xxxx */
	/* UHSUBADDX		cccc 0110 0111 xxxx xxxx xxxx 0101 xxxx */
	/* UHSUB16		cccc 0110 0111 xxxx xxxx xxxx 0111 xxxx */
	/* UHADD8		cccc 0110 0111 xxxx xxxx xxxx 1001 xxxx */
	/* UHSUB8		cccc 0110 0111 xxxx xxxx xxxx 1111 xxxx */
	DECODE_EMULATEX	(0x0f800010, 0x06000010, PROBES_MMI,
						 REGS(NOPC, NOPC, 0, 0, NOPC)),

	/* PKHBT		cccc 0110 1000 xxxx xxxx xxxx x001 xxxx */
	/* PKHTB		cccc 0110 1000 xxxx xxxx xxxx x101 xxxx */
	DECODE_EMULATEX	(0x0ff00030, 0x06800010, PROBES_PACK,
						 REGS(NOPC, NOPC, 0, 0, NOPC)),

	/* ???			cccc 0110 1001 xxxx xxxx xxxx 0111 xxxx */
	/* ???			cccc 0110 1101 xxxx xxxx xxxx 0111 xxxx */
	DECODE_REJECT	(0x0fb000f0, 0x06900070),

	/* SXTB16		cccc 0110 1000 1111 xxxx xxxx 0111 xxxx */
	/* SXTB			cccc 0110 1010 1111 xxxx xxxx 0111 xxxx */
	/* SXTH			cccc 0110 1011 1111 xxxx xxxx 0111 xxxx */
	/* UXTB16		cccc 0110 1100 1111 xxxx xxxx 0111 xxxx */
	/* UXTB			cccc 0110 1110 1111 xxxx xxxx 0111 xxxx */
	/* UXTH			cccc 0110 1111 1111 xxxx xxxx 0111 xxxx */
	DECODE_EMULATEX	(0x0f8f00f0, 0x068f0070, PROBES_EXTEND,
						 REGS(0, NOPC, 0, 0, NOPC)),

	/* SXTAB16		cccc 0110 1000 xxxx xxxx xxxx 0111 xxxx */
	/* SXTAB		cccc 0110 1010 xxxx xxxx xxxx 0111 xxxx */
	/* SXTAH		cccc 0110 1011 xxxx xxxx xxxx 0111 xxxx */
	/* UXTAB16		cccc 0110 1100 xxxx xxxx xxxx 0111 xxxx */
	/* UXTAB		cccc 0110 1110 xxxx xxxx xxxx 0111 xxxx */
	/* UXTAH		cccc 0110 1111 xxxx xxxx xxxx 0111 xxxx */
	DECODE_EMULATEX	(0x0f8000f0, 0x06800070, PROBES_EXTEND_ADD,
						 REGS(NOPCX, NOPC, 0, 0, NOPC)),

	DECODE_END
};

static const union decode_item arm_cccc_0111_____xxx1_table[] = {
	/* Media instructions						*/

	/* UNDEFINED		cccc 0111 1111 xxxx xxxx xxxx 1111 xxxx */
	DECODE_REJECT	(0x0ff000f0, 0x07f000f0),

	/* SMLALD		cccc 0111 0100 xxxx xxxx xxxx 00x1 xxxx */
	/* SMLSLD		cccc 0111 0100 xxxx xxxx xxxx 01x1 xxxx */
	DECODE_EMULATEX	(0x0ff00090, 0x07400010, PROBES_MUL_ADD_LONG,
						 REGS(NOPC, NOPC, NOPC, 0, NOPC)),

	/* SMUAD		cccc 0111 0000 xxxx 1111 xxxx 00x1 xxxx */
	/* SMUSD		cccc 0111 0000 xxxx 1111 xxxx 01x1 xxxx */
	DECODE_OR	(0x0ff0f090, 0x0700f010),
	/* SMMUL		cccc 0111 0101 xxxx 1111 xxxx 00x1 xxxx */
	DECODE_OR	(0x0ff0f0d0, 0x0750f010),
	/* USAD8		cccc 0111 1000 xxxx 1111 xxxx 0001 xxxx */
	DECODE_EMULATEX	(0x0ff0f0f0, 0x0780f010, PROBES_MUL_ADD,
						 REGS(NOPC, 0, NOPC, 0, NOPC)),

	/* SMLAD		cccc 0111 0000 xxxx xxxx xxxx 00x1 xxxx */
	/* SMLSD		cccc 0111 0000 xxxx xxxx xxxx 01x1 xxxx */
	DECODE_OR	(0x0ff00090, 0x07000010),
	/* SMMLA		cccc 0111 0101 xxxx xxxx xxxx 00x1 xxxx */
	DECODE_OR	(0x0ff000d0, 0x07500010),
	/* USADA8		cccc 0111 1000 xxxx xxxx xxxx 0001 xxxx */
	DECODE_EMULATEX	(0x0ff000f0, 0x07800010, PROBES_MUL_ADD,
						 REGS(NOPC, NOPCX, NOPC, 0, NOPC)),

	/* SMMLS		cccc 0111 0101 xxxx xxxx xxxx 11x1 xxxx */
	DECODE_EMULATEX	(0x0ff000d0, 0x075000d0, PROBES_MUL_ADD,
						 REGS(NOPC, NOPC, NOPC, 0, NOPC)),

	/* SBFX			cccc 0111 101x xxxx xxxx xxxx x101 xxxx */
	/* UBFX			cccc 0111 111x xxxx xxxx xxxx x101 xxxx */
	DECODE_EMULATEX	(0x0fa00070, 0x07a00050, PROBES_BITFIELD,
						 REGS(0, NOPC, 0, 0, NOPC)),

	/* BFC			cccc 0111 110x xxxx xxxx xxxx x001 1111 */
	DECODE_EMULATEX	(0x0fe0007f, 0x07c0001f, PROBES_BITFIELD,
						 REGS(0, NOPC, 0, 0, 0)),

	/* BFI			cccc 0111 110x xxxx xxxx xxxx x001 xxxx */
	DECODE_EMULATEX	(0x0fe00070, 0x07c00010, PROBES_BITFIELD,
						 REGS(0, NOPC, 0, 0, NOPCX)),

	DECODE_END
};

static const union decode_item arm_cccc_01xx_table[] = {
	/* Load/store word and unsigned byte				*/

	/* LDRB/STRB pc,[...]	cccc 01xx x0xx xxxx xxxx xxxx xxxx xxxx */
	DECODE_REJECT	(0x0c40f000, 0x0440f000),

	/* STRT			cccc 01x0 x010 xxxx xxxx xxxx xxxx xxxx */
	/* LDRT			cccc 01x0 x011 xxxx xxxx xxxx xxxx xxxx */
	/* STRBT		cccc 01x0 x110 xxxx xxxx xxxx xxxx xxxx */
	/* LDRBT		cccc 01x0 x111 xxxx xxxx xxxx xxxx xxxx */
	DECODE_REJECT	(0x0d200000, 0x04200000),

	/* STR (immediate)	cccc 010x x0x0 xxxx xxxx xxxx xxxx xxxx */
	/* STRB (immediate)	cccc 010x x1x0 xxxx xxxx xxxx xxxx xxxx */
	DECODE_EMULATEX	(0x0e100000, 0x04000000, PROBES_STORE,
						 REGS(NOPCWB, ANY, 0, 0, 0)),

	/* LDR (immediate)	cccc 010x x0x1 xxxx xxxx xxxx xxxx xxxx */
	/* LDRB (immediate)	cccc 010x x1x1 xxxx xxxx xxxx xxxx xxxx */
	DECODE_EMULATEX	(0x0e100000, 0x04100000, PROBES_LOAD,
						 REGS(NOPCWB, ANY, 0, 0, 0)),

	/* STR (register)	cccc 011x x0x0 xxxx xxxx xxxx xxxx xxxx */
	/* STRB (register)	cccc 011x x1x0 xxxx xxxx xxxx xxxx xxxx */
	DECODE_EMULATEX	(0x0e100000, 0x06000000, PROBES_STORE,
						 REGS(NOPCWB, ANY, 0, 0, NOPC)),

	/* LDR (register)	cccc 011x x0x1 xxxx xxxx xxxx xxxx xxxx */
	/* LDRB (register)	cccc 011x x1x1 xxxx xxxx xxxx xxxx xxxx */
	DECODE_EMULATEX	(0x0e100000, 0x06100000, PROBES_LOAD,
						 REGS(NOPCWB, ANY, 0, 0, NOPC)),

	DECODE_END
};

static const union decode_item arm_cccc_100x_table[] = {
	/* Block data transfer instructions				*/

	/* LDM			cccc 100x x0x1 xxxx xxxx xxxx xxxx xxxx */
	/* STM			cccc 100x x0x0 xxxx xxxx xxxx xxxx xxxx */
	DECODE_CUSTOM	(0x0e400000, 0x08000000, PROBES_LDMSTM),

	/* STM (user registers)	cccc 100x x1x0 xxxx xxxx xxxx xxxx xxxx */
	/* LDM (user registers)	cccc 100x x1x1 xxxx 0xxx xxxx xxxx xxxx */
	/* LDM (exception ret)	cccc 100x x1x1 xxxx 1xxx xxxx xxxx xxxx */
	DECODE_END
};

const union decode_item probes_decode_arm_table[] = {
	/*
	 * Unconditional instructions
	 *			1111 xxxx xxxx xxxx xxxx xxxx xxxx xxxx
	 */
	DECODE_TABLE	(0xf0000000, 0xf0000000, arm_1111_table),

	/*
	 * Miscellaneous instructions
	 *			cccc 0001 0xx0 xxxx xxxx xxxx 0xxx xxxx
	 */
	DECODE_TABLE	(0x0f900080, 0x01000000, arm_cccc_0001_0xx0____0xxx_table),

	/*
	 * Halfword multiply and multiply-accumulate
	 *			cccc 0001 0xx0 xxxx xxxx xxxx 1xx0 xxxx
	 */
	DECODE_TABLE	(0x0f900090, 0x01000080, arm_cccc_0001_0xx0____1xx0_table),

	/*
	 * Multiply and multiply-accumulate
	 *			cccc 0000 xxxx xxxx xxxx xxxx 1001 xxxx
	 */
	DECODE_TABLE	(0x0f0000f0, 0x00000090, arm_cccc_0000_____1001_table),

	/*
	 * Synchronization primitives
	 *			cccc 0001 xxxx xxxx xxxx xxxx 1001 xxxx
	 */
	DECODE_TABLE	(0x0f0000f0, 0x01000090, arm_cccc_0001_____1001_table),

	/*
	 * Extra load/store instructions
	 *			cccc 000x xxxx xxxx xxxx xxxx 1xx1 xxxx
	 */
	DECODE_TABLE	(0x0e000090, 0x00000090, arm_cccc_000x_____1xx1_table),

	/*
	 * Data-processing (register)
	 *			cccc 000x xxxx xxxx xxxx xxxx xxx0 xxxx
	 * Data-processing (register-shifted register)
	 *			cccc 000x xxxx xxxx xxxx xxxx 0xx1 xxxx
	 */
	DECODE_TABLE	(0x0e000000, 0x00000000, arm_cccc_000x_table),

	/*
	 * Data-processing (immediate)
	 *			cccc 001x xxxx xxxx xxxx xxxx xxxx xxxx
	 */
	DECODE_TABLE	(0x0e000000, 0x02000000, arm_cccc_001x_table),

	/*
	 * Media instructions
	 *			cccc 011x xxxx xxxx xxxx xxxx xxx1 xxxx
	 */
	DECODE_TABLE	(0x0f000010, 0x06000010, arm_cccc_0110_____xxx1_table),
	DECODE_TABLE	(0x0f000010, 0x07000010, arm_cccc_0111_____xxx1_table),

	/*
	 * Load/store word and unsigned byte
	 *			cccc 01xx xxxx xxxx xxxx xxxx xxxx xxxx
	 */
	DECODE_TABLE	(0x0c000000, 0x04000000, arm_cccc_01xx_table),

	/*
	 * Block data transfer instructions
	 *			cccc 100x xxxx xxxx xxxx xxxx xxxx xxxx
	 */
	DECODE_TABLE	(0x0e000000, 0x08000000, arm_cccc_100x_table),

	/* B			cccc 1010 xxxx xxxx xxxx xxxx xxxx xxxx */
	/* BL			cccc 1011 xxxx xxxx xxxx xxxx xxxx xxxx */
	DECODE_SIMULATE	(0x0e000000, 0x0a000000, PROBES_BRANCH),

	/*
	 * Supervisor Call, and coprocessor instructions
	 */

	/* MCRR			cccc 1100 0100 xxxx xxxx xxxx xxxx xxxx */
	/* MRRC			cccc 1100 0101 xxxx xxxx xxxx xxxx xxxx */
	/* LDC			cccc 110x xxx1 xxxx xxxx xxxx xxxx xxxx */
	/* STC			cccc 110x xxx0 xxxx xxxx xxxx xxxx xxxx */
	/* CDP			cccc 1110 xxxx xxxx xxxx xxxx xxx0 xxxx */
	/* MCR			cccc 1110 xxx0 xxxx xxxx xxxx xxx1 xxxx */
	/* MRC			cccc 1110 xxx1 xxxx xxxx xxxx xxx1 xxxx */
	/* SVC			cccc 1111 xxxx xxxx xxxx xxxx xxxx xxxx */
	DECODE_REJECT	(0x0c000000, 0x0c000000),

	DECODE_END
};
#ifdef CONFIG_ARM_KPROBES_TEST_MODULE
EXPORT_SYMBOL_GPL(probes_decode_arm_table);
#endif

static void __kprobes arm_singlestep(probes_opcode_t insn,
		struct arch_probes_insn *asi, struct pt_regs *regs)
{
	regs->ARM_pc += 4;
	asi->insn_handler(insn, asi, regs);
}

/* Return:
 *   INSN_REJECTED     If instruction is one not allowed to kprobe,
 *   INSN_GOOD         If instruction is supported and uses instruction slot,
 *   INSN_GOOD_NO_SLOT If instruction is supported but doesn't use its slot.
 *
 * For instructions we don't want to kprobe (INSN_REJECTED return result):
 *   These are generally ones that modify the processor state making
 *   them "hard" to simulate such as switches processor modes or
 *   make accesses in alternate modes.  Any of these could be simulated
 *   if the work was put into it, but low return considering they
 *   should also be very rare.
 */
enum probes_insn __kprobes
arm_probes_decode_insn(probes_opcode_t insn, struct arch_probes_insn *asi,
		       bool emulate, const union decode_action *actions,
		       const struct decode_checker *checkers[])
{
	asi->insn_singlestep = arm_singlestep;
	asi->insn_check_cc = probes_condition_checks[insn>>28];
	return probes_decode_insn(insn, asi, probes_decode_arm_table, false,
				  emulate, actions, checkers);
}
