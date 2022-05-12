/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2003-2019, Intel Corporation. All rights reserved.
 * Intel Management Engine Interface (Intel MEI) Linux driver
 */

#ifndef _MEI_DEV_H_
#define _MEI_DEV_H_

#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/poll.h>
#include <linux/mei.h>
#include <linux/mei_cl_bus.h>

#include "hw.h"
#include "hbm.h"

#define MEI_SLOT_SIZE             sizeof(u32)
#define MEI_RD_MSG_BUF_SIZE       (128 * MEI_SLOT_SIZE)

/*
 * Number of Maximum MEI Clients
 */
#define MEI_CLIENTS_MAX 256

/*
 * maximum number of consecutive resets
 */
#define MEI_MAX_CONSEC_RESET  3

/*
 * Number of File descriptors/handles
 * that can be opened to the driver.
 *
 * Limit to 255: 256 Total Clients
 * minus internal client for MEI Bus Messages
 */
#define  MEI_MAX_OPEN_HANDLE_COUNT (MEI_CLIENTS_MAX - 1)

/* File state */
enum file_state {
	MEI_FILE_UNINITIALIZED = 0,
	MEI_FILE_INITIALIZING,
	MEI_FILE_CONNECTING,
	MEI_FILE_CONNECTED,
	MEI_FILE_DISCONNECTING,
	MEI_FILE_DISCONNECT_REPLY,
	MEI_FILE_DISCONNECT_REQUIRED,
	MEI_FILE_DISCONNECTED,
};

/* MEI device states */
enum mei_dev_state {
	MEI_DEV_INITIALIZING = 0,
	MEI_DEV_INIT_CLIENTS,
	MEI_DEV_ENABLED,
	MEI_DEV_RESETTING,
	MEI_DEV_DISABLED,
	MEI_DEV_POWERING_DOWN,
	MEI_DEV_POWER_DOWN,
	MEI_DEV_POWER_UP
};

const char *mei_dev_state_str(int state);

enum mei_file_transaction_states {
	MEI_IDLE,
	MEI_WRITING,
	MEI_WRITE_COMPLETE,
};

/**
 * enum mei_cb_file_ops  - file operation associated with the callback
 * @MEI_FOP_READ:       read
 * @MEI_FOP_WRITE:      write
 * @MEI_FOP_CONNECT:    connect
 * @MEI_FOP_DISCONNECT: disconnect
 * @MEI_FOP_DISCONNECT_RSP: disconnect response
 * @MEI_FOP_NOTIFY_START:   start notification
 * @MEI_FOP_NOTIFY_STOP:    stop notification
 * @MEI_FOP_DMA_MAP:   request client dma map
 * @MEI_FOP_DMA_UNMAP: request client dma unmap
 */
enum mei_cb_file_ops {
	MEI_FOP_READ = 0,
	MEI_FOP_WRITE,
	MEI_FOP_CONNECT,
	MEI_FOP_DISCONNECT,
	MEI_FOP_DISCONNECT_RSP,
	MEI_FOP_NOTIFY_START,
	MEI_FOP_NOTIFY_STOP,
	MEI_FOP_DMA_MAP,
	MEI_FOP_DMA_UNMAP,
};

/**
 * enum mei_cl_io_mode - io mode between driver and fw
 *
 * @MEI_CL_IO_TX_BLOCKING: send is blocking
 * @MEI_CL_IO_TX_INTERNAL: internal communication between driver and FW
 *
 * @MEI_CL_IO_RX_NONBLOCK: recv is non-blocking
 */
enum mei_cl_io_mode {
	MEI_CL_IO_TX_BLOCKING = BIT(0),
	MEI_CL_IO_TX_INTERNAL = BIT(1),

	MEI_CL_IO_RX_NONBLOCK = BIT(2),
};

/*
 * Intel MEI message data struct
 */
struct mei_msg_data {
	size_t size;
	unsigned char *data;
};

