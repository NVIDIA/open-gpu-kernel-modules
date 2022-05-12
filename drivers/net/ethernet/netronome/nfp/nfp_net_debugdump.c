// SPDX-License-Identifier: (GPL-2.0-only OR BSD-2-Clause)
/* Copyright (C) 2017-2018 Netronome Systems, Inc. */

#include <linux/ethtool.h>
#include <linux/vmalloc.h>

#include "nfp_asm.h"
#include "nfp_main.h"
#include "nfpcore/nfp.h"
#include "nfpcore/nfp_nffw.h"
#include "nfpcore/nfp6000/nfp6000.h"

#define NFP_DUMP_SPEC_RTSYM	"_abi_dump_spec"

#define ALIGN8(x)	ALIGN(x, 8)

enum nfp_dumpspec_type {
	NFP_DUMPSPEC_TYPE_CPP_CSR = 0,
	NFP_DUMPSPEC_TYPE_XPB_CSR = 1,
	NFP_DUMPSPEC_TYPE_ME_CSR = 2,
	NFP_DUMPSPEC_TYPE_INDIRECT_ME_CSR = 3,
	NFP_DUMPSPEC_TYPE_RTSYM = 4,
	NFP_DUMPSPEC_TYPE_HWINFO = 5,
	NFP_DUMPSPEC_TYPE_FWNAME = 6,
	NFP_DUMPSPEC_TYPE_HWINFO_FIELD = 7,
	NFP_DUMPSPEC_TYPE_PROLOG = 10000,
	NFP_DUMPSPEC_TYPE_ERROR = 10001,
};

/* The following structs must be carefully aligned so that they can be used to
 * interpret the binary dumpspec and populate the dump data in a deterministic
 * way.
 */

/* generic type plus length */
struct nfp_dump_tl {
	__be32 type;
	__be32 length;	/* chunk length to follow, aligned to 8 bytes */
	char data[];
};

/* NFP CPP parameters */
struct nfp_dumpspec_cpp_isl_id {
	u8 target;
	u8 action;
	u8 token;
	u8 island;
};

struct nfp_dump_common_cpp {
	struct nfp_dumpspec_cpp_isl_id cpp_id;
	__be32 offset;		/* address to start dump */
	__be32 dump_length;	/* total bytes to dump, aligned to reg size */
};

/* CSR dumpables */
struct nfp_dumpspec_csr {
	struct nfp_dump_tl tl;
	struct nfp_dump_common_cpp cpp;
	__be32 register_width;	/* in bits */
};

struct nfp_dumpspec_rtsym {
	struct nfp_dump_tl tl;
	char rtsym[];
};

/* header for register dumpable */
struct nfp_dump_csr {
	struct nfp_dump_tl tl;
	struct nfp_dump_common_cpp cpp;
	__be32 register_width;	/* in bits */
	__be32 error;		/* error code encountered while reading */
	__be32 error_offset;	/* offset being read when error occurred */
};

struct nfp_dump_rtsym {
	struct nfp_dump_tl tl;
	struct nfp_dump_common_cpp cpp;
	__be32 error;		/* error code encountered while reading */
	u8 padded_name_length;	/* pad so data starts at 8 byte boundary */
	char rtsym[];
	/* after padded_name_length, there is dump_length data */
};

struct nfp_dump_prolog {
	struct nfp_dump_tl tl;
	__be32 dump_level;
};

struct nfp_dump_error {
	struct nfp_dump_tl tl;
	__be32 error;
	char padding[4];
	char spec[];
};

/* to track state through debug size calculation TLV traversal */
struct nfp_level_size {
	__be32 requested_level;	/* input */
	u32 total_size;		/* output */
};

/* to track state during debug dump creation TLV traversal */
struct nfp_dump_state {
	__be32 requested_level;	/* input param */
	u32 dumped_size;	/* adds up to size of dumped data */
	u32 buf_size;		/* size of buffer pointer to by p */
	void *p;		/* current point in dump buffer */
};

