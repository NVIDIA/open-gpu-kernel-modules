// SPDX-License-Identifier: GPL-2.0-only
/*
 * Kernel-based Virtual Machine driver for Linux
 *
 * AMD SVM support
 *
 * Copyright (C) 2006 Qumranet, Inc.
 * Copyright 2010 Red Hat, Inc. and/or its affiliates.
 *
 * Authors:
 *   Yaniv Kamay  <yaniv@qumranet.com>
 *   Avi Kivity   <avi@qumranet.com>
 */

#ifndef __SVM_SVM_H
#define __SVM_SVM_H

#include <linux/kvm_types.h>
#include <linux/kvm_host.h>
#include <linux/bits.h>

#include <asm/svm.h>
#include <asm/sev-common.h>

#define __sme_page_pa(x) __sme_set(page_to_pfn(x) << PAGE_SHIFT)

#define	IOPM_SIZE PAGE_SIZE * 3
#define	MSRPM_SIZE PAGE_SIZE * 2

#define MAX_DIRECT_ACCESS_MSRS	20
#define MSRPM_OFFSETS	16
extern u32 msrpm_offsets[MSRPM_OFFSETS] __read_mostly;
extern bool npt_enabled;

enum {
	VMCB_INTERCEPTS, /* Intercept vectors, TSC offset,
			    pause filter count */
	VMCB_PERM_MAP,   /* IOPM Base and MSRPM Base */
	VMCB_ASID,	 /* ASID */
	VMCB_INTR,	 /* int_ctl, int_vector */
	VMCB_NPT,        /* npt_en, nCR3, gPAT */
	VMCB_CR,	 /* CR0, CR3, CR4, EFER */
	VMCB_DR,         /* DR6, DR7 */
	VMCB_DT,         /* GDT, IDT */
	VMCB_SEG,        /* CS, DS, SS, ES, CPL */
	VMCB_CR2,        /* CR2 only */
	VMCB_LBR,        /* DBGCTL, BR_FROM, BR_TO, LAST_EX_FROM, LAST_EX_TO */
	VMCB_AVIC,       /* AVIC APIC_BAR, AVIC APIC_BACKING_PAGE,
			  * AVIC PHYSICAL_TABLE pointer,
			  * AVIC LOGICAL_TABLE pointer
			  */
	VMCB_DIRTY_MAX,
};

/* TPR and CR2 are always written before VMRUN */
#define VMCB_ALWAYS_DIRTY_MASK	((1U << VMCB_INTR) | (1U << VMCB_CR2))

struct kvm_sev_info {
	bool active;		/* SEV enabled guest */
	bool es_active;		/* SEV-ES enabled guest */
	unsigned int asid;	/* ASID used for this guest */
	unsigned int handle;	/* SEV firmware handle */
	int fd;			/* SEV device fd */
	unsigned long pages_locked; /* Number of pages locked */
	struct list_head regions_list;  /* List of registered regions */
	u64 ap_jump_table;	/* SEV-ES AP Jump Table address */
	struct kvm *enc_context_owner; /* Owner of copied encryption context */
	struct misc_cg *misc_cg; /* For misc cgroup accounting */
};

struct kvm_svm {
	struct kvm kvm;

	/* Struct members for AVIC */
	u32 avic_vm_id;
	struct page *avic_logical_id_table_page;
	struct page *avic_physical_id_table_page;
	struct hlist_node hnode;

	struct kvm_sev_info sev_info;
};

struct kvm_vcpu;

struct kvm_vmcb_info {
	struct vmcb *ptr;
	unsigned long pa;
	int cpu;
	uint64_t asid_generation;
};

struct svm_nested_state {
	struct kvm_vmcb_info vmcb02;
	u64 hsave_msr;
	u64 vm_cr_msr;
	u64 vmcb12_gpa;
	u64 last_vmcb12_gpa;

	/* These are the merged vectors */
	u32 *msrpm;

