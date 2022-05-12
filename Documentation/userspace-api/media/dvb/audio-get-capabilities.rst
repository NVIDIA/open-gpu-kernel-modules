.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: DTV.audio

.. _AUDIO_GET_CAPABILITIES:

======================
AUDIO_GET_CAPABILITIES
======================

Name
----

AUDIO_GET_CAPABILITIES

.. attention:: This ioctl is deprecated

Synopsis
--------

.. c:macro:: AUDIO_GET_CAPABILITIES

``int ioctl(int fd, AUDIO_GET_CAPABILITIES, unsigned int *cap)``

Arguments
---------

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -

       -  int fd

       -  File descriptor returned by a previous call to open().

    -

       -  unsigned int \*cap

       -  Returns a bit array of supported sound formats.

Description
-----------

This ioctl call asks the Audio Device to tell us about the decoding
capabilities of the audio hardware.

Return Value
------------

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
