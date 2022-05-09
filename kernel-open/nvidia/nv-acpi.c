/*
 * SPDX-FileCopyrightText: Copyright (c) 1999-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#define  __NO_VERSION__

#include "os-interface.h"
#include "nv-linux.h"
#include "nv-reg.h"

#include <linux/acpi.h>

#if defined(NV_LINUX_ACPI_EVENTS_SUPPORTED)
static NV_STATUS   nv_acpi_extract_integer (const union acpi_object *, void *, NvU32, NvU32 *);
static NV_STATUS   nv_acpi_extract_buffer  (const union acpi_object *, void *, NvU32, NvU32 *);
static NV_STATUS   nv_acpi_extract_package (const union acpi_object *, void *, NvU32, NvU32 *);
static NV_STATUS   nv_acpi_extract_object  (const union acpi_object *, void *, NvU32, NvU32 *);

static int         nv_acpi_add             (struct acpi_device *);
static int         nv_acpi_remove          (struct acpi_device *device);
static void        nv_acpi_event           (acpi_handle, u32, void *);
static void        nv_acpi_powersource_hotplug_event(acpi_handle, u32, void *);
static acpi_status nv_acpi_find_methods    (acpi_handle, u32, void *, void **);
static NV_STATUS   nv_acpi_nvif_method     (NvU32, NvU32, void *, NvU16, NvU32 *, void *, NvU16 *);

static NV_STATUS   nv_acpi_wmmx_method     (NvU32, NvU8 *, NvU16 *);

static const struct acpi_device_id nv_video_device_ids[] = {
    {
        .id          = ACPI_VIDEO_HID,
        .driver_data = 0,
    },
    {
        .id          = "",
        .driver_data = 0,
    },
};

static struct acpi_driver *nv_acpi_driver;
static acpi_handle nvif_handle = NULL;
static acpi_handle nvif_parent_gpu_handle  = NULL;
static acpi_handle wmmx_handle = NULL;

// Used for AC Power Source Hotplug Handling
static acpi_handle psr_handle = NULL;
static acpi_handle psr_device_handle = NULL;
static nv_acpi_t  *psr_nv_acpi_object = NULL;

static NvBool battery_present = NV_FALSE;

#define BIX_BATTERY_TECHNOLOGY_OFFSET 0x4
#define BIF_BATTERY_TECHNOLOGY_OFFSET 0x3
#define BATTERY_RECHARGABLE           0x1

/* Moved into acpi/video.h in Linux 4.10 */
#ifndef ACPI_VIDEO_NOTIFY_PROBE
#define ACPI_VIDEO_NOTIFY_PROBE    0x81
#endif

/* Added to acpi/video.h in Linux 3.1 */
#ifndef ACPI_VIDEO_CLASS
#define ACPI_VIDEO_CLASS    "video"
#endif

static const struct acpi_driver nv_acpi_driver_template = {
    .name = "NVIDIA ACPI Video Driver",
    .class = ACPI_VIDEO_CLASS,
    .ids = nv_video_device_ids,
    .ops = {
        .add = nv_acpi_add,
        .remove = nv_acpi_remove,
    },
};

static int nv_acpi_get_device_handle(nv_state_t *nv, acpi_handle *dev_handle)
{
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);

#if defined(DEVICE_ACPI_HANDLE)
    *dev_handle = DEVICE_ACPI_HANDLE(nvl->dev);
    return NV_TRUE;
#elif defined (ACPI_HANDLE)
    *dev_handle = ACPI_HANDLE(nvl->dev);
    return NV_TRUE;
#else
    return NV_FALSE;
#endif
}

static int nv_acpi_notify(struct notifier_block *nb,
                        unsigned long val, void *data)
{
    struct acpi_bus_event *info = data;
    nv_stack_t *sp = NULL;
    nv_linux_state_t *nvl = container_of(nb, nv_linux_state_t, acpi_nb);
    nv_state_t *nv = NV_STATE_PTR(nvl);

    if (!strcmp(info->device_class, ACPI_VIDEO_CLASS)) {
        if (nv_kmem_cache_alloc_stack(&sp) == 0) {
            /*
             * Function to handle device specific ACPI events
             * such as display hotplug and D-notifier events.
             */
            rm_acpi_notify(sp, nv, info->type);
            nv_kmem_cache_free_stack(sp);
        }
        else
            nv_printf(NV_DBG_ERRORS,
                "NVRM: nv_acpi_notify: failed to allocate stack\n");

        /*
         * Special case for ACPI_VIDEO_NOTIFY_PROBE event: intentionally return
         * NOTIFY_BAD to inform acpi-video to stop generating keypresses for
         * this event.
         */
        if (info->type == ACPI_VIDEO_NOTIFY_PROBE) {
            return NOTIFY_BAD;
        }
    }

    return NOTIFY_DONE;
}

void nv_acpi_register_notifier(nv_linux_state_t *nvl)
{
    nvl->acpi_nb.notifier_call = nv_acpi_notify;
    register_acpi_notifier(&nvl->acpi_nb);
}

void nv_acpi_unregister_notifier(nv_linux_state_t *nvl)
{
    unregister_acpi_notifier(&nvl->acpi_nb);
}

int nv_acpi_init(void)
{
    /*
     * This function will register the RM with the Linux
     * ACPI subsystem.
     */
    int status;
    nvidia_stack_t *sp = NULL;
    NvU32 acpi_event_config = 0;
    NV_STATUS rmStatus;

    status = nv_kmem_cache_alloc_stack(&sp);
    if (status != 0)
    {
        return status;
    }

    rmStatus = rm_read_registry_dword(sp, NULL,
                   NV_REG_REGISTER_FOR_ACPI_EVENTS, &acpi_event_config);
    nv_kmem_cache_free_stack(sp);

    if ((rmStatus == NV_OK) && (acpi_event_config == 0))
        return 0;

    if (nv_acpi_driver != NULL)
        return -EBUSY;

    rmStatus = os_alloc_mem((void **)&nv_acpi_driver,
            sizeof(struct acpi_driver));
    if (rmStatus != NV_OK)
        return -ENOMEM;

    memcpy((void *)nv_acpi_driver, (void *)&nv_acpi_driver_template,
            sizeof(struct acpi_driver));

    status = acpi_bus_register_driver(nv_acpi_driver);
    if (status < 0)
    {
        nv_printf(NV_DBG_INFO,
            "NVRM: nv_acpi_init: acpi_bus_register_driver() failed (%d)!\n", status);
        os_free_mem(nv_acpi_driver);
        nv_acpi_driver = NULL;
    }

    return status;
}

int nv_acpi_uninit(void)
{
    nvidia_stack_t *sp = NULL;
    NvU32 acpi_event_config = 0;
    NV_STATUS rmStatus;
    int rc;

    rc = nv_kmem_cache_alloc_stack(&sp);
    if (rc != 0)
    {
        return rc;
    }

    rmStatus = rm_read_registry_dword(sp, NULL,
                   NV_REG_REGISTER_FOR_ACPI_EVENTS, &acpi_event_config);
    nv_kmem_cache_free_stack(sp);

    if ((rmStatus == NV_OK) && (acpi_event_config == 0))
        return 0;

    if (nv_acpi_driver == NULL)
        return -ENXIO;

    acpi_bus_unregister_driver(nv_acpi_driver);
    os_free_mem(nv_acpi_driver);

    nv_acpi_driver = NULL;

    return 0;
}

