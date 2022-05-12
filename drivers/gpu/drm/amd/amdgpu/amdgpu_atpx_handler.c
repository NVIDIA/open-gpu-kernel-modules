// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2010 Red Hat Inc.
 * Author : Dave Airlie <airlied@redhat.com>
 *
 * ATPX support for both Intel/ATI
 */
#include <linux/vga_switcheroo.h>
#include <linux/slab.h>
#include <linux/acpi.h>
#include <linux/pci.h>
#include <linux/delay.h>

#include "amdgpu.h"
#include "amd_acpi.h"

#define AMDGPU_PX_QUIRK_FORCE_ATPX  (1 << 0)

struct amdgpu_px_quirk {
	u32 chip_vendor;
	u32 chip_device;
	u32 subsys_vendor;
	u32 subsys_device;
	u32 px_quirk_flags;
};

struct amdgpu_atpx_functions {
	bool px_params;
	bool power_cntl;
	bool disp_mux_cntl;
	bool i2c_mux_cntl;
	bool switch_start;
	bool switch_end;
	bool disp_connectors_mapping;
	bool disp_detection_ports;
};

struct amdgpu_atpx {
	acpi_handle handle;
	struct amdgpu_atpx_functions functions;
	bool is_hybrid;
	bool dgpu_req_power_for_displays;
};

static struct amdgpu_atpx_priv {
	bool atpx_detected;
#ifdef AMDKCL_PCIE_BRIDGE_PM_USABLE
	bool bridge_pm_usable;
#endif
	unsigned int quirks;
	/* handle for device - and atpx */
	acpi_handle dhandle;
	acpi_handle other_handle;
	struct amdgpu_atpx atpx;
} amdgpu_atpx_priv;

struct atpx_verify_interface {
	u16 size;		/* structure size in bytes (includes size field) */
	u16 version;		/* version */
	u32 function_bits;	/* supported functions bit vector */
} __packed;

struct atpx_px_params {
	u16 size;		/* structure size in bytes (includes size field) */
	u32 valid_flags;	/* which flags are valid */
	u32 flags;		/* flags */
} __packed;

struct atpx_power_control {
	u16 size;
	u8 dgpu_state;
} __packed;

struct atpx_mux {
	u16 size;
	u16 mux;
} __packed;

bool amdgpu_has_atpx(void) {
	return amdgpu_atpx_priv.atpx_detected;
}

bool amdgpu_has_atpx_dgpu_power_cntl(void) {
	return amdgpu_atpx_priv.atpx.functions.power_cntl;
}

bool amdgpu_is_atpx_hybrid(void) {
	return amdgpu_atpx_priv.atpx.is_hybrid;
}

bool amdgpu_atpx_dgpu_req_power_for_displays(void) {
	return amdgpu_atpx_priv.atpx.dgpu_req_power_for_displays;
}

#if defined(CONFIG_ACPI)
void *amdgpu_atpx_get_dhandle(void) {
	return amdgpu_atpx_priv.dhandle;
}
#endif

/**
 * amdgpu_atpx_call - call an ATPX method
 *
 * @handle: acpi handle
 * @function: the ATPX function to execute
 * @params: ATPX function params
 *
 * Executes the requested ATPX function (all asics).
 * Returns a pointer to the acpi output buffer.
 */
static union acpi_object *amdgpu_atpx_call(acpi_handle handle, int function,
					   struct acpi_buffer *params)
{
	acpi_status status;
	union acpi_object atpx_arg_elements[2];
	struct acpi_object_list atpx_arg;
	struct acpi_buffer buffer = { ACPI_ALLOCATE_BUFFER, NULL };

	atpx_arg.count = 2;
	atpx_arg.pointer = &atpx_arg_elements[0];

	atpx_arg_elements[0].type = ACPI_TYPE_INTEGER;
	atpx_arg_elements[0].integer.value = function;

	if (params) {
		atpx_arg_elements[1].type = ACPI_TYPE_BUFFER;
		atpx_arg_elements[1].buffer.length = params->length;
		atpx_arg_elements[1].buffer.pointer = params->pointer;
	} else {
		/* We need a second fake parameter */
		atpx_arg_elements[1].type = ACPI_TYPE_INTEGER;
		atpx_arg_elements[1].integer.value = 0;
	}

