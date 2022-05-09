/*
  Portions of this file are based on zlib.  Subsequent additions by NVIDIA.

  Copyright (c) 2001-2021, NVIDIA CORPORATION. All rights reserved.

  zlib.h -- interface of the 'zlib' general purpose compression library
  version 1.1.3, July 9th, 1998

  Copyright (C) 1995-1998 Jean-loup Gailly and Mark Adler

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

  Jean-loup Gailly        Mark Adler
  jloup@gzip.org          madler@alumni.caltech.edu

  The data format used by the zlib library is described by RFCs (Request for
  Comments) 1950 to 1952 in the files ftp://ds.internic.net/rfc/rfc1950.txt
  (zlib format), rfc1951.txt (deflate format) and rfc1952.txt (gzip format).
*/
#ifndef _INFLATE_H_
#define _INFLATE_H_

#include "nvos.h"

#define NOMEMCPY 1

typedef NvU8   uch;
typedef NvU16  ush;
typedef NvU32  ulg;

#define GZ_SLIDE_WINDOW_SIZE 32768

#define NEXTBYTE()  pGzState->inbuf[pGzState->inptr++]
#define NEEDBITS(n) {while(k<(n)){b|=((ulg)NEXTBYTE())<<k;k+=8;}}
#define DUMPBITS(n) {b>>=(n);k-=(n);}

/* If BMAX needs to be larger than 16, then h and x[] should be ulg. */
#define BMAX 16         /* maximum bit length of any code (16 for explode) */
#define N_MAX 288       /* maximum number of codes in any set */

/* Huffman code lookup table entry--this entry is four bytes for machines
   that have 16-bit pointers (e.g. PC's in the small or medium model).
   Valid extra bits are 0..13.  e == 15 is EOB (end of block), e == 16
   means that v is a literal, 16 < e < 32 means that v is a pointer to
   the next table, which codes e - 16 bits, and lastly e == 99 indicates
   an unused code.  If a code with e == 99 is looked up, this implies an
   error in the data. */
struct huft {
    uch e;                /* number of extra bits or operation */
    uch b;                /* number of bits in this code or subcode */
    union {
        ush n;              /* literal, length base, or distance base */
        struct huft *t;     /* pointer to next level of table */
    } v;
};

/* The inflate algorithm uses a sliding 32K byte window on the uncompressed
   stream to find repeated byte strings.  This is implemented here as a
   circular buffer.  The index is updated simply by incrementing and then
   and'ing with 0x7fff (32K-1). */
/* It is left to other modules to supply the 32K area.  It is assumed
   to be usable as if it were declared "uch slide[32768];" or as just
   "uch *slide;" and then malloc'ed in the latter case.  The definition
   must be in unzip.h, included above. */
/* unsigned pGzState->wp;             current position in slide */
#define WSIZE GZ_SLIDE_WINDOW_SIZE
#define flush_output(w) (pGzState->wp=(w),flush_window(pGzState))
#define Tracecv(A,B)
#define Tracevv(X)

#define GZ_STATE_ITERATOR_OK        0
#define GZ_STATE_ITERATOR_ERROR     1
#define GZ_STATE_ITERATOR_END       2

#define GZ_STATE_HUFT_OK            0
#define GZ_STATE_HUFT_INCOMP        1
#define GZ_STATE_HUFT_ERROR         2

typedef struct {
    unsigned int e;  /* table entry flag/number of extra bits */
    unsigned int n, d;        /* length and index for copy */
    unsigned int w;           /* current window position */
    struct huft *t;       /* pointer to table entry */
    ulg b;       /* bit buffer */
    unsigned int k;  /* number of bits in bit buffer */
    int continue_copy;                  /* last flush not finished*/
    unsigned int sn; /* used by inflated type 0 (stored) block */
} GZ_INFLATE_CODES_STATE, *PGZ_INFLATE_CODES_STATE;

typedef struct {
    struct huft *tl;      /* literal/length code table */
    struct huft *td;      /* distance code table */
    NvS32 bl;             /* lookup bits for tl */
    NvS32 bd;             /* lookup bits for td */

    NvU8 *inbuf,*outbuf;
    NvU32 outBufSize;
    NvU32 inptr,outptr;
    NvU32 outLower,outUpper;
    unsigned int wp;
    unsigned int wp1;     /* wp1 is index of first unflushed byte in slide window */
    unsigned int wp2;     /* wp2 is index of last unflushed byte in slide window  */
    uch *window;

    ulg bb;                         /* bit buffer */
    unsigned int bk;                /* bits in bit buffer */
    int e;                          /* last block flag */

    int newblock;                   /* start a new decompression block */
    NvU32 optSize;
    GZ_INFLATE_CODES_STATE codesState;

} GZ_INFLATE_STATE, *PGZ_INFLATE_STATE;

NV_STATUS utilGzIterator(PGZ_INFLATE_STATE pGzState);
NV_STATUS utilGzAllocate(const NvU8 *zArray, NvU32 numTotalBytes, PGZ_INFLATE_STATE *ppGzState);
NvU32 utilGzGetData(PGZ_INFLATE_STATE pGzState, NvU32 offset, NvU32 size, NvU8 * outBuffer);
NV_STATUS utilGzDestroy(PGZ_INFLATE_STATE pGzState);

#endif
