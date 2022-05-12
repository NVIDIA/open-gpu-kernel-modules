// SPDX-License-Identifier: GPL-2.0-only
/*
 * Motorola/Emerson MVME5100 with PPCBug firmware.
 *
 * Author: Stephen Chivers <schivers@csc.com>
 *
 * Copyright 2013 CSC Australia Pty. Ltd.
 */
#include "types.h"
#include "ops.h"
#include "io.h"

BSS_STACK(4096);

void platform_init(unsigned long r3, unsigned long r4, unsigned long r5)
{
	u32			heapsize;

	heapsize = 0x8000000 - (u32)_end; /* 128M */
	simple_alloc_init(_end, heapsize, 32, 64);
	fdt_init(_dtb_start);
	serial_console_init();
}
