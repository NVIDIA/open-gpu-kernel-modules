// SPDX-License-Identifier: GPL-2.0
/*
 * Universal Host Controller Interface driver for USB.
 *
 * Maintainer: Alan Stern <stern@rowland.harvard.edu>
 *
 * (C) Copyright 1999 Linus Torvalds
 * (C) Copyright 1999-2002 Johannes Erdfelt, johannes@erdfelt.com
 * (C) Copyright 1999 Randy Dunlap
 * (C) Copyright 1999 Georg Acher, acher@in.tum.de
 * (C) Copyright 1999 Deti Fliegl, deti@fliegl.de
 * (C) Copyright 1999 Thomas Sailer, sailer@ife.ee.ethz.ch
 * (C) Copyright 1999 Roman Weissgaerber, weissg@vienna.at
 * (C) Copyright 2000 Yggdrasil Computing, Inc. (port of new PCI interface
 *               support from usb-ohci.c by Adam Richter, adam@yggdrasil.com).
 * (C) Copyright 1999 Gregory P. Smith (from usb-ohci.c)
 * (C) Copyright 2004-2007 Alan Stern, stern@rowland.harvard.edu
 */


/*
 * Technically, updating td->status here is a race, but it's not really a
 * problem. The worst that can happen is that we set the IOC bit again
 * generating a spurious interrupt. We could fix this by creating another
 * QH and leaving the IOC bit always set, but then we would have to play
 * games with the FSBR code to make sure we get the correct order in all
 * the cases. I don't think it's worth the effort
 */
static void uhci_set_next_interrupt(struct uhci_hcd *uhci)
{
	if (uhci->is_stopped)
		mod_timer(&uhci_to_hcd(uhci)->rh_timer, jiffies);
	uhci->term_td->status |= cpu_to_hc32(uhci, TD_CTRL_IOC);
}

static inline void uhci_clear_next_interrupt(struct uhci_hcd *uhci)
{
	uhci->term_td->status &= ~cpu_to_hc32(uhci, TD_CTRL_IOC);
}


/*
 * Full-Speed Bandwidth Reclamation (FSBR).
 * We turn on FSBR whenever a queue that wants it is advancing,
 * and leave it on for a short time thereafter.
 */
static void uhci_fsbr_on(struct uhci_hcd *uhci)
{
	struct uhci_qh *lqh;

	/* The terminating skeleton QH always points back to the first
	 * FSBR QH.  Make the last async QH point to the terminating
	 * skeleton QH. */
	uhci->fsbr_is_on = 1;
	lqh = list_entry(uhci->skel_async_qh->node.prev,
			struct uhci_qh, node);
	lqh->link = LINK_TO_QH(uhci, uhci->skel_term_qh);
}

static void uhci_fsbr_off(struct uhci_hcd *uhci)
{
	struct uhci_qh *lqh;

	/* Remove the link from the last async QH to the terminating
	 * skeleton QH. */
	uhci->fsbr_is_on = 0;
	lqh = list_entry(uhci->skel_async_qh->node.prev,
			struct uhci_qh, node);
	lqh->link = UHCI_PTR_TERM(uhci);
}

static void uhci_add_fsbr(struct uhci_hcd *uhci, struct urb *urb)
{
	struct urb_priv *urbp = urb->hcpriv;

	urbp->fsbr = 1;
}

static void uhci_urbp_wants_fsbr(struct uhci_hcd *uhci, struct urb_priv *urbp)
{
	if (urbp->fsbr) {
		uhci->fsbr_is_wanted = 1;
		if (!uhci->fsbr_is_on)
			uhci_fsbr_on(uhci);
		else if (uhci->fsbr_expiring) {
			uhci->fsbr_expiring = 0;
			del_timer(&uhci->fsbr_timer);
		}
	}
}

static void uhci_fsbr_timeout(struct timer_list *t)
{
	struct uhci_hcd *uhci = from_timer(uhci, t, fsbr_timer);
	unsigned long flags;

	spin_lock_irqsave(&uhci->lock, flags);
	if (uhci->fsbr_expiring) {
		uhci->fsbr_expiring = 0;
		uhci_fsbr_off(uhci);
	}
	spin_unlock_irqrestore(&uhci->lock, flags);
}


static struct uhci_td *uhci_alloc_td(struct uhci_hcd *uhci)
{
	dma_addr_t dma_handle;
	struct uhci_td *td;

	td = dma_pool_alloc(uhci->td_pool, GFP_ATOMIC, &dma_handle);
	if (!td)
		return NULL;

	td->dma_handle = dma_handle;
	td->frame = -1;

	INIT_LIST_HEAD(&td->list);
	INIT_LIST_HEAD(&td->fl_list);

	return td;
}

static void uhci_free_td(struct uhci_hcd *uhci, struct uhci_td *td)
{
	if (!list_empty(&td->list))
		dev_WARN(uhci_dev(uhci), "td %p still in list!\n", td);
	if (!list_empty(&td->fl_list))
		dev_WARN(uhci_dev(uhci), "td %p still in fl_list!\n", td);

	dma_pool_free(uhci->td_pool, td, td->dma_handle);
}

static inline void uhci_fill_td(struct uhci_hcd *uhci, struct uhci_td *td,
		u32 status, u32 token, u32 buffer)
{
	td->status = cpu_to_hc32(uhci, status);
	td->token = cpu_to_hc32(uhci, token);
	td->buffer = cpu_to_hc32(uhci, buffer);
}

static void uhci_add_td_to_urbp(struct uhci_td *td, struct urb_priv *urbp)
{
	list_add_tail(&td->list, &urbp->td_list);
}

static void uhci_remove_td_from_urbp(struct uhci_td *td)
{
	list_del_init(&td->list);
}

/*
 * We insert Isochronous URBs directly into the frame list at the beginning
 */
static inline void uhci_insert_td_in_frame_list(struct uhci_hcd *uhci,
		struct uhci_td *td, unsigned framenum)
{
	framenum &= (UHCI_NUMFRAMES - 1);

	td->frame = framenum;

	/* Is there a TD already mapped there? */
	if (uhci->frame_cpu[framenum]) {
		struct uhci_td *ftd, *ltd;

		ftd = uhci->frame_cpu[framenum];
		ltd = list_entry(ftd->fl_list.prev, struct uhci_td, fl_list);

		list_add_tail(&td->fl_list, &ftd->fl_list);

		td->link = ltd->link;
		wmb();
		ltd->link = LINK_TO_TD(uhci, td);
	} else {
		td->link = uhci->frame[framenum];
		wmb();
		uhci->frame[framenum] = LINK_TO_TD(uhci, td);
		uhci->frame_cpu[framenum] = td;
	}
}

static inline void uhci_remove_td_from_frame_list(struct uhci_hcd *uhci,
		struct uhci_td *td)
{
	/* If it's not inserted, don't remove it */
	if (td->frame == -1) {
		WARN_ON(!list_empty(&td->fl_list));
		return;
	}

	if (uhci->frame_cpu[td->frame] == td) {
		if (list_empty(&td->fl_list)) {
			uhci->frame[td->frame] = td->link;
			uhci->frame_cpu[td->frame] = NULL;
		} else {
			struct uhci_td *ntd;

			ntd = list_entry(td->fl_list.next,
					 struct uhci_td,
					 fl_list);
			uhci->frame[td->frame] = LINK_TO_TD(uhci, ntd);
			uhci->frame_cpu[td->frame] = ntd;
		}
	} else {
		struct uhci_td *ptd;

		ptd = list_entry(td->fl_list.prev, struct uhci_td, fl_list);
		ptd->link = td->link;
	}

	list_del_init(&td->fl_list);
	td->frame = -1;
}

