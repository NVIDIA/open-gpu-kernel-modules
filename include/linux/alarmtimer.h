/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_ALARMTIMER_H
#define _LINUX_ALARMTIMER_H

#include <linux/time.h>
#include <linux/hrtimer.h>
#include <linux/timerqueue.h>

struct rtc_device;

enum alarmtimer_type {
	ALARM_REALTIME,
	ALARM_BOOTTIME,

	/* Supported types end here */
	ALARM_NUMTYPE,

	/* Used for tracing information. No usable types. */
	ALARM_REALTIME_FREEZER,
	ALARM_BOOTTIME_FREEZER,
};

enum alarmtimer_restart {
	ALARMTIMER_NORESTART,
	ALARMTIMER_RESTART,
};


#define ALARMTIMER_STATE_INACTIVE	0x00
#define ALARMTIMER_STATE_ENQUEUED	0x01

/**
 * struct alarm - Alarm timer structure
 * @node:	timerqueue node for adding to the event list this value
 *		also includes the expiration time.
 * @timer:	hrtimer used to schedule events while running
 * @function:	Function pointer to be executed when the timer fires.
 * @type:	Alarm type (BOOTTIME/REALTIME).
 * @state:	Flag that represents if the alarm is set to fire or not.
 * @data:	Internal data value.
 */
struct alarm {
	struct timerqueue_node	node;
	struct hrtimer		timer;
	enum alarmtimer_restart	(*function)(struct alarm *, ktime_t now);
	enum alarmtimer_type	type;
	int			state;
	void			*data;
};

void alarm_init(struct alarm *alarm, enum alarmtimer_type type,
		enum alarmtimer_restart (*function)(struct alarm *, ktime_t));
void alarm_start(struct alarm *alarm, ktime_t start);
void alarm_start_relative(struct alarm *alarm, ktime_t start);
void alarm_restart(struct alarm *alarm);
int alarm_try_to_cancel(struct alarm *alarm);
int alarm_cancel(struct alarm *alarm);

u64 alarm_forward(struct alarm *alarm, ktime_t now, ktime_t interval);
u64 alarm_forward_now(struct alarm *alarm, ktime_t interval);
ktime_t alarm_expires_remaining(const struct alarm *alarm);

#ifdef CONFIG_RTC_CLASS
/* Provide way to access the rtc device being used by alarmtimers */
struct rtc_device *alarmtimer_get_rtcdev(void);
#else
static inline struct rtc_device *alarmtimer_get_rtcdev(void) { return NULL; }
#endif

#endif
