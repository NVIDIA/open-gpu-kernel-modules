/*
 * JFFS2 -- Journalling Flash File System, Version 2.
 *
 * Copyright © 2001-2007 Red Hat, Inc.
 * Copyright © 2004-2010 David Woodhouse <dwmw2@infradead.org>
 *
 * Created by David Woodhouse <dwmw2@infradead.org>
 *
 * For licensing information, see the file 'LICENCE' in this directory.
 *
 */

#ifndef _JFFS2_DEBUG_H_
#define _JFFS2_DEBUG_H_

#include <linux/printk.h>
#include <linux/sched.h>

#ifndef CONFIG_JFFS2_FS_DEBUG
#define CONFIG_JFFS2_FS_DEBUG 0
#endif

#if CONFIG_JFFS2_FS_DEBUG > 0
/* Enable "paranoia" checks and dumps */
#define JFFS2_DBG_PARANOIA_CHECKS
#define JFFS2_DBG_DUMPS

/*
 * By defining/undefining the below macros one may select debugging messages
 * fro specific JFFS2 subsystems.
 */
#define JFFS2_DBG_READINODE_MESSAGES
#define JFFS2_DBG_FRAGTREE_MESSAGES
#define JFFS2_DBG_DENTLIST_MESSAGES
#define JFFS2_DBG_NODEREF_MESSAGES
#define JFFS2_DBG_INOCACHE_MESSAGES
#define JFFS2_DBG_SUMMARY_MESSAGES
#define JFFS2_DBG_FSBUILD_MESSAGES
#endif

#if CONFIG_JFFS2_FS_DEBUG > 1
#define JFFS2_DBG_FRAGTREE2_MESSAGES
#define JFFS2_DBG_READINODE2_MESSAGES
#define JFFS2_DBG_MEMALLOC_MESSAGES
#endif

/* Sanity checks are supposed to be light-weight and enabled by default */
#define JFFS2_DBG_SANITY_CHECKS

/*
 * Dx() are mainly used for debugging messages, they must go away and be
 * superseded by nicer dbg_xxx() macros...
 */
#if CONFIG_JFFS2_FS_DEBUG > 0
#define DEBUG
#define D1(x) x
#else
#define D1(x)
#endif

#if CONFIG_JFFS2_FS_DEBUG > 1
#define D2(x) x
#else
#define D2(x)
#endif

