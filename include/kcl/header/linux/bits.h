/* SPDX-License-Identifier: MIT */
#ifndef _KCL_HEADER__LINUX_BITS_H_H_
#define _KCL_HEADER__LINUX_BITS_H_H_

#if defined(HAVE_LINUX_BITS_H)
#include_next <linux/bits.h>
#else
#include <linux/bitops.h>
#endif

#endif
