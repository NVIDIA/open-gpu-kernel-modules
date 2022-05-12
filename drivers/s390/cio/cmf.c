// SPDX-License-Identifier: GPL-2.0+
/*
 * Linux on zSeries Channel Measurement Facility support
 *
 * Copyright IBM Corp. 2000, 2006
 *
 * Authors: Arnd Bergmann <arndb@de.ibm.com>
 *	    Cornelia Huck <cornelia.huck@de.ibm.com>
 *
 * original idea from Natarajan Krishnaswami <nkrishna@us.ibm.com>
 */

#define KMSG_COMPONENT "cio"
#define pr_fmt(fmt) KMSG_COMPONENT ": " fmt

#include <linux/memblock.h>
#include <linux/device.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/export.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>
#include <linux/timex.h>	/* get_tod_clock() */

#include <asm/ccwdev.h>
#include <asm/cio.h>
#include <asm/cmb.h>
#include <asm/div64.h>

#include "cio.h"
#include "css.h"
#include "device.h"
#include "ioasm.h"
#include "chsc.h"

/*
 * parameter to enable cmf during boot, possible uses are:
 *  "s390cmf" -- enable cmf and allocate 2 MB of ram so measuring can be
 *               used on any subchannel
 *  "s390cmf=<num>" -- enable cmf and allocate enough memory to measure
 *                     <num> subchannel, where <num> is an integer
 *                     between 1 and 65535, default is 1024
 */
#define ARGSTRING "s390cmf"

/* indices for READCMB */
enum cmb_index {
	avg_utilization = -1,
 /* basic and exended format: */
	cmb_ssch_rsch_count = 0,
	cmb_sample_count,
	cmb_device_connect_time,
	cmb_function_pending_time,
	cmb_device_disconnect_time,
	cmb_control_unit_queuing_time,
	cmb_device_active_only_time,
 /* extended format only: */
	cmb_device_busy_time,
	cmb_initial_command_response_time,
};

/**
 * enum cmb_format - types of supported measurement block formats
 *
 * @CMF_BASIC:      traditional channel measurement blocks supported
 *		    by all machines that we run on
 * @CMF_EXTENDED:   improved format that was introduced with the z990
 *		    machine
 * @CMF_AUTODETECT: default: use extended format when running on a machine
 *		    supporting extended format, otherwise fall back to
 *		    basic format
 */
enum cmb_format {
	CMF_BASIC,
	CMF_EXTENDED,
	CMF_AUTODETECT = -1,
};

/*
 * format - actual format for all measurement blocks
 *
 * The format module parameter can be set to a value of 0 (zero)
 * or 1, indicating basic or extended format as described for
 * enum cmb_format.
 */
static int format = CMF_AUTODETECT;
module_param(format, bint, 0444);

/**
 * struct cmb_operations - functions to use depending on cmb_format
 *
 * Most of these functions operate on a struct ccw_device. There is only
 * one instance of struct cmb_operations because the format of the measurement
 * data is guaranteed to be the same for every ccw_device.
 *
 * @alloc:	allocate memory for a channel measurement block,
 *		either with the help of a special pool or with kmalloc
 * @free:	free memory allocated with @alloc
 * @set:	enable or disable measurement
 * @read:	read a measurement entry at an index
 * @readall:	read a measurement block in a common format
 * @reset:	clear the data in the associated measurement block and
 *		reset its time stamp
 */
struct cmb_operations {
	int  (*alloc)  (struct ccw_device *);
	void (*free)   (struct ccw_device *);
	int  (*set)    (struct ccw_device *, u32);
	u64  (*read)   (struct ccw_device *, int);
	int  (*readall)(struct ccw_device *, struct cmbdata *);
	void (*reset)  (struct ccw_device *);
/* private: */
	struct attribute_group *attr_group;
};
static struct cmb_operations *cmbops;

struct cmb_data {
	void *hw_block;   /* Pointer to block updated by hardware */
	void *last_block; /* Last changed block copied from hardware block */
	int size;	  /* Size of hw_block and last_block */
	unsigned long long last_update;  /* when last_block was updated */
};

/*
 * Our user interface is designed in terms of nanoseconds,
 * while the hardware measures total times in its own
 * unit.
 */
static inline u64 time_to_nsec(u32 value)
{
	return ((u64)value) * 128000ull;
}

/*
 * Users are usually interested in average times,
 * not accumulated time.
 * This also helps us with atomicity problems
 * when reading sinlge values.
 */
static inline u64 time_to_avg_nsec(u32 value, u32 count)
{
	u64 ret;

	/* no samples yet, avoid division by 0 */
	if (count == 0)
		return 0;

	/* value comes in units of 128 µsec */
	ret = time_to_nsec(value);
	do_div(ret, count);

	return ret;
}

#define CMF_OFF 0
#define CMF_ON	2

/*
 * Activate or deactivate the channel monitor. When area is NULL,
 * the monitor is deactivated. The channel monitor needs to
 * be active in order to measure subchannels, which also need
 * to be enabled.
 */
