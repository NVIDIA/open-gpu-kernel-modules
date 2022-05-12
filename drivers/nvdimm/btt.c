// SPDX-License-Identifier: GPL-2.0-only
/*
 * Block Translation Table
 * Copyright (c) 2014-2015, Intel Corporation.
 */
#include <linux/highmem.h>
#include <linux/debugfs.h>
#include <linux/blkdev.h>
#include <linux/pagemap.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/mutex.h>
#include <linux/hdreg.h>
#include <linux/genhd.h>
#include <linux/sizes.h>
#include <linux/ndctl.h>
#include <linux/fs.h>
#include <linux/nd.h>
#include <linux/backing-dev.h>
#include "btt.h"
#include "nd.h"

enum log_ent_request {
	LOG_NEW_ENT = 0,
	LOG_OLD_ENT
};

static struct device *to_dev(struct arena_info *arena)
{
	return &arena->nd_btt->dev;
}

static u64 adjust_initial_offset(struct nd_btt *nd_btt, u64 offset)
{
	return offset + nd_btt->initial_offset;
}

static int arena_read_bytes(struct arena_info *arena, resource_size_t offset,
		void *buf, size_t n, unsigned long flags)
{
	struct nd_btt *nd_btt = arena->nd_btt;
	struct nd_namespace_common *ndns = nd_btt->ndns;

	/* arena offsets may be shifted from the base of the device */
	offset = adjust_initial_offset(nd_btt, offset);
	return nvdimm_read_bytes(ndns, offset, buf, n, flags);
}

static int arena_write_bytes(struct arena_info *arena, resource_size_t offset,
		void *buf, size_t n, unsigned long flags)
{
	struct nd_btt *nd_btt = arena->nd_btt;
	struct nd_namespace_common *ndns = nd_btt->ndns;

	/* arena offsets may be shifted from the base of the device */
	offset = adjust_initial_offset(nd_btt, offset);
	return nvdimm_write_bytes(ndns, offset, buf, n, flags);
}

static int btt_info_write(struct arena_info *arena, struct btt_sb *super)
{
	int ret;

	/*
	 * infooff and info2off should always be at least 512B aligned.
	 * We rely on that to make sure rw_bytes does error clearing
	 * correctly, so make sure that is the case.
	 */
	dev_WARN_ONCE(to_dev(arena), !IS_ALIGNED(arena->infooff, 512),
		"arena->infooff: %#llx is unaligned\n", arena->infooff);
	dev_WARN_ONCE(to_dev(arena), !IS_ALIGNED(arena->info2off, 512),
		"arena->info2off: %#llx is unaligned\n", arena->info2off);

	ret = arena_write_bytes(arena, arena->info2off, super,
			sizeof(struct btt_sb), 0);
	if (ret)
		return ret;

	return arena_write_bytes(arena, arena->infooff, super,
			sizeof(struct btt_sb), 0);
}

static int btt_info_read(struct arena_info *arena, struct btt_sb *super)
{
	return arena_read_bytes(arena, arena->infooff, super,
			sizeof(struct btt_sb), 0);
}

/*
 * 'raw' version of btt_map write
 * Assumptions:
 *   mapping is in little-endian
 *   mapping contains 'E' and 'Z' flags as desired
 */
static int __btt_map_write(struct arena_info *arena, u32 lba, __le32 mapping,
		unsigned long flags)
{
	u64 ns_off = arena->mapoff + (lba * MAP_ENT_SIZE);

	if (unlikely(lba >= arena->external_nlba))
		dev_err_ratelimited(to_dev(arena),
			"%s: lba %#x out of range (max: %#x)\n",
			__func__, lba, arena->external_nlba);
	return arena_write_bytes(arena, ns_off, &mapping, MAP_ENT_SIZE, flags);
}

static int btt_map_write(struct arena_info *arena, u32 lba, u32 mapping,
			u32 z_flag, u32 e_flag, unsigned long rwb_flags)
{
	u32 ze;
	__le32 mapping_le;

	/*
	 * This 'mapping' is supposed to be just the LBA mapping, without
	 * any flags set, so strip the flag bits.
	 */
	mapping = ent_lba(mapping);

	ze = (z_flag << 1) + e_flag;
	switch (ze) {
	case 0:
		/*
		 * We want to set neither of the Z or E flags, and
		 * in the actual layout, this means setting the bit
		 * positions of both to '1' to indicate a 'normal'
		 * map entry
		 */
		mapping |= MAP_ENT_NORMAL;
		break;
	case 1:
		mapping |= (1 << MAP_ERR_SHIFT);
		break;
	case 2:
		mapping |= (1 << MAP_TRIM_SHIFT);
		break;
	default:
		/*
		 * The case where Z and E are both sent in as '1' could be
		 * construed as a valid 'normal' case, but we decide not to,
		 * to avoid confusion
		 */
		dev_err_ratelimited(to_dev(arena),
			"Invalid use of Z and E flags\n");
		return -EIO;
	}

	mapping_le = cpu_to_le32(mapping);
	return __btt_map_write(arena, lba, mapping_le, rwb_flags);
}

static int btt_map_read(struct arena_info *arena, u32 lba, u32 *mapping,
			int *trim, int *error, unsigned long rwb_flags)
{
	int ret;
	__le32 in;
	u32 raw_mapping, postmap, ze, z_flag, e_flag;
	u64 ns_off = arena->mapoff + (lba * MAP_ENT_SIZE);

	if (unlikely(lba >= arena->external_nlba))
		dev_err_ratelimited(to_dev(arena),
			"%s: lba %#x out of range (max: %#x)\n",
			__func__, lba, arena->external_nlba);

	ret = arena_read_bytes(arena, ns_off, &in, MAP_ENT_SIZE, rwb_flags);
	if (ret)
		return ret;

	raw_mapping = le32_to_cpu(in);

	z_flag = ent_z_flag(raw_mapping);
	e_flag = ent_e_flag(raw_mapping);
	ze = (z_flag << 1) + e_flag;
	postmap = ent_lba(raw_mapping);

	/* Reuse the {z,e}_flag variables for *trim and *error */
	z_flag = 0;
	e_flag = 0;

	switch (ze) {
	case 0:
		/* Initial state. Return postmap = premap */
		*mapping = lba;
		break;
	case 1:
		*mapping = postmap;
		e_flag = 1;
		break;
	case 2:
		*mapping = postmap;
		z_flag = 1;
		break;
	case 3:
		*mapping = postmap;
		break;
	default:
		return -EIO;
	}

	if (trim)
		*trim = z_flag;
	if (error)
		*error = e_flag;

	return ret;
}

static int btt_log_group_read(struct arena_info *arena, u32 lane,
			struct log_group *log)
{
	return arena_read_bytes(arena,
			arena->logoff + (lane * LOG_GRP_SIZE), log,
			LOG_GRP_SIZE, 0);
}

static struct dentry *debugfs_root;