typedef int (*nfp_tlv_visit)(struct nfp_pf *pf, struct nfp_dump_tl *tl,
			     void *param);

static int
nfp_traverse_tlvs(struct nfp_pf *pf, void *data, u32 data_length, void *param,
		  nfp_tlv_visit tlv_visit)
{
	long long remaining = data_length;
	struct nfp_dump_tl *tl;
	u32 total_tlv_size;
	void *p = data;
	int err;

	while (remaining >= sizeof(*tl)) {
		tl = p;
		if (!tl->type && !tl->length)
			break;

		if (be32_to_cpu(tl->length) > remaining - sizeof(*tl))
			return -EINVAL;

		total_tlv_size = sizeof(*tl) + be32_to_cpu(tl->length);

		/* Spec TLVs should be aligned to 4 bytes. */
		if (total_tlv_size % 4 != 0)
			return -EINVAL;

		p += total_tlv_size;
		remaining -= total_tlv_size;
		err = tlv_visit(pf, tl, param);
		if (err)
			return err;
	}

	return 0;
}

static u32 nfp_get_numeric_cpp_id(struct nfp_dumpspec_cpp_isl_id *cpp_id)
{
	return NFP_CPP_ISLAND_ID(cpp_id->target, cpp_id->action, cpp_id->token,
				 cpp_id->island);
}

struct nfp_dumpspec *
nfp_net_dump_load_dumpspec(struct nfp_cpp *cpp, struct nfp_rtsym_table *rtbl)
{
	const struct nfp_rtsym *specsym;
	struct nfp_dumpspec *dumpspec;
	int bytes_read;
	u64 sym_size;

	specsym = nfp_rtsym_lookup(rtbl, NFP_DUMP_SPEC_RTSYM);
	if (!specsym)
		return NULL;
	sym_size = nfp_rtsym_size(specsym);

	/* expected size of this buffer is in the order of tens of kilobytes */
	dumpspec = vmalloc(sizeof(*dumpspec) + sym_size);
	if (!dumpspec)
		return NULL;
	dumpspec->size = sym_size;

	bytes_read = nfp_rtsym_read(cpp, specsym, 0, dumpspec->data, sym_size);
	if (bytes_read != sym_size) {
		vfree(dumpspec);
		nfp_warn(cpp, "Debug dump specification read failed.\n");
		return NULL;
	}

	return dumpspec;
}

static int nfp_dump_error_tlv_size(struct nfp_dump_tl *spec)
{
	return ALIGN8(sizeof(struct nfp_dump_error) + sizeof(*spec) +
		      be32_to_cpu(spec->length));
}

static int nfp_calc_fwname_tlv_size(struct nfp_pf *pf)
{
	u32 fwname_len = strlen(nfp_mip_name(pf->mip));

	return sizeof(struct nfp_dump_tl) + ALIGN8(fwname_len + 1);
}

static int nfp_calc_hwinfo_field_sz(struct nfp_pf *pf, struct nfp_dump_tl *spec)
{
	u32 tl_len, key_len;
	const char *value;

	tl_len = be32_to_cpu(spec->length);
	key_len = strnlen(spec->data, tl_len);
	if (key_len == tl_len)
		return nfp_dump_error_tlv_size(spec);

	value = nfp_hwinfo_lookup(pf->hwinfo, spec->data);
	if (!value)
		return nfp_dump_error_tlv_size(spec);

	return sizeof(struct nfp_dump_tl) + ALIGN8(key_len + strlen(value) + 2);
}

static bool nfp_csr_spec_valid(struct nfp_dumpspec_csr *spec_csr)
{
	u32 required_read_sz = sizeof(*spec_csr) - sizeof(spec_csr->tl);
	u32 available_sz = be32_to_cpu(spec_csr->tl.length);
	u32 reg_width;

	if (available_sz < required_read_sz)
		return false;

	reg_width = be32_to_cpu(spec_csr->register_width);

	return reg_width == 32 || reg_width == 64;
}

