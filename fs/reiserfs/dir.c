/*
 * Copyright 2000 by Hans Reiser, licensing governed by reiserfs/README
 */

#include <linux/string.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include "reiserfs.h"
#include <linux/stat.h>
#include <linux/buffer_head.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

extern const struct reiserfs_key MIN_KEY;

static int reiserfs_readdir(struct file *, struct dir_context *);
static int reiserfs_dir_fsync(struct file *filp, loff_t start, loff_t end,
			      int datasync);

const struct file_operations reiserfs_dir_operations = {
	.llseek = generic_file_llseek,
	.read = generic_read_dir,
	.iterate_shared = reiserfs_readdir,
	.fsync = reiserfs_dir_fsync,
	.unlocked_ioctl = reiserfs_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl = reiserfs_compat_ioctl,
#endif
};

static int reiserfs_dir_fsync(struct file *filp, loff_t start, loff_t end,
			      int datasync)
{
	struct inode *inode = filp->f_mapping->host;
	int err;

	err = file_write_and_wait_range(filp, start, end);
	if (err)
		return err;

	inode_lock(inode);
	reiserfs_write_lock(inode->i_sb);
	err = reiserfs_commit_for_inode(inode);
	reiserfs_write_unlock(inode->i_sb);
	inode_unlock(inode);
	if (err < 0)
		return err;
	return 0;
}

#define store_ih(where,what) copy_item_head (where, what)

static inline bool is_privroot_deh(struct inode *dir, struct reiserfs_de_head *deh)
{
	struct dentry *privroot = REISERFS_SB(dir->i_sb)->priv_root;
	return (d_really_is_positive(privroot) &&
	        deh->deh_objectid == INODE_PKEY(d_inode(privroot))->k_objectid);
}

int reiserfs_readdir_inode(struct inode *inode, struct dir_context *ctx)
{

	/* key of current position in the directory (key of directory entry) */
	struct cpu_key pos_key;

	INITIALIZE_PATH(path_to_entry);
	struct buffer_head *bh;
	int item_num, entry_num;
	const struct reiserfs_key *rkey;
	struct item_head *ih, tmp_ih;
	int search_res;
	char *local_buf;
	loff_t next_pos;
	char small_buf[32];	/* avoid kmalloc if we can */
	struct reiserfs_dir_entry de;
	int ret = 0;
	int depth;

	reiserfs_write_lock(inode->i_sb);

	reiserfs_check_lock_depth(inode->i_sb, "readdir");

	/*
	 * form key for search the next directory entry using
	 * f_pos field of file structure
	 */
	make_cpu_key(&pos_key, inode, ctx->pos ?: DOT_OFFSET, TYPE_DIRENTRY, 3);
	next_pos = cpu_key_k_offset(&pos_key);

	path_to_entry.reada = PATH_READA;
	while (1) {
research:
		/*
		 * search the directory item, containing entry with
		 * specified key
		 */
		search_res =
		    search_by_entry_key(inode->i_sb, &pos_key, &path_to_entry,
					&de);
		if (search_res == IO_ERROR) {
			/*
			 * FIXME: we could just skip part of directory
			 * which could not be read
			 */
			ret = -EIO;
			goto out;
		}
		entry_num = de.de_entry_num;
		bh = de.de_bh;
		item_num = de.de_item_num;
		ih = de.de_ih;
		store_ih(&tmp_ih, ih);

		/* we must have found item, that is item of this directory, */
		RFALSE(COMP_SHORT_KEYS(&ih->ih_key, &pos_key),
		       "vs-9000: found item %h does not match to dir we readdir %K",
		       ih, &pos_key);
		RFALSE(item_num > B_NR_ITEMS(bh) - 1,
		       "vs-9005 item_num == %d, item amount == %d",
		       item_num, B_NR_ITEMS(bh));

		/*
		 * and entry must be not more than number of entries
		 * in the item
		 */
		RFALSE(ih_entry_count(ih) < entry_num,
		       "vs-9010: entry number is too big %d (%d)",
		       entry_num, ih_entry_count(ih));

		/*
		 * go through all entries in the directory item beginning
		 * from the entry, that has been found
		 */
		if (search_res == POSITION_FOUND
		    || entry_num < ih_entry_count(ih)) {
			struct reiserfs_de_head *deh =
			    B_I_DEH(bh, ih) + entry_num;

			for (; entry_num < ih_entry_count(ih);
			     entry_num++, deh++) {
				int d_reclen;
				char *d_name;
				ino_t d_ino;
				loff_t cur_pos = deh_offset(deh);

				/* it is hidden entry */
				if (!de_visible(deh))
					continue;
				d_reclen = entry_length(bh, ih, entry_num);
				d_name = B_I_DEH_ENTRY_FILE_NAME(bh, ih, deh);

				if (d_reclen <= 0 ||
				    d_name + d_reclen > bh->b_data + bh->b_size) {
					/*
					 * There is corrupted data in entry,
					 * We'd better stop here
					 */
					pathrelse(&path_to_entry);
					ret = -EIO;
					goto out;
				}

				if (!d_name[d_reclen - 1])
					d_reclen = strlen(d_name);

				/* too big to send back to VFS */
				if (d_reclen >
				    REISERFS_MAX_NAME(inode->i_sb->
						      s_blocksize)) {
					continue;
				}

				/* Ignore the .reiserfs_priv entry */
				if (is_privroot_deh(inode, deh))
					continue;

				ctx->pos = deh_offset(deh);
				d_ino = deh_objectid(deh);
				if (d_reclen <= 32) {
					local_buf = small_buf;
				} else {
					local_buf = kmalloc(d_reclen,
							    GFP_NOFS);
					if (!local_buf) {
						pathrelse(&path_to_entry);
						ret = -ENOMEM;
						goto out;
					}
					if (item_moved(&tmp_ih, &path_to_entry)) {
						kfree(local_buf);
						goto research;
					}
				}

				/*
				 * Note, that we copy name to user space via
				 * temporary buffer (local_buf) because
				 * filldir will block if user space buffer is
				 * swapped out. At that time entry can move to
				 * somewhere else
				 */
				memcpy(local_buf, d_name, d_reclen);

				/*
				 * Since filldir might sleep, we can release
				 * the write lock here for other waiters
				 */
				depth = reiserfs_write_unlock_nested(inode->i_sb);
				if (!dir_emit
				    (ctx, local_buf, d_reclen, d_ino,
				     DT_UNKNOWN)) {
					reiserfs_write_lock_nested(inode->i_sb, depth);
					if (local_buf != small_buf) {
						kfree(local_buf);
					}
					goto end;
				}
				reiserfs_write_lock_nested(inode->i_sb, depth);
				if (local_buf != small_buf) {
					kfree(local_buf);
				}

				/* deh_offset(deh) may be invalid now. */
				next_pos = cur_pos + 1;

				if (item_moved(&tmp_ih, &path_to_entry)) {
					set_cpu_key_k_offset(&pos_key,
							     next_pos);
					goto research;
				}
			}	/* for */
		}

		/* end of directory has been reached */
		if (item_num != B_NR_ITEMS(bh) - 1)
			goto end;

		/*
		 * item we went through is last item of node. Using right
		 * delimiting key check is it directory end
		 */
		rkey = get_rkey(&path_to_entry, inode->i_sb);
		if (!comp_le_keys(rkey, &MIN_KEY)) {
			/*
			 * set pos_key to key, that is the smallest and greater
			 * that key of the last entry in the item
			 */
			set_cpu_key_k_offset(&pos_key, next_pos);
			continue;
		}

		/* end of directory has been reached */
		if (COMP_SHORT_KEYS(rkey, &pos_key)) {
			goto end;
		}

		/* directory continues in the right neighboring block */
		set_cpu_key_k_offset(&pos_key,
				     le_key_k_offset(KEY_FORMAT_3_5, rkey));

	}			/* while */

end:
	ctx->pos = next_pos;
	pathrelse(&path_to_entry);
	reiserfs_check_path(&path_to_entry);
out:
	reiserfs_write_unlock(inode->i_sb);
	return ret;
}

