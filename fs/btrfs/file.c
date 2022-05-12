// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2007 Oracle.  All rights reserved.
 */

#include <linux/fs.h>
#include <linux/pagemap.h>
#include <linux/time.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/backing-dev.h>
#include <linux/falloc.h>
#include <linux/writeback.h>
#include <linux/compat.h>
#include <linux/slab.h>
#include <linux/btrfs.h>
#include <linux/uio.h>
#include <linux/iversion.h>
#include "ctree.h"
#include "disk-io.h"
#include "transaction.h"
#include "btrfs_inode.h"
#include "print-tree.h"
#include "tree-log.h"
#include "locking.h"
#include "volumes.h"
#include "qgroup.h"
#include "compression.h"
#include "delalloc-space.h"
#include "reflink.h"

static struct kmem_cache *btrfs_inode_defrag_cachep;
/*
 * when auto defrag is enabled we
 * queue up these defrag structs to remember which
 * inodes need defragging passes
 */
struct inode_defrag {
	struct rb_node rb_node;
	/* objectid */
	u64 ino;
	/*
	 * transid where the defrag was added, we search for
	 * extents newer than this
	 */
	u64 transid;

	/* root objectid */
	u64 root;

	/* last offset we were able to defrag */
	u64 last_offset;

	/* if we've wrapped around back to zero once already */
	int cycled;
};

static int __compare_inode_defrag(struct inode_defrag *defrag1,
				  struct inode_defrag *defrag2)
{
	if (defrag1->root > defrag2->root)
		return 1;
	else if (defrag1->root < defrag2->root)
		return -1;
	else if (defrag1->ino > defrag2->ino)
		return 1;
	else if (defrag1->ino < defrag2->ino)
		return -1;
	else
		return 0;
}

/* pop a record for an inode into the defrag tree.  The lock
 * must be held already
 *
 * If you're inserting a record for an older transid than an
 * existing record, the transid already in the tree is lowered
 *
 * If an existing record is found the defrag item you
 * pass in is freed
 */
static int __btrfs_add_inode_defrag(struct btrfs_inode *inode,
				    struct inode_defrag *defrag)
{
	struct btrfs_fs_info *fs_info = inode->root->fs_info;
	struct inode_defrag *entry;
	struct rb_node **p;
	struct rb_node *parent = NULL;
	int ret;

	p = &fs_info->defrag_inodes.rb_node;
	while (*p) {
		parent = *p;
		entry = rb_entry(parent, struct inode_defrag, rb_node);

		ret = __compare_inode_defrag(defrag, entry);
		if (ret < 0)
			p = &parent->rb_left;
		else if (ret > 0)
			p = &parent->rb_right;
		else {
			/* if we're reinserting an entry for
			 * an old defrag run, make sure to
			 * lower the transid of our existing record
			 */
			if (defrag->transid < entry->transid)
				entry->transid = defrag->transid;
			if (defrag->last_offset > entry->last_offset)
				entry->last_offset = defrag->last_offset;
			return -EEXIST;
		}
	}
	set_bit(BTRFS_INODE_IN_DEFRAG, &inode->runtime_flags);
	rb_link_node(&defrag->rb_node, parent, p);
	rb_insert_color(&defrag->rb_node, &fs_info->defrag_inodes);
	return 0;
}

static inline int __need_auto_defrag(struct btrfs_fs_info *fs_info)
{
	if (!btrfs_test_opt(fs_info, AUTO_DEFRAG))
		return 0;

	if (btrfs_fs_closing(fs_info))
		return 0;

	return 1;
}

/*
 * insert a defrag record for this inode if auto defrag is
 * enabled
 */
int btrfs_add_inode_defrag(struct btrfs_trans_handle *trans,
			   struct btrfs_inode *inode)
{
	struct btrfs_root *root = inode->root;
	struct btrfs_fs_info *fs_info = root->fs_info;
	struct inode_defrag *defrag;
	u64 transid;
	int ret;

	if (!__need_auto_defrag(fs_info))
		return 0;

	if (test_bit(BTRFS_INODE_IN_DEFRAG, &inode->runtime_flags))
		return 0;

	if (trans)
		transid = trans->transid;
	else
		transid = inode->root->last_trans;

	defrag = kmem_cache_zalloc(btrfs_inode_defrag_cachep, GFP_NOFS);
	if (!defrag)
		return -ENOMEM;

	defrag->ino = btrfs_ino(inode);
	defrag->transid = transid;
	defrag->root = root->root_key.objectid;

	spin_lock(&fs_info->defrag_inodes_lock);
	if (!test_bit(BTRFS_INODE_IN_DEFRAG, &inode->runtime_flags)) {
		/*
		 * If we set IN_DEFRAG flag and evict the inode from memory,
		 * and then re-read this inode, this new inode doesn't have
		 * IN_DEFRAG flag. At the case, we may find the existed defrag.
		 */
		ret = __btrfs_add_inode_defrag(inode, defrag);
		if (ret)
			kmem_cache_free(btrfs_inode_defrag_cachep, defrag);
	} else {
		kmem_cache_free(btrfs_inode_defrag_cachep, defrag);
	}
	spin_unlock(&fs_info->defrag_inodes_lock);
	return 0;
}

/*
 * Requeue the defrag object. If there is a defrag object that points to
 * the same inode in the tree, we will merge them together (by
 * __btrfs_add_inode_defrag()) and free the one that we want to requeue.
 */
static void btrfs_requeue_inode_defrag(struct btrfs_inode *inode,
				       struct inode_defrag *defrag)
{
	struct btrfs_fs_info *fs_info = inode->root->fs_info;
	int ret;

	if (!__need_auto_defrag(fs_info))
		goto out;

	/*
	 * Here we don't check the IN_DEFRAG flag, because we need merge
	 * them together.
	 */
	spin_lock(&fs_info->defrag_inodes_lock);
	ret = __btrfs_add_inode_defrag(inode, defrag);
	spin_unlock(&fs_info->defrag_inodes_lock);
	if (ret)
		goto out;
	return;
out:
	kmem_cache_free(btrfs_inode_defrag_cachep, defrag);
}

/*
 * pick the defragable inode that we want, if it doesn't exist, we will get
 * the next one.
 */
static struct inode_defrag *
btrfs_pick_defrag_inode(struct btrfs_fs_info *fs_info, u64 root, u64 ino)
{
	struct inode_defrag *entry = NULL;
	struct inode_defrag tmp;
	struct rb_node *p;
	struct rb_node *parent = NULL;
	int ret;

	tmp.ino = ino;
	tmp.root = root;

	spin_lock(&fs_info->defrag_inodes_lock);
	p = fs_info->defrag_inodes.rb_node;
	while (p) {
		parent = p;
		entry = rb_entry(parent, struct inode_defrag, rb_node);

		ret = __compare_inode_defrag(&tmp, entry);
		if (ret < 0)
			p = parent->rb_left;
		else if (ret > 0)
			p = parent->rb_right;
		else
			goto out;
	}

	if (parent && __compare_inode_defrag(&tmp, entry) > 0) {
		parent = rb_next(parent);
		if (parent)
			entry = rb_entry(parent, struct inode_defrag, rb_node);
		else
			entry = NULL;
	}
out:
	if (entry)
		rb_erase(parent, &fs_info->defrag_inodes);
	spin_unlock(&fs_info->defrag_inodes_lock);
	return entry;
}

void btrfs_cleanup_defrag_inodes(struct btrfs_fs_info *fs_info)
{
	struct inode_defrag *defrag;
	struct rb_node *node;

	spin_lock(&fs_info->defrag_inodes_lock);
	node = rb_first(&fs_info->defrag_inodes);
	while (node) {
		rb_erase(node, &fs_info->defrag_inodes);
		defrag = rb_entry(node, struct inode_defrag, rb_node);
		kmem_cache_free(btrfs_inode_defrag_cachep, defrag);

		cond_resched_lock(&fs_info->defrag_inodes_lock);

		node = rb_first(&fs_info->defrag_inodes);
	}
	spin_unlock(&fs_info->defrag_inodes_lock);
}

#define BTRFS_DEFRAG_BATCH	1024

static int __btrfs_run_defrag_inode(struct btrfs_fs_info *fs_info,
				    struct inode_defrag *defrag)
{
	struct btrfs_root *inode_root;
	struct inode *inode;
	struct btrfs_ioctl_defrag_range_args range;
	int num_defrag;
	int ret;

	/* get the inode */
	inode_root = btrfs_get_fs_root(fs_info, defrag->root, true);
	if (IS_ERR(inode_root)) {
		ret = PTR_ERR(inode_root);
		goto cleanup;
	}

	inode = btrfs_iget(fs_info->sb, defrag->ino, inode_root);
	btrfs_put_root(inode_root);
	if (IS_ERR(inode)) {
		ret = PTR_ERR(inode);
		goto cleanup;
	}

	/* do a chunk of defrag */
	clear_bit(BTRFS_INODE_IN_DEFRAG, &BTRFS_I(inode)->runtime_flags);
	memset(&range, 0, sizeof(range));
	range.len = (u64)-1;
	range.start = defrag->last_offset;

	sb_start_write(fs_info->sb);
	num_defrag = btrfs_defrag_file(inode, NULL, &range, defrag->transid,
				       BTRFS_DEFRAG_BATCH);
	sb_end_write(fs_info->sb);
	/*
	 * if we filled the whole defrag batch, there
	 * must be more work to do.  Queue this defrag
	 * again
	 */
	if (num_defrag == BTRFS_DEFRAG_BATCH) {
		defrag->last_offset = range.start;
		btrfs_requeue_inode_defrag(BTRFS_I(inode), defrag);
	} else if (defrag->last_offset && !defrag->cycled) {
		/*
		 * we didn't fill our defrag batch, but
		 * we didn't start at zero.  Make sure we loop
		 * around to the start of the file.
		 */
		defrag->last_offset = 0;
		defrag->cycled = 1;
		btrfs_requeue_inode_defrag(BTRFS_I(inode), defrag);
	} else {
		kmem_cache_free(btrfs_inode_defrag_cachep, defrag);
	}

	iput(inode);
	return 0;
cleanup:
	kmem_cache_free(btrfs_inode_defrag_cachep, defrag);
	return ret;
}

/*
 * run through the list of inodes in the FS that need
 * defragging
 */
int btrfs_run_defrag_inodes(struct btrfs_fs_info *fs_info)
{
	struct inode_defrag *defrag;
	u64 first_ino = 0;
	u64 root_objectid = 0;

	atomic_inc(&fs_info->defrag_running);
	while (1) {
		/* Pause the auto defragger. */
		if (test_bit(BTRFS_FS_STATE_REMOUNTING,
			     &fs_info->fs_state))
			break;

		if (!__need_auto_defrag(fs_info))
			break;

		/* find an inode to defrag */
		defrag = btrfs_pick_defrag_inode(fs_info, root_objectid,
						 first_ino);
		if (!defrag) {
			if (root_objectid || first_ino) {
				root_objectid = 0;
				first_ino = 0;
				continue;
			} else {
				break;
			}
		}

		first_ino = defrag->ino + 1;
		root_objectid = defrag->root;

		__btrfs_run_defrag_inode(fs_info, defrag);
	}
	atomic_dec(&fs_info->defrag_running);

	/*
	 * during unmount, we use the transaction_wait queue to
	 * wait for the defragger to stop
	 */
	wake_up(&fs_info->transaction_wait);
	return 0;
}

/* simple helper to fault in pages and copy.  This should go away
 * and be replaced with calls into generic code.
 */
static noinline int btrfs_copy_from_user(loff_t pos, size_t write_bytes,
					 struct page **prepared_pages,
					 struct iov_iter *i)
{
	size_t copied = 0;
	size_t total_copied = 0;
	int pg = 0;
	int offset = offset_in_page(pos);

	while (write_bytes > 0) {
		size_t count = min_t(size_t,
				     PAGE_SIZE - offset, write_bytes);
		struct page *page = prepared_pages[pg];
		/*
		 * Copy data from userspace to the current page
		 */
		copied = iov_iter_copy_from_user_atomic(page, i, offset, count);

		/* Flush processor's dcache for this page */
		flush_dcache_page(page);

		/*
		 * if we get a partial write, we can end up with
		 * partially up to date pages.  These add
		 * a lot of complexity, so make sure they don't
		 * happen by forcing this copy to be retried.
		 *
		 * The rest of the btrfs_file_write code will fall
		 * back to page at a time copies after we return 0.
		 */
		if (!PageUptodate(page) && copied < count)
			copied = 0;

		iov_iter_advance(i, copied);
		write_bytes -= copied;
		total_copied += copied;

		/* Return to btrfs_file_write_iter to fault page */
		if (unlikely(copied == 0))
			break;

		if (copied < PAGE_SIZE - offset) {
			offset += copied;
		} else {
			pg++;
			offset = 0;
		}
	}
	return total_copied;
}

/*
 * unlocks pages after btrfs_file_write is done with them
 */
static void btrfs_drop_pages(struct page **pages, size_t num_pages)
{
	size_t i;
	for (i = 0; i < num_pages; i++) {
		/* page checked is some magic around finding pages that
		 * have been modified without going through btrfs_set_page_dirty
		 * clear it here. There should be no need to mark the pages
		 * accessed as prepare_pages should have marked them accessed
		 * in prepare_pages via find_or_create_page()
		 */
		ClearPageChecked(pages[i]);
		unlock_page(pages[i]);
		put_page(pages[i]);
	}
}

/*
 * After btrfs_copy_from_user(), update the following things for delalloc:
 * - Mark newly dirtied pages as DELALLOC in the io tree.
 *   Used to advise which range is to be written back.
 * - Mark modified pages as Uptodate/Dirty and not needing COW fixup
 * - Update inode size for past EOF write
 */
int btrfs_dirty_pages(struct btrfs_inode *inode, struct page **pages,
		      size_t num_pages, loff_t pos, size_t write_bytes,
		      struct extent_state **cached, bool noreserve)
{
	struct btrfs_fs_info *fs_info = inode->root->fs_info;
	int err = 0;
	int i;
	u64 num_bytes;
	u64 start_pos;
	u64 end_of_last_block;
	u64 end_pos = pos + write_bytes;
	loff_t isize = i_size_read(&inode->vfs_inode);
	unsigned int extra_bits = 0;

	if (write_bytes == 0)
		return 0;

	if (noreserve)
		extra_bits |= EXTENT_NORESERVE;

	start_pos = round_down(pos, fs_info->sectorsize);
	num_bytes = round_up(write_bytes + pos - start_pos,
			     fs_info->sectorsize);

	end_of_last_block = start_pos + num_bytes - 1;

	/*
	 * The pages may have already been dirty, clear out old accounting so
	 * we can set things up properly
	 */
	clear_extent_bit(&inode->io_tree, start_pos, end_of_last_block,
			 EXTENT_DELALLOC | EXTENT_DO_ACCOUNTING | EXTENT_DEFRAG,
			 0, 0, cached);

	err = btrfs_set_extent_delalloc(inode, start_pos, end_of_last_block,
					extra_bits, cached);
	if (err)
		return err;

