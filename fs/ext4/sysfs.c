// SPDX-License-Identifier: GPL-2.0
/*
 *  linux/fs/ext4/sysfs.c
 *
 * Copyright (C) 1992, 1993, 1994, 1995
 * Remy Card (card@masi.ibp.fr)
 * Theodore Ts'o (tytso@mit.edu)
 *
 */

#include <linux/time.h>
#include <linux/fs.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <linux/part_stat.h>

#include "ext4.h"
#include "ext4_jbd2.h"

typedef enum {
	attr_noop,
	attr_delayed_allocation_blocks,
	attr_session_write_kbytes,
	attr_lifetime_write_kbytes,
	attr_reserved_clusters,
	attr_sra_exceeded_retry_limit,
	attr_inode_readahead,
	attr_trigger_test_error,
	attr_first_error_time,
	attr_last_error_time,
	attr_feature,
	attr_pointer_ui,
	attr_pointer_ul,
	attr_pointer_u64,
	attr_pointer_u8,
	attr_pointer_string,
	attr_pointer_atomic,
	attr_journal_task,
} attr_id_t;

typedef enum {
	ptr_explicit,
	ptr_ext4_sb_info_offset,
	ptr_ext4_super_block_offset,
} attr_ptr_t;

static const char proc_dirname[] = "fs/ext4";
static struct proc_dir_entry *ext4_proc_root;

struct ext4_attr {
	struct attribute attr;
	short attr_id;
	short attr_ptr;
	unsigned short attr_size;
	union {
		int offset;
		void *explicit_ptr;
	} u;
};

static ssize_t session_write_kbytes_show(struct ext4_sb_info *sbi, char *buf)
{
	struct super_block *sb = sbi->s_buddy_cache->i_sb;

	return snprintf(buf, PAGE_SIZE, "%lu\n",
			(part_stat_read(sb->s_bdev, sectors[STAT_WRITE]) -
			 sbi->s_sectors_written_start) >> 1);
}

static ssize_t lifetime_write_kbytes_show(struct ext4_sb_info *sbi, char *buf)
{
	struct super_block *sb = sbi->s_buddy_cache->i_sb;

	return snprintf(buf, PAGE_SIZE, "%llu\n",
			(unsigned long long)(sbi->s_kbytes_written +
			((part_stat_read(sb->s_bdev, sectors[STAT_WRITE]) -
			  EXT4_SB(sb)->s_sectors_written_start) >> 1)));
}

static ssize_t inode_readahead_blks_store(struct ext4_sb_info *sbi,
					  const char *buf, size_t count)
{
	unsigned long t;
	int ret;

	ret = kstrtoul(skip_spaces(buf), 0, &t);
	if (ret)
		return ret;

	if (t && (!is_power_of_2(t) || t > 0x40000000))
		return -EINVAL;

	sbi->s_inode_readahead_blks = t;
	return count;
}

static ssize_t reserved_clusters_store(struct ext4_sb_info *sbi,
				   const char *buf, size_t count)
{
	unsigned long long val;
	ext4_fsblk_t clusters = (ext4_blocks_count(sbi->s_es) >>
				 sbi->s_cluster_bits);
	int ret;

	ret = kstrtoull(skip_spaces(buf), 0, &val);
	if (ret || val >= clusters)
		return -EINVAL;

	atomic64_set(&sbi->s_resv_clusters, val);
	return count;
}

static ssize_t trigger_test_error(struct ext4_sb_info *sbi,
				  const char *buf, size_t count)
{
	int len = count;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	if (len && buf[len-1] == '\n')
		len--;

	if (len)
		ext4_error(sbi->s_sb, "%.*s", len, buf);
	return count;
}

static ssize_t journal_task_show(struct ext4_sb_info *sbi, char *buf)
{
	if (!sbi->s_journal)
		return snprintf(buf, PAGE_SIZE, "<none>\n");
	return snprintf(buf, PAGE_SIZE, "%d\n",
			task_pid_vnr(sbi->s_journal->j_task));
}

