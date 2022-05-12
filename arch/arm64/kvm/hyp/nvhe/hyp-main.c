// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2020 - Google Inc
 * Author: Andrew Scull <ascull@google.com>
 */

#include <hyp/switch.h>

#include <asm/pgtable-types.h>
#include <asm/kvm_asm.h>
#include <asm/kvm_emulate.h>
#include <asm/kvm_host.h>
#include <asm/kvm_hyp.h>
#include <asm/kvm_mmu.h>

#include <nvhe/mem_protect.h>
#include <nvhe/mm.h>
#include <nvhe/trap_handler.h>

DEFINE_PER_CPU(struct kvm_nvhe_init_params, kvm_init_params);

void __kvm_hyp_host_forward_smc(struct kvm_cpu_context *host_ctxt);

static void handle___kvm_vcpu_run(struct kvm_cpu_context *host_ctxt)
{
	DECLARE_REG(struct kvm_vcpu *, vcpu, host_ctxt, 1);

	cpu_reg(host_ctxt, 1) =  __kvm_vcpu_run(kern_hyp_va(vcpu));
}

static void handle___kvm_adjust_pc(struct kvm_cpu_context *host_ctxt)
{
	DECLARE_REG(struct kvm_vcpu *, vcpu, host_ctxt, 1);

	__kvm_adjust_pc(kern_hyp_va(vcpu));
}

static void handle___kvm_flush_vm_context(struct kvm_cpu_context *host_ctxt)
{
	__kvm_flush_vm_context();
}

static void handle___kvm_tlb_flush_vmid_ipa(struct kvm_cpu_context *host_ctxt)
{
	DECLARE_REG(struct kvm_s2_mmu *, mmu, host_ctxt, 1);
	DECLARE_REG(phys_addr_t, ipa, host_ctxt, 2);
	DECLARE_REG(int, level, host_ctxt, 3);

	__kvm_tlb_flush_vmid_ipa(kern_hyp_va(mmu), ipa, level);
}

static void handle___kvm_tlb_flush_vmid(struct kvm_cpu_context *host_ctxt)
{
	DECLARE_REG(struct kvm_s2_mmu *, mmu, host_ctxt, 1);

	__kvm_tlb_flush_vmid(kern_hyp_va(mmu));
}

static void handle___kvm_flush_cpu_context(struct kvm_cpu_context *host_ctxt)
{
	DECLARE_REG(struct kvm_s2_mmu *, mmu, host_ctxt, 1);

	__kvm_flush_cpu_context(kern_hyp_va(mmu));
}

static void handle___kvm_timer_set_cntvoff(struct kvm_cpu_context *host_ctxt)
{
	__kvm_timer_set_cntvoff(cpu_reg(host_ctxt, 1));
}

static void handle___kvm_enable_ssbs(struct kvm_cpu_context *host_ctxt)
{
	u64 tmp;

	tmp = read_sysreg_el2(SYS_SCTLR);
	tmp |= SCTLR_ELx_DSSBS;
	write_sysreg_el2(tmp, SYS_SCTLR);
}

static void handle___vgic_v3_get_gic_config(struct kvm_cpu_context *host_ctxt)
{
	cpu_reg(host_ctxt, 1) = __vgic_v3_get_gic_config();
}

static void handle___vgic_v3_read_vmcr(struct kvm_cpu_context *host_ctxt)
{
	cpu_reg(host_ctxt, 1) = __vgic_v3_read_vmcr();
}

static void handle___vgic_v3_write_vmcr(struct kvm_cpu_context *host_ctxt)
{
	__vgic_v3_write_vmcr(cpu_reg(host_ctxt, 1));
}

static void handle___vgic_v3_init_lrs(struct kvm_cpu_context *host_ctxt)
{
	__vgic_v3_init_lrs();
}

static void handle___kvm_get_mdcr_el2(struct kvm_cpu_context *host_ctxt)
{
	cpu_reg(host_ctxt, 1) = __kvm_get_mdcr_el2();
}

static void handle___vgic_v3_save_aprs(struct kvm_cpu_context *host_ctxt)
{
	DECLARE_REG(struct vgic_v3_cpu_if *, cpu_if, host_ctxt, 1);

	__vgic_v3_save_aprs(kern_hyp_va(cpu_if));
}

static void handle___vgic_v3_restore_aprs(struct kvm_cpu_context *host_ctxt)
{
	DECLARE_REG(struct vgic_v3_cpu_if *, cpu_if, host_ctxt, 1);

	__vgic_v3_restore_aprs(kern_hyp_va(cpu_if));
}

static void handle___pkvm_init(struct kvm_cpu_context *host_ctxt)
{
	DECLARE_REG(phys_addr_t, phys, host_ctxt, 1);
	DECLARE_REG(unsigned long, size, host_ctxt, 2);
	DECLARE_REG(unsigned long, nr_cpus, host_ctxt, 3);
	DECLARE_REG(unsigned long *, per_cpu_base, host_ctxt, 4);
	DECLARE_REG(u32, hyp_va_bits, host_ctxt, 5);

	/*
	 * __pkvm_init() will return only if an error occurred, otherwise it
	 * will tail-call in __pkvm_init_finalise() which will have to deal
	 * with the host context directly.
	 */
	cpu_reg(host_ctxt, 1) = __pkvm_init(phys, size, nr_cpus, per_cpu_base,
					    hyp_va_bits);
}

