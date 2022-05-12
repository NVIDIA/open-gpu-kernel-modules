// SPDX-License-Identifier: GPL-2.0-only
/*
 *
 * Copyright IBM Corp. 2007
 * Copyright 2011 Freescale Semiconductor, Inc.
 *
 * Authors: Hollis Blanchard <hollisb@us.ibm.com>
 */

#include <linux/jiffies.h>
#include <linux/hrtimer.h>
#include <linux/types.h>
#include <linux/string.h>
#include <linux/kvm_host.h>
#include <linux/clockchips.h>

#include <asm/reg.h>
#include <asm/time.h>
#include <asm/byteorder.h>
#include <asm/kvm_ppc.h>
#include <asm/disassemble.h>
#include <asm/ppc-opcode.h>
#include "timing.h"
#include "trace.h"

void kvmppc_emulate_dec(struct kvm_vcpu *vcpu)
{
	unsigned long dec_nsec;
	unsigned long long dec_time;

	pr_debug("mtDEC: %lx\n", vcpu->arch.dec);
	hrtimer_try_to_cancel(&vcpu->arch.dec_timer);

#ifdef CONFIG_PPC_BOOK3S
	/* mtdec lowers the interrupt line when positive. */
	kvmppc_core_dequeue_dec(vcpu);
#endif

#ifdef CONFIG_BOOKE
	/* On BOOKE, DEC = 0 is as good as decrementer not enabled */
	if (vcpu->arch.dec == 0)
		return;
#endif

	/*
	 * The decrementer ticks at the same rate as the timebase, so
	 * that's how we convert the guest DEC value to the number of
	 * host ticks.
	 */

	dec_time = vcpu->arch.dec;
	/*
	 * Guest timebase ticks at the same frequency as host timebase.
	 * So use the host timebase calculations for decrementer emulation.
	 */
	dec_time = tb_to_ns(dec_time);
	dec_nsec = do_div(dec_time, NSEC_PER_SEC);
	hrtimer_start(&vcpu->arch.dec_timer,
		ktime_set(dec_time, dec_nsec), HRTIMER_MODE_REL);
	vcpu->arch.dec_jiffies = get_tb();
}

u32 kvmppc_get_dec(struct kvm_vcpu *vcpu, u64 tb)
{
	u64 jd = tb - vcpu->arch.dec_jiffies;

#ifdef CONFIG_BOOKE
	if (vcpu->arch.dec < jd)
		return 0;
#endif

	return vcpu->arch.dec - jd;
}

static int kvmppc_emulate_mtspr(struct kvm_vcpu *vcpu, int sprn, int rs)
{
	enum emulation_result emulated = EMULATE_DONE;
	ulong spr_val = kvmppc_get_gpr(vcpu, rs);

	switch (sprn) {
	case SPRN_SRR0:
		kvmppc_set_srr0(vcpu, spr_val);
		break;
	case SPRN_SRR1:
		kvmppc_set_srr1(vcpu, spr_val);
		break;

	/* XXX We need to context-switch the timebase for
	 * watchdog and FIT. */
	case SPRN_TBWL: break;
	case SPRN_TBWU: break;

	case SPRN_DEC:
		vcpu->arch.dec = (u32) spr_val;
		kvmppc_emulate_dec(vcpu);
		break;

	case SPRN_SPRG0:
		kvmppc_set_sprg0(vcpu, spr_val);
		break;
	case SPRN_SPRG1:
		kvmppc_set_sprg1(vcpu, spr_val);
		break;
	case SPRN_SPRG2:
		kvmppc_set_sprg2(vcpu, spr_val);
		break;
	case SPRN_SPRG3:
		kvmppc_set_sprg3(vcpu, spr_val);
		break;

	/* PIR can legally be written, but we ignore it */
	case SPRN_PIR: break;

	default:
		emulated = vcpu->kvm->arch.kvm_ops->emulate_mtspr(vcpu, sprn,
								  spr_val);
		if (emulated == EMULATE_FAIL)
			printk(KERN_INFO "mtspr: unknown spr "
				"0x%x\n", sprn);
		break;
	}

	kvmppc_set_exit_type(vcpu, EMULATED_MTSPR_EXITS);

	return emulated;
}

