=============
PCMCIA Driver
=============

sysfs
-----

New PCMCIA IDs may be added to a device driver pcmcia_device_id table at
runtime as shown below::

  echo "match_flags manf_id card_id func_id function device_no \
  prod_id_hash[0] prod_id_hash[1] prod_id_hash[2] prod_id_hash[3]" > \
  /sys/bus/pcmcia/drivers/{driver}/new_id

All fields are passed in as hexadecimal values (no leading 0x).
The meaning is described in the PCMCIA specification, the match_flags is
a bitwise or-ed combination from PCMCIA_DEV_ID_MATCH_* constants
defined in include/linux/mod_devicetable.h.

Once added, the driver probe routine will be invoked for any unclaimed
PCMCIA device listed in its (newly updated) pcmcia_device_id list.

A common use-case is to add a new device according to the manufacturer ID
and the card ID (form the manf_id and card_id file in the device tree).
For this, just use::

  echo "0x3 manf_id card_id 0 0 0 0 0 0 0" > \
    /sys/bus/pcmcia/drivers/{driver}/new_id

after loading the driver.
