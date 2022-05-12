// SPDX-License-Identifier: GPL-2.0
/*
 * Device driver for the Apple Desktop Bus
 * and the /dev/adb device on macintoshes.
 *
 * Copyright (C) 1996 Paul Mackerras.
 *
 * Modified to declare controllers as structures, added
 * client notification of bus reset and handles PowerBook
 * sleep, by Benjamin Herrenschmidt.
 *
 * To do:
 *
 * - /sys/bus/adb to list the devices and infos
 * - more /dev/adb to allow userland to receive the
 *   flow of auto-polling datas from a given device.
 * - move bus probe to a kernel thread
 */

#include <linux/types.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/sched/signal.h>
#include <linux/adb.h>
#include <linux/cuda.h>
#include <linux/pmu.h>
#include <linux/notifier.h>
#include <linux/wait.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/spinlock.h>
#include <linux/completion.h>
#include <linux/device.h>
#include <linux/kthread.h>
#include <linux/platform_device.h>
#include <linux/mutex.h>

#include <linux/uaccess.h>
#ifdef CONFIG_PPC
#include <asm/prom.h>
#include <asm/machdep.h>
#endif


EXPORT_SYMBOL(adb_client_list);

extern struct adb_driver via_macii_driver;
extern struct adb_driver via_cuda_driver;
extern struct adb_driver adb_iop_driver;
extern struct adb_driver via_pmu_driver;
extern struct adb_driver macio_adb_driver;

static DEFINE_MUTEX(adb_mutex);
static struct adb_driver *adb_driver_list[] = {
#ifdef CONFIG_ADB_MACII
	&via_macii_driver,
#endif
#ifdef CONFIG_ADB_CUDA
	&via_cuda_driver,
#endif
#ifdef CONFIG_ADB_IOP
	&adb_iop_driver,
#endif
#ifdef CONFIG_ADB_PMU
	&via_pmu_driver,
#endif
#ifdef CONFIG_ADB_MACIO
	&macio_adb_driver,
#endif
	NULL
};

static struct class *adb_dev_class;

static struct adb_driver *adb_controller;
BLOCKING_NOTIFIER_HEAD(adb_client_list);
static int adb_got_sleep;
static int adb_inited;
static DEFINE_SEMAPHORE(adb_probe_mutex);
static int sleepy_trackpad;
static int autopoll_devs;
int __adb_probe_sync;

static int adb_scan_bus(void);
static int do_adb_reset_bus(void);
static void adbdev_init(void);
static int try_handler_change(int, int);

static struct adb_handler {
	void (*handler)(unsigned char *, int, int);
	int original_address;
	int handler_id;
	int busy;
} adb_handler[16];

/*
 * The adb_handler_mutex mutex protects all accesses to the original_address
 * and handler_id fields of adb_handler[i] for all i, and changes to the
 * handler field.
 * Accesses to the handler field are protected by the adb_handler_lock
 * rwlock.  It is held across all calls to any handler, so that by the
 * time adb_unregister returns, we know that the old handler isn't being
 * called.
 */
static DEFINE_MUTEX(adb_handler_mutex);
static DEFINE_RWLOCK(adb_handler_lock);

#if 0
static void printADBreply(struct adb_request *req)
{
        int i;

        printk("adb reply (%d)", req->reply_len);
        for(i = 0; i < req->reply_len; i++)
                printk(" %x", req->reply[i]);
        printk("\n");

}
#endif

