/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Device tables which are exported to userspace via
 * scripts/mod/file2alias.c.  You must keep that file in sync with this
 * header.
 */

#ifndef LINUX_MOD_DEVICETABLE_H
#define LINUX_MOD_DEVICETABLE_H

#ifdef __KERNEL__
#include <linux/types.h>
#include <linux/uuid.h>
typedef unsigned long kernel_ulong_t;
#endif

#define PCI_ANY_ID (~0)

/**
 * struct pci_device_id - PCI device ID structure
 * @vendor:		Vendor ID to match (or PCI_ANY_ID)
 * @device:		Device ID to match (or PCI_ANY_ID)
 * @subvendor:		Subsystem vendor ID to match (or PCI_ANY_ID)
 * @subdevice:		Subsystem device ID to match (or PCI_ANY_ID)
 * @class:		Device class, subclass, and "interface" to match.
 *			See Appendix D of the PCI Local Bus Spec or
 *			include/linux/pci_ids.h for a full list of classes.
 *			Most drivers do not need to specify class/class_mask
 *			as vendor/device is normally sufficient.
 * @class_mask:		Limit which sub-fields of the class field are compared.
 *			See drivers/scsi/sym53c8xx_2/ for example of usage.
 * @driver_data:	Data private to the driver.
 *			Most drivers don't need to use driver_data field.
 *			Best practice is to use driver_data as an index
 *			into a static list of equivalent device types,
 *			instead of using it as a pointer.
 */
struct pci_device_id {
	__u32 vendor, device;		/* Vendor and device ID or PCI_ANY_ID*/
	__u32 subvendor, subdevice;	/* Subsystem ID's or PCI_ANY_ID */
	__u32 class, class_mask;	/* (class,subclass,prog-if) triplet */
	kernel_ulong_t driver_data;	/* Data private to the driver */
};


#define IEEE1394_MATCH_VENDOR_ID	0x0001
#define IEEE1394_MATCH_MODEL_ID		0x0002
#define IEEE1394_MATCH_SPECIFIER_ID	0x0004
#define IEEE1394_MATCH_VERSION		0x0008

struct ieee1394_device_id {
	__u32 match_flags;
	__u32 vendor_id;
	__u32 model_id;
	__u32 specifier_id;
	__u32 version;
	kernel_ulong_t driver_data;
};


/*
 * Device table entry for "new style" table-driven USB drivers.
 * User mode code can read these tables to choose which modules to load.
 * Declare the table as a MODULE_DEVICE_TABLE.
 *
 * A probe() parameter will point to a matching entry from this table.
 * Use the driver_info field for each match to hold information tied
 * to that match:  device quirks, etc.
 *
 * Terminate the driver's table with an all-zeroes entry.
 * Use the flag values to control which fields are compared.
 */

/**
 * struct usb_device_id - identifies USB devices for probing and hotplugging
 * @match_flags: Bit mask controlling which of the other fields are used to
 *	match against new devices. Any field except for driver_info may be
 *	used, although some only make sense in conjunction with other fields.
 *	This is usually set by a USB_DEVICE_*() macro, which sets all
 *	other fields in this structure except for driver_info.
 * @idVendor: USB vendor ID for a device; numbers are assigned
 *	by the USB forum to its members.
 * @idProduct: Vendor-assigned product ID.
 * @bcdDevice_lo: Low end of range of vendor-assigned product version numbers.
 *	This is also used to identify individual product versions, for
 *	a range consisting of a single device.
 * @bcdDevice_hi: High end of version number range.  The range of product
 *	versions is inclusive.
 * @bDeviceClass: Class of device; numbers are assigned
 *	by the USB forum.  Products may choose to implement classes,
 *	or be vendor-specific.  Device classes specify behavior of all
 *	the interfaces on a device.
 * @bDeviceSubClass: Subclass of device; associated with bDeviceClass.
 * @bDeviceProtocol: Protocol of device; associated with bDeviceClass.
 * @bInterfaceClass: Class of interface; numbers are assigned
 *	by the USB forum.  Products may choose to implement classes,
 *	or be vendor-specific.  Interface classes specify behavior only
 *	of a given interface; other interfaces may support other classes.
 * @bInterfaceSubClass: Subclass of interface; associated with bInterfaceClass.
 * @bInterfaceProtocol: Protocol of interface; associated with bInterfaceClass.
 * @bInterfaceNumber: Number of interface; composite devices may use
 *	fixed interface numbers to differentiate between vendor-specific
 *	interfaces.
 * @driver_info: Holds information used by the driver.  Usually it holds
 *	a pointer to a descriptor understood by the driver, or perhaps
 *	device flags.
 *
 * In most cases, drivers will create a table of device IDs by using
 * USB_DEVICE(), or similar macros designed for that purpose.
 * They will then export it to userspace using MODULE_DEVICE_TABLE(),
 * and provide it to the USB core through their usb_driver structure.
 *
 * See the usb_match_id() function for information about how matches are
 * performed.  Briefly, you will normally use one of several macros to help
 * construct these entries.  Each entry you provide will either identify
 * one or more specific products, or will identify a class of products
 * which have agreed to behave the same.  You should put the more specific
 * matches towards the beginning of your table, so that driver_info can
 * record quirks of specific products.
 */
