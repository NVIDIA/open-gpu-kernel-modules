// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * PTP 1588 clock support - User space test program
 *
 * Copyright (C) 2010 OMICRON electronics GmbH
 */
#define _GNU_SOURCE
#define __SANE_USERSPACE_TYPES__        /* For PPC64, to get LL64 types */
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/timex.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <linux/ptp_clock.h>

#define DEVICE "/dev/ptp0"

#ifndef ADJ_SETOFFSET
#define ADJ_SETOFFSET 0x0100
#endif

#ifndef CLOCK_INVALID
#define CLOCK_INVALID -1
#endif

#define NSEC_PER_SEC 1000000000LL

/* clock_adjtime is not available in GLIBC < 2.14 */
#if !__GLIBC_PREREQ(2, 14)
#include <sys/syscall.h>
static int clock_adjtime(clockid_t id, struct timex *tx)
{
	return syscall(__NR_clock_adjtime, id, tx);
}
#endif

static void show_flag_test(int rq_index, unsigned int flags, int err)
{
	printf("PTP_EXTTS_REQUEST%c flags 0x%08x : (%d) %s\n",
	       rq_index ? '1' + rq_index : ' ',
	       flags, err, strerror(errno));
	/* sigh, uClibc ... */
	errno = 0;
}

static void do_flag_test(int fd, unsigned int index)
{
	struct ptp_extts_request extts_request;
	unsigned long request[2] = {
		PTP_EXTTS_REQUEST,
		PTP_EXTTS_REQUEST2,
	};
	unsigned int enable_flags[5] = {
		PTP_ENABLE_FEATURE,
		PTP_ENABLE_FEATURE | PTP_RISING_EDGE,
		PTP_ENABLE_FEATURE | PTP_FALLING_EDGE,
		PTP_ENABLE_FEATURE | PTP_RISING_EDGE | PTP_FALLING_EDGE,
		PTP_ENABLE_FEATURE | (PTP_EXTTS_VALID_FLAGS + 1),
	};
	int err, i, j;

	memset(&extts_request, 0, sizeof(extts_request));
	extts_request.index = index;

	for (i = 0; i < 2; i++) {
		for (j = 0; j < 5; j++) {
			extts_request.flags = enable_flags[j];
			err = ioctl(fd, request[i], &extts_request);
			show_flag_test(i, extts_request.flags, err);

			extts_request.flags = 0;
			err = ioctl(fd, request[i], &extts_request);
		}
	}
}

static clockid_t get_clockid(int fd)
{
#define CLOCKFD 3
	return (((unsigned int) ~fd) << 3) | CLOCKFD;
}

static long ppb_to_scaled_ppm(int ppb)
{
	/*
	 * The 'freq' field in the 'struct timex' is in parts per
	 * million, but with a 16 bit binary fractional field.
	 * Instead of calculating either one of
	 *
	 *    scaled_ppm = (ppb / 1000) << 16  [1]
	 *    scaled_ppm = (ppb << 16) / 1000  [2]
	 *
	 * we simply use double precision math, in order to avoid the
	 * truncation in [1] and the possible overflow in [2].
	 */
	return (long) (ppb * 65.536);
}

static int64_t pctns(struct ptp_clock_time *t)
{
	return t->sec * 1000000000LL + t->nsec;
}

