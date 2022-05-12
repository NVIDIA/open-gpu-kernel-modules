.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_G_PRIORITY:

******************************************
ioctl VIDIOC_G_PRIORITY, VIDIOC_S_PRIORITY
******************************************

Name
====

VIDIOC_G_PRIORITY - VIDIOC_S_PRIORITY - Query or request the access priority associated with a file descriptor

Synopsis
========

.. c:macro:: VIDIOC_G_PRIORITY

``int ioctl(int fd, VIDIOC_G_PRIORITY, enum v4l2_priority *argp)``

.. c:macro:: VIDIOC_S_PRIORITY

``int ioctl(int fd, VIDIOC_S_PRIORITY, const enum v4l2_priority *argp)``

Arguments
=========

``fd``
    File descriptor returned by :c:func:`open()`.

``argp``
    Pointer to an enum :c:type:`v4l2_priority` type.

Description
===========

To query the current access priority applications call the
:ref:`VIDIOC_G_PRIORITY <VIDIOC_G_PRIORITY>` ioctl with a pointer to an enum v4l2_priority
variable where the driver stores the current priority.

To request an access priority applications store the desired priority in
an enum v4l2_priority variable and call :ref:`VIDIOC_S_PRIORITY <VIDIOC_G_PRIORITY>` ioctl
with a pointer to this variable.

.. c:type:: v4l2_priority

.. tabularcolumns:: |p{6.6cm}|p{2.2cm}|p{8.5cm}|

.. flat-table:: enum v4l2_priority
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 1 4

    * - ``V4L2_PRIORITY_UNSET``
      - 0
      -
    * - ``V4L2_PRIORITY_BACKGROUND``
      - 1
      - Lowest priority, usually applications running in background, for
	example monitoring VBI transmissions. A proxy application running
	in user space will be necessary if multiple applications want to
	read from a device at this priority.
    * - ``V4L2_PRIORITY_INTERACTIVE``
      - 2
      -
    * - ``V4L2_PRIORITY_DEFAULT``
      - 2
      - Medium priority, usually applications started and interactively
	controlled by the user. For example TV viewers, Teletext browsers,
	or just "panel" applications to change the channel or video
	controls. This is the default priority unless an application
	requests another.
    * - ``V4L2_PRIORITY_RECORD``
      - 3
      - Highest priority. Only one file descriptor can have this priority,
	it blocks any other fd from changing device properties. Usually
	applications which must not be interrupted, like video recording.

Return Value
============

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.

EINVAL
    The requested priority value is invalid.

EBUSY
    Another application already requested higher priority.