static int reiserfs_readdir(struct file *file, struct dir_context *ctx)
{
	return reiserfs_readdir_inode(file_inode(file), ctx);
}

/*
 * compose directory item containing "." and ".." entries (entries are
 * not aligned to 4 byte boundary)
 */
void make_empty_dir_item_v1(char *body, __le32 dirid, __le32 objid,
			    __le32 par_dirid, __le32 par_objid)
{
	struct reiserfs_de_head *dot, *dotdot;

	memset(body, 0, EMPTY_DIR_SIZE_V1);
	dot = (struct reiserfs_de_head *)body;
	dotdot = dot + 1;

	/* direntry header of "." */
	put_deh_offset(dot, DOT_OFFSET);
	/* these two are from make_le_item_head, and are LE */
	dot->deh_dir_id = dirid;
	dot->deh_objectid = objid;
	dot->deh_state = 0;	/* Endian safe if 0 */
	put_deh_location(dot, EMPTY_DIR_SIZE_V1 - strlen("."));
	mark_de_visible(dot);

	/* direntry header of ".." */
	put_deh_offset(dotdot, DOT_DOT_OFFSET);
	/* key of ".." for the root directory */
	/* these two are from the inode, and are LE */
	dotdot->deh_dir_id = par_dirid;
	dotdot->deh_objectid = par_objid;
	dotdot->deh_state = 0;	/* Endian safe if 0 */
	put_deh_location(dotdot, deh_location(dot) - strlen(".."));
	mark_de_visible(dotdot);

	/* copy ".." and "." */
	memcpy(body + deh_location(dot), ".", 1);
	memcpy(body + deh_location(dotdot), "..", 2);
}

/* compose directory item containing "." and ".." entries */
void make_empty_dir_item(char *body, __le32 dirid, __le32 objid,
			 __le32 par_dirid, __le32 par_objid)
{
	struct reiserfs_de_head *dot, *dotdot;

	memset(body, 0, EMPTY_DIR_SIZE);
	dot = (struct reiserfs_de_head *)body;
	dotdot = dot + 1;

	/* direntry header of "." */
	put_deh_offset(dot, DOT_OFFSET);
	/* these two are from make_le_item_head, and are LE */
	dot->deh_dir_id = dirid;
	dot->deh_objectid = objid;
	dot->deh_state = 0;	/* Endian safe if 0 */
	put_deh_location(dot, EMPTY_DIR_SIZE - ROUND_UP(strlen(".")));
	mark_de_visible(dot);

	/* direntry header of ".." */
	put_deh_offset(dotdot, DOT_DOT_OFFSET);
	/* key of ".." for the root directory */
	/* these two are from the inode, and are LE */
	dotdot->deh_dir_id = par_dirid;
	dotdot->deh_objectid = par_objid;
	dotdot->deh_state = 0;	/* Endian safe if 0 */
	put_deh_location(dotdot, deh_location(dot) - ROUND_UP(strlen("..")));
	mark_de_visible(dotdot);

	/* copy ".." and "." */
	memcpy(body + deh_location(dot), ".", 1);
	memcpy(body + deh_location(dotdot), "..", 2);
}