static inline void uhci_remove_tds_from_frame(struct uhci_hcd *uhci,
		unsigned int framenum)
{
	struct uhci_td *ftd, *ltd;

	framenum &= (UHCI_NUMFRAMES - 1);

	ftd = uhci->frame_cpu[framenum];
	if (ftd) {
		ltd = list_entry(ftd->fl_list.prev, struct uhci_td, fl_list);
		uhci->frame[framenum] = ltd->link;
		uhci->frame_cpu[framenum] = NULL;

		while (!list_empty(&ftd->fl_list))
			list_del_init(ftd->fl_list.prev);
	}
}

/*
 * Remove all the TDs for an Isochronous URB from the frame list
 */
static void uhci_unlink_isochronous_tds(struct uhci_hcd *uhci, struct urb *urb)
{
	struct urb_priv *urbp = (struct urb_priv *) urb->hcpriv;
	struct uhci_td *td;

	list_for_each_entry(td, &urbp->td_list, list)
		uhci_remove_td_from_frame_list(uhci, td);
}

static struct uhci_qh *uhci_alloc_qh(struct uhci_hcd *uhci,
		struct usb_device *udev, struct usb_host_endpoint *hep)
{
	dma_addr_t dma_handle;
	struct uhci_qh *qh;

	qh = dma_pool_zalloc(uhci->qh_pool, GFP_ATOMIC, &dma_handle);
	if (!qh)
		return NULL;

	qh->dma_handle = dma_handle;

	qh->element = UHCI_PTR_TERM(uhci);
	qh->link = UHCI_PTR_TERM(uhci);

	INIT_LIST_HEAD(&qh->queue);
	INIT_LIST_HEAD(&qh->node);

	if (udev) {		/* Normal QH */
		qh->type = usb_endpoint_type(&hep->desc);
		if (qh->type != USB_ENDPOINT_XFER_ISOC) {
			qh->dummy_td = uhci_alloc_td(uhci);
			if (!qh->dummy_td) {
				dma_pool_free(uhci->qh_pool, qh, dma_handle);
				return NULL;
			}
		}
		qh->state = QH_STATE_IDLE;
		qh->hep = hep;
		qh->udev = udev;
		hep->hcpriv = qh;

		if (qh->type == USB_ENDPOINT_XFER_INT ||
				qh->type == USB_ENDPOINT_XFER_ISOC)
			qh->load = usb_calc_bus_time(udev->speed,
					usb_endpoint_dir_in(&hep->desc),
					qh->type == USB_ENDPOINT_XFER_ISOC,
					usb_endpoint_maxp(&hep->desc))
				/ 1000 + 1;

	} else {		/* Skeleton QH */
		qh->state = QH_STATE_ACTIVE;
		qh->type = -1;
	}
	return qh;
}

static void uhci_free_qh(struct uhci_hcd *uhci, struct uhci_qh *qh)
{
	WARN_ON(qh->state != QH_STATE_IDLE && qh->udev);
	if (!list_empty(&qh->queue))
		dev_WARN(uhci_dev(uhci), "qh %p list not empty!\n", qh);

	list_del(&qh->node);
	if (qh->udev) {
		qh->hep->hcpriv = NULL;
		if (qh->dummy_td)
			uhci_free_td(uhci, qh->dummy_td);
	}
	dma_pool_free(uhci->qh_pool, qh, qh->dma_handle);
}

/*
 * When a queue is stopped and a dequeued URB is given back, adjust
 * the previous TD link (if the URB isn't first on the queue) or
 * save its toggle value (if it is first and is currently executing).
 *
 * Returns 0 if the URB should not yet be given back, 1 otherwise.
 */
static int uhci_cleanup_queue(struct uhci_hcd *uhci, struct uhci_qh *qh,
		struct urb *urb)
{
	struct urb_priv *urbp = urb->hcpriv;
	struct uhci_td *td;
	int ret = 1;

	/* Isochronous pipes don't use toggles and their TD link pointers
	 * get adjusted during uhci_urb_dequeue().  But since their queues
	 * cannot truly be stopped, we have to watch out for dequeues
	 * occurring after the nominal unlink frame. */
	if (qh->type == USB_ENDPOINT_XFER_ISOC) {
		ret = (uhci->frame_number + uhci->is_stopped !=
				qh->unlink_frame);
		goto done;
	}

	/* If the URB isn't first on its queue, adjust the link pointer
	 * of the last TD in the previous URB.  The toggle doesn't need
	 * to be saved since this URB can't be executing yet. */
	if (qh->queue.next != &urbp->node) {
		struct urb_priv *purbp;
		struct uhci_td *ptd;

		purbp = list_entry(urbp->node.prev, struct urb_priv, node);
		WARN_ON(list_empty(&purbp->td_list));
		ptd = list_entry(purbp->td_list.prev, struct uhci_td,
				list);
		td = list_entry(urbp->td_list.prev, struct uhci_td,
				list);
		ptd->link = td->link;
		goto done;
	}

	/* If the QH element pointer is UHCI_PTR_TERM then then currently
	 * executing URB has already been unlinked, so this one isn't it. */
	if (qh_element(qh) == UHCI_PTR_TERM(uhci))
		goto done;
	qh->element = UHCI_PTR_TERM(uhci);

	/* Control pipes don't have to worry about toggles */
	if (qh->type == USB_ENDPOINT_XFER_CONTROL)
		goto done;

	/* Save the next toggle value */
	WARN_ON(list_empty(&urbp->td_list));
	td = list_entry(urbp->td_list.next, struct uhci_td, list);
	qh->needs_fixup = 1;
	qh->initial_toggle = uhci_toggle(td_token(uhci, td));

done:
	return ret;
}

/*
 * Fix up the data toggles for URBs in a queue, when one of them
 * terminates early (short transfer, error, or dequeued).
 */
static void uhci_fixup_toggles(struct uhci_hcd *uhci, struct uhci_qh *qh,
			int skip_first)
{
	struct urb_priv *urbp = NULL;
	struct uhci_td *td;
	unsigned int toggle = qh->initial_toggle;
	unsigned int pipe;

	/* Fixups for a short transfer start with the second URB in the
	 * queue (the short URB is the first). */
	if (skip_first)
		urbp = list_entry(qh->queue.next, struct urb_priv, node);

	/* When starting with the first URB, if the QH element pointer is
	 * still valid then we know the URB's toggles are okay. */
	else if (qh_element(qh) != UHCI_PTR_TERM(uhci))
		toggle = 2;

	/* Fix up the toggle for the URBs in the queue.  Normally this
	 * loop won't run more than once: When an error or short transfer
	 * occurs, the queue usually gets emptied. */
	urbp = list_prepare_entry(urbp, &qh->queue, node);
	list_for_each_entry_continue(urbp, &qh->queue, node) {

		/* If the first TD has the right toggle value, we don't
		 * need to change any toggles in this URB */
		td = list_entry(urbp->td_list.next, struct uhci_td, list);
		if (toggle > 1 || uhci_toggle(td_token(uhci, td)) == toggle) {
			td = list_entry(urbp->td_list.prev, struct uhci_td,
					list);
			toggle = uhci_toggle(td_token(uhci, td)) ^ 1;

		/* Otherwise all the toggles in the URB have to be switched */
		} else {
			list_for_each_entry(td, &urbp->td_list, list) {
				td->token ^= cpu_to_hc32(uhci,
							TD_TOKEN_TOGGLE);
				toggle ^= 1;
			}
		}
	}

	wmb();
	pipe = list_entry(qh->queue.next, struct urb_priv, node)->urb->pipe;
	usb_settoggle(qh->udev, usb_pipeendpoint(pipe),
			usb_pipeout(pipe), toggle);
	qh->needs_fixup = 0;
}

/*
 * Link an Isochronous QH into its skeleton's list
 */
