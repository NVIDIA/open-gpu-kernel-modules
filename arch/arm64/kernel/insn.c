// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2013 Huawei Ltd.
 * Author: Jiang Liu <liuj97@gmail.com>
 *
 * Copyright (C) 2014-2016 Zi Shen Lim <zlim.lnx@gmail.com>
 */
#include <linux/bitops.h>
#include <linux/bug.h>
#include <linux/compiler.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/smp.h>
#include <linux/spinlock.h>
#include <linux/stop_machine.h>
#include <linux/types.h>
#include <linux/uaccess.h>

#include <asm/cacheflush.h>
#include <asm/debug-monitors.h>
#include <asm/fixmap.h>
#include <asm/insn.h>
#include <asm/kprobes.h>
#include <asm/sections.h>

#define AARCH64_INSN_SF_BIT	BIT(31)
#define AARCH64_INSN_N_BIT	BIT(22)
#define AARCH64_INSN_LSL_12	BIT(22)

static const int aarch64_insn_encoding_class[] = {
	AARCH64_INSN_CLS_UNKNOWN,
	AARCH64_INSN_CLS_UNKNOWN,
	AARCH64_INSN_CLS_UNKNOWN,
	AARCH64_INSN_CLS_UNKNOWN,
	AARCH64_INSN_CLS_LDST,
	AARCH64_INSN_CLS_DP_REG,
	AARCH64_INSN_CLS_LDST,
	AARCH64_INSN_CLS_DP_FPSIMD,
	AARCH64_INSN_CLS_DP_IMM,
	AARCH64_INSN_CLS_DP_IMM,
	AARCH64_INSN_CLS_BR_SYS,
	AARCH64_INSN_CLS_BR_SYS,
	AARCH64_INSN_CLS_LDST,
	AARCH64_INSN_CLS_DP_REG,
	AARCH64_INSN_CLS_LDST,
	AARCH64_INSN_CLS_DP_FPSIMD,
};

enum aarch64_insn_encoding_class __kprobes aarch64_get_insn_class(u32 insn)
{
	return aarch64_insn_encoding_class[(insn >> 25) & 0xf];
}

bool __kprobes aarch64_insn_is_steppable_hint(u32 insn)
{
	if (!aarch64_insn_is_hint(insn))
		return false;

	switch (insn & 0xFE0) {
	case AARCH64_INSN_HINT_XPACLRI:
	case AARCH64_INSN_HINT_PACIA_1716:
	case AARCH64_INSN_HINT_PACIB_1716:
	case AARCH64_INSN_HINT_PACIAZ:
	case AARCH64_INSN_HINT_PACIASP:
	case AARCH64_INSN_HINT_PACIBZ:
	case AARCH64_INSN_HINT_PACIBSP:
	case AARCH64_INSN_HINT_BTI:
	case AARCH64_INSN_HINT_BTIC:
	case AARCH64_INSN_HINT_BTIJ:
	case AARCH64_INSN_HINT_BTIJC:
	case AARCH64_INSN_HINT_NOP:
		return true;
	default:
		return false;
	}
}

bool aarch64_insn_is_branch_imm(u32 insn)
{
	return (aarch64_insn_is_b(insn) || aarch64_insn_is_bl(insn) ||
		aarch64_insn_is_tbz(insn) || aarch64_insn_is_tbnz(insn) ||
		aarch64_insn_is_cbz(insn) || aarch64_insn_is_cbnz(insn) ||
		aarch64_insn_is_bcond(insn));
}

static DEFINE_RAW_SPINLOCK(patch_lock);

static bool is_exit_text(unsigned long addr)
{
	/* discarded with init text/data */
	return system_state < SYSTEM_RUNNING &&
		addr >= (unsigned long)__exittext_begin &&
		addr < (unsigned long)__exittext_end;
}

static bool is_image_text(unsigned long addr)
{
	return core_kernel_text(addr) || is_exit_text(addr);
}

static void __kprobes *patch_map(void *addr, int fixmap)
{
	unsigned long uintaddr = (uintptr_t) addr;
	bool image = is_image_text(uintaddr);
	struct page *page;

	if (image)
		page = phys_to_page(__pa_symbol(addr));
	else if (IS_ENABLED(CONFIG_STRICT_MODULE_RWX))
		page = vmalloc_to_page(addr);
	else
		return addr;

	BUG_ON(!page);
	return (void *)set_fixmap_offset(fixmap, page_to_phys(page) +
			(uintaddr & ~PAGE_MASK));
}

static void __kprobes patch_unmap(int fixmap)
{
	clear_fixmap(fixmap);
}
/*
 * In ARMv8-A, A64 instructions have a fixed length of 32 bits and are always
 * little-endian.
 */
int __kprobes aarch64_insn_read(void *addr, u32 *insnp)
{
	int ret;
	__le32 val;

	ret = copy_from_kernel_nofault(&val, addr, AARCH64_INSN_SIZE);
	if (!ret)
		*insnp = le32_to_cpu(val);

	return ret;
}

static int __kprobes __aarch64_insn_write(void *addr, __le32 insn)
{
	void *waddr = addr;
	unsigned long flags = 0;
	int ret;

	raw_spin_lock_irqsave(&patch_lock, flags);
	waddr = patch_map(addr, FIX_TEXT_POKE0);

	ret = copy_to_kernel_nofault(waddr, &insn, AARCH64_INSN_SIZE);

	patch_unmap(FIX_TEXT_POKE0);
	raw_spin_unlock_irqrestore(&patch_lock, flags);

	return ret;
}

int __kprobes aarch64_insn_write(void *addr, u32 insn)
{
	return __aarch64_insn_write(addr, cpu_to_le32(insn));
}

bool __kprobes aarch64_insn_uses_literal(u32 insn)
{
	/* ldr/ldrsw (literal), prfm */

	return aarch64_insn_is_ldr_lit(insn) ||
		aarch64_insn_is_ldrsw_lit(insn) ||
		aarch64_insn_is_adr_adrp(insn) ||
		aarch64_insn_is_prfm_lit(insn);
}

bool __kprobes aarch64_insn_is_branch(u32 insn)
{
	/* b, bl, cb*, tb*, ret*, b.cond, br*, blr* */

	return aarch64_insn_is_b(insn) ||
		aarch64_insn_is_bl(insn) ||
		aarch64_insn_is_cbz(insn) ||
		aarch64_insn_is_cbnz(insn) ||
		aarch64_insn_is_tbz(insn) ||
		aarch64_insn_is_tbnz(insn) ||
		aarch64_insn_is_ret(insn) ||
		aarch64_insn_is_ret_auth(insn) ||
		aarch64_insn_is_br(insn) ||
		aarch64_insn_is_br_auth(insn) ||
		aarch64_insn_is_blr(insn) ||
		aarch64_insn_is_blr_auth(insn) ||
		aarch64_insn_is_bcond(insn);
}

int __kprobes aarch64_insn_patch_text_nosync(void *addr, u32 insn)
{
	u32 *tp = addr;
	int ret;

	/* A64 instructions must be word aligned */
	if ((uintptr_t)tp & 0x3)
		return -EINVAL;

	ret = aarch64_insn_write(tp, insn);
	if (ret == 0)
		__flush_icache_range((uintptr_t)tp,
				     (uintptr_t)tp + AARCH64_INSN_SIZE);

	return ret;
}

