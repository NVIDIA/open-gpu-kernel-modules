/* SPDX-License-Identifier: GPL-2.0 */
/*
 * caam descriptor construction helper functions
 *
 * Copyright 2008-2012 Freescale Semiconductor, Inc.
 * Copyright 2019 NXP
 */

#ifndef DESC_CONSTR_H
#define DESC_CONSTR_H

#include "desc.h"
#include "regs.h"

#define IMMEDIATE (1 << 23)
#define CAAM_CMD_SZ sizeof(u32)
#define CAAM_PTR_SZ caam_ptr_sz
#define CAAM_PTR_SZ_MAX sizeof(dma_addr_t)
#define CAAM_PTR_SZ_MIN sizeof(u32)
#define CAAM_DESC_BYTES_MAX (CAAM_CMD_SZ * MAX_CAAM_DESCSIZE)
#define __DESC_JOB_IO_LEN(n) (CAAM_CMD_SZ * 5 + (n) * 3)
#define DESC_JOB_IO_LEN __DESC_JOB_IO_LEN(CAAM_PTR_SZ)
#define DESC_JOB_IO_LEN_MAX __DESC_JOB_IO_LEN(CAAM_PTR_SZ_MAX)
#define DESC_JOB_IO_LEN_MIN __DESC_JOB_IO_LEN(CAAM_PTR_SZ_MIN)

/*
 * The CAAM QI hardware constructs a job descriptor which points
 * to shared descriptor (as pointed by context_a of FQ to CAAM).
 * When the job descriptor is executed by deco, the whole job
 * descriptor together with shared descriptor gets loaded in
 * deco buffer which is 64 words long (each 32-bit).
 *
 * The job descriptor constructed by QI hardware has layout:
 *
 *	HEADER		(1 word)
 *	Shdesc ptr	(1 or 2 words)
 *	SEQ_OUT_PTR	(1 word)
 *	Out ptr		(1 or 2 words)
 *	Out length	(1 word)
 *	SEQ_IN_PTR	(1 word)
 *	In ptr		(1 or 2 words)
 *	In length	(1 word)
 *
 * The shdesc ptr is used to fetch shared descriptor contents
 * into deco buffer.
 *
 * Apart from shdesc contents, the total number of words that
 * get loaded in deco buffer are '8' or '11'. The remaining words
 * in deco buffer can be used for storing shared descriptor.
 */
#define MAX_SDLEN	((CAAM_DESC_BYTES_MAX - DESC_JOB_IO_LEN_MIN) / CAAM_CMD_SZ)

#ifdef DEBUG
#define PRINT_POS do { printk(KERN_DEBUG "%02d: %s\n", desc_len(desc),\
			      &__func__[sizeof("append")]); } while (0)
#else
#define PRINT_POS
#endif

#define SET_OK_NO_PROP_ERRORS (IMMEDIATE | LDST_CLASS_DECO | \
			       LDST_SRCDST_WORD_DECOCTRL | \
			       (LDOFF_CHG_SHARE_OK_NO_PROP << \
				LDST_OFFSET_SHIFT))
#define DISABLE_AUTO_INFO_FIFO (IMMEDIATE | LDST_CLASS_DECO | \
				LDST_SRCDST_WORD_DECOCTRL | \
				(LDOFF_DISABLE_AUTO_NFIFO << LDST_OFFSET_SHIFT))
#define ENABLE_AUTO_INFO_FIFO (IMMEDIATE | LDST_CLASS_DECO | \
			       LDST_SRCDST_WORD_DECOCTRL | \
			       (LDOFF_ENABLE_AUTO_NFIFO << LDST_OFFSET_SHIFT))

extern bool caam_little_end;
extern size_t caam_ptr_sz;

/*
 * HW fetches 4 S/G table entries at a time, irrespective of how many entries
 * are in the table. It's SW's responsibility to make sure these accesses
 * do not have side effects.
 */
static inline int pad_sg_nents(int sg_nents)
{
	return ALIGN(sg_nents, 4);
}

static inline int desc_len(u32 * const desc)
{
	return caam32_to_cpu(*desc) & HDR_DESCLEN_MASK;
}

static inline int desc_bytes(void * const desc)
{
	return desc_len(desc) * CAAM_CMD_SZ;
}

static inline u32 *desc_end(u32 * const desc)
{
	return desc + desc_len(desc);
}

static inline void *sh_desc_pdb(u32 * const desc)
{
	return desc + 1;
}

