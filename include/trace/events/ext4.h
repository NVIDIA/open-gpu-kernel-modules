/* SPDX-License-Identifier: GPL-2.0 */
#undef TRACE_SYSTEM
#define TRACE_SYSTEM ext4

#if !defined(_TRACE_EXT4_H) || defined(TRACE_HEADER_MULTI_READ)
#define _TRACE_EXT4_H

#include <linux/writeback.h>
#include <linux/tracepoint.h>

struct ext4_allocation_context;
struct ext4_allocation_request;
struct ext4_extent;
struct ext4_prealloc_space;
struct ext4_inode_info;
struct mpage_da_data;
struct ext4_map_blocks;
struct extent_status;
struct ext4_fsmap;
struct partial_cluster;

#define EXT4_I(inode) (container_of(inode, struct ext4_inode_info, vfs_inode))

#define show_mballoc_flags(flags) __print_flags(flags, "|",	\
	{ EXT4_MB_HINT_MERGE,		"HINT_MERGE" },		\
	{ EXT4_MB_HINT_RESERVED,	"HINT_RESV" },		\
	{ EXT4_MB_HINT_METADATA,	"HINT_MDATA" },		\
	{ EXT4_MB_HINT_FIRST,		"HINT_FIRST" },		\
	{ EXT4_MB_HINT_BEST,		"HINT_BEST" },		\
	{ EXT4_MB_HINT_DATA,		"HINT_DATA" },		\
	{ EXT4_MB_HINT_NOPREALLOC,	"HINT_NOPREALLOC" },	\
	{ EXT4_MB_HINT_GROUP_ALLOC,	"HINT_GRP_ALLOC" },	\
	{ EXT4_MB_HINT_GOAL_ONLY,	"HINT_GOAL_ONLY" },	\
	{ EXT4_MB_HINT_TRY_GOAL,	"HINT_TRY_GOAL" },	\
	{ EXT4_MB_DELALLOC_RESERVED,	"DELALLOC_RESV" },	\
	{ EXT4_MB_STREAM_ALLOC,		"STREAM_ALLOC" },	\
	{ EXT4_MB_USE_ROOT_BLOCKS,	"USE_ROOT_BLKS" },	\
	{ EXT4_MB_USE_RESERVED,		"USE_RESV" },		\
	{ EXT4_MB_STRICT_CHECK,		"STRICT_CHECK" })

#define show_map_flags(flags) __print_flags(flags, "|",			\
	{ EXT4_GET_BLOCKS_CREATE,		"CREATE" },		\
	{ EXT4_GET_BLOCKS_UNWRIT_EXT,		"UNWRIT" },		\
	{ EXT4_GET_BLOCKS_DELALLOC_RESERVE,	"DELALLOC" },		\
	{ EXT4_GET_BLOCKS_PRE_IO,		"PRE_IO" },		\
	{ EXT4_GET_BLOCKS_CONVERT,		"CONVERT" },		\
	{ EXT4_GET_BLOCKS_METADATA_NOFAIL,	"METADATA_NOFAIL" },	\
	{ EXT4_GET_BLOCKS_NO_NORMALIZE,		"NO_NORMALIZE" },	\
	{ EXT4_GET_BLOCKS_CONVERT_UNWRITTEN,	"CONVERT_UNWRITTEN" },  \
	{ EXT4_GET_BLOCKS_ZERO,			"ZERO" },		\
	{ EXT4_GET_BLOCKS_IO_SUBMIT,		"IO_SUBMIT" },		\
	{ EXT4_EX_NOCACHE,			"EX_NOCACHE" })

/*
 * __print_flags() requires that all enum values be wrapped in the
 * TRACE_DEFINE_ENUM macro so that the enum value can be encoded in the ftrace
 * ring buffer.
 */
TRACE_DEFINE_ENUM(BH_New);
TRACE_DEFINE_ENUM(BH_Mapped);
TRACE_DEFINE_ENUM(BH_Unwritten);
TRACE_DEFINE_ENUM(BH_Boundary);

#define show_mflags(flags) __print_flags(flags, "",	\
	{ EXT4_MAP_NEW,		"N" },			\
	{ EXT4_MAP_MAPPED,	"M" },			\
	{ EXT4_MAP_UNWRITTEN,	"U" },			\
	{ EXT4_MAP_BOUNDARY,	"B" })

#define show_free_flags(flags) __print_flags(flags, "|",	\
	{ EXT4_FREE_BLOCKS_METADATA,		"METADATA" },	\
	{ EXT4_FREE_BLOCKS_FORGET,		"FORGET" },	\
	{ EXT4_FREE_BLOCKS_VALIDATED,		"VALIDATED" },	\
	{ EXT4_FREE_BLOCKS_NO_QUOT_UPDATE,	"NO_QUOTA" },	\
	{ EXT4_FREE_BLOCKS_NOFREE_FIRST_CLUSTER,"1ST_CLUSTER" },\
	{ EXT4_FREE_BLOCKS_NOFREE_LAST_CLUSTER,	"LAST_CLUSTER" })

TRACE_DEFINE_ENUM(ES_WRITTEN_B);
TRACE_DEFINE_ENUM(ES_UNWRITTEN_B);
TRACE_DEFINE_ENUM(ES_DELAYED_B);
TRACE_DEFINE_ENUM(ES_HOLE_B);
TRACE_DEFINE_ENUM(ES_REFERENCED_B);

#define show_extent_status(status) __print_flags(status, "",	\
	{ EXTENT_STATUS_WRITTEN,	"W" },			\
	{ EXTENT_STATUS_UNWRITTEN,	"U" },			\
	{ EXTENT_STATUS_DELAYED,	"D" },			\
	{ EXTENT_STATUS_HOLE,		"H" },			\
	{ EXTENT_STATUS_REFERENCED,	"R" })

#define show_falloc_mode(mode) __print_flags(mode, "|",		\
	{ FALLOC_FL_KEEP_SIZE,		"KEEP_SIZE"},		\
	{ FALLOC_FL_PUNCH_HOLE,		"PUNCH_HOLE"},		\
	{ FALLOC_FL_NO_HIDE_STALE,	"NO_HIDE_STALE"},	\
	{ FALLOC_FL_COLLAPSE_RANGE,	"COLLAPSE_RANGE"},	\
	{ FALLOC_FL_ZERO_RANGE,		"ZERO_RANGE"})

#define show_fc_reason(reason)						\
	__print_symbolic(reason,					\
		{ EXT4_FC_REASON_XATTR,		"XATTR"},		\
		{ EXT4_FC_REASON_CROSS_RENAME,	"CROSS_RENAME"},	\
		{ EXT4_FC_REASON_JOURNAL_FLAG_CHANGE, "JOURNAL_FLAG_CHANGE"}, \
		{ EXT4_FC_REASON_NOMEM,	"NO_MEM"},			\
		{ EXT4_FC_REASON_SWAP_BOOT,	"SWAP_BOOT"},		\
		{ EXT4_FC_REASON_RESIZE,	"RESIZE"},		\
		{ EXT4_FC_REASON_RENAME_DIR,	"RENAME_DIR"},		\
		{ EXT4_FC_REASON_FALLOC_RANGE,	"FALLOC_RANGE"},	\
		{ EXT4_FC_REASON_INODE_JOURNAL_DATA,	"INODE_JOURNAL_DATA"})

TRACE_EVENT(ext4_other_inode_update_time,
	TP_PROTO(struct inode *inode, ino_t orig_ino),

	TP_ARGS(inode, orig_ino),

	TP_STRUCT__entry(
		__field(	dev_t,	dev			)
		__field(	ino_t,	ino			)
		__field(	ino_t,	orig_ino		)
		__field(	uid_t,	uid			)
		__field(	gid_t,	gid			)
		__field(	__u16, mode			)
	),

	TP_fast_assign(
		__entry->orig_ino = orig_ino;
		__entry->dev	= inode->i_sb->s_dev;
		__entry->ino	= inode->i_ino;
		__entry->uid	= i_uid_read(inode);
		__entry->gid	= i_gid_read(inode);
		__entry->mode	= inode->i_mode;
	),

	TP_printk("dev %d,%d orig_ino %lu ino %lu mode 0%o uid %u gid %u",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  (unsigned long) __entry->orig_ino,
		  (unsigned long) __entry->ino, __entry->mode,
		  __entry->uid, __entry->gid)
);

TRACE_EVENT(ext4_free_inode,
	TP_PROTO(struct inode *inode),

	TP_ARGS(inode),

	TP_STRUCT__entry(
		__field(	dev_t,	dev			)
		__field(	ino_t,	ino			)
		__field(	uid_t,	uid			)
		__field(	gid_t,	gid			)
		__field(	__u64, blocks			)
		__field(	__u16, mode			)
	),

	TP_fast_assign(
		__entry->dev	= inode->i_sb->s_dev;
		__entry->ino	= inode->i_ino;
		__entry->uid	= i_uid_read(inode);
		__entry->gid	= i_gid_read(inode);
		__entry->blocks	= inode->i_blocks;
		__entry->mode	= inode->i_mode;
	),

	TP_printk("dev %d,%d ino %lu mode 0%o uid %u gid %u blocks %llu",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  (unsigned long) __entry->ino, __entry->mode,
		  __entry->uid, __entry->gid, __entry->blocks)
);

TRACE_EVENT(ext4_request_inode,
	TP_PROTO(struct inode *dir, int mode),

	TP_ARGS(dir, mode),

	TP_STRUCT__entry(
		__field(	dev_t,	dev			)
		__field(	ino_t,	dir			)
		__field(	__u16, mode			)
	),

	TP_fast_assign(
		__entry->dev	= dir->i_sb->s_dev;
		__entry->dir	= dir->i_ino;
		__entry->mode	= mode;
	),

	TP_printk("dev %d,%d dir %lu mode 0%o",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  (unsigned long) __entry->dir, __entry->mode)
);

TRACE_EVENT(ext4_allocate_inode,
	TP_PROTO(struct inode *inode, struct inode *dir, int mode),

	TP_ARGS(inode, dir, mode),

	TP_STRUCT__entry(
		__field(	dev_t,	dev			)
		__field(	ino_t,	ino			)
		__field(	ino_t,	dir			)
		__field(	__u16,	mode			)
	),

	TP_fast_assign(
		__entry->dev	= inode->i_sb->s_dev;
		__entry->ino	= inode->i_ino;
		__entry->dir	= dir->i_ino;
		__entry->mode	= mode;
	),

	TP_printk("dev %d,%d ino %lu dir %lu mode 0%o",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  (unsigned long) __entry->ino,
		  (unsigned long) __entry->dir, __entry->mode)
);

TRACE_EVENT(ext4_evict_inode,
	TP_PROTO(struct inode *inode),

	TP_ARGS(inode),

	TP_STRUCT__entry(
		__field(	dev_t,	dev			)
		__field(	ino_t,	ino			)
		__field(	int,	nlink			)
	),

	TP_fast_assign(
		__entry->dev	= inode->i_sb->s_dev;
		__entry->ino	= inode->i_ino;
		__entry->nlink	= inode->i_nlink;
	),

	TP_printk("dev %d,%d ino %lu nlink %d",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  (unsigned long) __entry->ino, __entry->nlink)
);

TRACE_EVENT(ext4_drop_inode,
	TP_PROTO(struct inode *inode, int drop),

	TP_ARGS(inode, drop),

	TP_STRUCT__entry(
		__field(	dev_t,	dev			)
		__field(	ino_t,	ino			)
		__field(	int,	drop			)
	),

	TP_fast_assign(
		__entry->dev	= inode->i_sb->s_dev;
		__entry->ino	= inode->i_ino;
		__entry->drop	= drop;
	),

	TP_printk("dev %d,%d ino %lu drop %d",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  (unsigned long) __entry->ino, __entry->drop)
);

TRACE_EVENT(ext4_nfs_commit_metadata,
	TP_PROTO(struct inode *inode),

	TP_ARGS(inode),

	TP_STRUCT__entry(
		__field(	dev_t,	dev			)
		__field(	ino_t,	ino			)
	),

	TP_fast_assign(
		__entry->dev	= inode->i_sb->s_dev;
		__entry->ino	= inode->i_ino;
	),

	TP_printk("dev %d,%d ino %lu",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  (unsigned long) __entry->ino)
);

