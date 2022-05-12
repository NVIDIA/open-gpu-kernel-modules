// SPDX-License-Identifier: GPL-2.0-only
/*
 * Mapping of DWARF debug register numbers into register names.
 *
 * Copyright (C) 2010 Will Deacon, ARM Ltd.
 */

#include <errno.h>
#include <stddef.h>
#include <string.h>
#include <dwarf-regs.h>
#include <linux/ptrace.h> /* for struct user_pt_regs */
#include <linux/stringify.h>

struct pt_regs_dwarfnum {
	const char *name;
	unsigned int dwarfnum;
};

#define REG_DWARFNUM_NAME(r, num) {.name = r, .dwarfnum = num}
#define GPR_DWARFNUM_NAME(num) \
	{.name = __stringify(%x##num), .dwarfnum = num}
#define REG_DWARFNUM_END {.name = NULL, .dwarfnum = 0}
#define DWARFNUM2OFFSET(index) \
	(index * sizeof((struct user_pt_regs *)0)->regs[0])

/*
 * Reference:
 * http://infocenter.arm.com/help/topic/com.arm.doc.ihi0057b/IHI0057B_aadwarf64.pdf
 */
static const struct pt_regs_dwarfnum regdwarfnum_table[] = {
	GPR_DWARFNUM_NAME(0),
	GPR_DWARFNUM_NAME(1),
	GPR_DWARFNUM_NAME(2),
	GPR_DWARFNUM_NAME(3),
	GPR_DWARFNUM_NAME(4),
	GPR_DWARFNUM_NAME(5),
	GPR_DWARFNUM_NAME(6),
	GPR_DWARFNUM_NAME(7),
	GPR_DWARFNUM_NAME(8),
	GPR_DWARFNUM_NAME(9),
	GPR_DWARFNUM_NAME(10),
	GPR_DWARFNUM_NAME(11),
	GPR_DWARFNUM_NAME(12),
	GPR_DWARFNUM_NAME(13),
	GPR_DWARFNUM_NAME(14),
	GPR_DWARFNUM_NAME(15),
	GPR_DWARFNUM_NAME(16),
	GPR_DWARFNUM_NAME(17),
	GPR_DWARFNUM_NAME(18),
	GPR_DWARFNUM_NAME(19),
	GPR_DWARFNUM_NAME(20),
	GPR_DWARFNUM_NAME(21),
	GPR_DWARFNUM_NAME(22),
	GPR_DWARFNUM_NAME(23),
	GPR_DWARFNUM_NAME(24),
	GPR_DWARFNUM_NAME(25),
	GPR_DWARFNUM_NAME(26),
	GPR_DWARFNUM_NAME(27),
	GPR_DWARFNUM_NAME(28),
	GPR_DWARFNUM_NAME(29),
	REG_DWARFNUM_NAME("%lr", 30),
	REG_DWARFNUM_NAME("%sp", 31),
	REG_DWARFNUM_END,
};

/**
 * get_arch_regstr() - lookup register name from it's DWARF register number
 * @n:	the DWARF register number
 *
 * get_arch_regstr() returns the name of the register in struct
 * regdwarfnum_table from it's DWARF register number. If the register is not
 * found in the table, this returns NULL;
 */
const char *get_arch_regstr(unsigned int n)
{
	const struct pt_regs_dwarfnum *roff;
	for (roff = regdwarfnum_table; roff->name != NULL; roff++)
		if (roff->dwarfnum == n)
			return roff->name;
	return NULL;
}

int regs_query_register_offset(const char *name)
{
	const struct pt_regs_dwarfnum *roff;

	for (roff = regdwarfnum_table; roff->name != NULL; roff++)
		if (!strcmp(roff->name, name))
			return DWARFNUM2OFFSET(roff->dwarfnum);
	return -EINVAL;
}
