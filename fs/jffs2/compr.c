/*
 * JFFS2 -- Journalling Flash File System, Version 2.
 *
 * Copyright © 2001-2007 Red Hat, Inc.
 * Copyright © 2004-2010 David Woodhouse <dwmw2@infradead.org>
 * Copyright © 2004 Ferenc Havasi <havasi@inf.u-szeged.hu>,
 *		    University of Szeged, Hungary
 *
 * Created by Arjan van de Ven <arjan@infradead.org>
 *
 * For licensing information, see the file 'LICENCE' in this directory.
 *
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include "compr.h"

static DEFINE_SPINLOCK(jffs2_compressor_list_lock);

/* Available compressors are on this list */
static LIST_HEAD(jffs2_compressor_list);

/* Actual compression mode */
static int jffs2_compression_mode = JFFS2_COMPR_MODE_PRIORITY;

/* Statistics for blocks stored without compression */
static uint32_t none_stat_compr_blocks=0,none_stat_decompr_blocks=0,none_stat_compr_size=0;


/*
 * Return 1 to use this compression
 */
static int jffs2_is_best_compression(struct jffs2_compressor *this,
		struct jffs2_compressor *best, uint32_t size, uint32_t bestsize)
{
	switch (jffs2_compression_mode) {
	case JFFS2_COMPR_MODE_SIZE:
		if (bestsize > size)
			return 1;
		return 0;
	case JFFS2_COMPR_MODE_FAVOURLZO:
		if ((this->compr == JFFS2_COMPR_LZO) && (bestsize > size))
			return 1;
		if ((best->compr != JFFS2_COMPR_LZO) && (bestsize > size))
			return 1;
		if ((this->compr == JFFS2_COMPR_LZO) && (bestsize > (size * FAVOUR_LZO_PERCENT / 100)))
			return 1;
		if ((bestsize * FAVOUR_LZO_PERCENT / 100) > size)
			return 1;

		return 0;
	}
	/* Shouldn't happen */
	return 0;
}

/*
 * jffs2_selected_compress:
 * @compr: Explicit compression type to use (ie, JFFS2_COMPR_ZLIB).
 *	If 0, just take the first available compression mode.
 * @data_in: Pointer to uncompressed data
 * @cpage_out: Pointer to returned pointer to buffer for compressed data
 * @datalen: On entry, holds the amount of data available for compression.
 *	On exit, expected to hold the amount of data actually compressed.
 * @cdatalen: On entry, holds the amount of space available for compressed
 *	data. On exit, expected to hold the actual size of the compressed
 *	data.
 *
 * Returns: the compression type used.  Zero is used to show that the data
 * could not be compressed; probably because we couldn't find the requested
 * compression mode.
 */
static int jffs2_selected_compress(u8 compr, unsigned char *data_in,
		unsigned char **cpage_out, u32 *datalen, u32 *cdatalen)
{
	struct jffs2_compressor *this;
	int err, ret = JFFS2_COMPR_NONE;
	uint32_t orig_slen, orig_dlen;
	char *output_buf;

	output_buf = kmalloc(*cdatalen, GFP_KERNEL);
	if (!output_buf) {
		pr_warn("No memory for compressor allocation. Compression failed.\n");
		return ret;
	}
	orig_slen = *datalen;
	orig_dlen = *cdatalen;
	spin_lock(&jffs2_compressor_list_lock);
	list_for_each_entry(this, &jffs2_compressor_list, list) {
		/* Skip decompress-only and disabled modules */
		if (!this->compress || this->disabled)
			continue;

		/* Skip if not the desired compression type */
		if (compr && (compr != this->compr))
			continue;

		/*
		 * Either compression type was unspecified, or we found our
		 * compressor; either way, we're good to go.
		 */
		this->usecount++;
		spin_unlock(&jffs2_compressor_list_lock);

		*datalen  = orig_slen;
		*cdatalen = orig_dlen;
		err = this->compress(data_in, output_buf, datalen, cdatalen);

		spin_lock(&jffs2_compressor_list_lock);
		this->usecount--;
		if (!err) {
			/* Success */
			ret = this->compr;
			this->stat_compr_blocks++;
			this->stat_compr_orig_size += *datalen;
			this->stat_compr_new_size += *cdatalen;
			break;
		}
	}
	spin_unlock(&jffs2_compressor_list_lock);
	if (ret == JFFS2_COMPR_NONE)
		kfree(output_buf);
	else
		*cpage_out = output_buf;

	return ret;
}

/* jffs2_compress:
 * @data_in: Pointer to uncompressed data
 * @cpage_out: Pointer to returned pointer to buffer for compressed data
 * @datalen: On entry, holds the amount of data available for compression.
 *	On exit, expected to hold the amount of data actually compressed.
 * @cdatalen: On entry, holds the amount of space available for compressed
 *	data. On exit, expected to hold the actual size of the compressed
 *	data.
 *
 * Returns: Lower byte to be stored with data indicating compression type used.
 * Zero is used to show that the data could not be compressed - the
 * compressed version was actually larger than the original.
 * Upper byte will be used later. (soon)
 *
 * If the cdata buffer isn't large enough to hold all the uncompressed data,
 * jffs2_compress should compress as much as will fit, and should set
 * *datalen accordingly to show the amount of data which were compressed.
 */
