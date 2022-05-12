// SPDX-License-Identifier: GPL-2.0
/* Copyright (c) 2020 Facebook */
#define _GNU_SOURCE
#include <argp.h>
#include <linux/compiler.h>
#include <sys/time.h>
#include <sched.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/sysinfo.h>
#include <sys/resource.h>
#include <signal.h>
#include "bench.h"
#include "testing_helpers.h"

struct env env = {
	.warmup_sec = 1,
	.duration_sec = 5,
	.affinity = false,
	.consumer_cnt = 1,
	.producer_cnt = 1,
};

static int libbpf_print_fn(enum libbpf_print_level level,
		    const char *format, va_list args)
{
	if (level == LIBBPF_DEBUG && !env.verbose)
		return 0;
	return vfprintf(stderr, format, args);
}

static int bump_memlock_rlimit(void)
{
	struct rlimit rlim_new = {
		.rlim_cur	= RLIM_INFINITY,
		.rlim_max	= RLIM_INFINITY,
	};

	return setrlimit(RLIMIT_MEMLOCK, &rlim_new);
}

void setup_libbpf()
{
	int err;

	libbpf_set_print(libbpf_print_fn);

	err = bump_memlock_rlimit();
	if (err)
		fprintf(stderr, "failed to increase RLIMIT_MEMLOCK: %d", err);
}

void hits_drops_report_progress(int iter, struct bench_res *res, long delta_ns)
{
	double hits_per_sec, drops_per_sec;
	double hits_per_prod;

	hits_per_sec = res->hits / 1000000.0 / (delta_ns / 1000000000.0);
	hits_per_prod = hits_per_sec / env.producer_cnt;
	drops_per_sec = res->drops / 1000000.0 / (delta_ns / 1000000000.0);

	printf("Iter %3d (%7.3lfus): ",
	       iter, (delta_ns - 1000000000) / 1000.0);

	printf("hits %8.3lfM/s (%7.3lfM/prod), drops %8.3lfM/s\n",
	       hits_per_sec, hits_per_prod, drops_per_sec);
}

void hits_drops_report_final(struct bench_res res[], int res_cnt)
{
	int i;
	double hits_mean = 0.0, drops_mean = 0.0;
	double hits_stddev = 0.0, drops_stddev = 0.0;

	for (i = 0; i < res_cnt; i++) {
		hits_mean += res[i].hits / 1000000.0 / (0.0 + res_cnt);
		drops_mean += res[i].drops / 1000000.0 / (0.0 + res_cnt);
	}

	if (res_cnt > 1)  {
		for (i = 0; i < res_cnt; i++) {
			hits_stddev += (hits_mean - res[i].hits / 1000000.0) *
				       (hits_mean - res[i].hits / 1000000.0) /
				       (res_cnt - 1.0);
			drops_stddev += (drops_mean - res[i].drops / 1000000.0) *
					(drops_mean - res[i].drops / 1000000.0) /
					(res_cnt - 1.0);
		}
		hits_stddev = sqrt(hits_stddev);
		drops_stddev = sqrt(drops_stddev);
	}
	printf("Summary: hits %8.3lf \u00B1 %5.3lfM/s (%7.3lfM/prod), ",
	       hits_mean, hits_stddev, hits_mean / env.producer_cnt);
	printf("drops %8.3lf \u00B1 %5.3lfM/s\n",
	       drops_mean, drops_stddev);
}

const char *argp_program_version = "benchmark";
const char *argp_program_bug_address = "<bpf@vger.kernel.org>";
const char argp_program_doc[] =
"benchmark    Generic benchmarking framework.\n"
"\n"
"This tool runs benchmarks.\n"
"\n"
"USAGE: benchmark <bench-name>\n"
"\n"
"EXAMPLES:\n"
"    # run 'count-local' benchmark with 1 producer and 1 consumer\n"
"    benchmark count-local\n"
"    # run 'count-local' with 16 producer and 8 consumer thread, pinned to CPUs\n"
"    benchmark -p16 -c8 -a count-local\n";

enum {
	ARG_PROD_AFFINITY_SET = 1000,
	ARG_CONS_AFFINITY_SET = 1001,
};

static const struct argp_option opts[] = {
	{ "list", 'l', NULL, 0, "List available benchmarks"},
	{ "duration", 'd', "SEC", 0, "Duration of benchmark, seconds"},
	{ "warmup", 'w', "SEC", 0, "Warm-up period, seconds"},
	{ "producers", 'p', "NUM", 0, "Number of producer threads"},
	{ "consumers", 'c', "NUM", 0, "Number of consumer threads"},
	{ "verbose", 'v', NULL, 0, "Verbose debug output"},
	{ "affinity", 'a', NULL, 0, "Set consumer/producer thread affinity"},
	{ "prod-affinity", ARG_PROD_AFFINITY_SET, "CPUSET", 0,
	  "Set of CPUs for producer threads; implies --affinity"},
	{ "cons-affinity", ARG_CONS_AFFINITY_SET, "CPUSET", 0,
	  "Set of CPUs for consumer threads; implies --affinity"},
	{},
};

