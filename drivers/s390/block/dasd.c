// SPDX-License-Identifier: GPL-2.0
/*
 * Author(s)......: Holger Smolinski <Holger.Smolinski@de.ibm.com>
 *		    Horst Hummel <Horst.Hummel@de.ibm.com>
 *		    Carsten Otte <Cotte@de.ibm.com>
 *		    Martin Schwidefsky <schwidefsky@de.ibm.com>
 * Bugreports.to..: <Linux390@de.ibm.com>
 * Copyright IBM Corp. 1999, 2009
 */

#define KMSG_COMPONENT "dasd"
#define pr_fmt(fmt) KMSG_COMPONENT ": " fmt

#include <linux/kmod.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/ctype.h>
#include <linux/major.h>
#include <linux/slab.h>
#include <linux/hdreg.h>
#include <linux/async.h>
#include <linux/mutex.h>
#include <linux/debugfs.h>
#include <linux/seq_file.h>
#include <linux/vmalloc.h>

#include <asm/ccwdev.h>
#include <asm/ebcdic.h>
#include <asm/idals.h>
#include <asm/itcw.h>
#include <asm/diag.h>

/* This is ugly... */
#define PRINTK_HEADER "dasd:"

#include "dasd_int.h"
/*
 * SECTION: Constant definitions to be used within this file
 */
#define DASD_CHANQ_MAX_SIZE 4

#define DASD_DIAG_MOD		"dasd_diag_mod"

static unsigned int queue_depth = 32;
static unsigned int nr_hw_queues = 4;

module_param(queue_depth, uint, 0444);
MODULE_PARM_DESC(queue_depth, "Default queue depth for new DASD devices");

module_param(nr_hw_queues, uint, 0444);
MODULE_PARM_DESC(nr_hw_queues, "Default number of hardware queues for new DASD devices");

/*
 * SECTION: exported variables of dasd.c
 */
debug_info_t *dasd_debug_area;
EXPORT_SYMBOL(dasd_debug_area);
static struct dentry *dasd_debugfs_root_entry;
struct dasd_discipline *dasd_diag_discipline_pointer;
EXPORT_SYMBOL(dasd_diag_discipline_pointer);
void dasd_int_handler(struct ccw_device *, unsigned long, struct irb *);

MODULE_AUTHOR("Holger Smolinski <Holger.Smolinski@de.ibm.com>");
MODULE_DESCRIPTION("Linux on S/390 DASD device driver,"
		   " Copyright IBM Corp. 2000");
MODULE_LICENSE("GPL");

/*
 * SECTION: prototypes for static functions of dasd.c
 */
static int  dasd_alloc_queue(struct dasd_block *);
static void dasd_free_queue(struct dasd_block *);
static int dasd_flush_block_queue(struct dasd_block *);
static void dasd_device_tasklet(unsigned long);
static void dasd_block_tasklet(unsigned long);
static void do_kick_device(struct work_struct *);
static void do_reload_device(struct work_struct *);
static void do_requeue_requests(struct work_struct *);
static void dasd_return_cqr_cb(struct dasd_ccw_req *, void *);
static void dasd_device_timeout(struct timer_list *);
static void dasd_block_timeout(struct timer_list *);
static void __dasd_process_erp(struct dasd_device *, struct dasd_ccw_req *);
static void dasd_profile_init(struct dasd_profile *, struct dentry *);
static void dasd_profile_exit(struct dasd_profile *);
static void dasd_hosts_init(struct dentry *, struct dasd_device *);
static void dasd_hosts_exit(struct dasd_device *);

/*
 * SECTION: Operations on the device structure.
 */
static wait_queue_head_t dasd_init_waitq;
static wait_queue_head_t dasd_flush_wq;
static wait_queue_head_t generic_waitq;
static wait_queue_head_t shutdown_waitq;

/*
 * Allocate memory for a new device structure.
 */
struct dasd_device *dasd_alloc_device(void)
{
	struct dasd_device *device;

	device = kzalloc(sizeof(struct dasd_device), GFP_ATOMIC);
	if (!device)
		return ERR_PTR(-ENOMEM);

	/* Get two pages for normal block device operations. */
	device->ccw_mem = (void *) __get_free_pages(GFP_ATOMIC | GFP_DMA, 1);
	if (!device->ccw_mem) {
		kfree(device);
		return ERR_PTR(-ENOMEM);
	}
	/* Get one page for error recovery. */
	device->erp_mem = (void *) get_zeroed_page(GFP_ATOMIC | GFP_DMA);
	if (!device->erp_mem) {
		free_pages((unsigned long) device->ccw_mem, 1);
		kfree(device);
		return ERR_PTR(-ENOMEM);
	}
	/* Get two pages for ese format. */
	device->ese_mem = (void *)__get_free_pages(GFP_ATOMIC | GFP_DMA, 1);
	if (!device->ese_mem) {
		free_page((unsigned long) device->erp_mem);
		free_pages((unsigned long) device->ccw_mem, 1);
		kfree(device);
		return ERR_PTR(-ENOMEM);
	}

	dasd_init_chunklist(&device->ccw_chunks, device->ccw_mem, PAGE_SIZE*2);
	dasd_init_chunklist(&device->erp_chunks, device->erp_mem, PAGE_SIZE);
	dasd_init_chunklist(&device->ese_chunks, device->ese_mem, PAGE_SIZE * 2);
	spin_lock_init(&device->mem_lock);
	atomic_set(&device->tasklet_scheduled, 0);
	tasklet_init(&device->tasklet, dasd_device_tasklet,
		     (unsigned long) device);
	INIT_LIST_HEAD(&device->ccw_queue);
	timer_setup(&device->timer, dasd_device_timeout, 0);
	INIT_WORK(&device->kick_work, do_kick_device);
	INIT_WORK(&device->reload_device, do_reload_device);
	INIT_WORK(&device->requeue_requests, do_requeue_requests);
	device->state = DASD_STATE_NEW;
	device->target = DASD_STATE_NEW;
	mutex_init(&device->state_mutex);
	spin_lock_init(&device->profile.lock);
	return device;
}

/*
 * Free memory of a device structure.
 */
void dasd_free_device(struct dasd_device *device)
{
	kfree(device->private);
	free_pages((unsigned long) device->ese_mem, 1);
	free_page((unsigned long) device->erp_mem);
	free_pages((unsigned long) device->ccw_mem, 1);
	kfree(device);
}

/*
 * Allocate memory for a new device structure.
 */
struct dasd_block *dasd_alloc_block(void)
{
	struct dasd_block *block;

	block = kzalloc(sizeof(*block), GFP_ATOMIC);
	if (!block)
		return ERR_PTR(-ENOMEM);
	/* open_count = 0 means device online but not in use */
	atomic_set(&block->open_count, -1);

	atomic_set(&block->tasklet_scheduled, 0);
	tasklet_init(&block->tasklet, dasd_block_tasklet,
		     (unsigned long) block);
	INIT_LIST_HEAD(&block->ccw_queue);
	spin_lock_init(&block->queue_lock);
	INIT_LIST_HEAD(&block->format_list);
	spin_lock_init(&block->format_lock);
	timer_setup(&block->timer, dasd_block_timeout, 0);
	spin_lock_init(&block->profile.lock);

	return block;
}
EXPORT_SYMBOL_GPL(dasd_alloc_block);

/*
 * Free memory of a device structure.
 */
void dasd_free_block(struct dasd_block *block)
{
	kfree(block);
}
EXPORT_SYMBOL_GPL(dasd_free_block);

/*
 * Make a new device known to the system.
 */
static int dasd_state_new_to_known(struct dasd_device *device)
{
	int rc;

	/*
	 * As long as the device is not in state DASD_STATE_NEW we want to
	 * keep the reference count > 0.
	 */
	dasd_get_device(device);

	if (device->block) {
		rc = dasd_alloc_queue(device->block);
		if (rc) {
			dasd_put_device(device);
			return rc;
		}
	}
	device->state = DASD_STATE_KNOWN;
	return 0;
}

/*
 * Let the system forget about a device.
 */
static int dasd_state_known_to_new(struct dasd_device *device)
{
	/* Disable extended error reporting for this device. */
	dasd_eer_disable(device);
	device->state = DASD_STATE_NEW;

	if (device->block)
		dasd_free_queue(device->block);

	/* Give up reference we took in dasd_state_new_to_known. */
	dasd_put_device(device);
	return 0;
}

static struct dentry *dasd_debugfs_setup(const char *name,
					 struct dentry *base_dentry)
{
	struct dentry *pde;

	if (!base_dentry)
		return NULL;
	pde = debugfs_create_dir(name, base_dentry);
	if (!pde || IS_ERR(pde))
		return NULL;
	return pde;
}

/*
 * Request the irq line for the device.
 */
static int dasd_state_known_to_basic(struct dasd_device *device)
{
	struct dasd_block *block = device->block;
	int rc = 0;

	/* Allocate and register gendisk structure. */
	if (block) {
		rc = dasd_gendisk_alloc(block);
		if (rc)
			return rc;
		block->debugfs_dentry =
			dasd_debugfs_setup(block->gdp->disk_name,
					   dasd_debugfs_root_entry);
		dasd_profile_init(&block->profile, block->debugfs_dentry);
		if (dasd_global_profile_level == DASD_PROFILE_ON)
			dasd_profile_on(&device->block->profile);
	}
	device->debugfs_dentry =
		dasd_debugfs_setup(dev_name(&device->cdev->dev),
				   dasd_debugfs_root_entry);
	dasd_profile_init(&device->profile, device->debugfs_dentry);
	dasd_hosts_init(device->debugfs_dentry, device);

	/* register 'device' debug area, used for all DBF_DEV_XXX calls */
	device->debug_area = debug_register(dev_name(&device->cdev->dev), 4, 1,
					    8 * sizeof(long));
	debug_register_view(device->debug_area, &debug_sprintf_view);
	debug_set_level(device->debug_area, DBF_WARNING);
	DBF_DEV_EVENT(DBF_EMERG, device, "%s", "debug area created");

	device->state = DASD_STATE_BASIC;

	return rc;
}

/*
 * Release the irq line for the device. Terminate any running i/o.
 */
static int dasd_state_basic_to_known(struct dasd_device *device)
{
	int rc;

	if (device->discipline->basic_to_known) {
		rc = device->discipline->basic_to_known(device);
		if (rc)
			return rc;
	}

	if (device->block) {
		dasd_profile_exit(&device->block->profile);
		debugfs_remove(device->block->debugfs_dentry);
		dasd_gendisk_free(device->block);
		dasd_block_clear_timer(device->block);
	}
	rc = dasd_flush_device_queue(device);
	if (rc)
		return rc;
	dasd_device_clear_timer(device);
	dasd_profile_exit(&device->profile);
	dasd_hosts_exit(device);
	debugfs_remove(device->debugfs_dentry);
	DBF_DEV_EVENT(DBF_EMERG, device, "%p debug area deleted", device);
	if (device->debug_area != NULL) {
		debug_unregister(device->debug_area);
		device->debug_area = NULL;
	}
	device->state = DASD_STATE_KNOWN;
	return 0;
}

/*
 * Do the initial analysis. The do_analysis function may return
 * -EAGAIN in which case the device keeps the state DASD_STATE_BASIC
 * until the discipline decides to continue the startup sequence
 * by calling the function dasd_change_state. The eckd disciplines
 * uses this to start a ccw that detects the format. The completion
 * interrupt for this detection ccw uses the kernel event daemon to
 * trigger the call to dasd_change_state. All this is done in the
 * discipline code, see dasd_eckd.c.
 * After the analysis ccw is done (do_analysis returned 0) the block
 * device is setup.
 * In case the analysis returns an error, the device setup is stopped
 * (a fake disk was already added to allow formatting).
 */
static int dasd_state_basic_to_ready(struct dasd_device *device)
{
	int rc;
	struct dasd_block *block;
	struct gendisk *disk;

	rc = 0;
	block = device->block;
	/* make disk known with correct capacity */
	if (block) {
		if (block->base->discipline->do_analysis != NULL)
			rc = block->base->discipline->do_analysis(block);
		if (rc) {
			if (rc != -EAGAIN) {
				device->state = DASD_STATE_UNFMT;
				disk = device->block->gdp;
				kobject_uevent(&disk_to_dev(disk)->kobj,
					       KOBJ_CHANGE);
				goto out;
			}
			return rc;
		}
		if (device->discipline->setup_blk_queue)
			device->discipline->setup_blk_queue(block);
		set_capacity(block->gdp,
			     block->blocks << block->s2b_shift);
		device->state = DASD_STATE_READY;
		rc = dasd_scan_partitions(block);
		if (rc) {
			device->state = DASD_STATE_BASIC;
			return rc;
		}
	} else {
		device->state = DASD_STATE_READY;
	}
out:
	if (device->discipline->basic_to_ready)
		rc = device->discipline->basic_to_ready(device);
	return rc;
}

static inline
int _wait_for_empty_queues(struct dasd_device *device)
{
	if (device->block)
		return list_empty(&device->ccw_queue) &&
			list_empty(&device->block->ccw_queue);
	else
		return list_empty(&device->ccw_queue);
}

/*
 * Remove device from block device layer. Destroy dirty buffers.
 * Forget format information. Check if the target level is basic
 * and if it is create fake disk for formatting.
 */
static int dasd_state_ready_to_basic(struct dasd_device *device)
{
	int rc;

	device->state = DASD_STATE_BASIC;
	if (device->block) {
		struct dasd_block *block = device->block;
		rc = dasd_flush_block_queue(block);
		if (rc) {
			device->state = DASD_STATE_READY;
			return rc;
		}
		dasd_destroy_partitions(block);
		block->blocks = 0;
		block->bp_block = 0;
		block->s2b_shift = 0;
	}
	return 0;
}

/*
 * Back to basic.
 */
static int dasd_state_unfmt_to_basic(struct dasd_device *device)
{
	device->state = DASD_STATE_BASIC;
	return 0;
}

/*
 * Make the device online and schedule the bottom half to start
 * the requeueing of requests from the linux request queue to the
 * ccw queue.
 */
static int
dasd_state_ready_to_online(struct dasd_device * device)
{
	device->state = DASD_STATE_ONLINE;
	if (device->block) {
		dasd_schedule_block_bh(device->block);
		if ((device->features & DASD_FEATURE_USERAW)) {
			kobject_uevent(&disk_to_dev(device->block->gdp)->kobj,
					KOBJ_CHANGE);
			return 0;
		}
		disk_uevent(device->block->bdev->bd_disk, KOBJ_CHANGE);
	}
	return 0;
}

/*
 * Stop the requeueing of requests again.
 */
static int dasd_state_online_to_ready(struct dasd_device *device)
{
	int rc;

	if (device->discipline->online_to_ready) {
		rc = device->discipline->online_to_ready(device);
		if (rc)
			return rc;
	}

	device->state = DASD_STATE_READY;
	if (device->block && !(device->features & DASD_FEATURE_USERAW))
		disk_uevent(device->block->bdev->bd_disk, KOBJ_CHANGE);
	return 0;
}

/*
 * Device startup state changes.
 */
static int dasd_increase_state(struct dasd_device *device)
{
	int rc;

	rc = 0;
	if (device->state == DASD_STATE_NEW &&
	    device->target >= DASD_STATE_KNOWN)
		rc = dasd_state_new_to_known(device);

	if (!rc &&
	    device->state == DASD_STATE_KNOWN &&
	    device->target >= DASD_STATE_BASIC)
		rc = dasd_state_known_to_basic(device);

	if (!rc &&
	    device->state == DASD_STATE_BASIC &&
	    device->target >= DASD_STATE_READY)
		rc = dasd_state_basic_to_ready(device);

	if (!rc &&
	    device->state == DASD_STATE_UNFMT &&
	    device->target > DASD_STATE_UNFMT)
		rc = -EPERM;

	if (!rc &&
	    device->state == DASD_STATE_READY &&
	    device->target >= DASD_STATE_ONLINE)
		rc = dasd_state_ready_to_online(device);

	return rc;
}

/*
 * Device shutdown state changes.
 */
static int dasd_decrease_state(struct dasd_device *device)
{
	int rc;

	rc = 0;
	if (device->state == DASD_STATE_ONLINE &&
	    device->target <= DASD_STATE_READY)
		rc = dasd_state_online_to_ready(device);

	if (!rc &&
	    device->state == DASD_STATE_READY &&
	    device->target <= DASD_STATE_BASIC)
		rc = dasd_state_ready_to_basic(device);

	if (!rc &&
	    device->state == DASD_STATE_UNFMT &&
	    device->target <= DASD_STATE_BASIC)
		rc = dasd_state_unfmt_to_basic(device);

	if (!rc &&
	    device->state == DASD_STATE_BASIC &&
	    device->target <= DASD_STATE_KNOWN)
		rc = dasd_state_basic_to_known(device);

	if (!rc &&
	    device->state == DASD_STATE_KNOWN &&
	    device->target <= DASD_STATE_NEW)
		rc = dasd_state_known_to_new(device);

	return rc;
}

/*
 * This is the main startup/shutdown routine.
 */
static void dasd_change_state(struct dasd_device *device)
{
	int rc;

	if (device->state == device->target)
		/* Already where we want to go today... */
		return;
	if (device->state < device->target)
		rc = dasd_increase_state(device);
	else
		rc = dasd_decrease_state(device);
	if (rc == -EAGAIN)
		return;
	if (rc)
		device->target = device->state;

	/* let user-space know that the device status changed */
	kobject_uevent(&device->cdev->dev.kobj, KOBJ_CHANGE);

	if (device->state == device->target)
		wake_up(&dasd_init_waitq);
}