struct aarch64_insn_patch {
	void		**text_addrs;
	u32		*new_insns;
	int		insn_cnt;
	atomic_t	cpu_count;
};

static int __kprobes aarch64_insn_patch_text_cb(void *arg)
{
	int i, ret = 0;
	struct aarch64_insn_patch *pp = arg;

	/* The first CPU becomes master */
	if (atomic_inc_return(&pp->cpu_count) == 1) {
		for (i = 0; ret == 0 && i < pp->insn_cnt; i++)
			ret = aarch64_insn_patch_text_nosync(pp->text_addrs[i],
							     pp->new_insns[i]);
		/* Notify other processors with an additional increment. */
		atomic_inc(&pp->cpu_count);
	} else {
		while (atomic_read(&pp->cpu_count) <= num_online_cpus())
			cpu_relax();
		isb();
	}

	return ret;
}

int __kprobes aarch64_insn_patch_text(void *addrs[], u32 insns[], int cnt)
{
	struct aarch64_insn_patch patch = {
		.text_addrs = addrs,
		.new_insns = insns,
		.insn_cnt = cnt,
		.cpu_count = ATOMIC_INIT(0),
	};

	if (cnt <= 0)
		return -EINVAL;

	return stop_machine_cpuslocked(aarch64_insn_patch_text_cb, &patch,
				       cpu_online_mask);
}

static int __kprobes aarch64_get_imm_shift_mask(enum aarch64_insn_imm_type type,
						u32 *maskp, int *shiftp)
{
	u32 mask;
	int shift;

	switch (type) {
	case AARCH64_INSN_IMM_26:
		mask = BIT(26) - 1;
		shift = 0;
		break;
	case AARCH64_INSN_IMM_19:
		mask = BIT(19) - 1;
		shift = 5;
		break;
	case AARCH64_INSN_IMM_16:
		mask = BIT(16) - 1;
		shift = 5;
		break;
	case AARCH64_INSN_IMM_14:
		mask = BIT(14) - 1;
		shift = 5;
		break;
	case AARCH64_INSN_IMM_12:
		mask = BIT(12) - 1;
		shift = 10;
		break;
	case AARCH64_INSN_IMM_9:
		mask = BIT(9) - 1;
		shift = 12;
		break;
	case AARCH64_INSN_IMM_7:
		mask = BIT(7) - 1;
		shift = 15;
		break;
	case AARCH64_INSN_IMM_6:
	case AARCH64_INSN_IMM_S:
		mask = BIT(6) - 1;
		shift = 10;
		break;
	case AARCH64_INSN_IMM_R:
		mask = BIT(6) - 1;
		shift = 16;
		break;
	case AARCH64_INSN_IMM_N:
		mask = 1;
		shift = 22;
		break;
	default:
		return -EINVAL;
	}

	*maskp = mask;
	*shiftp = shift;

	return 0;
}

#define ADR_IMM_HILOSPLIT	2
#define ADR_IMM_SIZE		SZ_2M
#define ADR_IMM_LOMASK		((1 << ADR_IMM_HILOSPLIT) - 1)
#define ADR_IMM_HIMASK		((ADR_IMM_SIZE >> ADR_IMM_HILOSPLIT) - 1)
#define ADR_IMM_LOSHIFT		29
#define ADR_IMM_HISHIFT		5

u64 aarch64_insn_decode_immediate(enum aarch64_insn_imm_type type, u32 insn)
{
	u32 immlo, immhi, mask;
	int shift;

	switch (type) {
	case AARCH64_INSN_IMM_ADR:
		shift = 0;
		immlo = (insn >> ADR_IMM_LOSHIFT) & ADR_IMM_LOMASK;
		immhi = (insn >> ADR_IMM_HISHIFT) & ADR_IMM_HIMASK;
		insn = (immhi << ADR_IMM_HILOSPLIT) | immlo;
		mask = ADR_IMM_SIZE - 1;
		break;
	default:
		if (aarch64_get_imm_shift_mask(type, &mask, &shift) < 0) {
			pr_err("aarch64_insn_decode_immediate: unknown immediate encoding %d\n",
			       type);
			return 0;
		}
	}

	return (insn >> shift) & mask;
}

u32 __kprobes aarch64_insn_encode_immediate(enum aarch64_insn_imm_type type,
				  u32 insn, u64 imm)
{
	u32 immlo, immhi, mask;
	int shift;

	if (insn == AARCH64_BREAK_FAULT)
		return AARCH64_BREAK_FAULT;

	switch (type) {
	case AARCH64_INSN_IMM_ADR:
		shift = 0;
		immlo = (imm & ADR_IMM_LOMASK) << ADR_IMM_LOSHIFT;
		imm >>= ADR_IMM_HILOSPLIT;
		immhi = (imm & ADR_IMM_HIMASK) << ADR_IMM_HISHIFT;
		imm = immlo | immhi;
		mask = ((ADR_IMM_LOMASK << ADR_IMM_LOSHIFT) |
			(ADR_IMM_HIMASK << ADR_IMM_HISHIFT));
		break;
	default:
		if (aarch64_get_imm_shift_mask(type, &mask, &shift) < 0) {
			pr_err("aarch64_insn_encode_immediate: unknown immediate encoding %d\n",
			       type);
			return AARCH64_BREAK_FAULT;
		}
	}

	/* Update the immediate field. */
	insn &= ~(mask << shift);
	insn |= (imm & mask) << shift;

	return insn;
}

u32 aarch64_insn_decode_register(enum aarch64_insn_register_type type,
					u32 insn)
{
	int shift;

	switch (type) {
	case AARCH64_INSN_REGTYPE_RT:
	case AARCH64_INSN_REGTYPE_RD:
		shift = 0;
		break;
	case AARCH64_INSN_REGTYPE_RN:
		shift = 5;
		break;
	case AARCH64_INSN_REGTYPE_RT2:
	case AARCH64_INSN_REGTYPE_RA:
		shift = 10;
		break;
	case AARCH64_INSN_REGTYPE_RM:
		shift = 16;
		break;
	default:
		pr_err("%s: unknown register type encoding %d\n", __func__,
		       type);
		return 0;
	}

	return (insn >> shift) & GENMASK(4, 0);
}

static u32 aarch64_insn_encode_register(enum aarch64_insn_register_type type,
					u32 insn,
					enum aarch64_insn_register reg)
{
	int shift;

	if (insn == AARCH64_BREAK_FAULT)
		return AARCH64_BREAK_FAULT;

	if (reg < AARCH64_INSN_REG_0 || reg > AARCH64_INSN_REG_SP) {
		pr_err("%s: unknown register encoding %d\n", __func__, reg);
		return AARCH64_BREAK_FAULT;
	}

	switch (type) {
	case AARCH64_INSN_REGTYPE_RT:
	case AARCH64_INSN_REGTYPE_RD:
		shift = 0;
		break;
	case AARCH64_INSN_REGTYPE_RN:
		shift = 5;
		break;
	case AARCH64_INSN_REGTYPE_RT2:
	case AARCH64_INSN_REGTYPE_RA:
		shift = 10;
		break;
	case AARCH64_INSN_REGTYPE_RM:
	case AARCH64_INSN_REGTYPE_RS:
		shift = 16;
		break;
	default:
		pr_err("%s: unknown register type encoding %d\n", __func__,
		       type);
		return AARCH64_BREAK_FAULT;
	}

