// SPDX-License-Identifier: GPL-2.0-only
/*
   SCSI Tape Driver for Linux version 1.1 and newer. See the accompanying
   file Documentation/scsi/st.rst for more information.

   History:
   Rewritten from Dwayne Forsyth's SCSI tape driver by Kai Makisara.
   Contribution and ideas from several people including (in alphabetical
   order) Klaus Ehrenfried, Eugene Exarevsky, Eric Lee Green, Wolfgang Denk,
   Steve Hirsch, Andreas Koppenh"ofer, Michael Leodolter, Eyal Lebedinsky,
   Michael Schaefer, J"org Weule, and Eric Youngdale.

   Copyright 1992 - 2016 Kai Makisara
   email Kai.Makisara@kolumbus.fi

   Some small formal changes - aeb, 950809

   Last modified: 18-JAN-1998 Richard Gooch <rgooch@atnf.csiro.au> Devfs support
 */

static const char *verstr = "20160209";

#include <linux/module.h>

#include <linux/compat.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/sched/signal.h>
#include <linux/mm.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/mtio.h>
#include <linux/cdrom.h>
#include <linux/ioctl.h>
#include <linux/fcntl.h>
#include <linux/spinlock.h>
#include <linux/blkdev.h>
#include <linux/moduleparam.h>
#include <linux/cdev.h>
#include <linux/idr.h>
#include <linux/delay.h>
#include <linux/mutex.h>

#include <linux/uaccess.h>
#include <asm/dma.h>
#include <asm/unaligned.h>

#include <scsi/scsi.h>
#include <scsi/scsi_dbg.h>
#include <scsi/scsi_device.h>
#include <scsi/scsi_driver.h>
#include <scsi/scsi_eh.h>
#include <scsi/scsi_host.h>
#include <scsi/scsi_ioctl.h>
#include <scsi/sg.h>


/* The driver prints some debugging information on the console if DEBUG
   is defined and non-zero. */
#define DEBUG 1
#define NO_DEBUG 0

#define ST_DEB_MSG  KERN_NOTICE
#if DEBUG
/* The message level for the debug messages is currently set to KERN_NOTICE
   so that people can easily see the messages. Later when the debugging messages
   in the drivers are more widely classified, this may be changed to KERN_DEBUG. */
#define DEB(a) a
#define DEBC(a) if (debugging) { a ; }
#else
#define DEB(a)
#define DEBC(a)
#endif

#define ST_KILOBYTE 1024

#include "st_options.h"
#include "st.h"

static int buffer_kbs;
static int max_sg_segs;
static int try_direct_io = TRY_DIRECT_IO;
static int try_rdio = 1;
static int try_wdio = 1;
static int debug_flag;

static struct class st_sysfs_class;
static const struct attribute_group *st_dev_groups[];
static const struct attribute_group *st_drv_groups[];

MODULE_AUTHOR("Kai Makisara");
MODULE_DESCRIPTION("SCSI tape (st) driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS_CHARDEV_MAJOR(SCSI_TAPE_MAJOR);
MODULE_ALIAS_SCSI_DEVICE(TYPE_TAPE);

/* Set 'perm' (4th argument) to 0 to disable module_param's definition
 * of sysfs parameters (which module_param doesn't yet support).
 * Sysfs parameters defined explicitly later.
 */
module_param_named(buffer_kbs, buffer_kbs, int, 0);
MODULE_PARM_DESC(buffer_kbs, "Default driver buffer size for fixed block mode (KB; 32)");
module_param_named(max_sg_segs, max_sg_segs, int, 0);
MODULE_PARM_DESC(max_sg_segs, "Maximum number of scatter/gather segments to use (256)");
module_param_named(try_direct_io, try_direct_io, int, 0);
MODULE_PARM_DESC(try_direct_io, "Try direct I/O between user buffer and tape drive (1)");
module_param_named(debug_flag, debug_flag, int, 0);
MODULE_PARM_DESC(debug_flag, "Enable DEBUG, same as setting debugging=1");


/* Extra parameters for testing */
module_param_named(try_rdio, try_rdio, int, 0);
MODULE_PARM_DESC(try_rdio, "Try direct read i/o when possible");
module_param_named(try_wdio, try_wdio, int, 0);
MODULE_PARM_DESC(try_wdio, "Try direct write i/o when possible");

#ifndef MODULE
static int write_threshold_kbs;  /* retained for compatibility */
static struct st_dev_parm {
	char *name;
	int *val;
} parms[] __initdata = {
	{
		"buffer_kbs", &buffer_kbs
	},
	{       /* Retained for compatibility with 2.4 */
		"write_threshold_kbs", &write_threshold_kbs
	},
	{
		"max_sg_segs", NULL
	},
	{
		"try_direct_io", &try_direct_io
	},
	{
		"debug_flag", &debug_flag
	}
};
#endif

/* Restrict the number of modes so that names for all are assigned */
#if ST_NBR_MODES > 16
#error "Maximum number of modes is 16"
#endif
/* Bit reversed order to get same names for same minors with all
   mode counts */
static const char *st_formats[] = {
	"",  "r", "k", "s", "l", "t", "o", "u",
	"m", "v", "p", "x", "a", "y", "q", "z"}; 

/* The default definitions have been moved to st_options.h */

#define ST_FIXED_BUFFER_SIZE (ST_FIXED_BUFFER_BLOCKS * ST_KILOBYTE)

/* The buffer size should fit into the 24 bits for length in the
   6-byte SCSI read and write commands. */
#if ST_FIXED_BUFFER_SIZE >= (2 << 24 - 1)
#error "Buffer size should not exceed (2 << 24 - 1) bytes!"
#endif

static int debugging = DEBUG;

#define MAX_RETRIES 0
#define MAX_WRITE_RETRIES 0
#define MAX_READY_RETRIES 0
#define NO_TAPE  NOT_READY

#define ST_TIMEOUT (900 * HZ)
#define ST_LONG_TIMEOUT (14000 * HZ)

/* Remove mode bits and auto-rewind bit (7) */
#define TAPE_NR(x) ( ((iminor(x) & ~255) >> (ST_NBR_MODE_BITS + 1)) | \
	(iminor(x) & ((1 << ST_MODE_SHIFT)-1)))
#define TAPE_MODE(x) ((iminor(x) & ST_MODE_MASK) >> ST_MODE_SHIFT)

/* Construct the minor number from the device (d), mode (m), and non-rewind (n) data */
#define TAPE_MINOR(d, m, n) (((d & ~(255 >> (ST_NBR_MODE_BITS + 1))) << (ST_NBR_MODE_BITS + 1)) | \
  (d & (255 >> (ST_NBR_MODE_BITS + 1))) | (m << ST_MODE_SHIFT) | ((n != 0) << 7) )

/* Internal ioctl to set both density (uppermost 8 bits) and blocksize (lower
   24 bits) */
#define SET_DENS_AND_BLK 0x10001

static int st_fixed_buffer_size = ST_FIXED_BUFFER_SIZE;
static int st_max_sg_segs = ST_MAX_SG;

static int modes_defined;

static int enlarge_buffer(struct st_buffer *, int);
static void clear_buffer(struct st_buffer *);
static void normalize_buffer(struct st_buffer *);
static int append_to_buffer(const char __user *, struct st_buffer *, int);
static int from_buffer(struct st_buffer *, char __user *, int);
static void move_buffer_data(struct st_buffer *, int);

static int sgl_map_user_pages(struct st_buffer *, const unsigned int,
			      unsigned long, size_t, int);
static int sgl_unmap_user_pages(struct st_buffer *, const unsigned int, int);

static int st_probe(struct device *);
static int st_remove(struct device *);

static struct scsi_driver st_template = {
	.gendrv = {
		.name		= "st",
		.owner		= THIS_MODULE,
		.probe		= st_probe,
		.remove		= st_remove,
		.groups		= st_drv_groups,
	},
};

static int st_compression(struct scsi_tape *, int);

static int find_partition(struct scsi_tape *);
static int switch_partition(struct scsi_tape *);

static int st_int_ioctl(struct scsi_tape *, unsigned int, unsigned long);

static void scsi_tape_release(struct kref *);

#define to_scsi_tape(obj) container_of(obj, struct scsi_tape, kref)

static DEFINE_MUTEX(st_ref_mutex);
static DEFINE_SPINLOCK(st_index_lock);
static DEFINE_SPINLOCK(st_use_lock);
static DEFINE_IDR(st_index_idr);



#ifndef SIGS_FROM_OSST
#define SIGS_FROM_OSST \
	{"OnStream", "SC-", "", "osst"}, \
	{"OnStream", "DI-", "", "osst"}, \
	{"OnStream", "DP-", "", "osst"}, \
	{"OnStream", "USB", "", "osst"}, \
	{"OnStream", "FW-", "", "osst"}
#endif

static struct scsi_tape *scsi_tape_get(int dev)
{
	struct scsi_tape *STp = NULL;

	mutex_lock(&st_ref_mutex);
	spin_lock(&st_index_lock);

	STp = idr_find(&st_index_idr, dev);
	if (!STp) goto out;

	kref_get(&STp->kref);

	if (!STp->device)
		goto out_put;

	if (scsi_device_get(STp->device))
		goto out_put;

	goto out;

out_put:
	kref_put(&STp->kref, scsi_tape_release);
	STp = NULL;
out:
	spin_unlock(&st_index_lock);
	mutex_unlock(&st_ref_mutex);
	return STp;
}

static void scsi_tape_put(struct scsi_tape *STp)
{
	struct scsi_device *sdev = STp->device;

	mutex_lock(&st_ref_mutex);
	kref_put(&STp->kref, scsi_tape_release);
	scsi_device_put(sdev);
	mutex_unlock(&st_ref_mutex);
}

struct st_reject_data {
	char *vendor;
	char *model;
	char *rev;
	char *driver_hint; /* Name of the correct driver, NULL if unknown */
};

static struct st_reject_data reject_list[] = {
	/* {"XXX", "Yy-", "", NULL},  example */
	SIGS_FROM_OSST,
	{NULL, }};

/* If the device signature is on the list of incompatible drives, the
   function returns a pointer to the name of the correct driver (if known) */
static char * st_incompatible(struct scsi_device* SDp)
{
	struct st_reject_data *rp;

	for (rp=&(reject_list[0]); rp->vendor != NULL; rp++)
		if (!strncmp(rp->vendor, SDp->vendor, strlen(rp->vendor)) &&
		    !strncmp(rp->model, SDp->model, strlen(rp->model)) &&
		    !strncmp(rp->rev, SDp->rev, strlen(rp->rev))) {
			if (rp->driver_hint)
				return rp->driver_hint;
			else
				return "unknown";
		}
	return NULL;
}


static inline char *tape_name(struct scsi_tape *tape)
{
	return tape->disk->disk_name;
}

