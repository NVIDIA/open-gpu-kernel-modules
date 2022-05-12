============
APM or ACPI?
============

If you have a relatively recent x86 mobile, desktop, or server system,
odds are it supports either Advanced Power Management (APM) or
Advanced Configuration and Power Interface (ACPI).  ACPI is the newer
of the two technologies and puts power management in the hands of the
operating system, allowing for more intelligent power management than
is possible with BIOS controlled APM.

The best way to determine which, if either, your system supports is to
build a kernel with both ACPI and APM enabled (as of 2.3.x ACPI is
enabled by default).  If a working ACPI implementation is found, the
ACPI driver will override and disable APM, otherwise the APM driver
will be used.

No, sorry, you cannot have both ACPI and APM enabled and running at
once.  Some people with broken ACPI or broken APM implementations
would like to use both to get a full set of working features, but you
simply cannot mix and match the two.  Only one power management
interface can be in control of the machine at once.  Think about it..

User-space Daemons
------------------
Both APM and ACPI rely on user-space daemons, apmd and acpid
respectively, to be completely functional.  Obtain both of these
daemons from your Linux distribution or from the Internet (see below)
and be sure that they are started sometime in the system boot process.
Go ahead and start both.  If ACPI or APM is not available on your
system the associated daemon will exit gracefully.

  =====  =======================================
  apmd   http://ftp.debian.org/pool/main/a/apmd/
  acpid  http://acpid.sf.net/
  =====  =======================================
