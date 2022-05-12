// SPDX-License-Identifier: GPL-2.0-only
/*
 * X86 specific Hyper-V initialization code.
 *
 * Copyright (C) 2016, Microsoft, Inc.
 *
 * Author : K. Y. Srinivasan <kys@microsoft.com>
 */

#include <linux/acpi.h>
#include <linux/efi.h>
#include <linux/types.h>
#include <linux/bitfield.h>
#include <asm/apic.h>
#include <asm/desc.h>
#include <asm/hypervisor.h>
#include <asm/hyperv-tlfs.h>
#include <asm/mshyperv.h>
#include <asm/idtentry.h>
#include <linux/kexec.h>
#include <linux/version.h>
#include <linux/vmalloc.h>
#include <linux/mm.h>
#include <linux/hyperv.h>
#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/cpuhotplug.h>
#include <linux/syscore_ops.h>
#include <clocksource/hyperv_timer.h>
#include <linux/highmem.h>

int hyperv_init_cpuhp;
u64 hv_current_partition_id = ~0ull;
EXPORT_SYMBOL_GPL(hv_current_partition_id);

void *hv_hypercall_pg;
EXPORT_SYMBOL_GPL(hv_hypercall_pg);

/* Storage to save the hypercall page temporarily for hibernation */
static void *hv_hypercall_pg_saved;

u32 *hv_vp_index;
EXPORT_SYMBOL_GPL(hv_vp_index);

struct hv_vp_assist_page **hv_vp_assist_page;
EXPORT_SYMBOL_GPL(hv_vp_assist_page);

void  __percpu **hyperv_pcpu_input_arg;
EXPORT_SYMBOL_GPL(hyperv_pcpu_input_arg);

void  __percpu **hyperv_pcpu_output_arg;
EXPORT_SYMBOL_GPL(hyperv_pcpu_output_arg);

u32 hv_max_vp_index;
EXPORT_SYMBOL_GPL(hv_max_vp_index);

static int hv_cpu_init(unsigned int cpu)
{
	u64 msr_vp_index;
	struct hv_vp_assist_page **hvp = &hv_vp_assist_page[smp_processor_id()];
	void **input_arg;
	struct page *pg;

	/* hv_cpu_init() can be called with IRQs disabled from hv_resume() */
	pg = alloc_pages(irqs_disabled() ? GFP_ATOMIC : GFP_KERNEL, hv_root_partition ? 1 : 0);
	if (unlikely(!pg))
		return -ENOMEM;

	input_arg = (void **)this_cpu_ptr(hyperv_pcpu_input_arg);
	*input_arg = page_address(pg);
	if (hv_root_partition) {
		void **output_arg;

		output_arg = (void **)this_cpu_ptr(hyperv_pcpu_output_arg);
		*output_arg = page_address(pg + 1);
	}

	msr_vp_index = hv_get_register(HV_REGISTER_VP_INDEX);

	hv_vp_index[smp_processor_id()] = msr_vp_index;

	if (msr_vp_index > hv_max_vp_index)
		hv_max_vp_index = msr_vp_index;

	if (!hv_vp_assist_page)
		return 0;

	/*
	 * The VP ASSIST PAGE is an "overlay" page (see Hyper-V TLFS's Section
	 * 5.2.1 "GPA Overlay Pages"). Here it must be zeroed out to make sure
	 * we always write the EOI MSR in hv_apic_eoi_write() *after* the
	 * EOI optimization is disabled in hv_cpu_die(), otherwise a CPU may
	 * not be stopped in the case of CPU offlining and the VM will hang.
	 */
	if (!*hvp) {
		*hvp = __vmalloc(PAGE_SIZE, GFP_KERNEL | __GFP_ZERO);
	}

	if (*hvp) {
		u64 val;

		val = vmalloc_to_pfn(*hvp);
		val = (val << HV_X64_MSR_VP_ASSIST_PAGE_ADDRESS_SHIFT) |
			HV_X64_MSR_VP_ASSIST_PAGE_ENABLE;

		wrmsrl(HV_X64_MSR_VP_ASSIST_PAGE, val);
	}

	return 0;
}

static void (*hv_reenlightenment_cb)(void);

