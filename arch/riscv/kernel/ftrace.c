// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2013 Linaro Limited
 * Author: AKASHI Takahiro <takahiro.akashi@linaro.org>
 * Copyright (C) 2017 Andes Technology Corporation
 */

#include <linux/ftrace.h>
#include <linux/uaccess.h>
#include <linux/memory.h>
#include <asm/cacheflush.h>
#include <asm/patch.h>

#ifdef CONFIG_DYNAMIC_FTRACE
int ftrace_arch_code_modify_prepare(void) __acquires(&text_mutex)
{
	mutex_lock(&text_mutex);
	return 0;
}

int ftrace_arch_code_modify_post_process(void) __releases(&text_mutex)
{
	mutex_unlock(&text_mutex);
	return 0;
}

static int ftrace_check_current_call(unsigned long hook_pos,
				     unsigned int *expected)
{
	unsigned int replaced[2];
	unsigned int nops[2] = {NOP4, NOP4};

	/* we expect nops at the hook position */
	if (!expected)
		expected = nops;

	/*
	 * Read the text we want to modify;
	 * return must be -EFAULT on read error
	 */
	if (copy_from_kernel_nofault(replaced, (void *)hook_pos,
			MCOUNT_INSN_SIZE))
		return -EFAULT;

	/*
	 * Make sure it is what we expect it to be;
	 * return must be -EINVAL on failed comparison
	 */
	if (memcmp(expected, replaced, sizeof(replaced))) {
		pr_err("%p: expected (%08x %08x) but got (%08x %08x)\n",
		       (void *)hook_pos, expected[0], expected[1], replaced[0],
		       replaced[1]);
		return -EINVAL;
	}

	return 0;
}

static int __ftrace_modify_call(unsigned long hook_pos, unsigned long target,
				bool enable)
{
	unsigned int call[2];
	unsigned int nops[2] = {NOP4, NOP4};

	make_call(hook_pos, target, call);

	/* Replace the auipc-jalr pair at once. Return -EPERM on write error. */
	if (patch_text_nosync
	    ((void *)hook_pos, enable ? call : nops, MCOUNT_INSN_SIZE))
		return -EPERM;

	return 0;
}

/*
 * Put 5 instructions with 16 bytes at the front of function within
 * patchable function entry nops' area.
 *
 * 0: REG_S  ra, -SZREG(sp)
 * 1: auipc  ra, 0x?
 * 2: jalr   -?(ra)
 * 3: REG_L  ra, -SZREG(sp)
 *
 * So the opcodes is:
 * 0: 0xfe113c23 (sd)/0xfe112e23 (sw)
 * 1: 0x???????? -> auipc
 * 2: 0x???????? -> jalr
 * 3: 0xff813083 (ld)/0xffc12083 (lw)
 */
#if __riscv_xlen == 64
#define INSN0	0xfe113c23
#define INSN3	0xff813083
#elif __riscv_xlen == 32
#define INSN0	0xfe112e23
#define INSN3	0xffc12083
#endif

#define FUNC_ENTRY_SIZE	16
#define FUNC_ENTRY_JMP	4

int ftrace_make_call(struct dyn_ftrace *rec, unsigned long addr)
{
	unsigned int call[4] = {INSN0, 0, 0, INSN3};
	unsigned long target = addr;
	unsigned long caller = rec->ip + FUNC_ENTRY_JMP;

	call[1] = to_auipc_insn((unsigned int)(target - caller));
	call[2] = to_jalr_insn((unsigned int)(target - caller));

	if (patch_text_nosync((void *)rec->ip, call, FUNC_ENTRY_SIZE))
		return -EPERM;

	return 0;
}

int ftrace_make_nop(struct module *mod, struct dyn_ftrace *rec,
		    unsigned long addr)
{
	unsigned int nops[4] = {NOP4, NOP4, NOP4, NOP4};

	if (patch_text_nosync((void *)rec->ip, nops, FUNC_ENTRY_SIZE))
		return -EPERM;

	return 0;
}


/*
 * This is called early on, and isn't wrapped by
 * ftrace_arch_code_modify_{prepare,post_process}() and therefor doesn't hold
 * text_mutex, which triggers a lockdep failure.  SMP isn't running so we could
 * just directly poke the text, but it's simpler to just take the lock
 * ourselves.
 */
int ftrace_init_nop(struct module *mod, struct dyn_ftrace *rec)
{
	int out;

	ftrace_arch_code_modify_prepare();
	out = ftrace_make_nop(mod, rec, MCOUNT_ADDR);
	ftrace_arch_code_modify_post_process();

	return out;
}

int ftrace_update_ftrace_func(ftrace_func_t func)
{
	int ret = __ftrace_modify_call((unsigned long)&ftrace_call,
				       (unsigned long)func, true);
	if (!ret) {
		ret = __ftrace_modify_call((unsigned long)&ftrace_regs_call,
					   (unsigned long)func, true);
	}

	return ret;
}

int __init ftrace_dyn_arch_init(void)
{
	return 0;
}
#endif

#ifdef CONFIG_DYNAMIC_FTRACE_WITH_REGS
int ftrace_modify_call(struct dyn_ftrace *rec, unsigned long old_addr,
		       unsigned long addr)
{
	unsigned int call[2];
	unsigned long caller = rec->ip + FUNC_ENTRY_JMP;
	int ret;

	make_call(caller, old_addr, call);
	ret = ftrace_check_current_call(caller, call);

	if (ret)
		return ret;

	return __ftrace_modify_call(caller, addr, true);
}
#endif

#ifdef CONFIG_FUNCTION_GRAPH_TRACER
/*
 * Most of this function is copied from arm64.
 */
void prepare_ftrace_return(unsigned long *parent, unsigned long self_addr,
			   unsigned long frame_pointer)
{
	unsigned long return_hooker = (unsigned long)&return_to_handler;
	unsigned long old;

	if (unlikely(atomic_read(&current->tracing_graph_pause)))
		return;

	/*
	 * We don't suffer access faults, so no extra fault-recovery assembly
	 * is needed here.
	 */
	old = *parent;

	if (!function_graph_enter(old, self_addr, frame_pointer, parent))
		*parent = return_hooker;
}

#ifdef CONFIG_DYNAMIC_FTRACE
extern void ftrace_graph_call(void);
extern void ftrace_graph_regs_call(void);
int ftrace_enable_ftrace_graph_caller(void)
{
	int ret;

	ret = __ftrace_modify_call((unsigned long)&ftrace_graph_call,
				    (unsigned long)&prepare_ftrace_return, true);
	if (ret)
		return ret;

	return __ftrace_modify_call((unsigned long)&ftrace_graph_regs_call,
				    (unsigned long)&prepare_ftrace_return, true);
}

int ftrace_disable_ftrace_graph_caller(void)
{
	int ret;

	ret = __ftrace_modify_call((unsigned long)&ftrace_graph_call,
				    (unsigned long)&prepare_ftrace_return, false);
	if (ret)
		return ret;

	return __ftrace_modify_call((unsigned long)&ftrace_graph_regs_call,
				    (unsigned long)&prepare_ftrace_return, false);
}
#endif /* CONFIG_DYNAMIC_FTRACE */
#endif /* CONFIG_FUNCTION_GRAPH_TRACER */
