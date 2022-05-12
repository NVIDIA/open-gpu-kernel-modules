/* SPDX-License-Identifier: GPL-2.0 */
/*
 * XDR standard data types and function declarations
 *
 * Copyright (C) 1995-1997 Olaf Kirch <okir@monad.swb.de>
 *
 * Based on:
 *   RFC 4506 "XDR: External Data Representation Standard", May 2006
 */

#ifndef _SUNRPC_XDR_H_
#define _SUNRPC_XDR_H_

#include <linux/uio.h>
#include <asm/byteorder.h>
#include <asm/unaligned.h>
#include <linux/scatterlist.h>

struct bio_vec;
struct rpc_rqst;

/*
 * Size of an XDR encoding unit in bytes, i.e. 32 bits,
 * as defined in Section 3 of RFC 4506. All encoded
 * XDR data items are aligned on a boundary of 32 bits.
 */
#define XDR_UNIT		sizeof(__be32)

/*
 * Buffer adjustment
 */
#define XDR_QUADLEN(l)		(((l) + 3) >> 2)

/*
 * Generic opaque `network object.'
 */
#define XDR_MAX_NETOBJ		1024
struct xdr_netobj {
	unsigned int		len;
	u8 *			data;
};

/*
 * Basic structure for transmission/reception of a client XDR message.
 * Features a header (for a linear buffer containing RPC headers
 * and the data payload for short messages), and then an array of
 * pages.
 * The tail iovec allows you to append data after the page array. Its
 * main interest is for appending padding to the pages in order to
 * satisfy the int_32-alignment requirements in RFC1832.
 *
 * For the future, we might want to string several of these together
 * in a list if anybody wants to make use of NFSv4 COMPOUND
 * operations and/or has a need for scatter/gather involving pages.
 */
struct xdr_buf {
	struct kvec	head[1],	/* RPC header + non-page data */
			tail[1];	/* Appended after page data */

	struct bio_vec	*bvec;
	struct page **	pages;		/* Array of pages */
	unsigned int	page_base,	/* Start of page data */
			page_len,	/* Length of page data */
			flags;		/* Flags for data disposition */
#define XDRBUF_READ		0x01		/* target of file read */
#define XDRBUF_WRITE		0x02		/* source of file write */
#define XDRBUF_SPARSE_PAGES	0x04		/* Page array is sparse */

	unsigned int	buflen,		/* Total length of storage buffer */
			len;		/* Length of XDR encoded message */
};

static inline void
xdr_buf_init(struct xdr_buf *buf, void *start, size_t len)
{
	buf->head[0].iov_base = start;
	buf->head[0].iov_len = len;
	buf->tail[0].iov_len = 0;
	buf->pages = NULL;
	buf->page_len = 0;
	buf->flags = 0;
	buf->len = 0;
	buf->buflen = len;
}

/*
 * pre-xdr'ed macros.
 */

#define	xdr_zero	cpu_to_be32(0)
#define	xdr_one		cpu_to_be32(1)
#define	xdr_two		cpu_to_be32(2)

#define	rpc_auth_null	cpu_to_be32(RPC_AUTH_NULL)
#define	rpc_auth_unix	cpu_to_be32(RPC_AUTH_UNIX)
#define	rpc_auth_short	cpu_to_be32(RPC_AUTH_SHORT)
#define	rpc_auth_gss	cpu_to_be32(RPC_AUTH_GSS)

#define	rpc_call	cpu_to_be32(RPC_CALL)
#define	rpc_reply	cpu_to_be32(RPC_REPLY)

#define	rpc_msg_accepted	cpu_to_be32(RPC_MSG_ACCEPTED)

