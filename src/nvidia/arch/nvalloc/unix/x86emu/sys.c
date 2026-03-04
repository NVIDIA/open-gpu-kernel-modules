/****************************************************************************
*
*                                               Realmode X86 Emulator Library
*
*               Copyright (C) 1996-1999 SciTech Software, Inc.
*                                    Copyright (C) David Mosberger-Tang
*                                          Copyright (C) 1999 Egbert Eich
*
*  ========================================================================
*
*  Permission to use, copy, modify, distribute, and sell this software and
*  its documentation for any purpose is hereby granted without fee,
*  provided that the above copyright notice appear in all copies and that
*  both that copyright notice and this permission notice appear in
*  supporting documentation, and that the name of the authors not be used
*  in advertising or publicity pertaining to distribution of the software
*  without specific, written prior permission.  The authors makes no
*  representations about the suitability of this software for any purpose.
*  It is provided "as is" without express or implied warranty.
*
*  THE AUTHORS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
*  INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
*  EVENT SHALL THE AUTHORS BE LIABLE FOR ANY SPECIAL, INDIRECT OR
*  CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
*  USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
*  OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
*  PERFORMANCE OF THIS SOFTWARE.
*
*  ========================================================================
*
* Language:             ANSI C
* Environment:  Any
* Developer:    Kendall Bennett
*
* Description:  This file includes subroutines which are related to
*                               programmed I/O and memory access. Included in this module
*                               are default functions with limited usefulness. For real
*                               uses these functions will most likely be overriden by the
*                               user library.
*
****************************************************************************/

#include "x86emu/x86emu.h"
#include "x86emu/x86emui.h"
#include "x86emu/regs.h"
#include "x86emu/debug.h"
#include "x86emu/prim_ops.h"
#ifndef NO_SYS_HEADERS
#include <string.h>
#endif
/*------------------------- Global Variables ------------------------------*/

X86EMU_sysEnv           _X86EMU_env;            /* Global emulator machine state */
X86EMU_intrFuncs        _X86EMU_intrTab[256];

/*----------------------------- Implementation ----------------------------*/
#if defined(__alpha__) || defined(__alpha)
/* to cope with broken egcs-1.1.2 :-(((( */

#define ALPHA_UALOADS
/*
 * inline functions to do unaligned accesses
 * from linux/include/asm-alpha/unaligned.h
 */

/*
 * EGCS 1.1 knows about arbitrary unaligned loads.  Define some
 * packed structures to talk about such things with.
 */

#if defined(__GNUC__) && ((__GNUC__ > 2) || (__GNUC_MINOR__ >= 91))
struct __una_u64 { unsigned long  x __attribute__((packed)); };
struct __una_u32 { unsigned int   x __attribute__((packed)); };
struct __una_u16 { unsigned short x __attribute__((packed)); };
#endif

static __inline__ unsigned long ldq_u(unsigned long * r11)
{
#if defined(__GNUC__) && ((__GNUC__ > 2) || (__GNUC_MINOR__ >= 91))
        const struct __una_u64 *ptr = (const struct __una_u64 *) r11;
        return ptr->x;
#else
        unsigned long r1,r2;
        __asm__("ldq_u %0,%3\n\t"
                "ldq_u %1,%4\n\t"
                "extql %0,%2,%0\n\t"
                "extqh %1,%2,%1"
                :"=&r" (r1), "=&r" (r2)
                :"r" (r11),
                 "m" (*r11),
                 "m" (*(const unsigned long *)(7+(char *) r11)));
        return r1 | r2;
#endif
}

static __inline__ unsigned long ldl_u(unsigned int * r11)
{
#if defined(__GNUC__) && ((__GNUC__ > 2) || (__GNUC_MINOR__ >= 91))
        const struct __una_u32 *ptr = (const struct __una_u32 *) r11;
        return ptr->x;
#else
        unsigned long r1,r2;
        __asm__("ldq_u %0,%3\n\t"
                "ldq_u %1,%4\n\t"
                "extll %0,%2,%0\n\t"
                "extlh %1,%2,%1"
                :"=&r" (r1), "=&r" (r2)
                :"r" (r11),
                 "m" (*r11),
                 "m" (*(const unsigned long *)(3+(char *) r11)));
        return r1 | r2;
#endif
}

