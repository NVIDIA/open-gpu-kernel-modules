// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Procedures for interfacing to Open Firmware.
 *
 * Paul Mackerras	August 1996.
 * Copyright (C) 1996-2005 Paul Mackerras.
 * 
 *  Adapted for 64bit PowerPC by Dave Engebretsen and Peter Bergner.
 *    {engebret|bergner}@us.ibm.com 
 */

#undef DEBUG_PROM

/* we cannot use FORTIFY as it brings in new symbols */
#define __NO_FORTIFY

#include <stdarg.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/init.h>
#include <linux/threads.h>
#include <linux/spinlock.h>
#include <linux/types.h>
#include <linux/pci.h>
#include <linux/proc_fs.h>
#include <linux/delay.h>
#include <linux/initrd.h>
#include <linux/bitops.h>
#include <linux/pgtable.h>
#include <asm/prom.h>
#include <asm/rtas.h>
#include <asm/page.h>
#include <asm/processor.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <asm/smp.h>
#include <asm/mmu.h>
#include <asm/iommu.h>
#include <asm/btext.h>
#include <asm/sections.h>
#include <asm/machdep.h>
#include <asm/asm-prototypes.h>
#include <asm/ultravisor-api.h>

#include <linux/linux_logo.h>

/* All of prom_init bss lives here */
#define __prombss __section(".bss.prominit")

/*
 * Eventually bump that one up
 */
#define DEVTREE_CHUNK_SIZE	0x100000

/*
 * This is the size of the local memory reserve map that gets copied
 * into the boot params passed to the kernel. That size is totally
 * flexible as the kernel just reads the list until it encounters an
 * entry with size 0, so it can be changed without breaking binary
 * compatibility
 */
#define MEM_RESERVE_MAP_SIZE	8

/*
 * prom_init() is called very early on, before the kernel text
 * and data have been mapped to KERNELBASE.  At this point the code
 * is running at whatever address it has been loaded at.
 * On ppc32 we compile with -mrelocatable, which means that references
 * to extern and static variables get relocated automatically.
 * ppc64 objects are always relocatable, we just need to relocate the
 * TOC.
 *
 * Because OF may have mapped I/O devices into the area starting at
 * KERNELBASE, particularly on CHRP machines, we can't safely call
 * OF once the kernel has been mapped to KERNELBASE.  Therefore all
 * OF calls must be done within prom_init().
 *
 * ADDR is used in calls to call_prom.  The 4th and following
 * arguments to call_prom should be 32-bit values.
 * On ppc64, 64 bit values are truncated to 32 bits (and
 * fortunately don't get interpreted as two arguments).
 */
#define ADDR(x)		(u32)(unsigned long)(x)

#ifdef CONFIG_PPC64
#define OF_WORKAROUNDS	0
#else
#define OF_WORKAROUNDS	of_workarounds
static int of_workarounds __prombss;
#endif

#define OF_WA_CLAIM	1	/* do phys/virt claim separately, then map */
#define OF_WA_LONGTRAIL	2	/* work around longtrail bugs */

#define PROM_BUG() do {						\
        prom_printf("kernel BUG at %s line 0x%x!\n",		\
		    __FILE__, __LINE__);			\
	__builtin_trap();					\
} while (0)

#ifdef DEBUG_PROM
#define prom_debug(x...)	prom_printf(x)
#else
#define prom_debug(x...)	do { } while (0)
#endif


typedef u32 prom_arg_t;

struct prom_args {
        __be32 service;
        __be32 nargs;
        __be32 nret;
        __be32 args[10];
};

struct prom_t {
	ihandle root;
	phandle chosen;
	int cpu;
	ihandle stdout;
	ihandle mmumap;
	ihandle memory;
};

struct mem_map_entry {
	__be64	base;
	__be64	size;
};

typedef __be32 cell_t;

extern void __start(unsigned long r3, unsigned long r4, unsigned long r5,
		    unsigned long r6, unsigned long r7, unsigned long r8,
		    unsigned long r9);

#ifdef CONFIG_PPC64
extern int enter_prom(struct prom_args *args, unsigned long entry);
#else
static inline int enter_prom(struct prom_args *args, unsigned long entry)
{
	return ((int (*)(struct prom_args *))entry)(args);
}
#endif

extern void copy_and_flush(unsigned long dest, unsigned long src,
			   unsigned long size, unsigned long offset);

/* prom structure */
static struct prom_t __prombss prom;

static unsigned long __prombss prom_entry;

static char __prombss of_stdout_device[256];
static char __prombss prom_scratch[256];

static unsigned long __prombss dt_header_start;
static unsigned long __prombss dt_struct_start, dt_struct_end;
static unsigned long __prombss dt_string_start, dt_string_end;

static unsigned long __prombss prom_initrd_start, prom_initrd_end;

#ifdef CONFIG_PPC64
static int __prombss prom_iommu_force_on;
static int __prombss prom_iommu_off;
static unsigned long __prombss prom_tce_alloc_start;
static unsigned long __prombss prom_tce_alloc_end;
#endif

#ifdef CONFIG_PPC_PSERIES
static bool __prombss prom_radix_disable;
static bool __prombss prom_radix_gtse_disable;
static bool __prombss prom_xive_disable;
#endif

#ifdef CONFIG_PPC_SVM
static bool __prombss prom_svm_enable;
#endif

struct platform_support {
	bool hash_mmu;
	bool radix_mmu;
	bool radix_gtse;
	bool xive;
};

/* Platforms codes are now obsolete in the kernel. Now only used within this
 * file and ultimately gone too. Feel free to change them if you need, they
 * are not shared with anything outside of this file anymore
 */
#define PLATFORM_PSERIES	0x0100
#define PLATFORM_PSERIES_LPAR	0x0101
#define PLATFORM_LPAR		0x0001
#define PLATFORM_POWERMAC	0x0400
#define PLATFORM_GENERIC	0x0500

static int __prombss of_platform;

static char __prombss prom_cmd_line[COMMAND_LINE_SIZE];

static unsigned long __prombss prom_memory_limit;

static unsigned long __prombss alloc_top;
static unsigned long __prombss alloc_top_high;
static unsigned long __prombss alloc_bottom;
static unsigned long __prombss rmo_top;
static unsigned long __prombss ram_top;

static struct mem_map_entry __prombss mem_reserve_map[MEM_RESERVE_MAP_SIZE];
static int __prombss mem_reserve_cnt;

static cell_t __prombss regbuf[1024];

static bool  __prombss rtas_has_query_cpu_stopped;


/*
 * Error results ... some OF calls will return "-1" on error, some
 * will return 0, some will return either. To simplify, here are
 * macros to use with any ihandle or phandle return value to check if
 * it is valid
 */

#define PROM_ERROR		(-1u)
#define PHANDLE_VALID(p)	((p) != 0 && (p) != PROM_ERROR)
#define IHANDLE_VALID(i)	((i) != 0 && (i) != PROM_ERROR)

/* Copied from lib/string.c and lib/kstrtox.c */

static int __init prom_strcmp(const char *cs, const char *ct)
{
	unsigned char c1, c2;

	while (1) {
		c1 = *cs++;
		c2 = *ct++;
		if (c1 != c2)
			return c1 < c2 ? -1 : 1;
		if (!c1)
			break;
	}
	return 0;
}

static char __init *prom_strcpy(char *dest, const char *src)
{
	char *tmp = dest;

	while ((*dest++ = *src++) != '\0')
		/* nothing */;
	return tmp;
}

static int __init prom_strncmp(const char *cs, const char *ct, size_t count)
{
	unsigned char c1, c2;

	while (count) {
		c1 = *cs++;
		c2 = *ct++;
		if (c1 != c2)
			return c1 < c2 ? -1 : 1;
		if (!c1)
			break;
		count--;
	}
	return 0;
}

static size_t __init prom_strlen(const char *s)
{
	const char *sc;

	for (sc = s; *sc != '\0'; ++sc)
		/* nothing */;
	return sc - s;
}

static int __init prom_memcmp(const void *cs, const void *ct, size_t count)
{
	const unsigned char *su1, *su2;
	int res = 0;

	for (su1 = cs, su2 = ct; 0 < count; ++su1, ++su2, count--)
		if ((res = *su1 - *su2) != 0)
			break;
	return res;
}

static char __init *prom_strstr(const char *s1, const char *s2)
{
	size_t l1, l2;

	l2 = prom_strlen(s2);
	if (!l2)
		return (char *)s1;
	l1 = prom_strlen(s1);
	while (l1 >= l2) {
		l1--;
		if (!prom_memcmp(s1, s2, l2))
			return (char *)s1;
		s1++;
	}
	return NULL;
}

static size_t __init prom_strlcat(char *dest, const char *src, size_t count)
{
	size_t dsize = prom_strlen(dest);
	size_t len = prom_strlen(src);
	size_t res = dsize + len;

	/* This would be a bug */
	if (dsize >= count)
		return count;

	dest += dsize;
	count -= dsize;
	if (len >= count)
		len = count-1;
	memcpy(dest, src, len);
	dest[len] = 0;
	return res;

}

#ifdef CONFIG_PPC_PSERIES
static int __init prom_strtobool(const char *s, bool *res)
{
	if (!s)
		return -EINVAL;

	switch (s[0]) {
	case 'y':
	case 'Y':
	case '1':
		*res = true;
		return 0;
	case 'n':
	case 'N':
	case '0':
		*res = false;
		return 0;
	case 'o':
	case 'O':
		switch (s[1]) {
		case 'n':
		case 'N':
			*res = true;
			return 0;
		case 'f':
		case 'F':
			*res = false;
			return 0;
		default:
			break;
		}
		break;
	default:
		break;
	}

	return -EINVAL;
}
#endif

/* This is the one and *ONLY* place where we actually call open
 * firmware.
 */

static int __init call_prom(const char *service, int nargs, int nret, ...)
{
	int i;
	struct prom_args args;
	va_list list;

	args.service = cpu_to_be32(ADDR(service));
	args.nargs = cpu_to_be32(nargs);
	args.nret = cpu_to_be32(nret);

	va_start(list, nret);
	for (i = 0; i < nargs; i++)
		args.args[i] = cpu_to_be32(va_arg(list, prom_arg_t));
	va_end(list);

	for (i = 0; i < nret; i++)
		args.args[nargs+i] = 0;

	if (enter_prom(&args, prom_entry) < 0)
		return PROM_ERROR;

	return (nret > 0) ? be32_to_cpu(args.args[nargs]) : 0;
}

static int __init call_prom_ret(const char *service, int nargs, int nret,
				prom_arg_t *rets, ...)
{
	int i;
	struct prom_args args;
	va_list list;

	args.service = cpu_to_be32(ADDR(service));
	args.nargs = cpu_to_be32(nargs);
	args.nret = cpu_to_be32(nret);

	va_start(list, rets);
	for (i = 0; i < nargs; i++)
		args.args[i] = cpu_to_be32(va_arg(list, prom_arg_t));
	va_end(list);

	for (i = 0; i < nret; i++)
		args.args[nargs+i] = 0;

	if (enter_prom(&args, prom_entry) < 0)
		return PROM_ERROR;

	if (rets != NULL)
		for (i = 1; i < nret; ++i)
			rets[i-1] = be32_to_cpu(args.args[nargs+i]);

	return (nret > 0) ? be32_to_cpu(args.args[nargs]) : 0;
}


static void __init prom_print(const char *msg)
{
	const char *p, *q;

	if (prom.stdout == 0)
		return;

	for (p = msg; *p != 0; p = q) {
		for (q = p; *q != 0 && *q != '\n'; ++q)
			;
		if (q > p)
			call_prom("write", 3, 1, prom.stdout, p, q - p);
		if (*q == 0)
			break;
		++q;
		call_prom("write", 3, 1, prom.stdout, ADDR("\r\n"), 2);
	}
}


/*
 * Both prom_print_hex & prom_print_dec takes an unsigned long as input so that
 * we do not need __udivdi3 or __umoddi3 on 32bits.
 */
static void __init prom_print_hex(unsigned long val)
{
	int i, nibbles = sizeof(val)*2;
	char buf[sizeof(val)*2+1];

	for (i = nibbles-1;  i >= 0;  i--) {
		buf[i] = (val & 0xf) + '0';
		if (buf[i] > '9')
			buf[i] += ('a'-'0'-10);
		val >>= 4;
	}
	buf[nibbles] = '\0';
	call_prom("write", 3, 1, prom.stdout, buf, nibbles);
}

/* max number of decimal digits in an unsigned long */
#define UL_DIGITS 21
static void __init prom_print_dec(unsigned long val)
{
	int i, size;
	char buf[UL_DIGITS+1];

	for (i = UL_DIGITS-1; i >= 0;  i--) {
		buf[i] = (val % 10) + '0';
		val = val/10;
		if (val == 0)
			break;
	}
	/* shift stuff down */
	size = UL_DIGITS - i;
	call_prom("write", 3, 1, prom.stdout, buf+i, size);
}

__printf(1, 2)
static void __init prom_printf(const char *format, ...)
{
	const char *p, *q, *s;
	va_list args;
	unsigned long v;
	long vs;
	int n = 0;

	va_start(args, format);
	for (p = format; *p != 0; p = q) {
		for (q = p; *q != 0 && *q != '\n' && *q != '%'; ++q)
			;
		if (q > p)
			call_prom("write", 3, 1, prom.stdout, p, q - p);
		if (*q == 0)
			break;
		if (*q == '\n') {
			++q;
			call_prom("write", 3, 1, prom.stdout,
				  ADDR("\r\n"), 2);
			continue;
		}
		++q;
		if (*q == 0)
			break;
		while (*q == 'l') {
			++q;
			++n;
		}
		switch (*q) {
		case 's':
			++q;
			s = va_arg(args, const char *);
			prom_print(s);
			break;
		case 'x':
			++q;
			switch (n) {
			case 0:
				v = va_arg(args, unsigned int);
				break;
			case 1:
				v = va_arg(args, unsigned long);
				break;
			case 2:
			default:
				v = va_arg(args, unsigned long long);
				break;
			}
			prom_print_hex(v);
			break;
		case 'u':
			++q;
			switch (n) {
			case 0:
				v = va_arg(args, unsigned int);
				break;
			case 1:
				v = va_arg(args, unsigned long);
				break;
			case 2:
			default:
				v = va_arg(args, unsigned long long);
				break;
			}
			prom_print_dec(v);
			break;
		case 'd':
			++q;
			switch (n) {
			case 0:
				vs = va_arg(args, int);
				break;
			case 1:
				vs = va_arg(args, long);
				break;
			case 2:
			default:
				vs = va_arg(args, long long);
				break;
			}
			if (vs < 0) {
				prom_print("-");
				vs = -vs;
			}
			prom_print_dec(vs);
			break;
		}
	}
	va_end(args);
}


