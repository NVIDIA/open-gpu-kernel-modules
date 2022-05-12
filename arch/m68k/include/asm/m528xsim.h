/* SPDX-License-Identifier: GPL-2.0 */
/****************************************************************************/

/*
 *	m528xsim.h -- ColdFire 5280/5282 System Integration Module support.
 *
 *	(C) Copyright 2003, Greg Ungerer (gerg@snapgear.com)
 */

/****************************************************************************/
#ifndef	m528xsim_h
#define	m528xsim_h
/****************************************************************************/

#define	CPU_NAME		"COLDFIRE(m528x)"
#define	CPU_INSTR_PER_JIFFY	3
#define	MCF_BUSCLK		MCF_CLK

#include <asm/m52xxacr.h>

/*
 *	Define the 5280/5282 SIM register set addresses.
 */
#define	MCFICM_INTC0		(MCF_IPSBAR + 0x0c00)	/* Base for Interrupt Ctrl 0 */
#define	MCFICM_INTC1		(MCF_IPSBAR + 0x0d00)	/* Base for Interrupt Ctrl 0 */

#define	MCFINTC_IPRH		0x00		/* Interrupt pending 32-63 */
#define	MCFINTC_IPRL		0x04		/* Interrupt pending 1-31 */
#define	MCFINTC_IMRH		0x08		/* Interrupt mask 32-63 */
#define	MCFINTC_IMRL		0x0c		/* Interrupt mask 1-31 */
#define	MCFINTC_INTFRCH		0x10		/* Interrupt force 32-63 */
#define	MCFINTC_INTFRCL		0x14		/* Interrupt force 1-31 */
#define	MCFINTC_IRLR		0x18		/* */
#define	MCFINTC_IACKL		0x19		/* */
#define	MCFINTC_ICR0		0x40		/* Base ICR register */

#define	MCFINT_VECBASE		64		/* Vector base number */
#define	MCFINT_UART0		13		/* Interrupt number for UART0 */
#define	MCFINT_UART1		14		/* Interrupt number for UART1 */
#define	MCFINT_UART2		15		/* Interrupt number for UART2 */
#define	MCFINT_I2C0		17		/* Interrupt number for I2C */
#define	MCFINT_QSPI		18		/* Interrupt number for QSPI */
#define	MCFINT_FECRX0		23		/* Interrupt number for FEC */
#define	MCFINT_FECTX0		27		/* Interrupt number for FEC */
#define	MCFINT_FECENTC0		29		/* Interrupt number for FEC */
#define	MCFINT_PIT1		55		/* Interrupt number for PIT1 */

#define	MCF_IRQ_UART0	        (MCFINT_VECBASE + MCFINT_UART0)
#define	MCF_IRQ_UART1	        (MCFINT_VECBASE + MCFINT_UART1)
#define	MCF_IRQ_UART2	        (MCFINT_VECBASE + MCFINT_UART2)

#define	MCF_IRQ_FECRX0		(MCFINT_VECBASE + MCFINT_FECRX0)
#define	MCF_IRQ_FECTX0		(MCFINT_VECBASE + MCFINT_FECTX0)
#define	MCF_IRQ_FECENTC0	(MCFINT_VECBASE + MCFINT_FECENTC0)

#define	MCF_IRQ_QSPI		(MCFINT_VECBASE + MCFINT_QSPI)
#define MCF_IRQ_PIT1		(MCFINT_VECBASE + MCFINT_PIT1)
#define	MCF_IRQ_I2C0		(MCFINT_VECBASE + MCFINT_I2C0)

/*
 *	SDRAM configuration registers.
 */
#define	MCFSIM_DCR		(MCF_IPSBAR + 0x00000044) /* Control */
#define	MCFSIM_DACR0		(MCF_IPSBAR + 0x00000048) /* Base address 0 */
#define	MCFSIM_DMR0		(MCF_IPSBAR + 0x0000004c) /* Address mask 0 */
#define	MCFSIM_DACR1		(MCF_IPSBAR + 0x00000050) /* Base address 1 */
#define	MCFSIM_DMR1		(MCF_IPSBAR + 0x00000054) /* Address mask 1 */

