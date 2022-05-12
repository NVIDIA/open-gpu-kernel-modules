// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (c) International Business Machines Corp., 2006
 *
 * Author: Artem Bityutskiy (Битюцкий Артём)
 */

/*
 * The UBI Eraseblock Association (EBA) sub-system.
 *
 * This sub-system is responsible for I/O to/from logical eraseblock.
 *
 * Although in this implementation the EBA table is fully kept and managed in
 * RAM, which assumes poor scalability, it might be (partially) maintained on
 * flash in future implementations.
 *
 * The EBA sub-system implements per-logical eraseblock locking. Before
 * accessing a logical eraseblock it is locked for reading or writing. The
 * per-logical eraseblock locking is implemented by means of the lock tree. The
 * lock tree is an RB-tree which refers all the currently locked logical
 * eraseblocks. The lock tree elements are &struct ubi_ltree_entry objects.
 * They are indexed by (@vol_id, @lnum) pairs.
 *
 * EBA also maintains the global sequence counter which is incremented each
 * time a logical eraseblock is mapped to a physical eraseblock and it is
 * stored in the volume identifier header. This means that each VID header has
 * a unique sequence number. The sequence number is only increased an we assume
 * 64 bits is enough to never overflow.
 */

#include <linux/slab.h>
#include <linux/crc32.h>
#include <linux/err.h>
#include "ubi.h"

/* Number of physical eraseblocks reserved for atomic LEB change operation */
#define EBA_RESERVED_PEBS 1

/**
 * struct ubi_eba_entry - structure encoding a single LEB -> PEB association
 * @pnum: the physical eraseblock number attached to the LEB
 *
 * This structure is encoding a LEB -> PEB association. Note that the LEB
 * number is not stored here, because it is the index used to access the
 * entries table.
 */
struct ubi_eba_entry {
	int pnum;
};

/**
 * struct ubi_eba_table - LEB -> PEB association information
 * @entries: the LEB to PEB mapping (one entry per LEB).
 *
 * This structure is private to the EBA logic and should be kept here.
 * It is encoding the LEB to PEB association table, and is subject to
 * changes.
 */
struct ubi_eba_table {
	struct ubi_eba_entry *entries;
};

/**
 * next_sqnum - get next sequence number.
 * @ubi: UBI device description object
 *
 * This function returns next sequence number to use, which is just the current
 * global sequence counter value. It also increases the global sequence
 * counter.
 */
unsigned long long ubi_next_sqnum(struct ubi_device *ubi)
{
	unsigned long long sqnum;

	spin_lock(&ubi->ltree_lock);
	sqnum = ubi->global_sqnum++;
	spin_unlock(&ubi->ltree_lock);

	return sqnum;
}

/**
 * ubi_get_compat - get compatibility flags of a volume.
 * @ubi: UBI device description object
 * @vol_id: volume ID
 *
 * This function returns compatibility flags for an internal volume. User
 * volumes have no compatibility flags, so %0 is returned.
 */
static int ubi_get_compat(const struct ubi_device *ubi, int vol_id)
{
	if (vol_id == UBI_LAYOUT_VOLUME_ID)
		return UBI_LAYOUT_VOLUME_COMPAT;
	return 0;
}

/**
 * ubi_eba_get_ldesc - get information about a LEB
 * @vol: volume description object
 * @lnum: logical eraseblock number
 * @ldesc: the LEB descriptor to fill
 *
 * Used to query information about a specific LEB.
 * It is currently only returning the physical position of the LEB, but will be
 * extended to provide more information.
 */
void ubi_eba_get_ldesc(struct ubi_volume *vol, int lnum,
		       struct ubi_eba_leb_desc *ldesc)
{
	ldesc->lnum = lnum;
	ldesc->pnum = vol->eba_tbl->entries[lnum].pnum;
}

/**
 * ubi_eba_create_table - allocate a new EBA table and initialize it with all
 *			  LEBs unmapped
 * @vol: volume containing the EBA table to copy
 * @nentries: number of entries in the table
 *
 * Allocate a new EBA table and initialize it with all LEBs unmapped.
 * Returns a valid pointer if it succeed, an ERR_PTR() otherwise.
 */
struct ubi_eba_table *ubi_eba_create_table(struct ubi_volume *vol,
					   int nentries)
{
	struct ubi_eba_table *tbl;
	int err = -ENOMEM;
	int i;

	tbl = kzalloc(sizeof(*tbl), GFP_KERNEL);
	if (!tbl)
		return ERR_PTR(-ENOMEM);

	tbl->entries = kmalloc_array(nentries, sizeof(*tbl->entries),
				     GFP_KERNEL);
	if (!tbl->entries)
		goto err;

	for (i = 0; i < nentries; i++)
		tbl->entries[i].pnum = UBI_LEB_UNMAPPED;

	return tbl;

err:
	kfree(tbl);

	return ERR_PTR(err);
}

/**
 * ubi_eba_destroy_table - destroy an EBA table
 * @tbl: the table to destroy
 *
 * Destroy an EBA table.
 */
void ubi_eba_destroy_table(struct ubi_eba_table *tbl)
{
	if (!tbl)
		return;

	kfree(tbl->entries);
	kfree(tbl);
}

/**
 * ubi_eba_copy_table - copy the EBA table attached to vol into another table
 * @vol: volume containing the EBA table to copy
 * @dst: destination
 * @nentries: number of entries to copy
 *
 * Copy the EBA table stored in vol into the one pointed by dst.
 */
void ubi_eba_copy_table(struct ubi_volume *vol, struct ubi_eba_table *dst,
			int nentries)
{
	struct ubi_eba_table *src;
	int i;

	ubi_assert(dst && vol && vol->eba_tbl);

	src = vol->eba_tbl;

	for (i = 0; i < nentries; i++)
		dst->entries[i].pnum = src->entries[i].pnum;
}

/**
 * ubi_eba_replace_table - assign a new EBA table to a volume
 * @vol: volume containing the EBA table to copy
 * @tbl: new EBA table
 *
 * Assign a new EBA table to the volume and release the old one.
 */
void ubi_eba_replace_table(struct ubi_volume *vol, struct ubi_eba_table *tbl)
{
	ubi_eba_destroy_table(vol->eba_tbl);
	vol->eba_tbl = tbl;
}

/**
 * ltree_lookup - look up the lock tree.
 * @ubi: UBI device description object
 * @vol_id: volume ID
 * @lnum: logical eraseblock number
 *
 * This function returns a pointer to the corresponding &struct ubi_ltree_entry
 * object if the logical eraseblock is locked and %NULL if it is not.
 * @ubi->ltree_lock has to be locked.
 */
static struct ubi_ltree_entry *ltree_lookup(struct ubi_device *ubi, int vol_id,
					    int lnum)
{
	struct rb_node *p;

	p = ubi->ltree.rb_node;
	while (p) {
		struct ubi_ltree_entry *le;

		le = rb_entry(p, struct ubi_ltree_entry, rb);

		if (vol_id < le->vol_id)
			p = p->rb_left;
		else if (vol_id > le->vol_id)
			p = p->rb_right;
		else {
			if (lnum < le->lnum)
				p = p->rb_left;
			else if (lnum > le->lnum)
				p = p->rb_right;
			else
				return le;
		}
	}

	return NULL;
}

