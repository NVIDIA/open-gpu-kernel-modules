// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Symmetric key cipher operations.
 *
 * Generic encrypt/decrypt wrapper for ciphers, handles operations across
 * multiple page boundaries by using temporary blocks.  In user context,
 * the kernel is given a chance to schedule us once per page.
 *
 * Copyright (c) 2015 Herbert Xu <herbert@gondor.apana.org.au>
 */

#include <crypto/internal/aead.h>
#include <crypto/internal/cipher.h>
#include <crypto/internal/skcipher.h>
#include <crypto/scatterwalk.h>
#include <linux/bug.h>
#include <linux/cryptouser.h>
#include <linux/compiler.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/rtnetlink.h>
#include <linux/seq_file.h>
#include <net/netlink.h>

#include "internal.h"

enum {
	SKCIPHER_WALK_PHYS = 1 << 0,
	SKCIPHER_WALK_SLOW = 1 << 1,
	SKCIPHER_WALK_COPY = 1 << 2,
	SKCIPHER_WALK_DIFF = 1 << 3,
	SKCIPHER_WALK_SLEEP = 1 << 4,
};

struct skcipher_walk_buffer {
	struct list_head entry;
	struct scatter_walk dst;
	unsigned int len;
	u8 *data;
	u8 buffer[];
};

static int skcipher_walk_next(struct skcipher_walk *walk);

static inline void skcipher_unmap(struct scatter_walk *walk, void *vaddr)
{
	if (PageHighMem(scatterwalk_page(walk)))
		kunmap_atomic(vaddr);
}

static inline void *skcipher_map(struct scatter_walk *walk)
{
	struct page *page = scatterwalk_page(walk);

	return (PageHighMem(page) ? kmap_atomic(page) : page_address(page)) +
	       offset_in_page(walk->offset);
}

static inline void skcipher_map_src(struct skcipher_walk *walk)
{
	walk->src.virt.addr = skcipher_map(&walk->in);
}

static inline void skcipher_map_dst(struct skcipher_walk *walk)
{
	walk->dst.virt.addr = skcipher_map(&walk->out);
}

static inline void skcipher_unmap_src(struct skcipher_walk *walk)
{
	skcipher_unmap(&walk->in, walk->src.virt.addr);
}

static inline void skcipher_unmap_dst(struct skcipher_walk *walk)
{
	skcipher_unmap(&walk->out, walk->dst.virt.addr);
}

static inline gfp_t skcipher_walk_gfp(struct skcipher_walk *walk)
{
	return walk->flags & SKCIPHER_WALK_SLEEP ? GFP_KERNEL : GFP_ATOMIC;
}

/* Get a spot of the specified length that does not straddle a page.
 * The caller needs to ensure that there is enough space for this operation.
 */
static inline u8 *skcipher_get_spot(u8 *start, unsigned int len)
{
	u8 *end_page = (u8 *)(((unsigned long)(start + len - 1)) & PAGE_MASK);

	return max(start, end_page);
}

static int skcipher_done_slow(struct skcipher_walk *walk, unsigned int bsize)
{
	u8 *addr;

	addr = (u8 *)ALIGN((unsigned long)walk->buffer, walk->alignmask + 1);
	addr = skcipher_get_spot(addr, bsize);
	scatterwalk_copychunks(addr, &walk->out, bsize,
			       (walk->flags & SKCIPHER_WALK_PHYS) ? 2 : 1);
	return 0;
}

