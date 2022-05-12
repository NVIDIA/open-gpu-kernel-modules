// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *
 * Copyright (C) IBM Corporation, 2012
 *
 * Author: Anton Blanchard <anton@au.ibm.com>
 */

/*
 * Sparse (as at v0.5.0) gets very, very confused by this file.
 * Make it a bit simpler for it.
 */
#if !defined(__CHECKER__)
#include <altivec.h>
#else
#define vec_xor(a, b) a ^ b
#define vector __attribute__((vector_size(16)))
#endif

#include "xor_vmx.h"

typedef vector signed char unative_t;

#define DEFINE(V)				\
	unative_t *V = (unative_t *)V##_in;	\
	unative_t V##_0, V##_1, V##_2, V##_3

#define LOAD(V)			\
	do {			\
		V##_0 = V[0];	\
		V##_1 = V[1];	\
		V##_2 = V[2];	\
		V##_3 = V[3];	\
	} while (0)

#define STORE(V)		\
	do {			\
		V[0] = V##_0;	\
		V[1] = V##_1;	\
		V[2] = V##_2;	\
		V[3] = V##_3;	\
	} while (0)

#define XOR(V1, V2)					\
	do {						\
		V1##_0 = vec_xor(V1##_0, V2##_0);	\
		V1##_1 = vec_xor(V1##_1, V2##_1);	\
		V1##_2 = vec_xor(V1##_2, V2##_2);	\
		V1##_3 = vec_xor(V1##_3, V2##_3);	\
	} while (0)

void __xor_altivec_2(unsigned long bytes, unsigned long *v1_in,
		     unsigned long *v2_in)
{
	DEFINE(v1);
	DEFINE(v2);
	unsigned long lines = bytes / (sizeof(unative_t)) / 4;

	do {
		LOAD(v1);
		LOAD(v2);
		XOR(v1, v2);
		STORE(v1);

		v1 += 4;
		v2 += 4;
	} while (--lines > 0);
}

void __xor_altivec_3(unsigned long bytes, unsigned long *v1_in,
		     unsigned long *v2_in, unsigned long *v3_in)
{
	DEFINE(v1);
	DEFINE(v2);
	DEFINE(v3);
	unsigned long lines = bytes / (sizeof(unative_t)) / 4;

	do {
		LOAD(v1);
		LOAD(v2);
		LOAD(v3);
		XOR(v1, v2);
		XOR(v1, v3);
		STORE(v1);

		v1 += 4;
		v2 += 4;
		v3 += 4;
	} while (--lines > 0);
}

void __xor_altivec_4(unsigned long bytes, unsigned long *v1_in,
		     unsigned long *v2_in, unsigned long *v3_in,
		     unsigned long *v4_in)
{
	DEFINE(v1);
	DEFINE(v2);
	DEFINE(v3);
	DEFINE(v4);
	unsigned long lines = bytes / (sizeof(unative_t)) / 4;

	do {
		LOAD(v1);
		LOAD(v2);
		LOAD(v3);
		LOAD(v4);
		XOR(v1, v2);
		XOR(v3, v4);
		XOR(v1, v3);
		STORE(v1);

		v1 += 4;
		v2 += 4;
		v3 += 4;
		v4 += 4;
	} while (--lines > 0);
}

void __xor_altivec_5(unsigned long bytes, unsigned long *v1_in,
		     unsigned long *v2_in, unsigned long *v3_in,
		     unsigned long *v4_in, unsigned long *v5_in)
{
	DEFINE(v1);
	DEFINE(v2);
	DEFINE(v3);
	DEFINE(v4);
	DEFINE(v5);
	unsigned long lines = bytes / (sizeof(unative_t)) / 4;

	do {
		LOAD(v1);
		LOAD(v2);
		LOAD(v3);
		LOAD(v4);
		LOAD(v5);
		XOR(v1, v2);
		XOR(v3, v4);
		XOR(v1, v5);
		XOR(v1, v3);
		STORE(v1);

		v1 += 4;
		v2 += 4;
		v3 += 4;
		v4 += 4;
		v5 += 4;
	} while (--lines > 0);
}
