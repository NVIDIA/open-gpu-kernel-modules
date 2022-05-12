================
mtouchusb driver
================

Changes
=======

- 0.3 - Created based off of scanner & INSTALL from the original touchscreen
  driver on freecode (http://freecode.com/projects/3mtouchscreendriver)
- Amended for linux-2.4.18, then 2.4.19

- 0.5 - Complete rewrite using Linux Input in 2.6.3
  Unfortunately no calibration support at this time

- 1.4 - Multiple changes to support the EXII 5000UC and house cleaning
  Changed reset from standard USB dev reset to vendor reset
  Changed data sent to host from compensated to raw coordinates
  Eliminated vendor/product module params
  Performed multiple successful tests with an EXII-5010UC

Supported Hardware
==================

::

        All controllers have the Vendor: 0x0596 & Product: 0x0001


        Controller Description          Part Number
        ------------------------------------------------------

        USB Capacitive - Pearl Case     14-205  (Discontinued)
        USB Capacitive - Black Case     14-124  (Discontinued)
        USB Capacitive - No Case        14-206  (Discontinued)

        USB Capacitive - Pearl Case     EXII-5010UC
        USB Capacitive - Black Case     EXII-5030UC
        USB Capacitive - No Case        EXII-5050UC

Driver Notes
============

Installation is simple, you only need to add Linux Input, Linux USB, and the
driver to the kernel.  The driver can also be optionally built as a module.

This driver appears to be one of possible 2 Linux USB Input Touchscreen
drivers.  Although 3M produces a binary only driver available for
download, I persist in updating this driver since I would like to use the
touchscreen for embedded apps using QTEmbedded, DirectFB, etc. So I feel the
logical choice is to use Linux Input.

Currently there is no way to calibrate the device via this driver.  Even if
the device could be calibrated, the driver pulls to raw coordinate data from
the controller.  This means calibration must be performed within the
userspace.

The controller screen resolution is now 0 to 16384 for both X and Y reporting
the raw touch data.  This is the same for the old and new capacitive USB
controllers.

Perhaps at some point an abstract function will be placed into evdev so
generic functions like calibrations, resets, and vendor information can be
requested from the userspace (And the drivers would handle the vendor specific
tasks).

TODO
====

Implement a control urb again to handle requests to and from the device
such as calibration, etc once/if it becomes available.

Disclaimer
==========

I am not a MicroTouch/3M employee, nor have I ever been.  3M does not support
this driver!  If you want touch drivers only supported within X, please go to:

http://www.3m.com/3MTouchSystems/

Thanks
======

A huge thank you to 3M Touch Systems for the EXII-5010UC controllers for
testing!
