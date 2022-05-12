// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/string.h>
#include <linux/uaccess.h>
#include <linux/delay.h>
#include <linux/mm.h>
#include <asm/checksum.h>
#include <asm/sections.h>

EXPORT_SYMBOL(memchr);
EXPORT_SYMBOL(memcpy);
EXPORT_SYMBOL(memset);
EXPORT_SYMBOL(memmove);
EXPORT_SYMBOL(__copy_user);
EXPORT_SYMBOL(__udelay);
EXPORT_SYMBOL(__ndelay);
EXPORT_SYMBOL(__const_udelay);
EXPORT_SYMBOL(strlen);
EXPORT_SYMBOL(csum_partial);
EXPORT_SYMBOL(csum_partial_copy_generic);
EXPORT_SYMBOL(copy_page);
EXPORT_SYMBOL(__clear_user);
EXPORT_SYMBOL(empty_zero_page);
#ifdef CONFIG_FLATMEM
/* need in pfn_valid macro */
EXPORT_SYMBOL(min_low_pfn);
EXPORT_SYMBOL(max_low_pfn);
#endif

#define DECLARE_EXPORT(name)		\
	extern void name(void);EXPORT_SYMBOL(name)

DECLARE_EXPORT(__udivsi3);
DECLARE_EXPORT(__sdivsi3);
DECLARE_EXPORT(__lshrsi3);
DECLARE_EXPORT(__ashrsi3);
DECLARE_EXPORT(__ashlsi3);
DECLARE_EXPORT(__lshrsi3_r0);
DECLARE_EXPORT(__ashrsi3_r0);
DECLARE_EXPORT(__ashlsi3_r0);

DECLARE_EXPORT(__ashiftrt_r4_0);
DECLARE_EXPORT(__ashiftrt_r4_1);
DECLARE_EXPORT(__ashiftrt_r4_2);
DECLARE_EXPORT(__ashiftrt_r4_3);
DECLARE_EXPORT(__ashiftrt_r4_4);
DECLARE_EXPORT(__ashiftrt_r4_5);
DECLARE_EXPORT(__ashiftrt_r4_6);
DECLARE_EXPORT(__ashiftrt_r4_7);
DECLARE_EXPORT(__ashiftrt_r4_8);
DECLARE_EXPORT(__ashiftrt_r4_9);
DECLARE_EXPORT(__ashiftrt_r4_10);
DECLARE_EXPORT(__ashiftrt_r4_11);
DECLARE_EXPORT(__ashiftrt_r4_12);
DECLARE_EXPORT(__ashiftrt_r4_13);
DECLARE_EXPORT(__ashiftrt_r4_14);
DECLARE_EXPORT(__ashiftrt_r4_15);
DECLARE_EXPORT(__ashiftrt_r4_16);
DECLARE_EXPORT(__ashiftrt_r4_17);
DECLARE_EXPORT(__ashiftrt_r4_18);
DECLARE_EXPORT(__ashiftrt_r4_19);
DECLARE_EXPORT(__ashiftrt_r4_20);
DECLARE_EXPORT(__ashiftrt_r4_21);
DECLARE_EXPORT(__ashiftrt_r4_22);
DECLARE_EXPORT(__ashiftrt_r4_23);
DECLARE_EXPORT(__ashiftrt_r4_24);
DECLARE_EXPORT(__ashiftrt_r4_25);
DECLARE_EXPORT(__ashiftrt_r4_26);
DECLARE_EXPORT(__ashiftrt_r4_27);
DECLARE_EXPORT(__ashiftrt_r4_28);
DECLARE_EXPORT(__ashiftrt_r4_29);
DECLARE_EXPORT(__ashiftrt_r4_30);
DECLARE_EXPORT(__ashiftrt_r4_31);
DECLARE_EXPORT(__ashiftrt_r4_32);
DECLARE_EXPORT(__movstr);
DECLARE_EXPORT(__movstrSI8);
DECLARE_EXPORT(__movstrSI12);
DECLARE_EXPORT(__movstrSI16);
DECLARE_EXPORT(__movstrSI20);
DECLARE_EXPORT(__movstrSI24);
DECLARE_EXPORT(__movstrSI28);
DECLARE_EXPORT(__movstrSI32);
DECLARE_EXPORT(__movstrSI36);
DECLARE_EXPORT(__movstrSI40);
DECLARE_EXPORT(__movstrSI44);
DECLARE_EXPORT(__movstrSI48);
DECLARE_EXPORT(__movstrSI52);
DECLARE_EXPORT(__movstrSI56);
DECLARE_EXPORT(__movstrSI60);
DECLARE_EXPORT(__movstr_i4_even);
DECLARE_EXPORT(__movstr_i4_odd);
DECLARE_EXPORT(__movstrSI12_i4);
DECLARE_EXPORT(__movmem);
DECLARE_EXPORT(__movmemSI8);
DECLARE_EXPORT(__movmemSI12);
DECLARE_EXPORT(__movmemSI16);
DECLARE_EXPORT(__movmemSI20);
DECLARE_EXPORT(__movmemSI24);
DECLARE_EXPORT(__movmemSI28);
DECLARE_EXPORT(__movmemSI32);
DECLARE_EXPORT(__movmemSI36);
DECLARE_EXPORT(__movmemSI40);
DECLARE_EXPORT(__movmemSI44);
DECLARE_EXPORT(__movmemSI48);
DECLARE_EXPORT(__movmemSI52);
DECLARE_EXPORT(__movmemSI56);
DECLARE_EXPORT(__movmemSI60);
DECLARE_EXPORT(__movmem_i4_even);
DECLARE_EXPORT(__movmem_i4_odd);
DECLARE_EXPORT(__movmemSI12_i4);
DECLARE_EXPORT(__udiv_qrnnd_16);
DECLARE_EXPORT(__sdivsi3_i4);
DECLARE_EXPORT(__udivsi3_i4);
DECLARE_EXPORT(__sdivsi3_i4i);
DECLARE_EXPORT(__udivsi3_i4i);
#ifdef CONFIG_MCOUNT
DECLARE_EXPORT(mcount);
#endif
