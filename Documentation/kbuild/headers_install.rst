=============================================
Exporting kernel headers for use by userspace
=============================================

The "make headers_install" command exports the kernel's header files in a
form suitable for use by userspace programs.

The linux kernel's exported header files describe the API for user space
programs attempting to use kernel services.  These kernel header files are
used by the system's C library (such as glibc or uClibc) to define available
system calls, as well as constants and structures to be used with these
system calls.  The C library's header files include the kernel header files
from the "linux" subdirectory.  The system's libc headers are usually
installed at the default location /usr/include and the kernel headers in
subdirectories under that (most notably /usr/include/linux and
/usr/include/asm).

Kernel headers are backwards compatible, but not forwards compatible.  This
means that a program built against a C library using older kernel headers
should run on a newer kernel (although it may not have access to new
features), but a program built against newer kernel headers may not work on an
older kernel.

The "make headers_install" command can be run in the top level directory of the
kernel source code (or using a standard out-of-tree build).  It takes two
optional arguments::

  make headers_install ARCH=i386 INSTALL_HDR_PATH=/usr

ARCH indicates which architecture to produce headers for, and defaults to the
current architecture.  The linux/asm directory of the exported kernel headers
is platform-specific, to see a complete list of supported architectures use
the command::

  ls -d include/asm-* | sed 's/.*-//'

INSTALL_HDR_PATH indicates where to install the headers. It defaults to
"./usr".

An 'include' directory is automatically created inside INSTALL_HDR_PATH and
headers are installed in 'INSTALL_HDR_PATH/include'.

The kernel header export infrastructure is maintained by David Woodhouse
<dwmw2@infradead.org>.
