/* SPDX-License-Identifier: GPL-2.0 */
/*
 * MUSB OTG driver register defines
 *
 * Copyright 2005 Mentor Graphics Corporation
 * Copyright (C) 2005-2006 by Texas Instruments
 * Copyright (C) 2006-2007 Nokia Corporation
 */

#ifndef __MUSB_REGS_H__
#define __MUSB_REGS_H__

#define MUSB_EP0_FIFOSIZE	64	/* This is non-configurable */

/*
 * MUSB Register bits
 */

/* POWER */
#define MUSB_POWER_ISOUPDATE	0x80
#define MUSB_POWER_SOFTCONN	0x40
#define MUSB_POWER_HSENAB	0x20
#define MUSB_POWER_HSMODE	0x10
#define MUSB_POWER_RESET	0x08
#define MUSB_POWER_RESUME	0x04
#define MUSB_POWER_SUSPENDM	0x02
#define MUSB_POWER_ENSUSPEND	0x01

/* INTRUSB */
#define MUSB_INTR_SUSPEND	0x01
#define MUSB_INTR_RESUME	0x02
#define MUSB_INTR_RESET		0x04
#define MUSB_INTR_BABBLE	0x04
#define MUSB_INTR_SOF		0x08
#define MUSB_INTR_CONNECT	0x10
#define MUSB_INTR_DISCONNECT	0x20
#define MUSB_INTR_SESSREQ	0x40
#define MUSB_INTR_VBUSERROR	0x80	/* For SESSION end */

/* DEVCTL */
#define MUSB_DEVCTL_BDEVICE	0x80
#define MUSB_DEVCTL_FSDEV	0x40
#define MUSB_DEVCTL_LSDEV	0x20
#define MUSB_DEVCTL_VBUS	0x18
#define MUSB_DEVCTL_VBUS_SHIFT	3
#define MUSB_DEVCTL_HM		0x04
#define MUSB_DEVCTL_HR		0x02
#define MUSB_DEVCTL_SESSION	0x01

/* BABBLE_CTL */
#define MUSB_BABBLE_FORCE_TXIDLE	0x80
#define MUSB_BABBLE_SW_SESSION_CTRL	0x40
#define MUSB_BABBLE_STUCK_J		0x20
#define MUSB_BABBLE_RCV_DISABLE		0x04

/* MUSB ULPI VBUSCONTROL */
#define MUSB_ULPI_USE_EXTVBUS	0x01
#define MUSB_ULPI_USE_EXTVBUSIND 0x02
/* ULPI_REG_CONTROL */
#define MUSB_ULPI_REG_REQ	(1 << 0)
#define MUSB_ULPI_REG_CMPLT	(1 << 1)
#define MUSB_ULPI_RDN_WR	(1 << 2)

/* TESTMODE */
#define MUSB_TEST_FORCE_HOST	0x80
#define MUSB_TEST_FIFO_ACCESS	0x40
#define MUSB_TEST_FORCE_FS	0x20
#define MUSB_TEST_FORCE_HS	0x10
#define MUSB_TEST_PACKET	0x08
#define MUSB_TEST_K		0x04
#define MUSB_TEST_J		0x02
#define MUSB_TEST_SE0_NAK	0x01

/* Allocate for double-packet buffering (effectively doubles assigned _SIZE) */
#define MUSB_FIFOSZ_DPB	0x10
/* Allocation size (8, 16, 32, ... 4096) */
#define MUSB_FIFOSZ_SIZE	0x0f

/* CSR0 */
#define MUSB_CSR0_FLUSHFIFO	0x0100
#define MUSB_CSR0_TXPKTRDY	0x0002
#define MUSB_CSR0_RXPKTRDY	0x0001

/* CSR0 in Peripheral mode */
#define MUSB_CSR0_P_SVDSETUPEND	0x0080
#define MUSB_CSR0_P_SVDRXPKTRDY	0x0040
#define MUSB_CSR0_P_SENDSTALL	0x0020
#define MUSB_CSR0_P_SETUPEND	0x0010
#define MUSB_CSR0_P_DATAEND	0x0008
#define MUSB_CSR0_P_SENTSTALL	0x0004

/* CSR0 in Host mode */
#define MUSB_CSR0_H_DIS_PING		0x0800
#define MUSB_CSR0_H_WR_DATATOGGLE	0x0400	/* Set to allow setting: */
#define MUSB_CSR0_H_DATATOGGLE		0x0200	/* Data toggle control */
#define MUSB_CSR0_H_NAKTIMEOUT		0x0080
#define MUSB_CSR0_H_STATUSPKT		0x0040
#define MUSB_CSR0_H_REQPKT		0x0020
#define MUSB_CSR0_H_ERROR		0x0010
#define MUSB_CSR0_H_SETUPPKT		0x0008
#define MUSB_CSR0_H_RXSTALL		0x0004

