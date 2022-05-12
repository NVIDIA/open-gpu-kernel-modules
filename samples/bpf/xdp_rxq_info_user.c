/* SPDX-License-Identifier: GPL-2.0
 * Copyright (c) 2017 Jesper Dangaard Brouer, Red Hat Inc.
 */
static const char *__doc__ = " XDP RX-queue info extract example\n\n"
	"Monitor how many packets per sec (pps) are received\n"
	"per NIC RX queue index and which CPU processed the packet\n"
	;

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <locale.h>
#include <sys/resource.h>
#include <getopt.h>
#include <net/if.h>
#include <time.h>

#include <arpa/inet.h>
#include <linux/if_link.h>

#include <bpf/bpf.h>
#include <bpf/libbpf.h>
#include "bpf_util.h"

static int ifindex = -1;
static char ifname_buf[IF_NAMESIZE];
static char *ifname;
static __u32 prog_id;

static __u32 xdp_flags = XDP_FLAGS_UPDATE_IF_NOEXIST;

static struct bpf_map *stats_global_map;
static struct bpf_map *rx_queue_index_map;

/* Exit return codes */
#define EXIT_OK		0
#define EXIT_FAIL		1
#define EXIT_FAIL_OPTION	2
#define EXIT_FAIL_XDP		3
#define EXIT_FAIL_BPF		4
#define EXIT_FAIL_MEM		5

static const struct option long_options[] = {
	{"help",	no_argument,		NULL, 'h' },
	{"dev",		required_argument,	NULL, 'd' },
	{"skb-mode",	no_argument,		NULL, 'S' },
	{"sec",		required_argument,	NULL, 's' },
	{"no-separators", no_argument,		NULL, 'z' },
	{"action",	required_argument,	NULL, 'a' },
	{"readmem",	no_argument,		NULL, 'r' },
	{"swapmac",	no_argument,		NULL, 'm' },
	{"force",	no_argument,		NULL, 'F' },
	{0, 0, NULL,  0 }
};

static void int_exit(int sig)
{
	__u32 curr_prog_id = 0;

	if (ifindex > -1) {
		if (bpf_get_link_xdp_id(ifindex, &curr_prog_id, xdp_flags)) {
			printf("bpf_get_link_xdp_id failed\n");
			exit(EXIT_FAIL);
		}
		if (prog_id == curr_prog_id) {
			fprintf(stderr,
				"Interrupted: Removing XDP program on ifindex:%d device:%s\n",
				ifindex, ifname);
			bpf_set_link_xdp_fd(ifindex, -1, xdp_flags);
		} else if (!curr_prog_id) {
			printf("couldn't find a prog id on a given iface\n");
		} else {
			printf("program on interface changed, not removing\n");
		}
	}
	exit(EXIT_OK);
}

struct config {
	__u32 action;
	int ifindex;
	__u32 options;
};
enum cfg_options_flags {
	NO_TOUCH = 0x0U,
	READ_MEM = 0x1U,
	SWAP_MAC = 0x2U,
};
#define XDP_ACTION_MAX (XDP_TX + 1)
#define XDP_ACTION_MAX_STRLEN 11
static const char *xdp_action_names[XDP_ACTION_MAX] = {
	[XDP_ABORTED]	= "XDP_ABORTED",
	[XDP_DROP]	= "XDP_DROP",
	[XDP_PASS]	= "XDP_PASS",
	[XDP_TX]	= "XDP_TX",
};

static const char *action2str(int action)
{
	if (action < XDP_ACTION_MAX)
		return xdp_action_names[action];
	return NULL;
}

static int parse_xdp_action(char *action_str)
{
	size_t maxlen;
	__u64 action = -1;
	int i;

	for (i = 0; i < XDP_ACTION_MAX; i++) {
		maxlen = XDP_ACTION_MAX_STRLEN;
		if (strncmp(xdp_action_names[i], action_str, maxlen) == 0) {
			action = i;
			break;
		}
	}
	return action;
}

static void list_xdp_actions(void)
{
	int i;

	printf("Available XDP --action <options>\n");
	for (i = 0; i < XDP_ACTION_MAX; i++)
		printf("\t%s\n", xdp_action_names[i]);
	printf("\n");
}

static char* options2str(enum cfg_options_flags flag)
{
	if (flag == NO_TOUCH)
		return "no_touch";
	if (flag & SWAP_MAC)
		return "swapmac";
	if (flag & READ_MEM)
		return "read";
	fprintf(stderr, "ERR: Unknown config option flags");
	exit(EXIT_FAIL);
}