/*
 * Kick starter for devices that did not complete the startup/shutdown
 * procedure or were sleeping because of a pending state.
 * dasd_kick_device will schedule a call do do_kick_device to the kernel
 * event daemon.
 */
static void do_kick_device(struct work_struct *work)
{
	struct dasd_device *device = container_of(work, struct dasd_device, kick_work);
	mutex_lock(&device->state_mutex);
	dasd_change_state(device);
	mutex_unlock(&device->state_mutex);
	dasd_schedule_device_bh(device);
	dasd_put_device(device);
}

void dasd_kick_device(struct dasd_device *device)
{
	dasd_get_device(device);
	/* queue call to dasd_kick_device to the kernel event daemon. */
	if (!schedule_work(&device->kick_work))
		dasd_put_device(device);
}
EXPORT_SYMBOL(dasd_kick_device);

/*
 * dasd_reload_device will schedule a call do do_reload_device to the kernel
 * event daemon.
 */
static void do_reload_device(struct work_struct *work)
{
	struct dasd_device *device = container_of(work, struct dasd_device,
						  reload_device);
	device->discipline->reload(device);
	dasd_put_device(device);
}

void dasd_reload_device(struct dasd_device *device)
{
	dasd_get_device(device);
	/* queue call to dasd_reload_device to the kernel event daemon. */
	if (!schedule_work(&device->reload_device))
		dasd_put_device(device);
}
EXPORT_SYMBOL(dasd_reload_device);

/*
 * Set the target state for a device and starts the state change.
 */
void dasd_set_target_state(struct dasd_device *device, int target)
{
	dasd_get_device(device);
	mutex_lock(&device->state_mutex);
	/* If we are in probeonly mode stop at DASD_STATE_READY. */
	if (dasd_probeonly && target > DASD_STATE_READY)
		target = DASD_STATE_READY;
	if (device->target != target) {
		if (device->state == target)
			wake_up(&dasd_init_waitq);
		device->target = target;
	}
	if (device->state != device->target)
		dasd_change_state(device);
	mutex_unlock(&device->state_mutex);
	dasd_put_device(device);
}
EXPORT_SYMBOL(dasd_set_target_state);

/*
 * Enable devices with device numbers in [from..to].
 */
static inline int _wait_for_device(struct dasd_device *device)
{
	return (device->state == device->target);
}

void dasd_enable_device(struct dasd_device *device)
{
	dasd_set_target_state(device, DASD_STATE_ONLINE);
	if (device->state <= DASD_STATE_KNOWN)
		/* No discipline for device found. */
		dasd_set_target_state(device, DASD_STATE_NEW);
	/* Now wait for the devices to come up. */
	wait_event(dasd_init_waitq, _wait_for_device(device));

	dasd_reload_device(device);
	if (device->discipline->kick_validate)
		device->discipline->kick_validate(device);
}
EXPORT_SYMBOL(dasd_enable_device);

/*
 * SECTION: device operation (interrupt handler, start i/o, term i/o ...)
 */

unsigned int dasd_global_profile_level = DASD_PROFILE_OFF;

#ifdef CONFIG_DASD_PROFILE
struct dasd_profile dasd_global_profile = {
	.lock = __SPIN_LOCK_UNLOCKED(dasd_global_profile.lock),
};
static struct dentry *dasd_debugfs_global_entry;

/*
 * Add profiling information for cqr before execution.
 */
static void dasd_profile_start(struct dasd_block *block,
			       struct dasd_ccw_req *cqr,
			       struct request *req)
{
	struct list_head *l;
	unsigned int counter;
	struct dasd_device *device;

	/* count the length of the chanq for statistics */
	counter = 0;
	if (dasd_global_profile_level || block->profile.data)
		list_for_each(l, &block->ccw_queue)
			if (++counter >= 31)
				break;

	spin_lock(&dasd_global_profile.lock);
	if (dasd_global_profile.data) {
		dasd_global_profile.data->dasd_io_nr_req[counter]++;
		if (rq_data_dir(req) == READ)
			dasd_global_profile.data->dasd_read_nr_req[counter]++;
	}
	spin_unlock(&dasd_global_profile.lock);

	spin_lock(&block->profile.lock);
	if (block->profile.data) {
		block->profile.data->dasd_io_nr_req[counter]++;
		if (rq_data_dir(req) == READ)
			block->profile.data->dasd_read_nr_req[counter]++;
	}
	spin_unlock(&block->profile.lock);

	/*
	 * We count the request for the start device, even though it may run on
	 * some other device due to error recovery. This way we make sure that
	 * we count each request only once.
	 */
	device = cqr->startdev;
	if (device->profile.data) {
		counter = 1; /* request is not yet queued on the start device */
		list_for_each(l, &device->ccw_queue)
			if (++counter >= 31)
				break;
	}
	spin_lock(&device->profile.lock);
	if (device->profile.data) {
		device->profile.data->dasd_io_nr_req[counter]++;
		if (rq_data_dir(req) == READ)
			device->profile.data->dasd_read_nr_req[counter]++;
	}
	spin_unlock(&device->profile.lock);
}

/*
 * Add profiling information for cqr after execution.
 */

#define dasd_profile_counter(value, index)			   \
{								   \
	for (index = 0; index < 31 && value >> (2+index); index++) \
		;						   \
}

static void dasd_profile_end_add_data(struct dasd_profile_info *data,
				      int is_alias,
				      int is_tpm,
				      int is_read,
				      long sectors,
				      int sectors_ind,
				      int tottime_ind,
				      int tottimeps_ind,
				      int strtime_ind,
				      int irqtime_ind,
				      int irqtimeps_ind,
				      int endtime_ind)
{
	/* in case of an overflow, reset the whole profile */
	if (data->dasd_io_reqs == UINT_MAX) {
			memset(data, 0, sizeof(*data));
			ktime_get_real_ts64(&data->starttod);
	}
	data->dasd_io_reqs++;
	data->dasd_io_sects += sectors;
	if (is_alias)
		data->dasd_io_alias++;
	if (is_tpm)
		data->dasd_io_tpm++;

	data->dasd_io_secs[sectors_ind]++;
	data->dasd_io_times[tottime_ind]++;
	data->dasd_io_timps[tottimeps_ind]++;
	data->dasd_io_time1[strtime_ind]++;
	data->dasd_io_time2[irqtime_ind]++;
	data->dasd_io_time2ps[irqtimeps_ind]++;
	data->dasd_io_time3[endtime_ind]++;

	if (is_read) {
		data->dasd_read_reqs++;
		data->dasd_read_sects += sectors;
		if (is_alias)
			data->dasd_read_alias++;
		if (is_tpm)
			data->dasd_read_tpm++;
		data->dasd_read_secs[sectors_ind]++;
		data->dasd_read_times[tottime_ind]++;
		data->dasd_read_time1[strtime_ind]++;
		data->dasd_read_time2[irqtime_ind]++;
		data->dasd_read_time3[endtime_ind]++;
	}
}

static void dasd_profile_end(struct dasd_block *block,
			     struct dasd_ccw_req *cqr,
			     struct request *req)
{
	unsigned long strtime, irqtime, endtime, tottime;
	unsigned long tottimeps, sectors;
	struct dasd_device *device;
	int sectors_ind, tottime_ind, tottimeps_ind, strtime_ind;
	int irqtime_ind, irqtimeps_ind, endtime_ind;
	struct dasd_profile_info *data;

	device = cqr->startdev;
	if (!(dasd_global_profile_level ||
	      block->profile.data ||
	      device->profile.data))
		return;

	sectors = blk_rq_sectors(req);
	if (!cqr->buildclk || !cqr->startclk ||
	    !cqr->stopclk || !cqr->endclk ||
	    !sectors)
		return;

	strtime = ((cqr->startclk - cqr->buildclk) >> 12);
	irqtime = ((cqr->stopclk - cqr->startclk) >> 12);
	endtime = ((cqr->endclk - cqr->stopclk) >> 12);
	tottime = ((cqr->endclk - cqr->buildclk) >> 12);
	tottimeps = tottime / sectors;

	dasd_profile_counter(sectors, sectors_ind);
	dasd_profile_counter(tottime, tottime_ind);
	dasd_profile_counter(tottimeps, tottimeps_ind);
	dasd_profile_counter(strtime, strtime_ind);
	dasd_profile_counter(irqtime, irqtime_ind);
	dasd_profile_counter(irqtime / sectors, irqtimeps_ind);
	dasd_profile_counter(endtime, endtime_ind);

	spin_lock(&dasd_global_profile.lock);
	if (dasd_global_profile.data) {
		data = dasd_global_profile.data;
		data->dasd_sum_times += tottime;
		data->dasd_sum_time_str += strtime;
		data->dasd_sum_time_irq += irqtime;
		data->dasd_sum_time_end += endtime;
		dasd_profile_end_add_data(dasd_global_profile.data,
					  cqr->startdev != block->base,
					  cqr->cpmode == 1,
					  rq_data_dir(req) == READ,
					  sectors, sectors_ind, tottime_ind,
					  tottimeps_ind, strtime_ind,
					  irqtime_ind, irqtimeps_ind,
					  endtime_ind);
	}
	spin_unlock(&dasd_global_profile.lock);

	spin_lock(&block->profile.lock);
	if (block->profile.data) {
		data = block->profile.data;
		data->dasd_sum_times += tottime;
		data->dasd_sum_time_str += strtime;
		data->dasd_sum_time_irq += irqtime;
		data->dasd_sum_time_end += endtime;
		dasd_profile_end_add_data(block->profile.data,
					  cqr->startdev != block->base,
					  cqr->cpmode == 1,
					  rq_data_dir(req) == READ,
					  sectors, sectors_ind, tottime_ind,
					  tottimeps_ind, strtime_ind,
					  irqtime_ind, irqtimeps_ind,
					  endtime_ind);
	}
	spin_unlock(&block->profile.lock);

	spin_lock(&device->profile.lock);
	if (device->profile.data) {
		data = device->profile.data;
		data->dasd_sum_times += tottime;
		data->dasd_sum_time_str += strtime;
		data->dasd_sum_time_irq += irqtime;
		data->dasd_sum_time_end += endtime;
		dasd_profile_end_add_data(device->profile.data,
					  cqr->startdev != block->base,
					  cqr->cpmode == 1,
					  rq_data_dir(req) == READ,
					  sectors, sectors_ind, tottime_ind,
					  tottimeps_ind, strtime_ind,
					  irqtime_ind, irqtimeps_ind,
					  endtime_ind);
	}
	spin_unlock(&device->profile.lock);
}

void dasd_profile_reset(struct dasd_profile *profile)
{
	struct dasd_profile_info *data;

	spin_lock_bh(&profile->lock);
	data = profile->data;
	if (!data) {
		spin_unlock_bh(&profile->lock);
		return;
	}
	memset(data, 0, sizeof(*data));
	ktime_get_real_ts64(&data->starttod);
	spin_unlock_bh(&profile->lock);
}

int dasd_profile_on(struct dasd_profile *profile)
{
	struct dasd_profile_info *data;

	data = kzalloc(sizeof(*data), GFP_KERNEL);
	if (!data)
		return -ENOMEM;
	spin_lock_bh(&profile->lock);
	if (profile->data) {
		spin_unlock_bh(&profile->lock);
		kfree(data);
		return 0;
	}
	ktime_get_real_ts64(&data->starttod);
	profile->data = data;
	spin_unlock_bh(&profile->lock);
	return 0;
}

void dasd_profile_off(struct dasd_profile *profile)
{
	spin_lock_bh(&profile->lock);
	kfree(profile->data);
	profile->data = NULL;
	spin_unlock_bh(&profile->lock);
}

char *dasd_get_user_string(const char __user *user_buf, size_t user_len)
{
	char *buffer;

	buffer = vmalloc(user_len + 1);
	if (buffer == NULL)
		return ERR_PTR(-ENOMEM);
	if (copy_from_user(buffer, user_buf, user_len) != 0) {
		vfree(buffer);
		return ERR_PTR(-EFAULT);
	}
	/* got the string, now strip linefeed. */
	if (buffer[user_len - 1] == '\n')
		buffer[user_len - 1] = 0;
	else
		buffer[user_len] = 0;
	return buffer;
}

static ssize_t dasd_stats_write(struct file *file,
				const char __user *user_buf,
				size_t user_len, loff_t *pos)
{
	char *buffer, *str;
	int rc;
	struct seq_file *m = (struct seq_file *)file->private_data;
	struct dasd_profile *prof = m->private;

	if (user_len > 65536)
		user_len = 65536;
	buffer = dasd_get_user_string(user_buf, user_len);
	if (IS_ERR(buffer))
		return PTR_ERR(buffer);

	str = skip_spaces(buffer);
	rc = user_len;
	if (strncmp(str, "reset", 5) == 0) {
		dasd_profile_reset(prof);
	} else if (strncmp(str, "on", 2) == 0) {
		rc = dasd_profile_on(prof);
		if (rc)
			goto out;
		rc = user_len;
		if (prof == &dasd_global_profile) {
			dasd_profile_reset(prof);
			dasd_global_profile_level = DASD_PROFILE_GLOBAL_ONLY;
		}
	} else if (strncmp(str, "off", 3) == 0) {
		if (prof == &dasd_global_profile)
			dasd_global_profile_level = DASD_PROFILE_OFF;
		dasd_profile_off(prof);
	} else
		rc = -EINVAL;
out:
	vfree(buffer);
	return rc;
}

static void dasd_stats_array(struct seq_file *m, unsigned int *array)
{
	int i;

	for (i = 0; i < 32; i++)
		seq_printf(m, "%u ", array[i]);
	seq_putc(m, '\n');
}

static void dasd_stats_seq_print(struct seq_file *m,
				 struct dasd_profile_info *data)
{
	seq_printf(m, "start_time %lld.%09ld\n",
		   (s64)data->starttod.tv_sec, data->starttod.tv_nsec);
	seq_printf(m, "total_requests %u\n", data->dasd_io_reqs);
	seq_printf(m, "total_sectors %u\n", data->dasd_io_sects);
	seq_printf(m, "total_pav %u\n", data->dasd_io_alias);
	seq_printf(m, "total_hpf %u\n", data->dasd_io_tpm);
	seq_printf(m, "avg_total %lu\n", data->dasd_io_reqs ?
		   data->dasd_sum_times / data->dasd_io_reqs : 0UL);
	seq_printf(m, "avg_build_to_ssch %lu\n", data->dasd_io_reqs ?
		   data->dasd_sum_time_str / data->dasd_io_reqs : 0UL);
	seq_printf(m, "avg_ssch_to_irq %lu\n", data->dasd_io_reqs ?
		   data->dasd_sum_time_irq / data->dasd_io_reqs : 0UL);
	seq_printf(m, "avg_irq_to_end %lu\n", data->dasd_io_reqs ?
		   data->dasd_sum_time_end / data->dasd_io_reqs : 0UL);
	seq_puts(m, "histogram_sectors ");
	dasd_stats_array(m, data->dasd_io_secs);
	seq_puts(m, "histogram_io_times ");
	dasd_stats_array(m, data->dasd_io_times);
	seq_puts(m, "histogram_io_times_weighted ");
	dasd_stats_array(m, data->dasd_io_timps);
	seq_puts(m, "histogram_time_build_to_ssch ");
	dasd_stats_array(m, data->dasd_io_time1);
	seq_puts(m, "histogram_time_ssch_to_irq ");
	dasd_stats_array(m, data->dasd_io_time2);
	seq_puts(m, "histogram_time_ssch_to_irq_weighted ");
	dasd_stats_array(m, data->dasd_io_time2ps);
	seq_puts(m, "histogram_time_irq_to_end ");
	dasd_stats_array(m, data->dasd_io_time3);
	seq_puts(m, "histogram_ccw_queue_length ");
	dasd_stats_array(m, data->dasd_io_nr_req);
	seq_printf(m, "total_read_requests %u\n", data->dasd_read_reqs);
	seq_printf(m, "total_read_sectors %u\n", data->dasd_read_sects);
	seq_printf(m, "total_read_pav %u\n", data->dasd_read_alias);
	seq_printf(m, "total_read_hpf %u\n", data->dasd_read_tpm);
	seq_puts(m, "histogram_read_sectors ");
	dasd_stats_array(m, data->dasd_read_secs);
	seq_puts(m, "histogram_read_times ");
	dasd_stats_array(m, data->dasd_read_times);
	seq_puts(m, "histogram_read_time_build_to_ssch ");
	dasd_stats_array(m, data->dasd_read_time1);
	seq_puts(m, "histogram_read_time_ssch_to_irq ");
	dasd_stats_array(m, data->dasd_read_time2);
	seq_puts(m, "histogram_read_time_irq_to_end ");
	dasd_stats_array(m, data->dasd_read_time3);
	seq_puts(m, "histogram_read_ccw_queue_length ");
	dasd_stats_array(m, data->dasd_read_nr_req);
}

static int dasd_stats_show(struct seq_file *m, void *v)
{
	struct dasd_profile *profile;
	struct dasd_profile_info *data;

	profile = m->private;
	spin_lock_bh(&profile->lock);
	data = profile->data;
	if (!data) {
		spin_unlock_bh(&profile->lock);
		seq_puts(m, "disabled\n");
		return 0;
	}
	dasd_stats_seq_print(m, data);
	spin_unlock_bh(&profile->lock);
	return 0;
}