static inline void cmf_activate(void *area, unsigned int onoff)
{
	register void * __gpr2 asm("2");
	register long __gpr1 asm("1");

	__gpr2 = area;
	__gpr1 = onoff;
	/* activate channel measurement */
	asm("schm" : : "d" (__gpr2), "d" (__gpr1) );
}

static int set_schib(struct ccw_device *cdev, u32 mme, int mbfc,
		     unsigned long address)
{
	struct subchannel *sch = to_subchannel(cdev->dev.parent);
	int ret;

	sch->config.mme = mme;
	sch->config.mbfc = mbfc;
	/* address can be either a block address or a block index */
	if (mbfc)
		sch->config.mba = address;
	else
		sch->config.mbi = address;

	ret = cio_commit_config(sch);
	if (!mme && ret == -ENODEV) {
		/*
		 * The task was to disable measurement block updates but
		 * the subchannel is already gone. Report success.
		 */
		ret = 0;
	}
	return ret;
}

struct set_schib_struct {
	u32 mme;
	int mbfc;
	unsigned long address;
	wait_queue_head_t wait;
	int ret;
};

#define CMF_PENDING 1
#define SET_SCHIB_TIMEOUT (10 * HZ)

static int set_schib_wait(struct ccw_device *cdev, u32 mme,
			  int mbfc, unsigned long address)
{
	struct set_schib_struct set_data;
	int ret = -ENODEV;

	spin_lock_irq(cdev->ccwlock);
	if (!cdev->private->cmb)
		goto out;

	ret = set_schib(cdev, mme, mbfc, address);
	if (ret != -EBUSY)
		goto out;

	/* if the device is not online, don't even try again */
	if (cdev->private->state != DEV_STATE_ONLINE)
		goto out;

	init_waitqueue_head(&set_data.wait);
	set_data.mme = mme;
	set_data.mbfc = mbfc;
	set_data.address = address;
	set_data.ret = CMF_PENDING;

	cdev->private->state = DEV_STATE_CMFCHANGE;
	cdev->private->cmb_wait = &set_data;
	spin_unlock_irq(cdev->ccwlock);

	ret = wait_event_interruptible_timeout(set_data.wait,
					       set_data.ret != CMF_PENDING,
					       SET_SCHIB_TIMEOUT);
	spin_lock_irq(cdev->ccwlock);
	if (ret <= 0) {
		if (set_data.ret == CMF_PENDING) {
			set_data.ret = (ret == 0) ? -ETIME : ret;
			if (cdev->private->state == DEV_STATE_CMFCHANGE)
				cdev->private->state = DEV_STATE_ONLINE;
		}
	}
	cdev->private->cmb_wait = NULL;
	ret = set_data.ret;
out:
	spin_unlock_irq(cdev->ccwlock);
	return ret;
}

void retry_set_schib(struct ccw_device *cdev)
{
	struct set_schib_struct *set_data = cdev->private->cmb_wait;

	if (!set_data)
		return;

	set_data->ret = set_schib(cdev, set_data->mme, set_data->mbfc,
				  set_data->address);
	wake_up(&set_data->wait);
}

static int cmf_copy_block(struct ccw_device *cdev)
{
	struct subchannel *sch = to_subchannel(cdev->dev.parent);
	struct cmb_data *cmb_data;
	void *hw_block;

	if (cio_update_schib(sch))
		return -ENODEV;

	if (scsw_fctl(&sch->schib.scsw) & SCSW_FCTL_START_FUNC) {
		/* Don't copy if a start function is in progress. */
		if ((!(scsw_actl(&sch->schib.scsw) & SCSW_ACTL_SUSPENDED)) &&
		    (scsw_actl(&sch->schib.scsw) &
		     (SCSW_ACTL_DEVACT | SCSW_ACTL_SCHACT)) &&
		    (!(scsw_stctl(&sch->schib.scsw) & SCSW_STCTL_SEC_STATUS)))
			return -EBUSY;
	}
	cmb_data = cdev->private->cmb;
	hw_block = cmb_data->hw_block;
	memcpy(cmb_data->last_block, hw_block, cmb_data->size);
	cmb_data->last_update = get_tod_clock();
	return 0;
}

struct copy_block_struct {
	wait_queue_head_t wait;
	int ret;
};

static int cmf_cmb_copy_wait(struct ccw_device *cdev)
{
	struct copy_block_struct copy_block;
	int ret = -ENODEV;

	spin_lock_irq(cdev->ccwlock);
	if (!cdev->private->cmb)
		goto out;

	ret = cmf_copy_block(cdev);
	if (ret != -EBUSY)
		goto out;

	if (cdev->private->state != DEV_STATE_ONLINE)
		goto out;

	init_waitqueue_head(&copy_block.wait);
	copy_block.ret = CMF_PENDING;

	cdev->private->state = DEV_STATE_CMFUPDATE;
	cdev->private->cmb_wait = &copy_block;
	spin_unlock_irq(cdev->ccwlock);

	ret = wait_event_interruptible(copy_block.wait,
				       copy_block.ret != CMF_PENDING);
	spin_lock_irq(cdev->ccwlock);
	if (ret) {
		if (copy_block.ret == CMF_PENDING) {
			copy_block.ret = -ERESTARTSYS;
			if (cdev->private->state == DEV_STATE_CMFUPDATE)
				cdev->private->state = DEV_STATE_ONLINE;
		}
	}
	cdev->private->cmb_wait = NULL;
	ret = copy_block.ret;
out:
	spin_unlock_irq(cdev->ccwlock);
	return ret;
}