#define	rpc_success		cpu_to_be32(RPC_SUCCESS)
#define	rpc_prog_unavail	cpu_to_be32(RPC_PROG_UNAVAIL)
#define	rpc_prog_mismatch	cpu_to_be32(RPC_PROG_MISMATCH)
#define	rpc_proc_unavail	cpu_to_be32(RPC_PROC_UNAVAIL)
#define	rpc_garbage_args	cpu_to_be32(RPC_GARBAGE_ARGS)
#define	rpc_system_err		cpu_to_be32(RPC_SYSTEM_ERR)
#define	rpc_drop_reply		cpu_to_be32(RPC_DROP_REPLY)

#define	rpc_mismatch		cpu_to_be32(RPC_MISMATCH)
#define	rpc_auth_error		cpu_to_be32(RPC_AUTH_ERROR)

#define	rpc_auth_ok		cpu_to_be32(RPC_AUTH_OK)
#define	rpc_autherr_badcred	cpu_to_be32(RPC_AUTH_BADCRED)
#define	rpc_autherr_rejectedcred cpu_to_be32(RPC_AUTH_REJECTEDCRED)
#define	rpc_autherr_badverf	cpu_to_be32(RPC_AUTH_BADVERF)
#define	rpc_autherr_rejectedverf cpu_to_be32(RPC_AUTH_REJECTEDVERF)
#define	rpc_autherr_tooweak	cpu_to_be32(RPC_AUTH_TOOWEAK)
#define	rpcsec_gsserr_credproblem	cpu_to_be32(RPCSEC_GSS_CREDPROBLEM)
#define	rpcsec_gsserr_ctxproblem	cpu_to_be32(RPCSEC_GSS_CTXPROBLEM)

/*
 * Miscellaneous XDR helper functions
 */
__be32 *xdr_encode_opaque_fixed(__be32 *p, const void *ptr, unsigned int len);
__be32 *xdr_encode_opaque(__be32 *p, const void *ptr, unsigned int len);
__be32 *xdr_encode_string(__be32 *p, const char *s);
__be32 *xdr_decode_string_inplace(__be32 *p, char **sp, unsigned int *lenp,
			unsigned int maxlen);
__be32 *xdr_encode_netobj(__be32 *p, const struct xdr_netobj *);
__be32 *xdr_decode_netobj(__be32 *p, struct xdr_netobj *);

void	xdr_inline_pages(struct xdr_buf *, unsigned int,
			 struct page **, unsigned int, unsigned int);
void	xdr_terminate_string(const struct xdr_buf *, const u32);
size_t	xdr_buf_pagecount(const struct xdr_buf *buf);
int	xdr_alloc_bvec(struct xdr_buf *buf, gfp_t gfp);
void	xdr_free_bvec(struct xdr_buf *buf);

static inline __be32 *xdr_encode_array(__be32 *p, const void *s, unsigned int len)
{
	return xdr_encode_opaque(p, s, len);
}

/*
 * Decode 64bit quantities (NFSv3 support)
 */
static inline __be32 *
xdr_encode_hyper(__be32 *p, __u64 val)
{
	put_unaligned_be64(val, p);
	return p + 2;
}

static inline __be32 *
xdr_decode_hyper(__be32 *p, __u64 *valp)
{
	*valp = get_unaligned_be64(p);
	return p + 2;
}

static inline __be32 *
xdr_decode_opaque_fixed(__be32 *p, void *ptr, unsigned int len)
{
	memcpy(ptr, p, len);
	return p + XDR_QUADLEN(len);
}

static inline void xdr_netobj_dup(struct xdr_netobj *dst,
				  struct xdr_netobj *src, gfp_t gfp_mask)
{
	dst->data = kmemdup(src->data, src->len, gfp_mask);
	dst->len = src->len;
}

/*
 * Adjust kvec to reflect end of xdr'ed data (RPC client XDR)
 */
static inline int
xdr_adjust_iovec(struct kvec *iov, __be32 *p)
{
	return iov->iov_len = ((u8 *) p - (u8 *) iov->iov_base);
}

/*
 * XDR buffer helper functions
 */
