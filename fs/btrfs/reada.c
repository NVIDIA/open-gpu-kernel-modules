// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2011 STRATO.  All rights reserved.
 */

#include <linux/sched.h>
#include <linux/pagemap.h>
#include <linux/writeback.h>
#include <linux/blkdev.h>
#include <linux/slab.h>
#include <linux/workqueue.h>
#include "ctree.h"
#include "volumes.h"
#include "disk-io.h"
#include "transaction.h"
#include "dev-replace.h"
#include "block-group.h"

#undef DEBUG

/*
 * This is the implementation for the generic read ahead framework.
 *
 * To trigger a readahead, btrfs_reada_add must be called. It will start
 * a read ahead for the given range [start, end) on tree root. The returned
 * handle can either be used to wait on the readahead to finish
 * (btrfs_reada_wait), or to send it to the background (btrfs_reada_detach).
 *
 * The read ahead works as follows:
 * On btrfs_reada_add, the root of the tree is inserted into a radix_tree.
 * reada_start_machine will then search for extents to prefetch and trigger
 * some reads. When a read finishes for a node, all contained node/leaf
 * pointers that lie in the given range will also be enqueued. The reads will
 * be triggered in sequential order, thus giving a big win over a naive
 * enumeration. It will also make use of multi-device layouts. Each disk
 * will have its on read pointer and all disks will by utilized in parallel.
 * Also will no two disks read both sides of a mirror simultaneously, as this
 * would waste seeking capacity. Instead both disks will read different parts
 * of the filesystem.
 * Any number of readaheads can be started in parallel. The read order will be
 * determined globally, i.e. 2 parallel readaheads will normally finish faster
 * than the 2 started one after another.
 */

#define MAX_IN_FLIGHT 6

struct reada_extctl {
	struct list_head	list;
	struct reada_control	*rc;
	u64			generation;
};

struct reada_extent {
	u64			logical;
	u64			owner_root;
	struct btrfs_key	top;
	struct list_head	extctl;
	int 			refcnt;
	spinlock_t		lock;
	struct reada_zone	*zones[BTRFS_MAX_MIRRORS];
	int			nzones;
	int			scheduled;
	int			level;
};

struct reada_zone {
	u64			start;
	u64			end;
	u64			elems;
	struct list_head	list;
	spinlock_t		lock;
	int			locked;
	struct btrfs_device	*device;
	struct btrfs_device	*devs[BTRFS_MAX_MIRRORS]; /* full list, incl
							   * self */
	int			ndevs;
	struct kref		refcnt;
};

struct reada_machine_work {
	struct btrfs_work	work;
	struct btrfs_fs_info	*fs_info;
};

static void reada_extent_put(struct btrfs_fs_info *, struct reada_extent *);
static void reada_control_release(struct kref *kref);
static void reada_zone_release(struct kref *kref);
static void reada_start_machine(struct btrfs_fs_info *fs_info);
static void __reada_start_machine(struct btrfs_fs_info *fs_info);

static int reada_add_block(struct reada_control *rc, u64 logical,
			   struct btrfs_key *top, u64 owner_root,
			   u64 generation, int level);

