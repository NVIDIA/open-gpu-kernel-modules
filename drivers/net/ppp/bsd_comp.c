/*
 * Update: The Berkeley copyright was changed, and the change
 * is retroactive to all "true" BSD software (ie everything
 * from UCB as opposed to other peoples code that just carried
 * the same license). The new copyright doesn't clash with the
 * GPL, so the module-only restriction has been removed..
 */

/* Because this code is derived from the 4.3BSD compress source:
 *
 * Copyright (c) 1985, 1986 The Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * James A. Woods, derived from original work by Spencer Thomas
 * and Joseph Orost.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * This version is for use with contiguous buffers on Linux-derived systems.
 *
 *  ==FILEVERSION 20000226==
 *
 *  NOTE TO MAINTAINERS:
 *     If you modify this file at all, please set the number above to the
 *     date of the modification as YYMMDD (year month day).
 *     bsd_comp.c is shipped with a PPP distribution as well as with
 *     the kernel; if everyone increases the FILEVERSION number above,
 *     then scripts can do the right thing when deciding whether to
 *     install a new bsd_comp.c file. Don't change the format of that
 *     line otherwise, so the installation script can recognize it.
 *
 * From: bsd_comp.c,v 1.3 1994/12/08 01:59:58 paulus Exp
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/string.h>

#include <linux/ppp_defs.h>

#undef   PACKETPTR
#define  PACKETPTR 1
#include <linux/ppp-comp.h>
#undef   PACKETPTR

#include <asm/byteorder.h>

/*
 * PPP "BSD compress" compression
 *  The differences between this compression and the classic BSD LZW
 *  source are obvious from the requirement that the classic code worked
 *  with files while this handles arbitrarily long streams that
 *  are broken into packets.  They are:
 *
 *	When the code size expands, a block of junk is not emitted by
 *	    the compressor and not expected by the decompressor.
 *
 *	New codes are not necessarily assigned every time an old
 *	    code is output by the compressor.  This is because a packet
 *	    end forces a code to be emitted, but does not imply that a
 *	    new sequence has been seen.
 *
 *	The compression ratio is checked at the first end of a packet
 *	    after the appropriate gap.	Besides simplifying and speeding
 *	    things up, this makes it more likely that the transmitter
 *	    and receiver will agree when the dictionary is cleared when
 *	    compression is not going well.
 */

/*
 * Macros to extract protocol version and number of bits
 * from the third byte of the BSD Compress CCP configuration option.
 */

#define BSD_VERSION(x)	((x) >> 5)
#define BSD_NBITS(x)	((x) & 0x1F)

#define BSD_CURRENT_VERSION	1

/*
 * A dictionary for doing BSD compress.
 */

struct bsd_dict {
    union {				/* hash value */
	unsigned long	fcode;
	struct {
#if defined(__LITTLE_ENDIAN)		/* Little endian order */
	    unsigned short	prefix;	/* preceding code */
	    unsigned char	suffix; /* last character of new code */
	    unsigned char	pad;
#elif defined(__BIG_ENDIAN)		/* Big endian order */
	    unsigned char	pad;
	    unsigned char	suffix; /* last character of new code */
	    unsigned short	prefix; /* preceding code */
#else
#error Endianness not defined...
#endif
	} hs;
    } f;
    unsigned short codem1;		/* output of hash table -1 */
    unsigned short cptr;		/* map code to hash table entry */
};

struct bsd_db {
    int	    totlen;			/* length of this structure */
    unsigned int   hsize;		/* size of the hash table */
    unsigned char  hshift;		/* used in hash function */
    unsigned char  n_bits;		/* current bits/code */
    unsigned char  maxbits;		/* maximum bits/code */
    unsigned char  debug;		/* non-zero if debug desired */
    unsigned char  unit;		/* ppp unit number */
    unsigned short seqno;		/* sequence # of next packet */
    unsigned int   mru;			/* size of receive (decompress) bufr */
    unsigned int   maxmaxcode;		/* largest valid code */
    unsigned int   max_ent;		/* largest code in use */
    unsigned int   in_count;		/* uncompressed bytes, aged */
    unsigned int   bytes_out;		/* compressed bytes, aged */
    unsigned int   ratio;		/* recent compression ratio */
    unsigned int   checkpoint;		/* when to next check the ratio */
    unsigned int   clear_count;		/* times dictionary cleared */
    unsigned int   incomp_count;	/* incompressible packets */
    unsigned int   incomp_bytes;	/* incompressible bytes */
    unsigned int   uncomp_count;	/* uncompressed packets */
    unsigned int   uncomp_bytes;	/* uncompressed bytes */
    unsigned int   comp_count;		/* compressed packets */
    unsigned int   comp_bytes;		/* compressed bytes */
    unsigned short  *lens;		/* array of lengths of codes */
    struct bsd_dict *dict;		/* dictionary */
};