struct usb_device_id {
	/* which fields to match against? */
	__u16		match_flags;

	/* Used for product specific matches; range is inclusive */
	__u16		idVendor;
	__u16		idProduct;
	__u16		bcdDevice_lo;
	__u16		bcdDevice_hi;

	/* Used for device class matches */
	__u8		bDeviceClass;
	__u8		bDeviceSubClass;
	__u8		bDeviceProtocol;

	/* Used for interface class matches */
	__u8		bInterfaceClass;
	__u8		bInterfaceSubClass;
	__u8		bInterfaceProtocol;

	/* Used for vendor-specific interface matches */
	__u8		bInterfaceNumber;

	/* not matched against */
	kernel_ulong_t	driver_info
		__attribute__((aligned(sizeof(kernel_ulong_t))));
};

/* Some useful macros to use to create struct usb_device_id */
#define USB_DEVICE_ID_MATCH_VENDOR		0x0001
#define USB_DEVICE_ID_MATCH_PRODUCT		0x0002
#define USB_DEVICE_ID_MATCH_DEV_LO		0x0004
#define USB_DEVICE_ID_MATCH_DEV_HI		0x0008
#define USB_DEVICE_ID_MATCH_DEV_CLASS		0x0010
#define USB_DEVICE_ID_MATCH_DEV_SUBCLASS	0x0020
#define USB_DEVICE_ID_MATCH_DEV_PROTOCOL	0x0040
#define USB_DEVICE_ID_MATCH_INT_CLASS		0x0080
#define USB_DEVICE_ID_MATCH_INT_SUBCLASS	0x0100
#define USB_DEVICE_ID_MATCH_INT_PROTOCOL	0x0200
#define USB_DEVICE_ID_MATCH_INT_NUMBER		0x0400

#define HID_ANY_ID				(~0)
#define HID_BUS_ANY				0xffff
#define HID_GROUP_ANY				0x0000

struct hid_device_id {
	__u16 bus;
	__u16 group;
	__u32 vendor;
	__u32 product;
	kernel_ulong_t driver_data;
};

/* s390 CCW devices */
struct ccw_device_id {
	__u16	match_flags;	/* which fields to match against */

	__u16	cu_type;	/* control unit type     */
	__u16	dev_type;	/* device type           */
	__u8	cu_model;	/* control unit model    */
	__u8	dev_model;	/* device model          */

	kernel_ulong_t driver_info;
};

#define CCW_DEVICE_ID_MATCH_CU_TYPE		0x01
#define CCW_DEVICE_ID_MATCH_CU_MODEL		0x02
#define CCW_DEVICE_ID_MATCH_DEVICE_TYPE		0x04
#define CCW_DEVICE_ID_MATCH_DEVICE_MODEL	0x08