struct mei_dma_data {
	u8 buffer_id;
	void *vaddr;
	dma_addr_t daddr;
	size_t size;
};

/**
 * struct mei_dma_dscr - dma address descriptor
 *
 * @vaddr: dma buffer virtual address
 * @daddr: dma buffer physical address
 * @size : dma buffer size
 */
struct mei_dma_dscr {
	void *vaddr;
	dma_addr_t daddr;
	size_t size;
};

/* Maximum number of processed FW status registers */
#define MEI_FW_STATUS_MAX 6
/* Minimal  buffer for FW status string (8 bytes in dw + space or '\0') */
#define MEI_FW_STATUS_STR_SZ (MEI_FW_STATUS_MAX * (8 + 1))


/*
 * struct mei_fw_status - storage of FW status data
 *
 * @count: number of actually available elements in array
 * @status: FW status registers
 */
struct mei_fw_status {
	int count;
	u32 status[MEI_FW_STATUS_MAX];
};

/**
 * struct mei_me_client - representation of me (fw) client
 *
 * @list: link in me client list
 * @refcnt: struct reference count
 * @props: client properties
 * @client_id: me client id
 * @tx_flow_ctrl_creds: flow control credits
 * @connect_count: number connections to this client
 * @bus_added: added to bus
 */
struct mei_me_client {
	struct list_head list;
	struct kref refcnt;
	struct mei_client_properties props;
	u8 client_id;
	u8 tx_flow_ctrl_creds;
	u8 connect_count;
	u8 bus_added;
};


struct mei_cl;

/**
 * struct mei_cl_cb - file operation callback structure
 *
 * @list: link in callback queue
 * @cl: file client who is running this operation
 * @fop_type: file operation type
 * @buf: buffer for data associated with the callback
 * @buf_idx: last read index
 * @vtag: virtual tag
 * @fp: pointer to file structure
 * @status: io status of the cb
 * @internal: communication between driver and FW flag
 * @blocking: transmission blocking mode
 */
struct mei_cl_cb {
	struct list_head list;
	struct mei_cl *cl;
	enum mei_cb_file_ops fop_type;
	struct mei_msg_data buf;
	size_t buf_idx;
	u8 vtag;
	const struct file *fp;
	int status;
	u32 internal:1;
	u32 blocking:1;
};

/**
 * struct mei_cl_vtag - file pointer to vtag mapping structure
 *
 * @list: link in map queue
 * @fp: file pointer
 * @vtag: corresponding vtag
 * @pending_read: the read is pending on this file
 */
struct mei_cl_vtag {
	struct list_head list;
	const struct file *fp;
	u8 vtag;
	u8 pending_read:1;
};

/**
 * struct mei_cl - me client host representation
 *    carried in file->private_data
 *
 * @link: link in the clients list
 * @dev: mei parent device
 * @state: file operation state
 * @tx_wait: wait queue for tx completion
 * @rx_wait: wait queue for rx completion
 * @wait:  wait queue for management operation
 * @ev_wait: notification wait queue
 * @ev_async: event async notification
 * @status: connection status
 * @me_cl: fw client connected
 * @fp: file associated with client
 * @host_client_id: host id
 * @vtag_map: vtag map
 * @tx_flow_ctrl_creds: transmit flow credentials
 * @rx_flow_ctrl_creds: receive flow credentials
 * @timer_count:  watchdog timer for operation completion
 * @notify_en: notification - enabled/disabled
 * @notify_ev: pending notification event
 * @tx_cb_queued: number of tx callbacks in queue
 * @writing_state: state of the tx
 * @rd_pending: pending read credits
 * @rd_completed_lock: protects rd_completed queue
 * @rd_completed: completed read
 * @dma: dma settings
 * @dma_mapped: dma buffer is currently mapped.
 *
 * @cldev: device on the mei client bus
 */
