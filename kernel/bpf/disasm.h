/* SPDX-License-Identifier: GPL-2.0-only */
/* Copyright (c) 2011-2014 PLUMgrid, http://plumgrid.com
 * Copyright (c) 2016 Facebook
 */

#ifndef __BPF_DISASM_H__
#define __BPF_DISASM_H__

#include <linux/bpf.h>
#include <linux/kernel.h>
#include <linux/stringify.h>
#ifndef __KERNEL__
#include <stdio.h>
#include <string.h>
#endif

extern const char *const bpf_alu_string[16];
extern const char *const bpf_class_string[8];

const char *func_id_name(int id);

typedef __printf(2, 3) void (*bpf_insn_print_t)(void *private_data,
						const char *, ...);
typedef const char *(*bpf_insn_revmap_call_t)(void *private_data,
					      const struct bpf_insn *insn);
typedef const char *(*bpf_insn_print_imm_t)(void *private_data,
					    const struct bpf_insn *insn,
					    __u64 full_imm);

struct bpf_insn_cbs {
	bpf_insn_print_t	cb_print;
	bpf_insn_revmap_call_t	cb_call;
	bpf_insn_print_imm_t	cb_imm;
	void			*private_data;
};

void print_bpf_insn(const struct bpf_insn_cbs *cbs,
		    const struct bpf_insn *insn,
		    bool allow_ptr_leaks);
#endif
