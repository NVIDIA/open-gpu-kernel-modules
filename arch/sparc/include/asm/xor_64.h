/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * include/asm/xor.h
 *
 * High speed xor_block operation for RAID4/5 utilizing the
 * UltraSparc Visual Instruction Set and Niagara block-init
 * twin-load instructions.
 *
 * Copyright (C) 1997, 1999 Jakub Jelinek (jj@ultra.linux.cz)
 * Copyright (C) 2006 David S. Miller <davem@davemloft.net>
 */

#include <asm/spitfire.h>

void xor_vis_2(unsigned long, unsigned long *, unsigned long *);
void xor_vis_3(unsigned long, unsigned long *, unsigned long *,
	       unsigned long *);
void xor_vis_4(unsigned long, unsigned long *, unsigned long *,
	       unsigned long *, unsigned long *);
void xor_vis_5(unsigned long, unsigned long *, unsigned long *,
	       unsigned long *, unsigned long *, unsigned long *);

/* XXX Ugh, write cheetah versions... -DaveM */

static struct xor_block_template xor_block_VIS = {
        .name	= "VIS",
        .do_2	= xor_vis_2,
        .do_3	= xor_vis_3,
        .do_4	= xor_vis_4,
        .do_5	= xor_vis_5,
};

void xor_niagara_2(unsigned long, unsigned long *, unsigned long *);
void xor_niagara_3(unsigned long, unsigned long *, unsigned long *,
		   unsigned long *);
void xor_niagara_4(unsigned long, unsigned long *, unsigned long *,
		   unsigned long *, unsigned long *);
void xor_niagara_5(unsigned long, unsigned long *, unsigned long *,
		   unsigned long *, unsigned long *, unsigned long *);

static struct xor_block_template xor_block_niagara = {
        .name	= "Niagara",
        .do_2	= xor_niagara_2,
        .do_3	= xor_niagara_3,
        .do_4	= xor_niagara_4,
        .do_5	= xor_niagara_5,
};

#undef XOR_TRY_TEMPLATES
#define XOR_TRY_TEMPLATES				\
	do {						\
		xor_speed(&xor_block_VIS);		\
		xor_speed(&xor_block_niagara);		\
	} while (0)

/* For VIS for everything except Niagara.  */
#define XOR_SELECT_TEMPLATE(FASTEST) \
	((tlb_type == hypervisor && \
	  (sun4v_chip_type == SUN4V_CHIP_NIAGARA1 || \
	   sun4v_chip_type == SUN4V_CHIP_NIAGARA2 || \
	   sun4v_chip_type == SUN4V_CHIP_NIAGARA3 || \
	   sun4v_chip_type == SUN4V_CHIP_NIAGARA4 || \
	   sun4v_chip_type == SUN4V_CHIP_NIAGARA5)) ? \
	 &xor_block_niagara : \
	 &xor_block_VIS)
