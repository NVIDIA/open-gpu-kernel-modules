/*
 * FIPS 180-2 SHA-224/256/384/512 implementation
 * Last update: 02/02/2007
 * Issue date:  04/30/2005
 *
 * Copyright (C) 2005, 2007 Olivier Gay <olivier.gay@a3.epfl.ch>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
/** \file 	SHA256.h
 *  \brief 	SHA256 definitions.
 *  \author Steven A. Fontana Sr. 
 *  \date 	September 4 2009
*/
#ifndef NV_SHA2_H
#define NV_SHA2_H

#include <nvtypes.h>

#define NV_SHA256_DIGEST_SIZE ( 256 / 8)

#define NV_SHA256_BLOCK_SIZE  ( 512 / 8)

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    NvU32 tot_len;
    NvU32 len;
    NvU8 block[2 * NV_SHA256_BLOCK_SIZE];
    NvU32 h[8];
} nv_sha256_ctx;

void nv_sha256_init(nv_sha256_ctx * ctx);
void nv_sha256_update(nv_sha256_ctx *ctx, const NvU8 *message, NvU32 len);
void nv_sha256_final(nv_sha256_ctx *ctx, NvU8 *digest);

void nv_sha256_noPad(nv_sha256_ctx *ctx, NvU8 *digest);

void nv_sha256(const NvU8 *message, NvU32 len, NvU8 *digest);

#ifdef __cplusplus
}
#endif

#endif /* !NV_SHA2_H */
