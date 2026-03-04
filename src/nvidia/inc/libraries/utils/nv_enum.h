/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * @file  nv_enum.h
 * @brief A header providing code-generation tools to define an enumerated type
 *        from a specification of a potentially-nested enum of limited depth.
 * @see confluence page "Proposal for Better Enum Generation (NV_ENUM) Version 1.0"
 */

/**
 * @defgroup NV_UTILS_ENUM Infrastructure for generating better enumerated values.
 *
 * @brief Generates symbols comprising an enumerated type given a list of
 *          entries provided via macro argument.
 *
 * This file exposes macro functions which generate enum types and associated
 * metadata from an enum specification consisting of entry names and values,
 * with support for nesting enums up to a maximum depth of  2. The associated
 * metadata generated from the enum specification allows for conversion of
 * contiguous enums (those without holes within their valid value range) to and
 * from indices, iteration over enum values (for each loop) and runtime
 * determination of whether a given integer is a valid enum value. Additionally,
 * macros are provided to "export" an enum such that only the entry names and
 * values are defined, and no name is given to the enum. This is useful in
 * situations where two different APIs utilize the same enum definition, such as
 * in the RM SDK where enums are disallowed in control call parameters, but are
 * very much desired inside of the driver.
 * @{
 */

#ifndef NV_ENUM_H_
#define NV_ENUM_H_

#define NV_ENUM_MIN ((NvS64) 0)
#define NV_ENUM_MAX ((NvS64)0xFFFFFFFF)

/** @brief Fully expands both arguments, then concatenates them. */
#define NV_ENUM_CONCATENATE(a, b) NV_ENUM_CONCATENATE2(a, b)
#define NV_ENUM_CONCATENATE2(a, b) _NV_ENUM_CONCATENATE(a, b)
#define _NV_ENUM_CONCATENATE(a, b) a##b

/** @brief Fully expands the given argument, then stringifies it. */
#define NV_ENUM_STRINGIFY(s) _NV_ENUM_STRINGIFY(s)
#define _NV_ENUM_STRINGIFY(s) #s

/** @brief Expands the given argument. */
#define NV_EXPAND_ONE(x) x

/** @brief Discards the given argument, expands to nothing. */
#define NV_DISCARD_ONE(x)


/**
 * @brief Fully expands both arguments, then attempts to invoke parameter `a` as
 *        a macro with parameter `b` as its argument.
 *
 * @param a : Macro                 Macro to apply
 * @param b : Argument List         Arguments to pass to macro `a`, surrounded by parentehses
 */
#define NV_ENUM_APPLY(a, b) _NV_ENUM_APPLY(a, b)
#define _NV_ENUM_APPLY(a, b) a b

/** @brief expands to the Nth argument */
#define NV_ENUM_A1(a, b, c, d, e, f) a
#define NV_ENUM_A2(a, b, c, d, e, f) b
#define NV_ENUM_A3(a, b, c, d, e, f) c
#define NV_ENUM_A4(a, b, c, d, e, f) d
#define NV_ENUM_A5(a, b, c, d, e, f) e
#define NV_ENUM_A6(a, b, c, d, e, f) f

/**
 * @brief Expands to an argument list containing 6 elements with argument `b`
 *        moved to the last place.
 */
#define NV_ENUM_DL_POP(a, b, c, d, e, f) (a, c, d, e, f, b)

/**
 * @brief Expands to argument list `l` with its first element replaced by
 *        parameter `r`
 */
#define NV_ENUM_NV_ENUM_REPLACE_1(r, l) (r, NV_ENUM_APPLY(NV_ENUM_A2, l), NV_ENUM_APPLY(NV_ENUM_A3, l), NV_ENUM_APPLY(NV_ENUM_A4, l), NV_ENUM_APPLY(NV_ENUM_A5, l), NV_ENUM_APPLY(NV_ENUM_A6, l))

/**
 * @brief Expands to argument list `l` with its first element replaced by
 *        parameter `r1`, its fifth argument replaced by parameter `r5`, and its
 *        sixth argument replaced by parameter `r6`
 */
#define NV_ENUM_REPLACE_3(r1, r5, r6, l) (r1, NV_ENUM_APPLY(NV_ENUM_A2, l), NV_ENUM_APPLY(NV_ENUM_A3, l), NV_ENUM_APPLY(NV_ENUM_A4, l), r5, r6)

/**
 * @brief Expands to argument list `l` with its first element replaced by
 *        parameter `r1`, its second argument replaced by parameter `r2`, its
 *        fifth argument replaced by parameter `r5`, and its sixth argument
 *        replaced by parameter `r6`
 */
#define NV_ENUM_REPLACE_4(r1, r2, r5, r6, l) (r1, r2, NV_ENUM_APPLY(NV_ENUM_A3, l), NV_ENUM_APPLY(NV_ENUM_A4, l), r5, r6)


