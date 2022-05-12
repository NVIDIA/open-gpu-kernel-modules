// SPDX-License-Identifier: GPL-2.0
/*
 * linux/fs/ext2/xattr.c
 *
 * Copyright (C) 2001-2003 Andreas Gruenbacher <agruen@suse.de>
 *
 * Fix by Harrison Xing <harrison@mountainviewdata.com>.
 * Extended attributes for symlinks and special files added per
 *  suggestion of Luka Renko <luka.renko@hermes.si>.
 * xattr consolidation Copyright (c) 2004 James Morris <jmorris@redhat.com>,
 *  Red Hat Inc.
 *
 */

/*
 * Extended attributes are stored on disk blocks allocated outside of
 * any inode. The i_file_acl field is then made to point to this allocated
 * block. If all extended attributes of an inode are identical, these
 * inodes may share the same extended attribute block. Such situations
 * are automatically detected by keeping a cache of recent attribute block
 * numbers and hashes over the block's contents in memory.
 *
 *
 * Extended attribute block layout:
 *
 *   +------------------+
 *   | header           |
 *   | entry 1          | |
 *   | entry 2          | | growing downwards
 *   | entry 3          | v
 *   | four null bytes  |
 *   | . . .            |
 *   | value 1          | ^
 *   | value 3          | | growing upwards
 *   | value 2          | |
 *   +------------------+
 *
 * The block header is followed by multiple entry descriptors. These entry
 * descriptors are variable in size, and aligned to EXT2_XATTR_PAD
 * byte boundaries. The entry descriptors are sorted by attribute name,
 * so that two extended attribute blocks can be compared efficiently.
 *
 * Attribute values are aligned to the end of the block, stored in
 * no specific order. They are also padded to EXT2_XATTR_PAD byte
 * boundaries. No additional gaps are left between them.
 *
 * Locking strategy
 * ----------------
 * EXT2_I(inode)->i_file_acl is protected by EXT2_I(inode)->xattr_sem.
 * EA blocks are only changed if they are exclusive to an inode, so
 * holding xattr_sem also means that nothing but the EA block's reference
 * count will change. Multiple writers to an EA block are synchronized
 * by the bh lock. No more than a single bh lock is held at any time
 * to avoid deadlocks.
 */

#include <linux/buffer_head.h>
#include <linux/init.h>
#include <linux/printk.h>
#include <linux/slab.h>
#include <linux/mbcache.h>
#include <linux/quotaops.h>
#include <linux/rwsem.h>
#include <linux/security.h>
#include "ext2.h"
#include "xattr.h"
#include "acl.h"

#define HDR(bh) ((struct ext2_xattr_header *)((bh)->b_data))
#define ENTRY(ptr) ((struct ext2_xattr_entry *)(ptr))
#define FIRST_ENTRY(bh) ENTRY(HDR(bh)+1)
#define IS_LAST_ENTRY(entry) (*(__u32 *)(entry) == 0)

#ifdef EXT2_XATTR_DEBUG
# define ea_idebug(inode, f...) do { \
		printk(KERN_DEBUG "inode %s:%ld: ", \
			inode->i_sb->s_id, inode->i_ino); \
		printk(f); \
		printk("\n"); \
	} while (0)
# define ea_bdebug(bh, f...) do { \
		printk(KERN_DEBUG "block %pg:%lu: ", \
			bh->b_bdev, (unsigned long) bh->b_blocknr); \
		printk(f); \
		printk("\n"); \
	} while (0)
#else
# define ea_idebug(inode, f...)	no_printk(f)
# define ea_bdebug(bh, f...)	no_printk(f)
#endif

static int ext2_xattr_set2(struct inode *, struct buffer_head *,
			   struct ext2_xattr_header *);

static int ext2_xattr_cache_insert(struct mb_cache *, struct buffer_head *);
static struct buffer_head *ext2_xattr_cache_find(struct inode *,
						 struct ext2_xattr_header *);
static void ext2_xattr_rehash(struct ext2_xattr_header *,
			      struct ext2_xattr_entry *);

static const struct xattr_handler *ext2_xattr_handler_map[] = {
	[EXT2_XATTR_INDEX_USER]		     = &ext2_xattr_user_handler,
#ifdef CONFIG_EXT2_FS_POSIX_ACL
	[EXT2_XATTR_INDEX_POSIX_ACL_ACCESS]  = &posix_acl_access_xattr_handler,
	[EXT2_XATTR_INDEX_POSIX_ACL_DEFAULT] = &posix_acl_default_xattr_handler,
#endif
	[EXT2_XATTR_INDEX_TRUSTED]	     = &ext2_xattr_trusted_handler,
#ifdef CONFIG_EXT2_FS_SECURITY
	[EXT2_XATTR_INDEX_SECURITY]	     = &ext2_xattr_security_handler,
#endif
};

