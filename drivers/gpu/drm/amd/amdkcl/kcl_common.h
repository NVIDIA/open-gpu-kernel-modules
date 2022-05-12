/* SPDX-License-Identifier: MIT */
#ifndef AMDKCL_COMMON_H
#define AMDKCL_COMMON_H

#include <linux/kernel.h>
#include <linux/version.h>

#ifdef pr_fmt
#undef pr_fmt
#endif
#define pr_fmt(fmt) "amdkcl: " fmt

void *amdkcl_fp_setup(const char *symbol, void *dummy);

/*
 * create dummy func
 */
#define amdkcl_dummy_symbol(name, ret_type, ret, ...) \
ret_type name(__VA_ARGS__) \
{ \
	pr_warn_once("%s is not supported\n", #name); \
	ret ;\
} \
EXPORT_SYMBOL(name);

#endif
