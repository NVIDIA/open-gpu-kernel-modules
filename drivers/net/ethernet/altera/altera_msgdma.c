// SPDX-License-Identifier: GPL-2.0-only
/* Altera TSE SGDMA and MSGDMA Linux driver
 * Copyright (C) 2014 Altera Corporation. All rights reserved
 */

#include <linux/netdevice.h>
#include "altera_utils.h"
#include "altera_tse.h"
#include "altera_msgdmahw.h"
#include "altera_msgdma.h"

/* No initialization work to do for MSGDMA */
int msgdma_initialize(struct altera_tse_private *priv)
{
	return 0;
}

void msgdma_uninitialize(struct altera_tse_private *priv)
{
}

void msgdma_start_rxdma(struct altera_tse_private *priv)
{
}

void msgdma_reset(struct altera_tse_private *priv)
{
	int counter;

	/* Reset Rx mSGDMA */
	csrwr32(MSGDMA_CSR_STAT_MASK, priv->rx_dma_csr,
		msgdma_csroffs(status));
	csrwr32(MSGDMA_CSR_CTL_RESET, priv->rx_dma_csr,
		msgdma_csroffs(control));

	counter = 0;
	while (counter++ < ALTERA_TSE_SW_RESET_WATCHDOG_CNTR) {
		if (tse_bit_is_clear(priv->rx_dma_csr, msgdma_csroffs(status),
				     MSGDMA_CSR_STAT_RESETTING))
			break;
		udelay(1);
	}

	if (counter >= ALTERA_TSE_SW_RESET_WATCHDOG_CNTR)
		netif_warn(priv, drv, priv->dev,
			   "TSE Rx mSGDMA resetting bit never cleared!\n");

	/* clear all status bits */
	csrwr32(MSGDMA_CSR_STAT_MASK, priv->rx_dma_csr, msgdma_csroffs(status));

	/* Reset Tx mSGDMA */
	csrwr32(MSGDMA_CSR_STAT_MASK, priv->tx_dma_csr,
		msgdma_csroffs(status));

	csrwr32(MSGDMA_CSR_CTL_RESET, priv->tx_dma_csr,
		msgdma_csroffs(control));

	counter = 0;
	while (counter++ < ALTERA_TSE_SW_RESET_WATCHDOG_CNTR) {
		if (tse_bit_is_clear(priv->tx_dma_csr, msgdma_csroffs(status),
				     MSGDMA_CSR_STAT_RESETTING))
			break;
		udelay(1);
	}

	if (counter >= ALTERA_TSE_SW_RESET_WATCHDOG_CNTR)
		netif_warn(priv, drv, priv->dev,
			   "TSE Tx mSGDMA resetting bit never cleared!\n");

	/* clear all status bits */
	csrwr32(MSGDMA_CSR_STAT_MASK, priv->tx_dma_csr, msgdma_csroffs(status));
}

void msgdma_disable_rxirq(struct altera_tse_private *priv)
{
	tse_clear_bit(priv->rx_dma_csr, msgdma_csroffs(control),
		      MSGDMA_CSR_CTL_GLOBAL_INTR);
}

void msgdma_enable_rxirq(struct altera_tse_private *priv)
{
	tse_set_bit(priv->rx_dma_csr, msgdma_csroffs(control),
		    MSGDMA_CSR_CTL_GLOBAL_INTR);
}

void msgdma_disable_txirq(struct altera_tse_private *priv)
{
	tse_clear_bit(priv->tx_dma_csr, msgdma_csroffs(control),
		      MSGDMA_CSR_CTL_GLOBAL_INTR);
}

void msgdma_enable_txirq(struct altera_tse_private *priv)
{
	tse_set_bit(priv->tx_dma_csr, msgdma_csroffs(control),
		    MSGDMA_CSR_CTL_GLOBAL_INTR);
}

void msgdma_clear_rxirq(struct altera_tse_private *priv)
{
	csrwr32(MSGDMA_CSR_STAT_IRQ, priv->rx_dma_csr, msgdma_csroffs(status));
}

void msgdma_clear_txirq(struct altera_tse_private *priv)
{
	csrwr32(MSGDMA_CSR_STAT_IRQ, priv->tx_dma_csr, msgdma_csroffs(status));
}

