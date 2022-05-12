/* SPDX-License-Identifier: BSD-3-Clause OR GPL-2.0 */
/******************************************************************************
 *
 * Module Name: amlresrc.h - AML resource descriptors
 *
 * Copyright (C) 2000 - 2021, Intel Corp.
 *
 *****************************************************************************/

/* acpisrc:struct_defs -- for acpisrc conversion */

#ifndef __AMLRESRC_H
#define __AMLRESRC_H

/*
 * Resource descriptor tags, as defined in the ACPI specification.
 * Used to symbolically reference fields within a descriptor.
 */
#define ACPI_RESTAG_ADDRESS                     "_ADR"
#define ACPI_RESTAG_ALIGNMENT                   "_ALN"
#define ACPI_RESTAG_ADDRESSSPACE                "_ASI"
#define ACPI_RESTAG_ACCESSSIZE                  "_ASZ"
#define ACPI_RESTAG_TYPESPECIFICATTRIBUTES      "_ATT"
#define ACPI_RESTAG_BASEADDRESS                 "_BAS"
#define ACPI_RESTAG_BUSMASTER                   "_BM_"	/* Master(1), Slave(0) */
#define ACPI_RESTAG_DEBOUNCETIME                "_DBT"
#define ACPI_RESTAG_DECODE                      "_DEC"
#define ACPI_RESTAG_DEVICEPOLARITY              "_DPL"
#define ACPI_RESTAG_DMA                         "_DMA"
#define ACPI_RESTAG_DMATYPE                     "_TYP"	/* Compatible(0), A(1), B(2), F(3) */
#define ACPI_RESTAG_DRIVESTRENGTH               "_DRS"
#define ACPI_RESTAG_ENDIANNESS                  "_END"
#define ACPI_RESTAG_FLOWCONTROL                 "_FLC"
#define ACPI_RESTAG_FUNCTION                    "_FUN"
#define ACPI_RESTAG_GRANULARITY                 "_GRA"
#define ACPI_RESTAG_INTERRUPT                   "_INT"
#define ACPI_RESTAG_INTERRUPTLEVEL              "_LL_"	/* active_lo(1), active_hi(0) */
#define ACPI_RESTAG_INTERRUPTSHARE              "_SHR"	/* Shareable(1), no_share(0) */
#define ACPI_RESTAG_INTERRUPTTYPE               "_HE_"	/* Edge(1), Level(0) */
#define ACPI_RESTAG_IORESTRICTION               "_IOR"
#define ACPI_RESTAG_LENGTH                      "_LEN"
#define ACPI_RESTAG_LINE                        "_LIN"
#define ACPI_RESTAG_LOCALPORT                   "_PRT"
#define ACPI_RESTAG_MEMATTRIBUTES               "_MTP"	/* Memory(0), Reserved(1), ACPI(2), NVS(3) */
#define ACPI_RESTAG_MEMTYPE                     "_MEM"	/* non_cache(0), Cacheable(1) Cache+combine(2), Cache+prefetch(3) */
#define ACPI_RESTAG_MAXADDR                     "_MAX"
#define ACPI_RESTAG_MINADDR                     "_MIN"
#define ACPI_RESTAG_MAXTYPE                     "_MAF"
#define ACPI_RESTAG_MINTYPE                     "_MIF"
#define ACPI_RESTAG_MODE                        "_MOD"
#define ACPI_RESTAG_PARITY                      "_PAR"
#define ACPI_RESTAG_PHASE                       "_PHA"
#define ACPI_RESTAG_PHYTYPE                     "_PHY"
#define ACPI_RESTAG_PIN                         "_PIN"
#define ACPI_RESTAG_PINCONFIG                   "_PPI"
#define ACPI_RESTAG_PINCONFIG_TYPE              "_TYP"
#define ACPI_RESTAG_PINCONFIG_VALUE             "_VAL"
#define ACPI_RESTAG_POLARITY                    "_POL"
#define ACPI_RESTAG_REGISTERBITOFFSET           "_RBO"
#define ACPI_RESTAG_REGISTERBITWIDTH            "_RBW"
#define ACPI_RESTAG_RANGETYPE                   "_RNG"
#define ACPI_RESTAG_READWRITETYPE               "_RW_"	/* read_only(0), Writeable (1) */
#define ACPI_RESTAG_LENGTH_RX                   "_RXL"
#define ACPI_RESTAG_LENGTH_TX                   "_TXL"
#define ACPI_RESTAG_SLAVEMODE                   "_SLV"
#define ACPI_RESTAG_SPEED                       "_SPE"
#define ACPI_RESTAG_STOPBITS                    "_STB"
#define ACPI_RESTAG_TRANSLATION                 "_TRA"
#define ACPI_RESTAG_TRANSTYPE                   "_TRS"	/* Sparse(1), Dense(0) */
#define ACPI_RESTAG_TYPE                        "_TTP"	/* Translation(1), Static (0) */
#define ACPI_RESTAG_XFERTYPE                    "_SIZ"	/* 8(0), 8And16(1), 16(2) */
#define ACPI_RESTAG_VENDORDATA                  "_VEN"

