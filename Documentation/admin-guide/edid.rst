.. SPDX-License-Identifier: GPL-2.0

====
EDID
====

In the good old days when graphics parameters were configured explicitly
in a file called xorg.conf, even broken hardware could be managed.

Today, with the advent of Kernel Mode Setting, a graphics board is
either correctly working because all components follow the standards -
or the computer is unusable, because the screen remains dark after
booting or it displays the wrong area. Cases when this happens are:

- The graphics board does not recognize the monitor.
- The graphics board is unable to detect any EDID data.
- The graphics board incorrectly forwards EDID data to the driver.
- The monitor sends no or bogus EDID data.
- A KVM sends its own EDID data instead of querying the connected monitor.

Adding the kernel parameter "nomodeset" helps in most cases, but causes
restrictions later on.

As a remedy for such situations, the kernel configuration item
CONFIG_DRM_LOAD_EDID_FIRMWARE was introduced. It allows to provide an
individually prepared or corrected EDID data set in the /lib/firmware
directory from where it is loaded via the firmware interface. The code
(see drivers/gpu/drm/drm_edid_load.c) contains built-in data sets for
commonly used screen resolutions (800x600, 1024x768, 1280x1024, 1600x1200,
1680x1050, 1920x1080) as binary blobs, but the kernel source tree does
not contain code to create these data. In order to elucidate the origin
of the built-in binary EDID blobs and to facilitate the creation of
individual data for a specific misbehaving monitor, commented sources
and a Makefile environment are given here.

To create binary EDID and C source code files from the existing data
material, simply type "make" in tools/edid/.

If you want to create your own EDID file, copy the file 1024x768.S,
replace the settings with your own data and add a new target to the
Makefile. Please note that the EDID data structure expects the timing
values in a different way as compared to the standard X11 format.

X11:
  HTimings:
    hdisp hsyncstart hsyncend htotal
  VTimings:
    vdisp vsyncstart vsyncend vtotal

EDID::

  #define XPIX hdisp
  #define XBLANK htotal-hdisp
  #define XOFFSET hsyncstart-hdisp
  #define XPULSE hsyncend-hsyncstart

  #define YPIX vdisp
  #define YBLANK vtotal-vdisp
  #define YOFFSET vsyncstart-vdisp
  #define YPULSE vsyncend-vsyncstart