int skcipher_walk_done(struct skcipher_walk *walk, int err)
{
	unsigned int n = walk->nbytes;
	unsigned int nbytes = 0;

	if (!n)
		goto finish;

	if (likely(err >= 0)) {
		n -= err;
		nbytes = walk->total - n;
	}

	if (likely(!(walk->flags & (SKCIPHER_WALK_PHYS |
				    SKCIPHER_WALK_SLOW |
				    SKCIPHER_WALK_COPY |
				    SKCIPHER_WALK_DIFF)))) {
unmap_src:
		skcipher_unmap_src(walk);
	} else if (walk->flags & SKCIPHER_WALK_DIFF) {
		skcipher_unmap_dst(walk);
		goto unmap_src;
	} else if (walk->flags & SKCIPHER_WALK_COPY) {
		skcipher_map_dst(walk);
		memcpy(walk->dst.virt.addr, walk->page, n);
		skcipher_unmap_dst(walk);
	} else if (unlikely(walk->flags & SKCIPHER_WALK_SLOW)) {
		if (err > 0) {
			/*
			 * Didn't process all bytes.  Either the algorithm is
			 * broken, or this was the last step and it turned out
			 * the message wasn't evenly divisible into blocks but
			 * the algorithm requires it.
			 */
			err = -EINVAL;
			nbytes = 0;
		} else
			n = skcipher_done_slow(walk, n);
	}

	if (err > 0)
		err = 0;

	walk->total = nbytes;
	walk->nbytes = 0;

	scatterwalk_advance(&walk->in, n);
	scatterwalk_advance(&walk->out, n);
	scatterwalk_done(&walk->in, 0, nbytes);
	scatterwalk_done(&walk->out, 1, nbytes);

	if (nbytes) {
		crypto_yield(walk->flags & SKCIPHER_WALK_SLEEP ?
			     CRYPTO_TFM_REQ_MAY_SLEEP : 0);
		return skcipher_walk_next(walk);
	}

finish:
	/* Short-circuit for the common/fast path. */
	if (!((unsigned long)walk->buffer | (unsigned long)walk->page))
		goto out;

	if (walk->flags & SKCIPHER_WALK_PHYS)
		goto out;

	if (walk->iv != walk->oiv)
		memcpy(walk->oiv, walk->iv, walk->ivsize);
	if (walk->buffer != walk->page)
		kfree(walk->buffer);
	if (walk->page)
		free_page((unsigned long)walk->page);

out:
	return err;
}
EXPORT_SYMBOL_GPL(skcipher_walk_done);

void skcipher_walk_complete(struct skcipher_walk *walk, int err)
{
	struct skcipher_walk_buffer *p, *tmp;

	list_for_each_entry_safe(p, tmp, &walk->buffers, entry) {
		u8 *data;

		if (err)
			goto done;

		data = p->data;
		if (!data) {
			data = PTR_ALIGN(&p->buffer[0], walk->alignmask + 1);
			data = skcipher_get_spot(data, walk->stride);
		}

		scatterwalk_copychunks(data, &p->dst, p->len, 1);

		if (offset_in_page(p->data) + p->len + walk->stride >
		    PAGE_SIZE)
			free_page((unsigned long)p->data);

done:
		list_del(&p->entry);
		kfree(p);
	}

	if (!err && walk->iv != walk->oiv)
		memcpy(walk->oiv, walk->iv, walk->ivsize);
	if (walk->buffer != walk->page)
		kfree(walk->buffer);
	if (walk->page)
		free_page((unsigned long)walk->page);
}
EXPORT_SYMBOL_GPL(skcipher_walk_complete);

static void skcipher_queue_write(struct skcipher_walk *walk,
				 struct skcipher_walk_buffer *p)
{
	p->dst = walk->out;
	list_add_tail(&p->entry, &walk->buffers);
}

static int skcipher_next_slow(struct skcipher_walk *walk, unsigned int bsize)
{
	bool phys = walk->flags & SKCIPHER_WALK_PHYS;
	unsigned alignmask = walk->alignmask;
	struct skcipher_walk_buffer *p;
	unsigned a;
	unsigned n;
	u8 *buffer;
	void *v;

	if (!phys) {
		if (!walk->buffer)
			walk->buffer = walk->page;
		buffer = walk->buffer;
		if (buffer)
			goto ok;
	}

	/* Start with the minimum alignment of kmalloc. */
	a = crypto_tfm_ctx_alignment() - 1;
	n = bsize;

	if (phys) {
		/* Calculate the minimum alignment of p->buffer. */
		a &= (sizeof(*p) ^ (sizeof(*p) - 1)) >> 1;
		n += sizeof(*p);
	}

	/* Minimum size to align p->buffer by alignmask. */
	n += alignmask & ~a;

	/* Minimum size to ensure p->buffer does not straddle a page. */
	n += (bsize - 1) & ~(alignmask | a);

	v = kzalloc(n, skcipher_walk_gfp(walk));
	if (!v)
		return skcipher_walk_done(walk, -ENOMEM);

	if (phys) {
		p = v;
		p->len = bsize;
		skcipher_queue_write(walk, p);
		buffer = p->buffer;
	} else {
		walk->buffer = v;
		buffer = v;
	}

ok:
	walk->dst.virt.addr = PTR_ALIGN(buffer, alignmask + 1);
	walk->dst.virt.addr = skcipher_get_spot(walk->dst.virt.addr, bsize);
	walk->src.virt.addr = walk->dst.virt.addr;

	scatterwalk_copychunks(walk->src.virt.addr, &walk->in, bsize, 0);

	walk->nbytes = bsize;
	walk->flags |= SKCIPHER_WALK_SLOW;

	return 0;
}

