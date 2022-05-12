/* SPDX-License-Identifier: GPL-2.0 */
/*
 * CPU-measurement facilities
 *
 *  Copyright IBM Corp. 2012, 2018
 *  Author(s): Hendrik Brueckner <brueckner@linux.vnet.ibm.com>
 *	       Jan Glauber <jang@linux.vnet.ibm.com>
 */
#ifndef _ASM_S390_CPU_MF_H
#define _ASM_S390_CPU_MF_H

#include <linux/errno.h>
#include <asm/facility.h>

asm(".include \"asm/cpu_mf-insn.h\"\n");

#define CPU_MF_INT_SF_IAE	(1 << 31)	/* invalid entry address */
#define CPU_MF_INT_SF_ISE	(1 << 30)	/* incorrect SDBT entry */
#define CPU_MF_INT_SF_PRA	(1 << 29)	/* program request alert */
#define CPU_MF_INT_SF_SACA	(1 << 23)	/* sampler auth. change alert */
#define CPU_MF_INT_SF_LSDA	(1 << 22)	/* loss of sample data alert */
#define CPU_MF_INT_CF_MTDA	(1 << 15)	/* loss of MT ctr. data alert */
#define CPU_MF_INT_CF_CACA	(1 <<  7)	/* counter auth. change alert */
#define CPU_MF_INT_CF_LCDA	(1 <<  6)	/* loss of counter data alert */
#define CPU_MF_INT_CF_MASK	(CPU_MF_INT_CF_MTDA|CPU_MF_INT_CF_CACA| \
				 CPU_MF_INT_CF_LCDA)
#define CPU_MF_INT_SF_MASK	(CPU_MF_INT_SF_IAE|CPU_MF_INT_SF_ISE|	\
				 CPU_MF_INT_SF_PRA|CPU_MF_INT_SF_SACA|	\
				 CPU_MF_INT_SF_LSDA)

#define CPU_MF_SF_RIBM_NOTAV	0x1		/* Sampling unavailable */

/* CPU measurement facility support */
static inline int cpum_cf_avail(void)
{
	return test_facility(40) && test_facility(67);
}

static inline int cpum_sf_avail(void)
{
	return test_facility(40) && test_facility(68);
}


struct cpumf_ctr_info {
	u16   cfvn;
	u16   auth_ctl;
	u16   enable_ctl;
	u16   act_ctl;
	u16   max_cpu;
	u16   csvn;
	u16   max_cg;
	u16   reserved1;
	u32   reserved2[12];
} __packed;

/* QUERY SAMPLING INFORMATION block */
struct hws_qsi_info_block {	    /* Bit(s) */
	unsigned int b0_13:14;	    /* 0-13: zeros			 */
	unsigned int as:1;	    /* 14: basic-sampling authorization	 */
	unsigned int ad:1;	    /* 15: diag-sampling authorization	 */
	unsigned int b16_21:6;	    /* 16-21: zeros			 */
	unsigned int es:1;	    /* 22: basic-sampling enable control */
	unsigned int ed:1;	    /* 23: diag-sampling enable control	 */
	unsigned int b24_29:6;	    /* 24-29: zeros			 */
	unsigned int cs:1;	    /* 30: basic-sampling activation control */
	unsigned int cd:1;	    /* 31: diag-sampling activation control */
	unsigned int bsdes:16;	    /* 4-5: size of basic sampling entry */
	unsigned int dsdes:16;	    /* 6-7: size of diagnostic sampling entry */
	unsigned long min_sampl_rate; /* 8-15: minimum sampling interval */
	unsigned long max_sampl_rate; /* 16-23: maximum sampling interval*/
	unsigned long tear;	    /* 24-31: TEAR contents		 */
	unsigned long dear;	    /* 32-39: DEAR contents		 */
	unsigned int rsvrd0:24;	    /* 40-42: reserved			 */
	unsigned int ribm:8;	    /* 43: Reserved by IBM		 */
	unsigned int cpu_speed;     /* 44-47: CPU speed			 */
	unsigned long long rsvrd1;  /* 48-55: reserved			 */
	unsigned long long rsvrd2;  /* 56-63: reserved			 */
} __packed;

