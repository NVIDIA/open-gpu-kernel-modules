================================================
The Unified Extensible Firmware Interface (UEFI)
================================================

UEFI, the Unified Extensible Firmware Interface, is a specification
governing the behaviours of compatible firmware interfaces. It is
maintained by the UEFI Forum - http://www.uefi.org/.

UEFI is an evolution of its predecessor 'EFI', so the terms EFI and
UEFI are used somewhat interchangeably in this document and associated
source code. As a rule, anything new uses 'UEFI', whereas 'EFI' refers
to legacy code or specifications.

UEFI support in Linux
=====================
Booting on a platform with firmware compliant with the UEFI specification
makes it possible for the kernel to support additional features:

- UEFI Runtime Services
- Retrieving various configuration information through the standardised
  interface of UEFI configuration tables. (ACPI, SMBIOS, ...)

For actually enabling [U]EFI support, enable:

- CONFIG_EFI=y
- CONFIG_EFIVAR_FS=y or m

The implementation depends on receiving information about the UEFI environment
in a Flattened Device Tree (FDT) - so is only available with CONFIG_OF.

UEFI stub
=========
The "stub" is a feature that extends the Image/zImage into a valid UEFI
PE/COFF executable, including a loader application that makes it possible to
load the kernel directly from the UEFI shell, boot menu, or one of the
lightweight bootloaders like Gummiboot or rEFInd.

The kernel image built with stub support remains a valid kernel image for
booting in non-UEFI environments.

UEFI kernel support on ARM
==========================
UEFI kernel support on the ARM architectures (arm and arm64) is only available
when boot is performed through the stub.

When booting in UEFI mode, the stub deletes any memory nodes from a provided DT.
Instead, the kernel reads the UEFI memory map.

The stub populates the FDT /chosen node with (and the kernel scans for) the
following parameters:

==========================  ======   ===========================================
Name                        Size     Description
==========================  ======   ===========================================
linux,uefi-system-table     64-bit   Physical address of the UEFI System Table.

linux,uefi-mmap-start       64-bit   Physical address of the UEFI memory map,
                                     populated by the UEFI GetMemoryMap() call.

linux,uefi-mmap-size        32-bit   Size in bytes of the UEFI memory map
                                     pointed to in previous entry.

linux,uefi-mmap-desc-size   32-bit   Size in bytes of each entry in the UEFI
                                     memory map.

linux,uefi-mmap-desc-ver    32-bit   Version of the mmap descriptor format.

linux,initrd-start          64-bit   Physical start address of an initrd

linux,initrd-end            64-bit   Physical end address of an initrd

kaslr-seed                  64-bit   Entropy used to randomize the kernel image
                                     base address location.
==========================  ======   ===========================================