/* s390 AP bus devices */
struct ap_device_id {
	__u16 match_flags;	/* which fields to match against */
	__u8 dev_type;		/* device type */
	kernel_ulong_t driver_info;
};

#define AP_DEVICE_ID_MATCH_CARD_TYPE		0x01
#define AP_DEVICE_ID_MATCH_QUEUE_TYPE		0x02

/* s390 css bus devices (subchannels) */
struct css_device_id {
	__u8 match_flags;
	__u8 type; /* subchannel type */
	kernel_ulong_t driver_data;
};

#define ACPI_ID_LEN	9

struct acpi_device_id {
	__u8 id[ACPI_ID_LEN];
	kernel_ulong_t driver_data;
	__u32 cls;
	__u32 cls_msk;
};

#define PNP_ID_LEN	8
#define PNP_MAX_DEVICES	8

struct pnp_device_id {
	__u8 id[PNP_ID_LEN];
	kernel_ulong_t driver_data;
};

struct pnp_card_device_id {
	__u8 id[PNP_ID_LEN];
	kernel_ulong_t driver_data;
	struct {
		__u8 id[PNP_ID_LEN];
	} devs[PNP_MAX_DEVICES];
};


#define SERIO_ANY	0xff

struct serio_device_id {
	__u8 type;
	__u8 extra;
	__u8 id;
	__u8 proto;
};

struct hda_device_id {
	__u32 vendor_id;
	__u32 rev_id;
	__u8 api_version;
	const char *name;
	unsigned long driver_data;
};

struct sdw_device_id {
	__u16 mfg_id;
	__u16 part_id;
	__u8  sdw_version;
	__u8  class_id;
	kernel_ulong_t driver_data;
};

/*
 * Struct used for matching a device
 */
struct of_device_id {
	char	name[32];
	char	type[32];
	char	compatible[128];
	const void *data;
};

/* VIO */
struct vio_device_id {
	char type[32];
	char compat[32];
};

/* PCMCIA */

struct pcmcia_device_id {
	__u16		match_flags;

	__u16		manf_id;
	__u16		card_id;

	__u8		func_id;

	/* for real multi-function devices */
	__u8		function;

	/* for pseudo multi-function devices */
	__u8		device_no;

	__u32		prod_id_hash[4];

	/* not matched against in kernelspace */
	const char *	prod_id[4];

	/* not matched against */
	kernel_ulong_t	driver_info;
	char *		cisfile;
};

#define PCMCIA_DEV_ID_MATCH_MANF_ID	0x0001
#define PCMCIA_DEV_ID_MATCH_CARD_ID	0x0002
#define PCMCIA_DEV_ID_MATCH_FUNC_ID	0x0004
#define PCMCIA_DEV_ID_MATCH_FUNCTION	0x0008
#define PCMCIA_DEV_ID_MATCH_PROD_ID1	0x0010
#define PCMCIA_DEV_ID_MATCH_PROD_ID2	0x0020
#define PCMCIA_DEV_ID_MATCH_PROD_ID3	0x0040
#define PCMCIA_DEV_ID_MATCH_PROD_ID4	0x0080
#define PCMCIA_DEV_ID_MATCH_DEVICE_NO	0x0100
#define PCMCIA_DEV_ID_MATCH_FAKE_CIS	0x0200
#define PCMCIA_DEV_ID_MATCH_ANONYMOUS	0x0400

/* Input */
#define INPUT_DEVICE_ID_EV_MAX		0x1f
#define INPUT_DEVICE_ID_KEY_MIN_INTERESTING	0x71
#define INPUT_DEVICE_ID_KEY_MAX		0x2ff
#define INPUT_DEVICE_ID_REL_MAX		0x0f
#define INPUT_DEVICE_ID_ABS_MAX		0x3f
#define INPUT_DEVICE_ID_MSC_MAX		0x07
#define INPUT_DEVICE_ID_LED_MAX		0x0f
#define INPUT_DEVICE_ID_SND_MAX		0x07
#define INPUT_DEVICE_ID_FF_MAX		0x7f
#define INPUT_DEVICE_ID_SW_MAX		0x10
#define INPUT_DEVICE_ID_PROP_MAX	0x1f