static __inline__ unsigned long ldw_u(unsigned short * r11)
{
#if defined(__GNUC__) && ((__GNUC__ > 2) || (__GNUC_MINOR__ >= 91))
        const struct __una_u16 *ptr = (const struct __una_u16 *) r11;
        return ptr->x;
#else
        unsigned long r1,r2;
        __asm__("ldq_u %0,%3\n\t"
                "ldq_u %1,%4\n\t"
                "extwl %0,%2,%0\n\t"
                "extwh %1,%2,%1"
                :"=&r" (r1), "=&r" (r2)
                :"r" (r11),
                 "m" (*r11),
                 "m" (*(const unsigned long *)(1+(char *) r11)));
        return r1 | r2;
#endif
}

/*
 * Elemental unaligned stores
 */

static __inline__ void stq_u(unsigned long r5, unsigned long * r11)
{
#if defined(__GNUC__) && ((__GNUC__ > 2) || (__GNUC_MINOR__ >= 91))
        struct __una_u64 *ptr = (struct __una_u64 *) r11;
        ptr->x = r5;
#else
        unsigned long r1,r2,r3,r4;

        __asm__("ldq_u %3,%1\n\t"
                "ldq_u %2,%0\n\t"
                "insqh %6,%7,%5\n\t"
                "insql %6,%7,%4\n\t"
                "mskqh %3,%7,%3\n\t"
                "mskql %2,%7,%2\n\t"
                "bis %3,%5,%3\n\t"
                "bis %2,%4,%2\n\t"
                "stq_u %3,%1\n\t"
                "stq_u %2,%0"
                :"=m" (*r11),
                 "=m" (*(unsigned long *)(7+(char *) r11)),
                 "=&r" (r1), "=&r" (r2), "=&r" (r3), "=&r" (r4)
                :"r" (r5), "r" (r11));
#endif
}

static __inline__ void stl_u(unsigned long r5, unsigned int * r11)
{
#if defined(__GNUC__) && ((__GNUC__ > 2) || (__GNUC_MINOR__ >= 91))
        struct __una_u32 *ptr = (struct __una_u32 *) r11;
        ptr->x = r5;
#else
        unsigned long r1,r2,r3,r4;

        __asm__("ldq_u %3,%1\n\t"
                "ldq_u %2,%0\n\t"
                "inslh %6,%7,%5\n\t"
                "insll %6,%7,%4\n\t"
                "msklh %3,%7,%3\n\t"
                "mskll %2,%7,%2\n\t"
                "bis %3,%5,%3\n\t"
                "bis %2,%4,%2\n\t"
                "stq_u %3,%1\n\t"
                "stq_u %2,%0"
                :"=m" (*r11),
                 "=m" (*(unsigned long *)(3+(char *) r11)),
                 "=&r" (r1), "=&r" (r2), "=&r" (r3), "=&r" (r4)
                :"r" (r5), "r" (r11));
#endif
}

static __inline__ void stw_u(unsigned long r5, unsigned short * r11)
{
#if defined(__GNUC__) && ((__GNUC__ > 2) || (__GNUC_MINOR__ >= 91))
        struct __una_u16 *ptr = (struct __una_u16 *) r11;
        ptr->x = r5;
#else
        unsigned long r1,r2,r3,r4;

        __asm__("ldq_u %3,%1\n\t"
                "ldq_u %2,%0\n\t"
                "inswh %6,%7,%5\n\t"
                "inswl %6,%7,%4\n\t"
                "mskwh %3,%7,%3\n\t"
                "mskwl %2,%7,%2\n\t"
                "bis %3,%5,%3\n\t"
                "bis %2,%4,%2\n\t"
                "stq_u %3,%1\n\t"
                "stq_u %2,%0"
                :"=m" (*r11),
                 "=m" (*(unsigned long *)(1+(char *) r11)),
                 "=&r" (r1), "=&r" (r2), "=&r" (r3), "=&r" (r4)
                :"r" (r5), "r" (r11));
#endif
}

#elif defined(__GNUC__) && ((__GNUC__ < 3)) && \
             (defined (__ia64__) || defined (ia64__))
#define IA64_UALOADS
/*
 * EGCS 1.1 knows about arbitrary unaligned loads.  Define some
 * packed structures to talk about such things with.
 */
struct __una_u64 { unsigned long  x __attribute__((packed)); };
struct __una_u32 { unsigned int   x __attribute__((packed)); };
struct __una_u16 { unsigned short x __attribute__((packed)); };

static __inline__ unsigned long
__uldq (const unsigned long * r11)
{
    const struct __una_u64 *ptr = (const struct __una_u64 *) r11;
    return ptr->x;
}

static __inline__ unsigned long
uldl (const unsigned int * r11)
{
    const struct __una_u32 *ptr = (const struct __una_u32 *) r11;
    return ptr->x;
}

