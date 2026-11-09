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

# Converts OpenRM binhex-encoded images to Nova-compatible binary blobs.

import sys
import os
import argparse
import re
import struct
import tempfile
import urllib.request
import shutil

# Locate the shared helper module relative to this script (not the current
# working directory), so that the script can be run from anywhere.
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from extract_firmware_common import (
    MyException,
    get_bytes,
    is_supported,
    symlink,
    extract_run_file,
)

# Newer firmware versions that Nova will support might have new files
# that conflict with the symlinks for older firmware versions that
# Nouveau supports.  To avoid these collisions, place all Nova images
# in a new subdirectory.
subdir = "gsp"

# -------------------------------------------------------------------
# Build tag-length-value (TLV) list
# -------------------------------------------------------------------

class TLV:
    def __init__(self, filename: str, gpu: str):
        self.filename = filename
        self.gpu = gpu
        self.entries = []

    def add(self, tag: str, value):
        filename = f"{self.gpu}/{subdir}/{self.filename}.tlv"

        if len(tag) != 4:
            raise MyException(f"TLV tag '{tag}' in {filename} must be exactly 4 characters")

        if any(t == tag for t, _ in self.entries):
            raise MyException(f"TLV tag '{tag}' in {filename} already exists")

        # Integers are a special case, as they have no "length" in Python
        if isinstance(value, int):
            # Reject negative numbers and integers larger than 64-bit
            if not (0 <= value <= 0xFFFFFFFFFFFFFFFF):
                raise MyException(f"TLV tag '{tag}' in {filename} integer value {value} out of range")
        else:
            if len(value) == 0:
                raise MyException(f"TLV tag '{tag}' in {filename} has no data")
            # We don't want non-ASCII strings anywhere
            if isinstance(value, str) and not value.isascii():
                raise MyException(f"TLV tag '{tag}' in {filename} value is a string but contains non-ASCII characters")

        self.entries.append((tag, value))

    def write(self):
        global outputpath
        global version

        # Add the version last so that it's not iterated over every time
        self.add("VERS", version)

        print(f"Creating nvidia/{self.gpu}/{subdir}/{self.filename}.tlv")
        os.makedirs(f"{outputpath}/nvidia/{self.gpu}/{subdir}/", exist_ok = True)

        with open(f"{outputpath}/nvidia/{self.gpu}/{subdir}/{self.filename}.tlv", "wb") as f:
            f.write(struct.pack('4s', b"NVFW"))

            for tag, value in self.entries:
                # Convert strings and integers into bytearrays
                if isinstance(value, str):
                    # TLV strings are not null-terminated.
                    value = value.encode('ascii')
                elif isinstance(value, int):
                    # Support 32-bit and 64-bit integers
                    value = struct.pack('<I' if value < 0x100000000 else '<Q', value)

                f.write(struct.pack('<4sI', tag.encode('ascii'), len(value)))
                f.write(value)
                # Add padding bytes if necessary
                f.write(b'\x00' * ((-len(value)) % 4))

# -------------------------------------------------------------------
# Read ELF images
# -------------------------------------------------------------------

class ELF64:
    EI_NIDENT = 16
    ELF_MAGIC = b'\x7fELF'
    ELFCLASS64 = 2

    def __init__(self, filename: str):
        self.filename = filename
        with open(filename, 'rb') as f:
            data = f.read()

        if len(data) < self.EI_NIDENT:
            raise MyException(f"{filename}: file too small to be an ELF image")

        if data[:4] != self.ELF_MAGIC:
            raise MyException(f"{filename}: not an ELF file")

        if data[4] != self.ELFCLASS64:
            raise MyException(f"{filename}: not a 64-bit ELF file")

        if data[5] != 1:
            raise MyException(f"{filename}: big-endian ELF files are not supported")

        (e_shoff, e_shentsize, e_shnum, e_shstrndx) = struct.unpack_from('<Q10xHHH', data, 0x28)

        shstrtab_off = struct.unpack_from('<24xQ', data,
            e_shoff + e_shstrndx * e_shentsize)[0]

        self.sections = []
        for i in range(e_shnum):
            sh = e_shoff + i * e_shentsize
            (sh_name, sh_offset, sh_size) = struct.unpack_from('<I20xQQ', data, sh)

            name_start = shstrtab_off + sh_name
            name_end = data.index(b'\x00', name_start)
            name = data[name_start:name_end].decode('ascii')

            # We only care about sections that start with ".fw"
            if name.startswith('.fw'):
                self.sections.append((name, data[sh_offset:sh_offset + sh_size]))

            # as well as the ".note.gnu.build-id" section
            if name == ".note.gnu.build-id":
                self.sections.append((name, data[sh_offset:sh_offset + sh_size]))

    def section(self, name: str) -> bytes:
        for sec_name, sec_data in self.sections:
            if sec_name == name:
                return sec_data
        raise MyException(f"ELF {self.filename} does not have a {name} section")

# -------------------------------------------------------------------
# Parse autogenerated NVOC Hal field tables
# -------------------------------------------------------------------

# RM autogenerates per-chip "Hal field" assignments in files such as g_gpu_arch_nvoc.c.
# Each field is initialized with an if / else-if / else chain, where every if-branch
# annotates its (possibly multi-line) condition with a "/* ChipHal: GPU1 | GPU2 | ... */"
# comment listing the GPUs the branch applies to, and the trailing else is a catch-all
# default.  This parses one such field and returns a dictionary mapping each GPU name (as
# it appears in the comment, e.g. "TU102") to the value expression assigned for that GPU.
# The catch-all default value is stored under the None key.

# Parse a C integer constant expression and return its value.  Handles plain constants,
# hex ("0x..."), and arbitrary arithmetic/bitwise expressions such as "((22U << 20) + (32U
# << 20) + ((48U << 10) * 2048U))".  Integer suffixes ("U", "L", "UL", "ULL",
# case-insensitive) are stripped, after which the C and Python syntax coincide, so the
# expression is simply evaluated by Python.
def parse_c_value(value: str) -> int | bool:
    # NVOC stores booleans as "((NvBool)(0 == 0))" (true) and
    # "((NvBool)(0 != 0))" (false).  Drop the (NvBool) cast so that what remains
    # ("((0 == 0))" / "((0 != 0))") is valid Python that evaluates to True/False.
    normalized = value.replace('(NvBool)', '')

    # Strip integer suffixes from each numeric literal (hex or decimal) so that
    # Python can parse them, e.g. "0x10U" -> "0x10", "2048U" -> "2048".
    normalized = re.sub(r'(0[xX][0-9a-fA-F]+|\d+)[uUlL]+', r'\1', normalized)
    return eval(normalized)

