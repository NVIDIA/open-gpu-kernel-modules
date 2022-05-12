/* SPDX-License-Identifier: GPL-2.0 */
#ifndef XFS_DISCARD_H
#define XFS_DISCARD_H 1

struct fstrim_range;
struct list_head;

extern int	xfs_ioc_trim(struct xfs_mount *, struct fstrim_range __user *);

#endif /* XFS_DISCARD_H */
