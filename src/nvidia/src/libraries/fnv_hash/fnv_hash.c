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

/*!
 * @file
 * @brief NBSI table initialization, search and hash routines.
 */

#include "nvacpitypes.h"
#include "platform/nbsi/nbsi_read.h"

/***
 *
 * Fowler/Noll/Vo- hash
 *
 * The basis of this hash algorithm was taken from an idea sent
 * as reviewer comments to the IEEE POSIX P1003.2 committee by:
 *
 *      Phong Vo (https://www.research.att.com/info/kpv/)
 *      Glenn Fowler (https://www.research.att.com/~gsf/)
 *
 * In a subsequent ballot round:
 *
 *      Landon Curt Noll (https://www.isthe.com/chongo/)
 *
 * improved on their algorithm.  Some people tried this hash
 * and found that it worked rather well.  In an EMail message
 * to Landon, they named it the ``Fowler/Noll/Vo'' or FNV hash.
 *
 * FNV hashes are designed to be fast while maintaining a low
 * collision rate. The FNV speed allows one to quickly hash lots
 * of data while maintaining a reasonable collision rate.  See:
 *
 *      https://www.isthe.com/chongo/tech/comp/fnv/index.html
 *
 * for more details as well as other forms of the FNV hash.
 *
 ***
    ... more stuff here from the fnv.h header that I deleted to keep the
    ... size down.
 ***
 *
 * Please do not copyright this code.  This code is in the public domain.
 *
 * LANDON CURT NOLL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO
 * EVENT SHALL LANDON CURT NOLL BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * By:
 *    chongo <Landon Curt Noll> /\oo/\
 *      https://www.isthe.com/chongo/
 *
 * Share and Enjoy!    :-)
 */

// This code from hash_32.c

/*
 * fnv_32_buf - perform a 32 bit Fowler/Noll/Vo hash on a buffer
 *
 * input:
 *    buf    - start of buffer to hash
 *    len    - length of buffer in octets
 *    hval   - previous hash value or 0 if first call
 *    prevPartHashLen - previous partial hash dataLen
 *
 * returns:
 *    32 bit hash as a static hash type
 *
 * NOTE: To use the 32 bit FNV-0 historic hash, use FNV0_32_INIT as the hval
 *     argument on the first call to either fnv_32_buf() or fnv_32_str().
 *
 * NOTE: To use the recommended 32 bit FNV-1 hash, use FNV1_32_INIT as the hval
 *     argument on the first call to either fnv_32_buf() or fnv_32_str().
 */
NvU32 fnv32buf(const void *buf, NvU32 len, NvU32 hval, NvU32 prevPartHashLen)
{
    const NvU8 *bp = (const NvU8 *)buf;    /* start of buffer */
    const NvU8 *be = bp + len;       /* beyond end of buffer */
    NvU8 counter = (NvU8) (prevPartHashLen & 1);

    /*
     * FNV-1a hash each octet in the buffer
     */
    while (bp < be) {

        /* xor the bottom with the current octet */
        //
        // The true FNV1 algorithm doesn't xor the 7th bit based on bit 0
        // of the position counter...
        // This was done because we had a collision in NBSI string hashes.
        //
        hval ^= (((NvU32)(NV_TO_UPPER(*bp))) ^ ((counter++ & 0x1) << 7));
        bp++;

        /* multiply by the 32 bit FNV magic prime mod 2^32 */
        hval += (hval<<1) + (hval<<4) + (hval<<7) + (hval<<8) + (hval<<24);

    }

    /* return our new hash value */
    return hval;
}

//----------------------------------------------------------------------------
//  NvU16 fnv32bufUnicode(* data, data_len, hval, prevPartHashLen)
//
//  This function computes a 16 bit hash using the Fowler/Noll/Vo hash method
//  The self described public domain code was copied and modifed to fit into
//  our system and in this case changed from byte array to unicode array with
//  the upper bytes of the unicode character being ignored.
//
//  Input parameters:
//      *data       - Byte array
//      dataLen     - length of Byte array
//      hval        - initial hash value
//      prevPartHashLen - previous partial hash dataLen
//
//  Returns the 16 bit hash.
//
//----------------------------------------------------------------------------