static void arena_debugfs_init(struct arena_info *a, struct dentry *parent,
				int idx)
{
	char dirname[32];
	struct dentry *d;

	/* If for some reason, parent bttN was not created, exit */
	if (!parent)
		return;

	snprintf(dirname, 32, "arena%d", idx);
	d = debugfs_create_dir(dirname, parent);
	if (IS_ERR_OR_NULL(d))
		return;
	a->debugfs_dir = d;

	debugfs_create_x64("size", S_IRUGO, d, &a->size);
	debugfs_create_x64("external_lba_start", S_IRUGO, d,
				&a->external_lba_start);
	debugfs_create_x32("internal_nlba", S_IRUGO, d, &a->internal_nlba);
	debugfs_create_u32("internal_lbasize", S_IRUGO, d,
				&a->internal_lbasize);
	debugfs_create_x32("external_nlba", S_IRUGO, d, &a->external_nlba);
	debugfs_create_u32("external_lbasize", S_IRUGO, d,
				&a->external_lbasize);
	debugfs_create_u32("nfree", S_IRUGO, d, &a->nfree);
	debugfs_create_u16("version_major", S_IRUGO, d, &a->version_major);
	debugfs_create_u16("version_minor", S_IRUGO, d, &a->version_minor);
	debugfs_create_x64("nextoff", S_IRUGO, d, &a->nextoff);
	debugfs_create_x64("infooff", S_IRUGO, d, &a->infooff);
	debugfs_create_x64("dataoff", S_IRUGO, d, &a->dataoff);
	debugfs_create_x64("mapoff", S_IRUGO, d, &a->mapoff);
	debugfs_create_x64("logoff", S_IRUGO, d, &a->logoff);
	debugfs_create_x64("info2off", S_IRUGO, d, &a->info2off);
	debugfs_create_x32("flags", S_IRUGO, d, &a->flags);
	debugfs_create_u32("log_index_0", S_IRUGO, d, &a->log_index[0]);
	debugfs_create_u32("log_index_1", S_IRUGO, d, &a->log_index[1]);
}

static void btt_debugfs_init(struct btt *btt)
{
	int i = 0;
	struct arena_info *arena;

	btt->debugfs_dir = debugfs_create_dir(dev_name(&btt->nd_btt->dev),
						debugfs_root);
	if (IS_ERR_OR_NULL(btt->debugfs_dir))
		return;

	list_for_each_entry(arena, &btt->arena_list, list) {
		arena_debugfs_init(arena, btt->debugfs_dir, i);
		i++;
	}
}

static u32 log_seq(struct log_group *log, int log_idx)
{
	return le32_to_cpu(log->ent[log_idx].seq);
}

/*
 * This function accepts two log entries, and uses the
 * sequence number to find the 'older' entry.
 * It also updates the sequence number in this old entry to
 * make it the 'new' one if the mark_flag is set.
 * Finally, it returns which of the entries was the older one.
 *
 * TODO The logic feels a bit kludge-y. make it better..
 */
static int btt_log_get_old(struct arena_info *a, struct log_group *log)
{
	int idx0 = a->log_index[0];
	int idx1 = a->log_index[1];
	int old;

	/*
	 * the first ever time this is seen, the entry goes into [0]
	 * the next time, the following logic works out to put this
	 * (next) entry into [1]
	 */
	if (log_seq(log, idx0) == 0) {
		log->ent[idx0].seq = cpu_to_le32(1);
		return 0;
	}

	if (log_seq(log, idx0) == log_seq(log, idx1))
		return -EINVAL;
	if (log_seq(log, idx0) + log_seq(log, idx1) > 5)
		return -EINVAL;

	if (log_seq(log, idx0) < log_seq(log, idx1)) {
		if ((log_seq(log, idx1) - log_seq(log, idx0)) == 1)
			old = 0;
		else
			old = 1;
	} else {
		if ((log_seq(log, idx0) - log_seq(log, idx1)) == 1)
			old = 1;
		else
			old = 0;
	}

	return old;
}

/*
 * This function copies the desired (old/new) log entry into ent if
 * it is not NULL. It returns the sub-slot number (0 or 1)
 * where the desired log entry was found. Negative return values
 * indicate errors.
 */
static int btt_log_read(struct arena_info *arena, u32 lane,
			struct log_entry *ent, int old_flag)
{
	int ret;
	int old_ent, ret_ent;
	struct log_group log;

	ret = btt_log_group_read(arena, lane, &log);
	if (ret)
		return -EIO;

	old_ent = btt_log_get_old(arena, &log);
	if (old_ent < 0 || old_ent > 1) {
		dev_err(to_dev(arena),
				"log corruption (%d): lane %d seq [%d, %d]\n",
				old_ent, lane, log.ent[arena->log_index[0]].seq,
				log.ent[arena->log_index[1]].seq);
		/* TODO set error state? */
		return -EIO;
	}

	ret_ent = (old_flag ? old_ent : (1 - old_ent));

	if (ent != NULL)
		memcpy(ent, &log.ent[arena->log_index[ret_ent]], LOG_ENT_SIZE);

	return ret_ent;
}

/*
 * This function commits a log entry to media
 * It does _not_ prepare the freelist entry for the next write
 * btt_flog_write is the wrapper for updating the freelist elements
 */
static int __btt_log_write(struct arena_info *arena, u32 lane,
			u32 sub, struct log_entry *ent, unsigned long flags)
{
	int ret;
	u32 group_slot = arena->log_index[sub];
	unsigned int log_half = LOG_ENT_SIZE / 2;
	void *src = ent;
	u64 ns_off;

	ns_off = arena->logoff + (lane * LOG_GRP_SIZE) +
		(group_slot * LOG_ENT_SIZE);
	/* split the 16B write into atomic, durable halves */
	ret = arena_write_bytes(arena, ns_off, src, log_half, flags);
	if (ret)
		return ret;

	ns_off += log_half;
	src += log_half;
	return arena_write_bytes(arena, ns_off, src, log_half, flags);
}

static int btt_flog_write(struct arena_info *arena, u32 lane, u32 sub,
			struct log_entry *ent)
{
	int ret;

	ret = __btt_log_write(arena, lane, sub, ent, NVDIMM_IO_ATOMIC);
	if (ret)
		return ret;

	/* prepare the next free entry */
	arena->freelist[lane].sub = 1 - arena->freelist[lane].sub;
	if (++(arena->freelist[lane].seq) == 4)
		arena->freelist[lane].seq = 1;
	if (ent_e_flag(le32_to_cpu(ent->old_map)))
		arena->freelist[lane].has_err = 1;
	arena->freelist[lane].block = ent_lba(le32_to_cpu(ent->old_map));

	return ret;
}

/*
 * This function initializes the BTT map to the initial state, which is
 * all-zeroes, and indicates an identity mapping
 */