struct mei_cl {
	struct list_head link;
	struct mei_device *dev;
	enum file_state state;
	wait_queue_head_t tx_wait;
	wait_queue_head_t rx_wait;
	wait_queue_head_t wait;
	wait_queue_head_t ev_wait;
	struct fasync_struct *ev_async;
	int status;
	struct mei_me_client *me_cl;
	const struct file *fp;
	u8 host_client_id;
	struct list_head vtag_map;
	u8 tx_flow_ctrl_creds;
	u8 rx_flow_ctrl_creds;
	u8 timer_count;
	u8 notify_en;
	u8 notify_ev;
	u8 tx_cb_queued;
	enum mei_file_transaction_states writing_state;
	struct list_head rd_pending;
	spinlock_t rd_completed_lock; /* protects rd_completed queue */
	struct list_head rd_completed;
	struct mei_dma_data dma;
	u8 dma_mapped;

	struct mei_cl_device *cldev;
};

#define MEI_TX_QUEUE_LIMIT_DEFAULT 50
#define MEI_TX_QUEUE_LIMIT_MAX 255
#define MEI_TX_QUEUE_LIMIT_MIN 30

/**
 * struct mei_hw_ops - hw specific ops
 *
 * @host_is_ready    : query for host readiness
 *
 * @hw_is_ready      : query if hw is ready
 * @hw_reset         : reset hw
 * @hw_start         : start hw after reset
 * @hw_config        : configure hw
 *
 * @fw_status        : get fw status registers
 * @trc_status       : get trc status register
 * @pg_state         : power gating state of the device
 * @pg_in_transition : is device now in pg transition
 * @pg_is_enabled    : is power gating enabled
 *
 * @intr_clear       : clear pending interrupts
 * @intr_enable      : enable interrupts
 * @intr_disable     : disable interrupts
 * @synchronize_irq  : synchronize irqs
 *
 * @hbuf_free_slots  : query for write buffer empty slots
 * @hbuf_is_ready    : query if write buffer is empty
 * @hbuf_depth       : query for write buffer depth
 *
 * @write            : write a message to FW
 *
 * @rdbuf_full_slots : query how many slots are filled
 *
 * @read_hdr         : get first 4 bytes (header)
 * @read             : read a buffer from the FW
 */
struct mei_hw_ops {

	bool (*host_is_ready)(struct mei_device *dev);

	bool (*hw_is_ready)(struct mei_device *dev);
	int (*hw_reset)(struct mei_device *dev, bool enable);
	int (*hw_start)(struct mei_device *dev);
	int (*hw_config)(struct mei_device *dev);

	int (*fw_status)(struct mei_device *dev, struct mei_fw_status *fw_sts);
	int (*trc_status)(struct mei_device *dev, u32 *trc);

	enum mei_pg_state (*pg_state)(struct mei_device *dev);
	bool (*pg_in_transition)(struct mei_device *dev);
	bool (*pg_is_enabled)(struct mei_device *dev);

	void (*intr_clear)(struct mei_device *dev);
	void (*intr_enable)(struct mei_device *dev);
	void (*intr_disable)(struct mei_device *dev);
	void (*synchronize_irq)(struct mei_device *dev);

	int (*hbuf_free_slots)(struct mei_device *dev);
	bool (*hbuf_is_ready)(struct mei_device *dev);
	u32 (*hbuf_depth)(const struct mei_device *dev);
	int (*write)(struct mei_device *dev,
		     const void *hdr, size_t hdr_len,
		     const void *data, size_t data_len);

	int (*rdbuf_full_slots)(struct mei_device *dev);

	u32 (*read_hdr)(const struct mei_device *dev);
	int (*read)(struct mei_device *dev,
		     unsigned char *buf, unsigned long len);
};

/* MEI bus API*/
void mei_cl_bus_rescan_work(struct work_struct *work);
void mei_cl_bus_dev_fixup(struct mei_cl_device *dev);
ssize_t __mei_cl_send(struct mei_cl *cl, u8 *buf, size_t length, u8 vtag,
		      unsigned int mode);
