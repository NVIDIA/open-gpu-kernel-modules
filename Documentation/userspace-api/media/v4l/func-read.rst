.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _func-read:

***********
V4L2 read()
***********

Name
====

v4l2-read - Read from a V4L2 device

Synopsis
========

.. code-block:: c

    #include <unistd.h>

.. c:function:: ssize_t read( int fd, void *buf, size_t count )

Arguments
=========

``fd``
    File descriptor returned by :c:func:`open()`.

``buf``
   Buffer to be filled

``count``
  Max number of bytes to read

Description
===========

:c:func:`read()` attempts to read up to ``count`` bytes from file
descriptor ``fd`` into the buffer starting at ``buf``. The layout of the
data in the buffer is discussed in the respective device interface
section, see ##. If ``count`` is zero, :c:func:`read()` returns zero
and has no other results. If ``count`` is greater than ``SSIZE_MAX``,
the result is unspecified. Regardless of the ``count`` value each
:c:func:`read()` call will provide at most one frame (two fields)
worth of data.

By default :c:func:`read()` blocks until data becomes available. When
the ``O_NONBLOCK`` flag was given to the :c:func:`open()`
function it returns immediately with an ``EAGAIN`` error code when no data
is available. The :c:func:`select()` or
:c:func:`poll()` functions can always be used to suspend
execution until data becomes available. All drivers supporting the
:c:func:`read()` function must also support :c:func:`select()` and
:c:func:`poll()`.

Drivers can implement read functionality in different ways, using a
single or multiple buffers and discarding the oldest or newest frames
once the internal buffers are filled.

:c:func:`read()` never returns a "snapshot" of a buffer being filled.
Using a single buffer the driver will stop capturing when the
application starts reading the buffer until the read is finished. Thus
only the period of the vertical blanking interval is available for
reading, or the capture rate must fall below the nominal frame rate of
the video standard.

The behavior of :c:func:`read()` when called during the active picture
period or the vertical blanking separating the top and bottom field
depends on the discarding policy. A driver discarding the oldest frames
keeps capturing into an internal buffer, continuously overwriting the
previously, not read frame, and returns the frame being received at the
time of the :c:func:`read()` call as soon as it is complete.

A driver discarding the newest frames stops capturing until the next
:c:func:`read()` call. The frame being received at :c:func:`read()`
time is discarded, returning the following frame instead. Again this
implies a reduction of the capture rate to one half or less of the
nominal frame rate. An example of this model is the video read mode of
the bttv driver, initiating a DMA to user memory when :c:func:`read()`
is called and returning when the DMA finished.

In the multiple buffer model drivers maintain a ring of internal
buffers, automatically advancing to the next free buffer. This allows
continuous capturing when the application can empty the buffers fast
enough. Again, the behavior when the driver runs out of free buffers
depends on the discarding policy.

Applications can get and set the number of buffers used internally by
the driver with the :ref:`VIDIOC_G_PARM <VIDIOC_G_PARM>` and
:ref:`VIDIOC_S_PARM <VIDIOC_G_PARM>` ioctls. They are optional,
however. The discarding policy is not reported and cannot be changed.
For minimum requirements see :ref:`devices`.

Return Value
============

On success, the number of bytes read is returned. It is not an error if
this number is smaller than the number of bytes requested, or the amount
of data required for one frame. This may happen for example because
:c:func:`read()` was interrupted by a signal. On error, -1 is
returned, and the ``errno`` variable is set appropriately. In this case
the next read will start at the beginning of a new frame. Possible error
codes are:

EAGAIN
    Non-blocking I/O has been selected using O_NONBLOCK and no data was
    immediately available for reading.

EBADF
    ``fd`` is not a valid file descriptor or is not open for reading, or
    the process already has the maximum number of files open.

EBUSY
    The driver does not support multiple read streams and the device is
    already in use.

EFAULT
    ``buf`` references an inaccessible memory area.

EINTR
    The call was interrupted by a signal before any data was read.

EIO
    I/O error. This indicates some hardware problem or a failure to
    communicate with a remote device (USB camera etc.).

EINVAL
    The :c:func:`read()` function is not supported by this driver, not
    on this device, or generally not on this type of device.
