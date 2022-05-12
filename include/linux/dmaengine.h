/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2004 - 2006 Intel Corporation. All rights reserved.
 */
#ifndef LINUX_DMAENGINE_H
#define LINUX_DMAENGINE_H

#include <linux/device.h>
#include <linux/err.h>
#include <linux/uio.h>
#include <linux/bug.h>
#include <linux/scatterlist.h>
#include <linux/bitmap.h>
#include <linux/types.h>
#include <asm/page.h>

/**
 * typedef dma_cookie_t - an opaque DMA cookie
 *
 * if dma_cookie_t is >0 it's a DMA request cookie, <0 it's an error code
 */
typedef s32 dma_cookie_t;
#define DMA_MIN_COOKIE	1

static inline int dma_submit_error(dma_cookie_t cookie)
{
	return cookie < 0 ? cookie : 0;
}

/**
 * enum dma_status - DMA transaction status
 * @DMA_COMPLETE: transaction completed
 * @DMA_IN_PROGRESS: transaction not yet processed
 * @DMA_PAUSED: transaction is paused
 * @DMA_ERROR: transaction failed
 */
enum dma_status {
	DMA_COMPLETE,
	DMA_IN_PROGRESS,
	DMA_PAUSED,
	DMA_ERROR,
	DMA_OUT_OF_ORDER,
};

/**
 * enum dma_transaction_type - DMA transaction types/indexes
 *
 * Note: The DMA_ASYNC_TX capability is not to be set by drivers.  It is
 * automatically set as dma devices are registered.
 */
enum dma_transaction_type {
	DMA_MEMCPY,
	DMA_XOR,
	DMA_PQ,
	DMA_XOR_VAL,
	DMA_PQ_VAL,
	DMA_MEMSET,
	DMA_MEMSET_SG,
	DMA_INTERRUPT,
	DMA_PRIVATE,
	DMA_ASYNC_TX,
	DMA_SLAVE,
	DMA_CYCLIC,
	DMA_INTERLEAVE,
	DMA_COMPLETION_NO_ORDER,
	DMA_REPEAT,
	DMA_LOAD_EOT,
/* last transaction type for creation of the capabilities mask */
	DMA_TX_TYPE_END,
};

/**
 * enum dma_transfer_direction - dma transfer mode and direction indicator
 * @DMA_MEM_TO_MEM: Async/Memcpy mode
 * @DMA_MEM_TO_DEV: Slave mode & From Memory to Device
 * @DMA_DEV_TO_MEM: Slave mode & From Device to Memory
 * @DMA_DEV_TO_DEV: Slave mode & From Device to Device
 */
enum dma_transfer_direction {
	DMA_MEM_TO_MEM,
	DMA_MEM_TO_DEV,
	DMA_DEV_TO_MEM,
	DMA_DEV_TO_DEV,
	DMA_TRANS_NONE,
};

/**
 * Interleaved Transfer Request
 * ----------------------------
 * A chunk is collection of contiguous bytes to be transferred.
 * The gap(in bytes) between two chunks is called inter-chunk-gap(ICG).
 * ICGs may or may not change between chunks.
 * A FRAME is the smallest series of contiguous {chunk,icg} pairs,
 *  that when repeated an integral number of times, specifies the transfer.
 * A transfer template is specification of a Frame, the number of times
 *  it is to be repeated and other per-transfer attributes.
 *
 * Practically, a client driver would have ready a template for each
 *  type of transfer it is going to need during its lifetime and
 *  set only 'src_start' and 'dst_start' before submitting the requests.
 *
 *
 *  |      Frame-1        |       Frame-2       | ~ |       Frame-'numf'  |
 *  |====....==.===...=...|====....==.===...=...| ~ |====....==.===...=...|
 *
 *    ==  Chunk size
 *    ... ICG
 */

/**
 * struct data_chunk - Element of scatter-gather list that makes a frame.
 * @size: Number of bytes to read from source.
 *	  size_dst := fn(op, size_src), so doesn't mean much for destination.
 * @icg: Number of bytes to jump after last src/dst address of this
 *	 chunk and before first src/dst address for next chunk.
 *	 Ignored for dst(assumed 0), if dst_inc is true and dst_sgl is false.
 *	 Ignored for src(assumed 0), if src_inc is true and src_sgl is false.
 * @dst_icg: Number of bytes to jump after last dst address of this
 *	 chunk and before the first dst address for next chunk.
 *	 Ignored if dst_inc is true and dst_sgl is false.
 * @src_icg: Number of bytes to jump after last src address of this
 *	 chunk and before the first src address for next chunk.
 *	 Ignored if src_inc is true and src_sgl is false.
 */
struct data_chunk {
	size_t size;
	size_t icg;
	size_t dst_icg;
	size_t src_icg;
};

/**
 * struct dma_interleaved_template - Template to convey DMAC the transfer pattern
 *	 and attributes.
 * @src_start: Bus address of source for the first chunk.
 * @dst_start: Bus address of destination for the first chunk.
 * @dir: Specifies the type of Source and Destination.
 * @src_inc: If the source address increments after reading from it.
 * @dst_inc: If the destination address increments after writing to it.
 * @src_sgl: If the 'icg' of sgl[] applies to Source (scattered read).
 *		Otherwise, source is read contiguously (icg ignored).
 *		Ignored if src_inc is false.
 * @dst_sgl: If the 'icg' of sgl[] applies to Destination (scattered write).
 *		Otherwise, destination is filled contiguously (icg ignored).
 *		Ignored if dst_inc is false.
 * @numf: Number of frames in this template.
 * @frame_size: Number of chunks in a frame i.e, size of sgl[].
 * @sgl: Array of {chunk,icg} pairs that make up a frame.
 */
struct dma_interleaved_template {
	dma_addr_t src_start;
	dma_addr_t dst_start;
	enum dma_transfer_direction dir;
	bool src_inc;
	bool dst_inc;
	bool src_sgl;
	bool dst_sgl;
	size_t numf;
	size_t frame_size;
	struct data_chunk sgl[];
};

/**
 * enum dma_ctrl_flags - DMA flags to augment operation preparation,
 *  control completion, and communicate status.
 * @DMA_PREP_INTERRUPT - trigger an interrupt (callback) upon completion of
 *  this transaction
 * @DMA_CTRL_ACK - if clear, the descriptor cannot be reused until the client
 *  acknowledges receipt, i.e. has a chance to establish any dependency
 *  chains
 * @DMA_PREP_PQ_DISABLE_P - prevent generation of P while generating Q
 * @DMA_PREP_PQ_DISABLE_Q - prevent generation of Q while generating P
 * @DMA_PREP_CONTINUE - indicate to a driver that it is reusing buffers as
 *  sources that were the result of a previous operation, in the case of a PQ
 *  operation it continues the calculation with new sources
 * @DMA_PREP_FENCE - tell the driver that subsequent operations depend
 *  on the result of this operation
 * @DMA_CTRL_REUSE: client can reuse the descriptor and submit again till
 *  cleared or freed
 * @DMA_PREP_CMD: tell the driver that the data passed to DMA API is command
 *  data and the descriptor should be in different format from normal
 *  data descriptors.
 * @DMA_PREP_REPEAT: tell the driver that the transaction shall be automatically
 *  repeated when it ends until a transaction is issued on the same channel
 *  with the DMA_PREP_LOAD_EOT flag set. This flag is only applicable to
 *  interleaved transactions and is ignored for all other transaction types.
 * @DMA_PREP_LOAD_EOT: tell the driver that the transaction shall replace any
 *  active repeated (as indicated by DMA_PREP_REPEAT) transaction when the
 *  repeated transaction ends. Not setting this flag when the previously queued
 *  transaction is marked with DMA_PREP_REPEAT will cause the new transaction
 *  to never be processed and stay in the issued queue forever. The flag is
 *  ignored if the previous transaction is not a repeated transaction.
 */
