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
* Description:  This file contains the code to handle debugging of the
*                               emulator.
*
****************************************************************************/

#include "x86emu/x86emui.h"
#ifndef NO_SYS_HEADERS
#include <stdarg.h>
#include <stdlib.h>
#endif

/*----------------------------- Implementation ----------------------------*/

#ifdef X86EMU_DEBUG

static void     print_encoded_bytes (u16 s, u16 o);
static void     print_decoded_instruction (void);
static int      parse_line (char *s, int *ps, int *n);

/* should look something like debug's output. */
void X86EMU_trace_regs (void)
{
        if (DEBUG_TRACE()) {
                x86emu_dump_regs();
    }
        if (DEBUG_DECODE() && ! DEBUG_DECODE_NOPRINT()) {
                NV_PRINTF(LEVEL_INFO, "%04x:%04x ", M.x86.saved_cs,
                          M.x86.saved_ip);
                print_encoded_bytes( M.x86.saved_cs, M.x86.saved_ip);
                print_decoded_instruction();
    }
}

void X86EMU_trace_xregs (void)
{
        if (DEBUG_TRACE()) {
                x86emu_dump_xregs();
    }
}

void x86emu_just_disassemble (void)
{
    /*
     * This routine called if the flag DEBUG_DISASSEMBLE is set kind
     * of a hack!
     */
        NV_PRINTF(LEVEL_INFO, "%04x:%04x ", M.x86.saved_cs,
                  M.x86.saved_ip);
        print_encoded_bytes( M.x86.saved_cs, M.x86.saved_ip);
        print_decoded_instruction();
}

static void disassemble_forward (u16 seg, u16 off, int n)
{
        X86EMU_sysEnv tregs;
        int i;
        u8 op1;
    /*
     * hack, hack, hack.  What we do is use the exact machinery set up
     * for execution, except that now there is an additional state
     * flag associated with the "execution", and we are using a copy
     * of the register struct.  All the major opcodes, once fully
     * decoded, have the following two steps: TRACE_REGS(r,m);
     * SINGLE_STEP(r,m); which disappear if DEBUG is not defined to
     * the preprocessor.  The TRACE_REGS macro expands to:
     *
     * if (debug&DEBUG_DISASSEMBLE)
     *     {just_disassemble(); goto EndOfInstruction;}
     *     if (debug&DEBUG_TRACE) trace_regs(r,m);
     *
     * ......  and at the last line of the routine.
     *
     * EndOfInstruction: end_instr();
     *
     * Up to the point where TRACE_REG is expanded, NO modifications
     * are done to any register EXCEPT the IP register, for fetch and
     * decoding purposes.
     *
     * This was done for an entirely different reason, but makes a
     * nice way to get the system to help debug codes.
     */
        tregs = M;
    tregs.x86.R_IP = off;
    tregs.x86.R_CS = seg;

    /* reset the decoding buffers */
    tregs.x86.enc_str_pos = 0;
    tregs.x86.enc_pos = 0;

    /* turn on the "disassemble only, no execute" flag */
    tregs.x86.debug |= DEBUG_DISASSEMBLE_F;

    /* DUMP NEXT n instructions to screen in straight_line fashion */
    /*
     * This looks like the regular instruction fetch stream, except
     * that when this occurs, each fetched opcode, upon seeing the
     * DEBUG_DISASSEMBLE flag set, exits immediately after decoding
     * the instruction.  XXX --- CHECK THAT MEM IS NOT AFFECTED!!!
     * Note the use of a copy of the register structure...
     */
    for (i=0; i<n; i++) {
                op1 = (*sys_rdb)(((u32)M.x86.R_CS<<4) + (M.x86.R_IP++));
                (x86emu_optab[op1])(op1);
    }
    /* end major hack mode. */
}

void x86emu_check_ip_access (void)
{
    /* NULL as of now */
}

void x86emu_check_sp_access (void)
{
}

void x86emu_check_mem_access (u32 dummy)
{
        /*  check bounds, etc */
}

void x86emu_check_data_access (uint dummy1, uint dummy2)
{
        /*  check bounds, etc */
}

void x86emu_inc_decoded_inst_len (int x)
{
        M.x86.enc_pos += x;
}

void x86emu_decode_printf (const char *x)
{
    NvU32 max_size = sizeof(M.x86.decoded_buf) - M.x86.enc_str_pos;

    nvDbgSnprintf(M.x86.decoded_buf + M.x86.enc_str_pos, max_size, "%s", x);
    M.x86.enc_str_pos += portStringLength(x);
}

void x86emu_decode_printf2 (const char *x, int y)
{
    char temp[100];
    nvDbgSnprintf(temp, sizeof(temp), x, y);
    x86emu_decode_printf(temp);
}

void x86emu_end_instr (void)
{
        M.x86.enc_str_pos = 0;
        M.x86.enc_pos = 0;
}