static int nv_acpi_add(struct acpi_device *device)
{
    /*
     * This function will cause RM to initialize the things it needs for acpi interaction
     * on the display device.
     */
    int status = -1;
    NV_STATUS rmStatus = NV_ERR_GENERIC;
    nv_acpi_t *pNvAcpiObject = NULL;
    union acpi_object control_argument_0 = { ACPI_TYPE_INTEGER };
    struct acpi_object_list control_argument_list = { 0, NULL };
    nvidia_stack_t *sp = NULL;
    struct list_head *node, *next;
    unsigned long long device_id = 0;
    int device_counter = 0;

    status = nv_kmem_cache_alloc_stack(&sp);
    if (status != 0)
    {
        return status;
    }

    // allocate data structure we need
    rmStatus = os_alloc_mem((void **) &pNvAcpiObject, sizeof(nv_acpi_t));
    if (rmStatus != NV_OK)
    {
        nv_kmem_cache_free_stack(sp);
        nv_printf(NV_DBG_ERRORS,
            "NVRM: nv_acpi_add: failed to allocate ACPI device management data!\n");
        return -ENOMEM;
    }

    os_mem_set((void *)pNvAcpiObject, 0, sizeof(nv_acpi_t));

    device->driver_data = pNvAcpiObject;
    pNvAcpiObject->device = device;

    pNvAcpiObject->sp = sp;

    // grab handles to all the important nodes representing devices

    list_for_each_safe(node, next, &device->children)
    {
        struct acpi_device *dev =
            list_entry(node, struct acpi_device, node);

        if (!dev)
            continue;

        if (device_counter == NV_MAXNUM_DISPLAY_DEVICES)
        {
            nv_printf(NV_DBG_ERRORS,
                      "NVRM: nv_acpi_add: Total number of devices cannot exceed %d\n",
                      NV_MAXNUM_DISPLAY_DEVICES);
            break;
        }

        status =
            acpi_evaluate_integer(dev->handle, "_ADR", NULL, &device_id);
        if (ACPI_FAILURE(status))
            /* Couldnt query device_id for this device */
            continue;

        device_id = (device_id & 0xffff);

        if ((device_id != 0x100) && /* Not a known CRT device-id */
            (device_id != 0x200) && /* Not a known TV device-id */
            (device_id != 0x0110) && (device_id != 0x0118) && (device_id != 0x0400) && /* Not an LCD*/
            (device_id != 0x0111) && (device_id != 0x0120) && (device_id != 0x0300)) /* Not a known DVI device-id */
        {
            /* This isnt a known device Id.
               Do default switching on this system. */
            pNvAcpiObject->default_display_mask = 1;
            break;
        }

        pNvAcpiObject->pNvVideo[device_counter].dev_id = device_id;
        pNvAcpiObject->pNvVideo[device_counter].dev_handle = dev->handle;

        device_counter++;

    }

    // arg 0, bits 1:0, 0 = enable events
    control_argument_0.integer.type = ACPI_TYPE_INTEGER;
    control_argument_0.integer.value = 0x0;

    // listify it
    control_argument_list.count = 1;
    control_argument_list.pointer = &control_argument_0;

    // _DOS method takes 1 argument and returns nothing
    status = acpi_evaluate_object(device->handle, "_DOS", &control_argument_list, NULL);

    if (ACPI_FAILURE(status))
    {
        nv_printf(NV_DBG_INFO,
            "NVRM: nv_acpi_add: failed to enable display switch events (%d)!\n", status);
    }

    status = acpi_install_notify_handler(device->handle, ACPI_DEVICE_NOTIFY,
                    nv_acpi_event, pNvAcpiObject);

    if (ACPI_FAILURE(status))
    {
        nv_printf(NV_DBG_INFO,
            "NVRM: nv_acpi_add: failed to install event notification handler (%d)!\n", status);
    }
    else
    {
        try_module_get(THIS_MODULE);
        pNvAcpiObject->notify_handler_installed = 1;
    }

    return 0;
}

static int nv_acpi_remove(struct acpi_device *device)
{
    /*
     * This function will cause RM to relinquish control of the VGA ACPI device.
     */
    acpi_status status;
    union acpi_object control_argument_0 = { ACPI_TYPE_INTEGER };
    struct acpi_object_list control_argument_list = { 0, NULL };
    nv_acpi_t *pNvAcpiObject = device->driver_data;


    pNvAcpiObject->default_display_mask = 0;

    // arg 0, bits 1:0, 1 = disable events
    control_argument_0.integer.type = ACPI_TYPE_INTEGER;
    control_argument_0.integer.value = 0x1;

    // listify it
    control_argument_list.count = 1;
    control_argument_list.pointer = &control_argument_0;

    // _DOS method takes 1 argument and returns nothing
    status = acpi_evaluate_object(device->handle, "_DOS", &control_argument_list, NULL);

    if (ACPI_FAILURE(status))
    {
        nv_printf(NV_DBG_INFO,
            "NVRM: nv_acpi_remove: failed to disable display switch events (%d)!\n", status);
    }

    if (pNvAcpiObject->notify_handler_installed)
    {
        // remove event notifier
        status = acpi_remove_notify_handler(device->handle, ACPI_DEVICE_NOTIFY, nv_acpi_event);
    }

    if (pNvAcpiObject->notify_handler_installed &&
        ACPI_FAILURE(status))
    {
        nv_printf(NV_DBG_INFO,
            "NVRM: nv_acpi_remove: failed to remove event notification handler (%d)!\n", status);
    }
    else
    {
        nv_kmem_cache_free_stack(pNvAcpiObject->sp);
        os_free_mem((void *)pNvAcpiObject);
        module_put(THIS_MODULE);
        device->driver_data = NULL;
    }

    return status;
}

/*
 * The ACPI specification defines IDs for various ACPI video
 * extension events like display switch events, AC/battery
 * events, docking events, etc..
 * Whenever an ACPI event is received by the corresponding
 * event handler installed within the core NVIDIA driver, the
 * code can verify the event ID before processing it.
 */