TRACE_EVENT(ext4_mark_inode_dirty,
	TP_PROTO(struct inode *inode, unsigned long IP),

	TP_ARGS(inode, IP),

	TP_STRUCT__entry(
		__field(	dev_t,	dev			)
		__field(	ino_t,	ino			)
		__field(unsigned long,	ip			)
	),

	TP_fast_assign(
		__entry->dev	= inode->i_sb->s_dev;
		__entry->ino	= inode->i_ino;
		__entry->ip	= IP;
	),

	TP_printk("dev %d,%d ino %lu caller %pS",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  (unsigned long) __entry->ino, (void *)__entry->ip)
);

TRACE_EVENT(ext4_begin_ordered_truncate,
	TP_PROTO(struct inode *inode, loff_t new_size),

	TP_ARGS(inode, new_size),

	TP_STRUCT__entry(
		__field(	dev_t,	dev			)
		__field(	ino_t,	ino			)
		__field(	loff_t,	new_size		)
	),

	TP_fast_assign(
		__entry->dev		= inode->i_sb->s_dev;
		__entry->ino		= inode->i_ino;
		__entry->new_size	= new_size;
	),

	TP_printk("dev %d,%d ino %lu new_size %lld",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  (unsigned long) __entry->ino,
		  __entry->new_size)
);

DECLARE_EVENT_CLASS(ext4__write_begin,

	TP_PROTO(struct inode *inode, loff_t pos, unsigned int len,
		 unsigned int flags),

	TP_ARGS(inode, pos, len, flags),

	TP_STRUCT__entry(
		__field(	dev_t,	dev			)
		__field(	ino_t,	ino			)
		__field(	loff_t,	pos			)
		__field(	unsigned int, len		)
		__field(	unsigned int, flags		)
	),

	TP_fast_assign(
		__entry->dev	= inode->i_sb->s_dev;
		__entry->ino	= inode->i_ino;
		__entry->pos	= pos;
		__entry->len	= len;
		__entry->flags	= flags;
	),

	TP_printk("dev %d,%d ino %lu pos %lld len %u flags %u",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  (unsigned long) __entry->ino,
		  __entry->pos, __entry->len, __entry->flags)
);

DEFINE_EVENT(ext4__write_begin, ext4_write_begin,

	TP_PROTO(struct inode *inode, loff_t pos, unsigned int len,
		 unsigned int flags),

	TP_ARGS(inode, pos, len, flags)
);

DEFINE_EVENT(ext4__write_begin, ext4_da_write_begin,

	TP_PROTO(struct inode *inode, loff_t pos, unsigned int len,
		 unsigned int flags),

	TP_ARGS(inode, pos, len, flags)
);

DECLARE_EVENT_CLASS(ext4__write_end,
	TP_PROTO(struct inode *inode, loff_t pos, unsigned int len,
			unsigned int copied),

	TP_ARGS(inode, pos, len, copied),

	TP_STRUCT__entry(
		__field(	dev_t,	dev			)
		__field(	ino_t,	ino			)
		__field(	loff_t,	pos			)
		__field(	unsigned int, len		)
		__field(	unsigned int, copied		)
	),

	TP_fast_assign(
		__entry->dev	= inode->i_sb->s_dev;
		__entry->ino	= inode->i_ino;
		__entry->pos	= pos;
		__entry->len	= len;
		__entry->copied	= copied;
	),

	TP_printk("dev %d,%d ino %lu pos %lld len %u copied %u",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  (unsigned long) __entry->ino,
		  __entry->pos, __entry->len, __entry->copied)
);

DEFINE_EVENT(ext4__write_end, ext4_write_end,

	TP_PROTO(struct inode *inode, loff_t pos, unsigned int len,
		 unsigned int copied),

	TP_ARGS(inode, pos, len, copied)
);

DEFINE_EVENT(ext4__write_end, ext4_journalled_write_end,

	TP_PROTO(struct inode *inode, loff_t pos, unsigned int len,
		 unsigned int copied),

	TP_ARGS(inode, pos, len, copied)
);

DEFINE_EVENT(ext4__write_end, ext4_da_write_end,

	TP_PROTO(struct inode *inode, loff_t pos, unsigned int len,
		 unsigned int copied),

	TP_ARGS(inode, pos, len, copied)
);

TRACE_EVENT(ext4_writepages,
	TP_PROTO(struct inode *inode, struct writeback_control *wbc),

	TP_ARGS(inode, wbc),

	TP_STRUCT__entry(
		__field(	dev_t,	dev			)
		__field(	ino_t,	ino			)
		__field(	long,	nr_to_write		)
		__field(	long,	pages_skipped		)
		__field(	loff_t,	range_start		)
		__field(	loff_t,	range_end		)
		__field(       pgoff_t,	writeback_index		)
		__field(	int,	sync_mode		)
		__field(	char,	for_kupdate		)
		__field(	char,	range_cyclic		)
	),

	TP_fast_assign(
		__entry->dev		= inode->i_sb->s_dev;
		__entry->ino		= inode->i_ino;
		__entry->nr_to_write	= wbc->nr_to_write;
		__entry->pages_skipped	= wbc->pages_skipped;
		__entry->range_start	= wbc->range_start;
		__entry->range_end	= wbc->range_end;
		__entry->writeback_index = inode->i_mapping->writeback_index;
		__entry->sync_mode	= wbc->sync_mode;
		__entry->for_kupdate	= wbc->for_kupdate;
		__entry->range_cyclic	= wbc->range_cyclic;
	),

	TP_printk("dev %d,%d ino %lu nr_to_write %ld pages_skipped %ld "
		  "range_start %lld range_end %lld sync_mode %d "
		  "for_kupdate %d range_cyclic %d writeback_index %lu",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  (unsigned long) __entry->ino, __entry->nr_to_write,
		  __entry->pages_skipped, __entry->range_start,
		  __entry->range_end, __entry->sync_mode,
		  __entry->for_kupdate, __entry->range_cyclic,
		  (unsigned long) __entry->writeback_index)
);

TRACE_EVENT(ext4_da_write_pages,
	TP_PROTO(struct inode *inode, pgoff_t first_page,
		 struct writeback_control *wbc),

	TP_ARGS(inode, first_page, wbc),

	TP_STRUCT__entry(
		__field(	dev_t,	dev			)
		__field(	ino_t,	ino			)
		__field(      pgoff_t,	first_page		)
		__field(	 long,	nr_to_write		)
		__field(	  int,	sync_mode		)
	),

	TP_fast_assign(
		__entry->dev		= inode->i_sb->s_dev;
		__entry->ino		= inode->i_ino;
		__entry->first_page	= first_page;
		__entry->nr_to_write	= wbc->nr_to_write;
		__entry->sync_mode	= wbc->sync_mode;
	),

	TP_printk("dev %d,%d ino %lu first_page %lu nr_to_write %ld "
		  "sync_mode %d",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  (unsigned long) __entry->ino, __entry->first_page,
		  __entry->nr_to_write, __entry->sync_mode)
);

TRACE_EVENT(ext4_da_write_pages_extent,
	TP_PROTO(struct inode *inode, struct ext4_map_blocks *map),

	TP_ARGS(inode, map),

	TP_STRUCT__entry(
		__field(	dev_t,	dev			)
		__field(	ino_t,	ino			)
		__field(	__u64,	lblk			)
		__field(	__u32,	len			)
		__field(	__u32,	flags			)
	),

	TP_fast_assign(
		__entry->dev		= inode->i_sb->s_dev;
		__entry->ino		= inode->i_ino;
		__entry->lblk		= map->m_lblk;
		__entry->len		= map->m_len;
		__entry->flags		= map->m_flags;
	),

	TP_printk("dev %d,%d ino %lu lblk %llu len %u flags %s",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  (unsigned long) __entry->ino, __entry->lblk, __entry->len,
		  show_mflags(__entry->flags))
);

TRACE_EVENT(ext4_writepages_result,
	TP_PROTO(struct inode *inode, struct writeback_control *wbc,
			int ret, int pages_written),

	TP_ARGS(inode, wbc, ret, pages_written),

	TP_STRUCT__entry(
		__field(	dev_t,	dev			)
		__field(	ino_t,	ino			)
		__field(	int,	ret			)
		__field(	int,	pages_written		)
		__field(	long,	pages_skipped		)
		__field(       pgoff_t,	writeback_index		)
		__field(	int,	sync_mode		)
	),

	TP_fast_assign(
		__entry->dev		= inode->i_sb->s_dev;
		__entry->ino		= inode->i_ino;
		__entry->ret		= ret;
		__entry->pages_written	= pages_written;
		__entry->pages_skipped	= wbc->pages_skipped;
		__entry->writeback_index = inode->i_mapping->writeback_index;
		__entry->sync_mode	= wbc->sync_mode;
	),

	TP_printk("dev %d,%d ino %lu ret %d pages_written %d pages_skipped %ld "
		  "sync_mode %d writeback_index %lu",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  (unsigned long) __entry->ino, __entry->ret,
		  __entry->pages_written, __entry->pages_skipped,
		  __entry->sync_mode,
		  (unsigned long) __entry->writeback_index)
);

DECLARE_EVENT_CLASS(ext4__page_op,
	TP_PROTO(struct page *page),

	TP_ARGS(page),

	TP_STRUCT__entry(
		__field(	dev_t,	dev			)
		__field(	ino_t,	ino			)
		__field(	pgoff_t, index			)

	),

	TP_fast_assign(
		__entry->dev	= page->mapping->host->i_sb->s_dev;
		__entry->ino	= page->mapping->host->i_ino;
		__entry->index	= page->index;
	),

	TP_printk("dev %d,%d ino %lu page_index %lu",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  (unsigned long) __entry->ino,
		  (unsigned long) __entry->index)
);

DEFINE_EVENT(ext4__page_op, ext4_writepage,

	TP_PROTO(struct page *page),

	TP_ARGS(page)
);

DEFINE_EVENT(ext4__page_op, ext4_readpage,

	TP_PROTO(struct page *page),

	TP_ARGS(page)
);

DEFINE_EVENT(ext4__page_op, ext4_releasepage,

	TP_PROTO(struct page *page),

	TP_ARGS(page)
);

DECLARE_EVENT_CLASS(ext4_invalidatepage_op,
	TP_PROTO(struct page *page, unsigned int offset, unsigned int length),

	TP_ARGS(page, offset, length),

	TP_STRUCT__entry(
		__field(	dev_t,	dev			)
		__field(	ino_t,	ino			)
		__field(	pgoff_t, index			)
		__field(	unsigned int, offset		)
		__field(	unsigned int, length		)
	),

	TP_fast_assign(
		__entry->dev	= page->mapping->host->i_sb->s_dev;
		__entry->ino	= page->mapping->host->i_ino;
		__entry->index	= page->index;
		__entry->offset	= offset;
		__entry->length	= length;
	),

	TP_printk("dev %d,%d ino %lu page_index %lu offset %u length %u",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  (unsigned long) __entry->ino,
		  (unsigned long) __entry->index,
		  __entry->offset, __entry->length)
);

DEFINE_EVENT(ext4_invalidatepage_op, ext4_invalidatepage,
	TP_PROTO(struct page *page, unsigned int offset, unsigned int length),

	TP_ARGS(page, offset, length)
);

DEFINE_EVENT(ext4_invalidatepage_op, ext4_journalled_invalidatepage,
	TP_PROTO(struct page *page, unsigned int offset, unsigned int length),

	TP_ARGS(page, offset, length)
);

TRACE_EVENT(ext4_discard_blocks,
	TP_PROTO(struct super_block *sb, unsigned long long blk,
			unsigned long long count),

	TP_ARGS(sb, blk, count),

	TP_STRUCT__entry(
		__field(	dev_t,	dev			)
		__field(	__u64,	blk			)
		__field(	__u64,	count			)

	),

	TP_fast_assign(
		__entry->dev	= sb->s_dev;
		__entry->blk	= blk;
		__entry->count	= count;
	),

	TP_printk("dev %d,%d blk %llu count %llu",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  __entry->blk, __entry->count)
);