/* CSR0 bits to avoid zeroing (write zero clears, write 1 ignored) */
#define MUSB_CSR0_P_WZC_BITS	\
	(MUSB_CSR0_P_SENTSTALL)
#define MUSB_CSR0_H_WZC_BITS	\
	(MUSB_CSR0_H_NAKTIMEOUT | MUSB_CSR0_H_RXSTALL \
	| MUSB_CSR0_RXPKTRDY)

/* TxType/RxType */
#define MUSB_TYPE_SPEED		0xc0
#define MUSB_TYPE_SPEED_SHIFT	6
#define MUSB_TYPE_PROTO		0x30	/* Implicitly zero for ep0 */
#define MUSB_TYPE_PROTO_SHIFT	4
#define MUSB_TYPE_REMOTE_END	0xf	/* Implicitly zero for ep0 */

/* CONFIGDATA */
#define MUSB_CONFIGDATA_MPRXE		0x80	/* Auto bulk pkt combining */
#define MUSB_CONFIGDATA_MPTXE		0x40	/* Auto bulk pkt splitting */
#define MUSB_CONFIGDATA_BIGENDIAN	0x20
#define MUSB_CONFIGDATA_HBRXE		0x10	/* HB-ISO for RX */
#define MUSB_CONFIGDATA_HBTXE		0x08	/* HB-ISO for TX */
#define MUSB_CONFIGDATA_DYNFIFO		0x04	/* Dynamic FIFO sizing */
#define MUSB_CONFIGDATA_SOFTCONE	0x02	/* SoftConnect */
#define MUSB_CONFIGDATA_UTMIDW		0x01	/* Data width 0/1 => 8/16bits */

/* TXCSR in Peripheral and Host mode */
#define MUSB_TXCSR_AUTOSET		0x8000
#define MUSB_TXCSR_DMAENAB		0x1000
#define MUSB_TXCSR_FRCDATATOG		0x0800
#define MUSB_TXCSR_DMAMODE		0x0400
#define MUSB_TXCSR_CLRDATATOG		0x0040
#define MUSB_TXCSR_FLUSHFIFO		0x0008
#define MUSB_TXCSR_FIFONOTEMPTY		0x0002
#define MUSB_TXCSR_TXPKTRDY		0x0001

/* TXCSR in Peripheral mode */
#define MUSB_TXCSR_P_ISO		0x4000
#define MUSB_TXCSR_P_INCOMPTX		0x0080
#define MUSB_TXCSR_P_SENTSTALL		0x0020
#define MUSB_TXCSR_P_SENDSTALL		0x0010
#define MUSB_TXCSR_P_UNDERRUN		0x0004

/* TXCSR in Host mode */
#define MUSB_TXCSR_H_WR_DATATOGGLE	0x0200
#define MUSB_TXCSR_H_DATATOGGLE		0x0100
#define MUSB_TXCSR_H_NAKTIMEOUT		0x0080
#define MUSB_TXCSR_H_RXSTALL		0x0020
#define MUSB_TXCSR_H_ERROR		0x0004

/* TXCSR bits to avoid zeroing (write zero clears, write 1 ignored) */
#define MUSB_TXCSR_P_WZC_BITS	\
	(MUSB_TXCSR_P_INCOMPTX | MUSB_TXCSR_P_SENTSTALL \
	| MUSB_TXCSR_P_UNDERRUN | MUSB_TXCSR_FIFONOTEMPTY)
#define MUSB_TXCSR_H_WZC_BITS	\
	(MUSB_TXCSR_H_NAKTIMEOUT | MUSB_TXCSR_H_RXSTALL \
	| MUSB_TXCSR_H_ERROR | MUSB_TXCSR_FIFONOTEMPTY)

/* RXCSR in Peripheral and Host mode */
#define MUSB_RXCSR_AUTOCLEAR		0x8000
#define MUSB_RXCSR_DMAENAB		0x2000
#define MUSB_RXCSR_DISNYET		0x1000
#define MUSB_RXCSR_PID_ERR		0x1000
#define MUSB_RXCSR_DMAMODE		0x0800
#define MUSB_RXCSR_INCOMPRX		0x0100
#define MUSB_RXCSR_CLRDATATOG		0x0080
#define MUSB_RXCSR_FLUSHFIFO		0x0010
#define MUSB_RXCSR_DATAERROR		0x0008
#define MUSB_RXCSR_FIFOFULL		0x0002
#define MUSB_RXCSR_RXPKTRDY		0x0001

