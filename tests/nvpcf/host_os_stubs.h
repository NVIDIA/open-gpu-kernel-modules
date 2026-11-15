/* Host stand-ins for the NVPCF resample unit test. */
#ifndef NVPCF_HOST_OS_STUBS_H
#define NVPCF_HOST_OS_STUBS_H

#include <stdio.h>
#include <string.h>

typedef unsigned char NvU8;
typedef unsigned short NvU16;
typedef unsigned int NvU32;
typedef unsigned long long NvU64;
typedef int NvS32;
typedef NvU8 NvBool;
typedef NvU32 NvHandle;
typedef int NV_STATUS;
typedef unsigned long NvLength;
typedef struct nv_stack { int unused; } nvidia_stack_t;
typedef nvidia_stack_t nv_stack_t;

#define NV_API_CALL
#define NV_OK 0
#define NV_ERR_INVALID_STATE 0x35
#define NV_TRUE 1
#define NV_FALSE 0
#define LEVEL_INFO 3
#define RMAPI_GPU_LOCK_INTERNAL 7
#define NV0000_CTRL_CMD_SYSTEM_NVPCF_GET_POWER_MODE_INFO 0x13bU
#define NVPCF0100_CTRL_CONFIG_DSM_2X_FUNC_GET_STATIC_CASE 4U
#define NVPCF0100_CTRL_CONFIG_DSM_2X_FUNC_GET_DYNAMIC_CASE 3U
#define API_LOCK_FLAGS_NONE 0
#define GPUS_LOCK_FLAGS_NONE 0
#define RM_LOCK_MODULES_EVENT 11
#define THREAD_STATE_FLAGS_NONE 0
#define NV_DYNAMIC_PM_FINE 2
#define NV2080_NOTIFIERS_NVPCF_EVENTS 0xC0U

typedef struct nv_state_t {
    struct {
        NvHandle hClient;
    } rmapi;
} nv_state_t;

typedef struct {
    NvU32 gpuId;
    void *pOsGpuInfo;
} OBJGPU;

#define NV_GET_NV_STATE(pGpu) \
    ((nv_state_t *)((pGpu) ? (pGpu)->pOsGpuInfo : NULL))

typedef struct NV0000_CTRL_SYSTEM_NVPCF_GET_POWER_MODE_INFO_PARAMS {
    NvU32 gpuId;
    NvU32 subFunc;
    NvS32 ctgpOffsetmW;
    NvS32 targetTppOffsetmW;
    NvS32 maxOutputOffsetmW;
    NvS32 minOutputOffsetmW;
    NvS32 ctgpBattOffsetmW;
    NvS32 targetTppBattOffsetmW;
    NvS32 maxOutputBattOffsetmW;
    NvS32 minOutputBattOffsetmW;
    NvU32 dcRosReserveOverridemW;
    NvU32 dcTspLongTimescaleLimitOverridemA;
    NvU32 dcTspShortTimescaleLimitOverridemA;
    NvU32 tpp;
    NvU32 ratedTgp;
    NvBool bEnableForAC;
    NvBool bEnableForDC;
    NvU8 version;
} NV0000_CTRL_SYSTEM_NVPCF_GET_POWER_MODE_INFO_PARAMS;

typedef struct THREAD_STATE_NODE { int unused; } THREAD_STATE_NODE;

typedef struct _RM_API {
    NV_STATUS (*Control)(struct _RM_API *pRmApi, NvHandle hClient, NvHandle hObject,
                         NvU32 cmd, void *pParams, NvU32 paramsSize);
} RM_API;

static char nvpcf_logs[16][192];
static int nvpcf_log_n;
static char nvpcf_trace[24][24];
static int nvpcf_trace_n;
static int nvpcf_enter_calls;
static int nvpcf_exit_calls;
static nvidia_stack_t *nvpcf_enter_sp;
static int nvpcf_rmapi_which;
static int nvpcf_control_calls;
static NvU32 nvpcf_last_cmd;
static NvU32 nvpcf_last_sub;
static NvU32 nvpcf_last_ver;
static NvU32 nvpcf_last_gpu;
static NvHandle nvpcf_last_hclient;
static NvHandle nvpcf_last_hobject;
static NvU32 nvpcf_last_size;
static NV_STATUS nvpcf_static_status;
static NV_STATUS nvpcf_dynamic_status;
static NvU8 nvpcf_static_version;
static NV0000_CTRL_SYSTEM_NVPCF_GET_POWER_MODE_INFO_PARAMS nvpcf_dynamic;
static NV_STATUS nvpcf_api_lock_status;
static NV_STATUS nvpcf_gpu_lock_status;
static NV_STATUS nvpcf_ref_status;
static int nvpcf_gpu_locked;
static int nvpcf_api_locked;
static int nvpcf_notify_calls;
static NvU32 nvpcf_notify_id;
static OBJGPU *nvpcf_gpu;
static RM_API nvpcf_rmapi;