const struct xattr_handler *ext2_xattr_handlers[] = {
	&ext2_xattr_user_handler,
	&ext2_xattr_trusted_handler,
#ifdef CONFIG_EXT2_FS_POSIX_ACL
	&posix_acl_access_xattr_handler,
	&posix_acl_default_xattr_handler,
#endif
#ifdef CONFIG_EXT2_FS_SECURITY
	&ext2_xattr_security_handler,
#endif
	NULL
};

#define EA_BLOCK_CACHE(inode)	(EXT2_SB(inode->i_sb)->s_ea_block_cache)

static inline const struct xattr_handler *
ext2_xattr_handler(int name_index)
{
	const struct xattr_handler *handler = NULL;

	if (name_index > 0 && name_index < ARRAY_SIZE(ext2_xattr_handler_map))
		handler = ext2_xattr_handler_map[name_index];
	return handler;
}

static bool
ext2_xattr_header_valid(struct ext2_xattr_header *header)
{
	if (header->h_magic != cpu_to_le32(EXT2_XATTR_MAGIC) ||
	    header->h_blocks != cpu_to_le32(1))
		return false;

	return true;
}

static bool
ext2_xattr_entry_valid(struct ext2_xattr_entry *entry,
		       char *end, size_t end_offs)
{
	struct ext2_xattr_entry *next;
	size_t size;

	next = EXT2_XATTR_NEXT(entry);
	if ((char *)next >= end)
		return false;

	if (entry->e_value_block != 0)
		return false;

	size = le32_to_cpu(entry->e_value_size);
	if (size > end_offs ||
	    le16_to_cpu(entry->e_value_offs) + size > end_offs)
		return false;

	return true;
}

static int
ext2_xattr_cmp_entry(int name_index, size_t name_len, const char *name,
		     struct ext2_xattr_entry *entry)
{
	int cmp;

	cmp = name_index - entry->e_name_index;
	if (!cmp)
		cmp = name_len - entry->e_name_len;
	if (!cmp)
		cmp = memcmp(name, entry->e_name, name_len);

	return cmp;
}

/*
 * ext2_xattr_get()
 *
 * Copy an extended attribute into the buffer
 * provided, or compute the buffer size required.
 * Buffer is NULL to compute the size of the buffer required.
 *
 * Returns a negative error number on failure, or the number of bytes
 * used / required on success.
 */
int
ext2_xattr_get(struct inode *inode, int name_index, const char *name,
	       void *buffer, size_t buffer_size)
{
	struct buffer_head *bh = NULL;
	struct ext2_xattr_entry *entry;
	size_t name_len, size;
	char *end;
	int error, not_found;
	struct mb_cache *ea_block_cache = EA_BLOCK_CACHE(inode);

	ea_idebug(inode, "name=%d.%s, buffer=%p, buffer_size=%ld",
		  name_index, name, buffer, (long)buffer_size);

	if (name == NULL)
		return -EINVAL;
	name_len = strlen(name);
	if (name_len > 255)
		return -ERANGE;

	down_read(&EXT2_I(inode)->xattr_sem);
	error = -ENODATA;
	if (!EXT2_I(inode)->i_file_acl)
		goto cleanup;
	ea_idebug(inode, "reading block %d", EXT2_I(inode)->i_file_acl);
	bh = sb_bread(inode->i_sb, EXT2_I(inode)->i_file_acl);
	error = -EIO;
	if (!bh)
		goto cleanup;
	ea_bdebug(bh, "b_count=%d, refcount=%d",
		atomic_read(&(bh->b_count)), le32_to_cpu(HDR(bh)->h_refcount));
	end = bh->b_data + bh->b_size;
	if (!ext2_xattr_header_valid(HDR(bh))) {
bad_block:
		ext2_error(inode->i_sb, "ext2_xattr_get",
			"inode %ld: bad block %d", inode->i_ino,
			EXT2_I(inode)->i_file_acl);
		error = -EIO;
		goto cleanup;
	}

	/* find named attribute */
	entry = FIRST_ENTRY(bh);
	while (!IS_LAST_ENTRY(entry)) {
		if (!ext2_xattr_entry_valid(entry, end,
		    inode->i_sb->s_blocksize))
			goto bad_block;

		not_found = ext2_xattr_cmp_entry(name_index, name_len, name,
						 entry);
		if (!not_found)
			goto found;
		if (not_found < 0)
			break;

		entry = EXT2_XATTR_NEXT(entry);
	}
	if (ext2_xattr_cache_insert(ea_block_cache, bh))
		ea_idebug(inode, "cache insert failed");
	error = -ENODATA;
	goto cleanup;
found:
	size = le32_to_cpu(entry->e_value_size);
	if (ext2_xattr_cache_insert(ea_block_cache, bh))
		ea_idebug(inode, "cache insert failed");
	if (buffer) {
		error = -ERANGE;
		if (size > buffer_size)
			goto cleanup;
		/* return value of attribute */
		memcpy(buffer, bh->b_data + le16_to_cpu(entry->e_value_offs),
			size);
	}
	error = size;

cleanup:
	brelse(bh);
	up_read(&EXT2_I(inode)->xattr_sem);

	return error;
}