#define BSD_OVHD	2		/* BSD compress overhead/packet */
#define MIN_BSD_BITS	9
#define BSD_INIT_BITS	MIN_BSD_BITS
#define MAX_BSD_BITS	15

static void	bsd_free (void *state);
static void	*bsd_alloc(unsigned char *options, int opt_len, int decomp);
static void	*bsd_comp_alloc (unsigned char *options, int opt_len);
static void	*bsd_decomp_alloc (unsigned char *options, int opt_len);

static int	bsd_init        (void *db, unsigned char *options,
			         int opt_len, int unit, int debug, int decomp);
static int	bsd_comp_init   (void *state, unsigned char *options,
			         int opt_len, int unit, int opthdr, int debug);
static int	bsd_decomp_init (void *state, unsigned char *options,
				 int opt_len, int unit, int opthdr, int mru,
				 int debug);

static void	bsd_reset (void *state);
static void	bsd_comp_stats (void *state, struct compstat *stats);

static int	bsd_compress (void *state, unsigned char *rptr,
			      unsigned char *obuf, int isize, int osize);
static void	bsd_incomp (void *state, unsigned char *ibuf, int icnt);

static int	bsd_decompress (void *state, unsigned char *ibuf, int isize,
				unsigned char *obuf, int osize);

/* These are in ppp_generic.c */
extern int  ppp_register_compressor   (struct compressor *cp);
extern void ppp_unregister_compressor (struct compressor *cp);

/*
 * the next two codes should not be changed lightly, as they must not
 * lie within the contiguous general code space.
 */
#define CLEAR	256			/* table clear output code */
#define FIRST	257			/* first free entry */
#define LAST	255

#define MAXCODE(b)	((1 << (b)) - 1)
#define BADCODEM1	MAXCODE(MAX_BSD_BITS)

#define BSD_HASH(prefix,suffix,hshift) ((((unsigned long)(suffix))<<(hshift)) \
					 ^ (unsigned long)(prefix))
#define BSD_KEY(prefix,suffix)		((((unsigned long)(suffix)) << 16) \
					 + (unsigned long)(prefix))

#define CHECK_GAP	10000		/* Ratio check interval */

#define RATIO_SCALE_LOG	8
#define RATIO_SCALE	(1<<RATIO_SCALE_LOG)
#define RATIO_MAX	(0x7fffffff>>RATIO_SCALE_LOG)

/*
 * clear the dictionary
 */

static void
bsd_clear(struct bsd_db *db)
{
    db->clear_count++;
    db->max_ent      = FIRST-1;
    db->n_bits       = BSD_INIT_BITS;
    db->bytes_out    = 0;
    db->in_count     = 0;
    db->ratio	     = 0;
    db->checkpoint   = CHECK_GAP;
}

/*
 * If the dictionary is full, then see if it is time to reset it.
 *
 * Compute the compression ratio using fixed-point arithmetic
 * with 8 fractional bits.
 *
 * Since we have an infinite stream instead of a single file,
 * watch only the local compression ratio.
 *
 * Since both peers must reset the dictionary at the same time even in
 * the absence of CLEAR codes (while packets are incompressible), they
 * must compute the same ratio.
 */

