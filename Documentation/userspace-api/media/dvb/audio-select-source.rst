.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: DTV.audio

.. _AUDIO_SELECT_SOURCE:

===================
AUDIO_SELECT_SOURCE
===================

Name
----

AUDIO_SELECT_SOURCE

.. attention:: This ioctl is deprecated

Synopsis
--------

.. c:macro:: AUDIO_SELECT_SOURCE

``int ioctl(int fd, AUDIO_SELECT_SOURCE, struct audio_stream_source *source)``

Arguments
---------

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -

       -  int fd

       -  File descriptor returned by a previous call to open().

    -

       -  audio_stream_source_t source

       -  Indicates the source that shall be used for the Audio stream.

Description
-----------

This ioctl call informs the audio device which source shall be used for
the input data. The possible sources are demux or memory. If
AUDIO_SOURCE_MEMORY is selected, the data is fed to the Audio Device
through the write command.

Return Value
------------

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
