// SPDX-License-Identifier: GPL-2.0
/*
 *    tape device driver for S/390 and zSeries tapes.
 *
 *  S390 and zSeries version
 *    Copyright IBM Corp. 2001
 *    Author(s): Carsten Otte <cotte@de.ibm.com>
 *		 Michael Holzheu <holzheu@de.ibm.com>
 *		 Tuan Ngo-Anh <ngoanh@de.ibm.com>
 *
 * PROCFS Functions
 */

#define KMSG_COMPONENT "tape"
#define pr_fmt(fmt) KMSG_COMPONENT ": " fmt

#include <linux/module.h>
#include <linux/vmalloc.h>
#include <linux/seq_file.h>
#include <linux/proc_fs.h>

#define TAPE_DBF_AREA	tape_core_dbf

#include "tape.h"

static const char *tape_med_st_verbose[MS_SIZE] =
{
	[MS_UNKNOWN] = "UNKNOWN ",
	[MS_LOADED] = "LOADED  ",
	[MS_UNLOADED] = "UNLOADED"
};

/* our proc tapedevices entry */
static struct proc_dir_entry *tape_proc_devices;

/*
 * Show function for /proc/tapedevices
 */
static int tape_proc_show(struct seq_file *m, void *v)
{
	struct tape_device *device;
	struct tape_request *request;
	const char *str;
	unsigned long n;

	n = (unsigned long) v - 1;
	if (!n) {
		seq_printf(m, "TapeNo\tBusID      CuType/Model\t"
			"DevType/Model\tBlkSize\tState\tOp\tMedState\n");
	}
	device = tape_find_device(n);
	if (IS_ERR(device))
		return 0;
	spin_lock_irq(get_ccwdev_lock(device->cdev));
	seq_printf(m, "%d\t", (int) n);
	seq_printf(m, "%-10.10s ", dev_name(&device->cdev->dev));
	seq_printf(m, "%04X/", device->cdev->id.cu_type);
	seq_printf(m, "%02X\t", device->cdev->id.cu_model);
	seq_printf(m, "%04X/", device->cdev->id.dev_type);
	seq_printf(m, "%02X\t\t", device->cdev->id.dev_model);
	if (device->char_data.block_size == 0)
		seq_printf(m, "auto\t");
	else
		seq_printf(m, "%i\t", device->char_data.block_size);
	if (device->tape_state >= 0 &&
	    device->tape_state < TS_SIZE)
		str = tape_state_verbose[device->tape_state];
	else
		str = "UNKNOWN";
	seq_printf(m, "%s\t", str);
	if (!list_empty(&device->req_queue)) {
		request = list_entry(device->req_queue.next,
				     struct tape_request, list);
		str = tape_op_verbose[request->op];
	} else
		str = "---";
	seq_printf(m, "%s\t", str);
	seq_printf(m, "%s\n", tape_med_st_verbose[device->medium_state]);
	spin_unlock_irq(get_ccwdev_lock(device->cdev));
	tape_put_device(device);
        return 0;
}

static void *tape_proc_start(struct seq_file *m, loff_t *pos)
{
	if (*pos >= 256 / TAPE_MINORS_PER_DEV)
		return NULL;
	return (void *)((unsigned long) *pos + 1);
}

static void *tape_proc_next(struct seq_file *m, void *v, loff_t *pos)
{
	++*pos;
	return tape_proc_start(m, pos);
}

static void tape_proc_stop(struct seq_file *m, void *v)
{
}

static const struct seq_operations tape_proc_seq = {
	.start		= tape_proc_start,
	.next		= tape_proc_next,
	.stop		= tape_proc_stop,
	.show		= tape_proc_show,
};

/*
 * Initialize procfs stuff on startup
 */
void
tape_proc_init(void)
{
	tape_proc_devices = proc_create_seq("tapedevices", 0444, NULL,
					    &tape_proc_seq);
}

/*
 * Cleanup all stuff registered to the procfs
 */
void
tape_proc_cleanup(void)
{
	if (tape_proc_devices != NULL)
		remove_proc_entry ("tapedevices", NULL);
}