DECLARE_EVENT_CLASS(ext4__mb_new_pa,
	TP_PROTO(struct ext4_allocation_context *ac,
		 struct ext4_prealloc_space *pa),

	TP_ARGS(ac, pa),

	TP_STRUCT__entry(
		__field(	dev_t,	dev			)
		__field(	ino_t,	ino			)
		__field(	__u64,	pa_pstart		)
		__field(	__u64,	pa_lstart		)
		__field(	__u32,	pa_len			)

	),

	TP_fast_assign(
		__entry->dev		= ac->ac_sb->s_dev;
		__entry->ino		= ac->ac_inode->i_ino;
		__entry->pa_pstart	= pa->pa_pstart;
		__entry->pa_lstart	= pa->pa_lstart;
		__entry->pa_len		= pa->pa_len;
	),

	TP_printk("dev %d,%d ino %lu pstart %llu len %u lstart %llu",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  (unsigned long) __entry->ino,
		  __entry->pa_pstart, __entry->pa_len, __entry->pa_lstart)
);

DEFINE_EVENT(ext4__mb_new_pa, ext4_mb_new_inode_pa,

	TP_PROTO(struct ext4_allocation_context *ac,
		 struct ext4_prealloc_space *pa),

	TP_ARGS(ac, pa)
);

DEFINE_EVENT(ext4__mb_new_pa, ext4_mb_new_group_pa,

	TP_PROTO(struct ext4_allocation_context *ac,
		 struct ext4_prealloc_space *pa),

	TP_ARGS(ac, pa)
);

TRACE_EVENT(ext4_mb_release_inode_pa,
	TP_PROTO(struct ext4_prealloc_space *pa,
		 unsigned long long block, unsigned int count),

	TP_ARGS(pa, block, count),

	TP_STRUCT__entry(
		__field(	dev_t,	dev			)
		__field(	ino_t,	ino			)
		__field(	__u64,	block			)
		__field(	__u32,	count			)

	),

	TP_fast_assign(
		__entry->dev		= pa->pa_inode->i_sb->s_dev;
		__entry->ino		= pa->pa_inode->i_ino;
		__entry->block		= block;
		__entry->count		= count;
	),

	TP_printk("dev %d,%d ino %lu block %llu count %u",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  (unsigned long) __entry->ino,
		  __entry->block, __entry->count)
);

TRACE_EVENT(ext4_mb_release_group_pa,
	TP_PROTO(struct super_block *sb, struct ext4_prealloc_space *pa),

	TP_ARGS(sb, pa),

	TP_STRUCT__entry(
		__field(	dev_t,	dev			)
		__field(	__u64,	pa_pstart		)
		__field(	__u32,	pa_len			)

	),

	TP_fast_assign(
		__entry->dev		= sb->s_dev;
		__entry->pa_pstart	= pa->pa_pstart;
		__entry->pa_len		= pa->pa_len;
	),

	TP_printk("dev %d,%d pstart %llu len %u",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  __entry->pa_pstart, __entry->pa_len)
);

TRACE_EVENT(ext4_discard_preallocations,
	TP_PROTO(struct inode *inode, unsigned int len, unsigned int needed),

	TP_ARGS(inode, len, needed),

	TP_STRUCT__entry(
		__field(	dev_t,		dev		)
		__field(	ino_t,		ino		)
		__field(	unsigned int,	len		)
		__field(	unsigned int,	needed		)

	),

	TP_fast_assign(
		__entry->dev	= inode->i_sb->s_dev;
		__entry->ino	= inode->i_ino;
		__entry->len	= len;
		__entry->needed	= needed;
	),

	TP_printk("dev %d,%d ino %lu len: %u needed %u",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  (unsigned long) __entry->ino, __entry->len,
		  __entry->needed)
);

TRACE_EVENT(ext4_mb_discard_preallocations,
	TP_PROTO(struct super_block *sb, int needed),

	TP_ARGS(sb, needed),

	TP_STRUCT__entry(
		__field(	dev_t,	dev			)
		__field(	int,	needed			)

	),

	TP_fast_assign(
		__entry->dev	= sb->s_dev;
		__entry->needed	= needed;
	),

	TP_printk("dev %d,%d needed %d",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  __entry->needed)
);

TRACE_EVENT(ext4_request_blocks,
	TP_PROTO(struct ext4_allocation_request *ar),

	TP_ARGS(ar),

	TP_STRUCT__entry(
		__field(	dev_t,	dev			)
		__field(	ino_t,	ino			)
		__field(	unsigned int, len		)
		__field(	__u32,  logical			)
		__field(	__u32,	lleft			)
		__field(	__u32,	lright			)
		__field(	__u64,	goal			)
		__field(	__u64,	pleft			)
		__field(	__u64,	pright			)
		__field(	unsigned int, flags		)
	),

	TP_fast_assign(
		__entry->dev	= ar->inode->i_sb->s_dev;
		__entry->ino	= ar->inode->i_ino;
		__entry->len	= ar->len;
		__entry->logical = ar->logical;
		__entry->goal	= ar->goal;
		__entry->lleft	= ar->lleft;
		__entry->lright	= ar->lright;
		__entry->pleft	= ar->pleft;
		__entry->pright	= ar->pright;
		__entry->flags	= ar->flags;
	),

	TP_printk("dev %d,%d ino %lu flags %s len %u lblk %u goal %llu "
		  "lleft %u lright %u pleft %llu pright %llu ",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  (unsigned long) __entry->ino, show_mballoc_flags(__entry->flags),
		  __entry->len, __entry->logical, __entry->goal,
		  __entry->lleft, __entry->lright, __entry->pleft,
		  __entry->pright)
);

TRACE_EVENT(ext4_allocate_blocks,
	TP_PROTO(struct ext4_allocation_request *ar, unsigned long long block),

	TP_ARGS(ar, block),

	TP_STRUCT__entry(
		__field(	dev_t,	dev			)
		__field(	ino_t,	ino			)
		__field(	__u64,	block			)
		__field(	unsigned int, len		)
		__field(	__u32,  logical			)
		__field(	__u32,	lleft			)
		__field(	__u32,	lright			)
		__field(	__u64,	goal			)
		__field(	__u64,	pleft			)
		__field(	__u64,	pright			)
		__field(	unsigned int, flags		)
	),

	TP_fast_assign(
		__entry->dev	= ar->inode->i_sb->s_dev;
		__entry->ino	= ar->inode->i_ino;
		__entry->block	= block;
		__entry->len	= ar->len;
		__entry->logical = ar->logical;
		__entry->goal	= ar->goal;
		__entry->lleft	= ar->lleft;
		__entry->lright	= ar->lright;
		__entry->pleft	= ar->pleft;
		__entry->pright	= ar->pright;
		__entry->flags	= ar->flags;
	),

	TP_printk("dev %d,%d ino %lu flags %s len %u block %llu lblk %u "
		  "goal %llu lleft %u lright %u pleft %llu pright %llu",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  (unsigned long) __entry->ino, show_mballoc_flags(__entry->flags),
		  __entry->len, __entry->block, __entry->logical,
		  __entry->goal,  __entry->lleft, __entry->lright,
		  __entry->pleft, __entry->pright)
);

TRACE_EVENT(ext4_free_blocks,
	TP_PROTO(struct inode *inode, __u64 block, unsigned long count,
		 int flags),

	TP_ARGS(inode, block, count, flags),

	TP_STRUCT__entry(
		__field(	dev_t,	dev			)
		__field(	ino_t,	ino			)
		__field(	__u64,	block			)
		__field(	unsigned long,	count		)
		__field(	int,	flags			)
		__field(	__u16,	mode			)
	),

	TP_fast_assign(
		__entry->dev		= inode->i_sb->s_dev;
		__entry->ino		= inode->i_ino;
		__entry->block		= block;
		__entry->count		= count;
		__entry->flags		= flags;
		__entry->mode		= inode->i_mode;
	),

	TP_printk("dev %d,%d ino %lu mode 0%o block %llu count %lu flags %s",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  (unsigned long) __entry->ino,
		  __entry->mode, __entry->block, __entry->count,
		  show_free_flags(__entry->flags))
);

TRACE_EVENT(ext4_sync_file_enter,
	TP_PROTO(struct file *file, int datasync),

	TP_ARGS(file, datasync),

	TP_STRUCT__entry(
		__field(	dev_t,	dev			)
		__field(	ino_t,	ino			)
		__field(	ino_t,	parent			)
		__field(	int,	datasync		)
	),

	TP_fast_assign(
		struct dentry *dentry = file->f_path.dentry;

		__entry->dev		= dentry->d_sb->s_dev;
		__entry->ino		= d_inode(dentry)->i_ino;
		__entry->datasync	= datasync;
		__entry->parent		= d_inode(dentry->d_parent)->i_ino;
	),

	TP_printk("dev %d,%d ino %lu parent %lu datasync %d ",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  (unsigned long) __entry->ino,
		  (unsigned long) __entry->parent, __entry->datasync)
);

TRACE_EVENT(ext4_sync_file_exit,
	TP_PROTO(struct inode *inode, int ret),

	TP_ARGS(inode, ret),

	TP_STRUCT__entry(
		__field(	dev_t,	dev			)
		__field(	ino_t,	ino			)
		__field(	int,	ret			)
	),

	TP_fast_assign(
		__entry->dev		= inode->i_sb->s_dev;
		__entry->ino		= inode->i_ino;
		__entry->ret		= ret;
	),

	TP_printk("dev %d,%d ino %lu ret %d",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  (unsigned long) __entry->ino,
		  __entry->ret)
);

TRACE_EVENT(ext4_sync_fs,
	TP_PROTO(struct super_block *sb, int wait),

	TP_ARGS(sb, wait),

	TP_STRUCT__entry(
		__field(	dev_t,	dev			)
		__field(	int,	wait			)

	),

	TP_fast_assign(
		__entry->dev	= sb->s_dev;
		__entry->wait	= wait;
	),

	TP_printk("dev %d,%d wait %d",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  __entry->wait)
);

TRACE_EVENT(ext4_alloc_da_blocks,
	TP_PROTO(struct inode *inode),

	TP_ARGS(inode),

	TP_STRUCT__entry(
		__field(	dev_t,	dev			)
		__field(	ino_t,	ino			)
		__field( unsigned int,	data_blocks		)
	),

	TP_fast_assign(
		__entry->dev	= inode->i_sb->s_dev;
		__entry->ino	= inode->i_ino;
		__entry->data_blocks = EXT4_I(inode)->i_reserved_data_blocks;
	),

	TP_printk("dev %d,%d ino %lu reserved_data_blocks %u",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  (unsigned long) __entry->ino,
		  __entry->data_blocks)
);

TRACE_EVENT(ext4_mballoc_alloc,
	TP_PROTO(struct ext4_allocation_context *ac),

	TP_ARGS(ac),

	TP_STRUCT__entry(
		__field(	dev_t,	dev			)
		__field(	ino_t,	ino			)
		__field(	__u32, 	orig_logical		)
		__field(	  int,	orig_start		)
		__field(	__u32, 	orig_group		)
		__field(	  int,	orig_len		)
		__field(	__u32, 	goal_logical		)
		__field(	  int,	goal_start		)
		__field(	__u32, 	goal_group		)
		__field(	  int,	goal_len		)
		__field(	__u32, 	result_logical		)
		__field(	  int,	result_start		)
		__field(	__u32, 	result_group		)
		__field(	  int,	result_len		)
		__field(	__u16,	found			)
		__field(	__u16,	groups			)
		__field(	__u16,	buddy			)
		__field(	__u16,	flags			)
		__field(	__u16,	tail			)
		__field(	__u8,	cr			)
	),

	TP_fast_assign(
		__entry->dev		= ac->ac_inode->i_sb->s_dev;
		__entry->ino		= ac->ac_inode->i_ino;
		__entry->orig_logical	= ac->ac_o_ex.fe_logical;
		__entry->orig_start	= ac->ac_o_ex.fe_start;
		__entry->orig_group	= ac->ac_o_ex.fe_group;
		__entry->orig_len	= ac->ac_o_ex.fe_len;
		__entry->goal_logical	= ac->ac_g_ex.fe_logical;
		__entry->goal_start	= ac->ac_g_ex.fe_start;
		__entry->goal_group	= ac->ac_g_ex.fe_group;
		__entry->goal_len	= ac->ac_g_ex.fe_len;
		__entry->result_logical	= ac->ac_f_ex.fe_logical;
		__entry->result_start	= ac->ac_f_ex.fe_start;
		__entry->result_group	= ac->ac_f_ex.fe_group;
		__entry->result_len	= ac->ac_f_ex.fe_len;
		__entry->found		= ac->ac_found;
		__entry->flags		= ac->ac_flags;
		__entry->groups		= ac->ac_groups_scanned;
		__entry->buddy		= ac->ac_buddy;
		__entry->tail		= ac->ac_tail;
		__entry->cr		= ac->ac_criteria;
	),

	TP_printk("dev %d,%d inode %lu orig %u/%d/%u@%u goal %u/%d/%u@%u "
		  "result %u/%d/%u@%u blks %u grps %u cr %u flags %s "
		  "tail %u broken %u",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  (unsigned long) __entry->ino,
		  __entry->orig_group, __entry->orig_start,
		  __entry->orig_len, __entry->orig_logical,
		  __entry->goal_group, __entry->goal_start,
		  __entry->goal_len, __entry->goal_logical,
		  __entry->result_group, __entry->result_start,
		  __entry->result_len, __entry->result_logical,
		  __entry->found, __entry->groups, __entry->cr,
		  show_mballoc_flags(__entry->flags), __entry->tail,
		  __entry->buddy ? 1 << __entry->buddy : 0)
);