/*
 * ext2_xattr_list()
 *
 * Copy a list of attribute names into the buffer
 * provided, or compute the buffer size required.
 * Buffer is NULL to compute the size of the buffer required.
 *
 * Returns a negative error number on failure, or the number of bytes
 * used / required on success.
 */
static int
ext2_xattr_list(struct dentry *dentry, char *buffer, size_t buffer_size)
{
	struct inode *inode = d_inode(dentry);
	struct buffer_head *bh = NULL;
	struct ext2_xattr_entry *entry;
	char *end;
	size_t rest = buffer_size;
	int error;
	struct mb_cache *ea_block_cache = EA_BLOCK_CACHE(inode);

	ea_idebug(inode, "buffer=%p, buffer_size=%ld",
		  buffer, (long)buffer_size);

	down_read(&EXT2_I(inode)->xattr_sem);
	error = 0;
	if (!EXT2_I(inode)->i_file_acl)
		goto cleanup;
	ea_idebug(inode, "reading block %d", EXT2_I(inode)->i_file_acl);
	bh = sb_bread(inode->i_sb, EXT2_I(inode)->i_file_acl);
	error = -EIO;
	if (!bh)
		goto cleanup;
	ea_bdebug(bh, "b_count=%d, refcount=%d",
		atomic_read(&(bh->b_count)), le32_to_cpu(HDR(bh)->h_refcount));
	end = bh->b_data + bh->b_size;
	if (!ext2_xattr_header_valid(HDR(bh))) {
bad_block:
		ext2_error(inode->i_sb, "ext2_xattr_list",
			"inode %ld: bad block %d", inode->i_ino,
			EXT2_I(inode)->i_file_acl);
		error = -EIO;
		goto cleanup;
	}

	/* check the on-disk data structure */
	entry = FIRST_ENTRY(bh);
	while (!IS_LAST_ENTRY(entry)) {
		if (!ext2_xattr_entry_valid(entry, end,
		    inode->i_sb->s_blocksize))
			goto bad_block;
		entry = EXT2_XATTR_NEXT(entry);
	}
	if (ext2_xattr_cache_insert(ea_block_cache, bh))
		ea_idebug(inode, "cache insert failed");

	/* list the attribute names */
	for (entry = FIRST_ENTRY(bh); !IS_LAST_ENTRY(entry);
	     entry = EXT2_XATTR_NEXT(entry)) {
		const struct xattr_handler *handler =
			ext2_xattr_handler(entry->e_name_index);

		if (handler && (!handler->list || handler->list(dentry))) {
			const char *prefix = handler->prefix ?: handler->name;
			size_t prefix_len = strlen(prefix);
			size_t size = prefix_len + entry->e_name_len + 1;

			if (buffer) {
				if (size > rest) {
					error = -ERANGE;
					goto cleanup;
				}
				memcpy(buffer, prefix, prefix_len);
				buffer += prefix_len;
				memcpy(buffer, entry->e_name, entry->e_name_len);
				buffer += entry->e_name_len;
				*buffer++ = 0;
			}
			rest -= size;
		}
	}
	error = buffer_size - rest;  /* total size */

cleanup:
	brelse(bh);
	up_read(&EXT2_I(inode)->xattr_sem);

	return error;
}

/*
 * Inode operation listxattr()
 *
 * d_inode(dentry)->i_mutex: don't care
 */
ssize_t
ext2_listxattr(struct dentry *dentry, char *buffer, size_t size)
{
	return ext2_xattr_list(dentry, buffer, size);
}

/*
 * If the EXT2_FEATURE_COMPAT_EXT_ATTR feature of this file system is
 * not set, set it.
 */
static void ext2_xattr_update_super_block(struct super_block *sb)
{
	if (EXT2_HAS_COMPAT_FEATURE(sb, EXT2_FEATURE_COMPAT_EXT_ATTR))
		return;

	spin_lock(&EXT2_SB(sb)->s_lock);
	ext2_update_dynamic_rev(sb);
	EXT2_SET_COMPAT_FEATURE(sb, EXT2_FEATURE_COMPAT_EXT_ATTR);
	spin_unlock(&EXT2_SB(sb)->s_lock);
	mark_buffer_dirty(EXT2_SB(sb)->s_sbh);
}

/*
 * ext2_xattr_set()
 *
 * Create, replace or remove an extended attribute for this inode.  Value
 * is NULL to remove an existing extended attribute, and non-NULL to
 * either replace an existing extended attribute, or create a new extended
 * attribute. The flags XATTR_REPLACE and XATTR_CREATE
 * specify that an extended attribute must exist and must not exist
 * previous to the call, respectively.
 *
 * Returns 0, or a negative error number on failure.
 */