/* SET SAMPLING CONTROLS request block */
struct hws_lsctl_request_block {
	unsigned int s:1;	    /* 0: maximum buffer indicator	 */
	unsigned int h:1;	    /* 1: part. level reserved for VM use*/
	unsigned long long b2_53:52;/* 2-53: zeros			 */
	unsigned int es:1;	    /* 54: basic-sampling enable control */
	unsigned int ed:1;	    /* 55: diag-sampling enable control	 */
	unsigned int b56_61:6;	    /* 56-61: - zeros			 */
	unsigned int cs:1;	    /* 62: basic-sampling activation control */
	unsigned int cd:1;	    /* 63: diag-sampling activation control  */
	unsigned long interval;     /* 8-15: sampling interval		 */
	unsigned long tear;	    /* 16-23: TEAR contents		 */
	unsigned long dear;	    /* 24-31: DEAR contents		 */
	/* 32-63:							 */
	unsigned long rsvrd1;	    /* reserved				 */
	unsigned long rsvrd2;	    /* reserved				 */
	unsigned long rsvrd3;	    /* reserved				 */
	unsigned long rsvrd4;	    /* reserved				 */
} __packed;

struct hws_basic_entry {
	unsigned int def:16;	    /* 0-15  Data Entry Format		 */
	unsigned int R:4;	    /* 16-19 reserved			 */
	unsigned int U:4;	    /* 20-23 Number of unique instruct.  */
	unsigned int z:2;	    /* zeros				 */
	unsigned int T:1;	    /* 26 PSW DAT mode			 */
	unsigned int W:1;	    /* 27 PSW wait state		 */
	unsigned int P:1;	    /* 28 PSW Problem state		 */
	unsigned int AS:2;	    /* 29-30 PSW address-space control	 */
	unsigned int I:1;	    /* 31 entry valid or invalid	 */
	unsigned int CL:2;	    /* 32-33 Configuration Level	 */
	unsigned int:14;
	unsigned int prim_asn:16;   /* primary ASN			 */
	unsigned long long ia;	    /* Instruction Address		 */
	unsigned long long gpp;     /* Guest Program Parameter		 */
	unsigned long long hpp;     /* Host Program Parameter		 */
} __packed;

struct hws_diag_entry {
	unsigned int def:16;	    /* 0-15  Data Entry Format		 */
	unsigned int R:15;	    /* 16-19 and 20-30 reserved		 */
	unsigned int I:1;	    /* 31 entry valid or invalid	 */
	u8	     data[];	    /* Machine-dependent sample data	 */
} __packed;

struct hws_combined_entry {
	struct hws_basic_entry	basic;	/* Basic-sampling data entry */
	struct hws_diag_entry	diag;	/* Diagnostic-sampling data entry */
} __packed;

struct hws_trailer_entry {
	union {
		struct {
			unsigned int f:1;	/* 0 - Block Full Indicator   */
			unsigned int a:1;	/* 1 - Alert request control  */
			unsigned int t:1;	/* 2 - Timestamp format	      */
			unsigned int :29;	/* 3 - 31: Reserved	      */
			unsigned int bsdes:16;	/* 32-47: size of basic SDE   */
			unsigned int dsdes:16;	/* 48-63: size of diagnostic SDE */
		};
		unsigned long long flags;	/* 0 - 63: All indicators     */
	};
	unsigned long long overflow;	 /* 64 - sample Overflow count	      */
	unsigned char timestamp[16];	 /* 16 - 31 timestamp		      */
	unsigned long long reserved1;	 /* 32 -Reserved		      */
	unsigned long long reserved2;	 /*				      */
	union {				 /* 48 - reserved for programming use */
		struct {
			unsigned int clock_base:1; /* in progusage2 */
			unsigned long long progusage1:63;
			unsigned long long progusage2;
		};
		unsigned long long progusage[2];
	};
} __packed;

/* Load program parameter */
static inline void lpp(void *pp)
{
	asm volatile(".insn s,0xb2800000,0(%0)\n":: "a" (pp) : "memory");
}

/* Query counter information */
static inline int qctri(struct cpumf_ctr_info *info)
{
	int rc = -EINVAL;

	asm volatile (
		"0:	.insn	s,0xb28e0000,%1\n"
		"1:	lhi	%0,0\n"
		"2:\n"
		EX_TABLE(1b, 2b)
		: "+d" (rc), "=Q" (*info));
	return rc;
}