TRACE_EVENT(ext4_mballoc_prealloc,
	TP_PROTO(struct ext4_allocation_context *ac),

	TP_ARGS(ac),

	TP_STRUCT__entry(
		__field(	dev_t,	dev			)
		__field(	ino_t,	ino			)
		__field(	__u32, 	orig_logical		)
		__field(	  int,	orig_start		)
		__field(	__u32, 	orig_group		)
		__field(	  int,	orig_len		)
		__field(	__u32, 	result_logical		)
		__field(	  int,	result_start		)
		__field(	__u32, 	result_group		)
		__field(	  int,	result_len		)
	),

	TP_fast_assign(
		__entry->dev		= ac->ac_inode->i_sb->s_dev;
		__entry->ino		= ac->ac_inode->i_ino;
		__entry->orig_logical	= ac->ac_o_ex.fe_logical;
		__entry->orig_start	= ac->ac_o_ex.fe_start;
		__entry->orig_group	= ac->ac_o_ex.fe_group;
		__entry->orig_len	= ac->ac_o_ex.fe_len;
		__entry->result_logical	= ac->ac_b_ex.fe_logical;
		__entry->result_start	= ac->ac_b_ex.fe_start;
		__entry->result_group	= ac->ac_b_ex.fe_group;
		__entry->result_len	= ac->ac_b_ex.fe_len;
	),

	TP_printk("dev %d,%d inode %lu orig %u/%d/%u@%u result %u/%d/%u@%u",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  (unsigned long) __entry->ino,
		  __entry->orig_group, __entry->orig_start,
		  __entry->orig_len, __entry->orig_logical,
		  __entry->result_group, __entry->result_start,
		  __entry->result_len, __entry->result_logical)
);

DECLARE_EVENT_CLASS(ext4__mballoc,
	TP_PROTO(struct super_block *sb,
		 struct inode *inode,
		 ext4_group_t group,
		 ext4_grpblk_t start,
		 ext4_grpblk_t len),

	TP_ARGS(sb, inode, group, start, len),

	TP_STRUCT__entry(
		__field(	dev_t,	dev			)
		__field(	ino_t,	ino			)
		__field(	  int,	result_start		)
		__field(	__u32, 	result_group		)
		__field(	  int,	result_len		)
	),

	TP_fast_assign(
		__entry->dev		= sb->s_dev;
		__entry->ino		= inode ? inode->i_ino : 0;
		__entry->result_start	= start;
		__entry->result_group	= group;
		__entry->result_len	= len;
	),

	TP_printk("dev %d,%d inode %lu extent %u/%d/%d ",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  (unsigned long) __entry->ino,
		  __entry->result_group, __entry->result_start,
		  __entry->result_len)
);

DEFINE_EVENT(ext4__mballoc, ext4_mballoc_discard,

	TP_PROTO(struct super_block *sb,
		 struct inode *inode,
		 ext4_group_t group,
		 ext4_grpblk_t start,
		 ext4_grpblk_t len),

	TP_ARGS(sb, inode, group, start, len)
);

DEFINE_EVENT(ext4__mballoc, ext4_mballoc_free,

	TP_PROTO(struct super_block *sb,
		 struct inode *inode,
		 ext4_group_t group,
		 ext4_grpblk_t start,
		 ext4_grpblk_t len),

	TP_ARGS(sb, inode, group, start, len)
);

TRACE_EVENT(ext4_forget,
	TP_PROTO(struct inode *inode, int is_metadata, __u64 block),

	TP_ARGS(inode, is_metadata, block),

	TP_STRUCT__entry(
		__field(	dev_t,	dev			)
		__field(	ino_t,	ino			)
		__field(	__u64,	block			)
		__field(	int,	is_metadata		)
		__field(	__u16,	mode			)
	),

	TP_fast_assign(
		__entry->dev	= inode->i_sb->s_dev;
		__entry->ino	= inode->i_ino;
		__entry->block	= block;
		__entry->is_metadata = is_metadata;
		__entry->mode	= inode->i_mode;
	),

	TP_printk("dev %d,%d ino %lu mode 0%o is_metadata %d block %llu",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  (unsigned long) __entry->ino,
		  __entry->mode, __entry->is_metadata, __entry->block)
);

TRACE_EVENT(ext4_da_update_reserve_space,
	TP_PROTO(struct inode *inode, int used_blocks, int quota_claim),

	TP_ARGS(inode, used_blocks, quota_claim),

	TP_STRUCT__entry(
		__field(	dev_t,	dev			)
		__field(	ino_t,	ino			)
		__field(	__u64,	i_blocks		)
		__field(	int,	used_blocks		)
		__field(	int,	reserved_data_blocks	)
		__field(	int,	quota_claim		)
		__field(	__u16,	mode			)
	),

	TP_fast_assign(
		__entry->dev	= inode->i_sb->s_dev;
		__entry->ino	= inode->i_ino;
		__entry->i_blocks = inode->i_blocks;
		__entry->used_blocks = used_blocks;
		__entry->reserved_data_blocks =
				EXT4_I(inode)->i_reserved_data_blocks;
		__entry->quota_claim = quota_claim;
		__entry->mode	= inode->i_mode;
	),

	TP_printk("dev %d,%d ino %lu mode 0%o i_blocks %llu used_blocks %d "
		  "reserved_data_blocks %d quota_claim %d",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  (unsigned long) __entry->ino,
		  __entry->mode, __entry->i_blocks,
		  __entry->used_blocks, __entry->reserved_data_blocks,
		  __entry->quota_claim)
);

TRACE_EVENT(ext4_da_reserve_space,
	TP_PROTO(struct inode *inode),

	TP_ARGS(inode),

	TP_STRUCT__entry(
		__field(	dev_t,	dev			)
		__field(	ino_t,	ino			)
		__field(	__u64,	i_blocks		)
		__field(	int,	reserved_data_blocks	)
		__field(	__u16,  mode			)
	),

	TP_fast_assign(
		__entry->dev	= inode->i_sb->s_dev;
		__entry->ino	= inode->i_ino;
		__entry->i_blocks = inode->i_blocks;
		__entry->reserved_data_blocks = EXT4_I(inode)->i_reserved_data_blocks;
		__entry->mode	= inode->i_mode;
	),

	TP_printk("dev %d,%d ino %lu mode 0%o i_blocks %llu "
		  "reserved_data_blocks %d",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  (unsigned long) __entry->ino,
		  __entry->mode, __entry->i_blocks,
		  __entry->reserved_data_blocks)
);

TRACE_EVENT(ext4_da_release_space,
	TP_PROTO(struct inode *inode, int freed_blocks),

	TP_ARGS(inode, freed_blocks),

	TP_STRUCT__entry(
		__field(	dev_t,	dev			)
		__field(	ino_t,	ino			)
		__field(	__u64,	i_blocks		)
		__field(	int,	freed_blocks		)
		__field(	int,	reserved_data_blocks	)
		__field(	__u16,  mode			)
	),

	TP_fast_assign(
		__entry->dev	= inode->i_sb->s_dev;
		__entry->ino	= inode->i_ino;
		__entry->i_blocks = inode->i_blocks;
		__entry->freed_blocks = freed_blocks;
		__entry->reserved_data_blocks = EXT4_I(inode)->i_reserved_data_blocks;
		__entry->mode	= inode->i_mode;
	),

	TP_printk("dev %d,%d ino %lu mode 0%o i_blocks %llu freed_blocks %d "
		  "reserved_data_blocks %d",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  (unsigned long) __entry->ino,
		  __entry->mode, __entry->i_blocks,
		  __entry->freed_blocks, __entry->reserved_data_blocks)
);

DECLARE_EVENT_CLASS(ext4__bitmap_load,
	TP_PROTO(struct super_block *sb, unsigned long group),

	TP_ARGS(sb, group),

	TP_STRUCT__entry(
		__field(	dev_t,	dev			)
		__field(	__u32,	group			)

	),

	TP_fast_assign(
		__entry->dev	= sb->s_dev;
		__entry->group	= group;
	),

	TP_printk("dev %d,%d group %u",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  __entry->group)
);

DEFINE_EVENT(ext4__bitmap_load, ext4_mb_bitmap_load,

	TP_PROTO(struct super_block *sb, unsigned long group),

	TP_ARGS(sb, group)
);

DEFINE_EVENT(ext4__bitmap_load, ext4_mb_buddy_bitmap_load,

	TP_PROTO(struct super_block *sb, unsigned long group),

	TP_ARGS(sb, group)
);

DEFINE_EVENT(ext4__bitmap_load, ext4_load_inode_bitmap,

	TP_PROTO(struct super_block *sb, unsigned long group),

	TP_ARGS(sb, group)
);

TRACE_EVENT(ext4_read_block_bitmap_load,
	TP_PROTO(struct super_block *sb, unsigned long group, bool prefetch),

	TP_ARGS(sb, group, prefetch),

	TP_STRUCT__entry(
		__field(	dev_t,	dev			)
		__field(	__u32,	group			)
		__field(	bool,	prefetch		)

	),

	TP_fast_assign(
		__entry->dev	= sb->s_dev;
		__entry->group	= group;
		__entry->prefetch = prefetch;
	),

	TP_printk("dev %d,%d group %u prefetch %d",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  __entry->group, __entry->prefetch)
);

DECLARE_EVENT_CLASS(ext4__fallocate_mode,
	TP_PROTO(struct inode *inode, loff_t offset, loff_t len, int mode),

	TP_ARGS(inode, offset, len, mode),

	TP_STRUCT__entry(
		__field(	dev_t,	dev			)
		__field(	ino_t,	ino			)
		__field(	loff_t,	offset			)
		__field(	loff_t, len			)
		__field(	int,	mode			)
	),

	TP_fast_assign(
		__entry->dev	= inode->i_sb->s_dev;
		__entry->ino	= inode->i_ino;
		__entry->offset	= offset;
		__entry->len	= len;
		__entry->mode	= mode;
	),

	TP_printk("dev %d,%d ino %lu offset %lld len %lld mode %s",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  (unsigned long) __entry->ino,
		  __entry->offset, __entry->len,
		  show_falloc_mode(__entry->mode))
);

DEFINE_EVENT(ext4__fallocate_mode, ext4_fallocate_enter,

	TP_PROTO(struct inode *inode, loff_t offset, loff_t len, int mode),

	TP_ARGS(inode, offset, len, mode)
);

DEFINE_EVENT(ext4__fallocate_mode, ext4_punch_hole,

	TP_PROTO(struct inode *inode, loff_t offset, loff_t len, int mode),

	TP_ARGS(inode, offset, len, mode)
);

DEFINE_EVENT(ext4__fallocate_mode, ext4_zero_range,

	TP_PROTO(struct inode *inode, loff_t offset, loff_t len, int mode),

	TP_ARGS(inode, offset, len, mode)
);

