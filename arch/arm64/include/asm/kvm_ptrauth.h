/* SPDX-License-Identifier: GPL-2.0 */
/* arch/arm64/include/asm/kvm_ptrauth.h: Guest/host ptrauth save/restore
 * Copyright 2019 Arm Limited
 * Authors: Mark Rutland <mark.rutland@arm.com>
 *         Amit Daniel Kachhap <amit.kachhap@arm.com>
 */

#ifndef __ASM_KVM_PTRAUTH_H
#define __ASM_KVM_PTRAUTH_H

#ifdef __ASSEMBLY__

#include <asm/sysreg.h>

#ifdef	CONFIG_ARM64_PTR_AUTH

#define PTRAUTH_REG_OFFSET(x)	(x - CPU_APIAKEYLO_EL1)

/*
 * CPU_AP*_EL1 values exceed immediate offset range (512) for stp
 * instruction so below macros takes CPU_APIAKEYLO_EL1 as base and
 * calculates the offset of the keys from this base to avoid an extra add
 * instruction. These macros assumes the keys offsets follow the order of
 * the sysreg enum in kvm_host.h.
 */
.macro	ptrauth_save_state base, reg1, reg2
	mrs_s	\reg1, SYS_APIAKEYLO_EL1
	mrs_s	\reg2, SYS_APIAKEYHI_EL1
	stp	\reg1, \reg2, [\base, #PTRAUTH_REG_OFFSET(CPU_APIAKEYLO_EL1)]
	mrs_s	\reg1, SYS_APIBKEYLO_EL1
	mrs_s	\reg2, SYS_APIBKEYHI_EL1
	stp	\reg1, \reg2, [\base, #PTRAUTH_REG_OFFSET(CPU_APIBKEYLO_EL1)]
	mrs_s	\reg1, SYS_APDAKEYLO_EL1
	mrs_s	\reg2, SYS_APDAKEYHI_EL1
	stp	\reg1, \reg2, [\base, #PTRAUTH_REG_OFFSET(CPU_APDAKEYLO_EL1)]
	mrs_s	\reg1, SYS_APDBKEYLO_EL1
	mrs_s	\reg2, SYS_APDBKEYHI_EL1
	stp	\reg1, \reg2, [\base, #PTRAUTH_REG_OFFSET(CPU_APDBKEYLO_EL1)]
	mrs_s	\reg1, SYS_APGAKEYLO_EL1
	mrs_s	\reg2, SYS_APGAKEYHI_EL1
	stp	\reg1, \reg2, [\base, #PTRAUTH_REG_OFFSET(CPU_APGAKEYLO_EL1)]
.endm

.macro	ptrauth_restore_state base, reg1, reg2
	ldp	\reg1, \reg2, [\base, #PTRAUTH_REG_OFFSET(CPU_APIAKEYLO_EL1)]
	msr_s	SYS_APIAKEYLO_EL1, \reg1
	msr_s	SYS_APIAKEYHI_EL1, \reg2
	ldp	\reg1, \reg2, [\base, #PTRAUTH_REG_OFFSET(CPU_APIBKEYLO_EL1)]
	msr_s	SYS_APIBKEYLO_EL1, \reg1
	msr_s	SYS_APIBKEYHI_EL1, \reg2
	ldp	\reg1, \reg2, [\base, #PTRAUTH_REG_OFFSET(CPU_APDAKEYLO_EL1)]
	msr_s	SYS_APDAKEYLO_EL1, \reg1
	msr_s	SYS_APDAKEYHI_EL1, \reg2
	ldp	\reg1, \reg2, [\base, #PTRAUTH_REG_OFFSET(CPU_APDBKEYLO_EL1)]
	msr_s	SYS_APDBKEYLO_EL1, \reg1
	msr_s	SYS_APDBKEYHI_EL1, \reg2
	ldp	\reg1, \reg2, [\base, #PTRAUTH_REG_OFFSET(CPU_APGAKEYLO_EL1)]
	msr_s	SYS_APGAKEYLO_EL1, \reg1
	msr_s	SYS_APGAKEYHI_EL1, \reg2
.endm

/*
 * Both ptrauth_switch_to_guest and ptrauth_switch_to_hyp macros will
 * check for the presence ARM64_HAS_ADDRESS_AUTH, which is defined as
 * (ARM64_HAS_ADDRESS_AUTH_ARCH || ARM64_HAS_ADDRESS_AUTH_IMP_DEF) and
 * then proceed ahead with the save/restore of Pointer Authentication
 * key registers if enabled for the guest.
 */
.macro ptrauth_switch_to_guest g_ctxt, reg1, reg2, reg3
alternative_if_not ARM64_HAS_ADDRESS_AUTH
	b	.L__skip_switch\@
alternative_else_nop_endif
	mrs	\reg1, hcr_el2
	and	\reg1, \reg1, #(HCR_API | HCR_APK)
	cbz	\reg1, .L__skip_switch\@
	add	\reg1, \g_ctxt, #CPU_APIAKEYLO_EL1
	ptrauth_restore_state	\reg1, \reg2, \reg3
.L__skip_switch\@:
.endm

.macro ptrauth_switch_to_hyp g_ctxt, h_ctxt, reg1, reg2, reg3
alternative_if_not ARM64_HAS_ADDRESS_AUTH
	b	.L__skip_switch\@
alternative_else_nop_endif
	mrs	\reg1, hcr_el2
	and	\reg1, \reg1, #(HCR_API | HCR_APK)
	cbz	\reg1, .L__skip_switch\@
	add	\reg1, \g_ctxt, #CPU_APIAKEYLO_EL1
	ptrauth_save_state	\reg1, \reg2, \reg3
	add	\reg1, \h_ctxt, #CPU_APIAKEYLO_EL1
	ptrauth_restore_state	\reg1, \reg2, \reg3
	isb
.L__skip_switch\@:
.endm

#else /* !CONFIG_ARM64_PTR_AUTH */
.macro ptrauth_switch_to_guest g_ctxt, reg1, reg2, reg3
.endm
.macro ptrauth_switch_to_hyp g_ctxt, h_ctxt, reg1, reg2, reg3
.endm
#endif /* CONFIG_ARM64_PTR_AUTH */
#endif /* __ASSEMBLY__ */
#endif /* __ASM_KVM_PTRAUTH_H */
