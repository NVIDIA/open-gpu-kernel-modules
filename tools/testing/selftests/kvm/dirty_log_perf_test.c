// SPDX-License-Identifier: GPL-2.0
/*
 * KVM dirty page logging performance test
 *
 * Based on dirty_log_test.c
 *
 * Copyright (C) 2018, Red Hat, Inc.
 * Copyright (C) 2020, Google, Inc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <linux/bitmap.h>

#include "kvm_util.h"
#include "test_util.h"
#include "perf_test_util.h"
#include "guest_modes.h"

/* How many host loops to run by default (one KVM_GET_DIRTY_LOG for each loop)*/
#define TEST_HOST_LOOP_N		2UL

static int nr_vcpus = 1;
static uint64_t guest_percpu_mem_size = DEFAULT_PER_VCPU_MEM_SIZE;

/* Host variables */
static u64 dirty_log_manual_caps;
static bool host_quit;
static int iteration;
static int vcpu_last_completed_iteration[KVM_MAX_VCPUS];

static void *vcpu_worker(void *data)
{
	int ret;
	struct kvm_vm *vm = perf_test_args.vm;
	uint64_t pages_count = 0;
	struct kvm_run *run;
	struct timespec start;
	struct timespec ts_diff;
	struct timespec total = (struct timespec){0};
	struct timespec avg;
	struct perf_test_vcpu_args *vcpu_args = (struct perf_test_vcpu_args *)data;
	int vcpu_id = vcpu_args->vcpu_id;

	vcpu_args_set(vm, vcpu_id, 1, vcpu_id);
	run = vcpu_state(vm, vcpu_id);

	while (!READ_ONCE(host_quit)) {
		int current_iteration = READ_ONCE(iteration);

		clock_gettime(CLOCK_MONOTONIC, &start);
		ret = _vcpu_run(vm, vcpu_id);
		ts_diff = timespec_elapsed(start);

		TEST_ASSERT(ret == 0, "vcpu_run failed: %d\n", ret);
		TEST_ASSERT(get_ucall(vm, vcpu_id, NULL) == UCALL_SYNC,
			    "Invalid guest sync status: exit_reason=%s\n",
			    exit_reason_str(run->exit_reason));

		pr_debug("Got sync event from vCPU %d\n", vcpu_id);
		vcpu_last_completed_iteration[vcpu_id] = current_iteration;
		pr_debug("vCPU %d updated last completed iteration to %d\n",
			 vcpu_id, vcpu_last_completed_iteration[vcpu_id]);

		if (current_iteration) {
			pages_count += vcpu_args->pages;
			total = timespec_add(total, ts_diff);
			pr_debug("vCPU %d iteration %d dirty memory time: %ld.%.9lds\n",
				vcpu_id, current_iteration, ts_diff.tv_sec,
				ts_diff.tv_nsec);
		} else {
			pr_debug("vCPU %d iteration %d populate memory time: %ld.%.9lds\n",
				vcpu_id, current_iteration, ts_diff.tv_sec,
				ts_diff.tv_nsec);
		}

		while (current_iteration == READ_ONCE(iteration) &&
		       !READ_ONCE(host_quit)) {}
	}

	avg = timespec_div(total, vcpu_last_completed_iteration[vcpu_id]);
	pr_debug("\nvCPU %d dirtied 0x%lx pages over %d iterations in %ld.%.9lds. (Avg %ld.%.9lds/iteration)\n",
		vcpu_id, pages_count, vcpu_last_completed_iteration[vcpu_id],
		total.tv_sec, total.tv_nsec, avg.tv_sec, avg.tv_nsec);

	return NULL;
}

struct test_params {
	unsigned long iterations;
	uint64_t phys_offset;
	int wr_fract;
	bool partition_vcpu_memory_access;
	enum vm_mem_backing_src_type backing_src;
};

