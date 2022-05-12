// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *	DECstation PROM-based early console support.
 *
 *	Copyright (C) 2004, 2007  Maciej W. Rozycki
 */
#include <linux/console.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/string.h>

#include <asm/dec/prom.h>

static void __init prom_console_write(struct console *con, const char *s,
				      unsigned int c)
{
	char buf[81];
	unsigned int chunk = sizeof(buf) - 1;

	while (c > 0) {
		if (chunk > c)
			chunk = c;
		memcpy(buf, s, chunk);
		buf[chunk] = '\0';
		prom_printf("%s", buf);
		s += chunk;
		c -= chunk;
	}
}

static struct console promcons __initdata = {
	.name	= "prom",
	.write	= prom_console_write,
	.flags	= CON_BOOT | CON_PRINTBUFFER,
	.index	= -1,
};

void __init register_prom_console(void)
{
	register_console(&promcons);
}