static int btt_map_init(struct arena_info *arena)
{
	int ret = -EINVAL;
	void *zerobuf;
	size_t offset = 0;
	size_t chunk_size = SZ_2M;
	size_t mapsize = arena->logoff - arena->mapoff;

	zerobuf = kzalloc(chunk_size, GFP_KERNEL);
	if (!zerobuf)
		return -ENOMEM;

	/*
	 * mapoff should always be at least 512B  aligned. We rely on that to
	 * make sure rw_bytes does error clearing correctly, so make sure that
	 * is the case.
	 */
	dev_WARN_ONCE(to_dev(arena), !IS_ALIGNED(arena->mapoff, 512),
		"arena->mapoff: %#llx is unaligned\n", arena->mapoff);

	while (mapsize) {
		size_t size = min(mapsize, chunk_size);

		dev_WARN_ONCE(to_dev(arena), size < 512,
			"chunk size: %#zx is unaligned\n", size);
		ret = arena_write_bytes(arena, arena->mapoff + offset, zerobuf,
				size, 0);
		if (ret)
			goto free;

		offset += size;
		mapsize -= size;
		cond_resched();
	}

 free:
	kfree(zerobuf);
	return ret;
}

/*
 * This function initializes the BTT log with 'fake' entries pointing
 * to the initial reserved set of blocks as being free
 */
static int btt_log_init(struct arena_info *arena)
{
	size_t logsize = arena->info2off - arena->logoff;
	size_t chunk_size = SZ_4K, offset = 0;
	struct log_entry ent;
	void *zerobuf;
	int ret;
	u32 i;

	zerobuf = kzalloc(chunk_size, GFP_KERNEL);
	if (!zerobuf)
		return -ENOMEM;
	/*
	 * logoff should always be at least 512B  aligned. We rely on that to
	 * make sure rw_bytes does error clearing correctly, so make sure that
	 * is the case.
	 */
	dev_WARN_ONCE(to_dev(arena), !IS_ALIGNED(arena->logoff, 512),
		"arena->logoff: %#llx is unaligned\n", arena->logoff);

	while (logsize) {
		size_t size = min(logsize, chunk_size);

		dev_WARN_ONCE(to_dev(arena), size < 512,
			"chunk size: %#zx is unaligned\n", size);
		ret = arena_write_bytes(arena, arena->logoff + offset, zerobuf,
				size, 0);
		if (ret)
			goto free;

		offset += size;
		logsize -= size;
		cond_resched();
	}

	for (i = 0; i < arena->nfree; i++) {
		ent.lba = cpu_to_le32(i);
		ent.old_map = cpu_to_le32(arena->external_nlba + i);
		ent.new_map = cpu_to_le32(arena->external_nlba + i);
		ent.seq = cpu_to_le32(LOG_SEQ_INIT);
		ret = __btt_log_write(arena, i, 0, &ent, 0);
		if (ret)
			goto free;
	}

 free:
	kfree(zerobuf);
	return ret;
}

static u64 to_namespace_offset(struct arena_info *arena, u64 lba)
{
	return arena->dataoff + ((u64)lba * arena->internal_lbasize);
}

static int arena_clear_freelist_error(struct arena_info *arena, u32 lane)
{
	int ret = 0;

	if (arena->freelist[lane].has_err) {
		void *zero_page = page_address(ZERO_PAGE(0));
		u32 lba = arena->freelist[lane].block;
		u64 nsoff = to_namespace_offset(arena, lba);
		unsigned long len = arena->sector_size;

		mutex_lock(&arena->err_lock);

		while (len) {
			unsigned long chunk = min(len, PAGE_SIZE);

			ret = arena_write_bytes(arena, nsoff, zero_page,
				chunk, 0);
			if (ret)
				break;
			len -= chunk;
			nsoff += chunk;
			if (len == 0)
				arena->freelist[lane].has_err = 0;
		}
		mutex_unlock(&arena->err_lock);
	}
	return ret;
}

static int btt_freelist_init(struct arena_info *arena)
{
	int new, ret;
	struct log_entry log_new;
	u32 i, map_entry, log_oldmap, log_newmap;

	arena->freelist = kcalloc(arena->nfree, sizeof(struct free_entry),
					GFP_KERNEL);
	if (!arena->freelist)
		return -ENOMEM;

	for (i = 0; i < arena->nfree; i++) {
		new = btt_log_read(arena, i, &log_new, LOG_NEW_ENT);
		if (new < 0)
			return new;

		/* old and new map entries with any flags stripped out */
		log_oldmap = ent_lba(le32_to_cpu(log_new.old_map));
		log_newmap = ent_lba(le32_to_cpu(log_new.new_map));

		/* sub points to the next one to be overwritten */
		arena->freelist[i].sub = 1 - new;
		arena->freelist[i].seq = nd_inc_seq(le32_to_cpu(log_new.seq));
		arena->freelist[i].block = log_oldmap;

		/*
		 * FIXME: if error clearing fails during init, we want to make
		 * the BTT read-only
		 */
		if (ent_e_flag(le32_to_cpu(log_new.old_map)) &&
		    !ent_normal(le32_to_cpu(log_new.old_map))) {
			arena->freelist[i].has_err = 1;
			ret = arena_clear_freelist_error(arena, i);
			if (ret)
				dev_err_ratelimited(to_dev(arena),
					"Unable to clear known errors\n");
		}

		/* This implies a newly created or untouched flog entry */
		if (log_oldmap == log_newmap)
			continue;

		/* Check if map recovery is needed */
		ret = btt_map_read(arena, le32_to_cpu(log_new.lba), &map_entry,
				NULL, NULL, 0);
		if (ret)
			return ret;

		/*
		 * The map_entry from btt_read_map is stripped of any flag bits,
		 * so use the stripped out versions from the log as well for
		 * testing whether recovery is needed. For restoration, use the
		 * 'raw' version of the log entries as that captured what we
		 * were going to write originally.
		 */
		if ((log_newmap != map_entry) && (log_oldmap == map_entry)) {
			/*
			 * Last transaction wrote the flog, but wasn't able
			 * to complete the map write. So fix up the map.
			 */
			ret = btt_map_write(arena, le32_to_cpu(log_new.lba),
					le32_to_cpu(log_new.new_map), 0, 0, 0);
			if (ret)
				return ret;
		}
	}

	return 0;
}

static bool ent_is_padding(struct log_entry *ent)
{
	return (ent->lba == 0) && (ent->old_map == 0) && (ent->new_map == 0)
		&& (ent->seq == 0);
}

/*
 * Detecting valid log indices: We read a log group (see the comments in btt.h
 * for a description of a 'log_group' and its 'slots'), and iterate over its
 * four slots. We expect that a padding slot will be all-zeroes, and use this
 * to detect a padding slot vs. an actual entry.
 *
 * If a log_group is in the initial state, i.e. hasn't been used since the
 * creation of this BTT layout, it will have three of the four slots with
 * zeroes. We skip over these log_groups for the detection of log_index. If
 * all log_groups are in the initial state (i.e. the BTT has never been
 * written to), it is safe to assume the 'new format' of log entries in slots
 * (0, 1).
 */