static int dasd_stats_open(struct inode *inode, struct file *file)
{
	struct dasd_profile *profile = inode->i_private;
	return single_open(file, dasd_stats_show, profile);
}

static const struct file_operations dasd_stats_raw_fops = {
	.owner		= THIS_MODULE,
	.open		= dasd_stats_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
	.write		= dasd_stats_write,
};

static void dasd_profile_init(struct dasd_profile *profile,
			      struct dentry *base_dentry)
{
	umode_t mode;
	struct dentry *pde;

	if (!base_dentry)
		return;
	profile->dentry = NULL;
	profile->data = NULL;
	mode = (S_IRUSR | S_IWUSR | S_IFREG);
	pde = debugfs_create_file("statistics", mode, base_dentry,
				  profile, &dasd_stats_raw_fops);
	if (pde && !IS_ERR(pde))
		profile->dentry = pde;
	return;
}

static void dasd_profile_exit(struct dasd_profile *profile)
{
	dasd_profile_off(profile);
	debugfs_remove(profile->dentry);
	profile->dentry = NULL;
}

static void dasd_statistics_removeroot(void)
{
	dasd_global_profile_level = DASD_PROFILE_OFF;
	dasd_profile_exit(&dasd_global_profile);
	debugfs_remove(dasd_debugfs_global_entry);
	debugfs_remove(dasd_debugfs_root_entry);
}

static void dasd_statistics_createroot(void)
{
	struct dentry *pde;

	dasd_debugfs_root_entry = NULL;
	pde = debugfs_create_dir("dasd", NULL);
	if (!pde || IS_ERR(pde))
		goto error;
	dasd_debugfs_root_entry = pde;
	pde = debugfs_create_dir("global", dasd_debugfs_root_entry);
	if (!pde || IS_ERR(pde))
		goto error;
	dasd_debugfs_global_entry = pde;
	dasd_profile_init(&dasd_global_profile, dasd_debugfs_global_entry);
	return;

error:
	DBF_EVENT(DBF_ERR, "%s",
		  "Creation of the dasd debugfs interface failed");
	dasd_statistics_removeroot();
	return;
}

#else
#define dasd_profile_start(block, cqr, req) do {} while (0)
#define dasd_profile_end(block, cqr, req) do {} while (0)

static void dasd_statistics_createroot(void)
{
	return;
}

static void dasd_statistics_removeroot(void)
{
	return;
}

int dasd_stats_generic_show(struct seq_file *m, void *v)
{
	seq_puts(m, "Statistics are not activated in this kernel\n");
	return 0;
}

static void dasd_profile_init(struct dasd_profile *profile,
			      struct dentry *base_dentry)
{
	return;
}

static void dasd_profile_exit(struct dasd_profile *profile)
{
	return;
}

int dasd_profile_on(struct dasd_profile *profile)
{
	return 0;
}

#endif				/* CONFIG_DASD_PROFILE */

static int dasd_hosts_show(struct seq_file *m, void *v)
{
	struct dasd_device *device;
	int rc = -EOPNOTSUPP;

	device = m->private;
	dasd_get_device(device);

	if (device->discipline->hosts_print)
		rc = device->discipline->hosts_print(device, m);

	dasd_put_device(device);
	return rc;
}

DEFINE_SHOW_ATTRIBUTE(dasd_hosts);

static void dasd_hosts_exit(struct dasd_device *device)
{
	debugfs_remove(device->hosts_dentry);
	device->hosts_dentry = NULL;
}

static void dasd_hosts_init(struct dentry *base_dentry,
			    struct dasd_device *device)
{
	struct dentry *pde;
	umode_t mode;

	if (!base_dentry)
		return;

	mode = S_IRUSR | S_IFREG;
	pde = debugfs_create_file("host_access_list", mode, base_dentry,
				  device, &dasd_hosts_fops);
	if (pde && !IS_ERR(pde))
		device->hosts_dentry = pde;
}

struct dasd_ccw_req *dasd_smalloc_request(int magic, int cplength, int datasize,
					  struct dasd_device *device,
					  struct dasd_ccw_req *cqr)
{
	unsigned long flags;
	char *data, *chunk;
	int size = 0;

	if (cplength > 0)
		size += cplength * sizeof(struct ccw1);
	if (datasize > 0)
		size += datasize;
	if (!cqr)
		size += (sizeof(*cqr) + 7L) & -8L;

	spin_lock_irqsave(&device->mem_lock, flags);
	data = chunk = dasd_alloc_chunk(&device->ccw_chunks, size);
	spin_unlock_irqrestore(&device->mem_lock, flags);
	if (!chunk)
		return ERR_PTR(-ENOMEM);
	if (!cqr) {
		cqr = (void *) data;
		data += (sizeof(*cqr) + 7L) & -8L;
	}
	memset(cqr, 0, sizeof(*cqr));
	cqr->mem_chunk = chunk;
	if (cplength > 0) {
		cqr->cpaddr = data;
		data += cplength * sizeof(struct ccw1);
		memset(cqr->cpaddr, 0, cplength * sizeof(struct ccw1));
	}
	if (datasize > 0) {
		cqr->data = data;
 		memset(cqr->data, 0, datasize);
	}
	cqr->magic = magic;
	set_bit(DASD_CQR_FLAGS_USE_ERP, &cqr->flags);
	dasd_get_device(device);
	return cqr;
}
EXPORT_SYMBOL(dasd_smalloc_request);

struct dasd_ccw_req *dasd_fmalloc_request(int magic, int cplength,
					  int datasize,
					  struct dasd_device *device)
{
	struct dasd_ccw_req *cqr;
	unsigned long flags;
	int size, cqr_size;
	char *data;

	cqr_size = (sizeof(*cqr) + 7L) & -8L;
	size = cqr_size;
	if (cplength > 0)
		size += cplength * sizeof(struct ccw1);
	if (datasize > 0)
		size += datasize;

	spin_lock_irqsave(&device->mem_lock, flags);
	cqr = dasd_alloc_chunk(&device->ese_chunks, size);
	spin_unlock_irqrestore(&device->mem_lock, flags);
	if (!cqr)
		return ERR_PTR(-ENOMEM);
	memset(cqr, 0, sizeof(*cqr));
	data = (char *)cqr + cqr_size;
	cqr->cpaddr = NULL;
	if (cplength > 0) {
		cqr->cpaddr = data;
		data += cplength * sizeof(struct ccw1);
		memset(cqr->cpaddr, 0, cplength * sizeof(struct ccw1));
	}
	cqr->data = NULL;
	if (datasize > 0) {
		cqr->data = data;
		memset(cqr->data, 0, datasize);
	}

	cqr->magic = magic;
	set_bit(DASD_CQR_FLAGS_USE_ERP, &cqr->flags);
	dasd_get_device(device);

	return cqr;
}
EXPORT_SYMBOL(dasd_fmalloc_request);

void dasd_sfree_request(struct dasd_ccw_req *cqr, struct dasd_device *device)
{
	unsigned long flags;

	spin_lock_irqsave(&device->mem_lock, flags);
	dasd_free_chunk(&device->ccw_chunks, cqr->mem_chunk);
	spin_unlock_irqrestore(&device->mem_lock, flags);
	dasd_put_device(device);
}
EXPORT_SYMBOL(dasd_sfree_request);

void dasd_ffree_request(struct dasd_ccw_req *cqr, struct dasd_device *device)
{
	unsigned long flags;

	spin_lock_irqsave(&device->mem_lock, flags);
	dasd_free_chunk(&device->ese_chunks, cqr);
	spin_unlock_irqrestore(&device->mem_lock, flags);
	dasd_put_device(device);
}
EXPORT_SYMBOL(dasd_ffree_request);

/*
 * Check discipline magic in cqr.
 */
static inline int dasd_check_cqr(struct dasd_ccw_req *cqr)
{
	struct dasd_device *device;

	if (cqr == NULL)
		return -EINVAL;
	device = cqr->startdev;
	if (strncmp((char *) &cqr->magic, device->discipline->ebcname, 4)) {
		DBF_DEV_EVENT(DBF_WARNING, device,
			    " dasd_ccw_req 0x%08x magic doesn't match"
			    " discipline 0x%08x",
			    cqr->magic,
			    *(unsigned int *) device->discipline->name);
		return -EINVAL;
	}
	return 0;
}

/*
 * Terminate the current i/o and set the request to clear_pending.
 * Timer keeps device runnig.
 * ccw_device_clear can fail if the i/o subsystem
 * is in a bad mood.
 */
int dasd_term_IO(struct dasd_ccw_req *cqr)
{
	struct dasd_device *device;
	int retries, rc;
	char errorstring[ERRORLENGTH];

	/* Check the cqr */
	rc = dasd_check_cqr(cqr);
	if (rc)
		return rc;
	retries = 0;
	device = (struct dasd_device *) cqr->startdev;
	while ((retries < 5) && (cqr->status == DASD_CQR_IN_IO)) {
		rc = ccw_device_clear(device->cdev, (long) cqr);
		switch (rc) {
		case 0:	/* termination successful */
			cqr->status = DASD_CQR_CLEAR_PENDING;
			cqr->stopclk = get_tod_clock();
			cqr->starttime = 0;
			DBF_DEV_EVENT(DBF_DEBUG, device,
				      "terminate cqr %p successful",
				      cqr);
			break;
		case -ENODEV:
			DBF_DEV_EVENT(DBF_ERR, device, "%s",
				      "device gone, retry");
			break;
		case -EINVAL:
			/*
			 * device not valid so no I/O could be running
			 * handle CQR as termination successful
			 */
			cqr->status = DASD_CQR_CLEARED;
			cqr->stopclk = get_tod_clock();
			cqr->starttime = 0;
			/* no retries for invalid devices */
			cqr->retries = -1;
			DBF_DEV_EVENT(DBF_ERR, device, "%s",
				      "EINVAL, handle as terminated");
			/* fake rc to success */
			rc = 0;
			break;
		default:
			/* internal error 10 - unknown rc*/
			snprintf(errorstring, ERRORLENGTH, "10 %d", rc);
			dev_err(&device->cdev->dev, "An error occurred in the "
				"DASD device driver, reason=%s\n", errorstring);
			BUG();
			break;
		}
		retries++;
	}
	dasd_schedule_device_bh(device);
	return rc;
}
EXPORT_SYMBOL(dasd_term_IO);

/*
 * Start the i/o. This start_IO can fail if the channel is really busy.
 * In that case set up a timer to start the request later.
 */
int dasd_start_IO(struct dasd_ccw_req *cqr)
{
	struct dasd_device *device;
	int rc;
	char errorstring[ERRORLENGTH];

	/* Check the cqr */
	rc = dasd_check_cqr(cqr);
	if (rc) {
		cqr->intrc = rc;
		return rc;
	}
	device = (struct dasd_device *) cqr->startdev;
	if (((cqr->block &&
	      test_bit(DASD_FLAG_LOCK_STOLEN, &cqr->block->base->flags)) ||
	     test_bit(DASD_FLAG_LOCK_STOLEN, &device->flags)) &&
	    !test_bit(DASD_CQR_ALLOW_SLOCK, &cqr->flags)) {
		DBF_DEV_EVENT(DBF_DEBUG, device, "start_IO: return request %p "
			      "because of stolen lock", cqr);
		cqr->status = DASD_CQR_ERROR;
		cqr->intrc = -EPERM;
		return -EPERM;
	}
	if (cqr->retries < 0) {
		/* internal error 14 - start_IO run out of retries */
		sprintf(errorstring, "14 %p", cqr);
		dev_err(&device->cdev->dev, "An error occurred in the DASD "
			"device driver, reason=%s\n", errorstring);
		cqr->status = DASD_CQR_ERROR;
		return -EIO;
	}
	cqr->startclk = get_tod_clock();
	cqr->starttime = jiffies;
	cqr->retries--;
	if (!test_bit(DASD_CQR_VERIFY_PATH, &cqr->flags)) {
		cqr->lpm &= dasd_path_get_opm(device);
		if (!cqr->lpm)
			cqr->lpm = dasd_path_get_opm(device);
	}
	if (cqr->cpmode == 1) {
		rc = ccw_device_tm_start(device->cdev, cqr->cpaddr,
					 (long) cqr, cqr->lpm);
	} else {
		rc = ccw_device_start(device->cdev, cqr->cpaddr,
				      (long) cqr, cqr->lpm, 0);
	}
	switch (rc) {
	case 0:
		cqr->status = DASD_CQR_IN_IO;
		break;
	case -EBUSY:
		DBF_DEV_EVENT(DBF_WARNING, device, "%s",
			      "start_IO: device busy, retry later");
		break;
	case -EACCES:
		/* -EACCES indicates that the request used only a subset of the
		 * available paths and all these paths are gone. If the lpm of
		 * this request was only a subset of the opm (e.g. the ppm) then
		 * we just do a retry with all available paths.
		 * If we already use the full opm, something is amiss, and we
		 * need a full path verification.
		 */
		if (test_bit(DASD_CQR_VERIFY_PATH, &cqr->flags)) {
			DBF_DEV_EVENT(DBF_WARNING, device,
				      "start_IO: selected paths gone (%x)",
				      cqr->lpm);
		} else if (cqr->lpm != dasd_path_get_opm(device)) {
			cqr->lpm = dasd_path_get_opm(device);
			DBF_DEV_EVENT(DBF_DEBUG, device, "%s",
				      "start_IO: selected paths gone,"
				      " retry on all paths");
		} else {
			DBF_DEV_EVENT(DBF_WARNING, device, "%s",
				      "start_IO: all paths in opm gone,"
				      " do path verification");
			dasd_generic_last_path_gone(device);
			dasd_path_no_path(device);
			dasd_path_set_tbvpm(device,
					  ccw_device_get_path_mask(
						  device->cdev));
		}
		break;
	case -ENODEV:
		DBF_DEV_EVENT(DBF_WARNING, device, "%s",
			      "start_IO: -ENODEV device gone, retry");
		break;
	case -EIO:
		DBF_DEV_EVENT(DBF_WARNING, device, "%s",
			      "start_IO: -EIO device gone, retry");
		break;
	case -EINVAL:
		DBF_DEV_EVENT(DBF_WARNING, device, "%s",
			      "start_IO: -EINVAL device currently "
			      "not accessible");
		break;
	default:
		/* internal error 11 - unknown rc */
		snprintf(errorstring, ERRORLENGTH, "11 %d", rc);
		dev_err(&device->cdev->dev,
			"An error occurred in the DASD device driver, "
			"reason=%s\n", errorstring);
		BUG();
		break;
	}
	cqr->intrc = rc;
	return rc;
}
EXPORT_SYMBOL(dasd_start_IO);

/*
 * Timeout function for dasd devices. This is used for different purposes
 *  1) missing interrupt handler for normal operation
 *  2) delayed start of request where start_IO failed with -EBUSY
 *  3) timeout for missing state change interrupts
 * The head of the ccw queue will have status DASD_CQR_IN_IO for 1),
 * DASD_CQR_QUEUED for 2) and 3).
 */
static void dasd_device_timeout(struct timer_list *t)
{
	unsigned long flags;
	struct dasd_device *device;

	device = from_timer(device, t, timer);
	spin_lock_irqsave(get_ccwdev_lock(device->cdev), flags);
	/* re-activate request queue */
	dasd_device_remove_stop_bits(device, DASD_STOPPED_PENDING);
	spin_unlock_irqrestore(get_ccwdev_lock(device->cdev), flags);
	dasd_schedule_device_bh(device);
}

/*
 * Setup timeout for a device in jiffies.
 */
void dasd_device_set_timer(struct dasd_device *device, int expires)
{
	if (expires == 0)
		del_timer(&device->timer);
	else
		mod_timer(&device->timer, jiffies + expires);
}
EXPORT_SYMBOL(dasd_device_set_timer);

/*
 * Clear timeout for a device.
 */
void dasd_device_clear_timer(struct dasd_device *device)
{
	del_timer(&device->timer);
}
EXPORT_SYMBOL(dasd_device_clear_timer);

static void dasd_handle_killed_request(struct ccw_device *cdev,
				       unsigned long intparm)
{
	struct dasd_ccw_req *cqr;
	struct dasd_device *device;

	if (!intparm)
		return;
	cqr = (struct dasd_ccw_req *) intparm;
	if (cqr->status != DASD_CQR_IN_IO) {
		DBF_EVENT_DEVID(DBF_DEBUG, cdev,
				"invalid status in handle_killed_request: "
				"%02x", cqr->status);
		return;
	}

	device = dasd_device_from_cdev_locked(cdev);
	if (IS_ERR(device)) {
		DBF_EVENT_DEVID(DBF_DEBUG, cdev, "%s",
				"unable to get device from cdev");
		return;
	}

	if (!cqr->startdev ||
	    device != cqr->startdev ||
	    strncmp(cqr->startdev->discipline->ebcname,
		    (char *) &cqr->magic, 4)) {
		DBF_EVENT_DEVID(DBF_DEBUG, cdev, "%s",
				"invalid device in request");
		dasd_put_device(device);
		return;
	}

	/* Schedule request to be retried. */
	cqr->status = DASD_CQR_QUEUED;

	dasd_device_clear_timer(device);
	dasd_schedule_device_bh(device);
	dasd_put_device(device);
}

