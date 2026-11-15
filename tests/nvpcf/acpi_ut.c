#define NVPCF_HOST_TEST 1
#define NVPCF_HOST_ACPI 1
#include "nv-acpi.c"

#include <stdio.h>
#include <string.h>

static int failures;

static void expect(int cond, const char *name)
{
    if (cond)
        return;
    fprintf(stderr, "FAIL %s\n", name);
    failures++;
}

static void reset_acpi(void)
{
    nvif_handle = NULL;
    wmmx_handle = NULL;
    psr_handle = NULL;
    psr_device_handle = NULL;
    psr_nv_acpi_object = NULL;
    nvpcf_handle = NULL;
    nvpcf_device_handle = NULL;
    nvpcf_nv_acpi_object = NULL;
    nvpcf_notify_dead = 0;
    nvpcf_found_n = 0;
    nvpcf_type_status = 0;
    nvpcf_type_value = ACPI_TYPE_ANY;
    nvpcf_schedule_calls = 0;
    nvpcf_cancel_calls = 0;
    nvpcf_cancel_sync_calls = 0;
    nvpcf_printf_calls = 0;
    nvpcf_alloc_calls = 0;
    nvpcf_free_calls = 0;
    nvpcf_notify_calls = 0;
    nvpcf_drop_calls = 0;
    nvpcf_install_calls = 0;
    nvpcf_uninstall_calls = 0;
    nvpcf_alloc_result = 0;
    nvpcf_alloc_stack = NULL;
    nvpcf_lock_depth = 0;
}

static void allow(const char *name, acpi_handle handle)
{
    nvpcf_found_name[nvpcf_found_n] = name;
    nvpcf_found_handle[nvpcf_found_n] = handle;
    nvpcf_found_n++;
}

static void test_choose_handle(void)
{
    acpi_handle parent = (acpi_handle)(void *)0x100;
    acpi_handle method = (acpi_handle)(void *)0x200;
    NvU32 present = 0;

    reset_acpi();
    allow("NPCF", method);
    nvpcf_type_status = 0;
    nvpcf_type_value = ACPI_TYPE_DEVICE;
    expect(nv_acpi_find_methods(parent, 0, NULL, NULL) == 0, "find returns 0");
    expect(nvpcf_handle == method, "npfc handle");
    expect(nvpcf_device_handle == method, "device handle is the device");

    reset_acpi();
    allow("NPCF", method);
    nvpcf_type_value = ACPI_TYPE_ANY;
    nv_acpi_find_methods(parent, 0, NULL, NULL);
    expect(nvpcf_device_handle == parent, "non-device keeps parent");

    reset_acpi();
    allow("NPCF", method);
    nvpcf_type_status = 5;
    nvpcf_type_value = ACPI_TYPE_DEVICE;
    nv_acpi_find_methods(parent, 0, NULL, NULL);
    expect(nvpcf_device_handle == parent, "type failure keeps parent");

    reset_acpi();
    allow("NVIF", (acpi_handle)(void *)0x1);
    allow("WMMX", (acpi_handle)(void *)0x2);
    allow("_PSR", (acpi_handle)(void *)0x3);
    nv_acpi_find_methods(parent, 0, NULL, NULL);
    expect(nvif_handle == (acpi_handle)(void *)0x1, "nvif");
    expect(wmmx_handle == (acpi_handle)(void *)0x2, "wmmx");
    expect(psr_handle == (acpi_handle)(void *)0x3, "psr");
    expect(psr_device_handle == parent, "psr parent");
    expect(nvpcf_handle == NULL, "missing npcf");

    reset_acpi();
    nv_acpi_methods_init(NULL);
    nv_acpi_methods_init(&present);
    expect(present == 0, "nothing present");
    expect(nvpcf_install_calls == 0, "no install");
}

static void test_init_installs_and_arms(void)
{
    NvU32 present = 0;
    acpi_handle method = (acpi_handle)(void *)0x200;

    reset_acpi();
    allow("NVIF", (acpi_handle)(void *)0x1);
    allow("WMMX", (acpi_handle)(void *)0x2);
    allow("NPCF", method);
    nvpcf_type_value = ACPI_TYPE_DEVICE;
    nvpcf_notify_dead = 1;
    nv_acpi_methods_init(&present);
    expect(present == (NV_ACPI_NVIF_HANDLE_PRESENT | NV_ACPI_WMMX_HANDLE_PRESENT),
           "handle flags");
    expect(nvpcf_notify_dead == 0, "init clears dead");
    expect(nvpcf_install_calls == 1, "installs nvpcf");
    expect(nvpcf_installed_handle == method, "installed on device");
    expect(nvpcf_installed_handler == nv_acpi_nvpcf_event, "nvpcf handler");
    expect(nvpcf_nv_acpi_object != NULL, "object stored");

    nv_acpi_methods_init(&present);
    expect(nvpcf_install_calls == 1, "second init does not reinstall");
}