	insn &= ~(GENMASK(4, 0) << shift);
	insn |= reg << shift;

	return insn;
}

static u32 aarch64_insn_encode_ldst_size(enum aarch64_insn_size_type type,
					 u32 insn)
{
	u32 size;

	switch (type) {
	case AARCH64_INSN_SIZE_8:
		size = 0;
		break;
	case AARCH64_INSN_SIZE_16:
		size = 1;
		break;
	case AARCH64_INSN_SIZE_32:
		size = 2;
		break;
	case AARCH64_INSN_SIZE_64:
		size = 3;
		break;
	default:
		pr_err("%s: unknown size encoding %d\n", __func__, type);
		return AARCH64_BREAK_FAULT;
	}

	insn &= ~GENMASK(31, 30);
	insn |= size << 30;

	return insn;
}

static inline long branch_imm_common(unsigned long pc, unsigned long addr,
				     long range)
{
	long offset;

	if ((pc & 0x3) || (addr & 0x3)) {
		pr_err("%s: A64 instructions must be word aligned\n", __func__);
		return range;
	}

	offset = ((long)addr - (long)pc);

	if (offset < -range || offset >= range) {
		pr_err("%s: offset out of range\n", __func__);
		return range;
	}

	return offset;
}

u32 __kprobes aarch64_insn_gen_branch_imm(unsigned long pc, unsigned long addr,
					  enum aarch64_insn_branch_type type)
{
	u32 insn;
	long offset;

	/*
	 * B/BL support [-128M, 128M) offset
	 * ARM64 virtual address arrangement guarantees all kernel and module
	 * texts are within +/-128M.
	 */
	offset = branch_imm_common(pc, addr, SZ_128M);
	if (offset >= SZ_128M)
		return AARCH64_BREAK_FAULT;

	switch (type) {
	case AARCH64_INSN_BRANCH_LINK:
		insn = aarch64_insn_get_bl_value();
		break;
	case AARCH64_INSN_BRANCH_NOLINK:
		insn = aarch64_insn_get_b_value();
		break;
	default:
		pr_err("%s: unknown branch encoding %d\n", __func__, type);
		return AARCH64_BREAK_FAULT;
	}

	return aarch64_insn_encode_immediate(AARCH64_INSN_IMM_26, insn,
					     offset >> 2);
}

u32 aarch64_insn_gen_comp_branch_imm(unsigned long pc, unsigned long addr,
				     enum aarch64_insn_register reg,
				     enum aarch64_insn_variant variant,
				     enum aarch64_insn_branch_type type)
{
	u32 insn;
	long offset;

	offset = branch_imm_common(pc, addr, SZ_1M);
	if (offset >= SZ_1M)
		return AARCH64_BREAK_FAULT;

	switch (type) {
	case AARCH64_INSN_BRANCH_COMP_ZERO:
		insn = aarch64_insn_get_cbz_value();
		break;
	case AARCH64_INSN_BRANCH_COMP_NONZERO:
		insn = aarch64_insn_get_cbnz_value();
		break;
	default:
		pr_err("%s: unknown branch encoding %d\n", __func__, type);
		return AARCH64_BREAK_FAULT;
	}

	switch (variant) {
	case AARCH64_INSN_VARIANT_32BIT:
		break;
	case AARCH64_INSN_VARIANT_64BIT:
		insn |= AARCH64_INSN_SF_BIT;
		break;
	default:
		pr_err("%s: unknown variant encoding %d\n", __func__, variant);
		return AARCH64_BREAK_FAULT;
	}

	insn = aarch64_insn_encode_register(AARCH64_INSN_REGTYPE_RT, insn, reg);

	return aarch64_insn_encode_immediate(AARCH64_INSN_IMM_19, insn,
					     offset >> 2);
}

u32 aarch64_insn_gen_cond_branch_imm(unsigned long pc, unsigned long addr,
				     enum aarch64_insn_condition cond)
{
	u32 insn;
	long offset;

	offset = branch_imm_common(pc, addr, SZ_1M);

	insn = aarch64_insn_get_bcond_value();

	if (cond < AARCH64_INSN_COND_EQ || cond > AARCH64_INSN_COND_AL) {
		pr_err("%s: unknown condition encoding %d\n", __func__, cond);
		return AARCH64_BREAK_FAULT;
	}
	insn |= cond;

	return aarch64_insn_encode_immediate(AARCH64_INSN_IMM_19, insn,
					     offset >> 2);
}

u32 __kprobes aarch64_insn_gen_hint(enum aarch64_insn_hint_cr_op op)
{
	return aarch64_insn_get_hint_value() | op;
}

u32 __kprobes aarch64_insn_gen_nop(void)
{
	return aarch64_insn_gen_hint(AARCH64_INSN_HINT_NOP);
}

u32 aarch64_insn_gen_branch_reg(enum aarch64_insn_register reg,
				enum aarch64_insn_branch_type type)
{
	u32 insn;

	switch (type) {
	case AARCH64_INSN_BRANCH_NOLINK:
		insn = aarch64_insn_get_br_value();
		break;
	case AARCH64_INSN_BRANCH_LINK:
		insn = aarch64_insn_get_blr_value();
		break;
	case AARCH64_INSN_BRANCH_RETURN:
		insn = aarch64_insn_get_ret_value();
		break;
	default:
		pr_err("%s: unknown branch encoding %d\n", __func__, type);
		return AARCH64_BREAK_FAULT;
	}

	return aarch64_insn_encode_register(AARCH64_INSN_REGTYPE_RN, insn, reg);
}

u32 aarch64_insn_gen_load_store_reg(enum aarch64_insn_register reg,
				    enum aarch64_insn_register base,
				    enum aarch64_insn_register offset,
				    enum aarch64_insn_size_type size,
				    enum aarch64_insn_ldst_type type)
{
	u32 insn;

	switch (type) {
	case AARCH64_INSN_LDST_LOAD_REG_OFFSET:
		insn = aarch64_insn_get_ldr_reg_value();
		break;
	case AARCH64_INSN_LDST_STORE_REG_OFFSET:
		insn = aarch64_insn_get_str_reg_value();
		break;
	default:
		pr_err("%s: unknown load/store encoding %d\n", __func__, type);
		return AARCH64_BREAK_FAULT;
	}

	insn = aarch64_insn_encode_ldst_size(size, insn);

	insn = aarch64_insn_encode_register(AARCH64_INSN_REGTYPE_RT, insn, reg);

	insn = aarch64_insn_encode_register(AARCH64_INSN_REGTYPE_RN, insn,
					    base);

	return aarch64_insn_encode_register(AARCH64_INSN_REGTYPE_RM, insn,
					    offset);
}

u32 aarch64_insn_gen_load_store_pair(enum aarch64_insn_register reg1,
				     enum aarch64_insn_register reg2,
				     enum aarch64_insn_register base,
				     int offset,
				     enum aarch64_insn_variant variant,
				     enum aarch64_insn_ldst_type type)
{
	u32 insn;
	int shift;

