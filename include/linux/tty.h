/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_TTY_H
#define _LINUX_TTY_H

#include <linux/fs.h>
#include <linux/major.h>
#include <linux/termios.h>
#include <linux/workqueue.h>
#include <linux/tty_driver.h>
#include <linux/tty_ldisc.h>
#include <linux/mutex.h>
#include <linux/tty_flags.h>
#include <linux/seq_file.h>
#include <uapi/linux/tty.h>
#include <linux/rwsem.h>
#include <linux/llist.h>


/*
 * (Note: the *_driver.minor_start values 1, 64, 128, 192 are
 * hardcoded at present.)
 */
#define NR_UNIX98_PTY_DEFAULT	4096      /* Default maximum for Unix98 ptys */
#define NR_UNIX98_PTY_RESERVE	1024	  /* Default reserve for main devpts */
#define NR_UNIX98_PTY_MAX	(1 << MINORBITS) /* Absolute limit */

/*
 * This character is the same as _POSIX_VDISABLE: it cannot be used as
 * a c_cc[] character, but indicates that a particular special character
 * isn't in use (eg VINTR has no character etc)
 */
#define __DISABLED_CHAR '\0'

struct tty_buffer {
	union {
		struct tty_buffer *next;
		struct llist_node free;
	};
	int used;
	int size;
	int commit;
	int read;
	int flags;
	/* Data points here */
	unsigned long data[];
};

/* Values for .flags field of tty_buffer */
#define TTYB_NORMAL	1	/* buffer has no flags buffer */

static inline unsigned char *char_buf_ptr(struct tty_buffer *b, int ofs)
{
	return ((unsigned char *)b->data) + ofs;
}

static inline char *flag_buf_ptr(struct tty_buffer *b, int ofs)
{
	return (char *)char_buf_ptr(b, ofs) + b->size;
}

struct tty_bufhead {
	struct tty_buffer *head;	/* Queue head */
	struct work_struct work;
	struct mutex	   lock;
	atomic_t	   priority;
	struct tty_buffer sentinel;
	struct llist_head free;		/* Free queue head */
	atomic_t	   mem_used;    /* In-use buffers excluding free list */
	int		   mem_limit;
	struct tty_buffer *tail;	/* Active buffer */
};
/*
 * When a break, frame error, or parity error happens, these codes are
 * stuffed into the flags buffer.
 */
#define TTY_NORMAL	0
#define TTY_BREAK	1
#define TTY_FRAME	2
#define TTY_PARITY	3
#define TTY_OVERRUN	4

#define INTR_CHAR(tty) ((tty)->termios.c_cc[VINTR])
#define QUIT_CHAR(tty) ((tty)->termios.c_cc[VQUIT])
#define ERASE_CHAR(tty) ((tty)->termios.c_cc[VERASE])
#define KILL_CHAR(tty) ((tty)->termios.c_cc[VKILL])
#define EOF_CHAR(tty) ((tty)->termios.c_cc[VEOF])
#define TIME_CHAR(tty) ((tty)->termios.c_cc[VTIME])
#define MIN_CHAR(tty) ((tty)->termios.c_cc[VMIN])
#define SWTC_CHAR(tty) ((tty)->termios.c_cc[VSWTC])
#define START_CHAR(tty) ((tty)->termios.c_cc[VSTART])
#define STOP_CHAR(tty) ((tty)->termios.c_cc[VSTOP])
#define SUSP_CHAR(tty) ((tty)->termios.c_cc[VSUSP])
#define EOL_CHAR(tty) ((tty)->termios.c_cc[VEOL])
#define REPRINT_CHAR(tty) ((tty)->termios.c_cc[VREPRINT])
#define DISCARD_CHAR(tty) ((tty)->termios.c_cc[VDISCARD])
#define WERASE_CHAR(tty) ((tty)->termios.c_cc[VWERASE])
#define LNEXT_CHAR(tty)	((tty)->termios.c_cc[VLNEXT])
#define EOL2_CHAR(tty) ((tty)->termios.c_cc[VEOL2])

#define _I_FLAG(tty, f)	((tty)->termios.c_iflag & (f))
#define _O_FLAG(tty, f)	((tty)->termios.c_oflag & (f))
#define _C_FLAG(tty, f)	((tty)->termios.c_cflag & (f))
#define _L_FLAG(tty, f)	((tty)->termios.c_lflag & (f))

