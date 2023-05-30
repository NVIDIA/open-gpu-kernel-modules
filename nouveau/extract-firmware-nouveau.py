#!/usr/bin/env python3

# SPDX-FileCopyrightText: Copyright (c) 2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

# Converts OpenRM binhex-encoded images to Nouveau-compatible binary blobs
# See nouveau_firmware_layout.ods for documentation on the file format

import sys
import os
import argparse
import shutil
import re
import gzip
import struct

class MyException(Exception):
    pass

def round_up_to_base(x, base = 10):
    return x + (base - x) % base

def getbytes(filename, array):
    """Extract the bytes for the given array in the given file.

    :param filename: the file to parse
    :param array: the name of the array to parse
    :returns: byte array

    This function scans the file for the array and returns a bytearray of
    its contents, uncompressing the data if it is tagged as compressed.

    This function assumes that each array is immediately preceded with a comment
    section that specifies whether the array is compressed and how many bytes of
    data there should be.  Example:

    #if defined(BINDATA_INCLUDE_DATA)
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
    {
        0x63, 0x60, 0x00, 0x02, 0x46, 0x20, 0x96, 0x02, 0x62, 0x66, 0x08, 0x13, 0x4c, 0x48, 0x42, 0x69,
        0x20, 0x00, 0x00, 0x30, 0x39, 0x0a, 0xfc, 0x24, 0x00, 0x00, 0x00,
    };
    #endif // defined(BINDATA_INCLUDE_DATA)
    """

    with open(filename) as f:
        for line in f:
            if "COMPRESSION: NO" in line:
                compressed = False
            if "COMPRESSION: YES" in line:
                compressed = True
            m = re.search("DATA SIZE \(bytes\): (\d+)", line)
            if m:
                data_size = int(m.group(1))
            m = re.search("COMPRESSED SIZE \(bytes\): (\d+)", line)
            if m:
                compressed_size = int(m.group(1))
            if "static BINDATA_CONST NvU8 " + array in line:
                break
        else:
            raise MyException(f"array {array} not found in {filename}")

        output = b''
        for line in f:
            if "};" in line:
                break
            bytes = [int(b, 16) for b in re.findall('0x[0-9a-f][0-9a-f]', line)]
            if len(bytes) > 0:
                output += struct.pack(f"{len(bytes)}B", *bytes)

    if len(output) == 0:
        raise MyException(f"no data found for {array}")

    if compressed:
        if len(output) != compressed_size:
            raise MyException(f"compressed array {array} in {filename} should be {compressed_size} bytes but is actually {len(output)}.")
        gzipheader = struct.pack("<4BL2B", 0x1f, 0x8b, 8, 0, 0, 0, 3)
        output = gzip.decompress(gzipheader + output)
        if len(output) != data_size:
            raise MyException(f"array {array} in {filename} decompressed to {len(output)} bytes but should have been {data_size} bytes.")
        return output
    else:
        if len(output) != data_size:
            raise MyException(f"array {array} in {filename} should be {compressed_size} bytes but is actually {len(output)}.")
        return output

# GSP bootloader
def bootloader(gpu, type):
    global outputpath
    global version

    GPU=gpu.upper()
    filename = f"src/nvidia/generated/g_bindata_kgspGetBinArchiveGspRmBoot_{GPU}.c"

    print(f"Creating nvidia/{gpu}/gsp/bootloader-{version}.bin")
    os.makedirs(f"{outputpath}/nvidia/{gpu}/gsp/", exist_ok = True)

    with open(f"{outputpath}/nvidia/{gpu}/gsp/bootloader-{version}.bin", "wb") as f:
        # Extract the actual bootloader firmware
        array = f"kgspBinArchiveGspRmBoot_{GPU}_ucode_image{type}data"
        firmware = getbytes(filename, array)
        firmware_size = len(firmware)

        # Extract the descriptor (RM_RISCV_UCODE_DESC)
        array = f"kgspBinArchiveGspRmBoot_{GPU}_ucode_desc{type}data"
        descriptor = getbytes(filename, array)
        descriptor_size = len(descriptor)

        # First, add the nvfw_bin_hdr header
        total_size = round_up_to_base(24 + firmware_size + descriptor_size, 256)
        firmware_offset = 24 + descriptor_size
        f.write(struct.pack("<6L", 0x10de, 1, total_size, 24, firmware_offset, firmware_size))

        # Second, add the descriptor
        f.write(descriptor)

        # Finally, the actual bootloader image
        f.write(firmware)

