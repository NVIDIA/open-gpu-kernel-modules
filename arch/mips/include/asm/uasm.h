/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2004, 2005, 2006, 2008	 Thiemo Seufer
 * Copyright (C) 2005  Maciej W. Rozycki
 * Copyright (C) 2006  Ralf Baechle (ralf@linux-mips.org)
 * Copyright (C) 2012, 2013  MIPS Technologies, Inc.  All rights reserved.
 */

#ifndef __ASM_UASM_H
#define __ASM_UASM_H

#include <linux/types.h>

#ifdef CONFIG_EXPORT_UASM
#include <linux/export.h>
#define UASM_EXPORT_SYMBOL(sym) EXPORT_SYMBOL(sym)
#else
#define UASM_EXPORT_SYMBOL(sym)
#endif

#define Ip_u1u2u3(op)							\
void uasm_i##op(u32 **buf, unsigned int a, unsigned int b, unsigned int c)

#define Ip_u2u1u3(op)							\
void uasm_i##op(u32 **buf, unsigned int a, unsigned int b, unsigned int c)

#define Ip_u3u2u1(op)							\
void uasm_i##op(u32 **buf, unsigned int a, unsigned int b, unsigned int c)

#define Ip_u3u1u2(op)							\
void uasm_i##op(u32 **buf, unsigned int a, unsigned int b, unsigned int c)

#define Ip_u1u2s3(op)							\
void uasm_i##op(u32 **buf, unsigned int a, unsigned int b, signed int c)

#define Ip_u2s3u1(op)							\
void uasm_i##op(u32 **buf, unsigned int a, signed int b, unsigned int c)

#define Ip_s3s1s2(op)							\
void uasm_i##op(u32 **buf, int a, int b, int c)

#define Ip_u2u1s3(op)							\
void uasm_i##op(u32 **buf, unsigned int a, unsigned int b, signed int c)

#define Ip_u2u1msbu3(op)						\
void uasm_i##op(u32 **buf, unsigned int a, unsigned int b, unsigned int c, \
	   unsigned int d)

#define Ip_u1u2(op)							\
void uasm_i##op(u32 **buf, unsigned int a, unsigned int b)

#define Ip_u2u1(op)							\
void uasm_i##op(u32 **buf, unsigned int a, unsigned int b)

#define Ip_u1s2(op)							\
void uasm_i##op(u32 **buf, unsigned int a, signed int b)

#define Ip_u1(op) void uasm_i##op(u32 **buf, unsigned int a)

#define Ip_0(op) void uasm_i##op(u32 **buf)