static int log_set_indices(struct arena_info *arena)
{
	bool idx_set = false, initial_state = true;
	int ret, log_index[2] = {-1, -1};
	u32 i, j, next_idx = 0;
	struct log_group log;
	u32 pad_count = 0;

	for (i = 0; i < arena->nfree; i++) {
		ret = btt_log_group_read(arena, i, &log);
		if (ret < 0)
			return ret;

		for (j = 0; j < 4; j++) {
			if (!idx_set) {
				if (ent_is_padding(&log.ent[j])) {
					pad_count++;
					continue;
				} else {
					/* Skip if index has been recorded */
					if ((next_idx == 1) &&
						(j == log_index[0]))
						continue;
					/* valid entry, record index */
					log_index[next_idx] = j;
					next_idx++;
				}
				if (next_idx == 2) {
					/* two valid entries found */
					idx_set = true;
				} else if (next_idx > 2) {
					/* too many valid indices */
					return -ENXIO;
				}
			} else {
				/*
				 * once the indices have been set, just verify
				 * that all subsequent log groups are either in
				 * their initial state or follow the same
				 * indices.
				 */
				if (j == log_index[0]) {
					/* entry must be 'valid' */
					if (ent_is_padding(&log.ent[j]))
						return -ENXIO;
				} else if (j == log_index[1]) {
					;
					/*
					 * log_index[1] can be padding if the
					 * lane never got used and it is still
					 * in the initial state (three 'padding'
					 * entries)
					 */
				} else {
					/* entry must be invalid (padding) */
					if (!ent_is_padding(&log.ent[j]))
						return -ENXIO;
				}
			}
		}
		/*
		 * If any of the log_groups have more than one valid,
		 * non-padding entry, then the we are no longer in the
		 * initial_state
		 */
		if (pad_count < 3)
			initial_state = false;
		pad_count = 0;
	}

	if (!initial_state && !idx_set)
		return -ENXIO;

	/*
	 * If all the entries in the log were in the initial state,
	 * assume new padding scheme
	 */
	if (initial_state)
		log_index[1] = 1;

	/*
	 * Only allow the known permutations of log/padding indices,
	 * i.e. (0, 1), and (0, 2)
	 */
	if ((log_index[0] == 0) && ((log_index[1] == 1) || (log_index[1] == 2)))
		; /* known index possibilities */
	else {
		dev_err(to_dev(arena), "Found an unknown padding scheme\n");
		return -ENXIO;
	}

	arena->log_index[0] = log_index[0];
	arena->log_index[1] = log_index[1];
	dev_dbg(to_dev(arena), "log_index_0 = %d\n", log_index[0]);
	dev_dbg(to_dev(arena), "log_index_1 = %d\n", log_index[1]);
	return 0;
}

static int btt_rtt_init(struct arena_info *arena)
{
	arena->rtt = kcalloc(arena->nfree, sizeof(u32), GFP_KERNEL);
	if (arena->rtt == NULL)
		return -ENOMEM;

	return 0;
}

static int btt_maplocks_init(struct arena_info *arena)
{
	u32 i;

	arena->map_locks = kcalloc(arena->nfree, sizeof(struct aligned_lock),
				GFP_KERNEL);
	if (!arena->map_locks)
		return -ENOMEM;

	for (i = 0; i < arena->nfree; i++)
		spin_lock_init(&arena->map_locks[i].lock);

	return 0;
}

static struct arena_info *alloc_arena(struct btt *btt, size_t size,
				size_t start, size_t arena_off)
{
	struct arena_info *arena;
	u64 logsize, mapsize, datasize;
	u64 available = size;

	arena = kzalloc(sizeof(struct arena_info), GFP_KERNEL);
	if (!arena)
		return NULL;
	arena->nd_btt = btt->nd_btt;
	arena->sector_size = btt->sector_size;
	mutex_init(&arena->err_lock);

	if (!size)
		return arena;

	arena->size = size;
	arena->external_lba_start = start;
	arena->external_lbasize = btt->lbasize;
	arena->internal_lbasize = roundup(arena->external_lbasize,
					INT_LBASIZE_ALIGNMENT);
	arena->nfree = BTT_DEFAULT_NFREE;
	arena->version_major = btt->nd_btt->version_major;
	arena->version_minor = btt->nd_btt->version_minor;

	if (available % BTT_PG_SIZE)
		available -= (available % BTT_PG_SIZE);

	/* Two pages are reserved for the super block and its copy */
	available -= 2 * BTT_PG_SIZE;

	/* The log takes a fixed amount of space based on nfree */
	logsize = roundup(arena->nfree * LOG_GRP_SIZE, BTT_PG_SIZE);
	available -= logsize;

	/* Calculate optimal split between map and data area */
	arena->internal_nlba = div_u64(available - BTT_PG_SIZE,
			arena->internal_lbasize + MAP_ENT_SIZE);
	arena->external_nlba = arena->internal_nlba - arena->nfree;

	mapsize = roundup((arena->external_nlba * MAP_ENT_SIZE), BTT_PG_SIZE);
	datasize = available - mapsize;

	/* 'Absolute' values, relative to start of storage space */
	arena->infooff = arena_off;
	arena->dataoff = arena->infooff + BTT_PG_SIZE;
	arena->mapoff = arena->dataoff + datasize;
	arena->logoff = arena->mapoff + mapsize;
	arena->info2off = arena->logoff + logsize;

	/* Default log indices are (0,1) */
	arena->log_index[0] = 0;
	arena->log_index[1] = 1;
	return arena;
}

static void free_arenas(struct btt *btt)
{
	struct arena_info *arena, *next;

	list_for_each_entry_safe(arena, next, &btt->arena_list, list) {
		list_del(&arena->list);
		kfree(arena->rtt);
		kfree(arena->map_locks);
		kfree(arena->freelist);
		debugfs_remove_recursive(arena->debugfs_dir);
		kfree(arena);
	}
}

/*
 * This function reads an existing valid btt superblock and
 * populates the corresponding arena_info struct
 */
static void parse_arena_meta(struct arena_info *arena, struct btt_sb *super,
				u64 arena_off)
{
	arena->internal_nlba = le32_to_cpu(super->internal_nlba);
	arena->internal_lbasize = le32_to_cpu(super->internal_lbasize);
	arena->external_nlba = le32_to_cpu(super->external_nlba);
	arena->external_lbasize = le32_to_cpu(super->external_lbasize);
	arena->nfree = le32_to_cpu(super->nfree);
	arena->version_major = le16_to_cpu(super->version_major);
	arena->version_minor = le16_to_cpu(super->version_minor);

	arena->nextoff = (super->nextoff == 0) ? 0 : (arena_off +
			le64_to_cpu(super->nextoff));
	arena->infooff = arena_off;
	arena->dataoff = arena_off + le64_to_cpu(super->dataoff);
	arena->mapoff = arena_off + le64_to_cpu(super->mapoff);
	arena->logoff = arena_off + le64_to_cpu(super->logoff);
	arena->info2off = arena_off + le64_to_cpu(super->info2off);

	arena->size = (le64_to_cpu(super->nextoff) > 0)
		? (le64_to_cpu(super->nextoff))
		: (arena->info2off - arena->infooff + BTT_PG_SIZE);

	arena->flags = le32_to_cpu(super->flags);
}