#define st_printk(prefix, t, fmt, a...) \
	sdev_prefix_printk(prefix, (t)->device, tape_name(t), fmt, ##a)
#ifdef DEBUG
#define DEBC_printk(t, fmt, a...) \
	if (debugging) { st_printk(ST_DEB_MSG, t, fmt, ##a ); }
#else
#define DEBC_printk(t, fmt, a...)
#endif

static void st_analyze_sense(struct st_request *SRpnt, struct st_cmdstatus *s)
{
	const u8 *ucp;
	const u8 *sense = SRpnt->sense;

	s->have_sense = scsi_normalize_sense(SRpnt->sense,
				SCSI_SENSE_BUFFERSIZE, &s->sense_hdr);
	s->flags = 0;

	if (s->have_sense) {
		s->deferred = 0;
		s->remainder_valid =
			scsi_get_sense_info_fld(sense, SCSI_SENSE_BUFFERSIZE, &s->uremainder64);
		switch (sense[0] & 0x7f) {
		case 0x71:
			s->deferred = 1;
			fallthrough;
		case 0x70:
			s->fixed_format = 1;
			s->flags = sense[2] & 0xe0;
			break;
		case 0x73:
			s->deferred = 1;
			fallthrough;
		case 0x72:
			s->fixed_format = 0;
			ucp = scsi_sense_desc_find(sense, SCSI_SENSE_BUFFERSIZE, 4);
			s->flags = ucp ? (ucp[3] & 0xe0) : 0;
			break;
		}
	}
}


/* Convert the result to success code */
static int st_chk_result(struct scsi_tape *STp, struct st_request * SRpnt)
{
	int result = SRpnt->result;
	u8 scode;
	DEB(const char *stp;)
	char *name = tape_name(STp);
	struct st_cmdstatus *cmdstatp;

	if (!result)
		return 0;

	cmdstatp = &STp->buffer->cmdstat;
	st_analyze_sense(SRpnt, cmdstatp);

	if (cmdstatp->have_sense)
		scode = STp->buffer->cmdstat.sense_hdr.sense_key;
	else
		scode = 0;

	DEB(
	if (debugging) {
		st_printk(ST_DEB_MSG, STp,
			    "Error: %x, cmd: %x %x %x %x %x %x\n", result,
			    SRpnt->cmd[0], SRpnt->cmd[1], SRpnt->cmd[2],
			    SRpnt->cmd[3], SRpnt->cmd[4], SRpnt->cmd[5]);
		if (cmdstatp->have_sense)
			__scsi_print_sense(STp->device, name,
					   SRpnt->sense, SCSI_SENSE_BUFFERSIZE);
	} ) /* end DEB */
	if (!debugging) { /* Abnormal conditions for tape */
		if (!cmdstatp->have_sense)
			st_printk(KERN_WARNING, STp,
			       "Error %x (driver bt 0x%x, host bt 0x%x).\n",
			       result, driver_byte(result), host_byte(result));
		else if (cmdstatp->have_sense &&
			 scode != NO_SENSE &&
			 scode != RECOVERED_ERROR &&
			 /* scode != UNIT_ATTENTION && */
			 scode != BLANK_CHECK &&
			 scode != VOLUME_OVERFLOW &&
			 SRpnt->cmd[0] != MODE_SENSE &&
			 SRpnt->cmd[0] != TEST_UNIT_READY) {

			__scsi_print_sense(STp->device, name,
					   SRpnt->sense, SCSI_SENSE_BUFFERSIZE);
		}
	}

	if (cmdstatp->fixed_format &&
	    STp->cln_mode >= EXTENDED_SENSE_START) {  /* Only fixed format sense */
		if (STp->cln_sense_value)
			STp->cleaning_req |= ((SRpnt->sense[STp->cln_mode] &
					       STp->cln_sense_mask) == STp->cln_sense_value);
		else
			STp->cleaning_req |= ((SRpnt->sense[STp->cln_mode] &
					       STp->cln_sense_mask) != 0);
	}
	if (cmdstatp->have_sense &&
	    cmdstatp->sense_hdr.asc == 0 && cmdstatp->sense_hdr.ascq == 0x17)
		STp->cleaning_req = 1; /* ASC and ASCQ => cleaning requested */

	STp->pos_unknown |= STp->device->was_reset;

	if (cmdstatp->have_sense &&
	    scode == RECOVERED_ERROR
#if ST_RECOVERED_WRITE_FATAL
	    && SRpnt->cmd[0] != WRITE_6
	    && SRpnt->cmd[0] != WRITE_FILEMARKS
#endif
	    ) {
		STp->recover_count++;
		STp->recover_reg++;

		DEB(
		if (debugging) {
			if (SRpnt->cmd[0] == READ_6)
				stp = "read";
			else if (SRpnt->cmd[0] == WRITE_6)
				stp = "write";
			else
				stp = "ioctl";
			st_printk(ST_DEB_MSG, STp,
				  "Recovered %s error (%d).\n",
				  stp, STp->recover_count);
		} ) /* end DEB */

		if (cmdstatp->flags == 0)
			return 0;
	}
	return (-EIO);
}

static struct st_request *st_allocate_request(struct scsi_tape *stp)
{
	struct st_request *streq;

	streq = kzalloc(sizeof(*streq), GFP_KERNEL);
	if (streq)
		streq->stp = stp;
	else {
		st_printk(KERN_ERR, stp,
			  "Can't get SCSI request.\n");
		if (signal_pending(current))
			stp->buffer->syscall_result = -EINTR;
		else
			stp->buffer->syscall_result = -EBUSY;
	}

	return streq;
}

static void st_release_request(struct st_request *streq)
{
	kfree(streq);
}

static void st_do_stats(struct scsi_tape *STp, struct request *req)
{
	ktime_t now;

	now = ktime_get();
	if (scsi_req(req)->cmd[0] == WRITE_6) {
		now = ktime_sub(now, STp->stats->write_time);
		atomic64_add(ktime_to_ns(now), &STp->stats->tot_write_time);
		atomic64_add(ktime_to_ns(now), &STp->stats->tot_io_time);
		atomic64_inc(&STp->stats->write_cnt);
		if (scsi_req(req)->result) {
			atomic64_add(atomic_read(&STp->stats->last_write_size)
				- STp->buffer->cmdstat.residual,
				&STp->stats->write_byte_cnt);
			if (STp->buffer->cmdstat.residual > 0)
				atomic64_inc(&STp->stats->resid_cnt);
		} else
			atomic64_add(atomic_read(&STp->stats->last_write_size),
				&STp->stats->write_byte_cnt);
	} else if (scsi_req(req)->cmd[0] == READ_6) {
		now = ktime_sub(now, STp->stats->read_time);
		atomic64_add(ktime_to_ns(now), &STp->stats->tot_read_time);
		atomic64_add(ktime_to_ns(now), &STp->stats->tot_io_time);
		atomic64_inc(&STp->stats->read_cnt);
		if (scsi_req(req)->result) {
			atomic64_add(atomic_read(&STp->stats->last_read_size)
				- STp->buffer->cmdstat.residual,
				&STp->stats->read_byte_cnt);
			if (STp->buffer->cmdstat.residual > 0)
				atomic64_inc(&STp->stats->resid_cnt);
		} else
			atomic64_add(atomic_read(&STp->stats->last_read_size),
				&STp->stats->read_byte_cnt);
	} else {
		now = ktime_sub(now, STp->stats->other_time);
		atomic64_add(ktime_to_ns(now), &STp->stats->tot_io_time);
		atomic64_inc(&STp->stats->other_cnt);
	}
	atomic64_dec(&STp->stats->in_flight);
}

static void st_scsi_execute_end(struct request *req, blk_status_t status)
{
	struct st_request *SRpnt = req->end_io_data;
	struct scsi_request *rq = scsi_req(req);
	struct scsi_tape *STp = SRpnt->stp;
	struct bio *tmp;

	STp->buffer->cmdstat.midlevel_result = SRpnt->result = rq->result;
	STp->buffer->cmdstat.residual = rq->resid_len;

	st_do_stats(STp, req);

	tmp = SRpnt->bio;
	if (rq->sense_len)
		memcpy(SRpnt->sense, rq->sense, SCSI_SENSE_BUFFERSIZE);
	if (SRpnt->waiting)
		complete(SRpnt->waiting);

	blk_rq_unmap_user(tmp);
	blk_put_request(req);
}

static int st_scsi_execute(struct st_request *SRpnt, const unsigned char *cmd,
			   int data_direction, void *buffer, unsigned bufflen,
			   int timeout, int retries)
{
	struct request *req;
	struct scsi_request *rq;
	struct rq_map_data *mdata = &SRpnt->stp->buffer->map_data;
	int err = 0;
	struct scsi_tape *STp = SRpnt->stp;

	req = blk_get_request(SRpnt->stp->device->request_queue,
			data_direction == DMA_TO_DEVICE ?
			REQ_OP_SCSI_OUT : REQ_OP_SCSI_IN, 0);
	if (IS_ERR(req))
		return DRIVER_ERROR << 24;
	rq = scsi_req(req);
	req->rq_flags |= RQF_QUIET;

	mdata->null_mapped = 1;

	if (bufflen) {
		err = blk_rq_map_user(req->q, req, mdata, NULL, bufflen,
				      GFP_KERNEL);
		if (err) {
			blk_put_request(req);
			return DRIVER_ERROR << 24;
		}
	}

	atomic64_inc(&STp->stats->in_flight);
	if (cmd[0] == WRITE_6) {
		atomic_set(&STp->stats->last_write_size, bufflen);
		STp->stats->write_time = ktime_get();
	} else if (cmd[0] == READ_6) {
		atomic_set(&STp->stats->last_read_size, bufflen);
		STp->stats->read_time = ktime_get();
	} else {
		STp->stats->other_time = ktime_get();
	}

	SRpnt->bio = req->bio;
	rq->cmd_len = COMMAND_SIZE(cmd[0]);
	memset(rq->cmd, 0, BLK_MAX_CDB);
	memcpy(rq->cmd, cmd, rq->cmd_len);
	req->timeout = timeout;
	rq->retries = retries;
	req->end_io_data = SRpnt;

	blk_execute_rq_nowait(NULL, req, 1, st_scsi_execute_end);
	return 0;
}

/* Do the scsi command. Waits until command performed if do_wait is true.
   Otherwise write_behind_check() is used to check that the command
   has finished. */
static struct st_request *
st_do_scsi(struct st_request * SRpnt, struct scsi_tape * STp, unsigned char *cmd,
	   int bytes, int direction, int timeout, int retries, int do_wait)
{
	struct completion *waiting;
	struct rq_map_data *mdata = &STp->buffer->map_data;
	int ret;

	/* if async, make sure there's no command outstanding */
	if (!do_wait && ((STp->buffer)->last_SRpnt)) {
		st_printk(KERN_ERR, STp,
			  "Async command already active.\n");
		if (signal_pending(current))
			(STp->buffer)->syscall_result = (-EINTR);
		else
			(STp->buffer)->syscall_result = (-EBUSY);
		return NULL;
	}

	if (!SRpnt) {
		SRpnt = st_allocate_request(STp);
		if (!SRpnt)
			return NULL;
	}

	/* If async IO, set last_SRpnt. This ptr tells write_behind_check
	   which IO is outstanding. It's nulled out when the IO completes. */
	if (!do_wait)
		(STp->buffer)->last_SRpnt = SRpnt;

	waiting = &STp->wait;
	init_completion(waiting);
	SRpnt->waiting = waiting;

	if (STp->buffer->do_dio) {
		mdata->page_order = 0;
		mdata->nr_entries = STp->buffer->sg_segs;
		mdata->pages = STp->buffer->mapped_pages;
	} else {
		mdata->page_order = STp->buffer->reserved_page_order;
		mdata->nr_entries =
			DIV_ROUND_UP(bytes, PAGE_SIZE << mdata->page_order);
		mdata->pages = STp->buffer->reserved_pages;
		mdata->offset = 0;
	}

	memcpy(SRpnt->cmd, cmd, sizeof(SRpnt->cmd));
	STp->buffer->cmdstat.have_sense = 0;
	STp->buffer->syscall_result = 0;

	ret = st_scsi_execute(SRpnt, cmd, direction, NULL, bytes, timeout,
			      retries);
	if (ret) {
		/* could not allocate the buffer or request was too large */
		(STp->buffer)->syscall_result = (-EBUSY);
		(STp->buffer)->last_SRpnt = NULL;
	} else if (do_wait) {
		wait_for_completion(waiting);
		SRpnt->waiting = NULL;
		(STp->buffer)->syscall_result = st_chk_result(STp, SRpnt);
	}

	return SRpnt;
}


/* Handle the write-behind checking (waits for completion). Returns -ENOSPC if
   write has been correct but EOM early warning reached, -EIO if write ended in
   error or zero if write successful. Asynchronous writes are used only in
   variable block mode. */
static int write_behind_check(struct scsi_tape * STp)
{
	int retval = 0;
	struct st_buffer *STbuffer;
	struct st_partstat *STps;
	struct st_cmdstatus *cmdstatp;
	struct st_request *SRpnt;

	STbuffer = STp->buffer;
	if (!STbuffer->writing)
		return 0;

	DEB(
	if (STp->write_pending)
		STp->nbr_waits++;
	else
		STp->nbr_finished++;
	) /* end DEB */

	wait_for_completion(&(STp->wait));
	SRpnt = STbuffer->last_SRpnt;
	STbuffer->last_SRpnt = NULL;
	SRpnt->waiting = NULL;

	(STp->buffer)->syscall_result = st_chk_result(STp, SRpnt);
	st_release_request(SRpnt);

	STbuffer->buffer_bytes -= STbuffer->writing;
	STps = &(STp->ps[STp->partition]);
	if (STps->drv_block >= 0) {
		if (STp->block_size == 0)
			STps->drv_block++;
		else
			STps->drv_block += STbuffer->writing / STp->block_size;
	}

	cmdstatp = &STbuffer->cmdstat;
	if (STbuffer->syscall_result) {
		retval = -EIO;
		if (cmdstatp->have_sense && !cmdstatp->deferred &&
		    (cmdstatp->flags & SENSE_EOM) &&
		    (cmdstatp->sense_hdr.sense_key == NO_SENSE ||
		     cmdstatp->sense_hdr.sense_key == RECOVERED_ERROR)) {
			/* EOM at write-behind, has all data been written? */
			if (!cmdstatp->remainder_valid ||
			    cmdstatp->uremainder64 == 0)
				retval = -ENOSPC;
		}
		if (retval == -EIO)
			STps->drv_block = -1;
	}
	STbuffer->writing = 0;

	DEB(if (debugging && retval)
		    st_printk(ST_DEB_MSG, STp,
				"Async write error %x, return value %d.\n",
				STbuffer->cmdstat.midlevel_result, retval);) /* end DEB */

	return retval;
}


/* Step over EOF if it has been inadvertently crossed (ioctl not used because
   it messes up the block number). */
static int cross_eof(struct scsi_tape * STp, int forward)
{
	struct st_request *SRpnt;
	unsigned char cmd[MAX_COMMAND_SIZE];

	cmd[0] = SPACE;
	cmd[1] = 0x01;		/* Space FileMarks */
	if (forward) {
		cmd[2] = cmd[3] = 0;
		cmd[4] = 1;
	} else
		cmd[2] = cmd[3] = cmd[4] = 0xff;	/* -1 filemarks */
	cmd[5] = 0;

	DEBC_printk(STp, "Stepping over filemark %s.\n",
		    forward ? "forward" : "backward");

	SRpnt = st_do_scsi(NULL, STp, cmd, 0, DMA_NONE,
			   STp->device->request_queue->rq_timeout,
			   MAX_RETRIES, 1);
	if (!SRpnt)
		return (STp->buffer)->syscall_result;

	st_release_request(SRpnt);
	SRpnt = NULL;

	if ((STp->buffer)->cmdstat.midlevel_result != 0)
		st_printk(KERN_ERR, STp,
			  "Stepping over filemark %s failed.\n",
			  forward ? "forward" : "backward");

	return (STp->buffer)->syscall_result;
}


/* Flush the write buffer (never need to write if variable blocksize). */
static int st_flush_write_buffer(struct scsi_tape * STp)
{
	int transfer, blks;
	int result;
	unsigned char cmd[MAX_COMMAND_SIZE];
	struct st_request *SRpnt;
	struct st_partstat *STps;

	result = write_behind_check(STp);
	if (result)
		return result;

	result = 0;
	if (STp->dirty == 1) {

		transfer = STp->buffer->buffer_bytes;
		DEBC_printk(STp, "Flushing %d bytes.\n", transfer);

		memset(cmd, 0, MAX_COMMAND_SIZE);
		cmd[0] = WRITE_6;
		cmd[1] = 1;
		blks = transfer / STp->block_size;
		cmd[2] = blks >> 16;
		cmd[3] = blks >> 8;
		cmd[4] = blks;

		SRpnt = st_do_scsi(NULL, STp, cmd, transfer, DMA_TO_DEVICE,
				   STp->device->request_queue->rq_timeout,
				   MAX_WRITE_RETRIES, 1);
		if (!SRpnt)
			return (STp->buffer)->syscall_result;

		STps = &(STp->ps[STp->partition]);
		if ((STp->buffer)->syscall_result != 0) {
			struct st_cmdstatus *cmdstatp = &STp->buffer->cmdstat;

			if (cmdstatp->have_sense && !cmdstatp->deferred &&
			    (cmdstatp->flags & SENSE_EOM) &&
			    (cmdstatp->sense_hdr.sense_key == NO_SENSE ||
			     cmdstatp->sense_hdr.sense_key == RECOVERED_ERROR) &&
			    (!cmdstatp->remainder_valid ||
			     cmdstatp->uremainder64 == 0)) { /* All written at EOM early warning */
				STp->dirty = 0;
				(STp->buffer)->buffer_bytes = 0;
				if (STps->drv_block >= 0)
					STps->drv_block += blks;
				result = (-ENOSPC);
			} else {
				st_printk(KERN_ERR, STp, "Error on flush.\n");
				STps->drv_block = (-1);
				result = (-EIO);
			}
		} else {
			if (STps->drv_block >= 0)
				STps->drv_block += blks;
			STp->dirty = 0;
			(STp->buffer)->buffer_bytes = 0;
		}
		st_release_request(SRpnt);
		SRpnt = NULL;
	}
	return result;
}


/* Flush the tape buffer. The tape will be positioned correctly unless
   seek_next is true. */
static int flush_buffer(struct scsi_tape *STp, int seek_next)
{
	int backspace, result;
	struct st_partstat *STps;

	/*
	 * If there was a bus reset, block further access
	 * to this device.
	 */
	if (STp->pos_unknown)
		return (-EIO);

	if (STp->ready != ST_READY)
		return 0;
	STps = &(STp->ps[STp->partition]);
	if (STps->rw == ST_WRITING)	/* Writing */
		return st_flush_write_buffer(STp);

	if (STp->block_size == 0)
		return 0;

	backspace = ((STp->buffer)->buffer_bytes +
		     (STp->buffer)->read_pointer) / STp->block_size -
	    ((STp->buffer)->read_pointer + STp->block_size - 1) /
	    STp->block_size;
	(STp->buffer)->buffer_bytes = 0;
	(STp->buffer)->read_pointer = 0;
	result = 0;
	if (!seek_next) {
		if (STps->eof == ST_FM_HIT) {
			result = cross_eof(STp, 0);	/* Back over the EOF hit */
			if (!result)
				STps->eof = ST_NOEOF;
			else {
				if (STps->drv_file >= 0)
					STps->drv_file++;
				STps->drv_block = 0;
			}
		}
		if (!result && backspace > 0)
			result = st_int_ioctl(STp, MTBSR, backspace);
	} else if (STps->eof == ST_FM_HIT) {
		if (STps->drv_file >= 0)
			STps->drv_file++;
		STps->drv_block = 0;
		STps->eof = ST_NOEOF;
	}
	return result;

}

/* Set the mode parameters */
static int set_mode_densblk(struct scsi_tape * STp, struct st_modedef * STm)
{
	int set_it = 0;
	unsigned long arg;

	if (!STp->density_changed &&
	    STm->default_density >= 0 &&
	    STm->default_density != STp->density) {
		arg = STm->default_density;
		set_it = 1;
	} else
		arg = STp->density;
	arg <<= MT_ST_DENSITY_SHIFT;
	if (!STp->blksize_changed &&
	    STm->default_blksize >= 0 &&
	    STm->default_blksize != STp->block_size) {
		arg |= STm->default_blksize;
		set_it = 1;
	} else
		arg |= STp->block_size;
	if (set_it &&
	    st_int_ioctl(STp, SET_DENS_AND_BLK, arg)) {
		st_printk(KERN_WARNING, STp,
			  "Can't set default block size to %d bytes "
			  "and density %x.\n",
			  STm->default_blksize, STm->default_density);
		if (modes_defined)
			return (-EINVAL);
	}
	return 0;
}


/* Lock or unlock the drive door. Don't use when st_request allocated. */
static int do_door_lock(struct scsi_tape * STp, int do_lock)
{
	int retval;

	DEBC_printk(STp, "%socking drive door.\n", do_lock ? "L" : "Unl");

	retval = scsi_set_medium_removal(STp->device,
			do_lock ? SCSI_REMOVAL_PREVENT : SCSI_REMOVAL_ALLOW);
	if (!retval)
		STp->door_locked = do_lock ? ST_LOCKED_EXPLICIT : ST_UNLOCKED;
	else
		STp->door_locked = ST_LOCK_FAILS;
	return retval;
}


/* Set the internal state after reset */
static void reset_state(struct scsi_tape *STp)
{
	int i;
	struct st_partstat *STps;

	STp->pos_unknown = 0;
	for (i = 0; i < ST_NBR_PARTITIONS; i++) {
		STps = &(STp->ps[i]);
		STps->rw = ST_IDLE;
		STps->eof = ST_NOEOF;
		STps->at_sm = 0;
		STps->last_block_valid = 0;
		STps->drv_block = -1;
		STps->drv_file = -1;
	}
	if (STp->can_partitions) {
		STp->partition = find_partition(STp);
		if (STp->partition < 0)
			STp->partition = 0;
		STp->new_partition = STp->partition;
	}
}

/* Test if the drive is ready. Returns either one of the codes below or a negative system
   error code. */
#define CHKRES_READY       0
#define CHKRES_NEW_SESSION 1
#define CHKRES_NOT_READY   2
#define CHKRES_NO_TAPE     3

#define MAX_ATTENTIONS    10

static int test_ready(struct scsi_tape *STp, int do_wait)
{
	int attentions, waits, max_wait, scode;
	int retval = CHKRES_READY, new_session = 0;
	unsigned char cmd[MAX_COMMAND_SIZE];
	struct st_request *SRpnt = NULL;
	struct st_cmdstatus *cmdstatp = &STp->buffer->cmdstat;

	max_wait = do_wait ? ST_BLOCK_SECONDS : 0;

	for (attentions=waits=0; ; ) {
		memset((void *) &cmd[0], 0, MAX_COMMAND_SIZE);
		cmd[0] = TEST_UNIT_READY;
		SRpnt = st_do_scsi(SRpnt, STp, cmd, 0, DMA_NONE,
				   STp->long_timeout, MAX_READY_RETRIES, 1);

		if (!SRpnt) {
			retval = (STp->buffer)->syscall_result;
			break;
		}

		if (cmdstatp->have_sense) {

			scode = cmdstatp->sense_hdr.sense_key;

			if (scode == UNIT_ATTENTION) { /* New media? */
				new_session = 1;
				if (attentions < MAX_ATTENTIONS) {
					attentions++;
					continue;
				}
				else {
					retval = (-EIO);
					break;
				}
			}

			if (scode == NOT_READY) {
				if (waits < max_wait) {
					if (msleep_interruptible(1000)) {
						retval = (-EINTR);
						break;
					}
					waits++;
					continue;
				}
				else {
					if ((STp->device)->scsi_level >= SCSI_2 &&
					    cmdstatp->sense_hdr.asc == 0x3a)	/* Check ASC */
						retval = CHKRES_NO_TAPE;
					else
						retval = CHKRES_NOT_READY;
					break;
				}
			}
		}

		retval = (STp->buffer)->syscall_result;
		if (!retval)
			retval = new_session ? CHKRES_NEW_SESSION : CHKRES_READY;
		break;
	}

	if (SRpnt != NULL)
		st_release_request(SRpnt);
	return retval;
}


/* See if the drive is ready and gather information about the tape. Return values:
   < 0   negative error code from errno.h
   0     drive ready
   1     drive not ready (possibly no tape)
*/
static int check_tape(struct scsi_tape *STp, struct file *filp)
{
	int i, retval, new_session = 0, do_wait;
	unsigned char cmd[MAX_COMMAND_SIZE], saved_cleaning;
	unsigned short st_flags = filp->f_flags;
	struct st_request *SRpnt = NULL;
	struct st_modedef *STm;
	struct st_partstat *STps;
	struct inode *inode = file_inode(filp);
	int mode = TAPE_MODE(inode);

	STp->ready = ST_READY;

	if (mode != STp->current_mode) {
		DEBC_printk(STp, "Mode change from %d to %d.\n",
			    STp->current_mode, mode);
		new_session = 1;
		STp->current_mode = mode;
	}
	STm = &(STp->modes[STp->current_mode]);

	saved_cleaning = STp->cleaning_req;
	STp->cleaning_req = 0;

	do_wait = ((filp->f_flags & O_NONBLOCK) == 0);
	retval = test_ready(STp, do_wait);

	if (retval < 0)
	    goto err_out;

	if (retval == CHKRES_NEW_SESSION) {
		STp->pos_unknown = 0;
		STp->partition = STp->new_partition = 0;
		if (STp->can_partitions)
			STp->nbr_partitions = 1; /* This guess will be updated later
                                                    if necessary */
		for (i = 0; i < ST_NBR_PARTITIONS; i++) {
			STps = &(STp->ps[i]);
			STps->rw = ST_IDLE;
			STps->eof = ST_NOEOF;
			STps->at_sm = 0;
			STps->last_block_valid = 0;
			STps->drv_block = 0;
			STps->drv_file = 0;
		}
		new_session = 1;
	}
	else {
		STp->cleaning_req |= saved_cleaning;

		if (retval == CHKRES_NOT_READY || retval == CHKRES_NO_TAPE) {
			if (retval == CHKRES_NO_TAPE)
				STp->ready = ST_NO_TAPE;
			else
				STp->ready = ST_NOT_READY;

			STp->density = 0;	/* Clear the erroneous "residue" */
			STp->write_prot = 0;
			STp->block_size = 0;
			STp->ps[0].drv_file = STp->ps[0].drv_block = (-1);
			STp->partition = STp->new_partition = 0;
			STp->door_locked = ST_UNLOCKED;
			return CHKRES_NOT_READY;
		}
	}

	if (STp->omit_blklims)
		STp->min_block = STp->max_block = (-1);
	else {
		memset((void *) &cmd[0], 0, MAX_COMMAND_SIZE);
		cmd[0] = READ_BLOCK_LIMITS;

		SRpnt = st_do_scsi(SRpnt, STp, cmd, 6, DMA_FROM_DEVICE,
				   STp->device->request_queue->rq_timeout,
				   MAX_READY_RETRIES, 1);
		if (!SRpnt) {
			retval = (STp->buffer)->syscall_result;
			goto err_out;
		}

		if (!SRpnt->result && !STp->buffer->cmdstat.have_sense) {
			STp->max_block = ((STp->buffer)->b_data[1] << 16) |
			    ((STp->buffer)->b_data[2] << 8) | (STp->buffer)->b_data[3];
			STp->min_block = ((STp->buffer)->b_data[4] << 8) |
			    (STp->buffer)->b_data[5];
			if ( DEB( debugging || ) !STp->inited)
				st_printk(KERN_INFO, STp,
					  "Block limits %d - %d bytes.\n",
					  STp->min_block, STp->max_block);
		} else {
			STp->min_block = STp->max_block = (-1);
			DEBC_printk(STp, "Can't read block limits.\n");
		}
	}

	memset((void *) &cmd[0], 0, MAX_COMMAND_SIZE);
	cmd[0] = MODE_SENSE;
	cmd[4] = 12;

	SRpnt = st_do_scsi(SRpnt, STp, cmd, 12, DMA_FROM_DEVICE,
			   STp->device->request_queue->rq_timeout,
			   MAX_READY_RETRIES, 1);
	if (!SRpnt) {
		retval = (STp->buffer)->syscall_result;
		goto err_out;
	}

	if ((STp->buffer)->syscall_result != 0) {
		DEBC_printk(STp, "No Mode Sense.\n");
		STp->block_size = ST_DEFAULT_BLOCK;	/* Educated guess (?) */
		(STp->buffer)->syscall_result = 0;	/* Prevent error propagation */
		STp->drv_write_prot = 0;
	} else {
		DEBC_printk(STp,"Mode sense. Length %d, "
			    "medium %x, WBS %x, BLL %d\n",
			    (STp->buffer)->b_data[0],
			    (STp->buffer)->b_data[1],
			    (STp->buffer)->b_data[2],
			    (STp->buffer)->b_data[3]);

		if ((STp->buffer)->b_data[3] >= 8) {
			STp->drv_buffer = ((STp->buffer)->b_data[2] >> 4) & 7;
			STp->density = (STp->buffer)->b_data[4];
			STp->block_size = (STp->buffer)->b_data[9] * 65536 +
			    (STp->buffer)->b_data[10] * 256 + (STp->buffer)->b_data[11];
			DEBC_printk(STp, "Density %x, tape length: %x, "
				    "drv buffer: %d\n",
				    STp->density,
				    (STp->buffer)->b_data[5] * 65536 +
				    (STp->buffer)->b_data[6] * 256 +
				    (STp->buffer)->b_data[7],
				    STp->drv_buffer);
		}
		STp->drv_write_prot = ((STp->buffer)->b_data[2] & 0x80) != 0;
		if (!STp->drv_buffer && STp->immediate_filemark) {
			st_printk(KERN_WARNING, STp,
				  "non-buffered tape: disabling "
				  "writing immediate filemarks\n");
			STp->immediate_filemark = 0;
		}
	}
	st_release_request(SRpnt);
	SRpnt = NULL;
	STp->inited = 1;

	if (STp->block_size > 0)
		(STp->buffer)->buffer_blocks =
			(STp->buffer)->buffer_size / STp->block_size;
	else
		(STp->buffer)->buffer_blocks = 1;
	(STp->buffer)->buffer_bytes = (STp->buffer)->read_pointer = 0;

	DEBC_printk(STp, "Block size: %d, buffer size: %d (%d blocks).\n",
		    STp->block_size, (STp->buffer)->buffer_size,
		    (STp->buffer)->buffer_blocks);

	if (STp->drv_write_prot) {
		STp->write_prot = 1;

		DEBC_printk(STp, "Write protected\n");

		if (do_wait &&
		    ((st_flags & O_ACCMODE) == O_WRONLY ||
		     (st_flags & O_ACCMODE) == O_RDWR)) {
			retval = (-EROFS);
			goto err_out;
		}
	}

	if (STp->can_partitions && STp->nbr_partitions < 1) {
		/* This code is reached when the device is opened for the first time
		   after the driver has been initialized with tape in the drive and the
		   partition support has been enabled. */
		DEBC_printk(STp, "Updating partition number in status.\n");
		if ((STp->partition = find_partition(STp)) < 0) {
			retval = STp->partition;
			goto err_out;
		}
		STp->new_partition = STp->partition;
		STp->nbr_partitions = 1; /* This guess will be updated when necessary */
	}

	if (new_session) {	/* Change the drive parameters for the new mode */
		STp->density_changed = STp->blksize_changed = 0;
		STp->compression_changed = 0;
		if (!(STm->defaults_for_writes) &&
		    (retval = set_mode_densblk(STp, STm)) < 0)
		    goto err_out;

		if (STp->default_drvbuffer != 0xff) {
			if (st_int_ioctl(STp, MTSETDRVBUFFER, STp->default_drvbuffer))
				st_printk(KERN_WARNING, STp,
					  "Can't set default drive "
					  "buffering to %d.\n",
					  STp->default_drvbuffer);
		}
	}

	return CHKRES_READY;

 err_out:
	return retval;
}


/* Open the device. Needs to take the BKL only because of incrementing the SCSI host
   module count. */
static int st_open(struct inode *inode, struct file *filp)
{
	int i, retval = (-EIO);
	int resumed = 0;
	struct scsi_tape *STp;
	struct st_partstat *STps;
	int dev = TAPE_NR(inode);

	/*
	 * We really want to do nonseekable_open(inode, filp); here, but some
	 * versions of tar incorrectly call lseek on tapes and bail out if that
	 * fails.  So we disallow pread() and pwrite(), but permit lseeks.
	 */
	filp->f_mode &= ~(FMODE_PREAD | FMODE_PWRITE);

	if (!(STp = scsi_tape_get(dev))) {
		return -ENXIO;
	}

	filp->private_data = STp;

	spin_lock(&st_use_lock);
	if (STp->in_use) {
		spin_unlock(&st_use_lock);
		DEBC_printk(STp, "Device already in use.\n");
		scsi_tape_put(STp);
		return (-EBUSY);
	}

	STp->in_use = 1;
	spin_unlock(&st_use_lock);
	STp->rew_at_close = STp->autorew_dev = (iminor(inode) & 0x80) == 0;

	if (scsi_autopm_get_device(STp->device) < 0) {
		retval = -EIO;
		goto err_out;
	}
	resumed = 1;
	if (!scsi_block_when_processing_errors(STp->device)) {
		retval = (-ENXIO);
		goto err_out;
	}

	/* See that we have at least a one page buffer available */
	if (!enlarge_buffer(STp->buffer, PAGE_SIZE)) {
		st_printk(KERN_WARNING, STp,
			  "Can't allocate one page tape buffer.\n");
		retval = (-EOVERFLOW);
		goto err_out;
	}

	(STp->buffer)->cleared = 0;
	(STp->buffer)->writing = 0;
	(STp->buffer)->syscall_result = 0;

	STp->write_prot = ((filp->f_flags & O_ACCMODE) == O_RDONLY);

	STp->dirty = 0;
	for (i = 0; i < ST_NBR_PARTITIONS; i++) {
		STps = &(STp->ps[i]);
		STps->rw = ST_IDLE;
	}
	STp->try_dio_now = STp->try_dio;
	STp->recover_count = 0;
	DEB( STp->nbr_waits = STp->nbr_finished = 0;
	     STp->nbr_requests = STp->nbr_dio = STp->nbr_pages = 0; )

	retval = check_tape(STp, filp);
	if (retval < 0)
		goto err_out;
	if ((filp->f_flags & O_NONBLOCK) == 0 &&
	    retval != CHKRES_READY) {
		if (STp->ready == NO_TAPE)
			retval = (-ENOMEDIUM);
		else
			retval = (-EIO);
		goto err_out;
	}
	return 0;

 err_out:
	normalize_buffer(STp->buffer);
	spin_lock(&st_use_lock);
	STp->in_use = 0;
	spin_unlock(&st_use_lock);
	if (resumed)
		scsi_autopm_put_device(STp->device);
	scsi_tape_put(STp);
	return retval;

}


/* Flush the tape buffer before close */
static int st_flush(struct file *filp, fl_owner_t id)
{
	int result = 0, result2;
	unsigned char cmd[MAX_COMMAND_SIZE];
	struct st_request *SRpnt;
	struct scsi_tape *STp = filp->private_data;
	struct st_modedef *STm = &(STp->modes[STp->current_mode]);
	struct st_partstat *STps = &(STp->ps[STp->partition]);

	if (file_count(filp) > 1)
		return 0;

	if (STps->rw == ST_WRITING && !STp->pos_unknown) {
		result = st_flush_write_buffer(STp);
		if (result != 0 && result != (-ENOSPC))
			goto out;
	}

	if (STp->can_partitions &&
	    (result2 = switch_partition(STp)) < 0) {
		DEBC_printk(STp, "switch_partition at close failed.\n");
		if (result == 0)
			result = result2;
		goto out;
	}

	DEBC( if (STp->nbr_requests)
		st_printk(KERN_DEBUG, STp,
			  "Number of r/w requests %d, dio used in %d, "
			  "pages %d.\n", STp->nbr_requests, STp->nbr_dio,
			  STp->nbr_pages));

	if (STps->rw == ST_WRITING && !STp->pos_unknown) {
		struct st_cmdstatus *cmdstatp = &STp->buffer->cmdstat;

#if DEBUG
		DEBC_printk(STp, "Async write waits %d, finished %d.\n",
			    STp->nbr_waits, STp->nbr_finished);
#endif
		memset(cmd, 0, MAX_COMMAND_SIZE);
		cmd[0] = WRITE_FILEMARKS;
		if (STp->immediate_filemark)
			cmd[1] = 1;
		cmd[4] = 1 + STp->two_fm;

		SRpnt = st_do_scsi(NULL, STp, cmd, 0, DMA_NONE,
				   STp->device->request_queue->rq_timeout,
				   MAX_WRITE_RETRIES, 1);
		if (!SRpnt) {
			result = (STp->buffer)->syscall_result;
			goto out;
		}

		if (STp->buffer->syscall_result == 0 ||
		    (cmdstatp->have_sense && !cmdstatp->deferred &&
		     (cmdstatp->flags & SENSE_EOM) &&
		     (cmdstatp->sense_hdr.sense_key == NO_SENSE ||
		      cmdstatp->sense_hdr.sense_key == RECOVERED_ERROR) &&
		     (!cmdstatp->remainder_valid || cmdstatp->uremainder64 == 0))) {
			/* Write successful at EOM */
			st_release_request(SRpnt);
			SRpnt = NULL;
			if (STps->drv_file >= 0)
				STps->drv_file++;
			STps->drv_block = 0;
			if (STp->two_fm)
				cross_eof(STp, 0);
			STps->eof = ST_FM;
		}
		else { /* Write error */
			st_release_request(SRpnt);
			SRpnt = NULL;
			st_printk(KERN_ERR, STp,
				  "Error on write filemark.\n");
			if (result == 0)
				result = (-EIO);
		}

		DEBC_printk(STp, "Buffer flushed, %d EOF(s) written\n", cmd[4]);
	} else if (!STp->rew_at_close) {
		STps = &(STp->ps[STp->partition]);
		if (!STm->sysv || STps->rw != ST_READING) {
			if (STp->can_bsr)
				result = flush_buffer(STp, 0);
			else if (STps->eof == ST_FM_HIT) {
				result = cross_eof(STp, 0);
				if (result) {
					if (STps->drv_file >= 0)
						STps->drv_file++;
					STps->drv_block = 0;
					STps->eof = ST_FM;
				} else
					STps->eof = ST_NOEOF;
			}
		} else if ((STps->eof == ST_NOEOF &&
			    !(result = cross_eof(STp, 1))) ||
			   STps->eof == ST_FM_HIT) {
			if (STps->drv_file >= 0)
				STps->drv_file++;
			STps->drv_block = 0;
			STps->eof = ST_FM;
		}
	}

      out:
	if (STp->rew_at_close) {
		result2 = st_int_ioctl(STp, MTREW, 1);
		if (result == 0)
			result = result2;
	}
	return result;
}


/* Close the device and release it. BKL is not needed: this is the only thread
   accessing this tape. */
static int st_release(struct inode *inode, struct file *filp)
{
	struct scsi_tape *STp = filp->private_data;

	if (STp->door_locked == ST_LOCKED_AUTO)
		do_door_lock(STp, 0);

	normalize_buffer(STp->buffer);
	spin_lock(&st_use_lock);
	STp->in_use = 0;
	spin_unlock(&st_use_lock);
	scsi_autopm_put_device(STp->device);
	scsi_tape_put(STp);

	return 0;
}

/* The checks common to both reading and writing */
static ssize_t rw_checks(struct scsi_tape *STp, struct file *filp, size_t count)
{
	ssize_t retval = 0;

	/*
	 * If we are in the middle of error recovery, don't let anyone
	 * else try and use this device.  Also, if error recovery fails, it
	 * may try and take the device offline, in which case all further
	 * access to the device is prohibited.
	 */
	if (!scsi_block_when_processing_errors(STp->device)) {
		retval = (-ENXIO);
		goto out;
	}

	if (STp->ready != ST_READY) {
		if (STp->ready == ST_NO_TAPE)
			retval = (-ENOMEDIUM);
		else
			retval = (-EIO);
		goto out;
	}

	if (! STp->modes[STp->current_mode].defined) {
		retval = (-ENXIO);
		goto out;
	}


	/*
	 * If there was a bus reset, block further access
	 * to this device.
	 */
	if (STp->pos_unknown) {
		retval = (-EIO);
		goto out;
	}

	if (count == 0)
		goto out;

	DEB(
	if (!STp->in_use) {
		st_printk(ST_DEB_MSG, STp,
			  "Incorrect device.\n");
		retval = (-EIO);
		goto out;
	} ) /* end DEB */

	if (STp->can_partitions &&
	    (retval = switch_partition(STp)) < 0)
		goto out;

	if (STp->block_size == 0 && STp->max_block > 0 &&
	    (count < STp->min_block || count > STp->max_block)) {
		retval = (-EINVAL);
		goto out;
	}

	if (STp->do_auto_lock && STp->door_locked == ST_UNLOCKED &&
	    !do_door_lock(STp, 1))
		STp->door_locked = ST_LOCKED_AUTO;

 out:
	return retval;
}


static int setup_buffering(struct scsi_tape *STp, const char __user *buf,
			   size_t count, int is_read)
{
	int i, bufsize, retval = 0;
	struct st_buffer *STbp = STp->buffer;

	if (is_read)
		i = STp->try_dio_now && try_rdio;
	else
		i = STp->try_dio_now && try_wdio;

	if (i && ((unsigned long)buf & queue_dma_alignment(
					STp->device->request_queue)) == 0) {
		i = sgl_map_user_pages(STbp, STbp->use_sg, (unsigned long)buf,
				       count, (is_read ? READ : WRITE));
		if (i > 0) {
			STbp->do_dio = i;
			STbp->buffer_bytes = 0;   /* can be used as transfer counter */
		}
		else
			STbp->do_dio = 0;  /* fall back to buffering with any error */
		STbp->sg_segs = STbp->do_dio;
		DEB(
		     if (STbp->do_dio) {
			STp->nbr_dio++;
			STp->nbr_pages += STbp->do_dio;
		     }
		)
	} else
		STbp->do_dio = 0;
	DEB( STp->nbr_requests++; )

	if (!STbp->do_dio) {
		if (STp->block_size)
			bufsize = STp->block_size > st_fixed_buffer_size ?
				STp->block_size : st_fixed_buffer_size;
		else {
			bufsize = count;
			/* Make sure that data from previous user is not leaked even if
			   HBA does not return correct residual */
			if (is_read && STp->sili && !STbp->cleared)
				clear_buffer(STbp);
		}

		if (bufsize > STbp->buffer_size &&
		    !enlarge_buffer(STbp, bufsize)) {
			st_printk(KERN_WARNING, STp,
				  "Can't allocate %d byte tape buffer.\n",
				  bufsize);
			retval = (-EOVERFLOW);
			goto out;
		}
		if (STp->block_size)
			STbp->buffer_blocks = bufsize / STp->block_size;
	}

 out:
	return retval;
}


/* Can be called more than once after each setup_buffer() */
static void release_buffering(struct scsi_tape *STp, int is_read)
{
	struct st_buffer *STbp;

	STbp = STp->buffer;
	if (STbp->do_dio) {
		sgl_unmap_user_pages(STbp, STbp->do_dio, is_read);
		STbp->do_dio = 0;
		STbp->sg_segs = 0;
	}
}


/* Write command */
static ssize_t
st_write(struct file *filp, const char __user *buf, size_t count, loff_t * ppos)
{
	ssize_t total;
	ssize_t i, do_count, blks, transfer;
	ssize_t retval;
	int undone, retry_eot = 0, scode;
	int async_write;
	unsigned char cmd[MAX_COMMAND_SIZE];
	const char __user *b_point;
	struct st_request *SRpnt = NULL;
	struct scsi_tape *STp = filp->private_data;
	struct st_modedef *STm;
	struct st_partstat *STps;
	struct st_buffer *STbp;

	if (mutex_lock_interruptible(&STp->lock))
		return -ERESTARTSYS;

	retval = rw_checks(STp, filp, count);
	if (retval || count == 0)
		goto out;

	/* Write must be integral number of blocks */
	if (STp->block_size != 0 && (count % STp->block_size) != 0) {
		st_printk(KERN_WARNING, STp,
			  "Write not multiple of tape block size.\n");
		retval = (-EINVAL);
		goto out;
	}

	STm = &(STp->modes[STp->current_mode]);
	STps = &(STp->ps[STp->partition]);

	if (STp->write_prot) {
		retval = (-EACCES);
		goto out;
	}


	if (STps->rw == ST_READING) {
		retval = flush_buffer(STp, 0);
		if (retval)
			goto out;
		STps->rw = ST_WRITING;
	} else if (STps->rw != ST_WRITING &&
		   STps->drv_file == 0 && STps->drv_block == 0) {
		if ((retval = set_mode_densblk(STp, STm)) < 0)
			goto out;
		if (STm->default_compression != ST_DONT_TOUCH &&
		    !(STp->compression_changed)) {
			if (st_compression(STp, (STm->default_compression == ST_YES))) {
				st_printk(KERN_WARNING, STp,
					  "Can't set default compression.\n");
				if (modes_defined) {
					retval = (-EINVAL);
					goto out;
				}
			}
		}
	}

	STbp = STp->buffer;
	i = write_behind_check(STp);
	if (i) {
		if (i == -ENOSPC)
			STps->eof = ST_EOM_OK;
		else
			STps->eof = ST_EOM_ERROR;
	}

	if (STps->eof == ST_EOM_OK) {
		STps->eof = ST_EOD_1;  /* allow next write */
		retval = (-ENOSPC);
		goto out;
	}
	else if (STps->eof == ST_EOM_ERROR) {
		retval = (-EIO);
		goto out;
	}

	/* Check the buffer readability in cases where copy_user might catch
	   the problems after some tape movement. */
	if (STp->block_size != 0 &&
	    !STbp->do_dio &&
	    (copy_from_user(&i, buf, 1) != 0 ||
	     copy_from_user(&i, buf + count - 1, 1) != 0)) {
		retval = (-EFAULT);
		goto out;
	}

	retval = setup_buffering(STp, buf, count, 0);
	if (retval)
		goto out;

	total = count;

	memset(cmd, 0, MAX_COMMAND_SIZE);
	cmd[0] = WRITE_6;
	cmd[1] = (STp->block_size != 0);

	STps->rw = ST_WRITING;

	b_point = buf;
	while (count > 0 && !retry_eot) {

		if (STbp->do_dio) {
			do_count = count;
		}
		else {
			if (STp->block_size == 0)
				do_count = count;
			else {
				do_count = STbp->buffer_blocks * STp->block_size -
					STbp->buffer_bytes;
				if (do_count > count)
					do_count = count;
			}

			i = append_to_buffer(b_point, STbp, do_count);
			if (i) {
				retval = i;
				goto out;
			}
		}
		count -= do_count;
		b_point += do_count;

		async_write = STp->block_size == 0 && !STbp->do_dio &&
			STm->do_async_writes && STps->eof < ST_EOM_OK;

		if (STp->block_size != 0 && STm->do_buffer_writes &&
		    !(STp->try_dio_now && try_wdio) && STps->eof < ST_EOM_OK &&
		    STbp->buffer_bytes < STbp->buffer_size) {
			STp->dirty = 1;
			/* Don't write a buffer that is not full enough. */
			if (!async_write && count == 0)
				break;
		}

	retry_write:
		if (STp->block_size == 0)
			blks = transfer = do_count;
		else {
			if (!STbp->do_dio)
				blks = STbp->buffer_bytes;
			else
				blks = do_count;
			blks /= STp->block_size;
			transfer = blks * STp->block_size;
		}
		cmd[2] = blks >> 16;
		cmd[3] = blks >> 8;
		cmd[4] = blks;

		SRpnt = st_do_scsi(SRpnt, STp, cmd, transfer, DMA_TO_DEVICE,
				   STp->device->request_queue->rq_timeout,
				   MAX_WRITE_RETRIES, !async_write);
		if (!SRpnt) {
			retval = STbp->syscall_result;
			goto out;
		}
		if (async_write && !STbp->syscall_result) {
			STbp->writing = transfer;
			STp->dirty = !(STbp->writing ==
				       STbp->buffer_bytes);
			SRpnt = NULL;  /* Prevent releasing this request! */
			DEB( STp->write_pending = 1; )
			break;
		}

		if (STbp->syscall_result != 0) {
			struct st_cmdstatus *cmdstatp = &STp->buffer->cmdstat;

			DEBC_printk(STp, "Error on write:\n");
			if (cmdstatp->have_sense && (cmdstatp->flags & SENSE_EOM)) {
				scode = cmdstatp->sense_hdr.sense_key;
				if (cmdstatp->remainder_valid)
					undone = (int)cmdstatp->uremainder64;
				else if (STp->block_size == 0 &&
					 scode == VOLUME_OVERFLOW)
					undone = transfer;
				else
					undone = 0;
				if (STp->block_size != 0)
					undone *= STp->block_size;
				if (undone <= do_count) {
					/* Only data from this write is not written */
					count += undone;
					b_point -= undone;
					do_count -= undone;
					if (STp->block_size)
						blks = (transfer - undone) / STp->block_size;
					STps->eof = ST_EOM_OK;
					/* Continue in fixed block mode if all written
					   in this request but still something left to write
					   (retval left to zero)
					*/
					if (STp->block_size == 0 ||
					    undone > 0 || count == 0)
						retval = (-ENOSPC); /* EOM within current request */
					DEBC_printk(STp, "EOM with %d "
						    "bytes unwritten.\n",
						    (int)count);
				} else {
					/* EOT within data buffered earlier (possible only
					   in fixed block mode without direct i/o) */
					if (!retry_eot && !cmdstatp->deferred &&
					    (scode == NO_SENSE || scode == RECOVERED_ERROR)) {
						move_buffer_data(STp->buffer, transfer - undone);
						retry_eot = 1;
						if (STps->drv_block >= 0) {
							STps->drv_block += (transfer - undone) /
								STp->block_size;
						}
						STps->eof = ST_EOM_OK;
						DEBC_printk(STp, "Retry "
							    "write of %d "
							    "bytes at EOM.\n",
							    STp->buffer->buffer_bytes);
						goto retry_write;
					}
					else {
						/* Either error within data buffered by driver or
						   failed retry */
						count -= do_count;
						blks = do_count = 0;
						STps->eof = ST_EOM_ERROR;
						STps->drv_block = (-1); /* Too cautious? */
						retval = (-EIO);	/* EOM for old data */
						DEBC_printk(STp, "EOM with "
							    "lost data.\n");
					}
				}
			} else {
				count += do_count;
				STps->drv_block = (-1);		/* Too cautious? */
				retval = STbp->syscall_result;
			}

		}

		if (STps->drv_block >= 0) {
			if (STp->block_size == 0)
				STps->drv_block += (do_count > 0);
			else
				STps->drv_block += blks;
		}

		STbp->buffer_bytes = 0;
		STp->dirty = 0;

		if (retval || retry_eot) {
			if (count < total)
				retval = total - count;
			goto out;
		}
	}

	if (STps->eof == ST_EOD_1)
		STps->eof = ST_EOM_OK;
	else if (STps->eof != ST_EOM_OK)
		STps->eof = ST_NOEOF;
	retval = total - count;

 out:
	if (SRpnt != NULL)
		st_release_request(SRpnt);
	release_buffering(STp, 0);
	mutex_unlock(&STp->lock);

	return retval;
}

/* Read data from the tape. Returns zero in the normal case, one if the
   eof status has changed, and the negative error code in case of a
   fatal error. Otherwise updates the buffer and the eof state.

   Does release user buffer mapping if it is set.
*/
static long read_tape(struct scsi_tape *STp, long count,
		      struct st_request ** aSRpnt)
{
	int transfer, blks, bytes;
	unsigned char cmd[MAX_COMMAND_SIZE];
	struct st_request *SRpnt;
	struct st_modedef *STm;
	struct st_partstat *STps;
	struct st_buffer *STbp;
	int retval = 0;

	if (count == 0)
		return 0;

	STm = &(STp->modes[STp->current_mode]);
	STps = &(STp->ps[STp->partition]);
	if (STps->eof == ST_FM_HIT)
		return 1;
	STbp = STp->buffer;

	if (STp->block_size == 0)
		blks = bytes = count;
	else {
		if (!(STp->try_dio_now && try_rdio) && STm->do_read_ahead) {
			blks = (STp->buffer)->buffer_blocks;
			bytes = blks * STp->block_size;
		} else {
			bytes = count;
			if (!STbp->do_dio && bytes > (STp->buffer)->buffer_size)
				bytes = (STp->buffer)->buffer_size;
			blks = bytes / STp->block_size;
			bytes = blks * STp->block_size;
		}
	}

	memset(cmd, 0, MAX_COMMAND_SIZE);
	cmd[0] = READ_6;
	cmd[1] = (STp->block_size != 0);
	if (!cmd[1] && STp->sili)
		cmd[1] |= 2;
	cmd[2] = blks >> 16;
	cmd[3] = blks >> 8;
	cmd[4] = blks;

	SRpnt = *aSRpnt;
	SRpnt = st_do_scsi(SRpnt, STp, cmd, bytes, DMA_FROM_DEVICE,
			   STp->device->request_queue->rq_timeout,
			   MAX_RETRIES, 1);
	release_buffering(STp, 1);
	*aSRpnt = SRpnt;
	if (!SRpnt)
		return STbp->syscall_result;

	STbp->read_pointer = 0;
	STps->at_sm = 0;

	/* Something to check */
	if (STbp->syscall_result) {
		struct st_cmdstatus *cmdstatp = &STp->buffer->cmdstat;

		retval = 1;
		DEBC_printk(STp,
			    "Sense: %2x %2x %2x %2x %2x %2x %2x %2x\n",
			    SRpnt->sense[0], SRpnt->sense[1],
			    SRpnt->sense[2], SRpnt->sense[3],
			    SRpnt->sense[4], SRpnt->sense[5],
			    SRpnt->sense[6], SRpnt->sense[7]);
		if (cmdstatp->have_sense) {

			if (cmdstatp->sense_hdr.sense_key == BLANK_CHECK)
				cmdstatp->flags &= 0xcf;	/* No need for EOM in this case */

			if (cmdstatp->flags != 0) { /* EOF, EOM, or ILI */
				/* Compute the residual count */
				if (cmdstatp->remainder_valid)
					transfer = (int)cmdstatp->uremainder64;
				else
					transfer = 0;
				if (cmdstatp->sense_hdr.sense_key == MEDIUM_ERROR) {
					if (STp->block_size == 0)
						transfer = bytes;
					/* Some drives set ILI with MEDIUM ERROR */
					cmdstatp->flags &= ~SENSE_ILI;
				}

				if (cmdstatp->flags & SENSE_ILI) {	/* ILI */
					if (STp->block_size == 0 &&
					    transfer < 0) {
						st_printk(KERN_NOTICE, STp,
							  "Failed to read %d "
							  "byte block with %d "
							  "byte transfer.\n",
							  bytes - transfer,
							  bytes);
						if (STps->drv_block >= 0)
							STps->drv_block += 1;
						STbp->buffer_bytes = 0;
						return (-ENOMEM);
					} else if (STp->block_size == 0) {
						STbp->buffer_bytes = bytes - transfer;
					} else {
						st_release_request(SRpnt);
						SRpnt = *aSRpnt = NULL;
						if (transfer == blks) {	/* We did not get anything, error */
							st_printk(KERN_NOTICE, STp,
								  "Incorrect "
								  "block size.\n");
							if (STps->drv_block >= 0)
								STps->drv_block += blks - transfer + 1;
							st_int_ioctl(STp, MTBSR, 1);
							return (-EIO);
						}
						/* We have some data, deliver it */
						STbp->buffer_bytes = (blks - transfer) *
						    STp->block_size;
						DEBC_printk(STp, "ILI but "
							    "enough data "
							    "received %ld "
							    "%d.\n", count,
							    STbp->buffer_bytes);
						if (STps->drv_block >= 0)
							STps->drv_block += 1;
						if (st_int_ioctl(STp, MTBSR, 1))
							return (-EIO);
					}
				} else if (cmdstatp->flags & SENSE_FMK) {	/* FM overrides EOM */
					if (STps->eof != ST_FM_HIT)
						STps->eof = ST_FM_HIT;
					else
						STps->eof = ST_EOD_2;
					if (STp->block_size == 0)
						STbp->buffer_bytes = 0;
					else
						STbp->buffer_bytes =
						    bytes - transfer * STp->block_size;
					DEBC_printk(STp, "EOF detected (%d "
						    "bytes read).\n",
						    STbp->buffer_bytes);
				} else if (cmdstatp->flags & SENSE_EOM) {
					if (STps->eof == ST_FM)
						STps->eof = ST_EOD_1;
					else
						STps->eof = ST_EOM_OK;
					if (STp->block_size == 0)
						STbp->buffer_bytes = bytes - transfer;
					else
						STbp->buffer_bytes =
						    bytes - transfer * STp->block_size;

					DEBC_printk(STp, "EOM detected (%d "
						    "bytes read).\n",
						    STbp->buffer_bytes);
				}
			}
			/* end of EOF, EOM, ILI test */
			else {	/* nonzero sense key */
				DEBC_printk(STp, "Tape error while reading.\n");
				STps->drv_block = (-1);
				if (STps->eof == ST_FM &&
				    cmdstatp->sense_hdr.sense_key == BLANK_CHECK) {
					DEBC_printk(STp, "Zero returned for "
						    "first BLANK CHECK "
						    "after EOF.\n");
					STps->eof = ST_EOD_2;	/* First BLANK_CHECK after FM */
				} else	/* Some other extended sense code */
					retval = (-EIO);
			}

			if (STbp->buffer_bytes < 0)  /* Caused by bogus sense data */
				STbp->buffer_bytes = 0;
		}
		/* End of extended sense test */
		else {		/* Non-extended sense */
			retval = STbp->syscall_result;
		}

	}
	/* End of error handling */
	else {			/* Read successful */
		STbp->buffer_bytes = bytes;
		if (STp->sili) /* In fixed block mode residual is always zero here */
			STbp->buffer_bytes -= STp->buffer->cmdstat.residual;
	}

	if (STps->drv_block >= 0) {
		if (STp->block_size == 0)
			STps->drv_block++;
		else
			STps->drv_block += STbp->buffer_bytes / STp->block_size;
	}
	return retval;
}


/* Read command */
static ssize_t
st_read(struct file *filp, char __user *buf, size_t count, loff_t * ppos)
{
	ssize_t total;
	ssize_t retval = 0;
	ssize_t i, transfer;
	int special, do_dio = 0;
	struct st_request *SRpnt = NULL;
	struct scsi_tape *STp = filp->private_data;
	struct st_modedef *STm;
	struct st_partstat *STps;
	struct st_buffer *STbp = STp->buffer;

	if (mutex_lock_interruptible(&STp->lock))
		return -ERESTARTSYS;

	retval = rw_checks(STp, filp, count);
	if (retval || count == 0)
		goto out;

	STm = &(STp->modes[STp->current_mode]);
	if (STp->block_size != 0 && (count % STp->block_size) != 0) {
		if (!STm->do_read_ahead) {
			retval = (-EINVAL);	/* Read must be integral number of blocks */
			goto out;
		}
		STp->try_dio_now = 0;  /* Direct i/o can't handle split blocks */
	}

	STps = &(STp->ps[STp->partition]);
	if (STps->rw == ST_WRITING) {
		retval = flush_buffer(STp, 0);
		if (retval)
			goto out;
		STps->rw = ST_READING;
	}
	DEB(
	if (debugging && STps->eof != ST_NOEOF)
		st_printk(ST_DEB_MSG, STp,
			  "EOF/EOM flag up (%d). Bytes %d\n",
			  STps->eof, STbp->buffer_bytes);
	) /* end DEB */

	retval = setup_buffering(STp, buf, count, 1);
	if (retval)
		goto out;
	do_dio = STbp->do_dio;

	if (STbp->buffer_bytes == 0 &&
	    STps->eof >= ST_EOD_1) {
		if (STps->eof < ST_EOD) {
			STps->eof += 1;
			retval = 0;
			goto out;
		}
		retval = (-EIO);	/* EOM or Blank Check */
		goto out;
	}

	if (do_dio) {
		/* Check the buffer writability before any tape movement. Don't alter
		   buffer data. */
		if (copy_from_user(&i, buf, 1) != 0 ||
		    copy_to_user(buf, &i, 1) != 0 ||
		    copy_from_user(&i, buf + count - 1, 1) != 0 ||
		    copy_to_user(buf + count - 1, &i, 1) != 0) {
			retval = (-EFAULT);
			goto out;
		}
	}

	STps->rw = ST_READING;


	/* Loop until enough data in buffer or a special condition found */
	for (total = 0, special = 0; total < count && !special;) {

		/* Get new data if the buffer is empty */
		if (STbp->buffer_bytes == 0) {
			special = read_tape(STp, count - total, &SRpnt);
			if (special < 0) {	/* No need to continue read */
				retval = special;
				goto out;
			}
		}

		/* Move the data from driver buffer to user buffer */
		if (STbp->buffer_bytes > 0) {
			DEB(
			if (debugging && STps->eof != ST_NOEOF)
				st_printk(ST_DEB_MSG, STp,
					  "EOF up (%d). Left %d, needed %d.\n",
					  STps->eof, STbp->buffer_bytes,
					  (int)(count - total));
			) /* end DEB */
			transfer = STbp->buffer_bytes < count - total ?
			    STbp->buffer_bytes : count - total;
			if (!do_dio) {
				i = from_buffer(STbp, buf, transfer);
				if (i) {
					retval = i;
					goto out;
				}
			}
			buf += transfer;
			total += transfer;
		}

		if (STp->block_size == 0)
			break;	/* Read only one variable length block */

	}			/* for (total = 0, special = 0;
                                   total < count && !special; ) */

	/* Change the eof state if no data from tape or buffer */
	if (total == 0) {
		if (STps->eof == ST_FM_HIT) {
			STps->eof = ST_FM;
			STps->drv_block = 0;
			if (STps->drv_file >= 0)
				STps->drv_file++;
		} else if (STps->eof == ST_EOD_1) {
			STps->eof = ST_EOD_2;
			STps->drv_block = 0;
			if (STps->drv_file >= 0)
				STps->drv_file++;
		} else if (STps->eof == ST_EOD_2)
			STps->eof = ST_EOD;
	} else if (STps->eof == ST_FM)
		STps->eof = ST_NOEOF;
	retval = total;

 out:
	if (SRpnt != NULL) {
		st_release_request(SRpnt);
		SRpnt = NULL;
	}
	if (do_dio) {
		release_buffering(STp, 1);
		STbp->buffer_bytes = 0;
	}
	mutex_unlock(&STp->lock);

	return retval;
}



DEB(
/* Set the driver options */
static void st_log_options(struct scsi_tape * STp, struct st_modedef * STm)
{
	if (debugging) {
		st_printk(KERN_INFO, STp,
			  "Mode %d options: buffer writes: %d, "
			  "async writes: %d, read ahead: %d\n",
			  STp->current_mode, STm->do_buffer_writes,
			  STm->do_async_writes, STm->do_read_ahead);
		st_printk(KERN_INFO, STp,
			  "    can bsr: %d, two FMs: %d, "
			  "fast mteom: %d, auto lock: %d,\n",
			  STp->can_bsr, STp->two_fm, STp->fast_mteom,
			  STp->do_auto_lock);
		st_printk(KERN_INFO, STp,
			  "    defs for wr: %d, no block limits: %d, "
			  "partitions: %d, s2 log: %d\n",
			  STm->defaults_for_writes, STp->omit_blklims,
			  STp->can_partitions, STp->scsi2_logical);
		st_printk(KERN_INFO, STp,
			  "    sysv: %d nowait: %d sili: %d "
			  "nowait_filemark: %d\n",
			  STm->sysv, STp->immediate, STp->sili,
			  STp->immediate_filemark);
		st_printk(KERN_INFO, STp, "    debugging: %d\n", debugging);
	}
}
	)


static int st_set_options(struct scsi_tape *STp, long options)
{
	int value;
	long code;
	struct st_modedef *STm;
	struct cdev *cd0, *cd1;
	struct device *d0, *d1;

	STm = &(STp->modes[STp->current_mode]);
	if (!STm->defined) {
		cd0 = STm->cdevs[0];
		cd1 = STm->cdevs[1];
		d0  = STm->devs[0];
		d1  = STm->devs[1];
		memcpy(STm, &(STp->modes[0]), sizeof(struct st_modedef));
		STm->cdevs[0] = cd0;
		STm->cdevs[1] = cd1;
		STm->devs[0]  = d0;
		STm->devs[1]  = d1;
		modes_defined = 1;
		DEBC_printk(STp, "Initialized mode %d definition from mode 0\n",
			    STp->current_mode);
	}

	code = options & MT_ST_OPTIONS;
	if (code == MT_ST_BOOLEANS) {
		STm->do_buffer_writes = (options & MT_ST_BUFFER_WRITES) != 0;
		STm->do_async_writes = (options & MT_ST_ASYNC_WRITES) != 0;
		STm->defaults_for_writes = (options & MT_ST_DEF_WRITES) != 0;
		STm->do_read_ahead = (options & MT_ST_READ_AHEAD) != 0;
		STp->two_fm = (options & MT_ST_TWO_FM) != 0;
		STp->fast_mteom = (options & MT_ST_FAST_MTEOM) != 0;
		STp->do_auto_lock = (options & MT_ST_AUTO_LOCK) != 0;
		STp->can_bsr = (options & MT_ST_CAN_BSR) != 0;
		STp->omit_blklims = (options & MT_ST_NO_BLKLIMS) != 0;
		if ((STp->device)->scsi_level >= SCSI_2)
			STp->can_partitions = (options & MT_ST_CAN_PARTITIONS) != 0;
		STp->scsi2_logical = (options & MT_ST_SCSI2LOGICAL) != 0;
		STp->immediate = (options & MT_ST_NOWAIT) != 0;
		STp->immediate_filemark = (options & MT_ST_NOWAIT_EOF) != 0;
		STm->sysv = (options & MT_ST_SYSV) != 0;
		STp->sili = (options & MT_ST_SILI) != 0;
		DEB( debugging = (options & MT_ST_DEBUGGING) != 0;
		     st_log_options(STp, STm); )
	} else if (code == MT_ST_SETBOOLEANS || code == MT_ST_CLEARBOOLEANS) {
		value = (code == MT_ST_SETBOOLEANS);
		if ((options & MT_ST_BUFFER_WRITES) != 0)
			STm->do_buffer_writes = value;
		if ((options & MT_ST_ASYNC_WRITES) != 0)
			STm->do_async_writes = value;
		if ((options & MT_ST_DEF_WRITES) != 0)
			STm->defaults_for_writes = value;
		if ((options & MT_ST_READ_AHEAD) != 0)
			STm->do_read_ahead = value;
		if ((options & MT_ST_TWO_FM) != 0)
			STp->two_fm = value;
		if ((options & MT_ST_FAST_MTEOM) != 0)
			STp->fast_mteom = value;
		if ((options & MT_ST_AUTO_LOCK) != 0)
			STp->do_auto_lock = value;
		if ((options & MT_ST_CAN_BSR) != 0)
			STp->can_bsr = value;
		if ((options & MT_ST_NO_BLKLIMS) != 0)
			STp->omit_blklims = value;
		if ((STp->device)->scsi_level >= SCSI_2 &&
		    (options & MT_ST_CAN_PARTITIONS) != 0)
			STp->can_partitions = value;
		if ((options & MT_ST_SCSI2LOGICAL) != 0)
			STp->scsi2_logical = value;
		if ((options & MT_ST_NOWAIT) != 0)
			STp->immediate = value;
		if ((options & MT_ST_NOWAIT_EOF) != 0)
			STp->immediate_filemark = value;
		if ((options & MT_ST_SYSV) != 0)
			STm->sysv = value;
		if ((options & MT_ST_SILI) != 0)
			STp->sili = value;
		DEB(
		if ((options & MT_ST_DEBUGGING) != 0)
			debugging = value;
			st_log_options(STp, STm); )
	} else if (code == MT_ST_WRITE_THRESHOLD) {
		/* Retained for compatibility */
	} else if (code == MT_ST_DEF_BLKSIZE) {
		value = (options & ~MT_ST_OPTIONS);
		if (value == ~MT_ST_OPTIONS) {
			STm->default_blksize = (-1);
			DEBC_printk(STp, "Default block size disabled.\n");
		} else {
			STm->default_blksize = value;
			DEBC_printk(STp,"Default block size set to "
				    "%d bytes.\n", STm->default_blksize);
			if (STp->ready == ST_READY) {
				STp->blksize_changed = 0;
				set_mode_densblk(STp, STm);
			}
		}
	} else if (code == MT_ST_TIMEOUTS) {
		value = (options & ~MT_ST_OPTIONS);
		if ((value & MT_ST_SET_LONG_TIMEOUT) != 0) {
			STp->long_timeout = (value & ~MT_ST_SET_LONG_TIMEOUT) * HZ;
			DEBC_printk(STp, "Long timeout set to %d seconds.\n",
				    (value & ~MT_ST_SET_LONG_TIMEOUT));
		} else {
			blk_queue_rq_timeout(STp->device->request_queue,
					     value * HZ);
			DEBC_printk(STp, "Normal timeout set to %d seconds.\n",
				    value);
		}
	} else if (code == MT_ST_SET_CLN) {
		value = (options & ~MT_ST_OPTIONS) & 0xff;
		if (value != 0 &&
			(value < EXTENDED_SENSE_START ||
				value >= SCSI_SENSE_BUFFERSIZE))
			return (-EINVAL);
		STp->cln_mode = value;
		STp->cln_sense_mask = (options >> 8) & 0xff;
		STp->cln_sense_value = (options >> 16) & 0xff;
		st_printk(KERN_INFO, STp,
			  "Cleaning request mode %d, mask %02x, value %02x\n",
			  value, STp->cln_sense_mask, STp->cln_sense_value);
	} else if (code == MT_ST_DEF_OPTIONS) {
		code = (options & ~MT_ST_CLEAR_DEFAULT);
		value = (options & MT_ST_CLEAR_DEFAULT);
		if (code == MT_ST_DEF_DENSITY) {
			if (value == MT_ST_CLEAR_DEFAULT) {
				STm->default_density = (-1);
				DEBC_printk(STp,
					    "Density default disabled.\n");
			} else {
				STm->default_density = value & 0xff;
				DEBC_printk(STp, "Density default set to %x\n",
					    STm->default_density);
				if (STp->ready == ST_READY) {
					STp->density_changed = 0;
					set_mode_densblk(STp, STm);
				}
			}
		} else if (code == MT_ST_DEF_DRVBUFFER) {
			if (value == MT_ST_CLEAR_DEFAULT) {
				STp->default_drvbuffer = 0xff;
				DEBC_printk(STp,
					    "Drive buffer default disabled.\n");
			} else {
				STp->default_drvbuffer = value & 7;
				DEBC_printk(STp,
					    "Drive buffer default set to %x\n",
					    STp->default_drvbuffer);
				if (STp->ready == ST_READY)
					st_int_ioctl(STp, MTSETDRVBUFFER, STp->default_drvbuffer);
			}
		} else if (code == MT_ST_DEF_COMPRESSION) {
			if (value == MT_ST_CLEAR_DEFAULT) {
				STm->default_compression = ST_DONT_TOUCH;
				DEBC_printk(STp,
					    "Compression default disabled.\n");
			} else {
				if ((value & 0xff00) != 0) {
					STp->c_algo = (value & 0xff00) >> 8;
					DEBC_printk(STp, "Compression "
						    "algorithm set to 0x%x.\n",
						    STp->c_algo);
				}
				if ((value & 0xff) != 0xff) {
					STm->default_compression = (value & 1 ? ST_YES : ST_NO);
					DEBC_printk(STp, "Compression default "
						    "set to %x\n",
						    (value & 1));
					if (STp->ready == ST_READY) {
						STp->compression_changed = 0;
						st_compression(STp, (STm->default_compression == ST_YES));
					}
				}
			}
		}
	} else
		return (-EIO);

	return 0;
}

#define MODE_HEADER_LENGTH  4

/* Mode header and page byte offsets */
#define MH_OFF_DATA_LENGTH     0
#define MH_OFF_MEDIUM_TYPE     1
#define MH_OFF_DEV_SPECIFIC    2
#define MH_OFF_BDESCS_LENGTH   3
#define MP_OFF_PAGE_NBR        0
#define MP_OFF_PAGE_LENGTH     1

/* Mode header and page bit masks */
#define MH_BIT_WP              0x80
#define MP_MSK_PAGE_NBR        0x3f

/* Don't return block descriptors */
#define MODE_SENSE_OMIT_BDESCS 0x08

#define MODE_SELECT_PAGE_FORMAT 0x10

/* Read a mode page into the tape buffer. The block descriptors are included
   if incl_block_descs is true. The page control is ored to the page number
   parameter, if necessary. */
static int read_mode_page(struct scsi_tape *STp, int page, int omit_block_descs)
{
	unsigned char cmd[MAX_COMMAND_SIZE];
	struct st_request *SRpnt;

	memset(cmd, 0, MAX_COMMAND_SIZE);
	cmd[0] = MODE_SENSE;
	if (omit_block_descs)
		cmd[1] = MODE_SENSE_OMIT_BDESCS;
	cmd[2] = page;
	cmd[4] = 255;

	SRpnt = st_do_scsi(NULL, STp, cmd, cmd[4], DMA_FROM_DEVICE,
			   STp->device->request_queue->rq_timeout, 0, 1);
	if (SRpnt == NULL)
		return (STp->buffer)->syscall_result;

	st_release_request(SRpnt);

	return STp->buffer->syscall_result;
}


/* Send the mode page in the tape buffer to the drive. Assumes that the mode data
   in the buffer is correctly formatted. The long timeout is used if slow is non-zero. */
static int write_mode_page(struct scsi_tape *STp, int page, int slow)
{
	int pgo;
	unsigned char cmd[MAX_COMMAND_SIZE];
	struct st_request *SRpnt;
	int timeout;

	memset(cmd, 0, MAX_COMMAND_SIZE);
	cmd[0] = MODE_SELECT;
	cmd[1] = MODE_SELECT_PAGE_FORMAT;
	pgo = MODE_HEADER_LENGTH + (STp->buffer)->b_data[MH_OFF_BDESCS_LENGTH];
	cmd[4] = pgo + (STp->buffer)->b_data[pgo + MP_OFF_PAGE_LENGTH] + 2;

	/* Clear reserved fields */
	(STp->buffer)->b_data[MH_OFF_DATA_LENGTH] = 0;
	(STp->buffer)->b_data[MH_OFF_MEDIUM_TYPE] = 0;
	(STp->buffer)->b_data[MH_OFF_DEV_SPECIFIC] &= ~MH_BIT_WP;
	(STp->buffer)->b_data[pgo + MP_OFF_PAGE_NBR] &= MP_MSK_PAGE_NBR;

	timeout = slow ?
		STp->long_timeout : STp->device->request_queue->rq_timeout;
	SRpnt = st_do_scsi(NULL, STp, cmd, cmd[4], DMA_TO_DEVICE,
			   timeout, 0, 1);
	if (SRpnt == NULL)
		return (STp->buffer)->syscall_result;

	st_release_request(SRpnt);

	return STp->buffer->syscall_result;
}


#define COMPRESSION_PAGE        0x0f
#define COMPRESSION_PAGE_LENGTH 16

#define CP_OFF_DCE_DCC          2
#define CP_OFF_C_ALGO           7

#define DCE_MASK  0x80
#define DCC_MASK  0x40
#define RED_MASK  0x60


/* Control the compression with mode page 15. Algorithm not changed if zero.

   The block descriptors are read and written because Sony SDT-7000 does not
   work without this (suggestion from Michael Schaefer <Michael.Schaefer@dlr.de>).
   Including block descriptors should not cause any harm to other drives. */

static int st_compression(struct scsi_tape * STp, int state)
{
	int retval;
	int mpoffs;  /* Offset to mode page start */
	unsigned char *b_data = (STp->buffer)->b_data;

	if (STp->ready != ST_READY)
		return (-EIO);

	/* Read the current page contents */
	retval = read_mode_page(STp, COMPRESSION_PAGE, 0);
	if (retval) {
		DEBC_printk(STp, "Compression mode page not supported.\n");
		return (-EIO);
	}

	mpoffs = MODE_HEADER_LENGTH + b_data[MH_OFF_BDESCS_LENGTH];
	DEBC_printk(STp, "Compression state is %d.\n",
		    (b_data[mpoffs + CP_OFF_DCE_DCC] & DCE_MASK ? 1 : 0));

	/* Check if compression can be changed */
	if ((b_data[mpoffs + CP_OFF_DCE_DCC] & DCC_MASK) == 0) {
		DEBC_printk(STp, "Compression not supported.\n");
		return (-EIO);
	}

	/* Do the change */
	if (state) {
		b_data[mpoffs + CP_OFF_DCE_DCC] |= DCE_MASK;
		if (STp->c_algo != 0)
			b_data[mpoffs + CP_OFF_C_ALGO] = STp->c_algo;
	}
	else {
		b_data[mpoffs + CP_OFF_DCE_DCC] &= ~DCE_MASK;
		if (STp->c_algo != 0)
			b_data[mpoffs + CP_OFF_C_ALGO] = 0; /* no compression */
	}

	retval = write_mode_page(STp, COMPRESSION_PAGE, 0);
	if (retval) {
		DEBC_printk(STp, "Compression change failed.\n");
		return (-EIO);
	}
	DEBC_printk(STp, "Compression state changed to %d.\n", state);

	STp->compression_changed = 1;
	return 0;
}


/* Process the load and unload commands (does unload if the load code is zero) */
static int do_load_unload(struct scsi_tape *STp, struct file *filp, int load_code)
{
	int retval = (-EIO), timeout;
	unsigned char cmd[MAX_COMMAND_SIZE];
	struct st_partstat *STps;
	struct st_request *SRpnt;

	if (STp->ready != ST_READY && !load_code) {
		if (STp->ready == ST_NO_TAPE)
			return (-ENOMEDIUM);
		else
			return (-EIO);
	}

	memset(cmd, 0, MAX_COMMAND_SIZE);
	cmd[0] = START_STOP;
	if (load_code)
		cmd[4] |= 1;
	/*
	 * If arg >= 1 && arg <= 6 Enhanced load/unload in HP C1553A
	 */
	if (load_code >= 1 + MT_ST_HPLOADER_OFFSET
	    && load_code <= 6 + MT_ST_HPLOADER_OFFSET) {
		DEBC_printk(STp, " Enhanced %sload slot %2d.\n",
			    (cmd[4]) ? "" : "un",
			    load_code - MT_ST_HPLOADER_OFFSET);
		cmd[3] = load_code - MT_ST_HPLOADER_OFFSET; /* MediaID field of C1553A */
	}
	if (STp->immediate) {
		cmd[1] = 1;	/* Don't wait for completion */
		timeout = STp->device->request_queue->rq_timeout;
	}
	else
		timeout = STp->long_timeout;

	DEBC(
		if (!load_code)
			st_printk(ST_DEB_MSG, STp, "Unloading tape.\n");
		else
			st_printk(ST_DEB_MSG, STp, "Loading tape.\n");
		);

	SRpnt = st_do_scsi(NULL, STp, cmd, 0, DMA_NONE,
			   timeout, MAX_RETRIES, 1);
	if (!SRpnt)
		return (STp->buffer)->syscall_result;

	retval = (STp->buffer)->syscall_result;
	st_release_request(SRpnt);

	if (!retval) {	/* SCSI command successful */

		if (!load_code) {
			STp->rew_at_close = 0;
			STp->ready = ST_NO_TAPE;
		}
		else {
			STp->rew_at_close = STp->autorew_dev;
			retval = check_tape(STp, filp);
			if (retval > 0)
				retval = 0;
		}
	}
	else {
		STps = &(STp->ps[STp->partition]);
		STps->drv_file = STps->drv_block = (-1);
	}

	return retval;
}

#if DEBUG
#define ST_DEB_FORWARD  0
#define ST_DEB_BACKWARD 1
static void deb_space_print(struct scsi_tape *STp, int direction, char *units, unsigned char *cmd)
{
	s32 sc;

	if (!debugging)
		return;

	sc = sign_extend32(get_unaligned_be24(&cmd[2]), 23);
	if (direction)
		sc = -sc;
	st_printk(ST_DEB_MSG, STp, "Spacing tape %s over %d %s.\n",
		  direction ? "backward" : "forward", sc, units);
}
#else
#define ST_DEB_FORWARD  0
#define ST_DEB_BACKWARD 1
static void deb_space_print(struct scsi_tape *STp, int direction, char *units, unsigned char *cmd) {}
#endif


/* Internal ioctl function */
static int st_int_ioctl(struct scsi_tape *STp, unsigned int cmd_in, unsigned long arg)
{
	int timeout;
	long ltmp;
	int ioctl_result;
	int chg_eof = 1;
	unsigned char cmd[MAX_COMMAND_SIZE];
	struct st_request *SRpnt;
	struct st_partstat *STps;
	int fileno, blkno, at_sm, undone;
	int datalen = 0, direction = DMA_NONE;

	WARN_ON(STp->buffer->do_dio != 0);
	if (STp->ready != ST_READY) {
		if (STp->ready == ST_NO_TAPE)
			return (-ENOMEDIUM);
		else
			return (-EIO);
	}
	timeout = STp->long_timeout;
	STps = &(STp->ps[STp->partition]);
	fileno = STps->drv_file;
	blkno = STps->drv_block;
	at_sm = STps->at_sm;

	memset(cmd, 0, MAX_COMMAND_SIZE);
	switch (cmd_in) {
	case MTFSFM:
		chg_eof = 0;	/* Changed from the FSF after this */
		fallthrough;
	case MTFSF:
		cmd[0] = SPACE;
		cmd[1] = 0x01;	/* Space FileMarks */
		cmd[2] = (arg >> 16);
		cmd[3] = (arg >> 8);
		cmd[4] = arg;
		deb_space_print(STp, ST_DEB_FORWARD, "filemarks", cmd);
		if (fileno >= 0)
			fileno += arg;
		blkno = 0;
		at_sm &= (arg == 0);
		break;
	case MTBSFM:
		chg_eof = 0;	/* Changed from the FSF after this */
		fallthrough;
	case MTBSF:
		cmd[0] = SPACE;
		cmd[1] = 0x01;	/* Space FileMarks */
		ltmp = (-arg);
		cmd[2] = (ltmp >> 16);
		cmd[3] = (ltmp >> 8);
		cmd[4] = ltmp;
		deb_space_print(STp, ST_DEB_BACKWARD, "filemarks", cmd);
		if (fileno >= 0)
			fileno -= arg;
		blkno = (-1);	/* We can't know the block number */
		at_sm &= (arg == 0);
		break;
	case MTFSR:
		cmd[0] = SPACE;
		cmd[1] = 0x00;	/* Space Blocks */
		cmd[2] = (arg >> 16);
		cmd[3] = (arg >> 8);
		cmd[4] = arg;
		deb_space_print(STp, ST_DEB_FORWARD, "blocks", cmd);
		if (blkno >= 0)
			blkno += arg;
		at_sm &= (arg == 0);
		break;
	case MTBSR:
		cmd[0] = SPACE;
		cmd[1] = 0x00;	/* Space Blocks */
		ltmp = (-arg);
		cmd[2] = (ltmp >> 16);
		cmd[3] = (ltmp >> 8);
		cmd[4] = ltmp;
		deb_space_print(STp, ST_DEB_BACKWARD, "blocks", cmd);
		if (blkno >= 0)
			blkno -= arg;
		at_sm &= (arg == 0);
		break;
	case MTFSS:
		cmd[0] = SPACE;
		cmd[1] = 0x04;	/* Space Setmarks */
		cmd[2] = (arg >> 16);
		cmd[3] = (arg >> 8);
		cmd[4] = arg;
		deb_space_print(STp, ST_DEB_FORWARD, "setmarks", cmd);
		if (arg != 0) {
			blkno = fileno = (-1);
			at_sm = 1;
		}
		break;
	case MTBSS:
		cmd[0] = SPACE;
		cmd[1] = 0x04;	/* Space Setmarks */
		ltmp = (-arg);
		cmd[2] = (ltmp >> 16);
		cmd[3] = (ltmp >> 8);
		cmd[4] = ltmp;
		deb_space_print(STp, ST_DEB_BACKWARD, "setmarks", cmd);
		if (arg != 0) {
			blkno = fileno = (-1);
			at_sm = 1;
		}
		break;
	case MTWEOF:
	case MTWEOFI:
	case MTWSM:
		if (STp->write_prot)
			return (-EACCES);
		cmd[0] = WRITE_FILEMARKS;
		if (cmd_in == MTWSM)
			cmd[1] = 2;
		if (cmd_in == MTWEOFI ||
		    (cmd_in == MTWEOF && STp->immediate_filemark))
			cmd[1] |= 1;
		cmd[2] = (arg >> 16);
		cmd[3] = (arg >> 8);
		cmd[4] = arg;
		timeout = STp->device->request_queue->rq_timeout;
		DEBC(
			if (cmd_in != MTWSM)
				st_printk(ST_DEB_MSG, STp,
					  "Writing %d filemarks.\n",
					  cmd[2] * 65536 +
					  cmd[3] * 256 +
					  cmd[4]);
			else
				st_printk(ST_DEB_MSG, STp,
					  "Writing %d setmarks.\n",
					  cmd[2] * 65536 +
					  cmd[3] * 256 +
					  cmd[4]);
		)
		if (fileno >= 0)
			fileno += arg;
		blkno = 0;
		at_sm = (cmd_in == MTWSM);
		break;
	case MTREW:
		cmd[0] = REZERO_UNIT;
		if (STp->immediate) {
			cmd[1] = 1;	/* Don't wait for completion */
			timeout = STp->device->request_queue->rq_timeout;
		}
		DEBC_printk(STp, "Rewinding tape.\n");
		fileno = blkno = at_sm = 0;
		break;
	case MTNOP:
		DEBC_printk(STp, "No op on tape.\n");
		return 0;	/* Should do something ? */
	case MTRETEN:
		cmd[0] = START_STOP;
		if (STp->immediate) {
			cmd[1] = 1;	/* Don't wait for completion */
			timeout = STp->device->request_queue->rq_timeout;
		}
		cmd[4] = 3;
		DEBC_printk(STp, "Retensioning tape.\n");
		fileno = blkno = at_sm = 0;
		break;
	case MTEOM:
		if (!STp->fast_mteom) {
			/* space to the end of tape */
			ioctl_result = st_int_ioctl(STp, MTFSF, 0x7fffff);
			fileno = STps->drv_file;
			if (STps->eof >= ST_EOD_1)
				return 0;
			/* The next lines would hide the number of spaced FileMarks
			   That's why I inserted the previous lines. I had no luck
			   with detecting EOM with FSF, so we go now to EOM.
			   Joerg Weule */
		} else
			fileno = (-1);
		cmd[0] = SPACE;
		cmd[1] = 3;
		DEBC_printk(STp, "Spacing to end of recorded medium.\n");
		blkno = -1;
		at_sm = 0;
		break;
	case MTERASE:
		if (STp->write_prot)
			return (-EACCES);
		cmd[0] = ERASE;
		cmd[1] = (arg ? 1 : 0);	/* Long erase with non-zero argument */
		if (STp->immediate) {
			cmd[1] |= 2;	/* Don't wait for completion */
			timeout = STp->device->request_queue->rq_timeout;
		}
		else
			timeout = STp->long_timeout * 8;

		DEBC_printk(STp, "Erasing tape.\n");
		fileno = blkno = at_sm = 0;
		break;
	case MTSETBLK:		/* Set block length */
	case MTSETDENSITY:	/* Set tape density */
	case MTSETDRVBUFFER:	/* Set drive buffering */
	case SET_DENS_AND_BLK:	/* Set density and block size */
		chg_eof = 0;
		if (STp->dirty || (STp->buffer)->buffer_bytes != 0)
			return (-EIO);	/* Not allowed if data in buffer */
		if ((cmd_in == MTSETBLK || cmd_in == SET_DENS_AND_BLK) &&
		    (arg & MT_ST_BLKSIZE_MASK) != 0 &&
		    STp->max_block > 0 &&
		    ((arg & MT_ST_BLKSIZE_MASK) < STp->min_block ||
		     (arg & MT_ST_BLKSIZE_MASK) > STp->max_block)) {
			st_printk(KERN_WARNING, STp, "Illegal block size.\n");
			return (-EINVAL);
		}
		cmd[0] = MODE_SELECT;
		if ((STp->use_pf & USE_PF))
			cmd[1] = MODE_SELECT_PAGE_FORMAT;
		cmd[4] = datalen = 12;
		direction = DMA_TO_DEVICE;

		memset((STp->buffer)->b_data, 0, 12);
		if (cmd_in == MTSETDRVBUFFER)
			(STp->buffer)->b_data[2] = (arg & 7) << 4;
		else
			(STp->buffer)->b_data[2] =
			    STp->drv_buffer << 4;
		(STp->buffer)->b_data[3] = 8;	/* block descriptor length */
		if (cmd_in == MTSETDENSITY) {
			(STp->buffer)->b_data[4] = arg;
			STp->density_changed = 1;	/* At least we tried ;-) */
		} else if (cmd_in == SET_DENS_AND_BLK)
			(STp->buffer)->b_data[4] = arg >> 24;
		else
			(STp->buffer)->b_data[4] = STp->density;
		if (cmd_in == MTSETBLK || cmd_in == SET_DENS_AND_BLK) {
			ltmp = arg & MT_ST_BLKSIZE_MASK;
			if (cmd_in == MTSETBLK)
				STp->blksize_changed = 1; /* At least we tried ;-) */
		} else
			ltmp = STp->block_size;
		(STp->buffer)->b_data[9] = (ltmp >> 16);
		(STp->buffer)->b_data[10] = (ltmp >> 8);
		(STp->buffer)->b_data[11] = ltmp;
		timeout = STp->device->request_queue->rq_timeout;
		DEBC(
			if (cmd_in == MTSETBLK || cmd_in == SET_DENS_AND_BLK)
				st_printk(ST_DEB_MSG, STp,
					  "Setting block size to %d bytes.\n",
					  (STp->buffer)->b_data[9] * 65536 +
					  (STp->buffer)->b_data[10] * 256 +
					  (STp->buffer)->b_data[11]);
			if (cmd_in == MTSETDENSITY || cmd_in == SET_DENS_AND_BLK)
				st_printk(ST_DEB_MSG, STp,
					  "Setting density code to %x.\n",
					  (STp->buffer)->b_data[4]);
			if (cmd_in == MTSETDRVBUFFER)
				st_printk(ST_DEB_MSG, STp,
					  "Setting drive buffer code to %d.\n",
					  ((STp->buffer)->b_data[2] >> 4) & 7);
		)
		break;
	default:
		return (-ENOSYS);
	}

	SRpnt = st_do_scsi(NULL, STp, cmd, datalen, direction,
			   timeout, MAX_RETRIES, 1);
	if (!SRpnt)
		return (STp->buffer)->syscall_result;

	ioctl_result = (STp->buffer)->syscall_result;

	if (!ioctl_result) {	/* SCSI command successful */
		st_release_request(SRpnt);
		SRpnt = NULL;
		STps->drv_block = blkno;
		STps->drv_file = fileno;
		STps->at_sm = at_sm;

		if (cmd_in == MTBSFM)
			ioctl_result = st_int_ioctl(STp, MTFSF, 1);
		else if (cmd_in == MTFSFM)
			ioctl_result = st_int_ioctl(STp, MTBSF, 1);

		if (cmd_in == MTSETBLK || cmd_in == SET_DENS_AND_BLK) {
			STp->block_size = arg & MT_ST_BLKSIZE_MASK;
			if (STp->block_size != 0) {
				(STp->buffer)->buffer_blocks =
				    (STp->buffer)->buffer_size / STp->block_size;
			}
			(STp->buffer)->buffer_bytes = (STp->buffer)->read_pointer = 0;
			if (cmd_in == SET_DENS_AND_BLK)
				STp->density = arg >> MT_ST_DENSITY_SHIFT;
		} else if (cmd_in == MTSETDRVBUFFER)
			STp->drv_buffer = (arg & 7);
		else if (cmd_in == MTSETDENSITY)
			STp->density = arg;

		if (cmd_in == MTEOM)
			STps->eof = ST_EOD;
		else if (cmd_in == MTFSF)
			STps->eof = ST_FM;
		else if (chg_eof)
			STps->eof = ST_NOEOF;

		if (cmd_in == MTWEOF || cmd_in == MTWEOFI)
			STps->rw = ST_IDLE;  /* prevent automatic WEOF at close */
	} else { /* SCSI command was not completely successful. Don't return
                    from this block without releasing the SCSI command block! */
		struct st_cmdstatus *cmdstatp = &STp->buffer->cmdstat;

		if (cmdstatp->flags & SENSE_EOM) {
			if (cmd_in != MTBSF && cmd_in != MTBSFM &&
			    cmd_in != MTBSR && cmd_in != MTBSS)
				STps->eof = ST_EOM_OK;
			STps->drv_block = 0;
		}

		if (cmdstatp->remainder_valid)
			undone = (int)cmdstatp->uremainder64;
		else
			undone = 0;

		if ((cmd_in == MTWEOF || cmd_in == MTWEOFI) &&
		    cmdstatp->have_sense &&
		    (cmdstatp->flags & SENSE_EOM)) {
			if (cmdstatp->sense_hdr.sense_key == NO_SENSE ||
			    cmdstatp->sense_hdr.sense_key == RECOVERED_ERROR) {
				ioctl_result = 0;	/* EOF(s) written successfully at EOM */
				STps->eof = ST_NOEOF;
			} else {  /* Writing EOF(s) failed */
				if (fileno >= 0)
					fileno -= undone;
				if (undone < arg)
					STps->eof = ST_NOEOF;
			}
			STps->drv_file = fileno;
		} else if ((cmd_in == MTFSF) || (cmd_in == MTFSFM)) {
			if (fileno >= 0)
				STps->drv_file = fileno - undone;
			else
				STps->drv_file = fileno;
			STps->drv_block = -1;
			STps->eof = ST_NOEOF;
		} else if ((cmd_in == MTBSF) || (cmd_in == MTBSFM)) {
			if (arg > 0 && undone < 0)  /* Some drives get this wrong */
				undone = (-undone);
			if (STps->drv_file >= 0)
				STps->drv_file = fileno + undone;
			STps->drv_block = 0;
			STps->eof = ST_NOEOF;
		} else if (cmd_in == MTFSR) {
			if (cmdstatp->flags & SENSE_FMK) {	/* Hit filemark */
				if (STps->drv_file >= 0)
					STps->drv_file++;
				STps->drv_block = 0;
				STps->eof = ST_FM;
			} else {
				if (blkno >= undone)
					STps->drv_block = blkno - undone;
				else
					STps->drv_block = (-1);
				STps->eof = ST_NOEOF;
			}
		} else if (cmd_in == MTBSR) {
			if (cmdstatp->flags & SENSE_FMK) {	/* Hit filemark */
				STps->drv_file--;
				STps->drv_block = (-1);
			} else {
				if (arg > 0 && undone < 0)  /* Some drives get this wrong */
					undone = (-undone);
				if (STps->drv_block >= 0)
					STps->drv_block = blkno + undone;
			}
			STps->eof = ST_NOEOF;
		} else if (cmd_in == MTEOM) {
			STps->drv_file = (-1);
			STps->drv_block = (-1);
			STps->eof = ST_EOD;
		} else if (cmd_in == MTSETBLK ||
			   cmd_in == MTSETDENSITY ||
			   cmd_in == MTSETDRVBUFFER ||
			   cmd_in == SET_DENS_AND_BLK) {
			if (cmdstatp->sense_hdr.sense_key == ILLEGAL_REQUEST &&
			    !(STp->use_pf & PF_TESTED)) {
				/* Try the other possible state of Page Format if not
				   already tried */
				STp->use_pf = (STp->use_pf ^ USE_PF) | PF_TESTED;
				st_release_request(SRpnt);
				SRpnt = NULL;
				return st_int_ioctl(STp, cmd_in, arg);
			}
		} else if (chg_eof)
			STps->eof = ST_NOEOF;

		if (cmdstatp->sense_hdr.sense_key == BLANK_CHECK)
			STps->eof = ST_EOD;

		st_release_request(SRpnt);
		SRpnt = NULL;
	}

	return ioctl_result;
}


/* Get the tape position. If bt == 2, arg points into a kernel space mt_loc
   structure. */

static int get_location(struct scsi_tape *STp, unsigned int *block, int *partition,
			int logical)
{
	int result;
	unsigned char scmd[MAX_COMMAND_SIZE];
	struct st_request *SRpnt;

	if (STp->ready != ST_READY)
		return (-EIO);

	memset(scmd, 0, MAX_COMMAND_SIZE);
	if ((STp->device)->scsi_level < SCSI_2) {
		scmd[0] = QFA_REQUEST_BLOCK;
		scmd[4] = 3;
	} else {
		scmd[0] = READ_POSITION;
		if (!logical && !STp->scsi2_logical)
			scmd[1] = 1;
	}
	SRpnt = st_do_scsi(NULL, STp, scmd, 20, DMA_FROM_DEVICE,
			   STp->device->request_queue->rq_timeout,
			   MAX_READY_RETRIES, 1);
	if (!SRpnt)
		return (STp->buffer)->syscall_result;

	if ((STp->buffer)->syscall_result != 0 ||
	    (STp->device->scsi_level >= SCSI_2 &&
	     ((STp->buffer)->b_data[0] & 4) != 0)) {
		*block = *partition = 0;
		DEBC_printk(STp, " Can't read tape position.\n");
		result = (-EIO);
	} else {
		result = 0;
		if ((STp->device)->scsi_level < SCSI_2) {
			*block = ((STp->buffer)->b_data[0] << 16)
			    + ((STp->buffer)->b_data[1] << 8)
			    + (STp->buffer)->b_data[2];
			*partition = 0;
		} else {
			*block = ((STp->buffer)->b_data[4] << 24)
			    + ((STp->buffer)->b_data[5] << 16)
			    + ((STp->buffer)->b_data[6] << 8)
			    + (STp->buffer)->b_data[7];
			*partition = (STp->buffer)->b_data[1];
			if (((STp->buffer)->b_data[0] & 0x80) &&
			    (STp->buffer)->b_data[1] == 0)	/* BOP of partition 0 */
				STp->ps[0].drv_block = STp->ps[0].drv_file = 0;
		}
		DEBC_printk(STp, "Got tape pos. blk %d part %d.\n",
			    *block, *partition);
	}
	st_release_request(SRpnt);
	SRpnt = NULL;

	return result;
}


/* Set the tape block and partition. Negative partition means that only the
   block should be set in vendor specific way. */
static int set_location(struct scsi_tape *STp, unsigned int block, int partition,
			int logical)
{
	struct st_partstat *STps;
	int result, p;
	unsigned int blk;
	int timeout;
	unsigned char scmd[MAX_COMMAND_SIZE];
	struct st_request *SRpnt;

	if (STp->ready != ST_READY)
		return (-EIO);
	timeout = STp->long_timeout;
	STps = &(STp->ps[STp->partition]);

	DEBC_printk(STp, "Setting block to %d and partition to %d.\n",
		    block, partition);
	DEB(if (partition < 0)
		return (-EIO); )

	/* Update the location at the partition we are leaving */
	if ((!STp->can_partitions && partition != 0) ||
	    partition >= ST_NBR_PARTITIONS)
		return (-EINVAL);
	if (partition != STp->partition) {
		if (get_location(STp, &blk, &p, 1))
			STps->last_block_valid = 0;
		else {
			STps->last_block_valid = 1;
			STps->last_block_visited = blk;
			DEBC_printk(STp, "Visited block %d for "
				    "partition %d saved.\n",
				    blk, STp->partition);
		}
	}

	memset(scmd, 0, MAX_COMMAND_SIZE);
	if ((STp->device)->scsi_level < SCSI_2) {
		scmd[0] = QFA_SEEK_BLOCK;
		scmd[2] = (block >> 16);
		scmd[3] = (block >> 8);
		scmd[4] = block;
		scmd[5] = 0;
	} else {
		scmd[0] = SEEK_10;
		scmd[3] = (block >> 24);
		scmd[4] = (block >> 16);
		scmd[5] = (block >> 8);
		scmd[6] = block;
		if (!logical && !STp->scsi2_logical)
			scmd[1] = 4;
		if (STp->partition != partition) {
			scmd[1] |= 2;
			scmd[8] = partition;
			DEBC_printk(STp, "Trying to change partition "
				    "from %d to %d\n", STp->partition,
				    partition);
		}
	}
	if (STp->immediate) {
		scmd[1] |= 1;		/* Don't wait for completion */
		timeout = STp->device->request_queue->rq_timeout;
	}

	SRpnt = st_do_scsi(NULL, STp, scmd, 0, DMA_NONE,
			   timeout, MAX_READY_RETRIES, 1);
	if (!SRpnt)
		return (STp->buffer)->syscall_result;

	STps->drv_block = STps->drv_file = (-1);
	STps->eof = ST_NOEOF;
	if ((STp->buffer)->syscall_result != 0) {
		result = (-EIO);
		if (STp->can_partitions &&
		    (STp->device)->scsi_level >= SCSI_2 &&
		    (p = find_partition(STp)) >= 0)
			STp->partition = p;
	} else {
		if (STp->can_partitions) {
			STp->partition = partition;
			STps = &(STp->ps[partition]);
			if (!STps->last_block_valid ||
			    STps->last_block_visited != block) {
				STps->at_sm = 0;
				STps->rw = ST_IDLE;
			}
		} else
			STps->at_sm = 0;
		if (block == 0)
			STps->drv_block = STps->drv_file = 0;
		result = 0;
	}

	st_release_request(SRpnt);
	SRpnt = NULL;

	return result;
}


/* Find the current partition number for the drive status. Called from open and
   returns either partition number of negative error code. */
static int find_partition(struct scsi_tape *STp)
{
	int i, partition;
	unsigned int block;

	if ((i = get_location(STp, &block, &partition, 1)) < 0)
		return i;
	if (partition >= ST_NBR_PARTITIONS)
		return (-EIO);
	return partition;
}


/* Change the partition if necessary */
static int switch_partition(struct scsi_tape *STp)
{
	struct st_partstat *STps;

	if (STp->partition == STp->new_partition)
		return 0;
	STps = &(STp->ps[STp->new_partition]);
	if (!STps->last_block_valid)
		STps->last_block_visited = 0;
	return set_location(STp, STps->last_block_visited, STp->new_partition, 1);
}

/* Functions for reading and writing the medium partition mode page. */

#define PART_PAGE   0x11
#define PART_PAGE_FIXED_LENGTH 8

#define PP_OFF_MAX_ADD_PARTS   2
#define PP_OFF_NBR_ADD_PARTS   3
#define PP_OFF_FLAGS           4
#define PP_OFF_PART_UNITS      6
#define PP_OFF_RESERVED        7

#define PP_BIT_IDP             0x20
#define PP_BIT_FDP             0x80
#define PP_MSK_PSUM_MB         0x10
#define PP_MSK_PSUM_UNITS      0x18
#define PP_MSK_POFM            0x04

/* Get the number of partitions on the tape. As a side effect reads the
   mode page into the tape buffer. */
static int nbr_partitions(struct scsi_tape *STp)
{
	int result;

	if (STp->ready != ST_READY)
		return (-EIO);

	result = read_mode_page(STp, PART_PAGE, 1);

	if (result) {
		DEBC_printk(STp, "Can't read medium partition page.\n");
		result = (-EIO);
	} else {
		result = (STp->buffer)->b_data[MODE_HEADER_LENGTH +
					      PP_OFF_NBR_ADD_PARTS] + 1;
		DEBC_printk(STp, "Number of partitions %d.\n", result);
	}

	return result;
}


static int format_medium(struct scsi_tape *STp, int format)
{
	int result = 0;
	int timeout = STp->long_timeout;
	unsigned char scmd[MAX_COMMAND_SIZE];
	struct st_request *SRpnt;

	memset(scmd, 0, MAX_COMMAND_SIZE);
	scmd[0] = FORMAT_UNIT;
	scmd[2] = format;
	if (STp->immediate) {
		scmd[1] |= 1;		/* Don't wait for completion */
		timeout = STp->device->request_queue->rq_timeout;
	}
	DEBC_printk(STp, "Sending FORMAT MEDIUM\n");
	SRpnt = st_do_scsi(NULL, STp, scmd, 0, DMA_NONE,
			   timeout, MAX_RETRIES, 1);
	if (!SRpnt)
		result = STp->buffer->syscall_result;
	return result;
}


/* Partition the tape into two partitions if size > 0 or one partition if
   size == 0.

   The block descriptors are read and written because Sony SDT-7000 does not
   work without this (suggestion from Michael Schaefer <Michael.Schaefer@dlr.de>).

   My HP C1533A drive returns only one partition size field. This is used to
   set the size of partition 1. There is no size field for the default partition.
   Michael Schaefer's Sony SDT-7000 returns two descriptors and the second is
   used to set the size of partition 1 (this is what the SCSI-3 standard specifies).
   The following algorithm is used to accommodate both drives: if the number of
   partition size fields is greater than the maximum number of additional partitions
   in the mode page, the second field is used. Otherwise the first field is used.

   For Seagate DDS drives the page length must be 8 when no partitions is defined
   and 10 when 1 partition is defined (information from Eric Lee Green). This is
   is acceptable also to some other old drives and enforced if the first partition
   size field is used for the first additional partition size.

   For drives that advertize SCSI-3 or newer, use the SSC-3 methods.
 */
static int partition_tape(struct scsi_tape *STp, int size)
{
	int result;
	int target_partition;
	bool scsi3 = STp->device->scsi_level >= SCSI_3, needs_format = false;
	int pgo, psd_cnt, psdo;
	int psum = PP_MSK_PSUM_MB, units = 0;
	unsigned char *bp;

	result = read_mode_page(STp, PART_PAGE, 0);
	if (result) {
		DEBC_printk(STp, "Can't read partition mode page.\n");
		return result;
	}
	target_partition = 1;
	if (size < 0) {
		target_partition = 0;
		size = -size;
	}

	/* The mode page is in the buffer. Let's modify it and write it. */
	bp = (STp->buffer)->b_data;
	pgo = MODE_HEADER_LENGTH + bp[MH_OFF_BDESCS_LENGTH];
	DEBC_printk(STp, "Partition page length is %d bytes.\n",
		    bp[pgo + MP_OFF_PAGE_LENGTH] + 2);

	psd_cnt = (bp[pgo + MP_OFF_PAGE_LENGTH] + 2 - PART_PAGE_FIXED_LENGTH) / 2;

	if (scsi3) {
		needs_format = (bp[pgo + PP_OFF_FLAGS] & PP_MSK_POFM) != 0;
		if (needs_format && size == 0) {
			/* No need to write the mode page when clearing
			 *  partitioning
			 */
			DEBC_printk(STp, "Formatting tape with one partition.\n");
			result = format_medium(STp, 0);
			goto out;
		}
		if (needs_format)  /* Leave the old value for HP DATs claiming SCSI_3 */
			psd_cnt = 2;
		if ((bp[pgo + PP_OFF_FLAGS] & PP_MSK_PSUM_UNITS) == PP_MSK_PSUM_UNITS) {
			/* Use units scaling for large partitions if the device
			 * suggests it and no precision lost. Required for IBM
			 * TS1140/50 drives that don't support MB units.
			 */
			if (size >= 1000 && (size % 1000) == 0) {
				size /= 1000;
				psum = PP_MSK_PSUM_UNITS;
				units = 9; /* GB */
			}
		}
		/* Try it anyway if too large to specify in MB */
		if (psum == PP_MSK_PSUM_MB && size >= 65534) {
			size /= 1000;
			psum = PP_MSK_PSUM_UNITS;
			units = 9;  /* GB */
		}
	}

	if (size >= 65535 ||  /* Does not fit into two bytes */
	    (target_partition == 0 && psd_cnt < 2)) {
		result = -EINVAL;
		goto out;
	}

	psdo = pgo + PART_PAGE_FIXED_LENGTH;
	/* The second condition is for HP DDS which use only one partition size
	 * descriptor
	 */
	if (target_partition > 0 &&
	    (psd_cnt > bp[pgo + PP_OFF_MAX_ADD_PARTS] ||
	     bp[pgo + PP_OFF_MAX_ADD_PARTS] != 1)) {
		bp[psdo] = bp[psdo + 1] = 0xff;  /* Rest to partition 0 */
		psdo += 2;
	}
	memset(bp + psdo, 0, bp[pgo + PP_OFF_NBR_ADD_PARTS] * 2);

	DEBC_printk(STp, "psd_cnt %d, max.parts %d, nbr_parts %d\n",
		    psd_cnt, bp[pgo + PP_OFF_MAX_ADD_PARTS],
		    bp[pgo + PP_OFF_NBR_ADD_PARTS]);

	if (size == 0) {
		bp[pgo + PP_OFF_NBR_ADD_PARTS] = 0;
		if (psd_cnt <= bp[pgo + PP_OFF_MAX_ADD_PARTS])
		    bp[pgo + MP_OFF_PAGE_LENGTH] = 6;
		DEBC_printk(STp, "Formatting tape with one partition.\n");
	} else {
		bp[psdo] = (size >> 8) & 0xff;
		bp[psdo + 1] = size & 0xff;
		if (target_partition == 0)
			bp[psdo + 2] = bp[psdo + 3] = 0xff;
		bp[pgo + 3] = 1;
		if (bp[pgo + MP_OFF_PAGE_LENGTH] < 8)
		    bp[pgo + MP_OFF_PAGE_LENGTH] = 8;
		DEBC_printk(STp,
			    "Formatting tape with two partitions (%i = %d MB).\n",
			    target_partition, units > 0 ? size * 1000 : size);
	}
	bp[pgo + PP_OFF_PART_UNITS] = 0;
	bp[pgo + PP_OFF_RESERVED] = 0;
	if (size != 1 || units != 0) {
		bp[pgo + PP_OFF_FLAGS] = PP_BIT_IDP | psum |
			(bp[pgo + PP_OFF_FLAGS] & 0x07);
		bp[pgo + PP_OFF_PART_UNITS] = units;
	} else
		bp[pgo + PP_OFF_FLAGS] = PP_BIT_FDP |
			(bp[pgo + PP_OFF_FLAGS] & 0x1f);
	bp[pgo + MP_OFF_PAGE_LENGTH] = 6 + psd_cnt * 2;

	result = write_mode_page(STp, PART_PAGE, 1);

	if (!result && needs_format)
		result = format_medium(STp, 1);

	if (result) {
		st_printk(KERN_INFO, STp, "Partitioning of tape failed.\n");
		result = (-EIO);
	}

out:
	return result;
}



/* The ioctl command */
static long st_ioctl_common(struct file *file, unsigned int cmd_in, void __user *p)
{
	int i, cmd_nr, cmd_type, bt;
	int retval = 0;
	unsigned int blk;
	struct scsi_tape *STp = file->private_data;
	struct st_modedef *STm;
	struct st_partstat *STps;

	if (mutex_lock_interruptible(&STp->lock))
		return -ERESTARTSYS;

	DEB(
	if (debugging && !STp->in_use) {
		st_printk(ST_DEB_MSG, STp, "Incorrect device.\n");
		retval = (-EIO);
		goto out;
	} ) /* end DEB */

	STm = &(STp->modes[STp->current_mode]);
	STps = &(STp->ps[STp->partition]);

	/*
	 * If we are in the middle of error recovery, don't let anyone
	 * else try and use this device.  Also, if error recovery fails, it
	 * may try and take the device offline, in which case all further
	 * access to the device is prohibited.
	 */
	retval = scsi_ioctl_block_when_processing_errors(STp->device, cmd_in,
			file->f_flags & O_NDELAY);
	if (retval)
		goto out;

	cmd_type = _IOC_TYPE(cmd_in);
	cmd_nr = _IOC_NR(cmd_in);

	if (cmd_type == _IOC_TYPE(MTIOCTOP) && cmd_nr == _IOC_NR(MTIOCTOP)) {
		struct mtop mtc;

		if (_IOC_SIZE(cmd_in) != sizeof(mtc)) {
			retval = (-EINVAL);
			goto out;
		}

		i = copy_from_user(&mtc, p, sizeof(struct mtop));
		if (i) {
			retval = (-EFAULT);
			goto out;
		}

		if (mtc.mt_op == MTSETDRVBUFFER && !capable(CAP_SYS_ADMIN)) {
			st_printk(KERN_WARNING, STp,
				  "MTSETDRVBUFFER only allowed for root.\n");
			retval = (-EPERM);
			goto out;
		}
		if (!STm->defined &&
		    (mtc.mt_op != MTSETDRVBUFFER &&
		     (mtc.mt_count & MT_ST_OPTIONS) == 0)) {
			retval = (-ENXIO);
			goto out;
		}

		if (!STp->pos_unknown) {

			if (STps->eof == ST_FM_HIT) {
				if (mtc.mt_op == MTFSF || mtc.mt_op == MTFSFM ||
                                    mtc.mt_op == MTEOM) {
					mtc.mt_count -= 1;
					if (STps->drv_file >= 0)
						STps->drv_file += 1;
				} else if (mtc.mt_op == MTBSF || mtc.mt_op == MTBSFM) {
					mtc.mt_count += 1;
					if (STps->drv_file >= 0)
						STps->drv_file += 1;
				}
			}

			if (mtc.mt_op == MTSEEK) {
				/* Old position must be restored if partition will be
                                   changed */
				i = !STp->can_partitions ||
				    (STp->new_partition != STp->partition);
			} else {
				i = mtc.mt_op == MTREW || mtc.mt_op == MTOFFL ||
				    mtc.mt_op == MTRETEN || mtc.mt_op == MTEOM ||
				    mtc.mt_op == MTLOCK || mtc.mt_op == MTLOAD ||
				    mtc.mt_op == MTFSF || mtc.mt_op == MTFSFM ||
				    mtc.mt_op == MTBSF || mtc.mt_op == MTBSFM ||
				    mtc.mt_op == MTCOMPRESSION;
			}
			i = flush_buffer(STp, i);
			if (i < 0) {
				retval = i;
				goto out;
			}
			if (STps->rw == ST_WRITING &&
			    (mtc.mt_op == MTREW || mtc.mt_op == MTOFFL ||
			     mtc.mt_op == MTSEEK ||
			     mtc.mt_op == MTBSF || mtc.mt_op == MTBSFM)) {
				i = st_int_ioctl(STp, MTWEOF, 1);
				if (i < 0) {
					retval = i;
					goto out;
				}
				if (mtc.mt_op == MTBSF || mtc.mt_op == MTBSFM)
					mtc.mt_count++;
				STps->rw = ST_IDLE;
			     }

		} else {
			/*
			 * If there was a bus reset, block further access
			 * to this device.  If the user wants to rewind the tape,
			 * then reset the flag and allow access again.
			 */
			if (mtc.mt_op != MTREW &&
			    mtc.mt_op != MTOFFL &&
			    mtc.mt_op != MTRETEN &&
			    mtc.mt_op != MTERASE &&
			    mtc.mt_op != MTSEEK &&
			    mtc.mt_op != MTEOM) {
				retval = (-EIO);
				goto out;
			}
			reset_state(STp);
			/* remove this when the midlevel properly clears was_reset */
			STp->device->was_reset = 0;
		}

		if (mtc.mt_op != MTNOP && mtc.mt_op != MTSETBLK &&
		    mtc.mt_op != MTSETDENSITY && mtc.mt_op != MTWSM &&
		    mtc.mt_op != MTSETDRVBUFFER && mtc.mt_op != MTSETPART)
			STps->rw = ST_IDLE;	/* Prevent automatic WEOF and fsf */

		if (mtc.mt_op == MTOFFL && STp->door_locked != ST_UNLOCKED)
			do_door_lock(STp, 0);	/* Ignore result! */

		if (mtc.mt_op == MTSETDRVBUFFER &&
		    (mtc.mt_count & MT_ST_OPTIONS) != 0) {
			retval = st_set_options(STp, mtc.mt_count);
			goto out;
		}

		if (mtc.mt_op == MTSETPART) {
			if (!STp->can_partitions ||
			    mtc.mt_count < 0 || mtc.mt_count >= ST_NBR_PARTITIONS) {
				retval = (-EINVAL);
				goto out;
			}
			if (mtc.mt_count >= STp->nbr_partitions &&
			    (STp->nbr_partitions = nbr_partitions(STp)) < 0) {
				retval = (-EIO);
				goto out;
			}
			if (mtc.mt_count >= STp->nbr_partitions) {
				retval = (-EINVAL);
				goto out;
			}
			STp->new_partition = mtc.mt_count;
			retval = 0;
			goto out;
		}

		if (mtc.mt_op == MTMKPART) {
			if (!STp->can_partitions) {
				retval = (-EINVAL);
				goto out;
			}
			i = do_load_unload(STp, file, 1);
			if (i < 0) {
				retval = i;
				goto out;
			}
			i = partition_tape(STp, mtc.mt_count);
			if (i < 0) {
				retval = i;
				goto out;
			}
			for (i = 0; i < ST_NBR_PARTITIONS; i++) {
				STp->ps[i].rw = ST_IDLE;
				STp->ps[i].at_sm = 0;
				STp->ps[i].last_block_valid = 0;
			}
			STp->partition = STp->new_partition = 0;
			STp->nbr_partitions = mtc.mt_count != 0 ? 2 : 1;
			STps->drv_block = STps->drv_file = 0;
			retval = 0;
			goto out;
		}

		if (mtc.mt_op == MTSEEK) {
			i = set_location(STp, mtc.mt_count, STp->new_partition, 0);
			if (!STp->can_partitions)
				STp->ps[0].rw = ST_IDLE;
			retval = i;
			goto out;
		}

		if (mtc.mt_op == MTUNLOAD || mtc.mt_op == MTOFFL) {
			retval = do_load_unload(STp, file, 0);
			goto out;
		}

		if (mtc.mt_op == MTLOAD) {
			retval = do_load_unload(STp, file, max(1, mtc.mt_count));
			goto out;
		}

		if (mtc.mt_op == MTLOCK || mtc.mt_op == MTUNLOCK) {
			retval = do_door_lock(STp, (mtc.mt_op == MTLOCK));
			goto out;
		}

		if (STp->can_partitions && STp->ready == ST_READY &&
		    (i = switch_partition(STp)) < 0) {
			retval = i;
			goto out;
		}

		if (mtc.mt_op == MTCOMPRESSION)
			retval = st_compression(STp, (mtc.mt_count & 1));
		else
			retval = st_int_ioctl(STp, mtc.mt_op, mtc.mt_count);
		goto out;
	}
	if (!STm->defined) {
		retval = (-ENXIO);
		goto out;
	}

	if ((i = flush_buffer(STp, 0)) < 0) {
		retval = i;
		goto out;
	}
	if (STp->can_partitions &&
	    (i = switch_partition(STp)) < 0) {
		retval = i;
		goto out;
	}

	if (cmd_type == _IOC_TYPE(MTIOCGET) && cmd_nr == _IOC_NR(MTIOCGET)) {
		struct mtget mt_status;

		if (_IOC_SIZE(cmd_in) != sizeof(struct mtget)) {
			 retval = (-EINVAL);
			 goto out;
		}

		mt_status.mt_type = STp->tape_type;
		mt_status.mt_dsreg =
		    ((STp->block_size << MT_ST_BLKSIZE_SHIFT) & MT_ST_BLKSIZE_MASK) |
		    ((STp->density << MT_ST_DENSITY_SHIFT) & MT_ST_DENSITY_MASK);
		mt_status.mt_blkno = STps->drv_block;
		mt_status.mt_fileno = STps->drv_file;
		if (STp->block_size != 0) {
			if (STps->rw == ST_WRITING)
				mt_status.mt_blkno +=
				    (STp->buffer)->buffer_bytes / STp->block_size;
			else if (STps->rw == ST_READING)
				mt_status.mt_blkno -=
                                        ((STp->buffer)->buffer_bytes +
                                         STp->block_size - 1) / STp->block_size;
		}

		mt_status.mt_gstat = 0;
		if (STp->drv_write_prot)
			mt_status.mt_gstat |= GMT_WR_PROT(0xffffffff);
		if (mt_status.mt_blkno == 0) {
			if (mt_status.mt_fileno == 0)
				mt_status.mt_gstat |= GMT_BOT(0xffffffff);
			else
				mt_status.mt_gstat |= GMT_EOF(0xffffffff);
		}
		mt_status.mt_erreg = (STp->recover_reg << MT_ST_SOFTERR_SHIFT);
		mt_status.mt_resid = STp->partition;
		if (STps->eof == ST_EOM_OK || STps->eof == ST_EOM_ERROR)
			mt_status.mt_gstat |= GMT_EOT(0xffffffff);
		else if (STps->eof >= ST_EOM_OK)
			mt_status.mt_gstat |= GMT_EOD(0xffffffff);
		if (STp->density == 1)
			mt_status.mt_gstat |= GMT_D_800(0xffffffff);
		else if (STp->density == 2)
			mt_status.mt_gstat |= GMT_D_1600(0xffffffff);
		else if (STp->density == 3)
			mt_status.mt_gstat |= GMT_D_6250(0xffffffff);
		if (STp->ready == ST_READY)
			mt_status.mt_gstat |= GMT_ONLINE(0xffffffff);
		if (STp->ready == ST_NO_TAPE)
			mt_status.mt_gstat |= GMT_DR_OPEN(0xffffffff);
		if (STps->at_sm)
			mt_status.mt_gstat |= GMT_SM(0xffffffff);
		if (STm->do_async_writes ||
                    (STm->do_buffer_writes && STp->block_size != 0) ||
		    STp->drv_buffer != 0)
			mt_status.mt_gstat |= GMT_IM_REP_EN(0xffffffff);
		if (STp->cleaning_req)
			mt_status.mt_gstat |= GMT_CLN(0xffffffff);

		retval = put_user_mtget(p, &mt_status);
		if (retval)
			goto out;

		STp->recover_reg = 0;		/* Clear after read */
		goto out;
	}			/* End of MTIOCGET */
	if (cmd_type == _IOC_TYPE(MTIOCPOS) && cmd_nr == _IOC_NR(MTIOCPOS)) {
		struct mtpos mt_pos;
		if (_IOC_SIZE(cmd_in) != sizeof(struct mtpos)) {
			 retval = (-EINVAL);
			 goto out;
		}
		if ((i = get_location(STp, &blk, &bt, 0)) < 0) {
			retval = i;
			goto out;
		}
		mt_pos.mt_blkno = blk;
		retval = put_user_mtpos(p, &mt_pos);
		goto out;
	}
	mutex_unlock(&STp->lock);
	switch (cmd_in) {
		case SCSI_IOCTL_STOP_UNIT:
			/* unload */
			retval = scsi_ioctl(STp->device, cmd_in, p);
			if (!retval) {
				STp->rew_at_close = 0;
				STp->ready = ST_NO_TAPE;
			}
			return retval;

		case SCSI_IOCTL_GET_IDLUN:
		case SCSI_IOCTL_GET_BUS_NUMBER:
			break;

		default:
			if ((cmd_in == SG_IO ||
			     cmd_in == SCSI_IOCTL_SEND_COMMAND ||
			     cmd_in == CDROM_SEND_PACKET) &&
			    !capable(CAP_SYS_RAWIO))
				i = -EPERM;
			else
				i = scsi_cmd_ioctl(STp->disk->queue, STp->disk,
						   file->f_mode, cmd_in, p);
			if (i != -ENOTTY)
				return i;
			break;
	}
	return -ENOTTY;

 out:
	mutex_unlock(&STp->lock);
	return retval;
}

static long st_ioctl(struct file *file, unsigned int cmd_in, unsigned long arg)
{
	void __user *p = (void __user *)arg;
	struct scsi_tape *STp = file->private_data;
	int ret;

	ret = st_ioctl_common(file, cmd_in, p);
	if (ret != -ENOTTY)
		return ret;

	return scsi_ioctl(STp->device, cmd_in, p);
}

#ifdef CONFIG_COMPAT
static long st_compat_ioctl(struct file *file, unsigned int cmd_in, unsigned long arg)
{
	void __user *p = compat_ptr(arg);
	struct scsi_tape *STp = file->private_data;
	int ret;

	/* argument conversion is handled using put_user_mtpos/put_user_mtget */
	switch (cmd_in) {
	case MTIOCPOS32:
		return st_ioctl_common(file, MTIOCPOS, p);
	case MTIOCGET32:
		return st_ioctl_common(file, MTIOCGET, p);
	}

	ret = st_ioctl_common(file, cmd_in, p);
	if (ret != -ENOTTY)
		return ret;

	return scsi_compat_ioctl(STp->device, cmd_in, p);
}
#endif



/* Try to allocate a new tape buffer. Calling function must not hold
   dev_arr_lock. */
static struct st_buffer *new_tape_buffer(int max_sg)
{
	struct st_buffer *tb;

	tb = kzalloc(sizeof(struct st_buffer), GFP_KERNEL);
	if (!tb) {
		printk(KERN_NOTICE "st: Can't allocate new tape buffer.\n");
		return NULL;
	}
	tb->frp_segs = 0;
	tb->use_sg = max_sg;
	tb->buffer_size = 0;

	tb->reserved_pages = kcalloc(max_sg, sizeof(struct page *),
				     GFP_KERNEL);
	if (!tb->reserved_pages) {
		kfree(tb);
		return NULL;
	}

	return tb;
}


/* Try to allocate enough space in the tape buffer */
#define ST_MAX_ORDER 6

static int enlarge_buffer(struct st_buffer * STbuffer, int new_size)
{
	int segs, max_segs, b_size, order, got;
	gfp_t priority;

	if (new_size <= STbuffer->buffer_size)
		return 1;

	if (STbuffer->buffer_size <= PAGE_SIZE)
		normalize_buffer(STbuffer);  /* Avoid extra segment */

	max_segs = STbuffer->use_sg;

	priority = GFP_KERNEL | __GFP_NOWARN;

	if (STbuffer->cleared)
		priority |= __GFP_ZERO;

	if (STbuffer->frp_segs) {
		order = STbuffer->reserved_page_order;
		b_size = PAGE_SIZE << order;
	} else {
		for (b_size = PAGE_SIZE, order = 0;
		     order < ST_MAX_ORDER &&
			     max_segs * (PAGE_SIZE << order) < new_size;
		     order++, b_size *= 2)
			;  /* empty */
		STbuffer->reserved_page_order = order;
	}
	if (max_segs * (PAGE_SIZE << order) < new_size) {
		if (order == ST_MAX_ORDER)
			return 0;
		normalize_buffer(STbuffer);
		return enlarge_buffer(STbuffer, new_size);
	}

	for (segs = STbuffer->frp_segs, got = STbuffer->buffer_size;
	     segs < max_segs && got < new_size;) {
		struct page *page;

		page = alloc_pages(priority, order);
		if (!page) {
			DEB(STbuffer->buffer_size = got);
			normalize_buffer(STbuffer);
			return 0;
		}

		STbuffer->frp_segs += 1;
		got += b_size;
		STbuffer->buffer_size = got;
		STbuffer->reserved_pages[segs] = page;
		segs++;
	}
	STbuffer->b_data = page_address(STbuffer->reserved_pages[0]);

	return 1;
}


/* Make sure that no data from previous user is in the internal buffer */
static void clear_buffer(struct st_buffer * st_bp)
{
	int i;

	for (i=0; i < st_bp->frp_segs; i++)
		memset(page_address(st_bp->reserved_pages[i]), 0,
		       PAGE_SIZE << st_bp->reserved_page_order);
	st_bp->cleared = 1;
}


/* Release the extra buffer */
static void normalize_buffer(struct st_buffer * STbuffer)
{
	int i, order = STbuffer->reserved_page_order;

	for (i = 0; i < STbuffer->frp_segs; i++) {
		__free_pages(STbuffer->reserved_pages[i], order);
		STbuffer->buffer_size -= (PAGE_SIZE << order);
	}
	STbuffer->frp_segs = 0;
	STbuffer->sg_segs = 0;
	STbuffer->reserved_page_order = 0;
	STbuffer->map_data.offset = 0;
}


/* Move data from the user buffer to the tape buffer. Returns zero (success) or
   negative error code. */
static int append_to_buffer(const char __user *ubp, struct st_buffer * st_bp, int do_count)
{
	int i, cnt, res, offset;
	int length = PAGE_SIZE << st_bp->reserved_page_order;

	for (i = 0, offset = st_bp->buffer_bytes;
	     i < st_bp->frp_segs && offset >= length; i++)
		offset -= length;
	if (i == st_bp->frp_segs) {	/* Should never happen */
		printk(KERN_WARNING "st: append_to_buffer offset overflow.\n");
		return (-EIO);
	}
	for (; i < st_bp->frp_segs && do_count > 0; i++) {
		struct page *page = st_bp->reserved_pages[i];
		cnt = length - offset < do_count ? length - offset : do_count;
		res = copy_from_user(page_address(page) + offset, ubp, cnt);
		if (res)
			return (-EFAULT);
		do_count -= cnt;
		st_bp->buffer_bytes += cnt;
		ubp += cnt;
		offset = 0;
	}
	if (do_count) /* Should never happen */
		return (-EIO);

	return 0;
}


/* Move data from the tape buffer to the user buffer. Returns zero (success) or
   negative error code. */
static int from_buffer(struct st_buffer * st_bp, char __user *ubp, int do_count)
{
	int i, cnt, res, offset;
	int length = PAGE_SIZE << st_bp->reserved_page_order;

	for (i = 0, offset = st_bp->read_pointer;
	     i < st_bp->frp_segs && offset >= length; i++)
		offset -= length;
	if (i == st_bp->frp_segs) {	/* Should never happen */
		printk(KERN_WARNING "st: from_buffer offset overflow.\n");
		return (-EIO);
	}
	for (; i < st_bp->frp_segs && do_count > 0; i++) {
		struct page *page = st_bp->reserved_pages[i];
		cnt = length - offset < do_count ? length - offset : do_count;
		res = copy_to_user(ubp, page_address(page) + offset, cnt);
		if (res)
			return (-EFAULT);
		do_count -= cnt;
		st_bp->buffer_bytes -= cnt;
		st_bp->read_pointer += cnt;
		ubp += cnt;
		offset = 0;
	}
	if (do_count) /* Should never happen */
		return (-EIO);

	return 0;
}


/* Move data towards start of buffer */
static void move_buffer_data(struct st_buffer * st_bp, int offset)
{
	int src_seg, dst_seg, src_offset = 0, dst_offset;
	int count, total;
	int length = PAGE_SIZE << st_bp->reserved_page_order;

	if (offset == 0)
		return;

	total=st_bp->buffer_bytes - offset;
	for (src_seg=0; src_seg < st_bp->frp_segs; src_seg++) {
		src_offset = offset;
		if (src_offset < length)
			break;
		offset -= length;
	}

	st_bp->buffer_bytes = st_bp->read_pointer = total;
	for (dst_seg=dst_offset=0; total > 0; ) {
		struct page *dpage = st_bp->reserved_pages[dst_seg];
		struct page *spage = st_bp->reserved_pages[src_seg];

		count = min(length - dst_offset, length - src_offset);
		memmove(page_address(dpage) + dst_offset,
			page_address(spage) + src_offset, count);
		src_offset += count;
		if (src_offset >= length) {
			src_seg++;
			src_offset = 0;
		}
		dst_offset += count;
		if (dst_offset >= length) {
			dst_seg++;
			dst_offset = 0;
		}
		total -= count;
	}
}

/* Validate the options from command line or module parameters */
static void validate_options(void)
{
	if (buffer_kbs > 0)
		st_fixed_buffer_size = buffer_kbs * ST_KILOBYTE;
	if (max_sg_segs >= ST_FIRST_SG)
		st_max_sg_segs = max_sg_segs;
}

#ifndef MODULE
/* Set the boot options. Syntax is defined in Documenation/scsi/st.txt.
 */
static int __init st_setup(char *str)
{
	int i, len, ints[5];
	char *stp;

	stp = get_options(str, ARRAY_SIZE(ints), ints);

	if (ints[0] > 0) {
		for (i = 0; i < ints[0] && i < ARRAY_SIZE(parms); i++)
			if (parms[i].val)
				*parms[i].val = ints[i + 1];
	} else {
		while (stp != NULL) {
			for (i = 0; i < ARRAY_SIZE(parms); i++) {
				len = strlen(parms[i].name);
				if (!strncmp(stp, parms[i].name, len) &&
				    (*(stp + len) == ':' || *(stp + len) == '=')) {
					if (parms[i].val)
						*parms[i].val =
							simple_strtoul(stp + len + 1, NULL, 0);
					else
						printk(KERN_WARNING "st: Obsolete parameter %s\n",
						       parms[i].name);
					break;
				}
			}
			if (i >= ARRAY_SIZE(parms))
				 printk(KERN_WARNING "st: invalid parameter in '%s'\n",
					stp);
			stp = strchr(stp, ',');
			if (stp)
				stp++;
		}
	}

	validate_options();

	return 1;
}

__setup("st=", st_setup);

#endif

static const struct file_operations st_fops =
{
	.owner =	THIS_MODULE,
	.read =		st_read,
	.write =	st_write,
	.unlocked_ioctl = st_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl = st_compat_ioctl,
#endif
	.open =		st_open,
	.flush =	st_flush,
	.release =	st_release,
	.llseek =	noop_llseek,
};

static int create_one_cdev(struct scsi_tape *tape, int mode, int rew)
{
	int i, error;
	dev_t cdev_devno;
	struct cdev *cdev;
	struct device *dev;
	struct st_modedef *STm = &(tape->modes[mode]);
	char name[10];
	int dev_num = tape->index;

	cdev_devno = MKDEV(SCSI_TAPE_MAJOR, TAPE_MINOR(dev_num, mode, rew));

	cdev = cdev_alloc();
	if (!cdev) {
		pr_err("st%d: out of memory. Device not attached.\n", dev_num);
		error = -ENOMEM;
		goto out;
	}
	cdev->owner = THIS_MODULE;
	cdev->ops = &st_fops;
	STm->cdevs[rew] = cdev;

	error = cdev_add(cdev, cdev_devno, 1);
	if (error) {
		pr_err("st%d: Can't add %s-rewind mode %d\n", dev_num,
		       rew ? "non" : "auto", mode);
		pr_err("st%d: Device not attached.\n", dev_num);
		goto out_free;
	}

	i = mode << (4 - ST_NBR_MODE_BITS);
	snprintf(name, 10, "%s%s%s", rew ? "n" : "",
		 tape->disk->disk_name, st_formats[i]);

	dev = device_create(&st_sysfs_class, &tape->device->sdev_gendev,
			    cdev_devno, &tape->modes[mode], "%s", name);
	if (IS_ERR(dev)) {
		pr_err("st%d: device_create failed\n", dev_num);
		error = PTR_ERR(dev);
		goto out_free;
	}

	STm->devs[rew] = dev;

	return 0;
out_free:
	cdev_del(STm->cdevs[rew]);
out:
	STm->cdevs[rew] = NULL;
	STm->devs[rew] = NULL;
	return error;
}

static int create_cdevs(struct scsi_tape *tape)
{
	int mode, error;
	for (mode = 0; mode < ST_NBR_MODES; ++mode) {
		error = create_one_cdev(tape, mode, 0);
		if (error)
			return error;
		error = create_one_cdev(tape, mode, 1);
		if (error)
			return error;
	}

	return sysfs_create_link(&tape->device->sdev_gendev.kobj,
				 &tape->modes[0].devs[0]->kobj, "tape");
}

static void remove_cdevs(struct scsi_tape *tape)
{
	int mode, rew;
	sysfs_remove_link(&tape->device->sdev_gendev.kobj, "tape");
	for (mode = 0; mode < ST_NBR_MODES; mode++) {
		struct st_modedef *STm = &(tape->modes[mode]);
		for (rew = 0; rew < 2; rew++) {
			if (STm->cdevs[rew])
				cdev_del(STm->cdevs[rew]);
			if (STm->devs[rew])
				device_unregister(STm->devs[rew]);
		}
	}
}

static int st_probe(struct device *dev)
{
	struct scsi_device *SDp = to_scsi_device(dev);
	struct gendisk *disk = NULL;
	struct scsi_tape *tpnt = NULL;
	struct st_modedef *STm;
	struct st_partstat *STps;
	struct st_buffer *buffer;
	int i, error;
	char *stp;

	if (SDp->type != TYPE_TAPE)
		return -ENODEV;
	if ((stp = st_incompatible(SDp))) {
		sdev_printk(KERN_INFO, SDp,
			    "OnStream tapes are no longer supported;\n");
		sdev_printk(KERN_INFO, SDp,
			    "please mail to linux-scsi@vger.kernel.org.\n");
		return -ENODEV;
	}

	scsi_autopm_get_device(SDp);
	i = queue_max_segments(SDp->request_queue);
	if (st_max_sg_segs < i)
		i = st_max_sg_segs;
	buffer = new_tape_buffer(i);
	if (buffer == NULL) {
		sdev_printk(KERN_ERR, SDp,
			    "st: Can't allocate new tape buffer. "
			    "Device not attached.\n");
		goto out;
	}

	disk = alloc_disk(1);
	if (!disk) {
		sdev_printk(KERN_ERR, SDp,
			    "st: out of memory. Device not attached.\n");
		goto out_buffer_free;
	}

	tpnt = kzalloc(sizeof(struct scsi_tape), GFP_KERNEL);
	if (tpnt == NULL) {
		sdev_printk(KERN_ERR, SDp,
			    "st: Can't allocate device descriptor.\n");
		goto out_put_disk;
	}
	kref_init(&tpnt->kref);
	tpnt->disk = disk;
	disk->private_data = &tpnt->driver;
	/* SCSI tape doesn't register this gendisk via add_disk().  Manually
	 * take queue reference that release_disk() expects. */
	if (!blk_get_queue(SDp->request_queue))
		goto out_put_disk;
	disk->queue = SDp->request_queue;
	tpnt->driver = &st_template;

	tpnt->device = SDp;
	if (SDp->scsi_level <= 2)
		tpnt->tape_type = MT_ISSCSI1;
	else
		tpnt->tape_type = MT_ISSCSI2;

	tpnt->buffer = buffer;
	tpnt->buffer->last_SRpnt = NULL;

	tpnt->inited = 0;
	tpnt->dirty = 0;
	tpnt->in_use = 0;
	tpnt->drv_buffer = 1;	/* Try buffering if no mode sense */
	tpnt->use_pf = (SDp->scsi_level >= SCSI_2);
	tpnt->density = 0;
	tpnt->do_auto_lock = ST_AUTO_LOCK;
	tpnt->can_bsr = (SDp->scsi_level > 2 ? 1 : ST_IN_FILE_POS); /* BSR mandatory in SCSI3 */
	tpnt->can_partitions = 0;
	tpnt->two_fm = ST_TWO_FM;
	tpnt->fast_mteom = ST_FAST_MTEOM;
	tpnt->scsi2_logical = ST_SCSI2LOGICAL;
	tpnt->sili = ST_SILI;
	tpnt->immediate = ST_NOWAIT;
	tpnt->immediate_filemark = 0;
	tpnt->default_drvbuffer = 0xff;		/* No forced buffering */
	tpnt->partition = 0;
	tpnt->new_partition = 0;
	tpnt->nbr_partitions = 0;
	blk_queue_rq_timeout(tpnt->device->request_queue, ST_TIMEOUT);
	tpnt->long_timeout = ST_LONG_TIMEOUT;
	tpnt->try_dio = try_direct_io;

	for (i = 0; i < ST_NBR_MODES; i++) {
		STm = &(tpnt->modes[i]);
		STm->defined = 0;
		STm->sysv = ST_SYSV;
		STm->defaults_for_writes = 0;
		STm->do_async_writes = ST_ASYNC_WRITES;
		STm->do_buffer_writes = ST_BUFFER_WRITES;
		STm->do_read_ahead = ST_READ_AHEAD;
		STm->default_compression = ST_DONT_TOUCH;
		STm->default_blksize = (-1);	/* No forced size */
		STm->default_density = (-1);	/* No forced density */
		STm->tape = tpnt;
	}

	for (i = 0; i < ST_NBR_PARTITIONS; i++) {
		STps = &(tpnt->ps[i]);
		STps->rw = ST_IDLE;
		STps->eof = ST_NOEOF;
		STps->at_sm = 0;
		STps->last_block_valid = 0;
		STps->drv_block = (-1);
		STps->drv_file = (-1);
	}

	tpnt->current_mode = 0;
	tpnt->modes[0].defined = 1;

	tpnt->density_changed = tpnt->compression_changed =
	    tpnt->blksize_changed = 0;
	mutex_init(&tpnt->lock);

	idr_preload(GFP_KERNEL);
	spin_lock(&st_index_lock);
	error = idr_alloc(&st_index_idr, tpnt, 0, ST_MAX_TAPES + 1, GFP_NOWAIT);
	spin_unlock(&st_index_lock);
	idr_preload_end();
	if (error < 0) {
		pr_warn("st: idr allocation failed: %d\n", error);
		goto out_put_queue;
	}
	tpnt->index = error;
	sprintf(disk->disk_name, "st%d", tpnt->index);
	tpnt->stats = kzalloc(sizeof(struct scsi_tape_stats), GFP_KERNEL);
	if (tpnt->stats == NULL) {
		sdev_printk(KERN_ERR, SDp,
			    "st: Can't allocate statistics.\n");
		goto out_idr_remove;
	}

	dev_set_drvdata(dev, tpnt);


	error = create_cdevs(tpnt);
	if (error)
		goto out_remove_devs;
	scsi_autopm_put_device(SDp);

	sdev_printk(KERN_NOTICE, SDp,
		    "Attached scsi tape %s\n", tape_name(tpnt));
	sdev_printk(KERN_INFO, SDp, "%s: try direct i/o: %s (alignment %d B)\n",
		    tape_name(tpnt), tpnt->try_dio ? "yes" : "no",
		    queue_dma_alignment(SDp->request_queue) + 1);

	return 0;

out_remove_devs:
	remove_cdevs(tpnt);
	kfree(tpnt->stats);
out_idr_remove:
	spin_lock(&st_index_lock);
	idr_remove(&st_index_idr, tpnt->index);
	spin_unlock(&st_index_lock);
out_put_queue:
	blk_put_queue(disk->queue);
out_put_disk:
	put_disk(disk);
	kfree(tpnt);
out_buffer_free:
	kfree(buffer);
out:
	scsi_autopm_put_device(SDp);
	return -ENODEV;
};


static int st_remove(struct device *dev)
{
	struct scsi_tape *tpnt = dev_get_drvdata(dev);
	int index = tpnt->index;

	scsi_autopm_get_device(to_scsi_device(dev));
	remove_cdevs(tpnt);

	mutex_lock(&st_ref_mutex);
	kref_put(&tpnt->kref, scsi_tape_release);
	mutex_unlock(&st_ref_mutex);
	spin_lock(&st_index_lock);
	idr_remove(&st_index_idr, index);
	spin_unlock(&st_index_lock);
	return 0;
}

/**
 *      scsi_tape_release - Called to free the Scsi_Tape structure
 *      @kref: pointer to embedded kref
 *
 *      st_ref_mutex must be held entering this routine.  Because it is
 *      called on last put, you should always use the scsi_tape_get()
 *      scsi_tape_put() helpers which manipulate the semaphore directly
 *      and never do a direct kref_put().
 **/
static void scsi_tape_release(struct kref *kref)
{
	struct scsi_tape *tpnt = to_scsi_tape(kref);
	struct gendisk *disk = tpnt->disk;

	tpnt->device = NULL;

	if (tpnt->buffer) {
		normalize_buffer(tpnt->buffer);
		kfree(tpnt->buffer->reserved_pages);
		kfree(tpnt->buffer);
	}

	disk->private_data = NULL;
	put_disk(disk);
	kfree(tpnt->stats);
	kfree(tpnt);
	return;
}

static struct class st_sysfs_class = {
	.name = "scsi_tape",
	.dev_groups = st_dev_groups,
};

static int __init init_st(void)
{
	int err;

	validate_options();

	printk(KERN_INFO "st: Version %s, fixed bufsize %d, s/g segs %d\n",
		verstr, st_fixed_buffer_size, st_max_sg_segs);

	debugging = (debug_flag > 0) ? debug_flag : NO_DEBUG;
	if (debugging) {
		printk(KERN_INFO "st: Debugging enabled debug_flag = %d\n",
			debugging);
	}

	err = class_register(&st_sysfs_class);
	if (err) {
		pr_err("Unable register sysfs class for SCSI tapes\n");
		return err;
	}

	err = register_chrdev_region(MKDEV(SCSI_TAPE_MAJOR, 0),
				     ST_MAX_TAPE_ENTRIES, "st");
	if (err) {
		printk(KERN_ERR "Unable to get major %d for SCSI tapes\n",
		       SCSI_TAPE_MAJOR);
		goto err_class;
	}

	err = scsi_register_driver(&st_template.gendrv);
	if (err)
		goto err_chrdev;

	return 0;

err_chrdev:
	unregister_chrdev_region(MKDEV(SCSI_TAPE_MAJOR, 0),
				 ST_MAX_TAPE_ENTRIES);
err_class:
	class_unregister(&st_sysfs_class);
	return err;
}

static void __exit exit_st(void)
{
	scsi_unregister_driver(&st_template.gendrv);
	unregister_chrdev_region(MKDEV(SCSI_TAPE_MAJOR, 0),
				 ST_MAX_TAPE_ENTRIES);
	class_unregister(&st_sysfs_class);
	idr_destroy(&st_index_idr);
	printk(KERN_INFO "st: Unloaded.\n");
}

module_init(init_st);
module_exit(exit_st);


/* The sysfs driver interface. Read-only at the moment */
static ssize_t try_direct_io_show(struct device_driver *ddp, char *buf)
{
	return scnprintf(buf, PAGE_SIZE, "%d\n", try_direct_io);
}
static DRIVER_ATTR_RO(try_direct_io);

static ssize_t fixed_buffer_size_show(struct device_driver *ddp, char *buf)
{
	return scnprintf(buf, PAGE_SIZE, "%d\n", st_fixed_buffer_size);
}
static DRIVER_ATTR_RO(fixed_buffer_size);

static ssize_t max_sg_segs_show(struct device_driver *ddp, char *buf)
{
	return scnprintf(buf, PAGE_SIZE, "%d\n", st_max_sg_segs);
}
static DRIVER_ATTR_RO(max_sg_segs);

static ssize_t version_show(struct device_driver *ddd, char *buf)
{
	return scnprintf(buf, PAGE_SIZE, "[%s]\n", verstr);
}
static DRIVER_ATTR_RO(version);

#if DEBUG
static ssize_t debug_flag_store(struct device_driver *ddp,
	const char *buf, size_t count)
{
/* We only care what the first byte of the data is the rest is unused.
 * if it's a '1' we turn on debug and if it's a '0' we disable it. All
 * other values have -EINVAL returned if they are passed in.
 */
	if (count > 0) {
		if (buf[0] == '0') {
			debugging = NO_DEBUG;
			return count;
		} else if (buf[0] == '1') {
			debugging = 1;
			return count;
		}
	}
	return -EINVAL;
}

static ssize_t debug_flag_show(struct device_driver *ddp, char *buf)
{
	return scnprintf(buf, PAGE_SIZE, "%d\n", debugging);
}
static DRIVER_ATTR_RW(debug_flag);
#endif

static struct attribute *st_drv_attrs[] = {
	&driver_attr_try_direct_io.attr,
	&driver_attr_fixed_buffer_size.attr,
	&driver_attr_max_sg_segs.attr,
	&driver_attr_version.attr,
#if DEBUG
	&driver_attr_debug_flag.attr,
#endif
	NULL,
};
ATTRIBUTE_GROUPS(st_drv);

/* The sysfs simple class interface */
static ssize_t
defined_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct st_modedef *STm = dev_get_drvdata(dev);
	ssize_t l = 0;

	l = snprintf(buf, PAGE_SIZE, "%d\n", STm->defined);
	return l;
}
static DEVICE_ATTR_RO(defined);

static ssize_t
default_blksize_show(struct device *dev, struct device_attribute *attr,
		     char *buf)
{
	struct st_modedef *STm = dev_get_drvdata(dev);
	ssize_t l = 0;

	l = snprintf(buf, PAGE_SIZE, "%d\n", STm->default_blksize);
	return l;
}
static DEVICE_ATTR_RO(default_blksize);

static ssize_t
default_density_show(struct device *dev, struct device_attribute *attr,
		     char *buf)
{
	struct st_modedef *STm = dev_get_drvdata(dev);
	ssize_t l = 0;
	char *fmt;

	fmt = STm->default_density >= 0 ? "0x%02x\n" : "%d\n";
	l = snprintf(buf, PAGE_SIZE, fmt, STm->default_density);
	return l;
}
static DEVICE_ATTR_RO(default_density);

static ssize_t
default_compression_show(struct device *dev, struct device_attribute *attr,
			 char *buf)
{
	struct st_modedef *STm = dev_get_drvdata(dev);
	ssize_t l = 0;

	l = snprintf(buf, PAGE_SIZE, "%d\n", STm->default_compression - 1);
	return l;
}
static DEVICE_ATTR_RO(default_compression);

static ssize_t
options_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct st_modedef *STm = dev_get_drvdata(dev);
	struct scsi_tape *STp = STm->tape;
	int options;
	ssize_t l = 0;

	options = STm->do_buffer_writes ? MT_ST_BUFFER_WRITES : 0;
	options |= STm->do_async_writes ? MT_ST_ASYNC_WRITES : 0;
	options |= STm->do_read_ahead ? MT_ST_READ_AHEAD : 0;
	DEB( options |= debugging ? MT_ST_DEBUGGING : 0 );
	options |= STp->two_fm ? MT_ST_TWO_FM : 0;
	options |= STp->fast_mteom ? MT_ST_FAST_MTEOM : 0;
	options |= STm->defaults_for_writes ? MT_ST_DEF_WRITES : 0;
	options |= STp->can_bsr ? MT_ST_CAN_BSR : 0;
	options |= STp->omit_blklims ? MT_ST_NO_BLKLIMS : 0;
	options |= STp->can_partitions ? MT_ST_CAN_PARTITIONS : 0;
	options |= STp->scsi2_logical ? MT_ST_SCSI2LOGICAL : 0;
	options |= STm->sysv ? MT_ST_SYSV : 0;
	options |= STp->immediate ? MT_ST_NOWAIT : 0;
	options |= STp->immediate_filemark ? MT_ST_NOWAIT_EOF : 0;
	options |= STp->sili ? MT_ST_SILI : 0;

	l = snprintf(buf, PAGE_SIZE, "0x%08x\n", options);
	return l;
}
static DEVICE_ATTR_RO(options);

/* Support for tape stats */

/**
 * read_cnt_show - return read count - count of reads made from tape drive
 * @dev: struct device
 * @attr: attribute structure
 * @buf: buffer to return formatted data in
 */
static ssize_t read_cnt_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct st_modedef *STm = dev_get_drvdata(dev);

	return sprintf(buf, "%lld",
		       (long long)atomic64_read(&STm->tape->stats->read_cnt));
}
static DEVICE_ATTR_RO(read_cnt);