/*!
 * @brief Convenience LISP-like wrappers for CAR and CDR
 *
 * @note For those unfamiliar with LISP, most LISP interpreters allow for
 *       convenient macros which expand to nested invocations of CAR and CDR,
 *       formed by specifying 'A' and 'D' in any order between 'C' and 'R'. A
 *       regular expression which identifies this pattern is: 'C(A|D)+R'. The
 *       order of operations is performed from right to left, e.g. CAAADR
 *       applies CDR, then CAR, then CAR, then CAR. These are used to unpack
 *       data at specific locations within nested lists, which this tool uses
 *       often. There is no such thing as a meta-macro in the c preprocessor, so
 *       we have defined the operations which we use frequently here.
 *
 * @note instead of LISP-style structured lists which are formatted as
 *       records containing two elements each (e.g. (car, (cdr, ()))), this tool
 *       uses preprocessor argument lists (e.g. (car, cdr, etc)) because the
 *       former require proper recursion to deal with, which this tool does not
 *       have available to it.
 *
 * @note Because some compilers do not support variadic macros, we cannot use
 *       the generic versions of CAR and CDR here, so we have replaced them
 *       with very specific size-restricted versions.
 */
#define NV_CAAR(l)       NV_ENUM_APPLY(NV_ENUM_A1, NV_ENUM_APPLY(NV_ENUM_A1, l))
#define NV_CADAR(l)      NV_ENUM_APPLY(NV_ENUM_A2, NV_ENUM_APPLY(NV_ENUM_A1, l))
#define NV_CADDAR(l)     NV_ENUM_APPLY(NV_ENUM_A3, NV_ENUM_APPLY(NV_ENUM_A1, l))
#define NV_CADDDAR(l)    NV_ENUM_APPLY(NV_ENUM_A4, NV_ENUM_APPLY(NV_ENUM_A1, l))
#define NV_CADDDDAR(l)   NV_ENUM_APPLY(NV_ENUM_A5, NV_ENUM_APPLY(NV_ENUM_A1, l))
#define NV_CADDDDDAR(l)  NV_ENUM_APPLY(NV_ENUM_A6, NV_ENUM_APPLY(NV_ENUM_A1, l))
#define NV_CAADR(l)      NV_ENUM_APPLY(NV_ENUM_A1, NV_ENUM_APPLY(NV_ENUM_A2, l))
#define NV_CADADR(l)     NV_ENUM_APPLY(NV_ENUM_A2, NV_ENUM_APPLY(NV_ENUM_A2, l))
#define NV_CADDADR(l)    NV_ENUM_APPLY(NV_ENUM_A3, NV_ENUM_APPLY(NV_ENUM_A2, l))
#define NV_CADDDADR(l)   NV_ENUM_APPLY(NV_ENUM_A4, NV_ENUM_APPLY(NV_ENUM_A2, l))
#define NV_CADDDDADR(l)  NV_ENUM_APPLY(NV_ENUM_A5, NV_ENUM_APPLY(NV_ENUM_A2, l))

/*!
 * @brief Performs per-entry generation function, and either expands or extinguishes it
 *
 * @param dat__        Data List - Generation data table
 * @param name        Identifier - Name of enum entry
 * @param value  Integer Literal - Value for this entry
 *
 * @return the result of the generation function for this table,
 *         or nothing if this layer is being filtered (i.e. nested enum)
 */
#define NV_ENUM_ENTRY(dat__, name, value) \
    NV_ENUM_DAT_ENTRY(dat__) (NV_ENUM_DAT_GEN2(dat__) (dat__, name, value))

/*!
 * @brief Expands enum entries within nested enum specification using an updated
 *        data list specification
 *
 * @note the Current Enum Name is concatenated with parameter `name`
 *       the function table has its first entry popped
 *       other variables are unchanged.
 *
 * @param dat__        Data List - Generation data table
 * @param name             Token - String to append to previous enum name
 * @param res_lo Integer Literal - Min value of this nested enum
 * @param res_hi Integer Literal - Max value of this nested enum
 * @param entries         Macro  - Nested Enum Specification
 */
#define NV_ENUM_NEST_EXPAND0(dat__, name, res_lo, res_hi, entries) \
    entries(NV_ENUM_APPLY(NV_ENUM_DL_POP, NV_ENUM_NV_ENUM_REPLACE_1(NV_ENUM_NV_ENUM_REPLACE_1(NV_ENUM_CONCATENATE(NV_ENUM_DAT_CURR_NAME(dat__), name), NV_ENUM_APPLY(NV_ENUM_A1, dat__)), dat__)))
