/*
 * Copyright 2000 by Hans Reiser, licensing governed by reiserfs/README
 */

#include <linux/capability.h>
#include <linux/fs.h>
#include <linux/mount.h>
#include "reiserfs.h"
#include <linux/time.h>
#include <linux/uaccess.h>
#include <linux/pagemap.h>
#include <linux/compat.h>
#include <linux/fileattr.h>

int reiserfs_fileattr_get(struct dentry *dentry, struct fileattr *fa)
{
	struct inode *inode = d_inode(dentry);

	if (!reiserfs_attrs(inode->i_sb))
		return -ENOTTY;

	fileattr_fill_flags(fa, REISERFS_I(inode)->i_attrs);

	return 0;
}

int reiserfs_fileattr_set(struct user_namespace *mnt_userns,
			  struct dentry *dentry, struct fileattr *fa)
{
	struct inode *inode = d_inode(dentry);
	unsigned int flags = fa->flags;
	int err;

	reiserfs_write_lock(inode->i_sb);

	err = -ENOTTY;
	if (!reiserfs_attrs(inode->i_sb))
		goto unlock;

	err = -EOPNOTSUPP;
	if (fileattr_has_fsx(fa))
		goto unlock;

	/*
	 * Is it quota file? Do not allow user to mess with it
	 */
	err = -EPERM;
	if (IS_NOQUOTA(inode))
		goto unlock;

	if ((flags & REISERFS_NOTAIL_FL) && S_ISREG(inode->i_mode)) {
		err = reiserfs_unpack(inode);
		if (err)
			goto unlock;
	}
	sd_attrs_to_i_attrs(flags, inode);
	REISERFS_I(inode)->i_attrs = flags;
	inode->i_ctime = current_time(inode);
	mark_inode_dirty(inode);
	err = 0;
unlock:
	reiserfs_write_unlock(inode->i_sb);

	return err;
}

/*
 * reiserfs_ioctl - handler for ioctl for inode
 * supported commands:
 *  1) REISERFS_IOC_UNPACK - try to unpack tail from direct item into indirect
 *                           and prevent packing file (argument arg has t
 *			      be non-zero)
 *  2) REISERFS_IOC_[GS]ETFLAGS, REISERFS_IOC_[GS]ETVERSION
 *  3) That's all for a while ...
 */
long reiserfs_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	struct inode *inode = file_inode(filp);
	int err = 0;

	reiserfs_write_lock(inode->i_sb);

	switch (cmd) {
	case REISERFS_IOC_UNPACK:
		if (S_ISREG(inode->i_mode)) {
			if (arg)
				err = reiserfs_unpack(inode);
		} else
			err = -ENOTTY;
		break;
		/*
		 * following two cases are taken from fs/ext2/ioctl.c by Remy
		 * Card (card@masi.ibp.fr)
		 */
	case REISERFS_IOC_GETVERSION:
		err = put_user(inode->i_generation, (int __user *)arg);
		break;
	case REISERFS_IOC_SETVERSION:
		if (!inode_owner_or_capable(&init_user_ns, inode)) {
			err = -EPERM;
			break;
		}
		err = mnt_want_write_file(filp);
		if (err)
			break;
		if (get_user(inode->i_generation, (int __user *)arg)) {
			err = -EFAULT;
			goto setversion_out;
		}
		inode->i_ctime = current_time(inode);
		mark_inode_dirty(inode);
setversion_out:
		mnt_drop_write_file(filp);
		break;
	default:
		err = -ENOTTY;
	}

	reiserfs_write_unlock(inode->i_sb);

	return err;
}

#ifdef CONFIG_COMPAT
long reiserfs_compat_ioctl(struct file *file, unsigned int cmd,
				unsigned long arg)
{
	/*
	 * These are just misnamed, they actually
	 * get/put from/to user an int
	 */
	switch (cmd) {
	case REISERFS_IOC32_UNPACK:
		cmd = REISERFS_IOC_UNPACK;
		break;
	case REISERFS_IOC32_GETVERSION:
		cmd = REISERFS_IOC_GETVERSION;
		break;
	case REISERFS_IOC32_SETVERSION:
		cmd = REISERFS_IOC_SETVERSION;
		break;
	default:
		return -ENOIOCTLCMD;
	}

	return reiserfs_ioctl(file, cmd, (unsigned long) compat_ptr(arg));
}
#endif

int reiserfs_commit_write(struct file *f, struct page *page,
			  unsigned from, unsigned to);
/*
 * reiserfs_unpack
 * Function try to convert tail from direct item into indirect.
 * It set up nopack attribute in the REISERFS_I(inode)->nopack
 */
int reiserfs_unpack(struct inode *inode)
{
	int retval = 0;
	int index;
	struct page *page;
	struct address_space *mapping;
	unsigned long write_from;
	unsigned long blocksize = inode->i_sb->s_blocksize;

	if (inode->i_size == 0) {
		REISERFS_I(inode)->i_flags |= i_nopack_mask;
		return 0;
	}
	/* ioctl already done */
	if (REISERFS_I(inode)->i_flags & i_nopack_mask) {
		return 0;
	}

	/* we need to make sure nobody is changing the file size beneath us */
	{
		int depth = reiserfs_write_unlock_nested(inode->i_sb);

		inode_lock(inode);
		reiserfs_write_lock_nested(inode->i_sb, depth);
	}

	reiserfs_write_lock(inode->i_sb);

	write_from = inode->i_size & (blocksize - 1);
	/* if we are on a block boundary, we are already unpacked.  */
	if (write_from == 0) {
		REISERFS_I(inode)->i_flags |= i_nopack_mask;
		goto out;
	}

	/*
	 * we unpack by finding the page with the tail, and calling
	 * __reiserfs_write_begin on that page.  This will force a
	 * reiserfs_get_block to unpack the tail for us.
	 */
	index = inode->i_size >> PAGE_SHIFT;
	mapping = inode->i_mapping;
	page = grab_cache_page(mapping, index);
	retval = -ENOMEM;
	if (!page) {
		goto out;
	}
	retval = __reiserfs_write_begin(page, write_from, 0);
	if (retval)
		goto out_unlock;

	/* conversion can change page contents, must flush */
	flush_dcache_page(page);
	retval = reiserfs_commit_write(NULL, page, write_from, write_from);
	REISERFS_I(inode)->i_flags |= i_nopack_mask;

out_unlock:
	unlock_page(page);
	put_page(page);

out:
	inode_unlock(inode);
	reiserfs_write_unlock(inode->i_sb);
	return retval;
}