static inline void link_iso(struct uhci_hcd *uhci, struct uhci_qh *qh)
{
	list_add_tail(&qh->node, &uhci->skel_iso_qh->node);

	/* Isochronous QHs aren't linked by the hardware */
}

/*
 * Link a high-period interrupt QH into the schedule at the end of its
 * skeleton's list
 */
static void link_interrupt(struct uhci_hcd *uhci, struct uhci_qh *qh)
{
	struct uhci_qh *pqh;

	list_add_tail(&qh->node, &uhci->skelqh[qh->skel]->node);

	pqh = list_entry(qh->node.prev, struct uhci_qh, node);
	qh->link = pqh->link;
	wmb();
	pqh->link = LINK_TO_QH(uhci, qh);
}

/*
 * Link a period-1 interrupt or async QH into the schedule at the
 * correct spot in the async skeleton's list, and update the FSBR link
 */
static void link_async(struct uhci_hcd *uhci, struct uhci_qh *qh)
{
	struct uhci_qh *pqh;
	__hc32 link_to_new_qh;

	/* Find the predecessor QH for our new one and insert it in the list.
	 * The list of QHs is expected to be short, so linear search won't
	 * take too long. */
	list_for_each_entry_reverse(pqh, &uhci->skel_async_qh->node, node) {
		if (pqh->skel <= qh->skel)
			break;
	}
	list_add(&qh->node, &pqh->node);

	/* Link it into the schedule */
	qh->link = pqh->link;
	wmb();
	link_to_new_qh = LINK_TO_QH(uhci, qh);
	pqh->link = link_to_new_qh;

	/* If this is now the first FSBR QH, link the terminating skeleton
	 * QH to it. */
	if (pqh->skel < SKEL_FSBR && qh->skel >= SKEL_FSBR)
		uhci->skel_term_qh->link = link_to_new_qh;
}

/*
 * Put a QH on the schedule in both hardware and software
 */
static void uhci_activate_qh(struct uhci_hcd *uhci, struct uhci_qh *qh)
{
	WARN_ON(list_empty(&qh->queue));

	/* Set the element pointer if it isn't set already.
	 * This isn't needed for Isochronous queues, but it doesn't hurt. */
	if (qh_element(qh) == UHCI_PTR_TERM(uhci)) {
		struct urb_priv *urbp = list_entry(qh->queue.next,
				struct urb_priv, node);
		struct uhci_td *td = list_entry(urbp->td_list.next,
				struct uhci_td, list);

		qh->element = LINK_TO_TD(uhci, td);
	}

	/* Treat the queue as if it has just advanced */
	qh->wait_expired = 0;
	qh->advance_jiffies = jiffies;

	if (qh->state == QH_STATE_ACTIVE)
		return;
	qh->state = QH_STATE_ACTIVE;

	/* Move the QH from its old list to the correct spot in the appropriate
	 * skeleton's list */
	if (qh == uhci->next_qh)
		uhci->next_qh = list_entry(qh->node.next, struct uhci_qh,
				node);
	list_del(&qh->node);

	if (qh->skel == SKEL_ISO)
		link_iso(uhci, qh);
	else if (qh->skel < SKEL_ASYNC)
		link_interrupt(uhci, qh);
	else
		link_async(uhci, qh);
}

/*
 * Unlink a high-period interrupt QH from the schedule
 */
static void unlink_interrupt(struct uhci_hcd *uhci, struct uhci_qh *qh)
{
	struct uhci_qh *pqh;

	pqh = list_entry(qh->node.prev, struct uhci_qh, node);
	pqh->link = qh->link;
	mb();
}

/*
 * Unlink a period-1 interrupt or async QH from the schedule
 */
static void unlink_async(struct uhci_hcd *uhci, struct uhci_qh *qh)
{
	struct uhci_qh *pqh;
	__hc32 link_to_next_qh = qh->link;

	pqh = list_entry(qh->node.prev, struct uhci_qh, node);
	pqh->link = link_to_next_qh;

	/* If this was the old first FSBR QH, link the terminating skeleton
	 * QH to the next (new first FSBR) QH. */
	if (pqh->skel < SKEL_FSBR && qh->skel >= SKEL_FSBR)
		uhci->skel_term_qh->link = link_to_next_qh;
	mb();
}

/*
 * Take a QH off the hardware schedule
 */
static void uhci_unlink_qh(struct uhci_hcd *uhci, struct uhci_qh *qh)
{
	if (qh->state == QH_STATE_UNLINKING)
		return;
	WARN_ON(qh->state != QH_STATE_ACTIVE || !qh->udev);
	qh->state = QH_STATE_UNLINKING;

	/* Unlink the QH from the schedule and record when we did it */
	if (qh->skel == SKEL_ISO)
		;
	else if (qh->skel < SKEL_ASYNC)
		unlink_interrupt(uhci, qh);
	else
		unlink_async(uhci, qh);

	uhci_get_current_frame_number(uhci);
	qh->unlink_frame = uhci->frame_number;

	/* Force an interrupt so we know when the QH is fully unlinked */
	if (list_empty(&uhci->skel_unlink_qh->node) || uhci->is_stopped)
		uhci_set_next_interrupt(uhci);

	/* Move the QH from its old list to the end of the unlinking list */
	if (qh == uhci->next_qh)
		uhci->next_qh = list_entry(qh->node.next, struct uhci_qh,
				node);
	list_move_tail(&qh->node, &uhci->skel_unlink_qh->node);
}

/*
 * When we and the controller are through with a QH, it becomes IDLE.
 * This happens when a QH has been off the schedule (on the unlinking
 * list) for more than one frame, or when an error occurs while adding
 * the first URB onto a new QH.
 */
static void uhci_make_qh_idle(struct uhci_hcd *uhci, struct uhci_qh *qh)
{
	WARN_ON(qh->state == QH_STATE_ACTIVE);

	if (qh == uhci->next_qh)
		uhci->next_qh = list_entry(qh->node.next, struct uhci_qh,
				node);
	list_move(&qh->node, &uhci->idle_qh_list);
	qh->state = QH_STATE_IDLE;

	/* Now that the QH is idle, its post_td isn't being used */
	if (qh->post_td) {
		uhci_free_td(uhci, qh->post_td);
		qh->post_td = NULL;
	}

	/* If anyone is waiting for a QH to become idle, wake them up */
	if (uhci->num_waiting)
		wake_up_all(&uhci->waitqh);
}

/*
 * Find the highest existing bandwidth load for a given phase and period.
 */
static int uhci_highest_load(struct uhci_hcd *uhci, int phase, int period)
{
	int highest_load = uhci->load[phase];

	for (phase += period; phase < MAX_PHASE; phase += period)
		highest_load = max_t(int, highest_load, uhci->load[phase]);
	return highest_load;
}

/*
 * Set qh->phase to the optimal phase for a periodic transfer and
 * check whether the bandwidth requirement is acceptable.
 */
static int uhci_check_bandwidth(struct uhci_hcd *uhci, struct uhci_qh *qh)
{
	int minimax_load;

	/* Find the optimal phase (unless it is already set) and get
	 * its load value. */
	if (qh->phase >= 0)
		minimax_load = uhci_highest_load(uhci, qh->phase, qh->period);
	else {
		int phase, load;
		int max_phase = min_t(int, MAX_PHASE, qh->period);

		qh->phase = 0;
		minimax_load = uhci_highest_load(uhci, qh->phase, qh->period);
		for (phase = 1; phase < max_phase; ++phase) {
			load = uhci_highest_load(uhci, phase, qh->period);
			if (load < minimax_load) {
				minimax_load = load;
				qh->phase = phase;
			}
		}
	}

	/* Maximum allowable periodic bandwidth is 90%, or 900 us per frame */
	if (minimax_load + qh->load > 900) {
		dev_dbg(uhci_dev(uhci), "bandwidth allocation failed: "
				"period %d, phase %d, %d + %d us\n",
				qh->period, qh->phase, minimax_load, qh->load);
		return -ENOSPC;
	}
	return 0;
}

