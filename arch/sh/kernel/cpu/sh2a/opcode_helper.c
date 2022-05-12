// SPDX-License-Identifier: GPL-2.0
/*
 * arch/sh/kernel/cpu/sh2a/opcode_helper.c
 *
 * Helper for the SH-2A 32-bit opcodes.
 *
 *  Copyright (C) 2007  Paul Mundt
 */
#include <linux/kernel.h>

/*
 * Instructions on SH are generally fixed at 16-bits, however, SH-2A
 * introduces some 32-bit instructions. Since there are no real
 * constraints on their use (and they can be mixed and matched), we need
 * to check the instruction encoding to work out if it's a true 32-bit
 * instruction or not.
 *
 * Presently, 32-bit opcodes have only slight variations in what the
 * actual encoding looks like in the first-half of the instruction, which
 * makes it fairly straightforward to differentiate from the 16-bit ones.
 *
 * First 16-bits of encoding		Used by
 *
 *	0011nnnnmmmm0001	mov.b, mov.w, mov.l, fmov.d,
 *				fmov.s, movu.b, movu.w
 *
 *	0011nnnn0iii1001        bclr.b, bld.b, bset.b, bst.b, band.b,
 *				bandnot.b, bldnot.b, bor.b, bornot.b,
 *				bxor.b
 *
 *	0000nnnniiii0000        movi20
 *	0000nnnniiii0001        movi20s
 */
unsigned int instruction_size(unsigned int insn)
{
	/* Look for the common cases */
	switch ((insn & 0xf00f)) {
	case 0x0000:	/* movi20 */
	case 0x0001:	/* movi20s */
	case 0x3001:	/* 32-bit mov/fmov/movu variants */
		return 4;
	}

	/* And the special cases.. */
	switch ((insn & 0xf08f)) {
	case 0x3009:	/* 32-bit b*.b bit operations */
		return 4;
	}

	return 2;
}