static int
nfp_calc_rtsym_dump_sz(struct nfp_pf *pf, struct nfp_dump_tl *spec)
{
	struct nfp_rtsym_table *rtbl = pf->rtbl;
	struct nfp_dumpspec_rtsym *spec_rtsym;
	const struct nfp_rtsym *sym;
	u32 tl_len, key_len;

	spec_rtsym = (struct nfp_dumpspec_rtsym *)spec;
	tl_len = be32_to_cpu(spec->length);
	key_len = strnlen(spec_rtsym->rtsym, tl_len);
	if (key_len == tl_len)
		return nfp_dump_error_tlv_size(spec);

	sym = nfp_rtsym_lookup(rtbl, spec_rtsym->rtsym);
	if (!sym)
		return nfp_dump_error_tlv_size(spec);

	return ALIGN8(offsetof(struct nfp_dump_rtsym, rtsym) + key_len + 1) +
	       ALIGN8(nfp_rtsym_size(sym));
}

static int
nfp_add_tlv_size(struct nfp_pf *pf, struct nfp_dump_tl *tl, void *param)
{
	struct nfp_dumpspec_csr *spec_csr;
	u32 *size = param;
	u32 hwinfo_size;

	switch (be32_to_cpu(tl->type)) {
	case NFP_DUMPSPEC_TYPE_FWNAME:
		*size += nfp_calc_fwname_tlv_size(pf);
		break;
	case NFP_DUMPSPEC_TYPE_CPP_CSR:
	case NFP_DUMPSPEC_TYPE_XPB_CSR:
	case NFP_DUMPSPEC_TYPE_ME_CSR:
		spec_csr = (struct nfp_dumpspec_csr *)tl;
		if (!nfp_csr_spec_valid(spec_csr))
			*size += nfp_dump_error_tlv_size(tl);
		else
			*size += ALIGN8(sizeof(struct nfp_dump_csr)) +
				 ALIGN8(be32_to_cpu(spec_csr->cpp.dump_length));
		break;
	case NFP_DUMPSPEC_TYPE_INDIRECT_ME_CSR:
		spec_csr = (struct nfp_dumpspec_csr *)tl;
		if (!nfp_csr_spec_valid(spec_csr))
			*size += nfp_dump_error_tlv_size(tl);
		else
			*size += ALIGN8(sizeof(struct nfp_dump_csr)) +
				 ALIGN8(be32_to_cpu(spec_csr->cpp.dump_length) *
					NFP_IND_NUM_CONTEXTS);
		break;
	case NFP_DUMPSPEC_TYPE_RTSYM:
		*size += nfp_calc_rtsym_dump_sz(pf, tl);
		break;
	case NFP_DUMPSPEC_TYPE_HWINFO:
		hwinfo_size = nfp_hwinfo_get_packed_str_size(pf->hwinfo);
		*size += sizeof(struct nfp_dump_tl) + ALIGN8(hwinfo_size);
		break;
	case NFP_DUMPSPEC_TYPE_HWINFO_FIELD:
		*size += nfp_calc_hwinfo_field_sz(pf, tl);
		break;
	default:
		*size += nfp_dump_error_tlv_size(tl);
		break;
	}

	return 0;
}

static int
nfp_calc_specific_level_size(struct nfp_pf *pf, struct nfp_dump_tl *dump_level,
			     void *param)
{
	struct nfp_level_size *lev_sz = param;

	if (dump_level->type != lev_sz->requested_level)
		return 0;

	return nfp_traverse_tlvs(pf, dump_level->data,
				 be32_to_cpu(dump_level->length),
				 &lev_sz->total_size, nfp_add_tlv_size);
}

s64 nfp_net_dump_calculate_size(struct nfp_pf *pf, struct nfp_dumpspec *spec,
				u32 flag)
{
	struct nfp_level_size lev_sz;
	int err;

	lev_sz.requested_level = cpu_to_be32(flag);
	lev_sz.total_size = ALIGN8(sizeof(struct nfp_dump_prolog));

	err = nfp_traverse_tlvs(pf, spec->data, spec->size, &lev_sz,
				nfp_calc_specific_level_size);
	if (err)
		return err;

	return lev_sz.total_size;
}

