/* SPDX-License-Identifier: GPL-2.0 */

#define _GNU_SOURCE

#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <dlfcn.h>
#include <string.h>
#include <inttypes.h>
#include <signal.h>
#include <sys/ucontext.h>
#include <errno.h>
#include <err.h>
#include <sched.h>
#include <stdbool.h>
#include <setjmp.h>
#include <sys/uio.h>

#include "helpers.h"

#ifdef __x86_64__
# define VSYS(x) (x)
#else
# define VSYS(x) 0
#endif

#ifndef SYS_getcpu
# ifdef __x86_64__
#  define SYS_getcpu 309
# else
#  define SYS_getcpu 318
# endif
#endif

/* max length of lines in /proc/self/maps - anything longer is skipped here */
#define MAPS_LINE_LEN 128

static void sethandler(int sig, void (*handler)(int, siginfo_t *, void *),
		       int flags)
{
	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_sigaction = handler;
	sa.sa_flags = SA_SIGINFO | flags;
	sigemptyset(&sa.sa_mask);
	if (sigaction(sig, &sa, 0))
		err(1, "sigaction");
}

/* vsyscalls and vDSO */
bool vsyscall_map_r = false, vsyscall_map_x = false;

typedef long (*gtod_t)(struct timeval *tv, struct timezone *tz);
const gtod_t vgtod = (gtod_t)VSYS(0xffffffffff600000);
gtod_t vdso_gtod;

typedef int (*vgettime_t)(clockid_t, struct timespec *);
vgettime_t vdso_gettime;

typedef long (*time_func_t)(time_t *t);
const time_func_t vtime = (time_func_t)VSYS(0xffffffffff600400);
time_func_t vdso_time;

typedef long (*getcpu_t)(unsigned *, unsigned *, void *);
const getcpu_t vgetcpu = (getcpu_t)VSYS(0xffffffffff600800);
getcpu_t vdso_getcpu;

static void init_vdso(void)
{
	void *vdso = dlopen("linux-vdso.so.1", RTLD_LAZY | RTLD_LOCAL | RTLD_NOLOAD);
	if (!vdso)
		vdso = dlopen("linux-gate.so.1", RTLD_LAZY | RTLD_LOCAL | RTLD_NOLOAD);
	if (!vdso) {
		printf("[WARN]\tfailed to find vDSO\n");
		return;
	}

	vdso_gtod = (gtod_t)dlsym(vdso, "__vdso_gettimeofday");
	if (!vdso_gtod)
		printf("[WARN]\tfailed to find gettimeofday in vDSO\n");

	vdso_gettime = (vgettime_t)dlsym(vdso, "__vdso_clock_gettime");
	if (!vdso_gettime)
		printf("[WARN]\tfailed to find clock_gettime in vDSO\n");

	vdso_time = (time_func_t)dlsym(vdso, "__vdso_time");
	if (!vdso_time)
		printf("[WARN]\tfailed to find time in vDSO\n");

	vdso_getcpu = (getcpu_t)dlsym(vdso, "__vdso_getcpu");
	if (!vdso_getcpu) {
		/* getcpu() was never wired up in the 32-bit vDSO. */
		printf("[%s]\tfailed to find getcpu in vDSO\n",
		       sizeof(long) == 8 ? "WARN" : "NOTE");
	}
}

static int init_vsys(void)
{
#ifdef __x86_64__
	int nerrs = 0;
	FILE *maps;
	char line[MAPS_LINE_LEN];
	bool found = false;

	maps = fopen("/proc/self/maps", "r");
	if (!maps) {
		printf("[WARN]\tCould not open /proc/self/maps -- assuming vsyscall is r-x\n");
		vsyscall_map_r = true;
		return 0;
	}

	while (fgets(line, MAPS_LINE_LEN, maps)) {
		char r, x;
		void *start, *end;
		char name[MAPS_LINE_LEN];

		/* sscanf() is safe here as strlen(name) >= strlen(line) */
		if (sscanf(line, "%p-%p %c-%cp %*x %*x:%*x %*u %s",
			   &start, &end, &r, &x, name) != 5)
			continue;

		if (strcmp(name, "[vsyscall]"))
			continue;

		printf("\tvsyscall map: %s", line);

		if (start != (void *)0xffffffffff600000 ||
		    end != (void *)0xffffffffff601000) {
			printf("[FAIL]\taddress range is nonsense\n");
			nerrs++;
		}

		printf("\tvsyscall permissions are %c-%c\n", r, x);
		vsyscall_map_r = (r == 'r');
		vsyscall_map_x = (x == 'x');

		found = true;
		break;
	}

	fclose(maps);

	if (!found) {
		printf("\tno vsyscall map in /proc/self/maps\n");
		vsyscall_map_r = false;
		vsyscall_map_x = false;
	}

	return nerrs;
#else
	return 0;
#endif
}

