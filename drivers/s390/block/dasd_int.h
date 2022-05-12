/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Author(s)......: Holger Smolinski <Holger.Smolinski@de.ibm.com>
 *		    Horst Hummel <Horst.Hummel@de.ibm.com>
 *		    Martin Schwidefsky <schwidefsky@de.ibm.com>
 * Bugreports.to..: <Linux390@de.ibm.com>
 * Copyright IBM Corp. 1999, 2009
 */

#ifndef DASD_INT_H
#define DASD_INT_H

/* we keep old device allocation scheme; IOW, minors are still in 0..255 */
#define DASD_PER_MAJOR (1U << (MINORBITS - DASD_PARTN_BITS))
#define DASD_PARTN_MASK ((1 << DASD_PARTN_BITS) - 1)

/*
 * States a dasd device can have:
 *   new: the dasd_device structure is allocated.
 *   known: the discipline for the device is identified.
 *   basic: the device can do basic i/o.
 *   unfmt: the device could not be analyzed (format is unknown).
 *   ready: partition detection is done and the device is can do block io.
 *   online: the device accepts requests from the block device queue.
 *
 * Things to do for startup state transitions:
 *   new -> known: find discipline for the device and create devfs entries.
 *   known -> basic: request irq line for the device.
 *   basic -> ready: do the initial analysis, e.g. format detection,
 *                   do block device setup and detect partitions.
 *   ready -> online: schedule the device tasklet.
 * Things to do for shutdown state transitions:
 *   online -> ready: just set the new device state.
 *   ready -> basic: flush requests from the block device layer, clear
 *                   partition information and reset format information.
 *   basic -> known: terminate all requests and free irq.
 *   known -> new: remove devfs entries and forget discipline.
 */

#define DASD_STATE_NEW	  0
#define DASD_STATE_KNOWN  1
#define DASD_STATE_BASIC  2
#define DASD_STATE_UNFMT  3
#define DASD_STATE_READY  4
#define DASD_STATE_ONLINE 5

#include <linux/module.h>
#include <linux/wait.h>
#include <linux/blkdev.h>
#include <linux/genhd.h>
#include <linux/hdreg.h>
#include <linux/interrupt.h>
#include <linux/log2.h>
#include <asm/ccwdev.h>
#include <linux/workqueue.h>
#include <asm/debug.h>
#include <asm/dasd.h>
#include <asm/idals.h>
#include <linux/bitops.h>
#include <linux/blk-mq.h>

/* DASD discipline magic */
#define DASD_ECKD_MAGIC 0xC5C3D2C4
#define DASD_DIAG_MAGIC 0xC4C9C1C7
#define DASD_FBA_MAGIC 0xC6C2C140

/*
 * SECTION: Type definitions
 */
struct dasd_device;
struct dasd_block;

/* BIT DEFINITIONS FOR SENSE DATA */
#define DASD_SENSE_BIT_0 0x80
#define DASD_SENSE_BIT_1 0x40
#define DASD_SENSE_BIT_2 0x20
#define DASD_SENSE_BIT_3 0x10

/* BIT DEFINITIONS FOR SIM SENSE */
#define DASD_SIM_SENSE 0x0F
#define DASD_SIM_MSG_TO_OP 0x03
#define DASD_SIM_LOG 0x0C

/* lock class for nested cdev lock */
#define CDEV_NESTED_FIRST 1
#define CDEV_NESTED_SECOND 2

/*
 * SECTION: MACROs for klogd and s390 debug feature (dbf)
 */
#define DBF_DEV_EVENT(d_level, d_device, d_str, d_data...) \
do { \
	debug_sprintf_event(d_device->debug_area, \
			    d_level, \
			    d_str "\n", \
			    d_data); \
} while(0)

#define DBF_EVENT(d_level, d_str, d_data...)\
do { \
	debug_sprintf_event(dasd_debug_area, \
			    d_level,\
			    d_str "\n", \
			    d_data); \
} while(0)

#define DBF_EVENT_DEVID(d_level, d_cdev, d_str, d_data...)	\
do { \
	struct ccw_dev_id __dev_id;			\
	ccw_device_get_id(d_cdev, &__dev_id);		\
	debug_sprintf_event(dasd_debug_area,		\
			    d_level,					\
			    "0.%x.%04x " d_str "\n",			\
			    __dev_id.ssid, __dev_id.devno, d_data);	\
} while (0)

/* limit size for an errorstring */
#define ERRORLENGTH 30

/* definition of dbf debug levels */
#define	DBF_EMERG	0	/* system is unusable			*/
#define	DBF_ALERT	1	/* action must be taken immediately	*/
#define	DBF_CRIT	2	/* critical conditions			*/
#define	DBF_ERR		3	/* error conditions			*/
#define	DBF_WARNING	4	/* warning conditions			*/
#define	DBF_NOTICE	5	/* normal but significant condition	*/
#define	DBF_INFO	6	/* informational			*/
#define	DBF_DEBUG	6	/* debug-level messages			*/

/* messages to be written via klogd and dbf */
#define DEV_MESSAGE(d_loglevel,d_device,d_string,d_args...)\
do { \
	printk(d_loglevel PRINTK_HEADER " %s: " d_string "\n", \
	       dev_name(&d_device->cdev->dev), d_args); \
	DBF_DEV_EVENT(DBF_ALERT, d_device, d_string, d_args); \
} while(0)

#define MESSAGE(d_loglevel,d_string,d_args...)\
do { \
	printk(d_loglevel PRINTK_HEADER " " d_string "\n", d_args); \
	DBF_EVENT(DBF_ALERT, d_string, d_args); \
} while(0)

/* messages to be written via klogd only */
#define DEV_MESSAGE_LOG(d_loglevel,d_device,d_string,d_args...)\
do { \
	printk(d_loglevel PRINTK_HEADER " %s: " d_string "\n", \
	       dev_name(&d_device->cdev->dev), d_args); \
} while(0)

#define MESSAGE_LOG(d_loglevel,d_string,d_args...)\
do { \
	printk(d_loglevel PRINTK_HEADER " " d_string "\n", d_args); \
} while(0)

/* Macro to calculate number of blocks per page */
#define BLOCKS_PER_PAGE(blksize) (PAGE_SIZE / blksize)