static int skcipher_next_copy(struct skcipher_walk *walk)
{
	struct skcipher_walk_buffer *p;
	u8 *tmp = walk->page;

	skcipher_map_src(walk);
	memcpy(tmp, walk->src.virt.addr, walk->nbytes);
	skcipher_unmap_src(walk);

	walk->src.virt.addr = tmp;
	walk->dst.virt.addr = tmp;

	if (!(walk->flags & SKCIPHER_WALK_PHYS))
		return 0;

	p = kmalloc(sizeof(*p), skcipher_walk_gfp(walk));
	if (!p)
		return -ENOMEM;

	p->data = walk->page;
	p->len = walk->nbytes;
	skcipher_queue_write(walk, p);

	if (offset_in_page(walk->page) + walk->nbytes + walk->stride >
	    PAGE_SIZE)
		walk->page = NULL;
	else
		walk->page += walk->nbytes;

	return 0;
}

static int skcipher_next_fast(struct skcipher_walk *walk)
{
	unsigned long diff;

	walk->src.phys.page = scatterwalk_page(&walk->in);
	walk->src.phys.offset = offset_in_page(walk->in.offset);
	walk->dst.phys.page = scatterwalk_page(&walk->out);
	walk->dst.phys.offset = offset_in_page(walk->out.offset);

	if (walk->flags & SKCIPHER_WALK_PHYS)
		return 0;

	diff = walk->src.phys.offset - walk->dst.phys.offset;
	diff |= walk->src.virt.page - walk->dst.virt.page;

	skcipher_map_src(walk);
	walk->dst.virt.addr = walk->src.virt.addr;

	if (diff) {
		walk->flags |= SKCIPHER_WALK_DIFF;
		skcipher_map_dst(walk);
	}

	return 0;
}

static int skcipher_walk_next(struct skcipher_walk *walk)
{
	unsigned int bsize;
	unsigned int n;
	int err;

	walk->flags &= ~(SKCIPHER_WALK_SLOW | SKCIPHER_WALK_COPY |
			 SKCIPHER_WALK_DIFF);

	n = walk->total;
	bsize = min(walk->stride, max(n, walk->blocksize));
	n = scatterwalk_clamp(&walk->in, n);
	n = scatterwalk_clamp(&walk->out, n);

	if (unlikely(n < bsize)) {
		if (unlikely(walk->total < walk->blocksize))
			return skcipher_walk_done(walk, -EINVAL);

slow_path:
		err = skcipher_next_slow(walk, bsize);
		goto set_phys_lowmem;
	}

	if (unlikely((walk->in.offset | walk->out.offset) & walk->alignmask)) {
		if (!walk->page) {
			gfp_t gfp = skcipher_walk_gfp(walk);

			walk->page = (void *)__get_free_page(gfp);
			if (!walk->page)
				goto slow_path;
		}

		walk->nbytes = min_t(unsigned, n,
				     PAGE_SIZE - offset_in_page(walk->page));
		walk->flags |= SKCIPHER_WALK_COPY;
		err = skcipher_next_copy(walk);
		goto set_phys_lowmem;
	}

	walk->nbytes = n;

	return skcipher_next_fast(walk);

set_phys_lowmem:
	if (!err && (walk->flags & SKCIPHER_WALK_PHYS)) {
		walk->src.phys.page = virt_to_page(walk->src.virt.addr);
		walk->dst.phys.page = virt_to_page(walk->dst.virt.addr);
		walk->src.phys.offset &= PAGE_SIZE - 1;
		walk->dst.phys.offset &= PAGE_SIZE - 1;
	}
	return err;
}

