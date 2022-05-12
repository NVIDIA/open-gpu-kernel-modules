/* SPDX-License-Identifier: GPL-2.0 */
/*
 * zs.h: Definitions for the DECstation Z85C30 serial driver.
 *
 * Adapted from drivers/sbus/char/sunserial.h by Paul Mackerras.
 * Adapted from drivers/macintosh/macserial.h by Harald Koerfgen.
 *
 * Copyright (C) 1996 Paul Mackerras (Paul.Mackerras@cs.anu.edu.au)
 * Copyright (C) 1995 David S. Miller (davem@caip.rutgers.edu)
 * Copyright (C) 2004, 2005, 2007  Maciej W. Rozycki
 */
#ifndef _SERIAL_ZS_H
#define _SERIAL_ZS_H

#ifdef __KERNEL__

#define ZS_NUM_REGS 16

/*
 * This is our internal structure for each serial port's state.
 */
struct zs_port {
	struct zs_scc	*scc;			/* Containing SCC.  */
	struct uart_port port;			/* Underlying UART.  */

	int		clk_mode;		/* May be 1, 16, 32, or 64.  */

	unsigned int	tty_break;		/* Set on BREAK condition.  */
	int		tx_stopped;		/* Output is suspended.  */

	unsigned int	mctrl;			/* State of modem lines.  */
	u8		brk;			/* BREAK state from RR0.  */

	u8		regs[ZS_NUM_REGS];	/* Channel write registers.  */
};

/*
 * Per-SCC state for locking and the interrupt handler.
 */
struct zs_scc {
	struct zs_port	zport[2];
	spinlock_t	zlock;
	atomic_t	irq_guard;
	int		initialised;
};

#endif /* __KERNEL__ */

/*
 * Conversion routines to/from brg time constants from/to bits per second.
 */
#define ZS_BRG_TO_BPS(brg, freq) ((freq) / 2 / ((brg) + 2))
#define ZS_BPS_TO_BRG(bps, freq) ((((freq) + (bps)) / (2 * (bps))) - 2)

/*
 * The Zilog register set.
 */

/* Write Register 0 (Command) */
#define R0		0	/* Register selects */
#define R1		1
#define R2		2
#define R3		3
#define R4		4
#define R5		5
#define R6		6
#define R7		7
#define R8		8
#define R9		9
#define R10		10
#define R11		11
#define R12		12
#define R13		13
#define R14		14
#define R15		15

#define NULLCODE	0	/* Null Code */
#define POINT_HIGH	0x8	/* Select upper half of registers */
#define RES_EXT_INT	0x10	/* Reset Ext. Status Interrupts */
#define SEND_ABORT	0x18	/* HDLC Abort */
#define RES_RxINT_FC	0x20	/* Reset RxINT on First Character */
#define RES_Tx_P	0x28	/* Reset TxINT Pending */
#define ERR_RES		0x30	/* Error Reset */
#define RES_H_IUS	0x38	/* Reset highest IUS */

#define RES_Rx_CRC	0x40	/* Reset Rx CRC Checker */
#define RES_Tx_CRC	0x80	/* Reset Tx CRC Checker */
#define RES_EOM_L	0xC0	/* Reset EOM latch */

/* Write Register 1 (Tx/Rx/Ext Int Enable and WAIT/DMA Commands) */
#define EXT_INT_ENAB	0x1	/* Ext Int Enable */
#define TxINT_ENAB	0x2	/* Tx Int Enable */
#define PAR_SPEC	0x4	/* Parity is special condition */

#define RxINT_DISAB	0	/* Rx Int Disable */
#define RxINT_FCERR	0x8	/* Rx Int on First Character Only or Error */
#define RxINT_ALL	0x10	/* Int on all Rx Characters or error */
#define RxINT_ERR	0x18	/* Int on error only */
#define RxINT_MASK	0x18

#define WT_RDY_RT	0x20	/* Wait/Ready on R/T */
#define WT_FN_RDYFN	0x40	/* Wait/FN/Ready FN */
#define WT_RDY_ENAB	0x80	/* Wait/Ready Enable */

/* Write Register 2 (Interrupt Vector) */