static int discover_arenas(struct btt *btt)
{
	int ret = 0;
	struct arena_info *arena;
	struct btt_sb *super;
	size_t remaining = btt->rawsize;
	u64 cur_nlba = 0;
	size_t cur_off = 0;
	int num_arenas = 0;

	super = kzalloc(sizeof(*super), GFP_KERNEL);
	if (!super)
		return -ENOMEM;

	while (remaining) {
		/* Alloc memory for arena */
		arena = alloc_arena(btt, 0, 0, 0);
		if (!arena) {
			ret = -ENOMEM;
			goto out_super;
		}

		arena->infooff = cur_off;
		ret = btt_info_read(arena, super);
		if (ret)
			goto out;

		if (!nd_btt_arena_is_valid(btt->nd_btt, super)) {
			if (remaining == btt->rawsize) {
				btt->init_state = INIT_NOTFOUND;
				dev_info(to_dev(arena), "No existing arenas\n");
				goto out;
			} else {
				dev_err(to_dev(arena),
						"Found corrupted metadata!\n");
				ret = -ENODEV;
				goto out;
			}
		}

		arena->external_lba_start = cur_nlba;
		parse_arena_meta(arena, super, cur_off);

		ret = log_set_indices(arena);
		if (ret) {
			dev_err(to_dev(arena),
				"Unable to deduce log/padding indices\n");
			goto out;
		}

		ret = btt_freelist_init(arena);
		if (ret)
			goto out;

		ret = btt_rtt_init(arena);
		if (ret)
			goto out;

		ret = btt_maplocks_init(arena);
		if (ret)
			goto out;

		list_add_tail(&arena->list, &btt->arena_list);

		remaining -= arena->size;
		cur_off += arena->size;
		cur_nlba += arena->external_nlba;
		num_arenas++;

		if (arena->nextoff == 0)
			break;
	}
	btt->num_arenas = num_arenas;
	btt->nlba = cur_nlba;
	btt->init_state = INIT_READY;

	kfree(super);
	return ret;

 out:
	kfree(arena);
	free_arenas(btt);
 out_super:
	kfree(super);
	return ret;
}

static int create_arenas(struct btt *btt)
{
	size_t remaining = btt->rawsize;
	size_t cur_off = 0;

	while (remaining) {
		struct arena_info *arena;
		size_t arena_size = min_t(u64, ARENA_MAX_SIZE, remaining);

		remaining -= arena_size;
		if (arena_size < ARENA_MIN_SIZE)
			break;

		arena = alloc_arena(btt, arena_size, btt->nlba, cur_off);
		if (!arena) {
			free_arenas(btt);
			return -ENOMEM;
		}
		btt->nlba += arena->external_nlba;
		if (remaining >= ARENA_MIN_SIZE)
			arena->nextoff = arena->size;
		else
			arena->nextoff = 0;
		cur_off += arena_size;
		list_add_tail(&arena->list, &btt->arena_list);
	}

	return 0;
}

/*
 * This function completes arena initialization by writing
 * all the metadata.
 * It is only called for an uninitialized arena when a write
 * to that arena occurs for the first time.
 */
static int btt_arena_write_layout(struct arena_info *arena)
{
	int ret;
	u64 sum;
	struct btt_sb *super;
	struct nd_btt *nd_btt = arena->nd_btt;
	const u8 *parent_uuid = nd_dev_to_uuid(&nd_btt->ndns->dev);

	ret = btt_map_init(arena);
	if (ret)
		return ret;

	ret = btt_log_init(arena);
	if (ret)
		return ret;

	super = kzalloc(sizeof(struct btt_sb), GFP_NOIO);
	if (!super)
		return -ENOMEM;

	strncpy(super->signature, BTT_SIG, BTT_SIG_LEN);
	memcpy(super->uuid, nd_btt->uuid, 16);
	memcpy(super->parent_uuid, parent_uuid, 16);
	super->flags = cpu_to_le32(arena->flags);
	super->version_major = cpu_to_le16(arena->version_major);
	super->version_minor = cpu_to_le16(arena->version_minor);
	super->external_lbasize = cpu_to_le32(arena->external_lbasize);
	super->external_nlba = cpu_to_le32(arena->external_nlba);
	super->internal_lbasize = cpu_to_le32(arena->internal_lbasize);
	super->internal_nlba = cpu_to_le32(arena->internal_nlba);
	super->nfree = cpu_to_le32(arena->nfree);
	super->infosize = cpu_to_le32(sizeof(struct btt_sb));
	super->nextoff = cpu_to_le64(arena->nextoff);
	/*
	 * Subtract arena->infooff (arena start) so numbers are relative
	 * to 'this' arena
	 */
	super->dataoff = cpu_to_le64(arena->dataoff - arena->infooff);
	super->mapoff = cpu_to_le64(arena->mapoff - arena->infooff);
	super->logoff = cpu_to_le64(arena->logoff - arena->infooff);
	super->info2off = cpu_to_le64(arena->info2off - arena->infooff);

	super->flags = 0;
	sum = nd_sb_checksum((struct nd_gen_sb *) super);
	super->checksum = cpu_to_le64(sum);

	ret = btt_info_write(arena, super);

	kfree(super);
	return ret;
}

/*
 * This function completes the initialization for the BTT namespace
 * such that it is ready to accept IOs
 */
static int btt_meta_init(struct btt *btt)
{
	int ret = 0;
	struct arena_info *arena;

	mutex_lock(&btt->init_lock);
	list_for_each_entry(arena, &btt->arena_list, list) {
		ret = btt_arena_write_layout(arena);
		if (ret)
			goto unlock;

		ret = btt_freelist_init(arena);
		if (ret)
			goto unlock;

		ret = btt_rtt_init(arena);
		if (ret)
			goto unlock;

		ret = btt_maplocks_init(arena);
		if (ret)
			goto unlock;
	}

	btt->init_state = INIT_READY;

 unlock:
	mutex_unlock(&btt->init_lock);
	return ret;
}

static u32 btt_meta_size(struct btt *btt)
{
	return btt->lbasize - btt->sector_size;
}

/*
 * This function calculates the arena in which the given LBA lies
 * by doing a linear walk. This is acceptable since we expect only
 * a few arenas. If we have backing devices that get much larger,
 * we can construct a balanced binary tree of arenas at init time
 * so that this range search becomes faster.
 */
static int lba_to_arena(struct btt *btt, sector_t sector, __u32 *premap,
				struct arena_info **arena)
{
	struct arena_info *arena_list;
	__u64 lba = div_u64(sector << SECTOR_SHIFT, btt->sector_size);

	list_for_each_entry(arena_list, &btt->arena_list, list) {
		if (lba < arena_list->external_nlba) {
			*arena = arena_list;
			*premap = lba;
			return 0;
		}
		lba -= arena_list->external_nlba;
	}