static unsigned int __init prom_claim(unsigned long virt, unsigned long size,
				unsigned long align)
{

	if (align == 0 && (OF_WORKAROUNDS & OF_WA_CLAIM)) {
		/*
		 * Old OF requires we claim physical and virtual separately
		 * and then map explicitly (assuming virtual mode)
		 */
		int ret;
		prom_arg_t result;

		ret = call_prom_ret("call-method", 5, 2, &result,
				    ADDR("claim"), prom.memory,
				    align, size, virt);
		if (ret != 0 || result == -1)
			return -1;
		ret = call_prom_ret("call-method", 5, 2, &result,
				    ADDR("claim"), prom.mmumap,
				    align, size, virt);
		if (ret != 0) {
			call_prom("call-method", 4, 1, ADDR("release"),
				  prom.memory, size, virt);
			return -1;
		}
		/* the 0x12 is M (coherence) + PP == read/write */
		call_prom("call-method", 6, 1,
			  ADDR("map"), prom.mmumap, 0x12, size, virt, virt);
		return virt;
	}
	return call_prom("claim", 3, 1, (prom_arg_t)virt, (prom_arg_t)size,
			 (prom_arg_t)align);
}

static void __init __attribute__((noreturn)) prom_panic(const char *reason)
{
	prom_print(reason);
	/* Do not call exit because it clears the screen on pmac
	 * it also causes some sort of double-fault on early pmacs */
	if (of_platform == PLATFORM_POWERMAC)
		asm("trap\n");

	/* ToDo: should put up an SRC here on pSeries */
	call_prom("exit", 0, 0);

	for (;;)			/* should never get here */
		;
}


static int __init prom_next_node(phandle *nodep)
{
	phandle node;

	if ((node = *nodep) != 0
	    && (*nodep = call_prom("child", 1, 1, node)) != 0)
		return 1;
	if ((*nodep = call_prom("peer", 1, 1, node)) != 0)
		return 1;
	for (;;) {
		if ((node = call_prom("parent", 1, 1, node)) == 0)
			return 0;
		if ((*nodep = call_prom("peer", 1, 1, node)) != 0)
			return 1;
	}
}

static inline int __init prom_getprop(phandle node, const char *pname,
				      void *value, size_t valuelen)
{
	return call_prom("getprop", 4, 1, node, ADDR(pname),
			 (u32)(unsigned long) value, (u32) valuelen);
}

static inline int __init prom_getproplen(phandle node, const char *pname)
{
	return call_prom("getproplen", 2, 1, node, ADDR(pname));
}

static void add_string(char **str, const char *q)
{
	char *p = *str;

	while (*q)
		*p++ = *q++;
	*p++ = ' ';
	*str = p;
}

static char *tohex(unsigned int x)
{
	static const char digits[] __initconst = "0123456789abcdef";
	static char result[9] __prombss;
	int i;

	result[8] = 0;
	i = 8;
	do {
		--i;
		result[i] = digits[x & 0xf];
		x >>= 4;
	} while (x != 0 && i > 0);
	return &result[i];
}

static int __init prom_setprop(phandle node, const char *nodename,
			       const char *pname, void *value, size_t valuelen)
{
	char cmd[256], *p;

	if (!(OF_WORKAROUNDS & OF_WA_LONGTRAIL))
		return call_prom("setprop", 4, 1, node, ADDR(pname),
				 (u32)(unsigned long) value, (u32) valuelen);

	/* gah... setprop doesn't work on longtrail, have to use interpret */
	p = cmd;
	add_string(&p, "dev");
	add_string(&p, nodename);
	add_string(&p, tohex((u32)(unsigned long) value));
	add_string(&p, tohex(valuelen));
	add_string(&p, tohex(ADDR(pname)));
	add_string(&p, tohex(prom_strlen(pname)));
	add_string(&p, "property");
	*p = 0;
	return call_prom("interpret", 1, 1, (u32)(unsigned long) cmd);
}

/* We can't use the standard versions because of relocation headaches. */
#define isxdigit(c)	(('0' <= (c) && (c) <= '9') \
			 || ('a' <= (c) && (c) <= 'f') \
			 || ('A' <= (c) && (c) <= 'F'))

#define isdigit(c)	('0' <= (c) && (c) <= '9')
#define islower(c)	('a' <= (c) && (c) <= 'z')
#define toupper(c)	(islower(c) ? ((c) - 'a' + 'A') : (c))

static unsigned long prom_strtoul(const char *cp, const char **endp)
{
	unsigned long result = 0, base = 10, value;

	if (*cp == '0') {
		base = 8;
		cp++;
		if (toupper(*cp) == 'X') {
			cp++;
			base = 16;
		}
	}

	while (isxdigit(*cp) &&
	       (value = isdigit(*cp) ? *cp - '0' : toupper(*cp) - 'A' + 10) < base) {
		result = result * base + value;
		cp++;
	}

	if (endp)
		*endp = cp;

	return result;
}

static unsigned long prom_memparse(const char *ptr, const char **retptr)
{
	unsigned long ret = prom_strtoul(ptr, retptr);
	int shift = 0;

	/*
	 * We can't use a switch here because GCC *may* generate a
	 * jump table which won't work, because we're not running at
	 * the address we're linked at.
	 */
	if ('G' == **retptr || 'g' == **retptr)
		shift = 30;

	if ('M' == **retptr || 'm' == **retptr)
		shift = 20;

	if ('K' == **retptr || 'k' == **retptr)
		shift = 10;

	if (shift) {
		ret <<= shift;
		(*retptr)++;
	}

	return ret;
}

/*
 * Early parsing of the command line passed to the kernel, used for
 * "mem=x" and the options that affect the iommu
 */
static void __init early_cmdline_parse(void)
{
	const char *opt;

	char *p;
	int l = 0;

	prom_cmd_line[0] = 0;
	p = prom_cmd_line;

	if (!IS_ENABLED(CONFIG_CMDLINE_FORCE) && (long)prom.chosen > 0)
		l = prom_getprop(prom.chosen, "bootargs", p, COMMAND_LINE_SIZE-1);

	if (IS_ENABLED(CONFIG_CMDLINE_EXTEND) || l <= 0 || p[0] == '\0')
		prom_strlcat(prom_cmd_line, " " CONFIG_CMDLINE,
			     sizeof(prom_cmd_line));

	prom_printf("command line: %s\n", prom_cmd_line);

#ifdef CONFIG_PPC64
	opt = prom_strstr(prom_cmd_line, "iommu=");
	if (opt) {
		prom_printf("iommu opt is: %s\n", opt);
		opt += 6;
		while (*opt && *opt == ' ')
			opt++;
		if (!prom_strncmp(opt, "off", 3))
			prom_iommu_off = 1;
		else if (!prom_strncmp(opt, "force", 5))
			prom_iommu_force_on = 1;
	}
#endif
	opt = prom_strstr(prom_cmd_line, "mem=");
	if (opt) {
		opt += 4;
		prom_memory_limit = prom_memparse(opt, (const char **)&opt);
#ifdef CONFIG_PPC64
		/* Align to 16 MB == size of ppc64 large page */
		prom_memory_limit = ALIGN(prom_memory_limit, 0x1000000);
#endif
	}

#ifdef CONFIG_PPC_PSERIES
	prom_radix_disable = !IS_ENABLED(CONFIG_PPC_RADIX_MMU_DEFAULT);
	opt = prom_strstr(prom_cmd_line, "disable_radix");
	if (opt) {
		opt += 13;
		if (*opt && *opt == '=') {
			bool val;

			if (prom_strtobool(++opt, &val))
				prom_radix_disable = false;
			else
				prom_radix_disable = val;
		} else
			prom_radix_disable = true;
	}
	if (prom_radix_disable)
		prom_debug("Radix disabled from cmdline\n");

	opt = prom_strstr(prom_cmd_line, "radix_hcall_invalidate=on");
	if (opt) {
		prom_radix_gtse_disable = true;
		prom_debug("Radix GTSE disabled from cmdline\n");
	}

	opt = prom_strstr(prom_cmd_line, "xive=off");
	if (opt) {
		prom_xive_disable = true;
		prom_debug("XIVE disabled from cmdline\n");
	}
#endif /* CONFIG_PPC_PSERIES */

#ifdef CONFIG_PPC_SVM
	opt = prom_strstr(prom_cmd_line, "svm=");
	if (opt) {
		bool val;

		opt += sizeof("svm=") - 1;
		if (!prom_strtobool(opt, &val))
			prom_svm_enable = val;
	}
#endif /* CONFIG_PPC_SVM */
}

#ifdef CONFIG_PPC_PSERIES
/*
 * The architecture vector has an array of PVR mask/value pairs,
 * followed by # option vectors - 1, followed by the option vectors.
 *
 * See prom.h for the definition of the bits specified in the
 * architecture vector.
 */

/* Firmware expects the value to be n - 1, where n is the # of vectors */
#define NUM_VECTORS(n)		((n) - 1)

/*
 * Firmware expects 1 + n - 2, where n is the length of the option vector in
 * bytes. The 1 accounts for the length byte itself, the - 2 .. ?
 */
#define VECTOR_LENGTH(n)	(1 + (n) - 2)

struct option_vector1 {
	u8 byte1;
	u8 arch_versions;
	u8 arch_versions3;
} __packed;

struct option_vector2 {
	u8 byte1;
	__be16 reserved;
	__be32 real_base;
	__be32 real_size;
	__be32 virt_base;
	__be32 virt_size;
	__be32 load_base;
	__be32 min_rma;
	__be32 min_load;
	u8 min_rma_percent;
	u8 max_pft_size;
} __packed;

struct option_vector3 {
	u8 byte1;
	u8 byte2;
} __packed;

struct option_vector4 {
	u8 byte1;
	u8 min_vp_cap;
} __packed;

struct option_vector5 {
	u8 byte1;
	u8 byte2;
	u8 byte3;
	u8 cmo;
	u8 associativity;
	u8 bin_opts;
	u8 micro_checkpoint;
	u8 reserved0;
	__be32 max_cpus;
	__be16 papr_level;
	__be16 reserved1;
	u8 platform_facilities;
	u8 reserved2;
	__be16 reserved3;
	u8 subprocessors;
	u8 byte22;
	u8 intarch;
	u8 mmu;
	u8 hash_ext;
	u8 radix_ext;
} __packed;

struct option_vector6 {
	u8 reserved;
	u8 secondary_pteg;
	u8 os_name;
} __packed;

struct ibm_arch_vec {
	struct { u32 mask, val; } pvrs[14];

	u8 num_vectors;

	u8 vec1_len;
	struct option_vector1 vec1;

	u8 vec2_len;
	struct option_vector2 vec2;

	u8 vec3_len;
	struct option_vector3 vec3;

	u8 vec4_len;
	struct option_vector4 vec4;

	u8 vec5_len;
	struct option_vector5 vec5;

	u8 vec6_len;
	struct option_vector6 vec6;
} __packed;

