// SPDX-License-Identifier: GPL-2.0
/*
 * This code comes from arch/arm64/kernel/crash_dump.c
 * Created by: AKASHI Takahiro <takahiro.akashi@linaro.org>
 * Copyright (C) 2017 Linaro Limited
 */

#include <linux/crash_dump.h>
#include <linux/io.h>

/**
 * copy_oldmem_page() - copy one page from old kernel memory
 * @pfn: page frame number to be copied
 * @buf: buffer where the copied page is placed
 * @csize: number of bytes to copy
 * @offset: offset in bytes into the page
 * @userbuf: if set, @buf is in a user address space
 *
 * This function copies one page from old kernel memory into buffer pointed by
 * @buf. If @buf is in userspace, set @userbuf to %1. Returns number of bytes
 * copied or negative error in case of failure.
 */
ssize_t copy_oldmem_page(unsigned long pfn, char *buf,
			 size_t csize, unsigned long offset,
			 int userbuf)
{
	void *vaddr;

	if (!csize)
		return 0;

	vaddr = memremap(__pfn_to_phys(pfn), PAGE_SIZE, MEMREMAP_WB);
	if (!vaddr)
		return -ENOMEM;

	if (userbuf) {
		if (copy_to_user((char __user *)buf, vaddr + offset, csize)) {
			memunmap(vaddr);
			return -EFAULT;
		}
	} else
		memcpy(buf, vaddr + offset, csize);

	memunmap(vaddr);
	return csize;
}