uint16_t jffs2_compress(struct jffs2_sb_info *c, struct jffs2_inode_info *f,
			unsigned char *data_in, unsigned char **cpage_out,
			uint32_t *datalen, uint32_t *cdatalen)
{
	int ret = JFFS2_COMPR_NONE;
	int mode, compr_ret;
	struct jffs2_compressor *this, *best=NULL;
	unsigned char *output_buf = NULL, *tmp_buf;
	uint32_t orig_slen, orig_dlen;
	uint32_t best_slen=0, best_dlen=0;

	if (c->mount_opts.override_compr)
		mode = c->mount_opts.compr;
	else
		mode = jffs2_compression_mode;

	switch (mode) {
	case JFFS2_COMPR_MODE_NONE:
		break;
	case JFFS2_COMPR_MODE_PRIORITY:
		ret = jffs2_selected_compress(0, data_in, cpage_out, datalen,
				cdatalen);
		break;
	case JFFS2_COMPR_MODE_SIZE:
	case JFFS2_COMPR_MODE_FAVOURLZO:
		orig_slen = *datalen;
		orig_dlen = *cdatalen;
		spin_lock(&jffs2_compressor_list_lock);
		list_for_each_entry(this, &jffs2_compressor_list, list) {
			/* Skip decompress-only backwards-compatibility and disabled modules */
			if ((!this->compress)||(this->disabled))
				continue;
			/* Allocating memory for output buffer if necessary */
			if ((this->compr_buf_size < orig_slen) && (this->compr_buf)) {
				spin_unlock(&jffs2_compressor_list_lock);
				kfree(this->compr_buf);
				spin_lock(&jffs2_compressor_list_lock);
				this->compr_buf_size=0;
				this->compr_buf=NULL;
			}
			if (!this->compr_buf) {
				spin_unlock(&jffs2_compressor_list_lock);
				tmp_buf = kmalloc(orig_slen, GFP_KERNEL);
				spin_lock(&jffs2_compressor_list_lock);
				if (!tmp_buf) {
					pr_warn("No memory for compressor allocation. (%d bytes)\n",
						orig_slen);
					continue;
				}
				else {
					this->compr_buf = tmp_buf;
					this->compr_buf_size = orig_slen;
				}
			}
			this->usecount++;
			spin_unlock(&jffs2_compressor_list_lock);
			*datalen  = orig_slen;
			*cdatalen = orig_dlen;
			compr_ret = this->compress(data_in, this->compr_buf, datalen, cdatalen);
			spin_lock(&jffs2_compressor_list_lock);
			this->usecount--;
			if (!compr_ret) {
				if (((!best_dlen) || jffs2_is_best_compression(this, best, *cdatalen, best_dlen))
						&& (*cdatalen < *datalen)) {
					best_dlen = *cdatalen;
					best_slen = *datalen;
					best = this;
				}
			}
		}
		if (best_dlen) {
			*cdatalen = best_dlen;
			*datalen  = best_slen;
			output_buf = best->compr_buf;
			best->compr_buf = NULL;
			best->compr_buf_size = 0;
			best->stat_compr_blocks++;
			best->stat_compr_orig_size += best_slen;
			best->stat_compr_new_size  += best_dlen;
			ret = best->compr;
			*cpage_out = output_buf;
		}
		spin_unlock(&jffs2_compressor_list_lock);
		break;
	case JFFS2_COMPR_MODE_FORCELZO:
		ret = jffs2_selected_compress(JFFS2_COMPR_LZO, data_in,
				cpage_out, datalen, cdatalen);
		break;
	case JFFS2_COMPR_MODE_FORCEZLIB:
		ret = jffs2_selected_compress(JFFS2_COMPR_ZLIB, data_in,
				cpage_out, datalen, cdatalen);
		break;
	default:
		pr_err("unknown compression mode\n");
	}

	if (ret == JFFS2_COMPR_NONE) {
		*cpage_out = data_in;
		*datalen = *cdatalen;
		none_stat_compr_blocks++;
		none_stat_compr_size += *datalen;
	}
	return ret;
}