/*
 *	DMA unit base addresses.
 */
#define	MCFDMA_BASE0		(MCF_IPSBAR + 0x00000100)
#define	MCFDMA_BASE1		(MCF_IPSBAR + 0x00000140)
#define	MCFDMA_BASE2		(MCF_IPSBAR + 0x00000180)
#define	MCFDMA_BASE3		(MCF_IPSBAR + 0x000001C0)

/*
 *	UART module.
 */
#define	MCFUART_BASE0		(MCF_IPSBAR + 0x00000200)
#define	MCFUART_BASE1		(MCF_IPSBAR + 0x00000240)
#define	MCFUART_BASE2		(MCF_IPSBAR + 0x00000280)

/*
 *	FEC ethernet module.
 */
#define	MCFFEC_BASE0		(MCF_IPSBAR + 0x00001000)
#define	MCFFEC_SIZE0		0x800

/*
 *	QSPI module.
 */
#define	MCFQSPI_BASE		(MCF_IPSBAR + 0x340)
#define	MCFQSPI_SIZE		0x40

#define	MCFQSPI_CS0		147
#define	MCFQSPI_CS1		148
#define	MCFQSPI_CS2		149
#define	MCFQSPI_CS3		150

/*
 * 	GPIO registers
 */
#define MCFGPIO_PODR_A		(MCF_IPSBAR + 0x00100000)
#define MCFGPIO_PODR_B		(MCF_IPSBAR + 0x00100001)
#define MCFGPIO_PODR_C		(MCF_IPSBAR + 0x00100002)
#define MCFGPIO_PODR_D		(MCF_IPSBAR + 0x00100003)
#define MCFGPIO_PODR_E		(MCF_IPSBAR + 0x00100004)
#define MCFGPIO_PODR_F		(MCF_IPSBAR + 0x00100005)
#define MCFGPIO_PODR_G		(MCF_IPSBAR + 0x00100006)
#define MCFGPIO_PODR_H		(MCF_IPSBAR + 0x00100007)
#define MCFGPIO_PODR_J		(MCF_IPSBAR + 0x00100008)
#define MCFGPIO_PODR_DD		(MCF_IPSBAR + 0x00100009)
#define MCFGPIO_PODR_EH		(MCF_IPSBAR + 0x0010000A)
#define MCFGPIO_PODR_EL		(MCF_IPSBAR + 0x0010000B)
#define MCFGPIO_PODR_AS		(MCF_IPSBAR + 0x0010000C)
#define MCFGPIO_PODR_QS		(MCF_IPSBAR + 0x0010000D)
#define MCFGPIO_PODR_SD		(MCF_IPSBAR + 0x0010000E)
#define MCFGPIO_PODR_TC		(MCF_IPSBAR + 0x0010000F)
#define MCFGPIO_PODR_TD		(MCF_IPSBAR + 0x00100010)
#define MCFGPIO_PODR_UA		(MCF_IPSBAR + 0x00100011)

#define MCFGPIO_PDDR_A		(MCF_IPSBAR + 0x00100014)
#define MCFGPIO_PDDR_B		(MCF_IPSBAR + 0x00100015)
#define MCFGPIO_PDDR_C		(MCF_IPSBAR + 0x00100016)
#define MCFGPIO_PDDR_D		(MCF_IPSBAR + 0x00100017)
#define MCFGPIO_PDDR_E		(MCF_IPSBAR + 0x00100018)
#define MCFGPIO_PDDR_F		(MCF_IPSBAR + 0x00100019)
#define MCFGPIO_PDDR_G		(MCF_IPSBAR + 0x0010001A)
#define MCFGPIO_PDDR_H		(MCF_IPSBAR + 0x0010001B)
#define MCFGPIO_PDDR_J		(MCF_IPSBAR + 0x0010001C)
#define MCFGPIO_PDDR_DD		(MCF_IPSBAR + 0x0010001D)
#define MCFGPIO_PDDR_EH		(MCF_IPSBAR + 0x0010001E)
#define MCFGPIO_PDDR_EL		(MCF_IPSBAR + 0x0010001F)
#define MCFGPIO_PDDR_AS		(MCF_IPSBAR + 0x00100020)
#define MCFGPIO_PDDR_QS		(MCF_IPSBAR + 0x00100021)
#define MCFGPIO_PDDR_SD		(MCF_IPSBAR + 0x00100022)
#define MCFGPIO_PDDR_TC		(MCF_IPSBAR + 0x00100023)
#define MCFGPIO_PDDR_TD		(MCF_IPSBAR + 0x00100024)
#define MCFGPIO_PDDR_UA		(MCF_IPSBAR + 0x00100025)

