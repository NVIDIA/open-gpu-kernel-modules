=======================================
Kernel driver for Mellanox systems LEDs
=======================================

Provide system LED support for the nex Mellanox systems:
"msx6710", "msx6720", "msb7700", "msn2700", "msx1410",
"msn2410", "msb7800", "msn2740", "msn2100".

Description
-----------
Driver provides the following LEDs for the systems "msx6710", "msx6720",
"msb7700", "msn2700", "msx1410", "msn2410", "msb7800", "msn2740":

  - mlxcpld:fan1:green
  - mlxcpld:fan1:red
  - mlxcpld:fan2:green
  - mlxcpld:fan2:red
  - mlxcpld:fan3:green
  - mlxcpld:fan3:red
  - mlxcpld:fan4:green
  - mlxcpld:fan4:red
  - mlxcpld:psu:green
  - mlxcpld:psu:red
  - mlxcpld:status:green
  - mlxcpld:status:red

 "status"
  - CPLD reg offset: 0x20
  - Bits [3:0]

 "psu"
  - CPLD reg offset: 0x20
  - Bits [7:4]

 "fan1"
  - CPLD reg offset: 0x21
  - Bits [3:0]

 "fan2"
  - CPLD reg offset: 0x21
  - Bits [7:4]

 "fan3"
  - CPLD reg offset: 0x22
  - Bits [3:0]

 "fan4"
  - CPLD reg offset: 0x22
  - Bits [7:4]

 Color mask for all the above LEDs:

  [bit3,bit2,bit1,bit0] or
  [bit7,bit6,bit5,bit4]:

	- [0,0,0,0] = LED OFF
	- [0,1,0,1] = Red static ON
	- [1,1,0,1] = Green static ON
	- [0,1,1,0] = Red blink 3Hz
	- [1,1,1,0] = Green blink 3Hz
	- [0,1,1,1] = Red blink 6Hz
	- [1,1,1,1] = Green blink 6Hz

Driver provides the following LEDs for the system "msn2100":

  - mlxcpld:fan:green
  - mlxcpld:fan:red
  - mlxcpld:psu1:green
  - mlxcpld:psu1:red
  - mlxcpld:psu2:green
  - mlxcpld:psu2:red
  - mlxcpld:status:green
  - mlxcpld:status:red
  - mlxcpld:uid:blue

 "status"
  - CPLD reg offset: 0x20
  - Bits [3:0]

 "fan"
  - CPLD reg offset: 0x21
  - Bits [3:0]

 "psu1"
  - CPLD reg offset: 0x23
  - Bits [3:0]

 "psu2"
  - CPLD reg offset: 0x23
  - Bits [7:4]

 "uid"
  - CPLD reg offset: 0x24
  - Bits [3:0]

 Color mask for all the above LEDs, excepted uid:

  [bit3,bit2,bit1,bit0] or
  [bit7,bit6,bit5,bit4]:

	- [0,0,0,0] = LED OFF
	- [0,1,0,1] = Red static ON
	- [1,1,0,1] = Green static ON
	- [0,1,1,0] = Red blink 3Hz
	- [1,1,1,0] = Green blink 3Hz
	- [0,1,1,1] = Red blink 6Hz
	- [1,1,1,1] = Green blink 6Hz

 Color mask for uid LED:
  [bit3,bit2,bit1,bit0]:

	- [0,0,0,0] = LED OFF
	- [1,1,0,1] = Blue static ON
	- [1,1,1,0] = Blue blink 3Hz
	- [1,1,1,1] = Blue blink 6Hz

Driver supports HW blinking at 3Hz and 6Hz frequency (50% duty cycle).
For 3Hz duty cylce is about 167 msec, for 6Hz is about 83 msec.
