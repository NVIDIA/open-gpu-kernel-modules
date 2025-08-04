#!/usr/bin/env python3

# SPDX-FileCopyrightText: Copyright (c) 2023-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
import re
import gzip
import struct
import zlib
import tempfile
import urllib.request

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
            m = re.search(r"DATA SIZE \(bytes\): (\d+)", line)
            if m:
                data_size = int(m.group(1))
            m = re.search(r"COMPRESSED SIZE \(bytes\): (\d+)", line)
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
            raise MyException(f"array {array} in {filename} should be {data_size} bytes but is actually {len(output)}.")
        return output

# GSP bootloader
def bootloader(gpu, fuse):
    global outputpath
    global version

    GPU = gpu.upper()
    filename = f"src/nvidia/generated/g_bindata_kgspGetBinArchiveGspRmBoot_{GPU}.c"

    print(f"Creating nvidia/{gpu}/gsp/bootloader-{version}.bin")
    os.makedirs(f"{outputpath}/nvidia/{gpu}/gsp/", exist_ok = True)

    with open(f"{outputpath}/nvidia/{gpu}/gsp/bootloader-{version}.bin", "wb") as f:
        # Extract the actual bootloader firmware
        array = f"kgspBinArchiveGspRmBoot_{GPU}_ucode_image{fuse}data"
        firmware = getbytes(filename, array)
        firmware_size = len(firmware)

        # Extract the descriptor (RM_RISCV_UCODE_DESC)
        array = f"kgspBinArchiveGspRmBoot_{GPU}_ucode_desc{fuse}data"
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
def booter(gpu, load, sigsize, fuse = "prod"):
    global outputpath
    global version

    GPU = gpu.upper()
    LOAD = load.capitalize()

    filename = f"src/nvidia/generated/g_bindata_kgspGetBinArchiveBooter{LOAD}Ucode_{GPU}.c"

    print(f"Creating nvidia/{gpu}/gsp/booter_{load}-{version}.bin")
    os.makedirs(f"{outputpath}/nvidia/{gpu}/gsp/", exist_ok = True)

    with open(f"{outputpath}/nvidia/{gpu}/gsp/booter_{load}-{version}.bin", "wb") as f:
        # Extract the actual booter firmware
        array = f"kgspBinArchiveBooter{LOAD}Ucode_{GPU}_image_{fuse}_data"
        firmware = getbytes(filename, array)
        firmware_size = len(firmware)

        # Extract the signatures
        array = f"kgspBinArchiveBooter{LOAD}Ucode_{GPU}_sig_{fuse}_data"
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
        array = f"kgspBinArchiveBooter{LOAD}Ucode_{GPU}_header_{fuse}_data"
        descriptor = getbytes(filename, array)

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

    filename = f"src/nvidia/generated/g_bindata_ksec2GetBinArchiveSecurescrubUcode_{GPUX}.c"

    print(f"Creating nvidia/{gpu}/gsp/scrubber-{version}.bin")
    os.makedirs(f"{outputpath}/nvidia/{gpu}/gsp/", exist_ok = True)

    with open(f"{outputpath}/nvidia/{gpu}/gsp/scrubber-{version}.bin", "wb") as f:
        # Extract the actual scrubber firmware
        array = f"ksec2BinArchiveSecurescrubUcode_{GPUX}_image_{fuse}_data[]"
        firmware = getbytes(filename, array)
        firmware_size = len(firmware)

        # Extract the signatures
        array = f"ksec2BinArchiveSecurescrubUcode_{GPUX}_sig_{fuse}_data"
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
        array = f"ksec2BinArchiveSecurescrubUcode_{GPUX}_header_{fuse}_data"
        descriptor = getbytes(filename, array)

        # Fifth, the descriptor
        f.write(descriptor)

        # And finally, the actual scrubber image
        f.write(firmware)

ELF_HDR_SIZE = 52
ELF_SHDR_SIZE = 40
ELF_ALIGNMENT = 4