void cmf_retry_copy_block(struct ccw_device *cdev)
{
	struct copy_block_struct *copy_block = cdev->private->cmb_wait;

	if (!copy_block)
		return;

	copy_block->ret = cmf_copy_block(cdev);
	wake_up(&copy_block->wait);
}

static void cmf_generic_reset(struct ccw_device *cdev)
{
	struct cmb_data *cmb_data;

	spin_lock_irq(cdev->ccwlock);
	cmb_data = cdev->private->cmb;
	if (cmb_data) {
		memset(cmb_data->last_block, 0, cmb_data->size);
		/*
		 * Need to reset hw block as well to make the hardware start
		 * from 0 again.
		 */
		memset(cmb_data->hw_block, 0, cmb_data->size);
		cmb_data->last_update = 0;
	}
	cdev->private->cmb_start_time = get_tod_clock();
	spin_unlock_irq(cdev->ccwlock);
}

/**
 * struct cmb_area - container for global cmb data
 *
 * @mem:	pointer to CMBs (only in basic measurement mode)
 * @list:	contains a linked list of all subchannels
 * @num_channels: number of channels to be measured
 * @lock:	protect concurrent access to @mem and @list
 */
struct cmb_area {
	struct cmb *mem;
	struct list_head list;
	int num_channels;
	spinlock_t lock;
};

static struct cmb_area cmb_area = {
	.lock = __SPIN_LOCK_UNLOCKED(cmb_area.lock),
	.list = LIST_HEAD_INIT(cmb_area.list),
	.num_channels  = 1024,
};

/* ****** old style CMB handling ********/

/*
 * Basic channel measurement blocks are allocated in one contiguous
 * block of memory, which can not be moved as long as any channel
 * is active. Therefore, a maximum number of subchannels needs to
 * be defined somewhere. This is a module parameter, defaulting to
 * a reasonable value of 1024, or 32 kb of memory.
 * Current kernels don't allow kmalloc with more than 128kb, so the
 * maximum is 4096.
 */

module_param_named(maxchannels, cmb_area.num_channels, uint, 0444);

/**
 * struct cmb - basic channel measurement block
 * @ssch_rsch_count: number of ssch and rsch
 * @sample_count: number of samples
 * @device_connect_time: time of device connect
 * @function_pending_time: time of function pending
 * @device_disconnect_time: time of device disconnect
 * @control_unit_queuing_time: time of control unit queuing
 * @device_active_only_time: time of device active only
 * @reserved: unused in basic measurement mode
 *
 * The measurement block as used by the hardware. The fields are described
 * further in z/Architecture Principles of Operation, chapter 17.
 *
 * The cmb area made up from these blocks must be a contiguous array and may
 * not be reallocated or freed.
 * Only one cmb area can be present in the system.
 */
struct cmb {
	u16 ssch_rsch_count;
	u16 sample_count;
	u32 device_connect_time;
	u32 function_pending_time;
	u32 device_disconnect_time;
	u32 control_unit_queuing_time;
	u32 device_active_only_time;
	u32 reserved[2];
};

/*
 * Insert a single device into the cmb_area list.
 * Called with cmb_area.lock held from alloc_cmb.
 */
static int alloc_cmb_single(struct ccw_device *cdev,
			    struct cmb_data *cmb_data)
{
	struct cmb *cmb;
	struct ccw_device_private *node;
	int ret;

	spin_lock_irq(cdev->ccwlock);
	if (!list_empty(&cdev->private->cmb_list)) {
		ret = -EBUSY;
		goto out;
	}

	/*
	 * Find first unused cmb in cmb_area.mem.
	 * This is a little tricky: cmb_area.list
	 * remains sorted by ->cmb->hw_data pointers.
	 */
	cmb = cmb_area.mem;
	list_for_each_entry(node, &cmb_area.list, cmb_list) {
		struct cmb_data *data;
		data = node->cmb;
		if ((struct cmb*)data->hw_block > cmb)
			break;
		cmb++;
	}
	if (cmb - cmb_area.mem >= cmb_area.num_channels) {
		ret = -ENOMEM;
		goto out;
	}

	/* insert new cmb */
	list_add_tail(&cdev->private->cmb_list, &node->cmb_list);
	cmb_data->hw_block = cmb;
	cdev->private->cmb = cmb_data;
	ret = 0;
out:
	spin_unlock_irq(cdev->ccwlock);
	return ret;
}

