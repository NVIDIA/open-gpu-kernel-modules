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

# Converts OpenRM binhex-encoded images to Nouveau- and Nova-compatible
# binary blobs.
# See nouveau_firmware_layout.ods for documentation on the file format.

import sys
import os
import argparse
import re
import gzip
import struct
import zlib
import tempfile
import urllib.request

FLCN_BLK_ALIGNMENT = 256

class MyException(Exception):
    pass

def round_up_to_base(x, base = 10):
    return x + (base - x) % base

def parse_array(f):
    """Parses a bindata array definition and returns its binhex as bytes

    Example:
    static BINDATA_CONST NvU8 ksec2BinArchiveSecurescrubUcode_AD10X_header_prod_data[] =
    {
        0x63, 0x60, 0x00, 0x02, 0x46, 0x20, 0x96, 0x02, 0x62, 0x66, 0x08, 0x13, 0x4c, 0x48, 0x42, 0x69,
        0x20, 0x00, 0x00, 0x30, 0x39, 0x0a, 0xfc, 0x24, 0x00, 0x00, 0x00,
    };
    """
    output = b''
    for line in f:
        if "};" in line:
            break
        bytes = [int(b, 16) for b in re.findall('0x[0-9a-f][0-9a-f]', line)]
        if len(bytes) > 0:
            output += struct.pack(f"{len(bytes)}B", *bytes)

    return output