extern void xdr_shift_buf(struct xdr_buf *, size_t);
extern void xdr_buf_from_iov(const struct kvec *, struct xdr_buf *);
extern int xdr_buf_subsegment(const struct xdr_buf *, struct xdr_buf *, unsigned int, unsigned int);
extern void xdr_buf_trim(struct xdr_buf *, unsigned int);
extern int read_bytes_from_xdr_buf(const struct xdr_buf *, unsigned int, void *, unsigned int);
extern int write_bytes_to_xdr_buf(const struct xdr_buf *, unsigned int, void *, unsigned int);

extern int xdr_encode_word(const struct xdr_buf *, unsigned int, u32);
extern int xdr_decode_word(const struct xdr_buf *, unsigned int, u32 *);

struct xdr_array2_desc;
typedef int (*xdr_xcode_elem_t)(struct xdr_array2_desc *desc, void *elem);
struct xdr_array2_desc {
	unsigned int elem_size;
	unsigned int array_len;
	unsigned int array_maxlen;
	xdr_xcode_elem_t xcode;
};

extern int xdr_decode_array2(const struct xdr_buf *buf, unsigned int base,
			     struct xdr_array2_desc *desc);
extern int xdr_encode_array2(const struct xdr_buf *buf, unsigned int base,
			     struct xdr_array2_desc *desc);
extern void _copy_from_pages(char *p, struct page **pages, size_t pgbase,
			     size_t len);

/*
 * Provide some simple tools for XDR buffer overflow-checking etc.
 */
struct xdr_stream {
	__be32 *p;		/* start of available buffer */
	struct xdr_buf *buf;	/* XDR buffer to read/write */

	__be32 *end;		/* end of available buffer space */
	struct kvec *iov;	/* pointer to the current kvec */
	struct kvec scratch;	/* Scratch buffer */
	struct page **page_ptr;	/* pointer to the current page */
	unsigned int nwords;	/* Remaining decode buffer length */

	struct rpc_rqst *rqst;	/* For debugging */
};

/*
 * These are the xdr_stream style generic XDR encode and decode functions.
 */
typedef void	(*kxdreproc_t)(struct rpc_rqst *rqstp, struct xdr_stream *xdr,
		const void *obj);
typedef int	(*kxdrdproc_t)(struct rpc_rqst *rqstp, struct xdr_stream *xdr,
		void *obj);

extern void xdr_init_encode(struct xdr_stream *xdr, struct xdr_buf *buf,
			    __be32 *p, struct rpc_rqst *rqst);
extern __be32 *xdr_reserve_space(struct xdr_stream *xdr, size_t nbytes);
extern int xdr_reserve_space_vec(struct xdr_stream *xdr, struct kvec *vec,
		size_t nbytes);
extern void xdr_commit_encode(struct xdr_stream *xdr);
extern void xdr_truncate_encode(struct xdr_stream *xdr, size_t len);
extern int xdr_restrict_buflen(struct xdr_stream *xdr, int newbuflen);
extern void xdr_write_pages(struct xdr_stream *xdr, struct page **pages,
		unsigned int base, unsigned int len);
extern unsigned int xdr_stream_pos(const struct xdr_stream *xdr);
extern unsigned int xdr_page_pos(const struct xdr_stream *xdr);
extern void xdr_init_decode(struct xdr_stream *xdr, struct xdr_buf *buf,
			    __be32 *p, struct rpc_rqst *rqst);
extern void xdr_init_decode_pages(struct xdr_stream *xdr, struct xdr_buf *buf,
		struct page **pages, unsigned int len);
extern __be32 *xdr_inline_decode(struct xdr_stream *xdr, size_t nbytes);
extern unsigned int xdr_read_pages(struct xdr_stream *xdr, unsigned int len);
extern void xdr_enter_page(struct xdr_stream *xdr, unsigned int len);
extern int xdr_process_buf(const struct xdr_buf *buf, unsigned int offset, unsigned int len, int (*actor)(struct scatterlist *, void *), void *data);
extern unsigned int xdr_align_data(struct xdr_stream *, unsigned int offset, unsigned int length);
extern unsigned int xdr_expand_hole(struct xdr_stream *, unsigned int offset, unsigned int length);
extern bool xdr_stream_subsegment(struct xdr_stream *xdr, struct xdr_buf *subbuf,
				  unsigned int len);

