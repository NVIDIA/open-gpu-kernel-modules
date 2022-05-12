.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: DTV.audio

.. _AUDIO_SET_MUTE:

==============
AUDIO_SET_MUTE
==============

Name
----

AUDIO_SET_MUTE

.. attention:: This ioctl is deprecated

Synopsis
--------

.. c:macro:: AUDIO_SET_MUTE

``int ioctl(int fd, AUDIO_SET_MUTE, boolean state)``

Arguments
---------

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -

       -  int fd

       -  File descriptor returned by a previous call to open().

    -

       -  boolean state

       -  Indicates if audio device shall mute or not.

          TRUE: Audio Mute

          FALSE: Audio Un-mute

Description
-----------

This ioctl is for Digital TV devices only. To control a V4L2 decoder use the
V4L2 :ref:`VIDIOC_DECODER_CMD` with the
``V4L2_DEC_CMD_START_MUTE_AUDIO`` flag instead.

This ioctl call asks the audio device to mute the stream that is
currently being played.

Return Value
------------

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
