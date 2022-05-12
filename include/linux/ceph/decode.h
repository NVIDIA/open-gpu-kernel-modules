/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __CEPH_DECODE_H
#define __CEPH_DECODE_H

#include <linux/err.h>
#include <linux/bug.h>
#include <linux/slab.h>
#include <linux/time.h>
#include <asm/unaligned.h>

#include <linux/ceph/types.h>

/*
 * in all cases,
 *   void **p     pointer to position pointer
 *   void *end    pointer to end of buffer (last byte + 1)
 */

static inline u64 ceph_decode_64(void **p)
{
	u64 v = get_unaligned_le64(*p);
	*p += sizeof(u64);
	return v;
}
static inline u32 ceph_decode_32(void **p)
{
	u32 v = get_unaligned_le32(*p);
	*p += sizeof(u32);
	return v;
}
static inline u16 ceph_decode_16(void **p)
{
	u16 v = get_unaligned_le16(*p);
	*p += sizeof(u16);
	return v;
}
static inline u8 ceph_decode_8(void **p)
{
	u8 v = *(u8 *)*p;
	(*p)++;
	return v;
}
static inline void ceph_decode_copy(void **p, void *pv, size_t n)
{
	memcpy(pv, *p, n);
	*p += n;
}

/*
 * bounds check input.
 */
static inline bool ceph_has_room(void **p, void *end, size_t n)
{
	return end >= *p && n <= end - *p;
}

#define ceph_decode_need(p, end, n, bad)			\
	do {							\
		if (!likely(ceph_has_room(p, end, n)))		\
			goto bad;				\
	} while (0)

#define ceph_decode_64_safe(p, end, v, bad)			\
	do {							\
		ceph_decode_need(p, end, sizeof(u64), bad);	\
		v = ceph_decode_64(p);				\
	} while (0)
#define ceph_decode_32_safe(p, end, v, bad)			\
	do {							\
		ceph_decode_need(p, end, sizeof(u32), bad);	\
		v = ceph_decode_32(p);				\
	} while (0)
#define ceph_decode_16_safe(p, end, v, bad)			\
	do {							\
		ceph_decode_need(p, end, sizeof(u16), bad);	\
		v = ceph_decode_16(p);				\
	} while (0)
#define ceph_decode_8_safe(p, end, v, bad)			\
	do {							\
		ceph_decode_need(p, end, sizeof(u8), bad);	\
		v = ceph_decode_8(p);				\
	} while (0)

#define ceph_decode_copy_safe(p, end, pv, n, bad)		\
	do {							\
		ceph_decode_need(p, end, n, bad);		\
		ceph_decode_copy(p, pv, n);			\
	} while (0)

/*
 * Allocate a buffer big enough to hold the wire-encoded string, and
 * decode the string into it.  The resulting string will always be
 * terminated with '\0'.  If successful, *p will be advanced
 * past the decoded data.  Also, if lenp is not a null pointer, the
 * length (not including the terminating '\0') will be recorded in
 * *lenp.  Note that a zero-length string is a valid return value.
 *
 * Returns a pointer to the newly-allocated string buffer, or a
 * pointer-coded errno if an error occurs.  Neither *p nor *lenp
 * will have been updated if an error is returned.
 *
 * There are two possible failures:
 *   - converting the string would require accessing memory at or
 *     beyond the "end" pointer provided (-ERANGE)
 *   - memory could not be allocated for the result (-ENOMEM)
 */
static inline char *ceph_extract_encoded_string(void **p, void *end,
						size_t *lenp, gfp_t gfp)
{
	u32 len;
	void *sp = *p;
	char *buf;

	ceph_decode_32_safe(&sp, end, len, bad);
	if (!ceph_has_room(&sp, end, len))
		goto bad;

	buf = kmalloc(len + 1, gfp);
	if (!buf)
		return ERR_PTR(-ENOMEM);

	if (len)
		memcpy(buf, sp, len);
	buf[len] = '\0';

	*p = (char *) *p + sizeof (u32) + len;
	if (lenp)
		*lenp = (size_t) len;

	return buf;

bad:
	return ERR_PTR(-ERANGE);
}

/*
 * skip helpers
 */
#define ceph_decode_skip_n(p, end, n, bad)			\
	do {							\
		ceph_decode_need(p, end, n, bad);		\
                *p += n;					\
	} while (0)

#define ceph_decode_skip_64(p, end, bad)			\
ceph_decode_skip_n(p, end, sizeof(u64), bad)