NvU32 fnv32bufUnicode(const void *buf, NvU32 len, NvU32 hval, NvU32 prevPartHashLen)
{
    const NvU16 *bp = (const NvU16 *)buf;    /* start of buffer */
    const NvU16 *be = bp + len;        /* beyond end of buffer */
    NvU8 counter = (NvU8) (prevPartHashLen & 1);

    /*
     * FNV-1a hash each octet in the buffer
     */
    while (bp < be) {

        /* xor the bottom with the current octet */
        //
        // The true FNV1 algorithm doesn't xor the 7th bit based on bit 0...
        // This was done because we had a collision in NBSI string hashes.
        //

        hval ^= (((NvU32)(NV_TO_UPPER(*bp))) ^ ((counter++ & 0x1) << 7));
        bp++;

        /* multiply by the 32 bit FNV magic prime mod 2^32 */
        hval += (hval<<1) + (hval<<4) + (hval<<7) + (hval<<8) + (hval<<24);

    }

    /* return our new hash value */
    return hval;
}

//----------------------------------------------------------------------------
//  NvU16 fnv1Hash16(* data, data_len)
//
//  This function computes a 16 bit hash using the Fowler/Noll/Vo hash method
//  The self described public domain code was copied and modifed to fit into
//  our system.
//
//  Input parameters:
//      *data       - Byte array
//      dataLen    - length of Byte array
//
//  Returns the 16 bit hash.
//
//----------------------------------------------------------------------------

NvU16 fnv1Hash16(const NvU8 * data, NvU32 dataLen)
{
    NvU32 hash;

    NV_ASSERT(data);

    hash = fnv32buf(data, dataLen, FNV1_32_INIT, 0);
    hash = (hash>>16) ^ (hash & MASK_16);
    return (NvU16) hash;
}

//----------------------------------------------------------------------------
//  NvU16 fnv1Hash16Unicode(* data, data_len)
//
//  This function computes a 16 bit hash using the Fowler/Noll/Vo hash method
//  The self described public domain code was copied and modifed to fit into
//  our system.
//
//  Input parameters:
//      *data      - Unicode array
//      dataLen    - length of Byte array
//
//  Returns the 16 bit hash.
//
//----------------------------------------------------------------------------

NvU16 fnv1Hash16Unicode(const NvU16 * data, NvU32 dataLen)
{
    NvU32 hash;

    NV_ASSERT(data);

    hash = fnv32bufUnicode(data, dataLen, FNV1_32_INIT, 0);
    hash = (hash>>16) ^ (hash & MASK_16);
    return (NvU16) hash;
}

//----------------------------------------------------------------------------
//  NvU32 fnv1Hash20Array(* data, data_len, * rtnHashArray, numOSes)
//
//  This function computes a 20 bit hash for each possible OS to search for
//
//  Input parameters:
//      *data         - Byte array
//      dataLen       - length of Byte array
//      *rtnHashArray - array to return the computed hashes
//      numOSes       - Count of OSes to compute. 1 means generic.
//
//  Output:
//      rtnHashArray - filled in.
//
//----------------------------------------------------------------------------

void fnv1Hash20Array
(
    const NvU8 * data,
    NvU32 dataLen,
    NvU32 * rtnHashArray,
    NvU8 numOSes
)
{
    NvU32 baseHash;
    NvU32 hash;
    NvU8 i;
    NBSI_OBJ *pNbsiObj = getNbsiObject();

    NV_ASSERT(data);
    NV_ASSERT(rtnHashArray);
    NV_ASSERT(numOSes <= pNbsiObj->curMaxNbsiOSes);
    NV_ASSERT(pNbsiObj->curMaxNbsiOSes <= MAX_NBSI_OS);

    if (dataLen == 0)
    {
        // If element is empty just copy the predone os hashes
        for (i = 0; i < numOSes; i++)
        {
            rtnHashArray[i] = pNbsiObj->nbsiOSstrHash[i];
        }
    }
    else
    {
        // Compute the base element hash.
        baseHash = fnv32buf(data, dataLen, FNV1_32_INIT, 0);
        rtnHashArray[0] = ((baseHash>>20) ^ baseHash) & MASK_20;

        // Now for each OS string, compute the other hashes
        for (i = 1; i < numOSes; i++)
        {
            // start the hash with the original elements 32 bit hash
            hash = fnv32buf(&pNbsiObj->nbsiOSstr[i][0],
                            pNbsiObj->nbsiOSstrLen[i],
                            baseHash,
                            dataLen);
            rtnHashArray[i] = ((hash>>20) ^ hash) & MASK_20;
        }
    }
    return;
}