struct dasd_ccw_req {
	unsigned int magic;		/* Eye catcher */
	int intrc;			/* internal error, e.g. from start_IO */
	struct list_head devlist;	/* for dasd_device request queue */
	struct list_head blocklist;	/* for dasd_block request queue */
	struct dasd_block *block;	/* the originating block device */
	struct dasd_device *memdev;	/* the device used to allocate this */
	struct dasd_device *startdev;	/* device the request is started on */
	struct dasd_device *basedev;	/* base device if no block->base */
	void *cpaddr;			/* address of ccw or tcw */
	short retries;			/* A retry counter */
	unsigned char cpmode;		/* 0 = cmd mode, 1 = itcw */
	char status;			/* status of this request */
	char lpm;			/* logical path mask */
	unsigned long flags;        	/* flags of this request */
	struct dasd_queue *dq;
	unsigned long starttime;	/* jiffies time of request start */
	unsigned long expires;		/* expiration period in jiffies */
	void *data;			/* pointer to data area */
	struct irb irb;			/* device status in case of an error */
	struct dasd_ccw_req *refers;	/* ERP-chain queueing. */
	void *function; 		/* originating ERP action */
	void *mem_chunk;

	unsigned long buildclk;		/* TOD-clock of request generation */
	unsigned long startclk;		/* TOD-clock of request start */
	unsigned long stopclk;		/* TOD-clock of request interrupt */
	unsigned long endclk;		/* TOD-clock of request termination */

	void (*callback)(struct dasd_ccw_req *, void *data);
	void *callback_data;
	unsigned int proc_bytes;	/* bytes for partial completion */
};

/*
 * dasd_ccw_req -> status can be:
 */
#define DASD_CQR_FILLED 	0x00	/* request is ready to be processed */
#define DASD_CQR_DONE		0x01	/* request is completed successfully */
#define DASD_CQR_NEED_ERP	0x02	/* request needs recovery action */
#define DASD_CQR_IN_ERP 	0x03	/* request is in recovery */
#define DASD_CQR_FAILED 	0x04	/* request is finally failed */
#define DASD_CQR_TERMINATED	0x05	/* request was stopped by driver */

#define DASD_CQR_QUEUED 	0x80	/* request is queued to be processed */
#define DASD_CQR_IN_IO		0x81	/* request is currently in IO */
#define DASD_CQR_ERROR		0x82	/* request is completed with error */
#define DASD_CQR_CLEAR_PENDING	0x83	/* request is clear pending */
#define DASD_CQR_CLEARED	0x84	/* request was cleared */
#define DASD_CQR_SUCCESS	0x85	/* request was successful */

/* default expiration time*/
#define DASD_EXPIRES	  300
#define DASD_EXPIRES_MAX  40000000
#define DASD_RETRIES	  256
#define DASD_RETRIES_MAX  32768

/* per dasd_ccw_req flags */
#define DASD_CQR_FLAGS_USE_ERP   0	/* use ERP for this request */
#define DASD_CQR_FLAGS_FAILFAST  1	/* FAILFAST */
#define DASD_CQR_VERIFY_PATH	 2	/* path verification request */
#define DASD_CQR_ALLOW_SLOCK	 3	/* Try this request even when lock was
					 * stolen. Should not be combined with
					 * DASD_CQR_FLAGS_USE_ERP
					 */
/*
 * The following flags are used to suppress output of certain errors.
 */
#define DASD_CQR_SUPPRESS_NRF	4	/* Suppress 'No Record Found' error */
#define DASD_CQR_SUPPRESS_FP	5	/* Suppress 'File Protected' error*/
#define DASD_CQR_SUPPRESS_IL	6	/* Suppress 'Incorrect Length' error */
#define DASD_CQR_SUPPRESS_CR	7	/* Suppress 'Command Reject' error */

#define DASD_REQ_PER_DEV 4

/* Signature for error recovery functions. */
typedef struct dasd_ccw_req *(*dasd_erp_fn_t) (struct dasd_ccw_req *);

/*
 * A single CQR can only contain a maximum of 255 CCWs. It is limited by
 * the locate record and locate record extended count value which can only hold
 * 1 Byte max.
 */
#define DASD_CQR_MAX_CCW 255

/*
 * Unique identifier for dasd device.
 */
#define UA_NOT_CONFIGURED  0x00
#define UA_BASE_DEVICE	   0x01
#define UA_BASE_PAV_ALIAS  0x02
#define UA_HYPER_PAV_ALIAS 0x03

struct dasd_uid {
	__u8 type;
	char vendor[4];
	char serial[15];
	__u16 ssid;
	__u8 real_unit_addr;
	__u8 base_unit_addr;
	char vduit[33];
};

/*
 * the struct dasd_discipline is
 * sth like a table of virtual functions, if you think of dasd_eckd
 * inheriting dasd...
 * no, currently we are not planning to reimplement the driver in C++
 */
struct dasd_discipline {
	struct module *owner;
	char ebcname[8];	/* a name used for tagging and printks */
	char name[8];		/* a name used for tagging and printks */

	struct list_head list;	/* used for list of disciplines */

	/*
	 * Device recognition functions. check_device is used to verify
	 * the sense data and the information returned by read device
	 * characteristics. It returns 0 if the discipline can be used
	 * for the device in question. uncheck_device is called during
	 * device shutdown to deregister a device from its discipline.
	 */
	int (*check_device) (struct dasd_device *);
	void (*uncheck_device) (struct dasd_device *);

	/*
	 * do_analysis is used in the step from device state "basic" to
	 * state "accept". It returns 0 if the device can be made ready,
	 * it returns -EMEDIUMTYPE if the device can't be made ready or
	 * -EAGAIN if do_analysis started a ccw that needs to complete
	 * before the analysis may be repeated.
	 */
	int (*do_analysis) (struct dasd_block *);

	/*
	 * This function is called, when new paths become available.
	 * Disciplins may use this callback to do necessary setup work,
	 * e.g. verify that new path is compatible with the current
	 * configuration.
	 */
	int (*pe_handler)(struct dasd_device *, __u8, __u8);

	/*
	 * Last things to do when a device is set online, and first things
	 * when it is set offline.
	 */
	int (*basic_to_ready) (struct dasd_device *);
	int (*online_to_ready) (struct dasd_device *);
	int (*basic_to_known)(struct dasd_device *);

