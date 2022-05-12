// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *	LAPB release 002
 *
 *	This code REQUIRES 2.1.15 or higher/ NET3.038
 *
 *	History
 *	LAPB 001	Jonathan Naulor	Started Coding
 *	LAPB 002	Jonathan Naylor	New timer architecture.
 *	2000-10-29	Henner Eisen	lapb_data_indication() return status.
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/errno.h>
#include <linux/types.h>
#include <linux/socket.h>
#include <linux/in.h>
#include <linux/kernel.h>
#include <linux/timer.h>
#include <linux/string.h>
#include <linux/sockios.h>
#include <linux/net.h>
#include <linux/inet.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/slab.h>
#include <net/sock.h>
#include <linux/uaccess.h>
#include <linux/fcntl.h>
#include <linux/mm.h>
#include <linux/interrupt.h>
#include <net/lapb.h>

/*
 *	State machine for state 0, Disconnected State.
 *	The handling of the timer(s) is in file lapb_timer.c.
 */
static void lapb_state0_machine(struct lapb_cb *lapb, struct sk_buff *skb,
				struct lapb_frame *frame)
{
	switch (frame->type) {
	case LAPB_SABM:
		lapb_dbg(1, "(%p) S0 RX SABM(%d)\n", lapb->dev, frame->pf);
		if (lapb->mode & LAPB_EXTENDED) {
			lapb_dbg(1, "(%p) S0 TX DM(%d)\n",
				 lapb->dev, frame->pf);
			lapb_send_control(lapb, LAPB_DM, frame->pf,
					  LAPB_RESPONSE);
		} else {
			lapb_dbg(1, "(%p) S0 TX UA(%d)\n",
				 lapb->dev, frame->pf);
			lapb_dbg(0, "(%p) S0 -> S3\n", lapb->dev);
			lapb_send_control(lapb, LAPB_UA, frame->pf,
					  LAPB_RESPONSE);
			lapb_stop_t1timer(lapb);
			lapb_stop_t2timer(lapb);
			lapb->state     = LAPB_STATE_3;
			lapb->condition = 0x00;
			lapb->n2count   = 0;
			lapb->vs        = 0;
			lapb->vr        = 0;
			lapb->va        = 0;
			lapb_connect_indication(lapb, LAPB_OK);
		}
		break;

	case LAPB_SABME:
		lapb_dbg(1, "(%p) S0 RX SABME(%d)\n", lapb->dev, frame->pf);
		if (lapb->mode & LAPB_EXTENDED) {
			lapb_dbg(1, "(%p) S0 TX UA(%d)\n",
				 lapb->dev, frame->pf);
			lapb_dbg(0, "(%p) S0 -> S3\n", lapb->dev);
			lapb_send_control(lapb, LAPB_UA, frame->pf,
					  LAPB_RESPONSE);
			lapb_stop_t1timer(lapb);
			lapb_stop_t2timer(lapb);
			lapb->state     = LAPB_STATE_3;
			lapb->condition = 0x00;
			lapb->n2count   = 0;
			lapb->vs        = 0;
			lapb->vr        = 0;
			lapb->va        = 0;
			lapb_connect_indication(lapb, LAPB_OK);
		} else {
			lapb_dbg(1, "(%p) S0 TX DM(%d)\n",
				 lapb->dev, frame->pf);
			lapb_send_control(lapb, LAPB_DM, frame->pf,
					  LAPB_RESPONSE);
		}
		break;

	case LAPB_DISC:
		lapb_dbg(1, "(%p) S0 RX DISC(%d)\n", lapb->dev, frame->pf);
		lapb_dbg(1, "(%p) S0 TX UA(%d)\n", lapb->dev, frame->pf);
		lapb_send_control(lapb, LAPB_UA, frame->pf, LAPB_RESPONSE);
		break;

	default:
		break;
	}

	kfree_skb(skb);
}

/*
 *	State machine for state 1, Awaiting Connection State.
 *	The handling of the timer(s) is in file lapb_timer.c.
 */