#define INPUT_DEVICE_ID_MATCH_BUS	1
#define INPUT_DEVICE_ID_MATCH_VENDOR	2
#define INPUT_DEVICE_ID_MATCH_PRODUCT	4
#define INPUT_DEVICE_ID_MATCH_VERSION	8

#define INPUT_DEVICE_ID_MATCH_EVBIT	0x0010
#define INPUT_DEVICE_ID_MATCH_KEYBIT	0x0020
#define INPUT_DEVICE_ID_MATCH_RELBIT	0x0040
#define INPUT_DEVICE_ID_MATCH_ABSBIT	0x0080
#define INPUT_DEVICE_ID_MATCH_MSCIT	0x0100
#define INPUT_DEVICE_ID_MATCH_LEDBIT	0x0200
#define INPUT_DEVICE_ID_MATCH_SNDBIT	0x0400
#define INPUT_DEVICE_ID_MATCH_FFBIT	0x0800
#define INPUT_DEVICE_ID_MATCH_SWBIT	0x1000
#define INPUT_DEVICE_ID_MATCH_PROPBIT	0x2000

struct input_device_id {

	kernel_ulong_t flags;

	__u16 bustype;
	__u16 vendor;
	__u16 product;
	__u16 version;

	kernel_ulong_t evbit[INPUT_DEVICE_ID_EV_MAX / BITS_PER_LONG + 1];
	kernel_ulong_t keybit[INPUT_DEVICE_ID_KEY_MAX / BITS_PER_LONG + 1];
	kernel_ulong_t relbit[INPUT_DEVICE_ID_REL_MAX / BITS_PER_LONG + 1];
	kernel_ulong_t absbit[INPUT_DEVICE_ID_ABS_MAX / BITS_PER_LONG + 1];
	kernel_ulong_t mscbit[INPUT_DEVICE_ID_MSC_MAX / BITS_PER_LONG + 1];
	kernel_ulong_t ledbit[INPUT_DEVICE_ID_LED_MAX / BITS_PER_LONG + 1];
	kernel_ulong_t sndbit[INPUT_DEVICE_ID_SND_MAX / BITS_PER_LONG + 1];
	kernel_ulong_t ffbit[INPUT_DEVICE_ID_FF_MAX / BITS_PER_LONG + 1];
	kernel_ulong_t swbit[INPUT_DEVICE_ID_SW_MAX / BITS_PER_LONG + 1];
	kernel_ulong_t propbit[INPUT_DEVICE_ID_PROP_MAX / BITS_PER_LONG + 1];

	kernel_ulong_t driver_info;
};

/* EISA */

#define EISA_SIG_LEN   8

/* The EISA signature, in ASCII form, null terminated */
struct eisa_device_id {
	char          sig[EISA_SIG_LEN];
	kernel_ulong_t driver_data;
};

#define EISA_DEVICE_MODALIAS_FMT "eisa:s%s"

struct parisc_device_id {
	__u8	hw_type;	/* 5 bits used */
	__u8	hversion_rev;	/* 4 bits */
	__u16	hversion;	/* 12 bits */
	__u32	sversion;	/* 20 bits */
};

#define PA_HWTYPE_ANY_ID	0xff
#define PA_HVERSION_REV_ANY_ID	0xff
#define PA_HVERSION_ANY_ID	0xffff
#define PA_SVERSION_ANY_ID	0xffffffff

/* SDIO */

#define SDIO_ANY_ID (~0)

struct sdio_device_id {
	__u8	class;			/* Standard interface or SDIO_ANY_ID */
	__u16	vendor;			/* Vendor or SDIO_ANY_ID */
	__u16	device;			/* Device ID or SDIO_ANY_ID */
	kernel_ulong_t driver_data;	/* Data private to the driver */
};

