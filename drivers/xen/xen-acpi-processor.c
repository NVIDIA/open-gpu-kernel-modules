// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright 2012 by Oracle Inc
 * Author: Konrad Rzeszutek Wilk <konrad.wilk@oracle.com>
 *
 * This code borrows ideas from
 * https://lore.kernel.org/lkml/1322673664-14642-6-git-send-email-konrad.wilk@oracle.com
 * so many thanks go to Kevin Tian <kevin.tian@intel.com>
 * and Yu Ke <ke.yu@intel.com>.
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/cpumask.h>
#include <linux/cpufreq.h>
#include <linux/freezer.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/syscore_ops.h>
#include <linux/acpi.h>
#include <acpi/processor.h>
#include <xen/xen.h>
#include <xen/interface/platform.h>
#include <asm/xen/hypercall.h>

static int no_hypercall;
MODULE_PARM_DESC(off, "Inhibit the hypercall.");
module_param_named(off, no_hypercall, int, 0400);

/*
 * Note: Do not convert the acpi_id* below to cpumask_var_t or use cpumask_bit
 * - as those shrink to nr_cpu_bits (which is dependent on possible_cpu), which
 * can be less than what we want to put in. Instead use the 'nr_acpi_bits'
 * which is dynamically computed based on the MADT or x2APIC table.
 */
static unsigned int nr_acpi_bits;
/* Mutex to protect the acpi_ids_done - for CPU hotplug use. */
static DEFINE_MUTEX(acpi_ids_mutex);
/* Which ACPI ID we have processed from 'struct acpi_processor'. */
static unsigned long *acpi_ids_done;
/* Which ACPI ID exist in the SSDT/DSDT processor definitions. */
static unsigned long *acpi_id_present;
/* And if there is an _CST definition (or a PBLK) for the ACPI IDs */
static unsigned long *acpi_id_cst_present;
/* Which ACPI P-State dependencies for a enumerated processor */
static struct acpi_psd_package *acpi_psd;

static int push_cxx_to_hypervisor(struct acpi_processor *_pr)
{
	struct xen_platform_op op = {
		.cmd			= XENPF_set_processor_pminfo,
		.interface_version	= XENPF_INTERFACE_VERSION,
		.u.set_pminfo.id	= _pr->acpi_id,
		.u.set_pminfo.type	= XEN_PM_CX,
	};
	struct xen_processor_cx *dst_cx, *dst_cx_states = NULL;
	struct acpi_processor_cx *cx;
	unsigned int i, ok;
	int ret = 0;

	dst_cx_states = kcalloc(_pr->power.count,
				sizeof(struct xen_processor_cx), GFP_KERNEL);
	if (!dst_cx_states)
		return -ENOMEM;

	for (ok = 0, i = 1; i <= _pr->power.count; i++) {
		cx = &_pr->power.states[i];
		if (!cx->valid)
			continue;

		dst_cx = &(dst_cx_states[ok++]);

		dst_cx->reg.space_id = ACPI_ADR_SPACE_SYSTEM_IO;
		if (cx->entry_method == ACPI_CSTATE_SYSTEMIO) {
			dst_cx->reg.bit_width = 8;
			dst_cx->reg.bit_offset = 0;
			dst_cx->reg.access_size = 1;
		} else {
			dst_cx->reg.space_id = ACPI_ADR_SPACE_FIXED_HARDWARE;
			if (cx->entry_method == ACPI_CSTATE_FFH) {
				/* NATIVE_CSTATE_BEYOND_HALT */
				dst_cx->reg.bit_offset = 2;
				dst_cx->reg.bit_width = 1; /* VENDOR_INTEL */
			}
			dst_cx->reg.access_size = 0;
		}
		dst_cx->reg.address = cx->address;

		dst_cx->type = cx->type;
		dst_cx->latency = cx->latency;

		dst_cx->dpcnt = 0;
		set_xen_guest_handle(dst_cx->dp, NULL);
	}
	if (!ok) {
		pr_debug("No _Cx for ACPI CPU %u\n", _pr->acpi_id);
		kfree(dst_cx_states);
		return -EINVAL;
	}
	op.u.set_pminfo.power.count = ok;
	op.u.set_pminfo.power.flags.bm_control = _pr->flags.bm_control;
	op.u.set_pminfo.power.flags.bm_check = _pr->flags.bm_check;
	op.u.set_pminfo.power.flags.has_cst = _pr->flags.has_cst;
	op.u.set_pminfo.power.flags.power_setup_done =
		_pr->flags.power_setup_done;

	set_xen_guest_handle(op.u.set_pminfo.power.states, dst_cx_states);

	if (!no_hypercall)
		ret = HYPERVISOR_platform_op(&op);

	if (!ret) {
		pr_debug("ACPI CPU%u - C-states uploaded.\n", _pr->acpi_id);
		for (i = 1; i <= _pr->power.count; i++) {
			cx = &_pr->power.states[i];
			if (!cx->valid)
				continue;
			pr_debug("     C%d: %s %d uS\n",
				 cx->type, cx->desc, (u32)cx->latency);
		}
	} else if ((ret != -EINVAL) && (ret != -ENOSYS))
		/* EINVAL means the ACPI ID is incorrect - meaning the ACPI
		 * table is referencing a non-existing CPU - which can happen
		 * with broken ACPI tables. */
		pr_err("(CX): Hypervisor error (%d) for ACPI CPU%u\n",
		       ret, _pr->acpi_id);

	kfree(dst_cx_states);

	return ret;
}
static struct xen_processor_px *
xen_copy_pss_data(struct acpi_processor *_pr,
		  struct xen_processor_performance *dst_perf)
{
	struct xen_processor_px *dst_states = NULL;
	unsigned int i;