#define EXT4_ATTR(_name,_mode,_id)					\
static struct ext4_attr ext4_attr_##_name = {				\
	.attr = {.name = __stringify(_name), .mode = _mode },		\
	.attr_id = attr_##_id,						\
}

#define EXT4_ATTR_FUNC(_name,_mode)  EXT4_ATTR(_name,_mode,_name)

#define EXT4_ATTR_FEATURE(_name)   EXT4_ATTR(_name, 0444, feature)

#define EXT4_ATTR_OFFSET(_name,_mode,_id,_struct,_elname)	\
static struct ext4_attr ext4_attr_##_name = {			\
	.attr = {.name = __stringify(_name), .mode = _mode },	\
	.attr_id = attr_##_id,					\
	.attr_ptr = ptr_##_struct##_offset,			\
	.u = {							\
		.offset = offsetof(struct _struct, _elname),\
	},							\
}

#define EXT4_ATTR_STRING(_name,_mode,_size,_struct,_elname)	\
static struct ext4_attr ext4_attr_##_name = {			\
	.attr = {.name = __stringify(_name), .mode = _mode },	\
	.attr_id = attr_pointer_string,				\
	.attr_size = _size,					\
	.attr_ptr = ptr_##_struct##_offset,			\
	.u = {							\
		.offset = offsetof(struct _struct, _elname),\
	},							\
}

#define EXT4_RO_ATTR_ES_UI(_name,_elname)				\
	EXT4_ATTR_OFFSET(_name, 0444, pointer_ui, ext4_super_block, _elname)

#define EXT4_RO_ATTR_ES_U8(_name,_elname)				\
	EXT4_ATTR_OFFSET(_name, 0444, pointer_u8, ext4_super_block, _elname)

#define EXT4_RO_ATTR_ES_U64(_name,_elname)				\
	EXT4_ATTR_OFFSET(_name, 0444, pointer_u64, ext4_super_block, _elname)

#define EXT4_RO_ATTR_ES_STRING(_name,_elname,_size)			\
	EXT4_ATTR_STRING(_name, 0444, _size, ext4_super_block, _elname)

#define EXT4_RW_ATTR_SBI_UI(_name,_elname)	\
	EXT4_ATTR_OFFSET(_name, 0644, pointer_ui, ext4_sb_info, _elname)

#define EXT4_RW_ATTR_SBI_UL(_name,_elname)	\
	EXT4_ATTR_OFFSET(_name, 0644, pointer_ul, ext4_sb_info, _elname)

#define EXT4_RO_ATTR_SBI_ATOMIC(_name,_elname)	\
	EXT4_ATTR_OFFSET(_name, 0444, pointer_atomic, ext4_sb_info, _elname)

#define EXT4_ATTR_PTR(_name,_mode,_id,_ptr) \
static struct ext4_attr ext4_attr_##_name = {			\
	.attr = {.name = __stringify(_name), .mode = _mode },	\
	.attr_id = attr_##_id,					\
	.attr_ptr = ptr_explicit,				\
	.u = {							\
		.explicit_ptr = _ptr,				\
	},							\
}

#define ATTR_LIST(name) &ext4_attr_##name.attr

EXT4_ATTR_FUNC(delayed_allocation_blocks, 0444);
EXT4_ATTR_FUNC(session_write_kbytes, 0444);
EXT4_ATTR_FUNC(lifetime_write_kbytes, 0444);
EXT4_ATTR_FUNC(reserved_clusters, 0644);
EXT4_ATTR_FUNC(sra_exceeded_retry_limit, 0444);

EXT4_ATTR_OFFSET(inode_readahead_blks, 0644, inode_readahead,
		 ext4_sb_info, s_inode_readahead_blks);
