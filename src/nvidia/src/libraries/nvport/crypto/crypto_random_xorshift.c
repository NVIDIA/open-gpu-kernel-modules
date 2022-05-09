/*
 * SPDX-FileCopyrightText: Copyright (c) 2016-2016 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/**
 * @file
 * @brief CRYPTO module PRNG implementation using the xorshift algorithm.
 *
 * For details about the Xorshift algorithms, see:
 *    https://en.wikipedia.org/wiki/Xorshift
 *
 * @note Xorshift algorithms take either 128bit or 1024bit seeds. The algorithm
 * author suggests seeding a splitmix64.c with a 64bit value, and using its
 * output to seed xorshift.
 * See http://xorshift.di.unimi.it/ for details.
 *
 * @warning Xorshift algorithms are NOT CRYPTOGRAPHICALLY SECURE. They generally
 * perform really well on various randomness tests, but are not suitable for
 * security sensitive operations such as key generation. If you require a CSRNG
 * use @ref portCryptoExTrueRandomGetU32 and family.
 */
#include "nvport/nvport.h"


/**
 * @brief Number of 64bit words used to store the state of the algorithm.
 * xorshift128+ uses 2 qwords of state, and xorshift1024* uses 16 qwords
 */
#define XORSHIFT_STATE_QWORDS 2

struct PORT_CRYPTO_PRNG 
{ 
    NvU64 state[XORSHIFT_STATE_QWORDS]; 
};
PORT_CRYPTO_PRNG *portCryptoDefaultGenerator;

void portCryptoInitialize()
{
    NvU64 seed;
#if defined(PORT_CRYPTO_PRNG_SEED)
    seed = PORT_CRYPTO_PRNG_SEED;
#elif PORT_IS_FUNC_SUPPORTED(portCryptoExTrueRandomGetU64)
    seed = portCryptoExTrueRandomGetU64();
#elif PORT_IS_MODULE_SUPPORTED(time)
    seed = portTimeGetUptimeNanosecondsHighPrecision();
#elif defined(NVRM) && !defined(NVWATCH)
    {
        extern NvU64 osGetTimestamp(void);
        seed = osGetTimestamp();
    }
#else
    seed = (NvUPtr)&portCryptoDefaultGenerator;
#endif
    portCryptoPseudoRandomSetSeed(seed);
}

void portCryptoShutdown()
{
    portCryptoPseudoRandomGeneratorDestroy(portCryptoDefaultGenerator);
    portCryptoDefaultGenerator = NULL;
}


/**
 * @brief Initializes a xorshift state from a 64bit seed. Performed using a
 * splitmix64 PRNG.
 * 
 * Adapted from: http://xorshift.di.unimi.it/splitmix64.c
 */
static void _initState(NvU64 seed64, NvU64 state[XORSHIFT_STATE_QWORDS]) 
{
    NvU32 i;
    for (i = 0; i < XORSHIFT_STATE_QWORDS; i++)
    {
        NvU64 z = (seed64 += 0x9E3779B97F4A7C15ULL);
        z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
        z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
        state[i] = z ^ (z >> 31);
    }
}

/**
 * @brief Get the next 64bit value using the xorshift128+ algorithm
 *
 * Adapted from: http://xorshift.di.unimi.it/xorshift128plus.c
 */
static NvU64 _xorshift128plus_GetU64(NvU64 state[2]) 
{
    NvU64 s1 = state[0];
    const NvU64 s0 = state[1];
    state[0] = s0;
    s1 ^= s1 << 23; // a
    state[1] = s1 ^ s0 ^ (s1 >> 18) ^ (s0 >> 5); // b, c
    return state[1] + s0; 
}

PORT_CRYPTO_PRNG *portCryptoPseudoRandomGeneratorCreate(NvU64 seed)
{
    PORT_CRYPTO_PRNG *pPrng = portMemAllocNonPaged(sizeof(*pPrng));

    if (pPrng != NULL)
    {
        _initState(seed, pPrng->state);
    }
    return pPrng;
}

void portCryptoPseudoRandomGeneratorDestroy(PORT_CRYPTO_PRNG *pPrng) 
{
    portMemFree(pPrng);
}

NvU32 portCryptoPseudoRandomGeneratorGetU32(PORT_CRYPTO_PRNG *pPrng)
{

    return (NvU32) _xorshift128plus_GetU64(pPrng->state);
}
NvU64 portCryptoPseudoRandomGeneratorGetU64(PORT_CRYPTO_PRNG *pPrng)
{
    return _xorshift128plus_GetU64(pPrng->state);
}

NV_STATUS portCryptoPseudoRandomGeneratorFillBuffer(PORT_CRYPTO_PRNG *pPrng, NvU8 *pBuffer, NvLength bufSize)
{
    NvLength i;

    PORT_ASSERT_CHECKED(pPrng != NULL);

    /** @note Unlike True Random generators which don't have seeds, here we must
     *  preserve the complete order of bytes across platforms. That means that
     *  we cannot fill the misaligned section first, then copy aligned qwords,
     *  and then fill the remainder - That way we lose some bytes
     */
    
    // Maybe require 64bit alignment for buffers:
    // PORT_ASSERT_CHECKED(portUtilCheckAlignment(pBuffer, sizeof(NvU64)));

    if (pBuffer == NULL)
        return NV_ERR_INVALID_POINTER;

    for (i = 0; i < bufSize; i+=8)
    {
        NvU64 x = _xorshift128plus_GetU64(pPrng->state);
        portMemCopy(pBuffer+i, bufSize-i, &x, (bufSize-i < 8) ? bufSize-i : 8);
    }

    return NV_OK;
}


void portCryptoPseudoRandomSetSeed(NvU64 seed)
{
    if (portCryptoDefaultGenerator)
        portCryptoPseudoRandomGeneratorDestroy(portCryptoDefaultGenerator);
    portCryptoDefaultGenerator = portCryptoPseudoRandomGeneratorCreate(seed);
}

NvU32 portCryptoPseudoRandomGetU32()
{
    return portCryptoPseudoRandomGeneratorGetU32(portCryptoDefaultGenerator);
}

NvU64 portCryptoPseudoRandomGetU64()
{
    return portCryptoPseudoRandomGeneratorGetU64(portCryptoDefaultGenerator);
}

NV_STATUS portCryptoPseudoRandomFillBuffer(NvU8 *pBuffer, NvLength bufSize)
{
    return portCryptoPseudoRandomGeneratorFillBuffer(portCryptoDefaultGenerator, pBuffer, bufSize);
}