void dasd_generic_handle_state_change(struct dasd_device *device)
{
	/* First of all start sense subsystem status request. */
	dasd_eer_snss(device);

	dasd_device_remove_stop_bits(device, DASD_STOPPED_PENDING);
	dasd_schedule_device_bh(device);
	if (device->block) {
		dasd_schedule_block_bh(device->block);
		if (device->block->request_queue)
			blk_mq_run_hw_queues(device->block->request_queue,
					     true);
	}
}
EXPORT_SYMBOL_GPL(dasd_generic_handle_state_change);

static int dasd_check_hpf_error(struct irb *irb)
{
	return (scsw_tm_is_valid_schxs(&irb->scsw) &&
	    (irb->scsw.tm.sesq == SCSW_SESQ_DEV_NOFCX ||
	     irb->scsw.tm.sesq == SCSW_SESQ_PATH_NOFCX));
}

static int dasd_ese_needs_format(struct dasd_block *block, struct irb *irb)
{
	struct dasd_device *device = NULL;
	u8 *sense = NULL;

	if (!block)
		return 0;
	device = block->base;
	if (!device || !device->discipline->is_ese)
		return 0;
	if (!device->discipline->is_ese(device))
		return 0;

	sense = dasd_get_sense(irb);
	if (!sense)
		return 0;

	return !!(sense[1] & SNS1_NO_REC_FOUND) ||
		!!(sense[1] & SNS1_FILE_PROTECTED) ||
		scsw_cstat(&irb->scsw) == SCHN_STAT_INCORR_LEN;
}

static int dasd_ese_oos_cond(u8 *sense)
{
	return sense[0] & SNS0_EQUIPMENT_CHECK &&
		sense[1] & SNS1_PERM_ERR &&
		sense[1] & SNS1_WRITE_INHIBITED &&
		sense[25] == 0x01;
}

/*
 * Interrupt handler for "normal" ssch-io based dasd devices.
 */
void dasd_int_handler(struct ccw_device *cdev, unsigned long intparm,
		      struct irb *irb)
{
	struct dasd_ccw_req *cqr, *next, *fcqr;
	struct dasd_device *device;
	unsigned long now;
	int nrf_suppressed = 0;
	int fp_suppressed = 0;
	u8 *sense = NULL;
	int expires;

	cqr = (struct dasd_ccw_req *) intparm;
	if (IS_ERR(irb)) {
		switch (PTR_ERR(irb)) {
		case -EIO:
			if (cqr && cqr->status == DASD_CQR_CLEAR_PENDING) {
				device = cqr->startdev;
				cqr->status = DASD_CQR_CLEARED;
				dasd_device_clear_timer(device);
				wake_up(&dasd_flush_wq);
				dasd_schedule_device_bh(device);
				return;
			}
			break;
		case -ETIMEDOUT:
			DBF_EVENT_DEVID(DBF_WARNING, cdev, "%s: "
					"request timed out\n", __func__);
			break;
		default:
			DBF_EVENT_DEVID(DBF_WARNING, cdev, "%s: "
					"unknown error %ld\n", __func__,
					PTR_ERR(irb));
		}
		dasd_handle_killed_request(cdev, intparm);
		return;
	}

	now = get_tod_clock();
	/* check for conditions that should be handled immediately */
	if (!cqr ||
	    !(scsw_dstat(&irb->scsw) == (DEV_STAT_CHN_END | DEV_STAT_DEV_END) &&
	      scsw_cstat(&irb->scsw) == 0)) {
		if (cqr)
			memcpy(&cqr->irb, irb, sizeof(*irb));
		device = dasd_device_from_cdev_locked(cdev);
		if (IS_ERR(device))
			return;
		/* ignore unsolicited interrupts for DIAG discipline */
		if (device->discipline == dasd_diag_discipline_pointer) {
			dasd_put_device(device);
			return;
		}

		/*
		 * In some cases 'File Protected' or 'No Record Found' errors
		 * might be expected and debug log messages for the
		 * corresponding interrupts shouldn't be written then.
		 * Check if either of the according suppress bits is set.
		 */
		sense = dasd_get_sense(irb);
		if (sense) {
			fp_suppressed = (sense[1] & SNS1_FILE_PROTECTED) &&
				test_bit(DASD_CQR_SUPPRESS_FP, &cqr->flags);
			nrf_suppressed = (sense[1] & SNS1_NO_REC_FOUND) &&
				test_bit(DASD_CQR_SUPPRESS_NRF, &cqr->flags);

			/*
			 * Extent pool probably out-of-space.
			 * Stop device and check exhaust level.
			 */
			if (dasd_ese_oos_cond(sense)) {
				dasd_generic_space_exhaust(device, cqr);
				device->discipline->ext_pool_exhaust(device, cqr);
				dasd_put_device(device);
				return;
			}
		}
		if (!(fp_suppressed || nrf_suppressed))
			device->discipline->dump_sense_dbf(device, irb, "int");

		if (device->features & DASD_FEATURE_ERPLOG)
			device->discipline->dump_sense(device, cqr, irb);
		device->discipline->check_for_device_change(device, cqr, irb);
		dasd_put_device(device);
	}

	/* check for for attention message */
	if (scsw_dstat(&irb->scsw) & DEV_STAT_ATTENTION) {
		device = dasd_device_from_cdev_locked(cdev);
		if (!IS_ERR(device)) {
			device->discipline->check_attention(device,
							    irb->esw.esw1.lpum);
			dasd_put_device(device);
		}
	}

	if (!cqr)
		return;

	device = (struct dasd_device *) cqr->startdev;
	if (!device ||
	    strncmp(device->discipline->ebcname, (char *) &cqr->magic, 4)) {
		DBF_EVENT_DEVID(DBF_DEBUG, cdev, "%s",
				"invalid device in request");
		return;
	}

	if (dasd_ese_needs_format(cqr->block, irb)) {
		if (rq_data_dir((struct request *)cqr->callback_data) == READ) {
			device->discipline->ese_read(cqr, irb);
			cqr->status = DASD_CQR_SUCCESS;
			cqr->stopclk = now;
			dasd_device_clear_timer(device);
			dasd_schedule_device_bh(device);
			return;
		}
		fcqr = device->discipline->ese_format(device, cqr, irb);
		if (IS_ERR(fcqr)) {
			if (PTR_ERR(fcqr) == -EINVAL) {
				cqr->status = DASD_CQR_ERROR;
				return;
			}
			/*
			 * If we can't format now, let the request go
			 * one extra round. Maybe we can format later.
			 */
			cqr->status = DASD_CQR_QUEUED;
			dasd_schedule_device_bh(device);
			return;
		} else {
			fcqr->status = DASD_CQR_QUEUED;
			cqr->status = DASD_CQR_QUEUED;
			list_add(&fcqr->devlist, &device->ccw_queue);
			dasd_schedule_device_bh(device);
			return;
		}
	}

	/* Check for clear pending */
	if (cqr->status == DASD_CQR_CLEAR_PENDING &&
	    scsw_fctl(&irb->scsw) & SCSW_FCTL_CLEAR_FUNC) {
		cqr->status = DASD_CQR_CLEARED;
		dasd_device_clear_timer(device);
		wake_up(&dasd_flush_wq);
		dasd_schedule_device_bh(device);
		return;
	}

	/* check status - the request might have been killed by dyn detach */
	if (cqr->status != DASD_CQR_IN_IO) {
		DBF_DEV_EVENT(DBF_DEBUG, device, "invalid status: bus_id %s, "
			      "status %02x", dev_name(&cdev->dev), cqr->status);
		return;
	}

	next = NULL;
	expires = 0;
	if (scsw_dstat(&irb->scsw) == (DEV_STAT_CHN_END | DEV_STAT_DEV_END) &&
	    scsw_cstat(&irb->scsw) == 0) {
		/* request was completed successfully */
		cqr->status = DASD_CQR_SUCCESS;
		cqr->stopclk = now;
		/* Start first request on queue if possible -> fast_io. */
		if (cqr->devlist.next != &device->ccw_queue) {
			next = list_entry(cqr->devlist.next,
					  struct dasd_ccw_req, devlist);
		}
	} else {  /* error */
		/* check for HPF error
		 * call discipline function to requeue all requests
		 * and disable HPF accordingly
		 */
		if (cqr->cpmode && dasd_check_hpf_error(irb) &&
		    device->discipline->handle_hpf_error)
			device->discipline->handle_hpf_error(device, irb);
		/*
		 * If we don't want complex ERP for this request, then just
		 * reset this and retry it in the fastpath
		 */
		if (!test_bit(DASD_CQR_FLAGS_USE_ERP, &cqr->flags) &&
		    cqr->retries > 0) {
			if (cqr->lpm == dasd_path_get_opm(device))
				DBF_DEV_EVENT(DBF_DEBUG, device,
					      "default ERP in fastpath "
					      "(%i retries left)",
					      cqr->retries);
			if (!test_bit(DASD_CQR_VERIFY_PATH, &cqr->flags))
				cqr->lpm = dasd_path_get_opm(device);
			cqr->status = DASD_CQR_QUEUED;
			next = cqr;
		} else
			cqr->status = DASD_CQR_ERROR;
	}
	if (next && (next->status == DASD_CQR_QUEUED) &&
	    (!device->stopped)) {
		if (device->discipline->start_IO(next) == 0)
			expires = next->expires;
	}
	if (expires != 0)
		dasd_device_set_timer(device, expires);
	else
		dasd_device_clear_timer(device);
	dasd_schedule_device_bh(device);
}
EXPORT_SYMBOL(dasd_int_handler);

enum uc_todo dasd_generic_uc_handler(struct ccw_device *cdev, struct irb *irb)
{
	struct dasd_device *device;

	device = dasd_device_from_cdev_locked(cdev);

	if (IS_ERR(device))
		goto out;
	if (test_bit(DASD_FLAG_OFFLINE, &device->flags) ||
	   device->state != device->target ||
	   !device->discipline->check_for_device_change){
		dasd_put_device(device);
		goto out;
	}
	if (device->discipline->dump_sense_dbf)
		device->discipline->dump_sense_dbf(device, irb, "uc");
	device->discipline->check_for_device_change(device, NULL, irb);
	dasd_put_device(device);
out:
	return UC_TODO_RETRY;
}
EXPORT_SYMBOL_GPL(dasd_generic_uc_handler);

/*
 * If we have an error on a dasd_block layer request then we cancel
 * and return all further requests from the same dasd_block as well.
 */
static void __dasd_device_recovery(struct dasd_device *device,
				   struct dasd_ccw_req *ref_cqr)
{
	struct list_head *l, *n;
	struct dasd_ccw_req *cqr;

	/*
	 * only requeue request that came from the dasd_block layer
	 */
	if (!ref_cqr->block)
		return;

	list_for_each_safe(l, n, &device->ccw_queue) {
		cqr = list_entry(l, struct dasd_ccw_req, devlist);
		if (cqr->status == DASD_CQR_QUEUED &&
		    ref_cqr->block == cqr->block) {
			cqr->status = DASD_CQR_CLEARED;
		}
	}
};

/*
 * Remove those ccw requests from the queue that need to be returned
 * to the upper layer.
 */
static void __dasd_device_process_ccw_queue(struct dasd_device *device,
					    struct list_head *final_queue)
{
	struct list_head *l, *n;
	struct dasd_ccw_req *cqr;

	/* Process request with final status. */
	list_for_each_safe(l, n, &device->ccw_queue) {
		cqr = list_entry(l, struct dasd_ccw_req, devlist);

		/* Skip any non-final request. */
		if (cqr->status == DASD_CQR_QUEUED ||
		    cqr->status == DASD_CQR_IN_IO ||
		    cqr->status == DASD_CQR_CLEAR_PENDING)
			continue;
		if (cqr->status == DASD_CQR_ERROR) {
			__dasd_device_recovery(device, cqr);
		}
		/* Rechain finished requests to final queue */
		list_move_tail(&cqr->devlist, final_queue);
	}
}

static void __dasd_process_cqr(struct dasd_device *device,
			       struct dasd_ccw_req *cqr)
{
	char errorstring[ERRORLENGTH];

	switch (cqr->status) {
	case DASD_CQR_SUCCESS:
		cqr->status = DASD_CQR_DONE;
		break;
	case DASD_CQR_ERROR:
		cqr->status = DASD_CQR_NEED_ERP;
		break;
	case DASD_CQR_CLEARED:
		cqr->status = DASD_CQR_TERMINATED;
		break;
	default:
		/* internal error 12 - wrong cqr status*/
		snprintf(errorstring, ERRORLENGTH, "12 %p %x02", cqr, cqr->status);
		dev_err(&device->cdev->dev,
			"An error occurred in the DASD device driver, "
			"reason=%s\n", errorstring);
		BUG();
	}
	if (cqr->callback)
		cqr->callback(cqr, cqr->callback_data);
}

/*
 * the cqrs from the final queue are returned to the upper layer
 * by setting a dasd_block state and calling the callback function
 */
static void __dasd_device_process_final_queue(struct dasd_device *device,
					      struct list_head *final_queue)
{
	struct list_head *l, *n;
	struct dasd_ccw_req *cqr;
	struct dasd_block *block;

	list_for_each_safe(l, n, final_queue) {
		cqr = list_entry(l, struct dasd_ccw_req, devlist);
		list_del_init(&cqr->devlist);
		block = cqr->block;
		if (!block) {
			__dasd_process_cqr(device, cqr);
		} else {
			spin_lock_bh(&block->queue_lock);
			__dasd_process_cqr(device, cqr);
			spin_unlock_bh(&block->queue_lock);
		}
	}
}

/*
 * Take a look at the first request on the ccw queue and check
 * if it reached its expire time. If so, terminate the IO.
 */
static void __dasd_device_check_expire(struct dasd_device *device)
{
	struct dasd_ccw_req *cqr;

	if (list_empty(&device->ccw_queue))
		return;
	cqr = list_entry(device->ccw_queue.next, struct dasd_ccw_req, devlist);
	if ((cqr->status == DASD_CQR_IN_IO && cqr->expires != 0) &&
	    (time_after_eq(jiffies, cqr->expires + cqr->starttime))) {
		if (test_bit(DASD_FLAG_SAFE_OFFLINE_RUNNING, &device->flags)) {
			/*
			 * IO in safe offline processing should not
			 * run out of retries
			 */
			cqr->retries++;
		}
		if (device->discipline->term_IO(cqr) != 0) {
			/* Hmpf, try again in 5 sec */
			dev_err(&device->cdev->dev,
				"cqr %p timed out (%lus) but cannot be "
				"ended, retrying in 5 s\n",
				cqr, (cqr->expires/HZ));
			cqr->expires += 5*HZ;
			dasd_device_set_timer(device, 5*HZ);
		} else {
			dev_err(&device->cdev->dev,
				"cqr %p timed out (%lus), %i retries "
				"remaining\n", cqr, (cqr->expires/HZ),
				cqr->retries);
		}
	}
}

/*
 * return 1 when device is not eligible for IO
 */
static int __dasd_device_is_unusable(struct dasd_device *device,
				     struct dasd_ccw_req *cqr)
{
	int mask = ~(DASD_STOPPED_DC_WAIT | DASD_STOPPED_NOSPC);

	if (test_bit(DASD_FLAG_OFFLINE, &device->flags) &&
	    !test_bit(DASD_FLAG_SAFE_OFFLINE_RUNNING, &device->flags)) {
		/*
		 * dasd is being set offline
		 * but it is no safe offline where we have to allow I/O
		 */
		return 1;
	}
	if (device->stopped) {
		if (device->stopped & mask) {
			/* stopped and CQR will not change that. */
			return 1;
		}
		if (!test_bit(DASD_CQR_VERIFY_PATH, &cqr->flags)) {
			/* CQR is not able to change device to
			 * operational. */
			return 1;
		}
		/* CQR required to get device operational. */
	}
	return 0;
}

/*
 * Take a look at the first request on the ccw queue and check
 * if it needs to be started.
 */
static void __dasd_device_start_head(struct dasd_device *device)
{
	struct dasd_ccw_req *cqr;
	int rc;

	if (list_empty(&device->ccw_queue))
		return;
	cqr = list_entry(device->ccw_queue.next, struct dasd_ccw_req, devlist);
	if (cqr->status != DASD_CQR_QUEUED)
		return;
	/* if device is not usable return request to upper layer */
	if (__dasd_device_is_unusable(device, cqr)) {
		cqr->intrc = -EAGAIN;
		cqr->status = DASD_CQR_CLEARED;
		dasd_schedule_device_bh(device);
		return;
	}

	rc = device->discipline->start_IO(cqr);
	if (rc == 0)
		dasd_device_set_timer(device, cqr->expires);
	else if (rc == -EACCES) {
		dasd_schedule_device_bh(device);
	} else
		/* Hmpf, try again in 1/2 sec */
		dasd_device_set_timer(device, 50);
}

static void __dasd_device_check_path_events(struct dasd_device *device)
{
	__u8 tbvpm, fcsecpm;
	int rc;

	tbvpm = dasd_path_get_tbvpm(device);
	fcsecpm = dasd_path_get_fcsecpm(device);

	if (!tbvpm && !fcsecpm)
		return;

	if (device->stopped & ~(DASD_STOPPED_DC_WAIT))
		return;
	rc = device->discipline->pe_handler(device, tbvpm, fcsecpm);
	if (rc) {
		dasd_device_set_timer(device, 50);
	} else {
		dasd_path_clear_all_verify(device);
		dasd_path_clear_all_fcsec(device);
	}
};