/* recurses */
/* in case of err, eb might be NULL */
static void __readahead_hook(struct btrfs_fs_info *fs_info,
			     struct reada_extent *re, struct extent_buffer *eb,
			     int err)
{
	int nritems;
	int i;
	u64 bytenr;
	u64 generation;
	struct list_head list;

	spin_lock(&re->lock);
	/*
	 * just take the full list from the extent. afterwards we
	 * don't need the lock anymore
	 */
	list_replace_init(&re->extctl, &list);
	re->scheduled = 0;
	spin_unlock(&re->lock);

	/*
	 * this is the error case, the extent buffer has not been
	 * read correctly. We won't access anything from it and
	 * just cleanup our data structures. Effectively this will
	 * cut the branch below this node from read ahead.
	 */
	if (err)
		goto cleanup;

	/*
	 * FIXME: currently we just set nritems to 0 if this is a leaf,
	 * effectively ignoring the content. In a next step we could
	 * trigger more readahead depending from the content, e.g.
	 * fetch the checksums for the extents in the leaf.
	 */
	if (!btrfs_header_level(eb))
		goto cleanup;

	nritems = btrfs_header_nritems(eb);
	generation = btrfs_header_generation(eb);
	for (i = 0; i < nritems; i++) {
		struct reada_extctl *rec;
		u64 n_gen;
		struct btrfs_key key;
		struct btrfs_key next_key;

		btrfs_node_key_to_cpu(eb, &key, i);
		if (i + 1 < nritems)
			btrfs_node_key_to_cpu(eb, &next_key, i + 1);
		else
			next_key = re->top;
		bytenr = btrfs_node_blockptr(eb, i);
		n_gen = btrfs_node_ptr_generation(eb, i);

		list_for_each_entry(rec, &list, list) {
			struct reada_control *rc = rec->rc;

			/*
			 * if the generation doesn't match, just ignore this
			 * extctl. This will probably cut off a branch from
			 * prefetch. Alternatively one could start a new (sub-)
			 * prefetch for this branch, starting again from root.
			 * FIXME: move the generation check out of this loop
			 */
#ifdef DEBUG
			if (rec->generation != generation) {
				btrfs_debug(fs_info,
					    "generation mismatch for (%llu,%d,%llu) %llu != %llu",
					    key.objectid, key.type, key.offset,
					    rec->generation, generation);
			}
#endif
			if (rec->generation == generation &&
			    btrfs_comp_cpu_keys(&key, &rc->key_end) < 0 &&
			    btrfs_comp_cpu_keys(&next_key, &rc->key_start) > 0)
				reada_add_block(rc, bytenr, &next_key,
						btrfs_header_owner(eb), n_gen,
						btrfs_header_level(eb) - 1);
		}
	}

cleanup:
	/*
	 * free extctl records
	 */
	while (!list_empty(&list)) {
		struct reada_control *rc;
		struct reada_extctl *rec;

		rec = list_first_entry(&list, struct reada_extctl, list);
		list_del(&rec->list);
		rc = rec->rc;
		kfree(rec);

		kref_get(&rc->refcnt);
		if (atomic_dec_and_test(&rc->elems)) {
			kref_put(&rc->refcnt, reada_control_release);
			wake_up(&rc->wait);
		}
		kref_put(&rc->refcnt, reada_control_release);

		reada_extent_put(fs_info, re);	/* one ref for each entry */
	}

	return;
}

int btree_readahead_hook(struct extent_buffer *eb, int err)
{
	struct btrfs_fs_info *fs_info = eb->fs_info;
	int ret = 0;
	struct reada_extent *re;

	/* find extent */
	spin_lock(&fs_info->reada_lock);
	re = radix_tree_lookup(&fs_info->reada_tree,
			       eb->start >> fs_info->sectorsize_bits);
	if (re)
		re->refcnt++;
	spin_unlock(&fs_info->reada_lock);
	if (!re) {
		ret = -1;
		goto start_machine;
	}

	__readahead_hook(fs_info, re, eb, err);
	reada_extent_put(fs_info, re);	/* our ref */

start_machine:
	reada_start_machine(fs_info);
	return ret;
}

static struct reada_zone *reada_find_zone(struct btrfs_device *dev, u64 logical,
					  struct btrfs_bio *bbio)
{
	struct btrfs_fs_info *fs_info = dev->fs_info;
	int ret;
	struct reada_zone *zone;
	struct btrfs_block_group *cache = NULL;
	u64 start;
	u64 end;
	int i;

	zone = NULL;
	spin_lock(&fs_info->reada_lock);
	ret = radix_tree_gang_lookup(&dev->reada_zones, (void **)&zone,
				     logical >> fs_info->sectorsize_bits, 1);
	if (ret == 1 && logical >= zone->start && logical <= zone->end) {
		kref_get(&zone->refcnt);
		spin_unlock(&fs_info->reada_lock);
		return zone;
	}

	spin_unlock(&fs_info->reada_lock);

	cache = btrfs_lookup_block_group(fs_info, logical);
	if (!cache)
		return NULL;

	start = cache->start;
	end = start + cache->length - 1;
	btrfs_put_block_group(cache);

	zone = kzalloc(sizeof(*zone), GFP_KERNEL);
	if (!zone)
		return NULL;

	ret = radix_tree_preload(GFP_KERNEL);
	if (ret) {
		kfree(zone);
		return NULL;
	}