	BUILD_BUG_ON(sizeof(struct xen_processor_px) !=
		     sizeof(struct acpi_processor_px));

	dst_states = kcalloc(_pr->performance->state_count,
			     sizeof(struct xen_processor_px), GFP_KERNEL);
	if (!dst_states)
		return ERR_PTR(-ENOMEM);

	dst_perf->state_count = _pr->performance->state_count;
	for (i = 0; i < _pr->performance->state_count; i++) {
		/* Fortunatly for us, they are both the same size */
		memcpy(&(dst_states[i]), &(_pr->performance->states[i]),
		       sizeof(struct acpi_processor_px));
	}
	return dst_states;
}
static int xen_copy_psd_data(struct acpi_processor *_pr,
			     struct xen_processor_performance *dst)
{
	struct acpi_psd_package *pdomain;

	BUILD_BUG_ON(sizeof(struct xen_psd_package) !=
		     sizeof(struct acpi_psd_package));

	/* This information is enumerated only if acpi_processor_preregister_performance
	 * has been called.
	 */
	dst->shared_type = _pr->performance->shared_type;

	pdomain = &(_pr->performance->domain_info);

	/* 'acpi_processor_preregister_performance' does not parse if the
	 * num_processors <= 1, but Xen still requires it. Do it manually here.
	 */
	if (pdomain->num_processors <= 1) {
		if (pdomain->coord_type == DOMAIN_COORD_TYPE_SW_ALL)
			dst->shared_type = CPUFREQ_SHARED_TYPE_ALL;
		else if (pdomain->coord_type == DOMAIN_COORD_TYPE_HW_ALL)
			dst->shared_type = CPUFREQ_SHARED_TYPE_HW;
		else if (pdomain->coord_type == DOMAIN_COORD_TYPE_SW_ANY)
			dst->shared_type = CPUFREQ_SHARED_TYPE_ANY;

	}
	memcpy(&(dst->domain_info), pdomain, sizeof(struct acpi_psd_package));
	return 0;
}
static int xen_copy_pct_data(struct acpi_pct_register *pct,
			     struct xen_pct_register *dst_pct)
{
	/* It would be nice if you could just do 'memcpy(pct, dst_pct') but
	 * sadly the Xen structure did not have the proper padding so the
	 * descriptor field takes two (dst_pct) bytes instead of one (pct).
	 */
	dst_pct->descriptor = pct->descriptor;
	dst_pct->length = pct->length;
	dst_pct->space_id = pct->space_id;
	dst_pct->bit_width = pct->bit_width;
	dst_pct->bit_offset = pct->bit_offset;
	dst_pct->reserved = pct->reserved;
	dst_pct->address = pct->address;
	return 0;
}
static int push_pxx_to_hypervisor(struct acpi_processor *_pr)
{
	int ret = 0;
	struct xen_platform_op op = {
		.cmd			= XENPF_set_processor_pminfo,
		.interface_version	= XENPF_INTERFACE_VERSION,
		.u.set_pminfo.id	= _pr->acpi_id,
		.u.set_pminfo.type	= XEN_PM_PX,
	};
	struct xen_processor_performance *dst_perf;
	struct xen_processor_px *dst_states = NULL;

	dst_perf = &op.u.set_pminfo.perf;

	dst_perf->platform_limit = _pr->performance_platform_limit;
	dst_perf->flags |= XEN_PX_PPC;
	xen_copy_pct_data(&(_pr->performance->control_register),
			  &dst_perf->control_register);
	xen_copy_pct_data(&(_pr->performance->status_register),
			  &dst_perf->status_register);
	dst_perf->flags |= XEN_PX_PCT;
	dst_states = xen_copy_pss_data(_pr, dst_perf);
	if (!IS_ERR_OR_NULL(dst_states)) {
		set_xen_guest_handle(dst_perf->states, dst_states);
		dst_perf->flags |= XEN_PX_PSS;
	}
	if (!xen_copy_psd_data(_pr, dst_perf))
		dst_perf->flags |= XEN_PX_PSD;

	if (dst_perf->flags != (XEN_PX_PSD | XEN_PX_PSS | XEN_PX_PCT | XEN_PX_PPC)) {
		pr_warn("ACPI CPU%u missing some P-state data (%x), skipping\n",
			_pr->acpi_id, dst_perf->flags);
		ret = -ENODEV;
		goto err_free;
	}

	if (!no_hypercall)
		ret = HYPERVISOR_platform_op(&op);

	if (!ret) {
		struct acpi_processor_performance *perf;
		unsigned int i;

		perf = _pr->performance;
		pr_debug("ACPI CPU%u - P-states uploaded.\n", _pr->acpi_id);
		for (i = 0; i < perf->state_count; i++) {
			pr_debug("     %cP%d: %d MHz, %d mW, %d uS\n",
			(i == perf->state ? '*' : ' '), i,
			(u32) perf->states[i].core_frequency,
			(u32) perf->states[i].power,
			(u32) perf->states[i].transition_latency);
		}
	} else if ((ret != -EINVAL) && (ret != -ENOSYS))
		/* EINVAL means the ACPI ID is incorrect - meaning the ACPI
		 * table is referencing a non-existing CPU - which can happen
		 * with broken ACPI tables. */
		pr_warn("(_PXX): Hypervisor error (%d) for ACPI CPU%u\n",
			ret, _pr->acpi_id);
err_free:
	if (!IS_ERR_OR_NULL(dst_states))
		kfree(dst_states);

	return ret;
}
static int upload_pm_data(struct acpi_processor *_pr)
{
	int err = 0;

	mutex_lock(&acpi_ids_mutex);
	if (__test_and_set_bit(_pr->acpi_id, acpi_ids_done)) {
		mutex_unlock(&acpi_ids_mutex);
		return -EBUSY;
	}
	if (_pr->flags.power)
		err = push_cxx_to_hypervisor(_pr);

	if (_pr->performance && _pr->performance->states)
		err |= push_pxx_to_hypervisor(_pr);

	mutex_unlock(&acpi_ids_mutex);
	return err;
}
static unsigned int __init get_max_acpi_id(void)
{
	struct xenpf_pcpuinfo *info;
	struct xen_platform_op op = {
		.cmd = XENPF_get_cpuinfo,
		.interface_version = XENPF_INTERFACE_VERSION,
	};
	int ret = 0;
	unsigned int i, last_cpu, max_acpi_id = 0;

	info = &op.u.pcpu_info;
	info->xen_cpuid = 0;

	ret = HYPERVISOR_platform_op(&op);
	if (ret)
		return NR_CPUS;

	/* The max_present is the same irregardless of the xen_cpuid */
	last_cpu = op.u.pcpu_info.max_present;
	for (i = 0; i <= last_cpu; i++) {
		info->xen_cpuid = i;
		ret = HYPERVISOR_platform_op(&op);
		if (ret)
			continue;
		max_acpi_id = max(info->acpi_id, max_acpi_id);
	}
	max_acpi_id *= 2; /* Slack for CPU hotplug support. */
	pr_debug("Max ACPI ID: %u\n", max_acpi_id);
	return max_acpi_id;
}
/*
 * The read_acpi_id and check_acpi_ids are there to support the Xen
 * oddity of virtual CPUs != physical CPUs in the initial domain.
 * The user can supply 'xen_max_vcpus=X' on the Xen hypervisor line
 * which will band the amount of CPUs the initial domain can see.
 * In general that is OK, except it plays havoc with any of the
 * for_each_[present|online]_cpu macros which are banded to the virtual
 * CPU amount.
 */
