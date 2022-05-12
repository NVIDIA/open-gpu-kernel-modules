=================================
Linux on the Nios II architecture
=================================

This is a port of Linux to Nios II (nios2) processor.

In order to compile for Nios II, you need a version of GCC with support for the generic
system call ABI. Please see this link for more information on how compiling and booting
software for the Nios II platform:
http://www.rocketboards.org/foswiki/Documentation/NiosIILinuxUserManual

For reference, please see the following link:
http://www.altera.com/literature/lit-nio2.jsp

What is Nios II?
================
Nios II is a 32-bit embedded-processor architecture designed specifically for the
Altera family of FPGAs. In order to support Linux, Nios II needs to be configured
with MMU and hardware multiplier enabled.

Nios II ABI
===========
Please refer to chapter "Application Binary Interface" in Nios II Processor Reference
Handbook.
