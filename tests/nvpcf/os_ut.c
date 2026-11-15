#define NVPCF_HOST_TEST 1
#include "osapi.c"

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

static NV0000_CTRL_SYSTEM_NVPCF_GET_POWER_MODE_INFO_PARAMS sample(void)
{
    NV0000_CTRL_SYSTEM_NVPCF_GET_POWER_MODE_INFO_PARAMS b;

    memset(&b, 0, sizeof(b));
    b.version = 0x25;
    b.ctgpOffsetmW = 55000;
    b.targetTppOffsetmW = 160000;
    b.maxOutputOffsetmW = 1000;
    b.minOutputOffsetmW = -1000;
    b.ctgpBattOffsetmW = 20000;
    b.targetTppBattOffsetmW = 80000;
    b.maxOutputBattOffsetmW = 500;
    b.minOutputBattOffsetmW = -500;
    b.bEnableForAC = 1;
    b.bEnableForDC = 1;
    b.tpp = 160000;
    b.ratedTgp = 175000;
    b.dcRosReserveOverridemW = 15000;
    b.dcTspLongTimescaleLimitOverridemA = 4000;
    b.dcTspShortTimescaleLimitOverridemA = 8000;
    b.gpuId = 7;
    b.subFunc = NVPCF0100_CTRL_CONFIG_DSM_2X_FUNC_GET_DYNAMIC_CASE;
    return b;
}

static int trace_is(const char **want, int n)
{
    int i;

    if (nvpcf_trace_n != n)
        return 0;
    for (i = 0; i < n; i++)
    {
        if (strcmp(nvpcf_trace[i], want[i]) != 0)
            return 0;
    }
    return 1;
}

static int log_has(const char *fragment)
{
    int i;

    for (i = 0; i < nvpcf_log_n; i++)
    {
        if (strstr(nvpcf_logs[i], fragment) != NULL)
            return 1;
    }
    return 0;
}

static void test_same_budget(void)
{
    NV0000_CTRL_SYSTEM_NVPCF_GET_POWER_MODE_INFO_PARAMS a = sample();
    NV0000_CTRL_SYSTEM_NVPCF_GET_POWER_MODE_INFO_PARAMS b = sample();

    expect(rm_nvpcf_same_budget(&a, &b), "identical");

    b = a;
    b.version = 0x24;
    expect(!rm_nvpcf_same_budget(&a, &b), "version");
    b = a;
    b.ctgpOffsetmW++;
    expect(!rm_nvpcf_same_budget(&a, &b), "ctgp");
    b = a;
    b.targetTppOffsetmW++;
    expect(!rm_nvpcf_same_budget(&a, &b), "tpp offset");
    b = a;
    b.maxOutputOffsetmW++;
    expect(!rm_nvpcf_same_budget(&a, &b), "max output");
    b = a;
    b.minOutputOffsetmW++;
    expect(!rm_nvpcf_same_budget(&a, &b), "min output");
    b = a;
    b.ctgpBattOffsetmW++;
    expect(!rm_nvpcf_same_budget(&a, &b), "dc ctgp");
    b = a;
    b.targetTppBattOffsetmW++;
    expect(!rm_nvpcf_same_budget(&a, &b), "dc tpp");
    b = a;
    b.maxOutputBattOffsetmW++;
    expect(!rm_nvpcf_same_budget(&a, &b), "dc max");
    b = a;
    b.minOutputBattOffsetmW++;
    expect(!rm_nvpcf_same_budget(&a, &b), "dc min");
    b = a;
    b.bEnableForAC = 0;
    expect(!rm_nvpcf_same_budget(&a, &b), "ac enable");
    b = a;
    b.bEnableForDC = 0;
    expect(!rm_nvpcf_same_budget(&a, &b), "dc enable");
    b = a;
    b.tpp++;
    expect(!rm_nvpcf_same_budget(&a, &b), "tpp");
    b = a;
    b.ratedTgp++;
    expect(!rm_nvpcf_same_budget(&a, &b), "rated tgp");
    b = a;
    b.dcRosReserveOverridemW++;
    expect(!rm_nvpcf_same_budget(&a, &b), "ros");
    b = a;
    b.dcTspLongTimescaleLimitOverridemA++;
    expect(!rm_nvpcf_same_budget(&a, &b), "long tsp");
    b = a;
    b.dcTspShortTimescaleLimitOverridemA++;
    expect(!rm_nvpcf_same_budget(&a, &b), "short tsp");

    a = sample();
    b = sample();
    a.version = b.version = 0x23;
    a.dcTspLongTimescaleLimitOverridemA = b.dcTspLongTimescaleLimitOverridemA = 0;
    a.dcTspShortTimescaleLimitOverridemA = b.dcTspShortTimescaleLimitOverridemA = 0;
    expect(rm_nvpcf_same_budget(&a, &b), "zero tsp still compares");
}

