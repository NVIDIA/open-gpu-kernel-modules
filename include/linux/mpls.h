/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_MPLS_H
#define _LINUX_MPLS_H

#include <uapi/linux/mpls.h>

#define MPLS_TTL_MASK		(MPLS_LS_TTL_MASK >> MPLS_LS_TTL_SHIFT)
#define MPLS_BOS_MASK		(MPLS_LS_S_MASK >> MPLS_LS_S_SHIFT)
#define MPLS_TC_MASK		(MPLS_LS_TC_MASK >> MPLS_LS_TC_SHIFT)
#define MPLS_LABEL_MASK		(MPLS_LS_LABEL_MASK >> MPLS_LS_LABEL_SHIFT)

#endif  /* _LINUX_MPLS_H */