	return -EIO;
}

/*
 * The following (lock_map, unlock_map) are mostly just to improve
 * readability, since they index into an array of locks
 */
static void lock_map(struct arena_info *arena, u32 premap)
		__acquires(&arena->map_locks[idx].lock)
{
	u32 idx = (premap * MAP_ENT_SIZE / L1_CACHE_BYTES) % arena->nfree;

	spin_lock(&arena->map_locks[idx].lock);
}

static void unlock_map(struct arena_info *arena, u32 premap)
		__releases(&arena->map_locks[idx].lock)
{
	u32 idx = (premap * MAP_ENT_SIZE / L1_CACHE_BYTES) % arena->nfree;

	spin_unlock(&arena->map_locks[idx].lock);
}

static int btt_data_read(struct arena_info *arena, struct page *page,
			unsigned int off, u32 lba, u32 len)
{
	int ret;
	u64 nsoff = to_namespace_offset(arena, lba);
	void *mem = kmap_atomic(page);

	ret = arena_read_bytes(arena, nsoff, mem + off, len, NVDIMM_IO_ATOMIC);
	kunmap_atomic(mem);

	return ret;
}

static int btt_data_write(struct arena_info *arena, u32 lba,
			struct page *page, unsigned int off, u32 len)
{
	int ret;
	u64 nsoff = to_namespace_offset(arena, lba);
	void *mem = kmap_atomic(page);

	ret = arena_write_bytes(arena, nsoff, mem + off, len, NVDIMM_IO_ATOMIC);
	kunmap_atomic(mem);

	return ret;
}

static void zero_fill_data(struct page *page, unsigned int off, u32 len)
{
	void *mem = kmap_atomic(page);

	memset(mem + off, 0, len);
	kunmap_atomic(mem);
}

#ifdef CONFIG_BLK_DEV_INTEGRITY
static int btt_rw_integrity(struct btt *btt, struct bio_integrity_payload *bip,
			struct arena_info *arena, u32 postmap, int rw)
{
	unsigned int len = btt_meta_size(btt);
	u64 meta_nsoff;
	int ret = 0;

	if (bip == NULL)
		return 0;

	meta_nsoff = to_namespace_offset(arena, postmap) + btt->sector_size;

	while (len) {
		unsigned int cur_len;
		struct bio_vec bv;
		void *mem;

		bv = bvec_iter_bvec(bip->bip_vec, bip->bip_iter);
		/*
		 * The 'bv' obtained from bvec_iter_bvec has its .bv_len and
		 * .bv_offset already adjusted for iter->bi_bvec_done, and we
		 * can use those directly
		 */

		cur_len = min(len, bv.bv_len);
		mem = kmap_atomic(bv.bv_page);
		if (rw)
			ret = arena_write_bytes(arena, meta_nsoff,
					mem + bv.bv_offset, cur_len,
					NVDIMM_IO_ATOMIC);
		else
			ret = arena_read_bytes(arena, meta_nsoff,
					mem + bv.bv_offset, cur_len,
					NVDIMM_IO_ATOMIC);

		kunmap_atomic(mem);
		if (ret)
			return ret;

		len -= cur_len;
		meta_nsoff += cur_len;
		if (!bvec_iter_advance(bip->bip_vec, &bip->bip_iter, cur_len))
			return -EIO;
	}

	return ret;
}

#else /* CONFIG_BLK_DEV_INTEGRITY */
static int btt_rw_integrity(struct btt *btt, struct bio_integrity_payload *bip,
			struct arena_info *arena, u32 postmap, int rw)
{
	return 0;
}
#endif

static int btt_read_pg(struct btt *btt, struct bio_integrity_payload *bip,
			struct page *page, unsigned int off, sector_t sector,
			unsigned int len)
{
	int ret = 0;
	int t_flag, e_flag;
	struct arena_info *arena = NULL;
	u32 lane = 0, premap, postmap;

	while (len) {
		u32 cur_len;

		lane = nd_region_acquire_lane(btt->nd_region);

		ret = lba_to_arena(btt, sector, &premap, &arena);
		if (ret)
			goto out_lane;

		cur_len = min(btt->sector_size, len);

		ret = btt_map_read(arena, premap, &postmap, &t_flag, &e_flag,
				NVDIMM_IO_ATOMIC);
		if (ret)
			goto out_lane;

		/*
		 * We loop to make sure that the post map LBA didn't change
		 * from under us between writing the RTT and doing the actual
		 * read.
		 */
		while (1) {
			u32 new_map;
			int new_t, new_e;

			if (t_flag) {
				zero_fill_data(page, off, cur_len);
				goto out_lane;
			}

			if (e_flag) {
				ret = -EIO;
				goto out_lane;
			}

			arena->rtt[lane] = RTT_VALID | postmap;
			/*
			 * Barrier to make sure this write is not reordered
			 * to do the verification map_read before the RTT store
			 */
			barrier();

			ret = btt_map_read(arena, premap, &new_map, &new_t,
						&new_e, NVDIMM_IO_ATOMIC);
			if (ret)
				goto out_rtt;

			if ((postmap == new_map) && (t_flag == new_t) &&
					(e_flag == new_e))
				break;

			postmap = new_map;
			t_flag = new_t;
			e_flag = new_e;
		}

		ret = btt_data_read(arena, page, off, postmap, cur_len);
		if (ret) {
			/* Media error - set the e_flag */
			if (btt_map_write(arena, premap, postmap, 0, 1, NVDIMM_IO_ATOMIC))
				dev_warn_ratelimited(to_dev(arena),
					"Error persistently tracking bad blocks at %#x\n",
					premap);
			goto out_rtt;
		}

		if (bip) {
			ret = btt_rw_integrity(btt, bip, arena, postmap, READ);
			if (ret)
				goto out_rtt;
		}

		arena->rtt[lane] = RTT_INVALID;
		nd_region_release_lane(btt->nd_region, lane);

		len -= cur_len;
		off += cur_len;
		sector += btt->sector_size >> SECTOR_SHIFT;
	}

	return 0;

 out_rtt:
	arena->rtt[lane] = RTT_INVALID;
 out_lane:
	nd_region_release_lane(btt->nd_region, lane);
	return ret;
}

/*
 * Normally, arena_{read,write}_bytes will take care of the initial offset
 * adjustment, but in the case of btt_is_badblock, where we query is_bad_pmem,
 * we need the final, raw namespace offset here
 */
static bool btt_is_badblock(struct btt *btt, struct arena_info *arena,
		u32 postmap)
{
	u64 nsoff = adjust_initial_offset(arena->nd_btt,
			to_namespace_offset(arena, postmap));
	sector_t phys_sector = nsoff >> 9;

	return is_bad_pmem(btt->phys_bb, phys_sector, arena->internal_lbasize);
}