	zone->start = start;
	zone->end = end;
	INIT_LIST_HEAD(&zone->list);
	spin_lock_init(&zone->lock);
	zone->locked = 0;
	kref_init(&zone->refcnt);
	zone->elems = 0;
	zone->device = dev; /* our device always sits at index 0 */
	for (i = 0; i < bbio->num_stripes; ++i) {
		/* bounds have already been checked */
		zone->devs[i] = bbio->stripes[i].dev;
	}
	zone->ndevs = bbio->num_stripes;

	spin_lock(&fs_info->reada_lock);
	ret = radix_tree_insert(&dev->reada_zones,
			(unsigned long)(zone->end >> fs_info->sectorsize_bits),
			zone);

	if (ret == -EEXIST) {
		kfree(zone);
		ret = radix_tree_gang_lookup(&dev->reada_zones, (void **)&zone,
					logical >> fs_info->sectorsize_bits, 1);
		if (ret == 1 && logical >= zone->start && logical <= zone->end)
			kref_get(&zone->refcnt);
		else
			zone = NULL;
	}
	spin_unlock(&fs_info->reada_lock);
	radix_tree_preload_end();

	return zone;
}

static struct reada_extent *reada_find_extent(struct btrfs_fs_info *fs_info,
					      u64 logical,
					      struct btrfs_key *top,
					      u64 owner_root, int level)
{
	int ret;
	struct reada_extent *re = NULL;
	struct reada_extent *re_exist = NULL;
	struct btrfs_bio *bbio = NULL;
	struct btrfs_device *dev;
	struct btrfs_device *prev_dev;
	u64 length;
	int real_stripes;
	int nzones = 0;
	unsigned long index = logical >> fs_info->sectorsize_bits;
	int dev_replace_is_ongoing;
	int have_zone = 0;

	spin_lock(&fs_info->reada_lock);
	re = radix_tree_lookup(&fs_info->reada_tree, index);
	if (re)
		re->refcnt++;
	spin_unlock(&fs_info->reada_lock);

	if (re)
		return re;

	re = kzalloc(sizeof(*re), GFP_KERNEL);
	if (!re)
		return NULL;

	re->logical = logical;
	re->top = *top;
	INIT_LIST_HEAD(&re->extctl);
	spin_lock_init(&re->lock);
	re->refcnt = 1;
	re->owner_root = owner_root;
	re->level = level;

	/*
	 * map block
	 */
	length = fs_info->nodesize;
	ret = btrfs_map_block(fs_info, BTRFS_MAP_GET_READ_MIRRORS, logical,
			&length, &bbio, 0);
	if (ret || !bbio || length < fs_info->nodesize)
		goto error;

	if (bbio->num_stripes > BTRFS_MAX_MIRRORS) {
		btrfs_err(fs_info,
			   "readahead: more than %d copies not supported",
			   BTRFS_MAX_MIRRORS);
		goto error;
	}

	real_stripes = bbio->num_stripes - bbio->num_tgtdevs;
	for (nzones = 0; nzones < real_stripes; ++nzones) {
		struct reada_zone *zone;

		dev = bbio->stripes[nzones].dev;

		/* cannot read ahead on missing device. */
		if (!dev->bdev)
			continue;

		zone = reada_find_zone(dev, logical, bbio);
		if (!zone)
			continue;

		re->zones[re->nzones++] = zone;
		spin_lock(&zone->lock);
		if (!zone->elems)
			kref_get(&zone->refcnt);
		++zone->elems;
		spin_unlock(&zone->lock);
		spin_lock(&fs_info->reada_lock);
		kref_put(&zone->refcnt, reada_zone_release);
		spin_unlock(&fs_info->reada_lock);
	}
	if (re->nzones == 0) {
		/* not a single zone found, error and out */
		goto error;
	}

	/* Insert extent in reada tree + all per-device trees, all or nothing */
	down_read(&fs_info->dev_replace.rwsem);
	ret = radix_tree_preload(GFP_KERNEL);
	if (ret) {
		up_read(&fs_info->dev_replace.rwsem);
		goto error;
	}

	spin_lock(&fs_info->reada_lock);
	ret = radix_tree_insert(&fs_info->reada_tree, index, re);
	if (ret == -EEXIST) {
		re_exist = radix_tree_lookup(&fs_info->reada_tree, index);
		re_exist->refcnt++;
		spin_unlock(&fs_info->reada_lock);
		radix_tree_preload_end();
		up_read(&fs_info->dev_replace.rwsem);
		goto error;
	}
	if (ret) {
		spin_unlock(&fs_info->reada_lock);
		radix_tree_preload_end();
		up_read(&fs_info->dev_replace.rwsem);
		goto error;
	}
	radix_tree_preload_end();
	prev_dev = NULL;
	dev_replace_is_ongoing = btrfs_dev_replace_is_ongoing(
			&fs_info->dev_replace);
	for (nzones = 0; nzones < re->nzones; ++nzones) {
		dev = re->zones[nzones]->device;

		if (dev == prev_dev) {
			/*
			 * in case of DUP, just add the first zone. As both
			 * are on the same device, there's nothing to gain
			 * from adding both.
			 * Also, it wouldn't work, as the tree is per device
			 * and adding would fail with EEXIST
			 */
			continue;
		}
		if (!dev->bdev)
			continue;

		if (test_bit(BTRFS_DEV_STATE_NO_READA, &dev->dev_state))
			continue;

		if (dev_replace_is_ongoing &&
		    dev == fs_info->dev_replace.tgtdev) {
			/*
			 * as this device is selected for reading only as
			 * a last resort, skip it for read ahead.
			 */
			continue;
		}
		prev_dev = dev;
		ret = radix_tree_insert(&dev->reada_extents, index, re);
		if (ret) {
			while (--nzones >= 0) {
				dev = re->zones[nzones]->device;
				BUG_ON(dev == NULL);
				/* ignore whether the entry was inserted */
				radix_tree_delete(&dev->reada_extents, index);
			}
			radix_tree_delete(&fs_info->reada_tree, index);
			spin_unlock(&fs_info->reada_lock);
			up_read(&fs_info->dev_replace.rwsem);
			goto error;
		}
		have_zone = 1;
	}
	if (!have_zone)
		radix_tree_delete(&fs_info->reada_tree, index);
	spin_unlock(&fs_info->reada_lock);
	up_read(&fs_info->dev_replace.rwsem);

	if (!have_zone)
		goto error;

	btrfs_put_bbio(bbio);
	return re;

error:
	for (nzones = 0; nzones < re->nzones; ++nzones) {
		struct reada_zone *zone;

		zone = re->zones[nzones];
		kref_get(&zone->refcnt);
		spin_lock(&zone->lock);
		--zone->elems;
		if (zone->elems == 0) {
			/*
			 * no fs_info->reada_lock needed, as this can't be
			 * the last ref
			 */
			kref_put(&zone->refcnt, reada_zone_release);
		}
		spin_unlock(&zone->lock);

		spin_lock(&fs_info->reada_lock);
		kref_put(&zone->refcnt, reada_zone_release);
		spin_unlock(&fs_info->reada_lock);
	}
	btrfs_put_bbio(bbio);
	kfree(re);
	return re_exist;
}

