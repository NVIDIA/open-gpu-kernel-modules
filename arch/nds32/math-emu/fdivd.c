// SPDX-License-Identifier: GPL-2.0
// Copyright (C) 2005-2018 Andes Technology Corporation

#include <linux/uaccess.h>
#include <asm/sfp-machine.h>
#include <math-emu/soft-fp.h>
#include <math-emu/double.h>

void fdivd(void *ft, void *fa, void *fb)
{
	FP_DECL_D(A);
	FP_DECL_D(B);
	FP_DECL_D(R);
	FP_DECL_EX;

	FP_UNPACK_DP(A, fa);
	FP_UNPACK_DP(B, fb);

	if (B_c == FP_CLS_ZERO && A_c != FP_CLS_ZERO)
		FP_SET_EXCEPTION(FP_EX_DIVZERO);

	FP_DIV_D(R, A, B);

	FP_PACK_DP(ft, R);

	__FPU_FPCSR |= FP_CUR_EXCEPTIONS;
}
