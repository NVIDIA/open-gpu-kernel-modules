/* SPDX-License-Identifier: GPL-2.0-only */
/*******************************************************************************
  DWMAC DMA Header file.

  Copyright (C) 2007-2009  STMicroelectronics Ltd


  Author: Giuseppe Cavallaro <peppe.cavallaro@st.com>
*******************************************************************************/

#ifndef __DWMAC_DMA_H__
#define __DWMAC_DMA_H__

/* DMA CRS Control and Status Register Mapping */
#define DMA_BUS_MODE		0x00001000	/* Bus Mode */
#define DMA_XMT_POLL_DEMAND	0x00001004	/* Transmit Poll Demand */
#define DMA_RCV_POLL_DEMAND	0x00001008	/* Received Poll Demand */
#define DMA_RCV_BASE_ADDR	0x0000100c	/* Receive List Base */
#define DMA_TX_BASE_ADDR	0x00001010	/* Transmit List Base */
#define DMA_STATUS		0x00001014	/* Status Register */
#define DMA_CONTROL		0x00001018	/* Ctrl (Operational Mode) */
#define DMA_INTR_ENA		0x0000101c	/* Interrupt Enable */
#define DMA_MISSED_FRAME_CTR	0x00001020	/* Missed Frame Counter */

/* SW Reset */
#define DMA_BUS_MODE_SFT_RESET	0x00000001	/* Software Reset */

/* Rx watchdog register */
#define DMA_RX_WATCHDOG		0x00001024

/* AXI Master Bus Mode */
#define DMA_AXI_BUS_MODE	0x00001028

#define DMA_AXI_EN_LPI		BIT(31)
#define DMA_AXI_LPI_XIT_FRM	BIT(30)
#define DMA_AXI_WR_OSR_LMT	GENMASK(23, 20)
#define DMA_AXI_WR_OSR_LMT_SHIFT	20
#define DMA_AXI_WR_OSR_LMT_MASK	0xf
#define DMA_AXI_RD_OSR_LMT	GENMASK(19, 16)
#define DMA_AXI_RD_OSR_LMT_SHIFT	16
#define DMA_AXI_RD_OSR_LMT_MASK	0xf

#define DMA_AXI_OSR_MAX		0xf
#define DMA_AXI_MAX_OSR_LIMIT ((DMA_AXI_OSR_MAX << DMA_AXI_WR_OSR_LMT_SHIFT) | \
			       (DMA_AXI_OSR_MAX << DMA_AXI_RD_OSR_LMT_SHIFT))
#define	DMA_AXI_1KBBE		BIT(13)
#define DMA_AXI_AAL		BIT(12)
#define DMA_AXI_BLEN256		BIT(7)
#define DMA_AXI_BLEN128		BIT(6)
#define DMA_AXI_BLEN64		BIT(5)
#define DMA_AXI_BLEN32		BIT(4)
#define DMA_AXI_BLEN16		BIT(3)
#define DMA_AXI_BLEN8		BIT(2)
#define DMA_AXI_BLEN4		BIT(1)
#define DMA_BURST_LEN_DEFAULT	(DMA_AXI_BLEN256 | DMA_AXI_BLEN128 | \
				 DMA_AXI_BLEN64 | DMA_AXI_BLEN32 | \
				 DMA_AXI_BLEN16 | DMA_AXI_BLEN8 | \
				 DMA_AXI_BLEN4)

#define DMA_AXI_UNDEF		BIT(0)

#define DMA_AXI_BURST_LEN_MASK	0x000000FE

#define DMA_CUR_TX_BUF_ADDR	0x00001050	/* Current Host Tx Buffer */
#define DMA_CUR_RX_BUF_ADDR	0x00001054	/* Current Host Rx Buffer */
#define DMA_HW_FEATURE		0x00001058	/* HW Feature Register */

/* DMA Control register defines */
#define DMA_CONTROL_ST		0x00002000	/* Start/Stop Transmission */
#define DMA_CONTROL_SR		0x00000002	/* Start/Stop Receive */

/* DMA Normal interrupt */
#define DMA_INTR_ENA_NIE 0x00010000	/* Normal Summary */
#define DMA_INTR_ENA_TIE 0x00000001	/* Transmit Interrupt */
#define DMA_INTR_ENA_TUE 0x00000004	/* Transmit Buffer Unavailable */
#define DMA_INTR_ENA_RIE 0x00000040	/* Receive Interrupt */
#define DMA_INTR_ENA_ERE 0x00004000	/* Early Receive */

#define DMA_INTR_NORMAL	(DMA_INTR_ENA_NIE | DMA_INTR_ENA_RIE | \
			DMA_INTR_ENA_TIE)

/* DMA Abnormal interrupt */
#define DMA_INTR_ENA_AIE 0x00008000	/* Abnormal Summary */
#define DMA_INTR_ENA_FBE 0x00002000	/* Fatal Bus Error */
#define DMA_INTR_ENA_ETE 0x00000400	/* Early Transmit */
#define DMA_INTR_ENA_RWE 0x00000200	/* Receive Watchdog */
#define DMA_INTR_ENA_RSE 0x00000100	/* Receive Stopped */
#define DMA_INTR_ENA_RUE 0x00000080	/* Receive Buffer Unavailable */
#define DMA_INTR_ENA_UNE 0x00000020	/* Tx Underflow */
#define DMA_INTR_ENA_OVE 0x00000010	/* Receive Overflow */
#define DMA_INTR_ENA_TJE 0x00000008	/* Transmit Jabber */
#define DMA_INTR_ENA_TSE 0x00000002	/* Transmit Stopped */