/*
 * Reserve a periodic QH's bandwidth in the schedule
 */
static void uhci_reserve_bandwidth(struct uhci_hcd *uhci, struct uhci_qh *qh)
{
	int i;
	int load = qh->load;
	char *p = "??";

	for (i = qh->phase; i < MAX_PHASE; i += qh->period) {
		uhci->load[i] += load;
		uhci->total_load += load;
	}
	uhci_to_hcd(uhci)->self.bandwidth_allocated =
			uhci->total_load / MAX_PHASE;
	switch (qh->type) {
	case USB_ENDPOINT_XFER_INT:
		++uhci_to_hcd(uhci)->self.bandwidth_int_reqs;
		p = "INT";
		break;
	case USB_ENDPOINT_XFER_ISOC:
		++uhci_to_hcd(uhci)->self.bandwidth_isoc_reqs;
		p = "ISO";
		break;
	}
	qh->bandwidth_reserved = 1;
	dev_dbg(uhci_dev(uhci),
			"%s dev %d ep%02x-%s, period %d, phase %d, %d us\n",
			"reserve", qh->udev->devnum,
			qh->hep->desc.bEndpointAddress, p,
			qh->period, qh->phase, load);
}

/*
 * Release a periodic QH's bandwidth reservation
 */
static void uhci_release_bandwidth(struct uhci_hcd *uhci, struct uhci_qh *qh)
{
	int i;
	int load = qh->load;
	char *p = "??";

	for (i = qh->phase; i < MAX_PHASE; i += qh->period) {
		uhci->load[i] -= load;
		uhci->total_load -= load;
	}
	uhci_to_hcd(uhci)->self.bandwidth_allocated =
			uhci->total_load / MAX_PHASE;
	switch (qh->type) {
	case USB_ENDPOINT_XFER_INT:
		--uhci_to_hcd(uhci)->self.bandwidth_int_reqs;
		p = "INT";
		break;
	case USB_ENDPOINT_XFER_ISOC:
		--uhci_to_hcd(uhci)->self.bandwidth_isoc_reqs;
		p = "ISO";
		break;
	}
	qh->bandwidth_reserved = 0;
	dev_dbg(uhci_dev(uhci),
			"%s dev %d ep%02x-%s, period %d, phase %d, %d us\n",
			"release", qh->udev->devnum,
			qh->hep->desc.bEndpointAddress, p,
			qh->period, qh->phase, load);
}

static inline struct urb_priv *uhci_alloc_urb_priv(struct uhci_hcd *uhci,
		struct urb *urb)
{
	struct urb_priv *urbp;

	urbp = kmem_cache_zalloc(uhci_up_cachep, GFP_ATOMIC);
	if (!urbp)
		return NULL;

	urbp->urb = urb;
	urb->hcpriv = urbp;

	INIT_LIST_HEAD(&urbp->node);
	INIT_LIST_HEAD(&urbp->td_list);

	return urbp;
}

static void uhci_free_urb_priv(struct uhci_hcd *uhci,
		struct urb_priv *urbp)
{
	struct uhci_td *td, *tmp;

	if (!list_empty(&urbp->node))
		dev_WARN(uhci_dev(uhci), "urb %p still on QH's list!\n",
				urbp->urb);

	list_for_each_entry_safe(td, tmp, &urbp->td_list, list) {
		uhci_remove_td_from_urbp(td);
		uhci_free_td(uhci, td);
	}

	kmem_cache_free(uhci_up_cachep, urbp);
}

/*
 * Map status to standard result codes
 *
 * <status> is (td_status(uhci, td) & 0xF60000), a.k.a.
 * uhci_status_bits(td_status(uhci, td)).
 * Note: <status> does not include the TD_CTRL_NAK bit.
 * <dir_out> is True for output TDs and False for input TDs.
 */
static int uhci_map_status(int status, int dir_out)
{
	if (!status)
		return 0;
	if (status & TD_CTRL_BITSTUFF)			/* Bitstuff error */
		return -EPROTO;
	if (status & TD_CTRL_CRCTIMEO) {		/* CRC/Timeout */
		if (dir_out)
			return -EPROTO;
		else
			return -EILSEQ;
	}
	if (status & TD_CTRL_BABBLE)			/* Babble */
		return -EOVERFLOW;
	if (status & TD_CTRL_DBUFERR)			/* Buffer error */
		return -ENOSR;
	if (status & TD_CTRL_STALLED)			/* Stalled */
		return -EPIPE;
	return 0;
}

/*
 * Control transfers
 */
static int uhci_submit_control(struct uhci_hcd *uhci, struct urb *urb,
		struct uhci_qh *qh)
{
	struct uhci_td *td;
	unsigned long destination, status;
	int maxsze = usb_endpoint_maxp(&qh->hep->desc);
	int len = urb->transfer_buffer_length;
	dma_addr_t data = urb->transfer_dma;
	__hc32 *plink;
	struct urb_priv *urbp = urb->hcpriv;
	int skel;

	/* The "pipe" thing contains the destination in bits 8--18 */
	destination = (urb->pipe & PIPE_DEVEP_MASK) | USB_PID_SETUP;

	/* 3 errors, dummy TD remains inactive */
	status = uhci_maxerr(3);
	if (urb->dev->speed == USB_SPEED_LOW)
		status |= TD_CTRL_LS;

	/*
	 * Build the TD for the control request setup packet
	 */
	td = qh->dummy_td;
	uhci_add_td_to_urbp(td, urbp);
	uhci_fill_td(uhci, td, status, destination | uhci_explen(8),
			urb->setup_dma);
	plink = &td->link;
	status |= TD_CTRL_ACTIVE;

	/*
	 * If direction is "send", change the packet ID from SETUP (0x2D)
	 * to OUT (0xE1).  Else change it from SETUP to IN (0x69) and
	 * set Short Packet Detect (SPD) for all data packets.
	 *
	 * 0-length transfers always get treated as "send".
	 */
	if (usb_pipeout(urb->pipe) || len == 0)
		destination ^= (USB_PID_SETUP ^ USB_PID_OUT);
	else {
		destination ^= (USB_PID_SETUP ^ USB_PID_IN);
		status |= TD_CTRL_SPD;
	}

	/*
	 * Build the DATA TDs
	 */
	while (len > 0) {
		int pktsze = maxsze;

		if (len <= pktsze) {		/* The last data packet */
			pktsze = len;
			status &= ~TD_CTRL_SPD;
		}

		td = uhci_alloc_td(uhci);
		if (!td)
			goto nomem;
		*plink = LINK_TO_TD(uhci, td);

		/* Alternate Data0/1 (start with Data1) */
		destination ^= TD_TOKEN_TOGGLE;

		uhci_add_td_to_urbp(td, urbp);
		uhci_fill_td(uhci, td, status,
			destination | uhci_explen(pktsze), data);
		plink = &td->link;

		data += pktsze;
		len -= pktsze;
	}

	/*
	 * Build the final TD for control status
	 */
	td = uhci_alloc_td(uhci);
	if (!td)
		goto nomem;
	*plink = LINK_TO_TD(uhci, td);

	/* Change direction for the status transaction */
	destination ^= (USB_PID_IN ^ USB_PID_OUT);
	destination |= TD_TOKEN_TOGGLE;		/* End in Data1 */

	uhci_add_td_to_urbp(td, urbp);
	uhci_fill_td(uhci, td, status | TD_CTRL_IOC,
			destination | uhci_explen(0), 0);
	plink = &td->link;