static int kvmppc_emulate_mfspr(struct kvm_vcpu *vcpu, int sprn, int rt)
{
	enum emulation_result emulated = EMULATE_DONE;
	ulong spr_val = 0;

	switch (sprn) {
	case SPRN_SRR0:
		spr_val = kvmppc_get_srr0(vcpu);
		break;
	case SPRN_SRR1:
		spr_val = kvmppc_get_srr1(vcpu);
		break;
	case SPRN_PVR:
		spr_val = vcpu->arch.pvr;
		break;
	case SPRN_PIR:
		spr_val = vcpu->vcpu_id;
		break;

	/* Note: mftb and TBRL/TBWL are user-accessible, so
	 * the guest can always access the real TB anyways.
	 * In fact, we probably will never see these traps. */
	case SPRN_TBWL:
		spr_val = get_tb() >> 32;
		break;
	case SPRN_TBWU:
		spr_val = get_tb();
		break;

	case SPRN_SPRG0:
		spr_val = kvmppc_get_sprg0(vcpu);
		break;
	case SPRN_SPRG1:
		spr_val = kvmppc_get_sprg1(vcpu);
		break;
	case SPRN_SPRG2:
		spr_val = kvmppc_get_sprg2(vcpu);
		break;
	case SPRN_SPRG3:
		spr_val = kvmppc_get_sprg3(vcpu);
		break;
	/* Note: SPRG4-7 are user-readable, so we don't get
	 * a trap. */

	case SPRN_DEC:
		spr_val = kvmppc_get_dec(vcpu, get_tb());
		break;
	default:
		emulated = vcpu->kvm->arch.kvm_ops->emulate_mfspr(vcpu, sprn,
								  &spr_val);
		if (unlikely(emulated == EMULATE_FAIL)) {
			printk(KERN_INFO "mfspr: unknown spr "
				"0x%x\n", sprn);
		}
		break;
	}

	if (emulated == EMULATE_DONE)
		kvmppc_set_gpr(vcpu, rt, spr_val);
	kvmppc_set_exit_type(vcpu, EMULATED_MFSPR_EXITS);

	return emulated;
}

/* XXX Should probably auto-generate instruction decoding for a particular core
 * from opcode tables in the future. */
int kvmppc_emulate_instruction(struct kvm_vcpu *vcpu)
{
	u32 inst;
	int rs, rt, sprn;
	enum emulation_result emulated;
	int advance = 1;

	/* this default type might be overwritten by subcategories */
	kvmppc_set_exit_type(vcpu, EMULATED_INST_EXITS);

	emulated = kvmppc_get_last_inst(vcpu, INST_GENERIC, &inst);
	if (emulated != EMULATE_DONE)
		return emulated;

	pr_debug("Emulating opcode %d / %d\n", get_op(inst), get_xop(inst));

	rs = get_rs(inst);
	rt = get_rt(inst);
	sprn = get_sprn(inst);

	switch (get_op(inst)) {
	case OP_TRAP:
#ifdef CONFIG_PPC_BOOK3S
	case OP_TRAP_64:
		kvmppc_core_queue_program(vcpu, SRR1_PROGTRAP);
#else
		kvmppc_core_queue_program(vcpu,
					  vcpu->arch.shared->esr | ESR_PTR);
#endif
		advance = 0;
		break;

	case 31:
		switch (get_xop(inst)) {

		case OP_31_XOP_TRAP:
#ifdef CONFIG_64BIT
		case OP_31_XOP_TRAP_64:
#endif
#ifdef CONFIG_PPC_BOOK3S
			kvmppc_core_queue_program(vcpu, SRR1_PROGTRAP);
#else
			kvmppc_core_queue_program(vcpu,
					vcpu->arch.shared->esr | ESR_PTR);
#endif
			advance = 0;
			break;

		case OP_31_XOP_MFSPR:
			emulated = kvmppc_emulate_mfspr(vcpu, sprn, rt);
			if (emulated == EMULATE_AGAIN) {
				emulated = EMULATE_DONE;
				advance = 0;
			}
			break;

		case OP_31_XOP_MTSPR:
			emulated = kvmppc_emulate_mtspr(vcpu, sprn, rs);
			if (emulated == EMULATE_AGAIN) {
				emulated = EMULATE_DONE;
				advance = 0;
			}
			break;

		case OP_31_XOP_TLBSYNC:
			break;

		default:
			/* Attempt core-specific emulation below. */
			emulated = EMULATE_FAIL;
		}
		break;

	case 0:
		/*
		 * Instruction with primary opcode 0. Based on PowerISA
		 * these are illegal instructions.
		 */
		if (inst == KVMPPC_INST_SW_BREAKPOINT) {
			vcpu->run->exit_reason = KVM_EXIT_DEBUG;
			vcpu->run->debug.arch.status = 0;
			vcpu->run->debug.arch.address = kvmppc_get_pc(vcpu);
			emulated = EMULATE_EXIT_USER;
			advance = 0;
		} else
			emulated = EMULATE_FAIL;

		break;

	default:
		emulated = EMULATE_FAIL;
	}

	if (emulated == EMULATE_FAIL) {
		emulated = vcpu->kvm->arch.kvm_ops->emulate_op(vcpu, inst,
							       &advance);
		if (emulated == EMULATE_AGAIN) {
			advance = 0;
		} else if (emulated == EMULATE_FAIL) {
			advance = 0;
			printk(KERN_ERR "Couldn't emulate instruction 0x%08x "
			       "(op %d xop %d)\n", inst, get_op(inst), get_xop(inst));
		}
	}

	trace_kvm_ppc_instr(inst, kvmppc_get_pc(vcpu), emulated);

	/* Advance past emulated instruction. */
	if (advance)
		kvmppc_set_pc(vcpu, kvmppc_get_pc(vcpu) + 4);

	return emulated;
}
EXPORT_SYMBOL_GPL(kvmppc_emulate_instruction);
