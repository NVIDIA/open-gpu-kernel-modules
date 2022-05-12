/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_POWERPC_MC146818RTC_H
#define _ASM_POWERPC_MC146818RTC_H

/*
 * Machine dependent access functions for RTC registers.
 */

#ifdef __KERNEL__

#include <asm/io.h>

#ifndef RTC_PORT
#define RTC_PORT(x)	(0x70 + (x))
#define RTC_ALWAYS_BCD	1	/* RTC operates in binary mode */
#endif

/*
 * The yet supported machines all access the RTC index register via
 * an ISA port access but the way to access the date register differs ...
 */
#define CMOS_READ(addr) ({ \
outb_p((addr),RTC_PORT(0)); \
inb_p(RTC_PORT(1)); \
})
#define CMOS_WRITE(val, addr) ({ \
outb_p((addr),RTC_PORT(0)); \
outb_p((val),RTC_PORT(1)); \
})

#endif	/* __KERNEL__ */
#endif	/* _ASM_POWERPC_MC146818RTC_H */
