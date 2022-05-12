// SPDX-License-Identifier: GPL-2.0
/*
 * MUSB OTG driver host support
 *
 * Copyright 2005 Mentor Graphics Corporation
 * Copyright (C) 2005-2006 by Texas Instruments
 * Copyright (C) 2006-2007 Nokia Corporation
 * Copyright (C) 2008-2009 MontaVista Software, Inc. <source@mvista.com>
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/list.h>
#include <linux/dma-mapping.h>

#include "musb_core.h"
#include "musb_host.h"
#include "musb_trace.h"

/* MUSB HOST status 22-mar-2006
 *
 * - There's still lots of partial code duplication for fault paths, so
 *   they aren't handled as consistently as they need to be.
 *
 * - PIO mostly behaved when last tested.
 *     + including ep0, with all usbtest cases 9, 10
 *     + usbtest 14 (ep0out) doesn't seem to run at all
 *     + double buffered OUT/TX endpoints saw stalls(!) with certain usbtest
 *       configurations, but otherwise double buffering passes basic tests.
 *     + for 2.6.N, for N > ~10, needs API changes for hcd framework.
 *
 * - DMA (CPPI) ... partially behaves, not currently recommended
 *     + about 1/15 the speed of typical EHCI implementations (PCI)
 *     + RX, all too often reqpkt seems to misbehave after tx
 *     + TX, no known issues (other than evident silicon issue)
 *
 * - DMA (Mentor/OMAP) ...has at least toggle update problems
 *
 * - [23-feb-2009] minimal traffic scheduling to avoid bulk RX packet
 *   starvation ... nothing yet for TX, interrupt, or bulk.
 *
 * - Not tested with HNP, but some SRP paths seem to behave.
 *
 * NOTE 24-August-2006:
 *
 * - Bulk traffic finally uses both sides of hardware ep1, freeing up an
 *   extra endpoint for periodic use enabling hub + keybd + mouse.  That
 *   mostly works, except that with "usbnet" it's easy to trigger cases
 *   with "ping" where RX loses.  (a) ping to davinci, even "ping -f",
 *   fine; but (b) ping _from_ davinci, even "ping -c 1", ICMP RX loses
 *   although ARP RX wins.  (That test was done with a full speed link.)
 */


/*
 * NOTE on endpoint usage:
 *
 * CONTROL transfers all go through ep0.  BULK ones go through dedicated IN
 * and OUT endpoints ... hardware is dedicated for those "async" queue(s).
 * (Yes, bulk _could_ use more of the endpoints than that, and would even
 * benefit from it.)
 *
 * INTERUPPT and ISOCHRONOUS transfers are scheduled to the other endpoints.
 * So far that scheduling is both dumb and optimistic:  the endpoint will be
 * "claimed" until its software queue is no longer refilled.  No multiplexing
 * of transfers between endpoints, or anything clever.
 */

struct musb *hcd_to_musb(struct usb_hcd *hcd)
{
	return *(struct musb **) hcd->hcd_priv;
}


static void musb_ep_program(struct musb *musb, u8 epnum,
			struct urb *urb, int is_out,
			u8 *buf, u32 offset, u32 len);

/*
 * Clear TX fifo. Needed to avoid BABBLE errors.
 */
static void musb_h_tx_flush_fifo(struct musb_hw_ep *ep)
{
	struct musb	*musb = ep->musb;
	void __iomem	*epio = ep->regs;
	u16		csr;
	int		retries = 1000;

	csr = musb_readw(epio, MUSB_TXCSR);
	while (csr & MUSB_TXCSR_FIFONOTEMPTY) {
		csr |= MUSB_TXCSR_FLUSHFIFO | MUSB_TXCSR_TXPKTRDY;
		musb_writew(epio, MUSB_TXCSR, csr);
		csr = musb_readw(epio, MUSB_TXCSR);

		/*
		 * FIXME: sometimes the tx fifo flush failed, it has been
		 * observed during device disconnect on AM335x.
		 *
		 * To reproduce the issue, ensure tx urb(s) are queued when
		 * unplug the usb device which is connected to AM335x usb
		 * host port.
		 *
		 * I found using a usb-ethernet device and running iperf
		 * (client on AM335x) has very high chance to trigger it.
		 *
		 * Better to turn on musb_dbg() in musb_cleanup_urb() with
		 * CPPI enabled to see the issue when aborting the tx channel.
		 */
		if (dev_WARN_ONCE(musb->controller, retries-- < 1,
				"Could not flush host TX%d fifo: csr: %04x\n",
				ep->epnum, csr))
			return;
		mdelay(1);
	}
}

static void musb_h_ep0_flush_fifo(struct musb_hw_ep *ep)
{
	void __iomem	*epio = ep->regs;
	u16		csr;
	int		retries = 5;

	/* scrub any data left in the fifo */
	do {
		csr = musb_readw(epio, MUSB_TXCSR);
		if (!(csr & (MUSB_CSR0_TXPKTRDY | MUSB_CSR0_RXPKTRDY)))
			break;
		musb_writew(epio, MUSB_TXCSR, MUSB_CSR0_FLUSHFIFO);
		csr = musb_readw(epio, MUSB_TXCSR);
		udelay(10);
	} while (--retries);

	WARN(!retries, "Could not flush host TX%d fifo: csr: %04x\n",
			ep->epnum, csr);

	/* and reset for the next transfer */
	musb_writew(epio, MUSB_TXCSR, 0);
}

/*
 * Start transmit. Caller is responsible for locking shared resources.
 * musb must be locked.
 */
static inline void musb_h_tx_start(struct musb_hw_ep *ep)
{
	u16	txcsr;

	/* NOTE: no locks here; caller should lock and select EP */
	if (ep->epnum) {
		txcsr = musb_readw(ep->regs, MUSB_TXCSR);
		txcsr |= MUSB_TXCSR_TXPKTRDY | MUSB_TXCSR_H_WZC_BITS;
		musb_writew(ep->regs, MUSB_TXCSR, txcsr);
	} else {
		txcsr = MUSB_CSR0_H_SETUPPKT | MUSB_CSR0_TXPKTRDY;
		musb_writew(ep->regs, MUSB_CSR0, txcsr);
	}

}

static inline void musb_h_tx_dma_start(struct musb_hw_ep *ep)
{
	u16	txcsr;

	/* NOTE: no locks here; caller should lock and select EP */
	txcsr = musb_readw(ep->regs, MUSB_TXCSR);
	txcsr |= MUSB_TXCSR_DMAENAB | MUSB_TXCSR_H_WZC_BITS;
	if (is_cppi_enabled(ep->musb))
		txcsr |= MUSB_TXCSR_DMAMODE;
	musb_writew(ep->regs, MUSB_TXCSR, txcsr);
}

static void musb_ep_set_qh(struct musb_hw_ep *ep, int is_in, struct musb_qh *qh)
{
	if (is_in != 0 || ep->is_shared_fifo)
		ep->in_qh  = qh;
	if (is_in == 0 || ep->is_shared_fifo)
		ep->out_qh = qh;
}

static struct musb_qh *musb_ep_get_qh(struct musb_hw_ep *ep, int is_in)
{
	return is_in ? ep->in_qh : ep->out_qh;
}

/*
 * Start the URB at the front of an endpoint's queue
 * end must be claimed from the caller.
 *
 * Context: controller locked, irqs blocked
 */
static void
musb_start_urb(struct musb *musb, int is_in, struct musb_qh *qh)
{
	u32			len;
	void __iomem		*mbase =  musb->mregs;
	struct urb		*urb = next_urb(qh);
	void			*buf = urb->transfer_buffer;
	u32			offset = 0;
	struct musb_hw_ep	*hw_ep = qh->hw_ep;
	int			epnum = hw_ep->epnum;

	/* initialize software qh state */
	qh->offset = 0;
	qh->segsize = 0;

	/* gather right source of data */
	switch (qh->type) {
	case USB_ENDPOINT_XFER_CONTROL:
		/* control transfers always start with SETUP */
		is_in = 0;
		musb->ep0_stage = MUSB_EP0_START;
		buf = urb->setup_packet;
		len = 8;
		break;
	case USB_ENDPOINT_XFER_ISOC:
		qh->iso_idx = 0;
		qh->frame = 0;
		offset = urb->iso_frame_desc[0].offset;
		len = urb->iso_frame_desc[0].length;
		break;
	default:		/* bulk, interrupt */
		/* actual_length may be nonzero on retry paths */
		buf = urb->transfer_buffer + urb->actual_length;
		len = urb->transfer_buffer_length - urb->actual_length;
	}

	trace_musb_urb_start(musb, urb);

	/* Configure endpoint */
	musb_ep_set_qh(hw_ep, is_in, qh);
	musb_ep_program(musb, epnum, urb, !is_in, buf, offset, len);

	/* transmit may have more work: start it when it is time */
	if (is_in)
		return;

	/* determine if the time is right for a periodic transfer */
	switch (qh->type) {
	case USB_ENDPOINT_XFER_ISOC:
	case USB_ENDPOINT_XFER_INT:
		musb_dbg(musb, "check whether there's still time for periodic Tx");
		/* FIXME this doesn't implement that scheduling policy ...
		 * or handle framecounter wrapping
		 */
		if (1) {	/* Always assume URB_ISO_ASAP */
			/* REVISIT the SOF irq handler shouldn't duplicate
			 * this code; and we don't init urb->start_frame...
			 */
			qh->frame = 0;
			goto start;
		} else {
			qh->frame = urb->start_frame;
			/* enable SOF interrupt so we can count down */
			musb_dbg(musb, "SOF for %d", epnum);
#if 1 /* ifndef	CONFIG_ARCH_DAVINCI */
			musb_writeb(mbase, MUSB_INTRUSBE, 0xff);
#endif
		}
		break;
	default:
start:
		musb_dbg(musb, "Start TX%d %s", epnum,
			hw_ep->tx_channel ? "dma" : "pio");

		if (!hw_ep->tx_channel)
			musb_h_tx_start(hw_ep);
		else if (is_cppi_enabled(musb) || tusb_dma_omap(musb))
			musb_h_tx_dma_start(hw_ep);
	}
}

/* Context: caller owns controller lock, IRQs are blocked */
static void musb_giveback(struct musb *musb, struct urb *urb, int status)
__releases(musb->lock)
__acquires(musb->lock)
{
	trace_musb_urb_gb(musb, urb);

	usb_hcd_unlink_urb_from_ep(musb->hcd, urb);
	spin_unlock(&musb->lock);
	usb_hcd_giveback_urb(musb->hcd, urb, status);
	spin_lock(&musb->lock);
}

/*
 * Advance this hardware endpoint's queue, completing the specified URB and
 * advancing to either the next URB queued to that qh, or else invalidating
 * that qh and advancing to the next qh scheduled after the current one.
 *
 * Context: caller owns controller lock, IRQs are blocked
 */
static void musb_advance_schedule(struct musb *musb, struct urb *urb,
				  struct musb_hw_ep *hw_ep, int is_in)
{
	struct musb_qh		*qh = musb_ep_get_qh(hw_ep, is_in);
	struct musb_hw_ep	*ep = qh->hw_ep;
	int			ready = qh->is_ready;
	int			status;
	u16			toggle;

	status = (urb->status == -EINPROGRESS) ? 0 : urb->status;

	/* save toggle eagerly, for paranoia */
	switch (qh->type) {
	case USB_ENDPOINT_XFER_BULK:
	case USB_ENDPOINT_XFER_INT:
		toggle = musb->io.get_toggle(qh, !is_in);
		usb_settoggle(urb->dev, qh->epnum, !is_in, toggle ? 1 : 0);
		break;
	case USB_ENDPOINT_XFER_ISOC:
		if (status == 0 && urb->error_count)
			status = -EXDEV;
		break;
	}

	qh->is_ready = 0;
	musb_giveback(musb, urb, status);
	qh->is_ready = ready;