static int nfp_add_tlv(u32 type, u32 total_tlv_sz, struct nfp_dump_state *dump)
{
	struct nfp_dump_tl *tl = dump->p;

	if (total_tlv_sz > dump->buf_size)
		return -ENOSPC;

	if (dump->buf_size - total_tlv_sz < dump->dumped_size)
		return -ENOSPC;

	tl->type = cpu_to_be32(type);
	tl->length = cpu_to_be32(total_tlv_sz - sizeof(*tl));

	dump->dumped_size += total_tlv_sz;
	dump->p += total_tlv_sz;

	return 0;
}

static int
nfp_dump_error_tlv(struct nfp_dump_tl *spec, int error,
		   struct nfp_dump_state *dump)
{
	struct nfp_dump_error *dump_header = dump->p;
	u32 total_spec_size, total_size;
	int err;

	total_spec_size = sizeof(*spec) + be32_to_cpu(spec->length);
	total_size = ALIGN8(sizeof(*dump_header) + total_spec_size);

	err = nfp_add_tlv(NFP_DUMPSPEC_TYPE_ERROR, total_size, dump);
	if (err)
		return err;

	dump_header->error = cpu_to_be32(error);
	memcpy(dump_header->spec, spec, total_spec_size);

	return 0;
}

static int nfp_dump_fwname(struct nfp_pf *pf, struct nfp_dump_state *dump)
{
	struct nfp_dump_tl *dump_header = dump->p;
	u32 fwname_len, total_size;
	const char *fwname;
	int err;

	fwname = nfp_mip_name(pf->mip);
	fwname_len = strlen(fwname);
	total_size = sizeof(*dump_header) + ALIGN8(fwname_len + 1);

	err = nfp_add_tlv(NFP_DUMPSPEC_TYPE_FWNAME, total_size, dump);
	if (err)
		return err;

	memcpy(dump_header->data, fwname, fwname_len);

	return 0;
}

static int
nfp_dump_hwinfo(struct nfp_pf *pf, struct nfp_dump_tl *spec,
		struct nfp_dump_state *dump)
{
	struct nfp_dump_tl *dump_header = dump->p;
	u32 hwinfo_size, total_size;
	char *hwinfo;
	int err;

	hwinfo = nfp_hwinfo_get_packed_strings(pf->hwinfo);
	hwinfo_size = nfp_hwinfo_get_packed_str_size(pf->hwinfo);
	total_size = sizeof(*dump_header) + ALIGN8(hwinfo_size);

	err = nfp_add_tlv(NFP_DUMPSPEC_TYPE_HWINFO, total_size, dump);
	if (err)
		return err;

	memcpy(dump_header->data, hwinfo, hwinfo_size);

	return 0;
}

static int nfp_dump_hwinfo_field(struct nfp_pf *pf, struct nfp_dump_tl *spec,
				 struct nfp_dump_state *dump)
{
	struct nfp_dump_tl *dump_header = dump->p;
	u32 tl_len, key_len, val_len;
	const char *key, *value;
	u32 total_size;
	int err;

	tl_len = be32_to_cpu(spec->length);
	key_len = strnlen(spec->data, tl_len);
	if (key_len == tl_len)
		return nfp_dump_error_tlv(spec, -EINVAL, dump);

	key = spec->data;
	value = nfp_hwinfo_lookup(pf->hwinfo, key);
	if (!value)
		return nfp_dump_error_tlv(spec, -ENOENT, dump);

	val_len = strlen(value);
	total_size = sizeof(*dump_header) + ALIGN8(key_len + val_len + 2);
	err = nfp_add_tlv(NFP_DUMPSPEC_TYPE_HWINFO_FIELD, total_size, dump);
	if (err)
		return err;

	memcpy(dump_header->data, key, key_len + 1);
	memcpy(dump_header->data + key_len + 1, value, val_len + 1);

	return 0;
}