#define NV_ENUM_NEST_EXPAND1(dat__, name, res_lo, res_hi, entries) \
    entries(NV_ENUM_APPLY(NV_ENUM_DL_POP, NV_ENUM_NV_ENUM_REPLACE_1(NV_ENUM_NV_ENUM_REPLACE_1(NV_ENUM_CONCATENATE(NV_ENUM_DAT_CURR_NAME(dat__), name), NV_ENUM_APPLY(NV_ENUM_A1, dat__)), dat__)))
#define NV_ENUM_NEST_EXPAND2(dat__, name, res_lo, res_hi, entries) \
    entries(NV_ENUM_APPLY(NV_ENUM_DL_POP, NV_ENUM_NV_ENUM_REPLACE_1(NV_ENUM_NV_ENUM_REPLACE_1(NV_ENUM_CONCATENATE(NV_ENUM_DAT_CURR_NAME(dat__), name), NV_ENUM_APPLY(NV_ENUM_A1, dat__)), dat__)))
#define NV_ENUM_NEST_EXPAND3(dat__, name, res_lo, res_hi, entries) \
    entries(NV_ENUM_APPLY(NV_ENUM_DL_POP, NV_ENUM_NV_ENUM_REPLACE_1(NV_ENUM_NV_ENUM_REPLACE_1(NV_ENUM_CONCATENATE(NV_ENUM_DAT_CURR_NAME(dat__), name), NV_ENUM_APPLY(NV_ENUM_A1, dat__)), dat__)))

/*!
 * @brief Performs all enum generation for the given nested enum specification
 *
 * @note the Current Enum Name is concatenated with parameter `name`
 *       the Nested Enum Name is updated to match the Current Enum Name
 *       Res. Min is updated with parameter `res_lo`
 *       Res. Max is updated with parameter `res_hi`
 *       the function table has its first entry popped
 *       other variables are unchanged
 *
 * @param dat__        Data List - Generation data table
 * @param name             Token - String to append to previous enum name
 * @param res_lo Integer Literal - Min value of this nested enum
 * @param res_hi Integer Literal - Max value of this nested enum
 * @param entries         Macro  - Nested Enum Specification
 */
#define NV_ENUM_NEST_GEN(dat__, name, res_lo, res_hi, entries)               \
    NV_ENUM_DAT_GEN1(dat__)(                                                 \
        NV_ENUM_APPLY(                                                       \
            NV_ENUM_DL_POP,                                                  \
            NV_ENUM_NV_ENUM_REPLACE_1(                                       \
                NV_ENUM_REPLACE_4(                                           \
                    NV_ENUM_CONCATENATE(NV_ENUM_DAT_CURR_NAME(dat__), name), \
                    NV_ENUM_CONCATENATE(NV_ENUM_DAT_CURR_NAME(dat__), name), \
                    res_lo,                                                  \
                    res_hi,                                                  \
                    NV_ENUM_APPLY(                                           \
                        NV_ENUM_A1,                                          \
                        dat__                                                \
                    )                                                        \
                ),                                                           \
                dat__                                                        \
            )                                                                \
        ),                                                                   \
        entries                                                              \
    )

/*!
 * @note Definition: Data List
 *       This tool packs information used in each depth of processing within a
 *       nested list, which is passed to each invocation of NV_ENUM_ENTRY and
 *       NV_ENUM_NEST. The format of this object is as follows:
 *       (
 *           ( <Current Enum Name>, <0-Depth Nested Enum Name>, <Prefix>, <Root Enum Name>, <Res. Min>, <Res. Max> )
 *           ( <Entry Fn>, <Nest Fn>, <Per-Enum Gen Fn>, <Per-Entry Gen Fn> )
 *           ( <Entry Fn>, <Nest Fn>, <Per-Enum Gen Fn>, <Per-Entry Gen Fn> )
 *           ( <Entry Fn>, <Nest Fn>, <Per-Enum Gen Fn>, <Per-Entry Gen Fn> )
 *           ...
 *       )
 *
 *           Root Enum Name: Name of enum passed to NV_ENUM DEF (unaffected by NV_ENUM_NEST)
 * 0-Depth Nested Enum Name: Name of the enum at depth 0 (affected by NV_ENUM_NEST)
 *                   Prefix: Text prepended to each entry name (no spaces)
 *        Current Enum Name: Name of the enum at current depth
 *             Reserved Min: The minimum allowable enum value at this depth
 *             Reserved Max: The maximum allowable enum value at this depth
 *                 Entry Fn: macro called once per entry with the entry as an argument
 *                  Nest Fn: Duplicate definition of NV_ENUM_NEST_EXPAND to WAR recursion limits
 *          Per-Enum Gen Fn: Function to call once per NV_ENUM_DEF or NV_ENUM_NEST
 *         Per-Entry Gen Fn: Function to call once per NV_ENUM_ENTRY
 *
 */