static const struct ibm_arch_vec ibm_architecture_vec_template __initconst = {
	.pvrs = {
		{
			.mask = cpu_to_be32(0xfffe0000), /* POWER5/POWER5+ */
			.val  = cpu_to_be32(0x003a0000),
		},
		{
			.mask = cpu_to_be32(0xffff0000), /* POWER6 */
			.val  = cpu_to_be32(0x003e0000),
		},
		{
			.mask = cpu_to_be32(0xffff0000), /* POWER7 */
			.val  = cpu_to_be32(0x003f0000),
		},
		{
			.mask = cpu_to_be32(0xffff0000), /* POWER8E */
			.val  = cpu_to_be32(0x004b0000),
		},
		{
			.mask = cpu_to_be32(0xffff0000), /* POWER8NVL */
			.val  = cpu_to_be32(0x004c0000),
		},
		{
			.mask = cpu_to_be32(0xffff0000), /* POWER8 */
			.val  = cpu_to_be32(0x004d0000),
		},
		{
			.mask = cpu_to_be32(0xffff0000), /* POWER9 */
			.val  = cpu_to_be32(0x004e0000),
		},
		{
			.mask = cpu_to_be32(0xffff0000), /* POWER10 */
			.val  = cpu_to_be32(0x00800000),
		},
		{
			.mask = cpu_to_be32(0xffffffff), /* all 3.1-compliant */
			.val  = cpu_to_be32(0x0f000006),
		},
		{
			.mask = cpu_to_be32(0xffffffff), /* all 3.00-compliant */
			.val  = cpu_to_be32(0x0f000005),
		},
		{
			.mask = cpu_to_be32(0xffffffff), /* all 2.07-compliant */
			.val  = cpu_to_be32(0x0f000004),
		},
		{
			.mask = cpu_to_be32(0xffffffff), /* all 2.06-compliant */
			.val  = cpu_to_be32(0x0f000003),
		},
		{
			.mask = cpu_to_be32(0xffffffff), /* all 2.05-compliant */
			.val  = cpu_to_be32(0x0f000002),
		},
		{
			.mask = cpu_to_be32(0xfffffffe), /* all 2.04-compliant and earlier */
			.val  = cpu_to_be32(0x0f000001),
		},
	},

	.num_vectors = NUM_VECTORS(6),

	.vec1_len = VECTOR_LENGTH(sizeof(struct option_vector1)),
	.vec1 = {
		.byte1 = 0,
		.arch_versions = OV1_PPC_2_00 | OV1_PPC_2_01 | OV1_PPC_2_02 | OV1_PPC_2_03 |
				 OV1_PPC_2_04 | OV1_PPC_2_05 | OV1_PPC_2_06 | OV1_PPC_2_07,
		.arch_versions3 = OV1_PPC_3_00 | OV1_PPC_3_1,
	},

	.vec2_len = VECTOR_LENGTH(sizeof(struct option_vector2)),
	/* option vector 2: Open Firmware options supported */
	.vec2 = {
		.byte1 = OV2_REAL_MODE,
		.reserved = 0,
		.real_base = cpu_to_be32(0xffffffff),
		.real_size = cpu_to_be32(0xffffffff),
		.virt_base = cpu_to_be32(0xffffffff),
		.virt_size = cpu_to_be32(0xffffffff),
		.load_base = cpu_to_be32(0xffffffff),
		.min_rma = cpu_to_be32(512),		/* 512MB min RMA */
		.min_load = cpu_to_be32(0xffffffff),	/* full client load */
		.min_rma_percent = 0,	/* min RMA percentage of total RAM */
		.max_pft_size = 48,	/* max log_2(hash table size) */
	},

	.vec3_len = VECTOR_LENGTH(sizeof(struct option_vector3)),
	/* option vector 3: processor options supported */
	.vec3 = {
		.byte1 = 0,			/* don't ignore, don't halt */
		.byte2 = OV3_FP | OV3_VMX | OV3_DFP,
	},

	.vec4_len = VECTOR_LENGTH(sizeof(struct option_vector4)),
	/* option vector 4: IBM PAPR implementation */
	.vec4 = {
		.byte1 = 0,			/* don't halt */
		.min_vp_cap = OV4_MIN_ENT_CAP,	/* minimum VP entitled capacity */
	},

	.vec5_len = VECTOR_LENGTH(sizeof(struct option_vector5)),
	/* option vector 5: PAPR/OF options */
	.vec5 = {
		.byte1 = 0,				/* don't ignore, don't halt */
		.byte2 = OV5_FEAT(OV5_LPAR) | OV5_FEAT(OV5_SPLPAR) | OV5_FEAT(OV5_LARGE_PAGES) |
		OV5_FEAT(OV5_DRCONF_MEMORY) | OV5_FEAT(OV5_DONATE_DEDICATE_CPU) |
#ifdef CONFIG_PCI_MSI
		/* PCIe/MSI support.  Without MSI full PCIe is not supported */
		OV5_FEAT(OV5_MSI),
#else
		0,
#endif
		.byte3 = 0,
		.cmo =
#ifdef CONFIG_PPC_SMLPAR
		OV5_FEAT(OV5_CMO) | OV5_FEAT(OV5_XCMO),
#else
		0,
#endif
		.associativity = OV5_FEAT(OV5_TYPE1_AFFINITY) | OV5_FEAT(OV5_PRRN),
		.bin_opts = OV5_FEAT(OV5_RESIZE_HPT) | OV5_FEAT(OV5_HP_EVT),
		.micro_checkpoint = 0,
		.reserved0 = 0,
		.max_cpus = cpu_to_be32(NR_CPUS),	/* number of cores supported */
		.papr_level = 0,
		.reserved1 = 0,
		.platform_facilities = OV5_FEAT(OV5_PFO_HW_RNG) | OV5_FEAT(OV5_PFO_HW_ENCR) | OV5_FEAT(OV5_PFO_HW_842),
		.reserved2 = 0,
		.reserved3 = 0,
		.subprocessors = 1,
		.byte22 = OV5_FEAT(OV5_DRMEM_V2) | OV5_FEAT(OV5_DRC_INFO),
		.intarch = 0,
		.mmu = 0,
		.hash_ext = 0,
		.radix_ext = 0,
	},

	/* option vector 6: IBM PAPR hints */
	.vec6_len = VECTOR_LENGTH(sizeof(struct option_vector6)),
	.vec6 = {
		.reserved = 0,
		.secondary_pteg = 0,
		.os_name = OV6_LINUX,
	},
};

static struct ibm_arch_vec __prombss ibm_architecture_vec  ____cacheline_aligned;

/* Old method - ELF header with PT_NOTE sections only works on BE */
#ifdef __BIG_ENDIAN__
static const struct fake_elf {
	Elf32_Ehdr	elfhdr;
	Elf32_Phdr	phdr[2];
	struct chrpnote {
		u32	namesz;
		u32	descsz;
		u32	type;
		char	name[8];	/* "PowerPC" */
		struct chrpdesc {
			u32	real_mode;
			u32	real_base;
			u32	real_size;
			u32	virt_base;
			u32	virt_size;
			u32	load_base;
		} chrpdesc;
	} chrpnote;
	struct rpanote {
		u32	namesz;
		u32	descsz;
		u32	type;
		char	name[24];	/* "IBM,RPA-Client-Config" */
		struct rpadesc {
			u32	lpar_affinity;
			u32	min_rmo_size;
			u32	min_rmo_percent;
			u32	max_pft_size;
			u32	splpar;
			u32	min_load;
			u32	new_mem_def;
			u32	ignore_me;
		} rpadesc;
	} rpanote;
} fake_elf __initconst = {
	.elfhdr = {
		.e_ident = { 0x7f, 'E', 'L', 'F',
			     ELFCLASS32, ELFDATA2MSB, EV_CURRENT },
		.e_type = ET_EXEC,	/* yeah right */
		.e_machine = EM_PPC,
		.e_version = EV_CURRENT,
		.e_phoff = offsetof(struct fake_elf, phdr),
		.e_phentsize = sizeof(Elf32_Phdr),
		.e_phnum = 2
	},
	.phdr = {
		[0] = {
			.p_type = PT_NOTE,
			.p_offset = offsetof(struct fake_elf, chrpnote),
			.p_filesz = sizeof(struct chrpnote)
		}, [1] = {
			.p_type = PT_NOTE,
			.p_offset = offsetof(struct fake_elf, rpanote),
			.p_filesz = sizeof(struct rpanote)
		}
	},
	.chrpnote = {
		.namesz = sizeof("PowerPC"),
		.descsz = sizeof(struct chrpdesc),
		.type = 0x1275,
		.name = "PowerPC",
		.chrpdesc = {
			.real_mode = ~0U,	/* ~0 means "don't care" */
			.real_base = ~0U,
			.real_size = ~0U,
			.virt_base = ~0U,
			.virt_size = ~0U,
			.load_base = ~0U
		},
	},
	.rpanote = {
		.namesz = sizeof("IBM,RPA-Client-Config"),
		.descsz = sizeof(struct rpadesc),
		.type = 0x12759999,
		.name = "IBM,RPA-Client-Config",
		.rpadesc = {
			.lpar_affinity = 0,
			.min_rmo_size = 64,	/* in megabytes */
			.min_rmo_percent = 0,
			.max_pft_size = 48,	/* 2^48 bytes max PFT size */
			.splpar = 1,
			.min_load = ~0U,
			.new_mem_def = 0
		}
	}
};
#endif /* __BIG_ENDIAN__ */

static int __init prom_count_smt_threads(void)
{
	phandle node;
	char type[64];
	unsigned int plen;

	/* Pick up th first CPU node we can find */
	for (node = 0; prom_next_node(&node); ) {
		type[0] = 0;
		prom_getprop(node, "device_type", type, sizeof(type));

		if (prom_strcmp(type, "cpu"))
			continue;
		/*
		 * There is an entry for each smt thread, each entry being
		 * 4 bytes long.  All cpus should have the same number of
		 * smt threads, so return after finding the first.
		 */
		plen = prom_getproplen(node, "ibm,ppc-interrupt-server#s");
		if (plen == PROM_ERROR)
			break;
		plen >>= 2;
		prom_debug("Found %lu smt threads per core\n", (unsigned long)plen);

		/* Sanity check */
		if (plen < 1 || plen > 64) {
			prom_printf("Threads per core %lu out of bounds, assuming 1\n",
				    (unsigned long)plen);
			return 1;
		}
		return plen;
	}
	prom_debug("No threads found, assuming 1 per core\n");

	return 1;

}

static void __init prom_parse_mmu_model(u8 val,
					struct platform_support *support)
{
	switch (val) {
	case OV5_FEAT(OV5_MMU_DYNAMIC):
	case OV5_FEAT(OV5_MMU_EITHER): /* Either Available */
		prom_debug("MMU - either supported\n");
		support->radix_mmu = !prom_radix_disable;
		support->hash_mmu = true;
		break;
	case OV5_FEAT(OV5_MMU_RADIX): /* Only Radix */
		prom_debug("MMU - radix only\n");
		if (prom_radix_disable) {
			/*
			 * If we __have__ to do radix, we're better off ignoring
			 * the command line rather than not booting.
			 */
			prom_printf("WARNING: Ignoring cmdline option disable_radix\n");
		}
		support->radix_mmu = true;
		break;
	case OV5_FEAT(OV5_MMU_HASH):
		prom_debug("MMU - hash only\n");
		support->hash_mmu = true;
		break;
	default:
		prom_debug("Unknown mmu support option: 0x%x\n", val);
		break;
	}
}

static void __init prom_parse_xive_model(u8 val,
					 struct platform_support *support)
{
	switch (val) {
	case OV5_FEAT(OV5_XIVE_EITHER): /* Either Available */
		prom_debug("XIVE - either mode supported\n");
		support->xive = !prom_xive_disable;
		break;
	case OV5_FEAT(OV5_XIVE_EXPLOIT): /* Only Exploitation mode */
		prom_debug("XIVE - exploitation mode supported\n");
		if (prom_xive_disable) {
			/*
			 * If we __have__ to do XIVE, we're better off ignoring
			 * the command line rather than not booting.
			 */
			prom_printf("WARNING: Ignoring cmdline option xive=off\n");
		}
		support->xive = true;
		break;
	case OV5_FEAT(OV5_XIVE_LEGACY): /* Only Legacy mode */
		prom_debug("XIVE - legacy mode supported\n");
		break;
	default:
		prom_debug("Unknown xive support option: 0x%x\n", val);
		break;
	}
}

static void __init prom_parse_platform_support(u8 index, u8 val,
					       struct platform_support *support)
{
	switch (index) {
	case OV5_INDX(OV5_MMU_SUPPORT): /* MMU Model */
		prom_parse_mmu_model(val & OV5_FEAT(OV5_MMU_SUPPORT), support);
		break;
	case OV5_INDX(OV5_RADIX_GTSE): /* Radix Extensions */
		if (val & OV5_FEAT(OV5_RADIX_GTSE))
			support->radix_gtse = !prom_radix_gtse_disable;
		break;
	case OV5_INDX(OV5_XIVE_SUPPORT): /* Interrupt mode */
		prom_parse_xive_model(val & OV5_FEAT(OV5_XIVE_SUPPORT),
				      support);
		break;
	}
}

static void __init prom_check_platform_support(void)
{
	struct platform_support supported = {
		.hash_mmu = false,
		.radix_mmu = false,
		.radix_gtse = false,
		.xive = false
	};
	int prop_len = prom_getproplen(prom.chosen,
				       "ibm,arch-vec-5-platform-support");

	/*
	 * First copy the architecture vec template
	 *
	 * use memcpy() instead of *vec = *vec_template so that GCC replaces it
	 * by __memcpy() when KASAN is active
	 */
	memcpy(&ibm_architecture_vec, &ibm_architecture_vec_template,
	       sizeof(ibm_architecture_vec));

	if (prop_len > 1) {
		int i;
		u8 vec[8];
		prom_debug("Found ibm,arch-vec-5-platform-support, len: %d\n",
			   prop_len);
		if (prop_len > sizeof(vec))
			prom_printf("WARNING: ibm,arch-vec-5-platform-support longer than expected (len: %d)\n",
				    prop_len);
		prom_getprop(prom.chosen, "ibm,arch-vec-5-platform-support", &vec, sizeof(vec));
		for (i = 0; i < prop_len; i += 2) {
			prom_debug("%d: index = 0x%x val = 0x%x\n", i / 2, vec[i], vec[i + 1]);
			prom_parse_platform_support(vec[i], vec[i + 1], &supported);
		}
	}

	if (supported.radix_mmu && IS_ENABLED(CONFIG_PPC_RADIX_MMU)) {
		/* Radix preferred - Check if GTSE is also supported */
		prom_debug("Asking for radix\n");
		ibm_architecture_vec.vec5.mmu = OV5_FEAT(OV5_MMU_RADIX);
		if (supported.radix_gtse)
			ibm_architecture_vec.vec5.radix_ext =
					OV5_FEAT(OV5_RADIX_GTSE);
		else
			prom_debug("Radix GTSE isn't supported\n");
	} else if (supported.hash_mmu) {
		/* Default to hash mmu (if we can) */
		prom_debug("Asking for hash\n");
		ibm_architecture_vec.vec5.mmu = OV5_FEAT(OV5_MMU_HASH);
	} else {
		/* We're probably on a legacy hypervisor */
		prom_debug("Assuming legacy hash support\n");
	}

	if (supported.xive) {
		prom_debug("Asking for XIVE\n");
		ibm_architecture_vec.vec5.intarch = OV5_FEAT(OV5_XIVE_EXPLOIT);
	}
}

static void __init prom_send_capabilities(void)
{
	ihandle root;
	prom_arg_t ret;
	u32 cores;

	/* Check ibm,arch-vec-5-platform-support and fixup vec5 if required */
	prom_check_platform_support();

	root = call_prom("open", 1, 1, ADDR("/"));
	if (root != 0) {
		/* We need to tell the FW about the number of cores we support.
		 *
		 * To do that, we count the number of threads on the first core
		 * (we assume this is the same for all cores) and use it to
		 * divide NR_CPUS.
		 */

		cores = DIV_ROUND_UP(NR_CPUS, prom_count_smt_threads());
		prom_printf("Max number of cores passed to firmware: %u (NR_CPUS = %d)\n",
			    cores, NR_CPUS);

		ibm_architecture_vec.vec5.max_cpus = cpu_to_be32(cores);

		/* try calling the ibm,client-architecture-support method */
		prom_printf("Calling ibm,client-architecture-support...");
		if (call_prom_ret("call-method", 3, 2, &ret,
				  ADDR("ibm,client-architecture-support"),
				  root,
				  ADDR(&ibm_architecture_vec)) == 0) {
			/* the call exists... */
			if (ret)
				prom_printf("\nWARNING: ibm,client-architecture"
					    "-support call FAILED!\n");
			call_prom("close", 1, 0, root);
			prom_printf(" done\n");
			return;
		}
		call_prom("close", 1, 0, root);
		prom_printf(" not implemented\n");
	}

#ifdef __BIG_ENDIAN__
	{
		ihandle elfloader;

		/* no ibm,client-architecture-support call, try the old way */
		elfloader = call_prom("open", 1, 1,
				      ADDR("/packages/elf-loader"));
		if (elfloader == 0) {
			prom_printf("couldn't open /packages/elf-loader\n");
			return;
		}
		call_prom("call-method", 3, 1, ADDR("process-elf-header"),
			  elfloader, ADDR(&fake_elf));
		call_prom("close", 1, 0, elfloader);
	}
#endif /* __BIG_ENDIAN__ */
}
#endif /* CONFIG_PPC_PSERIES */