/* SSB core, see drivers/ssb/ */
struct ssb_device_id {
	__u16	vendor;
	__u16	coreid;
	__u8	revision;
	__u8	__pad;
} __attribute__((packed, aligned(2)));
#define SSB_DEVICE(_vendor, _coreid, _revision)  \
	{ .vendor = _vendor, .coreid = _coreid, .revision = _revision, }

#define SSB_ANY_VENDOR		0xFFFF
#define SSB_ANY_ID		0xFFFF
#define SSB_ANY_REV		0xFF

/* Broadcom's specific AMBA core, see drivers/bcma/ */
struct bcma_device_id {
	__u16	manuf;
	__u16	id;
	__u8	rev;
	__u8	class;
} __attribute__((packed,aligned(2)));
#define BCMA_CORE(_manuf, _id, _rev, _class)  \
	{ .manuf = _manuf, .id = _id, .rev = _rev, .class = _class, }

#define BCMA_ANY_MANUF		0xFFFF
#define BCMA_ANY_ID		0xFFFF
#define BCMA_ANY_REV		0xFF
#define BCMA_ANY_CLASS		0xFF

struct virtio_device_id {
	__u32 device;
	__u32 vendor;
};
#define VIRTIO_DEV_ANY_ID	0xffffffff

/*
 * For Hyper-V devices we use the device guid as the id.
 */
struct hv_vmbus_device_id {
	guid_t guid;
	kernel_ulong_t driver_data;	/* Data private to the driver */
};

/* rpmsg */

#define RPMSG_NAME_SIZE			32
#define RPMSG_DEVICE_MODALIAS_FMT	"rpmsg:%s"

struct rpmsg_device_id {
	char name[RPMSG_NAME_SIZE];
};

/* i2c */

#define I2C_NAME_SIZE	20
#define I2C_MODULE_PREFIX "i2c:"

struct i2c_device_id {
	char name[I2C_NAME_SIZE];
	kernel_ulong_t driver_data;	/* Data private to the driver */
};

/* pci_epf */

#define PCI_EPF_NAME_SIZE	20
#define PCI_EPF_MODULE_PREFIX	"pci_epf:"

struct pci_epf_device_id {
	char name[PCI_EPF_NAME_SIZE];
	kernel_ulong_t driver_data;
};

/* i3c */

#define I3C_MATCH_DCR			0x1
#define I3C_MATCH_MANUF			0x2
#define I3C_MATCH_PART			0x4
#define I3C_MATCH_EXTRA_INFO		0x8

struct i3c_device_id {
	__u8 match_flags;
	__u8 dcr;
	__u16 manuf_id;
	__u16 part_id;
	__u16 extra_info;

	const void *data;
};

/* spi */

#define SPI_NAME_SIZE	32
#define SPI_MODULE_PREFIX "spi:"

struct spi_device_id {
	char name[SPI_NAME_SIZE];
	kernel_ulong_t driver_data;	/* Data private to the driver */
};

/* SLIMbus */

#define SLIMBUS_NAME_SIZE	32
#define SLIMBUS_MODULE_PREFIX	"slim:"

struct slim_device_id {
	__u16 manf_id, prod_code;
	__u16 dev_index, instance;

	/* Data private to the driver */
	kernel_ulong_t driver_data;
};

#define APR_NAME_SIZE	32
#define APR_MODULE_PREFIX "apr:"

struct apr_device_id {
	char name[APR_NAME_SIZE];
	__u32 domain_id;
	__u32 svc_id;
	__u32 svc_version;
	kernel_ulong_t driver_data;	/* Data private to the driver */
};

#define SPMI_NAME_SIZE	32
#define SPMI_MODULE_PREFIX "spmi:"

struct spmi_device_id {
	char name[SPMI_NAME_SIZE];
	kernel_ulong_t driver_data;	/* Data private to the driver */
};