static void usage(char *progname)
{
	fprintf(stderr,
		"usage: %s [options]\n"
		" -c         query the ptp clock's capabilities\n"
		" -d name    device to open\n"
		" -e val     read 'val' external time stamp events\n"
		" -f val     adjust the ptp clock frequency by 'val' ppb\n"
		" -g         get the ptp clock time\n"
		" -h         prints this message\n"
		" -i val     index for event/trigger\n"
		" -k val     measure the time offset between system and phc clock\n"
		"            for 'val' times (Maximum 25)\n"
		" -l         list the current pin configuration\n"
		" -L pin,val configure pin index 'pin' with function 'val'\n"
		"            the channel index is taken from the '-i' option\n"
		"            'val' specifies the auxiliary function:\n"
		"            0 - none\n"
		"            1 - external time stamp\n"
		"            2 - periodic output\n"
		" -p val     enable output with a period of 'val' nanoseconds\n"
		" -H val     set output phase to 'val' nanoseconds (requires -p)\n"
		" -w val     set output pulse width to 'val' nanoseconds (requires -p)\n"
		" -P val     enable or disable (val=1|0) the system clock PPS\n"
		" -s         set the ptp clock time from the system time\n"
		" -S         set the system time from the ptp clock time\n"
		" -t val     shift the ptp clock time by 'val' seconds\n"
		" -T val     set the ptp clock time to 'val' seconds\n"
		" -z         test combinations of rising/falling external time stamp flags\n",
		progname);
}