def parse_hal_field(filename: str, variable: str) -> dict:
    with open(filename) as f:
        text = f.read()

    # Locate the "// Hal field -- <variable>" marker that begins the block.
    marker = re.search(rf'^[ \t]*//[ \t]*Hal field --[ \t]*{variable}[ \t]*\n',
                       text, re.MULTILINE)
    if not marker:
        raise MyException(f"Could not find Hal field '{variable}' in {filename}")

    # The field is initialized by a single if / else-if / else statement that begins right
    # after the marker.  Walk its brace-delimited branch bodies to find where the
    # statement ends: the chain continues only while a closing brace is followed by
    # another "else".  Branch conditions contain only parentheses and bodies contain a
    # single assignment, so the only braces are the ones that delimit each body.
    pos = marker.end()
    while True:
        open_brace = text.find('{', pos)
        if open_brace == -1:
            raise MyException(f"Malformed Hal field '{variable}' in {filename}")
        # Branch bodies are never nested, so the body ends at the next '}'.
        close_brace = text.find('}', open_brace)
        if close_brace == -1:
            raise MyException(f"Malformed Hal field '{variable}' in {filename}")
        pos = close_brace + 1
        # Skip whitespace and "// ..." comments to see if another branch follows.
        if not re.match(r'(?:\s|//[^\n]*\n)*else\b', text[pos:]):
            break
    block = text[marker.end():pos]

    result = {}

    # Each if / else-if branch ends its condition with a "/* ChipHal: GPU1 | GPU2 | ... */"
    # comment, immediately followed by a "{ pThis-><variable> = <value>; }" body.
    # re.DOTALL lets both the GPU list and the value span multiple lines.
    branch = re.compile(
        rf'/\*\s*ChipHal:\s*(.*?)\s*\*/\s*'
        rf'\{{\s*pThis->{variable}\s*=\s*(.*?);\s*\}}',
        re.DOTALL)
    for gpus, value in branch.findall(block):
        for gpu in gpus.split('|'):
            result[gpu.strip().lower()] = parse_c_value(value)

    # The trailing "// default" / else branch is the catch-all.
    default = re.search(
        rf'//\s*default\s*else\s*\{{\s*pThis->{variable}\s*=\s*(.*?);\s*\}}',
        block, re.DOTALL)
    if default:
        result[None] = parse_c_value(default.group(1))

    return result

# -------------------------------------------------------------------
# Generate firmware binaries
# -------------------------------------------------------------------

# Generic Falcon bootloader.  First, FWSEC runs on the RISC-V GSP core.
# Then this generic bootloader runs on the SEC2 core, in order to restart the GSP
# core to run GSP-RM on it.  This is only used on TU10x and GA100 GPUs.
def generic_bootloader(gpu):
    GPU = gpu.upper()
    filename = f"src/nvidia/generated/g_bindata_ksec2GetBinArchiveBlUcode_{GPU}.c"

    tlv = TLV("gen_bootloader", gpu)

    # Extract the descriptor (RM_FLCN_BL_DESC)
    descriptor = get_bytes(filename, f"RM_FLCN_BL_DESC ksec2BinArchiveBlUcode_{GPU}", "ucode_desc")
    (start_tag, dmem_load_off, code_offset, code_size, data_off,
        data_size) = struct.unpack("<6I", descriptor)
    # Both RM and Nova only load the code section, and both assume that
    # code_off and dmem_load_off are zero.
    if code_offset != 0:
        raise MyException(f"code offset in ksec2BinArchiveBlUcode_{GPU} should be 0 but is {code_offset}")
    if dmem_load_off != 0:
        raise MyException(f"dmem load offset in ksec2BinArchiveBlUcode_{GPU} should be 0 but is {dmem_load_off}")
    # Ensure the start tag fits in the register
    if start_tag > 65535:
        raise MyException(f"start tag in ksec2BinArchiveBlUcode_{GPU} value of {start_tag} is too large")

    # Extract the actual bootloader firmware
    firmware = get_bytes(filename, f"ksec2BinArchiveBlUcode_{GPU}", "ucode_image")
    if code_offset + code_size > len(firmware):
        raise MyException(f"{code_offset=} + {code_size=} in ksec2BinArchiveBlUcode_{GPU} exceeds image size of {len(firmware)}")

    tlv.add("STRT", start_tag)
    tlv.add("CDSZ", code_size)

    tlv.add("BLOB", firmware)

    tlv.write()