static int bsd_check (struct bsd_db *db)	/* 1=output CLEAR */
  {
    unsigned int new_ratio;

    if (db->in_count >= db->checkpoint)
      {
	/* age the ratio by limiting the size of the counts */
	if (db->in_count >= RATIO_MAX || db->bytes_out >= RATIO_MAX)
	  {
	    db->in_count  -= (db->in_count  >> 2);
	    db->bytes_out -= (db->bytes_out >> 2);
	  }

	db->checkpoint = db->in_count + CHECK_GAP;

	if (db->max_ent >= db->maxmaxcode)
	  {
	    /* Reset the dictionary only if the ratio is worse,
	     * or if it looks as if it has been poisoned
	     * by incompressible data.
	     *
	     * This does not overflow, because
	     *	db->in_count <= RATIO_MAX.
	     */

	    new_ratio = db->in_count << RATIO_SCALE_LOG;
	    if (db->bytes_out != 0)
	      {
		new_ratio /= db->bytes_out;
	      }

	    if (new_ratio < db->ratio || new_ratio < 1 * RATIO_SCALE)
	      {
		bsd_clear (db);
		return 1;
	      }
	    db->ratio = new_ratio;
	  }
      }
    return 0;
  }

/*
 * Return statistics.
 */

static void bsd_comp_stats (void *state, struct compstat *stats)
  {
    struct bsd_db *db = (struct bsd_db *) state;

    stats->unc_bytes    = db->uncomp_bytes;
    stats->unc_packets  = db->uncomp_count;
    stats->comp_bytes   = db->comp_bytes;
    stats->comp_packets = db->comp_count;
    stats->inc_bytes    = db->incomp_bytes;
    stats->inc_packets  = db->incomp_count;
    stats->in_count     = db->in_count;
    stats->bytes_out    = db->bytes_out;
  }

/*
 * Reset state, as on a CCP ResetReq.
 */

static void bsd_reset (void *state)
  {
    struct bsd_db *db = (struct bsd_db *) state;

    bsd_clear(db);

    db->seqno       = 0;
    db->clear_count = 0;
  }

/*
 * Release the compression structure
 */

static void bsd_free (void *state)
{
	struct bsd_db *db = state;

	if (!db)
		return;

/*
 * Release the dictionary
 */
	vfree(db->dict);
	db->dict = NULL;
/*
 * Release the string buffer
 */
	vfree(db->lens);
	db->lens = NULL;
/*
 * Finally release the structure itself.
 */
	kfree(db);
}

/*
 * Allocate space for a (de) compressor.
 */

static void *bsd_alloc (unsigned char *options, int opt_len, int decomp)
  {
    int bits;
    unsigned int hsize, hshift, maxmaxcode;
    struct bsd_db *db;

    if (opt_len != 3 || options[0] != CI_BSD_COMPRESS || options[1] != 3
	|| BSD_VERSION(options[2]) != BSD_CURRENT_VERSION)
      {
	return NULL;
      }

    bits = BSD_NBITS(options[2]);

    switch (bits)
      {
    case 9:			/* needs 82152 for both directions */
    case 10:			/* needs 84144 */
    case 11:			/* needs 88240 */
    case 12:			/* needs 96432 */
	hsize = 5003;
	hshift = 4;
	break;
    case 13:			/* needs 176784 */
	hsize = 9001;
	hshift = 5;
	break;
    case 14:			/* needs 353744 */
	hsize = 18013;
	hshift = 6;
	break;
    case 15:			/* needs 691440 */
	hsize = 35023;
	hshift = 7;
	break;
    case 16:			/* needs 1366160--far too much, */
	/* hsize = 69001; */	/* and 69001 is too big for cptr */
	/* hshift = 8; */	/* in struct bsd_db */
	/* break; */
    default:
	return NULL;
      }
/*
 * Allocate the main control structure for this instance.
 */
    maxmaxcode = MAXCODE(bits);
    db         = kzalloc(sizeof (struct bsd_db),
					    GFP_KERNEL);
    if (!db)
      {
	return NULL;
      }

/*
 * Allocate space for the dictionary. This may be more than one page in
 * length.
 */
    db->dict = vmalloc(array_size(hsize, sizeof(struct bsd_dict)));
    if (!db->dict)
      {
	bsd_free (db);
	return NULL;
      }

/*
 * If this is the compression buffer then there is no length data.
 */
    if (!decomp)
      {
	db->lens = NULL;
      }
/*
 * For decompression, the length information is needed as well.
 */
    else
      {
        db->lens = vmalloc(array_size(sizeof(db->lens[0]), (maxmaxcode + 1)));
	if (!db->lens)
	  {
	    bsd_free (db);
	    return NULL;
	  }
      }
/*
 * Initialize the data information for the compression code
 */
    db->totlen     = sizeof (struct bsd_db)   +
      		    (sizeof (struct bsd_dict) * hsize);

    db->hsize      = hsize;
    db->hshift     = hshift;
    db->maxmaxcode = maxmaxcode;
    db->maxbits    = bits;

    return (void *) db;
  }