	switch (type) {
	case AARCH64_INSN_LDST_LOAD_PAIR_PRE_INDEX:
		insn = aarch64_insn_get_ldp_pre_value();
		break;
	case AARCH64_INSN_LDST_STORE_PAIR_PRE_INDEX:
		insn = aarch64_insn_get_stp_pre_value();
		break;
	case AARCH64_INSN_LDST_LOAD_PAIR_POST_INDEX:
		insn = aarch64_insn_get_ldp_post_value();
		break;
	case AARCH64_INSN_LDST_STORE_PAIR_POST_INDEX:
		insn = aarch64_insn_get_stp_post_value();
		break;
	default:
		pr_err("%s: unknown load/store encoding %d\n", __func__, type);
		return AARCH64_BREAK_FAULT;
	}

	switch (variant) {
	case AARCH64_INSN_VARIANT_32BIT:
		if ((offset & 0x3) || (offset < -256) || (offset > 252)) {
			pr_err("%s: offset must be multiples of 4 in the range of [-256, 252] %d\n",
			       __func__, offset);
			return AARCH64_BREAK_FAULT;
		}
		shift = 2;
		break;
	case AARCH64_INSN_VARIANT_64BIT:
		if ((offset & 0x7) || (offset < -512) || (offset > 504)) {
			pr_err("%s: offset must be multiples of 8 in the range of [-512, 504] %d\n",
			       __func__, offset);
			return AARCH64_BREAK_FAULT;
		}
		shift = 3;
		insn |= AARCH64_INSN_SF_BIT;
		break;
	default:
		pr_err("%s: unknown variant encoding %d\n", __func__, variant);
		return AARCH64_BREAK_FAULT;
	}

	insn = aarch64_insn_encode_register(AARCH64_INSN_REGTYPE_RT, insn,
					    reg1);

	insn = aarch64_insn_encode_register(AARCH64_INSN_REGTYPE_RT2, insn,
					    reg2);

	insn = aarch64_insn_encode_register(AARCH64_INSN_REGTYPE_RN, insn,
					    base);

	return aarch64_insn_encode_immediate(AARCH64_INSN_IMM_7, insn,
					     offset >> shift);
}

u32 aarch64_insn_gen_load_store_ex(enum aarch64_insn_register reg,
				   enum aarch64_insn_register base,
				   enum aarch64_insn_register state,
				   enum aarch64_insn_size_type size,
				   enum aarch64_insn_ldst_type type)
{
	u32 insn;

	switch (type) {
	case AARCH64_INSN_LDST_LOAD_EX:
		insn = aarch64_insn_get_load_ex_value();
		break;
	case AARCH64_INSN_LDST_STORE_EX:
		insn = aarch64_insn_get_store_ex_value();
		break;
	default:
		pr_err("%s: unknown load/store exclusive encoding %d\n", __func__, type);
		return AARCH64_BREAK_FAULT;
	}

	insn = aarch64_insn_encode_ldst_size(size, insn);

	insn = aarch64_insn_encode_register(AARCH64_INSN_REGTYPE_RT, insn,
					    reg);

	insn = aarch64_insn_encode_register(AARCH64_INSN_REGTYPE_RN, insn,
					    base);

	insn = aarch64_insn_encode_register(AARCH64_INSN_REGTYPE_RT2, insn,
					    AARCH64_INSN_REG_ZR);

	return aarch64_insn_encode_register(AARCH64_INSN_REGTYPE_RS, insn,
					    state);
}

u32 aarch64_insn_gen_ldadd(enum aarch64_insn_register result,
			   enum aarch64_insn_register address,
			   enum aarch64_insn_register value,
			   enum aarch64_insn_size_type size)
{
	u32 insn = aarch64_insn_get_ldadd_value();

	switch (size) {
	case AARCH64_INSN_SIZE_32:
	case AARCH64_INSN_SIZE_64:
		break;
	default:
		pr_err("%s: unimplemented size encoding %d\n", __func__, size);
		return AARCH64_BREAK_FAULT;
	}

	insn = aarch64_insn_encode_ldst_size(size, insn);

	insn = aarch64_insn_encode_register(AARCH64_INSN_REGTYPE_RT, insn,
					    result);

	insn = aarch64_insn_encode_register(AARCH64_INSN_REGTYPE_RN, insn,
					    address);

	return aarch64_insn_encode_register(AARCH64_INSN_REGTYPE_RS, insn,
					    value);
}

u32 aarch64_insn_gen_stadd(enum aarch64_insn_register address,
			   enum aarch64_insn_register value,
			   enum aarch64_insn_size_type size)
{
	/*
	 * STADD is simply encoded as an alias for LDADD with XZR as
	 * the destination register.
	 */
	return aarch64_insn_gen_ldadd(AARCH64_INSN_REG_ZR, address,
				      value, size);
}

static u32 aarch64_insn_encode_prfm_imm(enum aarch64_insn_prfm_type type,
					enum aarch64_insn_prfm_target target,
					enum aarch64_insn_prfm_policy policy,
					u32 insn)
{
	u32 imm_type = 0, imm_target = 0, imm_policy = 0;

	switch (type) {
	case AARCH64_INSN_PRFM_TYPE_PLD:
		break;
	case AARCH64_INSN_PRFM_TYPE_PLI:
		imm_type = BIT(0);
		break;
	case AARCH64_INSN_PRFM_TYPE_PST:
		imm_type = BIT(1);
		break;
	default:
		pr_err("%s: unknown prfm type encoding %d\n", __func__, type);
		return AARCH64_BREAK_FAULT;
	}

	switch (target) {
	case AARCH64_INSN_PRFM_TARGET_L1:
		break;
	case AARCH64_INSN_PRFM_TARGET_L2:
		imm_target = BIT(0);
		break;
	case AARCH64_INSN_PRFM_TARGET_L3:
		imm_target = BIT(1);
		break;
	default:
		pr_err("%s: unknown prfm target encoding %d\n", __func__, target);
		return AARCH64_BREAK_FAULT;
	}

	switch (policy) {
	case AARCH64_INSN_PRFM_POLICY_KEEP:
		break;
	case AARCH64_INSN_PRFM_POLICY_STRM:
		imm_policy = BIT(0);
		break;
	default:
		pr_err("%s: unknown prfm policy encoding %d\n", __func__, policy);
		return AARCH64_BREAK_FAULT;
	}

	/* In this case, imm5 is encoded into Rt field. */
	insn &= ~GENMASK(4, 0);
	insn |= imm_policy | (imm_target << 1) | (imm_type << 3);

	return insn;
}

u32 aarch64_insn_gen_prefetch(enum aarch64_insn_register base,
			      enum aarch64_insn_prfm_type type,
			      enum aarch64_insn_prfm_target target,
			      enum aarch64_insn_prfm_policy policy)
{
	u32 insn = aarch64_insn_get_prfm_value();

	insn = aarch64_insn_encode_ldst_size(AARCH64_INSN_SIZE_64, insn);

	insn = aarch64_insn_encode_prfm_imm(type, target, policy, insn);

	insn = aarch64_insn_encode_register(AARCH64_INSN_REGTYPE_RN, insn,
					    base);

	return aarch64_insn_encode_immediate(AARCH64_INSN_IMM_12, insn, 0);
}

u32 aarch64_insn_gen_add_sub_imm(enum aarch64_insn_register dst,
				 enum aarch64_insn_register src,
				 int imm, enum aarch64_insn_variant variant,
				 enum aarch64_insn_adsb_type type)
{
	u32 insn;