/**
 * xdr_set_scratch_buffer - Attach a scratch buffer for decoding data.
 * @xdr: pointer to xdr_stream struct
 * @buf: pointer to an empty buffer
 * @buflen: size of 'buf'
 *
 * The scratch buffer is used when decoding from an array of pages.
 * If an xdr_inline_decode() call spans across page boundaries, then
 * we copy the data into the scratch buffer in order to allow linear
 * access.
 */
static inline void
xdr_set_scratch_buffer(struct xdr_stream *xdr, void *buf, size_t buflen)
{
	xdr->scratch.iov_base = buf;
	xdr->scratch.iov_len = buflen;
}

/**
 * xdr_set_scratch_page - Attach a scratch buffer for decoding data
 * @xdr: pointer to xdr_stream struct
 * @page: an anonymous page
 *
 * See xdr_set_scratch_buffer().
 */
static inline void
xdr_set_scratch_page(struct xdr_stream *xdr, struct page *page)
{
	xdr_set_scratch_buffer(xdr, page_address(page), PAGE_SIZE);
}

/**
 * xdr_reset_scratch_buffer - Clear scratch buffer information
 * @xdr: pointer to xdr_stream struct
 *
 * See xdr_set_scratch_buffer().
 */
static inline void
xdr_reset_scratch_buffer(struct xdr_stream *xdr)
{
	xdr_set_scratch_buffer(xdr, NULL, 0);
}

/**
 * xdr_stream_remaining - Return the number of bytes remaining in the stream
 * @xdr: pointer to struct xdr_stream
 *
 * Return value:
 *   Number of bytes remaining in @xdr before xdr->end
 */
static inline size_t
xdr_stream_remaining(const struct xdr_stream *xdr)
{
	return xdr->nwords << 2;
}

ssize_t xdr_stream_decode_opaque(struct xdr_stream *xdr, void *ptr,
		size_t size);
ssize_t xdr_stream_decode_opaque_dup(struct xdr_stream *xdr, void **ptr,
		size_t maxlen, gfp_t gfp_flags);
ssize_t xdr_stream_decode_string(struct xdr_stream *xdr, char *str,
		size_t size);
ssize_t xdr_stream_decode_string_dup(struct xdr_stream *xdr, char **str,
		size_t maxlen, gfp_t gfp_flags);
/**
 * xdr_align_size - Calculate padded size of an object
 * @n: Size of an object being XDR encoded (in bytes)
 *
 * Return value:
 *   Size (in bytes) of the object including xdr padding
 */
static inline size_t
xdr_align_size(size_t n)
{
	const size_t mask = XDR_UNIT - 1;

	return (n + mask) & ~mask;
}

/**
 * xdr_pad_size - Calculate size of an object's pad
 * @n: Size of an object being XDR encoded (in bytes)
 *
 * This implementation avoids the need for conditional
 * branches or modulo division.
 *
 * Return value:
 *   Size (in bytes) of the needed XDR pad
 */
static inline size_t xdr_pad_size(size_t n)
{
	return xdr_align_size(n) - n;
}

/**
 * xdr_stream_encode_item_present - Encode a "present" list item
 * @xdr: pointer to xdr_stream
 *
 * Return values:
 *   On success, returns length in bytes of XDR buffer consumed
 *   %-EMSGSIZE on XDR buffer overflow
 */
static inline ssize_t xdr_stream_encode_item_present(struct xdr_stream *xdr)
{
	const size_t len = XDR_UNIT;
	__be32 *p = xdr_reserve_space(xdr, len);

	if (unlikely(!p))
		return -EMSGSIZE;
	*p = xdr_one;
	return len;
}