ssize_t __mei_cl_recv(struct mei_cl *cl, u8 *buf, size_t length, u8 *vtag,
		      unsigned int mode, unsigned long timeout);
bool mei_cl_bus_rx_event(struct mei_cl *cl);
bool mei_cl_bus_notify_event(struct mei_cl *cl);
void mei_cl_bus_remove_devices(struct mei_device *bus);
int mei_cl_bus_init(void);
void mei_cl_bus_exit(void);

/**
 * enum mei_pg_event - power gating transition events
 *
 * @MEI_PG_EVENT_IDLE: the driver is not in power gating transition
 * @MEI_PG_EVENT_WAIT: the driver is waiting for a pg event to complete
 * @MEI_PG_EVENT_RECEIVED: the driver received pg event
 * @MEI_PG_EVENT_INTR_WAIT: the driver is waiting for a pg event interrupt
 * @MEI_PG_EVENT_INTR_RECEIVED: the driver received pg event interrupt
 */
enum mei_pg_event {
	MEI_PG_EVENT_IDLE,
	MEI_PG_EVENT_WAIT,
	MEI_PG_EVENT_RECEIVED,
	MEI_PG_EVENT_INTR_WAIT,
	MEI_PG_EVENT_INTR_RECEIVED,
};

/**
 * enum mei_pg_state - device internal power gating state
 *
 * @MEI_PG_OFF: device is not power gated - it is active
 * @MEI_PG_ON:  device is power gated - it is in lower power state
 */
enum mei_pg_state {
	MEI_PG_OFF = 0,
	MEI_PG_ON =  1,
};

const char *mei_pg_state_str(enum mei_pg_state state);

/**
 * struct mei_fw_version - MEI FW version struct
 *
 * @platform: platform identifier
 * @major: major version field
 * @minor: minor version field
 * @buildno: build number version field
 * @hotfix: hotfix number version field
 */
struct mei_fw_version {
	u8 platform;
	u8 major;
	u16 minor;
	u16 buildno;
	u16 hotfix;
};

#define MEI_MAX_FW_VER_BLOCKS 3

/**
 * struct mei_device -  MEI private device struct
 *
 * @dev         : device on a bus
 * @cdev        : character device
 * @minor       : minor number allocated for device
 *
 * @write_list  : write pending list
 * @write_waiting_list : write completion list
 * @ctrl_wr_list : pending control write list
 * @ctrl_rd_list : pending control read list
 * @tx_queue_limit: tx queues per client linit
 *
 * @file_list   : list of opened handles
 * @open_handle_count: number of opened handles
 *
 * @device_lock : big device lock
 * @timer_work  : MEI timer delayed work (timeouts)
 *
 * @recvd_hw_ready : hw ready message received flag
 *
 * @wait_hw_ready : wait queue for receive HW ready message form FW
 * @wait_pg     : wait queue for receive PG message from FW
 * @wait_hbm_start : wait queue for receive HBM start message from FW
 *
 * @reset_count : number of consecutive resets
 * @dev_state   : device state
 * @hbm_state   : state of host bus message protocol
 * @init_clients_timer : HBM init handshake timeout
 *
 * @pg_event    : power gating event
 * @pg_domain   : runtime PM domain
 *
 * @rd_msg_buf  : control messages buffer
 * @rd_msg_hdr  : read message header storage
 * @rd_msg_hdr_count : how many dwords were already read from header
 *
 * @hbuf_is_ready : query if the host host/write buffer is ready
 * @dr_dscr: DMA ring descriptors: TX, RX, and CTRL
 *
 * @version     : HBM protocol version in use
 * @hbm_f_pg_supported  : hbm feature pgi protocol
 * @hbm_f_dc_supported  : hbm feature dynamic clients
 * @hbm_f_dot_supported : hbm feature disconnect on timeout
 * @hbm_f_ev_supported  : hbm feature event notification
 * @hbm_f_fa_supported  : hbm feature fixed address client
 * @hbm_f_ie_supported  : hbm feature immediate reply to enum request
 * @hbm_f_os_supported  : hbm feature support OS ver message
 * @hbm_f_dr_supported  : hbm feature dma ring supported
 * @hbm_f_vt_supported  : hbm feature vtag supported
 * @hbm_f_cap_supported : hbm feature capabilities message supported
 * @hbm_f_cd_supported  : hbm feature client dma supported
 *
 * @fw_ver : FW versions
 *
 * @fw_f_fw_ver_supported : fw feature: fw version supported
 *
 * @me_clients_rwsem: rw lock over me_clients list
 * @me_clients  : list of FW clients
 * @me_clients_map : FW clients bit map
 * @host_clients_map : host clients id pool
 *
 * @allow_fixed_address: allow user space to connect a fixed client
 * @override_fixed_address: force allow fixed address behavior
 *
 * @reset_work  : work item for the device reset
 * @bus_rescan_work : work item for the bus rescan
 *
 * @device_list : mei client bus list
 * @cl_bus_lock : client bus list lock
 *
 * @kind        : kind of mei device
 *
 * @dbgfs_dir   : debugfs mei root directory
 *
 * @ops:        : hw specific operations
 * @hw          : hw specific data
 */