	switch (type) {
	case AARCH64_INSN_ADSB_ADD:
		insn = aarch64_insn_get_add_imm_value();
		break;
	case AARCH64_INSN_ADSB_SUB:
		insn = aarch64_insn_get_sub_imm_value();
		break;
	case AARCH64_INSN_ADSB_ADD_SETFLAGS:
		insn = aarch64_insn_get_adds_imm_value();
		break;
	case AARCH64_INSN_ADSB_SUB_SETFLAGS:
		insn = aarch64_insn_get_subs_imm_value();
		break;
	default:
		pr_err("%s: unknown add/sub encoding %d\n", __func__, type);
		return AARCH64_BREAK_FAULT;
	}

	switch (variant) {
	case AARCH64_INSN_VARIANT_32BIT:
		break;
	case AARCH64_INSN_VARIANT_64BIT:
		insn |= AARCH64_INSN_SF_BIT;
		break;
	default:
		pr_err("%s: unknown variant encoding %d\n", __func__, variant);
		return AARCH64_BREAK_FAULT;
	}

	/* We can't encode more than a 24bit value (12bit + 12bit shift) */
	if (imm & ~(BIT(24) - 1))
		goto out;

	/* If we have something in the top 12 bits... */
	if (imm & ~(SZ_4K - 1)) {
		/* ... and in the low 12 bits -> error */
		if (imm & (SZ_4K - 1))
			goto out;

		imm >>= 12;
		insn |= AARCH64_INSN_LSL_12;
	}

	insn = aarch64_insn_encode_register(AARCH64_INSN_REGTYPE_RD, insn, dst);

	insn = aarch64_insn_encode_register(AARCH64_INSN_REGTYPE_RN, insn, src);

	return aarch64_insn_encode_immediate(AARCH64_INSN_IMM_12, insn, imm);

out:
	pr_err("%s: invalid immediate encoding %d\n", __func__, imm);
	return AARCH64_BREAK_FAULT;
}

u32 aarch64_insn_gen_bitfield(enum aarch64_insn_register dst,
			      enum aarch64_insn_register src,
			      int immr, int imms,
			      enum aarch64_insn_variant variant,
			      enum aarch64_insn_bitfield_type type)
{
	u32 insn;
	u32 mask;

	switch (type) {
	case AARCH64_INSN_BITFIELD_MOVE:
		insn = aarch64_insn_get_bfm_value();
		break;
	case AARCH64_INSN_BITFIELD_MOVE_UNSIGNED:
		insn = aarch64_insn_get_ubfm_value();
		break;
	case AARCH64_INSN_BITFIELD_MOVE_SIGNED:
		insn = aarch64_insn_get_sbfm_value();
		break;
	default:
		pr_err("%s: unknown bitfield encoding %d\n", __func__, type);
		return AARCH64_BREAK_FAULT;
	}

	switch (variant) {
	case AARCH64_INSN_VARIANT_32BIT:
		mask = GENMASK(4, 0);
		break;
	case AARCH64_INSN_VARIANT_64BIT:
		insn |= AARCH64_INSN_SF_BIT | AARCH64_INSN_N_BIT;
		mask = GENMASK(5, 0);
		break;
	default:
		pr_err("%s: unknown variant encoding %d\n", __func__, variant);
		return AARCH64_BREAK_FAULT;
	}

	if (immr & ~mask) {
		pr_err("%s: invalid immr encoding %d\n", __func__, immr);
		return AARCH64_BREAK_FAULT;
	}
	if (imms & ~mask) {
		pr_err("%s: invalid imms encoding %d\n", __func__, imms);
		return AARCH64_BREAK_FAULT;
	}

	insn = aarch64_insn_encode_register(AARCH64_INSN_REGTYPE_RD, insn, dst);

	insn = aarch64_insn_encode_register(AARCH64_INSN_REGTYPE_RN, insn, src);

	insn = aarch64_insn_encode_immediate(AARCH64_INSN_IMM_R, insn, immr);

	return aarch64_insn_encode_immediate(AARCH64_INSN_IMM_S, insn, imms);
}

u32 aarch64_insn_gen_movewide(enum aarch64_insn_register dst,
			      int imm, int shift,
			      enum aarch64_insn_variant variant,
			      enum aarch64_insn_movewide_type type)
{
	u32 insn;

	switch (type) {
	case AARCH64_INSN_MOVEWIDE_ZERO:
		insn = aarch64_insn_get_movz_value();
		break;
	case AARCH64_INSN_MOVEWIDE_KEEP:
		insn = aarch64_insn_get_movk_value();
		break;
	case AARCH64_INSN_MOVEWIDE_INVERSE:
		insn = aarch64_insn_get_movn_value();
		break;
	default:
		pr_err("%s: unknown movewide encoding %d\n", __func__, type);
		return AARCH64_BREAK_FAULT;
	}

	if (imm & ~(SZ_64K - 1)) {
		pr_err("%s: invalid immediate encoding %d\n", __func__, imm);
		return AARCH64_BREAK_FAULT;
	}

	switch (variant) {
	case AARCH64_INSN_VARIANT_32BIT:
		if (shift != 0 && shift != 16) {
			pr_err("%s: invalid shift encoding %d\n", __func__,
			       shift);
			return AARCH64_BREAK_FAULT;
		}
		break;
	case AARCH64_INSN_VARIANT_64BIT:
		insn |= AARCH64_INSN_SF_BIT;
		if (shift != 0 && shift != 16 && shift != 32 && shift != 48) {
			pr_err("%s: invalid shift encoding %d\n", __func__,
			       shift);
			return AARCH64_BREAK_FAULT;
		}
		break;
	default:
		pr_err("%s: unknown variant encoding %d\n", __func__, variant);
		return AARCH64_BREAK_FAULT;
	}

	insn |= (shift >> 4) << 21;

	insn = aarch64_insn_encode_register(AARCH64_INSN_REGTYPE_RD, insn, dst);

	return aarch64_insn_encode_immediate(AARCH64_INSN_IMM_16, insn, imm);
}

u32 aarch64_insn_gen_add_sub_shifted_reg(enum aarch64_insn_register dst,
					 enum aarch64_insn_register src,
					 enum aarch64_insn_register reg,
					 int shift,
					 enum aarch64_insn_variant variant,
					 enum aarch64_insn_adsb_type type)
{
	u32 insn;

	switch (type) {
	case AARCH64_INSN_ADSB_ADD:
		insn = aarch64_insn_get_add_value();
		break;
	case AARCH64_INSN_ADSB_SUB:
		insn = aarch64_insn_get_sub_value();
		break;
	case AARCH64_INSN_ADSB_ADD_SETFLAGS:
		insn = aarch64_insn_get_adds_value();
		break;
	case AARCH64_INSN_ADSB_SUB_SETFLAGS:
		insn = aarch64_insn_get_subs_value();
		break;
	default:
		pr_err("%s: unknown add/sub encoding %d\n", __func__, type);
		return AARCH64_BREAK_FAULT;
	}

	switch (variant) {
	case AARCH64_INSN_VARIANT_32BIT:
		if (shift & ~(SZ_32 - 1)) {
			pr_err("%s: invalid shift encoding %d\n", __func__,
			       shift);
			return AARCH64_BREAK_FAULT;
		}
		break;
	case AARCH64_INSN_VARIANT_64BIT:
		insn |= AARCH64_INSN_SF_BIT;
		if (shift & ~(SZ_64 - 1)) {
			pr_err("%s: invalid shift encoding %d\n", __func__,
			       shift);
			return AARCH64_BREAK_FAULT;
		}
		break;
	default:
		pr_err("%s: unknown variant encoding %d\n", __func__, variant);
		return AARCH64_BREAK_FAULT;
	}


