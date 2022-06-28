/*
 * SPDX-FileCopyrightText: Copyright (c) 2007-2012 NVIDIA CORPORATION & AFFILIATES
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

/*!
 * Utility header file to generate a one-way hash from an arbitrary
 * byte array, using the Secure Hashing Algorithm 1 (SHA-1) as defined
 * in FIPS PUB 180-1 published April 17, 1995:
 *
 *   https://www.itl.nist.gov/fipspubs/fip180-1.htm
 *
 * Some common test cases (see Appendices A and B of the above document):
 *
 *   SHA1("abc") =
 *     A9993E36 4706816A BA3E2571 7850C26C 9CD0D89D
 *
 *   SHA1("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq") =
 *     84983E44 1C3BD26E BAAE4AA1 F95129E5 E54670F1
 */

#ifndef __NV_SHA1_H__
#define __NV_SHA1_H__

#include "nvtypes.h"

/*!
 * @brief Structure used by the SHA-1 functions to maintain the state of the
 * calculations.
 */

typedef struct
{
    NvU32  state[5];
    NvU32  count;
    NvU8   buffer[128];
} Sha1Context;


/*!
 * @brief Pointer to a memory accessor function for use by the SHA-1 hash
 * function.
 *
 * Due to memory constraints in some environments where this code is executed
 * (e.g., the PMU/DPU), the data that needs to be processed by the SHA-1 hash
 * function may not be readily available. This function is responsible for
 * copying the data into a buffer to be used by the SHA-1 function.
 *
 * Besides, SHA1 library can be used by many different clients, so we need to
 * provide the memory accessor functions which can work in client's environment.
 *
 * @param[out]  pBuff   The buffer to copy the new data to.
 * @param[in]   index   The desired offset to begin copying from.
 * @param[in]   size    The requested number of bytes to be copied.
 * @param[in]   info    Pointer to the data passed into GenerateSha1 as pData.
 *
 * @return The actual number of bytes copied into the buffer.
 */

typedef NvU32 Sha1CopyFunc(NvU8 *pBuff, NvU32 index, NvU32 size, void *pInfo);


/*
 * The following values are defined by the SHA-1 algorithm for initial values.
 */
#define SHA1_INIT_H0    0x67452301  //!< Initial H0 value
#define SHA1_INIT_H1    0xEFCDAB89  //!< Initial H1 value
#define SHA1_INIT_H2    0x98BADCFE  //!< Initial H2 value
#define SHA1_INIT_H3    0x10325476  //!< Initial H3 value
#define SHA1_INIT_H4    0xC3D2E1F0  //!< Initial H4 value


/*!
 * @brief Reverses the byte order of a word; that is, switching the endianness
 * of the word.
 *
 * @param[in]   a   A 32-bit word
 *
 * @returns The 32-bit word with its byte order reversed.
 */

#define REVERSE_BYTE_ORDER(a) \
    (((a) >> 24) | ((a) << 24) | (((a) >> 8) & 0xFF00) | (((a) << 8) & 0xFF0000))


/*!
 * @brief   Computation step as defined by SHA-1.
 *
 * Unlike the 64 byte buffer version outlined in the SHA-1 algorithm, this
 * function uses a 128 byte buffer to minimize the calculation needed to
 * index the data.
 *
 * @param[in,out]  pState
 *         Pointer to State word array.
 *
 * @param[in]  pBuffer
 *      Data to operate on. 128 bytes in length. No length checking is done,
 *      and is assumed to have been done by the calling function.
 */

static void
_sha1Transform
(
    NvU32 *pState,
    NvU8  *pBuffer
)
{
    NvU32 a = pState[0];
    NvU32 b = pState[1];
    NvU32 c = pState[2];
    NvU32 d = pState[3];
    NvU32 e = pState[4];
    NvU32 *pBuf = (NvU32 *)pBuffer;
    NvU32 *p;
    NvU32 i;
    NvU32 j;
    NvU32 k;

    for (i = 0; i < 80; i++)
    {
        p = &pBuf[i & 0xf];
        j = p[0];
        if (i < 16)
        {
            j = REVERSE_BYTE_ORDER(j);
        }
        else
        {
            j ^= p[2] ^ p[8] ^ p[13];
            j = (j << 1) + (j >> 31);
        }
        p[0] = p[16] = j;
        if (i < 40)
        {
            if (i < 20)
            {
                k = 0x5a827999 + ((b & (c ^ d)) ^ d);
            }
            else
            {
                k = 0x6ed9eba1 + (b ^ c ^ d);
            }
        }
        else
        {
            if (i < 60)
            {
                k = 0x8f1bbcdc + (((b | c) & d) | (b & c));
            }
            else
            {
                k = 0xca62c1d6 + (b ^ c ^ d);
            }
        }
        j += (a << 5) + (a >> 27) + e + k;
        e = d;
        d = c;
        c = (b << 30) + (b >> 2);
        b = a;
        a = j;
    }
    pState[0] += a;
    pState[1] += b;
    pState[2] += c;
    pState[3] += d;
    pState[4] += e;
}


/*!
 * Initializes the SHA-1 context.
 *
 * @param[out] pContext
 *      Pointer to the context to initialize.
 */