	/* reclaim resources (and bandwidth) ASAP; deschedule it, and
	 * invalidate qh as soon as list_empty(&hep->urb_list)
	 */
	if (list_empty(&qh->hep->urb_list)) {
		struct list_head	*head;
		struct dma_controller	*dma = musb->dma_controller;

		if (is_in) {
			ep->rx_reinit = 1;
			if (ep->rx_channel) {
				dma->channel_release(ep->rx_channel);
				ep->rx_channel = NULL;
			}
		} else {
			ep->tx_reinit = 1;
			if (ep->tx_channel) {
				dma->channel_release(ep->tx_channel);
				ep->tx_channel = NULL;
			}
		}

		/* Clobber old pointers to this qh */
		musb_ep_set_qh(ep, is_in, NULL);
		qh->hep->hcpriv = NULL;

		switch (qh->type) {

		case USB_ENDPOINT_XFER_CONTROL:
		case USB_ENDPOINT_XFER_BULK:
			/* fifo policy for these lists, except that NAKing
			 * should rotate a qh to the end (for fairness).
			 */
			if (qh->mux == 1) {
				head = qh->ring.prev;
				list_del(&qh->ring);
				kfree(qh);
				qh = first_qh(head);
				break;
			}
			fallthrough;

		case USB_ENDPOINT_XFER_ISOC:
		case USB_ENDPOINT_XFER_INT:
			/* this is where periodic bandwidth should be
			 * de-allocated if it's tracked and allocated;
			 * and where we'd update the schedule tree...
			 */
			kfree(qh);
			qh = NULL;
			break;
		}
	}

	if (qh != NULL && qh->is_ready) {
		musb_dbg(musb, "... next ep%d %cX urb %p",
		    hw_ep->epnum, is_in ? 'R' : 'T', next_urb(qh));
		musb_start_urb(musb, is_in, qh);
	}
}

static u16 musb_h_flush_rxfifo(struct musb_hw_ep *hw_ep, u16 csr)
{
	/* we don't want fifo to fill itself again;
	 * ignore dma (various models),
	 * leave toggle alone (may not have been saved yet)
	 */
	csr |= MUSB_RXCSR_FLUSHFIFO | MUSB_RXCSR_RXPKTRDY;
	csr &= ~(MUSB_RXCSR_H_REQPKT
		| MUSB_RXCSR_H_AUTOREQ
		| MUSB_RXCSR_AUTOCLEAR);

	/* write 2x to allow double buffering */
	musb_writew(hw_ep->regs, MUSB_RXCSR, csr);
	musb_writew(hw_ep->regs, MUSB_RXCSR, csr);

	/* flush writebuffer */
	return musb_readw(hw_ep->regs, MUSB_RXCSR);
}

/*
 * PIO RX for a packet (or part of it).
 */
static bool
musb_host_packet_rx(struct musb *musb, struct urb *urb, u8 epnum, u8 iso_err)
{
	u16			rx_count;
	u8			*buf;
	u16			csr;
	bool			done = false;
	u32			length;
	int			do_flush = 0;
	struct musb_hw_ep	*hw_ep = musb->endpoints + epnum;
	void __iomem		*epio = hw_ep->regs;
	struct musb_qh		*qh = hw_ep->in_qh;
	int			pipe = urb->pipe;
	void			*buffer = urb->transfer_buffer;

	/* musb_ep_select(mbase, epnum); */
	rx_count = musb_readw(epio, MUSB_RXCOUNT);
	musb_dbg(musb, "RX%d count %d, buffer %p len %d/%d", epnum, rx_count,
			urb->transfer_buffer, qh->offset,
			urb->transfer_buffer_length);

	/* unload FIFO */
	if (usb_pipeisoc(pipe)) {
		int					status = 0;
		struct usb_iso_packet_descriptor	*d;

		if (iso_err) {
			status = -EILSEQ;
			urb->error_count++;
		}

		d = urb->iso_frame_desc + qh->iso_idx;
		buf = buffer + d->offset;
		length = d->length;
		if (rx_count > length) {
			if (status == 0) {
				status = -EOVERFLOW;
				urb->error_count++;
			}
			musb_dbg(musb, "OVERFLOW %d into %d", rx_count, length);
			do_flush = 1;
		} else
			length = rx_count;
		urb->actual_length += length;
		d->actual_length = length;

		d->status = status;

		/* see if we are done */
		done = (++qh->iso_idx >= urb->number_of_packets);
	} else {
		/* non-isoch */
		buf = buffer + qh->offset;
		length = urb->transfer_buffer_length - qh->offset;
		if (rx_count > length) {
			if (urb->status == -EINPROGRESS)
				urb->status = -EOVERFLOW;
			musb_dbg(musb, "OVERFLOW %d into %d", rx_count, length);
			do_flush = 1;
		} else
			length = rx_count;
		urb->actual_length += length;
		qh->offset += length;

		/* see if we are done */
		done = (urb->actual_length == urb->transfer_buffer_length)
			|| (rx_count < qh->maxpacket)
			|| (urb->status != -EINPROGRESS);
		if (done
				&& (urb->status == -EINPROGRESS)
				&& (urb->transfer_flags & URB_SHORT_NOT_OK)
				&& (urb->actual_length
					< urb->transfer_buffer_length))
			urb->status = -EREMOTEIO;
	}

	musb_read_fifo(hw_ep, length, buf);

	csr = musb_readw(epio, MUSB_RXCSR);
	csr |= MUSB_RXCSR_H_WZC_BITS;
	if (unlikely(do_flush))
		musb_h_flush_rxfifo(hw_ep, csr);
	else {
		/* REVISIT this assumes AUTOCLEAR is never set */
		csr &= ~(MUSB_RXCSR_RXPKTRDY | MUSB_RXCSR_H_REQPKT);
		if (!done)
			csr |= MUSB_RXCSR_H_REQPKT;
		musb_writew(epio, MUSB_RXCSR, csr);
	}

	return done;
}

/* we don't always need to reinit a given side of an endpoint...
 * when we do, use tx/rx reinit routine and then construct a new CSR
 * to address data toggle, NYET, and DMA or PIO.
 *
 * it's possible that driver bugs (especially for DMA) or aborting a
 * transfer might have left the endpoint busier than it should be.
 * the busy/not-empty tests are basically paranoia.
 */
static void
musb_rx_reinit(struct musb *musb, struct musb_qh *qh, u8 epnum)
{
	struct musb_hw_ep *ep = musb->endpoints + epnum;
	u16	csr;

	/* NOTE:  we know the "rx" fifo reinit never triggers for ep0.
	 * That always uses tx_reinit since ep0 repurposes TX register
	 * offsets; the initial SETUP packet is also a kind of OUT.
	 */

	/* if programmed for Tx, put it in RX mode */
	if (ep->is_shared_fifo) {
		csr = musb_readw(ep->regs, MUSB_TXCSR);
		if (csr & MUSB_TXCSR_MODE) {
			musb_h_tx_flush_fifo(ep);
			csr = musb_readw(ep->regs, MUSB_TXCSR);
			musb_writew(ep->regs, MUSB_TXCSR,
				    csr | MUSB_TXCSR_FRCDATATOG);
		}

		/*
		 * Clear the MODE bit (and everything else) to enable Rx.
		 * NOTE: we mustn't clear the DMAMODE bit before DMAENAB.
		 */
		if (csr & MUSB_TXCSR_DMAMODE)
			musb_writew(ep->regs, MUSB_TXCSR, MUSB_TXCSR_DMAMODE);
		musb_writew(ep->regs, MUSB_TXCSR, 0);

	/* scrub all previous state, clearing toggle */
	}
	csr = musb_readw(ep->regs, MUSB_RXCSR);
	if (csr & MUSB_RXCSR_RXPKTRDY)
		WARNING("rx%d, packet/%d ready?\n", ep->epnum,
			musb_readw(ep->regs, MUSB_RXCOUNT));

	musb_h_flush_rxfifo(ep, MUSB_RXCSR_CLRDATATOG);

	/* target addr and (for multipoint) hub addr/port */
	if (musb->is_multipoint) {
		musb_write_rxfunaddr(musb, epnum, qh->addr_reg);
		musb_write_rxhubaddr(musb, epnum, qh->h_addr_reg);
		musb_write_rxhubport(musb, epnum, qh->h_port_reg);
	} else
		musb_writeb(musb->mregs, MUSB_FADDR, qh->addr_reg);

	/* protocol/endpoint, interval/NAKlimit, i/o size */
	musb_writeb(ep->regs, MUSB_RXTYPE, qh->type_reg);
	musb_writeb(ep->regs, MUSB_RXINTERVAL, qh->intv_reg);
	/* NOTE: bulk combining rewrites high bits of maxpacket */
	/* Set RXMAXP with the FIFO size of the endpoint
	 * to disable double buffer mode.
	 */
	musb_writew(ep->regs, MUSB_RXMAXP,
			qh->maxpacket | ((qh->hb_mult - 1) << 11));

	ep->rx_reinit = 0;
}

static void musb_tx_dma_set_mode_mentor(struct dma_controller *dma,
		struct musb_hw_ep *hw_ep, struct musb_qh *qh,
		struct urb *urb, u32 offset,
		u32 *length, u8 *mode)
{
	struct dma_channel	*channel = hw_ep->tx_channel;
	void __iomem		*epio = hw_ep->regs;
	u16			pkt_size = qh->maxpacket;
	u16			csr;

	if (*length > channel->max_len)
		*length = channel->max_len;

	csr = musb_readw(epio, MUSB_TXCSR);
	if (*length > pkt_size) {
		*mode = 1;
		csr |= MUSB_TXCSR_DMAMODE | MUSB_TXCSR_DMAENAB;
		/* autoset shouldn't be set in high bandwidth */
		/*
		 * Enable Autoset according to table
		 * below
		 * bulk_split hb_mult	Autoset_Enable
		 *	0	1	Yes(Normal)
		 *	0	>1	No(High BW ISO)
		 *	1	1	Yes(HS bulk)
		 *	1	>1	Yes(FS bulk)
		 */
		if (qh->hb_mult == 1 || (qh->hb_mult > 1 &&
					can_bulk_split(hw_ep->musb, qh->type)))
			csr |= MUSB_TXCSR_AUTOSET;
	} else {
		*mode = 0;
		csr &= ~(MUSB_TXCSR_AUTOSET | MUSB_TXCSR_DMAMODE);
		csr |= MUSB_TXCSR_DMAENAB; /* against programmer's guide */
	}
	channel->desired_mode = *mode;
	musb_writew(epio, MUSB_TXCSR, csr);
}

static void musb_tx_dma_set_mode_cppi_tusb(struct dma_controller *dma,
					   struct musb_hw_ep *hw_ep,
					   struct musb_qh *qh,
					   struct urb *urb,
					   u32 offset,
					   u32 *length,
					   u8 *mode)
{
	struct dma_channel *channel = hw_ep->tx_channel;

	channel->actual_len = 0;

	/*
	 * TX uses "RNDIS" mode automatically but needs help
	 * to identify the zero-length-final-packet case.
	 */
	*mode = (urb->transfer_flags & URB_ZERO_PACKET) ? 1 : 0;
}

static bool musb_tx_dma_program(struct dma_controller *dma,
		struct musb_hw_ep *hw_ep, struct musb_qh *qh,
		struct urb *urb, u32 offset, u32 length)
{
	struct dma_channel	*channel = hw_ep->tx_channel;
	u16			pkt_size = qh->maxpacket;
	u8			mode;

	if (musb_dma_inventra(hw_ep->musb) || musb_dma_ux500(hw_ep->musb))
		musb_tx_dma_set_mode_mentor(dma, hw_ep, qh, urb, offset,
					    &length, &mode);
	else if (is_cppi_enabled(hw_ep->musb) || tusb_dma_omap(hw_ep->musb))
		musb_tx_dma_set_mode_cppi_tusb(dma, hw_ep, qh, urb, offset,
					       &length, &mode);
	else
		return false;

	qh->segsize = length;

	/*
	 * Ensure the data reaches to main memory before starting
	 * DMA transfer
	 */
	wmb();

	if (!dma->channel_program(channel, pkt_size, mode,
			urb->transfer_dma + offset, length)) {
		void __iomem *epio = hw_ep->regs;
		u16 csr;

		dma->channel_release(channel);
		hw_ep->tx_channel = NULL;

		csr = musb_readw(epio, MUSB_TXCSR);
		csr &= ~(MUSB_TXCSR_AUTOSET | MUSB_TXCSR_DMAENAB);
		musb_writew(epio, MUSB_TXCSR, csr | MUSB_TXCSR_H_WZC_BITS);
		return false;
	}
	return true;
}

/*
 * Program an HDRC endpoint as per the given URB
 * Context: irqs blocked, controller lock held
 */
