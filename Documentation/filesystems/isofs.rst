.. SPDX-License-Identifier: GPL-2.0

==================
ISO9660 Filesystem
==================

Mount options that are the same as for msdos and vfat partitions.

  =========	========================================================
  gid=nnn	All files in the partition will be in group nnn.
  uid=nnn	All files in the partition will be owned by user id nnn.
  umask=nnn	The permission mask (see umask(1)) for the partition.
  =========	========================================================

Mount options that are the same as vfat partitions. These are only useful
when using discs encoded using Microsoft's Joliet extensions.

 ==============	=============================================================
 iocharset=name Character set to use for converting from Unicode to
		ASCII.  Joliet filenames are stored in Unicode format, but
		Unix for the most part doesn't know how to deal with Unicode.
		There is also an option of doing UTF-8 translations with the
		utf8 option.
  utf8          Encode Unicode names in UTF-8 format. Default is no.
 ==============	=============================================================

Mount options unique to the isofs filesystem.

 ================= ============================================================
  block=512        Set the block size for the disk to 512 bytes
  block=1024       Set the block size for the disk to 1024 bytes
  block=2048       Set the block size for the disk to 2048 bytes
  check=relaxed    Matches filenames with different cases
  check=strict     Matches only filenames with the exact same case
  cruft            Try to handle badly formatted CDs.
  map=off          Do not map non-Rock Ridge filenames to lower case
  map=normal       Map non-Rock Ridge filenames to lower case
  map=acorn        As map=normal but also apply Acorn extensions if present
  mode=xxx         Sets the permissions on files to xxx unless Rock Ridge
		   extensions set the permissions otherwise
  dmode=xxx        Sets the permissions on directories to xxx unless Rock Ridge
		   extensions set the permissions otherwise
  overriderockperm Set permissions on files and directories according to
		   'mode' and 'dmode' even though Rock Ridge extensions are
		   present.
  nojoliet         Ignore Joliet extensions if they are present.
  norock           Ignore Rock Ridge extensions if they are present.
  hide		   Completely strip hidden files from the file system.
  showassoc	   Show files marked with the 'associated' bit
  unhide	   Deprecated; showing hidden files is now default;
		   If given, it is a synonym for 'showassoc' which will
		   recreate previous unhide behavior
  session=x        Select number of session on multisession CD
  sbsector=xxx     Session begins from sector xxx
 ================= ============================================================

Recommended documents about ISO 9660 standard are located at:

- http://www.y-adagio.com/
- ftp://ftp.ecma.ch/ecma-st/Ecma-119.pdf

Quoting from the PDF "This 2nd Edition of Standard ECMA-119 is technically
identical with ISO 9660.", so it is a valid and gratis substitute of the
official ISO specification.
