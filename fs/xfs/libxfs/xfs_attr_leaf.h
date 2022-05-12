/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2000,2002-2003,2005 Silicon Graphics, Inc.
 * Copyright (c) 2013 Red Hat, Inc.
 * All Rights Reserved.
 */
#ifndef __XFS_ATTR_LEAF_H__
#define	__XFS_ATTR_LEAF_H__

struct attrlist;
struct xfs_attr_list_context;
struct xfs_da_args;
struct xfs_da_state;
struct xfs_da_state_blk;
struct xfs_inode;
struct xfs_trans;

/*
 * Incore version of the attribute leaf header.
 */
struct xfs_attr3_icleaf_hdr {
	uint32_t	forw;
	uint32_t	back;
	uint16_t	magic;
	uint16_t	count;
	uint16_t	usedbytes;
	/*
	 * Firstused is 32-bit here instead of 16-bit like the on-disk variant
	 * to support maximum fsb size of 64k without overflow issues throughout
	 * the attr code. Instead, the overflow condition is handled on
	 * conversion to/from disk.
	 */
	uint32_t	firstused;
	__u8		holes;
	struct {
		uint16_t	base;
		uint16_t	size;
	} freemap[XFS_ATTR_LEAF_MAPSIZE];
};

/*========================================================================
 * Function prototypes for the kernel.
 *========================================================================*/

/*
 * Internal routines when attribute fork size < XFS_LITINO(mp).
 */
void	xfs_attr_shortform_create(struct xfs_da_args *args);
void	xfs_attr_shortform_add(struct xfs_da_args *args, int forkoff);
int	xfs_attr_shortform_lookup(struct xfs_da_args *args);
int	xfs_attr_shortform_getvalue(struct xfs_da_args *args);
int	xfs_attr_shortform_to_leaf(struct xfs_da_args *args,
			struct xfs_buf **leaf_bp);
int	xfs_attr_shortform_remove(struct xfs_da_args *args);
int	xfs_attr_sf_findname(struct xfs_da_args *args,
			     struct xfs_attr_sf_entry **sfep,
			     unsigned int *basep);
int	xfs_attr_shortform_allfit(struct xfs_buf *bp, struct xfs_inode *dp);
int	xfs_attr_shortform_bytesfit(struct xfs_inode *dp, int bytes);
xfs_failaddr_t xfs_attr_shortform_verify(struct xfs_inode *ip);
void	xfs_attr_fork_remove(struct xfs_inode *ip, struct xfs_trans *tp);

/*
 * Internal routines when attribute fork size == XFS_LBSIZE(mp).
 */
int	xfs_attr3_leaf_to_node(struct xfs_da_args *args);
int	xfs_attr3_leaf_to_shortform(struct xfs_buf *bp,
				   struct xfs_da_args *args, int forkoff);
int	xfs_attr3_leaf_clearflag(struct xfs_da_args *args);
int	xfs_attr3_leaf_setflag(struct xfs_da_args *args);
int	xfs_attr3_leaf_flipflags(struct xfs_da_args *args);

/*
 * Routines used for growing the Btree.
 */
int	xfs_attr3_leaf_split(struct xfs_da_state *state,
				   struct xfs_da_state_blk *oldblk,
				   struct xfs_da_state_blk *newblk);
int	xfs_attr3_leaf_lookup_int(struct xfs_buf *leaf,
					struct xfs_da_args *args);
int	xfs_attr3_leaf_getvalue(struct xfs_buf *bp, struct xfs_da_args *args);
int	xfs_attr3_leaf_add(struct xfs_buf *leaf_buffer,
				 struct xfs_da_args *args);
int	xfs_attr3_leaf_remove(struct xfs_buf *leaf_buffer,
				    struct xfs_da_args *args);
int	xfs_attr3_leaf_list_int(struct xfs_buf *bp,
				struct xfs_attr_list_context *context);

/*
 * Routines used for shrinking the Btree.
 */
int	xfs_attr3_leaf_toosmall(struct xfs_da_state *state, int *retval);
void	xfs_attr3_leaf_unbalance(struct xfs_da_state *state,
				       struct xfs_da_state_blk *drop_blk,
				       struct xfs_da_state_blk *save_blk);
/*
 * Utility routines.
 */
xfs_dahash_t	xfs_attr_leaf_lasthash(struct xfs_buf *bp, int *count);
int	xfs_attr_leaf_order(struct xfs_buf *leaf1_bp,
				   struct xfs_buf *leaf2_bp);
int	xfs_attr_leaf_newentsize(struct xfs_da_args *args, int *local);
int	xfs_attr3_leaf_read(struct xfs_trans *tp, struct xfs_inode *dp,
			xfs_dablk_t bno, struct xfs_buf **bpp);
void	xfs_attr3_leaf_hdr_from_disk(struct xfs_da_geometry *geo,
				     struct xfs_attr3_icleaf_hdr *to,
				     struct xfs_attr_leafblock *from);
void	xfs_attr3_leaf_hdr_to_disk(struct xfs_da_geometry *geo,
				   struct xfs_attr_leafblock *to,
				   struct xfs_attr3_icleaf_hdr *from);

#endif	/* __XFS_ATTR_LEAF_H__ */
