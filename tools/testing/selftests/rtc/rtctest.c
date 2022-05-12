// SPDX-License-Identifier: GPL-2.0
/*
 * Real Time Clock Driver Test Program
 *
 * Copyright (c) 2018 Alexandre Belloni <alexandre.belloni@bootlin.com>
 */

#include <errno.h>
#include <fcntl.h>
#include <linux/rtc.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "../kselftest_harness.h"

#define NUM_UIE 3
#define ALARM_DELTA 3

static char *rtc_file = "/dev/rtc0";

FIXTURE(rtc) {
	int fd;
};

FIXTURE_SETUP(rtc) {
	self->fd = open(rtc_file, O_RDONLY);
	ASSERT_NE(-1, self->fd);
}

FIXTURE_TEARDOWN(rtc) {
	close(self->fd);
}

TEST_F(rtc, date_read) {
	int rc;
	struct rtc_time rtc_tm;

	/* Read the RTC time/date */
	rc = ioctl(self->fd, RTC_RD_TIME, &rtc_tm);
	ASSERT_NE(-1, rc);

	TH_LOG("Current RTC date/time is %02d/%02d/%02d %02d:%02d:%02d.",
	       rtc_tm.tm_mday, rtc_tm.tm_mon + 1, rtc_tm.tm_year + 1900,
	       rtc_tm.tm_hour, rtc_tm.tm_min, rtc_tm.tm_sec);
}

TEST_F_TIMEOUT(rtc, uie_read, NUM_UIE + 2) {
	int i, rc, irq = 0;
	unsigned long data;

	/* Turn on update interrupts */
	rc = ioctl(self->fd, RTC_UIE_ON, 0);
	if (rc == -1) {
		ASSERT_EQ(EINVAL, errno);
		TH_LOG("skip update IRQs not supported.");
		return;
	}

	for (i = 0; i < NUM_UIE; i++) {
		/* This read will block */
		rc = read(self->fd, &data, sizeof(data));
		ASSERT_NE(-1, rc);
		irq++;
	}

	EXPECT_EQ(NUM_UIE, irq);

	rc = ioctl(self->fd, RTC_UIE_OFF, 0);
	ASSERT_NE(-1, rc);
}

TEST_F(rtc, uie_select) {
	int i, rc, irq = 0;
	unsigned long data;

	/* Turn on update interrupts */
	rc = ioctl(self->fd, RTC_UIE_ON, 0);
	if (rc == -1) {
		ASSERT_EQ(EINVAL, errno);
		TH_LOG("skip update IRQs not supported.");
		return;
	}

	for (i = 0; i < NUM_UIE; i++) {
		struct timeval tv = { .tv_sec = 2 };
		fd_set readfds;

		FD_ZERO(&readfds);
		FD_SET(self->fd, &readfds);
		/* The select will wait until an RTC interrupt happens. */
		rc = select(self->fd + 1, &readfds, NULL, NULL, &tv);
		ASSERT_NE(-1, rc);
		ASSERT_NE(0, rc);

		/* This read won't block */
		rc = read(self->fd, &data, sizeof(unsigned long));
		ASSERT_NE(-1, rc);
		irq++;
	}

	EXPECT_EQ(NUM_UIE, irq);

	rc = ioctl(self->fd, RTC_UIE_OFF, 0);
	ASSERT_NE(-1, rc);
}

TEST_F(rtc, alarm_alm_set) {
	struct timeval tv = { .tv_sec = ALARM_DELTA + 2 };
	unsigned long data;
	struct rtc_time tm;
	fd_set readfds;
	time_t secs, new;
	int rc;

	rc = ioctl(self->fd, RTC_RD_TIME, &tm);
	ASSERT_NE(-1, rc);

	secs = timegm((struct tm *)&tm) + ALARM_DELTA;
	gmtime_r(&secs, (struct tm *)&tm);

	rc = ioctl(self->fd, RTC_ALM_SET, &tm);
	if (rc == -1) {
		ASSERT_EQ(EINVAL, errno);
		TH_LOG("skip alarms are not supported.");
		return;
	}

	rc = ioctl(self->fd, RTC_ALM_READ, &tm);
	ASSERT_NE(-1, rc);

	TH_LOG("Alarm time now set to %02d:%02d:%02d.",
	       tm.tm_hour, tm.tm_min, tm.tm_sec);

	/* Enable alarm interrupts */
	rc = ioctl(self->fd, RTC_AIE_ON, 0);
	ASSERT_NE(-1, rc);

	FD_ZERO(&readfds);
	FD_SET(self->fd, &readfds);

	rc = select(self->fd + 1, &readfds, NULL, NULL, &tv);
	ASSERT_NE(-1, rc);
	ASSERT_NE(0, rc);

	/* Disable alarm interrupts */
	rc = ioctl(self->fd, RTC_AIE_OFF, 0);
	ASSERT_NE(-1, rc);

	rc = read(self->fd, &data, sizeof(unsigned long));
	ASSERT_NE(-1, rc);
	TH_LOG("data: %lx", data);

	rc = ioctl(self->fd, RTC_RD_TIME, &tm);
	ASSERT_NE(-1, rc);

	new = timegm((struct tm *)&tm);
	ASSERT_EQ(new, secs);
}

