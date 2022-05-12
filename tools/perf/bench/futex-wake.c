// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2013  Davidlohr Bueso <davidlohr@hp.com>
 *
 * futex-wake: Block a bunch of threads on a futex and wake'em up, N at a time.
 *
 * This program is particularly useful to measure the latency of nthread wakeups
 * in non-error situations:  all waiters are queued and all wake calls wakeup
 * one or more tasks, and thus the waitqueue is never empty.
 */

/* For the CLR_() macros */
#include <string.h>
#include <pthread.h>

#include <signal.h>
#include "../util/stat.h"
#include <subcmd/parse-options.h>
#include <linux/compiler.h>
#include <linux/kernel.h>
#include <linux/time64.h>
#include <errno.h>
#include <perf/cpumap.h>
#include "bench.h"
#include "futex.h"

#include <err.h>
#include <stdlib.h>
#include <sys/time.h>

/* all threads will block on the same futex */
static u_int32_t futex1 = 0;

/*
 * How many wakeups to do at a time.
 * Default to 1 in order to make the kernel work more.
 */
static unsigned int nwakes = 1;

pthread_t *worker;
static bool done = false, silent = false, fshared = false;
static pthread_mutex_t thread_lock;
static pthread_cond_t thread_parent, thread_worker;
static struct stats waketime_stats, wakeup_stats;
static unsigned int threads_starting, nthreads = 0;
static int futex_flag = 0;

static const struct option options[] = {
	OPT_UINTEGER('t', "threads", &nthreads, "Specify amount of threads"),
	OPT_UINTEGER('w', "nwakes",  &nwakes,   "Specify amount of threads to wake at once"),
	OPT_BOOLEAN( 's', "silent",  &silent,   "Silent mode: do not display data/details"),
	OPT_BOOLEAN( 'S', "shared",  &fshared,  "Use shared futexes instead of private ones"),
	OPT_END()
};

static const char * const bench_futex_wake_usage[] = {
	"perf bench futex wake <options>",
	NULL
};

static void *workerfn(void *arg __maybe_unused)
{
	pthread_mutex_lock(&thread_lock);
	threads_starting--;
	if (!threads_starting)
		pthread_cond_signal(&thread_parent);
	pthread_cond_wait(&thread_worker, &thread_lock);
	pthread_mutex_unlock(&thread_lock);

	while (1) {
		if (futex_wait(&futex1, 0, NULL, futex_flag) != EINTR)
			break;
	}

	pthread_exit(NULL);
	return NULL;
}

static void print_summary(void)
{
	double waketime_avg = avg_stats(&waketime_stats);
	double waketime_stddev = stddev_stats(&waketime_stats);
	unsigned int wakeup_avg = avg_stats(&wakeup_stats);

	printf("Wokeup %d of %d threads in %.4f ms (+-%.2f%%)\n",
	       wakeup_avg,
	       nthreads,
	       waketime_avg / USEC_PER_MSEC,
	       rel_stddev_stats(waketime_stddev, waketime_avg));
}

static void block_threads(pthread_t *w,
			  pthread_attr_t thread_attr, struct perf_cpu_map *cpu)
{
	cpu_set_t cpuset;
	unsigned int i;

	threads_starting = nthreads;

	/* create and block all threads */
	for (i = 0; i < nthreads; i++) {
		CPU_ZERO(&cpuset);
		CPU_SET(cpu->map[i % cpu->nr], &cpuset);

		if (pthread_attr_setaffinity_np(&thread_attr, sizeof(cpu_set_t), &cpuset))
			err(EXIT_FAILURE, "pthread_attr_setaffinity_np");

		if (pthread_create(&w[i], &thread_attr, workerfn, NULL))
			err(EXIT_FAILURE, "pthread_create");
	}
}

static void toggle_done(int sig __maybe_unused,
			siginfo_t *info __maybe_unused,
			void *uc __maybe_unused)
{
	done = true;
}

int bench_futex_wake(int argc, const char **argv)
{
	int ret = 0;
	unsigned int i, j;
	struct sigaction act;
	pthread_attr_t thread_attr;
	struct perf_cpu_map *cpu;

	argc = parse_options(argc, argv, options, bench_futex_wake_usage, 0);
	if (argc) {
		usage_with_options(bench_futex_wake_usage, options);
		exit(EXIT_FAILURE);
	}

	cpu = perf_cpu_map__new(NULL);
	if (!cpu)
		err(EXIT_FAILURE, "calloc");

	memset(&act, 0, sizeof(act));
	sigfillset(&act.sa_mask);
	act.sa_sigaction = toggle_done;
	sigaction(SIGINT, &act, NULL);

	if (!nthreads)
		nthreads = cpu->nr;

	worker = calloc(nthreads, sizeof(*worker));
	if (!worker)
		err(EXIT_FAILURE, "calloc");

	if (!fshared)
		futex_flag = FUTEX_PRIVATE_FLAG;

	printf("Run summary [PID %d]: blocking on %d threads (at [%s] futex %p), "
	       "waking up %d at a time.\n\n",
	       getpid(), nthreads, fshared ? "shared":"private",  &futex1, nwakes);

	init_stats(&wakeup_stats);
	init_stats(&waketime_stats);
	pthread_attr_init(&thread_attr);
	pthread_mutex_init(&thread_lock, NULL);
	pthread_cond_init(&thread_parent, NULL);
	pthread_cond_init(&thread_worker, NULL);

	for (j = 0; j < bench_repeat && !done; j++) {
		unsigned int nwoken = 0;
		struct timeval start, end, runtime;

		/* create, launch & block all threads */
		block_threads(worker, thread_attr, cpu);

		/* make sure all threads are already blocked */
		pthread_mutex_lock(&thread_lock);
		while (threads_starting)
			pthread_cond_wait(&thread_parent, &thread_lock);
		pthread_cond_broadcast(&thread_worker);
		pthread_mutex_unlock(&thread_lock);

		usleep(100000);

		/* Ok, all threads are patiently blocked, start waking folks up */
		gettimeofday(&start, NULL);
		while (nwoken != nthreads)
			nwoken += futex_wake(&futex1, nwakes, futex_flag);
		gettimeofday(&end, NULL);
		timersub(&end, &start, &runtime);

		update_stats(&wakeup_stats, nwoken);
		update_stats(&waketime_stats, runtime.tv_usec);

		if (!silent) {
			printf("[Run %d]: Wokeup %d of %d threads in %.4f ms\n",
			       j + 1, nwoken, nthreads, runtime.tv_usec / (double)USEC_PER_MSEC);
		}

		for (i = 0; i < nthreads; i++) {
			ret = pthread_join(worker[i], NULL);
			if (ret)
				err(EXIT_FAILURE, "pthread_join");
		}

	}

	/* cleanup & report results */
	pthread_cond_destroy(&thread_parent);
	pthread_cond_destroy(&thread_worker);
	pthread_mutex_destroy(&thread_lock);
	pthread_attr_destroy(&thread_attr);

	print_summary();

	free(worker);
	return ret;
}