enum dma_ctrl_flags {
	DMA_PREP_INTERRUPT = (1 << 0),
	DMA_CTRL_ACK = (1 << 1),
	DMA_PREP_PQ_DISABLE_P = (1 << 2),
	DMA_PREP_PQ_DISABLE_Q = (1 << 3),
	DMA_PREP_CONTINUE = (1 << 4),
	DMA_PREP_FENCE = (1 << 5),
	DMA_CTRL_REUSE = (1 << 6),
	DMA_PREP_CMD = (1 << 7),
	DMA_PREP_REPEAT = (1 << 8),
	DMA_PREP_LOAD_EOT = (1 << 9),
};

/**
 * enum sum_check_bits - bit position of pq_check_flags
 */
enum sum_check_bits {
	SUM_CHECK_P = 0,
	SUM_CHECK_Q = 1,
};

/**
 * enum pq_check_flags - result of async_{xor,pq}_zero_sum operations
 * @SUM_CHECK_P_RESULT - 1 if xor zero sum error, 0 otherwise
 * @SUM_CHECK_Q_RESULT - 1 if reed-solomon zero sum error, 0 otherwise
 */
enum sum_check_flags {
	SUM_CHECK_P_RESULT = (1 << SUM_CHECK_P),
	SUM_CHECK_Q_RESULT = (1 << SUM_CHECK_Q),
};


/**
 * dma_cap_mask_t - capabilities bitmap modeled after cpumask_t.
 * See linux/cpumask.h
 */
typedef struct { DECLARE_BITMAP(bits, DMA_TX_TYPE_END); } dma_cap_mask_t;

/**
 * struct dma_chan_percpu - the per-CPU part of struct dma_chan
 * @memcpy_count: transaction counter
 * @bytes_transferred: byte counter
 */

/**
 * enum dma_desc_metadata_mode - per descriptor metadata mode types supported
 * @DESC_METADATA_CLIENT - the metadata buffer is allocated/provided by the
 *  client driver and it is attached (via the dmaengine_desc_attach_metadata()
 *  helper) to the descriptor.
 *
 * Client drivers interested to use this mode can follow:
 * - DMA_MEM_TO_DEV / DEV_MEM_TO_MEM:
 *   1. prepare the descriptor (dmaengine_prep_*)
 *	construct the metadata in the client's buffer
 *   2. use dmaengine_desc_attach_metadata() to attach the buffer to the
 *	descriptor
 *   3. submit the transfer
 * - DMA_DEV_TO_MEM:
 *   1. prepare the descriptor (dmaengine_prep_*)
 *   2. use dmaengine_desc_attach_metadata() to attach the buffer to the
 *	descriptor
 *   3. submit the transfer
 *   4. when the transfer is completed, the metadata should be available in the
 *	attached buffer
 *
 * @DESC_METADATA_ENGINE - the metadata buffer is allocated/managed by the DMA
 *  driver. The client driver can ask for the pointer, maximum size and the
 *  currently used size of the metadata and can directly update or read it.
 *  dmaengine_desc_get_metadata_ptr() and dmaengine_desc_set_metadata_len() is
 *  provided as helper functions.
 *
 *  Note: the metadata area for the descriptor is no longer valid after the
 *  transfer has been completed (valid up to the point when the completion
 *  callback returns if used).
 *
 * Client drivers interested to use this mode can follow:
 * - DMA_MEM_TO_DEV / DEV_MEM_TO_MEM:
 *   1. prepare the descriptor (dmaengine_prep_*)
 *   2. use dmaengine_desc_get_metadata_ptr() to get the pointer to the engine's
 *	metadata area
 *   3. update the metadata at the pointer
 *   4. use dmaengine_desc_set_metadata_len()  to tell the DMA engine the amount
 *	of data the client has placed into the metadata buffer
 *   5. submit the transfer
 * - DMA_DEV_TO_MEM:
 *   1. prepare the descriptor (dmaengine_prep_*)
 *   2. submit the transfer
 *   3. on transfer completion, use dmaengine_desc_get_metadata_ptr() to get the
 *	pointer to the engine's metadata area
 *   4. Read out the metadata from the pointer
 *
 * Note: the two mode is not compatible and clients must use one mode for a
 * descriptor.
 */
enum dma_desc_metadata_mode {
	DESC_METADATA_NONE = 0,
	DESC_METADATA_CLIENT = BIT(0),
	DESC_METADATA_ENGINE = BIT(1),
};

struct dma_chan_percpu {
	/* stats */
	unsigned long memcpy_count;
	unsigned long bytes_transferred;
};

/**
 * struct dma_router - DMA router structure
 * @dev: pointer to the DMA router device
 * @route_free: function to be called when the route can be disconnected
 */
struct dma_router {
	struct device *dev;
	void (*route_free)(struct device *dev, void *route_data);
};

/**
 * struct dma_chan - devices supply DMA channels, clients use them
 * @device: ptr to the dma device who supplies this channel, always !%NULL
 * @slave: ptr to the device using this channel
 * @cookie: last cookie value returned to client
 * @completed_cookie: last completed cookie for this channel
 * @chan_id: channel ID for sysfs
 * @dev: class device for sysfs
 * @name: backlink name for sysfs
 * @dbg_client_name: slave name for debugfs in format:
 *	dev_name(requester's dev):channel name, for example: "2b00000.mcasp:tx"
 * @device_node: used to add this to the device chan list
 * @local: per-cpu pointer to a struct dma_chan_percpu
 * @client_count: how many clients are using this channel
 * @table_count: number of appearances in the mem-to-mem allocation table
 * @router: pointer to the DMA router structure
 * @route_data: channel specific data for the router
 * @private: private data for certain client-channel associations
 */
struct dma_chan {
	struct dma_device *device;
	struct device *slave;
	dma_cookie_t cookie;
	dma_cookie_t completed_cookie;

	/* sysfs */
	int chan_id;
	struct dma_chan_dev *dev;
	const char *name;
#ifdef CONFIG_DEBUG_FS
	char *dbg_client_name;
#endif

	struct list_head device_node;
	struct dma_chan_percpu __percpu *local;
	int client_count;
	int table_count;

	/* DMA router */
	struct dma_router *router;
	void *route_data;

	void *private;
};

/**
 * struct dma_chan_dev - relate sysfs device node to backing channel device
 * @chan: driver channel device
 * @device: sysfs device
 * @dev_id: parent dma_device dev_id
 * @chan_dma_dev: The channel is using custom/different dma-mapping
 * compared to the parent dma_device
 */
struct dma_chan_dev {
	struct dma_chan *chan;
	struct device device;
	int dev_id;
	bool chan_dma_dev;
};

/**
 * enum dma_slave_buswidth - defines bus width of the DMA slave
 * device, source or target buses
 */
enum dma_slave_buswidth {
	DMA_SLAVE_BUSWIDTH_UNDEFINED = 0,
	DMA_SLAVE_BUSWIDTH_1_BYTE = 1,
	DMA_SLAVE_BUSWIDTH_2_BYTES = 2,
	DMA_SLAVE_BUSWIDTH_3_BYTES = 3,
	DMA_SLAVE_BUSWIDTH_4_BYTES = 4,
	DMA_SLAVE_BUSWIDTH_8_BYTES = 8,
	DMA_SLAVE_BUSWIDTH_16_BYTES = 16,
	DMA_SLAVE_BUSWIDTH_32_BYTES = 32,
	DMA_SLAVE_BUSWIDTH_64_BYTES = 64,
};