static acpi_status
read_acpi_id(acpi_handle handle, u32 lvl, void *context, void **rv)
{
	u32 acpi_id;
	acpi_status status;
	acpi_object_type acpi_type;
	unsigned long long tmp;
	union acpi_object object = { 0 };
	struct acpi_buffer buffer = { sizeof(union acpi_object), &object };
	acpi_io_address pblk = 0;

	status = acpi_get_type(handle, &acpi_type);
	if (ACPI_FAILURE(status))
		return AE_OK;

	switch (acpi_type) {
	case ACPI_TYPE_PROCESSOR:
		status = acpi_evaluate_object(handle, NULL, NULL, &buffer);
		if (ACPI_FAILURE(status))
			return AE_OK;
		acpi_id = object.processor.proc_id;
		pblk = object.processor.pblk_address;
		break;
	case ACPI_TYPE_DEVICE:
		status = acpi_evaluate_integer(handle, "_UID", NULL, &tmp);
		if (ACPI_FAILURE(status))
			return AE_OK;
		acpi_id = tmp;
		break;
	default:
		return AE_OK;
	}
	if (invalid_phys_cpuid(acpi_get_phys_id(handle,
						acpi_type == ACPI_TYPE_DEVICE,
						acpi_id))) {
		pr_debug("CPU with ACPI ID %u is unavailable\n", acpi_id);
		return AE_OK;
	}
	/* There are more ACPI Processor objects than in x2APIC or MADT.
	 * This can happen with incorrect ACPI SSDT declerations. */
	if (acpi_id >= nr_acpi_bits) {
		pr_debug("max acpi id %u, trying to set %u\n",
			 nr_acpi_bits - 1, acpi_id);
		return AE_OK;
	}
	/* OK, There is a ACPI Processor object */
	__set_bit(acpi_id, acpi_id_present);

	pr_debug("ACPI CPU%u w/ PBLK:0x%lx\n", acpi_id, (unsigned long)pblk);

	/* It has P-state dependencies */
	if (!acpi_processor_get_psd(handle, &acpi_psd[acpi_id])) {
		pr_debug("ACPI CPU%u w/ PST:coord_type = %llu domain = %llu\n",
			 acpi_id, acpi_psd[acpi_id].coord_type,
			 acpi_psd[acpi_id].domain);
	}

	status = acpi_evaluate_object(handle, "_CST", NULL, &buffer);
	if (ACPI_FAILURE(status)) {
		if (!pblk)
			return AE_OK;
	}
	/* .. and it has a C-state */
	__set_bit(acpi_id, acpi_id_cst_present);

	return AE_OK;
}
static int check_acpi_ids(struct acpi_processor *pr_backup)
{

	if (!pr_backup)
		return -ENODEV;

	if (acpi_id_present && acpi_id_cst_present)
		/* OK, done this once .. skip to uploading */
		goto upload;

	/* All online CPUs have been processed at this stage. Now verify
	 * whether in fact "online CPUs" == physical CPUs.
	 */
	acpi_id_present = bitmap_zalloc(nr_acpi_bits, GFP_KERNEL);
	if (!acpi_id_present)
		return -ENOMEM;

	acpi_id_cst_present = bitmap_zalloc(nr_acpi_bits, GFP_KERNEL);
	if (!acpi_id_cst_present) {
		bitmap_free(acpi_id_present);
		return -ENOMEM;
	}

	acpi_psd = kcalloc(nr_acpi_bits, sizeof(struct acpi_psd_package),
			   GFP_KERNEL);
	if (!acpi_psd) {
		bitmap_free(acpi_id_present);
		bitmap_free(acpi_id_cst_present);
		return -ENOMEM;
	}

	acpi_walk_namespace(ACPI_TYPE_PROCESSOR, ACPI_ROOT_OBJECT,
			    ACPI_UINT32_MAX,
			    read_acpi_id, NULL, NULL, NULL);
	acpi_get_devices(ACPI_PROCESSOR_DEVICE_HID, read_acpi_id, NULL, NULL);

upload:
	if (!bitmap_equal(acpi_id_present, acpi_ids_done, nr_acpi_bits)) {
		unsigned int i;
		for_each_set_bit(i, acpi_id_present, nr_acpi_bits) {
			pr_backup->acpi_id = i;
			/* Mask out C-states if there are no _CST or PBLK */
			pr_backup->flags.power = test_bit(i, acpi_id_cst_present);
			/* num_entries is non-zero if we evaluated _PSD */
			if (acpi_psd[i].num_entries) {
				memcpy(&pr_backup->performance->domain_info,
				       &acpi_psd[i],
				       sizeof(struct acpi_psd_package));
			}
			(void)upload_pm_data(pr_backup);
		}
	}

	return 0;
}

