/* SPDX-License-Identifier: GPL-2.0
 *
 * include/asm-sh/cpu-sh4/freq.h
 *
 * Copyright (C) 2002, 2003 Paul Mundt
 */
#ifndef __ASM_CPU_SH4_FREQ_H
#define __ASM_CPU_SH4_FREQ_H

#if defined(CONFIG_CPU_SUBTYPE_SH7722) || \
    defined(CONFIG_CPU_SUBTYPE_SH7723) || \
    defined(CONFIG_CPU_SUBTYPE_SH7343) || \
    defined(CONFIG_CPU_SUBTYPE_SH7366)
#define FRQCR		        0xa4150000
#define VCLKCR			0xa4150004
#define SCLKACR			0xa4150008
#define SCLKBCR			0xa415000c
#define IrDACLKCR		0xa4150010
#define MSTPCR0			0xa4150030
#define MSTPCR1			0xa4150034
#define MSTPCR2			0xa4150038
#elif defined(CONFIG_CPU_SUBTYPE_SH7757)
#define	FRQCR			0xffc80000
#define	OSCCR			0xffc80018
#define	PLLCR			0xffc80024
#elif defined(CONFIG_CPU_SUBTYPE_SH7763) || \
      defined(CONFIG_CPU_SUBTYPE_SH7780)
#define	FRQCR			0xffc80000
#elif defined(CONFIG_CPU_SUBTYPE_SH7724)
#define FRQCRA			0xa4150000
#define FRQCRB			0xa4150004
#define VCLKCR			0xa4150048

#define FCLKACR			0xa4150008
#define FCLKBCR			0xa415000c
#define FRQCR			FRQCRA
#define SCLKACR			FCLKACR
#define SCLKBCR			FCLKBCR
#define FCLKACR			0xa4150008
#define FCLKBCR			0xa415000c
#define IrDACLKCR		0xa4150018

#define MSTPCR0			0xa4150030
#define MSTPCR1			0xa4150034
#define MSTPCR2			0xa4150038

#elif defined(CONFIG_CPU_SUBTYPE_SH7734)
#define FRQCR0			0xffc80000
#define FRQCR2			0xffc80008
#define FRQMR1			0xffc80014
#define FRQMR2			0xffc80018
#elif defined(CONFIG_CPU_SUBTYPE_SH7785)
#define FRQCR0			0xffc80000
#define FRQCR1			0xffc80004
#define FRQMR1			0xffc80014
#elif defined(CONFIG_CPU_SUBTYPE_SH7786)
#define FRQCR0			0xffc40000
#define FRQCR1			0xffc40004
#define FRQMR1			0xffc40014
#elif defined(CONFIG_CPU_SUBTYPE_SHX3)
#define FRQCR0			0xffc00000
#define FRQCR1			0xffc00004
#define FRQMR1			0xffc00014
#else
#define FRQCR			0xffc00000
#define FRQCR_PSTBY		0x0200
#define FRQCR_PLLEN		0x0400
#define FRQCR_CKOEN		0x0800
#endif
#define MIN_DIVISOR_NR		0
#define MAX_DIVISOR_NR		3

#endif /* __ASM_CPU_SH4_FREQ_H */