#define ACPI_DISPLAY_DEVICE_CHANGE_EVENT     0x80
#define NVIF_NOTIFY_DISPLAY_DETECT           0xCB
#define NVIF_DISPLAY_DEVICE_CHANGE_EVENT     NVIF_NOTIFY_DISPLAY_DETECT
static void nv_acpi_event(acpi_handle handle, u32 event_type, void *data)
{
    /*
     * This function will handle acpi events from the linux kernel, used
     * to detect notifications from the VGA device.
     */
    nv_acpi_t *pNvAcpiObject = data;
    u32 event_val = 0;
    unsigned long long state;
    int status = 0;
    int device_counter = 0;

    if (event_type == NVIF_DISPLAY_DEVICE_CHANGE_EVENT)
    {
        /* We are getting NVIF events on this machine. We arent putting a very
           extensive handling in-place to communicate back with SBIOS, know
           the next enabled devices, and then do the switch. We just
           pass a default display switch event, so that X-driver decides
           the switching policy itself. */
        rm_system_event(pNvAcpiObject->sp, NV_SYSTEM_ACPI_DISPLAY_SWITCH_EVENT, 0);
    }
    if (event_type == ACPI_DISPLAY_DEVICE_CHANGE_EVENT)
    {
        if (pNvAcpiObject->default_display_mask != 1)
        {
            while ((device_counter < NV_MAXNUM_DISPLAY_DEVICES) &&
                   (pNvAcpiObject->pNvVideo[device_counter].dev_handle))
            {
                acpi_handle dev_handle = pNvAcpiObject->pNvVideo[device_counter].dev_handle;
                int dev_id = pNvAcpiObject->pNvVideo[device_counter].dev_id;

                status = acpi_evaluate_integer(dev_handle,
                                               "_DGS",
                                               NULL,
                                               &state);
                if (ACPI_FAILURE(status))
                {
                    nv_printf(NV_DBG_INFO,
                    "NVRM: nv_acpi_event: failed to query _DGS method for display device 0x%x\n",
                    dev_id);
                }
                else if (state)
                {
                    /* Check if the device is a CRT ...*/
                    if (dev_id == 0x0100)
                    {
                        event_val |= NV_HOTKEY_STATUS_DISPLAY_ENABLE_CRT;
                    }
                    /* device-id for a TV */
                    else if (dev_id == 0x0200)
                    {
                        event_val |= NV_HOTKEY_STATUS_DISPLAY_ENABLE_TV;
                    }
                    else if ((dev_id == 0x0110) ||  /* device id for internal LCD */
                             (dev_id == 0x0118) ||  /* alternate ACPI ID for the
                                                                        internal LCD */
                             (dev_id == 0x0400))    /* ACPI spec 3.0 specified
                                                 device id for a internal LCD*/
                    {
                        event_val |= NV_HOTKEY_STATUS_DISPLAY_ENABLE_LCD;
                    }
                    else if ((dev_id == 0x0111) || /* the set
                                                    of possible device-ids for a DFP */
                             (dev_id == 0x0120) ||
                             (dev_id == 0x0300))   /* ACPI spec 3.0 specified
                                                    device id for non-LVDS DFP */
                    {
                        event_val |= NV_HOTKEY_STATUS_DISPLAY_ENABLE_DFP;
                    }
                }
                device_counter++;
            }
        }

        nv_printf(NV_DBG_INFO,
        "NVRM: nv_acpi_event: Event-type 0x%x, Event-val 0x%x\n",
        event_type, event_val);

        rm_system_event(pNvAcpiObject->sp, NV_SYSTEM_ACPI_DISPLAY_SWITCH_EVENT, event_val);
    }

    // no unsubscription or re-enable necessary. Once DOD has been set, we are go.
    // once we are subscribed to ACPI events, we don't have to re-subscribe unless
    // unsubscribe.
}

NV_STATUS NV_API_CALL nv_acpi_get_powersource(NvU32 *ac_plugged)
{
    unsigned long long val;
    int status = 0;

    if (!ac_plugged)
        return NV_ERR_INVALID_ARGUMENT;

    if (!psr_device_handle)
        return NV_ERR_INVALID_ARGUMENT;

    // Check whether or not AC power is plugged in
    status = acpi_evaluate_integer(psr_device_handle, "_PSR", NULL, &val);
    if (ACPI_FAILURE(status))
        return NV_ERR_GENERIC;

    // AC Power Source Plug State
    //  - 0x0 unplugged
    //  - 0x1 plugged
    *ac_plugged = (val == 0x1);

    return NV_OK;
}

#define ACPI_POWER_SOURCE_CHANGE_EVENT       0x80
static void nv_acpi_powersource_hotplug_event(acpi_handle handle, u32 event_type, void *data)
{
    /*
     * This function will handle acpi events from the linux kernel, used
     * to detect notifications from Power Source device
     */
    nv_acpi_t *pNvAcpiObject = data;
    u32 ac_plugged = 0;

    if (event_type == ACPI_POWER_SOURCE_CHANGE_EVENT)
    {
        if (nv_acpi_get_powersource(&ac_plugged) != NV_OK)
            return;

        rm_system_event(pNvAcpiObject->sp, NV_SYSTEM_ACPI_BATTERY_POWER_EVENT, !ac_plugged);
    }
}
/*
 * End of ACPI event handler functions
 */

/* Do the necessary allocations and install notifier "handler" on the device-node "device" */
static nv_acpi_t* nv_install_notifier(struct acpi_device *device, acpi_notify_handler handler)
{
    nvidia_stack_t *sp = NULL;
    nv_acpi_t *pNvAcpiObject = NULL;
    NV_STATUS rmStatus = NV_ERR_GENERIC;
    acpi_status status = -1;

    if (!device)
        return NULL;

    if (nv_kmem_cache_alloc_stack(&sp) != 0)
    {
        return NULL;
    }

    rmStatus = os_alloc_mem((void **) &pNvAcpiObject, sizeof(nv_acpi_t));
    if (rmStatus != NV_OK)
        goto return_error;

    os_mem_set((void *)pNvAcpiObject, 0, sizeof(nv_acpi_t));

    // store a device reference in our object
    pNvAcpiObject->device = device;
    pNvAcpiObject->sp = sp;

    status = acpi_install_notify_handler(device->handle, ACPI_DEVICE_NOTIFY,
              handler, pNvAcpiObject);
    if (!ACPI_FAILURE(status))
    {
        pNvAcpiObject->notify_handler_installed = 1;

        return pNvAcpiObject;
    }

return_error:
     nv_kmem_cache_free_stack(sp);
     if (pNvAcpiObject)
         os_free_mem((void *)pNvAcpiObject);

     return NULL;
}

/* Tear-down and remove whatever nv_install_notifier did */
static void nv_uninstall_notifier(nv_acpi_t *pNvAcpiObject, acpi_notify_handler handler)
{
    acpi_status status;

    if (pNvAcpiObject && pNvAcpiObject->notify_handler_installed)
    {
        status = acpi_remove_notify_handler(pNvAcpiObject->device->handle, ACPI_DEVICE_NOTIFY, handler);
        if (ACPI_FAILURE(status))
        {
            nv_printf(NV_DBG_INFO,
                "NVRM: nv_acpi_methods_uninit: failed to remove event notification handler (%d)!\n", status);
        }
        else
        {
            nv_kmem_cache_free_stack(pNvAcpiObject->sp);
            os_free_mem((void *)pNvAcpiObject);
        }
    }

    return;
}

/*
 * acpi methods init function.
 * check if the NVIF, _DSM and WMMX methods are present in the acpi namespace.
 * store NVIF, _DSM and WMMX handle if found.
 */

