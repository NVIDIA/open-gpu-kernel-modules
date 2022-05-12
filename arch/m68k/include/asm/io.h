/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _M68K_IO_H
#define _M68K_IO_H

#if defined(__uClinux__) || defined(CONFIG_COLDFIRE)
#include <asm/io_no.h>
#else
#include <asm/io_mm.h>
#endif

#include <asm-generic/io.h>

#endif /* _M68K_IO_H */
