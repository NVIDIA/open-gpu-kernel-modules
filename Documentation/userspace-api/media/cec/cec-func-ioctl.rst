.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: CEC

.. _cec-func-ioctl:

***********
cec ioctl()
***********

Name
====

cec-ioctl - Control a cec device

Synopsis
========

.. code-block:: c

    #include <sys/ioctl.h>

``int ioctl(int fd, int request, void *argp)``

Arguments
=========

``fd``
    File descriptor returned by :c:func:`open()`.

``request``
    CEC ioctl request code as defined in the cec.h header file, for
    example :ref:`CEC_ADAP_G_CAPS <CEC_ADAP_G_CAPS>`.

``argp``
    Pointer to a request-specific structure.

Description
===========

The :c:func:`ioctl()` function manipulates cec device parameters. The
argument ``fd`` must be an open file descriptor.

The ioctl ``request`` code specifies the cec function to be called. It
has encoded in it whether the argument is an input, output or read/write
parameter, and the size of the argument ``argp`` in bytes.

Macros and structures definitions specifying cec ioctl requests and
their parameters are located in the cec.h header file. All cec ioctl
requests, their respective function and parameters are specified in
:ref:`cec-user-func`.

Return Value
============

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.

Request-specific error codes are listed in the individual requests
descriptions.

When an ioctl that takes an output or read/write parameter fails, the
parameter remains unmodified.
