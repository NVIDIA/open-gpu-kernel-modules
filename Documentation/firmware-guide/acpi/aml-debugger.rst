.. SPDX-License-Identifier: GPL-2.0
.. include:: <isonum.txt>

================
The AML Debugger
================

:Copyright: |copy| 2016, Intel Corporation
:Author: Lv Zheng <lv.zheng@intel.com>


This document describes the usage of the AML debugger embedded in the Linux
kernel.

1. Build the debugger
=====================

The following kernel configuration items are required to enable the AML
debugger interface from the Linux kernel::

   CONFIG_ACPI_DEBUGGER=y
   CONFIG_ACPI_DEBUGGER_USER=m

The userspace utilities can be built from the kernel source tree using
the following commands::

   $ cd tools
   $ make acpi

The resultant userspace tool binary is then located at::

   tools/power/acpi/acpidbg

It can be installed to system directories by running "make install" (as a
sufficiently privileged user).

2. Start the userspace debugger interface
=========================================

After booting the kernel with the debugger built-in, the debugger can be
started by using the following commands::

   # mount -t debugfs none /sys/kernel/debug
   # modprobe acpi_dbg
   # tools/power/acpi/acpidbg

That spawns the interactive AML debugger environment where you can execute
debugger commands.

The commands are documented in the "ACPICA Overview and Programmer Reference"
that can be downloaded from

https://acpica.org/documentation

The detailed debugger commands reference is located in Chapter 12 "ACPICA
Debugger Reference".  The "help" command can be used for a quick reference.

3. Stop the userspace debugger interface
========================================

The interactive debugger interface can be closed by pressing Ctrl+C or using
the "quit" or "exit" commands.  When finished, unload the module with::

   # rmmod acpi_dbg

The module unloading may fail if there is an acpidbg instance running.

4. Run the debugger in a script
===============================

It may be useful to run the AML debugger in a test script. "acpidbg" supports
this in a special "batch" mode.  For example, the following command outputs
the entire ACPI namespace::

   # acpidbg -b "namespace"