//
// Data list accessor macros
//

/*! @brief Given data list, returns Current Enum Name */
#define NV_ENUM_DAT_CURR_NAME(dat__) NV_CAAR(dat__)
/*! @brief Given data list, returns 0-depth nested enum name */
#define NV_ENUM_DAT_NEST_NAME(dat__) NV_CADAR(dat__)
/*! @brief Given data list, returns Prefix */
#define NV_ENUM_DAT_PREFIX(dat__) NV_CADDAR(dat__)
/*! @brief Given data List, returns Root Enum Name */
#define NV_ENUM_DAT_ROOT_NAME(dat__) NV_CADDDAR(dat__)
/*! @brief Given data list, returns Res. Min at current depth */
#define NV_ENUM_DAT_MIN(dat__) NV_CADDDDAR(dat__)
/*! @brief Given data list, returns Res. Max at current depth */
#define NV_ENUM_DAT_MAX(dat__) NV_CADDDDDAR(dat__)
/*! @brief Given data list, returns Entry Fn at current depth */
#define NV_ENUM_DAT_ENTRY(dat__) NV_CAADR(dat__)
/*! @brief Given data list, returns Nest Fn at current depth */
#define NV_ENUM_NEST(dat__) NV_CADADR(dat__)
/*! @brief Given data list, returns Per-Enum Gen Fn at current depth */
#define NV_ENUM_DAT_GEN1(dat__) NV_CADDADR(dat__)
/*! @brief Given data list, returns Per-Entry Gen Fn at current depth */
#define NV_ENUM_DAT_GEN2(dat__) NV_CADDDADR(dat__)

/*!
 * @brief constructs a data list to be used for generation of the root enum
 */
#define NV_ENUM_DEPTH_0(name, prefix, gen1_fn, gen2_fn)                       \
    ( (name, name, prefix, name, NV_ENUM_MIN, NV_ENUM_MAX)                    \
    , (NV_EXPAND_ONE, NV_ENUM_NEST_EXPAND0, gen1_fn, gen2_fn, unused, unused) \
    , (NV_EXPAND_ONE, NV_ENUM_NEST_EXPAND1, gen1_fn, gen2_fn, unused, unused) \
    , (NV_EXPAND_ONE, NV_ENUM_NEST_EXPAND2, gen1_fn, gen2_fn, unused, unused) \
    , (NV_EXPAND_ONE, NV_ENUM_NEST_EXPAND3, gen1_fn, gen2_fn, unused, unused) \
    , (unused, unused, unused, unused, unused, unused)                        \
    )

/*!
 * @brief constructs a data list to be used for generation of enums at depth 1
 */
#define NV_ENUM_DEPTH_1(name, prefix, gen1_fn, gen2_fn)                       \
    ( (name, name, prefix, name, NV_ENUM_MIN, NV_ENUM_MAX)                    \
    , (NV_DISCARD_ONE, NV_ENUM_NEST_GEN, gen1_fn, gen2_fn, unused, unused)    \
    , (NV_EXPAND_ONE, NV_ENUM_NEST_EXPAND0, gen1_fn, gen2_fn, unused, unused) \
    , (NV_EXPAND_ONE, NV_ENUM_NEST_EXPAND1, gen1_fn, gen2_fn, unused, unused) \
    , (NV_EXPAND_ONE, NV_ENUM_NEST_EXPAND2, gen1_fn, gen2_fn, unused, unused) \
    , (unused, unused, unused, unused, unused, unused)                        \
    )

/*!
 * @brief constructs a data list to be used for generation of enums at depth 2
 */
#define NV_ENUM_DEPTH_2(name, prefix, gen1_fn, gen2_fn)                        \
    ( (name, name, prefix, name, NV_ENUM_MIN, NV_ENUM_MAX)                     \
    , (NV_DISCARD_ONE, NV_ENUM_NEST_EXPAND0, gen1_fn, gen2_fn, unused, unused) \
    , (NV_DISCARD_ONE, NV_ENUM_NEST_GEN, gen1_fn, gen2_fn, unused, unused)     \
    , (NV_EXPAND_ONE, NV_ENUM_NEST_EXPAND1, gen1_fn, gen2_fn, unused, unused)  \
    , (NV_EXPAND_ONE, NV_ENUM_NEST_EXPAND2, gen1_fn, gen2_fn, unused, unused)  \
    , (unused, unused, unused, unused, unused, unused)                         \
    )

///
/// Generator Functions
///


/*! @brief Generates an enum type given the enum specification in entries */
#define NV_ENUM_GEN_MAIN(dat__, entries) \
    enum NV_ENUM_DAT_CURR_NAME(dat__) { entries(dat__) };