static void reada_extent_put(struct btrfs_fs_info *fs_info,
			     struct reada_extent *re)
{
	int i;
	unsigned long index = re->logical >> fs_info->sectorsize_bits;

	spin_lock(&fs_info->reada_lock);
	if (--re->refcnt) {
		spin_unlock(&fs_info->reada_lock);
		return;
	}

	radix_tree_delete(&fs_info->reada_tree, index);
	for (i = 0; i < re->nzones; ++i) {
		struct reada_zone *zone = re->zones[i];

		radix_tree_delete(&zone->device->reada_extents, index);
	}

	spin_unlock(&fs_info->reada_lock);

	for (i = 0; i < re->nzones; ++i) {
		struct reada_zone *zone = re->zones[i];

		kref_get(&zone->refcnt);
		spin_lock(&zone->lock);
		--zone->elems;
		if (zone->elems == 0) {
			/* no fs_info->reada_lock needed, as this can't be
			 * the last ref */
			kref_put(&zone->refcnt, reada_zone_release);
		}
		spin_unlock(&zone->lock);

		spin_lock(&fs_info->reada_lock);
		kref_put(&zone->refcnt, reada_zone_release);
		spin_unlock(&fs_info->reada_lock);
	}

	kfree(re);
}

static void reada_zone_release(struct kref *kref)
{
	struct reada_zone *zone = container_of(kref, struct reada_zone, refcnt);
	struct btrfs_fs_info *fs_info = zone->device->fs_info;

	lockdep_assert_held(&fs_info->reada_lock);

	radix_tree_delete(&zone->device->reada_zones,
			  zone->end >> fs_info->sectorsize_bits);

	kfree(zone);
}