static void test_control_rejects_missing_client(void)
{
    NV0000_CTRL_SYSTEM_NVPCF_GET_POWER_MODE_INFO_PARAMS p = sample();
    nv_state_t nv;

    memset(&nv, 0, sizeof(nv));
    nvpcf_control_calls = 0;
    expect(rm_nvpcf_control(&nv, &p) == NV_ERR_INVALID_STATE, "no client status");
    expect(nvpcf_control_calls == 0, "no client skips Control");
}

static void test_no_client_wakes_and_logs_once(void)
{
    nv_state_t nv;
    OBJGPU gpu;

    memset(&nv, 0, sizeof(nv));
    memset(&gpu, 0, sizeof(gpu));
    nvpcf_log_n = 0;
    expect(rm_nvpcf_resample_power_budget(&gpu, &nv) == NV_TRUE, "no client wakes");
    expect(log_has("no RM client"), "no client log");
    expect(rm_nvpcf_loggedNoClient == NV_TRUE, "no client flag");
    nvpcf_log_n = 0;
    expect(rm_nvpcf_resample_power_budget(&gpu, &nv) == NV_TRUE, "no client wakes again");
    expect(nvpcf_log_n == 0, "no client log once");
}

static void test_static_failure_does_not_cache(void)
{
    nv_state_t nv;
    OBJGPU gpu;

    memset(&nv, 0, sizeof(nv));
    memset(&gpu, 0, sizeof(gpu));
    nv.rmapi.hClient = 0x11;
    gpu.gpuId = 7;
    nvpcf_static_status = NV_ERR_INVALID_STATE;
    nvpcf_log_n = 0;
    nvpcf_control_calls = 0;
    expect(rm_nvpcf_resample_power_budget(&gpu, &nv) == NV_TRUE, "static fail wakes");
    expect(rm_nvpcf_haveVersion == NV_FALSE, "static fail not cached");
    expect(log_has("static table read failed"), "static log");
    expect(nvpcf_last_sub == NVPCF0100_CTRL_CONFIG_DSM_2X_FUNC_GET_STATIC_CASE,
           "static subfunc");
    expect(nvpcf_last_gpu == 7, "static gpu id");
    expect(nvpcf_rmapi_which == RMAPI_GPU_LOCK_INTERNAL, "internal rmapi");
    expect(nvpcf_last_hclient == 0x11 && nvpcf_last_hobject == 0x11, "client object");
    expect(nvpcf_last_cmd == NV0000_CTRL_CMD_SYSTEM_NVPCF_GET_POWER_MODE_INFO, "cmd");
    expect(nvpcf_last_size == sizeof(nvpcf_dynamic), "param size");
    nvpcf_log_n = 0;
    expect(rm_nvpcf_resample_power_budget(&gpu, &nv) == NV_TRUE, "static fail again");
    expect(nvpcf_log_n == 0, "static log once");

    nvpcf_static_status = NV_OK;
    nvpcf_static_version = 0;
    expect(rm_nvpcf_resample_power_budget(&gpu, &nv) == NV_TRUE, "version 0 wakes");
    expect(rm_nvpcf_haveVersion == NV_FALSE, "version 0 not cached");
    expect(rm_nvpcf_version == 0, "version stays 0");
}

static void test_dynamic_failure_keeps_last_budget(void)
{
    nv_state_t nv;
    OBJGPU gpu;

    memset(&nv, 0, sizeof(nv));
    memset(&gpu, 0, sizeof(gpu));
    nv.rmapi.hClient = 0x11;
    gpu.gpuId = 7;
    nvpcf_static_version = 0x25;
    nvpcf_dynamic = sample();
    expect(rm_nvpcf_resample_power_budget(&gpu, &nv) == NV_TRUE, "first sample wakes");
    expect(rm_nvpcf_haveVersion == NV_TRUE, "version cached");
    expect(rm_nvpcf_version == 0x25, "cached version");
    expect(log_has("NVPCF resample version"), "first sample log");

    nvpcf_control_calls = 0;
    nvpcf_log_n = 0;
    nvpcf_dynamic_status = NV_ERR_INVALID_STATE;
    expect(rm_nvpcf_resample_power_budget(&gpu, &nv) == NV_TRUE, "dynamic fail wakes");
    expect(nvpcf_control_calls == 1, "static not reread");
    expect(nvpcf_last_sub == NVPCF0100_CTRL_CONFIG_DSM_2X_FUNC_GET_DYNAMIC_CASE,
           "dynamic subfunc");
    expect(nvpcf_last_ver == 0x25, "dynamic uses cached version");
    expect(log_has("dynamic budget read failed"), "dynamic log");
    expect(rm_nvpcf_haveLastBudget == NV_TRUE, "last budget kept");
    nvpcf_log_n = 0;
    expect(rm_nvpcf_resample_power_budget(&gpu, &nv) == NV_TRUE, "dynamic fail again");
    expect(nvpcf_log_n == 0, "dynamic log once");
}

