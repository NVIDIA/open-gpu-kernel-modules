.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _output:

**********************
Video Output Interface
**********************

Video output devices encode stills or image sequences as analog video
signal. With this interface applications can control the encoding
process and move images from user space to the driver.

Conventionally V4L2 video output devices are accessed through character
device special files named ``/dev/video`` and ``/dev/video0`` to
``/dev/video63`` with major number 81 and minor numbers 0 to 63.
``/dev/video`` is typically a symbolic link to the preferred video
device.

.. note:: The same device file names are used also for video capture devices.

Querying Capabilities
=====================

Devices supporting the video output interface set the
``V4L2_CAP_VIDEO_OUTPUT`` or ``V4L2_CAP_VIDEO_OUTPUT_MPLANE`` flag in
the ``capabilities`` field of struct
:c:type:`v4l2_capability` returned by the
:ref:`VIDIOC_QUERYCAP` ioctl. As secondary device
functions they may also support the :ref:`raw VBI output <raw-vbi>`
(``V4L2_CAP_VBI_OUTPUT``) interface. At least one of the read/write or
streaming I/O methods must be supported. Modulators and audio outputs
are optional.

Supplemental Functions
======================

Video output devices shall support :ref:`audio output <audio>`,
:ref:`modulator <tuner>`, :ref:`controls <control>`,
:ref:`cropping and scaling <crop>` and
:ref:`streaming parameter <streaming-par>` ioctls as needed. The
:ref:`video output <video>` ioctls must be supported by all video
output devices.

Image Format Negotiation
========================

The output is determined by cropping and image format parameters. The
former select an area of the video picture where the image will appear,
the latter how images are stored in memory, i. e. in RGB or YUV format,
the number of bits per pixel or width and height. Together they also
define how images are scaled in the process.

As usual these parameters are *not* reset at :c:func:`open()`
time to permit Unix tool chains, programming a device and then writing
to it as if it was a plain file. Well written V4L2 applications ensure
they really get what they want, including cropping and scaling.

Cropping initialization at minimum requires to reset the parameters to
defaults. An example is given in :ref:`crop`.

To query the current image format applications set the ``type`` field of
a struct :c:type:`v4l2_format` to
``V4L2_BUF_TYPE_VIDEO_OUTPUT`` or ``V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE``
and call the :ref:`VIDIOC_G_FMT <VIDIOC_G_FMT>` ioctl with a pointer
to this structure. Drivers fill the struct
:c:type:`v4l2_pix_format` ``pix`` or the struct
:c:type:`v4l2_pix_format_mplane` ``pix_mp``
member of the ``fmt`` union.

To request different parameters applications set the ``type`` field of a
struct :c:type:`v4l2_format` as above and initialize all
fields of the struct :c:type:`v4l2_pix_format`
``vbi`` member of the ``fmt`` union, or better just modify the results
of :ref:`VIDIOC_G_FMT <VIDIOC_G_FMT>`, and call the :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>`
ioctl with a pointer to this structure. Drivers may adjust the
parameters and finally return the actual parameters as :ref:`VIDIOC_G_FMT <VIDIOC_G_FMT>`
does.

Like :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` the :ref:`VIDIOC_TRY_FMT <VIDIOC_G_FMT>` ioctl
can be used to learn about hardware limitations without disabling I/O or
possibly time consuming hardware preparations.

The contents of struct :c:type:`v4l2_pix_format` and
struct :c:type:`v4l2_pix_format_mplane` are
discussed in :ref:`pixfmt`. See also the specification of the
:ref:`VIDIOC_G_FMT <VIDIOC_G_FMT>`, :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` and :ref:`VIDIOC_TRY_FMT <VIDIOC_G_FMT>` ioctls for
details. Video output devices must implement both the :ref:`VIDIOC_G_FMT <VIDIOC_G_FMT>`
and :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl, even if :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ignores all
requests and always returns default parameters as :ref:`VIDIOC_G_FMT <VIDIOC_G_FMT>` does.
:ref:`VIDIOC_TRY_FMT <VIDIOC_G_FMT>` is optional.

Writing Images
==============

A video output device may support the :ref:`write() function <rw>`
and/or streaming (:ref:`memory mapping <mmap>` or
:ref:`user pointer <userp>`) I/O. See :ref:`io` for details.
