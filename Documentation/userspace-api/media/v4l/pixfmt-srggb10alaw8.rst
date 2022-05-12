.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later

.. _V4L2-PIX-FMT-SBGGR10ALAW8:
.. _v4l2-pix-fmt-sgbrg10alaw8:
.. _v4l2-pix-fmt-sgrbg10alaw8:
.. _v4l2-pix-fmt-srggb10alaw8:

***********************************************************************************************************************************************
V4L2_PIX_FMT_SBGGR10ALAW8 ('aBA8'), V4L2_PIX_FMT_SGBRG10ALAW8 ('aGA8'), V4L2_PIX_FMT_SGRBG10ALAW8 ('agA8'), V4L2_PIX_FMT_SRGGB10ALAW8 ('aRA8'),
***********************************************************************************************************************************************

V4L2_PIX_FMT_SGBRG10ALAW8
V4L2_PIX_FMT_SGRBG10ALAW8
V4L2_PIX_FMT_SRGGB10ALAW8
10-bit Bayer formats compressed to 8 bits


Description
===========

These four pixel formats are raw sRGB / Bayer formats with 10 bits per
color compressed to 8 bits each, using the A-LAW algorithm. Each color
component consumes 8 bits of memory. In other respects this format is
similar to :ref:`V4L2-PIX-FMT-SRGGB8`.