/* dmi */
enum dmi_field {
	DMI_NONE,
	DMI_BIOS_VENDOR,
	DMI_BIOS_VERSION,
	DMI_BIOS_DATE,
	DMI_BIOS_RELEASE,
	DMI_EC_FIRMWARE_RELEASE,
	DMI_SYS_VENDOR,
	DMI_PRODUCT_NAME,
	DMI_PRODUCT_VERSION,
	DMI_PRODUCT_SERIAL,
	DMI_PRODUCT_UUID,
	DMI_PRODUCT_SKU,
	DMI_PRODUCT_FAMILY,
	DMI_BOARD_VENDOR,
	DMI_BOARD_NAME,
	DMI_BOARD_VERSION,
	DMI_BOARD_SERIAL,
	DMI_BOARD_ASSET_TAG,
	DMI_CHASSIS_VENDOR,
	DMI_CHASSIS_TYPE,
	DMI_CHASSIS_VERSION,
	DMI_CHASSIS_SERIAL,
	DMI_CHASSIS_ASSET_TAG,
	DMI_STRING_MAX,
	DMI_OEM_STRING,	/* special case - will not be in dmi_ident */
};

struct dmi_strmatch {
	unsigned char slot:7;
	unsigned char exact_match:1;
	char substr[79];
};

struct dmi_system_id {
	int (*callback)(const struct dmi_system_id *);
	const char *ident;
	struct dmi_strmatch matches[4];
	void *driver_data;
};
/*
 * struct dmi_device_id appears during expansion of
 * "MODULE_DEVICE_TABLE(dmi, x)". Compiler doesn't look inside it
 * but this is enough for gcc 3.4.6 to error out:
 *	error: storage size of '__mod_dmi_device_table' isn't known
 */
#define dmi_device_id dmi_system_id

#define DMI_MATCH(a, b)	{ .slot = a, .substr = b }
#define DMI_EXACT_MATCH(a, b)	{ .slot = a, .substr = b, .exact_match = 1 }

#define PLATFORM_NAME_SIZE	20
#define PLATFORM_MODULE_PREFIX	"platform:"

struct platform_device_id {
	char name[PLATFORM_NAME_SIZE];
	kernel_ulong_t driver_data;
};

#define MDIO_NAME_SIZE		32
#define MDIO_MODULE_PREFIX	"mdio:"

#define MDIO_ID_FMT "%u%u%u%u%u%u%u%u%u%u%u%u%u%u%u%u%u%u%u%u%u%u%u%u%u%u%u%u%u%u%u%u"
#define MDIO_ID_ARGS(_id) \
	((_id)>>31) & 1, ((_id)>>30) & 1, ((_id)>>29) & 1, ((_id)>>28) & 1, \
	((_id)>>27) & 1, ((_id)>>26) & 1, ((_id)>>25) & 1, ((_id)>>24) & 1, \
	((_id)>>23) & 1, ((_id)>>22) & 1, ((_id)>>21) & 1, ((_id)>>20) & 1, \
	((_id)>>19) & 1, ((_id)>>18) & 1, ((_id)>>17) & 1, ((_id)>>16) & 1, \
	((_id)>>15) & 1, ((_id)>>14) & 1, ((_id)>>13) & 1, ((_id)>>12) & 1, \
	((_id)>>11) & 1, ((_id)>>10) & 1, ((_id)>>9) & 1, ((_id)>>8) & 1, \
	((_id)>>7) & 1, ((_id)>>6) & 1, ((_id)>>5) & 1, ((_id)>>4) & 1, \
	((_id)>>3) & 1, ((_id)>>2) & 1, ((_id)>>1) & 1, (_id) & 1

/**
 * struct mdio_device_id - identifies PHY devices on an MDIO/MII bus
 * @phy_id: The result of
 *     (mdio_read(&MII_PHYSID1) << 16 | mdio_read(&MII_PHYSID2)) & @phy_id_mask
 *     for this PHY type
 * @phy_id_mask: Defines the significant bits of @phy_id.  A value of 0
 *     is used to terminate an array of struct mdio_device_id.
 */
struct mdio_device_id {
	__u32 phy_id;
	__u32 phy_id_mask;
};

struct zorro_device_id {
	__u32 id;			/* Device ID or ZORRO_WILDCARD */
	kernel_ulong_t driver_data;	/* Data private to the driver */
};

