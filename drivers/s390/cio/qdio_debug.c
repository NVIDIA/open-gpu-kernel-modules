// SPDX-License-Identifier: GPL-2.0
/*
 *  Copyright IBM Corp. 2008, 2009
 *
 *  Author: Jan Glauber (jang@linux.vnet.ibm.com)
 */
#include <linux/seq_file.h>
#include <linux/debugfs.h>
#include <linux/uaccess.h>
#include <linux/export.h>
#include <linux/slab.h>
#include <asm/debug.h>
#include "qdio_debug.h"
#include "qdio.h"

debug_info_t *qdio_dbf_setup;
debug_info_t *qdio_dbf_error;

static struct dentry *debugfs_root;
#define QDIO_DEBUGFS_NAME_LEN	10
#define QDIO_DBF_NAME_LEN	20

struct qdio_dbf_entry {
	char dbf_name[QDIO_DBF_NAME_LEN];
	debug_info_t *dbf_info;
	struct list_head dbf_list;
};

static LIST_HEAD(qdio_dbf_list);
static DEFINE_MUTEX(qdio_dbf_list_mutex);

static debug_info_t *qdio_get_dbf_entry(char *name)
{
	struct qdio_dbf_entry *entry;
	debug_info_t *rc = NULL;

	mutex_lock(&qdio_dbf_list_mutex);
	list_for_each_entry(entry, &qdio_dbf_list, dbf_list) {
		if (strcmp(entry->dbf_name, name) == 0) {
			rc = entry->dbf_info;
			break;
		}
	}
	mutex_unlock(&qdio_dbf_list_mutex);
	return rc;
}

static void qdio_clear_dbf_list(void)
{
	struct qdio_dbf_entry *entry, *tmp;

	mutex_lock(&qdio_dbf_list_mutex);
	list_for_each_entry_safe(entry, tmp, &qdio_dbf_list, dbf_list) {
		list_del(&entry->dbf_list);
		debug_unregister(entry->dbf_info);
		kfree(entry);
	}
	mutex_unlock(&qdio_dbf_list_mutex);
}

int qdio_allocate_dbf(struct qdio_irq *irq_ptr)
{
	char text[QDIO_DBF_NAME_LEN];
	struct qdio_dbf_entry *new_entry;

	DBF_EVENT("irq:%8lx", (unsigned long)irq_ptr);

	/* allocate trace view for the interface */
	snprintf(text, QDIO_DBF_NAME_LEN, "qdio_%s",
		 dev_name(&irq_ptr->cdev->dev));
	irq_ptr->debug_area = qdio_get_dbf_entry(text);
	if (irq_ptr->debug_area)
		DBF_DEV_EVENT(DBF_ERR, irq_ptr, "dbf reused");
	else {
		irq_ptr->debug_area = debug_register(text, 2, 1, 16);
		if (!irq_ptr->debug_area)
			return -ENOMEM;
		if (debug_register_view(irq_ptr->debug_area,
						&debug_hex_ascii_view)) {
			debug_unregister(irq_ptr->debug_area);
			return -ENOMEM;
		}
		debug_set_level(irq_ptr->debug_area, DBF_WARN);
		DBF_DEV_EVENT(DBF_ERR, irq_ptr, "dbf created");
		new_entry = kzalloc(sizeof(struct qdio_dbf_entry), GFP_KERNEL);
		if (!new_entry) {
			debug_unregister(irq_ptr->debug_area);
			return -ENOMEM;
		}
		strlcpy(new_entry->dbf_name, text, QDIO_DBF_NAME_LEN);
		new_entry->dbf_info = irq_ptr->debug_area;
		mutex_lock(&qdio_dbf_list_mutex);
		list_add(&new_entry->dbf_list, &qdio_dbf_list);
		mutex_unlock(&qdio_dbf_list_mutex);
	}
	return 0;
}

