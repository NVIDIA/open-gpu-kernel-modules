=======================================================
VIA Integration Graphic Chip Console Framebuffer Driver
=======================================================

Platform
--------
    The console framebuffer driver is for graphics chips of
    VIA UniChrome Family
    (CLE266, PM800 / CN400 / CN300,
    P4M800CE / P4M800Pro / CN700 / VN800,
    CX700 / VX700, K8M890, P4M890,
    CN896 / P4M900, VX800, VX855)

Driver features
---------------
    Device: CRT, LCD, DVI

    Support viafb_mode::

	CRT:
	    640x480(60, 75, 85, 100, 120 Hz), 720x480(60 Hz),
	    720x576(60 Hz), 800x600(60, 75, 85, 100, 120 Hz),
	    848x480(60 Hz), 856x480(60 Hz), 1024x512(60 Hz),
	    1024x768(60, 75, 85, 100 Hz), 1152x864(75 Hz),
	    1280x768(60 Hz), 1280x960(60 Hz), 1280x1024(60, 75, 85 Hz),
	    1440x1050(60 Hz), 1600x1200(60, 75 Hz), 1280x720(60 Hz),
	    1920x1080(60 Hz), 1400x1050(60 Hz), 800x480(60 Hz)

    color depth: 8 bpp, 16 bpp, 32 bpp supports.

    Support 2D hardware accelerator.

Using the viafb module
----------------------
    Start viafb with default settings::

	#modprobe viafb

    Start viafb with user options::

	#modprobe viafb viafb_mode=800x600 viafb_bpp=16 viafb_refresh=60
		  viafb_active_dev=CRT+DVI viafb_dvi_port=DVP1
		  viafb_mode1=1024x768 viafb_bpp=16 viafb_refresh1=60
		  viafb_SAMM_ON=1

    viafb_mode:
	- 640x480 (default)
	- 720x480
	- 800x600
	- 1024x768

    viafb_bpp:
	- 8, 16, 32 (default:32)

    viafb_refresh:
	- 60, 75, 85, 100, 120 (default:60)

    viafb_lcd_dsp_method:
	- 0 : expansion (default)
	- 1 : centering

    viafb_lcd_mode:
	0 : LCD panel with LSB data format input (default)
	1 : LCD panel with MSB data format input

    viafb_lcd_panel_id:
	- 0 : Resolution: 640x480, Channel: single, Dithering: Enable
	- 1 : Resolution: 800x600, Channel: single, Dithering: Enable
	- 2 : Resolution: 1024x768, Channel: single, Dithering: Enable (default)
	- 3 : Resolution: 1280x768, Channel: single, Dithering: Enable
	- 4 : Resolution: 1280x1024, Channel: dual, Dithering: Enable
	- 5 : Resolution: 1400x1050, Channel: dual, Dithering: Enable
	- 6 : Resolution: 1600x1200, Channel: dual, Dithering: Enable

	- 8 : Resolution: 800x480, Channel: single, Dithering: Enable
	- 9 : Resolution: 1024x768, Channel: dual, Dithering: Enable
	- 10: Resolution: 1024x768, Channel: single, Dithering: Disable
	- 11: Resolution: 1024x768, Channel: dual, Dithering: Disable
	- 12: Resolution: 1280x768, Channel: single, Dithering: Disable
	- 13: Resolution: 1280x1024, Channel: dual, Dithering: Disable
	- 14: Resolution: 1400x1050, Channel: dual, Dithering: Disable
	- 15: Resolution: 1600x1200, Channel: dual, Dithering: Disable
	- 16: Resolution: 1366x768, Channel: single, Dithering: Disable
	- 17: Resolution: 1024x600, Channel: single, Dithering: Enable
	- 18: Resolution: 1280x768, Channel: dual, Dithering: Enable
	- 19: Resolution: 1280x800, Channel: single, Dithering: Enable

    viafb_accel:
	- 0 : No 2D Hardware Acceleration
	- 1 : 2D Hardware Acceleration (default)

    viafb_SAMM_ON:
	- 0 : viafb_SAMM_ON disable (default)
	- 1 : viafb_SAMM_ON enable

    viafb_mode1: (secondary display device)
	- 640x480 (default)
	- 720x480
	- 800x600
	- 1024x768

    viafb_bpp1: (secondary display device)
	- 8, 16, 32 (default:32)

    viafb_refresh1: (secondary display device)
	- 60, 75, 85, 100, 120 (default:60)

    viafb_active_dev:
	This option is used to specify active devices.(CRT, DVI, CRT+LCD...)
	DVI stands for DVI or HDMI, E.g., If you want to enable HDMI,
	set viafb_active_dev=DVI. In SAMM case, the previous of
	viafb_active_dev is primary device, and the following is
	secondary device.

	For example:

	To enable one device, such as DVI only, we can use::

	    modprobe viafb viafb_active_dev=DVI

	To enable two devices, such as CRT+DVI::

	    modprobe viafb viafb_active_dev=CRT+DVI;

	For DuoView case, we can use::

	    modprobe viafb viafb_active_dev=CRT+DVI

	OR::

	    modprobe viafb viafb_active_dev=DVI+CRT...

	For SAMM case:

	If CRT is primary and DVI is secondary, we should use::

	    modprobe viafb viafb_active_dev=CRT+DVI viafb_SAMM_ON=1...

	If DVI is primary and CRT is secondary, we should use::

	    modprobe viafb viafb_active_dev=DVI+CRT viafb_SAMM_ON=1...

    viafb_display_hardware_layout:
	This option is used to specify display hardware layout for CX700 chip.

	- 1 : LCD only
	- 2 : DVI only
	- 3 : LCD+DVI (default)
	- 4 : LCD1+LCD2 (internal + internal)
	- 16: LCD1+ExternalLCD2 (internal + external)

    viafb_second_size:
	This option is used to set second device memory size(MB) in SAMM case.
	The minimal size is 16.

    viafb_platform_epia_dvi:
	This option is used to enable DVI on EPIA - M

	- 0 : No DVI on EPIA - M (default)
	- 1 : DVI on EPIA - M

    viafb_bus_width:
	When using 24 - Bit Bus Width Digital Interface,
	this option should be set.

	- 12: 12-Bit LVDS or 12-Bit TMDS (default)
	- 24: 24-Bit LVDS or 24-Bit TMDS

    viafb_device_lcd_dualedge:
	When using Dual Edge Panel, this option should be set.

	- 0 : No Dual Edge Panel (default)
	- 1 : Dual Edge Panel

    viafb_lcd_port:
	This option is used to specify LCD output port,
	available values are "DVP0" "DVP1" "DFP_HIGHLOW" "DFP_HIGH" "DFP_LOW".

	for external LCD + external DVI on CX700(External LCD is on DVP0),
	we should use::

	    modprobe viafb viafb_lcd_port=DVP0...