/* syscalls */
static inline long sys_gtod(struct timeval *tv, struct timezone *tz)
{
	return syscall(SYS_gettimeofday, tv, tz);
}

static inline int sys_clock_gettime(clockid_t id, struct timespec *ts)
{
	return syscall(SYS_clock_gettime, id, ts);
}

static inline long sys_time(time_t *t)
{
	return syscall(SYS_time, t);
}

static inline long sys_getcpu(unsigned * cpu, unsigned * node,
			      void* cache)
{
	return syscall(SYS_getcpu, cpu, node, cache);
}

static jmp_buf jmpbuf;
static volatile unsigned long segv_err;

static void sigsegv(int sig, siginfo_t *info, void *ctx_void)
{
	ucontext_t *ctx = (ucontext_t *)ctx_void;

	segv_err =  ctx->uc_mcontext.gregs[REG_ERR];
	siglongjmp(jmpbuf, 1);
}

static double tv_diff(const struct timeval *a, const struct timeval *b)
{
	return (double)(a->tv_sec - b->tv_sec) +
		(double)((int)a->tv_usec - (int)b->tv_usec) * 1e-6;
}

static int check_gtod(const struct timeval *tv_sys1,
		      const struct timeval *tv_sys2,
		      const struct timezone *tz_sys,
		      const char *which,
		      const struct timeval *tv_other,
		      const struct timezone *tz_other)
{
	int nerrs = 0;
	double d1, d2;

	if (tz_other && (tz_sys->tz_minuteswest != tz_other->tz_minuteswest || tz_sys->tz_dsttime != tz_other->tz_dsttime)) {
		printf("[FAIL] %s tz mismatch\n", which);
		nerrs++;
	}

	d1 = tv_diff(tv_other, tv_sys1);
	d2 = tv_diff(tv_sys2, tv_other); 
	printf("\t%s time offsets: %lf %lf\n", which, d1, d2);

	if (d1 < 0 || d2 < 0) {
		printf("[FAIL]\t%s time was inconsistent with the syscall\n", which);
		nerrs++;
	} else {
		printf("[OK]\t%s gettimeofday()'s timeval was okay\n", which);
	}

	return nerrs;
}

static int test_gtod(void)
{
	struct timeval tv_sys1, tv_sys2, tv_vdso, tv_vsys;
	struct timezone tz_sys, tz_vdso, tz_vsys;
	long ret_vdso = -1;
	long ret_vsys = -1;
	int nerrs = 0;

	printf("[RUN]\ttest gettimeofday()\n");

	if (sys_gtod(&tv_sys1, &tz_sys) != 0)
		err(1, "syscall gettimeofday");
	if (vdso_gtod)
		ret_vdso = vdso_gtod(&tv_vdso, &tz_vdso);
	if (vsyscall_map_x)
		ret_vsys = vgtod(&tv_vsys, &tz_vsys);
	if (sys_gtod(&tv_sys2, &tz_sys) != 0)
		err(1, "syscall gettimeofday");

	if (vdso_gtod) {
		if (ret_vdso == 0) {
			nerrs += check_gtod(&tv_sys1, &tv_sys2, &tz_sys, "vDSO", &tv_vdso, &tz_vdso);
		} else {
			printf("[FAIL]\tvDSO gettimeofday() failed: %ld\n", ret_vdso);
			nerrs++;
		}
	}

	if (vsyscall_map_x) {
		if (ret_vsys == 0) {
			nerrs += check_gtod(&tv_sys1, &tv_sys2, &tz_sys, "vsyscall", &tv_vsys, &tz_vsys);
		} else {
			printf("[FAIL]\tvsys gettimeofday() failed: %ld\n", ret_vsys);
			nerrs++;
		}
	}

	return nerrs;
}

