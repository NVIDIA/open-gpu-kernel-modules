// SPDX-License-Identifier: GPL-2.0-or-later
/* align.c - handle alignment exceptions for the Power PC.
 *
 * Copyright (c) 1996 Paul Mackerras <paulus@cs.anu.edu.au>
 * Copyright (c) 1998-1999 TiVo, Inc.
 *   PowerPC 403GCX modifications.
 * Copyright (c) 1999 Grant Erickson <grant@lcse.umn.edu>
 *   PowerPC 403GCX/405GP modifications.
 * Copyright (c) 2001-2002 PPC64 team, IBM Corp
 *   64-bit and Power4 support
 * Copyright (c) 2005 Benjamin Herrenschmidt, IBM Corp
 *                    <benh@kernel.crashing.org>
 *   Merge ppc32 and ppc64 implementations
 */

#include <linux/kernel.h>
#include <linux/mm.h>
#include <asm/processor.h>
#include <linux/uaccess.h>
#include <asm/cache.h>
#include <asm/cputable.h>
#include <asm/emulated_ops.h>
#include <asm/switch_to.h>
#include <asm/disassemble.h>
#include <asm/cpu_has_feature.h>
#include <asm/sstep.h>
#include <asm/inst.h>

struct aligninfo {
	unsigned char len;
	unsigned char flags;
};


#define INVALID	{ 0, 0 }

/* Bits in the flags field */
#define LD	0	/* load */
#define ST	1	/* store */
#define SE	2	/* sign-extend value, or FP ld/st as word */
#define SW	0x20	/* byte swap */
#define E4	0x40	/* SPE endianness is word */
#define E8	0x80	/* SPE endianness is double word */

#ifdef CONFIG_SPE

static struct aligninfo spe_aligninfo[32] = {
	{ 8, LD+E8 },		/* 0 00 00: evldd[x] */
	{ 8, LD+E4 },		/* 0 00 01: evldw[x] */
	{ 8, LD },		/* 0 00 10: evldh[x] */
	INVALID,		/* 0 00 11 */
	{ 2, LD },		/* 0 01 00: evlhhesplat[x] */
	INVALID,		/* 0 01 01 */
	{ 2, LD },		/* 0 01 10: evlhhousplat[x] */
	{ 2, LD+SE },		/* 0 01 11: evlhhossplat[x] */
	{ 4, LD },		/* 0 10 00: evlwhe[x] */
	INVALID,		/* 0 10 01 */
	{ 4, LD },		/* 0 10 10: evlwhou[x] */
	{ 4, LD+SE },		/* 0 10 11: evlwhos[x] */
	{ 4, LD+E4 },		/* 0 11 00: evlwwsplat[x] */
	INVALID,		/* 0 11 01 */
	{ 4, LD },		/* 0 11 10: evlwhsplat[x] */
	INVALID,		/* 0 11 11 */

	{ 8, ST+E8 },		/* 1 00 00: evstdd[x] */
	{ 8, ST+E4 },		/* 1 00 01: evstdw[x] */
	{ 8, ST },		/* 1 00 10: evstdh[x] */
	INVALID,		/* 1 00 11 */
	INVALID,		/* 1 01 00 */
	INVALID,		/* 1 01 01 */
	INVALID,		/* 1 01 10 */
	INVALID,		/* 1 01 11 */
	{ 4, ST },		/* 1 10 00: evstwhe[x] */
	INVALID,		/* 1 10 01 */
	{ 4, ST },		/* 1 10 10: evstwho[x] */
	INVALID,		/* 1 10 11 */
	{ 4, ST+E4 },		/* 1 11 00: evstwwe[x] */
	INVALID,		/* 1 11 01 */
	{ 4, ST+E4 },		/* 1 11 10: evstwwo[x] */
	INVALID,		/* 1 11 11 */
};