/* Default sizes for "small" resource descriptors */

#define ASL_RDESC_IRQ_SIZE                      0x02
#define ASL_RDESC_DMA_SIZE                      0x02
#define ASL_RDESC_ST_DEPEND_SIZE                0x00
#define ASL_RDESC_END_DEPEND_SIZE               0x00
#define ASL_RDESC_IO_SIZE                       0x07
#define ASL_RDESC_FIXED_IO_SIZE                 0x03
#define ASL_RDESC_FIXED_DMA_SIZE                0x05
#define ASL_RDESC_END_TAG_SIZE                  0x01

struct asl_resource_node {
	u32 buffer_length;
	void *buffer;
	struct asl_resource_node *next;
};

struct asl_resource_info {
	union acpi_parse_object *descriptor_type_op;	/* Resource descriptor parse node */
	union acpi_parse_object *mapping_op;	/* Used for mapfile support */
	u32 current_byte_offset;	/* Offset in resource template */
};

/* Macros used to generate AML resource length fields */

#define ACPI_AML_SIZE_LARGE(r)      (sizeof (r) - sizeof (struct aml_resource_large_header))
#define ACPI_AML_SIZE_SMALL(r)      (sizeof (r) - sizeof (struct aml_resource_small_header))

/*
 * Resource descriptors defined in the ACPI specification.
 *
 * Packing/alignment must be BYTE because these descriptors
 * are used to overlay the raw AML byte stream.
 */
#pragma pack(1)

/*
 * SMALL descriptors
 */
#define AML_RESOURCE_SMALL_HEADER_COMMON \
	u8                              descriptor_type;

struct aml_resource_small_header {
AML_RESOURCE_SMALL_HEADER_COMMON};

struct aml_resource_irq {
	AML_RESOURCE_SMALL_HEADER_COMMON u16 irq_mask;
	u8 flags;
};

struct aml_resource_irq_noflags {
	AML_RESOURCE_SMALL_HEADER_COMMON u16 irq_mask;
};

struct aml_resource_dma {
	AML_RESOURCE_SMALL_HEADER_COMMON u8 dma_channel_mask;
	u8 flags;
};

struct aml_resource_start_dependent {
	AML_RESOURCE_SMALL_HEADER_COMMON u8 flags;
};

struct aml_resource_start_dependent_noprio {
AML_RESOURCE_SMALL_HEADER_COMMON};

struct aml_resource_end_dependent {
AML_RESOURCE_SMALL_HEADER_COMMON};

struct aml_resource_io {
	AML_RESOURCE_SMALL_HEADER_COMMON u8 flags;
	u16 minimum;
	u16 maximum;
	u8 alignment;
	u8 address_length;
};

struct aml_resource_fixed_io {
	AML_RESOURCE_SMALL_HEADER_COMMON u16 address;
	u8 address_length;
};

struct aml_resource_vendor_small {
AML_RESOURCE_SMALL_HEADER_COMMON};

struct aml_resource_end_tag {
	AML_RESOURCE_SMALL_HEADER_COMMON u8 checksum;
};

struct aml_resource_fixed_dma {
	AML_RESOURCE_SMALL_HEADER_COMMON u16 request_lines;
	u16 channels;
	u8 width;
};

/*
 * LARGE descriptors
 */
#define AML_RESOURCE_LARGE_HEADER_COMMON \
	u8                              descriptor_type;\
	u16                             resource_length;

struct aml_resource_large_header {
AML_RESOURCE_LARGE_HEADER_COMMON};

/* General Flags for address space resource descriptors */

#define ACPI_RESOURCE_FLAG_DEC      2
#define ACPI_RESOURCE_FLAG_MIF      4
#define ACPI_RESOURCE_FLAG_MAF      8