static void reada_control_release(struct kref *kref)
{
	struct reada_control *rc = container_of(kref, struct reada_control,
						refcnt);

	kfree(rc);
}

static int reada_add_block(struct reada_control *rc, u64 logical,
			   struct btrfs_key *top, u64 owner_root,
			   u64 generation, int level)
{
	struct btrfs_fs_info *fs_info = rc->fs_info;
	struct reada_extent *re;
	struct reada_extctl *rec;

	/* takes one ref */
	re = reada_find_extent(fs_info, logical, top, owner_root, level);
	if (!re)
		return -1;

	rec = kzalloc(sizeof(*rec), GFP_KERNEL);
	if (!rec) {
		reada_extent_put(fs_info, re);
		return -ENOMEM;
	}

	rec->rc = rc;
	rec->generation = generation;
	atomic_inc(&rc->elems);

	spin_lock(&re->lock);
	list_add_tail(&rec->list, &re->extctl);
	spin_unlock(&re->lock);

	/* leave the ref on the extent */

	return 0;
}

/*
 * called with fs_info->reada_lock held
 */
static void reada_peer_zones_set_lock(struct reada_zone *zone, int lock)
{
	int i;
	unsigned long index = zone->end >> zone->device->fs_info->sectorsize_bits;

	for (i = 0; i < zone->ndevs; ++i) {
		struct reada_zone *peer;
		peer = radix_tree_lookup(&zone->devs[i]->reada_zones, index);
		if (peer && peer->device != zone->device)
			peer->locked = lock;
	}
}

/*
 * called with fs_info->reada_lock held
 */
static int reada_pick_zone(struct btrfs_device *dev)
{
	struct reada_zone *top_zone = NULL;
	struct reada_zone *top_locked_zone = NULL;
	u64 top_elems = 0;
	u64 top_locked_elems = 0;
	unsigned long index = 0;
	int ret;

	if (dev->reada_curr_zone) {
		reada_peer_zones_set_lock(dev->reada_curr_zone, 0);
		kref_put(&dev->reada_curr_zone->refcnt, reada_zone_release);
		dev->reada_curr_zone = NULL;
	}
	/* pick the zone with the most elements */
	while (1) {
		struct reada_zone *zone;

		ret = radix_tree_gang_lookup(&dev->reada_zones,
					     (void **)&zone, index, 1);
		if (ret == 0)
			break;
		index = (zone->end >> dev->fs_info->sectorsize_bits) + 1;
		if (zone->locked) {
			if (zone->elems > top_locked_elems) {
				top_locked_elems = zone->elems;
				top_locked_zone = zone;
			}
		} else {
			if (zone->elems > top_elems) {
				top_elems = zone->elems;
				top_zone = zone;
			}
		}
	}
	if (top_zone)
		dev->reada_curr_zone = top_zone;
	else if (top_locked_zone)
		dev->reada_curr_zone = top_locked_zone;
	else
		return 0;

	dev->reada_next = dev->reada_curr_zone->start;
	kref_get(&dev->reada_curr_zone->refcnt);
	reada_peer_zones_set_lock(dev->reada_curr_zone, 1);

	return 1;
}

static int reada_tree_block_flagged(struct btrfs_fs_info *fs_info, u64 bytenr,
				    u64 owner_root, int level, int mirror_num,
				    struct extent_buffer **eb)
{
	struct extent_buffer *buf = NULL;
	int ret;

	buf = btrfs_find_create_tree_block(fs_info, bytenr, owner_root, level);
	if (IS_ERR(buf))
		return 0;

	set_bit(EXTENT_BUFFER_READAHEAD, &buf->bflags);

	ret = read_extent_buffer_pages(buf, WAIT_PAGE_LOCK, mirror_num);
	if (ret) {
		free_extent_buffer_stale(buf);
		return ret;
	}

	if (test_bit(EXTENT_BUFFER_CORRUPT, &buf->bflags)) {
		free_extent_buffer_stale(buf);
		return -EIO;
	} else if (extent_buffer_uptodate(buf)) {
		*eb = buf;
	} else {
		free_extent_buffer(buf);
	}
	return 0;
}