/**
 * struct dma_slave_config - dma slave channel runtime config
 * @direction: whether the data shall go in or out on this slave
 * channel, right now. DMA_MEM_TO_DEV and DMA_DEV_TO_MEM are
 * legal values. DEPRECATED, drivers should use the direction argument
 * to the device_prep_slave_sg and device_prep_dma_cyclic functions or
 * the dir field in the dma_interleaved_template structure.
 * @src_addr: this is the physical address where DMA slave data
 * should be read (RX), if the source is memory this argument is
 * ignored.
 * @dst_addr: this is the physical address where DMA slave data
 * should be written (TX), if the source is memory this argument
 * is ignored.
 * @src_addr_width: this is the width in bytes of the source (RX)
 * register where DMA data shall be read. If the source
 * is memory this may be ignored depending on architecture.
 * Legal values: 1, 2, 3, 4, 8, 16, 32, 64.
 * @dst_addr_width: same as src_addr_width but for destination
 * target (TX) mutatis mutandis.
 * @src_maxburst: the maximum number of words (note: words, as in
 * units of the src_addr_width member, not bytes) that can be sent
 * in one burst to the device. Typically something like half the
 * FIFO depth on I/O peripherals so you don't overflow it. This
 * may or may not be applicable on memory sources.
 * @dst_maxburst: same as src_maxburst but for destination target
 * mutatis mutandis.
 * @src_port_window_size: The length of the register area in words the data need
 * to be accessed on the device side. It is only used for devices which is using
 * an area instead of a single register to receive the data. Typically the DMA
 * loops in this area in order to transfer the data.
 * @dst_port_window_size: same as src_port_window_size but for the destination
 * port.
 * @device_fc: Flow Controller Settings. Only valid for slave channels. Fill
 * with 'true' if peripheral should be flow controller. Direction will be
 * selected at Runtime.
 * @slave_id: Slave requester id. Only valid for slave channels. The dma
 * slave peripheral will have unique id as dma requester which need to be
 * pass as slave config.
 * @peripheral_config: peripheral configuration for programming peripheral
 * for dmaengine transfer
 * @peripheral_size: peripheral configuration buffer size
 *
 * This struct is passed in as configuration data to a DMA engine
 * in order to set up a certain channel for DMA transport at runtime.
 * The DMA device/engine has to provide support for an additional
 * callback in the dma_device structure, device_config and this struct
 * will then be passed in as an argument to the function.
 *
 * The rationale for adding configuration information to this struct is as
 * follows: if it is likely that more than one DMA slave controllers in
 * the world will support the configuration option, then make it generic.
 * If not: if it is fixed so that it be sent in static from the platform
 * data, then prefer to do that.
 */
struct dma_slave_config {
	enum dma_transfer_direction direction;
	phys_addr_t src_addr;
	phys_addr_t dst_addr;
	enum dma_slave_buswidth src_addr_width;
	enum dma_slave_buswidth dst_addr_width;
	u32 src_maxburst;
	u32 dst_maxburst;
	u32 src_port_window_size;
	u32 dst_port_window_size;
	bool device_fc;
	unsigned int slave_id;
	void *peripheral_config;
	size_t peripheral_size;
};

/**
 * enum dma_residue_granularity - Granularity of the reported transfer residue
 * @DMA_RESIDUE_GRANULARITY_DESCRIPTOR: Residue reporting is not support. The
 *  DMA channel is only able to tell whether a descriptor has been completed or
 *  not, which means residue reporting is not supported by this channel. The
 *  residue field of the dma_tx_state field will always be 0.
 * @DMA_RESIDUE_GRANULARITY_SEGMENT: Residue is updated after each successfully
 *  completed segment of the transfer (For cyclic transfers this is after each
 *  period). This is typically implemented by having the hardware generate an
 *  interrupt after each transferred segment and then the drivers updates the
 *  outstanding residue by the size of the segment. Another possibility is if
 *  the hardware supports scatter-gather and the segment descriptor has a field
 *  which gets set after the segment has been completed. The driver then counts
 *  the number of segments without the flag set to compute the residue.
 * @DMA_RESIDUE_GRANULARITY_BURST: Residue is updated after each transferred
 *  burst. This is typically only supported if the hardware has a progress
 *  register of some sort (E.g. a register with the current read/write address
 *  or a register with the amount of bursts/beats/bytes that have been
 *  transferred or still need to be transferred).
 */
enum dma_residue_granularity {
	DMA_RESIDUE_GRANULARITY_DESCRIPTOR = 0,
	DMA_RESIDUE_GRANULARITY_SEGMENT = 1,
	DMA_RESIDUE_GRANULARITY_BURST = 2,
};

/**
 * struct dma_slave_caps - expose capabilities of a slave channel only
 * @src_addr_widths: bit mask of src addr widths the channel supports.
 *	Width is specified in bytes, e.g. for a channel supporting
 *	a width of 4 the mask should have BIT(4) set.
 * @dst_addr_widths: bit mask of dst addr widths the channel supports
 * @directions: bit mask of slave directions the channel supports.
 *	Since the enum dma_transfer_direction is not defined as bit flag for
 *	each type, the dma controller should set BIT(<TYPE>) and same
 *	should be checked by controller as well
 * @min_burst: min burst capability per-transfer
 * @max_burst: max burst capability per-transfer
 * @max_sg_burst: max number of SG list entries executed in a single burst
 *	DMA tansaction with no software intervention for reinitialization.
 *	Zero value means unlimited number of entries.
 * @cmd_pause: true, if pause is supported (i.e. for reading residue or
 *	       for resume later)
 * @cmd_resume: true, if resume is supported
 * @cmd_terminate: true, if terminate cmd is supported
 * @residue_granularity: granularity of the reported transfer residue
 * @descriptor_reuse: if a descriptor can be reused by client and
 * resubmitted multiple times
 */
struct dma_slave_caps {
	u32 src_addr_widths;
	u32 dst_addr_widths;
	u32 directions;
	u32 min_burst;
	u32 max_burst;
	u32 max_sg_burst;
	bool cmd_pause;
	bool cmd_resume;
	bool cmd_terminate;
	enum dma_residue_granularity residue_granularity;
	bool descriptor_reuse;
};

static inline const char *dma_chan_name(struct dma_chan *chan)
{
	return dev_name(&chan->dev->device);
}

void dma_chan_cleanup(struct kref *kref);

/**
 * typedef dma_filter_fn - callback filter for dma_request_channel
 * @chan: channel to be reviewed
 * @filter_param: opaque parameter passed through dma_request_channel
 *
 * When this optional parameter is specified in a call to dma_request_channel a
 * suitable channel is passed to this routine for further dispositioning before
 * being returned.  Where 'suitable' indicates a non-busy channel that
 * satisfies the given capability mask.  It returns 'true' to indicate that the
 * channel is suitable.
 */
typedef bool (*dma_filter_fn)(struct dma_chan *chan, void *filter_param);

typedef void (*dma_async_tx_callback)(void *dma_async_param);

enum dmaengine_tx_result {
	DMA_TRANS_NOERROR = 0,		/* SUCCESS */
	DMA_TRANS_READ_FAILED,		/* Source DMA read failed */
	DMA_TRANS_WRITE_FAILED,		/* Destination DMA write failed */
	DMA_TRANS_ABORTED,		/* Op never submitted / aborted */
};

struct dmaengine_result {
	enum dmaengine_tx_result result;
	u32 residue;
};

typedef void (*dma_async_tx_callback_result)(void *dma_async_param,
				const struct dmaengine_result *result);

struct dmaengine_unmap_data {
#if IS_ENABLED(CONFIG_DMA_ENGINE_RAID)
	u16 map_cnt;
#else
	u8 map_cnt;
#endif
	u8 to_cnt;
	u8 from_cnt;
	u8 bidi_cnt;
	struct device *dev;
	struct kref kref;
	size_t len;
	dma_addr_t addr[];
};

struct dma_async_tx_descriptor;

struct dma_descriptor_metadata_ops {
	int (*attach)(struct dma_async_tx_descriptor *desc, void *data,
		      size_t len);