/**
 * xdr_stream_encode_item_absent - Encode a "not present" list item
 * @xdr: pointer to xdr_stream
 *
 * Return values:
 *   On success, returns length in bytes of XDR buffer consumed
 *   %-EMSGSIZE on XDR buffer overflow
 */
static inline int xdr_stream_encode_item_absent(struct xdr_stream *xdr)
{
	const size_t len = XDR_UNIT;
	__be32 *p = xdr_reserve_space(xdr, len);

	if (unlikely(!p))
		return -EMSGSIZE;
	*p = xdr_zero;
	return len;
}

/**
 * xdr_encode_bool - Encode a boolean item
 * @p: address in a buffer into which to encode
 * @n: boolean value to encode
 *
 * Return value:
 *   Address of item following the encoded boolean
 */
static inline __be32 *xdr_encode_bool(__be32 *p, u32 n)
{
	*p = n ? xdr_one : xdr_zero;
	return p++;
}

/**
 * xdr_stream_encode_bool - Encode a boolean item
 * @xdr: pointer to xdr_stream
 * @n: boolean value to encode
 *
 * Return values:
 *   On success, returns length in bytes of XDR buffer consumed
 *   %-EMSGSIZE on XDR buffer overflow
 */
static inline int xdr_stream_encode_bool(struct xdr_stream *xdr, __u32 n)
{
	const size_t len = XDR_UNIT;
	__be32 *p = xdr_reserve_space(xdr, len);

	if (unlikely(!p))
		return -EMSGSIZE;
	xdr_encode_bool(p, n);
	return len;
}

/**
 * xdr_stream_encode_u32 - Encode a 32-bit integer
 * @xdr: pointer to xdr_stream
 * @n: integer to encode
 *
 * Return values:
 *   On success, returns length in bytes of XDR buffer consumed
 *   %-EMSGSIZE on XDR buffer overflow
 */
static inline ssize_t
xdr_stream_encode_u32(struct xdr_stream *xdr, __u32 n)
{
	const size_t len = sizeof(n);
	__be32 *p = xdr_reserve_space(xdr, len);

	if (unlikely(!p))
		return -EMSGSIZE;
	*p = cpu_to_be32(n);
	return len;
}

/**
 * xdr_stream_encode_u64 - Encode a 64-bit integer
 * @xdr: pointer to xdr_stream
 * @n: 64-bit integer to encode
 *
 * Return values:
 *   On success, returns length in bytes of XDR buffer consumed
 *   %-EMSGSIZE on XDR buffer overflow
 */
static inline ssize_t
xdr_stream_encode_u64(struct xdr_stream *xdr, __u64 n)
{
	const size_t len = sizeof(n);
	__be32 *p = xdr_reserve_space(xdr, len);

	if (unlikely(!p))
		return -EMSGSIZE;
	xdr_encode_hyper(p, n);
	return len;
}

/**
 * xdr_stream_encode_opaque_inline - Encode opaque xdr data
 * @xdr: pointer to xdr_stream
 * @ptr: pointer to void pointer
 * @len: size of object
 *
 * Return values:
 *   On success, returns length in bytes of XDR buffer consumed
 *   %-EMSGSIZE on XDR buffer overflow
 */
static inline ssize_t
xdr_stream_encode_opaque_inline(struct xdr_stream *xdr, void **ptr, size_t len)
{
	size_t count = sizeof(__u32) + xdr_align_size(len);
	__be32 *p = xdr_reserve_space(xdr, count);

	if (unlikely(!p)) {
		*ptr = NULL;
		return -EMSGSIZE;
	}
	xdr_encode_opaque(p, NULL, len);
	*ptr = ++p;
	return count;
}

/**
 * xdr_stream_encode_opaque_fixed - Encode fixed length opaque xdr data
 * @xdr: pointer to xdr_stream
 * @ptr: pointer to opaque data object
 * @len: size of object pointed to by @ptr
 *
 * Return values:
 *   On success, returns length in bytes of XDR buffer consumed
 *   %-EMSGSIZE on XDR buffer overflow
 */