static void musb_ep_program(struct musb *musb, u8 epnum,
			struct urb *urb, int is_out,
			u8 *buf, u32 offset, u32 len)
{
	struct dma_controller	*dma_controller;
	struct dma_channel	*dma_channel;
	u8			dma_ok;
	void __iomem		*mbase = musb->mregs;
	struct musb_hw_ep	*hw_ep = musb->endpoints + epnum;
	void __iomem		*epio = hw_ep->regs;
	struct musb_qh		*qh = musb_ep_get_qh(hw_ep, !is_out);
	u16			packet_sz = qh->maxpacket;
	u8			use_dma = 1;
	u16			csr;

	musb_dbg(musb, "%s hw%d urb %p spd%d dev%d ep%d%s "
				"h_addr%02x h_port%02x bytes %d",
			is_out ? "-->" : "<--",
			epnum, urb, urb->dev->speed,
			qh->addr_reg, qh->epnum, is_out ? "out" : "in",
			qh->h_addr_reg, qh->h_port_reg,
			len);

	musb_ep_select(mbase, epnum);

	if (is_out && !len) {
		use_dma = 0;
		csr = musb_readw(epio, MUSB_TXCSR);
		csr &= ~MUSB_TXCSR_DMAENAB;
		musb_writew(epio, MUSB_TXCSR, csr);
		hw_ep->tx_channel = NULL;
	}

	/* candidate for DMA? */
	dma_controller = musb->dma_controller;
	if (use_dma && is_dma_capable() && epnum && dma_controller) {
		dma_channel = is_out ? hw_ep->tx_channel : hw_ep->rx_channel;
		if (!dma_channel) {
			dma_channel = dma_controller->channel_alloc(
					dma_controller, hw_ep, is_out);
			if (is_out)
				hw_ep->tx_channel = dma_channel;
			else
				hw_ep->rx_channel = dma_channel;
		}
	} else
		dma_channel = NULL;

	/* make sure we clear DMAEnab, autoSet bits from previous run */

	/* OUT/transmit/EP0 or IN/receive? */
	if (is_out) {
		u16	csr;
		u16	int_txe;
		u16	load_count;

		csr = musb_readw(epio, MUSB_TXCSR);

		/* disable interrupt in case we flush */
		int_txe = musb->intrtxe;
		musb_writew(mbase, MUSB_INTRTXE, int_txe & ~(1 << epnum));

		/* general endpoint setup */
		if (epnum) {
			/* flush all old state, set default */
			/*
			 * We could be flushing valid
			 * packets in double buffering
			 * case
			 */
			if (!hw_ep->tx_double_buffered)
				musb_h_tx_flush_fifo(hw_ep);

			/*
			 * We must not clear the DMAMODE bit before or in
			 * the same cycle with the DMAENAB bit, so we clear
			 * the latter first...
			 */
			csr &= ~(MUSB_TXCSR_H_NAKTIMEOUT
					| MUSB_TXCSR_AUTOSET
					| MUSB_TXCSR_DMAENAB
					| MUSB_TXCSR_FRCDATATOG
					| MUSB_TXCSR_H_RXSTALL
					| MUSB_TXCSR_H_ERROR
					| MUSB_TXCSR_TXPKTRDY
					);
			csr |= MUSB_TXCSR_MODE;

			if (!hw_ep->tx_double_buffered)
				csr |= musb->io.set_toggle(qh, is_out, urb);

			musb_writew(epio, MUSB_TXCSR, csr);
			/* REVISIT may need to clear FLUSHFIFO ... */
			csr &= ~MUSB_TXCSR_DMAMODE;
			musb_writew(epio, MUSB_TXCSR, csr);
			csr = musb_readw(epio, MUSB_TXCSR);
		} else {
			/* endpoint 0: just flush */
			musb_h_ep0_flush_fifo(hw_ep);
		}

		/* target addr and (for multipoint) hub addr/port */
		if (musb->is_multipoint) {
			musb_write_txfunaddr(musb, epnum, qh->addr_reg);
			musb_write_txhubaddr(musb, epnum, qh->h_addr_reg);
			musb_write_txhubport(musb, epnum, qh->h_port_reg);
/* FIXME if !epnum, do the same for RX ... */
		} else
			musb_writeb(mbase, MUSB_FADDR, qh->addr_reg);

		/* protocol/endpoint/interval/NAKlimit */
		if (epnum) {
			musb_writeb(epio, MUSB_TXTYPE, qh->type_reg);
			if (can_bulk_split(musb, qh->type)) {
				qh->hb_mult = hw_ep->max_packet_sz_tx
						/ packet_sz;
				musb_writew(epio, MUSB_TXMAXP, packet_sz
					| ((qh->hb_mult) - 1) << 11);
			} else {
				musb_writew(epio, MUSB_TXMAXP,
						qh->maxpacket |
						((qh->hb_mult - 1) << 11));
			}
			musb_writeb(epio, MUSB_TXINTERVAL, qh->intv_reg);
		} else {
			musb_writeb(epio, MUSB_NAKLIMIT0, qh->intv_reg);
			if (musb->is_multipoint)
				musb_writeb(epio, MUSB_TYPE0,
						qh->type_reg);
		}

		if (can_bulk_split(musb, qh->type))
			load_count = min((u32) hw_ep->max_packet_sz_tx,
						len);
		else
			load_count = min((u32) packet_sz, len);

		if (dma_channel && musb_tx_dma_program(dma_controller,
					hw_ep, qh, urb, offset, len))
			load_count = 0;

		if (load_count) {
			/* PIO to load FIFO */
			qh->segsize = load_count;
			if (!buf) {
				sg_miter_start(&qh->sg_miter, urb->sg, 1,
						SG_MITER_ATOMIC
						| SG_MITER_FROM_SG);
				if (!sg_miter_next(&qh->sg_miter)) {
					dev_err(musb->controller,
							"error: sg"
							"list empty\n");
					sg_miter_stop(&qh->sg_miter);
					goto finish;
				}
				buf = qh->sg_miter.addr + urb->sg->offset +
					urb->actual_length;
				load_count = min_t(u32, load_count,
						qh->sg_miter.length);
				musb_write_fifo(hw_ep, load_count, buf);
				qh->sg_miter.consumed = load_count;
				sg_miter_stop(&qh->sg_miter);
			} else
				musb_write_fifo(hw_ep, load_count, buf);
		}
finish:
		/* re-enable interrupt */
		musb_writew(mbase, MUSB_INTRTXE, int_txe);

	/* IN/receive */
	} else {
		u16 csr = 0;

		if (hw_ep->rx_reinit) {
			musb_rx_reinit(musb, qh, epnum);
			csr |= musb->io.set_toggle(qh, is_out, urb);

			if (qh->type == USB_ENDPOINT_XFER_INT)
				csr |= MUSB_RXCSR_DISNYET;

		} else {
			csr = musb_readw(hw_ep->regs, MUSB_RXCSR);

			if (csr & (MUSB_RXCSR_RXPKTRDY
					| MUSB_RXCSR_DMAENAB
					| MUSB_RXCSR_H_REQPKT))
				ERR("broken !rx_reinit, ep%d csr %04x\n",
						hw_ep->epnum, csr);

			/* scrub any stale state, leaving toggle alone */
			csr &= MUSB_RXCSR_DISNYET;
		}

		/* kick things off */

		if ((is_cppi_enabled(musb) || tusb_dma_omap(musb)) && dma_channel) {
			/* Candidate for DMA */
			dma_channel->actual_len = 0L;
			qh->segsize = len;

			/* AUTOREQ is in a DMA register */
			musb_writew(hw_ep->regs, MUSB_RXCSR, csr);
			csr = musb_readw(hw_ep->regs, MUSB_RXCSR);

			/*
			 * Unless caller treats short RX transfers as
			 * errors, we dare not queue multiple transfers.
			 */
			dma_ok = dma_controller->channel_program(dma_channel,
					packet_sz, !(urb->transfer_flags &
						     URB_SHORT_NOT_OK),
					urb->transfer_dma + offset,
					qh->segsize);
			if (!dma_ok) {
				dma_controller->channel_release(dma_channel);
				hw_ep->rx_channel = dma_channel = NULL;
			} else
				csr |= MUSB_RXCSR_DMAENAB;
		}

		csr |= MUSB_RXCSR_H_REQPKT;
		musb_dbg(musb, "RXCSR%d := %04x", epnum, csr);
		musb_writew(hw_ep->regs, MUSB_RXCSR, csr);
		csr = musb_readw(hw_ep->regs, MUSB_RXCSR);
	}
}

/* Schedule next QH from musb->in_bulk/out_bulk and move the current qh to
 * the end; avoids starvation for other endpoints.
 */
static void musb_bulk_nak_timeout(struct musb *musb, struct musb_hw_ep *ep,
	int is_in)
{
	struct dma_channel	*dma;
	struct urb		*urb;
	void __iomem		*mbase = musb->mregs;
	void __iomem		*epio = ep->regs;
	struct musb_qh		*cur_qh, *next_qh;
	u16			rx_csr, tx_csr;
	u16			toggle;

	musb_ep_select(mbase, ep->epnum);
	if (is_in) {
		dma = is_dma_capable() ? ep->rx_channel : NULL;

		/*
		 * Need to stop the transaction by clearing REQPKT first
		 * then the NAK Timeout bit ref MUSBMHDRC USB 2.0 HIGH-SPEED
		 * DUAL-ROLE CONTROLLER Programmer's Guide, section 9.2.2
		 */
		rx_csr = musb_readw(epio, MUSB_RXCSR);
		rx_csr |= MUSB_RXCSR_H_WZC_BITS;
		rx_csr &= ~MUSB_RXCSR_H_REQPKT;
		musb_writew(epio, MUSB_RXCSR, rx_csr);
		rx_csr &= ~MUSB_RXCSR_DATAERROR;
		musb_writew(epio, MUSB_RXCSR, rx_csr);

		cur_qh = first_qh(&musb->in_bulk);
	} else {
		dma = is_dma_capable() ? ep->tx_channel : NULL;

		/* clear nak timeout bit */
		tx_csr = musb_readw(epio, MUSB_TXCSR);
		tx_csr |= MUSB_TXCSR_H_WZC_BITS;
		tx_csr &= ~MUSB_TXCSR_H_NAKTIMEOUT;
		musb_writew(epio, MUSB_TXCSR, tx_csr);

		cur_qh = first_qh(&musb->out_bulk);
	}
	if (cur_qh) {
		urb = next_urb(cur_qh);
		if (dma_channel_status(dma) == MUSB_DMA_STATUS_BUSY) {
			dma->status = MUSB_DMA_STATUS_CORE_ABORT;
			musb->dma_controller->channel_abort(dma);
			urb->actual_length += dma->actual_len;
			dma->actual_len = 0L;
		}
		toggle = musb->io.get_toggle(cur_qh, !is_in);
		usb_settoggle(urb->dev, cur_qh->epnum, !is_in, toggle ? 1 : 0);

		if (is_in) {
			/* move cur_qh to end of queue */
			list_move_tail(&cur_qh->ring, &musb->in_bulk);

			/* get the next qh from musb->in_bulk */
			next_qh = first_qh(&musb->in_bulk);

			/* set rx_reinit and schedule the next qh */
			ep->rx_reinit = 1;
		} else {
			/* move cur_qh to end of queue */
			list_move_tail(&cur_qh->ring, &musb->out_bulk);

			/* get the next qh from musb->out_bulk */
			next_qh = first_qh(&musb->out_bulk);

			/* set tx_reinit and schedule the next qh */
			ep->tx_reinit = 1;
		}

		if (next_qh)
			musb_start_urb(musb, is_in, next_qh);
	}
}

/*
 * Service the default endpoint (ep0) as host.
 * Return true until it's time to start the status stage.
 */