static int test_time(void) {
	int nerrs = 0;

	printf("[RUN]\ttest time()\n");
	long t_sys1, t_sys2, t_vdso = 0, t_vsys = 0;
	long t2_sys1 = -1, t2_sys2 = -1, t2_vdso = -1, t2_vsys = -1;
	t_sys1 = sys_time(&t2_sys1);
	if (vdso_time)
		t_vdso = vdso_time(&t2_vdso);
	if (vsyscall_map_x)
		t_vsys = vtime(&t2_vsys);
	t_sys2 = sys_time(&t2_sys2);
	if (t_sys1 < 0 || t_sys1 != t2_sys1 || t_sys2 < 0 || t_sys2 != t2_sys2) {
		printf("[FAIL]\tsyscall failed (ret1:%ld output1:%ld ret2:%ld output2:%ld)\n", t_sys1, t2_sys1, t_sys2, t2_sys2);
		nerrs++;
		return nerrs;
	}

	if (vdso_time) {
		if (t_vdso < 0 || t_vdso != t2_vdso) {
			printf("[FAIL]\tvDSO failed (ret:%ld output:%ld)\n", t_vdso, t2_vdso);
			nerrs++;
		} else if (t_vdso < t_sys1 || t_vdso > t_sys2) {
			printf("[FAIL]\tvDSO returned the wrong time (%ld %ld %ld)\n", t_sys1, t_vdso, t_sys2);
			nerrs++;
		} else {
			printf("[OK]\tvDSO time() is okay\n");
		}
	}

	if (vsyscall_map_x) {
		if (t_vsys < 0 || t_vsys != t2_vsys) {
			printf("[FAIL]\tvsyscall failed (ret:%ld output:%ld)\n", t_vsys, t2_vsys);
			nerrs++;
		} else if (t_vsys < t_sys1 || t_vsys > t_sys2) {
			printf("[FAIL]\tvsyscall returned the wrong time (%ld %ld %ld)\n", t_sys1, t_vsys, t_sys2);
			nerrs++;
		} else {
			printf("[OK]\tvsyscall time() is okay\n");
		}
	}

	return nerrs;
}

static int test_getcpu(int cpu)
{
	int nerrs = 0;
	long ret_sys, ret_vdso = -1, ret_vsys = -1;

	printf("[RUN]\tgetcpu() on CPU %d\n", cpu);

	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);
	CPU_SET(cpu, &cpuset);
	if (sched_setaffinity(0, sizeof(cpuset), &cpuset) != 0) {
		printf("[SKIP]\tfailed to force CPU %d\n", cpu);
		return nerrs;
	}

	unsigned cpu_sys, cpu_vdso, cpu_vsys, node_sys, node_vdso, node_vsys;
	unsigned node = 0;
	bool have_node = false;
	ret_sys = sys_getcpu(&cpu_sys, &node_sys, 0);
	if (vdso_getcpu)
		ret_vdso = vdso_getcpu(&cpu_vdso, &node_vdso, 0);
	if (vsyscall_map_x)
		ret_vsys = vgetcpu(&cpu_vsys, &node_vsys, 0);

	if (ret_sys == 0) {
		if (cpu_sys != cpu) {
			printf("[FAIL]\tsyscall reported CPU %hu but should be %d\n", cpu_sys, cpu);
			nerrs++;
		}

		have_node = true;
		node = node_sys;
	}

	if (vdso_getcpu) {
		if (ret_vdso) {
			printf("[FAIL]\tvDSO getcpu() failed\n");
			nerrs++;
		} else {
			if (!have_node) {
				have_node = true;
				node = node_vdso;
			}

			if (cpu_vdso != cpu) {
				printf("[FAIL]\tvDSO reported CPU %hu but should be %d\n", cpu_vdso, cpu);
				nerrs++;
			} else {
				printf("[OK]\tvDSO reported correct CPU\n");
			}

			if (node_vdso != node) {
				printf("[FAIL]\tvDSO reported node %hu but should be %hu\n", node_vdso, node);
				nerrs++;
			} else {
				printf("[OK]\tvDSO reported correct node\n");
			}
		}
	}

	if (vsyscall_map_x) {
		if (ret_vsys) {
			printf("[FAIL]\tvsyscall getcpu() failed\n");
			nerrs++;
		} else {
			if (!have_node) {
				have_node = true;
				node = node_vsys;
			}

			if (cpu_vsys != cpu) {
				printf("[FAIL]\tvsyscall reported CPU %hu but should be %d\n", cpu_vsys, cpu);
				nerrs++;
			} else {
				printf("[OK]\tvsyscall reported correct CPU\n");
			}

			if (node_vsys != node) {
				printf("[FAIL]\tvsyscall reported node %hu but should be %hu\n", node_vsys, node);
				nerrs++;
			} else {
				printf("[OK]\tvsyscall reported correct node\n");
			}
		}
	}

	return nerrs;
}

static int test_vsys_r(void)
{
#ifdef __x86_64__
	printf("[RUN]\tChecking read access to the vsyscall page\n");
	bool can_read;
	if (sigsetjmp(jmpbuf, 1) == 0) {
		*(volatile int *)0xffffffffff600000;
		can_read = true;
	} else {
		can_read = false;
	}

	if (can_read && !vsyscall_map_r) {
		printf("[FAIL]\tWe have read access, but we shouldn't\n");
		return 1;
	} else if (!can_read && vsyscall_map_r) {
		printf("[FAIL]\tWe don't have read access, but we should\n");
		return 1;
	} else if (can_read) {
		printf("[OK]\tWe have read access\n");
	} else {
		printf("[OK]\tWe do not have read access: #PF(0x%lx)\n",
		       segv_err);
	}
#endif

	return 0;
}