/*
 * Memory allocation strategy... our layout is normally:
 *
 *  at 14Mb or more we have vmlinux, then a gap and initrd.  In some
 *  rare cases, initrd might end up being before the kernel though.
 *  We assume this won't override the final kernel at 0, we have no
 *  provision to handle that in this version, but it should hopefully
 *  never happen.
 *
 *  alloc_top is set to the top of RMO, eventually shrink down if the
 *  TCEs overlap
 *
 *  alloc_bottom is set to the top of kernel/initrd
 *
 *  from there, allocations are done this way : rtas is allocated
 *  topmost, and the device-tree is allocated from the bottom. We try
 *  to grow the device-tree allocation as we progress. If we can't,
 *  then we fail, we don't currently have a facility to restart
 *  elsewhere, but that shouldn't be necessary.
 *
 *  Note that calls to reserve_mem have to be done explicitly, memory
 *  allocated with either alloc_up or alloc_down isn't automatically
 *  reserved.
 */


/*
 * Allocates memory in the RMO upward from the kernel/initrd
 *
 * When align is 0, this is a special case, it means to allocate in place
 * at the current location of alloc_bottom or fail (that is basically
 * extending the previous allocation). Used for the device-tree flattening
 */
static unsigned long __init alloc_up(unsigned long size, unsigned long align)
{
	unsigned long base = alloc_bottom;
	unsigned long addr = 0;

	if (align)
		base = ALIGN(base, align);
	prom_debug("%s(%lx, %lx)\n", __func__, size, align);
	if (ram_top == 0)
		prom_panic("alloc_up() called with mem not initialized\n");

	if (align)
		base = ALIGN(alloc_bottom, align);
	else
		base = alloc_bottom;

	for(; (base + size) <= alloc_top; 
	    base = ALIGN(base + 0x100000, align)) {
		prom_debug("    trying: 0x%lx\n\r", base);
		addr = (unsigned long)prom_claim(base, size, 0);
		if (addr != PROM_ERROR && addr != 0)
			break;
		addr = 0;
		if (align == 0)
			break;
	}
	if (addr == 0)
		return 0;
	alloc_bottom = addr + size;

	prom_debug(" -> %lx\n", addr);
	prom_debug("  alloc_bottom : %lx\n", alloc_bottom);
	prom_debug("  alloc_top    : %lx\n", alloc_top);
	prom_debug("  alloc_top_hi : %lx\n", alloc_top_high);
	prom_debug("  rmo_top      : %lx\n", rmo_top);
	prom_debug("  ram_top      : %lx\n", ram_top);

	return addr;
}

/*
 * Allocates memory downward, either from top of RMO, or if highmem
 * is set, from the top of RAM.  Note that this one doesn't handle
 * failures.  It does claim memory if highmem is not set.
 */
static unsigned long __init alloc_down(unsigned long size, unsigned long align,
				       int highmem)
{
	unsigned long base, addr = 0;

	prom_debug("%s(%lx, %lx, %s)\n", __func__, size, align,
		   highmem ? "(high)" : "(low)");
	if (ram_top == 0)
		prom_panic("alloc_down() called with mem not initialized\n");

	if (highmem) {
		/* Carve out storage for the TCE table. */
		addr = ALIGN_DOWN(alloc_top_high - size, align);
		if (addr <= alloc_bottom)
			return 0;
		/* Will we bump into the RMO ? If yes, check out that we
		 * didn't overlap existing allocations there, if we did,
		 * we are dead, we must be the first in town !
		 */
		if (addr < rmo_top) {
			/* Good, we are first */
			if (alloc_top == rmo_top)
				alloc_top = rmo_top = addr;
			else
				return 0;
		}
		alloc_top_high = addr;
		goto bail;
	}

	base = ALIGN_DOWN(alloc_top - size, align);
	for (; base > alloc_bottom;
	     base = ALIGN_DOWN(base - 0x100000, align))  {
		prom_debug("    trying: 0x%lx\n\r", base);
		addr = (unsigned long)prom_claim(base, size, 0);
		if (addr != PROM_ERROR && addr != 0)
			break;
		addr = 0;
	}
	if (addr == 0)
		return 0;
	alloc_top = addr;

 bail:
	prom_debug(" -> %lx\n", addr);
	prom_debug("  alloc_bottom : %lx\n", alloc_bottom);
	prom_debug("  alloc_top    : %lx\n", alloc_top);
	prom_debug("  alloc_top_hi : %lx\n", alloc_top_high);
	prom_debug("  rmo_top      : %lx\n", rmo_top);
	prom_debug("  ram_top      : %lx\n", ram_top);

	return addr;
}

/*
 * Parse a "reg" cell
 */
static unsigned long __init prom_next_cell(int s, cell_t **cellp)
{
	cell_t *p = *cellp;
	unsigned long r = 0;

	/* Ignore more than 2 cells */
	while (s > sizeof(unsigned long) / 4) {
		p++;
		s--;
	}
	r = be32_to_cpu(*p++);
#ifdef CONFIG_PPC64
	if (s > 1) {
		r <<= 32;
		r |= be32_to_cpu(*(p++));
	}
#endif
	*cellp = p;
	return r;
}

/*
 * Very dumb function for adding to the memory reserve list, but
 * we don't need anything smarter at this point
 *
 * XXX Eventually check for collisions.  They should NEVER happen.
 * If problems seem to show up, it would be a good start to track
 * them down.
 */
static void __init reserve_mem(u64 base, u64 size)
{
	u64 top = base + size;
	unsigned long cnt = mem_reserve_cnt;

	if (size == 0)
		return;

	/* We need to always keep one empty entry so that we
	 * have our terminator with "size" set to 0 since we are
	 * dumb and just copy this entire array to the boot params
	 */
	base = ALIGN_DOWN(base, PAGE_SIZE);
	top = ALIGN(top, PAGE_SIZE);
	size = top - base;

	if (cnt >= (MEM_RESERVE_MAP_SIZE - 1))
		prom_panic("Memory reserve map exhausted !\n");
	mem_reserve_map[cnt].base = cpu_to_be64(base);
	mem_reserve_map[cnt].size = cpu_to_be64(size);
	mem_reserve_cnt = cnt + 1;
}

/*
 * Initialize memory allocation mechanism, parse "memory" nodes and
 * obtain that way the top of memory and RMO to setup out local allocator
 */
static void __init prom_init_mem(void)
{
	phandle node;
	char type[64];
	unsigned int plen;
	cell_t *p, *endp;
	__be32 val;
	u32 rac, rsc;

	/*
	 * We iterate the memory nodes to find
	 * 1) top of RMO (first node)
	 * 2) top of memory
	 */
	val = cpu_to_be32(2);
	prom_getprop(prom.root, "#address-cells", &val, sizeof(val));
	rac = be32_to_cpu(val);
	val = cpu_to_be32(1);
	prom_getprop(prom.root, "#size-cells", &val, sizeof(rsc));
	rsc = be32_to_cpu(val);
	prom_debug("root_addr_cells: %x\n", rac);
	prom_debug("root_size_cells: %x\n", rsc);

	prom_debug("scanning memory:\n");

	for (node = 0; prom_next_node(&node); ) {
		type[0] = 0;
		prom_getprop(node, "device_type", type, sizeof(type));

		if (type[0] == 0) {
			/*
			 * CHRP Longtrail machines have no device_type
			 * on the memory node, so check the name instead...
			 */
			prom_getprop(node, "name", type, sizeof(type));
		}
		if (prom_strcmp(type, "memory"))
			continue;

		plen = prom_getprop(node, "reg", regbuf, sizeof(regbuf));
		if (plen > sizeof(regbuf)) {
			prom_printf("memory node too large for buffer !\n");
			plen = sizeof(regbuf);
		}
		p = regbuf;
		endp = p + (plen / sizeof(cell_t));

#ifdef DEBUG_PROM
		memset(prom_scratch, 0, sizeof(prom_scratch));
		call_prom("package-to-path", 3, 1, node, prom_scratch,
			  sizeof(prom_scratch) - 1);
		prom_debug("  node %s :\n", prom_scratch);
#endif /* DEBUG_PROM */

		while ((endp - p) >= (rac + rsc)) {
			unsigned long base, size;

			base = prom_next_cell(rac, &p);
			size = prom_next_cell(rsc, &p);

			if (size == 0)
				continue;
			prom_debug("    %lx %lx\n", base, size);
			if (base == 0 && (of_platform & PLATFORM_LPAR))
				rmo_top = size;
			if ((base + size) > ram_top)
				ram_top = base + size;
		}
	}

	alloc_bottom = PAGE_ALIGN((unsigned long)&_end + 0x4000);

	/*
	 * If prom_memory_limit is set we reduce the upper limits *except* for
	 * alloc_top_high. This must be the real top of RAM so we can put
	 * TCE's up there.
	 */

	alloc_top_high = ram_top;

	if (prom_memory_limit) {
		if (prom_memory_limit <= alloc_bottom) {
			prom_printf("Ignoring mem=%lx <= alloc_bottom.\n",
				    prom_memory_limit);
			prom_memory_limit = 0;
		} else if (prom_memory_limit >= ram_top) {
			prom_printf("Ignoring mem=%lx >= ram_top.\n",
				    prom_memory_limit);
			prom_memory_limit = 0;
		} else {
			ram_top = prom_memory_limit;
			rmo_top = min(rmo_top, prom_memory_limit);
		}
	}

	/*
	 * Setup our top alloc point, that is top of RMO or top of
	 * segment 0 when running non-LPAR.
	 * Some RS64 machines have buggy firmware where claims up at
	 * 1GB fail.  Cap at 768MB as a workaround.
	 * Since 768MB is plenty of room, and we need to cap to something
	 * reasonable on 32-bit, cap at 768MB on all machines.
	 */
	if (!rmo_top)
		rmo_top = ram_top;
	rmo_top = min(0x30000000ul, rmo_top);
	alloc_top = rmo_top;
	alloc_top_high = ram_top;

	/*
	 * Check if we have an initrd after the kernel but still inside
	 * the RMO.  If we do move our bottom point to after it.
	 */
	if (prom_initrd_start &&
	    prom_initrd_start < rmo_top &&
	    prom_initrd_end > alloc_bottom)
		alloc_bottom = PAGE_ALIGN(prom_initrd_end);

	prom_printf("memory layout at init:\n");
	prom_printf("  memory_limit : %lx (16 MB aligned)\n",
		    prom_memory_limit);
	prom_printf("  alloc_bottom : %lx\n", alloc_bottom);
	prom_printf("  alloc_top    : %lx\n", alloc_top);
	prom_printf("  alloc_top_hi : %lx\n", alloc_top_high);
	prom_printf("  rmo_top      : %lx\n", rmo_top);
	prom_printf("  ram_top      : %lx\n", ram_top);
}

static void __init prom_close_stdin(void)
{
	__be32 val;
	ihandle stdin;

	if (prom_getprop(prom.chosen, "stdin", &val, sizeof(val)) > 0) {
		stdin = be32_to_cpu(val);
		call_prom("close", 1, 0, stdin);
	}
}

#ifdef CONFIG_PPC_SVM
static int prom_rtas_hcall(uint64_t args)
{
	register uint64_t arg1 asm("r3") = H_RTAS;
	register uint64_t arg2 asm("r4") = args;

	asm volatile("sc 1\n" : "=r" (arg1) :
			"r" (arg1),
			"r" (arg2) :);
	return arg1;
}

static struct rtas_args __prombss os_term_args;

static void __init prom_rtas_os_term(char *str)
{
	phandle rtas_node;
	__be32 val;
	u32 token;

	prom_debug("%s: start...\n", __func__);
	rtas_node = call_prom("finddevice", 1, 1, ADDR("/rtas"));
	prom_debug("rtas_node: %x\n", rtas_node);
	if (!PHANDLE_VALID(rtas_node))
		return;

	val = 0;
	prom_getprop(rtas_node, "ibm,os-term", &val, sizeof(val));
	token = be32_to_cpu(val);
	prom_debug("ibm,os-term: %x\n", token);
	if (token == 0)
		prom_panic("Could not get token for ibm,os-term\n");
	os_term_args.token = cpu_to_be32(token);
	os_term_args.nargs = cpu_to_be32(1);
	os_term_args.nret = cpu_to_be32(1);
	os_term_args.args[0] = cpu_to_be32(__pa(str));
	prom_rtas_hcall((uint64_t)&os_term_args);
}
#endif /* CONFIG_PPC_SVM */

/*
 * Allocate room for and instantiate RTAS
 */
static void __init prom_instantiate_rtas(void)
{
	phandle rtas_node;
	ihandle rtas_inst;
	u32 base, entry = 0;
	__be32 val;
	u32 size = 0;

	prom_debug("prom_instantiate_rtas: start...\n");

	rtas_node = call_prom("finddevice", 1, 1, ADDR("/rtas"));
	prom_debug("rtas_node: %x\n", rtas_node);
	if (!PHANDLE_VALID(rtas_node))
		return;

	val = 0;
	prom_getprop(rtas_node, "rtas-size", &val, sizeof(size));
	size = be32_to_cpu(val);
	if (size == 0)
		return;

	base = alloc_down(size, PAGE_SIZE, 0);
	if (base == 0)
		prom_panic("Could not allocate memory for RTAS\n");

	rtas_inst = call_prom("open", 1, 1, ADDR("/rtas"));
	if (!IHANDLE_VALID(rtas_inst)) {
		prom_printf("opening rtas package failed (%x)\n", rtas_inst);
		return;
	}

	prom_printf("instantiating rtas at 0x%x...", base);

	if (call_prom_ret("call-method", 3, 2, &entry,
			  ADDR("instantiate-rtas"),
			  rtas_inst, base) != 0
	    || entry == 0) {
		prom_printf(" failed\n");
		return;
	}
	prom_printf(" done\n");

	reserve_mem(base, size);

	val = cpu_to_be32(base);
	prom_setprop(rtas_node, "/rtas", "linux,rtas-base",
		     &val, sizeof(val));
	val = cpu_to_be32(entry);
	prom_setprop(rtas_node, "/rtas", "linux,rtas-entry",
		     &val, sizeof(val));

	/* Check if it supports "query-cpu-stopped-state" */
	if (prom_getprop(rtas_node, "query-cpu-stopped-state",
			 &val, sizeof(val)) != PROM_ERROR)
		rtas_has_query_cpu_stopped = true;

	prom_debug("rtas base     = 0x%x\n", base);
	prom_debug("rtas entry    = 0x%x\n", entry);
	prom_debug("rtas size     = 0x%x\n", size);

	prom_debug("prom_instantiate_rtas: end...\n");
}