# GSP bootloader
def gsp_bootloader(gpu: str, debug = None):
    if debug is not None:
        fuse = "_dbg" if debug else "_prod"
        name = f"gsp-bootloader-{gpu}-{'dbg' if debug else 'prod'}"
    else:
        fuse = ""
        name = f"gsp-bootloader-{gpu}"

    GPU = gpu.upper()
    filename = f"src/nvidia/generated/g_bindata_kgspGetBinArchiveGspRmBoot_{GPU}.c"

    tlv = TLV("gsp_bootloader", gpu)

    # Extract the descriptor (RM_RISCV_UCODE_DESC)
    # Note: the size of RM_RISCV_UCODE_DESC varies from version to version, but Nova
    # only cares about the first few fields.  So we use unpack_from() which ignores excess
    # bytes.
    descriptor = get_bytes(filename, f"kgspBinArchiveGspRmBoot_{GPU}", f"ucode_desc{fuse}")
    (desc_version, bootloader_offset, bootloader_size, bootloader_param_offset, bootloader_param_size,
     riscv_elf_offset, riscv_elf_size, app_version, manifest_offset, manifest_size, monitor_data_offset,
     monitor_data_size, monitor_code_offset, monitor_code_size) = struct.unpack_from("<14I", descriptor)

    if desc_version < 4:
        raise MyException(f"unsupported version {desc_version} for {name}")

    # Extract the actual bootloader firmware
    firmware = get_bytes(filename, f"kgspBinArchiveGspRmBoot_{GPU}", f"ucode_image{fuse}")

    # Validate a few of the offsets
    if manifest_offset + manifest_size > len(firmware):
        raise MyException(f"{manifest_offset=} + {manifest_size=} is too large for {name} (size={len(firmware)})")
    if monitor_data_offset + monitor_data_size > len(firmware):
        raise MyException(f"{monitor_data_offset=} + {monitor_data_size=} is too large for {name} (size={len(firmware)})")
    if monitor_code_offset + monitor_code_size > len(firmware):
        raise MyException(f"{monitor_code_offset=} + {monitor_code_size=} is too large for {name} (size={len(firmware)})")

    if manifest_offset % 256 != 0:
        raise MyException(f"{manifest_offset=} is not 256-byte aligned")
    if monitor_data_offset % 256 != 0:
        raise MyException(f"{monitor_data_offset=} is not 256-byte aligned")
    if monitor_code_offset % 256 != 0:
        raise MyException(f"{monitor_code_offset=} is not 256-byte aligned")

    tlv.add("CDOF", monitor_code_offset)
    tlv.add("DAOF", monitor_data_offset)
    tlv.add("MFOF", manifest_offset)
    tlv.add("APPV", app_version)

    tlv.add("BLOB", firmware)
    tlv.write()

# GSP Booter load and unload
def booter(gpu, load, sigsize, debug = False):
    fuse = "dbg" if debug else "prod"
    GPU = gpu.upper()
    LOAD = load.capitalize()
    name = f"booter-{load}-{gpu}-{fuse}"

    filename = f"src/nvidia/generated/g_bindata_kgspGetBinArchiveBooter{LOAD}Ucode_{GPU}.c"

    tlv = TLV(f"booter_{load}", gpu)

    # Query the number of signatures.  This should be a 4-byte array (32-bit little-endian integer)
    raw = get_bytes(filename, f"kgspBinArchiveBooter{LOAD}Ucode_{GPU}", "num_sigs")
    if len(raw) != 4:
        raise MyException(f"num_sigs array for {name} is wrong size of {len(raw)}")
    num_sigs = struct.unpack("<I", raw)[0]
    if num_sigs < 1 or num_sigs > 15:
        raise MyException(f"out of range number of signatures ({num_sigs}) for {name}")
    tlv.add("NSIG", num_sigs)

    # Extract the signatures.  Technically, we don't need to pass the signature size to
    # this function, but doing so allows us to double-check all the array sizes.
    signatures = get_bytes(filename, f"kgspBinArchiveBooter{LOAD}Ucode_{GPU}", f"sig_{fuse}")
    signatures_size = len(signatures)
    if signatures_size % sigsize:
        raise MyException(f"signature file size for {name} is {signatures_size}, an uneven multiple of {sigsize}")
    if num_sigs != signatures_size // sigsize:
        raise MyException(f"mismatch number of signatures ({signatures_size // sigsize}), should be {num_sigs}")
    tlv.add("SIGN", signatures)

    # Extract the patch location
    raw = get_bytes(filename, f"kgspBinArchiveBooter{LOAD}Ucode_{GPU}", "patch_loc")
    if len(raw) != 4:
        raise MyException(f"patch_loc[] array for {name} should be one element, but is {len(raw)} bytes.")
    patchloc = struct.unpack("<I", raw)[0]
    tlv.add("PLOC", patchloc)

    # Extract the patch sig offset.  RM expects this to be zero, but doesn't use it,
    # so if it's ever non-zero, something has changed.
    raw = get_bytes(filename, f"kgspBinArchiveBooter{LOAD}Ucode_{GPU}", "patch_sig")
    if len(raw) != 4:
        raise MyException(f"patch_sig[] array for {name} should be one element, but is {len(raw)} bytes.")
    patchsig = struct.unpack("<I", raw)[0]
    if patchsig != 0:
        raise MyException(f"patch_sig for {name} should be 0, but is instead {patchsig}.")

    # Extract the patch meta variables
    raw = get_bytes(filename, f"kgspBinArchiveBooter{LOAD}Ucode_{GPU}", "patch_meta")
    fuse_ver, engine_id, ucode_id = struct.unpack("<III", raw)
    tlv.add("FUSE", fuse_ver)
    tlv.add("ENID", engine_id)
    tlv.add("UCID", ucode_id)

    # Extract the descriptor (nvfw_hs_load_header_v2)
    descriptor = get_bytes(filename, f"kgspBinArchiveBooter{LOAD}Ucode_{GPU}", f"header_{fuse}")
    if len(descriptor) < 36:
        raise MyException(f"nvfw_hs_load_header_v2 descriptor for {name} must be at least 36 bytes, but is instead {len(descriptor)} bytes.")

    # Extract some of individual fields of nvfw_hs_load_header_v2
    (os_code_offset, os_code_size, os_data_offset, os_data_size, num_apps,
     app_code_offset, app_code_size, app_data_offset, app_data_size) = struct.unpack_from("<9I", descriptor)
    # Verify that sizeof(descriptor) == 5 * 4 + num_apps * 16
    if len(descriptor) != 5 * 4 + num_apps * 16:
        raise MyException(f"nvfw_hs_load_header_v2 descriptor for {name} should be {5 * 4 + num_apps * 16} bytes, but is instead {len(descriptor)} bytes.")
    # Nova depends on os_code_size == app_code_offset
    if os_code_size != app_code_offset:
        raise MyException(f"nvfw_hs_load_header_v2 descriptor for {name} has os_code_size={os_code_size} and app_code_offset={app_code_offset}, but they should be the same.")

    # Extract the actual booter firmware
    firmware = get_bytes(filename, f"kgspBinArchiveBooter{LOAD}Ucode_{GPU}", f"image_{fuse}")

    if os_code_size + os_data_size + app_code_size > len(firmware):
        raise MyException(f"{os_code_size=} + {os_data_size=} + {app_code_size=} for {name} exceeds image size of {len(firmware)}")
    if os_code_offset + os_code_size > len(firmware):
        raise MyException(f"{os_code_offset=} + {os_code_size=} for {name} exceeds image size of {len(firmware)}")
    if os_data_offset + os_data_size > len(firmware):
        raise MyException(f"{os_data_offset=} + {os_data_size=} for {name} exceeds image size of {len(firmware)}")
    if app_code_offset + app_code_size > len(firmware):
        raise MyException(f"{app_code_offset=} + {app_code_size=} for {name} exceeds image size of {len(firmware)}")

    tlv.add("CDOF", os_code_offset)
    tlv.add("CDSZ", os_code_size)
    tlv.add("DAOF", os_data_offset)
    tlv.add("DASZ", os_data_size)
    tlv.add("A0CO", app_code_offset)
    tlv.add("A0CS", app_code_size)

    tlv.add("BLOB", firmware)

    tlv.write()