void NV_API_CALL nv_acpi_methods_init(NvU32 *handlesPresent)
{
#if defined(NV_ACPI_BUS_GET_DEVICE_PRESENT)
    struct acpi_device *device = NULL;
    int retVal = -1;
#endif


    if (!handlesPresent) // Caller passed us invalid pointer.
        return;


    *handlesPresent = 0;

    NV_ACPI_WALK_NAMESPACE(ACPI_TYPE_DEVICE, ACPI_ROOT_OBJECT,
                        ACPI_UINT32_MAX, nv_acpi_find_methods, NULL, NULL);

#if defined(NV_ACPI_BUS_GET_DEVICE_PRESENT)
    if (nvif_handle)
    {
        *handlesPresent = NV_ACPI_NVIF_HANDLE_PRESENT;
        do
        {
            if (!nvif_parent_gpu_handle) /* unknown error */
                break;

            retVal = acpi_bus_get_device(nvif_parent_gpu_handle, &device);

            if (ACPI_FAILURE(retVal) || !device)
                break;

            if (device->driver_data)
            {
                nvif_parent_gpu_handle = NULL;
                break;  /* Someone else has already populated this device
                           nodes' structures. So nothing more to be done */
            }

            device->driver_data  = nv_install_notifier(device, nv_acpi_event);


            if (!device->driver_data)
                nvif_parent_gpu_handle = NULL;

        } while (0);
    }
#endif

    if (wmmx_handle)
        *handlesPresent = *handlesPresent | NV_ACPI_WMMX_HANDLE_PRESENT;

#if defined(NV_ACPI_BUS_GET_DEVICE_PRESENT)
    if (psr_handle)
    {
        // Since _PSR is not a per-GPU construct we only need to register a
        // single notifier for the _PSR event. Skip registration for subsequent
        // devices
        if (psr_nv_acpi_object == NULL)
        {
            retVal = acpi_bus_get_device(psr_device_handle, &device);

            if (!(ACPI_FAILURE(retVal) || !device))
            {
                psr_nv_acpi_object = nv_install_notifier(device, nv_acpi_powersource_hotplug_event);
            }
        }
    }
#endif

    return;
}

acpi_status nv_acpi_find_methods(
    acpi_handle handle,
    u32 nest_level,
    void *dummy1,
    void **dummy2
)
{
    acpi_handle method_handle;

    if (!acpi_get_handle(handle, "NVIF", &method_handle))
    {
        nvif_handle = method_handle;
        nvif_parent_gpu_handle = handle;
    }

    if (!acpi_get_handle(handle, "WMMX", &method_handle))
    {
        wmmx_handle = method_handle;
    }

    if (!acpi_get_handle(handle, "_PSR", &method_handle))
    {
        psr_handle = method_handle;
        psr_device_handle = handle;
    }

    return 0;
}

void NV_API_CALL nv_acpi_methods_uninit(void)
{
    struct acpi_device *device = NULL;

    nvif_handle = NULL;
    wmmx_handle = NULL;

    if (psr_nv_acpi_object != NULL)
    {
        nv_uninstall_notifier(psr_nv_acpi_object, nv_acpi_powersource_hotplug_event);

        psr_handle = NULL;
        psr_device_handle = NULL;
        psr_nv_acpi_object = NULL;
    }

    if (nvif_parent_gpu_handle == NULL)
        return;

#if defined(NV_ACPI_BUS_GET_DEVICE_PRESENT)
    acpi_bus_get_device(nvif_parent_gpu_handle, &device);

    nv_uninstall_notifier(device->driver_data, nv_acpi_event);
#endif

    device->driver_data = NULL;
    nvif_parent_gpu_handle = NULL;

    return;
}