static int adb_scan_bus(void)
{
	int i, highFree=0, noMovement;
	int devmask = 0;
	struct adb_request req;
	
	/* assumes adb_handler[] is all zeroes at this point */
	for (i = 1; i < 16; i++) {
		/* see if there is anything at address i */
		adb_request(&req, NULL, ADBREQ_SYNC | ADBREQ_REPLY, 1,
                            (i << 4) | 0xf);
		if (req.reply_len > 1)
			/* one or more devices at this address */
			adb_handler[i].original_address = i;
		else if (i > highFree)
			highFree = i;
	}

	/* Note we reset noMovement to 0 each time we move a device */
	for (noMovement = 1; noMovement < 2 && highFree > 0; noMovement++) {
		for (i = 1; i < 16; i++) {
			if (adb_handler[i].original_address == 0)
				continue;
			/*
			 * Send a "talk register 3" command to address i
			 * to provoke a collision if there is more than
			 * one device at this address.
			 */
			adb_request(&req, NULL, ADBREQ_SYNC | ADBREQ_REPLY, 1,
				    (i << 4) | 0xf);
			/*
			 * Move the device(s) which didn't detect a
			 * collision to address `highFree'.  Hopefully
			 * this only moves one device.
			 */
			adb_request(&req, NULL, ADBREQ_SYNC, 3,
				    (i<< 4) | 0xb, (highFree | 0x60), 0xfe);
			/*
			 * See if anybody actually moved. This is suggested
			 * by HW TechNote 01:
			 *
			 * https://developer.apple.com/technotes/hw/hw_01.html
			 */
			adb_request(&req, NULL, ADBREQ_SYNC | ADBREQ_REPLY, 1,
				    (highFree << 4) | 0xf);
			if (req.reply_len <= 1) continue;
			/*
			 * Test whether there are any device(s) left
			 * at address i.
			 */
			adb_request(&req, NULL, ADBREQ_SYNC | ADBREQ_REPLY, 1,
				    (i << 4) | 0xf);
			if (req.reply_len > 1) {
				/*
				 * There are still one or more devices
				 * left at address i.  Register the one(s)
				 * we moved to `highFree', and find a new
				 * value for highFree.
				 */
				adb_handler[highFree].original_address =
					adb_handler[i].original_address;
				while (highFree > 0 &&
				       adb_handler[highFree].original_address)
					highFree--;
				if (highFree <= 0)
					break;

				noMovement = 0;
			} else {
				/*
				 * No devices left at address i; move the
				 * one(s) we moved to `highFree' back to i.
				 */
				adb_request(&req, NULL, ADBREQ_SYNC, 3,
					    (highFree << 4) | 0xb,
					    (i | 0x60), 0xfe);
			}
		}	
	}

	/* Now fill in the handler_id field of the adb_handler entries. */
	for (i = 1; i < 16; i++) {
		if (adb_handler[i].original_address == 0)
			continue;
		adb_request(&req, NULL, ADBREQ_SYNC | ADBREQ_REPLY, 1,
			    (i << 4) | 0xf);
		adb_handler[i].handler_id = req.reply[2];
		printk(KERN_DEBUG "adb device [%d]: %d 0x%X\n", i,
		       adb_handler[i].original_address,
		       adb_handler[i].handler_id);
		devmask |= 1 << i;
	}
	return devmask;
}

/*
 * This kernel task handles ADB probing. It dies once probing is
 * completed.
 */
static int
adb_probe_task(void *x)
{
	pr_debug("adb: starting probe task...\n");
	do_adb_reset_bus();
	pr_debug("adb: finished probe task...\n");

	up(&adb_probe_mutex);

	return 0;
}

static void
__adb_probe_task(struct work_struct *bullshit)
{
	kthread_run(adb_probe_task, NULL, "kadbprobe");
}

static DECLARE_WORK(adb_reset_work, __adb_probe_task);

int
adb_reset_bus(void)
{
	if (__adb_probe_sync) {
		do_adb_reset_bus();
		return 0;
	}

	down(&adb_probe_mutex);
	schedule_work(&adb_reset_work);
	return 0;
}

#ifdef CONFIG_PM
/*
 * notify clients before sleep
 */
static int __adb_suspend(struct platform_device *dev, pm_message_t state)
{
	adb_got_sleep = 1;
	/* We need to get a lock on the probe thread */
	down(&adb_probe_mutex);
	/* Stop autopoll */
	if (adb_controller->autopoll)
		adb_controller->autopoll(0);
	blocking_notifier_call_chain(&adb_client_list, ADB_MSG_POWERDOWN, NULL);

	return 0;
}

static int adb_suspend(struct device *dev)
{
	return __adb_suspend(to_platform_device(dev), PMSG_SUSPEND);
}

static int adb_freeze(struct device *dev)
{
	return __adb_suspend(to_platform_device(dev), PMSG_FREEZE);
}

static int adb_poweroff(struct device *dev)
{
	return __adb_suspend(to_platform_device(dev), PMSG_HIBERNATE);
}