static void *bsd_comp_alloc (unsigned char *options, int opt_len)
  {
    return bsd_alloc (options, opt_len, 0);
  }

static void *bsd_decomp_alloc (unsigned char *options, int opt_len)
  {
    return bsd_alloc (options, opt_len, 1);
  }

/*
 * Initialize the database.
 */

static int bsd_init (void *state, unsigned char *options,
		     int opt_len, int unit, int debug, int decomp)
  {
    struct bsd_db *db = state;
    int indx;

    if ((opt_len != 3) || (options[0] != CI_BSD_COMPRESS) || (options[1] != 3)
	|| (BSD_VERSION(options[2]) != BSD_CURRENT_VERSION)
	|| (BSD_NBITS(options[2]) != db->maxbits)
	|| (decomp && db->lens == NULL))
      {
	return 0;
      }

    if (decomp)
      {
	indx = LAST;
	do
	  {
	    db->lens[indx] = 1;
	  }
	while (indx-- > 0);
      }

    indx = db->hsize;
    while (indx-- != 0)
      {
	db->dict[indx].codem1 = BADCODEM1;
	db->dict[indx].cptr   = 0;
      }

    db->unit = unit;
    db->mru  = 0;
#ifndef DEBUG
    if (debug)
#endif
      db->debug = 1;

    bsd_reset(db);

    return 1;
  }

static int bsd_comp_init (void *state, unsigned char *options,
			  int opt_len, int unit, int opthdr, int debug)
  {
    return bsd_init (state, options, opt_len, unit, debug, 0);
  }

static int bsd_decomp_init (void *state, unsigned char *options,
			    int opt_len, int unit, int opthdr, int mru,
			    int debug)
  {
    return bsd_init (state, options, opt_len, unit, debug, 1);
  }

/*
 * Obtain pointers to the various structures in the compression tables
 */

#define dict_ptrx(p,idx) &(p->dict[idx])
#define lens_ptrx(p,idx) &(p->lens[idx])

#ifdef DEBUG
static unsigned short *lens_ptr(struct bsd_db *db, int idx)
  {
    if ((unsigned int) idx > (unsigned int) db->maxmaxcode)
      {
	printk ("<9>ppp: lens_ptr(%d) > max\n", idx);
	idx = 0;
      }
    return lens_ptrx (db, idx);
  }

static struct bsd_dict *dict_ptr(struct bsd_db *db, int idx)
  {
    if ((unsigned int) idx >= (unsigned int) db->hsize)
      {
	printk ("<9>ppp: dict_ptr(%d) > max\n", idx);
	idx = 0;
      }
    return dict_ptrx (db, idx);
  }

#else
#define lens_ptr(db,idx) lens_ptrx(db,idx)
#define dict_ptr(db,idx) dict_ptrx(db,idx)
#endif

/*
 * compress a packet
 *
 *	The result of this function is the size of the compressed
 *	packet. A zero is returned if the packet was not compressed
 *	for some reason, such as the size being larger than uncompressed.
 *
 *	One change from the BSD compress command is that when the
 *	code size expands, we do not output a bunch of padding.
 */

