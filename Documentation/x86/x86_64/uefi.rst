.. SPDX-License-Identifier: GPL-2.0

=====================================
General note on [U]EFI x86_64 support
=====================================

The nomenclature EFI and UEFI are used interchangeably in this document.

Although the tools below are _not_ needed for building the kernel,
the needed bootloader support and associated tools for x86_64 platforms
with EFI firmware and specifications are listed below.

1. UEFI specification:  http://www.uefi.org

2. Booting Linux kernel on UEFI x86_64 platform requires bootloader
   support. Elilo with x86_64 support can be used.

3. x86_64 platform with EFI/UEFI firmware.

Mechanics
---------

- Build the kernel with the following configuration::

	CONFIG_FB_EFI=y
	CONFIG_FRAMEBUFFER_CONSOLE=y

  If EFI runtime services are expected, the following configuration should
  be selected::

	CONFIG_EFI=y
	CONFIG_EFI_VARS=y or m		# optional

- Create a VFAT partition on the disk
- Copy the following to the VFAT partition:

	elilo bootloader with x86_64 support, elilo configuration file,
	kernel image built in first step and corresponding
	initrd. Instructions on building elilo and its dependencies
	can be found in the elilo sourceforge project.

- Boot to EFI shell and invoke elilo choosing the kernel image built
  in first step.
- If some or all EFI runtime services don't work, you can try following
  kernel command line parameters to turn off some or all EFI runtime
  services.

	noefi
		turn off all EFI runtime services
	reboot_type=k
		turn off EFI reboot runtime service

- If the EFI memory map has additional entries not in the E820 map,
  you can include those entries in the kernels memory map of available
  physical RAM by using the following kernel command line parameter.

	add_efi_memmap
		include EFI memory map of available physical RAM
