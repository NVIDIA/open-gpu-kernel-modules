// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2012-2015 - ARM Ltd
 * Author: Marc Zyngier <marc.zyngier@arm.com>
 */

#include <hyp/sysreg-sr.h>

#include <linux/compiler.h>
#include <linux/kvm_host.h>

#include <asm/kprobes.h>
#include <asm/kvm_asm.h>
#include <asm/kvm_emulate.h>
#include <asm/kvm_hyp.h>

/*
 * VHE: Host and guest must save mdscr_el1 and sp_el0 (and the PC and
 * pstate, which are handled as part of the el2 return state) on every
 * switch (sp_el0 is being dealt with in the assembly code).
 * tpidr_el0 and tpidrro_el0 only need to be switched when going
 * to host userspace or a different VCPU.  EL1 registers only need to be
 * switched when potentially going to run a different VCPU.  The latter two
 * classes are handled as part of kvm_arch_vcpu_load and kvm_arch_vcpu_put.
 */

void sysreg_save_host_state_vhe(struct kvm_cpu_context *ctxt)
{
	__sysreg_save_common_state(ctxt);
}
NOKPROBE_SYMBOL(sysreg_save_host_state_vhe);

void sysreg_save_guest_state_vhe(struct kvm_cpu_context *ctxt)
{
	__sysreg_save_common_state(ctxt);
	__sysreg_save_el2_return_state(ctxt);
}
NOKPROBE_SYMBOL(sysreg_save_guest_state_vhe);

void sysreg_restore_host_state_vhe(struct kvm_cpu_context *ctxt)
{
	__sysreg_restore_common_state(ctxt);
}
NOKPROBE_SYMBOL(sysreg_restore_host_state_vhe);

void sysreg_restore_guest_state_vhe(struct kvm_cpu_context *ctxt)
{
	__sysreg_restore_common_state(ctxt);
	__sysreg_restore_el2_return_state(ctxt);
}
NOKPROBE_SYMBOL(sysreg_restore_guest_state_vhe);

/**
 * kvm_vcpu_load_sysregs_vhe - Load guest system registers to the physical CPU
 *
 * @vcpu: The VCPU pointer
 *
 * Load system registers that do not affect the host's execution, for
 * example EL1 system registers on a VHE system where the host kernel
 * runs at EL2.  This function is called from KVM's vcpu_load() function
 * and loading system register state early avoids having to load them on
 * every entry to the VM.
 */
void kvm_vcpu_load_sysregs_vhe(struct kvm_vcpu *vcpu)
{
	struct kvm_cpu_context *guest_ctxt = &vcpu->arch.ctxt;
	struct kvm_cpu_context *host_ctxt;

	host_ctxt = &this_cpu_ptr(&kvm_host_data)->host_ctxt;
	__sysreg_save_user_state(host_ctxt);

	/*
	 * Load guest EL1 and user state
	 *
	 * We must restore the 32-bit state before the sysregs, thanks
	 * to erratum #852523 (Cortex-A57) or #853709 (Cortex-A72).
	 */
	__sysreg32_restore_state(vcpu);
	__sysreg_restore_user_state(guest_ctxt);
	__sysreg_restore_el1_state(guest_ctxt);

	vcpu->arch.sysregs_loaded_on_cpu = true;

	activate_traps_vhe_load(vcpu);
}

/**
 * kvm_vcpu_put_sysregs_vhe - Restore host system registers to the physical CPU
 *
 * @vcpu: The VCPU pointer
 *
 * Save guest system registers that do not affect the host's execution, for
 * example EL1 system registers on a VHE system where the host kernel
 * runs at EL2.  This function is called from KVM's vcpu_put() function
 * and deferring saving system register state until we're no longer running the
 * VCPU avoids having to save them on every exit from the VM.
 */
void kvm_vcpu_put_sysregs_vhe(struct kvm_vcpu *vcpu)
{
	struct kvm_cpu_context *guest_ctxt = &vcpu->arch.ctxt;
	struct kvm_cpu_context *host_ctxt;

	host_ctxt = &this_cpu_ptr(&kvm_host_data)->host_ctxt;
	deactivate_traps_vhe_put();

	__sysreg_save_el1_state(guest_ctxt);
	__sysreg_save_user_state(guest_ctxt);
	__sysreg32_save_state(vcpu);

	/* Restore host user state */
	__sysreg_restore_user_state(host_ctxt);

	vcpu->arch.sysregs_loaded_on_cpu = false;
}
