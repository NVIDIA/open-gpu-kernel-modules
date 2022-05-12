/* SPDX-License-Identifier: GPL-2.0 */
/* oplib.h:  Describes the interface and available routines in the
 *           Linux Prom library.
 *
 * Copyright (C) 1995, 2007 David S. Miller (davem@davemloft.net)
 * Copyright (C) 1996 Jakub Jelinek (jj@sunsite.mff.cuni.cz)
 */

#ifndef __SPARC64_OPLIB_H
#define __SPARC64_OPLIB_H

#include <asm/openprom.h>

/* OBP version string. */
extern char prom_version[];

/* Root node of the prom device tree, this stays constant after
 * initialization is complete.
 */
extern phandle prom_root_node;

/* PROM stdout */
extern int prom_stdout;

/* /chosen node of the prom device tree, this stays constant after
 * initialization is complete.
 */
extern phandle prom_chosen_node;

/* Helper values and strings in arch/sparc64/kernel/head.S */
extern const char prom_peer_name[];
extern const char prom_compatible_name[];
extern const char prom_root_compatible[];
extern const char prom_cpu_compatible[];
extern const char prom_finddev_name[];
extern const char prom_chosen_path[];
extern const char prom_cpu_path[];
extern const char prom_getprop_name[];
extern const char prom_mmu_name[];
extern const char prom_callmethod_name[];
extern const char prom_translate_name[];
extern const char prom_map_name[];
extern const char prom_unmap_name[];
extern int prom_mmu_ihandle_cache;
extern unsigned int prom_boot_mapped_pc;
extern unsigned int prom_boot_mapping_mode;
extern unsigned long prom_boot_mapping_phys_high, prom_boot_mapping_phys_low;

struct linux_mlist_p1275 {
	struct linux_mlist_p1275 *theres_more;
	unsigned long start_adr;
	unsigned long num_bytes;
};

struct linux_mem_p1275 {
	struct linux_mlist_p1275 **p1275_totphys;
	struct linux_mlist_p1275 **p1275_prommap;
	struct linux_mlist_p1275 **p1275_available; /* What we can use */
};

/* The functions... */

/* You must call prom_init() before using any of the library services,
 * preferably as early as possible.  Pass it the romvec pointer.
 */
void prom_init(void *cif_handler);
void prom_init_report(void);

/* Boot argument acquisition, returns the boot command line string. */
char *prom_getbootargs(void);

/* Miscellaneous routines, don't really fit in any category per se. */

/* Reboot the machine with the command line passed. */
void prom_reboot(const char *boot_command);

/* Evaluate the forth string passed. */
void prom_feval(const char *forth_string);

/* Enter the prom, with possibility of continuation with the 'go'
 * command in newer proms.
 */
void prom_cmdline(void);

/* Enter the prom, with no chance of continuation for the stand-alone
 * which calls this.
 */
void prom_halt(void) __attribute__ ((noreturn));

/* Halt and power-off the machine. */
void prom_halt_power_off(void) __attribute__ ((noreturn));

/* Acquire the IDPROM of the root node in the prom device tree.  This
 * gets passed a buffer where you would like it stuffed.  The return value
 * is the format type of this idprom or 0xff on error.
 */
unsigned char prom_get_idprom(char *idp_buffer, int idpbuf_size);

/* Write a buffer of characters to the console. */
void prom_console_write_buf(const char *buf, int len);

/* Prom's internal routines, don't use in kernel/boot code. */
__printf(1, 2) void prom_printf(const char *fmt, ...);
void prom_write(const char *buf, unsigned int len);

/* Multiprocessor operations... */
#ifdef CONFIG_SMP
/* Start the CPU with the given device tree node at the passed program
 * counter with the given arg passed in via register %o0.
 */
void prom_startcpu(int cpunode, unsigned long pc, unsigned long arg);

/* Start the CPU with the given cpu ID at the passed program
 * counter with the given arg passed in via register %o0.
 */
void prom_startcpu_cpuid(int cpuid, unsigned long pc, unsigned long arg);

/* Stop the CPU with the given cpu ID.  */
void prom_stopcpu_cpuid(int cpuid);

/* Stop the current CPU. */
void prom_stopself(void);

/* Idle the current CPU. */
void prom_idleself(void);

/* Resume the CPU with the passed device tree node. */
void prom_resumecpu(int cpunode);
#endif

/* Power management interfaces. */