static NV_STATUS nv_acpi_extract_integer(
    const union acpi_object *acpi_object,
    void  *buffer,
    NvU32  buffer_size,
    NvU32 *data_size
)
{
    if (acpi_object->type != ACPI_TYPE_INTEGER)
        return NV_ERR_INVALID_ARGUMENT;

    if (acpi_object->integer.value & ~0xffffffffULL)
        *data_size = sizeof(acpi_object->integer.value);
    else
        *data_size = sizeof(NvU32);

    if ((buffer_size < sizeof(NvU32)) ||
        ((buffer_size < sizeof(acpi_object->integer.value)) &&
         (acpi_object->integer.value & ~0xffffffffULL)))
    {
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    memcpy(buffer, &acpi_object->integer.value, *data_size);

    return NV_OK;
}

static NV_STATUS nv_acpi_extract_buffer(
    const union acpi_object *acpi_object,
    void  *buffer,
    NvU32  buffer_size,
    NvU32 *data_size
)
{
    if (acpi_object->type != ACPI_TYPE_BUFFER)
        return NV_ERR_INVALID_ARGUMENT;

    *data_size = acpi_object->buffer.length;

    if (buffer_size < acpi_object->buffer.length)
        return NV_ERR_BUFFER_TOO_SMALL;

    memcpy(buffer, acpi_object->buffer.pointer, *data_size);

    return NV_OK;
}

static NV_STATUS nv_acpi_extract_package(
    const union acpi_object *acpi_object,
    void  *buffer,
    NvU32  buffer_size,
    NvU32 *data_size
)
{
    NV_STATUS status = NV_OK;
    NvU32 i, element_size = 0;

    if (acpi_object->type != ACPI_TYPE_PACKAGE)
        return NV_ERR_INVALID_ARGUMENT;

    *data_size = 0;
    for (i = 0; i < acpi_object->package.count; i++)
    {
        buffer = ((char *)buffer + element_size);
        buffer_size -= element_size;

        status = nv_acpi_extract_object(&acpi_object->package.elements[i],
                                        buffer, buffer_size, &element_size);
        if (status != NV_OK)
            break;

        *data_size += element_size;
    }

    return status;
}

static NV_STATUS nv_acpi_extract_object(
    const union acpi_object *acpi_object,
    void  *buffer,
    NvU32  buffer_size,
    NvU32 *data_size
)
{
    NV_STATUS status;

    switch (acpi_object->type)
    {
        case ACPI_TYPE_INTEGER:
            status = nv_acpi_extract_integer(acpi_object, buffer,
                    buffer_size, data_size);
            break;

        case ACPI_TYPE_BUFFER:
            status = nv_acpi_extract_buffer(acpi_object, buffer,
                    buffer_size, data_size);
            break;

        case ACPI_TYPE_PACKAGE:
            status = nv_acpi_extract_package(acpi_object, buffer,
                    buffer_size, data_size);
            break;

        case ACPI_TYPE_ANY:
            /*
             * ACPI_TYPE_ANY is used to represent a NULL/Uninitialized object which is objectType 0
             * in the ACPI SPEC. This should not be treated as error.
             */
            status = NV_OK;
            break;

        default:
            status = NV_ERR_NOT_SUPPORTED;
    }

    return status;
}

NV_STATUS NV_API_CALL nv_acpi_method(
    NvU32 acpi_method,
    NvU32 function,
    NvU32 subFunction,
    void  *inParams,
    NvU16 inParamSize,
    NvU32 *outStatus,
    void  *outData,
    NvU16 *outDataSize
)
{
    NV_STATUS status;

    switch (acpi_method)
    {
        case NV_EVAL_ACPI_METHOD_NVIF:
            status = nv_acpi_nvif_method(function,
                                         subFunction,
                                         inParams,
                                         inParamSize,
                                         outStatus,
                                         outData,
                                         outDataSize);
            break;

        case NV_EVAL_ACPI_METHOD_WMMX:
            status = nv_acpi_wmmx_method(function,  outData, outDataSize);
            break;

        default:
            status = NV_ERR_NOT_SUPPORTED;
    }

    return status;
}

/*
 * This function executes an NVIF ACPI method.
 */
static NV_STATUS nv_acpi_nvif_method(
    NvU32 function,
    NvU32 subFunction,
    void  *inParams,
    NvU16 inParamSize,
    NvU32 *outStatus,
    void  *outData,
    NvU16 *outDataSize
)
{
    acpi_status status;
    struct acpi_object_list input;
    struct acpi_buffer output = { ACPI_ALLOCATE_BUFFER, NULL };
    union acpi_object *nvif = NULL;
    union acpi_object nvif_params[3];
    NvU16 localOutDataSize;
    NvU8  localInParams[8];

    if (!nvif_handle)
        return NV_ERR_NOT_SUPPORTED;

    if (!NV_MAY_SLEEP())
    {
#if defined(DEBUG)
        nv_printf(NV_DBG_ERRORS,
            "NVRM: nv_acpi_nvif_method: invalid context!\n");
#endif
        return NV_ERR_NOT_SUPPORTED;
    }

    nvif_params[0].integer.type  =  ACPI_TYPE_INTEGER;
    nvif_params[0].integer.value =  function;

    nvif_params[1].integer.type  =  ACPI_TYPE_INTEGER;
    nvif_params[1].integer.value =  subFunction;

    nvif_params[2].buffer.type  =  ACPI_TYPE_BUFFER;

    if (inParams && (inParamSize > 0))
    {
        nvif_params[2].buffer.length  = inParamSize;
        nvif_params[2].buffer.pointer = inParams;
    }
    else
    {
        memset(localInParams, 0, 8);
        nvif_params[2].buffer.length  = 8;
        nvif_params[2].buffer.pointer = localInParams;
    }

    input.count = 3;
    input.pointer = nvif_params;

    status = acpi_evaluate_object(nvif_handle, NULL, &input, &output);
    if (ACPI_FAILURE(status))
    {
        nv_printf(NV_DBG_INFO,
            "NVRM: nv_acpi_nvif_method: failed to get NVIF data, "
            "status 0x%x, function 0x%x, subFunction 0x%x!\n",
            status, function, subFunction);
        return NV_ERR_GENERIC;
    }

    nvif = output.pointer;
    if (nvif && (nvif->type == ACPI_TYPE_BUFFER) && (nvif->buffer.length >= 4))
    {
        if (outStatus)
        {
            *outStatus = nvif->buffer.pointer[3] << 24 |
                         nvif->buffer.pointer[2] << 16 |
                         nvif->buffer.pointer[1] << 8  |
                         nvif->buffer.pointer[0];
        }

        if (outData && outDataSize)
        {
            localOutDataSize = nvif->buffer.length - 4;
            if (localOutDataSize <= *outDataSize)
            {
                *outDataSize = NV_MIN(*outDataSize, localOutDataSize);
                memcpy(outData, &nvif->buffer.pointer[4], *outDataSize);
            }
            else
            {
                *outDataSize = localOutDataSize;
                kfree(output.pointer);
                return NV_ERR_BUFFER_TOO_SMALL;
            }
        }
    }
    else
    {
        nv_printf(NV_DBG_INFO,
            "NVRM: nv_acpi_nvif_method: NVIF data invalid, function 0x%x, "
            "subFunction 0x%x!\n", function, subFunction);
        kfree(output.pointer);
        return NV_ERR_GENERIC;
    }

    kfree(output.pointer);
    return NV_OK;
}

#define MAX_INPUT_PARAM_SIZE     1024
/*
 * This function executes a _DSM ACPI method.
 */
NV_STATUS NV_API_CALL nv_acpi_dsm_method(
    nv_state_t *nv,
    NvU8  *pAcpiDsmGuid,
    NvU32 acpiDsmRev,
    NvBool acpiNvpcfDsmFunction,
    NvU32 acpiDsmSubFunction,
    void  *pInParams,
    NvU16 inParamSize,
    NvU32 *outStatus,
    void  *pOutData,
    NvU16 *pSize
)
{
    NV_STATUS status = NV_ERR_OPERATING_SYSTEM;
    acpi_status acpi_status;
    struct acpi_object_list input;
    union acpi_object *dsm = NULL;
    struct acpi_buffer output = { ACPI_ALLOCATE_BUFFER, NULL };
    union acpi_object dsm_params[4];
    NvU8 *argument3 = NULL;
    NvU32 data_size;
    acpi_handle dev_handle  = NULL;

    if (!nv_acpi_get_device_handle(nv, &dev_handle))
        return NV_ERR_NOT_SUPPORTED;

    if (!dev_handle)
        return NV_ERR_INVALID_ARGUMENT;

    if ((!pInParams) || (inParamSize > MAX_INPUT_PARAM_SIZE) || (!pOutData) || (!pSize))
    {
        nv_printf(NV_DBG_INFO,
                  "NVRM: %s: invalid argument(s)!\n", __FUNCTION__);
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (!NV_MAY_SLEEP())
    {
#if defined(DEBUG)
        nv_printf(NV_DBG_INFO,
                  "NVRM: %s: invalid argument(s)!\n", __FUNCTION__);
#endif
        return NV_ERR_NOT_SUPPORTED;
    }

    status = os_alloc_mem((void **)&argument3, inParamSize);
    if (status != NV_OK)
        return status;

    //
    // dsm_params[0].buffer.pointer and dsm_params[1].integer.value set in
    // switch below based on acpiDsmFunction
    //

    dsm_params[0].buffer.type    = ACPI_TYPE_BUFFER;
    dsm_params[0].buffer.length  = 0x10;
    dsm_params[0].buffer.pointer = pAcpiDsmGuid;

    dsm_params[1].integer.type   = ACPI_TYPE_INTEGER;
    dsm_params[1].integer.value  = acpiDsmRev;

    dsm_params[2].integer.type   = ACPI_TYPE_INTEGER;
    dsm_params[2].integer.value  = acpiDsmSubFunction;

    dsm_params[3].buffer.type    = ACPI_TYPE_BUFFER;
    dsm_params[3].buffer.length  = inParamSize;
    memcpy(argument3, pInParams, dsm_params[3].buffer.length);
    dsm_params[3].buffer.pointer = argument3;

    // parameters for dsm calls (GUID, rev, subfunction, data)
    input.count = 4;
    input.pointer = dsm_params;

    if (acpiNvpcfDsmFunction)
    {
        //
        // acpi_evaluate_object() can operate with either valid object pathname or
        // valid object handle. For NVPCF DSM function, use valid pathname as we do
        // not have device handle for NVPCF device
        //
        dev_handle = NULL;
        acpi_status = acpi_evaluate_object(dev_handle, "\\_SB.NPCF._DSM", &input, &output);
    }
    else
    {
        acpi_status = acpi_evaluate_object(dev_handle, "_DSM", &input, &output);
    }

    if (ACPI_FAILURE(acpi_status))
    {
        nv_printf(NV_DBG_INFO,
              "NVRM: %s: failed to evaluate _DSM method!\n", __FUNCTION__);
        goto exit;
    }

    dsm = output.pointer;
    if (dsm != NULL)
    {
        if (outStatus)
        {
            *outStatus = dsm->buffer.pointer[3] << 24 |
                         dsm->buffer.pointer[2] << 16 |
                         dsm->buffer.pointer[1] << 8  |
                         dsm->buffer.pointer[0];
        }

        status = nv_acpi_extract_object(dsm, pOutData, *pSize, &data_size);
        *pSize = data_size;

        kfree(output.pointer);
    }
    if (status != NV_OK)
    {
        nv_printf(NV_DBG_ERRORS,
                  "NVRM: %s: DSM data invalid!\n", __FUNCTION__);
    }

exit:
    os_free_mem(argument3);
    return status;
}

/*
 * This function executes a _DDC ACPI method.
 */
NV_STATUS NV_API_CALL nv_acpi_ddc_method(
    nv_state_t *nv,
    void *pEdidBuffer,
    NvU32 *pSize,
    NvBool bReadMultiBlock
)
{
    acpi_status status;
    struct acpi_device *device = NULL;
    union acpi_object *ddc = NULL;
    struct list_head *node, *next;
    NvU32 i, largestEdidSize;
    acpi_handle dev_handle  = NULL;
    acpi_handle lcd_dev_handle  = NULL;

    if (!nv_acpi_get_device_handle(nv, &dev_handle))
        return NV_ERR_NOT_SUPPORTED;

    if (!dev_handle)
        return NV_ERR_INVALID_ARGUMENT;

#if defined(NV_ACPI_BUS_GET_DEVICE_PRESENT)
    status = acpi_bus_get_device(dev_handle, &device);
#else
    return NV_ERR_NOT_SUPPORTED;
#endif

    if (ACPI_FAILURE(status) || !device)
        return NV_ERR_INVALID_ARGUMENT;

    if (!NV_MAY_SLEEP())
    {
#if defined(DEBUG)
        nv_printf(NV_DBG_ERRORS,
                  "NVRM: %s: invalid context!\n",
                  __FUNCTION__);
#endif
        return NV_ERR_NOT_SUPPORTED;
    }

    list_for_each_safe(node, next, &device->children)
    {
        unsigned long long device_id = 0;
        struct acpi_device *dev =
            list_entry(node, struct acpi_device, node);

        if (!dev)
            continue;

        status = acpi_evaluate_integer(dev->handle, "_ADR", NULL, &device_id);
        if (ACPI_FAILURE(status))
            /* Couldnt query device_id for this device */
            continue;

        switch (device_id & 0xffff) {
            case 0x0110:
            case 0x0118:
            case 0x0400:
            case 0xA420:
                lcd_dev_handle = dev->handle;
                nv_printf(NV_DBG_INFO, "NVRM: %s Found LCD: %x\n",
                          __FUNCTION__, device_id);
                break;
            default:
                break;
        }

        if (lcd_dev_handle != NULL)
            break;
    }

    if (lcd_dev_handle == NULL)
    {
        nv_printf(NV_DBG_INFO, "NVRM: %s LCD not found\n", __FUNCTION__);
        return NV_ERR_GENERIC;
    }

    //
    // As per ACPI Spec 3.0:
    // ARG0 = 0x1 for 128 bytes edid buffer
    // ARG0 = 0x2 for 256 bytes edid buffer
    //

    largestEdidSize = bReadMultiBlock ? 2 : 1;

    for (i = largestEdidSize; i >= 1; i--)
    {
        struct acpi_buffer output = { ACPI_ALLOCATE_BUFFER, NULL };
        union acpi_object ddc_arg0 = { ACPI_TYPE_INTEGER };
        struct acpi_object_list input = { 1, &ddc_arg0 };

        ddc_arg0.integer.value = i;
        status = acpi_evaluate_object(lcd_dev_handle, "_DDC", &input, &output);
        if (ACPI_SUCCESS(status)) {
            ddc = output.pointer;
            break;
        }
    }

    if (ACPI_FAILURE(status))
    {
        nv_printf(NV_DBG_INFO,
                  "NVRM: %s: failed status: %08x \n",
                  __FUNCTION__,
                  status);
        return NV_ERR_GENERIC;
    }
    else
    {
        if (ddc && (ddc->type == ACPI_TYPE_BUFFER) && (ddc->buffer.length > 0))
        {
            if (ddc->buffer.length <= *pSize)
            {
                *pSize = NV_MIN(*pSize, ddc->buffer.length);
                memcpy(pEdidBuffer, ddc->buffer.pointer, *pSize);
            }
            else
            {
                kfree(ddc);
                return NV_ERR_BUFFER_TOO_SMALL;
            }
        }
    }

    kfree(ddc);
    return NV_OK;
}

/*
 * This function executes a _ROM ACPI method.
 */
NV_STATUS NV_API_CALL nv_acpi_rom_method(
    nv_state_t *nv,
    NvU32 *pInData,
    NvU32 *pOutData
)
{
    acpi_status status;
    struct acpi_buffer output = { ACPI_ALLOCATE_BUFFER, NULL };
    union acpi_object *rom;
    union acpi_object rom_arg[2];
    struct acpi_object_list input = { 2, rom_arg };
    acpi_handle dev_handle  = NULL;
    uint32_t offset, length;

    if (!nv_acpi_get_device_handle(nv, &dev_handle))
        return NV_ERR_NOT_SUPPORTED;

    if (!dev_handle)
        return NV_ERR_INVALID_ARGUMENT;

    if (!NV_MAY_SLEEP())
    {
#if defined(DEBUG)
        nv_printf(NV_DBG_ERRORS,
                  "NVRM: %s: invalid context!\n", __FUNCTION__);
#endif
        return NV_ERR_NOT_SUPPORTED;
    }

    offset = pInData[0];
    length = pInData[1];

    rom_arg[0].type = ACPI_TYPE_INTEGER;
    rom_arg[0].integer.value = offset;
    rom_arg[1].type = ACPI_TYPE_INTEGER;
    rom_arg[1].integer.value = length;

    status = acpi_evaluate_object(dev_handle, "_ROM", &input, &output);
    if (ACPI_FAILURE(status))
    {
        nv_printf(NV_DBG_INFO,
              "NVRM: %s: failed to evaluate _ROM method!\n", __FUNCTION__);
        return NV_ERR_GENERIC;
    }
    else
    {
        rom = output.pointer;

        if ((rom != NULL) && (rom->type == ACPI_TYPE_BUFFER) &&
            (rom->buffer.length >= length))
        {
            memcpy(pOutData, rom->buffer.pointer, length);
        }
        else
        {
            nv_printf(NV_DBG_INFO,
                  "NVRM: %s: Invalid _ROM data\n", __FUNCTION__);
            kfree(output.pointer);
            return NV_ERR_GENERIC;
        }
    }

    kfree(output.pointer);
    return NV_OK;
}

/*
 * This function executes a _DOD ACPI method.
 */
NV_STATUS NV_API_CALL nv_acpi_dod_method(
    nv_state_t *nv,
    NvU32      *pOutData,
    NvU32      *pSize
)
{
    acpi_status status;
    struct acpi_buffer output = { ACPI_ALLOCATE_BUFFER, NULL };
    union acpi_object *dod;
    acpi_handle dev_handle = NULL;
    NvU32 i, count = (*pSize / sizeof(NvU32));

    if (!nv_acpi_get_device_handle(nv, &dev_handle))
        return NV_ERR_NOT_SUPPORTED;

    if (!dev_handle)
        return NV_ERR_INVALID_ARGUMENT;

    if (!NV_MAY_SLEEP())
    {
#if defined(DEBUG)
        nv_printf(NV_DBG_ERRORS,
              "NVRM: %s: invalid context!\n", __FUNCTION__);
#endif
        return NV_ERR_NOT_SUPPORTED;
    }

    status = acpi_evaluate_object(dev_handle, "_DOD", NULL, &output);

    if (ACPI_FAILURE(status))
    {
        nv_printf(NV_DBG_INFO,
              "NVRM: %s: failed to evaluate _DOD method!\n", __FUNCTION__);
        return NV_ERR_GENERIC;
    }
    else
    {
        dod = output.pointer;
        *pSize = 0;

        if ((dod != NULL) && (dod->type == ACPI_TYPE_PACKAGE) &&
            (dod->package.count <= count))
        {
            for (i = 0; i < dod->package.count; i++)
            {
                if (dod->package.elements[i].type != ACPI_TYPE_INTEGER)
                {
                    nv_printf(NV_DBG_INFO,
                            "NVRM: %s: _DOD entry invalid!\n", __FUNCTION__);
                    kfree(output.pointer);
                    return NV_ERR_GENERIC;
                }

                pOutData[i] = dod->package.elements[i].integer.value;
                *pSize += sizeof(NvU32);
            }
        }
        else
        {
            nv_printf(NV_DBG_INFO,
                      "NVRM: %s: _DOD data too large!\n", __FUNCTION__);
            kfree(output.pointer);
            return NV_ERR_GENERIC;
        }
    }

    kfree(output.pointer);
    return NV_OK;
}

/*
 * This function executes a WMMX ACPI method.
 */
static NV_STATUS  nv_acpi_wmmx_method(
    NvU32 arg2,
    NvU8  *outData,
    NvU16 *outDataSize
)
{
    acpi_status status;
    struct acpi_object_list input;
    struct acpi_buffer output = { ACPI_ALLOCATE_BUFFER, NULL };
    union acpi_object *mmx = NULL;
    union acpi_object mmx_params[3];

    if (!wmmx_handle)
        return NV_ERR_NOT_SUPPORTED;

    if (!NV_MAY_SLEEP())
    {
#if defined(DEBUG)
        nv_printf(NV_DBG_ERRORS,
            "NVRM: nv_acpi_wmmx_method: invalid context!\n");
#endif
        return NV_ERR_NOT_SUPPORTED;
    }

   /* argument 0 and argument 1 are not used in WMMX method, passing 0 */

    mmx_params[0].integer.type  =  ACPI_TYPE_INTEGER;
    mmx_params[0].integer.value =  0;

    mmx_params[1].integer.type  =  ACPI_TYPE_INTEGER;
    mmx_params[1].integer.value =  0;

    mmx_params[2].integer.type  =  ACPI_TYPE_INTEGER;
    mmx_params[2].integer.value =  arg2;

    input.count = 3;
    input.pointer = mmx_params;

    status = acpi_evaluate_object(wmmx_handle, NULL, &input, &output);
    if (ACPI_FAILURE(status))
    {
        nv_printf(NV_DBG_INFO,
            "NVRM: nv_acpi_wmmx_method: failed to get WMMX data, "
            "status 0x%x!\n", status);
        return NV_ERR_GENERIC;
    }

    mmx = output.pointer;
    if (mmx && (mmx->type == ACPI_TYPE_BUFFER) && (mmx->buffer.length > 0))
    {
        if (outData && outDataSize)
        {
            if (mmx->buffer.length <= *outDataSize)
            {
                *outDataSize = NV_MIN(*outDataSize, mmx->buffer.length);
                memcpy(outData, mmx->buffer.pointer, *outDataSize);
            }
            else
            {
                kfree(output.pointer);
                return NV_ERR_BUFFER_TOO_SMALL;
            }
        }
    }
    else
    {
        nv_printf(NV_DBG_ERRORS,
                "NVRM: nv_acpi_wmmx_method: WMMX data invalid.\n");
        kfree(output.pointer);
        return NV_ERR_GENERIC;
    }

    kfree(output.pointer);
    return NV_OK;
}

NvBool nv_acpi_power_resource_method_present(
    struct pci_dev *pdev
)
{
    acpi_handle handle = NULL;
    struct acpi_buffer buf = { ACPI_ALLOCATE_BUFFER, NULL };
    union acpi_object *object_package, *object_reference;
    acpi_status status;

#if defined(DEVICE_ACPI_HANDLE)
    handle = DEVICE_ACPI_HANDLE(&pdev->dev);
#elif defined (ACPI_HANDLE)
    handle = ACPI_HANDLE(&pdev->dev);
#endif

    if (!handle)
        return NV_FALSE;

    status = acpi_evaluate_object(handle, "_PR3", NULL, &buf);
    if (ACPI_FAILURE(status))
    {
        nv_printf(NV_DBG_INFO,"NVRM: Failed to evaluate _PR3 object\n");
        return NV_FALSE;
    }

    if (!buf.pointer)
    {
       nv_printf(NV_DBG_INFO, "NVRM: output buffer pointer is null"
                              " for _PR3 method\n");
       return NV_FALSE;
    }

    object_package = buf.pointer;

    /*
     * _PR3 object should be of type package and
     * it should contain only one reference
     */
    if ((object_package->type != ACPI_TYPE_PACKAGE) &&
        (object_package->package.count != 0x1))
    {
        nv_printf(NV_DBG_ERRORS,"NVRM: _PR3 object is not a type 'package'\n");
        return NV_FALSE;
    }

    object_reference = object_package->package.elements;

    /* Check for the reference and the actual type of the reference. */
    if ((object_reference->reference.actual_type != ACPI_TYPE_POWER) &&
        (object_reference->type !=  ACPI_TYPE_LOCAL_REFERENCE))
    {
        nv_printf(NV_DBG_ERRORS,
                     "NVRM: _PR3 object does not contain POWER Reference\n");
        return NV_FALSE;
    }
    return NV_TRUE;
}

/*
 * This function executes MUX ACPI methods.
 */
NV_STATUS NV_API_CALL nv_acpi_mux_method(
    nv_state_t *nv,
    NvU32 *pInOut,
    NvU32 muxAcpiId,
    const char *pMethodName
)
{
    acpi_status status;
    struct acpi_device *device    = NULL;
    struct acpi_buffer output     = { ACPI_ALLOCATE_BUFFER, NULL };
    union acpi_object *mux        = NULL;
    union acpi_object mux_arg     = { ACPI_TYPE_INTEGER };
    struct acpi_object_list input = { 1, &mux_arg };
    acpi_handle dev_handle        = NULL;
    acpi_handle mux_dev_handle    = NULL;
    unsigned long long device_id  = 0;
    struct list_head *node, *next;

    if ((strcmp(pMethodName, "MXDS") != 0)
        && (strcmp(pMethodName, "MXDM") != 0))
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: %s: Unsupported ACPI method %s\n",
                  __FUNCTION__, pMethodName);
        return NV_ERR_NOT_SUPPORTED;
    }
    else
    {
        nv_printf(NV_DBG_INFO, "NVRM: %s: Call for %s ACPI method \n",
                  __FUNCTION__, pMethodName);
    }

    if (!nv_acpi_get_device_handle(nv, &dev_handle))
        return NV_ERR_NOT_SUPPORTED;

    if (!dev_handle)
        return NV_ERR_INVALID_ARGUMENT;

#if defined(NV_ACPI_BUS_GET_DEVICE_PRESENT)
    status = acpi_bus_get_device(dev_handle, &device);
#else
    return NV_ERR_NOT_SUPPORTED;
#endif


    if (ACPI_FAILURE(status) || !device)
        return NV_ERR_INVALID_ARGUMENT;

    if (!NV_MAY_SLEEP())
    {
#if defined(DEBUG)
        nv_printf(NV_DBG_ERRORS, "NVRM: %s: invalid context!\n", __FUNCTION__);
#endif
        return NV_ERR_NOT_SUPPORTED;
    }

    list_for_each_safe(node, next, &device->children)
    {
        struct acpi_device *dev = list_entry(node, struct acpi_device, node);

        if (!dev)
            continue;

        status = acpi_evaluate_integer(dev->handle, "_ADR", NULL, &device_id);
        if (ACPI_FAILURE(status))
            /* Could not query device_id for this device */
            continue;

        if (device_id == muxAcpiId)
        {
            mux_dev_handle = dev->handle;
            break;
        }
    }

    if (mux_dev_handle == NULL)
    {
        nv_printf(NV_DBG_INFO,
                  "NVRM: %s Mux device handle not found\n", __FUNCTION__);
        return NV_ERR_GENERIC;
    }

    mux_arg.integer.type = ACPI_TYPE_INTEGER;
    mux_arg.integer.value = (NvU64) *pInOut;

    status = acpi_evaluate_object(mux_dev_handle, (acpi_string)pMethodName,
                                  &input, &output);

    if (ACPI_FAILURE(status))
    {
        nv_printf(NV_DBG_INFO, "NVRM: %s: Failed to evaluate %s method!\n",
                  __FUNCTION__, pMethodName);
        return NV_ERR_GENERIC;
    }
    else
    {
        mux = output.pointer;

        if (mux && (mux->type == ACPI_TYPE_INTEGER))
        {
            *pInOut = mux->integer.value;
        }
        else
        {
            nv_printf(NV_DBG_INFO,
                      "NVRM: %s: Invalid MUX data\n", __FUNCTION__);
            kfree(output.pointer);
            return NV_ERR_GENERIC;
        }
    }

    kfree(output.pointer);
    return NV_OK;
}