/*
 * Go through all request on the dasd_device request queue,
 * terminate them on the cdev if necessary, and return them to the
 * submitting layer via callback.
 * Note:
 * Make sure that all 'submitting layers' still exist when
 * this function is called!. In other words, when 'device' is a base
 * device then all block layer requests must have been removed before
 * via dasd_flush_block_queue.
 */
int dasd_flush_device_queue(struct dasd_device *device)
{
	struct dasd_ccw_req *cqr, *n;
	int rc;
	struct list_head flush_queue;

	INIT_LIST_HEAD(&flush_queue);
	spin_lock_irq(get_ccwdev_lock(device->cdev));
	rc = 0;
	list_for_each_entry_safe(cqr, n, &device->ccw_queue, devlist) {
		/* Check status and move request to flush_queue */
		switch (cqr->status) {
		case DASD_CQR_IN_IO:
			rc = device->discipline->term_IO(cqr);
			if (rc) {
				/* unable to terminate requeust */
				dev_err(&device->cdev->dev,
					"Flushing the DASD request queue "
					"failed for request %p\n", cqr);
				/* stop flush processing */
				goto finished;
			}
			break;
		case DASD_CQR_QUEUED:
			cqr->stopclk = get_tod_clock();
			cqr->status = DASD_CQR_CLEARED;
			break;
		default: /* no need to modify the others */
			break;
		}
		list_move_tail(&cqr->devlist, &flush_queue);
	}
finished:
	spin_unlock_irq(get_ccwdev_lock(device->cdev));
	/*
	 * After this point all requests must be in state CLEAR_PENDING,
	 * CLEARED, SUCCESS or ERROR. Now wait for CLEAR_PENDING to become
	 * one of the others.
	 */
	list_for_each_entry_safe(cqr, n, &flush_queue, devlist)
		wait_event(dasd_flush_wq,
			   (cqr->status != DASD_CQR_CLEAR_PENDING));
	/*
	 * Now set each request back to TERMINATED, DONE or NEED_ERP
	 * and call the callback function of flushed requests
	 */
	__dasd_device_process_final_queue(device, &flush_queue);
	return rc;
}
EXPORT_SYMBOL_GPL(dasd_flush_device_queue);

/*
 * Acquire the device lock and process queues for the device.
 */
static void dasd_device_tasklet(unsigned long data)
{
	struct dasd_device *device = (struct dasd_device *) data;
	struct list_head final_queue;

	atomic_set (&device->tasklet_scheduled, 0);
	INIT_LIST_HEAD(&final_queue);
	spin_lock_irq(get_ccwdev_lock(device->cdev));
	/* Check expire time of first request on the ccw queue. */
	__dasd_device_check_expire(device);
	/* find final requests on ccw queue */
	__dasd_device_process_ccw_queue(device, &final_queue);
	__dasd_device_check_path_events(device);
	spin_unlock_irq(get_ccwdev_lock(device->cdev));
	/* Now call the callback function of requests with final status */
	__dasd_device_process_final_queue(device, &final_queue);
	spin_lock_irq(get_ccwdev_lock(device->cdev));
	/* Now check if the head of the ccw queue needs to be started. */
	__dasd_device_start_head(device);
	spin_unlock_irq(get_ccwdev_lock(device->cdev));
	if (waitqueue_active(&shutdown_waitq))
		wake_up(&shutdown_waitq);
	dasd_put_device(device);
}

/*
 * Schedules a call to dasd_tasklet over the device tasklet.
 */
void dasd_schedule_device_bh(struct dasd_device *device)
{
	/* Protect against rescheduling. */
	if (atomic_cmpxchg (&device->tasklet_scheduled, 0, 1) != 0)
		return;
	dasd_get_device(device);
	tasklet_hi_schedule(&device->tasklet);
}
EXPORT_SYMBOL(dasd_schedule_device_bh);

void dasd_device_set_stop_bits(struct dasd_device *device, int bits)
{
	device->stopped |= bits;
}
EXPORT_SYMBOL_GPL(dasd_device_set_stop_bits);

void dasd_device_remove_stop_bits(struct dasd_device *device, int bits)
{
	device->stopped &= ~bits;
	if (!device->stopped)
		wake_up(&generic_waitq);
}
EXPORT_SYMBOL_GPL(dasd_device_remove_stop_bits);

/*
 * Queue a request to the head of the device ccw_queue.
 * Start the I/O if possible.
 */
void dasd_add_request_head(struct dasd_ccw_req *cqr)
{
	struct dasd_device *device;
	unsigned long flags;

	device = cqr->startdev;
	spin_lock_irqsave(get_ccwdev_lock(device->cdev), flags);
	cqr->status = DASD_CQR_QUEUED;
	list_add(&cqr->devlist, &device->ccw_queue);
	/* let the bh start the request to keep them in order */
	dasd_schedule_device_bh(device);
	spin_unlock_irqrestore(get_ccwdev_lock(device->cdev), flags);
}
EXPORT_SYMBOL(dasd_add_request_head);

/*
 * Queue a request to the tail of the device ccw_queue.
 * Start the I/O if possible.
 */
void dasd_add_request_tail(struct dasd_ccw_req *cqr)
{
	struct dasd_device *device;
	unsigned long flags;

	device = cqr->startdev;
	spin_lock_irqsave(get_ccwdev_lock(device->cdev), flags);
	cqr->status = DASD_CQR_QUEUED;
	list_add_tail(&cqr->devlist, &device->ccw_queue);
	/* let the bh start the request to keep them in order */
	dasd_schedule_device_bh(device);
	spin_unlock_irqrestore(get_ccwdev_lock(device->cdev), flags);
}
EXPORT_SYMBOL(dasd_add_request_tail);

/*
 * Wakeup helper for the 'sleep_on' functions.
 */
void dasd_wakeup_cb(struct dasd_ccw_req *cqr, void *data)
{
	spin_lock_irq(get_ccwdev_lock(cqr->startdev->cdev));
	cqr->callback_data = DASD_SLEEPON_END_TAG;
	spin_unlock_irq(get_ccwdev_lock(cqr->startdev->cdev));
	wake_up(&generic_waitq);
}
EXPORT_SYMBOL_GPL(dasd_wakeup_cb);

static inline int _wait_for_wakeup(struct dasd_ccw_req *cqr)
{
	struct dasd_device *device;
	int rc;

	device = cqr->startdev;
	spin_lock_irq(get_ccwdev_lock(device->cdev));
	rc = (cqr->callback_data == DASD_SLEEPON_END_TAG);
	spin_unlock_irq(get_ccwdev_lock(device->cdev));
	return rc;
}

/*
 * checks if error recovery is necessary, returns 1 if yes, 0 otherwise.
 */
static int __dasd_sleep_on_erp(struct dasd_ccw_req *cqr)
{
	struct dasd_device *device;
	dasd_erp_fn_t erp_fn;

	if (cqr->status == DASD_CQR_FILLED)
		return 0;
	device = cqr->startdev;
	if (test_bit(DASD_CQR_FLAGS_USE_ERP, &cqr->flags)) {
		if (cqr->status == DASD_CQR_TERMINATED) {
			device->discipline->handle_terminated_request(cqr);
			return 1;
		}
		if (cqr->status == DASD_CQR_NEED_ERP) {
			erp_fn = device->discipline->erp_action(cqr);
			erp_fn(cqr);
			return 1;
		}
		if (cqr->status == DASD_CQR_FAILED)
			dasd_log_sense(cqr, &cqr->irb);
		if (cqr->refers) {
			__dasd_process_erp(device, cqr);
			return 1;
		}
	}
	return 0;
}

static int __dasd_sleep_on_loop_condition(struct dasd_ccw_req *cqr)
{
	if (test_bit(DASD_CQR_FLAGS_USE_ERP, &cqr->flags)) {
		if (cqr->refers) /* erp is not done yet */
			return 1;
		return ((cqr->status != DASD_CQR_DONE) &&
			(cqr->status != DASD_CQR_FAILED));
	} else
		return (cqr->status == DASD_CQR_FILLED);
}

static int _dasd_sleep_on(struct dasd_ccw_req *maincqr, int interruptible)
{
	struct dasd_device *device;
	int rc;
	struct list_head ccw_queue;
	struct dasd_ccw_req *cqr;

	INIT_LIST_HEAD(&ccw_queue);
	maincqr->status = DASD_CQR_FILLED;
	device = maincqr->startdev;
	list_add(&maincqr->blocklist, &ccw_queue);
	for (cqr = maincqr;  __dasd_sleep_on_loop_condition(cqr);
	     cqr = list_first_entry(&ccw_queue,
				    struct dasd_ccw_req, blocklist)) {

		if (__dasd_sleep_on_erp(cqr))
			continue;
		if (cqr->status != DASD_CQR_FILLED) /* could be failed */
			continue;
		if (test_bit(DASD_FLAG_LOCK_STOLEN, &device->flags) &&
		    !test_bit(DASD_CQR_ALLOW_SLOCK, &cqr->flags)) {
			cqr->status = DASD_CQR_FAILED;
			cqr->intrc = -EPERM;
			continue;
		}
		/* Non-temporary stop condition will trigger fail fast */
		if (device->stopped & ~DASD_STOPPED_PENDING &&
		    test_bit(DASD_CQR_FLAGS_FAILFAST, &cqr->flags) &&
		    (!dasd_eer_enabled(device))) {
			cqr->status = DASD_CQR_FAILED;
			cqr->intrc = -ENOLINK;
			continue;
		}
		/*
		 * Don't try to start requests if device is in
		 * offline processing, it might wait forever
		 */
		if (test_bit(DASD_FLAG_OFFLINE, &device->flags)) {
			cqr->status = DASD_CQR_FAILED;
			cqr->intrc = -ENODEV;
			continue;
		}
		/*
		 * Don't try to start requests if device is stopped
		 * except path verification requests
		 */
		if (!test_bit(DASD_CQR_VERIFY_PATH, &cqr->flags)) {
			if (interruptible) {
				rc = wait_event_interruptible(
					generic_waitq, !(device->stopped));
				if (rc == -ERESTARTSYS) {
					cqr->status = DASD_CQR_FAILED;
					maincqr->intrc = rc;
					continue;
				}
			} else
				wait_event(generic_waitq, !(device->stopped));
		}
		if (!cqr->callback)
			cqr->callback = dasd_wakeup_cb;

		cqr->callback_data = DASD_SLEEPON_START_TAG;
		dasd_add_request_tail(cqr);
		if (interruptible) {
			rc = wait_event_interruptible(
				generic_waitq, _wait_for_wakeup(cqr));
			if (rc == -ERESTARTSYS) {
				dasd_cancel_req(cqr);
				/* wait (non-interruptible) for final status */
				wait_event(generic_waitq,
					   _wait_for_wakeup(cqr));
				cqr->status = DASD_CQR_FAILED;
				maincqr->intrc = rc;
				continue;
			}
		} else
			wait_event(generic_waitq, _wait_for_wakeup(cqr));
	}

	maincqr->endclk = get_tod_clock();
	if ((maincqr->status != DASD_CQR_DONE) &&
	    (maincqr->intrc != -ERESTARTSYS))
		dasd_log_sense(maincqr, &maincqr->irb);
	if (maincqr->status == DASD_CQR_DONE)
		rc = 0;
	else if (maincqr->intrc)
		rc = maincqr->intrc;
	else
		rc = -EIO;
	return rc;
}

static inline int _wait_for_wakeup_queue(struct list_head *ccw_queue)
{
	struct dasd_ccw_req *cqr;

	list_for_each_entry(cqr, ccw_queue, blocklist) {
		if (cqr->callback_data != DASD_SLEEPON_END_TAG)
			return 0;
	}

	return 1;
}

static int _dasd_sleep_on_queue(struct list_head *ccw_queue, int interruptible)
{
	struct dasd_device *device;
	struct dasd_ccw_req *cqr, *n;
	u8 *sense = NULL;
	int rc;

retry:
	list_for_each_entry_safe(cqr, n, ccw_queue, blocklist) {
		device = cqr->startdev;
		if (cqr->status != DASD_CQR_FILLED) /*could be failed*/
			continue;

		if (test_bit(DASD_FLAG_LOCK_STOLEN, &device->flags) &&
		    !test_bit(DASD_CQR_ALLOW_SLOCK, &cqr->flags)) {
			cqr->status = DASD_CQR_FAILED;
			cqr->intrc = -EPERM;
			continue;
		}
		/*Non-temporary stop condition will trigger fail fast*/
		if (device->stopped & ~DASD_STOPPED_PENDING &&
		    test_bit(DASD_CQR_FLAGS_FAILFAST, &cqr->flags) &&
		    !dasd_eer_enabled(device)) {
			cqr->status = DASD_CQR_FAILED;
			cqr->intrc = -EAGAIN;
			continue;
		}

		/*Don't try to start requests if device is stopped*/
		if (interruptible) {
			rc = wait_event_interruptible(
				generic_waitq, !device->stopped);
			if (rc == -ERESTARTSYS) {
				cqr->status = DASD_CQR_FAILED;
				cqr->intrc = rc;
				continue;
			}
		} else
			wait_event(generic_waitq, !(device->stopped));

		if (!cqr->callback)
			cqr->callback = dasd_wakeup_cb;
		cqr->callback_data = DASD_SLEEPON_START_TAG;
		dasd_add_request_tail(cqr);
	}

	wait_event(generic_waitq, _wait_for_wakeup_queue(ccw_queue));

	rc = 0;
	list_for_each_entry_safe(cqr, n, ccw_queue, blocklist) {
		/*
		 * In some cases the 'File Protected' or 'Incorrect Length'
		 * error might be expected and error recovery would be
		 * unnecessary in these cases.	Check if the according suppress
		 * bit is set.
		 */
		sense = dasd_get_sense(&cqr->irb);
		if (sense && sense[1] & SNS1_FILE_PROTECTED &&
		    test_bit(DASD_CQR_SUPPRESS_FP, &cqr->flags))
			continue;
		if (scsw_cstat(&cqr->irb.scsw) == 0x40 &&
		    test_bit(DASD_CQR_SUPPRESS_IL, &cqr->flags))
			continue;

		/*
		 * for alias devices simplify error recovery and
		 * return to upper layer
		 * do not skip ERP requests
		 */
		if (cqr->startdev != cqr->basedev && !cqr->refers &&
		    (cqr->status == DASD_CQR_TERMINATED ||
		     cqr->status == DASD_CQR_NEED_ERP))
			return -EAGAIN;

		/* normal recovery for basedev IO */
		if (__dasd_sleep_on_erp(cqr))
			/* handle erp first */
			goto retry;
	}

	return 0;
}

/*
 * Queue a request to the tail of the device ccw_queue and wait for
 * it's completion.
 */
int dasd_sleep_on(struct dasd_ccw_req *cqr)
{
	return _dasd_sleep_on(cqr, 0);
}
EXPORT_SYMBOL(dasd_sleep_on);

/*
 * Start requests from a ccw_queue and wait for their completion.
 */
int dasd_sleep_on_queue(struct list_head *ccw_queue)
{
	return _dasd_sleep_on_queue(ccw_queue, 0);
}
EXPORT_SYMBOL(dasd_sleep_on_queue);

/*
 * Start requests from a ccw_queue and wait interruptible for their completion.
 */
int dasd_sleep_on_queue_interruptible(struct list_head *ccw_queue)
{
	return _dasd_sleep_on_queue(ccw_queue, 1);
}
EXPORT_SYMBOL(dasd_sleep_on_queue_interruptible);

/*
 * Queue a request to the tail of the device ccw_queue and wait
 * interruptible for it's completion.
 */
int dasd_sleep_on_interruptible(struct dasd_ccw_req *cqr)
{
	return _dasd_sleep_on(cqr, 1);
}
EXPORT_SYMBOL(dasd_sleep_on_interruptible);

/*
 * Whoa nelly now it gets really hairy. For some functions (e.g. steal lock
 * for eckd devices) the currently running request has to be terminated
 * and be put back to status queued, before the special request is added
 * to the head of the queue. Then the special request is waited on normally.
 */
static inline int _dasd_term_running_cqr(struct dasd_device *device)
{
	struct dasd_ccw_req *cqr;
	int rc;

	if (list_empty(&device->ccw_queue))
		return 0;
	cqr = list_entry(device->ccw_queue.next, struct dasd_ccw_req, devlist);
	rc = device->discipline->term_IO(cqr);
	if (!rc)
		/*
		 * CQR terminated because a more important request is pending.
		 * Undo decreasing of retry counter because this is
		 * not an error case.
		 */
		cqr->retries++;
	return rc;
}

int dasd_sleep_on_immediatly(struct dasd_ccw_req *cqr)
{
	struct dasd_device *device;
	int rc;

	device = cqr->startdev;
	if (test_bit(DASD_FLAG_LOCK_STOLEN, &device->flags) &&
	    !test_bit(DASD_CQR_ALLOW_SLOCK, &cqr->flags)) {
		cqr->status = DASD_CQR_FAILED;
		cqr->intrc = -EPERM;
		return -EIO;
	}
	spin_lock_irq(get_ccwdev_lock(device->cdev));
	rc = _dasd_term_running_cqr(device);
	if (rc) {
		spin_unlock_irq(get_ccwdev_lock(device->cdev));
		return rc;
	}
	cqr->callback = dasd_wakeup_cb;
	cqr->callback_data = DASD_SLEEPON_START_TAG;
	cqr->status = DASD_CQR_QUEUED;
	/*
	 * add new request as second
	 * first the terminated cqr needs to be finished
	 */
	list_add(&cqr->devlist, device->ccw_queue.next);

	/* let the bh start the request to keep them in order */
	dasd_schedule_device_bh(device);

	spin_unlock_irq(get_ccwdev_lock(device->cdev));

	wait_event(generic_waitq, _wait_for_wakeup(cqr));

	if (cqr->status == DASD_CQR_DONE)
		rc = 0;
	else if (cqr->intrc)
		rc = cqr->intrc;
	else
		rc = -EIO;

	/* kick tasklets */
	dasd_schedule_device_bh(device);
	if (device->block)
		dasd_schedule_block_bh(device->block);

	return rc;
}
EXPORT_SYMBOL(dasd_sleep_on_immediatly);

