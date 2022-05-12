// SPDX-License-Identifier: GPL-2.0
#include <stdbool.h>
#include <linux/err.h>
#include <linux/string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "evlist.h"
#include "evsel.h"
#include "thread_map.h"
#include "record.h"
#include "tests.h"
#include "debug.h"
#include "util/mmap.h"
#include <errno.h>
#include <perf/mmap.h>

#ifndef O_DIRECTORY
#define O_DIRECTORY    00200000
#endif
#ifndef AT_FDCWD
#define AT_FDCWD       -100
#endif

int test__syscall_openat_tp_fields(struct test *test __maybe_unused, int subtest __maybe_unused)
{
	struct record_opts opts = {
		.target = {
			.uid = UINT_MAX,
			.uses_mmap = true,
		},
		.no_buffering = true,
		.freq	      = 1,
		.mmap_pages   = 256,
		.raw_samples  = true,
	};
	const char *filename = "/etc/passwd";
	int flags = O_RDONLY | O_DIRECTORY;
	struct evlist *evlist = evlist__new();
	struct evsel *evsel;
	int err = -1, i, nr_events = 0, nr_polls = 0;
	char sbuf[STRERR_BUFSIZE];

	if (evlist == NULL) {
		pr_debug("%s: evlist__new\n", __func__);
		goto out;
	}

	evsel = evsel__newtp("syscalls", "sys_enter_openat");
	if (IS_ERR(evsel)) {
		pr_debug("%s: evsel__newtp\n", __func__);
		goto out_delete_evlist;
	}

	evlist__add(evlist, evsel);

	err = evlist__create_maps(evlist, &opts.target);
	if (err < 0) {
		pr_debug("%s: evlist__create_maps\n", __func__);
		goto out_delete_evlist;
	}

	evsel__config(evsel, &opts, NULL);

	perf_thread_map__set_pid(evlist->core.threads, 0, getpid());

	err = evlist__open(evlist);
	if (err < 0) {
		pr_debug("perf_evlist__open: %s\n",
			 str_error_r(errno, sbuf, sizeof(sbuf)));
		goto out_delete_evlist;
	}

	err = evlist__mmap(evlist, UINT_MAX);
	if (err < 0) {
		pr_debug("evlist__mmap: %s\n",
			 str_error_r(errno, sbuf, sizeof(sbuf)));
		goto out_delete_evlist;
	}

	evlist__enable(evlist);

	/*
	 * Generate the event:
	 */
	openat(AT_FDCWD, filename, flags);

	while (1) {
		int before = nr_events;

		for (i = 0; i < evlist->core.nr_mmaps; i++) {
			union perf_event *event;
			struct mmap *md;

			md = &evlist->mmap[i];
			if (perf_mmap__read_init(&md->core) < 0)
				continue;

			while ((event = perf_mmap__read_event(&md->core)) != NULL) {
				const u32 type = event->header.type;
				int tp_flags;
				struct perf_sample sample;

				++nr_events;

				if (type != PERF_RECORD_SAMPLE) {
					perf_mmap__consume(&md->core);
					continue;
				}

				err = evsel__parse_sample(evsel, event, &sample);
				if (err) {
					pr_debug("Can't parse sample, err = %d\n", err);
					goto out_delete_evlist;
				}

				tp_flags = evsel__intval(evsel, &sample, "flags");

				if (flags != tp_flags) {
					pr_debug("%s: Expected flags=%#x, got %#x\n",
						 __func__, flags, tp_flags);
					goto out_delete_evlist;
				}

				goto out_ok;
			}
			perf_mmap__read_done(&md->core);
		}

		if (nr_events == before)
			evlist__poll(evlist, 10);

		if (++nr_polls > 5) {
			pr_debug("%s: no events!\n", __func__);
			goto out_delete_evlist;
		}
	}
out_ok:
	err = 0;
out_delete_evlist:
	evlist__delete(evlist);
out:
	return err;
}
