/* SPDX-License-Identifier: GPL-2.0 */
struct acpi_smb_hc;
enum acpi_smb_protocol {
	SMBUS_WRITE_QUICK = 2,
	SMBUS_READ_QUICK = 3,
	SMBUS_SEND_BYTE = 4,
	SMBUS_RECEIVE_BYTE = 5,
	SMBUS_WRITE_BYTE = 6,
	SMBUS_READ_BYTE = 7,
	SMBUS_WRITE_WORD  = 8,
	SMBUS_READ_WORD  = 9,
	SMBUS_WRITE_BLOCK = 0xa,
	SMBUS_READ_BLOCK = 0xb,
	SMBUS_PROCESS_CALL = 0xc,
	SMBUS_BLOCK_PROCESS_CALL = 0xd,
};

enum acpi_sbs_device_addr {
	ACPI_SBS_CHARGER = 0x9,
	ACPI_SBS_MANAGER = 0xa,
	ACPI_SBS_BATTERY = 0xb,
};

typedef void (*smbus_alarm_callback)(void *context);

extern int acpi_smbus_read(struct acpi_smb_hc *hc, u8 protocol, u8 address,
		u8 command, u8 *data);
extern int acpi_smbus_write(struct acpi_smb_hc *hc, u8 protocol, u8 slave_address,
		u8 command, u8 *data, u8 length);
extern int acpi_smbus_register_callback(struct acpi_smb_hc *hc,
		smbus_alarm_callback callback, void *context);
extern int acpi_smbus_unregister_callback(struct acpi_smb_hc *hc);
