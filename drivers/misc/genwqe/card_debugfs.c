// SPDX-License-Identifier: GPL-2.0-only
/*
 * IBM Accelerator Family 'GenWQE'
 *
 * (C) Copyright IBM Corp. 2013
 *
 * Author: Frank Haverkamp <haver@linux.vnet.ibm.com>
 * Author: Joerg-Stephan Vogt <jsvogt@de.ibm.com>
 * Author: Michael Jung <mijung@gmx.net>
 * Author: Michael Ruettger <michael@ibmra.de>
 */

/*
 * Debugfs interfaces for the GenWQE card. Help to debug potential
 * problems. Dump internal chip state for debugging and failure
 * determination.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/debugfs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>

#include "card_base.h"
#include "card_ddcb.h"

static void dbg_uidn_show(struct seq_file *s, struct genwqe_reg *regs,
			  int entries)
{
	unsigned int i;
	u32 v_hi, v_lo;

	for (i = 0; i < entries; i++) {
		v_hi = (regs[i].val >> 32) & 0xffffffff;
		v_lo = (regs[i].val)       & 0xffffffff;

		seq_printf(s, "  0x%08x 0x%08x 0x%08x 0x%08x EXT_ERR_REC\n",
			   regs[i].addr, regs[i].idx, v_hi, v_lo);
	}
}

static int curr_dbg_uidn_show(struct seq_file *s, void *unused, int uid)
{
	struct genwqe_dev *cd = s->private;
	int entries;
	struct genwqe_reg *regs;

	entries = genwqe_ffdc_buff_size(cd, uid);
	if (entries < 0)
		return -EINVAL;

	if (entries == 0)
		return 0;

	regs = kcalloc(entries, sizeof(*regs), GFP_KERNEL);
	if (regs == NULL)
		return -ENOMEM;

	genwqe_stop_traps(cd); /* halt the traps while dumping data */
	genwqe_ffdc_buff_read(cd, uid, regs, entries);
	genwqe_start_traps(cd);

	dbg_uidn_show(s, regs, entries);
	kfree(regs);
	return 0;
}

static int curr_dbg_uid0_show(struct seq_file *s, void *unused)
{
	return curr_dbg_uidn_show(s, unused, 0);
}

DEFINE_SHOW_ATTRIBUTE(curr_dbg_uid0);

static int curr_dbg_uid1_show(struct seq_file *s, void *unused)
{
	return curr_dbg_uidn_show(s, unused, 1);
}

DEFINE_SHOW_ATTRIBUTE(curr_dbg_uid1);

static int curr_dbg_uid2_show(struct seq_file *s, void *unused)
{
	return curr_dbg_uidn_show(s, unused, 2);
}

DEFINE_SHOW_ATTRIBUTE(curr_dbg_uid2);

static int prev_dbg_uidn_show(struct seq_file *s, void *unused, int uid)
{
	struct genwqe_dev *cd = s->private;

	dbg_uidn_show(s, cd->ffdc[uid].regs,  cd->ffdc[uid].entries);
	return 0;
}

static int prev_dbg_uid0_show(struct seq_file *s, void *unused)
{
	return prev_dbg_uidn_show(s, unused, 0);
}

DEFINE_SHOW_ATTRIBUTE(prev_dbg_uid0);

static int prev_dbg_uid1_show(struct seq_file *s, void *unused)
{
	return prev_dbg_uidn_show(s, unused, 1);
}

DEFINE_SHOW_ATTRIBUTE(prev_dbg_uid1);

static int prev_dbg_uid2_show(struct seq_file *s, void *unused)
{
	return prev_dbg_uidn_show(s, unused, 2);
}

DEFINE_SHOW_ATTRIBUTE(prev_dbg_uid2);

