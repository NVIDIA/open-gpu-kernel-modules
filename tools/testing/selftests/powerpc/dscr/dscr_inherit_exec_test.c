// SPDX-License-Identifier: GPL-2.0-only
/*
 * POWER Data Stream Control Register (DSCR) fork exec test
 *
 * This testcase modifies the DSCR using mtspr, forks & execs and
 * verifies that the child is using the changed DSCR using mfspr.
 *
 * When using the privilege state SPR, the instructions such as
 * mfspr or mtspr are privileged and the kernel emulates them
 * for us. Instructions using problem state SPR can be executed
 * directly without any emulation if the HW supports them. Else
 * they also get emulated by the kernel.
 *
 * Copyright 2012, Anton Blanchard, IBM Corporation.
 * Copyright 2015, Anshuman Khandual, IBM Corporation.
 */
#include "dscr.h"

static char *prog;

static void do_exec(unsigned long parent_dscr)
{
	unsigned long cur_dscr, cur_dscr_usr;

	cur_dscr = get_dscr();
	cur_dscr_usr = get_dscr_usr();

	if (cur_dscr != parent_dscr) {
		fprintf(stderr, "Parent DSCR %ld was not inherited "
				"over exec (kernel value)\n", parent_dscr);
		exit(1);
	}

	if (cur_dscr_usr != parent_dscr) {
		fprintf(stderr, "Parent DSCR %ld was not inherited "
				"over exec (user value)\n", parent_dscr);
		exit(1);
	}
	exit(0);
}

int dscr_inherit_exec(void)
{
	unsigned long i, dscr = 0;
	pid_t pid;

	SKIP_IF(!have_hwcap2(PPC_FEATURE2_DSCR));

	for (i = 0; i < COUNT; i++) {
		dscr++;
		if (dscr > DSCR_MAX)
			dscr = 0;

		if (dscr == get_default_dscr())
			continue;

		if (i % 2 == 0)
			set_dscr_usr(dscr);
		else
			set_dscr(dscr);

		pid = fork();
		if (pid == -1) {
			perror("fork() failed");
			exit(1);
		} else if (pid) {
			int status;

			if (waitpid(pid, &status, 0) == -1) {
				perror("waitpid() failed");
				exit(1);
			}

			if (!WIFEXITED(status)) {
				fprintf(stderr, "Child didn't exit cleanly\n");
				exit(1);
			}

			if (WEXITSTATUS(status) != 0) {
				fprintf(stderr, "Child didn't exit cleanly\n");
				return 1;
			}
		} else {
			char dscr_str[16];

			sprintf(dscr_str, "%ld", dscr);
			execlp(prog, prog, "exec", dscr_str, NULL);
			exit(1);
		}
	}
	return 0;
}

int main(int argc, char *argv[])
{
	if (argc == 3 && !strcmp(argv[1], "exec")) {
		unsigned long parent_dscr;

		parent_dscr = atoi(argv[2]);
		do_exec(parent_dscr);
	} else if (argc != 1) {
		fprintf(stderr, "Usage: %s\n", argv[0]);
		exit(1);
	}

	prog = argv[0];
	return test_harness(dscr_inherit_exec, "dscr_inherit_exec_test");
}