/* Put the current CPU to sleep. */
void prom_sleepself(void);

/* Put the entire system to sleep. */
int prom_sleepsystem(void);

/* Initiate a wakeup event. */
int prom_wakeupsystem(void);

/* MMU and memory related OBP interfaces. */

/* Get unique string identifying SIMM at given physical address. */
int prom_getunumber(int syndrome_code,
		    unsigned long phys_addr,
		    char *buf, int buflen);

/* Retain physical memory to the caller across soft resets. */
int prom_retain(const char *name, unsigned long size,
		unsigned long align, unsigned long *paddr);

/* Load explicit I/D TLB entries into the calling processor. */
long prom_itlb_load(unsigned long index,
		    unsigned long tte_data,
		    unsigned long vaddr);

long prom_dtlb_load(unsigned long index,
		    unsigned long tte_data,
		    unsigned long vaddr);

/* Map/Unmap client program address ranges.  First the format of
 * the mapping mode argument.
 */
#define PROM_MAP_WRITE	0x0001 /* Writable */
#define PROM_MAP_READ	0x0002 /* Readable - sw */
#define PROM_MAP_EXEC	0x0004 /* Executable - sw */
#define PROM_MAP_LOCKED	0x0010 /* Locked, use i/dtlb load calls for this instead */
#define PROM_MAP_CACHED	0x0020 /* Cacheable in both L1 and L2 caches */
#define PROM_MAP_SE	0x0040 /* Side-Effects */
#define PROM_MAP_GLOB	0x0080 /* Global */
#define PROM_MAP_IE	0x0100 /* Invert-Endianness */
#define PROM_MAP_DEFAULT (PROM_MAP_WRITE | PROM_MAP_READ | PROM_MAP_EXEC | PROM_MAP_CACHED)

int prom_map(int mode, unsigned long size,
	     unsigned long vaddr, unsigned long paddr);
void prom_unmap(unsigned long size, unsigned long vaddr);


/* PROM device tree traversal functions... */

/* Get the child node of the given node, or zero if no child exists. */
phandle prom_getchild(phandle parent_node);

/* Get the next sibling node of the given node, or zero if no further
 * siblings exist.
 */
phandle prom_getsibling(phandle node);

/* Get the length, at the passed node, of the given property type.
 * Returns -1 on error (ie. no such property at this node).
 */
int prom_getproplen(phandle thisnode, const char *property);

/* Fetch the requested property using the given buffer.  Returns
 * the number of bytes the prom put into your buffer or -1 on error.
 */
int prom_getproperty(phandle thisnode, const char *property,
		     char *prop_buffer, int propbuf_size);

/* Acquire an integer property. */
int prom_getint(phandle node, const char *property);

/* Acquire an integer property, with a default value. */
int prom_getintdefault(phandle node, const char *property, int defval);

/* Acquire a boolean property, 0=FALSE 1=TRUE. */
int prom_getbool(phandle node, const char *prop);

/* Acquire a string property, null string on error. */
void prom_getstring(phandle node, const char *prop, char *buf,
		    int bufsize);

/* Does the passed node have the given "name"? YES=1 NO=0 */
int prom_nodematch(phandle thisnode, const char *name);

/* Search all siblings starting at the passed node for "name" matching
 * the given string.  Returns the node on success, zero on failure.
 */
phandle prom_searchsiblings(phandle node_start, const char *name);

/* Return the first property type, as a string, for the given node.
 * Returns a null string on error. Buffer should be at least 32B long.
 */
char *prom_firstprop(phandle node, char *buffer);

/* Returns the next property after the passed property for the given
 * node.  Returns null string on failure. Buffer should be at least 32B long.
 */
char *prom_nextprop(phandle node, const char *prev_property, char *buf);

/* Returns 1 if the specified node has given property. */
int prom_node_has_property(phandle node, const char *property);

/* Returns phandle of the path specified */
phandle prom_finddevice(const char *name);

/* Set the indicated property at the given node with the passed value.
 * Returns the number of bytes of your value that the prom took.
 */
int prom_setprop(phandle node, const char *prop_name, char *prop_value,
		 int value_size);

phandle prom_inst2pkg(int);
void prom_sun4v_guest_soft_state(void);

int prom_ihandle2path(int handle, char *buffer, int bufsize);

/* Client interface level routines. */
void p1275_cmd_direct(unsigned long *);

#endif /* !(__SPARC64_OPLIB_H) */