static bool musb_h_ep0_continue(struct musb *musb, u16 len, struct urb *urb)
{
	bool			 more = false;
	u8			*fifo_dest = NULL;
	u16			fifo_count = 0;
	struct musb_hw_ep	*hw_ep = musb->control_ep;
	struct musb_qh		*qh = hw_ep->in_qh;
	struct usb_ctrlrequest	*request;

	switch (musb->ep0_stage) {
	case MUSB_EP0_IN:
		fifo_dest = urb->transfer_buffer + urb->actual_length;
		fifo_count = min_t(size_t, len, urb->transfer_buffer_length -
				   urb->actual_length);
		if (fifo_count < len)
			urb->status = -EOVERFLOW;

		musb_read_fifo(hw_ep, fifo_count, fifo_dest);

		urb->actual_length += fifo_count;
		if (len < qh->maxpacket) {
			/* always terminate on short read; it's
			 * rarely reported as an error.
			 */
		} else if (urb->actual_length <
				urb->transfer_buffer_length)
			more = true;
		break;
	case MUSB_EP0_START:
		request = (struct usb_ctrlrequest *) urb->setup_packet;

		if (!request->wLength) {
			musb_dbg(musb, "start no-DATA");
			break;
		} else if (request->bRequestType & USB_DIR_IN) {
			musb_dbg(musb, "start IN-DATA");
			musb->ep0_stage = MUSB_EP0_IN;
			more = true;
			break;
		} else {
			musb_dbg(musb, "start OUT-DATA");
			musb->ep0_stage = MUSB_EP0_OUT;
			more = true;
		}
		fallthrough;
	case MUSB_EP0_OUT:
		fifo_count = min_t(size_t, qh->maxpacket,
				   urb->transfer_buffer_length -
				   urb->actual_length);
		if (fifo_count) {
			fifo_dest = (u8 *) (urb->transfer_buffer
					+ urb->actual_length);
			musb_dbg(musb, "Sending %d byte%s to ep0 fifo %p",
					fifo_count,
					(fifo_count == 1) ? "" : "s",
					fifo_dest);
			musb_write_fifo(hw_ep, fifo_count, fifo_dest);

			urb->actual_length += fifo_count;
			more = true;
		}
		break;
	default:
		ERR("bogus ep0 stage %d\n", musb->ep0_stage);
		break;
	}

	return more;
}

/*
 * Handle default endpoint interrupt as host. Only called in IRQ time
 * from musb_interrupt().
 *
 * called with controller irqlocked
 */
irqreturn_t musb_h_ep0_irq(struct musb *musb)
{
	struct urb		*urb;
	u16			csr, len;
	int			status = 0;
	void __iomem		*mbase = musb->mregs;
	struct musb_hw_ep	*hw_ep = musb->control_ep;
	void __iomem		*epio = hw_ep->regs;
	struct musb_qh		*qh = hw_ep->in_qh;
	bool			complete = false;
	irqreturn_t		retval = IRQ_NONE;

	/* ep0 only has one queue, "in" */
	urb = next_urb(qh);

	musb_ep_select(mbase, 0);
	csr = musb_readw(epio, MUSB_CSR0);
	len = (csr & MUSB_CSR0_RXPKTRDY)
			? musb_readb(epio, MUSB_COUNT0)
			: 0;

	musb_dbg(musb, "<== csr0 %04x, qh %p, count %d, urb %p, stage %d",
		csr, qh, len, urb, musb->ep0_stage);

	/* if we just did status stage, we are done */
	if (MUSB_EP0_STATUS == musb->ep0_stage) {
		retval = IRQ_HANDLED;
		complete = true;
	}

	/* prepare status */
	if (csr & MUSB_CSR0_H_RXSTALL) {
		musb_dbg(musb, "STALLING ENDPOINT");
		status = -EPIPE;

	} else if (csr & MUSB_CSR0_H_ERROR) {
		musb_dbg(musb, "no response, csr0 %04x", csr);
		status = -EPROTO;

	} else if (csr & MUSB_CSR0_H_NAKTIMEOUT) {
		musb_dbg(musb, "control NAK timeout");

		/* NOTE:  this code path would be a good place to PAUSE a
		 * control transfer, if another one is queued, so that
		 * ep0 is more likely to stay busy.  That's already done
		 * for bulk RX transfers.
		 *
		 * if (qh->ring.next != &musb->control), then
		 * we have a candidate... NAKing is *NOT* an error
		 */
		musb_writew(epio, MUSB_CSR0, 0);
		retval = IRQ_HANDLED;
	}

	if (status) {
		musb_dbg(musb, "aborting");
		retval = IRQ_HANDLED;
		if (urb)
			urb->status = status;
		complete = true;

		/* use the proper sequence to abort the transfer */
		if (csr & MUSB_CSR0_H_REQPKT) {
			csr &= ~MUSB_CSR0_H_REQPKT;
			musb_writew(epio, MUSB_CSR0, csr);
			csr &= ~MUSB_CSR0_H_NAKTIMEOUT;
			musb_writew(epio, MUSB_CSR0, csr);
		} else {
			musb_h_ep0_flush_fifo(hw_ep);
		}

		musb_writeb(epio, MUSB_NAKLIMIT0, 0);

		/* clear it */
		musb_writew(epio, MUSB_CSR0, 0);
	}

	if (unlikely(!urb)) {
		/* stop endpoint since we have no place for its data, this
		 * SHOULD NEVER HAPPEN! */
		ERR("no URB for end 0\n");

		musb_h_ep0_flush_fifo(hw_ep);
		goto done;
	}

	if (!complete) {
		/* call common logic and prepare response */
		if (musb_h_ep0_continue(musb, len, urb)) {
			/* more packets required */
			csr = (MUSB_EP0_IN == musb->ep0_stage)
				?  MUSB_CSR0_H_REQPKT : MUSB_CSR0_TXPKTRDY;
		} else {
			/* data transfer complete; perform status phase */
			if (usb_pipeout(urb->pipe)
					|| !urb->transfer_buffer_length)
				csr = MUSB_CSR0_H_STATUSPKT
					| MUSB_CSR0_H_REQPKT;
			else
				csr = MUSB_CSR0_H_STATUSPKT
					| MUSB_CSR0_TXPKTRDY;

			/* disable ping token in status phase */
			csr |= MUSB_CSR0_H_DIS_PING;

			/* flag status stage */
			musb->ep0_stage = MUSB_EP0_STATUS;

			musb_dbg(musb, "ep0 STATUS, csr %04x", csr);

		}
		musb_writew(epio, MUSB_CSR0, csr);
		retval = IRQ_HANDLED;
	} else
		musb->ep0_stage = MUSB_EP0_IDLE;

	/* call completion handler if done */
	if (complete)
		musb_advance_schedule(musb, urb, hw_ep, 1);
done:
	return retval;
}


#ifdef CONFIG_USB_INVENTRA_DMA

/* Host side TX (OUT) using Mentor DMA works as follows:
	submit_urb ->
		- if queue was empty, Program Endpoint
		- ... which starts DMA to fifo in mode 1 or 0

	DMA Isr (transfer complete) -> TxAvail()
		- Stop DMA (~DmaEnab)	(<--- Alert ... currently happens
					only in musb_cleanup_urb)
		- TxPktRdy has to be set in mode 0 or for
			short packets in mode 1.
*/

#endif

/* Service a Tx-Available or dma completion irq for the endpoint */
void musb_host_tx(struct musb *musb, u8 epnum)
{
	int			pipe;
	bool			done = false;
	u16			tx_csr;
	size_t			length = 0;
	size_t			offset = 0;
	struct musb_hw_ep	*hw_ep = musb->endpoints + epnum;
	void __iomem		*epio = hw_ep->regs;
	struct musb_qh		*qh = hw_ep->out_qh;
	struct urb		*urb = next_urb(qh);
	u32			status = 0;
	void __iomem		*mbase = musb->mregs;
	struct dma_channel	*dma;
	bool			transfer_pending = false;

	musb_ep_select(mbase, epnum);
	tx_csr = musb_readw(epio, MUSB_TXCSR);

	/* with CPPI, DMA sometimes triggers "extra" irqs */
	if (!urb) {
		musb_dbg(musb, "extra TX%d ready, csr %04x", epnum, tx_csr);
		return;
	}

	pipe = urb->pipe;
	dma = is_dma_capable() ? hw_ep->tx_channel : NULL;
	trace_musb_urb_tx(musb, urb);
	musb_dbg(musb, "OUT/TX%d end, csr %04x%s", epnum, tx_csr,
			dma ? ", dma" : "");

	/* check for errors */
	if (tx_csr & MUSB_TXCSR_H_RXSTALL) {
		/* dma was disabled, fifo flushed */
		musb_dbg(musb, "TX end %d stall", epnum);

		/* stall; record URB status */
		status = -EPIPE;

	} else if (tx_csr & MUSB_TXCSR_H_ERROR) {
		/* (NON-ISO) dma was disabled, fifo flushed */
		musb_dbg(musb, "TX 3strikes on ep=%d", epnum);

		status = -ETIMEDOUT;

	} else if (tx_csr & MUSB_TXCSR_H_NAKTIMEOUT) {
		if (USB_ENDPOINT_XFER_BULK == qh->type && qh->mux == 1
				&& !list_is_singular(&musb->out_bulk)) {
			musb_dbg(musb, "NAK timeout on TX%d ep", epnum);
			musb_bulk_nak_timeout(musb, hw_ep, 0);
		} else {
			musb_dbg(musb, "TX ep%d device not responding", epnum);
			/* NOTE:  this code path would be a good place to PAUSE a
			 * transfer, if there's some other (nonperiodic) tx urb
			 * that could use this fifo.  (dma complicates it...)
			 * That's already done for bulk RX transfers.
			 *
			 * if (bulk && qh->ring.next != &musb->out_bulk), then
			 * we have a candidate... NAKing is *NOT* an error
			 */
			musb_ep_select(mbase, epnum);
			musb_writew(epio, MUSB_TXCSR,
					MUSB_TXCSR_H_WZC_BITS
					| MUSB_TXCSR_TXPKTRDY);
		}
		return;
	}

done:
	if (status) {
		if (dma_channel_status(dma) == MUSB_DMA_STATUS_BUSY) {
			dma->status = MUSB_DMA_STATUS_CORE_ABORT;
			musb->dma_controller->channel_abort(dma);
		}

		/* do the proper sequence to abort the transfer in the
		 * usb core; the dma engine should already be stopped.
		 */
		musb_h_tx_flush_fifo(hw_ep);
		tx_csr &= ~(MUSB_TXCSR_AUTOSET
				| MUSB_TXCSR_DMAENAB
				| MUSB_TXCSR_H_ERROR
				| MUSB_TXCSR_H_RXSTALL
				| MUSB_TXCSR_H_NAKTIMEOUT
				);

		musb_ep_select(mbase, epnum);
		musb_writew(epio, MUSB_TXCSR, tx_csr);
		/* REVISIT may need to clear FLUSHFIFO ... */
		musb_writew(epio, MUSB_TXCSR, tx_csr);
		musb_writeb(epio, MUSB_TXINTERVAL, 0);

		done = true;
	}

	/* second cppi case */
	if (dma_channel_status(dma) == MUSB_DMA_STATUS_BUSY) {
		musb_dbg(musb, "extra TX%d ready, csr %04x", epnum, tx_csr);
		return;
	}

	if (is_dma_capable() && dma && !status) {
		/*
		 * DMA has completed.  But if we're using DMA mode 1 (multi
		 * packet DMA), we need a terminal TXPKTRDY interrupt before
		 * we can consider this transfer completed, lest we trash
		 * its last packet when writing the next URB's data.  So we
		 * switch back to mode 0 to get that interrupt; we'll come
		 * back here once it happens.
		 */
		if (tx_csr & MUSB_TXCSR_DMAMODE) {
			/*
			 * We shouldn't clear DMAMODE with DMAENAB set; so
			 * clear them in a safe order.  That should be OK
			 * once TXPKTRDY has been set (and I've never seen
			 * it being 0 at this moment -- DMA interrupt latency
			 * is significant) but if it hasn't been then we have
			 * no choice but to stop being polite and ignore the
			 * programmer's guide... :-)
			 *
			 * Note that we must write TXCSR with TXPKTRDY cleared
			 * in order not to re-trigger the packet send (this bit
			 * can't be cleared by CPU), and there's another caveat:
			 * TXPKTRDY may be set shortly and then cleared in the
			 * double-buffered FIFO mode, so we do an extra TXCSR
			 * read for debouncing...
			 */
			tx_csr &= musb_readw(epio, MUSB_TXCSR);
			if (tx_csr & MUSB_TXCSR_TXPKTRDY) {
				tx_csr &= ~(MUSB_TXCSR_DMAENAB |
					    MUSB_TXCSR_TXPKTRDY);
				musb_writew(epio, MUSB_TXCSR,
					    tx_csr | MUSB_TXCSR_H_WZC_BITS);
			}
			tx_csr &= ~(MUSB_TXCSR_DMAMODE |
				    MUSB_TXCSR_TXPKTRDY);
			musb_writew(epio, MUSB_TXCSR,
				    tx_csr | MUSB_TXCSR_H_WZC_BITS);

			/*
			 * There is no guarantee that we'll get an interrupt
			 * after clearing DMAMODE as we might have done this
			 * too late (after TXPKTRDY was cleared by controller).
			 * Re-read TXCSR as we have spoiled its previous value.
			 */
			tx_csr = musb_readw(epio, MUSB_TXCSR);
		}

		/*
		 * We may get here from a DMA completion or TXPKTRDY interrupt.
		 * In any case, we must check the FIFO status here and bail out
		 * only if the FIFO still has data -- that should prevent the
		 * "missed" TXPKTRDY interrupts and deal with double-buffered
		 * FIFO mode too...
		 */
		if (tx_csr & (MUSB_TXCSR_FIFONOTEMPTY | MUSB_TXCSR_TXPKTRDY)) {
			musb_dbg(musb,
				"DMA complete but FIFO not empty, CSR %04x",
				tx_csr);
			return;
		}
	}

	if (!status || dma || usb_pipeisoc(pipe)) {
		if (dma)
			length = dma->actual_len;
		else
			length = qh->segsize;
		qh->offset += length;

		if (usb_pipeisoc(pipe)) {
			struct usb_iso_packet_descriptor	*d;

			d = urb->iso_frame_desc + qh->iso_idx;
			d->actual_length = length;
			d->status = status;
			if (++qh->iso_idx >= urb->number_of_packets) {
				done = true;
			} else {
				d++;
				offset = d->offset;
				length = d->length;
			}
		} else if (dma && urb->transfer_buffer_length == qh->offset) {
			done = true;
		} else {
			/* see if we need to send more data, or ZLP */
			if (qh->segsize < qh->maxpacket)
				done = true;
			else if (qh->offset == urb->transfer_buffer_length
					&& !(urb->transfer_flags
						& URB_ZERO_PACKET))
				done = true;
			if (!done) {
				offset = qh->offset;
				length = urb->transfer_buffer_length - offset;
				transfer_pending = true;
			}
		}
	}

	/* urb->status != -EINPROGRESS means request has been faulted,
	 * so we must abort this transfer after cleanup
	 */
	if (urb->status != -EINPROGRESS) {
		done = true;
		if (status == 0)
			status = urb->status;
	}

	if (done) {
		/* set status */
		urb->status = status;
		urb->actual_length = qh->offset;
		musb_advance_schedule(musb, urb, hw_ep, USB_DIR_OUT);
		return;
	} else if ((usb_pipeisoc(pipe) || transfer_pending) && dma) {
		if (musb_tx_dma_program(musb->dma_controller, hw_ep, qh, urb,
				offset, length)) {
			if (is_cppi_enabled(musb) || tusb_dma_omap(musb))
				musb_h_tx_dma_start(hw_ep);
			return;
		}
	} else	if (tx_csr & MUSB_TXCSR_DMAENAB) {
		musb_dbg(musb, "not complete, but DMA enabled?");
		return;
	}

	/*
	 * PIO: start next packet in this URB.
	 *
	 * REVISIT: some docs say that when hw_ep->tx_double_buffered,
	 * (and presumably, FIFO is not half-full) we should write *two*
	 * packets before updating TXCSR; other docs disagree...
	 */
	if (length > qh->maxpacket)
		length = qh->maxpacket;
	/* Unmap the buffer so that CPU can use it */
	usb_hcd_unmap_urb_for_dma(musb->hcd, urb);

	/*
	 * We need to map sg if the transfer_buffer is
	 * NULL.
	 */
	if (!urb->transfer_buffer) {
		/* sg_miter_start is already done in musb_ep_program */
		if (!sg_miter_next(&qh->sg_miter)) {
			dev_err(musb->controller, "error: sg list empty\n");
			sg_miter_stop(&qh->sg_miter);
			status = -EINVAL;
			goto done;
		}
		length = min_t(u32, length, qh->sg_miter.length);
		musb_write_fifo(hw_ep, length, qh->sg_miter.addr);
		qh->sg_miter.consumed = length;
		sg_miter_stop(&qh->sg_miter);
	} else {
		musb_write_fifo(hw_ep, length, urb->transfer_buffer + offset);
	}

	qh->segsize = length;

	musb_ep_select(mbase, epnum);
	musb_writew(epio, MUSB_TXCSR,
			MUSB_TXCSR_H_WZC_BITS | MUSB_TXCSR_TXPKTRDY);
}