#define MCFGPIO_PPDSDR_A	(MCF_IPSBAR + 0x00100028)
#define MCFGPIO_PPDSDR_B	(MCF_IPSBAR + 0x00100029)
#define MCFGPIO_PPDSDR_C	(MCF_IPSBAR + 0x0010002A)
#define MCFGPIO_PPDSDR_D	(MCF_IPSBAR + 0x0010002B)
#define MCFGPIO_PPDSDR_E	(MCF_IPSBAR + 0x0010002C)
#define MCFGPIO_PPDSDR_F	(MCF_IPSBAR + 0x0010002D)
#define MCFGPIO_PPDSDR_G	(MCF_IPSBAR + 0x0010002E)
#define MCFGPIO_PPDSDR_H	(MCF_IPSBAR + 0x0010002F)
#define MCFGPIO_PPDSDR_J	(MCF_IPSBAR + 0x00100030)
#define MCFGPIO_PPDSDR_DD	(MCF_IPSBAR + 0x00100031)
#define MCFGPIO_PPDSDR_EH	(MCF_IPSBAR + 0x00100032)
#define MCFGPIO_PPDSDR_EL	(MCF_IPSBAR + 0x00100033)
#define MCFGPIO_PPDSDR_AS	(MCF_IPSBAR + 0x00100034)
#define MCFGPIO_PPDSDR_QS	(MCF_IPSBAR + 0x00100035)
#define MCFGPIO_PPDSDR_SD	(MCF_IPSBAR + 0x00100036)
#define MCFGPIO_PPDSDR_TC	(MCF_IPSBAR + 0x00100037)
#define MCFGPIO_PPDSDR_TD	(MCF_IPSBAR + 0x00100038)
#define MCFGPIO_PPDSDR_UA	(MCF_IPSBAR + 0x00100039)

#define MCFGPIO_PCLRR_A		(MCF_IPSBAR + 0x0010003C)
#define MCFGPIO_PCLRR_B		(MCF_IPSBAR + 0x0010003D)
#define MCFGPIO_PCLRR_C		(MCF_IPSBAR + 0x0010003E)
#define MCFGPIO_PCLRR_D		(MCF_IPSBAR + 0x0010003F)
#define MCFGPIO_PCLRR_E		(MCF_IPSBAR + 0x00100040)
#define MCFGPIO_PCLRR_F		(MCF_IPSBAR + 0x00100041)
#define MCFGPIO_PCLRR_G		(MCF_IPSBAR + 0x00100042)
#define MCFGPIO_PCLRR_H		(MCF_IPSBAR + 0x00100043)
#define MCFGPIO_PCLRR_J		(MCF_IPSBAR + 0x00100044)
#define MCFGPIO_PCLRR_DD	(MCF_IPSBAR + 0x00100045)
#define MCFGPIO_PCLRR_EH	(MCF_IPSBAR + 0x00100046)
#define MCFGPIO_PCLRR_EL	(MCF_IPSBAR + 0x00100047)
#define MCFGPIO_PCLRR_AS	(MCF_IPSBAR + 0x00100048)
#define MCFGPIO_PCLRR_QS	(MCF_IPSBAR + 0x00100049)
#define MCFGPIO_PCLRR_SD	(MCF_IPSBAR + 0x0010004A)
#define MCFGPIO_PCLRR_TC	(MCF_IPSBAR + 0x0010004B)
#define MCFGPIO_PCLRR_TD	(MCF_IPSBAR + 0x0010004C)
#define MCFGPIO_PCLRR_UA	(MCF_IPSBAR + 0x0010004D)

