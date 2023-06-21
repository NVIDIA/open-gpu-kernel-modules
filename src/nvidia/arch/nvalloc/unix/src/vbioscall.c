/*
 * SPDX-FileCopyrightText: Copyright (c) 1999-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */


/******************************************************************************
*
*   Description:
*       This file provides the glue layer between the RM's OS object
*       unixCallVideoBIOS() method and the x86 real mode emulator
*
******************************************************************************/


#include <core/core.h>
#include <nv.h>
#include <nv-priv.h>
#include <osapi.h>

#include <os/os.h>

#include <x86emu/x86emu.h>

#define IO_LOG(port, val)

#define NV_ROMLEY_VGA_PHYS_ADDR                 0x9d000
#define NV_VGA_PHYS_ADDR                        0xa0000
#define NV_VIDEO_ROM_PHYS_ADDR                  0xc0000
#define NV_EXPANSION_VIDEO_ROM_PHYS_ADDR        0xc8000
#define NV_SYSTEM_ROM_PHYS_ADDR                 0xf0000

#define X86EMU_LOWMEM       0x600   // Interrupt vectors, BIOS data size
#define X86EMU_STACK_ADDR   0x10000
#define X86EMU_STACK_SIZE   0x1000
#define X86EMU_BUF_ADDR     0x20000 // EDID buffer for VESA int10 0x4f15
#define X86EMU_BUF_SIZE     128

#define OP_HLT              0xf4    // HLT instruction - causes the emulator to return

// This is where IRET from int10 will take us
#define X86EMU_IRET_SEG     (X86EMU_STACK_ADDR >> 4)
#define X86EMU_IRET_OFF     0

#define X86EMU_DFLT_FLAGS   (F_IF)

static NvBool    x86emuReady;
static void     *x86emuStack;

static struct x86emu_mem_seg {
    NvU32           start;
    NvU32           end;
    void            *vaddr;
}   x86emu_mem_map[] = {
    {NV_ROMLEY_VGA_PHYS_ADDR, NV_VGA_PHYS_ADDR - 1},
    {NV_VGA_PHYS_ADDR, NV_VIDEO_ROM_PHYS_ADDR - 1},                       // vga
    {NV_VIDEO_ROM_PHYS_ADDR, NV_EXPANSION_VIDEO_ROM_PHYS_ADDR -1},        // vbios, sbios
    {NV_EXPANSION_VIDEO_ROM_PHYS_ADDR, NV_SYSTEM_ROM_PHYS_ADDR -1},       // vbios, sbios
    {NV_SYSTEM_ROM_PHYS_ADDR, 0xFFFFF},                                   // Motherboard BIOS
    {X86EMU_STACK_ADDR, X86EMU_STACK_ADDR + X86EMU_STACK_SIZE - 1},       // emulator stack
    {0,                 X86EMU_LOWMEM - 1},                               // Interrupt vectors, BIOS data
    {X86EMU_BUF_ADDR,   X86EMU_BUF_ADDR + X86EMU_BUF_SIZE - 1},
};

enum {
    X86EMU_SEG_ROMLEY_VGA = 0,
    X86EMU_SEG_VGA,
    X86EMU_SEG_ROM,
    X86EMU_SEG_ROM_EXPANSION,
    X86EMU_SEG_ROM_MOTHERBOARD_BIOS,
    X86EMU_SEG_EMULATOR_STACK,
    X86EMU_SEG_BIOS_DATA,
    X86EMU_SEG_EDID_BUFFER,
    X86EMU_NUM_SEGS
};

ct_assert(X86EMU_NUM_SEGS == NV_ARRAY_ELEMENTS(x86emu_mem_map));

static NvU8
x_inb(NvU16 port)
{
    NvU8 val;

    val = os_io_read_byte(port);
    IO_LOG(port, val);
    return val;
}

static void
x_outb(NvU16 port, NvU8 val)
{
    IO_LOG(port, val);
    os_io_write_byte(port, val);
}

static NvU16
x_inw(NvU16 port)
{
    NvU16 val;

    val = os_io_read_word(port);
    IO_LOG(port, val);
    return val;
}

static void
x_outw(NvU16 port, NvU16 val)
{
    IO_LOG(port, val);
    os_io_write_word(port, val);
}

static u32
x_inl(X86EMU_pioAddr port)
{
    NvU32 val;

    val = os_io_read_dword(port);
    IO_LOG(port, val);
    return val;
}

static void
x_outl(X86EMU_pioAddr port, u32 val)
{
    IO_LOG(port, val);
    os_io_write_dword(port, val);
}