#ifdef CONFIG_USB_TI_CPPI41_DMA
/* Seems to set up ISO for cppi41 and not advance len. See commit c57c41d */
static int musb_rx_dma_iso_cppi41(struct dma_controller *dma,
				  struct musb_hw_ep *hw_ep,
				  struct musb_qh *qh,
				  struct urb *urb,
				  size_t len)
{
	struct dma_channel *channel = hw_ep->rx_channel;
	void __iomem *epio = hw_ep->regs;
	dma_addr_t *buf;
	u32 length;
	u16 val;

	buf = (void *)urb->iso_frame_desc[qh->iso_idx].offset +
		(u32)urb->transfer_dma;

	length = urb->iso_frame_desc[qh->iso_idx].length;

	val = musb_readw(epio, MUSB_RXCSR);
	val |= MUSB_RXCSR_DMAENAB;
	musb_writew(hw_ep->regs, MUSB_RXCSR, val);

	return dma->channel_program(channel, qh->maxpacket, 0,
				   (u32)buf, length);
}
#else
static inline int musb_rx_dma_iso_cppi41(struct dma_controller *dma,
					 struct musb_hw_ep *hw_ep,
					 struct musb_qh *qh,
					 struct urb *urb,
					 size_t len)
{
	return false;
}
#endif

#if defined(CONFIG_USB_INVENTRA_DMA) || defined(CONFIG_USB_UX500_DMA) || \
	defined(CONFIG_USB_TI_CPPI41_DMA)
/* Host side RX (IN) using Mentor DMA works as follows:
	submit_urb ->
		- if queue was empty, ProgramEndpoint
		- first IN token is sent out (by setting ReqPkt)
	LinuxIsr -> RxReady()
	/\	=> first packet is received
	|	- Set in mode 0 (DmaEnab, ~ReqPkt)
	|		-> DMA Isr (transfer complete) -> RxReady()
	|		    - Ack receive (~RxPktRdy), turn off DMA (~DmaEnab)
	|		    - if urb not complete, send next IN token (ReqPkt)
	|			   |		else complete urb.
	|			   |
	---------------------------
 *
 * Nuances of mode 1:
 *	For short packets, no ack (+RxPktRdy) is sent automatically
 *	(even if AutoClear is ON)
 *	For full packets, ack (~RxPktRdy) and next IN token (+ReqPkt) is sent
 *	automatically => major problem, as collecting the next packet becomes
 *	difficult. Hence mode 1 is not used.
 *
 * REVISIT
 *	All we care about at this driver level is that
 *       (a) all URBs terminate with REQPKT cleared and fifo(s) empty;
 *       (b) termination conditions are: short RX, or buffer full;
 *       (c) fault modes include
 *           - iff URB_SHORT_NOT_OK, short RX status is -EREMOTEIO.
 *             (and that endpoint's dma queue stops immediately)
 *           - overflow (full, PLUS more bytes in the terminal packet)
 *
 *	So for example, usb-storage sets URB_SHORT_NOT_OK, and would
 *	thus be a great candidate for using mode 1 ... for all but the
 *	last packet of one URB's transfer.
 */
static int musb_rx_dma_inventra_cppi41(struct dma_controller *dma,
				       struct musb_hw_ep *hw_ep,
				       struct musb_qh *qh,
				       struct urb *urb,
				       size_t len)
{
	struct dma_channel *channel = hw_ep->rx_channel;
	void __iomem *epio = hw_ep->regs;
	u16 val;
	int pipe;
	bool done;

	pipe = urb->pipe;

	if (usb_pipeisoc(pipe)) {
		struct usb_iso_packet_descriptor *d;

		d = urb->iso_frame_desc + qh->iso_idx;
		d->actual_length = len;

		/* even if there was an error, we did the dma
		 * for iso_frame_desc->length
		 */
		if (d->status != -EILSEQ && d->status != -EOVERFLOW)
			d->status = 0;

		if (++qh->iso_idx >= urb->number_of_packets) {
			done = true;
		} else {
			/* REVISIT: Why ignore return value here? */
			if (musb_dma_cppi41(hw_ep->musb))
				done = musb_rx_dma_iso_cppi41(dma, hw_ep, qh,
							      urb, len);
			done = false;
		}

	} else  {
		/* done if urb buffer is full or short packet is recd */
		done = (urb->actual_length + len >=
			urb->transfer_buffer_length
			|| channel->actual_len < qh->maxpacket
			|| channel->rx_packet_done);
	}

	/* send IN token for next packet, without AUTOREQ */
	if (!done) {
		val = musb_readw(epio, MUSB_RXCSR);
		val |= MUSB_RXCSR_H_REQPKT;
		musb_writew(epio, MUSB_RXCSR, MUSB_RXCSR_H_WZC_BITS | val);
	}

	return done;
}

/* Disadvantage of using mode 1:
 *	It's basically usable only for mass storage class; essentially all
 *	other protocols also terminate transfers on short packets.
 *
 * Details:
 *	An extra IN token is sent at the end of the transfer (due to AUTOREQ)
 *	If you try to use mode 1 for (transfer_buffer_length - 512), and try
 *	to use the extra IN token to grab the last packet using mode 0, then
 *	the problem is that you cannot be sure when the device will send the
 *	last packet and RxPktRdy set. Sometimes the packet is recd too soon
 *	such that it gets lost when RxCSR is re-set at the end of the mode 1
 *	transfer, while sometimes it is recd just a little late so that if you
 *	try to configure for mode 0 soon after the mode 1 transfer is
 *	completed, you will find rxcount 0. Okay, so you might think why not
 *	wait for an interrupt when the pkt is recd. Well, you won't get any!
 */
static int musb_rx_dma_in_inventra_cppi41(struct dma_controller *dma,
					  struct musb_hw_ep *hw_ep,
					  struct musb_qh *qh,
					  struct urb *urb,
					  size_t len,
					  u8 iso_err)
{
	struct musb *musb = hw_ep->musb;
	void __iomem *epio = hw_ep->regs;
	struct dma_channel *channel = hw_ep->rx_channel;
	u16 rx_count, val;
	int length, pipe, done;
	dma_addr_t buf;

	rx_count = musb_readw(epio, MUSB_RXCOUNT);
	pipe = urb->pipe;

	if (usb_pipeisoc(pipe)) {
		int d_status = 0;
		struct usb_iso_packet_descriptor *d;

		d = urb->iso_frame_desc + qh->iso_idx;

		if (iso_err) {
			d_status = -EILSEQ;
			urb->error_count++;
		}
		if (rx_count > d->length) {
			if (d_status == 0) {
				d_status = -EOVERFLOW;
				urb->error_count++;
			}
			musb_dbg(musb, "** OVERFLOW %d into %d",
				rx_count, d->length);

			length = d->length;
		} else
			length = rx_count;
		d->status = d_status;
		buf = urb->transfer_dma + d->offset;
	} else {
		length = rx_count;
		buf = urb->transfer_dma + urb->actual_length;
	}

	channel->desired_mode = 0;
#ifdef USE_MODE1
	/* because of the issue below, mode 1 will
	 * only rarely behave with correct semantics.
	 */
	if ((urb->transfer_flags & URB_SHORT_NOT_OK)
	    && (urb->transfer_buffer_length - urb->actual_length)
	    > qh->maxpacket)
		channel->desired_mode = 1;
	if (rx_count < hw_ep->max_packet_sz_rx) {
		length = rx_count;
		channel->desired_mode = 0;
	} else {
		length = urb->transfer_buffer_length;
	}
#endif

	/* See comments above on disadvantages of using mode 1 */
	val = musb_readw(epio, MUSB_RXCSR);
	val &= ~MUSB_RXCSR_H_REQPKT;

	if (channel->desired_mode == 0)
		val &= ~MUSB_RXCSR_H_AUTOREQ;
	else
		val |= MUSB_RXCSR_H_AUTOREQ;
	val |= MUSB_RXCSR_DMAENAB;

	/* autoclear shouldn't be set in high bandwidth */
	if (qh->hb_mult == 1)
		val |= MUSB_RXCSR_AUTOCLEAR;

	musb_writew(epio, MUSB_RXCSR, MUSB_RXCSR_H_WZC_BITS | val);

	/* REVISIT if when actual_length != 0,
	 * transfer_buffer_length needs to be
	 * adjusted first...
	 */
	done = dma->channel_program(channel, qh->maxpacket,
				   channel->desired_mode,
				   buf, length);

	if (!done) {
		dma->channel_release(channel);
		hw_ep->rx_channel = NULL;
		channel = NULL;
		val = musb_readw(epio, MUSB_RXCSR);
		val &= ~(MUSB_RXCSR_DMAENAB
			 | MUSB_RXCSR_H_AUTOREQ
			 | MUSB_RXCSR_AUTOCLEAR);
		musb_writew(epio, MUSB_RXCSR, val);
	}

	return done;
}
#else
static inline int musb_rx_dma_inventra_cppi41(struct dma_controller *dma,
					      struct musb_hw_ep *hw_ep,
					      struct musb_qh *qh,
					      struct urb *urb,
					      size_t len)
{
	return false;
}

