// SPDX-License-Identifier: GPL-2.0
/*
 * printf.c:  Internal prom library printf facility.
 *
 * Copyright (C) 1995 David S. Miller (davem@caip.rutgers.edu)
 * Copyright (C) 1997 Jakub Jelinek (jj@sunsite.mff.cuni.cz)
 * Copyright (c) 2002 Pete Zaitcev (zaitcev@yahoo.com)
 *
 * We used to warn all over the code: DO NOT USE prom_printf(),
 * and yet people do. Anton's banking code was outputting banks
 * with prom_printf for most of the 2.4 lifetime. Since an effective
 * stick is not available, we deployed a carrot: an early printk
 * through PROM by means of -p boot option. This ought to fix it.
 * USE printk; if you need, deploy -p.
 */

#include <linux/kernel.h>
#include <linux/compiler.h>
#include <linux/spinlock.h>

#include <asm/openprom.h>
#include <asm/oplib.h>

#define CONSOLE_WRITE_BUF_SIZE	1024

static char ppbuf[1024];
static char console_write_buf[CONSOLE_WRITE_BUF_SIZE];
static DEFINE_RAW_SPINLOCK(console_write_lock);

void notrace prom_write(const char *buf, unsigned int n)
{
	unsigned int dest_len;
	unsigned long flags;
	char *dest;

	dest = console_write_buf;
	raw_spin_lock_irqsave(&console_write_lock, flags);

	dest_len = 0;
	while (n-- != 0) {
		char ch = *buf++;
		if (ch == '\n') {
			*dest++ = '\r';
			dest_len++;
		}
		*dest++ = ch;
		dest_len++;
		if (dest_len >= CONSOLE_WRITE_BUF_SIZE - 1) {
			prom_console_write_buf(console_write_buf, dest_len);
			dest = console_write_buf;
			dest_len = 0;
		}
	}
	if (dest_len)
		prom_console_write_buf(console_write_buf, dest_len);

	raw_spin_unlock_irqrestore(&console_write_lock, flags);
}

void notrace prom_printf(const char *fmt, ...)
{
	va_list args;
	int i;

	va_start(args, fmt);
	i = vscnprintf(ppbuf, sizeof(ppbuf), fmt, args);
	va_end(args);

	prom_write(ppbuf, i);
}