static void hv_reenlightenment_notify(struct work_struct *dummy)
{
	struct hv_tsc_emulation_status emu_status;

	rdmsrl(HV_X64_MSR_TSC_EMULATION_STATUS, *(u64 *)&emu_status);

	/* Don't issue the callback if TSC accesses are not emulated */
	if (hv_reenlightenment_cb && emu_status.inprogress)
		hv_reenlightenment_cb();
}
static DECLARE_DELAYED_WORK(hv_reenlightenment_work, hv_reenlightenment_notify);

void hyperv_stop_tsc_emulation(void)
{
	u64 freq;
	struct hv_tsc_emulation_status emu_status;

	rdmsrl(HV_X64_MSR_TSC_EMULATION_STATUS, *(u64 *)&emu_status);
	emu_status.inprogress = 0;
	wrmsrl(HV_X64_MSR_TSC_EMULATION_STATUS, *(u64 *)&emu_status);

	rdmsrl(HV_X64_MSR_TSC_FREQUENCY, freq);
	tsc_khz = div64_u64(freq, 1000);
}
EXPORT_SYMBOL_GPL(hyperv_stop_tsc_emulation);

static inline bool hv_reenlightenment_available(void)
{
	/*
	 * Check for required features and privileges to make TSC frequency
	 * change notifications work.
	 */
	return ms_hyperv.features & HV_ACCESS_FREQUENCY_MSRS &&
		ms_hyperv.misc_features & HV_FEATURE_FREQUENCY_MSRS_AVAILABLE &&
		ms_hyperv.features & HV_ACCESS_REENLIGHTENMENT;
}

DEFINE_IDTENTRY_SYSVEC(sysvec_hyperv_reenlightenment)
{
	ack_APIC_irq();
	inc_irq_stat(irq_hv_reenlightenment_count);
	schedule_delayed_work(&hv_reenlightenment_work, HZ/10);
}

void set_hv_tscchange_cb(void (*cb)(void))
{
	struct hv_reenlightenment_control re_ctrl = {
		.vector = HYPERV_REENLIGHTENMENT_VECTOR,
		.enabled = 1,
		.target_vp = hv_vp_index[smp_processor_id()]
	};
	struct hv_tsc_emulation_control emu_ctrl = {.enabled = 1};

	if (!hv_reenlightenment_available()) {
		pr_warn("Hyper-V: reenlightenment support is unavailable\n");
		return;
	}

	hv_reenlightenment_cb = cb;

	/* Make sure callback is registered before we write to MSRs */
	wmb();

	wrmsrl(HV_X64_MSR_REENLIGHTENMENT_CONTROL, *((u64 *)&re_ctrl));
	wrmsrl(HV_X64_MSR_TSC_EMULATION_CONTROL, *((u64 *)&emu_ctrl));
}
EXPORT_SYMBOL_GPL(set_hv_tscchange_cb);

void clear_hv_tscchange_cb(void)
{
	struct hv_reenlightenment_control re_ctrl;

	if (!hv_reenlightenment_available())
		return;

	rdmsrl(HV_X64_MSR_REENLIGHTENMENT_CONTROL, *(u64 *)&re_ctrl);
	re_ctrl.enabled = 0;
	wrmsrl(HV_X64_MSR_REENLIGHTENMENT_CONTROL, *(u64 *)&re_ctrl);

	hv_reenlightenment_cb = NULL;
}
EXPORT_SYMBOL_GPL(clear_hv_tscchange_cb);

