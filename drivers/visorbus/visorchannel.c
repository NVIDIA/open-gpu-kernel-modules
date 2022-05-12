// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2010 - 2015 UNISYS CORPORATION
 * All rights reserved.
 */

/*
 *  This provides s-Par channel communication primitives, which are
 *  independent of the mechanism used to access the channel data.
 */

#include <linux/uuid.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <linux/visorbus.h>

#include "visorbus_private.h"
#include "controlvmchannel.h"

#define VISOR_DRV_NAME "visorchannel"

#define VISOR_CONSOLEVIDEO_CHANNEL_GUID \
	GUID_INIT(0x3cd6e705, 0xd6a2, 0x4aa5, \
		  0xad, 0x5c, 0x7b, 0x8, 0x88, 0x9d, 0xff, 0xe2)

static const guid_t visor_video_guid = VISOR_CONSOLEVIDEO_CHANNEL_GUID;

struct visorchannel {
	u64 physaddr;
	ulong nbytes;
	void *mapped;
	bool requested;
	struct channel_header chan_hdr;
	guid_t guid;
	/*
	 * channel creator knows if more than one thread will be inserting or
	 * removing
	 */
	bool needs_lock;
	/* protect head writes in chan_hdr */
	spinlock_t insert_lock;
	/* protect tail writes in chan_hdr */
	spinlock_t remove_lock;
	guid_t type;
	guid_t inst;
};

void visorchannel_destroy(struct visorchannel *channel)
{
	if (!channel)
		return;

	if (channel->mapped) {
		memunmap(channel->mapped);
		if (channel->requested)
			release_mem_region(channel->physaddr, channel->nbytes);
	}
	kfree(channel);
}

u64 visorchannel_get_physaddr(struct visorchannel *channel)
{
	return channel->physaddr;
}

ulong visorchannel_get_nbytes(struct visorchannel *channel)
{
	return channel->nbytes;
}

char *visorchannel_guid_id(const guid_t *guid, char *s)
{
	sprintf(s, "%pUL", guid);
	return s;
}

char *visorchannel_id(struct visorchannel *channel, char *s)
{
	return visorchannel_guid_id(&channel->guid, s);
}

char *visorchannel_zoneid(struct visorchannel *channel, char *s)
{
	return visorchannel_guid_id(&channel->chan_hdr.zone_guid, s);
}

u64 visorchannel_get_clientpartition(struct visorchannel *channel)
{
	return channel->chan_hdr.partition_handle;
}

int visorchannel_set_clientpartition(struct visorchannel *channel,
				     u64 partition_handle)
{
	channel->chan_hdr.partition_handle = partition_handle;
	return 0;
}

/**
 * visorchannel_get_guid() - queries the GUID of the designated channel
 * @channel: the channel to query
 *
 * Return: the GUID of the provided channel
 */
const guid_t *visorchannel_get_guid(struct visorchannel *channel)
{
	return &channel->guid;
}
EXPORT_SYMBOL_GPL(visorchannel_get_guid);

int visorchannel_read(struct visorchannel *channel, ulong offset, void *dest,
		      ulong nbytes)
{
	if (offset + nbytes > channel->nbytes)
		return -EIO;

	memcpy(dest, channel->mapped + offset, nbytes);
	return 0;
}

int visorchannel_write(struct visorchannel *channel, ulong offset, void *dest,
		       ulong nbytes)
{
	size_t chdr_size = sizeof(struct channel_header);
	size_t copy_size;

	if (offset + nbytes > channel->nbytes)
		return -EIO;

	if (offset < chdr_size) {
		copy_size = min(chdr_size - offset, nbytes);
		memcpy(((char *)(&channel->chan_hdr)) + offset,
		       dest, copy_size);
	}
	memcpy(channel->mapped + offset, dest, nbytes);
	return 0;
}

void *visorchannel_get_header(struct visorchannel *channel)
{
	return &channel->chan_hdr;
}

/*
 * Return offset of a specific SIGNAL_QUEUE_HEADER from the beginning of a
 * channel header
 */
static int sig_queue_offset(struct channel_header *chan_hdr, int q)
{
	return ((chan_hdr)->ch_space_offset +
	       ((q) * sizeof(struct signal_queue_header)));
}

/*
 * Return offset of a specific queue entry (data) from the beginning of a
 * channel header
 */
static int sig_data_offset(struct channel_header *chan_hdr, int q,
			   struct signal_queue_header *sig_hdr, int slot)
{
	return (sig_queue_offset(chan_hdr, q) + sig_hdr->sig_base_offset +
	       (slot * sig_hdr->signal_size));
}

