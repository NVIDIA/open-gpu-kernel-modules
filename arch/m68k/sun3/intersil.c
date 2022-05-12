/*
 * arch/m68k/sun3/intersil.c
 *
 * basic routines for accessing the intersil clock within the sun3 machines
 *
 * started 11/12/1999 Sam Creasey
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file COPYING in the main directory of this archive
 * for more details.
 */

#include <linux/kernel.h>
#include <linux/rtc.h>

#include <asm/errno.h>
#include <asm/intersil.h>
#include <asm/machdep.h>


/* bits to set for start/run of the intersil */
#define STOP_VAL (INTERSIL_STOP | INTERSIL_INT_ENABLE | INTERSIL_24H_MODE)
#define START_VAL (INTERSIL_RUN | INTERSIL_INT_ENABLE | INTERSIL_24H_MODE)

/* get/set hwclock */

int sun3_hwclk(int set, struct rtc_time *t)
{
	volatile struct intersil_dt *todintersil;
	unsigned long flags;

        todintersil = (struct intersil_dt *) &intersil_clock->counter;

	local_irq_save(flags);

	intersil_clock->cmd_reg = STOP_VAL;

	/* set or read the clock */
	if(set) {
		todintersil->csec = 0;
		todintersil->hour = t->tm_hour;
		todintersil->minute = t->tm_min;
		todintersil->second = t->tm_sec;
		todintersil->month = t->tm_mon + 1;
		todintersil->day = t->tm_mday;
		todintersil->year = (t->tm_year - 68) % 100;
		todintersil->weekday = t->tm_wday;
	} else {
		/* read clock */
		t->tm_sec = todintersil->csec;
		t->tm_hour = todintersil->hour;
		t->tm_min = todintersil->minute;
		t->tm_sec = todintersil->second;
		t->tm_mon = todintersil->month - 1;
		t->tm_mday = todintersil->day;
		t->tm_year = todintersil->year + 68;
		t->tm_wday = todintersil->weekday;
		if (t->tm_year < 70)
			t->tm_year += 100;
	}

	intersil_clock->cmd_reg = START_VAL;

	local_irq_restore(flags);

	return 0;

}

