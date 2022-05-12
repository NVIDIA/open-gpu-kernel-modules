/* SPDX-License-Identifier: GPL-2.0
 *
 *	Low-Level PCI Support for SH7780 targets
 *
 *  Dustin McIntire (dustin@sensoria.com) (c) 2001
 *  Paul Mundt (lethal@linux-sh.org) (c) 2003
 */

#ifndef _PCI_SH7780_H_
#define _PCI_SH7780_H_

/* SH7780 Control Registers */
#define	PCIECR			0xFE000008
#define PCIECR_ENBL		0x01

/* SH7780 Specific Values */
#define SH7780_PCI_CONFIG_BASE	0xFD000000	/* Config space base addr */
#define SH7780_PCI_CONFIG_SIZE	0x01000000	/* Config space size */

#define SH7780_PCIREG_BASE	0xFE040000	/* PCI regs base address */

/* SH7780 PCI Config Registers */
#define SH7780_PCIIR		0x114		/* PCI Interrupt Register */
#define SH7780_PCIIMR		0x118		/* PCI Interrupt Mask Register */
#define SH7780_PCIAIR		0x11C		/* Error Address Register */
#define SH7780_PCICIR		0x120		/* Error Command/Data Register */
#define SH7780_PCIAINT		0x130		/* Arbiter Interrupt Register */
#define SH7780_PCIAINTM		0x134		/* Arbiter Int. Mask Register */
#define SH7780_PCIBMIR		0x138		/* Error Bus Master Register */
#define SH7780_PCIPAR		0x1C0		/* PIO Address Register */
#define SH7780_PCIPINT		0x1CC		/* Power Mgmnt Int. Register */
#define SH7780_PCIPINTM		0x1D0		/* Power Mgmnt Mask Register */

#define SH7780_PCIMBR(x)	(0x1E0 + ((x) * 8))
#define SH7780_PCIMBMR(x)	(0x1E4 + ((x) * 8))
#define SH7780_PCIIOBR		0x1F8
#define SH7780_PCIIOBMR		0x1FC
#define SH7780_PCICSCR0		0x210		/* Cache Snoop1 Cnt. Register */
#define SH7780_PCICSCR1		0x214		/* Cache Snoop2 Cnt. Register */
#define SH7780_PCICSAR0		0x218	/* Cache Snoop1 Addr. Register */
#define SH7780_PCICSAR1		0x21C	/* Cache Snoop2 Addr. Register */

#endif /* _PCI_SH7780_H_ */