# GPU memory scrubber, needed for some GPUs and configurations
def scrubber(gpu, sigsize, debug = False):
    fuse = "dbg" if debug else "prod"
    # Unfortunately, RM breaks convention with the scrubber image and labels
    # the files and arrays with AD10X instead of AD102.
    GPUX = f"{gpu[:-1].upper()}X"
    name = f"scrubber-{gpu}-{fuse}"

    filename = f"src/nvidia/generated/g_bindata_ksec2GetBinArchiveSecurescrubUcode_{GPUX}.c"

    tlv = TLV("scrubber", gpu)

    # Query the number of signatures.  This should be a 4-byte array (32-bit little-endian integer)
    raw = get_bytes(filename, f"ksec2BinArchiveSecurescrubUcode_{GPUX}", "num_sigs")
    if len(raw) != 4:
        raise MyException(f"num_sigs array for {name} is wrong size of {len(raw)}")
    num_sigs = struct.unpack("<I", raw)[0]
    if num_sigs < 1 or num_sigs > 15:
        raise MyException(f"out of range number of signatures ({num_sigs}) for {name}")
    tlv.add("NSIG", num_sigs)

    # Extract the signatures.  Technically, we don't need to pass the signature size to
    # this function, but doing so allows us to double-check all the array sizes.
    signatures = get_bytes(filename, f"ksec2BinArchiveSecurescrubUcode_{GPUX}", f"sig_{fuse}")
    signatures_size = len(signatures)
    if signatures_size % sigsize:
        raise MyException(f"signature file size for {name} is {signatures_size}, an uneven multiple of {sigsize}")
    if num_sigs != signatures_size // sigsize:
        raise MyException(f"mismatch number of signatures ({signatures_size // sigsize}), should be {num_sigs}")
    tlv.add("SIGN", signatures)

    # Extract the patch location
    raw = get_bytes(filename, f"ksec2BinArchiveSecurescrubUcode_{GPUX}", "patch_loc")
    if len(raw) != 4:
        raise MyException(f"patch_loc[] array for {name} should be one element, but is {len(raw)} bytes.")
    patchloc = struct.unpack("<I", raw)[0]
    tlv.add("PLOC", patchloc)

    # Extract the patch sig offset.  RM expects this to be zero, but doesn't use it,
    # so if it's ever non-zero, something has changed.
    raw = get_bytes(filename, f"ksec2BinArchiveSecurescrubUcode_{GPUX}", "patch_sig")
    if len(raw) != 4:
        raise MyException(f"patch_sig[] array for {name} should be one element, but is {len(raw)} bytes.")
    patchsig = struct.unpack("<I", raw)[0]
    if patchsig != 0:
        raise MyException(f"patch_sig for {name} should be 0, but is instead {patchsig}.")

    # Extract the patch meta variables
    raw = get_bytes(filename, f"ksec2BinArchiveSecurescrubUcode_{GPUX}", "patch_meta")
    fuse_ver, engine_id, ucode_id = struct.unpack("<III", raw)
    tlv.add("FUSE", fuse_ver)
    tlv.add("ENID", engine_id)
    tlv.add("UCID", ucode_id)

    # Extract the descriptor (nvfw_hs_load_header_v2)
    descriptor = get_bytes(filename, f"ksec2BinArchiveSecurescrubUcode_{GPUX}", f"header_{fuse}")
    if len(descriptor) < 36:
        raise MyException(f"nvfw_hs_load_header_v2 descriptor for {name} must be at least 36 bytes, but is instead {len(descriptor)} bytes.")

    # Extract some of individual fields of nvfw_hs_load_header_v2
    (os_code_offset, os_code_size, os_data_offset, os_data_size, num_apps,
     app_code_offset, app_code_size, app_data_offset, app_data_size) = struct.unpack_from("<9I", descriptor)
    # Verify that sizeof(descriptor) == 5 * 4 + num_apps * 16
    if len(descriptor) != 5 * 4 + num_apps * 16:
        raise MyException(f"nvfw_hs_load_header_v2 descriptor for {name} should be {5 * 4 + num_apps * 16} bytes, but is instead {len(descriptor)} bytes.")
    # Nova depends on os_code_size == app_code_offset
    if os_code_size != app_code_offset:
        raise MyException(f"nvfw_hs_load_header_v2 descriptor for {name} has os_code_size={os_code_size} and app_code_offset={app_code_offset}, but they should be the same.")

    # Extract the actual scrubber firmware
    firmware = get_bytes(filename, f"ksec2BinArchiveSecurescrubUcode_{GPUX}", f"image_{fuse}")

    if os_code_size + os_data_size + app_code_size > len(firmware):
        raise MyException(f"{os_code_size=} + {os_data_size=} + {app_code_size=} for {name} exceeds image size of {len(firmware)}")
    if os_code_offset + os_code_size > len(firmware):
        raise MyException(f"{os_code_offset=} + {os_code_size=} for {name} exceeds image size of {len(firmware)}")
    if os_data_offset + os_data_size > len(firmware):
        raise MyException(f"{os_data_offset=} + {os_data_size=} for {name} exceeds image size of {len(firmware)}")
    if app_code_offset + app_code_size > len(firmware):
        raise MyException(f"{app_code_offset=} + {app_code_size=} for {name} exceeds image size of {len(firmware)}")

    tlv.add("CDOF", os_code_offset)
    tlv.add("CDSZ", os_code_size)
    tlv.add("DAOF", os_data_offset)
    tlv.add("DASZ", os_data_size)
    tlv.add("A0CO", app_code_offset)
    tlv.add("A0CS", app_code_size)

    tlv.add("BLOB", firmware)

    tlv.write()