static int test_vsys_x(void)
{
#ifdef __x86_64__
	if (vsyscall_map_x) {
		/* We already tested this adequately. */
		return 0;
	}

	printf("[RUN]\tMake sure that vsyscalls really page fault\n");

	bool can_exec;
	if (sigsetjmp(jmpbuf, 1) == 0) {
		vgtod(NULL, NULL);
		can_exec = true;
	} else {
		can_exec = false;
	}

	if (can_exec) {
		printf("[FAIL]\tExecuting the vsyscall did not page fault\n");
		return 1;
	} else if (segv_err & (1 << 4)) { /* INSTR */
		printf("[OK]\tExecuting the vsyscall page failed: #PF(0x%lx)\n",
		       segv_err);
	} else {
		printf("[FAIL]\tExecution failed with the wrong error: #PF(0x%lx)\n",
		       segv_err);
		return 1;
	}
#endif

	return 0;
}

/*
 * Debuggers expect ptrace() to be able to peek at the vsyscall page.
 * Use process_vm_readv() as a proxy for ptrace() to test this.  We
 * want it to work in the vsyscall=emulate case and to fail in the
 * vsyscall=xonly case.
 *
 * It's worth noting that this ABI is a bit nutty.  write(2) can't
 * read from the vsyscall page on any kernel version or mode.  The
 * fact that ptrace() ever worked was a nice courtesy of old kernels,
 * but the code to support it is fairly gross.
 */
static int test_process_vm_readv(void)
{
#ifdef __x86_64__
	char buf[4096];
	struct iovec local, remote;
	int ret;

	printf("[RUN]\tprocess_vm_readv() from vsyscall page\n");

	local.iov_base = buf;
	local.iov_len = 4096;
	remote.iov_base = (void *)0xffffffffff600000;
	remote.iov_len = 4096;
	ret = process_vm_readv(getpid(), &local, 1, &remote, 1, 0);
	if (ret != 4096) {
		/*
		 * We expect process_vm_readv() to work if and only if the
		 * vsyscall page is readable.
		 */
		printf("[%s]\tprocess_vm_readv() failed (ret = %d, errno = %d)\n", vsyscall_map_r ? "FAIL" : "OK", ret, errno);
		return vsyscall_map_r ? 1 : 0;
	}

	if (vsyscall_map_r) {
		if (!memcmp(buf, (const void *)0xffffffffff600000, 4096)) {
			printf("[OK]\tIt worked and read correct data\n");
		} else {
			printf("[FAIL]\tIt worked but returned incorrect data\n");
			return 1;
		}
	} else {
		printf("[FAIL]\tprocess_rm_readv() succeeded, but it should have failed in this configuration\n");
		return 1;
	}
#endif

	return 0;
}

#ifdef __x86_64__
static volatile sig_atomic_t num_vsyscall_traps;

static void sigtrap(int sig, siginfo_t *info, void *ctx_void)
{
	ucontext_t *ctx = (ucontext_t *)ctx_void;
	unsigned long ip = ctx->uc_mcontext.gregs[REG_RIP];

	if (((ip ^ 0xffffffffff600000UL) & ~0xfffUL) == 0)
		num_vsyscall_traps++;
}

static int test_emulation(void)
{
	time_t tmp;
	bool is_native;

	if (!vsyscall_map_x)
		return 0;

	printf("[RUN]\tchecking that vsyscalls are emulated\n");
	sethandler(SIGTRAP, sigtrap, 0);
	set_eflags(get_eflags() | X86_EFLAGS_TF);
	vtime(&tmp);
	set_eflags(get_eflags() & ~X86_EFLAGS_TF);

	/*
	 * If vsyscalls are emulated, we expect a single trap in the
	 * vsyscall page -- the call instruction will trap with RIP
	 * pointing to the entry point before emulation takes over.
	 * In native mode, we expect two traps, since whatever code
	 * the vsyscall page contains will be more than just a ret
	 * instruction.
	 */
	is_native = (num_vsyscall_traps > 1);

	printf("[%s]\tvsyscalls are %s (%d instructions in vsyscall page)\n",
	       (is_native ? "FAIL" : "OK"),
	       (is_native ? "native" : "emulated"),
	       (int)num_vsyscall_traps);

	return is_native;
}
#endif

int main(int argc, char **argv)
{
	int nerrs = 0;

	init_vdso();
	nerrs += init_vsys();

	nerrs += test_gtod();
	nerrs += test_time();
	nerrs += test_getcpu(0);
	nerrs += test_getcpu(1);

	sethandler(SIGSEGV, sigsegv, 0);
	nerrs += test_vsys_r();
	nerrs += test_vsys_x();

	nerrs += test_process_vm_readv();

#ifdef __x86_64__
	nerrs += test_emulation();
#endif

	return nerrs ? 1 : 0;
}
