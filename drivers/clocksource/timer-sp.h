/* SPDX-License-Identifier: GPL-2.0 */
/*
 * ARM timer implementation, found in Integrator, Versatile and Realview
 * platforms.  Not all platforms support all registers and bits in these
 * registers, so we mark them with A for Integrator AP, C for Integrator
 * CP, V for Versatile and R for Realview.
 *
 * Integrator AP has 16-bit timers, Integrator CP, Versatile and Realview
 * can have 16-bit or 32-bit selectable via a bit in the control register.
 *
 * Every SP804 contains two identical timers.
 */
#define NR_TIMERS	2
#define TIMER_1_BASE	0x00
#define TIMER_2_BASE	0x20

#define TIMER_LOAD	0x00			/* ACVR rw */
#define TIMER_VALUE	0x04			/* ACVR ro */
#define TIMER_CTRL	0x08			/* ACVR rw */
#define TIMER_CTRL_ONESHOT	(1 << 0)	/*  CVR */
#define TIMER_CTRL_32BIT	(1 << 1)	/*  CVR */
#define TIMER_CTRL_DIV1		(0 << 2)	/* ACVR */
#define TIMER_CTRL_DIV16	(1 << 2)	/* ACVR */
#define TIMER_CTRL_DIV256	(2 << 2)	/* ACVR */
#define TIMER_CTRL_IE		(1 << 5)	/*   VR */
#define TIMER_CTRL_PERIODIC	(1 << 6)	/* ACVR */
#define TIMER_CTRL_ENABLE	(1 << 7)	/* ACVR */

#define TIMER_INTCLR	0x0c			/* ACVR wo */
#define TIMER_RIS	0x10			/*  CVR ro */
#define TIMER_MIS	0x14			/*  CVR ro */
#define TIMER_BGLOAD	0x18			/*  CVR rw */

struct sp804_timer {
	int load;
	int load_h;
	int value;
	int value_h;
	int ctrl;
	int intclr;
	int ris;
	int mis;
	int bgload;
	int bgload_h;
	int timer_base[NR_TIMERS];
	int width;
};

struct sp804_clkevt {
	void __iomem *base;
	void __iomem *load;
	void __iomem *load_h;
	void __iomem *value;
	void __iomem *value_h;
	void __iomem *ctrl;
	void __iomem *intclr;
	void __iomem *ris;
	void __iomem *mis;
	void __iomem *bgload;
	void __iomem *bgload_h;
	unsigned long reload;
	int width;
};