static int hv_cpu_die(unsigned int cpu)
{
	struct hv_reenlightenment_control re_ctrl;
	unsigned int new_cpu;
	unsigned long flags;
	void **input_arg;
	void *pg;

	local_irq_save(flags);
	input_arg = (void **)this_cpu_ptr(hyperv_pcpu_input_arg);
	pg = *input_arg;
	*input_arg = NULL;

	if (hv_root_partition) {
		void **output_arg;

		output_arg = (void **)this_cpu_ptr(hyperv_pcpu_output_arg);
		*output_arg = NULL;
	}

	local_irq_restore(flags);

	free_pages((unsigned long)pg, hv_root_partition ? 1 : 0);

	if (hv_vp_assist_page && hv_vp_assist_page[cpu])
		wrmsrl(HV_X64_MSR_VP_ASSIST_PAGE, 0);

	if (hv_reenlightenment_cb == NULL)
		return 0;

	rdmsrl(HV_X64_MSR_REENLIGHTENMENT_CONTROL, *((u64 *)&re_ctrl));
	if (re_ctrl.target_vp == hv_vp_index[cpu]) {
		/*
		 * Reassign reenlightenment notifications to some other online
		 * CPU or just disable the feature if there are no online CPUs
		 * left (happens on hibernation).
		 */
		new_cpu = cpumask_any_but(cpu_online_mask, cpu);

		if (new_cpu < nr_cpu_ids)
			re_ctrl.target_vp = hv_vp_index[new_cpu];
		else
			re_ctrl.enabled = 0;

		wrmsrl(HV_X64_MSR_REENLIGHTENMENT_CONTROL, *((u64 *)&re_ctrl));
	}

	return 0;
}

static int __init hv_pci_init(void)
{
	int gen2vm = efi_enabled(EFI_BOOT);

	/*
	 * For Generation-2 VM, we exit from pci_arch_init() by returning 0.
	 * The purpose is to suppress the harmless warning:
	 * "PCI: Fatal: No config space access function found"
	 */
	if (gen2vm)
		return 0;

	/* For Generation-1 VM, we'll proceed in pci_arch_init().  */
	return 1;
}

static int hv_suspend(void)
{
	union hv_x64_msr_hypercall_contents hypercall_msr;
	int ret;

	if (hv_root_partition)
		return -EPERM;

	/*
	 * Reset the hypercall page as it is going to be invalidated
	 * across hibernation. Setting hv_hypercall_pg to NULL ensures
	 * that any subsequent hypercall operation fails safely instead of
	 * crashing due to an access of an invalid page. The hypercall page
	 * pointer is restored on resume.
	 */
	hv_hypercall_pg_saved = hv_hypercall_pg;
	hv_hypercall_pg = NULL;

	/* Disable the hypercall page in the hypervisor */
	rdmsrl(HV_X64_MSR_HYPERCALL, hypercall_msr.as_uint64);
	hypercall_msr.enable = 0;
	wrmsrl(HV_X64_MSR_HYPERCALL, hypercall_msr.as_uint64);

	ret = hv_cpu_die(0);
	return ret;
}

static void hv_resume(void)
{
	union hv_x64_msr_hypercall_contents hypercall_msr;
	int ret;

	ret = hv_cpu_init(0);
	WARN_ON(ret);

	/* Re-enable the hypercall page */
	rdmsrl(HV_X64_MSR_HYPERCALL, hypercall_msr.as_uint64);
	hypercall_msr.enable = 1;
	hypercall_msr.guest_physical_address =
		vmalloc_to_pfn(hv_hypercall_pg_saved);
	wrmsrl(HV_X64_MSR_HYPERCALL, hypercall_msr.as_uint64);

	hv_hypercall_pg = hv_hypercall_pg_saved;
	hv_hypercall_pg_saved = NULL;

	/*
	 * Reenlightenment notifications are disabled by hv_cpu_die(0),
	 * reenable them here if hv_reenlightenment_cb was previously set.
	 */
	if (hv_reenlightenment_cb)
		set_hv_tscchange_cb(hv_reenlightenment_cb);
}

/* Note: when the ops are called, only CPU0 is online and IRQs are disabled. */
static struct syscore_ops hv_syscore_ops = {
	.suspend	= hv_suspend,
	.resume		= hv_resume,
};

static void (* __initdata old_setup_percpu_clockev)(void);

static void __init hv_stimer_setup_percpu_clockev(void)
{
	/*
	 * Ignore any errors in setting up stimer clockevents
	 * as we can run with the LAPIC timer as a fallback.
	 */
	(void)hv_stimer_alloc(false);

	/*
	 * Still register the LAPIC timer, because the direct-mode STIMER is
	 * not supported by old versions of Hyper-V. This also allows users
	 * to switch to LAPIC timer via /sys, if they want to.
	 */
	if (old_setup_percpu_clockev)
		old_setup_percpu_clockev();
}