static int skcipher_copy_iv(struct skcipher_walk *walk)
{
	unsigned a = crypto_tfm_ctx_alignment() - 1;
	unsigned alignmask = walk->alignmask;
	unsigned ivsize = walk->ivsize;
	unsigned bs = walk->stride;
	unsigned aligned_bs;
	unsigned size;
	u8 *iv;

	aligned_bs = ALIGN(bs, alignmask + 1);

	/* Minimum size to align buffer by alignmask. */
	size = alignmask & ~a;

	if (walk->flags & SKCIPHER_WALK_PHYS)
		size += ivsize;
	else {
		size += aligned_bs + ivsize;

		/* Minimum size to ensure buffer does not straddle a page. */
		size += (bs - 1) & ~(alignmask | a);
	}

	walk->buffer = kmalloc(size, skcipher_walk_gfp(walk));
	if (!walk->buffer)
		return -ENOMEM;

	iv = PTR_ALIGN(walk->buffer, alignmask + 1);
	iv = skcipher_get_spot(iv, bs) + aligned_bs;

	walk->iv = memcpy(iv, walk->iv, walk->ivsize);
	return 0;
}

static int skcipher_walk_first(struct skcipher_walk *walk)
{
	if (WARN_ON_ONCE(in_irq()))
		return -EDEADLK;

	walk->buffer = NULL;
	if (unlikely(((unsigned long)walk->iv & walk->alignmask))) {
		int err = skcipher_copy_iv(walk);
		if (err)
			return err;
	}

	walk->page = NULL;

	return skcipher_walk_next(walk);
}

static int skcipher_walk_skcipher(struct skcipher_walk *walk,
				  struct skcipher_request *req)
{
	struct crypto_skcipher *tfm = crypto_skcipher_reqtfm(req);

	walk->total = req->cryptlen;
	walk->nbytes = 0;
	walk->iv = req->iv;
	walk->oiv = req->iv;

	if (unlikely(!walk->total))
		return 0;

	scatterwalk_start(&walk->in, req->src);
	scatterwalk_start(&walk->out, req->dst);

	walk->flags &= ~SKCIPHER_WALK_SLEEP;
	walk->flags |= req->base.flags & CRYPTO_TFM_REQ_MAY_SLEEP ?
		       SKCIPHER_WALK_SLEEP : 0;

	walk->blocksize = crypto_skcipher_blocksize(tfm);
	walk->stride = crypto_skcipher_walksize(tfm);
	walk->ivsize = crypto_skcipher_ivsize(tfm);
	walk->alignmask = crypto_skcipher_alignmask(tfm);

	return skcipher_walk_first(walk);
}

int skcipher_walk_virt(struct skcipher_walk *walk,
		       struct skcipher_request *req, bool atomic)
{
	int err;

	might_sleep_if(req->base.flags & CRYPTO_TFM_REQ_MAY_SLEEP);

	walk->flags &= ~SKCIPHER_WALK_PHYS;

	err = skcipher_walk_skcipher(walk, req);

	walk->flags &= atomic ? ~SKCIPHER_WALK_SLEEP : ~0;

	return err;
}
EXPORT_SYMBOL_GPL(skcipher_walk_virt);

int skcipher_walk_async(struct skcipher_walk *walk,
			struct skcipher_request *req)
{
	walk->flags |= SKCIPHER_WALK_PHYS;

	INIT_LIST_HEAD(&walk->buffers);

	return skcipher_walk_skcipher(walk, req);
}
EXPORT_SYMBOL_GPL(skcipher_walk_async);

static int skcipher_walk_aead_common(struct skcipher_walk *walk,
				     struct aead_request *req, bool atomic)
{
	struct crypto_aead *tfm = crypto_aead_reqtfm(req);
	int err;

	walk->nbytes = 0;
	walk->iv = req->iv;
	walk->oiv = req->iv;

	if (unlikely(!walk->total))
		return 0;

	walk->flags &= ~SKCIPHER_WALK_PHYS;

	scatterwalk_start(&walk->in, req->src);
	scatterwalk_start(&walk->out, req->dst);

	scatterwalk_copychunks(NULL, &walk->in, req->assoclen, 2);
	scatterwalk_copychunks(NULL, &walk->out, req->assoclen, 2);

	scatterwalk_done(&walk->in, 0, walk->total);
	scatterwalk_done(&walk->out, 0, walk->total);

	if (req->base.flags & CRYPTO_TFM_REQ_MAY_SLEEP)
		walk->flags |= SKCIPHER_WALK_SLEEP;
	else
		walk->flags &= ~SKCIPHER_WALK_SLEEP;

