/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * cifs_unicode:  Unicode kernel case support
 *
 * Function:
 *     Convert a unicode character to upper or lower case using
 *     compressed tables.
 *
 *   Copyright (c) International Business Machines  Corp., 2000,2009
 *
 * Notes:
 *     These APIs are based on the C library functions.  The semantics
 *     should match the C functions but with expanded size operands.
 *
 *     The upper/lower functions are based on a table created by mkupr.
 *     This is a compressed table of upper and lower case conversion.
 */
#ifndef _CIFS_UNICODE_H
#define _CIFS_UNICODE_H

#include <asm/byteorder.h>
#include <linux/types.h>
#include <linux/nls.h>

#define  UNIUPR_NOLOWER		/* Example to not expand lower case tables */

/*
 * Windows maps these to the user defined 16 bit Unicode range since they are
 * reserved symbols (along with \ and /), otherwise illegal to store
 * in filenames in NTFS
 */
#define UNI_ASTERISK    (__u16) ('*' + 0xF000)
#define UNI_QUESTION    (__u16) ('?' + 0xF000)
#define UNI_COLON       (__u16) (':' + 0xF000)
#define UNI_GRTRTHAN    (__u16) ('>' + 0xF000)
#define UNI_LESSTHAN    (__u16) ('<' + 0xF000)
#define UNI_PIPE        (__u16) ('|' + 0xF000)
#define UNI_SLASH       (__u16) ('\\' + 0xF000)

/*
 * Macs use an older "SFM" mapping of the symbols above. Fortunately it does
 * not conflict (although almost does) with the mapping above.
 */

#define SFM_DOUBLEQUOTE ((__u16) 0xF020)
#define SFM_ASTERISK    ((__u16) 0xF021)
#define SFM_QUESTION    ((__u16) 0xF025)
#define SFM_COLON       ((__u16) 0xF022)
#define SFM_GRTRTHAN    ((__u16) 0xF024)
#define SFM_LESSTHAN    ((__u16) 0xF023)
#define SFM_PIPE        ((__u16) 0xF027)
#define SFM_SLASH       ((__u16) 0xF026)
#define SFM_SPACE	((__u16) 0xF028)
#define SFM_PERIOD	((__u16) 0xF029)

/*
 * Mapping mechanism to use when one of the seven reserved characters is
 * encountered.  We can only map using one of the mechanisms at a time
 * since otherwise readdir could return directory entries which we would
 * not be able to open
 *
 * NO_MAP_UNI_RSVD  = do not perform any remapping of the character
 * SFM_MAP_UNI_RSVD = map reserved characters using SFM scheme (MAC compatible)
 * SFU_MAP_UNI_RSVD = map reserved characters ala SFU ("mapchars" option)
 *
 */
#define NO_MAP_UNI_RSVD		0
#define SFM_MAP_UNI_RSVD	1
#define SFU_MAP_UNI_RSVD	2

/* Just define what we want from uniupr.h.  We don't want to define the tables
 * in each source file.
 */
#ifndef	UNICASERANGE_DEFINED
struct UniCaseRange {
	wchar_t start;
	wchar_t end;
	signed char *table;
};
#endif				/* UNICASERANGE_DEFINED */

#ifndef UNIUPR_NOUPPER
extern signed char CifsUniUpperTable[512];
extern const struct UniCaseRange CifsUniUpperRange[];
#endif				/* UNIUPR_NOUPPER */

#ifndef UNIUPR_NOLOWER
extern signed char CifsUniLowerTable[512];
extern const struct UniCaseRange CifsUniLowerRange[];
#endif				/* UNIUPR_NOLOWER */

#ifdef __KERNEL__
int cifs_from_utf16(char *to, const __le16 *from, int tolen, int fromlen,
		    const struct nls_table *cp, int map_type);
int cifs_utf16_bytes(const __le16 *from, int maxbytes,
		     const struct nls_table *codepage);
int cifs_strtoUTF16(__le16 *, const char *, int, const struct nls_table *);
char *cifs_strndup_from_utf16(const char *src, const int maxlen,
			      const bool is_unicode,
			      const struct nls_table *codepage);