static inline ssize_t
xdr_stream_encode_opaque_fixed(struct xdr_stream *xdr, const void *ptr, size_t len)
{
	__be32 *p = xdr_reserve_space(xdr, len);

	if (unlikely(!p))
		return -EMSGSIZE;
	xdr_encode_opaque_fixed(p, ptr, len);
	return xdr_align_size(len);
}

/**
 * xdr_stream_encode_opaque - Encode variable length opaque xdr data
 * @xdr: pointer to xdr_stream
 * @ptr: pointer to opaque data object
 * @len: size of object pointed to by @ptr
 *
 * Return values:
 *   On success, returns length in bytes of XDR buffer consumed
 *   %-EMSGSIZE on XDR buffer overflow
 */
static inline ssize_t
xdr_stream_encode_opaque(struct xdr_stream *xdr, const void *ptr, size_t len)
{
	size_t count = sizeof(__u32) + xdr_align_size(len);
	__be32 *p = xdr_reserve_space(xdr, count);

	if (unlikely(!p))
		return -EMSGSIZE;
	xdr_encode_opaque(p, ptr, len);
	return count;
}

/**
 * xdr_stream_encode_uint32_array - Encode variable length array of integers
 * @xdr: pointer to xdr_stream
 * @array: array of integers
 * @array_size: number of elements in @array
 *
 * Return values:
 *   On success, returns length in bytes of XDR buffer consumed
 *   %-EMSGSIZE on XDR buffer overflow
 */
static inline ssize_t
xdr_stream_encode_uint32_array(struct xdr_stream *xdr,
		const __u32 *array, size_t array_size)
{
	ssize_t ret = (array_size+1) * sizeof(__u32);
	__be32 *p = xdr_reserve_space(xdr, ret);

	if (unlikely(!p))
		return -EMSGSIZE;
	*p++ = cpu_to_be32(array_size);
	for (; array_size > 0; p++, array++, array_size--)
		*p = cpu_to_be32p(array);
	return ret;
}

/**
 * xdr_item_is_absent - symbolically handle XDR discriminators
 * @p: pointer to undecoded discriminator
 *
 * Return values:
 *   %true if the following XDR item is absent
 *   %false if the following XDR item is present
 */
static inline bool xdr_item_is_absent(const __be32 *p)
{
	return *p == xdr_zero;
}

/**
 * xdr_item_is_present - symbolically handle XDR discriminators
 * @p: pointer to undecoded discriminator
 *
 * Return values:
 *   %true if the following XDR item is present
 *   %false if the following XDR item is absent
 */
static inline bool xdr_item_is_present(const __be32 *p)
{
	return *p != xdr_zero;
}

/**
 * xdr_stream_decode_bool - Decode a boolean
 * @xdr: pointer to xdr_stream
 * @ptr: pointer to a u32 in which to store the result
 *
 * Return values:
 *   %0 on success
 *   %-EBADMSG on XDR buffer overflow
 */
static inline ssize_t
xdr_stream_decode_bool(struct xdr_stream *xdr, __u32 *ptr)
{
	const size_t count = sizeof(*ptr);
	__be32 *p = xdr_inline_decode(xdr, count);

	if (unlikely(!p))
		return -EBADMSG;
	*ptr = (*p != xdr_zero);
	return 0;
}

/**
 * xdr_stream_decode_u32 - Decode a 32-bit integer
 * @xdr: pointer to xdr_stream
 * @ptr: location to store integer
 *
 * Return values:
 *   %0 on success
 *   %-EBADMSG on XDR buffer overflow
 */
static inline ssize_t
xdr_stream_decode_u32(struct xdr_stream *xdr, __u32 *ptr)
{
	const size_t count = sizeof(*ptr);
	__be32 *p = xdr_inline_decode(xdr, count);

	if (unlikely(!p))
		return -EBADMSG;
	*ptr = be32_to_cpup(p);
	return 0;
}

