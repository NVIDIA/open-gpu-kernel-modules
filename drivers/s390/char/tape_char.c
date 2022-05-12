// SPDX-License-Identifier: GPL-2.0
/*
 *    character device frontend for tape device driver
 *
 *  S390 and zSeries version
 *    Copyright IBM Corp. 2001, 2006
 *    Author(s): Carsten Otte <cotte@de.ibm.com>
 *		 Michael Holzheu <holzheu@de.ibm.com>
 *		 Tuan Ngo-Anh <ngoanh@de.ibm.com>
 *		 Martin Schwidefsky <schwidefsky@de.ibm.com>
 */

#define KMSG_COMPONENT "tape"
#define pr_fmt(fmt) KMSG_COMPONENT ": " fmt

#include <linux/module.h>
#include <linux/types.h>
#include <linux/proc_fs.h>
#include <linux/mtio.h>
#include <linux/compat.h>

#include <linux/uaccess.h>

#define TAPE_DBF_AREA	tape_core_dbf

#include "tape.h"
#include "tape_std.h"
#include "tape_class.h"

#define TAPECHAR_MAJOR		0	/* get dynamic major */

/*
 * file operation structure for tape character frontend
 */
static ssize_t tapechar_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t tapechar_write(struct file *, const char __user *, size_t, loff_t *);
static int tapechar_open(struct inode *,struct file *);
static int tapechar_release(struct inode *,struct file *);
static long tapechar_ioctl(struct file *, unsigned int, unsigned long);
#ifdef CONFIG_COMPAT
static long tapechar_compat_ioctl(struct file *, unsigned int, unsigned long);
#endif

static const struct file_operations tape_fops =
{
	.owner = THIS_MODULE,
	.read = tapechar_read,
	.write = tapechar_write,
	.unlocked_ioctl = tapechar_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl = tapechar_compat_ioctl,
#endif
	.open = tapechar_open,
	.release = tapechar_release,
	.llseek = no_llseek,
};

static int tapechar_major = TAPECHAR_MAJOR;

/*
 * This function is called for every new tapedevice
 */
int
tapechar_setup_device(struct tape_device * device)
{
	char	device_name[20];

	sprintf(device_name, "ntibm%i", device->first_minor / 2);
	device->nt = register_tape_dev(
		&device->cdev->dev,
		MKDEV(tapechar_major, device->first_minor),
		&tape_fops,
		device_name,
		"non-rewinding"
	);
	device_name[0] = 'r';
	device->rt = register_tape_dev(
		&device->cdev->dev,
		MKDEV(tapechar_major, device->first_minor + 1),
		&tape_fops,
		device_name,
		"rewinding"
	);

	return 0;
}

void
tapechar_cleanup_device(struct tape_device *device)
{
	unregister_tape_dev(&device->cdev->dev, device->rt);
	device->rt = NULL;
	unregister_tape_dev(&device->cdev->dev, device->nt);
	device->nt = NULL;
}

static int
tapechar_check_idalbuffer(struct tape_device *device, size_t block_size)
{
	struct idal_buffer *new;

	if (device->char_data.idal_buf != NULL &&
	    device->char_data.idal_buf->size == block_size)
		return 0;

	if (block_size > MAX_BLOCKSIZE) {
		DBF_EVENT(3, "Invalid blocksize (%zd > %d)\n",
			block_size, MAX_BLOCKSIZE);
		return -EINVAL;
	}

	/* The current idal buffer is not correct. Allocate a new one. */
	new = idal_buffer_alloc(block_size, 0);
	if (IS_ERR(new))
		return -ENOMEM;

	if (device->char_data.idal_buf != NULL)
		idal_buffer_free(device->char_data.idal_buf);

	device->char_data.idal_buf = new;

	return 0;
}

/*
 * Tape device read function
 */
static ssize_t
tapechar_read(struct file *filp, char __user *data, size_t count, loff_t *ppos)
{
	struct tape_device *device;
	struct tape_request *request;
	size_t block_size;
	int rc;

	DBF_EVENT(6, "TCHAR:read\n");
	device = (struct tape_device *) filp->private_data;

	/*
	 * If the tape isn't terminated yet, do it now. And since we then
	 * are at the end of the tape there wouldn't be anything to read
	 * anyways. So we return immediately.
	 */
	if(device->required_tapemarks) {
		return tape_std_terminate_write(device);
	}

	/* Find out block size to use */
	if (device->char_data.block_size != 0) {
		if (count < device->char_data.block_size) {
			DBF_EVENT(3, "TCHAR:read smaller than block "
				  "size was requested\n");
			return -EINVAL;
		}
		block_size = device->char_data.block_size;
	} else {
		block_size = count;
	}

	rc = tapechar_check_idalbuffer(device, block_size);
	if (rc)
		return rc;

	DBF_EVENT(6, "TCHAR:nbytes: %lx\n", block_size);
	/* Let the discipline build the ccw chain. */
	request = device->discipline->read_block(device, block_size);
	if (IS_ERR(request))
		return PTR_ERR(request);
	/* Execute it. */
	rc = tape_do_io(device, request);
	if (rc == 0) {
		rc = block_size - request->rescnt;
		DBF_EVENT(6, "TCHAR:rbytes:  %x\n", rc);
		/* Copy data from idal buffer to user space. */
		if (idal_buffer_to_user(device->char_data.idal_buf,
					data, rc) != 0)
			rc = -EFAULT;
	}
	tape_free_request(request);
	return rc;
}

