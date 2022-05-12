// SPDX-License-Identifier: GPL-2.0-only
/*
 * AArch64 KGDB support
 *
 * Based on arch/arm/kernel/kgdb.c
 *
 * Copyright (C) 2013 Cavium Inc.
 * Author: Vijaya Kumar K <vijaya.kumar@caviumnetworks.com>
 */

#include <linux/bug.h>
#include <linux/irq.h>
#include <linux/kdebug.h>
#include <linux/kgdb.h>
#include <linux/kprobes.h>
#include <linux/sched/task_stack.h>

#include <asm/debug-monitors.h>
#include <asm/insn.h>
#include <asm/traps.h>

struct dbg_reg_def_t dbg_reg_def[DBG_MAX_REG_NUM] = {
	{ "x0", 8, offsetof(struct pt_regs, regs[0])},
	{ "x1", 8, offsetof(struct pt_regs, regs[1])},
	{ "x2", 8, offsetof(struct pt_regs, regs[2])},
	{ "x3", 8, offsetof(struct pt_regs, regs[3])},
	{ "x4", 8, offsetof(struct pt_regs, regs[4])},
	{ "x5", 8, offsetof(struct pt_regs, regs[5])},
	{ "x6", 8, offsetof(struct pt_regs, regs[6])},
	{ "x7", 8, offsetof(struct pt_regs, regs[7])},
	{ "x8", 8, offsetof(struct pt_regs, regs[8])},
	{ "x9", 8, offsetof(struct pt_regs, regs[9])},
	{ "x10", 8, offsetof(struct pt_regs, regs[10])},
	{ "x11", 8, offsetof(struct pt_regs, regs[11])},
	{ "x12", 8, offsetof(struct pt_regs, regs[12])},
	{ "x13", 8, offsetof(struct pt_regs, regs[13])},
	{ "x14", 8, offsetof(struct pt_regs, regs[14])},
	{ "x15", 8, offsetof(struct pt_regs, regs[15])},
	{ "x16", 8, offsetof(struct pt_regs, regs[16])},
	{ "x17", 8, offsetof(struct pt_regs, regs[17])},
	{ "x18", 8, offsetof(struct pt_regs, regs[18])},
	{ "x19", 8, offsetof(struct pt_regs, regs[19])},
	{ "x20", 8, offsetof(struct pt_regs, regs[20])},
	{ "x21", 8, offsetof(struct pt_regs, regs[21])},
	{ "x22", 8, offsetof(struct pt_regs, regs[22])},
	{ "x23", 8, offsetof(struct pt_regs, regs[23])},
	{ "x24", 8, offsetof(struct pt_regs, regs[24])},
	{ "x25", 8, offsetof(struct pt_regs, regs[25])},
	{ "x26", 8, offsetof(struct pt_regs, regs[26])},
	{ "x27", 8, offsetof(struct pt_regs, regs[27])},
	{ "x28", 8, offsetof(struct pt_regs, regs[28])},
	{ "x29", 8, offsetof(struct pt_regs, regs[29])},
	{ "x30", 8, offsetof(struct pt_regs, regs[30])},
	{ "sp", 8, offsetof(struct pt_regs, sp)},
	{ "pc", 8, offsetof(struct pt_regs, pc)},
	/*
	 * struct pt_regs thinks PSTATE is 64-bits wide but gdb remote
	 * protocol disagrees. Therefore we must extract only the lower
	 * 32-bits. Look for the big comment in asm/kgdb.h for more
	 * detail.
	 */
	{ "pstate", 4, offsetof(struct pt_regs, pstate)
#ifdef CONFIG_CPU_BIG_ENDIAN
							+ 4
#endif
	},
	{ "v0", 16, -1 },
	{ "v1", 16, -1 },
	{ "v2", 16, -1 },
	{ "v3", 16, -1 },
	{ "v4", 16, -1 },
	{ "v5", 16, -1 },
	{ "v6", 16, -1 },
	{ "v7", 16, -1 },
	{ "v8", 16, -1 },
	{ "v9", 16, -1 },
	{ "v10", 16, -1 },
	{ "v11", 16, -1 },
	{ "v12", 16, -1 },
	{ "v13", 16, -1 },
	{ "v14", 16, -1 },
	{ "v15", 16, -1 },
	{ "v16", 16, -1 },
	{ "v17", 16, -1 },
	{ "v18", 16, -1 },
	{ "v19", 16, -1 },
	{ "v20", 16, -1 },
	{ "v21", 16, -1 },
	{ "v22", 16, -1 },
	{ "v23", 16, -1 },
	{ "v24", 16, -1 },
	{ "v25", 16, -1 },
	{ "v26", 16, -1 },
	{ "v27", 16, -1 },
	{ "v28", 16, -1 },
	{ "v29", 16, -1 },
	{ "v30", 16, -1 },
	{ "v31", 16, -1 },
	{ "fpsr", 4, -1 },
	{ "fpcr", 4, -1 },
};

