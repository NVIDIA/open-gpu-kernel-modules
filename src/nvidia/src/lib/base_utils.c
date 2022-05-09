/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * @brief Common utility code that has no natural home
 */


#include "lib/base_utils.h"
#include "os/os.h"

//
// Log2 approximation that assumes a power of 2 number passed in.
//
NvU32 nvLogBase2(NvU64 val)
{
    NvU32 i;

    NV_ASSERT(((val)&(val-1)) == 0);

    for (i = 0; i < 64; i++)
    {
       if ((1ull << i) == val)
       {
           break;
       }
    }

    NV_ASSERT(i < 64);

    return i;
}


/**
 * @brief Finds the lowest unset bit of a given bitfield.
 *
 * Returns the lowest value of X such that the expression
 * pBitField[X/32] & (1<<(X%32)) is zero.
 *
 * If all bits are set, returns numElements*32.
 *
 * @param[in] pBitField
 * @param[in] numElements size of array pBitField
 *
 * @return the lowest zero bit, numElements*32 otherwise.
 */
NvU32 nvBitFieldLSZero(NvU32 *pBitField32, NvU32 numElements)
{
    NvU32 i;

    for (i = 0; i < numElements; ++i)
    {
        NvU32 temp = ~pBitField32[i];
        if (temp)
        {
            LOWESTBITIDX_32(temp);
            return temp + i * sizeof(NvU32) * 8;
        }
    }

    return numElements*32;
}

/**
 * @brief Finds the highest unset bit of a given bitfield.
 *
 * Returns the highest value of X such that the expression
 * pBitField[X/32] & (1<<(X%32)) is zero.
 *
 * If all bits are set, returns numElements*32.
 *
 * @param[in] pBitField
 * @param[in] numBits  must be a multiple of 32.
 *
 * @return The highest zero bit, numElements*32 otherwise.
 */
NvU32 nvBitFieldMSZero(NvU32 *pBitField32, NvU32 numElements)
{
    NvU32 i = 0, j = numElements - 1;

    while (i++ < numElements)
    {
        NvU32 temp = ~pBitField32[j];
        if (temp)
        {
            HIGHESTBITIDX_32(temp);
            return temp + j * sizeof(NvU32) * 8;
        }
        j--;
    }

    return numElements * 32;
}

NvBool nvBitFieldTest(NvU32 *pBitField, NvU32 numElements, NvU32 bit)
{
    return (bit < numElements*32 ? (NvBool) !!(pBitField[bit/32] & NVBIT(bit%32)) : NV_FALSE);
}

void nvBitFieldSet(NvU32 *pBitField, NvU32 numElements, NvU32 bit, NvBool val)
{
    NV_ASSERT(bit < numElements*32);
    pBitField[bit/32] = (pBitField[bit/32] & ~NVBIT(bit%32)) | (val ? NVBIT(bit%32) : 0);
}

//
// Sort an array of n elements/structures.
// Example:
//    NvBool integerLess(void * a, void * b)
//    {
//      return *(NvU32 *)a < *(NvU32 *)b;
//    }
//    NvU32 array[1000];
//    ...
//    NvU32 temp[1000];
//    nvMergeSort(array, arrsize(array), temp, sizeof(NvU32), integerLess);
//
#define EL(n) ((char *)array+(n)*elementSize)
void nvMergeSort(void * array, NvU32 n, void * tempBuffer,  NvU32 elementSize, NvBool (*less)(void *, void *))
{
    char * mergeArray = (char *)tempBuffer;
    NvU32 m, i;

    //
    //  Bottom-up merge sort divides the sort into a sequence of passes.
    //  In each pass, the array is divided into blocks of size 'm'.
    //  Every pair of two adjacent blocks are merged (in place).
    //  The next pass is started with twice the block size
    //
    for (m = 1; m<=n; m*=2)
    {
        for (i = 0; i<(n-m); i+=2*m)
        {
            NvU32 loMin = i;
            NvU32 lo    = loMin;
            NvU32 loMax = i+m;
            NvU32 hi    = i+m;
            NvU32 hiMax = NV_MIN(n,i+2*m);

            char * dest = mergeArray;

            //
            // Standard merge of [lo, loMax) and [hi, hiMax)
            //
            while (1)
            {
                if (less(EL(lo), EL(hi)))
                {
                    portMemCopy(dest, elementSize, EL(lo), elementSize);
                    lo++;
                    dest+=elementSize;
                    if (lo >= loMax)
                        break;
                }
                else
                {
                    portMemCopy(dest, elementSize, EL(hi), elementSize);
                    hi++;
                    dest+=elementSize;
                    if (hi >= hiMax)
                        break;
                }
            }

            //
            // Copy remaining items (only one of these loops can run)
            //
            while (lo < loMax)
            {
                portMemCopy(dest, elementSize,EL(lo), elementSize);
                dest+=elementSize;
                lo++;
            }

            while (hi < hiMax)
            {
                portMemCopy(dest, elementSize,EL(hi), elementSize);
                dest+=elementSize;
                hi++;
            }

            //
            // Copy merged data back over array
            //
            portMemCopy(EL(loMin), (NvU32)(dest - mergeArray), mergeArray, (NvU32)(dest - mergeArray));
        }
    }
}