/**
 * ltree_add_entry - add new entry to the lock tree.
 * @ubi: UBI device description object
 * @vol_id: volume ID
 * @lnum: logical eraseblock number
 *
 * This function adds new entry for logical eraseblock (@vol_id, @lnum) to the
 * lock tree. If such entry is already there, its usage counter is increased.
 * Returns pointer to the lock tree entry or %-ENOMEM if memory allocation
 * failed.
 */
static struct ubi_ltree_entry *ltree_add_entry(struct ubi_device *ubi,
					       int vol_id, int lnum)
{
	struct ubi_ltree_entry *le, *le1, *le_free;

	le = kmalloc(sizeof(struct ubi_ltree_entry), GFP_NOFS);
	if (!le)
		return ERR_PTR(-ENOMEM);

	le->users = 0;
	init_rwsem(&le->mutex);
	le->vol_id = vol_id;
	le->lnum = lnum;

	spin_lock(&ubi->ltree_lock);
	le1 = ltree_lookup(ubi, vol_id, lnum);

	if (le1) {
		/*
		 * This logical eraseblock is already locked. The newly
		 * allocated lock entry is not needed.
		 */
		le_free = le;
		le = le1;
	} else {
		struct rb_node **p, *parent = NULL;

		/*
		 * No lock entry, add the newly allocated one to the
		 * @ubi->ltree RB-tree.
		 */
		le_free = NULL;

		p = &ubi->ltree.rb_node;
		while (*p) {
			parent = *p;
			le1 = rb_entry(parent, struct ubi_ltree_entry, rb);

			if (vol_id < le1->vol_id)
				p = &(*p)->rb_left;
			else if (vol_id > le1->vol_id)
				p = &(*p)->rb_right;
			else {
				ubi_assert(lnum != le1->lnum);
				if (lnum < le1->lnum)
					p = &(*p)->rb_left;
				else
					p = &(*p)->rb_right;
			}
		}

		rb_link_node(&le->rb, parent, p);
		rb_insert_color(&le->rb, &ubi->ltree);
	}
	le->users += 1;
	spin_unlock(&ubi->ltree_lock);

	kfree(le_free);
	return le;
}

/**
 * leb_read_lock - lock logical eraseblock for reading.
 * @ubi: UBI device description object
 * @vol_id: volume ID
 * @lnum: logical eraseblock number
 *
 * This function locks a logical eraseblock for reading. Returns zero in case
 * of success and a negative error code in case of failure.
 */
static int leb_read_lock(struct ubi_device *ubi, int vol_id, int lnum)
{
	struct ubi_ltree_entry *le;

	le = ltree_add_entry(ubi, vol_id, lnum);
	if (IS_ERR(le))
		return PTR_ERR(le);
	down_read(&le->mutex);
	return 0;
}

/**
 * leb_read_unlock - unlock logical eraseblock.
 * @ubi: UBI device description object
 * @vol_id: volume ID
 * @lnum: logical eraseblock number
 */
static void leb_read_unlock(struct ubi_device *ubi, int vol_id, int lnum)
{
	struct ubi_ltree_entry *le;

	spin_lock(&ubi->ltree_lock);
	le = ltree_lookup(ubi, vol_id, lnum);
	le->users -= 1;
	ubi_assert(le->users >= 0);
	up_read(&le->mutex);
	if (le->users == 0) {
		rb_erase(&le->rb, &ubi->ltree);
		kfree(le);
	}
	spin_unlock(&ubi->ltree_lock);
}

/**
 * leb_write_lock - lock logical eraseblock for writing.
 * @ubi: UBI device description object
 * @vol_id: volume ID
 * @lnum: logical eraseblock number
 *
 * This function locks a logical eraseblock for writing. Returns zero in case
 * of success and a negative error code in case of failure.
 */
static int leb_write_lock(struct ubi_device *ubi, int vol_id, int lnum)
{
	struct ubi_ltree_entry *le;

	le = ltree_add_entry(ubi, vol_id, lnum);
	if (IS_ERR(le))
		return PTR_ERR(le);
	down_write(&le->mutex);
	return 0;
}

/**
 * leb_write_trylock - try to lock logical eraseblock for writing.
 * @ubi: UBI device description object
 * @vol_id: volume ID
 * @lnum: logical eraseblock number
 *
 * This function locks a logical eraseblock for writing if there is no
 * contention and does nothing if there is contention. Returns %0 in case of
 * success, %1 in case of contention, and and a negative error code in case of
 * failure.
 */
static int leb_write_trylock(struct ubi_device *ubi, int vol_id, int lnum)
{
	struct ubi_ltree_entry *le;

	le = ltree_add_entry(ubi, vol_id, lnum);
	if (IS_ERR(le))
		return PTR_ERR(le);
	if (down_write_trylock(&le->mutex))
		return 0;

	/* Contention, cancel */
	spin_lock(&ubi->ltree_lock);
	le->users -= 1;
	ubi_assert(le->users >= 0);
	if (le->users == 0) {
		rb_erase(&le->rb, &ubi->ltree);
		kfree(le);
	}
	spin_unlock(&ubi->ltree_lock);

	return 1;
}

/**
 * leb_write_unlock - unlock logical eraseblock.
 * @ubi: UBI device description object
 * @vol_id: volume ID
 * @lnum: logical eraseblock number
 */
static void leb_write_unlock(struct ubi_device *ubi, int vol_id, int lnum)
{
	struct ubi_ltree_entry *le;

	spin_lock(&ubi->ltree_lock);
	le = ltree_lookup(ubi, vol_id, lnum);
	le->users -= 1;
	ubi_assert(le->users >= 0);
	up_write(&le->mutex);
	if (le->users == 0) {
		rb_erase(&le->rb, &ubi->ltree);
		kfree(le);
	}
	spin_unlock(&ubi->ltree_lock);
}

/**
 * ubi_eba_is_mapped - check if a LEB is mapped.
 * @vol: volume description object
 * @lnum: logical eraseblock number
 *
 * This function returns true if the LEB is mapped, false otherwise.
 */
bool ubi_eba_is_mapped(struct ubi_volume *vol, int lnum)
{
	return vol->eba_tbl->entries[lnum].pnum >= 0;
}

/**
 * ubi_eba_unmap_leb - un-map logical eraseblock.
 * @ubi: UBI device description object
 * @vol: volume description object
 * @lnum: logical eraseblock number
 *
 * This function un-maps logical eraseblock @lnum and schedules corresponding
 * physical eraseblock for erasure. Returns zero in case of success and a
 * negative error code in case of failure.
 */
int ubi_eba_unmap_leb(struct ubi_device *ubi, struct ubi_volume *vol,
		      int lnum)
{
	int err, pnum, vol_id = vol->vol_id;

	if (ubi->ro_mode)
		return -EROFS;

	err = leb_write_lock(ubi, vol_id, lnum);
	if (err)
		return err;

	pnum = vol->eba_tbl->entries[lnum].pnum;
	if (pnum < 0)
		/* This logical eraseblock is already unmapped */
		goto out_unlock;

	dbg_eba("erase LEB %d:%d, PEB %d", vol_id, lnum, pnum);

	down_read(&ubi->fm_eba_sem);
	vol->eba_tbl->entries[lnum].pnum = UBI_LEB_UNMAPPED;
	up_read(&ubi->fm_eba_sem);
	err = ubi_wl_put_peb(ubi, vol_id, lnum, pnum, 0);

out_unlock:
	leb_write_unlock(ubi, vol_id, lnum);
	return err;
}