struct mei_device {
	struct device *dev;
	struct cdev cdev;
	int minor;

	struct list_head write_list;
	struct list_head write_waiting_list;
	struct list_head ctrl_wr_list;
	struct list_head ctrl_rd_list;
	u8 tx_queue_limit;

	struct list_head file_list;
	long open_handle_count;

	struct mutex device_lock;
	struct delayed_work timer_work;

	bool recvd_hw_ready;
	/*
	 * waiting queue for receive message from FW
	 */
	wait_queue_head_t wait_hw_ready;
	wait_queue_head_t wait_pg;
	wait_queue_head_t wait_hbm_start;

	/*
	 * mei device  states
	 */
	unsigned long reset_count;
	enum mei_dev_state dev_state;
	enum mei_hbm_state hbm_state;
	u16 init_clients_timer;

	/*
	 * Power Gating support
	 */
	enum mei_pg_event pg_event;
#ifdef CONFIG_PM
	struct dev_pm_domain pg_domain;
#endif /* CONFIG_PM */

	unsigned char rd_msg_buf[MEI_RD_MSG_BUF_SIZE];
	u32 rd_msg_hdr[MEI_RD_MSG_BUF_SIZE];
	int rd_msg_hdr_count;

	/* write buffer */
	bool hbuf_is_ready;

	struct mei_dma_dscr dr_dscr[DMA_DSCR_NUM];

	struct hbm_version version;
	unsigned int hbm_f_pg_supported:1;
	unsigned int hbm_f_dc_supported:1;
	unsigned int hbm_f_dot_supported:1;
	unsigned int hbm_f_ev_supported:1;
	unsigned int hbm_f_fa_supported:1;
	unsigned int hbm_f_ie_supported:1;
	unsigned int hbm_f_os_supported:1;
	unsigned int hbm_f_dr_supported:1;
	unsigned int hbm_f_vt_supported:1;
	unsigned int hbm_f_cap_supported:1;
	unsigned int hbm_f_cd_supported:1;

	struct mei_fw_version fw_ver[MEI_MAX_FW_VER_BLOCKS];

	unsigned int fw_f_fw_ver_supported:1;

	struct rw_semaphore me_clients_rwsem;
	struct list_head me_clients;
	DECLARE_BITMAP(me_clients_map, MEI_CLIENTS_MAX);
	DECLARE_BITMAP(host_clients_map, MEI_CLIENTS_MAX);

	bool allow_fixed_address;
	bool override_fixed_address;

	struct work_struct reset_work;
	struct work_struct bus_rescan_work;