/*
 * Cancels a request that was started with dasd_sleep_on_req.
 * This is useful to timeout requests. The request will be
 * terminated if it is currently in i/o.
 * Returns 0 if request termination was successful
 *	   negative error code if termination failed
 * Cancellation of a request is an asynchronous operation! The calling
 * function has to wait until the request is properly returned via callback.
 */
static int __dasd_cancel_req(struct dasd_ccw_req *cqr)
{
	struct dasd_device *device = cqr->startdev;
	int rc = 0;

	switch (cqr->status) {
	case DASD_CQR_QUEUED:
		/* request was not started - just set to cleared */
		cqr->status = DASD_CQR_CLEARED;
		break;
	case DASD_CQR_IN_IO:
		/* request in IO - terminate IO and release again */
		rc = device->discipline->term_IO(cqr);
		if (rc) {
			dev_err(&device->cdev->dev,
				"Cancelling request %p failed with rc=%d\n",
				cqr, rc);
		} else {
			cqr->stopclk = get_tod_clock();
		}
		break;
	default: /* already finished or clear pending - do nothing */
		break;
	}
	dasd_schedule_device_bh(device);
	return rc;
}

int dasd_cancel_req(struct dasd_ccw_req *cqr)
{
	struct dasd_device *device = cqr->startdev;
	unsigned long flags;
	int rc;

	spin_lock_irqsave(get_ccwdev_lock(device->cdev), flags);
	rc = __dasd_cancel_req(cqr);
	spin_unlock_irqrestore(get_ccwdev_lock(device->cdev), flags);
	return rc;
}

/*
 * SECTION: Operations of the dasd_block layer.
 */

/*
 * Timeout function for dasd_block. This is used when the block layer
 * is waiting for something that may not come reliably, (e.g. a state
 * change interrupt)
 */
static void dasd_block_timeout(struct timer_list *t)
{
	unsigned long flags;
	struct dasd_block *block;

	block = from_timer(block, t, timer);
	spin_lock_irqsave(get_ccwdev_lock(block->base->cdev), flags);
	/* re-activate request queue */
	dasd_device_remove_stop_bits(block->base, DASD_STOPPED_PENDING);
	spin_unlock_irqrestore(get_ccwdev_lock(block->base->cdev), flags);
	dasd_schedule_block_bh(block);
	blk_mq_run_hw_queues(block->request_queue, true);
}

/*
 * Setup timeout for a dasd_block in jiffies.
 */
void dasd_block_set_timer(struct dasd_block *block, int expires)
{
	if (expires == 0)
		del_timer(&block->timer);
	else
		mod_timer(&block->timer, jiffies + expires);
}
EXPORT_SYMBOL(dasd_block_set_timer);

/*
 * Clear timeout for a dasd_block.
 */
void dasd_block_clear_timer(struct dasd_block *block)
{
	del_timer(&block->timer);
}
EXPORT_SYMBOL(dasd_block_clear_timer);

/*
 * Process finished error recovery ccw.
 */
static void __dasd_process_erp(struct dasd_device *device,
			       struct dasd_ccw_req *cqr)
{
	dasd_erp_fn_t erp_fn;

	if (cqr->status == DASD_CQR_DONE)
		DBF_DEV_EVENT(DBF_NOTICE, device, "%s", "ERP successful");
	else
		dev_err(&device->cdev->dev, "ERP failed for the DASD\n");
	erp_fn = device->discipline->erp_postaction(cqr);
	erp_fn(cqr);
}

static void __dasd_cleanup_cqr(struct dasd_ccw_req *cqr)
{
	struct request *req;
	blk_status_t error = BLK_STS_OK;
	unsigned int proc_bytes;
	int status;

	req = (struct request *) cqr->callback_data;
	dasd_profile_end(cqr->block, cqr, req);

	proc_bytes = cqr->proc_bytes;
	status = cqr->block->base->discipline->free_cp(cqr, req);
	if (status < 0)
		error = errno_to_blk_status(status);
	else if (status == 0) {
		switch (cqr->intrc) {
		case -EPERM:
			error = BLK_STS_NEXUS;
			break;
		case -ENOLINK:
			error = BLK_STS_TRANSPORT;
			break;
		case -ETIMEDOUT:
			error = BLK_STS_TIMEOUT;
			break;
		default:
			error = BLK_STS_IOERR;
			break;
		}
	}

	/*
	 * We need to take care for ETIMEDOUT errors here since the
	 * complete callback does not get called in this case.
	 * Take care of all errors here and avoid additional code to
	 * transfer the error value to the complete callback.
	 */
	if (error) {
		blk_mq_end_request(req, error);
		blk_mq_run_hw_queues(req->q, true);
	} else {
		/*
		 * Partial completed requests can happen with ESE devices.
		 * During read we might have gotten a NRF error and have to
		 * complete a request partially.
		 */
		if (proc_bytes) {
			blk_update_request(req, BLK_STS_OK,
					   blk_rq_bytes(req) - proc_bytes);
			blk_mq_requeue_request(req, true);
		} else if (likely(!blk_should_fake_timeout(req->q))) {
			blk_mq_complete_request(req);
		}
	}
}

/*
 * Process ccw request queue.
 */
static void __dasd_process_block_ccw_queue(struct dasd_block *block,
					   struct list_head *final_queue)
{
	struct list_head *l, *n;
	struct dasd_ccw_req *cqr;
	dasd_erp_fn_t erp_fn;
	unsigned long flags;
	struct dasd_device *base = block->base;

restart:
	/* Process request with final status. */
	list_for_each_safe(l, n, &block->ccw_queue) {
		cqr = list_entry(l, struct dasd_ccw_req, blocklist);
		if (cqr->status != DASD_CQR_DONE &&
		    cqr->status != DASD_CQR_FAILED &&
		    cqr->status != DASD_CQR_NEED_ERP &&
		    cqr->status != DASD_CQR_TERMINATED)
			continue;

		if (cqr->status == DASD_CQR_TERMINATED) {
			base->discipline->handle_terminated_request(cqr);
			goto restart;
		}

		/*  Process requests that may be recovered */
		if (cqr->status == DASD_CQR_NEED_ERP) {
			erp_fn = base->discipline->erp_action(cqr);
			if (IS_ERR(erp_fn(cqr)))
				continue;
			goto restart;
		}

		/* log sense for fatal error */
		if (cqr->status == DASD_CQR_FAILED) {
			dasd_log_sense(cqr, &cqr->irb);
		}

		/* First of all call extended error reporting. */
		if (dasd_eer_enabled(base) &&
		    cqr->status == DASD_CQR_FAILED) {
			dasd_eer_write(base, cqr, DASD_EER_FATALERROR);

			/* restart request  */
			cqr->status = DASD_CQR_FILLED;
			cqr->retries = 255;
			spin_lock_irqsave(get_ccwdev_lock(base->cdev), flags);
			dasd_device_set_stop_bits(base, DASD_STOPPED_QUIESCE);
			spin_unlock_irqrestore(get_ccwdev_lock(base->cdev),
					       flags);
			goto restart;
		}

		/* Process finished ERP request. */
		if (cqr->refers) {
			__dasd_process_erp(base, cqr);
			goto restart;
		}

		/* Rechain finished requests to final queue */
		cqr->endclk = get_tod_clock();
		list_move_tail(&cqr->blocklist, final_queue);
	}
}

static void dasd_return_cqr_cb(struct dasd_ccw_req *cqr, void *data)
{
	dasd_schedule_block_bh(cqr->block);
}

static void __dasd_block_start_head(struct dasd_block *block)
{
	struct dasd_ccw_req *cqr;

	if (list_empty(&block->ccw_queue))
		return;
	/* We allways begin with the first requests on the queue, as some
	 * of previously started requests have to be enqueued on a
	 * dasd_device again for error recovery.
	 */
	list_for_each_entry(cqr, &block->ccw_queue, blocklist) {
		if (cqr->status != DASD_CQR_FILLED)
			continue;
		if (test_bit(DASD_FLAG_LOCK_STOLEN, &block->base->flags) &&
		    !test_bit(DASD_CQR_ALLOW_SLOCK, &cqr->flags)) {
			cqr->status = DASD_CQR_FAILED;
			cqr->intrc = -EPERM;
			dasd_schedule_block_bh(block);
			continue;
		}
		/* Non-temporary stop condition will trigger fail fast */
		if (block->base->stopped & ~DASD_STOPPED_PENDING &&
		    test_bit(DASD_CQR_FLAGS_FAILFAST, &cqr->flags) &&
		    (!dasd_eer_enabled(block->base))) {
			cqr->status = DASD_CQR_FAILED;
			cqr->intrc = -ENOLINK;
			dasd_schedule_block_bh(block);
			continue;
		}
		/* Don't try to start requests if device is stopped */
		if (block->base->stopped)
			return;

		/* just a fail safe check, should not happen */
		if (!cqr->startdev)
			cqr->startdev = block->base;

		/* make sure that the requests we submit find their way back */
		cqr->callback = dasd_return_cqr_cb;

		dasd_add_request_tail(cqr);
	}
}

/*
 * Central dasd_block layer routine. Takes requests from the generic
 * block layer request queue, creates ccw requests, enqueues them on
 * a dasd_device and processes ccw requests that have been returned.
 */
static void dasd_block_tasklet(unsigned long data)
{
	struct dasd_block *block = (struct dasd_block *) data;
	struct list_head final_queue;
	struct list_head *l, *n;
	struct dasd_ccw_req *cqr;
	struct dasd_queue *dq;

	atomic_set(&block->tasklet_scheduled, 0);
	INIT_LIST_HEAD(&final_queue);
	spin_lock_irq(&block->queue_lock);
	/* Finish off requests on ccw queue */
	__dasd_process_block_ccw_queue(block, &final_queue);
	spin_unlock_irq(&block->queue_lock);

	/* Now call the callback function of requests with final status */
	list_for_each_safe(l, n, &final_queue) {
		cqr = list_entry(l, struct dasd_ccw_req, blocklist);
		dq = cqr->dq;
		spin_lock_irq(&dq->lock);
		list_del_init(&cqr->blocklist);
		__dasd_cleanup_cqr(cqr);
		spin_unlock_irq(&dq->lock);
	}

	spin_lock_irq(&block->queue_lock);
	/* Now check if the head of the ccw queue needs to be started. */
	__dasd_block_start_head(block);
	spin_unlock_irq(&block->queue_lock);

	if (waitqueue_active(&shutdown_waitq))
		wake_up(&shutdown_waitq);
	dasd_put_device(block->base);
}

static void _dasd_wake_block_flush_cb(struct dasd_ccw_req *cqr, void *data)
{
	wake_up(&dasd_flush_wq);
}

/*
 * Requeue a request back to the block request queue
 * only works for block requests
 */
static int _dasd_requeue_request(struct dasd_ccw_req *cqr)
{
	struct dasd_block *block = cqr->block;
	struct request *req;

	if (!block)
		return -EINVAL;
	/*
	 * If the request is an ERP request there is nothing to requeue.
	 * This will be done with the remaining original request.
	 */
	if (cqr->refers)
		return 0;
	spin_lock_irq(&cqr->dq->lock);
	req = (struct request *) cqr->callback_data;
	blk_mq_requeue_request(req, false);
	spin_unlock_irq(&cqr->dq->lock);

	return 0;
}

/*
 * Go through all request on the dasd_block request queue, cancel them
 * on the respective dasd_device, and return them to the generic
 * block layer.
 */
static int dasd_flush_block_queue(struct dasd_block *block)
{
	struct dasd_ccw_req *cqr, *n;
	int rc, i;
	struct list_head flush_queue;
	unsigned long flags;

	INIT_LIST_HEAD(&flush_queue);
	spin_lock_bh(&block->queue_lock);
	rc = 0;
restart:
	list_for_each_entry_safe(cqr, n, &block->ccw_queue, blocklist) {
		/* if this request currently owned by a dasd_device cancel it */
		if (cqr->status >= DASD_CQR_QUEUED)
			rc = dasd_cancel_req(cqr);
		if (rc < 0)
			break;
		/* Rechain request (including erp chain) so it won't be
		 * touched by the dasd_block_tasklet anymore.
		 * Replace the callback so we notice when the request
		 * is returned from the dasd_device layer.
		 */
		cqr->callback = _dasd_wake_block_flush_cb;
		for (i = 0; cqr != NULL; cqr = cqr->refers, i++)
			list_move_tail(&cqr->blocklist, &flush_queue);
		if (i > 1)
			/* moved more than one request - need to restart */
			goto restart;
	}
	spin_unlock_bh(&block->queue_lock);
	/* Now call the callback function of flushed requests */
restart_cb:
	list_for_each_entry_safe(cqr, n, &flush_queue, blocklist) {
		wait_event(dasd_flush_wq, (cqr->status < DASD_CQR_QUEUED));
		/* Process finished ERP request. */
		if (cqr->refers) {
			spin_lock_bh(&block->queue_lock);
			__dasd_process_erp(block->base, cqr);
			spin_unlock_bh(&block->queue_lock);
			/* restart list_for_xx loop since dasd_process_erp
			 * might remove multiple elements */
			goto restart_cb;
		}
		/* call the callback function */
		spin_lock_irqsave(&cqr->dq->lock, flags);
		cqr->endclk = get_tod_clock();
		list_del_init(&cqr->blocklist);
		__dasd_cleanup_cqr(cqr);
		spin_unlock_irqrestore(&cqr->dq->lock, flags);
	}
	return rc;
}

/*
 * Schedules a call to dasd_tasklet over the device tasklet.
 */
void dasd_schedule_block_bh(struct dasd_block *block)
{
	/* Protect against rescheduling. */
	if (atomic_cmpxchg(&block->tasklet_scheduled, 0, 1) != 0)
		return;
	/* life cycle of block is bound to it's base device */
	dasd_get_device(block->base);
	tasklet_hi_schedule(&block->tasklet);
}
EXPORT_SYMBOL(dasd_schedule_block_bh);


/*
 * SECTION: external block device operations
 * (request queue handling, open, release, etc.)
 */

/*
 * Dasd request queue function. Called from ll_rw_blk.c
 */
static blk_status_t do_dasd_request(struct blk_mq_hw_ctx *hctx,
				    const struct blk_mq_queue_data *qd)
{
	struct dasd_block *block = hctx->queue->queuedata;
	struct dasd_queue *dq = hctx->driver_data;
	struct request *req = qd->rq;
	struct dasd_device *basedev;
	struct dasd_ccw_req *cqr;
	blk_status_t rc = BLK_STS_OK;

	basedev = block->base;
	spin_lock_irq(&dq->lock);
	if (basedev->state < DASD_STATE_READY ||
	    test_bit(DASD_FLAG_OFFLINE, &basedev->flags)) {
		DBF_DEV_EVENT(DBF_ERR, basedev,
			      "device not ready for request %p", req);
		rc = BLK_STS_IOERR;
		goto out;
	}

	/*
	 * if device is stopped do not fetch new requests
	 * except failfast is active which will let requests fail
	 * immediately in __dasd_block_start_head()
	 */
	if (basedev->stopped && !(basedev->features & DASD_FEATURE_FAILFAST)) {
		DBF_DEV_EVENT(DBF_ERR, basedev,
			      "device stopped request %p", req);
		rc = BLK_STS_RESOURCE;
		goto out;
	}

	if (basedev->features & DASD_FEATURE_READONLY &&
	    rq_data_dir(req) == WRITE) {
		DBF_DEV_EVENT(DBF_ERR, basedev,
			      "Rejecting write request %p", req);
		rc = BLK_STS_IOERR;
		goto out;
	}

	if (test_bit(DASD_FLAG_ABORTALL, &basedev->flags) &&
	    (basedev->features & DASD_FEATURE_FAILFAST ||
	     blk_noretry_request(req))) {
		DBF_DEV_EVENT(DBF_ERR, basedev,
			      "Rejecting failfast request %p", req);
		rc = BLK_STS_IOERR;
		goto out;
	}

	cqr = basedev->discipline->build_cp(basedev, block, req);
	if (IS_ERR(cqr)) {
		if (PTR_ERR(cqr) == -EBUSY ||
		    PTR_ERR(cqr) == -ENOMEM ||
		    PTR_ERR(cqr) == -EAGAIN) {
			rc = BLK_STS_RESOURCE;
			goto out;
		}
		DBF_DEV_EVENT(DBF_ERR, basedev,
			      "CCW creation failed (rc=%ld) on request %p",
			      PTR_ERR(cqr), req);
		rc = BLK_STS_IOERR;
		goto out;
	}
	/*
	 *  Note: callback is set to dasd_return_cqr_cb in
	 * __dasd_block_start_head to cover erp requests as well
	 */
	cqr->callback_data = req;
	cqr->status = DASD_CQR_FILLED;
	cqr->dq = dq;

	blk_mq_start_request(req);
	spin_lock(&block->queue_lock);
	list_add_tail(&cqr->blocklist, &block->ccw_queue);
	INIT_LIST_HEAD(&cqr->devlist);
	dasd_profile_start(block, cqr, req);
	dasd_schedule_block_bh(block);
	spin_unlock(&block->queue_lock);

out:
	spin_unlock_irq(&dq->lock);
	return rc;
}