def parse_struct(f):
    """Parses a struct definition and returns its binhex as bytes

    Example:
    static const RM_FLCN_BL_DESC ksec2BinArchiveBlUcode_TU102_ucode_desc_data = {
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

    # Build the five possible array/struct names.  BINDATA_LABEL was added in r575,
    # and NV_DECLARE_ALIGNED(NvU8, 8) was added in r590.
    arrays = [
        f"static BINDATA_CONST NvU8 {array1}_{array2}_data",
        f"static BINDATA_CONST NvU8 {array1}_BINDATA_LABEL_{array2.upper()}_data",
        f"static BINDATA_CONST NV_DECLARE_ALIGNED(NvU8, 8) {array1}_BINDATA_LABEL_{array2.upper()}_data",
        f"static const {array1}_{array2}_data",
        f"static const {array1}_BINDATA_LABEL_{array2.upper()}_data",
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
            m = next((a for a in arrays if a in line), None)
            if m:
                # We found the array, so remember its name in case we need to report an error
                array = m
                break
        else:
            raise MyException(f"array {array1}_{array2}_data not found in {filename}")

        if is_struct:
            output = parse_struct(f)
            # Struct entries reference themselves for the size.  The only way
            # to determine the actual size is to compile the C code.  Instead,
            # just assume the header file is complete.
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

    if is_compressed:
        if len(output) != compressed_size:
            raise MyException(f"compressed array {array} in {filename} should be {compressed_size} bytes but is actually {len(output)}.")
        gzipheader = struct.pack("<4BL2B", 0x1f, 0x8b, 8, 0, 0, 0, 3)
        output = gzip.decompress(gzipheader + output)
        if len(output) != data_size:
            raise MyException(f"array {array} in {filename} decompressed to {len(output)} bytes but should have been {data_size} bytes.")
        return output
    else:
        if len(output) != data_size:
            raise MyException(f"array {array} in {filename} should be {data_size} bytes but is actually {len(output)}.")
        return output

# Generic Falcon bootloader.  First, FWSEC runs on the RISC-V GSP core.
# Then this generic bootloader runs on the SEC2 core, in order to restart the GSP
# core to run GSP-RM on it.  This is only used on TU10x and GA100 GPUs.
def generic_bootloader(gpu):
    global outputpath
    global version

    GPU = gpu.upper()
    filename = f"src/nvidia/generated/g_bindata_ksec2GetBinArchiveBlUcode_{GPU}.c"

    print(f"Creating nvidia/{gpu}/gsp/gen_bootloader-{version}.bin")
    os.makedirs(f"{outputpath}/nvidia/{gpu}/gsp/", exist_ok = True)

    with open(f"{outputpath}/nvidia/{gpu}/gsp/gen_bootloader-{version}.bin", "wb") as f:
        # Extract the actual bootloader firmware
        firmware = get_bytes(filename, f"ksec2BinArchiveBlUcode_{GPU}", "ucode_image")
        firmware_size = len(firmware)

        # Extract the descriptor (RM_RISCV_UCODE_DESC)
        # Note: the size of RM_RISCV_UCODE_DESC varies from version to version, but Nouveau
        # only cares about the first few fields.
        descriptor = get_bytes(filename, f"RM_FLCN_BL_DESC ksec2BinArchiveBlUcode_{GPU}", "ucode_desc")
        descriptor_size = len(descriptor) # 24

        # First, add the nvfw_bin_hdr header
        total_size = round_up_to_base(24 + firmware_size + descriptor_size, FLCN_BLK_ALIGNMENT)
        firmware_offset = 24 + descriptor_size
        f.write(struct.pack("<6L", 0x10de, 1, total_size, 24, firmware_offset, firmware_size))

        # Second, add the descriptor
        f.write(descriptor)

        # Finally, the actual bootloader image
        f.write(firmware)

# GSP bootloader
def gsp_bootloader(gpu, fuse = ""):
    global outputpath
    global version

    # Prepend an underscore if not empty
    if len(fuse) > 0:
        fuse = f"_{fuse}"

    GPU = gpu.upper()
    filename = f"src/nvidia/generated/g_bindata_kgspGetBinArchiveGspRmBoot_{GPU}.c"

    print(f"Creating nvidia/{gpu}/gsp/bootloader-{version}.bin")
    os.makedirs(f"{outputpath}/nvidia/{gpu}/gsp/", exist_ok = True)

    with open(f"{outputpath}/nvidia/{gpu}/gsp/bootloader-{version}.bin", "wb") as f:
        # Extract the actual bootloader firmware
        firmware = get_bytes(filename, f"kgspBinArchiveGspRmBoot_{GPU}", f"ucode_image{fuse}")
        firmware_size = len(firmware)

        # Extract the descriptor (RM_RISCV_UCODE_DESC)
        descriptor = get_bytes(filename, f"kgspBinArchiveGspRmBoot_{GPU}", f"ucode_desc{fuse}")
        descriptor_size = len(descriptor) # 76 on TU10x/GA100, 84 on GA102+

        # First, add the nvfw_bin_hdr header
        total_size = round_up_to_base(24 + firmware_size + descriptor_size, FLCN_BLK_ALIGNMENT)
        firmware_offset = 24 + descriptor_size
        f.write(struct.pack("<6L", 0x10de, 1, total_size, 24, firmware_offset, firmware_size))

        # Second, add the descriptor
        f.write(descriptor)

        # Finally, the actual bootloader image
        f.write(firmware)

# GSP Booter load and unload
def booter(gpu, load, sigsize, fuse = "prod"):
    global outputpath
    global version

    GPU = gpu.upper()
    LOAD = load.capitalize()
    name = f"booter-{load}-{gpu}-{fuse}"

    filename = f"src/nvidia/generated/g_bindata_kgspGetBinArchiveBooter{LOAD}Ucode_{GPU}.c"

    print(f"Creating nvidia/{gpu}/gsp/booter_{load}-{version}.bin")
    os.makedirs(f"{outputpath}/nvidia/{gpu}/gsp/", exist_ok = True)

    with open(f"{outputpath}/nvidia/{gpu}/gsp/booter_{load}-{version}.bin", "wb") as f:
        # Extract the actual booter firmware
        firmware = get_bytes(filename, f"kgspBinArchiveBooter{LOAD}Ucode_{GPU}", f"image_{fuse}")
        firmware_size = len(firmware)

        # Query the number of signatures.  This should be a 4-byte array (32-bit little-endian integer)
        bytes = get_bytes(filename, f"kgspBinArchiveBooter{LOAD}Ucode_{GPU}", "num_sigs")
        if len(bytes) != 4:
            raise MyException(f"num_sigs array for {name} is wrong size of {len(bytes)}")
        num_sigs = struct.unpack("<L", bytes)[0]
        if num_sigs < 1 or num_sigs > 15:
            raise MyException(f"out of range number of signatures ({num_sigs}) for {name}")

        # Extract the signatures.  Technically, we don't need to pass the signature size to
        # this function, but doing so allows us to double-check all the array sizes.
        signatures = get_bytes(filename, f"kgspBinArchiveBooter{LOAD}Ucode_{GPU}", f"sig_{fuse}")
        signatures_size = len(signatures)
        if signatures_size % sigsize:
            raise MyException(f"signature file size for {name} is {signatures_size}, an uneven multiple of {sigsize}")
        if num_sigs != signatures_size // sigsize:
            raise MyException(f"mismatch number of signatures ({signatures_size // sigsize}), should be {num_sigs}")

        # First, add the nvfw_bin_hdr header
        # 120 = sizeof(nvfw_bin_hdr) + sizeof(nvfw_hs_header_v2) + sizeof(meta vars) +
        #   sizeof(nvfw_hs_load_header_v2)
        # Note that 120 assumes that nvfw_hs_header_v2.num_apps == 1
        total_size = round_up_to_base(120 + signatures_size + firmware_size, FLCN_BLK_ALIGNMENT)
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
        bytes = get_bytes(filename, f"kgspBinArchiveBooter{LOAD}Ucode_{GPU}", "patch_loc")
        patchloc = struct.unpack("<L", bytes)[0]

        # Extract the patch meta variables
        bytes = get_bytes(filename, f"kgspBinArchiveBooter{LOAD}Ucode_{GPU}", "patch_meta")
        fuse_ver, engine_id, ucode_id = struct.unpack("<LLL", bytes)

        # Fourth, patch_loc[], patch_sig[], fuse_ver, engine_id, ucode_id, and num_sigs
        f.write(struct.pack("<6L", patchloc, 0, fuse_ver, engine_id, ucode_id, num_sigs))

        # Extract the descriptor (nvfw_hs_load_header_v2)
        descriptor = get_bytes(filename, f"kgspBinArchiveBooter{LOAD}Ucode_{GPU}", f"header_{fuse}")

        # Extract some of individual fields of nvfw_hs_load_header_v2
        # num_apps is the fifth field of struct nvfw_hs_load_header_v2
        (os_code_offset, os_code_size, os_data_offset, os_data_size, num_apps,
         app_code_offset, app_code_size, app_data_offset, app_data_size) = struct.unpack("<9L", descriptor)
        # Verify that sizeof(descriptor) == 5 * 4 + num_apps * 16
        if len(descriptor) != 5 * 4 + num_apps * 16:
            raise MyException(f"nvfw_hs_load_header_v2 descriptor for {name} should be {5 * 4 + num_apps * 16} bytes, but is instead {len(descriptor)} bytes.")
        # Nova depends on os_code_size == app_code_offset
        if os_code_size != app_code_offset:
            raise MyException(f"nvfw_hs_load_header_v2 descriptor for {name} has os_code_size={os_code_size} and app_code_offset={app_code_offset}, but they should be the same.")

        # Fifth, the descriptor
        f.write(descriptor)

        # And finally, the actual booter image
        f.write(firmware)

# GPU memory scrubber, needed for some GPUs and configurations
def scrubber(gpu, sigsize, fuse = "prod"):
    global outputpath
    global version

    # Unfortunately, RM breaks convention with the scrubber image and labels
    # the files and arrays with AD10X instead of AD102.
    GPUX = f"{gpu[:-1].upper()}X"
    name = f"scrubber-{gpu}-{fuse}"

    filename = f"src/nvidia/generated/g_bindata_ksec2GetBinArchiveSecurescrubUcode_{GPUX}.c"

    print(f"Creating nvidia/{gpu}/gsp/scrubber-{version}.bin")
    os.makedirs(f"{outputpath}/nvidia/{gpu}/gsp/", exist_ok = True)

    with open(f"{outputpath}/nvidia/{gpu}/gsp/scrubber-{version}.bin", "wb") as f:
        # Extract the actual scrubber firmware
        firmware = get_bytes(filename, f"ksec2BinArchiveSecurescrubUcode_{GPUX}", f"image_{fuse}")
        firmware_size = len(firmware)

        # Query the number of signatures.  This should be a 4-byte array (32-bit little-endian integer)
        bytes = get_bytes(filename, f"ksec2BinArchiveSecurescrubUcode_{GPUX}", "num_sigs")
        if len(bytes) != 4:
            raise MyException(f"num_sigs array for {name} is wrong size of {len(bytes)}")
        num_sigs = struct.unpack("<L", bytes)[0]
        if num_sigs < 1 or num_sigs > 15:
            raise MyException(f"out of range number of signatures ({num_sigs}) for {name}")

        # Extract the signatures
        signatures = get_bytes(filename, f"ksec2BinArchiveSecurescrubUcode_{GPUX}", f"sig_{fuse}")
        signatures_size = len(signatures)
        if signatures_size % sigsize:
            raise MyException(f"signature file size for {name} is {signatures_size}, an uneven multiple of {sigsize}")
        if num_sigs != signatures_size // sigsize:
            raise MyException(f"mismatch number of signatures ({signatures_size // sigsize}), should be {num_sigs}")

        # First, add the nvfw_bin_hdr header
        total_size = round_up_to_base(120 + signatures_size + firmware_size, FLCN_BLK_ALIGNMENT)
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
        bytes = get_bytes(filename, f"ksec2BinArchiveSecurescrubUcode_{GPUX}", "patch_loc")
        patchloc = struct.unpack("<L", bytes)[0]

        # Extract the patch meta variables
        bytes = get_bytes(filename, f"ksec2BinArchiveSecurescrubUcode_{GPUX}", "patch_meta")
        fuse_ver, engine_id, ucode_id = struct.unpack("<LLL", bytes)

        # Fourth, patch_loc[], patch_sig[], fuse_ver, engine_id, ucode_id, and num_sigs
        f.write(struct.pack("<6L", patchloc, 0, fuse_ver, engine_id, ucode_id, num_sigs))

        # Extract the descriptor (nvkm_gsp_booter_fw_hdr)
        descriptor = get_bytes(filename, f"ksec2BinArchiveSecurescrubUcode_{GPUX}", f"header_{fuse}")

        # Fifth, the descriptor
        f.write(descriptor)

        # And finally, the actual scrubber image
        f.write(firmware)

# Elf header sizes for 32-bit and 64-bit ELF images
ELF_HDR_SIZE_32 = 52
ELF_HDR_SIZE_64 = 64

# Elf section header sizes for 32-bit and 64-bit ELF images
ELF_SHDR_SIZE_32 = 40
ELF_SHDR_SIZE_64 = 64

# Padded alignment of blobs inside the ELF sections
ELF_ALIGNMENT = 4

# Create an ELF header with no program header and 'shnum' section headers,
# not including the .shstrtab and NULL sections.
# The section headers appear after the ELF header, and the section data
# follows.  Note that e_shstrndx cannot be zero, because that implies
# that the .shstrndx sectio`n does not exist.
def elf_header(shnum: int, elf64: bool):
    PACK = "<B3s5B7xHHI3QI6H" if elf64 else "<B3s5B7xHH5I6H"
    ELF_HDR_SIZE = ELF_HDR_SIZE_64 if elf64 else ELF_HDR_SIZE_32
    ELF_SHDR_SIZE = ELF_SHDR_SIZE_64 if elf64 else ELF_SHDR_SIZE_32
    EI_CLASS = 2 if elf64 else 1

    bytes = struct.pack(PACK,
        0x7f, b'ELF',
        EI_CLASS, 1, 1, 0, 0, # EI_CLASS, EI_DATA, EI_VERSION, EI_OSABI, EI_ABIVERSION
        0, 0, 1, # e_type, e_machine, e_version
        0, 0, ELF_HDR_SIZE, 0, # e_entry, e_phoff, e_shoff, e_flags
        ELF_HDR_SIZE, 0, 0, # e_ehsize, e_phentsize, e_phnum
        ELF_SHDR_SIZE, shnum + 2, 1) # e_shentsize, e_shnum, e_shstrndx

    return bytes

# Create an ELF section header, where 'sh_name' is the offset of the
# section name, 'sh_offset' is the offset of the section data, and 'sh_size'
# is the size (in bytes) of the image in the section data.
# We set sh_flags to SHF_OS_NONCONFORMING and use the sh_info field to store
# a 32-bit CRC of the image data.
def elf_section_header(sh_name, sh_offset, sh_size, sh_info, elf64: bool):
    PACK = "<2I4Q2I2Q" if elf64 else "<10I"

    bytes = struct.pack(PACK,
        sh_name,
        1, 0xFFF00102, 0, # sh_type, sh_flags, sh_addr
        sh_offset, sh_size,
        0, # sh_link
        sh_info,
        4, 0) # sh_addralign, sh_entsize

    return bytes

# A little-known fact about ELF files is that the first section header must
# be empty.  Readelf doesn't care about that, but objdump does.  This may be
# why the first byte of the .shstrtab should be zero.
def elf_section_header_null(elf64: bool):
    ELF_SHDR_SIZE = ELF_SHDR_SIZE_64 if elf64 else ELF_SHDR_SIZE_32

    return b'\0' * ELF_SHDR_SIZE

# Create a shstrtab ELF section header.
# 'shnum' is the number of sections.
# 'sh_offset' is the offset of the .shstrtab section.
# 'sh_size' is the unpadded size of the section.
# The section itself should be padded to the nearest 8-byte boundary, so that
# all the sections are aligned.
def elf_section_header_shstrtab(sh_name, shnum, sh_size, elf64: bool):
    PACK = "<2I4Q2I2Q" if elf64 else "<10I"
    ELF_HDR_SIZE = ELF_HDR_SIZE_64 if elf64 else ELF_HDR_SIZE_32
    ELF_SHDR_SIZE = ELF_SHDR_SIZE_64 if elf64 else ELF_SHDR_SIZE_32

    sh_offset = ELF_HDR_SIZE + ELF_SHDR_SIZE * (shnum + 2);

    bytes = struct.pack(PACK,
        sh_name,
        3, 0x20, 0, # sh_type (SHT_STRTAB), sh_flags (SHF_STRINGS), sh_addr
        sh_offset, sh_size,
        0, 0, 1, 1) # sh_link, sh_info, sh_addralign, sh_entsize

    return bytes

# Build the .shstrtab section, where 'names' is a list of strings
def elf_build_shstrtab(names):
    bytes = bytearray(b'\0')
    for name in ['.shstrtab'] + names:
        bytes.extend(name.encode('ascii') + b'\x00')

    return bytes

# Returns a tuple of the size of a bytearray and the size aligned up
def sizes(b):
    return (len(b), round_up_to_base(len(b), ELF_ALIGNMENT))

# Returns the sh_name offset of a given section name in the .shstrtab section
# 'needle' is the name of the section
# 'haystack' is the .shstrtab section
def offset_of(needle, haystack):
    null_terminated = bytearray(needle.encode('ascii') + b'\x00')
    position = haystack.find(null_terminated)
    if position == -1:
        raise MyException(f"unknown section name {needle}")

    return position

# Writes a bunch of bytes to f, padded with zeroes to the nearest 4 bytes
# Returns the total number of bytes written
def write_padded(f, b):
    f.write(b)

    (len, padded) = sizes(b)
    if padded > len:
        padding_length = padded - len;
        f.write(b'\0' * padding_length)

    return padded

# Unlike the other images, FMC firmware and its metadata are encapsulated in
# an ELF image.  FMC metadata is simpler than the other firmware types, as it
# comprises just three binary blobs.
def fmc(gpu: str, fuse: str, elf64: bool):
    global outputpath
    global version

    ELF_HDR_SIZE = ELF_HDR_SIZE_64 if elf64 else ELF_HDR_SIZE_32
    ELF_SHDR_SIZE = ELF_SHDR_SIZE_64 if elf64 else ELF_SHDR_SIZE_32

    GPU=gpu.upper()
    filename = f"src/nvidia/generated/g_bindata_kgspGetBinArchiveGspRmFmcGfw{fuse}Signed_{GPU}.c"

    print(f"Creating nvidia/{gpu}/gsp/fmc-{version}.bin")
    os.makedirs(f"{outputpath}/nvidia/{gpu}/gsp/", exist_ok = True)

    ucode_hash = get_bytes(filename, f"kgspBinArchiveGspRmFmcGfw{fuse}Signed_{GPU}", "ucode_hash")
    (ucode_hash_size, ucode_hash_padded_size) = sizes(ucode_hash)

    ucode_sig = get_bytes(filename, f"kgspBinArchiveGspRmFmcGfw{fuse}Signed_{GPU}", "ucode_sig")
    (ucode_sig_size, ucode_sig_padded_size) = sizes(ucode_sig)

    ucode_pkey = get_bytes(filename, f"kgspBinArchiveGspRmFmcGfw{fuse}Signed_{GPU}", "ucode_pkey")
    (ucode_pkey_size, ucode_pkey_padded_size) = sizes(ucode_pkey)

    ucode_image = get_bytes(filename, f"kgspBinArchiveGspRmFmcGfw{fuse}Signed_{GPU}", "ucode_image")
    (ucode_image_size, ucode_image_padded_size) = sizes(ucode_image)

    shnum = 4 # The number of image sections

    # Build the .shstrtab section data
    shstrtab = elf_build_shstrtab(['hash', 'signature', 'publickey', 'image'])
    (shstrtab_size, shstrtab_padded_size) = sizes(shstrtab)

    # Calculate the offsets of each section
    shstrtab_offset = ELF_HDR_SIZE + ELF_SHDR_SIZE * (shnum + 2)
    hash_offset = shstrtab_offset + shstrtab_padded_size
    signature_offset = hash_offset + ucode_hash_padded_size
    pkey_offset = signature_offset + ucode_sig_padded_size
    image_offset = pkey_offset + ucode_pkey_padded_size

    with open(f"{outputpath}/nvidia/{gpu}/gsp/fmc-{version}.bin", "wb") as f:
        # Create the ELF header
        header = elf_header(shnum, elf64)
        f.write(header)

        # Add the section headers

        header = elf_section_header_null(elf64)
        f.write(header)

        header = elf_section_header_shstrtab(offset_of(".shstrtab", shstrtab), shnum, len(shstrtab), elf64)
        f.write(header)

        header = elf_section_header(offset_of("hash", shstrtab),
            hash_offset, ucode_hash_size, zlib.crc32(ucode_hash), elf64)
        f.write(header)

        header = elf_section_header(offset_of("signature", shstrtab),
            signature_offset, ucode_sig_size, zlib.crc32(ucode_sig), elf64)
        f.write(header)

        header = elf_section_header(offset_of("publickey", shstrtab),
            pkey_offset, ucode_pkey_size, zlib.crc32(ucode_pkey), elf64)
        f.write(header)

        header = elf_section_header(offset_of("image", shstrtab),
            image_offset, ucode_image_size, zlib.crc32(ucode_image), elf64)
        f.write(header)

        # Make sure we're where we are supposed to be
        assert f.tell() == shstrtab_offset

        # Write the .shstrtab section data.
        write_padded(f, shstrtab)
        assert f.tell() % ELF_ALIGNMENT == 0

        # Finally, write the four images in sequence
        write_padded(f, ucode_hash)
        assert f.tell() % ELF_ALIGNMENT == 0

        write_padded(f, ucode_sig)
        assert f.tell() % ELF_ALIGNMENT == 0

        write_padded(f, ucode_pkey)
        assert f.tell() % ELF_ALIGNMENT == 0

        write_padded(f, ucode_image)
        assert f.tell() % ELF_ALIGNMENT == 0

# Extract the GSP-RM firmware from the .run file and copy the binaries
# to the target directory.
def gsp_firmware(filename):
    global outputpath
    global version

    import subprocess
    import shutil

    basename = os.path.basename(filename)

    with tempfile.TemporaryDirectory() as temp:
        os.chdir(temp)

        try:
            print(f"Validating {basename}")

            result = subprocess.run(['/bin/sh', filename, '--check'], shell=False,
                                    check=True, timeout=10,
                                    stdout = subprocess.PIPE, stderr = subprocess.STDOUT)
            output = result.stdout.strip().decode("ascii")
            if not "check sums and md5 sums are ok" in output:
                raise MyException(f"{basename} is not a valid Nvidia driver .run file")
        except subprocess.CalledProcessError as error:
            print(error.output.decode())
            raise

        try:
            print(f"Extracting {basename} to {temp}")
            # The -x parameter tells the installer to only extract the
            # contents and then exit.
            subprocess.run(['/bin/sh', filename, '-x'], shell=False,
                           check=True, timeout=60,
                           stdout = subprocess.PIPE, stderr = subprocess.STDOUT)
        except subprocess.SubprocessError as error:
            print(error.output.decode())
            raise

        try:
            # The .run file extracts its contents to a directory with the same
            # name as the file itself, minus the .run.  The GSP-RM firmware
            # images are in the 'firmware' subdirectory.
            result = subprocess.run(['/bin/sh', filename, '--target-directory'], shell=False,
                                    check=True, timeout=10,
                                    stdout = subprocess.PIPE, stderr = subprocess.DEVNULL)
            directory = result.stdout.strip().decode("ascii")
            os.chdir(f"{directory}/firmware")
        except subprocess.SubprocessError as e:
            print(e.output.decode())
            raise

        if not os.path.exists('gsp_tu10x.bin') or not os.path.exists('gsp_ga10x.bin'):
            raise MyException(f"Firmware files are missing in {basename}")

        shutil.copyfile('gsp_tu10x.bin', f"{outputpath}/nvidia/tu102/gsp/gsp-{version}.bin")
        print(f"Copied gsp_tu10x.bin to tu102/gsp/gsp-{version}.bin")
        shutil.copyfile('gsp_ga10x.bin', f"{outputpath}/nvidia/ga102/gsp/gsp-{version}.bin")
        print(f"Copied gsp_ga10x.bin to ga102/gsp/gsp-{version}.bin")

        # Copy ucodes binaries if present (r610+).  Each ucodes.bin is paired
        # with the corresponding gsp.bin and loaded separately by the driver.
        if os.path.exists('ucodes_tu10x.bin'):
            shutil.copyfile('ucodes_tu10x.bin', f"{outputpath}/nvidia/tu102/gsp/ucodes-{version}.bin")
            print(f"Copied ucodes_tu10x.bin to tu102/gsp/ucodes-{version}.bin")
        if os.path.exists('ucodes_ga10x.bin'):
            shutil.copyfile('ucodes_ga10x.bin', f"{outputpath}/nvidia/ga102/gsp/ucodes-{version}.bin")
            print(f"Copied ucodes_ga10x.bin to ga102/gsp/ucodes-{version}.bin")


# Extract GSP firmware from a local build output directory.
# This is an NVIDIA-internal feature for use with internal build systems.
def gsp_firmware_from_build(gsp_build_dir):
    global outputpath
    global version

    import shutil

    if not os.path.isdir(gsp_build_dir):
        raise MyException(f"GSP build directory does not exist: {gsp_build_dir}")

    tu10x_src = os.path.join(gsp_build_dir, "gsp_tu10x.bin")
    ga10x_src = os.path.join(gsp_build_dir, "gsp_ga10x.bin")

    if not os.path.exists(tu10x_src):
        raise MyException(f"GSP firmware not found: {tu10x_src}")
    if not os.path.exists(ga10x_src):
        raise MyException(f"GSP firmware not found: {ga10x_src}")

    os.makedirs(f"{outputpath}/nvidia/tu102/gsp/", exist_ok = True)
    os.makedirs(f"{outputpath}/nvidia/ga102/gsp/", exist_ok = True)

    shutil.copyfile(tu10x_src, f"{outputpath}/nvidia/tu102/gsp/gsp-{version}.bin")
    print(f"Copied gsp_tu10x.bin to nvidia/tu102/gsp/gsp-{version}.bin")

    shutil.copyfile(ga10x_src, f"{outputpath}/nvidia/ga102/gsp/gsp-{version}.bin")
    print(f"Copied gsp_ga10x.bin to nvidia/ga102/gsp/gsp-{version}.bin")

    # Copy ucodes binaries if present (r610+)
    ucodes_tu10x_src = os.path.join(gsp_build_dir, "ucodes_tu10x.bin")
    ucodes_ga10x_src = os.path.join(gsp_build_dir, "ucodes_ga10x.bin")

    if os.path.exists(ucodes_tu10x_src):
        shutil.copyfile(ucodes_tu10x_src, f"{outputpath}/nvidia/tu102/gsp/ucodes-{version}.bin")
        print(f"Copied ucodes_tu10x.bin to nvidia/tu102/gsp/ucodes-{version}.bin")

    if os.path.exists(ucodes_ga10x_src):
        shutil.copyfile(ucodes_ga10x_src, f"{outputpath}/nvidia/ga102/gsp/ucodes-{version}.bin")
        print(f"Copied ucodes_ga10x.bin to nvidia/ga102/gsp/ucodes-{version}.bin")

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

# Create symlinks in the target directory for the other GPUs.  This mirrors
# what the WHENCE file in linux-firmware does.
def symlinks():
    global outputpath
    global version

    print(f"Creating symlinks in {outputpath}/nvidia")
    os.chdir(f"{outputpath}/nvidia")

    for d in ['tu116', 'ga100', 'ad102']:
        os.makedirs(d, exist_ok = True)

    for d in ['tu104', 'tu106']:
        os.makedirs(d, exist_ok = True)
        symlink('../tu102/gsp', f"{d}/gsp", target_is_directory = True)

    os.makedirs('tu117', exist_ok = True)
    symlink('../tu116/gsp', 'tu117/gsp', target_is_directory = True)

    for d in ['ga103', 'ga104', 'ga106', 'ga107']:
        os.makedirs(d, exist_ok = True)
        symlink('../ga102/gsp', f"{d}/gsp", target_is_directory = True)

    for d in ['ad103', 'ad104', 'ad106', 'ad107']:
        # Some older versions of /lib/firmware had symlinks from ad10x/gsp to ad102/gsp,
        # even though there were no other directories in ad10x.  Delete the existing
        # ad10x directory so that we can replace it with a symlink.
        if os.path.islink(f"{d}/gsp"):
            os.remove(f"{d}/gsp")
            os.rmdir(d)
        symlink('ad102', d, target_is_directory = True)

    # TU11x uses the same GSP bootloader as TU10x
    symlink(f"../../tu102/gsp/bootloader-{version}.bin", f"tu116/gsp/bootloader-{version}.bin")

    # TU11x and GA100 use the same generic bootloader as TU10x
    symlink(f"../../tu102/gsp/gen_bootloader-{version}.bin", f"tu116/gsp/gen_bootloader-{version}.bin")
    symlink(f"../../tu102/gsp/gen_bootloader-{version}.bin", f"ga100/gsp/gen_bootloader-{version}.bin")

    # Blackwell is only supported with GSP, so we can symlink the top-level directories
    # instead of just the gsp/ subdirectories.
    for d in ['gb102']:
        symlink('gb100', d, target_is_directory = True)

    for d in ['gb203', 'gb205', 'gb206', 'gb207']:
        symlink('gb202', d, target_is_directory = True)

    # Symlink the GSP-RM image
    symlink(f"../../tu102/gsp/gsp-{version}.bin", f"tu116/gsp/gsp-{version}.bin")
    symlink(f"../../tu102/gsp/gsp-{version}.bin", f"ga100/gsp/gsp-{version}.bin")
    symlink(f"../../ga102/gsp/gsp-{version}.bin", f"ad102/gsp/gsp-{version}.bin")
    symlink(f"../../ga102/gsp/gsp-{version}.bin", f"gh100/gsp/gsp-{version}.bin")
    symlink(f"../../ga102/gsp/gsp-{version}.bin", f"gb100/gsp/gsp-{version}.bin")
    symlink(f"../../ga102/gsp/gsp-{version}.bin", f"gb202/gsp/gsp-{version}.bin")

    # Symlink the ucodes binaries
    if os.path.exists(f"tu102/gsp/ucodes-{version}.bin"):
        symlink(f"../../tu102/gsp/ucodes-{version}.bin", f"tu116/gsp/ucodes-{version}.bin")
        symlink(f"../../tu102/gsp/ucodes-{version}.bin", f"ga100/gsp/ucodes-{version}.bin")
    if os.path.exists(f"ga102/gsp/ucodes-{version}.bin"):
        symlink(f"../../ga102/gsp/ucodes-{version}.bin", f"ad102/gsp/ucodes-{version}.bin")
        symlink(f"../../ga102/gsp/ucodes-{version}.bin", f"gh100/gsp/ucodes-{version}.bin")
        symlink(f"../../ga102/gsp/ucodes-{version}.bin", f"gb100/gsp/ucodes-{version}.bin")
        symlink(f"../../ga102/gsp/ucodes-{version}.bin", f"gb202/gsp/ucodes-{version}.bin")

# Create a text file that can be inserted as-is to the WHENCE file of the
# linux-firmware git repository.  Note that existing firmware versions in
# the repository must be maintained, so those entries are hard-coded here.
# Also note that Nouveau supports Ada and later only with GSP, which is why
# ga103/gsp -> ga102/gsp, but ad103 -> ad102.
#
# Some hard rules for the layout of files:
#  1. No file of any version can symlink to a file of a different version,
#     even if the files are identical.  This allows distros to ship each version
#     independently.
#  2. All files must be located in the /gsp/ subdirectory of the GPU directory,
#     and there must be no symlinks to any files outside of the /gsp/ directory.
#     This allows the Nova driver to find all of the files it needs inside
#     the /gsp/ directory.
#  3. The WHENCE file should list each version in a separate block.  This wasn't
#     enforced in early versions of the WHENCE file.
#  4. Replacing a file/directory with a symlink (or vice versa) is strongly
#     discouraged.  Many distros cannot handle this transition.
#  5. The r535 release is deprecated, so no changes should be made to it.
def whence(gsp_origin = None):
    global outputpath
    global version

    if gsp_origin is None:
        gsp_origin = f"NVIDIA-Linux-x86_64-{version}.run"
    versions = [ "535.113.01", "570.144" ]
    whence = []

    whence.append(f"""