# GSP Booter load and unload
def booter(gpu, load, sigsize):
    global outputpath
    global version

    GPU = gpu.upper()
    LOAD = load.capitalize()

    filename = f"src/nvidia/generated/g_bindata_kgspGetBinArchiveBooter{LOAD}Ucode_{GPU}.c"

    print(f"Creating nvidia/{gpu}/gsp/booter_{load}-{version}.bin")
    os.makedirs(f"{outputpath}/nvidia/{gpu}/gsp/", exist_ok = True)

    with open(f"{outputpath}/nvidia/{gpu}/gsp/booter_{load}-{version}.bin", "wb") as f:
        # Extract the actual scrubber firmware
        array = f"kgspBinArchiveBooter{LOAD}Ucode_{GPU}_image_prod_data"
        firmware = getbytes(filename, array)
        firmware_size = len(firmware)

        # Extract the signatures
        array = f"kgspBinArchiveBooter{LOAD}Ucode_{GPU}_sig_prod_data"
        signatures = getbytes(filename, array)
        signatures_size = len(signatures)
        if signatures_size % sigsize:
            raise MyException(f"signature file size for {array} is uneven value of {sigsize}")
        num_sigs = int(signatures_size / sigsize);
        if num_sigs < 1:
            raise MyException(f"invalid number of signatures {num_sigs}")

        # First, add the nvfw_bin_hdr header
        total_size = round_up_to_base(120 + signatures_size + firmware_size, 256)
        firmware_offset = 120 + signatures_size
        f.write(struct.pack("<6L", 0x10de, 1, total_size, 24, firmware_offset, firmware_size))

        # Second, add the nvfw_hs_header_v2 header
        patch_loc_offset = 60 + signatures_size
        patch_sig_offset = patch_loc_offset + 4
        meta_data_offset = patch_sig_offset + 4
        num_sig_offset = meta_data_offset + 12
        header_offset = num_sig_offset + 4
        f.write(struct.pack("<9L", 60, signatures_size, patch_loc_offset,
                             patch_sig_offset, meta_data_offset, 12,
                             num_sig_offset, header_offset, 36))

        # Third, the actual signatures
        f.write(signatures)

        # Extract the patch location
        array = f"kgspBinArchiveBooter{LOAD}Ucode_{GPU}_patch_loc_data"
        bytes = getbytes(filename, array)
        patchloc = struct.unpack("<L", bytes)[0]

        # Extract the patch meta variables
        array = f"kgspBinArchiveBooter{LOAD}Ucode_{GPU}_patch_meta_data"
        bytes = getbytes(filename, array)
        fuse_ver, engine_id, ucode_id = struct.unpack("<LLL", bytes)

        # Fourth, patch_loc[], patch_sig[], fuse_ver, engine_id, ucode_id, and num_sigs
        f.write(struct.pack("<6L", patchloc, 0, fuse_ver, engine_id, ucode_id, num_sigs))

        # Extract the descriptor (nvkm_gsp_booter_fw_hdr)
        array = f"kgspBinArchiveBooter{LOAD}Ucode_{GPU}_header_prod_data"
        descriptor = getbytes(filename, array)

        # Fifth, the descriptor
        f.write(descriptor)

        # And finally, the actual scrubber image
        f.write(firmware)