static int bsd_compress (void *state, unsigned char *rptr, unsigned char *obuf,
			 int isize, int osize)
  {
    struct bsd_db *db;
    int hshift;
    unsigned int max_ent;
    unsigned int n_bits;
    unsigned int bitno;
    unsigned long accm;
    int ent;
    unsigned long fcode;
    struct bsd_dict *dictp;
    unsigned char c;
    int hval;
    int disp;
    int ilen;
    int mxcode;
    unsigned char *wptr;
    int olen;

#define PUTBYTE(v)			\
  {					\
    ++olen;				\
    if (wptr)				\
      {					\
	*wptr++ = (unsigned char) (v);	\
	if (olen >= osize)		\
	  {				\
	    wptr = NULL;		\
	  }				\
      }					\
  }

#define OUTPUT(ent)			\
  {					\
    bitno -= n_bits;			\
    accm |= ((ent) << bitno);		\
    do					\
      {					\
	PUTBYTE(accm >> 24);		\
	accm <<= 8;			\
	bitno += 8;			\
      }					\
    while (bitno <= 24);		\
  }

  /*
   * If the protocol is not in the range we're interested in,
   * just return without compressing the packet.  If it is,
   * the protocol becomes the first byte to compress.
   */

    ent = PPP_PROTOCOL(rptr);
    if (ent < 0x21 || ent > 0xf9)
      {
	return 0;
      }

    db      = (struct bsd_db *) state;
    hshift  = db->hshift;
    max_ent = db->max_ent;
    n_bits  = db->n_bits;
    bitno   = 32;
    accm    = 0;
    mxcode  = MAXCODE (n_bits);

    /* Initialize the output pointers */
    wptr  = obuf;
    olen  = PPP_HDRLEN + BSD_OVHD;

    if (osize > isize)
      {
	osize = isize;
      }

    /* This is the PPP header information */
    if (wptr)
      {
	*wptr++ = PPP_ADDRESS(rptr);
	*wptr++ = PPP_CONTROL(rptr);
	*wptr++ = 0;
	*wptr++ = PPP_COMP;
	*wptr++ = db->seqno >> 8;
	*wptr++ = db->seqno;
      }

    /* Skip the input header */
    rptr  += PPP_HDRLEN;
    isize -= PPP_HDRLEN;
    ilen   = ++isize;	/* Low byte of protocol is counted as input */

    while (--ilen > 0)
      {
	c     = *rptr++;
	fcode = BSD_KEY  (ent, c);
	hval  = BSD_HASH (ent, c, hshift);
	dictp = dict_ptr (db, hval);

	/* Validate and then check the entry. */
	if (dictp->codem1 >= max_ent)
	  {
	    goto nomatch;
	  }

	if (dictp->f.fcode == fcode)
	  {
	    ent = dictp->codem1 + 1;
	    continue;	/* found (prefix,suffix) */
	  }

	/* continue probing until a match or invalid entry */
	disp = (hval == 0) ? 1 : hval;

	do
	  {
	    hval += disp;
	    if (hval >= db->hsize)
	      {
		hval -= db->hsize;
	      }
	    dictp = dict_ptr (db, hval);
	    if (dictp->codem1 >= max_ent)
	      {
		goto nomatch;
	      }
	  }
	while (dictp->f.fcode != fcode);

	ent = dictp->codem1 + 1;	/* finally found (prefix,suffix) */
	continue;

nomatch:
	OUTPUT(ent);		/* output the prefix */

	/* code -> hashtable */
	if (max_ent < db->maxmaxcode)
	  {
	    struct bsd_dict *dictp2;
	    struct bsd_dict *dictp3;
	    int    indx;

	    /* expand code size if needed */
	    if (max_ent >= mxcode)
	      {
		db->n_bits = ++n_bits;
		mxcode     = MAXCODE (n_bits);
	      }

	    /* Invalidate old hash table entry using
	     * this code, and then take it over.
	     */

	    dictp2 = dict_ptr (db, max_ent + 1);
	    indx   = dictp2->cptr;
	    dictp3 = dict_ptr (db, indx);

	    if (dictp3->codem1 == max_ent)
	      {
		dictp3->codem1 = BADCODEM1;
	      }

	    dictp2->cptr   = hval;
	    dictp->codem1  = max_ent;
	    dictp->f.fcode = fcode;
	    db->max_ent    = ++max_ent;

	    if (db->lens)
	      {
		unsigned short *len1 = lens_ptr (db, max_ent);
		unsigned short *len2 = lens_ptr (db, ent);
		*len1 = *len2 + 1;
	      }
	  }
	ent = c;
      }

    OUTPUT(ent);		/* output the last code */

    db->bytes_out    += olen - PPP_HDRLEN - BSD_OVHD;
    db->uncomp_bytes += isize;
    db->in_count     += isize;
    ++db->uncomp_count;
    ++db->seqno;

    if (bitno < 32)
      {
	++db->bytes_out; /* must be set before calling bsd_check */
      }

    /*
     * Generate the clear command if needed
     */

    if (bsd_check(db))
      {
	OUTPUT (CLEAR);
      }

    /*
     * Pad dribble bits of last code with ones.
     * Do not emit a completely useless byte of ones.
     */

    if (bitno != 32)
      {
	PUTBYTE((accm | (0xff << (bitno-8))) >> 24);
      }

    /*
     * Increase code size if we would have without the packet
     * boundary because the decompressor will do so.
     */

    if (max_ent >= mxcode && max_ent < db->maxmaxcode)
      {
	db->n_bits++;
      }

    /* If output length is too large then this is an incomplete frame. */
    if (wptr == NULL)
      {
	++db->incomp_count;
	db->incomp_bytes += isize;
	olen              = 0;
      }
    else /* Count the number of compressed frames */
      {
	++db->comp_count;
	db->comp_bytes += olen;
      }

    /* Return the resulting output length */
    return olen;
#undef OUTPUT
#undef PUTBYTE
  }

