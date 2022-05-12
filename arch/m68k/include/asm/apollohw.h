/* SPDX-License-Identifier: GPL-2.0 */
/* apollohw.h : some structures to access apollo HW */

#ifndef _ASMm68k_APOLLOHW_H_
#define _ASMm68k_APOLLOHW_H_

#include <linux/types.h>

#include <asm/bootinfo-apollo.h>


extern u_long apollo_model;


/*
   see scn2681 data sheet for more info.
   member names are read_write.
*/

#define DECLARE_2681_FIELD(x) unsigned char x; unsigned char dummy##x

struct SCN2681 {

	DECLARE_2681_FIELD(mra);
	DECLARE_2681_FIELD(sra_csra);
	DECLARE_2681_FIELD(BRGtest_cra);
	DECLARE_2681_FIELD(rhra_thra);
	DECLARE_2681_FIELD(ipcr_acr);
	DECLARE_2681_FIELD(isr_imr);
	DECLARE_2681_FIELD(ctu_ctur);
	DECLARE_2681_FIELD(ctl_ctlr);
	DECLARE_2681_FIELD(mrb);
	DECLARE_2681_FIELD(srb_csrb);
	DECLARE_2681_FIELD(tst_crb);
	DECLARE_2681_FIELD(rhrb_thrb);
	DECLARE_2681_FIELD(reserved);
	DECLARE_2681_FIELD(ip_opcr);
	DECLARE_2681_FIELD(startCnt_setOutBit);
	DECLARE_2681_FIELD(stopCnt_resetOutBit);

};

struct mc146818 {
        unsigned char second, alarm_second;
        unsigned char minute, alarm_minute;
        unsigned char hours, alarm_hours;
        unsigned char day_of_week, day_of_month;
        unsigned char month, year;
};


#define IO_BASE 0x80000000

extern u_long sio01_physaddr;
extern u_long sio23_physaddr;
extern u_long rtc_physaddr;
extern u_long pica_physaddr;
extern u_long picb_physaddr;
extern u_long cpuctrl_physaddr;
extern u_long timer_physaddr;

#define SAU7_SIO01_PHYSADDR 0x10400
#define SAU7_SIO23_PHYSADDR 0x10500
#define SAU7_RTC_PHYSADDR 0x10900
#define SAU7_PICA 0x11000
#define SAU7_PICB 0x11100
#define SAU7_CPUCTRL 0x10100
#define SAU7_TIMER 0x010800

#define SAU8_SIO01_PHYSADDR 0x8400
#define SAU8_RTC_PHYSADDR 0x8900
#define SAU8_PICA 0x9400
#define SAU8_PICB 0x9500
#define SAU8_CPUCTRL 0x8100
#define SAU8_TIMER 0x8800

#define sio01 ((*(volatile struct SCN2681 *)(IO_BASE + sio01_physaddr)))
#define sio23 ((*(volatile struct SCN2681 *)(IO_BASE + sio23_physaddr)))
#define rtc (((volatile struct mc146818 *)(IO_BASE + rtc_physaddr)))
#define cpuctrl (*(volatile unsigned int *)(IO_BASE + cpuctrl_physaddr))
#define pica (IO_BASE + pica_physaddr)
#define picb (IO_BASE + picb_physaddr)
#define apollo_timer (IO_BASE + timer_physaddr)
#define addr_xlat_map ((unsigned short *)(IO_BASE + 0x17000))

#define isaIO2mem(x) (((((x) & 0x3f8)  << 7) | (((x) & 0xfc00) >> 6) | ((x) & 0x7)) + 0x40000 + IO_BASE)

#define IRQ_APOLLO	IRQ_USER

#endif