int jffs2_decompress(struct jffs2_sb_info *c, struct jffs2_inode_info *f,
		     uint16_t comprtype, unsigned char *cdata_in,
		     unsigned char *data_out, uint32_t cdatalen, uint32_t datalen)
{
	struct jffs2_compressor *this;
	int ret;

	/* Older code had a bug where it would write non-zero 'usercompr'
	   fields. Deal with it. */
	if ((comprtype & 0xff) <= JFFS2_COMPR_ZLIB)
		comprtype &= 0xff;

	switch (comprtype & 0xff) {
	case JFFS2_COMPR_NONE:
		/* This should be special-cased elsewhere, but we might as well deal with it */
		memcpy(data_out, cdata_in, datalen);
		none_stat_decompr_blocks++;
		break;
	case JFFS2_COMPR_ZERO:
		memset(data_out, 0, datalen);
		break;
	default:
		spin_lock(&jffs2_compressor_list_lock);
		list_for_each_entry(this, &jffs2_compressor_list, list) {
			if (comprtype == this->compr) {
				this->usecount++;
				spin_unlock(&jffs2_compressor_list_lock);
				ret = this->decompress(cdata_in, data_out, cdatalen, datalen);
				spin_lock(&jffs2_compressor_list_lock);
				if (ret) {
					pr_warn("Decompressor \"%s\" returned %d\n",
						this->name, ret);
				}
				else {
					this->stat_decompr_blocks++;
				}
				this->usecount--;
				spin_unlock(&jffs2_compressor_list_lock);
				return ret;
			}
		}
		pr_warn("compression type 0x%02x not available\n", comprtype);
		spin_unlock(&jffs2_compressor_list_lock);
		return -EIO;
	}
	return 0;
}

int jffs2_register_compressor(struct jffs2_compressor *comp)
{
	struct jffs2_compressor *this;

	if (!comp->name) {
		pr_warn("NULL compressor name at registering JFFS2 compressor. Failed.\n");
		return -1;
	}
	comp->compr_buf_size=0;
	comp->compr_buf=NULL;
	comp->usecount=0;
	comp->stat_compr_orig_size=0;
	comp->stat_compr_new_size=0;
	comp->stat_compr_blocks=0;
	comp->stat_decompr_blocks=0;
	jffs2_dbg(1, "Registering JFFS2 compressor \"%s\"\n", comp->name);

	spin_lock(&jffs2_compressor_list_lock);

	list_for_each_entry(this, &jffs2_compressor_list, list) {
		if (this->priority < comp->priority) {
			list_add(&comp->list, this->list.prev);
			goto out;
		}
	}
	list_add_tail(&comp->list, &jffs2_compressor_list);
out:
	D2(list_for_each_entry(this, &jffs2_compressor_list, list) {
		printk(KERN_DEBUG "Compressor \"%s\", prio %d\n", this->name, this->priority);
	})

	spin_unlock(&jffs2_compressor_list_lock);

	return 0;
}

int jffs2_unregister_compressor(struct jffs2_compressor *comp)
{
	D2(struct jffs2_compressor *this);

	jffs2_dbg(1, "Unregistering JFFS2 compressor \"%s\"\n", comp->name);

	spin_lock(&jffs2_compressor_list_lock);

	if (comp->usecount) {
		spin_unlock(&jffs2_compressor_list_lock);
		pr_warn("Compressor module is in use. Unregister failed.\n");
		return -1;
	}
	list_del(&comp->list);

	D2(list_for_each_entry(this, &jffs2_compressor_list, list) {
		printk(KERN_DEBUG "Compressor \"%s\", prio %d\n", this->name, this->priority);
	})
	spin_unlock(&jffs2_compressor_list_lock);
	return 0;
}

void jffs2_free_comprbuf(unsigned char *comprbuf, unsigned char *orig)
{
	if (orig != comprbuf)
		kfree(comprbuf);
}

int __init jffs2_compressors_init(void)
{
/* Registering compressors */
#ifdef CONFIG_JFFS2_ZLIB
	jffs2_zlib_init();
#endif
#ifdef CONFIG_JFFS2_RTIME
	jffs2_rtime_init();
#endif
#ifdef CONFIG_JFFS2_RUBIN
	jffs2_rubinmips_init();
	jffs2_dynrubin_init();
#endif
#ifdef CONFIG_JFFS2_LZO
	jffs2_lzo_init();
#endif
/* Setting default compression mode */
#ifdef CONFIG_JFFS2_CMODE_NONE
	jffs2_compression_mode = JFFS2_COMPR_MODE_NONE;
	jffs2_dbg(1, "default compression mode: none\n");
#else
#ifdef CONFIG_JFFS2_CMODE_SIZE
	jffs2_compression_mode = JFFS2_COMPR_MODE_SIZE;
	jffs2_dbg(1, "default compression mode: size\n");
#else
#ifdef CONFIG_JFFS2_CMODE_FAVOURLZO
	jffs2_compression_mode = JFFS2_COMPR_MODE_FAVOURLZO;
	jffs2_dbg(1, "default compression mode: favourlzo\n");
#else
	jffs2_dbg(1, "default compression mode: priority\n");
#endif
#endif
#endif
	return 0;
}

int jffs2_compressors_exit(void)
{
/* Unregistering compressors */
#ifdef CONFIG_JFFS2_LZO
	jffs2_lzo_exit();
#endif
#ifdef CONFIG_JFFS2_RUBIN
	jffs2_dynrubin_exit();
	jffs2_rubinmips_exit();
#endif
#ifdef CONFIG_JFFS2_RTIME
	jffs2_rtime_exit();
#endif
#ifdef CONFIG_JFFS2_ZLIB
	jffs2_zlib_exit();
#endif
	return 0;
}