	walk->blocksize = crypto_aead_blocksize(tfm);
	walk->stride = crypto_aead_chunksize(tfm);
	walk->ivsize = crypto_aead_ivsize(tfm);
	walk->alignmask = crypto_aead_alignmask(tfm);

	err = skcipher_walk_first(walk);

	if (atomic)
		walk->flags &= ~SKCIPHER_WALK_SLEEP;

	return err;
}

int skcipher_walk_aead_encrypt(struct skcipher_walk *walk,
			       struct aead_request *req, bool atomic)
{
	walk->total = req->cryptlen;

	return skcipher_walk_aead_common(walk, req, atomic);
}
EXPORT_SYMBOL_GPL(skcipher_walk_aead_encrypt);

int skcipher_walk_aead_decrypt(struct skcipher_walk *walk,
			       struct aead_request *req, bool atomic)
{
	struct crypto_aead *tfm = crypto_aead_reqtfm(req);

	walk->total = req->cryptlen - crypto_aead_authsize(tfm);

	return skcipher_walk_aead_common(walk, req, atomic);
}
EXPORT_SYMBOL_GPL(skcipher_walk_aead_decrypt);

static void skcipher_set_needkey(struct crypto_skcipher *tfm)
{
	if (crypto_skcipher_max_keysize(tfm) != 0)
		crypto_skcipher_set_flags(tfm, CRYPTO_TFM_NEED_KEY);
}

static int skcipher_setkey_unaligned(struct crypto_skcipher *tfm,
				     const u8 *key, unsigned int keylen)
{
	unsigned long alignmask = crypto_skcipher_alignmask(tfm);
	struct skcipher_alg *cipher = crypto_skcipher_alg(tfm);
	u8 *buffer, *alignbuffer;
	unsigned long absize;
	int ret;

	absize = keylen + alignmask;
	buffer = kmalloc(absize, GFP_ATOMIC);
	if (!buffer)
		return -ENOMEM;

	alignbuffer = (u8 *)ALIGN((unsigned long)buffer, alignmask + 1);
	memcpy(alignbuffer, key, keylen);
	ret = cipher->setkey(tfm, alignbuffer, keylen);
	kfree_sensitive(buffer);
	return ret;
}

int crypto_skcipher_setkey(struct crypto_skcipher *tfm, const u8 *key,
			   unsigned int keylen)
{
	struct skcipher_alg *cipher = crypto_skcipher_alg(tfm);
	unsigned long alignmask = crypto_skcipher_alignmask(tfm);
	int err;

	if (keylen < cipher->min_keysize || keylen > cipher->max_keysize)
		return -EINVAL;

	if ((unsigned long)key & alignmask)
		err = skcipher_setkey_unaligned(tfm, key, keylen);
	else
		err = cipher->setkey(tfm, key, keylen);

	if (unlikely(err)) {
		skcipher_set_needkey(tfm);
		return err;
	}

	crypto_skcipher_clear_flags(tfm, CRYPTO_TFM_NEED_KEY);
	return 0;
}
EXPORT_SYMBOL_GPL(crypto_skcipher_setkey);

int crypto_skcipher_encrypt(struct skcipher_request *req)
{
	struct crypto_skcipher *tfm = crypto_skcipher_reqtfm(req);
	struct crypto_alg *alg = tfm->base.__crt_alg;
	unsigned int cryptlen = req->cryptlen;
	int ret;

	crypto_stats_get(alg);
	if (crypto_skcipher_get_flags(tfm) & CRYPTO_TFM_NEED_KEY)
		ret = -ENOKEY;
	else
		ret = crypto_skcipher_alg(tfm)->encrypt(req);
	crypto_stats_skcipher_encrypt(cryptlen, ret, alg);
	return ret;
}
EXPORT_SYMBOL_GPL(crypto_skcipher_encrypt);

int crypto_skcipher_decrypt(struct skcipher_request *req)
{
	struct crypto_skcipher *tfm = crypto_skcipher_reqtfm(req);
	struct crypto_alg *alg = tfm->base.__crt_alg;
	unsigned int cryptlen = req->cryptlen;
	int ret;

	crypto_stats_get(alg);
	if (crypto_skcipher_get_flags(tfm) & CRYPTO_TFM_NEED_KEY)
		ret = -ENOKEY;
	else
		ret = crypto_skcipher_alg(tfm)->decrypt(req);
	crypto_stats_skcipher_decrypt(cryptlen, ret, alg);
	return ret;
}
EXPORT_SYMBOL_GPL(crypto_skcipher_decrypt);