TRACE_EVENT(ext4_fallocate_exit,
	TP_PROTO(struct inode *inode, loff_t offset,
		 unsigned int max_blocks, int ret),

	TP_ARGS(inode, offset, max_blocks, ret),

	TP_STRUCT__entry(
		__field(	dev_t,	dev			)
		__field(	ino_t,	ino			)
		__field(	loff_t,	pos			)
		__field(	unsigned int,	blocks		)
		__field(	int, 	ret			)
	),

	TP_fast_assign(
		__entry->dev	= inode->i_sb->s_dev;
		__entry->ino	= inode->i_ino;
		__entry->pos	= offset;
		__entry->blocks	= max_blocks;
		__entry->ret	= ret;
	),

	TP_printk("dev %d,%d ino %lu pos %lld blocks %u ret %d",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  (unsigned long) __entry->ino,
		  __entry->pos, __entry->blocks,
		  __entry->ret)
);

TRACE_EVENT(ext4_unlink_enter,
	TP_PROTO(struct inode *parent, struct dentry *dentry),

	TP_ARGS(parent, dentry),

	TP_STRUCT__entry(
		__field(	dev_t,	dev			)
		__field(	ino_t,	ino			)
		__field(	ino_t,	parent			)
		__field(	loff_t,	size			)
	),

	TP_fast_assign(
		__entry->dev		= dentry->d_sb->s_dev;
		__entry->ino		= d_inode(dentry)->i_ino;
		__entry->parent		= parent->i_ino;
		__entry->size		= d_inode(dentry)->i_size;
	),

	TP_printk("dev %d,%d ino %lu size %lld parent %lu",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  (unsigned long) __entry->ino, __entry->size,
		  (unsigned long) __entry->parent)
);

TRACE_EVENT(ext4_unlink_exit,
	TP_PROTO(struct dentry *dentry, int ret),

	TP_ARGS(dentry, ret),

	TP_STRUCT__entry(
		__field(	dev_t,	dev			)
		__field(	ino_t,	ino			)
		__field(	int,	ret			)
	),

	TP_fast_assign(
		__entry->dev		= dentry->d_sb->s_dev;
		__entry->ino		= d_inode(dentry)->i_ino;
		__entry->ret		= ret;
	),

	TP_printk("dev %d,%d ino %lu ret %d",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  (unsigned long) __entry->ino,
		  __entry->ret)
);

DECLARE_EVENT_CLASS(ext4__truncate,
	TP_PROTO(struct inode *inode),

	TP_ARGS(inode),

	TP_STRUCT__entry(
		__field(	dev_t,		dev		)
		__field(	ino_t,		ino		)
		__field(	__u64,		blocks		)
	),

	TP_fast_assign(
		__entry->dev    = inode->i_sb->s_dev;
		__entry->ino    = inode->i_ino;
		__entry->blocks	= inode->i_blocks;
	),

	TP_printk("dev %d,%d ino %lu blocks %llu",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  (unsigned long) __entry->ino, __entry->blocks)
);

DEFINE_EVENT(ext4__truncate, ext4_truncate_enter,

	TP_PROTO(struct inode *inode),

	TP_ARGS(inode)
);

DEFINE_EVENT(ext4__truncate, ext4_truncate_exit,

	TP_PROTO(struct inode *inode),

	TP_ARGS(inode)
);

/* 'ux' is the unwritten extent. */
TRACE_EVENT(ext4_ext_convert_to_initialized_enter,
	TP_PROTO(struct inode *inode, struct ext4_map_blocks *map,
		 struct ext4_extent *ux),

	TP_ARGS(inode, map, ux),

	TP_STRUCT__entry(
		__field(	dev_t,		dev	)
		__field(	ino_t,		ino	)
		__field(	ext4_lblk_t,	m_lblk	)
		__field(	unsigned,	m_len	)
		__field(	ext4_lblk_t,	u_lblk	)
		__field(	unsigned,	u_len	)
		__field(	ext4_fsblk_t,	u_pblk	)
	),

	TP_fast_assign(
		__entry->dev		= inode->i_sb->s_dev;
		__entry->ino		= inode->i_ino;
		__entry->m_lblk		= map->m_lblk;
		__entry->m_len		= map->m_len;
		__entry->u_lblk		= le32_to_cpu(ux->ee_block);
		__entry->u_len		= ext4_ext_get_actual_len(ux);
		__entry->u_pblk		= ext4_ext_pblock(ux);
	),

	TP_printk("dev %d,%d ino %lu m_lblk %u m_len %u u_lblk %u u_len %u "
		  "u_pblk %llu",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  (unsigned long) __entry->ino,
		  __entry->m_lblk, __entry->m_len,
		  __entry->u_lblk, __entry->u_len, __entry->u_pblk)
);

/*
 * 'ux' is the unwritten extent.
 * 'ix' is the initialized extent to which blocks are transferred.
 */
TRACE_EVENT(ext4_ext_convert_to_initialized_fastpath,
	TP_PROTO(struct inode *inode, struct ext4_map_blocks *map,
		 struct ext4_extent *ux, struct ext4_extent *ix),

	TP_ARGS(inode, map, ux, ix),

	TP_STRUCT__entry(
		__field(	dev_t,		dev	)
		__field(	ino_t,		ino	)
		__field(	ext4_lblk_t,	m_lblk	)
		__field(	unsigned,	m_len	)
		__field(	ext4_lblk_t,	u_lblk	)
		__field(	unsigned,	u_len	)
		__field(	ext4_fsblk_t,	u_pblk	)
		__field(	ext4_lblk_t,	i_lblk	)
		__field(	unsigned,	i_len	)
		__field(	ext4_fsblk_t,	i_pblk	)
	),

	TP_fast_assign(
		__entry->dev		= inode->i_sb->s_dev;
		__entry->ino		= inode->i_ino;
		__entry->m_lblk		= map->m_lblk;
		__entry->m_len		= map->m_len;
		__entry->u_lblk		= le32_to_cpu(ux->ee_block);
		__entry->u_len		= ext4_ext_get_actual_len(ux);
		__entry->u_pblk		= ext4_ext_pblock(ux);
		__entry->i_lblk		= le32_to_cpu(ix->ee_block);
		__entry->i_len		= ext4_ext_get_actual_len(ix);
		__entry->i_pblk		= ext4_ext_pblock(ix);
	),

	TP_printk("dev %d,%d ino %lu m_lblk %u m_len %u "
		  "u_lblk %u u_len %u u_pblk %llu "
		  "i_lblk %u i_len %u i_pblk %llu ",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  (unsigned long) __entry->ino,
		  __entry->m_lblk, __entry->m_len,
		  __entry->u_lblk, __entry->u_len, __entry->u_pblk,
		  __entry->i_lblk, __entry->i_len, __entry->i_pblk)
);

DECLARE_EVENT_CLASS(ext4__map_blocks_enter,
	TP_PROTO(struct inode *inode, ext4_lblk_t lblk,
		 unsigned int len, unsigned int flags),

	TP_ARGS(inode, lblk, len, flags),

	TP_STRUCT__entry(
		__field(	dev_t,		dev		)
		__field(	ino_t,		ino		)
		__field(	ext4_lblk_t,	lblk		)
		__field(	unsigned int,	len		)
		__field(	unsigned int,	flags		)
	),

	TP_fast_assign(
		__entry->dev    = inode->i_sb->s_dev;
		__entry->ino    = inode->i_ino;
		__entry->lblk	= lblk;
		__entry->len	= len;
		__entry->flags	= flags;
	),

	TP_printk("dev %d,%d ino %lu lblk %u len %u flags %s",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  (unsigned long) __entry->ino,
		  __entry->lblk, __entry->len, show_map_flags(__entry->flags))
);

DEFINE_EVENT(ext4__map_blocks_enter, ext4_ext_map_blocks_enter,
	TP_PROTO(struct inode *inode, ext4_lblk_t lblk,
		 unsigned len, unsigned flags),

	TP_ARGS(inode, lblk, len, flags)
);

DEFINE_EVENT(ext4__map_blocks_enter, ext4_ind_map_blocks_enter,
	TP_PROTO(struct inode *inode, ext4_lblk_t lblk,
		 unsigned len, unsigned flags),

	TP_ARGS(inode, lblk, len, flags)
);

DECLARE_EVENT_CLASS(ext4__map_blocks_exit,
	TP_PROTO(struct inode *inode, unsigned flags, struct ext4_map_blocks *map,
		 int ret),

	TP_ARGS(inode, flags, map, ret),

	TP_STRUCT__entry(
		__field(	dev_t,		dev		)
		__field(	ino_t,		ino		)
		__field(	unsigned int,	flags		)
		__field(	ext4_fsblk_t,	pblk		)
		__field(	ext4_lblk_t,	lblk		)
		__field(	unsigned int,	len		)
		__field(	unsigned int,	mflags		)
		__field(	int,		ret		)
	),

	TP_fast_assign(
		__entry->dev    = inode->i_sb->s_dev;
		__entry->ino    = inode->i_ino;
		__entry->flags	= flags;
		__entry->pblk	= map->m_pblk;
		__entry->lblk	= map->m_lblk;
		__entry->len	= map->m_len;
		__entry->mflags	= map->m_flags;
		__entry->ret	= ret;
	),

	TP_printk("dev %d,%d ino %lu flags %s lblk %u pblk %llu len %u "
		  "mflags %s ret %d",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  (unsigned long) __entry->ino,
		  show_map_flags(__entry->flags), __entry->lblk, __entry->pblk,
		  __entry->len, show_mflags(__entry->mflags), __entry->ret)
);

DEFINE_EVENT(ext4__map_blocks_exit, ext4_ext_map_blocks_exit,
	TP_PROTO(struct inode *inode, unsigned flags,
		 struct ext4_map_blocks *map, int ret),

	TP_ARGS(inode, flags, map, ret)
);

DEFINE_EVENT(ext4__map_blocks_exit, ext4_ind_map_blocks_exit,
	TP_PROTO(struct inode *inode, unsigned flags,
		 struct ext4_map_blocks *map, int ret),

	TP_ARGS(inode, flags, map, ret)
);

TRACE_EVENT(ext4_ext_load_extent,
	TP_PROTO(struct inode *inode, ext4_lblk_t lblk, ext4_fsblk_t pblk),

	TP_ARGS(inode, lblk, pblk),

	TP_STRUCT__entry(
		__field(	dev_t,		dev		)
		__field(	ino_t,		ino		)
		__field(	ext4_fsblk_t,	pblk		)
		__field(	ext4_lblk_t,	lblk		)
	),

	TP_fast_assign(
		__entry->dev    = inode->i_sb->s_dev;
		__entry->ino    = inode->i_ino;
		__entry->pblk	= pblk;
		__entry->lblk	= lblk;
	),

	TP_printk("dev %d,%d ino %lu lblk %u pblk %llu",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  (unsigned long) __entry->ino,
		  __entry->lblk, __entry->pblk)
);

TRACE_EVENT(ext4_load_inode,
	TP_PROTO(struct super_block *sb, unsigned long ino),

	TP_ARGS(sb, ino),

	TP_STRUCT__entry(
		__field(	dev_t,	dev		)
		__field(	ino_t,	ino		)
	),

	TP_fast_assign(
		__entry->dev		= sb->s_dev;
		__entry->ino		= ino;
	),

	TP_printk("dev %d,%d ino %ld",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  (unsigned long) __entry->ino)
);

TRACE_EVENT(ext4_journal_start,
	TP_PROTO(struct super_block *sb, int blocks, int rsv_blocks,
		 int revoke_creds, unsigned long IP),

	TP_ARGS(sb, blocks, rsv_blocks, revoke_creds, IP),

	TP_STRUCT__entry(
		__field(	dev_t,	dev			)
		__field(unsigned long,	ip			)
		__field(	  int,	blocks			)
		__field(	  int,	rsv_blocks		)
		__field(	  int,	revoke_creds		)
	),

	TP_fast_assign(
		__entry->dev		 = sb->s_dev;
		__entry->ip		 = IP;
		__entry->blocks		 = blocks;
		__entry->rsv_blocks	 = rsv_blocks;
		__entry->revoke_creds	 = revoke_creds;
	),

	TP_printk("dev %d,%d blocks %d, rsv_blocks %d, revoke_creds %d, "
		  "caller %pS", MAJOR(__entry->dev), MINOR(__entry->dev),
		  __entry->blocks, __entry->rsv_blocks, __entry->revoke_creds,
		  (void *)__entry->ip)
);