/*
 * reset bus after sleep
 */
static int __adb_resume(struct platform_device *dev)
{
	adb_got_sleep = 0;
	up(&adb_probe_mutex);
	adb_reset_bus();

	return 0;
}

static int adb_resume(struct device *dev)
{
	return __adb_resume(to_platform_device(dev));
}
#endif /* CONFIG_PM */

static int __init adb_init(void)
{
	struct adb_driver *driver;
	int i;

#ifdef CONFIG_PPC32
	if (!machine_is(chrp) && !machine_is(powermac))
		return 0;
#endif
#ifdef CONFIG_MAC
	if (!MACH_IS_MAC)
		return 0;
#endif

	/* xmon may do early-init */
	if (adb_inited)
		return 0;
	adb_inited = 1;
		
	adb_controller = NULL;

	i = 0;
	while ((driver = adb_driver_list[i++]) != NULL) {
		if (!driver->probe()) {
			adb_controller = driver;
			break;
		}
	}
	if (adb_controller != NULL && adb_controller->init &&
	    adb_controller->init())
		adb_controller = NULL;
	if (adb_controller == NULL) {
		pr_warn("Warning: no ADB interface detected\n");
	} else {
#ifdef CONFIG_PPC
		if (of_machine_is_compatible("AAPL,PowerBook1998") ||
			of_machine_is_compatible("PowerBook1,1"))
			sleepy_trackpad = 1;
#endif /* CONFIG_PPC */

		adbdev_init();
		adb_reset_bus();
	}
	return 0;
}

device_initcall(adb_init);

static int
do_adb_reset_bus(void)
{
	int ret;
	
	if (adb_controller == NULL)
		return -ENXIO;
		
	if (adb_controller->autopoll)
		adb_controller->autopoll(0);

	blocking_notifier_call_chain(&adb_client_list,
		ADB_MSG_PRE_RESET, NULL);

	if (sleepy_trackpad) {
		/* Let the trackpad settle down */
		msleep(500);
	}

	mutex_lock(&adb_handler_mutex);
	write_lock_irq(&adb_handler_lock);
	memset(adb_handler, 0, sizeof(adb_handler));
	write_unlock_irq(&adb_handler_lock);

	/* That one is still a bit synchronous, oh well... */
	if (adb_controller->reset_bus)
		ret = adb_controller->reset_bus();
	else
		ret = 0;

	if (sleepy_trackpad) {
		/* Let the trackpad settle down */
		msleep(1500);
	}

	if (!ret) {
		autopoll_devs = adb_scan_bus();
		if (adb_controller->autopoll)
			adb_controller->autopoll(autopoll_devs);
	}
	mutex_unlock(&adb_handler_mutex);

	blocking_notifier_call_chain(&adb_client_list,
		ADB_MSG_POST_RESET, NULL);
	
	return ret;
}

void
adb_poll(void)
{
	if ((adb_controller == NULL)||(adb_controller->poll == NULL))
		return;
	adb_controller->poll();
}
EXPORT_SYMBOL(adb_poll);

static void adb_sync_req_done(struct adb_request *req)
{
	struct completion *comp = req->arg;

	complete(comp);
}

int
adb_request(struct adb_request *req, void (*done)(struct adb_request *),
	    int flags, int nbytes, ...)
{
	va_list list;
	int i;
	int rc;
	struct completion comp;

	if ((adb_controller == NULL) || (adb_controller->send_request == NULL))
		return -ENXIO;
	if (nbytes < 1)
		return -EINVAL;

	req->nbytes = nbytes+1;
	req->done = done;
	req->reply_expected = flags & ADBREQ_REPLY;
	req->data[0] = ADB_PACKET;
	va_start(list, nbytes);
	for (i = 0; i < nbytes; ++i)
		req->data[i+1] = va_arg(list, int);
	va_end(list);

	if (flags & ADBREQ_NOSEND)
		return 0;

	/* Synchronous requests block using an on-stack completion */
	if (flags & ADBREQ_SYNC) {
		WARN_ON(done);
		req->done = adb_sync_req_done;
		req->arg = &comp;
		init_completion(&comp);
	}

	rc = adb_controller->send_request(req, 0);

	if ((flags & ADBREQ_SYNC) && !rc && !req->complete)
		wait_for_completion(&comp);

	return rc;
}
EXPORT_SYMBOL(adb_request);

 /* Ultimately this should return the number of devices with
    the given default id.
    And it does it now ! Note: changed behaviour: This function
    will now register if default_id _and_ handler_id both match
    but handler_id can be left to 0 to match with default_id only.
    When handler_id is set, this function will try to adjust
    the handler_id id it doesn't match. */