#ifdef CONFIG_MTD_UBI_FASTMAP
/**
 * check_mapping - check and fixup a mapping
 * @ubi: UBI device description object
 * @vol: volume description object
 * @lnum: logical eraseblock number
 * @pnum: physical eraseblock number
 *
 * Checks whether a given mapping is valid. Fastmap cannot track LEB unmap
 * operations, if such an operation is interrupted the mapping still looks
 * good, but upon first read an ECC is reported to the upper layer.
 * Normaly during the full-scan at attach time this is fixed, for Fastmap
 * we have to deal with it while reading.
 * If the PEB behind a LEB shows this symthom we change the mapping to
 * %UBI_LEB_UNMAPPED and schedule the PEB for erasure.
 *
 * Returns 0 on success, negative error code in case of failure.
 */
static int check_mapping(struct ubi_device *ubi, struct ubi_volume *vol, int lnum,
			 int *pnum)
{
	int err;
	struct ubi_vid_io_buf *vidb;
	struct ubi_vid_hdr *vid_hdr;

	if (!ubi->fast_attach)
		return 0;

	if (!vol->checkmap || test_bit(lnum, vol->checkmap))
		return 0;

	vidb = ubi_alloc_vid_buf(ubi, GFP_NOFS);
	if (!vidb)
		return -ENOMEM;

	err = ubi_io_read_vid_hdr(ubi, *pnum, vidb, 0);
	if (err > 0 && err != UBI_IO_BITFLIPS) {
		int torture = 0;

		switch (err) {
			case UBI_IO_FF:
			case UBI_IO_FF_BITFLIPS:
			case UBI_IO_BAD_HDR:
			case UBI_IO_BAD_HDR_EBADMSG:
				break;
			default:
				ubi_assert(0);
		}

		if (err == UBI_IO_BAD_HDR_EBADMSG || err == UBI_IO_FF_BITFLIPS)
			torture = 1;

		down_read(&ubi->fm_eba_sem);
		vol->eba_tbl->entries[lnum].pnum = UBI_LEB_UNMAPPED;
		up_read(&ubi->fm_eba_sem);
		ubi_wl_put_peb(ubi, vol->vol_id, lnum, *pnum, torture);

		*pnum = UBI_LEB_UNMAPPED;
	} else if (err < 0) {
		ubi_err(ubi, "unable to read VID header back from PEB %i: %i",
			*pnum, err);

		goto out_free;
	} else {
		int found_vol_id, found_lnum;

		ubi_assert(err == 0 || err == UBI_IO_BITFLIPS);

		vid_hdr = ubi_get_vid_hdr(vidb);
		found_vol_id = be32_to_cpu(vid_hdr->vol_id);
		found_lnum = be32_to_cpu(vid_hdr->lnum);

		if (found_lnum != lnum || found_vol_id != vol->vol_id) {
			ubi_err(ubi, "EBA mismatch! PEB %i is LEB %i:%i instead of LEB %i:%i",
				*pnum, found_vol_id, found_lnum, vol->vol_id, lnum);
			ubi_ro_mode(ubi);
			err = -EINVAL;
			goto out_free;
		}
	}

	set_bit(lnum, vol->checkmap);
	err = 0;

out_free:
	ubi_free_vid_buf(vidb);

	return err;
}
#else
static int check_mapping(struct ubi_device *ubi, struct ubi_volume *vol, int lnum,
		  int *pnum)
{
	return 0;
}
#endif

/**
 * ubi_eba_read_leb - read data.
 * @ubi: UBI device description object
 * @vol: volume description object
 * @lnum: logical eraseblock number
 * @buf: buffer to store the read data
 * @offset: offset from where to read
 * @len: how many bytes to read
 * @check: data CRC check flag
 *
 * If the logical eraseblock @lnum is unmapped, @buf is filled with 0xFF
 * bytes. The @check flag only makes sense for static volumes and forces
 * eraseblock data CRC checking.
 *
 * In case of success this function returns zero. In case of a static volume,
 * if data CRC mismatches - %-EBADMSG is returned. %-EBADMSG may also be
 * returned for any volume type if an ECC error was detected by the MTD device
 * driver. Other negative error cored may be returned in case of other errors.
 */
int ubi_eba_read_leb(struct ubi_device *ubi, struct ubi_volume *vol, int lnum,
		     void *buf, int offset, int len, int check)
{
	int err, pnum, scrub = 0, vol_id = vol->vol_id;
	struct ubi_vid_io_buf *vidb;
	struct ubi_vid_hdr *vid_hdr;
	uint32_t crc;

	err = leb_read_lock(ubi, vol_id, lnum);
	if (err)
		return err;

	pnum = vol->eba_tbl->entries[lnum].pnum;
	if (pnum >= 0) {
		err = check_mapping(ubi, vol, lnum, &pnum);
		if (err < 0)
			goto out_unlock;
	}

	if (pnum == UBI_LEB_UNMAPPED) {
		/*
		 * The logical eraseblock is not mapped, fill the whole buffer
		 * with 0xFF bytes. The exception is static volumes for which
		 * it is an error to read unmapped logical eraseblocks.
		 */
		dbg_eba("read %d bytes from offset %d of LEB %d:%d (unmapped)",
			len, offset, vol_id, lnum);
		leb_read_unlock(ubi, vol_id, lnum);
		ubi_assert(vol->vol_type != UBI_STATIC_VOLUME);
		memset(buf, 0xFF, len);
		return 0;
	}

	dbg_eba("read %d bytes from offset %d of LEB %d:%d, PEB %d",
		len, offset, vol_id, lnum, pnum);

	if (vol->vol_type == UBI_DYNAMIC_VOLUME)
		check = 0;

retry:
	if (check) {
		vidb = ubi_alloc_vid_buf(ubi, GFP_NOFS);
		if (!vidb) {
			err = -ENOMEM;
			goto out_unlock;
		}

		vid_hdr = ubi_get_vid_hdr(vidb);

		err = ubi_io_read_vid_hdr(ubi, pnum, vidb, 1);
		if (err && err != UBI_IO_BITFLIPS) {
			if (err > 0) {
				/*
				 * The header is either absent or corrupted.
				 * The former case means there is a bug -
				 * switch to read-only mode just in case.
				 * The latter case means a real corruption - we
				 * may try to recover data. FIXME: but this is
				 * not implemented.
				 */
				if (err == UBI_IO_BAD_HDR_EBADMSG ||
				    err == UBI_IO_BAD_HDR) {
					ubi_warn(ubi, "corrupted VID header at PEB %d, LEB %d:%d",
						 pnum, vol_id, lnum);
					err = -EBADMSG;
				} else {
					/*
					 * Ending up here in the non-Fastmap case
					 * is a clear bug as the VID header had to
					 * be present at scan time to have it referenced.
					 * With fastmap the story is more complicated.
					 * Fastmap has the mapping info without the need
					 * of a full scan. So the LEB could have been
					 * unmapped, Fastmap cannot know this and keeps
					 * the LEB referenced.
					 * This is valid and works as the layer above UBI
					 * has to do bookkeeping about used/referenced
					 * LEBs in any case.
					 */
					if (ubi->fast_attach) {
						err = -EBADMSG;
					} else {
						err = -EINVAL;
						ubi_ro_mode(ubi);
					}
				}
			}
			goto out_free;
		} else if (err == UBI_IO_BITFLIPS)
			scrub = 1;

		ubi_assert(lnum < be32_to_cpu(vid_hdr->used_ebs));
		ubi_assert(len == be32_to_cpu(vid_hdr->data_size));

		crc = be32_to_cpu(vid_hdr->data_crc);
		ubi_free_vid_buf(vidb);
	}

	err = ubi_io_read_data(ubi, buf, pnum, offset, len);
	if (err) {
		if (err == UBI_IO_BITFLIPS)
			scrub = 1;
		else if (mtd_is_eccerr(err)) {
			if (vol->vol_type == UBI_DYNAMIC_VOLUME)
				goto out_unlock;
			scrub = 1;
			if (!check) {
				ubi_msg(ubi, "force data checking");
				check = 1;
				goto retry;
			}
		} else
			goto out_unlock;
	}

	if (check) {
		uint32_t crc1 = crc32(UBI_CRC32_INIT, buf, len);
		if (crc1 != crc) {
			ubi_warn(ubi, "CRC error: calculated %#08x, must be %#08x",
				 crc1, crc);
			err = -EBADMSG;
			goto out_unlock;
		}
	}

	if (scrub)
		err = ubi_wl_scrub_peb(ubi, pnum);

	leb_read_unlock(ubi, vol_id, lnum);
	return err;

out_free:
	ubi_free_vid_buf(vidb);
out_unlock:
	leb_read_unlock(ubi, vol_id, lnum);
	return err;
}