	for (i = 0; i < num_pages; i++) {
		struct page *p = pages[i];
		SetPageUptodate(p);
		ClearPageChecked(p);
		set_page_dirty(p);
	}

	/*
	 * we've only changed i_size in ram, and we haven't updated
	 * the disk i_size.  There is no need to log the inode
	 * at this time.
	 */
	if (end_pos > isize)
		i_size_write(&inode->vfs_inode, end_pos);
	return 0;
}

/*
 * this drops all the extents in the cache that intersect the range
 * [start, end].  Existing extents are split as required.
 */
void btrfs_drop_extent_cache(struct btrfs_inode *inode, u64 start, u64 end,
			     int skip_pinned)
{
	struct extent_map *em;
	struct extent_map *split = NULL;
	struct extent_map *split2 = NULL;
	struct extent_map_tree *em_tree = &inode->extent_tree;
	u64 len = end - start + 1;
	u64 gen;
	int ret;
	int testend = 1;
	unsigned long flags;
	int compressed = 0;
	bool modified;

	WARN_ON(end < start);
	if (end == (u64)-1) {
		len = (u64)-1;
		testend = 0;
	}
	while (1) {
		int no_splits = 0;

		modified = false;
		if (!split)
			split = alloc_extent_map();
		if (!split2)
			split2 = alloc_extent_map();
		if (!split || !split2)
			no_splits = 1;

		write_lock(&em_tree->lock);
		em = lookup_extent_mapping(em_tree, start, len);
		if (!em) {
			write_unlock(&em_tree->lock);
			break;
		}
		flags = em->flags;
		gen = em->generation;
		if (skip_pinned && test_bit(EXTENT_FLAG_PINNED, &em->flags)) {
			if (testend && em->start + em->len >= start + len) {
				free_extent_map(em);
				write_unlock(&em_tree->lock);
				break;
			}
			start = em->start + em->len;
			if (testend)
				len = start + len - (em->start + em->len);
			free_extent_map(em);
			write_unlock(&em_tree->lock);
			continue;
		}
		compressed = test_bit(EXTENT_FLAG_COMPRESSED, &em->flags);
		clear_bit(EXTENT_FLAG_PINNED, &em->flags);
		clear_bit(EXTENT_FLAG_LOGGING, &flags);
		modified = !list_empty(&em->list);
		if (no_splits)
			goto next;

		if (em->start < start) {
			split->start = em->start;
			split->len = start - em->start;

			if (em->block_start < EXTENT_MAP_LAST_BYTE) {
				split->orig_start = em->orig_start;
				split->block_start = em->block_start;

				if (compressed)
					split->block_len = em->block_len;
				else
					split->block_len = split->len;
				split->orig_block_len = max(split->block_len,
						em->orig_block_len);
				split->ram_bytes = em->ram_bytes;
			} else {
				split->orig_start = split->start;
				split->block_len = 0;
				split->block_start = em->block_start;
				split->orig_block_len = 0;
				split->ram_bytes = split->len;
			}

			split->generation = gen;
			split->flags = flags;
			split->compress_type = em->compress_type;
			replace_extent_mapping(em_tree, em, split, modified);
			free_extent_map(split);
			split = split2;
			split2 = NULL;
		}
		if (testend && em->start + em->len > start + len) {
			u64 diff = start + len - em->start;

			split->start = start + len;
			split->len = em->start + em->len - (start + len);
			split->flags = flags;
			split->compress_type = em->compress_type;
			split->generation = gen;

			if (em->block_start < EXTENT_MAP_LAST_BYTE) {
				split->orig_block_len = max(em->block_len,
						    em->orig_block_len);

				split->ram_bytes = em->ram_bytes;
				if (compressed) {
					split->block_len = em->block_len;
					split->block_start = em->block_start;
					split->orig_start = em->orig_start;
				} else {
					split->block_len = split->len;
					split->block_start = em->block_start
						+ diff;
					split->orig_start = em->orig_start;
				}
			} else {
				split->ram_bytes = split->len;
				split->orig_start = split->start;
				split->block_len = 0;
				split->block_start = em->block_start;
				split->orig_block_len = 0;
			}

			if (extent_map_in_tree(em)) {
				replace_extent_mapping(em_tree, em, split,
						       modified);
			} else {
				ret = add_extent_mapping(em_tree, split,
							 modified);
				ASSERT(ret == 0); /* Logic error */
			}
			free_extent_map(split);
			split = NULL;
		}
next:
		if (extent_map_in_tree(em))
			remove_extent_mapping(em_tree, em);
		write_unlock(&em_tree->lock);

		/* once for us */
		free_extent_map(em);
		/* once for the tree*/
		free_extent_map(em);
	}
	if (split)
		free_extent_map(split);
	if (split2)
		free_extent_map(split2);
}

/*
 * this is very complex, but the basic idea is to drop all extents
 * in the range start - end.  hint_block is filled in with a block number
 * that would be a good hint to the block allocator for this file.
 *
 * If an extent intersects the range but is not entirely inside the range
 * it is either truncated or split.  Anything entirely inside the range
 * is deleted from the tree.
 *
 * Note: the VFS' inode number of bytes is not updated, it's up to the caller
 * to deal with that. We set the field 'bytes_found' of the arguments structure
 * with the number of allocated bytes found in the target range, so that the
 * caller can update the inode's number of bytes in an atomic way when
 * replacing extents in a range to avoid races with stat(2).
 */
int btrfs_drop_extents(struct btrfs_trans_handle *trans,
		       struct btrfs_root *root, struct btrfs_inode *inode,
		       struct btrfs_drop_extents_args *args)
{
	struct btrfs_fs_info *fs_info = root->fs_info;
	struct extent_buffer *leaf;
	struct btrfs_file_extent_item *fi;
	struct btrfs_ref ref = { 0 };
	struct btrfs_key key;
	struct btrfs_key new_key;
	u64 ino = btrfs_ino(inode);
	u64 search_start = args->start;
	u64 disk_bytenr = 0;
	u64 num_bytes = 0;
	u64 extent_offset = 0;
	u64 extent_end = 0;
	u64 last_end = args->start;
	int del_nr = 0;
	int del_slot = 0;
	int extent_type;
	int recow;
	int ret;
	int modify_tree = -1;
	int update_refs;
	int found = 0;
	int leafs_visited = 0;
	struct btrfs_path *path = args->path;

	args->bytes_found = 0;
	args->extent_inserted = false;

	/* Must always have a path if ->replace_extent is true */
	ASSERT(!(args->replace_extent && !args->path));

	if (!path) {
		path = btrfs_alloc_path();
		if (!path) {
			ret = -ENOMEM;
			goto out;
		}
	}

	if (args->drop_cache)
		btrfs_drop_extent_cache(inode, args->start, args->end - 1, 0);

	if (args->start >= inode->disk_i_size && !args->replace_extent)
		modify_tree = 0;

	update_refs = (test_bit(BTRFS_ROOT_SHAREABLE, &root->state) ||
		       root == fs_info->tree_root);
	while (1) {
		recow = 0;
		ret = btrfs_lookup_file_extent(trans, root, path, ino,
					       search_start, modify_tree);
		if (ret < 0)
			break;
		if (ret > 0 && path->slots[0] > 0 && search_start == args->start) {
			leaf = path->nodes[0];
			btrfs_item_key_to_cpu(leaf, &key, path->slots[0] - 1);
			if (key.objectid == ino &&
			    key.type == BTRFS_EXTENT_DATA_KEY)
				path->slots[0]--;
		}
		ret = 0;
		leafs_visited++;
next_slot:
		leaf = path->nodes[0];
		if (path->slots[0] >= btrfs_header_nritems(leaf)) {
			BUG_ON(del_nr > 0);
			ret = btrfs_next_leaf(root, path);
			if (ret < 0)
				break;
			if (ret > 0) {
				ret = 0;
				break;
			}
			leafs_visited++;
			leaf = path->nodes[0];
			recow = 1;
		}

		btrfs_item_key_to_cpu(leaf, &key, path->slots[0]);

		if (key.objectid > ino)
			break;
		if (WARN_ON_ONCE(key.objectid < ino) ||
		    key.type < BTRFS_EXTENT_DATA_KEY) {
			ASSERT(del_nr == 0);
			path->slots[0]++;
			goto next_slot;
		}
		if (key.type > BTRFS_EXTENT_DATA_KEY || key.offset >= args->end)
			break;

		fi = btrfs_item_ptr(leaf, path->slots[0],
				    struct btrfs_file_extent_item);
		extent_type = btrfs_file_extent_type(leaf, fi);

		if (extent_type == BTRFS_FILE_EXTENT_REG ||
		    extent_type == BTRFS_FILE_EXTENT_PREALLOC) {
			disk_bytenr = btrfs_file_extent_disk_bytenr(leaf, fi);
			num_bytes = btrfs_file_extent_disk_num_bytes(leaf, fi);
			extent_offset = btrfs_file_extent_offset(leaf, fi);
			extent_end = key.offset +
				btrfs_file_extent_num_bytes(leaf, fi);
		} else if (extent_type == BTRFS_FILE_EXTENT_INLINE) {
			extent_end = key.offset +
				btrfs_file_extent_ram_bytes(leaf, fi);
		} else {
			/* can't happen */
			BUG();
		}

		/*
		 * Don't skip extent items representing 0 byte lengths. They
		 * used to be created (bug) if while punching holes we hit
		 * -ENOSPC condition. So if we find one here, just ensure we
		 * delete it, otherwise we would insert a new file extent item
		 * with the same key (offset) as that 0 bytes length file
		 * extent item in the call to setup_items_for_insert() later
		 * in this function.
		 */
		if (extent_end == key.offset && extent_end >= search_start) {
			last_end = extent_end;
			goto delete_extent_item;
		}

		if (extent_end <= search_start) {
			path->slots[0]++;
			goto next_slot;
		}

		found = 1;
		search_start = max(key.offset, args->start);
		if (recow || !modify_tree) {
			modify_tree = -1;
			btrfs_release_path(path);
			continue;
		}

		/*
		 *     | - range to drop - |
		 *  | -------- extent -------- |
		 */
		if (args->start > key.offset && args->end < extent_end) {
			BUG_ON(del_nr > 0);
			if (extent_type == BTRFS_FILE_EXTENT_INLINE) {
				ret = -EOPNOTSUPP;
				break;
			}

			memcpy(&new_key, &key, sizeof(new_key));
			new_key.offset = args->start;
			ret = btrfs_duplicate_item(trans, root, path,
						   &new_key);
			if (ret == -EAGAIN) {
				btrfs_release_path(path);
				continue;
			}
			if (ret < 0)
				break;

			leaf = path->nodes[0];
			fi = btrfs_item_ptr(leaf, path->slots[0] - 1,
					    struct btrfs_file_extent_item);
			btrfs_set_file_extent_num_bytes(leaf, fi,
							args->start - key.offset);

			fi = btrfs_item_ptr(leaf, path->slots[0],
					    struct btrfs_file_extent_item);

			extent_offset += args->start - key.offset;
			btrfs_set_file_extent_offset(leaf, fi, extent_offset);
			btrfs_set_file_extent_num_bytes(leaf, fi,
							extent_end - args->start);
			btrfs_mark_buffer_dirty(leaf);

			if (update_refs && disk_bytenr > 0) {
				btrfs_init_generic_ref(&ref,
						BTRFS_ADD_DELAYED_REF,
						disk_bytenr, num_bytes, 0);
				btrfs_init_data_ref(&ref,
						root->root_key.objectid,
						new_key.objectid,
						args->start - extent_offset);
				ret = btrfs_inc_extent_ref(trans, &ref);
				BUG_ON(ret); /* -ENOMEM */
			}
			key.offset = args->start;
		}
		/*
		 * From here on out we will have actually dropped something, so
		 * last_end can be updated.
		 */
		last_end = extent_end;

		/*
		 *  | ---- range to drop ----- |
		 *      | -------- extent -------- |
		 */
		if (args->start <= key.offset && args->end < extent_end) {
			if (extent_type == BTRFS_FILE_EXTENT_INLINE) {
				ret = -EOPNOTSUPP;
				break;
			}

			memcpy(&new_key, &key, sizeof(new_key));
			new_key.offset = args->end;
			btrfs_set_item_key_safe(fs_info, path, &new_key);

			extent_offset += args->end - key.offset;
			btrfs_set_file_extent_offset(leaf, fi, extent_offset);
			btrfs_set_file_extent_num_bytes(leaf, fi,
							extent_end - args->end);
			btrfs_mark_buffer_dirty(leaf);
			if (update_refs && disk_bytenr > 0)
				args->bytes_found += args->end - key.offset;
			break;
		}

		search_start = extent_end;
		/*
		 *       | ---- range to drop ----- |
		 *  | -------- extent -------- |
		 */
		if (args->start > key.offset && args->end >= extent_end) {
			BUG_ON(del_nr > 0);
			if (extent_type == BTRFS_FILE_EXTENT_INLINE) {
				ret = -EOPNOTSUPP;
				break;
			}

			btrfs_set_file_extent_num_bytes(leaf, fi,
							args->start - key.offset);
			btrfs_mark_buffer_dirty(leaf);
			if (update_refs && disk_bytenr > 0)
				args->bytes_found += extent_end - args->start;
			if (args->end == extent_end)
				break;

			path->slots[0]++;
			goto next_slot;
		}

		/*
		 *  | ---- range to drop ----- |
		 *    | ------ extent ------ |
		 */
		if (args->start <= key.offset && args->end >= extent_end) {
delete_extent_item:
			if (del_nr == 0) {
				del_slot = path->slots[0];
				del_nr = 1;
			} else {
				BUG_ON(del_slot + del_nr != path->slots[0]);
				del_nr++;
			}

			if (update_refs &&
			    extent_type == BTRFS_FILE_EXTENT_INLINE) {
				args->bytes_found += extent_end - key.offset;
				extent_end = ALIGN(extent_end,
						   fs_info->sectorsize);
			} else if (update_refs && disk_bytenr > 0) {
				btrfs_init_generic_ref(&ref,
						BTRFS_DROP_DELAYED_REF,
						disk_bytenr, num_bytes, 0);
				btrfs_init_data_ref(&ref,
						root->root_key.objectid,
						key.objectid,
						key.offset - extent_offset);
				ret = btrfs_free_extent(trans, &ref);
				BUG_ON(ret); /* -ENOMEM */
				args->bytes_found += extent_end - key.offset;
			}

			if (args->end == extent_end)
				break;

			if (path->slots[0] + 1 < btrfs_header_nritems(leaf)) {
				path->slots[0]++;
				goto next_slot;
			}

			ret = btrfs_del_items(trans, root, path, del_slot,
					      del_nr);
			if (ret) {
				btrfs_abort_transaction(trans, ret);
				break;
			}

			del_nr = 0;
			del_slot = 0;

			btrfs_release_path(path);
			continue;
		}

		BUG();
	}

	if (!ret && del_nr > 0) {
		/*
		 * Set path->slots[0] to first slot, so that after the delete
		 * if items are move off from our leaf to its immediate left or
		 * right neighbor leafs, we end up with a correct and adjusted
		 * path->slots[0] for our insertion (if args->replace_extent).
		 */
		path->slots[0] = del_slot;
		ret = btrfs_del_items(trans, root, path, del_slot, del_nr);
		if (ret)
			btrfs_abort_transaction(trans, ret);
	}

