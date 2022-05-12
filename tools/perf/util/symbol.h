/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __PERF_SYMBOL
#define __PERF_SYMBOL 1

#include <linux/types.h>
#include <linux/refcount.h>
#include <stdbool.h>
#include <stdint.h>
#include <linux/list.h>
#include <linux/rbtree.h>
#include <stdio.h>
#include "path.h"
#include "symbol_conf.h"
#include "spark.h"

#ifdef HAVE_LIBELF_SUPPORT
#include <libelf.h>
#include <gelf.h>
#endif
#include <elf.h>

struct dso;
struct map;
struct maps;
struct option;
struct build_id;

/*
 * libelf 0.8.x and earlier do not support ELF_C_READ_MMAP;
 * for newer versions we can use mmap to reduce memory usage:
 */
#ifdef ELF_C_READ_MMAP
# define PERF_ELF_C_READ_MMAP ELF_C_READ_MMAP
#else
# define PERF_ELF_C_READ_MMAP ELF_C_READ
#endif

#ifdef HAVE_LIBELF_SUPPORT
Elf_Scn *elf_section_by_name(Elf *elf, GElf_Ehdr *ep,
			     GElf_Shdr *shp, const char *name, size_t *idx);
#endif

/** struct symbol - symtab entry
 *
 * @ignore - resolvable but tools ignore it (e.g. idle routines)
 */
struct symbol {
	struct rb_node	rb_node;
	u64		start;
	u64		end;
	u16		namelen;
	u8		type:4;
	u8		binding:4;
	u8		idle:1;
	u8		ignore:1;
	u8		inlined:1;
	u8		arch_sym;
	bool		annotate2;
	char		name[];
};

void symbol__delete(struct symbol *sym);
void symbols__delete(struct rb_root_cached *symbols);

/* symbols__for_each_entry - iterate over symbols (rb_root)
 *
 * @symbols: the rb_root of symbols
 * @pos: the 'struct symbol *' to use as a loop cursor
 * @nd: the 'struct rb_node *' to use as a temporary storage
 */
#define symbols__for_each_entry(symbols, pos, nd)			\
	for (nd = rb_first_cached(symbols);					\
	     nd && (pos = rb_entry(nd, struct symbol, rb_node));	\
	     nd = rb_next(nd))

static inline size_t symbol__size(const struct symbol *sym)
{
	return sym->end - sym->start;
}

struct strlist;
struct intlist;

struct symbol_name_rb_node {
	struct rb_node	rb_node;
	struct symbol	sym;
};

static inline int __symbol__join_symfs(char *bf, size_t size, const char *path)
{
	return path__join(bf, size, symbol_conf.symfs, path);
}

#define symbol__join_symfs(bf, path) __symbol__join_symfs(bf, sizeof(bf), path)

extern int vmlinux_path__nr_entries;
extern char **vmlinux_path;

static inline void *symbol__priv(struct symbol *sym)
{
	return ((void *)sym) - symbol_conf.priv_size;
}

struct ref_reloc_sym {
	const char	*name;
	u64		addr;
	u64		unrelocated_addr;
};

struct addr_location {
	struct thread *thread;
	struct maps   *maps;
	struct map    *map;
	struct symbol *sym;
	const char    *srcline;
	u64	      addr;
	char	      level;
	u8	      filtered;
	u8	      cpumode;
	s32	      cpu;
	s32	      socket;
};

int dso__load(struct dso *dso, struct map *map);
int dso__load_vmlinux(struct dso *dso, struct map *map,
		      const char *vmlinux, bool vmlinux_allocated);
int dso__load_vmlinux_path(struct dso *dso, struct map *map);
int __dso__load_kallsyms(struct dso *dso, const char *filename, struct map *map,
			 bool no_kcore);
int dso__load_kallsyms(struct dso *dso, const char *filename, struct map *map);

void dso__insert_symbol(struct dso *dso,
			struct symbol *sym);
void dso__delete_symbol(struct dso *dso,
			struct symbol *sym);

struct symbol *dso__find_symbol(struct dso *dso, u64 addr);
struct symbol *dso__find_symbol_by_name(struct dso *dso, const char *name);

struct symbol *symbol__next_by_name(struct symbol *sym);

struct symbol *dso__first_symbol(struct dso *dso);
struct symbol *dso__last_symbol(struct dso *dso);
struct symbol *dso__next_symbol(struct symbol *sym);

enum dso_type dso__type_fd(int fd);

int filename__read_build_id(const char *filename, struct build_id *id);
int sysfs__read_build_id(const char *filename, struct build_id *bid);
int modules__parse(const char *filename, void *arg,
		   int (*process_module)(void *arg, const char *name,
					 u64 start, u64 size));
int filename__read_debuglink(const char *filename, char *debuglink,
			     size_t size);

struct perf_env;
int symbol__init(struct perf_env *env);
void symbol__exit(void);
void symbol__elf_init(void);
int symbol__annotation_init(void);

