/* SPDX-License-Identifier: GPL-2.0 */
/****************************************************************************/

/*
 *	mcftimer.h -- ColdFire internal TIMER support defines.
 *
 *	(C) Copyright 1999-2006, Greg Ungerer <gerg@snapgear.com>
 * 	(C) Copyright 2000, Lineo Inc. (www.lineo.com) 
 */

/****************************************************************************/
#ifndef	mcftimer_h
#define	mcftimer_h
/****************************************************************************/

/*
 *	Define the TIMER register set addresses.
 */
#define	MCFTIMER_TMR		0x00		/* Timer Mode reg (r/w) */
#define	MCFTIMER_TRR		0x04		/* Timer Reference (r/w) */
#define	MCFTIMER_TCR		0x08		/* Timer Capture reg (r/w) */
#define	MCFTIMER_TCN		0x0C		/* Timer Counter reg (r/w) */
#if defined(CONFIG_M53xx) || defined(CONFIG_M5441x)
#define	MCFTIMER_TER		0x03		/* Timer Event reg (r/w) */
#else
#define	MCFTIMER_TER		0x11		/* Timer Event reg (r/w) */
#endif

/*
 *	Bit definitions for the Timer Mode Register (TMR).
 *	Register bit flags are common across ColdFires.
 */
#define	MCFTIMER_TMR_PREMASK	0xff00		/* Prescalar mask */
#define	MCFTIMER_TMR_DISCE	0x0000		/* Disable capture */
#define	MCFTIMER_TMR_ANYCE	0x00c0		/* Capture any edge */
#define	MCFTIMER_TMR_FALLCE	0x0080		/* Capture fallingedge */
#define	MCFTIMER_TMR_RISECE	0x0040		/* Capture rising edge */
#define	MCFTIMER_TMR_ENOM	0x0020		/* Enable output toggle */
#define	MCFTIMER_TMR_DISOM	0x0000		/* Do single output pulse  */
#define	MCFTIMER_TMR_ENORI	0x0010		/* Enable ref interrupt */
#define	MCFTIMER_TMR_DISORI	0x0000		/* Disable ref interrupt */
#define	MCFTIMER_TMR_RESTART	0x0008		/* Restart counter */
#define	MCFTIMER_TMR_FREERUN	0x0000		/* Free running counter */
#define	MCFTIMER_TMR_CLKTIN	0x0006		/* Input clock is TIN */
#define	MCFTIMER_TMR_CLK16	0x0004		/* Input clock is /16 */
#define	MCFTIMER_TMR_CLK1	0x0002		/* Input clock is /1 */
#define	MCFTIMER_TMR_CLKSTOP	0x0000		/* Stop counter */
#define	MCFTIMER_TMR_ENABLE	0x0001		/* Enable timer */
#define	MCFTIMER_TMR_DISABLE	0x0000		/* Disable timer */

/*
 *	Bit definitions for the Timer Event Registers (TER).
 */
#define	MCFTIMER_TER_CAP	0x01		/* Capture event */
#define	MCFTIMER_TER_REF	0x02		/* Reference event */

/****************************************************************************/
#endif	/* mcftimer_h */
