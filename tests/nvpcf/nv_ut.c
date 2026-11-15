#define NVPCF_HOST_TEST 1
#include "nv.c"

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

int main(void)
{
    nv_stack_t stack;
    struct device dev;
    int i;
    const char *want[] = {
        "state_exit", "cancel", "shutdown", "rsync", "nvlink", "cap", "resources"
    };

    nv_module_exit(&stack);
    expect(nvpcf_order_n == 7, "exit call count");
    for (i = 0; i < 7; i++)
        expect(nvpcf_order_n > i && strcmp(nvpcf_order[i], want[i]) == 0, want[i]);
    expect(strcmp(nvpcf_order[1], "cancel") == 0 &&
               strcmp(nvpcf_order[2], "shutdown") == 0,
           "cancel before rm shutdown");

    nvpcf_order_n = 0;
    nvpcf_drop_calls = 0;
    expect(nvidia_suspend(&dev, 0, 0) == NV_OK, "suspend returns");
    expect(nvpcf_drop_calls == 1, "suspend drops the cache");
    expect(nvpcf_order_n == 1 && strcmp(nvpcf_order[0], "drop") == 0,
           "suspend drop is the tested call");

    if (failures)
    {
        fprintf(stderr, "%d nv exit tests failed\n", failures);
        return 1;
    }
    printf("nv exit: all tests passed\n");
    return 0;
}