static void usage(char *argv[])
{
	int i;

	printf("\nDOCUMENTATION:\n%s\n", __doc__);
	printf(" Usage: %s (options-see-below)\n", argv[0]);
	printf(" Listing options:\n");
	for (i = 0; long_options[i].name != 0; i++) {
		printf(" --%-12s", long_options[i].name);
		if (long_options[i].flag != NULL)
			printf(" flag (internal value:%d)",
				*long_options[i].flag);
		else
			printf(" short-option: -%c",
				long_options[i].val);
		printf("\n");
	}
	printf("\n");
	list_xdp_actions();
}

#define NANOSEC_PER_SEC 1000000000 /* 10^9 */
static __u64 gettime(void)
{
	struct timespec t;
	int res;

	res = clock_gettime(CLOCK_MONOTONIC, &t);
	if (res < 0) {
		fprintf(stderr, "Error with gettimeofday! (%i)\n", res);
		exit(EXIT_FAIL);
	}
	return (__u64) t.tv_sec * NANOSEC_PER_SEC + t.tv_nsec;
}

/* Common stats data record shared with _kern.c */
struct datarec {
	__u64 processed;
	__u64 issue;
};
struct record {
	__u64 timestamp;
	struct datarec total;
	struct datarec *cpu;
};
struct stats_record {
	struct record stats;
	struct record *rxq;
};

static struct datarec *alloc_record_per_cpu(void)
{
	unsigned int nr_cpus = bpf_num_possible_cpus();
	struct datarec *array;

	array = calloc(nr_cpus, sizeof(struct datarec));
	if (!array) {
		fprintf(stderr, "Mem alloc error (nr_cpus:%u)\n", nr_cpus);
		exit(EXIT_FAIL_MEM);
	}
	return array;
}

static struct record *alloc_record_per_rxq(void)
{
	unsigned int nr_rxqs = bpf_map__def(rx_queue_index_map)->max_entries;
	struct record *array;

	array = calloc(nr_rxqs, sizeof(struct record));
	if (!array) {
		fprintf(stderr, "Mem alloc error (nr_rxqs:%u)\n", nr_rxqs);
		exit(EXIT_FAIL_MEM);
	}
	return array;
}

static struct stats_record *alloc_stats_record(void)
{
	unsigned int nr_rxqs = bpf_map__def(rx_queue_index_map)->max_entries;
	struct stats_record *rec;
	int i;

	rec = calloc(1, sizeof(struct stats_record));
	if (!rec) {
		fprintf(stderr, "Mem alloc error\n");
		exit(EXIT_FAIL_MEM);
	}
	rec->rxq = alloc_record_per_rxq();
	for (i = 0; i < nr_rxqs; i++)
		rec->rxq[i].cpu = alloc_record_per_cpu();

	rec->stats.cpu = alloc_record_per_cpu();
	return rec;
}

static void free_stats_record(struct stats_record *r)
{
	unsigned int nr_rxqs = bpf_map__def(rx_queue_index_map)->max_entries;
	int i;

	for (i = 0; i < nr_rxqs; i++)
		free(r->rxq[i].cpu);

	free(r->rxq);
	free(r->stats.cpu);
	free(r);
}

static bool map_collect_percpu(int fd, __u32 key, struct record *rec)
{
	/* For percpu maps, userspace gets a value per possible CPU */
	unsigned int nr_cpus = bpf_num_possible_cpus();
	struct datarec values[nr_cpus];
	__u64 sum_processed = 0;
	__u64 sum_issue = 0;
	int i;

	if ((bpf_map_lookup_elem(fd, &key, values)) != 0) {
		fprintf(stderr,
			"ERR: bpf_map_lookup_elem failed key:0x%X\n", key);
		return false;
	}
	/* Get time as close as possible to reading map contents */
	rec->timestamp = gettime();

	/* Record and sum values from each CPU */
	for (i = 0; i < nr_cpus; i++) {
		rec->cpu[i].processed = values[i].processed;
		sum_processed        += values[i].processed;
		rec->cpu[i].issue = values[i].issue;
		sum_issue        += values[i].issue;
	}
	rec->total.processed = sum_processed;
	rec->total.issue     = sum_issue;
	return true;
}

static void stats_collect(struct stats_record *rec)
{
	int fd, i, max_rxqs;

	fd = bpf_map__fd(stats_global_map);
	map_collect_percpu(fd, 0, &rec->stats);

	fd = bpf_map__fd(rx_queue_index_map);
	max_rxqs = bpf_map__def(rx_queue_index_map)->max_entries;
	for (i = 0; i < max_rxqs; i++)
		map_collect_percpu(fd, i, &rec->rxq[i]);
}

static double calc_period(struct record *r, struct record *p)
{
	double period_ = 0;
	__u64 period = 0;

	period = r->timestamp - p->timestamp;
	if (period > 0)
		period_ = ((double) period / NANOSEC_PER_SEC);

	return period_;
}