static int curr_regs_show(struct seq_file *s, void *unused)
{
	struct genwqe_dev *cd = s->private;
	unsigned int i;
	struct genwqe_reg *regs;

	regs = kcalloc(GENWQE_FFDC_REGS, sizeof(*regs), GFP_KERNEL);
	if (regs == NULL)
		return -ENOMEM;

	genwqe_stop_traps(cd);
	genwqe_read_ffdc_regs(cd, regs, GENWQE_FFDC_REGS, 1);
	genwqe_start_traps(cd);

	for (i = 0; i < GENWQE_FFDC_REGS; i++) {
		if (regs[i].addr == 0xffffffff)
			break;  /* invalid entries */

		if (regs[i].val == 0x0ull)
			continue;  /* do not print 0x0 FIRs */

		seq_printf(s, "  0x%08x 0x%016llx\n",
			   regs[i].addr, regs[i].val);
	}
	return 0;
}

DEFINE_SHOW_ATTRIBUTE(curr_regs);

static int prev_regs_show(struct seq_file *s, void *unused)
{
	struct genwqe_dev *cd = s->private;
	unsigned int i;
	struct genwqe_reg *regs = cd->ffdc[GENWQE_DBG_REGS].regs;

	if (regs == NULL)
		return -EINVAL;

	for (i = 0; i < GENWQE_FFDC_REGS; i++) {
		if (regs[i].addr == 0xffffffff)
			break;  /* invalid entries */

		if (regs[i].val == 0x0ull)
			continue;  /* do not print 0x0 FIRs */

		seq_printf(s, "  0x%08x 0x%016llx\n",
			   regs[i].addr, regs[i].val);
	}
	return 0;
}

DEFINE_SHOW_ATTRIBUTE(prev_regs);

static int jtimer_show(struct seq_file *s, void *unused)
{
	struct genwqe_dev *cd = s->private;
	unsigned int vf_num;
	u64 jtimer;

	jtimer = genwqe_read_vreg(cd, IO_SLC_VF_APPJOB_TIMEOUT, 0);
	seq_printf(s, "  PF   0x%016llx %d msec\n", jtimer,
		   GENWQE_PF_JOBTIMEOUT_MSEC);

	for (vf_num = 0; vf_num < cd->num_vfs; vf_num++) {
		jtimer = genwqe_read_vreg(cd, IO_SLC_VF_APPJOB_TIMEOUT,
					  vf_num + 1);
		seq_printf(s, "  VF%-2d 0x%016llx %d msec\n", vf_num, jtimer,
			   cd->vf_jobtimeout_msec[vf_num]);
	}
	return 0;
}

DEFINE_SHOW_ATTRIBUTE(jtimer);

static int queue_working_time_show(struct seq_file *s, void *unused)
{
	struct genwqe_dev *cd = s->private;
	unsigned int vf_num;
	u64 t;

	t = genwqe_read_vreg(cd, IO_SLC_VF_QUEUE_WTIME, 0);
	seq_printf(s, "  PF   0x%016llx\n", t);

	for (vf_num = 0; vf_num < cd->num_vfs; vf_num++) {
		t = genwqe_read_vreg(cd, IO_SLC_VF_QUEUE_WTIME, vf_num + 1);
		seq_printf(s, "  VF%-2d 0x%016llx\n", vf_num, t);
	}
	return 0;
}

DEFINE_SHOW_ATTRIBUTE(queue_working_time);