static acpi_status nv_acpi_find_battery_info(
    acpi_handle handle,
    NvBool bUseBix
)
{
    acpi_status status = AE_OK;
    struct acpi_buffer buf = { ACPI_ALLOCATE_BUFFER, NULL };
    union acpi_object *object_package;
    NvU32 battery_technology_offset;

    status = acpi_evaluate_object(handle, NULL, NULL, &buf);

    if (ACPI_FAILURE(status))
    {
        nv_printf(NV_DBG_INFO, "NVRM: Failed to evaluate battery's object\n");
        return AE_OK;
    }

    if (!buf.pointer)
    {
        nv_printf(NV_DBG_INFO, "NVRM: Battery object output buffer is null\n");
        return AE_OK;
    }

    object_package = buf.pointer;

    if (object_package->type != ACPI_TYPE_PACKAGE)
    {
        nv_printf(NV_DBG_INFO, "NVRM: Battery method output is not package\n");
        return AE_OK;
    }

    if (bUseBix)
    {
        battery_technology_offset = BIX_BATTERY_TECHNOLOGY_OFFSET;
    }
    else
    {
        battery_technology_offset = BIF_BATTERY_TECHNOLOGY_OFFSET;
    }

    /*
     * Only checking here for Battery technology type.
     * Other fields like Battery Model/Serial number could also be checked but
     * driver need to support the case where user has removed battery from the
     * system.
     * _STA method on the battery device handle couldn't be used due to the same
     * reason.
     * Hence just cheking if battery technology of slot is rechargable or not.
     */
    
    if ((object_package->package.elements[battery_technology_offset].type != ACPI_TYPE_INTEGER) ||
        (object_package->package.elements[battery_technology_offset].integer.value != BATTERY_RECHARGABLE))
    {
        return AE_OK;
    }

    battery_present = NV_TRUE;

    /* Stop traversing acpi tree. */
    return AE_CTRL_TERMINATE;
}

