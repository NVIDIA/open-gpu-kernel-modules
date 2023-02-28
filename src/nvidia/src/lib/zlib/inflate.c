/* inflate.c -- Not copyrighted 1992 by Mark Adler
   version c10p1, 10 January 1993 */

/* You can do whatever you like with this source file, though I would
   prefer that if you modify it and redistribute it that you include
   comments to that effect with your name and the date.  Thank you.
   [The history has been moved to the file ChangeLog.]
 */

/*
   Inflate deflated (PKZIP's method 8 compressed) data.  The compression
   method searches for as much of the current string of bytes (up to a
   length of 258) in the previous 32K bytes.  If it doesn't find any
   matches (of at least length 3), it codes the next byte.  Otherwise, it
   codes the length of the matched string and its distance backwards from
   the current position.  There is a single Huffman code that codes both
   single bytes (called "literals") and match lengths.  A second Huffman
   code codes the distance information, which follows a length code.  Each
   length or distance code actually represents a base value and a number
   of "extra" (sometimes zero) bits to get to add to the base value.  At
   the end of each deflated block is a special end-of-block (EOB) literal/
   length code.  The decoding process is basically: get a literal/length
   code; if EOB then done; if a literal, emit the decoded byte; if a
   length then get the distance and emit the referred-to bytes from the
   sliding window of previously emitted data.

   There are (currently) three kinds of inflate blocks: stored, fixed, and
   dynamic.  The compressor deals with some chunk of data at a time, and
   decides which method to use on a chunk-by-chunk basis.  A chunk might
   typically be 32K or 64K.  If the chunk is uncompressible, then the
   "stored" method is used.  In this case, the bytes are simply stored as
   is, eight bits per byte, with none of the above coding.  The bytes are
   preceded by a count, since there is no longer an EOB code.

   If the data is compressible, then either the fixed or dynamic methods
   are used.  In the dynamic method, the compressed data is preceded by
   an encoding of the literal/length and distance Huffman codes that are
   to be used to decode this block.  The representation is itself Huffman
   coded, and so is preceded by a description of that code.  These code
   descriptions take up a little space, and so for small blocks, there is
   a predefined set of codes, called the fixed codes.  The fixed method is
   used if the block codes up smaller that way (usually for quite small
   chunks), otherwise the dynamic method is used.  In the latter case, the
   codes are customized to the probabilities in the current block, and so
   can code it much better than the pre-determined fixed codes.

   The Huffman codes themselves are decoded using a mutli-level table
   lookup, in order to maximize the speed of decoding plus the speed of
   building the decoding tables.  See the comments below that precede the
   lbits and dbits tuning parameters.
 */


/*
   Notes beyond the 1.93a appnote.txt:

   1. Distance pointers never point before the beginning of the output
      stream.
   2. Distance pointers can point back across blocks, up to 32k away.
   3. There is an implied maximum of 7 bits for the bit length table and
      15 bits for the actual data.
   4. If only one code exists, then it is encoded using one bit.  (Zero
      would be more efficient, but perhaps a little confusing.)  If two
      codes exist, they are coded using one bit each (0 and 1).
   5. There is no way of sending zero distance codes--a dummy must be
      sent if there are none.  (History: a pre 2.0 version of PKZIP would
      store blocks with no distance codes, but this was discovered to be
      too harsh a criterion.)  Valid only for 1.93a.  2.04c does allow
      zero distance codes, which is sent as one code of zero bits in
      length.
   6. There are up to 286 literal/length codes.  Code 256 represents the
      end-of-block.  Note however that the static length tree defines
      288 codes just to fill out the Huffman codes.  Codes 286 and 287
      cannot be used though, since there is no length base or extra bits
      defined for them.  Similarly, there are up to 30 distance codes.
      However, static trees define 32 codes (all 5 bits) to fill out the
      Huffman codes, but the last two had better not show up in the data.
   7. Unzip can check dynamic Huffman blocks for complete code sets.
      The exception is that a single code would not be complete (see #4).
   8. The five bits following the block type is really the number of
      literal codes sent minus 257.
   9. Length codes 8,16,16 are interpreted as 13 length codes of 8 bits
      (1+6+6).  Therefore, to output three times the length, you output
      three codes (1+1+1), whereas to output four times the same length,
      you only need two codes (1+3).  Hmm.
  10. In the tree reconstruction algorithm, Code = Code + Increment
      only if BitLength(i) is not zero.  (Pretty obvious.)
  11. Correction: 4 Bits: # of Bit Length codes - 4     (4 - 19)
  12. Note: length code 284 can represent 227-258, but length code 285
      really is 258.  The last length deserves its own, short code
      since it gets used a lot in very redundant files.  The length
      258 is special since 258 - 3 (the min match length) is 255.
  13. The literal/length and distance code bit lengths are read as a
      single stream of lengths.  It is possible (and advantageous) for
      a repeat code (16, 17, or 18) to go across the boundary between
      the two sets of lengths.
 */

//-----------------------------------------------------------------------------
// NVIDIA modifications are solely around interface cleanup, compiler warnings, etc.
//-----------------------------------------------------------------------------

#include "nvtypes.h"
#include "nvstatus.h"

#ifndef NVGZ_USER
#define __DRIVER_BUILD__
// driver build
#include "os/os.h"
#endif /* NVGZ_USER */

