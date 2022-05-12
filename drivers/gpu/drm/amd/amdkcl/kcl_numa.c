// SPDX-License-Identifier: GPL-2.0-only
#include <linux/acpi.h>

#ifndef HAVE_PXM_TO_NODE
int (*_kcl_pxm_to_node)(int pxm);
EXPORT_SYMBOL(_kcl_pxm_to_node);

/* Copied from include/acpi/acpi_numa.h */
static int __kcl_pxm_to_node_stub(int pxm)
{
	return 0;
}
#endif

void amdkcl_numa_init(void)
{
#ifndef HAVE_PXM_TO_NODE
	_kcl_pxm_to_node = amdkcl_fp_setup("pxm_to_node", __kcl_pxm_to_node_stub);
#endif
}