static acpi_status nv_acpi_find_battery_device(
    acpi_handle handle,
    u32 nest_level,
    void *dummy1,
    void **dummy2
)
{
    acpi_handle bif_method_handle;
    acpi_handle bix_method_handle;
    acpi_status status = AE_OK;

    // Find method Battery Information /Extended/ (_BIX or _BIF) and then Battery type.
    if (!acpi_get_handle(handle, "_BIX", &bix_method_handle))
    {
        status = nv_acpi_find_battery_info(bix_method_handle, NV_TRUE/*bUseBix*/);
    }

    if ((battery_present == NV_FALSE) &&
        !acpi_get_handle(handle, "_BIF", &bif_method_handle))
    {
        status = nv_acpi_find_battery_info(bif_method_handle, NV_FALSE/*bUseBix*/);
    }

    return status;
}

NvBool NV_API_CALL nv_acpi_is_battery_present(void)
{
    NV_ACPI_WALK_NAMESPACE(ACPI_TYPE_DEVICE, ACPI_ROOT_OBJECT, ACPI_UINT32_MAX,
                           nv_acpi_find_battery_device, NULL, NULL);

    if (battery_present == NV_TRUE)
    {
        return NV_TRUE;
    }

    return NV_FALSE;
}

#else // NV_LINUX_ACPI_EVENTS_SUPPORTED