	/* A VMRUN has started but has not yet been performed, so
	 * we cannot inject a nested vmexit yet.  */
	bool nested_run_pending;

	/* cache for control fields of the guest */
	struct vmcb_control_area ctl;

	bool initialized;
};

struct vcpu_svm {
	struct kvm_vcpu vcpu;
	/* vmcb always points at current_vmcb->ptr, it's purely a shorthand. */
	struct vmcb *vmcb;
	struct kvm_vmcb_info vmcb01;
	struct kvm_vmcb_info *current_vmcb;
	struct svm_cpu_data *svm_data;
	u32 asid;
	u32 sysenter_esp_hi;
	u32 sysenter_eip_hi;
	uint64_t tsc_aux;

	u64 msr_decfg;

	u64 next_rip;

	u64 spec_ctrl;
	/*
	 * Contains guest-controlled bits of VIRT_SPEC_CTRL, which will be
	 * translated into the appropriate L2_CFG bits on the host to
	 * perform speculative control.
	 */
	u64 virt_spec_ctrl;

	u32 *msrpm;

	ulong nmi_iret_rip;

	struct svm_nested_state nested;

	bool nmi_singlestep;
	u64 nmi_singlestep_guest_rflags;

	unsigned int3_injected;
	unsigned long int3_rip;

	/* cached guest cpuid flags for faster access */
	bool nrips_enabled	: 1;

	u32 ldr_reg;
	u32 dfr_reg;
	struct page *avic_backing_page;
	u64 *avic_physical_id_cache;
	bool avic_is_running;

	/*
	 * Per-vcpu list of struct amd_svm_iommu_ir:
	 * This is used mainly to store interrupt remapping information used
	 * when update the vcpu affinity. This avoids the need to scan for
	 * IRTE and try to match ga_tag in the IOMMU driver.
	 */
	struct list_head ir_list;
	spinlock_t ir_list_lock;

	/* Save desired MSR intercept (read: pass-through) state */
	struct {
		DECLARE_BITMAP(read, MAX_DIRECT_ACCESS_MSRS);
		DECLARE_BITMAP(write, MAX_DIRECT_ACCESS_MSRS);
	} shadow_msr_intercept;

	/* SEV-ES support */
	struct vmcb_save_area *vmsa;
	struct ghcb *ghcb;
	struct kvm_host_map ghcb_map;
	bool received_first_sipi;

	/* SEV-ES scratch area support */
	void *ghcb_sa;
	u64 ghcb_sa_len;
	bool ghcb_sa_sync;
	bool ghcb_sa_free;

	bool guest_state_loaded;
};

struct svm_cpu_data {
	int cpu;

	u64 asid_generation;
	u32 max_asid;
	u32 next_asid;
	u32 min_asid;
	struct kvm_ldttss_desc *tss_desc;

	struct page *save_area;
	struct vmcb *current_vmcb;

	/* index = sev_asid, value = vmcb pointer */
	struct vmcb **sev_vmcbs;
};

DECLARE_PER_CPU(struct svm_cpu_data *, svm_data);

void recalc_intercepts(struct vcpu_svm *svm);

static inline struct kvm_svm *to_kvm_svm(struct kvm *kvm)
{
	return container_of(kvm, struct kvm_svm, kvm);
}

static inline bool sev_guest(struct kvm *kvm)
{
#ifdef CONFIG_KVM_AMD_SEV
	struct kvm_sev_info *sev = &to_kvm_svm(kvm)->sev_info;

	return sev->active;
#else
	return false;
#endif
}

static inline bool sev_es_guest(struct kvm *kvm)
{
#ifdef CONFIG_KVM_AMD_SEV
	struct kvm_sev_info *sev = &to_kvm_svm(kvm)->sev_info;

	return sev_guest(kvm) && sev->es_active;
#else
	return false;
#endif
}