struct symbol *symbol__new(u64 start, u64 len, u8 binding, u8 type, const char *name);
size_t __symbol__fprintf_symname_offs(const struct symbol *sym,
				      const struct addr_location *al,
				      bool unknown_as_addr,
				      bool print_offsets, FILE *fp);
size_t symbol__fprintf_symname_offs(const struct symbol *sym,
				    const struct addr_location *al, FILE *fp);
size_t __symbol__fprintf_symname(const struct symbol *sym,
				 const struct addr_location *al,
				 bool unknown_as_addr, FILE *fp);
size_t symbol__fprintf_symname(const struct symbol *sym, FILE *fp);
size_t symbol__fprintf(struct symbol *sym, FILE *fp);
bool symbol__restricted_filename(const char *filename,
				 const char *restricted_filename);
int symbol__config_symfs(const struct option *opt __maybe_unused,
			 const char *dir, int unset __maybe_unused);

struct symsrc;

#ifdef HAVE_LIBBFD_SUPPORT
int dso__load_bfd_symbols(struct dso *dso, const char *debugfile);
#endif

int dso__load_sym(struct dso *dso, struct map *map, struct symsrc *syms_ss,
		  struct symsrc *runtime_ss, int kmodule);
int dso__synthesize_plt_symbols(struct dso *dso, struct symsrc *ss);

char *dso__demangle_sym(struct dso *dso, int kmodule, const char *elf_name);

void __symbols__insert(struct rb_root_cached *symbols, struct symbol *sym,
		       bool kernel);
void symbols__insert(struct rb_root_cached *symbols, struct symbol *sym);
void symbols__fixup_duplicate(struct rb_root_cached *symbols);
void symbols__fixup_end(struct rb_root_cached *symbols);
void maps__fixup_end(struct maps *maps);

typedef int (*mapfn_t)(u64 start, u64 len, u64 pgoff, void *data);
int file__read_maps(int fd, bool exe, mapfn_t mapfn, void *data,
		    bool *is_64_bit);

#define PERF_KCORE_EXTRACT "/tmp/perf-kcore-XXXXXX"

struct kcore_extract {
	char *kcore_filename;
	u64 addr;
	u64 offs;
	u64 len;
	char extract_filename[sizeof(PERF_KCORE_EXTRACT)];
	int fd;
};

int kcore_extract__create(struct kcore_extract *kce);
void kcore_extract__delete(struct kcore_extract *kce);

int kcore_copy(const char *from_dir, const char *to_dir);
int compare_proc_modules(const char *from, const char *to);

int setup_list(struct strlist **list, const char *list_str,
	       const char *list_name);
int setup_intlist(struct intlist **list, const char *list_str,
		  const char *list_name);

#ifdef HAVE_LIBELF_SUPPORT
bool elf__needs_adjust_symbols(GElf_Ehdr ehdr);
void arch__sym_update(struct symbol *s, GElf_Sym *sym);
#endif

const char *arch__normalize_symbol_name(const char *name);
#define SYMBOL_A 0
#define SYMBOL_B 1

void arch__symbols__fixup_end(struct symbol *p, struct symbol *c);
int arch__compare_symbol_names(const char *namea, const char *nameb);
int arch__compare_symbol_names_n(const char *namea, const char *nameb,
				 unsigned int n);
int arch__choose_best_symbol(struct symbol *syma, struct symbol *symb);

enum symbol_tag_include {
	SYMBOL_TAG_INCLUDE__NONE = 0,
	SYMBOL_TAG_INCLUDE__DEFAULT_ONLY
};

int symbol__match_symbol_name(const char *namea, const char *nameb,
			      enum symbol_tag_include includes);

/* structure containing an SDT note's info */
struct sdt_note {
	char *name;			/* name of the note*/
	char *provider;			/* provider name */
	char *args;
	bool bit32;			/* whether the location is 32 bits? */
	union {				/* location, base and semaphore addrs */
		Elf64_Addr a64[3];
		Elf32_Addr a32[3];
	} addr;
	struct list_head note_list;	/* SDT notes' list */
};

int get_sdt_note_list(struct list_head *head, const char *target);
int cleanup_sdt_note_list(struct list_head *sdt_notes);
int sdt_notes__get_count(struct list_head *start);

#define SDT_PROBES_SCN ".probes"
#define SDT_BASE_SCN ".stapsdt.base"
#define SDT_NOTE_SCN  ".note.stapsdt"
#define SDT_NOTE_TYPE 3
#define SDT_NOTE_NAME "stapsdt"
#define NR_ADDR 3

enum {
	SDT_NOTE_IDX_LOC = 0,
	SDT_NOTE_IDX_BASE,
	SDT_NOTE_IDX_REFCTR,
};

struct mem_info *mem_info__new(void);
struct mem_info *mem_info__get(struct mem_info *mi);
void   mem_info__put(struct mem_info *mi);

static inline void __mem_info__zput(struct mem_info **mi)
{
	mem_info__put(*mi);
	*mi = NULL;
}

#define mem_info__zput(mi) __mem_info__zput(&mi)

#endif /* __PERF_SYMBOL */