#define ZORRO_WILDCARD			(0xffffffff)	/* not official */

#define ZORRO_DEVICE_MODALIAS_FMT	"zorro:i%08X"

#define ISAPNP_ANY_ID		0xffff
struct isapnp_device_id {
	unsigned short card_vendor, card_device;
	unsigned short vendor, function;
	kernel_ulong_t driver_data;	/* data private to the driver */
};

/**
 * struct amba_id - identifies a device on an AMBA bus
 * @id: The significant bits if the hardware device ID
 * @mask: Bitmask specifying which bits of the id field are significant when
 *	matching.  A driver binds to a device when ((hardware device ID) & mask)
 *	== id.
 * @data: Private data used by the driver.
 */
struct amba_id {
	unsigned int		id;
	unsigned int		mask;
	void			*data;
};

/**
 * struct mips_cdmm_device_id - identifies devices in MIPS CDMM bus
 * @type:	Device type identifier.
 */
struct mips_cdmm_device_id {
	__u8	type;
};

/*
 * Match x86 CPUs for CPU specific drivers.
 * See documentation of "x86_match_cpu" for details.
 */

/*
 * MODULE_DEVICE_TABLE expects this struct to be called x86cpu_device_id.
 * Although gcc seems to ignore this error, clang fails without this define.
 */
#define x86cpu_device_id x86_cpu_id
struct x86_cpu_id {
	__u16 vendor;
	__u16 family;
	__u16 model;
	__u16 steppings;
	__u16 feature;	/* bit index */
	kernel_ulong_t driver_data;
};

/* Wild cards for x86_cpu_id::vendor, family, model and feature */
#define X86_VENDOR_ANY 0xffff
#define X86_FAMILY_ANY 0
#define X86_MODEL_ANY  0
#define X86_STEPPING_ANY 0
#define X86_FEATURE_ANY 0	/* Same as FPU, you can't test for that */

/*
 * Generic table type for matching CPU features.
 * @feature:	the bit number of the feature (0 - 65535)
 */

struct cpu_feature {
	__u16	feature;
};

#define IPACK_ANY_FORMAT 0xff
#define IPACK_ANY_ID (~0)
struct ipack_device_id {
	__u8  format;			/* Format version or IPACK_ANY_ID */
	__u32 vendor;			/* Vendor ID or IPACK_ANY_ID */
	__u32 device;			/* Device ID or IPACK_ANY_ID */
};

#define MEI_CL_MODULE_PREFIX "mei:"
#define MEI_CL_NAME_SIZE 32
#define MEI_CL_VERSION_ANY 0xff

/**
 * struct mei_cl_device_id - MEI client device identifier
 * @name: helper name
 * @uuid: client uuid
 * @version: client protocol version
 * @driver_info: information used by the driver.
 *
 * identifies mei client device by uuid and name
 */
struct mei_cl_device_id {
	char name[MEI_CL_NAME_SIZE];
	uuid_le uuid;
	__u8    version;
	kernel_ulong_t driver_info;
};

/* RapidIO */

#define RIO_ANY_ID	0xffff

/**
 * struct rio_device_id - RIO device identifier
 * @did: RapidIO device ID
 * @vid: RapidIO vendor ID
 * @asm_did: RapidIO assembly device ID
 * @asm_vid: RapidIO assembly vendor ID
 *
 * Identifies a RapidIO device based on both the device/vendor IDs and
 * the assembly device/vendor IDs.
 */
struct rio_device_id {
	__u16 did, vid;
	__u16 asm_did, asm_vid;
};

struct mcb_device_id {
	__u16 device;
	kernel_ulong_t driver_data;
};

struct ulpi_device_id {
	__u16 vendor;
	__u16 product;
	kernel_ulong_t driver_data;
};

/**
 * struct fsl_mc_device_id - MC object device identifier
 * @vendor: vendor ID
 * @obj_type: MC object type
 *
 * Type of entries in the "device Id" table for MC object devices supported by
 * a MC object device driver. The last entry of the table has vendor set to 0x0
 */