	leaf = path->nodes[0];
	/*
	 * If btrfs_del_items() was called, it might have deleted a leaf, in
	 * which case it unlocked our path, so check path->locks[0] matches a
	 * write lock.
	 */
	if (!ret && args->replace_extent && leafs_visited == 1 &&
	    path->locks[0] == BTRFS_WRITE_LOCK &&
	    btrfs_leaf_free_space(leaf) >=
	    sizeof(struct btrfs_item) + args->extent_item_size) {

		key.objectid = ino;
		key.type = BTRFS_EXTENT_DATA_KEY;
		key.offset = args->start;
		if (!del_nr && path->slots[0] < btrfs_header_nritems(leaf)) {
			struct btrfs_key slot_key;

			btrfs_item_key_to_cpu(leaf, &slot_key, path->slots[0]);
			if (btrfs_comp_cpu_keys(&key, &slot_key) > 0)
				path->slots[0]++;
		}
		setup_items_for_insert(root, path, &key,
				       &args->extent_item_size, 1);
		args->extent_inserted = true;
	}

	if (!args->path)
		btrfs_free_path(path);
	else if (!args->extent_inserted)
		btrfs_release_path(path);
out:
	args->drop_end = found ? min(args->end, last_end) : args->end;

	return ret;
}

static int extent_mergeable(struct extent_buffer *leaf, int slot,
			    u64 objectid, u64 bytenr, u64 orig_offset,
			    u64 *start, u64 *end)
{
	struct btrfs_file_extent_item *fi;
	struct btrfs_key key;
	u64 extent_end;

	if (slot < 0 || slot >= btrfs_header_nritems(leaf))
		return 0;

	btrfs_item_key_to_cpu(leaf, &key, slot);
	if (key.objectid != objectid || key.type != BTRFS_EXTENT_DATA_KEY)
		return 0;

	fi = btrfs_item_ptr(leaf, slot, struct btrfs_file_extent_item);
	if (btrfs_file_extent_type(leaf, fi) != BTRFS_FILE_EXTENT_REG ||
	    btrfs_file_extent_disk_bytenr(leaf, fi) != bytenr ||
	    btrfs_file_extent_offset(leaf, fi) != key.offset - orig_offset ||
	    btrfs_file_extent_compression(leaf, fi) ||
	    btrfs_file_extent_encryption(leaf, fi) ||
	    btrfs_file_extent_other_encoding(leaf, fi))
		return 0;

	extent_end = key.offset + btrfs_file_extent_num_bytes(leaf, fi);
	if ((*start && *start != key.offset) || (*end && *end != extent_end))
		return 0;

	*start = key.offset;
	*end = extent_end;
	return 1;
}

/*
 * Mark extent in the range start - end as written.
 *
 * This changes extent type from 'pre-allocated' to 'regular'. If only
 * part of extent is marked as written, the extent will be split into
 * two or three.
 */
int btrfs_mark_extent_written(struct btrfs_trans_handle *trans,
			      struct btrfs_inode *inode, u64 start, u64 end)
{
	struct btrfs_fs_info *fs_info = trans->fs_info;
	struct btrfs_root *root = inode->root;
	struct extent_buffer *leaf;
	struct btrfs_path *path;
	struct btrfs_file_extent_item *fi;
	struct btrfs_ref ref = { 0 };
	struct btrfs_key key;
	struct btrfs_key new_key;
	u64 bytenr;
	u64 num_bytes;
	u64 extent_end;
	u64 orig_offset;
	u64 other_start;
	u64 other_end;
	u64 split;
	int del_nr = 0;
	int del_slot = 0;
	int recow;
	int ret = 0;
	u64 ino = btrfs_ino(inode);

	path = btrfs_alloc_path();
	if (!path)
		return -ENOMEM;
again:
	recow = 0;
	split = start;
	key.objectid = ino;
	key.type = BTRFS_EXTENT_DATA_KEY;
	key.offset = split;

	ret = btrfs_search_slot(trans, root, &key, path, -1, 1);
	if (ret < 0)
		goto out;
	if (ret > 0 && path->slots[0] > 0)
		path->slots[0]--;

	leaf = path->nodes[0];
	btrfs_item_key_to_cpu(leaf, &key, path->slots[0]);
	if (key.objectid != ino ||
	    key.type != BTRFS_EXTENT_DATA_KEY) {
		ret = -EINVAL;
		btrfs_abort_transaction(trans, ret);
		goto out;
	}
	fi = btrfs_item_ptr(leaf, path->slots[0],
			    struct btrfs_file_extent_item);
	if (btrfs_file_extent_type(leaf, fi) != BTRFS_FILE_EXTENT_PREALLOC) {
		ret = -EINVAL;
		btrfs_abort_transaction(trans, ret);
		goto out;
	}
	extent_end = key.offset + btrfs_file_extent_num_bytes(leaf, fi);
	if (key.offset > start || extent_end < end) {
		ret = -EINVAL;
		btrfs_abort_transaction(trans, ret);
		goto out;
	}

	bytenr = btrfs_file_extent_disk_bytenr(leaf, fi);
	num_bytes = btrfs_file_extent_disk_num_bytes(leaf, fi);
	orig_offset = key.offset - btrfs_file_extent_offset(leaf, fi);
	memcpy(&new_key, &key, sizeof(new_key));

	if (start == key.offset && end < extent_end) {
		other_start = 0;
		other_end = start;
		if (extent_mergeable(leaf, path->slots[0] - 1,
				     ino, bytenr, orig_offset,
				     &other_start, &other_end)) {
			new_key.offset = end;
			btrfs_set_item_key_safe(fs_info, path, &new_key);
			fi = btrfs_item_ptr(leaf, path->slots[0],
					    struct btrfs_file_extent_item);
			btrfs_set_file_extent_generation(leaf, fi,
							 trans->transid);
			btrfs_set_file_extent_num_bytes(leaf, fi,
							extent_end - end);
			btrfs_set_file_extent_offset(leaf, fi,
						     end - orig_offset);
			fi = btrfs_item_ptr(leaf, path->slots[0] - 1,
					    struct btrfs_file_extent_item);
			btrfs_set_file_extent_generation(leaf, fi,
							 trans->transid);
			btrfs_set_file_extent_num_bytes(leaf, fi,
							end - other_start);
			btrfs_mark_buffer_dirty(leaf);
			goto out;
		}
	}

	if (start > key.offset && end == extent_end) {
		other_start = end;
		other_end = 0;
		if (extent_mergeable(leaf, path->slots[0] + 1,
				     ino, bytenr, orig_offset,
				     &other_start, &other_end)) {
			fi = btrfs_item_ptr(leaf, path->slots[0],
					    struct btrfs_file_extent_item);
			btrfs_set_file_extent_num_bytes(leaf, fi,
							start - key.offset);
			btrfs_set_file_extent_generation(leaf, fi,
							 trans->transid);
			path->slots[0]++;
			new_key.offset = start;
			btrfs_set_item_key_safe(fs_info, path, &new_key);

			fi = btrfs_item_ptr(leaf, path->slots[0],
					    struct btrfs_file_extent_item);
			btrfs_set_file_extent_generation(leaf, fi,
							 trans->transid);
			btrfs_set_file_extent_num_bytes(leaf, fi,
							other_end - start);
			btrfs_set_file_extent_offset(leaf, fi,
						     start - orig_offset);
			btrfs_mark_buffer_dirty(leaf);
			goto out;
		}
	}

	while (start > key.offset || end < extent_end) {
		if (key.offset == start)
			split = end;

		new_key.offset = split;
		ret = btrfs_duplicate_item(trans, root, path, &new_key);
		if (ret == -EAGAIN) {
			btrfs_release_path(path);
			goto again;
		}
		if (ret < 0) {
			btrfs_abort_transaction(trans, ret);
			goto out;
		}

		leaf = path->nodes[0];
		fi = btrfs_item_ptr(leaf, path->slots[0] - 1,
				    struct btrfs_file_extent_item);
		btrfs_set_file_extent_generation(leaf, fi, trans->transid);
		btrfs_set_file_extent_num_bytes(leaf, fi,
						split - key.offset);

		fi = btrfs_item_ptr(leaf, path->slots[0],
				    struct btrfs_file_extent_item);

		btrfs_set_file_extent_generation(leaf, fi, trans->transid);
		btrfs_set_file_extent_offset(leaf, fi, split - orig_offset);
		btrfs_set_file_extent_num_bytes(leaf, fi,
						extent_end - split);
		btrfs_mark_buffer_dirty(leaf);

		btrfs_init_generic_ref(&ref, BTRFS_ADD_DELAYED_REF, bytenr,
				       num_bytes, 0);
		btrfs_init_data_ref(&ref, root->root_key.objectid, ino,
				    orig_offset);
		ret = btrfs_inc_extent_ref(trans, &ref);
		if (ret) {
			btrfs_abort_transaction(trans, ret);
			goto out;
		}

		if (split == start) {
			key.offset = start;
		} else {
			if (start != key.offset) {
				ret = -EINVAL;
				btrfs_abort_transaction(trans, ret);
				goto out;
			}
			path->slots[0]--;
			extent_end = end;
		}
		recow = 1;
	}

	other_start = end;
	other_end = 0;
	btrfs_init_generic_ref(&ref, BTRFS_DROP_DELAYED_REF, bytenr,
			       num_bytes, 0);
	btrfs_init_data_ref(&ref, root->root_key.objectid, ino, orig_offset);
	if (extent_mergeable(leaf, path->slots[0] + 1,
			     ino, bytenr, orig_offset,
			     &other_start, &other_end)) {
		if (recow) {
			btrfs_release_path(path);
			goto again;
		}
		extent_end = other_end;
		del_slot = path->slots[0] + 1;
		del_nr++;
		ret = btrfs_free_extent(trans, &ref);
		if (ret) {
			btrfs_abort_transaction(trans, ret);
			goto out;
		}
	}
	other_start = 0;
	other_end = start;
	if (extent_mergeable(leaf, path->slots[0] - 1,
			     ino, bytenr, orig_offset,
			     &other_start, &other_end)) {
		if (recow) {
			btrfs_release_path(path);
			goto again;
		}
		key.offset = other_start;
		del_slot = path->slots[0];
		del_nr++;
		ret = btrfs_free_extent(trans, &ref);
		if (ret) {
			btrfs_abort_transaction(trans, ret);
			goto out;
		}
	}
	if (del_nr == 0) {
		fi = btrfs_item_ptr(leaf, path->slots[0],
			   struct btrfs_file_extent_item);
		btrfs_set_file_extent_type(leaf, fi,
					   BTRFS_FILE_EXTENT_REG);
		btrfs_set_file_extent_generation(leaf, fi, trans->transid);
		btrfs_mark_buffer_dirty(leaf);
	} else {
		fi = btrfs_item_ptr(leaf, del_slot - 1,
			   struct btrfs_file_extent_item);
		btrfs_set_file_extent_type(leaf, fi,
					   BTRFS_FILE_EXTENT_REG);
		btrfs_set_file_extent_generation(leaf, fi, trans->transid);
		btrfs_set_file_extent_num_bytes(leaf, fi,
						extent_end - key.offset);
		btrfs_mark_buffer_dirty(leaf);

		ret = btrfs_del_items(trans, root, path, del_slot, del_nr);
		if (ret < 0) {
			btrfs_abort_transaction(trans, ret);
			goto out;
		}
	}
out:
	btrfs_free_path(path);
	return ret;
}

/*
 * on error we return an unlocked page and the error value
 * on success we return a locked page and 0
 */
static int prepare_uptodate_page(struct inode *inode,
				 struct page *page, u64 pos,
				 bool force_uptodate)
{
	int ret = 0;

	if (((pos & (PAGE_SIZE - 1)) || force_uptodate) &&
	    !PageUptodate(page)) {
		ret = btrfs_readpage(NULL, page);
		if (ret)
			return ret;
		lock_page(page);
		if (!PageUptodate(page)) {
			unlock_page(page);
			return -EIO;
		}
		if (page->mapping != inode->i_mapping) {
			unlock_page(page);
			return -EAGAIN;
		}
	}
	return 0;
}

/*
 * this just gets pages into the page cache and locks them down.
 */
static noinline int prepare_pages(struct inode *inode, struct page **pages,
				  size_t num_pages, loff_t pos,
				  size_t write_bytes, bool force_uptodate)
{
	int i;
	unsigned long index = pos >> PAGE_SHIFT;
	gfp_t mask = btrfs_alloc_write_mask(inode->i_mapping);
	int err = 0;
	int faili;

	for (i = 0; i < num_pages; i++) {
again:
		pages[i] = find_or_create_page(inode->i_mapping, index + i,
					       mask | __GFP_WRITE);
		if (!pages[i]) {
			faili = i - 1;
			err = -ENOMEM;
			goto fail;
		}

		err = set_page_extent_mapped(pages[i]);
		if (err < 0) {
			faili = i;
			goto fail;
		}

		if (i == 0)
			err = prepare_uptodate_page(inode, pages[i], pos,
						    force_uptodate);
		if (!err && i == num_pages - 1)
			err = prepare_uptodate_page(inode, pages[i],
						    pos + write_bytes, false);
		if (err) {
			put_page(pages[i]);
			if (err == -EAGAIN) {
				err = 0;
				goto again;
			}
			faili = i - 1;
			goto fail;
		}
		wait_on_page_writeback(pages[i]);
	}

	return 0;
fail:
	while (faili >= 0) {
		unlock_page(pages[faili]);
		put_page(pages[faili]);
		faili--;
	}
	return err;

}

/*
 * This function locks the extent and properly waits for data=ordered extents
 * to finish before allowing the pages to be modified if need.
 *
 * The return value:
 * 1 - the extent is locked
 * 0 - the extent is not locked, and everything is OK
 * -EAGAIN - need re-prepare the pages
 * the other < 0 number - Something wrong happens
 */
static noinline int
lock_and_cleanup_extent_if_need(struct btrfs_inode *inode, struct page **pages,
				size_t num_pages, loff_t pos,
				size_t write_bytes,
				u64 *lockstart, u64 *lockend,
				struct extent_state **cached_state)
{
	struct btrfs_fs_info *fs_info = inode->root->fs_info;
	u64 start_pos;
	u64 last_pos;
	int i;
	int ret = 0;

	start_pos = round_down(pos, fs_info->sectorsize);
	last_pos = round_up(pos + write_bytes, fs_info->sectorsize) - 1;

	if (start_pos < inode->vfs_inode.i_size) {
		struct btrfs_ordered_extent *ordered;

		lock_extent_bits(&inode->io_tree, start_pos, last_pos,
				cached_state);
		ordered = btrfs_lookup_ordered_range(inode, start_pos,
						     last_pos - start_pos + 1);
		if (ordered &&
		    ordered->file_offset + ordered->num_bytes > start_pos &&
		    ordered->file_offset <= last_pos) {
			unlock_extent_cached(&inode->io_tree, start_pos,
					last_pos, cached_state);
			for (i = 0; i < num_pages; i++) {
				unlock_page(pages[i]);
				put_page(pages[i]);
			}
			btrfs_start_ordered_extent(ordered, 1);
			btrfs_put_ordered_extent(ordered);
			return -EAGAIN;
		}
		if (ordered)
			btrfs_put_ordered_extent(ordered);

		*lockstart = start_pos;
		*lockend = last_pos;
		ret = 1;
	}