struct aml_resource_memory24 {
	AML_RESOURCE_LARGE_HEADER_COMMON u8 flags;
	u16 minimum;
	u16 maximum;
	u16 alignment;
	u16 address_length;
};

struct aml_resource_vendor_large {
AML_RESOURCE_LARGE_HEADER_COMMON};

struct aml_resource_memory32 {
	AML_RESOURCE_LARGE_HEADER_COMMON u8 flags;
	u32 minimum;
	u32 maximum;
	u32 alignment;
	u32 address_length;
};

struct aml_resource_fixed_memory32 {
	AML_RESOURCE_LARGE_HEADER_COMMON u8 flags;
	u32 address;
	u32 address_length;
};

#define AML_RESOURCE_ADDRESS_COMMON \
	u8                              resource_type; \
	u8                              flags; \
	u8                              specific_flags;

struct aml_resource_address {
AML_RESOURCE_LARGE_HEADER_COMMON AML_RESOURCE_ADDRESS_COMMON};

struct aml_resource_extended_address64 {
	AML_RESOURCE_LARGE_HEADER_COMMON
	    AML_RESOURCE_ADDRESS_COMMON u8 revision_ID;
	u8 reserved;
	u64 granularity;
	u64 minimum;
	u64 maximum;
	u64 translation_offset;
	u64 address_length;
	u64 type_specific;
};

#define AML_RESOURCE_EXTENDED_ADDRESS_REVISION          1	/* ACPI 3.0 */

struct aml_resource_address64 {
	AML_RESOURCE_LARGE_HEADER_COMMON
	    AML_RESOURCE_ADDRESS_COMMON u64 granularity;
	u64 minimum;
	u64 maximum;
	u64 translation_offset;
	u64 address_length;
};

struct aml_resource_address32 {
	AML_RESOURCE_LARGE_HEADER_COMMON
	    AML_RESOURCE_ADDRESS_COMMON u32 granularity;
	u32 minimum;
	u32 maximum;
	u32 translation_offset;
	u32 address_length;
};

struct aml_resource_address16 {
	AML_RESOURCE_LARGE_HEADER_COMMON
	    AML_RESOURCE_ADDRESS_COMMON u16 granularity;
	u16 minimum;
	u16 maximum;
	u16 translation_offset;
	u16 address_length;
};

struct aml_resource_extended_irq {
	AML_RESOURCE_LARGE_HEADER_COMMON u8 flags;
	u8 interrupt_count;
	u32 interrupts[1];
	/* res_source_index, res_source optional fields follow */
};

struct aml_resource_generic_register {
	AML_RESOURCE_LARGE_HEADER_COMMON u8 address_space_id;
	u8 bit_width;
	u8 bit_offset;
	u8 access_size;		/* ACPI 3.0, was previously Reserved */
	u64 address;
};

/* Common descriptor for gpio_int and gpio_io (ACPI 5.0) */

struct aml_resource_gpio {
	AML_RESOURCE_LARGE_HEADER_COMMON u8 revision_id;
	u8 connection_type;
	u16 flags;
	u16 int_flags;
	u8 pin_config;
	u16 drive_strength;
	u16 debounce_timeout;
	u16 pin_table_offset;
	u8 res_source_index;
	u16 res_source_offset;
	u16 vendor_offset;
	u16 vendor_length;
	/*
	 * Optional fields follow immediately:
	 * 1) PIN list (Words)
	 * 2) Resource Source String
	 * 3) Vendor Data bytes
	 */
};

#define AML_RESOURCE_GPIO_REVISION              1	/* ACPI 5.0 */

/* Values for connection_type above */

#define AML_RESOURCE_GPIO_TYPE_INT              0
#define AML_RESOURCE_GPIO_TYPE_IO               1
#define AML_RESOURCE_MAX_GPIOTYPE               1

/* Common preamble for all serial descriptors (ACPI 5.0) */

#define AML_RESOURCE_SERIAL_COMMON \
	u8                              revision_id; \
	u8                              res_source_index; \
	u8                              type; \
	u8                              flags; \
	u16                             type_specific_flags; \
	u8                              type_revision_id; \
	u16                             type_data_length; \

/* Values for the type field above */

#define AML_RESOURCE_I2C_SERIALBUSTYPE          1
#define AML_RESOURCE_SPI_SERIALBUSTYPE          2
#define AML_RESOURCE_UART_SERIALBUSTYPE         3
#define AML_RESOURCE_CSI2_SERIALBUSTYPE         4
#define AML_RESOURCE_MAX_SERIALBUSTYPE          4
#define AML_RESOURCE_VENDOR_SERIALBUSTYPE       192	/* Vendor defined is 0xC0-0xFF (NOT SUPPORTED) */

