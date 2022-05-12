/* SPDX-License-Identifier: GPL-2.0 */
#ifndef AMDKCL_IO_H
#define AMDKCL_IO_H

#include <linux/io.h>

#ifndef HAVE_MEMREMAP_WC
enum {
	MEMREMAP_WC = 1 << 2,
};
#endif

#endif /* AMDKCL_IO_H */
