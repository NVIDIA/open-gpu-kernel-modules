==================
Driver i2c-mlxcpld
==================

Author: Michael Shych <michaelsh@mellanox.com>

This is the Mellanox I2C controller logic, implemented in Lattice CPLD
device.

Device supports:
 - Master mode.
 - One physical bus.
 - Polling mode.

This controller is equipped within the next Mellanox systems:
"msx6710", "msx6720", "msb7700", "msn2700", "msx1410", "msn2410", "msb7800",
"msn2740", "msn2100".

The next transaction types are supported:
 - Receive Byte/Block.
 - Send Byte/Block.
 - Read Byte/Block.
 - Write Byte/Block.

Registers:

=============== === =======================================================================
CPBLTY		0x0 - capability reg.
			Bits [6:5] - transaction length. b01 - 72B is supported,
			36B in other case.
			Bit 7 - SMBus block read support.
CTRL		0x1 - control reg.
			Resets all the registers.
HALF_CYC	0x4 - cycle reg.
			Configure the width of I2C SCL half clock cycle (in 4 LPC_CLK
			units).
I2C_HOLD	0x5 - hold reg.
			OE (output enable) is delayed by value set to this register
			(in LPC_CLK units)
CMD			0x6 - command reg.
			Bit 0, 0 = write, 1 = read.
			Bits [7:1] - the 7bit Address of the I2C device.
			It should be written last as it triggers an I2C transaction.
NUM_DATA	0x7 - data size reg.
			Number of data bytes to write in read transaction
NUM_ADDR	0x8 - address reg.
			Number of address bytes to write in read transaction.
STATUS		0x9 - status reg.
			Bit 0 - transaction is completed.
			Bit 4 - ACK/NACK.
DATAx		0xa - 0x54  - 68 bytes data buffer regs.
			For write transaction address is specified in four first bytes
			(DATA1 - DATA4), data starting from DATA4.
			For read transactions address is sent in a separate transaction and
			specified in the four first bytes (DATA0 - DATA3). Data is read
			starting from DATA0.
=============== === =======================================================================