	void *(*get_ptr)(struct dma_async_tx_descriptor *desc,
			 size_t *payload_len, size_t *max_len);
	int (*set_len)(struct dma_async_tx_descriptor *desc,
		       size_t payload_len);
};

/**
 * struct dma_async_tx_descriptor - async transaction descriptor
 * ---dma generic offload fields---
 * @cookie: tracking cookie for this transaction, set to -EBUSY if
 *	this tx is sitting on a dependency list
 * @flags: flags to augment operation preparation, control completion, and
 *	communicate status
 * @phys: physical address of the descriptor
 * @chan: target channel for this operation
 * @tx_submit: accept the descriptor, assign ordered cookie and mark the
 * descriptor pending. To be pushed on .issue_pending() call
 * @callback: routine to call after this operation is complete
 * @callback_param: general parameter to pass to the callback routine
 * @desc_metadata_mode: core managed metadata mode to protect mixed use of
 *	DESC_METADATA_CLIENT or DESC_METADATA_ENGINE. Otherwise
 *	DESC_METADATA_NONE
 * @metadata_ops: DMA driver provided metadata mode ops, need to be set by the
 *	DMA driver if metadata mode is supported with the descriptor
 * ---async_tx api specific fields---
 * @next: at completion submit this descriptor
 * @parent: pointer to the next level up in the dependency chain
 * @lock: protect the parent and next pointers
 */
struct dma_async_tx_descriptor {
	dma_cookie_t cookie;
	enum dma_ctrl_flags flags; /* not a 'long' to pack with cookie */
	dma_addr_t phys;
	struct dma_chan *chan;
	dma_cookie_t (*tx_submit)(struct dma_async_tx_descriptor *tx);
	int (*desc_free)(struct dma_async_tx_descriptor *tx);
	dma_async_tx_callback callback;
	dma_async_tx_callback_result callback_result;
	void *callback_param;
	struct dmaengine_unmap_data *unmap;
	enum dma_desc_metadata_mode desc_metadata_mode;
	struct dma_descriptor_metadata_ops *metadata_ops;
#ifdef CONFIG_ASYNC_TX_ENABLE_CHANNEL_SWITCH
	struct dma_async_tx_descriptor *next;
	struct dma_async_tx_descriptor *parent;
	spinlock_t lock;
#endif
};

#ifdef CONFIG_DMA_ENGINE
static inline void dma_set_unmap(struct dma_async_tx_descriptor *tx,
				 struct dmaengine_unmap_data *unmap)
{
	kref_get(&unmap->kref);
	tx->unmap = unmap;
}

struct dmaengine_unmap_data *
dmaengine_get_unmap_data(struct device *dev, int nr, gfp_t flags);
void dmaengine_unmap_put(struct dmaengine_unmap_data *unmap);
#else
static inline void dma_set_unmap(struct dma_async_tx_descriptor *tx,
				 struct dmaengine_unmap_data *unmap)
{
}
static inline struct dmaengine_unmap_data *
dmaengine_get_unmap_data(struct device *dev, int nr, gfp_t flags)
{
	return NULL;
}
static inline void dmaengine_unmap_put(struct dmaengine_unmap_data *unmap)
{
}
#endif

static inline void dma_descriptor_unmap(struct dma_async_tx_descriptor *tx)
{
	if (!tx->unmap)
		return;

	dmaengine_unmap_put(tx->unmap);
	tx->unmap = NULL;
}

#ifndef CONFIG_ASYNC_TX_ENABLE_CHANNEL_SWITCH
static inline void txd_lock(struct dma_async_tx_descriptor *txd)
{
}
static inline void txd_unlock(struct dma_async_tx_descriptor *txd)
{
}
static inline void txd_chain(struct dma_async_tx_descriptor *txd, struct dma_async_tx_descriptor *next)
{
	BUG();
}
static inline void txd_clear_parent(struct dma_async_tx_descriptor *txd)
{
}
static inline void txd_clear_next(struct dma_async_tx_descriptor *txd)
{
}
static inline struct dma_async_tx_descriptor *txd_next(struct dma_async_tx_descriptor *txd)
{
	return NULL;
}
static inline struct dma_async_tx_descriptor *txd_parent(struct dma_async_tx_descriptor *txd)
{
	return NULL;
}

#else
static inline void txd_lock(struct dma_async_tx_descriptor *txd)
{
	spin_lock_bh(&txd->lock);
}
static inline void txd_unlock(struct dma_async_tx_descriptor *txd)
{
	spin_unlock_bh(&txd->lock);
}
static inline void txd_chain(struct dma_async_tx_descriptor *txd, struct dma_async_tx_descriptor *next)
{
	txd->next = next;
	next->parent = txd;
}
static inline void txd_clear_parent(struct dma_async_tx_descriptor *txd)
{
	txd->parent = NULL;
}
static inline void txd_clear_next(struct dma_async_tx_descriptor *txd)
{
	txd->next = NULL;
}
static inline struct dma_async_tx_descriptor *txd_parent(struct dma_async_tx_descriptor *txd)
{
	return txd->parent;
}
static inline struct dma_async_tx_descriptor *txd_next(struct dma_async_tx_descriptor *txd)
{
	return txd->next;
}
#endif

/**
 * struct dma_tx_state - filled in to report the status of
 * a transfer.
 * @last: last completed DMA cookie
 * @used: last issued DMA cookie (i.e. the one in progress)
 * @residue: the remaining number of bytes left to transmit
 *	on the selected transfer for states DMA_IN_PROGRESS and
 *	DMA_PAUSED if this is implemented in the driver, else 0
 * @in_flight_bytes: amount of data in bytes cached by the DMA.
 */
struct dma_tx_state {
	dma_cookie_t last;
	dma_cookie_t used;
	u32 residue;
	u32 in_flight_bytes;
};

/**
 * enum dmaengine_alignment - defines alignment of the DMA async tx
 * buffers
 */
enum dmaengine_alignment {
	DMAENGINE_ALIGN_1_BYTE = 0,
	DMAENGINE_ALIGN_2_BYTES = 1,
	DMAENGINE_ALIGN_4_BYTES = 2,
	DMAENGINE_ALIGN_8_BYTES = 3,
	DMAENGINE_ALIGN_16_BYTES = 4,
	DMAENGINE_ALIGN_32_BYTES = 5,
	DMAENGINE_ALIGN_64_BYTES = 6,
	DMAENGINE_ALIGN_128_BYTES = 7,
	DMAENGINE_ALIGN_256_BYTES = 8,
};

/**
 * struct dma_slave_map - associates slave device and it's slave channel with
 * parameter to be used by a filter function
 * @devname: name of the device
 * @slave: slave channel name
 * @param: opaque parameter to pass to struct dma_filter.fn
 */
struct dma_slave_map {
	const char *devname;
	const char *slave;
	void *param;
};

/**
 * struct dma_filter - information for slave device/channel to filter_fn/param
 * mapping
 * @fn: filter function callback
 * @mapcnt: number of slave device/channel in the map
 * @map: array of channel to filter mapping data
 */
struct dma_filter {
	dma_filter_fn fn;
	int mapcnt;
	const struct dma_slave_map *map;
};

