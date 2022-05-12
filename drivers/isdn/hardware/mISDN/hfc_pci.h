/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  specific defines for CCD's HFC 2BDS0 PCI chips
 *
 * Author     Werner Cornelius (werner@isdn4linux.de)
 *
 * Copyright 1999  by Werner Cornelius (werner@isdn4linux.de)
 */

/*
 * thresholds for transparent B-channel mode
 * change mask and threshold simultaneously
 */
#define HFCPCI_BTRANS_THRESHOLD 128
#define HFCPCI_FILLEMPTY	64
#define HFCPCI_BTRANS_THRESMASK 0x00

/* defines for PCI config */
#define PCI_ENA_MEMIO		0x02
#define PCI_ENA_MASTER		0x04

/* GCI/IOM bus monitor registers */
#define HCFPCI_C_I		0x08
#define HFCPCI_TRxR		0x0C
#define HFCPCI_MON1_D		0x28
#define HFCPCI_MON2_D		0x2C

/* GCI/IOM bus timeslot registers */
#define HFCPCI_B1_SSL		0x80
#define HFCPCI_B2_SSL		0x84
#define HFCPCI_AUX1_SSL		0x88
#define HFCPCI_AUX2_SSL		0x8C
#define HFCPCI_B1_RSL		0x90
#define HFCPCI_B2_RSL		0x94
#define HFCPCI_AUX1_RSL		0x98
#define HFCPCI_AUX2_RSL		0x9C

/* GCI/IOM bus data registers */
#define HFCPCI_B1_D		0xA0
#define HFCPCI_B2_D		0xA4
#define HFCPCI_AUX1_D		0xA8
#define HFCPCI_AUX2_D		0xAC

/* GCI/IOM bus configuration registers */
#define HFCPCI_MST_EMOD		0xB4
#define HFCPCI_MST_MODE		0xB8
#define HFCPCI_CONNECT		0xBC


/* Interrupt and status registers */
#define HFCPCI_FIFO_EN		0x44
#define HFCPCI_TRM		0x48
#define HFCPCI_B_MODE		0x4C
#define HFCPCI_CHIP_ID		0x58
#define HFCPCI_CIRM		0x60
#define HFCPCI_CTMT		0x64
#define HFCPCI_INT_M1		0x68
#define HFCPCI_INT_M2		0x6C
#define HFCPCI_INT_S1		0x78
#define HFCPCI_INT_S2		0x7C
#define HFCPCI_STATUS		0x70

/* S/T section registers */
#define HFCPCI_STATES		0xC0
#define HFCPCI_SCTRL		0xC4
#define HFCPCI_SCTRL_E		0xC8
#define HFCPCI_SCTRL_R		0xCC
#define HFCPCI_SQ		0xD0
#define HFCPCI_CLKDEL		0xDC
#define HFCPCI_B1_REC		0xF0
#define HFCPCI_B1_SEND		0xF0
#define HFCPCI_B2_REC		0xF4
#define HFCPCI_B2_SEND		0xF4
#define HFCPCI_D_REC		0xF8
#define HFCPCI_D_SEND		0xF8
#define HFCPCI_E_REC		0xFC


/* bits in status register (READ) */
#define HFCPCI_PCI_PROC		0x02
#define HFCPCI_NBUSY		0x04
#define HFCPCI_TIMER_ELAP	0x10
#define HFCPCI_STATINT		0x20
#define HFCPCI_FRAMEINT		0x40
#define HFCPCI_ANYINT		0x80

/* bits in CTMT (Write) */
#define HFCPCI_CLTIMER		0x80
#define HFCPCI_TIM3_125		0x04
#define HFCPCI_TIM25		0x10
#define HFCPCI_TIM50		0x14
#define HFCPCI_TIM400		0x18
#define HFCPCI_TIM800		0x1C
#define HFCPCI_AUTO_TIMER	0x20
#define HFCPCI_TRANSB2		0x02
#define HFCPCI_TRANSB1		0x01

/* bits in CIRM (Write) */
#define HFCPCI_AUX_MSK		0x07
#define HFCPCI_RESET		0x08
#define HFCPCI_B1_REV		0x40
#define HFCPCI_B2_REV		0x80

/* bits in INT_M1 and INT_S1 */
#define HFCPCI_INTS_B1TRANS	0x01
#define HFCPCI_INTS_B2TRANS	0x02
#define HFCPCI_INTS_DTRANS	0x04
#define HFCPCI_INTS_B1REC	0x08
#define HFCPCI_INTS_B2REC	0x10
#define HFCPCI_INTS_DREC	0x20
#define HFCPCI_INTS_L1STATE	0x40
#define HFCPCI_INTS_TIMER	0x80