static void run_test(enum vm_guest_mode mode, void *arg)
{
	struct test_params *p = arg;
	pthread_t *vcpu_threads;
	struct kvm_vm *vm;
	unsigned long *bmap;
	uint64_t guest_num_pages;
	uint64_t host_num_pages;
	int vcpu_id;
	struct timespec start;
	struct timespec ts_diff;
	struct timespec get_dirty_log_total = (struct timespec){0};
	struct timespec vcpu_dirty_total = (struct timespec){0};
	struct timespec avg;
	struct kvm_enable_cap cap = {};
	struct timespec clear_dirty_log_total = (struct timespec){0};

	vm = perf_test_create_vm(mode, nr_vcpus, guest_percpu_mem_size,
				 p->backing_src);

	perf_test_args.wr_fract = p->wr_fract;

	guest_num_pages = (nr_vcpus * guest_percpu_mem_size) >> vm_get_page_shift(vm);
	guest_num_pages = vm_adjust_num_guest_pages(mode, guest_num_pages);
	host_num_pages = vm_num_host_pages(mode, guest_num_pages);
	bmap = bitmap_alloc(host_num_pages);

	if (dirty_log_manual_caps) {
		cap.cap = KVM_CAP_MANUAL_DIRTY_LOG_PROTECT2;
		cap.args[0] = dirty_log_manual_caps;
		vm_enable_cap(vm, &cap);
	}

	vcpu_threads = malloc(nr_vcpus * sizeof(*vcpu_threads));
	TEST_ASSERT(vcpu_threads, "Memory allocation failed");

	perf_test_setup_vcpus(vm, nr_vcpus, guest_percpu_mem_size,
			      p->partition_vcpu_memory_access);

	sync_global_to_guest(vm, perf_test_args);

	/* Start the iterations */
	iteration = 0;
	host_quit = false;

	clock_gettime(CLOCK_MONOTONIC, &start);
	for (vcpu_id = 0; vcpu_id < nr_vcpus; vcpu_id++) {
		vcpu_last_completed_iteration[vcpu_id] = -1;

		pthread_create(&vcpu_threads[vcpu_id], NULL, vcpu_worker,
			       &perf_test_args.vcpu_args[vcpu_id]);
	}

	/* Allow the vCPUs to populate memory */
	pr_debug("Starting iteration %d - Populating\n", iteration);
	for (vcpu_id = 0; vcpu_id < nr_vcpus; vcpu_id++) {
		while (READ_ONCE(vcpu_last_completed_iteration[vcpu_id]) !=
		       iteration)
			;
	}

	ts_diff = timespec_elapsed(start);
	pr_info("Populate memory time: %ld.%.9lds\n",
		ts_diff.tv_sec, ts_diff.tv_nsec);

	/* Enable dirty logging */
	clock_gettime(CLOCK_MONOTONIC, &start);
	vm_mem_region_set_flags(vm, PERF_TEST_MEM_SLOT_INDEX,
				KVM_MEM_LOG_DIRTY_PAGES);
	ts_diff = timespec_elapsed(start);
	pr_info("Enabling dirty logging time: %ld.%.9lds\n\n",
		ts_diff.tv_sec, ts_diff.tv_nsec);

	while (iteration < p->iterations) {
		/*
		 * Incrementing the iteration number will start the vCPUs
		 * dirtying memory again.
		 */
		clock_gettime(CLOCK_MONOTONIC, &start);
		iteration++;

		pr_debug("Starting iteration %d\n", iteration);
		for (vcpu_id = 0; vcpu_id < nr_vcpus; vcpu_id++) {
			while (READ_ONCE(vcpu_last_completed_iteration[vcpu_id])
			       != iteration)
				;
		}

		ts_diff = timespec_elapsed(start);
		vcpu_dirty_total = timespec_add(vcpu_dirty_total, ts_diff);
		pr_info("Iteration %d dirty memory time: %ld.%.9lds\n",
			iteration, ts_diff.tv_sec, ts_diff.tv_nsec);

		clock_gettime(CLOCK_MONOTONIC, &start);
		kvm_vm_get_dirty_log(vm, PERF_TEST_MEM_SLOT_INDEX, bmap);

		ts_diff = timespec_elapsed(start);
		get_dirty_log_total = timespec_add(get_dirty_log_total,
						   ts_diff);
		pr_info("Iteration %d get dirty log time: %ld.%.9lds\n",
			iteration, ts_diff.tv_sec, ts_diff.tv_nsec);

		if (dirty_log_manual_caps) {
			clock_gettime(CLOCK_MONOTONIC, &start);
			kvm_vm_clear_dirty_log(vm, PERF_TEST_MEM_SLOT_INDEX, bmap, 0,
					       host_num_pages);

			ts_diff = timespec_elapsed(start);
			clear_dirty_log_total = timespec_add(clear_dirty_log_total,
							     ts_diff);
			pr_info("Iteration %d clear dirty log time: %ld.%.9lds\n",
				iteration, ts_diff.tv_sec, ts_diff.tv_nsec);
		}
	}

	/* Disable dirty logging */
	clock_gettime(CLOCK_MONOTONIC, &start);
	vm_mem_region_set_flags(vm, PERF_TEST_MEM_SLOT_INDEX, 0);
	ts_diff = timespec_elapsed(start);
	pr_info("Disabling dirty logging time: %ld.%.9lds\n",
		ts_diff.tv_sec, ts_diff.tv_nsec);

	/* Tell the vcpu thread to quit */
	host_quit = true;
	for (vcpu_id = 0; vcpu_id < nr_vcpus; vcpu_id++)
		pthread_join(vcpu_threads[vcpu_id], NULL);

	avg = timespec_div(get_dirty_log_total, p->iterations);
	pr_info("Get dirty log over %lu iterations took %ld.%.9lds. (Avg %ld.%.9lds/iteration)\n",
		p->iterations, get_dirty_log_total.tv_sec,
		get_dirty_log_total.tv_nsec, avg.tv_sec, avg.tv_nsec);

	if (dirty_log_manual_caps) {
		avg = timespec_div(clear_dirty_log_total, p->iterations);
		pr_info("Clear dirty log over %lu iterations took %ld.%.9lds. (Avg %ld.%.9lds/iteration)\n",
			p->iterations, clear_dirty_log_total.tv_sec,
			clear_dirty_log_total.tv_nsec, avg.tv_sec, avg.tv_nsec);
	}

	free(bmap);
	free(vcpu_threads);
	perf_test_destroy_vm(vm);
}