static int alloc_cmb(struct ccw_device *cdev)
{
	int ret;
	struct cmb *mem;
	ssize_t size;
	struct cmb_data *cmb_data;

	/* Allocate private cmb_data. */
	cmb_data = kzalloc(sizeof(struct cmb_data), GFP_KERNEL);
	if (!cmb_data)
		return -ENOMEM;

	cmb_data->last_block = kzalloc(sizeof(struct cmb), GFP_KERNEL);
	if (!cmb_data->last_block) {
		kfree(cmb_data);
		return -ENOMEM;
	}
	cmb_data->size = sizeof(struct cmb);
	spin_lock(&cmb_area.lock);

	if (!cmb_area.mem) {
		/* there is no user yet, so we need a new area */
		size = sizeof(struct cmb) * cmb_area.num_channels;
		WARN_ON(!list_empty(&cmb_area.list));

		spin_unlock(&cmb_area.lock);
		mem = (void*)__get_free_pages(GFP_KERNEL | GFP_DMA,
				 get_order(size));
		spin_lock(&cmb_area.lock);

		if (cmb_area.mem) {
			/* ok, another thread was faster */
			free_pages((unsigned long)mem, get_order(size));
		} else if (!mem) {
			/* no luck */
			ret = -ENOMEM;
			goto out;
		} else {
			/* everything ok */
			memset(mem, 0, size);
			cmb_area.mem = mem;
			cmf_activate(cmb_area.mem, CMF_ON);
		}
	}

	/* do the actual allocation */
	ret = alloc_cmb_single(cdev, cmb_data);
out:
	spin_unlock(&cmb_area.lock);
	if (ret) {
		kfree(cmb_data->last_block);
		kfree(cmb_data);
	}
	return ret;
}

static void free_cmb(struct ccw_device *cdev)
{
	struct ccw_device_private *priv;
	struct cmb_data *cmb_data;

	spin_lock(&cmb_area.lock);
	spin_lock_irq(cdev->ccwlock);

	priv = cdev->private;
	cmb_data = priv->cmb;
	priv->cmb = NULL;
	if (cmb_data)
		kfree(cmb_data->last_block);
	kfree(cmb_data);
	list_del_init(&priv->cmb_list);

	if (list_empty(&cmb_area.list)) {
		ssize_t size;
		size = sizeof(struct cmb) * cmb_area.num_channels;
		cmf_activate(NULL, CMF_OFF);
		free_pages((unsigned long)cmb_area.mem, get_order(size));
		cmb_area.mem = NULL;
	}
	spin_unlock_irq(cdev->ccwlock);
	spin_unlock(&cmb_area.lock);
}

static int set_cmb(struct ccw_device *cdev, u32 mme)
{
	u16 offset;
	struct cmb_data *cmb_data;
	unsigned long flags;

	spin_lock_irqsave(cdev->ccwlock, flags);
	if (!cdev->private->cmb) {
		spin_unlock_irqrestore(cdev->ccwlock, flags);
		return -EINVAL;
	}
	cmb_data = cdev->private->cmb;
	offset = mme ? (struct cmb *)cmb_data->hw_block - cmb_area.mem : 0;
	spin_unlock_irqrestore(cdev->ccwlock, flags);

	return set_schib_wait(cdev, mme, 0, offset);
}

/* calculate utilization in 0.1 percent units */
static u64 __cmb_utilization(u64 device_connect_time, u64 function_pending_time,
			     u64 device_disconnect_time, u64 start_time)
{
	u64 utilization, elapsed_time;

	utilization = time_to_nsec(device_connect_time +
				   function_pending_time +
				   device_disconnect_time);

	elapsed_time = get_tod_clock() - start_time;
	elapsed_time = tod_to_ns(elapsed_time);
	elapsed_time /= 1000;

	return elapsed_time ? (utilization / elapsed_time) : 0;
}

static u64 read_cmb(struct ccw_device *cdev, int index)
{
	struct cmb_data *cmb_data;
	unsigned long flags;
	struct cmb *cmb;
	u64 ret = 0;
	u32 val;

	spin_lock_irqsave(cdev->ccwlock, flags);
	cmb_data = cdev->private->cmb;
	if (!cmb_data)
		goto out;

	cmb = cmb_data->hw_block;
	switch (index) {
	case avg_utilization:
		ret = __cmb_utilization(cmb->device_connect_time,
					cmb->function_pending_time,
					cmb->device_disconnect_time,
					cdev->private->cmb_start_time);
		goto out;
	case cmb_ssch_rsch_count:
		ret = cmb->ssch_rsch_count;
		goto out;
	case cmb_sample_count:
		ret = cmb->sample_count;
		goto out;
	case cmb_device_connect_time:
		val = cmb->device_connect_time;
		break;
	case cmb_function_pending_time:
		val = cmb->function_pending_time;
		break;
	case cmb_device_disconnect_time:
		val = cmb->device_disconnect_time;
		break;
	case cmb_control_unit_queuing_time:
		val = cmb->control_unit_queuing_time;
		break;
	case cmb_device_active_only_time:
		val = cmb->device_active_only_time;
		break;
	default:
		goto out;
	}
	ret = time_to_avg_nsec(val, cmb->sample_count);
out:
	spin_unlock_irqrestore(cdev->ccwlock, flags);
	return ret;
}

