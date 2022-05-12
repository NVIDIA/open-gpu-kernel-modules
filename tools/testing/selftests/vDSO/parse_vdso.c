/*
 * parse_vdso.c: Linux reference vDSO parser
 * Written by Andrew Lutomirski, 2011-2014.
 *
 * This code is meant to be linked in to various programs that run on Linux.
 * As such, it is available with as few restrictions as possible.  This file
 * is licensed under the Creative Commons Zero License, version 1.0,
 * available at http://creativecommons.org/publicdomain/zero/1.0/legalcode
 *
 * The vDSO is a regular ELF DSO that the kernel maps into user space when
 * it starts a program.  It works equally well in statically and dynamically
 * linked binaries.
 *
 * This code is tested on x86.  In principle it should work on any
 * architecture that has a vDSO.
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include <elf.h>

#include "parse_vdso.h"

/* And here's the code. */
#ifndef ELF_BITS
# if ULONG_MAX > 0xffffffffUL
#  define ELF_BITS 64
# else
#  define ELF_BITS 32
# endif
#endif

#define ELF_BITS_XFORM2(bits, x) Elf##bits##_##x
#define ELF_BITS_XFORM(bits, x) ELF_BITS_XFORM2(bits, x)
#define ELF(x) ELF_BITS_XFORM(ELF_BITS, x)

static struct vdso_info
{
	bool valid;

	/* Load information */
	uintptr_t load_addr;
	uintptr_t load_offset;  /* load_addr - recorded vaddr */

	/* Symbol table */
	ELF(Sym) *symtab;
	const char *symstrings;
	ELF(Word) *bucket, *chain;
	ELF(Word) nbucket, nchain;

	/* Version table */
	ELF(Versym) *versym;
	ELF(Verdef) *verdef;
} vdso_info;

/* Straight from the ELF specification. */
static unsigned long elf_hash(const unsigned char *name)
{
	unsigned long h = 0, g;
	while (*name)
	{
		h = (h << 4) + *name++;
		if (g = h & 0xf0000000)
			h ^= g >> 24;
		h &= ~g;
	}
	return h;
}

void vdso_init_from_sysinfo_ehdr(uintptr_t base)
{
	size_t i;
	bool found_vaddr = false;

	vdso_info.valid = false;

	vdso_info.load_addr = base;

	ELF(Ehdr) *hdr = (ELF(Ehdr)*)base;
	if (hdr->e_ident[EI_CLASS] !=
	    (ELF_BITS == 32 ? ELFCLASS32 : ELFCLASS64)) {
		return;  /* Wrong ELF class -- check ELF_BITS */
	}

	ELF(Phdr) *pt = (ELF(Phdr)*)(vdso_info.load_addr + hdr->e_phoff);
	ELF(Dyn) *dyn = 0;

	/*
	 * We need two things from the segment table: the load offset
	 * and the dynamic table.
	 */
	for (i = 0; i < hdr->e_phnum; i++)
	{
		if (pt[i].p_type == PT_LOAD && !found_vaddr) {
			found_vaddr = true;
			vdso_info.load_offset =	base
				+ (uintptr_t)pt[i].p_offset
				- (uintptr_t)pt[i].p_vaddr;
		} else if (pt[i].p_type == PT_DYNAMIC) {
			dyn = (ELF(Dyn)*)(base + pt[i].p_offset);
		}
	}

	if (!found_vaddr || !dyn)
		return;  /* Failed */

	/*
	 * Fish out the useful bits of the dynamic table.
	 */
	ELF(Word) *hash = 0;
	vdso_info.symstrings = 0;
	vdso_info.symtab = 0;
	vdso_info.versym = 0;
	vdso_info.verdef = 0;
	for (i = 0; dyn[i].d_tag != DT_NULL; i++) {
		switch (dyn[i].d_tag) {
		case DT_STRTAB:
			vdso_info.symstrings = (const char *)
				((uintptr_t)dyn[i].d_un.d_ptr
				 + vdso_info.load_offset);
			break;
		case DT_SYMTAB:
			vdso_info.symtab = (ELF(Sym) *)
				((uintptr_t)dyn[i].d_un.d_ptr
				 + vdso_info.load_offset);
			break;
		case DT_HASH:
			hash = (ELF(Word) *)
				((uintptr_t)dyn[i].d_un.d_ptr
				 + vdso_info.load_offset);
			break;
		case DT_VERSYM:
			vdso_info.versym = (ELF(Versym) *)
				((uintptr_t)dyn[i].d_un.d_ptr
				 + vdso_info.load_offset);
			break;
		case DT_VERDEF:
			vdso_info.verdef = (ELF(Verdef) *)
				((uintptr_t)dyn[i].d_un.d_ptr
				 + vdso_info.load_offset);
			break;
		}
	}
	if (!vdso_info.symstrings || !vdso_info.symtab || !hash)
		return;  /* Failed */

	if (!vdso_info.verdef)
		vdso_info.versym = 0;

	/* Parse the hash table header. */
	vdso_info.nbucket = hash[0];
	vdso_info.nchain = hash[1];
	vdso_info.bucket = &hash[2];
	vdso_info.chain = &hash[vdso_info.nbucket + 2];

	/* That's all we need. */
	vdso_info.valid = true;
}