	status = acpi_evaluate_object(handle, NULL, &atpx_arg, &buffer);

	/* Fail only if calling the method fails and ATPX is supported */
	if (ACPI_FAILURE(status) && status != AE_NOT_FOUND) {
		printk("failed to evaluate ATPX got %s\n",
		       acpi_format_exception(status));
		kfree(buffer.pointer);
		return NULL;
	}

	return buffer.pointer;
}

/**
 * amdgpu_atpx_parse_functions - parse supported functions
 *
 * @f: supported functions struct
 * @mask: supported functions mask from ATPX
 *
 * Use the supported functions mask from ATPX function
 * ATPX_FUNCTION_VERIFY_INTERFACE to determine what functions
 * are supported (all asics).
 */
static void amdgpu_atpx_parse_functions(struct amdgpu_atpx_functions *f, u32 mask)
{
	f->px_params = mask & ATPX_GET_PX_PARAMETERS_SUPPORTED;
	f->power_cntl = mask & ATPX_POWER_CONTROL_SUPPORTED;
	f->disp_mux_cntl = mask & ATPX_DISPLAY_MUX_CONTROL_SUPPORTED;
	f->i2c_mux_cntl = mask & ATPX_I2C_MUX_CONTROL_SUPPORTED;
	f->switch_start = mask & ATPX_GRAPHICS_DEVICE_SWITCH_START_NOTIFICATION_SUPPORTED;
	f->switch_end = mask & ATPX_GRAPHICS_DEVICE_SWITCH_END_NOTIFICATION_SUPPORTED;
	f->disp_connectors_mapping = mask & ATPX_GET_DISPLAY_CONNECTORS_MAPPING_SUPPORTED;
	f->disp_detection_ports = mask & ATPX_GET_DISPLAY_DETECTION_PORTS_SUPPORTED;
}

/**
 * amdgpu_atpx_validate - validate ATPX functions
 *
 * @atpx: amdgpu atpx struct
 *
 * Validate that required functions are enabled (all asics).
 * returns 0 on success, error on failure.
 */
static int amdgpu_atpx_validate(struct amdgpu_atpx *atpx)
{
	u32 valid_bits = 0;

	if (atpx->functions.px_params) {
		union acpi_object *info;
		struct atpx_px_params output;
		size_t size;

		info = amdgpu_atpx_call(atpx->handle, ATPX_FUNCTION_GET_PX_PARAMETERS, NULL);
		if (!info)
			return -EIO;

		memset(&output, 0, sizeof(output));

		size = *(u16 *) info->buffer.pointer;
		if (size < 10) {
			printk("ATPX buffer is too small: %zu\n", size);
			kfree(info);
			return -EINVAL;
		}
		size = min(sizeof(output), size);

		memcpy(&output, info->buffer.pointer, size);

		valid_bits = output.flags & output.valid_flags;

		kfree(info);
	}

	/* if separate mux flag is set, mux controls are required */
	if (valid_bits & ATPX_SEPARATE_MUX_FOR_I2C) {
		atpx->functions.i2c_mux_cntl = true;
		atpx->functions.disp_mux_cntl = true;
	}
	/* if any outputs are muxed, mux controls are required */
	if (valid_bits & (ATPX_CRT1_RGB_SIGNAL_MUXED |
			  ATPX_TV_SIGNAL_MUXED |
			  ATPX_DFP_SIGNAL_MUXED))
		atpx->functions.disp_mux_cntl = true;


	/* some bioses set these bits rather than flagging power_cntl as supported */
	if (valid_bits & (ATPX_DYNAMIC_PX_SUPPORTED |
			  ATPX_DYNAMIC_DGPU_POWER_OFF_SUPPORTED))
		atpx->functions.power_cntl = true;

	atpx->is_hybrid = false;
	if (valid_bits & ATPX_MS_HYBRID_GFX_SUPPORTED) {
		if (amdgpu_atpx_priv.quirks & AMDGPU_PX_QUIRK_FORCE_ATPX) {
			printk("ATPX Hybrid Graphics, forcing to ATPX\n");
			atpx->functions.power_cntl = true;
			atpx->is_hybrid = false;
		} else {
			printk("ATPX Hybrid Graphics\n");
#ifdef AMDKCL_PCIE_BRIDGE_PM_USABLE
			/*
			 * Disable legacy PM methods only when pcie port PM is usable,
			 * otherwise the device might fail to power off or power on.
			 */
			atpx->functions.power_cntl = !amdgpu_atpx_priv.bridge_pm_usable;
#else
                        /*
                         * This is a temporary hack for the kernel doesn't support D3.
                         */
			atpx->functions.power_cntl = true;
#endif
			atpx->is_hybrid = true;
		}
	}

	atpx->dgpu_req_power_for_displays = false;
	if (valid_bits & ATPX_DGPU_REQ_POWER_FOR_DISPLAYS)
		atpx->dgpu_req_power_for_displays = true;

	return 0;
}

