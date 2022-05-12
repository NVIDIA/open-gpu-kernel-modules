/* SPDX-License-Identifier: GPL-2.0-only */
/*
 *
 * Copyright SUSE Linux Products GmbH 2010
 *
 * Authors: Alexander Graf <agraf@suse.de>
 */

#ifndef __ASM_KVM_BOOKE_H__
#define __ASM_KVM_BOOKE_H__

#include <linux/types.h>
#include <linux/kvm_host.h>

/*
 * Number of available lpids. Only the low-order 6 bits of LPID rgister are
 * implemented on e500mc+ cores.
 */
#define KVMPPC_NR_LPIDS                        64

#define KVMPPC_INST_EHPRIV		0x7c00021c
#define EHPRIV_OC_SHIFT			11
/* "ehpriv 1" : ehpriv with OC = 1 is used for debug emulation */
#define EHPRIV_OC_DEBUG			1

static inline void kvmppc_set_gpr(struct kvm_vcpu *vcpu, int num, ulong val)
{
	vcpu->arch.regs.gpr[num] = val;
}

static inline ulong kvmppc_get_gpr(struct kvm_vcpu *vcpu, int num)
{
	return vcpu->arch.regs.gpr[num];
}

static inline void kvmppc_set_cr(struct kvm_vcpu *vcpu, u32 val)
{
	vcpu->arch.regs.ccr = val;
}

static inline u32 kvmppc_get_cr(struct kvm_vcpu *vcpu)
{
	return vcpu->arch.regs.ccr;
}

static inline void kvmppc_set_xer(struct kvm_vcpu *vcpu, ulong val)
{
	vcpu->arch.regs.xer = val;
}

static inline ulong kvmppc_get_xer(struct kvm_vcpu *vcpu)
{
	return vcpu->arch.regs.xer;
}

static inline bool kvmppc_need_byteswap(struct kvm_vcpu *vcpu)
{
	/* XXX Would need to check TLB entry */
	return false;
}

static inline void kvmppc_set_ctr(struct kvm_vcpu *vcpu, ulong val)
{
	vcpu->arch.regs.ctr = val;
}

static inline ulong kvmppc_get_ctr(struct kvm_vcpu *vcpu)
{
	return vcpu->arch.regs.ctr;
}

static inline void kvmppc_set_lr(struct kvm_vcpu *vcpu, ulong val)
{
	vcpu->arch.regs.link = val;
}

static inline ulong kvmppc_get_lr(struct kvm_vcpu *vcpu)
{
	return vcpu->arch.regs.link;
}

static inline void kvmppc_set_pc(struct kvm_vcpu *vcpu, ulong val)
{
	vcpu->arch.regs.nip = val;
}

static inline ulong kvmppc_get_pc(struct kvm_vcpu *vcpu)
{
	return vcpu->arch.regs.nip;
}

#ifdef CONFIG_BOOKE
static inline ulong kvmppc_get_fault_dar(struct kvm_vcpu *vcpu)
{
	return vcpu->arch.fault_dear;
}
#endif

static inline bool kvmppc_supports_magic_page(struct kvm_vcpu *vcpu)
{
	/* Magic page is only supported on e500v2 */
#ifdef CONFIG_KVM_E500V2
	return true;
#else
	return false;
#endif
}
#endif /* __ASM_KVM_BOOKE_H__ */