#define DMA_INTR_ABNORMAL	(DMA_INTR_ENA_AIE | DMA_INTR_ENA_FBE | \
				DMA_INTR_ENA_UNE)

/* DMA default interrupt mask */
#define DMA_INTR_DEFAULT_MASK	(DMA_INTR_NORMAL | DMA_INTR_ABNORMAL)
#define DMA_INTR_DEFAULT_RX	(DMA_INTR_ENA_RIE)
#define DMA_INTR_DEFAULT_TX	(DMA_INTR_ENA_TIE)

/* DMA Status register defines */
#define DMA_STATUS_GLPII	0x40000000	/* GMAC LPI interrupt */
#define DMA_STATUS_GPI		0x10000000	/* PMT interrupt */
#define DMA_STATUS_GMI		0x08000000	/* MMC interrupt */
#define DMA_STATUS_GLI		0x04000000	/* GMAC Line interface int */
#define DMA_STATUS_EB_MASK	0x00380000	/* Error Bits Mask */
#define DMA_STATUS_EB_TX_ABORT	0x00080000	/* Error Bits - TX Abort */
#define DMA_STATUS_EB_RX_ABORT	0x00100000	/* Error Bits - RX Abort */
#define DMA_STATUS_TS_MASK	0x00700000	/* Transmit Process State */
#define DMA_STATUS_TS_SHIFT	20
#define DMA_STATUS_RS_MASK	0x000e0000	/* Receive Process State */
#define DMA_STATUS_RS_SHIFT	17
#define DMA_STATUS_NIS	0x00010000	/* Normal Interrupt Summary */
#define DMA_STATUS_AIS	0x00008000	/* Abnormal Interrupt Summary */
#define DMA_STATUS_ERI	0x00004000	/* Early Receive Interrupt */
#define DMA_STATUS_FBI	0x00002000	/* Fatal Bus Error Interrupt */
#define DMA_STATUS_ETI	0x00000400	/* Early Transmit Interrupt */
#define DMA_STATUS_RWT	0x00000200	/* Receive Watchdog Timeout */
#define DMA_STATUS_RPS	0x00000100	/* Receive Process Stopped */
#define DMA_STATUS_RU	0x00000080	/* Receive Buffer Unavailable */
#define DMA_STATUS_RI	0x00000040	/* Receive Interrupt */
#define DMA_STATUS_UNF	0x00000020	/* Transmit Underflow */
#define DMA_STATUS_OVF	0x00000010	/* Receive Overflow */
#define DMA_STATUS_TJT	0x00000008	/* Transmit Jabber Timeout */
#define DMA_STATUS_TU	0x00000004	/* Transmit Buffer Unavailable */
#define DMA_STATUS_TPS	0x00000002	/* Transmit Process Stopped */
#define DMA_STATUS_TI	0x00000001	/* Transmit Interrupt */
#define DMA_CONTROL_FTF		0x00100000	/* Flush transmit FIFO */

#define DMA_STATUS_MSK_COMMON		(DMA_STATUS_NIS | \
					 DMA_STATUS_AIS | \
					 DMA_STATUS_FBI)

#define DMA_STATUS_MSK_RX		(DMA_STATUS_ERI | \
					 DMA_STATUS_RWT | \
					 DMA_STATUS_RPS | \
					 DMA_STATUS_RU | \
					 DMA_STATUS_RI | \
					 DMA_STATUS_OVF | \
					 DMA_STATUS_MSK_COMMON)

#define DMA_STATUS_MSK_TX		(DMA_STATUS_ETI | \
					 DMA_STATUS_UNF | \
					 DMA_STATUS_TJT | \
					 DMA_STATUS_TU | \
					 DMA_STATUS_TPS | \
					 DMA_STATUS_TI | \
					 DMA_STATUS_MSK_COMMON)

#define NUM_DWMAC100_DMA_REGS	9
#define NUM_DWMAC1000_DMA_REGS	23

void dwmac_enable_dma_transmission(void __iomem *ioaddr);
void dwmac_enable_dma_irq(void __iomem *ioaddr, u32 chan, bool rx, bool tx);
void dwmac_disable_dma_irq(void __iomem *ioaddr, u32 chan, bool rx, bool tx);
void dwmac_dma_start_tx(void __iomem *ioaddr, u32 chan);
void dwmac_dma_stop_tx(void __iomem *ioaddr, u32 chan);
void dwmac_dma_start_rx(void __iomem *ioaddr, u32 chan);
void dwmac_dma_stop_rx(void __iomem *ioaddr, u32 chan);
int dwmac_dma_interrupt(void __iomem *ioaddr, struct stmmac_extra_stats *x,
			u32 chan, u32 dir);
int dwmac_dma_reset(void __iomem *ioaddr);

#endif /* __DWMAC_DMA_H__ */
