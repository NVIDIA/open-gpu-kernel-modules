// SPDX-License-Identifier: GPL-2.0-only
/*
 *
 * Copyright IBM Corp. 2008
 * Copyright 2011 Freescale Semiconductor, Inc.
 *
 * Authors: Hollis Blanchard <hollisb@us.ibm.com>
 */

#include <linux/kvm_host.h>
#include <asm/disassemble.h>

#include "booke.h"

#define OP_19_XOP_RFI     50
#define OP_19_XOP_RFCI    51
#define OP_19_XOP_RFDI    39

#define OP_31_XOP_MFMSR   83
#define OP_31_XOP_WRTEE   131
#define OP_31_XOP_MTMSR   146
#define OP_31_XOP_WRTEEI  163

static void kvmppc_emul_rfi(struct kvm_vcpu *vcpu)
{
	vcpu->arch.regs.nip = vcpu->arch.shared->srr0;
	kvmppc_set_msr(vcpu, vcpu->arch.shared->srr1);
}

static void kvmppc_emul_rfdi(struct kvm_vcpu *vcpu)
{
	vcpu->arch.regs.nip = vcpu->arch.dsrr0;
	kvmppc_set_msr(vcpu, vcpu->arch.dsrr1);
}

static void kvmppc_emul_rfci(struct kvm_vcpu *vcpu)
{
	vcpu->arch.regs.nip = vcpu->arch.csrr0;
	kvmppc_set_msr(vcpu, vcpu->arch.csrr1);
}

int kvmppc_booke_emulate_op(struct kvm_vcpu *vcpu,
                            unsigned int inst, int *advance)
{
	int emulated = EMULATE_DONE;
	int rs = get_rs(inst);
	int rt = get_rt(inst);

	switch (get_op(inst)) {
	case 19:
		switch (get_xop(inst)) {
		case OP_19_XOP_RFI:
			kvmppc_emul_rfi(vcpu);
			kvmppc_set_exit_type(vcpu, EMULATED_RFI_EXITS);
			*advance = 0;
			break;

		case OP_19_XOP_RFCI:
			kvmppc_emul_rfci(vcpu);
			kvmppc_set_exit_type(vcpu, EMULATED_RFCI_EXITS);
			*advance = 0;
			break;

		case OP_19_XOP_RFDI:
			kvmppc_emul_rfdi(vcpu);
			kvmppc_set_exit_type(vcpu, EMULATED_RFDI_EXITS);
			*advance = 0;
			break;

		default:
			emulated = EMULATE_FAIL;
			break;
		}
		break;

	case 31:
		switch (get_xop(inst)) {

		case OP_31_XOP_MFMSR:
			kvmppc_set_gpr(vcpu, rt, vcpu->arch.shared->msr);
			kvmppc_set_exit_type(vcpu, EMULATED_MFMSR_EXITS);
			break;

		case OP_31_XOP_MTMSR:
			kvmppc_set_exit_type(vcpu, EMULATED_MTMSR_EXITS);
			kvmppc_set_msr(vcpu, kvmppc_get_gpr(vcpu, rs));
			break;

		case OP_31_XOP_WRTEE:
			vcpu->arch.shared->msr = (vcpu->arch.shared->msr & ~MSR_EE)
					| (kvmppc_get_gpr(vcpu, rs) & MSR_EE);
			kvmppc_set_exit_type(vcpu, EMULATED_WRTEE_EXITS);
			break;

		case OP_31_XOP_WRTEEI:
			vcpu->arch.shared->msr = (vcpu->arch.shared->msr & ~MSR_EE)
							 | (inst & MSR_EE);
			kvmppc_set_exit_type(vcpu, EMULATED_WRTEE_EXITS);
			break;

		default:
			emulated = EMULATE_FAIL;
		}

		break;

	default:
		emulated = EMULATE_FAIL;
	}

	return emulated;
}

/*
 * NOTE: some of these registers are not emulated on BOOKE_HV (GS-mode).
 * Their backing store is in real registers, and these functions
 * will return the wrong result if called for them in another context
 * (such as debugging).
 */
