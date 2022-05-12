.. SPDX-License-Identifier: GPL-2.0

=========
Zero Page
=========
The additional fields in struct boot_params as a part of 32-bit boot
protocol of kernel. These should be filled by bootloader or 16-bit
real-mode setup code of the kernel. References/settings to it mainly
are in::

  arch/x86/include/uapi/asm/bootparam.h

===========	=====	=======================	=================================================
Offset/Size	Proto	Name			Meaning

000/040		ALL	screen_info		Text mode or frame buffer information
						(struct screen_info)
040/014		ALL	apm_bios_info		APM BIOS information (struct apm_bios_info)
058/008		ALL	tboot_addr      	Physical address of tboot shared page
060/010		ALL	ist_info		Intel SpeedStep (IST) BIOS support information
						(struct ist_info)
080/010		ALL	hd0_info		hd0 disk parameter, OBSOLETE!!
090/010		ALL	hd1_info		hd1 disk parameter, OBSOLETE!!
0A0/010		ALL	sys_desc_table		System description table (struct sys_desc_table),
						OBSOLETE!!
0B0/010		ALL	olpc_ofw_header		OLPC's OpenFirmware CIF and friends
0C0/004		ALL	ext_ramdisk_image	ramdisk_image high 32bits
0C4/004		ALL	ext_ramdisk_size	ramdisk_size high 32bits
0C8/004		ALL	ext_cmd_line_ptr	cmd_line_ptr high 32bits
140/080		ALL	edid_info		Video mode setup (struct edid_info)
1C0/020		ALL	efi_info		EFI 32 information (struct efi_info)
1E0/004		ALL	alt_mem_k		Alternative mem check, in KB
1E4/004		ALL	scratch			Scratch field for the kernel setup code
1E8/001		ALL	e820_entries		Number of entries in e820_table (below)
1E9/001		ALL	eddbuf_entries		Number of entries in eddbuf (below)
1EA/001		ALL	edd_mbr_sig_buf_entries	Number of entries in edd_mbr_sig_buffer
						(below)
1EB/001		ALL     kbd_status      	Numlock is enabled
1EC/001		ALL     secure_boot		Secure boot is enabled in the firmware
1EF/001		ALL	sentinel		Used to detect broken bootloaders
290/040		ALL	edd_mbr_sig_buffer	EDD MBR signatures
2D0/A00		ALL	e820_table		E820 memory map table
						(array of struct e820_entry)
D00/1EC		ALL	eddbuf			EDD data (array of struct edd_info)
===========	=====	=======================	=================================================
