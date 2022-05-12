/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __ASM_MACH_TX49XX_MANGLE_PORT_H
#define __ASM_MACH_TX49XX_MANGLE_PORT_H

#define __swizzle_addr_b(port)	(port)
#define __swizzle_addr_w(port)	(port)
#define __swizzle_addr_l(port)	(port)
#define __swizzle_addr_q(port)	(port)

#define ioswabb(a, x)		(x)
#define __mem_ioswabb(a, x)	(x)
#if defined(CONFIG_TOSHIBA_RBTX4939) && \
	IS_ENABLED(CONFIG_SMC91X) && \
	defined(__BIG_ENDIAN)
#define NEEDS_TXX9_IOSWABW
extern u16 (*ioswabw)(volatile u16 *a, u16 x);
extern u16 (*__mem_ioswabw)(volatile u16 *a, u16 x);
#else
#define ioswabw(a, x)		le16_to_cpu((__force __le16)(x))
#define __mem_ioswabw(a, x)	(x)
#endif
#define ioswabl(a, x)		le32_to_cpu((__force __le32)(x))
#define __mem_ioswabl(a, x)	(x)
#define ioswabq(a, x)		le64_to_cpu((__force __le64)(x))
#define __mem_ioswabq(a, x)	(x)

#endif /* __ASM_MACH_TX49XX_MANGLE_PORT_H */
