/* SPDX-License-Identifier: GPL-2.0 */
/*
 * zonefs filesystem driver tracepoints.
 *
 * Copyright (C) 2021 Western Digital Corporation or its affiliates.
 */

#undef TRACE_SYSTEM
#define TRACE_SYSTEM zonefs

#if !defined(_TRACE_ZONEFS_H) || defined(TRACE_HEADER_MULTI_READ)
#define _TRACE_ZONEFS_H

#include <linux/tracepoint.h>
#include <linux/trace_seq.h>
#include <linux/blkdev.h>

#include "zonefs.h"

#define show_dev(dev) MAJOR(dev), MINOR(dev)

TRACE_EVENT(zonefs_zone_mgmt,
	    TP_PROTO(struct inode *inode, enum req_opf op),
	    TP_ARGS(inode, op),
	    TP_STRUCT__entry(
			     __field(dev_t, dev)
			     __field(ino_t, ino)
			     __field(int, op)
			     __field(sector_t, sector)
			     __field(sector_t, nr_sectors)
	    ),
	    TP_fast_assign(
			   __entry->dev = inode->i_sb->s_dev;
			   __entry->ino = inode->i_ino;
			   __entry->op = op;
			   __entry->sector = ZONEFS_I(inode)->i_zsector;
			   __entry->nr_sectors =
				   ZONEFS_I(inode)->i_zone_size >> SECTOR_SHIFT;
	    ),
	    TP_printk("bdev=(%d,%d), ino=%lu op=%s, sector=%llu, nr_sectors=%llu",
		      show_dev(__entry->dev), (unsigned long)__entry->ino,
		      blk_op_str(__entry->op), __entry->sector,
		      __entry->nr_sectors
	    )
);

TRACE_EVENT(zonefs_file_dio_append,
	    TP_PROTO(struct inode *inode, ssize_t size, ssize_t ret),
	    TP_ARGS(inode, size, ret),
	    TP_STRUCT__entry(
			     __field(dev_t, dev)
			     __field(ino_t, ino)
			     __field(sector_t, sector)
			     __field(ssize_t, size)
			     __field(loff_t, wpoffset)
			     __field(ssize_t, ret)
	    ),
	    TP_fast_assign(
			   __entry->dev = inode->i_sb->s_dev;
			   __entry->ino = inode->i_ino;
			   __entry->sector = ZONEFS_I(inode)->i_zsector;
			   __entry->size = size;
			   __entry->wpoffset = ZONEFS_I(inode)->i_wpoffset;
			   __entry->ret = ret;
	    ),
	    TP_printk("bdev=(%d, %d), ino=%lu, sector=%llu, size=%zu, wpoffset=%llu, ret=%zu",
		      show_dev(__entry->dev), (unsigned long)__entry->ino,
		      __entry->sector, __entry->size, __entry->wpoffset,
		      __entry->ret
	    )
);

TRACE_EVENT(zonefs_iomap_begin,
	    TP_PROTO(struct inode *inode, struct iomap *iomap),
	    TP_ARGS(inode, iomap),
	    TP_STRUCT__entry(
			     __field(dev_t, dev)
			     __field(ino_t, ino)
			     __field(u64, addr)
			     __field(loff_t, offset)
			     __field(u64, length)
	    ),
	    TP_fast_assign(
			   __entry->dev = inode->i_sb->s_dev;
			   __entry->ino = inode->i_ino;
			   __entry->addr = iomap->addr;
			   __entry->offset = iomap->offset;
			   __entry->length = iomap->length;
	    ),
	    TP_printk("bdev=(%d,%d), ino=%lu, addr=%llu, offset=%llu, length=%llu",
		      show_dev(__entry->dev), (unsigned long)__entry->ino,
		      __entry->addr, __entry->offset, __entry->length
	    )
);

#endif /* _TRACE_ZONEFS_H */

#undef TRACE_INCLUDE_PATH
#define TRACE_INCLUDE_PATH .
#undef TRACE_INCLUDE_FILE
#define TRACE_INCLUDE_FILE trace

/* This part must be outside protection */
#include <trace/define_trace.h>
