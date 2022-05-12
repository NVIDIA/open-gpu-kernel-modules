============
Device table
============

Matching of PCMCIA devices to drivers is done using one or more of the
following criteria:

- manufactor ID
- card ID
- product ID strings _and_ hashes of these strings
- function ID
- device function (actual and pseudo)

You should use the helpers in include/pcmcia/device_id.h for generating the
struct pcmcia_device_id[] entries which match devices to drivers.

If you want to match product ID strings, you also need to pass the crc32
hashes of the string to the macro, e.g. if you want to match the product ID
string 1, you need to use

PCMCIA_DEVICE_PROD_ID1("some_string", 0x(hash_of_some_string)),

If the hash is incorrect, the kernel will inform you about this in "dmesg"
upon module initialization, and tell you of the correct hash.

You can determine the hash of the product ID strings by catting the file
"modalias" in the sysfs directory of the PCMCIA device. It generates a string
in the following form:
pcmcia:m0149cC1ABf06pfn00fn00pa725B842DpbF1EFEE84pc0877B627pd00000000

The hex value after "pa" is the hash of product ID string 1, after "pb" for
string 2 and so on.

Alternatively, you can use crc32hash (see tools/pcmcia/crc32hash.c)
to determine the crc32 hash.  Simply pass the string you want to evaluate
as argument to this program, e.g.:
$ tools/pcmcia/crc32hash "Dual Speed"