TRACE_EVENT(ext4_journal_start_reserved,
	TP_PROTO(struct super_block *sb, int blocks, unsigned long IP),

	TP_ARGS(sb, blocks, IP),

	TP_STRUCT__entry(
		__field(	dev_t,	dev			)
		__field(unsigned long,	ip			)
		__field(	  int,	blocks			)
	),

	TP_fast_assign(
		__entry->dev		 = sb->s_dev;
		__entry->ip		 = IP;
		__entry->blocks		 = blocks;
	),

	TP_printk("dev %d,%d blocks, %d caller %pS",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  __entry->blocks, (void *)__entry->ip)
);

DECLARE_EVENT_CLASS(ext4__trim,
	TP_PROTO(struct super_block *sb,
		 ext4_group_t group,
		 ext4_grpblk_t start,
		 ext4_grpblk_t len),

	TP_ARGS(sb, group, start, len),

	TP_STRUCT__entry(
		__field(	int,	dev_major		)
		__field(	int,	dev_minor		)
		__field(	__u32, 	group			)
		__field(	int,	start			)
		__field(	int,	len			)
	),

	TP_fast_assign(
		__entry->dev_major	= MAJOR(sb->s_dev);
		__entry->dev_minor	= MINOR(sb->s_dev);
		__entry->group		= group;
		__entry->start		= start;
		__entry->len		= len;
	),

	TP_printk("dev %d,%d group %u, start %d, len %d",
		  __entry->dev_major, __entry->dev_minor,
		  __entry->group, __entry->start, __entry->len)
);

DEFINE_EVENT(ext4__trim, ext4_trim_extent,

	TP_PROTO(struct super_block *sb,
		 ext4_group_t group,
		 ext4_grpblk_t start,
		 ext4_grpblk_t len),

	TP_ARGS(sb, group, start, len)
);

DEFINE_EVENT(ext4__trim, ext4_trim_all_free,

	TP_PROTO(struct super_block *sb,
		 ext4_group_t group,
		 ext4_grpblk_t start,
		 ext4_grpblk_t len),

	TP_ARGS(sb, group, start, len)
);

TRACE_EVENT(ext4_ext_handle_unwritten_extents,
	TP_PROTO(struct inode *inode, struct ext4_map_blocks *map, int flags,
		 unsigned int allocated, ext4_fsblk_t newblock),

	TP_ARGS(inode, map, flags, allocated, newblock),

	TP_STRUCT__entry(
		__field(	dev_t,		dev		)
		__field(	ino_t,		ino		)
		__field(	int,		flags		)
		__field(	ext4_lblk_t,	lblk		)
		__field(	ext4_fsblk_t,	pblk		)
		__field(	unsigned int,	len		)
		__field(	unsigned int,	allocated	)
		__field(	ext4_fsblk_t,	newblk		)
	),

	TP_fast_assign(
		__entry->dev		= inode->i_sb->s_dev;
		__entry->ino		= inode->i_ino;
		__entry->flags		= flags;
		__entry->lblk		= map->m_lblk;
		__entry->pblk		= map->m_pblk;
		__entry->len		= map->m_len;
		__entry->allocated	= allocated;
		__entry->newblk		= newblock;
	),

	TP_printk("dev %d,%d ino %lu m_lblk %u m_pblk %llu m_len %u flags %s "
		  "allocated %d newblock %llu",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  (unsigned long) __entry->ino,
		  (unsigned) __entry->lblk, (unsigned long long) __entry->pblk,
		  __entry->len, show_map_flags(__entry->flags),
		  (unsigned int) __entry->allocated,
		  (unsigned long long) __entry->newblk)
);

TRACE_EVENT(ext4_get_implied_cluster_alloc_exit,
	TP_PROTO(struct super_block *sb, struct ext4_map_blocks *map, int ret),

	TP_ARGS(sb, map, ret),

	TP_STRUCT__entry(
		__field(	dev_t,		dev	)
		__field(	unsigned int,	flags	)
		__field(	ext4_lblk_t,	lblk	)
		__field(	ext4_fsblk_t,	pblk	)
		__field(	unsigned int,	len	)
		__field(	int,		ret	)
	),

	TP_fast_assign(
		__entry->dev	= sb->s_dev;
		__entry->flags	= map->m_flags;
		__entry->lblk	= map->m_lblk;
		__entry->pblk	= map->m_pblk;
		__entry->len	= map->m_len;
		__entry->ret	= ret;
	),

	TP_printk("dev %d,%d m_lblk %u m_pblk %llu m_len %u m_flags %s ret %d",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  __entry->lblk, (unsigned long long) __entry->pblk,
		  __entry->len, show_mflags(__entry->flags), __entry->ret)
);

TRACE_EVENT(ext4_ext_show_extent,
	TP_PROTO(struct inode *inode, ext4_lblk_t lblk, ext4_fsblk_t pblk,
		 unsigned short len),

	TP_ARGS(inode, lblk, pblk, len),

	TP_STRUCT__entry(
		__field(	dev_t,		dev	)
		__field(	ino_t,		ino	)
		__field(	ext4_fsblk_t,	pblk	)
		__field(	ext4_lblk_t,	lblk	)
		__field(	unsigned short,	len	)
	),

	TP_fast_assign(
		__entry->dev	= inode->i_sb->s_dev;
		__entry->ino	= inode->i_ino;
		__entry->pblk	= pblk;
		__entry->lblk	= lblk;
		__entry->len	= len;
	),

	TP_printk("dev %d,%d ino %lu lblk %u pblk %llu len %u",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  (unsigned long) __entry->ino,
		  (unsigned) __entry->lblk,
		  (unsigned long long) __entry->pblk,
		  (unsigned short) __entry->len)
);

TRACE_EVENT(ext4_remove_blocks,
	TP_PROTO(struct inode *inode, struct ext4_extent *ex,
		 ext4_lblk_t from, ext4_fsblk_t to,
		 struct partial_cluster *pc),

	TP_ARGS(inode, ex, from, to, pc),

	TP_STRUCT__entry(
		__field(	dev_t,		dev	)
		__field(	ino_t,		ino	)
		__field(	ext4_lblk_t,	from	)
		__field(	ext4_lblk_t,	to	)
		__field(	ext4_fsblk_t,	ee_pblk	)
		__field(	ext4_lblk_t,	ee_lblk	)
		__field(	unsigned short,	ee_len	)
		__field(	ext4_fsblk_t,	pc_pclu	)
		__field(	ext4_lblk_t,	pc_lblk	)
		__field(	int,		pc_state)
	),

	TP_fast_assign(
		__entry->dev		= inode->i_sb->s_dev;
		__entry->ino		= inode->i_ino;
		__entry->from		= from;
		__entry->to		= to;
		__entry->ee_pblk	= ext4_ext_pblock(ex);
		__entry->ee_lblk	= le32_to_cpu(ex->ee_block);
		__entry->ee_len		= ext4_ext_get_actual_len(ex);
		__entry->pc_pclu	= pc->pclu;
		__entry->pc_lblk	= pc->lblk;
		__entry->pc_state	= pc->state;
	),

	TP_printk("dev %d,%d ino %lu extent [%u(%llu), %u]"
		  "from %u to %u partial [pclu %lld lblk %u state %d]",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  (unsigned long) __entry->ino,
		  (unsigned) __entry->ee_lblk,
		  (unsigned long long) __entry->ee_pblk,
		  (unsigned short) __entry->ee_len,
		  (unsigned) __entry->from,
		  (unsigned) __entry->to,
		  (long long) __entry->pc_pclu,
		  (unsigned int) __entry->pc_lblk,
		  (int) __entry->pc_state)
);

TRACE_EVENT(ext4_ext_rm_leaf,
	TP_PROTO(struct inode *inode, ext4_lblk_t start,
		 struct ext4_extent *ex,
		 struct partial_cluster *pc),

	TP_ARGS(inode, start, ex, pc),

	TP_STRUCT__entry(
		__field(	dev_t,		dev	)
		__field(	ino_t,		ino	)
		__field(	ext4_lblk_t,	start	)
		__field(	ext4_lblk_t,	ee_lblk	)
		__field(	ext4_fsblk_t,	ee_pblk	)
		__field(	short,		ee_len	)
		__field(	ext4_fsblk_t,	pc_pclu	)
		__field(	ext4_lblk_t,	pc_lblk	)
		__field(	int,		pc_state)
	),

	TP_fast_assign(
		__entry->dev		= inode->i_sb->s_dev;
		__entry->ino		= inode->i_ino;
		__entry->start		= start;
		__entry->ee_lblk	= le32_to_cpu(ex->ee_block);
		__entry->ee_pblk	= ext4_ext_pblock(ex);
		__entry->ee_len		= ext4_ext_get_actual_len(ex);
		__entry->pc_pclu	= pc->pclu;
		__entry->pc_lblk	= pc->lblk;
		__entry->pc_state	= pc->state;
	),

	TP_printk("dev %d,%d ino %lu start_lblk %u last_extent [%u(%llu), %u]"
		  "partial [pclu %lld lblk %u state %d]",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  (unsigned long) __entry->ino,
		  (unsigned) __entry->start,
		  (unsigned) __entry->ee_lblk,
		  (unsigned long long) __entry->ee_pblk,
		  (unsigned short) __entry->ee_len,
		  (long long) __entry->pc_pclu,
		  (unsigned int) __entry->pc_lblk,
		  (int) __entry->pc_state)
);

TRACE_EVENT(ext4_ext_rm_idx,
	TP_PROTO(struct inode *inode, ext4_fsblk_t pblk),

	TP_ARGS(inode, pblk),

	TP_STRUCT__entry(
		__field(	dev_t,		dev	)
		__field(	ino_t,		ino	)
		__field(	ext4_fsblk_t,	pblk	)
	),

	TP_fast_assign(
		__entry->dev	= inode->i_sb->s_dev;
		__entry->ino	= inode->i_ino;
		__entry->pblk	= pblk;
	),

	TP_printk("dev %d,%d ino %lu index_pblk %llu",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  (unsigned long) __entry->ino,
		  (unsigned long long) __entry->pblk)
);

TRACE_EVENT(ext4_ext_remove_space,
	TP_PROTO(struct inode *inode, ext4_lblk_t start,
		 ext4_lblk_t end, int depth),

	TP_ARGS(inode, start, end, depth),

	TP_STRUCT__entry(
		__field(	dev_t,		dev	)
		__field(	ino_t,		ino	)
		__field(	ext4_lblk_t,	start	)
		__field(	ext4_lblk_t,	end	)
		__field(	int,		depth	)
	),

	TP_fast_assign(
		__entry->dev	= inode->i_sb->s_dev;
		__entry->ino	= inode->i_ino;
		__entry->start	= start;
		__entry->end	= end;
		__entry->depth	= depth;
	),

	TP_printk("dev %d,%d ino %lu since %u end %u depth %d",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  (unsigned long) __entry->ino,
		  (unsigned) __entry->start,
		  (unsigned) __entry->end,
		  __entry->depth)
);

TRACE_EVENT(ext4_ext_remove_space_done,
	TP_PROTO(struct inode *inode, ext4_lblk_t start, ext4_lblk_t end,
		 int depth, struct partial_cluster *pc, __le16 eh_entries),

	TP_ARGS(inode, start, end, depth, pc, eh_entries),

	TP_STRUCT__entry(
		__field(	dev_t,		dev		)
		__field(	ino_t,		ino		)
		__field(	ext4_lblk_t,	start		)
		__field(	ext4_lblk_t,	end		)
		__field(	int,		depth		)
		__field(	ext4_fsblk_t,	pc_pclu		)
		__field(	ext4_lblk_t,	pc_lblk		)
		__field(	int,		pc_state	)
		__field(	unsigned short,	eh_entries	)
	),

	TP_fast_assign(
		__entry->dev		= inode->i_sb->s_dev;
		__entry->ino		= inode->i_ino;
		__entry->start		= start;
		__entry->end		= end;
		__entry->depth		= depth;
		__entry->pc_pclu	= pc->pclu;
		__entry->pc_lblk	= pc->lblk;
		__entry->pc_state	= pc->state;
		__entry->eh_entries	= le16_to_cpu(eh_entries);
	),

	TP_printk("dev %d,%d ino %lu since %u end %u depth %d "
		  "partial [pclu %lld lblk %u state %d] "
		  "remaining_entries %u",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  (unsigned long) __entry->ino,
		  (unsigned) __entry->start,
		  (unsigned) __entry->end,
		  __entry->depth,
		  (long long) __entry->pc_pclu,
		  (unsigned int) __entry->pc_lblk,
		  (int) __entry->pc_state,
		  (unsigned short) __entry->eh_entries)
);

