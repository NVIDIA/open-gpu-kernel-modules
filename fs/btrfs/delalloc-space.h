/* SPDX-License-Identifier: GPL-2.0 */

#ifndef BTRFS_DELALLOC_SPACE_H
#define BTRFS_DELALLOC_SPACE_H

struct extent_changeset;

int btrfs_alloc_data_chunk_ondemand(struct btrfs_inode *inode, u64 bytes);
int btrfs_check_data_free_space(struct btrfs_inode *inode,
			struct extent_changeset **reserved, u64 start, u64 len);
void btrfs_free_reserved_data_space(struct btrfs_inode *inode,
			struct extent_changeset *reserved, u64 start, u64 len);
void btrfs_delalloc_release_space(struct btrfs_inode *inode,
				  struct extent_changeset *reserved,
				  u64 start, u64 len, bool qgroup_free);
void btrfs_free_reserved_data_space_noquota(struct btrfs_fs_info *fs_info,
					    u64 len);
void btrfs_delalloc_release_metadata(struct btrfs_inode *inode, u64 num_bytes,
				     bool qgroup_free);
int btrfs_delalloc_reserve_space(struct btrfs_inode *inode,
			struct extent_changeset **reserved, u64 start, u64 len);

#endif /* BTRFS_DELALLOC_SPACE_H */