/* RXCSR in Peripheral mode */
#define MUSB_RXCSR_P_ISO		0x4000
#define MUSB_RXCSR_P_SENTSTALL		0x0040
#define MUSB_RXCSR_P_SENDSTALL		0x0020
#define MUSB_RXCSR_P_OVERRUN		0x0004

/* RXCSR in Host mode */
#define MUSB_RXCSR_H_AUTOREQ		0x4000
#define MUSB_RXCSR_H_WR_DATATOGGLE	0x0400
#define MUSB_RXCSR_H_DATATOGGLE		0x0200
#define MUSB_RXCSR_H_RXSTALL		0x0040
#define MUSB_RXCSR_H_REQPKT		0x0020
#define MUSB_RXCSR_H_ERROR		0x0004

/* RXCSR bits to avoid zeroing (write zero clears, write 1 ignored) */
#define MUSB_RXCSR_P_WZC_BITS	\
	(MUSB_RXCSR_P_SENTSTALL | MUSB_RXCSR_P_OVERRUN \
	| MUSB_RXCSR_RXPKTRDY)
#define MUSB_RXCSR_H_WZC_BITS	\
	(MUSB_RXCSR_H_RXSTALL | MUSB_RXCSR_H_ERROR \
	| MUSB_RXCSR_DATAERROR | MUSB_RXCSR_RXPKTRDY)

/* HUBADDR */
#define MUSB_HUBADDR_MULTI_TT		0x80


/*
 * Common USB registers
 */

#define MUSB_FADDR		0x00	/* 8-bit */
#define MUSB_POWER		0x01	/* 8-bit */

#define MUSB_INTRTX		0x02	/* 16-bit */
#define MUSB_INTRRX		0x04
#define MUSB_INTRTXE		0x06
#define MUSB_INTRRXE		0x08
#define MUSB_INTRUSB		0x0A	/* 8 bit */
#define MUSB_INTRUSBE		0x0B	/* 8 bit */
#define MUSB_FRAME		0x0C
#define MUSB_INDEX		0x0E	/* 8 bit */
#define MUSB_TESTMODE		0x0F	/* 8 bit */

/*
 * Additional Control Registers
 */

#define MUSB_DEVCTL		0x60	/* 8 bit */
#define MUSB_BABBLE_CTL		0x61	/* 8 bit */

/* These are always controlled through the INDEX register */
#define MUSB_TXFIFOSZ		0x62	/* 8-bit (see masks) */
#define MUSB_RXFIFOSZ		0x63	/* 8-bit (see masks) */
#define MUSB_TXFIFOADD		0x64	/* 16-bit offset shifted right 3 */
#define MUSB_RXFIFOADD		0x66	/* 16-bit offset shifted right 3 */

/* REVISIT: vctrl/vstatus: optional vendor utmi+phy register at 0x68 */
#define MUSB_HWVERS		0x6C	/* 8 bit */
#define MUSB_ULPI_BUSCONTROL	0x70	/* 8 bit */
#define MUSB_ULPI_INT_MASK	0x72	/* 8 bit */
#define MUSB_ULPI_INT_SRC	0x73	/* 8 bit */
#define MUSB_ULPI_REG_DATA	0x74	/* 8 bit */
#define MUSB_ULPI_REG_ADDR	0x75	/* 8 bit */
#define MUSB_ULPI_REG_CONTROL	0x76	/* 8 bit */
#define MUSB_ULPI_RAW_DATA	0x77	/* 8 bit */

#define MUSB_EPINFO		0x78	/* 8 bit */
#define MUSB_RAMINFO		0x79	/* 8 bit */
#define MUSB_LINKINFO		0x7a	/* 8 bit */
#define MUSB_VPLEN		0x7b	/* 8 bit */
#define MUSB_HS_EOF1		0x7c	/* 8 bit */
#define MUSB_FS_EOF1		0x7d	/* 8 bit */
#define MUSB_LS_EOF1		0x7e	/* 8 bit */

