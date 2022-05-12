/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Machine dependent access functions for RTC registers.
 */
#ifndef _ASM_MC146818RTC_H
#define _ASM_MC146818RTC_H

#include <linux/io.h>
#include <linux/kernel.h>

#define RTC_IRQ BUILD_BUG_ON(1)

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

#endif /* _ASM_MC146818RTC_H */