/**
 * struct dma_device - info on the entity supplying DMA services
 * @chancnt: how many DMA channels are supported
 * @privatecnt: how many DMA channels are requested by dma_request_channel
 * @channels: the list of struct dma_chan
 * @global_node: list_head for global dma_device_list
 * @filter: information for device/slave to filter function/param mapping
 * @cap_mask: one or more dma_capability flags
 * @desc_metadata_modes: supported metadata modes by the DMA device
 * @max_xor: maximum number of xor sources, 0 if no capability
 * @max_pq: maximum number of PQ sources and PQ-continue capability
 * @copy_align: alignment shift for memcpy operations
 * @xor_align: alignment shift for xor operations
 * @pq_align: alignment shift for pq operations
 * @fill_align: alignment shift for memset operations
 * @dev_id: unique device ID
 * @dev: struct device reference for dma mapping api
 * @owner: owner module (automatically set based on the provided dev)
 * @src_addr_widths: bit mask of src addr widths the device supports
 *	Width is specified in bytes, e.g. for a device supporting
 *	a width of 4 the mask should have BIT(4) set.
 * @dst_addr_widths: bit mask of dst addr widths the device supports
 * @directions: bit mask of slave directions the device supports.
 *	Since the enum dma_transfer_direction is not defined as bit flag for
 *	each type, the dma controller should set BIT(<TYPE>) and same
 *	should be checked by controller as well
 * @min_burst: min burst capability per-transfer
 * @max_burst: max burst capability per-transfer
 * @max_sg_burst: max number of SG list entries executed in a single burst
 *	DMA tansaction with no software intervention for reinitialization.
 *	Zero value means unlimited number of entries.
 * @residue_granularity: granularity of the transfer residue reported
 *	by tx_status
 * @device_alloc_chan_resources: allocate resources and return the
 *	number of allocated descriptors
 * @device_router_config: optional callback for DMA router configuration
 * @device_free_chan_resources: release DMA channel's resources
 * @device_prep_dma_memcpy: prepares a memcpy operation
 * @device_prep_dma_xor: prepares a xor operation
 * @device_prep_dma_xor_val: prepares a xor validation operation
 * @device_prep_dma_pq: prepares a pq operation
 * @device_prep_dma_pq_val: prepares a pqzero_sum operation
 * @device_prep_dma_memset: prepares a memset operation
 * @device_prep_dma_memset_sg: prepares a memset operation over a scatter list
 * @device_prep_dma_interrupt: prepares an end of chain interrupt operation
 * @device_prep_slave_sg: prepares a slave dma operation
 * @device_prep_dma_cyclic: prepare a cyclic dma operation suitable for audio.
 *	The function takes a buffer of size buf_len. The callback function will
 *	be called after period_len bytes have been transferred.
 * @device_prep_interleaved_dma: Transfer expression in a generic way.
 * @device_prep_dma_imm_data: DMA's 8 byte immediate data to the dst address
 * @device_caps: May be used to override the generic DMA slave capabilities
 *	with per-channel specific ones
 * @device_config: Pushes a new configuration to a channel, return 0 or an error
 *	code
 * @device_pause: Pauses any transfer happening on a channel. Returns
 *	0 or an error code
 * @device_resume: Resumes any transfer on a channel previously
 *	paused. Returns 0 or an error code
 * @device_terminate_all: Aborts all transfers on a channel. Returns 0
 *	or an error code
 * @device_synchronize: Synchronizes the termination of a transfers to the
 *  current context.
 * @device_tx_status: poll for transaction completion, the optional
 *	txstate parameter can be supplied with a pointer to get a
 *	struct with auxiliary transfer status information, otherwise the call
 *	will just return a simple status code
 * @device_issue_pending: push pending transactions to hardware
 * @descriptor_reuse: a submitted transfer can be resubmitted after completion
 * @device_release: called sometime atfer dma_async_device_unregister() is
 *     called and there are no further references to this structure. This
 *     must be implemented to free resources however many existing drivers
 *     do not and are therefore not safe to unbind while in use.
 * @dbg_summary_show: optional routine to show contents in debugfs; default code
 *     will be used when this is omitted, but custom code can show extra,
 *     controller specific information.
 */
struct dma_device {
	struct kref ref;
	unsigned int chancnt;
	unsigned int privatecnt;
	struct list_head channels;
	struct list_head global_node;
	struct dma_filter filter;
	dma_cap_mask_t  cap_mask;
	enum dma_desc_metadata_mode desc_metadata_modes;
	unsigned short max_xor;
	unsigned short max_pq;
	enum dmaengine_alignment copy_align;
	enum dmaengine_alignment xor_align;
	enum dmaengine_alignment pq_align;
	enum dmaengine_alignment fill_align;
	#define DMA_HAS_PQ_CONTINUE (1 << 15)

	int dev_id;
	struct device *dev;
	struct module *owner;
	struct ida chan_ida;
	struct mutex chan_mutex;	/* to protect chan_ida */

	u32 src_addr_widths;
	u32 dst_addr_widths;
	u32 directions;
	u32 min_burst;
	u32 max_burst;
	u32 max_sg_burst;
	bool descriptor_reuse;
	enum dma_residue_granularity residue_granularity;

	int (*device_alloc_chan_resources)(struct dma_chan *chan);
	int (*device_router_config)(struct dma_chan *chan);
	void (*device_free_chan_resources)(struct dma_chan *chan);

	struct dma_async_tx_descriptor *(*device_prep_dma_memcpy)(
		struct dma_chan *chan, dma_addr_t dst, dma_addr_t src,
		size_t len, unsigned long flags);
	struct dma_async_tx_descriptor *(*device_prep_dma_xor)(
		struct dma_chan *chan, dma_addr_t dst, dma_addr_t *src,
		unsigned int src_cnt, size_t len, unsigned long flags);
	struct dma_async_tx_descriptor *(*device_prep_dma_xor_val)(
		struct dma_chan *chan, dma_addr_t *src,	unsigned int src_cnt,
		size_t len, enum sum_check_flags *result, unsigned long flags);
	struct dma_async_tx_descriptor *(*device_prep_dma_pq)(
		struct dma_chan *chan, dma_addr_t *dst, dma_addr_t *src,
		unsigned int src_cnt, const unsigned char *scf,
		size_t len, unsigned long flags);
	struct dma_async_tx_descriptor *(*device_prep_dma_pq_val)(
		struct dma_chan *chan, dma_addr_t *pq, dma_addr_t *src,
		unsigned int src_cnt, const unsigned char *scf, size_t len,
		enum sum_check_flags *pqres, unsigned long flags);
	struct dma_async_tx_descriptor *(*device_prep_dma_memset)(
		struct dma_chan *chan, dma_addr_t dest, int value, size_t len,
		unsigned long flags);
	struct dma_async_tx_descriptor *(*device_prep_dma_memset_sg)(
		struct dma_chan *chan, struct scatterlist *sg,
		unsigned int nents, int value, unsigned long flags);
	struct dma_async_tx_descriptor *(*device_prep_dma_interrupt)(
		struct dma_chan *chan, unsigned long flags);

	struct dma_async_tx_descriptor *(*device_prep_slave_sg)(
		struct dma_chan *chan, struct scatterlist *sgl,
		unsigned int sg_len, enum dma_transfer_direction direction,
		unsigned long flags, void *context);
	struct dma_async_tx_descriptor *(*device_prep_dma_cyclic)(
		struct dma_chan *chan, dma_addr_t buf_addr, size_t buf_len,
		size_t period_len, enum dma_transfer_direction direction,
		unsigned long flags);
	struct dma_async_tx_descriptor *(*device_prep_interleaved_dma)(
		struct dma_chan *chan, struct dma_interleaved_template *xt,
		unsigned long flags);
	struct dma_async_tx_descriptor *(*device_prep_dma_imm_data)(
		struct dma_chan *chan, dma_addr_t dst, u64 data,
		unsigned long flags);

	void (*device_caps)(struct dma_chan *chan,
			    struct dma_slave_caps *caps);
	int (*device_config)(struct dma_chan *chan,
			     struct dma_slave_config *config);
	int (*device_pause)(struct dma_chan *chan);
	int (*device_resume)(struct dma_chan *chan);
	int (*device_terminate_all)(struct dma_chan *chan);
	void (*device_synchronize)(struct dma_chan *chan);