/**
 * ubi_eba_read_leb_sg - read data into a scatter gather list.
 * @ubi: UBI device description object
 * @vol: volume description object
 * @lnum: logical eraseblock number
 * @sgl: UBI scatter gather list to store the read data
 * @offset: offset from where to read
 * @len: how many bytes to read
 * @check: data CRC check flag
 *
 * This function works exactly like ubi_eba_read_leb(). But instead of
 * storing the read data into a buffer it writes to an UBI scatter gather
 * list.
 */
int ubi_eba_read_leb_sg(struct ubi_device *ubi, struct ubi_volume *vol,
			struct ubi_sgl *sgl, int lnum, int offset, int len,
			int check)
{
	int to_read;
	int ret;
	struct scatterlist *sg;

	for (;;) {
		ubi_assert(sgl->list_pos < UBI_MAX_SG_COUNT);
		sg = &sgl->sg[sgl->list_pos];
		if (len < sg->length - sgl->page_pos)
			to_read = len;
		else
			to_read = sg->length - sgl->page_pos;

		ret = ubi_eba_read_leb(ubi, vol, lnum,
				       sg_virt(sg) + sgl->page_pos, offset,
				       to_read, check);
		if (ret < 0)
			return ret;

		offset += to_read;
		len -= to_read;
		if (!len) {
			sgl->page_pos += to_read;
			if (sgl->page_pos == sg->length) {
				sgl->list_pos++;
				sgl->page_pos = 0;
			}

			break;
		}

		sgl->list_pos++;
		sgl->page_pos = 0;
	}

	return ret;
}

/**
 * try_recover_peb - try to recover from write failure.
 * @vol: volume description object
 * @pnum: the physical eraseblock to recover
 * @lnum: logical eraseblock number
 * @buf: data which was not written because of the write failure
 * @offset: offset of the failed write
 * @len: how many bytes should have been written
 * @vidb: VID buffer
 * @retry: whether the caller should retry in case of failure
 *
 * This function is called in case of a write failure and moves all good data
 * from the potentially bad physical eraseblock to a good physical eraseblock.
 * This function also writes the data which was not written due to the failure.
 * Returns 0 in case of success, and a negative error code in case of failure.
 * In case of failure, the %retry parameter is set to false if this is a fatal
 * error (retrying won't help), and true otherwise.
 */
static int try_recover_peb(struct ubi_volume *vol, int pnum, int lnum,
			   const void *buf, int offset, int len,
			   struct ubi_vid_io_buf *vidb, bool *retry)
{
	struct ubi_device *ubi = vol->ubi;
	struct ubi_vid_hdr *vid_hdr;
	int new_pnum, err, vol_id = vol->vol_id, data_size;
	uint32_t crc;

	*retry = false;

	new_pnum = ubi_wl_get_peb(ubi);
	if (new_pnum < 0) {
		err = new_pnum;
		goto out_put;
	}

	ubi_msg(ubi, "recover PEB %d, move data to PEB %d",
		pnum, new_pnum);

	err = ubi_io_read_vid_hdr(ubi, pnum, vidb, 1);
	if (err && err != UBI_IO_BITFLIPS) {
		if (err > 0)
			err = -EIO;
		goto out_put;
	}

	vid_hdr = ubi_get_vid_hdr(vidb);
	ubi_assert(vid_hdr->vol_type == UBI_VID_DYNAMIC);

	mutex_lock(&ubi->buf_mutex);
	memset(ubi->peb_buf + offset, 0xFF, len);

	/* Read everything before the area where the write failure happened */
	if (offset > 0) {
		err = ubi_io_read_data(ubi, ubi->peb_buf, pnum, 0, offset);
		if (err && err != UBI_IO_BITFLIPS)
			goto out_unlock;
	}

	*retry = true;

	memcpy(ubi->peb_buf + offset, buf, len);

	data_size = offset + len;
	crc = crc32(UBI_CRC32_INIT, ubi->peb_buf, data_size);
	vid_hdr->sqnum = cpu_to_be64(ubi_next_sqnum(ubi));
	vid_hdr->copy_flag = 1;
	vid_hdr->data_size = cpu_to_be32(data_size);
	vid_hdr->data_crc = cpu_to_be32(crc);
	err = ubi_io_write_vid_hdr(ubi, new_pnum, vidb);
	if (err)
		goto out_unlock;

	err = ubi_io_write_data(ubi, ubi->peb_buf, new_pnum, 0, data_size);

out_unlock:
	mutex_unlock(&ubi->buf_mutex);

	if (!err)
		vol->eba_tbl->entries[lnum].pnum = new_pnum;

out_put:
	up_read(&ubi->fm_eba_sem);

	if (!err) {
		ubi_wl_put_peb(ubi, vol_id, lnum, pnum, 1);
		ubi_msg(ubi, "data was successfully recovered");
	} else if (new_pnum >= 0) {
		/*
		 * Bad luck? This physical eraseblock is bad too? Crud. Let's
		 * try to get another one.
		 */
		ubi_wl_put_peb(ubi, vol_id, lnum, new_pnum, 1);
		ubi_warn(ubi, "failed to write to PEB %d", new_pnum);
	}

	return err;
}

/**
 * recover_peb - recover from write failure.
 * @ubi: UBI device description object
 * @pnum: the physical eraseblock to recover
 * @vol_id: volume ID
 * @lnum: logical eraseblock number
 * @buf: data which was not written because of the write failure
 * @offset: offset of the failed write
 * @len: how many bytes should have been written
 *
 * This function is called in case of a write failure and moves all good data
 * from the potentially bad physical eraseblock to a good physical eraseblock.
 * This function also writes the data which was not written due to the failure.
 * Returns 0 in case of success, and a negative error code in case of failure.
 * This function tries %UBI_IO_RETRIES before giving up.
 */