/*
 * Tape device write function
 */
static ssize_t
tapechar_write(struct file *filp, const char __user *data, size_t count, loff_t *ppos)
{
	struct tape_device *device;
	struct tape_request *request;
	size_t block_size;
	size_t written;
	int nblocks;
	int i, rc;

	DBF_EVENT(6, "TCHAR:write\n");
	device = (struct tape_device *) filp->private_data;
	/* Find out block size and number of blocks */
	if (device->char_data.block_size != 0) {
		if (count < device->char_data.block_size) {
			DBF_EVENT(3, "TCHAR:write smaller than block "
				  "size was requested\n");
			return -EINVAL;
		}
		block_size = device->char_data.block_size;
		nblocks = count / block_size;
	} else {
		block_size = count;
		nblocks = 1;
	}

	rc = tapechar_check_idalbuffer(device, block_size);
	if (rc)
		return rc;

	DBF_EVENT(6,"TCHAR:nbytes: %lx\n", block_size);
	DBF_EVENT(6, "TCHAR:nblocks: %x\n", nblocks);
	/* Let the discipline build the ccw chain. */
	request = device->discipline->write_block(device, block_size);
	if (IS_ERR(request))
		return PTR_ERR(request);
	rc = 0;
	written = 0;
	for (i = 0; i < nblocks; i++) {
		/* Copy data from user space to idal buffer. */
		if (idal_buffer_from_user(device->char_data.idal_buf,
					  data, block_size)) {
			rc = -EFAULT;
			break;
		}
		rc = tape_do_io(device, request);
		if (rc)
			break;
		DBF_EVENT(6, "TCHAR:wbytes: %lx\n",
			  block_size - request->rescnt);
		written += block_size - request->rescnt;
		if (request->rescnt != 0)
			break;
		data += block_size;
	}
	tape_free_request(request);
	if (rc == -ENOSPC) {
		/*
		 * Ok, the device has no more space. It has NOT written
		 * the block.
		 */
		if (device->discipline->process_eov)
			device->discipline->process_eov(device);
		if (written > 0)
			rc = 0;

	}

	/*
	 * After doing a write we always need two tapemarks to correctly
	 * terminate the tape (one to terminate the file, the second to
	 * flag the end of recorded data.
	 * Since process_eov positions the tape in front of the written
	 * tapemark it doesn't hurt to write two marks again.
	 */
	if (!rc)
		device->required_tapemarks = 2;

	return rc ? rc : written;
}

/*
 * Character frontend tape device open function.
 */
static int
tapechar_open (struct inode *inode, struct file *filp)
{
	struct tape_device *device;
	int minor, rc;

	DBF_EVENT(6, "TCHAR:open: %i:%i\n",
		imajor(file_inode(filp)),
		iminor(file_inode(filp)));

	if (imajor(file_inode(filp)) != tapechar_major)
		return -ENODEV;

	minor = iminor(file_inode(filp));
	device = tape_find_device(minor / TAPE_MINORS_PER_DEV);
	if (IS_ERR(device)) {
		DBF_EVENT(3, "TCHAR:open: tape_find_device() failed\n");
		return PTR_ERR(device);
	}

	rc = tape_open(device);
	if (rc == 0) {
		filp->private_data = device;
		stream_open(inode, filp);
	} else
		tape_put_device(device);

	return rc;
}

/*
 * Character frontend tape device release function.
 */