/* acpi_perf_data is a pointer to percpu data. */
static struct acpi_processor_performance __percpu *acpi_perf_data;

static void free_acpi_perf_data(void)
{
	unsigned int i;

	/* Freeing a NULL pointer is OK, and alloc_percpu zeroes. */
	for_each_possible_cpu(i)
		free_cpumask_var(per_cpu_ptr(acpi_perf_data, i)
				 ->shared_cpu_map);
	free_percpu(acpi_perf_data);
}

static int xen_upload_processor_pm_data(void)
{
	struct acpi_processor *pr_backup = NULL;
	unsigned int i;
	int rc = 0;

	pr_info("Uploading Xen processor PM info\n");

	for_each_possible_cpu(i) {
		struct acpi_processor *_pr;
		_pr = per_cpu(processors, i /* APIC ID */);
		if (!_pr)
			continue;

		if (!pr_backup) {
			pr_backup = kzalloc(sizeof(struct acpi_processor), GFP_KERNEL);
			if (pr_backup)
				memcpy(pr_backup, _pr, sizeof(struct acpi_processor));
		}
		(void)upload_pm_data(_pr);
	}

	rc = check_acpi_ids(pr_backup);
	kfree(pr_backup);

	return rc;
}

static void xen_acpi_processor_resume_worker(struct work_struct *dummy)
{
	int rc;

	bitmap_zero(acpi_ids_done, nr_acpi_bits);

	rc = xen_upload_processor_pm_data();
	if (rc != 0)
		pr_info("ACPI data upload failed, error = %d\n", rc);
}