File: nvidia/tu102/gsp/bootloader-535.113.01.bin
File: nvidia/tu102/gsp/booter_load-535.113.01.bin
File: nvidia/tu102/gsp/booter_unload-535.113.01.bin
File: nvidia/tu116/gsp/booter_load-535.113.01.bin
File: nvidia/tu116/gsp/booter_unload-535.113.01.bin
Link: nvidia/tu116/gsp/bootloader-535.113.01.bin -> ../../tu102/gsp/bootloader-535.113.01.bin
File: nvidia/ga100/gsp/bootloader-535.113.01.bin
File: nvidia/ga100/gsp/booter_load-535.113.01.bin
File: nvidia/ga100/gsp/booter_unload-535.113.01.bin
File: nvidia/ga102/gsp/bootloader-535.113.01.bin
File: nvidia/ga102/gsp/booter_load-535.113.01.bin
File: nvidia/ga102/gsp/booter_unload-535.113.01.bin
File: nvidia/ad102/gsp/bootloader-535.113.01.bin
File: nvidia/ad102/gsp/booter_load-535.113.01.bin
File: nvidia/ad102/gsp/booter_unload-535.113.01.bin

File: nvidia/tu102/gsp/gsp-535.113.01.bin
Origin: gsp_tu10x.bin from NVIDIA-Linux-x86_64-535.113.01.run
Link: nvidia/tu116/gsp/gsp-535.113.01.bin -> ../../tu102/gsp/gsp-535.113.01.bin
Link: nvidia/ga100/gsp/gsp-535.113.01.bin -> ../../tu102/gsp/gsp-535.113.01.bin

