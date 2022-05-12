// SPDX-License-Identifier: GPL-2.0
/*
 *  linux/fs/hfsplus/catalog.c
 *
 * Copyright (C) 2001
 * Brad Boyer (flar@allandria.com)
 * (C) 2003 Ardis Technologies <roman@ardistech.com>
 *
 * Handling of catalog records
 */


#include "hfsplus_fs.h"
#include "hfsplus_raw.h"

int hfsplus_cat_case_cmp_key(const hfsplus_btree_key *k1,
			     const hfsplus_btree_key *k2)
{
	__be32 k1p, k2p;

	k1p = k1->cat.parent;
	k2p = k2->cat.parent;
	if (k1p != k2p)
		return be32_to_cpu(k1p) < be32_to_cpu(k2p) ? -1 : 1;

	return hfsplus_strcasecmp(&k1->cat.name, &k2->cat.name);
}

int hfsplus_cat_bin_cmp_key(const hfsplus_btree_key *k1,
			    const hfsplus_btree_key *k2)
{
	__be32 k1p, k2p;

	k1p = k1->cat.parent;
	k2p = k2->cat.parent;
	if (k1p != k2p)
		return be32_to_cpu(k1p) < be32_to_cpu(k2p) ? -1 : 1;

	return hfsplus_strcmp(&k1->cat.name, &k2->cat.name);
}

/* Generates key for catalog file/folders record. */
int hfsplus_cat_build_key(struct super_block *sb,
		hfsplus_btree_key *key, u32 parent, const struct qstr *str)
{
	int len, err;

	key->cat.parent = cpu_to_be32(parent);
	err = hfsplus_asc2uni(sb, &key->cat.name, HFSPLUS_MAX_STRLEN,
			str->name, str->len);
	if (unlikely(err < 0))
		return err;

	len = be16_to_cpu(key->cat.name.length);
	key->key_len = cpu_to_be16(6 + 2 * len);
	return 0;
}

/* Generates key for catalog thread record. */
void hfsplus_cat_build_key_with_cnid(struct super_block *sb,
			hfsplus_btree_key *key, u32 parent)
{
	key->cat.parent = cpu_to_be32(parent);
	key->cat.name.length = 0;
	key->key_len = cpu_to_be16(6);
}

static void hfsplus_cat_build_key_uni(hfsplus_btree_key *key, u32 parent,
				      struct hfsplus_unistr *name)
{
	int ustrlen;

	ustrlen = be16_to_cpu(name->length);
	key->cat.parent = cpu_to_be32(parent);
	key->cat.name.length = cpu_to_be16(ustrlen);
	ustrlen *= 2;
	memcpy(key->cat.name.unicode, name->unicode, ustrlen);
	key->key_len = cpu_to_be16(6 + ustrlen);
}

void hfsplus_cat_set_perms(struct inode *inode, struct hfsplus_perm *perms)
{
	if (inode->i_flags & S_IMMUTABLE)
		perms->rootflags |= HFSPLUS_FLG_IMMUTABLE;
	else
		perms->rootflags &= ~HFSPLUS_FLG_IMMUTABLE;
	if (inode->i_flags & S_APPEND)
		perms->rootflags |= HFSPLUS_FLG_APPEND;
	else
		perms->rootflags &= ~HFSPLUS_FLG_APPEND;

	perms->userflags = HFSPLUS_I(inode)->userflags;
	perms->mode = cpu_to_be16(inode->i_mode);
	perms->owner = cpu_to_be32(i_uid_read(inode));
	perms->group = cpu_to_be32(i_gid_read(inode));

	if (S_ISREG(inode->i_mode))
		perms->dev = cpu_to_be32(inode->i_nlink);
	else if (S_ISBLK(inode->i_mode) || S_ISCHR(inode->i_mode))
		perms->dev = cpu_to_be32(inode->i_rdev);
	else
		perms->dev = 0;
}