# Unlike the other images, FMC firmware and its metadata are encapsulated in
# an ELF image.  FMC metadata is simpler than the other firmware types, as it
# comprises just three binary blobs.
def fmc(gpu: str, debug = False):
    fuse = "Debug" if debug else "Prod"
    GPU=gpu.upper()
    filename = f"src/nvidia/generated/g_bindata_kgspGetBinArchiveGspRmFmcGfw{fuse}Signed_{GPU}.c"

    tlv = TLV("fmc", gpu)

    ucode_hash = get_bytes(filename, f"kgspBinArchiveGspRmFmcGfw{fuse}Signed_{GPU}", "ucode_hash")
    if len(ucode_hash) != 48:
        raise MyException(f"FSP hash length for {gpu} should be 48 but is {len(ucode_hash)}")
    tlv.add("HASH", ucode_hash)

    # Some GPUs use RSAPSS3K (384-byte sig/pkey), and others use ECDSAP384
    # (97/97-byte sig).  Just make some simple range checks that Nova expects.

    ucode_sig = get_bytes(filename, f"kgspBinArchiveGspRmFmcGfw{fuse}Signed_{GPU}", "ucode_sig")
    if len(ucode_sig) < 96 or len(ucode_sig) > 384:
        raise MyException(f"FSP signature for {gpu} has an invalid length of {len(ucode_sig)}")
    tlv.add("SIGN", ucode_sig)

    ucode_pkey = get_bytes(filename, f"kgspBinArchiveGspRmFmcGfw{fuse}Signed_{GPU}", "ucode_pkey")
    if len(ucode_pkey) < 97 or len(ucode_pkey) > 384:
        raise MyException(f"FSP public key for {gpu} has an invalid length of {len(ucode_pkey)}")
    tlv.add("PKEY", ucode_pkey)

    ucode_image = get_bytes(filename, f"kgspBinArchiveGspRmFmcGfw{fuse}Signed_{GPU}", "ucode_image")
    tlv.add("BLOB", ucode_image)

    tlv.write()

def fwimage_from_gsp_elf(filename: str, gpu: str):
    global outputpath

    elf = ELF64(filename)

    os.makedirs(f"{outputpath}/nvidia/{gpu}/{subdir}/", exist_ok = True)

    with open(f"{outputpath}/nvidia/{gpu}/{subdir}/gsp.bin", "wb") as f:
        f.write(elf.section(".fwimage"))

    print(f"Created {gpu}/{subdir}/gsp.bin from {filename}")

# Generate a gsp.tlv file that points to the correct GSP image
# `elf` is the original ELF image from the .run file or build
# `signame` is the name of the .fwsignature section to extract
# `gpu` is the GPU name
def gsp_tlv_from_elf(elf: ELF64, signame: str, gpu: str):
    signature = elf.section(signame)

    tlv = TLV("gsp", gpu)
    tlv.add("SIGN", signature)
    tlv.add("SIZE", len(elf.section(".fwimage")))
    tlv.add("FILE", "gsp.bin")

    # The ".note.gnu.build-id" section contains a 16-byte ELF note header
    # followed by the 20-byte build ID.
    build_id = elf.section(".note.gnu.build-id")
    tlv.add("BLID", build_id[16:])

    tlv.write()

# Copy ucodes binaries if present (r610+) and creates its TLV.  Each ucodes.bin
# is paired with the corresponding gsp.bin and loaded separately by the driver.
def ucodes(gsp_source):
    global outputpath

    tu10x_ucodes_src = os.path.join(gsp_source, "ucodes_tu10x.bin")
    ga10x_ucodes_src = os.path.join(gsp_source, "ucodes_ga10x.bin")

    if os.path.exists(tu10x_ucodes_src):
        tlv = TLV("ucodes", "tu102")
        tlv.add("SIZE", os.path.getsize(tu10x_ucodes_src))
        tlv.add("FILE", "ucodes.bin")
        tlv.write()

        os.makedirs(f"{outputpath}/nvidia/tu102/{subdir}/", exist_ok = True)
        shutil.copyfile(tu10x_ucodes_src, f"{outputpath}/nvidia/tu102/{subdir}/ucodes.bin")
        print(f"Copied ucodes_tu10x.bin to nvidia/tu102/{subdir}/ucodes.bin")

    if os.path.exists(ga10x_ucodes_src):
        tlv = TLV("ucodes", "ga102")
        tlv.add("SIZE", os.path.getsize(ga10x_ucodes_src))
        tlv.add("FILE", "ucodes.bin")
        tlv.write()

        os.makedirs(f"{outputpath}/nvidia/ga102/{subdir}/", exist_ok = True)
        shutil.copyfile(ga10x_ucodes_src, f"{outputpath}/nvidia/ga102/{subdir}/ucodes.bin")
        print(f"Copied ucodes_ga10x.bin to nvidia/ga102/{subdir}/ucodes.bin")