#define I_IGNBRK(tty)	_I_FLAG((tty), IGNBRK)
#define I_BRKINT(tty)	_I_FLAG((tty), BRKINT)
#define I_IGNPAR(tty)	_I_FLAG((tty), IGNPAR)
#define I_PARMRK(tty)	_I_FLAG((tty), PARMRK)
#define I_INPCK(tty)	_I_FLAG((tty), INPCK)
#define I_ISTRIP(tty)	_I_FLAG((tty), ISTRIP)
#define I_INLCR(tty)	_I_FLAG((tty), INLCR)
#define I_IGNCR(tty)	_I_FLAG((tty), IGNCR)
#define I_ICRNL(tty)	_I_FLAG((tty), ICRNL)
#define I_IUCLC(tty)	_I_FLAG((tty), IUCLC)
#define I_IXON(tty)	_I_FLAG((tty), IXON)
#define I_IXANY(tty)	_I_FLAG((tty), IXANY)
#define I_IXOFF(tty)	_I_FLAG((tty), IXOFF)
#define I_IMAXBEL(tty)	_I_FLAG((tty), IMAXBEL)
#define I_IUTF8(tty)	_I_FLAG((tty), IUTF8)

#define O_OPOST(tty)	_O_FLAG((tty), OPOST)
#define O_OLCUC(tty)	_O_FLAG((tty), OLCUC)
#define O_ONLCR(tty)	_O_FLAG((tty), ONLCR)
#define O_OCRNL(tty)	_O_FLAG((tty), OCRNL)
#define O_ONOCR(tty)	_O_FLAG((tty), ONOCR)
#define O_ONLRET(tty)	_O_FLAG((tty), ONLRET)
#define O_OFILL(tty)	_O_FLAG((tty), OFILL)
#define O_OFDEL(tty)	_O_FLAG((tty), OFDEL)
#define O_NLDLY(tty)	_O_FLAG((tty), NLDLY)
#define O_CRDLY(tty)	_O_FLAG((tty), CRDLY)
#define O_TABDLY(tty)	_O_FLAG((tty), TABDLY)
#define O_BSDLY(tty)	_O_FLAG((tty), BSDLY)
#define O_VTDLY(tty)	_O_FLAG((tty), VTDLY)
#define O_FFDLY(tty)	_O_FLAG((tty), FFDLY)

#define C_BAUD(tty)	_C_FLAG((tty), CBAUD)
#define C_CSIZE(tty)	_C_FLAG((tty), CSIZE)
#define C_CSTOPB(tty)	_C_FLAG((tty), CSTOPB)
#define C_CREAD(tty)	_C_FLAG((tty), CREAD)
#define C_PARENB(tty)	_C_FLAG((tty), PARENB)
#define C_PARODD(tty)	_C_FLAG((tty), PARODD)
#define C_HUPCL(tty)	_C_FLAG((tty), HUPCL)
#define C_CLOCAL(tty)	_C_FLAG((tty), CLOCAL)
#define C_CIBAUD(tty)	_C_FLAG((tty), CIBAUD)
#define C_CRTSCTS(tty)	_C_FLAG((tty), CRTSCTS)
#define C_CMSPAR(tty)	_C_FLAG((tty), CMSPAR)

#define L_ISIG(tty)	_L_FLAG((tty), ISIG)
#define L_ICANON(tty)	_L_FLAG((tty), ICANON)
#define L_XCASE(tty)	_L_FLAG((tty), XCASE)
#define L_ECHO(tty)	_L_FLAG((tty), ECHO)
#define L_ECHOE(tty)	_L_FLAG((tty), ECHOE)
#define L_ECHOK(tty)	_L_FLAG((tty), ECHOK)
#define L_ECHONL(tty)	_L_FLAG((tty), ECHONL)
#define L_NOFLSH(tty)	_L_FLAG((tty), NOFLSH)
#define L_TOSTOP(tty)	_L_FLAG((tty), TOSTOP)
#define L_ECHOCTL(tty)	_L_FLAG((tty), ECHOCTL)
#define L_ECHOPRT(tty)	_L_FLAG((tty), ECHOPRT)
#define L_ECHOKE(tty)	_L_FLAG((tty), ECHOKE)
#define L_FLUSHO(tty)	_L_FLAG((tty), FLUSHO)
#define L_PENDIN(tty)	_L_FLAG((tty), PENDIN)
#define L_IEXTEN(tty)	_L_FLAG((tty), IEXTEN)
#define L_EXTPROC(tty)	_L_FLAG((tty), EXTPROC)