#define jffs2_dbg(level, fmt, ...)		\
do {						\
	if (CONFIG_JFFS2_FS_DEBUG >= level)	\
		pr_debug(fmt, ##__VA_ARGS__);	\
} while (0)

/* The prefixes of JFFS2 messages */
#define JFFS2_DBG		KERN_DEBUG
#define JFFS2_DBG_PREFIX	"[JFFS2 DBG]"
#define JFFS2_DBG_MSG_PREFIX	JFFS2_DBG JFFS2_DBG_PREFIX

/* JFFS2 message macros */
#define JFFS2_ERROR(fmt, ...)					\
	pr_err("error: (%d) %s: " fmt,				\
	       task_pid_nr(current), __func__, ##__VA_ARGS__)

#define JFFS2_WARNING(fmt, ...)						\
	pr_warn("warning: (%d) %s: " fmt,				\
		task_pid_nr(current), __func__, ##__VA_ARGS__)

#define JFFS2_NOTICE(fmt, ...)						\
	pr_notice("notice: (%d) %s: " fmt,				\
		  task_pid_nr(current), __func__, ##__VA_ARGS__)

#define JFFS2_DEBUG(fmt, ...)						\
	printk(KERN_DEBUG "[JFFS2 DBG] (%d) %s: " fmt,			\
	       task_pid_nr(current), __func__, ##__VA_ARGS__)

/*
 * We split our debugging messages on several parts, depending on the JFFS2
 * subsystem the message belongs to.
 */
/* Read inode debugging messages */
#ifdef JFFS2_DBG_READINODE_MESSAGES
#define dbg_readinode(fmt, ...)	JFFS2_DEBUG(fmt, ##__VA_ARGS__)
#else
#define dbg_readinode(fmt, ...)	no_printk(fmt, ##__VA_ARGS__)
#endif
#ifdef JFFS2_DBG_READINODE2_MESSAGES
#define dbg_readinode2(fmt, ...)	JFFS2_DEBUG(fmt, ##__VA_ARGS__)
#else
#define dbg_readinode2(fmt, ...)	no_printk(fmt, ##__VA_ARGS__)
#endif

/* Fragtree build debugging messages */
#ifdef JFFS2_DBG_FRAGTREE_MESSAGES
#define dbg_fragtree(fmt, ...)	JFFS2_DEBUG(fmt, ##__VA_ARGS__)
#else
#define dbg_fragtree(fmt, ...)	no_printk(fmt, ##__VA_ARGS__)
#endif
#ifdef JFFS2_DBG_FRAGTREE2_MESSAGES
#define dbg_fragtree2(fmt, ...)	JFFS2_DEBUG(fmt, ##__VA_ARGS__)
#else
#define dbg_fragtree2(fmt, ...)	no_printk(fmt, ##__VA_ARGS__)
#endif

/* Directory entry list manilulation debugging messages */
#ifdef JFFS2_DBG_DENTLIST_MESSAGES
#define dbg_dentlist(fmt, ...)	JFFS2_DEBUG(fmt, ##__VA_ARGS__)
#else
#define dbg_dentlist(fmt, ...)	no_printk(fmt, ##__VA_ARGS__)
#endif

/* Print the messages about manipulating node_refs */
#ifdef JFFS2_DBG_NODEREF_MESSAGES
#define dbg_noderef(fmt, ...)	JFFS2_DEBUG(fmt, ##__VA_ARGS__)
#else
#define dbg_noderef(fmt, ...)	no_printk(fmt, ##__VA_ARGS__)
#endif

/* Manipulations with the list of inodes (JFFS2 inocache) */
#ifdef JFFS2_DBG_INOCACHE_MESSAGES
#define dbg_inocache(fmt, ...)	JFFS2_DEBUG(fmt, ##__VA_ARGS__)
#else
#define dbg_inocache(fmt, ...)	no_printk(fmt, ##__VA_ARGS__)
#endif

/* Summary debugging messages */
#ifdef JFFS2_DBG_SUMMARY_MESSAGES
#define dbg_summary(fmt, ...)	JFFS2_DEBUG(fmt, ##__VA_ARGS__)
#else
#define dbg_summary(fmt, ...)	no_printk(fmt, ##__VA_ARGS__)
#endif

/* File system build messages */
#ifdef JFFS2_DBG_FSBUILD_MESSAGES
#define dbg_fsbuild(fmt, ...)	JFFS2_DEBUG(fmt, ##__VA_ARGS__)
#else
#define dbg_fsbuild(fmt, ...)	no_printk(fmt, ##__VA_ARGS__)
#endif

/* Watch the object allocations */
#ifdef JFFS2_DBG_MEMALLOC_MESSAGES
#define dbg_memalloc(fmt, ...)	JFFS2_DEBUG(fmt, ##__VA_ARGS__)
#else
#define dbg_memalloc(fmt, ...)	no_printk(fmt, ##__VA_ARGS__)
#endif

/* Watch the XATTR subsystem */
#ifdef JFFS2_DBG_XATTR_MESSAGES
#define dbg_xattr(fmt, ...)  JFFS2_DEBUG(fmt, ##__VA_ARGS__)
#else
#define dbg_xattr(fmt, ...)  no_printk(fmt, ##__VA_ARGS__)
#endif 

/* "Sanity" checks */
void
__jffs2_dbg_acct_sanity_check_nolock(struct jffs2_sb_info *c,
				     struct jffs2_eraseblock *jeb);
void
__jffs2_dbg_acct_sanity_check(struct jffs2_sb_info *c,
			      struct jffs2_eraseblock *jeb);

/* "Paranoia" checks */
void
__jffs2_dbg_fragtree_paranoia_check(struct jffs2_inode_info *f);
void
__jffs2_dbg_fragtree_paranoia_check_nolock(struct jffs2_inode_info *f);
void
__jffs2_dbg_acct_paranoia_check(struct jffs2_sb_info *c,
			   	struct jffs2_eraseblock *jeb);
void
__jffs2_dbg_acct_paranoia_check_nolock(struct jffs2_sb_info *c,
				       struct jffs2_eraseblock *jeb);
void
__jffs2_dbg_prewrite_paranoia_check(struct jffs2_sb_info *c,
				    uint32_t ofs, int len);

/* "Dump" functions */
void
__jffs2_dbg_dump_jeb(struct jffs2_sb_info *c, struct jffs2_eraseblock *jeb);
void
__jffs2_dbg_dump_jeb_nolock(struct jffs2_eraseblock *jeb);
void
__jffs2_dbg_dump_block_lists(struct jffs2_sb_info *c);
void
__jffs2_dbg_dump_block_lists_nolock(struct jffs2_sb_info *c);
void
__jffs2_dbg_dump_node_refs(struct jffs2_sb_info *c,
		 	   struct jffs2_eraseblock *jeb);
void
__jffs2_dbg_dump_node_refs_nolock(struct jffs2_sb_info *c,
				  struct jffs2_eraseblock *jeb);
void
__jffs2_dbg_dump_fragtree(struct jffs2_inode_info *f);
void
__jffs2_dbg_dump_fragtree_nolock(struct jffs2_inode_info *f);
void
__jffs2_dbg_dump_buffer(unsigned char *buf, int len, uint32_t offs);
void
__jffs2_dbg_dump_node(struct jffs2_sb_info *c, uint32_t ofs);

#ifdef JFFS2_DBG_PARANOIA_CHECKS
#define jffs2_dbg_fragtree_paranoia_check(f)			\
	__jffs2_dbg_fragtree_paranoia_check(f)
#define jffs2_dbg_fragtree_paranoia_check_nolock(f)		\
	__jffs2_dbg_fragtree_paranoia_check_nolock(f)
#define jffs2_dbg_acct_paranoia_check(c, jeb)			\
	__jffs2_dbg_acct_paranoia_check(c,jeb)
#define jffs2_dbg_acct_paranoia_check_nolock(c, jeb)		\
	__jffs2_dbg_acct_paranoia_check_nolock(c,jeb)
#define jffs2_dbg_prewrite_paranoia_check(c, ofs, len)		\
	__jffs2_dbg_prewrite_paranoia_check(c, ofs, len)
#else
#define jffs2_dbg_fragtree_paranoia_check(f)
#define jffs2_dbg_fragtree_paranoia_check_nolock(f)
#define jffs2_dbg_acct_paranoia_check(c, jeb)
#define jffs2_dbg_acct_paranoia_check_nolock(c, jeb)
#define jffs2_dbg_prewrite_paranoia_check(c, ofs, len)
#endif /* !JFFS2_PARANOIA_CHECKS */

#ifdef JFFS2_DBG_DUMPS
#define jffs2_dbg_dump_jeb(c, jeb)				\
	__jffs2_dbg_dump_jeb(c, jeb);
#define jffs2_dbg_dump_jeb_nolock(jeb)				\
	__jffs2_dbg_dump_jeb_nolock(jeb);
#define jffs2_dbg_dump_block_lists(c)				\
	__jffs2_dbg_dump_block_lists(c)
#define jffs2_dbg_dump_block_lists_nolock(c)			\
	__jffs2_dbg_dump_block_lists_nolock(c)
#define jffs2_dbg_dump_fragtree(f)				\
	__jffs2_dbg_dump_fragtree(f);
#define jffs2_dbg_dump_fragtree_nolock(f)			\
	__jffs2_dbg_dump_fragtree_nolock(f);
#define jffs2_dbg_dump_buffer(buf, len, offs)			\
	__jffs2_dbg_dump_buffer(*buf, len, offs);
#define jffs2_dbg_dump_node(c, ofs)				\
	__jffs2_dbg_dump_node(c, ofs);
#else
#define jffs2_dbg_dump_jeb(c, jeb)
#define jffs2_dbg_dump_jeb_nolock(jeb)
#define jffs2_dbg_dump_block_lists(c)
#define jffs2_dbg_dump_block_lists_nolock(c)
#define jffs2_dbg_dump_fragtree(f)
#define jffs2_dbg_dump_fragtree_nolock(f)
#define jffs2_dbg_dump_buffer(buf, len, offs)
#define jffs2_dbg_dump_node(c, ofs)
#endif /* !JFFS2_DBG_DUMPS */

#ifdef JFFS2_DBG_SANITY_CHECKS
#define jffs2_dbg_acct_sanity_check(c, jeb)			\
	__jffs2_dbg_acct_sanity_check(c, jeb)
#define jffs2_dbg_acct_sanity_check_nolock(c, jeb)		\
	__jffs2_dbg_acct_sanity_check_nolock(c, jeb)
#else
#define jffs2_dbg_acct_sanity_check(c, jeb)
#define jffs2_dbg_acct_sanity_check_nolock(c, jeb)
#endif /* !JFFS2_DBG_SANITY_CHECKS */

#endif /* _JFFS2_DEBUG_H_ */