	/*
	 * Initialize block layer request queue.
	 */
	void (*setup_blk_queue)(struct dasd_block *);
	/* (struct dasd_device *);
	 * Device operation functions. build_cp creates a ccw chain for
	 * a block device request, start_io starts the request and
	 * term_IO cancels it (e.g. in case of a timeout). format_device
	 * formats the device and check_device_format compares the format of
	 * a device with the expected format_data.
	 * handle_terminated_request allows to examine a cqr and prepare
	 * it for retry.
	 */
	struct dasd_ccw_req *(*build_cp) (struct dasd_device *,
					  struct dasd_block *,
					  struct request *);
	int (*start_IO) (struct dasd_ccw_req *);
	int (*term_IO) (struct dasd_ccw_req *);
	void (*handle_terminated_request) (struct dasd_ccw_req *);
	int (*format_device) (struct dasd_device *,
			      struct format_data_t *, int);
	int (*check_device_format)(struct dasd_device *,
				   struct format_check_t *, int);
	int (*free_cp) (struct dasd_ccw_req *, struct request *);

	/*
	 * Error recovery functions. examine_error() returns a value that
	 * indicates what to do for an error condition. If examine_error()
	 * returns 'dasd_era_recover' erp_action() is called to create a
	 * special error recovery ccw. erp_postaction() is called after
	 * an error recovery ccw has finished its execution. dump_sense
	 * is called for every error condition to print the sense data
	 * to the console.
	 */
	dasd_erp_fn_t(*erp_action) (struct dasd_ccw_req *);
	dasd_erp_fn_t(*erp_postaction) (struct dasd_ccw_req *);
	void (*dump_sense) (struct dasd_device *, struct dasd_ccw_req *,
			    struct irb *);
	void (*dump_sense_dbf) (struct dasd_device *, struct irb *, char *);
	void (*check_for_device_change) (struct dasd_device *,
					 struct dasd_ccw_req *,
					 struct irb *);

        /* i/o control functions. */
	int (*fill_geometry) (struct dasd_block *, struct hd_geometry *);
	int (*fill_info) (struct dasd_device *, struct dasd_information2_t *);
	int (*ioctl) (struct dasd_block *, unsigned int, void __user *);

	/* reload device after state change */
	int (*reload) (struct dasd_device *);

	int (*get_uid) (struct dasd_device *, struct dasd_uid *);
	void (*kick_validate) (struct dasd_device *);
	int (*check_attention)(struct dasd_device *, __u8);
	int (*host_access_count)(struct dasd_device *);
	int (*hosts_print)(struct dasd_device *, struct seq_file *);
	void (*handle_hpf_error)(struct dasd_device *, struct irb *);
	void (*disable_hpf)(struct dasd_device *);
	int (*hpf_enabled)(struct dasd_device *);
	void (*reset_path)(struct dasd_device *, __u8);

	/*
	 * Extent Space Efficient (ESE) relevant functions
	 */
	int (*is_ese)(struct dasd_device *);
	/* Capacity */
	int (*space_allocated)(struct dasd_device *);
	int (*space_configured)(struct dasd_device *);
	int (*logical_capacity)(struct dasd_device *);
	int (*release_space)(struct dasd_device *, struct format_data_t *);
	/* Extent Pool */
	int (*ext_pool_id)(struct dasd_device *);
	int (*ext_size)(struct dasd_device *);
	int (*ext_pool_cap_at_warnlevel)(struct dasd_device *);
	int (*ext_pool_warn_thrshld)(struct dasd_device *);
	int (*ext_pool_oos)(struct dasd_device *);
	int (*ext_pool_exhaust)(struct dasd_device *, struct dasd_ccw_req *);
	struct dasd_ccw_req *(*ese_format)(struct dasd_device *,
					   struct dasd_ccw_req *, struct irb *);
	int (*ese_read)(struct dasd_ccw_req *, struct irb *);
};

extern struct dasd_discipline *dasd_diag_discipline_pointer;

/*
 * Notification numbers for extended error reporting notifications:
 * The DASD_EER_DISABLE notification is sent before a dasd_device (and it's
 * eer pointer) is freed. The error reporting module needs to do all necessary
 * cleanup steps.
 * The DASD_EER_TRIGGER notification sends the actual error reports (triggers).
 */
#define DASD_EER_DISABLE 0
#define DASD_EER_TRIGGER 1

/* Trigger IDs for extended error reporting DASD_EER_TRIGGER notification */
#define DASD_EER_FATALERROR  1
#define DASD_EER_NOPATH      2
#define DASD_EER_STATECHANGE 3
#define DASD_EER_PPRCSUSPEND 4
#define DASD_EER_NOSPC	     5

/* DASD path handling */

#define DASD_PATH_OPERATIONAL  1
#define DASD_PATH_TBV	       2
#define DASD_PATH_PP	       3
#define DASD_PATH_NPP	       4
#define DASD_PATH_MISCABLED    5
#define DASD_PATH_NOHPF        6
#define DASD_PATH_CUIR	       7
#define DASD_PATH_IFCC	       8
#define DASD_PATH_FCSEC	       9

#define DASD_THRHLD_MAX		4294967295U
#define DASD_INTERVAL_MAX	4294967295U

/* FC Endpoint Security Capabilities */
#define DASD_FC_SECURITY_UNSUP		0
#define DASD_FC_SECURITY_AUTH		1
#define DASD_FC_SECURITY_ENC_FCSP2	2
#define DASD_FC_SECURITY_ENC_ERAS	3

#define DASD_FC_SECURITY_ENC_STR	"Encryption"
static const struct {
	u8 value;
	char *name;
} dasd_path_fcs_mnemonics[] = {
	{ DASD_FC_SECURITY_UNSUP,	"Unsupported" },
	{ DASD_FC_SECURITY_AUTH,	"Authentication" },
	{ DASD_FC_SECURITY_ENC_FCSP2,	DASD_FC_SECURITY_ENC_STR },
	{ DASD_FC_SECURITY_ENC_ERAS,	DASD_FC_SECURITY_ENC_STR },
};

static inline char *dasd_path_get_fcs_str(int val)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(dasd_path_fcs_mnemonics); i++) {
		if (dasd_path_fcs_mnemonics[i].value == val)
			return dasd_path_fcs_mnemonics[i].name;
	}

	return dasd_path_fcs_mnemonics[0].name;
}