int kvmppc_booke_emulate_mtspr(struct kvm_vcpu *vcpu, int sprn, ulong spr_val)
{
	int emulated = EMULATE_DONE;
	bool debug_inst = false;

	switch (sprn) {
	case SPRN_DEAR:
		vcpu->arch.shared->dar = spr_val;
		break;
	case SPRN_ESR:
		vcpu->arch.shared->esr = spr_val;
		break;
	case SPRN_CSRR0:
		vcpu->arch.csrr0 = spr_val;
		break;
	case SPRN_CSRR1:
		vcpu->arch.csrr1 = spr_val;
		break;
	case SPRN_DSRR0:
		vcpu->arch.dsrr0 = spr_val;
		break;
	case SPRN_DSRR1:
		vcpu->arch.dsrr1 = spr_val;
		break;
	case SPRN_IAC1:
		/*
		 * If userspace is debugging guest then guest
		 * can not access debug registers.
		 */
		if (vcpu->guest_debug)
			break;

		debug_inst = true;
		vcpu->arch.dbg_reg.iac1 = spr_val;
		break;
	case SPRN_IAC2:
		/*
		 * If userspace is debugging guest then guest
		 * can not access debug registers.
		 */
		if (vcpu->guest_debug)
			break;

		debug_inst = true;
		vcpu->arch.dbg_reg.iac2 = spr_val;
		break;
#if CONFIG_PPC_ADV_DEBUG_IACS > 2
	case SPRN_IAC3:
		/*
		 * If userspace is debugging guest then guest
		 * can not access debug registers.
		 */
		if (vcpu->guest_debug)
			break;

		debug_inst = true;
		vcpu->arch.dbg_reg.iac3 = spr_val;
		break;
	case SPRN_IAC4:
		/*
		 * If userspace is debugging guest then guest
		 * can not access debug registers.
		 */
		if (vcpu->guest_debug)
			break;

		debug_inst = true;
		vcpu->arch.dbg_reg.iac4 = spr_val;
		break;
#endif
	case SPRN_DAC1:
		/*
		 * If userspace is debugging guest then guest
		 * can not access debug registers.
		 */
		if (vcpu->guest_debug)
			break;

		debug_inst = true;
		vcpu->arch.dbg_reg.dac1 = spr_val;
		break;
	case SPRN_DAC2:
		/*
		 * If userspace is debugging guest then guest
		 * can not access debug registers.
		 */
		if (vcpu->guest_debug)
			break;

		debug_inst = true;
		vcpu->arch.dbg_reg.dac2 = spr_val;
		break;
	case SPRN_DBCR0:
		/*
		 * If userspace is debugging guest then guest
		 * can not access debug registers.
		 */
		if (vcpu->guest_debug)
			break;

		debug_inst = true;
		spr_val &= (DBCR0_IDM | DBCR0_IC | DBCR0_BT | DBCR0_TIE |
			DBCR0_IAC1 | DBCR0_IAC2 | DBCR0_IAC3 | DBCR0_IAC4  |
			DBCR0_DAC1R | DBCR0_DAC1W | DBCR0_DAC2R | DBCR0_DAC2W);

		vcpu->arch.dbg_reg.dbcr0 = spr_val;
		break;
	case SPRN_DBCR1:
		/*
		 * If userspace is debugging guest then guest
		 * can not access debug registers.
		 */
		if (vcpu->guest_debug)
			break;

		debug_inst = true;
		vcpu->arch.dbg_reg.dbcr1 = spr_val;
		break;
	case SPRN_DBCR2:
		/*
		 * If userspace is debugging guest then guest
		 * can not access debug registers.
		 */
		if (vcpu->guest_debug)
			break;

		debug_inst = true;
		vcpu->arch.dbg_reg.dbcr2 = spr_val;
		break;
	case SPRN_DBSR:
		/*
		 * If userspace is debugging guest then guest
		 * can not access debug registers.
		 */
		if (vcpu->guest_debug)
			break;

		vcpu->arch.dbsr &= ~spr_val;
		if (!(vcpu->arch.dbsr & ~DBSR_IDE))
			kvmppc_core_dequeue_debug(vcpu);
		break;
	case SPRN_TSR:
		kvmppc_clr_tsr_bits(vcpu, spr_val);
		break;
	case SPRN_TCR:
		/*
		 * WRC is a 2-bit field that is supposed to preserve its
		 * value once written to non-zero.
		 */
		if (vcpu->arch.tcr & TCR_WRC_MASK) {
			spr_val &= ~TCR_WRC_MASK;
			spr_val |= vcpu->arch.tcr & TCR_WRC_MASK;
		}
		kvmppc_set_tcr(vcpu, spr_val);
		break;

	case SPRN_DECAR:
		vcpu->arch.decar = spr_val;
		break;
	/*
	 * Note: SPRG4-7 are user-readable.
	 * These values are loaded into the real SPRGs when resuming the
	 * guest (PR-mode only).
	 */
	case SPRN_SPRG4:
		kvmppc_set_sprg4(vcpu, spr_val);
		break;
	case SPRN_SPRG5:
		kvmppc_set_sprg5(vcpu, spr_val);
		break;
	case SPRN_SPRG6:
		kvmppc_set_sprg6(vcpu, spr_val);
		break;
	case SPRN_SPRG7:
		kvmppc_set_sprg7(vcpu, spr_val);
		break;

	case SPRN_IVPR:
		vcpu->arch.ivpr = spr_val;
#ifdef CONFIG_KVM_BOOKE_HV
		mtspr(SPRN_GIVPR, spr_val);
#endif
		break;
	case SPRN_IVOR0:
		vcpu->arch.ivor[BOOKE_IRQPRIO_CRITICAL] = spr_val;
		break;
	case SPRN_IVOR1:
		vcpu->arch.ivor[BOOKE_IRQPRIO_MACHINE_CHECK] = spr_val;
		break;
	case SPRN_IVOR2:
		vcpu->arch.ivor[BOOKE_IRQPRIO_DATA_STORAGE] = spr_val;
#ifdef CONFIG_KVM_BOOKE_HV
		mtspr(SPRN_GIVOR2, spr_val);
#endif
		break;
	case SPRN_IVOR3:
		vcpu->arch.ivor[BOOKE_IRQPRIO_INST_STORAGE] = spr_val;
		break;
	case SPRN_IVOR4:
		vcpu->arch.ivor[BOOKE_IRQPRIO_EXTERNAL] = spr_val;
		break;
	case SPRN_IVOR5:
		vcpu->arch.ivor[BOOKE_IRQPRIO_ALIGNMENT] = spr_val;
		break;
	case SPRN_IVOR6:
		vcpu->arch.ivor[BOOKE_IRQPRIO_PROGRAM] = spr_val;
		break;
	case SPRN_IVOR7:
		vcpu->arch.ivor[BOOKE_IRQPRIO_FP_UNAVAIL] = spr_val;
		break;
	case SPRN_IVOR8:
		vcpu->arch.ivor[BOOKE_IRQPRIO_SYSCALL] = spr_val;
#ifdef CONFIG_KVM_BOOKE_HV
		mtspr(SPRN_GIVOR8, spr_val);
#endif
		break;
	case SPRN_IVOR9:
		vcpu->arch.ivor[BOOKE_IRQPRIO_AP_UNAVAIL] = spr_val;
		break;
	case SPRN_IVOR10:
		vcpu->arch.ivor[BOOKE_IRQPRIO_DECREMENTER] = spr_val;
		break;
	case SPRN_IVOR11:
		vcpu->arch.ivor[BOOKE_IRQPRIO_FIT] = spr_val;
		break;
	case SPRN_IVOR12:
		vcpu->arch.ivor[BOOKE_IRQPRIO_WATCHDOG] = spr_val;
		break;
	case SPRN_IVOR13:
		vcpu->arch.ivor[BOOKE_IRQPRIO_DTLB_MISS] = spr_val;
		break;
	case SPRN_IVOR14:
		vcpu->arch.ivor[BOOKE_IRQPRIO_ITLB_MISS] = spr_val;
		break;
	case SPRN_IVOR15:
		vcpu->arch.ivor[BOOKE_IRQPRIO_DEBUG] = spr_val;
		break;
	case SPRN_MCSR:
		vcpu->arch.mcsr &= ~spr_val;
		break;
#if defined(CONFIG_64BIT)
	case SPRN_EPCR:
		kvmppc_set_epcr(vcpu, spr_val);
#ifdef CONFIG_KVM_BOOKE_HV
		mtspr(SPRN_EPCR, vcpu->arch.shadow_epcr);
#endif
		break;
#endif
	default:
		emulated = EMULATE_FAIL;
	}

	if (debug_inst) {
		current->thread.debug = vcpu->arch.dbg_reg;
		switch_booke_debug_regs(&vcpu->arch.dbg_reg);
	}
	return emulated;
}