	enum dma_status (*device_tx_status)(struct dma_chan *chan,
					    dma_cookie_t cookie,
					    struct dma_tx_state *txstate);
	void (*device_issue_pending)(struct dma_chan *chan);
	void (*device_release)(struct dma_device *dev);
	/* debugfs support */
#ifdef CONFIG_DEBUG_FS
	void (*dbg_summary_show)(struct seq_file *s, struct dma_device *dev);
	struct dentry *dbg_dev_root;
#endif
};

static inline int dmaengine_slave_config(struct dma_chan *chan,
					  struct dma_slave_config *config)
{
	if (chan->device->device_config)
		return chan->device->device_config(chan, config);

	return -ENOSYS;
}

static inline bool is_slave_direction(enum dma_transfer_direction direction)
{
	return (direction == DMA_MEM_TO_DEV) || (direction == DMA_DEV_TO_MEM);
}

static inline struct dma_async_tx_descriptor *dmaengine_prep_slave_single(
	struct dma_chan *chan, dma_addr_t buf, size_t len,
	enum dma_transfer_direction dir, unsigned long flags)
{
	struct scatterlist sg;
	sg_init_table(&sg, 1);
	sg_dma_address(&sg) = buf;
	sg_dma_len(&sg) = len;

	if (!chan || !chan->device || !chan->device->device_prep_slave_sg)
		return NULL;

	return chan->device->device_prep_slave_sg(chan, &sg, 1,
						  dir, flags, NULL);
}

static inline struct dma_async_tx_descriptor *dmaengine_prep_slave_sg(
	struct dma_chan *chan, struct scatterlist *sgl,	unsigned int sg_len,
	enum dma_transfer_direction dir, unsigned long flags)
{
	if (!chan || !chan->device || !chan->device->device_prep_slave_sg)
		return NULL;

	return chan->device->device_prep_slave_sg(chan, sgl, sg_len,
						  dir, flags, NULL);
}

#ifdef CONFIG_RAPIDIO_DMA_ENGINE
struct rio_dma_ext;
static inline struct dma_async_tx_descriptor *dmaengine_prep_rio_sg(
	struct dma_chan *chan, struct scatterlist *sgl,	unsigned int sg_len,
	enum dma_transfer_direction dir, unsigned long flags,
	struct rio_dma_ext *rio_ext)
{
	if (!chan || !chan->device || !chan->device->device_prep_slave_sg)
		return NULL;

	return chan->device->device_prep_slave_sg(chan, sgl, sg_len,
						  dir, flags, rio_ext);
}
#endif

static inline struct dma_async_tx_descriptor *dmaengine_prep_dma_cyclic(
		struct dma_chan *chan, dma_addr_t buf_addr, size_t buf_len,
		size_t period_len, enum dma_transfer_direction dir,
		unsigned long flags)
{
	if (!chan || !chan->device || !chan->device->device_prep_dma_cyclic)
		return NULL;

	return chan->device->device_prep_dma_cyclic(chan, buf_addr, buf_len,
						period_len, dir, flags);
}

static inline struct dma_async_tx_descriptor *dmaengine_prep_interleaved_dma(
		struct dma_chan *chan, struct dma_interleaved_template *xt,
		unsigned long flags)
{
	if (!chan || !chan->device || !chan->device->device_prep_interleaved_dma)
		return NULL;
	if (flags & DMA_PREP_REPEAT &&
	    !test_bit(DMA_REPEAT, chan->device->cap_mask.bits))
		return NULL;

	return chan->device->device_prep_interleaved_dma(chan, xt, flags);
}

static inline struct dma_async_tx_descriptor *dmaengine_prep_dma_memset(
		struct dma_chan *chan, dma_addr_t dest, int value, size_t len,
		unsigned long flags)
{
	if (!chan || !chan->device || !chan->device->device_prep_dma_memset)
		return NULL;

	return chan->device->device_prep_dma_memset(chan, dest, value,
						    len, flags);
}

static inline struct dma_async_tx_descriptor *dmaengine_prep_dma_memcpy(
		struct dma_chan *chan, dma_addr_t dest, dma_addr_t src,
		size_t len, unsigned long flags)
{
	if (!chan || !chan->device || !chan->device->device_prep_dma_memcpy)
		return NULL;

	return chan->device->device_prep_dma_memcpy(chan, dest, src,
						    len, flags);
}

static inline bool dmaengine_is_metadata_mode_supported(struct dma_chan *chan,
		enum dma_desc_metadata_mode mode)
{
	if (!chan)
		return false;

	return !!(chan->device->desc_metadata_modes & mode);
}

#ifdef CONFIG_DMA_ENGINE
int dmaengine_desc_attach_metadata(struct dma_async_tx_descriptor *desc,
				   void *data, size_t len);
void *dmaengine_desc_get_metadata_ptr(struct dma_async_tx_descriptor *desc,
				      size_t *payload_len, size_t *max_len);
int dmaengine_desc_set_metadata_len(struct dma_async_tx_descriptor *desc,
				    size_t payload_len);
#else /* CONFIG_DMA_ENGINE */
static inline int dmaengine_desc_attach_metadata(
		struct dma_async_tx_descriptor *desc, void *data, size_t len)
{
	return -EINVAL;
}
static inline void *dmaengine_desc_get_metadata_ptr(
		struct dma_async_tx_descriptor *desc, size_t *payload_len,
		size_t *max_len)
{
	return NULL;
}
static inline int dmaengine_desc_set_metadata_len(
		struct dma_async_tx_descriptor *desc, size_t payload_len)
{
	return -EINVAL;
}
#endif /* CONFIG_DMA_ENGINE */

/**
 * dmaengine_terminate_all() - Terminate all active DMA transfers
 * @chan: The channel for which to terminate the transfers
 *
 * This function is DEPRECATED use either dmaengine_terminate_sync() or
 * dmaengine_terminate_async() instead.
 */
static inline int dmaengine_terminate_all(struct dma_chan *chan)
{
	if (chan->device->device_terminate_all)
		return chan->device->device_terminate_all(chan);

	return -ENOSYS;
}

/**
 * dmaengine_terminate_async() - Terminate all active DMA transfers
 * @chan: The channel for which to terminate the transfers
 *
 * Calling this function will terminate all active and pending descriptors
 * that have previously been submitted to the channel. It is not guaranteed
 * though that the transfer for the active descriptor has stopped when the
 * function returns. Furthermore it is possible the complete callback of a
 * submitted transfer is still running when this function returns.
 *
 * dmaengine_synchronize() needs to be called before it is safe to free
 * any memory that is accessed by previously submitted descriptors or before
 * freeing any resources accessed from within the completion callback of any
 * previously submitted descriptors.
 *
 * This function can be called from atomic context as well as from within a
 * complete callback of a descriptor submitted on the same channel.
 *
 * If none of the two conditions above apply consider using
 * dmaengine_terminate_sync() instead.
 */
static inline int dmaengine_terminate_async(struct dma_chan *chan)
{
	if (chan->device->device_terminate_all)
		return chan->device->device_terminate_all(chan);

	return -EINVAL;
}

/**
 * dmaengine_synchronize() - Synchronize DMA channel termination
 * @chan: The channel to synchronize
 *
 * Synchronizes to the DMA channel termination to the current context. When this
 * function returns it is guaranteed that all transfers for previously issued
 * descriptors have stopped and it is safe to free the memory associated
 * with them. Furthermore it is guaranteed that all complete callback functions
 * for a previously submitted descriptor have finished running and it is safe to
 * free resources accessed from within the complete callbacks.
 *
 * The behavior of this function is undefined if dma_async_issue_pending() has
 * been called between dmaengine_terminate_async() and this function.
 *
 * This function must only be called from non-atomic context and must not be
 * called from within a complete callback of a descriptor submitted on the same
 * channel.
 */