static void crypto_skcipher_exit_tfm(struct crypto_tfm *tfm)
{
	struct crypto_skcipher *skcipher = __crypto_skcipher_cast(tfm);
	struct skcipher_alg *alg = crypto_skcipher_alg(skcipher);

	alg->exit(skcipher);
}

static int crypto_skcipher_init_tfm(struct crypto_tfm *tfm)
{
	struct crypto_skcipher *skcipher = __crypto_skcipher_cast(tfm);
	struct skcipher_alg *alg = crypto_skcipher_alg(skcipher);

	skcipher_set_needkey(skcipher);

	if (alg->exit)
		skcipher->base.exit = crypto_skcipher_exit_tfm;

	if (alg->init)
		return alg->init(skcipher);

	return 0;
}

static void crypto_skcipher_free_instance(struct crypto_instance *inst)
{
	struct skcipher_instance *skcipher =
		container_of(inst, struct skcipher_instance, s.base);

	skcipher->free(skcipher);
}

static void crypto_skcipher_show(struct seq_file *m, struct crypto_alg *alg)
	__maybe_unused;
static void crypto_skcipher_show(struct seq_file *m, struct crypto_alg *alg)
{
	struct skcipher_alg *skcipher = container_of(alg, struct skcipher_alg,
						     base);

	seq_printf(m, "type         : skcipher\n");
	seq_printf(m, "async        : %s\n",
		   alg->cra_flags & CRYPTO_ALG_ASYNC ?  "yes" : "no");
	seq_printf(m, "blocksize    : %u\n", alg->cra_blocksize);
	seq_printf(m, "min keysize  : %u\n", skcipher->min_keysize);
	seq_printf(m, "max keysize  : %u\n", skcipher->max_keysize);
	seq_printf(m, "ivsize       : %u\n", skcipher->ivsize);
	seq_printf(m, "chunksize    : %u\n", skcipher->chunksize);
	seq_printf(m, "walksize     : %u\n", skcipher->walksize);
}

#ifdef CONFIG_NET
static int crypto_skcipher_report(struct sk_buff *skb, struct crypto_alg *alg)
{
	struct crypto_report_blkcipher rblkcipher;
	struct skcipher_alg *skcipher = container_of(alg, struct skcipher_alg,
						     base);

	memset(&rblkcipher, 0, sizeof(rblkcipher));

	strscpy(rblkcipher.type, "skcipher", sizeof(rblkcipher.type));
	strscpy(rblkcipher.geniv, "<none>", sizeof(rblkcipher.geniv));

	rblkcipher.blocksize = alg->cra_blocksize;
	rblkcipher.min_keysize = skcipher->min_keysize;
	rblkcipher.max_keysize = skcipher->max_keysize;
	rblkcipher.ivsize = skcipher->ivsize;

	return nla_put(skb, CRYPTOCFGA_REPORT_BLKCIPHER,
		       sizeof(rblkcipher), &rblkcipher);
}
#else
static int crypto_skcipher_report(struct sk_buff *skb, struct crypto_alg *alg)
{
	return -ENOSYS;
}
#endif

static const struct crypto_type crypto_skcipher_type = {
	.extsize = crypto_alg_extsize,
	.init_tfm = crypto_skcipher_init_tfm,
	.free = crypto_skcipher_free_instance,
#ifdef CONFIG_PROC_FS
	.show = crypto_skcipher_show,
#endif
	.report = crypto_skcipher_report,
	.maskclear = ~CRYPTO_ALG_TYPE_MASK,
	.maskset = CRYPTO_ALG_TYPE_MASK,
	.type = CRYPTO_ALG_TYPE_SKCIPHER,
	.tfmsize = offsetof(struct crypto_skcipher, base),
};

int crypto_grab_skcipher(struct crypto_skcipher_spawn *spawn,
			 struct crypto_instance *inst,
			 const char *name, u32 type, u32 mask)
{
	spawn->base.frontend = &crypto_skcipher_type;
	return crypto_grab_spawn(&spawn->base, inst, name, type, mask);
}
EXPORT_SYMBOL_GPL(crypto_grab_skcipher);