int
adb_register(int default_id, int handler_id, struct adb_ids *ids,
	     void (*handler)(unsigned char *, int, int))
{
	int i;

	mutex_lock(&adb_handler_mutex);
	ids->nids = 0;
	for (i = 1; i < 16; i++) {
		if ((adb_handler[i].original_address == default_id) &&
		    (!handler_id || (handler_id == adb_handler[i].handler_id) || 
		    try_handler_change(i, handler_id))) {
			if (adb_handler[i].handler != 0) {
				pr_err("Two handlers for ADB device %d\n",
				       default_id);
				continue;
			}
			write_lock_irq(&adb_handler_lock);
			adb_handler[i].handler = handler;
			write_unlock_irq(&adb_handler_lock);
			ids->id[ids->nids++] = i;
		}
	}
	mutex_unlock(&adb_handler_mutex);
	return ids->nids;
}
EXPORT_SYMBOL(adb_register);

int
adb_unregister(int index)
{
	int ret = -ENODEV;

	mutex_lock(&adb_handler_mutex);
	write_lock_irq(&adb_handler_lock);
	if (adb_handler[index].handler) {
		while(adb_handler[index].busy) {
			write_unlock_irq(&adb_handler_lock);
			yield();
			write_lock_irq(&adb_handler_lock);
		}
		ret = 0;
		adb_handler[index].handler = NULL;
	}
	write_unlock_irq(&adb_handler_lock);
	mutex_unlock(&adb_handler_mutex);
	return ret;
}
EXPORT_SYMBOL(adb_unregister);

void
adb_input(unsigned char *buf, int nb, int autopoll)
{
	int i, id;
	static int dump_adb_input;
	unsigned long flags;
	
	void (*handler)(unsigned char *, int, int);

	/* We skip keystrokes and mouse moves when the sleep process
	 * has been started. We stop autopoll, but this is another security
	 */
	if (adb_got_sleep)
		return;
		
	id = buf[0] >> 4;
	if (dump_adb_input) {
		pr_info("adb packet: ");
		for (i = 0; i < nb; ++i)
			pr_cont(" %x", buf[i]);
		pr_cont(", id = %d\n", id);
	}
	write_lock_irqsave(&adb_handler_lock, flags);
	handler = adb_handler[id].handler;
	if (handler != NULL)
		adb_handler[id].busy = 1;
	write_unlock_irqrestore(&adb_handler_lock, flags);
	if (handler != NULL) {
		(*handler)(buf, nb, autopoll);
		wmb();
		adb_handler[id].busy = 0;
	}
		
}

/* Try to change handler to new_id. Will return 1 if successful. */
static int try_handler_change(int address, int new_id)
{
	struct adb_request req;

	if (adb_handler[address].handler_id == new_id)
	    return 1;
	adb_request(&req, NULL, ADBREQ_SYNC, 3,
	    ADB_WRITEREG(address, 3), address | 0x20, new_id);
	adb_request(&req, NULL, ADBREQ_SYNC | ADBREQ_REPLY, 1,
	    ADB_READREG(address, 3));
	if (req.reply_len < 2)
	    return 0;
	if (req.reply[2] != new_id)
	    return 0;
	adb_handler[address].handler_id = req.reply[2];

	return 1;
}

int
adb_try_handler_change(int address, int new_id)
{
	int ret;

	mutex_lock(&adb_handler_mutex);
	ret = try_handler_change(address, new_id);
	mutex_unlock(&adb_handler_mutex);
	if (ret)
		pr_debug("adb handler change: [%d] 0x%X\n", address, new_id);
	return ret;
}
EXPORT_SYMBOL(adb_try_handler_change);