static int recover_peb(struct ubi_device *ubi, int pnum, int vol_id, int lnum,
		       const void *buf, int offset, int len)
{
	int err, idx = vol_id2idx(ubi, vol_id), tries;
	struct ubi_volume *vol = ubi->volumes[idx];
	struct ubi_vid_io_buf *vidb;

	vidb = ubi_alloc_vid_buf(ubi, GFP_NOFS);
	if (!vidb)
		return -ENOMEM;

	for (tries = 0; tries <= UBI_IO_RETRIES; tries++) {
		bool retry;

		err = try_recover_peb(vol, pnum, lnum, buf, offset, len, vidb,
				      &retry);
		if (!err || !retry)
			break;

		ubi_msg(ubi, "try again");
	}

	ubi_free_vid_buf(vidb);

	return err;
}

/**
 * try_write_vid_and_data - try to write VID header and data to a new PEB.
 * @vol: volume description object
 * @lnum: logical eraseblock number
 * @vidb: the VID buffer to write
 * @buf: buffer containing the data
 * @offset: where to start writing data
 * @len: how many bytes should be written
 *
 * This function tries to write VID header and data belonging to logical
 * eraseblock @lnum of volume @vol to a new physical eraseblock. Returns zero
 * in case of success and a negative error code in case of failure.
 * In case of error, it is possible that something was still written to the
 * flash media, but may be some garbage.
 */
static int try_write_vid_and_data(struct ubi_volume *vol, int lnum,
				  struct ubi_vid_io_buf *vidb, const void *buf,
				  int offset, int len)
{
	struct ubi_device *ubi = vol->ubi;
	int pnum, opnum, err, vol_id = vol->vol_id;

	pnum = ubi_wl_get_peb(ubi);
	if (pnum < 0) {
		err = pnum;
		goto out_put;
	}

	opnum = vol->eba_tbl->entries[lnum].pnum;

	dbg_eba("write VID hdr and %d bytes at offset %d of LEB %d:%d, PEB %d",
		len, offset, vol_id, lnum, pnum);

	err = ubi_io_write_vid_hdr(ubi, pnum, vidb);
	if (err) {
		ubi_warn(ubi, "failed to write VID header to LEB %d:%d, PEB %d",
			 vol_id, lnum, pnum);
		goto out_put;
	}

	if (len) {
		err = ubi_io_write_data(ubi, buf, pnum, offset, len);
		if (err) {
			ubi_warn(ubi,
				 "failed to write %d bytes at offset %d of LEB %d:%d, PEB %d",
				 len, offset, vol_id, lnum, pnum);
			goto out_put;
		}
	}

	vol->eba_tbl->entries[lnum].pnum = pnum;

out_put:
	up_read(&ubi->fm_eba_sem);

	if (err && pnum >= 0)
		err = ubi_wl_put_peb(ubi, vol_id, lnum, pnum, 1);
	else if (!err && opnum >= 0)
		err = ubi_wl_put_peb(ubi, vol_id, lnum, opnum, 0);

	return err;
}

/**
 * ubi_eba_write_leb - write data to dynamic volume.
 * @ubi: UBI device description object
 * @vol: volume description object
 * @lnum: logical eraseblock number
 * @buf: the data to write
 * @offset: offset within the logical eraseblock where to write
 * @len: how many bytes to write
 *
 * This function writes data to logical eraseblock @lnum of a dynamic volume
 * @vol. Returns zero in case of success and a negative error code in case
 * of failure. In case of error, it is possible that something was still
 * written to the flash media, but may be some garbage.
 * This function retries %UBI_IO_RETRIES times before giving up.
 */
int ubi_eba_write_leb(struct ubi_device *ubi, struct ubi_volume *vol, int lnum,
		      const void *buf, int offset, int len)
{
	int err, pnum, tries, vol_id = vol->vol_id;
	struct ubi_vid_io_buf *vidb;
	struct ubi_vid_hdr *vid_hdr;

	if (ubi->ro_mode)
		return -EROFS;

	err = leb_write_lock(ubi, vol_id, lnum);
	if (err)
		return err;

	pnum = vol->eba_tbl->entries[lnum].pnum;
	if (pnum >= 0) {
		err = check_mapping(ubi, vol, lnum, &pnum);
		if (err < 0)
			goto out;
	}

	if (pnum >= 0) {
		dbg_eba("write %d bytes at offset %d of LEB %d:%d, PEB %d",
			len, offset, vol_id, lnum, pnum);

		err = ubi_io_write_data(ubi, buf, pnum, offset, len);
		if (err) {
			ubi_warn(ubi, "failed to write data to PEB %d", pnum);
			if (err == -EIO && ubi->bad_allowed)
				err = recover_peb(ubi, pnum, vol_id, lnum, buf,
						  offset, len);
		}

		goto out;
	}

	/*
	 * The logical eraseblock is not mapped. We have to get a free physical
	 * eraseblock and write the volume identifier header there first.
	 */
	vidb = ubi_alloc_vid_buf(ubi, GFP_NOFS);
	if (!vidb) {
		leb_write_unlock(ubi, vol_id, lnum);
		return -ENOMEM;
	}

	vid_hdr = ubi_get_vid_hdr(vidb);

	vid_hdr->vol_type = UBI_VID_DYNAMIC;
	vid_hdr->sqnum = cpu_to_be64(ubi_next_sqnum(ubi));
	vid_hdr->vol_id = cpu_to_be32(vol_id);
	vid_hdr->lnum = cpu_to_be32(lnum);
	vid_hdr->compat = ubi_get_compat(ubi, vol_id);
	vid_hdr->data_pad = cpu_to_be32(vol->data_pad);

	for (tries = 0; tries <= UBI_IO_RETRIES; tries++) {
		err = try_write_vid_and_data(vol, lnum, vidb, buf, offset, len);
		if (err != -EIO || !ubi->bad_allowed)
			break;

		/*
		 * Fortunately, this is the first write operation to this
		 * physical eraseblock, so just put it and request a new one.
		 * We assume that if this physical eraseblock went bad, the
		 * erase code will handle that.
		 */
		vid_hdr->sqnum = cpu_to_be64(ubi_next_sqnum(ubi));
		ubi_msg(ubi, "try another PEB");
	}

	ubi_free_vid_buf(vidb);

out:
	if (err)
		ubi_ro_mode(ubi);

	leb_write_unlock(ubi, vol_id, lnum);

	return err;
}

/**
 * ubi_eba_write_leb_st - write data to static volume.
 * @ubi: UBI device description object
 * @vol: volume description object
 * @lnum: logical eraseblock number
 * @buf: data to write
 * @len: how many bytes to write
 * @used_ebs: how many logical eraseblocks will this volume contain
 *
 * This function writes data to logical eraseblock @lnum of static volume
 * @vol. The @used_ebs argument should contain total number of logical
 * eraseblock in this static volume.
 *
 * When writing to the last logical eraseblock, the @len argument doesn't have
 * to be aligned to the minimal I/O unit size. Instead, it has to be equivalent
 * to the real data size, although the @buf buffer has to contain the
 * alignment. In all other cases, @len has to be aligned.
 *
 * It is prohibited to write more than once to logical eraseblocks of static
 * volumes. This function returns zero in case of success and a negative error
 * code in case of failure.
 */
