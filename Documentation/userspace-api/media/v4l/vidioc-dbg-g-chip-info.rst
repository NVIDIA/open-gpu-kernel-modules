.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_DBG_G_CHIP_INFO:

****************************
ioctl VIDIOC_DBG_G_CHIP_INFO
****************************

Name
====

VIDIOC_DBG_G_CHIP_INFO - Identify the chips on a TV card

Synopsis
========

.. c:macro:: VIDIOC_DBG_G_CHIP_INFO

``int ioctl(int fd, VIDIOC_DBG_G_CHIP_INFO, struct v4l2_dbg_chip_info *argp)``

Arguments
=========

``fd``
    File descriptor returned by :c:func:`open()`.

``argp``
    Pointer to struct :c:type:`v4l2_dbg_chip_info`.

Description
===========

.. note::

    This is an :ref:`experimental` interface and may
    change in the future.

For driver debugging purposes this ioctl allows test applications to
query the driver about the chips present on the TV card. Regular
applications must not use it. When you found a chip specific bug, please
contact the linux-media mailing list
(`https://linuxtv.org/lists.php <https://linuxtv.org/lists.php>`__)
so it can be fixed.

Additionally the Linux kernel must be compiled with the
``CONFIG_VIDEO_ADV_DEBUG`` option to enable this ioctl.

To query the driver applications must initialize the ``match.type`` and
``match.addr`` or ``match.name`` fields of a struct
:c:type:`v4l2_dbg_chip_info` and call
:ref:`VIDIOC_DBG_G_CHIP_INFO` with a pointer to this structure. On success
the driver stores information about the selected chip in the ``name``
and ``flags`` fields.

When ``match.type`` is ``V4L2_CHIP_MATCH_BRIDGE``, ``match.addr``
selects the nth bridge 'chip' on the TV card. You can enumerate all
chips by starting at zero and incrementing ``match.addr`` by one until
:ref:`VIDIOC_DBG_G_CHIP_INFO` fails with an ``EINVAL`` error code. The number
zero always selects the bridge chip itself, e. g. the chip connected to
the PCI or USB bus. Non-zero numbers identify specific parts of the
bridge chip such as an AC97 register block.

When ``match.type`` is ``V4L2_CHIP_MATCH_SUBDEV``, ``match.addr``
selects the nth sub-device. This allows you to enumerate over all
sub-devices.

On success, the ``name`` field will contain a chip name and the
``flags`` field will contain ``V4L2_CHIP_FL_READABLE`` if the driver
supports reading registers from the device or ``V4L2_CHIP_FL_WRITABLE``
if the driver supports writing registers to the device.

We recommended the v4l2-dbg utility over calling this ioctl directly. It
is available from the LinuxTV v4l-dvb repository; see
`https://linuxtv.org/repo/ <https://linuxtv.org/repo/>`__ for access
instructions.

.. tabularcolumns:: |p{3.5cm}|p{3.5cm}|p{3.5cm}|p{6.6cm}|

.. _name-v4l2-dbg-match:

.. flat-table:: struct v4l2_dbg_match
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``type``
      - See :ref:`name-chip-match-types` for a list of possible types.
    * - union {
      - (anonymous)
    * - __u32
      - ``addr``
      - Match a chip by this number, interpreted according to the ``type``
	field.
    * - char
      - ``name[32]``
      - Match a chip by this name, interpreted according to the ``type``
	field. Currently unused.
    * - }
      -


.. tabularcolumns:: |p{4.4cm}|p{4.4cm}|p{8.5cm}|

.. c:type:: v4l2_dbg_chip_info

.. flat-table:: struct v4l2_dbg_chip_info
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - struct v4l2_dbg_match
      - ``match``
      - How to match the chip, see :ref:`name-v4l2-dbg-match`.
    * - char
      - ``name[32]``
      - The name of the chip.
    * - __u32
      - ``flags``
      - Set by the driver. If ``V4L2_CHIP_FL_READABLE`` is set, then the
	driver supports reading registers from the device. If
	``V4L2_CHIP_FL_WRITABLE`` is set, then it supports writing
	registers.
    * - __u32
      - ``reserved[8]``
      - Reserved fields, both application and driver must set these to 0.


.. tabularcolumns:: |p{6.6cm}|p{2.2cm}|p{8.5cm}|

.. _name-chip-match-types:

.. flat-table:: Chip Match Types
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 1 4

    * - ``V4L2_CHIP_MATCH_BRIDGE``
      - 0
      - Match the nth chip on the card, zero for the bridge chip. Does not
	match sub-devices.
    * - ``V4L2_CHIP_MATCH_SUBDEV``
      - 4
      - Match the nth sub-device.

Return Value
============

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.

EINVAL
    The ``match_type`` is invalid or no device could be matched.
