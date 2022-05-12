// SPDX-License-Identifier: GPL-2.0-or-later
#ifndef AMDKCL_NUMA_BACKPORT_H
#define AMDKCL_NUMA_BACKPORT_H

#if !defined(HAVE_PXM_TO_NODE)
extern int (*_kcl_pxm_to_node)(int pxm);
#define pxm_to_node _kcl_pxm_to_node
#endif

#endif