#ifdef CONFIG_PPC64
/*
 * Allocate room for and instantiate Stored Measurement Log (SML)
 */
static void __init prom_instantiate_sml(void)
{
	phandle ibmvtpm_node;
	ihandle ibmvtpm_inst;
	u32 entry = 0, size = 0, succ = 0;
	u64 base;
	__be32 val;

	prom_debug("prom_instantiate_sml: start...\n");

	ibmvtpm_node = call_prom("finddevice", 1, 1, ADDR("/vdevice/vtpm"));
	prom_debug("ibmvtpm_node: %x\n", ibmvtpm_node);
	if (!PHANDLE_VALID(ibmvtpm_node))
		return;

	ibmvtpm_inst = call_prom("open", 1, 1, ADDR("/vdevice/vtpm"));
	if (!IHANDLE_VALID(ibmvtpm_inst)) {
		prom_printf("opening vtpm package failed (%x)\n", ibmvtpm_inst);
		return;
	}

	if (prom_getprop(ibmvtpm_node, "ibm,sml-efi-reformat-supported",
			 &val, sizeof(val)) != PROM_ERROR) {
		if (call_prom_ret("call-method", 2, 2, &succ,
				  ADDR("reformat-sml-to-efi-alignment"),
				  ibmvtpm_inst) != 0 || succ == 0) {
			prom_printf("Reformat SML to EFI alignment failed\n");
			return;
		}

		if (call_prom_ret("call-method", 2, 2, &size,
				  ADDR("sml-get-allocated-size"),
				  ibmvtpm_inst) != 0 || size == 0) {
			prom_printf("SML get allocated size failed\n");
			return;
		}
	} else {
		if (call_prom_ret("call-method", 2, 2, &size,
				  ADDR("sml-get-handover-size"),
				  ibmvtpm_inst) != 0 || size == 0) {
			prom_printf("SML get handover size failed\n");
			return;
		}
	}

	base = alloc_down(size, PAGE_SIZE, 0);
	if (base == 0)
		prom_panic("Could not allocate memory for sml\n");

	prom_printf("instantiating sml at 0x%llx...", base);

	memset((void *)base, 0, size);

	if (call_prom_ret("call-method", 4, 2, &entry,
			  ADDR("sml-handover"),
			  ibmvtpm_inst, size, base) != 0 || entry == 0) {
		prom_printf("SML handover failed\n");
		return;
	}
	prom_printf(" done\n");

	reserve_mem(base, size);

	prom_setprop(ibmvtpm_node, "/vdevice/vtpm", "linux,sml-base",
		     &base, sizeof(base));
	prom_setprop(ibmvtpm_node, "/vdevice/vtpm", "linux,sml-size",
		     &size, sizeof(size));

	prom_debug("sml base     = 0x%llx\n", base);
	prom_debug("sml size     = 0x%x\n", size);

	prom_debug("prom_instantiate_sml: end...\n");
}

/*
 * Allocate room for and initialize TCE tables
 */
#ifdef __BIG_ENDIAN__
static void __init prom_initialize_tce_table(void)
{
	phandle node;
	ihandle phb_node;
	char compatible[64], type[64], model[64];
	char *path = prom_scratch;
	u64 base, align;
	u32 minalign, minsize;
	u64 tce_entry, *tce_entryp;
	u64 local_alloc_top, local_alloc_bottom;
	u64 i;

	if (prom_iommu_off)
		return;

	prom_debug("starting prom_initialize_tce_table\n");

	/* Cache current top of allocs so we reserve a single block */
	local_alloc_top = alloc_top_high;
	local_alloc_bottom = local_alloc_top;

	/* Search all nodes looking for PHBs. */
	for (node = 0; prom_next_node(&node); ) {
		compatible[0] = 0;
		type[0] = 0;
		model[0] = 0;
		prom_getprop(node, "compatible",
			     compatible, sizeof(compatible));
		prom_getprop(node, "device_type", type, sizeof(type));
		prom_getprop(node, "model", model, sizeof(model));

		if ((type[0] == 0) || (prom_strstr(type, "pci") == NULL))
			continue;

		/* Keep the old logic intact to avoid regression. */
		if (compatible[0] != 0) {
			if ((prom_strstr(compatible, "python") == NULL) &&
			    (prom_strstr(compatible, "Speedwagon") == NULL) &&
			    (prom_strstr(compatible, "Winnipeg") == NULL))
				continue;
		} else if (model[0] != 0) {
			if ((prom_strstr(model, "ython") == NULL) &&
			    (prom_strstr(model, "peedwagon") == NULL) &&
			    (prom_strstr(model, "innipeg") == NULL))
				continue;
		}

		if (prom_getprop(node, "tce-table-minalign", &minalign,
				 sizeof(minalign)) == PROM_ERROR)
			minalign = 0;
		if (prom_getprop(node, "tce-table-minsize", &minsize,
				 sizeof(minsize)) == PROM_ERROR)
			minsize = 4UL << 20;

		/*
		 * Even though we read what OF wants, we just set the table
		 * size to 4 MB.  This is enough to map 2GB of PCI DMA space.
		 * By doing this, we avoid the pitfalls of trying to DMA to
		 * MMIO space and the DMA alias hole.
		 */
		minsize = 4UL << 20;

		/* Align to the greater of the align or size */
		align = max(minalign, minsize);
		base = alloc_down(minsize, align, 1);
		if (base == 0)
			prom_panic("ERROR, cannot find space for TCE table.\n");
		if (base < local_alloc_bottom)
			local_alloc_bottom = base;

		/* It seems OF doesn't null-terminate the path :-( */
		memset(path, 0, sizeof(prom_scratch));
		/* Call OF to setup the TCE hardware */
		if (call_prom("package-to-path", 3, 1, node,
			      path, sizeof(prom_scratch) - 1) == PROM_ERROR) {
			prom_printf("package-to-path failed\n");
		}

		/* Save away the TCE table attributes for later use. */
		prom_setprop(node, path, "linux,tce-base", &base, sizeof(base));
		prom_setprop(node, path, "linux,tce-size", &minsize, sizeof(minsize));

		prom_debug("TCE table: %s\n", path);
		prom_debug("\tnode = 0x%x\n", node);
		prom_debug("\tbase = 0x%llx\n", base);
		prom_debug("\tsize = 0x%x\n", minsize);

		/* Initialize the table to have a one-to-one mapping
		 * over the allocated size.
		 */
		tce_entryp = (u64 *)base;
		for (i = 0; i < (minsize >> 3) ;tce_entryp++, i++) {
			tce_entry = (i << PAGE_SHIFT);
			tce_entry |= 0x3;
			*tce_entryp = tce_entry;
		}

		prom_printf("opening PHB %s", path);
		phb_node = call_prom("open", 1, 1, path);
		if (phb_node == 0)
			prom_printf("... failed\n");
		else
			prom_printf("... done\n");

		call_prom("call-method", 6, 0, ADDR("set-64-bit-addressing"),
			  phb_node, -1, minsize,
			  (u32) base, (u32) (base >> 32));
		call_prom("close", 1, 0, phb_node);
	}

	reserve_mem(local_alloc_bottom, local_alloc_top - local_alloc_bottom);

	/* These are only really needed if there is a memory limit in
	 * effect, but we don't know so export them always. */
	prom_tce_alloc_start = local_alloc_bottom;
	prom_tce_alloc_end = local_alloc_top;

	/* Flag the first invalid entry */
	prom_debug("ending prom_initialize_tce_table\n");
}
#endif /* __BIG_ENDIAN__ */
#endif /* CONFIG_PPC64 */

/*
 * With CHRP SMP we need to use the OF to start the other processors.
 * We can't wait until smp_boot_cpus (the OF is trashed by then)
 * so we have to put the processors into a holding pattern controlled
 * by the kernel (not OF) before we destroy the OF.
 *
 * This uses a chunk of low memory, puts some holding pattern
 * code there and sends the other processors off to there until
 * smp_boot_cpus tells them to do something.  The holding pattern
 * checks that address until its cpu # is there, when it is that
 * cpu jumps to __secondary_start().  smp_boot_cpus() takes care
 * of setting those values.
 *
 * We also use physical address 0x4 here to tell when a cpu
 * is in its holding pattern code.
 *
 * -- Cort
 */
/*
 * We want to reference the copy of __secondary_hold_* in the
 * 0 - 0x100 address range
 */
#define LOW_ADDR(x)	(((unsigned long) &(x)) & 0xff)

static void __init prom_hold_cpus(void)
{
	unsigned long i;
	phandle node;
	char type[64];
	unsigned long *spinloop
		= (void *) LOW_ADDR(__secondary_hold_spinloop);
	unsigned long *acknowledge
		= (void *) LOW_ADDR(__secondary_hold_acknowledge);
	unsigned long secondary_hold = LOW_ADDR(__secondary_hold);

	/*
	 * On pseries, if RTAS supports "query-cpu-stopped-state",
	 * we skip this stage, the CPUs will be started by the
	 * kernel using RTAS.
	 */
	if ((of_platform == PLATFORM_PSERIES ||
	     of_platform == PLATFORM_PSERIES_LPAR) &&
	    rtas_has_query_cpu_stopped) {
		prom_printf("prom_hold_cpus: skipped\n");
		return;
	}

	prom_debug("prom_hold_cpus: start...\n");
	prom_debug("    1) spinloop       = 0x%lx\n", (unsigned long)spinloop);
	prom_debug("    1) *spinloop      = 0x%lx\n", *spinloop);
	prom_debug("    1) acknowledge    = 0x%lx\n",
		   (unsigned long)acknowledge);
	prom_debug("    1) *acknowledge   = 0x%lx\n", *acknowledge);
	prom_debug("    1) secondary_hold = 0x%lx\n", secondary_hold);

	/* Set the common spinloop variable, so all of the secondary cpus
	 * will block when they are awakened from their OF spinloop.
	 * This must occur for both SMP and non SMP kernels, since OF will
	 * be trashed when we move the kernel.
	 */
	*spinloop = 0;

	/* look for cpus */
	for (node = 0; prom_next_node(&node); ) {
		unsigned int cpu_no;
		__be32 reg;

		type[0] = 0;
		prom_getprop(node, "device_type", type, sizeof(type));
		if (prom_strcmp(type, "cpu") != 0)
			continue;

		/* Skip non-configured cpus. */
		if (prom_getprop(node, "status", type, sizeof(type)) > 0)
			if (prom_strcmp(type, "okay") != 0)
				continue;

		reg = cpu_to_be32(-1); /* make sparse happy */
		prom_getprop(node, "reg", &reg, sizeof(reg));
		cpu_no = be32_to_cpu(reg);

		prom_debug("cpu hw idx   = %u\n", cpu_no);

		/* Init the acknowledge var which will be reset by
		 * the secondary cpu when it awakens from its OF
		 * spinloop.
		 */
		*acknowledge = (unsigned long)-1;

		if (cpu_no != prom.cpu) {
			/* Primary Thread of non-boot cpu or any thread */
			prom_printf("starting cpu hw idx %u... ", cpu_no);
			call_prom("start-cpu", 3, 0, node,
				  secondary_hold, cpu_no);

			for (i = 0; (i < 100000000) && 
			     (*acknowledge == ((unsigned long)-1)); i++ )
				mb();

			if (*acknowledge == cpu_no)
				prom_printf("done\n");
			else
				prom_printf("failed: %lx\n", *acknowledge);
		}
#ifdef CONFIG_SMP
		else
			prom_printf("boot cpu hw idx %u\n", cpu_no);
#endif /* CONFIG_SMP */
	}

	prom_debug("prom_hold_cpus: end...\n");
}


static void __init prom_init_client_services(unsigned long pp)
{
	/* Get a handle to the prom entry point before anything else */
	prom_entry = pp;

	/* get a handle for the stdout device */
	prom.chosen = call_prom("finddevice", 1, 1, ADDR("/chosen"));
	if (!PHANDLE_VALID(prom.chosen))
		prom_panic("cannot find chosen"); /* msg won't be printed :( */

	/* get device tree root */
	prom.root = call_prom("finddevice", 1, 1, ADDR("/"));
	if (!PHANDLE_VALID(prom.root))
		prom_panic("cannot find device tree root"); /* msg won't be printed :( */

	prom.mmumap = 0;
}

#ifdef CONFIG_PPC32
/*
 * For really old powermacs, we need to map things we claim.
 * For that, we need the ihandle of the mmu.
 * Also, on the longtrail, we need to work around other bugs.
 */
static void __init prom_find_mmu(void)
{
	phandle oprom;
	char version[64];

	oprom = call_prom("finddevice", 1, 1, ADDR("/openprom"));
	if (!PHANDLE_VALID(oprom))
		return;
	if (prom_getprop(oprom, "model", version, sizeof(version)) <= 0)
		return;
	version[sizeof(version) - 1] = 0;
	/* XXX might need to add other versions here */
	if (prom_strcmp(version, "Open Firmware, 1.0.5") == 0)
		of_workarounds = OF_WA_CLAIM;
	else if (prom_strncmp(version, "FirmWorks,3.", 12) == 0) {
		of_workarounds = OF_WA_CLAIM | OF_WA_LONGTRAIL;
		call_prom("interpret", 1, 1, "dev /memory 0 to allow-reclaim");
	} else
		return;
	prom.memory = call_prom("open", 1, 1, ADDR("/memory"));
	prom_getprop(prom.chosen, "mmu", &prom.mmumap,
		     sizeof(prom.mmumap));
	prom.mmumap = be32_to_cpu(prom.mmumap);
	if (!IHANDLE_VALID(prom.memory) || !IHANDLE_VALID(prom.mmumap))
		of_workarounds &= ~OF_WA_CLAIM;		/* hmmm */
}
#else
#define prom_find_mmu()
#endif