int ubi_eba_write_leb_st(struct ubi_device *ubi, struct ubi_volume *vol,
			 int lnum, const void *buf, int len, int used_ebs)
{
	int err, tries, data_size = len, vol_id = vol->vol_id;
	struct ubi_vid_io_buf *vidb;
	struct ubi_vid_hdr *vid_hdr;
	uint32_t crc;

	if (ubi->ro_mode)
		return -EROFS;

	if (lnum == used_ebs - 1)
		/* If this is the last LEB @len may be unaligned */
		len = ALIGN(data_size, ubi->min_io_size);
	else
		ubi_assert(!(len & (ubi->min_io_size - 1)));

	vidb = ubi_alloc_vid_buf(ubi, GFP_NOFS);
	if (!vidb)
		return -ENOMEM;

	vid_hdr = ubi_get_vid_hdr(vidb);

	err = leb_write_lock(ubi, vol_id, lnum);
	if (err)
		goto out;

	vid_hdr->sqnum = cpu_to_be64(ubi_next_sqnum(ubi));
	vid_hdr->vol_id = cpu_to_be32(vol_id);
	vid_hdr->lnum = cpu_to_be32(lnum);
	vid_hdr->compat = ubi_get_compat(ubi, vol_id);
	vid_hdr->data_pad = cpu_to_be32(vol->data_pad);

	crc = crc32(UBI_CRC32_INIT, buf, data_size);
	vid_hdr->vol_type = UBI_VID_STATIC;
	vid_hdr->data_size = cpu_to_be32(data_size);
	vid_hdr->used_ebs = cpu_to_be32(used_ebs);
	vid_hdr->data_crc = cpu_to_be32(crc);

	ubi_assert(vol->eba_tbl->entries[lnum].pnum < 0);

	for (tries = 0; tries <= UBI_IO_RETRIES; tries++) {
		err = try_write_vid_and_data(vol, lnum, vidb, buf, 0, len);
		if (err != -EIO || !ubi->bad_allowed)
			break;

		vid_hdr->sqnum = cpu_to_be64(ubi_next_sqnum(ubi));
		ubi_msg(ubi, "try another PEB");
	}

	if (err)
		ubi_ro_mode(ubi);

	leb_write_unlock(ubi, vol_id, lnum);

out:
	ubi_free_vid_buf(vidb);

	return err;
}

/*
 * ubi_eba_atomic_leb_change - change logical eraseblock atomically.
 * @ubi: UBI device description object
 * @vol: volume description object
 * @lnum: logical eraseblock number
 * @buf: data to write
 * @len: how many bytes to write
 *
 * This function changes the contents of a logical eraseblock atomically. @buf
 * has to contain new logical eraseblock data, and @len - the length of the
 * data, which has to be aligned. This function guarantees that in case of an
 * unclean reboot the old contents is preserved. Returns zero in case of
 * success and a negative error code in case of failure.
 *
 * UBI reserves one LEB for the "atomic LEB change" operation, so only one
 * LEB change may be done at a time. This is ensured by @ubi->alc_mutex.
 */
int ubi_eba_atomic_leb_change(struct ubi_device *ubi, struct ubi_volume *vol,
			      int lnum, const void *buf, int len)
{
	int err, tries, vol_id = vol->vol_id;
	struct ubi_vid_io_buf *vidb;
	struct ubi_vid_hdr *vid_hdr;
	uint32_t crc;

	if (ubi->ro_mode)
		return -EROFS;

	if (len == 0) {
		/*
		 * Special case when data length is zero. In this case the LEB
		 * has to be unmapped and mapped somewhere else.
		 */
		err = ubi_eba_unmap_leb(ubi, vol, lnum);
		if (err)
			return err;
		return ubi_eba_write_leb(ubi, vol, lnum, NULL, 0, 0);
	}

	vidb = ubi_alloc_vid_buf(ubi, GFP_NOFS);
	if (!vidb)
		return -ENOMEM;

	vid_hdr = ubi_get_vid_hdr(vidb);

	mutex_lock(&ubi->alc_mutex);
	err = leb_write_lock(ubi, vol_id, lnum);
	if (err)
		goto out_mutex;

	vid_hdr->sqnum = cpu_to_be64(ubi_next_sqnum(ubi));
	vid_hdr->vol_id = cpu_to_be32(vol_id);
	vid_hdr->lnum = cpu_to_be32(lnum);
	vid_hdr->compat = ubi_get_compat(ubi, vol_id);
	vid_hdr->data_pad = cpu_to_be32(vol->data_pad);

	crc = crc32(UBI_CRC32_INIT, buf, len);
	vid_hdr->vol_type = UBI_VID_DYNAMIC;
	vid_hdr->data_size = cpu_to_be32(len);
	vid_hdr->copy_flag = 1;
	vid_hdr->data_crc = cpu_to_be32(crc);

	dbg_eba("change LEB %d:%d", vol_id, lnum);

	for (tries = 0; tries <= UBI_IO_RETRIES; tries++) {
		err = try_write_vid_and_data(vol, lnum, vidb, buf, 0, len);
		if (err != -EIO || !ubi->bad_allowed)
			break;

		vid_hdr->sqnum = cpu_to_be64(ubi_next_sqnum(ubi));
		ubi_msg(ubi, "try another PEB");
	}

	/*
	 * This flash device does not admit of bad eraseblocks or
	 * something nasty and unexpected happened. Switch to read-only
	 * mode just in case.
	 */
	if (err)
		ubi_ro_mode(ubi);

	leb_write_unlock(ubi, vol_id, lnum);

out_mutex:
	mutex_unlock(&ubi->alc_mutex);
	ubi_free_vid_buf(vidb);
	return err;
}

/**
 * is_error_sane - check whether a read error is sane.
 * @err: code of the error happened during reading
 *
 * This is a helper function for 'ubi_eba_copy_leb()' which is called when we
 * cannot read data from the target PEB (an error @err happened). If the error
 * code is sane, then we treat this error as non-fatal. Otherwise the error is
 * fatal and UBI will be switched to R/O mode later.
 *
 * The idea is that we try not to switch to R/O mode if the read error is
 * something which suggests there was a real read problem. E.g., %-EIO. Or a
 * memory allocation failed (-%ENOMEM). Otherwise, it is safer to switch to R/O
 * mode, simply because we do not know what happened at the MTD level, and we
 * cannot handle this. E.g., the underlying driver may have become crazy, and
 * it is safer to switch to R/O mode to preserve the data.
 *
 * And bear in mind, this is about reading from the target PEB, i.e. the PEB
 * which we have just written.
 */
static int is_error_sane(int err)
{
	if (err == -EIO || err == -ENOMEM || err == UBI_IO_BAD_HDR ||
	    err == UBI_IO_BAD_HDR_EBADMSG || err == -ETIMEDOUT)
		return 0;
	return 1;
}

/**
 * ubi_eba_copy_leb - copy logical eraseblock.
 * @ubi: UBI device description object
 * @from: physical eraseblock number from where to copy
 * @to: physical eraseblock number where to copy
 * @vidb: data structure from where the VID header is derived
 *
 * This function copies logical eraseblock from physical eraseblock @from to
 * physical eraseblock @to. The @vid_hdr buffer may be changed by this
 * function. Returns:
 *   o %0 in case of success;
 *   o %MOVE_CANCEL_RACE, %MOVE_TARGET_WR_ERR, %MOVE_TARGET_BITFLIPS, etc;
 *   o a negative error code in case of failure.
 */
int ubi_eba_copy_leb(struct ubi_device *ubi, int from, int to,
		     struct ubi_vid_io_buf *vidb)
{
	int err, vol_id, lnum, data_size, aldata_size, idx;
	struct ubi_vid_hdr *vid_hdr = ubi_get_vid_hdr(vidb);
	struct ubi_volume *vol;
	uint32_t crc;