char *dbg_get_reg(int regno, void *mem, struct pt_regs *regs)
{
	if (regno >= DBG_MAX_REG_NUM || regno < 0)
		return NULL;

	if (dbg_reg_def[regno].offset != -1)
		memcpy(mem, (void *)regs + dbg_reg_def[regno].offset,
		       dbg_reg_def[regno].size);
	else
		memset(mem, 0, dbg_reg_def[regno].size);
	return dbg_reg_def[regno].name;
}

int dbg_set_reg(int regno, void *mem, struct pt_regs *regs)
{
	if (regno >= DBG_MAX_REG_NUM || regno < 0)
		return -EINVAL;

	if (dbg_reg_def[regno].offset != -1)
		memcpy((void *)regs + dbg_reg_def[regno].offset, mem,
		       dbg_reg_def[regno].size);
	return 0;
}

void
sleeping_thread_to_gdb_regs(unsigned long *gdb_regs, struct task_struct *task)
{
	struct cpu_context *cpu_context = &task->thread.cpu_context;

	/* Initialize to zero */
	memset((char *)gdb_regs, 0, NUMREGBYTES);

	gdb_regs[19] = cpu_context->x19;
	gdb_regs[20] = cpu_context->x20;
	gdb_regs[21] = cpu_context->x21;
	gdb_regs[22] = cpu_context->x22;
	gdb_regs[23] = cpu_context->x23;
	gdb_regs[24] = cpu_context->x24;
	gdb_regs[25] = cpu_context->x25;
	gdb_regs[26] = cpu_context->x26;
	gdb_regs[27] = cpu_context->x27;
	gdb_regs[28] = cpu_context->x28;
	gdb_regs[29] = cpu_context->fp;

	gdb_regs[31] = cpu_context->sp;
	gdb_regs[32] = cpu_context->pc;
}

void kgdb_arch_set_pc(struct pt_regs *regs, unsigned long pc)
{
	regs->pc = pc;
}

static int compiled_break;

static void kgdb_arch_update_addr(struct pt_regs *regs,
				char *remcom_in_buffer)
{
	unsigned long addr;
	char *ptr;

	ptr = &remcom_in_buffer[1];
	if (kgdb_hex2long(&ptr, &addr))
		kgdb_arch_set_pc(regs, addr);
	else if (compiled_break == 1)
		kgdb_arch_set_pc(regs, regs->pc + 4);

	compiled_break = 0;
}

int kgdb_arch_handle_exception(int exception_vector, int signo,
			       int err_code, char *remcom_in_buffer,
			       char *remcom_out_buffer,
			       struct pt_regs *linux_regs)
{
	int err;

	switch (remcom_in_buffer[0]) {
	case 'D':
	case 'k':
		/*
		 * Packet D (Detach), k (kill). No special handling
		 * is required here. Handle same as c packet.
		 */
	case 'c':
		/*
		 * Packet c (Continue) to continue executing.
		 * Set pc to required address.
		 * Try to read optional parameter and set pc.
		 * If this was a compiled breakpoint, we need to move
		 * to the next instruction else we will just breakpoint
		 * over and over again.
		 */
		kgdb_arch_update_addr(linux_regs, remcom_in_buffer);
		atomic_set(&kgdb_cpu_doing_single_step, -1);
		kgdb_single_step =  0;

		/*
		 * Received continue command, disable single step
		 */
		if (kernel_active_single_step())
			kernel_disable_single_step();

		err = 0;
		break;
	case 's':
		/*
		 * Update step address value with address passed
		 * with step packet.
		 * On debug exception return PC is copied to ELR
		 * So just update PC.
		 * If no step address is passed, resume from the address
		 * pointed by PC. Do not update PC
		 */
		kgdb_arch_update_addr(linux_regs, remcom_in_buffer);
		atomic_set(&kgdb_cpu_doing_single_step, raw_smp_processor_id());
		kgdb_single_step =  1;

		/*
		 * Enable single step handling
		 */
		if (!kernel_active_single_step())
			kernel_enable_single_step(linux_regs);
		err = 0;
		break;
	default:
		err = -1;
	}
	return err;
}

