// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Cryptographic API.
 *
 * Cipher operations.
 *
 * Copyright (c) 2002 James Morris <jmorris@intercode.com.au>
 *               2002 Adam J. Richter <adam@yggdrasil.com>
 *               2004 Jean-Luc Cooke <jlcooke@certainkey.com>
 */

#include <crypto/scatterwalk.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/scatterlist.h>

static inline void memcpy_dir(void *buf, void *sgdata, size_t nbytes, int out)
{
	void *src = out ? buf : sgdata;
	void *dst = out ? sgdata : buf;

	memcpy(dst, src, nbytes);
}

void scatterwalk_copychunks(void *buf, struct scatter_walk *walk,
			    size_t nbytes, int out)
{
	for (;;) {
		unsigned int len_this_page = scatterwalk_pagelen(walk);
		u8 *vaddr;

		if (len_this_page > nbytes)
			len_this_page = nbytes;

		if (out != 2) {
			vaddr = scatterwalk_map(walk);
			memcpy_dir(buf, vaddr, len_this_page, out);
			scatterwalk_unmap(vaddr);
		}

		scatterwalk_advance(walk, len_this_page);

		if (nbytes == len_this_page)
			break;

		buf += len_this_page;
		nbytes -= len_this_page;

		scatterwalk_pagedone(walk, out & 1, 1);
	}
}
EXPORT_SYMBOL_GPL(scatterwalk_copychunks);

void scatterwalk_map_and_copy(void *buf, struct scatterlist *sg,
			      unsigned int start, unsigned int nbytes, int out)
{
	struct scatter_walk walk;
	struct scatterlist tmp[2];

	if (!nbytes)
		return;

	sg = scatterwalk_ffwd(tmp, sg, start);

	scatterwalk_start(&walk, sg);
	scatterwalk_copychunks(buf, &walk, nbytes, out);
	scatterwalk_done(&walk, out, 0);
}
EXPORT_SYMBOL_GPL(scatterwalk_map_and_copy);

struct scatterlist *scatterwalk_ffwd(struct scatterlist dst[2],
				     struct scatterlist *src,
				     unsigned int len)
{
	for (;;) {
		if (!len)
			return src;

		if (src->length > len)
			break;

		len -= src->length;
		src = sg_next(src);
	}

	sg_init_table(dst, 2);
	sg_set_page(dst, sg_page(src), src->length - len, src->offset + len);
	scatterwalk_crypto_chain(dst, sg_next(src), 2);

	return dst;
}
EXPORT_SYMBOL_GPL(scatterwalk_ffwd);
