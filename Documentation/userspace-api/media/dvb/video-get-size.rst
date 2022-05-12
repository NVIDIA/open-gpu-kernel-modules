.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: DTV.video

.. _VIDEO_GET_SIZE:

==============
VIDEO_GET_SIZE
==============

Name
----

VIDEO_GET_SIZE

.. attention:: This ioctl is deprecated.

Synopsis
--------

.. c:macro:: VIDEO_GET_SIZE

``int ioctl(int fd, VIDEO_GET_SIZE, video_size_t *size)``

Arguments
---------

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  .. row 1

       -  int fd

       -  File descriptor returned by a previous call to open().

    -  .. row 2

       -  int request

       -  Equals VIDEO_GET_SIZE for this command.

    -  .. row 3

       -  video_size_t \*size

       -  Returns the size and aspect ratio.

Description
-----------

This ioctl returns the size and aspect ratio.

.. c:type:: video_size_t

.. code-block::c

	typedef struct {
		int w;
		int h;
		video_format_t aspect_ratio;
	} video_size_t;

Return Value
------------

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
