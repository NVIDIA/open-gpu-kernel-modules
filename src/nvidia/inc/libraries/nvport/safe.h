/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2015 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * @brief Safe module public interface
 */

#ifndef _NVPORT_H_
#error "This file cannot be included directly. Include nvport.h instead."
#endif

#ifndef _NVPORT_SAFE_H_
#define _NVPORT_SAFE_H_

#ifndef PORT_SAFE_INLINE
#define PORT_SAFE_INLINE PORT_INLINE
#endif

/**
 * @defgroup NVPORT_SAFE Safe integer operations module
 *
 * @brief This module contains functions for safe use of integer types, without
 * concern for overflow errors
 *
 * All functions return true if the operation was performed successfully,
 * and false if there was an overflow (or division by zero).
 * The final value is handed back in both cases, but if the function returned
 * false, the value handed back is undefined.
 *
 * @note These functions should be used:
 *  - When operating on data passing through the trust boundary (e.g. RM API)
 *  - When operating on signed data types (where overflows are undefined!)
 *  - Instead of checking overflows manually
 * For internal functions, it is recommended not to clutter the code with this.
 * Usually an internal overflow is a bug, and it should be fixed up the stack.
 *
 *
 * @note A couple of artificial examples:
 *  - GOOD - Data received from user, not to be trusted.
 * ~~~{.c}
 *  NvU32 NV_APIENTRY NvRmBzero(NvU8 *mem, NvLength len)
 *  {
 *      NvUPtr uptr = (NvUPtr) mem;
 *      if (mem == NULL || !portSafeAddUPtr(uptr, len, &uptr))
 *          return NV_ERR_INVALID_PARAMETER;
 *      while ((NvUPtr) mem != uptr)
 *          *mem++ = 0;
 *      return NV_OK;
 *  }
 * ~~~
 *  - GOOD - Internal RM function, allowed to crash if given invalid params
 * ~~~{.c}
 *  void bzero(NvU8 *mem, NvLength len)
 *  {
 *      while (len > 0)
 *          mem[--len] = 0;
 *  }
 * ~~~
 *  - BAD - If you are already checking for overflows manually, use these functions
 * ~~~{.c}
 * NV_STATUS osAllocMemTracked(void **ppAddress, NvU32 size)
 * {
 *      NvU32 paddedSize = size;
 *      // allocate three extra dwords to hold the size and some debug tags
 *      paddedSize += 3 * sizeof(NvU32);
 *      // check for the overflow after increasing the size
 *      if (paddedSize < size)
 *          return NV_ERR_INSUFFICIENT_RESOURCES;
 *      size = paddedSize;
 *      ...
 * }
 * ~~~
 *  - GOOD - Use provided functions
 * ~~~{.c}
 * NV_STATUS osAllocMemTracked(void **ppAddress, NvU32 size)
 * {
 *      if (!portSafeAddU32(size, 3*sizeof(NvU32), &size))
 *          return NV_ERR_INSUFFICIENT_RESOURCES;
 *      ...
 * }
 * ~~~
 *
 * @{
 */

/**
 * @name Core Functions
 * @{
 */