static int reada_start_machine_dev(struct btrfs_device *dev)
{
	struct btrfs_fs_info *fs_info = dev->fs_info;
	struct reada_extent *re = NULL;
	int mirror_num = 0;
	struct extent_buffer *eb = NULL;
	u64 logical;
	int ret;
	int i;

	spin_lock(&fs_info->reada_lock);
	if (dev->reada_curr_zone == NULL) {
		ret = reada_pick_zone(dev);
		if (!ret) {
			spin_unlock(&fs_info->reada_lock);
			return 0;
		}
	}
	/*
	 * FIXME currently we issue the reads one extent at a time. If we have
	 * a contiguous block of extents, we could also coagulate them or use
	 * plugging to speed things up
	 */
	ret = radix_tree_gang_lookup(&dev->reada_extents, (void **)&re,
				dev->reada_next >> fs_info->sectorsize_bits, 1);
	if (ret == 0 || re->logical > dev->reada_curr_zone->end) {
		ret = reada_pick_zone(dev);
		if (!ret) {
			spin_unlock(&fs_info->reada_lock);
			return 0;
		}
		re = NULL;
		ret = radix_tree_gang_lookup(&dev->reada_extents, (void **)&re,
				dev->reada_next >> fs_info->sectorsize_bits, 1);
	}
	if (ret == 0) {
		spin_unlock(&fs_info->reada_lock);
		return 0;
	}
	dev->reada_next = re->logical + fs_info->nodesize;
	re->refcnt++;

	spin_unlock(&fs_info->reada_lock);

	spin_lock(&re->lock);
	if (re->scheduled || list_empty(&re->extctl)) {
		spin_unlock(&re->lock);
		reada_extent_put(fs_info, re);
		return 0;
	}
	re->scheduled = 1;
	spin_unlock(&re->lock);

	/*
	 * find mirror num
	 */
	for (i = 0; i < re->nzones; ++i) {
		if (re->zones[i]->device == dev) {
			mirror_num = i + 1;
			break;
		}
	}
	logical = re->logical;

	atomic_inc(&dev->reada_in_flight);
	ret = reada_tree_block_flagged(fs_info, logical, re->owner_root,
				       re->level, mirror_num, &eb);
	if (ret)
		__readahead_hook(fs_info, re, NULL, ret);
	else if (eb)
		__readahead_hook(fs_info, re, eb, ret);

	if (eb)
		free_extent_buffer(eb);

	atomic_dec(&dev->reada_in_flight);
	reada_extent_put(fs_info, re);

	return 1;

}

static void reada_start_machine_worker(struct btrfs_work *work)
{
	struct reada_machine_work *rmw;
	int old_ioprio;

	rmw = container_of(work, struct reada_machine_work, work);

	old_ioprio = IOPRIO_PRIO_VALUE(task_nice_ioclass(current),
				       task_nice_ioprio(current));
	set_task_ioprio(current, BTRFS_IOPRIO_READA);
	__reada_start_machine(rmw->fs_info);
	set_task_ioprio(current, old_ioprio);

	atomic_dec(&rmw->fs_info->reada_works_cnt);

	kfree(rmw);
}

/* Try to start up to 10k READA requests for a group of devices */
static int reada_start_for_fsdevs(struct btrfs_fs_devices *fs_devices)
{
	u64 enqueued;
	u64 total = 0;
	struct btrfs_device *device;

	do {
		enqueued = 0;
		list_for_each_entry(device, &fs_devices->devices, dev_list) {
			if (atomic_read(&device->reada_in_flight) <
			    MAX_IN_FLIGHT)
				enqueued += reada_start_machine_dev(device);
		}
		total += enqueued;
	} while (enqueued && total < 10000);

	return total;
}

static void __reada_start_machine(struct btrfs_fs_info *fs_info)
{
	struct btrfs_fs_devices *fs_devices = fs_info->fs_devices, *seed_devs;
	int i;
	u64 enqueued = 0;

	mutex_lock(&fs_devices->device_list_mutex);

	enqueued += reada_start_for_fsdevs(fs_devices);
	list_for_each_entry(seed_devs, &fs_devices->seed_list, seed_list)
		enqueued += reada_start_for_fsdevs(seed_devs);

	mutex_unlock(&fs_devices->device_list_mutex);
	if (enqueued == 0)
		return;

	/*
	 * If everything is already in the cache, this is effectively single
	 * threaded. To a) not hold the caller for too long and b) to utilize
	 * more cores, we broke the loop above after 10000 iterations and now
	 * enqueue to workers to finish it. This will distribute the load to
	 * the cores.
	 */
	for (i = 0; i < 2; ++i) {
		reada_start_machine(fs_info);
		if (atomic_read(&fs_info->reada_works_cnt) >
		    BTRFS_MAX_MIRRORS * 2)
			break;
	}
}