#define NV_PRINTF(level, fmt, ...)                                             \
    do                                                                         \
    {                                                                          \
        (void)(level);                                                         \
        snprintf(nvpcf_logs[nvpcf_log_n & 15], sizeof(nvpcf_logs[0]), fmt,    \
                 ##__VA_ARGS__);                                               \
        nvpcf_log_n++;                                                         \
    } while (0)

#define NV_ENTER_RM_RUNTIME(sp, fp)                                            \
    do                                                                         \
    {                                                                          \
        (void)(fp);                                                            \
        nvpcf_enter_calls++;                                                   \
        nvpcf_enter_sp = (sp);                                                 \
    } while (0)

#define NV_EXIT_RM_RUNTIME(sp, fp)                                             \
    do                                                                         \
    {                                                                          \
        (void)(sp);                                                            \
        (void)(fp);                                                            \
        nvpcf_exit_calls++;                                                    \
    } while (0)

static void *portMemSet(void *pData, NvU8 value, NvLength lengthBytes)
{
    return memset(pData, value, lengthBytes);
}

static void nvpcf_trace_add(const char *s)
{
    if (nvpcf_trace_n < 24)
        snprintf(nvpcf_trace[nvpcf_trace_n++], sizeof(nvpcf_trace[0]), "%s", s);
}

static void threadStateInit(THREAD_STATE_NODE *p, NvU32 flags)
{
    (void)p;
    (void)flags;
}

static void threadStateFree(THREAD_STATE_NODE *p, NvU32 flags)
{
    (void)p;
    (void)flags;
}

static NV_STATUS nvpcf_control(RM_API *pRmApi, NvHandle hClient, NvHandle hObject,
                               NvU32 cmd, void *pParams, NvU32 paramsSize)
{
    NV0000_CTRL_SYSTEM_NVPCF_GET_POWER_MODE_INFO_PARAMS *p = pParams;

    (void)pRmApi;
    nvpcf_control_calls++;
    nvpcf_last_hclient = hClient;
    nvpcf_last_hobject = hObject;
    nvpcf_last_cmd = cmd;
    nvpcf_last_size = paramsSize;
    nvpcf_last_sub = p->subFunc;
    nvpcf_last_ver = p->version;
    nvpcf_last_gpu = p->gpuId;
    if (p->subFunc == NVPCF0100_CTRL_CONFIG_DSM_2X_FUNC_GET_STATIC_CASE)
    {
        if (nvpcf_static_status != NV_OK)
            return nvpcf_static_status;
        memset(p, 0, sizeof(*p));
        p->version = nvpcf_static_version;
        return NV_OK;
    }
    if (nvpcf_dynamic_status != NV_OK)
        return nvpcf_dynamic_status;
    *p = nvpcf_dynamic;
    return NV_OK;
}

static RM_API *rmapiGetInterface(int which)
{
    nvpcf_rmapi_which = which;
    nvpcf_rmapi.Control = nvpcf_control;
    return &nvpcf_rmapi;
}

static NV_STATUS rmapiLockAcquire(NvU32 flags, NvU32 module)
{
    (void)flags;
    (void)module;
    nvpcf_trace_add("api_lock");
    if (nvpcf_api_lock_status == NV_OK)
        nvpcf_api_locked = 1;
    return nvpcf_api_lock_status;
}

static void rmapiLockRelease(void)
{
    nvpcf_api_locked = 0;
    nvpcf_trace_add("api_unlock");
}

static OBJGPU *gpumgrGetGpu(NvU32 index)
{
    (void)index;
    nvpcf_trace_add("get_gpu");
    return nvpcf_gpu;
}

static NV_STATUS rmGpuLocksAcquire(NvU32 flags, NvU32 module)
{
    (void)flags;
    (void)module;
    nvpcf_trace_add("gpu_lock");
    if (nvpcf_gpu_lock_status == NV_OK)
        nvpcf_gpu_locked = 1;
    return nvpcf_gpu_lock_status;
}

static void rmGpuLocksRelease(NvU32 flags, void *unused)
{
    (void)flags;
    (void)unused;
    nvpcf_gpu_locked = 0;
    nvpcf_trace_add("gpu_unlock");
}

static NV_STATUS os_ref_dynamic_power(nv_state_t *nv, int mode)
{
    (void)nv;
    (void)mode;
    nvpcf_trace_add("ref");
    return nvpcf_ref_status;
}

static void os_unref_dynamic_power(nv_state_t *nv, int mode)
{
    (void)nv;
    (void)mode;
    nvpcf_trace_add("unref");
}

static void gpuNotifySubDeviceEvent(OBJGPU *pGpu, NvU32 id, void *p,
                                    NvU32 a, NvU32 b, NvU32 c)
{
    (void)pGpu;
    (void)p;
    (void)a;
    (void)b;
    (void)c;
    nvpcf_notify_calls++;
    nvpcf_notify_id = id;
    nvpcf_trace_add("notify");
}

#endif