static inline int musb_rx_dma_in_inventra_cppi41(struct dma_controller *dma,
						 struct musb_hw_ep *hw_ep,
						 struct musb_qh *qh,
						 struct urb *urb,
						 size_t len,
						 u8 iso_err)
{
	return false;
}
#endif

/*
 * Service an RX interrupt for the given IN endpoint; docs cover bulk, iso,
 * and high-bandwidth IN transfer cases.
 */
void musb_host_rx(struct musb *musb, u8 epnum)
{
	struct urb		*urb;
	struct musb_hw_ep	*hw_ep = musb->endpoints + epnum;
	struct dma_controller	*c = musb->dma_controller;
	void __iomem		*epio = hw_ep->regs;
	struct musb_qh		*qh = hw_ep->in_qh;
	size_t			xfer_len;
	void __iomem		*mbase = musb->mregs;
	u16			rx_csr, val;
	bool			iso_err = false;
	bool			done = false;
	u32			status;
	struct dma_channel	*dma;
	unsigned int sg_flags = SG_MITER_ATOMIC | SG_MITER_TO_SG;

	musb_ep_select(mbase, epnum);

	urb = next_urb(qh);
	dma = is_dma_capable() ? hw_ep->rx_channel : NULL;
	status = 0;
	xfer_len = 0;

	rx_csr = musb_readw(epio, MUSB_RXCSR);
	val = rx_csr;

	if (unlikely(!urb)) {
		/* REVISIT -- THIS SHOULD NEVER HAPPEN ... but, at least
		 * usbtest #11 (unlinks) triggers it regularly, sometimes
		 * with fifo full.  (Only with DMA??)
		 */
		musb_dbg(musb, "BOGUS RX%d ready, csr %04x, count %d",
			epnum, val, musb_readw(epio, MUSB_RXCOUNT));
		musb_h_flush_rxfifo(hw_ep, MUSB_RXCSR_CLRDATATOG);
		return;
	}

	trace_musb_urb_rx(musb, urb);

	/* check for errors, concurrent stall & unlink is not really
	 * handled yet! */
	if (rx_csr & MUSB_RXCSR_H_RXSTALL) {
		musb_dbg(musb, "RX end %d STALL", epnum);

		/* stall; record URB status */
		status = -EPIPE;

	} else if (rx_csr & MUSB_RXCSR_H_ERROR) {
		dev_err(musb->controller, "ep%d RX three-strikes error", epnum);

		/*
		 * The three-strikes error could only happen when the USB
		 * device is not accessible, for example detached or powered
		 * off. So return the fatal error -ESHUTDOWN so hopefully the
		 * USB device drivers won't immediately resubmit the same URB.
		 */
		status = -ESHUTDOWN;
		musb_writeb(epio, MUSB_RXINTERVAL, 0);

		rx_csr &= ~MUSB_RXCSR_H_ERROR;
		musb_writew(epio, MUSB_RXCSR, rx_csr);

	} else if (rx_csr & MUSB_RXCSR_DATAERROR) {

		if (USB_ENDPOINT_XFER_ISOC != qh->type) {
			musb_dbg(musb, "RX end %d NAK timeout", epnum);

			/* NOTE: NAKing is *NOT* an error, so we want to
			 * continue.  Except ... if there's a request for
			 * another QH, use that instead of starving it.
			 *
			 * Devices like Ethernet and serial adapters keep
			 * reads posted at all times, which will starve
			 * other devices without this logic.
			 */
			if (usb_pipebulk(urb->pipe)
					&& qh->mux == 1
					&& !list_is_singular(&musb->in_bulk)) {
				musb_bulk_nak_timeout(musb, hw_ep, 1);
				return;
			}
			musb_ep_select(mbase, epnum);
			rx_csr |= MUSB_RXCSR_H_WZC_BITS;
			rx_csr &= ~MUSB_RXCSR_DATAERROR;
			musb_writew(epio, MUSB_RXCSR, rx_csr);

			goto finish;
		} else {
			musb_dbg(musb, "RX end %d ISO data error", epnum);
			/* packet error reported later */
			iso_err = true;
		}
	} else if (rx_csr & MUSB_RXCSR_INCOMPRX) {
		musb_dbg(musb, "end %d high bandwidth incomplete ISO packet RX",
				epnum);
		status = -EPROTO;
	}

	/* faults abort the transfer */
	if (status) {
		/* clean up dma and collect transfer count */
		if (dma_channel_status(dma) == MUSB_DMA_STATUS_BUSY) {
			dma->status = MUSB_DMA_STATUS_CORE_ABORT;
			musb->dma_controller->channel_abort(dma);
			xfer_len = dma->actual_len;
		}
		musb_h_flush_rxfifo(hw_ep, MUSB_RXCSR_CLRDATATOG);
		musb_writeb(epio, MUSB_RXINTERVAL, 0);
		done = true;
		goto finish;
	}

	if (unlikely(dma_channel_status(dma) == MUSB_DMA_STATUS_BUSY)) {
		/* SHOULD NEVER HAPPEN ... but at least DaVinci has done it */
		ERR("RX%d dma busy, csr %04x\n", epnum, rx_csr);
		goto finish;
	}

	/* thorough shutdown for now ... given more precise fault handling
	 * and better queueing support, we might keep a DMA pipeline going
	 * while processing this irq for earlier completions.
	 */

	/* FIXME this is _way_ too much in-line logic for Mentor DMA */
	if (!musb_dma_inventra(musb) && !musb_dma_ux500(musb) &&
	    (rx_csr & MUSB_RXCSR_H_REQPKT)) {
		/* REVISIT this happened for a while on some short reads...
		 * the cleanup still needs investigation... looks bad...
		 * and also duplicates dma cleanup code above ... plus,
		 * shouldn't this be the "half full" double buffer case?
		 */
		if (dma_channel_status(dma) == MUSB_DMA_STATUS_BUSY) {
			dma->status = MUSB_DMA_STATUS_CORE_ABORT;
			musb->dma_controller->channel_abort(dma);
			xfer_len = dma->actual_len;
			done = true;
		}

		musb_dbg(musb, "RXCSR%d %04x, reqpkt, len %zu%s", epnum, rx_csr,
				xfer_len, dma ? ", dma" : "");
		rx_csr &= ~MUSB_RXCSR_H_REQPKT;

		musb_ep_select(mbase, epnum);
		musb_writew(epio, MUSB_RXCSR,
				MUSB_RXCSR_H_WZC_BITS | rx_csr);
	}

	if (dma && (rx_csr & MUSB_RXCSR_DMAENAB)) {
		xfer_len = dma->actual_len;

		val &= ~(MUSB_RXCSR_DMAENAB
			| MUSB_RXCSR_H_AUTOREQ
			| MUSB_RXCSR_AUTOCLEAR
			| MUSB_RXCSR_RXPKTRDY);
		musb_writew(hw_ep->regs, MUSB_RXCSR, val);

		if (musb_dma_inventra(musb) || musb_dma_ux500(musb) ||
		    musb_dma_cppi41(musb)) {
			    done = musb_rx_dma_inventra_cppi41(c, hw_ep, qh, urb, xfer_len);
			    musb_dbg(hw_ep->musb,
				    "ep %d dma %s, rxcsr %04x, rxcount %d",
				    epnum, done ? "off" : "reset",
				    musb_readw(epio, MUSB_RXCSR),
				    musb_readw(epio, MUSB_RXCOUNT));
		} else {
			done = true;
		}

	} else if (urb->status == -EINPROGRESS) {
		/* if no errors, be sure a packet is ready for unloading */
		if (unlikely(!(rx_csr & MUSB_RXCSR_RXPKTRDY))) {
			status = -EPROTO;
			ERR("Rx interrupt with no errors or packet!\n");

			/* FIXME this is another "SHOULD NEVER HAPPEN" */

/* SCRUB (RX) */
			/* do the proper sequence to abort the transfer */
			musb_ep_select(mbase, epnum);
			val &= ~MUSB_RXCSR_H_REQPKT;
			musb_writew(epio, MUSB_RXCSR, val);
			goto finish;
		}

		/* we are expecting IN packets */
		if ((musb_dma_inventra(musb) || musb_dma_ux500(musb) ||
		    musb_dma_cppi41(musb)) && dma) {
			musb_dbg(hw_ep->musb,
				"RX%d count %d, buffer 0x%llx len %d/%d",
				epnum, musb_readw(epio, MUSB_RXCOUNT),
				(unsigned long long) urb->transfer_dma
				+ urb->actual_length,
				qh->offset,
				urb->transfer_buffer_length);

			if (musb_rx_dma_in_inventra_cppi41(c, hw_ep, qh, urb,
							   xfer_len, iso_err))
				goto finish;
			else
				dev_err(musb->controller, "error: rx_dma failed\n");
		}

		if (!dma) {
			unsigned int received_len;

			/* Unmap the buffer so that CPU can use it */
			usb_hcd_unmap_urb_for_dma(musb->hcd, urb);

			/*
			 * We need to map sg if the transfer_buffer is
			 * NULL.
			 */
			if (!urb->transfer_buffer) {
				qh->use_sg = true;
				sg_miter_start(&qh->sg_miter, urb->sg, 1,
						sg_flags);
			}

			if (qh->use_sg) {
				if (!sg_miter_next(&qh->sg_miter)) {
					dev_err(musb->controller, "error: sg list empty\n");
					sg_miter_stop(&qh->sg_miter);
					status = -EINVAL;
					done = true;
					goto finish;
				}
				urb->transfer_buffer = qh->sg_miter.addr;
				received_len = urb->actual_length;
				qh->offset = 0x0;
				done = musb_host_packet_rx(musb, urb, epnum,
						iso_err);
				/* Calculate the number of bytes received */
				received_len = urb->actual_length -
					received_len;
				qh->sg_miter.consumed = received_len;
				sg_miter_stop(&qh->sg_miter);
			} else {
				done = musb_host_packet_rx(musb, urb,
						epnum, iso_err);
			}
			musb_dbg(musb, "read %spacket", done ? "last " : "");
		}
	}

finish:
	urb->actual_length += xfer_len;
	qh->offset += xfer_len;
	if (done) {
		if (qh->use_sg) {
			qh->use_sg = false;
			urb->transfer_buffer = NULL;
		}

		if (urb->status == -EINPROGRESS)
			urb->status = status;
		musb_advance_schedule(musb, urb, hw_ep, USB_DIR_IN);
	}
}

/* schedule nodes correspond to peripheral endpoints, like an OHCI QH.
 * the software schedule associates multiple such nodes with a given
 * host side hardware endpoint + direction; scheduling may activate
 * that hardware endpoint.
 */