/*
 * Block timeout callback, called from the block layer
 *
 * Return values:
 * BLK_EH_RESET_TIMER if the request should be left running
 * BLK_EH_DONE if the request is handled or terminated
 *		      by the driver.
 */
enum blk_eh_timer_return dasd_times_out(struct request *req, bool reserved)
{
	struct dasd_block *block = req->q->queuedata;
	struct dasd_device *device;
	struct dasd_ccw_req *cqr;
	unsigned long flags;
	int rc = 0;

	cqr = blk_mq_rq_to_pdu(req);
	if (!cqr)
		return BLK_EH_DONE;

	spin_lock_irqsave(&cqr->dq->lock, flags);
	device = cqr->startdev ? cqr->startdev : block->base;
	if (!device->blk_timeout) {
		spin_unlock_irqrestore(&cqr->dq->lock, flags);
		return BLK_EH_RESET_TIMER;
	}
	DBF_DEV_EVENT(DBF_WARNING, device,
		      " dasd_times_out cqr %p status %x",
		      cqr, cqr->status);

	spin_lock(&block->queue_lock);
	spin_lock(get_ccwdev_lock(device->cdev));
	cqr->retries = -1;
	cqr->intrc = -ETIMEDOUT;
	if (cqr->status >= DASD_CQR_QUEUED) {
		rc = __dasd_cancel_req(cqr);
	} else if (cqr->status == DASD_CQR_FILLED ||
		   cqr->status == DASD_CQR_NEED_ERP) {
		cqr->status = DASD_CQR_TERMINATED;
	} else if (cqr->status == DASD_CQR_IN_ERP) {
		struct dasd_ccw_req *searchcqr, *nextcqr, *tmpcqr;

		list_for_each_entry_safe(searchcqr, nextcqr,
					 &block->ccw_queue, blocklist) {
			tmpcqr = searchcqr;
			while (tmpcqr->refers)
				tmpcqr = tmpcqr->refers;
			if (tmpcqr != cqr)
				continue;
			/* searchcqr is an ERP request for cqr */
			searchcqr->retries = -1;
			searchcqr->intrc = -ETIMEDOUT;
			if (searchcqr->status >= DASD_CQR_QUEUED) {
				rc = __dasd_cancel_req(searchcqr);
			} else if ((searchcqr->status == DASD_CQR_FILLED) ||
				   (searchcqr->status == DASD_CQR_NEED_ERP)) {
				searchcqr->status = DASD_CQR_TERMINATED;
				rc = 0;
			} else if (searchcqr->status == DASD_CQR_IN_ERP) {
				/*
				 * Shouldn't happen; most recent ERP
				 * request is at the front of queue
				 */
				continue;
			}
			break;
		}
	}
	spin_unlock(get_ccwdev_lock(device->cdev));
	dasd_schedule_block_bh(block);
	spin_unlock(&block->queue_lock);
	spin_unlock_irqrestore(&cqr->dq->lock, flags);

	return rc ? BLK_EH_RESET_TIMER : BLK_EH_DONE;
}

static int dasd_init_hctx(struct blk_mq_hw_ctx *hctx, void *data,
			  unsigned int idx)
{
	struct dasd_queue *dq = kzalloc(sizeof(*dq), GFP_KERNEL);

	if (!dq)
		return -ENOMEM;

	spin_lock_init(&dq->lock);
	hctx->driver_data = dq;

	return 0;
}

static void dasd_exit_hctx(struct blk_mq_hw_ctx *hctx, unsigned int idx)
{
	kfree(hctx->driver_data);
	hctx->driver_data = NULL;
}

static void dasd_request_done(struct request *req)
{
	blk_mq_end_request(req, 0);
	blk_mq_run_hw_queues(req->q, true);
}

static struct blk_mq_ops dasd_mq_ops = {
	.queue_rq = do_dasd_request,
	.complete = dasd_request_done,
	.timeout = dasd_times_out,
	.init_hctx = dasd_init_hctx,
	.exit_hctx = dasd_exit_hctx,
};

/*
 * Allocate and initialize request queue and default I/O scheduler.
 */
static int dasd_alloc_queue(struct dasd_block *block)
{
	int rc;

	block->tag_set.ops = &dasd_mq_ops;
	block->tag_set.cmd_size = sizeof(struct dasd_ccw_req);
	block->tag_set.nr_hw_queues = nr_hw_queues;
	block->tag_set.queue_depth = queue_depth;
	block->tag_set.flags = BLK_MQ_F_SHOULD_MERGE;
	block->tag_set.numa_node = NUMA_NO_NODE;

	rc = blk_mq_alloc_tag_set(&block->tag_set);
	if (rc)
		return rc;

	block->request_queue = blk_mq_init_queue(&block->tag_set);
	if (IS_ERR(block->request_queue))
		return PTR_ERR(block->request_queue);

	block->request_queue->queuedata = block;

	return 0;
}

/*
 * Deactivate and free request queue.
 */
static void dasd_free_queue(struct dasd_block *block)
{
	if (block->request_queue) {
		blk_cleanup_queue(block->request_queue);
		blk_mq_free_tag_set(&block->tag_set);
		block->request_queue = NULL;
	}
}

static int dasd_open(struct block_device *bdev, fmode_t mode)
{
	struct dasd_device *base;
	int rc;

	base = dasd_device_from_gendisk(bdev->bd_disk);
	if (!base)
		return -ENODEV;

	atomic_inc(&base->block->open_count);
	if (test_bit(DASD_FLAG_OFFLINE, &base->flags)) {
		rc = -ENODEV;
		goto unlock;
	}

	if (!try_module_get(base->discipline->owner)) {
		rc = -EINVAL;
		goto unlock;
	}

	if (dasd_probeonly) {
		dev_info(&base->cdev->dev,
			 "Accessing the DASD failed because it is in "
			 "probeonly mode\n");
		rc = -EPERM;
		goto out;
	}

	if (base->state <= DASD_STATE_BASIC) {
		DBF_DEV_EVENT(DBF_ERR, base, " %s",
			      " Cannot open unrecognized device");
		rc = -ENODEV;
		goto out;
	}

	if ((mode & FMODE_WRITE) &&
	    (test_bit(DASD_FLAG_DEVICE_RO, &base->flags) ||
	     (base->features & DASD_FEATURE_READONLY))) {
		rc = -EROFS;
		goto out;
	}

	dasd_put_device(base);
	return 0;

out:
	module_put(base->discipline->owner);
unlock:
	atomic_dec(&base->block->open_count);
	dasd_put_device(base);
	return rc;
}

static void dasd_release(struct gendisk *disk, fmode_t mode)
{
	struct dasd_device *base = dasd_device_from_gendisk(disk);
	if (base) {
		atomic_dec(&base->block->open_count);
		module_put(base->discipline->owner);
		dasd_put_device(base);
	}
}

/*
 * Return disk geometry.
 */
static int dasd_getgeo(struct block_device *bdev, struct hd_geometry *geo)
{
	struct dasd_device *base;

	base = dasd_device_from_gendisk(bdev->bd_disk);
	if (!base)
		return -ENODEV;

	if (!base->discipline ||
	    !base->discipline->fill_geometry) {
		dasd_put_device(base);
		return -EINVAL;
	}
	base->discipline->fill_geometry(base->block, geo);
	geo->start = get_start_sect(bdev) >> base->block->s2b_shift;
	dasd_put_device(base);
	return 0;
}

const struct block_device_operations
dasd_device_operations = {
	.owner		= THIS_MODULE,
	.open		= dasd_open,
	.release	= dasd_release,
	.ioctl		= dasd_ioctl,
	.compat_ioctl	= dasd_ioctl,
	.getgeo		= dasd_getgeo,
	.set_read_only	= dasd_set_read_only,
};

/*******************************************************************************
 * end of block device operations
 */

static void
dasd_exit(void)
{
#ifdef CONFIG_PROC_FS
	dasd_proc_exit();
#endif
	dasd_eer_exit();
	kmem_cache_destroy(dasd_page_cache);
	dasd_page_cache = NULL;
	dasd_gendisk_exit();
	dasd_devmap_exit();
	if (dasd_debug_area != NULL) {
		debug_unregister(dasd_debug_area);
		dasd_debug_area = NULL;
	}
	dasd_statistics_removeroot();
}

/*
 * SECTION: common functions for ccw_driver use
 */

/*
 * Is the device read-only?
 * Note that this function does not report the setting of the
 * readonly device attribute, but how it is configured in z/VM.
 */
int dasd_device_is_ro(struct dasd_device *device)
{
	struct ccw_dev_id dev_id;
	struct diag210 diag_data;
	int rc;

	if (!MACHINE_IS_VM)
		return 0;
	ccw_device_get_id(device->cdev, &dev_id);
	memset(&diag_data, 0, sizeof(diag_data));
	diag_data.vrdcdvno = dev_id.devno;
	diag_data.vrdclen = sizeof(diag_data);
	rc = diag210(&diag_data);
	if (rc == 0 || rc == 2) {
		return diag_data.vrdcvfla & 0x80;
	} else {
		DBF_EVENT(DBF_WARNING, "diag210 failed for dev=%04x with rc=%d",
			  dev_id.devno, rc);
		return 0;
	}
}
EXPORT_SYMBOL_GPL(dasd_device_is_ro);

static void dasd_generic_auto_online(void *data, async_cookie_t cookie)
{
	struct ccw_device *cdev = data;
	int ret;

	ret = ccw_device_set_online(cdev);
	if (ret)
		pr_warn("%s: Setting the DASD online failed with rc=%d\n",
			dev_name(&cdev->dev), ret);
}

/*
 * Initial attempt at a probe function. this can be simplified once
 * the other detection code is gone.
 */
int dasd_generic_probe(struct ccw_device *cdev)
{
	cdev->handler = &dasd_int_handler;

	/*
	 * Automatically online either all dasd devices (dasd_autodetect)
	 * or all devices specified with dasd= parameters during
	 * initial probe.
	 */
	if ((dasd_get_feature(cdev, DASD_FEATURE_INITIAL_ONLINE) > 0 ) ||
	    (dasd_autodetect && dasd_busid_known(dev_name(&cdev->dev)) != 0))
		async_schedule(dasd_generic_auto_online, cdev);
	return 0;
}
EXPORT_SYMBOL_GPL(dasd_generic_probe);

void dasd_generic_free_discipline(struct dasd_device *device)
{
	/* Forget the discipline information. */
	if (device->discipline) {
		if (device->discipline->uncheck_device)
			device->discipline->uncheck_device(device);
		module_put(device->discipline->owner);
		device->discipline = NULL;
	}
	if (device->base_discipline) {
		module_put(device->base_discipline->owner);
		device->base_discipline = NULL;
	}
}
EXPORT_SYMBOL_GPL(dasd_generic_free_discipline);

/*
 * This will one day be called from a global not_oper handler.
 * It is also used by driver_unregister during module unload.
 */
void dasd_generic_remove(struct ccw_device *cdev)
{
	struct dasd_device *device;
	struct dasd_block *block;

	device = dasd_device_from_cdev(cdev);
	if (IS_ERR(device))
		return;

	if (test_and_set_bit(DASD_FLAG_OFFLINE, &device->flags) &&
	    !test_bit(DASD_FLAG_SAFE_OFFLINE_RUNNING, &device->flags)) {
		/* Already doing offline processing */
		dasd_put_device(device);
		return;
	}
	/*
	 * This device is removed unconditionally. Set offline
	 * flag to prevent dasd_open from opening it while it is
	 * no quite down yet.
	 */
	dasd_set_target_state(device, DASD_STATE_NEW);
	cdev->handler = NULL;
	/* dasd_delete_device destroys the device reference. */
	block = device->block;
	dasd_delete_device(device);
	/*
	 * life cycle of block is bound to device, so delete it after
	 * device was safely removed
	 */
	if (block)
		dasd_free_block(block);
}
EXPORT_SYMBOL_GPL(dasd_generic_remove);

/*
 * Activate a device. This is called from dasd_{eckd,fba}_probe() when either
 * the device is detected for the first time and is supposed to be used
 * or the user has started activation through sysfs.
 */
int dasd_generic_set_online(struct ccw_device *cdev,
			    struct dasd_discipline *base_discipline)
{
	struct dasd_discipline *discipline;
	struct dasd_device *device;
	int rc;

	/* first online clears initial online feature flag */
	dasd_set_feature(cdev, DASD_FEATURE_INITIAL_ONLINE, 0);
	device = dasd_create_device(cdev);
	if (IS_ERR(device))
		return PTR_ERR(device);

	discipline = base_discipline;
	if (device->features & DASD_FEATURE_USEDIAG) {
	  	if (!dasd_diag_discipline_pointer) {
			/* Try to load the required module. */
			rc = request_module(DASD_DIAG_MOD);
			if (rc) {
				pr_warn("%s Setting the DASD online failed "
					"because the required module %s "
					"could not be loaded (rc=%d)\n",
					dev_name(&cdev->dev), DASD_DIAG_MOD,
					rc);
				dasd_delete_device(device);
				return -ENODEV;
			}
		}
		/* Module init could have failed, so check again here after
		 * request_module(). */
		if (!dasd_diag_discipline_pointer) {
			pr_warn("%s Setting the DASD online failed because of missing DIAG discipline\n",
				dev_name(&cdev->dev));
			dasd_delete_device(device);
			return -ENODEV;
		}
		discipline = dasd_diag_discipline_pointer;
	}
	if (!try_module_get(base_discipline->owner)) {
		dasd_delete_device(device);
		return -EINVAL;
	}
	if (!try_module_get(discipline->owner)) {
		module_put(base_discipline->owner);
		dasd_delete_device(device);
		return -EINVAL;
	}
	device->base_discipline = base_discipline;
	device->discipline = discipline;

	/* check_device will allocate block device if necessary */
	rc = discipline->check_device(device);
	if (rc) {
		pr_warn("%s Setting the DASD online with discipline %s failed with rc=%i\n",
			dev_name(&cdev->dev), discipline->name, rc);
		module_put(discipline->owner);
		module_put(base_discipline->owner);
		dasd_delete_device(device);
		return rc;
	}

	dasd_set_target_state(device, DASD_STATE_ONLINE);
	if (device->state <= DASD_STATE_KNOWN) {
		pr_warn("%s Setting the DASD online failed because of a missing discipline\n",
			dev_name(&cdev->dev));
		rc = -ENODEV;
		dasd_set_target_state(device, DASD_STATE_NEW);
		if (device->block)
			dasd_free_block(device->block);
		dasd_delete_device(device);
	} else
		pr_debug("dasd_generic device %s found\n",
				dev_name(&cdev->dev));

	wait_event(dasd_init_waitq, _wait_for_device(device));

	dasd_put_device(device);
	return rc;
}
EXPORT_SYMBOL_GPL(dasd_generic_set_online);

