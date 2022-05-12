// SPDX-License-Identifier: GPL-2.0
#include <errno.h>
#include <inttypes.h>
/* For the CPU_* macros */
#include <pthread.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <api/fs/fs.h>
#include <linux/err.h>
#include <linux/string.h>
#include <api/fs/tracing_path.h>
#include "evsel.h"
#include "tests.h"
#include "thread_map.h"
#include <perf/cpumap.h>
#include "debug.h"
#include "stat.h"
#include "util/counts.h"

int test__openat_syscall_event_on_all_cpus(struct test *test __maybe_unused, int subtest __maybe_unused)
{
	int err = -1, fd, cpu;
	struct perf_cpu_map *cpus;
	struct evsel *evsel;
	unsigned int nr_openat_calls = 111, i;
	cpu_set_t cpu_set;
	struct perf_thread_map *threads = thread_map__new(-1, getpid(), UINT_MAX);
	char sbuf[STRERR_BUFSIZE];
	char errbuf[BUFSIZ];

	if (threads == NULL) {
		pr_debug("thread_map__new\n");
		return -1;
	}

	cpus = perf_cpu_map__new(NULL);
	if (cpus == NULL) {
		pr_debug("perf_cpu_map__new\n");
		goto out_thread_map_delete;
	}

	CPU_ZERO(&cpu_set);

	evsel = evsel__newtp("syscalls", "sys_enter_openat");
	if (IS_ERR(evsel)) {
		tracing_path__strerror_open_tp(errno, errbuf, sizeof(errbuf), "syscalls", "sys_enter_openat");
		pr_debug("%s\n", errbuf);
		goto out_cpu_map_delete;
	}

	if (evsel__open(evsel, cpus, threads) < 0) {
		pr_debug("failed to open counter: %s, "
			 "tweak /proc/sys/kernel/perf_event_paranoid?\n",
			 str_error_r(errno, sbuf, sizeof(sbuf)));
		goto out_evsel_delete;
	}

	for (cpu = 0; cpu < cpus->nr; ++cpu) {
		unsigned int ncalls = nr_openat_calls + cpu;
		/*
		 * XXX eventually lift this restriction in a way that
		 * keeps perf building on older glibc installations
		 * without CPU_ALLOC. 1024 cpus in 2010 still seems
		 * a reasonable upper limit tho :-)
		 */
		if (cpus->map[cpu] >= CPU_SETSIZE) {
			pr_debug("Ignoring CPU %d\n", cpus->map[cpu]);
			continue;
		}

		CPU_SET(cpus->map[cpu], &cpu_set);
		if (sched_setaffinity(0, sizeof(cpu_set), &cpu_set) < 0) {
			pr_debug("sched_setaffinity() failed on CPU %d: %s ",
				 cpus->map[cpu],
				 str_error_r(errno, sbuf, sizeof(sbuf)));
			goto out_close_fd;
		}
		for (i = 0; i < ncalls; ++i) {
			fd = openat(0, "/etc/passwd", O_RDONLY);
			close(fd);
		}
		CPU_CLR(cpus->map[cpu], &cpu_set);
	}

	/*
	 * Here we need to explicitly preallocate the counts, as if
	 * we use the auto allocation it will allocate just for 1 cpu,
	 * as we start by cpu 0.
	 */
	if (evsel__alloc_counts(evsel, cpus->nr, 1) < 0) {
		pr_debug("evsel__alloc_counts(ncpus=%d)\n", cpus->nr);
		goto out_close_fd;
	}

	err = 0;

	for (cpu = 0; cpu < cpus->nr; ++cpu) {
		unsigned int expected;

		if (cpus->map[cpu] >= CPU_SETSIZE)
			continue;

		if (evsel__read_on_cpu(evsel, cpu, 0) < 0) {
			pr_debug("evsel__read_on_cpu\n");
			err = -1;
			break;
		}

		expected = nr_openat_calls + cpu;
		if (perf_counts(evsel->counts, cpu, 0)->val != expected) {
			pr_debug("evsel__read_on_cpu: expected to intercept %d calls on cpu %d, got %" PRIu64 "\n",
				 expected, cpus->map[cpu], perf_counts(evsel->counts, cpu, 0)->val);
			err = -1;
		}
	}

	evsel__free_counts(evsel);
out_close_fd:
	perf_evsel__close_fd(&evsel->core);
out_evsel_delete:
	evsel__delete(evsel);
out_cpu_map_delete:
	perf_cpu_map__put(cpus);
out_thread_map_delete:
	perf_thread_map__put(threads);
	return err;
}