static int readall_cmb(struct ccw_device *cdev, struct cmbdata *data)
{
	struct cmb *cmb;
	struct cmb_data *cmb_data;
	u64 time;
	unsigned long flags;
	int ret;

	ret = cmf_cmb_copy_wait(cdev);
	if (ret < 0)
		return ret;
	spin_lock_irqsave(cdev->ccwlock, flags);
	cmb_data = cdev->private->cmb;
	if (!cmb_data) {
		ret = -ENODEV;
		goto out;
	}
	if (cmb_data->last_update == 0) {
		ret = -EAGAIN;
		goto out;
	}
	cmb = cmb_data->last_block;
	time = cmb_data->last_update - cdev->private->cmb_start_time;

	memset(data, 0, sizeof(struct cmbdata));

	/* we only know values before device_busy_time */
	data->size = offsetof(struct cmbdata, device_busy_time);

	data->elapsed_time = tod_to_ns(time);

	/* copy data to new structure */
	data->ssch_rsch_count = cmb->ssch_rsch_count;
	data->sample_count = cmb->sample_count;

	/* time fields are converted to nanoseconds while copying */
	data->device_connect_time = time_to_nsec(cmb->device_connect_time);
	data->function_pending_time = time_to_nsec(cmb->function_pending_time);
	data->device_disconnect_time =
		time_to_nsec(cmb->device_disconnect_time);
	data->control_unit_queuing_time
		= time_to_nsec(cmb->control_unit_queuing_time);
	data->device_active_only_time
		= time_to_nsec(cmb->device_active_only_time);
	ret = 0;
out:
	spin_unlock_irqrestore(cdev->ccwlock, flags);
	return ret;
}

static void reset_cmb(struct ccw_device *cdev)
{
	cmf_generic_reset(cdev);
}

static int cmf_enabled(struct ccw_device *cdev)
{
	int enabled;

	spin_lock_irq(cdev->ccwlock);
	enabled = !!cdev->private->cmb;
	spin_unlock_irq(cdev->ccwlock);

	return enabled;
}

static struct attribute_group cmf_attr_group;

static struct cmb_operations cmbops_basic = {
	.alloc	= alloc_cmb,
	.free	= free_cmb,
	.set	= set_cmb,
	.read	= read_cmb,
	.readall    = readall_cmb,
	.reset	    = reset_cmb,
	.attr_group = &cmf_attr_group,
};

/* ******** extended cmb handling ********/

/**
 * struct cmbe - extended channel measurement block
 * @ssch_rsch_count: number of ssch and rsch
 * @sample_count: number of samples
 * @device_connect_time: time of device connect
 * @function_pending_time: time of function pending
 * @device_disconnect_time: time of device disconnect
 * @control_unit_queuing_time: time of control unit queuing
 * @device_active_only_time: time of device active only
 * @device_busy_time: time of device busy
 * @initial_command_response_time: initial command response time
 * @reserved: unused
 *
 * The measurement block as used by the hardware. May be in any 64 bit physical
 * location.
 * The fields are described further in z/Architecture Principles of Operation,
 * third edition, chapter 17.
 */
struct cmbe {
	u32 ssch_rsch_count;
	u32 sample_count;
	u32 device_connect_time;
	u32 function_pending_time;
	u32 device_disconnect_time;
	u32 control_unit_queuing_time;
	u32 device_active_only_time;
	u32 device_busy_time;
	u32 initial_command_response_time;
	u32 reserved[7];
} __packed __aligned(64);

static struct kmem_cache *cmbe_cache;

static int alloc_cmbe(struct ccw_device *cdev)
{
	struct cmb_data *cmb_data;
	struct cmbe *cmbe;
	int ret = -ENOMEM;

	cmbe = kmem_cache_zalloc(cmbe_cache, GFP_KERNEL);
	if (!cmbe)
		return ret;

	cmb_data = kzalloc(sizeof(*cmb_data), GFP_KERNEL);
	if (!cmb_data)
		goto out_free;

	cmb_data->last_block = kzalloc(sizeof(struct cmbe), GFP_KERNEL);
	if (!cmb_data->last_block)
		goto out_free;

	cmb_data->size = sizeof(*cmbe);
	cmb_data->hw_block = cmbe;

	spin_lock(&cmb_area.lock);
	spin_lock_irq(cdev->ccwlock);
	if (cdev->private->cmb)
		goto out_unlock;

	cdev->private->cmb = cmb_data;

	/* activate global measurement if this is the first channel */
	if (list_empty(&cmb_area.list))
		cmf_activate(NULL, CMF_ON);
	list_add_tail(&cdev->private->cmb_list, &cmb_area.list);

	spin_unlock_irq(cdev->ccwlock);
	spin_unlock(&cmb_area.lock);
	return 0;

out_unlock:
	spin_unlock_irq(cdev->ccwlock);
	spin_unlock(&cmb_area.lock);
	ret = -EBUSY;
out_free:
	if (cmb_data)
		kfree(cmb_data->last_block);
	kfree(cmb_data);
	kmem_cache_free(cmbe_cache, cmbe);

	return ret;
}