static int
tapechar_release(struct inode *inode, struct file *filp)
{
	struct tape_device *device;

	DBF_EVENT(6, "TCHAR:release: %x\n", iminor(inode));
	device = (struct tape_device *) filp->private_data;

	/*
	 * If this is the rewinding tape minor then rewind. In that case we
	 * write all required tapemarks. Otherwise only one to terminate the
	 * file.
	 */
	if ((iminor(inode) & 1) != 0) {
		if (device->required_tapemarks)
			tape_std_terminate_write(device);
		tape_mtop(device, MTREW, 1);
	} else {
		if (device->required_tapemarks > 1) {
			if (tape_mtop(device, MTWEOF, 1) == 0)
				device->required_tapemarks--;
		}
	}

	if (device->char_data.idal_buf != NULL) {
		idal_buffer_free(device->char_data.idal_buf);
		device->char_data.idal_buf = NULL;
	}
	tape_release(device);
	filp->private_data = NULL;
	tape_put_device(device);

	return 0;
}

/*
 * Tape device io controls.
 */
static int
__tapechar_ioctl(struct tape_device *device,
		 unsigned int no, void __user *data)
{
	int rc;

	if (no == MTIOCTOP) {
		struct mtop op;

		if (copy_from_user(&op, data, sizeof(op)) != 0)
			return -EFAULT;
		if (op.mt_count < 0)
			return -EINVAL;

		/*
		 * Operations that change tape position should write final
		 * tapemarks.
		 */
		switch (op.mt_op) {
			case MTFSF:
			case MTBSF:
			case MTFSR:
			case MTBSR:
			case MTREW:
			case MTOFFL:
			case MTEOM:
			case MTRETEN:
			case MTBSFM:
			case MTFSFM:
			case MTSEEK:
				if (device->required_tapemarks)
					tape_std_terminate_write(device);
			default:
				;
		}
		rc = tape_mtop(device, op.mt_op, op.mt_count);

		if (op.mt_op == MTWEOF && rc == 0) {
			if (op.mt_count > device->required_tapemarks)
				device->required_tapemarks = 0;
			else
				device->required_tapemarks -= op.mt_count;
		}
		return rc;
	}
	if (no == MTIOCPOS) {
		/* MTIOCPOS: query the tape position. */
		struct mtpos pos;

		rc = tape_mtop(device, MTTELL, 1);
		if (rc < 0)
			return rc;
		pos.mt_blkno = rc;
		return put_user_mtpos(data, &pos);
	}
	if (no == MTIOCGET) {
		/* MTIOCGET: query the tape drive status. */
		struct mtget get;

		memset(&get, 0, sizeof(get));
		get.mt_type = MT_ISUNKNOWN;
		get.mt_resid = 0 /* device->devstat.rescnt */;
		get.mt_dsreg =
			((device->char_data.block_size << MT_ST_BLKSIZE_SHIFT)
			 & MT_ST_BLKSIZE_MASK);
		/* FIXME: mt_gstat, mt_erreg, mt_fileno */
		get.mt_gstat = 0;
		get.mt_erreg = 0;
		get.mt_fileno = 0;
		get.mt_gstat  = device->tape_generic_status;

		if (device->medium_state == MS_LOADED) {
			rc = tape_mtop(device, MTTELL, 1);

			if (rc < 0)
				return rc;

			if (rc == 0)
				get.mt_gstat |= GMT_BOT(~0);

			get.mt_blkno = rc;
		}

		return put_user_mtget(data, &get);
	}
	/* Try the discipline ioctl function. */
	if (device->discipline->ioctl_fn == NULL)
		return -EINVAL;
	return device->discipline->ioctl_fn(device, no, (unsigned long)data);
}

static long
tapechar_ioctl(struct file *filp, unsigned int no, unsigned long data)
{
	struct tape_device *device;
	long rc;

	DBF_EVENT(6, "TCHAR:ioct\n");

	device = (struct tape_device *) filp->private_data;
	mutex_lock(&device->mutex);
	rc = __tapechar_ioctl(device, no, (void __user *)data);
	mutex_unlock(&device->mutex);
	return rc;
}

#ifdef CONFIG_COMPAT
static long
tapechar_compat_ioctl(struct file *filp, unsigned int no, unsigned long data)
{
	struct tape_device *device = filp->private_data;
	long rc;

	if (no == MTIOCPOS32)
		no = MTIOCPOS;
	else if (no == MTIOCGET32)
		no = MTIOCGET;

	mutex_lock(&device->mutex);
	rc = __tapechar_ioctl(device, no, compat_ptr(data));
	mutex_unlock(&device->mutex);
	return rc;
}
#endif /* CONFIG_COMPAT */

/*
 * Initialize character device frontend.
 */
int
tapechar_init (void)
{
	dev_t	dev;

	if (alloc_chrdev_region(&dev, 0, 256, "tape") != 0)
		return -1;

	tapechar_major = MAJOR(dev);

	return 0;
}

/*
 * cleanup
 */
void
tapechar_exit(void)
{
	unregister_chrdev_region(MKDEV(tapechar_major, 0), 256);
}
