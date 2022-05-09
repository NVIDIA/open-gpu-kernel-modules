/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2016 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * @brief Crypto module public interface
 */

#ifndef _NVPORT_H_
#error "This file cannot be included directly. Include nvport.h instead."
#endif

#ifndef _NVPORT_CRYPTO_H_
#define _NVPORT_CRYPTO_H_
/**
 * @defgroup NVPORT_CRYPTO Cryptography operations
 *
 * @brief This module contains cryptographic and PRNG functions
 *
 *
 * A note on terminology:
 *
 * Pseudorandom numbers are deterministic and reproducible. When given the same
 * seed, they will always give the same sequence, across all platforms. They are
 * not suitable for cryptography or any security sensitive operations.
 *
 * True random numbers are generated from hardware, and as such are completely
 * nondeterministic. There is no support for setting a seed, and you can expect
 * the output to always be different. Unlike pseudorandom numbers, true random 
 * output will always vary across different platforms.
 *
 * These numbers are suitable for security sensitive and cryptography operations.
 *
 * In case of kernelmode code, the entropy pool will contain bits that are not
 * available to usermode clients. As a consequence, a usermode client cannot
 * deplete the entropy pool to lower the security 
 *
 * @note Unless ending with the "-Blocking" suffix, all functions are 
 * non-blocking. With regards to True Random numbers, this has a consequence 
 * that if there are insufficient bits in the entropy pool, they will be used
 * to seed a custom PRNG which will provide the final output. A blocking 
 * version of some functions may be available as an extended function.
 * 
 * @note As a general rule, you should always use the non-blocking version of a
 * function, unless ALL the following conditions are satisfied:
 * - First time booting a clean OS
 * - No connection to the network
 * - The GPU is not booted yet
 * - Dealing with a remote machine (i.e. no direct mouse/keyboard input)
 * - No HW random support (older CPUs)
 *
 * For additional information, see these links:
 * - http://www.2uo.de/myths-about-urandom/
 * - https://bugs.ruby-lang.org/issues/9569
 * - http://security.stackexchange.com/questions/3936/is-a-rand-from-dev-urandom-secure-for-a-login-key
 *
 * @{
 */


/**
 * @name Core Functions
 * @{
 */

/**
 * @brief Initializes global CRYPTO module state
 *
 * This function is called by @ref portInitialize. It is available here in case
 * it is needed to initialize the CRYPTO module without initializing all the
 * others. e.g. for unit tests.
 *
 */
void portCryptoInitialize(void);
/**
 * @brief Destroys global CRYPTO module state
 *
 * This function is called by @ref portShutdown. It is available here in case
 * it is needed to initialize the CRYPTO module without initializing all the
 * others. e.g. for unit tests.
 *
 */
void portCryptoShutdown(void);

/**
 * @brief A pseudorandom number generator object
 */
typedef struct PORT_CRYPTO_PRNG PORT_CRYPTO_PRNG;


/**
 * @brief Construct a PRNG with the given seed.
 *
 * @warning These objects are not Cryptographically Secure, and thus not
 * appropriate for any security sensitive operations. Use "True" random instead.
 *
 * The same seed will always result in the same sequence returned by 
 * @ref portCryptoPseudoRandomGeneratorGetU32, 
 * @ref portCryptoPseudoRandomGeneratorGetU64 and
 * @ref portCryptoPseudoRandomGeneratorFillBuffer. This behavior is consistent
 * across all platforms. The following code will always print the same thing:
 * ~~~{.c}
 * PORT_CRYPTO_PRNG *pPrng = portCryptoPseudoRandomGeneratorCreate(0xdeadbeef);
 * if (pPrng)
 * {
 *     NvU32 n = portCryptoPseudoRandomGeneratorGetU32(pPrng);
 *     portDbgPrintf("%u", n);
 *     portCryptoPseudoRandomGeneratorDestroy(pPrng);
 * }
 * ~~~
 *
 * @return NULL if the construction failed, a PRNG object otherwise.
 *
 */ 
PORT_CRYPTO_PRNG *portCryptoPseudoRandomGeneratorCreate(NvU64 seed); 
/**
 * @brief Destroys an object created with 
 * @ref portCryptoPseudoRandomGeneratorCreate
 *
 */
void portCryptoPseudoRandomGeneratorDestroy(PORT_CRYPTO_PRNG *pPrng);
/**
 * @brief Returns a 32bit pseudorandom number from a given PRNG.
 *
 * @warning The numbers generated in this way are not appropriate for security
 * sensitive operations. Use @ref portCryptoExTrueRandomGetU32 instead.
 *
 * @param [in] pPrng - Generator object. If NULL, the default one will be used.
 *
 */