/* Write Register 3 (Receive Parameters and Control) */
#define RxENABLE	0x1	/* Rx Enable */
#define SYNC_L_INH	0x2	/* Sync Character Load Inhibit */
#define ADD_SM		0x4	/* Address Search Mode (SDLC) */
#define RxCRC_ENAB	0x8	/* Rx CRC Enable */
#define ENT_HM		0x10	/* Enter Hunt Mode */
#define AUTO_ENAB	0x20	/* Auto Enables */
#define Rx5		0x0	/* Rx 5 Bits/Character */
#define Rx7		0x40	/* Rx 7 Bits/Character */
#define Rx6		0x80	/* Rx 6 Bits/Character */
#define Rx8		0xc0	/* Rx 8 Bits/Character */
#define RxNBITS_MASK	0xc0

/* Write Register 4 (Transmit/Receive Miscellaneous Parameters and Modes) */
#define PAR_ENA		0x1	/* Parity Enable */
#define PAR_EVEN	0x2	/* Parity Even/Odd* */

#define SYNC_ENAB	0	/* Sync Modes Enable */
#define SB1		0x4	/* 1 stop bit/char */
#define SB15		0x8	/* 1.5 stop bits/char */
#define SB2		0xc	/* 2 stop bits/char */
#define SB_MASK		0xc

#define MONSYNC		0	/* 8 Bit Sync character */
#define BISYNC		0x10	/* 16 bit sync character */
#define SDLC		0x20	/* SDLC Mode (01111110 Sync Flag) */
#define EXTSYNC		0x30	/* External Sync Mode */

#define X1CLK		0x0	/* x1 clock mode */
#define X16CLK		0x40	/* x16 clock mode */
#define X32CLK		0x80	/* x32 clock mode */
#define X64CLK		0xc0	/* x64 clock mode */
#define XCLK_MASK	0xc0

/* Write Register 5 (Transmit Parameters and Controls) */
#define TxCRC_ENAB	0x1	/* Tx CRC Enable */
#define RTS		0x2	/* RTS */
#define SDLC_CRC	0x4	/* SDLC/CRC-16 */
#define TxENAB		0x8	/* Tx Enable */
#define SND_BRK		0x10	/* Send Break */
#define Tx5		0x0	/* Tx 5 bits (or less)/character */
#define Tx7		0x20	/* Tx 7 bits/character */
#define Tx6		0x40	/* Tx 6 bits/character */
#define Tx8		0x60	/* Tx 8 bits/character */
#define TxNBITS_MASK	0x60
#define DTR		0x80	/* DTR */

/* Write Register 6 (Sync bits 0-7/SDLC Address Field) */

/* Write Register 7 (Sync bits 8-15/SDLC 01111110) */

/* Write Register 8 (Transmit Buffer) */

/* Write Register 9 (Master Interrupt Control) */
#define VIS		1	/* Vector Includes Status */
#define NV		2	/* No Vector */
#define DLC		4	/* Disable Lower Chain */
#define MIE		8	/* Master Interrupt Enable */
#define STATHI		0x10	/* Status high */
#define SOFTACK		0x20	/* Software Interrupt Acknowledge */
#define NORESET		0	/* No reset on write to R9 */
#define CHRB		0x40	/* Reset channel B */
#define CHRA		0x80	/* Reset channel A */
#define FHWRES		0xc0	/* Force hardware reset */

/* Write Register 10 (Miscellaneous Transmitter/Receiver Control Bits) */
#define BIT6		1	/* 6 bit/8bit sync */
#define LOOPMODE	2	/* SDLC Loop mode */
#define ABUNDER		4	/* Abort/flag on SDLC xmit underrun */
#define MARKIDLE	8	/* Mark/flag on idle */
#define GAOP		0x10	/* Go active on poll */
#define NRZ		0	/* NRZ mode */
#define NRZI		0x20	/* NRZI mode */
#define FM1		0x40	/* FM1 (transition = 1) */
#define FM0		0x60	/* FM0 (transition = 0) */
#define CRCPS		0x80	/* CRC Preset I/O */

/* Write Register 11 (Clock Mode Control) */
#define TRxCXT		0	/* TRxC = Xtal output */
#define TRxCTC		1	/* TRxC = Transmit clock */
#define TRxCBR		2	/* TRxC = BR Generator Output */
#define TRxCDP		3	/* TRxC = DPLL output */
#define TRxCOI		4	/* TRxC O/I */
#define TCRTxCP		0	/* Transmit clock = RTxC pin */
#define TCTRxCP		8	/* Transmit clock = TRxC pin */
#define TCBR		0x10	/* Transmit clock = BR Generator output */
#define TCDPLL		0x18	/* Transmit clock = DPLL output */
#define RCRTxCP		0	/* Receive clock = RTxC pin */
#define RCTRxCP		0x20	/* Receive clock = TRxC pin */
#define RCBR		0x40	/* Receive clock = BR Generator output */
#define RCDPLL		0x60	/* Receive clock = DPLL output */
#define RTxCX		0x80	/* RTxC Xtal/No Xtal */

