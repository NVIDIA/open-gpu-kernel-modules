/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __ASM_ALTERNATIVE_MACROS_H
#define __ASM_ALTERNATIVE_MACROS_H

#include <asm/cpucaps.h>

#define ARM64_CB_PATCH ARM64_NCAPS

/* A64 instructions are always 32 bits. */
#define	AARCH64_INSN_SIZE		4

#ifndef __ASSEMBLY__

#include <linux/stringify.h>

#define ALTINSTR_ENTRY(feature)					              \
	" .word 661b - .\n"				/* label           */ \
	" .word 663f - .\n"				/* new instruction */ \
	" .hword " __stringify(feature) "\n"		/* feature bit     */ \
	" .byte 662b-661b\n"				/* source len      */ \
	" .byte 664f-663f\n"				/* replacement len */

#define ALTINSTR_ENTRY_CB(feature, cb)					      \
	" .word 661b - .\n"				/* label           */ \
	" .word " __stringify(cb) "- .\n"		/* callback */	      \
	" .hword " __stringify(feature) "\n"		/* feature bit     */ \
	" .byte 662b-661b\n"				/* source len      */ \
	" .byte 664f-663f\n"				/* replacement len */

/*
 * alternative assembly primitive:
 *
 * If any of these .org directive fail, it means that insn1 and insn2
 * don't have the same length. This used to be written as
 *
 * .if ((664b-663b) != (662b-661b))
 * 	.error "Alternatives instruction length mismatch"
 * .endif
 *
 * but most assemblers die if insn1 or insn2 have a .inst. This should
 * be fixed in a binutils release posterior to 2.25.51.0.2 (anything
 * containing commit 4e4d08cf7399b606 or c1baaddf8861).
 *
 * Alternatives with callbacks do not generate replacement instructions.
 */
#define __ALTERNATIVE_CFG(oldinstr, newinstr, feature, cfg_enabled)	\
	".if "__stringify(cfg_enabled)" == 1\n"				\
	"661:\n\t"							\
	oldinstr "\n"							\
	"662:\n"							\
	".pushsection .altinstructions,\"a\"\n"				\
	ALTINSTR_ENTRY(feature)						\
	".popsection\n"							\
	".subsection 1\n"						\
	"663:\n\t"							\
	newinstr "\n"							\
	"664:\n\t"							\
	".org	. - (664b-663b) + (662b-661b)\n\t"			\
	".org	. - (662b-661b) + (664b-663b)\n\t"			\
	".previous\n"							\
	".endif\n"

#define __ALTERNATIVE_CFG_CB(oldinstr, feature, cfg_enabled, cb)	\
	".if "__stringify(cfg_enabled)" == 1\n"				\
	"661:\n\t"							\
	oldinstr "\n"							\
	"662:\n"							\
	".pushsection .altinstructions,\"a\"\n"				\
	ALTINSTR_ENTRY_CB(feature, cb)					\
	".popsection\n"							\
	"663:\n\t"							\
	"664:\n\t"							\
	".endif\n"

#define _ALTERNATIVE_CFG(oldinstr, newinstr, feature, cfg, ...)	\
	__ALTERNATIVE_CFG(oldinstr, newinstr, feature, IS_ENABLED(cfg))

#define ALTERNATIVE_CB(oldinstr, cb) \
	__ALTERNATIVE_CFG_CB(oldinstr, ARM64_CB_PATCH, 1, cb)
#else

#include <asm/assembler.h>

.macro altinstruction_entry orig_offset alt_offset feature orig_len alt_len
	.word \orig_offset - .
	.word \alt_offset - .
	.hword \feature
	.byte \orig_len
	.byte \alt_len
.endm

.macro alternative_insn insn1, insn2, cap, enable = 1
	.if \enable
661:	\insn1
662:	.pushsection .altinstructions, "a"
	altinstruction_entry 661b, 663f, \cap, 662b-661b, 664f-663f
	.popsection
	.subsection 1
663:	\insn2
664:	.org	. - (664b-663b) + (662b-661b)
	.org	. - (662b-661b) + (664b-663b)
	.previous
	.endif
.endm

/*
 * Alternative sequences
 *
 * The code for the case where the capability is not present will be
 * assembled and linked as normal. There are no restrictions on this
 * code.
 *
 * The code for the case where the capability is present will be
 * assembled into a special section to be used for dynamic patching.
 * Code for that case must:
 *
 * 1. Be exactly the same length (in bytes) as the default code
 *    sequence.
 *
 * 2. Not contain a branch target that is used outside of the
 *    alternative sequence it is defined in (branches into an
 *    alternative sequence are not fixed up).
 */

/*
 * Begin an alternative code sequence.
 */
.macro alternative_if_not cap
	.set .Lasm_alt_mode, 0
	.pushsection .altinstructions, "a"
	altinstruction_entry 661f, 663f, \cap, 662f-661f, 664f-663f
	.popsection
661:
.endm

.macro alternative_if cap
	.set .Lasm_alt_mode, 1
	.pushsection .altinstructions, "a"
	altinstruction_entry 663f, 661f, \cap, 664f-663f, 662f-661f
	.popsection
	.subsection 1
	.align 2	/* So GAS knows label 661 is suitably aligned */
661:
.endm

.macro alternative_cb cb
	.set .Lasm_alt_mode, 0
	.pushsection .altinstructions, "a"
	altinstruction_entry 661f, \cb, ARM64_CB_PATCH, 662f-661f, 0
	.popsection
661:
.endm

/*
 * Provide the other half of the alternative code sequence.
 */
.macro alternative_else
662:
	.if .Lasm_alt_mode==0
	.subsection 1
	.else
	.previous
	.endif
663:
.endm

/*
 * Complete an alternative code sequence.
 */
.macro alternative_endif
664:
	.org	. - (664b-663b) + (662b-661b)
	.org	. - (662b-661b) + (664b-663b)
	.if .Lasm_alt_mode==0
	.previous
	.endif
.endm

/*
 * Callback-based alternative epilogue
 */
.macro alternative_cb_end
662:
.endm

/*
 * Provides a trivial alternative or default sequence consisting solely
 * of NOPs. The number of NOPs is chosen automatically to match the
 * previous case.
 */
.macro alternative_else_nop_endif
alternative_else
	nops	(662b-661b) / AARCH64_INSN_SIZE
alternative_endif
.endm

#define _ALTERNATIVE_CFG(insn1, insn2, cap, cfg, ...)	\
	alternative_insn insn1, insn2, cap, IS_ENABLED(cfg)

.macro user_alt, label, oldinstr, newinstr, cond
9999:	alternative_insn "\oldinstr", "\newinstr", \cond
	_asm_extable 9999b, \label
.endm

#endif  /*  __ASSEMBLY__  */

/*
 * Usage: asm(ALTERNATIVE(oldinstr, newinstr, feature));
 *
 * Usage: asm(ALTERNATIVE(oldinstr, newinstr, feature, CONFIG_FOO));
 * N.B. If CONFIG_FOO is specified, but not selected, the whole block
 *      will be omitted, including oldinstr.
 */
#define ALTERNATIVE(oldinstr, newinstr, ...)   \
	_ALTERNATIVE_CFG(oldinstr, newinstr, __VA_ARGS__, 1)

#endif /* __ASM_ALTERNATIVE_MACROS_H */