struct aml_resource_common_serialbus {
AML_RESOURCE_LARGE_HEADER_COMMON AML_RESOURCE_SERIAL_COMMON};

struct aml_resource_csi2_serialbus {
	AML_RESOURCE_LARGE_HEADER_COMMON AML_RESOURCE_SERIAL_COMMON
	    /*
	     * Optional fields follow immediately:
	     * 1) Vendor Data bytes
	     * 2) Resource Source String
	     */
};

#define AML_RESOURCE_CSI2_REVISION              1	/* ACPI 6.4 */
#define AML_RESOURCE_CSI2_TYPE_REVISION         1	/* ACPI 6.4 */
#define AML_RESOURCE_CSI2_MIN_DATA_LEN          0	/* ACPI 6.4 */

struct aml_resource_i2c_serialbus {
	AML_RESOURCE_LARGE_HEADER_COMMON
	    AML_RESOURCE_SERIAL_COMMON u32 connection_speed;
	u16 slave_address;
	/*
	 * Optional fields follow immediately:
	 * 1) Vendor Data bytes
	 * 2) Resource Source String
	 */
};

#define AML_RESOURCE_I2C_REVISION               1	/* ACPI 5.0 */
#define AML_RESOURCE_I2C_TYPE_REVISION          1	/* ACPI 5.0 */
#define AML_RESOURCE_I2C_MIN_DATA_LEN           6

struct aml_resource_spi_serialbus {
	AML_RESOURCE_LARGE_HEADER_COMMON
	    AML_RESOURCE_SERIAL_COMMON u32 connection_speed;
	u8 data_bit_length;
	u8 clock_phase;
	u8 clock_polarity;
	u16 device_selection;
	/*
	 * Optional fields follow immediately:
	 * 1) Vendor Data bytes
	 * 2) Resource Source String
	 */
};

#define AML_RESOURCE_SPI_REVISION               1	/* ACPI 5.0 */
#define AML_RESOURCE_SPI_TYPE_REVISION          1	/* ACPI 5.0 */
#define AML_RESOURCE_SPI_MIN_DATA_LEN           9

struct aml_resource_uart_serialbus {
	AML_RESOURCE_LARGE_HEADER_COMMON
	    AML_RESOURCE_SERIAL_COMMON u32 default_baud_rate;
	u16 rx_fifo_size;
	u16 tx_fifo_size;
	u8 parity;
	u8 lines_enabled;
	/*
	 * Optional fields follow immediately:
	 * 1) Vendor Data bytes
	 * 2) Resource Source String
	 */
};

#define AML_RESOURCE_UART_REVISION              1	/* ACPI 5.0 */
#define AML_RESOURCE_UART_TYPE_REVISION         1	/* ACPI 5.0 */
#define AML_RESOURCE_UART_MIN_DATA_LEN          10

struct aml_resource_pin_function {
	AML_RESOURCE_LARGE_HEADER_COMMON u8 revision_id;
	u16 flags;
	u8 pin_config;
	u16 function_number;
	u16 pin_table_offset;
	u8 res_source_index;
	u16 res_source_offset;
	u16 vendor_offset;
	u16 vendor_length;
	/*
	 * Optional fields follow immediately:
	 * 1) PIN list (Words)
	 * 2) Resource Source String
	 * 3) Vendor Data bytes
	 */
};

#define AML_RESOURCE_PIN_FUNCTION_REVISION      1	/* ACPI 6.2 */

struct aml_resource_pin_config {
	AML_RESOURCE_LARGE_HEADER_COMMON u8 revision_id;
	u16 flags;
	u8 pin_config_type;
	u32 pin_config_value;
	u16 pin_table_offset;
	u8 res_source_index;
	u16 res_source_offset;
	u16 vendor_offset;
	u16 vendor_length;
	/*
	 * Optional fields follow immediately:
	 * 1) PIN list (Words)
	 * 2) Resource Source String
	 * 3) Vendor Data bytes
	 */
};

#define AML_RESOURCE_PIN_CONFIG_REVISION      1	/* ACPI 6.2 */

struct aml_resource_pin_group {
	AML_RESOURCE_LARGE_HEADER_COMMON u8 revision_id;
	u16 flags;
	u16 pin_table_offset;
	u16 label_offset;
	u16 vendor_offset;
	u16 vendor_length;
	/*
	 * Optional fields follow immediately:
	 * 1) PIN list (Words)
	 * 2) Resource Label String
	 * 3) Vendor Data bytes
	 */
};