Notes:
    1. CRT may not display properly for DuoView CRT & DVI display at
       the "640x480" PAL mode with DVI overscan enabled.
    2. SAMM stands for single adapter multi monitors. It is different from
       multi-head since SAMM support multi monitor at driver layers, thus fbcon
       layer doesn't even know about it; SAMM's second screen doesn't have a
       device node file, thus a user mode application can't access it directly.
       When SAMM is enabled, viafb_mode and viafb_mode1, viafb_bpp and
       viafb_bpp1, viafb_refresh and viafb_refresh1 can be different.
    3. When console is depending on viafbinfo1, dynamically change resolution
       and bpp, need to call VIAFB specified ioctl interface VIAFB_SET_DEVICE
       instead of calling common ioctl function FBIOPUT_VSCREENINFO since
       viafb doesn't support multi-head well, or it will cause screen crush.


Configure viafb with "fbset" tool
---------------------------------

    "fbset" is an inbox utility of Linux.

    1. Inquire current viafb information, type::

	   # fbset -i

    2. Set various resolutions and viafb_refresh rates::

	   # fbset <resolution-vertical_sync>

       example::

	   # fbset "1024x768-75"

       or::

	   # fbset -g 1024 768 1024 768 32

       Check the file "/etc/fb.modes" to find display modes available.

    3. Set the color depth::

	   # fbset -depth <value>

       example::

	   # fbset -depth 16


Configure viafb via /proc
-------------------------
    The following files exist in /proc/viafb

    supported_output_devices
	This read-only file contains a full ',' separated list containing all
	output devices that could be available on your platform. It is likely
	that not all of those have a connector on your hardware but it should
	provide a good starting point to figure out which of those names match
	a real connector.

	Example::

		# cat /proc/viafb/supported_output_devices

    iga1/output_devices, iga2/output_devices
	These two files are readable and writable. iga1 and iga2 are the two
	independent units that produce the screen image. Those images can be
	forwarded to one or more output devices. Reading those files is a way
	to query which output devices are currently used by an iga.

	Example::

		# cat /proc/viafb/iga1/output_devices

	If there are no output devices printed the output of this iga is lost.
	This can happen for example if only one (the other) iga is used.
	Writing to these files allows adjusting the output devices during
	runtime. One can add new devices, remove existing ones or switch
	between igas. Essentially you can write a ',' separated list of device
	names (or a single one) in the same format as the output to those
	files. You can add a '+' or '-' as a prefix allowing simple addition
	and removal of devices. So a prefix '+' adds the devices from your list
	to the already existing ones, '-' removes the listed devices from the
	existing ones and if no prefix is given it replaces all existing ones
	with the listed ones. If you remove devices they are expected to turn
	off. If you add devices that are already part of the other iga they are
	removed there and added to the new one.

	Examples:

	Add CRT as output device to iga1::

		# echo +CRT > /proc/viafb/iga1/output_devices

	Remove (turn off) DVP1 and LVDS1 as output devices of iga2::

		# echo -DVP1,LVDS1 > /proc/viafb/iga2/output_devices

	Replace all iga1 output devices by CRT::

		# echo CRT > /proc/viafb/iga1/output_devices


Bootup with viafb
-----------------

Add the following line to your grub.conf::

    append = "video=viafb:viafb_mode=1024x768,viafb_bpp=32,viafb_refresh=85"


VIA Framebuffer modes
=====================

.. include:: viafb.modes
   :literal:
