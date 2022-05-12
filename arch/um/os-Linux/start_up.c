// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2000 - 2007 Jeff Dike (jdike@{addtoit,linux.intel}.com)
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sched.h>
#include <signal.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <asm/unistd.h>
#include <init.h>
#include <os.h>
#include <mem_user.h>
#include <ptrace_user.h>
#include <registers.h>
#include <skas.h>

static void ptrace_child(void)
{
	int ret;
	/* Calling os_getpid because some libcs cached getpid incorrectly */
	int pid = os_getpid(), ppid = getppid();
	int sc_result;

	if (change_sig(SIGWINCH, 0) < 0 ||
	    ptrace(PTRACE_TRACEME, 0, 0, 0) < 0) {
		perror("ptrace");
		kill(pid, SIGKILL);
	}
	kill(pid, SIGSTOP);

	/*
	 * This syscall will be intercepted by the parent. Don't call more than
	 * once, please.
	 */
	sc_result = os_getpid();

	if (sc_result == pid)
		/* Nothing modified by the parent, we are running normally. */
		ret = 1;
	else if (sc_result == ppid)
		/*
		 * Expected in check_ptrace and check_sysemu when they succeed
		 * in modifying the stack frame
		 */
		ret = 0;
	else
		/* Serious trouble! This could be caused by a bug in host 2.6
		 * SKAS3/2.6 patch before release -V6, together with a bug in
		 * the UML code itself.
		 */
		ret = 2;

	exit(ret);
}

static void fatal_perror(const char *str)
{
	perror(str);
	exit(1);
}

static void fatal(char *fmt, ...)
{
	va_list list;

	va_start(list, fmt);
	vfprintf(stderr, fmt, list);
	va_end(list);

	exit(1);
}

static void non_fatal(char *fmt, ...)
{
	va_list list;

	va_start(list, fmt);
	vfprintf(stderr, fmt, list);
	va_end(list);
}

static int start_ptraced_child(void)
{
	int pid, n, status;

	fflush(stdout);

	pid = fork();
	if (pid == 0)
		ptrace_child();
	else if (pid < 0)
		fatal_perror("start_ptraced_child : fork failed");

	CATCH_EINTR(n = waitpid(pid, &status, WUNTRACED));
	if (n < 0)
		fatal_perror("check_ptrace : waitpid failed");
	if (!WIFSTOPPED(status) || (WSTOPSIG(status) != SIGSTOP))
		fatal("check_ptrace : expected SIGSTOP, got status = %d",
		      status);

	return pid;
}

/* When testing for SYSEMU support, if it is one of the broken versions, we
 * must just avoid using sysemu, not panic, but only if SYSEMU features are
 * broken.
 * So only for SYSEMU features we test mustpanic, while normal host features
 * must work anyway!
 */
static int stop_ptraced_child(int pid, int exitcode, int mustexit)
{
	int status, n, ret = 0;

	if (ptrace(PTRACE_CONT, pid, 0, 0) < 0) {
		perror("stop_ptraced_child : ptrace failed");
		return -1;
	}
	CATCH_EINTR(n = waitpid(pid, &status, 0));
	if (!WIFEXITED(status) || (WEXITSTATUS(status) != exitcode)) {
		int exit_with = WEXITSTATUS(status);
		if (exit_with == 2)
			non_fatal("check_ptrace : child exited with status 2. "
				  "\nDisabling SYSEMU support.\n");
		non_fatal("check_ptrace : child exited with exitcode %d, while "
			  "expecting %d; status 0x%x\n", exit_with,
			  exitcode, status);
		if (mustexit)
			exit(1);
		ret = -1;
	}

	return ret;
}

/* Changed only during early boot */
static int force_sysemu_disabled = 0;

static int __init nosysemu_cmd_param(char *str, int* add)
{
	force_sysemu_disabled = 1;
	return 0;
}

__uml_setup("nosysemu", nosysemu_cmd_param,
"nosysemu\n"
"    Turns off syscall emulation patch for ptrace (SYSEMU).\n"
"    SYSEMU is a performance-patch introduced by Laurent Vivier. It changes\n"
"    behaviour of ptrace() and helps reduce host context switch rates.\n"
"    To make it work, you need a kernel patch for your host, too.\n"
"    See http://perso.wanadoo.fr/laurent.vivier/UML/ for further \n"
"    information.\n\n");

static void __init check_sysemu(void)
{
	unsigned long regs[MAX_REG_NR];
	int pid, n, status, count=0;

	os_info("Checking syscall emulation patch for ptrace...");
	sysemu_supported = 0;
	pid = start_ptraced_child();

	if (ptrace(PTRACE_SYSEMU, pid, 0, 0) < 0)
		goto fail;

	CATCH_EINTR(n = waitpid(pid, &status, WUNTRACED));
	if (n < 0)
		fatal_perror("check_sysemu : wait failed");
	if (!WIFSTOPPED(status) || (WSTOPSIG(status) != SIGTRAP))
		fatal("check_sysemu : expected SIGTRAP, got status = %d\n",
		      status);

	if (ptrace(PTRACE_GETREGS, pid, 0, regs) < 0)
		fatal_perror("check_sysemu : PTRACE_GETREGS failed");
	if (PT_SYSCALL_NR(regs) != __NR_getpid) {
		non_fatal("check_sysemu got system call number %d, "
			  "expected %d...", PT_SYSCALL_NR(regs), __NR_getpid);
		goto fail;
	}

	n = ptrace(PTRACE_POKEUSER, pid, PT_SYSCALL_RET_OFFSET, os_getpid());
	if (n < 0) {
		non_fatal("check_sysemu : failed to modify system call "
			  "return");
		goto fail;
	}

	if (stop_ptraced_child(pid, 0, 0) < 0)
		goto fail_stopped;

	sysemu_supported = 1;
	os_info("OK\n");
	set_using_sysemu(!force_sysemu_disabled);

	os_info("Checking advanced syscall emulation patch for ptrace...");
	pid = start_ptraced_child();

	if ((ptrace(PTRACE_OLDSETOPTIONS, pid, 0,
		   (void *) PTRACE_O_TRACESYSGOOD) < 0))
		fatal_perror("check_sysemu: PTRACE_OLDSETOPTIONS failed");

	while (1) {
		count++;
		if (ptrace(PTRACE_SYSEMU_SINGLESTEP, pid, 0, 0) < 0)
			goto fail;
		CATCH_EINTR(n = waitpid(pid, &status, WUNTRACED));
		if (n < 0)
			fatal_perror("check_sysemu: wait failed");

		if (WIFSTOPPED(status) &&
		    (WSTOPSIG(status) == (SIGTRAP|0x80))) {
			if (!count) {
				non_fatal("check_sysemu: SYSEMU_SINGLESTEP "
					  "doesn't singlestep");
				goto fail;
			}
			n = ptrace(PTRACE_POKEUSER, pid, PT_SYSCALL_RET_OFFSET,
				   os_getpid());
			if (n < 0)
				fatal_perror("check_sysemu : failed to modify "
					     "system call return");
			break;
		}
		else if (WIFSTOPPED(status) && (WSTOPSIG(status) == SIGTRAP))
			count++;
		else {
			non_fatal("check_sysemu: expected SIGTRAP or "
				  "(SIGTRAP | 0x80), got status = %d\n",
				  status);
			goto fail;
		}
	}
	if (stop_ptraced_child(pid, 0, 0) < 0)
		goto fail_stopped;

	sysemu_supported = 2;
	os_info("OK\n");

	if (!force_sysemu_disabled)
		set_using_sysemu(sysemu_supported);
	return;

fail:
	stop_ptraced_child(pid, 1, 0);
fail_stopped:
	non_fatal("missing\n");
}

static void __init check_ptrace(void)
{
	int pid, syscall, n, status;

	os_info("Checking that ptrace can change system call numbers...");
	pid = start_ptraced_child();

	if ((ptrace(PTRACE_OLDSETOPTIONS, pid, 0,
		   (void *) PTRACE_O_TRACESYSGOOD) < 0))
		fatal_perror("check_ptrace: PTRACE_OLDSETOPTIONS failed");

	while (1) {
		if (ptrace(PTRACE_SYSCALL, pid, 0, 0) < 0)
			fatal_perror("check_ptrace : ptrace failed");

		CATCH_EINTR(n = waitpid(pid, &status, WUNTRACED));
		if (n < 0)
			fatal_perror("check_ptrace : wait failed");

		if (!WIFSTOPPED(status) ||
		   (WSTOPSIG(status) != (SIGTRAP | 0x80)))
			fatal("check_ptrace : expected (SIGTRAP|0x80), "
			       "got status = %d", status);

		syscall = ptrace(PTRACE_PEEKUSER, pid, PT_SYSCALL_NR_OFFSET,
				 0);
		if (syscall == __NR_getpid) {
			n = ptrace(PTRACE_POKEUSER, pid, PT_SYSCALL_NR_OFFSET,
				   __NR_getppid);
			if (n < 0)
				fatal_perror("check_ptrace : failed to modify "
					     "system call");
			break;
		}
	}
	stop_ptraced_child(pid, 0, 1);
	os_info("OK\n");
	check_sysemu();
}

extern void check_tmpexec(void);

static void __init check_coredump_limit(void)
{
	struct rlimit lim;
	int err = getrlimit(RLIMIT_CORE, &lim);

	if (err) {
		perror("Getting core dump limit");
		return;
	}

	os_info("Core dump limits :\n\tsoft - ");
	if (lim.rlim_cur == RLIM_INFINITY)
		os_info("NONE\n");
	else
		os_info("%llu\n", (unsigned long long)lim.rlim_cur);

	os_info("\thard - ");
	if (lim.rlim_max == RLIM_INFINITY)
		os_info("NONE\n");
	else
		os_info("%llu\n", (unsigned long long)lim.rlim_max);
}

void __init os_early_checks(void)
{
	int pid;

	/* Print out the core dump limits early */
	check_coredump_limit();

	check_ptrace();

	/* Need to check this early because mmapping happens before the
	 * kernel is running.
	 */
	check_tmpexec();

	pid = start_ptraced_child();
	if (init_registers(pid))
		fatal("Failed to initialize default registers");
	stop_ptraced_child(pid, 1, 1);
}

int __init parse_iomem(char *str, int *add)
{
	struct iomem_region *new;
	struct stat64 buf;
	char *file, *driver;
	int fd, size;

	driver = str;
	file = strchr(str,',');
	if (file == NULL) {
		os_warn("parse_iomem : failed to parse iomem\n");
		goto out;
	}
	*file = '\0';
	file++;
	fd = open(file, O_RDWR, 0);
	if (fd < 0) {
		perror("parse_iomem - Couldn't open io file");
		goto out;
	}

	if (fstat64(fd, &buf) < 0) {
		perror("parse_iomem - cannot stat_fd file");
		goto out_close;
	}

	new = malloc(sizeof(*new));
	if (new == NULL) {
		perror("Couldn't allocate iomem_region struct");
		goto out_close;
	}

	size = (buf.st_size + UM_KERN_PAGE_SIZE) & ~(UM_KERN_PAGE_SIZE - 1);

	*new = ((struct iomem_region) { .next		= iomem_regions,
					.driver		= driver,
					.fd		= fd,
					.size		= size,
					.phys		= 0,
					.virt		= 0 });
	iomem_regions = new;
	iomem_size += new->size + UM_KERN_PAGE_SIZE;

	return 0;
 out_close:
	close(fd);
 out:
	return 1;
}
