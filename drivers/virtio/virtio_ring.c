// SPDX-License-Identifier: GPL-2.0-or-later
/* Virtio ring implementation.
 *
 *  Copyright 2007 Rusty Russell IBM Corporation
 */
#include <linux/virtio.h>
#include <linux/virtio_ring.h>
#include <linux/virtio_config.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/hrtimer.h>
#include <linux/dma-mapping.h>
#include <xen/xen.h>

#ifdef DEBUG
/* For development, we want to crash whenever the ring is screwed. */
#define BAD_RING(_vq, fmt, args...)				\
	do {							\
		dev_err(&(_vq)->vq.vdev->dev,			\
			"%s:"fmt, (_vq)->vq.name, ##args);	\
		BUG();						\
	} while (0)
/* Caller is supposed to guarantee no reentry. */
#define START_USE(_vq)						\
	do {							\
		if ((_vq)->in_use)				\
			panic("%s:in_use = %i\n",		\
			      (_vq)->vq.name, (_vq)->in_use);	\
		(_vq)->in_use = __LINE__;			\
	} while (0)
#define END_USE(_vq) \
	do { BUG_ON(!(_vq)->in_use); (_vq)->in_use = 0; } while(0)
#define LAST_ADD_TIME_UPDATE(_vq)				\
	do {							\
		ktime_t now = ktime_get();			\
								\
		/* No kick or get, with .1 second between?  Warn. */ \
		if ((_vq)->last_add_time_valid)			\
			WARN_ON(ktime_to_ms(ktime_sub(now,	\
				(_vq)->last_add_time)) > 100);	\
		(_vq)->last_add_time = now;			\
		(_vq)->last_add_time_valid = true;		\
	} while (0)
#define LAST_ADD_TIME_CHECK(_vq)				\
	do {							\
		if ((_vq)->last_add_time_valid) {		\
			WARN_ON(ktime_to_ms(ktime_sub(ktime_get(), \
				      (_vq)->last_add_time)) > 100); \
		}						\
	} while (0)
#define LAST_ADD_TIME_INVALID(_vq)				\
	((_vq)->last_add_time_valid = false)
#else
#define BAD_RING(_vq, fmt, args...)				\
	do {							\
		dev_err(&_vq->vq.vdev->dev,			\
			"%s:"fmt, (_vq)->vq.name, ##args);	\
		(_vq)->broken = true;				\
	} while (0)
#define START_USE(vq)
#define END_USE(vq)
#define LAST_ADD_TIME_UPDATE(vq)
#define LAST_ADD_TIME_CHECK(vq)
#define LAST_ADD_TIME_INVALID(vq)
#endif

struct vring_desc_state_split {
	void *data;			/* Data for callback. */
	struct vring_desc *indir_desc;	/* Indirect descriptor, if any. */
};

struct vring_desc_state_packed {
	void *data;			/* Data for callback. */
	struct vring_packed_desc *indir_desc; /* Indirect descriptor, if any. */
	u16 num;			/* Descriptor list length. */
	u16 next;			/* The next desc state in a list. */
	u16 last;			/* The last desc state in a list. */
};

struct vring_desc_extra_packed {
	dma_addr_t addr;		/* Buffer DMA addr. */
	u32 len;			/* Buffer length. */
	u16 flags;			/* Descriptor flags. */
};

struct vring_virtqueue {
	struct virtqueue vq;

	/* Is this a packed ring? */
	bool packed_ring;

	/* Is DMA API used? */
	bool use_dma_api;

	/* Can we use weak barriers? */
	bool weak_barriers;

	/* Other side has made a mess, don't try any more. */
	bool broken;

	/* Host supports indirect buffers */
	bool indirect;

	/* Host publishes avail event idx */
	bool event;

	/* Head of free buffer list. */
	unsigned int free_head;
	/* Number we've added since last sync. */
	unsigned int num_added;

	/* Last used index we've seen. */
	u16 last_used_idx;

	union {
		/* Available for split ring */
		struct {
			/* Actual memory layout for this queue. */
			struct vring vring;

			/* Last written value to avail->flags */
			u16 avail_flags_shadow;

			/*
			 * Last written value to avail->idx in
			 * guest byte order.
			 */
			u16 avail_idx_shadow;

			/* Per-descriptor state. */
			struct vring_desc_state_split *desc_state;

			/* DMA address and size information */
			dma_addr_t queue_dma_addr;
			size_t queue_size_in_bytes;
		} split;

		/* Available for packed ring */
		struct {
			/* Actual memory layout for this queue. */
			struct {
				unsigned int num;
				struct vring_packed_desc *desc;
				struct vring_packed_desc_event *driver;
				struct vring_packed_desc_event *device;
			} vring;

			/* Driver ring wrap counter. */
			bool avail_wrap_counter;

			/* Device ring wrap counter. */
			bool used_wrap_counter;

			/* Avail used flags. */
			u16 avail_used_flags;

			/* Index of the next avail descriptor. */
			u16 next_avail_idx;

			/*
			 * Last written value to driver->flags in
			 * guest byte order.
			 */
			u16 event_flags_shadow;

			/* Per-descriptor state. */
			struct vring_desc_state_packed *desc_state;
			struct vring_desc_extra_packed *desc_extra;

			/* DMA address and size information */
			dma_addr_t ring_dma_addr;
			dma_addr_t driver_event_dma_addr;
			dma_addr_t device_event_dma_addr;
			size_t ring_size_in_bytes;
			size_t event_size_in_bytes;
		} packed;
	};

	/* How to notify other side. FIXME: commonalize hcalls! */
	bool (*notify)(struct virtqueue *vq);

	/* DMA, allocation, and size information */
	bool we_own_ring;

#ifdef DEBUG
	/* They're supposed to lock for us. */
	unsigned int in_use;

	/* Figure out if their kicks are too delayed. */
	bool last_add_time_valid;
	ktime_t last_add_time;
#endif
};


/*
 * Helpers.
 */

#define to_vvq(_vq) container_of(_vq, struct vring_virtqueue, vq)

static inline bool virtqueue_use_indirect(struct virtqueue *_vq,
					  unsigned int total_sg)
{
	struct vring_virtqueue *vq = to_vvq(_vq);

	/*
	 * If the host supports indirect descriptor tables, and we have multiple
	 * buffers, then go indirect. FIXME: tune this threshold
	 */
	return (vq->indirect && total_sg > 1 && vq->vq.num_free);
}

/*
 * Modern virtio devices have feature bits to specify whether they need a
 * quirk and bypass the IOMMU. If not there, just use the DMA API.
 *
 * If there, the interaction between virtio and DMA API is messy.
 *
 * On most systems with virtio, physical addresses match bus addresses,
 * and it doesn't particularly matter whether we use the DMA API.
 *
 * On some systems, including Xen and any system with a physical device
 * that speaks virtio behind a physical IOMMU, we must use the DMA API
 * for virtio DMA to work at all.
 *
 * On other systems, including SPARC and PPC64, virtio-pci devices are
 * enumerated as though they are behind an IOMMU, but the virtio host
 * ignores the IOMMU, so we must either pretend that the IOMMU isn't
 * there or somehow map everything as the identity.
 *
 * For the time being, we preserve historic behavior and bypass the DMA
 * API.
 *
 * TODO: install a per-device DMA ops structure that does the right thing
 * taking into account all the above quirks, and use the DMA API
 * unconditionally on data path.
 */

static bool vring_use_dma_api(struct virtio_device *vdev)
{
	if (!virtio_has_dma_quirk(vdev))
		return true;

	/* Otherwise, we are left to guess. */
	/*
	 * In theory, it's possible to have a buggy QEMU-supposed
	 * emulated Q35 IOMMU and Xen enabled at the same time.  On
	 * such a configuration, virtio has never worked and will
	 * not work without an even larger kludge.  Instead, enable
	 * the DMA API if we're a Xen guest, which at least allows
	 * all of the sensible Xen configurations to work correctly.
	 */
	if (xen_domain())
		return true;

	return false;
}

size_t virtio_max_dma_size(struct virtio_device *vdev)
{
	size_t max_segment_size = SIZE_MAX;

	if (vring_use_dma_api(vdev))
		max_segment_size = dma_max_mapping_size(&vdev->dev);

	return max_segment_size;
}
EXPORT_SYMBOL_GPL(virtio_max_dma_size);

static void *vring_alloc_queue(struct virtio_device *vdev, size_t size,
			      dma_addr_t *dma_handle, gfp_t flag)
{
	if (vring_use_dma_api(vdev)) {
		return dma_alloc_coherent(vdev->dev.parent, size,
					  dma_handle, flag);
	} else {
		void *queue = alloc_pages_exact(PAGE_ALIGN(size), flag);

		if (queue) {
			phys_addr_t phys_addr = virt_to_phys(queue);
			*dma_handle = (dma_addr_t)phys_addr;

			/*
			 * Sanity check: make sure we dind't truncate
			 * the address.  The only arches I can find that
			 * have 64-bit phys_addr_t but 32-bit dma_addr_t
			 * are certain non-highmem MIPS and x86
			 * configurations, but these configurations
			 * should never allocate physical pages above 32
			 * bits, so this is fine.  Just in case, throw a
			 * warning and abort if we end up with an
			 * unrepresentable address.
			 */
			if (WARN_ON_ONCE(*dma_handle != phys_addr)) {
				free_pages_exact(queue, PAGE_ALIGN(size));
				return NULL;
			}
		}
		return queue;
	}
}

static void vring_free_queue(struct virtio_device *vdev, size_t size,
			     void *queue, dma_addr_t dma_handle)
{
	if (vring_use_dma_api(vdev))
		dma_free_coherent(vdev->dev.parent, size, queue, dma_handle);
	else
		free_pages_exact(queue, PAGE_ALIGN(size));
}

/*
 * The DMA ops on various arches are rather gnarly right now, and
 * making all of the arch DMA ops work on the vring device itself
 * is a mess.  For now, we use the parent device for DMA ops.
 */
static inline struct device *vring_dma_dev(const struct vring_virtqueue *vq)
{
	return vq->vq.vdev->dev.parent;
}

/* Map one sg entry. */
static dma_addr_t vring_map_one_sg(const struct vring_virtqueue *vq,
				   struct scatterlist *sg,
				   enum dma_data_direction direction)
{
	if (!vq->use_dma_api)
		return (dma_addr_t)sg_phys(sg);

	/*
	 * We can't use dma_map_sg, because we don't use scatterlists in
	 * the way it expects (we don't guarantee that the scatterlist
	 * will exist for the lifetime of the mapping).
	 */
	return dma_map_page(vring_dma_dev(vq),
			    sg_page(sg), sg->offset, sg->length,
			    direction);
}

static dma_addr_t vring_map_single(const struct vring_virtqueue *vq,
				   void *cpu_addr, size_t size,
				   enum dma_data_direction direction)
{
	if (!vq->use_dma_api)
		return (dma_addr_t)virt_to_phys(cpu_addr);

	return dma_map_single(vring_dma_dev(vq),
			      cpu_addr, size, direction);
}

static int vring_mapping_error(const struct vring_virtqueue *vq,
			       dma_addr_t addr)
{
	if (!vq->use_dma_api)
		return 0;

	return dma_mapping_error(vring_dma_dev(vq), addr);
}


/*
 * Split ring specific functions - *_split().
 */

static void vring_unmap_one_split(const struct vring_virtqueue *vq,
				  struct vring_desc *desc)
{
	u16 flags;

	if (!vq->use_dma_api)
		return;

	flags = virtio16_to_cpu(vq->vq.vdev, desc->flags);

	if (flags & VRING_DESC_F_INDIRECT) {
		dma_unmap_single(vring_dma_dev(vq),
				 virtio64_to_cpu(vq->vq.vdev, desc->addr),
				 virtio32_to_cpu(vq->vq.vdev, desc->len),
				 (flags & VRING_DESC_F_WRITE) ?
				 DMA_FROM_DEVICE : DMA_TO_DEVICE);
	} else {
		dma_unmap_page(vring_dma_dev(vq),
			       virtio64_to_cpu(vq->vq.vdev, desc->addr),
			       virtio32_to_cpu(vq->vq.vdev, desc->len),
			       (flags & VRING_DESC_F_WRITE) ?
			       DMA_FROM_DEVICE : DMA_TO_DEVICE);
	}
}

static struct vring_desc *alloc_indirect_split(struct virtqueue *_vq,
					       unsigned int total_sg,
					       gfp_t gfp)
{
	struct vring_desc *desc;
	unsigned int i;

	/*
	 * We require lowmem mappings for the descriptors because
	 * otherwise virt_to_phys will give us bogus addresses in the
	 * virtqueue.
	 */
	gfp &= ~__GFP_HIGHMEM;

	desc = kmalloc_array(total_sg, sizeof(struct vring_desc), gfp);
	if (!desc)
		return NULL;

	for (i = 0; i < total_sg; i++)
		desc[i].next = cpu_to_virtio16(_vq->vdev, i + 1);
	return desc;
}

static inline int virtqueue_add_split(struct virtqueue *_vq,
				      struct scatterlist *sgs[],
				      unsigned int total_sg,
				      unsigned int out_sgs,
				      unsigned int in_sgs,
				      void *data,
				      void *ctx,
				      gfp_t gfp)
{
	struct vring_virtqueue *vq = to_vvq(_vq);
	struct scatterlist *sg;
	struct vring_desc *desc;
	unsigned int i, n, avail, descs_used, prev, err_idx;
	int head;
	bool indirect;

	START_USE(vq);

	BUG_ON(data == NULL);
	BUG_ON(ctx && vq->indirect);

	if (unlikely(vq->broken)) {
		END_USE(vq);
		return -EIO;
	}

	LAST_ADD_TIME_UPDATE(vq);

	BUG_ON(total_sg == 0);

	head = vq->free_head;

	if (virtqueue_use_indirect(_vq, total_sg))
		desc = alloc_indirect_split(_vq, total_sg, gfp);
	else {
		desc = NULL;
		WARN_ON_ONCE(total_sg > vq->split.vring.num && !vq->indirect);
	}

	if (desc) {
		/* Use a single buffer which doesn't continue */
		indirect = true;
		/* Set up rest to use this indirect table. */
		i = 0;
		descs_used = 1;
	} else {
		indirect = false;
		desc = vq->split.vring.desc;
		i = head;
		descs_used = total_sg;
	}

	if (vq->vq.num_free < descs_used) {
		pr_debug("Can't add buf len %i - avail = %i\n",
			 descs_used, vq->vq.num_free);
		/* FIXME: for historical reasons, we force a notify here if
		 * there are outgoing parts to the buffer.  Presumably the
		 * host should service the ring ASAP. */
		if (out_sgs)
			vq->notify(&vq->vq);
		if (indirect)
			kfree(desc);
		END_USE(vq);
		return -ENOSPC;
	}

	for (n = 0; n < out_sgs; n++) {
		for (sg = sgs[n]; sg; sg = sg_next(sg)) {
			dma_addr_t addr = vring_map_one_sg(vq, sg, DMA_TO_DEVICE);
			if (vring_mapping_error(vq, addr))
				goto unmap_release;

			desc[i].flags = cpu_to_virtio16(_vq->vdev, VRING_DESC_F_NEXT);
			desc[i].addr = cpu_to_virtio64(_vq->vdev, addr);
			desc[i].len = cpu_to_virtio32(_vq->vdev, sg->length);
			prev = i;
			i = virtio16_to_cpu(_vq->vdev, desc[i].next);
		}
	}
	for (; n < (out_sgs + in_sgs); n++) {
		for (sg = sgs[n]; sg; sg = sg_next(sg)) {
			dma_addr_t addr = vring_map_one_sg(vq, sg, DMA_FROM_DEVICE);
			if (vring_mapping_error(vq, addr))
				goto unmap_release;

			desc[i].flags = cpu_to_virtio16(_vq->vdev, VRING_DESC_F_NEXT | VRING_DESC_F_WRITE);
			desc[i].addr = cpu_to_virtio64(_vq->vdev, addr);
			desc[i].len = cpu_to_virtio32(_vq->vdev, sg->length);
			prev = i;
			i = virtio16_to_cpu(_vq->vdev, desc[i].next);
		}
	}
	/* Last one doesn't continue. */
	desc[prev].flags &= cpu_to_virtio16(_vq->vdev, ~VRING_DESC_F_NEXT);

	if (indirect) {
		/* Now that the indirect table is filled in, map it. */
		dma_addr_t addr = vring_map_single(
			vq, desc, total_sg * sizeof(struct vring_desc),
			DMA_TO_DEVICE);
		if (vring_mapping_error(vq, addr))
			goto unmap_release;

		vq->split.vring.desc[head].flags = cpu_to_virtio16(_vq->vdev,
				VRING_DESC_F_INDIRECT);
		vq->split.vring.desc[head].addr = cpu_to_virtio64(_vq->vdev,
				addr);

		vq->split.vring.desc[head].len = cpu_to_virtio32(_vq->vdev,
				total_sg * sizeof(struct vring_desc));
	}

	/* We're using some buffers from the free list. */
	vq->vq.num_free -= descs_used;

	/* Update free pointer */
	if (indirect)
		vq->free_head = virtio16_to_cpu(_vq->vdev,
					vq->split.vring.desc[head].next);
	else
		vq->free_head = i;

	/* Store token and indirect buffer state. */
	vq->split.desc_state[head].data = data;
	if (indirect)
		vq->split.desc_state[head].indir_desc = desc;
	else
		vq->split.desc_state[head].indir_desc = ctx;

	/* Put entry in available array (but don't update avail->idx until they
	 * do sync). */
	avail = vq->split.avail_idx_shadow & (vq->split.vring.num - 1);
	vq->split.vring.avail->ring[avail] = cpu_to_virtio16(_vq->vdev, head);

	/* Descriptors and available array need to be set before we expose the
	 * new available array entries. */
	virtio_wmb(vq->weak_barriers);
	vq->split.avail_idx_shadow++;
	vq->split.vring.avail->idx = cpu_to_virtio16(_vq->vdev,
						vq->split.avail_idx_shadow);
	vq->num_added++;

	pr_debug("Added buffer head %i to %p\n", head, vq);
	END_USE(vq);

	/* This is very unlikely, but theoretically possible.  Kick
	 * just in case. */
	if (unlikely(vq->num_added == (1 << 16) - 1))
		virtqueue_kick(_vq);

	return 0;

unmap_release:
	err_idx = i;

	if (indirect)
		i = 0;
	else
		i = head;

	for (n = 0; n < total_sg; n++) {
		if (i == err_idx)
			break;
		vring_unmap_one_split(vq, &desc[i]);
		i = virtio16_to_cpu(_vq->vdev, desc[i].next);
	}

	if (indirect)
		kfree(desc);

	END_USE(vq);
	return -ENOMEM;
}

static bool virtqueue_kick_prepare_split(struct virtqueue *_vq)
{
	struct vring_virtqueue *vq = to_vvq(_vq);
	u16 new, old;
	bool needs_kick;

	START_USE(vq);
	/* We need to expose available array entries before checking avail
	 * event. */
	virtio_mb(vq->weak_barriers);

	old = vq->split.avail_idx_shadow - vq->num_added;
	new = vq->split.avail_idx_shadow;
	vq->num_added = 0;

	LAST_ADD_TIME_CHECK(vq);
	LAST_ADD_TIME_INVALID(vq);

	if (vq->event) {
		needs_kick = vring_need_event(virtio16_to_cpu(_vq->vdev,
					vring_avail_event(&vq->split.vring)),
					      new, old);
	} else {
		needs_kick = !(vq->split.vring.used->flags &
					cpu_to_virtio16(_vq->vdev,
						VRING_USED_F_NO_NOTIFY));
	}
	END_USE(vq);
	return needs_kick;
}

static void detach_buf_split(struct vring_virtqueue *vq, unsigned int head,
			     void **ctx)
{
	unsigned int i, j;
	__virtio16 nextflag = cpu_to_virtio16(vq->vq.vdev, VRING_DESC_F_NEXT);

	/* Clear data ptr. */
	vq->split.desc_state[head].data = NULL;

	/* Put back on free list: unmap first-level descriptors and find end */
	i = head;

	while (vq->split.vring.desc[i].flags & nextflag) {
		vring_unmap_one_split(vq, &vq->split.vring.desc[i]);
		i = virtio16_to_cpu(vq->vq.vdev, vq->split.vring.desc[i].next);
		vq->vq.num_free++;
	}

	vring_unmap_one_split(vq, &vq->split.vring.desc[i]);
	vq->split.vring.desc[i].next = cpu_to_virtio16(vq->vq.vdev,
						vq->free_head);
	vq->free_head = head;

	/* Plus final descriptor */
	vq->vq.num_free++;

	if (vq->indirect) {
		struct vring_desc *indir_desc =
				vq->split.desc_state[head].indir_desc;
		u32 len;

		/* Free the indirect table, if any, now that it's unmapped. */
		if (!indir_desc)
			return;

		len = virtio32_to_cpu(vq->vq.vdev,
				vq->split.vring.desc[head].len);

		BUG_ON(!(vq->split.vring.desc[head].flags &
			 cpu_to_virtio16(vq->vq.vdev, VRING_DESC_F_INDIRECT)));
		BUG_ON(len == 0 || len % sizeof(struct vring_desc));

		for (j = 0; j < len / sizeof(struct vring_desc); j++)
			vring_unmap_one_split(vq, &indir_desc[j]);

		kfree(indir_desc);
		vq->split.desc_state[head].indir_desc = NULL;
	} else if (ctx) {
		*ctx = vq->split.desc_state[head].indir_desc;
	}
}

static inline bool more_used_split(const struct vring_virtqueue *vq)
{
	return vq->last_used_idx != virtio16_to_cpu(vq->vq.vdev,
			vq->split.vring.used->idx);
}

static void *virtqueue_get_buf_ctx_split(struct virtqueue *_vq,
					 unsigned int *len,
					 void **ctx)
{
	struct vring_virtqueue *vq = to_vvq(_vq);
	void *ret;
	unsigned int i;
	u16 last_used;

	START_USE(vq);

	if (unlikely(vq->broken)) {
		END_USE(vq);
		return NULL;
	}

	if (!more_used_split(vq)) {
		pr_debug("No more buffers in queue\n");
		END_USE(vq);
		return NULL;
	}

	/* Only get used array entries after they have been exposed by host. */
	virtio_rmb(vq->weak_barriers);

	last_used = (vq->last_used_idx & (vq->split.vring.num - 1));
	i = virtio32_to_cpu(_vq->vdev,
			vq->split.vring.used->ring[last_used].id);
	*len = virtio32_to_cpu(_vq->vdev,
			vq->split.vring.used->ring[last_used].len);

	if (unlikely(i >= vq->split.vring.num)) {
		BAD_RING(vq, "id %u out of range\n", i);
		return NULL;
	}
	if (unlikely(!vq->split.desc_state[i].data)) {
		BAD_RING(vq, "id %u is not a head!\n", i);
		return NULL;
	}

	/* detach_buf_split clears data, so grab it now. */
	ret = vq->split.desc_state[i].data;
	detach_buf_split(vq, i, ctx);
	vq->last_used_idx++;
	/* If we expect an interrupt for the next entry, tell host
	 * by writing event index and flush out the write before
	 * the read in the next get_buf call. */
	if (!(vq->split.avail_flags_shadow & VRING_AVAIL_F_NO_INTERRUPT))
		virtio_store_mb(vq->weak_barriers,
				&vring_used_event(&vq->split.vring),
				cpu_to_virtio16(_vq->vdev, vq->last_used_idx));

	LAST_ADD_TIME_INVALID(vq);

	END_USE(vq);
	return ret;
}

static void virtqueue_disable_cb_split(struct virtqueue *_vq)
{
	struct vring_virtqueue *vq = to_vvq(_vq);

	if (!(vq->split.avail_flags_shadow & VRING_AVAIL_F_NO_INTERRUPT)) {
		vq->split.avail_flags_shadow |= VRING_AVAIL_F_NO_INTERRUPT;
		if (!vq->event)
			vq->split.vring.avail->flags =
				cpu_to_virtio16(_vq->vdev,
						vq->split.avail_flags_shadow);
	}
}

static unsigned virtqueue_enable_cb_prepare_split(struct virtqueue *_vq)
{
	struct vring_virtqueue *vq = to_vvq(_vq);
	u16 last_used_idx;

	START_USE(vq);

	/* We optimistically turn back on interrupts, then check if there was
	 * more to do. */
	/* Depending on the VIRTIO_RING_F_EVENT_IDX feature, we need to
	 * either clear the flags bit or point the event index at the next
	 * entry. Always do both to keep code simple. */
	if (vq->split.avail_flags_shadow & VRING_AVAIL_F_NO_INTERRUPT) {
		vq->split.avail_flags_shadow &= ~VRING_AVAIL_F_NO_INTERRUPT;
		if (!vq->event)
			vq->split.vring.avail->flags =
				cpu_to_virtio16(_vq->vdev,
						vq->split.avail_flags_shadow);
	}
	vring_used_event(&vq->split.vring) = cpu_to_virtio16(_vq->vdev,
			last_used_idx = vq->last_used_idx);
	END_USE(vq);
	return last_used_idx;
}

static bool virtqueue_poll_split(struct virtqueue *_vq, unsigned last_used_idx)
{
	struct vring_virtqueue *vq = to_vvq(_vq);

	return (u16)last_used_idx != virtio16_to_cpu(_vq->vdev,
			vq->split.vring.used->idx);
}

static bool virtqueue_enable_cb_delayed_split(struct virtqueue *_vq)
{
	struct vring_virtqueue *vq = to_vvq(_vq);
	u16 bufs;

	START_USE(vq);

	/* We optimistically turn back on interrupts, then check if there was
	 * more to do. */
	/* Depending on the VIRTIO_RING_F_USED_EVENT_IDX feature, we need to
	 * either clear the flags bit or point the event index at the next
	 * entry. Always update the event index to keep code simple. */
	if (vq->split.avail_flags_shadow & VRING_AVAIL_F_NO_INTERRUPT) {
		vq->split.avail_flags_shadow &= ~VRING_AVAIL_F_NO_INTERRUPT;
		if (!vq->event)
			vq->split.vring.avail->flags =
				cpu_to_virtio16(_vq->vdev,
						vq->split.avail_flags_shadow);
	}
	/* TODO: tune this threshold */
	bufs = (u16)(vq->split.avail_idx_shadow - vq->last_used_idx) * 3 / 4;

	virtio_store_mb(vq->weak_barriers,
			&vring_used_event(&vq->split.vring),
			cpu_to_virtio16(_vq->vdev, vq->last_used_idx + bufs));

	if (unlikely((u16)(virtio16_to_cpu(_vq->vdev, vq->split.vring.used->idx)
					- vq->last_used_idx) > bufs)) {
		END_USE(vq);
		return false;
	}

	END_USE(vq);
	return true;
}

static void *virtqueue_detach_unused_buf_split(struct virtqueue *_vq)
{
	struct vring_virtqueue *vq = to_vvq(_vq);
	unsigned int i;
	void *buf;

	START_USE(vq);

	for (i = 0; i < vq->split.vring.num; i++) {
		if (!vq->split.desc_state[i].data)
			continue;
		/* detach_buf_split clears data, so grab it now. */
		buf = vq->split.desc_state[i].data;
		detach_buf_split(vq, i, NULL);
		vq->split.avail_idx_shadow--;
		vq->split.vring.avail->idx = cpu_to_virtio16(_vq->vdev,
				vq->split.avail_idx_shadow);
		END_USE(vq);
		return buf;
	}
	/* That should have freed everything. */
	BUG_ON(vq->vq.num_free != vq->split.vring.num);

	END_USE(vq);
	return NULL;
}

static struct virtqueue *vring_create_virtqueue_split(
	unsigned int index,
	unsigned int num,
	unsigned int vring_align,
	struct virtio_device *vdev,
	bool weak_barriers,
	bool may_reduce_num,
	bool context,
	bool (*notify)(struct virtqueue *),
	void (*callback)(struct virtqueue *),
	const char *name)
{
	struct virtqueue *vq;
	void *queue = NULL;
	dma_addr_t dma_addr;
	size_t queue_size_in_bytes;
	struct vring vring;

	/* We assume num is a power of 2. */
	if (num & (num - 1)) {
		dev_warn(&vdev->dev, "Bad virtqueue length %u\n", num);
		return NULL;
	}

	/* TODO: allocate each queue chunk individually */
	for (; num && vring_size(num, vring_align) > PAGE_SIZE; num /= 2) {
		queue = vring_alloc_queue(vdev, vring_size(num, vring_align),
					  &dma_addr,
					  GFP_KERNEL|__GFP_NOWARN|__GFP_ZERO);
		if (queue)
			break;
		if (!may_reduce_num)
			return NULL;
	}

	if (!num)
		return NULL;

	if (!queue) {
		/* Try to get a single page. You are my only hope! */
		queue = vring_alloc_queue(vdev, vring_size(num, vring_align),
					  &dma_addr, GFP_KERNEL|__GFP_ZERO);
	}
	if (!queue)
		return NULL;

	queue_size_in_bytes = vring_size(num, vring_align);
	vring_init(&vring, num, queue, vring_align);

	vq = __vring_new_virtqueue(index, vring, vdev, weak_barriers, context,
				   notify, callback, name);
	if (!vq) {
		vring_free_queue(vdev, queue_size_in_bytes, queue,
				 dma_addr);
		return NULL;
	}

	to_vvq(vq)->split.queue_dma_addr = dma_addr;
	to_vvq(vq)->split.queue_size_in_bytes = queue_size_in_bytes;
	to_vvq(vq)->we_own_ring = true;

	return vq;
}


/*
 * Packed ring specific functions - *_packed().
 */

static void vring_unmap_state_packed(const struct vring_virtqueue *vq,
				     struct vring_desc_extra_packed *state)
{
	u16 flags;

	if (!vq->use_dma_api)
		return;

	flags = state->flags;

	if (flags & VRING_DESC_F_INDIRECT) {
		dma_unmap_single(vring_dma_dev(vq),
				 state->addr, state->len,
				 (flags & VRING_DESC_F_WRITE) ?
				 DMA_FROM_DEVICE : DMA_TO_DEVICE);
	} else {
		dma_unmap_page(vring_dma_dev(vq),
			       state->addr, state->len,
			       (flags & VRING_DESC_F_WRITE) ?
			       DMA_FROM_DEVICE : DMA_TO_DEVICE);
	}
}

static void vring_unmap_desc_packed(const struct vring_virtqueue *vq,
				   struct vring_packed_desc *desc)
{
	u16 flags;

	if (!vq->use_dma_api)
		return;

	flags = le16_to_cpu(desc->flags);

	if (flags & VRING_DESC_F_INDIRECT) {
		dma_unmap_single(vring_dma_dev(vq),
				 le64_to_cpu(desc->addr),
				 le32_to_cpu(desc->len),
				 (flags & VRING_DESC_F_WRITE) ?
				 DMA_FROM_DEVICE : DMA_TO_DEVICE);
	} else {
		dma_unmap_page(vring_dma_dev(vq),
			       le64_to_cpu(desc->addr),
			       le32_to_cpu(desc->len),
			       (flags & VRING_DESC_F_WRITE) ?
			       DMA_FROM_DEVICE : DMA_TO_DEVICE);
	}
}

static struct vring_packed_desc *alloc_indirect_packed(unsigned int total_sg,
						       gfp_t gfp)
{
	struct vring_packed_desc *desc;

	/*
	 * We require lowmem mappings for the descriptors because
	 * otherwise virt_to_phys will give us bogus addresses in the
	 * virtqueue.
	 */
	gfp &= ~__GFP_HIGHMEM;

	desc = kmalloc_array(total_sg, sizeof(struct vring_packed_desc), gfp);

	return desc;
}

static int virtqueue_add_indirect_packed(struct vring_virtqueue *vq,
				       struct scatterlist *sgs[],
				       unsigned int total_sg,
				       unsigned int out_sgs,
				       unsigned int in_sgs,
				       void *data,
				       gfp_t gfp)
{
	struct vring_packed_desc *desc;
	struct scatterlist *sg;
	unsigned int i, n, err_idx;
	u16 head, id;
	dma_addr_t addr;

	head = vq->packed.next_avail_idx;
	desc = alloc_indirect_packed(total_sg, gfp);

	if (unlikely(vq->vq.num_free < 1)) {
		pr_debug("Can't add buf len 1 - avail = 0\n");
		kfree(desc);
		END_USE(vq);
		return -ENOSPC;
	}

	i = 0;
	id = vq->free_head;
	BUG_ON(id == vq->packed.vring.num);

	for (n = 0; n < out_sgs + in_sgs; n++) {
		for (sg = sgs[n]; sg; sg = sg_next(sg)) {
			addr = vring_map_one_sg(vq, sg, n < out_sgs ?
					DMA_TO_DEVICE : DMA_FROM_DEVICE);
			if (vring_mapping_error(vq, addr))
				goto unmap_release;

			desc[i].flags = cpu_to_le16(n < out_sgs ?
						0 : VRING_DESC_F_WRITE);
			desc[i].addr = cpu_to_le64(addr);
			desc[i].len = cpu_to_le32(sg->length);
			i++;
		}
	}

	/* Now that the indirect table is filled in, map it. */
	addr = vring_map_single(vq, desc,
			total_sg * sizeof(struct vring_packed_desc),
			DMA_TO_DEVICE);
	if (vring_mapping_error(vq, addr))
		goto unmap_release;

	vq->packed.vring.desc[head].addr = cpu_to_le64(addr);
	vq->packed.vring.desc[head].len = cpu_to_le32(total_sg *
				sizeof(struct vring_packed_desc));
	vq->packed.vring.desc[head].id = cpu_to_le16(id);

	if (vq->use_dma_api) {
		vq->packed.desc_extra[id].addr = addr;
		vq->packed.desc_extra[id].len = total_sg *
				sizeof(struct vring_packed_desc);
		vq->packed.desc_extra[id].flags = VRING_DESC_F_INDIRECT |
						  vq->packed.avail_used_flags;
	}

	/*
	 * A driver MUST NOT make the first descriptor in the list
	 * available before all subsequent descriptors comprising
	 * the list are made available.
	 */
	virtio_wmb(vq->weak_barriers);
	vq->packed.vring.desc[head].flags = cpu_to_le16(VRING_DESC_F_INDIRECT |
						vq->packed.avail_used_flags);

	/* We're using some buffers from the free list. */
	vq->vq.num_free -= 1;

	/* Update free pointer */
	n = head + 1;
	if (n >= vq->packed.vring.num) {
		n = 0;
		vq->packed.avail_wrap_counter ^= 1;
		vq->packed.avail_used_flags ^=
				1 << VRING_PACKED_DESC_F_AVAIL |
				1 << VRING_PACKED_DESC_F_USED;
	}
	vq->packed.next_avail_idx = n;
	vq->free_head = vq->packed.desc_state[id].next;

	/* Store token and indirect buffer state. */
	vq->packed.desc_state[id].num = 1;
	vq->packed.desc_state[id].data = data;
	vq->packed.desc_state[id].indir_desc = desc;
	vq->packed.desc_state[id].last = id;

	vq->num_added += 1;

	pr_debug("Added buffer head %i to %p\n", head, vq);
	END_USE(vq);

	return 0;

unmap_release:
	err_idx = i;

	for (i = 0; i < err_idx; i++)
		vring_unmap_desc_packed(vq, &desc[i]);

	kfree(desc);

	END_USE(vq);
	return -ENOMEM;
}

static inline int virtqueue_add_packed(struct virtqueue *_vq,
				       struct scatterlist *sgs[],
				       unsigned int total_sg,
				       unsigned int out_sgs,
				       unsigned int in_sgs,
				       void *data,
				       void *ctx,
				       gfp_t gfp)
{
	struct vring_virtqueue *vq = to_vvq(_vq);
	struct vring_packed_desc *desc;
	struct scatterlist *sg;
	unsigned int i, n, c, descs_used, err_idx;
	__le16 head_flags, flags;
	u16 head, id, prev, curr, avail_used_flags;

	START_USE(vq);

	BUG_ON(data == NULL);
	BUG_ON(ctx && vq->indirect);

	if (unlikely(vq->broken)) {
		END_USE(vq);
		return -EIO;
	}

	LAST_ADD_TIME_UPDATE(vq);

	BUG_ON(total_sg == 0);

	if (virtqueue_use_indirect(_vq, total_sg))
		return virtqueue_add_indirect_packed(vq, sgs, total_sg,
				out_sgs, in_sgs, data, gfp);

	head = vq->packed.next_avail_idx;
	avail_used_flags = vq->packed.avail_used_flags;

	WARN_ON_ONCE(total_sg > vq->packed.vring.num && !vq->indirect);

	desc = vq->packed.vring.desc;
	i = head;
	descs_used = total_sg;

	if (unlikely(vq->vq.num_free < descs_used)) {
		pr_debug("Can't add buf len %i - avail = %i\n",
			 descs_used, vq->vq.num_free);
		END_USE(vq);
		return -ENOSPC;
	}

	id = vq->free_head;
	BUG_ON(id == vq->packed.vring.num);

	curr = id;
	c = 0;
	for (n = 0; n < out_sgs + in_sgs; n++) {
		for (sg = sgs[n]; sg; sg = sg_next(sg)) {
			dma_addr_t addr = vring_map_one_sg(vq, sg, n < out_sgs ?
					DMA_TO_DEVICE : DMA_FROM_DEVICE);
			if (vring_mapping_error(vq, addr))
				goto unmap_release;

			flags = cpu_to_le16(vq->packed.avail_used_flags |
				    (++c == total_sg ? 0 : VRING_DESC_F_NEXT) |
				    (n < out_sgs ? 0 : VRING_DESC_F_WRITE));
			if (i == head)
				head_flags = flags;
			else
				desc[i].flags = flags;

			desc[i].addr = cpu_to_le64(addr);
			desc[i].len = cpu_to_le32(sg->length);
			desc[i].id = cpu_to_le16(id);

			if (unlikely(vq->use_dma_api)) {
				vq->packed.desc_extra[curr].addr = addr;
				vq->packed.desc_extra[curr].len = sg->length;
				vq->packed.desc_extra[curr].flags =
					le16_to_cpu(flags);
			}
			prev = curr;
			curr = vq->packed.desc_state[curr].next;

			if ((unlikely(++i >= vq->packed.vring.num))) {
				i = 0;
				vq->packed.avail_used_flags ^=
					1 << VRING_PACKED_DESC_F_AVAIL |
					1 << VRING_PACKED_DESC_F_USED;
			}
		}
	}

	if (i < head)
		vq->packed.avail_wrap_counter ^= 1;

	/* We're using some buffers from the free list. */
	vq->vq.num_free -= descs_used;

	/* Update free pointer */
	vq->packed.next_avail_idx = i;
	vq->free_head = curr;

	/* Store token. */
	vq->packed.desc_state[id].num = descs_used;
	vq->packed.desc_state[id].data = data;
	vq->packed.desc_state[id].indir_desc = ctx;
	vq->packed.desc_state[id].last = prev;

	/*
	 * A driver MUST NOT make the first descriptor in the list
	 * available before all subsequent descriptors comprising
	 * the list are made available.
	 */
	virtio_wmb(vq->weak_barriers);
	vq->packed.vring.desc[head].flags = head_flags;
	vq->num_added += descs_used;

	pr_debug("Added buffer head %i to %p\n", head, vq);
	END_USE(vq);

	return 0;

unmap_release:
	err_idx = i;
	i = head;

	vq->packed.avail_used_flags = avail_used_flags;

	for (n = 0; n < total_sg; n++) {
		if (i == err_idx)
			break;
		vring_unmap_desc_packed(vq, &desc[i]);
		i++;
		if (i >= vq->packed.vring.num)
			i = 0;
	}

	END_USE(vq);
	return -EIO;
}

static bool virtqueue_kick_prepare_packed(struct virtqueue *_vq)
{
	struct vring_virtqueue *vq = to_vvq(_vq);
	u16 new, old, off_wrap, flags, wrap_counter, event_idx;
	bool needs_kick;
	union {
		struct {
			__le16 off_wrap;
			__le16 flags;
		};
		u32 u32;
	} snapshot;

	START_USE(vq);

	/*
	 * We need to expose the new flags value before checking notification
	 * suppressions.
	 */
	virtio_mb(vq->weak_barriers);

	old = vq->packed.next_avail_idx - vq->num_added;
	new = vq->packed.next_avail_idx;
	vq->num_added = 0;

	snapshot.u32 = *(u32 *)vq->packed.vring.device;
	flags = le16_to_cpu(snapshot.flags);

	LAST_ADD_TIME_CHECK(vq);
	LAST_ADD_TIME_INVALID(vq);

	if (flags != VRING_PACKED_EVENT_FLAG_DESC) {
		needs_kick = (flags != VRING_PACKED_EVENT_FLAG_DISABLE);
		goto out;
	}

	off_wrap = le16_to_cpu(snapshot.off_wrap);

	wrap_counter = off_wrap >> VRING_PACKED_EVENT_F_WRAP_CTR;
	event_idx = off_wrap & ~(1 << VRING_PACKED_EVENT_F_WRAP_CTR);
	if (wrap_counter != vq->packed.avail_wrap_counter)
		event_idx -= vq->packed.vring.num;

	needs_kick = vring_need_event(event_idx, new, old);
out:
	END_USE(vq);
	return needs_kick;
}

static void detach_buf_packed(struct vring_virtqueue *vq,
			      unsigned int id, void **ctx)
{
	struct vring_desc_state_packed *state = NULL;
	struct vring_packed_desc *desc;
	unsigned int i, curr;

	state = &vq->packed.desc_state[id];

	/* Clear data ptr. */
	state->data = NULL;

	vq->packed.desc_state[state->last].next = vq->free_head;
	vq->free_head = id;
	vq->vq.num_free += state->num;

	if (unlikely(vq->use_dma_api)) {
		curr = id;
		for (i = 0; i < state->num; i++) {
			vring_unmap_state_packed(vq,
				&vq->packed.desc_extra[curr]);
			curr = vq->packed.desc_state[curr].next;
		}
	}

	if (vq->indirect) {
		u32 len;

		/* Free the indirect table, if any, now that it's unmapped. */
		desc = state->indir_desc;
		if (!desc)
			return;

		if (vq->use_dma_api) {
			len = vq->packed.desc_extra[id].len;
			for (i = 0; i < len / sizeof(struct vring_packed_desc);
					i++)
				vring_unmap_desc_packed(vq, &desc[i]);
		}
		kfree(desc);
		state->indir_desc = NULL;
	} else if (ctx) {
		*ctx = state->indir_desc;
	}
}

static inline bool is_used_desc_packed(const struct vring_virtqueue *vq,
				       u16 idx, bool used_wrap_counter)
{
	bool avail, used;
	u16 flags;

	flags = le16_to_cpu(vq->packed.vring.desc[idx].flags);
	avail = !!(flags & (1 << VRING_PACKED_DESC_F_AVAIL));
	used = !!(flags & (1 << VRING_PACKED_DESC_F_USED));

	return avail == used && used == used_wrap_counter;
}

static inline bool more_used_packed(const struct vring_virtqueue *vq)
{
	return is_used_desc_packed(vq, vq->last_used_idx,
			vq->packed.used_wrap_counter);
}

static void *virtqueue_get_buf_ctx_packed(struct virtqueue *_vq,
					  unsigned int *len,
					  void **ctx)
{
	struct vring_virtqueue *vq = to_vvq(_vq);
	u16 last_used, id;
	void *ret;

	START_USE(vq);

	if (unlikely(vq->broken)) {
		END_USE(vq);
		return NULL;
	}

	if (!more_used_packed(vq)) {
		pr_debug("No more buffers in queue\n");
		END_USE(vq);
		return NULL;
	}

	/* Only get used elements after they have been exposed by host. */
	virtio_rmb(vq->weak_barriers);

	last_used = vq->last_used_idx;
	id = le16_to_cpu(vq->packed.vring.desc[last_used].id);
	*len = le32_to_cpu(vq->packed.vring.desc[last_used].len);

	if (unlikely(id >= vq->packed.vring.num)) {
		BAD_RING(vq, "id %u out of range\n", id);
		return NULL;
	}
	if (unlikely(!vq->packed.desc_state[id].data)) {
		BAD_RING(vq, "id %u is not a head!\n", id);
		return NULL;
	}

	/* detach_buf_packed clears data, so grab it now. */
	ret = vq->packed.desc_state[id].data;
	detach_buf_packed(vq, id, ctx);

	vq->last_used_idx += vq->packed.desc_state[id].num;
	if (unlikely(vq->last_used_idx >= vq->packed.vring.num)) {
		vq->last_used_idx -= vq->packed.vring.num;
		vq->packed.used_wrap_counter ^= 1;
	}

	/*
	 * If we expect an interrupt for the next entry, tell host
	 * by writing event index and flush out the write before
	 * the read in the next get_buf call.
	 */
	if (vq->packed.event_flags_shadow == VRING_PACKED_EVENT_FLAG_DESC)
		virtio_store_mb(vq->weak_barriers,
				&vq->packed.vring.driver->off_wrap,
				cpu_to_le16(vq->last_used_idx |
					(vq->packed.used_wrap_counter <<
					 VRING_PACKED_EVENT_F_WRAP_CTR)));

	LAST_ADD_TIME_INVALID(vq);

	END_USE(vq);
	return ret;
}

static void virtqueue_disable_cb_packed(struct virtqueue *_vq)
{
	struct vring_virtqueue *vq = to_vvq(_vq);

	if (vq->packed.event_flags_shadow != VRING_PACKED_EVENT_FLAG_DISABLE) {
		vq->packed.event_flags_shadow = VRING_PACKED_EVENT_FLAG_DISABLE;
		vq->packed.vring.driver->flags =
			cpu_to_le16(vq->packed.event_flags_shadow);
	}
}

static unsigned virtqueue_enable_cb_prepare_packed(struct virtqueue *_vq)
{
	struct vring_virtqueue *vq = to_vvq(_vq);

	START_USE(vq);

	/*
	 * We optimistically turn back on interrupts, then check if there was
	 * more to do.
	 */

	if (vq->event) {
		vq->packed.vring.driver->off_wrap =
			cpu_to_le16(vq->last_used_idx |
				(vq->packed.used_wrap_counter <<
				 VRING_PACKED_EVENT_F_WRAP_CTR));
		/*
		 * We need to update event offset and event wrap
		 * counter first before updating event flags.
		 */
		virtio_wmb(vq->weak_barriers);
	}

	if (vq->packed.event_flags_shadow == VRING_PACKED_EVENT_FLAG_DISABLE) {
		vq->packed.event_flags_shadow = vq->event ?
				VRING_PACKED_EVENT_FLAG_DESC :
				VRING_PACKED_EVENT_FLAG_ENABLE;
		vq->packed.vring.driver->flags =
				cpu_to_le16(vq->packed.event_flags_shadow);
	}

	END_USE(vq);
	return vq->last_used_idx | ((u16)vq->packed.used_wrap_counter <<
			VRING_PACKED_EVENT_F_WRAP_CTR);
}

static bool virtqueue_poll_packed(struct virtqueue *_vq, u16 off_wrap)
{
	struct vring_virtqueue *vq = to_vvq(_vq);
	bool wrap_counter;
	u16 used_idx;

	wrap_counter = off_wrap >> VRING_PACKED_EVENT_F_WRAP_CTR;
	used_idx = off_wrap & ~(1 << VRING_PACKED_EVENT_F_WRAP_CTR);

	return is_used_desc_packed(vq, used_idx, wrap_counter);
}

static bool virtqueue_enable_cb_delayed_packed(struct virtqueue *_vq)
{
	struct vring_virtqueue *vq = to_vvq(_vq);
	u16 used_idx, wrap_counter;
	u16 bufs;

	START_USE(vq);

	/*
	 * We optimistically turn back on interrupts, then check if there was
	 * more to do.
	 */

	if (vq->event) {
		/* TODO: tune this threshold */
		bufs = (vq->packed.vring.num - vq->vq.num_free) * 3 / 4;
		wrap_counter = vq->packed.used_wrap_counter;

		used_idx = vq->last_used_idx + bufs;
		if (used_idx >= vq->packed.vring.num) {
			used_idx -= vq->packed.vring.num;
			wrap_counter ^= 1;
		}

		vq->packed.vring.driver->off_wrap = cpu_to_le16(used_idx |
			(wrap_counter << VRING_PACKED_EVENT_F_WRAP_CTR));

		/*
		 * We need to update event offset and event wrap
		 * counter first before updating event flags.
		 */
		virtio_wmb(vq->weak_barriers);
	}

	if (vq->packed.event_flags_shadow == VRING_PACKED_EVENT_FLAG_DISABLE) {
		vq->packed.event_flags_shadow = vq->event ?
				VRING_PACKED_EVENT_FLAG_DESC :
				VRING_PACKED_EVENT_FLAG_ENABLE;
		vq->packed.vring.driver->flags =
				cpu_to_le16(vq->packed.event_flags_shadow);
	}

	/*
	 * We need to update event suppression structure first
	 * before re-checking for more used buffers.
	 */
	virtio_mb(vq->weak_barriers);

	if (is_used_desc_packed(vq,
				vq->last_used_idx,
				vq->packed.used_wrap_counter)) {
		END_USE(vq);
		return false;
	}

	END_USE(vq);
	return true;
}

static void *virtqueue_detach_unused_buf_packed(struct virtqueue *_vq)
{
	struct vring_virtqueue *vq = to_vvq(_vq);
	unsigned int i;
	void *buf;

	START_USE(vq);

	for (i = 0; i < vq->packed.vring.num; i++) {
		if (!vq->packed.desc_state[i].data)
			continue;
		/* detach_buf clears data, so grab it now. */
		buf = vq->packed.desc_state[i].data;
		detach_buf_packed(vq, i, NULL);
		END_USE(vq);
		return buf;
	}
	/* That should have freed everything. */
	BUG_ON(vq->vq.num_free != vq->packed.vring.num);

	END_USE(vq);
	return NULL;
}

static struct virtqueue *vring_create_virtqueue_packed(
	unsigned int index,
	unsigned int num,
	unsigned int vring_align,
	struct virtio_device *vdev,
	bool weak_barriers,
	bool may_reduce_num,
	bool context,
	bool (*notify)(struct virtqueue *),
	void (*callback)(struct virtqueue *),
	const char *name)
{
	struct vring_virtqueue *vq;
	struct vring_packed_desc *ring;
	struct vring_packed_desc_event *driver, *device;
	dma_addr_t ring_dma_addr, driver_event_dma_addr, device_event_dma_addr;
	size_t ring_size_in_bytes, event_size_in_bytes;
	unsigned int i;

	ring_size_in_bytes = num * sizeof(struct vring_packed_desc);

	ring = vring_alloc_queue(vdev, ring_size_in_bytes,
				 &ring_dma_addr,
				 GFP_KERNEL|__GFP_NOWARN|__GFP_ZERO);
	if (!ring)
		goto err_ring;

	event_size_in_bytes = sizeof(struct vring_packed_desc_event);

	driver = vring_alloc_queue(vdev, event_size_in_bytes,
				   &driver_event_dma_addr,
				   GFP_KERNEL|__GFP_NOWARN|__GFP_ZERO);
	if (!driver)
		goto err_driver;

	device = vring_alloc_queue(vdev, event_size_in_bytes,
				   &device_event_dma_addr,
				   GFP_KERNEL|__GFP_NOWARN|__GFP_ZERO);
	if (!device)
		goto err_device;

	vq = kmalloc(sizeof(*vq), GFP_KERNEL);
	if (!vq)
		goto err_vq;

	vq->vq.callback = callback;
	vq->vq.vdev = vdev;
	vq->vq.name = name;
	vq->vq.num_free = num;
	vq->vq.index = index;
	vq->we_own_ring = true;
	vq->notify = notify;
	vq->weak_barriers = weak_barriers;
	vq->broken = false;
	vq->last_used_idx = 0;
	vq->num_added = 0;
	vq->packed_ring = true;
	vq->use_dma_api = vring_use_dma_api(vdev);
#ifdef DEBUG
	vq->in_use = false;
	vq->last_add_time_valid = false;
#endif

	vq->indirect = virtio_has_feature(vdev, VIRTIO_RING_F_INDIRECT_DESC) &&
		!context;
	vq->event = virtio_has_feature(vdev, VIRTIO_RING_F_EVENT_IDX);

	if (virtio_has_feature(vdev, VIRTIO_F_ORDER_PLATFORM))
		vq->weak_barriers = false;

	vq->packed.ring_dma_addr = ring_dma_addr;
	vq->packed.driver_event_dma_addr = driver_event_dma_addr;
	vq->packed.device_event_dma_addr = device_event_dma_addr;

	vq->packed.ring_size_in_bytes = ring_size_in_bytes;
	vq->packed.event_size_in_bytes = event_size_in_bytes;

	vq->packed.vring.num = num;
	vq->packed.vring.desc = ring;
	vq->packed.vring.driver = driver;
	vq->packed.vring.device = device;

	vq->packed.next_avail_idx = 0;
	vq->packed.avail_wrap_counter = 1;
	vq->packed.used_wrap_counter = 1;
	vq->packed.event_flags_shadow = 0;
	vq->packed.avail_used_flags = 1 << VRING_PACKED_DESC_F_AVAIL;

	vq->packed.desc_state = kmalloc_array(num,
			sizeof(struct vring_desc_state_packed),
			GFP_KERNEL);
	if (!vq->packed.desc_state)
		goto err_desc_state;

	memset(vq->packed.desc_state, 0,
		num * sizeof(struct vring_desc_state_packed));

	/* Put everything in free lists. */
	vq->free_head = 0;
	for (i = 0; i < num-1; i++)
		vq->packed.desc_state[i].next = i + 1;

	vq->packed.desc_extra = kmalloc_array(num,
			sizeof(struct vring_desc_extra_packed),
			GFP_KERNEL);
	if (!vq->packed.desc_extra)
		goto err_desc_extra;

	memset(vq->packed.desc_extra, 0,
		num * sizeof(struct vring_desc_extra_packed));

	/* No callback?  Tell other side not to bother us. */
	if (!callback) {
		vq->packed.event_flags_shadow = VRING_PACKED_EVENT_FLAG_DISABLE;
		vq->packed.vring.driver->flags =
			cpu_to_le16(vq->packed.event_flags_shadow);
	}

	list_add_tail(&vq->vq.list, &vdev->vqs);
	return &vq->vq;

err_desc_extra:
	kfree(vq->packed.desc_state);
err_desc_state:
	kfree(vq);
err_vq:
	vring_free_queue(vdev, event_size_in_bytes, device, device_event_dma_addr);
err_device:
	vring_free_queue(vdev, event_size_in_bytes, driver, driver_event_dma_addr);
err_driver:
	vring_free_queue(vdev, ring_size_in_bytes, ring, ring_dma_addr);
err_ring:
	return NULL;
}


/*
 * Generic functions and exported symbols.
 */

static inline int virtqueue_add(struct virtqueue *_vq,
				struct scatterlist *sgs[],
				unsigned int total_sg,
				unsigned int out_sgs,
				unsigned int in_sgs,
				void *data,
				void *ctx,
				gfp_t gfp)
{
	struct vring_virtqueue *vq = to_vvq(_vq);

	return vq->packed_ring ? virtqueue_add_packed(_vq, sgs, total_sg,
					out_sgs, in_sgs, data, ctx, gfp) :
				 virtqueue_add_split(_vq, sgs, total_sg,
					out_sgs, in_sgs, data, ctx, gfp);
}

/**
 * virtqueue_add_sgs - expose buffers to other end
 * @_vq: the struct virtqueue we're talking about.
 * @sgs: array of terminated scatterlists.
 * @out_sgs: the number of scatterlists readable by other side
 * @in_sgs: the number of scatterlists which are writable (after readable ones)
 * @data: the token identifying the buffer.
 * @gfp: how to do memory allocations (if necessary).
 *
 * Caller must ensure we don't call this with other virtqueue operations
 * at the same time (except where noted).
 *
 * Returns zero or a negative error (ie. ENOSPC, ENOMEM, EIO).
 */
int virtqueue_add_sgs(struct virtqueue *_vq,
		      struct scatterlist *sgs[],
		      unsigned int out_sgs,
		      unsigned int in_sgs,
		      void *data,
		      gfp_t gfp)
{
	unsigned int i, total_sg = 0;

	/* Count them first. */
	for (i = 0; i < out_sgs + in_sgs; i++) {
		struct scatterlist *sg;

		for (sg = sgs[i]; sg; sg = sg_next(sg))
			total_sg++;
	}
	return virtqueue_add(_vq, sgs, total_sg, out_sgs, in_sgs,
			     data, NULL, gfp);
}
EXPORT_SYMBOL_GPL(virtqueue_add_sgs);

/**
 * virtqueue_add_outbuf - expose output buffers to other end
 * @vq: the struct virtqueue we're talking about.
 * @sg: scatterlist (must be well-formed and terminated!)
 * @num: the number of entries in @sg readable by other side
 * @data: the token identifying the buffer.
 * @gfp: how to do memory allocations (if necessary).
 *
 * Caller must ensure we don't call this with other virtqueue operations
 * at the same time (except where noted).
 *
 * Returns zero or a negative error (ie. ENOSPC, ENOMEM, EIO).
 */
int virtqueue_add_outbuf(struct virtqueue *vq,
			 struct scatterlist *sg, unsigned int num,
			 void *data,
			 gfp_t gfp)
{
	return virtqueue_add(vq, &sg, num, 1, 0, data, NULL, gfp);
}
EXPORT_SYMBOL_GPL(virtqueue_add_outbuf);

/**
 * virtqueue_add_inbuf - expose input buffers to other end
 * @vq: the struct virtqueue we're talking about.
 * @sg: scatterlist (must be well-formed and terminated!)
 * @num: the number of entries in @sg writable by other side
 * @data: the token identifying the buffer.
 * @gfp: how to do memory allocations (if necessary).
 *
 * Caller must ensure we don't call this with other virtqueue operations
 * at the same time (except where noted).
 *
 * Returns zero or a negative error (ie. ENOSPC, ENOMEM, EIO).
 */
int virtqueue_add_inbuf(struct virtqueue *vq,
			struct scatterlist *sg, unsigned int num,
			void *data,
			gfp_t gfp)
{
	return virtqueue_add(vq, &sg, num, 0, 1, data, NULL, gfp);
}
EXPORT_SYMBOL_GPL(virtqueue_add_inbuf);

/**
 * virtqueue_add_inbuf_ctx - expose input buffers to other end
 * @vq: the struct virtqueue we're talking about.
 * @sg: scatterlist (must be well-formed and terminated!)
 * @num: the number of entries in @sg writable by other side
 * @data: the token identifying the buffer.
 * @ctx: extra context for the token
 * @gfp: how to do memory allocations (if necessary).
 *
 * Caller must ensure we don't call this with other virtqueue operations
 * at the same time (except where noted).
 *
 * Returns zero or a negative error (ie. ENOSPC, ENOMEM, EIO).
 */
int virtqueue_add_inbuf_ctx(struct virtqueue *vq,
			struct scatterlist *sg, unsigned int num,
			void *data,
			void *ctx,
			gfp_t gfp)
{
	return virtqueue_add(vq, &sg, num, 0, 1, data, ctx, gfp);
}
EXPORT_SYMBOL_GPL(virtqueue_add_inbuf_ctx);

/**
 * virtqueue_kick_prepare - first half of split virtqueue_kick call.
 * @_vq: the struct virtqueue
 *
 * Instead of virtqueue_kick(), you can do:
 *	if (virtqueue_kick_prepare(vq))
 *		virtqueue_notify(vq);
 *
 * This is sometimes useful because the virtqueue_kick_prepare() needs
 * to be serialized, but the actual virtqueue_notify() call does not.
 */
bool virtqueue_kick_prepare(struct virtqueue *_vq)
{
	struct vring_virtqueue *vq = to_vvq(_vq);

	return vq->packed_ring ? virtqueue_kick_prepare_packed(_vq) :
				 virtqueue_kick_prepare_split(_vq);
}
EXPORT_SYMBOL_GPL(virtqueue_kick_prepare);

/**
 * virtqueue_notify - second half of split virtqueue_kick call.
 * @_vq: the struct virtqueue
 *
 * This does not need to be serialized.
 *
 * Returns false if host notify failed or queue is broken, otherwise true.
 */
bool virtqueue_notify(struct virtqueue *_vq)
{
	struct vring_virtqueue *vq = to_vvq(_vq);

	if (unlikely(vq->broken))
		return false;

	/* Prod other side to tell it about changes. */
	if (!vq->notify(_vq)) {
		vq->broken = true;
		return false;
	}
	return true;
}
EXPORT_SYMBOL_GPL(virtqueue_notify);

/**
 * virtqueue_kick - update after add_buf
 * @vq: the struct virtqueue
 *
 * After one or more virtqueue_add_* calls, invoke this to kick
 * the other side.
 *
 * Caller must ensure we don't call this with other virtqueue
 * operations at the same time (except where noted).
 *
 * Returns false if kick failed, otherwise true.
 */
bool virtqueue_kick(struct virtqueue *vq)
{
	if (virtqueue_kick_prepare(vq))
		return virtqueue_notify(vq);
	return true;
}
EXPORT_SYMBOL_GPL(virtqueue_kick);

/**
 * virtqueue_get_buf - get the next used buffer
 * @_vq: the struct virtqueue we're talking about.
 * @len: the length written into the buffer
 * @ctx: extra context for the token
 *
 * If the device wrote data into the buffer, @len will be set to the
 * amount written.  This means you don't need to clear the buffer
 * beforehand to ensure there's no data leakage in the case of short
 * writes.
 *
 * Caller must ensure we don't call this with other virtqueue
 * operations at the same time (except where noted).
 *
 * Returns NULL if there are no used buffers, or the "data" token
 * handed to virtqueue_add_*().
 */
void *virtqueue_get_buf_ctx(struct virtqueue *_vq, unsigned int *len,
			    void **ctx)
{
	struct vring_virtqueue *vq = to_vvq(_vq);

	return vq->packed_ring ? virtqueue_get_buf_ctx_packed(_vq, len, ctx) :
				 virtqueue_get_buf_ctx_split(_vq, len, ctx);
}
EXPORT_SYMBOL_GPL(virtqueue_get_buf_ctx);

void *virtqueue_get_buf(struct virtqueue *_vq, unsigned int *len)
{
	return virtqueue_get_buf_ctx(_vq, len, NULL);
}
EXPORT_SYMBOL_GPL(virtqueue_get_buf);
/**
 * virtqueue_disable_cb - disable callbacks
 * @_vq: the struct virtqueue we're talking about.
 *
 * Note that this is not necessarily synchronous, hence unreliable and only
 * useful as an optimization.
 *
 * Unlike other operations, this need not be serialized.
 */
void virtqueue_disable_cb(struct virtqueue *_vq)
{
	struct vring_virtqueue *vq = to_vvq(_vq);

	if (vq->packed_ring)
		virtqueue_disable_cb_packed(_vq);
	else
		virtqueue_disable_cb_split(_vq);
}
EXPORT_SYMBOL_GPL(virtqueue_disable_cb);

/**
 * virtqueue_enable_cb_prepare - restart callbacks after disable_cb
 * @_vq: the struct virtqueue we're talking about.
 *
 * This re-enables callbacks; it returns current queue state
 * in an opaque unsigned value. This value should be later tested by
 * virtqueue_poll, to detect a possible race between the driver checking for
 * more work, and enabling callbacks.
 *
 * Caller must ensure we don't call this with other virtqueue
 * operations at the same time (except where noted).
 */
unsigned virtqueue_enable_cb_prepare(struct virtqueue *_vq)
{
	struct vring_virtqueue *vq = to_vvq(_vq);

	return vq->packed_ring ? virtqueue_enable_cb_prepare_packed(_vq) :
				 virtqueue_enable_cb_prepare_split(_vq);
}
EXPORT_SYMBOL_GPL(virtqueue_enable_cb_prepare);

/**
 * virtqueue_poll - query pending used buffers
 * @_vq: the struct virtqueue we're talking about.
 * @last_used_idx: virtqueue state (from call to virtqueue_enable_cb_prepare).
 *
 * Returns "true" if there are pending used buffers in the queue.
 *
 * This does not need to be serialized.
 */
bool virtqueue_poll(struct virtqueue *_vq, unsigned last_used_idx)
{
	struct vring_virtqueue *vq = to_vvq(_vq);

	if (unlikely(vq->broken))
		return false;

	virtio_mb(vq->weak_barriers);
	return vq->packed_ring ? virtqueue_poll_packed(_vq, last_used_idx) :
				 virtqueue_poll_split(_vq, last_used_idx);
}
EXPORT_SYMBOL_GPL(virtqueue_poll);

/**
 * virtqueue_enable_cb - restart callbacks after disable_cb.
 * @_vq: the struct virtqueue we're talking about.
 *
 * This re-enables callbacks; it returns "false" if there are pending
 * buffers in the queue, to detect a possible race between the driver
 * checking for more work, and enabling callbacks.
 *
 * Caller must ensure we don't call this with other virtqueue
 * operations at the same time (except where noted).
 */
bool virtqueue_enable_cb(struct virtqueue *_vq)
{
	unsigned last_used_idx = virtqueue_enable_cb_prepare(_vq);

	return !virtqueue_poll(_vq, last_used_idx);
}
EXPORT_SYMBOL_GPL(virtqueue_enable_cb);

/**
 * virtqueue_enable_cb_delayed - restart callbacks after disable_cb.
 * @_vq: the struct virtqueue we're talking about.
 *
 * This re-enables callbacks but hints to the other side to delay
 * interrupts until most of the available buffers have been processed;
 * it returns "false" if there are many pending buffers in the queue,
 * to detect a possible race between the driver checking for more work,
 * and enabling callbacks.
 *
 * Caller must ensure we don't call this with other virtqueue
 * operations at the same time (except where noted).
 */
bool virtqueue_enable_cb_delayed(struct virtqueue *_vq)
{
	struct vring_virtqueue *vq = to_vvq(_vq);

	return vq->packed_ring ? virtqueue_enable_cb_delayed_packed(_vq) :
				 virtqueue_enable_cb_delayed_split(_vq);
}
EXPORT_SYMBOL_GPL(virtqueue_enable_cb_delayed);

/**
 * virtqueue_detach_unused_buf - detach first unused buffer
 * @_vq: the struct virtqueue we're talking about.
 *
 * Returns NULL or the "data" token handed to virtqueue_add_*().
 * This is not valid on an active queue; it is useful only for device
 * shutdown.
 */
void *virtqueue_detach_unused_buf(struct virtqueue *_vq)
{
	struct vring_virtqueue *vq = to_vvq(_vq);

	return vq->packed_ring ? virtqueue_detach_unused_buf_packed(_vq) :
				 virtqueue_detach_unused_buf_split(_vq);
}
EXPORT_SYMBOL_GPL(virtqueue_detach_unused_buf);

static inline bool more_used(const struct vring_virtqueue *vq)
{
	return vq->packed_ring ? more_used_packed(vq) : more_used_split(vq);
}

irqreturn_t vring_interrupt(int irq, void *_vq)
{
	struct vring_virtqueue *vq = to_vvq(_vq);

	if (!more_used(vq)) {
		pr_debug("virtqueue interrupt with no work for %p\n", vq);
		return IRQ_NONE;
	}

	if (unlikely(vq->broken))
		return IRQ_HANDLED;

	pr_debug("virtqueue callback for %p (%p)\n", vq, vq->vq.callback);
	if (vq->vq.callback)
		vq->vq.callback(&vq->vq);

	return IRQ_HANDLED;
}
EXPORT_SYMBOL_GPL(vring_interrupt);

/* Only available for split ring */
struct virtqueue *__vring_new_virtqueue(unsigned int index,
					struct vring vring,
					struct virtio_device *vdev,
					bool weak_barriers,
					bool context,
					bool (*notify)(struct virtqueue *),
					void (*callback)(struct virtqueue *),
					const char *name)
{
	unsigned int i;
	struct vring_virtqueue *vq;

	if (virtio_has_feature(vdev, VIRTIO_F_RING_PACKED))
		return NULL;

	vq = kmalloc(sizeof(*vq), GFP_KERNEL);
	if (!vq)
		return NULL;

	vq->packed_ring = false;
	vq->vq.callback = callback;
	vq->vq.vdev = vdev;
	vq->vq.name = name;
	vq->vq.num_free = vring.num;
	vq->vq.index = index;
	vq->we_own_ring = false;
	vq->notify = notify;
	vq->weak_barriers = weak_barriers;
	vq->broken = false;
	vq->last_used_idx = 0;
	vq->num_added = 0;
	vq->use_dma_api = vring_use_dma_api(vdev);
#ifdef DEBUG
	vq->in_use = false;
	vq->last_add_time_valid = false;
#endif

	vq->indirect = virtio_has_feature(vdev, VIRTIO_RING_F_INDIRECT_DESC) &&
		!context;
	vq->event = virtio_has_feature(vdev, VIRTIO_RING_F_EVENT_IDX);

	if (virtio_has_feature(vdev, VIRTIO_F_ORDER_PLATFORM))
		vq->weak_barriers = false;

	vq->split.queue_dma_addr = 0;
	vq->split.queue_size_in_bytes = 0;

	vq->split.vring = vring;
	vq->split.avail_flags_shadow = 0;
	vq->split.avail_idx_shadow = 0;

	/* No callback?  Tell other side not to bother us. */
	if (!callback) {
		vq->split.avail_flags_shadow |= VRING_AVAIL_F_NO_INTERRUPT;
		if (!vq->event)
			vq->split.vring.avail->flags = cpu_to_virtio16(vdev,
					vq->split.avail_flags_shadow);
	}

	vq->split.desc_state = kmalloc_array(vring.num,
			sizeof(struct vring_desc_state_split), GFP_KERNEL);
	if (!vq->split.desc_state) {
		kfree(vq);
		return NULL;
	}

	/* Put everything in free lists. */
	vq->free_head = 0;
	for (i = 0; i < vring.num-1; i++)
		vq->split.vring.desc[i].next = cpu_to_virtio16(vdev, i + 1);
	memset(vq->split.desc_state, 0, vring.num *
			sizeof(struct vring_desc_state_split));

	list_add_tail(&vq->vq.list, &vdev->vqs);
	return &vq->vq;
}
EXPORT_SYMBOL_GPL(__vring_new_virtqueue);

struct virtqueue *vring_create_virtqueue(
	unsigned int index,
	unsigned int num,
	unsigned int vring_align,
	struct virtio_device *vdev,
	bool weak_barriers,
	bool may_reduce_num,
	bool context,
	bool (*notify)(struct virtqueue *),
	void (*callback)(struct virtqueue *),
	const char *name)
{

	if (virtio_has_feature(vdev, VIRTIO_F_RING_PACKED))
		return vring_create_virtqueue_packed(index, num, vring_align,
				vdev, weak_barriers, may_reduce_num,
				context, notify, callback, name);

	return vring_create_virtqueue_split(index, num, vring_align,
			vdev, weak_barriers, may_reduce_num,
			context, notify, callback, name);
}
EXPORT_SYMBOL_GPL(vring_create_virtqueue);

/* Only available for split ring */
struct virtqueue *vring_new_virtqueue(unsigned int index,
				      unsigned int num,
				      unsigned int vring_align,
				      struct virtio_device *vdev,
				      bool weak_barriers,
				      bool context,
				      void *pages,
				      bool (*notify)(struct virtqueue *vq),
				      void (*callback)(struct virtqueue *vq),
				      const char *name)
{
	struct vring vring;

	if (virtio_has_feature(vdev, VIRTIO_F_RING_PACKED))
		return NULL;

	vring_init(&vring, num, pages, vring_align);
	return __vring_new_virtqueue(index, vring, vdev, weak_barriers, context,
				     notify, callback, name);
}
EXPORT_SYMBOL_GPL(vring_new_virtqueue);

void vring_del_virtqueue(struct virtqueue *_vq)
{
	struct vring_virtqueue *vq = to_vvq(_vq);

	if (vq->we_own_ring) {
		if (vq->packed_ring) {
			vring_free_queue(vq->vq.vdev,
					 vq->packed.ring_size_in_bytes,
					 vq->packed.vring.desc,
					 vq->packed.ring_dma_addr);

			vring_free_queue(vq->vq.vdev,
					 vq->packed.event_size_in_bytes,
					 vq->packed.vring.driver,
					 vq->packed.driver_event_dma_addr);

			vring_free_queue(vq->vq.vdev,
					 vq->packed.event_size_in_bytes,
					 vq->packed.vring.device,
					 vq->packed.device_event_dma_addr);

			kfree(vq->packed.desc_state);
			kfree(vq->packed.desc_extra);
		} else {
			vring_free_queue(vq->vq.vdev,
					 vq->split.queue_size_in_bytes,
					 vq->split.vring.desc,
					 vq->split.queue_dma_addr);
		}
	}
	if (!vq->packed_ring)
		kfree(vq->split.desc_state);
	list_del(&_vq->list);
	kfree(vq);
}
EXPORT_SYMBOL_GPL(vring_del_virtqueue);

/* Manipulates transport-specific feature bits. */
void vring_transport_features(struct virtio_device *vdev)
{
	unsigned int i;

	for (i = VIRTIO_TRANSPORT_F_START; i < VIRTIO_TRANSPORT_F_END; i++) {
		switch (i) {
		case VIRTIO_RING_F_INDIRECT_DESC:
			break;
		case VIRTIO_RING_F_EVENT_IDX:
			break;
		case VIRTIO_F_VERSION_1:
			break;
		case VIRTIO_F_ACCESS_PLATFORM:
			break;
		case VIRTIO_F_RING_PACKED:
			break;
		case VIRTIO_F_ORDER_PLATFORM:
			break;
		default:
			/* We don't understand this bit. */
			__virtio_clear_bit(vdev, i);
		}
	}
}
EXPORT_SYMBOL_GPL(vring_transport_features);

/**
 * virtqueue_get_vring_size - return the size of the virtqueue's vring
 * @_vq: the struct virtqueue containing the vring of interest.
 *
 * Returns the size of the vring.  This is mainly used for boasting to
 * userspace.  Unlike other operations, this need not be serialized.
 */
unsigned int virtqueue_get_vring_size(struct virtqueue *_vq)
{

	struct vring_virtqueue *vq = to_vvq(_vq);

	return vq->packed_ring ? vq->packed.vring.num : vq->split.vring.num;
}
EXPORT_SYMBOL_GPL(virtqueue_get_vring_size);

bool virtqueue_is_broken(struct virtqueue *_vq)
{
	struct vring_virtqueue *vq = to_vvq(_vq);

	return vq->broken;
}
EXPORT_SYMBOL_GPL(virtqueue_is_broken);

/*
 * This should prevent the device from being used, allowing drivers to
 * recover.  You may need to grab appropriate locks to flush.
 */
void virtio_break_device(struct virtio_device *dev)
{
	struct virtqueue *_vq;

	list_for_each_entry(_vq, &dev->vqs, list) {
		struct vring_virtqueue *vq = to_vvq(_vq);
		vq->broken = true;
	}
}
EXPORT_SYMBOL_GPL(virtio_break_device);

dma_addr_t virtqueue_get_desc_addr(struct virtqueue *_vq)
{
	struct vring_virtqueue *vq = to_vvq(_vq);

	BUG_ON(!vq->we_own_ring);

	if (vq->packed_ring)
		return vq->packed.ring_dma_addr;

	return vq->split.queue_dma_addr;
}
EXPORT_SYMBOL_GPL(virtqueue_get_desc_addr);

dma_addr_t virtqueue_get_avail_addr(struct virtqueue *_vq)
{
	struct vring_virtqueue *vq = to_vvq(_vq);

	BUG_ON(!vq->we_own_ring);

	if (vq->packed_ring)
		return vq->packed.driver_event_dma_addr;

	return vq->split.queue_dma_addr +
		((char *)vq->split.vring.avail - (char *)vq->split.vring.desc);
}
EXPORT_SYMBOL_GPL(virtqueue_get_avail_addr);

dma_addr_t virtqueue_get_used_addr(struct virtqueue *_vq)
{
	struct vring_virtqueue *vq = to_vvq(_vq);

	BUG_ON(!vq->we_own_ring);

	if (vq->packed_ring)
		return vq->packed.device_event_dma_addr;

	return vq->split.queue_dma_addr +
		((char *)vq->split.vring.used - (char *)vq->split.vring.desc);
}
EXPORT_SYMBOL_GPL(virtqueue_get_used_addr);

/* Only available for split ring */
const struct vring *virtqueue_get_vring(struct virtqueue *vq)
{
	return &to_vvq(vq)->split.vring;
}
EXPORT_SYMBOL_GPL(virtqueue_get_vring);

MODULE_LICENSE("GPL");