static int musb_schedule(
	struct musb		*musb,
	struct musb_qh		*qh,
	int			is_in)
{
	int			idle = 0;
	int			best_diff;
	int			best_end, epnum;
	struct musb_hw_ep	*hw_ep = NULL;
	struct list_head	*head = NULL;
	u8			toggle;
	u8			txtype;
	struct urb		*urb = next_urb(qh);

	/* use fixed hardware for control and bulk */
	if (qh->type == USB_ENDPOINT_XFER_CONTROL) {
		head = &musb->control;
		hw_ep = musb->control_ep;
		goto success;
	}

	/* else, periodic transfers get muxed to other endpoints */

	/*
	 * We know this qh hasn't been scheduled, so all we need to do
	 * is choose which hardware endpoint to put it on ...
	 *
	 * REVISIT what we really want here is a regular schedule tree
	 * like e.g. OHCI uses.
	 */
	best_diff = 4096;
	best_end = -1;

	for (epnum = 1, hw_ep = musb->endpoints + 1;
			epnum < musb->nr_endpoints;
			epnum++, hw_ep++) {
		int	diff;

		if (musb_ep_get_qh(hw_ep, is_in) != NULL)
			continue;

		if (hw_ep == musb->bulk_ep)
			continue;

		if (is_in)
			diff = hw_ep->max_packet_sz_rx;
		else
			diff = hw_ep->max_packet_sz_tx;
		diff -= (qh->maxpacket * qh->hb_mult);

		if (diff >= 0 && best_diff > diff) {

			/*
			 * Mentor controller has a bug in that if we schedule
			 * a BULK Tx transfer on an endpoint that had earlier
			 * handled ISOC then the BULK transfer has to start on
			 * a zero toggle.  If the BULK transfer starts on a 1
			 * toggle then this transfer will fail as the mentor
			 * controller starts the Bulk transfer on a 0 toggle
			 * irrespective of the programming of the toggle bits
			 * in the TXCSR register.  Check for this condition
			 * while allocating the EP for a Tx Bulk transfer.  If
			 * so skip this EP.
			 */
			hw_ep = musb->endpoints + epnum;
			toggle = usb_gettoggle(urb->dev, qh->epnum, !is_in);
			txtype = (musb_readb(hw_ep->regs, MUSB_TXTYPE)
					>> 4) & 0x3;
			if (!is_in && (qh->type == USB_ENDPOINT_XFER_BULK) &&
				toggle && (txtype == USB_ENDPOINT_XFER_ISOC))
				continue;

			best_diff = diff;
			best_end = epnum;
		}
	}
	/* use bulk reserved ep1 if no other ep is free */
	if (best_end < 0 && qh->type == USB_ENDPOINT_XFER_BULK) {
		hw_ep = musb->bulk_ep;
		if (is_in)
			head = &musb->in_bulk;
		else
			head = &musb->out_bulk;

		/* Enable bulk RX/TX NAK timeout scheme when bulk requests are
		 * multiplexed. This scheme does not work in high speed to full
		 * speed scenario as NAK interrupts are not coming from a
		 * full speed device connected to a high speed device.
		 * NAK timeout interval is 8 (128 uframe or 16ms) for HS and
		 * 4 (8 frame or 8ms) for FS device.
		 */
		if (qh->dev)
			qh->intv_reg =
				(USB_SPEED_HIGH == qh->dev->speed) ? 8 : 4;
		goto success;
	} else if (best_end < 0) {
		dev_err(musb->controller,
				"%s hwep alloc failed for %dx%d\n",
				musb_ep_xfertype_string(qh->type),
				qh->hb_mult, qh->maxpacket);
		return -ENOSPC;
	}

	idle = 1;
	qh->mux = 0;
	hw_ep = musb->endpoints + best_end;
	musb_dbg(musb, "qh %p periodic slot %d", qh, best_end);
success:
	if (head) {
		idle = list_empty(head);
		list_add_tail(&qh->ring, head);
		qh->mux = 1;
	}
	qh->hw_ep = hw_ep;
	qh->hep->hcpriv = qh;
	if (idle)
		musb_start_urb(musb, is_in, qh);
	return 0;
}

static int musb_urb_enqueue(
	struct usb_hcd			*hcd,
	struct urb			*urb,
	gfp_t				mem_flags)
{
	unsigned long			flags;
	struct musb			*musb = hcd_to_musb(hcd);
	struct usb_host_endpoint	*hep = urb->ep;
	struct musb_qh			*qh;
	struct usb_endpoint_descriptor	*epd = &hep->desc;
	int				ret;
	unsigned			type_reg;
	unsigned			interval;

	/* host role must be active */
	if (!is_host_active(musb) || !musb->is_active)
		return -ENODEV;

	trace_musb_urb_enq(musb, urb);

	spin_lock_irqsave(&musb->lock, flags);
	ret = usb_hcd_link_urb_to_ep(hcd, urb);
	qh = ret ? NULL : hep->hcpriv;
	if (qh)
		urb->hcpriv = qh;
	spin_unlock_irqrestore(&musb->lock, flags);

	/* DMA mapping was already done, if needed, and this urb is on
	 * hep->urb_list now ... so we're done, unless hep wasn't yet
	 * scheduled onto a live qh.
	 *
	 * REVISIT best to keep hep->hcpriv valid until the endpoint gets
	 * disabled, testing for empty qh->ring and avoiding qh setup costs
	 * except for the first urb queued after a config change.
	 */
	if (qh || ret)
		return ret;

	/* Allocate and initialize qh, minimizing the work done each time
	 * hw_ep gets reprogrammed, or with irqs blocked.  Then schedule it.
	 *
	 * REVISIT consider a dedicated qh kmem_cache, so it's harder
	 * for bugs in other kernel code to break this driver...
	 */
	qh = kzalloc(sizeof *qh, mem_flags);
	if (!qh) {
		spin_lock_irqsave(&musb->lock, flags);
		usb_hcd_unlink_urb_from_ep(hcd, urb);
		spin_unlock_irqrestore(&musb->lock, flags);
		return -ENOMEM;
	}

	qh->hep = hep;
	qh->dev = urb->dev;
	INIT_LIST_HEAD(&qh->ring);
	qh->is_ready = 1;

	qh->maxpacket = usb_endpoint_maxp(epd);
	qh->type = usb_endpoint_type(epd);

	/* Bits 11 & 12 of wMaxPacketSize encode high bandwidth multiplier.
	 * Some musb cores don't support high bandwidth ISO transfers; and
	 * we don't (yet!) support high bandwidth interrupt transfers.
	 */
	qh->hb_mult = usb_endpoint_maxp_mult(epd);
	if (qh->hb_mult > 1) {
		int ok = (qh->type == USB_ENDPOINT_XFER_ISOC);

		if (ok)
			ok = (usb_pipein(urb->pipe) && musb->hb_iso_rx)
				|| (usb_pipeout(urb->pipe) && musb->hb_iso_tx);
		if (!ok) {
			dev_err(musb->controller,
				"high bandwidth %s (%dx%d) not supported\n",
				musb_ep_xfertype_string(qh->type),
				qh->hb_mult, qh->maxpacket & 0x7ff);
			ret = -EMSGSIZE;
			goto done;
		}
		qh->maxpacket &= 0x7ff;
	}

	qh->epnum = usb_endpoint_num(epd);

	/* NOTE: urb->dev->devnum is wrong during SET_ADDRESS */
	qh->addr_reg = (u8) usb_pipedevice(urb->pipe);

	/* precompute rxtype/txtype/type0 register */
	type_reg = (qh->type << 4) | qh->epnum;
	switch (urb->dev->speed) {
	case USB_SPEED_LOW:
		type_reg |= 0xc0;
		break;
	case USB_SPEED_FULL:
		type_reg |= 0x80;
		break;
	default:
		type_reg |= 0x40;
	}
	qh->type_reg = type_reg;

	/* Precompute RXINTERVAL/TXINTERVAL register */
	switch (qh->type) {
	case USB_ENDPOINT_XFER_INT:
		/*
		 * Full/low speeds use the  linear encoding,
		 * high speed uses the logarithmic encoding.
		 */
		if (urb->dev->speed <= USB_SPEED_FULL) {
			interval = max_t(u8, epd->bInterval, 1);
			break;
		}
		fallthrough;
	case USB_ENDPOINT_XFER_ISOC:
		/* ISO always uses logarithmic encoding */
		interval = min_t(u8, epd->bInterval, 16);
		break;
	default:
		/* REVISIT we actually want to use NAK limits, hinting to the
		 * transfer scheduling logic to try some other qh, e.g. try
		 * for 2 msec first:
		 *
		 * interval = (USB_SPEED_HIGH == urb->dev->speed) ? 16 : 2;
		 *
		 * The downside of disabling this is that transfer scheduling
		 * gets VERY unfair for nonperiodic transfers; a misbehaving
		 * peripheral could make that hurt.  That's perfectly normal
		 * for reads from network or serial adapters ... so we have
		 * partial NAKlimit support for bulk RX.
		 *
		 * The upside of disabling it is simpler transfer scheduling.
		 */
		interval = 0;
	}
	qh->intv_reg = interval;

	/* precompute addressing for external hub/tt ports */
	if (musb->is_multipoint) {
		struct usb_device	*parent = urb->dev->parent;

		if (parent != hcd->self.root_hub) {
			qh->h_addr_reg = (u8) parent->devnum;

			/* set up tt info if needed */
			if (urb->dev->tt) {
				qh->h_port_reg = (u8) urb->dev->ttport;
				if (urb->dev->tt->hub)
					qh->h_addr_reg =
						(u8) urb->dev->tt->hub->devnum;
				if (urb->dev->tt->multi)
					qh->h_addr_reg |= 0x80;
			}
		}
	}

	/* invariant: hep->hcpriv is null OR the qh that's already scheduled.
	 * until we get real dma queues (with an entry for each urb/buffer),
	 * we only have work to do in the former case.
	 */
	spin_lock_irqsave(&musb->lock, flags);
	if (hep->hcpriv || !next_urb(qh)) {
		/* some concurrent activity submitted another urb to hep...
		 * odd, rare, error prone, but legal.
		 */
		kfree(qh);
		qh = NULL;
		ret = 0;
	} else
		ret = musb_schedule(musb, qh,
				epd->bEndpointAddress & USB_ENDPOINT_DIR_MASK);

	if (ret == 0) {
		urb->hcpriv = qh;
		/* FIXME set urb->start_frame for iso/intr, it's tested in
		 * musb_start_urb(), but otherwise only konicawc cares ...
		 */
	}
	spin_unlock_irqrestore(&musb->lock, flags);

done:
	if (ret != 0) {
		spin_lock_irqsave(&musb->lock, flags);
		usb_hcd_unlink_urb_from_ep(hcd, urb);
		spin_unlock_irqrestore(&musb->lock, flags);
		kfree(qh);
	}
	return ret;
}


/*
 * abort a transfer that's at the head of a hardware queue.
 * called with controller locked, irqs blocked
 * that hardware queue advances to the next transfer, unless prevented
 */
static int musb_cleanup_urb(struct urb *urb, struct musb_qh *qh)
{
	struct musb_hw_ep	*ep = qh->hw_ep;
	struct musb		*musb = ep->musb;
	void __iomem		*epio = ep->regs;
	unsigned		hw_end = ep->epnum;
	void __iomem		*regs = ep->musb->mregs;
	int			is_in = usb_pipein(urb->pipe);
	int			status = 0;
	u16			csr;
	struct dma_channel	*dma = NULL;

	musb_ep_select(regs, hw_end);

	if (is_dma_capable()) {
		dma = is_in ? ep->rx_channel : ep->tx_channel;
		if (dma) {
			status = ep->musb->dma_controller->channel_abort(dma);
			musb_dbg(musb, "abort %cX%d DMA for urb %p --> %d",
				is_in ? 'R' : 'T', ep->epnum,
				urb, status);
			urb->actual_length += dma->actual_len;
		}
	}

	/* turn off DMA requests, discard state, stop polling ... */
	if (ep->epnum && is_in) {
		/* giveback saves bulk toggle */
		csr = musb_h_flush_rxfifo(ep, 0);

		/* clear the endpoint's irq status here to avoid bogus irqs */
		if (is_dma_capable() && dma)
			musb_platform_clear_ep_rxintr(musb, ep->epnum);
	} else if (ep->epnum) {
		musb_h_tx_flush_fifo(ep);
		csr = musb_readw(epio, MUSB_TXCSR);
		csr &= ~(MUSB_TXCSR_AUTOSET
			| MUSB_TXCSR_DMAENAB
			| MUSB_TXCSR_H_RXSTALL
			| MUSB_TXCSR_H_NAKTIMEOUT
			| MUSB_TXCSR_H_ERROR
			| MUSB_TXCSR_TXPKTRDY);
		musb_writew(epio, MUSB_TXCSR, csr);
		/* REVISIT may need to clear FLUSHFIFO ... */
		musb_writew(epio, MUSB_TXCSR, csr);
		/* flush cpu writebuffer */
		csr = musb_readw(epio, MUSB_TXCSR);
	} else  {
		musb_h_ep0_flush_fifo(ep);
	}
	if (status == 0)
		musb_advance_schedule(ep->musb, urb, ep, is_in);
	return status;
}