static void print_encoded_bytes (u16 s, u16 o)
{
    int i;
    char buf1[64];
        for (i=0; i< M.x86.enc_pos; i++) {
            nvDbgSnprintf(buf1 + 2 * i, sizeof(buf1) - (2 * i),
                          "%02x", fetch_data_byte_abs(s, o + i));
    }
    NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "%-20s", buf1);
}

static void print_decoded_instruction (void)
{
    NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "%s", M.x86.decoded_buf);
}

void x86emu_print_int_vect (u16 iv)
{
        u16 seg,off;

        if (iv > 256) return;
        seg   = fetch_data_word_abs(0,iv*4);
        off   = fetch_data_word_abs(0,iv*4+2);
        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "%04x:%04x ", seg, off);
}

void X86EMU_dump_memory (u16 seg, u16 off, u32 amt)
{
        u32 start = off & 0xfffffff0;
        u32 end  = (off+16) & 0xfffffff0;
        u32 i;

        while (end <= off + amt) {
                NV_PRINTF(LEVEL_INFO, "%04x:%04x ", seg, start);
                for (i=start; i< off; i++)
                    NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "   ");
                for (       ; i< end; i++)
                    NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "%02x ", fetch_data_byte_abs(seg, i));
                NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "\n");
                start = end;
                end = start + 16;
        }
}

void x86emu_single_step (void)
{
    int ps[10];
    int ntok;
    int cmd;
    int done;
                int segment;
    int offset;
    static int breakpoint;
    static int noDecode = 1;

                if (DEBUG_BREAK()) {
                                if (M.x86.saved_ip != breakpoint) {
                                                return;
                                } else {
                  M.x86.debug &= ~DEBUG_DECODE_NOPRINT_F;
                                                M.x86.debug |= DEBUG_TRACE_F;
                                                M.x86.debug &= ~DEBUG_BREAK_F;
                                                print_decoded_instruction ();
                                                X86EMU_trace_regs();
                                }
                }
    done=0;
        offset = M.x86.saved_ip;
    while (!done) {
        NV_PRINTF(LEVEL_INFO, "-");

        /*
         * Set _X86EMU_env.x86.debug_cmd from a kernel debugger to a command to
         * control the emulator debugger
         */
        if (M.x86.debug_cmd[0] == '\0')
        {
            DBG_BREAKPOINT();
        }

        cmd = parse_line(M.x86.debug_cmd, ps, &ntok);
        switch(cmd) {
          case 'u':
                        disassemble_forward(M.x86.saved_cs,(u16)offset,10);
            break;
          case 'd':
                                                        if (ntok == 2) {
                                                                        segment = M.x86.saved_cs;
                                                                        offset = ps[1];
                                                                        X86EMU_dump_memory(segment,(u16)offset,16);
                                                                        offset += 16;
                                                        } else if (ntok == 3) {
                                                                        segment = ps[1];
                                                                        offset = ps[2];
                                                                        X86EMU_dump_memory(segment,(u16)offset,16);
                                                                        offset += 16;
                                                        } else {
                                                                        segment = M.x86.saved_cs;
                                                                        X86EMU_dump_memory(segment,(u16)offset,16);
                                                                        offset += 16;
                                                        }
            break;
          case 'c':
                        M.x86.debug ^= DEBUG_TRACECALL_F;
            break;
          case 's':
                        M.x86.debug ^= DEBUG_SVC_F | DEBUG_SYS_F | DEBUG_SYSINT_F;
            break;
          case 'r':
                        X86EMU_trace_regs();
            break;
          case 'x':
                        X86EMU_trace_xregs();
            break;
          case 'g':
            if (ntok == 2) {
                breakpoint = ps[1];
                if (noDecode) {
                                M.x86.debug |= DEBUG_DECODE_NOPRINT_F;
                } else {
                                M.x86.debug &= ~DEBUG_DECODE_NOPRINT_F;
                }
                M.x86.debug &= ~DEBUG_TRACE_F;
                M.x86.debug |= DEBUG_BREAK_F;
                done = 1;
            }
            break;
          case 'q':
          M.x86.debug |= DEBUG_EXIT;
          return;
          case 'P':
              noDecode = (noDecode)?0:1;
              NV_PRINTF(LEVEL_INFO, "Toggled decoding to %s\n",
                        (noDecode) ? "FALSE" : "TRUE");
              break;
          case 'a':
              X86EMU_trace_off();
              /* fall thru */
          case 't':
          case 0:
            done = 1;
            break;
        }
        portMemSet(&M.x86.debug_cmd, 0, sizeof(M.x86.debug_cmd));
    }
}

int X86EMU_trace_on(void)
{
        return M.x86.debug |= DEBUG_STEP_F | DEBUG_DECODE_F | DEBUG_TRACE_F;
}

int X86EMU_trace_off(void)
{
        return M.x86.debug &= ~(DEBUG_STEP_F | DEBUG_DECODE_F | DEBUG_TRACE_F);
}

