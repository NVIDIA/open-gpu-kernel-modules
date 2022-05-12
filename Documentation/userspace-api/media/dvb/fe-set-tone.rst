.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: DTV.fe

.. _FE_SET_TONE:

*****************
ioctl FE_SET_TONE
*****************

Name
====

FE_SET_TONE - Sets/resets the generation of the continuous 22kHz tone.

Synopsis
========

.. c:macro:: FE_SET_TONE

``int ioctl(int fd, FE_SET_TONE, enum fe_sec_tone_mode tone)``

Arguments
=========

``fd``
    File descriptor returned by :c:func:`open()`.

``tone``
    an integer enumered value described at :c:type:`fe_sec_tone_mode`

Description
===========

This ioctl is used to set the generation of the continuous 22kHz tone.
This call requires read/write permissions.

Usually, satellite antenna subsystems require that the digital TV device
to send a 22kHz tone in order to select between high/low band on some
dual-band LNBf. It is also used to send signals to DiSEqC equipment, but
this is done using the DiSEqC ioctls.

.. attention:: If more than one device is connected to the same antenna,
   setting a tone may interfere on other devices, as they may lose the
   capability of selecting the band. So, it is recommended that applications
   would change to SEC_TONE_OFF when the device is not used.

Return Value
============

On success 0 is returned.

On error -1 is returned, and the ``errno`` variable is set
appropriately.

Generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