TEST_F(rtc, alarm_wkalm_set) {
	struct timeval tv = { .tv_sec = ALARM_DELTA + 2 };
	struct rtc_wkalrm alarm = { 0 };
	struct rtc_time tm;
	unsigned long data;
	fd_set readfds;
	time_t secs, new;
	int rc;

	rc = ioctl(self->fd, RTC_RD_TIME, &alarm.time);
	ASSERT_NE(-1, rc);

	secs = timegm((struct tm *)&alarm.time) + ALARM_DELTA;
	gmtime_r(&secs, (struct tm *)&alarm.time);

	alarm.enabled = 1;

	rc = ioctl(self->fd, RTC_WKALM_SET, &alarm);
	if (rc == -1) {
		ASSERT_EQ(EINVAL, errno);
		TH_LOG("skip alarms are not supported.");
		return;
	}

	rc = ioctl(self->fd, RTC_WKALM_RD, &alarm);
	ASSERT_NE(-1, rc);

	TH_LOG("Alarm time now set to %02d/%02d/%02d %02d:%02d:%02d.",
	       alarm.time.tm_mday, alarm.time.tm_mon + 1,
	       alarm.time.tm_year + 1900, alarm.time.tm_hour,
	       alarm.time.tm_min, alarm.time.tm_sec);

	FD_ZERO(&readfds);
	FD_SET(self->fd, &readfds);

	rc = select(self->fd + 1, &readfds, NULL, NULL, &tv);
	ASSERT_NE(-1, rc);
	ASSERT_NE(0, rc);

	rc = read(self->fd, &data, sizeof(unsigned long));
	ASSERT_NE(-1, rc);

	rc = ioctl(self->fd, RTC_RD_TIME, &tm);
	ASSERT_NE(-1, rc);

	new = timegm((struct tm *)&tm);
	ASSERT_EQ(new, secs);
}

TEST_F_TIMEOUT(rtc, alarm_alm_set_minute, 65) {
	struct timeval tv = { .tv_sec = 62 };
	unsigned long data;
	struct rtc_time tm;
	fd_set readfds;
	time_t secs, new;
	int rc;

	rc = ioctl(self->fd, RTC_RD_TIME, &tm);
	ASSERT_NE(-1, rc);

	secs = timegm((struct tm *)&tm) + 60 - tm.tm_sec;
	gmtime_r(&secs, (struct tm *)&tm);

	rc = ioctl(self->fd, RTC_ALM_SET, &tm);
	if (rc == -1) {
		ASSERT_EQ(EINVAL, errno);
		TH_LOG("skip alarms are not supported.");
		return;
	}

	rc = ioctl(self->fd, RTC_ALM_READ, &tm);
	ASSERT_NE(-1, rc);

	TH_LOG("Alarm time now set to %02d:%02d:%02d.",
	       tm.tm_hour, tm.tm_min, tm.tm_sec);

	/* Enable alarm interrupts */
	rc = ioctl(self->fd, RTC_AIE_ON, 0);
	ASSERT_NE(-1, rc);

	FD_ZERO(&readfds);
	FD_SET(self->fd, &readfds);

	rc = select(self->fd + 1, &readfds, NULL, NULL, &tv);
	ASSERT_NE(-1, rc);
	ASSERT_NE(0, rc);

	/* Disable alarm interrupts */
	rc = ioctl(self->fd, RTC_AIE_OFF, 0);
	ASSERT_NE(-1, rc);

	rc = read(self->fd, &data, sizeof(unsigned long));
	ASSERT_NE(-1, rc);
	TH_LOG("data: %lx", data);

	rc = ioctl(self->fd, RTC_RD_TIME, &tm);
	ASSERT_NE(-1, rc);

	new = timegm((struct tm *)&tm);
	ASSERT_EQ(new, secs);
}

TEST_F_TIMEOUT(rtc, alarm_wkalm_set_minute, 65) {
	struct timeval tv = { .tv_sec = 62 };
	struct rtc_wkalrm alarm = { 0 };
	struct rtc_time tm;
	unsigned long data;
	fd_set readfds;
	time_t secs, new;
	int rc;

	rc = ioctl(self->fd, RTC_RD_TIME, &alarm.time);
	ASSERT_NE(-1, rc);

	secs = timegm((struct tm *)&alarm.time) + 60 - alarm.time.tm_sec;
	gmtime_r(&secs, (struct tm *)&alarm.time);

	alarm.enabled = 1;

	rc = ioctl(self->fd, RTC_WKALM_SET, &alarm);
	if (rc == -1) {
		ASSERT_EQ(EINVAL, errno);
		TH_LOG("skip alarms are not supported.");
		return;
	}

	rc = ioctl(self->fd, RTC_WKALM_RD, &alarm);
	ASSERT_NE(-1, rc);

	TH_LOG("Alarm time now set to %02d/%02d/%02d %02d:%02d:%02d.",
	       alarm.time.tm_mday, alarm.time.tm_mon + 1,
	       alarm.time.tm_year + 1900, alarm.time.tm_hour,
	       alarm.time.tm_min, alarm.time.tm_sec);

	FD_ZERO(&readfds);
	FD_SET(self->fd, &readfds);

	rc = select(self->fd + 1, &readfds, NULL, NULL, &tv);
	ASSERT_NE(-1, rc);
	ASSERT_NE(0, rc);

	rc = read(self->fd, &data, sizeof(unsigned long));
	ASSERT_NE(-1, rc);

	rc = ioctl(self->fd, RTC_RD_TIME, &tm);
	ASSERT_NE(-1, rc);

	new = timegm((struct tm *)&tm);
	ASSERT_EQ(new, secs);
}

static void __attribute__((constructor))
__constructor_order_last(void)
{
	if (!__constructor_order)
		__constructor_order = _CONSTRUCTOR_ORDER_BACKWARD;
}

int main(int argc, char **argv)
{
	switch (argc) {
	case 2:
		rtc_file = argv[1];
		/* FALLTHROUGH */
	case 1:
		break;
	default:
		fprintf(stderr, "usage: %s [rtcdev]\n", argv[0]);
		return 1;
	}

	return test_harness_run(argc, argv);
}