	insn = aarch64_insn_encode_register(AARCH64_INSN_REGTYPE_RD, insn, dst);

	insn = aarch64_insn_encode_register(AARCH64_INSN_REGTYPE_RN, insn, src);

	insn = aarch64_insn_encode_register(AARCH64_INSN_REGTYPE_RM, insn, reg);

	return aarch64_insn_encode_immediate(AARCH64_INSN_IMM_6, insn, shift);
}

u32 aarch64_insn_gen_data1(enum aarch64_insn_register dst,
			   enum aarch64_insn_register src,
			   enum aarch64_insn_variant variant,
			   enum aarch64_insn_data1_type type)
{
	u32 insn;

	switch (type) {
	case AARCH64_INSN_DATA1_REVERSE_16:
		insn = aarch64_insn_get_rev16_value();
		break;
	case AARCH64_INSN_DATA1_REVERSE_32:
		insn = aarch64_insn_get_rev32_value();
		break;
	case AARCH64_INSN_DATA1_REVERSE_64:
		if (variant != AARCH64_INSN_VARIANT_64BIT) {
			pr_err("%s: invalid variant for reverse64 %d\n",
			       __func__, variant);
			return AARCH64_BREAK_FAULT;
		}
		insn = aarch64_insn_get_rev64_value();
		break;
	default:
		pr_err("%s: unknown data1 encoding %d\n", __func__, type);
		return AARCH64_BREAK_FAULT;
	}

	switch (variant) {
	case AARCH64_INSN_VARIANT_32BIT:
		break;
	case AARCH64_INSN_VARIANT_64BIT:
		insn |= AARCH64_INSN_SF_BIT;
		break;
	default:
		pr_err("%s: unknown variant encoding %d\n", __func__, variant);
		return AARCH64_BREAK_FAULT;
	}

	insn = aarch64_insn_encode_register(AARCH64_INSN_REGTYPE_RD, insn, dst);

	return aarch64_insn_encode_register(AARCH64_INSN_REGTYPE_RN, insn, src);
}

u32 aarch64_insn_gen_data2(enum aarch64_insn_register dst,
			   enum aarch64_insn_register src,
			   enum aarch64_insn_register reg,
			   enum aarch64_insn_variant variant,
			   enum aarch64_insn_data2_type type)
{
	u32 insn;

	switch (type) {
	case AARCH64_INSN_DATA2_UDIV:
		insn = aarch64_insn_get_udiv_value();
		break;
	case AARCH64_INSN_DATA2_SDIV:
		insn = aarch64_insn_get_sdiv_value();
		break;
	case AARCH64_INSN_DATA2_LSLV:
		insn = aarch64_insn_get_lslv_value();
		break;
	case AARCH64_INSN_DATA2_LSRV:
		insn = aarch64_insn_get_lsrv_value();
		break;
	case AARCH64_INSN_DATA2_ASRV:
		insn = aarch64_insn_get_asrv_value();
		break;
	case AARCH64_INSN_DATA2_RORV:
		insn = aarch64_insn_get_rorv_value();
		break;
	default:
		pr_err("%s: unknown data2 encoding %d\n", __func__, type);
		return AARCH64_BREAK_FAULT;
	}

	switch (variant) {
	case AARCH64_INSN_VARIANT_32BIT:
		break;
	case AARCH64_INSN_VARIANT_64BIT:
		insn |= AARCH64_INSN_SF_BIT;
		break;
	default:
		pr_err("%s: unknown variant encoding %d\n", __func__, variant);
		return AARCH64_BREAK_FAULT;
	}

	insn = aarch64_insn_encode_register(AARCH64_INSN_REGTYPE_RD, insn, dst);

	insn = aarch64_insn_encode_register(AARCH64_INSN_REGTYPE_RN, insn, src);

	return aarch64_insn_encode_register(AARCH64_INSN_REGTYPE_RM, insn, reg);
}

u32 aarch64_insn_gen_data3(enum aarch64_insn_register dst,
			   enum aarch64_insn_register src,
			   enum aarch64_insn_register reg1,
			   enum aarch64_insn_register reg2,
			   enum aarch64_insn_variant variant,
			   enum aarch64_insn_data3_type type)
{
	u32 insn;

	switch (type) {
	case AARCH64_INSN_DATA3_MADD:
		insn = aarch64_insn_get_madd_value();
		break;
	case AARCH64_INSN_DATA3_MSUB:
		insn = aarch64_insn_get_msub_value();
		break;
	default:
		pr_err("%s: unknown data3 encoding %d\n", __func__, type);
		return AARCH64_BREAK_FAULT;
	}

	switch (variant) {
	case AARCH64_INSN_VARIANT_32BIT:
		break;
	case AARCH64_INSN_VARIANT_64BIT:
		insn |= AARCH64_INSN_SF_BIT;
		break;
	default:
		pr_err("%s: unknown variant encoding %d\n", __func__, variant);
		return AARCH64_BREAK_FAULT;
	}

	insn = aarch64_insn_encode_register(AARCH64_INSN_REGTYPE_RD, insn, dst);

	insn = aarch64_insn_encode_register(AARCH64_INSN_REGTYPE_RA, insn, src);

	insn = aarch64_insn_encode_register(AARCH64_INSN_REGTYPE_RN, insn,
					    reg1);

	return aarch64_insn_encode_register(AARCH64_INSN_REGTYPE_RM, insn,
					    reg2);
}

u32 aarch64_insn_gen_logical_shifted_reg(enum aarch64_insn_register dst,
					 enum aarch64_insn_register src,
					 enum aarch64_insn_register reg,
					 int shift,
					 enum aarch64_insn_variant variant,
					 enum aarch64_insn_logic_type type)
{
	u32 insn;

	switch (type) {
	case AARCH64_INSN_LOGIC_AND:
		insn = aarch64_insn_get_and_value();
		break;
	case AARCH64_INSN_LOGIC_BIC:
		insn = aarch64_insn_get_bic_value();
		break;
	case AARCH64_INSN_LOGIC_ORR:
		insn = aarch64_insn_get_orr_value();
		break;
	case AARCH64_INSN_LOGIC_ORN:
		insn = aarch64_insn_get_orn_value();
		break;
	case AARCH64_INSN_LOGIC_EOR:
		insn = aarch64_insn_get_eor_value();
		break;
	case AARCH64_INSN_LOGIC_EON:
		insn = aarch64_insn_get_eon_value();
		break;
	case AARCH64_INSN_LOGIC_AND_SETFLAGS:
		insn = aarch64_insn_get_ands_value();
		break;
	case AARCH64_INSN_LOGIC_BIC_SETFLAGS:
		insn = aarch64_insn_get_bics_value();
		break;
	default:
		pr_err("%s: unknown logical encoding %d\n", __func__, type);
		return AARCH64_BREAK_FAULT;
	}

	switch (variant) {
	case AARCH64_INSN_VARIANT_32BIT:
		if (shift & ~(SZ_32 - 1)) {
			pr_err("%s: invalid shift encoding %d\n", __func__,
			       shift);
			return AARCH64_BREAK_FAULT;
		}
		break;
	case AARCH64_INSN_VARIANT_64BIT:
		insn |= AARCH64_INSN_SF_BIT;
		if (shift & ~(SZ_64 - 1)) {
			pr_err("%s: invalid shift encoding %d\n", __func__,
			       shift);
			return AARCH64_BREAK_FAULT;
		}
		break;
	default:
		pr_err("%s: unknown variant encoding %d\n", __func__, variant);
		return AARCH64_BREAK_FAULT;
	}