DECLARE_EVENT_CLASS(ext4__es_extent,
	TP_PROTO(struct inode *inode, struct extent_status *es),

	TP_ARGS(inode, es),

	TP_STRUCT__entry(
		__field(	dev_t,		dev		)
		__field(	ino_t,		ino		)
		__field(	ext4_lblk_t,	lblk		)
		__field(	ext4_lblk_t,	len		)
		__field(	ext4_fsblk_t,	pblk		)
		__field(	char, status	)
	),

	TP_fast_assign(
		__entry->dev	= inode->i_sb->s_dev;
		__entry->ino	= inode->i_ino;
		__entry->lblk	= es->es_lblk;
		__entry->len	= es->es_len;
		__entry->pblk	= ext4_es_show_pblock(es);
		__entry->status	= ext4_es_status(es);
	),

	TP_printk("dev %d,%d ino %lu es [%u/%u) mapped %llu status %s",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  (unsigned long) __entry->ino,
		  __entry->lblk, __entry->len,
		  __entry->pblk, show_extent_status(__entry->status))
);

DEFINE_EVENT(ext4__es_extent, ext4_es_insert_extent,
	TP_PROTO(struct inode *inode, struct extent_status *es),

	TP_ARGS(inode, es)
);

DEFINE_EVENT(ext4__es_extent, ext4_es_cache_extent,
	TP_PROTO(struct inode *inode, struct extent_status *es),

	TP_ARGS(inode, es)
);

TRACE_EVENT(ext4_es_remove_extent,
	TP_PROTO(struct inode *inode, ext4_lblk_t lblk, ext4_lblk_t len),

	TP_ARGS(inode, lblk, len),

	TP_STRUCT__entry(
		__field(	dev_t,	dev			)
		__field(	ino_t,	ino			)
		__field(	loff_t,	lblk			)
		__field(	loff_t,	len			)
	),

	TP_fast_assign(
		__entry->dev	= inode->i_sb->s_dev;
		__entry->ino	= inode->i_ino;
		__entry->lblk	= lblk;
		__entry->len	= len;
	),

	TP_printk("dev %d,%d ino %lu es [%lld/%lld)",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  (unsigned long) __entry->ino,
		  __entry->lblk, __entry->len)
);

TRACE_EVENT(ext4_es_find_extent_range_enter,
	TP_PROTO(struct inode *inode, ext4_lblk_t lblk),

	TP_ARGS(inode, lblk),

	TP_STRUCT__entry(
		__field(	dev_t,		dev		)
		__field(	ino_t,		ino		)
		__field(	ext4_lblk_t,	lblk		)
	),

	TP_fast_assign(
		__entry->dev	= inode->i_sb->s_dev;
		__entry->ino	= inode->i_ino;
		__entry->lblk	= lblk;
	),

	TP_printk("dev %d,%d ino %lu lblk %u",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  (unsigned long) __entry->ino, __entry->lblk)
);

TRACE_EVENT(ext4_es_find_extent_range_exit,
	TP_PROTO(struct inode *inode, struct extent_status *es),

	TP_ARGS(inode, es),

	TP_STRUCT__entry(
		__field(	dev_t,		dev		)
		__field(	ino_t,		ino		)
		__field(	ext4_lblk_t,	lblk		)
		__field(	ext4_lblk_t,	len		)
		__field(	ext4_fsblk_t,	pblk		)
		__field(	char, status	)
	),

	TP_fast_assign(
		__entry->dev	= inode->i_sb->s_dev;
		__entry->ino	= inode->i_ino;
		__entry->lblk	= es->es_lblk;
		__entry->len	= es->es_len;
		__entry->pblk	= ext4_es_show_pblock(es);
		__entry->status	= ext4_es_status(es);
	),

	TP_printk("dev %d,%d ino %lu es [%u/%u) mapped %llu status %s",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  (unsigned long) __entry->ino,
		  __entry->lblk, __entry->len,
		  __entry->pblk, show_extent_status(__entry->status))
);

TRACE_EVENT(ext4_es_lookup_extent_enter,
	TP_PROTO(struct inode *inode, ext4_lblk_t lblk),

	TP_ARGS(inode, lblk),

	TP_STRUCT__entry(
		__field(	dev_t,		dev		)
		__field(	ino_t,		ino		)
		__field(	ext4_lblk_t,	lblk		)
	),

	TP_fast_assign(
		__entry->dev	= inode->i_sb->s_dev;
		__entry->ino	= inode->i_ino;
		__entry->lblk	= lblk;
	),

	TP_printk("dev %d,%d ino %lu lblk %u",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  (unsigned long) __entry->ino, __entry->lblk)
);

TRACE_EVENT(ext4_es_lookup_extent_exit,
	TP_PROTO(struct inode *inode, struct extent_status *es,
		 int found),

	TP_ARGS(inode, es, found),

	TP_STRUCT__entry(
		__field(	dev_t,		dev		)
		__field(	ino_t,		ino		)
		__field(	ext4_lblk_t,	lblk		)
		__field(	ext4_lblk_t,	len		)
		__field(	ext4_fsblk_t,	pblk		)
		__field(	char,		status		)
		__field(	int,		found		)
	),

	TP_fast_assign(
		__entry->dev	= inode->i_sb->s_dev;
		__entry->ino	= inode->i_ino;
		__entry->lblk	= es->es_lblk;
		__entry->len	= es->es_len;
		__entry->pblk	= ext4_es_show_pblock(es);
		__entry->status	= ext4_es_status(es);
		__entry->found	= found;
	),

	TP_printk("dev %d,%d ino %lu found %d [%u/%u) %llu %s",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  (unsigned long) __entry->ino, __entry->found,
		  __entry->lblk, __entry->len,
		  __entry->found ? __entry->pblk : 0,
		  show_extent_status(__entry->found ? __entry->status : 0))
);

DECLARE_EVENT_CLASS(ext4__es_shrink_enter,
	TP_PROTO(struct super_block *sb, int nr_to_scan, int cache_cnt),

	TP_ARGS(sb, nr_to_scan, cache_cnt),

	TP_STRUCT__entry(
		__field(	dev_t,	dev			)
		__field(	int,	nr_to_scan		)
		__field(	int,	cache_cnt		)
	),

	TP_fast_assign(
		__entry->dev		= sb->s_dev;
		__entry->nr_to_scan	= nr_to_scan;
		__entry->cache_cnt	= cache_cnt;
	),

	TP_printk("dev %d,%d nr_to_scan %d cache_cnt %d",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  __entry->nr_to_scan, __entry->cache_cnt)
);

DEFINE_EVENT(ext4__es_shrink_enter, ext4_es_shrink_count,
	TP_PROTO(struct super_block *sb, int nr_to_scan, int cache_cnt),

	TP_ARGS(sb, nr_to_scan, cache_cnt)
);

DEFINE_EVENT(ext4__es_shrink_enter, ext4_es_shrink_scan_enter,
	TP_PROTO(struct super_block *sb, int nr_to_scan, int cache_cnt),

	TP_ARGS(sb, nr_to_scan, cache_cnt)
);

TRACE_EVENT(ext4_es_shrink_scan_exit,
	TP_PROTO(struct super_block *sb, int nr_shrunk, int cache_cnt),

	TP_ARGS(sb, nr_shrunk, cache_cnt),

	TP_STRUCT__entry(
		__field(	dev_t,	dev			)
		__field(	int,	nr_shrunk		)
		__field(	int,	cache_cnt		)
	),

	TP_fast_assign(
		__entry->dev		= sb->s_dev;
		__entry->nr_shrunk	= nr_shrunk;
		__entry->cache_cnt	= cache_cnt;
	),

	TP_printk("dev %d,%d nr_shrunk %d cache_cnt %d",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  __entry->nr_shrunk, __entry->cache_cnt)
);

TRACE_EVENT(ext4_collapse_range,
	TP_PROTO(struct inode *inode, loff_t offset, loff_t len),

	TP_ARGS(inode, offset, len),

	TP_STRUCT__entry(
		__field(dev_t,	dev)
		__field(ino_t,	ino)
		__field(loff_t,	offset)
		__field(loff_t, len)
	),

	TP_fast_assign(
		__entry->dev	= inode->i_sb->s_dev;
		__entry->ino	= inode->i_ino;
		__entry->offset	= offset;
		__entry->len	= len;
	),

	TP_printk("dev %d,%d ino %lu offset %lld len %lld",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  (unsigned long) __entry->ino,
		  __entry->offset, __entry->len)
);

TRACE_EVENT(ext4_insert_range,
	TP_PROTO(struct inode *inode, loff_t offset, loff_t len),

	TP_ARGS(inode, offset, len),

	TP_STRUCT__entry(
		__field(dev_t,	dev)
		__field(ino_t,	ino)
		__field(loff_t,	offset)
		__field(loff_t, len)
	),

	TP_fast_assign(
		__entry->dev	= inode->i_sb->s_dev;
		__entry->ino	= inode->i_ino;
		__entry->offset	= offset;
		__entry->len	= len;
	),

	TP_printk("dev %d,%d ino %lu offset %lld len %lld",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  (unsigned long) __entry->ino,
		  __entry->offset, __entry->len)
);

TRACE_EVENT(ext4_es_shrink,
	TP_PROTO(struct super_block *sb, int nr_shrunk, u64 scan_time,
		 int nr_skipped, int retried),

	TP_ARGS(sb, nr_shrunk, scan_time, nr_skipped, retried),

	TP_STRUCT__entry(
		__field(	dev_t,		dev		)
		__field(	int,		nr_shrunk	)
		__field(	unsigned long long, scan_time	)
		__field(	int,		nr_skipped	)
		__field(	int,		retried		)
	),

	TP_fast_assign(
		__entry->dev		= sb->s_dev;
		__entry->nr_shrunk	= nr_shrunk;
		__entry->scan_time	= div_u64(scan_time, 1000);
		__entry->nr_skipped	= nr_skipped;
		__entry->retried	= retried;
	),

	TP_printk("dev %d,%d nr_shrunk %d, scan_time %llu "
		  "nr_skipped %d retried %d",
		  MAJOR(__entry->dev), MINOR(__entry->dev), __entry->nr_shrunk,
		  __entry->scan_time, __entry->nr_skipped, __entry->retried)
);

TRACE_EVENT(ext4_es_insert_delayed_block,
	TP_PROTO(struct inode *inode, struct extent_status *es,
		 bool allocated),

	TP_ARGS(inode, es, allocated),

	TP_STRUCT__entry(
		__field(	dev_t,		dev		)
		__field(	ino_t,		ino		)
		__field(	ext4_lblk_t,	lblk		)
		__field(	ext4_lblk_t,	len		)
		__field(	ext4_fsblk_t,	pblk		)
		__field(	char,		status		)
		__field(	bool,		allocated	)
	),

	TP_fast_assign(
		__entry->dev		= inode->i_sb->s_dev;
		__entry->ino		= inode->i_ino;
		__entry->lblk		= es->es_lblk;
		__entry->len		= es->es_len;
		__entry->pblk		= ext4_es_show_pblock(es);
		__entry->status		= ext4_es_status(es);
		__entry->allocated	= allocated;
	),

	TP_printk("dev %d,%d ino %lu es [%u/%u) mapped %llu status %s "
		  "allocated %d",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  (unsigned long) __entry->ino,
		  __entry->lblk, __entry->len,
		  __entry->pblk, show_extent_status(__entry->status),
		  __entry->allocated)
);

/* fsmap traces */
DECLARE_EVENT_CLASS(ext4_fsmap_class,
	TP_PROTO(struct super_block *sb, u32 keydev, u32 agno, u64 bno, u64 len,
		 u64 owner),
	TP_ARGS(sb, keydev, agno, bno, len, owner),
	TP_STRUCT__entry(
		__field(dev_t, dev)
		__field(dev_t, keydev)
		__field(u32, agno)
		__field(u64, bno)
		__field(u64, len)
		__field(u64, owner)
	),
	TP_fast_assign(
		__entry->dev = sb->s_bdev->bd_dev;
		__entry->keydev = new_decode_dev(keydev);
		__entry->agno = agno;
		__entry->bno = bno;
		__entry->len = len;
		__entry->owner = owner;
	),
	TP_printk("dev %d:%d keydev %d:%d agno %u bno %llu len %llu owner %lld\n",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  MAJOR(__entry->keydev), MINOR(__entry->keydev),
		  __entry->agno,
		  __entry->bno,
		  __entry->len,
		  __entry->owner)
)
#define DEFINE_FSMAP_EVENT(name) \
DEFINE_EVENT(ext4_fsmap_class, name, \
	TP_PROTO(struct super_block *sb, u32 keydev, u32 agno, u64 bno, u64 len, \
		 u64 owner), \
	TP_ARGS(sb, keydev, agno, bno, len, owner))
