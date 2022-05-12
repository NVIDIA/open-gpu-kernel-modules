/* SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note */
/*
 *  S390 version
 *
 *  Derived from "include/asm-i386/types.h"
 */

#ifndef _UAPI_S390_TYPES_H
#define _UAPI_S390_TYPES_H

#include <asm-generic/int-ll64.h>

#ifndef __ASSEMBLY__

/* A address type so that arithmetic can be done on it & it can be upgraded to
   64 bit when necessary 
*/
typedef unsigned long addr_t; 
typedef __signed__ long saddr_t;

typedef struct {
	__u32 u[4];
} __vector128;

#endif /* __ASSEMBLY__ */

#endif /* _UAPI_S390_TYPES_H */