static inline void dmaengine_synchronize(struct dma_chan *chan)
{
	might_sleep();

	if (chan->device->device_synchronize)
		chan->device->device_synchronize(chan);
}

/**
 * dmaengine_terminate_sync() - Terminate all active DMA transfers
 * @chan: The channel for which to terminate the transfers
 *
 * Calling this function will terminate all active and pending transfers
 * that have previously been submitted to the channel. It is similar to
 * dmaengine_terminate_async() but guarantees that the DMA transfer has actually
 * stopped and that all complete callbacks have finished running when the
 * function returns.
 *
 * This function must only be called from non-atomic context and must not be
 * called from within a complete callback of a descriptor submitted on the same
 * channel.
 */
static inline int dmaengine_terminate_sync(struct dma_chan *chan)
{
	int ret;

	ret = dmaengine_terminate_async(chan);
	if (ret)
		return ret;

	dmaengine_synchronize(chan);

	return 0;
}

static inline int dmaengine_pause(struct dma_chan *chan)
{
	if (chan->device->device_pause)
		return chan->device->device_pause(chan);

	return -ENOSYS;
}

static inline int dmaengine_resume(struct dma_chan *chan)
{
	if (chan->device->device_resume)
		return chan->device->device_resume(chan);

	return -ENOSYS;
}

static inline enum dma_status dmaengine_tx_status(struct dma_chan *chan,
	dma_cookie_t cookie, struct dma_tx_state *state)
{
	return chan->device->device_tx_status(chan, cookie, state);
}

static inline dma_cookie_t dmaengine_submit(struct dma_async_tx_descriptor *desc)
{
	return desc->tx_submit(desc);
}

static inline bool dmaengine_check_align(enum dmaengine_alignment align,
					 size_t off1, size_t off2, size_t len)
{
	return !(((1 << align) - 1) & (off1 | off2 | len));
}

static inline bool is_dma_copy_aligned(struct dma_device *dev, size_t off1,
				       size_t off2, size_t len)
{
	return dmaengine_check_align(dev->copy_align, off1, off2, len);
}

static inline bool is_dma_xor_aligned(struct dma_device *dev, size_t off1,
				      size_t off2, size_t len)
{
	return dmaengine_check_align(dev->xor_align, off1, off2, len);
}

static inline bool is_dma_pq_aligned(struct dma_device *dev, size_t off1,
				     size_t off2, size_t len)
{
	return dmaengine_check_align(dev->pq_align, off1, off2, len);
}

static inline bool is_dma_fill_aligned(struct dma_device *dev, size_t off1,
				       size_t off2, size_t len)
{
	return dmaengine_check_align(dev->fill_align, off1, off2, len);
}

static inline void
dma_set_maxpq(struct dma_device *dma, int maxpq, int has_pq_continue)
{
	dma->max_pq = maxpq;
	if (has_pq_continue)
		dma->max_pq |= DMA_HAS_PQ_CONTINUE;
}

static inline bool dmaf_continue(enum dma_ctrl_flags flags)
{
	return (flags & DMA_PREP_CONTINUE) == DMA_PREP_CONTINUE;
}

static inline bool dmaf_p_disabled_continue(enum dma_ctrl_flags flags)
{
	enum dma_ctrl_flags mask = DMA_PREP_CONTINUE | DMA_PREP_PQ_DISABLE_P;

	return (flags & mask) == mask;
}

static inline bool dma_dev_has_pq_continue(struct dma_device *dma)
{
	return (dma->max_pq & DMA_HAS_PQ_CONTINUE) == DMA_HAS_PQ_CONTINUE;
}

static inline unsigned short dma_dev_to_maxpq(struct dma_device *dma)
{
	return dma->max_pq & ~DMA_HAS_PQ_CONTINUE;
}

/* dma_maxpq - reduce maxpq in the face of continued operations
 * @dma - dma device with PQ capability
 * @flags - to check if DMA_PREP_CONTINUE and DMA_PREP_PQ_DISABLE_P are set
 *
 * When an engine does not support native continuation we need 3 extra
 * source slots to reuse P and Q with the following coefficients:
 * 1/ {00} * P : remove P from Q', but use it as a source for P'
 * 2/ {01} * Q : use Q to continue Q' calculation
 * 3/ {00} * Q : subtract Q from P' to cancel (2)
 *
 * In the case where P is disabled we only need 1 extra source:
 * 1/ {01} * Q : use Q to continue Q' calculation
 */
static inline int dma_maxpq(struct dma_device *dma, enum dma_ctrl_flags flags)
{
	if (dma_dev_has_pq_continue(dma) || !dmaf_continue(flags))
		return dma_dev_to_maxpq(dma);
	if (dmaf_p_disabled_continue(flags))
		return dma_dev_to_maxpq(dma) - 1;
	if (dmaf_continue(flags))
		return dma_dev_to_maxpq(dma) - 3;
	BUG();
}

static inline size_t dmaengine_get_icg(bool inc, bool sgl, size_t icg,
				      size_t dir_icg)
{
	if (inc) {
		if (dir_icg)
			return dir_icg;
		if (sgl)
			return icg;
	}

	return 0;
}

static inline size_t dmaengine_get_dst_icg(struct dma_interleaved_template *xt,
					   struct data_chunk *chunk)
{
	return dmaengine_get_icg(xt->dst_inc, xt->dst_sgl,
				 chunk->icg, chunk->dst_icg);
}

static inline size_t dmaengine_get_src_icg(struct dma_interleaved_template *xt,
					   struct data_chunk *chunk)
{
	return dmaengine_get_icg(xt->src_inc, xt->src_sgl,
				 chunk->icg, chunk->src_icg);
}

/* --- public DMA engine API --- */

#ifdef CONFIG_DMA_ENGINE
void dmaengine_get(void);
void dmaengine_put(void);
#else
static inline void dmaengine_get(void)
{
}
static inline void dmaengine_put(void)
{
}
#endif

#ifdef CONFIG_ASYNC_TX_DMA
#define async_dmaengine_get()	dmaengine_get()
#define async_dmaengine_put()	dmaengine_put()
#ifndef CONFIG_ASYNC_TX_ENABLE_CHANNEL_SWITCH
#define async_dma_find_channel(type) dma_find_channel(DMA_ASYNC_TX)
#else
#define async_dma_find_channel(type) dma_find_channel(type)
#endif /* CONFIG_ASYNC_TX_ENABLE_CHANNEL_SWITCH */
#else
static inline void async_dmaengine_get(void)
{
}
static inline void async_dmaengine_put(void)
{
}
static inline struct dma_chan *
async_dma_find_channel(enum dma_transaction_type type)
{
	return NULL;
}
#endif /* CONFIG_ASYNC_TX_DMA */
void dma_async_tx_descriptor_init(struct dma_async_tx_descriptor *tx,
				  struct dma_chan *chan);

static inline void async_tx_ack(struct dma_async_tx_descriptor *tx)
{
	tx->flags |= DMA_CTRL_ACK;
}

static inline void async_tx_clear_ack(struct dma_async_tx_descriptor *tx)
{
	tx->flags &= ~DMA_CTRL_ACK;
}

static inline bool async_tx_test_ack(struct dma_async_tx_descriptor *tx)
{
	return (tx->flags & DMA_CTRL_ACK) == DMA_CTRL_ACK;
}

#define dma_cap_set(tx, mask) __dma_cap_set((tx), &(mask))
static inline void
__dma_cap_set(enum dma_transaction_type tx_type, dma_cap_mask_t *dstp)
{
	set_bit(tx_type, dstp->bits);
}

#define dma_cap_clear(tx, mask) __dma_cap_clear((tx), &(mask))
static inline void
__dma_cap_clear(enum dma_transaction_type tx_type, dma_cap_mask_t *dstp)
{
	clear_bit(tx_type, dstp->bits);
}