static void help(char *name)
{
	puts("");
	printf("usage: %s [-h] [-i iterations] [-p offset] "
	       "[-m mode] [-b vcpu bytes] [-v vcpus] [-o] [-s mem type]\n", name);
	puts("");
	printf(" -i: specify iteration counts (default: %"PRIu64")\n",
	       TEST_HOST_LOOP_N);
	printf(" -p: specify guest physical test memory offset\n"
	       "     Warning: a low offset can conflict with the loaded test code.\n");
	guest_modes_help();
	printf(" -b: specify the size of the memory region which should be\n"
	       "     dirtied by each vCPU. e.g. 10M or 3G.\n"
	       "     (default: 1G)\n");
	printf(" -f: specify the fraction of pages which should be written to\n"
	       "     as opposed to simply read, in the form\n"
	       "     1/<fraction of pages to write>.\n"
	       "     (default: 1 i.e. all pages are written to.)\n");
	printf(" -v: specify the number of vCPUs to run.\n");
	printf(" -o: Overlap guest memory accesses instead of partitioning\n"
	       "     them into a separate region of memory for each vCPU.\n");
	printf(" -s: specify the type of memory that should be used to\n"
	       "     back the guest data region.\n\n");
	backing_src_help();
	puts("");
	exit(0);
}

int main(int argc, char *argv[])
{
	int max_vcpus = kvm_check_cap(KVM_CAP_MAX_VCPUS);
	struct test_params p = {
		.iterations = TEST_HOST_LOOP_N,
		.wr_fract = 1,
		.partition_vcpu_memory_access = true,
		.backing_src = VM_MEM_SRC_ANONYMOUS,
	};
	int opt;

	dirty_log_manual_caps =
		kvm_check_cap(KVM_CAP_MANUAL_DIRTY_LOG_PROTECT2);
	dirty_log_manual_caps &= (KVM_DIRTY_LOG_MANUAL_PROTECT_ENABLE |
				  KVM_DIRTY_LOG_INITIALLY_SET);

	guest_modes_append_default();

	while ((opt = getopt(argc, argv, "hi:p:m:b:f:v:os:")) != -1) {
		switch (opt) {
		case 'i':
			p.iterations = atoi(optarg);
			break;
		case 'p':
			p.phys_offset = strtoull(optarg, NULL, 0);
			break;
		case 'm':
			guest_modes_cmdline(optarg);
			break;
		case 'b':
			guest_percpu_mem_size = parse_size(optarg);
			break;
		case 'f':
			p.wr_fract = atoi(optarg);
			TEST_ASSERT(p.wr_fract >= 1,
				    "Write fraction cannot be less than one");
			break;
		case 'v':
			nr_vcpus = atoi(optarg);
			TEST_ASSERT(nr_vcpus > 0 && nr_vcpus <= max_vcpus,
				    "Invalid number of vcpus, must be between 1 and %d", max_vcpus);
			break;
		case 'o':
			p.partition_vcpu_memory_access = false;
		case 's':
			p.backing_src = parse_backing_src_type(optarg);
			break;
		case 'h':
		default:
			help(argv[0]);
			break;
		}
	}

	TEST_ASSERT(p.iterations >= 2, "The test should have at least two iterations");

	pr_info("Test iterations: %"PRIu64"\n",	p.iterations);

	for_each_guest_mode(run_test, &p);

	return 0;
}