	/*
	 * We should be called after prepare_pages() which should have locked
	 * all pages in the range.
	 */
	for (i = 0; i < num_pages; i++)
		WARN_ON(!PageLocked(pages[i]));

	return ret;
}

static int check_can_nocow(struct btrfs_inode *inode, loff_t pos,
			   size_t *write_bytes, bool nowait)
{
	struct btrfs_fs_info *fs_info = inode->root->fs_info;
	struct btrfs_root *root = inode->root;
	u64 lockstart, lockend;
	u64 num_bytes;
	int ret;

	if (!(inode->flags & (BTRFS_INODE_NODATACOW | BTRFS_INODE_PREALLOC)))
		return 0;

	if (!nowait && !btrfs_drew_try_write_lock(&root->snapshot_lock))
		return -EAGAIN;

	lockstart = round_down(pos, fs_info->sectorsize);
	lockend = round_up(pos + *write_bytes,
			   fs_info->sectorsize) - 1;
	num_bytes = lockend - lockstart + 1;

	if (nowait) {
		struct btrfs_ordered_extent *ordered;

		if (!try_lock_extent(&inode->io_tree, lockstart, lockend))
			return -EAGAIN;

		ordered = btrfs_lookup_ordered_range(inode, lockstart,
						     num_bytes);
		if (ordered) {
			btrfs_put_ordered_extent(ordered);
			ret = -EAGAIN;
			goto out_unlock;
		}
	} else {
		btrfs_lock_and_flush_ordered_range(inode, lockstart,
						   lockend, NULL);
	}

	ret = can_nocow_extent(&inode->vfs_inode, lockstart, &num_bytes,
			NULL, NULL, NULL, false);
	if (ret <= 0) {
		ret = 0;
		if (!nowait)
			btrfs_drew_write_unlock(&root->snapshot_lock);
	} else {
		*write_bytes = min_t(size_t, *write_bytes ,
				     num_bytes - pos + lockstart);
	}
out_unlock:
	unlock_extent(&inode->io_tree, lockstart, lockend);

	return ret;
}

static int check_nocow_nolock(struct btrfs_inode *inode, loff_t pos,
			      size_t *write_bytes)
{
	return check_can_nocow(inode, pos, write_bytes, true);
}

/*
 * Check if we can do nocow write into the range [@pos, @pos + @write_bytes)
 *
 * @pos:	 File offset
 * @write_bytes: The length to write, will be updated to the nocow writeable
 *		 range
 *
 * This function will flush ordered extents in the range to ensure proper
 * nocow checks.
 *
 * Return:
 * >0		and update @write_bytes if we can do nocow write
 *  0		if we can't do nocow write
 * -EAGAIN	if we can't get the needed lock or there are ordered extents
 * 		for * (nowait == true) case
 * <0		if other error happened
 *
 * NOTE: Callers need to release the lock by btrfs_check_nocow_unlock().
 */
int btrfs_check_nocow_lock(struct btrfs_inode *inode, loff_t pos,
			   size_t *write_bytes)
{
	return check_can_nocow(inode, pos, write_bytes, false);
}

void btrfs_check_nocow_unlock(struct btrfs_inode *inode)
{
	btrfs_drew_write_unlock(&inode->root->snapshot_lock);
}

static void update_time_for_write(struct inode *inode)
{
	struct timespec64 now;

	if (IS_NOCMTIME(inode))
		return;

	now = current_time(inode);
	if (!timespec64_equal(&inode->i_mtime, &now))
		inode->i_mtime = now;

	if (!timespec64_equal(&inode->i_ctime, &now))
		inode->i_ctime = now;

	if (IS_I_VERSION(inode))
		inode_inc_iversion(inode);
}

static int btrfs_write_check(struct kiocb *iocb, struct iov_iter *from,
			     size_t count)
{
	struct file *file = iocb->ki_filp;
	struct inode *inode = file_inode(file);
	struct btrfs_fs_info *fs_info = btrfs_sb(inode->i_sb);
	loff_t pos = iocb->ki_pos;
	int ret;
	loff_t oldsize;
	loff_t start_pos;

	if (iocb->ki_flags & IOCB_NOWAIT) {
		size_t nocow_bytes = count;

		/* We will allocate space in case nodatacow is not set, so bail */
		if (check_nocow_nolock(BTRFS_I(inode), pos, &nocow_bytes) <= 0)
			return -EAGAIN;
		/*
		 * There are holes in the range or parts of the range that must
		 * be COWed (shared extents, RO block groups, etc), so just bail
		 * out.
		 */
		if (nocow_bytes < count)
			return -EAGAIN;
	}

	current->backing_dev_info = inode_to_bdi(inode);
	ret = file_remove_privs(file);
	if (ret)
		return ret;

	/*
	 * We reserve space for updating the inode when we reserve space for the
	 * extent we are going to write, so we will enospc out there.  We don't
	 * need to start yet another transaction to update the inode as we will
	 * update the inode when we finish writing whatever data we write.
	 */
	update_time_for_write(inode);

	start_pos = round_down(pos, fs_info->sectorsize);
	oldsize = i_size_read(inode);
	if (start_pos > oldsize) {
		/* Expand hole size to cover write data, preventing empty gap */
		loff_t end_pos = round_up(pos + count, fs_info->sectorsize);

		ret = btrfs_cont_expand(BTRFS_I(inode), oldsize, end_pos);
		if (ret) {
			current->backing_dev_info = NULL;
			return ret;
		}
	}

	return 0;
}

static noinline ssize_t btrfs_buffered_write(struct kiocb *iocb,
					       struct iov_iter *i)
{
	struct file *file = iocb->ki_filp;
	loff_t pos;
	struct inode *inode = file_inode(file);
	struct btrfs_fs_info *fs_info = btrfs_sb(inode->i_sb);
	struct page **pages = NULL;
	struct extent_changeset *data_reserved = NULL;
	u64 release_bytes = 0;
	u64 lockstart;
	u64 lockend;
	size_t num_written = 0;
	int nrptrs;
	ssize_t ret;
	bool only_release_metadata = false;
	bool force_page_uptodate = false;
	loff_t old_isize = i_size_read(inode);
	unsigned int ilock_flags = 0;

	if (iocb->ki_flags & IOCB_NOWAIT)
		ilock_flags |= BTRFS_ILOCK_TRY;

	ret = btrfs_inode_lock(inode, ilock_flags);
	if (ret < 0)
		return ret;

	ret = generic_write_checks(iocb, i);
	if (ret <= 0)
		goto out;

	ret = btrfs_write_check(iocb, i, ret);
	if (ret < 0)
		goto out;

	pos = iocb->ki_pos;
	nrptrs = min(DIV_ROUND_UP(iov_iter_count(i), PAGE_SIZE),
			PAGE_SIZE / (sizeof(struct page *)));
	nrptrs = min(nrptrs, current->nr_dirtied_pause - current->nr_dirtied);
	nrptrs = max(nrptrs, 8);
	pages = kmalloc_array(nrptrs, sizeof(struct page *), GFP_KERNEL);
	if (!pages) {
		ret = -ENOMEM;
		goto out;
	}

	while (iov_iter_count(i) > 0) {
		struct extent_state *cached_state = NULL;
		size_t offset = offset_in_page(pos);
		size_t sector_offset;
		size_t write_bytes = min(iov_iter_count(i),
					 nrptrs * (size_t)PAGE_SIZE -
					 offset);
		size_t num_pages;
		size_t reserve_bytes;
		size_t dirty_pages;
		size_t copied;
		size_t dirty_sectors;
		size_t num_sectors;
		int extents_locked;

		/*
		 * Fault pages before locking them in prepare_pages
		 * to avoid recursive lock
		 */
		if (unlikely(iov_iter_fault_in_readable(i, write_bytes))) {
			ret = -EFAULT;
			break;
		}

		only_release_metadata = false;
		sector_offset = pos & (fs_info->sectorsize - 1);

		extent_changeset_release(data_reserved);
		ret = btrfs_check_data_free_space(BTRFS_I(inode),
						  &data_reserved, pos,
						  write_bytes);
		if (ret < 0) {
			/*
			 * If we don't have to COW at the offset, reserve
			 * metadata only. write_bytes may get smaller than
			 * requested here.
			 */
			if (btrfs_check_nocow_lock(BTRFS_I(inode), pos,
						   &write_bytes) > 0)
				only_release_metadata = true;
			else
				break;
		}

		num_pages = DIV_ROUND_UP(write_bytes + offset, PAGE_SIZE);
		WARN_ON(num_pages > nrptrs);
		reserve_bytes = round_up(write_bytes + sector_offset,
					 fs_info->sectorsize);
		WARN_ON(reserve_bytes == 0);
		ret = btrfs_delalloc_reserve_metadata(BTRFS_I(inode),
				reserve_bytes);
		if (ret) {
			if (!only_release_metadata)
				btrfs_free_reserved_data_space(BTRFS_I(inode),
						data_reserved, pos,
						write_bytes);
			else
				btrfs_check_nocow_unlock(BTRFS_I(inode));
			break;
		}

		release_bytes = reserve_bytes;
again:
		/*
		 * This is going to setup the pages array with the number of
		 * pages we want, so we don't really need to worry about the
		 * contents of pages from loop to loop
		 */
		ret = prepare_pages(inode, pages, num_pages,
				    pos, write_bytes,
				    force_page_uptodate);
		if (ret) {
			btrfs_delalloc_release_extents(BTRFS_I(inode),
						       reserve_bytes);
			break;
		}

		extents_locked = lock_and_cleanup_extent_if_need(
				BTRFS_I(inode), pages,
				num_pages, pos, write_bytes, &lockstart,
				&lockend, &cached_state);
		if (extents_locked < 0) {
			if (extents_locked == -EAGAIN)
				goto again;
			btrfs_delalloc_release_extents(BTRFS_I(inode),
						       reserve_bytes);
			ret = extents_locked;
			break;
		}

		copied = btrfs_copy_from_user(pos, write_bytes, pages, i);

		num_sectors = BTRFS_BYTES_TO_BLKS(fs_info, reserve_bytes);
		dirty_sectors = round_up(copied + sector_offset,
					fs_info->sectorsize);
		dirty_sectors = BTRFS_BYTES_TO_BLKS(fs_info, dirty_sectors);

		/*
		 * if we have trouble faulting in the pages, fall
		 * back to one page at a time
		 */
		if (copied < write_bytes)
			nrptrs = 1;

		if (copied == 0) {
			force_page_uptodate = true;
			dirty_sectors = 0;
			dirty_pages = 0;
		} else {
			force_page_uptodate = false;
			dirty_pages = DIV_ROUND_UP(copied + offset,
						   PAGE_SIZE);
		}

		if (num_sectors > dirty_sectors) {
			/* release everything except the sectors we dirtied */
			release_bytes -= dirty_sectors << fs_info->sectorsize_bits;
			if (only_release_metadata) {
				btrfs_delalloc_release_metadata(BTRFS_I(inode),
							release_bytes, true);
			} else {
				u64 __pos;

				__pos = round_down(pos,
						   fs_info->sectorsize) +
					(dirty_pages << PAGE_SHIFT);
				btrfs_delalloc_release_space(BTRFS_I(inode),
						data_reserved, __pos,
						release_bytes, true);
			}
		}

		release_bytes = round_up(copied + sector_offset,
					fs_info->sectorsize);

		ret = btrfs_dirty_pages(BTRFS_I(inode), pages,
					dirty_pages, pos, copied,
					&cached_state, only_release_metadata);

		/*
		 * If we have not locked the extent range, because the range's
		 * start offset is >= i_size, we might still have a non-NULL
		 * cached extent state, acquired while marking the extent range
		 * as delalloc through btrfs_dirty_pages(). Therefore free any
		 * possible cached extent state to avoid a memory leak.
		 */
		if (extents_locked)
			unlock_extent_cached(&BTRFS_I(inode)->io_tree,
					     lockstart, lockend, &cached_state);
		else
			free_extent_state(cached_state);

		btrfs_delalloc_release_extents(BTRFS_I(inode), reserve_bytes);
		if (ret) {
			btrfs_drop_pages(pages, num_pages);
			break;
		}

		release_bytes = 0;
		if (only_release_metadata)
			btrfs_check_nocow_unlock(BTRFS_I(inode));

		btrfs_drop_pages(pages, num_pages);

		cond_resched();

		balance_dirty_pages_ratelimited(inode->i_mapping);

		pos += copied;
		num_written += copied;
	}

	kfree(pages);

	if (release_bytes) {
		if (only_release_metadata) {
			btrfs_check_nocow_unlock(BTRFS_I(inode));
			btrfs_delalloc_release_metadata(BTRFS_I(inode),
					release_bytes, true);
		} else {
			btrfs_delalloc_release_space(BTRFS_I(inode),
					data_reserved,
					round_down(pos, fs_info->sectorsize),
					release_bytes, true);
		}
	}

	extent_changeset_free(data_reserved);
	if (num_written > 0) {
		pagecache_isize_extended(inode, old_isize, iocb->ki_pos);
		iocb->ki_pos += num_written;
	}
out:
	btrfs_inode_unlock(inode, ilock_flags);
	return num_written ? num_written : ret;
}

static ssize_t check_direct_IO(struct btrfs_fs_info *fs_info,
			       const struct iov_iter *iter, loff_t offset)
{
	const u32 blocksize_mask = fs_info->sectorsize - 1;

	if (offset & blocksize_mask)
		return -EINVAL;

	if (iov_iter_alignment(iter) & blocksize_mask)
		return -EINVAL;

	return 0;
}