/* return 0 to indicate transmit is pending */
int msgdma_tx_buffer(struct altera_tse_private *priv, struct tse_buffer *buffer)
{
	csrwr32(lower_32_bits(buffer->dma_addr), priv->tx_dma_desc,
		msgdma_descroffs(read_addr_lo));
	csrwr32(upper_32_bits(buffer->dma_addr), priv->tx_dma_desc,
		msgdma_descroffs(read_addr_hi));
	csrwr32(0, priv->tx_dma_desc, msgdma_descroffs(write_addr_lo));
	csrwr32(0, priv->tx_dma_desc, msgdma_descroffs(write_addr_hi));
	csrwr32(buffer->len, priv->tx_dma_desc, msgdma_descroffs(len));
	csrwr32(0, priv->tx_dma_desc, msgdma_descroffs(burst_seq_num));
	csrwr32(MSGDMA_DESC_TX_STRIDE, priv->tx_dma_desc,
		msgdma_descroffs(stride));
	csrwr32(MSGDMA_DESC_CTL_TX_SINGLE, priv->tx_dma_desc,
		msgdma_descroffs(control));
	return 0;
}

u32 msgdma_tx_completions(struct altera_tse_private *priv)
{
	u32 ready = 0;
	u32 inuse;
	u32 status;

	/* Get number of sent descriptors */
	inuse = csrrd32(priv->tx_dma_csr, msgdma_csroffs(rw_fill_level))
			& 0xffff;

	if (inuse) { /* Tx FIFO is not empty */
		ready = max_t(int,
			      priv->tx_prod - priv->tx_cons - inuse - 1, 0);
	} else {
		/* Check for buffered last packet */
		status = csrrd32(priv->tx_dma_csr, msgdma_csroffs(status));
		if (status & MSGDMA_CSR_STAT_BUSY)
			ready = priv->tx_prod - priv->tx_cons - 1;
		else
			ready = priv->tx_prod - priv->tx_cons;
	}
	return ready;
}

/* Put buffer to the mSGDMA RX FIFO
 */
void msgdma_add_rx_desc(struct altera_tse_private *priv,
			struct tse_buffer *rxbuffer)
{
	u32 len = priv->rx_dma_buf_sz;
	dma_addr_t dma_addr = rxbuffer->dma_addr;
	u32 control = (MSGDMA_DESC_CTL_END_ON_EOP
			| MSGDMA_DESC_CTL_END_ON_LEN
			| MSGDMA_DESC_CTL_TR_COMP_IRQ
			| MSGDMA_DESC_CTL_EARLY_IRQ
			| MSGDMA_DESC_CTL_TR_ERR_IRQ
			| MSGDMA_DESC_CTL_GO);

	csrwr32(0, priv->rx_dma_desc, msgdma_descroffs(read_addr_lo));
	csrwr32(0, priv->rx_dma_desc, msgdma_descroffs(read_addr_hi));
	csrwr32(lower_32_bits(dma_addr), priv->rx_dma_desc,
		msgdma_descroffs(write_addr_lo));
	csrwr32(upper_32_bits(dma_addr), priv->rx_dma_desc,
		msgdma_descroffs(write_addr_hi));
	csrwr32(len, priv->rx_dma_desc, msgdma_descroffs(len));
	csrwr32(0, priv->rx_dma_desc, msgdma_descroffs(burst_seq_num));
	csrwr32(0x00010001, priv->rx_dma_desc, msgdma_descroffs(stride));
	csrwr32(control, priv->rx_dma_desc, msgdma_descroffs(control));
}

/* status is returned on upper 16 bits,
 * length is returned in lower 16 bits
 */
u32 msgdma_rx_status(struct altera_tse_private *priv)
{
	u32 rxstatus = 0;
	u32 pktlength;
	u32 pktstatus;

	if (csrrd32(priv->rx_dma_csr, msgdma_csroffs(resp_fill_level))
	    & 0xffff) {
		pktlength = csrrd32(priv->rx_dma_resp,
				    msgdma_respoffs(bytes_transferred));
		pktstatus = csrrd32(priv->rx_dma_resp,
				    msgdma_respoffs(status));
		rxstatus = pktstatus;
		rxstatus = rxstatus << 16;
		rxstatus |= (pktlength & 0xffff);
	}
	return rxstatus;
}