static void __init hv_get_partition_id(void)
{
	struct hv_get_partition_id *output_page;
	u64 status;
	unsigned long flags;

	local_irq_save(flags);
	output_page = *this_cpu_ptr(hyperv_pcpu_output_arg);
	status = hv_do_hypercall(HVCALL_GET_PARTITION_ID, NULL, output_page);
	if (!hv_result_success(status)) {
		/* No point in proceeding if this failed */
		pr_err("Failed to get partition ID: %lld\n", status);
		BUG();
	}
	hv_current_partition_id = output_page->partition_id;
	local_irq_restore(flags);
}

/*
 * This function is to be invoked early in the boot sequence after the
 * hypervisor has been detected.
 *
 * 1. Setup the hypercall page.
 * 2. Register Hyper-V specific clocksource.
 * 3. Setup Hyper-V specific APIC entry points.
 */
void __init hyperv_init(void)
{
	u64 guest_id, required_msrs;
	union hv_x64_msr_hypercall_contents hypercall_msr;
	int cpuhp, i;

	if (x86_hyper_type != X86_HYPER_MS_HYPERV)
		return;

	/* Absolutely required MSRs */
	required_msrs = HV_MSR_HYPERCALL_AVAILABLE |
		HV_MSR_VP_INDEX_AVAILABLE;

	if ((ms_hyperv.features & required_msrs) != required_msrs)
		return;

	/*
	 * Allocate the per-CPU state for the hypercall input arg.
	 * If this allocation fails, we will not be able to setup
	 * (per-CPU) hypercall input page and thus this failure is
	 * fatal on Hyper-V.
	 */
	hyperv_pcpu_input_arg = alloc_percpu(void  *);

	BUG_ON(hyperv_pcpu_input_arg == NULL);

	/* Allocate the per-CPU state for output arg for root */
	if (hv_root_partition) {
		hyperv_pcpu_output_arg = alloc_percpu(void *);
		BUG_ON(hyperv_pcpu_output_arg == NULL);
	}

	/* Allocate percpu VP index */
	hv_vp_index = kmalloc_array(num_possible_cpus(), sizeof(*hv_vp_index),
				    GFP_KERNEL);
	if (!hv_vp_index)
		return;

	for (i = 0; i < num_possible_cpus(); i++)
		hv_vp_index[i] = VP_INVAL;

	hv_vp_assist_page = kcalloc(num_possible_cpus(),
				    sizeof(*hv_vp_assist_page), GFP_KERNEL);
	if (!hv_vp_assist_page) {
		ms_hyperv.hints &= ~HV_X64_ENLIGHTENED_VMCS_RECOMMENDED;
		goto free_vp_index;
	}

	cpuhp = cpuhp_setup_state(CPUHP_AP_ONLINE_DYN, "x86/hyperv_init:online",
				  hv_cpu_init, hv_cpu_die);
	if (cpuhp < 0)
		goto free_vp_assist_page;

	/*
	 * Setup the hypercall page and enable hypercalls.
	 * 1. Register the guest ID
	 * 2. Enable the hypercall and register the hypercall page
	 */
	guest_id = generate_guest_id(0, LINUX_VERSION_CODE, 0);
	wrmsrl(HV_X64_MSR_GUEST_OS_ID, guest_id);

	hv_hypercall_pg = __vmalloc_node_range(PAGE_SIZE, 1, VMALLOC_START,
			VMALLOC_END, GFP_KERNEL, PAGE_KERNEL_ROX,
			VM_FLUSH_RESET_PERMS, NUMA_NO_NODE,
			__builtin_return_address(0));
	if (hv_hypercall_pg == NULL) {
		wrmsrl(HV_X64_MSR_GUEST_OS_ID, 0);
		goto remove_cpuhp_state;
	}

	rdmsrl(HV_X64_MSR_HYPERCALL, hypercall_msr.as_uint64);
	hypercall_msr.enable = 1;

	if (hv_root_partition) {
		struct page *pg;
		void *src, *dst;

		/*
		 * For the root partition, the hypervisor will set up its
		 * hypercall page. The hypervisor guarantees it will not show
		 * up in the root's address space. The root can't change the
		 * location of the hypercall page.
		 *
		 * Order is important here. We must enable the hypercall page
		 * so it is populated with code, then copy the code to an
		 * executable page.
		 */
		wrmsrl(HV_X64_MSR_HYPERCALL, hypercall_msr.as_uint64);

		pg = vmalloc_to_page(hv_hypercall_pg);
		dst = kmap(pg);
		src = memremap(hypercall_msr.guest_physical_address << PAGE_SHIFT, PAGE_SIZE,
				MEMREMAP_WB);
		BUG_ON(!(src && dst));
		memcpy(dst, src, HV_HYP_PAGE_SIZE);
		memunmap(src);
		kunmap(pg);
	} else {
		hypercall_msr.guest_physical_address = vmalloc_to_pfn(hv_hypercall_pg);
		wrmsrl(HV_X64_MSR_HYPERCALL, hypercall_msr.as_uint64);
	}

	/*
	 * hyperv_init() is called before LAPIC is initialized: see
	 * apic_intr_mode_init() -> x86_platform.apic_post_init() and
	 * apic_bsp_setup() -> setup_local_APIC(). The direct-mode STIMER
	 * depends on LAPIC, so hv_stimer_alloc() should be called from
	 * x86_init.timers.setup_percpu_clockev.
	 */
	old_setup_percpu_clockev = x86_init.timers.setup_percpu_clockev;
	x86_init.timers.setup_percpu_clockev = hv_stimer_setup_percpu_clockev;

	hv_apic_init();

	x86_init.pci.arch_init = hv_pci_init;

	register_syscore_ops(&hv_syscore_ops);

	hyperv_init_cpuhp = cpuhp;

	if (cpuid_ebx(HYPERV_CPUID_FEATURES) & HV_ACCESS_PARTITION_ID)
		hv_get_partition_id();

	BUG_ON(hv_root_partition && hv_current_partition_id == ~0ull);

#ifdef CONFIG_PCI_MSI
	/*
	 * If we're running as root, we want to create our own PCI MSI domain.
	 * We can't set this in hv_pci_init because that would be too late.
	 */
	if (hv_root_partition)
		x86_init.irqs.create_pci_msi_domain = hv_create_pci_msi_domain;
#endif

	/* Query the VMs extended capability once, so that it can be cached. */
	hv_query_ext_cap(0);
	return;

remove_cpuhp_state:
	cpuhp_remove_state(cpuhp);
free_vp_assist_page:
	kfree(hv_vp_assist_page);
	hv_vp_assist_page = NULL;
free_vp_index:
	kfree(hv_vp_index);
	hv_vp_index = NULL;
}