#ifndef __DRIVER_BUILD__
// user build : NVGZ_USER
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define portMemCopy(p1, s1, p2, s2) memcpy(p1, p2, ((s1) > (s2)) ? (s2) : (s1))
#define portMemSet  memset
#define portMemAllocNonPaged malloc
#define portMemFree  free
#define sizeof sizeof
#define NV_PRINTF(a,b) printf(b)
#endif

#include "lib/zlib/inflate.h"

/* Function prototypes */
static NvU32 huft_build(NvU8 *, NvU16, NvU32 , ush *, ush *,
                        struct huft **, NvS32 *);
static NvU32 huft_free(struct huft *);
static NvU32 inflate_codes_iterator(PGZ_INFLATE_STATE);
static NvU32 fixed_huft_build(PGZ_INFLATE_STATE);
static NvU32 dynamic_huft_build(PGZ_INFLATE_STATE);

static void flush_window(PGZ_INFLATE_STATE pGzState)
{
    if ( pGzState->wp == 0) return;

    pGzState->wp2 = pGzState->wp;

    // If output range is not specified, do normal output
    if (pGzState->outLower == 0xFFFFFFFF && pGzState->outUpper == 0xFFFFFFFF)
    {
        portMemCopy(pGzState->outbuf + pGzState->outptr, pGzState->wp, pGzState->window, pGzState->wp);
        pGzState->wp1 += pGzState->wp;
        pGzState->optSize += pGzState->wp;
    }
    //        slide     pGzState->outLower        pGzState->outUpper   slide
    // ----============-----|--------------|-----============
    else if (pGzState->outptr + pGzState->wp - 1 < pGzState->outLower
             || pGzState->outptr > pGzState->outUpper)
    {
    }
    //        slide     pGzState->outLower        pGzState->outUpper
    // ----=================|===-----------|-----------------
    else if (pGzState->outptr <= pGzState->outLower
             && pGzState->outptr + pGzState->wp - 1 >= pGzState->outLower
             && pGzState->outptr + pGzState->wp - 1 <= pGzState->outUpper)
    {
        portMemCopy(pGzState->outbuf,
                    pGzState->wp - (pGzState->outLower - pGzState->outptr), pGzState->window + pGzState->outLower - pGzState->outptr,
                    pGzState->wp - (pGzState->outLower - pGzState->outptr));
        pGzState->wp1 += pGzState->wp - (pGzState->outLower - pGzState->outptr);
        pGzState->optSize += pGzState->wp - (pGzState->outLower - pGzState->outptr);
    }
    //        slide     pGzState->outLower        pGzState->outUpper
    // ----=================|==============|===--------------
    else if (pGzState->outptr <= pGzState->outLower
             && pGzState->outptr + pGzState->wp - 1 > pGzState->outUpper )
    {
        portMemCopy(pGzState->outbuf,
                    pGzState->outUpper - pGzState->outLower + 1, pGzState->window + pGzState->outLower - pGzState->outptr,
                    pGzState->outUpper - pGzState->outLower + 1);
        pGzState->wp1 += pGzState->outUpper - pGzState->outptr + 1;
        pGzState->optSize += pGzState->outUpper - pGzState->outLower + 1;
    }
    //        slide     pGzState->outLower        pGzState->outUpper
    // ---------------------|===========---|-----------------
    else if (pGzState->outptr >= pGzState->outLower
             && pGzState->outptr + pGzState->wp - 1 <= pGzState->outUpper)
    {
        portMemCopy(pGzState->outbuf + pGzState->outptr - pGzState->outLower,
                    pGzState->wp, pGzState->window,
                    pGzState->wp);
        pGzState->wp1 += pGzState->wp;
        pGzState->optSize += pGzState->wp;
    }
    //        slide     pGzState->outLower        pGzState->outUpper
    // ---------------------|==============|===--------------
    else if (pGzState->outptr >= pGzState->outLower
             && pGzState->outptr + pGzState->wp - 1 > pGzState->outUpper)
    {
        portMemCopy(pGzState->outbuf + pGzState->outptr - pGzState->outLower,
                    pGzState->outUpper - pGzState->outptr + 1, pGzState->window,
                    pGzState->outUpper - pGzState->outptr + 1);
        pGzState->wp1 += pGzState->outUpper - pGzState->outptr + 1;
        pGzState->optSize += pGzState->outUpper - pGzState->outptr + 1;
    }

    pGzState->outptr += pGzState->wp;
    pGzState->wp = 0;
}


/* Tables for deflate from PKZIP's appnote.txt. */
static NvU32 border[] = {    /* Order of the bit length code lengths */
        16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};
static ush cplens[] = {         /* Copy lengths for literal codes 257..285 */
        3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31,
        35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258, 0, 0};
        /* note: see note #13 above about the 258 in this list. */
static ush cplext[] = {         /* Extra bits for literal codes 257..285 */
        0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2,
        3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0, 99, 99}; /* 99==invalid */
static ush cpdist[] = {         /* Copy offsets for distance codes 0..29 */
        1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193,
        257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145,
        8193, 12289, 16385, 24577};
static ush cpdext[] = {         /* Extra bits for distance codes */
        0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6,
        7, 7, 8, 8, 9, 9, 10, 10, 11, 11,
        12, 12, 13, 13};

