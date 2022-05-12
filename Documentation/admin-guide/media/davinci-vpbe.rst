.. SPDX-License-Identifier: GPL-2.0

The VPBE V4L2 driver design
===========================

Functional partitioning
-----------------------

Consists of the following:

 1. V4L2 display driver

    Implements creation of video2 and video3 device nodes and
    provides v4l2 device interface to manage VID0 and VID1 layers.

 2. Display controller

    Loads up VENC, OSD and external encoders such as ths8200. It provides
    a set of API calls to V4L2 drivers to set the output/standards
    in the VENC or external sub devices. It also provides
    a device object to access the services from OSD subdevice
    using sub device ops. The connection of external encoders to VENC LCD
    controller port is done at init time based on default output and standard
    selection or at run time when application change the output through
    V4L2 IOCTLs.

    When connected to an external encoder, vpbe controller is also responsible
    for setting up the interface between VENC and external encoders based on
    board specific settings (specified in board-xxx-evm.c). This allows
    interfacing external encoders such as ths8200. The setup_if_config()
    is implemented for this as well as configure_venc() (part of the next patch)
    API to set timings in VENC for a specific display resolution. As of this
    patch series, the interconnection and enabling and setting of the external
    encoders is not present, and would be a part of the next patch series.

 3. VENC subdevice module

    Responsible for setting outputs provided through internal DACs and also
    setting timings at LCD controller port when external encoders are connected
    at the port or LCD panel timings required. When external encoder/LCD panel
    is connected, the timings for a specific standard/preset is retrieved from
    the board specific table and the values are used to set the timings in
    venc using non-standard timing mode.

    Support LCD Panel displays using the VENC. For example to support a Logic
    PD display, it requires setting up the LCD controller port with a set of
    timings for the resolution supported and setting the dot clock. So we could
    add the available outputs as a board specific entry (i.e add the "LogicPD"
    output name to board-xxx-evm.c). A table of timings for various LCDs
    supported can be maintained in the board specific setup file to support
    various LCD displays.As of this patch a basic driver is present, and this
    support for external encoders and displays forms a part of the next
    patch series.

 4. OSD module

    OSD module implements all OSD layer management and hardware specific
    features. The VPBE module interacts with the OSD for enabling and
    disabling appropriate features of the OSD.

Current status
--------------

A fully functional working version of the V4L2 driver is available. This
driver has been tested with NTSC and PAL standards and buffer streaming.