static ssize_t btrfs_direct_write(struct kiocb *iocb, struct iov_iter *from)
{
	struct file *file = iocb->ki_filp;
	struct inode *inode = file_inode(file);
	struct btrfs_fs_info *fs_info = btrfs_sb(inode->i_sb);
	loff_t pos;
	ssize_t written = 0;
	ssize_t written_buffered;
	loff_t endbyte;
	ssize_t err;
	unsigned int ilock_flags = 0;
	struct iomap_dio *dio = NULL;

	if (iocb->ki_flags & IOCB_NOWAIT)
		ilock_flags |= BTRFS_ILOCK_TRY;

	/* If the write DIO is within EOF, use a shared lock */
	if (iocb->ki_pos + iov_iter_count(from) <= i_size_read(inode))
		ilock_flags |= BTRFS_ILOCK_SHARED;

relock:
	err = btrfs_inode_lock(inode, ilock_flags);
	if (err < 0)
		return err;

	err = generic_write_checks(iocb, from);
	if (err <= 0) {
		btrfs_inode_unlock(inode, ilock_flags);
		return err;
	}

	err = btrfs_write_check(iocb, from, err);
	if (err < 0) {
		btrfs_inode_unlock(inode, ilock_flags);
		goto out;
	}

	pos = iocb->ki_pos;
	/*
	 * Re-check since file size may have changed just before taking the
	 * lock or pos may have changed because of O_APPEND in generic_write_check()
	 */
	if ((ilock_flags & BTRFS_ILOCK_SHARED) &&
	    pos + iov_iter_count(from) > i_size_read(inode)) {
		btrfs_inode_unlock(inode, ilock_flags);
		ilock_flags &= ~BTRFS_ILOCK_SHARED;
		goto relock;
	}

	if (check_direct_IO(fs_info, from, pos)) {
		btrfs_inode_unlock(inode, ilock_flags);
		goto buffered;
	}

	dio = __iomap_dio_rw(iocb, from, &btrfs_dio_iomap_ops, &btrfs_dio_ops,
			     0);

	btrfs_inode_unlock(inode, ilock_flags);

	if (IS_ERR_OR_NULL(dio)) {
		err = PTR_ERR_OR_ZERO(dio);
		if (err < 0 && err != -ENOTBLK)
			goto out;
	} else {
		written = iomap_dio_complete(dio);
	}

	if (written < 0 || !iov_iter_count(from)) {
		err = written;
		goto out;
	}

buffered:
	pos = iocb->ki_pos;
	written_buffered = btrfs_buffered_write(iocb, from);
	if (written_buffered < 0) {
		err = written_buffered;
		goto out;
	}
	/*
	 * Ensure all data is persisted. We want the next direct IO read to be
	 * able to read what was just written.
	 */
	endbyte = pos + written_buffered - 1;
	err = btrfs_fdatawrite_range(inode, pos, endbyte);
	if (err)
		goto out;
	err = filemap_fdatawait_range(inode->i_mapping, pos, endbyte);
	if (err)
		goto out;
	written += written_buffered;
	iocb->ki_pos = pos + written_buffered;
	invalidate_mapping_pages(file->f_mapping, pos >> PAGE_SHIFT,
				 endbyte >> PAGE_SHIFT);
out:
	return written ? written : err;
}

static ssize_t btrfs_file_write_iter(struct kiocb *iocb,
				    struct iov_iter *from)
{
	struct file *file = iocb->ki_filp;
	struct btrfs_inode *inode = BTRFS_I(file_inode(file));
	ssize_t num_written = 0;
	const bool sync = iocb->ki_flags & IOCB_DSYNC;

	/*
	 * If the fs flips readonly due to some impossible error, although we
	 * have opened a file as writable, we have to stop this write operation
	 * to ensure consistency.
	 */
	if (test_bit(BTRFS_FS_STATE_ERROR, &inode->root->fs_info->fs_state))
		return -EROFS;

	if (!(iocb->ki_flags & IOCB_DIRECT) &&
	    (iocb->ki_flags & IOCB_NOWAIT))
		return -EOPNOTSUPP;

	if (sync)
		atomic_inc(&inode->sync_writers);

	if (iocb->ki_flags & IOCB_DIRECT)
		num_written = btrfs_direct_write(iocb, from);
	else
		num_written = btrfs_buffered_write(iocb, from);

	btrfs_set_inode_last_sub_trans(inode);

	if (num_written > 0)
		num_written = generic_write_sync(iocb, num_written);

	if (sync)
		atomic_dec(&inode->sync_writers);

	current->backing_dev_info = NULL;
	return num_written;
}

int btrfs_release_file(struct inode *inode, struct file *filp)
{
	struct btrfs_file_private *private = filp->private_data;

	if (private && private->filldir_buf)
		kfree(private->filldir_buf);
	kfree(private);
	filp->private_data = NULL;

	/*
	 * Set by setattr when we are about to truncate a file from a non-zero
	 * size to a zero size.  This tries to flush down new bytes that may
	 * have been written if the application were using truncate to replace
	 * a file in place.
	 */
	if (test_and_clear_bit(BTRFS_INODE_FLUSH_ON_CLOSE,
			       &BTRFS_I(inode)->runtime_flags))
			filemap_flush(inode->i_mapping);
	return 0;
}

static int start_ordered_ops(struct inode *inode, loff_t start, loff_t end)
{
	int ret;
	struct blk_plug plug;

	/*
	 * This is only called in fsync, which would do synchronous writes, so
	 * a plug can merge adjacent IOs as much as possible.  Esp. in case of
	 * multiple disks using raid profile, a large IO can be split to
	 * several segments of stripe length (currently 64K).
	 */
	blk_start_plug(&plug);
	atomic_inc(&BTRFS_I(inode)->sync_writers);
	ret = btrfs_fdatawrite_range(inode, start, end);
	atomic_dec(&BTRFS_I(inode)->sync_writers);
	blk_finish_plug(&plug);

	return ret;
}

static inline bool skip_inode_logging(const struct btrfs_log_ctx *ctx)
{
	struct btrfs_inode *inode = BTRFS_I(ctx->inode);
	struct btrfs_fs_info *fs_info = inode->root->fs_info;

	if (btrfs_inode_in_log(inode, fs_info->generation) &&
	    list_empty(&ctx->ordered_extents))
		return true;

	/*
	 * If we are doing a fast fsync we can not bail out if the inode's
	 * last_trans is <= then the last committed transaction, because we only
	 * update the last_trans of the inode during ordered extent completion,
	 * and for a fast fsync we don't wait for that, we only wait for the
	 * writeback to complete.
	 */
	if (inode->last_trans <= fs_info->last_trans_committed &&
	    (test_bit(BTRFS_INODE_NEEDS_FULL_SYNC, &inode->runtime_flags) ||
	     list_empty(&ctx->ordered_extents)))
		return true;

	return false;
}

/*
 * fsync call for both files and directories.  This logs the inode into
 * the tree log instead of forcing full commits whenever possible.
 *
 * It needs to call filemap_fdatawait so that all ordered extent updates are
 * in the metadata btree are up to date for copying to the log.
 *
 * It drops the inode mutex before doing the tree log commit.  This is an
 * important optimization for directories because holding the mutex prevents
 * new operations on the dir while we write to disk.
 */
int btrfs_sync_file(struct file *file, loff_t start, loff_t end, int datasync)
{
	struct dentry *dentry = file_dentry(file);
	struct inode *inode = d_inode(dentry);
	struct btrfs_fs_info *fs_info = btrfs_sb(inode->i_sb);
	struct btrfs_root *root = BTRFS_I(inode)->root;
	struct btrfs_trans_handle *trans;
	struct btrfs_log_ctx ctx;
	int ret = 0, err;
	u64 len;
	bool full_sync;

	trace_btrfs_sync_file(file, datasync);

	btrfs_init_log_ctx(&ctx, inode);

	/*
	 * Always set the range to a full range, otherwise we can get into
	 * several problems, from missing file extent items to represent holes
	 * when not using the NO_HOLES feature, to log tree corruption due to
	 * races between hole detection during logging and completion of ordered
	 * extents outside the range, to missing checksums due to ordered extents
	 * for which we flushed only a subset of their pages.
	 */
	start = 0;
	end = LLONG_MAX;
	len = (u64)LLONG_MAX + 1;

	/*
	 * We write the dirty pages in the range and wait until they complete
	 * out of the ->i_mutex. If so, we can flush the dirty pages by
	 * multi-task, and make the performance up.  See
	 * btrfs_wait_ordered_range for an explanation of the ASYNC check.
	 */
	ret = start_ordered_ops(inode, start, end);
	if (ret)
		goto out;

	btrfs_inode_lock(inode, BTRFS_ILOCK_MMAP);

	atomic_inc(&root->log_batch);

	/*
	 * Always check for the full sync flag while holding the inode's lock,
	 * to avoid races with other tasks. The flag must be either set all the
	 * time during logging or always off all the time while logging.
	 */
	full_sync = test_bit(BTRFS_INODE_NEEDS_FULL_SYNC,
			     &BTRFS_I(inode)->runtime_flags);

	/*
	 * Before we acquired the inode's lock and the mmap lock, someone may
	 * have dirtied more pages in the target range. We need to make sure
	 * that writeback for any such pages does not start while we are logging
	 * the inode, because if it does, any of the following might happen when
	 * we are not doing a full inode sync:
	 *
	 * 1) We log an extent after its writeback finishes but before its
	 *    checksums are added to the csum tree, leading to -EIO errors
	 *    when attempting to read the extent after a log replay.
	 *
	 * 2) We can end up logging an extent before its writeback finishes.
	 *    Therefore after the log replay we will have a file extent item
	 *    pointing to an unwritten extent (and no data checksums as well).
	 *
	 * So trigger writeback for any eventual new dirty pages and then we
	 * wait for all ordered extents to complete below.
	 */
	ret = start_ordered_ops(inode, start, end);
	if (ret) {
		btrfs_inode_unlock(inode, BTRFS_ILOCK_MMAP);
		goto out;
	}

	/*
	 * We have to do this here to avoid the priority inversion of waiting on
	 * IO of a lower priority task while holding a transaction open.
	 *
	 * For a full fsync we wait for the ordered extents to complete while
	 * for a fast fsync we wait just for writeback to complete, and then
	 * attach the ordered extents to the transaction so that a transaction
	 * commit waits for their completion, to avoid data loss if we fsync,
	 * the current transaction commits before the ordered extents complete
	 * and a power failure happens right after that.
	 *
	 * For zoned filesystem, if a write IO uses a ZONE_APPEND command, the
	 * logical address recorded in the ordered extent may change. We need
	 * to wait for the IO to stabilize the logical address.
	 */
	if (full_sync || btrfs_is_zoned(fs_info)) {
		ret = btrfs_wait_ordered_range(inode, start, len);
	} else {
		/*
		 * Get our ordered extents as soon as possible to avoid doing
		 * checksum lookups in the csum tree, and use instead the
		 * checksums attached to the ordered extents.
		 */
		btrfs_get_ordered_extents_for_logging(BTRFS_I(inode),
						      &ctx.ordered_extents);
		ret = filemap_fdatawait_range(inode->i_mapping, start, end);
	}

	if (ret)
		goto out_release_extents;

	atomic_inc(&root->log_batch);

	smp_mb();
	if (skip_inode_logging(&ctx)) {
		/*
		 * We've had everything committed since the last time we were
		 * modified so clear this flag in case it was set for whatever
		 * reason, it's no longer relevant.
		 */
		clear_bit(BTRFS_INODE_NEEDS_FULL_SYNC,
			  &BTRFS_I(inode)->runtime_flags);
		/*
		 * An ordered extent might have started before and completed
		 * already with io errors, in which case the inode was not
		 * updated and we end up here. So check the inode's mapping
		 * for any errors that might have happened since we last
		 * checked called fsync.
		 */
		ret = filemap_check_wb_err(inode->i_mapping, file->f_wb_err);
		goto out_release_extents;
	}

	/*
	 * We use start here because we will need to wait on the IO to complete
	 * in btrfs_sync_log, which could require joining a transaction (for
	 * example checking cross references in the nocow path).  If we use join
	 * here we could get into a situation where we're waiting on IO to
	 * happen that is blocked on a transaction trying to commit.  With start
	 * we inc the extwriter counter, so we wait for all extwriters to exit
	 * before we start blocking joiners.  This comment is to keep somebody
	 * from thinking they are super smart and changing this to
	 * btrfs_join_transaction *cough*Josef*cough*.
	 */
	trans = btrfs_start_transaction(root, 0);
	if (IS_ERR(trans)) {
		ret = PTR_ERR(trans);
		goto out_release_extents;
	}
	trans->in_fsync = true;

	ret = btrfs_log_dentry_safe(trans, dentry, &ctx);
	btrfs_release_log_ctx_extents(&ctx);
	if (ret < 0) {
		/* Fallthrough and commit/free transaction. */
		ret = 1;
	}

	/* we've logged all the items and now have a consistent
	 * version of the file in the log.  It is possible that
	 * someone will come in and modify the file, but that's
	 * fine because the log is consistent on disk, and we
	 * have references to all of the file's extents
	 *
	 * It is possible that someone will come in and log the
	 * file again, but that will end up using the synchronization
	 * inside btrfs_sync_log to keep things safe.
	 */
	btrfs_inode_unlock(inode, BTRFS_ILOCK_MMAP);

	if (ret != BTRFS_NO_LOG_SYNC) {
		if (!ret) {
			ret = btrfs_sync_log(trans, root, &ctx);
			if (!ret) {
				ret = btrfs_end_transaction(trans);
				goto out;
			}
		}
		if (!full_sync) {
			ret = btrfs_wait_ordered_range(inode, start, len);
			if (ret) {
				btrfs_end_transaction(trans);
				goto out;
			}
		}
		ret = btrfs_commit_transaction(trans);
	} else {
		ret = btrfs_end_transaction(trans);
	}
out:
	ASSERT(list_empty(&ctx.list));
	err = file_check_and_advance_wb_err(file);
	if (!ret)
		ret = err;
	return ret > 0 ? -EIO : ret;

out_release_extents:
	btrfs_release_log_ctx_extents(&ctx);
	btrfs_inode_unlock(inode, BTRFS_ILOCK_MMAP);
	goto out;
}

static const struct vm_operations_struct btrfs_file_vm_ops = {
	.fault		= filemap_fault,
	.map_pages	= filemap_map_pages,
	.page_mkwrite	= btrfs_page_mkwrite,
};

static int btrfs_file_mmap(struct file	*filp, struct vm_area_struct *vma)
{
	struct address_space *mapping = filp->f_mapping;

	if (!mapping->a_ops->readpage)
		return -ENOEXEC;

	file_accessed(filp);
	vma->vm_ops = &btrfs_file_vm_ops;

	return 0;
}

static int hole_mergeable(struct btrfs_inode *inode, struct extent_buffer *leaf,
			  int slot, u64 start, u64 end)
{
	struct btrfs_file_extent_item *fi;
	struct btrfs_key key;

	if (slot < 0 || slot >= btrfs_header_nritems(leaf))
		return 0;

	btrfs_item_key_to_cpu(leaf, &key, slot);
	if (key.objectid != btrfs_ino(inode) ||
	    key.type != BTRFS_EXTENT_DATA_KEY)
		return 0;

	fi = btrfs_item_ptr(leaf, slot, struct btrfs_file_extent_item);

	if (btrfs_file_extent_type(leaf, fi) != BTRFS_FILE_EXTENT_REG)
		return 0;

	if (btrfs_file_extent_disk_bytenr(leaf, fi))
		return 0;

	if (key.offset == end)
		return 1;
	if (key.offset + btrfs_file_extent_num_bytes(leaf, fi) == start)
		return 1;
	return 0;
}

static int fill_holes(struct btrfs_trans_handle *trans,
		struct btrfs_inode *inode,
		struct btrfs_path *path, u64 offset, u64 end)
{
	struct btrfs_fs_info *fs_info = trans->fs_info;
	struct btrfs_root *root = inode->root;
	struct extent_buffer *leaf;
	struct btrfs_file_extent_item *fi;
	struct extent_map *hole_em;
	struct extent_map_tree *em_tree = &inode->extent_tree;
	struct btrfs_key key;
	int ret;

	if (btrfs_fs_incompat(fs_info, NO_HOLES))
		goto out;

	key.objectid = btrfs_ino(inode);
	key.type = BTRFS_EXTENT_DATA_KEY;
	key.offset = offset;