/*
 * Update the "BSD Compress" dictionary on the receiver for
 * incompressible data by pretending to compress the incoming data.
 */

static void bsd_incomp (void *state, unsigned char *ibuf, int icnt)
  {
    (void) bsd_compress (state, ibuf, (char *) 0, icnt, 0);
  }

/*
 * Decompress "BSD Compress".
 *
 * Because of patent problems, we return DECOMP_ERROR for errors
 * found by inspecting the input data and for system problems, but
 * DECOMP_FATALERROR for any errors which could possibly be said to
 * be being detected "after" decompression.  For DECOMP_ERROR,
 * we can issue a CCP reset-request; for DECOMP_FATALERROR, we may be
 * infringing a patent of Motorola's if we do, so we take CCP down
 * instead.
 *
 * Given that the frame has the correct sequence number and a good FCS,
 * errors such as invalid codes in the input most likely indicate a
 * bug, so we return DECOMP_FATALERROR for them in order to turn off
 * compression, even though they are detected by inspecting the input.
 */

static int bsd_decompress (void *state, unsigned char *ibuf, int isize,
			   unsigned char *obuf, int osize)
  {
    struct bsd_db *db;
    unsigned int max_ent;
    unsigned long accm;
    unsigned int bitno;		/* 1st valid bit in accm */
    unsigned int n_bits;
    unsigned int tgtbitno;	/* bitno when we have a code */
    struct bsd_dict *dictp;
    int explen;
    int seq;
    unsigned int incode;
    unsigned int oldcode;
    unsigned int finchar;
    unsigned char *p;
    unsigned char *wptr;
    int adrs;
    int ctrl;
    int ilen;
    int codelen;
    int extra;

    db       = (struct bsd_db *) state;
    max_ent  = db->max_ent;
    accm     = 0;
    bitno    = 32;		/* 1st valid bit in accm */
    n_bits   = db->n_bits;
    tgtbitno = 32 - n_bits;	/* bitno when we have a code */

    /*
     * Save the address/control from the PPP header
     * and then get the sequence number.
     */

    adrs  = PPP_ADDRESS (ibuf);
    ctrl  = PPP_CONTROL (ibuf);

    seq   = (ibuf[4] << 8) + ibuf[5];

    ibuf += (PPP_HDRLEN + 2);
    ilen  = isize - (PPP_HDRLEN + 2);

    /*
     * Check the sequence number and give up if it differs from
     * the value we're expecting.
     */

    if (seq != db->seqno)
      {
	if (db->debug)
	  {
	    printk("bsd_decomp%d: bad sequence # %d, expected %d\n",
		   db->unit, seq, db->seqno - 1);
	  }
	return DECOMP_ERROR;
      }

    ++db->seqno;
    db->bytes_out += ilen;

    /*
     * Fill in the ppp header, but not the last byte of the protocol
     * (that comes from the decompressed data).
     */

    wptr    = obuf;
    *wptr++ = adrs;
    *wptr++ = ctrl;
    *wptr++ = 0;

    oldcode = CLEAR;
    explen  = 3;

    /*
     * Keep the checkpoint correctly so that incompressible packets
     * clear the dictionary at the proper times.
     */

    for (;;)
      {
	if (ilen-- <= 0)
	  {
	    db->in_count += (explen - 3); /* don't count the header */
	    break;
	  }

	/*
	 * Accumulate bytes until we have a complete code.
	 * Then get the next code, relying on the 32-bit,
	 * unsigned accm to mask the result.
	 */

	bitno -= 8;
	accm  |= *ibuf++ << bitno;
	if (tgtbitno < bitno)
	  {
	    continue;
	  }

	incode = accm >> tgtbitno;
	accm <<= n_bits;
	bitno += n_bits;

	/*
	 * The dictionary must only be cleared at the end of a packet.
	 */

	if (incode == CLEAR)
	  {
	    if (ilen > 0)
	      {
		if (db->debug)
		  {
		    printk("bsd_decomp%d: bad CLEAR\n", db->unit);
		  }
		return DECOMP_FATALERROR;	/* probably a bug */
	      }

	    bsd_clear(db);
	    break;
	  }

	if ((incode > max_ent + 2) || (incode > db->maxmaxcode)
	    || (incode > max_ent && oldcode == CLEAR))
	  {
	    if (db->debug)
	      {
		printk("bsd_decomp%d: bad code 0x%x oldcode=0x%x ",
		       db->unit, incode, oldcode);
		printk("max_ent=0x%x explen=%d seqno=%d\n",
		       max_ent, explen, db->seqno);
	      }
	    return DECOMP_FATALERROR;	/* probably a bug */
	  }

	/* Special case for KwKwK string. */
	if (incode > max_ent)
	  {
	    finchar = oldcode;
	    extra   = 1;
	  }
	else
	  {
	    finchar = incode;
	    extra   = 0;
	  }

	codelen = *(lens_ptr (db, finchar));
	explen += codelen + extra;
	if (explen > osize)
	  {
	    if (db->debug)
	      {
		printk("bsd_decomp%d: ran out of mru\n", db->unit);
#ifdef DEBUG
		printk("  len=%d, finchar=0x%x, codelen=%d, explen=%d\n",
		       ilen, finchar, codelen, explen);
#endif
	      }
	    return DECOMP_FATALERROR;
	  }

	/*
	 * Decode this code and install it in the decompressed buffer.
	 */

	wptr += codelen;
	p     = wptr;
	while (finchar > LAST)
	  {
	    struct bsd_dict *dictp2 = dict_ptr (db, finchar);

	    dictp = dict_ptr (db, dictp2->cptr);
#ifdef DEBUG
	    if (--codelen <= 0 || dictp->codem1 != finchar-1)
	      {
		if (codelen <= 0)
		  {
		    printk("bsd_decomp%d: fell off end of chain ", db->unit);
		    printk("0x%x at 0x%x by 0x%x, max_ent=0x%x\n",
			   incode, finchar, dictp2->cptr, max_ent);
		  }
		else
		  {
		    if (dictp->codem1 != finchar-1)
		      {
			printk("bsd_decomp%d: bad code chain 0x%x "
			       "finchar=0x%x ",
			       db->unit, incode, finchar);

			printk("oldcode=0x%x cptr=0x%x codem1=0x%x\n",
			       oldcode, dictp2->cptr, dictp->codem1);
		      }
		  }
		return DECOMP_FATALERROR;
	      }
#endif
	    *--p    = dictp->f.hs.suffix;
	    finchar = dictp->f.hs.prefix;
	  }
	*--p = finchar;

#ifdef DEBUG
	if (--codelen != 0)
	  {
	    printk("bsd_decomp%d: short by %d after code 0x%x, max_ent=0x%x\n",
		   db->unit, codelen, incode, max_ent);
	  }
#endif

	if (extra)		/* the KwKwK case again */
	  {
	    *wptr++ = finchar;
	  }

	/*
	 * If not first code in a packet, and
	 * if not out of code space, then allocate a new code.
	 *
	 * Keep the hash table correct so it can be used
	 * with uncompressed packets.
	 */

	if (oldcode != CLEAR && max_ent < db->maxmaxcode)
	  {
	    struct bsd_dict *dictp2, *dictp3;
	    unsigned short  *lens1,  *lens2;
	    unsigned long fcode;
	    int hval, disp, indx;

	    fcode = BSD_KEY(oldcode,finchar);
	    hval  = BSD_HASH(oldcode,finchar,db->hshift);
	    dictp = dict_ptr (db, hval);

	    /* look for a free hash table entry */
	    if (dictp->codem1 < max_ent)
	      {
		disp = (hval == 0) ? 1 : hval;
		do
		  {
		    hval += disp;
		    if (hval >= db->hsize)
		      {
			hval -= db->hsize;
		      }
		    dictp = dict_ptr (db, hval);
		  }
		while (dictp->codem1 < max_ent);
	      }

	    /*
	     * Invalidate previous hash table entry
	     * assigned this code, and then take it over
	     */

	    dictp2 = dict_ptr (db, max_ent + 1);
	    indx   = dictp2->cptr;
	    dictp3 = dict_ptr (db, indx);

	    if (dictp3->codem1 == max_ent)
	      {
		dictp3->codem1 = BADCODEM1;
	      }

	    dictp2->cptr   = hval;
	    dictp->codem1  = max_ent;
	    dictp->f.fcode = fcode;
	    db->max_ent    = ++max_ent;

	    /* Update the length of this string. */
	    lens1  = lens_ptr (db, max_ent);
	    lens2  = lens_ptr (db, oldcode);
	    *lens1 = *lens2 + 1;

	    /* Expand code size if needed. */
	    if (max_ent >= MAXCODE(n_bits) && max_ent < db->maxmaxcode)
	      {
		db->n_bits = ++n_bits;
		tgtbitno   = 32-n_bits;
	      }
	  }
	oldcode = incode;
      }

    ++db->comp_count;
    ++db->uncomp_count;
    db->comp_bytes   += isize - BSD_OVHD - PPP_HDRLEN;
    db->uncomp_bytes += explen;

    if (bsd_check(db))
      {
	if (db->debug)
	  {
	    printk("bsd_decomp%d: peer should have cleared dictionary on %d\n",
		   db->unit, db->seqno - 1);
	  }
      }
    return explen;
  }