static bool is_xpb_read(struct nfp_dumpspec_cpp_isl_id *cpp_id)
{
	return cpp_id->target == NFP_CPP_TARGET_ISLAND_XPB &&
	       cpp_id->action == 0 && cpp_id->token == 0;
}

static int
nfp_dump_csr_range(struct nfp_pf *pf, struct nfp_dumpspec_csr *spec_csr,
		   struct nfp_dump_state *dump)
{
	struct nfp_dump_csr *dump_header = dump->p;
	u32 reg_sz, header_size, total_size;
	u32 cpp_rd_addr, max_rd_addr;
	int bytes_read;
	void *dest;
	u32 cpp_id;
	int err;

	if (!nfp_csr_spec_valid(spec_csr))
		return nfp_dump_error_tlv(&spec_csr->tl, -EINVAL, dump);

	reg_sz = be32_to_cpu(spec_csr->register_width) / BITS_PER_BYTE;
	header_size = ALIGN8(sizeof(*dump_header));
	total_size = header_size +
		     ALIGN8(be32_to_cpu(spec_csr->cpp.dump_length));
	dest = dump->p + header_size;

	err = nfp_add_tlv(be32_to_cpu(spec_csr->tl.type), total_size, dump);
	if (err)
		return err;

	dump_header->cpp = spec_csr->cpp;
	dump_header->register_width = spec_csr->register_width;

	cpp_id = nfp_get_numeric_cpp_id(&spec_csr->cpp.cpp_id);
	cpp_rd_addr = be32_to_cpu(spec_csr->cpp.offset);
	max_rd_addr = cpp_rd_addr + be32_to_cpu(spec_csr->cpp.dump_length);

	while (cpp_rd_addr < max_rd_addr) {
		if (is_xpb_read(&spec_csr->cpp.cpp_id)) {
			err = nfp_xpb_readl(pf->cpp, cpp_rd_addr, (u32 *)dest);
		} else {
			bytes_read = nfp_cpp_read(pf->cpp, cpp_id, cpp_rd_addr,
						  dest, reg_sz);
			err = bytes_read == reg_sz ? 0 : -EIO;
		}
		if (err) {
			dump_header->error = cpu_to_be32(err);
			dump_header->error_offset = cpu_to_be32(cpp_rd_addr);
			break;
		}
		cpp_rd_addr += reg_sz;
		dest += reg_sz;
	}

	return 0;
}

/* Write context to CSRCtxPtr, then read from it. Then the value can be read
 * from IndCtxStatus.
 */
static int
nfp_read_indirect_csr(struct nfp_cpp *cpp,
		      struct nfp_dumpspec_cpp_isl_id cpp_params, u32 offset,
		      u32 reg_sz, u32 context, void *dest)
{
	u32 csr_ctx_ptr_offs;
	u32 cpp_id;
	int result;

	csr_ctx_ptr_offs = nfp_get_ind_csr_ctx_ptr_offs(offset);
	cpp_id = NFP_CPP_ISLAND_ID(cpp_params.target,
				   NFP_IND_ME_REFL_WR_SIG_INIT,
				   cpp_params.token, cpp_params.island);
	result = nfp_cpp_writel(cpp, cpp_id, csr_ctx_ptr_offs, context);
	if (result)
		return result;

	cpp_id = nfp_get_numeric_cpp_id(&cpp_params);
	result = nfp_cpp_read(cpp, cpp_id, csr_ctx_ptr_offs, dest, reg_sz);
	if (result != reg_sz)
		return result < 0 ? result : -EIO;

	result = nfp_cpp_read(cpp, cpp_id, offset, dest, reg_sz);
	if (result != reg_sz)
		return result < 0 ? result : -EIO;

	return 0;
}

static int
nfp_read_all_indirect_csr_ctx(struct nfp_cpp *cpp,
			      struct nfp_dumpspec_csr *spec_csr, u32 address,
			      u32 reg_sz, void *dest)
{
	u32 ctx;
	int err;

	for (ctx = 0; ctx < NFP_IND_NUM_CONTEXTS; ctx++) {
		err = nfp_read_indirect_csr(cpp, spec_csr->cpp.cpp_id, address,
					    reg_sz, ctx, dest + ctx * reg_sz);
		if (err)
			return err;
	}

	return 0;
}