/**
 * read_byte_cnt_show - return read byte count - tape drives
 * may use blocks less than 512 bytes this gives the raw byte count of
 * of data read from the tape drive.
 * @dev: struct device
 * @attr: attribute structure
 * @buf: buffer to return formatted data in
 */
static ssize_t read_byte_cnt_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct st_modedef *STm = dev_get_drvdata(dev);

	return sprintf(buf, "%lld",
		       (long long)atomic64_read(&STm->tape->stats->read_byte_cnt));
}
static DEVICE_ATTR_RO(read_byte_cnt);

/**
 * read_ns_show - return read ns - overall time spent waiting on reads in ns.
 * @dev: struct device
 * @attr: attribute structure
 * @buf: buffer to return formatted data in
 */
static ssize_t read_ns_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct st_modedef *STm = dev_get_drvdata(dev);

	return sprintf(buf, "%lld",
		       (long long)atomic64_read(&STm->tape->stats->tot_read_time));
}
static DEVICE_ATTR_RO(read_ns);

/**
 * write_cnt_show - write count - number of user calls
 * to write(2) that have written data to tape.
 * @dev: struct device
 * @attr: attribute structure
 * @buf: buffer to return formatted data in
 */
static ssize_t write_cnt_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct st_modedef *STm = dev_get_drvdata(dev);

	return sprintf(buf, "%lld",
		       (long long)atomic64_read(&STm->tape->stats->write_cnt));
}
static DEVICE_ATTR_RO(write_cnt);