struct crypto_skcipher *crypto_alloc_skcipher(const char *alg_name,
					      u32 type, u32 mask)
{
	return crypto_alloc_tfm(alg_name, &crypto_skcipher_type, type, mask);
}
EXPORT_SYMBOL_GPL(crypto_alloc_skcipher);

struct crypto_sync_skcipher *crypto_alloc_sync_skcipher(
				const char *alg_name, u32 type, u32 mask)
{
	struct crypto_skcipher *tfm;

	/* Only sync algorithms allowed. */
	mask |= CRYPTO_ALG_ASYNC;

	tfm = crypto_alloc_tfm(alg_name, &crypto_skcipher_type, type, mask);

	/*
	 * Make sure we do not allocate something that might get used with
	 * an on-stack request: check the request size.
	 */
	if (!IS_ERR(tfm) && WARN_ON(crypto_skcipher_reqsize(tfm) >
				    MAX_SYNC_SKCIPHER_REQSIZE)) {
		crypto_free_skcipher(tfm);
		return ERR_PTR(-EINVAL);
	}

	return (struct crypto_sync_skcipher *)tfm;
}
EXPORT_SYMBOL_GPL(crypto_alloc_sync_skcipher);

int crypto_has_skcipher(const char *alg_name, u32 type, u32 mask)
{
	return crypto_type_has_alg(alg_name, &crypto_skcipher_type, type, mask);
}
EXPORT_SYMBOL_GPL(crypto_has_skcipher);

static int skcipher_prepare_alg(struct skcipher_alg *alg)
{
	struct crypto_alg *base = &alg->base;

	if (alg->ivsize > PAGE_SIZE / 8 || alg->chunksize > PAGE_SIZE / 8 ||
	    alg->walksize > PAGE_SIZE / 8)
		return -EINVAL;

	if (!alg->chunksize)
		alg->chunksize = base->cra_blocksize;
	if (!alg->walksize)
		alg->walksize = alg->chunksize;

	base->cra_type = &crypto_skcipher_type;
	base->cra_flags &= ~CRYPTO_ALG_TYPE_MASK;
	base->cra_flags |= CRYPTO_ALG_TYPE_SKCIPHER;

	return 0;
}

int crypto_register_skcipher(struct skcipher_alg *alg)
{
	struct crypto_alg *base = &alg->base;
	int err;

	err = skcipher_prepare_alg(alg);
	if (err)
		return err;

	return crypto_register_alg(base);
}
EXPORT_SYMBOL_GPL(crypto_register_skcipher);

void crypto_unregister_skcipher(struct skcipher_alg *alg)
{
	crypto_unregister_alg(&alg->base);
}
EXPORT_SYMBOL_GPL(crypto_unregister_skcipher);

int crypto_register_skciphers(struct skcipher_alg *algs, int count)
{
	int i, ret;

	for (i = 0; i < count; i++) {
		ret = crypto_register_skcipher(&algs[i]);
		if (ret)
			goto err;
	}

	return 0;

err:
	for (--i; i >= 0; --i)
		crypto_unregister_skcipher(&algs[i]);

	return ret;
}
EXPORT_SYMBOL_GPL(crypto_register_skciphers);

void crypto_unregister_skciphers(struct skcipher_alg *algs, int count)
{
	int i;

	for (i = count - 1; i >= 0; --i)
		crypto_unregister_skcipher(&algs[i]);
}
EXPORT_SYMBOL_GPL(crypto_unregister_skciphers);

int skcipher_register_instance(struct crypto_template *tmpl,
			   struct skcipher_instance *inst)
{
	int err;

	if (WARN_ON(!inst->free))
		return -EINVAL;

	err = skcipher_prepare_alg(&inst->alg);
	if (err)
		return err;

	return crypto_register_instance(tmpl, skcipher_crypto_instance(inst));
}
EXPORT_SYMBOL_GPL(skcipher_register_instance);

static int skcipher_setkey_simple(struct crypto_skcipher *tfm, const u8 *key,
				  unsigned int keylen)
{
	struct crypto_cipher *cipher = skcipher_cipher_simple(tfm);

	crypto_cipher_clear_flags(cipher, CRYPTO_TFM_REQ_MASK);
	crypto_cipher_set_flags(cipher, crypto_skcipher_get_flags(tfm) &
				CRYPTO_TFM_REQ_MASK);
	return crypto_cipher_setkey(cipher, key, keylen);
}