int nv_acpi_init(void)
{
    return 0;
}

int nv_acpi_uninit(void)
{
    return 0;
}

void NV_API_CALL nv_acpi_methods_init(NvU32 *handlePresent)
{
    *handlePresent = 0;
}

void NV_API_CALL nv_acpi_methods_uninit(void)
{
    return;
}

NV_STATUS NV_API_CALL nv_acpi_method(
    NvU32 acpi_method,
    NvU32 function,
    NvU32 subFunction,
    void  *inParams,
    NvU16 inParamSize,
    NvU32 *outStatus,
    void  *outData,
    NvU16 *outDataSize
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS NV_API_CALL nv_acpi_dsm_method(
    nv_state_t *nv,
    NvU8  *pAcpiDsmGuid,
    NvU32 acpiDsmRev,
    NvBool acpiNvpcfDsmFunction,
    NvU32 acpiDsmSubFunction,
    void  *pInParams,
    NvU16 inParamSize,
    NvU32 *outStatus,
    void  *pOutData,
    NvU16 *pSize
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS NV_API_CALL nv_acpi_ddc_method(
    nv_state_t *nv,
    void *pEdidBuffer,
    NvU32 *pSize,
    NvBool bReadMultiBlock
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS NV_API_CALL nv_acpi_rom_method(
    nv_state_t *nv,
    NvU32 *pInData,
    NvU32 *pOutData
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS NV_API_CALL nv_acpi_dod_method(
    nv_state_t *nv,
    NvU32      *pOutData,
    NvU32      *pSize
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NvBool nv_acpi_power_resource_method_present(
    struct pci_dev *pdev
)
{
    return NV_FALSE;
}

NV_STATUS NV_API_CALL nv_acpi_get_powersource(NvU32 *ac_plugged)
{
    return NV_ERR_NOT_SUPPORTED;
}

void nv_acpi_register_notifier(nv_linux_state_t *nvl)
{
    return;
}

void nv_acpi_unregister_notifier(nv_linux_state_t *nvl)
{
    return;
}

NV_STATUS NV_API_CALL nv_acpi_mux_method(
    nv_state_t *nv,
    NvU32 *pInOut,
    NvU32 muxAcpiId,
    const char *pMethodName
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NvBool NV_API_CALL nv_acpi_is_battery_present(void)
{
    return NV_FALSE;
}
#endif