static int hfsplus_cat_build_record(hfsplus_cat_entry *entry,
		u32 cnid, struct inode *inode)
{
	struct hfsplus_sb_info *sbi = HFSPLUS_SB(inode->i_sb);

	if (S_ISDIR(inode->i_mode)) {
		struct hfsplus_cat_folder *folder;

		folder = &entry->folder;
		memset(folder, 0, sizeof(*folder));
		folder->type = cpu_to_be16(HFSPLUS_FOLDER);
		if (test_bit(HFSPLUS_SB_HFSX, &sbi->flags))
			folder->flags |= cpu_to_be16(HFSPLUS_HAS_FOLDER_COUNT);
		folder->id = cpu_to_be32(inode->i_ino);
		HFSPLUS_I(inode)->create_date =
			folder->create_date =
			folder->content_mod_date =
			folder->attribute_mod_date =
			folder->access_date = hfsp_now2mt();
		hfsplus_cat_set_perms(inode, &folder->permissions);
		if (inode == sbi->hidden_dir)
			/* invisible and namelocked */
			folder->user_info.frFlags = cpu_to_be16(0x5000);
		return sizeof(*folder);
	} else {
		struct hfsplus_cat_file *file;

		file = &entry->file;
		memset(file, 0, sizeof(*file));
		file->type = cpu_to_be16(HFSPLUS_FILE);
		file->flags = cpu_to_be16(HFSPLUS_FILE_THREAD_EXISTS);
		file->id = cpu_to_be32(cnid);
		HFSPLUS_I(inode)->create_date =
			file->create_date =
			file->content_mod_date =
			file->attribute_mod_date =
			file->access_date = hfsp_now2mt();
		if (cnid == inode->i_ino) {
			hfsplus_cat_set_perms(inode, &file->permissions);
			if (S_ISLNK(inode->i_mode)) {
				file->user_info.fdType =
					cpu_to_be32(HFSP_SYMLINK_TYPE);
				file->user_info.fdCreator =
					cpu_to_be32(HFSP_SYMLINK_CREATOR);
			} else {
				file->user_info.fdType =
					cpu_to_be32(sbi->type);
				file->user_info.fdCreator =
					cpu_to_be32(sbi->creator);
			}
			if (HFSPLUS_FLG_IMMUTABLE &
					(file->permissions.rootflags |
					file->permissions.userflags))
				file->flags |=
					cpu_to_be16(HFSPLUS_FILE_LOCKED);
		} else {
			file->user_info.fdType =
				cpu_to_be32(HFSP_HARDLINK_TYPE);
			file->user_info.fdCreator =
				cpu_to_be32(HFSP_HFSPLUS_CREATOR);
			file->user_info.fdFlags =
				cpu_to_be16(0x100);
			file->create_date =
				HFSPLUS_I(sbi->hidden_dir)->create_date;
			file->permissions.dev =
				cpu_to_be32(HFSPLUS_I(inode)->linkid);
		}
		return sizeof(*file);
	}
}

static int hfsplus_fill_cat_thread(struct super_block *sb,
				   hfsplus_cat_entry *entry, int type,
				   u32 parentid, const struct qstr *str)
{
	int err;

	entry->type = cpu_to_be16(type);
	entry->thread.reserved = 0;
	entry->thread.parentID = cpu_to_be32(parentid);
	err = hfsplus_asc2uni(sb, &entry->thread.nodeName, HFSPLUS_MAX_STRLEN,
				str->name, str->len);
	if (unlikely(err < 0))
		return err;

	return 10 + be16_to_cpu(entry->thread.nodeName.length) * 2;
}

/* Try to get a catalog entry for given catalog id */
int hfsplus_find_cat(struct super_block *sb, u32 cnid,
		     struct hfs_find_data *fd)
{
	hfsplus_cat_entry tmp;
	int err;
	u16 type;

	hfsplus_cat_build_key_with_cnid(sb, fd->search_key, cnid);
	err = hfs_brec_read(fd, &tmp, sizeof(hfsplus_cat_entry));
	if (err)
		return err;

	type = be16_to_cpu(tmp.type);
	if (type != HFSPLUS_FOLDER_THREAD && type != HFSPLUS_FILE_THREAD) {
		pr_err("found bad thread record in catalog\n");
		return -EIO;
	}

	if (be16_to_cpu(tmp.thread.nodeName.length) > 255) {
		pr_err("catalog name length corrupted\n");
		return -EIO;
	}

