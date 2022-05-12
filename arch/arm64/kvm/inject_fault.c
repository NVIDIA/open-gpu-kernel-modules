// SPDX-License-Identifier: GPL-2.0-only
/*
 * Fault injection for both 32 and 64bit guests.
 *
 * Copyright (C) 2012,2013 - ARM Ltd
 * Author: Marc Zyngier <marc.zyngier@arm.com>
 *
 * Based on arch/arm/kvm/emulate.c
 * Copyright (C) 2012 - Virtual Open Systems and Columbia University
 * Author: Christoffer Dall <c.dall@virtualopensystems.com>
 */

#include <linux/kvm_host.h>
#include <asm/kvm_emulate.h>
#include <asm/esr.h>

static void inject_abt64(struct kvm_vcpu *vcpu, bool is_iabt, unsigned long addr)
{
	unsigned long cpsr = *vcpu_cpsr(vcpu);
	bool is_aarch32 = vcpu_mode_is_32bit(vcpu);
	u32 esr = 0;

	vcpu->arch.flags |= (KVM_ARM64_EXCEPT_AA64_EL1		|
			     KVM_ARM64_EXCEPT_AA64_ELx_SYNC	|
			     KVM_ARM64_PENDING_EXCEPTION);

	vcpu_write_sys_reg(vcpu, addr, FAR_EL1);

	/*
	 * Build an {i,d}abort, depending on the level and the
	 * instruction set. Report an external synchronous abort.
	 */
	if (kvm_vcpu_trap_il_is32bit(vcpu))
		esr |= ESR_ELx_IL;

	/*
	 * Here, the guest runs in AArch64 mode when in EL1. If we get
	 * an AArch32 fault, it means we managed to trap an EL0 fault.
	 */
	if (is_aarch32 || (cpsr & PSR_MODE_MASK) == PSR_MODE_EL0t)
		esr |= (ESR_ELx_EC_IABT_LOW << ESR_ELx_EC_SHIFT);
	else
		esr |= (ESR_ELx_EC_IABT_CUR << ESR_ELx_EC_SHIFT);

	if (!is_iabt)
		esr |= ESR_ELx_EC_DABT_LOW << ESR_ELx_EC_SHIFT;

	vcpu_write_sys_reg(vcpu, esr | ESR_ELx_FSC_EXTABT, ESR_EL1);
}

static void inject_undef64(struct kvm_vcpu *vcpu)
{
	u32 esr = (ESR_ELx_EC_UNKNOWN << ESR_ELx_EC_SHIFT);

	vcpu->arch.flags |= (KVM_ARM64_EXCEPT_AA64_EL1		|
			     KVM_ARM64_EXCEPT_AA64_ELx_SYNC	|
			     KVM_ARM64_PENDING_EXCEPTION);

	/*
	 * Build an unknown exception, depending on the instruction
	 * set.
	 */
	if (kvm_vcpu_trap_il_is32bit(vcpu))
		esr |= ESR_ELx_IL;

	vcpu_write_sys_reg(vcpu, esr, ESR_EL1);
}

#define DFSR_FSC_EXTABT_LPAE	0x10
#define DFSR_FSC_EXTABT_nLPAE	0x08
#define DFSR_LPAE		BIT(9)
#define TTBCR_EAE		BIT(31)

static void inject_undef32(struct kvm_vcpu *vcpu)
{
	vcpu->arch.flags |= (KVM_ARM64_EXCEPT_AA32_UND |
			     KVM_ARM64_PENDING_EXCEPTION);
}

/*
 * Modelled after TakeDataAbortException() and TakePrefetchAbortException
 * pseudocode.
 */
static void inject_abt32(struct kvm_vcpu *vcpu, bool is_pabt, u32 addr)
{
	u64 far;
	u32 fsr;

	/* Give the guest an IMPLEMENTATION DEFINED exception */
	if (vcpu_read_sys_reg(vcpu, TCR_EL1) & TTBCR_EAE) {
		fsr = DFSR_LPAE | DFSR_FSC_EXTABT_LPAE;
	} else {
		/* no need to shuffle FS[4] into DFSR[10] as its 0 */
		fsr = DFSR_FSC_EXTABT_nLPAE;
	}

	far = vcpu_read_sys_reg(vcpu, FAR_EL1);

	if (is_pabt) {
		vcpu->arch.flags |= (KVM_ARM64_EXCEPT_AA32_IABT |
				     KVM_ARM64_PENDING_EXCEPTION);
		far &= GENMASK(31, 0);
		far |= (u64)addr << 32;
		vcpu_write_sys_reg(vcpu, fsr, IFSR32_EL2);
	} else { /* !iabt */
		vcpu->arch.flags |= (KVM_ARM64_EXCEPT_AA32_DABT |
				     KVM_ARM64_PENDING_EXCEPTION);
		far &= GENMASK(63, 32);
		far |= addr;
		vcpu_write_sys_reg(vcpu, fsr, ESR_EL1);
	}

	vcpu_write_sys_reg(vcpu, far, FAR_EL1);
}

/**
 * kvm_inject_dabt - inject a data abort into the guest
 * @vcpu: The VCPU to receive the data abort
 * @addr: The address to report in the DFAR
 *
 * It is assumed that this code is called from the VCPU thread and that the
 * VCPU therefore is not currently executing guest code.
 */
void kvm_inject_dabt(struct kvm_vcpu *vcpu, unsigned long addr)
{
	if (vcpu_el1_is_32bit(vcpu))
		inject_abt32(vcpu, false, addr);
	else
		inject_abt64(vcpu, false, addr);
}

/**
 * kvm_inject_pabt - inject a prefetch abort into the guest
 * @vcpu: The VCPU to receive the prefetch abort
 * @addr: The address to report in the DFAR
 *
 * It is assumed that this code is called from the VCPU thread and that the
 * VCPU therefore is not currently executing guest code.
 */
void kvm_inject_pabt(struct kvm_vcpu *vcpu, unsigned long addr)
{
	if (vcpu_el1_is_32bit(vcpu))
		inject_abt32(vcpu, true, addr);
	else
		inject_abt64(vcpu, true, addr);
}

/**
 * kvm_inject_undefined - inject an undefined instruction into the guest
 * @vcpu: The vCPU in which to inject the exception
 *
 * It is assumed that this code is called from the VCPU thread and that the
 * VCPU therefore is not currently executing guest code.
 */
void kvm_inject_undefined(struct kvm_vcpu *vcpu)
{
	if (vcpu_el1_is_32bit(vcpu))
		inject_undef32(vcpu);
	else
		inject_undef64(vcpu);
}

void kvm_set_sei_esr(struct kvm_vcpu *vcpu, u64 esr)
{
	vcpu_set_vsesr(vcpu, esr & ESR_ELx_ISS_MASK);
	*vcpu_hcr(vcpu) |= HCR_VSE;
}

/**
 * kvm_inject_vabt - inject an async abort / SError into the guest
 * @vcpu: The VCPU to receive the exception
 *
 * It is assumed that this code is called from the VCPU thread and that the
 * VCPU therefore is not currently executing guest code.
 *
 * Systems with the RAS Extensions specify an imp-def ESR (ISV/IDS = 1) with
 * the remaining ISS all-zeros so that this error is not interpreted as an
 * uncategorized RAS error. Without the RAS Extensions we can't specify an ESR
 * value, so the CPU generates an imp-def value.
 */
void kvm_inject_vabt(struct kvm_vcpu *vcpu)
{
	kvm_set_sei_esr(vcpu, ESR_ELx_ISV);
}