#define AML_RESOURCE_PIN_GROUP_REVISION      1	/* ACPI 6.2 */

struct aml_resource_pin_group_function {
	AML_RESOURCE_LARGE_HEADER_COMMON u8 revision_id;
	u16 flags;
	u16 function_number;
	u8 res_source_index;
	u16 res_source_offset;
	u16 res_source_label_offset;
	u16 vendor_offset;
	u16 vendor_length;
	/*
	 * Optional fields follow immediately:
	 * 1) Resource Source String
	 * 2) Resource Source Label String
	 * 3) Vendor Data bytes
	 */
};

#define AML_RESOURCE_PIN_GROUP_FUNCTION_REVISION    1	/* ACPI 6.2 */

struct aml_resource_pin_group_config {
	AML_RESOURCE_LARGE_HEADER_COMMON u8 revision_id;
	u16 flags;
	u8 pin_config_type;
	u32 pin_config_value;
	u8 res_source_index;
	u16 res_source_offset;
	u16 res_source_label_offset;
	u16 vendor_offset;
	u16 vendor_length;
	/*
	 * Optional fields follow immediately:
	 * 1) Resource Source String
	 * 2) Resource Source Label String
	 * 3) Vendor Data bytes
	 */
};

#define AML_RESOURCE_PIN_GROUP_CONFIG_REVISION    1	/* ACPI 6.2 */

/* restore default alignment */

#pragma pack()

/* Union of all resource descriptors, so we can allocate the worst case */

union aml_resource {
	/* Descriptor headers */

	u8 descriptor_type;
	struct aml_resource_small_header small_header;
	struct aml_resource_large_header large_header;

	/* Small resource descriptors */

	struct aml_resource_irq irq;
	struct aml_resource_dma dma;
	struct aml_resource_start_dependent start_dpf;
	struct aml_resource_end_dependent end_dpf;
	struct aml_resource_io io;
	struct aml_resource_fixed_io fixed_io;
	struct aml_resource_fixed_dma fixed_dma;
	struct aml_resource_vendor_small vendor_small;
	struct aml_resource_end_tag end_tag;

	/* Large resource descriptors */

	struct aml_resource_memory24 memory24;
	struct aml_resource_generic_register generic_reg;
	struct aml_resource_vendor_large vendor_large;
	struct aml_resource_memory32 memory32;
	struct aml_resource_fixed_memory32 fixed_memory32;
	struct aml_resource_address16 address16;
	struct aml_resource_address32 address32;
	struct aml_resource_address64 address64;
	struct aml_resource_extended_address64 ext_address64;
	struct aml_resource_extended_irq extended_irq;
	struct aml_resource_gpio gpio;
	struct aml_resource_i2c_serialbus i2c_serial_bus;
	struct aml_resource_spi_serialbus spi_serial_bus;
	struct aml_resource_uart_serialbus uart_serial_bus;
	struct aml_resource_csi2_serialbus csi2_serial_bus;
	struct aml_resource_common_serialbus common_serial_bus;
	struct aml_resource_pin_function pin_function;
	struct aml_resource_pin_config pin_config;
	struct aml_resource_pin_group pin_group;
	struct aml_resource_pin_group_function pin_group_function;
	struct aml_resource_pin_group_config pin_group_config;

	/* Utility overlays */

	struct aml_resource_address address;
	u32 dword_item;
	u16 word_item;
	u8 byte_item;
};

/* Interfaces used by both the disassembler and compiler */

void
mp_save_gpio_info(union acpi_parse_object *op,
		  union aml_resource *resource,
		  u32 pin_count, u16 *pin_list, char *device_name);

void
mp_save_serial_info(union acpi_parse_object *op,
		    union aml_resource *resource, char *device_name);

char *mp_get_hid_from_parse_tree(struct acpi_namespace_node *hid_node);

char *mp_get_hid_via_namestring(char *device_name);

char *mp_get_connection_info(union acpi_parse_object *op,
			     u32 pin_index,
			     struct acpi_namespace_node **target_node,
			     char **target_name);

char *mp_get_parent_device_hid(union acpi_parse_object *op,
			       struct acpi_namespace_node **target_node,
			       char **parent_device_name);

char *mp_get_ddn_value(char *device_name);

char *mp_get_hid_value(struct acpi_namespace_node *device_node);

#endif
