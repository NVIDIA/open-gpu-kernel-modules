=============================
USB 7-Segment Numeric Display
=============================

Manufactured by Delcom Engineering

Device Information
------------------
USB VENDOR_ID	0x0fc5
USB PRODUCT_ID	0x1227
Both the 6 character and 8 character displays have PRODUCT_ID,
and according to Delcom Engineering no queryable information
can be obtained from the device to tell them apart.

Device Modes
------------
By default, the driver assumes the display is only 6 characters
The mode for 6 characters is:

	MSB 0x06; LSB 0x3f

For the 8 character display:

	MSB 0x08; LSB 0xff

The device can accept "text" either in raw, hex, or ascii textmode.
raw controls each segment manually,
hex expects a value between 0-15 per character,
ascii expects a value between '0'-'9' and 'A'-'F'.
The default is ascii.

Device Operation
----------------
1.	Turn on the device:
	echo 1 > /sys/bus/usb/.../powered
2.	Set the device's mode:
	echo $mode_msb > /sys/bus/usb/.../mode_msb
	echo $mode_lsb > /sys/bus/usb/.../mode_lsb
3.	Set the textmode:
	echo $textmode > /sys/bus/usb/.../textmode
4.	set the text (for example):
	echo "123ABC" > /sys/bus/usb/.../text (ascii)
	echo "A1B2" > /sys/bus/usb/.../text (ascii)
	echo -ne "\x01\x02\x03" > /sys/bus/usb/.../text (hex)
5.	Set the decimal places.
	The device has either 6 or 8 decimal points.
	to set the nth decimal place calculate 10 ** n
	and echo it in to /sys/bus/usb/.../decimals
	To set multiple decimals points sum up each power.
	For example, to set the 0th and 3rd decimal place
	echo 1001 > /sys/bus/usb/.../decimals
