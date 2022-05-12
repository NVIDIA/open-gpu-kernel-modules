/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * This file contains NUMA specific prototypes and definitions.
 *
 * 2002/08/05 Erich Focht <efocht@ess.nec.de>
 *
 */
#ifndef _ASM_IA64_NUMA_H
#define _ASM_IA64_NUMA_H


#ifdef CONFIG_NUMA

#include <linux/cache.h>
#include <linux/cpumask.h>
#include <linux/numa.h>
#include <linux/smp.h>
#include <linux/threads.h>

#include <asm/mmzone.h>

extern u16 cpu_to_node_map[NR_CPUS] __cacheline_aligned;
extern cpumask_t node_to_cpu_mask[MAX_NUMNODES] __cacheline_aligned;
extern pg_data_t *pgdat_list[MAX_NUMNODES];

/* Stuff below this line could be architecture independent */

extern int num_node_memblks;		/* total number of memory chunks */

/*
 * List of node memory chunks. Filled when parsing SRAT table to
 * obtain information about memory nodes.
*/

struct node_memblk_s {
	unsigned long start_paddr;
	unsigned long size;
	int nid;		/* which logical node contains this chunk? */
	int bank;		/* which mem bank on this node */
};

struct node_cpuid_s {
	u16	phys_id;	/* id << 8 | eid */
	int	nid;		/* logical node containing this CPU */
};

extern struct node_memblk_s node_memblk[NR_NODE_MEMBLKS];
extern struct node_cpuid_s node_cpuid[NR_CPUS];

/*
 * ACPI 2.0 SLIT (System Locality Information Table)
 * http://devresource.hp.com/devresource/Docs/TechPapers/IA64/slit.pdf
 *
 * This is a matrix with "distances" between nodes, they should be
 * proportional to the memory access latency ratios.
 */

extern u8 numa_slit[MAX_NUMNODES * MAX_NUMNODES];
#define slit_distance(from,to) (numa_slit[(from) * MAX_NUMNODES + (to)])
extern int __node_distance(int from, int to);
#define node_distance(from,to) __node_distance(from, to)

extern int paddr_to_nid(unsigned long paddr);

#define local_nodeid (cpu_to_node_map[smp_processor_id()])

#define numa_off     0

extern void map_cpu_to_node(int cpu, int nid);
extern void unmap_cpu_from_node(int cpu, int nid);
extern void numa_clear_node(int cpu);

#else /* !CONFIG_NUMA */
#define map_cpu_to_node(cpu, nid)	do{}while(0)
#define unmap_cpu_from_node(cpu, nid)	do{}while(0)
#define paddr_to_nid(addr)	0
#define numa_clear_node(cpu)	do { } while (0)
#endif /* CONFIG_NUMA */

#endif /* _ASM_IA64_NUMA_H */