/* Macros for inflate() bit peeking and grabbing.
   The usage is:

        NEEDBITS(j)
        x = b & mask_bits[j];
        DUMPBITS(j)

   where NEEDBITS makes sure that b has at least j bits in it, and
   DUMPBITS removes the bits from b.  The macros use the variable k
   for the number of bits in b.  Normally, b and k are register
   variables for speed, and are initialized at the beginning of a
   routine that uses these macros from a global bit buffer and count.

   If we assume that EOB will be the longest code, then we will never
   ask for bits with NEEDBITS that are beyond the end of the stream.
   So, NEEDBITS should not read any more bytes than are needed to
   meet the request.  Then no bytes need to be "returned" to the buffer
   at the end of the last block.

   However, this assumption is not true for fixed blocks--the EOB code
   is 7 bits, but the other literal/length codes can be 8 or 9 bits.
   (The EOB code is shorter than other codes because fixed blocks are
   generally short.  So, while a block always has an EOB, many other
   literal/length codes have a significantly lower probability of
   showing up at all.)  However, by making the first table have a
   lookup of seven bits, the EOB code will be found in that first
   lookup, and so will not require that too many bits be pulled from
   the stream.
 */

static ush mask_bits[] = {
    0x0000,
    0x0001, 0x0003, 0x0007, 0x000f, 0x001f, 0x003f, 0x007f, 0x00ff,
    0x01ff, 0x03ff, 0x07ff, 0x0fff, 0x1fff, 0x3fff, 0x7fff, 0xffff
};

/*
   Huffman code decoding is performed using a multi-level table lookup.
   The fastest way to decode is to simply build a lookup table whose
   size is determined by the longest code.  However, the time it takes
   to build this table can also be a factor if the data being decoded
   is not very long.  The most common codes are necessarily the
   shortest codes, so those codes dominate the decoding time, and hence
   the speed.  The idea is you can have a shorter table that decodes the
   shorter, more probable codes, and then point to subsidiary tables for
   the longer codes.  The time it costs to decode the longer codes is
   then traded against the time it takes to make longer tables.

   This results of this trade are in the variables lbits and dbits
   below.  lbits is the number of bits the first level table for literal/
   length codes can decode in one step, and dbits is the same thing for
   the distance codes.  Subsequent tables are also less than or equal to
   those sizes.  These values may be adjusted either when all of the
   codes are shorter than that, in which case the longest code length in
   bits is used, or when the shortest code is *longer* than the requested
   table size, in which case the length of the shortest code in bits is
   used.

   There are two different values for the two tables, since they code a
   different number of possibilities each.  The literal/length table
   codes 286 possible values, or in a flat code, a little over eight
   bits.  The distance table codes 30 possible values, or a little less
   than five bits, flat.  The optimum values for speed end up being
   about one bit more than those, so lbits is 8+1 and dbits is 5+1.
   The optimum values may differ though from machine to machine, and
   possibly even between compilers.  Your mileage may vary.
 */


const NvU32 lbits = 9;          /* bits in base literal/length lookup table */
const NvU32 dbits = 6;          /* bits in base distance lookup table */

static NvU32 hufts;         /* track memory usage */

/*
 * Given a list of code lengths and a maximum table size, make a set of
 * tables to decode that set of codes.  Return zero on success, one if
 * the given code set is incomplete (the tables are still built in
 * case), two if the input is invalid (all zero length codes or an
 * oversubscribed set of lengths), and three if not enough memory.
 */
