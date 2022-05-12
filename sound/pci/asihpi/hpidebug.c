// SPDX-License-Identifier: GPL-2.0-only
/************************************************************************

    AudioScience HPI driver
    Copyright (C) 1997-2011  AudioScience Inc. <support@audioscience.com>


Debug macro translation.

************************************************************************/

#include "hpi_internal.h"
#include "hpidebug.h"

/* Debug level; 0 quiet; 1 informative, 2 debug, 3 verbose debug.  */
int hpi_debug_level = HPI_DEBUG_LEVEL_DEFAULT;

void hpi_debug_init(void)
{
	printk(KERN_INFO "debug start\n");
}

int hpi_debug_level_set(int level)
{
	int old_level;

	old_level = hpi_debug_level;
	hpi_debug_level = level;
	return old_level;
}

int hpi_debug_level_get(void)
{
	return hpi_debug_level;
}

void hpi_debug_message(struct hpi_message *phm, char *sz_fileline)
{
	if (phm) {
		printk(KERN_DEBUG "HPI_MSG%d,%d,%d,%d,%d\n", phm->version,
			phm->adapter_index, phm->obj_index, phm->function,
			phm->u.c.attribute);
	}

}

void hpi_debug_data(u16 *pdata, u32 len)
{
	u32 i;
	int j;
	int k;
	int lines;
	int cols = 8;

	lines = DIV_ROUND_UP(len, cols);
	if (lines > 8)
		lines = 8;

	for (i = 0, j = 0; j < lines; j++) {
		printk(KERN_DEBUG "%p:", (pdata + i));

		for (k = 0; k < cols && i < len; i++, k++)
			printk(KERN_CONT "%s%04x", k == 0 ? "" : " ", pdata[i]);

		printk(KERN_CONT "\n");
	}
}
