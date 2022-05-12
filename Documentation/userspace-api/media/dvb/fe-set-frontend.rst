.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: DTV.fe

.. _FE_SET_FRONTEND:

***************
FE_SET_FRONTEND
***************

.. attention:: This ioctl is deprecated.

Name
====

FE_SET_FRONTEND

Synopsis
========

.. c:macro:: FE_SET_FRONTEND

``int ioctl(int fd, FE_SET_FRONTEND, struct dvb_frontend_parameters *p)``

Arguments
=========

``fd``
    File descriptor returned by :c:func:`open()`.

``p``
    Points to parameters for tuning operation.

Description
===========

This ioctl call starts a tuning operation using specified parameters.
The result of this call will be successful if the parameters were valid
and the tuning could be initiated. The result of the tuning operation in
itself, however, will arrive asynchronously as an event (see
documentation for :ref:`FE_GET_EVENT` and
FrontendEvent.) If a new :ref:`FE_SET_FRONTEND`
operation is initiated before the previous one was completed, the
previous operation will be aborted in favor of the new one. This command
requires read/write access to the device.

Return Value
============

On success 0 is returned.

On error -1 is returned, and the ``errno`` variable is set
appropriately.

.. tabularcolumns:: |p{2.5cm}|p{15.0cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths: 1 16

    -  .. row 1

       -  ``EINVAL``

       -  Maximum supported symbol rate reached.

Generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