static int skcipher_init_tfm_simple(struct crypto_skcipher *tfm)
{
	struct skcipher_instance *inst = skcipher_alg_instance(tfm);
	struct crypto_cipher_spawn *spawn = skcipher_instance_ctx(inst);
	struct skcipher_ctx_simple *ctx = crypto_skcipher_ctx(tfm);
	struct crypto_cipher *cipher;

	cipher = crypto_spawn_cipher(spawn);
	if (IS_ERR(cipher))
		return PTR_ERR(cipher);

	ctx->cipher = cipher;
	return 0;
}

static void skcipher_exit_tfm_simple(struct crypto_skcipher *tfm)
{
	struct skcipher_ctx_simple *ctx = crypto_skcipher_ctx(tfm);

	crypto_free_cipher(ctx->cipher);
}

static void skcipher_free_instance_simple(struct skcipher_instance *inst)
{
	crypto_drop_cipher(skcipher_instance_ctx(inst));
	kfree(inst);
}

/**
 * skcipher_alloc_instance_simple - allocate instance of simple block cipher mode
 *
 * Allocate an skcipher_instance for a simple block cipher mode of operation,
 * e.g. cbc or ecb.  The instance context will have just a single crypto_spawn,
 * that for the underlying cipher.  The {min,max}_keysize, ivsize, blocksize,
 * alignmask, and priority are set from the underlying cipher but can be
 * overridden if needed.  The tfm context defaults to skcipher_ctx_simple, and
 * default ->setkey(), ->init(), and ->exit() methods are installed.
 *
 * @tmpl: the template being instantiated
 * @tb: the template parameters
 *
 * Return: a pointer to the new instance, or an ERR_PTR().  The caller still
 *	   needs to register the instance.
 */
struct skcipher_instance *skcipher_alloc_instance_simple(
	struct crypto_template *tmpl, struct rtattr **tb)
{
	u32 mask;
	struct skcipher_instance *inst;
	struct crypto_cipher_spawn *spawn;
	struct crypto_alg *cipher_alg;
	int err;

	err = crypto_check_attr_type(tb, CRYPTO_ALG_TYPE_SKCIPHER, &mask);
	if (err)
		return ERR_PTR(err);

	inst = kzalloc(sizeof(*inst) + sizeof(*spawn), GFP_KERNEL);
	if (!inst)
		return ERR_PTR(-ENOMEM);
	spawn = skcipher_instance_ctx(inst);

	err = crypto_grab_cipher(spawn, skcipher_crypto_instance(inst),
				 crypto_attr_alg_name(tb[1]), 0, mask);
	if (err)
		goto err_free_inst;
	cipher_alg = crypto_spawn_cipher_alg(spawn);

	err = crypto_inst_setname(skcipher_crypto_instance(inst), tmpl->name,
				  cipher_alg);
	if (err)
		goto err_free_inst;

	inst->free = skcipher_free_instance_simple;

	/* Default algorithm properties, can be overridden */
	inst->alg.base.cra_blocksize = cipher_alg->cra_blocksize;
	inst->alg.base.cra_alignmask = cipher_alg->cra_alignmask;
	inst->alg.base.cra_priority = cipher_alg->cra_priority;
	inst->alg.min_keysize = cipher_alg->cra_cipher.cia_min_keysize;
	inst->alg.max_keysize = cipher_alg->cra_cipher.cia_max_keysize;
	inst->alg.ivsize = cipher_alg->cra_blocksize;

	/* Use skcipher_ctx_simple by default, can be overridden */
	inst->alg.base.cra_ctxsize = sizeof(struct skcipher_ctx_simple);
	inst->alg.setkey = skcipher_setkey_simple;
	inst->alg.init = skcipher_init_tfm_simple;
	inst->alg.exit = skcipher_exit_tfm_simple;

	return inst;

err_free_inst:
	skcipher_free_instance_simple(inst);
	return ERR_PTR(err);
}
EXPORT_SYMBOL_GPL(skcipher_alloc_instance_simple);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Symmetric key cipher type");
MODULE_IMPORT_NS(CRYPTO_INTERNAL);