static NvU32 huft_build
(
    NvU8 *b,                    /* code lengths in bits (all assumed <= BMAX) */
    NvU16 n,                    /* number of codes (assumed <= N_MAX) */
    NvU32 s,                    /* number of simple-valued codes (0..s-1) */
    ush *d,                     /* list of base values for non-simple codes */
    ush *e,                     /* list of extra bits for non-simple codes */
    struct huft **t,            /* result: starting table */
    NvS32 *m                    /* maximum lookup bits, returns actual */
)
{
    NvU32 a;                    /* counter for codes of length k */
    NvU32 c[BMAX+1];            /* bit length count table */
    NvU32 f;                    /* i repeats in table every f entries */
    NvS32 g;                    /* maximum code length */
    NvS32 h;                    /* table level */
    NvU16 i;                    /* counter, current code */
    NvU32 j;                    /* counter */
    NvS32 k;                    /* number of bits in current code */
    NvS32 l;                    /* bits per table (returned in m) */
    NvU8  *p8;                  /* pointer into b[] */
    NvU16 *p16;                 /* pointer into v[] */
    NvU32 *p32;                 /* pointer into c[] */
    struct huft *q;             /* points to current table */
    struct huft r;              /* table entry for structure assignment */
    struct huft *u[BMAX];       /* table stack */
    NvU16 v[N_MAX];             /* values in order of bit length */
    NvS32 w;                    /* bits before this table == (l * h) */
    NvU32 x[BMAX+1];            /* bit offsets, then code stack */
    NvU32 *xp;                  /* pointer into x */
    NvS32 y;                    /* number of dummy codes added */
    NvU32 z;                    /* number of entries in current table */

    /* Generate counts for each bit length */
    portMemSet((void*)c,0,sizeof(c));

    p8 = b;  i = n;
    do {
        Tracecv(*p8, (stderr, (n-i >= ' ' && n-i <= '~' ? "%c %d\n" : "0x%x %d\n"),
            n-i, *p8));
        c[*p8]++;               /* assume all entries <= BMAX */
        p8++;                   /* Can't combine with above line (Solaris bug) */
    } while (--i);
    if (c[0] == n)              /* null input--all zero length codes */
    {
        *t = (struct huft *)NULL;
        *m = 0;
        return GZ_STATE_HUFT_OK;
    }


    /* Find minimum and maximum length, bound *m by those */
    l = *m;
    for (j = 1; j <= BMAX; j++)
        if (c[j])
            break;
    k = j;                      /* minimum code length */
    if ((NvU32)l < j)
        l = j;
    for (i = BMAX; i; i--)
        if (c[i])
            break;
    g = i;                      /* maximum code length */
    if ((NvU32)l > i)
        l = i;
    *m = l;


    /* Adjust last length count to fill out codes, if needed */
    for (y = 1 << j; j < i; j++, y <<= 1)
        if ((y -= c[j]) < 0)
            return GZ_STATE_HUFT_ERROR;           /* bad input: more codes than bits */
    if ((y -= c[i]) < 0)
        return GZ_STATE_HUFT_ERROR;
    c[i] += y;


    /* Generate starting offsets into the value table for each length */
    x[1] = j = 0;
    p32 = c + 1;  xp = x + 2;
    while (--i) {               /* note that i == g from above */
        *xp++ = (j += *p32++);
    }


    /* Make a table of values in order of bit lengths */
    p8 = b;  i = 0;
    do {
        if ((j = *p8++) != 0)
            v[x[j]++] = i;
    } while (++i < n);


    /* Generate the Huffman codes and for each, make the table entries */
    x[0] = i = 0;                 /* first Huffman code is zero */
    p16 = v;                      /* grab values in bit order */
    h = -1;                       /* no tables yet--level -1 */
    w = -l;                       /* bits decoded == (l * h) */
    u[0] = (struct huft *)NULL;   /* just to keep compilers happy */
    q = (struct huft *)NULL;      /* ditto */
    z = 0;                        /* ditto */

    /* go through the bit lengths (k already is bits in shortest code) */
    for (; k <= g; k++)
    {
        a = c[k];
        while (a--)
        {
            /* here i is the Huffman code of length k bits for value *p */
            /* make tables up to required level */
            while (k > w + l)
            {
                h++;
                w += l;                 /* previous table always l bits */

                /* compute minimum size table less than or equal to l bits */
                z = (NvU32)((z = (NvU32)(g - w)) > (NvU32)l ? (NvU32)l : z);  /* upper limit on table size */
                if ((f = 1 << (j = k - w)) > a + 1)     /* try a k-w bit table */
                {                       /* too few codes for k-w bit table */
                    f -= a + 1;           /* deduct codes from patterns left */
                    xp = c + k;
                    while (++j < z)       /* try smaller tables up to z bits */
                    {
                        if ((f <<= 1) <= *++xp)
                            break;            /* enough codes to use up j bits */
                        f -= *xp;           /* else deduct codes from patterns */
                    }
                }
                z = 1 << j;             /* table entries for j-bit table */

                /* allocate and link in new table */

                q = portMemAllocNonPaged((z + 1)*sizeof(struct huft));
                if (q == NULL)
                {
                    return GZ_STATE_HUFT_ERROR;
                }

                if (q == (struct huft *)NULL)
                {
                    if (h)
                        huft_free(u[0]);
                    return GZ_STATE_HUFT_ERROR;             /* not enough memory */
                }
                hufts += z + 1;         /* track memory usage */
                *t = q + 1;             /* link to list for huft_free() */
                *(t = &(q->v.t)) = (struct huft *)NULL;
                u[h] = ++q;             /* table starts after link */

                /* connect to last table, if there is one */
                if (h)
                {
                    x[h] = i;             /* save pattern for backing up */
                    r.b = (uch)l;         /* bits to dump before this table */
                    r.e = (uch)(16 + j);  /* bits in this table */
                    r.v.t = q;            /* pointer to this table */
                    j = i >> (w - l);     /* (get around Turbo C bug) */
                    u[h-1][j] = r;        /* connect to last table */
                }
            }

            /* set up table entry in r */
            r.b = (uch)(k - w);
            if (p16 >= v + n)
                r.e = 99;               /* out of values--invalid code */
            else if (*p16 < s)
            {
                r.e = (uch)(*p16 < 256 ? 16 : 15);    /* 256 is end-of-block code */
                r.v.n = (ush)(*p16);             /* simple code is just the value */
                p16++;                           /* one compiler does not like *p++ */
            }
            else
            {
                r.e = (uch)e[*p16 - s];   /* non-simple--look up in lists */
                r.v.n = d[*p16++ - s];
            }

            /* fill code-like entries with r */
            f = 1 << (k - w);
            for (j = i >> w; j < z; j += f)
                q[j] = r;

            /* backwards increment the k-bit code i */
            for (j = 1 << (k - 1); i & j; j >>= 1)
                i ^= j;
            i ^= j;

            /* backup over finished tables */
            while ((i & ((NvU32)(1 << w) - 1)) != x[h])
            {
                h--;                    /* don't need to update q */
                w -= l;
            }
        }
    }


    /* Return true (1) if we were given an incomplete table */
    return y != 0 && g != 1;
}