EXT4_RW_ATTR_SBI_UI(inode_goal, s_inode_goal);
EXT4_RW_ATTR_SBI_UI(mb_stats, s_mb_stats);
EXT4_RW_ATTR_SBI_UI(mb_max_to_scan, s_mb_max_to_scan);
EXT4_RW_ATTR_SBI_UI(mb_min_to_scan, s_mb_min_to_scan);
EXT4_RW_ATTR_SBI_UI(mb_order2_req, s_mb_order2_reqs);
EXT4_RW_ATTR_SBI_UI(mb_stream_req, s_mb_stream_request);
EXT4_RW_ATTR_SBI_UI(mb_group_prealloc, s_mb_group_prealloc);
EXT4_RW_ATTR_SBI_UI(mb_max_inode_prealloc, s_mb_max_inode_prealloc);
EXT4_RW_ATTR_SBI_UI(mb_max_linear_groups, s_mb_max_linear_groups);
EXT4_RW_ATTR_SBI_UI(extent_max_zeroout_kb, s_extent_max_zeroout_kb);
EXT4_ATTR(trigger_fs_error, 0200, trigger_test_error);
EXT4_RW_ATTR_SBI_UI(err_ratelimit_interval_ms, s_err_ratelimit_state.interval);
EXT4_RW_ATTR_SBI_UI(err_ratelimit_burst, s_err_ratelimit_state.burst);
EXT4_RW_ATTR_SBI_UI(warning_ratelimit_interval_ms, s_warning_ratelimit_state.interval);
EXT4_RW_ATTR_SBI_UI(warning_ratelimit_burst, s_warning_ratelimit_state.burst);
EXT4_RW_ATTR_SBI_UI(msg_ratelimit_interval_ms, s_msg_ratelimit_state.interval);
EXT4_RW_ATTR_SBI_UI(msg_ratelimit_burst, s_msg_ratelimit_state.burst);
#ifdef CONFIG_EXT4_DEBUG
EXT4_RW_ATTR_SBI_UL(simulate_fail, s_simulate_fail);
#endif
EXT4_RO_ATTR_SBI_ATOMIC(warning_count, s_warning_count);
EXT4_RO_ATTR_SBI_ATOMIC(msg_count, s_msg_count);
EXT4_RO_ATTR_ES_UI(errors_count, s_error_count);
EXT4_RO_ATTR_ES_U8(first_error_errcode, s_first_error_errcode);
EXT4_RO_ATTR_ES_U8(last_error_errcode, s_last_error_errcode);
EXT4_RO_ATTR_ES_UI(first_error_ino, s_first_error_ino);
EXT4_RO_ATTR_ES_UI(last_error_ino, s_last_error_ino);
EXT4_RO_ATTR_ES_U64(first_error_block, s_first_error_block);
EXT4_RO_ATTR_ES_U64(last_error_block, s_last_error_block);
EXT4_RO_ATTR_ES_UI(first_error_line, s_first_error_line);
EXT4_RO_ATTR_ES_UI(last_error_line, s_last_error_line);
EXT4_RO_ATTR_ES_STRING(first_error_func, s_first_error_func, 32);
EXT4_RO_ATTR_ES_STRING(last_error_func, s_last_error_func, 32);
EXT4_ATTR(first_error_time, 0444, first_error_time);
EXT4_ATTR(last_error_time, 0444, last_error_time);
EXT4_ATTR(journal_task, 0444, journal_task);
EXT4_RW_ATTR_SBI_UI(mb_prefetch, s_mb_prefetch);
EXT4_RW_ATTR_SBI_UI(mb_prefetch_limit, s_mb_prefetch_limit);

static unsigned int old_bump_val = 128;
EXT4_ATTR_PTR(max_writeback_mb_bump, 0444, pointer_ui, &old_bump_val);