Ip_u2u1s3(_addiu);
Ip_u3u1u2(_addu);
Ip_u3u1u2(_and);
Ip_u2u1u3(_andi);
Ip_u1u2s3(_bbit0);
Ip_u1u2s3(_bbit1);
Ip_u1u2s3(_beq);
Ip_u1u2s3(_beql);
Ip_u1s2(_bgez);
Ip_u1s2(_bgezl);
Ip_u1s2(_bgtz);
Ip_u1s2(_blez);
Ip_u1s2(_bltz);
Ip_u1s2(_bltzl);
Ip_u1u2s3(_bne);
Ip_u1(_break);
Ip_u2s3u1(_cache);
Ip_u1u2(_cfc1);
Ip_u2u1(_cfcmsa);
Ip_u1u2(_ctc1);
Ip_u2u1(_ctcmsa);
Ip_u2u1s3(_daddiu);
Ip_u3u1u2(_daddu);
Ip_u1u2(_ddivu);
Ip_u3u1u2(_ddivu_r6);
Ip_u1(_di);
Ip_u2u1msbu3(_dins);
Ip_u2u1msbu3(_dinsm);
Ip_u2u1msbu3(_dinsu);
Ip_u1u2(_divu);
Ip_u3u1u2(_divu_r6);
Ip_u1u2u3(_dmfc0);
Ip_u3u1u2(_dmodu);
Ip_u1u2u3(_dmtc0);
Ip_u1u2(_dmultu);
Ip_u3u1u2(_dmulu);
Ip_u2u1u3(_drotr);
Ip_u2u1u3(_drotr32);
Ip_u2u1(_dsbh);
Ip_u2u1(_dshd);
Ip_u2u1u3(_dsll);
Ip_u2u1u3(_dsll32);
Ip_u3u2u1(_dsllv);
Ip_u2u1u3(_dsra);
Ip_u2u1u3(_dsra32);
Ip_u3u2u1(_dsrav);
Ip_u2u1u3(_dsrl);
Ip_u2u1u3(_dsrl32);
Ip_u3u2u1(_dsrlv);
Ip_u3u1u2(_dsubu);
Ip_0(_eret);
Ip_u2u1msbu3(_ext);
Ip_u2u1msbu3(_ins);
Ip_u1(_j);
Ip_u1(_jal);
Ip_u2u1(_jalr);
Ip_u1(_jr);
Ip_u2s3u1(_lb);
Ip_u2s3u1(_lbu);
Ip_u2s3u1(_ld);
Ip_u3u1u2(_ldx);
Ip_u2s3u1(_lh);
Ip_u2s3u1(_lhu);
Ip_u2s3u1(_ll);
Ip_u2s3u1(_lld);
Ip_u1s2(_lui);
Ip_u2s3u1(_lw);
Ip_u2s3u1(_lwu);
Ip_u3u1u2(_lwx);
Ip_u1u2u3(_mfc0);
Ip_u1u2u3(_mfhc0);
Ip_u1(_mfhi);
Ip_u1(_mflo);
Ip_u3u1u2(_modu);
Ip_u3u1u2(_movn);
Ip_u3u1u2(_movz);
Ip_u1u2u3(_mtc0);
Ip_u1u2u3(_mthc0);
Ip_u1(_mthi);
Ip_u1(_mtlo);
Ip_u3u1u2(_mul);
Ip_u1u2(_multu);
Ip_u3u1u2(_mulu);
Ip_u3u1u2(_nor);
Ip_u3u1u2(_or);
Ip_u2u1u3(_ori);
Ip_u2s3u1(_pref);
Ip_0(_rfe);
Ip_u2u1u3(_rotr);
Ip_u2s3u1(_sb);
Ip_u2s3u1(_sc);
Ip_u2s3u1(_scd);
Ip_u2s3u1(_sd);
Ip_u3u1u2(_seleqz);
Ip_u3u1u2(_selnez);
Ip_u2s3u1(_sh);
Ip_u2u1u3(_sll);
Ip_u3u2u1(_sllv);
Ip_s3s1s2(_slt);
Ip_u2u1s3(_slti);
Ip_u2u1s3(_sltiu);
Ip_u3u1u2(_sltu);
Ip_u2u1u3(_sra);
Ip_u3u2u1(_srav);
Ip_u2u1u3(_srl);
Ip_u3u2u1(_srlv);
Ip_u3u1u2(_subu);
Ip_u2s3u1(_sw);
Ip_u1(_sync);
Ip_u1(_syscall);
Ip_0(_tlbp);
Ip_0(_tlbr);
Ip_0(_tlbwi);
Ip_0(_tlbwr);
Ip_u1(_wait);
Ip_u2u1(_wsbh);
Ip_u3u1u2(_xor);
Ip_u2u1u3(_xori);
Ip_u2u1(_yield);
Ip_u1u2(_ldpte);
Ip_u2u1u3(_lddir);

/* Handle labels. */
struct uasm_label {
	u32 *addr;
	int lab;
};

void uasm_build_label(struct uasm_label **lab, u32 *addr,
			int lid);
#ifdef CONFIG_64BIT
int uasm_in_compat_space_p(long addr);
#endif
int uasm_rel_hi(long val);
int uasm_rel_lo(long val);
void UASM_i_LA_mostly(u32 **buf, unsigned int rs, long addr);
void UASM_i_LA(u32 **buf, unsigned int rs, long addr);

