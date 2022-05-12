/* SPDX-License-Identifier: GPL-2.0 */
/*
 * fs/f2fs/xattr.h
 *
 * Copyright (c) 2012 Samsung Electronics Co., Ltd.
 *             http://www.samsung.com/
 *
 * Portions of this code from linux/fs/ext2/xattr.h
 *
 * On-disk format of extended attributes for the ext2 filesystem.
 *
 * (C) 2001 Andreas Gruenbacher, <a.gruenbacher@computer.org>
 */
#ifndef __F2FS_XATTR_H__
#define __F2FS_XATTR_H__

#include <linux/init.h>
#include <linux/xattr.h>

/* Magic value in attribute blocks */
#define F2FS_XATTR_MAGIC                0xF2F52011

/* Maximum number of references to one attribute block */
#define F2FS_XATTR_REFCOUNT_MAX         1024

/* Name indexes */
#define F2FS_SYSTEM_ADVISE_NAME			"system.advise"
#define F2FS_XATTR_INDEX_USER			1
#define F2FS_XATTR_INDEX_POSIX_ACL_ACCESS	2
#define F2FS_XATTR_INDEX_POSIX_ACL_DEFAULT	3
#define F2FS_XATTR_INDEX_TRUSTED		4
#define F2FS_XATTR_INDEX_LUSTRE			5
#define F2FS_XATTR_INDEX_SECURITY		6
#define F2FS_XATTR_INDEX_ADVISE			7
/* Should be same as EXT4_XATTR_INDEX_ENCRYPTION */
#define F2FS_XATTR_INDEX_ENCRYPTION		9
#define F2FS_XATTR_INDEX_VERITY			11

#define F2FS_XATTR_NAME_ENCRYPTION_CONTEXT	"c"
#define F2FS_XATTR_NAME_VERITY			"v"

struct f2fs_xattr_header {
	__le32  h_magic;        /* magic number for identification */
	__le32  h_refcount;     /* reference count */
	__u32   h_reserved[4];  /* zero right now */
};

struct f2fs_xattr_entry {
	__u8    e_name_index;
	__u8    e_name_len;
	__le16  e_value_size;   /* size of attribute value */
	char    e_name[];      /* attribute name */
};

#define XATTR_HDR(ptr)		((struct f2fs_xattr_header *)(ptr))
#define XATTR_ENTRY(ptr)	((struct f2fs_xattr_entry *)(ptr))
#define XATTR_FIRST_ENTRY(ptr)	(XATTR_ENTRY(XATTR_HDR(ptr) + 1))
#define XATTR_ROUND		(3)

#define XATTR_ALIGN(size)	(((size) + XATTR_ROUND) & ~XATTR_ROUND)

#define ENTRY_SIZE(entry) (XATTR_ALIGN(sizeof(struct f2fs_xattr_entry) + \
			(entry)->e_name_len + le16_to_cpu((entry)->e_value_size)))

#define XATTR_NEXT_ENTRY(entry)	((struct f2fs_xattr_entry *)((char *)(entry) +\
			ENTRY_SIZE(entry)))

#define IS_XATTR_LAST_ENTRY(entry) (*(__u32 *)(entry) == 0)

#define list_for_each_xattr(entry, addr) \
		for (entry = XATTR_FIRST_ENTRY(addr);\
				!IS_XATTR_LAST_ENTRY(entry);\
				entry = XATTR_NEXT_ENTRY(entry))
#define VALID_XATTR_BLOCK_SIZE	(PAGE_SIZE - sizeof(struct node_footer))
#define XATTR_PADDING_SIZE	(sizeof(__u32))
#define XATTR_SIZE(i)		((F2FS_I(i)->i_xattr_nid ?		\
					VALID_XATTR_BLOCK_SIZE : 0) +	\
						(inline_xattr_size(i)))
#define MIN_OFFSET(i)		XATTR_ALIGN(inline_xattr_size(i) +	\
						VALID_XATTR_BLOCK_SIZE)

#define MAX_VALUE_LEN(i)	(MIN_OFFSET(i) -			\
				sizeof(struct f2fs_xattr_header) -	\
				sizeof(struct f2fs_xattr_entry))

#define MAX_INLINE_XATTR_SIZE						\
			(DEF_ADDRS_PER_INODE -				\
			F2FS_TOTAL_EXTRA_ATTR_SIZE / sizeof(__le32) -	\
			DEF_INLINE_RESERVED_SIZE -			\
			MIN_INLINE_DENTRY_SIZE / sizeof(__le32))

/*
 * On-disk structure of f2fs_xattr
 * We use inline xattrs space + 1 block for xattr.
 *
 * +--------------------+
 * | f2fs_xattr_header  |
 * |                    |
 * +--------------------+
 * | f2fs_xattr_entry   |
 * | .e_name_index = 1  |
 * | .e_name_len = 3    |
 * | .e_value_size = 14 |
 * | .e_name = "foo"    |
 * | "value_of_xattr"   |<- value_offs = e_name + e_name_len
 * +--------------------+
 * | f2fs_xattr_entry   |
 * | .e_name_index = 4  |
 * | .e_name = "bar"    |
 * +--------------------+
 * |                    |
 * |        Free        |
 * |                    |
 * +--------------------+<- MIN_OFFSET
 * |   node_footer      |
 * | (nid, ino, offset) |
 * +--------------------+
 *
 **/

#ifdef CONFIG_F2FS_FS_XATTR
extern const struct xattr_handler f2fs_xattr_user_handler;
extern const struct xattr_handler f2fs_xattr_trusted_handler;
extern const struct xattr_handler f2fs_xattr_advise_handler;
extern const struct xattr_handler f2fs_xattr_security_handler;

extern const struct xattr_handler *f2fs_xattr_handlers[];

extern int f2fs_setxattr(struct inode *, int, const char *,
				const void *, size_t, struct page *, int);
extern int f2fs_getxattr(struct inode *, int, const char *, void *,
						size_t, struct page *);
extern ssize_t f2fs_listxattr(struct dentry *, char *, size_t);
extern int f2fs_init_xattr_caches(struct f2fs_sb_info *);
extern void f2fs_destroy_xattr_caches(struct f2fs_sb_info *);
#else

#define f2fs_xattr_handlers	NULL
#define f2fs_listxattr		NULL
static inline int f2fs_setxattr(struct inode *inode, int index,
		const char *name, const void *value, size_t size,
		struct page *page, int flags)
{
	return -EOPNOTSUPP;
}
static inline int f2fs_getxattr(struct inode *inode, int index,
			const char *name, void *buffer,
			size_t buffer_size, struct page *dpage)
{
	return -EOPNOTSUPP;
}
static inline int f2fs_init_xattr_caches(struct f2fs_sb_info *sbi) { return 0; }
static inline void f2fs_destroy_xattr_caches(struct f2fs_sb_info *sbi) { }
#endif

#ifdef CONFIG_F2FS_FS_SECURITY
extern int f2fs_init_security(struct inode *, struct inode *,
				const struct qstr *, struct page *);
#else
static inline int f2fs_init_security(struct inode *inode, struct inode *dir,
				const struct qstr *qstr, struct page *ipage)
{
	return 0;
}
#endif
#endif /* __F2FS_XATTR_H__ */