static void __init prom_init_stdout(void)
{
	char *path = of_stdout_device;
	char type[16];
	phandle stdout_node;
	__be32 val;

	if (prom_getprop(prom.chosen, "stdout", &val, sizeof(val)) <= 0)
		prom_panic("cannot find stdout");

	prom.stdout = be32_to_cpu(val);

	/* Get the full OF pathname of the stdout device */
	memset(path, 0, 256);
	call_prom("instance-to-path", 3, 1, prom.stdout, path, 255);
	prom_printf("OF stdout device is: %s\n", of_stdout_device);
	prom_setprop(prom.chosen, "/chosen", "linux,stdout-path",
		     path, prom_strlen(path) + 1);

	/* instance-to-package fails on PA-Semi */
	stdout_node = call_prom("instance-to-package", 1, 1, prom.stdout);
	if (stdout_node != PROM_ERROR) {
		val = cpu_to_be32(stdout_node);

		/* If it's a display, note it */
		memset(type, 0, sizeof(type));
		prom_getprop(stdout_node, "device_type", type, sizeof(type));
		if (prom_strcmp(type, "display") == 0)
			prom_setprop(stdout_node, path, "linux,boot-display", NULL, 0);
	}
}

static int __init prom_find_machine_type(void)
{
	char compat[256];
	int len, i = 0;
#ifdef CONFIG_PPC64
	phandle rtas;
	int x;
#endif

	/* Look for a PowerMac or a Cell */
	len = prom_getprop(prom.root, "compatible",
			   compat, sizeof(compat)-1);
	if (len > 0) {
		compat[len] = 0;
		while (i < len) {
			char *p = &compat[i];
			int sl = prom_strlen(p);
			if (sl == 0)
				break;
			if (prom_strstr(p, "Power Macintosh") ||
			    prom_strstr(p, "MacRISC"))
				return PLATFORM_POWERMAC;
#ifdef CONFIG_PPC64
			/* We must make sure we don't detect the IBM Cell
			 * blades as pSeries due to some firmware issues,
			 * so we do it here.
			 */
			if (prom_strstr(p, "IBM,CBEA") ||
			    prom_strstr(p, "IBM,CPBW-1.0"))
				return PLATFORM_GENERIC;
#endif /* CONFIG_PPC64 */
			i += sl + 1;
		}
	}
#ifdef CONFIG_PPC64
	/* Try to figure out if it's an IBM pSeries or any other
	 * PAPR compliant platform. We assume it is if :
	 *  - /device_type is "chrp" (please, do NOT use that for future
	 *    non-IBM designs !
	 *  - it has /rtas
	 */
	len = prom_getprop(prom.root, "device_type",
			   compat, sizeof(compat)-1);
	if (len <= 0)
		return PLATFORM_GENERIC;
	if (prom_strcmp(compat, "chrp"))
		return PLATFORM_GENERIC;

	/* Default to pSeries. We need to know if we are running LPAR */
	rtas = call_prom("finddevice", 1, 1, ADDR("/rtas"));
	if (!PHANDLE_VALID(rtas))
		return PLATFORM_GENERIC;
	x = prom_getproplen(rtas, "ibm,hypertas-functions");
	if (x != PROM_ERROR) {
		prom_debug("Hypertas detected, assuming LPAR !\n");
		return PLATFORM_PSERIES_LPAR;
	}
	return PLATFORM_PSERIES;
#else
	return PLATFORM_GENERIC;
#endif
}

static int __init prom_set_color(ihandle ih, int i, int r, int g, int b)
{
	return call_prom("call-method", 6, 1, ADDR("color!"), ih, i, b, g, r);
}

/*
 * If we have a display that we don't know how to drive,
 * we will want to try to execute OF's open method for it
 * later.  However, OF will probably fall over if we do that
 * we've taken over the MMU.
 * So we check whether we will need to open the display,
 * and if so, open it now.
 */
static void __init prom_check_displays(void)
{
	char type[16], *path;
	phandle node;
	ihandle ih;
	int i;

	static const unsigned char default_colors[] __initconst = {
		0x00, 0x00, 0x00,
		0x00, 0x00, 0xaa,
		0x00, 0xaa, 0x00,
		0x00, 0xaa, 0xaa,
		0xaa, 0x00, 0x00,
		0xaa, 0x00, 0xaa,
		0xaa, 0xaa, 0x00,
		0xaa, 0xaa, 0xaa,
		0x55, 0x55, 0x55,
		0x55, 0x55, 0xff,
		0x55, 0xff, 0x55,
		0x55, 0xff, 0xff,
		0xff, 0x55, 0x55,
		0xff, 0x55, 0xff,
		0xff, 0xff, 0x55,
		0xff, 0xff, 0xff
	};
	const unsigned char *clut;

	prom_debug("Looking for displays\n");
	for (node = 0; prom_next_node(&node); ) {
		memset(type, 0, sizeof(type));
		prom_getprop(node, "device_type", type, sizeof(type));
		if (prom_strcmp(type, "display") != 0)
			continue;

		/* It seems OF doesn't null-terminate the path :-( */
		path = prom_scratch;
		memset(path, 0, sizeof(prom_scratch));

		/*
		 * leave some room at the end of the path for appending extra
		 * arguments
		 */
		if (call_prom("package-to-path", 3, 1, node, path,
			      sizeof(prom_scratch) - 10) == PROM_ERROR)
			continue;
		prom_printf("found display   : %s, opening... ", path);
		
		ih = call_prom("open", 1, 1, path);
		if (ih == 0) {
			prom_printf("failed\n");
			continue;
		}

		/* Success */
		prom_printf("done\n");
		prom_setprop(node, path, "linux,opened", NULL, 0);

		/* Setup a usable color table when the appropriate
		 * method is available. Should update this to set-colors */
		clut = default_colors;
		for (i = 0; i < 16; i++, clut += 3)
			if (prom_set_color(ih, i, clut[0], clut[1],
					   clut[2]) != 0)
				break;

#ifdef CONFIG_LOGO_LINUX_CLUT224
		clut = PTRRELOC(logo_linux_clut224.clut);
		for (i = 0; i < logo_linux_clut224.clutsize; i++, clut += 3)
			if (prom_set_color(ih, i + 32, clut[0], clut[1],
					   clut[2]) != 0)
				break;
#endif /* CONFIG_LOGO_LINUX_CLUT224 */

#ifdef CONFIG_PPC_EARLY_DEBUG_BOOTX
		if (prom_getprop(node, "linux,boot-display", NULL, 0) !=
		    PROM_ERROR) {
			u32 width, height, pitch, addr;

			prom_printf("Setting btext !\n");

			if (prom_getprop(node, "width", &width, 4) == PROM_ERROR)
				return;

			if (prom_getprop(node, "height", &height, 4) == PROM_ERROR)
				return;

			if (prom_getprop(node, "linebytes", &pitch, 4) == PROM_ERROR)
				return;

			if (prom_getprop(node, "address", &addr, 4) == PROM_ERROR)
				return;

			prom_printf("W=%d H=%d LB=%d addr=0x%x\n",
				    width, height, pitch, addr);
			btext_setup_display(width, height, 8, pitch, addr);
			btext_prepare_BAT();
		}
#endif /* CONFIG_PPC_EARLY_DEBUG_BOOTX */
	}
}


/* Return (relocated) pointer to this much memory: moves initrd if reqd. */
static void __init *make_room(unsigned long *mem_start, unsigned long *mem_end,
			      unsigned long needed, unsigned long align)
{
	void *ret;

	*mem_start = ALIGN(*mem_start, align);
	while ((*mem_start + needed) > *mem_end) {
		unsigned long room, chunk;

		prom_debug("Chunk exhausted, claiming more at %lx...\n",
			   alloc_bottom);
		room = alloc_top - alloc_bottom;
		if (room > DEVTREE_CHUNK_SIZE)
			room = DEVTREE_CHUNK_SIZE;
		if (room < PAGE_SIZE)
			prom_panic("No memory for flatten_device_tree "
				   "(no room)\n");
		chunk = alloc_up(room, 0);
		if (chunk == 0)
			prom_panic("No memory for flatten_device_tree "
				   "(claim failed)\n");
		*mem_end = chunk + room;
	}

	ret = (void *)*mem_start;
	*mem_start += needed;

	return ret;
}

#define dt_push_token(token, mem_start, mem_end) do { 			\
		void *room = make_room(mem_start, mem_end, 4, 4);	\
		*(__be32 *)room = cpu_to_be32(token);			\
	} while(0)

static unsigned long __init dt_find_string(char *str)
{
	char *s, *os;

	s = os = (char *)dt_string_start;
	s += 4;
	while (s <  (char *)dt_string_end) {
		if (prom_strcmp(s, str) == 0)
			return s - os;
		s += prom_strlen(s) + 1;
	}
	return 0;
}

/*
 * The Open Firmware 1275 specification states properties must be 31 bytes or
 * less, however not all firmwares obey this. Make it 64 bytes to be safe.
 */
#define MAX_PROPERTY_NAME 64

static void __init scan_dt_build_strings(phandle node,
					 unsigned long *mem_start,
					 unsigned long *mem_end)
{
	char *prev_name, *namep, *sstart;
	unsigned long soff;
	phandle child;

	sstart =  (char *)dt_string_start;

	/* get and store all property names */
	prev_name = "";
	for (;;) {
		/* 64 is max len of name including nul. */
		namep = make_room(mem_start, mem_end, MAX_PROPERTY_NAME, 1);
		if (call_prom("nextprop", 3, 1, node, prev_name, namep) != 1) {
			/* No more nodes: unwind alloc */
			*mem_start = (unsigned long)namep;
			break;
		}

 		/* skip "name" */
		if (prom_strcmp(namep, "name") == 0) {
 			*mem_start = (unsigned long)namep;
 			prev_name = "name";
 			continue;
 		}
		/* get/create string entry */
		soff = dt_find_string(namep);
		if (soff != 0) {
			*mem_start = (unsigned long)namep;
			namep = sstart + soff;
		} else {
			/* Trim off some if we can */
			*mem_start = (unsigned long)namep + prom_strlen(namep) + 1;
			dt_string_end = *mem_start;
		}
		prev_name = namep;
	}

	/* do all our children */
	child = call_prom("child", 1, 1, node);
	while (child != 0) {
		scan_dt_build_strings(child, mem_start, mem_end);
		child = call_prom("peer", 1, 1, child);
	}
}

static void __init scan_dt_build_struct(phandle node, unsigned long *mem_start,
					unsigned long *mem_end)
{
	phandle child;
	char *namep, *prev_name, *sstart, *p, *ep, *lp, *path;
	unsigned long soff;
	unsigned char *valp;
	static char pname[MAX_PROPERTY_NAME] __prombss;
	int l, room, has_phandle = 0;

	dt_push_token(OF_DT_BEGIN_NODE, mem_start, mem_end);

	/* get the node's full name */
	namep = (char *)*mem_start;
	room = *mem_end - *mem_start;
	if (room > 255)
		room = 255;
	l = call_prom("package-to-path", 3, 1, node, namep, room);
	if (l >= 0) {
		/* Didn't fit?  Get more room. */
		if (l >= room) {
			if (l >= *mem_end - *mem_start)
				namep = make_room(mem_start, mem_end, l+1, 1);
			call_prom("package-to-path", 3, 1, node, namep, l);
		}
		namep[l] = '\0';

		/* Fixup an Apple bug where they have bogus \0 chars in the
		 * middle of the path in some properties, and extract
		 * the unit name (everything after the last '/').
		 */
		for (lp = p = namep, ep = namep + l; p < ep; p++) {
			if (*p == '/')
				lp = namep;
			else if (*p != 0)
				*lp++ = *p;
		}
		*lp = 0;
		*mem_start = ALIGN((unsigned long)lp + 1, 4);
	}

	/* get it again for debugging */
	path = prom_scratch;
	memset(path, 0, sizeof(prom_scratch));
	call_prom("package-to-path", 3, 1, node, path, sizeof(prom_scratch) - 1);

	/* get and store all properties */
	prev_name = "";
	sstart = (char *)dt_string_start;
	for (;;) {
		if (call_prom("nextprop", 3, 1, node, prev_name,
			      pname) != 1)
			break;

 		/* skip "name" */
		if (prom_strcmp(pname, "name") == 0) {
 			prev_name = "name";
 			continue;
 		}

		/* find string offset */
		soff = dt_find_string(pname);
		if (soff == 0) {
			prom_printf("WARNING: Can't find string index for"
				    " <%s>, node %s\n", pname, path);
			break;
		}
		prev_name = sstart + soff;

		/* get length */
		l = call_prom("getproplen", 2, 1, node, pname);

		/* sanity checks */
		if (l == PROM_ERROR)
			continue;

		/* push property head */
		dt_push_token(OF_DT_PROP, mem_start, mem_end);
		dt_push_token(l, mem_start, mem_end);
		dt_push_token(soff, mem_start, mem_end);

		/* push property content */
		valp = make_room(mem_start, mem_end, l, 4);
		call_prom("getprop", 4, 1, node, pname, valp, l);
		*mem_start = ALIGN(*mem_start, 4);

		if (!prom_strcmp(pname, "phandle"))
			has_phandle = 1;
	}

	/* Add a "phandle" property if none already exist */
	if (!has_phandle) {
		soff = dt_find_string("phandle");
		if (soff == 0)
			prom_printf("WARNING: Can't find string index for <phandle> node %s\n", path);
		else {
			dt_push_token(OF_DT_PROP, mem_start, mem_end);
			dt_push_token(4, mem_start, mem_end);
			dt_push_token(soff, mem_start, mem_end);
			valp = make_room(mem_start, mem_end, 4, 4);
			*(__be32 *)valp = cpu_to_be32(node);
		}
	}

	/* do all our children */
	child = call_prom("child", 1, 1, node);
	while (child != 0) {
		scan_dt_build_struct(child, mem_start, mem_end);
		child = call_prom("peer", 1, 1, child);
	}

	dt_push_token(OF_DT_END_NODE, mem_start, mem_end);
}