DEFINE_FSMAP_EVENT(ext4_fsmap_low_key);
DEFINE_FSMAP_EVENT(ext4_fsmap_high_key);
DEFINE_FSMAP_EVENT(ext4_fsmap_mapping);

DECLARE_EVENT_CLASS(ext4_getfsmap_class,
	TP_PROTO(struct super_block *sb, struct ext4_fsmap *fsmap),
	TP_ARGS(sb, fsmap),
	TP_STRUCT__entry(
		__field(dev_t, dev)
		__field(dev_t, keydev)
		__field(u64, block)
		__field(u64, len)
		__field(u64, owner)
		__field(u64, flags)
	),
	TP_fast_assign(
		__entry->dev = sb->s_bdev->bd_dev;
		__entry->keydev = new_decode_dev(fsmap->fmr_device);
		__entry->block = fsmap->fmr_physical;
		__entry->len = fsmap->fmr_length;
		__entry->owner = fsmap->fmr_owner;
		__entry->flags = fsmap->fmr_flags;
	),
	TP_printk("dev %d:%d keydev %d:%d block %llu len %llu owner %lld flags 0x%llx\n",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  MAJOR(__entry->keydev), MINOR(__entry->keydev),
		  __entry->block,
		  __entry->len,
		  __entry->owner,
		  __entry->flags)
)
#define DEFINE_GETFSMAP_EVENT(name) \
DEFINE_EVENT(ext4_getfsmap_class, name, \
	TP_PROTO(struct super_block *sb, struct ext4_fsmap *fsmap), \
	TP_ARGS(sb, fsmap))
DEFINE_GETFSMAP_EVENT(ext4_getfsmap_low_key);
DEFINE_GETFSMAP_EVENT(ext4_getfsmap_high_key);
DEFINE_GETFSMAP_EVENT(ext4_getfsmap_mapping);

TRACE_EVENT(ext4_shutdown,
	TP_PROTO(struct super_block *sb, unsigned long flags),

	TP_ARGS(sb, flags),

	TP_STRUCT__entry(
		__field(	dev_t,	dev			)
		__field(     unsigned,	flags			)
	),

	TP_fast_assign(
		__entry->dev	= sb->s_dev;
		__entry->flags	= flags;
	),

	TP_printk("dev %d,%d flags %u",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  __entry->flags)
);

TRACE_EVENT(ext4_error,
	TP_PROTO(struct super_block *sb, const char *function,
		 unsigned int line),

	TP_ARGS(sb, function, line),

	TP_STRUCT__entry(
		__field(	dev_t,	dev			)
		__field( const char *,	function		)
		__field(     unsigned,	line			)
	),

	TP_fast_assign(
		__entry->dev	= sb->s_dev;
		__entry->function = function;
		__entry->line	= line;
	),

	TP_printk("dev %d,%d function %s line %u",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  __entry->function, __entry->line)
);

TRACE_EVENT(ext4_prefetch_bitmaps,
	    TP_PROTO(struct super_block *sb, ext4_group_t group,
		     ext4_group_t next, unsigned int prefetch_ios),

	TP_ARGS(sb, group, next, prefetch_ios),

	TP_STRUCT__entry(
		__field(	dev_t,	dev			)
		__field(	__u32,	group			)
		__field(	__u32,	next			)
		__field(	__u32,	ios			)
	),

	TP_fast_assign(
		__entry->dev	= sb->s_dev;
		__entry->group	= group;
		__entry->next	= next;
		__entry->ios	= prefetch_ios;
	),

	TP_printk("dev %d,%d group %u next %u ios %u",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  __entry->group, __entry->next, __entry->ios)
);

TRACE_EVENT(ext4_lazy_itable_init,
	    TP_PROTO(struct super_block *sb, ext4_group_t group),

	TP_ARGS(sb, group),

	TP_STRUCT__entry(
		__field(	dev_t,	dev			)
		__field(	__u32,	group			)
	),

	TP_fast_assign(
		__entry->dev	= sb->s_dev;
		__entry->group	= group;
	),

	TP_printk("dev %d,%d group %u",
		  MAJOR(__entry->dev), MINOR(__entry->dev), __entry->group)
);

TRACE_EVENT(ext4_fc_replay_scan,
	TP_PROTO(struct super_block *sb, int error, int off),

	TP_ARGS(sb, error, off),

	TP_STRUCT__entry(
		__field(dev_t, dev)
		__field(int, error)
		__field(int, off)
	),

	TP_fast_assign(
		__entry->dev = sb->s_dev;
		__entry->error = error;
		__entry->off = off;
	),

	TP_printk("FC scan pass on dev %d,%d: error %d, off %d",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  __entry->error, __entry->off)
);

TRACE_EVENT(ext4_fc_replay,
	TP_PROTO(struct super_block *sb, int tag, int ino, int priv1, int priv2),

	TP_ARGS(sb, tag, ino, priv1, priv2),

	TP_STRUCT__entry(
		__field(dev_t, dev)
		__field(int, tag)
		__field(int, ino)
		__field(int, priv1)
		__field(int, priv2)
	),

	TP_fast_assign(
		__entry->dev = sb->s_dev;
		__entry->tag = tag;
		__entry->ino = ino;
		__entry->priv1 = priv1;
		__entry->priv2 = priv2;
	),

	TP_printk("FC Replay %d,%d: tag %d, ino %d, data1 %d, data2 %d",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  __entry->tag, __entry->ino, __entry->priv1, __entry->priv2)
);

TRACE_EVENT(ext4_fc_commit_start,
	TP_PROTO(struct super_block *sb),

	TP_ARGS(sb),

	TP_STRUCT__entry(
		__field(dev_t, dev)
	),

	TP_fast_assign(
		__entry->dev = sb->s_dev;
	),

	TP_printk("fast_commit started on dev %d,%d",
		  MAJOR(__entry->dev), MINOR(__entry->dev))
);

TRACE_EVENT(ext4_fc_commit_stop,
	    TP_PROTO(struct super_block *sb, int nblks, int reason),

	TP_ARGS(sb, nblks, reason),

	TP_STRUCT__entry(
		__field(dev_t, dev)
		__field(int, nblks)
		__field(int, reason)
		__field(int, num_fc)
		__field(int, num_fc_ineligible)
		__field(int, nblks_agg)
	),

	TP_fast_assign(
		__entry->dev = sb->s_dev;
		__entry->nblks = nblks;
		__entry->reason = reason;
		__entry->num_fc = EXT4_SB(sb)->s_fc_stats.fc_num_commits;
		__entry->num_fc_ineligible =
			EXT4_SB(sb)->s_fc_stats.fc_ineligible_commits;
		__entry->nblks_agg = EXT4_SB(sb)->s_fc_stats.fc_numblks;
	),

	TP_printk("fc on [%d,%d] nblks %d, reason %d, fc = %d, ineligible = %d, agg_nblks %d",
		  MAJOR(__entry->dev), MINOR(__entry->dev),
		  __entry->nblks, __entry->reason, __entry->num_fc,
		  __entry->num_fc_ineligible, __entry->nblks_agg)
);

#define FC_REASON_NAME_STAT(reason)					\
	show_fc_reason(reason),						\
	__entry->sbi->s_fc_stats.fc_ineligible_reason_count[reason]

TRACE_EVENT(ext4_fc_stats,
	    TP_PROTO(struct super_block *sb),

	    TP_ARGS(sb),

	    TP_STRUCT__entry(
		    __field(dev_t, dev)
		    __field(struct ext4_sb_info *, sbi)
		    __field(int, count)
		    ),

	    TP_fast_assign(
		    __entry->dev = sb->s_dev;
		    __entry->sbi = EXT4_SB(sb);
		    ),

	    TP_printk("dev %d:%d fc ineligible reasons:\n"
		      "%s:%d, %s:%d, %s:%d, %s:%d, %s:%d, %s:%d, %s:%d, %s:%d, %s:%d; "
		      "num_commits:%ld, ineligible: %ld, numblks: %ld",
		      MAJOR(__entry->dev), MINOR(__entry->dev),
		      FC_REASON_NAME_STAT(EXT4_FC_REASON_XATTR),
		      FC_REASON_NAME_STAT(EXT4_FC_REASON_CROSS_RENAME),
		      FC_REASON_NAME_STAT(EXT4_FC_REASON_JOURNAL_FLAG_CHANGE),
		      FC_REASON_NAME_STAT(EXT4_FC_REASON_NOMEM),
		      FC_REASON_NAME_STAT(EXT4_FC_REASON_SWAP_BOOT),
		      FC_REASON_NAME_STAT(EXT4_FC_REASON_RESIZE),
		      FC_REASON_NAME_STAT(EXT4_FC_REASON_RENAME_DIR),
		      FC_REASON_NAME_STAT(EXT4_FC_REASON_FALLOC_RANGE),
		      FC_REASON_NAME_STAT(EXT4_FC_REASON_INODE_JOURNAL_DATA),
		      __entry->sbi->s_fc_stats.fc_num_commits,
		      __entry->sbi->s_fc_stats.fc_ineligible_commits,
		      __entry->sbi->s_fc_stats.fc_numblks)

);

#define DEFINE_TRACE_DENTRY_EVENT(__type)				\
	TRACE_EVENT(ext4_fc_track_##__type,				\
	    TP_PROTO(struct inode *inode, struct dentry *dentry, int ret), \
									\
	    TP_ARGS(inode, dentry, ret),				\
									\
	    TP_STRUCT__entry(						\
		    __field(dev_t, dev)					\
		    __field(int, ino)					\
		    __field(int, error)					\
		    ),							\
									\
	    TP_fast_assign(						\
		    __entry->dev = inode->i_sb->s_dev;			\
		    __entry->ino = inode->i_ino;			\
		    __entry->error = ret;				\
		    ),							\
									\
	    TP_printk("dev %d:%d, inode %d, error %d, fc_%s",		\
		      MAJOR(__entry->dev), MINOR(__entry->dev),		\
		      __entry->ino, __entry->error,			\
		      #__type)						\
	)

DEFINE_TRACE_DENTRY_EVENT(create);
DEFINE_TRACE_DENTRY_EVENT(link);
DEFINE_TRACE_DENTRY_EVENT(unlink);

TRACE_EVENT(ext4_fc_track_inode,
	    TP_PROTO(struct inode *inode, int ret),

	    TP_ARGS(inode, ret),

	    TP_STRUCT__entry(
		    __field(dev_t, dev)
		    __field(int, ino)
		    __field(int, error)
		    ),

	    TP_fast_assign(
		    __entry->dev = inode->i_sb->s_dev;
		    __entry->ino = inode->i_ino;
		    __entry->error = ret;
		    ),

	    TP_printk("dev %d:%d, inode %d, error %d",
		      MAJOR(__entry->dev), MINOR(__entry->dev),
		      __entry->ino, __entry->error)
	);

TRACE_EVENT(ext4_fc_track_range,
	    TP_PROTO(struct inode *inode, long start, long end, int ret),

	    TP_ARGS(inode, start, end, ret),

	    TP_STRUCT__entry(
		    __field(dev_t, dev)
		    __field(int, ino)
		    __field(long, start)
		    __field(long, end)
		    __field(int, error)
		    ),

	    TP_fast_assign(
		    __entry->dev = inode->i_sb->s_dev;
		    __entry->ino = inode->i_ino;
		    __entry->start = start;
		    __entry->end = end;
		    __entry->error = ret;
		    ),

	    TP_printk("dev %d:%d, inode %d, error %d, start %ld, end %ld",
		      MAJOR(__entry->dev), MINOR(__entry->dev),
		      __entry->ino, __entry->error, __entry->start,
		      __entry->end)
	);

#endif /* _TRACE_EXT4_H */

/* This part must be outside protection */
#include <trace/define_trace.h>
