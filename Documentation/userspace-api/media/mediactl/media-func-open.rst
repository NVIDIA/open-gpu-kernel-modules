.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: MC

.. _media-func-open:

************
media open()
************

Name
====

media-open - Open a media device

Synopsis
========

.. code-block:: c

    #include <fcntl.h>

.. c:function:: int open( const char *device_name, int flags )

Arguments
=========

``device_name``
    Device to be opened.

``flags``
    Open flags. Access mode must be either ``O_RDONLY`` or ``O_RDWR``.
    Other flags have no effect.

Description
===========

To open a media device applications call :c:func:`open()` with the
desired device name. The function has no side effects; the device
configuration remain unchanged.

When the device is opened in read-only mode, attempts to modify its
configuration will result in an error, and ``errno`` will be set to
EBADF.

Return Value
============

:c:func:`open()` returns the new file descriptor on success. On error,
-1 is returned, and ``errno`` is set appropriately. Possible error codes
are:

EACCES
    The requested access to the file is not allowed.

EMFILE
    The process already has the maximum number of files open.

ENFILE
    The system limit on the total number of open files has been reached.

ENOMEM
    Insufficient kernel memory was available.

ENXIO
    No device corresponding to this device special file exists.
