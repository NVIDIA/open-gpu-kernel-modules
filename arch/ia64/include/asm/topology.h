/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright (C) 2002, Erich Focht, NEC
 *
 * All rights reserved.
 */
#ifndef _ASM_IA64_TOPOLOGY_H
#define _ASM_IA64_TOPOLOGY_H

#include <asm/acpi.h>
#include <asm/numa.h>
#include <asm/smp.h>

#ifdef CONFIG_NUMA

/* Nodes w/o CPUs are preferred for memory allocations, see build_zonelists */
#define PENALTY_FOR_NODE_WITH_CPUS 255

/*
 * Nodes within this distance are eligible for reclaim by zone_reclaim() when
 * zone_reclaim_mode is enabled.
 */
#define RECLAIM_DISTANCE 15

/*
 * Returns a bitmask of CPUs on Node 'node'.
 */
#define cpumask_of_node(node) ((node) == -1 ?				\
			       cpu_all_mask :				\
			       &node_to_cpu_mask[node])

/*
 * Determines the node for a given pci bus
 */
#define pcibus_to_node(bus) PCI_CONTROLLER(bus)->node

void build_cpu_to_node_map(void);

#endif /* CONFIG_NUMA */

#ifdef CONFIG_SMP
#define topology_physical_package_id(cpu)	(cpu_data(cpu)->socket_id)
#define topology_core_id(cpu)			(cpu_data(cpu)->core_id)
#define topology_core_cpumask(cpu)		(&cpu_core_map[cpu])
#define topology_sibling_cpumask(cpu)		(&per_cpu(cpu_sibling_map, cpu))
#endif

extern void arch_fix_phys_package_id(int num, u32 slot);

#define cpumask_of_pcibus(bus)	(pcibus_to_node(bus) == -1 ?		\
				 cpu_all_mask :				\
				 cpumask_of_node(pcibus_to_node(bus)))

#include <asm-generic/topology.h>

#endif /* _ASM_IA64_TOPOLOGY_H */