	ubi_assert(rwsem_is_locked(&ubi->fm_eba_sem));

	vol_id = be32_to_cpu(vid_hdr->vol_id);
	lnum = be32_to_cpu(vid_hdr->lnum);

	dbg_wl("copy LEB %d:%d, PEB %d to PEB %d", vol_id, lnum, from, to);

	if (vid_hdr->vol_type == UBI_VID_STATIC) {
		data_size = be32_to_cpu(vid_hdr->data_size);
		aldata_size = ALIGN(data_size, ubi->min_io_size);
	} else
		data_size = aldata_size =
			    ubi->leb_size - be32_to_cpu(vid_hdr->data_pad);

	idx = vol_id2idx(ubi, vol_id);
	spin_lock(&ubi->volumes_lock);
	/*
	 * Note, we may race with volume deletion, which means that the volume
	 * this logical eraseblock belongs to might be being deleted. Since the
	 * volume deletion un-maps all the volume's logical eraseblocks, it will
	 * be locked in 'ubi_wl_put_peb()' and wait for the WL worker to finish.
	 */
	vol = ubi->volumes[idx];
	spin_unlock(&ubi->volumes_lock);
	if (!vol) {
		/* No need to do further work, cancel */
		dbg_wl("volume %d is being removed, cancel", vol_id);
		return MOVE_CANCEL_RACE;
	}

	/*
	 * We do not want anybody to write to this logical eraseblock while we
	 * are moving it, so lock it.
	 *
	 * Note, we are using non-waiting locking here, because we cannot sleep
	 * on the LEB, since it may cause deadlocks. Indeed, imagine a task is
	 * unmapping the LEB which is mapped to the PEB we are going to move
	 * (@from). This task locks the LEB and goes sleep in the
	 * 'ubi_wl_put_peb()' function on the @ubi->move_mutex. In turn, we are
	 * holding @ubi->move_mutex and go sleep on the LEB lock. So, if the
	 * LEB is already locked, we just do not move it and return
	 * %MOVE_RETRY. Note, we do not return %MOVE_CANCEL_RACE here because
	 * we do not know the reasons of the contention - it may be just a
	 * normal I/O on this LEB, so we want to re-try.
	 */
	err = leb_write_trylock(ubi, vol_id, lnum);
	if (err) {
		dbg_wl("contention on LEB %d:%d, cancel", vol_id, lnum);
		return MOVE_RETRY;
	}

	/*
	 * The LEB might have been put meanwhile, and the task which put it is
	 * probably waiting on @ubi->move_mutex. No need to continue the work,
	 * cancel it.
	 */
	if (vol->eba_tbl->entries[lnum].pnum != from) {
		dbg_wl("LEB %d:%d is no longer mapped to PEB %d, mapped to PEB %d, cancel",
		       vol_id, lnum, from, vol->eba_tbl->entries[lnum].pnum);
		err = MOVE_CANCEL_RACE;
		goto out_unlock_leb;
	}

	/*
	 * OK, now the LEB is locked and we can safely start moving it. Since
	 * this function utilizes the @ubi->peb_buf buffer which is shared
	 * with some other functions - we lock the buffer by taking the
	 * @ubi->buf_mutex.
	 */
	mutex_lock(&ubi->buf_mutex);
	dbg_wl("read %d bytes of data", aldata_size);
	err = ubi_io_read_data(ubi, ubi->peb_buf, from, 0, aldata_size);
	if (err && err != UBI_IO_BITFLIPS) {
		ubi_warn(ubi, "error %d while reading data from PEB %d",
			 err, from);
		err = MOVE_SOURCE_RD_ERR;
		goto out_unlock_buf;
	}

	/*
	 * Now we have got to calculate how much data we have to copy. In
	 * case of a static volume it is fairly easy - the VID header contains
	 * the data size. In case of a dynamic volume it is more difficult - we
	 * have to read the contents, cut 0xFF bytes from the end and copy only
	 * the first part. We must do this to avoid writing 0xFF bytes as it
	 * may have some side-effects. And not only this. It is important not
	 * to include those 0xFFs to CRC because later the they may be filled
	 * by data.
	 */
	if (vid_hdr->vol_type == UBI_VID_DYNAMIC)
		aldata_size = data_size =
			ubi_calc_data_len(ubi, ubi->peb_buf, data_size);

	cond_resched();
	crc = crc32(UBI_CRC32_INIT, ubi->peb_buf, data_size);
	cond_resched();

	/*
	 * It may turn out to be that the whole @from physical eraseblock
	 * contains only 0xFF bytes. Then we have to only write the VID header
	 * and do not write any data. This also means we should not set
	 * @vid_hdr->copy_flag, @vid_hdr->data_size, and @vid_hdr->data_crc.
	 */
	if (data_size > 0) {
		vid_hdr->copy_flag = 1;
		vid_hdr->data_size = cpu_to_be32(data_size);
		vid_hdr->data_crc = cpu_to_be32(crc);
	}
	vid_hdr->sqnum = cpu_to_be64(ubi_next_sqnum(ubi));

	err = ubi_io_write_vid_hdr(ubi, to, vidb);
	if (err) {
		if (err == -EIO)
			err = MOVE_TARGET_WR_ERR;
		goto out_unlock_buf;
	}

	cond_resched();

	/* Read the VID header back and check if it was written correctly */
	err = ubi_io_read_vid_hdr(ubi, to, vidb, 1);
	if (err) {
		if (err != UBI_IO_BITFLIPS) {
			ubi_warn(ubi, "error %d while reading VID header back from PEB %d",
				 err, to);
			if (is_error_sane(err))
				err = MOVE_TARGET_RD_ERR;
		} else
			err = MOVE_TARGET_BITFLIPS;
		goto out_unlock_buf;
	}

	if (data_size > 0) {
		err = ubi_io_write_data(ubi, ubi->peb_buf, to, 0, aldata_size);
		if (err) {
			if (err == -EIO)
				err = MOVE_TARGET_WR_ERR;
			goto out_unlock_buf;
		}

		cond_resched();
	}

	ubi_assert(vol->eba_tbl->entries[lnum].pnum == from);
	vol->eba_tbl->entries[lnum].pnum = to;

out_unlock_buf:
	mutex_unlock(&ubi->buf_mutex);
out_unlock_leb:
	leb_write_unlock(ubi, vol_id, lnum);
	return err;
}

/**
 * print_rsvd_warning - warn about not having enough reserved PEBs.
 * @ubi: UBI device description object
 * @ai: UBI attach info object
 *
 * This is a helper function for 'ubi_eba_init()' which is called when UBI
 * cannot reserve enough PEBs for bad block handling. This function makes a
 * decision whether we have to print a warning or not. The algorithm is as
 * follows:
 *   o if this is a new UBI image, then just print the warning
 *   o if this is an UBI image which has already been used for some time, print
 *     a warning only if we can reserve less than 10% of the expected amount of
 *     the reserved PEB.
 *
 * The idea is that when UBI is used, PEBs become bad, and the reserved pool
 * of PEBs becomes smaller, which is normal and we do not want to scare users
 * with a warning every time they attach the MTD device. This was an issue
 * reported by real users.
 */