#define dma_cap_zero(mask) __dma_cap_zero(&(mask))
static inline void __dma_cap_zero(dma_cap_mask_t *dstp)
{
	bitmap_zero(dstp->bits, DMA_TX_TYPE_END);
}

#define dma_has_cap(tx, mask) __dma_has_cap((tx), &(mask))
static inline int
__dma_has_cap(enum dma_transaction_type tx_type, dma_cap_mask_t *srcp)
{
	return test_bit(tx_type, srcp->bits);
}

#define for_each_dma_cap_mask(cap, mask) \
	for_each_set_bit(cap, mask.bits, DMA_TX_TYPE_END)

/**
 * dma_async_issue_pending - flush pending transactions to HW
 * @chan: target DMA channel
 *
 * This allows drivers to push copies to HW in batches,
 * reducing MMIO writes where possible.
 */
static inline void dma_async_issue_pending(struct dma_chan *chan)
{
	chan->device->device_issue_pending(chan);
}

/**
 * dma_async_is_tx_complete - poll for transaction completion
 * @chan: DMA channel
 * @cookie: transaction identifier to check status of
 * @last: returns last completed cookie, can be NULL
 * @used: returns last issued cookie, can be NULL
 *
 * If @last and @used are passed in, upon return they reflect the driver
 * internal state and can be used with dma_async_is_complete() to check
 * the status of multiple cookies without re-checking hardware state.
 */
static inline enum dma_status dma_async_is_tx_complete(struct dma_chan *chan,
	dma_cookie_t cookie, dma_cookie_t *last, dma_cookie_t *used)
{
	struct dma_tx_state state;
	enum dma_status status;

	status = chan->device->device_tx_status(chan, cookie, &state);
	if (last)
		*last = state.last;
	if (used)
		*used = state.used;
	return status;
}

/**
 * dma_async_is_complete - test a cookie against chan state
 * @cookie: transaction identifier to test status of
 * @last_complete: last know completed transaction
 * @last_used: last cookie value handed out
 *
 * dma_async_is_complete() is used in dma_async_is_tx_complete()
 * the test logic is separated for lightweight testing of multiple cookies
 */
static inline enum dma_status dma_async_is_complete(dma_cookie_t cookie,
			dma_cookie_t last_complete, dma_cookie_t last_used)
{
	if (last_complete <= last_used) {
		if ((cookie <= last_complete) || (cookie > last_used))
			return DMA_COMPLETE;
	} else {
		if ((cookie <= last_complete) && (cookie > last_used))
			return DMA_COMPLETE;
	}
	return DMA_IN_PROGRESS;
}

static inline void
dma_set_tx_state(struct dma_tx_state *st, dma_cookie_t last, dma_cookie_t used, u32 residue)
{
	if (!st)
		return;

	st->last = last;
	st->used = used;
	st->residue = residue;
}

#ifdef CONFIG_DMA_ENGINE
struct dma_chan *dma_find_channel(enum dma_transaction_type tx_type);
enum dma_status dma_sync_wait(struct dma_chan *chan, dma_cookie_t cookie);
enum dma_status dma_wait_for_async_tx(struct dma_async_tx_descriptor *tx);
void dma_issue_pending_all(void);
struct dma_chan *__dma_request_channel(const dma_cap_mask_t *mask,
				       dma_filter_fn fn, void *fn_param,
				       struct device_node *np);

struct dma_chan *dma_request_chan(struct device *dev, const char *name);
struct dma_chan *dma_request_chan_by_mask(const dma_cap_mask_t *mask);

void dma_release_channel(struct dma_chan *chan);
int dma_get_slave_caps(struct dma_chan *chan, struct dma_slave_caps *caps);
#else
static inline struct dma_chan *dma_find_channel(enum dma_transaction_type tx_type)
{
	return NULL;
}
static inline enum dma_status dma_sync_wait(struct dma_chan *chan, dma_cookie_t cookie)
{
	return DMA_COMPLETE;
}
static inline enum dma_status dma_wait_for_async_tx(struct dma_async_tx_descriptor *tx)
{
	return DMA_COMPLETE;
}
static inline void dma_issue_pending_all(void)
{
}
static inline struct dma_chan *__dma_request_channel(const dma_cap_mask_t *mask,
						     dma_filter_fn fn,
						     void *fn_param,
						     struct device_node *np)
{
	return NULL;
}
static inline struct dma_chan *dma_request_chan(struct device *dev,
						const char *name)
{
	return ERR_PTR(-ENODEV);
}
static inline struct dma_chan *dma_request_chan_by_mask(
						const dma_cap_mask_t *mask)
{
	return ERR_PTR(-ENODEV);
}
static inline void dma_release_channel(struct dma_chan *chan)
{
}
static inline int dma_get_slave_caps(struct dma_chan *chan,
				     struct dma_slave_caps *caps)
{
	return -ENXIO;
}
#endif

static inline int dmaengine_desc_set_reuse(struct dma_async_tx_descriptor *tx)
{
	struct dma_slave_caps caps;
	int ret;

	ret = dma_get_slave_caps(tx->chan, &caps);
	if (ret)
		return ret;

	if (!caps.descriptor_reuse)
		return -EPERM;

	tx->flags |= DMA_CTRL_REUSE;
	return 0;
}

static inline void dmaengine_desc_clear_reuse(struct dma_async_tx_descriptor *tx)
{
	tx->flags &= ~DMA_CTRL_REUSE;
}

static inline bool dmaengine_desc_test_reuse(struct dma_async_tx_descriptor *tx)
{
	return (tx->flags & DMA_CTRL_REUSE) == DMA_CTRL_REUSE;
}

static inline int dmaengine_desc_free(struct dma_async_tx_descriptor *desc)
{
	/* this is supported for reusable desc, so check that */
	if (!dmaengine_desc_test_reuse(desc))
		return -EPERM;

	return desc->desc_free(desc);
}

/* --- DMA device --- */

int dma_async_device_register(struct dma_device *device);
int dmaenginem_async_device_register(struct dma_device *device);
void dma_async_device_unregister(struct dma_device *device);
int dma_async_device_channel_register(struct dma_device *device,
				      struct dma_chan *chan);
void dma_async_device_channel_unregister(struct dma_device *device,
					 struct dma_chan *chan);
void dma_run_dependencies(struct dma_async_tx_descriptor *tx);
#define dma_request_channel(mask, x, y) \
	__dma_request_channel(&(mask), x, y, NULL)

/* Deprecated, please use dma_request_chan() directly */
static inline struct dma_chan * __deprecated
dma_request_slave_channel(struct device *dev, const char *name)
{
	struct dma_chan *ch = dma_request_chan(dev, name);

	return IS_ERR(ch) ? NULL : ch;
}

static inline struct dma_chan
*dma_request_slave_channel_compat(const dma_cap_mask_t mask,
				  dma_filter_fn fn, void *fn_param,
				  struct device *dev, const char *name)
{
	struct dma_chan *chan;

	chan = dma_request_slave_channel(dev, name);
	if (chan)
		return chan;

	if (!fn || !fn_param)
		return NULL;

	return __dma_request_channel(&mask, fn, fn_param, NULL);
}

static inline char *
dmaengine_get_direction_text(enum dma_transfer_direction dir)
{
	switch (dir) {
	case DMA_DEV_TO_MEM:
		return "DEV_TO_MEM";
	case DMA_MEM_TO_DEV:
		return "MEM_TO_DEV";
	case DMA_MEM_TO_MEM:
		return "MEM_TO_MEM";
	case DMA_DEV_TO_DEV:
		return "DEV_TO_DEV";
	default:
		return "invalid";
	}
}

static inline struct device *dmaengine_get_dma_device(struct dma_chan *chan)
{
	if (chan->dev->chan_dma_dev)
		return &chan->dev->device;

	return chan->device->dev;
}

#endif /* DMAENGINE_H */