static int qstat_show(struct seq_file *m, void *v)
{
	unsigned char state;
	struct qdio_q *q = m->private;
	int i;

	if (!q)
		return 0;

	seq_printf(m, "Timestamp: %llx\n", q->timestamp);
	seq_printf(m, "Last Data IRQ: %llx  Last AI: %llx\n",
		   q->irq_ptr->last_data_irq_time, last_ai_time);
	seq_printf(m, "nr_used: %d  ftc: %d\n",
		   atomic_read(&q->nr_buf_used), q->first_to_check);
	if (q->is_input_q) {
		seq_printf(m, "batch start: %u  batch count: %u\n",
			   q->u.in.batch_start, q->u.in.batch_count);
		seq_printf(m, "DSCI: %x   IRQs disabled: %u\n",
			   *(u8 *)q->irq_ptr->dsci,
			   test_bit(QDIO_IRQ_DISABLED,
				    &q->irq_ptr->poll_state));
	}
	seq_printf(m, "SBAL states:\n");
	seq_printf(m, "|0      |8      |16     |24     |32     |40     |48     |56  63|\n");

	for (i = 0; i < QDIO_MAX_BUFFERS_PER_Q; i++) {
		debug_get_buf_state(q, i, &state);
		switch (state) {
		case SLSB_P_INPUT_NOT_INIT:
		case SLSB_P_OUTPUT_NOT_INIT:
			seq_printf(m, "N");
			break;
		case SLSB_P_OUTPUT_PENDING:
			seq_printf(m, "P");
			break;
		case SLSB_P_INPUT_PRIMED:
		case SLSB_CU_OUTPUT_PRIMED:
			seq_printf(m, "+");
			break;
		case SLSB_P_INPUT_ACK:
			seq_printf(m, "A");
			break;
		case SLSB_P_INPUT_ERROR:
		case SLSB_P_OUTPUT_ERROR:
			seq_printf(m, "x");
			break;
		case SLSB_CU_INPUT_EMPTY:
		case SLSB_P_OUTPUT_EMPTY:
			seq_printf(m, "-");
			break;
		case SLSB_P_INPUT_HALTED:
		case SLSB_P_OUTPUT_HALTED:
			seq_printf(m, ".");
			break;
		default:
			seq_printf(m, "?");
		}
		if (i == 63)
			seq_printf(m, "\n");
	}
	seq_printf(m, "\n");
	seq_printf(m, "|64     |72     |80     |88     |96     |104    |112    |   127|\n");

	seq_printf(m, "\nSBAL statistics:");
	if (!q->irq_ptr->perf_stat_enabled) {
		seq_printf(m, " disabled\n");
		return 0;
	}

	seq_printf(m, "\n1          2..        4..        8..        "
		   "16..       32..       64..       128\n");
	for (i = 0; i < ARRAY_SIZE(q->q_stats.nr_sbals); i++)
		seq_printf(m, "%-10u ", q->q_stats.nr_sbals[i]);
	seq_printf(m, "\nError      NOP        Total\n%-10u %-10u %-10u\n\n",
		   q->q_stats.nr_sbal_error, q->q_stats.nr_sbal_nop,
		   q->q_stats.nr_sbal_total);
	return 0;
}

DEFINE_SHOW_ATTRIBUTE(qstat);

static int ssqd_show(struct seq_file *m, void *v)
{
	struct ccw_device *cdev = m->private;
	struct qdio_ssqd_desc ssqd;
	int rc;

	rc = qdio_get_ssqd_desc(cdev, &ssqd);
	if (rc)
		return rc;

	seq_hex_dump(m, "", DUMP_PREFIX_NONE, 16, 4, &ssqd, sizeof(ssqd),
		     false);
	return 0;
}

DEFINE_SHOW_ATTRIBUTE(ssqd);

static char *qperf_names[] = {
	"Assumed adapter interrupts",
	"QDIO interrupts",
	"Requested PCIs",
	"Outbound tasklet runs",
	"SIGA read",
	"SIGA write",
	"SIGA sync",
	"Inbound calls",
	"Inbound stop_polling",
	"Inbound queue full",
	"Outbound calls",
	"Outbound handler",
	"Outbound queue full",
	"Outbound fast_requeue",
	"Outbound target_full",
	"QEBSM eqbs",
	"QEBSM eqbs partial",
	"QEBSM sqbs",
	"QEBSM sqbs partial",
	"Discarded interrupts"
};