#define ceph_decode_skip_32(p, end, bad)			\
ceph_decode_skip_n(p, end, sizeof(u32), bad)

#define ceph_decode_skip_16(p, end, bad)			\
ceph_decode_skip_n(p, end, sizeof(u16), bad)

#define ceph_decode_skip_8(p, end, bad)				\
ceph_decode_skip_n(p, end, sizeof(u8), bad)

#define ceph_decode_skip_string(p, end, bad)			\
	do {							\
		u32 len;					\
								\
		ceph_decode_32_safe(p, end, len, bad);		\
		ceph_decode_skip_n(p, end, len, bad);		\
	} while (0)

#define ceph_decode_skip_set(p, end, type, bad)			\
	do {							\
		u32 len;					\
								\
		ceph_decode_32_safe(p, end, len, bad);		\
		while (len--)					\
			ceph_decode_skip_##type(p, end, bad);	\
	} while (0)

#define ceph_decode_skip_map(p, end, ktype, vtype, bad)		\
	do {							\
		u32 len;					\
								\
		ceph_decode_32_safe(p, end, len, bad);		\
		while (len--) {					\
			ceph_decode_skip_##ktype(p, end, bad);	\
			ceph_decode_skip_##vtype(p, end, bad);	\
		}						\
	} while (0)

#define ceph_decode_skip_map_of_map(p, end, ktype1, ktype2, vtype2, bad) \
	do {							\
		u32 len;					\
								\
		ceph_decode_32_safe(p, end, len, bad);		\
		while (len--) {					\
			ceph_decode_skip_##ktype1(p, end, bad);	\
			ceph_decode_skip_map(p, end, ktype2, vtype2, bad); \
		}						\
	} while (0)

/*
 * struct ceph_timespec <-> struct timespec64
 */
static inline void ceph_decode_timespec64(struct timespec64 *ts,
					  const struct ceph_timespec *tv)
{
	/*
	 * This will still overflow in year 2106.  We could extend
	 * the protocol to steal two more bits from tv_nsec to
	 * add three more 136 year epochs after that the way ext4
	 * does if necessary.
	 */
	ts->tv_sec = (time64_t)le32_to_cpu(tv->tv_sec);
	ts->tv_nsec = (long)le32_to_cpu(tv->tv_nsec);
}
static inline void ceph_encode_timespec64(struct ceph_timespec *tv,
					  const struct timespec64 *ts)
{
	tv->tv_sec = cpu_to_le32((u32)ts->tv_sec);
	tv->tv_nsec = cpu_to_le32((u32)ts->tv_nsec);
}

/*
 * sockaddr_storage <-> ceph_sockaddr
 */
#define CEPH_ENTITY_ADDR_TYPE_NONE	0
#define CEPH_ENTITY_ADDR_TYPE_LEGACY	__cpu_to_le32(1)
#define CEPH_ENTITY_ADDR_TYPE_MSGR2	__cpu_to_le32(2)
#define CEPH_ENTITY_ADDR_TYPE_ANY	__cpu_to_le32(3)

static inline void ceph_encode_banner_addr(struct ceph_entity_addr *a)
{
	__be16 ss_family = htons(a->in_addr.ss_family);
	a->in_addr.ss_family = *(__u16 *)&ss_family;

	/* Banner addresses require TYPE_NONE */
	a->type = CEPH_ENTITY_ADDR_TYPE_NONE;
}
static inline void ceph_decode_banner_addr(struct ceph_entity_addr *a)
{
	__be16 ss_family = *(__be16 *)&a->in_addr.ss_family;
	a->in_addr.ss_family = ntohs(ss_family);
	WARN_ON(a->in_addr.ss_family == 512);
	a->type = CEPH_ENTITY_ADDR_TYPE_LEGACY;
}

extern int ceph_decode_entity_addr(void **p, void *end,
				   struct ceph_entity_addr *addr);
int ceph_decode_entity_addrvec(void **p, void *end, bool msgr2,
			       struct ceph_entity_addr *addr);

int ceph_entity_addr_encoding_len(const struct ceph_entity_addr *addr);
void ceph_encode_entity_addr(void **p, const struct ceph_entity_addr *addr);

/*
 * encoders
 */
