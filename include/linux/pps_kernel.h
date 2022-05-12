/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * PPS API kernel header
 *
 * Copyright (C) 2009   Rodolfo Giometti <giometti@linux.it>
 */

#ifndef LINUX_PPS_KERNEL_H
#define LINUX_PPS_KERNEL_H

#include <linux/pps.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/time.h>

/*
 * Global defines
 */

struct pps_device;

/* The specific PPS source info */
struct pps_source_info {
	char name[PPS_MAX_NAME_LEN];		/* symbolic name */
	char path[PPS_MAX_NAME_LEN];		/* path of connected device */
	int mode;				/* PPS allowed mode */

	void (*echo)(struct pps_device *pps,
			int event, void *data);	/* PPS echo function */

	struct module *owner;
	struct device *dev;		/* Parent device for device_create */
};

struct pps_event_time {
#ifdef CONFIG_NTP_PPS
	struct timespec64 ts_raw;
#endif /* CONFIG_NTP_PPS */
	struct timespec64 ts_real;
};

/* The main struct */
struct pps_device {
	struct pps_source_info info;		/* PSS source info */

	struct pps_kparams params;		/* PPS current params */

	__u32 assert_sequence;			/* PPS assert event seq # */
	__u32 clear_sequence;			/* PPS clear event seq # */
	struct pps_ktime assert_tu;
	struct pps_ktime clear_tu;
	int current_mode;			/* PPS mode at event time */

	unsigned int last_ev;			/* last PPS event id */
	wait_queue_head_t queue;		/* PPS event queue */

	unsigned int id;			/* PPS source unique ID */
	void const *lookup_cookie;		/* For pps_lookup_dev() only */
	struct cdev cdev;
	struct device *dev;
	struct fasync_struct *async_queue;	/* fasync method */
	spinlock_t lock;
};

/*
 * Global variables
 */

extern const struct attribute_group *pps_groups[];

/*
 * Internal functions.
 *
 * These are not actually part of the exported API, but this is a
 * convenient header file to put them in.
 */

extern int pps_register_cdev(struct pps_device *pps);
extern void pps_unregister_cdev(struct pps_device *pps);

/*
 * Exported functions
 */

extern struct pps_device *pps_register_source(
		struct pps_source_info *info, int default_params);
extern void pps_unregister_source(struct pps_device *pps);
extern void pps_event(struct pps_device *pps,
		struct pps_event_time *ts, int event, void *data);
/* Look up a pps_device by magic cookie */
struct pps_device *pps_lookup_dev(void const *cookie);

static inline void timespec_to_pps_ktime(struct pps_ktime *kt,
		struct timespec64 ts)
{
	kt->sec = ts.tv_sec;
	kt->nsec = ts.tv_nsec;
}

static inline void pps_get_ts(struct pps_event_time *ts)
{
	struct system_time_snapshot snap;

	ktime_get_snapshot(&snap);
	ts->ts_real = ktime_to_timespec64(snap.real);
#ifdef CONFIG_NTP_PPS
	ts->ts_raw = ktime_to_timespec64(snap.raw);
#endif
}

/* Subtract known time delay from PPS event time(s) */
static inline void pps_sub_ts(struct pps_event_time *ts, struct timespec64 delta)
{
	ts->ts_real = timespec64_sub(ts->ts_real, delta);
#ifdef CONFIG_NTP_PPS
	ts->ts_raw = timespec64_sub(ts->ts_raw, delta);
#endif
}

#endif /* LINUX_PPS_KERNEL_H */