/*! @brief Generates a single enum entry with the given name and value */
#define NV_ENUM_GEN_MAIN_FN(dat__, entry_name, value) \
    NV_ENUM_CONCATENATE(NV_ENUM_DAT_PREFIX(dat__), entry_name) = value,


/*! @brief Generates an enum typedef for the given enum. All nested types receive the same typedef (i.e. the root enum */
#define NV_ENUM_GEN_TYPEDEF(dat__, entries) \
    typedef enum NV_ENUM_DAT_ROOT_NAME(dat__) NV_ENUM_DAT_CURR_NAME(dat__);

/*! @brief Does nothing. There is no per-entry generation for typedefs. */
#define NV_ENUM_GEN_TYPEDEF_FN(dat__, entry_name, value)


/*! @brief Generates an enum with an added entry at the end to provide the enum size*/
#define NV_ENUM_GEN_SIZE(dat__, entries) \
    enum { entries(dat__) NV_ENUM_CONCATENATE(NV_ENUM_DAT_CURR_NAME(dat__), __SIZE) };

/*! @brief Generates a single enum entry with __UNUSED appended. These values are not meant to be used. */
#define NV_ENUM_GEN_SIZE_FN(dat__, entry_name, value) \
    NV_ENUM_CONCATENATE(NV_ENUM_CONCATENATE(NV_ENUM_DAT_NEST_NAME(dat__), entry_name), __UNUSED) = value,


/*! @brief Generates a conversion function from an enum value to string representation. */
#define NV_ENUM_GEN_STRING(dat__, entries)                                                      \
    static inline const char *                                                                  \
    NV_ENUM_CONCATENATE(NV_ENUM_DAT_CURR_NAME(dat__), _TO_STRING)                               \
    (                                                                                           \
        enum NV_ENUM_DAT_ROOT_NAME(dat__) in                                                    \
    )                                                                                           \
    {                                                                                           \
        switch (in)                                                                             \
        {                                                                                       \
            entries(dat__)                                                                      \
            default:                                                                            \
                break;                                                                          \
        }                                                                                       \
        return NV_ENUM_STRINGIFY(NV_ENUM_CONCATENATE(NV_ENUM_DAT_CURR_NAME(dat__), __UNKNOWN)); \
    }

/*! @brief Generates a case for the given enum entry, and its string representation. */
#define NV_ENUM_GEN_STRING_FN(dat__, entry_name, value)              \
    case NV_ENUM_CONCATENATE(NV_ENUM_DAT_PREFIX(dat__), entry_name): \
        return NV_ENUM_STRINGIFY(NV_ENUM_CONCATENATE(NV_ENUM_DAT_PREFIX(dat__), entry_name));


/*! @brief Generates a conversion function from NvU32 to enum value. */
#define NV_ENUM_GEN_FROM(dat__, entries)                        \
    static inline NV_STATUS                                     \
    NV_ENUM_CONCATENATE(NV_ENUM_DAT_CURR_NAME(dat__), _FROM32)  \
    (                                                           \
        NvU32 in,                                               \
        enum NV_ENUM_DAT_ROOT_NAME(dat__) *out                  \
    )                                                           \
    {                                                           \
        switch (in)                                             \
        {                                                       \
            entries(dat__)                                      \
            if (out != NULL)                                    \
                *out = ((enum NV_ENUM_DAT_ROOT_NAME(dat__))in); \
            return NV_OK;                                       \
            default:                                            \
                break;                                          \
        }                                                       \
        return NV_ERR_OUT_OF_RANGE;                             \
    }

/*! @brief Generates a case for the given enum entry. */
#define NV_ENUM_GEN_FROM_FN(dat__, entry_name, value) \
    case NV_ENUM_CONCATENATE(NV_ENUM_DAT_PREFIX(dat__), entry_name):


/*! @brief Generates a struct constant containing the smallest value contained within the enum (plus one). */
#define NV_ENUM_GEN_LO(dat__, entries) \
    typedef struct { char lo[(1 * entries(dat__) 0 + 1)]; } NV_ENUM_CONCATENATE(NV_ENUM_DAT_CURR_NAME(dat__), __LO) ;