static void free_cmbe(struct ccw_device *cdev)
{
	struct cmb_data *cmb_data;

	spin_lock(&cmb_area.lock);
	spin_lock_irq(cdev->ccwlock);
	cmb_data = cdev->private->cmb;
	cdev->private->cmb = NULL;
	if (cmb_data) {
		kfree(cmb_data->last_block);
		kmem_cache_free(cmbe_cache, cmb_data->hw_block);
	}
	kfree(cmb_data);

	/* deactivate global measurement if this is the last channel */
	list_del_init(&cdev->private->cmb_list);
	if (list_empty(&cmb_area.list))
		cmf_activate(NULL, CMF_OFF);
	spin_unlock_irq(cdev->ccwlock);
	spin_unlock(&cmb_area.lock);
}

static int set_cmbe(struct ccw_device *cdev, u32 mme)
{
	unsigned long mba;
	struct cmb_data *cmb_data;
	unsigned long flags;

	spin_lock_irqsave(cdev->ccwlock, flags);
	if (!cdev->private->cmb) {
		spin_unlock_irqrestore(cdev->ccwlock, flags);
		return -EINVAL;
	}
	cmb_data = cdev->private->cmb;
	mba = mme ? (unsigned long) cmb_data->hw_block : 0;
	spin_unlock_irqrestore(cdev->ccwlock, flags);

	return set_schib_wait(cdev, mme, 1, mba);
}

static u64 read_cmbe(struct ccw_device *cdev, int index)
{
	struct cmb_data *cmb_data;
	unsigned long flags;
	struct cmbe *cmb;
	u64 ret = 0;
	u32 val;

	spin_lock_irqsave(cdev->ccwlock, flags);
	cmb_data = cdev->private->cmb;
	if (!cmb_data)
		goto out;

	cmb = cmb_data->hw_block;
	switch (index) {
	case avg_utilization:
		ret = __cmb_utilization(cmb->device_connect_time,
					cmb->function_pending_time,
					cmb->device_disconnect_time,
					cdev->private->cmb_start_time);
		goto out;
	case cmb_ssch_rsch_count:
		ret = cmb->ssch_rsch_count;
		goto out;
	case cmb_sample_count:
		ret = cmb->sample_count;
		goto out;
	case cmb_device_connect_time:
		val = cmb->device_connect_time;
		break;
	case cmb_function_pending_time:
		val = cmb->function_pending_time;
		break;
	case cmb_device_disconnect_time:
		val = cmb->device_disconnect_time;
		break;
	case cmb_control_unit_queuing_time:
		val = cmb->control_unit_queuing_time;
		break;
	case cmb_device_active_only_time:
		val = cmb->device_active_only_time;
		break;
	case cmb_device_busy_time:
		val = cmb->device_busy_time;
		break;
	case cmb_initial_command_response_time:
		val = cmb->initial_command_response_time;
		break;
	default:
		goto out;
	}
	ret = time_to_avg_nsec(val, cmb->sample_count);
out:
	spin_unlock_irqrestore(cdev->ccwlock, flags);
	return ret;
}

static int readall_cmbe(struct ccw_device *cdev, struct cmbdata *data)
{
	struct cmbe *cmb;
	struct cmb_data *cmb_data;
	u64 time;
	unsigned long flags;
	int ret;

	ret = cmf_cmb_copy_wait(cdev);
	if (ret < 0)
		return ret;
	spin_lock_irqsave(cdev->ccwlock, flags);
	cmb_data = cdev->private->cmb;
	if (!cmb_data) {
		ret = -ENODEV;
		goto out;
	}
	if (cmb_data->last_update == 0) {
		ret = -EAGAIN;
		goto out;
	}
	time = cmb_data->last_update - cdev->private->cmb_start_time;

	memset (data, 0, sizeof(struct cmbdata));

	/* we only know values before device_busy_time */
	data->size = offsetof(struct cmbdata, device_busy_time);

	data->elapsed_time = tod_to_ns(time);

	cmb = cmb_data->last_block;
	/* copy data to new structure */
	data->ssch_rsch_count = cmb->ssch_rsch_count;
	data->sample_count = cmb->sample_count;

	/* time fields are converted to nanoseconds while copying */
	data->device_connect_time = time_to_nsec(cmb->device_connect_time);
	data->function_pending_time = time_to_nsec(cmb->function_pending_time);
	data->device_disconnect_time =
		time_to_nsec(cmb->device_disconnect_time);
	data->control_unit_queuing_time
		= time_to_nsec(cmb->control_unit_queuing_time);
	data->device_active_only_time
		= time_to_nsec(cmb->device_active_only_time);
	data->device_busy_time = time_to_nsec(cmb->device_busy_time);
	data->initial_command_response_time
		= time_to_nsec(cmb->initial_command_response_time);

	ret = 0;
out:
	spin_unlock_irqrestore(cdev->ccwlock, flags);
	return ret;
}

static void reset_cmbe(struct ccw_device *cdev)
{
	cmf_generic_reset(cdev);
}

static struct attribute_group cmf_attr_group_ext;