static inline void init_desc(u32 * const desc, u32 options)
{
	*desc = cpu_to_caam32((options | HDR_ONE) + 1);
}

static inline void init_sh_desc(u32 * const desc, u32 options)
{
	PRINT_POS;
	init_desc(desc, CMD_SHARED_DESC_HDR | options);
}

static inline void init_sh_desc_pdb(u32 * const desc, u32 options,
				    size_t pdb_bytes)
{
	u32 pdb_len = (pdb_bytes + CAAM_CMD_SZ - 1) / CAAM_CMD_SZ;

	init_sh_desc(desc, (((pdb_len + 1) << HDR_START_IDX_SHIFT) + pdb_len) |
		     options);
}

static inline void init_job_desc(u32 * const desc, u32 options)
{
	init_desc(desc, CMD_DESC_HDR | options);
}

static inline void init_job_desc_pdb(u32 * const desc, u32 options,
				     size_t pdb_bytes)
{
	u32 pdb_len = (pdb_bytes + CAAM_CMD_SZ - 1) / CAAM_CMD_SZ;

	init_job_desc(desc, (((pdb_len + 1) << HDR_START_IDX_SHIFT)) | options);
}

static inline void append_ptr(u32 * const desc, dma_addr_t ptr)
{
	if (caam_ptr_sz == sizeof(dma_addr_t)) {
		dma_addr_t *offset = (dma_addr_t *)desc_end(desc);

		*offset = cpu_to_caam_dma(ptr);
	} else {
		u32 *offset = (u32 *)desc_end(desc);

		*offset = cpu_to_caam_dma(ptr);
	}

	(*desc) = cpu_to_caam32(caam32_to_cpu(*desc) +
				CAAM_PTR_SZ / CAAM_CMD_SZ);
}

static inline void init_job_desc_shared(u32 * const desc, dma_addr_t ptr,
					int len, u32 options)
{
	PRINT_POS;
	init_job_desc(desc, HDR_SHARED | options |
		      (len << HDR_START_IDX_SHIFT));
	append_ptr(desc, ptr);
}

static inline void append_data(u32 * const desc, const void *data, int len)
{
	u32 *offset = desc_end(desc);

	if (len) /* avoid sparse warning: memcpy with byte count of 0 */
		memcpy(offset, data, len);

	(*desc) = cpu_to_caam32(caam32_to_cpu(*desc) +
				(len + CAAM_CMD_SZ - 1) / CAAM_CMD_SZ);
}

static inline void append_cmd(u32 * const desc, u32 command)
{
	u32 *cmd = desc_end(desc);

	*cmd = cpu_to_caam32(command);

	(*desc) = cpu_to_caam32(caam32_to_cpu(*desc) + 1);
}

#define append_u32 append_cmd

static inline void append_u64(u32 * const desc, u64 data)
{
	u32 *offset = desc_end(desc);

	/* Only 32-bit alignment is guaranteed in descriptor buffer */
	if (caam_little_end) {
		*offset = cpu_to_caam32(lower_32_bits(data));
		*(++offset) = cpu_to_caam32(upper_32_bits(data));
	} else {
		*offset = cpu_to_caam32(upper_32_bits(data));
		*(++offset) = cpu_to_caam32(lower_32_bits(data));
	}

	(*desc) = cpu_to_caam32(caam32_to_cpu(*desc) + 2);
}

/* Write command without affecting header, and return pointer to next word */
static inline u32 *write_cmd(u32 * const desc, u32 command)
{
	*desc = cpu_to_caam32(command);

	return desc + 1;
}

static inline void append_cmd_ptr(u32 * const desc, dma_addr_t ptr, int len,
				  u32 command)
{
	append_cmd(desc, command | len);
	append_ptr(desc, ptr);
}

/* Write length after pointer, rather than inside command */
static inline void append_cmd_ptr_extlen(u32 * const desc, dma_addr_t ptr,
					 unsigned int len, u32 command)
{
	append_cmd(desc, command);
	if (!(command & (SQIN_RTO | SQIN_PRE)))
		append_ptr(desc, ptr);
	append_cmd(desc, len);
}

static inline void append_cmd_data(u32 * const desc, const void *data, int len,
				   u32 command)
{
	append_cmd(desc, command | IMMEDIATE | len);
	append_data(desc, data, len);
}

