/* SPDX-License-Identifier: GPL-2.0 */
#ifndef AMDKCL_COMPILER_ATTRIBUTES_H
#define AMDKCL_COMPILER_ATTRIBUTES_H

#ifdef HAVE_LINUX_COMPILER_ATTRIBUTES_H
#include <linux/compiler_attributes.h>
#endif

#ifndef fallthrough
#define fallthrough                    do {} while (0)  /* fallthrough */
#endif

#endif /* AMDKCL_COMPILER_ATTRIBUTES_H */