#define MCFGPIO_PBCDPAR		(MCF_IPSBAR + 0x00100050)
#define MCFGPIO_PFPAR		(MCF_IPSBAR + 0x00100051)
#define MCFGPIO_PEPAR		(MCF_IPSBAR + 0x00100052)
#define MCFGPIO_PJPAR		(MCF_IPSBAR + 0x00100054)
#define MCFGPIO_PSDPAR		(MCF_IPSBAR + 0x00100055)
#define MCFGPIO_PASPAR		(MCF_IPSBAR + 0x00100056)
#define MCFGPIO_PEHLPAR		(MCF_IPSBAR + 0x00100058)
#define MCFGPIO_PQSPAR		(MCF_IPSBAR + 0x00100059)
#define MCFGPIO_PTCPAR		(MCF_IPSBAR + 0x0010005A)
#define MCFGPIO_PTDPAR		(MCF_IPSBAR + 0x0010005B)
#define MCFGPIO_PUAPAR		(MCF_IPSBAR + 0x0010005C)

/*
 * PIT timer base addresses.
 */
#define	MCFPIT_BASE1		(MCF_IPSBAR + 0x00150000)
#define	MCFPIT_BASE2		(MCF_IPSBAR + 0x00160000)
#define	MCFPIT_BASE3		(MCF_IPSBAR + 0x00170000)
#define	MCFPIT_BASE4		(MCF_IPSBAR + 0x00180000)

/*
 * 	Edge Port registers
 */
#define MCFEPORT_EPPAR		(MCF_IPSBAR + 0x00130000)
#define MCFEPORT_EPDDR		(MCF_IPSBAR + 0x00130002)
#define MCFEPORT_EPIER		(MCF_IPSBAR + 0x00130003)
#define MCFEPORT_EPDR		(MCF_IPSBAR + 0x00130004)
#define MCFEPORT_EPPDR		(MCF_IPSBAR + 0x00130005)
#define MCFEPORT_EPFR		(MCF_IPSBAR + 0x00130006)

/*
 * 	Queued ADC registers
 */
#define MCFQADC_PORTQA		(MCF_IPSBAR + 0x00190006)
#define MCFQADC_PORTQB		(MCF_IPSBAR + 0x00190007)
#define MCFQADC_DDRQA		(MCF_IPSBAR + 0x00190008)
#define MCFQADC_DDRQB		(MCF_IPSBAR + 0x00190009)

/*
 * 	General Purpose Timers registers
 */
#define MCFGPTA_GPTPORT		(MCF_IPSBAR + 0x001A001D)
#define MCFGPTA_GPTDDR		(MCF_IPSBAR + 0x001A001E)
#define MCFGPTB_GPTPORT		(MCF_IPSBAR + 0x001B001D)
#define MCFGPTB_GPTDDR		(MCF_IPSBAR + 0x001B001E)
/*
 *
 * definitions for generic gpio support
 *
 */
#define MCFGPIO_PODR		MCFGPIO_PODR_A	/* port output data */
#define MCFGPIO_PDDR		MCFGPIO_PDDR_A	/* port data direction */
#define MCFGPIO_PPDR		MCFGPIO_PPDSDR_A/* port pin data */
#define MCFGPIO_SETR		MCFGPIO_PPDSDR_A/* set output */
#define MCFGPIO_CLRR		MCFGPIO_PCLRR_A	/* clr output */

#define MCFGPIO_IRQ_MAX		8
#define MCFGPIO_IRQ_VECBASE	MCFINT_VECBASE
#define MCFGPIO_PIN_MAX		180

/*
 *  Reset Control Unit (relative to IPSBAR).
 */
#define	MCF_RCR			(MCF_IPSBAR + 0x110000)
#define	MCF_RSR			(MCF_IPSBAR + 0x110001)

#define	MCF_RCR_SWRESET		0x80		/* Software reset bit */
#define	MCF_RCR_FRCSTOUT	0x40		/* Force external reset */

/*
 * I2C module
 */
#define	MCFI2C_BASE0		(MCF_IPSBAR + 0x300)
#define	MCFI2C_SIZE0		0x40

/****************************************************************************/
#endif	/* m528xsim_h */
