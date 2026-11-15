/* Host stand-ins for the module-exit and suspend order test. */
#ifndef NVPCF_HOST_NV_STUBS_H
#define NVPCF_HOST_NV_STUBS_H

typedef int NV_STATUS;
typedef unsigned char NvBool;
typedef struct nv_stack { int unused; } nv_stack_t;
typedef struct device { int unused; } device;
typedef int nv_pm_action_t;

#define NV_OK 0

static const char *nvpcf_order[8];
static int nvpcf_order_n;
static int nvpcf_drop_calls;

static void nvpcf_order_add(const char *name)
{
    if (nvpcf_order_n < 8)
        nvpcf_order[nvpcf_order_n++] = name;
}

static void nv_module_state_exit(nv_stack_t *sp)
{
    (void)sp;
    nvpcf_order_add("state_exit");
}

static void nv_acpi_nvpcf_work_cancel(void)
{
    nvpcf_order_add("cancel");
}

static void rm_shutdown_rm(nv_stack_t *sp)
{
    (void)sp;
    nvpcf_order_add("shutdown");
}

static void nv_destroy_rsync_info(void)
{
    nvpcf_order_add("rsync");
}

static void nvlink_drivers_exit(void)
{
    nvpcf_order_add("nvlink");
}

static void nv_cap_drv_exit(void)
{
    nvpcf_order_add("cap");
}

static void nv_module_resources_exit(nv_stack_t *sp)
{
    (void)sp;
    nvpcf_order_add("resources");
}

static void rm_nvpcf_drop_cached_budget(void)
{
    nvpcf_drop_calls++;
    nvpcf_order_add("drop");
}

#endif