static inline void vmcb_mark_all_dirty(struct vmcb *vmcb)
{
	vmcb->control.clean = 0;
}

static inline void vmcb_mark_all_clean(struct vmcb *vmcb)
{
	vmcb->control.clean = ((1 << VMCB_DIRTY_MAX) - 1)
			       & ~VMCB_ALWAYS_DIRTY_MASK;
}

static inline void vmcb_mark_dirty(struct vmcb *vmcb, int bit)
{
	vmcb->control.clean &= ~(1 << bit);
}

static inline bool vmcb_is_dirty(struct vmcb *vmcb, int bit)
{
        return !test_bit(bit, (unsigned long *)&vmcb->control.clean);
}

static inline struct vcpu_svm *to_svm(struct kvm_vcpu *vcpu)
{
	return container_of(vcpu, struct vcpu_svm, vcpu);
}

static inline void vmcb_set_intercept(struct vmcb_control_area *control, u32 bit)
{
	WARN_ON_ONCE(bit >= 32 * MAX_INTERCEPT);
	__set_bit(bit, (unsigned long *)&control->intercepts);
}

static inline void vmcb_clr_intercept(struct vmcb_control_area *control, u32 bit)
{
	WARN_ON_ONCE(bit >= 32 * MAX_INTERCEPT);
	__clear_bit(bit, (unsigned long *)&control->intercepts);
}

static inline bool vmcb_is_intercept(struct vmcb_control_area *control, u32 bit)
{
	WARN_ON_ONCE(bit >= 32 * MAX_INTERCEPT);
	return test_bit(bit, (unsigned long *)&control->intercepts);
}

static inline void set_dr_intercepts(struct vcpu_svm *svm)
{
	struct vmcb *vmcb = svm->vmcb01.ptr;

	if (!sev_es_guest(svm->vcpu.kvm)) {
		vmcb_set_intercept(&vmcb->control, INTERCEPT_DR0_READ);
		vmcb_set_intercept(&vmcb->control, INTERCEPT_DR1_READ);
		vmcb_set_intercept(&vmcb->control, INTERCEPT_DR2_READ);
		vmcb_set_intercept(&vmcb->control, INTERCEPT_DR3_READ);
		vmcb_set_intercept(&vmcb->control, INTERCEPT_DR4_READ);
		vmcb_set_intercept(&vmcb->control, INTERCEPT_DR5_READ);
		vmcb_set_intercept(&vmcb->control, INTERCEPT_DR6_READ);
		vmcb_set_intercept(&vmcb->control, INTERCEPT_DR0_WRITE);
		vmcb_set_intercept(&vmcb->control, INTERCEPT_DR1_WRITE);
		vmcb_set_intercept(&vmcb->control, INTERCEPT_DR2_WRITE);
		vmcb_set_intercept(&vmcb->control, INTERCEPT_DR3_WRITE);
		vmcb_set_intercept(&vmcb->control, INTERCEPT_DR4_WRITE);
		vmcb_set_intercept(&vmcb->control, INTERCEPT_DR5_WRITE);
		vmcb_set_intercept(&vmcb->control, INTERCEPT_DR6_WRITE);
	}

	vmcb_set_intercept(&vmcb->control, INTERCEPT_DR7_READ);
	vmcb_set_intercept(&vmcb->control, INTERCEPT_DR7_WRITE);

	recalc_intercepts(svm);
}

static inline void clr_dr_intercepts(struct vcpu_svm *svm)
{
	struct vmcb *vmcb = svm->vmcb01.ptr;

	vmcb->control.intercepts[INTERCEPT_DR] = 0;

	/* DR7 access must remain intercepted for an SEV-ES guest */
	if (sev_es_guest(svm->vcpu.kvm)) {
		vmcb_set_intercept(&vmcb->control, INTERCEPT_DR7_READ);
		vmcb_set_intercept(&vmcb->control, INTERCEPT_DR7_WRITE);
	}

	recalc_intercepts(svm);
}