struct dasd_path {
	unsigned long flags;
	u8 cssid;
	u8 ssid;
	u8 chpid;
	struct dasd_conf_data *conf_data;
	atomic_t error_count;
	unsigned long errorclk;
	u8 fc_security;
	struct kobject kobj;
	bool in_sysfs;
};

#define to_dasd_path(path) container_of(path, struct dasd_path, kobj)

static inline void dasd_path_release(struct kobject *kobj)
{
/* Memory for the dasd_path kobject is freed when dasd_free_device() is called */
}


struct dasd_profile_info {
	/* legacy part of profile data, as in dasd_profile_info_t */
	unsigned int dasd_io_reqs;	 /* number of requests processed */
	unsigned int dasd_io_sects;	 /* number of sectors processed */
	unsigned int dasd_io_secs[32];	 /* histogram of request's sizes */
	unsigned int dasd_io_times[32];	 /* histogram of requests's times */
	unsigned int dasd_io_timps[32];	 /* h. of requests's times per sector */
	unsigned int dasd_io_time1[32];	 /* hist. of time from build to start */
	unsigned int dasd_io_time2[32];	 /* hist. of time from start to irq */
	unsigned int dasd_io_time2ps[32]; /* hist. of time from start to irq */
	unsigned int dasd_io_time3[32];	 /* hist. of time from irq to end */
	unsigned int dasd_io_nr_req[32]; /* hist. of # of requests in chanq */

	/* new data */
	struct timespec64 starttod;	   /* time of start or last reset */
	unsigned int dasd_io_alias;	   /* requests using an alias */
	unsigned int dasd_io_tpm;	   /* requests using transport mode */
	unsigned int dasd_read_reqs;	   /* total number of read  requests */
	unsigned int dasd_read_sects;	   /* total number read sectors */
	unsigned int dasd_read_alias;	   /* read request using an alias */
	unsigned int dasd_read_tpm;	   /* read requests in transport mode */
	unsigned int dasd_read_secs[32];   /* histogram of request's sizes */
	unsigned int dasd_read_times[32];  /* histogram of requests's times */
	unsigned int dasd_read_time1[32];  /* hist. time from build to start */
	unsigned int dasd_read_time2[32];  /* hist. of time from start to irq */
	unsigned int dasd_read_time3[32];  /* hist. of time from irq to end */
	unsigned int dasd_read_nr_req[32]; /* hist. of # of requests in chanq */
	unsigned long dasd_sum_times;	   /* sum of request times */
	unsigned long dasd_sum_time_str;   /* sum of time from build to start */
	unsigned long dasd_sum_time_irq;   /* sum of time from start to irq */
	unsigned long dasd_sum_time_end;   /* sum of time from irq to end */
};

struct dasd_profile {
	struct dentry *dentry;
	struct dasd_profile_info *data;
	spinlock_t lock;
};

struct dasd_format_entry {
	struct list_head list;
	sector_t track;
};

struct dasd_device {
	/* Block device stuff. */
	struct dasd_block *block;

        unsigned int devindex;
	unsigned long flags;	   /* per device flags */
	unsigned short features;   /* copy of devmap-features (read-only!) */

	/* extended error reporting stuff (eer) */
	struct dasd_ccw_req *eer_cqr;

	/* Device discipline stuff. */
	struct dasd_discipline *discipline;
	struct dasd_discipline *base_discipline;
	void *private;
	struct dasd_path path[8];
	__u8 opm;

	/* Device state and target state. */
	int state, target;
	struct mutex state_mutex;
	int stopped;		/* device (ccw_device_start) was stopped */

	/* reference count. */
        atomic_t ref_count;

	/* ccw queue and memory for static ccw/erp buffers. */
	struct list_head ccw_queue;
	spinlock_t mem_lock;
	void *ccw_mem;
	void *erp_mem;
	void *ese_mem;
	struct list_head ccw_chunks;
	struct list_head erp_chunks;
	struct list_head ese_chunks;

	atomic_t tasklet_scheduled;
        struct tasklet_struct tasklet;
	struct work_struct kick_work;
	struct work_struct reload_device;
	struct work_struct kick_validate;
	struct work_struct suc_work;
	struct work_struct requeue_requests;
	struct timer_list timer;

	debug_info_t *debug_area;

	struct ccw_device *cdev;

	/* hook for alias management */
	struct list_head alias_list;

	/* default expiration time in s */
	unsigned long default_expires;
	unsigned long default_retries;

	unsigned long blk_timeout;

	unsigned long path_thrhld;
	unsigned long path_interval;

	struct dentry *debugfs_dentry;
	struct dentry *hosts_dentry;
	struct dasd_profile profile;
	struct dasd_format_entry format_entry;
	struct kset *paths_info;
};

struct dasd_block {
	/* Block device stuff. */
	struct gendisk *gdp;
	struct request_queue *request_queue;
	spinlock_t request_queue_lock;
	struct blk_mq_tag_set tag_set;
	struct block_device *bdev;
	atomic_t open_count;

	unsigned long blocks;	   /* size of volume in blocks */
	unsigned int bp_block;	   /* bytes per block */
	unsigned int s2b_shift;	   /* log2 (bp_block/512) */

	struct dasd_device *base;
	struct list_head ccw_queue;
	spinlock_t queue_lock;

	atomic_t tasklet_scheduled;
	struct tasklet_struct tasklet;
	struct timer_list timer;

	struct dentry *debugfs_dentry;
	struct dasd_profile profile;

	struct list_head format_list;
	spinlock_t format_lock;
};

struct dasd_attention_data {
	struct dasd_device *device;
	__u8 lpum;
};

struct dasd_queue {
	spinlock_t lock;
};

/* reasons why device (ccw_device_start) was stopped */
#define DASD_STOPPED_NOT_ACC 1         /* not accessible */
#define DASD_STOPPED_QUIESCE 2         /* Quiesced */
#define DASD_STOPPED_PENDING 4         /* long busy */
#define DASD_STOPPED_DC_WAIT 8         /* disconnected, wait */
#define DASD_STOPPED_SU      16        /* summary unit check handling */
#define DASD_STOPPED_NOSPC   128       /* no space left */

/* per device flags */
#define DASD_FLAG_OFFLINE	3	/* device is in offline processing */
#define DASD_FLAG_EER_SNSS	4	/* A SNSS is required */
#define DASD_FLAG_EER_IN_USE	5	/* A SNSS request is running */
#define DASD_FLAG_DEVICE_RO	6	/* The device itself is read-only. Don't
					 * confuse this with the user specified
					 * read-only feature.
					 */
