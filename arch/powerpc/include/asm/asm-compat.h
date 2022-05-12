#ifndef _ASM_POWERPC_ASM_COMPAT_H
#define _ASM_POWERPC_ASM_COMPAT_H

#include <asm/asm-const.h>
#include <asm/types.h>
#include <asm/ppc-opcode.h>

#ifdef __powerpc64__

/* operations for longs and pointers */
#define PPC_LL		stringify_in_c(ld)
#define PPC_STL		stringify_in_c(std)
#define PPC_STLU	stringify_in_c(stdu)
#define PPC_LCMPI	stringify_in_c(cmpdi)
#define PPC_LCMPLI	stringify_in_c(cmpldi)
#define PPC_LCMP	stringify_in_c(cmpd)
#define PPC_LONG	stringify_in_c(.8byte)
#define PPC_LONG_ALIGN	stringify_in_c(.balign 8)
#define PPC_TLNEI	stringify_in_c(tdnei)
#define PPC_LLARX(t, a, b, eh)	PPC_LDARX(t, a, b, eh)
#define PPC_STLCX	stringify_in_c(stdcx.)
#define PPC_CNTLZL	stringify_in_c(cntlzd)
#define PPC_MTOCRF(FXM, RS) MTOCRF((FXM), RS)
#define PPC_LR_STKOFF	16
#define PPC_MIN_STKFRM	112

#ifdef __BIG_ENDIAN__
#define LHZX_BE	stringify_in_c(lhzx)
#define LWZX_BE	stringify_in_c(lwzx)
#define LDX_BE	stringify_in_c(ldx)
#define STWX_BE	stringify_in_c(stwx)
#define STDX_BE	stringify_in_c(stdx)
#else
#define LHZX_BE	stringify_in_c(lhbrx)
#define LWZX_BE	stringify_in_c(lwbrx)
#define LDX_BE	stringify_in_c(ldbrx)
#define STWX_BE	stringify_in_c(stwbrx)
#define STDX_BE	stringify_in_c(stdbrx)
#endif

#else /* 32-bit */

/* operations for longs and pointers */
#define PPC_LL		stringify_in_c(lwz)
#define PPC_STL		stringify_in_c(stw)
#define PPC_STLU	stringify_in_c(stwu)
#define PPC_LCMPI	stringify_in_c(cmpwi)
#define PPC_LCMPLI	stringify_in_c(cmplwi)
#define PPC_LCMP	stringify_in_c(cmpw)
#define PPC_LONG	stringify_in_c(.long)
#define PPC_LONG_ALIGN	stringify_in_c(.balign 4)
#define PPC_TLNEI	stringify_in_c(twnei)
#define PPC_LLARX(t, a, b, eh)	PPC_LWARX(t, a, b, eh)
#define PPC_STLCX	stringify_in_c(stwcx.)
#define PPC_CNTLZL	stringify_in_c(cntlzw)
#define PPC_MTOCRF	stringify_in_c(mtcrf)
#define PPC_LR_STKOFF	4
#define PPC_MIN_STKFRM	16

#endif

#endif /* _ASM_POWERPC_ASM_COMPAT_H */
