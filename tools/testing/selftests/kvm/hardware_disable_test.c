// SPDX-License-Identifier: GPL-2.0-only
/*
 * This test is intended to reproduce a crash that happens when
 * kvm_arch_hardware_disable is called and it attempts to unregister the user
 * return notifiers.
 */

#define _GNU_SOURCE

#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#include <test_util.h>

#include "kvm_util.h"

#define VCPU_NUM 4
#define SLEEPING_THREAD_NUM (1 << 4)
#define FORK_NUM (1ULL << 9)
#define DELAY_US_MAX 2000
#define GUEST_CODE_PIO_PORT 4

sem_t *sem;

/* Arguments for the pthreads */
struct payload {
	struct kvm_vm *vm;
	uint32_t index;
};

static void guest_code(void)
{
	for (;;)
		;  /* Some busy work */
	printf("Should not be reached.\n");
}

static void *run_vcpu(void *arg)
{
	struct payload *payload = (struct payload *)arg;
	struct kvm_run *state = vcpu_state(payload->vm, payload->index);

	vcpu_run(payload->vm, payload->index);

	TEST_ASSERT(false, "%s: exited with reason %d: %s\n",
		    __func__, state->exit_reason,
		    exit_reason_str(state->exit_reason));
	pthread_exit(NULL);
}

static void *sleeping_thread(void *arg)
{
	int fd;

	while (true) {
		fd = open("/dev/null", O_RDWR);
		close(fd);
	}
	TEST_ASSERT(false, "%s: exited\n", __func__);
	pthread_exit(NULL);
}

static inline void check_create_thread(pthread_t *thread, pthread_attr_t *attr,
				       void *(*f)(void *), void *arg)
{
	int r;

	r = pthread_create(thread, attr, f, arg);
	TEST_ASSERT(r == 0, "%s: failed to create thread", __func__);
}

static inline void check_set_affinity(pthread_t thread, cpu_set_t *cpu_set)
{
	int r;

	r = pthread_setaffinity_np(thread, sizeof(cpu_set_t), cpu_set);
	TEST_ASSERT(r == 0, "%s: failed set affinity", __func__);
}

static inline void check_join(pthread_t thread, void **retval)
{
	int r;

	r = pthread_join(thread, retval);
	TEST_ASSERT(r == 0, "%s: failed to join thread", __func__);
}

static void run_test(uint32_t run)
{
	struct kvm_vm *vm;
	cpu_set_t cpu_set;
	pthread_t threads[VCPU_NUM];
	pthread_t throw_away;
	struct payload payloads[VCPU_NUM];
	void *b;
	uint32_t i, j;

	CPU_ZERO(&cpu_set);
	for (i = 0; i < VCPU_NUM; i++)
		CPU_SET(i, &cpu_set);

	vm = vm_create(VM_MODE_DEFAULT, DEFAULT_GUEST_PHY_PAGES, O_RDWR);
	kvm_vm_elf_load(vm, program_invocation_name, 0, 0);
	vm_create_irqchip(vm);

	pr_debug("%s: [%d] start vcpus\n", __func__, run);
	for (i = 0; i < VCPU_NUM; ++i) {
		vm_vcpu_add_default(vm, i, guest_code);
		payloads[i].vm = vm;
		payloads[i].index = i;

		check_create_thread(&threads[i], NULL, run_vcpu,
				    (void *)&payloads[i]);
		check_set_affinity(threads[i], &cpu_set);

		for (j = 0; j < SLEEPING_THREAD_NUM; ++j) {
			check_create_thread(&throw_away, NULL, sleeping_thread,
					    (void *)NULL);
			check_set_affinity(throw_away, &cpu_set);
		}
	}
	pr_debug("%s: [%d] all threads launched\n", __func__, run);
	sem_post(sem);
	for (i = 0; i < VCPU_NUM; ++i)
		check_join(threads[i], &b);
	/* Should not be reached */
	TEST_ASSERT(false, "%s: [%d] child escaped the ninja\n", __func__, run);
}

void wait_for_child_setup(pid_t pid)
{
	/*
	 * Wait for the child to post to the semaphore, but wake up periodically
	 * to check if the child exited prematurely.
	 */
	for (;;) {
		const struct timespec wait_period = { .tv_sec = 1 };
		int status;

		if (!sem_timedwait(sem, &wait_period))
			return;

		/* Child is still running, keep waiting. */
		if (pid != waitpid(pid, &status, WNOHANG))
			continue;

		/*
		 * Child is no longer running, which is not expected.
		 *
		 * If it exited with a non-zero status, we explicitly forward
		 * the child's status in case it exited with KSFT_SKIP.
		 */
		if (WIFEXITED(status))
			exit(WEXITSTATUS(status));
		else
			TEST_ASSERT(false, "Child exited unexpectedly");
	}
}

int main(int argc, char **argv)
{
	uint32_t i;
	int s, r;
	pid_t pid;

	sem = sem_open("vm_sem", O_CREAT | O_EXCL, 0644, 0);
	sem_unlink("vm_sem");

	for (i = 0; i < FORK_NUM; ++i) {
		pid = fork();
		TEST_ASSERT(pid >= 0, "%s: unable to fork", __func__);
		if (pid == 0)
			run_test(i); /* This function always exits */

		pr_debug("%s: [%d] waiting semaphore\n", __func__, i);
		wait_for_child_setup(pid);
		r = (rand() % DELAY_US_MAX) + 1;
		pr_debug("%s: [%d] waiting %dus\n", __func__, i, r);
		usleep(r);
		r = waitpid(pid, &s, WNOHANG);
		TEST_ASSERT(r != pid,
			    "%s: [%d] child exited unexpectedly status: [%d]",
			    __func__, i, s);
		pr_debug("%s: [%d] killing child\n", __func__, i);
		kill(pid, SIGKILL);
	}

	sem_destroy(sem);
	exit(0);
}