/* Write Register 12 (Lower Byte of Baud Rate Generator Time Constant) */

/* Write Register 13 (Upper Byte of Baud Rate Generator Time Constant) */

/* Write Register 14 (Miscellaneous Control Bits) */
#define BRENABL		1	/* Baud rate generator enable */
#define BRSRC		2	/* Baud rate generator source */
#define DTRREQ		4	/* DTR/Request function */
#define AUTOECHO	8	/* Auto Echo */
#define LOOPBAK		0x10	/* Local loopback */
#define SEARCH		0x20	/* Enter search mode */
#define RMC		0x40	/* Reset missing clock */
#define DISDPLL		0x60	/* Disable DPLL */
#define SSBR		0x80	/* Set DPLL source = BR generator */
#define SSRTxC		0xa0	/* Set DPLL source = RTxC */
#define SFMM		0xc0	/* Set FM mode */
#define SNRZI		0xe0	/* Set NRZI mode */

/* Write Register 15 (External/Status Interrupt Control) */
#define WR7P_EN		1	/* WR7 Prime SDLC Feature Enable */
#define ZCIE		2	/* Zero count IE */
#define DCDIE		8	/* DCD IE */
#define SYNCIE		0x10	/* Sync/hunt IE */
#define CTSIE		0x20	/* CTS IE */
#define TxUIE		0x40	/* Tx Underrun/EOM IE */
#define BRKIE		0x80	/* Break/Abort IE */


/* Read Register 0 (Transmit/Receive Buffer Status and External Status) */
#define Rx_CH_AV	0x1	/* Rx Character Available */
#define ZCOUNT		0x2	/* Zero count */
#define Tx_BUF_EMP	0x4	/* Tx Buffer empty */
#define DCD		0x8	/* DCD */
#define SYNC_HUNT	0x10	/* Sync/hunt */
#define CTS		0x20	/* CTS */
#define TxEOM		0x40	/* Tx underrun */
#define BRK_ABRT	0x80	/* Break/Abort */

/* Read Register 1 (Special Receive Condition Status) */
#define ALL_SNT		0x1	/* All sent */
/* Residue Data for 8 Rx bits/char programmed */
#define RES3		0x8	/* 0/3 */
#define RES4		0x4	/* 0/4 */
#define RES5		0xc	/* 0/5 */
#define RES6		0x2	/* 0/6 */
#define RES7		0xa	/* 0/7 */
#define RES8		0x6	/* 0/8 */
#define RES18		0xe	/* 1/8 */
#define RES28		0x0	/* 2/8 */
/* Special Rx Condition Interrupts */
#define PAR_ERR		0x10	/* Parity Error */
#define Rx_OVR		0x20	/* Rx Overrun Error */
#define FRM_ERR		0x40	/* CRC/Framing Error */
#define END_FR		0x80	/* End of Frame (SDLC) */

/* Read Register 2 (Interrupt Vector (WR2) -- channel A).  */

/* Read Register 2 (Modified Interrupt Vector -- channel B).  */

/* Read Register 3 (Interrupt Pending Bits -- channel A only).  */
#define CHBEXT		0x1	/* Channel B Ext/Stat IP */
#define CHBTxIP		0x2	/* Channel B Tx IP */
#define CHBRxIP		0x4	/* Channel B Rx IP */
#define CHAEXT		0x8	/* Channel A Ext/Stat IP */
#define CHATxIP		0x10	/* Channel A Tx IP */
#define CHARxIP		0x20	/* Channel A Rx IP */

/* Read Register 6 (SDLC FIFO Status and Byte Count LSB) */

/* Read Register 7 (SDLC FIFO Status and Byte Count MSB) */

/* Read Register 8 (Receive Data) */

/* Read Register 10 (Miscellaneous Status Bits) */
#define ONLOOP		2	/* On loop */
#define LOOPSEND	0x10	/* Loop sending */
#define CLK2MIS		0x40	/* Two clocks missing */
#define CLK1MIS		0x80	/* One clock missing */

/* Read Register 12 (Lower Byte of Baud Rate Generator Constant (WR12)) */

/* Read Register 13 (Upper Byte of Baud Rate Generator Constant (WR13) */

/* Read Register 15 (External/Status Interrupt Control (WR15)) */

#endif /* _SERIAL_ZS_H */