static inline void set_exception_intercept(struct vcpu_svm *svm, u32 bit)
{
	struct vmcb *vmcb = svm->vmcb01.ptr;

	WARN_ON_ONCE(bit >= 32);
	vmcb_set_intercept(&vmcb->control, INTERCEPT_EXCEPTION_OFFSET + bit);

	recalc_intercepts(svm);
}

static inline void clr_exception_intercept(struct vcpu_svm *svm, u32 bit)
{
	struct vmcb *vmcb = svm->vmcb01.ptr;

	WARN_ON_ONCE(bit >= 32);
	vmcb_clr_intercept(&vmcb->control, INTERCEPT_EXCEPTION_OFFSET + bit);

	recalc_intercepts(svm);
}

static inline void svm_set_intercept(struct vcpu_svm *svm, int bit)
{
	struct vmcb *vmcb = svm->vmcb01.ptr;

	vmcb_set_intercept(&vmcb->control, bit);

	recalc_intercepts(svm);
}

static inline void svm_clr_intercept(struct vcpu_svm *svm, int bit)
{
	struct vmcb *vmcb = svm->vmcb01.ptr;

	vmcb_clr_intercept(&vmcb->control, bit);

	recalc_intercepts(svm);
}

static inline bool svm_is_intercept(struct vcpu_svm *svm, int bit)
{
	return vmcb_is_intercept(&svm->vmcb->control, bit);
}

static inline bool vgif_enabled(struct vcpu_svm *svm)
{
	return !!(svm->vmcb->control.int_ctl & V_GIF_ENABLE_MASK);
}

static inline void enable_gif(struct vcpu_svm *svm)
{
	if (vgif_enabled(svm))
		svm->vmcb->control.int_ctl |= V_GIF_MASK;
	else
		svm->vcpu.arch.hflags |= HF_GIF_MASK;
}

static inline void disable_gif(struct vcpu_svm *svm)
{
	if (vgif_enabled(svm))
		svm->vmcb->control.int_ctl &= ~V_GIF_MASK;
	else
		svm->vcpu.arch.hflags &= ~HF_GIF_MASK;
}

static inline bool gif_set(struct vcpu_svm *svm)
{
	if (vgif_enabled(svm))
		return !!(svm->vmcb->control.int_ctl & V_GIF_MASK);
	else
		return !!(svm->vcpu.arch.hflags & HF_GIF_MASK);
}

/* svm.c */
#define MSR_INVALID				0xffffffffU

extern bool dump_invalid_vmcb;

u32 svm_msrpm_offset(u32 msr);
u32 *svm_vcpu_alloc_msrpm(void);
void svm_vcpu_init_msrpm(struct kvm_vcpu *vcpu, u32 *msrpm);
void svm_vcpu_free_msrpm(u32 *msrpm);

int svm_set_efer(struct kvm_vcpu *vcpu, u64 efer);
void svm_set_cr0(struct kvm_vcpu *vcpu, unsigned long cr0);
void svm_set_cr4(struct kvm_vcpu *vcpu, unsigned long cr4);
void svm_flush_tlb(struct kvm_vcpu *vcpu);
void disable_nmi_singlestep(struct vcpu_svm *svm);
bool svm_smi_blocked(struct kvm_vcpu *vcpu);
bool svm_nmi_blocked(struct kvm_vcpu *vcpu);
bool svm_interrupt_blocked(struct kvm_vcpu *vcpu);
void svm_set_gif(struct vcpu_svm *svm, bool value);
int svm_invoke_exit_handler(struct kvm_vcpu *vcpu, u64 exit_code);
void set_msr_interception(struct kvm_vcpu *vcpu, u32 *msrpm, u32 msr,
			  int read, int write);

/* nested.c */