static void lapb_state1_machine(struct lapb_cb *lapb, struct sk_buff *skb,
				struct lapb_frame *frame)
{
	switch (frame->type) {
	case LAPB_SABM:
		lapb_dbg(1, "(%p) S1 RX SABM(%d)\n", lapb->dev, frame->pf);
		if (lapb->mode & LAPB_EXTENDED) {
			lapb_dbg(1, "(%p) S1 TX DM(%d)\n",
				 lapb->dev, frame->pf);
			lapb_send_control(lapb, LAPB_DM, frame->pf,
					  LAPB_RESPONSE);
		} else {
			lapb_dbg(1, "(%p) S1 TX UA(%d)\n",
				 lapb->dev, frame->pf);
			lapb_send_control(lapb, LAPB_UA, frame->pf,
					  LAPB_RESPONSE);
		}
		break;

	case LAPB_SABME:
		lapb_dbg(1, "(%p) S1 RX SABME(%d)\n", lapb->dev, frame->pf);
		if (lapb->mode & LAPB_EXTENDED) {
			lapb_dbg(1, "(%p) S1 TX UA(%d)\n",
				 lapb->dev, frame->pf);
			lapb_send_control(lapb, LAPB_UA, frame->pf,
					  LAPB_RESPONSE);
		} else {
			lapb_dbg(1, "(%p) S1 TX DM(%d)\n",
				 lapb->dev, frame->pf);
			lapb_send_control(lapb, LAPB_DM, frame->pf,
					  LAPB_RESPONSE);
		}
		break;

	case LAPB_DISC:
		lapb_dbg(1, "(%p) S1 RX DISC(%d)\n", lapb->dev, frame->pf);
		lapb_dbg(1, "(%p) S1 TX DM(%d)\n", lapb->dev, frame->pf);
		lapb_send_control(lapb, LAPB_DM, frame->pf, LAPB_RESPONSE);
		break;

	case LAPB_UA:
		lapb_dbg(1, "(%p) S1 RX UA(%d)\n", lapb->dev, frame->pf);
		if (frame->pf) {
			lapb_dbg(0, "(%p) S1 -> S3\n", lapb->dev);
			lapb_stop_t1timer(lapb);
			lapb_stop_t2timer(lapb);
			lapb->state     = LAPB_STATE_3;
			lapb->condition = 0x00;
			lapb->n2count   = 0;
			lapb->vs        = 0;
			lapb->vr        = 0;
			lapb->va        = 0;
			lapb_connect_confirmation(lapb, LAPB_OK);
		}
		break;

	case LAPB_DM:
		lapb_dbg(1, "(%p) S1 RX DM(%d)\n", lapb->dev, frame->pf);
		if (frame->pf) {
			lapb_dbg(0, "(%p) S1 -> S0\n", lapb->dev);
			lapb_clear_queues(lapb);
			lapb->state = LAPB_STATE_0;
			lapb_start_t1timer(lapb);
			lapb_stop_t2timer(lapb);
			lapb_disconnect_indication(lapb, LAPB_REFUSED);
		}
		break;
	}

	kfree_skb(skb);
}

/*
 *	State machine for state 2, Awaiting Release State.
 *	The handling of the timer(s) is in file lapb_timer.c
 */
static void lapb_state2_machine(struct lapb_cb *lapb, struct sk_buff *skb,
				struct lapb_frame *frame)
{
	switch (frame->type) {
	case LAPB_SABM:
	case LAPB_SABME:
		lapb_dbg(1, "(%p) S2 RX {SABM,SABME}(%d)\n",
			 lapb->dev, frame->pf);
		lapb_dbg(1, "(%p) S2 TX DM(%d)\n", lapb->dev, frame->pf);
		lapb_send_control(lapb, LAPB_DM, frame->pf, LAPB_RESPONSE);
		break;

	case LAPB_DISC:
		lapb_dbg(1, "(%p) S2 RX DISC(%d)\n", lapb->dev, frame->pf);
		lapb_dbg(1, "(%p) S2 TX UA(%d)\n", lapb->dev, frame->pf);
		lapb_send_control(lapb, LAPB_UA, frame->pf, LAPB_RESPONSE);
		break;

	case LAPB_UA:
		lapb_dbg(1, "(%p) S2 RX UA(%d)\n", lapb->dev, frame->pf);
		if (frame->pf) {
			lapb_dbg(0, "(%p) S2 -> S0\n", lapb->dev);
			lapb->state = LAPB_STATE_0;
			lapb_start_t1timer(lapb);
			lapb_stop_t2timer(lapb);
			lapb_disconnect_confirmation(lapb, LAPB_OK);
		}
		break;

	case LAPB_DM:
		lapb_dbg(1, "(%p) S2 RX DM(%d)\n", lapb->dev, frame->pf);
		if (frame->pf) {
			lapb_dbg(0, "(%p) S2 -> S0\n", lapb->dev);
			lapb->state = LAPB_STATE_0;
			lapb_start_t1timer(lapb);
			lapb_stop_t2timer(lapb);
			lapb_disconnect_confirmation(lapb, LAPB_NOTCONNECTED);
		}
		break;

	case LAPB_I:
	case LAPB_REJ:
	case LAPB_RNR:
	case LAPB_RR:
		lapb_dbg(1, "(%p) S2 RX {I,REJ,RNR,RR}(%d)\n",
		       lapb->dev, frame->pf);
		lapb_dbg(1, "(%p) S2 RX DM(%d)\n", lapb->dev, frame->pf);
		if (frame->pf)
			lapb_send_control(lapb, LAPB_DM, frame->pf,
					  LAPB_RESPONSE);
		break;
	}