File: nvidia/ga102/gsp/gsp-535.113.01.bin
Origin: gsp_ga10x.bin from NVIDIA-Linux-x86_64-535.113.01.run
Link: nvidia/ad102/gsp/gsp-535.113.01.bin -> ../../ga102/gsp/gsp-535.113.01.bin
""")

    whence.append(f"""
File: nvidia/tu102/gsp/gen_bootloader-570.144.bin
File: nvidia/tu102/gsp/bootloader-570.144.bin
File: nvidia/tu102/gsp/booter_load-570.144.bin
File: nvidia/tu102/gsp/booter_unload-570.144.bin
Link: nvidia/tu116/gsp/gen_bootloader-570.144.bin -> ../../tu102/gsp/gen_bootloader-570.144.bin
File: nvidia/tu116/gsp/booter_load-570.144.bin
File: nvidia/tu116/gsp/booter_unload-570.144.bin
Link: nvidia/tu116/gsp/bootloader-570.144.bin -> ../../tu102/gsp/bootloader-570.144.bin
Link: nvidia/ga100/gsp/gen_bootloader-570.144.bin -> ../../tu102/gsp/gen_bootloader-570.144.bin
File: nvidia/ga100/gsp/bootloader-570.144.bin
File: nvidia/ga100/gsp/booter_load-570.144.bin
File: nvidia/ga100/gsp/booter_unload-570.144.bin
File: nvidia/ga102/gsp/bootloader-570.144.bin
File: nvidia/ga102/gsp/booter_load-570.144.bin
File: nvidia/ga102/gsp/booter_unload-570.144.bin
File: nvidia/ad102/gsp/bootloader-570.144.bin
File: nvidia/ad102/gsp/booter_load-570.144.bin
File: nvidia/ad102/gsp/booter_unload-570.144.bin
File: nvidia/ad102/gsp/scrubber-570.144.bin
File: nvidia/gh100/gsp/bootloader-570.144.bin
File: nvidia/gh100/gsp/fmc-570.144.bin
File: nvidia/gb100/gsp/bootloader-570.144.bin
File: nvidia/gb100/gsp/fmc-570.144.bin
File: nvidia/gb202/gsp/bootloader-570.144.bin
File: nvidia/gb202/gsp/fmc-570.144.bin