static __inline__ unsigned long
uldw (const unsigned short * r11)
{
    const struct __una_u16 *ptr = (const struct __una_u16 *) r11;
    return ptr->x;
}

static __inline__ void
ustq (unsigned long r5, unsigned long * r11)
{
    struct __una_u64 *ptr = (struct __una_u64 *) r11;
    ptr->x = r5;
}

static __inline__ void
ustl (unsigned long r5, unsigned int * r11)
{
    struct __una_u32 *ptr = (struct __una_u32 *) r11;
    ptr->x = r5;
}

static __inline__ void
ustw (unsigned long r5, unsigned short * r11)
{
    struct __una_u16 *ptr = (struct __una_u16 *) r11;
    ptr->x = r5;
}

#endif

/****************************************************************************
PARAMETERS:
addr    - Emulator memory address to read

RETURNS:
Byte value read from emulator memory.

REMARKS:
Reads a byte value from the emulator memory.
****************************************************************************/
u8 X86API rdb(
    u32 addr)
{
        u8 val;

        if (addr > M.mem_size - 1) {
                DB(NV_PRINTF(LEVEL_INFO, "mem_read: address %#x out of range!\n", addr);)
                HALT_SYS();
                }
        val = *(u8*)(M.mem_base + addr);
DB(     if (DEBUG_MEM_TRACE())
                NV_PRINTF(LEVEL_INFO, "%#08x 1 -> %#x\n", addr, val);)
        return val;
}

/****************************************************************************
PARAMETERS:
addr    - Emulator memory address to read

RETURNS:
Word value read from emulator memory.

REMARKS:
Reads a word value from the emulator memory.
****************************************************************************/
u16 X86API rdw(
        u32 addr)
{
        u16 val = 0;

        if (addr > M.mem_size - 2) {
                DB(NV_PRINTF(LEVEL_INFO, "mem_read: address %#x out of range!\n", addr);)
                HALT_SYS();
                }
#ifdef __BIG_ENDIAN__
        if (addr & 0x1) {
                val = (*(u8*)(M.mem_base + addr) |
                          (*(u8*)(M.mem_base + addr + 1) << 8));
                }
        else
#endif
#if defined(ALPHA_UALOADS)
                val = ldw_u((u16*)(M.mem_base + addr));
#elif  defined(IA64_UALOADS)
      val = uldw((u16*)(M.mem_base + addr));
#else
                val = *(u16*)(M.mem_base + addr);
#endif
DB(     if (DEBUG_MEM_TRACE())
                NV_PRINTF(LEVEL_INFO, "%#08x 2 -> %#x\n", addr, val);)
    return val;
}

/****************************************************************************
PARAMETERS:
addr    - Emulator memory address to read

RETURNS:
Long value read from emulator memory.
REMARKS:
Reads a long value from the emulator memory.
****************************************************************************/
u32 X86API rdl(
        u32 addr)
{
        u32 val = 0;

        if (addr > M.mem_size - 4) {
                DB(NV_PRINTF(LEVEL_INFO, "mem_read: address %#x out of range!\n", addr);)
                HALT_SYS();
                }
#ifdef __BIG_ENDIAN__
        if (addr & 0x3) {
                val = (*(u8*)(M.mem_base + addr + 0) |
                          (*(u8*)(M.mem_base + addr + 1) << 8) |
                          (*(u8*)(M.mem_base + addr + 2) << 16) |
                          (*(u8*)(M.mem_base + addr + 3) << 24));
                }
        else
#endif
#if defined(ALPHA_UALOADS)
                val = ldl_u((u32*)(M.mem_base + addr));
#elif  defined(IA64_UALOADS)
        val = uldl((u32*)(M.mem_base + addr));
#else
                val = *(u32*)(M.mem_base + addr);
#endif
DB(     if (DEBUG_MEM_TRACE())
                NV_PRINTF(LEVEL_INFO, "%#08x 4 -> %#x\n", addr, val);)
        return val;
}

/****************************************************************************
PARAMETERS:
addr    - Emulator memory address to read
val             - Value to store

REMARKS:
Writes a byte value to emulator memory.
****************************************************************************/
void X86API wrb(
        u32 addr,
        u8 val)
{
DB(     if (DEBUG_MEM_TRACE())
                NV_PRINTF(LEVEL_INFO, "%#08x 1 <- %#x\n", addr, val);)
    if (addr > M.mem_size - 1) {
                DB(NV_PRINTF(LEVEL_INFO, "mem_write: address %#x out of range!\n", addr);)
                HALT_SYS();
                }
        *(u8*)(M.mem_base + addr) = val;
}

