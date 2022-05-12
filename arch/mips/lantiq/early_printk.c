// SPDX-License-Identifier: GPL-2.0-only
/*
 *
 *  Copyright (C) 2010 John Crispin <john@phrozen.org>
 */

#include <linux/cpu.h>
#include <lantiq_soc.h>
#include <asm/setup.h>

#define ASC_BUF		1024
#define LTQ_ASC_FSTAT	((u32 *)(LTQ_EARLY_ASC + 0x0048))
#ifdef __BIG_ENDIAN
#define LTQ_ASC_TBUF	((u32 *)(LTQ_EARLY_ASC + 0x0020 + 3))
#else
#define LTQ_ASC_TBUF	((u32 *)(LTQ_EARLY_ASC + 0x0020))
#endif
#define TXMASK		0x3F00
#define TXOFFSET	8

void prom_putchar(char c)
{
	unsigned long flags;

	local_irq_save(flags);
	do { } while ((ltq_r32(LTQ_ASC_FSTAT) & TXMASK) >> TXOFFSET);
	if (c == '\n')
		ltq_w8('\r', LTQ_ASC_TBUF);
	ltq_w8(c, LTQ_ASC_TBUF);
	local_irq_restore(flags);
}