struct device;
struct signal_struct;

/*
 * Port level information. Each device keeps its own port level information
 * so provide a common structure for those ports wanting to use common support
 * routines.
 *
 * The tty port has a different lifetime to the tty so must be kept apart.
 * In addition be careful as tty -> port mappings are valid for the life
 * of the tty object but in many cases port -> tty mappings are valid only
 * until a hangup so don't use the wrong path.
 */

struct tty_port;

struct tty_port_operations {
	/* Return 1 if the carrier is raised */
	int (*carrier_raised)(struct tty_port *port);
	/* Control the DTR line */
	void (*dtr_rts)(struct tty_port *port, int raise);
	/* Called when the last close completes or a hangup finishes
	   IFF the port was initialized. Do not use to free resources. Called
	   under the port mutex to serialize against activate/shutdowns */
	void (*shutdown)(struct tty_port *port);
	/* Called under the port mutex from tty_port_open, serialized using
	   the port mutex */
        /* FIXME: long term getting the tty argument *out* of this would be
           good for consoles */
	int (*activate)(struct tty_port *port, struct tty_struct *tty);
	/* Called on the final put of a port */
	void (*destruct)(struct tty_port *port);
};

struct tty_port_client_operations {
	int (*receive_buf)(struct tty_port *port, const unsigned char *, const unsigned char *, size_t);
	void (*write_wakeup)(struct tty_port *port);
};

extern const struct tty_port_client_operations tty_port_default_client_ops;

struct tty_port {
	struct tty_bufhead	buf;		/* Locked internally */
	struct tty_struct	*tty;		/* Back pointer */
	struct tty_struct	*itty;		/* internal back ptr */
	const struct tty_port_operations *ops;	/* Port operations */
	const struct tty_port_client_operations *client_ops; /* Port client operations */
	spinlock_t		lock;		/* Lock protecting tty field */
	int			blocked_open;	/* Waiting to open */
	int			count;		/* Usage count */
	wait_queue_head_t	open_wait;	/* Open waiters */
	wait_queue_head_t	delta_msr_wait;	/* Modem status change */
	unsigned long		flags;		/* User TTY flags ASYNC_ */
	unsigned long		iflags;		/* Internal flags TTY_PORT_ */
	unsigned char		console:1;	/* port is a console */
	struct mutex		mutex;		/* Locking */
	struct mutex		buf_mutex;	/* Buffer alloc lock */
	unsigned char		*xmit_buf;	/* Optional buffer */
	unsigned int		close_delay;	/* Close port delay */
	unsigned int		closing_wait;	/* Delay for output */
	int			drain_delay;	/* Set to zero if no pure time
						   based drain is needed else
						   set to size of fifo */
	struct kref		kref;		/* Ref counter */
	void 			*client_data;
};

/* tty_port::iflags bits -- use atomic bit ops */
#define TTY_PORT_INITIALIZED	0	/* device is initialized */
#define TTY_PORT_SUSPENDED	1	/* device is suspended */
#define TTY_PORT_ACTIVE		2	/* device is open */

/*
 * uart drivers: use the uart_port::status field and the UPSTAT_* defines
 * for s/w-based flow control steering and carrier detection status
 */
#define TTY_PORT_CTS_FLOW	3	/* h/w flow control enabled */
#define TTY_PORT_CHECK_CD	4	/* carrier detect enabled */
#define TTY_PORT_KOPENED	5	/* device exclusively opened by
					   kernel */

/*
 * Where all of the state associated with a tty is kept while the tty
 * is open.  Since the termios state should be kept even if the tty
 * has been closed --- for things like the baud rate, etc --- it is
 * not stored here, but rather a pointer to the real state is stored
 * here.  Possible the winsize structure should have the same
 * treatment, but (1) the default 80x24 is usually right and (2) it's
 * most often used by a windowing system, which will set the correct
 * size each time the window is created or resized anyway.
 * 						- TYT, 9/14/92
 */