#define	EVLDD		0x00
#define	EVLDW		0x01
#define	EVLDH		0x02
#define	EVLHHESPLAT	0x04
#define	EVLHHOUSPLAT	0x06
#define	EVLHHOSSPLAT	0x07
#define	EVLWHE		0x08
#define	EVLWHOU		0x0A
#define	EVLWHOS		0x0B
#define	EVLWWSPLAT	0x0C
#define	EVLWHSPLAT	0x0E
#define	EVSTDD		0x10
#define	EVSTDW		0x11
#define	EVSTDH		0x12
#define	EVSTWHE		0x18
#define	EVSTWHO		0x1A
#define	EVSTWWE		0x1C
#define	EVSTWWO		0x1E

/*
 * Emulate SPE loads and stores.
 * Only Book-E has these instructions, and it does true little-endian,
 * so we don't need the address swizzling.
 */
static int emulate_spe(struct pt_regs *regs, unsigned int reg,
		       struct ppc_inst ppc_instr)
{
	union {
		u64 ll;
		u32 w[2];
		u16 h[4];
		u8 v[8];
	} data, temp;
	unsigned char __user *p, *addr;
	unsigned long *evr = &current->thread.evr[reg];
	unsigned int nb, flags, instr;

	instr = ppc_inst_val(ppc_instr);
	instr = (instr >> 1) & 0x1f;

	/* DAR has the operand effective address */
	addr = (unsigned char __user *)regs->dar;

	nb = spe_aligninfo[instr].len;
	flags = spe_aligninfo[instr].flags;

	/* userland only */
	if (unlikely(!user_mode(regs)))
		return 0;

	flush_spe_to_thread(current);

	/* If we are loading, get the data from user space, else
	 * get it from register values
	 */
	if (flags & ST) {
		data.ll = 0;
		switch (instr) {
		case EVSTDD:
		case EVSTDW:
		case EVSTDH:
			data.w[0] = *evr;
			data.w[1] = regs->gpr[reg];
			break;
		case EVSTWHE:
			data.h[2] = *evr >> 16;
			data.h[3] = regs->gpr[reg] >> 16;
			break;
		case EVSTWHO:
			data.h[2] = *evr & 0xffff;
			data.h[3] = regs->gpr[reg] & 0xffff;
			break;
		case EVSTWWE:
			data.w[1] = *evr;
			break;
		case EVSTWWO:
			data.w[1] = regs->gpr[reg];
			break;
		default:
			return -EINVAL;
		}
	} else {
		temp.ll = data.ll = 0;
		p = addr;

		if (!user_read_access_begin(addr, nb))
			return -EFAULT;

		switch (nb) {
		case 8:
			unsafe_get_user(temp.v[0], p++, Efault_read);
			unsafe_get_user(temp.v[1], p++, Efault_read);
			unsafe_get_user(temp.v[2], p++, Efault_read);
			unsafe_get_user(temp.v[3], p++, Efault_read);
			fallthrough;
		case 4:
			unsafe_get_user(temp.v[4], p++, Efault_read);
			unsafe_get_user(temp.v[5], p++, Efault_read);
			fallthrough;
		case 2:
			unsafe_get_user(temp.v[6], p++, Efault_read);
			unsafe_get_user(temp.v[7], p++, Efault_read);
		}
		user_read_access_end();

		switch (instr) {
		case EVLDD:
		case EVLDW:
		case EVLDH:
			data.ll = temp.ll;
			break;
		case EVLHHESPLAT:
			data.h[0] = temp.h[3];
			data.h[2] = temp.h[3];
			break;
		case EVLHHOUSPLAT:
		case EVLHHOSSPLAT:
			data.h[1] = temp.h[3];
			data.h[3] = temp.h[3];
			break;
		case EVLWHE:
			data.h[0] = temp.h[2];
			data.h[2] = temp.h[3];
			break;
		case EVLWHOU:
		case EVLWHOS:
			data.h[1] = temp.h[2];
			data.h[3] = temp.h[3];
			break;
		case EVLWWSPLAT:
			data.w[0] = temp.w[1];
			data.w[1] = temp.w[1];
			break;
		case EVLWHSPLAT:
			data.h[0] = temp.h[2];
			data.h[1] = temp.h[2];
			data.h[2] = temp.h[3];
			data.h[3] = temp.h[3];
			break;
		default:
			return -EINVAL;
		}
	}