	kfree_skb(skb);
}

/*
 *	State machine for state 3, Connected State.
 *	The handling of the timer(s) is in file lapb_timer.c
 */
static void lapb_state3_machine(struct lapb_cb *lapb, struct sk_buff *skb,
				struct lapb_frame *frame)
{
	int queued = 0;
	int modulus = (lapb->mode & LAPB_EXTENDED) ? LAPB_EMODULUS :
						     LAPB_SMODULUS;

	switch (frame->type) {
	case LAPB_SABM:
		lapb_dbg(1, "(%p) S3 RX SABM(%d)\n", lapb->dev, frame->pf);
		if (lapb->mode & LAPB_EXTENDED) {
			lapb_dbg(1, "(%p) S3 TX DM(%d)\n",
				 lapb->dev, frame->pf);
			lapb_send_control(lapb, LAPB_DM, frame->pf,
					  LAPB_RESPONSE);
		} else {
			lapb_dbg(1, "(%p) S3 TX UA(%d)\n",
				 lapb->dev, frame->pf);
			lapb_send_control(lapb, LAPB_UA, frame->pf,
					  LAPB_RESPONSE);
			lapb_stop_t1timer(lapb);
			lapb_stop_t2timer(lapb);
			lapb->condition = 0x00;
			lapb->n2count   = 0;
			lapb->vs        = 0;
			lapb->vr        = 0;
			lapb->va        = 0;
			lapb_requeue_frames(lapb);
		}
		break;

	case LAPB_SABME:
		lapb_dbg(1, "(%p) S3 RX SABME(%d)\n", lapb->dev, frame->pf);
		if (lapb->mode & LAPB_EXTENDED) {
			lapb_dbg(1, "(%p) S3 TX UA(%d)\n",
				 lapb->dev, frame->pf);
			lapb_send_control(lapb, LAPB_UA, frame->pf,
					  LAPB_RESPONSE);
			lapb_stop_t1timer(lapb);
			lapb_stop_t2timer(lapb);
			lapb->condition = 0x00;
			lapb->n2count   = 0;
			lapb->vs        = 0;
			lapb->vr        = 0;
			lapb->va        = 0;
			lapb_requeue_frames(lapb);
		} else {
			lapb_dbg(1, "(%p) S3 TX DM(%d)\n",
				 lapb->dev, frame->pf);
			lapb_send_control(lapb, LAPB_DM, frame->pf,
					  LAPB_RESPONSE);
		}
		break;

	case LAPB_DISC:
		lapb_dbg(1, "(%p) S3 RX DISC(%d)\n", lapb->dev, frame->pf);
		lapb_dbg(0, "(%p) S3 -> S0\n", lapb->dev);
		lapb_clear_queues(lapb);
		lapb_send_control(lapb, LAPB_UA, frame->pf, LAPB_RESPONSE);
		lapb_start_t1timer(lapb);
		lapb_stop_t2timer(lapb);
		lapb->state = LAPB_STATE_0;
		lapb_disconnect_indication(lapb, LAPB_OK);
		break;

	case LAPB_DM:
		lapb_dbg(1, "(%p) S3 RX DM(%d)\n", lapb->dev, frame->pf);
		lapb_dbg(0, "(%p) S3 -> S0\n", lapb->dev);
		lapb_clear_queues(lapb);
		lapb->state = LAPB_STATE_0;
		lapb_start_t1timer(lapb);
		lapb_stop_t2timer(lapb);
		lapb_disconnect_indication(lapb, LAPB_NOTCONNECTED);
		break;

	case LAPB_RNR:
		lapb_dbg(1, "(%p) S3 RX RNR(%d) R%d\n",
			 lapb->dev, frame->pf, frame->nr);
		lapb->condition |= LAPB_PEER_RX_BUSY_CONDITION;
		lapb_check_need_response(lapb, frame->cr, frame->pf);
		if (lapb_validate_nr(lapb, frame->nr)) {
			lapb_check_iframes_acked(lapb, frame->nr);
		} else {
			lapb->frmr_data = *frame;
			lapb->frmr_type = LAPB_FRMR_Z;
			lapb_transmit_frmr(lapb);
			lapb_dbg(0, "(%p) S3 -> S4\n", lapb->dev);
			lapb_start_t1timer(lapb);
			lapb_stop_t2timer(lapb);
			lapb->state   = LAPB_STATE_4;
			lapb->n2count = 0;
		}
		break;

	case LAPB_RR:
		lapb_dbg(1, "(%p) S3 RX RR(%d) R%d\n",
			 lapb->dev, frame->pf, frame->nr);
		lapb->condition &= ~LAPB_PEER_RX_BUSY_CONDITION;
		lapb_check_need_response(lapb, frame->cr, frame->pf);
		if (lapb_validate_nr(lapb, frame->nr)) {
			lapb_check_iframes_acked(lapb, frame->nr);
		} else {
			lapb->frmr_data = *frame;
			lapb->frmr_type = LAPB_FRMR_Z;
			lapb_transmit_frmr(lapb);
			lapb_dbg(0, "(%p) S3 -> S4\n", lapb->dev);
			lapb_start_t1timer(lapb);
			lapb_stop_t2timer(lapb);
			lapb->state   = LAPB_STATE_4;
			lapb->n2count = 0;
		}
		break;

	case LAPB_REJ:
		lapb_dbg(1, "(%p) S3 RX REJ(%d) R%d\n",
			 lapb->dev, frame->pf, frame->nr);
		lapb->condition &= ~LAPB_PEER_RX_BUSY_CONDITION;
		lapb_check_need_response(lapb, frame->cr, frame->pf);
		if (lapb_validate_nr(lapb, frame->nr)) {
			lapb_frames_acked(lapb, frame->nr);
			lapb_stop_t1timer(lapb);
			lapb->n2count = 0;
			lapb_requeue_frames(lapb);
		} else {
			lapb->frmr_data = *frame;
			lapb->frmr_type = LAPB_FRMR_Z;
			lapb_transmit_frmr(lapb);
			lapb_dbg(0, "(%p) S3 -> S4\n", lapb->dev);
			lapb_start_t1timer(lapb);
			lapb_stop_t2timer(lapb);
			lapb->state   = LAPB_STATE_4;
			lapb->n2count = 0;
		}
		break;

	case LAPB_I:
		lapb_dbg(1, "(%p) S3 RX I(%d) S%d R%d\n",
			 lapb->dev, frame->pf, frame->ns, frame->nr);
		if (!lapb_validate_nr(lapb, frame->nr)) {
			lapb->frmr_data = *frame;
			lapb->frmr_type = LAPB_FRMR_Z;
			lapb_transmit_frmr(lapb);
			lapb_dbg(0, "(%p) S3 -> S4\n", lapb->dev);
			lapb_start_t1timer(lapb);
			lapb_stop_t2timer(lapb);
			lapb->state   = LAPB_STATE_4;
			lapb->n2count = 0;
			break;
		}
		if (lapb->condition & LAPB_PEER_RX_BUSY_CONDITION)
			lapb_frames_acked(lapb, frame->nr);
		else
			lapb_check_iframes_acked(lapb, frame->nr);

		if (frame->ns == lapb->vr) {
			int cn;
			cn = lapb_data_indication(lapb, skb);
			queued = 1;
			/*
			 * If upper layer has dropped the frame, we
			 * basically ignore any further protocol
			 * processing. This will cause the peer
			 * to re-transmit the frame later like
			 * a frame lost on the wire.
			 */
			if (cn == NET_RX_DROP) {
				pr_debug("rx congestion\n");
				break;
			}
			lapb->vr = (lapb->vr + 1) % modulus;
			lapb->condition &= ~LAPB_REJECT_CONDITION;
			if (frame->pf)
				lapb_enquiry_response(lapb);
			else {
				if (!(lapb->condition &
				      LAPB_ACK_PENDING_CONDITION)) {
					lapb->condition |= LAPB_ACK_PENDING_CONDITION;
					lapb_start_t2timer(lapb);
				}
			}
		} else {
			if (lapb->condition & LAPB_REJECT_CONDITION) {
				if (frame->pf)
					lapb_enquiry_response(lapb);
			} else {
				lapb_dbg(1, "(%p) S3 TX REJ(%d) R%d\n",
					 lapb->dev, frame->pf, lapb->vr);
				lapb->condition |= LAPB_REJECT_CONDITION;
				lapb_send_control(lapb, LAPB_REJ, frame->pf,
						  LAPB_RESPONSE);
				lapb->condition &= ~LAPB_ACK_PENDING_CONDITION;
			}
		}
		break;

	case LAPB_FRMR:
		lapb_dbg(1, "(%p) S3 RX FRMR(%d) %5ph\n",
			 lapb->dev, frame->pf,
			 skb->data);
		lapb_establish_data_link(lapb);
		lapb_dbg(0, "(%p) S3 -> S1\n", lapb->dev);
		lapb_requeue_frames(lapb);
		lapb->state = LAPB_STATE_1;
		break;

	case LAPB_ILLEGAL:
		lapb_dbg(1, "(%p) S3 RX ILLEGAL(%d)\n", lapb->dev, frame->pf);
		lapb->frmr_data = *frame;
		lapb->frmr_type = LAPB_FRMR_W;
		lapb_transmit_frmr(lapb);
		lapb_dbg(0, "(%p) S3 -> S4\n", lapb->dev);
		lapb_start_t1timer(lapb);
		lapb_stop_t2timer(lapb);
		lapb->state   = LAPB_STATE_4;
		lapb->n2count = 0;
		break;
	}