static void test_coalesce_then_change(void)
{
    nv_state_t nv;
    OBJGPU gpu;

    memset(&nv, 0, sizeof(nv));
    memset(&gpu, 0, sizeof(gpu));
    nv.rmapi.hClient = 0x11;
    gpu.gpuId = 7;
    rm_nvpcf_drop_cached_budget();
    nvpcf_static_status = NV_OK;
    nvpcf_dynamic_status = NV_OK;
    nvpcf_static_version = 0x25;
    nvpcf_dynamic = sample();
    expect(rm_nvpcf_resample_power_budget(&gpu, &nv) == NV_TRUE, "prime");

    nvpcf_log_n = 0;
    expect(rm_nvpcf_resample_power_budget(&gpu, &nv) == NV_FALSE, "duplicate suppressed");
    expect(rm_nvpcf_coalesced == 1, "one coalesce");
    expect(log_has("coalesced; budget unchanged"), "coalesce log");
    nvpcf_log_n = 0;
    expect(rm_nvpcf_resample_power_budget(&gpu, &nv) == NV_FALSE, "second duplicate");
    expect(rm_nvpcf_coalesced == 2, "two coalesced");
    expect(nvpcf_log_n == 0, "coalesce logs once");

    nvpcf_dynamic.ctgpOffsetmW = 0;
    nvpcf_log_n = 0;
    expect(rm_nvpcf_resample_power_budget(&gpu, &nv) == NV_TRUE, "change wakes");
    expect(log_has("budget changed after"), "change log");
    expect(rm_nvpcf_coalesced == 0, "coalesce cleared");
    expect(rm_nvpcf_lastBudget.ctgpOffsetmW == 0, "cache updated");
}

static void test_drop_clears_cache_not_logs(void)
{
    nv_state_t nv;
    OBJGPU gpu;

    memset(&nv, 0, sizeof(nv));
    memset(&gpu, 0, sizeof(gpu));
    nv.rmapi.hClient = 0x11;
    gpu.gpuId = 9;
    nvpcf_dynamic = sample();
    nvpcf_dynamic.gpuId = 9;
    expect(rm_nvpcf_resample_power_budget(&gpu, &nv) == NV_TRUE, "sample before drop");
    rm_nvpcf_drop_cached_budget();
    expect(rm_nvpcf_haveLastBudget == NV_FALSE, "drop last");
    expect(rm_nvpcf_haveVersion == NV_FALSE, "drop version flag");
    expect(rm_nvpcf_version == 0, "drop version");
    expect(rm_nvpcf_coalesced == 0, "drop coalesce");
    expect(rm_nvpcf_lastBudget.ctgpOffsetmW == 0, "drop budget");
    expect(rm_nvpcf_loggedNoClient == NV_TRUE, "log flag survives drop");
    expect(rm_nvpcf_loggedStaticFailure == NV_TRUE, "static flag survives");
    expect(rm_nvpcf_loggedDynamicFailure == NV_TRUE, "dynamic flag survives");

    nvpcf_control_calls = 0;
    expect(rm_nvpcf_resample_power_budget(&gpu, &nv) == NV_TRUE,
           "same numbers wake after drop");
    expect(nvpcf_control_calls == 2, "static reread after drop");
}