	ret = btrfs_search_slot(trans, root, &key, path, 0, 1);
	if (ret <= 0) {
		/*
		 * We should have dropped this offset, so if we find it then
		 * something has gone horribly wrong.
		 */
		if (ret == 0)
			ret = -EINVAL;
		return ret;
	}

	leaf = path->nodes[0];
	if (hole_mergeable(inode, leaf, path->slots[0] - 1, offset, end)) {
		u64 num_bytes;

		path->slots[0]--;
		fi = btrfs_item_ptr(leaf, path->slots[0],
				    struct btrfs_file_extent_item);
		num_bytes = btrfs_file_extent_num_bytes(leaf, fi) +
			end - offset;
		btrfs_set_file_extent_num_bytes(leaf, fi, num_bytes);
		btrfs_set_file_extent_ram_bytes(leaf, fi, num_bytes);
		btrfs_set_file_extent_offset(leaf, fi, 0);
		btrfs_mark_buffer_dirty(leaf);
		goto out;
	}

	if (hole_mergeable(inode, leaf, path->slots[0], offset, end)) {
		u64 num_bytes;

		key.offset = offset;
		btrfs_set_item_key_safe(fs_info, path, &key);
		fi = btrfs_item_ptr(leaf, path->slots[0],
				    struct btrfs_file_extent_item);
		num_bytes = btrfs_file_extent_num_bytes(leaf, fi) + end -
			offset;
		btrfs_set_file_extent_num_bytes(leaf, fi, num_bytes);
		btrfs_set_file_extent_ram_bytes(leaf, fi, num_bytes);
		btrfs_set_file_extent_offset(leaf, fi, 0);
		btrfs_mark_buffer_dirty(leaf);
		goto out;
	}
	btrfs_release_path(path);

	ret = btrfs_insert_file_extent(trans, root, btrfs_ino(inode),
			offset, 0, 0, end - offset, 0, end - offset, 0, 0, 0);
	if (ret)
		return ret;

out:
	btrfs_release_path(path);

	hole_em = alloc_extent_map();
	if (!hole_em) {
		btrfs_drop_extent_cache(inode, offset, end - 1, 0);
		set_bit(BTRFS_INODE_NEEDS_FULL_SYNC, &inode->runtime_flags);
	} else {
		hole_em->start = offset;
		hole_em->len = end - offset;
		hole_em->ram_bytes = hole_em->len;
		hole_em->orig_start = offset;

		hole_em->block_start = EXTENT_MAP_HOLE;
		hole_em->block_len = 0;
		hole_em->orig_block_len = 0;
		hole_em->compress_type = BTRFS_COMPRESS_NONE;
		hole_em->generation = trans->transid;

		do {
			btrfs_drop_extent_cache(inode, offset, end - 1, 0);
			write_lock(&em_tree->lock);
			ret = add_extent_mapping(em_tree, hole_em, 1);
			write_unlock(&em_tree->lock);
		} while (ret == -EEXIST);
		free_extent_map(hole_em);
		if (ret)
			set_bit(BTRFS_INODE_NEEDS_FULL_SYNC,
					&inode->runtime_flags);
	}

	return 0;
}

/*
 * Find a hole extent on given inode and change start/len to the end of hole
 * extent.(hole/vacuum extent whose em->start <= start &&
 *	   em->start + em->len > start)
 * When a hole extent is found, return 1 and modify start/len.
 */
static int find_first_non_hole(struct btrfs_inode *inode, u64 *start, u64 *len)
{
	struct btrfs_fs_info *fs_info = inode->root->fs_info;
	struct extent_map *em;
	int ret = 0;

	em = btrfs_get_extent(inode, NULL, 0,
			      round_down(*start, fs_info->sectorsize),
			      round_up(*len, fs_info->sectorsize));
	if (IS_ERR(em))
		return PTR_ERR(em);

	/* Hole or vacuum extent(only exists in no-hole mode) */
	if (em->block_start == EXTENT_MAP_HOLE) {
		ret = 1;
		*len = em->start + em->len > *start + *len ?
		       0 : *start + *len - em->start - em->len;
		*start = em->start + em->len;
	}
	free_extent_map(em);
	return ret;
}

static int btrfs_punch_hole_lock_range(struct inode *inode,
				       const u64 lockstart,
				       const u64 lockend,
				       struct extent_state **cached_state)
{
	while (1) {
		struct btrfs_ordered_extent *ordered;
		int ret;

		truncate_pagecache_range(inode, lockstart, lockend);

		lock_extent_bits(&BTRFS_I(inode)->io_tree, lockstart, lockend,
				 cached_state);
		ordered = btrfs_lookup_first_ordered_extent(BTRFS_I(inode),
							    lockend);

		/*
		 * We need to make sure we have no ordered extents in this range
		 * and nobody raced in and read a page in this range, if we did
		 * we need to try again.
		 */
		if ((!ordered ||
		    (ordered->file_offset + ordered->num_bytes <= lockstart ||
		     ordered->file_offset > lockend)) &&
		     !filemap_range_has_page(inode->i_mapping,
					     lockstart, lockend)) {
			if (ordered)
				btrfs_put_ordered_extent(ordered);
			break;
		}
		if (ordered)
			btrfs_put_ordered_extent(ordered);
		unlock_extent_cached(&BTRFS_I(inode)->io_tree, lockstart,
				     lockend, cached_state);
		ret = btrfs_wait_ordered_range(inode, lockstart,
					       lockend - lockstart + 1);
		if (ret)
			return ret;
	}
	return 0;
}

static int btrfs_insert_replace_extent(struct btrfs_trans_handle *trans,
				     struct btrfs_inode *inode,
				     struct btrfs_path *path,
				     struct btrfs_replace_extent_info *extent_info,
				     const u64 replace_len,
				     const u64 bytes_to_drop)
{
	struct btrfs_fs_info *fs_info = trans->fs_info;
	struct btrfs_root *root = inode->root;
	struct btrfs_file_extent_item *extent;
	struct extent_buffer *leaf;
	struct btrfs_key key;
	int slot;
	struct btrfs_ref ref = { 0 };
	int ret;

	if (replace_len == 0)
		return 0;

	if (extent_info->disk_offset == 0 &&
	    btrfs_fs_incompat(fs_info, NO_HOLES)) {
		btrfs_update_inode_bytes(inode, 0, bytes_to_drop);
		return 0;
	}

	key.objectid = btrfs_ino(inode);
	key.type = BTRFS_EXTENT_DATA_KEY;
	key.offset = extent_info->file_offset;
	ret = btrfs_insert_empty_item(trans, root, path, &key,
				      sizeof(struct btrfs_file_extent_item));
	if (ret)
		return ret;
	leaf = path->nodes[0];
	slot = path->slots[0];
	write_extent_buffer(leaf, extent_info->extent_buf,
			    btrfs_item_ptr_offset(leaf, slot),
			    sizeof(struct btrfs_file_extent_item));
	extent = btrfs_item_ptr(leaf, slot, struct btrfs_file_extent_item);
	ASSERT(btrfs_file_extent_type(leaf, extent) != BTRFS_FILE_EXTENT_INLINE);
	btrfs_set_file_extent_offset(leaf, extent, extent_info->data_offset);
	btrfs_set_file_extent_num_bytes(leaf, extent, replace_len);
	if (extent_info->is_new_extent)
		btrfs_set_file_extent_generation(leaf, extent, trans->transid);
	btrfs_mark_buffer_dirty(leaf);
	btrfs_release_path(path);

	ret = btrfs_inode_set_file_extent_range(inode, extent_info->file_offset,
						replace_len);
	if (ret)
		return ret;

	/* If it's a hole, nothing more needs to be done. */
	if (extent_info->disk_offset == 0) {
		btrfs_update_inode_bytes(inode, 0, bytes_to_drop);
		return 0;
	}

	btrfs_update_inode_bytes(inode, replace_len, bytes_to_drop);

	if (extent_info->is_new_extent && extent_info->insertions == 0) {
		key.objectid = extent_info->disk_offset;
		key.type = BTRFS_EXTENT_ITEM_KEY;
		key.offset = extent_info->disk_len;
		ret = btrfs_alloc_reserved_file_extent(trans, root,
						       btrfs_ino(inode),
						       extent_info->file_offset,
						       extent_info->qgroup_reserved,
						       &key);
	} else {
		u64 ref_offset;

		btrfs_init_generic_ref(&ref, BTRFS_ADD_DELAYED_REF,
				       extent_info->disk_offset,
				       extent_info->disk_len, 0);
		ref_offset = extent_info->file_offset - extent_info->data_offset;
		btrfs_init_data_ref(&ref, root->root_key.objectid,
				    btrfs_ino(inode), ref_offset);
		ret = btrfs_inc_extent_ref(trans, &ref);
	}

	extent_info->insertions++;

	return ret;
}

/*
 * The respective range must have been previously locked, as well as the inode.
 * The end offset is inclusive (last byte of the range).
 * @extent_info is NULL for fallocate's hole punching and non-NULL when replacing
 * the file range with an extent.
 * When not punching a hole, we don't want to end up in a state where we dropped
 * extents without inserting a new one, so we must abort the transaction to avoid
 * a corruption.
 */
int btrfs_replace_file_extents(struct btrfs_inode *inode,
			       struct btrfs_path *path, const u64 start,
			       const u64 end,
			       struct btrfs_replace_extent_info *extent_info,
			       struct btrfs_trans_handle **trans_out)
{
	struct btrfs_drop_extents_args drop_args = { 0 };
	struct btrfs_root *root = inode->root;
	struct btrfs_fs_info *fs_info = root->fs_info;
	u64 min_size = btrfs_calc_insert_metadata_size(fs_info, 1);
	u64 ino_size = round_up(inode->vfs_inode.i_size, fs_info->sectorsize);
	struct btrfs_trans_handle *trans = NULL;
	struct btrfs_block_rsv *rsv;
	unsigned int rsv_count;
	u64 cur_offset;
	u64 len = end - start;
	int ret = 0;

	if (end <= start)
		return -EINVAL;

	rsv = btrfs_alloc_block_rsv(fs_info, BTRFS_BLOCK_RSV_TEMP);
	if (!rsv) {
		ret = -ENOMEM;
		goto out;
	}
	rsv->size = btrfs_calc_insert_metadata_size(fs_info, 1);
	rsv->failfast = 1;

	/*
	 * 1 - update the inode
	 * 1 - removing the extents in the range
	 * 1 - adding the hole extent if no_holes isn't set or if we are
	 *     replacing the range with a new extent
	 */
	if (!btrfs_fs_incompat(fs_info, NO_HOLES) || extent_info)
		rsv_count = 3;
	else
		rsv_count = 2;

	trans = btrfs_start_transaction(root, rsv_count);
	if (IS_ERR(trans)) {
		ret = PTR_ERR(trans);
		trans = NULL;
		goto out_free;
	}

	ret = btrfs_block_rsv_migrate(&fs_info->trans_block_rsv, rsv,
				      min_size, false);
	BUG_ON(ret);
	trans->block_rsv = rsv;

	cur_offset = start;
	drop_args.path = path;
	drop_args.end = end + 1;
	drop_args.drop_cache = true;
	while (cur_offset < end) {
		drop_args.start = cur_offset;
		ret = btrfs_drop_extents(trans, root, inode, &drop_args);
		/* If we are punching a hole decrement the inode's byte count */
		if (!extent_info)
			btrfs_update_inode_bytes(inode, 0,
						 drop_args.bytes_found);
		if (ret != -ENOSPC) {
			/*
			 * When cloning we want to avoid transaction aborts when
			 * nothing was done and we are attempting to clone parts
			 * of inline extents, in such cases -EOPNOTSUPP is
			 * returned by __btrfs_drop_extents() without having
			 * changed anything in the file.
			 */
			if (extent_info && !extent_info->is_new_extent &&
			    ret && ret != -EOPNOTSUPP)
				btrfs_abort_transaction(trans, ret);
			break;
		}

		trans->block_rsv = &fs_info->trans_block_rsv;

		if (!extent_info && cur_offset < drop_args.drop_end &&
		    cur_offset < ino_size) {
			ret = fill_holes(trans, inode, path, cur_offset,
					 drop_args.drop_end);
			if (ret) {
				/*
				 * If we failed then we didn't insert our hole
				 * entries for the area we dropped, so now the
				 * fs is corrupted, so we must abort the
				 * transaction.
				 */
				btrfs_abort_transaction(trans, ret);
				break;
			}
		} else if (!extent_info && cur_offset < drop_args.drop_end) {
			/*
			 * We are past the i_size here, but since we didn't
			 * insert holes we need to clear the mapped area so we
			 * know to not set disk_i_size in this area until a new
			 * file extent is inserted here.
			 */
			ret = btrfs_inode_clear_file_extent_range(inode,
					cur_offset,
					drop_args.drop_end - cur_offset);
			if (ret) {
				/*
				 * We couldn't clear our area, so we could
				 * presumably adjust up and corrupt the fs, so
				 * we need to abort.
				 */
				btrfs_abort_transaction(trans, ret);
				break;
			}
		}

		if (extent_info &&
		    drop_args.drop_end > extent_info->file_offset) {
			u64 replace_len = drop_args.drop_end -
					  extent_info->file_offset;

			ret = btrfs_insert_replace_extent(trans, inode,	path,
					extent_info, replace_len,
					drop_args.bytes_found);
			if (ret) {
				btrfs_abort_transaction(trans, ret);
				break;
			}
			extent_info->data_len -= replace_len;
			extent_info->data_offset += replace_len;
			extent_info->file_offset += replace_len;
		}

		ret = btrfs_update_inode(trans, root, inode);
		if (ret)
			break;

		btrfs_end_transaction(trans);
		btrfs_btree_balance_dirty(fs_info);

		trans = btrfs_start_transaction(root, rsv_count);
		if (IS_ERR(trans)) {
			ret = PTR_ERR(trans);
			trans = NULL;
			break;
		}

		ret = btrfs_block_rsv_migrate(&fs_info->trans_block_rsv,
					      rsv, min_size, false);
		BUG_ON(ret);	/* shouldn't happen */
		trans->block_rsv = rsv;

		cur_offset = drop_args.drop_end;
		len = end - cur_offset;
		if (!extent_info && len) {
			ret = find_first_non_hole(inode, &cur_offset, &len);
			if (unlikely(ret < 0))
				break;
			if (ret && !len) {
				ret = 0;
				break;
			}
		}
	}

	/*
	 * If we were cloning, force the next fsync to be a full one since we
	 * we replaced (or just dropped in the case of cloning holes when
	 * NO_HOLES is enabled) file extent items and did not setup new extent
	 * maps for the replacement extents (or holes).
	 */
	if (extent_info && !extent_info->is_new_extent)
		set_bit(BTRFS_INODE_NEEDS_FULL_SYNC, &inode->runtime_flags);

	if (ret)
		goto out_trans;

