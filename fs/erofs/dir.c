// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2017-2018 HUAWEI, Inc.
 *             https://www.huawei.com/
 * Created by Gao Xiang <gaoxiang25@huawei.com>
 */
#include "internal.h"

static void debug_one_dentry(unsigned char d_type, const char *de_name,
			     unsigned int de_namelen)
{
#ifdef CONFIG_EROFS_FS_DEBUG
	/* since the on-disk name could not have the trailing '\0' */
	unsigned char dbg_namebuf[EROFS_NAME_LEN + 1];

	memcpy(dbg_namebuf, de_name, de_namelen);
	dbg_namebuf[de_namelen] = '\0';

	erofs_dbg("found dirent %s de_len %u d_type %d", dbg_namebuf,
		  de_namelen, d_type);
#endif
}

static int erofs_fill_dentries(struct inode *dir, struct dir_context *ctx,
			       void *dentry_blk, unsigned int *ofs,
			       unsigned int nameoff, unsigned int maxsize)
{
	struct erofs_dirent *de = dentry_blk + *ofs;
	const struct erofs_dirent *end = dentry_blk + nameoff;

	while (de < end) {
		const char *de_name;
		unsigned int de_namelen;
		unsigned char d_type;

		d_type = fs_ftype_to_dtype(de->file_type);

		nameoff = le16_to_cpu(de->nameoff);
		de_name = (char *)dentry_blk + nameoff;

		/* the last dirent in the block? */
		if (de + 1 >= end)
			de_namelen = strnlen(de_name, maxsize - nameoff);
		else
			de_namelen = le16_to_cpu(de[1].nameoff) - nameoff;

		/* a corrupted entry is found */
		if (nameoff + de_namelen > maxsize ||
		    de_namelen > EROFS_NAME_LEN) {
			erofs_err(dir->i_sb, "bogus dirent @ nid %llu",
				  EROFS_I(dir)->nid);
			DBG_BUGON(1);
			return -EFSCORRUPTED;
		}

		debug_one_dentry(d_type, de_name, de_namelen);
		if (!dir_emit(ctx, de_name, de_namelen,
			      le64_to_cpu(de->nid), d_type))
			/* stopped by some reason */
			return 1;
		++de;
		*ofs += sizeof(struct erofs_dirent);
	}
	*ofs = maxsize;
	return 0;
}

static int erofs_readdir(struct file *f, struct dir_context *ctx)
{
	struct inode *dir = file_inode(f);
	struct address_space *mapping = dir->i_mapping;
	const size_t dirsize = i_size_read(dir);
	unsigned int i = ctx->pos / EROFS_BLKSIZ;
	unsigned int ofs = ctx->pos % EROFS_BLKSIZ;
	int err = 0;
	bool initial = true;

	while (ctx->pos < dirsize) {
		struct page *dentry_page;
		struct erofs_dirent *de;
		unsigned int nameoff, maxsize;

		dentry_page = read_mapping_page(mapping, i, NULL);
		if (dentry_page == ERR_PTR(-ENOMEM)) {
			err = -ENOMEM;
			break;
		} else if (IS_ERR(dentry_page)) {
			erofs_err(dir->i_sb,
				  "fail to readdir of logical block %u of nid %llu",
				  i, EROFS_I(dir)->nid);
			err = -EFSCORRUPTED;
			break;
		}

		de = (struct erofs_dirent *)kmap(dentry_page);

		nameoff = le16_to_cpu(de->nameoff);

		if (nameoff < sizeof(struct erofs_dirent) ||
		    nameoff >= PAGE_SIZE) {
			erofs_err(dir->i_sb,
				  "invalid de[0].nameoff %u @ nid %llu",
				  nameoff, EROFS_I(dir)->nid);
			err = -EFSCORRUPTED;
			goto skip_this;
		}

		maxsize = min_t(unsigned int,
				dirsize - ctx->pos + ofs, PAGE_SIZE);

		/* search dirents at the arbitrary position */
		if (initial) {
			initial = false;

			ofs = roundup(ofs, sizeof(struct erofs_dirent));
			if (ofs >= nameoff)
				goto skip_this;
		}

		err = erofs_fill_dentries(dir, ctx, de, &ofs,
					  nameoff, maxsize);
skip_this:
		kunmap(dentry_page);

		put_page(dentry_page);

		ctx->pos = blknr_to_addr(i) + ofs;

		if (err)
			break;
		++i;
		ofs = 0;
	}
	return err < 0 ? err : 0;
}

const struct file_operations erofs_dir_fops = {
	.llseek		= generic_file_llseek,
	.read		= generic_read_dir,
	.iterate_shared	= erofs_readdir,
};