#define DASD_FLAG_IS_RESERVED	7	/* The device is reserved */
#define DASD_FLAG_LOCK_STOLEN	8	/* The device lock was stolen */
#define DASD_FLAG_SUSPENDED	9	/* The device was suspended */
#define DASD_FLAG_SAFE_OFFLINE	10	/* safe offline processing requested*/
#define DASD_FLAG_SAFE_OFFLINE_RUNNING	11	/* safe offline running */
#define DASD_FLAG_ABORTALL	12	/* Abort all noretry requests */
#define DASD_FLAG_PATH_VERIFY	13	/* Path verification worker running */
#define DASD_FLAG_SUC		14	/* unhandled summary unit check */

#define DASD_SLEEPON_START_TAG	((void *) 1)
#define DASD_SLEEPON_END_TAG	((void *) 2)

void dasd_put_device_wake(struct dasd_device *);

/*
 * Reference count inliners
 */
static inline void
dasd_get_device(struct dasd_device *device)
{
	atomic_inc(&device->ref_count);
}

static inline void
dasd_put_device(struct dasd_device *device)
{
	if (atomic_dec_return(&device->ref_count) == 0)
		dasd_put_device_wake(device);
}

/*
 * The static memory in ccw_mem and erp_mem is managed by a sorted
 * list of free memory chunks.
 */
struct dasd_mchunk
{
	struct list_head list;
	unsigned long size;
} __attribute__ ((aligned(8)));

static inline void
dasd_init_chunklist(struct list_head *chunk_list, void *mem,
		    unsigned long size)
{
	struct dasd_mchunk *chunk;

	INIT_LIST_HEAD(chunk_list);
	chunk = (struct dasd_mchunk *) mem;
	chunk->size = size - sizeof(struct dasd_mchunk);
	list_add(&chunk->list, chunk_list);
}

static inline void *
dasd_alloc_chunk(struct list_head *chunk_list, unsigned long size)
{
	struct dasd_mchunk *chunk, *tmp;

	size = (size + 7L) & -8L;
	list_for_each_entry(chunk, chunk_list, list) {
		if (chunk->size < size)
			continue;
		if (chunk->size > size + sizeof(struct dasd_mchunk)) {
			char *endaddr = (char *) (chunk + 1) + chunk->size;
			tmp = (struct dasd_mchunk *) (endaddr - size) - 1;
			tmp->size = size;
			chunk->size -= size + sizeof(struct dasd_mchunk);
			chunk = tmp;
		} else
			list_del(&chunk->list);
		return (void *) (chunk + 1);
	}
	return NULL;
}

static inline void
dasd_free_chunk(struct list_head *chunk_list, void *mem)
{
	struct dasd_mchunk *chunk, *tmp;
	struct list_head *p, *left;

	chunk = (struct dasd_mchunk *)
		((char *) mem - sizeof(struct dasd_mchunk));
	/* Find out the left neighbour in chunk_list. */
	left = chunk_list;
	list_for_each(p, chunk_list) {
		if (list_entry(p, struct dasd_mchunk, list) > chunk)
			break;
		left = p;
	}
	/* Try to merge with right neighbour = next element from left. */
	if (left->next != chunk_list) {
		tmp = list_entry(left->next, struct dasd_mchunk, list);
		if ((char *) (chunk + 1) + chunk->size == (char *) tmp) {
			list_del(&tmp->list);
			chunk->size += tmp->size + sizeof(struct dasd_mchunk);
		}
	}
	/* Try to merge with left neighbour. */
	if (left != chunk_list) {
		tmp = list_entry(left, struct dasd_mchunk, list);
		if ((char *) (tmp + 1) + tmp->size == (char *) chunk) {
			tmp->size += chunk->size + sizeof(struct dasd_mchunk);
			return;
		}
	}
	__list_add(&chunk->list, left, left->next);
}

/*
 * Check if bsize is in { 512, 1024, 2048, 4096 }
 */
static inline int
dasd_check_blocksize(int bsize)
{
	if (bsize < 512 || bsize > 4096 || !is_power_of_2(bsize))
		return -EMEDIUMTYPE;
	return 0;
}

/* externals in dasd.c */
#define DASD_PROFILE_OFF	 0
#define DASD_PROFILE_ON 	 1
#define DASD_PROFILE_GLOBAL_ONLY 2

extern debug_info_t *dasd_debug_area;
extern struct dasd_profile dasd_global_profile;
extern unsigned int dasd_global_profile_level;
extern const struct block_device_operations dasd_device_operations;

extern struct kmem_cache *dasd_page_cache;

struct dasd_ccw_req *
dasd_smalloc_request(int, int, int, struct dasd_device *, struct dasd_ccw_req *);
struct dasd_ccw_req *dasd_fmalloc_request(int, int, int, struct dasd_device *);
void dasd_sfree_request(struct dasd_ccw_req *, struct dasd_device *);
void dasd_ffree_request(struct dasd_ccw_req *, struct dasd_device *);
void dasd_wakeup_cb(struct dasd_ccw_req *, void *);

struct dasd_device *dasd_alloc_device(void);
void dasd_free_device(struct dasd_device *);

struct dasd_block *dasd_alloc_block(void);
void dasd_free_block(struct dasd_block *);

enum blk_eh_timer_return dasd_times_out(struct request *req, bool reserved);

void dasd_enable_device(struct dasd_device *);
void dasd_set_target_state(struct dasd_device *, int);
void dasd_kick_device(struct dasd_device *);
void dasd_reload_device(struct dasd_device *);
void dasd_schedule_requeue(struct dasd_device *);