/**
 * amdgpu_atpx_verify_interface - verify ATPX
 *
 * @atpx: amdgpu atpx struct
 *
 * Execute the ATPX_FUNCTION_VERIFY_INTERFACE ATPX function
 * to initialize ATPX and determine what features are supported
 * (all asics).
 * returns 0 on success, error on failure.
 */
static int amdgpu_atpx_verify_interface(struct amdgpu_atpx *atpx)
{
	union acpi_object *info;
	struct atpx_verify_interface output;
	size_t size;
	int err = 0;

	info = amdgpu_atpx_call(atpx->handle, ATPX_FUNCTION_VERIFY_INTERFACE, NULL);
	if (!info)
		return -EIO;

	memset(&output, 0, sizeof(output));

	size = *(u16 *) info->buffer.pointer;
	if (size < 8) {
		printk("ATPX buffer is too small: %zu\n", size);
		err = -EINVAL;
		goto out;
	}
	size = min(sizeof(output), size);

	memcpy(&output, info->buffer.pointer, size);

	/* TODO: check version? */
	printk("ATPX version %u, functions 0x%08x\n",
	       output.version, output.function_bits);

	amdgpu_atpx_parse_functions(&atpx->functions, output.function_bits);

out:
	kfree(info);
	return err;
}

/**
 * amdgpu_atpx_set_discrete_state - power up/down discrete GPU
 *
 * @atpx: atpx info struct
 * @state: discrete GPU state (0 = power down, 1 = power up)
 *
 * Execute the ATPX_FUNCTION_POWER_CONTROL ATPX function to
 * power down/up the discrete GPU (all asics).
 * Returns 0 on success, error on failure.
 */
static int amdgpu_atpx_set_discrete_state(struct amdgpu_atpx *atpx, u8 state)
{
	struct acpi_buffer params;
	union acpi_object *info;
	struct atpx_power_control input;

	if (atpx->functions.power_cntl) {
		input.size = 3;
		input.dgpu_state = state;
		params.length = input.size;
		params.pointer = &input;
		info = amdgpu_atpx_call(atpx->handle,
					ATPX_FUNCTION_POWER_CONTROL,
					&params);
		if (!info)
			return -EIO;
		kfree(info);

		/* 200ms delay is required after off */
		if (state == 0)
			msleep(200);
	}
	return 0;
}

/**
 * amdgpu_atpx_switch_disp_mux - switch display mux
 *
 * @atpx: atpx info struct
 * @mux_id: mux state (0 = integrated GPU, 1 = discrete GPU)
 *
 * Execute the ATPX_FUNCTION_DISPLAY_MUX_CONTROL ATPX function to
 * switch the display mux between the discrete GPU and integrated GPU
 * (all asics).
 * Returns 0 on success, error on failure.
 */