struct tty_operations;

struct tty_struct {
	int	magic;
	struct kref kref;
	struct device *dev;	/* class device or NULL (e.g. ptys, serdev) */
	struct tty_driver *driver;
	const struct tty_operations *ops;
	int index;

	/* Protects ldisc changes: Lock tty not pty */
	struct ld_semaphore ldisc_sem;
	struct tty_ldisc *ldisc;

	struct mutex atomic_write_lock;
	struct mutex legacy_mutex;
	struct mutex throttle_mutex;
	struct rw_semaphore termios_rwsem;
	struct mutex winsize_mutex;
	spinlock_t ctrl_lock;
	spinlock_t flow_lock;
	/* Termios values are protected by the termios rwsem */
	struct ktermios termios, termios_locked;
	char name[64];
	struct pid *pgrp;		/* Protected by ctrl lock */
	/*
	 * Writes protected by both ctrl lock and legacy mutex, readers must use
	 * at least one of them.
	 */
	struct pid *session;
	unsigned long flags;
	int count;
	struct winsize winsize;		/* winsize_mutex */
	unsigned long stopped:1,	/* flow_lock */
		      flow_stopped:1,
		      unused:BITS_PER_LONG - 2;
	int hw_stopped;
	unsigned long ctrl_status:8,	/* ctrl_lock */
		      packet:1,
		      unused_ctrl:BITS_PER_LONG - 9;
	unsigned int receive_room;	/* Bytes free for queue */
	int flow_change;

	struct tty_struct *link;
	struct fasync_struct *fasync;
	wait_queue_head_t write_wait;
	wait_queue_head_t read_wait;
	struct work_struct hangup_work;
	void *disc_data;
	void *driver_data;
	spinlock_t files_lock;		/* protects tty_files list */
	struct list_head tty_files;

#define N_TTY_BUF_SIZE 4096

	int closing;
	unsigned char *write_buf;
	int write_cnt;
	/* If the tty has a pending do_SAK, queue it here - akpm */
	struct work_struct SAK_work;
	struct tty_port *port;
} __randomize_layout;

/* Each of a tty's open files has private_data pointing to tty_file_private */
struct tty_file_private {
	struct tty_struct *tty;
	struct file *file;
	struct list_head list;
};

/* tty magic number */
#define TTY_MAGIC		0x5401

/*
 * These bits are used in the flags field of the tty structure.
 *
 * So that interrupts won't be able to mess up the queues,
 * copy_to_cooked must be atomic with respect to itself, as must
 * tty->write.  Thus, you must use the inline functions set_bit() and
 * clear_bit() to make things atomic.
 */
#define TTY_THROTTLED 		0	/* Call unthrottle() at threshold min */
#define TTY_IO_ERROR 		1	/* Cause an I/O error (may be no ldisc too) */
#define TTY_OTHER_CLOSED 	2	/* Other side (if any) has closed */
#define TTY_EXCLUSIVE 		3	/* Exclusive open mode */
#define TTY_DO_WRITE_WAKEUP 	5	/* Call write_wakeup after queuing new */
#define TTY_LDISC_OPEN	 	11	/* Line discipline is open */
#define TTY_PTY_LOCK 		16	/* pty private */
#define TTY_NO_WRITE_SPLIT 	17	/* Preserve write boundaries to driver */
#define TTY_HUPPED 		18	/* Post driver->hangup() */
#define TTY_HUPPING		19	/* Hangup in progress */
#define TTY_LDISC_CHANGING	20	/* Change pending - non-block IO */
#define TTY_LDISC_HALTED	22	/* Line discipline is halted */

static inline bool tty_io_nonblock(struct tty_struct *tty, struct file *file)
{
	return file->f_flags & O_NONBLOCK ||
		test_bit(TTY_LDISC_CHANGING, &tty->flags);
}

static inline bool tty_io_error(struct tty_struct *tty)
{
	return test_bit(TTY_IO_ERROR, &tty->flags);
}

static inline bool tty_throttled(struct tty_struct *tty)
{
	return test_bit(TTY_THROTTLED, &tty->flags);
}