static void parse_hex (char *s, int *pval)
{
    unsigned int val = 0;
    unsigned char c;

    if (s[0] == '0' && s[1] == 'x')
        s += 2;

    while ((c = *(s++)))
    {
        if (c >= '0' && c <= '9')
        {
            val <<= 4;
            val |= c - '0';
            continue;
        }
        else if (c >= 'a' && c <= 'f')
        {
            val <<= 4;
            val |= c - 'a';
            continue;
        }
        else if (c >= 'A' && c <= 'F')
        {
            val <<= 4;
            val |= c - 'A';
            continue;
        }
        break;
    }

    *pval = val;
}

static int parse_line (char *s, int *ps, int *n)
{
    int cmd;

    *n = 0;
    while(*s == ' ' || *s == '\t') s++;
    ps[*n] = *s;
    switch (*s) {
      case '\n':
        *n += 1;
        return 0;
      default:
        cmd = *s;
        *n += 1;
    }

    while (1)
    {
        while (*s != ' ' && *s != '\t' && *s != '\n' && *s != '\0')
            s++;

        if (*s == '\n' || *s == '\0')
            return cmd;

        while(*s == ' ' || *s == '\t')
            s++;

        parse_hex(s, &ps[*n]);
        *n += 1;
    }
}

void x86emu_dump_regs (void)
{
    NV_PRINTF(LEVEL_INFO, "\tAX=%04x  ", M.x86.R_AX);
    NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "BX=%04x  ", M.x86.R_BX);
    NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "CX=%04x  ", M.x86.R_CX);
    NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "DX=%04x  ", M.x86.R_DX);
    NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "SP=%04x  ", M.x86.R_SP);
    NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "BP=%04x  ", M.x86.R_BP);
    NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "SI=%04x  ", M.x86.R_SI);
    NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "DI=%04x\n", M.x86.R_DI);
    NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "\tDS=%04x  ", M.x86.R_DS);
    NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "ES=%04x  ", M.x86.R_ES);
    NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "SS=%04x  ", M.x86.R_SS);
    NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "CS=%04x  ", M.x86.R_CS);
    NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "IP=%04x   ", M.x86.R_IP);
    if (ACCESS_FLAG(F_OF)) {
        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "OV ");
    } else {
        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "NV ");
    }
    if (ACCESS_FLAG(F_DF)) {
        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "DN ");
    } else {
        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "UP ");
    }
    if (ACCESS_FLAG(F_IF)) {
        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "EI ");
    } else {
        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "DI ");
    }
    if (ACCESS_FLAG(F_SF)) {
        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "NG ");
    } else {
        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "PL ");
    }
    if (ACCESS_FLAG(F_ZF)) {
        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "ZR ");
    } else {
        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "NZ ");
    }
    if (ACCESS_FLAG(F_AF)) {
        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "AC ");
    } else {
        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "NA ");
    }
    if (ACCESS_FLAG(F_PF)) {
        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "PE ");
    } else {
        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "PO ");
    }
    if (ACCESS_FLAG(F_CF)) {
        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "CY ");
    } else {
        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "NC ");
    }
    NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "\n");
}

void x86emu_dump_xregs (void)
{
    NV_PRINTF(LEVEL_INFO, "\tEAX=%08x  ", M.x86.R_EAX);
    NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "EBX=%08x  ", M.x86.R_EBX);
    NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "ECX=%08x  ", M.x86.R_ECX);
    NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "EDX=%08x  \n", M.x86.R_EDX);
    NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "\tESP=%08x  ", M.x86.R_ESP);
    NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "EBP=%08x  ", M.x86.R_EBP);
    NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "ESI=%08x  ", M.x86.R_ESI);
    NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "EDI=%08x\n", M.x86.R_EDI);
    NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "\tDS=%04x  ", M.x86.R_DS);
    NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "ES=%04x  ", M.x86.R_ES);
    NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "SS=%04x  ", M.x86.R_SS);
    NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "CS=%04x  ", M.x86.R_CS);
    NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "EIP=%08x\n\t", M.x86.R_EIP);
    if (ACCESS_FLAG(F_OF)) {
        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "OV ");
    } else {
        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "NV ");
    }
    if (ACCESS_FLAG(F_DF)) {
        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "DN ");
    } else {
        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "UP ");
    }
    if (ACCESS_FLAG(F_IF)) {
        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "EI ");
    } else {
        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "DI ");
    }
    if (ACCESS_FLAG(F_SF)) {
        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "NG ");
    } else {
        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "PL ");
    }
    if (ACCESS_FLAG(F_ZF)) {
        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "ZR ");
    } else {
        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "NZ ");
    }
    if (ACCESS_FLAG(F_AF)) {
        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "AC ");
    } else {
        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "NA ");
    }
    if (ACCESS_FLAG(F_PF)) {
        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "PE ");
    } else {
        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "PO ");
    }
    if (ACCESS_FLAG(F_CF)) {
        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "CY ");
    } else {
        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "NC ");
    }
    NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "\n");
}

#else

void x86emu_dump_regs (void)
{
}

void x86emu_dump_xregs (void)
{
}

int X86EMU_trace_on(void)
{
    return 0;
}

int X86EMU_trace_off(void)
{
    return 0;
}

#endif /* X86EMU_DEBUG */