static int btt_write_pg(struct btt *btt, struct bio_integrity_payload *bip,
			sector_t sector, struct page *page, unsigned int off,
			unsigned int len)
{
	int ret = 0;
	struct arena_info *arena = NULL;
	u32 premap = 0, old_postmap, new_postmap, lane = 0, i;
	struct log_entry log;
	int sub;

	while (len) {
		u32 cur_len;
		int e_flag;

 retry:
		lane = nd_region_acquire_lane(btt->nd_region);

		ret = lba_to_arena(btt, sector, &premap, &arena);
		if (ret)
			goto out_lane;
		cur_len = min(btt->sector_size, len);

		if ((arena->flags & IB_FLAG_ERROR_MASK) != 0) {
			ret = -EIO;
			goto out_lane;
		}

		if (btt_is_badblock(btt, arena, arena->freelist[lane].block))
			arena->freelist[lane].has_err = 1;

		if (mutex_is_locked(&arena->err_lock)
				|| arena->freelist[lane].has_err) {
			nd_region_release_lane(btt->nd_region, lane);

			ret = arena_clear_freelist_error(arena, lane);
			if (ret)
				return ret;

			/* OK to acquire a different lane/free block */
			goto retry;
		}

		new_postmap = arena->freelist[lane].block;

		/* Wait if the new block is being read from */
		for (i = 0; i < arena->nfree; i++)
			while (arena->rtt[i] == (RTT_VALID | new_postmap))
				cpu_relax();


		if (new_postmap >= arena->internal_nlba) {
			ret = -EIO;
			goto out_lane;
		}

		ret = btt_data_write(arena, new_postmap, page, off, cur_len);
		if (ret)
			goto out_lane;

		if (bip) {
			ret = btt_rw_integrity(btt, bip, arena, new_postmap,
						WRITE);
			if (ret)
				goto out_lane;
		}

		lock_map(arena, premap);
		ret = btt_map_read(arena, premap, &old_postmap, NULL, &e_flag,
				NVDIMM_IO_ATOMIC);
		if (ret)
			goto out_map;
		if (old_postmap >= arena->internal_nlba) {
			ret = -EIO;
			goto out_map;
		}
		if (e_flag)
			set_e_flag(old_postmap);

		log.lba = cpu_to_le32(premap);
		log.old_map = cpu_to_le32(old_postmap);
		log.new_map = cpu_to_le32(new_postmap);
		log.seq = cpu_to_le32(arena->freelist[lane].seq);
		sub = arena->freelist[lane].sub;
		ret = btt_flog_write(arena, lane, sub, &log);
		if (ret)
			goto out_map;

		ret = btt_map_write(arena, premap, new_postmap, 0, 0,
			NVDIMM_IO_ATOMIC);
		if (ret)
			goto out_map;

		unlock_map(arena, premap);
		nd_region_release_lane(btt->nd_region, lane);

		if (e_flag) {
			ret = arena_clear_freelist_error(arena, lane);
			if (ret)
				return ret;
		}

		len -= cur_len;
		off += cur_len;
		sector += btt->sector_size >> SECTOR_SHIFT;
	}

	return 0;

 out_map:
	unlock_map(arena, premap);
 out_lane:
	nd_region_release_lane(btt->nd_region, lane);
	return ret;
}

static int btt_do_bvec(struct btt *btt, struct bio_integrity_payload *bip,
			struct page *page, unsigned int len, unsigned int off,
			unsigned int op, sector_t sector)
{
	int ret;

	if (!op_is_write(op)) {
		ret = btt_read_pg(btt, bip, page, off, sector, len);
		flush_dcache_page(page);
	} else {
		flush_dcache_page(page);
		ret = btt_write_pg(btt, bip, sector, page, off, len);
	}

	return ret;
}

static blk_qc_t btt_submit_bio(struct bio *bio)
{
	struct bio_integrity_payload *bip = bio_integrity(bio);
	struct btt *btt = bio->bi_bdev->bd_disk->private_data;
	struct bvec_iter iter;
	unsigned long start;
	struct bio_vec bvec;
	int err = 0;
	bool do_acct;

	if (!bio_integrity_prep(bio))
		return BLK_QC_T_NONE;

	do_acct = blk_queue_io_stat(bio->bi_bdev->bd_disk->queue);
	if (do_acct)
		start = bio_start_io_acct(bio);
	bio_for_each_segment(bvec, bio, iter) {
		unsigned int len = bvec.bv_len;

		if (len > PAGE_SIZE || len < btt->sector_size ||
				len % btt->sector_size) {
			dev_err_ratelimited(&btt->nd_btt->dev,
				"unaligned bio segment (len: %d)\n", len);
			bio->bi_status = BLK_STS_IOERR;
			break;
		}

		err = btt_do_bvec(btt, bip, bvec.bv_page, len, bvec.bv_offset,
				  bio_op(bio), iter.bi_sector);
		if (err) {
			dev_err(&btt->nd_btt->dev,
					"io error in %s sector %lld, len %d,\n",
					(op_is_write(bio_op(bio))) ? "WRITE" :
					"READ",
					(unsigned long long) iter.bi_sector, len);
			bio->bi_status = errno_to_blk_status(err);
			break;
		}
	}
	if (do_acct)
		bio_end_io_acct(bio, start);

	bio_endio(bio);
	return BLK_QC_T_NONE;
}

static int btt_rw_page(struct block_device *bdev, sector_t sector,
		struct page *page, unsigned int op)
{
	struct btt *btt = bdev->bd_disk->private_data;
	int rc;

	rc = btt_do_bvec(btt, NULL, page, thp_size(page), 0, op, sector);
	if (rc == 0)
		page_endio(page, op_is_write(op), 0);

	return rc;
}


static int btt_getgeo(struct block_device *bd, struct hd_geometry *geo)
{
	/* some standard values */
	geo->heads = 1 << 6;
	geo->sectors = 1 << 5;
	geo->cylinders = get_capacity(bd->bd_disk) >> 11;
	return 0;
}

static const struct block_device_operations btt_fops = {
	.owner =		THIS_MODULE,
	.submit_bio =		btt_submit_bio,
	.rw_page =		btt_rw_page,
	.getgeo =		btt_getgeo,
};

static int btt_blk_init(struct btt *btt)
{
	struct nd_btt *nd_btt = btt->nd_btt;
	struct nd_namespace_common *ndns = nd_btt->ndns;

	/* create a new disk and request queue for btt */
	btt->btt_queue = blk_alloc_queue(NUMA_NO_NODE);
	if (!btt->btt_queue)
		return -ENOMEM;

	btt->btt_disk = alloc_disk(0);
	if (!btt->btt_disk) {
		blk_cleanup_queue(btt->btt_queue);
		return -ENOMEM;
	}

	nvdimm_namespace_disk_name(ndns, btt->btt_disk->disk_name);
	btt->btt_disk->first_minor = 0;
	btt->btt_disk->fops = &btt_fops;
	btt->btt_disk->private_data = btt;
	btt->btt_disk->queue = btt->btt_queue;
	btt->btt_disk->flags = GENHD_FL_EXT_DEVT;

	blk_queue_logical_block_size(btt->btt_queue, btt->sector_size);
	blk_queue_max_hw_sectors(btt->btt_queue, UINT_MAX);
	blk_queue_flag_set(QUEUE_FLAG_NONROT, btt->btt_queue);

	if (btt_meta_size(btt)) {
		int rc = nd_integrity_init(btt->btt_disk, btt_meta_size(btt));

		if (rc) {
			del_gendisk(btt->btt_disk);
			put_disk(btt->btt_disk);
			blk_cleanup_queue(btt->btt_queue);
			return rc;
		}
	}
	set_capacity(btt->btt_disk, btt->nlba * btt->sector_size >> 9);
	device_add_disk(&btt->nd_btt->dev, btt->btt_disk, NULL);
	btt->nd_btt->size = btt->nlba * (u64)btt->sector_size;
	nvdimm_check_and_set_ro(btt->btt_disk);

	return 0;
}