int
ext2_xattr_set(struct inode *inode, int name_index, const char *name,
	       const void *value, size_t value_len, int flags)
{
	struct super_block *sb = inode->i_sb;
	struct buffer_head *bh = NULL;
	struct ext2_xattr_header *header = NULL;
	struct ext2_xattr_entry *here = NULL, *last = NULL;
	size_t name_len, free, min_offs = sb->s_blocksize;
	int not_found = 1, error;
	char *end;
	
	/*
	 * header -- Points either into bh, or to a temporarily
	 *           allocated buffer.
	 * here -- The named entry found, or the place for inserting, within
	 *         the block pointed to by header.
	 * last -- Points right after the last named entry within the block
	 *         pointed to by header.
	 * min_offs -- The offset of the first value (values are aligned
	 *             towards the end of the block).
	 * end -- Points right after the block pointed to by header.
	 */
	
	ea_idebug(inode, "name=%d.%s, value=%p, value_len=%ld",
		  name_index, name, value, (long)value_len);

	if (value == NULL)
		value_len = 0;
	if (name == NULL)
		return -EINVAL;
	name_len = strlen(name);
	if (name_len > 255 || value_len > sb->s_blocksize)
		return -ERANGE;
	error = dquot_initialize(inode);
	if (error)
		return error;
	down_write(&EXT2_I(inode)->xattr_sem);
	if (EXT2_I(inode)->i_file_acl) {
		/* The inode already has an extended attribute block. */
		bh = sb_bread(sb, EXT2_I(inode)->i_file_acl);
		error = -EIO;
		if (!bh)
			goto cleanup;
		ea_bdebug(bh, "b_count=%d, refcount=%d",
			atomic_read(&(bh->b_count)),
			le32_to_cpu(HDR(bh)->h_refcount));
		header = HDR(bh);
		end = bh->b_data + bh->b_size;
		if (!ext2_xattr_header_valid(header)) {
bad_block:
			ext2_error(sb, "ext2_xattr_set",
				"inode %ld: bad block %d", inode->i_ino, 
				   EXT2_I(inode)->i_file_acl);
			error = -EIO;
			goto cleanup;
		}
		/*
		 * Find the named attribute. If not found, 'here' will point
		 * to entry where the new attribute should be inserted to
		 * maintain sorting.
		 */
		last = FIRST_ENTRY(bh);
		while (!IS_LAST_ENTRY(last)) {
			if (!ext2_xattr_entry_valid(last, end, sb->s_blocksize))
				goto bad_block;
			if (last->e_value_size) {
				size_t offs = le16_to_cpu(last->e_value_offs);
				if (offs < min_offs)
					min_offs = offs;
			}
			if (not_found > 0) {
				not_found = ext2_xattr_cmp_entry(name_index,
								 name_len,
								 name, last);
				if (not_found <= 0)
					here = last;
			}
			last = EXT2_XATTR_NEXT(last);
		}
		if (not_found > 0)
			here = last;

		/* Check whether we have enough space left. */
		free = min_offs - ((char*)last - (char*)header) - sizeof(__u32);
	} else {
		/* We will use a new extended attribute block. */
		free = sb->s_blocksize -
			sizeof(struct ext2_xattr_header) - sizeof(__u32);
	}

	if (not_found) {
		/* Request to remove a nonexistent attribute? */
		error = -ENODATA;
		if (flags & XATTR_REPLACE)
			goto cleanup;
		error = 0;
		if (value == NULL)
			goto cleanup;
	} else {
		/* Request to create an existing attribute? */
		error = -EEXIST;
		if (flags & XATTR_CREATE)
			goto cleanup;
		free += EXT2_XATTR_SIZE(le32_to_cpu(here->e_value_size));
		free += EXT2_XATTR_LEN(name_len);
	}
	error = -ENOSPC;
	if (free < EXT2_XATTR_LEN(name_len) + EXT2_XATTR_SIZE(value_len))
		goto cleanup;

	/* Here we know that we can set the new attribute. */

	if (header) {
		/* assert(header == HDR(bh)); */
		lock_buffer(bh);
		if (header->h_refcount == cpu_to_le32(1)) {
			__u32 hash = le32_to_cpu(header->h_hash);

			ea_bdebug(bh, "modifying in-place");
			/*
			 * This must happen under buffer lock for
			 * ext2_xattr_set2() to reliably detect modified block
			 */
			mb_cache_entry_delete(EA_BLOCK_CACHE(inode), hash,
					      bh->b_blocknr);

			/* keep the buffer locked while modifying it. */
		} else {
			int offset;

			unlock_buffer(bh);
			ea_bdebug(bh, "cloning");
			header = kmemdup(HDR(bh), bh->b_size, GFP_KERNEL);
			error = -ENOMEM;
			if (header == NULL)
				goto cleanup;
			header->h_refcount = cpu_to_le32(1);

			offset = (char *)here - bh->b_data;
			here = ENTRY((char *)header + offset);
			offset = (char *)last - bh->b_data;
			last = ENTRY((char *)header + offset);
		}
	} else {
		/* Allocate a buffer where we construct the new block. */
		header = kzalloc(sb->s_blocksize, GFP_KERNEL);
		error = -ENOMEM;
		if (header == NULL)
			goto cleanup;
		end = (char *)header + sb->s_blocksize;
		header->h_magic = cpu_to_le32(EXT2_XATTR_MAGIC);
		header->h_blocks = header->h_refcount = cpu_to_le32(1);
		last = here = ENTRY(header+1);
	}

	/* Iff we are modifying the block in-place, bh is locked here. */

	if (not_found) {
		/* Insert the new name. */
		size_t size = EXT2_XATTR_LEN(name_len);
		size_t rest = (char *)last - (char *)here;
		memmove((char *)here + size, here, rest);
		memset(here, 0, size);
		here->e_name_index = name_index;
		here->e_name_len = name_len;
		memcpy(here->e_name, name, name_len);
	} else {
		if (here->e_value_size) {
			char *first_val = (char *)header + min_offs;
			size_t offs = le16_to_cpu(here->e_value_offs);
			char *val = (char *)header + offs;
			size_t size = EXT2_XATTR_SIZE(
				le32_to_cpu(here->e_value_size));

			if (size == EXT2_XATTR_SIZE(value_len)) {
				/* The old and the new value have the same
				   size. Just replace. */
				here->e_value_size = cpu_to_le32(value_len);
				memset(val + size - EXT2_XATTR_PAD, 0,
				       EXT2_XATTR_PAD); /* Clear pad bytes. */
				memcpy(val, value, value_len);
				goto skip_replace;
			}

			/* Remove the old value. */
			memmove(first_val + size, first_val, val - first_val);
			memset(first_val, 0, size);
			min_offs += size;

			/* Adjust all value offsets. */
			last = ENTRY(header+1);
			while (!IS_LAST_ENTRY(last)) {
				size_t o = le16_to_cpu(last->e_value_offs);
				if (o < offs)
					last->e_value_offs =
						cpu_to_le16(o + size);
				last = EXT2_XATTR_NEXT(last);
			}

			here->e_value_offs = 0;
		}
		if (value == NULL) {
			/* Remove the old name. */
			size_t size = EXT2_XATTR_LEN(name_len);
			last = ENTRY((char *)last - size);
			memmove(here, (char*)here + size,
				(char*)last - (char*)here);
			memset(last, 0, size);
		}
	}

	if (value != NULL) {
		/* Insert the new value. */
		here->e_value_size = cpu_to_le32(value_len);
		if (value_len) {
			size_t size = EXT2_XATTR_SIZE(value_len);
			char *val = (char *)header + min_offs - size;
			here->e_value_offs =
				cpu_to_le16((char *)val - (char *)header);
			memset(val + size - EXT2_XATTR_PAD, 0,
			       EXT2_XATTR_PAD); /* Clear the pad bytes. */
			memcpy(val, value, value_len);
		}
	}

skip_replace:
	if (IS_LAST_ENTRY(ENTRY(header+1))) {
		/* This block is now empty. */
		if (bh && header == HDR(bh))
			unlock_buffer(bh);  /* we were modifying in-place. */
		error = ext2_xattr_set2(inode, bh, NULL);
	} else {
		ext2_xattr_rehash(header, here);
		if (bh && header == HDR(bh))
			unlock_buffer(bh);  /* we were modifying in-place. */
		error = ext2_xattr_set2(inode, bh, header);
	}

cleanup:
	if (!(bh && header == HDR(bh)))
		kfree(header);
	brelse(bh);
	up_write(&EXT2_I(inode)->xattr_sem);

	return error;
}