	hfsplus_cat_build_key_uni(fd->search_key,
		be32_to_cpu(tmp.thread.parentID),
		&tmp.thread.nodeName);
	return hfs_brec_find(fd, hfs_find_rec_by_key);
}

static void hfsplus_subfolders_inc(struct inode *dir)
{
	struct hfsplus_sb_info *sbi = HFSPLUS_SB(dir->i_sb);

	if (test_bit(HFSPLUS_SB_HFSX, &sbi->flags)) {
		/*
		 * Increment subfolder count. Note, the value is only meaningful
		 * for folders with HFSPLUS_HAS_FOLDER_COUNT flag set.
		 */
		HFSPLUS_I(dir)->subfolders++;
	}
}

static void hfsplus_subfolders_dec(struct inode *dir)
{
	struct hfsplus_sb_info *sbi = HFSPLUS_SB(dir->i_sb);

	if (test_bit(HFSPLUS_SB_HFSX, &sbi->flags)) {
		/*
		 * Decrement subfolder count. Note, the value is only meaningful
		 * for folders with HFSPLUS_HAS_FOLDER_COUNT flag set.
		 *
		 * Check for zero. Some subfolders may have been created
		 * by an implementation ignorant of this counter.
		 */
		if (HFSPLUS_I(dir)->subfolders)
			HFSPLUS_I(dir)->subfolders--;
	}
}

int hfsplus_create_cat(u32 cnid, struct inode *dir,
		const struct qstr *str, struct inode *inode)
{
	struct super_block *sb = dir->i_sb;
	struct hfs_find_data fd;
	hfsplus_cat_entry entry;
	int entry_size;
	int err;

	hfs_dbg(CAT_MOD, "create_cat: %s,%u(%d)\n",
		str->name, cnid, inode->i_nlink);
	err = hfs_find_init(HFSPLUS_SB(sb)->cat_tree, &fd);
	if (err)
		return err;

	/*
	 * Fail early and avoid ENOSPC during the btree operations. We may
	 * have to split the root node at most once.
	 */
	err = hfs_bmap_reserve(fd.tree, 2 * fd.tree->depth);
	if (err)
		goto err2;

	hfsplus_cat_build_key_with_cnid(sb, fd.search_key, cnid);
	entry_size = hfsplus_fill_cat_thread(sb, &entry,
		S_ISDIR(inode->i_mode) ?
			HFSPLUS_FOLDER_THREAD : HFSPLUS_FILE_THREAD,
		dir->i_ino, str);
	if (unlikely(entry_size < 0)) {
		err = entry_size;
		goto err2;
	}

	err = hfs_brec_find(&fd, hfs_find_rec_by_key);
	if (err != -ENOENT) {
		if (!err)
			err = -EEXIST;
		goto err2;
	}
	err = hfs_brec_insert(&fd, &entry, entry_size);
	if (err)
		goto err2;

	err = hfsplus_cat_build_key(sb, fd.search_key, dir->i_ino, str);
	if (unlikely(err))
		goto err1;

	entry_size = hfsplus_cat_build_record(&entry, cnid, inode);
	err = hfs_brec_find(&fd, hfs_find_rec_by_key);
	if (err != -ENOENT) {
		/* panic? */
		if (!err)
			err = -EEXIST;
		goto err1;
	}
	err = hfs_brec_insert(&fd, &entry, entry_size);
	if (err)
		goto err1;

	dir->i_size++;
	if (S_ISDIR(inode->i_mode))
		hfsplus_subfolders_inc(dir);
	dir->i_mtime = dir->i_ctime = current_time(dir);
	hfsplus_mark_inode_dirty(dir, HFSPLUS_I_CAT_DIRTY);

	hfs_find_exit(&fd);
	return 0;

err1:
	hfsplus_cat_build_key_with_cnid(sb, fd.search_key, cnid);
	if (!hfs_brec_find(&fd, hfs_find_rec_by_key))
		hfs_brec_remove(&fd);
err2:
	hfs_find_exit(&fd);
	return err;
}