/*! @brief Builds a portion of the expression calculating the smallest enum value. */
#define NV_ENUM_GEN_LO_FN(dat__, entry_name, value) \
    (value)) + (0 *


/*! @brief Generates a struct constant containing the number of values contained within the enum. */
#define NV_ENUM_GEN_COUNT(dat__, entries) \
    typedef struct { char count[(0 + entries(dat__) 0)]; } NV_ENUM_CONCATENATE(NV_ENUM_DAT_CURR_NAME(dat__), __COUNT) ;

/*! @brief Builds a portion of the expression calculating the number of enum values. */
#define NV_ENUM_GEN_COUNT_FN(dat__, entry_name, value) \
    1 +


/*! @brief Generates a group of struct constants containing the above generated values. */
#define NV_ENUM_GEN_META(dat__, entries)                                                                                                                                                                                                                                                \
    typedef struct  { char          lo[sizeof(((NV_ENUM_CONCATENATE(NV_ENUM_DAT_CURR_NAME(dat__), __LO) *) NULL)->lo)];                                                                                                                                                                 \
                      char          hi[NV_ENUM_CONCATENATE(NV_ENUM_DAT_CURR_NAME(dat__), __SIZE)];                                                                                                                                                                                      \
                      char       count[sizeof(((NV_ENUM_CONCATENATE(NV_ENUM_DAT_CURR_NAME(dat__), __COUNT) *) NULL)->count)];                                                                                                                                                           \
                      char        size[NV_ENUM_CONCATENATE(NV_ENUM_DAT_CURR_NAME(dat__), __SIZE)];                                                                                                                                                                                      \
                      char bContiguous[(sizeof(((NV_ENUM_CONCATENATE(NV_ENUM_DAT_CURR_NAME(dat__), __COUNT) *) NULL)->count) == (NV_ENUM_CONCATENATE(NV_ENUM_DAT_CURR_NAME(dat__), __SIZE) - sizeof(((NV_ENUM_CONCATENATE(NV_ENUM_DAT_CURR_NAME(dat__), __LO) *) NULL)->lo) + 1)) + 1]; \
                    } NV_ENUM_CONCATENATE(NV_ENUM_DAT_CURR_NAME(dat__), __META) ;

/*! @brief unused. Nothing needs to be generated per-entry for this generator. */
#define NV_ENUM_GEN_META_FN(dat__, entry_name, value)

/*! @brief Generates a compile-time assertion. */
#define NV_ENUM_GEN_ASSERT_MONOTONIC(dat__, entries) \
    typedef char NV_ENUM_CONCATENATE(NV_ENUM_DAT_CURR_NAME(dat__), _assert_monotonic)[ (2 * ((0 > entries(dat__) ((NvU32)-1)) == 0)) - 1 ];

/*!
 * @brief Builds a portion of the expression asserting that all enum values
 * must be declared in strictly monotonically increasing order.
 */
#define NV_ENUM_GEN_ASSERT_MONOTONIC_FN(dat__, entry_name, value) \
    value) + (value >


/*! @brief Generates a compile-time assertion. */
#define NV_ENUM_GEN_ASSERT_IN_RANGE(dat__, entries) \
    typedef char NV_ENUM_CONCATENATE(NV_ENUM_DAT_CURR_NAME(dat__), _assert_in_range)[ (2 * ((1 * entries(dat__) 1) == 1)) - 1 ];

/*!
 * @brief Builds a portion of the expression asserting that all nested enum
 * values must be within the reserved range of their parent enum.
 */
#define NV_ENUM_GEN_ASSERT_IN_RANGE_FN(dat__, entry_name, value) \
    (((NvS64)value) >= NV_ENUM_DAT_MIN(dat__)) * (((NvS64)value) <= NV_ENUM_DAT_MAX(dat__)) *


///
/// End of Generator Functions
///


/*!
 * @brief Performs code generation for the given generator function pair
 *
 * @note This function must be updated if supporting deeper nesting in the future
 *
 * @param fn1        Macro - Per-Enum Gen Fn
 * @param fn2        Macro - Per-Entry Gen Fn
 * @param enum_name  Token - Root Enum Name
 * @param prefix     Token - Prefix
 * @param entries    Macro - Enum Specification
 */
#define NV_ENUM_GENERATOR(fn1, fn2, enum_name, prefix, entries) \
    fn1(NV_ENUM_DEPTH_0(enum_name, prefix, fn1, fn2), entries)  \
    entries(NV_ENUM_DEPTH_1(enum_name, prefix, fn1, fn2))       \
    entries(NV_ENUM_DEPTH_2(enum_name, prefix, fn1, fn2))       \

//
// Windows preprocessor crashes with "ran out of heap space" errors if the
// preproccessed output from a single macro gets too large, so skip the
// verification sanity asserts when running on windows to increase the size of
// representable enums
//
#if NVOS_IS_WINDOWS

/*!
 * @brief Generates an enum and associated metadata with the given enum name and prefix
 *
 * @param prefix     Token - Prefix
 * @param enum_name  Token - Root Enum Name
 * @param entries    Macro - Enum Specification
 */
#define NV_ENUM_DEF_PREFIX(prefix, enum_name, entries)                                                   \
    NV_ENUM_GEN_MAIN(NV_ENUM_DEPTH_0(enum_name, prefix, NV_ENUM_GEN_MAIN, NV_ENUM_GEN_MAIN_FN), entries) \
    NV_ENUM_GENERATOR(NV_ENUM_GEN_TYPEDEF, NV_ENUM_GEN_TYPEDEF_FN, enum_name, prefix, entries)           \
    NV_ENUM_GENERATOR(NV_ENUM_GEN_SIZE, NV_ENUM_GEN_SIZE_FN, enum_name, prefix, entries)                 \
    NV_ENUM_GENERATOR(NV_ENUM_GEN_STRING, NV_ENUM_GEN_STRING_FN, enum_name, prefix, entries)             \
    NV_ENUM_GENERATOR(NV_ENUM_GEN_FROM, NV_ENUM_GEN_FROM_FN, enum_name, prefix, entries)                 \
    NV_ENUM_GENERATOR(NV_ENUM_GEN_LO, NV_ENUM_GEN_LO_FN, enum_name, prefix, entries)                     \
    NV_ENUM_GENERATOR(NV_ENUM_GEN_COUNT, NV_ENUM_GEN_COUNT_FN, enum_name, prefix, entries)               \
    NV_ENUM_GENERATOR(NV_ENUM_GEN_META, NV_ENUM_GEN_META_FN, enum_name, prefix, entries)                 \

#else

/*!
 * @brief Generates an enum and associated metadata with the given enum name and prefix
 *
 * @param prefix     Token - Prefix
 * @param enum_name  Token - Root Enum Name
 * @param entries    Macro - Enum Specification
 */
#define NV_ENUM_DEF_PREFIX(prefix, enum_name, entries)                                                           \
    NV_ENUM_GEN_MAIN(NV_ENUM_DEPTH_0(enum_name, prefix, NV_ENUM_GEN_MAIN, NV_ENUM_GEN_MAIN_FN), entries)         \
    NV_ENUM_GENERATOR(NV_ENUM_GEN_TYPEDEF, NV_ENUM_GEN_TYPEDEF_FN, enum_name, prefix, entries)                   \
    NV_ENUM_GENERATOR(NV_ENUM_GEN_SIZE, NV_ENUM_GEN_SIZE_FN, enum_name, prefix, entries)                         \
    NV_ENUM_GENERATOR(NV_ENUM_GEN_STRING, NV_ENUM_GEN_STRING_FN, enum_name, prefix, entries)                     \
    NV_ENUM_GENERATOR(NV_ENUM_GEN_FROM, NV_ENUM_GEN_FROM_FN, enum_name, prefix, entries)                         \
    NV_ENUM_GENERATOR(NV_ENUM_GEN_LO, NV_ENUM_GEN_LO_FN, enum_name, prefix, entries)                             \
    NV_ENUM_GENERATOR(NV_ENUM_GEN_COUNT, NV_ENUM_GEN_COUNT_FN, enum_name, prefix, entries)                       \
    NV_ENUM_GENERATOR(NV_ENUM_GEN_META, NV_ENUM_GEN_META_FN, enum_name, prefix, entries)                         \
    NV_ENUM_GENERATOR(NV_ENUM_GEN_ASSERT_MONOTONIC, NV_ENUM_GEN_ASSERT_MONOTONIC_FN, enum_name, prefix, entries) \
    NV_ENUM_GENERATOR(NV_ENUM_GEN_ASSERT_IN_RANGE, NV_ENUM_GEN_ASSERT_IN_RANGE_FN, enum_name, prefix, entries)



#endif // NVOS_IS_WINDOWS

#define NV_ENUM_NOTHING

/*!
 * @brief Generates an enum and associated metadata with the given enum name
 *
 * @param prefix     Token - Prefix
 * @param enum_name  Token - Root Enum Name
 * @param entries    Macro - Enum Specification
 */
#define NV_ENUM_DEF(enum_name, entries) \
    NV_ENUM_DEF_PREFIX(NV_ENUM_NOTHING, enum_name, entries)

/*!
 * @brief Generates an exported enum with the given prefix
 *
 * @param prefix     Token - Prefix
 * @param entries    Macro - Enum Specification
 */
#define NV_ENUM_EXPORT_PREFIX(prefix, entries) \
    NV_ENUM_GEN_MAIN(NV_ENUM_DEPTH_0(NV_ENUM_NOTHING, prefix, NV_ENUM_GEN_MAIN, NV_ENUM_GEN_MAIN_FN), entries)

/*!
 * @brief Generates an exported enum
 *
 * @param entries    Macro - Enum Specification
 */
#define NV_ENUM_EXPORT(entries) \
    NV_ENUM_EXPORT_PREFIX( , entries)


///
/// Runtime Utility Functions
///

/*!
 * @brief Converrts an unsigned integer into an enum value, or returns error.
 *
 * @param[in]  type  identifier - Enum type name
 * @param[in]  value     rvalue - integer value belonging to given enum
 * @param[out] pResult  pointer - Optional pointer to enum, updated with value on success
 *
 * @return NV_OK if the value belongs to the enum
 *         NV_ERR_OUT_OF_RANGE otherwise
 */
#define NV_ENUM_FROM32(type, value, pResult) \
    (NV_ENUM_CONCATENATE(type, _FROM32)((value), (pResult)))

/*!
 * @brief Returns a string representation of the name of the given enum value
 *
 * @param[in] type  identifier - Enum type name
 * @param[in] value     rvalue - integer value belonging to given enum
 *
 * @return a string representing the given value
 */
#define NV_ENUM_TO_STRING(type, value) \
    (NV_ENUM_CONCATENATE(type, _TO_STRING)(value))

/*!
 * @brief Queries whether the given value belongs to the given enum
 *
 * @param[in] type  identifier - Enum type name
 * @param[in] value     rvalue - integer to check
 *
 * @return NV_TRUE if the given value is valid
 *         NV_FALSE otherwise
 */
#define NV_ENUM_IS(type, value) \
    (NV_OK == NV_ENUM_FROM32(type, (value), NULL))

/*!
 * @brief Queries the value of the smallest enum entry
 *
 * @param[in] type  identifier - Enum type name
 */
#define NV_ENUM_LO(type) \
    ((type)(sizeof(((NV_ENUM_CONCATENATE(type, __META) *) NULL)->lo) - 1))

/*!
 * @brief Queries the value of the largest enum entry
 *
 * @param[in] type  identifier - Enum type name
 */
#define NV_ENUM_HI(type) \
    ((type)(sizeof(((NV_ENUM_CONCATENATE(type, __META) *) NULL)->hi) - 1))

/*!
 * @brief Queries the number of values between the first and last enum entries
 * @note  This value is large enough to use in an array declaration with enum
 *        entries used as indices into the array.
 *
 * @param[in] type  identifier - Enum type name
 */
#define NV_ENUM_SIZE(type) \
    (sizeof(((NV_ENUM_CONCATENATE(type, __META) *) NULL)->size))

/*!
 * @brief Queries the number of values defined by the enum
 *
 * @param[in] type  identifier - Enum type name
 */
#define NV_ENUM_COUNT(type) \
    (sizeof(((NV_ENUM_CONCATENATE(type, __META) *) NULL)->count))

/*!
 * @brief Queries whether or not the enum is defined contiguously (i.e. no holes)
 *
 * @param[in] type  identifier - Enum type name
 *
 * @return NV_TRUE if each value between the lo and hi enum values are valid enum values
 */
#define NV_ENUM_IS_CONTIGUOUS(type) \
    ((NvBool)(sizeof(((NV_ENUM_CONCATENATE(type, __META) *) NULL)->bContiguous) - 1))

/*!
 * @brief Macros providing iteration over each value defined by the enum type
 * @note  Iteration is faster over contiguous enums
 *
 * @param[in] type   identifier - Enum type name
 * @param[in] value      lvalue - iterator holding current enum value
 */
#define FOR_EACH_IN_ENUM(type, value)                                                                            \
{                                                                                                                \
    NvU32 localValue;                                                                                            \
    for (localValue = value = NV_ENUM_LO(type); localValue <= NV_ENUM_HI(type); (value) = (type) (++localValue)) \
    {                                                                                                            \
        if (!NV_ENUM_IS_CONTIGUOUS(type) && !NV_ENUM_IS(type, localValue))                                       \
            continue;

#define FOR_EACH_IN_ENUM_END \
   }                         \
}

/*!
 * @brief Given the Nth enum value defined by the enum type, returns N
 * @note  Only supports contiguous enums
 *
 * @param[in] type   identifier - Enum type name
 * @param[in] value      rvalue - integer value belonging to enum type
 *
 * @return the index at which the enum value was defined within the enum, or -1
 */
#define NV_ENUM_TO_IDX(type, value) \
    ((NV_ENUM_IS_CONTIGUOUS(type) && NV_ENUM_IS(type, value)) ? ((value) - NV_ENUM_LO(type)) : ((NvU32)-1))

/*!
 * @brief Returns the Nth enum value defined by the given type
 * @note  Only supports contiguous enums
 *
 * @param[in] type   identifier - Enum type name
 * @param[in] value      rvalue - integer specifying entry index
 *
 * @return The Nth enum value defined within the enum, or NV_ENUM_SIZE(type) if non-existent
 */
#define NV_ENUM_FROM_IDX(type, idx) \
    ((type)((NV_ENUM_IS_CONTIGUOUS(type) && idx < NV_ENUM_COUNT(type)) ? (NV_ENUM_LO(type) + (idx)) : NV_ENUM_SIZE(type)))

///
/// End of Runtime Utility Functions
///

///@}
///  NV_UTILS_ENUM

#endif // NV_ENUM_H_