static void reada_start_machine(struct btrfs_fs_info *fs_info)
{
	struct reada_machine_work *rmw;

	rmw = kzalloc(sizeof(*rmw), GFP_KERNEL);
	if (!rmw) {
		/* FIXME we cannot handle this properly right now */
		BUG();
	}
	btrfs_init_work(&rmw->work, reada_start_machine_worker, NULL, NULL);
	rmw->fs_info = fs_info;

	btrfs_queue_work(fs_info->readahead_workers, &rmw->work);
	atomic_inc(&fs_info->reada_works_cnt);
}

#ifdef DEBUG
static void dump_devs(struct btrfs_fs_info *fs_info, int all)
{
	struct btrfs_device *device;
	struct btrfs_fs_devices *fs_devices = fs_info->fs_devices;
	unsigned long index;
	int ret;
	int i;
	int j;
	int cnt;

	spin_lock(&fs_info->reada_lock);
	list_for_each_entry(device, &fs_devices->devices, dev_list) {
		btrfs_debug(fs_info, "dev %lld has %d in flight", device->devid,
			atomic_read(&device->reada_in_flight));
		index = 0;
		while (1) {
			struct reada_zone *zone;
			ret = radix_tree_gang_lookup(&device->reada_zones,
						     (void **)&zone, index, 1);
			if (ret == 0)
				break;
			pr_debug("  zone %llu-%llu elems %llu locked %d devs",
				    zone->start, zone->end, zone->elems,
				    zone->locked);
			for (j = 0; j < zone->ndevs; ++j) {
				pr_cont(" %lld",
					zone->devs[j]->devid);
			}
			if (device->reada_curr_zone == zone)
				pr_cont(" curr off %llu",
					device->reada_next - zone->start);
			pr_cont("\n");
			index = (zone->end >> fs_info->sectorsize_bits) + 1;
		}
		cnt = 0;
		index = 0;
		while (all) {
			struct reada_extent *re = NULL;

			ret = radix_tree_gang_lookup(&device->reada_extents,
						     (void **)&re, index, 1);
			if (ret == 0)
				break;
			pr_debug("  re: logical %llu size %u empty %d scheduled %d",
				re->logical, fs_info->nodesize,
				list_empty(&re->extctl), re->scheduled);

			for (i = 0; i < re->nzones; ++i) {
				pr_cont(" zone %llu-%llu devs",
					re->zones[i]->start,
					re->zones[i]->end);
				for (j = 0; j < re->zones[i]->ndevs; ++j) {
					pr_cont(" %lld",
						re->zones[i]->devs[j]->devid);
				}
			}
			pr_cont("\n");
			index = (re->logical >> fs_info->sectorsize_bits) + 1;
			if (++cnt > 15)
				break;
		}
	}

	index = 0;
	cnt = 0;
	while (all) {
		struct reada_extent *re = NULL;

		ret = radix_tree_gang_lookup(&fs_info->reada_tree, (void **)&re,
					     index, 1);
		if (ret == 0)
			break;
		if (!re->scheduled) {
			index = (re->logical >> fs_info->sectorsize_bits) + 1;
			continue;
		}
		pr_debug("re: logical %llu size %u list empty %d scheduled %d",
			re->logical, fs_info->nodesize,
			list_empty(&re->extctl), re->scheduled);
		for (i = 0; i < re->nzones; ++i) {
			pr_cont(" zone %llu-%llu devs",
				re->zones[i]->start,
				re->zones[i]->end);
			for (j = 0; j < re->zones[i]->ndevs; ++j) {
				pr_cont(" %lld",
				       re->zones[i]->devs[j]->devid);
			}
		}
		pr_cont("\n");
		index = (re->logical >> fs_info->sectorsize_bits) + 1;
	}
	spin_unlock(&fs_info->reada_lock);
}
#endif

/*
 * interface
 */
