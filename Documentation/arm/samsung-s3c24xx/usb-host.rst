========================
S3C24XX USB Host support
========================



Introduction
------------

  This document details the S3C2410/S3C2440 in-built OHCI USB host support.

Configuration
-------------

  Enable at least the following kernel options:

  menuconfig::

   Device Drivers  --->
     USB support  --->
       <*> Support for Host-side USB
       <*>   OHCI HCD support


  .config:

    - CONFIG_USB
    - CONFIG_USB_OHCI_HCD


  Once these options are configured, the standard set of USB device
  drivers can be configured and used.


Board Support
-------------

  The driver attaches to a platform device, which will need to be
  added by the board specific support file in arch/arm/mach-s3c,
  such as mach-bast.c or mach-smdk2410.c

  The platform device's platform_data field is only needed if the
  board implements extra power control or over-current monitoring.

  The OHCI driver does not ensure the state of the S3C2410's MISCCTRL
  register, so if both ports are to be used for the host, then it is
  the board support file's responsibility to ensure that the second
  port is configured to be connected to the OHCI core.


Platform Data
-------------

  See include/linux/platform_data/usb-ohci-s3c2410.h for the
  descriptions of the platform device data. An implementation
  can be found in arch/arm/mach-s3c/simtec-usb.c .

  The `struct s3c2410_hcd_info` contains a pair of functions
  that get called to enable over-current detection, and to
  control the port power status.

  The ports are numbered 0 and 1.

  power_control:
    Called to enable or disable the power on the port.

  enable_oc:
    Called to enable or disable the over-current monitoring.
    This should claim or release the resources being used to
    check the power condition on the port, such as an IRQ.

  report_oc:
    The OHCI driver fills this field in for the over-current code
    to call when there is a change to the over-current state on
    an port. The ports argument is a bitmask of 1 bit per port,
    with bit X being 1 for an over-current on port X.

    The function s3c2410_usb_report_oc() has been provided to
    ensure this is called correctly.

  port[x]:
    This is struct describes each port, 0 or 1. The platform driver
    should set the flags field of each port to S3C_HCDFLG_USED if
    the port is enabled.



Document Author
---------------

Ben Dooks, Copyright 2005 Simtec Electronics