static int musb_urb_dequeue(struct usb_hcd *hcd, struct urb *urb, int status)
{
	struct musb		*musb = hcd_to_musb(hcd);
	struct musb_qh		*qh;
	unsigned long		flags;
	int			is_in  = usb_pipein(urb->pipe);
	int			ret;

	trace_musb_urb_deq(musb, urb);

	spin_lock_irqsave(&musb->lock, flags);
	ret = usb_hcd_check_unlink_urb(hcd, urb, status);
	if (ret)
		goto done;

	qh = urb->hcpriv;
	if (!qh)
		goto done;

	/*
	 * Any URB not actively programmed into endpoint hardware can be
	 * immediately given back; that's any URB not at the head of an
	 * endpoint queue, unless someday we get real DMA queues.  And even
	 * if it's at the head, it might not be known to the hardware...
	 *
	 * Otherwise abort current transfer, pending DMA, etc.; urb->status
	 * has already been updated.  This is a synchronous abort; it'd be
	 * OK to hold off until after some IRQ, though.
	 *
	 * NOTE: qh is invalid unless !list_empty(&hep->urb_list)
	 */
	if (!qh->is_ready
			|| urb->urb_list.prev != &qh->hep->urb_list
			|| musb_ep_get_qh(qh->hw_ep, is_in) != qh) {
		int	ready = qh->is_ready;

		qh->is_ready = 0;
		musb_giveback(musb, urb, 0);
		qh->is_ready = ready;

		/* If nothing else (usually musb_giveback) is using it
		 * and its URB list has emptied, recycle this qh.
		 */
		if (ready && list_empty(&qh->hep->urb_list)) {
			qh->hep->hcpriv = NULL;
			list_del(&qh->ring);
			kfree(qh);
		}
	} else
		ret = musb_cleanup_urb(urb, qh);
done:
	spin_unlock_irqrestore(&musb->lock, flags);
	return ret;
}

/* disable an endpoint */
static void
musb_h_disable(struct usb_hcd *hcd, struct usb_host_endpoint *hep)
{
	u8			is_in = hep->desc.bEndpointAddress & USB_DIR_IN;
	unsigned long		flags;
	struct musb		*musb = hcd_to_musb(hcd);
	struct musb_qh		*qh;
	struct urb		*urb;

	spin_lock_irqsave(&musb->lock, flags);

	qh = hep->hcpriv;
	if (qh == NULL)
		goto exit;

	/* NOTE: qh is invalid unless !list_empty(&hep->urb_list) */

	/* Kick the first URB off the hardware, if needed */
	qh->is_ready = 0;
	if (musb_ep_get_qh(qh->hw_ep, is_in) == qh) {
		urb = next_urb(qh);

		/* make software (then hardware) stop ASAP */
		if (!urb->unlinked)
			urb->status = -ESHUTDOWN;

		/* cleanup */
		musb_cleanup_urb(urb, qh);

		/* Then nuke all the others ... and advance the
		 * queue on hw_ep (e.g. bulk ring) when we're done.
		 */
		while (!list_empty(&hep->urb_list)) {
			urb = next_urb(qh);
			urb->status = -ESHUTDOWN;
			musb_advance_schedule(musb, urb, qh->hw_ep, is_in);
		}
	} else {
		/* Just empty the queue; the hardware is busy with
		 * other transfers, and since !qh->is_ready nothing
		 * will activate any of these as it advances.
		 */
		while (!list_empty(&hep->urb_list))
			musb_giveback(musb, next_urb(qh), -ESHUTDOWN);

		hep->hcpriv = NULL;
		list_del(&qh->ring);
		kfree(qh);
	}
exit:
	spin_unlock_irqrestore(&musb->lock, flags);
}

static int musb_h_get_frame_number(struct usb_hcd *hcd)
{
	struct musb	*musb = hcd_to_musb(hcd);

	return musb_readw(musb->mregs, MUSB_FRAME);
}

static int musb_h_start(struct usb_hcd *hcd)
{
	struct musb	*musb = hcd_to_musb(hcd);

	/* NOTE: musb_start() is called when the hub driver turns
	 * on port power, or when (OTG) peripheral starts.
	 */
	hcd->state = HC_STATE_RUNNING;
	musb->port1_status = 0;
	return 0;
}

static void musb_h_stop(struct usb_hcd *hcd)
{
	musb_stop(hcd_to_musb(hcd));
	hcd->state = HC_STATE_HALT;
}

static int musb_bus_suspend(struct usb_hcd *hcd)
{
	struct musb	*musb = hcd_to_musb(hcd);
	u8		devctl;
	int		ret;

	ret = musb_port_suspend(musb, true);
	if (ret)
		return ret;

	if (!is_host_active(musb))
		return 0;

	switch (musb->xceiv->otg->state) {
	case OTG_STATE_A_SUSPEND:
		return 0;
	case OTG_STATE_A_WAIT_VRISE:
		/* ID could be grounded even if there's no device
		 * on the other end of the cable.  NOTE that the
		 * A_WAIT_VRISE timers are messy with MUSB...
		 */
		devctl = musb_readb(musb->mregs, MUSB_DEVCTL);
		if ((devctl & MUSB_DEVCTL_VBUS) == MUSB_DEVCTL_VBUS)
			musb->xceiv->otg->state = OTG_STATE_A_WAIT_BCON;
		break;
	default:
		break;
	}

	if (musb->is_active) {
		WARNING("trying to suspend as %s while active\n",
				usb_otg_state_string(musb->xceiv->otg->state));
		return -EBUSY;
	} else
		return 0;
}

static int musb_bus_resume(struct usb_hcd *hcd)
{
	struct musb *musb = hcd_to_musb(hcd);

	if (musb->config &&
	    musb->config->host_port_deassert_reset_at_resume)
		musb_port_reset(musb, false);

	return 0;
}

#ifndef CONFIG_MUSB_PIO_ONLY

#define MUSB_USB_DMA_ALIGN 4

struct musb_temp_buffer {
	void *kmalloc_ptr;
	void *old_xfer_buffer;
	u8 data[];
};

static void musb_free_temp_buffer(struct urb *urb)
{
	enum dma_data_direction dir;
	struct musb_temp_buffer *temp;
	size_t length;

	if (!(urb->transfer_flags & URB_ALIGNED_TEMP_BUFFER))
		return;

	dir = usb_urb_dir_in(urb) ? DMA_FROM_DEVICE : DMA_TO_DEVICE;

	temp = container_of(urb->transfer_buffer, struct musb_temp_buffer,
			    data);

	if (dir == DMA_FROM_DEVICE) {
		if (usb_pipeisoc(urb->pipe))
			length = urb->transfer_buffer_length;
		else
			length = urb->actual_length;

		memcpy(temp->old_xfer_buffer, temp->data, length);
	}
	urb->transfer_buffer = temp->old_xfer_buffer;
	kfree(temp->kmalloc_ptr);

	urb->transfer_flags &= ~URB_ALIGNED_TEMP_BUFFER;
}

static int musb_alloc_temp_buffer(struct urb *urb, gfp_t mem_flags)
{
	enum dma_data_direction dir;
	struct musb_temp_buffer *temp;
	void *kmalloc_ptr;
	size_t kmalloc_size;

	if (urb->num_sgs || urb->sg ||
	    urb->transfer_buffer_length == 0 ||
	    !((uintptr_t)urb->transfer_buffer & (MUSB_USB_DMA_ALIGN - 1)))
		return 0;

	dir = usb_urb_dir_in(urb) ? DMA_FROM_DEVICE : DMA_TO_DEVICE;

	/* Allocate a buffer with enough padding for alignment */
	kmalloc_size = urb->transfer_buffer_length +
		sizeof(struct musb_temp_buffer) + MUSB_USB_DMA_ALIGN - 1;

	kmalloc_ptr = kmalloc(kmalloc_size, mem_flags);
	if (!kmalloc_ptr)
		return -ENOMEM;

	/* Position our struct temp_buffer such that data is aligned */
	temp = PTR_ALIGN(kmalloc_ptr, MUSB_USB_DMA_ALIGN);


	temp->kmalloc_ptr = kmalloc_ptr;
	temp->old_xfer_buffer = urb->transfer_buffer;
	if (dir == DMA_TO_DEVICE)
		memcpy(temp->data, urb->transfer_buffer,
		       urb->transfer_buffer_length);
	urb->transfer_buffer = temp->data;

	urb->transfer_flags |= URB_ALIGNED_TEMP_BUFFER;

	return 0;
}

static int musb_map_urb_for_dma(struct usb_hcd *hcd, struct urb *urb,
				      gfp_t mem_flags)
{
	struct musb	*musb = hcd_to_musb(hcd);
	int ret;

	/*
	 * The DMA engine in RTL1.8 and above cannot handle
	 * DMA addresses that are not aligned to a 4 byte boundary.
	 * For such engine implemented (un)map_urb_for_dma hooks.
	 * Do not use these hooks for RTL<1.8
	 */
	if (musb->hwvers < MUSB_HWVERS_1800)
		return usb_hcd_map_urb_for_dma(hcd, urb, mem_flags);

	ret = musb_alloc_temp_buffer(urb, mem_flags);
	if (ret)
		return ret;

	ret = usb_hcd_map_urb_for_dma(hcd, urb, mem_flags);
	if (ret)
		musb_free_temp_buffer(urb);

	return ret;
}

static void musb_unmap_urb_for_dma(struct usb_hcd *hcd, struct urb *urb)
{
	struct musb	*musb = hcd_to_musb(hcd);

	usb_hcd_unmap_urb_for_dma(hcd, urb);

	/* Do not use this hook for RTL<1.8 (see description above) */
	if (musb->hwvers < MUSB_HWVERS_1800)
		return;

	musb_free_temp_buffer(urb);
}
#endif /* !CONFIG_MUSB_PIO_ONLY */

static const struct hc_driver musb_hc_driver = {
	.description		= "musb-hcd",
	.product_desc		= "MUSB HDRC host driver",
	.hcd_priv_size		= sizeof(struct musb *),
	.flags			= HCD_USB2 | HCD_DMA | HCD_MEMORY,

	/* not using irq handler or reset hooks from usbcore, since
	 * those must be shared with peripheral code for OTG configs
	 */

	.start			= musb_h_start,
	.stop			= musb_h_stop,

	.get_frame_number	= musb_h_get_frame_number,

	.urb_enqueue		= musb_urb_enqueue,
	.urb_dequeue		= musb_urb_dequeue,
	.endpoint_disable	= musb_h_disable,

#ifndef CONFIG_MUSB_PIO_ONLY
	.map_urb_for_dma	= musb_map_urb_for_dma,
	.unmap_urb_for_dma	= musb_unmap_urb_for_dma,
#endif

	.hub_status_data	= musb_hub_status_data,
	.hub_control		= musb_hub_control,
	.bus_suspend		= musb_bus_suspend,
	.bus_resume		= musb_bus_resume,
	/* .start_port_reset	= NULL, */
	/* .hub_irq_enable	= NULL, */
};

int musb_host_alloc(struct musb *musb)
{
	struct device	*dev = musb->controller;

	/* usbcore sets dev->driver_data to hcd, and sometimes uses that... */
	musb->hcd = usb_create_hcd(&musb_hc_driver, dev, dev_name(dev));
	if (!musb->hcd)
		return -EINVAL;

	*musb->hcd->hcd_priv = (unsigned long) musb;
	musb->hcd->self.uses_pio_for_control = 1;
	musb->hcd->uses_new_polling = 1;
	musb->hcd->has_tt = 1;

	return 0;
}

void musb_host_cleanup(struct musb *musb)
{
	if (musb->port_mode == MUSB_PERIPHERAL)
		return;
	usb_remove_hcd(musb->hcd);
}

void musb_host_free(struct musb *musb)
{
	usb_put_hcd(musb->hcd);
}

int musb_host_setup(struct musb *musb, int power_budget)
{
	int ret;
	struct usb_hcd *hcd = musb->hcd;

	if (musb->port_mode == MUSB_HOST) {
		MUSB_HST_MODE(musb);
		musb->xceiv->otg->state = OTG_STATE_A_IDLE;
	}
	otg_set_host(musb->xceiv->otg, &hcd->self);
	/* don't support otg protocols */
	hcd->self.otg_port = 0;
	musb->xceiv->otg->host = &hcd->self;
	hcd->power_budget = 2 * (power_budget ? : 250);
	hcd->skip_phy_initialization = 1;

	ret = usb_add_hcd(hcd, 0, 0);
	if (ret < 0)
		return ret;

	device_wakeup_enable(hcd->self.controller);
	return 0;
}

void musb_host_resume_root_hub(struct musb *musb)
{
	usb_hcd_resume_root_hub(musb->hcd);
}

void musb_host_poke_root_hub(struct musb *musb)
{
	MUSB_HST_MODE(musb);
	if (musb->hcd->status_urb)
		usb_hcd_poll_rh_status(musb->hcd);
	else
		usb_hcd_resume_root_hub(musb->hcd);
}
