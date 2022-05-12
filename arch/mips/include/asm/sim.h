/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 1999, 2000, 2003 Ralf Baechle
 * Copyright (C) 1999, 2000 Silicon Graphics, Inc.
 */
#ifndef _ASM_SIM_H
#define _ASM_SIM_H


#include <asm/asm-offsets.h>

#define __str2(x) #x
#define __str(x) __str2(x)

#ifdef CONFIG_32BIT

#define save_static_function(symbol)					\
__asm__(								\
	".text\n\t"							\
	".globl\t__" #symbol "\n\t"					\
	".align\t2\n\t"							\
	".type\t__" #symbol ", @function\n\t"				\
	".ent\t__" #symbol ", 0\n__"					\
	#symbol":\n\t"							\
	".frame\t$29, 0, $31\n\t"					\
	"sw\t$16,"__str(PT_R16)"($29)\t\t\t# save_static_function\n\t"	\
	"sw\t$17,"__str(PT_R17)"($29)\n\t"				\
	"sw\t$18,"__str(PT_R18)"($29)\n\t"				\
	"sw\t$19,"__str(PT_R19)"($29)\n\t"				\
	"sw\t$20,"__str(PT_R20)"($29)\n\t"				\
	"sw\t$21,"__str(PT_R21)"($29)\n\t"				\
	"sw\t$22,"__str(PT_R22)"($29)\n\t"				\
	"sw\t$23,"__str(PT_R23)"($29)\n\t"				\
	"sw\t$30,"__str(PT_R30)"($29)\n\t"				\
	"j\t" #symbol "\n\t"						\
	".end\t__" #symbol "\n\t"					\
	".size\t__" #symbol",. - __" #symbol)

#endif /* CONFIG_32BIT */

#ifdef CONFIG_64BIT

#define save_static_function(symbol)					\
__asm__(								\
	".text\n\t"							\
	".globl\t__" #symbol "\n\t"					\
	".align\t2\n\t"							\
	".type\t__" #symbol ", @function\n\t"				\
	".ent\t__" #symbol ", 0\n__"					\
	#symbol":\n\t"							\
	".frame\t$29, 0, $31\n\t"					\
	"sd\t$16,"__str(PT_R16)"($29)\t\t\t# save_static_function\n\t"	\
	"sd\t$17,"__str(PT_R17)"($29)\n\t"				\
	"sd\t$18,"__str(PT_R18)"($29)\n\t"				\
	"sd\t$19,"__str(PT_R19)"($29)\n\t"				\
	"sd\t$20,"__str(PT_R20)"($29)\n\t"				\
	"sd\t$21,"__str(PT_R21)"($29)\n\t"				\
	"sd\t$22,"__str(PT_R22)"($29)\n\t"				\
	"sd\t$23,"__str(PT_R23)"($29)\n\t"				\
	"sd\t$30,"__str(PT_R30)"($29)\n\t"				\
	"j\t" #symbol "\n\t"						\
	".end\t__" #symbol "\n\t"					\
	".size\t__" #symbol",. - __" #symbol)

#endif /* CONFIG_64BIT */

#endif /* _ASM_SIM_H */