/**
 * write_byte_cnt_show - write byte count - raw count of
 * bytes written to tape.
 * @dev: struct device
 * @attr: attribute structure
 * @buf: buffer to return formatted data in
 */
static ssize_t write_byte_cnt_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct st_modedef *STm = dev_get_drvdata(dev);

	return sprintf(buf, "%lld",
		       (long long)atomic64_read(&STm->tape->stats->write_byte_cnt));
}
static DEVICE_ATTR_RO(write_byte_cnt);

/**
 * write_ns_show - write ns - number of nanoseconds waiting on write
 * requests to complete.
 * @dev: struct device
 * @attr: attribute structure
 * @buf: buffer to return formatted data in
 */
static ssize_t write_ns_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct st_modedef *STm = dev_get_drvdata(dev);

	return sprintf(buf, "%lld",
		       (long long)atomic64_read(&STm->tape->stats->tot_write_time));
}
static DEVICE_ATTR_RO(write_ns);

/**
 * in_flight_show - number of I/Os currently in flight -
 * in most cases this will be either 0 or 1. It may be higher if someone
 * has also issued other SCSI commands such as via an ioctl.
 * @dev: struct device
 * @attr: attribute structure
 * @buf: buffer to return formatted data in
 */
static ssize_t in_flight_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct st_modedef *STm = dev_get_drvdata(dev);

	return sprintf(buf, "%lld",
		       (long long)atomic64_read(&STm->tape->stats->in_flight));
}
static DEVICE_ATTR_RO(in_flight);