	/*
	 * Build the new dummy TD and activate the old one
	 */
	td = uhci_alloc_td(uhci);
	if (!td)
		goto nomem;
	*plink = LINK_TO_TD(uhci, td);

	uhci_fill_td(uhci, td, 0, USB_PID_OUT | uhci_explen(0), 0);
	wmb();
	qh->dummy_td->status |= cpu_to_hc32(uhci, TD_CTRL_ACTIVE);
	qh->dummy_td = td;

	/* Low-speed transfers get a different queue, and won't hog the bus.
	 * Also, some devices enumerate better without FSBR; the easiest way
	 * to do that is to put URBs on the low-speed queue while the device
	 * isn't in the CONFIGURED state. */
	if (urb->dev->speed == USB_SPEED_LOW ||
			urb->dev->state != USB_STATE_CONFIGURED)
		skel = SKEL_LS_CONTROL;
	else {
		skel = SKEL_FS_CONTROL;
		uhci_add_fsbr(uhci, urb);
	}
	if (qh->state != QH_STATE_ACTIVE)
		qh->skel = skel;
	return 0;

nomem:
	/* Remove the dummy TD from the td_list so it doesn't get freed */
	uhci_remove_td_from_urbp(qh->dummy_td);
	return -ENOMEM;
}

/*
 * Common submit for bulk and interrupt
 */
static int uhci_submit_common(struct uhci_hcd *uhci, struct urb *urb,
		struct uhci_qh *qh)
{
	struct uhci_td *td;
	unsigned long destination, status;
	int maxsze = usb_endpoint_maxp(&qh->hep->desc);
	int len = urb->transfer_buffer_length;
	int this_sg_len;
	dma_addr_t data;
	__hc32 *plink;
	struct urb_priv *urbp = urb->hcpriv;
	unsigned int toggle;
	struct scatterlist  *sg;
	int i;

	if (len < 0)
		return -EINVAL;

	/* The "pipe" thing contains the destination in bits 8--18 */
	destination = (urb->pipe & PIPE_DEVEP_MASK) | usb_packetid(urb->pipe);
	toggle = usb_gettoggle(urb->dev, usb_pipeendpoint(urb->pipe),
			 usb_pipeout(urb->pipe));

	/* 3 errors, dummy TD remains inactive */
	status = uhci_maxerr(3);
	if (urb->dev->speed == USB_SPEED_LOW)
		status |= TD_CTRL_LS;
	if (usb_pipein(urb->pipe))
		status |= TD_CTRL_SPD;

	i = urb->num_mapped_sgs;
	if (len > 0 && i > 0) {
		sg = urb->sg;
		data = sg_dma_address(sg);

		/* urb->transfer_buffer_length may be smaller than the
		 * size of the scatterlist (or vice versa)
		 */
		this_sg_len = min_t(int, sg_dma_len(sg), len);
	} else {
		sg = NULL;
		data = urb->transfer_dma;
		this_sg_len = len;
	}
	/*
	 * Build the DATA TDs
	 */
	plink = NULL;
	td = qh->dummy_td;
	for (;;) {	/* Allow zero length packets */
		int pktsze = maxsze;

		if (len <= pktsze) {		/* The last packet */
			pktsze = len;
			if (!(urb->transfer_flags & URB_SHORT_NOT_OK))
				status &= ~TD_CTRL_SPD;
		}

		if (plink) {
			td = uhci_alloc_td(uhci);
			if (!td)
				goto nomem;
			*plink = LINK_TO_TD(uhci, td);
		}
		uhci_add_td_to_urbp(td, urbp);
		uhci_fill_td(uhci, td, status,
				destination | uhci_explen(pktsze) |
					(toggle << TD_TOKEN_TOGGLE_SHIFT),
				data);
		plink = &td->link;
		status |= TD_CTRL_ACTIVE;

		toggle ^= 1;
		data += pktsze;
		this_sg_len -= pktsze;
		len -= maxsze;
		if (this_sg_len <= 0) {
			if (--i <= 0 || len <= 0)
				break;
			sg = sg_next(sg);
			data = sg_dma_address(sg);
			this_sg_len = min_t(int, sg_dma_len(sg), len);
		}
	}

	/*
	 * URB_ZERO_PACKET means adding a 0-length packet, if direction
	 * is OUT and the transfer_length was an exact multiple of maxsze,
	 * hence (len = transfer_length - N * maxsze) == 0
	 * however, if transfer_length == 0, the zero packet was already
	 * prepared above.
	 */
	if ((urb->transfer_flags & URB_ZERO_PACKET) &&
			usb_pipeout(urb->pipe) && len == 0 &&
			urb->transfer_buffer_length > 0) {
		td = uhci_alloc_td(uhci);
		if (!td)
			goto nomem;
		*plink = LINK_TO_TD(uhci, td);

		uhci_add_td_to_urbp(td, urbp);
		uhci_fill_td(uhci, td, status,
				destination | uhci_explen(0) |
					(toggle << TD_TOKEN_TOGGLE_SHIFT),
				data);
		plink = &td->link;

		toggle ^= 1;
	}

	/* Set the interrupt-on-completion flag on the last packet.
	 * A more-or-less typical 4 KB URB (= size of one memory page)
	 * will require about 3 ms to transfer; that's a little on the
	 * fast side but not enough to justify delaying an interrupt
	 * more than 2 or 3 URBs, so we will ignore the URB_NO_INTERRUPT
	 * flag setting. */
	td->status |= cpu_to_hc32(uhci, TD_CTRL_IOC);

	/*
	 * Build the new dummy TD and activate the old one
	 */
	td = uhci_alloc_td(uhci);
	if (!td)
		goto nomem;
	*plink = LINK_TO_TD(uhci, td);

	uhci_fill_td(uhci, td, 0, USB_PID_OUT | uhci_explen(0), 0);
	wmb();
	qh->dummy_td->status |= cpu_to_hc32(uhci, TD_CTRL_ACTIVE);
	qh->dummy_td = td;

	usb_settoggle(urb->dev, usb_pipeendpoint(urb->pipe),
			usb_pipeout(urb->pipe), toggle);
	return 0;

nomem:
	/* Remove the dummy TD from the td_list so it doesn't get freed */
	uhci_remove_td_from_urbp(qh->dummy_td);
	return -ENOMEM;
}

static int uhci_submit_bulk(struct uhci_hcd *uhci, struct urb *urb,
		struct uhci_qh *qh)
{
	int ret;

	/* Can't have low-speed bulk transfers */
	if (urb->dev->speed == USB_SPEED_LOW)
		return -EINVAL;

	if (qh->state != QH_STATE_ACTIVE)
		qh->skel = SKEL_BULK;
	ret = uhci_submit_common(uhci, urb, qh);
	if (ret == 0)
		uhci_add_fsbr(uhci, urb);
	return ret;
}

static int uhci_submit_interrupt(struct uhci_hcd *uhci, struct urb *urb,
		struct uhci_qh *qh)
{
	int ret;

	/* USB 1.1 interrupt transfers only involve one packet per interval.
	 * Drivers can submit URBs of any length, but longer ones will need
	 * multiple intervals to complete.
	 */

	if (!qh->bandwidth_reserved) {
		int exponent;

		/* Figure out which power-of-two queue to use */
		for (exponent = 7; exponent >= 0; --exponent) {
			if ((1 << exponent) <= urb->interval)
				break;
		}
		if (exponent < 0)
			return -EINVAL;

		/* If the slot is full, try a lower period */
		do {
			qh->period = 1 << exponent;
			qh->skel = SKEL_INDEX(exponent);

			/* For now, interrupt phase is fixed by the layout
			 * of the QH lists.
			 */
			qh->phase = (qh->period / 2) & (MAX_PHASE - 1);
			ret = uhci_check_bandwidth(uhci, qh);
		} while (ret != 0 && --exponent >= 0);
		if (ret)
			return ret;
	} else if (qh->period > urb->interval)
		return -EINVAL;		/* Can't decrease the period */

