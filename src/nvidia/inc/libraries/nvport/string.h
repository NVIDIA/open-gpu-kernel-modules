/*
 * SPDX-FileCopyrightText: Copyright (c) 2016-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * @brief String module public interface
 */

#ifndef _NVPORT_H_
#error "This file cannot be included directly. Include nvport.h instead."
#endif

#ifndef _NVPORT_STRING_H_
#define _NVPORT_STRING_H_

/**
 * @defgroup NVPORT_STRING String module
 *
 * @brief This module contains string functionality used by other modules.
 *
 * @{
 */

/**
 * @name Core Functions
 * @{
 */

/**
 * @brief Compare two strings, character by character.
 * 
 * Will only compare lengthBytes bytes. Strings are assumed to be at least that
 * long.
 *
 * Strings are allowed to overlap, but in .
 *
 * @returns:
 * - 0 if all bytes are equal
 * - <0 if str1 is less than str2 for the first unequal byte.
 * - >0 if str1 is greater than str2 for the first unequal byte.
 * @par Undefined:
 * Behavior is undefined if str1, str2 is NULL. <br>
 */
NvS32 portStringCompare(const char *str1, const char *str2, NvLength length);
/**
 * @brief Copy a string.
 * 
 * Will copy at most destSize bytes, stopping early if a null-terminator is found
 * or if srcSize bytes are read from the source.
 *
 * Null character is always written at the end of the string.
 *
 * @param dest     destination buffer, of at least destSize bytes (including null terminator).
 * @param src      source buffer, of at least srcSize bytes (including null terminator).
 *
 * @return size    bytes copied successfully including null terminator, min(destSize, srcSize)
 *
 * @par Undefined:
 * Number of  allocated bytes in destination buffer are smaller than destSize. <br>
 * Behavior is undefined if destination and source overlaps. <br>
 */
NvLength portStringCopy(char *dest, NvLength destSize, const char *src, NvLength srcSize);
/**
 * @brief Concatenate two strings
 * 
 * Will copy cat string after the end of str. Will copy only until str buffer is
 * filled. str is assumed to point to a buffer of at least strSize bytes.
 *
 * Null character is always written at the end of the string.
 *
 * @return str if concatenation is succeeded. 
 *
 * @par Undefined:
 * Number of  allocated bytes in destination buffer are smaller than destSize. <br>
 * Behavior is undefined if destination and source overlaps. <br>
 */
char *portStringCat(char *str, NvLength strSize, const char *cat, NvLength catSize);


/**
 * @brief Returns the index of the first NULL byte in the given string
 *
 */
 NvLength portStringLength(const char *str);


 /**
 * @brief Returns the index of the first NULL byte in the given string, it searches maxLength
 * chars. If NULL byte is not found it returns maxLength.
 *
 */
 NvLength portStringLengthSafe(const char *str, NvLength maxLength);


/**
 * @brief Converts a string from ASCII (8-bit) to UTF16 (16 bit)
 *
 * Can perform the conversion in place if dest == src.
 *
 * @returns The number of characters in destination buffer, without the null
 * terminator (i.e. strlen(dest))
 */
NvLength portStringConvertAsciiToUtf16(NvU16 *dest, NvLength destSize, const char *src, NvLength srcSize);

/**
 * @brief Writes the hexadecimal string representation of the buffer
 *
 * @returns The number of characters in destination buffer, without the null
 * terminator (i.e. strlen(str))
 */
NvLength portStringBufferToHex(char *str, NvLength strSize, const NvU8 *buf, NvLength bufSize);

/**
 * @brief Convert a binary buffer into readable group of hex digits
 *
 * @param groupCount - Number of groups
 * @param groups     - How to structure the groups (in number of hex chars)
 * @param separator  - Character to separate the groups
 *
 * For the traditional display of UUIDs, there would be five groups, {8,4,4,4,12}
 * with the separator being '-'.
 *
 * @note odd numbers for group sizes are not supported, they will be rounded down
 *
 * @returns The number of characters in destination buffer, without the null
 * terminator (i.e. strlen(str))
 */
NvLength portStringBufferToHexGroups(char *str, NvLength strSize, const NvU8 *buf, NvLength bufSize, NvLength groupCount, const NvU32 *groups, const char *separator);

/// @} End core functions

/**
 * @name Extended Functions
 * @{
 */

// Place extended functions here

/// @} End extended functions

#endif // _NVPORT_STRING_H_
/// @}