/****************************************************************************
PARAMETERS:
addr    - Emulator memory address to read
val             - Value to store

REMARKS:
Writes a word value to emulator memory.
****************************************************************************/
void X86API wrw(
        u32 addr,
        u16 val)
{
DB(     if (DEBUG_MEM_TRACE())
                NV_PRINTF(LEVEL_INFO, "%#08x 2 <- %#x\n", addr, val);)
        if (addr > M.mem_size - 2) {
                DB(NV_PRINTF(LEVEL_INFO, "mem_write: address %#x out of range!\n", addr);)
                HALT_SYS();
                }
#ifdef __BIG_ENDIAN__
        if (addr & 0x1) {
                *(u8*)(M.mem_base + addr + 0) = (val >> 0) & 0xff;
                *(u8*)(M.mem_base + addr + 1) = (val >> 8) & 0xff;
                }
        else
#endif
#if defined(ALPHA_UALOADS)
         stw_u(val,(u16*)(M.mem_base + addr));
#elif defined(IA64_UALOADS)
     ustw(val,(u16*)(M.mem_base + addr));
#else
         *(u16*)(M.mem_base + addr) = val;
#endif
}

/****************************************************************************
PARAMETERS:
addr    - Emulator memory address to read
val             - Value to store

REMARKS:
Writes a long value to emulator memory.
****************************************************************************/
void X86API wrl(
        u32 addr,
        u32 val)
{
DB(     if (DEBUG_MEM_TRACE())
                NV_PRINTF(LEVEL_INFO, "%#08x 4 <- %#x\n", addr, val);)
        if (addr > M.mem_size - 4) {
                DB(NV_PRINTF(LEVEL_INFO, "mem_write: address %#x out of range!\n", addr);)
                HALT_SYS();
                }
#ifdef __BIG_ENDIAN__
        if (addr & 0x1) {
                *(u8*)(M.mem_base + addr + 0) = (val >>  0) & 0xff;
                *(u8*)(M.mem_base + addr + 1) = (val >>  8) & 0xff;
                *(u8*)(M.mem_base + addr + 2) = (val >> 16) & 0xff;
                *(u8*)(M.mem_base + addr + 3) = (val >> 24) & 0xff;
                }
        else
#endif
#if defined(ALPHA_UALOADS)
         stl_u(val,(u32*)(M.mem_base + addr));
#elif defined(IA64_UALOADS)
     ustl(val,(u32*)(M.mem_base + addr));
#else
         *(u32*)(M.mem_base + addr) = val;
#endif
}

/****************************************************************************
PARAMETERS:
addr    - PIO address to read
RETURN:
0
REMARKS:
Default PIO byte read function. Doesn't perform real inb.
****************************************************************************/
static u8 X86API p_inb(
        X86EMU_pioAddr addr)
{
DB(     if (DEBUG_IO_TRACE())
                NV_PRINTF(LEVEL_INFO, "inb %#04x \n", addr);)
        return 0;
}

/****************************************************************************
PARAMETERS:
addr    - PIO address to read
RETURN:
0
REMARKS:
Default PIO word read function. Doesn't perform real inw.
****************************************************************************/
static u16 X86API p_inw(
        X86EMU_pioAddr addr)
{
DB(     if (DEBUG_IO_TRACE())
                NV_PRINTF(LEVEL_INFO, "inw %#04x \n", addr);)
        return 0;
}

/****************************************************************************
PARAMETERS:
addr    - PIO address to read
RETURN:
0
REMARKS:
Default PIO long read function. Doesn't perform real inl.
****************************************************************************/
static u32 X86API p_inl(
        X86EMU_pioAddr addr)
{
DB(     if (DEBUG_IO_TRACE())
                NV_PRINTF(LEVEL_INFO, "inl %#04x \n", addr);)
        return 0;
}

/****************************************************************************
PARAMETERS:
addr    - PIO address to write
val     - Value to store
REMARKS:
Default PIO byte write function. Doesn't perform real outb.
****************************************************************************/
static void X86API p_outb(
        X86EMU_pioAddr addr,
        u8 val)
{
DB(     if (DEBUG_IO_TRACE())
                NV_PRINTF(LEVEL_INFO, "outb %#02x -> %#04x \n", val, addr);)
    return;
}

/****************************************************************************
PARAMETERS:
addr    - PIO address to write
val     - Value to store
REMARKS:
Default PIO word write function. Doesn't perform real outw.
****************************************************************************/
static void X86API p_outw(
        X86EMU_pioAddr addr,
        u16 val)
{
DB(     if (DEBUG_IO_TRACE())
                NV_PRINTF(LEVEL_INFO, "outw %#04x -> %#04x \n", val, addr);)
        return;
}