/*
 * Free the malloc'ed tables built by huft_build(), which makes a linked
 * list of the tables it made, with the links in a dummy first entry of
 * each table.
 */
static NvU32 huft_free
(
    struct huft *t          /* table to free */
)
{
    struct huft *p, *q;

    /* Go through linked list, freeing from the malloced (t[-1]) address. */
    p = t;
    while (p != (struct huft *)NULL)
    {
        q = (--p)->v.t;
        portMemFree((void*)p);
        p = q;
    }
    return GZ_STATE_HUFT_OK;
}

static NvU32 inflate_codes_iterator_store(PGZ_INFLATE_STATE pGzState)
{
    NvU32 n = pGzState->codesState.sn;      /* number of bytes in block */
    NvU32 w = pGzState->codesState.w;       /* current window position */
    NvU32 k = pGzState->codesState.k;       /* number of bits in bit buffer */
    ulg b   = pGzState->codesState.b;       /* bit buffer */

    /* read and output the compressed data */
    while (n)
    {
        n--;
        NEEDBITS(8)
        pGzState->window[w++] = (uch)b;
        DUMPBITS(8)
        if (w == WSIZE)
        {
            flush_output(w);
            w = 0;
            break;
        }
    }

    /* restore the globals from the locals */
    pGzState->codesState.sn = n;
    pGzState->codesState.w = w;
    pGzState->codesState.b = b;
    pGzState->codesState.k = k;

    if (n != 0)
    {
        return GZ_STATE_ITERATOR_OK;
    }
    else
    {
        return GZ_STATE_ITERATOR_END;
    }
}

/* inflate (decompress) the codes in a deflated (compressed) block.
Return an error code or zero if it all goes ok. */
static NvU32 inflate_codes_iterator(PGZ_INFLATE_STATE pGzState)
{
    NvU32 e  = pGzState->codesState.e;       /* table entry flag/number of extra bits */
    NvU32 n = pGzState->codesState.n;        /* length and index for copy */
    NvU32 d = pGzState->codesState.d;
    NvU32 w = pGzState->codesState.w;        /* current window position */
    struct huft *t = pGzState->codesState.t;        /* pointer to table entry */
    ulg b = pGzState->codesState.b;                 /* bit buffer */
    NvU32 k = pGzState->codesState.k;        /* number of bits in bit buffer */
    NvU32 ml = mask_bits[pGzState->bl];      /* masks for bl and bd bits */
    NvU32 md = mask_bits[pGzState->bd];
    NvU32 r = 0;

    if (pGzState->codesState.continue_copy == 1)
        goto continue_copy;

    for (;;)
    {
        NEEDBITS((unsigned)pGzState->bl)
        if ((e = (t = pGzState->tl + ((unsigned)b & ml))->e) > 16)
        {
            do {
                if (e == 99)
                    return GZ_STATE_ITERATOR_ERROR;
                DUMPBITS(t->b)
                    e -= 16;
                NEEDBITS(e)
            } while ((e = (t = t->v.t + ((unsigned)b & mask_bits[e]))->e) > 16);
        }
        DUMPBITS(t->b)

        if (e == 16)                /* then it's a literal */
        {
            pGzState->window[w++] = (uch)t->v.n;
            Tracevv((stderr, "%c", pGzState->window[w-1]));
            if (w == WSIZE)
            {
                pGzState->wp1 = 0;
                flush_output(w);
                w = 0;
                r = GZ_STATE_ITERATOR_OK;
                goto exit;
            }
        }
        else                        /* it's an EOB or a length */
        {
            /* exit if end of block */
            if (e == 15)
            {
                r = GZ_STATE_ITERATOR_END;
                goto exit;
            }

            /* get length of block to copy */
            NEEDBITS(e)
            n = t->v.n + ((unsigned)b & mask_bits[e]);
            DUMPBITS(e);

            /* decode distance of block to copy */
            NEEDBITS((unsigned)pGzState->bd)
            if ((e = (t = pGzState->td + ((unsigned)b & md))->e) > 16)
            {
                do {
                    if (e == 99)
                        return 1;
                    DUMPBITS(t->b)
                        e -= 16;
                    NEEDBITS(e)
                } while ((e = (t = t->v.t + ((unsigned)b & mask_bits[e]))->e) > 16);
            }
            DUMPBITS(t->b)

            NEEDBITS(e)
            d = w - t->v.n - ((unsigned)b & mask_bits[e]);
            DUMPBITS(e)

            Tracevv((stderr,"\\[%d,%d]", w-d, n));

            /* do the copy */
            do {
                n -= (e = (e = WSIZE - ((d &= WSIZE-1) > w ? d : w)) > n ? n : e);
#if !defined(NOMEMCPY) && !defined(DEBUG)
                if (w - d >= e)         /* (this test assumes unsigned comparison) */
                {
                    memcpy(pGzState->window + w, pGzState->window + d, e);
                    w += e;
                    d += e;
                }
                else                      /* do it slow to avoid memcpy() overlap */
#endif /* !NOMEMCPY */
                {
                    do {
                        pGzState->window[w++] = pGzState->window[d++];
                        Tracevv((stderr, "%c", pGzState->window[w-1]));
                    } while (--e);
                }
                if (w == WSIZE)
                {
                    pGzState->wp1 = 0;
                    flush_output(w);
                    w = 0;
                    r = GZ_STATE_ITERATOR_OK;
                    pGzState->codesState.continue_copy = 1;
                    goto exit;
                }
continue_copy: ;
            } while (n);

            pGzState->codesState.continue_copy = 0;
        }
    }

exit:

    pGzState->codesState.e = e;  /* table entry flag/number of extra bits */
    pGzState->codesState.n = n;
    pGzState->codesState.d = d;        /* length and index for copy */
    pGzState->codesState.w = w;           /* current window position */
    pGzState->codesState.t = t;       /* pointer to table entry */
    pGzState->codesState.b = b;       /* bit buffer */
    pGzState->codesState.k = k;  /* number of bits in bit buffer */

    /* done */
    return r;
}