# Create a 32-bit generic ELF header with no program header, and 'shnum'
# section headers, not including the .shstrtab and NULL sections.
# The section headers appear after the ELF header, and the section data
# follows.  Note that e_shstrndx cannot be zero, because that implies
# that the .shstrndx section does not exist.
def elf_header(shnum: int):
    bytes = struct.pack("<B3s5B7xHH5I6H",
        0x7f, b'ELF',
        1, 1, 1, 0, 0, # EI_CLASS, EI_DATA, EI_VERSION, EI_OSABI, EI_ABIVERSION
        0, 0, 1, # e_type, e_machine, e_version
        0, 0, ELF_HDR_SIZE, 0, # e_entry, e_phoff, e_shoff, e_flags
        ELF_HDR_SIZE, 0, 0, # e_ehsize, e_phentsize, e_phnum
        ELF_SHDR_SIZE, shnum + 2, 1) # e_shentsize, e_shnum, e_shstrndx

    return bytes

# Create a 32-bit ELF section header, where 'sh_name' is the offset of the
# section name, 'sh_offset' is the offset of the section data, and 'sh_size'
# is the size (in bytes) of the image in the section data.
# We set sh_flags to SHF_OS_NONCONFORMING and use the sh_info field to store
# a 32-bit CRC of the image data.
def elf_section_header(sh_name, sh_offset, sh_size, sh_info):
    bytes = struct.pack("<10I",
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
def elf_section_header_null():
    return b'\0' * ELF_SHDR_SIZE

# Create a 64-bit .shstrtab ELF section header.
# 'shnum' is the number of sections.
# 'sh_offset' is the offset of the .shstrtab section.
# 'sh_size' is the unpadded size of the section.
# The section itself should be padded to the nearest 8-byte boundary, so that
# all the sections are aligned.
def elf_section_header_shstrtab(sh_name, shnum, sh_size):
    sh_offset = ELF_HDR_SIZE + ELF_SHDR_SIZE * (shnum + 2);

    bytes = struct.pack("<10I",
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

# Returns a tuple of the size of a bytearray and the size rounded up to the next 8
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
def fmc(gpu, fuse = "Prod"):
    global outputpath
    global version

    GPU=gpu.upper()
    filename = f"src/nvidia/generated/g_bindata_kgspGetBinArchiveGspRmFmcGfw{fuse}Signed_{GPU}.c"

    print(f"Creating nvidia/{gpu}/gsp/fmc-{version}.bin")
    os.makedirs(f"{outputpath}/nvidia/{gpu}/gsp/", exist_ok = True)

    array = f"kgspBinArchiveGspRmFmcGfw{fuse}Signed_{GPU}_ucode_hash_data"
    ucode_hash = getbytes(filename, array)
    (ucode_hash_size, ucode_hash_padded_size) = sizes(ucode_hash)

    array = f"kgspBinArchiveGspRmFmcGfw{fuse}Signed_{GPU}_ucode_sig_data"
    ucode_sig = getbytes(filename, array)
    (ucode_sig_size, ucode_sig_padded_size) = sizes(ucode_sig)

    array = f"kgspBinArchiveGspRmFmcGfw{fuse}Signed_{GPU}_ucode_pkey_data"
    ucode_pkey = getbytes(filename, array)
    (ucode_pkey_size, ucode_pkey_padded_size) = sizes(ucode_pkey)

    array = f"kgspBinArchiveGspRmFmcGfw{fuse}Signed_{GPU}_ucode_image_data"
    ucode_image = getbytes(filename, array)
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
        header = elf_header(shnum)
        f.write(header)

        # Add the section headers

        header = elf_section_header_null()
        f.write(header)

        header = elf_section_header_shstrtab(offset_of(".shstrtab", shstrtab), shnum, len(shstrtab))
        f.write(header)

        header = elf_section_header(offset_of("hash", shstrtab),
            hash_offset, ucode_hash_size, zlib.crc32(ucode_hash))
        f.write(header)

        header = elf_section_header(offset_of("signature", shstrtab),
            signature_offset, ucode_sig_size, zlib.crc32(ucode_sig))
        f.write(header)

        header = elf_section_header(offset_of("publickey", shstrtab),
            pkey_offset, ucode_pkey_size, zlib.crc32(ucode_pkey))
        f.write(header)

        header = elf_section_header(offset_of("image", shstrtab),
            image_offset, ucode_image_size, zlib.crc32(ucode_image))
        f.write(header)

        # Make sure we're where we are supposed to be
        assert f.tell() == ELF_HDR_SIZE + ELF_SHDR_SIZE * (shnum + 2)

        # Write the .shstrtab section data.
        write_padded(f, shstrtab)
        assert f.tell() % 4 == 0

        # Finally, write the four images in sequence
        write_padded(f, ucode_hash)
        assert f.tell() % 4 == 0

        write_padded(f, ucode_sig)
        assert f.tell() % 4 == 0

        write_padded(f, ucode_pkey)
        assert f.tell() % 4 == 0

        write_padded(f, ucode_image)
        assert f.tell() % 4 == 0

# Extract the GSP-RM firmware from the .run file and copy the binaries
# to the target directory.
def gsp_firmware(filename):
    global outputpath
    global version

    import subprocess
    import shutil
    import time

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
            print(error.output.decode())
            raise

        if not os.path.exists('gsp_tu10x.bin') or not os.path.exists('gsp_ga10x.bin'):
            print("Firmware files are missing")
            sys.exit(1)

        shutil.copyfile('gsp_tu10x.bin', f"{outputpath}/nvidia/tu102/gsp/gsp-{version}.bin")
        print(f"Copied gsp_tu10x.bin to tu102/gsp/gsp-{version}.bin")
        shutil.copyfile('gsp_ga10x.bin', f"{outputpath}/nvidia/ga102/gsp/gsp-{version}.bin")
        print(f"Copied gsp_ga10x.bin to ga102/gsp/gsp-{version}.bin")

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

    for d in ['tu104', 'tu106', 'tu117']:
        os.makedirs(d, exist_ok = True)
        symlink('../tu102/gsp', f"{d}/gsp", target_is_directory = True)

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

    # TU11x uses the same bootloader as TU10x
    symlink(f"../../tu102/gsp/bootloader-{version}.bin", f"tu116/gsp/bootloader-{version}.bin")

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

# Create a text file that can be inserted as-is to the WHENCE file of the
# linux-firmware git repository.  Note that existing firmware versions in
# the repository must be maintained, so those entries are hard-coded here.
# Also note that Nouveau supports Ada and later only with GSP, which is why
# ga103/gsp -> ga102/gsp, but ad103 -> ad102.
def whence():
    global outputpath
    global version

    whence = f"""File: nvidia/tu102/gsp/bootloader-535.113.01.bin
File: nvidia/tu102/gsp/booter_load-535.113.01.bin
File: nvidia/tu102/gsp/booter_unload-535.113.01.bin
File: nvidia/tu102/gsp/bootloader-{version}.bin
File: nvidia/tu102/gsp/booter_load-{version}.bin
File: nvidia/tu102/gsp/booter_unload-{version}.bin
Link: nvidia/tu104/gsp -> ../tu102/gsp
Link: nvidia/tu106/gsp -> ../tu102/gsp

File: nvidia/tu116/gsp/booter_load-535.113.01.bin
File: nvidia/tu116/gsp/booter_unload-535.113.01.bin
Link: nvidia/tu116/gsp/bootloader-535.113.01.bin -> ../../tu102/gsp/bootloader-535.113.01.bin
File: nvidia/tu116/gsp/booter_load-{version}.bin
File: nvidia/tu116/gsp/booter_unload-{version}.bin
Link: nvidia/tu116/gsp/bootloader-{version}.bin -> ../../tu102/gsp/bootloader-{version}.bin
Link: nvidia/tu117/gsp -> ../tu116/gsp

File: nvidia/ga100/gsp/bootloader-535.113.01.bin
File: nvidia/ga100/gsp/booter_load-535.113.01.bin
File: nvidia/ga100/gsp/booter_unload-535.113.01.bin
File: nvidia/ga100/gsp/bootloader-{version}.bin
File: nvidia/ga100/gsp/booter_load-{version}.bin
File: nvidia/ga100/gsp/booter_unload-{version}.bin

File: nvidia/ga102/gsp/bootloader-535.113.01.bin
File: nvidia/ga102/gsp/booter_load-535.113.01.bin
File: nvidia/ga102/gsp/booter_unload-535.113.01.bin
File: nvidia/ga102/gsp/bootloader-{version}.bin
File: nvidia/ga102/gsp/booter_load-{version}.bin
File: nvidia/ga102/gsp/booter_unload-{version}.bin
Link: nvidia/ga103/gsp -> ../ga102/gsp
Link: nvidia/ga104/gsp -> ../ga102/gsp
Link: nvidia/ga106/gsp -> ../ga102/gsp
Link: nvidia/ga107/gsp -> ../ga102/gsp

File: nvidia/ad102/gsp/bootloader-535.113.01.bin
File: nvidia/ad102/gsp/booter_load-535.113.01.bin
File: nvidia/ad102/gsp/booter_unload-535.113.01.bin
File: nvidia/ad102/gsp/bootloader-{version}.bin
File: nvidia/ad102/gsp/booter_load-{version}.bin
File: nvidia/ad102/gsp/booter_unload-{version}.bin
File: nvidia/ad102/gsp/scrubber-{version}.bin
Link: nvidia/ad103 -> ad102
Link: nvidia/ad104 -> ad102
Link: nvidia/ad106 -> ad102
Link: nvidia/ad107 -> ad102

File: nvidia/gh100/gsp/bootloader-{version}.bin
File: nvidia/gh100/gsp/fmc-{version}.bin

File: nvidia/gb100/gsp/bootloader-{version}.bin
File: nvidia/gb100/gsp/fmc-{version}.bin
Link: nvidia/gb102 -> gb100

File: nvidia/gb202/gsp/bootloader-{version}.bin
File: nvidia/gb202/gsp/fmc-{version}.bin
Link: nvidia/gb203 -> gb202
Link: nvidia/gb205 -> gb202
Link: nvidia/gb206 -> gb202
Link: nvidia/gb207 -> gb202

File: nvidia/tu102/gsp/gsp-535.113.01.bin
Origin: gsp_tu10x.bin from NVIDIA-Linux-x86_64-535.113.01.run
Link: nvidia/tu116/gsp/gsp-535.113.01.bin -> ../../tu102/gsp/gsp-535.113.01.bin
Link: nvidia/ga100/gsp/gsp-535.113.01.bin -> ../../tu102/gsp/gsp-535.113.01.bin

File: nvidia/ga102/gsp/gsp-535.113.01.bin
Origin: gsp_ga10x.bin from NVIDIA-Linux-x86_64-535.113.01.run
Link: nvidia/ad102/gsp/gsp-535.113.01.bin -> ../../ga102/gsp/gsp-535.113.01.bin

File: nvidia/tu102/gsp/gsp-{version}.bin
Origin: gsp_tu10x.bin from NVIDIA-Linux-x86_64-{version}.run
Link: nvidia/tu116/gsp/gsp-{version}.bin -> ../../tu102/gsp/gsp-{version}.bin
Link: nvidia/ga100/gsp/gsp-{version}.bin -> ../../tu102/gsp/gsp-{version}.bin

File: nvidia/ga102/gsp/gsp-{version}.bin
Origin: gsp_ga10x.bin from NVIDIA-Linux-x86_64-{version}.run
Link: nvidia/ad102/gsp/gsp-{version}.bin -> ../../ga102/gsp/gsp-{version}.bin
Link: nvidia/gh100/gsp/gsp-{version}.bin -> ../../ga102/gsp/gsp-{version}.bin
Link: nvidia/gb100/gsp/gsp-{version}.bin -> ../../ga102/gsp/gsp-{version}.bin
Link: nvidia/gb202/gsp/gsp-{version}.bin -> ../../ga102/gsp/gsp-{version}.bin
"""

    with open(f"{outputpath}/WHENCE.txt", 'w') as f:
        f.writelines(whence)

    print(f"Created {outputpath}/WHENCE.txt")

def main():
    global outputpath
    global version

    parser = argparse.ArgumentParser(
        description = 'Extract firmware binaries from the OpenRM git repository'
        ' in a format expected by the Nouveau device driver.',
        epilog = 'Running as root and specifying -o /lib/firmware will install'
        ' the firmware files directly where Nouveau expects them.'
        ' The --revision option is useful for testing new firmware'
        ' versions without changing Nouveau source code.'
        ' The --driver option lets you specify the local path to the .run file,'
        ' or the URL of a file to download, and this script also will extract'
        ' and copy the GSP-RM firmware images.  If no path/url is provided, then'
        ' the script will guess the URL and download the file automatically.')
    parser.add_argument('-i', '--input', default = os.getcwd(),
        help = 'Path to source directory (where version.mk exists)')
    parser.add_argument('-o', '--output', default = os.path.abspath(os.getcwd() + '/_out'),
        help = 'Path to target directory (where files will be written)')
    parser.add_argument('-r', '--revision',
        help = 'Files will be named with this version number')
    parser.add_argument('--debug-fused', action='store_true',
        help = 'Extract debug instead of production images')
    parser.add_argument('-d', '--driver',
        nargs = '?', const = '',
        help = 'Path or URL to NVIDIA-Linux-x86_64-<version>.run driver package, for also extracting the GSP-RM firmware')
    parser.add_argument('-s', '--symlink', action='store_true',
        help = 'Also create symlinks for all supported GPUs')
    parser.add_argument('-w', '--whence', action='store_true',
        help = 'Also generate a WHENCE file')
    args = parser.parse_args()

    os.chdir(args.input)

    version = args.revision
    if not version:
        with open("version.mk") as f:
            version = re.search(r'^NVIDIA_VERSION = ([^\s]+)', f.read(), re.MULTILINE).group(1)
        del f

    print(f"Generating files for version {version}")

    outputpath = args.output;
    print(f"Writing files to {outputpath}")

    os.makedirs(f"{outputpath}/nvidia", exist_ok = True)

    # TU10x and GA100 do not have debug-fused versions of the bootloader
    if args.debug_fused:
        print("Generation images for debug-fused GPUs")
        bootloader_fuse = "_dbg_"
        booter_fuse = "dbg" # Also used for scrubber
        fmc_fuse = "Debug"
    else:
        bootloader_fuse = "_prod_"
        booter_fuse = "prod"
        fmc_fuse = "Prod"

    booter("tu102", "load", 16, booter_fuse)
    booter("tu102", "unload", 16, booter_fuse)
    bootloader("tu102", "_")

    booter("tu116", "load", 16, booter_fuse)
    booter("tu116", "unload", 16, booter_fuse)
    # TU11x uses the same bootloader as TU10x

    booter("ga100", "load", 384, booter_fuse)
    booter("ga100", "unload", 384, booter_fuse)
    bootloader("ga100", "_")

    booter("ga102", "load", 384, booter_fuse)
    booter("ga102", "unload", 384, booter_fuse)
    bootloader("ga102", bootloader_fuse)

    booter("ad102", "load", 384, booter_fuse)
    booter("ad102", "unload", 384, booter_fuse)
    bootloader("ad102", bootloader_fuse)
    scrubber("ad102", 384, booter_fuse) # Not currently used by Nouveau

    bootloader("gh100", bootloader_fuse)
    fmc("gh100", fmc_fuse)

    bootloader("gb100", bootloader_fuse)
    fmc("gb100", fmc_fuse)

    bootloader("gb202", bootloader_fuse)
    fmc("gb202", fmc_fuse)

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
        else:
            if not os.path.exists(args.driver):
                print(f"File {args.driver} does not exist.")
                sys.exit(1)

            gsp_firmware(os.path.abspath(args.driver))

    if args.symlink:
        symlinks()

    if args.whence:
        whence()

if __name__ == "__main__":
    main()

