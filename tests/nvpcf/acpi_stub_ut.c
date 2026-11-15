#define NVPCF_HOST_TEST 1
#include "nv-acpi.c"

int main(void)
{
    nv_acpi_nvpcf_work_cancel();
    return 0;
}