static struct cmb_operations cmbops_extended = {
	.alloc	    = alloc_cmbe,
	.free	    = free_cmbe,
	.set	    = set_cmbe,
	.read	    = read_cmbe,
	.readall    = readall_cmbe,
	.reset	    = reset_cmbe,
	.attr_group = &cmf_attr_group_ext,
};

static ssize_t cmb_show_attr(struct device *dev, char *buf, enum cmb_index idx)
{
	return sprintf(buf, "%lld\n",
		(unsigned long long) cmf_read(to_ccwdev(dev), idx));
}

static ssize_t cmb_show_avg_sample_interval(struct device *dev,
					    struct device_attribute *attr,
					    char *buf)
{
	struct ccw_device *cdev = to_ccwdev(dev);
	unsigned long count;
	long interval;

	count = cmf_read(cdev, cmb_sample_count);
	spin_lock_irq(cdev->ccwlock);
	if (count) {
		interval = get_tod_clock() - cdev->private->cmb_start_time;
		interval = tod_to_ns(interval);
		interval /= count;
	} else
		interval = -1;
	spin_unlock_irq(cdev->ccwlock);
	return sprintf(buf, "%ld\n", interval);
}

static ssize_t cmb_show_avg_utilization(struct device *dev,
					struct device_attribute *attr,
					char *buf)
{
	unsigned long u = cmf_read(to_ccwdev(dev), avg_utilization);

	return sprintf(buf, "%02lu.%01lu%%\n", u / 10, u % 10);
}