#define NESTED_EXIT_HOST	0	/* Exit handled on host level */
#define NESTED_EXIT_DONE	1	/* Exit caused nested vmexit  */
#define NESTED_EXIT_CONTINUE	2	/* Further checks needed      */

static inline bool nested_svm_virtualize_tpr(struct kvm_vcpu *vcpu)
{
	struct vcpu_svm *svm = to_svm(vcpu);

	return is_guest_mode(vcpu) && (svm->nested.ctl.int_ctl & V_INTR_MASKING_MASK);
}

static inline bool nested_exit_on_smi(struct vcpu_svm *svm)
{
	return vmcb_is_intercept(&svm->nested.ctl, INTERCEPT_SMI);
}

static inline bool nested_exit_on_intr(struct vcpu_svm *svm)
{
	return vmcb_is_intercept(&svm->nested.ctl, INTERCEPT_INTR);
}

static inline bool nested_exit_on_nmi(struct vcpu_svm *svm)
{
	return vmcb_is_intercept(&svm->nested.ctl, INTERCEPT_NMI);
}

int enter_svm_guest_mode(struct kvm_vcpu *vcpu, u64 vmcb_gpa, struct vmcb *vmcb12);
void svm_leave_nested(struct vcpu_svm *svm);
void svm_free_nested(struct vcpu_svm *svm);
int svm_allocate_nested(struct vcpu_svm *svm);
int nested_svm_vmrun(struct kvm_vcpu *vcpu);
void nested_svm_vmloadsave(struct vmcb *from_vmcb, struct vmcb *to_vmcb);
int nested_svm_vmexit(struct vcpu_svm *svm);

static inline int nested_svm_simple_vmexit(struct vcpu_svm *svm, u32 exit_code)
{
	svm->vmcb->control.exit_code   = exit_code;
	svm->vmcb->control.exit_info_1 = 0;
	svm->vmcb->control.exit_info_2 = 0;
	return nested_svm_vmexit(svm);
}

int nested_svm_exit_handled(struct vcpu_svm *svm);
int nested_svm_check_permissions(struct kvm_vcpu *vcpu);
int nested_svm_check_exception(struct vcpu_svm *svm, unsigned nr,
			       bool has_error_code, u32 error_code);
int nested_svm_exit_special(struct vcpu_svm *svm);
void nested_sync_control_from_vmcb02(struct vcpu_svm *svm);
void nested_vmcb02_compute_g_pat(struct vcpu_svm *svm);
void svm_switch_vmcb(struct vcpu_svm *svm, struct kvm_vmcb_info *target_vmcb);

extern struct kvm_x86_nested_ops svm_nested_ops;

/* avic.c */

#define AVIC_LOGICAL_ID_ENTRY_GUEST_PHYSICAL_ID_MASK	(0xFF)
#define AVIC_LOGICAL_ID_ENTRY_VALID_BIT			31
#define AVIC_LOGICAL_ID_ENTRY_VALID_MASK		(1 << 31)

#define AVIC_PHYSICAL_ID_ENTRY_HOST_PHYSICAL_ID_MASK	(0xFFULL)
#define AVIC_PHYSICAL_ID_ENTRY_BACKING_PAGE_MASK	(0xFFFFFFFFFFULL << 12)
#define AVIC_PHYSICAL_ID_ENTRY_IS_RUNNING_MASK		(1ULL << 62)
#define AVIC_PHYSICAL_ID_ENTRY_VALID_MASK		(1ULL << 63)

#define VMCB_AVIC_APIC_BAR_MASK		0xFFFFFFFFFF000ULL

extern bool avic;

static inline void avic_update_vapic_bar(struct vcpu_svm *svm, u64 data)
{
	svm->vmcb->control.avic_vapic_bar = data & VMCB_AVIC_APIC_BAR_MASK;
	vmcb_mark_dirty(svm->vmcb, VMCB_AVIC);
}