	insn = aarch64_insn_encode_register(AARCH64_INSN_REGTYPE_RD, insn, dst);

	insn = aarch64_insn_encode_register(AARCH64_INSN_REGTYPE_RN, insn, src);

	insn = aarch64_insn_encode_register(AARCH64_INSN_REGTYPE_RM, insn, reg);

	return aarch64_insn_encode_immediate(AARCH64_INSN_IMM_6, insn, shift);
}

/*
 * MOV (register) is architecturally an alias of ORR (shifted register) where
 * MOV <*d>, <*m> is equivalent to ORR <*d>, <*ZR>, <*m>
 */
u32 aarch64_insn_gen_move_reg(enum aarch64_insn_register dst,
			      enum aarch64_insn_register src,
			      enum aarch64_insn_variant variant)
{
	return aarch64_insn_gen_logical_shifted_reg(dst, AARCH64_INSN_REG_ZR,
						    src, 0, variant,
						    AARCH64_INSN_LOGIC_ORR);
}

u32 aarch64_insn_gen_adr(unsigned long pc, unsigned long addr,
			 enum aarch64_insn_register reg,
			 enum aarch64_insn_adr_type type)
{
	u32 insn;
	s32 offset;

	switch (type) {
	case AARCH64_INSN_ADR_TYPE_ADR:
		insn = aarch64_insn_get_adr_value();
		offset = addr - pc;
		break;
	case AARCH64_INSN_ADR_TYPE_ADRP:
		insn = aarch64_insn_get_adrp_value();
		offset = (addr - ALIGN_DOWN(pc, SZ_4K)) >> 12;
		break;
	default:
		pr_err("%s: unknown adr encoding %d\n", __func__, type);
		return AARCH64_BREAK_FAULT;
	}

	if (offset < -SZ_1M || offset >= SZ_1M)
		return AARCH64_BREAK_FAULT;

	insn = aarch64_insn_encode_register(AARCH64_INSN_REGTYPE_RD, insn, reg);

	return aarch64_insn_encode_immediate(AARCH64_INSN_IMM_ADR, insn, offset);
}

/*
 * Decode the imm field of a branch, and return the byte offset as a
 * signed value (so it can be used when computing a new branch
 * target).
 */
s32 aarch64_get_branch_offset(u32 insn)
{
	s32 imm;

	if (aarch64_insn_is_b(insn) || aarch64_insn_is_bl(insn)) {
		imm = aarch64_insn_decode_immediate(AARCH64_INSN_IMM_26, insn);
		return (imm << 6) >> 4;
	}

	if (aarch64_insn_is_cbz(insn) || aarch64_insn_is_cbnz(insn) ||
	    aarch64_insn_is_bcond(insn)) {
		imm = aarch64_insn_decode_immediate(AARCH64_INSN_IMM_19, insn);
		return (imm << 13) >> 11;
	}

	if (aarch64_insn_is_tbz(insn) || aarch64_insn_is_tbnz(insn)) {
		imm = aarch64_insn_decode_immediate(AARCH64_INSN_IMM_14, insn);
		return (imm << 18) >> 16;
	}

	/* Unhandled instruction */
	BUG();
}

/*
 * Encode the displacement of a branch in the imm field and return the
 * updated instruction.
 */
u32 aarch64_set_branch_offset(u32 insn, s32 offset)
{
	if (aarch64_insn_is_b(insn) || aarch64_insn_is_bl(insn))
		return aarch64_insn_encode_immediate(AARCH64_INSN_IMM_26, insn,
						     offset >> 2);

	if (aarch64_insn_is_cbz(insn) || aarch64_insn_is_cbnz(insn) ||
	    aarch64_insn_is_bcond(insn))
		return aarch64_insn_encode_immediate(AARCH64_INSN_IMM_19, insn,
						     offset >> 2);

	if (aarch64_insn_is_tbz(insn) || aarch64_insn_is_tbnz(insn))
		return aarch64_insn_encode_immediate(AARCH64_INSN_IMM_14, insn,
						     offset >> 2);

	/* Unhandled instruction */
	BUG();
}

s32 aarch64_insn_adrp_get_offset(u32 insn)
{
	BUG_ON(!aarch64_insn_is_adrp(insn));
	return aarch64_insn_decode_immediate(AARCH64_INSN_IMM_ADR, insn) << 12;
}

u32 aarch64_insn_adrp_set_offset(u32 insn, s32 offset)
{
	BUG_ON(!aarch64_insn_is_adrp(insn));
	return aarch64_insn_encode_immediate(AARCH64_INSN_IMM_ADR, insn,
						offset >> 12);
}

/*
 * Extract the Op/CR data from a msr/mrs instruction.
 */
u32 aarch64_insn_extract_system_reg(u32 insn)
{
	return (insn & 0x1FFFE0) >> 5;
}

bool aarch32_insn_is_wide(u32 insn)
{
	return insn >= 0xe800;
}

/*
 * Macros/defines for extracting register numbers from instruction.
 */
u32 aarch32_insn_extract_reg_num(u32 insn, int offset)
{
	return (insn & (0xf << offset)) >> offset;
}

#define OPC2_MASK	0x7
#define OPC2_OFFSET	5
u32 aarch32_insn_mcr_extract_opc2(u32 insn)
{
	return (insn & (OPC2_MASK << OPC2_OFFSET)) >> OPC2_OFFSET;
}

#define CRM_MASK	0xf
u32 aarch32_insn_mcr_extract_crm(u32 insn)
{
	return insn & CRM_MASK;
}

static bool __kprobes __check_eq(unsigned long pstate)
{
	return (pstate & PSR_Z_BIT) != 0;
}

static bool __kprobes __check_ne(unsigned long pstate)
{
	return (pstate & PSR_Z_BIT) == 0;
}

static bool __kprobes __check_cs(unsigned long pstate)
{
	return (pstate & PSR_C_BIT) != 0;
}

static bool __kprobes __check_cc(unsigned long pstate)
{
	return (pstate & PSR_C_BIT) == 0;
}

static bool __kprobes __check_mi(unsigned long pstate)
{
	return (pstate & PSR_N_BIT) != 0;
}

static bool __kprobes __check_pl(unsigned long pstate)
{
	return (pstate & PSR_N_BIT) == 0;
}

static bool __kprobes __check_vs(unsigned long pstate)
{
	return (pstate & PSR_V_BIT) != 0;
}

static bool __kprobes __check_vc(unsigned long pstate)
{
	return (pstate & PSR_V_BIT) == 0;
}

static bool __kprobes __check_hi(unsigned long pstate)
{
	pstate &= ~(pstate >> 1);	/* PSR_C_BIT &= ~PSR_Z_BIT */
	return (pstate & PSR_C_BIT) != 0;
}

static bool __kprobes __check_ls(unsigned long pstate)
{
	pstate &= ~(pstate >> 1);	/* PSR_C_BIT &= ~PSR_Z_BIT */
	return (pstate & PSR_C_BIT) == 0;
}