/*
 * Second half of ext2_xattr_set(): Update the file system.
 */
static int
ext2_xattr_set2(struct inode *inode, struct buffer_head *old_bh,
		struct ext2_xattr_header *header)
{
	struct super_block *sb = inode->i_sb;
	struct buffer_head *new_bh = NULL;
	int error;
	struct mb_cache *ea_block_cache = EA_BLOCK_CACHE(inode);

	if (header) {
		new_bh = ext2_xattr_cache_find(inode, header);
		if (new_bh) {
			/* We found an identical block in the cache. */
			if (new_bh == old_bh) {
				ea_bdebug(new_bh, "keeping this block");
			} else {
				/* The old block is released after updating
				   the inode.  */
				ea_bdebug(new_bh, "reusing block");

				error = dquot_alloc_block(inode, 1);
				if (error) {
					unlock_buffer(new_bh);
					goto cleanup;
				}
				le32_add_cpu(&HDR(new_bh)->h_refcount, 1);
				ea_bdebug(new_bh, "refcount now=%d",
					le32_to_cpu(HDR(new_bh)->h_refcount));
			}
			unlock_buffer(new_bh);
		} else if (old_bh && header == HDR(old_bh)) {
			/* Keep this block. No need to lock the block as we
			   don't need to change the reference count. */
			new_bh = old_bh;
			get_bh(new_bh);
			ext2_xattr_cache_insert(ea_block_cache, new_bh);
		} else {
			/* We need to allocate a new block */
			ext2_fsblk_t goal = ext2_group_first_block_no(sb,
						EXT2_I(inode)->i_block_group);
			int block = ext2_new_block(inode, goal, &error);
			if (error)
				goto cleanup;
			ea_idebug(inode, "creating block %d", block);

			new_bh = sb_getblk(sb, block);
			if (unlikely(!new_bh)) {
				ext2_free_blocks(inode, block, 1);
				mark_inode_dirty(inode);
				error = -ENOMEM;
				goto cleanup;
			}
			lock_buffer(new_bh);
			memcpy(new_bh->b_data, header, new_bh->b_size);
			set_buffer_uptodate(new_bh);
			unlock_buffer(new_bh);
			ext2_xattr_cache_insert(ea_block_cache, new_bh);
			
			ext2_xattr_update_super_block(sb);
		}
		mark_buffer_dirty(new_bh);
		if (IS_SYNC(inode)) {
			sync_dirty_buffer(new_bh);
			error = -EIO;
			if (buffer_req(new_bh) && !buffer_uptodate(new_bh))
				goto cleanup;
		}
	}