# Extract the GSP-RM binaries and create the TLV files for each GPU that has its own
# .fwsignature section
def gsprm(tu10x_gsp_src, ga10x_gsp_src):
    global outputpath

    fwimage_from_gsp_elf(tu10x_gsp_src, "tu102")
    fwimage_from_gsp_elf(ga10x_gsp_src, "ga102")

    elf = ELF64(tu10x_gsp_src)
    gsp_tlv_from_elf(elf, ".fwsignature_tu10x", "tu102")
    gsp_tlv_from_elf(elf, ".fwsignature_tu11x", "tu116")
    gsp_tlv_from_elf(elf, ".fwsignature_ga100", "ga100")

    elf = ELF64(ga10x_gsp_src)
    gsp_tlv_from_elf(elf, ".fwsignature_ga10x", "ga102")
    gsp_tlv_from_elf(elf, ".fwsignature_ad10x", "ad102")
    gsp_tlv_from_elf(elf, ".fwsignature_gh100", "gh100")
    gsp_tlv_from_elf(elf, ".fwsignature_gb10x", "gb100")
    gsp_tlv_from_elf(elf, ".fwsignature_gb20x", "gb202")

    if os.path.isdir(f"{outputpath}/nvidia/gb10b/{subdir}"):
        gsp_tlv_from_elf(elf, ".fwsignature_gb10y", "gb10b")
    if os.path.isdir(f"{outputpath}/nvidia/gb20b/{subdir}"):
        gsp_tlv_from_elf(elf, ".fwsignature_gb20y", "gb20b")
    if os.path.isdir(f"{outputpath}/nvidia/gr100/{subdir}"):
        gsp_tlv_from_elf(elf, ".fwsignature_gr10x", "gr100")

# Extract the GSP-RM firmware from the .run file and copy the binaries
# to the target directory.
def gsp_firmware_from_run(filename):
    with tempfile.TemporaryDirectory() as temp:
        directory = extract_run_file(filename, temp)

        tu10x_gsp_src = os.path.join(directory, "gsp_tu10x.bin")
        ga10x_gsp_src = os.path.join(directory, "gsp_ga10x.bin")

        gsprm(tu10x_gsp_src, ga10x_gsp_src)
        ucodes(directory)

# Extract GSP firmware from a local build output directory.
# This is an NVIDIA-internal feature for use with internal build systems.
def gsp_firmware_from_build(gsp_build_dir):
    if not os.path.isdir(gsp_build_dir):
        raise MyException(f"GSP build directory does not exist: {gsp_build_dir}")

    tu10x_gsp_src = os.path.join(gsp_build_dir, "gsp_tu10x.bin")
    ga10x_gsp_src = os.path.join(gsp_build_dir, "gsp_ga10x.bin")

    if not os.path.exists(tu10x_gsp_src) or not os.path.exists(ga10x_gsp_src):
        raise MyException(f"Firmware files are missing in {gsp_build_dir}")

    gsprm(tu10x_gsp_src, ga10x_gsp_src)
    ucodes(gsp_build_dir)

# Find the .run file and extract the GSP-RM binaries from it.
#
# `pathspec` is either
# 1) empty string -- determine the URL to download the .run file that matches the current OpenRM tree
# 2) url -- download the .run file from that URL
# 3) filename -- use the specific filename
# 4) path -- extract the binaries from the local build
def driver(pathspec):
    global version

    if pathspec == '':
        # No path/url provided, so make a guess of the URL
        # to automatically download the right version.
        pathspec = f'https://download.nvidia.com/XFree86/Linux-x86_64/{version}/NVIDIA-Linux-x86_64-{version}.run'

    if re.search('^https?://', pathspec):
        with tempfile.NamedTemporaryFile(prefix = f'NVIDIA-Linux-x86_64-{version}-', suffix = '.run') as f:
            print(f"Downloading driver from {pathspec} as {f.name}")
            try:
                urllib.request.urlretrieve(pathspec, f.name)
            except urllib.error.HTTPError as e:
                if e.code == 404:
                    raise MyException(f"Driver version {version} not found at download.nvidia.com.")
                raise MyException(f"Failed to download {pathspec}: HTTP {e.code} {e.reason}")
            except urllib.error.URLError as e:
                raise MyException(f"Failed to download {pathspec}: {e.reason}")

            gsp_firmware_from_run(f.name)
    elif os.path.isfile(pathspec) and os.access(pathspec, os.R_OK):
        gsp_firmware_from_run(pathspec)
    elif os.path.isdir(pathspec):
        gsp_firmware_from_build(pathspec)
    else:
        raise MyException(f"{pathspec} does not exist or is unreadable.")