NvU32 portCryptoPseudoRandomGeneratorGetU32(PORT_CRYPTO_PRNG *pPrng);
/**
 * @brief Returns a 64bit pseudorandom number from a given PRNG.
 *
 * @warning The numbers generated in this way are not appropriate for security
 * sensitive operations. Use @ref portCryptoExTrueRandomGetU64 instead. 
 *
 * @param [in] pPrng - Generator object. If NULL, the default one will be used
 *
 */
NvU64 portCryptoPseudoRandomGeneratorGetU64(PORT_CRYPTO_PRNG *pPrng);
/**
 * @brief Fills a user provided buffer with a pseudorandom sequence from a given
 * PRNG
 *
 * @warning The numbers generated in this way are not appropriate for security
 * sensitive operations. Use @ref portCryptoExTrueRandomFillBuffer instead. 
 *
 * @param [in] pPrng - Generator object. If NULL, the default one will be used
 *
 * @return  NV_OK if successful;
 *          NV_ERR_INVALID_POINTER if pBuffer is NULL;
 *
 */
NV_STATUS portCryptoPseudoRandomGeneratorFillBuffer(PORT_CRYPTO_PRNG *pPrng, NvU8 *pBuffer, NvLength bufSize);

/**
 * @brief Sets the PRNG seed of the global generator
 *
 * The same seed will always result in the same sequence returned by 
 * @ref portCryptoPseudoRandomGetU32, @ref portCryptoPseudoRandomGetU64 and
 * @ref portCryptoPseudoRandomFillBuffer. This behavior is consistent across
 * all platforms. The following code will print the same thing on all platforms:
 * ~~~{.c}
 * portCryptoPseudoRandomSetSeed(0xdeadbeef);
 * NvU32 n = portCryptoPseudoRandomGetU32();
 * portDbgPrintf("%u", n);
 * ~~~
 *
 */
void portCryptoPseudoRandomSetSeed(NvU64 seed);

/**
 * @brief Returns a 32bit pseudorandom number from global generator
 *
 * This is equivalent to calling @ref portCryptoPseudoRandomGeneratorGetU32 with
 * a NULL generator object.
 *
 * @warning The numbers generated in this way are not appropriate for security
 * sensitive operations. Use @ref portCryptoExTrueRandomGetU32 instead. 
 *
 */
NvU32 portCryptoPseudoRandomGetU32(void);
/**
 * @brief Returns a 64bit pseudorandom number
 *
 * This is equivalent to calling @ref portCryptoPseudoRandomGeneratorGetU64 with
 * a NULL generator object.
 *
 * @warning The numbers generated in this way are not appropriate for security
 * sensitive operations. Use @ref portCryptoExTrueRandomGetU64 instead. 
 *
 */
NvU64 portCryptoPseudoRandomGetU64(void);
/**
 * @brief Fills a user provided buffer with a pseudorandom sequence.
 *
 * This is equivalent to calling @ref portCryptoPseudoRandomGeneratorFillBuffer
 * with a NULL generator object.
 *
 * @warning The numbers generated in this way are not appropriate for security
 * sensitive operations. Use @ref portCryptoExTrueRandomFillBuffear instead. 
 *
 * @return  NV_OK if successful;
 *          NV_ERR_INVALID_POINTER if pBuffer is NULL;
 *
 */
NV_STATUS portCryptoPseudoRandomFillBuffer(NvU8 *pBuffer, NvLength bufSize);

/**
 * @brief Calculate the MD5 hash of a given buffer
 *
 * @param [in]  pInBuffer  - Input data. Must not be NULL.
 * @param [in]  bufSize    - Size of input buffer, in bytes.
 * @param [out] pOutBuffer - Output buffer. Must be at least 16 bytes in length
 *
 * @return NV_OK if successful.
 */
NV_STATUS portCryptoHashMD5(const NvU8 *pInBuffer, NvLength bufSize, NvU8 pOutBuffer[16]);
/**
 * @brief Calculate the first 24 bits of the MD5 hash of a given buffer
 *
 * The 24 bits are interpreted as a full hash, and are stored as big endian. So,
 * if the full hash was d41d8cd98f00b204e9800998ecf8427e, the short 24bit hash
 * would be 0x00d41d8c.
 *
 * @param [in]  pInBuffer  - Input data. Must not be NULL.
 * @param [in]  bufSize    - Size of input buffer, in bytes.
 * @param [out] pOut       - Output location. Only the lowest 24 bits are set.
 *
 * @return NV_OK if successful.
 */