# GPU memory scrubber, needed for some GPUs and configurations
def scrubber(gpu, sigsize):
    global outputpath
    global version

    # Unfortunately, RM breaks convention with the scrubber image and labels
    # the files and arrays with AD10X instead of AD102.
    GPUX = f"{gpu[:-1].upper()}X"

    filename = f"src/nvidia/generated/g_bindata_ksec2GetBinArchiveSecurescrubUcode_{GPUX}.c"

    print(f"Creating nvidia/{gpu}/gsp/scrubber-{version}.bin")
    os.makedirs(f"{outputpath}/nvidia/{gpu}/gsp/", exist_ok = True)

    with open(f"{outputpath}/nvidia/{gpu}/gsp/scrubber-{version}.bin", "wb") as f:
        # Extract the actual scrubber firmware
        array = f"ksec2BinArchiveSecurescrubUcode_{GPUX}_image_prod_data[]"
        firmware = getbytes(filename, array)
        firmware_size = len(firmware)

        # Extract the signatures
        array = f"ksec2BinArchiveSecurescrubUcode_{GPUX}_sig_prod_data"
        signatures = getbytes(filename, array)
        signatures_size = len(signatures)
        if signatures_size % sigsize:
            raise MyException(f"signature file size for {array} is uneven value of {sigsize}")
        num_sigs = int(signatures_size / sigsize);
        if num_sigs < 1:
            raise MyException(f"invalid number of signatures {num_sigs}")

        # First, add the nvfw_bin_hdr header
        total_size = round_up_to_base(120 + signatures_size + firmware_size, 256)
        firmware_offset = 120 + signatures_size
        f.write(struct.pack("<6L", 0x10de, 1, total_size, 24, firmware_offset, firmware_size))

        # Second, add the nvfw_hs_header_v2 header
        patch_loc_offset = 60 + signatures_size
        patch_sig_offset = patch_loc_offset + 4
        meta_data_offset = patch_sig_offset + 4
        num_sig_offset = meta_data_offset + 12
        header_offset = num_sig_offset + 4
        f.write(struct.pack("<9L", 60, signatures_size, patch_loc_offset,
                             patch_sig_offset, meta_data_offset, 12,
                             num_sig_offset, header_offset, 36))

        # Third, the actual signatures
        f.write(signatures)

        # Extract the patch location
        array = f"ksec2BinArchiveSecurescrubUcode_{GPUX}_patch_loc_data"
        bytes = getbytes(filename, array)
        patchloc = struct.unpack("<L", bytes)[0]

        # Extract the patch meta variables
        array = f"ksec2BinArchiveSecurescrubUcode_{GPUX}_patch_meta_data"
        bytes = getbytes(filename, array)
        fuse_ver, engine_id, ucode_id = struct.unpack("<LLL", bytes)

        # Fourth, patch_loc[], patch_sig[], fuse_ver, engine_id, ucode_id, and num_sigs
        f.write(struct.pack("<6L", patchloc, 0, fuse_ver, engine_id, ucode_id, num_sigs))

        # Extract the descriptor (nvkm_gsp_booter_fw_hdr)
        array = f"ksec2BinArchiveSecurescrubUcode_{GPUX}_header_prod_data"
        descriptor = getbytes(filename, array)

        # Fifth, the descriptor
        f.write(descriptor)

        # And finally, the actual scrubber image
        f.write(firmware)

def main():
    global outputpath
    global version

    parser = argparse.ArgumentParser(
        description = 'Extract firmware binaries from the OpenRM git repository'
        ' in a format expected by the Nouveau device driver.')
    parser.add_argument('-i', '--input', default = os.getcwd(),
        help = 'Path to source directory (where version.mk exists)')
    parser.add_argument('-o', '--output', default = os.path.abspath(os.getcwd() + '/_out'),
        help = 'Path to target directory (where files will be written)')
    args = parser.parse_args()

    os.chdir(args.input)

    with open("version.mk") as f:
        version = re.search(r'^NVIDIA_VERSION = ([^\s]+)', f.read(), re.MULTILINE).group(1)
    print(f"Generating files for version {version}")

    # Normal version strings are of the format xxx.yy.zz, which are all
    # numbers. If it's a normal version string, convert it to a single number,
    # as Nouveau currently expects.  Otherwise, leave it alone.
    if set(version) <= set('0123456789.'):
        version = version.replace(".", "")

    outputpath = args.output;
    print(f"Writing files to {outputpath}")

    os.makedirs(f"{outputpath}/nvidia", exist_ok = True)

    booter("tu102", "load", 16)
    booter("tu102", "unload", 16)
    bootloader("tu102", "_")

    booter("tu116", "load", 16)
    booter("tu116", "unload", 16)
    # TU11x uses the same bootloader as TU10x

    booter("ga100", "load", 384)
    booter("ga100", "unload", 384)
    bootloader("ga100", "_")

    booter("ga102", "load", 384)
    booter("ga102", "unload", 384)
    bootloader("ga102", "_prod_")

    booter("ad102", "load", 384)
    booter("ad102", "unload", 384)
    bootloader("ad102", "_prod_")
    # scrubber("ad102", 384) # Not currently used by Nouveau

if __name__ == "__main__":
    main()