int dasd_generic_set_offline(struct ccw_device *cdev)
{
	struct dasd_device *device;
	struct dasd_block *block;
	int max_count, open_count, rc;
	unsigned long flags;

	rc = 0;
	spin_lock_irqsave(get_ccwdev_lock(cdev), flags);
	device = dasd_device_from_cdev_locked(cdev);
	if (IS_ERR(device)) {
		spin_unlock_irqrestore(get_ccwdev_lock(cdev), flags);
		return PTR_ERR(device);
	}

	/*
	 * We must make sure that this device is currently not in use.
	 * The open_count is increased for every opener, that includes
	 * the blkdev_get in dasd_scan_partitions. We are only interested
	 * in the other openers.
	 */
	if (device->block) {
		max_count = device->block->bdev ? 0 : -1;
		open_count = atomic_read(&device->block->open_count);
		if (open_count > max_count) {
			if (open_count > 0)
				pr_warn("%s: The DASD cannot be set offline with open count %i\n",
					dev_name(&cdev->dev), open_count);
			else
				pr_warn("%s: The DASD cannot be set offline while it is in use\n",
					dev_name(&cdev->dev));
			rc = -EBUSY;
			goto out_err;
		}
	}

	/*
	 * Test if the offline processing is already running and exit if so.
	 * If a safe offline is being processed this could only be a normal
	 * offline that should be able to overtake the safe offline and
	 * cancel any I/O we do not want to wait for any longer
	 */
	if (test_bit(DASD_FLAG_OFFLINE, &device->flags)) {
		if (test_bit(DASD_FLAG_SAFE_OFFLINE_RUNNING, &device->flags)) {
			clear_bit(DASD_FLAG_SAFE_OFFLINE_RUNNING,
				  &device->flags);
		} else {
			rc = -EBUSY;
			goto out_err;
		}
	}
	set_bit(DASD_FLAG_OFFLINE, &device->flags);

	/*
	 * if safe_offline is called set safe_offline_running flag and
	 * clear safe_offline so that a call to normal offline
	 * can overrun safe_offline processing
	 */
	if (test_and_clear_bit(DASD_FLAG_SAFE_OFFLINE, &device->flags) &&
	    !test_and_set_bit(DASD_FLAG_SAFE_OFFLINE_RUNNING, &device->flags)) {
		/* need to unlock here to wait for outstanding I/O */
		spin_unlock_irqrestore(get_ccwdev_lock(cdev), flags);
		/*
		 * If we want to set the device safe offline all IO operations
		 * should be finished before continuing the offline process
		 * so sync bdev first and then wait for our queues to become
		 * empty
		 */
		if (device->block) {
			rc = fsync_bdev(device->block->bdev);
			if (rc != 0)
				goto interrupted;
		}
		dasd_schedule_device_bh(device);
		rc = wait_event_interruptible(shutdown_waitq,
					      _wait_for_empty_queues(device));
		if (rc != 0)
			goto interrupted;

		/*
		 * check if a normal offline process overtook the offline
		 * processing in this case simply do nothing beside returning
		 * that we got interrupted
		 * otherwise mark safe offline as not running any longer and
		 * continue with normal offline
		 */
		spin_lock_irqsave(get_ccwdev_lock(cdev), flags);
		if (!test_bit(DASD_FLAG_SAFE_OFFLINE_RUNNING, &device->flags)) {
			rc = -ERESTARTSYS;
			goto out_err;
		}
		clear_bit(DASD_FLAG_SAFE_OFFLINE_RUNNING, &device->flags);
	}
	spin_unlock_irqrestore(get_ccwdev_lock(cdev), flags);

	dasd_set_target_state(device, DASD_STATE_NEW);
	/* dasd_delete_device destroys the device reference. */
	block = device->block;
	dasd_delete_device(device);
	/*
	 * life cycle of block is bound to device, so delete it after
	 * device was safely removed
	 */
	if (block)
		dasd_free_block(block);

	return 0;

interrupted:
	/* interrupted by signal */
	spin_lock_irqsave(get_ccwdev_lock(cdev), flags);
	clear_bit(DASD_FLAG_SAFE_OFFLINE_RUNNING, &device->flags);
	clear_bit(DASD_FLAG_OFFLINE, &device->flags);
out_err:
	dasd_put_device(device);
	spin_unlock_irqrestore(get_ccwdev_lock(cdev), flags);
	return rc;
}
EXPORT_SYMBOL_GPL(dasd_generic_set_offline);

int dasd_generic_last_path_gone(struct dasd_device *device)
{
	struct dasd_ccw_req *cqr;

	dev_warn(&device->cdev->dev, "No operational channel path is left "
		 "for the device\n");
	DBF_DEV_EVENT(DBF_WARNING, device, "%s", "last path gone");
	/* First of all call extended error reporting. */
	dasd_eer_write(device, NULL, DASD_EER_NOPATH);

	if (device->state < DASD_STATE_BASIC)
		return 0;
	/* Device is active. We want to keep it. */
	list_for_each_entry(cqr, &device->ccw_queue, devlist)
		if ((cqr->status == DASD_CQR_IN_IO) ||
		    (cqr->status == DASD_CQR_CLEAR_PENDING)) {
			cqr->status = DASD_CQR_QUEUED;
			cqr->retries++;
		}
	dasd_device_set_stop_bits(device, DASD_STOPPED_DC_WAIT);
	dasd_device_clear_timer(device);
	dasd_schedule_device_bh(device);
	return 1;
}
EXPORT_SYMBOL_GPL(dasd_generic_last_path_gone);

int dasd_generic_path_operational(struct dasd_device *device)
{
	dev_info(&device->cdev->dev, "A channel path to the device has become "
		 "operational\n");
	DBF_DEV_EVENT(DBF_WARNING, device, "%s", "path operational");
	dasd_device_remove_stop_bits(device, DASD_STOPPED_DC_WAIT);
	dasd_schedule_device_bh(device);
	if (device->block) {
		dasd_schedule_block_bh(device->block);
		if (device->block->request_queue)
			blk_mq_run_hw_queues(device->block->request_queue,
					     true);
		}

	if (!device->stopped)
		wake_up(&generic_waitq);

	return 1;
}
EXPORT_SYMBOL_GPL(dasd_generic_path_operational);

int dasd_generic_notify(struct ccw_device *cdev, int event)
{
	struct dasd_device *device;
	int ret;

	device = dasd_device_from_cdev_locked(cdev);
	if (IS_ERR(device))
		return 0;
	ret = 0;
	switch (event) {
	case CIO_GONE:
	case CIO_BOXED:
	case CIO_NO_PATH:
		dasd_path_no_path(device);
		ret = dasd_generic_last_path_gone(device);
		break;
	case CIO_OPER:
		ret = 1;
		if (dasd_path_get_opm(device))
			ret = dasd_generic_path_operational(device);
		break;
	}
	dasd_put_device(device);
	return ret;
}
EXPORT_SYMBOL_GPL(dasd_generic_notify);

void dasd_generic_path_event(struct ccw_device *cdev, int *path_event)
{
	struct dasd_device *device;
	int chp, oldopm, hpfpm, ifccpm;

	device = dasd_device_from_cdev_locked(cdev);
	if (IS_ERR(device))
		return;

	oldopm = dasd_path_get_opm(device);
	for (chp = 0; chp < 8; chp++) {
		if (path_event[chp] & PE_PATH_GONE) {
			dasd_path_notoper(device, chp);
		}
		if (path_event[chp] & PE_PATH_AVAILABLE) {
			dasd_path_available(device, chp);
			dasd_schedule_device_bh(device);
		}
		if (path_event[chp] & PE_PATHGROUP_ESTABLISHED) {
			if (!dasd_path_is_operational(device, chp) &&
			    !dasd_path_need_verify(device, chp)) {
				/*
				 * we can not establish a pathgroup on an
				 * unavailable path, so trigger a path
				 * verification first
				 */
			dasd_path_available(device, chp);
			dasd_schedule_device_bh(device);
			}
			DBF_DEV_EVENT(DBF_WARNING, device, "%s",
				      "Pathgroup re-established\n");
			if (device->discipline->kick_validate)
				device->discipline->kick_validate(device);
		}
		if (path_event[chp] & PE_PATH_FCES_EVENT) {
			dasd_path_fcsec_update(device, chp);
			dasd_schedule_device_bh(device);
		}
	}
	hpfpm = dasd_path_get_hpfpm(device);
	ifccpm = dasd_path_get_ifccpm(device);
	if (!dasd_path_get_opm(device) && hpfpm) {
		/*
		 * device has no operational paths but at least one path is
		 * disabled due to HPF errors
		 * disable HPF at all and use the path(s) again
		 */
		if (device->discipline->disable_hpf)
			device->discipline->disable_hpf(device);
		dasd_device_set_stop_bits(device, DASD_STOPPED_NOT_ACC);
		dasd_path_set_tbvpm(device, hpfpm);
		dasd_schedule_device_bh(device);
		dasd_schedule_requeue(device);
	} else if (!dasd_path_get_opm(device) && ifccpm) {
		/*
		 * device has no operational paths but at least one path is
		 * disabled due to IFCC errors
		 * trigger path verification on paths with IFCC errors
		 */
		dasd_path_set_tbvpm(device, ifccpm);
		dasd_schedule_device_bh(device);
	}
	if (oldopm && !dasd_path_get_opm(device) && !hpfpm && !ifccpm) {
		dev_warn(&device->cdev->dev,
			 "No verified channel paths remain for the device\n");
		DBF_DEV_EVENT(DBF_WARNING, device,
			      "%s", "last verified path gone");
		dasd_eer_write(device, NULL, DASD_EER_NOPATH);
		dasd_device_set_stop_bits(device,
					  DASD_STOPPED_DC_WAIT);
	}
	dasd_put_device(device);
}
EXPORT_SYMBOL_GPL(dasd_generic_path_event);

int dasd_generic_verify_path(struct dasd_device *device, __u8 lpm)
{
	if (!dasd_path_get_opm(device) && lpm) {
		dasd_path_set_opm(device, lpm);
		dasd_generic_path_operational(device);
	} else
		dasd_path_add_opm(device, lpm);
	return 0;
}
EXPORT_SYMBOL_GPL(dasd_generic_verify_path);

void dasd_generic_space_exhaust(struct dasd_device *device,
				struct dasd_ccw_req *cqr)
{
	dasd_eer_write(device, NULL, DASD_EER_NOSPC);

	if (device->state < DASD_STATE_BASIC)
		return;

	if (cqr->status == DASD_CQR_IN_IO ||
	    cqr->status == DASD_CQR_CLEAR_PENDING) {
		cqr->status = DASD_CQR_QUEUED;
		cqr->retries++;
	}
	dasd_device_set_stop_bits(device, DASD_STOPPED_NOSPC);
	dasd_device_clear_timer(device);
	dasd_schedule_device_bh(device);
}
EXPORT_SYMBOL_GPL(dasd_generic_space_exhaust);

void dasd_generic_space_avail(struct dasd_device *device)
{
	dev_info(&device->cdev->dev, "Extent pool space is available\n");
	DBF_DEV_EVENT(DBF_WARNING, device, "%s", "space available");

	dasd_device_remove_stop_bits(device, DASD_STOPPED_NOSPC);
	dasd_schedule_device_bh(device);

	if (device->block) {
		dasd_schedule_block_bh(device->block);
		if (device->block->request_queue)
			blk_mq_run_hw_queues(device->block->request_queue, true);
	}
	if (!device->stopped)
		wake_up(&generic_waitq);
}
EXPORT_SYMBOL_GPL(dasd_generic_space_avail);

/*
 * clear active requests and requeue them to block layer if possible
 */
static int dasd_generic_requeue_all_requests(struct dasd_device *device)
{
	struct list_head requeue_queue;
	struct dasd_ccw_req *cqr, *n;
	struct dasd_ccw_req *refers;
	int rc;

	INIT_LIST_HEAD(&requeue_queue);
	spin_lock_irq(get_ccwdev_lock(device->cdev));
	rc = 0;
	list_for_each_entry_safe(cqr, n, &device->ccw_queue, devlist) {
		/* Check status and move request to flush_queue */
		if (cqr->status == DASD_CQR_IN_IO) {
			rc = device->discipline->term_IO(cqr);
			if (rc) {
				/* unable to terminate requeust */
				dev_err(&device->cdev->dev,
					"Unable to terminate request %p "
					"on suspend\n", cqr);
				spin_unlock_irq(get_ccwdev_lock(device->cdev));
				dasd_put_device(device);
				return rc;
			}
		}
		list_move_tail(&cqr->devlist, &requeue_queue);
	}
	spin_unlock_irq(get_ccwdev_lock(device->cdev));

	list_for_each_entry_safe(cqr, n, &requeue_queue, devlist) {
		wait_event(dasd_flush_wq,
			   (cqr->status != DASD_CQR_CLEAR_PENDING));

		/*
		 * requeue requests to blocklayer will only work
		 * for block device requests
		 */
		if (_dasd_requeue_request(cqr))
			continue;

		/* remove requests from device and block queue */
		list_del_init(&cqr->devlist);
		while (cqr->refers != NULL) {
			refers = cqr->refers;
			/* remove the request from the block queue */
			list_del(&cqr->blocklist);
			/* free the finished erp request */
			dasd_free_erp_request(cqr, cqr->memdev);
			cqr = refers;
		}

		/*
		 * _dasd_requeue_request already checked for a valid
		 * blockdevice, no need to check again
		 * all erp requests (cqr->refers) have a cqr->block
		 * pointer copy from the original cqr
		 */
		list_del_init(&cqr->blocklist);
		cqr->block->base->discipline->free_cp(
			cqr, (struct request *) cqr->callback_data);
	}

	/*
	 * if requests remain then they are internal request
	 * and go back to the device queue
	 */
	if (!list_empty(&requeue_queue)) {
		/* move freeze_queue to start of the ccw_queue */
		spin_lock_irq(get_ccwdev_lock(device->cdev));
		list_splice_tail(&requeue_queue, &device->ccw_queue);
		spin_unlock_irq(get_ccwdev_lock(device->cdev));
	}
	dasd_schedule_device_bh(device);
	return rc;
}

static void do_requeue_requests(struct work_struct *work)
{
	struct dasd_device *device = container_of(work, struct dasd_device,
						  requeue_requests);
	dasd_generic_requeue_all_requests(device);
	dasd_device_remove_stop_bits(device, DASD_STOPPED_NOT_ACC);
	if (device->block)
		dasd_schedule_block_bh(device->block);
	dasd_put_device(device);
}

void dasd_schedule_requeue(struct dasd_device *device)
{
	dasd_get_device(device);
	/* queue call to dasd_reload_device to the kernel event daemon. */
	if (!schedule_work(&device->requeue_requests))
		dasd_put_device(device);
}
EXPORT_SYMBOL(dasd_schedule_requeue);

static struct dasd_ccw_req *dasd_generic_build_rdc(struct dasd_device *device,
						   int rdc_buffer_size,
						   int magic)
{
	struct dasd_ccw_req *cqr;
	struct ccw1 *ccw;

	cqr = dasd_smalloc_request(magic, 1 /* RDC */, rdc_buffer_size, device,
				   NULL);

	if (IS_ERR(cqr)) {
		/* internal error 13 - Allocating the RDC request failed*/
		dev_err(&device->cdev->dev,
			 "An error occurred in the DASD device driver, "
			 "reason=%s\n", "13");
		return cqr;
	}

	ccw = cqr->cpaddr;
	ccw->cmd_code = CCW_CMD_RDC;
	ccw->cda = (__u32)(addr_t) cqr->data;
	ccw->flags = 0;
	ccw->count = rdc_buffer_size;
	cqr->startdev = device;
	cqr->memdev = device;
	cqr->expires = 10*HZ;
	cqr->retries = 256;
	cqr->buildclk = get_tod_clock();
	cqr->status = DASD_CQR_FILLED;
	return cqr;
}


int dasd_generic_read_dev_chars(struct dasd_device *device, int magic,
				void *rdc_buffer, int rdc_buffer_size)
{
	int ret;
	struct dasd_ccw_req *cqr;

	cqr = dasd_generic_build_rdc(device, rdc_buffer_size, magic);
	if (IS_ERR(cqr))
		return PTR_ERR(cqr);

	ret = dasd_sleep_on(cqr);
	if (ret == 0)
		memcpy(rdc_buffer, cqr->data, rdc_buffer_size);
	dasd_sfree_request(cqr, cqr->memdev);
	return ret;
}
EXPORT_SYMBOL_GPL(dasd_generic_read_dev_chars);

/*
 *   In command mode and transport mode we need to look for sense
 *   data in different places. The sense data itself is allways
 *   an array of 32 bytes, so we can unify the sense data access
 *   for both modes.
 */
char *dasd_get_sense(struct irb *irb)
{
	struct tsb *tsb = NULL;
	char *sense = NULL;

	if (scsw_is_tm(&irb->scsw) && (irb->scsw.tm.fcxs == 0x01)) {
		if (irb->scsw.tm.tcw)
			tsb = tcw_get_tsb((struct tcw *)(unsigned long)
					  irb->scsw.tm.tcw);
		if (tsb && tsb->length == 64 && tsb->flags)
			switch (tsb->flags & 0x07) {
			case 1:	/* tsa_iostat */
				sense = tsb->tsa.iostat.sense;
				break;
			case 2: /* tsa_ddpc */
				sense = tsb->tsa.ddpc.sense;
				break;
			default:
				/* currently we don't use interrogate data */
				break;
			}
	} else if (irb->esw.esw0.erw.cons) {
		sense = irb->ecw;
	}
	return sense;
}
EXPORT_SYMBOL_GPL(dasd_get_sense);

void dasd_generic_shutdown(struct ccw_device *cdev)
{
	struct dasd_device *device;

	device = dasd_device_from_cdev(cdev);
	if (IS_ERR(device))
		return;

	if (device->block)
		dasd_schedule_block_bh(device->block);

	dasd_schedule_device_bh(device);

	wait_event(shutdown_waitq, _wait_for_empty_queues(device));
}
EXPORT_SYMBOL_GPL(dasd_generic_shutdown);

static int __init dasd_init(void)
{
	int rc;

	init_waitqueue_head(&dasd_init_waitq);
	init_waitqueue_head(&dasd_flush_wq);
	init_waitqueue_head(&generic_waitq);
	init_waitqueue_head(&shutdown_waitq);

	/* register 'common' DASD debug area, used for all DBF_XXX calls */
	dasd_debug_area = debug_register("dasd", 1, 1, 8 * sizeof(long));
	if (dasd_debug_area == NULL) {
		rc = -ENOMEM;
		goto failed;
	}
	debug_register_view(dasd_debug_area, &debug_sprintf_view);
	debug_set_level(dasd_debug_area, DBF_WARNING);

	DBF_EVENT(DBF_EMERG, "%s", "debug area created");

	dasd_diag_discipline_pointer = NULL;

	dasd_statistics_createroot();

	rc = dasd_devmap_init();
	if (rc)
		goto failed;
	rc = dasd_gendisk_init();
	if (rc)
		goto failed;
	rc = dasd_parse();
	if (rc)
		goto failed;
	rc = dasd_eer_init();
	if (rc)
		goto failed;
#ifdef CONFIG_PROC_FS
	rc = dasd_proc_init();
	if (rc)
		goto failed;
#endif

	return 0;
failed:
	pr_info("The DASD device driver could not be initialized\n");
	dasd_exit();
	return rc;
}

module_init(dasd_init);
module_exit(dasd_exit);