/**
 * io_ns_show - io wait ns - this is the number of ns spent
 * waiting on all I/O to complete. This includes tape movement commands
 * such as rewinding, seeking to end of file or tape, it also includes
 * read and write. To determine the time spent on tape movement
 * subtract the read and write ns from this value.
 * @dev: struct device
 * @attr: attribute structure
 * @buf: buffer to return formatted data in
 */
static ssize_t io_ns_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct st_modedef *STm = dev_get_drvdata(dev);

	return sprintf(buf, "%lld",
		       (long long)atomic64_read(&STm->tape->stats->tot_io_time));
}
static DEVICE_ATTR_RO(io_ns);

/**
 * other_cnt_show - other io count - this is the number of
 * I/O requests other than read and write requests.
 * Typically these are tape movement requests but will include driver
 * tape movement. This includes only requests issued by the st driver.
 * @dev: struct device
 * @attr: attribute structure
 * @buf: buffer to return formatted data in
 */
static ssize_t other_cnt_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct st_modedef *STm = dev_get_drvdata(dev);

	return sprintf(buf, "%lld",
		       (long long)atomic64_read(&STm->tape->stats->other_cnt));
}
static DEVICE_ATTR_RO(other_cnt);

/**
 * resid_cnt_show - A count of the number of times we get a residual
 * count - this should indicate someone issuing reads larger than the
 * block size on tape.
 * @dev: struct device
 * @attr: attribute structure
 * @buf: buffer to return formatted data in
 */
