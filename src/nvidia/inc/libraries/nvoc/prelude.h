/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/*
 * This file is part of the NVOC runtime.
 */

#ifndef _NVOC_PRELUDE_H_
#define _NVOC_PRELUDE_H_

#include "nvtypes.h"
#include "utils/nvmacro.h"

/* Calls the macro named in the first parameter with the rest of the given arguments. Written
 * like this instead of just func(__VA_ARGS__) because some preprocessors treat __VA_ARGS__
 * as a single argument even when it contains commas. */
#define NVOC_PP_CALL(func, ...) NV_EXPAND(func NV_EXPAND() (__VA_ARGS__))

/*! Macro to help specify prefixes on NVOC classes */
#define NVOC_PREFIX(x) [[nvoc::prefix(x)]]

/*! Macro to help specify NVOC classes attributes */
#define NVOC_ATTRIBUTE(str)  [[nvoc::classAttributes("\""#str"\"")]]

/*! Macro to help specify properties on NVOC classes */
#define NVOC_PROPERTY [[nvoc::property]]

#ifndef NV_PRINTF_STRINGS_ALLOWED
#if defined(DEBUG) || defined(NV_MODS) || defined(QA_BUILD)
#define NV_PRINTF_STRINGS_ALLOWED 1
#else
#define NV_PRINTF_STRINGS_ALLOWED 0
#endif
#endif

/*!
 * @brief Gets a pointer to the NVOC_CLASS_INFO for the named NVOC class.
 *
 * This is similar to C++'s typeid macro.
 */
#define classInfo(x) reinterpretCast((&__nvoc_class_def_##x), const NVOC_CLASS_INFO *)

/*!
 * @brief Gets a unique integer identifier for the named NVOC class.
 *
 * This is similar to the hash_code of C++'s std::type_info.
 */
#define classId(x) __nvoc_class_id_##x


/*! NVOC class IDs will be no wider than NVOC_CLASS_ID_MAX_WIDTH bits. */
#define NVOC_CLASS_ID_MAX_WIDTH 24

/*!
 * @brief Statically casts pObj to a TYPE*.  Fails to compile if the cast is invalid.
 *
 * This is similar to C++'s static_cast<TYPE>(pObj).
 */
#define staticCast(pObj, TYPE) ((pObj)? __staticCast_##TYPE((pObj)) : NULL)

/*!
 * @brief Statically casts pObj to a TYPE*.  Fails to compile if the cast is invalid.
 *
 * This version staticCast() skips pointer check as a trade of better binary size and
 * runtime efficiency.  The caller is responsible to ensure pObj can never be NULL.
 */
#define staticCastNoPtrCheck(pObj, TYPE) __staticCast_##TYPE((pObj))

/*!
 * @brief Dynamically casts pObj to a TYPE*.  Returns NULL if the cast is invalid.
 *
 * This is similar to C++'s dynamic_cast<TYPE>(pObj).
 */
#define dynamicCast(pObj, TYPE) (__dynamicCast_##TYPE((pObj)))

/*!
 * @brief Reinterpret e as if it had type T.
 *
 * This is similar to C++'s reinterpret_cast<T>(e).
 */
#define reinterpretCast(e, T) ((T)(e))

/*!
 * NVOC_OBJ_CREATE_FLAGS* are used with objCreateWithFlags()/objCreateDynamicWithFlags().
 *
 * NVOC_OBJ_CREATE_FLAGS_NONE
 *     Default behavior
 * NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY
 *     Use halspec from parent without adding the new created object the child tree
 * NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT
 *     Skip memory allocation on object create, assume the argument points to memory
 *     already allocated.
 */
#define NVOC_OBJ_CREATE_FLAGS_NONE                          0x0000u
#define NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY           0x0001u
#define NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT            0x0002u

/*!
 * @brief Create and construct a new object by class name.
 *
 * @param[out] ppNewObj A pointer to the new object
 * @param[in]  pParent  A pointer to the object that should be the new object's parent,
 *                      or NULL if the new object has no parent.
 * @param[in]  NAME     The name of the class of the new object.
 */
/* MSVC suppresses trailing commas at the final expansion but not at intermediate expansions, so we
 * need to put our trailing comma inside another macro so it will be eaten.  Normally, one would
 * just wrap the trailing comma and __VA_ARGS__ in NV_EXPAND, but Bullseye's preprocessor has
 * trouble dealing with that properly, so instead we use an indirect macro caller that Bullseye
 * seems to handle properly.  This avoids producing a "too many arguments for macro" warning (C4002). */