static inline bool avic_vcpu_is_running(struct kvm_vcpu *vcpu)
{
	struct vcpu_svm *svm = to_svm(vcpu);
	u64 *entry = svm->avic_physical_id_cache;

	if (!entry)
		return false;

	return (READ_ONCE(*entry) & AVIC_PHYSICAL_ID_ENTRY_IS_RUNNING_MASK);
}

int avic_ga_log_notifier(u32 ga_tag);
void avic_vm_destroy(struct kvm *kvm);
int avic_vm_init(struct kvm *kvm);
void avic_init_vmcb(struct vcpu_svm *svm);
void svm_toggle_avic_for_irq_window(struct kvm_vcpu *vcpu, bool activate);
int avic_incomplete_ipi_interception(struct kvm_vcpu *vcpu);
int avic_unaccelerated_access_interception(struct kvm_vcpu *vcpu);
int avic_init_vcpu(struct vcpu_svm *svm);
void avic_vcpu_load(struct kvm_vcpu *vcpu, int cpu);
void avic_vcpu_put(struct kvm_vcpu *vcpu);
void avic_post_state_restore(struct kvm_vcpu *vcpu);
void svm_set_virtual_apic_mode(struct kvm_vcpu *vcpu);
void svm_refresh_apicv_exec_ctrl(struct kvm_vcpu *vcpu);
bool svm_check_apicv_inhibit_reasons(ulong bit);
void svm_pre_update_apicv_exec_ctrl(struct kvm *kvm, bool activate);
void svm_load_eoi_exitmap(struct kvm_vcpu *vcpu, u64 *eoi_exit_bitmap);
void svm_hwapic_irr_update(struct kvm_vcpu *vcpu, int max_irr);
void svm_hwapic_isr_update(struct kvm_vcpu *vcpu, int max_isr);
int svm_deliver_avic_intr(struct kvm_vcpu *vcpu, int vec);
bool svm_dy_apicv_has_pending_interrupt(struct kvm_vcpu *vcpu);
int svm_update_pi_irte(struct kvm *kvm, unsigned int host_irq,
		       uint32_t guest_irq, bool set);
void svm_vcpu_blocking(struct kvm_vcpu *vcpu);
void svm_vcpu_unblocking(struct kvm_vcpu *vcpu);

/* sev.c */

#define GHCB_VERSION_MAX	1ULL
#define GHCB_VERSION_MIN	1ULL


extern unsigned int max_sev_asid;

void sev_vm_destroy(struct kvm *kvm);
int svm_mem_enc_op(struct kvm *kvm, void __user *argp);
int svm_register_enc_region(struct kvm *kvm,
			    struct kvm_enc_region *range);
int svm_unregister_enc_region(struct kvm *kvm,
			      struct kvm_enc_region *range);
int svm_vm_copy_asid_from(struct kvm *kvm, unsigned int source_fd);
void pre_sev_run(struct vcpu_svm *svm, int cpu);
void __init sev_set_cpu_caps(void);
void __init sev_hardware_setup(void);
void sev_hardware_teardown(void);
int sev_cpu_init(struct svm_cpu_data *sd);
void sev_free_vcpu(struct kvm_vcpu *vcpu);
int sev_handle_vmgexit(struct kvm_vcpu *vcpu);
int sev_es_string_io(struct vcpu_svm *svm, int size, unsigned int port, int in);
void sev_es_init_vmcb(struct vcpu_svm *svm);
void sev_es_create_vcpu(struct vcpu_svm *svm);
void sev_vcpu_deliver_sipi_vector(struct kvm_vcpu *vcpu, u8 vector);
void sev_es_prepare_guest_switch(struct vcpu_svm *svm, unsigned int cpu);
void sev_es_unmap_ghcb(struct vcpu_svm *svm);

/* vmenter.S */

void __svm_sev_es_vcpu_run(unsigned long vmcb_pa);
void __svm_vcpu_run(unsigned long vmcb_pa, unsigned long *regs);

#endif