#define cmf_attr(name) \
static ssize_t show_##name(struct device *dev, \
			   struct device_attribute *attr, char *buf)	\
{ return cmb_show_attr((dev), buf, cmb_##name); } \
static DEVICE_ATTR(name, 0444, show_##name, NULL);

#define cmf_attr_avg(name) \
static ssize_t show_avg_##name(struct device *dev, \
			       struct device_attribute *attr, char *buf) \
{ return cmb_show_attr((dev), buf, cmb_##name); } \
static DEVICE_ATTR(avg_##name, 0444, show_avg_##name, NULL);

cmf_attr(ssch_rsch_count);
cmf_attr(sample_count);
cmf_attr_avg(device_connect_time);
cmf_attr_avg(function_pending_time);
cmf_attr_avg(device_disconnect_time);
cmf_attr_avg(control_unit_queuing_time);
cmf_attr_avg(device_active_only_time);
cmf_attr_avg(device_busy_time);
cmf_attr_avg(initial_command_response_time);

static DEVICE_ATTR(avg_sample_interval, 0444, cmb_show_avg_sample_interval,
		   NULL);
static DEVICE_ATTR(avg_utilization, 0444, cmb_show_avg_utilization, NULL);

static struct attribute *cmf_attributes[] = {
	&dev_attr_avg_sample_interval.attr,
	&dev_attr_avg_utilization.attr,
	&dev_attr_ssch_rsch_count.attr,
	&dev_attr_sample_count.attr,
	&dev_attr_avg_device_connect_time.attr,
	&dev_attr_avg_function_pending_time.attr,
	&dev_attr_avg_device_disconnect_time.attr,
	&dev_attr_avg_control_unit_queuing_time.attr,
	&dev_attr_avg_device_active_only_time.attr,
	NULL,
};

static struct attribute_group cmf_attr_group = {
	.name  = "cmf",
	.attrs = cmf_attributes,
};

static struct attribute *cmf_attributes_ext[] = {
	&dev_attr_avg_sample_interval.attr,
	&dev_attr_avg_utilization.attr,
	&dev_attr_ssch_rsch_count.attr,
	&dev_attr_sample_count.attr,
	&dev_attr_avg_device_connect_time.attr,
	&dev_attr_avg_function_pending_time.attr,
	&dev_attr_avg_device_disconnect_time.attr,
	&dev_attr_avg_control_unit_queuing_time.attr,
	&dev_attr_avg_device_active_only_time.attr,
	&dev_attr_avg_device_busy_time.attr,
	&dev_attr_avg_initial_command_response_time.attr,
	NULL,
};

static struct attribute_group cmf_attr_group_ext = {
	.name  = "cmf",
	.attrs = cmf_attributes_ext,
};

static ssize_t cmb_enable_show(struct device *dev,
			       struct device_attribute *attr,
			       char *buf)
{
	struct ccw_device *cdev = to_ccwdev(dev);

	return sprintf(buf, "%d\n", cmf_enabled(cdev));
}

static ssize_t cmb_enable_store(struct device *dev,
				struct device_attribute *attr, const char *buf,
				size_t c)
{
	struct ccw_device *cdev = to_ccwdev(dev);
	unsigned long val;
	int ret;

	ret = kstrtoul(buf, 16, &val);
	if (ret)
		return ret;

	switch (val) {
	case 0:
		ret = disable_cmf(cdev);
		break;
	case 1:
		ret = enable_cmf(cdev);
		break;
	default:
		ret = -EINVAL;
	}

	return ret ? ret : c;
}
DEVICE_ATTR_RW(cmb_enable);

/**
 * enable_cmf() - switch on the channel measurement for a specific device
 *  @cdev:	The ccw device to be enabled
 *
 *  Enable channel measurements for @cdev. If this is called on a device
 *  for which channel measurement is already enabled a reset of the
 *  measurement data is triggered.
 *  Returns: %0 for success or a negative error value.
 *  Context:
 *    non-atomic
 */
int enable_cmf(struct ccw_device *cdev)
{
	int ret = 0;

	device_lock(&cdev->dev);
	if (cmf_enabled(cdev)) {
		cmbops->reset(cdev);
		goto out_unlock;
	}
	get_device(&cdev->dev);
	ret = cmbops->alloc(cdev);
	if (ret)
		goto out;
	cmbops->reset(cdev);
	ret = sysfs_create_group(&cdev->dev.kobj, cmbops->attr_group);
	if (ret) {
		cmbops->free(cdev);
		goto out;
	}
	ret = cmbops->set(cdev, 2);
	if (ret) {
		sysfs_remove_group(&cdev->dev.kobj, cmbops->attr_group);
		cmbops->free(cdev);
	}
out:
	if (ret)
		put_device(&cdev->dev);
out_unlock:
	device_unlock(&cdev->dev);
	return ret;
}

/**
 * __disable_cmf() - switch off the channel measurement for a specific device
 *  @cdev:	The ccw device to be disabled
 *
 *  Returns: %0 for success or a negative error value.
 *
 *  Context:
 *    non-atomic, device_lock() held.
 */
int __disable_cmf(struct ccw_device *cdev)
{
	int ret;

	ret = cmbops->set(cdev, 0);
	if (ret)
		return ret;

	sysfs_remove_group(&cdev->dev.kobj, cmbops->attr_group);
	cmbops->free(cdev);
	put_device(&cdev->dev);

	return ret;
}

/**
 * disable_cmf() - switch off the channel measurement for a specific device
 *  @cdev:	The ccw device to be disabled
 *
 *  Returns: %0 for success or a negative error value.
 *
 *  Context:
 *    non-atomic
 */
int disable_cmf(struct ccw_device *cdev)
{
	int ret;

	device_lock(&cdev->dev);
	ret = __disable_cmf(cdev);
	device_unlock(&cdev->dev);

	return ret;
}

/**
 * cmf_read() - read one value from the current channel measurement block
 * @cdev:	the channel to be read
 * @index:	the index of the value to be read
 *
 * Returns: The value read or %0 if the value cannot be read.
 *
 *  Context:
 *    any
 */
u64 cmf_read(struct ccw_device *cdev, int index)
{
	return cmbops->read(cdev, index);
}

/**
 * cmf_readall() - read the current channel measurement block
 * @cdev:	the channel to be read
 * @data:	a pointer to a data block that will be filled
 *
 * Returns: %0 on success, a negative error value otherwise.
 *
 *  Context:
 *    any
 */
int cmf_readall(struct ccw_device *cdev, struct cmbdata *data)
{
	return cmbops->readall(cdev, data);
}

/* Reenable cmf when a disconnected device becomes available again. */
int cmf_reenable(struct ccw_device *cdev)
{
	cmbops->reset(cdev);
	return cmbops->set(cdev, 2);
}

/**
 * cmf_reactivate() - reactivate measurement block updates
 *
 * Use this during resume from hibernate.
 */
void cmf_reactivate(void)
{
	spin_lock(&cmb_area.lock);
	if (!list_empty(&cmb_area.list))
		cmf_activate(cmb_area.mem, CMF_ON);
	spin_unlock(&cmb_area.lock);
}

static int __init init_cmbe(void)
{
	cmbe_cache = kmem_cache_create("cmbe_cache", sizeof(struct cmbe),
				       __alignof__(struct cmbe), 0, NULL);

	return cmbe_cache ? 0 : -ENOMEM;
}

static int __init init_cmf(void)
{
	char *format_string;
	char *detect_string;
	int ret;

	/*
	 * If the user did not give a parameter, see if we are running on a
	 * machine supporting extended measurement blocks, otherwise fall back
	 * to basic mode.
	 */
	if (format == CMF_AUTODETECT) {
		if (!css_general_characteristics.ext_mb) {
			format = CMF_BASIC;
		} else {
			format = CMF_EXTENDED;
		}
		detect_string = "autodetected";
	} else {
		detect_string = "parameter";
	}

	switch (format) {
	case CMF_BASIC:
		format_string = "basic";
		cmbops = &cmbops_basic;
		break;
	case CMF_EXTENDED:
		format_string = "extended";
		cmbops = &cmbops_extended;

		ret = init_cmbe();
		if (ret)
			return ret;
		break;
	default:
		return -EINVAL;
	}
	pr_info("Channel measurement facility initialized using format "
		"%s (mode %s)\n", format_string, detect_string);
	return 0;
}
device_initcall(init_cmf);

EXPORT_SYMBOL_GPL(enable_cmf);
EXPORT_SYMBOL_GPL(disable_cmf);
EXPORT_SYMBOL_GPL(cmf_read);
EXPORT_SYMBOL_GPL(cmf_readall);