static __u64 calc_pps(struct datarec *r, struct datarec *p, double period_)
{
	__u64 packets = 0;
	__u64 pps = 0;

	if (period_ > 0) {
		packets = r->processed - p->processed;
		pps = packets / period_;
	}
	return pps;
}

static __u64 calc_errs_pps(struct datarec *r,
			    struct datarec *p, double period_)
{
	__u64 packets = 0;
	__u64 pps = 0;

	if (period_ > 0) {
		packets = r->issue - p->issue;
		pps = packets / period_;
	}
	return pps;
}

static void stats_print(struct stats_record *stats_rec,
			struct stats_record *stats_prev,
			int action, __u32 cfg_opt)
{
	unsigned int nr_rxqs = bpf_map__def(rx_queue_index_map)->max_entries;
	unsigned int nr_cpus = bpf_num_possible_cpus();
	double pps = 0, err = 0;
	struct record *rec, *prev;
	double t;
	int rxq;
	int i;

	/* Header */
	printf("\nRunning XDP on dev:%s (ifindex:%d) action:%s options:%s\n",
	       ifname, ifindex, action2str(action), options2str(cfg_opt));

	/* stats_global_map */
	{
		char *fmt_rx = "%-15s %-7d %'-11.0f %'-10.0f %s\n";
		char *fm2_rx = "%-15s %-7s %'-11.0f\n";
		char *errstr = "";

		printf("%-15s %-7s %-11s %-11s\n",
		       "XDP stats", "CPU", "pps", "issue-pps");

		rec  =  &stats_rec->stats;
		prev = &stats_prev->stats;
		t = calc_period(rec, prev);
		for (i = 0; i < nr_cpus; i++) {
			struct datarec *r = &rec->cpu[i];
			struct datarec *p = &prev->cpu[i];

			pps = calc_pps     (r, p, t);
			err = calc_errs_pps(r, p, t);
			if (err > 0)
				errstr = "invalid-ifindex";
			if (pps > 0)
				printf(fmt_rx, "XDP-RX CPU",
					i, pps, err, errstr);
		}
		pps  = calc_pps     (&rec->total, &prev->total, t);
		err  = calc_errs_pps(&rec->total, &prev->total, t);
		printf(fm2_rx, "XDP-RX CPU", "total", pps, err);
	}

	/* rx_queue_index_map */
	printf("\n%-15s %-7s %-11s %-11s\n",
	       "RXQ stats", "RXQ:CPU", "pps", "issue-pps");

	for (rxq = 0; rxq < nr_rxqs; rxq++) {
		char *fmt_rx = "%-15s %3d:%-3d %'-11.0f %'-10.0f %s\n";
		char *fm2_rx = "%-15s %3d:%-3s %'-11.0f\n";
		char *errstr = "";
		int rxq_ = rxq;

		/* Last RXQ in map catch overflows */
		if (rxq_ == nr_rxqs - 1)
			rxq_ = -1;

		rec  =  &stats_rec->rxq[rxq];
		prev = &stats_prev->rxq[rxq];
		t = calc_period(rec, prev);
		for (i = 0; i < nr_cpus; i++) {
			struct datarec *r = &rec->cpu[i];
			struct datarec *p = &prev->cpu[i];

			pps = calc_pps     (r, p, t);
			err = calc_errs_pps(r, p, t);
			if (err > 0) {
				if (rxq_ == -1)
					errstr = "map-overflow-RXQ";
				else
					errstr = "err";
			}
			if (pps > 0)
				printf(fmt_rx, "rx_queue_index",
				       rxq_, i, pps, err, errstr);
		}
		pps  = calc_pps     (&rec->total, &prev->total, t);
		err  = calc_errs_pps(&rec->total, &prev->total, t);
		if (pps || err)
			printf(fm2_rx, "rx_queue_index", rxq_, "sum", pps, err);
	}
}