static void *
Mem_addr_xlat(NvU32 addr, NvU32 sz)
{
    int                     i;
    struct x86emu_mem_seg   *pseg;

    for (i = 0; i < X86EMU_NUM_SEGS; ++i)
    {
        pseg = x86emu_mem_map + i;
        if (pseg->vaddr != 0 && addr >= pseg->start && addr + sz - 1 <= pseg->end)
            return (void *)((char *)pseg->vaddr + addr - pseg->start);
    }

    X86EMU_halt_sys();

    return 0;
}

static NvU8
Mem_rb(u32 addr)
{
    NvU8    *va = Mem_addr_xlat(addr, 1);

    return va != 0 ? *va : 0;
}

static NvU16
Mem_rw(u32 addr)
{
    NvU16   *va = Mem_addr_xlat(addr, 2);

    return va != 0 ? *va : 0;
}

static u32
Mem_rl(u32 addr)
{
    NvU32   *va = Mem_addr_xlat(addr, 4);

    return va != 0 ? *va : 0;
}

static void
Mem_wb(u32 addr, NvU8 val)
{
    NvU8    *va = Mem_addr_xlat(addr, 1);

    if (va != 0)
        *va = val;
}

static void
Mem_ww(u32 addr, NvU16 val)
{
    NvU16   *va = Mem_addr_xlat(addr, 2);

    if (va != 0)
        *va = val;
}

static void
Mem_wl(u32 addr, u32 val)
{
    NvU32   *va = Mem_addr_xlat(addr, 4);

    if (va != 0)
        *va = val;
}

static NvU16
get_int_seg(int i)
{
    return Mem_rw(i * 4 + 2);
}


static NvU16
get_int_off(int i)
{
    return Mem_rw(i * 4);
}


static void
pushw(NvU16 i)
{
        M.x86.R_ESP -= 2;
        Mem_ww((M.x86.R_SS << 4) + M.x86.R_ESP, i);
}

static void
x86emu_do_int(int num)
{
#if 0
    Int10Current->num = num;

    if (!int_handler(Int10Current)) {
        X86EMU_halt_sys();
    }
#else
    if ((num == 0x15) && (M.x86.R_AX == 0x5f80))
    {
        //
        // Handle the MXM_SYS_INFO_CALLBACK_NUM int 15h SBIOS
        // callback: disclaim support by returning a value
        // other than 005fh (MXM_SYS_INFO_CALLBACK_FUNC_SUPPORTED)
        // to the caller.
        //
        M.x86.R_AX = 0;
    }
    else
    {
        NV_PRINTF(LEVEL_ERROR, "x86emu: int $%d (eax = %08x)\n",
                  num, M.x86.R_EAX);
        DBG_BREAKPOINT();
        X86EMU_halt_sys();
    }
#endif
}

NV_STATUS
RmInitX86EmuState(OBJGPU *pGpu) {
    int                     i;
    struct x86emu_mem_seg   *pseg;
    X86EMU_intrFuncs        *intFuncs;
    X86EMU_pioFuncs         pioFuncs = {
        (&x_inb),
        (&x_inw),
        (&x_inl),
        (&x_outb),
        (&x_outw),
        (&x_outl)
    };

    X86EMU_memFuncs         memFuncs = {
        (&Mem_rb),
        (&Mem_rw),
        (&Mem_rl),
        (&Mem_wb),
        (&Mem_ww),
        (&Mem_wl)
    };

    if (!NV_PRIMARY_VGA(NV_GET_NV_STATE(pGpu)))     // not the primary GPU
        return NV_OK;

    NV_ASSERT(!x86emuReady);

    x86emuStack = portMemAllocNonPaged(
                      X86EMU_STACK_SIZE + sizeof(X86EMU_intrFuncs*) * 256);
    if (x86emuStack == NULL)
        return NV_ERR_NO_MEMORY;

    // Interrupt dispatch table
    intFuncs = (void *)((NvU8 *)x86emuStack + X86EMU_STACK_SIZE);

    // Fill virtual addresses in the memory map
    for (i = 0; i < X86EMU_NUM_SEGS; ++i)
    {
        pseg = x86emu_mem_map + i;
        switch (i)
        {
            case X86EMU_SEG_ROM:
            case X86EMU_SEG_ROM_EXPANSION:
            case X86EMU_SEG_ROM_MOTHERBOARD_BIOS:
                nv_get_updated_emu_seg(&pseg->start, &pseg->end);
                /* fallthrough */
            case X86EMU_SEG_VGA:
            case X86EMU_SEG_BIOS_DATA:
                pseg->vaddr = os_map_kernel_space(pseg->start,
                                                  pseg->end - pseg->start + 1,
                                                  NV_MEMORY_CACHED);
                if (pseg->vaddr == 0)
                {
                    NV_PRINTF(LEVEL_ERROR, "x86emu can't map phys addr 0x%05x\n",
                              pseg->start);
                    return NV_ERR_GENERIC;
                }
                break;
            case X86EMU_SEG_EMULATOR_STACK:
                pseg->vaddr = x86emuStack;
                break;
            default:
                pseg->vaddr = 0;
                break;
        }
    }
    X86EMU_setupMemFuncs(&memFuncs);

    M.mem_base = 0;
    M.mem_size = 1024*1024;
    X86EMU_setupPioFuncs(&pioFuncs);

    for (i=0;i<256;i++)
        intFuncs[i] = x86emu_do_int;
    X86EMU_setupIntrFuncs(intFuncs);

    x86emuReady = NV_TRUE;

    return NV_OK;
}