//----------------------------------------------------------------------------
//  NvU32 fnv1Hash20ArrayUnicode(* data, data_len, * rtnHashArray, numOSes)
//
//  This function computes a 20 bit hash for each possible OS to search for
//
//  Input parameters:
//      *data         - Byte array
//      dataLen       - length of Byte array
//      *rtnHashArray - array to return the computed hashes
//      numOSes       - Count of OSes to compute. 1 means generic.
//
//  Output:
//      rtnHashArray - filled in.
//
//----------------------------------------------------------------------------

void fnv1Hash20ArrayUnicode
(
    const NvU16 * data,
    NvU32 dataLen,
    NvU32 * rtnHashArray,
    NvU8 numOSes
)
{
    NvU32 baseHash;
    NvU32 hash;
    NvU8 i;
    NBSI_OBJ *pNbsiObj = getNbsiObject();

    NV_ASSERT(data);
    NV_ASSERT(rtnHashArray);
    NV_ASSERT(numOSes <= pNbsiObj->curMaxNbsiOSes);
    NV_ASSERT(pNbsiObj->curMaxNbsiOSes <= MAX_NBSI_OS);

    if (dataLen == 0)
    {
        // If element is empty just copy the predone os hashes
        for (i = 0; i < numOSes; i++)
        {
            rtnHashArray[i] = pNbsiObj->nbsiOSstrHash[i];
        }
    }
    else
    {
        // Compute the base element hash.
        baseHash = fnv32bufUnicode(data, dataLen, FNV1_32_INIT, 0);
        rtnHashArray[0] = ((baseHash>>20) ^ baseHash) & MASK_20;

        //
        // Now for each OS string, compute the other hashes
        // Note these are stored in ascii not unicode
        //
        for (i = 1; i < numOSes; i++)
        {
            // start with the hash with the original elements 32 bit hash
            hash = fnv32buf(&pNbsiObj->nbsiOSstr[i][0],
                             pNbsiObj->nbsiOSstrLen[i],
                             baseHash,
                             dataLen);
            rtnHashArray[i] = ((hash>>20) ^ hash) & MASK_20;
        }
    }
    return;
}

#define FNV1_64_INIT ((NvU64) 0xCBF29CE484222325ULL)
                            // ((u_int64_t)14695981039346656037)
#define FNV1_64_PRIME ((NvU64) 0x100000001B3)
                             // ((u_int64_t)1099511628211)
/*
 * fnv_64_buf - perform a 64 bit Fowler/Noll/Vo hash on a buffer
 *
 * input:
 *    buf    - start of buffer to hash
 *    len    - length of buffer in octets
 *    hval    - previous hash value or 0 if first call
 *
 * returns:
 *    64 bit hash as a static hash type
 *
 * NOTE: To use the 64 bit FNV-0 historic hash, use FNV0_64_INIT as the hval
 *     argument on the first call to either fnv_64_buf() or fnv_64_str().
 *
 * NOTE: To use the recommended 64 bit FNV-1 hash, use FNV1_64_INIT as the hval
 *     argument on the first call to either fnv_64_buf() or fnv_64_str().
 */
static NvU64 fnv64buf(const NvU8 *buf, NvU32 len, NvU64 hval)
{
    const NvU8 *bp = buf;            /* start of buffer */
    const NvU8 *be = bp + len;       /* beyond end of buffer */

    /*
     * FNV-1a hash each octet in the buffer
     */
    while (bp < be) {

        /* xor the bottom with the current octet */
        hval ^= ((NvU64)*bp++);

        /* multiply by the 64 bit FNV magic prime mod 2^64 */
        hval += (hval << 1) + (hval << 4) + (hval << 5) +
            (hval << 7) + (hval << 8) + (hval << 40);
    }

    /* return our new hash value */
    return hval;
}

//----------------------------------------------------------------------------
//  NvU64 fnv1Hash64(* data, data_len)
//
//  This function computes a 64 bit hash using the Fowler/Noll/Vo hash method
//  The self described public domain code was copied and modifed to fit into
//  our system.
//
//  Input parameters:
//      *data       - Byte array
//      dataLen    - length of Byte array
//
//  Returns the 64 bit hash.
//
//----------------------------------------------------------------------------

NvU64 fnv1Hash64(const NvU8 * data, NvU32 dataLen)
{
    NvU64 hash;

    NV_ASSERT(data);

    hash = fnv64buf(data, dataLen, FNV1_64_INIT);
    return hash;
}