extern struct argp bench_ringbufs_argp;

static const struct argp_child bench_parsers[] = {
	{ &bench_ringbufs_argp, 0, "Ring buffers benchmark", 0 },
	{},
};

static error_t parse_arg(int key, char *arg, struct argp_state *state)
{
	static int pos_args;

	switch (key) {
	case 'v':
		env.verbose = true;
		break;
	case 'l':
		env.list = true;
		break;
	case 'd':
		env.duration_sec = strtol(arg, NULL, 10);
		if (env.duration_sec <= 0) {
			fprintf(stderr, "Invalid duration: %s\n", arg);
			argp_usage(state);
		}
		break;
	case 'w':
		env.warmup_sec = strtol(arg, NULL, 10);
		if (env.warmup_sec <= 0) {
			fprintf(stderr, "Invalid warm-up duration: %s\n", arg);
			argp_usage(state);
		}
		break;
	case 'p':
		env.producer_cnt = strtol(arg, NULL, 10);
		if (env.producer_cnt <= 0) {
			fprintf(stderr, "Invalid producer count: %s\n", arg);
			argp_usage(state);
		}
		break;
	case 'c':
		env.consumer_cnt = strtol(arg, NULL, 10);
		if (env.consumer_cnt <= 0) {
			fprintf(stderr, "Invalid consumer count: %s\n", arg);
			argp_usage(state);
		}
		break;
	case 'a':
		env.affinity = true;
		break;
	case ARG_PROD_AFFINITY_SET:
		env.affinity = true;
		if (parse_num_list(arg, &env.prod_cpus.cpus,
				   &env.prod_cpus.cpus_len)) {
			fprintf(stderr, "Invalid format of CPU set for producers.");
			argp_usage(state);
		}
		break;
	case ARG_CONS_AFFINITY_SET:
		env.affinity = true;
		if (parse_num_list(arg, &env.cons_cpus.cpus,
				   &env.cons_cpus.cpus_len)) {
			fprintf(stderr, "Invalid format of CPU set for consumers.");
			argp_usage(state);
		}
		break;
	case ARGP_KEY_ARG:
		if (pos_args++) {
			fprintf(stderr,
				"Unrecognized positional argument: %s\n", arg);
			argp_usage(state);
		}
		env.bench_name = strdup(arg);
		break;
	default:
		return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

static void parse_cmdline_args(int argc, char **argv)
{
	static const struct argp argp = {
		.options = opts,
		.parser = parse_arg,
		.doc = argp_program_doc,
		.children = bench_parsers,
	};
	if (argp_parse(&argp, argc, argv, 0, NULL, NULL))
		exit(1);
	if (!env.list && !env.bench_name) {
		argp_help(&argp, stderr, ARGP_HELP_DOC, "bench");
		exit(1);
	}
}

static void collect_measurements(long delta_ns);

static __u64 last_time_ns;
static void sigalarm_handler(int signo)
{
	long new_time_ns = get_time_ns();
	long delta_ns = new_time_ns - last_time_ns;

	collect_measurements(delta_ns);

	last_time_ns = new_time_ns;
}

/* set up periodic 1-second timer */
static void setup_timer()
{
	static struct sigaction sigalarm_action = {
		.sa_handler = sigalarm_handler,
	};
	struct itimerval timer_settings = {};
	int err;

	last_time_ns = get_time_ns();
	err = sigaction(SIGALRM, &sigalarm_action, NULL);
	if (err < 0) {
		fprintf(stderr, "failed to install SIGALRM handler: %d\n", -errno);
		exit(1);
	}
	timer_settings.it_interval.tv_sec = 1;
	timer_settings.it_value.tv_sec = 1;
	err = setitimer(ITIMER_REAL, &timer_settings, NULL);
	if (err < 0) {
		fprintf(stderr, "failed to arm interval timer: %d\n", -errno);
		exit(1);
	}
}

static void set_thread_affinity(pthread_t thread, int cpu)
{
	cpu_set_t cpuset;

	CPU_ZERO(&cpuset);
	CPU_SET(cpu, &cpuset);
	if (pthread_setaffinity_np(thread, sizeof(cpuset), &cpuset)) {
		fprintf(stderr, "setting affinity to CPU #%d failed: %d\n",
			cpu, errno);
		exit(1);
	}
}

static int next_cpu(struct cpu_set *cpu_set)
{
	if (cpu_set->cpus) {
		int i;

		/* find next available CPU */
		for (i = cpu_set->next_cpu; i < cpu_set->cpus_len; i++) {
			if (cpu_set->cpus[i]) {
				cpu_set->next_cpu = i + 1;
				return i;
			}
		}
		fprintf(stderr, "Not enough CPUs specified, need CPU #%d or higher.\n", i);
		exit(1);
	}

	return cpu_set->next_cpu++;
}

static struct bench_state {
	int res_cnt;
	struct bench_res *results;
	pthread_t *consumers;
	pthread_t *producers;
} state;

const struct bench *bench = NULL;

extern const struct bench bench_count_global;
extern const struct bench bench_count_local;
extern const struct bench bench_rename_base;
extern const struct bench bench_rename_kprobe;
extern const struct bench bench_rename_kretprobe;
extern const struct bench bench_rename_rawtp;
extern const struct bench bench_rename_fentry;
extern const struct bench bench_rename_fexit;
extern const struct bench bench_trig_base;
extern const struct bench bench_trig_tp;
extern const struct bench bench_trig_rawtp;
extern const struct bench bench_trig_kprobe;
extern const struct bench bench_trig_fentry;
extern const struct bench bench_trig_fentry_sleep;
extern const struct bench bench_trig_fmodret;
extern const struct bench bench_rb_libbpf;
extern const struct bench bench_rb_custom;
extern const struct bench bench_pb_libbpf;
extern const struct bench bench_pb_custom;

static const struct bench *benchs[] = {
	&bench_count_global,
	&bench_count_local,
	&bench_rename_base,
	&bench_rename_kprobe,
	&bench_rename_kretprobe,
	&bench_rename_rawtp,
	&bench_rename_fentry,
	&bench_rename_fexit,
	&bench_trig_base,
	&bench_trig_tp,
	&bench_trig_rawtp,
	&bench_trig_kprobe,
	&bench_trig_fentry,
	&bench_trig_fentry_sleep,
	&bench_trig_fmodret,
	&bench_rb_libbpf,
	&bench_rb_custom,
	&bench_pb_libbpf,
	&bench_pb_custom,
};

static void setup_benchmark()
{
	int i, err;

	if (!env.bench_name) {
		fprintf(stderr, "benchmark name is not specified\n");
		exit(1);
	}

	for (i = 0; i < ARRAY_SIZE(benchs); i++) {
		if (strcmp(benchs[i]->name, env.bench_name) == 0) {
			bench = benchs[i];
			break;
		}
	}
	if (!bench) {
		fprintf(stderr, "benchmark '%s' not found\n", env.bench_name);
		exit(1);
	}

	printf("Setting up benchmark '%s'...\n", bench->name);

	state.producers = calloc(env.producer_cnt, sizeof(*state.producers));
	state.consumers = calloc(env.consumer_cnt, sizeof(*state.consumers));
	state.results = calloc(env.duration_sec + env.warmup_sec + 2,
			       sizeof(*state.results));
	if (!state.producers || !state.consumers || !state.results)
		exit(1);

	if (bench->validate)
		bench->validate();
	if (bench->setup)
		bench->setup();

	for (i = 0; i < env.consumer_cnt; i++) {
		err = pthread_create(&state.consumers[i], NULL,
				     bench->consumer_thread, (void *)(long)i);
		if (err) {
			fprintf(stderr, "failed to create consumer thread #%d: %d\n",
				i, -errno);
			exit(1);
		}
		if (env.affinity)
			set_thread_affinity(state.consumers[i],
					    next_cpu(&env.cons_cpus));
	}

	/* unless explicit producer CPU list is specified, continue after
	 * last consumer CPU
	 */
	if (!env.prod_cpus.cpus)
		env.prod_cpus.next_cpu = env.cons_cpus.next_cpu;

	for (i = 0; i < env.producer_cnt; i++) {
		err = pthread_create(&state.producers[i], NULL,
				     bench->producer_thread, (void *)(long)i);
		if (err) {
			fprintf(stderr, "failed to create producer thread #%d: %d\n",
				i, -errno);
			exit(1);
		}
		if (env.affinity)
			set_thread_affinity(state.producers[i],
					    next_cpu(&env.prod_cpus));
	}

	printf("Benchmark '%s' started.\n", bench->name);
}

static pthread_mutex_t bench_done_mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t bench_done = PTHREAD_COND_INITIALIZER;

static void collect_measurements(long delta_ns) {
	int iter = state.res_cnt++;
	struct bench_res *res = &state.results[iter];

	bench->measure(res);

	if (bench->report_progress)
		bench->report_progress(iter, res, delta_ns);

	if (iter == env.duration_sec + env.warmup_sec) {
		pthread_mutex_lock(&bench_done_mtx);
		pthread_cond_signal(&bench_done);
		pthread_mutex_unlock(&bench_done_mtx);
	}
}

int main(int argc, char **argv)
{
	parse_cmdline_args(argc, argv);

	if (env.list) {
		int i;

		printf("Available benchmarks:\n");
		for (i = 0; i < ARRAY_SIZE(benchs); i++) {
			printf("- %s\n", benchs[i]->name);
		}
		return 0;
	}

	setup_benchmark();

	setup_timer();

	pthread_mutex_lock(&bench_done_mtx);
	pthread_cond_wait(&bench_done, &bench_done_mtx);
	pthread_mutex_unlock(&bench_done_mtx);

	if (bench->report_final)
		/* skip first sample */
		bench->report_final(state.results + env.warmup_sec,
				    state.res_cnt - env.warmup_sec);

	return 0;
}