int
adb_get_infos(int address, int *original_address, int *handler_id)
{
	mutex_lock(&adb_handler_mutex);
	*original_address = adb_handler[address].original_address;
	*handler_id = adb_handler[address].handler_id;
	mutex_unlock(&adb_handler_mutex);

	return (*original_address != 0);
}


/*
 * /dev/adb device driver.
 */

#define ADB_MAJOR	56	/* major number for /dev/adb */

struct adbdev_state {
	spinlock_t	lock;
	atomic_t	n_pending;
	struct adb_request *completed;
  	wait_queue_head_t wait_queue;
	int		inuse;
};

static void adb_write_done(struct adb_request *req)
{
	struct adbdev_state *state = (struct adbdev_state *) req->arg;
	unsigned long flags;

	if (!req->complete) {
		req->reply_len = 0;
		req->complete = 1;
	}
	spin_lock_irqsave(&state->lock, flags);
	atomic_dec(&state->n_pending);
	if (!state->inuse) {
		kfree(req);
		if (atomic_read(&state->n_pending) == 0) {
			spin_unlock_irqrestore(&state->lock, flags);
			kfree(state);
			return;
		}
	} else {
		struct adb_request **ap = &state->completed;
		while (*ap != NULL)
			ap = &(*ap)->next;
		req->next = NULL;
		*ap = req;
		wake_up_interruptible(&state->wait_queue);
	}
	spin_unlock_irqrestore(&state->lock, flags);
}

static int
do_adb_query(struct adb_request *req)
{
	int	ret = -EINVAL;

	switch(req->data[1]) {
	case ADB_QUERY_GETDEVINFO:
		if (req->nbytes < 3)
			break;
		mutex_lock(&adb_handler_mutex);
		req->reply[0] = adb_handler[req->data[2]].original_address;
		req->reply[1] = adb_handler[req->data[2]].handler_id;
		mutex_unlock(&adb_handler_mutex);
		req->complete = 1;
		req->reply_len = 2;
		adb_write_done(req);
		ret = 0;
		break;
	}
	return ret;
}

static int adb_open(struct inode *inode, struct file *file)
{
	struct adbdev_state *state;
	int ret = 0;

	mutex_lock(&adb_mutex);
	if (iminor(inode) > 0 || adb_controller == NULL) {
		ret = -ENXIO;
		goto out;
	}
	state = kmalloc(sizeof(struct adbdev_state), GFP_KERNEL);
	if (state == 0) {
		ret = -ENOMEM;
		goto out;
	}
	file->private_data = state;
	spin_lock_init(&state->lock);
	atomic_set(&state->n_pending, 0);
	state->completed = NULL;
	init_waitqueue_head(&state->wait_queue);
	state->inuse = 1;

out:
	mutex_unlock(&adb_mutex);
	return ret;
}

static int adb_release(struct inode *inode, struct file *file)
{
	struct adbdev_state *state = file->private_data;
	unsigned long flags;

	mutex_lock(&adb_mutex);
	if (state) {
		file->private_data = NULL;
		spin_lock_irqsave(&state->lock, flags);
		if (atomic_read(&state->n_pending) == 0
		    && state->completed == NULL) {
			spin_unlock_irqrestore(&state->lock, flags);
			kfree(state);
		} else {
			state->inuse = 0;
			spin_unlock_irqrestore(&state->lock, flags);
		}
	}
	mutex_unlock(&adb_mutex);
	return 0;
}

static ssize_t adb_read(struct file *file, char __user *buf,
			size_t count, loff_t *ppos)
{
	int ret = 0;
	struct adbdev_state *state = file->private_data;
	struct adb_request *req;
	DECLARE_WAITQUEUE(wait, current);
	unsigned long flags;

	if (count < 2)
		return -EINVAL;
	if (count > sizeof(req->reply))
		count = sizeof(req->reply);

	req = NULL;
	spin_lock_irqsave(&state->lock, flags);
	add_wait_queue(&state->wait_queue, &wait);
	set_current_state(TASK_INTERRUPTIBLE);

	for (;;) {
		req = state->completed;
		if (req != NULL)
			state->completed = req->next;
		else if (atomic_read(&state->n_pending) == 0)
			ret = -EIO;
		if (req != NULL || ret != 0)
			break;
		
		if (file->f_flags & O_NONBLOCK) {
			ret = -EAGAIN;
			break;
		}
		if (signal_pending(current)) {
			ret = -ERESTARTSYS;
			break;
		}
		spin_unlock_irqrestore(&state->lock, flags);
		schedule();
		spin_lock_irqsave(&state->lock, flags);
	}

	set_current_state(TASK_RUNNING);
	remove_wait_queue(&state->wait_queue, &wait);
	spin_unlock_irqrestore(&state->lock, flags);
	
	if (ret)
		return ret;

	ret = req->reply_len;
	if (ret > count)
		ret = count;
	if (ret > 0 && copy_to_user(buf, req->reply, ret))
		ret = -EFAULT;

	kfree(req);
	return ret;
}

