/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 1994, 1995, 1996, 1999, 2000 by Ralf Baechle
 * Copyright (C) 1999, 2000 by Silicon Graphics
 * Copyright (C) 2002  Maciej W. Rozycki
 */
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/signal.h>	/* for SIGBUS */
#include <linux/sched.h>	/* schow_regs(), force_sig() */
#include <linux/sched/debug.h>
#include <linux/sched/signal.h>

#include <asm/ptrace.h>
#include <asm/sn/addrs.h>
#include <asm/sn/agent.h>
#include <asm/sn/arch.h>
#include <asm/tlbdebug.h>
#include <asm/traps.h>
#include <linux/uaccess.h>

static void dump_hub_information(unsigned long errst0, unsigned long errst1)
{
	static char *err_type[2][8] = {
		{ NULL, "Uncached Partial Read PRERR", "DERR", "Read Timeout",
		  NULL, NULL, NULL, NULL },
		{ "WERR", "Uncached Partial Write", "PWERR", "Write Timeout",
		  NULL, NULL, NULL, NULL }
	};
	union pi_err_stat0 st0;
	union pi_err_stat1 st1;

	st0.pi_stat0_word = errst0;
	st1.pi_stat1_word = errst1;

	if (!st0.pi_stat0_fmt.s0_valid) {
		pr_info("Hub does not contain valid error information\n");
		return;
	}

	pr_info("Hub has valid error information:\n");
	if (st0.pi_stat0_fmt.s0_ovr_run)
		pr_info("Overrun is set. Error stack may contain additional "
		       "information.\n");
	pr_info("Hub error address is %08lx\n",
		(unsigned long)st0.pi_stat0_fmt.s0_addr);
	pr_info("Incoming message command 0x%lx\n",
		(unsigned long)st0.pi_stat0_fmt.s0_cmd);
	pr_info("Supplemental field of incoming message is 0x%lx\n",
		(unsigned long)st0.pi_stat0_fmt.s0_supl);
	pr_info("T5 Rn (for RRB only) is 0x%lx\n",
		(unsigned long)st0.pi_stat0_fmt.s0_t5_req);
	pr_info("Error type is %s\n", err_type[st1.pi_stat1_fmt.s1_rw_rb]
	       [st0.pi_stat0_fmt.s0_err_type] ? : "invalid");
}

int ip27_be_handler(struct pt_regs *regs, int is_fixup)
{
	unsigned long errst0, errst1;
	int data = regs->cp0_cause & 4;
	int cpu = LOCAL_HUB_L(PI_CPU_NUM);

	if (is_fixup)
		return MIPS_BE_FIXUP;

	printk("Slice %c got %cbe at 0x%lx\n", 'A' + cpu, data ? 'd' : 'i',
	       regs->cp0_epc);
	printk("Hub information:\n");
	printk("ERR_INT_PEND = 0x%06llx\n", LOCAL_HUB_L(PI_ERR_INT_PEND));
	errst0 = LOCAL_HUB_L(cpu ? PI_ERR_STATUS0_B : PI_ERR_STATUS0_A);
	errst1 = LOCAL_HUB_L(cpu ? PI_ERR_STATUS1_B : PI_ERR_STATUS1_A);
	dump_hub_information(errst0, errst1);
	show_regs(regs);
	dump_tlb_all();
	while(1);
	force_sig(SIGBUS);
}

void __init ip27_be_init(void)
{
	/* XXX Initialize all the Hub & Bridge error handling here.  */
	int cpu = LOCAL_HUB_L(PI_CPU_NUM);
	int cpuoff = cpu << 8;

	board_be_handler = ip27_be_handler;

	LOCAL_HUB_S(PI_ERR_INT_PEND,
		    cpu ? PI_ERR_CLEAR_ALL_B : PI_ERR_CLEAR_ALL_A);
	LOCAL_HUB_S(PI_ERR_INT_MASK_A + cpuoff, 0);
	LOCAL_HUB_S(PI_ERR_STACK_ADDR_A + cpuoff, 0);
	LOCAL_HUB_S(PI_ERR_STACK_SIZE, 0);	/* Disable error stack */
	LOCAL_HUB_S(PI_SYSAD_ERRCHK_EN, PI_SYSAD_CHECK_ALL);
}