#define UASM_L_LA(lb)							\
static inline void uasm_l##lb(struct uasm_label **lab, u32 *addr)	\
{									\
	uasm_build_label(lab, addr, label##lb);				\
}

/* convenience macros for instructions */
#ifdef CONFIG_64BIT
# define UASM_i_ADDIU(buf, rs, rt, val) uasm_i_daddiu(buf, rs, rt, val)
# define UASM_i_ADDU(buf, rs, rt, rd) uasm_i_daddu(buf, rs, rt, rd)
# define UASM_i_LL(buf, rs, rt, off) uasm_i_lld(buf, rs, rt, off)
# define UASM_i_LW(buf, rs, rt, off) uasm_i_ld(buf, rs, rt, off)
# define UASM_i_LWX(buf, rs, rt, rd) uasm_i_ldx(buf, rs, rt, rd)
# define UASM_i_MFC0(buf, rt, rd...) uasm_i_dmfc0(buf, rt, rd)
# define UASM_i_MTC0(buf, rt, rd...) uasm_i_dmtc0(buf, rt, rd)
# define UASM_i_ROTR(buf, rs, rt, sh) uasm_i_drotr(buf, rs, rt, sh)
# define UASM_i_SC(buf, rs, rt, off) uasm_i_scd(buf, rs, rt, off)
# define UASM_i_SLL(buf, rs, rt, sh) uasm_i_dsll(buf, rs, rt, sh)
# define UASM_i_SRA(buf, rs, rt, sh) uasm_i_dsra(buf, rs, rt, sh)
# define UASM_i_SRL(buf, rs, rt, sh) uasm_i_dsrl(buf, rs, rt, sh)
# define UASM_i_SRL_SAFE(buf, rs, rt, sh) uasm_i_dsrl_safe(buf, rs, rt, sh)
# define UASM_i_SUBU(buf, rs, rt, rd) uasm_i_dsubu(buf, rs, rt, rd)
# define UASM_i_SW(buf, rs, rt, off) uasm_i_sd(buf, rs, rt, off)
#else
# define UASM_i_ADDIU(buf, rs, rt, val) uasm_i_addiu(buf, rs, rt, val)
# define UASM_i_ADDU(buf, rs, rt, rd) uasm_i_addu(buf, rs, rt, rd)
# define UASM_i_LL(buf, rs, rt, off) uasm_i_ll(buf, rs, rt, off)
# define UASM_i_LW(buf, rs, rt, off) uasm_i_lw(buf, rs, rt, off)
# define UASM_i_LWX(buf, rs, rt, rd) uasm_i_lwx(buf, rs, rt, rd)
# define UASM_i_MFC0(buf, rt, rd...) uasm_i_mfc0(buf, rt, rd)
# define UASM_i_MTC0(buf, rt, rd...) uasm_i_mtc0(buf, rt, rd)
# define UASM_i_ROTR(buf, rs, rt, sh) uasm_i_rotr(buf, rs, rt, sh)
# define UASM_i_SC(buf, rs, rt, off) uasm_i_sc(buf, rs, rt, off)
# define UASM_i_SLL(buf, rs, rt, sh) uasm_i_sll(buf, rs, rt, sh)
# define UASM_i_SRA(buf, rs, rt, sh) uasm_i_sra(buf, rs, rt, sh)
# define UASM_i_SRL(buf, rs, rt, sh) uasm_i_srl(buf, rs, rt, sh)
# define UASM_i_SRL_SAFE(buf, rs, rt, sh) uasm_i_srl(buf, rs, rt, sh)
# define UASM_i_SUBU(buf, rs, rt, rd) uasm_i_subu(buf, rs, rt, rd)
# define UASM_i_SW(buf, rs, rt, off) uasm_i_sw(buf, rs, rt, off)
#endif

#define uasm_i_b(buf, off) uasm_i_beq(buf, 0, 0, off)
#define uasm_i_beqz(buf, rs, off) uasm_i_beq(buf, rs, 0, off)
#define uasm_i_beqzl(buf, rs, off) uasm_i_beql(buf, rs, 0, off)
#define uasm_i_bnez(buf, rs, off) uasm_i_bne(buf, rs, 0, off)
#define uasm_i_bnezl(buf, rs, off) uasm_i_bnel(buf, rs, 0, off)
#define uasm_i_ehb(buf) uasm_i_sll(buf, 0, 0, 3)
#define uasm_i_move(buf, a, b) UASM_i_ADDU(buf, a, 0, b)
#define uasm_i_nop(buf) uasm_i_sll(buf, 0, 0, 0)
#define uasm_i_ssnop(buf) uasm_i_sll(buf, 0, 0, 1)

static inline void uasm_i_drotr_safe(u32 **p, unsigned int a1,
				     unsigned int a2, unsigned int a3)
{
	if (a3 < 32)
		uasm_i_drotr(p, a1, a2, a3);
	else
		uasm_i_drotr32(p, a1, a2, a3 - 32);
}

static inline void uasm_i_dsll_safe(u32 **p, unsigned int a1,
				    unsigned int a2, unsigned int a3)
{
	if (a3 < 32)
		uasm_i_dsll(p, a1, a2, a3);
	else
		uasm_i_dsll32(p, a1, a2, a3 - 32);
}

static inline void uasm_i_dsrl_safe(u32 **p, unsigned int a1,
				    unsigned int a2, unsigned int a3)
{
	if (a3 < 32)
		uasm_i_dsrl(p, a1, a2, a3);
	else
		uasm_i_dsrl32(p, a1, a2, a3 - 32);
}

static inline void uasm_i_dsra_safe(u32 **p, unsigned int a1,
				    unsigned int a2, unsigned int a3)
{
	if (a3 < 32)
		uasm_i_dsra(p, a1, a2, a3);
	else
		uasm_i_dsra32(p, a1, a2, a3 - 32);
}

/* Handle relocations. */
struct uasm_reloc {
	u32 *addr;
	unsigned int type;
	int lab;
};

/* This is zero so we can use zeroed label arrays. */
#define UASM_LABEL_INVALID 0

void uasm_r_mips_pc16(struct uasm_reloc **rel, u32 *addr, int lid);
void uasm_resolve_relocs(struct uasm_reloc *rel, struct uasm_label *lab);
void uasm_move_relocs(struct uasm_reloc *rel, u32 *first, u32 *end, long off);
void uasm_move_labels(struct uasm_label *lab, u32 *first, u32 *end, long off);
void uasm_copy_handler(struct uasm_reloc *rel, struct uasm_label *lab,
	u32 *first, u32 *end, u32 *target);
int uasm_insn_has_bdelay(struct uasm_reloc *rel, u32 *addr);

/* Convenience functions for labeled branches. */
void uasm_il_b(u32 **p, struct uasm_reloc **r, int lid);
void uasm_il_bbit0(u32 **p, struct uasm_reloc **r, unsigned int reg,
		   unsigned int bit, int lid);
void uasm_il_bbit1(u32 **p, struct uasm_reloc **r, unsigned int reg,
		   unsigned int bit, int lid);
void uasm_il_beq(u32 **p, struct uasm_reloc **r, unsigned int r1,
		 unsigned int r2, int lid);
void uasm_il_beqz(u32 **p, struct uasm_reloc **r, unsigned int reg, int lid);
void uasm_il_beqzl(u32 **p, struct uasm_reloc **r, unsigned int reg, int lid);
void uasm_il_bgezl(u32 **p, struct uasm_reloc **r, unsigned int reg, int lid);
void uasm_il_bgez(u32 **p, struct uasm_reloc **r, unsigned int reg, int lid);
void uasm_il_bltz(u32 **p, struct uasm_reloc **r, unsigned int reg, int lid);
void uasm_il_bne(u32 **p, struct uasm_reloc **r, unsigned int reg1,
		 unsigned int reg2, int lid);
void uasm_il_bnez(u32 **p, struct uasm_reloc **r, unsigned int reg, int lid);

#endif /* __ASM_UASM_H */