static inline void ceph_encode_64(void **p, u64 v)
{
	put_unaligned_le64(v, (__le64 *)*p);
	*p += sizeof(u64);
}
static inline void ceph_encode_32(void **p, u32 v)
{
	put_unaligned_le32(v, (__le32 *)*p);
	*p += sizeof(u32);
}
static inline void ceph_encode_16(void **p, u16 v)
{
	put_unaligned_le16(v, (__le16 *)*p);
	*p += sizeof(u16);
}
static inline void ceph_encode_8(void **p, u8 v)
{
	*(u8 *)*p = v;
	(*p)++;
}
static inline void ceph_encode_copy(void **p, const void *s, int len)
{
	memcpy(*p, s, len);
	*p += len;
}

/*
 * filepath, string encoders
 */
static inline void ceph_encode_filepath(void **p, void *end,
					u64 ino, const char *path)
{
	u32 len = path ? strlen(path) : 0;
	BUG_ON(*p + 1 + sizeof(ino) + sizeof(len) + len > end);
	ceph_encode_8(p, 1);
	ceph_encode_64(p, ino);
	ceph_encode_32(p, len);
	if (len)
		memcpy(*p, path, len);
	*p += len;
}

static inline void ceph_encode_string(void **p, void *end,
				      const char *s, u32 len)
{
	BUG_ON(*p + sizeof(len) + len > end);
	ceph_encode_32(p, len);
	if (len)
		memcpy(*p, s, len);
	*p += len;
}

/*
 * version and length starting block encoders/decoders
 */

/* current code version (u8) + compat code version (u8) + len of struct (u32) */
#define CEPH_ENCODING_START_BLK_LEN 6

/**
 * ceph_start_encoding - start encoding block
 * @struct_v: current (code) version of the encoding
 * @struct_compat: oldest code version that can decode it
 * @struct_len: length of struct encoding
 */
static inline void ceph_start_encoding(void **p, u8 struct_v, u8 struct_compat,
				       u32 struct_len)
{
	ceph_encode_8(p, struct_v);
	ceph_encode_8(p, struct_compat);
	ceph_encode_32(p, struct_len);
}

/**
 * ceph_start_decoding - start decoding block
 * @v: current version of the encoding that the code supports
 * @name: name of the struct (free-form)
 * @struct_v: out param for the encoding version
 * @struct_len: out param for the length of struct encoding
 *
 * Validates the length of struct encoding, so unsafe ceph_decode_*
 * variants can be used for decoding.
 */
static inline int ceph_start_decoding(void **p, void *end, u8 v,
				      const char *name, u8 *struct_v,
				      u32 *struct_len)
{
	u8 struct_compat;

	ceph_decode_need(p, end, CEPH_ENCODING_START_BLK_LEN, bad);
	*struct_v = ceph_decode_8(p);
	struct_compat = ceph_decode_8(p);
	if (v < struct_compat) {
		pr_warn("got struct_v %d struct_compat %d > %d of %s\n",
			*struct_v, struct_compat, v, name);
		return -EINVAL;
	}

	*struct_len = ceph_decode_32(p);
	ceph_decode_need(p, end, *struct_len, bad);
	return 0;

bad:
	return -ERANGE;
}

#define ceph_encode_need(p, end, n, bad)			\
	do {							\
		if (!likely(ceph_has_room(p, end, n)))		\
			goto bad;				\
	} while (0)

#define ceph_encode_64_safe(p, end, v, bad)			\
	do {							\
		ceph_encode_need(p, end, sizeof(u64), bad);	\
		ceph_encode_64(p, v);				\
	} while (0)
#define ceph_encode_32_safe(p, end, v, bad)			\
	do {							\
		ceph_encode_need(p, end, sizeof(u32), bad);	\
		ceph_encode_32(p, v);				\
	} while (0)
#define ceph_encode_16_safe(p, end, v, bad)			\
	do {							\
		ceph_encode_need(p, end, sizeof(u16), bad);	\
		ceph_encode_16(p, v);				\
	} while (0)
#define ceph_encode_8_safe(p, end, v, bad)			\
	do {							\
		ceph_encode_need(p, end, sizeof(u8), bad);	\
		ceph_encode_8(p, v);				\
	} while (0)

#define ceph_encode_copy_safe(p, end, pv, n, bad)		\
	do {							\
		ceph_encode_need(p, end, n, bad);		\
		ceph_encode_copy(p, pv, n);			\
	} while (0)
#define ceph_encode_string_safe(p, end, s, n, bad)		\
	do {							\
		ceph_encode_need(p, end, n, bad);		\
		ceph_encode_string(p, end, s, n);		\
	} while (0)


#endif