	/* Update the inode. */
	EXT2_I(inode)->i_file_acl = new_bh ? new_bh->b_blocknr : 0;
	inode->i_ctime = current_time(inode);
	if (IS_SYNC(inode)) {
		error = sync_inode_metadata(inode, 1);
		/* In case sync failed due to ENOSPC the inode was actually
		 * written (only some dirty data were not) so we just proceed
		 * as if nothing happened and cleanup the unused block */
		if (error && error != -ENOSPC) {
			if (new_bh && new_bh != old_bh) {
				dquot_free_block_nodirty(inode, 1);
				mark_inode_dirty(inode);
			}
			goto cleanup;
		}
	} else
		mark_inode_dirty(inode);

	error = 0;
	if (old_bh && old_bh != new_bh) {
		/*
		 * If there was an old block and we are no longer using it,
		 * release the old block.
		 */
		lock_buffer(old_bh);
		if (HDR(old_bh)->h_refcount == cpu_to_le32(1)) {
			__u32 hash = le32_to_cpu(HDR(old_bh)->h_hash);

			/*
			 * This must happen under buffer lock for
			 * ext2_xattr_set2() to reliably detect freed block
			 */
			mb_cache_entry_delete(ea_block_cache, hash,
					      old_bh->b_blocknr);
			/* Free the old block. */
			ea_bdebug(old_bh, "freeing");
			ext2_free_blocks(inode, old_bh->b_blocknr, 1);
			mark_inode_dirty(inode);
			/* We let our caller release old_bh, so we
			 * need to duplicate the buffer before. */
			get_bh(old_bh);
			bforget(old_bh);
		} else {
			/* Decrement the refcount only. */
			le32_add_cpu(&HDR(old_bh)->h_refcount, -1);
			dquot_free_block_nodirty(inode, 1);
			mark_inode_dirty(inode);
			mark_buffer_dirty(old_bh);
			ea_bdebug(old_bh, "refcount now=%d",
				le32_to_cpu(HDR(old_bh)->h_refcount));
		}
		unlock_buffer(old_bh);
	}

cleanup:
	brelse(new_bh);

	return error;
}

/*
 * ext2_xattr_delete_inode()
 *
 * Free extended attribute resources associated with this inode. This
 * is called immediately before an inode is freed.
 */
