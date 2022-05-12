.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_ENUMAUDIO:

**********************
ioctl VIDIOC_ENUMAUDIO
**********************

Name
====

VIDIOC_ENUMAUDIO - Enumerate audio inputs

Synopsis
========

.. c:macro:: VIDIOC_ENUMAUDIO

``int ioctl(int fd, VIDIOC_ENUMAUDIO, struct v4l2_audio *argp)``

Arguments
=========

``fd``
    File descriptor returned by :c:func:`open()`.

``argp``
    Pointer to struct :c:type:`v4l2_audio`.

Description
===========

To query the attributes of an audio input applications initialize the
``index`` field and zero out the ``reserved`` array of a struct
:c:type:`v4l2_audio` and call the :ref:`VIDIOC_ENUMAUDIO`
ioctl with a pointer to this structure. Drivers fill the rest of the
structure or return an ``EINVAL`` error code when the index is out of
bounds. To enumerate all audio inputs applications shall begin at index
zero, incrementing by one until the driver returns ``EINVAL``.

See :ref:`VIDIOC_G_AUDIO <VIDIOC_G_AUDIO>` for a description of struct
:c:type:`v4l2_audio`.

Return Value
============

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.

EINVAL
    The number of the audio input is out of bounds.