/*************************************************************
 * Table of addresses for the BSD compression module
 *************************************************************/

static struct compressor ppp_bsd_compress = {
	.compress_proto =	CI_BSD_COMPRESS,
	.comp_alloc =		bsd_comp_alloc,
	.comp_free =		bsd_free,
	.comp_init =		bsd_comp_init,
	.comp_reset =		bsd_reset,
	.compress =		bsd_compress,
	.comp_stat =		bsd_comp_stats,
	.decomp_alloc =		bsd_decomp_alloc,
	.decomp_free =		bsd_free,
	.decomp_init =		bsd_decomp_init,
	.decomp_reset =		bsd_reset,
	.decompress =		bsd_decompress,
	.incomp =		bsd_incomp,
	.decomp_stat =		bsd_comp_stats,
	.owner =		THIS_MODULE
};

/*************************************************************
 * Module support routines
 *************************************************************/

static int __init bsdcomp_init(void)
{
	int answer = ppp_register_compressor(&ppp_bsd_compress);
	if (answer == 0)
		printk(KERN_INFO "PPP BSD Compression module registered\n");
	return answer;
}

static void __exit bsdcomp_cleanup(void)
{
	ppp_unregister_compressor(&ppp_bsd_compress);
}

module_init(bsdcomp_init);
module_exit(bsdcomp_cleanup);
MODULE_LICENSE("Dual BSD/GPL");
MODULE_ALIAS("ppp-compress-" __stringify(CI_BSD_COMPRESS));