static int qperf_show(struct seq_file *m, void *v)
{
	struct qdio_irq *irq_ptr = m->private;
	unsigned int *stat;
	int i;

	if (!irq_ptr)
		return 0;
	if (!irq_ptr->perf_stat_enabled) {
		seq_printf(m, "disabled\n");
		return 0;
	}
	stat = (unsigned int *)&irq_ptr->perf_stat;

	for (i = 0; i < ARRAY_SIZE(qperf_names); i++)
		seq_printf(m, "%26s:\t%u\n",
			   qperf_names[i], *(stat + i));
	return 0;
}

static ssize_t qperf_seq_write(struct file *file, const char __user *ubuf,
			       size_t count, loff_t *off)
{
	struct seq_file *seq = file->private_data;
	struct qdio_irq *irq_ptr = seq->private;
	struct qdio_q *q;
	unsigned long val;
	int ret, i;

	if (!irq_ptr)
		return 0;

	ret = kstrtoul_from_user(ubuf, count, 10, &val);
	if (ret)
		return ret;

	switch (val) {
	case 0:
		irq_ptr->perf_stat_enabled = 0;
		memset(&irq_ptr->perf_stat, 0, sizeof(irq_ptr->perf_stat));
		for_each_input_queue(irq_ptr, q, i)
			memset(&q->q_stats, 0, sizeof(q->q_stats));
		for_each_output_queue(irq_ptr, q, i)
			memset(&q->q_stats, 0, sizeof(q->q_stats));
		break;
	case 1:
		irq_ptr->perf_stat_enabled = 1;
		break;
	}
	return count;
}

static int qperf_seq_open(struct inode *inode, struct file *filp)
{
	return single_open(filp, qperf_show,
			   file_inode(filp)->i_private);
}

static const struct file_operations debugfs_perf_fops = {
	.owner	 = THIS_MODULE,
	.open	 = qperf_seq_open,
	.read	 = seq_read,
	.write	 = qperf_seq_write,
	.llseek  = seq_lseek,
	.release = single_release,
};

static void setup_debugfs_entry(struct dentry *parent, struct qdio_q *q)
{
	char name[QDIO_DEBUGFS_NAME_LEN];

	snprintf(name, QDIO_DEBUGFS_NAME_LEN, "%s_%d",
		 q->is_input_q ? "input" : "output",
		 q->nr);
	debugfs_create_file(name, 0444, parent, q, &qstat_fops);
}

void qdio_setup_debug_entries(struct qdio_irq *irq_ptr)
{
	struct qdio_q *q;
	int i;

	irq_ptr->debugfs_dev = debugfs_create_dir(dev_name(&irq_ptr->cdev->dev),
						  debugfs_root);
	debugfs_create_file("statistics", S_IFREG | S_IRUGO | S_IWUSR,
			    irq_ptr->debugfs_dev, irq_ptr, &debugfs_perf_fops);
	debugfs_create_file("ssqd", 0444, irq_ptr->debugfs_dev, irq_ptr->cdev,
			    &ssqd_fops);

	for_each_input_queue(irq_ptr, q, i)
		setup_debugfs_entry(irq_ptr->debugfs_dev, q);
	for_each_output_queue(irq_ptr, q, i)
		setup_debugfs_entry(irq_ptr->debugfs_dev, q);
}

void qdio_shutdown_debug_entries(struct qdio_irq *irq_ptr)
{
	debugfs_remove_recursive(irq_ptr->debugfs_dev);
}

int __init qdio_debug_init(void)
{
	debugfs_root = debugfs_create_dir("qdio", NULL);

	qdio_dbf_setup = debug_register("qdio_setup", 16, 1, 16);
	debug_register_view(qdio_dbf_setup, &debug_hex_ascii_view);
	debug_set_level(qdio_dbf_setup, DBF_INFO);
	DBF_EVENT("dbf created\n");

	qdio_dbf_error = debug_register("qdio_error", 4, 1, 16);
	debug_register_view(qdio_dbf_error, &debug_hex_ascii_view);
	debug_set_level(qdio_dbf_error, DBF_INFO);
	DBF_ERROR("dbf created\n");
	return 0;
}

void qdio_debug_exit(void)
{
	qdio_clear_dbf_list();
	debugfs_remove_recursive(debugfs_root);
	debug_unregister(qdio_dbf_setup);
	debug_unregister(qdio_dbf_error);
}
