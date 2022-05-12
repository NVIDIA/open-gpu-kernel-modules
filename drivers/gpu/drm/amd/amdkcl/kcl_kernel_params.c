// SPDX-License-Identifier: GPL-2.0-or-later
/* Helpers for initial module or kernel cmdline parsing
   Copyright (C) 2001 Rusty Russell.

*/
#include <kcl/kcl_moduleparam.h>

// Copied from kernel/params.c
#define STANDARD_PARAM_DEF(name, type, format, strtolfn)      		\
	int param_set_##name(const char *val, const struct kernel_param *kp) \
	{								\
		return strtolfn(val, 0, (type *)kp->arg);		\
	}								\
	int param_get_##name(char *buffer, const struct kernel_param *kp) \
	{								\
		return scnprintf(buffer, PAGE_SIZE, format "\n",	\
				*((type *)kp->arg));			\
	}								\
	const struct kernel_param_ops param_ops_##name = {			\
		.set = param_set_##name,				\
		.get = param_get_##name,				\
	};								\
	EXPORT_SYMBOL(param_set_##name);				\
	EXPORT_SYMBOL(param_get_##name);				\
	EXPORT_SYMBOL(param_ops_##name)

#ifdef _kcl_param_check_hexint
STANDARD_PARAM_DEF(hexint,	unsigned int,		"%#08x", 	kstrtouint);
#endif
