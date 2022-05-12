=====================================================
Intel INT3496 ACPI device extcon driver documentation
=====================================================

The Intel INT3496 ACPI device extcon driver is a driver for ACPI
devices with an acpi-id of INT3496, such as found for example on
Intel Baytrail and Cherrytrail tablets.

This ACPI device describes how the OS can read the id-pin of the devices'
USB-otg port, as well as how it optionally can enable Vbus output on the
otg port and how it can optionally control the muxing of the data pins
between an USB host and an USB peripheral controller.

The ACPI devices exposes this functionality by returning an array with up
to 3 gpio descriptors from its ACPI _CRS (Current Resource Settings) call:

=======  =====================================================================
Index 0  The input gpio for the id-pin, this is always present and valid
Index 1  The output gpio for enabling Vbus output from the device to the otg
         port, write 1 to enable the Vbus output (this gpio descriptor may
         be absent or invalid)
Index 2  The output gpio for muxing of the data pins between the USB host and
         the USB peripheral controller, write 1 to mux to the peripheral
         controller
=======  =====================================================================

There is a mapping between indices and GPIO connection IDs as follows

	======= =======
	id	index 0
	vbus	index 1
	mux	index 2
	======= =======
