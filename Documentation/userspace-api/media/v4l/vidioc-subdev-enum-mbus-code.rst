.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_SUBDEV_ENUM_MBUS_CODE:

**********************************
ioctl VIDIOC_SUBDEV_ENUM_MBUS_CODE
**********************************

Name
====

VIDIOC_SUBDEV_ENUM_MBUS_CODE - Enumerate media bus formats

Synopsis
========

.. c:macro:: VIDIOC_SUBDEV_ENUM_MBUS_CODE

``int ioctl(int fd, VIDIOC_SUBDEV_ENUM_MBUS_CODE, struct v4l2_subdev_mbus_code_enum * argp)``

Arguments
=========

``fd``
    File descriptor returned by :c:func:`open()`.

``argp``
    Pointer to struct :c:type:`v4l2_subdev_mbus_code_enum`.

Description
===========

To enumerate media bus formats available at a given sub-device pad
applications initialize the ``pad``, ``which`` and ``index`` fields of
struct
:c:type:`v4l2_subdev_mbus_code_enum` and
call the :ref:`VIDIOC_SUBDEV_ENUM_MBUS_CODE` ioctl with a pointer to this
structure. Drivers fill the rest of the structure or return an ``EINVAL``
error code if either the ``pad`` or ``index`` are invalid. All media bus
formats are enumerable by beginning at index zero and incrementing by
one until ``EINVAL`` is returned.

Available media bus formats may depend on the current 'try' formats at
other pads of the sub-device, as well as on the current active links.
See :ref:`VIDIOC_SUBDEV_G_FMT` for more
information about the try formats.

.. c:type:: v4l2_subdev_mbus_code_enum

.. tabularcolumns:: |p{4.4cm}|p{4.4cm}|p{8.5cm}|

.. flat-table:: struct v4l2_subdev_mbus_code_enum
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``pad``
      - Pad number as reported by the media controller API.
    * - __u32
      - ``index``
      - Number of the format in the enumeration, set by the application.
    * - __u32
      - ``code``
      - The media bus format code, as defined in
	:ref:`v4l2-mbus-format`.
    * - __u32
      - ``which``
      - Media bus format codes to be enumerated, from enum
	:ref:`v4l2_subdev_format_whence <v4l2-subdev-format-whence>`.
    * - __u32
      - ``flags``
      - See :ref:`v4l2-subdev-mbus-code-flags`
    * - __u32
      - ``reserved``\ [7]
      - Reserved for future extensions. Applications and drivers must set
	the array to zero.



.. raw:: latex

   \footnotesize

.. tabularcolumns:: |p{8.8cm}|p{2.2cm}|p{6.3cm}|

.. _v4l2-subdev-mbus-code-flags:

.. flat-table:: Subdev Media Bus Code Enumerate Flags
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - V4L2_SUBDEV_MBUS_CODE_CSC_COLORSPACE
      - 0x00000001
      - The driver allows the application to try to change the default colorspace
	encoding. The application can ask to configure the colorspace of the
	subdevice when calling the :ref:`VIDIOC_SUBDEV_S_FMT <VIDIOC_SUBDEV_G_FMT>`
	ioctl with :ref:`V4L2_MBUS_FRAMEFMT_SET_CSC <mbus-framefmt-set-csc>` set.
	See :ref:`v4l2-mbus-format` on how to do this.
    * - V4L2_SUBDEV_MBUS_CODE_CSC_XFER_FUNC
      - 0x00000002
      - The driver allows the application to try to change the default transform function.
	The application can ask to configure the transform function of
	the subdevice when calling the :ref:`VIDIOC_SUBDEV_S_FMT <VIDIOC_SUBDEV_G_FMT>`
	ioctl with :ref:`V4L2_MBUS_FRAMEFMT_SET_CSC <mbus-framefmt-set-csc>` set.
	See :ref:`v4l2-mbus-format` on how to do this.
    * - V4L2_SUBDEV_MBUS_CODE_CSC_YCBCR_ENC
      - 0x00000004
      - The driver allows the application to try to change the default Y'CbCr
	encoding. The application can ask to configure the Y'CbCr encoding of the
	subdevice when calling the :ref:`VIDIOC_SUBDEV_S_FMT <VIDIOC_SUBDEV_G_FMT>`
	ioctl with :ref:`V4L2_MBUS_FRAMEFMT_SET_CSC <mbus-framefmt-set-csc>` set.
	See :ref:`v4l2-mbus-format` on how to do this.
    * - V4L2_SUBDEV_MBUS_CODE_CSC_HSV_ENC
      - 0x00000004
      - The driver allows the application to try to change the default HSV
	encoding. The application can ask to configure the HSV encoding of the
	subdevice when calling the :ref:`VIDIOC_SUBDEV_S_FMT <VIDIOC_SUBDEV_G_FMT>`
	ioctl with :ref:`V4L2_MBUS_FRAMEFMT_SET_CSC <mbus-framefmt-set-csc>` set.
	See :ref:`v4l2-mbus-format` on how to do this.
    * - V4L2_SUBDEV_MBUS_CODE_CSC_QUANTIZATION
      - 0x00000008
      - The driver allows the application to try to change the default
	quantization. The application can ask to configure the quantization of
	the subdevice when calling the :ref:`VIDIOC_SUBDEV_S_FMT <VIDIOC_SUBDEV_G_FMT>`
	ioctl with :ref:`V4L2_MBUS_FRAMEFMT_SET_CSC <mbus-framefmt-set-csc>` set.
	See :ref:`v4l2-mbus-format` on how to do this.

.. raw:: latex

   \normalsize

Return Value
============

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.

EINVAL
    The struct
    :c:type:`v4l2_subdev_mbus_code_enum`
    ``pad`` references a non-existing pad, or the ``index`` field is out
    of bounds.