static void huft_destroy(PGZ_INFLATE_STATE pGzState)
{
    /* free the decoding tables, return */
    if (pGzState->tl != NULL)
    {
        huft_free(pGzState->tl);
        pGzState->tl = NULL;
    }

    if (pGzState->td != NULL)
    {
        huft_free(pGzState->td);
        pGzState->td = NULL;
    }
}

static NvU32 fixed_huft_build(PGZ_INFLATE_STATE pGzState)
/* decompress an inflated type 1 (fixed Huffman codes) block.  We should
   either replace this with a custom decoder, or at least precompute the
   Huffman tables. */
{
    NvU32 i;                /* temporary variable */
    NvU8 l[N_MAX];      /* length list for huft_build */

    /* set up literal table */
    for (i = 0; i < 144; i++)
        l[i] = 8;
    for (; i < 256; i++)
        l[i] = 9;
    for (; i < 280; i++)
        l[i] = 7;
    for (; i < N_MAX; i++)          /* make a complete, but wrong code set */
        l[i] = 8;
    pGzState->bl = 7;
    if ((i = huft_build(l, N_MAX, 257, cplens, cplext, &pGzState->tl, &pGzState->bl)) != 0)
        return i;


    /* set up distance table */
    for (i = 0; i < 30; i++)      /* make an incomplete code set */
        l[i] = 5;
    pGzState->bd = 5;
    if ((i = huft_build(l, 30, 0, cpdist, cpdext, &pGzState->td, &pGzState->bd)) > GZ_STATE_HUFT_INCOMP)
    {
        huft_free(pGzState->tl);
        return i;
    }

    return GZ_STATE_HUFT_OK;
}

/* decompress an inflated type 2 (dynamic Huffman codes) block. */
static NvU32 dynamic_huft_build(PGZ_INFLATE_STATE pGzState)
{
    NvU32 i;                /* temporary variables */
    NvU32 j;
    NvU32 l;           /* last length */
    NvU32 m;           /* mask for bit lengths table */
    NvU32 n;           /* number of lengths to get */
    NvU32 nb;          /* number of bit length codes */
    NvU16 nl;          /* number of literal/length codes */
    NvU16 nd;          /* number of distance codes */
#ifdef PKZIP_BUG_WORKAROUND
    NvU8 ll[288+32];  /* literal/length and distance code lengths */
#else
    NvU8 ll[286+30];  /* literal/length and distance code lengths */
#endif
    ulg b;       /* bit buffer */
    NvU32 k;  /* number of bits in bit buffer */


    /* make local bit buffer */
    b = pGzState->bb;
    k = pGzState->bk;


    /* read in table lengths */
    NEEDBITS(5)
    nl = 257 + ((NvU8)b & 0x1f);      /* number of literal/length codes */
    DUMPBITS(5)
    NEEDBITS(5)
    nd = 1 + ((NvU8)b & 0x1f);        /* number of distance codes */
    DUMPBITS(5)
    NEEDBITS(4)
    nb = 4 + ((NvU8)b & 0xf);         /* number of bit length codes */
    DUMPBITS(4)
#ifdef PKZIP_BUG_WORKAROUND
    if (nl > 288 || nd > 32)
#else
    if (nl > 286 || nd > 30)
#endif
        return GZ_STATE_HUFT_INCOMP;                   /* bad lengths */

    /* read in bit-length-code lengths */
    for (j = 0; j < nb; j++)
    {
        NEEDBITS(3)
        ll[border[j]] = (NvU8)b & 7;
        DUMPBITS(3)
    }
    for (; j < 19; j++)
        ll[border[j]] = 0;

    /* build decoding table for trees--single level, 7 bit lookup */
    pGzState->bl = 7;
    if ((i = huft_build(ll, 19, 19, NULL, NULL, &pGzState->tl, &pGzState->bl)) != 0)
    {
        if (i == GZ_STATE_HUFT_INCOMP)
            huft_free(pGzState->tl);
        return i;                   /* incomplete code set */
    }

    /* read in literal and distance code lengths */
    n = nl + nd;
    m = mask_bits[pGzState->bl];
    i = l = 0;
    while ((NvU32)i < n)
    {
        NEEDBITS((NvU32)pGzState->bl)
        j = (pGzState->td = pGzState->tl + ((NvU32)b & m))->b;
        DUMPBITS(j)
            j = pGzState->td->v.n;
        if (j < 16)                 /* length of code in bits (0..15) */
            ll[i++] = (NvU8)(l = j);        /* save last length in l */
        else if (j == 16)           /* repeat last length 3 to 6 times */
        {
            NEEDBITS(2)
            j = 3 + ((NvU32)b & 3);
            DUMPBITS(2)
            if ((NvU32)i + j > n)
                return GZ_STATE_HUFT_INCOMP;
            while (j--)
                ll[i++] = (NvU8)l;
        }
        else if (j == 17)           /* 3 to 10 zero length codes */
        {
            NEEDBITS(3)
            j = 3 + ((NvU32)b & 7);
            DUMPBITS(3)
            if ((NvU32)i + j > n)
                return GZ_STATE_HUFT_INCOMP;
            while (j--)
                ll[i++] = 0;
            l = 0;
        }
        else                        /* j == 18: 11 to 138 zero length codes */
        {
            NEEDBITS(7)
            j = 11 + ((NvU32)b & 0x7f);
            DUMPBITS(7)
            if ((NvU32)i + j > n)
                return GZ_STATE_HUFT_INCOMP;
            while (j--)
                ll[i++] = 0;
            l = 0;
        }
    }

    /* free decoding table for trees */
    huft_free(pGzState->tl);

    /* restore the global bit buffer */
    pGzState->bb = b;
    pGzState->bk = k;

    /* build the decoding tables for literal/length and distance codes */
    pGzState->bl = lbits;
    if ((i = huft_build(ll, nl, 257, cplens, cplext, &pGzState->tl, &pGzState->bl)) != 0)
    {
        if (i == GZ_STATE_HUFT_INCOMP) {
            NV_PRINTF(LEVEL_ERROR, "dload, incomplete literal tree\n");
            huft_free(pGzState->tl);
        }
        return i;                   /* incomplete code set */
    }
    pGzState->bd = dbits;
    if ((i = huft_build(ll + nl, nd, 0, cpdist, cpdext, &pGzState->td, &pGzState->bd)) != 0)
    {
        if (i == GZ_STATE_HUFT_INCOMP) {
            NV_PRINTF(LEVEL_ERROR, "dload, incomplete distance tree\n");
#ifdef PKZIP_BUG_WORKAROUND
            i = GZ_STATE_HUFT_OK;
        }
#else
            huft_free(pGzState->td);
        }
        huft_free(pGzState->tl);
        return i;                   /* incomplete code set */
#endif
    }

    return GZ_STATE_HUFT_OK;
}