static void test_notify_paths(void)
{
    OBJGPU gpu;
    nvidia_stack_t stack;

    memset(&gpu, 0, sizeof(gpu));
    gpu.gpuId = 7;
    gpu.pOsGpuInfo = NULL;
    nvpcf_gpu = &gpu;
    nvpcf_api_lock_status = NV_ERR_INVALID_STATE;
    nvpcf_static_status = NV_OK;
    nvpcf_dynamic_status = NV_OK;
    nvpcf_static_version = 0x25;
    nvpcf_dynamic = sample();
    nvpcf_trace_n = 0;
    nvpcf_enter_calls = 0;
    nvpcf_exit_calls = 0;
    rm_acpi_nvpcf_notify(NULL);
    expect(nvpcf_enter_calls == 1 && nvpcf_exit_calls == 1, "null stack still enters");
    expect(nvpcf_enter_sp == NULL, "null stack pointer");
    {
        const char *want[] = {"api_lock"};
        expect(trace_is(want, 1), "api lock fail");
    }
    expect(nvpcf_notify_calls == 0, "no notify without api lock");

    nvpcf_api_lock_status = NV_OK;
    nvpcf_gpu = NULL;
    nvpcf_trace_n = 0;
    nvpcf_notify_calls = 0;
    rm_acpi_nvpcf_notify(&stack);
    {
        const char *want[] = {"api_lock", "get_gpu", "api_unlock"};
        expect(trace_is(want, 3), "null gpu");
    }
    expect(nvpcf_notify_calls == 0, "null gpu does not notify");

    {
        static nv_state_t state;
        memset(&state, 0, sizeof(state));
        gpu.pOsGpuInfo = &state;
    }
    nvpcf_gpu = &gpu;
    nvpcf_gpu_lock_status = NV_ERR_INVALID_STATE;
    nvpcf_ref_status = NV_OK;
    nvpcf_trace_n = 0;
    nvpcf_notify_calls = 0;
    rm_acpi_nvpcf_notify(&stack);
    {
        const char *want[] = {
            "api_lock", "get_gpu", "gpu_lock", "ref", "notify", "unref", "api_unlock"
        };
        expect(trace_is(want, 7), "gpu lock fail still wakes");
    }
    expect(nvpcf_notify_id == NV2080_NOTIFIERS_NVPCF_EVENTS, "notifier id");
    expect(nvpcf_gpu_locked == 0 && nvpcf_api_locked == 0, "locks released");

    nvpcf_ref_status = NV_ERR_INVALID_STATE;
    nvpcf_trace_n = 0;
    nvpcf_notify_calls = 0;
    rm_acpi_nvpcf_notify(&stack);
    {
        const char *want[] = {"api_lock", "get_gpu", "gpu_lock", "ref", "api_unlock"};
        expect(trace_is(want, 5), "ref fail skips notify");
    }
    expect(nvpcf_notify_calls == 0, "no notify on ref fail");

    nvpcf_gpu_lock_status = NV_OK;
    nvpcf_ref_status = NV_OK;
    ((nv_state_t *)gpu.pOsGpuInfo)->rmapi.hClient = 0x11;
    nvpcf_dynamic = sample();
    rm_nvpcf_drop_cached_budget();
    nvpcf_trace_n = 0;
    nvpcf_notify_calls = 0;
    rm_acpi_nvpcf_notify(&stack);
    {
        const char *want[] = {
            "api_lock", "get_gpu", "gpu_lock", "gpu_unlock", "ref", "notify",
            "unref", "api_unlock"
        };
        expect(trace_is(want, 8), "unlock before ref");
    }
    expect(nvpcf_notify_calls == 1, "changed budget notifies");

    nvpcf_trace_n = 0;
    nvpcf_notify_calls = 0;
    rm_acpi_nvpcf_notify(&stack);
    {
        const char *want[] = {
            "api_lock", "get_gpu", "gpu_lock", "gpu_unlock", "api_unlock"
        };
        expect(trace_is(want, 5), "duplicate skips ref");
    }
    expect(nvpcf_notify_calls == 0, "duplicate does not notify");

    nvpcf_ref_status = NV_ERR_INVALID_STATE;
    nvpcf_dynamic.ctgpOffsetmW = 1;
    nvpcf_trace_n = 0;
    nvpcf_notify_calls = 0;
    rm_acpi_nvpcf_notify(&stack);
    {
        const char *want[] = {
            "api_lock", "get_gpu", "gpu_lock", "gpu_unlock", "ref", "api_unlock"
        };
        expect(trace_is(want, 6), "changed budget ref fail");
    }
    expect(nvpcf_notify_calls == 0, "ref fail after change");
}

int main(void)
{
    test_same_budget();
    test_control_rejects_missing_client();
    test_no_client_wakes_and_logs_once();
    test_static_failure_does_not_cache();
    test_dynamic_failure_keeps_last_budget();
    test_coalesce_then_change();
    test_drop_clears_cache_not_logs();
    test_notify_paths();
    if (failures)
    {
        fprintf(stderr, "%d os nvpcf tests failed\n", failures);
        return 1;
    }
    printf("os nvpcf: all tests passed\n");
    return 0;
}