static void __init flatten_device_tree(void)
{
	phandle root;
	unsigned long mem_start, mem_end, room;
	struct boot_param_header *hdr;
	char *namep;
	u64 *rsvmap;

	/*
	 * Check how much room we have between alloc top & bottom (+/- a
	 * few pages), crop to 1MB, as this is our "chunk" size
	 */
	room = alloc_top - alloc_bottom - 0x4000;
	if (room > DEVTREE_CHUNK_SIZE)
		room = DEVTREE_CHUNK_SIZE;
	prom_debug("starting device tree allocs at %lx\n", alloc_bottom);

	/* Now try to claim that */
	mem_start = (unsigned long)alloc_up(room, PAGE_SIZE);
	if (mem_start == 0)
		prom_panic("Can't allocate initial device-tree chunk\n");
	mem_end = mem_start + room;

	/* Get root of tree */
	root = call_prom("peer", 1, 1, (phandle)0);
	if (root == (phandle)0)
		prom_panic ("couldn't get device tree root\n");

	/* Build header and make room for mem rsv map */ 
	mem_start = ALIGN(mem_start, 4);
	hdr = make_room(&mem_start, &mem_end,
			sizeof(struct boot_param_header), 4);
	dt_header_start = (unsigned long)hdr;
	rsvmap = make_room(&mem_start, &mem_end, sizeof(mem_reserve_map), 8);

	/* Start of strings */
	mem_start = PAGE_ALIGN(mem_start);
	dt_string_start = mem_start;
	mem_start += 4; /* hole */

	/* Add "phandle" in there, we'll need it */
	namep = make_room(&mem_start, &mem_end, 16, 1);
	prom_strcpy(namep, "phandle");
	mem_start = (unsigned long)namep + prom_strlen(namep) + 1;

	/* Build string array */
	prom_printf("Building dt strings...\n"); 
	scan_dt_build_strings(root, &mem_start, &mem_end);
	dt_string_end = mem_start;

	/* Build structure */
	mem_start = PAGE_ALIGN(mem_start);
	dt_struct_start = mem_start;
	prom_printf("Building dt structure...\n"); 
	scan_dt_build_struct(root, &mem_start, &mem_end);
	dt_push_token(OF_DT_END, &mem_start, &mem_end);
	dt_struct_end = PAGE_ALIGN(mem_start);

	/* Finish header */
	hdr->boot_cpuid_phys = cpu_to_be32(prom.cpu);
	hdr->magic = cpu_to_be32(OF_DT_HEADER);
	hdr->totalsize = cpu_to_be32(dt_struct_end - dt_header_start);
	hdr->off_dt_struct = cpu_to_be32(dt_struct_start - dt_header_start);
	hdr->off_dt_strings = cpu_to_be32(dt_string_start - dt_header_start);
	hdr->dt_strings_size = cpu_to_be32(dt_string_end - dt_string_start);
	hdr->off_mem_rsvmap = cpu_to_be32(((unsigned long)rsvmap) - dt_header_start);
	hdr->version = cpu_to_be32(OF_DT_VERSION);
	/* Version 16 is not backward compatible */
	hdr->last_comp_version = cpu_to_be32(0x10);

	/* Copy the reserve map in */
	memcpy(rsvmap, mem_reserve_map, sizeof(mem_reserve_map));

#ifdef DEBUG_PROM
	{
		int i;
		prom_printf("reserved memory map:\n");
		for (i = 0; i < mem_reserve_cnt; i++)
			prom_printf("  %llx - %llx\n",
				    be64_to_cpu(mem_reserve_map[i].base),
				    be64_to_cpu(mem_reserve_map[i].size));
	}
#endif
	/* Bump mem_reserve_cnt to cause further reservations to fail
	 * since it's too late.
	 */
	mem_reserve_cnt = MEM_RESERVE_MAP_SIZE;

	prom_printf("Device tree strings 0x%lx -> 0x%lx\n",
		    dt_string_start, dt_string_end);
	prom_printf("Device tree struct  0x%lx -> 0x%lx\n",
		    dt_struct_start, dt_struct_end);
}

#ifdef CONFIG_PPC_MAPLE
/* PIBS Version 1.05.0000 04/26/2005 has an incorrect /ht/isa/ranges property.
 * The values are bad, and it doesn't even have the right number of cells. */
static void __init fixup_device_tree_maple(void)
{
	phandle isa;
	u32 rloc = 0x01002000; /* IO space; PCI device = 4 */
	u32 isa_ranges[6];
	char *name;

	name = "/ht@0/isa@4";
	isa = call_prom("finddevice", 1, 1, ADDR(name));
	if (!PHANDLE_VALID(isa)) {
		name = "/ht@0/isa@6";
		isa = call_prom("finddevice", 1, 1, ADDR(name));
		rloc = 0x01003000; /* IO space; PCI device = 6 */
	}
	if (!PHANDLE_VALID(isa))
		return;

	if (prom_getproplen(isa, "ranges") != 12)
		return;
	if (prom_getprop(isa, "ranges", isa_ranges, sizeof(isa_ranges))
		== PROM_ERROR)
		return;

	if (isa_ranges[0] != 0x1 ||
		isa_ranges[1] != 0xf4000000 ||
		isa_ranges[2] != 0x00010000)
		return;

	prom_printf("Fixing up bogus ISA range on Maple/Apache...\n");

	isa_ranges[0] = 0x1;
	isa_ranges[1] = 0x0;
	isa_ranges[2] = rloc;
	isa_ranges[3] = 0x0;
	isa_ranges[4] = 0x0;
	isa_ranges[5] = 0x00010000;
	prom_setprop(isa, name, "ranges",
			isa_ranges, sizeof(isa_ranges));
}

#define CPC925_MC_START		0xf8000000
#define CPC925_MC_LENGTH	0x1000000
/* The values for memory-controller don't have right number of cells */
static void __init fixup_device_tree_maple_memory_controller(void)
{
	phandle mc;
	u32 mc_reg[4];
	char *name = "/hostbridge@f8000000";
	u32 ac, sc;

	mc = call_prom("finddevice", 1, 1, ADDR(name));
	if (!PHANDLE_VALID(mc))
		return;

	if (prom_getproplen(mc, "reg") != 8)
		return;

	prom_getprop(prom.root, "#address-cells", &ac, sizeof(ac));
	prom_getprop(prom.root, "#size-cells", &sc, sizeof(sc));
	if ((ac != 2) || (sc != 2))
		return;

	if (prom_getprop(mc, "reg", mc_reg, sizeof(mc_reg)) == PROM_ERROR)
		return;

	if (mc_reg[0] != CPC925_MC_START || mc_reg[1] != CPC925_MC_LENGTH)
		return;

	prom_printf("Fixing up bogus hostbridge on Maple...\n");

	mc_reg[0] = 0x0;
	mc_reg[1] = CPC925_MC_START;
	mc_reg[2] = 0x0;
	mc_reg[3] = CPC925_MC_LENGTH;
	prom_setprop(mc, name, "reg", mc_reg, sizeof(mc_reg));
}
#else
#define fixup_device_tree_maple()
#define fixup_device_tree_maple_memory_controller()
#endif

#ifdef CONFIG_PPC_CHRP
/*
 * Pegasos and BriQ lacks the "ranges" property in the isa node
 * Pegasos needs decimal IRQ 14/15, not hexadecimal
 * Pegasos has the IDE configured in legacy mode, but advertised as native
 */
static void __init fixup_device_tree_chrp(void)
{
	phandle ph;
	u32 prop[6];
	u32 rloc = 0x01006000; /* IO space; PCI device = 12 */
	char *name;
	int rc;

	name = "/pci@80000000/isa@c";
	ph = call_prom("finddevice", 1, 1, ADDR(name));
	if (!PHANDLE_VALID(ph)) {
		name = "/pci@ff500000/isa@6";
		ph = call_prom("finddevice", 1, 1, ADDR(name));
		rloc = 0x01003000; /* IO space; PCI device = 6 */
	}
	if (PHANDLE_VALID(ph)) {
		rc = prom_getproplen(ph, "ranges");
		if (rc == 0 || rc == PROM_ERROR) {
			prom_printf("Fixing up missing ISA range on Pegasos...\n");

			prop[0] = 0x1;
			prop[1] = 0x0;
			prop[2] = rloc;
			prop[3] = 0x0;
			prop[4] = 0x0;
			prop[5] = 0x00010000;
			prom_setprop(ph, name, "ranges", prop, sizeof(prop));
		}
	}

	name = "/pci@80000000/ide@C,1";
	ph = call_prom("finddevice", 1, 1, ADDR(name));
	if (PHANDLE_VALID(ph)) {
		prom_printf("Fixing up IDE interrupt on Pegasos...\n");
		prop[0] = 14;
		prop[1] = 0x0;
		prom_setprop(ph, name, "interrupts", prop, 2*sizeof(u32));
		prom_printf("Fixing up IDE class-code on Pegasos...\n");
		rc = prom_getprop(ph, "class-code", prop, sizeof(u32));
		if (rc == sizeof(u32)) {
			prop[0] &= ~0x5;
			prom_setprop(ph, name, "class-code", prop, sizeof(u32));
		}
	}
}
#else
#define fixup_device_tree_chrp()
#endif

#if defined(CONFIG_PPC64) && defined(CONFIG_PPC_PMAC)
static void __init fixup_device_tree_pmac(void)
{
	phandle u3, i2c, mpic;
	u32 u3_rev;
	u32 interrupts[2];
	u32 parent;

	/* Some G5s have a missing interrupt definition, fix it up here */
	u3 = call_prom("finddevice", 1, 1, ADDR("/u3@0,f8000000"));
	if (!PHANDLE_VALID(u3))
		return;
	i2c = call_prom("finddevice", 1, 1, ADDR("/u3@0,f8000000/i2c@f8001000"));
	if (!PHANDLE_VALID(i2c))
		return;
	mpic = call_prom("finddevice", 1, 1, ADDR("/u3@0,f8000000/mpic@f8040000"));
	if (!PHANDLE_VALID(mpic))
		return;

	/* check if proper rev of u3 */
	if (prom_getprop(u3, "device-rev", &u3_rev, sizeof(u3_rev))
	    == PROM_ERROR)
		return;
	if (u3_rev < 0x35 || u3_rev > 0x39)
		return;
	/* does it need fixup ? */
	if (prom_getproplen(i2c, "interrupts") > 0)
		return;

	prom_printf("fixing up bogus interrupts for u3 i2c...\n");

	/* interrupt on this revision of u3 is number 0 and level */
	interrupts[0] = 0;
	interrupts[1] = 1;
	prom_setprop(i2c, "/u3@0,f8000000/i2c@f8001000", "interrupts",
		     &interrupts, sizeof(interrupts));
	parent = (u32)mpic;
	prom_setprop(i2c, "/u3@0,f8000000/i2c@f8001000", "interrupt-parent",
		     &parent, sizeof(parent));
}
#else
#define fixup_device_tree_pmac()
#endif

#ifdef CONFIG_PPC_EFIKA
/*
 * The MPC5200 FEC driver requires an phy-handle property to tell it how
 * to talk to the phy.  If the phy-handle property is missing, then this
 * function is called to add the appropriate nodes and link it to the
 * ethernet node.
 */
static void __init fixup_device_tree_efika_add_phy(void)
{
	u32 node;
	char prop[64];
	int rv;

	/* Check if /builtin/ethernet exists - bail if it doesn't */
	node = call_prom("finddevice", 1, 1, ADDR("/builtin/ethernet"));
	if (!PHANDLE_VALID(node))
		return;

	/* Check if the phy-handle property exists - bail if it does */
	rv = prom_getprop(node, "phy-handle", prop, sizeof(prop));
	if (!rv)
		return;

	/*
	 * At this point the ethernet device doesn't have a phy described.
	 * Now we need to add the missing phy node and linkage
	 */

	/* Check for an MDIO bus node - if missing then create one */
	node = call_prom("finddevice", 1, 1, ADDR("/builtin/mdio"));
	if (!PHANDLE_VALID(node)) {
		prom_printf("Adding Ethernet MDIO node\n");
		call_prom("interpret", 1, 1,
			" s\" /builtin\" find-device"
			" new-device"
				" 1 encode-int s\" #address-cells\" property"
				" 0 encode-int s\" #size-cells\" property"
				" s\" mdio\" device-name"
				" s\" fsl,mpc5200b-mdio\" encode-string"
				" s\" compatible\" property"
				" 0xf0003000 0x400 reg"
				" 0x2 encode-int"
				" 0x5 encode-int encode+"
				" 0x3 encode-int encode+"
				" s\" interrupts\" property"
			" finish-device");
	}

	/* Check for a PHY device node - if missing then create one and
	 * give it's phandle to the ethernet node */
	node = call_prom("finddevice", 1, 1,
			 ADDR("/builtin/mdio/ethernet-phy"));
	if (!PHANDLE_VALID(node)) {
		prom_printf("Adding Ethernet PHY node\n");
		call_prom("interpret", 1, 1,
			" s\" /builtin/mdio\" find-device"
			" new-device"
				" s\" ethernet-phy\" device-name"
				" 0x10 encode-int s\" reg\" property"
				" my-self"
				" ihandle>phandle"
			" finish-device"
			" s\" /builtin/ethernet\" find-device"
				" encode-int"
				" s\" phy-handle\" property"
			" device-end");
	}
}

static void __init fixup_device_tree_efika(void)
{
	int sound_irq[3] = { 2, 2, 0 };
	int bcomm_irq[3*16] = { 3,0,0, 3,1,0, 3,2,0, 3,3,0,
				3,4,0, 3,5,0, 3,6,0, 3,7,0,
				3,8,0, 3,9,0, 3,10,0, 3,11,0,
				3,12,0, 3,13,0, 3,14,0, 3,15,0 };
	u32 node;
	char prop[64];
	int rv, len;

	/* Check if we're really running on a EFIKA */
	node = call_prom("finddevice", 1, 1, ADDR("/"));
	if (!PHANDLE_VALID(node))
		return;

	rv = prom_getprop(node, "model", prop, sizeof(prop));
	if (rv == PROM_ERROR)
		return;
	if (prom_strcmp(prop, "EFIKA5K2"))
		return;

	prom_printf("Applying EFIKA device tree fixups\n");

	/* Claiming to be 'chrp' is death */
	node = call_prom("finddevice", 1, 1, ADDR("/"));
	rv = prom_getprop(node, "device_type", prop, sizeof(prop));
	if (rv != PROM_ERROR && (prom_strcmp(prop, "chrp") == 0))
		prom_setprop(node, "/", "device_type", "efika", sizeof("efika"));

	/* CODEGEN,description is exposed in /proc/cpuinfo so
	   fix that too */
	rv = prom_getprop(node, "CODEGEN,description", prop, sizeof(prop));
	if (rv != PROM_ERROR && (prom_strstr(prop, "CHRP")))
		prom_setprop(node, "/", "CODEGEN,description",
			     "Efika 5200B PowerPC System",
			     sizeof("Efika 5200B PowerPC System"));

	/* Fixup bestcomm interrupts property */
	node = call_prom("finddevice", 1, 1, ADDR("/builtin/bestcomm"));
	if (PHANDLE_VALID(node)) {
		len = prom_getproplen(node, "interrupts");
		if (len == 12) {
			prom_printf("Fixing bestcomm interrupts property\n");
			prom_setprop(node, "/builtin/bestcom", "interrupts",
				     bcomm_irq, sizeof(bcomm_irq));
		}
	}

	/* Fixup sound interrupts property */
	node = call_prom("finddevice", 1, 1, ADDR("/builtin/sound"));
	if (PHANDLE_VALID(node)) {
		rv = prom_getprop(node, "interrupts", prop, sizeof(prop));
		if (rv == PROM_ERROR) {
			prom_printf("Adding sound interrupts property\n");
			prom_setprop(node, "/builtin/sound", "interrupts",
				     sound_irq, sizeof(sound_irq));
		}
	}

	/* Make sure ethernet phy-handle property exists */
	fixup_device_tree_efika_add_phy();
}
#else
#define fixup_device_tree_efika()
#endif