	if (!queued)
		kfree_skb(skb);
}

/*
 *	State machine for state 4, Frame Reject State.
 *	The handling of the timer(s) is in file lapb_timer.c.
 */
static void lapb_state4_machine(struct lapb_cb *lapb, struct sk_buff *skb,
				struct lapb_frame *frame)
{
	switch (frame->type) {
	case LAPB_SABM:
		lapb_dbg(1, "(%p) S4 RX SABM(%d)\n", lapb->dev, frame->pf);
		if (lapb->mode & LAPB_EXTENDED) {
			lapb_dbg(1, "(%p) S4 TX DM(%d)\n",
				 lapb->dev, frame->pf);
			lapb_send_control(lapb, LAPB_DM, frame->pf,
					  LAPB_RESPONSE);
		} else {
			lapb_dbg(1, "(%p) S4 TX UA(%d)\n",
				 lapb->dev, frame->pf);
			lapb_dbg(0, "(%p) S4 -> S3\n", lapb->dev);
			lapb_send_control(lapb, LAPB_UA, frame->pf,
					  LAPB_RESPONSE);
			lapb_stop_t1timer(lapb);
			lapb_stop_t2timer(lapb);
			lapb->state     = LAPB_STATE_3;
			lapb->condition = 0x00;
			lapb->n2count   = 0;
			lapb->vs        = 0;
			lapb->vr        = 0;
			lapb->va        = 0;
			lapb_connect_indication(lapb, LAPB_OK);
		}
		break;

	case LAPB_SABME:
		lapb_dbg(1, "(%p) S4 RX SABME(%d)\n", lapb->dev, frame->pf);
		if (lapb->mode & LAPB_EXTENDED) {
			lapb_dbg(1, "(%p) S4 TX UA(%d)\n",
				 lapb->dev, frame->pf);
			lapb_dbg(0, "(%p) S4 -> S3\n", lapb->dev);
			lapb_send_control(lapb, LAPB_UA, frame->pf,
					  LAPB_RESPONSE);
			lapb_stop_t1timer(lapb);
			lapb_stop_t2timer(lapb);
			lapb->state     = LAPB_STATE_3;
			lapb->condition = 0x00;
			lapb->n2count   = 0;
			lapb->vs        = 0;
			lapb->vr        = 0;
			lapb->va        = 0;
			lapb_connect_indication(lapb, LAPB_OK);
		} else {
			lapb_dbg(1, "(%p) S4 TX DM(%d)\n",
				 lapb->dev, frame->pf);
			lapb_send_control(lapb, LAPB_DM, frame->pf,
					  LAPB_RESPONSE);
		}
		break;
	}

	kfree_skb(skb);
}

/*
 *	Process an incoming LAPB frame
 */
void lapb_data_input(struct lapb_cb *lapb, struct sk_buff *skb)
{
	struct lapb_frame frame;

	if (lapb_decode(lapb, skb, &frame) < 0) {
		kfree_skb(skb);
		return;
	}

	switch (lapb->state) {
	case LAPB_STATE_0:
		lapb_state0_machine(lapb, skb, &frame); break;
	case LAPB_STATE_1:
		lapb_state1_machine(lapb, skb, &frame); break;
	case LAPB_STATE_2:
		lapb_state2_machine(lapb, skb, &frame); break;
	case LAPB_STATE_3:
		lapb_state3_machine(lapb, skb, &frame); break;
	case LAPB_STATE_4:
		lapb_state4_machine(lapb, skb, &frame); break;
	}

	lapb_kick(lapb);
}