	trans->block_rsv = &fs_info->trans_block_rsv;
	/*
	 * If we are using the NO_HOLES feature we might have had already an
	 * hole that overlaps a part of the region [lockstart, lockend] and
	 * ends at (or beyond) lockend. Since we have no file extent items to
	 * represent holes, drop_end can be less than lockend and so we must
	 * make sure we have an extent map representing the existing hole (the
	 * call to __btrfs_drop_extents() might have dropped the existing extent
	 * map representing the existing hole), otherwise the fast fsync path
	 * will not record the existence of the hole region
	 * [existing_hole_start, lockend].
	 */
	if (drop_args.drop_end <= end)
		drop_args.drop_end = end + 1;
	/*
	 * Don't insert file hole extent item if it's for a range beyond eof
	 * (because it's useless) or if it represents a 0 bytes range (when
	 * cur_offset == drop_end).
	 */
	if (!extent_info && cur_offset < ino_size &&
	    cur_offset < drop_args.drop_end) {
		ret = fill_holes(trans, inode, path, cur_offset,
				 drop_args.drop_end);
		if (ret) {
			/* Same comment as above. */
			btrfs_abort_transaction(trans, ret);
			goto out_trans;
		}
	} else if (!extent_info && cur_offset < drop_args.drop_end) {
		/* See the comment in the loop above for the reasoning here. */
		ret = btrfs_inode_clear_file_extent_range(inode, cur_offset,
					drop_args.drop_end - cur_offset);
		if (ret) {
			btrfs_abort_transaction(trans, ret);
			goto out_trans;
		}

	}
	if (extent_info) {
		ret = btrfs_insert_replace_extent(trans, inode, path,
				extent_info, extent_info->data_len,
				drop_args.bytes_found);
		if (ret) {
			btrfs_abort_transaction(trans, ret);
			goto out_trans;
		}
	}

out_trans:
	if (!trans)
		goto out_free;

	trans->block_rsv = &fs_info->trans_block_rsv;
	if (ret)
		btrfs_end_transaction(trans);
	else
		*trans_out = trans;
out_free:
	btrfs_free_block_rsv(fs_info, rsv);
out:
	return ret;
}

static int btrfs_punch_hole(struct inode *inode, loff_t offset, loff_t len)
{
	struct btrfs_fs_info *fs_info = btrfs_sb(inode->i_sb);
	struct btrfs_root *root = BTRFS_I(inode)->root;
	struct extent_state *cached_state = NULL;
	struct btrfs_path *path;
	struct btrfs_trans_handle *trans = NULL;
	u64 lockstart;
	u64 lockend;
	u64 tail_start;
	u64 tail_len;
	u64 orig_start = offset;
	int ret = 0;
	bool same_block;
	u64 ino_size;
	bool truncated_block = false;
	bool updated_inode = false;

	ret = btrfs_wait_ordered_range(inode, offset, len);
	if (ret)
		return ret;

	btrfs_inode_lock(inode, BTRFS_ILOCK_MMAP);
	ino_size = round_up(inode->i_size, fs_info->sectorsize);
	ret = find_first_non_hole(BTRFS_I(inode), &offset, &len);
	if (ret < 0)
		goto out_only_mutex;
	if (ret && !len) {
		/* Already in a large hole */
		ret = 0;
		goto out_only_mutex;
	}

	lockstart = round_up(offset, btrfs_inode_sectorsize(BTRFS_I(inode)));
	lockend = round_down(offset + len,
			     btrfs_inode_sectorsize(BTRFS_I(inode))) - 1;
	same_block = (BTRFS_BYTES_TO_BLKS(fs_info, offset))
		== (BTRFS_BYTES_TO_BLKS(fs_info, offset + len - 1));
	/*
	 * We needn't truncate any block which is beyond the end of the file
	 * because we are sure there is no data there.
	 */
	/*
	 * Only do this if we are in the same block and we aren't doing the
	 * entire block.
	 */
	if (same_block && len < fs_info->sectorsize) {
		if (offset < ino_size) {
			truncated_block = true;
			ret = btrfs_truncate_block(BTRFS_I(inode), offset, len,
						   0);
		} else {
			ret = 0;
		}
		goto out_only_mutex;
	}

	/* zero back part of the first block */
	if (offset < ino_size) {
		truncated_block = true;
		ret = btrfs_truncate_block(BTRFS_I(inode), offset, 0, 0);
		if (ret) {
			btrfs_inode_unlock(inode, BTRFS_ILOCK_MMAP);
			return ret;
		}
	}

	/* Check the aligned pages after the first unaligned page,
	 * if offset != orig_start, which means the first unaligned page
	 * including several following pages are already in holes,
	 * the extra check can be skipped */
	if (offset == orig_start) {
		/* after truncate page, check hole again */
		len = offset + len - lockstart;
		offset = lockstart;
		ret = find_first_non_hole(BTRFS_I(inode), &offset, &len);
		if (ret < 0)
			goto out_only_mutex;
		if (ret && !len) {
			ret = 0;
			goto out_only_mutex;
		}
		lockstart = offset;
	}

	/* Check the tail unaligned part is in a hole */
	tail_start = lockend + 1;
	tail_len = offset + len - tail_start;
	if (tail_len) {
		ret = find_first_non_hole(BTRFS_I(inode), &tail_start, &tail_len);
		if (unlikely(ret < 0))
			goto out_only_mutex;
		if (!ret) {
			/* zero the front end of the last page */
			if (tail_start + tail_len < ino_size) {
				truncated_block = true;
				ret = btrfs_truncate_block(BTRFS_I(inode),
							tail_start + tail_len,
							0, 1);
				if (ret)
					goto out_only_mutex;
			}
		}
	}

	if (lockend < lockstart) {
		ret = 0;
		goto out_only_mutex;
	}

	ret = btrfs_punch_hole_lock_range(inode, lockstart, lockend,
					  &cached_state);
	if (ret)
		goto out_only_mutex;

	path = btrfs_alloc_path();
	if (!path) {
		ret = -ENOMEM;
		goto out;
	}

	ret = btrfs_replace_file_extents(BTRFS_I(inode), path, lockstart,
					 lockend, NULL, &trans);
	btrfs_free_path(path);
	if (ret)
		goto out;

	ASSERT(trans != NULL);
	inode_inc_iversion(inode);
	inode->i_mtime = inode->i_ctime = current_time(inode);
	ret = btrfs_update_inode(trans, root, BTRFS_I(inode));
	updated_inode = true;
	btrfs_end_transaction(trans);
	btrfs_btree_balance_dirty(fs_info);
out:
	unlock_extent_cached(&BTRFS_I(inode)->io_tree, lockstart, lockend,
			     &cached_state);
out_only_mutex:
	if (!updated_inode && truncated_block && !ret) {
		/*
		 * If we only end up zeroing part of a page, we still need to
		 * update the inode item, so that all the time fields are
		 * updated as well as the necessary btrfs inode in memory fields
		 * for detecting, at fsync time, if the inode isn't yet in the
		 * log tree or it's there but not up to date.
		 */
		struct timespec64 now = current_time(inode);

		inode_inc_iversion(inode);
		inode->i_mtime = now;
		inode->i_ctime = now;
		trans = btrfs_start_transaction(root, 1);
		if (IS_ERR(trans)) {
			ret = PTR_ERR(trans);
		} else {
			int ret2;

			ret = btrfs_update_inode(trans, root, BTRFS_I(inode));
			ret2 = btrfs_end_transaction(trans);
			if (!ret)
				ret = ret2;
		}
	}
	btrfs_inode_unlock(inode, BTRFS_ILOCK_MMAP);
	return ret;
}

/* Helper structure to record which range is already reserved */
struct falloc_range {
	struct list_head list;
	u64 start;
	u64 len;
};

/*
 * Helper function to add falloc range
 *
 * Caller should have locked the larger range of extent containing
 * [start, len)
 */
static int add_falloc_range(struct list_head *head, u64 start, u64 len)
{
	struct falloc_range *prev = NULL;
	struct falloc_range *range = NULL;

	if (list_empty(head))
		goto insert;

	/*
	 * As fallocate iterate by bytenr order, we only need to check
	 * the last range.
	 */
	prev = list_entry(head->prev, struct falloc_range, list);
	if (prev->start + prev->len == start) {
		prev->len += len;
		return 0;
	}
insert:
	range = kmalloc(sizeof(*range), GFP_KERNEL);
	if (!range)
		return -ENOMEM;
	range->start = start;
	range->len = len;
	list_add_tail(&range->list, head);
	return 0;
}

static int btrfs_fallocate_update_isize(struct inode *inode,
					const u64 end,
					const int mode)
{
	struct btrfs_trans_handle *trans;
	struct btrfs_root *root = BTRFS_I(inode)->root;
	int ret;
	int ret2;

	if (mode & FALLOC_FL_KEEP_SIZE || end <= i_size_read(inode))
		return 0;

	trans = btrfs_start_transaction(root, 1);
	if (IS_ERR(trans))
		return PTR_ERR(trans);

	inode->i_ctime = current_time(inode);
	i_size_write(inode, end);
	btrfs_inode_safe_disk_i_size_write(BTRFS_I(inode), 0);
	ret = btrfs_update_inode(trans, root, BTRFS_I(inode));
	ret2 = btrfs_end_transaction(trans);

	return ret ? ret : ret2;
}

enum {
	RANGE_BOUNDARY_WRITTEN_EXTENT,
	RANGE_BOUNDARY_PREALLOC_EXTENT,
	RANGE_BOUNDARY_HOLE,
};

static int btrfs_zero_range_check_range_boundary(struct btrfs_inode *inode,
						 u64 offset)
{
	const u64 sectorsize = btrfs_inode_sectorsize(inode);
	struct extent_map *em;
	int ret;

	offset = round_down(offset, sectorsize);
	em = btrfs_get_extent(inode, NULL, 0, offset, sectorsize);
	if (IS_ERR(em))
		return PTR_ERR(em);

	if (em->block_start == EXTENT_MAP_HOLE)
		ret = RANGE_BOUNDARY_HOLE;
	else if (test_bit(EXTENT_FLAG_PREALLOC, &em->flags))
		ret = RANGE_BOUNDARY_PREALLOC_EXTENT;
	else
		ret = RANGE_BOUNDARY_WRITTEN_EXTENT;

	free_extent_map(em);
	return ret;
}

static int btrfs_zero_range(struct inode *inode,
			    loff_t offset,
			    loff_t len,
			    const int mode)
{
	struct btrfs_fs_info *fs_info = BTRFS_I(inode)->root->fs_info;
	struct extent_map *em;
	struct extent_changeset *data_reserved = NULL;
	int ret;
	u64 alloc_hint = 0;
	const u64 sectorsize = btrfs_inode_sectorsize(BTRFS_I(inode));
	u64 alloc_start = round_down(offset, sectorsize);
	u64 alloc_end = round_up(offset + len, sectorsize);
	u64 bytes_to_reserve = 0;
	bool space_reserved = false;

	inode_dio_wait(inode);

	em = btrfs_get_extent(BTRFS_I(inode), NULL, 0, alloc_start,
			      alloc_end - alloc_start);
	if (IS_ERR(em)) {
		ret = PTR_ERR(em);
		goto out;
	}

	/*
	 * Avoid hole punching and extent allocation for some cases. More cases
	 * could be considered, but these are unlikely common and we keep things
	 * as simple as possible for now. Also, intentionally, if the target
	 * range contains one or more prealloc extents together with regular
	 * extents and holes, we drop all the existing extents and allocate a
	 * new prealloc extent, so that we get a larger contiguous disk extent.
	 */
	if (em->start <= alloc_start &&
	    test_bit(EXTENT_FLAG_PREALLOC, &em->flags)) {
		const u64 em_end = em->start + em->len;

		if (em_end >= offset + len) {
			/*
			 * The whole range is already a prealloc extent,
			 * do nothing except updating the inode's i_size if
			 * needed.
			 */
			free_extent_map(em);
			ret = btrfs_fallocate_update_isize(inode, offset + len,
							   mode);
			goto out;
		}
		/*
		 * Part of the range is already a prealloc extent, so operate
		 * only on the remaining part of the range.
		 */
		alloc_start = em_end;
		ASSERT(IS_ALIGNED(alloc_start, sectorsize));
		len = offset + len - alloc_start;
		offset = alloc_start;
		alloc_hint = em->block_start + em->len;
	}
	free_extent_map(em);

	if (BTRFS_BYTES_TO_BLKS(fs_info, offset) ==
	    BTRFS_BYTES_TO_BLKS(fs_info, offset + len - 1)) {
		em = btrfs_get_extent(BTRFS_I(inode), NULL, 0, alloc_start,
				      sectorsize);
		if (IS_ERR(em)) {
			ret = PTR_ERR(em);
			goto out;
		}

		if (test_bit(EXTENT_FLAG_PREALLOC, &em->flags)) {
			free_extent_map(em);
			ret = btrfs_fallocate_update_isize(inode, offset + len,
							   mode);
			goto out;
		}
		if (len < sectorsize && em->block_start != EXTENT_MAP_HOLE) {
			free_extent_map(em);
			ret = btrfs_truncate_block(BTRFS_I(inode), offset, len,
						   0);
			if (!ret)
				ret = btrfs_fallocate_update_isize(inode,
								   offset + len,
								   mode);
			return ret;
		}
		free_extent_map(em);
		alloc_start = round_down(offset, sectorsize);
		alloc_end = alloc_start + sectorsize;
		goto reserve_space;
	}

	alloc_start = round_up(offset, sectorsize);
	alloc_end = round_down(offset + len, sectorsize);

	/*
	 * For unaligned ranges, check the pages at the boundaries, they might
	 * map to an extent, in which case we need to partially zero them, or
	 * they might map to a hole, in which case we need our allocation range
	 * to cover them.
	 */
	if (!IS_ALIGNED(offset, sectorsize)) {
		ret = btrfs_zero_range_check_range_boundary(BTRFS_I(inode),
							    offset);
		if (ret < 0)
			goto out;
		if (ret == RANGE_BOUNDARY_HOLE) {
			alloc_start = round_down(offset, sectorsize);
			ret = 0;
		} else if (ret == RANGE_BOUNDARY_WRITTEN_EXTENT) {
			ret = btrfs_truncate_block(BTRFS_I(inode), offset, 0, 0);
			if (ret)
				goto out;
		} else {
			ret = 0;
		}
	}

	if (!IS_ALIGNED(offset + len, sectorsize)) {
		ret = btrfs_zero_range_check_range_boundary(BTRFS_I(inode),
							    offset + len);
		if (ret < 0)
			goto out;
		if (ret == RANGE_BOUNDARY_HOLE) {
			alloc_end = round_up(offset + len, sectorsize);
			ret = 0;
		} else if (ret == RANGE_BOUNDARY_WRITTEN_EXTENT) {
			ret = btrfs_truncate_block(BTRFS_I(inode), offset + len,
						   0, 1);
			if (ret)
				goto out;
		} else {
			ret = 0;
		}
	}

reserve_space:
	if (alloc_start < alloc_end) {
		struct extent_state *cached_state = NULL;
		const u64 lockstart = alloc_start;
		const u64 lockend = alloc_end - 1;

		bytes_to_reserve = alloc_end - alloc_start;
		ret = btrfs_alloc_data_chunk_ondemand(BTRFS_I(inode),
						      bytes_to_reserve);
		if (ret < 0)
			goto out;
		space_reserved = true;
		ret = btrfs_punch_hole_lock_range(inode, lockstart, lockend,
						  &cached_state);
		if (ret)
			goto out;
		ret = btrfs_qgroup_reserve_data(BTRFS_I(inode), &data_reserved,
						alloc_start, bytes_to_reserve);
		if (ret) {
			unlock_extent_cached(&BTRFS_I(inode)->io_tree, lockstart,
					     lockend, &cached_state);
			goto out;
		}
		ret = btrfs_prealloc_file_range(inode, mode, alloc_start,
						alloc_end - alloc_start,
						i_blocksize(inode),
						offset + len, &alloc_hint);
		unlock_extent_cached(&BTRFS_I(inode)->io_tree, lockstart,
				     lockend, &cached_state);
		/* btrfs_prealloc_file_range releases reserved space on error */
		if (ret) {
			space_reserved = false;
			goto out;
		}
	}
	ret = btrfs_fallocate_update_isize(inode, offset + len, mode);
 out:
	if (ret && space_reserved)
		btrfs_free_reserved_data_space(BTRFS_I(inode), data_reserved,
					       alloc_start, bytes_to_reserve);
	extent_changeset_free(data_reserved);

	return ret;
}