#ifdef CONFIG_TTY
extern void tty_kref_put(struct tty_struct *tty);
extern struct pid *tty_get_pgrp(struct tty_struct *tty);
extern void tty_vhangup_self(void);
extern void disassociate_ctty(int priv);
extern dev_t tty_devnum(struct tty_struct *tty);
extern void proc_clear_tty(struct task_struct *p);
extern struct tty_struct *get_current_tty(void);
/* tty_io.c */
extern int __init tty_init(void);
extern const char *tty_name(const struct tty_struct *tty);
extern struct tty_struct *tty_kopen_exclusive(dev_t device);
extern struct tty_struct *tty_kopen_shared(dev_t device);
extern void tty_kclose(struct tty_struct *tty);
extern int tty_dev_name_to_number(const char *name, dev_t *number);
#else
static inline void tty_kref_put(struct tty_struct *tty)
{ }
static inline struct pid *tty_get_pgrp(struct tty_struct *tty)
{ return NULL; }
static inline void tty_vhangup_self(void)
{ }
static inline void disassociate_ctty(int priv)
{ }
static inline dev_t tty_devnum(struct tty_struct *tty)
{ return 0; }
static inline void proc_clear_tty(struct task_struct *p)
{ }
static inline struct tty_struct *get_current_tty(void)
{ return NULL; }
/* tty_io.c */
static inline int __init tty_init(void)
{ return 0; }
static inline const char *tty_name(const struct tty_struct *tty)
{ return "(none)"; }
static inline struct tty_struct *tty_kopen_exclusive(dev_t device)
{ return ERR_PTR(-ENODEV); }
static inline void tty_kclose(struct tty_struct *tty)
{ }
static inline int tty_dev_name_to_number(const char *name, dev_t *number)
{ return -ENOTSUPP; }
#endif

extern struct ktermios tty_std_termios;

extern int vcs_init(void);

extern struct class *tty_class;

/**
 *	tty_kref_get		-	get a tty reference
 *	@tty: tty device
 *
 *	Return a new reference to a tty object. The caller must hold
 *	sufficient locks/counts to ensure that their existing reference cannot
 *	go away
 */

static inline struct tty_struct *tty_kref_get(struct tty_struct *tty)
{
	if (tty)
		kref_get(&tty->kref);
	return tty;
}

extern const char *tty_driver_name(const struct tty_struct *tty);
extern void tty_wait_until_sent(struct tty_struct *tty, long timeout);
extern void stop_tty(struct tty_struct *tty);
extern void start_tty(struct tty_struct *tty);
extern int tty_register_driver(struct tty_driver *driver);
extern void tty_unregister_driver(struct tty_driver *driver);
extern struct device *tty_register_device(struct tty_driver *driver,
					  unsigned index, struct device *dev);
extern struct device *tty_register_device_attr(struct tty_driver *driver,
				unsigned index, struct device *device,
				void *drvdata,
				const struct attribute_group **attr_grp);
extern void tty_unregister_device(struct tty_driver *driver, unsigned index);
extern void tty_write_message(struct tty_struct *tty, char *msg);
extern int tty_send_xchar(struct tty_struct *tty, char ch);
extern int tty_put_char(struct tty_struct *tty, unsigned char c);
extern int tty_chars_in_buffer(struct tty_struct *tty);
extern int tty_write_room(struct tty_struct *tty);
extern void tty_driver_flush_buffer(struct tty_struct *tty);
extern void tty_throttle(struct tty_struct *tty);
extern void tty_unthrottle(struct tty_struct *tty);
extern int tty_throttle_safe(struct tty_struct *tty);
extern int tty_unthrottle_safe(struct tty_struct *tty);
extern int tty_do_resize(struct tty_struct *tty, struct winsize *ws);
extern int tty_get_icount(struct tty_struct *tty,
			  struct serial_icounter_struct *icount);
extern int is_current_pgrp_orphaned(void);
extern void tty_hangup(struct tty_struct *tty);
extern void tty_vhangup(struct tty_struct *tty);
extern int tty_hung_up_p(struct file *filp);
extern void do_SAK(struct tty_struct *tty);
extern void __do_SAK(struct tty_struct *tty);
extern void no_tty(void);
extern speed_t tty_termios_baud_rate(struct ktermios *termios);
extern void tty_termios_encode_baud_rate(struct ktermios *termios,
						speed_t ibaud, speed_t obaud);
extern void tty_encode_baud_rate(struct tty_struct *tty,
						speed_t ibaud, speed_t obaud);

