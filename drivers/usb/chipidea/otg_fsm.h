/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2014 Freescale Semiconductor, Inc.
 *
 * Author: Jun Li
 */

#ifndef __DRIVERS_USB_CHIPIDEA_OTG_FSM_H
#define __DRIVERS_USB_CHIPIDEA_OTG_FSM_H

#include <linux/usb/otg-fsm.h>

/*
 *  A-DEVICE timing  constants
 */

/* Wait for VBUS Rise  */
#define TA_WAIT_VRISE        (100)	/* a_wait_vrise: section 7.1.2
					 * a_wait_vrise_tmr: section 7.4.5.1
					 * TA_VBUS_RISE <= 100ms, section 4.4
					 * Table 4-1: Electrical Characteristics
					 * ->DC Electrical Timing
					 */
/* Wait for VBUS Fall  */
#define TA_WAIT_VFALL        (1000)	/* a_wait_vfall: section 7.1.7
					 * a_wait_vfall_tmr: section: 7.4.5.2
					 */
/* Wait for B-Connect */
#define TA_WAIT_BCON         (10000)	/* a_wait_bcon: section 7.1.3
					 * TA_WAIT_BCON: should be between 1100
					 * and 30000 ms, section 5.5, Table 5-1
					 */
/* A-Idle to B-Disconnect */
#define TA_AIDL_BDIS         (5000)	/* a_suspend min 200 ms, section 5.2.1
					 * TA_AIDL_BDIS: section 5.5, Table 5-1
					 */
/* B-Idle to A-Disconnect */
#define TA_BIDL_ADIS         (500)	/* TA_BIDL_ADIS: section 5.2.1
					 * 500ms is used for B switch to host
					 * for safe
					 */

/*
 * B-device timing constants
 */

/* Data-Line Pulse Time*/
#define TB_DATA_PLS          (10)	/* b_srp_init,continue 5~10ms
					 * section:5.1.3
					 */
/* SRP Fail Time  */
#define TB_SRP_FAIL          (6000)	/* b_srp_init,fail time 5~6s
					 * section:5.1.6
					 */
/* A-SE0 to B-Reset  */
#define TB_ASE0_BRST         (155)	/* minimum 155 ms, section:5.3.1 */
/* SE0 Time Before SRP */
#define TB_SE0_SRP           (1000)	/* b_idle,minimum 1s, section:5.1.2 */
/* SSEND time before SRP */
#define TB_SSEND_SRP         (1500)	/* minimum 1.5 sec, section:5.1.2 */

#define TB_AIDL_BDIS         (20)	/* 4ms ~ 150ms, section 5.2.1 */

#if IS_ENABLED(CONFIG_USB_OTG_FSM)

int ci_hdrc_otg_fsm_init(struct ci_hdrc *ci);
int ci_otg_fsm_work(struct ci_hdrc *ci);
irqreturn_t ci_otg_fsm_irq(struct ci_hdrc *ci);
void ci_hdrc_otg_fsm_start(struct ci_hdrc *ci);
void ci_hdrc_otg_fsm_remove(struct ci_hdrc *ci);

#else

static inline int ci_hdrc_otg_fsm_init(struct ci_hdrc *ci)
{
	return 0;
}

static inline int ci_otg_fsm_work(struct ci_hdrc *ci)
{
	return -ENXIO;
}

static inline irqreturn_t ci_otg_fsm_irq(struct ci_hdrc *ci)
{
	return IRQ_NONE;
}

static inline void ci_hdrc_otg_fsm_start(struct ci_hdrc *ci)
{

}

static inline void ci_hdrc_otg_fsm_remove(struct ci_hdrc *ci)
{

}

#endif

#endif /* __DRIVERS_USB_CHIPIDEA_OTG_FSM_H */
