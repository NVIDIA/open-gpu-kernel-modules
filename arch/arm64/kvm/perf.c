// SPDX-License-Identifier: GPL-2.0-only
/*
 * Based on the x86 implementation.
 *
 * Copyright (C) 2012 ARM Ltd.
 * Author: Marc Zyngier <marc.zyngier@arm.com>
 */

#include <linux/perf_event.h>
#include <linux/kvm_host.h>

#include <asm/kvm_emulate.h>

DEFINE_STATIC_KEY_FALSE(kvm_arm_pmu_available);

static int kvm_is_in_guest(void)
{
        return kvm_get_running_vcpu() != NULL;
}

static int kvm_is_user_mode(void)
{
	struct kvm_vcpu *vcpu;

	vcpu = kvm_get_running_vcpu();

	if (vcpu)
		return !vcpu_mode_priv(vcpu);

	return 0;
}

static unsigned long kvm_get_guest_ip(void)
{
	struct kvm_vcpu *vcpu;

	vcpu = kvm_get_running_vcpu();

	if (vcpu)
		return *vcpu_pc(vcpu);

	return 0;
}

static struct perf_guest_info_callbacks kvm_guest_cbs = {
	.is_in_guest	= kvm_is_in_guest,
	.is_user_mode	= kvm_is_user_mode,
	.get_guest_ip	= kvm_get_guest_ip,
};

int kvm_perf_init(void)
{
	if (kvm_pmu_probe_pmuver() != 0xf && !is_protected_kvm_enabled())
		static_branch_enable(&kvm_arm_pmu_available);

	return perf_register_guest_info_callbacks(&kvm_guest_cbs);
}

int kvm_perf_teardown(void)
{
	return perf_unregister_guest_info_callbacks(&kvm_guest_cbs);
}