#define APPEND_CMD_RET(cmd, op) \
static inline u32 *append_##cmd(u32 * const desc, u32 options) \
{ \
	u32 *cmd = desc_end(desc); \
	PRINT_POS; \
	append_cmd(desc, CMD_##op | options); \
	return cmd; \
}
APPEND_CMD_RET(jump, JUMP)
APPEND_CMD_RET(move, MOVE)
APPEND_CMD_RET(move_len, MOVE_LEN)

static inline void set_jump_tgt_here(u32 * const desc, u32 *jump_cmd)
{
	*jump_cmd = cpu_to_caam32(caam32_to_cpu(*jump_cmd) |
				  (desc_len(desc) - (jump_cmd - desc)));
}

static inline void set_move_tgt_here(u32 * const desc, u32 *move_cmd)
{
	u32 val = caam32_to_cpu(*move_cmd);

	val &= ~MOVE_OFFSET_MASK;
	val |= (desc_len(desc) << (MOVE_OFFSET_SHIFT + 2)) & MOVE_OFFSET_MASK;
	*move_cmd = cpu_to_caam32(val);
}

#define APPEND_CMD(cmd, op) \
static inline void append_##cmd(u32 * const desc, u32 options) \
{ \
	PRINT_POS; \
	append_cmd(desc, CMD_##op | options); \
}
APPEND_CMD(operation, OPERATION)

#define APPEND_CMD_LEN(cmd, op) \
static inline void append_##cmd(u32 * const desc, unsigned int len, \
				u32 options) \
{ \
	PRINT_POS; \
	append_cmd(desc, CMD_##op | len | options); \
}

APPEND_CMD_LEN(seq_load, SEQ_LOAD)
APPEND_CMD_LEN(seq_store, SEQ_STORE)
APPEND_CMD_LEN(seq_fifo_load, SEQ_FIFO_LOAD)
APPEND_CMD_LEN(seq_fifo_store, SEQ_FIFO_STORE)

#define APPEND_CMD_PTR(cmd, op) \
static inline void append_##cmd(u32 * const desc, dma_addr_t ptr, \
				unsigned int len, u32 options) \
{ \
	PRINT_POS; \
	append_cmd_ptr(desc, ptr, len, CMD_##op | options); \
}
APPEND_CMD_PTR(key, KEY)
APPEND_CMD_PTR(load, LOAD)
APPEND_CMD_PTR(fifo_load, FIFO_LOAD)
APPEND_CMD_PTR(fifo_store, FIFO_STORE)

static inline void append_store(u32 * const desc, dma_addr_t ptr,
				unsigned int len, u32 options)
{
	u32 cmd_src;

	cmd_src = options & LDST_SRCDST_MASK;

	append_cmd(desc, CMD_STORE | options | len);

	/* The following options do not require pointer */
	if (!(cmd_src == LDST_SRCDST_WORD_DESCBUF_SHARED ||
	      cmd_src == LDST_SRCDST_WORD_DESCBUF_JOB    ||
	      cmd_src == LDST_SRCDST_WORD_DESCBUF_JOB_WE ||
	      cmd_src == LDST_SRCDST_WORD_DESCBUF_SHARED_WE))
		append_ptr(desc, ptr);
}

#define APPEND_SEQ_PTR_INTLEN(cmd, op) \
static inline void append_seq_##cmd##_ptr_intlen(u32 * const desc, \
						 dma_addr_t ptr, \
						 unsigned int len, \
						 u32 options) \
{ \
	PRINT_POS; \
	if (options & (SQIN_RTO | SQIN_PRE)) \
		append_cmd(desc, CMD_SEQ_##op##_PTR | len | options); \
	else \
		append_cmd_ptr(desc, ptr, len, CMD_SEQ_##op##_PTR | options); \
}
APPEND_SEQ_PTR_INTLEN(in, IN)
APPEND_SEQ_PTR_INTLEN(out, OUT)

#define APPEND_CMD_PTR_TO_IMM(cmd, op) \
static inline void append_##cmd##_as_imm(u32 * const desc, const void *data, \
					 unsigned int len, u32 options) \
{ \
	PRINT_POS; \
	append_cmd_data(desc, data, len, CMD_##op | options); \
}
APPEND_CMD_PTR_TO_IMM(load, LOAD);
APPEND_CMD_PTR_TO_IMM(fifo_load, FIFO_LOAD);

#define APPEND_CMD_PTR_EXTLEN(cmd, op) \
static inline void append_##cmd##_extlen(u32 * const desc, dma_addr_t ptr, \
					 unsigned int len, u32 options) \
{ \
	PRINT_POS; \
	append_cmd_ptr_extlen(desc, ptr, len, CMD_##op | SQIN_EXT | options); \
}
APPEND_CMD_PTR_EXTLEN(seq_in_ptr, SEQ_IN_PTR)
APPEND_CMD_PTR_EXTLEN(seq_out_ptr, SEQ_OUT_PTR)

/*
 * Determine whether to store length internally or externally depending on
 * the size of its type
 */
#define APPEND_CMD_PTR_LEN(cmd, op, type) \
static inline void append_##cmd(u32 * const desc, dma_addr_t ptr, \
				type len, u32 options) \
{ \
	PRINT_POS; \
	if (sizeof(type) > sizeof(u16)) \
		append_##cmd##_extlen(desc, ptr, len, options); \
	else \
		append_##cmd##_intlen(desc, ptr, len, options); \
}
APPEND_CMD_PTR_LEN(seq_in_ptr, SEQ_IN_PTR, u32)
APPEND_CMD_PTR_LEN(seq_out_ptr, SEQ_OUT_PTR, u32)

/*
 * 2nd variant for commands whose specified immediate length differs
 * from length of immediate data provided, e.g., split keys
 */
#define APPEND_CMD_PTR_TO_IMM2(cmd, op) \
static inline void append_##cmd##_as_imm(u32 * const desc, const void *data, \
					 unsigned int data_len, \
					 unsigned int len, u32 options) \
{ \
	PRINT_POS; \
	append_cmd(desc, CMD_##op | IMMEDIATE | len | options); \
	append_data(desc, data, data_len); \
}
APPEND_CMD_PTR_TO_IMM2(key, KEY);

#define APPEND_CMD_RAW_IMM(cmd, op, type) \
static inline void append_##cmd##_imm_##type(u32 * const desc, type immediate, \
					     u32 options) \
{ \
	PRINT_POS; \
	if (options & LDST_LEN_MASK) \
		append_cmd(desc, CMD_##op | IMMEDIATE | options); \
	else \
		append_cmd(desc, CMD_##op | IMMEDIATE | options | \
			   sizeof(type)); \
	append_cmd(desc, immediate); \
}
APPEND_CMD_RAW_IMM(load, LOAD, u32);

/*
 * ee - endianness
 * size - size of immediate type in bytes
 */
#define APPEND_CMD_RAW_IMM2(cmd, op, ee, size) \
static inline void append_##cmd##_imm_##ee##size(u32 *desc, \
						   u##size immediate, \
						   u32 options) \
{ \
	__##ee##size data = cpu_to_##ee##size(immediate); \
	PRINT_POS; \
	append_cmd(desc, CMD_##op | IMMEDIATE | options | sizeof(data)); \
	append_data(desc, &data, sizeof(data)); \
}

APPEND_CMD_RAW_IMM2(load, LOAD, be, 32);

/*
 * Append math command. Only the last part of destination and source need to
 * be specified
 */
#define APPEND_MATH(op, desc, dest, src_0, src_1, len) \
append_cmd(desc, CMD_MATH | MATH_FUN_##op | MATH_DEST_##dest | \
	MATH_SRC0_##src_0 | MATH_SRC1_##src_1 | (u32)len);

#define append_math_add(desc, dest, src0, src1, len) \
	APPEND_MATH(ADD, desc, dest, src0, src1, len)
#define append_math_sub(desc, dest, src0, src1, len) \
	APPEND_MATH(SUB, desc, dest, src0, src1, len)
#define append_math_add_c(desc, dest, src0, src1, len) \
	APPEND_MATH(ADDC, desc, dest, src0, src1, len)
#define append_math_sub_b(desc, dest, src0, src1, len) \
	APPEND_MATH(SUBB, desc, dest, src0, src1, len)
#define append_math_and(desc, dest, src0, src1, len) \
	APPEND_MATH(AND, desc, dest, src0, src1, len)
#define append_math_or(desc, dest, src0, src1, len) \
	APPEND_MATH(OR, desc, dest, src0, src1, len)
#define append_math_xor(desc, dest, src0, src1, len) \
	APPEND_MATH(XOR, desc, dest, src0, src1, len)
#define append_math_lshift(desc, dest, src0, src1, len) \
	APPEND_MATH(LSHIFT, desc, dest, src0, src1, len)
#define append_math_rshift(desc, dest, src0, src1, len) \
	APPEND_MATH(RSHIFT, desc, dest, src0, src1, len)
#define append_math_ldshift(desc, dest, src0, src1, len) \
	APPEND_MATH(SHLD, desc, dest, src0, src1, len)

/* Exactly one source is IMM. Data is passed in as u32 value */
#define APPEND_MATH_IMM_u32(op, desc, dest, src_0, src_1, data) \
do { \
	APPEND_MATH(op, desc, dest, src_0, src_1, CAAM_CMD_SZ); \
	append_cmd(desc, data); \
} while (0)

#define append_math_add_imm_u32(desc, dest, src0, src1, data) \
	APPEND_MATH_IMM_u32(ADD, desc, dest, src0, src1, data)
#define append_math_sub_imm_u32(desc, dest, src0, src1, data) \
	APPEND_MATH_IMM_u32(SUB, desc, dest, src0, src1, data)
#define append_math_add_c_imm_u32(desc, dest, src0, src1, data) \
	APPEND_MATH_IMM_u32(ADDC, desc, dest, src0, src1, data)
#define append_math_sub_b_imm_u32(desc, dest, src0, src1, data) \
	APPEND_MATH_IMM_u32(SUBB, desc, dest, src0, src1, data)
#define append_math_and_imm_u32(desc, dest, src0, src1, data) \
	APPEND_MATH_IMM_u32(AND, desc, dest, src0, src1, data)
#define append_math_or_imm_u32(desc, dest, src0, src1, data) \
	APPEND_MATH_IMM_u32(OR, desc, dest, src0, src1, data)
#define append_math_xor_imm_u32(desc, dest, src0, src1, data) \
	APPEND_MATH_IMM_u32(XOR, desc, dest, src0, src1, data)
#define append_math_lshift_imm_u32(desc, dest, src0, src1, data) \
	APPEND_MATH_IMM_u32(LSHIFT, desc, dest, src0, src1, data)
#define append_math_rshift_imm_u32(desc, dest, src0, src1, data) \
	APPEND_MATH_IMM_u32(RSHIFT, desc, dest, src0, src1, data)

/* Exactly one source is IMM. Data is passed in as u64 value */
#define APPEND_MATH_IMM_u64(op, desc, dest, src_0, src_1, data) \
do { \
	u32 upper = (data >> 16) >> 16; \
	APPEND_MATH(op, desc, dest, src_0, src_1, CAAM_CMD_SZ * 2 | \
		    (upper ? 0 : MATH_IFB)); \
	if (upper) \
		append_u64(desc, data); \
	else \
		append_u32(desc, lower_32_bits(data)); \
} while (0)

#define append_math_add_imm_u64(desc, dest, src0, src1, data) \
	APPEND_MATH_IMM_u64(ADD, desc, dest, src0, src1, data)
#define append_math_sub_imm_u64(desc, dest, src0, src1, data) \
	APPEND_MATH_IMM_u64(SUB, desc, dest, src0, src1, data)
#define append_math_add_c_imm_u64(desc, dest, src0, src1, data) \
	APPEND_MATH_IMM_u64(ADDC, desc, dest, src0, src1, data)
#define append_math_sub_b_imm_u64(desc, dest, src0, src1, data) \
	APPEND_MATH_IMM_u64(SUBB, desc, dest, src0, src1, data)
#define append_math_and_imm_u64(desc, dest, src0, src1, data) \
	APPEND_MATH_IMM_u64(AND, desc, dest, src0, src1, data)
#define append_math_or_imm_u64(desc, dest, src0, src1, data) \
	APPEND_MATH_IMM_u64(OR, desc, dest, src0, src1, data)
#define append_math_xor_imm_u64(desc, dest, src0, src1, data) \
	APPEND_MATH_IMM_u64(XOR, desc, dest, src0, src1, data)
#define append_math_lshift_imm_u64(desc, dest, src0, src1, data) \
	APPEND_MATH_IMM_u64(LSHIFT, desc, dest, src0, src1, data)
#define append_math_rshift_imm_u64(desc, dest, src0, src1, data) \
	APPEND_MATH_IMM_u64(RSHIFT, desc, dest, src0, src1, data)

/**
 * struct alginfo - Container for algorithm details
 * @algtype: algorithm selector; for valid values, see documentation of the
 *           functions where it is used.
 * @keylen: length of the provided algorithm key, in bytes
 * @keylen_pad: padded length of the provided algorithm key, in bytes
 * @key_dma: dma (bus) address where algorithm key resides
 * @key_virt: virtual address where algorithm key resides
 * @key_inline: true - key can be inlined in the descriptor; false - key is
 *              referenced by the descriptor
 */
struct alginfo {
	u32 algtype;
	unsigned int keylen;
	unsigned int keylen_pad;
	dma_addr_t key_dma;
	const void *key_virt;
	bool key_inline;
};

/**
 * desc_inline_query() - Provide indications on which data items can be inlined
 *                       and which shall be referenced in a shared descriptor.
 * @sd_base_len: Shared descriptor base length - bytes consumed by the commands,
 *               excluding the data items to be inlined (or corresponding
 *               pointer if an item is not inlined). Each cnstr_* function that
 *               generates descriptors should have a define mentioning
 *               corresponding length.
 * @jd_len: Maximum length of the job descriptor(s) that will be used
 *          together with the shared descriptor.
 * @data_len: Array of lengths of the data items trying to be inlined
 * @inl_mask: 32bit mask with bit x = 1 if data item x can be inlined, 0
 *            otherwise.
 * @count: Number of data items (size of @data_len array); must be <= 32
 *
 * Return: 0 if data can be inlined / referenced, negative value if not. If 0,
 *         check @inl_mask for details.
 */
static inline int desc_inline_query(unsigned int sd_base_len,
				    unsigned int jd_len, unsigned int *data_len,
				    u32 *inl_mask, unsigned int count)
{
	int rem_bytes = (int)(CAAM_DESC_BYTES_MAX - sd_base_len - jd_len);
	unsigned int i;

	*inl_mask = 0;
	for (i = 0; (i < count) && (rem_bytes > 0); i++) {
		if (rem_bytes - (int)(data_len[i] +
			(count - i - 1) * CAAM_PTR_SZ) >= 0) {
			rem_bytes -= data_len[i];
			*inl_mask |= (1 << i);
		} else {
			rem_bytes -= CAAM_PTR_SZ;
		}
	}

	return (rem_bytes >= 0) ? 0 : -1;
}

/**
 * append_proto_dkp - Derived Key Protocol (DKP): key -> split key
 * @desc: pointer to buffer used for descriptor construction
 * @adata: pointer to authentication transform definitions.
 *         keylen should be the length of initial key, while keylen_pad
 *         the length of the derived (split) key.
 *         Valid algorithm values - one of OP_ALG_ALGSEL_{MD5, SHA1, SHA224,
 *         SHA256, SHA384, SHA512}.
 */
static inline void append_proto_dkp(u32 * const desc, struct alginfo *adata)
{
	u32 protid;

	/*
	 * Quick & dirty translation from OP_ALG_ALGSEL_{MD5, SHA*}
	 * to OP_PCLID_DKP_{MD5, SHA*}
	 */
	protid = (adata->algtype & OP_ALG_ALGSEL_SUBMASK) |
		 (0x20 << OP_ALG_ALGSEL_SHIFT);

	if (adata->key_inline) {
		int words;

		if (adata->keylen > adata->keylen_pad) {
			append_operation(desc, OP_TYPE_UNI_PROTOCOL | protid |
					 OP_PCL_DKP_SRC_PTR |
					 OP_PCL_DKP_DST_IMM | adata->keylen);
			append_ptr(desc, adata->key_dma);

			words = (ALIGN(adata->keylen_pad, CAAM_CMD_SZ) -
				 CAAM_PTR_SZ) / CAAM_CMD_SZ;
		} else {
			append_operation(desc, OP_TYPE_UNI_PROTOCOL | protid |
					 OP_PCL_DKP_SRC_IMM |
					 OP_PCL_DKP_DST_IMM | adata->keylen);
			append_data(desc, adata->key_virt, adata->keylen);

			words = (ALIGN(adata->keylen_pad, CAAM_CMD_SZ) -
				 ALIGN(adata->keylen, CAAM_CMD_SZ)) /
				CAAM_CMD_SZ;
		}

		/* Reserve space in descriptor buffer for the derived key */
		if (words)
			(*desc) = cpu_to_caam32(caam32_to_cpu(*desc) + words);
	} else {
		append_operation(desc, OP_TYPE_UNI_PROTOCOL | protid |
				 OP_PCL_DKP_SRC_PTR | OP_PCL_DKP_DST_PTR |
				 adata->keylen);
		append_ptr(desc, adata->key_dma);
	}
}

#endif /* DESC_CONSTR_H */