static void
_sha1Initialize
(
    Sha1Context *pContext
)
{
    pContext->count = 0;
    pContext->state[0] = SHA1_INIT_H0;
    pContext->state[1] = SHA1_INIT_H1;
    pContext->state[2] = SHA1_INIT_H2;
    pContext->state[3] = SHA1_INIT_H3;
    pContext->state[4] = SHA1_INIT_H4;
}


/*!
 * @brief Divides the input buffer into multiple 64-byte buffers and computes
 * the message digest for each.
 *
 * @param[in]  pContext
 *          Pointer to a Sha1Context.
 *
 * @param[in]  pData
 *          Pointer to the data array to compute the message digest.
 *
 * @param[in]  len
 *          Size of the data.
 *
 * @param[in]  copyFunc
 *          Copy routine to use.
 */

static void
_sha1Update
(
    Sha1Context *pContext,
    void        *pData,
    NvU32        len,
    Sha1CopyFunc copyFunc
)
{
    NvU32 buffer_offset = (pContext->count & 63);
    NvU32 copy_size;
    NvU32 idx = 0;

    pContext->count += len;
    while ((buffer_offset + len) > 63)
    {
        copy_size = 64 - buffer_offset;
        copyFunc(&pContext->buffer[buffer_offset], idx, copy_size, pData);
        _sha1Transform(pContext->state, pContext->buffer);
        buffer_offset = 0;
        idx += copy_size;
        len -= copy_size;
    }
    if (len > 0)
    {
        copyFunc(&pContext->buffer[buffer_offset], idx, len, pData);
    }
}


/*!
 * @brief fill memory with zero; not all environments in which this
 * code runs have memset(3).
 *
 * @param[out] pData
 *          The memory to be filled with zero
 *
 * @param[in]  nBytes
 *          The number of bytes of memory to fill with zero
 */

static NV_INLINE void
_sha1MemZero
(
    NvU8 *pData,
    NvU32 nBytes
)
{
    NvU32 i;

    for (i = 0; i < nBytes; i++) {
        pData[i] = 0;
    }
}


/*!
 * @brief Pads the message as specified by the SHA-1 algorithm and computes
 * the message digest on the final message chunk(s).
 *
 * @param[out] pDigest
 *          The SHA-1 hash values.
 *
 * @param[in]  pContext
 *          Pointer to a Sha1Context.
 */

static void
_sha1Final
(
    NvU8        *pDigest,
    Sha1Context *pContext
)
{
    NvU32  i;
    NvU32  bufferOffset = (pContext->count & 63);
    NvU8  *pBuffer = (NvU8*)&pContext->buffer[bufferOffset];
    NvU32 *pCount;
    NvU32 *pDig32;

    // append padding pattern to the end of input
    *pBuffer++ = 0x80;
    if (bufferOffset < 56)
    {
        _sha1MemZero(pBuffer, 59 - bufferOffset);
    }
    else
    {
        // need an extra sha1_transform
        if (bufferOffset < 63)
        {
            _sha1MemZero(pBuffer, 63 - bufferOffset);
        }
        _sha1Transform(pContext->state, pContext->buffer);
        _sha1MemZero(pContext->buffer, 60);
    }

    // set final count (this is the number of *bits* not *bytes*)
    pCount = (NvU32*)&pContext->buffer[15 << 2];
    *pCount = REVERSE_BYTE_ORDER(pContext->count << 3);

    _sha1Transform(pContext->state, pContext->buffer);

    // output hash with each dword in big endian
    if (pDigest)
    {
        pDig32 = (NvU32*) pDigest;
        for (i = 0; i < 5; i++)
        {
            pDig32[i] = REVERSE_BYTE_ORDER(pContext->state[i]);
        }
    }
}


/*!
 * @brief   Generates the SHA-1 hash value on the data provided.
 *
 * The function does not manipulate the source data directly, as it may not
 * have direct access to it. Therefore, it relies upon the copy function to
 * copy segments of the data into a local buffer before any manipulation takes
 * place.
 *
 * @param[out]  pHash
 *          Pointer to store the hash array. The buffer must be 20 bytes in
 *          length, and the result is stored in big endian format.
 *
 * @param[in]   pData
 *          The source data array to transform. The actual values and make-up
 *          of this parameter are dependent on the copy function.
 *
 * @param[in]   nBytes
 *          The size, in bytes, of the source data.
 *
 * @param[in]   copyFunc
 *          The function responsible for copying data from the source
 *          for use by the sha1 function. It is possible for the data
 *          to exist outside the current execution environment (e.g.,
 *          the PMU, and the data to hash are in system memory), so
 *          the function will never directly manipulate the source
 *          data.
 */

#define NV_SHA1_BLOCK_LENGTH  64
#define NV_SHA1_DIGEST_LENGTH 20

static void
sha1Generate
(
    NvU8         pHash[NV_SHA1_DIGEST_LENGTH],
    void        *pData,
    NvU32        nBytes,
    Sha1CopyFunc copyFunc
)
{
    Sha1Context context;

    _sha1Initialize(&context);
    _sha1Update(&context, pData, nBytes, copyFunc);
    _sha1Final(pHash, &context);
}


#endif /* __NV_SHA1_H__ */