static int ddcb_info_show(struct seq_file *s, void *unused)
{
	struct genwqe_dev *cd = s->private;
	unsigned int i;
	struct ddcb_queue *queue;
	struct ddcb *pddcb;

	queue = &cd->queue;
	seq_puts(s, "DDCB QUEUE:\n");
	seq_printf(s, "  ddcb_max:            %d\n"
		   "  ddcb_daddr:          %016llx - %016llx\n"
		   "  ddcb_vaddr:          %p\n"
		   "  ddcbs_in_flight:     %u\n"
		   "  ddcbs_max_in_flight: %u\n"
		   "  ddcbs_completed:     %u\n"
		   "  return_on_busy:      %u\n"
		   "  wait_on_busy:        %u\n"
		   "  irqs_processed:      %u\n",
		   queue->ddcb_max, (long long)queue->ddcb_daddr,
		   (long long)queue->ddcb_daddr +
		   (queue->ddcb_max * DDCB_LENGTH),
		   queue->ddcb_vaddr, queue->ddcbs_in_flight,
		   queue->ddcbs_max_in_flight, queue->ddcbs_completed,
		   queue->return_on_busy, queue->wait_on_busy,
		   cd->irqs_processed);

	/* Hardware State */
	seq_printf(s, "  0x%08x 0x%016llx IO_QUEUE_CONFIG\n"
		   "  0x%08x 0x%016llx IO_QUEUE_STATUS\n"
		   "  0x%08x 0x%016llx IO_QUEUE_SEGMENT\n"
		   "  0x%08x 0x%016llx IO_QUEUE_INITSQN\n"
		   "  0x%08x 0x%016llx IO_QUEUE_WRAP\n"
		   "  0x%08x 0x%016llx IO_QUEUE_OFFSET\n"
		   "  0x%08x 0x%016llx IO_QUEUE_WTIME\n"
		   "  0x%08x 0x%016llx IO_QUEUE_ERRCNTS\n"
		   "  0x%08x 0x%016llx IO_QUEUE_LRW\n",
		   queue->IO_QUEUE_CONFIG,
		   __genwqe_readq(cd, queue->IO_QUEUE_CONFIG),
		   queue->IO_QUEUE_STATUS,
		   __genwqe_readq(cd, queue->IO_QUEUE_STATUS),
		   queue->IO_QUEUE_SEGMENT,
		   __genwqe_readq(cd, queue->IO_QUEUE_SEGMENT),
		   queue->IO_QUEUE_INITSQN,
		   __genwqe_readq(cd, queue->IO_QUEUE_INITSQN),
		   queue->IO_QUEUE_WRAP,
		   __genwqe_readq(cd, queue->IO_QUEUE_WRAP),
		   queue->IO_QUEUE_OFFSET,
		   __genwqe_readq(cd, queue->IO_QUEUE_OFFSET),
		   queue->IO_QUEUE_WTIME,
		   __genwqe_readq(cd, queue->IO_QUEUE_WTIME),
		   queue->IO_QUEUE_ERRCNTS,
		   __genwqe_readq(cd, queue->IO_QUEUE_ERRCNTS),
		   queue->IO_QUEUE_LRW,
		   __genwqe_readq(cd, queue->IO_QUEUE_LRW));

	seq_printf(s, "DDCB list (ddcb_act=%d/ddcb_next=%d):\n",
		   queue->ddcb_act, queue->ddcb_next);

	pddcb = queue->ddcb_vaddr;
	for (i = 0; i < queue->ddcb_max; i++) {
		seq_printf(s, "  %-3d: RETC=%03x SEQ=%04x HSI/SHI=%02x/%02x ",
			   i, be16_to_cpu(pddcb->retc_16),
			   be16_to_cpu(pddcb->seqnum_16),
			   pddcb->hsi, pddcb->shi);
		seq_printf(s, "PRIV=%06llx CMD=%02x\n",
			   be64_to_cpu(pddcb->priv_64), pddcb->cmd);
		pddcb++;
	}
	return 0;
}

DEFINE_SHOW_ATTRIBUTE(ddcb_info);