extern int cifsConvertToUTF16(__le16 *target, const char *source, int maxlen,
			      const struct nls_table *cp, int mapChars);
extern int cifs_remap(struct cifs_sb_info *cifs_sb);
extern __le16 *cifs_strndup_to_utf16(const char *src, const int maxlen,
				     int *utf16_len, const struct nls_table *cp,
				     int remap);
#endif

wchar_t cifs_toupper(wchar_t in);

/*
 * UniStrcat:  Concatenate the second string to the first
 *
 * Returns:
 *     Address of the first string
 */
static inline __le16 *
UniStrcat(__le16 *ucs1, const __le16 *ucs2)
{
	__le16 *anchor = ucs1;	/* save a pointer to start of ucs1 */

	while (*ucs1++) ;	/* To end of first string */
	ucs1--;			/* Return to the null */
	while ((*ucs1++ = *ucs2++)) ;	/* copy string 2 over */
	return anchor;
}

/*
 * UniStrchr:  Find a character in a string
 *
 * Returns:
 *     Address of first occurrence of character in string
 *     or NULL if the character is not in the string
 */
static inline wchar_t *
UniStrchr(const wchar_t *ucs, wchar_t uc)
{
	while ((*ucs != uc) && *ucs)
		ucs++;

	if (*ucs == uc)
		return (wchar_t *) ucs;
	return NULL;
}

/*
 * UniStrcmp:  Compare two strings
 *
 * Returns:
 *     < 0:  First string is less than second
 *     = 0:  Strings are equal
 *     > 0:  First string is greater than second
 */
static inline int
UniStrcmp(const wchar_t *ucs1, const wchar_t *ucs2)
{
	while ((*ucs1 == *ucs2) && *ucs1) {
		ucs1++;
		ucs2++;
	}
	return (int) *ucs1 - (int) *ucs2;
}

/*
 * UniStrcpy:  Copy a string
 */
static inline wchar_t *
UniStrcpy(wchar_t *ucs1, const wchar_t *ucs2)
{
	wchar_t *anchor = ucs1;	/* save the start of result string */

	while ((*ucs1++ = *ucs2++)) ;
	return anchor;
}

/*
 * UniStrlen:  Return the length of a string (in 16 bit Unicode chars not bytes)
 */
static inline size_t
UniStrlen(const wchar_t *ucs1)
{
	int i = 0;

	while (*ucs1++)
		i++;
	return i;
}

/*
 * UniStrnlen:  Return the length (in 16 bit Unicode chars not bytes) of a
 *		string (length limited)
 */
static inline size_t
UniStrnlen(const wchar_t *ucs1, int maxlen)
{
	int i = 0;

	while (*ucs1++) {
		i++;
		if (i >= maxlen)
			break;
	}
	return i;
}

/*
 * UniStrncat:  Concatenate length limited string
 */
static inline wchar_t *
UniStrncat(wchar_t *ucs1, const wchar_t *ucs2, size_t n)
{
	wchar_t *anchor = ucs1;	/* save pointer to string 1 */

	while (*ucs1++) ;
	ucs1--;			/* point to null terminator of s1 */
	while (n-- && (*ucs1 = *ucs2)) {	/* copy s2 after s1 */
		ucs1++;
		ucs2++;
	}
	*ucs1 = 0;		/* Null terminate the result */
	return (anchor);
}

/*
 * UniStrncmp:  Compare length limited string
 */
static inline int
UniStrncmp(const wchar_t *ucs1, const wchar_t *ucs2, size_t n)
{
	if (!n)
		return 0;	/* Null strings are equal */
	while ((*ucs1 == *ucs2) && *ucs1 && --n) {
		ucs1++;
		ucs2++;
	}
	return (int) *ucs1 - (int) *ucs2;
}

/*
 * UniStrncmp_le:  Compare length limited string - native to little-endian
 */
static inline int
UniStrncmp_le(const wchar_t *ucs1, const wchar_t *ucs2, size_t n)
{
	if (!n)
		return 0;	/* Null strings are equal */
	while ((*ucs1 == __le16_to_cpu(*ucs2)) && *ucs1 && --n) {
		ucs1++;
		ucs2++;
	}
	return (int) *ucs1 - (int) __le16_to_cpu(*ucs2);
}