/*
 * This routine is called before kexec/kdump, it does the required cleanup.
 */
void hyperv_cleanup(void)
{
	union hv_x64_msr_hypercall_contents hypercall_msr;

	unregister_syscore_ops(&hv_syscore_ops);

	/* Reset our OS id */
	wrmsrl(HV_X64_MSR_GUEST_OS_ID, 0);

	/*
	 * Reset hypercall page reference before reset the page,
	 * let hypercall operations fail safely rather than
	 * panic the kernel for using invalid hypercall page
	 */
	hv_hypercall_pg = NULL;

	/* Reset the hypercall page */
	hypercall_msr.as_uint64 = 0;
	wrmsrl(HV_X64_MSR_HYPERCALL, hypercall_msr.as_uint64);

	/* Reset the TSC page */
	hypercall_msr.as_uint64 = 0;
	wrmsrl(HV_X64_MSR_REFERENCE_TSC, hypercall_msr.as_uint64);
}
EXPORT_SYMBOL_GPL(hyperv_cleanup);

void hyperv_report_panic(struct pt_regs *regs, long err, bool in_die)
{
	static bool panic_reported;
	u64 guest_id;

	if (in_die && !panic_on_oops)
		return;

	/*
	 * We prefer to report panic on 'die' chain as we have proper
	 * registers to report, but if we miss it (e.g. on BUG()) we need
	 * to report it on 'panic'.
	 */
	if (panic_reported)
		return;
	panic_reported = true;

	rdmsrl(HV_X64_MSR_GUEST_OS_ID, guest_id);

	wrmsrl(HV_X64_MSR_CRASH_P0, err);
	wrmsrl(HV_X64_MSR_CRASH_P1, guest_id);
	wrmsrl(HV_X64_MSR_CRASH_P2, regs->ip);
	wrmsrl(HV_X64_MSR_CRASH_P3, regs->ax);
	wrmsrl(HV_X64_MSR_CRASH_P4, regs->sp);

	/*
	 * Let Hyper-V know there is crash data available
	 */
	wrmsrl(HV_X64_MSR_CRASH_CTL, HV_CRASH_CTL_CRASH_NOTIFY);
}
EXPORT_SYMBOL_GPL(hyperv_report_panic);