	ret = uhci_submit_common(uhci, urb, qh);
	if (ret == 0) {
		urb->interval = qh->period;
		if (!qh->bandwidth_reserved)
			uhci_reserve_bandwidth(uhci, qh);
	}
	return ret;
}

/*
 * Fix up the data structures following a short transfer
 */
static int uhci_fixup_short_transfer(struct uhci_hcd *uhci,
		struct uhci_qh *qh, struct urb_priv *urbp)
{
	struct uhci_td *td;
	struct list_head *tmp;
	int ret;

	td = list_entry(urbp->td_list.prev, struct uhci_td, list);
	if (qh->type == USB_ENDPOINT_XFER_CONTROL) {

		/* When a control transfer is short, we have to restart
		 * the queue at the status stage transaction, which is
		 * the last TD. */
		WARN_ON(list_empty(&urbp->td_list));
		qh->element = LINK_TO_TD(uhci, td);
		tmp = td->list.prev;
		ret = -EINPROGRESS;

	} else {

		/* When a bulk/interrupt transfer is short, we have to
		 * fix up the toggles of the following URBs on the queue
		 * before restarting the queue at the next URB. */
		qh->initial_toggle =
			uhci_toggle(td_token(uhci, qh->post_td)) ^ 1;
		uhci_fixup_toggles(uhci, qh, 1);

		if (list_empty(&urbp->td_list))
			td = qh->post_td;
		qh->element = td->link;
		tmp = urbp->td_list.prev;
		ret = 0;
	}

	/* Remove all the TDs we skipped over, from tmp back to the start */
	while (tmp != &urbp->td_list) {
		td = list_entry(tmp, struct uhci_td, list);
		tmp = tmp->prev;

		uhci_remove_td_from_urbp(td);
		uhci_free_td(uhci, td);
	}
	return ret;
}

/*
 * Common result for control, bulk, and interrupt
 */
static int uhci_result_common(struct uhci_hcd *uhci, struct urb *urb)
{
	struct urb_priv *urbp = urb->hcpriv;
	struct uhci_qh *qh = urbp->qh;
	struct uhci_td *td, *tmp;
	unsigned status;
	int ret = 0;

	list_for_each_entry_safe(td, tmp, &urbp->td_list, list) {
		unsigned int ctrlstat;
		int len;

		ctrlstat = td_status(uhci, td);
		status = uhci_status_bits(ctrlstat);
		if (status & TD_CTRL_ACTIVE)
			return -EINPROGRESS;

		len = uhci_actual_length(ctrlstat);
		urb->actual_length += len;

		if (status) {
			ret = uhci_map_status(status,
					uhci_packetout(td_token(uhci, td)));
			if ((debug == 1 && ret != -EPIPE) || debug > 1) {
				/* Some debugging code */
				dev_dbg(&urb->dev->dev,
						"%s: failed with status %x\n",
						__func__, status);

				if (debug > 1 && errbuf) {
					/* Print the chain for debugging */
					uhci_show_qh(uhci, urbp->qh, errbuf,
						ERRBUF_LEN - EXTRA_SPACE, 0);
					lprintk(errbuf);
				}
			}

		/* Did we receive a short packet? */
		} else if (len < uhci_expected_length(td_token(uhci, td))) {

			/* For control transfers, go to the status TD if
			 * this isn't already the last data TD */
			if (qh->type == USB_ENDPOINT_XFER_CONTROL) {
				if (td->list.next != urbp->td_list.prev)
					ret = 1;
			}

			/* For bulk and interrupt, this may be an error */
			else if (urb->transfer_flags & URB_SHORT_NOT_OK)
				ret = -EREMOTEIO;

			/* Fixup needed only if this isn't the URB's last TD */
			else if (&td->list != urbp->td_list.prev)
				ret = 1;
		}

		uhci_remove_td_from_urbp(td);
		if (qh->post_td)
			uhci_free_td(uhci, qh->post_td);
		qh->post_td = td;

		if (ret != 0)
			goto err;
	}
	return ret;

err:
	if (ret < 0) {
		/* Note that the queue has stopped and save
		 * the next toggle value */
		qh->element = UHCI_PTR_TERM(uhci);
		qh->is_stopped = 1;
		qh->needs_fixup = (qh->type != USB_ENDPOINT_XFER_CONTROL);
		qh->initial_toggle = uhci_toggle(td_token(uhci, td)) ^
				(ret == -EREMOTEIO);

	} else		/* Short packet received */
		ret = uhci_fixup_short_transfer(uhci, qh, urbp);
	return ret;
}

/*
 * Isochronous transfers
 */
static int uhci_submit_isochronous(struct uhci_hcd *uhci, struct urb *urb,
		struct uhci_qh *qh)
{
	struct uhci_td *td = NULL;	/* Since urb->number_of_packets > 0 */
	int i;
	unsigned frame, next;
	unsigned long destination, status;
	struct urb_priv *urbp = (struct urb_priv *) urb->hcpriv;

	/* Values must not be too big (could overflow below) */
	if (urb->interval >= UHCI_NUMFRAMES ||
			urb->number_of_packets >= UHCI_NUMFRAMES)
		return -EFBIG;

	uhci_get_current_frame_number(uhci);

	/* Check the period and figure out the starting frame number */
	if (!qh->bandwidth_reserved) {
		qh->period = urb->interval;
		qh->phase = -1;		/* Find the best phase */
		i = uhci_check_bandwidth(uhci, qh);
		if (i)
			return i;

		/* Allow a little time to allocate the TDs */
		next = uhci->frame_number + 10;
		frame = qh->phase;

		/* Round up to the first available slot */
		frame += (next - frame + qh->period - 1) & -qh->period;

	} else if (qh->period != urb->interval) {
		return -EINVAL;		/* Can't change the period */

	} else {
		next = uhci->frame_number + 1;

		/* Find the next unused frame */
		if (list_empty(&qh->queue)) {
			frame = qh->iso_frame;
		} else {
			struct urb *lurb;

			lurb = list_entry(qh->queue.prev,
					struct urb_priv, node)->urb;
			frame = lurb->start_frame +
					lurb->number_of_packets *
					lurb->interval;
		}

		/* Fell behind? */
		if (!uhci_frame_before_eq(next, frame)) {

			/* USB_ISO_ASAP: Round up to the first available slot */
			if (urb->transfer_flags & URB_ISO_ASAP)
				frame += (next - frame + qh->period - 1) &
						-qh->period;

			/*
			 * Not ASAP: Use the next slot in the stream,
			 * no matter what.
			 */
			else if (!uhci_frame_before_eq(next,
					frame + (urb->number_of_packets - 1) *
						qh->period))
				dev_dbg(uhci_dev(uhci), "iso underrun %p (%u+%u < %u)\n",
						urb, frame,
						(urb->number_of_packets - 1) *
							qh->period,
						next);
		}
	}

	/* Make sure we won't have to go too far into the future */
	if (uhci_frame_before_eq(uhci->last_iso_frame + UHCI_NUMFRAMES,
			frame + urb->number_of_packets * urb->interval))
		return -EFBIG;
	urb->start_frame = frame;

	status = TD_CTRL_ACTIVE | TD_CTRL_IOS;
	destination = (urb->pipe & PIPE_DEVEP_MASK) | usb_packetid(urb->pipe);

	for (i = 0; i < urb->number_of_packets; i++) {
		td = uhci_alloc_td(uhci);
		if (!td)
			return -ENOMEM;

		uhci_add_td_to_urbp(td, urbp);
		uhci_fill_td(uhci, td, status, destination |
				uhci_explen(urb->iso_frame_desc[i].length),
				urb->transfer_dma +
					urb->iso_frame_desc[i].offset);
	}