static void print_rsvd_warning(struct ubi_device *ubi,
			       struct ubi_attach_info *ai)
{
	/*
	 * The 1 << 18 (256KiB) number is picked randomly, just a reasonably
	 * large number to distinguish between newly flashed and used images.
	 */
	if (ai->max_sqnum > (1 << 18)) {
		int min = ubi->beb_rsvd_level / 10;

		if (!min)
			min = 1;
		if (ubi->beb_rsvd_pebs > min)
			return;
	}

	ubi_warn(ubi, "cannot reserve enough PEBs for bad PEB handling, reserved %d, need %d",
		 ubi->beb_rsvd_pebs, ubi->beb_rsvd_level);
	if (ubi->corr_peb_count)
		ubi_warn(ubi, "%d PEBs are corrupted and not used",
			 ubi->corr_peb_count);
}

/**
 * self_check_eba - run a self check on the EBA table constructed by fastmap.
 * @ubi: UBI device description object
 * @ai_fastmap: UBI attach info object created by fastmap
 * @ai_scan: UBI attach info object created by scanning
 *
 * Returns < 0 in case of an internal error, 0 otherwise.
 * If a bad EBA table entry was found it will be printed out and
 * ubi_assert() triggers.
 */
int self_check_eba(struct ubi_device *ubi, struct ubi_attach_info *ai_fastmap,
		   struct ubi_attach_info *ai_scan)
{
	int i, j, num_volumes, ret = 0;
	int **scan_eba, **fm_eba;
	struct ubi_ainf_volume *av;
	struct ubi_volume *vol;
	struct ubi_ainf_peb *aeb;
	struct rb_node *rb;

	num_volumes = ubi->vtbl_slots + UBI_INT_VOL_COUNT;

	scan_eba = kmalloc_array(num_volumes, sizeof(*scan_eba), GFP_KERNEL);
	if (!scan_eba)
		return -ENOMEM;

	fm_eba = kmalloc_array(num_volumes, sizeof(*fm_eba), GFP_KERNEL);
	if (!fm_eba) {
		kfree(scan_eba);
		return -ENOMEM;
	}

	for (i = 0; i < num_volumes; i++) {
		vol = ubi->volumes[i];
		if (!vol)
			continue;

		scan_eba[i] = kmalloc_array(vol->reserved_pebs,
					    sizeof(**scan_eba),
					    GFP_KERNEL);
		if (!scan_eba[i]) {
			ret = -ENOMEM;
			goto out_free;
		}

		fm_eba[i] = kmalloc_array(vol->reserved_pebs,
					  sizeof(**fm_eba),
					  GFP_KERNEL);
		if (!fm_eba[i]) {
			ret = -ENOMEM;
			goto out_free;
		}

		for (j = 0; j < vol->reserved_pebs; j++)
			scan_eba[i][j] = fm_eba[i][j] = UBI_LEB_UNMAPPED;

		av = ubi_find_av(ai_scan, idx2vol_id(ubi, i));
		if (!av)
			continue;

		ubi_rb_for_each_entry(rb, aeb, &av->root, u.rb)
			scan_eba[i][aeb->lnum] = aeb->pnum;

		av = ubi_find_av(ai_fastmap, idx2vol_id(ubi, i));
		if (!av)
			continue;

		ubi_rb_for_each_entry(rb, aeb, &av->root, u.rb)
			fm_eba[i][aeb->lnum] = aeb->pnum;

		for (j = 0; j < vol->reserved_pebs; j++) {
			if (scan_eba[i][j] != fm_eba[i][j]) {
				if (scan_eba[i][j] == UBI_LEB_UNMAPPED ||
					fm_eba[i][j] == UBI_LEB_UNMAPPED)
					continue;

				ubi_err(ubi, "LEB:%i:%i is PEB:%i instead of %i!",
					vol->vol_id, j, fm_eba[i][j],
					scan_eba[i][j]);
				ubi_assert(0);
			}
		}
	}

out_free:
	for (i = 0; i < num_volumes; i++) {
		if (!ubi->volumes[i])
			continue;

		kfree(scan_eba[i]);
		kfree(fm_eba[i]);
	}

	kfree(scan_eba);
	kfree(fm_eba);
	return ret;
}

/**
 * ubi_eba_init - initialize the EBA sub-system using attaching information.
 * @ubi: UBI device description object
 * @ai: attaching information
 *
 * This function returns zero in case of success and a negative error code in
 * case of failure.
 */
int ubi_eba_init(struct ubi_device *ubi, struct ubi_attach_info *ai)
{
	int i, err, num_volumes;
	struct ubi_ainf_volume *av;
	struct ubi_volume *vol;
	struct ubi_ainf_peb *aeb;
	struct rb_node *rb;

	dbg_eba("initialize EBA sub-system");

	spin_lock_init(&ubi->ltree_lock);
	mutex_init(&ubi->alc_mutex);
	ubi->ltree = RB_ROOT;

	ubi->global_sqnum = ai->max_sqnum + 1;
	num_volumes = ubi->vtbl_slots + UBI_INT_VOL_COUNT;

	for (i = 0; i < num_volumes; i++) {
		struct ubi_eba_table *tbl;

		vol = ubi->volumes[i];
		if (!vol)
			continue;

		cond_resched();

		tbl = ubi_eba_create_table(vol, vol->reserved_pebs);
		if (IS_ERR(tbl)) {
			err = PTR_ERR(tbl);
			goto out_free;
		}

		ubi_eba_replace_table(vol, tbl);

		av = ubi_find_av(ai, idx2vol_id(ubi, i));
		if (!av)
			continue;

		ubi_rb_for_each_entry(rb, aeb, &av->root, u.rb) {
			if (aeb->lnum >= vol->reserved_pebs) {
				/*
				 * This may happen in case of an unclean reboot
				 * during re-size.
				 */
				ubi_move_aeb_to_list(av, aeb, &ai->erase);
			} else {
				struct ubi_eba_entry *entry;

				entry = &vol->eba_tbl->entries[aeb->lnum];
				entry->pnum = aeb->pnum;
			}
		}
	}

	if (ubi->avail_pebs < EBA_RESERVED_PEBS) {
		ubi_err(ubi, "no enough physical eraseblocks (%d, need %d)",
			ubi->avail_pebs, EBA_RESERVED_PEBS);
		if (ubi->corr_peb_count)
			ubi_err(ubi, "%d PEBs are corrupted and not used",
				ubi->corr_peb_count);
		err = -ENOSPC;
		goto out_free;
	}
	ubi->avail_pebs -= EBA_RESERVED_PEBS;
	ubi->rsvd_pebs += EBA_RESERVED_PEBS;

	if (ubi->bad_allowed) {
		ubi_calculate_reserved(ubi);

		if (ubi->avail_pebs < ubi->beb_rsvd_level) {
			/* No enough free physical eraseblocks */
			ubi->beb_rsvd_pebs = ubi->avail_pebs;
			print_rsvd_warning(ubi, ai);
		} else
			ubi->beb_rsvd_pebs = ubi->beb_rsvd_level;

		ubi->avail_pebs -= ubi->beb_rsvd_pebs;
		ubi->rsvd_pebs  += ubi->beb_rsvd_pebs;
	}

	dbg_eba("EBA sub-system is initialized");
	return 0;

out_free:
	for (i = 0; i < num_volumes; i++) {
		if (!ubi->volumes[i])
			continue;
		ubi_eba_replace_table(ubi->volumes[i], NULL);
	}
	return err;
}