static struct attribute *ext4_attrs[] = {
	ATTR_LIST(delayed_allocation_blocks),
	ATTR_LIST(session_write_kbytes),
	ATTR_LIST(lifetime_write_kbytes),
	ATTR_LIST(reserved_clusters),
	ATTR_LIST(sra_exceeded_retry_limit),
	ATTR_LIST(inode_readahead_blks),
	ATTR_LIST(inode_goal),
	ATTR_LIST(mb_stats),
	ATTR_LIST(mb_max_to_scan),
	ATTR_LIST(mb_min_to_scan),
	ATTR_LIST(mb_order2_req),
	ATTR_LIST(mb_stream_req),
	ATTR_LIST(mb_group_prealloc),
	ATTR_LIST(mb_max_inode_prealloc),
	ATTR_LIST(mb_max_linear_groups),
	ATTR_LIST(max_writeback_mb_bump),
	ATTR_LIST(extent_max_zeroout_kb),
	ATTR_LIST(trigger_fs_error),
	ATTR_LIST(err_ratelimit_interval_ms),
	ATTR_LIST(err_ratelimit_burst),
	ATTR_LIST(warning_ratelimit_interval_ms),
	ATTR_LIST(warning_ratelimit_burst),
	ATTR_LIST(msg_ratelimit_interval_ms),
	ATTR_LIST(msg_ratelimit_burst),
	ATTR_LIST(errors_count),
	ATTR_LIST(warning_count),
	ATTR_LIST(msg_count),
	ATTR_LIST(first_error_ino),
	ATTR_LIST(last_error_ino),
	ATTR_LIST(first_error_block),
	ATTR_LIST(last_error_block),
	ATTR_LIST(first_error_line),
	ATTR_LIST(last_error_line),
	ATTR_LIST(first_error_func),
	ATTR_LIST(last_error_func),
	ATTR_LIST(first_error_errcode),
	ATTR_LIST(last_error_errcode),
	ATTR_LIST(first_error_time),
	ATTR_LIST(last_error_time),
	ATTR_LIST(journal_task),
#ifdef CONFIG_EXT4_DEBUG
	ATTR_LIST(simulate_fail),
#endif
	ATTR_LIST(mb_prefetch),
	ATTR_LIST(mb_prefetch_limit),
	NULL,
};
ATTRIBUTE_GROUPS(ext4);

/* Features this copy of ext4 supports */
EXT4_ATTR_FEATURE(lazy_itable_init);
EXT4_ATTR_FEATURE(batched_discard);
EXT4_ATTR_FEATURE(meta_bg_resize);
#ifdef CONFIG_FS_ENCRYPTION
EXT4_ATTR_FEATURE(encryption);
EXT4_ATTR_FEATURE(test_dummy_encryption_v2);
#endif
#ifdef CONFIG_UNICODE
EXT4_ATTR_FEATURE(casefold);
#endif
#ifdef CONFIG_FS_VERITY
EXT4_ATTR_FEATURE(verity);
#endif
EXT4_ATTR_FEATURE(metadata_csum_seed);
EXT4_ATTR_FEATURE(fast_commit);
#if defined(CONFIG_UNICODE) && defined(CONFIG_FS_ENCRYPTION)
EXT4_ATTR_FEATURE(encrypted_casefold);
#endif

static struct attribute *ext4_feat_attrs[] = {
	ATTR_LIST(lazy_itable_init),
	ATTR_LIST(batched_discard),
	ATTR_LIST(meta_bg_resize),
#ifdef CONFIG_FS_ENCRYPTION
	ATTR_LIST(encryption),
	ATTR_LIST(test_dummy_encryption_v2),
#endif
#ifdef CONFIG_UNICODE
	ATTR_LIST(casefold),
#endif
#ifdef CONFIG_FS_VERITY
	ATTR_LIST(verity),
#endif
	ATTR_LIST(metadata_csum_seed),
	ATTR_LIST(fast_commit),
#if defined(CONFIG_UNICODE) && defined(CONFIG_FS_ENCRYPTION)
	ATTR_LIST(encrypted_casefold),
#endif
	NULL,
};
ATTRIBUTE_GROUPS(ext4_feat);

static void *calc_ptr(struct ext4_attr *a, struct ext4_sb_info *sbi)
{
	switch (a->attr_ptr) {
	case ptr_explicit:
		return a->u.explicit_ptr;
	case ptr_ext4_sb_info_offset:
		return (void *) (((char *) sbi) + a->u.offset);
	case ptr_ext4_super_block_offset:
		return (void *) (((char *) sbi->s_es) + a->u.offset);
	}
	return NULL;
}

static ssize_t __print_tstamp(char *buf, __le32 lo, __u8 hi)
{
	return snprintf(buf, PAGE_SIZE, "%lld\n",
			((time64_t)hi << 32) + le32_to_cpu(lo));
}