	if (flags & SW) {
		switch (flags & 0xf0) {
		case E8:
			data.ll = swab64(data.ll);
			break;
		case E4:
			data.w[0] = swab32(data.w[0]);
			data.w[1] = swab32(data.w[1]);
			break;
		/* Its half word endian */
		default:
			data.h[0] = swab16(data.h[0]);
			data.h[1] = swab16(data.h[1]);
			data.h[2] = swab16(data.h[2]);
			data.h[3] = swab16(data.h[3]);
			break;
		}
	}

	if (flags & SE) {
		data.w[0] = (s16)data.h[1];
		data.w[1] = (s16)data.h[3];
	}

	/* Store result to memory or update registers */
	if (flags & ST) {
		p = addr;

		if (!user_write_access_begin(addr, nb))
			return -EFAULT;

		switch (nb) {
		case 8:
			unsafe_put_user(data.v[0], p++, Efault_write);
			unsafe_put_user(data.v[1], p++, Efault_write);
			unsafe_put_user(data.v[2], p++, Efault_write);
			unsafe_put_user(data.v[3], p++, Efault_write);
			fallthrough;
		case 4:
			unsafe_put_user(data.v[4], p++, Efault_write);
			unsafe_put_user(data.v[5], p++, Efault_write);
			fallthrough;
		case 2:
			unsafe_put_user(data.v[6], p++, Efault_write);
			unsafe_put_user(data.v[7], p++, Efault_write);
		}
		user_write_access_end();
	} else {
		*evr = data.w[0];
		regs->gpr[reg] = data.w[1];
	}

	return 1;

Efault_read:
	user_read_access_end();
	return -EFAULT;

Efault_write:
	user_write_access_end();
	return -EFAULT;
}
#endif /* CONFIG_SPE */

/*
 * Called on alignment exception. Attempts to fixup
 *
 * Return 1 on success
 * Return 0 if unable to handle the interrupt
 * Return -EFAULT if data address is bad
 * Other negative return values indicate that the instruction can't
 * be emulated, and the process should be given a SIGBUS.
 */

int fix_alignment(struct pt_regs *regs)
{
	struct ppc_inst instr;
	struct instruction_op op;
	int r, type;

	if (is_kernel_addr(regs->nip))
		r = copy_inst_from_kernel_nofault(&instr, (void *)regs->nip);
	else
		r = __get_user_instr(instr, (void __user *)regs->nip);

	if (unlikely(r))
		return -EFAULT;
	if ((regs->msr & MSR_LE) != (MSR_KERNEL & MSR_LE)) {
		/* We don't handle PPC little-endian any more... */
		if (cpu_has_feature(CPU_FTR_PPC_LE))
			return -EIO;
		instr = ppc_inst_swab(instr);
	}

#ifdef CONFIG_SPE
	if (ppc_inst_primary_opcode(instr) == 0x4) {
		int reg = (ppc_inst_val(instr) >> 21) & 0x1f;
		PPC_WARN_ALIGNMENT(spe, regs);
		return emulate_spe(regs, reg, instr);
	}
#endif


	/*
	 * ISA 3.0 (such as P9) copy, copy_first, paste and paste_last alignment
	 * check.
	 *
	 * Send a SIGBUS to the process that caused the fault.
	 *
	 * We do not emulate these because paste may contain additional metadata
	 * when pasting to a co-processor. Furthermore, paste_last is the
	 * synchronisation point for preceding copy/paste sequences.
	 */
	if ((ppc_inst_val(instr) & 0xfc0006fe) == (PPC_INST_COPY & 0xfc0006fe))
		return -EIO;

	r = analyse_instr(&op, regs, instr);
	if (r < 0)
		return -EINVAL;

	type = GETTYPE(op.type);
	if (!OP_IS_LOAD_STORE(type)) {
		if (op.type != CACHEOP + DCBZ)
			return -EINVAL;
		PPC_WARN_ALIGNMENT(dcbz, regs);
		r = emulate_dcbz(op.ea, regs);
	} else {
		if (type == LARX || type == STCX)
			return -EIO;
		PPC_WARN_ALIGNMENT(unaligned, regs);
		r = emulate_loadstore(regs, &op);
	}

	if (!r)
		return 1;
	return r;
}
