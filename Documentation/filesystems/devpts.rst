.. SPDX-License-Identifier: GPL-2.0

=====================
The Devpts Filesystem
=====================

Each mount of the devpts filesystem is now distinct such that ptys
and their indicies allocated in one mount are independent from ptys
and their indicies in all other mounts.

All mounts of the devpts filesystem now create a ``/dev/pts/ptmx`` node
with permissions ``0000``.

To retain backwards compatibility the a ptmx device node (aka any node
created with ``mknod name c 5 2``) when opened will look for an instance
of devpts under the name ``pts`` in the same directory as the ptmx device
node.

As an option instead of placing a ``/dev/ptmx`` device node at ``/dev/ptmx``
it is possible to place a symlink to ``/dev/pts/ptmx`` at ``/dev/ptmx`` or
to bind mount ``/dev/ptx/ptmx`` to ``/dev/ptmx``.  If you opt for using
the devpts filesystem in this manner devpts should be mounted with
the ``ptmxmode=0666``, or ``chmod 0666 /dev/pts/ptmx`` should be called.

Total count of pty pairs in all instances is limited by sysctls::

    kernel.pty.max = 4096	- global limit
    kernel.pty.reserve = 1024	- reserved for filesystems mounted from the initial mount namespace
    kernel.pty.nr		- current count of ptys

Per-instance limit could be set by adding mount option ``max=<count>``.

This feature was added in kernel 3.4 together with
``sysctl kernel.pty.reserve``.

In kernels older than 3.4 sysctl ``kernel.pty.max`` works as per-instance limit.