int kvmppc_booke_emulate_mfspr(struct kvm_vcpu *vcpu, int sprn, ulong *spr_val)
{
	int emulated = EMULATE_DONE;

	switch (sprn) {
	case SPRN_IVPR:
		*spr_val = vcpu->arch.ivpr;
		break;
	case SPRN_DEAR:
		*spr_val = vcpu->arch.shared->dar;
		break;
	case SPRN_ESR:
		*spr_val = vcpu->arch.shared->esr;
		break;
	case SPRN_EPR:
		*spr_val = vcpu->arch.epr;
		break;
	case SPRN_CSRR0:
		*spr_val = vcpu->arch.csrr0;
		break;
	case SPRN_CSRR1:
		*spr_val = vcpu->arch.csrr1;
		break;
	case SPRN_DSRR0:
		*spr_val = vcpu->arch.dsrr0;
		break;
	case SPRN_DSRR1:
		*spr_val = vcpu->arch.dsrr1;
		break;
	case SPRN_IAC1:
		*spr_val = vcpu->arch.dbg_reg.iac1;
		break;
	case SPRN_IAC2:
		*spr_val = vcpu->arch.dbg_reg.iac2;
		break;
#if CONFIG_PPC_ADV_DEBUG_IACS > 2
	case SPRN_IAC3:
		*spr_val = vcpu->arch.dbg_reg.iac3;
		break;
	case SPRN_IAC4:
		*spr_val = vcpu->arch.dbg_reg.iac4;
		break;
#endif
	case SPRN_DAC1:
		*spr_val = vcpu->arch.dbg_reg.dac1;
		break;
	case SPRN_DAC2:
		*spr_val = vcpu->arch.dbg_reg.dac2;
		break;
	case SPRN_DBCR0:
		*spr_val = vcpu->arch.dbg_reg.dbcr0;
		if (vcpu->guest_debug)
			*spr_val = *spr_val | DBCR0_EDM;
		break;
	case SPRN_DBCR1:
		*spr_val = vcpu->arch.dbg_reg.dbcr1;
		break;
	case SPRN_DBCR2:
		*spr_val = vcpu->arch.dbg_reg.dbcr2;
		break;
	case SPRN_DBSR:
		*spr_val = vcpu->arch.dbsr;
		break;
	case SPRN_TSR:
		*spr_val = vcpu->arch.tsr;
		break;
	case SPRN_TCR:
		*spr_val = vcpu->arch.tcr;
		break;

	case SPRN_IVOR0:
		*spr_val = vcpu->arch.ivor[BOOKE_IRQPRIO_CRITICAL];
		break;
	case SPRN_IVOR1:
		*spr_val = vcpu->arch.ivor[BOOKE_IRQPRIO_MACHINE_CHECK];
		break;
	case SPRN_IVOR2:
		*spr_val = vcpu->arch.ivor[BOOKE_IRQPRIO_DATA_STORAGE];
		break;
	case SPRN_IVOR3:
		*spr_val = vcpu->arch.ivor[BOOKE_IRQPRIO_INST_STORAGE];
		break;
	case SPRN_IVOR4:
		*spr_val = vcpu->arch.ivor[BOOKE_IRQPRIO_EXTERNAL];
		break;
	case SPRN_IVOR5:
		*spr_val = vcpu->arch.ivor[BOOKE_IRQPRIO_ALIGNMENT];
		break;
	case SPRN_IVOR6:
		*spr_val = vcpu->arch.ivor[BOOKE_IRQPRIO_PROGRAM];
		break;
	case SPRN_IVOR7:
		*spr_val = vcpu->arch.ivor[BOOKE_IRQPRIO_FP_UNAVAIL];
		break;
	case SPRN_IVOR8:
		*spr_val = vcpu->arch.ivor[BOOKE_IRQPRIO_SYSCALL];
		break;
	case SPRN_IVOR9:
		*spr_val = vcpu->arch.ivor[BOOKE_IRQPRIO_AP_UNAVAIL];
		break;
	case SPRN_IVOR10:
		*spr_val = vcpu->arch.ivor[BOOKE_IRQPRIO_DECREMENTER];
		break;
	case SPRN_IVOR11:
		*spr_val = vcpu->arch.ivor[BOOKE_IRQPRIO_FIT];
		break;
	case SPRN_IVOR12:
		*spr_val = vcpu->arch.ivor[BOOKE_IRQPRIO_WATCHDOG];
		break;
	case SPRN_IVOR13:
		*spr_val = vcpu->arch.ivor[BOOKE_IRQPRIO_DTLB_MISS];
		break;
	case SPRN_IVOR14:
		*spr_val = vcpu->arch.ivor[BOOKE_IRQPRIO_ITLB_MISS];
		break;
	case SPRN_IVOR15:
		*spr_val = vcpu->arch.ivor[BOOKE_IRQPRIO_DEBUG];
		break;
	case SPRN_MCSR:
		*spr_val = vcpu->arch.mcsr;
		break;
#if defined(CONFIG_64BIT)
	case SPRN_EPCR:
		*spr_val = vcpu->arch.epcr;
		break;
#endif

	default:
		emulated = EMULATE_FAIL;
	}

	return emulated;
}
