// SPDX-License-Identifier: GPL-2.0-only
/* Copyright (c) 2017 Facebook
 */
#include <linux/slab.h>
#include <linux/bpf.h>

#include "map_in_map.h"

struct bpf_map *bpf_map_meta_alloc(int inner_map_ufd)
{
	struct bpf_map *inner_map, *inner_map_meta;
	u32 inner_map_meta_size;
	struct fd f;

	f = fdget(inner_map_ufd);
	inner_map = __bpf_map_get(f);
	if (IS_ERR(inner_map))
		return inner_map;

	/* Does not support >1 level map-in-map */
	if (inner_map->inner_map_meta) {
		fdput(f);
		return ERR_PTR(-EINVAL);
	}

	if (!inner_map->ops->map_meta_equal) {
		fdput(f);
		return ERR_PTR(-ENOTSUPP);
	}

	if (map_value_has_spin_lock(inner_map)) {
		fdput(f);
		return ERR_PTR(-ENOTSUPP);
	}

	inner_map_meta_size = sizeof(*inner_map_meta);
	/* In some cases verifier needs to access beyond just base map. */
	if (inner_map->ops == &array_map_ops)
		inner_map_meta_size = sizeof(struct bpf_array);

	inner_map_meta = kzalloc(inner_map_meta_size, GFP_USER);
	if (!inner_map_meta) {
		fdput(f);
		return ERR_PTR(-ENOMEM);
	}

	inner_map_meta->map_type = inner_map->map_type;
	inner_map_meta->key_size = inner_map->key_size;
	inner_map_meta->value_size = inner_map->value_size;
	inner_map_meta->map_flags = inner_map->map_flags;
	inner_map_meta->max_entries = inner_map->max_entries;
	inner_map_meta->spin_lock_off = inner_map->spin_lock_off;

	/* Misc members not needed in bpf_map_meta_equal() check. */
	inner_map_meta->ops = inner_map->ops;
	if (inner_map->ops == &array_map_ops) {
		inner_map_meta->bypass_spec_v1 = inner_map->bypass_spec_v1;
		container_of(inner_map_meta, struct bpf_array, map)->index_mask =
		     container_of(inner_map, struct bpf_array, map)->index_mask;
	}

	fdput(f);
	return inner_map_meta;
}

void bpf_map_meta_free(struct bpf_map *map_meta)
{
	kfree(map_meta);
}

bool bpf_map_meta_equal(const struct bpf_map *meta0,
			const struct bpf_map *meta1)
{
	/* No need to compare ops because it is covered by map_type */
	return meta0->map_type == meta1->map_type &&
		meta0->key_size == meta1->key_size &&
		meta0->value_size == meta1->value_size &&
		meta0->map_flags == meta1->map_flags;
}

void *bpf_map_fd_get_ptr(struct bpf_map *map,
			 struct file *map_file /* not used */,
			 int ufd)
{
	struct bpf_map *inner_map, *inner_map_meta;
	struct fd f;

	f = fdget(ufd);
	inner_map = __bpf_map_get(f);
	if (IS_ERR(inner_map))
		return inner_map;

	inner_map_meta = map->inner_map_meta;
	if (inner_map_meta->ops->map_meta_equal(inner_map_meta, inner_map))
		bpf_map_inc(inner_map);
	else
		inner_map = ERR_PTR(-EINVAL);

	fdput(f);
	return inner_map;
}

void bpf_map_fd_put_ptr(void *ptr)
{
	/* ptr->ops->map_free() has to go through one
	 * rcu grace period by itself.
	 */
	bpf_map_put(ptr);
}

u32 bpf_map_fd_sys_lookup_elem(void *ptr)
{
	return ((struct bpf_map *)ptr)->id;
}
