/* SPDX-License-Identifier: GPL-2.0 */
#ifndef ARCH_TESTS_H
#define ARCH_TESTS_H

#include <linux/compiler.h>
struct test;

/* Tests */
int test__rdpmc(struct test *test __maybe_unused, int subtest);
int test__insn_x86(struct test *test __maybe_unused, int subtest);
int test__intel_pt_pkt_decoder(struct test *test, int subtest);
int test__bp_modify(struct test *test, int subtest);
int test__x86_sample_parsing(struct test *test, int subtest);

#ifdef HAVE_DWARF_UNWIND_SUPPORT
struct thread;
struct perf_sample;
int test__arch_unwind_sample(struct perf_sample *sample,
			     struct thread *thread);
#endif

extern struct test arch_tests[];

#endif
