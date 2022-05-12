// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Hangzhou C-SKY Microsystems co.,ltd.
 * Mapping of DWARF debug register numbers into register names.
 */

#include <stddef.h>
#include <errno.h> /* for EINVAL */
#include <string.h> /* for strcmp */
#include <dwarf-regs.h>

struct pt_regs_dwarfnum {
	const char *name;
	unsigned int dwarfnum;
};

#define REG_DWARFNUM_NAME(r, num) {.name = r, .dwarfnum = num}
#define REG_DWARFNUM_END {.name = NULL, .dwarfnum = 0}

struct pt_regs_dwarfnum riscv_dwarf_regs_table[] = {
	REG_DWARFNUM_NAME("%zero", 0),
	REG_DWARFNUM_NAME("%ra", 1),
	REG_DWARFNUM_NAME("%sp", 2),
	REG_DWARFNUM_NAME("%gp", 3),
	REG_DWARFNUM_NAME("%tp", 4),
	REG_DWARFNUM_NAME("%t0", 5),
	REG_DWARFNUM_NAME("%t1", 6),
	REG_DWARFNUM_NAME("%t2", 7),
	REG_DWARFNUM_NAME("%s0", 8),
	REG_DWARFNUM_NAME("%s1", 9),
	REG_DWARFNUM_NAME("%a0", 10),
	REG_DWARFNUM_NAME("%a1", 11),
	REG_DWARFNUM_NAME("%a2", 12),
	REG_DWARFNUM_NAME("%a3", 13),
	REG_DWARFNUM_NAME("%a4", 14),
	REG_DWARFNUM_NAME("%a5", 15),
	REG_DWARFNUM_NAME("%a6", 16),
	REG_DWARFNUM_NAME("%a7", 17),
	REG_DWARFNUM_NAME("%s2", 18),
	REG_DWARFNUM_NAME("%s3", 19),
	REG_DWARFNUM_NAME("%s4", 20),
	REG_DWARFNUM_NAME("%s5", 21),
	REG_DWARFNUM_NAME("%s6", 22),
	REG_DWARFNUM_NAME("%s7", 23),
	REG_DWARFNUM_NAME("%s8", 24),
	REG_DWARFNUM_NAME("%s9", 25),
	REG_DWARFNUM_NAME("%s10", 26),
	REG_DWARFNUM_NAME("%s11", 27),
	REG_DWARFNUM_NAME("%t3", 28),
	REG_DWARFNUM_NAME("%t4", 29),
	REG_DWARFNUM_NAME("%t5", 30),
	REG_DWARFNUM_NAME("%t6", 31),
	REG_DWARFNUM_END,
};

#define RISCV_MAX_REGS ((sizeof(riscv_dwarf_regs_table) / \
		 sizeof(riscv_dwarf_regs_table[0])) - 1)

const char *get_arch_regstr(unsigned int n)
{
	return (n < RISCV_MAX_REGS) ? riscv_dwarf_regs_table[n].name : NULL;
}

int regs_query_register_offset(const char *name)
{
	const struct pt_regs_dwarfnum *roff;

	for (roff = riscv_dwarf_regs_table; roff->name; roff++)
		if (!strcmp(roff->name, name))
			return roff->dwarfnum;
	return -EINVAL;
}