/**
 * @brief Add two signed 8bit integers, writing the result to *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeAddS8(NvS8 augend, NvS8 addend, NvS8 *pResult);
/**
 * @brief Subtract two signed 8bit integers, writing the result to *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeSubS8(NvS8 minuend, NvS8 subtrahend, NvS8 *pResult);
/**
 * @brief Multiply two signed 8bit integers, writing the result to *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeMulS8(NvS8 multiplicand, NvS8 multiplier, NvS8 *pResult);
/**
 * @brief Divide two signed 8bit integers, writing the result to *pResult.
 *
 * @return NV_ERR_INVALID_PARAMETER if divisor is zero
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeDivS8(NvS8 dividend, NvS8 divisor, NvS8 *pResult);


/**
 * @brief Add two signed 16bit integers, writing the result to *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeAddS16(NvS16 augend, NvS16 addend, NvS16 *pResult);
/**
 * @brief Subtract two signed 16bit integers, writing the result to *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeSubS16(NvS16 minuend, NvS16 subtrahend, NvS16 *pResult);
/**
 * @brief Multiply two signed 16bit integers, writing the result to *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeMulS16(NvS16 multiplicand, NvS16 multiplier, NvS16 *pResult);
/**
 * @brief Divide two signed 16bit integers, writing the result to *pResult.
 *
 * @return NV_ERR_INVALID_PARAMETER if divisor is zero
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeDivS16(NvS16 dividend, NvS16 divisor, NvS16 *pResult);


/**
 * @brief Add two signed 32bit integers, writing the result to *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeAddS32(NvS32 augend, NvS32 addend, NvS32 *pResult);
/**
 * @brief Subtract two signed 32bit integers, writing the result to *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeSubS32(NvS32 minuend, NvS32 subtrahend, NvS32 *pResult);
/**
 * @brief Multiply two signed 32bit integers, writing the result to *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeMulS32(NvS32 multiplicand, NvS32 multiplier, NvS32 *pResult);
/**
 * @brief Divide two signed 32bit integers, writing the result to *pResult.
 *
 * @return NV_ERR_INVALID_PARAMETER if divisor is zero
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeDivS32(NvS32 dividend, NvS32 divisor, NvS32 *pResult);


/**
 * @brief Add two signed 64bit integers, writing the result to *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeAddS64(NvS64 augend, NvS64 addend, NvS64 *pResult);
/**
 * @brief Subtract two signed 64bit integers, writing the result to *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeSubS64(NvS64 minuend, NvS64 subtrahend, NvS64 *pResult);
/**
 * @brief Multiply two signed 64bit integers, writing the result to *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeMulS64(NvS64 multiplicand, NvS64 multiplier, NvS64 *pResult);
/**
 * @brief Divide two signed 64bit integers, writing the result to *pResult.
 *
 * @return NV_ERR_INVALID_PARAMETER if divisor is zero
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeDivS64(NvS64 dividend, NvS64 divisor, NvS64 *pResult);




/**
 * @brief Add two unsigned 8bit integers, writing the result to *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeAddU8(NvU8 augend, NvU8 addend, NvU8 *pResult);
/**
 * @brief Subtract two unsigned 8bit integers, writing the result to *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeSubU8(NvU8 minuend, NvU8 subtrahend, NvU8 *pResult);
/**
 * @brief Multiply two unsigned 8bit integers, writing the result to *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeMulU8(NvU8 multiplicand, NvU8 multiplier, NvU8 *pResult);
/**
 * @brief Divide two unsigned 8bit integers, writing the result to *pResult.
 *
 * @return NV_ERR_INVALID_PARAMETER if divisor is zero
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeDivU8(NvU8 dividend, NvU8 divisor, NvU8 *pResult);


/**
 * @brief Add two unsigned 16bit integers, writing the result to *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeAddU16(NvU16 augend, NvU16 addend, NvU16 *pResult);
/**
 * @brief Subtract two unsigned 16bit integers, writing the result to *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeSubU16(NvU16 minuend, NvU16 subtrahend, NvU16 *pResult);
/**
 * @brief Multiply two unsigned 16bit integers, writing the result to *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeMulU16(NvU16 multiplicand, NvU16 multiplier, NvU16 *pResult);
/**
 * @brief Divide two unsigned 16bit integers, writing the result to *pResult.
 *
 * @return NV_ERR_INVALID_PARAMETER if divisor is zero
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeDivU16(NvU16 dividend, NvU16 divisor, NvU16 *pResult);


/**
 * @brief Add two unsigned 32bit integers, writing the result to *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeAddU32(NvU32 augend, NvU32 addend, NvU32 *pResult);
/**
 * @brief Subtract two unsigned 32bit integers, writing the result to *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeSubU32(NvU32 minuend, NvU32 subtrahend, NvU32 *pResult);
/**
 * @brief Multiply two unsigned 32bit integers, writing the result to *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeMulU32(NvU32 multiplicand, NvU32 multiplier, NvU32 *pResult);
/**
 * @brief Divide two unsigned 32bit integers, writing the result to *pResult.
 *
 * @return NV_ERR_INVALID_PARAMETER if divisor is zero
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeDivU32(NvU32 dividend, NvU32 divisor, NvU32 *pResult);


/**
 * @brief Add two unsigned 64bit integers, writing the result to *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeAddU64(NvU64 augend, NvU64 addend, NvU64 *pResult);
/**
 * @brief Subtract two unsigned 64bit integers, writing the result to *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeSubU64(NvU64 minuend, NvU64 subtrahend, NvU64 *pResult);
/**
 * @brief Multiply two unsigned 64bit integers, writing the result to *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeMulU64(NvU64 multiplicand, NvU64 multiplier, NvU64 *pResult);
/**
 * @brief Divide two unsigned 64bit integers, writing the result to *pResult.
 *
 * @return NV_ERR_INVALID_PARAMETER if divisor is zero
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeDivU64(NvU64 dividend, NvU64 divisor, NvU64 *pResult);


/**
 * @brief Add two pointer-sized integers, writing the result to *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeAddUPtr(NvUPtr augend, NvUPtr addend, NvUPtr *pResult);
/**
 * @brief Subtract two pointer-sized integers, writing the result to *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeSubUPtr(NvUPtr minuend, NvUPtr subtrahend, NvUPtr *pResult);
/**
 * @brief Multiply two pointer-sized integers, writing the result to *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeMulUPtr(NvUPtr multiplicand, NvUPtr multiplier, NvUPtr *pResult);
/**
 * @brief Divide two pointer-sized integers, writing the result to *pResult.
 *
 * @return NV_ERR_INVALID_PARAMETER if divisor is zero
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeDivUPtr(NvUPtr dividend, NvUPtr divisor, NvUPtr *pResult);


/**
 * @brief Add two length integers, writing the result to *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeAddLength(NvLength augend, NvLength addend, NvLength *pResult);
/**
 * @brief Subtract two length integers, writing the result to *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeSubLength(NvLength minuend, NvLength subtrahend, NvLength *pResult);
/**
 * @brief Multiply two length integers, writing the result to *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeMulLength(NvLength multiplicand, NvLength multiplier, NvLength *pResult);
/**
 * @brief Divide two length integers, writing the result to *pResult.
 *
 * @return NV_ERR_INVALID_PARAMETER if divisor is zero
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeDivLength(NvLength dividend, NvLength divisor, NvLength *pResult);






/**
 * @brief Convert a 8bit signed integer to a 8bit unsigned integer *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeS8ToU8(NvS8 data, NvU8 *pResult);
/**
 * @brief Convert a 8bit signed integer to a 16bit unsigned integer *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeS8ToU16(NvS8 data, NvU16 *pResult);
/**
 * @brief Convert a 8bit signed integer to a 32bit unsigned integer *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeS8ToU32(NvS8 data, NvU32 *pResult);
/**
 * @brief Convert a 8bit signed integer to a 64bit unsigned integer *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeS8ToU64(NvS8 data, NvU64 *pResult);
/**
 * @brief Convert a 8bit signed integer to a pointer-sized unsigned integer *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeS8ToUPtr(NvS8 data, NvUPtr *pResult);
/**
 * @brief Convert a 8bit signed integer to a pointer-sized unsigned integer *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeS8ToLength(NvS8 data, NvLength *pResult);


/**
 * @brief Convert a 16bit signed integer to a 8bit signed integer *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeS16ToS8(NvS16 data, NvS8 *pResult);
/**
 * @brief Convert a 16bit signed integer to a 8bit unsigned integer *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeS16ToU8(NvS16 data, NvU8 *pResult);
/**
 * @brief Convert a 16bit signed integer to a 16bit unsigned integer *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeS16ToU16(NvS16 data, NvU16 *pResult);
/**
 * @brief Convert a 16bit signed integer to a 32bit unsigned integer *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeS16ToU32(NvS16 data, NvU32 *pResult);
/**
 * @brief Convert a 16bit signed integer to a 64bit unsigned integer *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeS16ToU64(NvS16 data, NvU64 *pResult);
/**
 * @brief Convert a 16bit signed integer to a pointer-sized unsigned integer *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeS16ToUPtr(NvS16 data, NvUPtr *pResult);
/**
 * @brief Convert a 16bit signed integer to a pointer-sized unsigned integer *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeS16ToLength(NvS16 data, NvLength *pResult);

/**
 * @brief Convert a 32bit signed integer to a 8bit signed integer *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeS32ToS8(NvS32 data, NvS8 *pResult);
/**
 * @brief Convert a 32bit signed integer to a 16bit signed integer *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeS32ToS16(NvS32 data, NvS16 *pResult);
/**
 * @brief Convert a 32bit signed integer to a 8bit unsigned integer *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeS32ToU8(NvS32 data, NvU8 *pResult);
/**
 * @brief Convert a 32bit signed integer to a 16bit unsigned integer *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeS32ToU16(NvS32 data, NvU16 *pResult);
/**
 * @brief Convert a 32bit signed integer to a 32bit unsigned integer *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeS32ToU32(NvS32 data, NvU32 *pResult);
/**
 * @brief Convert a 32bit signed integer to a 64bit unsigned integer *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeS32ToU64(NvS32 data, NvU64 *pResult);
/**
 * @brief Convert a 32bit signed integer to a pointer-sized unsigned integer *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeS32ToUPtr(NvS32 data, NvUPtr *pResult);
/**
 * @brief Convert a 32bit signed integer to a pointer-sized unsigned integer *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeS32ToLength(NvS32 data, NvLength *pResult);


/**
 * @brief Convert a 64bit signed integer to a 8bit signed integer *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeS64ToS8(NvS64 data, NvS8 *pResult);
/**
 * @brief Convert a 64bit signed integer to a 16bit signed integer *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeS64ToS16(NvS64 data, NvS16 *pResult);
/**
 * @brief Convert a 64bit signed integer to a 32bit signed integer *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeS64ToS32(NvS64 data, NvS32 *pResult);
/**
 * @brief Convert a 64bit signed integer to a 8bit unsigned integer *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeS64ToU8(NvS64 data, NvU8 *pResult);
/**
 * @brief Convert a 64bit signed integer to a 16bit unsigned integer *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeS64ToU16(NvS64 data, NvU16 *pResult);
/**
 * @brief Convert a 64bit signed integer to a 32bit unsigned integer *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeS64ToU32(NvS64 data, NvU32 *pResult);
/**
 * @brief Convert a 64bit signed integer to a 64bit unsigned integer *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeS64ToU64(NvS64 data, NvU64 *pResult);
/**
 * @brief Convert a 64bit signed integer to a pointer-sized unsigned integer *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeS64ToUPtr(NvS64 data, NvUPtr *pResult);
/**
 * @brief Convert a 64bit signed integer to a pointer-sized unsigned integer *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeS64ToLength(NvS64 data, NvLength *pResult);



/**
 * @brief Convert a 8bit unsigned integer to a 8bit signed integer *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeU8ToS8(NvU8 data, NvS8 *pResult);

/**
 * @brief Convert a 16bit unsigned integer to a 8bit signed integer *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeU16ToS8(NvU16 data, NvS8 *pResult);
/**
 * @brief Convert a 16bit unsigned integer to a 16bit signed integer *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeU16ToS16(NvU16 data, NvS16 *pResult);
/**
 * @brief Convert a 16bit unsigned integer to a 8bit unsigned integer *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeU16ToU8(NvU16 data, NvU8 *pResult);


/**
 * @brief Convert a 32bit unsigned integer to a 8bit signed integer *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeU32ToS8(NvU32 data, NvS8 *pResult);
/**
 * @brief Convert a 32bit unsigned integer to a 16bit signed integer *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeU32ToS16(NvU32 data, NvS16 *pResult);
/**
 * @brief Convert a 32bit unsigned integer to a 32bit signed integer *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeU32ToS32(NvU32 data, NvS32 *pResult);
/**
 * @brief Convert a 32bit unsigned integer to a 8bit unsigned integer *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeU32ToU8(NvU32 data, NvU8 *pResult);
/**
 * @brief Convert a 32bit unsigned integer to a 16bit unsigned integer *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeU32ToU16(NvU32 data, NvU16 *pResult);


/**
 * @brief Convert a 64bit unsigned integer to a 8bit signed integer *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeU64ToS8(NvU64 data, NvS8 *pResult);
/**
 * @brief Convert a 64bit unsigned integer to a 16bit signed integer *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeU64ToS16(NvU64 data, NvS16 *pResult);
/**
 * @brief Convert a 64bit unsigned integer to a 32bit signed integer *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeU64ToS32(NvU64 data, NvS32 *pResult);
/**
 * @brief Convert a 64bit unsigned integer to a 64bit signed integer *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeU64ToS64(NvU64 data, NvS64 *pResult);
/**
 * @brief Convert a 64bit unsigned integer to a 8bit unsigned integer *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeU64ToU8(NvU64 data, NvU8 *pResult);
/**
 * @brief Convert a 64bit unsigned integer to a 16bit unsigned integer *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeU64ToU16(NvU64 data, NvU16 *pResult);
/**
 * @brief Convert a 64bit unsigned integer to a 32bit unsigned integer *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeU64ToU32(NvU64 data, NvU32 *pResult);
/**
 * @brief Convert a 64bit unsigned integer to a pointer-sized unsigned integer *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeU64ToUPtr(NvU64 data, NvUPtr *pResult);
/**
 * @brief Convert a 64bit unsigned integer to a pointer-sized unsigned integer *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeU64ToLength(NvU64 data, NvLength *pResult);


/**
 * @brief Convert a pointer-sized unsigned integer to a 8bit signed integer *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeUPtrToS8(NvUPtr data, NvS8 *pResult);
/**
 * @brief Convert a pointer-sized unsigned integer to a 16bit signed integer *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeUPtrToS16(NvUPtr data, NvS16 *pResult);
/**
 * @brief Convert a pointer-sized unsigned integer to a 32bit signed integer *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeUPtrToS32(NvUPtr data, NvS32 *pResult);
/**
 * @brief Convert a pointer-sized unsigned integer to a 64bit signed integer *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeUPtrToS64(NvUPtr data, NvS64*pResult);
/**
 * @brief Convert a pointer-sized unsigned integer to a 8bit unsigned integer *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeUPtrToU8(NvUPtr data, NvU8 *pResult);
/**
 * @brief Convert a pointer-sized unsigned integer to a 16bit unsigned integer *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeUPtrToU16(NvUPtr data, NvU16 *pResult);
/**
 * @brief Convert a pointer-sized unsigned integer to a 32bit unsigned integer *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeUPtrToU32(NvUPtr data, NvU32 *pResult);
/**
 * @brief Convert a pointer-sized unsigned integer to a 64bit unsigned integer *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeUPtrToU64(NvUPtr data, NvU64 *pResult);
/**
 * @brief Convert a pointer-sized unsigned integer to a pointer-sized unsigned integer *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeUPtrToLength(NvUPtr data, NvLength *pResult);


/**
 * @brief Convert a length-sized unsigned integer to a 8bit signed integer *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeLengthToS8(NvLength data, NvS8 *pResult);
/**
 * @brief Convert a length-sized unsigned integer to a 16bit signed integer *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeLengthToS16(NvLength data, NvS16 *pResult);
/**
 * @brief Convert a length-sized unsigned integer to a 32bit signed integer *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeLengthToS32(NvLength data, NvS32 *pResult);
/**
 * @brief Convert a length-sized unsigned integer to a 64bit signed integer *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeLengthToS64(NvLength data, NvS64 *pResult);
/**
 * @brief Convert a length-sized unsigned integer to a 8bit unsigned integer *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeLengthToU8(NvLength data, NvU8 *pResult);
/**
 * @brief Convert a length-sized unsigned integer to a 16bit unsigned integer *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeLengthToU16(NvLength data, NvU16 *pResult);
/**
 * @brief Convert a length-sized unsigned integer to a 32bit unsigned integer *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeLengthToU32(NvLength data, NvU32 *pResult);
/**
 * @brief Convert a length-sized unsigned integer to a 64bit unsigned integer *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeLengthToU64(NvLength data, NvU64 *pResult);
/**
 * @brief Convert a length-sized unsigned integer to a pointer-sized unsigned integer *pResult.
 */
PORT_SAFE_INLINE NvBool NV_FORCERESULTCHECK portSafeLengthToUPtr(NvLength data, NvUPtr *pResult);



/// @} End core functions

/**
 * @name Extended Functions
 * @{
 */

// Place extended functions here

/// @} End extended functions


#if NVOS_IS_WINDOWS
#include "nvport/inline/safe_win.h"
#else
#include "nvport/inline/safe_generic.h"
#endif

#endif // _NVPORT_SAFE_H_
/// @}