static ssize_t resid_cnt_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct st_modedef *STm = dev_get_drvdata(dev);

	return sprintf(buf, "%lld",
		       (long long)atomic64_read(&STm->tape->stats->resid_cnt));
}
static DEVICE_ATTR_RO(resid_cnt);

static struct attribute *st_dev_attrs[] = {
	&dev_attr_defined.attr,
	&dev_attr_default_blksize.attr,
	&dev_attr_default_density.attr,
	&dev_attr_default_compression.attr,
	&dev_attr_options.attr,
	NULL,
};

static struct attribute *st_stats_attrs[] = {
	&dev_attr_read_cnt.attr,
	&dev_attr_read_byte_cnt.attr,
	&dev_attr_read_ns.attr,
	&dev_attr_write_cnt.attr,
	&dev_attr_write_byte_cnt.attr,
	&dev_attr_write_ns.attr,
	&dev_attr_in_flight.attr,
	&dev_attr_io_ns.attr,
	&dev_attr_other_cnt.attr,
	&dev_attr_resid_cnt.attr,
	NULL,
};

static struct attribute_group stats_group = {
	.name = "stats",
	.attrs = st_stats_attrs,
};

static struct attribute_group st_group = {
	.attrs = st_dev_attrs,
};

static const struct attribute_group *st_dev_groups[] = {
	&st_group,
	&stats_group,
	NULL,
};