static int amdgpu_atpx_switch_disp_mux(struct amdgpu_atpx *atpx, u16 mux_id)
{
	struct acpi_buffer params;
	union acpi_object *info;
	struct atpx_mux input;

	if (atpx->functions.disp_mux_cntl) {
		input.size = 4;
		input.mux = mux_id;
		params.length = input.size;
		params.pointer = &input;
		info = amdgpu_atpx_call(atpx->handle,
					ATPX_FUNCTION_DISPLAY_MUX_CONTROL,
					&params);
		if (!info)
			return -EIO;
		kfree(info);
	}
	return 0;
}

/**
 * amdgpu_atpx_switch_i2c_mux - switch i2c/hpd mux
 *
 * @atpx: atpx info struct
 * @mux_id: mux state (0 = integrated GPU, 1 = discrete GPU)
 *
 * Execute the ATPX_FUNCTION_I2C_MUX_CONTROL ATPX function to
 * switch the i2c/hpd mux between the discrete GPU and integrated GPU
 * (all asics).
 * Returns 0 on success, error on failure.
 */
static int amdgpu_atpx_switch_i2c_mux(struct amdgpu_atpx *atpx, u16 mux_id)
{
	struct acpi_buffer params;
	union acpi_object *info;
	struct atpx_mux input;

	if (atpx->functions.i2c_mux_cntl) {
		input.size = 4;
		input.mux = mux_id;
		params.length = input.size;
		params.pointer = &input;
		info = amdgpu_atpx_call(atpx->handle,
					ATPX_FUNCTION_I2C_MUX_CONTROL,
					&params);
		if (!info)
			return -EIO;
		kfree(info);
	}
	return 0;
}

/**
 * amdgpu_atpx_switch_start - notify the sbios of a GPU switch
 *
 * @atpx: atpx info struct
 * @mux_id: mux state (0 = integrated GPU, 1 = discrete GPU)
 *
 * Execute the ATPX_FUNCTION_GRAPHICS_DEVICE_SWITCH_START_NOTIFICATION ATPX
 * function to notify the sbios that a switch between the discrete GPU and
 * integrated GPU has begun (all asics).
 * Returns 0 on success, error on failure.
 */
static int amdgpu_atpx_switch_start(struct amdgpu_atpx *atpx, u16 mux_id)
{
	struct acpi_buffer params;
	union acpi_object *info;
	struct atpx_mux input;

	if (atpx->functions.switch_start) {
		input.size = 4;
		input.mux = mux_id;
		params.length = input.size;
		params.pointer = &input;
		info = amdgpu_atpx_call(atpx->handle,
					ATPX_FUNCTION_GRAPHICS_DEVICE_SWITCH_START_NOTIFICATION,
					&params);
		if (!info)
			return -EIO;
		kfree(info);
	}
	return 0;
}

/**
 * amdgpu_atpx_switch_end - notify the sbios of a GPU switch
 *
 * @atpx: atpx info struct
 * @mux_id: mux state (0 = integrated GPU, 1 = discrete GPU)
 *
 * Execute the ATPX_FUNCTION_GRAPHICS_DEVICE_SWITCH_END_NOTIFICATION ATPX
 * function to notify the sbios that a switch between the discrete GPU and
 * integrated GPU has ended (all asics).
 * Returns 0 on success, error on failure.
 */
static int amdgpu_atpx_switch_end(struct amdgpu_atpx *atpx, u16 mux_id)
{
	struct acpi_buffer params;
	union acpi_object *info;
	struct atpx_mux input;

	if (atpx->functions.switch_end) {
		input.size = 4;
		input.mux = mux_id;
		params.length = input.size;
		params.pointer = &input;
		info = amdgpu_atpx_call(atpx->handle,
					ATPX_FUNCTION_GRAPHICS_DEVICE_SWITCH_END_NOTIFICATION,
					&params);
		if (!info)
			return -EIO;
		kfree(info);
	}
	return 0;
}

/**
 * amdgpu_atpx_switchto - switch to the requested GPU
 *
 * @id: GPU to switch to
 *
 * Execute the necessary ATPX functions to switch between the discrete GPU and
 * integrated GPU (all asics).
 * Returns 0 on success, error on failure.
 */