int hfsplus_delete_cat(u32 cnid, struct inode *dir, const struct qstr *str)
{
	struct super_block *sb = dir->i_sb;
	struct hfs_find_data fd;
	struct hfsplus_fork_raw fork;
	struct list_head *pos;
	int err, off;
	u16 type;

	hfs_dbg(CAT_MOD, "delete_cat: %s,%u\n", str ? str->name : NULL, cnid);
	err = hfs_find_init(HFSPLUS_SB(sb)->cat_tree, &fd);
	if (err)
		return err;

	/*
	 * Fail early and avoid ENOSPC during the btree operations. We may
	 * have to split the root node at most once.
	 */
	err = hfs_bmap_reserve(fd.tree, 2 * (int)fd.tree->depth - 2);
	if (err)
		goto out;

	if (!str) {
		int len;

		hfsplus_cat_build_key_with_cnid(sb, fd.search_key, cnid);
		err = hfs_brec_find(&fd, hfs_find_rec_by_key);
		if (err)
			goto out;

		off = fd.entryoffset +
			offsetof(struct hfsplus_cat_thread, nodeName);
		fd.search_key->cat.parent = cpu_to_be32(dir->i_ino);
		hfs_bnode_read(fd.bnode,
			&fd.search_key->cat.name.length, off, 2);
		len = be16_to_cpu(fd.search_key->cat.name.length) * 2;
		hfs_bnode_read(fd.bnode,
			&fd.search_key->cat.name.unicode,
			off + 2, len);
		fd.search_key->key_len = cpu_to_be16(6 + len);
	} else {
		err = hfsplus_cat_build_key(sb, fd.search_key, dir->i_ino, str);
		if (unlikely(err))
			goto out;
	}

	err = hfs_brec_find(&fd, hfs_find_rec_by_key);
	if (err)
		goto out;

	type = hfs_bnode_read_u16(fd.bnode, fd.entryoffset);
	if (type == HFSPLUS_FILE) {
#if 0
		off = fd.entryoffset + offsetof(hfsplus_cat_file, data_fork);
		hfs_bnode_read(fd.bnode, &fork, off, sizeof(fork));
		hfsplus_free_fork(sb, cnid, &fork, HFSPLUS_TYPE_DATA);
#endif

		off = fd.entryoffset +
			offsetof(struct hfsplus_cat_file, rsrc_fork);
		hfs_bnode_read(fd.bnode, &fork, off, sizeof(fork));
		hfsplus_free_fork(sb, cnid, &fork, HFSPLUS_TYPE_RSRC);
	}

	/* we only need to take spinlock for exclusion with ->release() */
	spin_lock(&HFSPLUS_I(dir)->open_dir_lock);
	list_for_each(pos, &HFSPLUS_I(dir)->open_dir_list) {
		struct hfsplus_readdir_data *rd =
			list_entry(pos, struct hfsplus_readdir_data, list);
		if (fd.tree->keycmp(fd.search_key, (void *)&rd->key) < 0)
			rd->file->f_pos--;
	}
	spin_unlock(&HFSPLUS_I(dir)->open_dir_lock);

	err = hfs_brec_remove(&fd);
	if (err)
		goto out;

	hfsplus_cat_build_key_with_cnid(sb, fd.search_key, cnid);
	err = hfs_brec_find(&fd, hfs_find_rec_by_key);
	if (err)
		goto out;

	err = hfs_brec_remove(&fd);
	if (err)
		goto out;

	dir->i_size--;
	if (type == HFSPLUS_FOLDER)
		hfsplus_subfolders_dec(dir);
	dir->i_mtime = dir->i_ctime = current_time(dir);
	hfsplus_mark_inode_dirty(dir, HFSPLUS_I_CAT_DIRTY);

	if (type == HFSPLUS_FILE || type == HFSPLUS_FOLDER) {
		if (HFSPLUS_SB(sb)->attr_tree)
			hfsplus_delete_all_attrs(dir, cnid);
	}

out:
	hfs_find_exit(&fd);

	return err;
}