/* Load CPU-counter-set controls */
static inline int lcctl(u64 ctl)
{
	int cc;

	asm volatile (
		"	.insn	s,0xb2840000,%1\n"
		"	ipm	%0\n"
		"	srl	%0,28\n"
		: "=d" (cc) : "Q" (ctl) : "cc");
	return cc;
}

/* Extract CPU counter */
static inline int __ecctr(u64 ctr, u64 *content)
{
	u64 _content;
	int cc;

	asm volatile (
		"	.insn	rre,0xb2e40000,%0,%2\n"
		"	ipm	%1\n"
		"	srl	%1,28\n"
		: "=d" (_content), "=d" (cc) : "d" (ctr) : "cc");
	*content = _content;
	return cc;
}

/* Extract CPU counter */
static inline int ecctr(u64 ctr, u64 *val)
{
	u64 content;
	int cc;

	cc = __ecctr(ctr, &content);
	if (!cc)
		*val = content;
	return cc;
}

/* Store CPU counter multiple for a particular counter set */
enum stcctm_ctr_set {
	EXTENDED = 0,
	BASIC = 1,
	PROBLEM_STATE = 2,
	CRYPTO_ACTIVITY = 3,
	MT_DIAG = 5,
	MT_DIAG_CLEARING = 9,	/* clears loss-of-MT-ctr-data alert */
};

static __always_inline int stcctm(enum stcctm_ctr_set set, u64 range, u64 *dest)
{
	int cc;

	asm volatile (
		"	STCCTM	%2,%3,%1\n"
		"	ipm	%0\n"
		"	srl	%0,28\n"
		: "=d" (cc)
		: "Q" (*dest), "d" (range), "i" (set)
		: "cc", "memory");
	return cc;
}

/* Query sampling information */
static inline int qsi(struct hws_qsi_info_block *info)
{
	int cc = 1;

	asm volatile(
		"0:	.insn	s,0xb2860000,%1\n"
		"1:	lhi	%0,0\n"
		"2:\n"
		EX_TABLE(0b, 2b) EX_TABLE(1b, 2b)
		: "+d" (cc), "+Q" (*info));
	return cc ? -EINVAL : 0;
}

/* Load sampling controls */
static inline int lsctl(struct hws_lsctl_request_block *req)
{
	int cc;

	cc = 1;
	asm volatile(
		"0:	.insn	s,0xb2870000,0(%1)\n"
		"1:	ipm	%0\n"
		"	srl	%0,28\n"
		"2:\n"
		EX_TABLE(0b, 2b) EX_TABLE(1b, 2b)
		: "+d" (cc), "+a" (req)
		: "m" (*req)
		: "cc", "memory");

	return cc ? -EINVAL : 0;
}

/* Sampling control helper functions */

#include <linux/time.h>

static inline unsigned long freq_to_sample_rate(struct hws_qsi_info_block *qsi,
						unsigned long freq)
{
	return (USEC_PER_SEC / freq) * qsi->cpu_speed;
}

static inline unsigned long sample_rate_to_freq(struct hws_qsi_info_block *qsi,
						unsigned long rate)
{
	return USEC_PER_SEC * qsi->cpu_speed / rate;
}

#define SDB_TE_ALERT_REQ_MASK	0x4000000000000000UL
#define SDB_TE_BUFFER_FULL_MASK 0x8000000000000000UL

/* Return TOD timestamp contained in an trailer entry */
static inline unsigned long long trailer_timestamp(struct hws_trailer_entry *te)
{
	/* TOD in STCKE format */
	if (te->t)
		return *((unsigned long long *) &te->timestamp[1]);

	/* TOD in STCK format */
	return *((unsigned long long *) &te->timestamp[0]);
}

/* Return pointer to trailer entry of an sample data block */
static inline unsigned long *trailer_entry_ptr(unsigned long v)
{
	void *ret;

	ret = (void *) v;
	ret += PAGE_SIZE;
	ret -= sizeof(struct hws_trailer_entry);

	return (unsigned long *) ret;
}

/* Return true if the entry in the sample data block table (sdbt)
 * is a link to the next sdbt */
static inline int is_link_entry(unsigned long *s)
{
	return *s & 0x1ul ? 1 : 0;
}

/* Return pointer to the linked sdbt */
static inline unsigned long *get_next_sdbt(unsigned long *s)
{
	return (unsigned long *) (*s & ~0x1ul);
}
#endif /* _ASM_S390_CPU_MF_H */