static int amdgpu_atpx_switchto(enum vga_switcheroo_client_id id)
{
	u16 gpu_id;

	if (id == VGA_SWITCHEROO_IGD)
		gpu_id = ATPX_INTEGRATED_GPU;
	else
		gpu_id = ATPX_DISCRETE_GPU;

	amdgpu_atpx_switch_start(&amdgpu_atpx_priv.atpx, gpu_id);
	amdgpu_atpx_switch_disp_mux(&amdgpu_atpx_priv.atpx, gpu_id);
	amdgpu_atpx_switch_i2c_mux(&amdgpu_atpx_priv.atpx, gpu_id);
	amdgpu_atpx_switch_end(&amdgpu_atpx_priv.atpx, gpu_id);

	return 0;
}

/**
 * amdgpu_atpx_power_state - power down/up the requested GPU
 *
 * @id: GPU to power down/up
 * @state: requested power state (0 = off, 1 = on)
 *
 * Execute the necessary ATPX function to power down/up the discrete GPU
 * (all asics).
 * Returns 0 on success, error on failure.
 */
static int amdgpu_atpx_power_state(enum vga_switcheroo_client_id id,
				   enum vga_switcheroo_state state)
{
	/* on w500 ACPI can't change intel gpu state */
	if (id == VGA_SWITCHEROO_IGD)
		return 0;

	amdgpu_atpx_set_discrete_state(&amdgpu_atpx_priv.atpx, state);
	return 0;
}

/**
 * amdgpu_atpx_pci_probe_handle - look up the ATPX handle
 *
 * @pdev: pci device
 *
 * Look up the ATPX handles (all asics).
 * Returns true if the handles are found, false if not.
 */
static bool amdgpu_atpx_pci_probe_handle(struct pci_dev *pdev)
{
	acpi_handle dhandle, atpx_handle;
	acpi_status status;

	dhandle = ACPI_HANDLE(&pdev->dev);
	if (!dhandle)
		return false;

	status = acpi_get_handle(dhandle, "ATPX", &atpx_handle);
	if (ACPI_FAILURE(status)) {
		amdgpu_atpx_priv.other_handle = dhandle;
		return false;
	}
	amdgpu_atpx_priv.dhandle = dhandle;
	amdgpu_atpx_priv.atpx.handle = atpx_handle;
	return true;
}

/**
 * amdgpu_atpx_init - verify the ATPX interface
 *
 * Verify the ATPX interface (all asics).
 * Returns 0 on success, error on failure.
 */
static int amdgpu_atpx_init(void)
{
	int r;

	/* set up the ATPX handle */
	r = amdgpu_atpx_verify_interface(&amdgpu_atpx_priv.atpx);
	if (r)
		return r;

	/* validate the atpx setup */
	r = amdgpu_atpx_validate(&amdgpu_atpx_priv.atpx);
	if (r)
		return r;

	return 0;
}

/**
 * amdgpu_atpx_get_client_id - get the client id
 *
 * @pdev: pci device
 *
 * look up whether we are the integrated or discrete GPU (all asics).
 * Returns the client id.
 */
static enum vga_switcheroo_client_id amdgpu_atpx_get_client_id(struct pci_dev *pdev)
{
	if (amdgpu_atpx_priv.dhandle == ACPI_HANDLE(&pdev->dev))
		return VGA_SWITCHEROO_IGD;
	else
		return VGA_SWITCHEROO_DIS;
}

static const struct vga_switcheroo_handler amdgpu_atpx_handler = {
	.switchto = amdgpu_atpx_switchto,
	.power_state = amdgpu_atpx_power_state,
	.get_client_id = amdgpu_atpx_get_client_id,
};

static const struct amdgpu_px_quirk amdgpu_px_quirk_list[] = {
	/* HG _PR3 doesn't seem to work on this A+A weston board */
	{ 0x1002, 0x6900, 0x1002, 0x0124, AMDGPU_PX_QUIRK_FORCE_ATPX },
	{ 0x1002, 0x6900, 0x1028, 0x0812, AMDGPU_PX_QUIRK_FORCE_ATPX },
	{ 0x1002, 0x6900, 0x1028, 0x0813, AMDGPU_PX_QUIRK_FORCE_ATPX },
	{ 0x1002, 0x699f, 0x1028, 0x0814, AMDGPU_PX_QUIRK_FORCE_ATPX },
	{ 0x1002, 0x6900, 0x1025, 0x125A, AMDGPU_PX_QUIRK_FORCE_ATPX },
	{ 0x1002, 0x6900, 0x17AA, 0x3806, AMDGPU_PX_QUIRK_FORCE_ATPX },
	{ 0, 0, 0, 0, 0 },
};