static int
nfp_dump_indirect_csr_range(struct nfp_pf *pf,
			    struct nfp_dumpspec_csr *spec_csr,
			    struct nfp_dump_state *dump)
{
	struct nfp_dump_csr *dump_header = dump->p;
	u32 reg_sz, header_size, total_size;
	u32 cpp_rd_addr, max_rd_addr;
	u32 reg_data_length;
	void *dest;
	int err;

	if (!nfp_csr_spec_valid(spec_csr))
		return nfp_dump_error_tlv(&spec_csr->tl, -EINVAL, dump);

	reg_sz = be32_to_cpu(spec_csr->register_width) / BITS_PER_BYTE;
	header_size = ALIGN8(sizeof(*dump_header));
	reg_data_length = be32_to_cpu(spec_csr->cpp.dump_length) *
			  NFP_IND_NUM_CONTEXTS;
	total_size = header_size + ALIGN8(reg_data_length);
	dest = dump->p + header_size;

	err = nfp_add_tlv(be32_to_cpu(spec_csr->tl.type), total_size, dump);
	if (err)
		return err;

	dump_header->cpp = spec_csr->cpp;
	dump_header->register_width = spec_csr->register_width;

	cpp_rd_addr = be32_to_cpu(spec_csr->cpp.offset);
	max_rd_addr = cpp_rd_addr + be32_to_cpu(spec_csr->cpp.dump_length);
	while (cpp_rd_addr < max_rd_addr) {
		err = nfp_read_all_indirect_csr_ctx(pf->cpp, spec_csr,
						    cpp_rd_addr, reg_sz, dest);
		if (err) {
			dump_header->error = cpu_to_be32(err);
			dump_header->error_offset = cpu_to_be32(cpp_rd_addr);
			break;
		}
		cpp_rd_addr += reg_sz;
		dest += reg_sz * NFP_IND_NUM_CONTEXTS;
	}

	return 0;
}

static int
nfp_dump_single_rtsym(struct nfp_pf *pf, struct nfp_dumpspec_rtsym *spec,
		      struct nfp_dump_state *dump)
{
	struct nfp_dump_rtsym *dump_header = dump->p;
	struct nfp_dumpspec_cpp_isl_id cpp_params;
	struct nfp_rtsym_table *rtbl = pf->rtbl;
	u32 header_size, total_size, sym_size;
	const struct nfp_rtsym *sym;
	u32 tl_len, key_len;
	int bytes_read;
	void *dest;
	int err;

	tl_len = be32_to_cpu(spec->tl.length);
	key_len = strnlen(spec->rtsym, tl_len);
	if (key_len == tl_len)
		return nfp_dump_error_tlv(&spec->tl, -EINVAL, dump);

	sym = nfp_rtsym_lookup(rtbl, spec->rtsym);
	if (!sym)
		return nfp_dump_error_tlv(&spec->tl, -ENOENT, dump);

	sym_size = nfp_rtsym_size(sym);
	header_size =
		ALIGN8(offsetof(struct nfp_dump_rtsym, rtsym) + key_len + 1);
	total_size = header_size + ALIGN8(sym_size);
	dest = dump->p + header_size;

	err = nfp_add_tlv(be32_to_cpu(spec->tl.type), total_size, dump);
	if (err)
		return err;

	dump_header->padded_name_length =
		header_size - offsetof(struct nfp_dump_rtsym, rtsym);
	memcpy(dump_header->rtsym, spec->rtsym, key_len + 1);
	dump_header->cpp.dump_length = cpu_to_be32(sym_size);

	if (sym->type != NFP_RTSYM_TYPE_ABS) {
		cpp_params.target = sym->target;
		cpp_params.action = NFP_CPP_ACTION_RW;
		cpp_params.token  = 0;
		cpp_params.island = sym->domain;
		dump_header->cpp.cpp_id = cpp_params;
		dump_header->cpp.offset = cpu_to_be32(sym->addr);
	}