/**
 *	tty_get_baud_rate	-	get tty bit rates
 *	@tty: tty to query
 *
 *	Returns the baud rate as an integer for this terminal. The
 *	termios lock must be held by the caller and the terminal bit
 *	flags may be updated.
 *
 *	Locking: none
 */
static inline speed_t tty_get_baud_rate(struct tty_struct *tty)
{
	return tty_termios_baud_rate(&tty->termios);
}

extern void tty_termios_copy_hw(struct ktermios *new, struct ktermios *old);
extern int tty_termios_hw_change(const struct ktermios *a, const struct ktermios *b);
extern int tty_set_termios(struct tty_struct *tty, struct ktermios *kt);

extern struct tty_ldisc *tty_ldisc_ref(struct tty_struct *);
extern void tty_ldisc_deref(struct tty_ldisc *);
extern struct tty_ldisc *tty_ldisc_ref_wait(struct tty_struct *);
extern const struct seq_operations tty_ldiscs_seq_ops;

extern void tty_wakeup(struct tty_struct *tty);
extern void tty_ldisc_flush(struct tty_struct *tty);

extern int tty_mode_ioctl(struct tty_struct *tty, struct file *file,
			unsigned int cmd, unsigned long arg);
extern int tty_perform_flush(struct tty_struct *tty, unsigned long arg);
extern struct tty_struct *tty_init_dev(struct tty_driver *driver, int idx);
extern void tty_release_struct(struct tty_struct *tty, int idx);
extern void tty_init_termios(struct tty_struct *tty);
extern void tty_save_termios(struct tty_struct *tty);
extern int tty_standard_install(struct tty_driver *driver,
		struct tty_struct *tty);

extern struct mutex tty_mutex;

extern void tty_port_init(struct tty_port *port);
extern void tty_port_link_device(struct tty_port *port,
		struct tty_driver *driver, unsigned index);
extern struct device *tty_port_register_device(struct tty_port *port,
		struct tty_driver *driver, unsigned index,
		struct device *device);
extern struct device *tty_port_register_device_attr(struct tty_port *port,
		struct tty_driver *driver, unsigned index,
		struct device *device, void *drvdata,
		const struct attribute_group **attr_grp);
extern struct device *tty_port_register_device_serdev(struct tty_port *port,
		struct tty_driver *driver, unsigned index,
		struct device *device);
extern struct device *tty_port_register_device_attr_serdev(struct tty_port *port,
		struct tty_driver *driver, unsigned index,
		struct device *device, void *drvdata,
		const struct attribute_group **attr_grp);
extern void tty_port_unregister_device(struct tty_port *port,
		struct tty_driver *driver, unsigned index);
extern int tty_port_alloc_xmit_buf(struct tty_port *port);
extern void tty_port_free_xmit_buf(struct tty_port *port);
extern void tty_port_destroy(struct tty_port *port);
extern void tty_port_put(struct tty_port *port);

static inline struct tty_port *tty_port_get(struct tty_port *port)
{
	if (port && kref_get_unless_zero(&port->kref))
		return port;
	return NULL;
}

/* If the cts flow control is enabled, return true. */
static inline bool tty_port_cts_enabled(const struct tty_port *port)
{
	return test_bit(TTY_PORT_CTS_FLOW, &port->iflags);
}

static inline void tty_port_set_cts_flow(struct tty_port *port, bool val)
{
	assign_bit(TTY_PORT_CTS_FLOW, &port->iflags, val);
}

static inline bool tty_port_active(const struct tty_port *port)
{
	return test_bit(TTY_PORT_ACTIVE, &port->iflags);
}

static inline void tty_port_set_active(struct tty_port *port, bool val)
{
	assign_bit(TTY_PORT_ACTIVE, &port->iflags, val);
}

static inline bool tty_port_check_carrier(const struct tty_port *port)
{
	return test_bit(TTY_PORT_CHECK_CD, &port->iflags);
}

static inline void tty_port_set_check_carrier(struct tty_port *port, bool val)
{
	assign_bit(TTY_PORT_CHECK_CD, &port->iflags, val);
}

static inline bool tty_port_suspended(const struct tty_port *port)
{
	return test_bit(TTY_PORT_SUSPENDED, &port->iflags);
}

