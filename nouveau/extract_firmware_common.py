#!/usr/bin/env python3

# SPDX-FileCopyrightText: Copyright (c) 2023-2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
# SPDX-License-Identifier: MIT
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
# DEALINGS IN THE SOFTWARE.

# Shared helpers for the extract-firmware-nova.py and extract-firmware-nouveau.py
# scripts.  This module holds the code that is identical between the two scripts:
# parsing OpenRM binhex arrays/structs, GPU support detection, and symlink
# creation.

import os
import re
import gzip
import struct

class MyException(Exception):
    pass

def round_up_to_base(x, base = 10):
    return x + (base - x) % base

# -------------------------------------------------------------------
# Parse binhex arrays from OpenRM
# -------------------------------------------------------------------

def parse_array(f):
    """Parses a bindata array definition and returns its binhex as bytes

    Example:
    static BINDATA_CONST NvU8 ksec2BinArchiveSecurescrubUcode_AD10X_header_prod_data[] =
    {
        0x63, 0x60, 0x00, 0x02, 0x46, 0x20, 0x96, 0x02, 0x62, 0x66, 0x08, 0x13, 0x4c, 0x48, 0x42, 0x69,
        0x20, 0x00, 0x00, 0x30, 0x39, 0x0a, 0xfc, 0x24, 0x00, 0x00, 0x00,
    };

    Or an array of u64:
    static BINDATA_CONST NV_DECLARE_ALIGNED(NvU64, 8) kgspBinArchiveGspRmBoot_GA100_BINDATA_LABEL_UCODE_IMAGE_data[] =
    {
        0xfd1441144c3153edULL, 0x65de0b84b3bb7333ULL,
        0x24b04d6682608b21ULL, 0x2c42c5c50916e3b9ULL,

    """
    output = b''
    for line in f:
        if "};" in line:
            break
        # Try 64-bit ULL values first, then fall back to byte values
        ulls = [int(val, 16) for val in re.findall(r'0x([0-9a-f]+)ULL', line, re.IGNORECASE)]
        if len(ulls) > 0:
            output += struct.pack(f"<{len(ulls)}Q", *ulls)
        else:
            bytes = [int(b, 16) for b in re.findall(r'0x[0-9a-f][0-9a-f]', line, re.IGNORECASE)]
            if len(bytes) > 0:
                output += struct.pack(f"{len(bytes)}B", *bytes)

    return output

def parse_struct(f):
    """Parses a struct definition and returns its binhex as bytes

    Example:
    static const RM_FLCN_BL_DESC ksec2BinArchiveBlUcode_TU102_BINDATA_LABEL_UCODE_DESC_data = {
        0xfd,
        0,
        {
            0x0,
            0x200,
            0x200,
            0x100
        }
    };

    """
    output = b''
    for line in f:
        if "};" in line:
            break
        words = [int(b, 16) for b in re.findall('(?:0x|)[0-9a-f]+', line)]
        if len(words) > 0:
            output += struct.pack(f"<{len(words)}I", *words)


    return output

def get_bytes(filename, array1, array2):
    """Extract the bytes for the given array or struct in the given file.

    :param filename: the file to parse
    :param array1: the first half of name of the array/struct to parse
    :param array2: the second half
    :returns: byte array

    This function scans the file for the array or struct and returns a bytearray
    of its contents, uncompressing the data if it is tagged as compressed.

    This function assumes that each array/struct is immediately preceded with a
    comment section that specifies whether the array is compressed and how many
    bytes of data there should be.  Example:

    //
    // FUNCTION: ksec2GetBinArchiveSecurescrubUcode_AD10X("header_prod")
    // FILE NAME: kernel/inc/securescrub/bin/ad10x/g_securescrubuc_sec2_ad10x_boot_from_hs_prod.h
    // FILE TYPE: TEXT
    // VAR NAME: securescrub_ucode_header_ad10x_boot_from_hs
    // COMPRESSION: YES
    // COMPLEX_STRUCT: NO
    // DATA SIZE (bytes): 36
    // COMPRESSED SIZE (bytes): 27
    //
    static BINDATA_CONST NvU8 ksec2BinArchiveSecurescrubUcode_AD10X_header_prod_data[] =

    The actual extraction of binhex bytes is handled by parse_array() or parse_struct().
    """

    # A list of all expected array/struct names, and the size of the hex numbers.
    # BINDATA_LABEL was added in r575, NV_DECLARE_ALIGNED(NvU8, 8) was added in r590,
    # and NV_DECLARE_ALIGNED(NvU64, 8) was added in r615.
    arrays = [
        (f"static BINDATA_CONST NvU8 {array1}_{array2}_data", 1),
        (f"static BINDATA_CONST NvU8 {array1}_BINDATA_LABEL_{array2.upper()}_data", 1),
        (f"static BINDATA_CONST NV_DECLARE_ALIGNED(NvU8, 8) {array1}_BINDATA_LABEL_{array2.upper()}_data", 1),
        (f"static BINDATA_CONST NV_DECLARE_ALIGNED(NvU64, 8) {array1}_BINDATA_LABEL_{array2.upper()}_data", 8),
        (f"static const {array1}_{array2}_data", 4),
        (f"static const {array1}_BINDATA_LABEL_{array2.upper()}_data", 4),
    ]

    with open(filename) as f:
        for line in f:
            m = re.search(r"COMPRESSION: (\w*)", line)
            if m:
                is_compressed = m.group(1) == "YES"
            m = re.search(r"COMPLEX_STRUCT: (\w*)", line)
            if m:
                is_struct = m.group(1) == "YES"
            m = re.search(r"DATA SIZE \(bytes\): (\d+)", line)
            if m:
                data_size = int(m.group(1))
            m = re.search(r"DATA SIZE \(bytes\): sizeof\((\d+)\)", line)
            if m:
                data_size = None
            m = re.search(r"COMPRESSED SIZE \(bytes\): N/A", line)
            if m:
                compressed_size = None
            m = re.search(r"COMPRESSED SIZE \(bytes\): (\d+)", line)
            if m:
                compressed_size = int(m.group(1))
            m = next((a for a in arrays if a[0] in line), None)
            if m:
                # We found the array, so remember its name in case we need to report an error
                array = m[0]
                word_size = m[1]
                break
        else:
            raise MyException(f"array {array1}_{array2}_data not found in {filename}")

        if is_struct:
            output = parse_struct(f)
            # Struct entries reference themselves for the size.  The only way
            # to determine the actual size is to compile the C code.  Instead,
            # just assume the struct definition is complete.
            data_size = len(output)
        else:
            output = parse_array(f)

    if len(output) == 0:
        raise MyException(f"no data found for {array} in {filename}")

    # Structs are never compressed
    if is_struct and is_compressed:
        raise MyException(f"struct {array} in {filename} cannot be compressed")

    # Make sure we actually read a compressed size
    if is_compressed and not compressed_size:
        raise MyException(f"array {array} in {filename} compressed size is undetermined")

    # Just as a sanity check, make sure the compression was actually worth it
    if is_compressed and compressed_size > data_size:
        raise MyException(f"array {array} in {filename} compressed size is larger than uncompressed")

    # If the data is encoded as 8-byte integers, then the COMPRESSED_SIZE value
    # is actually the size of the compressed data before it was encoded into
    # 8-byte integers.  So we need to round up the advertised size in order for it
    # to match the actual size.

    if is_compressed:
        expected_size = round_up_to_base(compressed_size, word_size)
        if len(output) != expected_size:
            raise MyException(f"compressed array {array} in {filename} should be {expected_size} bytes but is actually {len(output)}.")
        gzipheader = struct.pack("<4BL2B", 0x1f, 0x8b, 8, 0, 0, 0, 3)
        output = gzip.decompress(gzipheader + output)
        if len(output) != data_size:
            raise MyException(f"array {array} in {filename} decompressed to {len(output)} bytes but should have been {data_size} bytes.")

        return output
    else:
        expected_size = round_up_to_base(data_size, word_size)
        if len(output) != expected_size:
            raise MyException(f"array {array} in {filename} should be {expected_size} bytes but is actually {len(output)}.")
        return output[:data_size]