static bool vdso_match_version(ELF(Versym) ver,
			       const char *name, ELF(Word) hash)
{
	/*
	 * This is a helper function to check if the version indexed by
	 * ver matches name (which hashes to hash).
	 *
	 * The version definition table is a mess, and I don't know how
	 * to do this in better than linear time without allocating memory
	 * to build an index.  I also don't know why the table has
	 * variable size entries in the first place.
	 *
	 * For added fun, I can't find a comprehensible specification of how
	 * to parse all the weird flags in the table.
	 *
	 * So I just parse the whole table every time.
	 */

	/* First step: find the version definition */
	ver &= 0x7fff;  /* Apparently bit 15 means "hidden" */
	ELF(Verdef) *def = vdso_info.verdef;
	while(true) {
		if ((def->vd_flags & VER_FLG_BASE) == 0
		    && (def->vd_ndx & 0x7fff) == ver)
			break;

		if (def->vd_next == 0)
			return false;  /* No definition. */

		def = (ELF(Verdef) *)((char *)def + def->vd_next);
	}

	/* Now figure out whether it matches. */
	ELF(Verdaux) *aux = (ELF(Verdaux)*)((char *)def + def->vd_aux);
	return def->vd_hash == hash
		&& !strcmp(name, vdso_info.symstrings + aux->vda_name);
}

void *vdso_sym(const char *version, const char *name)
{
	unsigned long ver_hash;
	if (!vdso_info.valid)
		return 0;

	ver_hash = elf_hash(version);
	ELF(Word) chain = vdso_info.bucket[elf_hash(name) % vdso_info.nbucket];

	for (; chain != STN_UNDEF; chain = vdso_info.chain[chain]) {
		ELF(Sym) *sym = &vdso_info.symtab[chain];

		/* Check for a defined global or weak function w/ right name. */
		if (ELF64_ST_TYPE(sym->st_info) != STT_FUNC)
			continue;
		if (ELF64_ST_BIND(sym->st_info) != STB_GLOBAL &&
		    ELF64_ST_BIND(sym->st_info) != STB_WEAK)
			continue;
		if (sym->st_shndx == SHN_UNDEF)
			continue;
		if (strcmp(name, vdso_info.symstrings + sym->st_name))
			continue;

		/* Check symbol version. */
		if (vdso_info.versym
		    && !vdso_match_version(vdso_info.versym[chain],
					   version, ver_hash))
			continue;

		return (void *)(vdso_info.load_offset + sym->st_value);
	}

	return 0;
}

void vdso_init_from_auxv(void *auxv)
{
	ELF(auxv_t) *elf_auxv = auxv;
	for (int i = 0; elf_auxv[i].a_type != AT_NULL; i++)
	{
		if (elf_auxv[i].a_type == AT_SYSINFO_EHDR) {
			vdso_init_from_sysinfo_ehdr(elf_auxv[i].a_un.a_val);
			return;
		}
	}

	vdso_info.valid = false;
}
