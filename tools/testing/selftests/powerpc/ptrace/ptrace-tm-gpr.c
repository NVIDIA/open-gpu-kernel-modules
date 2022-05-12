// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Ptrace test for GPR/FPR registers in TM context
 *
 * Copyright (C) 2015 Anshuman Khandual, IBM Corporation.
 */
#include "ptrace.h"
#include "ptrace-gpr.h"
#include "tm.h"

/* Tracer and Tracee Shared Data */
int shm_id;
unsigned long *cptr, *pptr;

float a = FPR_1;
float b = FPR_2;
float c = FPR_3;

void tm_gpr(void)
{
	unsigned long gpr_buf[18];
	unsigned long result, texasr;
	float fpr_buf[32];

	printf("Starting the child\n");
	cptr = (unsigned long *)shmat(shm_id, NULL, 0);

trans:
	cptr[1] = 0;
	asm __volatile__(
		ASM_LOAD_GPR_IMMED(gpr_1)
		ASM_LOAD_FPR_SINGLE_PRECISION(flt_1)
		"1: ;"
		"tbegin.;"
		"beq 2f;"
		ASM_LOAD_GPR_IMMED(gpr_2)
		ASM_LOAD_FPR_SINGLE_PRECISION(flt_2)
		"tsuspend.;"
		"li 7, 1;"
		"stw 7, 0(%[cptr1]);"
		"tresume.;"
		"b .;"

		"tend.;"
		"li 0, 0;"
		"ori %[res], 0, 0;"
		"b 3f;"

		/* Transaction abort handler */
		"2: ;"
		"li 0, 1;"
		"ori %[res], 0, 0;"
		"mfspr %[texasr], %[sprn_texasr];"

		"3: ;"
		: [res] "=r" (result), [texasr] "=r" (texasr)
		: [gpr_1]"i"(GPR_1), [gpr_2]"i"(GPR_2),
		[sprn_texasr] "i" (SPRN_TEXASR), [flt_1] "b" (&a),
		[flt_2] "b" (&b), [cptr1] "b" (&cptr[1])
		: "memory", "r7", "r8", "r9", "r10",
		"r11", "r12", "r13", "r14", "r15", "r16",
		"r17", "r18", "r19", "r20", "r21", "r22",
		"r23", "r24", "r25", "r26", "r27", "r28",
		"r29", "r30", "r31"
		);

	if (result) {
		if (!cptr[0])
			goto trans;

		shmdt((void *)cptr);
		store_gpr(gpr_buf);
		store_fpr_single_precision(fpr_buf);

		if (validate_gpr(gpr_buf, GPR_3))
			exit(1);

		if (validate_fpr_float(fpr_buf, c))
			exit(1);

		exit(0);
	}
	shmdt((void *)cptr);
	exit(1);
}

int trace_tm_gpr(pid_t child)
{
	unsigned long gpr[18];
	unsigned long fpr[32];

	FAIL_IF(start_trace(child));
	FAIL_IF(show_gpr(child, gpr));
	FAIL_IF(validate_gpr(gpr, GPR_2));
	FAIL_IF(show_fpr(child, fpr));
	FAIL_IF(validate_fpr(fpr, FPR_2_REP));
	FAIL_IF(show_ckpt_fpr(child, fpr));
	FAIL_IF(validate_fpr(fpr, FPR_1_REP));
	FAIL_IF(show_ckpt_gpr(child, gpr));
	FAIL_IF(validate_gpr(gpr, GPR_1));
	FAIL_IF(write_ckpt_gpr(child, GPR_3));
	FAIL_IF(write_ckpt_fpr(child, FPR_3_REP));

	pptr[0] = 1;
	FAIL_IF(stop_trace(child));

	return TEST_PASS;
}

int ptrace_tm_gpr(void)
{
	pid_t pid;
	int ret, status;

	SKIP_IF(!have_htm());
	shm_id = shmget(IPC_PRIVATE, sizeof(int) * 2, 0777|IPC_CREAT);
	pid = fork();
	if (pid < 0) {
		perror("fork() failed");
		return TEST_FAIL;
	}
	if (pid == 0)
		tm_gpr();

	if (pid) {
		pptr = (unsigned long *)shmat(shm_id, NULL, 0);

		while (!pptr[1])
			asm volatile("" : : : "memory");
		ret = trace_tm_gpr(pid);
		if (ret) {
			kill(pid, SIGTERM);
			return TEST_FAIL;
		}

		shmdt((void *)pptr);

		ret = wait(&status);
		shmctl(shm_id, IPC_RMID, NULL);
		if (ret != pid) {
			printf("Child's exit status not captured\n");
			return TEST_FAIL;
		}

		return (WIFEXITED(status) && WEXITSTATUS(status)) ? TEST_FAIL :
			TEST_PASS;
	}
	return TEST_PASS;
}

int main(int argc, char *argv[])
{
	return test_harness(ptrace_tm_gpr, "ptrace_tm_gpr");
}
