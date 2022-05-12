/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * arch/powerpc/include/asm/mpic_timer.h
 *
 * Header file for Mpic Global Timer
 *
 * Copyright 2013 Freescale Semiconductor, Inc.
 *
 * Author: Wang Dongsheng <Dongsheng.Wang@freescale.com>
 *	   Li Yang <leoli@freescale.com>
 */

#ifndef __MPIC_TIMER__
#define __MPIC_TIMER__

#include <linux/interrupt.h>
#include <linux/time.h>

struct mpic_timer {
	void			*dev;
	struct cascade_priv	*cascade_handle;
	unsigned int		num;
	unsigned int		irq;
};

#ifdef CONFIG_MPIC_TIMER
struct mpic_timer *mpic_request_timer(irq_handler_t fn,  void *dev,
		time64_t time);
void mpic_start_timer(struct mpic_timer *handle);
void mpic_stop_timer(struct mpic_timer *handle);
void mpic_get_remain_time(struct mpic_timer *handle, time64_t *time);
void mpic_free_timer(struct mpic_timer *handle);
#else
struct mpic_timer *mpic_request_timer(irq_handler_t fn,  void *dev,
		time64_t time) { return NULL; }
void mpic_start_timer(struct mpic_timer *handle) { }
void mpic_stop_timer(struct mpic_timer *handle) { }
void mpic_get_remain_time(struct mpic_timer *handle, time64_t *time) { }
void mpic_free_timer(struct mpic_timer *handle) { }
#endif

#endif