static void xen_acpi_processor_resume(void)
{
	static DECLARE_WORK(wq, xen_acpi_processor_resume_worker);

	/*
	 * xen_upload_processor_pm_data() calls non-atomic code.
	 * However, the context for xen_acpi_processor_resume is syscore
	 * with only the boot CPU online and in an atomic context.
	 *
	 * So defer the upload for some point safer.
	 */
	schedule_work(&wq);
}

static struct syscore_ops xap_syscore_ops = {
	.resume	= xen_acpi_processor_resume,
};

static int __init xen_acpi_processor_init(void)
{
	unsigned int i;
	int rc;

	if (!xen_initial_domain())
		return -ENODEV;

	nr_acpi_bits = get_max_acpi_id() + 1;
	acpi_ids_done = bitmap_zalloc(nr_acpi_bits, GFP_KERNEL);
	if (!acpi_ids_done)
		return -ENOMEM;

	acpi_perf_data = alloc_percpu(struct acpi_processor_performance);
	if (!acpi_perf_data) {
		pr_debug("Memory allocation error for acpi_perf_data\n");
		bitmap_free(acpi_ids_done);
		return -ENOMEM;
	}
	for_each_possible_cpu(i) {
		if (!zalloc_cpumask_var_node(
			&per_cpu_ptr(acpi_perf_data, i)->shared_cpu_map,
			GFP_KERNEL, cpu_to_node(i))) {
			rc = -ENOMEM;
			goto err_out;
		}
	}

	/* Do initialization in ACPI core. It is OK to fail here. */
	(void)acpi_processor_preregister_performance(acpi_perf_data);

	for_each_possible_cpu(i) {
		struct acpi_processor *pr;
		struct acpi_processor_performance *perf;

		pr = per_cpu(processors, i);
		perf = per_cpu_ptr(acpi_perf_data, i);
		if (!pr)
			continue;

		pr->performance = perf;
		rc = acpi_processor_get_performance_info(pr);
		if (rc)
			goto err_out;
	}

	rc = xen_upload_processor_pm_data();
	if (rc)
		goto err_unregister;

	register_syscore_ops(&xap_syscore_ops);

	return 0;
err_unregister:
	for_each_possible_cpu(i)
		acpi_processor_unregister_performance(i);

err_out:
	/* Freeing a NULL pointer is OK: alloc_percpu zeroes. */
	free_acpi_perf_data();
	bitmap_free(acpi_ids_done);
	return rc;
}
static void __exit xen_acpi_processor_exit(void)
{
	int i;

	unregister_syscore_ops(&xap_syscore_ops);
	bitmap_free(acpi_ids_done);
	bitmap_free(acpi_id_present);
	bitmap_free(acpi_id_cst_present);
	kfree(acpi_psd);
	for_each_possible_cpu(i)
		acpi_processor_unregister_performance(i);

	free_acpi_perf_data();
}

MODULE_AUTHOR("Konrad Rzeszutek Wilk <konrad.wilk@oracle.com>");
MODULE_DESCRIPTION("Xen ACPI Processor P-states (and Cx) driver which uploads PM data to Xen hypervisor");
MODULE_LICENSE("GPL");

/* We want to be loaded before the CPU freq scaling drivers are loaded.
 * They are loaded in late_initcall. */
device_initcall(xen_acpi_processor_init);
module_exit(xen_acpi_processor_exit);