	/* List of bus devices */
	struct list_head device_list;
	struct mutex cl_bus_lock;

	const char *kind;

#if IS_ENABLED(CONFIG_DEBUG_FS)
	struct dentry *dbgfs_dir;
#endif /* CONFIG_DEBUG_FS */

	const struct mei_hw_ops *ops;
	char hw[] __aligned(sizeof(void *));
};

static inline unsigned long mei_secs_to_jiffies(unsigned long sec)
{
	return msecs_to_jiffies(sec * MSEC_PER_SEC);
}

/**
 * mei_data2slots - get slots number from a message length
 *
 * @length: size of the messages in bytes
 *
 * Return: number of slots
 */
static inline u32 mei_data2slots(size_t length)
{
	return DIV_ROUND_UP(length, MEI_SLOT_SIZE);
}

/**
 * mei_hbm2slots - get slots number from a hbm message length
 *                 length + size of the mei message header
 *
 * @length: size of the messages in bytes
 *
 * Return: number of slots
 */
static inline u32 mei_hbm2slots(size_t length)
{
	return DIV_ROUND_UP(sizeof(struct mei_msg_hdr) + length, MEI_SLOT_SIZE);
}

/**
 * mei_slots2data - get data in slots - bytes from slots
 *
 * @slots: number of available slots
 *
 * Return: number of bytes in slots
 */
static inline u32 mei_slots2data(int slots)
{
	return slots * MEI_SLOT_SIZE;
}

/*
 * mei init function prototypes
 */
void mei_device_init(struct mei_device *dev,
		     struct device *device,
		     const struct mei_hw_ops *hw_ops);
int mei_reset(struct mei_device *dev);
int mei_start(struct mei_device *dev);
int mei_restart(struct mei_device *dev);
void mei_stop(struct mei_device *dev);
void mei_cancel_work(struct mei_device *dev);

void mei_set_devstate(struct mei_device *dev, enum mei_dev_state state);

int mei_dmam_ring_alloc(struct mei_device *dev);
void mei_dmam_ring_free(struct mei_device *dev);
bool mei_dma_ring_is_allocated(struct mei_device *dev);
void mei_dma_ring_reset(struct mei_device *dev);
void mei_dma_ring_read(struct mei_device *dev, unsigned char *buf, u32 len);
void mei_dma_ring_write(struct mei_device *dev, unsigned char *buf, u32 len);
u32 mei_dma_ring_empty_slots(struct mei_device *dev);

/*
 *  MEI interrupt functions prototype
 */

void mei_timer(struct work_struct *work);
void mei_schedule_stall_timer(struct mei_device *dev);
int mei_irq_read_handler(struct mei_device *dev,
			 struct list_head *cmpl_list, s32 *slots);

int mei_irq_write_handler(struct mei_device *dev, struct list_head *cmpl_list);
void mei_irq_compl_handler(struct mei_device *dev, struct list_head *cmpl_list);

/*
 * Register Access Function
 */


static inline int mei_hw_config(struct mei_device *dev)
{
	return dev->ops->hw_config(dev);
}

static inline enum mei_pg_state mei_pg_state(struct mei_device *dev)
{
	return dev->ops->pg_state(dev);
}

static inline bool mei_pg_in_transition(struct mei_device *dev)
{
	return dev->ops->pg_in_transition(dev);
}

static inline bool mei_pg_is_enabled(struct mei_device *dev)
{
	return dev->ops->pg_is_enabled(dev);
}

static inline int mei_hw_reset(struct mei_device *dev, bool enable)
{
	return dev->ops->hw_reset(dev, enable);
}

static inline int mei_hw_start(struct mei_device *dev)
{
	return dev->ops->hw_start(dev);
}

static inline void mei_clear_interrupts(struct mei_device *dev)
{
	dev->ops->intr_clear(dev);
}

static inline void mei_enable_interrupts(struct mei_device *dev)
{
	dev->ops->intr_enable(dev);
}