bool hv_is_hyperv_initialized(void)
{
	union hv_x64_msr_hypercall_contents hypercall_msr;

	/*
	 * Ensure that we're really on Hyper-V, and not a KVM or Xen
	 * emulation of Hyper-V
	 */
	if (x86_hyper_type != X86_HYPER_MS_HYPERV)
		return false;

	/*
	 * Verify that earlier initialization succeeded by checking
	 * that the hypercall page is setup
	 */
	hypercall_msr.as_uint64 = 0;
	rdmsrl(HV_X64_MSR_HYPERCALL, hypercall_msr.as_uint64);

	return hypercall_msr.enable;
}
EXPORT_SYMBOL_GPL(hv_is_hyperv_initialized);

bool hv_is_hibernation_supported(void)
{
	return !hv_root_partition && acpi_sleep_state_supported(ACPI_STATE_S4);
}
EXPORT_SYMBOL_GPL(hv_is_hibernation_supported);

enum hv_isolation_type hv_get_isolation_type(void)
{
	if (!(ms_hyperv.priv_high & HV_ISOLATION))
		return HV_ISOLATION_TYPE_NONE;
	return FIELD_GET(HV_ISOLATION_TYPE, ms_hyperv.isolation_config_b);
}
EXPORT_SYMBOL_GPL(hv_get_isolation_type);

bool hv_is_isolation_supported(void)
{
	return hv_get_isolation_type() != HV_ISOLATION_TYPE_NONE;
}
EXPORT_SYMBOL_GPL(hv_is_isolation_supported);

/* Bit mask of the extended capability to query: see HV_EXT_CAPABILITY_xxx */
bool hv_query_ext_cap(u64 cap_query)
{
	/*
	 * The address of the 'hv_extended_cap' variable will be used as an
	 * output parameter to the hypercall below and so it should be
	 * compatible with 'virt_to_phys'. Which means, it's address should be
	 * directly mapped. Use 'static' to keep it compatible; stack variables
	 * can be virtually mapped, making them imcompatible with
	 * 'virt_to_phys'.
	 * Hypercall input/output addresses should also be 8-byte aligned.
	 */
	static u64 hv_extended_cap __aligned(8);
	static bool hv_extended_cap_queried;
	u64 status;

	/*
	 * Querying extended capabilities is an extended hypercall. Check if the
	 * partition supports extended hypercall, first.
	 */
	if (!(ms_hyperv.priv_high & HV_ENABLE_EXTENDED_HYPERCALLS))
		return false;

	/* Extended capabilities do not change at runtime. */
	if (hv_extended_cap_queried)
		return hv_extended_cap & cap_query;

	status = hv_do_hypercall(HV_EXT_CALL_QUERY_CAPABILITIES, NULL,
				 &hv_extended_cap);

	/*
	 * The query extended capabilities hypercall should not fail under
	 * any normal circumstances. Avoid repeatedly making the hypercall, on
	 * error.
	 */
	hv_extended_cap_queried = true;
	status &= HV_HYPERCALL_RESULT_MASK;
	if (status != HV_STATUS_SUCCESS) {
		pr_err("Hyper-V: Extended query capabilities hypercall failed 0x%llx\n",
		       status);
		return false;
	}

	return hv_extended_cap & cap_query;
}
EXPORT_SYMBOL_GPL(hv_query_ext_cap);