struct fsl_mc_device_id {
	__u16 vendor;
	const char obj_type[16];
};

/**
 * struct tb_service_id - Thunderbolt service identifiers
 * @match_flags: Flags used to match the structure
 * @protocol_key: Protocol key the service supports
 * @protocol_id: Protocol id the service supports
 * @protocol_version: Version of the protocol
 * @protocol_revision: Revision of the protocol software
 * @driver_data: Driver specific data
 *
 * Thunderbolt XDomain services are exposed as devices where each device
 * carries the protocol information the service supports. Thunderbolt
 * XDomain service drivers match against that information.
 */
struct tb_service_id {
	__u32 match_flags;
	char protocol_key[8 + 1];
	__u32 protocol_id;
	__u32 protocol_version;
	__u32 protocol_revision;
	kernel_ulong_t driver_data;
};

#define TBSVC_MATCH_PROTOCOL_KEY	0x0001
#define TBSVC_MATCH_PROTOCOL_ID		0x0002
#define TBSVC_MATCH_PROTOCOL_VERSION	0x0004
#define TBSVC_MATCH_PROTOCOL_REVISION	0x0008

/* USB Type-C Alternate Modes */

#define TYPEC_ANY_MODE	0x7

/**
 * struct typec_device_id - USB Type-C alternate mode identifiers
 * @svid: Standard or Vendor ID
 * @mode: Mode index
 * @driver_data: Driver specific data
 */
struct typec_device_id {
	__u16 svid;
	__u8 mode;
	kernel_ulong_t driver_data;
};

/**
 * struct tee_client_device_id - tee based device identifier
 * @uuid: For TEE based client devices we use the device uuid as
 *        the identifier.
 */
struct tee_client_device_id {
	uuid_t uuid;
};

/* WMI */

#define WMI_MODULE_PREFIX	"wmi:"

/**
 * struct wmi_device_id - WMI device identifier
 * @guid_string: 36 char string of the form fa50ff2b-f2e8-45de-83fa-65417f2f49ba
 * @context: pointer to driver specific data
 */
struct wmi_device_id {
	const char guid_string[UUID_STRING_LEN+1];
	const void *context;
};

#define MHI_DEVICE_MODALIAS_FMT "mhi:%s"
#define MHI_NAME_SIZE 32

/**
 * struct mhi_device_id - MHI device identification
 * @chan: MHI channel name
 * @driver_data: driver data;
 */
struct mhi_device_id {
	const char chan[MHI_NAME_SIZE];
	kernel_ulong_t driver_data;
};

#define AUXILIARY_NAME_SIZE 32
#define AUXILIARY_MODULE_PREFIX "auxiliary:"

struct auxiliary_device_id {
	char name[AUXILIARY_NAME_SIZE];
	kernel_ulong_t driver_data;
};

/* Surface System Aggregator Module */

#define SSAM_MATCH_TARGET	0x1
#define SSAM_MATCH_INSTANCE	0x2
#define SSAM_MATCH_FUNCTION	0x4

struct ssam_device_id {
	__u8 match_flags;

	__u8 domain;
	__u8 category;
	__u8 target;
	__u8 instance;
	__u8 function;

	kernel_ulong_t driver_data;
};

/*
 * DFL (Device Feature List)
 *
 * DFL defines a linked list of feature headers within the device MMIO space to
 * provide an extensible way of adding features. Software can walk through these
 * predefined data structures to enumerate features. It is now used in the FPGA.
 * See Documentation/fpga/dfl.rst for more information.
 *
 * The dfl bus type is introduced to match the individual feature devices (dfl
 * devices) for specific dfl drivers.
 */

/**
 * struct dfl_device_id -  dfl device identifier
 * @type: DFL FIU type of the device. See enum dfl_id_type.
 * @feature_id: feature identifier local to its DFL FIU type.
 * @driver_data: driver specific data.
 */
struct dfl_device_id {
	__u16 type;
	__u16 feature_id;
	kernel_ulong_t driver_data;
};

#endif /* LINUX_MOD_DEVICETABLE_H */
