#!/bin/sh
#  SPDX-FileCopyrightText: Copyright (c) 2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
#  SPDX-License-Identifier: MIT
#
#  Permission is hereby granted, free of charge, to any person obtaining a
#  copy of this software and associated documentation files (the "Software"),
#  to deal in the Software without restriction, including without limitation
#  the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
#
#  The above copyright notice and this permission notice shall be included in
#  all copies or substantial portions of the Software.
#
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
#  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
#  DEALINGS IN THE SOFTWARE.
#

set -e

get_compiler_type()
{
    printf "#if defined(__clang__)
            clang
            #elif defined(__GNUC__)
            gcc
            #elif defined(__INTEL_COMPILER)
            icc
            #else
            unknown
            #endif" | $1 -E -P -
}

get_original_version()
{
    printf "#if defined(__clang__)
            __clang_major__ __clang_minor__ __clang_patchlevel__
            #elif defined(__GNUC__)
            __GNUC__ __GNUC_MINOR__ __GNUC_PATCHLEVEL__
            #elif defined(__INTEL_COMPILER)
            __INTEL_COMPILER __INTEL_COMPILER_UPDATE
            #endif" | $1 -E -P -
}

get_canonical_version()
{
    type=$(get_compiler_type $1)
    set -- $(get_original_version $1)

    # get the version based on the type
    if [ "$type" = "unknown" ]; then
       echo >&2 "unknown compiler: bailing out"
       exit 1
    elif [ "$type" = "icc" ]; then
       echo >&2 "icc is not supported"
       exit 1
    else
      major=$1
      minor=$2
    fi
    patch=$3
    echo $(($3 + $2 * 100 + $1 * 10000))
}

if [ "$1" = "type" ]; then
    echo $(get_compiler_type $2)
elif [ "$1" = "version_is_at_least" ]; then
    if [ -z "$3" ]; then
      echo >&2 "minimum compiler version cannot be empty"
      exit 1
    fi
    version=$(get_canonical_version $2)
    if [ "$version" -gt $(($3-1)) ]; then
      echo "1"
    fi
fi