File: nvidia/tu102/gsp/gsp-570.144.bin
Origin: gsp_tu10x.bin from NVIDIA-Linux-x86_64-570.144.run
Link: nvidia/tu116/gsp/gsp-570.144.bin -> ../../tu102/gsp/gsp-570.144.bin
Link: nvidia/ga100/gsp/gsp-570.144.bin -> ../../tu102/gsp/gsp-570.144.bin

File: nvidia/ga102/gsp/gsp-570.144.bin
Origin: gsp_ga10x.bin from NVIDIA-Linux-x86_64-570.144.run
Link: nvidia/ad102/gsp/gsp-570.144.bin -> ../../ga102/gsp/gsp-570.144.bin
Link: nvidia/gh100/gsp/gsp-570.144.bin -> ../../ga102/gsp/gsp-570.144.bin
Link: nvidia/gb100/gsp/gsp-570.144.bin -> ../../ga102/gsp/gsp-570.144.bin
Link: nvidia/gb202/gsp/gsp-570.144.bin -> ../../ga102/gsp/gsp-570.144.bin
""")

    if not version in versions:
        whence.append(f"""
File: nvidia/tu102/gsp/gen_bootloader-{version}.bin
File: nvidia/tu102/gsp/bootloader-{version}.bin
File: nvidia/tu102/gsp/booter_load-{version}.bin
File: nvidia/tu102/gsp/booter_unload-{version}.bin
File: nvidia/tu116/gsp/booter_load-{version}.bin
File: nvidia/tu116/gsp/booter_unload-{version}.bin
Link: nvidia/tu116/gsp/gen_bootloader-{version}.bin -> ../../tu102/gsp/gen_bootloader-{version}.bin
Link: nvidia/tu116/gsp/bootloader-{version}.bin -> ../../tu102/gsp/bootloader-{version}.bin
Link: nvidia/ga100/gsp/gen_bootloader-{version}.bin -> ../../tu102/gsp/gen_bootloader-{version}.bin
File: nvidia/ga100/gsp/bootloader-{version}.bin
File: nvidia/ga100/gsp/booter_load-{version}.bin
File: nvidia/ga100/gsp/booter_unload-{version}.bin
File: nvidia/ga102/gsp/bootloader-{version}.bin
File: nvidia/ga102/gsp/booter_load-{version}.bin
File: nvidia/ga102/gsp/booter_unload-{version}.bin
File: nvidia/ad102/gsp/bootloader-{version}.bin
File: nvidia/ad102/gsp/booter_load-{version}.bin
File: nvidia/ad102/gsp/booter_unload-{version}.bin
File: nvidia/ad102/gsp/scrubber-{version}.bin
File: nvidia/gh100/gsp/bootloader-{version}.bin
File: nvidia/gh100/gsp/fmc-{version}.bin
File: nvidia/gb100/gsp/bootloader-{version}.bin
File: nvidia/gb100/gsp/fmc-{version}.bin
File: nvidia/gb202/gsp/bootloader-{version}.bin
File: nvidia/gb202/gsp/fmc-{version}.bin