#define objCreate(ppNewObj, pParent, NAME, ...)                 \
    NVOC_PP_CALL(__objCreate_##NAME, (ppNewObj), (pParent), (NVOC_OBJ_CREATE_FLAGS_NONE), ##__VA_ARGS__)
#define objCreateWithFlags(ppNewObj, pParent, NAME, flags, ...) \
    NVOC_PP_CALL(__objCreate_##NAME, (ppNewObj), (pParent), (flags), ##__VA_ARGS__)

/*!
 * @brief Destruct and free an object and all of its children recursively.
 *
 * In C++, fields are destructed in reverse syntactic order.  Similarly, in
 * NVOC, runtime children are deleted in the reverse of the order they were
 * added (usually reverse creation order).
 */
#define objDelete(pObj) __nvoc_objDelete(staticCast((pObj), Dynamic))

/*!
 * @brief Get the given object's class ID
 */
#define objGetClassId(pObj) __nvoc_objGetClassId(staticCast((pObj), Dynamic))

/*!
 * @brief Get the given object's class info
 */
#define objGetClassInfo(pObj) __nvoc_objGetClassInfo(staticCast((pObj), Dynamic))

#if NV_PRINTF_STRINGS_ALLOWED
/*!
 * Get the given class's name from its class info.
 */
#define objGetClassName(pObj)               (objGetClassInfo((pObj))->name)
#endif

/*!
 * @brief Create and construct a new object by class ID.
 *
 * @param[out] ppNewObj   A pointer to the new object
 * @param[in]  pParent    A pointer to the object that should be the new object's parent,
 *                        or NULL if the new object has no parent.
 * @param[in]  pClassInfo A pointer to the NVOC_CLASS_INFO for the desired class.
 */
#define objCreateDynamic(ppNewObj, pParent, pClassInfo, ...)                       \
    __nvoc_objCreateDynamic((ppNewObj), staticCast((pParent), Dynamic),            \
                            (pClassInfo), (NVOC_OBJ_CREATE_FLAGS_NONE), ##__VA_ARGS__)
#define objCreateDynamicWithFlags(ppNewObj, pParent, pClassInfo, flags, ...) \
    __nvoc_objCreateDynamic((ppNewObj), staticCast((pParent), Dynamic),      \
                            (pClassInfo), (flags), ##__VA_ARGS__)

/*!
 * @brief Cast any object supporting Run-Time Type Information (RTTI) to 'Dynamic'.
 *
 * The purpose of this more complicated expression is to force a compile-time
 * error if `pObj` does not contain Metadata/RTTI information.
 *
 * Since the `__nvoc_rtti` pointer is always first, `pObj == &(pObj)->__nvoc_rtti`.
 * With metadata version 2, `__nvoc_rtti` is unioned with `__nvoc_metadata`,
 * which is okay since the RTTI structure is first in the metadata structure.
 *
 */
#define __staticCast_Dynamic(pObj) ((Dynamic*) &(pObj)->__nvoc_rtti)


/*
 * Helper macros for "pObject->getProperty(pObject, prop)"
 *
 * The NVOC property macros are currently based on IOM's property macros.
 *
 * Property inheritance for IOM (Improved Object Model) is done by introducing
 * 'prop##_BASE_CAST' and 'prop##_BASE_NAME'.  For IOM, those are defined in
 * generated file g_odb.h.  For NVOC, they are defined in each class's generated
 * header.
 *
 * In non-inheritance cases, getProperty/setProperty functions are equal to:
 *      #define getProperty(pObj, prop)                prop // or pdb.prop for IOM
 *      #define setProperty(pObj, prop, val)           prop = val // or pdb.prop = val for IOM
 *
 * Once the IOM model is phased out, these will become:
 *      #define getProperty(pObj, prop)                pObj->prop
 *      #define setProperty(pObj, prop, val)           pObj->prop = val
 */
#define getProperty(pObj, prop)         prop##_BASE_CAST prop##_BASE_NAME
#define setProperty(pObj, prop, val)    prop##_BASE_CAST prop##_BASE_NAME = val

/*! Special NULL pointer for macros that expect to staticCast their parameter */
#define NVOC_NULL_OBJECT ((Object*) NULL)


/*!
 * @brief Wrapper of the Run-Time Type Information (RTTI) pointer.
 *
 * @details In effect, this is the base class (not Object) for all classes
 * that support RTTI because the RTTI pointer is always first in memory,
 */
typedef struct {
    const struct NVOC_RTTI *__nvoc_rtti;
} Dynamic;


typedef NvU32 NVOC_CLASS_ID;

typedef struct NVOC_RTTI_PROVIDER {
    NvU32 dummy;
} NVOC_RTTI_PROVIDER;

typedef const NVOC_RTTI_PROVIDER *NVOC_RTTI_PROVIDER_ID;

//! Public metadata about an NVOC class definition.
typedef struct NVOC_CLASS_INFO
{
    const NvU32                  size;
    const NVOC_CLASS_ID          classId;
    const NVOC_RTTI_PROVIDER_ID  providerId;
#if NV_PRINTF_STRINGS_ALLOWED
    const char                  *name;
#endif
} NVOC_CLASS_INFO;


/*!
 * @brief Wrapper of private field and private function
 */
#if defined(__clang__) // clang
#define NVOC_PRIVATE_FIELD(x)       __attribute__((unavailable(#x " is a private field"))) x
#define NVOC_PRIVATE_FUNCTION(x)    __attribute__((unavailable(#x " is a private function"))) x
#elif defined(__INTEL_COMPILER) // icc
#pragma warning(error: 1786) // treat deprecated as error (globally affected)
#define NVOC_PRIVATE_FIELD(x)       __attribute__((deprecated(#x " is a private field"))) x
#define NVOC_PRIVATE_FUNCTION(x)    __attribute__((deprecated(#x " is a private function"))) x
#elif defined(__GNUC__) || defined(__GNUG__) // gcc
#pragma GCC diagnostic error "-Wdeprecated-declarations" // treat deprecated as error (globally affected)
#define NVOC_PRIVATE_FIELD(x)       __attribute__((deprecated(#x " is a private field"))) x
#define NVOC_PRIVATE_FUNCTION(x)    __attribute__((error(#x " is a private function"))) x
#else // other
#define NVOC_PRIVATE_FIELD(x)       x##_PRIVATE
#define NVOC_PRIVATE_FUNCTION(x)    x##_PRIVATE
#endif

#endif