/* Pointer swap trick */
static inline void swap(struct stats_record **a, struct stats_record **b)
{
	struct stats_record *tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

static void stats_poll(int interval, int action, __u32 cfg_opt)
{
	struct stats_record *record, *prev;

	record = alloc_stats_record();
	prev   = alloc_stats_record();
	stats_collect(record);

	while (1) {
		swap(&prev, &record);
		stats_collect(record);
		stats_print(record, prev, action, cfg_opt);
		sleep(interval);
	}

	free_stats_record(record);
	free_stats_record(prev);
}


int main(int argc, char **argv)
{
	__u32 cfg_options= NO_TOUCH ; /* Default: Don't touch packet memory */
	struct bpf_prog_load_attr prog_load_attr = {
		.prog_type	= BPF_PROG_TYPE_XDP,
	};
	struct bpf_prog_info info = {};
	__u32 info_len = sizeof(info);
	int prog_fd, map_fd, opt, err;
	bool use_separators = true;
	struct config cfg = { 0 };
	struct bpf_object *obj;
	struct bpf_map *map;
	char filename[256];
	int longindex = 0;
	int interval = 2;
	__u32 key = 0;


	char action_str_buf[XDP_ACTION_MAX_STRLEN + 1 /* for \0 */] = { 0 };
	int action = XDP_PASS; /* Default action */
	char *action_str = NULL;

	snprintf(filename, sizeof(filename), "%s_kern.o", argv[0]);
	prog_load_attr.file = filename;

	if (bpf_prog_load_xattr(&prog_load_attr, &obj, &prog_fd))
		return EXIT_FAIL;

	map =  bpf_object__find_map_by_name(obj, "config_map");
	stats_global_map = bpf_object__find_map_by_name(obj, "stats_global_map");
	rx_queue_index_map = bpf_object__find_map_by_name(obj, "rx_queue_index_map");
	if (!map || !stats_global_map || !rx_queue_index_map) {
		printf("finding a map in obj file failed\n");
		return EXIT_FAIL;
	}
	map_fd = bpf_map__fd(map);

	if (!prog_fd) {
		fprintf(stderr, "ERR: bpf_prog_load_xattr: %s\n", strerror(errno));
		return EXIT_FAIL;
	}

	/* Parse commands line args */
	while ((opt = getopt_long(argc, argv, "FhSrmzd:s:a:",
				  long_options, &longindex)) != -1) {
		switch (opt) {
		case 'd':
			if (strlen(optarg) >= IF_NAMESIZE) {
				fprintf(stderr, "ERR: --dev name too long\n");
				goto error;
			}
			ifname = (char *)&ifname_buf;
			strncpy(ifname, optarg, IF_NAMESIZE);
			ifindex = if_nametoindex(ifname);
			if (ifindex == 0) {
				fprintf(stderr,
					"ERR: --dev name unknown err(%d):%s\n",
					errno, strerror(errno));
				goto error;
			}
			break;
		case 's':
			interval = atoi(optarg);
			break;
		case 'S':
			xdp_flags |= XDP_FLAGS_SKB_MODE;
			break;
		case 'z':
			use_separators = false;
			break;
		case 'a':
			action_str = (char *)&action_str_buf;
			strncpy(action_str, optarg, XDP_ACTION_MAX_STRLEN);
			break;
		case 'r':
			cfg_options |= READ_MEM;
			break;
		case 'm':
			cfg_options |= SWAP_MAC;
			break;
		case 'F':
			xdp_flags &= ~XDP_FLAGS_UPDATE_IF_NOEXIST;
			break;
		case 'h':
		error:
		default:
			usage(argv);
			return EXIT_FAIL_OPTION;
		}
	}

	if (!(xdp_flags & XDP_FLAGS_SKB_MODE))
		xdp_flags |= XDP_FLAGS_DRV_MODE;

	/* Required option */
	if (ifindex == -1) {
		fprintf(stderr, "ERR: required option --dev missing\n");
		usage(argv);
		return EXIT_FAIL_OPTION;
	}
	cfg.ifindex = ifindex;

	/* Parse action string */
	if (action_str) {
		action = parse_xdp_action(action_str);
		if (action < 0) {
			fprintf(stderr, "ERR: Invalid XDP --action: %s\n",
				action_str);
			list_xdp_actions();
			return EXIT_FAIL_OPTION;
		}
	}
	cfg.action = action;

	/* XDP_TX requires changing MAC-addrs, else HW may drop */
	if (action == XDP_TX)
		cfg_options |= SWAP_MAC;
	cfg.options = cfg_options;

	/* Trick to pretty printf with thousands separators use %' */
	if (use_separators)
		setlocale(LC_NUMERIC, "en_US");

	/* User-side setup ifindex in config_map */
	err = bpf_map_update_elem(map_fd, &key, &cfg, 0);
	if (err) {
		fprintf(stderr, "Store config failed (err:%d)\n", err);
		exit(EXIT_FAIL_BPF);
	}

	/* Remove XDP program when program is interrupted or killed */
	signal(SIGINT, int_exit);
	signal(SIGTERM, int_exit);

	if (bpf_set_link_xdp_fd(ifindex, prog_fd, xdp_flags) < 0) {
		fprintf(stderr, "link set xdp fd failed\n");
		return EXIT_FAIL_XDP;
	}

	err = bpf_obj_get_info_by_fd(prog_fd, &info, &info_len);
	if (err) {
		printf("can't get prog info - %s\n", strerror(errno));
		return err;
	}
	prog_id = info.id;

	stats_poll(interval, action, cfg_options);
	return EXIT_OK;
}