struct reada_control *btrfs_reada_add(struct btrfs_root *root,
			struct btrfs_key *key_start, struct btrfs_key *key_end)
{
	struct reada_control *rc;
	u64 start;
	u64 generation;
	int ret;
	int level;
	struct extent_buffer *node;
	static struct btrfs_key max_key = {
		.objectid = (u64)-1,
		.type = (u8)-1,
		.offset = (u64)-1
	};

	rc = kzalloc(sizeof(*rc), GFP_KERNEL);
	if (!rc)
		return ERR_PTR(-ENOMEM);

	rc->fs_info = root->fs_info;
	rc->key_start = *key_start;
	rc->key_end = *key_end;
	atomic_set(&rc->elems, 0);
	init_waitqueue_head(&rc->wait);
	kref_init(&rc->refcnt);
	kref_get(&rc->refcnt); /* one ref for having elements */

	node = btrfs_root_node(root);
	start = node->start;
	generation = btrfs_header_generation(node);
	level = btrfs_header_level(node);
	free_extent_buffer(node);

	ret = reada_add_block(rc, start, &max_key, root->root_key.objectid,
			      generation, level);
	if (ret) {
		kfree(rc);
		return ERR_PTR(ret);
	}

	reada_start_machine(root->fs_info);

	return rc;
}

#ifdef DEBUG
int btrfs_reada_wait(void *handle)
{
	struct reada_control *rc = handle;
	struct btrfs_fs_info *fs_info = rc->fs_info;

	while (atomic_read(&rc->elems)) {
		if (!atomic_read(&fs_info->reada_works_cnt))
			reada_start_machine(fs_info);
		wait_event_timeout(rc->wait, atomic_read(&rc->elems) == 0,
				   5 * HZ);
		dump_devs(fs_info, atomic_read(&rc->elems) < 10 ? 1 : 0);
	}

	dump_devs(fs_info, atomic_read(&rc->elems) < 10 ? 1 : 0);

	kref_put(&rc->refcnt, reada_control_release);

	return 0;
}
#else
int btrfs_reada_wait(void *handle)
{
	struct reada_control *rc = handle;
	struct btrfs_fs_info *fs_info = rc->fs_info;

	while (atomic_read(&rc->elems)) {
		if (!atomic_read(&fs_info->reada_works_cnt))
			reada_start_machine(fs_info);
		wait_event_timeout(rc->wait, atomic_read(&rc->elems) == 0,
				   (HZ + 9) / 10);
	}

	kref_put(&rc->refcnt, reada_control_release);

	return 0;
}
#endif

void btrfs_reada_detach(void *handle)
{
	struct reada_control *rc = handle;

	kref_put(&rc->refcnt, reada_control_release);
}

/*
 * Before removing a device (device replace or device remove ioctls), call this
 * function to wait for all existing readahead requests on the device and to
 * make sure no one queues more readahead requests for the device.
 *
 * Must be called without holding neither the device list mutex nor the device
 * replace semaphore, otherwise it will deadlock.
 */
void btrfs_reada_remove_dev(struct btrfs_device *dev)
{
	struct btrfs_fs_info *fs_info = dev->fs_info;

	/* Serialize with readahead extent creation at reada_find_extent(). */
	spin_lock(&fs_info->reada_lock);
	set_bit(BTRFS_DEV_STATE_NO_READA, &dev->dev_state);
	spin_unlock(&fs_info->reada_lock);

	/*
	 * There might be readahead requests added to the radix trees which
	 * were not yet added to the readahead work queue. We need to start
	 * them and wait for their completion, otherwise we can end up with
	 * use-after-free problems when dropping the last reference on the
	 * readahead extents and their zones, as they need to access the
	 * device structure.
	 */
	reada_start_machine(fs_info);
	btrfs_flush_workqueue(fs_info->readahead_workers);
}

/*
 * If when removing a device (device replace or device remove ioctls) an error
 * happens after calling btrfs_reada_remove_dev(), call this to undo what that
 * function did. This is safe to call even if btrfs_reada_remove_dev() was not
 * called before.
 */
void btrfs_reada_undo_remove_dev(struct btrfs_device *dev)
{
	spin_lock(&dev->fs_info->reada_lock);
	clear_bit(BTRFS_DEV_STATE_NO_READA, &dev->dev_state);
	spin_unlock(&dev->fs_info->reada_lock);
}