	/* Set the interrupt-on-completion flag on the last packet. */
	td->status |= cpu_to_hc32(uhci, TD_CTRL_IOC);

	/* Add the TDs to the frame list */
	frame = urb->start_frame;
	list_for_each_entry(td, &urbp->td_list, list) {
		uhci_insert_td_in_frame_list(uhci, td, frame);
		frame += qh->period;
	}

	if (list_empty(&qh->queue)) {
		qh->iso_packet_desc = &urb->iso_frame_desc[0];
		qh->iso_frame = urb->start_frame;
	}

	qh->skel = SKEL_ISO;
	if (!qh->bandwidth_reserved)
		uhci_reserve_bandwidth(uhci, qh);
	return 0;
}

static int uhci_result_isochronous(struct uhci_hcd *uhci, struct urb *urb)
{
	struct uhci_td *td, *tmp;
	struct urb_priv *urbp = urb->hcpriv;
	struct uhci_qh *qh = urbp->qh;

	list_for_each_entry_safe(td, tmp, &urbp->td_list, list) {
		unsigned int ctrlstat;
		int status;
		int actlength;

		if (uhci_frame_before_eq(uhci->cur_iso_frame, qh->iso_frame))
			return -EINPROGRESS;

		uhci_remove_tds_from_frame(uhci, qh->iso_frame);

		ctrlstat = td_status(uhci, td);
		if (ctrlstat & TD_CTRL_ACTIVE) {
			status = -EXDEV;	/* TD was added too late? */
		} else {
			status = uhci_map_status(uhci_status_bits(ctrlstat),
					usb_pipeout(urb->pipe));
			actlength = uhci_actual_length(ctrlstat);

			urb->actual_length += actlength;
			qh->iso_packet_desc->actual_length = actlength;
			qh->iso_packet_desc->status = status;
		}
		if (status)
			urb->error_count++;

		uhci_remove_td_from_urbp(td);
		uhci_free_td(uhci, td);
		qh->iso_frame += qh->period;
		++qh->iso_packet_desc;
	}
	return 0;
}

static int uhci_urb_enqueue(struct usb_hcd *hcd,
		struct urb *urb, gfp_t mem_flags)
{
	int ret;
	struct uhci_hcd *uhci = hcd_to_uhci(hcd);
	unsigned long flags;
	struct urb_priv *urbp;
	struct uhci_qh *qh;

	spin_lock_irqsave(&uhci->lock, flags);

	ret = usb_hcd_link_urb_to_ep(hcd, urb);
	if (ret)
		goto done_not_linked;

	ret = -ENOMEM;
	urbp = uhci_alloc_urb_priv(uhci, urb);
	if (!urbp)
		goto done;

	if (urb->ep->hcpriv)
		qh = urb->ep->hcpriv;
	else {
		qh = uhci_alloc_qh(uhci, urb->dev, urb->ep);
		if (!qh)
			goto err_no_qh;
	}
	urbp->qh = qh;

	switch (qh->type) {
	case USB_ENDPOINT_XFER_CONTROL:
		ret = uhci_submit_control(uhci, urb, qh);
		break;
	case USB_ENDPOINT_XFER_BULK:
		ret = uhci_submit_bulk(uhci, urb, qh);
		break;
	case USB_ENDPOINT_XFER_INT:
		ret = uhci_submit_interrupt(uhci, urb, qh);
		break;
	case USB_ENDPOINT_XFER_ISOC:
		urb->error_count = 0;
		ret = uhci_submit_isochronous(uhci, urb, qh);
		break;
	}
	if (ret != 0)
		goto err_submit_failed;

	/* Add this URB to the QH */
	list_add_tail(&urbp->node, &qh->queue);

	/* If the new URB is the first and only one on this QH then either
	 * the QH is new and idle or else it's unlinked and waiting to
	 * become idle, so we can activate it right away.  But only if the
	 * queue isn't stopped. */
	if (qh->queue.next == &urbp->node && !qh->is_stopped) {
		uhci_activate_qh(uhci, qh);
		uhci_urbp_wants_fsbr(uhci, urbp);
	}
	goto done;

err_submit_failed:
	if (qh->state == QH_STATE_IDLE)
		uhci_make_qh_idle(uhci, qh);	/* Reclaim unused QH */
err_no_qh:
	uhci_free_urb_priv(uhci, urbp);
done:
	if (ret)
		usb_hcd_unlink_urb_from_ep(hcd, urb);
done_not_linked:
	spin_unlock_irqrestore(&uhci->lock, flags);
	return ret;
}

static int uhci_urb_dequeue(struct usb_hcd *hcd, struct urb *urb, int status)
{
	struct uhci_hcd *uhci = hcd_to_uhci(hcd);
	unsigned long flags;
	struct uhci_qh *qh;
	int rc;

	spin_lock_irqsave(&uhci->lock, flags);
	rc = usb_hcd_check_unlink_urb(hcd, urb, status);
	if (rc)
		goto done;

	qh = ((struct urb_priv *) urb->hcpriv)->qh;

	/* Remove Isochronous TDs from the frame list ASAP */
	if (qh->type == USB_ENDPOINT_XFER_ISOC) {
		uhci_unlink_isochronous_tds(uhci, urb);
		mb();

		/* If the URB has already started, update the QH unlink time */
		uhci_get_current_frame_number(uhci);
		if (uhci_frame_before_eq(urb->start_frame, uhci->frame_number))
			qh->unlink_frame = uhci->frame_number;
	}

	uhci_unlink_qh(uhci, qh);

done:
	spin_unlock_irqrestore(&uhci->lock, flags);
	return rc;
}

/*
 * Finish unlinking an URB and give it back
 */
static void uhci_giveback_urb(struct uhci_hcd *uhci, struct uhci_qh *qh,
		struct urb *urb, int status)
__releases(uhci->lock)
__acquires(uhci->lock)
{
	struct urb_priv *urbp = (struct urb_priv *) urb->hcpriv;

	if (qh->type == USB_ENDPOINT_XFER_CONTROL) {

		/* Subtract off the length of the SETUP packet from
		 * urb->actual_length.
		 */
		urb->actual_length -= min_t(u32, 8, urb->actual_length);
	}

	/* When giving back the first URB in an Isochronous queue,
	 * reinitialize the QH's iso-related members for the next URB. */
	else if (qh->type == USB_ENDPOINT_XFER_ISOC &&
			urbp->node.prev == &qh->queue &&
			urbp->node.next != &qh->queue) {
		struct urb *nurb = list_entry(urbp->node.next,
				struct urb_priv, node)->urb;

		qh->iso_packet_desc = &nurb->iso_frame_desc[0];
		qh->iso_frame = nurb->start_frame;
	}

	/* Take the URB off the QH's queue.  If the queue is now empty,
	 * this is a perfect time for a toggle fixup. */
	list_del_init(&urbp->node);
	if (list_empty(&qh->queue) && qh->needs_fixup) {
		usb_settoggle(urb->dev, usb_pipeendpoint(urb->pipe),
				usb_pipeout(urb->pipe), qh->initial_toggle);
		qh->needs_fixup = 0;
	}

	uhci_free_urb_priv(uhci, urbp);
	usb_hcd_unlink_urb_from_ep(uhci_to_hcd(uhci), urb);

	spin_unlock(&uhci->lock);
	usb_hcd_giveback_urb(uhci_to_hcd(uhci), urb, status);
	spin_lock(&uhci->lock);

	/* If the queue is now empty, we can unlink the QH and give up its
	 * reserved bandwidth. */
	if (list_empty(&qh->queue)) {
		uhci_unlink_qh(uhci, qh);
		if (qh->bandwidth_reserved)
			uhci_release_bandwidth(uhci, qh);
	}
}