void
RmFreeX86EmuState(OBJGPU *pGpu)
{
    int                     i;
    struct x86emu_mem_seg   *pseg;

    if (! x86emuReady || !NV_PRIMARY_VGA(NV_GET_NV_STATE(pGpu)))     // not the primary GPU
        return;

    portMemFree(x86emuStack);
    x86emuStack = 0;

    for (i = 0; i < X86EMU_NUM_SEGS; ++i)
    {
        pseg = x86emu_mem_map + i;
        switch (i)
        {
            case X86EMU_SEG_ROMLEY_VGA:
            case X86EMU_SEG_VGA:
            case X86EMU_SEG_ROM:
            case X86EMU_SEG_ROM_EXPANSION:
            case X86EMU_SEG_ROM_MOTHERBOARD_BIOS:
            case X86EMU_SEG_BIOS_DATA:
                if (pseg->vaddr != 0)
                    os_unmap_kernel_space(pseg->vaddr, pseg->end - pseg->start + 1);
        }
        pseg->vaddr = 0;
    }

    x86emuReady = NV_FALSE;
}

NV_STATUS
nv_vbios_call(
    OBJGPU *pGpu,
    NvU32 *eax,
    NvU32 *ebx)
{
    NvU16                   seg;
    NvU16                   off;
    struct x86emu_mem_seg   *pseg;

    if (!NV_PRIMARY_VGA(NV_GET_NV_STATE(pGpu)))
        return NV_ERR_GENERIC;

    seg = get_int_seg(0x10);
    off = get_int_off(0x10);

    pseg = &x86emu_mem_map[X86EMU_SEG_VGA];
    if (seg < (pseg->start >> 4))
    {
        pseg = &x86emu_mem_map[X86EMU_SEG_ROMLEY_VGA];
        if (seg < (pseg->start >> 4))
        {
            NV_PRINTF(LEVEL_ERROR,
                      "cannot call the VBIOS. INT10 vector not in ROM: %04x:%04x\n",
                      seg, off);
            return NV_ERR_GENERIC;
        }
        else if (pseg->vaddr == NULL)
        {
            pseg->vaddr = os_map_kernel_space(pseg->start,
                    (pseg->end - pseg->start + 1),
                     NV_MEMORY_CACHED);
            if (pseg->vaddr == NULL)
            {
                NV_PRINTF(LEVEL_ERROR, "x86emu can't map phys addr 0x%05x\n",
                          pseg->start);
                return NV_ERR_GENERIC;
            }
        }
    }

    // Reset the CPU
    portMemSet(&M, 0, sizeof(M));

    M.x86.R_SS = X86EMU_STACK_ADDR >> 4;
    M.x86.R_ESP = X86EMU_STACK_SIZE;
    M.x86.R_CS = seg;
    M.x86.R_EIP = off;
    M.x86.R_EFLG = X86EMU_DFLT_FLAGS;
    M.x86.R_EAX = *eax;
    M.x86.R_EBX = *ebx;
    M.x86.R_ECX = 0;
    M.x86.R_EDX = 0;
    M.x86.R_ES = X86EMU_BUF_ADDR >> 4;

    X86EMU_trace_on();

    // Map ES:EDI to buffer. Used by vesa intr 4f15 - read EDID
    pseg = &x86emu_mem_map[X86EMU_SEG_EDID_BUFFER];
    pseg->vaddr = NULL;

    // Prepare the code for IRET to jump to
    Mem_wb((X86EMU_IRET_SEG << 4) + X86EMU_IRET_OFF, OP_HLT);

    pushw(X86EMU_DFLT_FLAGS);
    pushw(X86EMU_IRET_SEG);
    pushw(X86EMU_IRET_OFF);

    // Run the emulator
    X86EMU_exec();

    *eax = M.x86.R_EAX;
    *ebx = M.x86.R_EBX;

    return NV_OK;
}
