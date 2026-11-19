/* Host stand-ins for the NVPCF ACPI unit test. */
#ifndef NVPCF_HOST_ACPI_STUBS_H
#define NVPCF_HOST_ACPI_STUBS_H

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

typedef unsigned char NvU8;
typedef unsigned short NvU16;
typedef unsigned int NvU32;
typedef int NV_STATUS;
typedef NvU8 NvBool;
typedef void *acpi_handle;
typedef unsigned int u32;
typedef int acpi_status;
typedef unsigned int acpi_object_type;
typedef struct nv_stack { int tag; } nvidia_stack_t;

#define NV_API_CALL
#define NV_OK 0
#define NV_TRUE 1
#define NV_FALSE 0
#define NV_ERR_NOT_SUPPORTED 0x56
#define NV_DBG_INFO 1
#define ACPI_TYPE_ANY 0
#define ACPI_TYPE_DEVICE 6
#define ACPI_ROOT_OBJECT ((acpi_handle)0)
#define ACPI_UINT32_MAX 0xffffffffU
#define ACPI_SUCCESS(x) ((x) == 0)
#define NV_ACPI_NVIF_HANDLE_PRESENT 0x01
#define NV_ACPI_WMMX_HANDLE_PRESENT 0x04
#define ACPI_NVPCF_EVENT_CHANGE_VALUE 0xC0

union acpi_object { int unused; };
struct device { int unused; };
struct pci_dev { int unused; };
typedef struct nv_state_t { int unused; } nv_state_t;
typedef struct nv_linux_state_t { int unused; } nv_linux_state_t;
typedef struct nv_acpi_t {
    acpi_handle handle;
} nv_acpi_t;

typedef void (*acpi_notify_handler)(acpi_handle, u32, void *);

struct work_struct {
    void (*fn)(struct work_struct *);
};

#define DEFINE_SPINLOCK(name) int name
#define DECLARE_WORK(name, fn) struct work_struct name = { fn }

static int nvpcf_lock_depth;
static int nvpcf_schedule_calls;
static int nvpcf_cancel_calls;
static int nvpcf_cancel_sync_calls;
static int nvpcf_printf_calls;
static char nvpcf_printf_buf[160];
static int nvpcf_alloc_result;
static nvidia_stack_t *nvpcf_alloc_stack;
static int nvpcf_alloc_calls;
static int nvpcf_free_calls;
static nvidia_stack_t *nvpcf_freed;
static int nvpcf_notify_calls;
static nvidia_stack_t *nvpcf_notified_sp;
static int nvpcf_drop_calls;
static int nvpcf_install_calls;
static acpi_handle nvpcf_installed_handle;
static acpi_notify_handler nvpcf_installed_handler;
static int nvpcf_uninstall_calls;
static nv_acpi_t nvpcf_installed_object;
static acpi_status nvpcf_type_status;
static acpi_object_type nvpcf_type_value;
static const char *nvpcf_found_name[8];
static acpi_handle nvpcf_found_handle[8];
static int nvpcf_found_n;

#define spin_lock_irqsave(lock, flags)                                         \
    do                                                                         \
    {                                                                          \
        (void)(lock);                                                          \
        (void)(flags);                                                         \
        nvpcf_lock_depth++;                                                    \
    } while (0)

#define spin_unlock_irqrestore(lock, flags)                                    \
    do                                                                         \
    {                                                                          \
        (void)(lock);                                                          \
        (void)(flags);                                                         \
        nvpcf_lock_depth--;                                                    \
    } while (0)

static int nvpcf_schedule(void *work)
{
    (void)work;
    nvpcf_schedule_calls++;
    return 1;
}
#define schedule_work(work) nvpcf_schedule(work)
#define cancel_work(work)                                                      \
    do                                                                         \
    {                                                                          \
        (void)(work);                                                          \
        nvpcf_cancel_calls++;                                                  \
    } while (0)
#define cancel_work_sync(work)                                                 \
    do                                                                         \
    {                                                                          \
        (void)(work);                                                          \
        nvpcf_cancel_sync_calls++;                                             \
    } while (0)

#define nv_printf(level, fmt, ...)                                             \
    do                                                                         \
    {                                                                          \
        (void)(level);                                                         \
        nvpcf_printf_calls++;                                                  \
        snprintf(nvpcf_printf_buf, sizeof(nvpcf_printf_buf), fmt,              \
                 ##__VA_ARGS__);                                               \
    } while (0)

#define NV_ACPI_WALK_NAMESPACE(type, start, depth, fn, ...)                    \
    fn((acpi_handle)(void *)0x10, 0, NULL, NULL)

static int nv_kmem_cache_alloc_stack(nvidia_stack_t **sp)
{
    nvpcf_alloc_calls++;
    if (nvpcf_alloc_result != 0)
        return nvpcf_alloc_result;
    *sp = nvpcf_alloc_stack;
    return 0;
}

static void nv_kmem_cache_free_stack(nvidia_stack_t *sp)
{
    nvpcf_free_calls++;
    nvpcf_freed = sp;
}

static void rm_acpi_nvpcf_notify(nvidia_stack_t *sp)
{
    nvpcf_notify_calls++;
    nvpcf_notified_sp = sp;
}

static void rm_nvpcf_drop_cached_budget(void)
{
    nvpcf_drop_calls++;
}

static nv_acpi_t *nv_install_notifier(acpi_handle handle,
                                     acpi_notify_handler handler,
                                     void *data)
{
    (void)data;
    nvpcf_install_calls++;
    nvpcf_installed_handle = handle;
    nvpcf_installed_handler = handler;
    nvpcf_installed_object.handle = handle;
    return &nvpcf_installed_object;
}

static void nv_uninstall_notifier(nv_acpi_t *obj, acpi_notify_handler handler)
{
    (void)obj;
    (void)handler;
    nvpcf_uninstall_calls++;
}

static void nv_acpi_powersource_hotplug_event(acpi_handle handle, u32 event,
                                             void *data)
{
    (void)handle;
    (void)event;
    (void)data;
}

static acpi_status acpi_get_handle(acpi_handle parent, const char *name,
                                  acpi_handle *out)
{
    int i;

    (void)parent;
    for (i = 0; i < nvpcf_found_n; i++)
    {
        if (strcmp(nvpcf_found_name[i], name) == 0)
        {
            *out = nvpcf_found_handle[i];
            return 0;
        }
    }
    return 5;
}

static acpi_status acpi_get_type(acpi_handle handle, acpi_object_type *type)
{
    (void)handle;
    *type = nvpcf_type_value;
    return nvpcf_type_status;
}

#endif
