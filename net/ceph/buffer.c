// SPDX-License-Identifier: GPL-2.0

#include <linux/ceph/ceph_debug.h>

#include <linux/module.h>
#include <linux/slab.h>

#include <linux/ceph/buffer.h>
#include <linux/ceph/decode.h>
#include <linux/ceph/libceph.h> /* for ceph_kvmalloc */

struct ceph_buffer *ceph_buffer_new(size_t len, gfp_t gfp)
{
	struct ceph_buffer *b;

	b = kmalloc(sizeof(*b), gfp);
	if (!b)
		return NULL;

	b->vec.iov_base = ceph_kvmalloc(len, gfp);
	if (!b->vec.iov_base) {
		kfree(b);
		return NULL;
	}

	kref_init(&b->kref);
	b->alloc_len = len;
	b->vec.iov_len = len;
	dout("buffer_new %p\n", b);
	return b;
}
EXPORT_SYMBOL(ceph_buffer_new);

void ceph_buffer_release(struct kref *kref)
{
	struct ceph_buffer *b = container_of(kref, struct ceph_buffer, kref);

	dout("buffer_release %p\n", b);
	kvfree(b->vec.iov_base);
	kfree(b);
}
EXPORT_SYMBOL(ceph_buffer_release);

int ceph_decode_buffer(struct ceph_buffer **b, void **p, void *end)
{
	size_t len;

	ceph_decode_need(p, end, sizeof(u32), bad);
	len = ceph_decode_32(p);
	dout("decode_buffer len %d\n", (int)len);
	ceph_decode_need(p, end, len, bad);
	*b = ceph_buffer_new(len, GFP_NOFS);
	if (!*b)
		return -ENOMEM;
	ceph_decode_copy(p, (*b)->vec.iov_base, len);
	return 0;
bad:
	return -EINVAL;
}