# Create symlinks in the target directory for the other GPUs.  This mirrors
# what the WHENCE file in linux-firmware does.
def symlinks():
    global outputpath
    from pathlib import Path

    print(f"Creating symlinks in {outputpath}/nvidia")
    prev_cwd = os.getcwd()
    os.chdir(f"{outputpath}/nvidia")

    try:
        # Turing / GA100
        for d in ['tu104', 'tu106', 'tu116', 'tu117', 'ga100']:
            os.makedirs(d, exist_ok = True)

        for d in ['tu104', 'tu106']:
            symlink(f'../tu102/{subdir}', f"{d}/{subdir}", target_is_directory = True)

        symlink(f'../tu116/{subdir}', f'tu117/{subdir}', target_is_directory = True)

        # TU11x uses the same GSP bootloader as TU10x
        symlink(f"../../tu102/{subdir}/gsp_bootloader.tlv", f"tu116/{subdir}/gsp_bootloader.tlv")

        # TU11x and GA100 use the same generic bootloader as TU10x
        symlink(f"../../tu102/{subdir}/gen_bootloader.tlv", f"tu116/{subdir}/gen_bootloader.tlv")
        symlink(f"../../tu102/{subdir}/gen_bootloader.tlv", f"ga100/{subdir}/gen_bootloader.tlv")

        # Symlink the GSP-RM image for TU11x and GA100
        symlink(f"../../tu102/{subdir}/gsp.bin", f"tu116/{subdir}/gsp.bin")
        symlink(f"../../tu102/{subdir}/gsp.bin", f"ga100/{subdir}/gsp.bin")

        # Ampere
        for d in ['ga103', 'ga104', 'ga106', 'ga107']:
            os.makedirs(d, exist_ok = True)
            symlink(f'../ga102/{subdir}', f"{d}/{subdir}", target_is_directory = True)

        # Ada
        for d in ['ad103', 'ad104', 'ad106', 'ad107']:
            symlink('ad102', d, target_is_directory = True)

        # Blackwell
        for d in ['gb102']:
            symlink('gb100', d, target_is_directory = True)

        for d in ['gb203', 'gb205', 'gb206', 'gb207']:
            symlink('gb202', d, target_is_directory = True)

        # Rubin
        for d in ['gr102']:
            symlink('gr100', d, target_is_directory = True)

        # Handle gsp.bin and gsp.tlv for all remaining paths.
        root = Path(".")
        paths = [p for p in root.glob("*") if os.path.isdir(f"{p}/{subdir}") and not os.path.exists(f"{p}/{subdir}/gsp.bin")]
        for p in paths:
            symlink(f"../../ga102/{subdir}/gsp.bin", f"{p}/{subdir}/gsp.bin")

        # Symlink the ucodes binaries, if they exist.
        if os.path.exists(f"tu102/{subdir}/ucodes.bin"):
            symlink(f"../../tu102/{subdir}/ucodes.bin", f"tu116/{subdir}/ucodes.bin")
            symlink(f"../../tu102/{subdir}/ucodes.bin", f"ga100/{subdir}/ucodes.bin")
            symlink(f"../../tu102/{subdir}/ucodes.tlv", f"tu116/{subdir}/ucodes.tlv")
            symlink(f"../../tu102/{subdir}/ucodes.tlv", f"ga100/{subdir}/ucodes.tlv")

        if os.path.exists(f"ga102/{subdir}/ucodes.bin"):
            paths = [p for p in root.glob("*") if os.path.isdir(f"{p}/{subdir}") and not os.path.exists(f"{p}/{subdir}/ucodes.bin") and p.name != "ga102"]
            for p in paths:
                symlink(f"../../ga102/{subdir}/ucodes.bin", f"{p}/{subdir}/ucodes.bin")
                symlink(f"../../ga102/{subdir}/ucodes.tlv", f"{p}/{subdir}/ucodes.tlv")

        # Verify that there are no broken symlinks, and that every file
        # symlink points to a file inside a gsp/ directory.
        for dirpath, dirnames, filenames in os.walk("."):
            for name in dirnames + filenames:
                full = os.path.join(dirpath, name)
                if not os.path.islink(full):
                    continue
                target = os.readlink(full)
                if not os.path.exists(full):
                    print(f"Warning: broken symlink {full} -> {target}")
                    continue
                # Directory symlinks (e.g. tu104/gsp -> ../tu102/gsp,
                # ad103 -> ad102) are exempt.
                if os.path.isdir(full):
                    continue
                resolved = os.path.normpath(os.path.join(dirpath, target))
                if os.path.basename(os.path.dirname(resolved)) != subdir:
                    print(f"Warning: symlink {full} -> {target} does not point to a file in a {subdir}/ directory")

        # Verify that we have the necessary files or links for every GPU.
        for dir in [entry.name for entry in os.scandir(".") if entry.is_dir()]:
            if not os.path.exists(f"{dir}/{subdir}/gsp.bin"):
                print(f"Warning: {dir}/{subdir}/gsp.bin is missing")
            if not os.path.exists(f"{dir}/{subdir}/gsp.tlv"):
                print(f"Warning: {dir}/{subdir}/gsp.tlv is missing")
            if os.path.islink(f"{dir}/{subdir}/gsp.tlv"):
                print(f"Warning: {dir}/{subdir}/gsp.tlv should not be a symlink")
            if not os.path.exists(f"{dir}/{subdir}/gsp_bootloader.tlv"):
                print(f"Warning: {dir}/{subdir}/gsp_bootloader.tlv is missing")

            has_load = os.path.exists(f"{dir}/{subdir}/booter_load.tlv");
            has_unload = os.path.exists(f"{dir}/{subdir}/booter_unload.tlv");
            has_fmc = os.path.exists(f"{dir}/{subdir}/fmc.tlv");
            if has_load != has_unload:
                print(f"Warning: {dir}/{subdir}/ booter_load requires booter_unload, and vice versa")
            if not has_load and not has_fmc:
                print(f"Warning: {dir}/{subdir}/ must have booter or fmc")
            if has_load and has_fmc:
                print(f"Warning: {dir}/{subdir}/ must not have both booter and fmc")

    finally:
        os.chdir(prev_cwd)

# Create a text file that can be inserted as-is to the WHENCE file of the linux-firmware
# git repository.
#
# Traverse the outputpath and build a WHENCE file from it.  This ensures that WHENCE file
# always matches the real world.  The drawback is that it requires the -d and -s options
# to get a full picture.

# We must also maintain compatibility with the existing directory hierarchy that is
# defined by Nouveau, which is why ga103/gsp -> ga102/gsp, but ad103 -> ad102.
#
# Some hard rules for the layout of files:
#  1. No file of any version can symlink to a file of a different version,
#     even if the files are identical.  This allows distros to ship each version
#     independently.  Note that this does not really apply to TLV files, but
#     the restriction is still valid.
#  2. All files must be located in the /gsp/ subdirectory of the GPU directory,
#     and there must be no symlinks to any files outside of the /gsp/ directory.
#     This allows the Nova driver to find all of the files it needs inside
#     the /gsp/ directory.
#  3. Replacing a file/directory with a symlink (or vice versa) is strongly
#     discouraged.  Many distros cannot handle this transition.
#  4. Ideally, this file should only change when adding support for new GPUs,
#     because newer versions of firmware images should have the same filename
#     as previous versions.
def whence():
    global outputpath

    whence = []
    for dirpath, dirnames, filenames in os.walk(f"{outputpath}/nvidia"):  # followlinks=False by default
        # Regular files and symlinks-to-files both land in filenames
        for name in filenames:
            full = os.path.join(dirpath, name)
            rel = os.path.relpath(full, outputpath)
            if os.path.islink(full):
                whence.append(f"Link: {rel} -> {os.readlink(full)}\n")
            else:
                whence.append(f"File: {rel}\n")
        # Symlinks-to-directories land in dirnames; with followlinks=False
        # os.walk lists them but won't recurse into them, so capture them here.
        for name in dirnames:
            full = os.path.join(dirpath, name)
            if os.path.islink(full):
                whence.append(f"Link: {os.path.relpath(full, outputpath)} -> {os.readlink(full)}\n")

    whence = sorted(whence, key=lambda s: s.split()[1])

    with open(f"{outputpath}/WHENCE.txt", 'w') as f:
        f.writelines(whence)

    print(f"Created {outputpath}/WHENCE.txt")

