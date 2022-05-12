/*
 * include/asm-xtensa/asmmacro.h
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2005 Tensilica Inc.
 */

#ifndef _XTENSA_ASMMACRO_H
#define _XTENSA_ASMMACRO_H

#include <asm/core.h>

/*
 * Some little helpers for loops. Use zero-overhead-loops
 * where applicable and if supported by the processor.
 *
 * __loopi ar, at, size, inc
 *         ar	register initialized with the start address
 *	   at	scratch register used by macro
 *	   size	size immediate value
 *	   inc	increment
 *
 * __loops ar, as, at, inc_log2[, mask_log2][, cond][, ncond]
 *	   ar	register initialized with the start address
 *	   as	register initialized with the size
 *	   at	scratch register use by macro
 *	   inc_log2	increment [in log2]
 *	   mask_log2	mask [in log2]
 *	   cond		true condition (used in loop'cond')
 *	   ncond	false condition (used in b'ncond')
 *
 * __loop  as
 *	   restart loop. 'as' register must not have been modified!
 *
 * __endla ar, as, incr
 *	   ar	start address (modified)
 *	   as	scratch register used by __loops/__loopi macros or
 *		end address used by __loopt macro
 *	   inc	increment
 */

/*
 * loop for given size as immediate
 */

	.macro	__loopi ar, at, size, incr

#if XCHAL_HAVE_LOOPS
		movi	\at, ((\size + \incr - 1) / (\incr))
		loop	\at, 99f
#else
		addi	\at, \ar, \size
		98:
#endif

	.endm

/*
 * loop for given size in register
 */

	.macro	__loops	ar, as, at, incr_log2, mask_log2, cond, ncond

#if XCHAL_HAVE_LOOPS
		.ifgt \incr_log2 - 1
			addi	\at, \as, (1 << \incr_log2) - 1
			.ifnc \mask_log2,
				extui	\at, \at, \incr_log2, \mask_log2
			.else
				srli	\at, \at, \incr_log2
			.endif
		.endif
		loop\cond	\at, 99f
#else
		.ifnc \mask_log2,
			extui	\at, \as, \incr_log2, \mask_log2
		.else
			.ifnc \ncond,
				srli	\at, \as, \incr_log2
			.endif
		.endif
		.ifnc \ncond,
			b\ncond	\at, 99f

		.endif
		.ifnc \mask_log2,
			slli	\at, \at, \incr_log2
			add	\at, \ar, \at
		.else
			add	\at, \ar, \as
		.endif
#endif
		98:

	.endm

/*
 * loop from ar to as
 */

	.macro	__loopt	ar, as, at, incr_log2

#if XCHAL_HAVE_LOOPS
		sub	\at, \as, \ar
		.ifgt	\incr_log2 - 1
			addi	\at, \at, (1 << \incr_log2) - 1
			srli	\at, \at, \incr_log2
		.endif
		loop	\at, 99f
#else
		98:
#endif

	.endm

/*
 * restart loop. registers must be unchanged
 */

	.macro	__loop	as

#if XCHAL_HAVE_LOOPS
		loop	\as, 99f
#else
		98:
#endif

	.endm

/*
 * end of loop with no increment of the address.
 */

	.macro	__endl	ar, as
#if !XCHAL_HAVE_LOOPS
		bltu	\ar, \as, 98b
#endif
		99:
	.endm

/*
 * end of loop with increment of the address.
 */

	.macro	__endla	ar, as, incr
		addi	\ar, \ar, \incr
		__endl	\ar \as
	.endm

/* Load or store instructions that may cause exceptions use the EX macro. */

#define EX(handler)				\
	.section __ex_table, "a";		\
	.word	97f, handler;			\
	.previous				\
97:


/*
 * Extract unaligned word that is split between two registers w0 and w1
 * into r regardless of machine endianness. SAR must be loaded with the
 * starting bit of the word (see __ssa8).
 */

	.macro __src_b	r, w0, w1
#ifdef __XTENSA_EB__
		src	\r, \w0, \w1
#else
		src	\r, \w1, \w0
#endif
	.endm

/*
 * Load 2 lowest address bits of r into SAR for __src_b to extract unaligned
 * word starting at r from two registers loaded from consecutive aligned
 * addresses covering r regardless of machine endianness.
 *
 *      r   0   1   2   3
 * LE SAR   0   8  16  24
 * BE SAR  32  24  16   8
 */

	.macro __ssa8	r
#ifdef __XTENSA_EB__
		ssa8b	\r
#else
		ssa8l	\r
#endif
	.endm

#define XTENSA_STACK_ALIGNMENT		16

#if defined(__XTENSA_WINDOWED_ABI__)
#define XTENSA_FRAME_SIZE_RESERVE	16
#define XTENSA_SPILL_STACK_RESERVE	32

#define abi_entry(frame_size) \
	entry sp, (XTENSA_FRAME_SIZE_RESERVE + \
		   (((frame_size) + XTENSA_STACK_ALIGNMENT - 1) & \
		    -XTENSA_STACK_ALIGNMENT))
#define abi_entry_default abi_entry(0)

#define abi_ret(frame_size) retw
#define abi_ret_default retw

#elif defined(__XTENSA_CALL0_ABI__)

#define XTENSA_SPILL_STACK_RESERVE	0

#define abi_entry(frame_size) __abi_entry (frame_size)

	.macro	__abi_entry frame_size
	.ifgt \frame_size
	addi sp, sp, -(((\frame_size) + XTENSA_STACK_ALIGNMENT - 1) & \
		       -XTENSA_STACK_ALIGNMENT)
	.endif
	.endm

#define abi_entry_default

#define abi_ret(frame_size) __abi_ret (frame_size)

	.macro	__abi_ret frame_size
	.ifgt \frame_size
	addi sp, sp, (((\frame_size) + XTENSA_STACK_ALIGNMENT - 1) & \
		      -XTENSA_STACK_ALIGNMENT)
	.endif
	ret
	.endm

#define abi_ret_default ret

#else
#error Unsupported Xtensa ABI
#endif

#define __XTENSA_HANDLER	.section ".exception.text", "ax"

#endif /* _XTENSA_ASMMACRO_H */