/*
 * Write the contents of a specific field within a SIGNAL_QUEUE_HEADER back into
 * host memory
 */
#define SIG_WRITE_FIELD(channel, queue, sig_hdr, FIELD) \
	visorchannel_write(channel, \
			   sig_queue_offset(&channel->chan_hdr, queue) + \
			   offsetof(struct signal_queue_header, FIELD), \
			   &((sig_hdr)->FIELD), \
			   sizeof((sig_hdr)->FIELD))

static int sig_read_header(struct visorchannel *channel, u32 queue,
			   struct signal_queue_header *sig_hdr)
{
	if (channel->chan_hdr.ch_space_offset < sizeof(struct channel_header))
		return -EINVAL;

	/* Read the appropriate SIGNAL_QUEUE_HEADER into local memory. */
	return visorchannel_read(channel,
				 sig_queue_offset(&channel->chan_hdr, queue),
				 sig_hdr, sizeof(struct signal_queue_header));
}

static int sig_read_data(struct visorchannel *channel, u32 queue,
			 struct signal_queue_header *sig_hdr, u32 slot,
			 void *data)
{
	int signal_data_offset = sig_data_offset(&channel->chan_hdr, queue,
						 sig_hdr, slot);

	return visorchannel_read(channel, signal_data_offset,
				 data, sig_hdr->signal_size);
}

static int sig_write_data(struct visorchannel *channel, u32 queue,
			  struct signal_queue_header *sig_hdr, u32 slot,
			  void *data)
{
	int signal_data_offset = sig_data_offset(&channel->chan_hdr, queue,
						 sig_hdr, slot);

	return visorchannel_write(channel, signal_data_offset,
				  data, sig_hdr->signal_size);
}

static int signalremove_inner(struct visorchannel *channel, u32 queue,
			      void *msg)
{
	struct signal_queue_header sig_hdr;
	int error;

	error = sig_read_header(channel, queue, &sig_hdr);
	if (error)
		return error;
	/* No signals to remove; have caller try again. */
	if (sig_hdr.head == sig_hdr.tail)
		return -EAGAIN;
	sig_hdr.tail = (sig_hdr.tail + 1) % sig_hdr.max_slots;
	error = sig_read_data(channel, queue, &sig_hdr, sig_hdr.tail, msg);
	if (error)
		return error;
	sig_hdr.num_received++;
	/*
	 * For each data field in SIGNAL_QUEUE_HEADER that was modified, update
	 * host memory. Required for channel sync.
	 */
	mb();
	error = SIG_WRITE_FIELD(channel, queue, &sig_hdr, tail);
	if (error)
		return error;
	error = SIG_WRITE_FIELD(channel, queue, &sig_hdr, num_received);
	if (error)
		return error;
	return 0;
}

/**
 * visorchannel_signalremove() - removes a message from the designated
 *                               channel/queue
 * @channel: the channel the message will be removed from
 * @queue:   the queue the message will be removed from
 * @msg:     the message to remove
 *
 * Return: integer error code indicating the status of the removal
 */
int visorchannel_signalremove(struct visorchannel *channel, u32 queue,
			      void *msg)
{
	int rc;
	unsigned long flags;

	if (channel->needs_lock) {
		spin_lock_irqsave(&channel->remove_lock, flags);
		rc = signalremove_inner(channel, queue, msg);
		spin_unlock_irqrestore(&channel->remove_lock, flags);
	} else {
		rc = signalremove_inner(channel, queue, msg);
	}

	return rc;
}
EXPORT_SYMBOL_GPL(visorchannel_signalremove);

static bool queue_empty(struct visorchannel *channel, u32 queue)
{
	struct signal_queue_header sig_hdr;

	if (sig_read_header(channel, queue, &sig_hdr))
		return true;
	return (sig_hdr.head == sig_hdr.tail);
}

/**
 * visorchannel_signalempty() - checks if the designated channel/queue contains
 *				any messages
 * @channel: the channel to query
 * @queue:   the queue in the channel to query
 *
 * Return: boolean indicating whether any messages in the designated
 *         channel/queue are present
 */
bool visorchannel_signalempty(struct visorchannel *channel, u32 queue)
{
	bool rc;
	unsigned long flags;

	if (!channel->needs_lock)
		return queue_empty(channel, queue);
	spin_lock_irqsave(&channel->remove_lock, flags);
	rc = queue_empty(channel, queue);
	spin_unlock_irqrestore(&channel->remove_lock, flags);
	return rc;
}
EXPORT_SYMBOL_GPL(visorchannel_signalempty);

