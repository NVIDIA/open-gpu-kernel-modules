/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * AMD Memory Encryption Support
 *
 * Copyright (C) 2016 Advanced Micro Devices, Inc.
 *
 * Author: Tom Lendacky <thomas.lendacky@amd.com>
 */
#ifndef KCL_KCL_MEM_ENCRYPT_H
#define KCL_KCL_MEM_ENCRYPT_H

#ifdef HAVE_LINUX_MEM_ENCRYPT_H
#include <linux/mem_encrypt.h>
#ifndef HAVE_MEM_ENCRYPT_ACTIVE
static inline bool mem_encrypt_active(void)
{
	return sme_me_mask;
}
#endif
#else
static inline bool mem_encrypt_active(void)
{
    return false;
}
#endif
#endif
