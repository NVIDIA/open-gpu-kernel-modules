// SPDX-License-Identifier: GPL-2.0-only
/*
 * xen_vmcall_test
 *
 * Copyright © 2020 Amazon.com, Inc. or its affiliates.
 *
 * Userspace hypercall testing
 */

#include "test_util.h"
#include "kvm_util.h"
#include "processor.h"

#define VCPU_ID		5

#define HCALL_REGION_GPA	0xc0000000ULL
#define HCALL_REGION_SLOT	10
#define PAGE_SIZE		4096

static struct kvm_vm *vm;

#define INPUTVALUE 17
#define ARGVALUE(x) (0xdeadbeef5a5a0000UL + x)
#define RETVALUE 0xcafef00dfbfbffffUL

#define XEN_HYPERCALL_MSR	0x40000200
#define HV_GUEST_OS_ID_MSR	0x40000000
#define HV_HYPERCALL_MSR	0x40000001

#define HVCALL_SIGNAL_EVENT		0x005d
#define HV_STATUS_INVALID_ALIGNMENT	4

static void guest_code(void)
{
	unsigned long rax = INPUTVALUE;
	unsigned long rdi = ARGVALUE(1);
	unsigned long rsi = ARGVALUE(2);
	unsigned long rdx = ARGVALUE(3);
	unsigned long rcx;
	register unsigned long r10 __asm__("r10") = ARGVALUE(4);
	register unsigned long r8 __asm__("r8") = ARGVALUE(5);
	register unsigned long r9 __asm__("r9") = ARGVALUE(6);

	/* First a direct invocation of 'vmcall' */
	__asm__ __volatile__("vmcall" :
			     "=a"(rax) :
			     "a"(rax), "D"(rdi), "S"(rsi), "d"(rdx),
			     "r"(r10), "r"(r8), "r"(r9));
	GUEST_ASSERT(rax == RETVALUE);

	/* Fill in the Xen hypercall page */
	__asm__ __volatile__("wrmsr" : : "c" (XEN_HYPERCALL_MSR),
			     "a" (HCALL_REGION_GPA & 0xffffffff),
			     "d" (HCALL_REGION_GPA >> 32));

	/* Set Hyper-V Guest OS ID */
	__asm__ __volatile__("wrmsr" : : "c" (HV_GUEST_OS_ID_MSR),
			     "a" (0x5a), "d" (0));

	/* Hyper-V hypercall page */
	u64 msrval = HCALL_REGION_GPA + PAGE_SIZE + 1;
	__asm__ __volatile__("wrmsr" : : "c" (HV_HYPERCALL_MSR),
			     "a" (msrval & 0xffffffff),
			     "d" (msrval >> 32));

	/* Invoke a Xen hypercall */
	__asm__ __volatile__("call *%1" : "=a"(rax) :
			     "r"(HCALL_REGION_GPA + INPUTVALUE * 32),
			     "a"(rax), "D"(rdi), "S"(rsi), "d"(rdx),
			     "r"(r10), "r"(r8), "r"(r9));
	GUEST_ASSERT(rax == RETVALUE);

	/* Invoke a Hyper-V hypercall */
	rax = 0;
	rcx = HVCALL_SIGNAL_EVENT;	/* code */
	rdx = 0x5a5a5a5a;		/* ingpa (badly aligned) */
	__asm__ __volatile__("call *%1" : "=a"(rax) :
			     "r"(HCALL_REGION_GPA + PAGE_SIZE),
			     "a"(rax), "c"(rcx), "d"(rdx),
			     "r"(r8));
	GUEST_ASSERT(rax == HV_STATUS_INVALID_ALIGNMENT);

	GUEST_DONE();
}

int main(int argc, char *argv[])
{
	if (!(kvm_check_cap(KVM_CAP_XEN_HVM) &
	      KVM_XEN_HVM_CONFIG_INTERCEPT_HCALL) ) {
		print_skip("KVM_XEN_HVM_CONFIG_INTERCEPT_HCALL not available");
		exit(KSFT_SKIP);
	}

	vm = vm_create_default(VCPU_ID, 0, (void *) guest_code);
	vcpu_set_hv_cpuid(vm, VCPU_ID);

	struct kvm_xen_hvm_config hvmc = {
		.flags = KVM_XEN_HVM_CONFIG_INTERCEPT_HCALL,
		.msr = XEN_HYPERCALL_MSR,
	};
	vm_ioctl(vm, KVM_XEN_HVM_CONFIG, &hvmc);

	/* Map a region for the hypercall pages */
	vm_userspace_mem_region_add(vm, VM_MEM_SRC_ANONYMOUS,
				    HCALL_REGION_GPA, HCALL_REGION_SLOT, 2, 0);
	virt_map(vm, HCALL_REGION_GPA, HCALL_REGION_GPA, 2, 0);

	for (;;) {
		volatile struct kvm_run *run = vcpu_state(vm, VCPU_ID);
		struct ucall uc;

		vcpu_run(vm, VCPU_ID);

		if (run->exit_reason == KVM_EXIT_XEN) {
			ASSERT_EQ(run->xen.type, KVM_EXIT_XEN_HCALL);
			ASSERT_EQ(run->xen.u.hcall.cpl, 0);
			ASSERT_EQ(run->xen.u.hcall.longmode, 1);
			ASSERT_EQ(run->xen.u.hcall.input, INPUTVALUE);
			ASSERT_EQ(run->xen.u.hcall.params[0], ARGVALUE(1));
			ASSERT_EQ(run->xen.u.hcall.params[1], ARGVALUE(2));
			ASSERT_EQ(run->xen.u.hcall.params[2], ARGVALUE(3));
			ASSERT_EQ(run->xen.u.hcall.params[3], ARGVALUE(4));
			ASSERT_EQ(run->xen.u.hcall.params[4], ARGVALUE(5));
			ASSERT_EQ(run->xen.u.hcall.params[5], ARGVALUE(6));
			run->xen.u.hcall.result = RETVALUE;
			continue;
		}

		TEST_ASSERT(run->exit_reason == KVM_EXIT_IO,
			    "Got exit_reason other than KVM_EXIT_IO: %u (%s)\n",
			    run->exit_reason,
			    exit_reason_str(run->exit_reason));

		switch (get_ucall(vm, VCPU_ID, &uc)) {
		case UCALL_ABORT:
			TEST_FAIL("%s", (const char *)uc.args[0]);
			/* NOT REACHED */
		case UCALL_SYNC:
			break;
		case UCALL_DONE:
			goto done;
		default:
			TEST_FAIL("Unknown ucall 0x%lx.", uc.cmd);
		}
	}
done:
	kvm_vm_free(vm);
	return 0;
}