static long btrfs_fallocate(struct file *file, int mode,
			    loff_t offset, loff_t len)
{
	struct inode *inode = file_inode(file);
	struct extent_state *cached_state = NULL;
	struct extent_changeset *data_reserved = NULL;
	struct falloc_range *range;
	struct falloc_range *tmp;
	struct list_head reserve_list;
	u64 cur_offset;
	u64 last_byte;
	u64 alloc_start;
	u64 alloc_end;
	u64 alloc_hint = 0;
	u64 locked_end;
	u64 actual_end = 0;
	struct extent_map *em;
	int blocksize = btrfs_inode_sectorsize(BTRFS_I(inode));
	int ret;

	/* Do not allow fallocate in ZONED mode */
	if (btrfs_is_zoned(btrfs_sb(inode->i_sb)))
		return -EOPNOTSUPP;

	alloc_start = round_down(offset, blocksize);
	alloc_end = round_up(offset + len, blocksize);
	cur_offset = alloc_start;

	/* Make sure we aren't being give some crap mode */
	if (mode & ~(FALLOC_FL_KEEP_SIZE | FALLOC_FL_PUNCH_HOLE |
		     FALLOC_FL_ZERO_RANGE))
		return -EOPNOTSUPP;

	if (mode & FALLOC_FL_PUNCH_HOLE)
		return btrfs_punch_hole(inode, offset, len);

	/*
	 * Only trigger disk allocation, don't trigger qgroup reserve
	 *
	 * For qgroup space, it will be checked later.
	 */
	if (!(mode & FALLOC_FL_ZERO_RANGE)) {
		ret = btrfs_alloc_data_chunk_ondemand(BTRFS_I(inode),
						      alloc_end - alloc_start);
		if (ret < 0)
			return ret;
	}

	btrfs_inode_lock(inode, BTRFS_ILOCK_MMAP);

	if (!(mode & FALLOC_FL_KEEP_SIZE) && offset + len > inode->i_size) {
		ret = inode_newsize_ok(inode, offset + len);
		if (ret)
			goto out;
	}

	/*
	 * TODO: Move these two operations after we have checked
	 * accurate reserved space, or fallocate can still fail but
	 * with page truncated or size expanded.
	 *
	 * But that's a minor problem and won't do much harm BTW.
	 */
	if (alloc_start > inode->i_size) {
		ret = btrfs_cont_expand(BTRFS_I(inode), i_size_read(inode),
					alloc_start);
		if (ret)
			goto out;
	} else if (offset + len > inode->i_size) {
		/*
		 * If we are fallocating from the end of the file onward we
		 * need to zero out the end of the block if i_size lands in the
		 * middle of a block.
		 */
		ret = btrfs_truncate_block(BTRFS_I(inode), inode->i_size, 0, 0);
		if (ret)
			goto out;
	}

	/*
	 * wait for ordered IO before we have any locks.  We'll loop again
	 * below with the locks held.
	 */
	ret = btrfs_wait_ordered_range(inode, alloc_start,
				       alloc_end - alloc_start);
	if (ret)
		goto out;

	if (mode & FALLOC_FL_ZERO_RANGE) {
		ret = btrfs_zero_range(inode, offset, len, mode);
		btrfs_inode_unlock(inode, BTRFS_ILOCK_MMAP);
		return ret;
	}

	locked_end = alloc_end - 1;
	while (1) {
		struct btrfs_ordered_extent *ordered;

		/* the extent lock is ordered inside the running
		 * transaction
		 */
		lock_extent_bits(&BTRFS_I(inode)->io_tree, alloc_start,
				 locked_end, &cached_state);
		ordered = btrfs_lookup_first_ordered_extent(BTRFS_I(inode),
							    locked_end);

		if (ordered &&
		    ordered->file_offset + ordered->num_bytes > alloc_start &&
		    ordered->file_offset < alloc_end) {
			btrfs_put_ordered_extent(ordered);
			unlock_extent_cached(&BTRFS_I(inode)->io_tree,
					     alloc_start, locked_end,
					     &cached_state);
			/*
			 * we can't wait on the range with the transaction
			 * running or with the extent lock held
			 */
			ret = btrfs_wait_ordered_range(inode, alloc_start,
						       alloc_end - alloc_start);
			if (ret)
				goto out;
		} else {
			if (ordered)
				btrfs_put_ordered_extent(ordered);
			break;
		}
	}

	/* First, check if we exceed the qgroup limit */
	INIT_LIST_HEAD(&reserve_list);
	while (cur_offset < alloc_end) {
		em = btrfs_get_extent(BTRFS_I(inode), NULL, 0, cur_offset,
				      alloc_end - cur_offset);
		if (IS_ERR(em)) {
			ret = PTR_ERR(em);
			break;
		}
		last_byte = min(extent_map_end(em), alloc_end);
		actual_end = min_t(u64, extent_map_end(em), offset + len);
		last_byte = ALIGN(last_byte, blocksize);
		if (em->block_start == EXTENT_MAP_HOLE ||
		    (cur_offset >= inode->i_size &&
		     !test_bit(EXTENT_FLAG_PREALLOC, &em->flags))) {
			ret = add_falloc_range(&reserve_list, cur_offset,
					       last_byte - cur_offset);
			if (ret < 0) {
				free_extent_map(em);
				break;
			}
			ret = btrfs_qgroup_reserve_data(BTRFS_I(inode),
					&data_reserved, cur_offset,
					last_byte - cur_offset);
			if (ret < 0) {
				cur_offset = last_byte;
				free_extent_map(em);
				break;
			}
		} else {
			/*
			 * Do not need to reserve unwritten extent for this
			 * range, free reserved data space first, otherwise
			 * it'll result in false ENOSPC error.
			 */
			btrfs_free_reserved_data_space(BTRFS_I(inode),
				data_reserved, cur_offset,
				last_byte - cur_offset);
		}
		free_extent_map(em);
		cur_offset = last_byte;
	}

	/*
	 * If ret is still 0, means we're OK to fallocate.
	 * Or just cleanup the list and exit.
	 */
	list_for_each_entry_safe(range, tmp, &reserve_list, list) {
		if (!ret)
			ret = btrfs_prealloc_file_range(inode, mode,
					range->start,
					range->len, i_blocksize(inode),
					offset + len, &alloc_hint);
		else
			btrfs_free_reserved_data_space(BTRFS_I(inode),
					data_reserved, range->start,
					range->len);
		list_del(&range->list);
		kfree(range);
	}
	if (ret < 0)
		goto out_unlock;

	/*
	 * We didn't need to allocate any more space, but we still extended the
	 * size of the file so we need to update i_size and the inode item.
	 */
	ret = btrfs_fallocate_update_isize(inode, actual_end, mode);
out_unlock:
	unlock_extent_cached(&BTRFS_I(inode)->io_tree, alloc_start, locked_end,
			     &cached_state);
out:
	btrfs_inode_unlock(inode, BTRFS_ILOCK_MMAP);
	/* Let go of our reservation. */
	if (ret != 0 && !(mode & FALLOC_FL_ZERO_RANGE))
		btrfs_free_reserved_data_space(BTRFS_I(inode), data_reserved,
				cur_offset, alloc_end - cur_offset);
	extent_changeset_free(data_reserved);
	return ret;
}

static loff_t find_desired_extent(struct btrfs_inode *inode, loff_t offset,
				  int whence)
{
	struct btrfs_fs_info *fs_info = inode->root->fs_info;
	struct extent_map *em = NULL;
	struct extent_state *cached_state = NULL;
	loff_t i_size = inode->vfs_inode.i_size;
	u64 lockstart;
	u64 lockend;
	u64 start;
	u64 len;
	int ret = 0;

	if (i_size == 0 || offset >= i_size)
		return -ENXIO;

	/*
	 * offset can be negative, in this case we start finding DATA/HOLE from
	 * the very start of the file.
	 */
	start = max_t(loff_t, 0, offset);

	lockstart = round_down(start, fs_info->sectorsize);
	lockend = round_up(i_size, fs_info->sectorsize);
	if (lockend <= lockstart)
		lockend = lockstart + fs_info->sectorsize;
	lockend--;
	len = lockend - lockstart + 1;

	lock_extent_bits(&inode->io_tree, lockstart, lockend, &cached_state);

	while (start < i_size) {
		em = btrfs_get_extent_fiemap(inode, start, len);
		if (IS_ERR(em)) {
			ret = PTR_ERR(em);
			em = NULL;
			break;
		}

		if (whence == SEEK_HOLE &&
		    (em->block_start == EXTENT_MAP_HOLE ||
		     test_bit(EXTENT_FLAG_PREALLOC, &em->flags)))
			break;
		else if (whence == SEEK_DATA &&
			   (em->block_start != EXTENT_MAP_HOLE &&
			    !test_bit(EXTENT_FLAG_PREALLOC, &em->flags)))
			break;

		start = em->start + em->len;
		free_extent_map(em);
		em = NULL;
		cond_resched();
	}
	free_extent_map(em);
	unlock_extent_cached(&inode->io_tree, lockstart, lockend,
			     &cached_state);
	if (ret) {
		offset = ret;
	} else {
		if (whence == SEEK_DATA && start >= i_size)
			offset = -ENXIO;
		else
			offset = min_t(loff_t, start, i_size);
	}

	return offset;
}

static loff_t btrfs_file_llseek(struct file *file, loff_t offset, int whence)
{
	struct inode *inode = file->f_mapping->host;

	switch (whence) {
	default:
		return generic_file_llseek(file, offset, whence);
	case SEEK_DATA:
	case SEEK_HOLE:
		btrfs_inode_lock(inode, BTRFS_ILOCK_SHARED);
		offset = find_desired_extent(BTRFS_I(inode), offset, whence);
		btrfs_inode_unlock(inode, BTRFS_ILOCK_SHARED);
		break;
	}

	if (offset < 0)
		return offset;

	return vfs_setpos(file, offset, inode->i_sb->s_maxbytes);
}

static int btrfs_file_open(struct inode *inode, struct file *filp)
{
	filp->f_mode |= FMODE_NOWAIT | FMODE_BUF_RASYNC;
	return generic_file_open(inode, filp);
}

static int check_direct_read(struct btrfs_fs_info *fs_info,
			     const struct iov_iter *iter, loff_t offset)
{
	int ret;
	int i, seg;

	ret = check_direct_IO(fs_info, iter, offset);
	if (ret < 0)
		return ret;

	if (!iter_is_iovec(iter))
		return 0;

	for (seg = 0; seg < iter->nr_segs; seg++)
		for (i = seg + 1; i < iter->nr_segs; i++)
			if (iter->iov[seg].iov_base == iter->iov[i].iov_base)
				return -EINVAL;
	return 0;
}

static ssize_t btrfs_direct_read(struct kiocb *iocb, struct iov_iter *to)
{
	struct inode *inode = file_inode(iocb->ki_filp);
	ssize_t ret;

	if (check_direct_read(btrfs_sb(inode->i_sb), to, iocb->ki_pos))
		return 0;

	btrfs_inode_lock(inode, BTRFS_ILOCK_SHARED);
	ret = iomap_dio_rw(iocb, to, &btrfs_dio_iomap_ops, &btrfs_dio_ops, 0);
	btrfs_inode_unlock(inode, BTRFS_ILOCK_SHARED);
	return ret;
}

static ssize_t btrfs_file_read_iter(struct kiocb *iocb, struct iov_iter *to)
{
	ssize_t ret = 0;

	if (iocb->ki_flags & IOCB_DIRECT) {
		ret = btrfs_direct_read(iocb, to);
		if (ret < 0 || !iov_iter_count(to) ||
		    iocb->ki_pos >= i_size_read(file_inode(iocb->ki_filp)))
			return ret;
	}

	return filemap_read(iocb, to, ret);
}

const struct file_operations btrfs_file_operations = {
	.llseek		= btrfs_file_llseek,
	.read_iter      = btrfs_file_read_iter,
	.splice_read	= generic_file_splice_read,
	.write_iter	= btrfs_file_write_iter,
	.splice_write	= iter_file_splice_write,
	.mmap		= btrfs_file_mmap,
	.open		= btrfs_file_open,
	.release	= btrfs_release_file,
	.fsync		= btrfs_sync_file,
	.fallocate	= btrfs_fallocate,
	.unlocked_ioctl	= btrfs_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl	= btrfs_compat_ioctl,
#endif
	.remap_file_range = btrfs_remap_file_range,
};

void __cold btrfs_auto_defrag_exit(void)
{
	kmem_cache_destroy(btrfs_inode_defrag_cachep);
}

int __init btrfs_auto_defrag_init(void)
{
	btrfs_inode_defrag_cachep = kmem_cache_create("btrfs_inode_defrag",
					sizeof(struct inode_defrag), 0,
					SLAB_MEM_SPREAD,
					NULL);
	if (!btrfs_inode_defrag_cachep)
		return -ENOMEM;

	return 0;
}

int btrfs_fdatawrite_range(struct inode *inode, loff_t start, loff_t end)
{
	int ret;

	/*
	 * So with compression we will find and lock a dirty page and clear the
	 * first one as dirty, setup an async extent, and immediately return
	 * with the entire range locked but with nobody actually marked with
	 * writeback.  So we can't just filemap_write_and_wait_range() and
	 * expect it to work since it will just kick off a thread to do the
	 * actual work.  So we need to call filemap_fdatawrite_range _again_
	 * since it will wait on the page lock, which won't be unlocked until
	 * after the pages have been marked as writeback and so we're good to go
	 * from there.  We have to do this otherwise we'll miss the ordered
	 * extents and that results in badness.  Please Josef, do not think you
	 * know better and pull this out at some point in the future, it is
	 * right and you are wrong.
	 */
	ret = filemap_fdatawrite_range(inode->i_mapping, start, end);
	if (!ret && test_bit(BTRFS_INODE_HAS_ASYNC_EXTENT,
			     &BTRFS_I(inode)->runtime_flags))
		ret = filemap_fdatawrite_range(inode->i_mapping, start, end);

	return ret;
}