void dasd_add_request_head(struct dasd_ccw_req *);
void dasd_add_request_tail(struct dasd_ccw_req *);
int  dasd_start_IO(struct dasd_ccw_req *);
int  dasd_term_IO(struct dasd_ccw_req *);
void dasd_schedule_device_bh(struct dasd_device *);
void dasd_schedule_block_bh(struct dasd_block *);
int  dasd_sleep_on(struct dasd_ccw_req *);
int  dasd_sleep_on_queue(struct list_head *);
int  dasd_sleep_on_immediatly(struct dasd_ccw_req *);
int  dasd_sleep_on_queue_interruptible(struct list_head *);
int  dasd_sleep_on_interruptible(struct dasd_ccw_req *);
void dasd_device_set_timer(struct dasd_device *, int);
void dasd_device_clear_timer(struct dasd_device *);
void dasd_block_set_timer(struct dasd_block *, int);
void dasd_block_clear_timer(struct dasd_block *);
int  dasd_cancel_req(struct dasd_ccw_req *);
int dasd_flush_device_queue(struct dasd_device *);
int dasd_generic_probe(struct ccw_device *);
void dasd_generic_free_discipline(struct dasd_device *);
void dasd_generic_remove (struct ccw_device *cdev);
int dasd_generic_set_online(struct ccw_device *, struct dasd_discipline *);
int dasd_generic_set_offline (struct ccw_device *cdev);
int dasd_generic_notify(struct ccw_device *, int);
int dasd_generic_last_path_gone(struct dasd_device *);
int dasd_generic_path_operational(struct dasd_device *);
void dasd_generic_shutdown(struct ccw_device *);

void dasd_generic_handle_state_change(struct dasd_device *);
enum uc_todo dasd_generic_uc_handler(struct ccw_device *, struct irb *);
void dasd_generic_path_event(struct ccw_device *, int *);
int dasd_generic_verify_path(struct dasd_device *, __u8);
void dasd_generic_space_exhaust(struct dasd_device *, struct dasd_ccw_req *);
void dasd_generic_space_avail(struct dasd_device *);

int dasd_generic_read_dev_chars(struct dasd_device *, int, void *, int);
char *dasd_get_sense(struct irb *);

void dasd_device_set_stop_bits(struct dasd_device *, int);
void dasd_device_remove_stop_bits(struct dasd_device *, int);

int dasd_device_is_ro(struct dasd_device *);

void dasd_profile_reset(struct dasd_profile *);
int dasd_profile_on(struct dasd_profile *);
void dasd_profile_off(struct dasd_profile *);
char *dasd_get_user_string(const char __user *, size_t);

/* externals in dasd_devmap.c */
extern int dasd_max_devindex;
extern int dasd_probeonly;
extern int dasd_autodetect;
extern int dasd_nopav;
extern int dasd_nofcx;

int dasd_devmap_init(void);
void dasd_devmap_exit(void);

struct dasd_device *dasd_create_device(struct ccw_device *);
void dasd_delete_device(struct dasd_device *);

int dasd_get_feature(struct ccw_device *, int);
int dasd_set_feature(struct ccw_device *, int, int);

extern const struct attribute_group *dasd_dev_groups[];
void dasd_path_create_kobj(struct dasd_device *, int);
void dasd_path_create_kobjects(struct dasd_device *);
void dasd_path_remove_kobjects(struct dasd_device *);

struct dasd_device *dasd_device_from_cdev(struct ccw_device *);
struct dasd_device *dasd_device_from_cdev_locked(struct ccw_device *);
struct dasd_device *dasd_device_from_devindex(int);

void dasd_add_link_to_gendisk(struct gendisk *, struct dasd_device *);
struct dasd_device *dasd_device_from_gendisk(struct gendisk *);

int dasd_parse(void) __init;
int dasd_busid_known(const char *);

/* externals in dasd_gendisk.c */
int  dasd_gendisk_init(void);
void dasd_gendisk_exit(void);
int dasd_gendisk_alloc(struct dasd_block *);
void dasd_gendisk_free(struct dasd_block *);
int dasd_scan_partitions(struct dasd_block *);
void dasd_destroy_partitions(struct dasd_block *);

/* externals in dasd_ioctl.c */
int dasd_ioctl(struct block_device *, fmode_t, unsigned int, unsigned long);
int dasd_set_read_only(struct block_device *bdev, bool ro);

/* externals in dasd_proc.c */
int dasd_proc_init(void);
void dasd_proc_exit(void);

/* externals in dasd_erp.c */
struct dasd_ccw_req *dasd_default_erp_action(struct dasd_ccw_req *);
struct dasd_ccw_req *dasd_default_erp_postaction(struct dasd_ccw_req *);
struct dasd_ccw_req *dasd_alloc_erp_request(char *, int, int,
					    struct dasd_device *);
void dasd_free_erp_request(struct dasd_ccw_req *, struct dasd_device *);
void dasd_log_sense(struct dasd_ccw_req *, struct irb *);
void dasd_log_sense_dbf(struct dasd_ccw_req *cqr, struct irb *irb);

/* externals in dasd_3990_erp.c */
struct dasd_ccw_req *dasd_3990_erp_action(struct dasd_ccw_req *);
void dasd_3990_erp_handle_sim(struct dasd_device *, char *);

/* externals in dasd_eer.c */
#ifdef CONFIG_DASD_EER
int dasd_eer_init(void);
void dasd_eer_exit(void);
int dasd_eer_enable(struct dasd_device *);
void dasd_eer_disable(struct dasd_device *);
void dasd_eer_write(struct dasd_device *, struct dasd_ccw_req *cqr,
		    unsigned int id);
void dasd_eer_snss(struct dasd_device *);

static inline int dasd_eer_enabled(struct dasd_device *device)
{
	return device->eer_cqr != NULL;
}
#else
#define dasd_eer_init()		(0)
#define dasd_eer_exit()		do { } while (0)
#define dasd_eer_enable(d)	(0)
#define dasd_eer_disable(d)	do { } while (0)
#define dasd_eer_write(d,c,i)	do { } while (0)
#define dasd_eer_snss(d)	do { } while (0)
#define dasd_eer_enabled(d)	(0)
#endif	/* CONFIG_DASD_ERR */


/* DASD path handling functions */

/*
 * helper functions to modify bit masks for a given channel path for a device
 */
static inline int dasd_path_is_operational(struct dasd_device *device, int chp)
{
	return test_bit(DASD_PATH_OPERATIONAL, &device->path[chp].flags);
}

static inline int dasd_path_need_verify(struct dasd_device *device, int chp)
{
	return test_bit(DASD_PATH_TBV, &device->path[chp].flags);
}

static inline void dasd_path_verify(struct dasd_device *device, int chp)
{
	__set_bit(DASD_PATH_TBV, &device->path[chp].flags);
}

static inline void dasd_path_clear_verify(struct dasd_device *device, int chp)
{
	__clear_bit(DASD_PATH_TBV, &device->path[chp].flags);
}

static inline void dasd_path_clear_all_verify(struct dasd_device *device)
{
	int chp;

	for (chp = 0; chp < 8; chp++)
		dasd_path_clear_verify(device, chp);
}