#define print_tstamp(buf, es, tstamp) \
	__print_tstamp(buf, (es)->tstamp, (es)->tstamp ## _hi)

static ssize_t ext4_attr_show(struct kobject *kobj,
			      struct attribute *attr, char *buf)
{
	struct ext4_sb_info *sbi = container_of(kobj, struct ext4_sb_info,
						s_kobj);
	struct ext4_attr *a = container_of(attr, struct ext4_attr, attr);
	void *ptr = calc_ptr(a, sbi);

	switch (a->attr_id) {
	case attr_delayed_allocation_blocks:
		return snprintf(buf, PAGE_SIZE, "%llu\n",
				(s64) EXT4_C2B(sbi,
		       percpu_counter_sum(&sbi->s_dirtyclusters_counter)));
	case attr_session_write_kbytes:
		return session_write_kbytes_show(sbi, buf);
	case attr_lifetime_write_kbytes:
		return lifetime_write_kbytes_show(sbi, buf);
	case attr_reserved_clusters:
		return snprintf(buf, PAGE_SIZE, "%llu\n",
				(unsigned long long)
				atomic64_read(&sbi->s_resv_clusters));
	case attr_sra_exceeded_retry_limit:
		return snprintf(buf, PAGE_SIZE, "%llu\n",
				(unsigned long long)
			percpu_counter_sum(&sbi->s_sra_exceeded_retry_limit));
	case attr_inode_readahead:
	case attr_pointer_ui:
		if (!ptr)
			return 0;
		if (a->attr_ptr == ptr_ext4_super_block_offset)
			return snprintf(buf, PAGE_SIZE, "%u\n",
					le32_to_cpup(ptr));
		else
			return snprintf(buf, PAGE_SIZE, "%u\n",
					*((unsigned int *) ptr));
	case attr_pointer_ul:
		if (!ptr)
			return 0;
		return snprintf(buf, PAGE_SIZE, "%lu\n",
				*((unsigned long *) ptr));
	case attr_pointer_u8:
		if (!ptr)
			return 0;
		return snprintf(buf, PAGE_SIZE, "%u\n",
				*((unsigned char *) ptr));
	case attr_pointer_u64:
		if (!ptr)
			return 0;
		if (a->attr_ptr == ptr_ext4_super_block_offset)
			return snprintf(buf, PAGE_SIZE, "%llu\n",
					le64_to_cpup(ptr));
		else
			return snprintf(buf, PAGE_SIZE, "%llu\n",
					*((unsigned long long *) ptr));
	case attr_pointer_string:
		if (!ptr)
			return 0;
		return snprintf(buf, PAGE_SIZE, "%.*s\n", a->attr_size,
				(char *) ptr);
	case attr_pointer_atomic:
		if (!ptr)
			return 0;
		return snprintf(buf, PAGE_SIZE, "%d\n",
				atomic_read((atomic_t *) ptr));
	case attr_feature:
		return snprintf(buf, PAGE_SIZE, "supported\n");
	case attr_first_error_time:
		return print_tstamp(buf, sbi->s_es, s_first_error_time);
	case attr_last_error_time:
		return print_tstamp(buf, sbi->s_es, s_last_error_time);
	case attr_journal_task:
		return journal_task_show(sbi, buf);
	}

	return 0;
}

static ssize_t ext4_attr_store(struct kobject *kobj,
			       struct attribute *attr,
			       const char *buf, size_t len)
{
	struct ext4_sb_info *sbi = container_of(kobj, struct ext4_sb_info,
						s_kobj);
	struct ext4_attr *a = container_of(attr, struct ext4_attr, attr);
	void *ptr = calc_ptr(a, sbi);
	unsigned long t;
	int ret;

	switch (a->attr_id) {
	case attr_reserved_clusters:
		return reserved_clusters_store(sbi, buf, len);
	case attr_pointer_ui:
		if (!ptr)
			return 0;
		ret = kstrtoul(skip_spaces(buf), 0, &t);
		if (ret)
			return ret;
		if (a->attr_ptr == ptr_ext4_super_block_offset)
			*((__le32 *) ptr) = cpu_to_le32(t);
		else
			*((unsigned int *) ptr) = t;
		return len;
	case attr_pointer_ul:
		if (!ptr)
			return 0;
		ret = kstrtoul(skip_spaces(buf), 0, &t);
		if (ret)
			return ret;
		*((unsigned long *) ptr) = t;
		return len;
	case attr_inode_readahead:
		return inode_readahead_blks_store(sbi, buf, len);
	case attr_trigger_test_error:
		return trigger_test_error(sbi, buf, len);
	}
	return 0;
}

static void ext4_sb_release(struct kobject *kobj)
{
	struct ext4_sb_info *sbi = container_of(kobj, struct ext4_sb_info,
						s_kobj);
	complete(&sbi->s_kobj_unregister);
}

static const struct sysfs_ops ext4_attr_ops = {
	.show	= ext4_attr_show,
	.store	= ext4_attr_store,
};

static struct kobj_type ext4_sb_ktype = {
	.default_groups = ext4_groups,
	.sysfs_ops	= &ext4_attr_ops,
	.release	= ext4_sb_release,
};

static struct kobj_type ext4_feat_ktype = {
	.default_groups = ext4_feat_groups,
	.sysfs_ops	= &ext4_attr_ops,
	.release	= (void (*)(struct kobject *))kfree,
};

static struct kobject *ext4_root;

static struct kobject *ext4_feat;

int ext4_register_sysfs(struct super_block *sb)
{
	struct ext4_sb_info *sbi = EXT4_SB(sb);
	int err;

	init_completion(&sbi->s_kobj_unregister);
	err = kobject_init_and_add(&sbi->s_kobj, &ext4_sb_ktype, ext4_root,
				   "%s", sb->s_id);
	if (err) {
		kobject_put(&sbi->s_kobj);
		wait_for_completion(&sbi->s_kobj_unregister);
		return err;
	}

	if (ext4_proc_root)
		sbi->s_proc = proc_mkdir(sb->s_id, ext4_proc_root);
	if (sbi->s_proc) {
		proc_create_single_data("options", S_IRUGO, sbi->s_proc,
				ext4_seq_options_show, sb);
		proc_create_single_data("es_shrinker_info", S_IRUGO,
				sbi->s_proc, ext4_seq_es_shrinker_info_show,
				sb);
		proc_create_single_data("fc_info", 0444, sbi->s_proc,
					ext4_fc_info_show, sb);
		proc_create_seq_data("mb_groups", S_IRUGO, sbi->s_proc,
				&ext4_mb_seq_groups_ops, sb);
		proc_create_single_data("mb_stats", 0444, sbi->s_proc,
				ext4_seq_mb_stats_show, sb);
		proc_create_seq_data("mb_structs_summary", 0444, sbi->s_proc,
				&ext4_mb_seq_structs_summary_ops, sb);
	}
	return 0;
}

void ext4_unregister_sysfs(struct super_block *sb)
{
	struct ext4_sb_info *sbi = EXT4_SB(sb);

	if (sbi->s_proc)
		remove_proc_subtree(sb->s_id, ext4_proc_root);
	kobject_del(&sbi->s_kobj);
}

int __init ext4_init_sysfs(void)
{
	int ret;

	ext4_root = kobject_create_and_add("ext4", fs_kobj);
	if (!ext4_root)
		return -ENOMEM;

	ext4_feat = kzalloc(sizeof(*ext4_feat), GFP_KERNEL);
	if (!ext4_feat) {
		ret = -ENOMEM;
		goto root_err;
	}

	ret = kobject_init_and_add(ext4_feat, &ext4_feat_ktype,
				   ext4_root, "features");
	if (ret)
		goto feat_err;

	ext4_proc_root = proc_mkdir(proc_dirname, NULL);
	return ret;

feat_err:
	kobject_put(ext4_feat);
	ext4_feat = NULL;
root_err:
	kobject_put(ext4_root);
	ext4_root = NULL;
	return ret;
}

void ext4_exit_sysfs(void)
{
	kobject_put(ext4_feat);
	ext4_feat = NULL;
	kobject_put(ext4_root);
	ext4_root = NULL;
	remove_proc_entry(proc_dirname, NULL);
	ext4_proc_root = NULL;
}