/* Offsets to endpoint registers */
#define MUSB_TXMAXP		0x00
#define MUSB_TXCSR		0x02
#define MUSB_CSR0		MUSB_TXCSR	/* Re-used for EP0 */
#define MUSB_RXMAXP		0x04
#define MUSB_RXCSR		0x06
#define MUSB_RXCOUNT		0x08
#define MUSB_COUNT0		MUSB_RXCOUNT	/* Re-used for EP0 */
#define MUSB_TXTYPE		0x0A
#define MUSB_TYPE0		MUSB_TXTYPE	/* Re-used for EP0 */
#define MUSB_TXINTERVAL		0x0B
#define MUSB_NAKLIMIT0		MUSB_TXINTERVAL	/* Re-used for EP0 */
#define MUSB_RXTYPE		0x0C
#define MUSB_RXINTERVAL		0x0D
#define MUSB_FIFOSIZE		0x0F
#define MUSB_CONFIGDATA		MUSB_FIFOSIZE	/* Re-used for EP0 */

#include "tusb6010.h"		/* Needed "only" for TUSB_EP0_CONF */

#define MUSB_TXCSR_MODE			0x2000

/* "bus control"/target registers, for host side multipoint (external hubs) */
#define MUSB_TXFUNCADDR		0x00
#define MUSB_TXHUBADDR		0x02
#define MUSB_TXHUBPORT		0x03

#define MUSB_RXFUNCADDR		0x04
#define MUSB_RXHUBADDR		0x06
#define MUSB_RXHUBPORT		0x07

static inline u8 musb_read_configdata(void __iomem *mbase)
{
	musb_writeb(mbase, MUSB_INDEX, 0);
	return musb_readb(mbase, 0x10 + MUSB_CONFIGDATA);
}

static inline void musb_write_rxfunaddr(struct musb *musb, u8 epnum,
		u8 qh_addr_reg)
{
	musb_writeb(musb->mregs,
		    musb->io.busctl_offset(epnum, MUSB_RXFUNCADDR),
		    qh_addr_reg);
}

static inline void musb_write_rxhubaddr(struct musb *musb, u8 epnum,
		u8 qh_h_addr_reg)
{
	musb_writeb(musb->mregs, musb->io.busctl_offset(epnum, MUSB_RXHUBADDR),
			qh_h_addr_reg);
}

static inline void musb_write_rxhubport(struct musb *musb, u8 epnum,
		u8 qh_h_port_reg)
{
	musb_writeb(musb->mregs, musb->io.busctl_offset(epnum, MUSB_RXHUBPORT),
			qh_h_port_reg);
}

static inline void musb_write_txfunaddr(struct musb *musb, u8 epnum,
		u8 qh_addr_reg)
{
	musb_writeb(musb->mregs,
		    musb->io.busctl_offset(epnum, MUSB_TXFUNCADDR),
		    qh_addr_reg);
}

static inline void musb_write_txhubaddr(struct musb *musb, u8 epnum,
		u8 qh_addr_reg)
{
	musb_writeb(musb->mregs, musb->io.busctl_offset(epnum, MUSB_TXHUBADDR),
			qh_addr_reg);
}

static inline void musb_write_txhubport(struct musb *musb, u8 epnum,
		u8 qh_h_port_reg)
{
	musb_writeb(musb->mregs, musb->io.busctl_offset(epnum, MUSB_TXHUBPORT),
			qh_h_port_reg);
}

static inline u8 musb_read_rxfunaddr(struct musb *musb, u8 epnum)
{
	return musb_readb(musb->mregs,
			  musb->io.busctl_offset(epnum, MUSB_RXFUNCADDR));
}

static inline u8 musb_read_rxhubaddr(struct musb *musb, u8 epnum)
{
	return musb_readb(musb->mregs,
			  musb->io.busctl_offset(epnum, MUSB_RXHUBADDR));
}

static inline u8 musb_read_rxhubport(struct musb *musb, u8 epnum)
{
	return musb_readb(musb->mregs,
			  musb->io.busctl_offset(epnum, MUSB_RXHUBPORT));
}

static inline u8 musb_read_txfunaddr(struct musb *musb, u8 epnum)
{
	return musb_readb(musb->mregs,
			  musb->io.busctl_offset(epnum, MUSB_TXFUNCADDR));
}

static inline u8 musb_read_txhubaddr(struct musb *musb, u8 epnum)
{
	return musb_readb(musb->mregs,
			  musb->io.busctl_offset(epnum, MUSB_TXHUBADDR));
}

static inline u8 musb_read_txhubport(struct musb *musb, u8 epnum)
{
	return musb_readb(musb->mregs,
			  musb->io.busctl_offset(epnum, MUSB_TXHUBPORT));
}

#endif	/* __MUSB_REGS_H__ */