# -------------------------------------------------------------------
# Generate firmware binaries
# -------------------------------------------------------------------

# Newer Blackwell (and later) GPUs are not supported on older versions of OpenRM.
# So check to see if the given GPU has the binhex source files needed.
def is_supported(gpu):
    GPU = gpu.upper()

    gsp_bootloader = f"src/nvidia/generated/g_bindata_kgspGetBinArchiveGspRmBoot_{GPU}.c"
    if not os.path.isfile(gsp_bootloader):
        return False

    fmc = f"src/nvidia/generated/g_bindata_kgspGetBinArchiveGspRmFmcGfwProdSigned_{GPU}.c"
    if not os.path.isfile(fmc):
        return False

    return True

# Create a symlink, deleting the existing file/link if necessary
def symlink(dest, source, target_is_directory = False):
    import errno

    try:
        os.symlink(dest, source, target_is_directory = target_is_directory)
    except OSError as e:
        if e.errno == errno.EEXIST:
            os.remove(source)
            os.symlink(dest, source, target_is_directory = target_is_directory)
        else:
            raise e

# Verify the .run file and extract its contents to the given temp directory
def extract_run_file(runfile, tempdir):
    import subprocess

    basename = os.path.basename(runfile)

    print(f"Validating {basename}")
    try:
        result = subprocess.run(['/bin/sh', runfile, '--check'], shell=False,
                                check=True, timeout=10,
                                stdout = subprocess.PIPE, stderr = subprocess.STDOUT)
        output = result.stdout.strip().decode("ascii")
        if not "check sums and md5 sums are ok" in output:
            raise MyException(f"{basename} is not a valid Nvidia driver .run file")
    except subprocess.CalledProcessError as error:
        print(error.output.decode())
        raise

    try:
        # The .run file extracts its contents to a directory with the same
        # name as the file itself, minus the .run.  The GSP-RM firmware
        # images are in the 'firmware' subdirectory.
        result = subprocess.run(['/bin/sh', runfile, '--target-directory'], shell=False,
                                check=True, timeout=10, cwd=tempdir ,
                                stdout = subprocess.PIPE, stderr = subprocess.DEVNULL)
        target = result.stdout.strip().decode("ascii")
        directory = f"{tempdir}/{target}/firmware"
    except subprocess.SubprocessError as e:
        print(e.output.decode())
        raise

    try:
        print(f"Extracting {basename} to {tempdir}")
        # The -x parameter tells the installer to only extract the
        # contents and then exit.
        subprocess.run(['/bin/sh', runfile, '-x'], shell=False,
                       check=True, timeout=60, cwd=tempdir,
                       stdout = subprocess.PIPE, stderr = subprocess.STDOUT)
    except subprocess.SubprocessError as error:
        print(error.output.decode())
        raise

    # As a final verification, make sure the gsp.bin files are there
    tu10x_src = f"{directory}/gsp_tu10x.bin"
    ga10x_src = f"{directory}/gsp_ga10x.bin"

    if not os.path.exists(tu10x_src) or not os.path.exists(ga10x_src):
        raise MyException(f"Firmware files are missing in {basename}")

    return directory