/* bits in INT_M2 */
#define HFCPCI_PROC_TRANS	0x01
#define HFCPCI_GCI_I_CHG	0x02
#define HFCPCI_GCI_MON_REC	0x04
#define HFCPCI_IRQ_ENABLE	0x08
#define HFCPCI_PMESEL		0x80

/* bits in STATES */
#define HFCPCI_STATE_MSK	0x0F
#define HFCPCI_LOAD_STATE	0x10
#define HFCPCI_ACTIVATE		0x20
#define HFCPCI_DO_ACTION	0x40
#define HFCPCI_NT_G2_G3		0x80

/* bits in HFCD_MST_MODE */
#define HFCPCI_MASTER		0x01
#define HFCPCI_SLAVE		0x00
#define HFCPCI_F0IO_POSITIV	0x02
#define HFCPCI_F0_NEGATIV	0x04
#define HFCPCI_F0_2C4		0x08
/* remaining bits are for codecs control */

/* bits in HFCD_SCTRL */
#define SCTRL_B1_ENA		0x01
#define SCTRL_B2_ENA		0x02
#define SCTRL_MODE_TE		0x00
#define SCTRL_MODE_NT		0x04
#define SCTRL_LOW_PRIO		0x08
#define SCTRL_SQ_ENA		0x10
#define SCTRL_TEST		0x20
#define SCTRL_NONE_CAP		0x40
#define SCTRL_PWR_DOWN		0x80

/* bits in SCTRL_E  */
#define HFCPCI_AUTO_AWAKE	0x01
#define HFCPCI_DBIT_1		0x04
#define HFCPCI_IGNORE_COL	0x08
#define HFCPCI_CHG_B1_B2	0x80

/* bits in FIFO_EN register */
#define HFCPCI_FIFOEN_B1	0x03
#define HFCPCI_FIFOEN_B2	0x0C
#define HFCPCI_FIFOEN_DTX	0x10
#define HFCPCI_FIFOEN_B1TX	0x01
#define HFCPCI_FIFOEN_B1RX	0x02
#define HFCPCI_FIFOEN_B2TX	0x04
#define HFCPCI_FIFOEN_B2RX	0x08


/* definitions of fifo memory area */
#define MAX_D_FRAMES 15
#define MAX_B_FRAMES 31
#define B_SUB_VAL    0x200
#define B_FIFO_SIZE  (0x2000 - B_SUB_VAL)
#define D_FIFO_SIZE  512
#define D_FREG_MASK  0xF

struct zt {
	__le16 z1;  /* Z1 pointer 16 Bit */
	__le16 z2;  /* Z2 pointer 16 Bit */
};

struct dfifo {
	u_char data[D_FIFO_SIZE]; /* FIFO data space */
	u_char fill1[0x20A0 - D_FIFO_SIZE]; /* reserved, do not use */
	u_char f1, f2; /* f pointers */
	u_char fill2[0x20C0 - 0x20A2]; /* reserved, do not use */
	/* mask index with D_FREG_MASK for access */
	struct zt za[MAX_D_FRAMES + 1];
	u_char fill3[0x4000 - 0x2100]; /* align 16K */
};

struct bzfifo {
	struct zt	za[MAX_B_FRAMES + 1]; /* only range 0x0..0x1F allowed */
	u_char		f1, f2; /* f pointers */
	u_char		fill[0x2100 - 0x2082]; /* alignment */
};


union fifo_area {
	struct {
		struct dfifo d_tx; /* D-send channel */
		struct dfifo d_rx; /* D-receive channel */
	} d_chan;
	struct {
		u_char		fill1[0x200];
		u_char		txdat_b1[B_FIFO_SIZE];
		struct bzfifo	txbz_b1;
		struct bzfifo	txbz_b2;
		u_char		txdat_b2[B_FIFO_SIZE];
		u_char		fill2[D_FIFO_SIZE];
		u_char		rxdat_b1[B_FIFO_SIZE];
		struct bzfifo	rxbz_b1;
		struct bzfifo	rxbz_b2;
		u_char rxdat_b2[B_FIFO_SIZE];
	} b_chans;
	u_char fill[32768];
};

#define Write_hfc(a, b, c) (writeb(c, (a->hw.pci_io) + b))
#define Read_hfc(a, b) (readb((a->hw.pci_io) + b))