static ssize_t adb_write(struct file *file, const char __user *buf,
			 size_t count, loff_t *ppos)
{
	int ret/*, i*/;
	struct adbdev_state *state = file->private_data;
	struct adb_request *req;

	if (count < 2 || count > sizeof(req->data))
		return -EINVAL;
	if (adb_controller == NULL)
		return -ENXIO;

	req = kmalloc(sizeof(struct adb_request),
					     GFP_KERNEL);
	if (req == NULL)
		return -ENOMEM;

	req->nbytes = count;
	req->done = adb_write_done;
	req->arg = (void *) state;
	req->complete = 0;
	
	ret = -EFAULT;
	if (copy_from_user(req->data, buf, count))
		goto out;

	atomic_inc(&state->n_pending);

	/* If a probe is in progress or we are sleeping, wait for it to complete */
	down(&adb_probe_mutex);

	/* Queries are special requests sent to the ADB driver itself */
	if (req->data[0] == ADB_QUERY) {
		if (count > 1)
			ret = do_adb_query(req);
		else
			ret = -EINVAL;
		up(&adb_probe_mutex);
	}
	/* Special case for ADB_BUSRESET request, all others are sent to
	   the controller */
	else if ((req->data[0] == ADB_PACKET) && (count > 1)
		&& (req->data[1] == ADB_BUSRESET)) {
		ret = do_adb_reset_bus();
		up(&adb_probe_mutex);
		atomic_dec(&state->n_pending);
		if (ret == 0)
			ret = count;
		goto out;
	} else {	
		req->reply_expected = ((req->data[1] & 0xc) == 0xc);
		if (adb_controller && adb_controller->send_request)
			ret = adb_controller->send_request(req, 0);
		else
			ret = -ENXIO;
		up(&adb_probe_mutex);
	}

	if (ret != 0) {
		atomic_dec(&state->n_pending);
		goto out;
	}
	return count;

out:
	kfree(req);
	return ret;
}

static const struct file_operations adb_fops = {
	.owner		= THIS_MODULE,
	.llseek		= no_llseek,
	.read		= adb_read,
	.write		= adb_write,
	.open		= adb_open,
	.release	= adb_release,
};

#ifdef CONFIG_PM
static const struct dev_pm_ops adb_dev_pm_ops = {
	.suspend = adb_suspend,
	.resume = adb_resume,
	/* Hibernate hooks */
	.freeze = adb_freeze,
	.thaw = adb_resume,
	.poweroff = adb_poweroff,
	.restore = adb_resume,
};
#endif

static struct platform_driver adb_pfdrv = {
	.driver = {
		.name = "adb",
#ifdef CONFIG_PM
		.pm = &adb_dev_pm_ops,
#endif
	},
};

static struct platform_device adb_pfdev = {
	.name = "adb",
};

static int __init
adb_dummy_probe(struct platform_device *dev)
{
	if (dev == &adb_pfdev)
		return 0;
	return -ENODEV;
}

static void __init
adbdev_init(void)
{
	if (register_chrdev(ADB_MAJOR, "adb", &adb_fops)) {
		pr_err("adb: unable to get major %d\n", ADB_MAJOR);
		return;
	}

	adb_dev_class = class_create(THIS_MODULE, "adb");
	if (IS_ERR(adb_dev_class))
		return;
	device_create(adb_dev_class, NULL, MKDEV(ADB_MAJOR, 0), NULL, "adb");

	platform_device_register(&adb_pfdev);
	platform_driver_probe(&adb_pfdrv, adb_dummy_probe);
}