static int kgdb_brk_fn(struct pt_regs *regs, unsigned int esr)
{
	kgdb_handle_exception(1, SIGTRAP, 0, regs);
	return DBG_HOOK_HANDLED;
}
NOKPROBE_SYMBOL(kgdb_brk_fn)

static int kgdb_compiled_brk_fn(struct pt_regs *regs, unsigned int esr)
{
	compiled_break = 1;
	kgdb_handle_exception(1, SIGTRAP, 0, regs);

	return DBG_HOOK_HANDLED;
}
NOKPROBE_SYMBOL(kgdb_compiled_brk_fn);

static int kgdb_step_brk_fn(struct pt_regs *regs, unsigned int esr)
{
	if (!kgdb_single_step)
		return DBG_HOOK_ERROR;

	kgdb_handle_exception(0, SIGTRAP, 0, regs);
	return DBG_HOOK_HANDLED;
}
NOKPROBE_SYMBOL(kgdb_step_brk_fn);

static struct break_hook kgdb_brkpt_hook = {
	.fn		= kgdb_brk_fn,
	.imm		= KGDB_DYN_DBG_BRK_IMM,
};

static struct break_hook kgdb_compiled_brkpt_hook = {
	.fn		= kgdb_compiled_brk_fn,
	.imm		= KGDB_COMPILED_DBG_BRK_IMM,
};

static struct step_hook kgdb_step_hook = {
	.fn		= kgdb_step_brk_fn
};

static int __kgdb_notify(struct die_args *args, unsigned long cmd)
{
	struct pt_regs *regs = args->regs;

	if (kgdb_handle_exception(1, args->signr, cmd, regs))
		return NOTIFY_DONE;
	return NOTIFY_STOP;
}

static int
kgdb_notify(struct notifier_block *self, unsigned long cmd, void *ptr)
{
	unsigned long flags;
	int ret;

	local_irq_save(flags);
	ret = __kgdb_notify(ptr, cmd);
	local_irq_restore(flags);

	return ret;
}

static struct notifier_block kgdb_notifier = {
	.notifier_call	= kgdb_notify,
	/*
	 * Want to be lowest priority
	 */
	.priority	= -INT_MAX,
};

/*
 * kgdb_arch_init - Perform any architecture specific initialization.
 * This function will handle the initialization of any architecture
 * specific callbacks.
 */
int kgdb_arch_init(void)
{
	int ret = register_die_notifier(&kgdb_notifier);

	if (ret != 0)
		return ret;

	register_kernel_break_hook(&kgdb_brkpt_hook);
	register_kernel_break_hook(&kgdb_compiled_brkpt_hook);
	register_kernel_step_hook(&kgdb_step_hook);
	return 0;
}

/*
 * kgdb_arch_exit - Perform any architecture specific uninitalization.
 * This function will handle the uninitalization of any architecture
 * specific callbacks, for dynamic registration and unregistration.
 */
void kgdb_arch_exit(void)
{
	unregister_kernel_break_hook(&kgdb_brkpt_hook);
	unregister_kernel_break_hook(&kgdb_compiled_brkpt_hook);
	unregister_kernel_step_hook(&kgdb_step_hook);
	unregister_die_notifier(&kgdb_notifier);
}

const struct kgdb_arch arch_kgdb_ops;

int kgdb_arch_set_breakpoint(struct kgdb_bkpt *bpt)
{
	int err;

	BUILD_BUG_ON(AARCH64_INSN_SIZE != BREAK_INSTR_SIZE);

	err = aarch64_insn_read((void *)bpt->bpt_addr, (u32 *)bpt->saved_instr);
	if (err)
		return err;

	return aarch64_insn_write((void *)bpt->bpt_addr,
			(u32)AARCH64_BREAK_KGDB_DYN_DBG);
}

int kgdb_arch_remove_breakpoint(struct kgdb_bkpt *bpt)
{
	return aarch64_insn_write((void *)bpt->bpt_addr,
			*(u32 *)bpt->saved_instr);
}