void
ext2_xattr_delete_inode(struct inode *inode)
{
	struct buffer_head *bh = NULL;
	struct ext2_sb_info *sbi = EXT2_SB(inode->i_sb);

	/*
	 * We are the only ones holding inode reference. The xattr_sem should
	 * better be unlocked! We could as well just not acquire xattr_sem at
	 * all but this makes the code more futureproof. OTOH we need trylock
	 * here to avoid false-positive warning from lockdep about reclaim
	 * circular dependency.
	 */
	if (WARN_ON_ONCE(!down_write_trylock(&EXT2_I(inode)->xattr_sem)))
		return;
	if (!EXT2_I(inode)->i_file_acl)
		goto cleanup;

	if (!ext2_data_block_valid(sbi, EXT2_I(inode)->i_file_acl, 1)) {
		ext2_error(inode->i_sb, "ext2_xattr_delete_inode",
			"inode %ld: xattr block %d is out of data blocks range",
			inode->i_ino, EXT2_I(inode)->i_file_acl);
		goto cleanup;
	}

	bh = sb_bread(inode->i_sb, EXT2_I(inode)->i_file_acl);
	if (!bh) {
		ext2_error(inode->i_sb, "ext2_xattr_delete_inode",
			"inode %ld: block %d read error", inode->i_ino,
			EXT2_I(inode)->i_file_acl);
		goto cleanup;
	}
	ea_bdebug(bh, "b_count=%d", atomic_read(&(bh->b_count)));
	if (!ext2_xattr_header_valid(HDR(bh))) {
		ext2_error(inode->i_sb, "ext2_xattr_delete_inode",
			"inode %ld: bad block %d", inode->i_ino,
			EXT2_I(inode)->i_file_acl);
		goto cleanup;
	}
	lock_buffer(bh);
	if (HDR(bh)->h_refcount == cpu_to_le32(1)) {
		__u32 hash = le32_to_cpu(HDR(bh)->h_hash);

		/*
		 * This must happen under buffer lock for ext2_xattr_set2() to
		 * reliably detect freed block
		 */
		mb_cache_entry_delete(EA_BLOCK_CACHE(inode), hash,
				      bh->b_blocknr);
		ext2_free_blocks(inode, EXT2_I(inode)->i_file_acl, 1);
		get_bh(bh);
		bforget(bh);
		unlock_buffer(bh);
	} else {
		le32_add_cpu(&HDR(bh)->h_refcount, -1);
		ea_bdebug(bh, "refcount now=%d",
			le32_to_cpu(HDR(bh)->h_refcount));
		unlock_buffer(bh);
		mark_buffer_dirty(bh);
		if (IS_SYNC(inode))
			sync_dirty_buffer(bh);
		dquot_free_block_nodirty(inode, 1);
	}
	EXT2_I(inode)->i_file_acl = 0;

cleanup:
	brelse(bh);
	up_write(&EXT2_I(inode)->xattr_sem);
}

/*
 * ext2_xattr_cache_insert()
 *
 * Create a new entry in the extended attribute cache, and insert
 * it unless such an entry is already in the cache.
 *
 * Returns 0, or a negative error number on failure.
 */
static int
ext2_xattr_cache_insert(struct mb_cache *cache, struct buffer_head *bh)
{
	__u32 hash = le32_to_cpu(HDR(bh)->h_hash);
	int error;

	error = mb_cache_entry_create(cache, GFP_NOFS, hash, bh->b_blocknr,
				      true);
	if (error) {
		if (error == -EBUSY) {
			ea_bdebug(bh, "already in cache");
			error = 0;
		}
	} else
		ea_bdebug(bh, "inserting [%x]", (int)hash);
	return error;
}

/*
 * ext2_xattr_cmp()
 *
 * Compare two extended attribute blocks for equality.
 *
 * Returns 0 if the blocks are equal, 1 if they differ, and
 * a negative error number on errors.
 */
static int
ext2_xattr_cmp(struct ext2_xattr_header *header1,
	       struct ext2_xattr_header *header2)
{
	struct ext2_xattr_entry *entry1, *entry2;

	entry1 = ENTRY(header1+1);
	entry2 = ENTRY(header2+1);
	while (!IS_LAST_ENTRY(entry1)) {
		if (IS_LAST_ENTRY(entry2))
			return 1;
		if (entry1->e_hash != entry2->e_hash ||
		    entry1->e_name_index != entry2->e_name_index ||
		    entry1->e_name_len != entry2->e_name_len ||
		    entry1->e_value_size != entry2->e_value_size ||
		    memcmp(entry1->e_name, entry2->e_name, entry1->e_name_len))
			return 1;
		if (entry1->e_value_block != 0 || entry2->e_value_block != 0)
			return -EIO;
		if (memcmp((char *)header1 + le16_to_cpu(entry1->e_value_offs),
			   (char *)header2 + le16_to_cpu(entry2->e_value_offs),
			   le32_to_cpu(entry1->e_value_size)))
			return 1;

		entry1 = EXT2_XATTR_NEXT(entry1);
		entry2 = EXT2_XATTR_NEXT(entry2);
	}
	if (!IS_LAST_ENTRY(entry2))
		return 1;
	return 0;
}

/*
 * ext2_xattr_cache_find()
 *
 * Find an identical extended attribute block.
 *
 * Returns a locked buffer head to the block found, or NULL if such
 * a block was not found or an error occurred.
 */