int main(int argc, char *argv[])
{
	struct ptp_clock_caps caps;
	struct ptp_extts_event event;
	struct ptp_extts_request extts_request;
	struct ptp_perout_request perout_request;
	struct ptp_pin_desc desc;
	struct timespec ts;
	struct timex tx;
	struct ptp_clock_time *pct;
	struct ptp_sys_offset *sysoff;

	char *progname;
	unsigned int i;
	int c, cnt, fd;

	char *device = DEVICE;
	clockid_t clkid;
	int adjfreq = 0x7fffffff;
	int adjtime = 0;
	int capabilities = 0;
	int extts = 0;
	int flagtest = 0;
	int gettime = 0;
	int index = 0;
	int list_pins = 0;
	int pct_offset = 0;
	int n_samples = 0;
	int pin_index = -1, pin_func;
	int pps = -1;
	int seconds = 0;
	int settime = 0;

	int64_t t1, t2, tp;
	int64_t interval, offset;
	int64_t perout_phase = -1;
	int64_t pulsewidth = -1;
	int64_t perout = -1;

	progname = strrchr(argv[0], '/');
	progname = progname ? 1+progname : argv[0];
	while (EOF != (c = getopt(argc, argv, "cd:e:f:ghH:i:k:lL:p:P:sSt:T:w:z"))) {
		switch (c) {
		case 'c':
			capabilities = 1;
			break;
		case 'd':
			device = optarg;
			break;
		case 'e':
			extts = atoi(optarg);
			break;
		case 'f':
			adjfreq = atoi(optarg);
			break;
		case 'g':
			gettime = 1;
			break;
		case 'H':
			perout_phase = atoll(optarg);
			break;
		case 'i':
			index = atoi(optarg);
			break;
		case 'k':
			pct_offset = 1;
			n_samples = atoi(optarg);
			break;
		case 'l':
			list_pins = 1;
			break;
		case 'L':
			cnt = sscanf(optarg, "%d,%d", &pin_index, &pin_func);
			if (cnt != 2) {
				usage(progname);
				return -1;
			}
			break;
		case 'p':
			perout = atoll(optarg);
			break;
		case 'P':
			pps = atoi(optarg);
			break;
		case 's':
			settime = 1;
			break;
		case 'S':
			settime = 2;
			break;
		case 't':
			adjtime = atoi(optarg);
			break;
		case 'T':
			settime = 3;
			seconds = atoi(optarg);
			break;
		case 'w':
			pulsewidth = atoi(optarg);
			break;
		case 'z':
			flagtest = 1;
			break;
		case 'h':
			usage(progname);
			return 0;
		case '?':
		default:
			usage(progname);
			return -1;
		}
	}

	fd = open(device, O_RDWR);
	if (fd < 0) {
		fprintf(stderr, "opening %s: %s\n", device, strerror(errno));
		return -1;
	}

	clkid = get_clockid(fd);
	if (CLOCK_INVALID == clkid) {
		fprintf(stderr, "failed to read clock id\n");
		return -1;
	}

	if (capabilities) {
		if (ioctl(fd, PTP_CLOCK_GETCAPS, &caps)) {
			perror("PTP_CLOCK_GETCAPS");
		} else {
			printf("capabilities:\n"
			       "  %d maximum frequency adjustment (ppb)\n"
			       "  %d programmable alarms\n"
			       "  %d external time stamp channels\n"
			       "  %d programmable periodic signals\n"
			       "  %d pulse per second\n"
			       "  %d programmable pins\n"
			       "  %d cross timestamping\n"
			       "  %d adjust_phase\n",
			       caps.max_adj,
			       caps.n_alarm,
			       caps.n_ext_ts,
			       caps.n_per_out,
			       caps.pps,
			       caps.n_pins,
			       caps.cross_timestamping,
			       caps.adjust_phase);
		}
	}

	if (0x7fffffff != adjfreq) {
		memset(&tx, 0, sizeof(tx));
		tx.modes = ADJ_FREQUENCY;
		tx.freq = ppb_to_scaled_ppm(adjfreq);
		if (clock_adjtime(clkid, &tx)) {
			perror("clock_adjtime");
		} else {
			puts("frequency adjustment okay");
		}
	}

	if (adjtime) {
		memset(&tx, 0, sizeof(tx));
		tx.modes = ADJ_SETOFFSET;
		tx.time.tv_sec = adjtime;
		tx.time.tv_usec = 0;
		if (clock_adjtime(clkid, &tx) < 0) {
			perror("clock_adjtime");
		} else {
			puts("time shift okay");
		}
	}

	if (gettime) {
		if (clock_gettime(clkid, &ts)) {
			perror("clock_gettime");
		} else {
			printf("clock time: %ld.%09ld or %s",
			       ts.tv_sec, ts.tv_nsec, ctime(&ts.tv_sec));
		}
	}

	if (settime == 1) {
		clock_gettime(CLOCK_REALTIME, &ts);
		if (clock_settime(clkid, &ts)) {
			perror("clock_settime");
		} else {
			puts("set time okay");
		}
	}

	if (settime == 2) {
		clock_gettime(clkid, &ts);
		if (clock_settime(CLOCK_REALTIME, &ts)) {
			perror("clock_settime");
		} else {
			puts("set time okay");
		}
	}

	if (settime == 3) {
		ts.tv_sec = seconds;
		ts.tv_nsec = 0;
		if (clock_settime(clkid, &ts)) {
			perror("clock_settime");
		} else {
			puts("set time okay");
		}
	}

	if (extts) {
		memset(&extts_request, 0, sizeof(extts_request));
		extts_request.index = index;
		extts_request.flags = PTP_ENABLE_FEATURE;
		if (ioctl(fd, PTP_EXTTS_REQUEST, &extts_request)) {
			perror("PTP_EXTTS_REQUEST");
			extts = 0;
		} else {
			puts("external time stamp request okay");
		}
		for (; extts; extts--) {
			cnt = read(fd, &event, sizeof(event));
			if (cnt != sizeof(event)) {
				perror("read");
				break;
			}
			printf("event index %u at %lld.%09u\n", event.index,
			       event.t.sec, event.t.nsec);
			fflush(stdout);
		}
		/* Disable the feature again. */
		extts_request.flags = 0;
		if (ioctl(fd, PTP_EXTTS_REQUEST, &extts_request)) {
			perror("PTP_EXTTS_REQUEST");
		}
	}

	if (flagtest) {
		do_flag_test(fd, index);
	}

	if (list_pins) {
		int n_pins = 0;
		if (ioctl(fd, PTP_CLOCK_GETCAPS, &caps)) {
			perror("PTP_CLOCK_GETCAPS");
		} else {
			n_pins = caps.n_pins;
		}
		for (i = 0; i < n_pins; i++) {
			desc.index = i;
			if (ioctl(fd, PTP_PIN_GETFUNC, &desc)) {
				perror("PTP_PIN_GETFUNC");
				break;
			}
			printf("name %s index %u func %u chan %u\n",
			       desc.name, desc.index, desc.func, desc.chan);
		}
	}

	if (pulsewidth >= 0 && perout < 0) {
		puts("-w can only be specified together with -p");
		return -1;
	}

	if (perout_phase >= 0 && perout < 0) {
		puts("-H can only be specified together with -p");
		return -1;
	}

	if (perout >= 0) {
		if (clock_gettime(clkid, &ts)) {
			perror("clock_gettime");
			return -1;
		}
		memset(&perout_request, 0, sizeof(perout_request));
		perout_request.index = index;
		perout_request.period.sec = perout / NSEC_PER_SEC;
		perout_request.period.nsec = perout % NSEC_PER_SEC;
		perout_request.flags = 0;
		if (pulsewidth >= 0) {
			perout_request.flags |= PTP_PEROUT_DUTY_CYCLE;
			perout_request.on.sec = pulsewidth / NSEC_PER_SEC;
			perout_request.on.nsec = pulsewidth % NSEC_PER_SEC;
		}
		if (perout_phase >= 0) {
			perout_request.flags |= PTP_PEROUT_PHASE;
			perout_request.phase.sec = perout_phase / NSEC_PER_SEC;
			perout_request.phase.nsec = perout_phase % NSEC_PER_SEC;
		} else {
			perout_request.start.sec = ts.tv_sec + 2;
			perout_request.start.nsec = 0;
		}

		if (ioctl(fd, PTP_PEROUT_REQUEST2, &perout_request)) {
			perror("PTP_PEROUT_REQUEST");
		} else {
			puts("periodic output request okay");
		}
	}

	if (pin_index >= 0) {
		memset(&desc, 0, sizeof(desc));
		desc.index = pin_index;
		desc.func = pin_func;
		desc.chan = index;
		if (ioctl(fd, PTP_PIN_SETFUNC, &desc)) {
			perror("PTP_PIN_SETFUNC");
		} else {
			puts("set pin function okay");
		}
	}

	if (pps != -1) {
		int enable = pps ? 1 : 0;
		if (ioctl(fd, PTP_ENABLE_PPS, enable)) {
			perror("PTP_ENABLE_PPS");
		} else {
			puts("pps for system time request okay");
		}
	}

	if (pct_offset) {
		if (n_samples <= 0 || n_samples > 25) {
			puts("n_samples should be between 1 and 25");
			usage(progname);
			return -1;
		}

		sysoff = calloc(1, sizeof(*sysoff));
		if (!sysoff) {
			perror("calloc");
			return -1;
		}
		sysoff->n_samples = n_samples;

		if (ioctl(fd, PTP_SYS_OFFSET, sysoff))
			perror("PTP_SYS_OFFSET");
		else
			puts("system and phc clock time offset request okay");

		pct = &sysoff->ts[0];
		for (i = 0; i < sysoff->n_samples; i++) {
			t1 = pctns(pct+2*i);
			tp = pctns(pct+2*i+1);
			t2 = pctns(pct+2*i+2);
			interval = t2 - t1;
			offset = (t2 + t1) / 2 - tp;

			printf("system time: %lld.%u\n",
				(pct+2*i)->sec, (pct+2*i)->nsec);
			printf("phc    time: %lld.%u\n",
				(pct+2*i+1)->sec, (pct+2*i+1)->nsec);
			printf("system time: %lld.%u\n",
				(pct+2*i+2)->sec, (pct+2*i+2)->nsec);
			printf("system/phc clock time offset is %" PRId64 " ns\n"
			       "system     clock time delay  is %" PRId64 " ns\n",
				offset, interval);
		}

		free(sysoff);
	}

	close(fd);
	return 0;
}