File: nvidia/tu102/gsp/gsp-{version}.bin
Origin: gsp_tu10x.bin from {gsp_origin}
Link: nvidia/tu116/gsp/gsp-{version}.bin -> ../../tu102/gsp/gsp-{version}.bin
Link: nvidia/ga100/gsp/gsp-{version}.bin -> ../../tu102/gsp/gsp-{version}.bin

File: nvidia/ga102/gsp/gsp-{version}.bin
Origin: gsp_ga10x.bin from {gsp_origin}
Link: nvidia/ad102/gsp/gsp-{version}.bin -> ../../ga102/gsp/gsp-{version}.bin
Link: nvidia/gh100/gsp/gsp-{version}.bin -> ../../ga102/gsp/gsp-{version}.bin
Link: nvidia/gb100/gsp/gsp-{version}.bin -> ../../ga102/gsp/gsp-{version}.bin
Link: nvidia/gb202/gsp/gsp-{version}.bin -> ../../ga102/gsp/gsp-{version}.bin
""")

    if os.path.exists(f"{outputpath}/nvidia/tu102/gsp/ucodes-{version}.bin"):
        whence.append(f"""
File: nvidia/tu102/gsp/ucodes-{version}.bin
Origin: ucodes_tu10x.bin from {gsp_origin}
Link: nvidia/tu116/gsp/ucodes-{version}.bin -> ../../tu102/gsp/ucodes-{version}.bin
Link: nvidia/ga100/gsp/ucodes-{version}.bin -> ../../tu102/gsp/ucodes-{version}.bin
""")

    if os.path.exists(f"{outputpath}/nvidia/ga102/gsp/ucodes-{version}.bin"):
        whence.append(f"""
