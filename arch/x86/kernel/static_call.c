// SPDX-License-Identifier: GPL-2.0
#include <linux/static_call.h>
#include <linux/memory.h>
#include <linux/bug.h>
#include <asm/text-patching.h>

enum insn_type {
	CALL = 0, /* site call */
	NOP = 1,  /* site cond-call */
	JMP = 2,  /* tramp / site tail-call */
	RET = 3,  /* tramp / site cond-tail-call */
};

/*
 * data16 data16 xorq %rax, %rax - a single 5 byte instruction that clears %rax
 * The REX.W cancels the effect of any data16.
 */
static const u8 xor5rax[] = { 0x66, 0x66, 0x48, 0x31, 0xc0 };

static void __ref __static_call_transform(void *insn, enum insn_type type, void *func)
{
	const void *emulate = NULL;
	int size = CALL_INSN_SIZE;
	const void *code;

	switch (type) {
	case CALL:
		code = text_gen_insn(CALL_INSN_OPCODE, insn, func);
		if (func == &__static_call_return0) {
			emulate = code;
			code = &xor5rax;
		}

		break;

	case NOP:
		code = x86_nops[5];
		break;

	case JMP:
		code = text_gen_insn(JMP32_INSN_OPCODE, insn, func);
		break;

	case RET:
		code = text_gen_insn(RET_INSN_OPCODE, insn, func);
		size = RET_INSN_SIZE;
		break;
	}

	if (memcmp(insn, code, size) == 0)
		return;

	if (unlikely(system_state == SYSTEM_BOOTING))
		return text_poke_early(insn, code, size);

	text_poke_bp(insn, code, size, emulate);
}

static void __static_call_validate(void *insn, bool tail)
{
	u8 opcode = *(u8 *)insn;

	if (tail) {
		if (opcode == JMP32_INSN_OPCODE ||
		    opcode == RET_INSN_OPCODE)
			return;
	} else {
		if (opcode == CALL_INSN_OPCODE ||
		    !memcmp(insn, x86_nops[5], 5) ||
		    !memcmp(insn, xor5rax, 5))
			return;
	}

	/*
	 * If we ever trigger this, our text is corrupt, we'll probably not live long.
	 */
	WARN_ONCE(1, "unexpected static_call insn opcode 0x%x at %pS\n", opcode, insn);
}

static inline enum insn_type __sc_insn(bool null, bool tail)
{
	/*
	 * Encode the following table without branches:
	 *
	 *	tail	null	insn
	 *	-----+-------+------
	 *	  0  |   0   |  CALL
	 *	  0  |   1   |  NOP
	 *	  1  |   0   |  JMP
	 *	  1  |   1   |  RET
	 */
	return 2*tail + null;
}

void arch_static_call_transform(void *site, void *tramp, void *func, bool tail)
{
	mutex_lock(&text_mutex);

	if (tramp) {
		__static_call_validate(tramp, true);
		__static_call_transform(tramp, __sc_insn(!func, true), func);
	}

	if (IS_ENABLED(CONFIG_HAVE_STATIC_CALL_INLINE) && site) {
		__static_call_validate(site, tail);
		__static_call_transform(site, __sc_insn(!func, tail), func);
	}

	mutex_unlock(&text_mutex);
}
EXPORT_SYMBOL_GPL(arch_static_call_transform);