static bool __kprobes __check_ge(unsigned long pstate)
{
	pstate ^= (pstate << 3);	/* PSR_N_BIT ^= PSR_V_BIT */
	return (pstate & PSR_N_BIT) == 0;
}

static bool __kprobes __check_lt(unsigned long pstate)
{
	pstate ^= (pstate << 3);	/* PSR_N_BIT ^= PSR_V_BIT */
	return (pstate & PSR_N_BIT) != 0;
}

static bool __kprobes __check_gt(unsigned long pstate)
{
	/*PSR_N_BIT ^= PSR_V_BIT */
	unsigned long temp = pstate ^ (pstate << 3);

	temp |= (pstate << 1);	/*PSR_N_BIT |= PSR_Z_BIT */
	return (temp & PSR_N_BIT) == 0;
}

static bool __kprobes __check_le(unsigned long pstate)
{
	/*PSR_N_BIT ^= PSR_V_BIT */
	unsigned long temp = pstate ^ (pstate << 3);

	temp |= (pstate << 1);	/*PSR_N_BIT |= PSR_Z_BIT */
	return (temp & PSR_N_BIT) != 0;
}

static bool __kprobes __check_al(unsigned long pstate)
{
	return true;
}

/*
 * Note that the ARMv8 ARM calls condition code 0b1111 "nv", but states that
 * it behaves identically to 0b1110 ("al").
 */
pstate_check_t * const aarch32_opcode_cond_checks[16] = {
	__check_eq, __check_ne, __check_cs, __check_cc,
	__check_mi, __check_pl, __check_vs, __check_vc,
	__check_hi, __check_ls, __check_ge, __check_lt,
	__check_gt, __check_le, __check_al, __check_al
};

static bool range_of_ones(u64 val)
{
	/* Doesn't handle full ones or full zeroes */
	u64 sval = val >> __ffs64(val);

	/* One of Sean Eron Anderson's bithack tricks */
	return ((sval + 1) & (sval)) == 0;
}

static u32 aarch64_encode_immediate(u64 imm,
				    enum aarch64_insn_variant variant,
				    u32 insn)
{
	unsigned int immr, imms, n, ones, ror, esz, tmp;
	u64 mask;

	switch (variant) {
	case AARCH64_INSN_VARIANT_32BIT:
		esz = 32;
		break;
	case AARCH64_INSN_VARIANT_64BIT:
		insn |= AARCH64_INSN_SF_BIT;
		esz = 64;
		break;
	default:
		pr_err("%s: unknown variant encoding %d\n", __func__, variant);
		return AARCH64_BREAK_FAULT;
	}

	mask = GENMASK(esz - 1, 0);

	/* Can't encode full zeroes, full ones, or value wider than the mask */
	if (!imm || imm == mask || imm & ~mask)
		return AARCH64_BREAK_FAULT;

	/*
	 * Inverse of Replicate(). Try to spot a repeating pattern
	 * with a pow2 stride.
	 */
	for (tmp = esz / 2; tmp >= 2; tmp /= 2) {
		u64 emask = BIT(tmp) - 1;

		if ((imm & emask) != ((imm >> tmp) & emask))
			break;

		esz = tmp;
		mask = emask;
	}

	/* N is only set if we're encoding a 64bit value */
	n = esz == 64;

	/* Trim imm to the element size */
	imm &= mask;

	/* That's how many ones we need to encode */
	ones = hweight64(imm);

	/*
	 * imms is set to (ones - 1), prefixed with a string of ones
	 * and a zero if they fit. Cap it to 6 bits.
	 */
	imms  = ones - 1;
	imms |= 0xf << ffs(esz);
	imms &= BIT(6) - 1;

	/* Compute the rotation */
	if (range_of_ones(imm)) {
		/*
		 * Pattern: 0..01..10..0
		 *
		 * Compute how many rotate we need to align it right
		 */
		ror = __ffs64(imm);
	} else {
		/*
		 * Pattern: 0..01..10..01..1
		 *
		 * Fill the unused top bits with ones, and check if
		 * the result is a valid immediate (all ones with a
		 * contiguous ranges of zeroes).
		 */
		imm |= ~mask;
		if (!range_of_ones(~imm))
			return AARCH64_BREAK_FAULT;

		/*
		 * Compute the rotation to get a continuous set of
		 * ones, with the first bit set at position 0
		 */
		ror = fls(~imm);
	}

	/*
	 * immr is the number of bits we need to rotate back to the
	 * original set of ones. Note that this is relative to the
	 * element size...
	 */
	immr = (esz - ror) % esz;

	insn = aarch64_insn_encode_immediate(AARCH64_INSN_IMM_N, insn, n);
	insn = aarch64_insn_encode_immediate(AARCH64_INSN_IMM_R, insn, immr);
	return aarch64_insn_encode_immediate(AARCH64_INSN_IMM_S, insn, imms);
}

u32 aarch64_insn_gen_logical_immediate(enum aarch64_insn_logic_type type,
				       enum aarch64_insn_variant variant,
				       enum aarch64_insn_register Rn,
				       enum aarch64_insn_register Rd,
				       u64 imm)
{
	u32 insn;

	switch (type) {
	case AARCH64_INSN_LOGIC_AND:
		insn = aarch64_insn_get_and_imm_value();
		break;
	case AARCH64_INSN_LOGIC_ORR:
		insn = aarch64_insn_get_orr_imm_value();
		break;
	case AARCH64_INSN_LOGIC_EOR:
		insn = aarch64_insn_get_eor_imm_value();
		break;
	case AARCH64_INSN_LOGIC_AND_SETFLAGS:
		insn = aarch64_insn_get_ands_imm_value();
		break;
	default:
		pr_err("%s: unknown logical encoding %d\n", __func__, type);
		return AARCH64_BREAK_FAULT;
	}

	insn = aarch64_insn_encode_register(AARCH64_INSN_REGTYPE_RD, insn, Rd);
	insn = aarch64_insn_encode_register(AARCH64_INSN_REGTYPE_RN, insn, Rn);
	return aarch64_encode_immediate(imm, variant, insn);
}

u32 aarch64_insn_gen_extr(enum aarch64_insn_variant variant,
			  enum aarch64_insn_register Rm,
			  enum aarch64_insn_register Rn,
			  enum aarch64_insn_register Rd,
			  u8 lsb)
{
	u32 insn;

	insn = aarch64_insn_get_extr_value();

	switch (variant) {
	case AARCH64_INSN_VARIANT_32BIT:
		if (lsb > 31)
			return AARCH64_BREAK_FAULT;
		break;
	case AARCH64_INSN_VARIANT_64BIT:
		if (lsb > 63)
			return AARCH64_BREAK_FAULT;
		insn |= AARCH64_INSN_SF_BIT;
		insn = aarch64_insn_encode_immediate(AARCH64_INSN_IMM_N, insn, 1);
		break;
	default:
		pr_err("%s: unknown variant encoding %d\n", __func__, variant);
		return AARCH64_BREAK_FAULT;
	}

	insn = aarch64_insn_encode_immediate(AARCH64_INSN_IMM_S, insn, lsb);
	insn = aarch64_insn_encode_register(AARCH64_INSN_REGTYPE_RD, insn, Rd);
	insn = aarch64_insn_encode_register(AARCH64_INSN_REGTYPE_RN, insn, Rn);
	return aarch64_insn_encode_register(AARCH64_INSN_REGTYPE_RM, insn, Rm);
}