/*
 * UniStrncpy:  Copy length limited string with pad
 */
static inline wchar_t *
UniStrncpy(wchar_t *ucs1, const wchar_t *ucs2, size_t n)
{
	wchar_t *anchor = ucs1;

	while (n-- && *ucs2)	/* Copy the strings */
		*ucs1++ = *ucs2++;

	n++;
	while (n--)		/* Pad with nulls */
		*ucs1++ = 0;
	return anchor;
}

/*
 * UniStrncpy_le:  Copy length limited string with pad to little-endian
 */
static inline wchar_t *
UniStrncpy_le(wchar_t *ucs1, const wchar_t *ucs2, size_t n)
{
	wchar_t *anchor = ucs1;

	while (n-- && *ucs2)	/* Copy the strings */
		*ucs1++ = __le16_to_cpu(*ucs2++);

	n++;
	while (n--)		/* Pad with nulls */
		*ucs1++ = 0;
	return anchor;
}

/*
 * UniStrstr:  Find a string in a string
 *
 * Returns:
 *     Address of first match found
 *     NULL if no matching string is found
 */
static inline wchar_t *
UniStrstr(const wchar_t *ucs1, const wchar_t *ucs2)
{
	const wchar_t *anchor1 = ucs1;
	const wchar_t *anchor2 = ucs2;

	while (*ucs1) {
		if (*ucs1 == *ucs2) {
			/* Partial match found */
			ucs1++;
			ucs2++;
		} else {
			if (!*ucs2)	/* Match found */
				return (wchar_t *) anchor1;
			ucs1 = ++anchor1;	/* No match */
			ucs2 = anchor2;
		}
	}

	if (!*ucs2)		/* Both end together */
		return (wchar_t *) anchor1;	/* Match found */
	return NULL;		/* No match */
}

#ifndef UNIUPR_NOUPPER
/*
 * UniToupper:  Convert a unicode character to upper case
 */
static inline wchar_t
UniToupper(register wchar_t uc)
{
	register const struct UniCaseRange *rp;

	if (uc < sizeof(CifsUniUpperTable)) {
		/* Latin characters */
		return uc + CifsUniUpperTable[uc];	/* Use base tables */
	} else {
		rp = CifsUniUpperRange;	/* Use range tables */
		while (rp->start) {
			if (uc < rp->start)	/* Before start of range */
				return uc;	/* Uppercase = input */
			if (uc <= rp->end)	/* In range */
				return uc + rp->table[uc - rp->start];
			rp++;	/* Try next range */
		}
	}
	return uc;		/* Past last range */
}

/*
 * UniStrupr:  Upper case a unicode string
 */
static inline __le16 *
UniStrupr(register __le16 *upin)
{
	register __le16 *up;

	up = upin;
	while (*up) {		/* For all characters */
		*up = cpu_to_le16(UniToupper(le16_to_cpu(*up)));
		up++;
	}
	return upin;		/* Return input pointer */
}
#endif				/* UNIUPR_NOUPPER */

#ifndef UNIUPR_NOLOWER
/*
 * UniTolower:  Convert a unicode character to lower case
 */
static inline wchar_t
UniTolower(register wchar_t uc)
{
	register const struct UniCaseRange *rp;

	if (uc < sizeof(CifsUniLowerTable)) {
		/* Latin characters */
		return uc + CifsUniLowerTable[uc];	/* Use base tables */
	} else {
		rp = CifsUniLowerRange;	/* Use range tables */
		while (rp->start) {
			if (uc < rp->start)	/* Before start of range */
				return uc;	/* Uppercase = input */
			if (uc <= rp->end)	/* In range */
				return uc + rp->table[uc - rp->start];
			rp++;	/* Try next range */
		}
	}
	return uc;		/* Past last range */
}

/*
 * UniStrlwr:  Lower case a unicode string
 */
static inline wchar_t *
UniStrlwr(register wchar_t *upin)
{
	register wchar_t *up;

	up = upin;
	while (*up) {		/* For all characters */
		*up = UniTolower(*up);
		up++;
	}
	return upin;		/* Return input pointer */
}

#endif

#endif /* _CIFS_UNICODE_H */