static
NV_STATUS utilGzInit(const NvU8 *zArray, NvU8* oBuffer, NvU32 numTotalBytes, NvU8* window, PGZ_INFLATE_STATE pGzState)
{
    portMemSet(pGzState, 0, sizeof(GZ_INFLATE_STATE));
    portMemSet(window, 0, GZ_SLIDE_WINDOW_SIZE);

    pGzState->inbuf  = (NvU8*)zArray;
    pGzState->outbuf = oBuffer;
    pGzState->outBufSize = numTotalBytes;
    pGzState->window = window;
    pGzState->newblock = 1;
    pGzState->outLower = 0xFFFFFFFF;
    pGzState->outUpper = 0xFFFFFFFF;

    return NV_OK;
}

/* NVIDIA addition: give pointers to input and known-large-enough output buffers. */
/* decompress an inflated entry                                                   */
NV_STATUS utilGzAllocate(const NvU8 *zArray, NvU32 numTotalBytes, PGZ_INFLATE_STATE *ppGzState)
{
    PGZ_INFLATE_STATE pGzState = NULL;
    NvU8 *window = NULL;
    NV_STATUS status = NV_OK;

    pGzState = portMemAllocNonPaged(sizeof(GZ_INFLATE_STATE));
    if (pGzState == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        goto done;
    }

    window = portMemAllocNonPaged(GZ_SLIDE_WINDOW_SIZE);
    if (window == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        goto done;
    }

    utilGzInit(zArray, 0, numTotalBytes, window, pGzState);

    *ppGzState = pGzState;

done:
    if (status != NV_OK)
    {
        portMemFree(pGzState);
        portMemFree(window);
    }
    return status;

}

