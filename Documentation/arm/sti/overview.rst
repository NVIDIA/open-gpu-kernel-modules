======================
STi ARM Linux Overview
======================

Introduction
------------

  The ST Microelectronics Multimedia and Application Processors range of
  CortexA9 System-on-Chip are supported by the 'STi' platform of
  ARM Linux. Currently STiH415, STiH416 SOCs are supported with both
  B2000 and B2020 Reference boards.


configuration
-------------

  A generic configuration is provided for both STiH415/416, and can be used as the
  default by::

	make stih41x_defconfig

Layout
------

  All the files for multiple machine families (STiH415, STiH416, and STiG125)
  are located in the platform code contained in arch/arm/mach-sti

  There is a generic board board-dt.c in the mach folder which support
  Flattened Device Tree, which means, It works with any compatible board with
  Device Trees.


Document Author
---------------

  Srinivas Kandagatla <srinivas.kandagatla@st.com>, (c) 2013 ST Microelectronics