static int signalinsert_inner(struct visorchannel *channel, u32 queue,
			      void *msg)
{
	struct signal_queue_header sig_hdr;
	int err;

	err = sig_read_header(channel, queue, &sig_hdr);
	if (err)
		return err;
	sig_hdr.head = (sig_hdr.head + 1) % sig_hdr.max_slots;
	if (sig_hdr.head == sig_hdr.tail) {
		sig_hdr.num_overflows++;
		err = SIG_WRITE_FIELD(channel, queue, &sig_hdr, num_overflows);
		if (err)
			return err;
		return -EIO;
	}
	err = sig_write_data(channel, queue, &sig_hdr, sig_hdr.head, msg);
	if (err)
		return err;
	sig_hdr.num_sent++;
	/*
	 * For each data field in SIGNAL_QUEUE_HEADER that was modified, update
	 * host memory. Required for channel sync.
	 */
	mb();
	err = SIG_WRITE_FIELD(channel, queue, &sig_hdr, head);
	if (err)
		return err;
	err = SIG_WRITE_FIELD(channel, queue, &sig_hdr, num_sent);
	if (err)
		return err;
	return 0;
}

/*
 * visorchannel_create() - creates the struct visorchannel abstraction for a
 *                         data area in memory, but does NOT modify this data
 *                         area
 * @physaddr:      physical address of start of channel
 * @gfp:           gfp_t to use when allocating memory for the data struct
 * @guid:          GUID that identifies channel type;
 * @needs_lock:    must specify true if you have multiple threads of execution
 *                 that will be calling visorchannel methods of this
 *                 visorchannel at the same time
 *
 * Return: pointer to visorchannel that was created if successful,
 *         otherwise NULL
 */
struct visorchannel *visorchannel_create(u64 physaddr, gfp_t gfp,
					 const guid_t *guid, bool needs_lock)
{
	struct visorchannel *channel;
	int err;
	size_t size = sizeof(struct channel_header);

	if (physaddr == 0)
		return NULL;

	channel = kzalloc(sizeof(*channel), gfp);
	if (!channel)
		return NULL;
	channel->needs_lock = needs_lock;
	spin_lock_init(&channel->insert_lock);
	spin_lock_init(&channel->remove_lock);
	/*
	 * Video driver constains the efi framebuffer so it will get a conflict
	 * resource when requesting its full mem region. Since we are only
	 * using the efi framebuffer for video we can ignore this. Remember that
	 * we haven't requested it so we don't try to release later on.
	 */
	channel->requested = request_mem_region(physaddr, size, VISOR_DRV_NAME);
	if (!channel->requested && !guid_equal(guid, &visor_video_guid))
		/* we only care about errors if this is not the video channel */
		goto err_destroy_channel;
	channel->mapped = memremap(physaddr, size, MEMREMAP_WB);
	if (!channel->mapped) {
		release_mem_region(physaddr, size);
		goto err_destroy_channel;
	}
	channel->physaddr = physaddr;
	channel->nbytes = size;
	err = visorchannel_read(channel, 0, &channel->chan_hdr, size);
	if (err)
		goto err_destroy_channel;
	size = (ulong)channel->chan_hdr.size;
	memunmap(channel->mapped);
	if (channel->requested)
		release_mem_region(channel->physaddr, channel->nbytes);
	channel->mapped = NULL;
	channel->requested = request_mem_region(channel->physaddr, size,
						VISOR_DRV_NAME);
	if (!channel->requested && !guid_equal(guid, &visor_video_guid))
		/* we only care about errors if this is not the video channel */
		goto err_destroy_channel;
	channel->mapped = memremap(channel->physaddr, size, MEMREMAP_WB);
	if (!channel->mapped) {
		release_mem_region(channel->physaddr, size);
		goto err_destroy_channel;
	}
	channel->nbytes = size;
	guid_copy(&channel->guid, guid);
	return channel;

err_destroy_channel:
	visorchannel_destroy(channel);
	return NULL;
}

/**
 * visorchannel_signalinsert() - inserts a message into the designated
 *                               channel/queue
 * @channel: the channel the message will be added to
 * @queue:   the queue the message will be added to
 * @msg:     the message to insert
 *
 * Return: integer error code indicating the status of the insertion
 */
int visorchannel_signalinsert(struct visorchannel *channel, u32 queue,
			      void *msg)
{
	int rc;
	unsigned long flags;

	if (channel->needs_lock) {
		spin_lock_irqsave(&channel->insert_lock, flags);
		rc = signalinsert_inner(channel, queue, msg);
		spin_unlock_irqrestore(&channel->insert_lock, flags);
	} else {
		rc = signalinsert_inner(channel, queue, msg);
	}

	return rc;
}
EXPORT_SYMBOL_GPL(visorchannel_signalinsert);