static inline void mei_disable_interrupts(struct mei_device *dev)
{
	dev->ops->intr_disable(dev);
}

static inline void mei_synchronize_irq(struct mei_device *dev)
{
	dev->ops->synchronize_irq(dev);
}

static inline bool mei_host_is_ready(struct mei_device *dev)
{
	return dev->ops->host_is_ready(dev);
}
static inline bool mei_hw_is_ready(struct mei_device *dev)
{
	return dev->ops->hw_is_ready(dev);
}

static inline bool mei_hbuf_is_ready(struct mei_device *dev)
{
	return dev->ops->hbuf_is_ready(dev);
}

static inline int mei_hbuf_empty_slots(struct mei_device *dev)
{
	return dev->ops->hbuf_free_slots(dev);
}

static inline u32 mei_hbuf_depth(const struct mei_device *dev)
{
	return dev->ops->hbuf_depth(dev);
}

static inline int mei_write_message(struct mei_device *dev,
				    const void *hdr, size_t hdr_len,
				    const void *data, size_t data_len)
{
	return dev->ops->write(dev, hdr, hdr_len, data, data_len);
}

static inline u32 mei_read_hdr(const struct mei_device *dev)
{
	return dev->ops->read_hdr(dev);
}

static inline void mei_read_slots(struct mei_device *dev,
		     unsigned char *buf, unsigned long len)
{
	dev->ops->read(dev, buf, len);
}

static inline int mei_count_full_read_slots(struct mei_device *dev)
{
	return dev->ops->rdbuf_full_slots(dev);
}

static inline int mei_trc_status(struct mei_device *dev, u32 *trc)
{
	if (dev->ops->trc_status)
		return dev->ops->trc_status(dev, trc);
	return -EOPNOTSUPP;
}

static inline int mei_fw_status(struct mei_device *dev,
				struct mei_fw_status *fw_status)
{
	return dev->ops->fw_status(dev, fw_status);
}

bool mei_hbuf_acquire(struct mei_device *dev);

bool mei_write_is_idle(struct mei_device *dev);

#if IS_ENABLED(CONFIG_DEBUG_FS)
void mei_dbgfs_register(struct mei_device *dev, const char *name);
void mei_dbgfs_deregister(struct mei_device *dev);
#else
static inline void mei_dbgfs_register(struct mei_device *dev, const char *name) {}
static inline void mei_dbgfs_deregister(struct mei_device *dev) {}
#endif /* CONFIG_DEBUG_FS */

int mei_register(struct mei_device *dev, struct device *parent);
void mei_deregister(struct mei_device *dev);

#define MEI_HDR_FMT "hdr:host=%02d me=%02d len=%d dma=%1d ext=%1d internal=%1d comp=%1d"
#define MEI_HDR_PRM(hdr)                  \
	(hdr)->host_addr, (hdr)->me_addr, \
	(hdr)->length, (hdr)->dma_ring, (hdr)->extended, \
	(hdr)->internal, (hdr)->msg_complete

ssize_t mei_fw_status2str(struct mei_fw_status *fw_sts, char *buf, size_t len);
/**
 * mei_fw_status_str - fetch and convert fw status registers to printable string
 *
 * @dev: the device structure
 * @buf: string buffer at minimal size MEI_FW_STATUS_STR_SZ
 * @len: buffer len must be >= MEI_FW_STATUS_STR_SZ
 *
 * Return: number of bytes written or < 0 on failure
 */
static inline ssize_t mei_fw_status_str(struct mei_device *dev,
					char *buf, size_t len)
{
	struct mei_fw_status fw_status;
	int ret;

	buf[0] = '\0';

	ret = mei_fw_status(dev, &fw_status);
	if (ret)
		return ret;

	ret = mei_fw_status2str(&fw_status, buf, MEI_FW_STATUS_STR_SZ);

	return ret;
}


#endif