static int info_show(struct seq_file *s, void *unused)
{
	struct genwqe_dev *cd = s->private;
	u64 app_id, slu_id, bitstream = -1;
	struct pci_dev *pci_dev = cd->pci_dev;

	slu_id = __genwqe_readq(cd, IO_SLU_UNITCFG);
	app_id = __genwqe_readq(cd, IO_APP_UNITCFG);

	if (genwqe_is_privileged(cd))
		bitstream = __genwqe_readq(cd, IO_SLU_BITSTREAM);

	seq_printf(s, "%s driver version: %s\n"
		   "    Device Name/Type: %s %s CardIdx: %d\n"
		   "    SLU/APP Config  : 0x%016llx/0x%016llx\n"
		   "    Build Date      : %u/%x/%u\n"
		   "    Base Clock      : %u MHz\n"
		   "    Arch/SVN Release: %u/%llx\n"
		   "    Bitstream       : %llx\n",
		   GENWQE_DEVNAME, DRV_VERSION, dev_name(&pci_dev->dev),
		   genwqe_is_privileged(cd) ?
		   "Physical" : "Virtual or no SR-IOV",
		   cd->card_idx, slu_id, app_id,
		   (u16)((slu_id >> 12) & 0x0fLLU),	   /* month */
		   (u16)((slu_id >>  4) & 0xffLLU),	   /* day */
		   (u16)((slu_id >> 16) & 0x0fLLU) + 2010, /* year */
		   genwqe_base_clock_frequency(cd),
		   (u16)((slu_id >> 32) & 0xffLLU), slu_id >> 40,
		   bitstream);

	return 0;
}

DEFINE_SHOW_ATTRIBUTE(info);

void genwqe_init_debugfs(struct genwqe_dev *cd)
{
	struct dentry *root;
	char card_name[64];
	char name[64];
	unsigned int i;

	sprintf(card_name, "%s%d_card", GENWQE_DEVNAME, cd->card_idx);

	root = debugfs_create_dir(card_name, cd->debugfs_genwqe);

	/* non privileged interfaces are done here */
	debugfs_create_file("ddcb_info", S_IRUGO, root, cd, &ddcb_info_fops);
	debugfs_create_file("info", S_IRUGO, root, cd, &info_fops);
	debugfs_create_x64("err_inject", 0666, root, &cd->err_inject);
	debugfs_create_u32("ddcb_software_timeout", 0666, root,
			   &cd->ddcb_software_timeout);
	debugfs_create_u32("kill_timeout", 0666, root, &cd->kill_timeout);

	/* privileged interfaces follow here */
	if (!genwqe_is_privileged(cd)) {
		cd->debugfs_root = root;
		return;
	}

	debugfs_create_file("curr_regs", S_IRUGO, root, cd, &curr_regs_fops);
	debugfs_create_file("curr_dbg_uid0", S_IRUGO, root, cd,
			    &curr_dbg_uid0_fops);
	debugfs_create_file("curr_dbg_uid1", S_IRUGO, root, cd,
			    &curr_dbg_uid1_fops);
	debugfs_create_file("curr_dbg_uid2", S_IRUGO, root, cd,
			    &curr_dbg_uid2_fops);
	debugfs_create_file("prev_regs", S_IRUGO, root, cd, &prev_regs_fops);
	debugfs_create_file("prev_dbg_uid0", S_IRUGO, root, cd,
			    &prev_dbg_uid0_fops);
	debugfs_create_file("prev_dbg_uid1", S_IRUGO, root, cd,
			    &prev_dbg_uid1_fops);
	debugfs_create_file("prev_dbg_uid2", S_IRUGO, root, cd,
			    &prev_dbg_uid2_fops);

	for (i = 0; i <  GENWQE_MAX_VFS; i++) {
		sprintf(name, "vf%u_jobtimeout_msec", i);
		debugfs_create_u32(name, 0666, root,
				   &cd->vf_jobtimeout_msec[i]);
	}

	debugfs_create_file("jobtimer", S_IRUGO, root, cd, &jtimer_fops);
	debugfs_create_file("queue_working_time", S_IRUGO, root, cd,
			    &queue_working_time_fops);
	debugfs_create_u32("skip_recovery", 0666, root, &cd->skip_recovery);
	debugfs_create_u32("use_platform_recovery", 0666, root,
			   &cd->use_platform_recovery);

	cd->debugfs_root = root;
}

void genqwe_exit_debugfs(struct genwqe_dev *cd)
{
	debugfs_remove_recursive(cd->debugfs_root);
}