#ifdef CONFIG_PPC_PASEMI_NEMO
/*
 * CFE supplied on Nemo is broken in several ways, biggest
 * problem is that it reassigns ISA interrupts to unused mpic ints.
 * Add an interrupt-controller property for the io-bridge to use
 * and correct the ints so we can attach them to an irq_domain
 */
static void __init fixup_device_tree_pasemi(void)
{
	u32 interrupts[2], parent, rval, val = 0;
	char *name, *pci_name;
	phandle iob, node;

	/* Find the root pci node */
	name = "/pxp@0,e0000000";
	iob = call_prom("finddevice", 1, 1, ADDR(name));
	if (!PHANDLE_VALID(iob))
		return;

	/* check if interrupt-controller node set yet */
	if (prom_getproplen(iob, "interrupt-controller") !=PROM_ERROR)
		return;

	prom_printf("adding interrupt-controller property for SB600...\n");

	prom_setprop(iob, name, "interrupt-controller", &val, 0);

	pci_name = "/pxp@0,e0000000/pci@11";
	node = call_prom("finddevice", 1, 1, ADDR(pci_name));
	parent = ADDR(iob);

	for( ; prom_next_node(&node); ) {
		/* scan each node for one with an interrupt */
		if (!PHANDLE_VALID(node))
			continue;

		rval = prom_getproplen(node, "interrupts");
		if (rval == 0 || rval == PROM_ERROR)
			continue;

		prom_getprop(node, "interrupts", &interrupts, sizeof(interrupts));
		if ((interrupts[0] < 212) || (interrupts[0] > 222))
			continue;

		/* found a node, update both interrupts and interrupt-parent */
		if ((interrupts[0] >= 212) && (interrupts[0] <= 215))
			interrupts[0] -= 203;
		if ((interrupts[0] >= 216) && (interrupts[0] <= 220))
			interrupts[0] -= 213;
		if (interrupts[0] == 221)
			interrupts[0] = 14;
		if (interrupts[0] == 222)
			interrupts[0] = 8;

		prom_setprop(node, pci_name, "interrupts", interrupts,
					sizeof(interrupts));
		prom_setprop(node, pci_name, "interrupt-parent", &parent,
					sizeof(parent));
	}

	/*
	 * The io-bridge has device_type set to 'io-bridge' change it to 'isa'
	 * so that generic isa-bridge code can add the SB600 and its on-board
	 * peripherals.
	 */
	name = "/pxp@0,e0000000/io-bridge@0";
	iob = call_prom("finddevice", 1, 1, ADDR(name));
	if (!PHANDLE_VALID(iob))
		return;

	/* device_type is already set, just change it. */

	prom_printf("Changing device_type of SB600 node...\n");

	prom_setprop(iob, name, "device_type", "isa", sizeof("isa"));
}
#else	/* !CONFIG_PPC_PASEMI_NEMO */
static inline void fixup_device_tree_pasemi(void) { }
#endif

static void __init fixup_device_tree(void)
{
	fixup_device_tree_maple();
	fixup_device_tree_maple_memory_controller();
	fixup_device_tree_chrp();
	fixup_device_tree_pmac();
	fixup_device_tree_efika();
	fixup_device_tree_pasemi();
}

static void __init prom_find_boot_cpu(void)
{
	__be32 rval;
	ihandle prom_cpu;
	phandle cpu_pkg;

	rval = 0;
	if (prom_getprop(prom.chosen, "cpu", &rval, sizeof(rval)) <= 0)
		return;
	prom_cpu = be32_to_cpu(rval);

	cpu_pkg = call_prom("instance-to-package", 1, 1, prom_cpu);

	if (!PHANDLE_VALID(cpu_pkg))
		return;

	prom_getprop(cpu_pkg, "reg", &rval, sizeof(rval));
	prom.cpu = be32_to_cpu(rval);

	prom_debug("Booting CPU hw index = %d\n", prom.cpu);
}

static void __init prom_check_initrd(unsigned long r3, unsigned long r4)
{
#ifdef CONFIG_BLK_DEV_INITRD
	if (r3 && r4 && r4 != 0xdeadbeef) {
		__be64 val;

		prom_initrd_start = is_kernel_addr(r3) ? __pa(r3) : r3;
		prom_initrd_end = prom_initrd_start + r4;

		val = cpu_to_be64(prom_initrd_start);
		prom_setprop(prom.chosen, "/chosen", "linux,initrd-start",
			     &val, sizeof(val));
		val = cpu_to_be64(prom_initrd_end);
		prom_setprop(prom.chosen, "/chosen", "linux,initrd-end",
			     &val, sizeof(val));

		reserve_mem(prom_initrd_start,
			    prom_initrd_end - prom_initrd_start);

		prom_debug("initrd_start=0x%lx\n", prom_initrd_start);
		prom_debug("initrd_end=0x%lx\n", prom_initrd_end);
	}
#endif /* CONFIG_BLK_DEV_INITRD */
}

#ifdef CONFIG_PPC64
#ifdef CONFIG_RELOCATABLE
static void reloc_toc(void)
{
}

static void unreloc_toc(void)
{
}
#else
static void __reloc_toc(unsigned long offset, unsigned long nr_entries)
{
	unsigned long i;
	unsigned long *toc_entry;

	/* Get the start of the TOC by using r2 directly. */
	asm volatile("addi %0,2,-0x8000" : "=b" (toc_entry));

	for (i = 0; i < nr_entries; i++) {
		*toc_entry = *toc_entry + offset;
		toc_entry++;
	}
}

static void reloc_toc(void)
{
	unsigned long offset = reloc_offset();
	unsigned long nr_entries =
		(__prom_init_toc_end - __prom_init_toc_start) / sizeof(long);

	__reloc_toc(offset, nr_entries);

	mb();
}

static void unreloc_toc(void)
{
	unsigned long offset = reloc_offset();
	unsigned long nr_entries =
		(__prom_init_toc_end - __prom_init_toc_start) / sizeof(long);

	mb();

	__reloc_toc(-offset, nr_entries);
}
#endif
#endif

#ifdef CONFIG_PPC_SVM
/*
 * Perform the Enter Secure Mode ultracall.
 */
static int enter_secure_mode(unsigned long kbase, unsigned long fdt)
{
	register unsigned long r3 asm("r3") = UV_ESM;
	register unsigned long r4 asm("r4") = kbase;
	register unsigned long r5 asm("r5") = fdt;

	asm volatile("sc 2" : "+r"(r3) : "r"(r4), "r"(r5));

	return r3;
}

/*
 * Call the Ultravisor to transfer us to secure memory if we have an ESM blob.
 */
static void __init setup_secure_guest(unsigned long kbase, unsigned long fdt)
{
	int ret;

	if (!prom_svm_enable)
		return;

	/* Switch to secure mode. */
	prom_printf("Switching to secure mode.\n");

	/*
	 * The ultravisor will do an integrity check of the kernel image but we
	 * relocated it so the check will fail. Restore the original image by
	 * relocating it back to the kernel virtual base address.
	 */
	if (IS_ENABLED(CONFIG_RELOCATABLE))
		relocate(KERNELBASE);

	ret = enter_secure_mode(kbase, fdt);

	/* Relocate the kernel again. */
	if (IS_ENABLED(CONFIG_RELOCATABLE))
		relocate(kbase);

	if (ret != U_SUCCESS) {
		prom_printf("Returned %d from switching to secure mode.\n", ret);
		prom_rtas_os_term("Switch to secure mode failed.\n");
	}
}
#else
static void __init setup_secure_guest(unsigned long kbase, unsigned long fdt)
{
}
#endif /* CONFIG_PPC_SVM */

/*
 * We enter here early on, when the Open Firmware prom is still
 * handling exceptions and the MMU hash table for us.
 */

unsigned long __init prom_init(unsigned long r3, unsigned long r4,
			       unsigned long pp,
			       unsigned long r6, unsigned long r7,
			       unsigned long kbase)
{	
	unsigned long hdr;

#ifdef CONFIG_PPC32
	unsigned long offset = reloc_offset();
	reloc_got2(offset);
#else
	reloc_toc();
#endif

	/*
	 * First zero the BSS
	 */
	memset(&__bss_start, 0, __bss_stop - __bss_start);

	/*
	 * Init interface to Open Firmware, get some node references,
	 * like /chosen
	 */
	prom_init_client_services(pp);

	/*
	 * See if this OF is old enough that we need to do explicit maps
	 * and other workarounds
	 */
	prom_find_mmu();

	/*
	 * Init prom stdout device
	 */
	prom_init_stdout();

	prom_printf("Preparing to boot %s", linux_banner);

	/*
	 * Get default machine type. At this point, we do not differentiate
	 * between pSeries SMP and pSeries LPAR
	 */
	of_platform = prom_find_machine_type();
	prom_printf("Detected machine type: %x\n", of_platform);

#ifndef CONFIG_NONSTATIC_KERNEL
	/* Bail if this is a kdump kernel. */
	if (PHYSICAL_START > 0)
		prom_panic("Error: You can't boot a kdump kernel from OF!\n");
#endif

	/*
	 * Check for an initrd
	 */
	prom_check_initrd(r3, r4);

	/*
	 * Do early parsing of command line
	 */
	early_cmdline_parse();

#ifdef CONFIG_PPC_PSERIES
	/*
	 * On pSeries, inform the firmware about our capabilities
	 */
	if (of_platform == PLATFORM_PSERIES ||
	    of_platform == PLATFORM_PSERIES_LPAR)
		prom_send_capabilities();
#endif

	/*
	 * Copy the CPU hold code
	 */
	if (of_platform != PLATFORM_POWERMAC)
		copy_and_flush(0, kbase, 0x100, 0);

	/*
	 * Initialize memory management within prom_init
	 */
	prom_init_mem();

	/*
	 * Determine which cpu is actually running right _now_
	 */
	prom_find_boot_cpu();

	/* 
	 * Initialize display devices
	 */
	prom_check_displays();

#if defined(CONFIG_PPC64) && defined(__BIG_ENDIAN__)
	/*
	 * Initialize IOMMU (TCE tables) on pSeries. Do that before anything else
	 * that uses the allocator, we need to make sure we get the top of memory
	 * available for us here...
	 */
	if (of_platform == PLATFORM_PSERIES)
		prom_initialize_tce_table();
#endif

	/*
	 * On non-powermacs, try to instantiate RTAS. PowerMacs don't
	 * have a usable RTAS implementation.
	 */
	if (of_platform != PLATFORM_POWERMAC)
		prom_instantiate_rtas();

#ifdef CONFIG_PPC64
	/* instantiate sml */
	prom_instantiate_sml();
#endif

	/*
	 * On non-powermacs, put all CPUs in spin-loops.
	 *
	 * PowerMacs use a different mechanism to spin CPUs
	 *
	 * (This must be done after instanciating RTAS)
	 */
	if (of_platform != PLATFORM_POWERMAC)
		prom_hold_cpus();

	/*
	 * Fill in some infos for use by the kernel later on
	 */
	if (prom_memory_limit) {
		__be64 val = cpu_to_be64(prom_memory_limit);
		prom_setprop(prom.chosen, "/chosen", "linux,memory-limit",
			     &val, sizeof(val));
	}
#ifdef CONFIG_PPC64
	if (prom_iommu_off)
		prom_setprop(prom.chosen, "/chosen", "linux,iommu-off",
			     NULL, 0);

	if (prom_iommu_force_on)
		prom_setprop(prom.chosen, "/chosen", "linux,iommu-force-on",
			     NULL, 0);

	if (prom_tce_alloc_start) {
		prom_setprop(prom.chosen, "/chosen", "linux,tce-alloc-start",
			     &prom_tce_alloc_start,
			     sizeof(prom_tce_alloc_start));
		prom_setprop(prom.chosen, "/chosen", "linux,tce-alloc-end",
			     &prom_tce_alloc_end,
			     sizeof(prom_tce_alloc_end));
	}
#endif

	/*
	 * Fixup any known bugs in the device-tree
	 */
	fixup_device_tree();

	/*
	 * Now finally create the flattened device-tree
	 */
	prom_printf("copying OF device tree...\n");
	flatten_device_tree();

	/*
	 * in case stdin is USB and still active on IBM machines...
	 * Unfortunately quiesce crashes on some powermacs if we have
	 * closed stdin already (in particular the powerbook 101).
	 */
	if (of_platform != PLATFORM_POWERMAC)
		prom_close_stdin();

	/*
	 * Call OF "quiesce" method to shut down pending DMA's from
	 * devices etc...
	 */
	prom_printf("Quiescing Open Firmware ...\n");
	call_prom("quiesce", 0, 0);

	/*
	 * And finally, call the kernel passing it the flattened device
	 * tree and NULL as r5, thus triggering the new entry point which
	 * is common to us and kexec
	 */
	hdr = dt_header_start;

	prom_printf("Booting Linux via __start() @ 0x%lx ...\n", kbase);
	prom_debug("->dt_header_start=0x%lx\n", hdr);

#ifdef CONFIG_PPC32
	reloc_got2(-offset);
#else
	unreloc_toc();
#endif

	/* Move to secure memory if we're supposed to be secure guests. */
	setup_secure_guest(kbase, hdr);

	__start(hdr, kbase, 0, 0, 0, 0, 0);

	return 0;
}