	bytes_read = nfp_rtsym_read(pf->cpp, sym, 0, dest, sym_size);
	if (bytes_read != sym_size) {
		if (bytes_read >= 0)
			bytes_read = -EIO;
		dump_header->error = cpu_to_be32(bytes_read);
	}

	return 0;
}

static int
nfp_dump_for_tlv(struct nfp_pf *pf, struct nfp_dump_tl *tl, void *param)
{
	struct nfp_dumpspec_rtsym *spec_rtsym;
	struct nfp_dump_state *dump = param;
	struct nfp_dumpspec_csr *spec_csr;
	int err;

	switch (be32_to_cpu(tl->type)) {
	case NFP_DUMPSPEC_TYPE_FWNAME:
		err = nfp_dump_fwname(pf, dump);
		if (err)
			return err;
		break;
	case NFP_DUMPSPEC_TYPE_CPP_CSR:
	case NFP_DUMPSPEC_TYPE_XPB_CSR:
	case NFP_DUMPSPEC_TYPE_ME_CSR:
		spec_csr = (struct nfp_dumpspec_csr *)tl;
		err = nfp_dump_csr_range(pf, spec_csr, dump);
		if (err)
			return err;
		break;
	case NFP_DUMPSPEC_TYPE_INDIRECT_ME_CSR:
		spec_csr = (struct nfp_dumpspec_csr *)tl;
		err = nfp_dump_indirect_csr_range(pf, spec_csr, dump);
		if (err)
			return err;
		break;
	case NFP_DUMPSPEC_TYPE_RTSYM:
		spec_rtsym = (struct nfp_dumpspec_rtsym *)tl;
		err = nfp_dump_single_rtsym(pf, spec_rtsym, dump);
		if (err)
			return err;
		break;
	case NFP_DUMPSPEC_TYPE_HWINFO:
		err = nfp_dump_hwinfo(pf, tl, dump);
		if (err)
			return err;
		break;
	case NFP_DUMPSPEC_TYPE_HWINFO_FIELD:
		err = nfp_dump_hwinfo_field(pf, tl, dump);
		if (err)
			return err;
		break;
	default:
		err = nfp_dump_error_tlv(tl, -EOPNOTSUPP, dump);
		if (err)
			return err;
	}

	return 0;
}

static int
nfp_dump_specific_level(struct nfp_pf *pf, struct nfp_dump_tl *dump_level,
			void *param)
{
	struct nfp_dump_state *dump = param;

	if (dump_level->type != dump->requested_level)
		return 0;

	return nfp_traverse_tlvs(pf, dump_level->data,
				 be32_to_cpu(dump_level->length), dump,
				 nfp_dump_for_tlv);
}

static int nfp_dump_populate_prolog(struct nfp_dump_state *dump)
{
	struct nfp_dump_prolog *prolog = dump->p;
	u32 total_size;
	int err;

	total_size = ALIGN8(sizeof(*prolog));

	err = nfp_add_tlv(NFP_DUMPSPEC_TYPE_PROLOG, total_size, dump);
	if (err)
		return err;

	prolog->dump_level = dump->requested_level;

	return 0;
}

int nfp_net_dump_populate_buffer(struct nfp_pf *pf, struct nfp_dumpspec *spec,
				 struct ethtool_dump *dump_param, void *dest)
{
	struct nfp_dump_state dump;
	int err;

	dump.requested_level = cpu_to_be32(dump_param->flag);
	dump.dumped_size = 0;
	dump.p = dest;
	dump.buf_size = dump_param->len;

	err = nfp_dump_populate_prolog(&dump);
	if (err)
		return err;

	err = nfp_traverse_tlvs(pf, spec->data, spec->size, &dump,
				nfp_dump_specific_level);
	if (err)
		return err;

	/* Set size of actual dump, to trigger warning if different from
	 * calculated size.
	 */
	dump_param->len = dump.dumped_size;

	return 0;
}