static inline void tty_port_set_suspended(struct tty_port *port, bool val)
{
	assign_bit(TTY_PORT_SUSPENDED, &port->iflags, val);
}

static inline bool tty_port_initialized(const struct tty_port *port)
{
	return test_bit(TTY_PORT_INITIALIZED, &port->iflags);
}

static inline void tty_port_set_initialized(struct tty_port *port, bool val)
{
	assign_bit(TTY_PORT_INITIALIZED, &port->iflags, val);
}

static inline bool tty_port_kopened(const struct tty_port *port)
{
	return test_bit(TTY_PORT_KOPENED, &port->iflags);
}

static inline void tty_port_set_kopened(struct tty_port *port, bool val)
{
	assign_bit(TTY_PORT_KOPENED, &port->iflags, val);
}

extern struct tty_struct *tty_port_tty_get(struct tty_port *port);
extern void tty_port_tty_set(struct tty_port *port, struct tty_struct *tty);
extern int tty_port_carrier_raised(struct tty_port *port);
extern void tty_port_raise_dtr_rts(struct tty_port *port);
extern void tty_port_lower_dtr_rts(struct tty_port *port);
extern void tty_port_hangup(struct tty_port *port);
extern void tty_port_tty_hangup(struct tty_port *port, bool check_clocal);
extern void tty_port_tty_wakeup(struct tty_port *port);
extern int tty_port_block_til_ready(struct tty_port *port,
				struct tty_struct *tty, struct file *filp);
extern int tty_port_close_start(struct tty_port *port,
				struct tty_struct *tty, struct file *filp);
extern void tty_port_close_end(struct tty_port *port, struct tty_struct *tty);
extern void tty_port_close(struct tty_port *port,
				struct tty_struct *tty, struct file *filp);
extern int tty_port_install(struct tty_port *port, struct tty_driver *driver,
				struct tty_struct *tty);
extern int tty_port_open(struct tty_port *port,
				struct tty_struct *tty, struct file *filp);
static inline int tty_port_users(struct tty_port *port)
{
	return port->count + port->blocked_open;
}

extern int tty_register_ldisc(int disc, struct tty_ldisc_ops *new_ldisc);
extern int tty_unregister_ldisc(int disc);
extern int tty_set_ldisc(struct tty_struct *tty, int disc);
extern int tty_ldisc_receive_buf(struct tty_ldisc *ld, const unsigned char *p,
				 char *f, int count);

/* n_tty.c */
extern void n_tty_inherit_ops(struct tty_ldisc_ops *ops);
#ifdef CONFIG_TTY
extern void __init n_tty_init(void);
#else
static inline void n_tty_init(void) { }
#endif

/* tty_audit.c */
#ifdef CONFIG_AUDIT
extern void tty_audit_exit(void);
extern void tty_audit_fork(struct signal_struct *sig);
extern int tty_audit_push(void);
#else
static inline void tty_audit_exit(void)
{
}
static inline void tty_audit_fork(struct signal_struct *sig)
{
}
static inline int tty_audit_push(void)
{
	return 0;
}
#endif

/* tty_ioctl.c */
extern int n_tty_ioctl_helper(struct tty_struct *tty, struct file *file,
		       unsigned int cmd, unsigned long arg);

/* vt.c */

extern int vt_ioctl(struct tty_struct *tty,
		    unsigned int cmd, unsigned long arg);

extern long vt_compat_ioctl(struct tty_struct *tty,
		     unsigned int cmd, unsigned long arg);

/* tty_mutex.c */
/* functions for preparation of BKL removal */
extern void tty_lock(struct tty_struct *tty);
extern int  tty_lock_interruptible(struct tty_struct *tty);
extern void tty_unlock(struct tty_struct *tty);
extern void tty_lock_slave(struct tty_struct *tty);
extern void tty_unlock_slave(struct tty_struct *tty);
extern void tty_set_lock_subclass(struct tty_struct *tty);

#ifdef CONFIG_PROC_FS
extern void proc_tty_register_driver(struct tty_driver *);
extern void proc_tty_unregister_driver(struct tty_driver *);
#else
static inline void proc_tty_register_driver(struct tty_driver *d) {}
static inline void proc_tty_unregister_driver(struct tty_driver *d) {}
#endif

#endif