NV_STATUS portCryptoHashMD5Short(const NvU8 *pInBuffer, NvLength bufSize, NvU32 *pOut);
/**
 * @brief Convert a binary representation of the MD5 hash to a 32digit hex string
 */
NV_STATUS portCryptoHashMD5BinaryToHexString(const NvU8 pBinary[16], char pHexStr[33]);
/**
 * @brief Convert a 32 digit hex string representation of the MD5 hash to binary
 */
NV_STATUS portCryptoHashMD5HexStringToBinary(const char *pHexStr, NvU8 pBinary[16]);

/// @} End core functions

/**
 * @name Extended Functions
 * @{
 */
#if defined(NV_MODS) || PORT_IS_KERNEL_BUILD
#define PORT_CRYPTO_TRUE_RANDOM_SUPPORTED 0
#else
#define PORT_CRYPTO_TRUE_RANDOM_SUPPORTED 1
#endif
/**
 * @brief Returns a 32bit random number
 *
 * @note This function does not block, but rather combines the bits from the
 * entropy pool with a PRNG to produce a random output of desired width.
 * This is considered safe for most cryptographic applications. You can use 
 * @ref portCryptoExTrueRandomGetU32Blocking for a guaranteed high entropy output. 
 */
NvU32 portCryptoExTrueRandomGetU32(void);
#define portCryptoExTrueRandomGetU32_SUPPORTED 0
/**
 * @brief Returns a 64bit random number
 *
 * @note This function does not block, but rather combines the bits from the
 * entropy pool with a PRNG to produce a random output of desired width. 
 * This is considered safe for most cryptographic applications. You can use
 * @ref portCryptoExTrueRandomGetU64Blocking for a guaranteed high entropy output. 
 */
NvU64 portCryptoExTrueRandomGetU64(void);
#define portCryptoExTrueRandomGetU64_SUPPORTED 0
/**
 * @brief Fills a user provided buffer with a random sequence.
 *
 * @note This function does not block, but rather combines the bits from the
 * entropy pool with a PRNG to produce a random output of desired width. This is
 * considered safe for most cryptographic applications. You can use 
 * @ref portCryptoExTrueRandomFillBufferBlocking for a guaranteed high entropy
 * output. 
 *
 * @return  NV_OK if successful;
 *          NV_ERR_INVALID_POINTER if pBuffer is NULL;
 */
NV_STATUS portCryptoExTrueRandomFillBuffer(NvU8 *pBuffer, NvLength bufSize);
#define portCryptoExTrueRandomFillBuffer_SUPPORTED 0

#define PORT_CRYPTO_TRUE_RANDOM_BLOCKING_SUPPORTED (!PORT_IS_KERNEL_BUILD && !NVOS_IS_WINDOWS)

/**
 * @brief Returns a 32bit random number, possibly blocking the thread.
 *
 * If there is not enough entropy bits available, the function will block until
 * available. Use @ref portCryptoExTrueRandomGetU32 unless you really need the
 * entire result to exclusively made of true random bits.
 */
NvU32 portCryptoExTrueRandomGetU32Blocking(void);
#define portCryptoExTrueRandomGetU32Blocking_SUPPORTED 0
/**
 * @brief Returns a 64bit random number, possibly blocking the thread.
 *
 * If there is not enough entropy bits available, the function will block until
 * available. Use @ref portCryptoExTrueRandomGetU64 unless you really need the
 * entire result to exclusively made of true random bits.
 */
NvU64 portCryptoExTrueRandomGetU64Blocking(void);
#define portCryptoExTrueRandomGetU64Blocking_SUPPORTED 0

/**
 * @brief Fills a user provided buffer with a random sequence,
 * possibly blocking the thread.
 *
 * If there is not enough entropy bits available, the function will block until
 * available. Use @ref portCryptoExTrueRandomFillBuffer unless you really need the
 * entire result to exclusively made of true random bits.
 */
NV_STATUS portCryptoExTrueRandomFillBufferBlocking(NvU8 *pBuffer, NvLength bufSize);
#define portCryptoExTrueRandomFillBufferBlocking_SUPPORTED 0

/// @} End extended functions

/// @}

#endif // _NVPORT_CRYPTO_H_
/// @}