NvU32 utilGzIterator(PGZ_INFLATE_STATE pGzState)
{
    NvU32 t;  /* block type */
    NvU32 w;  /* current window position */
    NvU32 b;  /* bit buffer */
    NvU32 k;  /* number of bits in bit buffer */
    NvU32 gzStatus = GZ_STATE_ITERATOR_ERROR;

    // new decompression block, we need to construct huffman tree.
    if (pGzState->newblock == 1)
    {
        /* make local bit buffer */
        b = pGzState->bb;
        k = pGzState->bk;

        /* read in last block bit */
        NEEDBITS(1)
        pGzState->e = (NvU32)b & 1;
        DUMPBITS(1)

        /* read in block type */
        NEEDBITS(2)
        t = (NvU32)b & 3;
        DUMPBITS(2)

        /* restore the global bit buffer */
        pGzState->bb = b;
        pGzState->bk = k;

        /* inflate that block type */
        switch (t)
        {
            case 2:
            {
                gzStatus = dynamic_huft_build(pGzState);
                break;
            }
            case 1:
            {
                gzStatus = fixed_huft_build(pGzState);
                break;
            }
            case 0:
            {
                NvU32 n;
                b = pGzState->bb;
                k = pGzState->bk;
                w = pGzState->wp;

                n = k & 7;
                DUMPBITS(n);

                /* get the length and its complement */
                NEEDBITS(16)
                n = ((unsigned int)b & 0xffff);
                DUMPBITS(16)
                NEEDBITS(16)
                if (n != (unsigned int)((~b) & 0xffff))
                {
                    return GZ_STATE_ITERATOR_ERROR;                 /* error in compressed data */
                }
                DUMPBITS(16)

                pGzState->wp = w;                       /* restore global window pointer */
                pGzState->bb = b;                       /* restore global bit buffer */
                pGzState->bk = k;
                pGzState->codesState.sn = n;
                break;
            }
            default:
            {
                return GZ_STATE_ITERATOR_ERROR;
            }
        }

        if (t != 0 && gzStatus != GZ_STATE_HUFT_OK)
        {
            return GZ_STATE_ITERATOR_ERROR;
        }

        pGzState->newblock = 0;

        /* make local copies of globals */
        pGzState->codesState.b = pGzState->bb;                       /* initialize bit buffer */
        pGzState->codesState.k = pGzState->bk;
        pGzState->codesState.w = pGzState->wp;                       /* initialize window position */
    }

    // decompress one slide window
    if (pGzState->codesState.sn == 0)
    {
        gzStatus = inflate_codes_iterator(pGzState);
    }
    else
    {
        gzStatus = inflate_codes_iterator_store(pGzState);
    }

    // decompression ok and current block finished.
    if (gzStatus == GZ_STATE_ITERATOR_END)
    {
        /* restore the globals from the locals */
        pGzState->wp = pGzState->codesState.w;                       /* restore global window pointer */
        pGzState->bb = pGzState->codesState.b;                       /* restore global bit buffer */
        pGzState->bk = pGzState->codesState.k;
        portMemSet(&pGzState->codesState, 0, sizeof(GZ_INFLATE_CODES_STATE));

        huft_destroy(pGzState);
        pGzState->newblock = 1;

        // current block is the last one, flush remain data in slide window
        if (pGzState->e)
        {
            while (pGzState->bk >= 8)
            {
                pGzState->bk -= 8;
                pGzState->inptr--;
            }

            /* flush out slide */
            flush_output(pGzState->wp);
        }

        // continue iteration
        gzStatus = GZ_STATE_ITERATOR_OK;
    }

    return gzStatus;
}

NV_STATUS utilGzDestroy(PGZ_INFLATE_STATE pGzState)
{
    huft_destroy(pGzState);
    portMemFree(pGzState->window);
    portMemFree(pGzState);
    return NV_OK;
}

NvU32 utilGzGetData(PGZ_INFLATE_STATE pGzState, NvU32 offset, NvU32 size, NvU8 * outBuffer)
{
    NvU32 sizew = 0, oldOutBufSize;
    NvU8 * oldInBuf, *oldOutBuf;
    uch * oldWindow;
    NV_STATUS status = NV_OK;

    if (pGzState == NULL || outBuffer == NULL || offset >= pGzState->outBufSize)
    {
        return 0;
    }

    pGzState->optSize = 0;
    // check requested range [offset, offset + size) with outptr
    if (pGzState->outptr != 0)
    {
        if ( offset >= ((pGzState->outptr + WSIZE - 1) / WSIZE - 1) * WSIZE + pGzState->wp1 )
        {
            // check remaining data in previous slide window
            pGzState->wp1 = offset - (((pGzState->outptr + WSIZE -1 ) / WSIZE - 1) * WSIZE);

            if (pGzState->wp1 < pGzState->wp2)
            {
                sizew = pGzState->wp2 - pGzState->wp1;

                // request can be satisfied from window
                if (sizew >= size)
                {
                    portMemCopy(outBuffer, size, pGzState->window + pGzState->wp1, size);
                    pGzState->wp1 += size;
                    pGzState->optSize += size;
                    return pGzState->optSize;
                }
                // copy data from slide window and continue iteration
                else
                {
                    portMemCopy(outBuffer, sizew,pGzState->window + pGzState->wp1, sizew);
                    outBuffer += sizew;
                    pGzState->optSize += sizew;
                }
            }
        }
        else
        {
            // slide window passed requested range, restart decompression from beginning.
            huft_destroy(pGzState);

            oldInBuf  = pGzState->inbuf;
            oldOutBuf = pGzState->outbuf;
            oldOutBufSize = pGzState->outBufSize;
            oldWindow = pGzState->window;

            utilGzInit(oldInBuf, oldOutBuf, oldOutBufSize, oldWindow, pGzState);
        }
    }

    pGzState->outLower = offset + sizew;
    pGzState->outUpper = offset + size - 1;
    pGzState->outbuf = outBuffer;
    pGzState->wp1 = 0;
    pGzState->wp2 = 0;

    while (pGzState->outptr < offset + size)
    {
         if ((status = utilGzIterator(pGzState)) != GZ_STATE_ITERATOR_OK)
             break;
    }

    if (status == GZ_STATE_ITERATOR_ERROR)
    {
        return 0;
    }

    return pGzState->optSize;
}