static inline void dasd_path_fcsec(struct dasd_device *device, int chp)
{
	__set_bit(DASD_PATH_FCSEC, &device->path[chp].flags);
}

static inline void dasd_path_clear_fcsec(struct dasd_device *device, int chp)
{
	__clear_bit(DASD_PATH_FCSEC, &device->path[chp].flags);
}

static inline int dasd_path_need_fcsec(struct dasd_device *device, int chp)
{
	return test_bit(DASD_PATH_FCSEC, &device->path[chp].flags);
}

static inline void dasd_path_clear_all_fcsec(struct dasd_device *device)
{
	int chp;

	for (chp = 0; chp < 8; chp++)
		dasd_path_clear_fcsec(device, chp);
}

static inline void dasd_path_operational(struct dasd_device *device, int chp)
{
	__set_bit(DASD_PATH_OPERATIONAL, &device->path[chp].flags);
	device->opm |= (0x80 >> chp);
}

static inline void dasd_path_nonpreferred(struct dasd_device *device, int chp)
{
	__set_bit(DASD_PATH_NPP, &device->path[chp].flags);
}

static inline int dasd_path_is_nonpreferred(struct dasd_device *device, int chp)
{
	return test_bit(DASD_PATH_NPP, &device->path[chp].flags);
}

static inline void dasd_path_clear_nonpreferred(struct dasd_device *device,
						int chp)
{
	__clear_bit(DASD_PATH_NPP, &device->path[chp].flags);
}

static inline void dasd_path_preferred(struct dasd_device *device, int chp)
{
	__set_bit(DASD_PATH_PP, &device->path[chp].flags);
}

static inline int dasd_path_is_preferred(struct dasd_device *device, int chp)
{
	return test_bit(DASD_PATH_PP, &device->path[chp].flags);
}

static inline void dasd_path_clear_preferred(struct dasd_device *device,
					     int chp)
{
	__clear_bit(DASD_PATH_PP, &device->path[chp].flags);
}

static inline void dasd_path_clear_oper(struct dasd_device *device, int chp)
{
	__clear_bit(DASD_PATH_OPERATIONAL, &device->path[chp].flags);
	device->opm &= ~(0x80 >> chp);
}

static inline void dasd_path_clear_cable(struct dasd_device *device, int chp)
{
	__clear_bit(DASD_PATH_MISCABLED, &device->path[chp].flags);
}

static inline void dasd_path_cuir(struct dasd_device *device, int chp)
{
	__set_bit(DASD_PATH_CUIR, &device->path[chp].flags);
}

static inline int dasd_path_is_cuir(struct dasd_device *device, int chp)
{
	return test_bit(DASD_PATH_CUIR, &device->path[chp].flags);
}

static inline void dasd_path_clear_cuir(struct dasd_device *device, int chp)
{
	__clear_bit(DASD_PATH_CUIR, &device->path[chp].flags);
}

static inline void dasd_path_ifcc(struct dasd_device *device, int chp)
{
	set_bit(DASD_PATH_IFCC, &device->path[chp].flags);
}

static inline int dasd_path_is_ifcc(struct dasd_device *device, int chp)
{
	return test_bit(DASD_PATH_IFCC, &device->path[chp].flags);
}

static inline void dasd_path_clear_ifcc(struct dasd_device *device, int chp)
{
	clear_bit(DASD_PATH_IFCC, &device->path[chp].flags);
}

static inline void dasd_path_clear_nohpf(struct dasd_device *device, int chp)
{
	__clear_bit(DASD_PATH_NOHPF, &device->path[chp].flags);
}

static inline void dasd_path_miscabled(struct dasd_device *device, int chp)
{
	__set_bit(DASD_PATH_MISCABLED, &device->path[chp].flags);
}

static inline int dasd_path_is_miscabled(struct dasd_device *device, int chp)
{
	return test_bit(DASD_PATH_MISCABLED, &device->path[chp].flags);
}

static inline void dasd_path_nohpf(struct dasd_device *device, int chp)
{
	__set_bit(DASD_PATH_NOHPF, &device->path[chp].flags);
}

static inline int dasd_path_is_nohpf(struct dasd_device *device, int chp)
{
	return test_bit(DASD_PATH_NOHPF, &device->path[chp].flags);
}

/*
 * get functions for path masks
 * will return a path masks for the given device
 */

static inline __u8 dasd_path_get_opm(struct dasd_device *device)
{
	return device->opm;
}

static inline __u8 dasd_path_get_tbvpm(struct dasd_device *device)
{
	int chp;
	__u8 tbvpm = 0x00;

	for (chp = 0; chp < 8; chp++)
		if (dasd_path_need_verify(device, chp))
			tbvpm |= 0x80 >> chp;
	return tbvpm;
}

static inline int dasd_path_get_fcsecpm(struct dasd_device *device)
{
	int chp;

	for (chp = 0; chp < 8; chp++)
		if (dasd_path_need_fcsec(device, chp))
			return 1;

	return 0;
}

static inline __u8 dasd_path_get_nppm(struct dasd_device *device)
{
	int chp;
	__u8 npm = 0x00;

	for (chp = 0; chp < 8; chp++) {
		if (dasd_path_is_nonpreferred(device, chp))
			npm |= 0x80 >> chp;
	}
	return npm;
}

static inline __u8 dasd_path_get_ppm(struct dasd_device *device)
{
	int chp;
	__u8 ppm = 0x00;

	for (chp = 0; chp < 8; chp++)
		if (dasd_path_is_preferred(device, chp))
			ppm |= 0x80 >> chp;
	return ppm;
}

static inline __u8 dasd_path_get_cablepm(struct dasd_device *device)
{
	int chp;
	__u8 cablepm = 0x00;

	for (chp = 0; chp < 8; chp++)
		if (dasd_path_is_miscabled(device, chp))
			cablepm |= 0x80 >> chp;
	return cablepm;
}

static inline __u8 dasd_path_get_cuirpm(struct dasd_device *device)
{
	int chp;
	__u8 cuirpm = 0x00;

	for (chp = 0; chp < 8; chp++)
		if (dasd_path_is_cuir(device, chp))
			cuirpm |= 0x80 >> chp;
	return cuirpm;
}

