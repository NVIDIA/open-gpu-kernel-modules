/* SPDX-License-Identifier: MIT */
/* Boundary double for the real AllocateDevice/FreeDevice function bodies.
 * Minimal types model only their accessed fields. No driver or GPU is loaded.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef uint32_t NvU32;
typedef int NvBool;
#define NV_TRUE 1
#define NV_FALSE 0
#define nvKmsKapiLogDebug(...) ((void)0)
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "CHECK failed at line %d: %s\n", __LINE__, #x); exit(1); } } while (0)
struct sema { int unused; };
struct NvKmsKapiDevice {
    struct sema *pSema;
    NvU32 gpuId, migDevice;
    void *privateData;
    void (*eventCallback)(void);
};
struct NvKmsKapiAllocateDeviceParams {
    NvU32 gpuId, migDevice;
    void *privateData;
    void (*eventCallback)(void);
};
enum fault { NONE, DEVICE_ALLOC, SEMA_ALLOC, GPU_OPEN, RM_ALLOC, KMS_ALLOC };
static enum fault fault;
static unsigned allocations, frees, semas, sema_frees;
static unsigned opens, closes, rm_frees, kms_frees;
static int gpu_live;
static void *nvKmsKapiCalloc(size_t n, size_t size)
{
    void *p;
    if (fault == DEVICE_ALLOC)
        return NULL;
    p = calloc(n, size);
    CHECK(p);
    allocations++;
    return p;
}
static void nvKmsKapiFree(void *p) { if (p) { frees++; free(p); } }
static struct sema *nvkms_sema_alloc(void)
{
    struct sema *s;
    if (fault == SEMA_ALLOC)
        return NULL;
    s = calloc(1, sizeof(*s));
    CHECK(s);
    semas++;
    return s;
}
static void nvkms_sema_free(struct sema *s)
{
    CHECK(s);
    sema_frees++;
    free(s);
}
static NvBool nvkms_open_gpu(NvU32 gpu, NvBool reset_aware)
{
    CHECK(gpu == 77 && reset_aware);
    opens++;
    if (fault == GPU_OPEN)
        return NV_FALSE;
    CHECK(!gpu_live);
    gpu_live = 1;
    return NV_TRUE;
}
static void nvkms_close_gpu(NvU32 gpu, NvBool reset_aware)
{
    if (!gpu_live) {
        fprintf(stderr, "CLOSE_WITHOUT_OPEN: gpu=%u, successful_opens=0\n", gpu);
        exit(1);
    }
    CHECK(gpu == 77 && reset_aware);
    gpu_live = 0;
    closes++;
}
static NvBool RmAllocateDevice(struct NvKmsKapiDevice *d)
{
    CHECK(d && gpu_live);
    return fault != RM_ALLOC;
}
static NvBool KmsAllocateDevice(struct NvKmsKapiDevice *d)
{
    CHECK(d && gpu_live);
    return fault != KMS_ALLOC;
}
/* Teardown boundaries intentionally tolerate NULL, so the NULL diagnostic
 * comes from upstream FreeDevice's own device->gpuId access, not our double.
 * Actual upstream KmsFreeDevice can dereference the NULL pointer even earlier.
 */
static void KmsFreeDevice(struct NvKmsKapiDevice *d) { (void)d; kms_frees++; }
static void RmFreeDevice(struct NvKmsKapiDevice *d) { (void)d; rm_frees++; }
/* ACTUAL_FREE_DEVICE */
/* ACTUAL_ALLOCATE_DEVICE */
int main(int argc, char **argv)
{
    struct NvKmsKapiAllocateDeviceParams params = { .gpuId = 77, .migDevice = 3 };
    struct NvKmsKapiDevice *d;
    const char *names[] = {"success", "device-alloc", "sema-alloc", "gpu-open", "rm-alloc", "kms-alloc"};
    unsigned i;
    CHECK(argc == 2);
    if (!strcmp(argv[1], "free-null")) {
        FreeDevice(NULL);
        CHECK(!closes && !rm_frees && !kms_frees && !frees && !sema_frees);
        puts("PASS: free-null");
        return 0;
    }
    for (i = 0; i < sizeof(names)/sizeof(names[0]); i++)
        if (!strcmp(argv[1], names[i]))
            break;
    CHECK(i < sizeof(names)/sizeof(names[0]));
    fault = i;
    d = AllocateDevice(&params);
    if (fault == NONE) {
        CHECK(d && d->gpuId == 77 && d->migDevice == 3 && gpu_live);
        FreeDevice(d);
    } else {
        CHECK(!d);
    }
    CHECK(allocations == frees && semas == sema_frees && !gpu_live);
    CHECK(opens == (fault != DEVICE_ALLOC && fault != SEMA_ALLOC));
    CHECK(closes == (fault == NONE || fault == RM_ALLOC || fault == KMS_ALLOC));
    CHECK(rm_frees == closes && kms_frees == closes);
    printf("PASS: %s; device=%u/%u semaphore=%u/%u GPU-close=%u\n",
           argv[1], allocations, frees, semas, sema_frees, closes);
    return 0;
}