int hfsplus_rename_cat(u32 cnid,
		       struct inode *src_dir, const struct qstr *src_name,
		       struct inode *dst_dir, const struct qstr *dst_name)
{
	struct super_block *sb = src_dir->i_sb;
	struct hfs_find_data src_fd, dst_fd;
	hfsplus_cat_entry entry;
	int entry_size, type;
	int err;

	hfs_dbg(CAT_MOD, "rename_cat: %u - %lu,%s - %lu,%s\n",
		cnid, src_dir->i_ino, src_name->name,
		dst_dir->i_ino, dst_name->name);
	err = hfs_find_init(HFSPLUS_SB(sb)->cat_tree, &src_fd);
	if (err)
		return err;
	dst_fd = src_fd;

	/*
	 * Fail early and avoid ENOSPC during the btree operations. We may
	 * have to split the root node at most twice.
	 */
	err = hfs_bmap_reserve(src_fd.tree, 4 * (int)src_fd.tree->depth - 1);
	if (err)
		goto out;

	/* find the old dir entry and read the data */
	err = hfsplus_cat_build_key(sb, src_fd.search_key,
			src_dir->i_ino, src_name);
	if (unlikely(err))
		goto out;

	err = hfs_brec_find(&src_fd, hfs_find_rec_by_key);
	if (err)
		goto out;
	if (src_fd.entrylength > sizeof(entry) || src_fd.entrylength < 0) {
		err = -EIO;
		goto out;
	}

	hfs_bnode_read(src_fd.bnode, &entry, src_fd.entryoffset,
				src_fd.entrylength);
	type = be16_to_cpu(entry.type);

	/* create new dir entry with the data from the old entry */
	err = hfsplus_cat_build_key(sb, dst_fd.search_key,
			dst_dir->i_ino, dst_name);
	if (unlikely(err))
		goto out;

	err = hfs_brec_find(&dst_fd, hfs_find_rec_by_key);
	if (err != -ENOENT) {
		if (!err)
			err = -EEXIST;
		goto out;
	}

	err = hfs_brec_insert(&dst_fd, &entry, src_fd.entrylength);
	if (err)
		goto out;
	dst_dir->i_size++;
	if (type == HFSPLUS_FOLDER)
		hfsplus_subfolders_inc(dst_dir);
	dst_dir->i_mtime = dst_dir->i_ctime = current_time(dst_dir);

	/* finally remove the old entry */
	err = hfsplus_cat_build_key(sb, src_fd.search_key,
			src_dir->i_ino, src_name);
	if (unlikely(err))
		goto out;

	err = hfs_brec_find(&src_fd, hfs_find_rec_by_key);
	if (err)
		goto out;
	err = hfs_brec_remove(&src_fd);
	if (err)
		goto out;
	src_dir->i_size--;
	if (type == HFSPLUS_FOLDER)
		hfsplus_subfolders_dec(src_dir);
	src_dir->i_mtime = src_dir->i_ctime = current_time(src_dir);

	/* remove old thread entry */
	hfsplus_cat_build_key_with_cnid(sb, src_fd.search_key, cnid);
	err = hfs_brec_find(&src_fd, hfs_find_rec_by_key);
	if (err)
		goto out;
	type = hfs_bnode_read_u16(src_fd.bnode, src_fd.entryoffset);
	err = hfs_brec_remove(&src_fd);
	if (err)
		goto out;

	/* create new thread entry */
	hfsplus_cat_build_key_with_cnid(sb, dst_fd.search_key, cnid);
	entry_size = hfsplus_fill_cat_thread(sb, &entry, type,
		dst_dir->i_ino, dst_name);
	if (unlikely(entry_size < 0)) {
		err = entry_size;
		goto out;
	}

	err = hfs_brec_find(&dst_fd, hfs_find_rec_by_key);
	if (err != -ENOENT) {
		if (!err)
			err = -EEXIST;
		goto out;
	}
	err = hfs_brec_insert(&dst_fd, &entry, entry_size);

	hfsplus_mark_inode_dirty(dst_dir, HFSPLUS_I_CAT_DIRTY);
	hfsplus_mark_inode_dirty(src_dir, HFSPLUS_I_CAT_DIRTY);
out:
	hfs_bnode_put(dst_fd.bnode);
	hfs_find_exit(&src_fd);
	return err;
}