/*
 * Scan the URBs in a QH's queue
 */
#define QH_FINISHED_UNLINKING(qh)			\
		(qh->state == QH_STATE_UNLINKING &&	\
		uhci->frame_number + uhci->is_stopped != qh->unlink_frame)

static void uhci_scan_qh(struct uhci_hcd *uhci, struct uhci_qh *qh)
{
	struct urb_priv *urbp;
	struct urb *urb;
	int status;

	while (!list_empty(&qh->queue)) {
		urbp = list_entry(qh->queue.next, struct urb_priv, node);
		urb = urbp->urb;

		if (qh->type == USB_ENDPOINT_XFER_ISOC)
			status = uhci_result_isochronous(uhci, urb);
		else
			status = uhci_result_common(uhci, urb);
		if (status == -EINPROGRESS)
			break;

		/* Dequeued but completed URBs can't be given back unless
		 * the QH is stopped or has finished unlinking. */
		if (urb->unlinked) {
			if (QH_FINISHED_UNLINKING(qh))
				qh->is_stopped = 1;
			else if (!qh->is_stopped)
				return;
		}

		uhci_giveback_urb(uhci, qh, urb, status);
		if (status < 0)
			break;
	}

	/* If the QH is neither stopped nor finished unlinking (normal case),
	 * our work here is done. */
	if (QH_FINISHED_UNLINKING(qh))
		qh->is_stopped = 1;
	else if (!qh->is_stopped)
		return;

	/* Otherwise give back each of the dequeued URBs */
restart:
	list_for_each_entry(urbp, &qh->queue, node) {
		urb = urbp->urb;
		if (urb->unlinked) {

			/* Fix up the TD links and save the toggles for
			 * non-Isochronous queues.  For Isochronous queues,
			 * test for too-recent dequeues. */
			if (!uhci_cleanup_queue(uhci, qh, urb)) {
				qh->is_stopped = 0;
				return;
			}
			uhci_giveback_urb(uhci, qh, urb, 0);
			goto restart;
		}
	}
	qh->is_stopped = 0;

	/* There are no more dequeued URBs.  If there are still URBs on the
	 * queue, the QH can now be re-activated. */
	if (!list_empty(&qh->queue)) {
		if (qh->needs_fixup)
			uhci_fixup_toggles(uhci, qh, 0);

		/* If the first URB on the queue wants FSBR but its time
		 * limit has expired, set the next TD to interrupt on
		 * completion before reactivating the QH. */
		urbp = list_entry(qh->queue.next, struct urb_priv, node);
		if (urbp->fsbr && qh->wait_expired) {
			struct uhci_td *td = list_entry(urbp->td_list.next,
					struct uhci_td, list);

			td->status |= cpu_to_hc32(uhci, TD_CTRL_IOC);
		}

		uhci_activate_qh(uhci, qh);
	}

	/* The queue is empty.  The QH can become idle if it is fully
	 * unlinked. */
	else if (QH_FINISHED_UNLINKING(qh))
		uhci_make_qh_idle(uhci, qh);
}

/*
 * Check for queues that have made some forward progress.
 * Returns 0 if the queue is not Isochronous, is ACTIVE, and
 * has not advanced since last examined; 1 otherwise.
 *
 * Early Intel controllers have a bug which causes qh->element sometimes
 * not to advance when a TD completes successfully.  The queue remains
 * stuck on the inactive completed TD.  We detect such cases and advance
 * the element pointer by hand.
 */
static int uhci_advance_check(struct uhci_hcd *uhci, struct uhci_qh *qh)
{
	struct urb_priv *urbp = NULL;
	struct uhci_td *td;
	int ret = 1;
	unsigned status;

	if (qh->type == USB_ENDPOINT_XFER_ISOC)
		goto done;

	/* Treat an UNLINKING queue as though it hasn't advanced.
	 * This is okay because reactivation will treat it as though
	 * it has advanced, and if it is going to become IDLE then
	 * this doesn't matter anyway.  Furthermore it's possible
	 * for an UNLINKING queue not to have any URBs at all, or
	 * for its first URB not to have any TDs (if it was dequeued
	 * just as it completed).  So it's not easy in any case to
	 * test whether such queues have advanced. */
	if (qh->state != QH_STATE_ACTIVE) {
		urbp = NULL;
		status = 0;

	} else {
		urbp = list_entry(qh->queue.next, struct urb_priv, node);
		td = list_entry(urbp->td_list.next, struct uhci_td, list);
		status = td_status(uhci, td);
		if (!(status & TD_CTRL_ACTIVE)) {

			/* We're okay, the queue has advanced */
			qh->wait_expired = 0;
			qh->advance_jiffies = jiffies;
			goto done;
		}
		ret = uhci->is_stopped;
	}

	/* The queue hasn't advanced; check for timeout */
	if (qh->wait_expired)
		goto done;

	if (time_after(jiffies, qh->advance_jiffies + QH_WAIT_TIMEOUT)) {

		/* Detect the Intel bug and work around it */
		if (qh->post_td && qh_element(qh) ==
			LINK_TO_TD(uhci, qh->post_td)) {
			qh->element = qh->post_td->link;
			qh->advance_jiffies = jiffies;
			ret = 1;
			goto done;
		}

		qh->wait_expired = 1;

		/* If the current URB wants FSBR, unlink it temporarily
		 * so that we can safely set the next TD to interrupt on
		 * completion.  That way we'll know as soon as the queue
		 * starts moving again. */
		if (urbp && urbp->fsbr && !(status & TD_CTRL_IOC))
			uhci_unlink_qh(uhci, qh);

	} else {
		/* Unmoving but not-yet-expired queues keep FSBR alive */
		if (urbp)
			uhci_urbp_wants_fsbr(uhci, urbp);
	}

done:
	return ret;
}

/*
 * Process events in the schedule, but only in one thread at a time
 */
static void uhci_scan_schedule(struct uhci_hcd *uhci)
{
	int i;
	struct uhci_qh *qh;

	/* Don't allow re-entrant calls */
	if (uhci->scan_in_progress) {
		uhci->need_rescan = 1;
		return;
	}
	uhci->scan_in_progress = 1;
rescan:
	uhci->need_rescan = 0;
	uhci->fsbr_is_wanted = 0;

	uhci_clear_next_interrupt(uhci);
	uhci_get_current_frame_number(uhci);
	uhci->cur_iso_frame = uhci->frame_number;

	/* Go through all the QH queues and process the URBs in each one */
	for (i = 0; i < UHCI_NUM_SKELQH - 1; ++i) {
		uhci->next_qh = list_entry(uhci->skelqh[i]->node.next,
				struct uhci_qh, node);
		while ((qh = uhci->next_qh) != uhci->skelqh[i]) {
			uhci->next_qh = list_entry(qh->node.next,
					struct uhci_qh, node);

			if (uhci_advance_check(uhci, qh)) {
				uhci_scan_qh(uhci, qh);
				if (qh->state == QH_STATE_ACTIVE) {
					uhci_urbp_wants_fsbr(uhci,
	list_entry(qh->queue.next, struct urb_priv, node));
				}
			}
		}
	}

	uhci->last_iso_frame = uhci->cur_iso_frame;
	if (uhci->need_rescan)
		goto rescan;
	uhci->scan_in_progress = 0;

	if (uhci->fsbr_is_on && !uhci->fsbr_is_wanted &&
			!uhci->fsbr_expiring) {
		uhci->fsbr_expiring = 1;
		mod_timer(&uhci->fsbr_timer, jiffies + FSBR_OFF_DELAY);
	}

	if (list_empty(&uhci->skel_unlink_qh->node))
		uhci_clear_next_interrupt(uhci);
	else
		uhci_set_next_interrupt(uhci);
}