def main():
    global outputpath
    global version

    parser = argparse.ArgumentParser(
        description = 'Extract firmware binaries from the OpenRM git repository'
        ' in a format expected by the Nova device drivers.',
        epilog = 'Running as root and specifying -o /lib/firmware will install'
        ' the firmware files directly where Nova expects them.'
        ' The --revision option is useful for testing new firmware'
        ' versions without changing Nova source code.'
        ' The --driver option accepts a .run file path, a URL, or a local'
        ' build output directory.  If -d is given with no argument, the .run'
        ' file is downloaded automatically.')
    parser.add_argument('-i', '--input', default = os.getcwd(),
        help = 'Path to source directory (where version.mk exists)')
    parser.add_argument('-o', '--output', default = os.path.join(os.getcwd(), '_out'),
        help = 'Path to target directory (where files will be written)')
    parser.add_argument('-r', '--revision',
        help = 'Files will be named with this version number.')
    parser.add_argument('--debug-fused', action='store_true',
        help = 'Extract debug instead of production images.')
    parser.add_argument('-d', '--driver',
        nargs = '?', const = '',
        help = 'Also extract GSP-RM firmware from a source.'
        ' A URL or path to a .run driver package downloads or extracts it.'
        ' A path to a local build output directory (e.g.'
        ' drivers/resman/build/gsp/_out/Linux_amd64_release) copies'
        ' the GSP firmware directly.  If -d is given with no argument,'
        ' the .run file is downloaded automatically.')
    parser.add_argument('-s', '--symlink', action='store_true',
        help = 'Also create symlinks for all supported GPUs.  Requires -d option.')
    parser.add_argument('-w', '--whence', action='store_true',
        help = 'Also generate a WHENCE file.  Requires -d and -s options.')

    args = parser.parse_args()

    if args.symlink and args.driver is None:
        parser.error("-s/--symlink requires -d/--driver")
    if args.whence and (args.driver is None or not args.symlink):
        parser.error("-w/--whence requires both -d/--driver and -s/--symlink")

    outputpath = os.path.abspath(args.output)

    # Symlinks should not be created in the linux-firmware git repository, because
    # it uses the WHENCE file to create symlinks.  Check for that, to avoid
    # accidental usage that is confusing.
    if args.symlink and os.path.exists(f"{outputpath}/copy-firmware.sh"):
        raise MyException("Symlinks should not be created in the linux-firmware repository")

    # If args.driver is a file or path, normalize it before the chdir.
    if args.driver and not re.search('^https?://', args.driver):
        args.driver = os.path.abspath(args.driver)

    args.input = os.path.abspath(args.input)
    os.chdir(args.input)

    if not os.path.isfile("version.mk"):
        raise MyException(f"Source directory {args.input} is incorrect")

    version = args.revision
    if not version:
        with open("version.mk") as f:
            m = re.search(r'^NVIDIA_VERSION = ([^\s]+)', f.read(), re.MULTILINE)
            if not m:
                raise MyException("Could not find or parse NVIDIA_VERSION from version.mk")
            version = m.group(1)

    if not version.isascii():
        raise MyException(f"Version string {version} must not contain non-ASCII characters")

    print(f"Generating files for version {version}")

    print(f"Writing files to {outputpath}")
    os.makedirs(f"{outputpath}/nvidia", exist_ok = True)

    # FIXME: These values are currently not used
    if os.path.exists('src/nvidia/generated/g_gpu_arch_nvoc.c'):
        try:
            bGpuArchIsZeroFb = parse_hal_field('src/nvidia/generated/g_gpu_arch_nvoc.c', 'bGpuArchIsZeroFb')
            bGpuarchSupportsIgpuRg = parse_hal_field('src/nvidia/generated/g_gpu_arch_nvoc.c', 'bGpuarchSupportsIgpuRg')
        except Exception:
            pass

    # The generic bootloader is only defined for TU102 but is used
    # by all TU1xx and GA100.
    generic_bootloader("tu102")

    booter("tu102", "load", 16, args.debug_fused)
    booter("tu102", "unload", 16, args.debug_fused)
    # TU10x and GA100 do not have debug-fused versions of the GSP bootloader
    gsp_bootloader("tu102")

    booter("tu116", "load", 16, args.debug_fused)
    booter("tu116", "unload", 16, args.debug_fused)
    # TU11x uses the same bootloader as TU10x

    booter("ga100", "load", 384, args.debug_fused)
    booter("ga100", "unload", 384, args.debug_fused)
    gsp_bootloader("ga100")

    booter("ga102", "load", 384, args.debug_fused)
    booter("ga102", "unload", 384, args.debug_fused)
    gsp_bootloader("ga102", args.debug_fused)

    booter("ad102", "load", 384, args.debug_fused)
    booter("ad102", "unload", 384, args.debug_fused)
    gsp_bootloader("ad102", args.debug_fused)
#    scrubber("ad102", 384, args.debug_fused) # Not currently used by Nova

    gsp_bootloader("gh100", args.debug_fused)
    fmc("gh100", args.debug_fused)

    gsp_bootloader("gb100", args.debug_fused)
    fmc("gb100", args.debug_fused)

    # GB10B (Jetson Thor) support was added in r580
    if is_supported("gb10b"):
        gsp_bootloader("gb10b", args.debug_fused)
        fmc("gb10b", args.debug_fused)

    gsp_bootloader("gb202", args.debug_fused)
    fmc("gb202", args.debug_fused)

    # GB20B (N1X) support was added in r580
    if is_supported("gb20b"):
        gsp_bootloader("gb20b", args.debug_fused)
        fmc("gb20b", args.debug_fused)

    # GR100 support was added in r610
    if is_supported("gr100"):
        gsp_bootloader("gr100", args.debug_fused)
        fmc("gr100", args.debug_fused)

    if args.driver is not None:
        driver(args.driver)

    if args.symlink:
        symlinks()

    if args.whence:
        whence()

if __name__ == "__main__":
    try:
        main()
    except MyException as e:
        # The full stack trace is too noisy with MyException
        print(f"Error: {e}", file=sys.stderr)
        sys.exit(1)