static inline __u8 dasd_path_get_ifccpm(struct dasd_device *device)
{
	int chp;
	__u8 ifccpm = 0x00;

	for (chp = 0; chp < 8; chp++)
		if (dasd_path_is_ifcc(device, chp))
			ifccpm |= 0x80 >> chp;
	return ifccpm;
}

static inline __u8 dasd_path_get_hpfpm(struct dasd_device *device)
{
	int chp;
	__u8 hpfpm = 0x00;

	for (chp = 0; chp < 8; chp++)
		if (dasd_path_is_nohpf(device, chp))
			hpfpm |= 0x80 >> chp;
	return hpfpm;
}

static inline u8 dasd_path_get_fcs_path(struct dasd_device *device, int chp)
{
	return device->path[chp].fc_security;
}

static inline int dasd_path_get_fcs_device(struct dasd_device *device)
{
	u8 fc_sec = 0;
	int chp;

	for (chp = 0; chp < 8; chp++) {
		if (device->opm & (0x80 >> chp)) {
			fc_sec = device->path[chp].fc_security;
			break;
		}
	}
	for (; chp < 8; chp++) {
		if (device->opm & (0x80 >> chp))
			if (device->path[chp].fc_security != fc_sec)
				return -EINVAL;
	}

	return fc_sec;
}

/*
 * add functions for path masks
 * the existing path mask will be extended by the given path mask
 */
static inline void dasd_path_add_tbvpm(struct dasd_device *device, __u8 pm)
{
	int chp;

	for (chp = 0; chp < 8; chp++)
		if (pm & (0x80 >> chp))
			dasd_path_verify(device, chp);
}

static inline __u8 dasd_path_get_notoperpm(struct dasd_device *device)
{
	int chp;
	__u8 nopm = 0x00;

	for (chp = 0; chp < 8; chp++)
		if (dasd_path_is_nohpf(device, chp) ||
		    dasd_path_is_ifcc(device, chp) ||
		    dasd_path_is_cuir(device, chp) ||
		    dasd_path_is_miscabled(device, chp))
			nopm |= 0x80 >> chp;
	return nopm;
}

static inline void dasd_path_add_opm(struct dasd_device *device, __u8 pm)
{
	int chp;

	for (chp = 0; chp < 8; chp++)
		if (pm & (0x80 >> chp)) {
			dasd_path_operational(device, chp);
			/*
			 * if the path is used
			 * it should not be in one of the negative lists
			 */
			dasd_path_clear_nohpf(device, chp);
			dasd_path_clear_cuir(device, chp);
			dasd_path_clear_cable(device, chp);
			dasd_path_clear_ifcc(device, chp);
		}
}

static inline void dasd_path_add_cablepm(struct dasd_device *device, __u8 pm)
{
	int chp;

	for (chp = 0; chp < 8; chp++)
		if (pm & (0x80 >> chp))
			dasd_path_miscabled(device, chp);
}

static inline void dasd_path_add_cuirpm(struct dasd_device *device, __u8 pm)
{
	int chp;

	for (chp = 0; chp < 8; chp++)
		if (pm & (0x80 >> chp))
			dasd_path_cuir(device, chp);
}

static inline void dasd_path_add_ifccpm(struct dasd_device *device, __u8 pm)
{
	int chp;

	for (chp = 0; chp < 8; chp++)
		if (pm & (0x80 >> chp))
			dasd_path_ifcc(device, chp);
}

static inline void dasd_path_add_nppm(struct dasd_device *device, __u8 pm)
{
	int chp;

	for (chp = 0; chp < 8; chp++)
		if (pm & (0x80 >> chp))
			dasd_path_nonpreferred(device, chp);
}

static inline void dasd_path_add_nohpfpm(struct dasd_device *device, __u8 pm)
{
	int chp;

	for (chp = 0; chp < 8; chp++)
		if (pm & (0x80 >> chp))
			dasd_path_nohpf(device, chp);
}

static inline void dasd_path_add_ppm(struct dasd_device *device, __u8 pm)
{
	int chp;

	for (chp = 0; chp < 8; chp++)
		if (pm & (0x80 >> chp))
			dasd_path_preferred(device, chp);
}

/*
 * set functions for path masks
 * the existing path mask will be replaced by the given path mask
 */
static inline void dasd_path_set_tbvpm(struct dasd_device *device, __u8 pm)
{
	int chp;

	for (chp = 0; chp < 8; chp++)
		if (pm & (0x80 >> chp))
			dasd_path_verify(device, chp);
		else
			dasd_path_clear_verify(device, chp);
}

static inline void dasd_path_set_opm(struct dasd_device *device, __u8 pm)
{
	int chp;

	for (chp = 0; chp < 8; chp++) {
		dasd_path_clear_oper(device, chp);
		if (pm & (0x80 >> chp)) {
			dasd_path_operational(device, chp);
			/*
			 * if the path is used
			 * it should not be in one of the negative lists
			 */
			dasd_path_clear_nohpf(device, chp);
			dasd_path_clear_cuir(device, chp);
			dasd_path_clear_cable(device, chp);
			dasd_path_clear_ifcc(device, chp);
		}
	}
}

/*
 * remove functions for path masks
 * the existing path mask will be cleared with the given path mask
 */
static inline void dasd_path_remove_opm(struct dasd_device *device, __u8 pm)
{
	int chp;

	for (chp = 0; chp < 8; chp++) {
		if (pm & (0x80 >> chp))
			dasd_path_clear_oper(device, chp);
	}
}

/*
 * add the newly available path to the to be verified pm and remove it from
 * normal operation until it is verified
 */
static inline void dasd_path_available(struct dasd_device *device, int chp)
{
	dasd_path_clear_oper(device, chp);
	dasd_path_verify(device, chp);
}

static inline void dasd_path_notoper(struct dasd_device *device, int chp)
{
	dasd_path_clear_oper(device, chp);
	dasd_path_clear_preferred(device, chp);
	dasd_path_clear_nonpreferred(device, chp);
}

static inline void dasd_path_fcsec_update(struct dasd_device *device, int chp)
{
	dasd_path_fcsec(device, chp);
}

/*
 * remove all paths from normal operation
 */
static inline void dasd_path_no_path(struct dasd_device *device)
{
	int chp;

	for (chp = 0; chp < 8; chp++)
		dasd_path_notoper(device, chp);

	dasd_path_clear_all_verify(device);
}

/* end - path handling */

#endif				/* DASD_H */