/* The following functions may be useful for a larger audience. */
static int sgl_map_user_pages(struct st_buffer *STbp,
			      const unsigned int max_pages, unsigned long uaddr,
			      size_t count, int rw)
{
	unsigned long end = (uaddr + count + PAGE_SIZE - 1) >> PAGE_SHIFT;
	unsigned long start = uaddr >> PAGE_SHIFT;
	const int nr_pages = end - start;
	int res, i;
	struct page **pages;
	struct rq_map_data *mdata = &STbp->map_data;

	/* User attempted Overflow! */
	if ((uaddr + count) < uaddr)
		return -EINVAL;

	/* Too big */
        if (nr_pages > max_pages)
		return -ENOMEM;

	/* Hmm? */
	if (count == 0)
		return 0;

	pages = kmalloc_array(max_pages, sizeof(*pages), GFP_KERNEL);
	if (pages == NULL)
		return -ENOMEM;

        /* Try to fault in all of the necessary pages */
        /* rw==READ means read from drive, write into memory area */
	res = pin_user_pages_fast(uaddr, nr_pages, rw == READ ? FOLL_WRITE : 0,
				  pages);

	/* Errors and no page mapped should return here */
	if (res < nr_pages)
		goto out_unmap;

        for (i=0; i < nr_pages; i++) {
                /* FIXME: flush superflous for rw==READ,
                 * probably wrong function for rw==WRITE
                 */
		flush_dcache_page(pages[i]);
        }

	mdata->offset = uaddr & ~PAGE_MASK;
	STbp->mapped_pages = pages;

	return nr_pages;
 out_unmap:
	if (res > 0) {
		unpin_user_pages(pages, res);
		res = 0;
	}
	kfree(pages);
	return res;
}


/* And unmap them... */
static int sgl_unmap_user_pages(struct st_buffer *STbp,
				const unsigned int nr_pages, int dirtied)
{
	/* FIXME: cache flush missing for rw==READ */
	unpin_user_pages_dirty_lock(STbp->mapped_pages, nr_pages, dirtied);

	kfree(STbp->mapped_pages);
	STbp->mapped_pages = NULL;

	return 0;
}