static void btt_blk_cleanup(struct btt *btt)
{
	del_gendisk(btt->btt_disk);
	put_disk(btt->btt_disk);
	blk_cleanup_queue(btt->btt_queue);
}

/**
 * btt_init - initialize a block translation table for the given device
 * @nd_btt:	device with BTT geometry and backing device info
 * @rawsize:	raw size in bytes of the backing device
 * @lbasize:	lba size of the backing device
 * @uuid:	A uuid for the backing device - this is stored on media
 * @maxlane:	maximum number of parallel requests the device can handle
 *
 * Initialize a Block Translation Table on a backing device to provide
 * single sector power fail atomicity.
 *
 * Context:
 * Might sleep.
 *
 * Returns:
 * Pointer to a new struct btt on success, NULL on failure.
 */
static struct btt *btt_init(struct nd_btt *nd_btt, unsigned long long rawsize,
		u32 lbasize, u8 *uuid, struct nd_region *nd_region)
{
	int ret;
	struct btt *btt;
	struct nd_namespace_io *nsio;
	struct device *dev = &nd_btt->dev;

	btt = devm_kzalloc(dev, sizeof(struct btt), GFP_KERNEL);
	if (!btt)
		return NULL;

	btt->nd_btt = nd_btt;
	btt->rawsize = rawsize;
	btt->lbasize = lbasize;
	btt->sector_size = ((lbasize >= 4096) ? 4096 : 512);
	INIT_LIST_HEAD(&btt->arena_list);
	mutex_init(&btt->init_lock);
	btt->nd_region = nd_region;
	nsio = to_nd_namespace_io(&nd_btt->ndns->dev);
	btt->phys_bb = &nsio->bb;

	ret = discover_arenas(btt);
	if (ret) {
		dev_err(dev, "init: error in arena_discover: %d\n", ret);
		return NULL;
	}

	if (btt->init_state != INIT_READY && nd_region->ro) {
		dev_warn(dev, "%s is read-only, unable to init btt metadata\n",
				dev_name(&nd_region->dev));
		return NULL;
	} else if (btt->init_state != INIT_READY) {
		btt->num_arenas = (rawsize / ARENA_MAX_SIZE) +
			((rawsize % ARENA_MAX_SIZE) ? 1 : 0);
		dev_dbg(dev, "init: %d arenas for %llu rawsize\n",
				btt->num_arenas, rawsize);

		ret = create_arenas(btt);
		if (ret) {
			dev_info(dev, "init: create_arenas: %d\n", ret);
			return NULL;
		}

		ret = btt_meta_init(btt);
		if (ret) {
			dev_err(dev, "init: error in meta_init: %d\n", ret);
			return NULL;
		}
	}

	ret = btt_blk_init(btt);
	if (ret) {
		dev_err(dev, "init: error in blk_init: %d\n", ret);
		return NULL;
	}

	btt_debugfs_init(btt);

	return btt;
}

/**
 * btt_fini - de-initialize a BTT
 * @btt:	the BTT handle that was generated by btt_init
 *
 * De-initialize a Block Translation Table on device removal
 *
 * Context:
 * Might sleep.
 */
static void btt_fini(struct btt *btt)
{
	if (btt) {
		btt_blk_cleanup(btt);
		free_arenas(btt);
		debugfs_remove_recursive(btt->debugfs_dir);
	}
}

int nvdimm_namespace_attach_btt(struct nd_namespace_common *ndns)
{
	struct nd_btt *nd_btt = to_nd_btt(ndns->claim);
	struct nd_region *nd_region;
	struct btt_sb *btt_sb;
	struct btt *btt;
	size_t size, rawsize;
	int rc;

	if (!nd_btt->uuid || !nd_btt->ndns || !nd_btt->lbasize) {
		dev_dbg(&nd_btt->dev, "incomplete btt configuration\n");
		return -ENODEV;
	}

	btt_sb = devm_kzalloc(&nd_btt->dev, sizeof(*btt_sb), GFP_KERNEL);
	if (!btt_sb)
		return -ENOMEM;

	size = nvdimm_namespace_capacity(ndns);
	rc = devm_namespace_enable(&nd_btt->dev, ndns, size);
	if (rc)
		return rc;

	/*
	 * If this returns < 0, that is ok as it just means there wasn't
	 * an existing BTT, and we're creating a new one. We still need to
	 * call this as we need the version dependent fields in nd_btt to be
	 * set correctly based on the holder class
	 */
	nd_btt_version(nd_btt, ndns, btt_sb);

	rawsize = size - nd_btt->initial_offset;
	if (rawsize < ARENA_MIN_SIZE) {
		dev_dbg(&nd_btt->dev, "%s must be at least %ld bytes\n",
				dev_name(&ndns->dev),
				ARENA_MIN_SIZE + nd_btt->initial_offset);
		return -ENXIO;
	}
	nd_region = to_nd_region(nd_btt->dev.parent);
	btt = btt_init(nd_btt, rawsize, nd_btt->lbasize, nd_btt->uuid,
			nd_region);
	if (!btt)
		return -ENOMEM;
	nd_btt->btt = btt;

	return 0;
}
EXPORT_SYMBOL(nvdimm_namespace_attach_btt);

int nvdimm_namespace_detach_btt(struct nd_btt *nd_btt)
{
	struct btt *btt = nd_btt->btt;

	btt_fini(btt);
	nd_btt->btt = NULL;

	return 0;
}
EXPORT_SYMBOL(nvdimm_namespace_detach_btt);

static int __init nd_btt_init(void)
{
	int rc = 0;

	debugfs_root = debugfs_create_dir("btt", NULL);
	if (IS_ERR_OR_NULL(debugfs_root))
		rc = -ENXIO;

	return rc;
}

static void __exit nd_btt_exit(void)
{
	debugfs_remove_recursive(debugfs_root);
}

MODULE_ALIAS_ND_DEVICE(ND_DEVICE_BTT);
MODULE_AUTHOR("Vishal Verma <vishal.l.verma@linux.intel.com>");
MODULE_LICENSE("GPL v2");
module_init(nd_btt_init);
module_exit(nd_btt_exit);