/****************************************************************************
PARAMETERS:
addr    - PIO address to write
val     - Value to store
REMARKS:
Default PIO ;ong write function. Doesn't perform real outl.
****************************************************************************/
static void X86API p_outl(
        X86EMU_pioAddr addr,
        u32 val)
{
DB(     if (DEBUG_IO_TRACE())
                NV_PRINTF(LEVEL_INFO, "outl %#08x -> %#04x \n", val, addr);)
    return;
}

/*------------------------- Global Variables ------------------------------*/

u8      (X86APIP sys_rdb)(u32 addr)                                 = rdb;
u16     (X86APIP sys_rdw)(u32 addr)                                 = rdw;
u32     (X86APIP sys_rdl)(u32 addr)                                 = rdl;
void    (X86APIP sys_wrb)(u32 addr,u8 val)                          = wrb;
void    (X86APIP sys_wrw)(u32 addr,u16 val)                 = wrw;
void    (X86APIP sys_wrl)(u32 addr,u32 val)                 = wrl;
u8      (X86APIP sys_inb)(X86EMU_pioAddr addr)              = p_inb;
u16     (X86APIP sys_inw)(X86EMU_pioAddr addr)              = p_inw;
u32     (X86APIP sys_inl)(X86EMU_pioAddr addr)              = p_inl;
void    (X86APIP sys_outb)(X86EMU_pioAddr addr, u8 val)         = p_outb;
void    (X86APIP sys_outw)(X86EMU_pioAddr addr, u16 val)        = p_outw;
void    (X86APIP sys_outl)(X86EMU_pioAddr addr, u32 val)        = p_outl;

/*----------------------------- Setup -------------------------------------*/

/****************************************************************************
PARAMETERS:
funcs   - New memory function pointers to make active

REMARKS:
This function is used to set the pointers to functions which access
memory space, allowing the user application to override these functions
and hook them out as necessary for their application.
****************************************************************************/
void X86EMU_setupMemFuncs(
        X86EMU_memFuncs *funcs)
{
    sys_rdb = funcs->rdb;
    sys_rdw = funcs->rdw;
    sys_rdl = funcs->rdl;
    sys_wrb = funcs->wrb;
    sys_wrw = funcs->wrw;
    sys_wrl = funcs->wrl;
}

/****************************************************************************
PARAMETERS:
funcs   - New programmed I/O function pointers to make active

REMARKS:
This function is used to set the pointers to functions which access
I/O space, allowing the user application to override these functions
and hook them out as necessary for their application.
****************************************************************************/
void X86EMU_setupPioFuncs(
        X86EMU_pioFuncs *funcs)
{
    sys_inb = funcs->inb;
    sys_inw = funcs->inw;
    sys_inl = funcs->inl;
    sys_outb = funcs->outb;
    sys_outw = funcs->outw;
    sys_outl = funcs->outl;
}

/****************************************************************************
PARAMETERS:
funcs   - New interrupt vector table to make active

REMARKS:
This function is used to set the pointers to functions which handle
interrupt processing in the emulator, allowing the user application to
hook interrupts as necessary for their application. Any interrupts that
are not hooked by the user application, and reflected and handled internally
in the emulator via the interrupt vector table. This allows the application
to get control when the code being emulated executes specific software
interrupts.
****************************************************************************/
void X86EMU_setupIntrFuncs(
        X86EMU_intrFuncs funcs[])
{
    int i;

        for (i=0; i < 256; i++)
                _X86EMU_intrTab[i] = NULL;
        if (funcs) {
                for (i = 0; i < 256; i++)
                        _X86EMU_intrTab[i] = funcs[i];
                }
}

/****************************************************************************
PARAMETERS:
int     - New software interrupt to prepare for

REMARKS:
This function is used to set up the emulator state to exceute a software
interrupt. This can be used by the user application code to allow an
interrupt to be hooked, examined and then reflected back to the emulator
so that the code in the emulator will continue processing the software
interrupt as per normal. This essentially allows system code to actively
hook and handle certain software interrupts as necessary.
****************************************************************************/
void X86EMU_prepareForInt(
        int num)
{
    push_word((u16)M.x86.R_FLG);
    CLEAR_FLAG(F_IF);
    CLEAR_FLAG(F_TF);
    push_word(M.x86.R_CS);
    M.x86.R_CS = mem_access_word(num * 4 + 2);
    push_word(M.x86.R_IP);
    M.x86.R_IP = mem_access_word(num * 4);
        M.x86.intr = 0;
}