static void test_events_and_work(void)
{
    struct work_struct *work = &nvpcf_notify_work;
    nvidia_stack_t stack;

    reset_acpi();
    nvpcf_notify_dead = 0;
    nv_acpi_nvpcf_event((acpi_handle)0, 0x10, NULL);
    expect(nvpcf_schedule_calls == 0, "unsupported event");
    expect(nvpcf_printf_calls == 1, "unsupported log");
    expect(strstr(nvpcf_printf_buf, "not supported") != NULL, "unsupported text");
    expect(nvpcf_lock_depth == 0, "lock balanced");

    nv_acpi_nvpcf_event((acpi_handle)0, ACPI_NVPCF_EVENT_CHANGE, NULL);
    nv_acpi_nvpcf_event((acpi_handle)0, ACPI_NVPCF_EVENT_CHANGE, NULL);
    expect(nvpcf_schedule_calls == 2, "each 0xC0 schedules");
    expect(work->fn == nv_acpi_nvpcf_work, "work function bound");

    nvpcf_notify_dead = 1;
    nv_acpi_nvpcf_event((acpi_handle)0, ACPI_NVPCF_EVENT_CHANGE, NULL);
    expect(nvpcf_schedule_calls == 2, "dead does not schedule");

    nvpcf_alloc_result = -1;
    nv_acpi_nvpcf_work(work);
    expect(nvpcf_notify_calls == 0 && nvpcf_free_calls == 0, "alloc failure returns");

    nvpcf_alloc_result = 0;
    nvpcf_alloc_stack = NULL;
    nv_acpi_nvpcf_work(work);
    expect(nvpcf_notify_calls == 1 && nvpcf_notified_sp == NULL, "null stack notifies");
    expect(nvpcf_free_calls == 1 && nvpcf_freed == NULL, "null stack freed");

    nvpcf_alloc_stack = &stack;
    nv_acpi_nvpcf_work(work);
    expect(nvpcf_notified_sp == &stack && nvpcf_freed == &stack, "owned stack");
    expect(nvpcf_lock_depth == 0, "work lock balanced");
}

static void test_unbind_and_exit_cancel(void)
{
    NvU32 present = 0;

    reset_acpi();
    nv_acpi_methods_uninit();
    expect(nvpcf_cancel_calls == 0 && nvpcf_drop_calls == 0, "unbind without object");

    allow("NPCF", (acpi_handle)(void *)0x200);
    allow("_PSR", (acpi_handle)(void *)0x3);
    nvpcf_type_value = ACPI_TYPE_DEVICE;
    nv_acpi_methods_init(&present);
    expect(nvpcf_install_calls == 2, "psr and nvpcf installed");
    nvpcf_notify_dead = 0;
    nv_acpi_methods_uninit();
    expect(nvpcf_notify_dead == 1, "unbind sets dead");
    expect(nvpcf_cancel_calls == 1, "unbind cancel_work");
    expect(nvpcf_cancel_sync_calls == 0, "unbind does not wait");
    expect(nvpcf_uninstall_calls == 2, "both notifiers removed");
    expect(nvpcf_handle == NULL && nvpcf_device_handle == NULL, "handles cleared");
    expect(nvpcf_nv_acpi_object == NULL && psr_nv_acpi_object == NULL, "objects cleared");
    expect(nvpcf_drop_calls == 1, "cache dropped on unbind");
    expect(nvif_handle == NULL && wmmx_handle == NULL, "other handles cleared");
    expect(nvpcf_lock_depth == 0, "unbind lock balanced");

    nv_acpi_nvpcf_work_cancel();
    expect(nvpcf_notify_dead == 1, "exit cancel stays dead");
    expect(nvpcf_cancel_sync_calls == 1, "exit waits");
    expect(nvpcf_cancel_calls == 1, "exit does not use cancel_work");
}

int main(void)
{
    test_choose_handle();
    test_init_installs_and_arms();
    test_events_and_work();
    test_unbind_and_exit_cancel();
    if (failures)
    {
        fprintf(stderr, "%d acpi nvpcf tests failed\n", failures);
        return 1;
    }
    printf("acpi nvpcf: all tests passed\n");
    return 0;
}