static void amdgpu_atpx_get_quirks(struct pci_dev *pdev)
{
	const struct amdgpu_px_quirk *p = amdgpu_px_quirk_list;

	/* Apply PX quirks */
	while (p && p->chip_device != 0) {
		if (pdev->vendor == p->chip_vendor &&
		    pdev->device == p->chip_device &&
		    pdev->subsystem_vendor == p->subsys_vendor &&
		    pdev->subsystem_device == p->subsys_device) {
			amdgpu_atpx_priv.quirks |= p->px_quirk_flags;
			break;
		}
		++p;
	}
}

/**
 * amdgpu_atpx_detect - detect whether we have PX
 *
 * Check if we have a PX system (all asics).
 * Returns true if we have a PX system, false if not.
 */
static bool amdgpu_atpx_detect(void)
{
	char acpi_method_name[255] = { 0 };
	struct acpi_buffer buffer = {sizeof(acpi_method_name), acpi_method_name};
	struct pci_dev *pdev = NULL;
	bool has_atpx = false;
	int vga_count = 0;
#ifdef AMDKCL_PCIE_BRIDGE_PM_USABLE
	bool d3_supported = false;
	struct pci_dev *parent_pdev;
#endif

	while ((pdev = pci_get_class(PCI_CLASS_DISPLAY_VGA << 8, pdev)) != NULL) {
		vga_count++;

		has_atpx |= amdgpu_atpx_pci_probe_handle(pdev);

#ifdef AMDKCL_PCIE_BRIDGE_PM_USABLE
		parent_pdev = pci_upstream_bridge(pdev);
		d3_supported |= parent_pdev && parent_pdev->bridge_d3;
#endif
		amdgpu_atpx_get_quirks(pdev);
	}

	while ((pdev = pci_get_class(PCI_CLASS_DISPLAY_OTHER << 8, pdev)) != NULL) {
		vga_count++;

		has_atpx |= amdgpu_atpx_pci_probe_handle(pdev);

#ifdef AMDKCL_PCIE_BRIDGE_PM_USABLE
		parent_pdev = pci_upstream_bridge(pdev);
		d3_supported |= parent_pdev && parent_pdev->bridge_d3;
#endif
		amdgpu_atpx_get_quirks(pdev);
	}

	if (has_atpx && vga_count == 2) {
		acpi_get_name(amdgpu_atpx_priv.atpx.handle, ACPI_FULL_PATHNAME, &buffer);
		pr_info("vga_switcheroo: detected switching method %s handle\n",
			acpi_method_name);
		amdgpu_atpx_priv.atpx_detected = true;
#ifdef AMDKCL_PCIE_BRIDGE_PM_USABLE
		amdgpu_atpx_priv.bridge_pm_usable = d3_supported;
#endif
		amdgpu_atpx_init();
		return true;
	}
	return false;
}

/**
 * amdgpu_register_atpx_handler - register with vga_switcheroo
 *
 * Register the PX callbacks with vga_switcheroo (all asics).
 */
void amdgpu_register_atpx_handler(void)
{
	bool r;
	enum vga_switcheroo_handler_flags_t handler_flags = 0;

	/* detect if we have any ATPX + 2 VGA in the system */
	r = amdgpu_atpx_detect();
	if (!r)
		return;

	vga_switcheroo_register_handler(&amdgpu_atpx_handler, handler_flags);
}

/**
 * amdgpu_unregister_atpx_handler - unregister with vga_switcheroo
 *
 * Unregister the PX callbacks with vga_switcheroo (all asics).
 */
void amdgpu_unregister_atpx_handler(void)
{
	vga_switcheroo_unregister_handler();
}