static void handle___pkvm_cpu_set_vector(struct kvm_cpu_context *host_ctxt)
{
	DECLARE_REG(enum arm64_hyp_spectre_vector, slot, host_ctxt, 1);

	cpu_reg(host_ctxt, 1) = pkvm_cpu_set_vector(slot);
}

static void handle___pkvm_create_mappings(struct kvm_cpu_context *host_ctxt)
{
	DECLARE_REG(unsigned long, start, host_ctxt, 1);
	DECLARE_REG(unsigned long, size, host_ctxt, 2);
	DECLARE_REG(unsigned long, phys, host_ctxt, 3);
	DECLARE_REG(enum kvm_pgtable_prot, prot, host_ctxt, 4);

	cpu_reg(host_ctxt, 1) = __pkvm_create_mappings(start, size, phys, prot);
}

static void handle___pkvm_create_private_mapping(struct kvm_cpu_context *host_ctxt)
{
	DECLARE_REG(phys_addr_t, phys, host_ctxt, 1);
	DECLARE_REG(size_t, size, host_ctxt, 2);
	DECLARE_REG(enum kvm_pgtable_prot, prot, host_ctxt, 3);

	cpu_reg(host_ctxt, 1) = __pkvm_create_private_mapping(phys, size, prot);
}

static void handle___pkvm_prot_finalize(struct kvm_cpu_context *host_ctxt)
{
	cpu_reg(host_ctxt, 1) = __pkvm_prot_finalize();
}

static void handle___pkvm_mark_hyp(struct kvm_cpu_context *host_ctxt)
{
	DECLARE_REG(phys_addr_t, start, host_ctxt, 1);
	DECLARE_REG(phys_addr_t, end, host_ctxt, 2);

	cpu_reg(host_ctxt, 1) = __pkvm_mark_hyp(start, end);
}
typedef void (*hcall_t)(struct kvm_cpu_context *);

#define HANDLE_FUNC(x)	[__KVM_HOST_SMCCC_FUNC_##x] = (hcall_t)handle_##x

static const hcall_t host_hcall[] = {
	HANDLE_FUNC(__kvm_vcpu_run),
	HANDLE_FUNC(__kvm_adjust_pc),
	HANDLE_FUNC(__kvm_flush_vm_context),
	HANDLE_FUNC(__kvm_tlb_flush_vmid_ipa),
	HANDLE_FUNC(__kvm_tlb_flush_vmid),
	HANDLE_FUNC(__kvm_flush_cpu_context),
	HANDLE_FUNC(__kvm_timer_set_cntvoff),
	HANDLE_FUNC(__kvm_enable_ssbs),
	HANDLE_FUNC(__vgic_v3_get_gic_config),
	HANDLE_FUNC(__vgic_v3_read_vmcr),
	HANDLE_FUNC(__vgic_v3_write_vmcr),
	HANDLE_FUNC(__vgic_v3_init_lrs),
	HANDLE_FUNC(__kvm_get_mdcr_el2),
	HANDLE_FUNC(__vgic_v3_save_aprs),
	HANDLE_FUNC(__vgic_v3_restore_aprs),
	HANDLE_FUNC(__pkvm_init),
	HANDLE_FUNC(__pkvm_cpu_set_vector),
	HANDLE_FUNC(__pkvm_create_mappings),
	HANDLE_FUNC(__pkvm_create_private_mapping),
	HANDLE_FUNC(__pkvm_prot_finalize),
	HANDLE_FUNC(__pkvm_mark_hyp),
};

static void handle_host_hcall(struct kvm_cpu_context *host_ctxt)
{
	DECLARE_REG(unsigned long, id, host_ctxt, 0);
	hcall_t hfn;

	id -= KVM_HOST_SMCCC_ID(0);

	if (unlikely(id >= ARRAY_SIZE(host_hcall)))
		goto inval;

	hfn = host_hcall[id];
	if (unlikely(!hfn))
		goto inval;

	cpu_reg(host_ctxt, 0) = SMCCC_RET_SUCCESS;
	hfn(host_ctxt);

	return;
inval:
	cpu_reg(host_ctxt, 0) = SMCCC_RET_NOT_SUPPORTED;
}

static void default_host_smc_handler(struct kvm_cpu_context *host_ctxt)
{
	__kvm_hyp_host_forward_smc(host_ctxt);
}

static void handle_host_smc(struct kvm_cpu_context *host_ctxt)
{
	bool handled;

	handled = kvm_host_psci_handler(host_ctxt);
	if (!handled)
		default_host_smc_handler(host_ctxt);

	/* SMC was trapped, move ELR past the current PC. */
	kvm_skip_host_instr();
}

void handle_trap(struct kvm_cpu_context *host_ctxt)
{
	u64 esr = read_sysreg_el2(SYS_ESR);

	switch (ESR_ELx_EC(esr)) {
	case ESR_ELx_EC_HVC64:
		handle_host_hcall(host_ctxt);
		break;
	case ESR_ELx_EC_SMC64:
		handle_host_smc(host_ctxt);
		break;
	case ESR_ELx_EC_SVE:
		sysreg_clear_set(cptr_el2, CPTR_EL2_TZ, 0);
		isb();
		sve_cond_update_zcr_vq(ZCR_ELx_LEN_MASK, SYS_ZCR_EL2);
		break;
	case ESR_ELx_EC_IABT_LOW:
	case ESR_ELx_EC_DABT_LOW:
		handle_host_mem_abort(host_ctxt);
		break;
	default:
		BUG();
	}
}
