// SPDX-License-Identifier: GPL-2.0-only
/*
 * Kernel-based Virtual Machine driver for Linux
 *
 * This module enables machines with Intel VT-x extensions to run virtual
 * machines without emulation or binary translation.
 *
 * Copyright (C) 2006 Qumranet, Inc.
 * Copyright 2010 Red Hat, Inc. and/or its affiliates.
 *
 * Authors:
 *   Avi Kivity   <avi@qumranet.com>
 *   Yaniv Kamay  <yaniv@qumranet.com>
 */

#include <linux/highmem.h>
#include <linux/hrtimer.h>
#include <linux/kernel.h>
#include <linux/kvm_host.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/mod_devicetable.h>
#include <linux/mm.h>
#include <linux/objtool.h>
#include <linux/sched.h>
#include <linux/sched/smt.h>
#include <linux/slab.h>
#include <linux/tboot.h>
#include <linux/trace_events.h>
#include <linux/entry-kvm.h>

#include <asm/apic.h>
#include <asm/asm.h>
#include <asm/cpu.h>
#include <asm/cpu_device_id.h>
#include <asm/debugreg.h>
#include <asm/desc.h>
#include <asm/fpu/internal.h>
#include <asm/idtentry.h>
#include <asm/io.h>
#include <asm/irq_remapping.h>
#include <asm/kexec.h>
#include <asm/perf_event.h>
#include <asm/mmu_context.h>
#include <asm/mshyperv.h>
#include <asm/mwait.h>
#include <asm/spec-ctrl.h>
#include <asm/virtext.h>
#include <asm/vmx.h>

#include "capabilities.h"
#include "cpuid.h"
#include "evmcs.h"
#include "hyperv.h"
#include "irq.h"
#include "kvm_cache_regs.h"
#include "lapic.h"
#include "mmu.h"
#include "nested.h"
#include "pmu.h"
#include "sgx.h"
#include "trace.h"
#include "vmcs.h"
#include "vmcs12.h"
#include "vmx.h"
#include "x86.h"

MODULE_AUTHOR("Qumranet");
MODULE_LICENSE("GPL");

#ifdef MODULE
static const struct x86_cpu_id vmx_cpu_id[] = {
	X86_MATCH_FEATURE(X86_FEATURE_VMX, NULL),
	{}
};
MODULE_DEVICE_TABLE(x86cpu, vmx_cpu_id);
#endif

bool __read_mostly enable_vpid = 1;
module_param_named(vpid, enable_vpid, bool, 0444);

static bool __read_mostly enable_vnmi = 1;
module_param_named(vnmi, enable_vnmi, bool, S_IRUGO);

bool __read_mostly flexpriority_enabled = 1;
module_param_named(flexpriority, flexpriority_enabled, bool, S_IRUGO);

bool __read_mostly enable_ept = 1;
module_param_named(ept, enable_ept, bool, S_IRUGO);

bool __read_mostly enable_unrestricted_guest = 1;
module_param_named(unrestricted_guest,
			enable_unrestricted_guest, bool, S_IRUGO);

bool __read_mostly enable_ept_ad_bits = 1;
module_param_named(eptad, enable_ept_ad_bits, bool, S_IRUGO);

static bool __read_mostly emulate_invalid_guest_state = true;
module_param(emulate_invalid_guest_state, bool, S_IRUGO);

static bool __read_mostly fasteoi = 1;
module_param(fasteoi, bool, S_IRUGO);

bool __read_mostly enable_apicv = 1;
module_param(enable_apicv, bool, S_IRUGO);

/*
 * If nested=1, nested virtualization is supported, i.e., guests may use
 * VMX and be a hypervisor for its own guests. If nested=0, guests may not
 * use VMX instructions.
 */
static bool __read_mostly nested = 1;
module_param(nested, bool, S_IRUGO);

bool __read_mostly enable_pml = 1;
module_param_named(pml, enable_pml, bool, S_IRUGO);

static bool __read_mostly dump_invalid_vmcs = 0;
module_param(dump_invalid_vmcs, bool, 0644);

#define MSR_BITMAP_MODE_X2APIC		1
#define MSR_BITMAP_MODE_X2APIC_APICV	2

#define KVM_VMX_TSC_MULTIPLIER_MAX     0xffffffffffffffffULL

/* Guest_tsc -> host_tsc conversion requires 64-bit division.  */
static int __read_mostly cpu_preemption_timer_multi;
static bool __read_mostly enable_preemption_timer = 1;
#ifdef CONFIG_X86_64
module_param_named(preemption_timer, enable_preemption_timer, bool, S_IRUGO);
#endif

extern bool __read_mostly allow_smaller_maxphyaddr;
module_param(allow_smaller_maxphyaddr, bool, S_IRUGO);

#define KVM_VM_CR0_ALWAYS_OFF (X86_CR0_NW | X86_CR0_CD)
#define KVM_VM_CR0_ALWAYS_ON_UNRESTRICTED_GUEST X86_CR0_NE
#define KVM_VM_CR0_ALWAYS_ON				\
	(KVM_VM_CR0_ALWAYS_ON_UNRESTRICTED_GUEST | 	\
	 X86_CR0_WP | X86_CR0_PG | X86_CR0_PE)

#define KVM_VM_CR4_ALWAYS_ON_UNRESTRICTED_GUEST X86_CR4_VMXE
#define KVM_PMODE_VM_CR4_ALWAYS_ON (X86_CR4_PAE | X86_CR4_VMXE)
#define KVM_RMODE_VM_CR4_ALWAYS_ON (X86_CR4_VME | X86_CR4_PAE | X86_CR4_VMXE)

#define RMODE_GUEST_OWNED_EFLAGS_BITS (~(X86_EFLAGS_IOPL | X86_EFLAGS_VM))

#define MSR_IA32_RTIT_STATUS_MASK (~(RTIT_STATUS_FILTEREN | \
	RTIT_STATUS_CONTEXTEN | RTIT_STATUS_TRIGGEREN | \
	RTIT_STATUS_ERROR | RTIT_STATUS_STOPPED | \
	RTIT_STATUS_BYTECNT))

/*
 * List of MSRs that can be directly passed to the guest.
 * In addition to these x2apic and PT MSRs are handled specially.
 */
static u32 vmx_possible_passthrough_msrs[MAX_POSSIBLE_PASSTHROUGH_MSRS] = {
	MSR_IA32_SPEC_CTRL,
	MSR_IA32_PRED_CMD,
	MSR_IA32_TSC,
#ifdef CONFIG_X86_64
	MSR_FS_BASE,
	MSR_GS_BASE,
	MSR_KERNEL_GS_BASE,
#endif
	MSR_IA32_SYSENTER_CS,
	MSR_IA32_SYSENTER_ESP,
	MSR_IA32_SYSENTER_EIP,
	MSR_CORE_C1_RES,
	MSR_CORE_C3_RESIDENCY,
	MSR_CORE_C6_RESIDENCY,
	MSR_CORE_C7_RESIDENCY,
};

/*
 * These 2 parameters are used to config the controls for Pause-Loop Exiting:
 * ple_gap:    upper bound on the amount of time between two successive
 *             executions of PAUSE in a loop. Also indicate if ple enabled.
 *             According to test, this time is usually smaller than 128 cycles.
 * ple_window: upper bound on the amount of time a guest is allowed to execute
 *             in a PAUSE loop. Tests indicate that most spinlocks are held for
 *             less than 2^12 cycles
 * Time is measured based on a counter that runs at the same rate as the TSC,
 * refer SDM volume 3b section 21.6.13 & 22.1.3.
 */
static unsigned int ple_gap = KVM_DEFAULT_PLE_GAP;
module_param(ple_gap, uint, 0444);

static unsigned int ple_window = KVM_VMX_DEFAULT_PLE_WINDOW;
module_param(ple_window, uint, 0444);

/* Default doubles per-vcpu window every exit. */
static unsigned int ple_window_grow = KVM_DEFAULT_PLE_WINDOW_GROW;
module_param(ple_window_grow, uint, 0444);

/* Default resets per-vcpu window every exit to ple_window. */
static unsigned int ple_window_shrink = KVM_DEFAULT_PLE_WINDOW_SHRINK;
module_param(ple_window_shrink, uint, 0444);

/* Default is to compute the maximum so we can never overflow. */
static unsigned int ple_window_max        = KVM_VMX_DEFAULT_PLE_WINDOW_MAX;
module_param(ple_window_max, uint, 0444);

/* Default is SYSTEM mode, 1 for host-guest mode */
int __read_mostly pt_mode = PT_MODE_SYSTEM;
module_param(pt_mode, int, S_IRUGO);

static DEFINE_STATIC_KEY_FALSE(vmx_l1d_should_flush);
static DEFINE_STATIC_KEY_FALSE(vmx_l1d_flush_cond);
static DEFINE_MUTEX(vmx_l1d_flush_mutex);

/* Storage for pre module init parameter parsing */
static enum vmx_l1d_flush_state __read_mostly vmentry_l1d_flush_param = VMENTER_L1D_FLUSH_AUTO;

static const struct {
	const char *option;
	bool for_parse;
} vmentry_l1d_param[] = {
	[VMENTER_L1D_FLUSH_AUTO]	 = {"auto", true},
	[VMENTER_L1D_FLUSH_NEVER]	 = {"never", true},
	[VMENTER_L1D_FLUSH_COND]	 = {"cond", true},
	[VMENTER_L1D_FLUSH_ALWAYS]	 = {"always", true},
	[VMENTER_L1D_FLUSH_EPT_DISABLED] = {"EPT disabled", false},
	[VMENTER_L1D_FLUSH_NOT_REQUIRED] = {"not required", false},
};

#define L1D_CACHE_ORDER 4
static void *vmx_l1d_flush_pages;

static int vmx_setup_l1d_flush(enum vmx_l1d_flush_state l1tf)
{
	struct page *page;
	unsigned int i;

	if (!boot_cpu_has_bug(X86_BUG_L1TF)) {
		l1tf_vmx_mitigation = VMENTER_L1D_FLUSH_NOT_REQUIRED;
		return 0;
	}

	if (!enable_ept) {
		l1tf_vmx_mitigation = VMENTER_L1D_FLUSH_EPT_DISABLED;
		return 0;
	}

	if (boot_cpu_has(X86_FEATURE_ARCH_CAPABILITIES)) {
		u64 msr;

		rdmsrl(MSR_IA32_ARCH_CAPABILITIES, msr);
		if (msr & ARCH_CAP_SKIP_VMENTRY_L1DFLUSH) {
			l1tf_vmx_mitigation = VMENTER_L1D_FLUSH_NOT_REQUIRED;
			return 0;
		}
	}

	/* If set to auto use the default l1tf mitigation method */
	if (l1tf == VMENTER_L1D_FLUSH_AUTO) {
		switch (l1tf_mitigation) {
		case L1TF_MITIGATION_OFF:
			l1tf = VMENTER_L1D_FLUSH_NEVER;
			break;
		case L1TF_MITIGATION_FLUSH_NOWARN:
		case L1TF_MITIGATION_FLUSH:
		case L1TF_MITIGATION_FLUSH_NOSMT:
			l1tf = VMENTER_L1D_FLUSH_COND;
			break;
		case L1TF_MITIGATION_FULL:
		case L1TF_MITIGATION_FULL_FORCE:
			l1tf = VMENTER_L1D_FLUSH_ALWAYS;
			break;
		}
	} else if (l1tf_mitigation == L1TF_MITIGATION_FULL_FORCE) {
		l1tf = VMENTER_L1D_FLUSH_ALWAYS;
	}

	if (l1tf != VMENTER_L1D_FLUSH_NEVER && !vmx_l1d_flush_pages &&
	    !boot_cpu_has(X86_FEATURE_FLUSH_L1D)) {
		/*
		 * This allocation for vmx_l1d_flush_pages is not tied to a VM
		 * lifetime and so should not be charged to a memcg.
		 */
		page = alloc_pages(GFP_KERNEL, L1D_CACHE_ORDER);
		if (!page)
			return -ENOMEM;
		vmx_l1d_flush_pages = page_address(page);

		/*
		 * Initialize each page with a different pattern in
		 * order to protect against KSM in the nested
		 * virtualization case.
		 */
		for (i = 0; i < 1u << L1D_CACHE_ORDER; ++i) {
			memset(vmx_l1d_flush_pages + i * PAGE_SIZE, i + 1,
			       PAGE_SIZE);
		}
	}

	l1tf_vmx_mitigation = l1tf;

	if (l1tf != VMENTER_L1D_FLUSH_NEVER)
		static_branch_enable(&vmx_l1d_should_flush);
	else
		static_branch_disable(&vmx_l1d_should_flush);

	if (l1tf == VMENTER_L1D_FLUSH_COND)
		static_branch_enable(&vmx_l1d_flush_cond);
	else
		static_branch_disable(&vmx_l1d_flush_cond);
	return 0;
}

static int vmentry_l1d_flush_parse(const char *s)
{
	unsigned int i;

	if (s) {
		for (i = 0; i < ARRAY_SIZE(vmentry_l1d_param); i++) {
			if (vmentry_l1d_param[i].for_parse &&
			    sysfs_streq(s, vmentry_l1d_param[i].option))
				return i;
		}
	}
	return -EINVAL;
}

static int vmentry_l1d_flush_set(const char *s, const struct kernel_param *kp)
{
	int l1tf, ret;

	l1tf = vmentry_l1d_flush_parse(s);
	if (l1tf < 0)
		return l1tf;

	if (!boot_cpu_has(X86_BUG_L1TF))
		return 0;

	/*
	 * Has vmx_init() run already? If not then this is the pre init
	 * parameter parsing. In that case just store the value and let
	 * vmx_init() do the proper setup after enable_ept has been
	 * established.
	 */
	if (l1tf_vmx_mitigation == VMENTER_L1D_FLUSH_AUTO) {
		vmentry_l1d_flush_param = l1tf;
		return 0;
	}

	mutex_lock(&vmx_l1d_flush_mutex);
	ret = vmx_setup_l1d_flush(l1tf);
	mutex_unlock(&vmx_l1d_flush_mutex);
	return ret;
}

static int vmentry_l1d_flush_get(char *s, const struct kernel_param *kp)
{
	if (WARN_ON_ONCE(l1tf_vmx_mitigation >= ARRAY_SIZE(vmentry_l1d_param)))
		return sprintf(s, "???\n");

	return sprintf(s, "%s\n", vmentry_l1d_param[l1tf_vmx_mitigation].option);
}

static const struct kernel_param_ops vmentry_l1d_flush_ops = {
	.set = vmentry_l1d_flush_set,
	.get = vmentry_l1d_flush_get,
};
module_param_cb(vmentry_l1d_flush, &vmentry_l1d_flush_ops, NULL, 0644);

static u32 vmx_segment_access_rights(struct kvm_segment *var);

void vmx_vmexit(void);

#define vmx_insn_failed(fmt...)		\
do {					\
	WARN_ONCE(1, fmt);		\
	pr_warn_ratelimited(fmt);	\
} while (0)

asmlinkage void vmread_error(unsigned long field, bool fault)
{
	if (fault)
		kvm_spurious_fault();
	else
		vmx_insn_failed("kvm: vmread failed: field=%lx\n", field);
}

noinline void vmwrite_error(unsigned long field, unsigned long value)
{
	vmx_insn_failed("kvm: vmwrite failed: field=%lx val=%lx err=%d\n",
			field, value, vmcs_read32(VM_INSTRUCTION_ERROR));
}

noinline void vmclear_error(struct vmcs *vmcs, u64 phys_addr)
{
	vmx_insn_failed("kvm: vmclear failed: %p/%llx\n", vmcs, phys_addr);
}

noinline void vmptrld_error(struct vmcs *vmcs, u64 phys_addr)
{
	vmx_insn_failed("kvm: vmptrld failed: %p/%llx\n", vmcs, phys_addr);
}

noinline void invvpid_error(unsigned long ext, u16 vpid, gva_t gva)
{
	vmx_insn_failed("kvm: invvpid failed: ext=0x%lx vpid=%u gva=0x%lx\n",
			ext, vpid, gva);
}

noinline void invept_error(unsigned long ext, u64 eptp, gpa_t gpa)
{
	vmx_insn_failed("kvm: invept failed: ext=0x%lx eptp=%llx gpa=0x%llx\n",
			ext, eptp, gpa);
}

static DEFINE_PER_CPU(struct vmcs *, vmxarea);
DEFINE_PER_CPU(struct vmcs *, current_vmcs);
/*
 * We maintain a per-CPU linked-list of VMCS loaded on that CPU. This is needed
 * when a CPU is brought down, and we need to VMCLEAR all VMCSs loaded on it.
 */
static DEFINE_PER_CPU(struct list_head, loaded_vmcss_on_cpu);

static DECLARE_BITMAP(vmx_vpid_bitmap, VMX_NR_VPIDS);
static DEFINE_SPINLOCK(vmx_vpid_lock);

struct vmcs_config vmcs_config;
struct vmx_capability vmx_capability;

#define VMX_SEGMENT_FIELD(seg)					\
	[VCPU_SREG_##seg] = {                                   \
		.selector = GUEST_##seg##_SELECTOR,		\
		.base = GUEST_##seg##_BASE,		   	\
		.limit = GUEST_##seg##_LIMIT,		   	\
		.ar_bytes = GUEST_##seg##_AR_BYTES,	   	\
	}

static const struct kvm_vmx_segment_field {
	unsigned selector;
	unsigned base;
	unsigned limit;
	unsigned ar_bytes;
} kvm_vmx_segment_fields[] = {
	VMX_SEGMENT_FIELD(CS),
	VMX_SEGMENT_FIELD(DS),
	VMX_SEGMENT_FIELD(ES),
	VMX_SEGMENT_FIELD(FS),
	VMX_SEGMENT_FIELD(GS),
	VMX_SEGMENT_FIELD(SS),
	VMX_SEGMENT_FIELD(TR),
	VMX_SEGMENT_FIELD(LDTR),
};

static inline void vmx_segment_cache_clear(struct vcpu_vmx *vmx)
{
	vmx->segment_cache.bitmask = 0;
}

static unsigned long host_idt_base;

#if IS_ENABLED(CONFIG_HYPERV)
static bool __read_mostly enlightened_vmcs = true;
module_param(enlightened_vmcs, bool, 0444);

static int kvm_fill_hv_flush_list_func(struct hv_guest_mapping_flush_list *flush,
		void *data)
{
	struct kvm_tlb_range *range = data;

	return hyperv_fill_flush_guest_mapping_list(flush, range->start_gfn,
			range->pages);
}

static inline int hv_remote_flush_root_ept(hpa_t root_ept,
					   struct kvm_tlb_range *range)
{
	if (range)
		return hyperv_flush_guest_mapping_range(root_ept,
				kvm_fill_hv_flush_list_func, (void *)range);
	else
		return hyperv_flush_guest_mapping(root_ept);
}

static int hv_remote_flush_tlb_with_range(struct kvm *kvm,
		struct kvm_tlb_range *range)
{
	struct kvm_vmx *kvm_vmx = to_kvm_vmx(kvm);
	struct kvm_vcpu *vcpu;
	int ret = 0, i, nr_unique_valid_roots;
	hpa_t root;

	spin_lock(&kvm_vmx->hv_root_ept_lock);

	if (!VALID_PAGE(kvm_vmx->hv_root_ept)) {
		nr_unique_valid_roots = 0;

		/*
		 * Flush all valid roots, and see if all vCPUs have converged
		 * on a common root, in which case future flushes can skip the
		 * loop and flush the common root.
		 */
		kvm_for_each_vcpu(i, vcpu, kvm) {
			root = to_vmx(vcpu)->hv_root_ept;
			if (!VALID_PAGE(root) || root == kvm_vmx->hv_root_ept)
				continue;

			/*
			 * Set the tracked root to the first valid root.  Keep
			 * this root for the entirety of the loop even if more
			 * roots are encountered as a low effort optimization
			 * to avoid flushing the same (first) root again.
			 */
			if (++nr_unique_valid_roots == 1)
				kvm_vmx->hv_root_ept = root;

			if (!ret)
				ret = hv_remote_flush_root_ept(root, range);

			/*
			 * Stop processing roots if a failure occurred and
			 * multiple valid roots have already been detected.
			 */
			if (ret && nr_unique_valid_roots > 1)
				break;
		}

		/*
		 * The optimized flush of a single root can't be used if there
		 * are multiple valid roots (obviously).
		 */
		if (nr_unique_valid_roots > 1)
			kvm_vmx->hv_root_ept = INVALID_PAGE;
	} else {
		ret = hv_remote_flush_root_ept(kvm_vmx->hv_root_ept, range);
	}

	spin_unlock(&kvm_vmx->hv_root_ept_lock);
	return ret;
}
static int hv_remote_flush_tlb(struct kvm *kvm)
{
	return hv_remote_flush_tlb_with_range(kvm, NULL);
}

static int hv_enable_direct_tlbflush(struct kvm_vcpu *vcpu)
{
	struct hv_enlightened_vmcs *evmcs;
	struct hv_partition_assist_pg **p_hv_pa_pg =
			&to_kvm_hv(vcpu->kvm)->hv_pa_pg;
	/*
	 * Synthetic VM-Exit is not enabled in current code and so All
	 * evmcs in singe VM shares same assist page.
	 */
	if (!*p_hv_pa_pg)
		*p_hv_pa_pg = kzalloc(PAGE_SIZE, GFP_KERNEL_ACCOUNT);

	if (!*p_hv_pa_pg)
		return -ENOMEM;

	evmcs = (struct hv_enlightened_vmcs *)to_vmx(vcpu)->loaded_vmcs->vmcs;

	evmcs->partition_assist_page =
		__pa(*p_hv_pa_pg);
	evmcs->hv_vm_id = (unsigned long)vcpu->kvm;
	evmcs->hv_enlightenments_control.nested_flush_hypercall = 1;

	return 0;
}

#endif /* IS_ENABLED(CONFIG_HYPERV) */

static void hv_track_root_ept(struct kvm_vcpu *vcpu, hpa_t root_ept)
{
#if IS_ENABLED(CONFIG_HYPERV)
	struct kvm_vmx *kvm_vmx = to_kvm_vmx(vcpu->kvm);

	if (kvm_x86_ops.tlb_remote_flush == hv_remote_flush_tlb) {
		spin_lock(&kvm_vmx->hv_root_ept_lock);
		to_vmx(vcpu)->hv_root_ept = root_ept;
		if (root_ept != kvm_vmx->hv_root_ept)
			kvm_vmx->hv_root_ept = INVALID_PAGE;
		spin_unlock(&kvm_vmx->hv_root_ept_lock);
	}
#endif
}

/*
 * Comment's format: document - errata name - stepping - processor name.
 * Refer from
 * https://www.virtualbox.org/svn/vbox/trunk/src/VBox/VMM/VMMR0/HMR0.cpp
 */
static u32 vmx_preemption_cpu_tfms[] = {
/* 323344.pdf - BA86   - D0 - Xeon 7500 Series */
0x000206E6,
/* 323056.pdf - AAX65  - C2 - Xeon L3406 */
/* 322814.pdf - AAT59  - C2 - i7-600, i5-500, i5-400 and i3-300 Mobile */
/* 322911.pdf - AAU65  - C2 - i5-600, i3-500 Desktop and Pentium G6950 */
0x00020652,
/* 322911.pdf - AAU65  - K0 - i5-600, i3-500 Desktop and Pentium G6950 */
0x00020655,
/* 322373.pdf - AAO95  - B1 - Xeon 3400 Series */
/* 322166.pdf - AAN92  - B1 - i7-800 and i5-700 Desktop */
/*
 * 320767.pdf - AAP86  - B1 -
 * i7-900 Mobile Extreme, i7-800 and i7-700 Mobile
 */
0x000106E5,
/* 321333.pdf - AAM126 - C0 - Xeon 3500 */
0x000106A0,
/* 321333.pdf - AAM126 - C1 - Xeon 3500 */
0x000106A1,
/* 320836.pdf - AAJ124 - C0 - i7-900 Desktop Extreme and i7-900 Desktop */
0x000106A4,
 /* 321333.pdf - AAM126 - D0 - Xeon 3500 */
 /* 321324.pdf - AAK139 - D0 - Xeon 5500 */
 /* 320836.pdf - AAJ124 - D0 - i7-900 Extreme and i7-900 Desktop */
0x000106A5,
 /* Xeon E3-1220 V2 */
0x000306A8,
};

static inline bool cpu_has_broken_vmx_preemption_timer(void)
{
	u32 eax = cpuid_eax(0x00000001), i;

	/* Clear the reserved bits */
	eax &= ~(0x3U << 14 | 0xfU << 28);
	for (i = 0; i < ARRAY_SIZE(vmx_preemption_cpu_tfms); i++)
		if (eax == vmx_preemption_cpu_tfms[i])
			return true;

	return false;
}

static inline bool cpu_need_virtualize_apic_accesses(struct kvm_vcpu *vcpu)
{
	return flexpriority_enabled && lapic_in_kernel(vcpu);
}

static inline bool report_flexpriority(void)
{
	return flexpriority_enabled;
}

static int possible_passthrough_msr_slot(u32 msr)
{
	u32 i;

	for (i = 0; i < ARRAY_SIZE(vmx_possible_passthrough_msrs); i++)
		if (vmx_possible_passthrough_msrs[i] == msr)
			return i;

	return -ENOENT;
}

static bool is_valid_passthrough_msr(u32 msr)
{
	bool r;

	switch (msr) {
	case 0x800 ... 0x8ff:
		/* x2APIC MSRs. These are handled in vmx_update_msr_bitmap_x2apic() */
		return true;
	case MSR_IA32_RTIT_STATUS:
	case MSR_IA32_RTIT_OUTPUT_BASE:
	case MSR_IA32_RTIT_OUTPUT_MASK:
	case MSR_IA32_RTIT_CR3_MATCH:
	case MSR_IA32_RTIT_ADDR0_A ... MSR_IA32_RTIT_ADDR3_B:
		/* PT MSRs. These are handled in pt_update_intercept_for_msr() */
	case MSR_LBR_SELECT:
	case MSR_LBR_TOS:
	case MSR_LBR_INFO_0 ... MSR_LBR_INFO_0 + 31:
	case MSR_LBR_NHM_FROM ... MSR_LBR_NHM_FROM + 31:
	case MSR_LBR_NHM_TO ... MSR_LBR_NHM_TO + 31:
	case MSR_LBR_CORE_FROM ... MSR_LBR_CORE_FROM + 8:
	case MSR_LBR_CORE_TO ... MSR_LBR_CORE_TO + 8:
		/* LBR MSRs. These are handled in vmx_update_intercept_for_lbr_msrs() */
		return true;
	}

	r = possible_passthrough_msr_slot(msr) != -ENOENT;

	WARN(!r, "Invalid MSR %x, please adapt vmx_possible_passthrough_msrs[]", msr);

	return r;
}

struct vmx_uret_msr *vmx_find_uret_msr(struct vcpu_vmx *vmx, u32 msr)
{
	int i;

	i = kvm_find_user_return_msr(msr);
	if (i >= 0)
		return &vmx->guest_uret_msrs[i];
	return NULL;
}

static int vmx_set_guest_uret_msr(struct vcpu_vmx *vmx,
				  struct vmx_uret_msr *msr, u64 data)
{
	unsigned int slot = msr - vmx->guest_uret_msrs;
	int ret = 0;

	u64 old_msr_data = msr->data;
	msr->data = data;
	if (msr->load_into_hardware) {
		preempt_disable();
		ret = kvm_set_user_return_msr(slot, msr->data, msr->mask);
		preempt_enable();
		if (ret)
			msr->data = old_msr_data;
	}
	return ret;
}

#ifdef CONFIG_KEXEC_CORE
static void crash_vmclear_local_loaded_vmcss(void)
{
	int cpu = raw_smp_processor_id();
	struct loaded_vmcs *v;

	list_for_each_entry(v, &per_cpu(loaded_vmcss_on_cpu, cpu),
			    loaded_vmcss_on_cpu_link)
		vmcs_clear(v->vmcs);
}
#endif /* CONFIG_KEXEC_CORE */

static void __loaded_vmcs_clear(void *arg)
{
	struct loaded_vmcs *loaded_vmcs = arg;
	int cpu = raw_smp_processor_id();

	if (loaded_vmcs->cpu != cpu)
		return; /* vcpu migration can race with cpu offline */
	if (per_cpu(current_vmcs, cpu) == loaded_vmcs->vmcs)
		per_cpu(current_vmcs, cpu) = NULL;

	vmcs_clear(loaded_vmcs->vmcs);
	if (loaded_vmcs->shadow_vmcs && loaded_vmcs->launched)
		vmcs_clear(loaded_vmcs->shadow_vmcs);

	list_del(&loaded_vmcs->loaded_vmcss_on_cpu_link);

	/*
	 * Ensure all writes to loaded_vmcs, including deleting it from its
	 * current percpu list, complete before setting loaded_vmcs->vcpu to
	 * -1, otherwise a different cpu can see vcpu == -1 first and add
	 * loaded_vmcs to its percpu list before it's deleted from this cpu's
	 * list. Pairs with the smp_rmb() in vmx_vcpu_load_vmcs().
	 */
	smp_wmb();

	loaded_vmcs->cpu = -1;
	loaded_vmcs->launched = 0;
}

void loaded_vmcs_clear(struct loaded_vmcs *loaded_vmcs)
{
	int cpu = loaded_vmcs->cpu;

	if (cpu != -1)
		smp_call_function_single(cpu,
			 __loaded_vmcs_clear, loaded_vmcs, 1);
}

static bool vmx_segment_cache_test_set(struct vcpu_vmx *vmx, unsigned seg,
				       unsigned field)
{
	bool ret;
	u32 mask = 1 << (seg * SEG_FIELD_NR + field);

	if (!kvm_register_is_available(&vmx->vcpu, VCPU_EXREG_SEGMENTS)) {
		kvm_register_mark_available(&vmx->vcpu, VCPU_EXREG_SEGMENTS);
		vmx->segment_cache.bitmask = 0;
	}
	ret = vmx->segment_cache.bitmask & mask;
	vmx->segment_cache.bitmask |= mask;
	return ret;
}

static u16 vmx_read_guest_seg_selector(struct vcpu_vmx *vmx, unsigned seg)
{
	u16 *p = &vmx->segment_cache.seg[seg].selector;

	if (!vmx_segment_cache_test_set(vmx, seg, SEG_FIELD_SEL))
		*p = vmcs_read16(kvm_vmx_segment_fields[seg].selector);
	return *p;
}

static ulong vmx_read_guest_seg_base(struct vcpu_vmx *vmx, unsigned seg)
{
	ulong *p = &vmx->segment_cache.seg[seg].base;

	if (!vmx_segment_cache_test_set(vmx, seg, SEG_FIELD_BASE))
		*p = vmcs_readl(kvm_vmx_segment_fields[seg].base);
	return *p;
}

static u32 vmx_read_guest_seg_limit(struct vcpu_vmx *vmx, unsigned seg)
{
	u32 *p = &vmx->segment_cache.seg[seg].limit;

	if (!vmx_segment_cache_test_set(vmx, seg, SEG_FIELD_LIMIT))
		*p = vmcs_read32(kvm_vmx_segment_fields[seg].limit);
	return *p;
}

static u32 vmx_read_guest_seg_ar(struct vcpu_vmx *vmx, unsigned seg)
{
	u32 *p = &vmx->segment_cache.seg[seg].ar;

	if (!vmx_segment_cache_test_set(vmx, seg, SEG_FIELD_AR))
		*p = vmcs_read32(kvm_vmx_segment_fields[seg].ar_bytes);
	return *p;
}

void vmx_update_exception_bitmap(struct kvm_vcpu *vcpu)
{
	u32 eb;

	eb = (1u << PF_VECTOR) | (1u << UD_VECTOR) | (1u << MC_VECTOR) |
	     (1u << DB_VECTOR) | (1u << AC_VECTOR);
	/*
	 * Guest access to VMware backdoor ports could legitimately
	 * trigger #GP because of TSS I/O permission bitmap.
	 * We intercept those #GP and allow access to them anyway
	 * as VMware does.
	 */
	if (enable_vmware_backdoor)
		eb |= (1u << GP_VECTOR);
	if ((vcpu->guest_debug &
	     (KVM_GUESTDBG_ENABLE | KVM_GUESTDBG_USE_SW_BP)) ==
	    (KVM_GUESTDBG_ENABLE | KVM_GUESTDBG_USE_SW_BP))
		eb |= 1u << BP_VECTOR;
	if (to_vmx(vcpu)->rmode.vm86_active)
		eb = ~0;
	if (!vmx_need_pf_intercept(vcpu))
		eb &= ~(1u << PF_VECTOR);

	/* When we are running a nested L2 guest and L1 specified for it a
	 * certain exception bitmap, we must trap the same exceptions and pass
	 * them to L1. When running L2, we will only handle the exceptions
	 * specified above if L1 did not want them.
	 */
	if (is_guest_mode(vcpu))
		eb |= get_vmcs12(vcpu)->exception_bitmap;
        else {
		/*
		 * If EPT is enabled, #PF is only trapped if MAXPHYADDR is mismatched
		 * between guest and host.  In that case we only care about present
		 * faults.  For vmcs02, however, PFEC_MASK and PFEC_MATCH are set in
		 * prepare_vmcs02_rare.
		 */
		bool selective_pf_trap = enable_ept && (eb & (1u << PF_VECTOR));
		int mask = selective_pf_trap ? PFERR_PRESENT_MASK : 0;
		vmcs_write32(PAGE_FAULT_ERROR_CODE_MASK, mask);
		vmcs_write32(PAGE_FAULT_ERROR_CODE_MATCH, mask);
	}

	vmcs_write32(EXCEPTION_BITMAP, eb);
}

/*
 * Check if MSR is intercepted for currently loaded MSR bitmap.
 */
static bool msr_write_intercepted(struct kvm_vcpu *vcpu, u32 msr)
{
	unsigned long *msr_bitmap;
	int f = sizeof(unsigned long);

	if (!cpu_has_vmx_msr_bitmap())
		return true;

	msr_bitmap = to_vmx(vcpu)->loaded_vmcs->msr_bitmap;

	if (msr <= 0x1fff) {
		return !!test_bit(msr, msr_bitmap + 0x800 / f);
	} else if ((msr >= 0xc0000000) && (msr <= 0xc0001fff)) {
		msr &= 0x1fff;
		return !!test_bit(msr, msr_bitmap + 0xc00 / f);
	}

	return true;
}

static void clear_atomic_switch_msr_special(struct vcpu_vmx *vmx,
		unsigned long entry, unsigned long exit)
{
	vm_entry_controls_clearbit(vmx, entry);
	vm_exit_controls_clearbit(vmx, exit);
}

int vmx_find_loadstore_msr_slot(struct vmx_msrs *m, u32 msr)
{
	unsigned int i;

	for (i = 0; i < m->nr; ++i) {
		if (m->val[i].index == msr)
			return i;
	}
	return -ENOENT;
}

static void clear_atomic_switch_msr(struct vcpu_vmx *vmx, unsigned msr)
{
	int i;
	struct msr_autoload *m = &vmx->msr_autoload;

	switch (msr) {
	case MSR_EFER:
		if (cpu_has_load_ia32_efer()) {
			clear_atomic_switch_msr_special(vmx,
					VM_ENTRY_LOAD_IA32_EFER,
					VM_EXIT_LOAD_IA32_EFER);
			return;
		}
		break;
	case MSR_CORE_PERF_GLOBAL_CTRL:
		if (cpu_has_load_perf_global_ctrl()) {
			clear_atomic_switch_msr_special(vmx,
					VM_ENTRY_LOAD_IA32_PERF_GLOBAL_CTRL,
					VM_EXIT_LOAD_IA32_PERF_GLOBAL_CTRL);
			return;
		}
		break;
	}
	i = vmx_find_loadstore_msr_slot(&m->guest, msr);
	if (i < 0)
		goto skip_guest;
	--m->guest.nr;
	m->guest.val[i] = m->guest.val[m->guest.nr];
	vmcs_write32(VM_ENTRY_MSR_LOAD_COUNT, m->guest.nr);

skip_guest:
	i = vmx_find_loadstore_msr_slot(&m->host, msr);
	if (i < 0)
		return;

	--m->host.nr;
	m->host.val[i] = m->host.val[m->host.nr];
	vmcs_write32(VM_EXIT_MSR_LOAD_COUNT, m->host.nr);
}

static void add_atomic_switch_msr_special(struct vcpu_vmx *vmx,
		unsigned long entry, unsigned long exit,
		unsigned long guest_val_vmcs, unsigned long host_val_vmcs,
		u64 guest_val, u64 host_val)
{
	vmcs_write64(guest_val_vmcs, guest_val);
	if (host_val_vmcs != HOST_IA32_EFER)
		vmcs_write64(host_val_vmcs, host_val);
	vm_entry_controls_setbit(vmx, entry);
	vm_exit_controls_setbit(vmx, exit);
}

static void add_atomic_switch_msr(struct vcpu_vmx *vmx, unsigned msr,
				  u64 guest_val, u64 host_val, bool entry_only)
{
	int i, j = 0;
	struct msr_autoload *m = &vmx->msr_autoload;

	switch (msr) {
	case MSR_EFER:
		if (cpu_has_load_ia32_efer()) {
			add_atomic_switch_msr_special(vmx,
					VM_ENTRY_LOAD_IA32_EFER,
					VM_EXIT_LOAD_IA32_EFER,
					GUEST_IA32_EFER,
					HOST_IA32_EFER,
					guest_val, host_val);
			return;
		}
		break;
	case MSR_CORE_PERF_GLOBAL_CTRL:
		if (cpu_has_load_perf_global_ctrl()) {
			add_atomic_switch_msr_special(vmx,
					VM_ENTRY_LOAD_IA32_PERF_GLOBAL_CTRL,
					VM_EXIT_LOAD_IA32_PERF_GLOBAL_CTRL,
					GUEST_IA32_PERF_GLOBAL_CTRL,
					HOST_IA32_PERF_GLOBAL_CTRL,
					guest_val, host_val);
			return;
		}
		break;
	case MSR_IA32_PEBS_ENABLE:
		/* PEBS needs a quiescent period after being disabled (to write
		 * a record).  Disabling PEBS through VMX MSR swapping doesn't
		 * provide that period, so a CPU could write host's record into
		 * guest's memory.
		 */
		wrmsrl(MSR_IA32_PEBS_ENABLE, 0);
	}

	i = vmx_find_loadstore_msr_slot(&m->guest, msr);
	if (!entry_only)
		j = vmx_find_loadstore_msr_slot(&m->host, msr);

	if ((i < 0 && m->guest.nr == MAX_NR_LOADSTORE_MSRS) ||
	    (j < 0 &&  m->host.nr == MAX_NR_LOADSTORE_MSRS)) {
		printk_once(KERN_WARNING "Not enough msr switch entries. "
				"Can't add msr %x\n", msr);
		return;
	}
	if (i < 0) {
		i = m->guest.nr++;
		vmcs_write32(VM_ENTRY_MSR_LOAD_COUNT, m->guest.nr);
	}
	m->guest.val[i].index = msr;
	m->guest.val[i].value = guest_val;

	if (entry_only)
		return;

	if (j < 0) {
		j = m->host.nr++;
		vmcs_write32(VM_EXIT_MSR_LOAD_COUNT, m->host.nr);
	}
	m->host.val[j].index = msr;
	m->host.val[j].value = host_val;
}

static bool update_transition_efer(struct vcpu_vmx *vmx)
{
	u64 guest_efer = vmx->vcpu.arch.efer;
	u64 ignore_bits = 0;
	int i;

	/* Shadow paging assumes NX to be available.  */
	if (!enable_ept)
		guest_efer |= EFER_NX;

	/*
	 * LMA and LME handled by hardware; SCE meaningless outside long mode.
	 */
	ignore_bits |= EFER_SCE;
#ifdef CONFIG_X86_64
	ignore_bits |= EFER_LMA | EFER_LME;
	/* SCE is meaningful only in long mode on Intel */
	if (guest_efer & EFER_LMA)
		ignore_bits &= ~(u64)EFER_SCE;
#endif

	/*
	 * On EPT, we can't emulate NX, so we must switch EFER atomically.
	 * On CPUs that support "load IA32_EFER", always switch EFER
	 * atomically, since it's faster than switching it manually.
	 */
	if (cpu_has_load_ia32_efer() ||
	    (enable_ept && ((vmx->vcpu.arch.efer ^ host_efer) & EFER_NX))) {
		if (!(guest_efer & EFER_LMA))
			guest_efer &= ~EFER_LME;
		if (guest_efer != host_efer)
			add_atomic_switch_msr(vmx, MSR_EFER,
					      guest_efer, host_efer, false);
		else
			clear_atomic_switch_msr(vmx, MSR_EFER);
		return false;
	}

	i = kvm_find_user_return_msr(MSR_EFER);
	if (i < 0)
		return false;

	clear_atomic_switch_msr(vmx, MSR_EFER);

	guest_efer &= ~ignore_bits;
	guest_efer |= host_efer & ignore_bits;

	vmx->guest_uret_msrs[i].data = guest_efer;
	vmx->guest_uret_msrs[i].mask = ~ignore_bits;

	return true;
}

#ifdef CONFIG_X86_32
/*
 * On 32-bit kernels, VM exits still load the FS and GS bases from the
 * VMCS rather than the segment table.  KVM uses this helper to figure
 * out the current bases to poke them into the VMCS before entry.
 */
static unsigned long segment_base(u16 selector)
{
	struct desc_struct *table;
	unsigned long v;

	if (!(selector & ~SEGMENT_RPL_MASK))
		return 0;

	table = get_current_gdt_ro();

	if ((selector & SEGMENT_TI_MASK) == SEGMENT_LDT) {
		u16 ldt_selector = kvm_read_ldt();

		if (!(ldt_selector & ~SEGMENT_RPL_MASK))
			return 0;

		table = (struct desc_struct *)segment_base(ldt_selector);
	}
	v = get_desc_base(&table[selector >> 3]);
	return v;
}
#endif

static inline bool pt_can_write_msr(struct vcpu_vmx *vmx)
{
	return vmx_pt_mode_is_host_guest() &&
	       !(vmx->pt_desc.guest.ctl & RTIT_CTL_TRACEEN);
}

static inline bool pt_output_base_valid(struct kvm_vcpu *vcpu, u64 base)
{
	/* The base must be 128-byte aligned and a legal physical address. */
	return kvm_vcpu_is_legal_aligned_gpa(vcpu, base, 128);
}

static inline void pt_load_msr(struct pt_ctx *ctx, u32 addr_range)
{
	u32 i;

	wrmsrl(MSR_IA32_RTIT_STATUS, ctx->status);
	wrmsrl(MSR_IA32_RTIT_OUTPUT_BASE, ctx->output_base);
	wrmsrl(MSR_IA32_RTIT_OUTPUT_MASK, ctx->output_mask);
	wrmsrl(MSR_IA32_RTIT_CR3_MATCH, ctx->cr3_match);
	for (i = 0; i < addr_range; i++) {
		wrmsrl(MSR_IA32_RTIT_ADDR0_A + i * 2, ctx->addr_a[i]);
		wrmsrl(MSR_IA32_RTIT_ADDR0_B + i * 2, ctx->addr_b[i]);
	}
}

static inline void pt_save_msr(struct pt_ctx *ctx, u32 addr_range)
{
	u32 i;

	rdmsrl(MSR_IA32_RTIT_STATUS, ctx->status);
	rdmsrl(MSR_IA32_RTIT_OUTPUT_BASE, ctx->output_base);
	rdmsrl(MSR_IA32_RTIT_OUTPUT_MASK, ctx->output_mask);
	rdmsrl(MSR_IA32_RTIT_CR3_MATCH, ctx->cr3_match);
	for (i = 0; i < addr_range; i++) {
		rdmsrl(MSR_IA32_RTIT_ADDR0_A + i * 2, ctx->addr_a[i]);
		rdmsrl(MSR_IA32_RTIT_ADDR0_B + i * 2, ctx->addr_b[i]);
	}
}

static void pt_guest_enter(struct vcpu_vmx *vmx)
{
	if (vmx_pt_mode_is_system())
		return;

	/*
	 * GUEST_IA32_RTIT_CTL is already set in the VMCS.
	 * Save host state before VM entry.
	 */
	rdmsrl(MSR_IA32_RTIT_CTL, vmx->pt_desc.host.ctl);
	if (vmx->pt_desc.guest.ctl & RTIT_CTL_TRACEEN) {
		wrmsrl(MSR_IA32_RTIT_CTL, 0);
		pt_save_msr(&vmx->pt_desc.host, vmx->pt_desc.addr_range);
		pt_load_msr(&vmx->pt_desc.guest, vmx->pt_desc.addr_range);
	}
}

static void pt_guest_exit(struct vcpu_vmx *vmx)
{
	if (vmx_pt_mode_is_system())
		return;

	if (vmx->pt_desc.guest.ctl & RTIT_CTL_TRACEEN) {
		pt_save_msr(&vmx->pt_desc.guest, vmx->pt_desc.addr_range);
		pt_load_msr(&vmx->pt_desc.host, vmx->pt_desc.addr_range);
	}

	/* Reload host state (IA32_RTIT_CTL will be cleared on VM exit). */
	wrmsrl(MSR_IA32_RTIT_CTL, vmx->pt_desc.host.ctl);
}

void vmx_set_host_fs_gs(struct vmcs_host_state *host, u16 fs_sel, u16 gs_sel,
			unsigned long fs_base, unsigned long gs_base)
{
	if (unlikely(fs_sel != host->fs_sel)) {
		if (!(fs_sel & 7))
			vmcs_write16(HOST_FS_SELECTOR, fs_sel);
		else
			vmcs_write16(HOST_FS_SELECTOR, 0);
		host->fs_sel = fs_sel;
	}
	if (unlikely(gs_sel != host->gs_sel)) {
		if (!(gs_sel & 7))
			vmcs_write16(HOST_GS_SELECTOR, gs_sel);
		else
			vmcs_write16(HOST_GS_SELECTOR, 0);
		host->gs_sel = gs_sel;
	}
	if (unlikely(fs_base != host->fs_base)) {
		vmcs_writel(HOST_FS_BASE, fs_base);
		host->fs_base = fs_base;
	}
	if (unlikely(gs_base != host->gs_base)) {
		vmcs_writel(HOST_GS_BASE, gs_base);
		host->gs_base = gs_base;
	}
}

void vmx_prepare_switch_to_guest(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	struct vmcs_host_state *host_state;
#ifdef CONFIG_X86_64
	int cpu = raw_smp_processor_id();
#endif
	unsigned long fs_base, gs_base;
	u16 fs_sel, gs_sel;
	int i;

	vmx->req_immediate_exit = false;

	/*
	 * Note that guest MSRs to be saved/restored can also be changed
	 * when guest state is loaded. This happens when guest transitions
	 * to/from long-mode by setting MSR_EFER.LMA.
	 */
	if (!vmx->guest_uret_msrs_loaded) {
		vmx->guest_uret_msrs_loaded = true;
		for (i = 0; i < kvm_nr_uret_msrs; ++i) {
			if (!vmx->guest_uret_msrs[i].load_into_hardware)
				continue;

			kvm_set_user_return_msr(i,
						vmx->guest_uret_msrs[i].data,
						vmx->guest_uret_msrs[i].mask);
		}
	}

    	if (vmx->nested.need_vmcs12_to_shadow_sync)
		nested_sync_vmcs12_to_shadow(vcpu);

	if (vmx->guest_state_loaded)
		return;

	host_state = &vmx->loaded_vmcs->host_state;

	/*
	 * Set host fs and gs selectors.  Unfortunately, 22.2.3 does not
	 * allow segment selectors with cpl > 0 or ti == 1.
	 */
	host_state->ldt_sel = kvm_read_ldt();

#ifdef CONFIG_X86_64
	savesegment(ds, host_state->ds_sel);
	savesegment(es, host_state->es_sel);

	gs_base = cpu_kernelmode_gs_base(cpu);
	if (likely(is_64bit_mm(current->mm))) {
		current_save_fsgs();
		fs_sel = current->thread.fsindex;
		gs_sel = current->thread.gsindex;
		fs_base = current->thread.fsbase;
		vmx->msr_host_kernel_gs_base = current->thread.gsbase;
	} else {
		savesegment(fs, fs_sel);
		savesegment(gs, gs_sel);
		fs_base = read_msr(MSR_FS_BASE);
		vmx->msr_host_kernel_gs_base = read_msr(MSR_KERNEL_GS_BASE);
	}

	wrmsrl(MSR_KERNEL_GS_BASE, vmx->msr_guest_kernel_gs_base);
#else
	savesegment(fs, fs_sel);
	savesegment(gs, gs_sel);
	fs_base = segment_base(fs_sel);
	gs_base = segment_base(gs_sel);
#endif

	vmx_set_host_fs_gs(host_state, fs_sel, gs_sel, fs_base, gs_base);
	vmx->guest_state_loaded = true;
}

static void vmx_prepare_switch_to_host(struct vcpu_vmx *vmx)
{
	struct vmcs_host_state *host_state;

	if (!vmx->guest_state_loaded)
		return;

	host_state = &vmx->loaded_vmcs->host_state;

	++vmx->vcpu.stat.host_state_reload;

#ifdef CONFIG_X86_64
	rdmsrl(MSR_KERNEL_GS_BASE, vmx->msr_guest_kernel_gs_base);
#endif
	if (host_state->ldt_sel || (host_state->gs_sel & 7)) {
		kvm_load_ldt(host_state->ldt_sel);
#ifdef CONFIG_X86_64
		load_gs_index(host_state->gs_sel);
#else
		loadsegment(gs, host_state->gs_sel);
#endif
	}
	if (host_state->fs_sel & 7)
		loadsegment(fs, host_state->fs_sel);
#ifdef CONFIG_X86_64
	if (unlikely(host_state->ds_sel | host_state->es_sel)) {
		loadsegment(ds, host_state->ds_sel);
		loadsegment(es, host_state->es_sel);
	}
#endif
	invalidate_tss_limit();
#ifdef CONFIG_X86_64
	wrmsrl(MSR_KERNEL_GS_BASE, vmx->msr_host_kernel_gs_base);
#endif
	load_fixmap_gdt(raw_smp_processor_id());
	vmx->guest_state_loaded = false;
	vmx->guest_uret_msrs_loaded = false;
}

#ifdef CONFIG_X86_64
static u64 vmx_read_guest_kernel_gs_base(struct vcpu_vmx *vmx)
{
	preempt_disable();
	if (vmx->guest_state_loaded)
		rdmsrl(MSR_KERNEL_GS_BASE, vmx->msr_guest_kernel_gs_base);
	preempt_enable();
	return vmx->msr_guest_kernel_gs_base;
}

static void vmx_write_guest_kernel_gs_base(struct vcpu_vmx *vmx, u64 data)
{
	preempt_disable();
	if (vmx->guest_state_loaded)
		wrmsrl(MSR_KERNEL_GS_BASE, data);
	preempt_enable();
	vmx->msr_guest_kernel_gs_base = data;
}
#endif

void vmx_vcpu_load_vmcs(struct kvm_vcpu *vcpu, int cpu,
			struct loaded_vmcs *buddy)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	bool already_loaded = vmx->loaded_vmcs->cpu == cpu;
	struct vmcs *prev;

	if (!already_loaded) {
		loaded_vmcs_clear(vmx->loaded_vmcs);
		local_irq_disable();

		/*
		 * Ensure loaded_vmcs->cpu is read before adding loaded_vmcs to
		 * this cpu's percpu list, otherwise it may not yet be deleted
		 * from its previous cpu's percpu list.  Pairs with the
		 * smb_wmb() in __loaded_vmcs_clear().
		 */
		smp_rmb();

		list_add(&vmx->loaded_vmcs->loaded_vmcss_on_cpu_link,
			 &per_cpu(loaded_vmcss_on_cpu, cpu));
		local_irq_enable();
	}

	prev = per_cpu(current_vmcs, cpu);
	if (prev != vmx->loaded_vmcs->vmcs) {
		per_cpu(current_vmcs, cpu) = vmx->loaded_vmcs->vmcs;
		vmcs_load(vmx->loaded_vmcs->vmcs);

		/*
		 * No indirect branch prediction barrier needed when switching
		 * the active VMCS within a guest, e.g. on nested VM-Enter.
		 * The L1 VMM can protect itself with retpolines, IBPB or IBRS.
		 */
		if (!buddy || WARN_ON_ONCE(buddy->vmcs != prev))
			indirect_branch_prediction_barrier();
	}

	if (!already_loaded) {
		void *gdt = get_current_gdt_ro();
		unsigned long sysenter_esp;

		/*
		 * Flush all EPTP/VPID contexts, the new pCPU may have stale
		 * TLB entries from its previous association with the vCPU.
		 */
		kvm_make_request(KVM_REQ_TLB_FLUSH, vcpu);

		/*
		 * Linux uses per-cpu TSS and GDT, so set these when switching
		 * processors.  See 22.2.4.
		 */
		vmcs_writel(HOST_TR_BASE,
			    (unsigned long)&get_cpu_entry_area(cpu)->tss.x86_tss);
		vmcs_writel(HOST_GDTR_BASE, (unsigned long)gdt);   /* 22.2.4 */

		rdmsrl(MSR_IA32_SYSENTER_ESP, sysenter_esp);
		vmcs_writel(HOST_IA32_SYSENTER_ESP, sysenter_esp); /* 22.2.3 */

		vmx->loaded_vmcs->cpu = cpu;
	}

	/* Setup TSC multiplier */
	if (kvm_has_tsc_control &&
	    vmx->current_tsc_ratio != vcpu->arch.tsc_scaling_ratio)
		decache_tsc_multiplier(vmx);
}

/*
 * Switches to specified vcpu, until a matching vcpu_put(), but assumes
 * vcpu mutex is already taken.
 */
static void vmx_vcpu_load(struct kvm_vcpu *vcpu, int cpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);

	vmx_vcpu_load_vmcs(vcpu, cpu, NULL);

	vmx_vcpu_pi_load(vcpu, cpu);

	vmx->host_debugctlmsr = get_debugctlmsr();
}

static void vmx_vcpu_put(struct kvm_vcpu *vcpu)
{
	vmx_vcpu_pi_put(vcpu);

	vmx_prepare_switch_to_host(to_vmx(vcpu));
}

static bool emulation_required(struct kvm_vcpu *vcpu)
{
	return emulate_invalid_guest_state && !vmx_guest_state_valid(vcpu);
}

unsigned long vmx_get_rflags(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	unsigned long rflags, save_rflags;

	if (!kvm_register_is_available(vcpu, VCPU_EXREG_RFLAGS)) {
		kvm_register_mark_available(vcpu, VCPU_EXREG_RFLAGS);
		rflags = vmcs_readl(GUEST_RFLAGS);
		if (vmx->rmode.vm86_active) {
			rflags &= RMODE_GUEST_OWNED_EFLAGS_BITS;
			save_rflags = vmx->rmode.save_rflags;
			rflags |= save_rflags & ~RMODE_GUEST_OWNED_EFLAGS_BITS;
		}
		vmx->rflags = rflags;
	}
	return vmx->rflags;
}

void vmx_set_rflags(struct kvm_vcpu *vcpu, unsigned long rflags)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	unsigned long old_rflags;

	if (is_unrestricted_guest(vcpu)) {
		kvm_register_mark_available(vcpu, VCPU_EXREG_RFLAGS);
		vmx->rflags = rflags;
		vmcs_writel(GUEST_RFLAGS, rflags);
		return;
	}

	old_rflags = vmx_get_rflags(vcpu);
	vmx->rflags = rflags;
	if (vmx->rmode.vm86_active) {
		vmx->rmode.save_rflags = rflags;
		rflags |= X86_EFLAGS_IOPL | X86_EFLAGS_VM;
	}
	vmcs_writel(GUEST_RFLAGS, rflags);

	if ((old_rflags ^ vmx->rflags) & X86_EFLAGS_VM)
		vmx->emulation_required = emulation_required(vcpu);
}

u32 vmx_get_interrupt_shadow(struct kvm_vcpu *vcpu)
{
	u32 interruptibility = vmcs_read32(GUEST_INTERRUPTIBILITY_INFO);
	int ret = 0;

	if (interruptibility & GUEST_INTR_STATE_STI)
		ret |= KVM_X86_SHADOW_INT_STI;
	if (interruptibility & GUEST_INTR_STATE_MOV_SS)
		ret |= KVM_X86_SHADOW_INT_MOV_SS;

	return ret;
}

void vmx_set_interrupt_shadow(struct kvm_vcpu *vcpu, int mask)
{
	u32 interruptibility_old = vmcs_read32(GUEST_INTERRUPTIBILITY_INFO);
	u32 interruptibility = interruptibility_old;

	interruptibility &= ~(GUEST_INTR_STATE_STI | GUEST_INTR_STATE_MOV_SS);

	if (mask & KVM_X86_SHADOW_INT_MOV_SS)
		interruptibility |= GUEST_INTR_STATE_MOV_SS;
	else if (mask & KVM_X86_SHADOW_INT_STI)
		interruptibility |= GUEST_INTR_STATE_STI;

	if ((interruptibility != interruptibility_old))
		vmcs_write32(GUEST_INTERRUPTIBILITY_INFO, interruptibility);
}

static int vmx_rtit_ctl_check(struct kvm_vcpu *vcpu, u64 data)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	unsigned long value;

	/*
	 * Any MSR write that attempts to change bits marked reserved will
	 * case a #GP fault.
	 */
	if (data & vmx->pt_desc.ctl_bitmask)
		return 1;

	/*
	 * Any attempt to modify IA32_RTIT_CTL while TraceEn is set will
	 * result in a #GP unless the same write also clears TraceEn.
	 */
	if ((vmx->pt_desc.guest.ctl & RTIT_CTL_TRACEEN) &&
		((vmx->pt_desc.guest.ctl ^ data) & ~RTIT_CTL_TRACEEN))
		return 1;

	/*
	 * WRMSR to IA32_RTIT_CTL that sets TraceEn but clears this bit
	 * and FabricEn would cause #GP, if
	 * CPUID.(EAX=14H, ECX=0):ECX.SNGLRGNOUT[bit 2] = 0
	 */
	if ((data & RTIT_CTL_TRACEEN) && !(data & RTIT_CTL_TOPA) &&
		!(data & RTIT_CTL_FABRIC_EN) &&
		!intel_pt_validate_cap(vmx->pt_desc.caps,
					PT_CAP_single_range_output))
		return 1;

	/*
	 * MTCFreq, CycThresh and PSBFreq encodings check, any MSR write that
	 * utilize encodings marked reserved will cause a #GP fault.
	 */
	value = intel_pt_validate_cap(vmx->pt_desc.caps, PT_CAP_mtc_periods);
	if (intel_pt_validate_cap(vmx->pt_desc.caps, PT_CAP_mtc) &&
			!test_bit((data & RTIT_CTL_MTC_RANGE) >>
			RTIT_CTL_MTC_RANGE_OFFSET, &value))
		return 1;
	value = intel_pt_validate_cap(vmx->pt_desc.caps,
						PT_CAP_cycle_thresholds);
	if (intel_pt_validate_cap(vmx->pt_desc.caps, PT_CAP_psb_cyc) &&
			!test_bit((data & RTIT_CTL_CYC_THRESH) >>
			RTIT_CTL_CYC_THRESH_OFFSET, &value))
		return 1;
	value = intel_pt_validate_cap(vmx->pt_desc.caps, PT_CAP_psb_periods);
	if (intel_pt_validate_cap(vmx->pt_desc.caps, PT_CAP_psb_cyc) &&
			!test_bit((data & RTIT_CTL_PSB_FREQ) >>
			RTIT_CTL_PSB_FREQ_OFFSET, &value))
		return 1;

	/*
	 * If ADDRx_CFG is reserved or the encodings is >2 will
	 * cause a #GP fault.
	 */
	value = (data & RTIT_CTL_ADDR0) >> RTIT_CTL_ADDR0_OFFSET;
	if ((value && (vmx->pt_desc.addr_range < 1)) || (value > 2))
		return 1;
	value = (data & RTIT_CTL_ADDR1) >> RTIT_CTL_ADDR1_OFFSET;
	if ((value && (vmx->pt_desc.addr_range < 2)) || (value > 2))
		return 1;
	value = (data & RTIT_CTL_ADDR2) >> RTIT_CTL_ADDR2_OFFSET;
	if ((value && (vmx->pt_desc.addr_range < 3)) || (value > 2))
		return 1;
	value = (data & RTIT_CTL_ADDR3) >> RTIT_CTL_ADDR3_OFFSET;
	if ((value && (vmx->pt_desc.addr_range < 4)) || (value > 2))
		return 1;

	return 0;
}

static bool vmx_can_emulate_instruction(struct kvm_vcpu *vcpu, void *insn, int insn_len)
{
	/*
	 * Emulation of instructions in SGX enclaves is impossible as RIP does
	 * not point  tthe failing instruction, and even if it did, the code
	 * stream is inaccessible.  Inject #UD instead of exiting to userspace
	 * so that guest userspace can't DoS the guest simply by triggering
	 * emulation (enclaves are CPL3 only).
	 */
	if (to_vmx(vcpu)->exit_reason.enclave_mode) {
		kvm_queue_exception(vcpu, UD_VECTOR);
		return false;
	}
	return true;
}

static int skip_emulated_instruction(struct kvm_vcpu *vcpu)
{
	union vmx_exit_reason exit_reason = to_vmx(vcpu)->exit_reason;
	unsigned long rip, orig_rip;
	u32 instr_len;

	/*
	 * Using VMCS.VM_EXIT_INSTRUCTION_LEN on EPT misconfig depends on
	 * undefined behavior: Intel's SDM doesn't mandate the VMCS field be
	 * set when EPT misconfig occurs.  In practice, real hardware updates
	 * VM_EXIT_INSTRUCTION_LEN on EPT misconfig, but other hypervisors
	 * (namely Hyper-V) don't set it due to it being undefined behavior,
	 * i.e. we end up advancing IP with some random value.
	 */
	if (!static_cpu_has(X86_FEATURE_HYPERVISOR) ||
	    exit_reason.basic != EXIT_REASON_EPT_MISCONFIG) {
		instr_len = vmcs_read32(VM_EXIT_INSTRUCTION_LEN);

		/*
		 * Emulating an enclave's instructions isn't supported as KVM
		 * cannot access the enclave's memory or its true RIP, e.g. the
		 * vmcs.GUEST_RIP points at the exit point of the enclave, not
		 * the RIP that actually triggered the VM-Exit.  But, because
		 * most instructions that cause VM-Exit will #UD in an enclave,
		 * most instruction-based VM-Exits simply do not occur.
		 *
		 * There are a few exceptions, notably the debug instructions
		 * INT1ICEBRK and INT3, as they are allowed in debug enclaves
		 * and generate #DB/#BP as expected, which KVM might intercept.
		 * But again, the CPU does the dirty work and saves an instr
		 * length of zero so VMMs don't shoot themselves in the foot.
		 * WARN if KVM tries to skip a non-zero length instruction on
		 * a VM-Exit from an enclave.
		 */
		if (!instr_len)
			goto rip_updated;

		WARN(exit_reason.enclave_mode,
		     "KVM: skipping instruction after SGX enclave VM-Exit");

		orig_rip = kvm_rip_read(vcpu);
		rip = orig_rip + instr_len;
#ifdef CONFIG_X86_64
		/*
		 * We need to mask out the high 32 bits of RIP if not in 64-bit
		 * mode, but just finding out that we are in 64-bit mode is
		 * quite expensive.  Only do it if there was a carry.
		 */
		if (unlikely(((rip ^ orig_rip) >> 31) == 3) && !is_64_bit_mode(vcpu))
			rip = (u32)rip;
#endif
		kvm_rip_write(vcpu, rip);
	} else {
		if (!kvm_emulate_instruction(vcpu, EMULTYPE_SKIP))
			return 0;
	}

rip_updated:
	/* skipping an emulated instruction also counts */
	vmx_set_interrupt_shadow(vcpu, 0);

	return 1;
}

/*
 * Recognizes a pending MTF VM-exit and records the nested state for later
 * delivery.
 */
static void vmx_update_emulated_instruction(struct kvm_vcpu *vcpu)
{
	struct vmcs12 *vmcs12 = get_vmcs12(vcpu);
	struct vcpu_vmx *vmx = to_vmx(vcpu);

	if (!is_guest_mode(vcpu))
		return;

	/*
	 * Per the SDM, MTF takes priority over debug-trap exceptions besides
	 * T-bit traps. As instruction emulation is completed (i.e. at the
	 * instruction boundary), any #DB exception pending delivery must be a
	 * debug-trap. Record the pending MTF state to be delivered in
	 * vmx_check_nested_events().
	 */
	if (nested_cpu_has_mtf(vmcs12) &&
	    (!vcpu->arch.exception.pending ||
	     vcpu->arch.exception.nr == DB_VECTOR))
		vmx->nested.mtf_pending = true;
	else
		vmx->nested.mtf_pending = false;
}

static int vmx_skip_emulated_instruction(struct kvm_vcpu *vcpu)
{
	vmx_update_emulated_instruction(vcpu);
	return skip_emulated_instruction(vcpu);
}

static void vmx_clear_hlt(struct kvm_vcpu *vcpu)
{
	/*
	 * Ensure that we clear the HLT state in the VMCS.  We don't need to
	 * explicitly skip the instruction because if the HLT state is set,
	 * then the instruction is already executing and RIP has already been
	 * advanced.
	 */
	if (kvm_hlt_in_guest(vcpu->kvm) &&
			vmcs_read32(GUEST_ACTIVITY_STATE) == GUEST_ACTIVITY_HLT)
		vmcs_write32(GUEST_ACTIVITY_STATE, GUEST_ACTIVITY_ACTIVE);
}

static void vmx_queue_exception(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	unsigned nr = vcpu->arch.exception.nr;
	bool has_error_code = vcpu->arch.exception.has_error_code;
	u32 error_code = vcpu->arch.exception.error_code;
	u32 intr_info = nr | INTR_INFO_VALID_MASK;

	kvm_deliver_exception_payload(vcpu);

	if (has_error_code) {
		vmcs_write32(VM_ENTRY_EXCEPTION_ERROR_CODE, error_code);
		intr_info |= INTR_INFO_DELIVER_CODE_MASK;
	}

	if (vmx->rmode.vm86_active) {
		int inc_eip = 0;
		if (kvm_exception_is_soft(nr))
			inc_eip = vcpu->arch.event_exit_inst_len;
		kvm_inject_realmode_interrupt(vcpu, nr, inc_eip);
		return;
	}

	WARN_ON_ONCE(vmx->emulation_required);

	if (kvm_exception_is_soft(nr)) {
		vmcs_write32(VM_ENTRY_INSTRUCTION_LEN,
			     vmx->vcpu.arch.event_exit_inst_len);
		intr_info |= INTR_TYPE_SOFT_EXCEPTION;
	} else
		intr_info |= INTR_TYPE_HARD_EXCEPTION;

	vmcs_write32(VM_ENTRY_INTR_INFO_FIELD, intr_info);

	vmx_clear_hlt(vcpu);
}

static void vmx_setup_uret_msr(struct vcpu_vmx *vmx, unsigned int msr,
			       bool load_into_hardware)
{
	struct vmx_uret_msr *uret_msr;

	uret_msr = vmx_find_uret_msr(vmx, msr);
	if (!uret_msr)
		return;

	uret_msr->load_into_hardware = load_into_hardware;
}

/*
 * Set up the vmcs to automatically save and restore system
 * msrs.  Don't touch the 64-bit msrs if the guest is in legacy
 * mode, as fiddling with msrs is very expensive.
 */
static void setup_msrs(struct vcpu_vmx *vmx)
{
#ifdef CONFIG_X86_64
	bool load_syscall_msrs;

	/*
	 * The SYSCALL MSRs are only needed on long mode guests, and only
	 * when EFER.SCE is set.
	 */
	load_syscall_msrs = is_long_mode(&vmx->vcpu) &&
			    (vmx->vcpu.arch.efer & EFER_SCE);

	vmx_setup_uret_msr(vmx, MSR_STAR, load_syscall_msrs);
	vmx_setup_uret_msr(vmx, MSR_LSTAR, load_syscall_msrs);
	vmx_setup_uret_msr(vmx, MSR_SYSCALL_MASK, load_syscall_msrs);
#endif
	vmx_setup_uret_msr(vmx, MSR_EFER, update_transition_efer(vmx));

	vmx_setup_uret_msr(vmx, MSR_TSC_AUX,
			   guest_cpuid_has(&vmx->vcpu, X86_FEATURE_RDTSCP) ||
			   guest_cpuid_has(&vmx->vcpu, X86_FEATURE_RDPID));

	/*
	 * hle=0, rtm=0, tsx_ctrl=1 can be found with some combinations of new
	 * kernel and old userspace.  If those guests run on a tsx=off host, do
	 * allow guests to use TSX_CTRL, but don't change the value in hardware
	 * so that TSX remains always disabled.
	 */
	vmx_setup_uret_msr(vmx, MSR_IA32_TSX_CTRL, boot_cpu_has(X86_FEATURE_RTM));

	if (cpu_has_vmx_msr_bitmap())
		vmx_update_msr_bitmap(&vmx->vcpu);

	/*
	 * The set of MSRs to load may have changed, reload MSRs before the
	 * next VM-Enter.
	 */
	vmx->guest_uret_msrs_loaded = false;
}

static u64 vmx_write_l1_tsc_offset(struct kvm_vcpu *vcpu, u64 offset)
{
	struct vmcs12 *vmcs12 = get_vmcs12(vcpu);
	u64 g_tsc_offset = 0;

	/*
	 * We're here if L1 chose not to trap WRMSR to TSC. According
	 * to the spec, this should set L1's TSC; The offset that L1
	 * set for L2 remains unchanged, and still needs to be added
	 * to the newly set TSC to get L2's TSC.
	 */
	if (is_guest_mode(vcpu) &&
	    (vmcs12->cpu_based_vm_exec_control & CPU_BASED_USE_TSC_OFFSETTING))
		g_tsc_offset = vmcs12->tsc_offset;

	trace_kvm_write_tsc_offset(vcpu->vcpu_id,
				   vcpu->arch.tsc_offset - g_tsc_offset,
				   offset);
	vmcs_write64(TSC_OFFSET, offset + g_tsc_offset);
	return offset + g_tsc_offset;
}

/*
 * nested_vmx_allowed() checks whether a guest should be allowed to use VMX
 * instructions and MSRs (i.e., nested VMX). Nested VMX is disabled for
 * all guests if the "nested" module option is off, and can also be disabled
 * for a single guest by disabling its VMX cpuid bit.
 */
bool nested_vmx_allowed(struct kvm_vcpu *vcpu)
{
	return nested && guest_cpuid_has(vcpu, X86_FEATURE_VMX);
}

static inline bool vmx_feature_control_msr_valid(struct kvm_vcpu *vcpu,
						 uint64_t val)
{
	uint64_t valid_bits = to_vmx(vcpu)->msr_ia32_feature_control_valid_bits;

	return !(val & ~valid_bits);
}

static int vmx_get_msr_feature(struct kvm_msr_entry *msr)
{
	switch (msr->index) {
	case MSR_IA32_VMX_BASIC ... MSR_IA32_VMX_VMFUNC:
		if (!nested)
			return 1;
		return vmx_get_vmx_msr(&vmcs_config.nested, msr->index, &msr->data);
	case MSR_IA32_PERF_CAPABILITIES:
		msr->data = vmx_get_perf_capabilities();
		return 0;
	default:
		return KVM_MSR_RET_INVALID;
	}
}

/*
 * Reads an msr value (of 'msr_index') into 'pdata'.
 * Returns 0 on success, non-0 otherwise.
 * Assumes vcpu_load() was already called.
 */
static int vmx_get_msr(struct kvm_vcpu *vcpu, struct msr_data *msr_info)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	struct vmx_uret_msr *msr;
	u32 index;

	switch (msr_info->index) {
#ifdef CONFIG_X86_64
	case MSR_FS_BASE:
		msr_info->data = vmcs_readl(GUEST_FS_BASE);
		break;
	case MSR_GS_BASE:
		msr_info->data = vmcs_readl(GUEST_GS_BASE);
		break;
	case MSR_KERNEL_GS_BASE:
		msr_info->data = vmx_read_guest_kernel_gs_base(vmx);
		break;
#endif
	case MSR_EFER:
		return kvm_get_msr_common(vcpu, msr_info);
	case MSR_IA32_TSX_CTRL:
		if (!msr_info->host_initiated &&
		    !(vcpu->arch.arch_capabilities & ARCH_CAP_TSX_CTRL_MSR))
			return 1;
		goto find_uret_msr;
	case MSR_IA32_UMWAIT_CONTROL:
		if (!msr_info->host_initiated && !vmx_has_waitpkg(vmx))
			return 1;

		msr_info->data = vmx->msr_ia32_umwait_control;
		break;
	case MSR_IA32_SPEC_CTRL:
		if (!msr_info->host_initiated &&
		    !guest_has_spec_ctrl_msr(vcpu))
			return 1;

		msr_info->data = to_vmx(vcpu)->spec_ctrl;
		break;
	case MSR_IA32_SYSENTER_CS:
		msr_info->data = vmcs_read32(GUEST_SYSENTER_CS);
		break;
	case MSR_IA32_SYSENTER_EIP:
		msr_info->data = vmcs_readl(GUEST_SYSENTER_EIP);
		break;
	case MSR_IA32_SYSENTER_ESP:
		msr_info->data = vmcs_readl(GUEST_SYSENTER_ESP);
		break;
	case MSR_IA32_BNDCFGS:
		if (!kvm_mpx_supported() ||
		    (!msr_info->host_initiated &&
		     !guest_cpuid_has(vcpu, X86_FEATURE_MPX)))
			return 1;
		msr_info->data = vmcs_read64(GUEST_BNDCFGS);
		break;
	case MSR_IA32_MCG_EXT_CTL:
		if (!msr_info->host_initiated &&
		    !(vmx->msr_ia32_feature_control &
		      FEAT_CTL_LMCE_ENABLED))
			return 1;
		msr_info->data = vcpu->arch.mcg_ext_ctl;
		break;
	case MSR_IA32_FEAT_CTL:
		msr_info->data = vmx->msr_ia32_feature_control;
		break;
	case MSR_IA32_SGXLEPUBKEYHASH0 ... MSR_IA32_SGXLEPUBKEYHASH3:
		if (!msr_info->host_initiated &&
		    !guest_cpuid_has(vcpu, X86_FEATURE_SGX_LC))
			return 1;
		msr_info->data = to_vmx(vcpu)->msr_ia32_sgxlepubkeyhash
			[msr_info->index - MSR_IA32_SGXLEPUBKEYHASH0];
		break;
	case MSR_IA32_VMX_BASIC ... MSR_IA32_VMX_VMFUNC:
		if (!nested_vmx_allowed(vcpu))
			return 1;
		if (vmx_get_vmx_msr(&vmx->nested.msrs, msr_info->index,
				    &msr_info->data))
			return 1;
		/*
		 * Enlightened VMCS v1 doesn't have certain fields, but buggy
		 * Hyper-V versions are still trying to use corresponding
		 * features when they are exposed. Filter out the essential
		 * minimum.
		 */
		if (!msr_info->host_initiated &&
		    vmx->nested.enlightened_vmcs_enabled)
			nested_evmcs_filter_control_msr(msr_info->index,
							&msr_info->data);
		break;
	case MSR_IA32_RTIT_CTL:
		if (!vmx_pt_mode_is_host_guest())
			return 1;
		msr_info->data = vmx->pt_desc.guest.ctl;
		break;
	case MSR_IA32_RTIT_STATUS:
		if (!vmx_pt_mode_is_host_guest())
			return 1;
		msr_info->data = vmx->pt_desc.guest.status;
		break;
	case MSR_IA32_RTIT_CR3_MATCH:
		if (!vmx_pt_mode_is_host_guest() ||
			!intel_pt_validate_cap(vmx->pt_desc.caps,
						PT_CAP_cr3_filtering))
			return 1;
		msr_info->data = vmx->pt_desc.guest.cr3_match;
		break;
	case MSR_IA32_RTIT_OUTPUT_BASE:
		if (!vmx_pt_mode_is_host_guest() ||
			(!intel_pt_validate_cap(vmx->pt_desc.caps,
					PT_CAP_topa_output) &&
			 !intel_pt_validate_cap(vmx->pt_desc.caps,
					PT_CAP_single_range_output)))
			return 1;
		msr_info->data = vmx->pt_desc.guest.output_base;
		break;
	case MSR_IA32_RTIT_OUTPUT_MASK:
		if (!vmx_pt_mode_is_host_guest() ||
			(!intel_pt_validate_cap(vmx->pt_desc.caps,
					PT_CAP_topa_output) &&
			 !intel_pt_validate_cap(vmx->pt_desc.caps,
					PT_CAP_single_range_output)))
			return 1;
		msr_info->data = vmx->pt_desc.guest.output_mask;
		break;
	case MSR_IA32_RTIT_ADDR0_A ... MSR_IA32_RTIT_ADDR3_B:
		index = msr_info->index - MSR_IA32_RTIT_ADDR0_A;
		if (!vmx_pt_mode_is_host_guest() ||
			(index >= 2 * intel_pt_validate_cap(vmx->pt_desc.caps,
					PT_CAP_num_address_ranges)))
			return 1;
		if (index % 2)
			msr_info->data = vmx->pt_desc.guest.addr_b[index / 2];
		else
			msr_info->data = vmx->pt_desc.guest.addr_a[index / 2];
		break;
	case MSR_IA32_DEBUGCTLMSR:
		msr_info->data = vmcs_read64(GUEST_IA32_DEBUGCTL);
		break;
	default:
	find_uret_msr:
		msr = vmx_find_uret_msr(vmx, msr_info->index);
		if (msr) {
			msr_info->data = msr->data;
			break;
		}
		return kvm_get_msr_common(vcpu, msr_info);
	}

	return 0;
}

static u64 nested_vmx_truncate_sysenter_addr(struct kvm_vcpu *vcpu,
						    u64 data)
{
#ifdef CONFIG_X86_64
	if (!guest_cpuid_has(vcpu, X86_FEATURE_LM))
		return (u32)data;
#endif
	return (unsigned long)data;
}

static u64 vcpu_supported_debugctl(struct kvm_vcpu *vcpu)
{
	u64 debugctl = vmx_supported_debugctl();

	if (!intel_pmu_lbr_is_enabled(vcpu))
		debugctl &= ~DEBUGCTLMSR_LBR_MASK;

	if (!guest_cpuid_has(vcpu, X86_FEATURE_BUS_LOCK_DETECT))
		debugctl &= ~DEBUGCTLMSR_BUS_LOCK_DETECT;

	return debugctl;
}

/*
 * Writes msr value into the appropriate "register".
 * Returns 0 on success, non-0 otherwise.
 * Assumes vcpu_load() was already called.
 */
static int vmx_set_msr(struct kvm_vcpu *vcpu, struct msr_data *msr_info)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	struct vmx_uret_msr *msr;
	int ret = 0;
	u32 msr_index = msr_info->index;
	u64 data = msr_info->data;
	u32 index;

	switch (msr_index) {
	case MSR_EFER:
		ret = kvm_set_msr_common(vcpu, msr_info);
		break;
#ifdef CONFIG_X86_64
	case MSR_FS_BASE:
		vmx_segment_cache_clear(vmx);
		vmcs_writel(GUEST_FS_BASE, data);
		break;
	case MSR_GS_BASE:
		vmx_segment_cache_clear(vmx);
		vmcs_writel(GUEST_GS_BASE, data);
		break;
	case MSR_KERNEL_GS_BASE:
		vmx_write_guest_kernel_gs_base(vmx, data);
		break;
#endif
	case MSR_IA32_SYSENTER_CS:
		if (is_guest_mode(vcpu))
			get_vmcs12(vcpu)->guest_sysenter_cs = data;
		vmcs_write32(GUEST_SYSENTER_CS, data);
		break;
	case MSR_IA32_SYSENTER_EIP:
		if (is_guest_mode(vcpu)) {
			data = nested_vmx_truncate_sysenter_addr(vcpu, data);
			get_vmcs12(vcpu)->guest_sysenter_eip = data;
		}
		vmcs_writel(GUEST_SYSENTER_EIP, data);
		break;
	case MSR_IA32_SYSENTER_ESP:
		if (is_guest_mode(vcpu)) {
			data = nested_vmx_truncate_sysenter_addr(vcpu, data);
			get_vmcs12(vcpu)->guest_sysenter_esp = data;
		}
		vmcs_writel(GUEST_SYSENTER_ESP, data);
		break;
	case MSR_IA32_DEBUGCTLMSR: {
		u64 invalid = data & ~vcpu_supported_debugctl(vcpu);
		if (invalid & (DEBUGCTLMSR_BTF|DEBUGCTLMSR_LBR)) {
			if (report_ignored_msrs)
				vcpu_unimpl(vcpu, "%s: BTF|LBR in IA32_DEBUGCTLMSR 0x%llx, nop\n",
					    __func__, data);
			data &= ~(DEBUGCTLMSR_BTF|DEBUGCTLMSR_LBR);
			invalid &= ~(DEBUGCTLMSR_BTF|DEBUGCTLMSR_LBR);
		}

		if (invalid)
			return 1;

		if (is_guest_mode(vcpu) && get_vmcs12(vcpu)->vm_exit_controls &
						VM_EXIT_SAVE_DEBUG_CONTROLS)
			get_vmcs12(vcpu)->guest_ia32_debugctl = data;

		vmcs_write64(GUEST_IA32_DEBUGCTL, data);
		if (intel_pmu_lbr_is_enabled(vcpu) && !to_vmx(vcpu)->lbr_desc.event &&
		    (data & DEBUGCTLMSR_LBR))
			intel_pmu_create_guest_lbr_event(vcpu);
		return 0;
	}
	case MSR_IA32_BNDCFGS:
		if (!kvm_mpx_supported() ||
		    (!msr_info->host_initiated &&
		     !guest_cpuid_has(vcpu, X86_FEATURE_MPX)))
			return 1;
		if (is_noncanonical_address(data & PAGE_MASK, vcpu) ||
		    (data & MSR_IA32_BNDCFGS_RSVD))
			return 1;
		vmcs_write64(GUEST_BNDCFGS, data);
		break;
	case MSR_IA32_UMWAIT_CONTROL:
		if (!msr_info->host_initiated && !vmx_has_waitpkg(vmx))
			return 1;

		/* The reserved bit 1 and non-32 bit [63:32] should be zero */
		if (data & (BIT_ULL(1) | GENMASK_ULL(63, 32)))
			return 1;

		vmx->msr_ia32_umwait_control = data;
		break;
	case MSR_IA32_SPEC_CTRL:
		if (!msr_info->host_initiated &&
		    !guest_has_spec_ctrl_msr(vcpu))
			return 1;

		if (kvm_spec_ctrl_test_value(data))
			return 1;

		vmx->spec_ctrl = data;
		if (!data)
			break;

		/*
		 * For non-nested:
		 * When it's written (to non-zero) for the first time, pass
		 * it through.
		 *
		 * For nested:
		 * The handling of the MSR bitmap for L2 guests is done in
		 * nested_vmx_prepare_msr_bitmap. We should not touch the
		 * vmcs02.msr_bitmap here since it gets completely overwritten
		 * in the merging. We update the vmcs01 here for L1 as well
		 * since it will end up touching the MSR anyway now.
		 */
		vmx_disable_intercept_for_msr(vcpu,
					      MSR_IA32_SPEC_CTRL,
					      MSR_TYPE_RW);
		break;
	case MSR_IA32_TSX_CTRL:
		if (!msr_info->host_initiated &&
		    !(vcpu->arch.arch_capabilities & ARCH_CAP_TSX_CTRL_MSR))
			return 1;
		if (data & ~(TSX_CTRL_RTM_DISABLE | TSX_CTRL_CPUID_CLEAR))
			return 1;
		goto find_uret_msr;
	case MSR_IA32_PRED_CMD:
		if (!msr_info->host_initiated &&
		    !guest_has_pred_cmd_msr(vcpu))
			return 1;

		if (data & ~PRED_CMD_IBPB)
			return 1;
		if (!boot_cpu_has(X86_FEATURE_IBPB))
			return 1;
		if (!data)
			break;

		wrmsrl(MSR_IA32_PRED_CMD, PRED_CMD_IBPB);

		/*
		 * For non-nested:
		 * When it's written (to non-zero) for the first time, pass
		 * it through.
		 *
		 * For nested:
		 * The handling of the MSR bitmap for L2 guests is done in
		 * nested_vmx_prepare_msr_bitmap. We should not touch the
		 * vmcs02.msr_bitmap here since it gets completely overwritten
		 * in the merging.
		 */
		vmx_disable_intercept_for_msr(vcpu, MSR_IA32_PRED_CMD, MSR_TYPE_W);
		break;
	case MSR_IA32_CR_PAT:
		if (!kvm_pat_valid(data))
			return 1;

		if (is_guest_mode(vcpu) &&
		    get_vmcs12(vcpu)->vm_exit_controls & VM_EXIT_SAVE_IA32_PAT)
			get_vmcs12(vcpu)->guest_ia32_pat = data;

		if (vmcs_config.vmentry_ctrl & VM_ENTRY_LOAD_IA32_PAT) {
			vmcs_write64(GUEST_IA32_PAT, data);
			vcpu->arch.pat = data;
			break;
		}
		ret = kvm_set_msr_common(vcpu, msr_info);
		break;
	case MSR_IA32_TSC_ADJUST:
		ret = kvm_set_msr_common(vcpu, msr_info);
		break;
	case MSR_IA32_MCG_EXT_CTL:
		if ((!msr_info->host_initiated &&
		     !(to_vmx(vcpu)->msr_ia32_feature_control &
		       FEAT_CTL_LMCE_ENABLED)) ||
		    (data & ~MCG_EXT_CTL_LMCE_EN))
			return 1;
		vcpu->arch.mcg_ext_ctl = data;
		break;
	case MSR_IA32_FEAT_CTL:
		if (!vmx_feature_control_msr_valid(vcpu, data) ||
		    (to_vmx(vcpu)->msr_ia32_feature_control &
		     FEAT_CTL_LOCKED && !msr_info->host_initiated))
			return 1;
		vmx->msr_ia32_feature_control = data;
		if (msr_info->host_initiated && data == 0)
			vmx_leave_nested(vcpu);

		/* SGX may be enabled/disabled by guest's firmware */
		vmx_write_encls_bitmap(vcpu, NULL);
		break;
	case MSR_IA32_SGXLEPUBKEYHASH0 ... MSR_IA32_SGXLEPUBKEYHASH3:
		/*
		 * On real hardware, the LE hash MSRs are writable before
		 * the firmware sets bit 0 in MSR 0x7a ("activating" SGX),
		 * at which point SGX related bits in IA32_FEATURE_CONTROL
		 * become writable.
		 *
		 * KVM does not emulate SGX activation for simplicity, so
		 * allow writes to the LE hash MSRs if IA32_FEATURE_CONTROL
		 * is unlocked.  This is technically not architectural
		 * behavior, but it's close enough.
		 */
		if (!msr_info->host_initiated &&
		    (!guest_cpuid_has(vcpu, X86_FEATURE_SGX_LC) ||
		    ((vmx->msr_ia32_feature_control & FEAT_CTL_LOCKED) &&
		    !(vmx->msr_ia32_feature_control & FEAT_CTL_SGX_LC_ENABLED))))
			return 1;
		vmx->msr_ia32_sgxlepubkeyhash
			[msr_index - MSR_IA32_SGXLEPUBKEYHASH0] = data;
		break;
	case MSR_IA32_VMX_BASIC ... MSR_IA32_VMX_VMFUNC:
		if (!msr_info->host_initiated)
			return 1; /* they are read-only */
		if (!nested_vmx_allowed(vcpu))
			return 1;
		return vmx_set_vmx_msr(vcpu, msr_index, data);
	case MSR_IA32_RTIT_CTL:
		if (!vmx_pt_mode_is_host_guest() ||
			vmx_rtit_ctl_check(vcpu, data) ||
			vmx->nested.vmxon)
			return 1;
		vmcs_write64(GUEST_IA32_RTIT_CTL, data);
		vmx->pt_desc.guest.ctl = data;
		pt_update_intercept_for_msr(vcpu);
		break;
	case MSR_IA32_RTIT_STATUS:
		if (!pt_can_write_msr(vmx))
			return 1;
		if (data & MSR_IA32_RTIT_STATUS_MASK)
			return 1;
		vmx->pt_desc.guest.status = data;
		break;
	case MSR_IA32_RTIT_CR3_MATCH:
		if (!pt_can_write_msr(vmx))
			return 1;
		if (!intel_pt_validate_cap(vmx->pt_desc.caps,
					   PT_CAP_cr3_filtering))
			return 1;
		vmx->pt_desc.guest.cr3_match = data;
		break;
	case MSR_IA32_RTIT_OUTPUT_BASE:
		if (!pt_can_write_msr(vmx))
			return 1;
		if (!intel_pt_validate_cap(vmx->pt_desc.caps,
					   PT_CAP_topa_output) &&
		    !intel_pt_validate_cap(vmx->pt_desc.caps,
					   PT_CAP_single_range_output))
			return 1;
		if (!pt_output_base_valid(vcpu, data))
			return 1;
		vmx->pt_desc.guest.output_base = data;
		break;
	case MSR_IA32_RTIT_OUTPUT_MASK:
		if (!pt_can_write_msr(vmx))
			return 1;
		if (!intel_pt_validate_cap(vmx->pt_desc.caps,
					   PT_CAP_topa_output) &&
		    !intel_pt_validate_cap(vmx->pt_desc.caps,
					   PT_CAP_single_range_output))
			return 1;
		vmx->pt_desc.guest.output_mask = data;
		break;
	case MSR_IA32_RTIT_ADDR0_A ... MSR_IA32_RTIT_ADDR3_B:
		if (!pt_can_write_msr(vmx))
			return 1;
		index = msr_info->index - MSR_IA32_RTIT_ADDR0_A;
		if (index >= 2 * intel_pt_validate_cap(vmx->pt_desc.caps,
						       PT_CAP_num_address_ranges))
			return 1;
		if (is_noncanonical_address(data, vcpu))
			return 1;
		if (index % 2)
			vmx->pt_desc.guest.addr_b[index / 2] = data;
		else
			vmx->pt_desc.guest.addr_a[index / 2] = data;
		break;
	case MSR_IA32_PERF_CAPABILITIES:
		if (data && !vcpu_to_pmu(vcpu)->version)
			return 1;
		if (data & PMU_CAP_LBR_FMT) {
			if ((data & PMU_CAP_LBR_FMT) !=
			    (vmx_get_perf_capabilities() & PMU_CAP_LBR_FMT))
				return 1;
			if (!intel_pmu_lbr_is_compatible(vcpu))
				return 1;
		}
		ret = kvm_set_msr_common(vcpu, msr_info);
		break;

	default:
	find_uret_msr:
		msr = vmx_find_uret_msr(vmx, msr_index);
		if (msr)
			ret = vmx_set_guest_uret_msr(vmx, msr, data);
		else
			ret = kvm_set_msr_common(vcpu, msr_info);
	}

	return ret;
}

static void vmx_cache_reg(struct kvm_vcpu *vcpu, enum kvm_reg reg)
{
	unsigned long guest_owned_bits;

	kvm_register_mark_available(vcpu, reg);

	switch (reg) {
	case VCPU_REGS_RSP:
		vcpu->arch.regs[VCPU_REGS_RSP] = vmcs_readl(GUEST_RSP);
		break;
	case VCPU_REGS_RIP:
		vcpu->arch.regs[VCPU_REGS_RIP] = vmcs_readl(GUEST_RIP);
		break;
	case VCPU_EXREG_PDPTR:
		if (enable_ept)
			ept_save_pdptrs(vcpu);
		break;
	case VCPU_EXREG_CR0:
		guest_owned_bits = vcpu->arch.cr0_guest_owned_bits;

		vcpu->arch.cr0 &= ~guest_owned_bits;
		vcpu->arch.cr0 |= vmcs_readl(GUEST_CR0) & guest_owned_bits;
		break;
	case VCPU_EXREG_CR3:
		if (is_unrestricted_guest(vcpu) ||
		    (enable_ept && is_paging(vcpu)))
			vcpu->arch.cr3 = vmcs_readl(GUEST_CR3);
		break;
	case VCPU_EXREG_CR4:
		guest_owned_bits = vcpu->arch.cr4_guest_owned_bits;

		vcpu->arch.cr4 &= ~guest_owned_bits;
		vcpu->arch.cr4 |= vmcs_readl(GUEST_CR4) & guest_owned_bits;
		break;
	default:
		WARN_ON_ONCE(1);
		break;
	}
}

static __init int cpu_has_kvm_support(void)
{
	return cpu_has_vmx();
}

static __init int vmx_disabled_by_bios(void)
{
	return !boot_cpu_has(X86_FEATURE_MSR_IA32_FEAT_CTL) ||
	       !boot_cpu_has(X86_FEATURE_VMX);
}

static int kvm_cpu_vmxon(u64 vmxon_pointer)
{
	u64 msr;

	cr4_set_bits(X86_CR4_VMXE);

	asm_volatile_goto("1: vmxon %[vmxon_pointer]\n\t"
			  _ASM_EXTABLE(1b, %l[fault])
			  : : [vmxon_pointer] "m"(vmxon_pointer)
			  : : fault);
	return 0;

fault:
	WARN_ONCE(1, "VMXON faulted, MSR_IA32_FEAT_CTL (0x3a) = 0x%llx\n",
		  rdmsrl_safe(MSR_IA32_FEAT_CTL, &msr) ? 0xdeadbeef : msr);
	cr4_clear_bits(X86_CR4_VMXE);

	return -EFAULT;
}

static int hardware_enable(void)
{
	int cpu = raw_smp_processor_id();
	u64 phys_addr = __pa(per_cpu(vmxarea, cpu));
	int r;

	if (cr4_read_shadow() & X86_CR4_VMXE)
		return -EBUSY;

	/*
	 * This can happen if we hot-added a CPU but failed to allocate
	 * VP assist page for it.
	 */
	if (static_branch_unlikely(&enable_evmcs) &&
	    !hv_get_vp_assist_page(cpu))
		return -EFAULT;

	intel_pt_handle_vmx(1);

	r = kvm_cpu_vmxon(phys_addr);
	if (r) {
		intel_pt_handle_vmx(0);
		return r;
	}

	if (enable_ept)
		ept_sync_global();

	return 0;
}

static void vmclear_local_loaded_vmcss(void)
{
	int cpu = raw_smp_processor_id();
	struct loaded_vmcs *v, *n;

	list_for_each_entry_safe(v, n, &per_cpu(loaded_vmcss_on_cpu, cpu),
				 loaded_vmcss_on_cpu_link)
		__loaded_vmcs_clear(v);
}

static void hardware_disable(void)
{
	vmclear_local_loaded_vmcss();

	if (cpu_vmxoff())
		kvm_spurious_fault();

	intel_pt_handle_vmx(0);
}

/*
 * There is no X86_FEATURE for SGX yet, but anyway we need to query CPUID
 * directly instead of going through cpu_has(), to ensure KVM is trapping
 * ENCLS whenever it's supported in hardware.  It does not matter whether
 * the host OS supports or has enabled SGX.
 */
static bool cpu_has_sgx(void)
{
	return cpuid_eax(0) >= 0x12 && (cpuid_eax(0x12) & BIT(0));
}

static __init int adjust_vmx_controls(u32 ctl_min, u32 ctl_opt,
				      u32 msr, u32 *result)
{
	u32 vmx_msr_low, vmx_msr_high;
	u32 ctl = ctl_min | ctl_opt;

	rdmsr(msr, vmx_msr_low, vmx_msr_high);

	ctl &= vmx_msr_high; /* bit == 0 in high word ==> must be zero */
	ctl |= vmx_msr_low;  /* bit == 1 in low word  ==> must be one  */

	/* Ensure minimum (required) set of control bits are supported. */
	if (ctl_min & ~ctl)
		return -EIO;

	*result = ctl;
	return 0;
}

static __init int setup_vmcs_config(struct vmcs_config *vmcs_conf,
				    struct vmx_capability *vmx_cap)
{
	u32 vmx_msr_low, vmx_msr_high;
	u32 min, opt, min2, opt2;
	u32 _pin_based_exec_control = 0;
	u32 _cpu_based_exec_control = 0;
	u32 _cpu_based_2nd_exec_control = 0;
	u32 _vmexit_control = 0;
	u32 _vmentry_control = 0;

	memset(vmcs_conf, 0, sizeof(*vmcs_conf));
	min = CPU_BASED_HLT_EXITING |
#ifdef CONFIG_X86_64
	      CPU_BASED_CR8_LOAD_EXITING |
	      CPU_BASED_CR8_STORE_EXITING |
#endif
	      CPU_BASED_CR3_LOAD_EXITING |
	      CPU_BASED_CR3_STORE_EXITING |
	      CPU_BASED_UNCOND_IO_EXITING |
	      CPU_BASED_MOV_DR_EXITING |
	      CPU_BASED_USE_TSC_OFFSETTING |
	      CPU_BASED_MWAIT_EXITING |
	      CPU_BASED_MONITOR_EXITING |
	      CPU_BASED_INVLPG_EXITING |
	      CPU_BASED_RDPMC_EXITING;

	opt = CPU_BASED_TPR_SHADOW |
	      CPU_BASED_USE_MSR_BITMAPS |
	      CPU_BASED_ACTIVATE_SECONDARY_CONTROLS;
	if (adjust_vmx_controls(min, opt, MSR_IA32_VMX_PROCBASED_CTLS,
				&_cpu_based_exec_control) < 0)
		return -EIO;
#ifdef CONFIG_X86_64
	if ((_cpu_based_exec_control & CPU_BASED_TPR_SHADOW))
		_cpu_based_exec_control &= ~CPU_BASED_CR8_LOAD_EXITING &
					   ~CPU_BASED_CR8_STORE_EXITING;
#endif
	if (_cpu_based_exec_control & CPU_BASED_ACTIVATE_SECONDARY_CONTROLS) {
		min2 = 0;
		opt2 = SECONDARY_EXEC_VIRTUALIZE_APIC_ACCESSES |
			SECONDARY_EXEC_VIRTUALIZE_X2APIC_MODE |
			SECONDARY_EXEC_WBINVD_EXITING |
			SECONDARY_EXEC_ENABLE_VPID |
			SECONDARY_EXEC_ENABLE_EPT |
			SECONDARY_EXEC_UNRESTRICTED_GUEST |
			SECONDARY_EXEC_PAUSE_LOOP_EXITING |
			SECONDARY_EXEC_DESC |
			SECONDARY_EXEC_ENABLE_RDTSCP |
			SECONDARY_EXEC_ENABLE_INVPCID |
			SECONDARY_EXEC_APIC_REGISTER_VIRT |
			SECONDARY_EXEC_VIRTUAL_INTR_DELIVERY |
			SECONDARY_EXEC_SHADOW_VMCS |
			SECONDARY_EXEC_XSAVES |
			SECONDARY_EXEC_RDSEED_EXITING |
			SECONDARY_EXEC_RDRAND_EXITING |
			SECONDARY_EXEC_ENABLE_PML |
			SECONDARY_EXEC_TSC_SCALING |
			SECONDARY_EXEC_ENABLE_USR_WAIT_PAUSE |
			SECONDARY_EXEC_PT_USE_GPA |
			SECONDARY_EXEC_PT_CONCEAL_VMX |
			SECONDARY_EXEC_ENABLE_VMFUNC |
			SECONDARY_EXEC_BUS_LOCK_DETECTION;
		if (cpu_has_sgx())
			opt2 |= SECONDARY_EXEC_ENCLS_EXITING;
		if (adjust_vmx_controls(min2, opt2,
					MSR_IA32_VMX_PROCBASED_CTLS2,
					&_cpu_based_2nd_exec_control) < 0)
			return -EIO;
	}
#ifndef CONFIG_X86_64
	if (!(_cpu_based_2nd_exec_control &
				SECONDARY_EXEC_VIRTUALIZE_APIC_ACCESSES))
		_cpu_based_exec_control &= ~CPU_BASED_TPR_SHADOW;
#endif

	if (!(_cpu_based_exec_control & CPU_BASED_TPR_SHADOW))
		_cpu_based_2nd_exec_control &= ~(
				SECONDARY_EXEC_APIC_REGISTER_VIRT |
				SECONDARY_EXEC_VIRTUALIZE_X2APIC_MODE |
				SECONDARY_EXEC_VIRTUAL_INTR_DELIVERY);

	rdmsr_safe(MSR_IA32_VMX_EPT_VPID_CAP,
		&vmx_cap->ept, &vmx_cap->vpid);

	if (_cpu_based_2nd_exec_control & SECONDARY_EXEC_ENABLE_EPT) {
		/* CR3 accesses and invlpg don't need to cause VM Exits when EPT
		   enabled */
		_cpu_based_exec_control &= ~(CPU_BASED_CR3_LOAD_EXITING |
					     CPU_BASED_CR3_STORE_EXITING |
					     CPU_BASED_INVLPG_EXITING);
	} else if (vmx_cap->ept) {
		vmx_cap->ept = 0;
		pr_warn_once("EPT CAP should not exist if not support "
				"1-setting enable EPT VM-execution control\n");
	}
	if (!(_cpu_based_2nd_exec_control & SECONDARY_EXEC_ENABLE_VPID) &&
		vmx_cap->vpid) {
		vmx_cap->vpid = 0;
		pr_warn_once("VPID CAP should not exist if not support "
				"1-setting enable VPID VM-execution control\n");
	}

	min = VM_EXIT_SAVE_DEBUG_CONTROLS | VM_EXIT_ACK_INTR_ON_EXIT;
#ifdef CONFIG_X86_64
	min |= VM_EXIT_HOST_ADDR_SPACE_SIZE;
#endif
	opt = VM_EXIT_LOAD_IA32_PERF_GLOBAL_CTRL |
	      VM_EXIT_LOAD_IA32_PAT |
	      VM_EXIT_LOAD_IA32_EFER |
	      VM_EXIT_CLEAR_BNDCFGS |
	      VM_EXIT_PT_CONCEAL_PIP |
	      VM_EXIT_CLEAR_IA32_RTIT_CTL;
	if (adjust_vmx_controls(min, opt, MSR_IA32_VMX_EXIT_CTLS,
				&_vmexit_control) < 0)
		return -EIO;

	min = PIN_BASED_EXT_INTR_MASK | PIN_BASED_NMI_EXITING;
	opt = PIN_BASED_VIRTUAL_NMIS | PIN_BASED_POSTED_INTR |
		 PIN_BASED_VMX_PREEMPTION_TIMER;
	if (adjust_vmx_controls(min, opt, MSR_IA32_VMX_PINBASED_CTLS,
				&_pin_based_exec_control) < 0)
		return -EIO;

	if (cpu_has_broken_vmx_preemption_timer())
		_pin_based_exec_control &= ~PIN_BASED_VMX_PREEMPTION_TIMER;
	if (!(_cpu_based_2nd_exec_control &
		SECONDARY_EXEC_VIRTUAL_INTR_DELIVERY))
		_pin_based_exec_control &= ~PIN_BASED_POSTED_INTR;

	min = VM_ENTRY_LOAD_DEBUG_CONTROLS;
	opt = VM_ENTRY_LOAD_IA32_PERF_GLOBAL_CTRL |
	      VM_ENTRY_LOAD_IA32_PAT |
	      VM_ENTRY_LOAD_IA32_EFER |
	      VM_ENTRY_LOAD_BNDCFGS |
	      VM_ENTRY_PT_CONCEAL_PIP |
	      VM_ENTRY_LOAD_IA32_RTIT_CTL;
	if (adjust_vmx_controls(min, opt, MSR_IA32_VMX_ENTRY_CTLS,
				&_vmentry_control) < 0)
		return -EIO;

	/*
	 * Some cpus support VM_{ENTRY,EXIT}_IA32_PERF_GLOBAL_CTRL but they
	 * can't be used due to an errata where VM Exit may incorrectly clear
	 * IA32_PERF_GLOBAL_CTRL[34:32].  Workaround the errata by using the
	 * MSR load mechanism to switch IA32_PERF_GLOBAL_CTRL.
	 */
	if (boot_cpu_data.x86 == 0x6) {
		switch (boot_cpu_data.x86_model) {
		case 26: /* AAK155 */
		case 30: /* AAP115 */
		case 37: /* AAT100 */
		case 44: /* BC86,AAY89,BD102 */
		case 46: /* BA97 */
			_vmentry_control &= ~VM_ENTRY_LOAD_IA32_PERF_GLOBAL_CTRL;
			_vmexit_control &= ~VM_EXIT_LOAD_IA32_PERF_GLOBAL_CTRL;
			pr_warn_once("kvm: VM_EXIT_LOAD_IA32_PERF_GLOBAL_CTRL "
					"does not work properly. Using workaround\n");
			break;
		default:
			break;
		}
	}


	rdmsr(MSR_IA32_VMX_BASIC, vmx_msr_low, vmx_msr_high);

	/* IA-32 SDM Vol 3B: VMCS size is never greater than 4kB. */
	if ((vmx_msr_high & 0x1fff) > PAGE_SIZE)
		return -EIO;

#ifdef CONFIG_X86_64
	/* IA-32 SDM Vol 3B: 64-bit CPUs always have VMX_BASIC_MSR[48]==0. */
	if (vmx_msr_high & (1u<<16))
		return -EIO;
#endif

	/* Require Write-Back (WB) memory type for VMCS accesses. */
	if (((vmx_msr_high >> 18) & 15) != 6)
		return -EIO;

	vmcs_conf->size = vmx_msr_high & 0x1fff;
	vmcs_conf->order = get_order(vmcs_conf->size);
	vmcs_conf->basic_cap = vmx_msr_high & ~0x1fff;

	vmcs_conf->revision_id = vmx_msr_low;

	vmcs_conf->pin_based_exec_ctrl = _pin_based_exec_control;
	vmcs_conf->cpu_based_exec_ctrl = _cpu_based_exec_control;
	vmcs_conf->cpu_based_2nd_exec_ctrl = _cpu_based_2nd_exec_control;
	vmcs_conf->vmexit_ctrl         = _vmexit_control;
	vmcs_conf->vmentry_ctrl        = _vmentry_control;

#if IS_ENABLED(CONFIG_HYPERV)
	if (enlightened_vmcs)
		evmcs_sanitize_exec_ctrls(vmcs_conf);
#endif

	return 0;
}

struct vmcs *alloc_vmcs_cpu(bool shadow, int cpu, gfp_t flags)
{
	int node = cpu_to_node(cpu);
	struct page *pages;
	struct vmcs *vmcs;

	pages = __alloc_pages_node(node, flags, vmcs_config.order);
	if (!pages)
		return NULL;
	vmcs = page_address(pages);
	memset(vmcs, 0, vmcs_config.size);

	/* KVM supports Enlightened VMCS v1 only */
	if (static_branch_unlikely(&enable_evmcs))
		vmcs->hdr.revision_id = KVM_EVMCS_VERSION;
	else
		vmcs->hdr.revision_id = vmcs_config.revision_id;

	if (shadow)
		vmcs->hdr.shadow_vmcs = 1;
	return vmcs;
}

void free_vmcs(struct vmcs *vmcs)
{
	free_pages((unsigned long)vmcs, vmcs_config.order);
}

/*
 * Free a VMCS, but before that VMCLEAR it on the CPU where it was last loaded
 */
void free_loaded_vmcs(struct loaded_vmcs *loaded_vmcs)
{
	if (!loaded_vmcs->vmcs)
		return;
	loaded_vmcs_clear(loaded_vmcs);
	free_vmcs(loaded_vmcs->vmcs);
	loaded_vmcs->vmcs = NULL;
	if (loaded_vmcs->msr_bitmap)
		free_page((unsigned long)loaded_vmcs->msr_bitmap);
	WARN_ON(loaded_vmcs->shadow_vmcs != NULL);
}

int alloc_loaded_vmcs(struct loaded_vmcs *loaded_vmcs)
{
	loaded_vmcs->vmcs = alloc_vmcs(false);
	if (!loaded_vmcs->vmcs)
		return -ENOMEM;

	vmcs_clear(loaded_vmcs->vmcs);

	loaded_vmcs->shadow_vmcs = NULL;
	loaded_vmcs->hv_timer_soft_disabled = false;
	loaded_vmcs->cpu = -1;
	loaded_vmcs->launched = 0;

	if (cpu_has_vmx_msr_bitmap()) {
		loaded_vmcs->msr_bitmap = (unsigned long *)
				__get_free_page(GFP_KERNEL_ACCOUNT);
		if (!loaded_vmcs->msr_bitmap)
			goto out_vmcs;
		memset(loaded_vmcs->msr_bitmap, 0xff, PAGE_SIZE);

		if (IS_ENABLED(CONFIG_HYPERV) &&
		    static_branch_unlikely(&enable_evmcs) &&
		    (ms_hyperv.nested_features & HV_X64_NESTED_MSR_BITMAP)) {
			struct hv_enlightened_vmcs *evmcs =
				(struct hv_enlightened_vmcs *)loaded_vmcs->vmcs;

			evmcs->hv_enlightenments_control.msr_bitmap = 1;
		}
	}

	memset(&loaded_vmcs->host_state, 0, sizeof(struct vmcs_host_state));
	memset(&loaded_vmcs->controls_shadow, 0,
		sizeof(struct vmcs_controls_shadow));

	return 0;

out_vmcs:
	free_loaded_vmcs(loaded_vmcs);
	return -ENOMEM;
}

static void free_kvm_area(void)
{
	int cpu;

	for_each_possible_cpu(cpu) {
		free_vmcs(per_cpu(vmxarea, cpu));
		per_cpu(vmxarea, cpu) = NULL;
	}
}

static __init int alloc_kvm_area(void)
{
	int cpu;

	for_each_possible_cpu(cpu) {
		struct vmcs *vmcs;

		vmcs = alloc_vmcs_cpu(false, cpu, GFP_KERNEL);
		if (!vmcs) {
			free_kvm_area();
			return -ENOMEM;
		}

		/*
		 * When eVMCS is enabled, alloc_vmcs_cpu() sets
		 * vmcs->revision_id to KVM_EVMCS_VERSION instead of
		 * revision_id reported by MSR_IA32_VMX_BASIC.
		 *
		 * However, even though not explicitly documented by
		 * TLFS, VMXArea passed as VMXON argument should
		 * still be marked with revision_id reported by
		 * physical CPU.
		 */
		if (static_branch_unlikely(&enable_evmcs))
			vmcs->hdr.revision_id = vmcs_config.revision_id;

		per_cpu(vmxarea, cpu) = vmcs;
	}
	return 0;
}

static void fix_pmode_seg(struct kvm_vcpu *vcpu, int seg,
		struct kvm_segment *save)
{
	if (!emulate_invalid_guest_state) {
		/*
		 * CS and SS RPL should be equal during guest entry according
		 * to VMX spec, but in reality it is not always so. Since vcpu
		 * is in the middle of the transition from real mode to
		 * protected mode it is safe to assume that RPL 0 is a good
		 * default value.
		 */
		if (seg == VCPU_SREG_CS || seg == VCPU_SREG_SS)
			save->selector &= ~SEGMENT_RPL_MASK;
		save->dpl = save->selector & SEGMENT_RPL_MASK;
		save->s = 1;
	}
	vmx_set_segment(vcpu, save, seg);
}

static void enter_pmode(struct kvm_vcpu *vcpu)
{
	unsigned long flags;
	struct vcpu_vmx *vmx = to_vmx(vcpu);

	/*
	 * Update real mode segment cache. It may be not up-to-date if segment
	 * register was written while vcpu was in a guest mode.
	 */
	vmx_get_segment(vcpu, &vmx->rmode.segs[VCPU_SREG_ES], VCPU_SREG_ES);
	vmx_get_segment(vcpu, &vmx->rmode.segs[VCPU_SREG_DS], VCPU_SREG_DS);
	vmx_get_segment(vcpu, &vmx->rmode.segs[VCPU_SREG_FS], VCPU_SREG_FS);
	vmx_get_segment(vcpu, &vmx->rmode.segs[VCPU_SREG_GS], VCPU_SREG_GS);
	vmx_get_segment(vcpu, &vmx->rmode.segs[VCPU_SREG_SS], VCPU_SREG_SS);
	vmx_get_segment(vcpu, &vmx->rmode.segs[VCPU_SREG_CS], VCPU_SREG_CS);

	vmx->rmode.vm86_active = 0;

	vmx_set_segment(vcpu, &vmx->rmode.segs[VCPU_SREG_TR], VCPU_SREG_TR);

	flags = vmcs_readl(GUEST_RFLAGS);
	flags &= RMODE_GUEST_OWNED_EFLAGS_BITS;
	flags |= vmx->rmode.save_rflags & ~RMODE_GUEST_OWNED_EFLAGS_BITS;
	vmcs_writel(GUEST_RFLAGS, flags);

	vmcs_writel(GUEST_CR4, (vmcs_readl(GUEST_CR4) & ~X86_CR4_VME) |
			(vmcs_readl(CR4_READ_SHADOW) & X86_CR4_VME));

	vmx_update_exception_bitmap(vcpu);

	fix_pmode_seg(vcpu, VCPU_SREG_CS, &vmx->rmode.segs[VCPU_SREG_CS]);
	fix_pmode_seg(vcpu, VCPU_SREG_SS, &vmx->rmode.segs[VCPU_SREG_SS]);
	fix_pmode_seg(vcpu, VCPU_SREG_ES, &vmx->rmode.segs[VCPU_SREG_ES]);
	fix_pmode_seg(vcpu, VCPU_SREG_DS, &vmx->rmode.segs[VCPU_SREG_DS]);
	fix_pmode_seg(vcpu, VCPU_SREG_FS, &vmx->rmode.segs[VCPU_SREG_FS]);
	fix_pmode_seg(vcpu, VCPU_SREG_GS, &vmx->rmode.segs[VCPU_SREG_GS]);
}

static void fix_rmode_seg(int seg, struct kvm_segment *save)
{
	const struct kvm_vmx_segment_field *sf = &kvm_vmx_segment_fields[seg];
	struct kvm_segment var = *save;

	var.dpl = 0x3;
	if (seg == VCPU_SREG_CS)
		var.type = 0x3;

	if (!emulate_invalid_guest_state) {
		var.selector = var.base >> 4;
		var.base = var.base & 0xffff0;
		var.limit = 0xffff;
		var.g = 0;
		var.db = 0;
		var.present = 1;
		var.s = 1;
		var.l = 0;
		var.unusable = 0;
		var.type = 0x3;
		var.avl = 0;
		if (save->base & 0xf)
			printk_once(KERN_WARNING "kvm: segment base is not "
					"paragraph aligned when entering "
					"protected mode (seg=%d)", seg);
	}

	vmcs_write16(sf->selector, var.selector);
	vmcs_writel(sf->base, var.base);
	vmcs_write32(sf->limit, var.limit);
	vmcs_write32(sf->ar_bytes, vmx_segment_access_rights(&var));
}

static void enter_rmode(struct kvm_vcpu *vcpu)
{
	unsigned long flags;
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	struct kvm_vmx *kvm_vmx = to_kvm_vmx(vcpu->kvm);

	vmx_get_segment(vcpu, &vmx->rmode.segs[VCPU_SREG_TR], VCPU_SREG_TR);
	vmx_get_segment(vcpu, &vmx->rmode.segs[VCPU_SREG_ES], VCPU_SREG_ES);
	vmx_get_segment(vcpu, &vmx->rmode.segs[VCPU_SREG_DS], VCPU_SREG_DS);
	vmx_get_segment(vcpu, &vmx->rmode.segs[VCPU_SREG_FS], VCPU_SREG_FS);
	vmx_get_segment(vcpu, &vmx->rmode.segs[VCPU_SREG_GS], VCPU_SREG_GS);
	vmx_get_segment(vcpu, &vmx->rmode.segs[VCPU_SREG_SS], VCPU_SREG_SS);
	vmx_get_segment(vcpu, &vmx->rmode.segs[VCPU_SREG_CS], VCPU_SREG_CS);

	vmx->rmode.vm86_active = 1;

	/*
	 * Very old userspace does not call KVM_SET_TSS_ADDR before entering
	 * vcpu. Warn the user that an update is overdue.
	 */
	if (!kvm_vmx->tss_addr)
		printk_once(KERN_WARNING "kvm: KVM_SET_TSS_ADDR need to be "
			     "called before entering vcpu\n");

	vmx_segment_cache_clear(vmx);

	vmcs_writel(GUEST_TR_BASE, kvm_vmx->tss_addr);
	vmcs_write32(GUEST_TR_LIMIT, RMODE_TSS_SIZE - 1);
	vmcs_write32(GUEST_TR_AR_BYTES, 0x008b);

	flags = vmcs_readl(GUEST_RFLAGS);
	vmx->rmode.save_rflags = flags;

	flags |= X86_EFLAGS_IOPL | X86_EFLAGS_VM;

	vmcs_writel(GUEST_RFLAGS, flags);
	vmcs_writel(GUEST_CR4, vmcs_readl(GUEST_CR4) | X86_CR4_VME);
	vmx_update_exception_bitmap(vcpu);

	fix_rmode_seg(VCPU_SREG_SS, &vmx->rmode.segs[VCPU_SREG_SS]);
	fix_rmode_seg(VCPU_SREG_CS, &vmx->rmode.segs[VCPU_SREG_CS]);
	fix_rmode_seg(VCPU_SREG_ES, &vmx->rmode.segs[VCPU_SREG_ES]);
	fix_rmode_seg(VCPU_SREG_DS, &vmx->rmode.segs[VCPU_SREG_DS]);
	fix_rmode_seg(VCPU_SREG_GS, &vmx->rmode.segs[VCPU_SREG_GS]);
	fix_rmode_seg(VCPU_SREG_FS, &vmx->rmode.segs[VCPU_SREG_FS]);

	kvm_mmu_reset_context(vcpu);
}

int vmx_set_efer(struct kvm_vcpu *vcpu, u64 efer)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	struct vmx_uret_msr *msr = vmx_find_uret_msr(vmx, MSR_EFER);

	/* Nothing to do if hardware doesn't support EFER. */
	if (!msr)
		return 0;

	vcpu->arch.efer = efer;
	if (efer & EFER_LMA) {
		vm_entry_controls_setbit(to_vmx(vcpu), VM_ENTRY_IA32E_MODE);
		msr->data = efer;
	} else {
		vm_entry_controls_clearbit(to_vmx(vcpu), VM_ENTRY_IA32E_MODE);

		msr->data = efer & ~EFER_LME;
	}
	setup_msrs(vmx);
	return 0;
}

#ifdef CONFIG_X86_64

static void enter_lmode(struct kvm_vcpu *vcpu)
{
	u32 guest_tr_ar;

	vmx_segment_cache_clear(to_vmx(vcpu));

	guest_tr_ar = vmcs_read32(GUEST_TR_AR_BYTES);
	if ((guest_tr_ar & VMX_AR_TYPE_MASK) != VMX_AR_TYPE_BUSY_64_TSS) {
		pr_debug_ratelimited("%s: tss fixup for long mode. \n",
				     __func__);
		vmcs_write32(GUEST_TR_AR_BYTES,
			     (guest_tr_ar & ~VMX_AR_TYPE_MASK)
			     | VMX_AR_TYPE_BUSY_64_TSS);
	}
	vmx_set_efer(vcpu, vcpu->arch.efer | EFER_LMA);
}

static void exit_lmode(struct kvm_vcpu *vcpu)
{
	vm_entry_controls_clearbit(to_vmx(vcpu), VM_ENTRY_IA32E_MODE);
	vmx_set_efer(vcpu, vcpu->arch.efer & ~EFER_LMA);
}

#endif

static void vmx_flush_tlb_all(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);

	/*
	 * INVEPT must be issued when EPT is enabled, irrespective of VPID, as
	 * the CPU is not required to invalidate guest-physical mappings on
	 * VM-Entry, even if VPID is disabled.  Guest-physical mappings are
	 * associated with the root EPT structure and not any particular VPID
	 * (INVVPID also isn't required to invalidate guest-physical mappings).
	 */
	if (enable_ept) {
		ept_sync_global();
	} else if (enable_vpid) {
		if (cpu_has_vmx_invvpid_global()) {
			vpid_sync_vcpu_global();
		} else {
			vpid_sync_vcpu_single(vmx->vpid);
			vpid_sync_vcpu_single(vmx->nested.vpid02);
		}
	}
}

static void vmx_flush_tlb_current(struct kvm_vcpu *vcpu)
{
	struct kvm_mmu *mmu = vcpu->arch.mmu;
	u64 root_hpa = mmu->root_hpa;

	/* No flush required if the current context is invalid. */
	if (!VALID_PAGE(root_hpa))
		return;

	if (enable_ept)
		ept_sync_context(construct_eptp(vcpu, root_hpa,
						mmu->shadow_root_level));
	else if (!is_guest_mode(vcpu))
		vpid_sync_context(to_vmx(vcpu)->vpid);
	else
		vpid_sync_context(nested_get_vpid02(vcpu));
}

static void vmx_flush_tlb_gva(struct kvm_vcpu *vcpu, gva_t addr)
{
	/*
	 * vpid_sync_vcpu_addr() is a nop if vmx->vpid==0, see the comment in
	 * vmx_flush_tlb_guest() for an explanation of why this is ok.
	 */
	vpid_sync_vcpu_addr(to_vmx(vcpu)->vpid, addr);
}

static void vmx_flush_tlb_guest(struct kvm_vcpu *vcpu)
{
	/*
	 * vpid_sync_context() is a nop if vmx->vpid==0, e.g. if enable_vpid==0
	 * or a vpid couldn't be allocated for this vCPU.  VM-Enter and VM-Exit
	 * are required to flush GVA->{G,H}PA mappings from the TLB if vpid is
	 * disabled (VM-Enter with vpid enabled and vpid==0 is disallowed),
	 * i.e. no explicit INVVPID is necessary.
	 */
	vpid_sync_context(to_vmx(vcpu)->vpid);
}

void vmx_ept_load_pdptrs(struct kvm_vcpu *vcpu)
{
	struct kvm_mmu *mmu = vcpu->arch.walk_mmu;

	if (!kvm_register_is_dirty(vcpu, VCPU_EXREG_PDPTR))
		return;

	if (is_pae_paging(vcpu)) {
		vmcs_write64(GUEST_PDPTR0, mmu->pdptrs[0]);
		vmcs_write64(GUEST_PDPTR1, mmu->pdptrs[1]);
		vmcs_write64(GUEST_PDPTR2, mmu->pdptrs[2]);
		vmcs_write64(GUEST_PDPTR3, mmu->pdptrs[3]);
	}
}

void ept_save_pdptrs(struct kvm_vcpu *vcpu)
{
	struct kvm_mmu *mmu = vcpu->arch.walk_mmu;

	if (WARN_ON_ONCE(!is_pae_paging(vcpu)))
		return;

	mmu->pdptrs[0] = vmcs_read64(GUEST_PDPTR0);
	mmu->pdptrs[1] = vmcs_read64(GUEST_PDPTR1);
	mmu->pdptrs[2] = vmcs_read64(GUEST_PDPTR2);
	mmu->pdptrs[3] = vmcs_read64(GUEST_PDPTR3);

	kvm_register_mark_dirty(vcpu, VCPU_EXREG_PDPTR);
}

static void ept_update_paging_mode_cr0(unsigned long *hw_cr0,
					unsigned long cr0,
					struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);

	if (!kvm_register_is_available(vcpu, VCPU_EXREG_CR3))
		vmx_cache_reg(vcpu, VCPU_EXREG_CR3);
	if (!(cr0 & X86_CR0_PG)) {
		/* From paging/starting to nonpaging */
		exec_controls_setbit(vmx, CPU_BASED_CR3_LOAD_EXITING |
					  CPU_BASED_CR3_STORE_EXITING);
		vcpu->arch.cr0 = cr0;
		vmx_set_cr4(vcpu, kvm_read_cr4(vcpu));
	} else if (!is_paging(vcpu)) {
		/* From nonpaging to paging */
		exec_controls_clearbit(vmx, CPU_BASED_CR3_LOAD_EXITING |
					    CPU_BASED_CR3_STORE_EXITING);
		vcpu->arch.cr0 = cr0;
		vmx_set_cr4(vcpu, kvm_read_cr4(vcpu));
	}

	if (!(cr0 & X86_CR0_WP))
		*hw_cr0 &= ~X86_CR0_WP;
}

void vmx_set_cr0(struct kvm_vcpu *vcpu, unsigned long cr0)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	unsigned long hw_cr0;

	hw_cr0 = (cr0 & ~KVM_VM_CR0_ALWAYS_OFF);
	if (is_unrestricted_guest(vcpu))
		hw_cr0 |= KVM_VM_CR0_ALWAYS_ON_UNRESTRICTED_GUEST;
	else {
		hw_cr0 |= KVM_VM_CR0_ALWAYS_ON;

		if (vmx->rmode.vm86_active && (cr0 & X86_CR0_PE))
			enter_pmode(vcpu);

		if (!vmx->rmode.vm86_active && !(cr0 & X86_CR0_PE))
			enter_rmode(vcpu);
	}

#ifdef CONFIG_X86_64
	if (vcpu->arch.efer & EFER_LME) {
		if (!is_paging(vcpu) && (cr0 & X86_CR0_PG))
			enter_lmode(vcpu);
		if (is_paging(vcpu) && !(cr0 & X86_CR0_PG))
			exit_lmode(vcpu);
	}
#endif

	if (enable_ept && !is_unrestricted_guest(vcpu))
		ept_update_paging_mode_cr0(&hw_cr0, cr0, vcpu);

	vmcs_writel(CR0_READ_SHADOW, cr0);
	vmcs_writel(GUEST_CR0, hw_cr0);
	vcpu->arch.cr0 = cr0;
	kvm_register_mark_available(vcpu, VCPU_EXREG_CR0);

	/* depends on vcpu->arch.cr0 to be set to a new value */
	vmx->emulation_required = emulation_required(vcpu);
}

static int vmx_get_max_tdp_level(void)
{
	if (cpu_has_vmx_ept_5levels())
		return 5;
	return 4;
}

u64 construct_eptp(struct kvm_vcpu *vcpu, hpa_t root_hpa, int root_level)
{
	u64 eptp = VMX_EPTP_MT_WB;

	eptp |= (root_level == 5) ? VMX_EPTP_PWL_5 : VMX_EPTP_PWL_4;

	if (enable_ept_ad_bits &&
	    (!is_guest_mode(vcpu) || nested_ept_ad_enabled(vcpu)))
		eptp |= VMX_EPTP_AD_ENABLE_BIT;
	eptp |= root_hpa;

	return eptp;
}

static void vmx_load_mmu_pgd(struct kvm_vcpu *vcpu, hpa_t root_hpa,
			     int root_level)
{
	struct kvm *kvm = vcpu->kvm;
	bool update_guest_cr3 = true;
	unsigned long guest_cr3;
	u64 eptp;

	if (enable_ept) {
		eptp = construct_eptp(vcpu, root_hpa, root_level);
		vmcs_write64(EPT_POINTER, eptp);

		hv_track_root_ept(vcpu, root_hpa);

		if (!enable_unrestricted_guest && !is_paging(vcpu))
			guest_cr3 = to_kvm_vmx(kvm)->ept_identity_map_addr;
		else if (test_bit(VCPU_EXREG_CR3, (ulong *)&vcpu->arch.regs_avail))
			guest_cr3 = vcpu->arch.cr3;
		else /* vmcs01.GUEST_CR3 is already up-to-date. */
			update_guest_cr3 = false;
		vmx_ept_load_pdptrs(vcpu);
	} else {
		guest_cr3 = root_hpa | kvm_get_active_pcid(vcpu);
	}

	if (update_guest_cr3)
		vmcs_writel(GUEST_CR3, guest_cr3);
}

static bool vmx_is_valid_cr4(struct kvm_vcpu *vcpu, unsigned long cr4)
{
	/*
	 * We operate under the default treatment of SMM, so VMX cannot be
	 * enabled under SMM.  Note, whether or not VMXE is allowed at all is
	 * handled by kvm_is_valid_cr4().
	 */
	if ((cr4 & X86_CR4_VMXE) && is_smm(vcpu))
		return false;

	if (to_vmx(vcpu)->nested.vmxon && !nested_cr4_valid(vcpu, cr4))
		return false;

	return true;
}

void vmx_set_cr4(struct kvm_vcpu *vcpu, unsigned long cr4)
{
	unsigned long old_cr4 = vcpu->arch.cr4;
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	/*
	 * Pass through host's Machine Check Enable value to hw_cr4, which
	 * is in force while we are in guest mode.  Do not let guests control
	 * this bit, even if host CR4.MCE == 0.
	 */
	unsigned long hw_cr4;

	hw_cr4 = (cr4_read_shadow() & X86_CR4_MCE) | (cr4 & ~X86_CR4_MCE);
	if (is_unrestricted_guest(vcpu))
		hw_cr4 |= KVM_VM_CR4_ALWAYS_ON_UNRESTRICTED_GUEST;
	else if (vmx->rmode.vm86_active)
		hw_cr4 |= KVM_RMODE_VM_CR4_ALWAYS_ON;
	else
		hw_cr4 |= KVM_PMODE_VM_CR4_ALWAYS_ON;

	if (!boot_cpu_has(X86_FEATURE_UMIP) && vmx_umip_emulated()) {
		if (cr4 & X86_CR4_UMIP) {
			secondary_exec_controls_setbit(vmx, SECONDARY_EXEC_DESC);
			hw_cr4 &= ~X86_CR4_UMIP;
		} else if (!is_guest_mode(vcpu) ||
			!nested_cpu_has2(get_vmcs12(vcpu), SECONDARY_EXEC_DESC)) {
			secondary_exec_controls_clearbit(vmx, SECONDARY_EXEC_DESC);
		}
	}

	vcpu->arch.cr4 = cr4;
	kvm_register_mark_available(vcpu, VCPU_EXREG_CR4);

	if (!is_unrestricted_guest(vcpu)) {
		if (enable_ept) {
			if (!is_paging(vcpu)) {
				hw_cr4 &= ~X86_CR4_PAE;
				hw_cr4 |= X86_CR4_PSE;
			} else if (!(cr4 & X86_CR4_PAE)) {
				hw_cr4 &= ~X86_CR4_PAE;
			}
		}

		/*
		 * SMEP/SMAP/PKU is disabled if CPU is in non-paging mode in
		 * hardware.  To emulate this behavior, SMEP/SMAP/PKU needs
		 * to be manually disabled when guest switches to non-paging
		 * mode.
		 *
		 * If !enable_unrestricted_guest, the CPU is always running
		 * with CR0.PG=1 and CR4 needs to be modified.
		 * If enable_unrestricted_guest, the CPU automatically
		 * disables SMEP/SMAP/PKU when the guest sets CR0.PG=0.
		 */
		if (!is_paging(vcpu))
			hw_cr4 &= ~(X86_CR4_SMEP | X86_CR4_SMAP | X86_CR4_PKE);
	}

	vmcs_writel(CR4_READ_SHADOW, cr4);
	vmcs_writel(GUEST_CR4, hw_cr4);

	if ((cr4 ^ old_cr4) & (X86_CR4_OSXSAVE | X86_CR4_PKE))
		kvm_update_cpuid_runtime(vcpu);
}

void vmx_get_segment(struct kvm_vcpu *vcpu, struct kvm_segment *var, int seg)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	u32 ar;

	if (vmx->rmode.vm86_active && seg != VCPU_SREG_LDTR) {
		*var = vmx->rmode.segs[seg];
		if (seg == VCPU_SREG_TR
		    || var->selector == vmx_read_guest_seg_selector(vmx, seg))
			return;
		var->base = vmx_read_guest_seg_base(vmx, seg);
		var->selector = vmx_read_guest_seg_selector(vmx, seg);
		return;
	}
	var->base = vmx_read_guest_seg_base(vmx, seg);
	var->limit = vmx_read_guest_seg_limit(vmx, seg);
	var->selector = vmx_read_guest_seg_selector(vmx, seg);
	ar = vmx_read_guest_seg_ar(vmx, seg);
	var->unusable = (ar >> 16) & 1;
	var->type = ar & 15;
	var->s = (ar >> 4) & 1;
	var->dpl = (ar >> 5) & 3;
	/*
	 * Some userspaces do not preserve unusable property. Since usable
	 * segment has to be present according to VMX spec we can use present
	 * property to amend userspace bug by making unusable segment always
	 * nonpresent. vmx_segment_access_rights() already marks nonpresent
	 * segment as unusable.
	 */
	var->present = !var->unusable;
	var->avl = (ar >> 12) & 1;
	var->l = (ar >> 13) & 1;
	var->db = (ar >> 14) & 1;
	var->g = (ar >> 15) & 1;
}

static u64 vmx_get_segment_base(struct kvm_vcpu *vcpu, int seg)
{
	struct kvm_segment s;

	if (to_vmx(vcpu)->rmode.vm86_active) {
		vmx_get_segment(vcpu, &s, seg);
		return s.base;
	}
	return vmx_read_guest_seg_base(to_vmx(vcpu), seg);
}

int vmx_get_cpl(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);

	if (unlikely(vmx->rmode.vm86_active))
		return 0;
	else {
		int ar = vmx_read_guest_seg_ar(vmx, VCPU_SREG_SS);
		return VMX_AR_DPL(ar);
	}
}

static u32 vmx_segment_access_rights(struct kvm_segment *var)
{
	u32 ar;

	if (var->unusable || !var->present)
		ar = 1 << 16;
	else {
		ar = var->type & 15;
		ar |= (var->s & 1) << 4;
		ar |= (var->dpl & 3) << 5;
		ar |= (var->present & 1) << 7;
		ar |= (var->avl & 1) << 12;
		ar |= (var->l & 1) << 13;
		ar |= (var->db & 1) << 14;
		ar |= (var->g & 1) << 15;
	}

	return ar;
}

void vmx_set_segment(struct kvm_vcpu *vcpu, struct kvm_segment *var, int seg)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	const struct kvm_vmx_segment_field *sf = &kvm_vmx_segment_fields[seg];

	vmx_segment_cache_clear(vmx);

	if (vmx->rmode.vm86_active && seg != VCPU_SREG_LDTR) {
		vmx->rmode.segs[seg] = *var;
		if (seg == VCPU_SREG_TR)
			vmcs_write16(sf->selector, var->selector);
		else if (var->s)
			fix_rmode_seg(seg, &vmx->rmode.segs[seg]);
		goto out;
	}

	vmcs_writel(sf->base, var->base);
	vmcs_write32(sf->limit, var->limit);
	vmcs_write16(sf->selector, var->selector);

	/*
	 *   Fix the "Accessed" bit in AR field of segment registers for older
	 * qemu binaries.
	 *   IA32 arch specifies that at the time of processor reset the
	 * "Accessed" bit in the AR field of segment registers is 1. And qemu
	 * is setting it to 0 in the userland code. This causes invalid guest
	 * state vmexit when "unrestricted guest" mode is turned on.
	 *    Fix for this setup issue in cpu_reset is being pushed in the qemu
	 * tree. Newer qemu binaries with that qemu fix would not need this
	 * kvm hack.
	 */
	if (is_unrestricted_guest(vcpu) && (seg != VCPU_SREG_LDTR))
		var->type |= 0x1; /* Accessed */

	vmcs_write32(sf->ar_bytes, vmx_segment_access_rights(var));

out:
	vmx->emulation_required = emulation_required(vcpu);
}

static void vmx_get_cs_db_l_bits(struct kvm_vcpu *vcpu, int *db, int *l)
{
	u32 ar = vmx_read_guest_seg_ar(to_vmx(vcpu), VCPU_SREG_CS);

	*db = (ar >> 14) & 1;
	*l = (ar >> 13) & 1;
}

static void vmx_get_idt(struct kvm_vcpu *vcpu, struct desc_ptr *dt)
{
	dt->size = vmcs_read32(GUEST_IDTR_LIMIT);
	dt->address = vmcs_readl(GUEST_IDTR_BASE);
}

static void vmx_set_idt(struct kvm_vcpu *vcpu, struct desc_ptr *dt)
{
	vmcs_write32(GUEST_IDTR_LIMIT, dt->size);
	vmcs_writel(GUEST_IDTR_BASE, dt->address);
}

static void vmx_get_gdt(struct kvm_vcpu *vcpu, struct desc_ptr *dt)
{
	dt->size = vmcs_read32(GUEST_GDTR_LIMIT);
	dt->address = vmcs_readl(GUEST_GDTR_BASE);
}

static void vmx_set_gdt(struct kvm_vcpu *vcpu, struct desc_ptr *dt)
{
	vmcs_write32(GUEST_GDTR_LIMIT, dt->size);
	vmcs_writel(GUEST_GDTR_BASE, dt->address);
}

static bool rmode_segment_valid(struct kvm_vcpu *vcpu, int seg)
{
	struct kvm_segment var;
	u32 ar;

	vmx_get_segment(vcpu, &var, seg);
	var.dpl = 0x3;
	if (seg == VCPU_SREG_CS)
		var.type = 0x3;
	ar = vmx_segment_access_rights(&var);

	if (var.base != (var.selector << 4))
		return false;
	if (var.limit != 0xffff)
		return false;
	if (ar != 0xf3)
		return false;

	return true;
}

static bool code_segment_valid(struct kvm_vcpu *vcpu)
{
	struct kvm_segment cs;
	unsigned int cs_rpl;

	vmx_get_segment(vcpu, &cs, VCPU_SREG_CS);
	cs_rpl = cs.selector & SEGMENT_RPL_MASK;

	if (cs.unusable)
		return false;
	if (~cs.type & (VMX_AR_TYPE_CODE_MASK|VMX_AR_TYPE_ACCESSES_MASK))
		return false;
	if (!cs.s)
		return false;
	if (cs.type & VMX_AR_TYPE_WRITEABLE_MASK) {
		if (cs.dpl > cs_rpl)
			return false;
	} else {
		if (cs.dpl != cs_rpl)
			return false;
	}
	if (!cs.present)
		return false;

	/* TODO: Add Reserved field check, this'll require a new member in the kvm_segment_field structure */
	return true;
}

static bool stack_segment_valid(struct kvm_vcpu *vcpu)
{
	struct kvm_segment ss;
	unsigned int ss_rpl;

	vmx_get_segment(vcpu, &ss, VCPU_SREG_SS);
	ss_rpl = ss.selector & SEGMENT_RPL_MASK;

	if (ss.unusable)
		return true;
	if (ss.type != 3 && ss.type != 7)
		return false;
	if (!ss.s)
		return false;
	if (ss.dpl != ss_rpl) /* DPL != RPL */
		return false;
	if (!ss.present)
		return false;

	return true;
}

static bool data_segment_valid(struct kvm_vcpu *vcpu, int seg)
{
	struct kvm_segment var;
	unsigned int rpl;

	vmx_get_segment(vcpu, &var, seg);
	rpl = var.selector & SEGMENT_RPL_MASK;

	if (var.unusable)
		return true;
	if (!var.s)
		return false;
	if (!var.present)
		return false;
	if (~var.type & (VMX_AR_TYPE_CODE_MASK|VMX_AR_TYPE_WRITEABLE_MASK)) {
		if (var.dpl < rpl) /* DPL < RPL */
			return false;
	}

	/* TODO: Add other members to kvm_segment_field to allow checking for other access
	 * rights flags
	 */
	return true;
}

static bool tr_valid(struct kvm_vcpu *vcpu)
{
	struct kvm_segment tr;

	vmx_get_segment(vcpu, &tr, VCPU_SREG_TR);

	if (tr.unusable)
		return false;
	if (tr.selector & SEGMENT_TI_MASK)	/* TI = 1 */
		return false;
	if (tr.type != 3 && tr.type != 11) /* TODO: Check if guest is in IA32e mode */
		return false;
	if (!tr.present)
		return false;

	return true;
}

static bool ldtr_valid(struct kvm_vcpu *vcpu)
{
	struct kvm_segment ldtr;

	vmx_get_segment(vcpu, &ldtr, VCPU_SREG_LDTR);

	if (ldtr.unusable)
		return true;
	if (ldtr.selector & SEGMENT_TI_MASK)	/* TI = 1 */
		return false;
	if (ldtr.type != 2)
		return false;
	if (!ldtr.present)
		return false;

	return true;
}

static bool cs_ss_rpl_check(struct kvm_vcpu *vcpu)
{
	struct kvm_segment cs, ss;

	vmx_get_segment(vcpu, &cs, VCPU_SREG_CS);
	vmx_get_segment(vcpu, &ss, VCPU_SREG_SS);

	return ((cs.selector & SEGMENT_RPL_MASK) ==
		 (ss.selector & SEGMENT_RPL_MASK));
}

/*
 * Check if guest state is valid. Returns true if valid, false if
 * not.
 * We assume that registers are always usable
 */
bool __vmx_guest_state_valid(struct kvm_vcpu *vcpu)
{
	/* real mode guest state checks */
	if (!is_protmode(vcpu) || (vmx_get_rflags(vcpu) & X86_EFLAGS_VM)) {
		if (!rmode_segment_valid(vcpu, VCPU_SREG_CS))
			return false;
		if (!rmode_segment_valid(vcpu, VCPU_SREG_SS))
			return false;
		if (!rmode_segment_valid(vcpu, VCPU_SREG_DS))
			return false;
		if (!rmode_segment_valid(vcpu, VCPU_SREG_ES))
			return false;
		if (!rmode_segment_valid(vcpu, VCPU_SREG_FS))
			return false;
		if (!rmode_segment_valid(vcpu, VCPU_SREG_GS))
			return false;
	} else {
	/* protected mode guest state checks */
		if (!cs_ss_rpl_check(vcpu))
			return false;
		if (!code_segment_valid(vcpu))
			return false;
		if (!stack_segment_valid(vcpu))
			return false;
		if (!data_segment_valid(vcpu, VCPU_SREG_DS))
			return false;
		if (!data_segment_valid(vcpu, VCPU_SREG_ES))
			return false;
		if (!data_segment_valid(vcpu, VCPU_SREG_FS))
			return false;
		if (!data_segment_valid(vcpu, VCPU_SREG_GS))
			return false;
		if (!tr_valid(vcpu))
			return false;
		if (!ldtr_valid(vcpu))
			return false;
	}
	/* TODO:
	 * - Add checks on RIP
	 * - Add checks on RFLAGS
	 */

	return true;
}

static int init_rmode_tss(struct kvm *kvm, void __user *ua)
{
	const void *zero_page = (const void *) __va(page_to_phys(ZERO_PAGE(0)));
	u16 data;
	int i;

	for (i = 0; i < 3; i++) {
		if (__copy_to_user(ua + PAGE_SIZE * i, zero_page, PAGE_SIZE))
			return -EFAULT;
	}

	data = TSS_BASE_SIZE + TSS_REDIRECTION_SIZE;
	if (__copy_to_user(ua + TSS_IOPB_BASE_OFFSET, &data, sizeof(u16)))
		return -EFAULT;

	data = ~0;
	if (__copy_to_user(ua + RMODE_TSS_SIZE - 1, &data, sizeof(u8)))
		return -EFAULT;

	return 0;
}

static int init_rmode_identity_map(struct kvm *kvm)
{
	struct kvm_vmx *kvm_vmx = to_kvm_vmx(kvm);
	int i, r = 0;
	void __user *uaddr;
	u32 tmp;

	/* Protect kvm_vmx->ept_identity_pagetable_done. */
	mutex_lock(&kvm->slots_lock);

	if (likely(kvm_vmx->ept_identity_pagetable_done))
		goto out;

	if (!kvm_vmx->ept_identity_map_addr)
		kvm_vmx->ept_identity_map_addr = VMX_EPT_IDENTITY_PAGETABLE_ADDR;

	uaddr = __x86_set_memory_region(kvm,
					IDENTITY_PAGETABLE_PRIVATE_MEMSLOT,
					kvm_vmx->ept_identity_map_addr,
					PAGE_SIZE);
	if (IS_ERR(uaddr)) {
		r = PTR_ERR(uaddr);
		goto out;
	}

	/* Set up identity-mapping pagetable for EPT in real mode */
	for (i = 0; i < PT32_ENT_PER_PAGE; i++) {
		tmp = (i << 22) + (_PAGE_PRESENT | _PAGE_RW | _PAGE_USER |
			_PAGE_ACCESSED | _PAGE_DIRTY | _PAGE_PSE);
		if (__copy_to_user(uaddr + i * sizeof(tmp), &tmp, sizeof(tmp))) {
			r = -EFAULT;
			goto out;
		}
	}
	kvm_vmx->ept_identity_pagetable_done = true;

out:
	mutex_unlock(&kvm->slots_lock);
	return r;
}

static void seg_setup(int seg)
{
	const struct kvm_vmx_segment_field *sf = &kvm_vmx_segment_fields[seg];
	unsigned int ar;

	vmcs_write16(sf->selector, 0);
	vmcs_writel(sf->base, 0);
	vmcs_write32(sf->limit, 0xffff);
	ar = 0x93;
	if (seg == VCPU_SREG_CS)
		ar |= 0x08; /* code segment */

	vmcs_write32(sf->ar_bytes, ar);
}

static int alloc_apic_access_page(struct kvm *kvm)
{
	struct page *page;
	void __user *hva;
	int ret = 0;

	mutex_lock(&kvm->slots_lock);
	if (kvm->arch.apic_access_page_done)
		goto out;
	hva = __x86_set_memory_region(kvm, APIC_ACCESS_PAGE_PRIVATE_MEMSLOT,
				      APIC_DEFAULT_PHYS_BASE, PAGE_SIZE);
	if (IS_ERR(hva)) {
		ret = PTR_ERR(hva);
		goto out;
	}

	page = gfn_to_page(kvm, APIC_DEFAULT_PHYS_BASE >> PAGE_SHIFT);
	if (is_error_page(page)) {
		ret = -EFAULT;
		goto out;
	}

	/*
	 * Do not pin the page in memory, so that memory hot-unplug
	 * is able to migrate it.
	 */
	put_page(page);
	kvm->arch.apic_access_page_done = true;
out:
	mutex_unlock(&kvm->slots_lock);
	return ret;
}

int allocate_vpid(void)
{
	int vpid;

	if (!enable_vpid)
		return 0;
	spin_lock(&vmx_vpid_lock);
	vpid = find_first_zero_bit(vmx_vpid_bitmap, VMX_NR_VPIDS);
	if (vpid < VMX_NR_VPIDS)
		__set_bit(vpid, vmx_vpid_bitmap);
	else
		vpid = 0;
	spin_unlock(&vmx_vpid_lock);
	return vpid;
}

void free_vpid(int vpid)
{
	if (!enable_vpid || vpid == 0)
		return;
	spin_lock(&vmx_vpid_lock);
	__clear_bit(vpid, vmx_vpid_bitmap);
	spin_unlock(&vmx_vpid_lock);
}

static void vmx_clear_msr_bitmap_read(ulong *msr_bitmap, u32 msr)
{
	int f = sizeof(unsigned long);

	if (msr <= 0x1fff)
		__clear_bit(msr, msr_bitmap + 0x000 / f);
	else if ((msr >= 0xc0000000) && (msr <= 0xc0001fff))
		__clear_bit(msr & 0x1fff, msr_bitmap + 0x400 / f);
}

static void vmx_clear_msr_bitmap_write(ulong *msr_bitmap, u32 msr)
{
	int f = sizeof(unsigned long);

	if (msr <= 0x1fff)
		__clear_bit(msr, msr_bitmap + 0x800 / f);
	else if ((msr >= 0xc0000000) && (msr <= 0xc0001fff))
		__clear_bit(msr & 0x1fff, msr_bitmap + 0xc00 / f);
}

static void vmx_set_msr_bitmap_read(ulong *msr_bitmap, u32 msr)
{
	int f = sizeof(unsigned long);

	if (msr <= 0x1fff)
		__set_bit(msr, msr_bitmap + 0x000 / f);
	else if ((msr >= 0xc0000000) && (msr <= 0xc0001fff))
		__set_bit(msr & 0x1fff, msr_bitmap + 0x400 / f);
}

static void vmx_set_msr_bitmap_write(ulong *msr_bitmap, u32 msr)
{
	int f = sizeof(unsigned long);

	if (msr <= 0x1fff)
		__set_bit(msr, msr_bitmap + 0x800 / f);
	else if ((msr >= 0xc0000000) && (msr <= 0xc0001fff))
		__set_bit(msr & 0x1fff, msr_bitmap + 0xc00 / f);
}

void vmx_disable_intercept_for_msr(struct kvm_vcpu *vcpu, u32 msr, int type)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	unsigned long *msr_bitmap = vmx->vmcs01.msr_bitmap;

	if (!cpu_has_vmx_msr_bitmap())
		return;

	if (static_branch_unlikely(&enable_evmcs))
		evmcs_touch_msr_bitmap();

	/*
	 * Mark the desired intercept state in shadow bitmap, this is needed
	 * for resync when the MSR filters change.
	*/
	if (is_valid_passthrough_msr(msr)) {
		int idx = possible_passthrough_msr_slot(msr);

		if (idx != -ENOENT) {
			if (type & MSR_TYPE_R)
				clear_bit(idx, vmx->shadow_msr_intercept.read);
			if (type & MSR_TYPE_W)
				clear_bit(idx, vmx->shadow_msr_intercept.write);
		}
	}

	if ((type & MSR_TYPE_R) &&
	    !kvm_msr_allowed(vcpu, msr, KVM_MSR_FILTER_READ)) {
		vmx_set_msr_bitmap_read(msr_bitmap, msr);
		type &= ~MSR_TYPE_R;
	}

	if ((type & MSR_TYPE_W) &&
	    !kvm_msr_allowed(vcpu, msr, KVM_MSR_FILTER_WRITE)) {
		vmx_set_msr_bitmap_write(msr_bitmap, msr);
		type &= ~MSR_TYPE_W;
	}

	if (type & MSR_TYPE_R)
		vmx_clear_msr_bitmap_read(msr_bitmap, msr);

	if (type & MSR_TYPE_W)
		vmx_clear_msr_bitmap_write(msr_bitmap, msr);
}

void vmx_enable_intercept_for_msr(struct kvm_vcpu *vcpu, u32 msr, int type)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	unsigned long *msr_bitmap = vmx->vmcs01.msr_bitmap;

	if (!cpu_has_vmx_msr_bitmap())
		return;

	if (static_branch_unlikely(&enable_evmcs))
		evmcs_touch_msr_bitmap();

	/*
	 * Mark the desired intercept state in shadow bitmap, this is needed
	 * for resync when the MSR filter changes.
	*/
	if (is_valid_passthrough_msr(msr)) {
		int idx = possible_passthrough_msr_slot(msr);

		if (idx != -ENOENT) {
			if (type & MSR_TYPE_R)
				set_bit(idx, vmx->shadow_msr_intercept.read);
			if (type & MSR_TYPE_W)
				set_bit(idx, vmx->shadow_msr_intercept.write);
		}
	}

	if (type & MSR_TYPE_R)
		vmx_set_msr_bitmap_read(msr_bitmap, msr);

	if (type & MSR_TYPE_W)
		vmx_set_msr_bitmap_write(msr_bitmap, msr);
}

static u8 vmx_msr_bitmap_mode(struct kvm_vcpu *vcpu)
{
	u8 mode = 0;

	if (cpu_has_secondary_exec_ctrls() &&
	    (secondary_exec_controls_get(to_vmx(vcpu)) &
	     SECONDARY_EXEC_VIRTUALIZE_X2APIC_MODE)) {
		mode |= MSR_BITMAP_MODE_X2APIC;
		if (enable_apicv && kvm_vcpu_apicv_active(vcpu))
			mode |= MSR_BITMAP_MODE_X2APIC_APICV;
	}

	return mode;
}

static void vmx_reset_x2apic_msrs(struct kvm_vcpu *vcpu, u8 mode)
{
	unsigned long *msr_bitmap = to_vmx(vcpu)->vmcs01.msr_bitmap;
	unsigned long read_intercept;
	int msr;

	read_intercept = (mode & MSR_BITMAP_MODE_X2APIC_APICV) ? 0 : ~0;

	for (msr = 0x800; msr <= 0x8ff; msr += BITS_PER_LONG) {
		unsigned int read_idx = msr / BITS_PER_LONG;
		unsigned int write_idx = read_idx + (0x800 / sizeof(long));

		msr_bitmap[read_idx] = read_intercept;
		msr_bitmap[write_idx] = ~0ul;
	}
}

static void vmx_update_msr_bitmap_x2apic(struct kvm_vcpu *vcpu, u8 mode)
{
	if (!cpu_has_vmx_msr_bitmap())
		return;

	vmx_reset_x2apic_msrs(vcpu, mode);

	/*
	 * TPR reads and writes can be virtualized even if virtual interrupt
	 * delivery is not in use.
	 */
	vmx_set_intercept_for_msr(vcpu, X2APIC_MSR(APIC_TASKPRI), MSR_TYPE_RW,
				  !(mode & MSR_BITMAP_MODE_X2APIC));

	if (mode & MSR_BITMAP_MODE_X2APIC_APICV) {
		vmx_enable_intercept_for_msr(vcpu, X2APIC_MSR(APIC_TMCCT), MSR_TYPE_RW);
		vmx_disable_intercept_for_msr(vcpu, X2APIC_MSR(APIC_EOI), MSR_TYPE_W);
		vmx_disable_intercept_for_msr(vcpu, X2APIC_MSR(APIC_SELF_IPI), MSR_TYPE_W);
	}
}

void vmx_update_msr_bitmap(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	u8 mode = vmx_msr_bitmap_mode(vcpu);
	u8 changed = mode ^ vmx->msr_bitmap_mode;

	if (!changed)
		return;

	if (changed & (MSR_BITMAP_MODE_X2APIC | MSR_BITMAP_MODE_X2APIC_APICV))
		vmx_update_msr_bitmap_x2apic(vcpu, mode);

	vmx->msr_bitmap_mode = mode;
}

void pt_update_intercept_for_msr(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	bool flag = !(vmx->pt_desc.guest.ctl & RTIT_CTL_TRACEEN);
	u32 i;

	vmx_set_intercept_for_msr(vcpu, MSR_IA32_RTIT_STATUS, MSR_TYPE_RW, flag);
	vmx_set_intercept_for_msr(vcpu, MSR_IA32_RTIT_OUTPUT_BASE, MSR_TYPE_RW, flag);
	vmx_set_intercept_for_msr(vcpu, MSR_IA32_RTIT_OUTPUT_MASK, MSR_TYPE_RW, flag);
	vmx_set_intercept_for_msr(vcpu, MSR_IA32_RTIT_CR3_MATCH, MSR_TYPE_RW, flag);
	for (i = 0; i < vmx->pt_desc.addr_range; i++) {
		vmx_set_intercept_for_msr(vcpu, MSR_IA32_RTIT_ADDR0_A + i * 2, MSR_TYPE_RW, flag);
		vmx_set_intercept_for_msr(vcpu, MSR_IA32_RTIT_ADDR0_B + i * 2, MSR_TYPE_RW, flag);
	}
}

static bool vmx_guest_apic_has_interrupt(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	void *vapic_page;
	u32 vppr;
	int rvi;

	if (WARN_ON_ONCE(!is_guest_mode(vcpu)) ||
		!nested_cpu_has_vid(get_vmcs12(vcpu)) ||
		WARN_ON_ONCE(!vmx->nested.virtual_apic_map.gfn))
		return false;

	rvi = vmx_get_rvi();

	vapic_page = vmx->nested.virtual_apic_map.hva;
	vppr = *((u32 *)(vapic_page + APIC_PROCPRI));

	return ((rvi & 0xf0) > (vppr & 0xf0));
}

static void vmx_msr_filter_changed(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	u32 i;

	/*
	 * Set intercept permissions for all potentially passed through MSRs
	 * again. They will automatically get filtered through the MSR filter,
	 * so we are back in sync after this.
	 */
	for (i = 0; i < ARRAY_SIZE(vmx_possible_passthrough_msrs); i++) {
		u32 msr = vmx_possible_passthrough_msrs[i];
		bool read = test_bit(i, vmx->shadow_msr_intercept.read);
		bool write = test_bit(i, vmx->shadow_msr_intercept.write);

		vmx_set_intercept_for_msr(vcpu, msr, MSR_TYPE_R, read);
		vmx_set_intercept_for_msr(vcpu, msr, MSR_TYPE_W, write);
	}

	pt_update_intercept_for_msr(vcpu);
	vmx_update_msr_bitmap_x2apic(vcpu, vmx_msr_bitmap_mode(vcpu));
}

static inline bool kvm_vcpu_trigger_posted_interrupt(struct kvm_vcpu *vcpu,
						     bool nested)
{
#ifdef CONFIG_SMP
	int pi_vec = nested ? POSTED_INTR_NESTED_VECTOR : POSTED_INTR_VECTOR;

	if (vcpu->mode == IN_GUEST_MODE) {
		/*
		 * The vector of interrupt to be delivered to vcpu had
		 * been set in PIR before this function.
		 *
		 * Following cases will be reached in this block, and
		 * we always send a notification event in all cases as
		 * explained below.
		 *
		 * Case 1: vcpu keeps in non-root mode. Sending a
		 * notification event posts the interrupt to vcpu.
		 *
		 * Case 2: vcpu exits to root mode and is still
		 * runnable. PIR will be synced to vIRR before the
		 * next vcpu entry. Sending a notification event in
		 * this case has no effect, as vcpu is not in root
		 * mode.
		 *
		 * Case 3: vcpu exits to root mode and is blocked.
		 * vcpu_block() has already synced PIR to vIRR and
		 * never blocks vcpu if vIRR is not cleared. Therefore,
		 * a blocked vcpu here does not wait for any requested
		 * interrupts in PIR, and sending a notification event
		 * which has no effect is safe here.
		 */

		apic->send_IPI_mask(get_cpu_mask(vcpu->cpu), pi_vec);
		return true;
	}
#endif
	return false;
}

static int vmx_deliver_nested_posted_interrupt(struct kvm_vcpu *vcpu,
						int vector)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);

	if (is_guest_mode(vcpu) &&
	    vector == vmx->nested.posted_intr_nv) {
		/*
		 * If a posted intr is not recognized by hardware,
		 * we will accomplish it in the next vmentry.
		 */
		vmx->nested.pi_pending = true;
		kvm_make_request(KVM_REQ_EVENT, vcpu);
		/* the PIR and ON have been set by L1. */
		if (!kvm_vcpu_trigger_posted_interrupt(vcpu, true))
			kvm_vcpu_kick(vcpu);
		return 0;
	}
	return -1;
}
/*
 * Send interrupt to vcpu via posted interrupt way.
 * 1. If target vcpu is running(non-root mode), send posted interrupt
 * notification to vcpu and hardware will sync PIR to vIRR atomically.
 * 2. If target vcpu isn't running(root mode), kick it to pick up the
 * interrupt from PIR in next vmentry.
 */
static int vmx_deliver_posted_interrupt(struct kvm_vcpu *vcpu, int vector)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	int r;

	r = vmx_deliver_nested_posted_interrupt(vcpu, vector);
	if (!r)
		return 0;

	if (!vcpu->arch.apicv_active)
		return -1;

	if (pi_test_and_set_pir(vector, &vmx->pi_desc))
		return 0;

	/* If a previous notification has sent the IPI, nothing to do.  */
	if (pi_test_and_set_on(&vmx->pi_desc))
		return 0;

	if (vcpu != kvm_get_running_vcpu() &&
	    !kvm_vcpu_trigger_posted_interrupt(vcpu, false))
		kvm_vcpu_kick(vcpu);

	return 0;
}

/*
 * Set up the vmcs's constant host-state fields, i.e., host-state fields that
 * will not change in the lifetime of the guest.
 * Note that host-state that does change is set elsewhere. E.g., host-state
 * that is set differently for each CPU is set in vmx_vcpu_load(), not here.
 */
void vmx_set_constant_host_state(struct vcpu_vmx *vmx)
{
	u32 low32, high32;
	unsigned long tmpl;
	unsigned long cr0, cr3, cr4;

	cr0 = read_cr0();
	WARN_ON(cr0 & X86_CR0_TS);
	vmcs_writel(HOST_CR0, cr0);  /* 22.2.3 */

	/*
	 * Save the most likely value for this task's CR3 in the VMCS.
	 * We can't use __get_current_cr3_fast() because we're not atomic.
	 */
	cr3 = __read_cr3();
	vmcs_writel(HOST_CR3, cr3);		/* 22.2.3  FIXME: shadow tables */
	vmx->loaded_vmcs->host_state.cr3 = cr3;

	/* Save the most likely value for this task's CR4 in the VMCS. */
	cr4 = cr4_read_shadow();
	vmcs_writel(HOST_CR4, cr4);			/* 22.2.3, 22.2.5 */
	vmx->loaded_vmcs->host_state.cr4 = cr4;

	vmcs_write16(HOST_CS_SELECTOR, __KERNEL_CS);  /* 22.2.4 */
#ifdef CONFIG_X86_64
	/*
	 * Load null selectors, so we can avoid reloading them in
	 * vmx_prepare_switch_to_host(), in case userspace uses
	 * the null selectors too (the expected case).
	 */
	vmcs_write16(HOST_DS_SELECTOR, 0);
	vmcs_write16(HOST_ES_SELECTOR, 0);
#else
	vmcs_write16(HOST_DS_SELECTOR, __KERNEL_DS);  /* 22.2.4 */
	vmcs_write16(HOST_ES_SELECTOR, __KERNEL_DS);  /* 22.2.4 */
#endif
	vmcs_write16(HOST_SS_SELECTOR, __KERNEL_DS);  /* 22.2.4 */
	vmcs_write16(HOST_TR_SELECTOR, GDT_ENTRY_TSS*8);  /* 22.2.4 */

	vmcs_writel(HOST_IDTR_BASE, host_idt_base);   /* 22.2.4 */

	vmcs_writel(HOST_RIP, (unsigned long)vmx_vmexit); /* 22.2.5 */

	rdmsr(MSR_IA32_SYSENTER_CS, low32, high32);
	vmcs_write32(HOST_IA32_SYSENTER_CS, low32);
	rdmsrl(MSR_IA32_SYSENTER_EIP, tmpl);
	vmcs_writel(HOST_IA32_SYSENTER_EIP, tmpl);   /* 22.2.3 */

	if (vmcs_config.vmexit_ctrl & VM_EXIT_LOAD_IA32_PAT) {
		rdmsr(MSR_IA32_CR_PAT, low32, high32);
		vmcs_write64(HOST_IA32_PAT, low32 | ((u64) high32 << 32));
	}

	if (cpu_has_load_ia32_efer())
		vmcs_write64(HOST_IA32_EFER, host_efer);
}

void set_cr4_guest_host_mask(struct vcpu_vmx *vmx)
{
	struct kvm_vcpu *vcpu = &vmx->vcpu;

	vcpu->arch.cr4_guest_owned_bits = KVM_POSSIBLE_CR4_GUEST_BITS &
					  ~vcpu->arch.cr4_guest_rsvd_bits;
	if (!enable_ept)
		vcpu->arch.cr4_guest_owned_bits &= ~X86_CR4_PGE;
	if (is_guest_mode(&vmx->vcpu))
		vcpu->arch.cr4_guest_owned_bits &=
			~get_vmcs12(vcpu)->cr4_guest_host_mask;
	vmcs_writel(CR4_GUEST_HOST_MASK, ~vcpu->arch.cr4_guest_owned_bits);
}

u32 vmx_pin_based_exec_ctrl(struct vcpu_vmx *vmx)
{
	u32 pin_based_exec_ctrl = vmcs_config.pin_based_exec_ctrl;

	if (!kvm_vcpu_apicv_active(&vmx->vcpu))
		pin_based_exec_ctrl &= ~PIN_BASED_POSTED_INTR;

	if (!enable_vnmi)
		pin_based_exec_ctrl &= ~PIN_BASED_VIRTUAL_NMIS;

	if (!enable_preemption_timer)
		pin_based_exec_ctrl &= ~PIN_BASED_VMX_PREEMPTION_TIMER;

	return pin_based_exec_ctrl;
}

static void vmx_refresh_apicv_exec_ctrl(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);

	pin_controls_set(vmx, vmx_pin_based_exec_ctrl(vmx));
	if (cpu_has_secondary_exec_ctrls()) {
		if (kvm_vcpu_apicv_active(vcpu))
			secondary_exec_controls_setbit(vmx,
				      SECONDARY_EXEC_APIC_REGISTER_VIRT |
				      SECONDARY_EXEC_VIRTUAL_INTR_DELIVERY);
		else
			secondary_exec_controls_clearbit(vmx,
					SECONDARY_EXEC_APIC_REGISTER_VIRT |
					SECONDARY_EXEC_VIRTUAL_INTR_DELIVERY);
	}

	if (cpu_has_vmx_msr_bitmap())
		vmx_update_msr_bitmap(vcpu);
}

u32 vmx_exec_control(struct vcpu_vmx *vmx)
{
	u32 exec_control = vmcs_config.cpu_based_exec_ctrl;

	if (vmx->vcpu.arch.switch_db_regs & KVM_DEBUGREG_WONT_EXIT)
		exec_control &= ~CPU_BASED_MOV_DR_EXITING;

	if (!cpu_need_tpr_shadow(&vmx->vcpu)) {
		exec_control &= ~CPU_BASED_TPR_SHADOW;
#ifdef CONFIG_X86_64
		exec_control |= CPU_BASED_CR8_STORE_EXITING |
				CPU_BASED_CR8_LOAD_EXITING;
#endif
	}
	if (!enable_ept)
		exec_control |= CPU_BASED_CR3_STORE_EXITING |
				CPU_BASED_CR3_LOAD_EXITING  |
				CPU_BASED_INVLPG_EXITING;
	if (kvm_mwait_in_guest(vmx->vcpu.kvm))
		exec_control &= ~(CPU_BASED_MWAIT_EXITING |
				CPU_BASED_MONITOR_EXITING);
	if (kvm_hlt_in_guest(vmx->vcpu.kvm))
		exec_control &= ~CPU_BASED_HLT_EXITING;
	return exec_control;
}

/*
 * Adjust a single secondary execution control bit to intercept/allow an
 * instruction in the guest.  This is usually done based on whether or not a
 * feature has been exposed to the guest in order to correctly emulate faults.
 */
static inline void
vmx_adjust_secondary_exec_control(struct vcpu_vmx *vmx, u32 *exec_control,
				  u32 control, bool enabled, bool exiting)
{
	/*
	 * If the control is for an opt-in feature, clear the control if the
	 * feature is not exposed to the guest, i.e. not enabled.  If the
	 * control is opt-out, i.e. an exiting control, clear the control if
	 * the feature _is_ exposed to the guest, i.e. exiting/interception is
	 * disabled for the associated instruction.  Note, the caller is
	 * responsible presetting exec_control to set all supported bits.
	 */
	if (enabled == exiting)
		*exec_control &= ~control;

	/*
	 * Update the nested MSR settings so that a nested VMM can/can't set
	 * controls for features that are/aren't exposed to the guest.
	 */
	if (nested) {
		if (enabled)
			vmx->nested.msrs.secondary_ctls_high |= control;
		else
			vmx->nested.msrs.secondary_ctls_high &= ~control;
	}
}

/*
 * Wrapper macro for the common case of adjusting a secondary execution control
 * based on a single guest CPUID bit, with a dedicated feature bit.  This also
 * verifies that the control is actually supported by KVM and hardware.
 */
#define vmx_adjust_sec_exec_control(vmx, exec_control, name, feat_name, ctrl_name, exiting) \
({									 \
	bool __enabled;							 \
									 \
	if (cpu_has_vmx_##name()) {					 \
		__enabled = guest_cpuid_has(&(vmx)->vcpu,		 \
					    X86_FEATURE_##feat_name);	 \
		vmx_adjust_secondary_exec_control(vmx, exec_control,	 \
			SECONDARY_EXEC_##ctrl_name, __enabled, exiting); \
	}								 \
})

/* More macro magic for ENABLE_/opt-in versus _EXITING/opt-out controls. */
#define vmx_adjust_sec_exec_feature(vmx, exec_control, lname, uname) \
	vmx_adjust_sec_exec_control(vmx, exec_control, lname, uname, ENABLE_##uname, false)

#define vmx_adjust_sec_exec_exiting(vmx, exec_control, lname, uname) \
	vmx_adjust_sec_exec_control(vmx, exec_control, lname, uname, uname##_EXITING, true)

static void vmx_compute_secondary_exec_control(struct vcpu_vmx *vmx)
{
	struct kvm_vcpu *vcpu = &vmx->vcpu;

	u32 exec_control = vmcs_config.cpu_based_2nd_exec_ctrl;

	if (vmx_pt_mode_is_system())
		exec_control &= ~(SECONDARY_EXEC_PT_USE_GPA | SECONDARY_EXEC_PT_CONCEAL_VMX);
	if (!cpu_need_virtualize_apic_accesses(vcpu))
		exec_control &= ~SECONDARY_EXEC_VIRTUALIZE_APIC_ACCESSES;
	if (vmx->vpid == 0)
		exec_control &= ~SECONDARY_EXEC_ENABLE_VPID;
	if (!enable_ept) {
		exec_control &= ~SECONDARY_EXEC_ENABLE_EPT;
		enable_unrestricted_guest = 0;
	}
	if (!enable_unrestricted_guest)
		exec_control &= ~SECONDARY_EXEC_UNRESTRICTED_GUEST;
	if (kvm_pause_in_guest(vmx->vcpu.kvm))
		exec_control &= ~SECONDARY_EXEC_PAUSE_LOOP_EXITING;
	if (!kvm_vcpu_apicv_active(vcpu))
		exec_control &= ~(SECONDARY_EXEC_APIC_REGISTER_VIRT |
				  SECONDARY_EXEC_VIRTUAL_INTR_DELIVERY);
	exec_control &= ~SECONDARY_EXEC_VIRTUALIZE_X2APIC_MODE;

	/* SECONDARY_EXEC_DESC is enabled/disabled on writes to CR4.UMIP,
	 * in vmx_set_cr4.  */
	exec_control &= ~SECONDARY_EXEC_DESC;

	/* SECONDARY_EXEC_SHADOW_VMCS is enabled when L1 executes VMPTRLD
	   (handle_vmptrld).
	   We can NOT enable shadow_vmcs here because we don't have yet
	   a current VMCS12
	*/
	exec_control &= ~SECONDARY_EXEC_SHADOW_VMCS;

	/*
	 * PML is enabled/disabled when dirty logging of memsmlots changes, but
	 * it needs to be set here when dirty logging is already active, e.g.
	 * if this vCPU was created after dirty logging was enabled.
	 */
	if (!vcpu->kvm->arch.cpu_dirty_logging_count)
		exec_control &= ~SECONDARY_EXEC_ENABLE_PML;

	if (cpu_has_vmx_xsaves()) {
		/* Exposing XSAVES only when XSAVE is exposed */
		bool xsaves_enabled =
			boot_cpu_has(X86_FEATURE_XSAVE) &&
			guest_cpuid_has(vcpu, X86_FEATURE_XSAVE) &&
			guest_cpuid_has(vcpu, X86_FEATURE_XSAVES);

		vcpu->arch.xsaves_enabled = xsaves_enabled;

		vmx_adjust_secondary_exec_control(vmx, &exec_control,
						  SECONDARY_EXEC_XSAVES,
						  xsaves_enabled, false);
	}

	/*
	 * RDPID is also gated by ENABLE_RDTSCP, turn on the control if either
	 * feature is exposed to the guest.  This creates a virtualization hole
	 * if both are supported in hardware but only one is exposed to the
	 * guest, but letting the guest execute RDTSCP or RDPID when either one
	 * is advertised is preferable to emulating the advertised instruction
	 * in KVM on #UD, and obviously better than incorrectly injecting #UD.
	 */
	if (cpu_has_vmx_rdtscp()) {
		bool rdpid_or_rdtscp_enabled =
			guest_cpuid_has(vcpu, X86_FEATURE_RDTSCP) ||
			guest_cpuid_has(vcpu, X86_FEATURE_RDPID);

		vmx_adjust_secondary_exec_control(vmx, &exec_control,
						  SECONDARY_EXEC_ENABLE_RDTSCP,
						  rdpid_or_rdtscp_enabled, false);
	}
	vmx_adjust_sec_exec_feature(vmx, &exec_control, invpcid, INVPCID);

	vmx_adjust_sec_exec_exiting(vmx, &exec_control, rdrand, RDRAND);
	vmx_adjust_sec_exec_exiting(vmx, &exec_control, rdseed, RDSEED);

	vmx_adjust_sec_exec_control(vmx, &exec_control, waitpkg, WAITPKG,
				    ENABLE_USR_WAIT_PAUSE, false);

	if (!vcpu->kvm->arch.bus_lock_detection_enabled)
		exec_control &= ~SECONDARY_EXEC_BUS_LOCK_DETECTION;

	vmx->secondary_exec_control = exec_control;
}

#define VMX_XSS_EXIT_BITMAP 0

/*
 * Noting that the initialization of Guest-state Area of VMCS is in
 * vmx_vcpu_reset().
 */
static void init_vmcs(struct vcpu_vmx *vmx)
{
	if (nested)
		nested_vmx_set_vmcs_shadowing_bitmap();

	if (cpu_has_vmx_msr_bitmap())
		vmcs_write64(MSR_BITMAP, __pa(vmx->vmcs01.msr_bitmap));

	vmcs_write64(VMCS_LINK_POINTER, -1ull); /* 22.3.1.5 */

	/* Control */
	pin_controls_set(vmx, vmx_pin_based_exec_ctrl(vmx));

	exec_controls_set(vmx, vmx_exec_control(vmx));

	if (cpu_has_secondary_exec_ctrls()) {
		vmx_compute_secondary_exec_control(vmx);
		secondary_exec_controls_set(vmx, vmx->secondary_exec_control);
	}

	if (kvm_vcpu_apicv_active(&vmx->vcpu)) {
		vmcs_write64(EOI_EXIT_BITMAP0, 0);
		vmcs_write64(EOI_EXIT_BITMAP1, 0);
		vmcs_write64(EOI_EXIT_BITMAP2, 0);
		vmcs_write64(EOI_EXIT_BITMAP3, 0);

		vmcs_write16(GUEST_INTR_STATUS, 0);

		vmcs_write16(POSTED_INTR_NV, POSTED_INTR_VECTOR);
		vmcs_write64(POSTED_INTR_DESC_ADDR, __pa((&vmx->pi_desc)));
	}

	if (!kvm_pause_in_guest(vmx->vcpu.kvm)) {
		vmcs_write32(PLE_GAP, ple_gap);
		vmx->ple_window = ple_window;
		vmx->ple_window_dirty = true;
	}

	vmcs_write32(PAGE_FAULT_ERROR_CODE_MASK, 0);
	vmcs_write32(PAGE_FAULT_ERROR_CODE_MATCH, 0);
	vmcs_write32(CR3_TARGET_COUNT, 0);           /* 22.2.1 */

	vmcs_write16(HOST_FS_SELECTOR, 0);            /* 22.2.4 */
	vmcs_write16(HOST_GS_SELECTOR, 0);            /* 22.2.4 */
	vmx_set_constant_host_state(vmx);
	vmcs_writel(HOST_FS_BASE, 0); /* 22.2.4 */
	vmcs_writel(HOST_GS_BASE, 0); /* 22.2.4 */

	if (cpu_has_vmx_vmfunc())
		vmcs_write64(VM_FUNCTION_CONTROL, 0);

	vmcs_write32(VM_EXIT_MSR_STORE_COUNT, 0);
	vmcs_write32(VM_EXIT_MSR_LOAD_COUNT, 0);
	vmcs_write64(VM_EXIT_MSR_LOAD_ADDR, __pa(vmx->msr_autoload.host.val));
	vmcs_write32(VM_ENTRY_MSR_LOAD_COUNT, 0);
	vmcs_write64(VM_ENTRY_MSR_LOAD_ADDR, __pa(vmx->msr_autoload.guest.val));

	if (vmcs_config.vmentry_ctrl & VM_ENTRY_LOAD_IA32_PAT)
		vmcs_write64(GUEST_IA32_PAT, vmx->vcpu.arch.pat);

	vm_exit_controls_set(vmx, vmx_vmexit_ctrl());

	/* 22.2.1, 20.8.1 */
	vm_entry_controls_set(vmx, vmx_vmentry_ctrl());

	vmx->vcpu.arch.cr0_guest_owned_bits = KVM_POSSIBLE_CR0_GUEST_BITS;
	vmcs_writel(CR0_GUEST_HOST_MASK, ~vmx->vcpu.arch.cr0_guest_owned_bits);

	set_cr4_guest_host_mask(vmx);

	if (vmx->vpid != 0)
		vmcs_write16(VIRTUAL_PROCESSOR_ID, vmx->vpid);

	if (cpu_has_vmx_xsaves())
		vmcs_write64(XSS_EXIT_BITMAP, VMX_XSS_EXIT_BITMAP);

	if (enable_pml) {
		vmcs_write64(PML_ADDRESS, page_to_phys(vmx->pml_pg));
		vmcs_write16(GUEST_PML_INDEX, PML_ENTITY_NUM - 1);
	}

	vmx_write_encls_bitmap(&vmx->vcpu, NULL);

	if (vmx_pt_mode_is_host_guest()) {
		memset(&vmx->pt_desc, 0, sizeof(vmx->pt_desc));
		/* Bit[6~0] are forced to 1, writes are ignored. */
		vmx->pt_desc.guest.output_mask = 0x7F;
		vmcs_write64(GUEST_IA32_RTIT_CTL, 0);
	}
}

static void vmx_vcpu_reset(struct kvm_vcpu *vcpu, bool init_event)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	struct msr_data apic_base_msr;
	u64 cr0;

	vmx->rmode.vm86_active = 0;
	vmx->spec_ctrl = 0;

	vmx->msr_ia32_umwait_control = 0;

	vmx->vcpu.arch.regs[VCPU_REGS_RDX] = get_rdx_init_val();
	vmx->hv_deadline_tsc = -1;
	kvm_set_cr8(vcpu, 0);

	if (!init_event) {
		apic_base_msr.data = APIC_DEFAULT_PHYS_BASE |
				     MSR_IA32_APICBASE_ENABLE;
		if (kvm_vcpu_is_reset_bsp(vcpu))
			apic_base_msr.data |= MSR_IA32_APICBASE_BSP;
		apic_base_msr.host_initiated = true;
		kvm_set_apic_base(vcpu, &apic_base_msr);
	}

	vmx_segment_cache_clear(vmx);

	seg_setup(VCPU_SREG_CS);
	vmcs_write16(GUEST_CS_SELECTOR, 0xf000);
	vmcs_writel(GUEST_CS_BASE, 0xffff0000ul);

	seg_setup(VCPU_SREG_DS);
	seg_setup(VCPU_SREG_ES);
	seg_setup(VCPU_SREG_FS);
	seg_setup(VCPU_SREG_GS);
	seg_setup(VCPU_SREG_SS);

	vmcs_write16(GUEST_TR_SELECTOR, 0);
	vmcs_writel(GUEST_TR_BASE, 0);
	vmcs_write32(GUEST_TR_LIMIT, 0xffff);
	vmcs_write32(GUEST_TR_AR_BYTES, 0x008b);

	vmcs_write16(GUEST_LDTR_SELECTOR, 0);
	vmcs_writel(GUEST_LDTR_BASE, 0);
	vmcs_write32(GUEST_LDTR_LIMIT, 0xffff);
	vmcs_write32(GUEST_LDTR_AR_BYTES, 0x00082);

	if (!init_event) {
		vmcs_write32(GUEST_SYSENTER_CS, 0);
		vmcs_writel(GUEST_SYSENTER_ESP, 0);
		vmcs_writel(GUEST_SYSENTER_EIP, 0);
		vmcs_write64(GUEST_IA32_DEBUGCTL, 0);
	}

	kvm_set_rflags(vcpu, X86_EFLAGS_FIXED);
	kvm_rip_write(vcpu, 0xfff0);

	vmcs_writel(GUEST_GDTR_BASE, 0);
	vmcs_write32(GUEST_GDTR_LIMIT, 0xffff);

	vmcs_writel(GUEST_IDTR_BASE, 0);
	vmcs_write32(GUEST_IDTR_LIMIT, 0xffff);

	vmcs_write32(GUEST_ACTIVITY_STATE, GUEST_ACTIVITY_ACTIVE);
	vmcs_write32(GUEST_INTERRUPTIBILITY_INFO, 0);
	vmcs_writel(GUEST_PENDING_DBG_EXCEPTIONS, 0);
	if (kvm_mpx_supported())
		vmcs_write64(GUEST_BNDCFGS, 0);

	setup_msrs(vmx);

	vmcs_write32(VM_ENTRY_INTR_INFO_FIELD, 0);  /* 22.2.1 */

	if (cpu_has_vmx_tpr_shadow() && !init_event) {
		vmcs_write64(VIRTUAL_APIC_PAGE_ADDR, 0);
		if (cpu_need_tpr_shadow(vcpu))
			vmcs_write64(VIRTUAL_APIC_PAGE_ADDR,
				     __pa(vcpu->arch.apic->regs));
		vmcs_write32(TPR_THRESHOLD, 0);
	}

	kvm_make_request(KVM_REQ_APIC_PAGE_RELOAD, vcpu);

	cr0 = X86_CR0_NW | X86_CR0_CD | X86_CR0_ET;
	vmx->vcpu.arch.cr0 = cr0;
	vmx_set_cr0(vcpu, cr0); /* enter rmode */
	vmx_set_cr4(vcpu, 0);
	vmx_set_efer(vcpu, 0);

	vmx_update_exception_bitmap(vcpu);

	vpid_sync_context(vmx->vpid);
	if (init_event)
		vmx_clear_hlt(vcpu);
}

static void vmx_enable_irq_window(struct kvm_vcpu *vcpu)
{
	exec_controls_setbit(to_vmx(vcpu), CPU_BASED_INTR_WINDOW_EXITING);
}

static void vmx_enable_nmi_window(struct kvm_vcpu *vcpu)
{
	if (!enable_vnmi ||
	    vmcs_read32(GUEST_INTERRUPTIBILITY_INFO) & GUEST_INTR_STATE_STI) {
		vmx_enable_irq_window(vcpu);
		return;
	}

	exec_controls_setbit(to_vmx(vcpu), CPU_BASED_NMI_WINDOW_EXITING);
}

static void vmx_inject_irq(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	uint32_t intr;
	int irq = vcpu->arch.interrupt.nr;

	trace_kvm_inj_virq(irq);

	++vcpu->stat.irq_injections;
	if (vmx->rmode.vm86_active) {
		int inc_eip = 0;
		if (vcpu->arch.interrupt.soft)
			inc_eip = vcpu->arch.event_exit_inst_len;
		kvm_inject_realmode_interrupt(vcpu, irq, inc_eip);
		return;
	}
	intr = irq | INTR_INFO_VALID_MASK;
	if (vcpu->arch.interrupt.soft) {
		intr |= INTR_TYPE_SOFT_INTR;
		vmcs_write32(VM_ENTRY_INSTRUCTION_LEN,
			     vmx->vcpu.arch.event_exit_inst_len);
	} else
		intr |= INTR_TYPE_EXT_INTR;
	vmcs_write32(VM_ENTRY_INTR_INFO_FIELD, intr);

	vmx_clear_hlt(vcpu);
}

static void vmx_inject_nmi(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);

	if (!enable_vnmi) {
		/*
		 * Tracking the NMI-blocked state in software is built upon
		 * finding the next open IRQ window. This, in turn, depends on
		 * well-behaving guests: They have to keep IRQs disabled at
		 * least as long as the NMI handler runs. Otherwise we may
		 * cause NMI nesting, maybe breaking the guest. But as this is
		 * highly unlikely, we can live with the residual risk.
		 */
		vmx->loaded_vmcs->soft_vnmi_blocked = 1;
		vmx->loaded_vmcs->vnmi_blocked_time = 0;
	}

	++vcpu->stat.nmi_injections;
	vmx->loaded_vmcs->nmi_known_unmasked = false;

	if (vmx->rmode.vm86_active) {
		kvm_inject_realmode_interrupt(vcpu, NMI_VECTOR, 0);
		return;
	}

	vmcs_write32(VM_ENTRY_INTR_INFO_FIELD,
			INTR_TYPE_NMI_INTR | INTR_INFO_VALID_MASK | NMI_VECTOR);

	vmx_clear_hlt(vcpu);
}

bool vmx_get_nmi_mask(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	bool masked;

	if (!enable_vnmi)
		return vmx->loaded_vmcs->soft_vnmi_blocked;
	if (vmx->loaded_vmcs->nmi_known_unmasked)
		return false;
	masked = vmcs_read32(GUEST_INTERRUPTIBILITY_INFO) & GUEST_INTR_STATE_NMI;
	vmx->loaded_vmcs->nmi_known_unmasked = !masked;
	return masked;
}

void vmx_set_nmi_mask(struct kvm_vcpu *vcpu, bool masked)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);

	if (!enable_vnmi) {
		if (vmx->loaded_vmcs->soft_vnmi_blocked != masked) {
			vmx->loaded_vmcs->soft_vnmi_blocked = masked;
			vmx->loaded_vmcs->vnmi_blocked_time = 0;
		}
	} else {
		vmx->loaded_vmcs->nmi_known_unmasked = !masked;
		if (masked)
			vmcs_set_bits(GUEST_INTERRUPTIBILITY_INFO,
				      GUEST_INTR_STATE_NMI);
		else
			vmcs_clear_bits(GUEST_INTERRUPTIBILITY_INFO,
					GUEST_INTR_STATE_NMI);
	}
}

bool vmx_nmi_blocked(struct kvm_vcpu *vcpu)
{
	if (is_guest_mode(vcpu) && nested_exit_on_nmi(vcpu))
		return false;

	if (!enable_vnmi && to_vmx(vcpu)->loaded_vmcs->soft_vnmi_blocked)
		return true;

	return (vmcs_read32(GUEST_INTERRUPTIBILITY_INFO) &
		(GUEST_INTR_STATE_MOV_SS | GUEST_INTR_STATE_STI |
		 GUEST_INTR_STATE_NMI));
}

static int vmx_nmi_allowed(struct kvm_vcpu *vcpu, bool for_injection)
{
	if (to_vmx(vcpu)->nested.nested_run_pending)
		return -EBUSY;

	/* An NMI must not be injected into L2 if it's supposed to VM-Exit.  */
	if (for_injection && is_guest_mode(vcpu) && nested_exit_on_nmi(vcpu))
		return -EBUSY;

	return !vmx_nmi_blocked(vcpu);
}

bool vmx_interrupt_blocked(struct kvm_vcpu *vcpu)
{
	if (is_guest_mode(vcpu) && nested_exit_on_intr(vcpu))
		return false;

	return !(vmx_get_rflags(vcpu) & X86_EFLAGS_IF) ||
	       (vmcs_read32(GUEST_INTERRUPTIBILITY_INFO) &
		(GUEST_INTR_STATE_STI | GUEST_INTR_STATE_MOV_SS));
}

static int vmx_interrupt_allowed(struct kvm_vcpu *vcpu, bool for_injection)
{
	if (to_vmx(vcpu)->nested.nested_run_pending)
		return -EBUSY;

       /*
        * An IRQ must not be injected into L2 if it's supposed to VM-Exit,
        * e.g. if the IRQ arrived asynchronously after checking nested events.
        */
	if (for_injection && is_guest_mode(vcpu) && nested_exit_on_intr(vcpu))
		return -EBUSY;

	return !vmx_interrupt_blocked(vcpu);
}

static int vmx_set_tss_addr(struct kvm *kvm, unsigned int addr)
{
	void __user *ret;

	if (enable_unrestricted_guest)
		return 0;

	mutex_lock(&kvm->slots_lock);
	ret = __x86_set_memory_region(kvm, TSS_PRIVATE_MEMSLOT, addr,
				      PAGE_SIZE * 3);
	mutex_unlock(&kvm->slots_lock);

	if (IS_ERR(ret))
		return PTR_ERR(ret);

	to_kvm_vmx(kvm)->tss_addr = addr;

	return init_rmode_tss(kvm, ret);
}

static int vmx_set_identity_map_addr(struct kvm *kvm, u64 ident_addr)
{
	to_kvm_vmx(kvm)->ept_identity_map_addr = ident_addr;
	return 0;
}

static bool rmode_exception(struct kvm_vcpu *vcpu, int vec)
{
	switch (vec) {
	case BP_VECTOR:
		/*
		 * Update instruction length as we may reinject the exception
		 * from user space while in guest debugging mode.
		 */
		to_vmx(vcpu)->vcpu.arch.event_exit_inst_len =
			vmcs_read32(VM_EXIT_INSTRUCTION_LEN);
		if (vcpu->guest_debug & KVM_GUESTDBG_USE_SW_BP)
			return false;
		fallthrough;
	case DB_VECTOR:
		return !(vcpu->guest_debug &
			(KVM_GUESTDBG_SINGLESTEP | KVM_GUESTDBG_USE_HW_BP));
	case DE_VECTOR:
	case OF_VECTOR:
	case BR_VECTOR:
	case UD_VECTOR:
	case DF_VECTOR:
	case SS_VECTOR:
	case GP_VECTOR:
	case MF_VECTOR:
		return true;
	}
	return false;
}

static int handle_rmode_exception(struct kvm_vcpu *vcpu,
				  int vec, u32 err_code)
{
	/*
	 * Instruction with address size override prefix opcode 0x67
	 * Cause the #SS fault with 0 error code in VM86 mode.
	 */
	if (((vec == GP_VECTOR) || (vec == SS_VECTOR)) && err_code == 0) {
		if (kvm_emulate_instruction(vcpu, 0)) {
			if (vcpu->arch.halt_request) {
				vcpu->arch.halt_request = 0;
				return kvm_vcpu_halt(vcpu);
			}
			return 1;
		}
		return 0;
	}

	/*
	 * Forward all other exceptions that are valid in real mode.
	 * FIXME: Breaks guest debugging in real mode, needs to be fixed with
	 *        the required debugging infrastructure rework.
	 */
	kvm_queue_exception(vcpu, vec);
	return 1;
}

static int handle_machine_check(struct kvm_vcpu *vcpu)
{
	/* handled by vmx_vcpu_run() */
	return 1;
}

/*
 * If the host has split lock detection disabled, then #AC is
 * unconditionally injected into the guest, which is the pre split lock
 * detection behaviour.
 *
 * If the host has split lock detection enabled then #AC is
 * only injected into the guest when:
 *  - Guest CPL == 3 (user mode)
 *  - Guest has #AC detection enabled in CR0
 *  - Guest EFLAGS has AC bit set
 */
static inline bool guest_inject_ac(struct kvm_vcpu *vcpu)
{
	if (!boot_cpu_has(X86_FEATURE_SPLIT_LOCK_DETECT))
		return true;

	return vmx_get_cpl(vcpu) == 3 && kvm_read_cr0_bits(vcpu, X86_CR0_AM) &&
	       (kvm_get_rflags(vcpu) & X86_EFLAGS_AC);
}

static int handle_exception_nmi(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	struct kvm_run *kvm_run = vcpu->run;
	u32 intr_info, ex_no, error_code;
	unsigned long cr2, dr6;
	u32 vect_info;

	vect_info = vmx->idt_vectoring_info;
	intr_info = vmx_get_intr_info(vcpu);

	if (is_machine_check(intr_info) || is_nmi(intr_info))
		return 1; /* handled by handle_exception_nmi_irqoff() */

	if (is_invalid_opcode(intr_info))
		return handle_ud(vcpu);

	error_code = 0;
	if (intr_info & INTR_INFO_DELIVER_CODE_MASK)
		error_code = vmcs_read32(VM_EXIT_INTR_ERROR_CODE);

	if (!vmx->rmode.vm86_active && is_gp_fault(intr_info)) {
		WARN_ON_ONCE(!enable_vmware_backdoor);

		/*
		 * VMware backdoor emulation on #GP interception only handles
		 * IN{S}, OUT{S}, and RDPMC, none of which generate a non-zero
		 * error code on #GP.
		 */
		if (error_code) {
			kvm_queue_exception_e(vcpu, GP_VECTOR, error_code);
			return 1;
		}
		return kvm_emulate_instruction(vcpu, EMULTYPE_VMWARE_GP);
	}

	/*
	 * The #PF with PFEC.RSVD = 1 indicates the guest is accessing
	 * MMIO, it is better to report an internal error.
	 * See the comments in vmx_handle_exit.
	 */
	if ((vect_info & VECTORING_INFO_VALID_MASK) &&
	    !(is_page_fault(intr_info) && !(error_code & PFERR_RSVD_MASK))) {
		vcpu->run->exit_reason = KVM_EXIT_INTERNAL_ERROR;
		vcpu->run->internal.suberror = KVM_INTERNAL_ERROR_SIMUL_EX;
		vcpu->run->internal.ndata = 4;
		vcpu->run->internal.data[0] = vect_info;
		vcpu->run->internal.data[1] = intr_info;
		vcpu->run->internal.data[2] = error_code;
		vcpu->run->internal.data[3] = vcpu->arch.last_vmentry_cpu;
		return 0;
	}

	if (is_page_fault(intr_info)) {
		cr2 = vmx_get_exit_qual(vcpu);
		if (enable_ept && !vcpu->arch.apf.host_apf_flags) {
			/*
			 * EPT will cause page fault only if we need to
			 * detect illegal GPAs.
			 */
			WARN_ON_ONCE(!allow_smaller_maxphyaddr);
			kvm_fixup_and_inject_pf_error(vcpu, cr2, error_code);
			return 1;
		} else
			return kvm_handle_page_fault(vcpu, error_code, cr2, NULL, 0);
	}

	ex_no = intr_info & INTR_INFO_VECTOR_MASK;

	if (vmx->rmode.vm86_active && rmode_exception(vcpu, ex_no))
		return handle_rmode_exception(vcpu, ex_no, error_code);

	switch (ex_no) {
	case DB_VECTOR:
		dr6 = vmx_get_exit_qual(vcpu);
		if (!(vcpu->guest_debug &
		      (KVM_GUESTDBG_SINGLESTEP | KVM_GUESTDBG_USE_HW_BP))) {
			if (is_icebp(intr_info))
				WARN_ON(!skip_emulated_instruction(vcpu));

			kvm_queue_exception_p(vcpu, DB_VECTOR, dr6);
			return 1;
		}
		kvm_run->debug.arch.dr6 = dr6 | DR6_ACTIVE_LOW;
		kvm_run->debug.arch.dr7 = vmcs_readl(GUEST_DR7);
		fallthrough;
	case BP_VECTOR:
		/*
		 * Update instruction length as we may reinject #BP from
		 * user space while in guest debugging mode. Reading it for
		 * #DB as well causes no harm, it is not used in that case.
		 */
		vmx->vcpu.arch.event_exit_inst_len =
			vmcs_read32(VM_EXIT_INSTRUCTION_LEN);
		kvm_run->exit_reason = KVM_EXIT_DEBUG;
		kvm_run->debug.arch.pc = kvm_get_linear_rip(vcpu);
		kvm_run->debug.arch.exception = ex_no;
		break;
	case AC_VECTOR:
		if (guest_inject_ac(vcpu)) {
			kvm_queue_exception_e(vcpu, AC_VECTOR, error_code);
			return 1;
		}

		/*
		 * Handle split lock. Depending on detection mode this will
		 * either warn and disable split lock detection for this
		 * task or force SIGBUS on it.
		 */
		if (handle_guest_split_lock(kvm_rip_read(vcpu)))
			return 1;
		fallthrough;
	default:
		kvm_run->exit_reason = KVM_EXIT_EXCEPTION;
		kvm_run->ex.exception = ex_no;
		kvm_run->ex.error_code = error_code;
		break;
	}
	return 0;
}

static __always_inline int handle_external_interrupt(struct kvm_vcpu *vcpu)
{
	++vcpu->stat.irq_exits;
	return 1;
}

static int handle_triple_fault(struct kvm_vcpu *vcpu)
{
	vcpu->run->exit_reason = KVM_EXIT_SHUTDOWN;
	vcpu->mmio_needed = 0;
	return 0;
}

static int handle_io(struct kvm_vcpu *vcpu)
{
	unsigned long exit_qualification;
	int size, in, string;
	unsigned port;

	exit_qualification = vmx_get_exit_qual(vcpu);
	string = (exit_qualification & 16) != 0;

	++vcpu->stat.io_exits;

	if (string)
		return kvm_emulate_instruction(vcpu, 0);

	port = exit_qualification >> 16;
	size = (exit_qualification & 7) + 1;
	in = (exit_qualification & 8) != 0;

	return kvm_fast_pio(vcpu, size, port, in);
}

static void
vmx_patch_hypercall(struct kvm_vcpu *vcpu, unsigned char *hypercall)
{
	/*
	 * Patch in the VMCALL instruction:
	 */
	hypercall[0] = 0x0f;
	hypercall[1] = 0x01;
	hypercall[2] = 0xc1;
}

/* called to set cr0 as appropriate for a mov-to-cr0 exit. */
static int handle_set_cr0(struct kvm_vcpu *vcpu, unsigned long val)
{
	if (is_guest_mode(vcpu)) {
		struct vmcs12 *vmcs12 = get_vmcs12(vcpu);
		unsigned long orig_val = val;

		/*
		 * We get here when L2 changed cr0 in a way that did not change
		 * any of L1's shadowed bits (see nested_vmx_exit_handled_cr),
		 * but did change L0 shadowed bits. So we first calculate the
		 * effective cr0 value that L1 would like to write into the
		 * hardware. It consists of the L2-owned bits from the new
		 * value combined with the L1-owned bits from L1's guest_cr0.
		 */
		val = (val & ~vmcs12->cr0_guest_host_mask) |
			(vmcs12->guest_cr0 & vmcs12->cr0_guest_host_mask);

		if (!nested_guest_cr0_valid(vcpu, val))
			return 1;

		if (kvm_set_cr0(vcpu, val))
			return 1;
		vmcs_writel(CR0_READ_SHADOW, orig_val);
		return 0;
	} else {
		if (to_vmx(vcpu)->nested.vmxon &&
		    !nested_host_cr0_valid(vcpu, val))
			return 1;

		return kvm_set_cr0(vcpu, val);
	}
}

static int handle_set_cr4(struct kvm_vcpu *vcpu, unsigned long val)
{
	if (is_guest_mode(vcpu)) {
		struct vmcs12 *vmcs12 = get_vmcs12(vcpu);
		unsigned long orig_val = val;

		/* analogously to handle_set_cr0 */
		val = (val & ~vmcs12->cr4_guest_host_mask) |
			(vmcs12->guest_cr4 & vmcs12->cr4_guest_host_mask);
		if (kvm_set_cr4(vcpu, val))
			return 1;
		vmcs_writel(CR4_READ_SHADOW, orig_val);
		return 0;
	} else
		return kvm_set_cr4(vcpu, val);
}

static int handle_desc(struct kvm_vcpu *vcpu)
{
	WARN_ON(!(vcpu->arch.cr4 & X86_CR4_UMIP));
	return kvm_emulate_instruction(vcpu, 0);
}

static int handle_cr(struct kvm_vcpu *vcpu)
{
	unsigned long exit_qualification, val;
	int cr;
	int reg;
	int err;
	int ret;

	exit_qualification = vmx_get_exit_qual(vcpu);
	cr = exit_qualification & 15;
	reg = (exit_qualification >> 8) & 15;
	switch ((exit_qualification >> 4) & 3) {
	case 0: /* mov to cr */
		val = kvm_register_read(vcpu, reg);
		trace_kvm_cr_write(cr, val);
		switch (cr) {
		case 0:
			err = handle_set_cr0(vcpu, val);
			return kvm_complete_insn_gp(vcpu, err);
		case 3:
			WARN_ON_ONCE(enable_unrestricted_guest);
			err = kvm_set_cr3(vcpu, val);
			return kvm_complete_insn_gp(vcpu, err);
		case 4:
			err = handle_set_cr4(vcpu, val);
			return kvm_complete_insn_gp(vcpu, err);
		case 8: {
				u8 cr8_prev = kvm_get_cr8(vcpu);
				u8 cr8 = (u8)val;
				err = kvm_set_cr8(vcpu, cr8);
				ret = kvm_complete_insn_gp(vcpu, err);
				if (lapic_in_kernel(vcpu))
					return ret;
				if (cr8_prev <= cr8)
					return ret;
				/*
				 * TODO: we might be squashing a
				 * KVM_GUESTDBG_SINGLESTEP-triggered
				 * KVM_EXIT_DEBUG here.
				 */
				vcpu->run->exit_reason = KVM_EXIT_SET_TPR;
				return 0;
			}
		}
		break;
	case 2: /* clts */
		WARN_ONCE(1, "Guest should always own CR0.TS");
		vmx_set_cr0(vcpu, kvm_read_cr0_bits(vcpu, ~X86_CR0_TS));
		trace_kvm_cr_write(0, kvm_read_cr0(vcpu));
		return kvm_skip_emulated_instruction(vcpu);
	case 1: /*mov from cr*/
		switch (cr) {
		case 3:
			WARN_ON_ONCE(enable_unrestricted_guest);
			val = kvm_read_cr3(vcpu);
			kvm_register_write(vcpu, reg, val);
			trace_kvm_cr_read(cr, val);
			return kvm_skip_emulated_instruction(vcpu);
		case 8:
			val = kvm_get_cr8(vcpu);
			kvm_register_write(vcpu, reg, val);
			trace_kvm_cr_read(cr, val);
			return kvm_skip_emulated_instruction(vcpu);
		}
		break;
	case 3: /* lmsw */
		val = (exit_qualification >> LMSW_SOURCE_DATA_SHIFT) & 0x0f;
		trace_kvm_cr_write(0, (kvm_read_cr0(vcpu) & ~0xful) | val);
		kvm_lmsw(vcpu, val);

		return kvm_skip_emulated_instruction(vcpu);
	default:
		break;
	}
	vcpu->run->exit_reason = 0;
	vcpu_unimpl(vcpu, "unhandled control register: op %d cr %d\n",
	       (int)(exit_qualification >> 4) & 3, cr);
	return 0;
}

static int handle_dr(struct kvm_vcpu *vcpu)
{
	unsigned long exit_qualification;
	int dr, dr7, reg;
	int err = 1;

	exit_qualification = vmx_get_exit_qual(vcpu);
	dr = exit_qualification & DEBUG_REG_ACCESS_NUM;

	/* First, if DR does not exist, trigger UD */
	if (!kvm_require_dr(vcpu, dr))
		return 1;

	if (kvm_x86_ops.get_cpl(vcpu) > 0)
		goto out;

	dr7 = vmcs_readl(GUEST_DR7);
	if (dr7 & DR7_GD) {
		/*
		 * As the vm-exit takes precedence over the debug trap, we
		 * need to emulate the latter, either for the host or the
		 * guest debugging itself.
		 */
		if (vcpu->guest_debug & KVM_GUESTDBG_USE_HW_BP) {
			vcpu->run->debug.arch.dr6 = DR6_BD | DR6_ACTIVE_LOW;
			vcpu->run->debug.arch.dr7 = dr7;
			vcpu->run->debug.arch.pc = kvm_get_linear_rip(vcpu);
			vcpu->run->debug.arch.exception = DB_VECTOR;
			vcpu->run->exit_reason = KVM_EXIT_DEBUG;
			return 0;
		} else {
			kvm_queue_exception_p(vcpu, DB_VECTOR, DR6_BD);
			return 1;
		}
	}

	if (vcpu->guest_debug == 0) {
		exec_controls_clearbit(to_vmx(vcpu), CPU_BASED_MOV_DR_EXITING);

		/*
		 * No more DR vmexits; force a reload of the debug registers
		 * and reenter on this instruction.  The next vmexit will
		 * retrieve the full state of the debug registers.
		 */
		vcpu->arch.switch_db_regs |= KVM_DEBUGREG_WONT_EXIT;
		return 1;
	}

	reg = DEBUG_REG_ACCESS_REG(exit_qualification);
	if (exit_qualification & TYPE_MOV_FROM_DR) {
		unsigned long val;

		kvm_get_dr(vcpu, dr, &val);
		kvm_register_write(vcpu, reg, val);
		err = 0;
	} else {
		err = kvm_set_dr(vcpu, dr, kvm_register_read(vcpu, reg));
	}

out:
	return kvm_complete_insn_gp(vcpu, err);
}

static void vmx_sync_dirty_debug_regs(struct kvm_vcpu *vcpu)
{
	get_debugreg(vcpu->arch.db[0], 0);
	get_debugreg(vcpu->arch.db[1], 1);
	get_debugreg(vcpu->arch.db[2], 2);
	get_debugreg(vcpu->arch.db[3], 3);
	get_debugreg(vcpu->arch.dr6, 6);
	vcpu->arch.dr7 = vmcs_readl(GUEST_DR7);

	vcpu->arch.switch_db_regs &= ~KVM_DEBUGREG_WONT_EXIT;
	exec_controls_setbit(to_vmx(vcpu), CPU_BASED_MOV_DR_EXITING);
}

static void vmx_set_dr7(struct kvm_vcpu *vcpu, unsigned long val)
{
	vmcs_writel(GUEST_DR7, val);
}

static int handle_tpr_below_threshold(struct kvm_vcpu *vcpu)
{
	kvm_apic_update_ppr(vcpu);
	return 1;
}

static int handle_interrupt_window(struct kvm_vcpu *vcpu)
{
	exec_controls_clearbit(to_vmx(vcpu), CPU_BASED_INTR_WINDOW_EXITING);

	kvm_make_request(KVM_REQ_EVENT, vcpu);

	++vcpu->stat.irq_window_exits;
	return 1;
}

static int handle_invlpg(struct kvm_vcpu *vcpu)
{
	unsigned long exit_qualification = vmx_get_exit_qual(vcpu);

	kvm_mmu_invlpg(vcpu, exit_qualification);
	return kvm_skip_emulated_instruction(vcpu);
}

static int handle_apic_access(struct kvm_vcpu *vcpu)
{
	if (likely(fasteoi)) {
		unsigned long exit_qualification = vmx_get_exit_qual(vcpu);
		int access_type, offset;

		access_type = exit_qualification & APIC_ACCESS_TYPE;
		offset = exit_qualification & APIC_ACCESS_OFFSET;
		/*
		 * Sane guest uses MOV to write EOI, with written value
		 * not cared. So make a short-circuit here by avoiding
		 * heavy instruction emulation.
		 */
		if ((access_type == TYPE_LINEAR_APIC_INST_WRITE) &&
		    (offset == APIC_EOI)) {
			kvm_lapic_set_eoi(vcpu);
			return kvm_skip_emulated_instruction(vcpu);
		}
	}
	return kvm_emulate_instruction(vcpu, 0);
}

static int handle_apic_eoi_induced(struct kvm_vcpu *vcpu)
{
	unsigned long exit_qualification = vmx_get_exit_qual(vcpu);
	int vector = exit_qualification & 0xff;

	/* EOI-induced VM exit is trap-like and thus no need to adjust IP */
	kvm_apic_set_eoi_accelerated(vcpu, vector);
	return 1;
}

static int handle_apic_write(struct kvm_vcpu *vcpu)
{
	unsigned long exit_qualification = vmx_get_exit_qual(vcpu);
	u32 offset = exit_qualification & 0xfff;

	/* APIC-write VM exit is trap-like and thus no need to adjust IP */
	kvm_apic_write_nodecode(vcpu, offset);
	return 1;
}

static int handle_task_switch(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	unsigned long exit_qualification;
	bool has_error_code = false;
	u32 error_code = 0;
	u16 tss_selector;
	int reason, type, idt_v, idt_index;

	idt_v = (vmx->idt_vectoring_info & VECTORING_INFO_VALID_MASK);
	idt_index = (vmx->idt_vectoring_info & VECTORING_INFO_VECTOR_MASK);
	type = (vmx->idt_vectoring_info & VECTORING_INFO_TYPE_MASK);

	exit_qualification = vmx_get_exit_qual(vcpu);

	reason = (u32)exit_qualification >> 30;
	if (reason == TASK_SWITCH_GATE && idt_v) {
		switch (type) {
		case INTR_TYPE_NMI_INTR:
			vcpu->arch.nmi_injected = false;
			vmx_set_nmi_mask(vcpu, true);
			break;
		case INTR_TYPE_EXT_INTR:
		case INTR_TYPE_SOFT_INTR:
			kvm_clear_interrupt_queue(vcpu);
			break;
		case INTR_TYPE_HARD_EXCEPTION:
			if (vmx->idt_vectoring_info &
			    VECTORING_INFO_DELIVER_CODE_MASK) {
				has_error_code = true;
				error_code =
					vmcs_read32(IDT_VECTORING_ERROR_CODE);
			}
			fallthrough;
		case INTR_TYPE_SOFT_EXCEPTION:
			kvm_clear_exception_queue(vcpu);
			break;
		default:
			break;
		}
	}
	tss_selector = exit_qualification;

	if (!idt_v || (type != INTR_TYPE_HARD_EXCEPTION &&
		       type != INTR_TYPE_EXT_INTR &&
		       type != INTR_TYPE_NMI_INTR))
		WARN_ON(!skip_emulated_instruction(vcpu));

	/*
	 * TODO: What about debug traps on tss switch?
	 *       Are we supposed to inject them and update dr6?
	 */
	return kvm_task_switch(vcpu, tss_selector,
			       type == INTR_TYPE_SOFT_INTR ? idt_index : -1,
			       reason, has_error_code, error_code);
}

static int handle_ept_violation(struct kvm_vcpu *vcpu)
{
	unsigned long exit_qualification;
	gpa_t gpa;
	u64 error_code;

	exit_qualification = vmx_get_exit_qual(vcpu);

	/*
	 * EPT violation happened while executing iret from NMI,
	 * "blocked by NMI" bit has to be set before next VM entry.
	 * There are errata that may cause this bit to not be set:
	 * AAK134, BY25.
	 */
	if (!(to_vmx(vcpu)->idt_vectoring_info & VECTORING_INFO_VALID_MASK) &&
			enable_vnmi &&
			(exit_qualification & INTR_INFO_UNBLOCK_NMI))
		vmcs_set_bits(GUEST_INTERRUPTIBILITY_INFO, GUEST_INTR_STATE_NMI);

	gpa = vmcs_read64(GUEST_PHYSICAL_ADDRESS);
	trace_kvm_page_fault(gpa, exit_qualification);

	/* Is it a read fault? */
	error_code = (exit_qualification & EPT_VIOLATION_ACC_READ)
		     ? PFERR_USER_MASK : 0;
	/* Is it a write fault? */
	error_code |= (exit_qualification & EPT_VIOLATION_ACC_WRITE)
		      ? PFERR_WRITE_MASK : 0;
	/* Is it a fetch fault? */
	error_code |= (exit_qualification & EPT_VIOLATION_ACC_INSTR)
		      ? PFERR_FETCH_MASK : 0;
	/* ept page table entry is present? */
	error_code |= (exit_qualification &
		       (EPT_VIOLATION_READABLE | EPT_VIOLATION_WRITABLE |
			EPT_VIOLATION_EXECUTABLE))
		      ? PFERR_PRESENT_MASK : 0;

	error_code |= (exit_qualification & EPT_VIOLATION_GVA_TRANSLATED) != 0 ?
	       PFERR_GUEST_FINAL_MASK : PFERR_GUEST_PAGE_MASK;

	vcpu->arch.exit_qualification = exit_qualification;

	/*
	 * Check that the GPA doesn't exceed physical memory limits, as that is
	 * a guest page fault.  We have to emulate the instruction here, because
	 * if the illegal address is that of a paging structure, then
	 * EPT_VIOLATION_ACC_WRITE bit is set.  Alternatively, if supported we
	 * would also use advanced VM-exit information for EPT violations to
	 * reconstruct the page fault error code.
	 */
	if (unlikely(allow_smaller_maxphyaddr && kvm_vcpu_is_illegal_gpa(vcpu, gpa)))
		return kvm_emulate_instruction(vcpu, 0);

	return kvm_mmu_page_fault(vcpu, gpa, error_code, NULL, 0);
}

static int handle_ept_misconfig(struct kvm_vcpu *vcpu)
{
	gpa_t gpa;

	if (!vmx_can_emulate_instruction(vcpu, NULL, 0))
		return 1;

	/*
	 * A nested guest cannot optimize MMIO vmexits, because we have an
	 * nGPA here instead of the required GPA.
	 */
	gpa = vmcs_read64(GUEST_PHYSICAL_ADDRESS);
	if (!is_guest_mode(vcpu) &&
	    !kvm_io_bus_write(vcpu, KVM_FAST_MMIO_BUS, gpa, 0, NULL)) {
		trace_kvm_fast_mmio(gpa);
		return kvm_skip_emulated_instruction(vcpu);
	}

	return kvm_mmu_page_fault(vcpu, gpa, PFERR_RSVD_MASK, NULL, 0);
}

static int handle_nmi_window(struct kvm_vcpu *vcpu)
{
	WARN_ON_ONCE(!enable_vnmi);
	exec_controls_clearbit(to_vmx(vcpu), CPU_BASED_NMI_WINDOW_EXITING);
	++vcpu->stat.nmi_window_exits;
	kvm_make_request(KVM_REQ_EVENT, vcpu);

	return 1;
}

static int handle_invalid_guest_state(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	bool intr_window_requested;
	unsigned count = 130;

	intr_window_requested = exec_controls_get(vmx) &
				CPU_BASED_INTR_WINDOW_EXITING;

	while (vmx->emulation_required && count-- != 0) {
		if (intr_window_requested && !vmx_interrupt_blocked(vcpu))
			return handle_interrupt_window(&vmx->vcpu);

		if (kvm_test_request(KVM_REQ_EVENT, vcpu))
			return 1;

		if (!kvm_emulate_instruction(vcpu, 0))
			return 0;

		if (vmx->emulation_required && !vmx->rmode.vm86_active &&
		    vcpu->arch.exception.pending) {
			vcpu->run->exit_reason = KVM_EXIT_INTERNAL_ERROR;
			vcpu->run->internal.suberror =
						KVM_INTERNAL_ERROR_EMULATION;
			vcpu->run->internal.ndata = 0;
			return 0;
		}

		if (vcpu->arch.halt_request) {
			vcpu->arch.halt_request = 0;
			return kvm_vcpu_halt(vcpu);
		}

		/*
		 * Note, return 1 and not 0, vcpu_run() will invoke
		 * xfer_to_guest_mode() which will create a proper return
		 * code.
		 */
		if (__xfer_to_guest_mode_work_pending())
			return 1;
	}

	return 1;
}

static void grow_ple_window(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	unsigned int old = vmx->ple_window;

	vmx->ple_window = __grow_ple_window(old, ple_window,
					    ple_window_grow,
					    ple_window_max);

	if (vmx->ple_window != old) {
		vmx->ple_window_dirty = true;
		trace_kvm_ple_window_update(vcpu->vcpu_id,
					    vmx->ple_window, old);
	}
}

static void shrink_ple_window(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	unsigned int old = vmx->ple_window;

	vmx->ple_window = __shrink_ple_window(old, ple_window,
					      ple_window_shrink,
					      ple_window);

	if (vmx->ple_window != old) {
		vmx->ple_window_dirty = true;
		trace_kvm_ple_window_update(vcpu->vcpu_id,
					    vmx->ple_window, old);
	}
}

/*
 * Indicate a busy-waiting vcpu in spinlock. We do not enable the PAUSE
 * exiting, so only get here on cpu with PAUSE-Loop-Exiting.
 */
static int handle_pause(struct kvm_vcpu *vcpu)
{
	if (!kvm_pause_in_guest(vcpu->kvm))
		grow_ple_window(vcpu);

	/*
	 * Intel sdm vol3 ch-25.1.3 says: The "PAUSE-loop exiting"
	 * VM-execution control is ignored if CPL > 0. OTOH, KVM
	 * never set PAUSE_EXITING and just set PLE if supported,
	 * so the vcpu must be CPL=0 if it gets a PAUSE exit.
	 */
	kvm_vcpu_on_spin(vcpu, true);
	return kvm_skip_emulated_instruction(vcpu);
}

static int handle_monitor_trap(struct kvm_vcpu *vcpu)
{
	return 1;
}

static int handle_invpcid(struct kvm_vcpu *vcpu)
{
	u32 vmx_instruction_info;
	unsigned long type;
	gva_t gva;
	struct {
		u64 pcid;
		u64 gla;
	} operand;

	if (!guest_cpuid_has(vcpu, X86_FEATURE_INVPCID)) {
		kvm_queue_exception(vcpu, UD_VECTOR);
		return 1;
	}

	vmx_instruction_info = vmcs_read32(VMX_INSTRUCTION_INFO);
	type = kvm_register_read(vcpu, (vmx_instruction_info >> 28) & 0xf);

	if (type > 3) {
		kvm_inject_gp(vcpu, 0);
		return 1;
	}

	/* According to the Intel instruction reference, the memory operand
	 * is read even if it isn't needed (e.g., for type==all)
	 */
	if (get_vmx_mem_address(vcpu, vmx_get_exit_qual(vcpu),
				vmx_instruction_info, false,
				sizeof(operand), &gva))
		return 1;

	return kvm_handle_invpcid(vcpu, type, gva);
}

static int handle_pml_full(struct kvm_vcpu *vcpu)
{
	unsigned long exit_qualification;

	trace_kvm_pml_full(vcpu->vcpu_id);

	exit_qualification = vmx_get_exit_qual(vcpu);

	/*
	 * PML buffer FULL happened while executing iret from NMI,
	 * "blocked by NMI" bit has to be set before next VM entry.
	 */
	if (!(to_vmx(vcpu)->idt_vectoring_info & VECTORING_INFO_VALID_MASK) &&
			enable_vnmi &&
			(exit_qualification & INTR_INFO_UNBLOCK_NMI))
		vmcs_set_bits(GUEST_INTERRUPTIBILITY_INFO,
				GUEST_INTR_STATE_NMI);

	/*
	 * PML buffer already flushed at beginning of VMEXIT. Nothing to do
	 * here.., and there's no userspace involvement needed for PML.
	 */
	return 1;
}

static fastpath_t handle_fastpath_preemption_timer(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);

	if (!vmx->req_immediate_exit &&
	    !unlikely(vmx->loaded_vmcs->hv_timer_soft_disabled)) {
		kvm_lapic_expired_hv_timer(vcpu);
		return EXIT_FASTPATH_REENTER_GUEST;
	}

	return EXIT_FASTPATH_NONE;
}

static int handle_preemption_timer(struct kvm_vcpu *vcpu)
{
	handle_fastpath_preemption_timer(vcpu);
	return 1;
}

/*
 * When nested=0, all VMX instruction VM Exits filter here.  The handlers
 * are overwritten by nested_vmx_setup() when nested=1.
 */
static int handle_vmx_instruction(struct kvm_vcpu *vcpu)
{
	kvm_queue_exception(vcpu, UD_VECTOR);
	return 1;
}

#ifndef CONFIG_X86_SGX_KVM
static int handle_encls(struct kvm_vcpu *vcpu)
{
	/*
	 * SGX virtualization is disabled.  There is no software enable bit for
	 * SGX, so KVM intercepts all ENCLS leafs and injects a #UD to prevent
	 * the guest from executing ENCLS (when SGX is supported by hardware).
	 */
	kvm_queue_exception(vcpu, UD_VECTOR);
	return 1;
}
#endif /* CONFIG_X86_SGX_KVM */

static int handle_bus_lock_vmexit(struct kvm_vcpu *vcpu)
{
	vcpu->run->exit_reason = KVM_EXIT_X86_BUS_LOCK;
	vcpu->run->flags |= KVM_RUN_X86_BUS_LOCK;
	return 0;
}

/*
 * The exit handlers return 1 if the exit was handled fully and guest execution
 * may resume.  Otherwise they set the kvm_run parameter to indicate what needs
 * to be done to userspace and return 0.
 */
static int (*kvm_vmx_exit_handlers[])(struct kvm_vcpu *vcpu) = {
	[EXIT_REASON_EXCEPTION_NMI]           = handle_exception_nmi,
	[EXIT_REASON_EXTERNAL_INTERRUPT]      = handle_external_interrupt,
	[EXIT_REASON_TRIPLE_FAULT]            = handle_triple_fault,
	[EXIT_REASON_NMI_WINDOW]	      = handle_nmi_window,
	[EXIT_REASON_IO_INSTRUCTION]          = handle_io,
	[EXIT_REASON_CR_ACCESS]               = handle_cr,
	[EXIT_REASON_DR_ACCESS]               = handle_dr,
	[EXIT_REASON_CPUID]                   = kvm_emulate_cpuid,
	[EXIT_REASON_MSR_READ]                = kvm_emulate_rdmsr,
	[EXIT_REASON_MSR_WRITE]               = kvm_emulate_wrmsr,
	[EXIT_REASON_INTERRUPT_WINDOW]        = handle_interrupt_window,
	[EXIT_REASON_HLT]                     = kvm_emulate_halt,
	[EXIT_REASON_INVD]		      = kvm_emulate_invd,
	[EXIT_REASON_INVLPG]		      = handle_invlpg,
	[EXIT_REASON_RDPMC]                   = kvm_emulate_rdpmc,
	[EXIT_REASON_VMCALL]                  = kvm_emulate_hypercall,
	[EXIT_REASON_VMCLEAR]		      = handle_vmx_instruction,
	[EXIT_REASON_VMLAUNCH]		      = handle_vmx_instruction,
	[EXIT_REASON_VMPTRLD]		      = handle_vmx_instruction,
	[EXIT_REASON_VMPTRST]		      = handle_vmx_instruction,
	[EXIT_REASON_VMREAD]		      = handle_vmx_instruction,
	[EXIT_REASON_VMRESUME]		      = handle_vmx_instruction,
	[EXIT_REASON_VMWRITE]		      = handle_vmx_instruction,
	[EXIT_REASON_VMOFF]		      = handle_vmx_instruction,
	[EXIT_REASON_VMON]		      = handle_vmx_instruction,
	[EXIT_REASON_TPR_BELOW_THRESHOLD]     = handle_tpr_below_threshold,
	[EXIT_REASON_APIC_ACCESS]             = handle_apic_access,
	[EXIT_REASON_APIC_WRITE]              = handle_apic_write,
	[EXIT_REASON_EOI_INDUCED]             = handle_apic_eoi_induced,
	[EXIT_REASON_WBINVD]                  = kvm_emulate_wbinvd,
	[EXIT_REASON_XSETBV]                  = kvm_emulate_xsetbv,
	[EXIT_REASON_TASK_SWITCH]             = handle_task_switch,
	[EXIT_REASON_MCE_DURING_VMENTRY]      = handle_machine_check,
	[EXIT_REASON_GDTR_IDTR]		      = handle_desc,
	[EXIT_REASON_LDTR_TR]		      = handle_desc,
	[EXIT_REASON_EPT_VIOLATION]	      = handle_ept_violation,
	[EXIT_REASON_EPT_MISCONFIG]           = handle_ept_misconfig,
	[EXIT_REASON_PAUSE_INSTRUCTION]       = handle_pause,
	[EXIT_REASON_MWAIT_INSTRUCTION]	      = kvm_emulate_mwait,
	[EXIT_REASON_MONITOR_TRAP_FLAG]       = handle_monitor_trap,
	[EXIT_REASON_MONITOR_INSTRUCTION]     = kvm_emulate_monitor,
	[EXIT_REASON_INVEPT]                  = handle_vmx_instruction,
	[EXIT_REASON_INVVPID]                 = handle_vmx_instruction,
	[EXIT_REASON_RDRAND]                  = kvm_handle_invalid_op,
	[EXIT_REASON_RDSEED]                  = kvm_handle_invalid_op,
	[EXIT_REASON_PML_FULL]		      = handle_pml_full,
	[EXIT_REASON_INVPCID]                 = handle_invpcid,
	[EXIT_REASON_VMFUNC]		      = handle_vmx_instruction,
	[EXIT_REASON_PREEMPTION_TIMER]	      = handle_preemption_timer,
	[EXIT_REASON_ENCLS]		      = handle_encls,
	[EXIT_REASON_BUS_LOCK]                = handle_bus_lock_vmexit,
};

static const int kvm_vmx_max_exit_handlers =
	ARRAY_SIZE(kvm_vmx_exit_handlers);

static void vmx_get_exit_info(struct kvm_vcpu *vcpu, u64 *info1, u64 *info2,
			      u32 *intr_info, u32 *error_code)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);

	*info1 = vmx_get_exit_qual(vcpu);
	if (!(vmx->exit_reason.failed_vmentry)) {
		*info2 = vmx->idt_vectoring_info;
		*intr_info = vmx_get_intr_info(vcpu);
		if (is_exception_with_error_code(*intr_info))
			*error_code = vmcs_read32(VM_EXIT_INTR_ERROR_CODE);
		else
			*error_code = 0;
	} else {
		*info2 = 0;
		*intr_info = 0;
		*error_code = 0;
	}
}

static void vmx_destroy_pml_buffer(struct vcpu_vmx *vmx)
{
	if (vmx->pml_pg) {
		__free_page(vmx->pml_pg);
		vmx->pml_pg = NULL;
	}
}

static void vmx_flush_pml_buffer(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	u64 *pml_buf;
	u16 pml_idx;

	pml_idx = vmcs_read16(GUEST_PML_INDEX);

	/* Do nothing if PML buffer is empty */
	if (pml_idx == (PML_ENTITY_NUM - 1))
		return;

	/* PML index always points to next available PML buffer entity */
	if (pml_idx >= PML_ENTITY_NUM)
		pml_idx = 0;
	else
		pml_idx++;

	pml_buf = page_address(vmx->pml_pg);
	for (; pml_idx < PML_ENTITY_NUM; pml_idx++) {
		u64 gpa;

		gpa = pml_buf[pml_idx];
		WARN_ON(gpa & (PAGE_SIZE - 1));
		kvm_vcpu_mark_page_dirty(vcpu, gpa >> PAGE_SHIFT);
	}

	/* reset PML index */
	vmcs_write16(GUEST_PML_INDEX, PML_ENTITY_NUM - 1);
}

static void vmx_dump_sel(char *name, uint32_t sel)
{
	pr_err("%s sel=0x%04x, attr=0x%05x, limit=0x%08x, base=0x%016lx\n",
	       name, vmcs_read16(sel),
	       vmcs_read32(sel + GUEST_ES_AR_BYTES - GUEST_ES_SELECTOR),
	       vmcs_read32(sel + GUEST_ES_LIMIT - GUEST_ES_SELECTOR),
	       vmcs_readl(sel + GUEST_ES_BASE - GUEST_ES_SELECTOR));
}

static void vmx_dump_dtsel(char *name, uint32_t limit)
{
	pr_err("%s                           limit=0x%08x, base=0x%016lx\n",
	       name, vmcs_read32(limit),
	       vmcs_readl(limit + GUEST_GDTR_BASE - GUEST_GDTR_LIMIT));
}

static void vmx_dump_msrs(char *name, struct vmx_msrs *m)
{
	unsigned int i;
	struct vmx_msr_entry *e;

	pr_err("MSR %s:\n", name);
	for (i = 0, e = m->val; i < m->nr; ++i, ++e)
		pr_err("  %2d: msr=0x%08x value=0x%016llx\n", i, e->index, e->value);
}

void dump_vmcs(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	u32 vmentry_ctl, vmexit_ctl;
	u32 cpu_based_exec_ctrl, pin_based_exec_ctrl, secondary_exec_control;
	unsigned long cr4;
	int efer_slot;

	if (!dump_invalid_vmcs) {
		pr_warn_ratelimited("set kvm_intel.dump_invalid_vmcs=1 to dump internal KVM state.\n");
		return;
	}

	vmentry_ctl = vmcs_read32(VM_ENTRY_CONTROLS);
	vmexit_ctl = vmcs_read32(VM_EXIT_CONTROLS);
	cpu_based_exec_ctrl = vmcs_read32(CPU_BASED_VM_EXEC_CONTROL);
	pin_based_exec_ctrl = vmcs_read32(PIN_BASED_VM_EXEC_CONTROL);
	cr4 = vmcs_readl(GUEST_CR4);
	secondary_exec_control = 0;
	if (cpu_has_secondary_exec_ctrls())
		secondary_exec_control = vmcs_read32(SECONDARY_VM_EXEC_CONTROL);

	pr_err("*** Guest State ***\n");
	pr_err("CR0: actual=0x%016lx, shadow=0x%016lx, gh_mask=%016lx\n",
	       vmcs_readl(GUEST_CR0), vmcs_readl(CR0_READ_SHADOW),
	       vmcs_readl(CR0_GUEST_HOST_MASK));
	pr_err("CR4: actual=0x%016lx, shadow=0x%016lx, gh_mask=%016lx\n",
	       cr4, vmcs_readl(CR4_READ_SHADOW), vmcs_readl(CR4_GUEST_HOST_MASK));
	pr_err("CR3 = 0x%016lx\n", vmcs_readl(GUEST_CR3));
	if (cpu_has_vmx_ept()) {
		pr_err("PDPTR0 = 0x%016llx  PDPTR1 = 0x%016llx\n",
		       vmcs_read64(GUEST_PDPTR0), vmcs_read64(GUEST_PDPTR1));
		pr_err("PDPTR2 = 0x%016llx  PDPTR3 = 0x%016llx\n",
		       vmcs_read64(GUEST_PDPTR2), vmcs_read64(GUEST_PDPTR3));
	}
	pr_err("RSP = 0x%016lx  RIP = 0x%016lx\n",
	       vmcs_readl(GUEST_RSP), vmcs_readl(GUEST_RIP));
	pr_err("RFLAGS=0x%08lx         DR7 = 0x%016lx\n",
	       vmcs_readl(GUEST_RFLAGS), vmcs_readl(GUEST_DR7));
	pr_err("Sysenter RSP=%016lx CS:RIP=%04x:%016lx\n",
	       vmcs_readl(GUEST_SYSENTER_ESP),
	       vmcs_read32(GUEST_SYSENTER_CS), vmcs_readl(GUEST_SYSENTER_EIP));
	vmx_dump_sel("CS:  ", GUEST_CS_SELECTOR);
	vmx_dump_sel("DS:  ", GUEST_DS_SELECTOR);
	vmx_dump_sel("SS:  ", GUEST_SS_SELECTOR);
	vmx_dump_sel("ES:  ", GUEST_ES_SELECTOR);
	vmx_dump_sel("FS:  ", GUEST_FS_SELECTOR);
	vmx_dump_sel("GS:  ", GUEST_GS_SELECTOR);
	vmx_dump_dtsel("GDTR:", GUEST_GDTR_LIMIT);
	vmx_dump_sel("LDTR:", GUEST_LDTR_SELECTOR);
	vmx_dump_dtsel("IDTR:", GUEST_IDTR_LIMIT);
	vmx_dump_sel("TR:  ", GUEST_TR_SELECTOR);
	efer_slot = vmx_find_loadstore_msr_slot(&vmx->msr_autoload.guest, MSR_EFER);
	if (vmentry_ctl & VM_ENTRY_LOAD_IA32_EFER)
		pr_err("EFER= 0x%016llx\n", vmcs_read64(GUEST_IA32_EFER));
	else if (efer_slot >= 0)
		pr_err("EFER= 0x%016llx (autoload)\n",
		       vmx->msr_autoload.guest.val[efer_slot].value);
	else if (vmentry_ctl & VM_ENTRY_IA32E_MODE)
		pr_err("EFER= 0x%016llx (effective)\n",
		       vcpu->arch.efer | (EFER_LMA | EFER_LME));
	else
		pr_err("EFER= 0x%016llx (effective)\n",
		       vcpu->arch.efer & ~(EFER_LMA | EFER_LME));
	if (vmentry_ctl & VM_ENTRY_LOAD_IA32_PAT)
		pr_err("PAT = 0x%016llx\n", vmcs_read64(GUEST_IA32_PAT));
	pr_err("DebugCtl = 0x%016llx  DebugExceptions = 0x%016lx\n",
	       vmcs_read64(GUEST_IA32_DEBUGCTL),
	       vmcs_readl(GUEST_PENDING_DBG_EXCEPTIONS));
	if (cpu_has_load_perf_global_ctrl() &&
	    vmentry_ctl & VM_ENTRY_LOAD_IA32_PERF_GLOBAL_CTRL)
		pr_err("PerfGlobCtl = 0x%016llx\n",
		       vmcs_read64(GUEST_IA32_PERF_GLOBAL_CTRL));
	if (vmentry_ctl & VM_ENTRY_LOAD_BNDCFGS)
		pr_err("BndCfgS = 0x%016llx\n", vmcs_read64(GUEST_BNDCFGS));
	pr_err("Interruptibility = %08x  ActivityState = %08x\n",
	       vmcs_read32(GUEST_INTERRUPTIBILITY_INFO),
	       vmcs_read32(GUEST_ACTIVITY_STATE));
	if (secondary_exec_control & SECONDARY_EXEC_VIRTUAL_INTR_DELIVERY)
		pr_err("InterruptStatus = %04x\n",
		       vmcs_read16(GUEST_INTR_STATUS));
	if (vmcs_read32(VM_ENTRY_MSR_LOAD_COUNT) > 0)
		vmx_dump_msrs("guest autoload", &vmx->msr_autoload.guest);
	if (vmcs_read32(VM_EXIT_MSR_STORE_COUNT) > 0)
		vmx_dump_msrs("guest autostore", &vmx->msr_autostore.guest);

	pr_err("*** Host State ***\n");
	pr_err("RIP = 0x%016lx  RSP = 0x%016lx\n",
	       vmcs_readl(HOST_RIP), vmcs_readl(HOST_RSP));
	pr_err("CS=%04x SS=%04x DS=%04x ES=%04x FS=%04x GS=%04x TR=%04x\n",
	       vmcs_read16(HOST_CS_SELECTOR), vmcs_read16(HOST_SS_SELECTOR),
	       vmcs_read16(HOST_DS_SELECTOR), vmcs_read16(HOST_ES_SELECTOR),
	       vmcs_read16(HOST_FS_SELECTOR), vmcs_read16(HOST_GS_SELECTOR),
	       vmcs_read16(HOST_TR_SELECTOR));
	pr_err("FSBase=%016lx GSBase=%016lx TRBase=%016lx\n",
	       vmcs_readl(HOST_FS_BASE), vmcs_readl(HOST_GS_BASE),
	       vmcs_readl(HOST_TR_BASE));
	pr_err("GDTBase=%016lx IDTBase=%016lx\n",
	       vmcs_readl(HOST_GDTR_BASE), vmcs_readl(HOST_IDTR_BASE));
	pr_err("CR0=%016lx CR3=%016lx CR4=%016lx\n",
	       vmcs_readl(HOST_CR0), vmcs_readl(HOST_CR3),
	       vmcs_readl(HOST_CR4));
	pr_err("Sysenter RSP=%016lx CS:RIP=%04x:%016lx\n",
	       vmcs_readl(HOST_IA32_SYSENTER_ESP),
	       vmcs_read32(HOST_IA32_SYSENTER_CS),
	       vmcs_readl(HOST_IA32_SYSENTER_EIP));
	if (vmexit_ctl & VM_EXIT_LOAD_IA32_EFER)
		pr_err("EFER= 0x%016llx\n", vmcs_read64(HOST_IA32_EFER));
	if (vmexit_ctl & VM_EXIT_LOAD_IA32_PAT)
		pr_err("PAT = 0x%016llx\n", vmcs_read64(HOST_IA32_PAT));
	if (cpu_has_load_perf_global_ctrl() &&
	    vmexit_ctl & VM_EXIT_LOAD_IA32_PERF_GLOBAL_CTRL)
		pr_err("PerfGlobCtl = 0x%016llx\n",
		       vmcs_read64(HOST_IA32_PERF_GLOBAL_CTRL));
	if (vmcs_read32(VM_EXIT_MSR_LOAD_COUNT) > 0)
		vmx_dump_msrs("host autoload", &vmx->msr_autoload.host);

	pr_err("*** Control State ***\n");
	pr_err("PinBased=%08x CPUBased=%08x SecondaryExec=%08x\n",
	       pin_based_exec_ctrl, cpu_based_exec_ctrl, secondary_exec_control);
	pr_err("EntryControls=%08x ExitControls=%08x\n", vmentry_ctl, vmexit_ctl);
	pr_err("ExceptionBitmap=%08x PFECmask=%08x PFECmatch=%08x\n",
	       vmcs_read32(EXCEPTION_BITMAP),
	       vmcs_read32(PAGE_FAULT_ERROR_CODE_MASK),
	       vmcs_read32(PAGE_FAULT_ERROR_CODE_MATCH));
	pr_err("VMEntry: intr_info=%08x errcode=%08x ilen=%08x\n",
	       vmcs_read32(VM_ENTRY_INTR_INFO_FIELD),
	       vmcs_read32(VM_ENTRY_EXCEPTION_ERROR_CODE),
	       vmcs_read32(VM_ENTRY_INSTRUCTION_LEN));
	pr_err("VMExit: intr_info=%08x errcode=%08x ilen=%08x\n",
	       vmcs_read32(VM_EXIT_INTR_INFO),
	       vmcs_read32(VM_EXIT_INTR_ERROR_CODE),
	       vmcs_read32(VM_EXIT_INSTRUCTION_LEN));
	pr_err("        reason=%08x qualification=%016lx\n",
	       vmcs_read32(VM_EXIT_REASON), vmcs_readl(EXIT_QUALIFICATION));
	pr_err("IDTVectoring: info=%08x errcode=%08x\n",
	       vmcs_read32(IDT_VECTORING_INFO_FIELD),
	       vmcs_read32(IDT_VECTORING_ERROR_CODE));
	pr_err("TSC Offset = 0x%016llx\n", vmcs_read64(TSC_OFFSET));
	if (secondary_exec_control & SECONDARY_EXEC_TSC_SCALING)
		pr_err("TSC Multiplier = 0x%016llx\n",
		       vmcs_read64(TSC_MULTIPLIER));
	if (cpu_based_exec_ctrl & CPU_BASED_TPR_SHADOW) {
		if (secondary_exec_control & SECONDARY_EXEC_VIRTUAL_INTR_DELIVERY) {
			u16 status = vmcs_read16(GUEST_INTR_STATUS);
			pr_err("SVI|RVI = %02x|%02x ", status >> 8, status & 0xff);
		}
		pr_cont("TPR Threshold = 0x%02x\n", vmcs_read32(TPR_THRESHOLD));
		if (secondary_exec_control & SECONDARY_EXEC_VIRTUALIZE_APIC_ACCESSES)
			pr_err("APIC-access addr = 0x%016llx ", vmcs_read64(APIC_ACCESS_ADDR));
		pr_cont("virt-APIC addr = 0x%016llx\n", vmcs_read64(VIRTUAL_APIC_PAGE_ADDR));
	}
	if (pin_based_exec_ctrl & PIN_BASED_POSTED_INTR)
		pr_err("PostedIntrVec = 0x%02x\n", vmcs_read16(POSTED_INTR_NV));
	if ((secondary_exec_control & SECONDARY_EXEC_ENABLE_EPT))
		pr_err("EPT pointer = 0x%016llx\n", vmcs_read64(EPT_POINTER));
	if (secondary_exec_control & SECONDARY_EXEC_PAUSE_LOOP_EXITING)
		pr_err("PLE Gap=%08x Window=%08x\n",
		       vmcs_read32(PLE_GAP), vmcs_read32(PLE_WINDOW));
	if (secondary_exec_control & SECONDARY_EXEC_ENABLE_VPID)
		pr_err("Virtual processor ID = 0x%04x\n",
		       vmcs_read16(VIRTUAL_PROCESSOR_ID));
}

/*
 * The guest has exited.  See if we can fix it or if we need userspace
 * assistance.
 */
static int __vmx_handle_exit(struct kvm_vcpu *vcpu, fastpath_t exit_fastpath)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	union vmx_exit_reason exit_reason = vmx->exit_reason;
	u32 vectoring_info = vmx->idt_vectoring_info;
	u16 exit_handler_index;

	/*
	 * Flush logged GPAs PML buffer, this will make dirty_bitmap more
	 * updated. Another good is, in kvm_vm_ioctl_get_dirty_log, before
	 * querying dirty_bitmap, we only need to kick all vcpus out of guest
	 * mode as if vcpus is in root mode, the PML buffer must has been
	 * flushed already.  Note, PML is never enabled in hardware while
	 * running L2.
	 */
	if (enable_pml && !is_guest_mode(vcpu))
		vmx_flush_pml_buffer(vcpu);

	/*
	 * We should never reach this point with a pending nested VM-Enter, and
	 * more specifically emulation of L2 due to invalid guest state (see
	 * below) should never happen as that means we incorrectly allowed a
	 * nested VM-Enter with an invalid vmcs12.
	 */
	WARN_ON_ONCE(vmx->nested.nested_run_pending);

	/* If guest state is invalid, start emulating */
	if (vmx->emulation_required)
		return handle_invalid_guest_state(vcpu);

	if (is_guest_mode(vcpu)) {
		/*
		 * PML is never enabled when running L2, bail immediately if a
		 * PML full exit occurs as something is horribly wrong.
		 */
		if (exit_reason.basic == EXIT_REASON_PML_FULL)
			goto unexpected_vmexit;

		/*
		 * The host physical addresses of some pages of guest memory
		 * are loaded into the vmcs02 (e.g. vmcs12's Virtual APIC
		 * Page). The CPU may write to these pages via their host
		 * physical address while L2 is running, bypassing any
		 * address-translation-based dirty tracking (e.g. EPT write
		 * protection).
		 *
		 * Mark them dirty on every exit from L2 to prevent them from
		 * getting out of sync with dirty tracking.
		 */
		nested_mark_vmcs12_pages_dirty(vcpu);

		if (nested_vmx_reflect_vmexit(vcpu))
			return 1;
	}

	if (exit_reason.failed_vmentry) {
		dump_vmcs(vcpu);
		vcpu->run->exit_reason = KVM_EXIT_FAIL_ENTRY;
		vcpu->run->fail_entry.hardware_entry_failure_reason
			= exit_reason.full;
		vcpu->run->fail_entry.cpu = vcpu->arch.last_vmentry_cpu;
		return 0;
	}

	if (unlikely(vmx->fail)) {
		dump_vmcs(vcpu);
		vcpu->run->exit_reason = KVM_EXIT_FAIL_ENTRY;
		vcpu->run->fail_entry.hardware_entry_failure_reason
			= vmcs_read32(VM_INSTRUCTION_ERROR);
		vcpu->run->fail_entry.cpu = vcpu->arch.last_vmentry_cpu;
		return 0;
	}

	/*
	 * Note:
	 * Do not try to fix EXIT_REASON_EPT_MISCONFIG if it caused by
	 * delivery event since it indicates guest is accessing MMIO.
	 * The vm-exit can be triggered again after return to guest that
	 * will cause infinite loop.
	 */
	if ((vectoring_info & VECTORING_INFO_VALID_MASK) &&
	    (exit_reason.basic != EXIT_REASON_EXCEPTION_NMI &&
	     exit_reason.basic != EXIT_REASON_EPT_VIOLATION &&
	     exit_reason.basic != EXIT_REASON_PML_FULL &&
	     exit_reason.basic != EXIT_REASON_APIC_ACCESS &&
	     exit_reason.basic != EXIT_REASON_TASK_SWITCH)) {
		int ndata = 3;

		vcpu->run->exit_reason = KVM_EXIT_INTERNAL_ERROR;
		vcpu->run->internal.suberror = KVM_INTERNAL_ERROR_DELIVERY_EV;
		vcpu->run->internal.data[0] = vectoring_info;
		vcpu->run->internal.data[1] = exit_reason.full;
		vcpu->run->internal.data[2] = vcpu->arch.exit_qualification;
		if (exit_reason.basic == EXIT_REASON_EPT_MISCONFIG) {
			vcpu->run->internal.data[ndata++] =
				vmcs_read64(GUEST_PHYSICAL_ADDRESS);
		}
		vcpu->run->internal.data[ndata++] = vcpu->arch.last_vmentry_cpu;
		vcpu->run->internal.ndata = ndata;
		return 0;
	}

	if (unlikely(!enable_vnmi &&
		     vmx->loaded_vmcs->soft_vnmi_blocked)) {
		if (!vmx_interrupt_blocked(vcpu)) {
			vmx->loaded_vmcs->soft_vnmi_blocked = 0;
		} else if (vmx->loaded_vmcs->vnmi_blocked_time > 1000000000LL &&
			   vcpu->arch.nmi_pending) {
			/*
			 * This CPU don't support us in finding the end of an
			 * NMI-blocked window if the guest runs with IRQs
			 * disabled. So we pull the trigger after 1 s of
			 * futile waiting, but inform the user about this.
			 */
			printk(KERN_WARNING "%s: Breaking out of NMI-blocked "
			       "state on VCPU %d after 1 s timeout\n",
			       __func__, vcpu->vcpu_id);
			vmx->loaded_vmcs->soft_vnmi_blocked = 0;
		}
	}

	if (exit_fastpath != EXIT_FASTPATH_NONE)
		return 1;

	if (exit_reason.basic >= kvm_vmx_max_exit_handlers)
		goto unexpected_vmexit;
#ifdef CONFIG_RETPOLINE
	if (exit_reason.basic == EXIT_REASON_MSR_WRITE)
		return kvm_emulate_wrmsr(vcpu);
	else if (exit_reason.basic == EXIT_REASON_PREEMPTION_TIMER)
		return handle_preemption_timer(vcpu);
	else if (exit_reason.basic == EXIT_REASON_INTERRUPT_WINDOW)
		return handle_interrupt_window(vcpu);
	else if (exit_reason.basic == EXIT_REASON_EXTERNAL_INTERRUPT)
		return handle_external_interrupt(vcpu);
	else if (exit_reason.basic == EXIT_REASON_HLT)
		return kvm_emulate_halt(vcpu);
	else if (exit_reason.basic == EXIT_REASON_EPT_MISCONFIG)
		return handle_ept_misconfig(vcpu);
#endif

	exit_handler_index = array_index_nospec((u16)exit_reason.basic,
						kvm_vmx_max_exit_handlers);
	if (!kvm_vmx_exit_handlers[exit_handler_index])
		goto unexpected_vmexit;

	return kvm_vmx_exit_handlers[exit_handler_index](vcpu);

unexpected_vmexit:
	vcpu_unimpl(vcpu, "vmx: unexpected exit reason 0x%x\n",
		    exit_reason.full);
	dump_vmcs(vcpu);
	vcpu->run->exit_reason = KVM_EXIT_INTERNAL_ERROR;
	vcpu->run->internal.suberror =
			KVM_INTERNAL_ERROR_UNEXPECTED_EXIT_REASON;
	vcpu->run->internal.ndata = 2;
	vcpu->run->internal.data[0] = exit_reason.full;
	vcpu->run->internal.data[1] = vcpu->arch.last_vmentry_cpu;
	return 0;
}

static int vmx_handle_exit(struct kvm_vcpu *vcpu, fastpath_t exit_fastpath)
{
	int ret = __vmx_handle_exit(vcpu, exit_fastpath);

	/*
	 * Even when current exit reason is handled by KVM internally, we
	 * still need to exit to user space when bus lock detected to inform
	 * that there is a bus lock in guest.
	 */
	if (to_vmx(vcpu)->exit_reason.bus_lock_detected) {
		if (ret > 0)
			vcpu->run->exit_reason = KVM_EXIT_X86_BUS_LOCK;

		vcpu->run->flags |= KVM_RUN_X86_BUS_LOCK;
		return 0;
	}
	return ret;
}

/*
 * Software based L1D cache flush which is used when microcode providing
 * the cache control MSR is not loaded.
 *
 * The L1D cache is 32 KiB on Nehalem and later microarchitectures, but to
 * flush it is required to read in 64 KiB because the replacement algorithm
 * is not exactly LRU. This could be sized at runtime via topology
 * information but as all relevant affected CPUs have 32KiB L1D cache size
 * there is no point in doing so.
 */
static noinstr void vmx_l1d_flush(struct kvm_vcpu *vcpu)
{
	int size = PAGE_SIZE << L1D_CACHE_ORDER;

	/*
	 * This code is only executed when the the flush mode is 'cond' or
	 * 'always'
	 */
	if (static_branch_likely(&vmx_l1d_flush_cond)) {
		bool flush_l1d;

		/*
		 * Clear the per-vcpu flush bit, it gets set again
		 * either from vcpu_run() or from one of the unsafe
		 * VMEXIT handlers.
		 */
		flush_l1d = vcpu->arch.l1tf_flush_l1d;
		vcpu->arch.l1tf_flush_l1d = false;

		/*
		 * Clear the per-cpu flush bit, it gets set again from
		 * the interrupt handlers.
		 */
		flush_l1d |= kvm_get_cpu_l1tf_flush_l1d();
		kvm_clear_cpu_l1tf_flush_l1d();

		if (!flush_l1d)
			return;
	}

	vcpu->stat.l1d_flush++;

	if (static_cpu_has(X86_FEATURE_FLUSH_L1D)) {
		native_wrmsrl(MSR_IA32_FLUSH_CMD, L1D_FLUSH);
		return;
	}

	asm volatile(
		/* First ensure the pages are in the TLB */
		"xorl	%%eax, %%eax\n"
		".Lpopulate_tlb:\n\t"
		"movzbl	(%[flush_pages], %%" _ASM_AX "), %%ecx\n\t"
		"addl	$4096, %%eax\n\t"
		"cmpl	%%eax, %[size]\n\t"
		"jne	.Lpopulate_tlb\n\t"
		"xorl	%%eax, %%eax\n\t"
		"cpuid\n\t"
		/* Now fill the cache */
		"xorl	%%eax, %%eax\n"
		".Lfill_cache:\n"
		"movzbl	(%[flush_pages], %%" _ASM_AX "), %%ecx\n\t"
		"addl	$64, %%eax\n\t"
		"cmpl	%%eax, %[size]\n\t"
		"jne	.Lfill_cache\n\t"
		"lfence\n"
		:: [flush_pages] "r" (vmx_l1d_flush_pages),
		    [size] "r" (size)
		: "eax", "ebx", "ecx", "edx");
}

static void vmx_update_cr8_intercept(struct kvm_vcpu *vcpu, int tpr, int irr)
{
	struct vmcs12 *vmcs12 = get_vmcs12(vcpu);
	int tpr_threshold;

	if (is_guest_mode(vcpu) &&
		nested_cpu_has(vmcs12, CPU_BASED_TPR_SHADOW))
		return;

	tpr_threshold = (irr == -1 || tpr < irr) ? 0 : irr;
	if (is_guest_mode(vcpu))
		to_vmx(vcpu)->nested.l1_tpr_threshold = tpr_threshold;
	else
		vmcs_write32(TPR_THRESHOLD, tpr_threshold);
}

void vmx_set_virtual_apic_mode(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	u32 sec_exec_control;

	if (!lapic_in_kernel(vcpu))
		return;

	if (!flexpriority_enabled &&
	    !cpu_has_vmx_virtualize_x2apic_mode())
		return;

	/* Postpone execution until vmcs01 is the current VMCS. */
	if (is_guest_mode(vcpu)) {
		vmx->nested.change_vmcs01_virtual_apic_mode = true;
		return;
	}

	sec_exec_control = secondary_exec_controls_get(vmx);
	sec_exec_control &= ~(SECONDARY_EXEC_VIRTUALIZE_APIC_ACCESSES |
			      SECONDARY_EXEC_VIRTUALIZE_X2APIC_MODE);

	switch (kvm_get_apic_mode(vcpu)) {
	case LAPIC_MODE_INVALID:
		WARN_ONCE(true, "Invalid local APIC state");
		break;
	case LAPIC_MODE_DISABLED:
		break;
	case LAPIC_MODE_XAPIC:
		if (flexpriority_enabled) {
			sec_exec_control |=
				SECONDARY_EXEC_VIRTUALIZE_APIC_ACCESSES;
			kvm_make_request(KVM_REQ_APIC_PAGE_RELOAD, vcpu);

			/*
			 * Flush the TLB, reloading the APIC access page will
			 * only do so if its physical address has changed, but
			 * the guest may have inserted a non-APIC mapping into
			 * the TLB while the APIC access page was disabled.
			 */
			kvm_make_request(KVM_REQ_TLB_FLUSH_CURRENT, vcpu);
		}
		break;
	case LAPIC_MODE_X2APIC:
		if (cpu_has_vmx_virtualize_x2apic_mode())
			sec_exec_control |=
				SECONDARY_EXEC_VIRTUALIZE_X2APIC_MODE;
		break;
	}
	secondary_exec_controls_set(vmx, sec_exec_control);

	vmx_update_msr_bitmap(vcpu);
}

static void vmx_set_apic_access_page_addr(struct kvm_vcpu *vcpu)
{
	struct page *page;

	/* Defer reload until vmcs01 is the current VMCS. */
	if (is_guest_mode(vcpu)) {
		to_vmx(vcpu)->nested.reload_vmcs01_apic_access_page = true;
		return;
	}

	if (!(secondary_exec_controls_get(to_vmx(vcpu)) &
	    SECONDARY_EXEC_VIRTUALIZE_APIC_ACCESSES))
		return;

	page = gfn_to_page(vcpu->kvm, APIC_DEFAULT_PHYS_BASE >> PAGE_SHIFT);
	if (is_error_page(page))
		return;

	vmcs_write64(APIC_ACCESS_ADDR, page_to_phys(page));
	vmx_flush_tlb_current(vcpu);

	/*
	 * Do not pin apic access page in memory, the MMU notifier
	 * will call us again if it is migrated or swapped out.
	 */
	put_page(page);
}

static void vmx_hwapic_isr_update(struct kvm_vcpu *vcpu, int max_isr)
{
	u16 status;
	u8 old;

	if (max_isr == -1)
		max_isr = 0;

	status = vmcs_read16(GUEST_INTR_STATUS);
	old = status >> 8;
	if (max_isr != old) {
		status &= 0xff;
		status |= max_isr << 8;
		vmcs_write16(GUEST_INTR_STATUS, status);
	}
}

static void vmx_set_rvi(int vector)
{
	u16 status;
	u8 old;

	if (vector == -1)
		vector = 0;

	status = vmcs_read16(GUEST_INTR_STATUS);
	old = (u8)status & 0xff;
	if ((u8)vector != old) {
		status &= ~0xff;
		status |= (u8)vector;
		vmcs_write16(GUEST_INTR_STATUS, status);
	}
}

static void vmx_hwapic_irr_update(struct kvm_vcpu *vcpu, int max_irr)
{
	/*
	 * When running L2, updating RVI is only relevant when
	 * vmcs12 virtual-interrupt-delivery enabled.
	 * However, it can be enabled only when L1 also
	 * intercepts external-interrupts and in that case
	 * we should not update vmcs02 RVI but instead intercept
	 * interrupt. Therefore, do nothing when running L2.
	 */
	if (!is_guest_mode(vcpu))
		vmx_set_rvi(max_irr);
}

static int vmx_sync_pir_to_irr(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	int max_irr;
	bool max_irr_updated;

	WARN_ON(!vcpu->arch.apicv_active);
	if (pi_test_on(&vmx->pi_desc)) {
		pi_clear_on(&vmx->pi_desc);
		/*
		 * IOMMU can write to PID.ON, so the barrier matters even on UP.
		 * But on x86 this is just a compiler barrier anyway.
		 */
		smp_mb__after_atomic();
		max_irr_updated =
			kvm_apic_update_irr(vcpu, vmx->pi_desc.pir, &max_irr);

		/*
		 * If we are running L2 and L1 has a new pending interrupt
		 * which can be injected, we should re-evaluate
		 * what should be done with this new L1 interrupt.
		 * If L1 intercepts external-interrupts, we should
		 * exit from L2 to L1. Otherwise, interrupt should be
		 * delivered directly to L2.
		 */
		if (is_guest_mode(vcpu) && max_irr_updated) {
			if (nested_exit_on_intr(vcpu))
				kvm_vcpu_exiting_guest_mode(vcpu);
			else
				kvm_make_request(KVM_REQ_EVENT, vcpu);
		}
	} else {
		max_irr = kvm_lapic_find_highest_irr(vcpu);
	}
	vmx_hwapic_irr_update(vcpu, max_irr);
	return max_irr;
}

static void vmx_load_eoi_exitmap(struct kvm_vcpu *vcpu, u64 *eoi_exit_bitmap)
{
	if (!kvm_vcpu_apicv_active(vcpu))
		return;

	vmcs_write64(EOI_EXIT_BITMAP0, eoi_exit_bitmap[0]);
	vmcs_write64(EOI_EXIT_BITMAP1, eoi_exit_bitmap[1]);
	vmcs_write64(EOI_EXIT_BITMAP2, eoi_exit_bitmap[2]);
	vmcs_write64(EOI_EXIT_BITMAP3, eoi_exit_bitmap[3]);
}

static void vmx_apicv_post_state_restore(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);

	pi_clear_on(&vmx->pi_desc);
	memset(vmx->pi_desc.pir, 0, sizeof(vmx->pi_desc.pir));
}

void vmx_do_interrupt_nmi_irqoff(unsigned long entry);

static void handle_interrupt_nmi_irqoff(struct kvm_vcpu *vcpu,
					unsigned long entry)
{
	kvm_before_interrupt(vcpu);
	vmx_do_interrupt_nmi_irqoff(entry);
	kvm_after_interrupt(vcpu);
}

static void handle_exception_nmi_irqoff(struct vcpu_vmx *vmx)
{
	const unsigned long nmi_entry = (unsigned long)asm_exc_nmi_noist;
	u32 intr_info = vmx_get_intr_info(&vmx->vcpu);

	/* if exit due to PF check for async PF */
	if (is_page_fault(intr_info))
		vmx->vcpu.arch.apf.host_apf_flags = kvm_read_and_reset_apf_flags();
	/* Handle machine checks before interrupts are enabled */
	else if (is_machine_check(intr_info))
		kvm_machine_check();
	/* We need to handle NMIs before interrupts are enabled */
	else if (is_nmi(intr_info))
		handle_interrupt_nmi_irqoff(&vmx->vcpu, nmi_entry);
}

static void handle_external_interrupt_irqoff(struct kvm_vcpu *vcpu)
{
	u32 intr_info = vmx_get_intr_info(vcpu);
	unsigned int vector = intr_info & INTR_INFO_VECTOR_MASK;
	gate_desc *desc = (gate_desc *)host_idt_base + vector;

	if (WARN_ONCE(!is_external_intr(intr_info),
	    "KVM: unexpected VM-Exit interrupt info: 0x%x", intr_info))
		return;

	handle_interrupt_nmi_irqoff(vcpu, gate_offset(desc));
}

static void vmx_handle_exit_irqoff(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);

	if (vmx->exit_reason.basic == EXIT_REASON_EXTERNAL_INTERRUPT)
		handle_external_interrupt_irqoff(vcpu);
	else if (vmx->exit_reason.basic == EXIT_REASON_EXCEPTION_NMI)
		handle_exception_nmi_irqoff(vmx);
}

/*
 * The kvm parameter can be NULL (module initialization, or invocation before
 * VM creation). Be sure to check the kvm parameter before using it.
 */
static bool vmx_has_emulated_msr(struct kvm *kvm, u32 index)
{
	switch (index) {
	case MSR_IA32_SMBASE:
		/*
		 * We cannot do SMM unless we can run the guest in big
		 * real mode.
		 */
		return enable_unrestricted_guest || emulate_invalid_guest_state;
	case MSR_IA32_VMX_BASIC ... MSR_IA32_VMX_VMFUNC:
		return nested;
	case MSR_AMD64_VIRT_SPEC_CTRL:
		/* This is AMD only.  */
		return false;
	default:
		return true;
	}
}

static void vmx_recover_nmi_blocking(struct vcpu_vmx *vmx)
{
	u32 exit_intr_info;
	bool unblock_nmi;
	u8 vector;
	bool idtv_info_valid;

	idtv_info_valid = vmx->idt_vectoring_info & VECTORING_INFO_VALID_MASK;

	if (enable_vnmi) {
		if (vmx->loaded_vmcs->nmi_known_unmasked)
			return;

		exit_intr_info = vmx_get_intr_info(&vmx->vcpu);
		unblock_nmi = (exit_intr_info & INTR_INFO_UNBLOCK_NMI) != 0;
		vector = exit_intr_info & INTR_INFO_VECTOR_MASK;
		/*
		 * SDM 3: 27.7.1.2 (September 2008)
		 * Re-set bit "block by NMI" before VM entry if vmexit caused by
		 * a guest IRET fault.
		 * SDM 3: 23.2.2 (September 2008)
		 * Bit 12 is undefined in any of the following cases:
		 *  If the VM exit sets the valid bit in the IDT-vectoring
		 *   information field.
		 *  If the VM exit is due to a double fault.
		 */
		if ((exit_intr_info & INTR_INFO_VALID_MASK) && unblock_nmi &&
		    vector != DF_VECTOR && !idtv_info_valid)
			vmcs_set_bits(GUEST_INTERRUPTIBILITY_INFO,
				      GUEST_INTR_STATE_NMI);
		else
			vmx->loaded_vmcs->nmi_known_unmasked =
				!(vmcs_read32(GUEST_INTERRUPTIBILITY_INFO)
				  & GUEST_INTR_STATE_NMI);
	} else if (unlikely(vmx->loaded_vmcs->soft_vnmi_blocked))
		vmx->loaded_vmcs->vnmi_blocked_time +=
			ktime_to_ns(ktime_sub(ktime_get(),
					      vmx->loaded_vmcs->entry_time));
}

static void __vmx_complete_interrupts(struct kvm_vcpu *vcpu,
				      u32 idt_vectoring_info,
				      int instr_len_field,
				      int error_code_field)
{
	u8 vector;
	int type;
	bool idtv_info_valid;

	idtv_info_valid = idt_vectoring_info & VECTORING_INFO_VALID_MASK;

	vcpu->arch.nmi_injected = false;
	kvm_clear_exception_queue(vcpu);
	kvm_clear_interrupt_queue(vcpu);

	if (!idtv_info_valid)
		return;

	kvm_make_request(KVM_REQ_EVENT, vcpu);

	vector = idt_vectoring_info & VECTORING_INFO_VECTOR_MASK;
	type = idt_vectoring_info & VECTORING_INFO_TYPE_MASK;

	switch (type) {
	case INTR_TYPE_NMI_INTR:
		vcpu->arch.nmi_injected = true;
		/*
		 * SDM 3: 27.7.1.2 (September 2008)
		 * Clear bit "block by NMI" before VM entry if a NMI
		 * delivery faulted.
		 */
		vmx_set_nmi_mask(vcpu, false);
		break;
	case INTR_TYPE_SOFT_EXCEPTION:
		vcpu->arch.event_exit_inst_len = vmcs_read32(instr_len_field);
		fallthrough;
	case INTR_TYPE_HARD_EXCEPTION:
		if (idt_vectoring_info & VECTORING_INFO_DELIVER_CODE_MASK) {
			u32 err = vmcs_read32(error_code_field);
			kvm_requeue_exception_e(vcpu, vector, err);
		} else
			kvm_requeue_exception(vcpu, vector);
		break;
	case INTR_TYPE_SOFT_INTR:
		vcpu->arch.event_exit_inst_len = vmcs_read32(instr_len_field);
		fallthrough;
	case INTR_TYPE_EXT_INTR:
		kvm_queue_interrupt(vcpu, vector, type == INTR_TYPE_SOFT_INTR);
		break;
	default:
		break;
	}
}

static void vmx_complete_interrupts(struct vcpu_vmx *vmx)
{
	__vmx_complete_interrupts(&vmx->vcpu, vmx->idt_vectoring_info,
				  VM_EXIT_INSTRUCTION_LEN,
				  IDT_VECTORING_ERROR_CODE);
}

static void vmx_cancel_injection(struct kvm_vcpu *vcpu)
{
	__vmx_complete_interrupts(vcpu,
				  vmcs_read32(VM_ENTRY_INTR_INFO_FIELD),
				  VM_ENTRY_INSTRUCTION_LEN,
				  VM_ENTRY_EXCEPTION_ERROR_CODE);

	vmcs_write32(VM_ENTRY_INTR_INFO_FIELD, 0);
}

static void atomic_switch_perf_msrs(struct vcpu_vmx *vmx)
{
	int i, nr_msrs;
	struct perf_guest_switch_msr *msrs;

	/* Note, nr_msrs may be garbage if perf_guest_get_msrs() returns NULL. */
	msrs = perf_guest_get_msrs(&nr_msrs);
	if (!msrs)
		return;

	for (i = 0; i < nr_msrs; i++)
		if (msrs[i].host == msrs[i].guest)
			clear_atomic_switch_msr(vmx, msrs[i].msr);
		else
			add_atomic_switch_msr(vmx, msrs[i].msr, msrs[i].guest,
					msrs[i].host, false);
}

static void vmx_update_hv_timer(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	u64 tscl;
	u32 delta_tsc;

	if (vmx->req_immediate_exit) {
		vmcs_write32(VMX_PREEMPTION_TIMER_VALUE, 0);
		vmx->loaded_vmcs->hv_timer_soft_disabled = false;
	} else if (vmx->hv_deadline_tsc != -1) {
		tscl = rdtsc();
		if (vmx->hv_deadline_tsc > tscl)
			/* set_hv_timer ensures the delta fits in 32-bits */
			delta_tsc = (u32)((vmx->hv_deadline_tsc - tscl) >>
				cpu_preemption_timer_multi);
		else
			delta_tsc = 0;

		vmcs_write32(VMX_PREEMPTION_TIMER_VALUE, delta_tsc);
		vmx->loaded_vmcs->hv_timer_soft_disabled = false;
	} else if (!vmx->loaded_vmcs->hv_timer_soft_disabled) {
		vmcs_write32(VMX_PREEMPTION_TIMER_VALUE, -1);
		vmx->loaded_vmcs->hv_timer_soft_disabled = true;
	}
}

void noinstr vmx_update_host_rsp(struct vcpu_vmx *vmx, unsigned long host_rsp)
{
	if (unlikely(host_rsp != vmx->loaded_vmcs->host_state.rsp)) {
		vmx->loaded_vmcs->host_state.rsp = host_rsp;
		vmcs_writel(HOST_RSP, host_rsp);
	}
}

static fastpath_t vmx_exit_handlers_fastpath(struct kvm_vcpu *vcpu)
{
	switch (to_vmx(vcpu)->exit_reason.basic) {
	case EXIT_REASON_MSR_WRITE:
		return handle_fastpath_set_msr_irqoff(vcpu);
	case EXIT_REASON_PREEMPTION_TIMER:
		return handle_fastpath_preemption_timer(vcpu);
	default:
		return EXIT_FASTPATH_NONE;
	}
}

static noinstr void vmx_vcpu_enter_exit(struct kvm_vcpu *vcpu,
					struct vcpu_vmx *vmx)
{
	kvm_guest_enter_irqoff();

	/* L1D Flush includes CPU buffer clear to mitigate MDS */
	if (static_branch_unlikely(&vmx_l1d_should_flush))
		vmx_l1d_flush(vcpu);
	else if (static_branch_unlikely(&mds_user_clear))
		mds_clear_cpu_buffers();

	if (vcpu->arch.cr2 != native_read_cr2())
		native_write_cr2(vcpu->arch.cr2);

	vmx->fail = __vmx_vcpu_run(vmx, (unsigned long *)&vcpu->arch.regs,
				   vmx->loaded_vmcs->launched);

	vcpu->arch.cr2 = native_read_cr2();

	kvm_guest_exit_irqoff();
}

static fastpath_t vmx_vcpu_run(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	unsigned long cr3, cr4;

	/* Record the guest's net vcpu time for enforced NMI injections. */
	if (unlikely(!enable_vnmi &&
		     vmx->loaded_vmcs->soft_vnmi_blocked))
		vmx->loaded_vmcs->entry_time = ktime_get();

	/* Don't enter VMX if guest state is invalid, let the exit handler
	   start emulation until we arrive back to a valid state */
	if (vmx->emulation_required)
		return EXIT_FASTPATH_NONE;

	trace_kvm_entry(vcpu);

	if (vmx->ple_window_dirty) {
		vmx->ple_window_dirty = false;
		vmcs_write32(PLE_WINDOW, vmx->ple_window);
	}

	/*
	 * We did this in prepare_switch_to_guest, because it needs to
	 * be within srcu_read_lock.
	 */
	WARN_ON_ONCE(vmx->nested.need_vmcs12_to_shadow_sync);

	if (kvm_register_is_dirty(vcpu, VCPU_REGS_RSP))
		vmcs_writel(GUEST_RSP, vcpu->arch.regs[VCPU_REGS_RSP]);
	if (kvm_register_is_dirty(vcpu, VCPU_REGS_RIP))
		vmcs_writel(GUEST_RIP, vcpu->arch.regs[VCPU_REGS_RIP]);

	cr3 = __get_current_cr3_fast();
	if (unlikely(cr3 != vmx->loaded_vmcs->host_state.cr3)) {
		vmcs_writel(HOST_CR3, cr3);
		vmx->loaded_vmcs->host_state.cr3 = cr3;
	}

	cr4 = cr4_read_shadow();
	if (unlikely(cr4 != vmx->loaded_vmcs->host_state.cr4)) {
		vmcs_writel(HOST_CR4, cr4);
		vmx->loaded_vmcs->host_state.cr4 = cr4;
	}

	/* When single-stepping over STI and MOV SS, we must clear the
	 * corresponding interruptibility bits in the guest state. Otherwise
	 * vmentry fails as it then expects bit 14 (BS) in pending debug
	 * exceptions being set, but that's not correct for the guest debugging
	 * case. */
	if (vcpu->guest_debug & KVM_GUESTDBG_SINGLESTEP)
		vmx_set_interrupt_shadow(vcpu, 0);

	kvm_load_guest_xsave_state(vcpu);

	pt_guest_enter(vmx);

	atomic_switch_perf_msrs(vmx);
	if (intel_pmu_lbr_is_enabled(vcpu))
		vmx_passthrough_lbr_msrs(vcpu);

	if (enable_preemption_timer)
		vmx_update_hv_timer(vcpu);

	kvm_wait_lapic_expire(vcpu);

	/*
	 * If this vCPU has touched SPEC_CTRL, restore the guest's value if
	 * it's non-zero. Since vmentry is serialising on affected CPUs, there
	 * is no need to worry about the conditional branch over the wrmsr
	 * being speculatively taken.
	 */
	x86_spec_ctrl_set_guest(vmx->spec_ctrl, 0);

	/* The actual VMENTER/EXIT is in the .noinstr.text section. */
	vmx_vcpu_enter_exit(vcpu, vmx);

	/*
	 * We do not use IBRS in the kernel. If this vCPU has used the
	 * SPEC_CTRL MSR it may have left it on; save the value and
	 * turn it off. This is much more efficient than blindly adding
	 * it to the atomic save/restore list. Especially as the former
	 * (Saving guest MSRs on vmexit) doesn't even exist in KVM.
	 *
	 * For non-nested case:
	 * If the L01 MSR bitmap does not intercept the MSR, then we need to
	 * save it.
	 *
	 * For nested case:
	 * If the L02 MSR bitmap does not intercept the MSR, then we need to
	 * save it.
	 */
	if (unlikely(!msr_write_intercepted(vcpu, MSR_IA32_SPEC_CTRL)))
		vmx->spec_ctrl = native_read_msr(MSR_IA32_SPEC_CTRL);

	x86_spec_ctrl_restore_host(vmx->spec_ctrl, 0);

	/* All fields are clean at this point */
	if (static_branch_unlikely(&enable_evmcs)) {
		current_evmcs->hv_clean_fields |=
			HV_VMX_ENLIGHTENED_CLEAN_FIELD_ALL;

		current_evmcs->hv_vp_id = kvm_hv_get_vpindex(vcpu);
	}

	/* MSR_IA32_DEBUGCTLMSR is zeroed on vmexit. Restore it if needed */
	if (vmx->host_debugctlmsr)
		update_debugctlmsr(vmx->host_debugctlmsr);

#ifndef CONFIG_X86_64
	/*
	 * The sysexit path does not restore ds/es, so we must set them to
	 * a reasonable value ourselves.
	 *
	 * We can't defer this to vmx_prepare_switch_to_host() since that
	 * function may be executed in interrupt context, which saves and
	 * restore segments around it, nullifying its effect.
	 */
	loadsegment(ds, __USER_DS);
	loadsegment(es, __USER_DS);
#endif

	vmx_register_cache_reset(vcpu);

	pt_guest_exit(vmx);

	kvm_load_host_xsave_state(vcpu);

	vmx->nested.nested_run_pending = 0;
	vmx->idt_vectoring_info = 0;

	if (unlikely(vmx->fail)) {
		vmx->exit_reason.full = 0xdead;
		return EXIT_FASTPATH_NONE;
	}

	vmx->exit_reason.full = vmcs_read32(VM_EXIT_REASON);
	if (unlikely((u16)vmx->exit_reason.basic == EXIT_REASON_MCE_DURING_VMENTRY))
		kvm_machine_check();

	if (likely(!vmx->exit_reason.failed_vmentry))
		vmx->idt_vectoring_info = vmcs_read32(IDT_VECTORING_INFO_FIELD);

	trace_kvm_exit(vmx->exit_reason.full, vcpu, KVM_ISA_VMX);

	if (unlikely(vmx->exit_reason.failed_vmentry))
		return EXIT_FASTPATH_NONE;

	vmx->loaded_vmcs->launched = 1;

	vmx_recover_nmi_blocking(vmx);
	vmx_complete_interrupts(vmx);

	if (is_guest_mode(vcpu))
		return EXIT_FASTPATH_NONE;

	return vmx_exit_handlers_fastpath(vcpu);
}

static void vmx_free_vcpu(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);

	if (enable_pml)
		vmx_destroy_pml_buffer(vmx);
	free_vpid(vmx->vpid);
	nested_vmx_free_vcpu(vcpu);
	free_loaded_vmcs(vmx->loaded_vmcs);
}

static int vmx_create_vcpu(struct kvm_vcpu *vcpu)
{
	struct vmx_uret_msr *tsx_ctrl;
	struct vcpu_vmx *vmx;
	int i, cpu, err;

	BUILD_BUG_ON(offsetof(struct vcpu_vmx, vcpu) != 0);
	vmx = to_vmx(vcpu);

	err = -ENOMEM;

	vmx->vpid = allocate_vpid();

	/*
	 * If PML is turned on, failure on enabling PML just results in failure
	 * of creating the vcpu, therefore we can simplify PML logic (by
	 * avoiding dealing with cases, such as enabling PML partially on vcpus
	 * for the guest), etc.
	 */
	if (enable_pml) {
		vmx->pml_pg = alloc_page(GFP_KERNEL_ACCOUNT | __GFP_ZERO);
		if (!vmx->pml_pg)
			goto free_vpid;
	}

	for (i = 0; i < kvm_nr_uret_msrs; ++i) {
		vmx->guest_uret_msrs[i].data = 0;
		vmx->guest_uret_msrs[i].mask = -1ull;
	}
	if (boot_cpu_has(X86_FEATURE_RTM)) {
		/*
		 * TSX_CTRL_CPUID_CLEAR is handled in the CPUID interception.
		 * Keep the host value unchanged to avoid changing CPUID bits
		 * under the host kernel's feet.
		 */
		tsx_ctrl = vmx_find_uret_msr(vmx, MSR_IA32_TSX_CTRL);
		if (tsx_ctrl)
			vmx->guest_uret_msrs[i].mask = ~(u64)TSX_CTRL_CPUID_CLEAR;
	}

	err = alloc_loaded_vmcs(&vmx->vmcs01);
	if (err < 0)
		goto free_pml;

	/* The MSR bitmap starts with all ones */
	bitmap_fill(vmx->shadow_msr_intercept.read, MAX_POSSIBLE_PASSTHROUGH_MSRS);
	bitmap_fill(vmx->shadow_msr_intercept.write, MAX_POSSIBLE_PASSTHROUGH_MSRS);

	vmx_disable_intercept_for_msr(vcpu, MSR_IA32_TSC, MSR_TYPE_R);
#ifdef CONFIG_X86_64
	vmx_disable_intercept_for_msr(vcpu, MSR_FS_BASE, MSR_TYPE_RW);
	vmx_disable_intercept_for_msr(vcpu, MSR_GS_BASE, MSR_TYPE_RW);
	vmx_disable_intercept_for_msr(vcpu, MSR_KERNEL_GS_BASE, MSR_TYPE_RW);
#endif
	vmx_disable_intercept_for_msr(vcpu, MSR_IA32_SYSENTER_CS, MSR_TYPE_RW);
	vmx_disable_intercept_for_msr(vcpu, MSR_IA32_SYSENTER_ESP, MSR_TYPE_RW);
	vmx_disable_intercept_for_msr(vcpu, MSR_IA32_SYSENTER_EIP, MSR_TYPE_RW);
	if (kvm_cstate_in_guest(vcpu->kvm)) {
		vmx_disable_intercept_for_msr(vcpu, MSR_CORE_C1_RES, MSR_TYPE_R);
		vmx_disable_intercept_for_msr(vcpu, MSR_CORE_C3_RESIDENCY, MSR_TYPE_R);
		vmx_disable_intercept_for_msr(vcpu, MSR_CORE_C6_RESIDENCY, MSR_TYPE_R);
		vmx_disable_intercept_for_msr(vcpu, MSR_CORE_C7_RESIDENCY, MSR_TYPE_R);
	}
	vmx->msr_bitmap_mode = 0;

	vmx->loaded_vmcs = &vmx->vmcs01;
	cpu = get_cpu();
	vmx_vcpu_load(vcpu, cpu);
	vcpu->cpu = cpu;
	init_vmcs(vmx);
	vmx_vcpu_put(vcpu);
	put_cpu();
	if (cpu_need_virtualize_apic_accesses(vcpu)) {
		err = alloc_apic_access_page(vcpu->kvm);
		if (err)
			goto free_vmcs;
	}

	if (enable_ept && !enable_unrestricted_guest) {
		err = init_rmode_identity_map(vcpu->kvm);
		if (err)
			goto free_vmcs;
	}

	if (nested)
		memcpy(&vmx->nested.msrs, &vmcs_config.nested, sizeof(vmx->nested.msrs));
	else
		memset(&vmx->nested.msrs, 0, sizeof(vmx->nested.msrs));

	vcpu_setup_sgx_lepubkeyhash(vcpu);

	vmx->nested.posted_intr_nv = -1;
	vmx->nested.current_vmptr = -1ull;

	vcpu->arch.microcode_version = 0x100000000ULL;
	vmx->msr_ia32_feature_control_valid_bits = FEAT_CTL_LOCKED;

	/*
	 * Enforce invariant: pi_desc.nv is always either POSTED_INTR_VECTOR
	 * or POSTED_INTR_WAKEUP_VECTOR.
	 */
	vmx->pi_desc.nv = POSTED_INTR_VECTOR;
	vmx->pi_desc.sn = 1;

#if IS_ENABLED(CONFIG_HYPERV)
	vmx->hv_root_ept = INVALID_PAGE;
#endif
	return 0;

free_vmcs:
	free_loaded_vmcs(vmx->loaded_vmcs);
free_pml:
	vmx_destroy_pml_buffer(vmx);
free_vpid:
	free_vpid(vmx->vpid);
	return err;
}

#define L1TF_MSG_SMT "L1TF CPU bug present and SMT on, data leak possible. See CVE-2018-3646 and https://www.kernel.org/doc/html/latest/admin-guide/hw-vuln/l1tf.html for details.\n"
#define L1TF_MSG_L1D "L1TF CPU bug present and virtualization mitigation disabled, data leak possible. See CVE-2018-3646 and https://www.kernel.org/doc/html/latest/admin-guide/hw-vuln/l1tf.html for details.\n"

static int vmx_vm_init(struct kvm *kvm)
{
#if IS_ENABLED(CONFIG_HYPERV)
	spin_lock_init(&to_kvm_vmx(kvm)->hv_root_ept_lock);
#endif

	if (!ple_gap)
		kvm->arch.pause_in_guest = true;

	if (boot_cpu_has(X86_BUG_L1TF) && enable_ept) {
		switch (l1tf_mitigation) {
		case L1TF_MITIGATION_OFF:
		case L1TF_MITIGATION_FLUSH_NOWARN:
			/* 'I explicitly don't care' is set */
			break;
		case L1TF_MITIGATION_FLUSH:
		case L1TF_MITIGATION_FLUSH_NOSMT:
		case L1TF_MITIGATION_FULL:
			/*
			 * Warn upon starting the first VM in a potentially
			 * insecure environment.
			 */
			if (sched_smt_active())
				pr_warn_once(L1TF_MSG_SMT);
			if (l1tf_vmx_mitigation == VMENTER_L1D_FLUSH_NEVER)
				pr_warn_once(L1TF_MSG_L1D);
			break;
		case L1TF_MITIGATION_FULL_FORCE:
			/* Flush is enforced */
			break;
		}
	}
	kvm_apicv_init(kvm, enable_apicv);
	return 0;
}

static int __init vmx_check_processor_compat(void)
{
	struct vmcs_config vmcs_conf;
	struct vmx_capability vmx_cap;

	if (!this_cpu_has(X86_FEATURE_MSR_IA32_FEAT_CTL) ||
	    !this_cpu_has(X86_FEATURE_VMX)) {
		pr_err("kvm: VMX is disabled on CPU %d\n", smp_processor_id());
		return -EIO;
	}

	if (setup_vmcs_config(&vmcs_conf, &vmx_cap) < 0)
		return -EIO;
	if (nested)
		nested_vmx_setup_ctls_msrs(&vmcs_conf.nested, vmx_cap.ept);
	if (memcmp(&vmcs_config, &vmcs_conf, sizeof(struct vmcs_config)) != 0) {
		printk(KERN_ERR "kvm: CPU %d feature inconsistency!\n",
				smp_processor_id());
		return -EIO;
	}
	return 0;
}

static u64 vmx_get_mt_mask(struct kvm_vcpu *vcpu, gfn_t gfn, bool is_mmio)
{
	u8 cache;
	u64 ipat = 0;

	/* We wanted to honor guest CD/MTRR/PAT, but doing so could result in
	 * memory aliases with conflicting memory types and sometimes MCEs.
	 * We have to be careful as to what are honored and when.
	 *
	 * For MMIO, guest CD/MTRR are ignored.  The EPT memory type is set to
	 * UC.  The effective memory type is UC or WC depending on guest PAT.
	 * This was historically the source of MCEs and we want to be
	 * conservative.
	 *
	 * When there is no need to deal with noncoherent DMA (e.g., no VT-d
	 * or VT-d has snoop control), guest CD/MTRR/PAT are all ignored.  The
	 * EPT memory type is set to WB.  The effective memory type is forced
	 * WB.
	 *
	 * Otherwise, we trust guest.  Guest CD/MTRR/PAT are all honored.  The
	 * EPT memory type is used to emulate guest CD/MTRR.
	 */

	if (is_mmio) {
		cache = MTRR_TYPE_UNCACHABLE;
		goto exit;
	}

	if (!kvm_arch_has_noncoherent_dma(vcpu->kvm)) {
		ipat = VMX_EPT_IPAT_BIT;
		cache = MTRR_TYPE_WRBACK;
		goto exit;
	}

	if (kvm_read_cr0(vcpu) & X86_CR0_CD) {
		ipat = VMX_EPT_IPAT_BIT;
		if (kvm_check_has_quirk(vcpu->kvm, KVM_X86_QUIRK_CD_NW_CLEARED))
			cache = MTRR_TYPE_WRBACK;
		else
			cache = MTRR_TYPE_UNCACHABLE;
		goto exit;
	}

	cache = kvm_mtrr_get_guest_memory_type(vcpu, gfn);

exit:
	return (cache << VMX_EPT_MT_EPTE_SHIFT) | ipat;
}

static void vmcs_set_secondary_exec_control(struct vcpu_vmx *vmx)
{
	/*
	 * These bits in the secondary execution controls field
	 * are dynamic, the others are mostly based on the hypervisor
	 * architecture and the guest's CPUID.  Do not touch the
	 * dynamic bits.
	 */
	u32 mask =
		SECONDARY_EXEC_SHADOW_VMCS |
		SECONDARY_EXEC_VIRTUALIZE_X2APIC_MODE |
		SECONDARY_EXEC_VIRTUALIZE_APIC_ACCESSES |
		SECONDARY_EXEC_DESC;

	u32 new_ctl = vmx->secondary_exec_control;
	u32 cur_ctl = secondary_exec_controls_get(vmx);

	secondary_exec_controls_set(vmx, (new_ctl & ~mask) | (cur_ctl & mask));
}

/*
 * Generate MSR_IA32_VMX_CR{0,4}_FIXED1 according to CPUID. Only set bits
 * (indicating "allowed-1") if they are supported in the guest's CPUID.
 */
static void nested_vmx_cr_fixed1_bits_update(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	struct kvm_cpuid_entry2 *entry;

	vmx->nested.msrs.cr0_fixed1 = 0xffffffff;
	vmx->nested.msrs.cr4_fixed1 = X86_CR4_PCE;

#define cr4_fixed1_update(_cr4_mask, _reg, _cpuid_mask) do {		\
	if (entry && (entry->_reg & (_cpuid_mask)))			\
		vmx->nested.msrs.cr4_fixed1 |= (_cr4_mask);	\
} while (0)

	entry = kvm_find_cpuid_entry(vcpu, 0x1, 0);
	cr4_fixed1_update(X86_CR4_VME,        edx, feature_bit(VME));
	cr4_fixed1_update(X86_CR4_PVI,        edx, feature_bit(VME));
	cr4_fixed1_update(X86_CR4_TSD,        edx, feature_bit(TSC));
	cr4_fixed1_update(X86_CR4_DE,         edx, feature_bit(DE));
	cr4_fixed1_update(X86_CR4_PSE,        edx, feature_bit(PSE));
	cr4_fixed1_update(X86_CR4_PAE,        edx, feature_bit(PAE));
	cr4_fixed1_update(X86_CR4_MCE,        edx, feature_bit(MCE));
	cr4_fixed1_update(X86_CR4_PGE,        edx, feature_bit(PGE));
	cr4_fixed1_update(X86_CR4_OSFXSR,     edx, feature_bit(FXSR));
	cr4_fixed1_update(X86_CR4_OSXMMEXCPT, edx, feature_bit(XMM));
	cr4_fixed1_update(X86_CR4_VMXE,       ecx, feature_bit(VMX));
	cr4_fixed1_update(X86_CR4_SMXE,       ecx, feature_bit(SMX));
	cr4_fixed1_update(X86_CR4_PCIDE,      ecx, feature_bit(PCID));
	cr4_fixed1_update(X86_CR4_OSXSAVE,    ecx, feature_bit(XSAVE));

	entry = kvm_find_cpuid_entry(vcpu, 0x7, 0);
	cr4_fixed1_update(X86_CR4_FSGSBASE,   ebx, feature_bit(FSGSBASE));
	cr4_fixed1_update(X86_CR4_SMEP,       ebx, feature_bit(SMEP));
	cr4_fixed1_update(X86_CR4_SMAP,       ebx, feature_bit(SMAP));
	cr4_fixed1_update(X86_CR4_PKE,        ecx, feature_bit(PKU));
	cr4_fixed1_update(X86_CR4_UMIP,       ecx, feature_bit(UMIP));
	cr4_fixed1_update(X86_CR4_LA57,       ecx, feature_bit(LA57));

#undef cr4_fixed1_update
}

static void nested_vmx_entry_exit_ctls_update(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);

	if (kvm_mpx_supported()) {
		bool mpx_enabled = guest_cpuid_has(vcpu, X86_FEATURE_MPX);

		if (mpx_enabled) {
			vmx->nested.msrs.entry_ctls_high |= VM_ENTRY_LOAD_BNDCFGS;
			vmx->nested.msrs.exit_ctls_high |= VM_EXIT_CLEAR_BNDCFGS;
		} else {
			vmx->nested.msrs.entry_ctls_high &= ~VM_ENTRY_LOAD_BNDCFGS;
			vmx->nested.msrs.exit_ctls_high &= ~VM_EXIT_CLEAR_BNDCFGS;
		}
	}
}

static void update_intel_pt_cfg(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	struct kvm_cpuid_entry2 *best = NULL;
	int i;

	for (i = 0; i < PT_CPUID_LEAVES; i++) {
		best = kvm_find_cpuid_entry(vcpu, 0x14, i);
		if (!best)
			return;
		vmx->pt_desc.caps[CPUID_EAX + i*PT_CPUID_REGS_NUM] = best->eax;
		vmx->pt_desc.caps[CPUID_EBX + i*PT_CPUID_REGS_NUM] = best->ebx;
		vmx->pt_desc.caps[CPUID_ECX + i*PT_CPUID_REGS_NUM] = best->ecx;
		vmx->pt_desc.caps[CPUID_EDX + i*PT_CPUID_REGS_NUM] = best->edx;
	}

	/* Get the number of configurable Address Ranges for filtering */
	vmx->pt_desc.addr_range = intel_pt_validate_cap(vmx->pt_desc.caps,
						PT_CAP_num_address_ranges);

	/* Initialize and clear the no dependency bits */
	vmx->pt_desc.ctl_bitmask = ~(RTIT_CTL_TRACEEN | RTIT_CTL_OS |
			RTIT_CTL_USR | RTIT_CTL_TSC_EN | RTIT_CTL_DISRETC);

	/*
	 * If CPUID.(EAX=14H,ECX=0):EBX[0]=1 CR3Filter can be set otherwise
	 * will inject an #GP
	 */
	if (intel_pt_validate_cap(vmx->pt_desc.caps, PT_CAP_cr3_filtering))
		vmx->pt_desc.ctl_bitmask &= ~RTIT_CTL_CR3EN;

	/*
	 * If CPUID.(EAX=14H,ECX=0):EBX[1]=1 CYCEn, CycThresh and
	 * PSBFreq can be set
	 */
	if (intel_pt_validate_cap(vmx->pt_desc.caps, PT_CAP_psb_cyc))
		vmx->pt_desc.ctl_bitmask &= ~(RTIT_CTL_CYCLEACC |
				RTIT_CTL_CYC_THRESH | RTIT_CTL_PSB_FREQ);

	/*
	 * If CPUID.(EAX=14H,ECX=0):EBX[3]=1 MTCEn BranchEn and
	 * MTCFreq can be set
	 */
	if (intel_pt_validate_cap(vmx->pt_desc.caps, PT_CAP_mtc))
		vmx->pt_desc.ctl_bitmask &= ~(RTIT_CTL_MTC_EN |
				RTIT_CTL_BRANCH_EN | RTIT_CTL_MTC_RANGE);

	/* If CPUID.(EAX=14H,ECX=0):EBX[4]=1 FUPonPTW and PTWEn can be set */
	if (intel_pt_validate_cap(vmx->pt_desc.caps, PT_CAP_ptwrite))
		vmx->pt_desc.ctl_bitmask &= ~(RTIT_CTL_FUP_ON_PTW |
							RTIT_CTL_PTW_EN);

	/* If CPUID.(EAX=14H,ECX=0):EBX[5]=1 PwrEvEn can be set */
	if (intel_pt_validate_cap(vmx->pt_desc.caps, PT_CAP_power_event_trace))
		vmx->pt_desc.ctl_bitmask &= ~RTIT_CTL_PWR_EVT_EN;

	/* If CPUID.(EAX=14H,ECX=0):ECX[0]=1 ToPA can be set */
	if (intel_pt_validate_cap(vmx->pt_desc.caps, PT_CAP_topa_output))
		vmx->pt_desc.ctl_bitmask &= ~RTIT_CTL_TOPA;

	/* If CPUID.(EAX=14H,ECX=0):ECX[3]=1 FabricEn can be set */
	if (intel_pt_validate_cap(vmx->pt_desc.caps, PT_CAP_output_subsys))
		vmx->pt_desc.ctl_bitmask &= ~RTIT_CTL_FABRIC_EN;

	/* unmask address range configure area */
	for (i = 0; i < vmx->pt_desc.addr_range; i++)
		vmx->pt_desc.ctl_bitmask &= ~(0xfULL << (32 + i * 4));
}

static void vmx_vcpu_after_set_cpuid(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);

	/* xsaves_enabled is recomputed in vmx_compute_secondary_exec_control(). */
	vcpu->arch.xsaves_enabled = false;

	if (cpu_has_secondary_exec_ctrls()) {
		vmx_compute_secondary_exec_control(vmx);
		vmcs_set_secondary_exec_control(vmx);
	}

	if (nested_vmx_allowed(vcpu))
		to_vmx(vcpu)->msr_ia32_feature_control_valid_bits |=
			FEAT_CTL_VMX_ENABLED_INSIDE_SMX |
			FEAT_CTL_VMX_ENABLED_OUTSIDE_SMX;
	else
		to_vmx(vcpu)->msr_ia32_feature_control_valid_bits &=
			~(FEAT_CTL_VMX_ENABLED_INSIDE_SMX |
			  FEAT_CTL_VMX_ENABLED_OUTSIDE_SMX);

	if (nested_vmx_allowed(vcpu)) {
		nested_vmx_cr_fixed1_bits_update(vcpu);
		nested_vmx_entry_exit_ctls_update(vcpu);
	}

	if (boot_cpu_has(X86_FEATURE_INTEL_PT) &&
			guest_cpuid_has(vcpu, X86_FEATURE_INTEL_PT))
		update_intel_pt_cfg(vcpu);

	if (boot_cpu_has(X86_FEATURE_RTM)) {
		struct vmx_uret_msr *msr;
		msr = vmx_find_uret_msr(vmx, MSR_IA32_TSX_CTRL);
		if (msr) {
			bool enabled = guest_cpuid_has(vcpu, X86_FEATURE_RTM);
			vmx_set_guest_uret_msr(vmx, msr, enabled ? 0 : TSX_CTRL_RTM_DISABLE);
		}
	}

	set_cr4_guest_host_mask(vmx);

	vmx_write_encls_bitmap(vcpu, NULL);
	if (guest_cpuid_has(vcpu, X86_FEATURE_SGX))
		vmx->msr_ia32_feature_control_valid_bits |= FEAT_CTL_SGX_ENABLED;
	else
		vmx->msr_ia32_feature_control_valid_bits &= ~FEAT_CTL_SGX_ENABLED;

	if (guest_cpuid_has(vcpu, X86_FEATURE_SGX_LC))
		vmx->msr_ia32_feature_control_valid_bits |=
			FEAT_CTL_SGX_LC_ENABLED;
	else
		vmx->msr_ia32_feature_control_valid_bits &=
			~FEAT_CTL_SGX_LC_ENABLED;

	/* Refresh #PF interception to account for MAXPHYADDR changes. */
	vmx_update_exception_bitmap(vcpu);
}

static __init void vmx_set_cpu_caps(void)
{
	kvm_set_cpu_caps();

	/* CPUID 0x1 */
	if (nested)
		kvm_cpu_cap_set(X86_FEATURE_VMX);

	/* CPUID 0x7 */
	if (kvm_mpx_supported())
		kvm_cpu_cap_check_and_set(X86_FEATURE_MPX);
	if (!cpu_has_vmx_invpcid())
		kvm_cpu_cap_clear(X86_FEATURE_INVPCID);
	if (vmx_pt_mode_is_host_guest())
		kvm_cpu_cap_check_and_set(X86_FEATURE_INTEL_PT);

	if (!enable_sgx) {
		kvm_cpu_cap_clear(X86_FEATURE_SGX);
		kvm_cpu_cap_clear(X86_FEATURE_SGX_LC);
		kvm_cpu_cap_clear(X86_FEATURE_SGX1);
		kvm_cpu_cap_clear(X86_FEATURE_SGX2);
	}

	if (vmx_umip_emulated())
		kvm_cpu_cap_set(X86_FEATURE_UMIP);

	/* CPUID 0xD.1 */
	supported_xss = 0;
	if (!cpu_has_vmx_xsaves())
		kvm_cpu_cap_clear(X86_FEATURE_XSAVES);

	/* CPUID 0x80000001 and 0x7 (RDPID) */
	if (!cpu_has_vmx_rdtscp()) {
		kvm_cpu_cap_clear(X86_FEATURE_RDTSCP);
		kvm_cpu_cap_clear(X86_FEATURE_RDPID);
	}

	if (cpu_has_vmx_waitpkg())
		kvm_cpu_cap_check_and_set(X86_FEATURE_WAITPKG);
}

static void vmx_request_immediate_exit(struct kvm_vcpu *vcpu)
{
	to_vmx(vcpu)->req_immediate_exit = true;
}

static int vmx_check_intercept_io(struct kvm_vcpu *vcpu,
				  struct x86_instruction_info *info)
{
	struct vmcs12 *vmcs12 = get_vmcs12(vcpu);
	unsigned short port;
	bool intercept;
	int size;

	if (info->intercept == x86_intercept_in ||
	    info->intercept == x86_intercept_ins) {
		port = info->src_val;
		size = info->dst_bytes;
	} else {
		port = info->dst_val;
		size = info->src_bytes;
	}

	/*
	 * If the 'use IO bitmaps' VM-execution control is 0, IO instruction
	 * VM-exits depend on the 'unconditional IO exiting' VM-execution
	 * control.
	 *
	 * Otherwise, IO instruction VM-exits are controlled by the IO bitmaps.
	 */
	if (!nested_cpu_has(vmcs12, CPU_BASED_USE_IO_BITMAPS))
		intercept = nested_cpu_has(vmcs12,
					   CPU_BASED_UNCOND_IO_EXITING);
	else
		intercept = nested_vmx_check_io_bitmaps(vcpu, port, size);

	/* FIXME: produce nested vmexit and return X86EMUL_INTERCEPTED.  */
	return intercept ? X86EMUL_UNHANDLEABLE : X86EMUL_CONTINUE;
}

static int vmx_check_intercept(struct kvm_vcpu *vcpu,
			       struct x86_instruction_info *info,
			       enum x86_intercept_stage stage,
			       struct x86_exception *exception)
{
	struct vmcs12 *vmcs12 = get_vmcs12(vcpu);

	switch (info->intercept) {
	/*
	 * RDPID causes #UD if disabled through secondary execution controls.
	 * Because it is marked as EmulateOnUD, we need to intercept it here.
	 * Note, RDPID is hidden behind ENABLE_RDTSCP.
	 */
	case x86_intercept_rdpid:
		if (!nested_cpu_has2(vmcs12, SECONDARY_EXEC_ENABLE_RDTSCP)) {
			exception->vector = UD_VECTOR;
			exception->error_code_valid = false;
			return X86EMUL_PROPAGATE_FAULT;
		}
		break;

	case x86_intercept_in:
	case x86_intercept_ins:
	case x86_intercept_out:
	case x86_intercept_outs:
		return vmx_check_intercept_io(vcpu, info);

	case x86_intercept_lgdt:
	case x86_intercept_lidt:
	case x86_intercept_lldt:
	case x86_intercept_ltr:
	case x86_intercept_sgdt:
	case x86_intercept_sidt:
	case x86_intercept_sldt:
	case x86_intercept_str:
		if (!nested_cpu_has2(vmcs12, SECONDARY_EXEC_DESC))
			return X86EMUL_CONTINUE;

		/* FIXME: produce nested vmexit and return X86EMUL_INTERCEPTED.  */
		break;

	/* TODO: check more intercepts... */
	default:
		break;
	}

	return X86EMUL_UNHANDLEABLE;
}

#ifdef CONFIG_X86_64
/* (a << shift) / divisor, return 1 if overflow otherwise 0 */
static inline int u64_shl_div_u64(u64 a, unsigned int shift,
				  u64 divisor, u64 *result)
{
	u64 low = a << shift, high = a >> (64 - shift);

	/* To avoid the overflow on divq */
	if (high >= divisor)
		return 1;

	/* Low hold the result, high hold rem which is discarded */
	asm("divq %2\n\t" : "=a" (low), "=d" (high) :
	    "rm" (divisor), "0" (low), "1" (high));
	*result = low;

	return 0;
}

static int vmx_set_hv_timer(struct kvm_vcpu *vcpu, u64 guest_deadline_tsc,
			    bool *expired)
{
	struct vcpu_vmx *vmx;
	u64 tscl, guest_tscl, delta_tsc, lapic_timer_advance_cycles;
	struct kvm_timer *ktimer = &vcpu->arch.apic->lapic_timer;

	vmx = to_vmx(vcpu);
	tscl = rdtsc();
	guest_tscl = kvm_read_l1_tsc(vcpu, tscl);
	delta_tsc = max(guest_deadline_tsc, guest_tscl) - guest_tscl;
	lapic_timer_advance_cycles = nsec_to_cycles(vcpu,
						    ktimer->timer_advance_ns);

	if (delta_tsc > lapic_timer_advance_cycles)
		delta_tsc -= lapic_timer_advance_cycles;
	else
		delta_tsc = 0;

	/* Convert to host delta tsc if tsc scaling is enabled */
	if (vcpu->arch.tsc_scaling_ratio != kvm_default_tsc_scaling_ratio &&
	    delta_tsc && u64_shl_div_u64(delta_tsc,
				kvm_tsc_scaling_ratio_frac_bits,
				vcpu->arch.tsc_scaling_ratio, &delta_tsc))
		return -ERANGE;

	/*
	 * If the delta tsc can't fit in the 32 bit after the multi shift,
	 * we can't use the preemption timer.
	 * It's possible that it fits on later vmentries, but checking
	 * on every vmentry is costly so we just use an hrtimer.
	 */
	if (delta_tsc >> (cpu_preemption_timer_multi + 32))
		return -ERANGE;

	vmx->hv_deadline_tsc = tscl + delta_tsc;
	*expired = !delta_tsc;
	return 0;
}

static void vmx_cancel_hv_timer(struct kvm_vcpu *vcpu)
{
	to_vmx(vcpu)->hv_deadline_tsc = -1;
}
#endif

static void vmx_sched_in(struct kvm_vcpu *vcpu, int cpu)
{
	if (!kvm_pause_in_guest(vcpu->kvm))
		shrink_ple_window(vcpu);
}

void vmx_update_cpu_dirty_logging(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);

	if (is_guest_mode(vcpu)) {
		vmx->nested.update_vmcs01_cpu_dirty_logging = true;
		return;
	}

	/*
	 * Note, cpu_dirty_logging_count can be changed concurrent with this
	 * code, but in that case another update request will be made and so
	 * the guest will never run with a stale PML value.
	 */
	if (vcpu->kvm->arch.cpu_dirty_logging_count)
		secondary_exec_controls_setbit(vmx, SECONDARY_EXEC_ENABLE_PML);
	else
		secondary_exec_controls_clearbit(vmx, SECONDARY_EXEC_ENABLE_PML);
}

static int vmx_pre_block(struct kvm_vcpu *vcpu)
{
	if (pi_pre_block(vcpu))
		return 1;

	if (kvm_lapic_hv_timer_in_use(vcpu))
		kvm_lapic_switch_to_sw_timer(vcpu);

	return 0;
}

static void vmx_post_block(struct kvm_vcpu *vcpu)
{
	if (kvm_x86_ops.set_hv_timer)
		kvm_lapic_switch_to_hv_timer(vcpu);

	pi_post_block(vcpu);
}

static void vmx_setup_mce(struct kvm_vcpu *vcpu)
{
	if (vcpu->arch.mcg_cap & MCG_LMCE_P)
		to_vmx(vcpu)->msr_ia32_feature_control_valid_bits |=
			FEAT_CTL_LMCE_ENABLED;
	else
		to_vmx(vcpu)->msr_ia32_feature_control_valid_bits &=
			~FEAT_CTL_LMCE_ENABLED;
}

static int vmx_smi_allowed(struct kvm_vcpu *vcpu, bool for_injection)
{
	/* we need a nested vmexit to enter SMM, postpone if run is pending */
	if (to_vmx(vcpu)->nested.nested_run_pending)
		return -EBUSY;
	return !is_smm(vcpu);
}

static int vmx_pre_enter_smm(struct kvm_vcpu *vcpu, char *smstate)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);

	vmx->nested.smm.guest_mode = is_guest_mode(vcpu);
	if (vmx->nested.smm.guest_mode)
		nested_vmx_vmexit(vcpu, -1, 0, 0);

	vmx->nested.smm.vmxon = vmx->nested.vmxon;
	vmx->nested.vmxon = false;
	vmx_clear_hlt(vcpu);
	return 0;
}

static int vmx_pre_leave_smm(struct kvm_vcpu *vcpu, const char *smstate)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	int ret;

	if (vmx->nested.smm.vmxon) {
		vmx->nested.vmxon = true;
		vmx->nested.smm.vmxon = false;
	}

	if (vmx->nested.smm.guest_mode) {
		ret = nested_vmx_enter_non_root_mode(vcpu, false);
		if (ret)
			return ret;

		vmx->nested.smm.guest_mode = false;
	}
	return 0;
}

static void vmx_enable_smi_window(struct kvm_vcpu *vcpu)
{
	/* RSM will cause a vmexit anyway.  */
}

static bool vmx_apic_init_signal_blocked(struct kvm_vcpu *vcpu)
{
	return to_vmx(vcpu)->nested.vmxon && !is_guest_mode(vcpu);
}

static void vmx_migrate_timers(struct kvm_vcpu *vcpu)
{
	if (is_guest_mode(vcpu)) {
		struct hrtimer *timer = &to_vmx(vcpu)->nested.preemption_timer;

		if (hrtimer_try_to_cancel(timer) == 1)
			hrtimer_start_expires(timer, HRTIMER_MODE_ABS_PINNED);
	}
}

static void hardware_unsetup(void)
{
	if (nested)
		nested_vmx_hardware_unsetup();

	free_kvm_area();
}

static bool vmx_check_apicv_inhibit_reasons(ulong bit)
{
	ulong supported = BIT(APICV_INHIBIT_REASON_DISABLE) |
			  BIT(APICV_INHIBIT_REASON_HYPERV);

	return supported & BIT(bit);
}

static struct kvm_x86_ops vmx_x86_ops __initdata = {
	.hardware_unsetup = hardware_unsetup,

	.hardware_enable = hardware_enable,
	.hardware_disable = hardware_disable,
	.cpu_has_accelerated_tpr = report_flexpriority,
	.has_emulated_msr = vmx_has_emulated_msr,

	.vm_size = sizeof(struct kvm_vmx),
	.vm_init = vmx_vm_init,

	.vcpu_create = vmx_create_vcpu,
	.vcpu_free = vmx_free_vcpu,
	.vcpu_reset = vmx_vcpu_reset,

	.prepare_guest_switch = vmx_prepare_switch_to_guest,
	.vcpu_load = vmx_vcpu_load,
	.vcpu_put = vmx_vcpu_put,

	.update_exception_bitmap = vmx_update_exception_bitmap,
	.get_msr_feature = vmx_get_msr_feature,
	.get_msr = vmx_get_msr,
	.set_msr = vmx_set_msr,
	.get_segment_base = vmx_get_segment_base,
	.get_segment = vmx_get_segment,
	.set_segment = vmx_set_segment,
	.get_cpl = vmx_get_cpl,
	.get_cs_db_l_bits = vmx_get_cs_db_l_bits,
	.set_cr0 = vmx_set_cr0,
	.is_valid_cr4 = vmx_is_valid_cr4,
	.set_cr4 = vmx_set_cr4,
	.set_efer = vmx_set_efer,
	.get_idt = vmx_get_idt,
	.set_idt = vmx_set_idt,
	.get_gdt = vmx_get_gdt,
	.set_gdt = vmx_set_gdt,
	.set_dr7 = vmx_set_dr7,
	.sync_dirty_debug_regs = vmx_sync_dirty_debug_regs,
	.cache_reg = vmx_cache_reg,
	.get_rflags = vmx_get_rflags,
	.set_rflags = vmx_set_rflags,

	.tlb_flush_all = vmx_flush_tlb_all,
	.tlb_flush_current = vmx_flush_tlb_current,
	.tlb_flush_gva = vmx_flush_tlb_gva,
	.tlb_flush_guest = vmx_flush_tlb_guest,

	.run = vmx_vcpu_run,
	.handle_exit = vmx_handle_exit,
	.skip_emulated_instruction = vmx_skip_emulated_instruction,
	.update_emulated_instruction = vmx_update_emulated_instruction,
	.set_interrupt_shadow = vmx_set_interrupt_shadow,
	.get_interrupt_shadow = vmx_get_interrupt_shadow,
	.patch_hypercall = vmx_patch_hypercall,
	.set_irq = vmx_inject_irq,
	.set_nmi = vmx_inject_nmi,
	.queue_exception = vmx_queue_exception,
	.cancel_injection = vmx_cancel_injection,
	.interrupt_allowed = vmx_interrupt_allowed,
	.nmi_allowed = vmx_nmi_allowed,
	.get_nmi_mask = vmx_get_nmi_mask,
	.set_nmi_mask = vmx_set_nmi_mask,
	.enable_nmi_window = vmx_enable_nmi_window,
	.enable_irq_window = vmx_enable_irq_window,
	.update_cr8_intercept = vmx_update_cr8_intercept,
	.set_virtual_apic_mode = vmx_set_virtual_apic_mode,
	.set_apic_access_page_addr = vmx_set_apic_access_page_addr,
	.refresh_apicv_exec_ctrl = vmx_refresh_apicv_exec_ctrl,
	.load_eoi_exitmap = vmx_load_eoi_exitmap,
	.apicv_post_state_restore = vmx_apicv_post_state_restore,
	.check_apicv_inhibit_reasons = vmx_check_apicv_inhibit_reasons,
	.hwapic_irr_update = vmx_hwapic_irr_update,
	.hwapic_isr_update = vmx_hwapic_isr_update,
	.guest_apic_has_interrupt = vmx_guest_apic_has_interrupt,
	.sync_pir_to_irr = vmx_sync_pir_to_irr,
	.deliver_posted_interrupt = vmx_deliver_posted_interrupt,
	.dy_apicv_has_pending_interrupt = pi_has_pending_interrupt,

	.set_tss_addr = vmx_set_tss_addr,
	.set_identity_map_addr = vmx_set_identity_map_addr,
	.get_mt_mask = vmx_get_mt_mask,

	.get_exit_info = vmx_get_exit_info,

	.vcpu_after_set_cpuid = vmx_vcpu_after_set_cpuid,

	.has_wbinvd_exit = cpu_has_vmx_wbinvd_exit,

	.write_l1_tsc_offset = vmx_write_l1_tsc_offset,

	.load_mmu_pgd = vmx_load_mmu_pgd,

	.check_intercept = vmx_check_intercept,
	.handle_exit_irqoff = vmx_handle_exit_irqoff,

	.request_immediate_exit = vmx_request_immediate_exit,

	.sched_in = vmx_sched_in,

	.cpu_dirty_log_size = PML_ENTITY_NUM,
	.update_cpu_dirty_logging = vmx_update_cpu_dirty_logging,

	.pre_block = vmx_pre_block,
	.post_block = vmx_post_block,

	.pmu_ops = &intel_pmu_ops,
	.nested_ops = &vmx_nested_ops,

	.update_pi_irte = pi_update_irte,
	.start_assignment = vmx_pi_start_assignment,

#ifdef CONFIG_X86_64
	.set_hv_timer = vmx_set_hv_timer,
	.cancel_hv_timer = vmx_cancel_hv_timer,
#endif

	.setup_mce = vmx_setup_mce,

	.smi_allowed = vmx_smi_allowed,
	.pre_enter_smm = vmx_pre_enter_smm,
	.pre_leave_smm = vmx_pre_leave_smm,
	.enable_smi_window = vmx_enable_smi_window,

	.can_emulate_instruction = vmx_can_emulate_instruction,
	.apic_init_signal_blocked = vmx_apic_init_signal_blocked,
	.migrate_timers = vmx_migrate_timers,

	.msr_filter_changed = vmx_msr_filter_changed,
	.complete_emulated_msr = kvm_complete_insn_gp,

	.vcpu_deliver_sipi_vector = kvm_vcpu_deliver_sipi_vector,
};

static __init void vmx_setup_user_return_msrs(void)
{

	/*
	 * Though SYSCALL is only supported in 64-bit mode on Intel CPUs, kvm
	 * will emulate SYSCALL in legacy mode if the vendor string in guest
	 * CPUID.0:{EBX,ECX,EDX} is "AuthenticAMD" or "AMDisbetter!" To
	 * support this emulation, MSR_STAR is included in the list for i386,
	 * but is never loaded into hardware.  MSR_CSTAR is also never loaded
	 * into hardware and is here purely for emulation purposes.
	 */
	const u32 vmx_uret_msrs_list[] = {
	#ifdef CONFIG_X86_64
		MSR_SYSCALL_MASK, MSR_LSTAR, MSR_CSTAR,
	#endif
		MSR_EFER, MSR_TSC_AUX, MSR_STAR,
		MSR_IA32_TSX_CTRL,
	};
	int i;

	BUILD_BUG_ON(ARRAY_SIZE(vmx_uret_msrs_list) != MAX_NR_USER_RETURN_MSRS);

	for (i = 0; i < ARRAY_SIZE(vmx_uret_msrs_list); ++i)
		kvm_add_user_return_msr(vmx_uret_msrs_list[i]);
}

static __init int hardware_setup(void)
{
	unsigned long host_bndcfgs;
	struct desc_ptr dt;
	int r, ept_lpage_level;

	store_idt(&dt);
	host_idt_base = dt.address;

	vmx_setup_user_return_msrs();

	if (setup_vmcs_config(&vmcs_config, &vmx_capability) < 0)
		return -EIO;

	if (boot_cpu_has(X86_FEATURE_NX))
		kvm_enable_efer_bits(EFER_NX);

	if (boot_cpu_has(X86_FEATURE_MPX)) {
		rdmsrl(MSR_IA32_BNDCFGS, host_bndcfgs);
		WARN_ONCE(host_bndcfgs, "KVM: BNDCFGS in host will be lost");
	}

	if (!cpu_has_vmx_mpx())
		supported_xcr0 &= ~(XFEATURE_MASK_BNDREGS |
				    XFEATURE_MASK_BNDCSR);

	if (!cpu_has_vmx_vpid() || !cpu_has_vmx_invvpid() ||
	    !(cpu_has_vmx_invvpid_single() || cpu_has_vmx_invvpid_global()))
		enable_vpid = 0;

	if (!cpu_has_vmx_ept() ||
	    !cpu_has_vmx_ept_4levels() ||
	    !cpu_has_vmx_ept_mt_wb() ||
	    !cpu_has_vmx_invept_global())
		enable_ept = 0;

	if (!cpu_has_vmx_ept_ad_bits() || !enable_ept)
		enable_ept_ad_bits = 0;

	if (!cpu_has_vmx_unrestricted_guest() || !enable_ept)
		enable_unrestricted_guest = 0;

	if (!cpu_has_vmx_flexpriority())
		flexpriority_enabled = 0;

	if (!cpu_has_virtual_nmis())
		enable_vnmi = 0;

	/*
	 * set_apic_access_page_addr() is used to reload apic access
	 * page upon invalidation.  No need to do anything if not
	 * using the APIC_ACCESS_ADDR VMCS field.
	 */
	if (!flexpriority_enabled)
		vmx_x86_ops.set_apic_access_page_addr = NULL;

	if (!cpu_has_vmx_tpr_shadow())
		vmx_x86_ops.update_cr8_intercept = NULL;

#if IS_ENABLED(CONFIG_HYPERV)
	if (ms_hyperv.nested_features & HV_X64_NESTED_GUEST_MAPPING_FLUSH
	    && enable_ept) {
		vmx_x86_ops.tlb_remote_flush = hv_remote_flush_tlb;
		vmx_x86_ops.tlb_remote_flush_with_range =
				hv_remote_flush_tlb_with_range;
	}
#endif

	if (!cpu_has_vmx_ple()) {
		ple_gap = 0;
		ple_window = 0;
		ple_window_grow = 0;
		ple_window_max = 0;
		ple_window_shrink = 0;
	}

	if (!cpu_has_vmx_apicv()) {
		enable_apicv = 0;
		vmx_x86_ops.sync_pir_to_irr = NULL;
	}

	if (cpu_has_vmx_tsc_scaling()) {
		kvm_has_tsc_control = true;
		kvm_max_tsc_scaling_ratio = KVM_VMX_TSC_MULTIPLIER_MAX;
		kvm_tsc_scaling_ratio_frac_bits = 48;
	}

	kvm_has_bus_lock_exit = cpu_has_vmx_bus_lock_detection();

	set_bit(0, vmx_vpid_bitmap); /* 0 is reserved for host */

	if (enable_ept)
		kvm_mmu_set_ept_masks(enable_ept_ad_bits,
				      cpu_has_vmx_ept_execute_only());

	if (!enable_ept)
		ept_lpage_level = 0;
	else if (cpu_has_vmx_ept_1g_page())
		ept_lpage_level = PG_LEVEL_1G;
	else if (cpu_has_vmx_ept_2m_page())
		ept_lpage_level = PG_LEVEL_2M;
	else
		ept_lpage_level = PG_LEVEL_4K;
	kvm_configure_mmu(enable_ept, vmx_get_max_tdp_level(), ept_lpage_level);

	/*
	 * Only enable PML when hardware supports PML feature, and both EPT
	 * and EPT A/D bit features are enabled -- PML depends on them to work.
	 */
	if (!enable_ept || !enable_ept_ad_bits || !cpu_has_vmx_pml())
		enable_pml = 0;

	if (!enable_pml)
		vmx_x86_ops.cpu_dirty_log_size = 0;

	if (!cpu_has_vmx_preemption_timer())
		enable_preemption_timer = false;

	if (enable_preemption_timer) {
		u64 use_timer_freq = 5000ULL * 1000 * 1000;
		u64 vmx_msr;

		rdmsrl(MSR_IA32_VMX_MISC, vmx_msr);
		cpu_preemption_timer_multi =
			vmx_msr & VMX_MISC_PREEMPTION_TIMER_RATE_MASK;

		if (tsc_khz)
			use_timer_freq = (u64)tsc_khz * 1000;
		use_timer_freq >>= cpu_preemption_timer_multi;

		/*
		 * KVM "disables" the preemption timer by setting it to its max
		 * value.  Don't use the timer if it might cause spurious exits
		 * at a rate faster than 0.1 Hz (of uninterrupted guest time).
		 */
		if (use_timer_freq > 0xffffffffu / 10)
			enable_preemption_timer = false;
	}

	if (!enable_preemption_timer) {
		vmx_x86_ops.set_hv_timer = NULL;
		vmx_x86_ops.cancel_hv_timer = NULL;
		vmx_x86_ops.request_immediate_exit = __kvm_request_immediate_exit;
	}

	kvm_set_posted_intr_wakeup_handler(pi_wakeup_handler);

	kvm_mce_cap_supported |= MCG_LMCE_P;

	if (pt_mode != PT_MODE_SYSTEM && pt_mode != PT_MODE_HOST_GUEST)
		return -EINVAL;
	if (!enable_ept || !cpu_has_vmx_intel_pt())
		pt_mode = PT_MODE_SYSTEM;

	setup_default_sgx_lepubkeyhash();

	if (nested) {
		nested_vmx_setup_ctls_msrs(&vmcs_config.nested,
					   vmx_capability.ept);

		r = nested_vmx_hardware_setup(kvm_vmx_exit_handlers);
		if (r)
			return r;
	}

	vmx_set_cpu_caps();

	r = alloc_kvm_area();
	if (r)
		nested_vmx_hardware_unsetup();
	return r;
}

static struct kvm_x86_init_ops vmx_init_ops __initdata = {
	.cpu_has_kvm_support = cpu_has_kvm_support,
	.disabled_by_bios = vmx_disabled_by_bios,
	.check_processor_compatibility = vmx_check_processor_compat,
	.hardware_setup = hardware_setup,

	.runtime_ops = &vmx_x86_ops,
};

static void vmx_cleanup_l1d_flush(void)
{
	if (vmx_l1d_flush_pages) {
		free_pages((unsigned long)vmx_l1d_flush_pages, L1D_CACHE_ORDER);
		vmx_l1d_flush_pages = NULL;
	}
	/* Restore state so sysfs ignores VMX */
	l1tf_vmx_mitigation = VMENTER_L1D_FLUSH_AUTO;
}

static void vmx_exit(void)
{
#ifdef CONFIG_KEXEC_CORE
	RCU_INIT_POINTER(crash_vmclear_loaded_vmcss, NULL);
	synchronize_rcu();
#endif

	kvm_exit();

#if IS_ENABLED(CONFIG_HYPERV)
	if (static_branch_unlikely(&enable_evmcs)) {
		int cpu;
		struct hv_vp_assist_page *vp_ap;
		/*
		 * Reset everything to support using non-enlightened VMCS
		 * access later (e.g. when we reload the module with
		 * enlightened_vmcs=0)
		 */
		for_each_online_cpu(cpu) {
			vp_ap =	hv_get_vp_assist_page(cpu);

			if (!vp_ap)
				continue;

			vp_ap->nested_control.features.directhypercall = 0;
			vp_ap->current_nested_vmcs = 0;
			vp_ap->enlighten_vmentry = 0;
		}

		static_branch_disable(&enable_evmcs);
	}
#endif
	vmx_cleanup_l1d_flush();
}
module_exit(vmx_exit);

static int __init vmx_init(void)
{
	int r, cpu;

#if IS_ENABLED(CONFIG_HYPERV)
	/*
	 * Enlightened VMCS usage should be recommended and the host needs
	 * to support eVMCS v1 or above. We can also disable eVMCS support
	 * with module parameter.
	 */
	if (enlightened_vmcs &&
	    ms_hyperv.hints & HV_X64_ENLIGHTENED_VMCS_RECOMMENDED &&
	    (ms_hyperv.nested_features & HV_X64_ENLIGHTENED_VMCS_VERSION) >=
	    KVM_EVMCS_VERSION) {
		int cpu;

		/* Check that we have assist pages on all online CPUs */
		for_each_online_cpu(cpu) {
			if (!hv_get_vp_assist_page(cpu)) {
				enlightened_vmcs = false;
				break;
			}
		}

		if (enlightened_vmcs) {
			pr_info("KVM: vmx: using Hyper-V Enlightened VMCS\n");
			static_branch_enable(&enable_evmcs);
		}

		if (ms_hyperv.nested_features & HV_X64_NESTED_DIRECT_FLUSH)
			vmx_x86_ops.enable_direct_tlbflush
				= hv_enable_direct_tlbflush;

	} else {
		enlightened_vmcs = false;
	}
#endif

	r = kvm_init(&vmx_init_ops, sizeof(struct vcpu_vmx),
		     __alignof__(struct vcpu_vmx), THIS_MODULE);
	if (r)
		return r;

	/*
	 * Must be called after kvm_init() so enable_ept is properly set
	 * up. Hand the parameter mitigation value in which was stored in
	 * the pre module init parser. If no parameter was given, it will
	 * contain 'auto' which will be turned into the default 'cond'
	 * mitigation mode.
	 */
	r = vmx_setup_l1d_flush(vmentry_l1d_flush_param);
	if (r) {
		vmx_exit();
		return r;
	}

	for_each_possible_cpu(cpu) {
		INIT_LIST_HEAD(&per_cpu(loaded_vmcss_on_cpu, cpu));

		pi_init_cpu(cpu);
	}

#ifdef CONFIG_KEXEC_CORE
	rcu_assign_pointer(crash_vmclear_loaded_vmcss,
			   crash_vmclear_local_loaded_vmcss);
#endif
	vmx_check_vmcs12_offsets();

	/*
	 * Shadow paging doesn't have a (further) performance penalty
	 * from GUEST_MAXPHYADDR < HOST_MAXPHYADDR so enable it
	 * by default
	 */
	if (!enable_ept)
		allow_smaller_maxphyaddr = true;

	return 0;
}
module_init(vmx_init);
