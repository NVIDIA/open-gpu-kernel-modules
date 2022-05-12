/*
 * include/asm-xtensa/cacheasm.h
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2006 Tensilica Inc.
 */

#include <asm/cache.h>
#include <asm/asmmacro.h>
#include <linux/stringify.h>

/*
 * Define cache functions as macros here so that they can be used
 * by the kernel and boot loader. We should consider moving them to a
 * library that can be linked by both.
 *
 * Locking
 *
 *   ___unlock_dcache_all
 *   ___unlock_icache_all
 *
 * Flush and invaldating
 *
 *   ___flush_invalidate_dcache_{all|range|page}
 *   ___flush_dcache_{all|range|page}
 *   ___invalidate_dcache_{all|range|page}
 *   ___invalidate_icache_{all|range|page}
 *
 */


	.macro	__loop_cache_unroll ar at insn size line_width max_immed

	.if	(1 << (\line_width)) > (\max_immed)
	.set	_reps, 1
	.elseif	(2 << (\line_width)) > (\max_immed)
	.set	_reps, 2
	.else
	.set	_reps, 4
	.endif

	__loopi	\ar, \at, \size, (_reps << (\line_width))
	.set	_index, 0
	.rep	_reps
	\insn	\ar, _index << (\line_width)
	.set	_index, _index + 1
	.endr
	__endla	\ar, \at, _reps << (\line_width)

	.endm


	.macro	__loop_cache_all ar at insn size line_width max_immed

	movi	\ar, 0
	__loop_cache_unroll \ar, \at, \insn, \size, \line_width, \max_immed

	.endm


	.macro	__loop_cache_range ar as at insn line_width

	extui	\at, \ar, 0, \line_width
	add	\as, \as, \at

	__loops	\ar, \as, \at, \line_width
	\insn	\ar, 0
	__endla	\ar, \at, (1 << (\line_width))

	.endm


	.macro	__loop_cache_page ar at insn line_width max_immed

	__loop_cache_unroll \ar, \at, \insn, PAGE_SIZE, \line_width, \max_immed

	.endm


	.macro	___unlock_dcache_all ar at

#if XCHAL_DCACHE_LINE_LOCKABLE && XCHAL_DCACHE_SIZE
	__loop_cache_all \ar \at diu XCHAL_DCACHE_SIZE \
		XCHAL_DCACHE_LINEWIDTH 240
#endif

	.endm


	.macro	___unlock_icache_all ar at

#if XCHAL_ICACHE_LINE_LOCKABLE && XCHAL_ICACHE_SIZE
	__loop_cache_all \ar \at iiu XCHAL_ICACHE_SIZE \
		XCHAL_ICACHE_LINEWIDTH 240
#endif

	.endm


	.macro	___flush_invalidate_dcache_all ar at

#if XCHAL_DCACHE_SIZE
	__loop_cache_all \ar \at diwbi XCHAL_DCACHE_SIZE \
		XCHAL_DCACHE_LINEWIDTH 240
#endif

	.endm


	.macro	___flush_dcache_all ar at

#if XCHAL_DCACHE_SIZE
	__loop_cache_all \ar \at diwb XCHAL_DCACHE_SIZE \
		XCHAL_DCACHE_LINEWIDTH 240
#endif

	.endm


	.macro	___invalidate_dcache_all ar at

#if XCHAL_DCACHE_SIZE
	__loop_cache_all \ar \at dii XCHAL_DCACHE_SIZE \
			 XCHAL_DCACHE_LINEWIDTH 1020
#endif

	.endm


	.macro	___invalidate_icache_all ar at

#if XCHAL_ICACHE_SIZE
	__loop_cache_all \ar \at iii XCHAL_ICACHE_SIZE \
			 XCHAL_ICACHE_LINEWIDTH 1020
#endif

	.endm



	.macro	___flush_invalidate_dcache_range ar as at

#if XCHAL_DCACHE_SIZE
	__loop_cache_range \ar \as \at dhwbi XCHAL_DCACHE_LINEWIDTH
#endif

	.endm


	.macro	___flush_dcache_range ar as at

#if XCHAL_DCACHE_SIZE
	__loop_cache_range \ar \as \at dhwb XCHAL_DCACHE_LINEWIDTH
#endif

	.endm


	.macro	___invalidate_dcache_range ar as at

#if XCHAL_DCACHE_SIZE
	__loop_cache_range \ar \as \at dhi XCHAL_DCACHE_LINEWIDTH
#endif

	.endm


	.macro	___invalidate_icache_range ar as at

#if XCHAL_ICACHE_SIZE
	__loop_cache_range \ar \as \at ihi XCHAL_ICACHE_LINEWIDTH
#endif

	.endm



	.macro	___flush_invalidate_dcache_page ar as

#if XCHAL_DCACHE_SIZE
	__loop_cache_page \ar \as dhwbi XCHAL_DCACHE_LINEWIDTH 1020
#endif

	.endm


	.macro ___flush_dcache_page ar as

#if XCHAL_DCACHE_SIZE
	__loop_cache_page \ar \as dhwb XCHAL_DCACHE_LINEWIDTH 1020
#endif

	.endm


	.macro	___invalidate_dcache_page ar as

#if XCHAL_DCACHE_SIZE
	__loop_cache_page \ar \as dhi XCHAL_DCACHE_LINEWIDTH 1020
#endif

	.endm


	.macro	___invalidate_icache_page ar as

#if XCHAL_ICACHE_SIZE
	__loop_cache_page \ar \as ihi XCHAL_ICACHE_LINEWIDTH 1020
#endif

	.endm