/**
 * xdr_stream_decode_u64 - Decode a 64-bit integer
 * @xdr: pointer to xdr_stream
 * @ptr: location to store 64-bit integer
 *
 * Return values:
 *   %0 on success
 *   %-EBADMSG on XDR buffer overflow
 */
static inline ssize_t
xdr_stream_decode_u64(struct xdr_stream *xdr, __u64 *ptr)
{
	const size_t count = sizeof(*ptr);
	__be32 *p = xdr_inline_decode(xdr, count);

	if (unlikely(!p))
		return -EBADMSG;
	xdr_decode_hyper(p, ptr);
	return 0;
}

/**
 * xdr_stream_decode_opaque_fixed - Decode fixed length opaque xdr data
 * @xdr: pointer to xdr_stream
 * @ptr: location to store data
 * @len: size of buffer pointed to by @ptr
 *
 * Return values:
 *   On success, returns size of object stored in @ptr
 *   %-EBADMSG on XDR buffer overflow
 */
static inline ssize_t
xdr_stream_decode_opaque_fixed(struct xdr_stream *xdr, void *ptr, size_t len)
{
	__be32 *p = xdr_inline_decode(xdr, len);

	if (unlikely(!p))
		return -EBADMSG;
	xdr_decode_opaque_fixed(p, ptr, len);
	return len;
}

/**
 * xdr_stream_decode_opaque_inline - Decode variable length opaque xdr data
 * @xdr: pointer to xdr_stream
 * @ptr: location to store pointer to opaque data
 * @maxlen: maximum acceptable object size
 *
 * Note: the pointer stored in @ptr cannot be assumed valid after the XDR
 * buffer has been destroyed, or even after calling xdr_inline_decode()
 * on @xdr. It is therefore expected that the object it points to should
 * be processed immediately.
 *
 * Return values:
 *   On success, returns size of object stored in *@ptr
 *   %-EBADMSG on XDR buffer overflow
 *   %-EMSGSIZE if the size of the object would exceed @maxlen
 */
static inline ssize_t
xdr_stream_decode_opaque_inline(struct xdr_stream *xdr, void **ptr, size_t maxlen)
{
	__be32 *p;
	__u32 len;

	*ptr = NULL;
	if (unlikely(xdr_stream_decode_u32(xdr, &len) < 0))
		return -EBADMSG;
	if (len != 0) {
		p = xdr_inline_decode(xdr, len);
		if (unlikely(!p))
			return -EBADMSG;
		if (unlikely(len > maxlen))
			return -EMSGSIZE;
		*ptr = p;
	}
	return len;
}

/**
 * xdr_stream_decode_uint32_array - Decode variable length array of integers
 * @xdr: pointer to xdr_stream
 * @array: location to store the integer array or NULL
 * @array_size: number of elements to store
 *
 * Return values:
 *   On success, returns number of elements stored in @array
 *   %-EBADMSG on XDR buffer overflow
 *   %-EMSGSIZE if the size of the array exceeds @array_size
 */
static inline ssize_t
xdr_stream_decode_uint32_array(struct xdr_stream *xdr,
		__u32 *array, size_t array_size)
{
	__be32 *p;
	__u32 len;
	ssize_t retval;

	if (unlikely(xdr_stream_decode_u32(xdr, &len) < 0))
		return -EBADMSG;
	p = xdr_inline_decode(xdr, len * sizeof(*p));
	if (unlikely(!p))
		return -EBADMSG;
	if (array == NULL)
		return len;
	if (len <= array_size) {
		if (len < array_size)
			memset(array+len, 0, (array_size-len)*sizeof(*array));
		array_size = len;
		retval = len;
	} else
		retval = -EMSGSIZE;
	for (; array_size > 0; p++, array++, array_size--)
		*array = be32_to_cpup(p);
	return retval;
}

#endif /* _SUNRPC_XDR_H_ */