#define RANGE(val,low,hi) (((val) >= (low)) && ((val) <= (hi)))

// Do not conflict with libc naming
NvS32 nvStrToL
(
    NvU8* pStr,
    NvU8** pEndStr,
    NvS32 base,
    NvU8 stopChar,
    NvU32 *numFound
)
{
    NvU32 num;
    NvU32 newnum;

    *numFound = 0;

    // scan for start of number
    for (;*pStr;pStr++)
    {
        if (RANGE(*pStr, '0', '9'))
        {
            *numFound = 1;
            break;
        }
        else if ((BASE16 == base) && (RANGE(*pStr,'a','f')))
        {
            *numFound = 1;
            break;
        }
        else if ((BASE16 == base) && (RANGE(*pStr,'A', 'F')))
        {
            *numFound = 1;
            break;
        }
        else if(*pStr == stopChar)
        {
            break;
        }
    }

    // convert number
    num = 0;
    for (;*pStr;pStr++)
    {
        if (RANGE(*pStr, '0', '9'))
        {
            newnum = *pStr - '0';
        }
        else if ((BASE16 == base) && (RANGE(*pStr,'a','f')))
        {
            newnum = *pStr - 'a' + 10;
        }
        else if ((BASE16 == base) && (RANGE(*pStr,'A', 'F')))
        {
            newnum = *pStr - 'A' + 10;
        }
        else
            break;

        num *= base;
        num += newnum;

    }

    *pEndStr = pStr;

    return num;
}

/**
 * @brief Returns MSB of input as a bit mask
 *
 * @param x
 * @return MSB of x
 */
NvU64
nvMsb64(NvU64 x)
{
    x |= (x >> 1);
    x |= (x >> 2);
    x |= (x >> 4);
    x |= (x >> 8);
    x |= (x >> 16);
    x |= (x >> 32);
    //
    // At this point, x has same MSB as input, but with all 1's below it, clear
    // everything but MSB
    //
    return(x & ~(x >> 1));
}

/**
 * @brief Convert unsigned long int to char*
 *
 * @param value to be converted to string
 * @param *string is the char array to be have the converted data
 * @param radix denoted the base of the operation : hex(16),octal(8)..etc
 * @return the converted string
 */
char * nvU32ToStr(NvU32 value, char *string, NvU32 radix)
{
  char tmp[33];
  char *tp = tmp;
  NvS32 i;
  NvU32 v = value;
  char *sp;

  if (radix > 36 || radix <= 1)
  {
    return 0;
  }

  while (v || tp == tmp)
  {
    i = v % radix;
    v = v / radix;
    if (i < 10)
      *tp++ = (char)(i + '0');
    else
      *tp++ = (char)(i + 'a' - 10);
  }

  sp = string;

  while (tp > tmp)
    *sp++ = *--tp;
  *sp = 0;

  return string;
}


/**
 * @brief Get the string length
 *
 * @param string for which length has to be calculated
 * @return the string length
 */
NvU32 nvStringLen(const char * str)
{
    NvU32 i = 0;
    while (str[i++] != '\0')
        ;
    return i - 1;
}