File: nvidia/ga102/gsp/ucodes-{version}.bin
Origin: ucodes_ga10x.bin from {gsp_origin}
Link: nvidia/ad102/gsp/ucodes-{version}.bin -> ../../ga102/gsp/ucodes-{version}.bin
Link: nvidia/gh100/gsp/ucodes-{version}.bin -> ../../ga102/gsp/ucodes-{version}.bin
Link: nvidia/gb100/gsp/ucodes-{version}.bin -> ../../ga102/gsp/ucodes-{version}.bin
Link: nvidia/gb202/gsp/ucodes-{version}.bin -> ../../ga102/gsp/ucodes-{version}.bin
""")

    # Symlinks for GPUs that share all images
    whence.append(f"""
Link: nvidia/tu104/gsp -> ../tu102/gsp
Link: nvidia/tu106/gsp -> ../tu102/gsp
Link: nvidia/tu117/gsp -> ../tu116/gsp
Link: nvidia/ga103/gsp -> ../ga102/gsp
Link: nvidia/ga104/gsp -> ../ga102/gsp
Link: nvidia/ga106/gsp -> ../ga102/gsp
Link: nvidia/ga107/gsp -> ../ga102/gsp
Link: nvidia/ad103 -> ad102
Link: nvidia/ad104 -> ad102
Link: nvidia/ad106 -> ad102
Link: nvidia/ad107 -> ad102
Link: nvidia/gb102 -> gb100
Link: nvidia/gb203 -> gb202
Link: nvidia/gb205 -> gb202
Link: nvidia/gb206 -> gb202
Link: nvidia/gb207 -> gb202
""")

    with open(f"{outputpath}/WHENCE.txt", 'w') as f:
        f.writelines(whence)

    print(f"Created {outputpath}/WHENCE.txt")

def main():
    global outputpath
    global version

    parser = argparse.ArgumentParser(
        description = 'Extract firmware binaries from the OpenRM git repository'
        ' in a format expected by the Nouveau/Nova device drivers.',
        epilog = 'Running as root and specifying -o /lib/firmware will install'
        ' the firmware files directly where Nouveau and Nova expects them.'
        ' The --revision option is useful for testing new firmware'
        ' versions without changing Nouveau/Nova source code.'
        ' The --driver option accepts a .run file path, a URL, or a local'
        ' build output directory.  If -d is given with no argument, the .run'
        ' file is downloaded automatically.'
        ' --elf32 and --elf64 are mutually exclusive.')
    parser.add_argument('-i', '--input', default = os.getcwd(),
        help = 'Path to source directory (where version.mk exists)')
    parser.add_argument('-o', '--output', default = os.path.join(os.getcwd(), '_out'),
        help = 'Path to target directory (where files will be written)')
    parser.add_argument('-r', '--revision',
        help = 'Files will be named with this version number')
    parser.add_argument('--debug-fused', action='store_true',
        help = 'Extract debug instead of production images')
    parser.add_argument('-d', '--driver',
        nargs = '?', const = '',
        help = 'Also extract GSP-RM firmware from a source.'
        ' A URL or path to a .run driver package downloads or extracts it.'
        ' A path to a local build output directory (e.g.'
        ' drivers/resman/build/gsp/_out/Linux_amd64_release) copies'
        ' the GSP firmware directly.  If -d is given with no argument,'
        ' the .run file is downloaded automatically.')
    parser.add_argument('-s', '--symlink', action='store_true',
        help = 'Also create symlinks for all supported GPUs')
    parser.add_argument('-w', '--whence', action='store_true',
        help = 'Also generate a WHENCE file')

    elf_group = parser.add_mutually_exclusive_group()
    elf_group.add_argument('--elf32', action='store_false', dest='elf64', default=True,
        help = 'Generate Nouveau-compatible 32-bit ELF images for FMC')
    elf_group.add_argument('--elf64', action='store_true', dest='elf64',
        help = 'Generate Nova-compatible 64-bit ELF images for FMC (default)')

    args = parser.parse_args()

    args.output = os.path.abspath(args.output)
    if args.driver is not None and args.driver != '' and not re.search('^http[s]://', args.driver):
        args.driver = os.path.abspath(args.driver)

    args.input = os.path.abspath(args.input)
    os.chdir(args.input)

    if not os.path.isfile("version.mk"):
        raise MyException(f"Source directory {args.input} is incorrect")

    version = args.revision
    if not version:
        with open("version.mk") as f:
            version = re.search(r'^NVIDIA_VERSION = ([^\s]+)', f.read(), re.MULTILINE).group(1)
        del f

    print(f"Generating files for version {version}")

    outputpath = args.output;
    print(f"Writing files to {outputpath}")

    os.makedirs(f"{outputpath}/nvidia", exist_ok = True)

    # TU10x and GA100 do not have debug-fused versions of the GSP bootloader
    if args.debug_fused:
        print("Generating images for debug-fused GPUs")
        fuse = "dbg"
        fmc_fuse = "Debug"
    else:
        fuse = "prod"
        fmc_fuse = "Prod"

    # The generic bootloader is only defined for TU102 but is used
    # by all TU1xx and GA100.
    generic_bootloader("tu102")

    booter("tu102", "load", 16, fuse)
    booter("tu102", "unload", 16, fuse)
    gsp_bootloader("tu102")

    booter("tu116", "load", 16, fuse)
    booter("tu116", "unload", 16, fuse)
    # TU11x uses the same bootloader as TU10x

    booter("ga100", "load", 384, fuse)
    booter("ga100", "unload", 384, fuse)
    gsp_bootloader("ga100")

    booter("ga102", "load", 384, fuse)
    booter("ga102", "unload", 384, fuse)
    gsp_bootloader("ga102", fuse)

    booter("ad102", "load", 384, fuse)
    booter("ad102", "unload", 384, fuse)
    gsp_bootloader("ad102", fuse)
    scrubber("ad102", 384, fuse) # Not currently used by Nouveau

    gsp_bootloader("gh100", fuse)
    fmc("gh100", fmc_fuse, args.elf64)

    gsp_bootloader("gb100", fuse)
    fmc("gb100", fmc_fuse, args.elf64)

    gsp_bootloader("gb202", fuse)
    fmc("gb202", fmc_fuse, args.elf64)

    gsp_origin = None

    if args.driver is not None:
        if args.driver == '':
            # No path/url provided, so make a guess of the URL
            # to automatically download the right version.
            args.driver = f'https://download.nvidia.com/XFree86/Linux-x86_64/{version}/NVIDIA-Linux-x86_64-{version}.run'

        if re.search('^http[s]://', args.driver):
            with tempfile.NamedTemporaryFile(prefix = f'NVIDIA-Linux-x86_64-{version}-', suffix = '.run') as f:
                print(f"Downloading driver from {args.driver} as {f.name}")
                urllib.request.urlretrieve(args.driver, f.name)
                gsp_firmware(f.name)
            del f
        elif os.path.isdir(args.driver):
            gsp_firmware_from_build(args.driver)
            gsp_origin = f"local build ({args.driver})"
        else:
            if not os.path.exists(args.driver):
                raise MyException(f"File {args.driver} does not exist.")

            gsp_firmware(args.driver)

    if args.symlink:
        symlinks()

    if args.whence:
        whence(gsp_origin)

if __name__ == "__main__":
    try:
        main()
    except MyException as e:
        # The full stack trace is too noisy with MyException
        print(f"Error: {e}", file=sys.stderr)
        sys.exit(1)