static struct buffer_head *
ext2_xattr_cache_find(struct inode *inode, struct ext2_xattr_header *header)
{
	__u32 hash = le32_to_cpu(header->h_hash);
	struct mb_cache_entry *ce;
	struct mb_cache *ea_block_cache = EA_BLOCK_CACHE(inode);

	if (!header->h_hash)
		return NULL;  /* never share */
	ea_idebug(inode, "looking for cached blocks [%x]", (int)hash);
again:
	ce = mb_cache_entry_find_first(ea_block_cache, hash);
	while (ce) {
		struct buffer_head *bh;

		bh = sb_bread(inode->i_sb, ce->e_value);
		if (!bh) {
			ext2_error(inode->i_sb, "ext2_xattr_cache_find",
				"inode %ld: block %ld read error",
				inode->i_ino, (unsigned long) ce->e_value);
		} else {
			lock_buffer(bh);
			/*
			 * We have to be careful about races with freeing or
			 * rehashing of xattr block. Once we hold buffer lock
			 * xattr block's state is stable so we can check
			 * whether the block got freed / rehashed or not.
			 * Since we unhash mbcache entry under buffer lock when
			 * freeing / rehashing xattr block, checking whether
			 * entry is still hashed is reliable.
			 */
			if (hlist_bl_unhashed(&ce->e_hash_list)) {
				mb_cache_entry_put(ea_block_cache, ce);
				unlock_buffer(bh);
				brelse(bh);
				goto again;
			} else if (le32_to_cpu(HDR(bh)->h_refcount) >
				   EXT2_XATTR_REFCOUNT_MAX) {
				ea_idebug(inode, "block %ld refcount %d>%d",
					  (unsigned long) ce->e_value,
					  le32_to_cpu(HDR(bh)->h_refcount),
					  EXT2_XATTR_REFCOUNT_MAX);
			} else if (!ext2_xattr_cmp(header, HDR(bh))) {
				ea_bdebug(bh, "b_count=%d",
					  atomic_read(&(bh->b_count)));
				mb_cache_entry_touch(ea_block_cache, ce);
				mb_cache_entry_put(ea_block_cache, ce);
				return bh;
			}
			unlock_buffer(bh);
			brelse(bh);
		}
		ce = mb_cache_entry_find_next(ea_block_cache, ce);
	}
	return NULL;
}

#define NAME_HASH_SHIFT 5
#define VALUE_HASH_SHIFT 16

/*
 * ext2_xattr_hash_entry()
 *
 * Compute the hash of an extended attribute.
 */
static inline void ext2_xattr_hash_entry(struct ext2_xattr_header *header,
					 struct ext2_xattr_entry *entry)
{
	__u32 hash = 0;
	char *name = entry->e_name;
	int n;

	for (n=0; n < entry->e_name_len; n++) {
		hash = (hash << NAME_HASH_SHIFT) ^
		       (hash >> (8*sizeof(hash) - NAME_HASH_SHIFT)) ^
		       *name++;
	}

	if (entry->e_value_block == 0 && entry->e_value_size != 0) {
		__le32 *value = (__le32 *)((char *)header +
			le16_to_cpu(entry->e_value_offs));
		for (n = (le32_to_cpu(entry->e_value_size) +
		     EXT2_XATTR_ROUND) >> EXT2_XATTR_PAD_BITS; n; n--) {
			hash = (hash << VALUE_HASH_SHIFT) ^
			       (hash >> (8*sizeof(hash) - VALUE_HASH_SHIFT)) ^
			       le32_to_cpu(*value++);
		}
	}
	entry->e_hash = cpu_to_le32(hash);
}

#undef NAME_HASH_SHIFT
#undef VALUE_HASH_SHIFT

#define BLOCK_HASH_SHIFT 16

/*
 * ext2_xattr_rehash()
 *
 * Re-compute the extended attribute hash value after an entry has changed.
 */
static void ext2_xattr_rehash(struct ext2_xattr_header *header,
			      struct ext2_xattr_entry *entry)
{
	struct ext2_xattr_entry *here;
	__u32 hash = 0;
	
	ext2_xattr_hash_entry(header, entry);
	here = ENTRY(header+1);
	while (!IS_LAST_ENTRY(here)) {
		if (!here->e_hash) {
			/* Block is not shared if an entry's hash value == 0 */
			hash = 0;
			break;
		}
		hash = (hash << BLOCK_HASH_SHIFT) ^
		       (hash >> (8*sizeof(hash) - BLOCK_HASH_SHIFT)) ^
		       le32_to_cpu(here->e_hash);
		here = EXT2_XATTR_NEXT(here);
	}
	header->h_hash = cpu_to_le32(hash);
}

#undef BLOCK_HASH_SHIFT

#define HASH_BUCKET_BITS 10

struct mb_cache *ext2_xattr_create_cache(void)
{
	return mb_cache_create(HASH_BUCKET_BITS);
}

void ext2_xattr_destroy_cache(struct mb_cache *cache)
{
	if (cache)
		mb_cache_destroy(cache);
}
