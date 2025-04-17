#define NVOC_KERNEL_FALCON_H_PRIVATE_ACCESS_ALLOWED

// Version of generated metadata structures
#ifdef NVOC_METADATA_VERSION
#undef NVOC_METADATA_VERSION
#endif
#define NVOC_METADATA_VERSION 2

#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_kernel_falcon_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0xb6b1af = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelFalcon;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_CrashCatEngine;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelCrashCatEngine;

// Forward declarations for KernelFalcon
void __nvoc_init__KernelCrashCatEngine(KernelCrashCatEngine*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init__KernelFalcon(KernelFalcon*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_funcTable_KernelFalcon(KernelFalcon*, RmHalspecOwner *pRmhalspecowner);
NV_STATUS __nvoc_ctor_KernelFalcon(KernelFalcon*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_dataField_KernelFalcon(KernelFalcon*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_dtor_KernelFalcon(KernelFalcon*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__KernelFalcon;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelFalcon;

// Down-thunk(s) to bridge KernelFalcon methods from ancestors (if any)
NvBool __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineConfigured(struct CrashCatEngine *arg_this);    // super
void __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineUnload(struct CrashCatEngine *arg_this);    // super
void __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineVprintf(struct CrashCatEngine *arg_this, NvBool bReportStart, const char *fmt, va_list args);    // super
NvU32 __nvoc_down_thunk_KernelCrashCatEngine_crashcatEnginePriRead(struct CrashCatEngine *arg_this, NvU32 offset);    // super
void __nvoc_down_thunk_KernelCrashCatEngine_crashcatEnginePriWrite(struct CrashCatEngine *arg_this, NvU32 offset, NvU32 data);    // super
void * __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineMapBufferDescriptor(struct CrashCatEngine *arg_this, CrashCatBufferDescriptor *pBufDesc);    // super
void __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineUnmapBufferDescriptor(struct CrashCatEngine *arg_this, CrashCatBufferDescriptor *pBufDesc);    // super
void __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineSyncBufferDescriptor(struct CrashCatEngine *arg_this, CrashCatBufferDescriptor *pBufDesc, NvU32 offset, NvU32 size);    // super
const NvU32 * __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineGetScratchOffsets(struct CrashCatEngine *arg_this, NV_CRASHCAT_SCRATCH_GROUP_ID scratchGroupId);    // super
NvU32 __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineGetWFL0Offset(struct CrashCatEngine *arg_this);    // super
NvU32 __nvoc_down_thunk_KernelFalcon_kcrashcatEngineRegRead(struct OBJGPU *pGpu, struct KernelCrashCatEngine *pKernelFlcn, NvU32 offset);    // this
void __nvoc_down_thunk_KernelFalcon_kcrashcatEngineRegWrite(struct OBJGPU *pGpu, struct KernelCrashCatEngine *pKernelFlcn, NvU32 offset, NvU32 data);    // this
NvU32 __nvoc_down_thunk_KernelFalcon_kcrashcatEngineMaskDmemAddr(struct OBJGPU *pGpu, struct KernelCrashCatEngine *pKernelFlcn, NvU32 addr);    // this

// Up-thunk(s) to bridge KernelFalcon methods to ancestors (if any)
NvBool __nvoc_up_thunk_KernelCrashCatEngine_kflcnConfigured(struct KernelFalcon *arg_this);    // this
void __nvoc_up_thunk_KernelCrashCatEngine_kflcnUnload(struct KernelFalcon *arg_this);    // this
void __nvoc_up_thunk_KernelCrashCatEngine_kflcnVprintf(struct KernelFalcon *arg_this, NvBool bReportStart, const char *fmt, va_list args);    // this
NvU32 __nvoc_up_thunk_KernelCrashCatEngine_kflcnPriRead(struct KernelFalcon *arg_this, NvU32 offset);    // this
void __nvoc_up_thunk_KernelCrashCatEngine_kflcnPriWrite(struct KernelFalcon *arg_this, NvU32 offset, NvU32 data);    // this
void * __nvoc_up_thunk_KernelCrashCatEngine_kflcnMapBufferDescriptor(struct KernelFalcon *arg_this, CrashCatBufferDescriptor *pBufDesc);    // this
void __nvoc_up_thunk_KernelCrashCatEngine_kflcnUnmapBufferDescriptor(struct KernelFalcon *arg_this, CrashCatBufferDescriptor *pBufDesc);    // this
void __nvoc_up_thunk_KernelCrashCatEngine_kflcnSyncBufferDescriptor(struct KernelFalcon *arg_this, CrashCatBufferDescriptor *pBufDesc, NvU32 offset, NvU32 size);    // this
void __nvoc_up_thunk_KernelCrashCatEngine_kflcnReadDmem(struct KernelFalcon *arg_this, NvU32 offset, NvU32 size, void *pBuf);    // this
void __nvoc_up_thunk_KernelCrashCatEngine_kflcnReadEmem(struct KernelFalcon *arg_this, NvU64 offset, NvU64 size, void *pBuf);    // this
const NvU32 * __nvoc_up_thunk_KernelCrashCatEngine_kflcnGetScratchOffsets(struct KernelFalcon *arg_this, NV_CRASHCAT_SCRATCH_GROUP_ID scratchGroupId);    // this
NvU32 __nvoc_up_thunk_KernelCrashCatEngine_kflcnGetWFL0Offset(struct KernelFalcon *arg_this);    // this

// Not instantiable because it's not derived from class "Object"
// Not instantiable because it's an abstract class with following pure virtual functions:
//  kflcnResetHw
const struct NVOC_CLASS_DEF __nvoc_class_def_KernelFalcon = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(KernelFalcon),
        /*classId=*/            classId(KernelFalcon),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "KernelFalcon",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) NULL,
    /*pCastInfo=*/          &__nvoc_castinfo__KernelFalcon,
    /*pExportInfo=*/        &__nvoc_export_info__KernelFalcon
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__KernelFalcon __nvoc_metadata__KernelFalcon = {
    .rtti.pClassDef = &__nvoc_class_def_KernelFalcon,    // (kflcn) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelFalcon,
    .rtti.offset    = 0,
    .metadata__KernelCrashCatEngine.rtti.pClassDef = &__nvoc_class_def_KernelCrashCatEngine,    // (kcrashcatEngine) super
    .metadata__KernelCrashCatEngine.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__KernelCrashCatEngine.rtti.offset    = NV_OFFSETOF(KernelFalcon, __nvoc_base_KernelCrashCatEngine),
    .metadata__KernelCrashCatEngine.metadata__CrashCatEngine.rtti.pClassDef = &__nvoc_class_def_CrashCatEngine,    // (crashcatEngine) super^2
    .metadata__KernelCrashCatEngine.metadata__CrashCatEngine.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__KernelCrashCatEngine.metadata__CrashCatEngine.rtti.offset    = NV_OFFSETOF(KernelFalcon, __nvoc_base_KernelCrashCatEngine.__nvoc_base_CrashCatEngine),

    .metadata__KernelCrashCatEngine.vtable.__kcrashcatEngineRegRead__ = &__nvoc_down_thunk_KernelFalcon_kcrashcatEngineRegRead,    // pure virtual
    .metadata__KernelCrashCatEngine.vtable.__kcrashcatEngineRegWrite__ = &__nvoc_down_thunk_KernelFalcon_kcrashcatEngineRegWrite,    // pure virtual
    .vtable.__kflcnResetHw__ = NULL,    // pure virtual
    .metadata__KernelCrashCatEngine.vtable.__kcrashcatEngineMaskDmemAddr__ = &__nvoc_down_thunk_KernelFalcon_kcrashcatEngineMaskDmemAddr,    // pure virtual
    .vtable.__kflcnConfigured__ = &__nvoc_up_thunk_KernelCrashCatEngine_kflcnConfigured,    // virtual inherited (kcrashcatEngine) base (kcrashcatEngine)
    .metadata__KernelCrashCatEngine.vtable.__kcrashcatEngineConfigured__ = &kcrashcatEngineConfigured_IMPL,    // virtual override (crashcatEngine) base (crashcatEngine)
    .metadata__KernelCrashCatEngine.metadata__CrashCatEngine.vtable.__crashcatEngineConfigured__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineConfigured,    // pure virtual
    .vtable.__kflcnUnload__ = &__nvoc_up_thunk_KernelCrashCatEngine_kflcnUnload,    // virtual inherited (kcrashcatEngine) base (kcrashcatEngine)
    .metadata__KernelCrashCatEngine.vtable.__kcrashcatEngineUnload__ = &kcrashcatEngineUnload_IMPL,    // virtual override (crashcatEngine) base (crashcatEngine)
    .metadata__KernelCrashCatEngine.metadata__CrashCatEngine.vtable.__crashcatEngineUnload__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineUnload,    // virtual
    .vtable.__kflcnVprintf__ = &__nvoc_up_thunk_KernelCrashCatEngine_kflcnVprintf,    // virtual inherited (kcrashcatEngine) base (kcrashcatEngine)
    .metadata__KernelCrashCatEngine.vtable.__kcrashcatEngineVprintf__ = &kcrashcatEngineVprintf_IMPL,    // virtual override (crashcatEngine) base (crashcatEngine)
    .metadata__KernelCrashCatEngine.metadata__CrashCatEngine.vtable.__crashcatEngineVprintf__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineVprintf,    // pure virtual
    .vtable.__kflcnPriRead__ = &__nvoc_up_thunk_KernelCrashCatEngine_kflcnPriRead,    // virtual inherited (kcrashcatEngine) base (kcrashcatEngine)
    .metadata__KernelCrashCatEngine.vtable.__kcrashcatEnginePriRead__ = &kcrashcatEnginePriRead_IMPL,    // virtual override (crashcatEngine) base (crashcatEngine)
    .metadata__KernelCrashCatEngine.metadata__CrashCatEngine.vtable.__crashcatEnginePriRead__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEnginePriRead,    // pure virtual
    .vtable.__kflcnPriWrite__ = &__nvoc_up_thunk_KernelCrashCatEngine_kflcnPriWrite,    // virtual inherited (kcrashcatEngine) base (kcrashcatEngine)
    .metadata__KernelCrashCatEngine.vtable.__kcrashcatEnginePriWrite__ = &kcrashcatEnginePriWrite_IMPL,    // virtual override (crashcatEngine) base (crashcatEngine)
    .metadata__KernelCrashCatEngine.metadata__CrashCatEngine.vtable.__crashcatEnginePriWrite__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEnginePriWrite,    // pure virtual
    .vtable.__kflcnMapBufferDescriptor__ = &__nvoc_up_thunk_KernelCrashCatEngine_kflcnMapBufferDescriptor,    // virtual inherited (kcrashcatEngine) base (kcrashcatEngine)
    .metadata__KernelCrashCatEngine.vtable.__kcrashcatEngineMapBufferDescriptor__ = &kcrashcatEngineMapBufferDescriptor_IMPL,    // virtual override (crashcatEngine) base (crashcatEngine)
    .metadata__KernelCrashCatEngine.metadata__CrashCatEngine.vtable.__crashcatEngineMapBufferDescriptor__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineMapBufferDescriptor,    // pure virtual
    .vtable.__kflcnUnmapBufferDescriptor__ = &__nvoc_up_thunk_KernelCrashCatEngine_kflcnUnmapBufferDescriptor,    // virtual inherited (kcrashcatEngine) base (kcrashcatEngine)
    .metadata__KernelCrashCatEngine.vtable.__kcrashcatEngineUnmapBufferDescriptor__ = &kcrashcatEngineUnmapBufferDescriptor_IMPL,    // virtual override (crashcatEngine) base (crashcatEngine)
    .metadata__KernelCrashCatEngine.metadata__CrashCatEngine.vtable.__crashcatEngineUnmapBufferDescriptor__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineUnmapBufferDescriptor,    // pure virtual
    .vtable.__kflcnSyncBufferDescriptor__ = &__nvoc_up_thunk_KernelCrashCatEngine_kflcnSyncBufferDescriptor,    // virtual inherited (kcrashcatEngine) base (kcrashcatEngine)
    .metadata__KernelCrashCatEngine.vtable.__kcrashcatEngineSyncBufferDescriptor__ = &kcrashcatEngineSyncBufferDescriptor_IMPL,    // virtual override (crashcatEngine) base (crashcatEngine)
    .metadata__KernelCrashCatEngine.metadata__CrashCatEngine.vtable.__crashcatEngineSyncBufferDescriptor__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineSyncBufferDescriptor,    // pure virtual
    .vtable.__kflcnReadDmem__ = &__nvoc_up_thunk_KernelCrashCatEngine_kflcnReadDmem,    // virtual halified (singleton optimized) inherited (kcrashcatEngine) base (kcrashcatEngine)
    .metadata__KernelCrashCatEngine.vtable.__kcrashcatEngineReadDmem__ = &kcrashcatEngineReadDmem_TU102,    // virtual halified (singleton optimized)
    .vtable.__kflcnReadEmem__ = &__nvoc_up_thunk_KernelCrashCatEngine_kflcnReadEmem,    // virtual halified (singleton optimized) inherited (kcrashcatEngine) base (kcrashcatEngine)
    .metadata__KernelCrashCatEngine.vtable.__kcrashcatEngineReadEmem__ = &kcrashcatEngineReadEmem_2fced3,    // virtual halified (singleton optimized)
    .vtable.__kflcnGetScratchOffsets__ = &__nvoc_up_thunk_KernelCrashCatEngine_kflcnGetScratchOffsets,    // virtual halified (singleton optimized) inherited (kcrashcatEngine) base (kcrashcatEngine)
    .metadata__KernelCrashCatEngine.vtable.__kcrashcatEngineGetScratchOffsets__ = &kcrashcatEngineGetScratchOffsets_TU102,    // virtual halified (singleton optimized) override (crashcatEngine) base (crashcatEngine)
    .metadata__KernelCrashCatEngine.metadata__CrashCatEngine.vtable.__crashcatEngineGetScratchOffsets__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineGetScratchOffsets,    // pure virtual
    .vtable.__kflcnGetWFL0Offset__ = &__nvoc_up_thunk_KernelCrashCatEngine_kflcnGetWFL0Offset,    // virtual halified (singleton optimized) inherited (kcrashcatEngine) base (kcrashcatEngine)
    .metadata__KernelCrashCatEngine.vtable.__kcrashcatEngineGetWFL0Offset__ = &kcrashcatEngineGetWFL0Offset_TU102,    // virtual halified (singleton optimized) override (crashcatEngine) base (crashcatEngine)
    .metadata__KernelCrashCatEngine.metadata__CrashCatEngine.vtable.__crashcatEngineGetWFL0Offset__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineGetWFL0Offset,    // pure virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__KernelFalcon = {
    .numRelatives = 3,
    .relatives = {
        &__nvoc_metadata__KernelFalcon.rtti,    // [0]: (kflcn) this
        &__nvoc_metadata__KernelFalcon.metadata__KernelCrashCatEngine.rtti,    // [1]: (kcrashcatEngine) super
        &__nvoc_metadata__KernelFalcon.metadata__KernelCrashCatEngine.metadata__CrashCatEngine.rtti,    // [2]: (crashcatEngine) super^2
    }
};

// 3 down-thunk(s) defined to bridge methods in KernelFalcon from superclasses

// kflcnRegRead: virtual halified (2 hals) override (kcrashcatEngine) base (kcrashcatEngine) body
NvU32 __nvoc_down_thunk_KernelFalcon_kcrashcatEngineRegRead(struct OBJGPU *pGpu, struct KernelCrashCatEngine *pKernelFlcn, NvU32 offset) {
    return kflcnRegRead(pGpu, (struct KernelFalcon *)(((unsigned char *) pKernelFlcn) - NV_OFFSETOF(KernelFalcon, __nvoc_base_KernelCrashCatEngine)), offset);
}

// kflcnRegWrite: virtual halified (2 hals) override (kcrashcatEngine) base (kcrashcatEngine) body
void __nvoc_down_thunk_KernelFalcon_kcrashcatEngineRegWrite(struct OBJGPU *pGpu, struct KernelCrashCatEngine *pKernelFlcn, NvU32 offset, NvU32 data) {
    kflcnRegWrite(pGpu, (struct KernelFalcon *)(((unsigned char *) pKernelFlcn) - NV_OFFSETOF(KernelFalcon, __nvoc_base_KernelCrashCatEngine)), offset, data);
}

// kflcnMaskDmemAddr: virtual halified (3 hals) override (kcrashcatEngine) base (kcrashcatEngine) body
NvU32 __nvoc_down_thunk_KernelFalcon_kcrashcatEngineMaskDmemAddr(struct OBJGPU *pGpu, struct KernelCrashCatEngine *pKernelFlcn, NvU32 addr) {
    return kflcnMaskDmemAddr(pGpu, (struct KernelFalcon *)(((unsigned char *) pKernelFlcn) - NV_OFFSETOF(KernelFalcon, __nvoc_base_KernelCrashCatEngine)), addr);
}


// 12 up-thunk(s) defined to bridge methods in KernelFalcon to superclasses

// kflcnConfigured: virtual inherited (kcrashcatEngine) base (kcrashcatEngine)
NvBool __nvoc_up_thunk_KernelCrashCatEngine_kflcnConfigured(struct KernelFalcon *arg_this) {
    return kcrashcatEngineConfigured((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) + NV_OFFSETOF(KernelFalcon, __nvoc_base_KernelCrashCatEngine)));
}

// kflcnUnload: virtual inherited (kcrashcatEngine) base (kcrashcatEngine)
void __nvoc_up_thunk_KernelCrashCatEngine_kflcnUnload(struct KernelFalcon *arg_this) {
    kcrashcatEngineUnload((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) + NV_OFFSETOF(KernelFalcon, __nvoc_base_KernelCrashCatEngine)));
}

// kflcnVprintf: virtual inherited (kcrashcatEngine) base (kcrashcatEngine)
void __nvoc_up_thunk_KernelCrashCatEngine_kflcnVprintf(struct KernelFalcon *arg_this, NvBool bReportStart, const char *fmt, va_list args) {
    kcrashcatEngineVprintf((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) + NV_OFFSETOF(KernelFalcon, __nvoc_base_KernelCrashCatEngine)), bReportStart, fmt, args);
}

// kflcnPriRead: virtual inherited (kcrashcatEngine) base (kcrashcatEngine)
NvU32 __nvoc_up_thunk_KernelCrashCatEngine_kflcnPriRead(struct KernelFalcon *arg_this, NvU32 offset) {
    return kcrashcatEnginePriRead((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) + NV_OFFSETOF(KernelFalcon, __nvoc_base_KernelCrashCatEngine)), offset);
}

// kflcnPriWrite: virtual inherited (kcrashcatEngine) base (kcrashcatEngine)
void __nvoc_up_thunk_KernelCrashCatEngine_kflcnPriWrite(struct KernelFalcon *arg_this, NvU32 offset, NvU32 data) {
    kcrashcatEnginePriWrite((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) + NV_OFFSETOF(KernelFalcon, __nvoc_base_KernelCrashCatEngine)), offset, data);
}

// kflcnMapBufferDescriptor: virtual inherited (kcrashcatEngine) base (kcrashcatEngine)
void * __nvoc_up_thunk_KernelCrashCatEngine_kflcnMapBufferDescriptor(struct KernelFalcon *arg_this, CrashCatBufferDescriptor *pBufDesc) {
    return kcrashcatEngineMapBufferDescriptor((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) + NV_OFFSETOF(KernelFalcon, __nvoc_base_KernelCrashCatEngine)), pBufDesc);
}

// kflcnUnmapBufferDescriptor: virtual inherited (kcrashcatEngine) base (kcrashcatEngine)
void __nvoc_up_thunk_KernelCrashCatEngine_kflcnUnmapBufferDescriptor(struct KernelFalcon *arg_this, CrashCatBufferDescriptor *pBufDesc) {
    kcrashcatEngineUnmapBufferDescriptor((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) + NV_OFFSETOF(KernelFalcon, __nvoc_base_KernelCrashCatEngine)), pBufDesc);
}

// kflcnSyncBufferDescriptor: virtual inherited (kcrashcatEngine) base (kcrashcatEngine)
void __nvoc_up_thunk_KernelCrashCatEngine_kflcnSyncBufferDescriptor(struct KernelFalcon *arg_this, CrashCatBufferDescriptor *pBufDesc, NvU32 offset, NvU32 size) {
    kcrashcatEngineSyncBufferDescriptor((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) + NV_OFFSETOF(KernelFalcon, __nvoc_base_KernelCrashCatEngine)), pBufDesc, offset, size);
}

// kflcnReadDmem: virtual halified (singleton optimized) inherited (kcrashcatEngine) base (kcrashcatEngine)
void __nvoc_up_thunk_KernelCrashCatEngine_kflcnReadDmem(struct KernelFalcon *arg_this, NvU32 offset, NvU32 size, void *pBuf) {
    kcrashcatEngineReadDmem((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) + NV_OFFSETOF(KernelFalcon, __nvoc_base_KernelCrashCatEngine)), offset, size, pBuf);
}

// kflcnReadEmem: virtual halified (singleton optimized) inherited (kcrashcatEngine) base (kcrashcatEngine)
void __nvoc_up_thunk_KernelCrashCatEngine_kflcnReadEmem(struct KernelFalcon *arg_this, NvU64 offset, NvU64 size, void *pBuf) {
    kcrashcatEngineReadEmem((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) + NV_OFFSETOF(KernelFalcon, __nvoc_base_KernelCrashCatEngine)), offset, size, pBuf);
}

// kflcnGetScratchOffsets: virtual halified (singleton optimized) inherited (kcrashcatEngine) base (kcrashcatEngine)
const NvU32 * __nvoc_up_thunk_KernelCrashCatEngine_kflcnGetScratchOffsets(struct KernelFalcon *arg_this, NV_CRASHCAT_SCRATCH_GROUP_ID scratchGroupId) {
    return kcrashcatEngineGetScratchOffsets((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) + NV_OFFSETOF(KernelFalcon, __nvoc_base_KernelCrashCatEngine)), scratchGroupId);
}

// kflcnGetWFL0Offset: virtual halified (singleton optimized) inherited (kcrashcatEngine) base (kcrashcatEngine)
NvU32 __nvoc_up_thunk_KernelCrashCatEngine_kflcnGetWFL0Offset(struct KernelFalcon *arg_this) {
    return kcrashcatEngineGetWFL0Offset((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) + NV_OFFSETOF(KernelFalcon, __nvoc_base_KernelCrashCatEngine)));
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelFalcon = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_KernelCrashCatEngine(KernelCrashCatEngine*);
void __nvoc_dtor_KernelFalcon(KernelFalcon *pThis) {
    __nvoc_dtor_KernelCrashCatEngine(&pThis->__nvoc_base_KernelCrashCatEngine);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_KernelFalcon(KernelFalcon *pThis, RmHalspecOwner *pRmhalspecowner) {
    ChipHal *chipHal = &pRmhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);

    pThis->riscvMode = ((NvTristate)2);
}

NV_STATUS __nvoc_ctor_KernelCrashCatEngine(KernelCrashCatEngine* , RmHalspecOwner* );
NV_STATUS __nvoc_ctor_KernelFalcon(KernelFalcon *pThis, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_KernelCrashCatEngine(&pThis->__nvoc_base_KernelCrashCatEngine, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_ctor_KernelFalcon_fail_KernelCrashCatEngine;
    __nvoc_init_dataField_KernelFalcon(pThis, pRmhalspecowner);
    goto __nvoc_ctor_KernelFalcon_exit; // Success

__nvoc_ctor_KernelFalcon_fail_KernelCrashCatEngine:
__nvoc_ctor_KernelFalcon_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_KernelFalcon_1(KernelFalcon *pThis, RmHalspecOwner *pRmhalspecowner) {
    ChipHal *chipHal = &pRmhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);

    // kflcnRegRead -- virtual halified (2 hals) override (kcrashcatEngine) base (kcrashcatEngine) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kflcnRegRead__ = &kflcnRegRead_474d46;
    }
    else
    {
        pThis->__kflcnRegRead__ = &kflcnRegRead_TU102;
    }

    // kflcnRegWrite -- virtual halified (2 hals) override (kcrashcatEngine) base (kcrashcatEngine) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kflcnRegWrite__ = &kflcnRegWrite_f2d351;
    }
    else
    {
        pThis->__kflcnRegWrite__ = &kflcnRegWrite_TU102;
    }

    // kflcnRiscvRegRead -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kflcnRiscvRegRead__ = &kflcnRiscvRegRead_474d46;
    }
    else
    {
        pThis->__kflcnRiscvRegRead__ = &kflcnRiscvRegRead_TU102;
    }

    // kflcnRiscvRegWrite -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kflcnRiscvRegWrite__ = &kflcnRiscvRegWrite_f2d351;
    }
    else
    {
        pThis->__kflcnRiscvRegWrite__ = &kflcnRiscvRegWrite_TU102;
    }

    // kflcnIsRiscvCpuEnabled -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kflcnIsRiscvCpuEnabled__ = &kflcnIsRiscvCpuEnabled_86b752;
    }
    else
    {
        pThis->__kflcnIsRiscvCpuEnabled__ = &kflcnIsRiscvCpuEnabled_TU102;
    }

    // kflcnIsRiscvActive -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kflcnIsRiscvActive__ = &kflcnIsRiscvActive_86b752;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
        {
            pThis->__kflcnIsRiscvActive__ = &kflcnIsRiscvActive_TU102;
        }
        else
        {
            pThis->__kflcnIsRiscvActive__ = &kflcnIsRiscvActive_GA102;
        }
    }

    // kflcnIsRiscvSelected -- halified (4 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kflcnIsRiscvSelected__ = &kflcnIsRiscvSelected_86b752;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
        {
            pThis->__kflcnIsRiscvSelected__ = &kflcnIsRiscvSelected_TU102;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
        {
            pThis->__kflcnIsRiscvSelected__ = &kflcnIsRiscvSelected_GA102;
        }
        else
        {
            pThis->__kflcnIsRiscvSelected__ = &kflcnIsRiscvSelected_e661f0;
        }
    }

    // kflcnRiscvProgramBcr -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kflcnRiscvProgramBcr__ = &kflcnRiscvProgramBcr_f2d351;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
        {
            pThis->__kflcnRiscvProgramBcr__ = &kflcnRiscvProgramBcr_f2d351;
        }
        else
        {
            pThis->__kflcnRiscvProgramBcr__ = &kflcnRiscvProgramBcr_GA102;
        }
    }

    // kflcnSwitchToFalcon -- halified (4 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kflcnSwitchToFalcon__ = &kflcnSwitchToFalcon_f2d351;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
        {
            pThis->__kflcnSwitchToFalcon__ = &kflcnSwitchToFalcon_TU102;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
        {
            pThis->__kflcnSwitchToFalcon__ = &kflcnSwitchToFalcon_GA102;
        }
        else
        {
            pThis->__kflcnSwitchToFalcon__ = &kflcnSwitchToFalcon_f2d351;
        }
    }

    // kflcnReset -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kflcnReset__ = &kflcnReset_5baef9;
    }
    else
    {
        pThis->__kflcnReset__ = &kflcnReset_TU102;
    }

    // kflcnResetIntoRiscv -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kflcnResetIntoRiscv__ = &kflcnResetIntoRiscv_5baef9;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
        {
            pThis->__kflcnResetIntoRiscv__ = &kflcnResetIntoRiscv_TU102;
        }
        else
        {
            pThis->__kflcnResetIntoRiscv__ = &kflcnResetIntoRiscv_GA102;
        }
    }

    // kflcnStartCpu -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kflcnStartCpu__ = &kflcnStartCpu_f2d351;
    }
    else
    {
        pThis->__kflcnStartCpu__ = &kflcnStartCpu_TU102;
    }

    // kflcnDisableCtxReq -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kflcnDisableCtxReq__ = &kflcnDisableCtxReq_f2d351;
    }
    else
    {
        pThis->__kflcnDisableCtxReq__ = &kflcnDisableCtxReq_TU102;
    }

    // kflcnPreResetWait -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kflcnPreResetWait__ = &kflcnPreResetWait_5baef9;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
        {
            pThis->__kflcnPreResetWait__ = &kflcnPreResetWait_GA102;
        }
        // default
        else
        {
            pThis->__kflcnPreResetWait__ = &kflcnPreResetWait_56cd7a;
        }
    }

    // kflcnWaitForResetToFinish -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kflcnWaitForResetToFinish__ = &kflcnWaitForResetToFinish_5baef9;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
        {
            pThis->__kflcnWaitForResetToFinish__ = &kflcnWaitForResetToFinish_TU102;
        }
        else
        {
            pThis->__kflcnWaitForResetToFinish__ = &kflcnWaitForResetToFinish_GA102;
        }
    }

    // kflcnWaitForHalt -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kflcnWaitForHalt__ = &kflcnWaitForHalt_5baef9;
    }
    else
    {
        pThis->__kflcnWaitForHalt__ = &kflcnWaitForHalt_TU102;
    }

    // kflcnWaitForHaltRiscv -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kflcnWaitForHaltRiscv__ = &kflcnWaitForHaltRiscv_5baef9;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
        {
            pThis->__kflcnWaitForHaltRiscv__ = &kflcnWaitForHaltRiscv_5baef9;
        }
        else
        {
            pThis->__kflcnWaitForHaltRiscv__ = &kflcnWaitForHaltRiscv_GA102;
        }
    }

    // kflcnReadIntrStatus -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kflcnReadIntrStatus__ = &kflcnReadIntrStatus_474d46;
    }
    else
    {
        pThis->__kflcnReadIntrStatus__ = &kflcnReadIntrStatus_TU102;
    }

    // kflcnRiscvReadIntrStatus -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kflcnRiscvReadIntrStatus__ = &kflcnRiscvReadIntrStatus_474d46;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
        {
            pThis->__kflcnRiscvReadIntrStatus__ = &kflcnRiscvReadIntrStatus_TU102;
        }
        else
        {
            pThis->__kflcnRiscvReadIntrStatus__ = &kflcnRiscvReadIntrStatus_GA102;
        }
    }

    // kflcnIntrRetrigger -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kflcnIntrRetrigger__ = &kflcnIntrRetrigger_f2d351;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
        {
            pThis->__kflcnIntrRetrigger__ = &kflcnIntrRetrigger_b3696a;
        }
        else
        {
            pThis->__kflcnIntrRetrigger__ = &kflcnIntrRetrigger_GA100;
        }
    }

    // kflcnMaskImemAddr -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kflcnMaskImemAddr__ = &kflcnMaskImemAddr_474d46;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
        {
            pThis->__kflcnMaskImemAddr__ = &kflcnMaskImemAddr_TU102;
        }
        else
        {
            pThis->__kflcnMaskImemAddr__ = &kflcnMaskImemAddr_GA100;
        }
    }

    // kflcnMaskDmemAddr -- virtual halified (3 hals) override (kcrashcatEngine) base (kcrashcatEngine) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kflcnMaskDmemAddr__ = &kflcnMaskDmemAddr_474d46;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
        {
            pThis->__kflcnMaskDmemAddr__ = &kflcnMaskDmemAddr_TU102;
        }
        else
        {
            pThis->__kflcnMaskDmemAddr__ = &kflcnMaskDmemAddr_GA100;
        }
    }

    // kflcnGetEccInterruptMask -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 */ 
    {
        pThis->__kflcnGetEccInterruptMask__ = &kflcnGetEccInterruptMask_GB100;
    }
    // default
    else
    {
        pThis->__kflcnGetEccInterruptMask__ = &kflcnGetEccInterruptMask_4a4dee;
    }
} // End __nvoc_init_funcTable_KernelFalcon_1 with approximately 60 basic block(s).


// Initialize vtable(s) for 36 virtual method(s).
void __nvoc_init_funcTable_KernelFalcon(KernelFalcon *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize vtable(s) with 23 per-object function pointer(s).
    __nvoc_init_funcTable_KernelFalcon_1(pThis, pRmhalspecowner);
}

// Initialize newly constructed object.
void __nvoc_init__KernelFalcon(KernelFalcon *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_CrashCatEngine = &pThis->__nvoc_base_KernelCrashCatEngine.__nvoc_base_CrashCatEngine;    // (crashcatEngine) super^2
    pThis->__nvoc_pbase_KernelCrashCatEngine = &pThis->__nvoc_base_KernelCrashCatEngine;    // (kcrashcatEngine) super
    pThis->__nvoc_pbase_KernelFalcon = pThis;    // (kflcn) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__KernelCrashCatEngine(&pThis->__nvoc_base_KernelCrashCatEngine, pRmhalspecowner);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_KernelCrashCatEngine.__nvoc_base_CrashCatEngine.__nvoc_metadata_ptr = &__nvoc_metadata__KernelFalcon.metadata__KernelCrashCatEngine.metadata__CrashCatEngine;    // (crashcatEngine) super^2
    pThis->__nvoc_base_KernelCrashCatEngine.__nvoc_metadata_ptr = &__nvoc_metadata__KernelFalcon.metadata__KernelCrashCatEngine;    // (kcrashcatEngine) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__KernelFalcon;    // (kflcn) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_KernelFalcon(pThis, pRmhalspecowner);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0xabcf08 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GenericKernelFalcon;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_CrashCatEngine;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelCrashCatEngine;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelFalcon;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_IntrService;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

// Forward declarations for GenericKernelFalcon
void __nvoc_init__KernelFalcon(KernelFalcon*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init__IntrService(IntrService*);
void __nvoc_init__Object(Object*);
void __nvoc_init__GenericKernelFalcon(GenericKernelFalcon*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_funcTable_GenericKernelFalcon(GenericKernelFalcon*);
NV_STATUS __nvoc_ctor_GenericKernelFalcon(GenericKernelFalcon*, RmHalspecOwner *pRmhalspecowner, struct OBJGPU *arg_pGpu, KernelFalconEngineConfig *arg_pFalconConfig);
void __nvoc_init_dataField_GenericKernelFalcon(GenericKernelFalcon*);
void __nvoc_dtor_GenericKernelFalcon(GenericKernelFalcon*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__GenericKernelFalcon;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__GenericKernelFalcon;

// Down-thunk(s) to bridge GenericKernelFalcon methods from ancestors (if any)
NvBool __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineConfigured(struct CrashCatEngine *arg_this);    // super^2
void __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineUnload(struct CrashCatEngine *arg_this);    // super^2
void __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineVprintf(struct CrashCatEngine *arg_this, NvBool bReportStart, const char *fmt, va_list args);    // super^2
NvU32 __nvoc_down_thunk_KernelCrashCatEngine_crashcatEnginePriRead(struct CrashCatEngine *arg_this, NvU32 offset);    // super^2
void __nvoc_down_thunk_KernelCrashCatEngine_crashcatEnginePriWrite(struct CrashCatEngine *arg_this, NvU32 offset, NvU32 data);    // super^2
void * __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineMapBufferDescriptor(struct CrashCatEngine *arg_this, CrashCatBufferDescriptor *pBufDesc);    // super^2
void __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineUnmapBufferDescriptor(struct CrashCatEngine *arg_this, CrashCatBufferDescriptor *pBufDesc);    // super^2
void __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineSyncBufferDescriptor(struct CrashCatEngine *arg_this, CrashCatBufferDescriptor *pBufDesc, NvU32 offset, NvU32 size);    // super^2
const NvU32 * __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineGetScratchOffsets(struct CrashCatEngine *arg_this, NV_CRASHCAT_SCRATCH_GROUP_ID scratchGroupId);    // super^2
NvU32 __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineGetWFL0Offset(struct CrashCatEngine *arg_this);    // super^2
NvU32 __nvoc_down_thunk_KernelFalcon_kcrashcatEngineRegRead(struct OBJGPU *pGpu, struct KernelCrashCatEngine *pKernelFlcn, NvU32 offset);    // super
void __nvoc_down_thunk_KernelFalcon_kcrashcatEngineRegWrite(struct OBJGPU *pGpu, struct KernelCrashCatEngine *pKernelFlcn, NvU32 offset, NvU32 data);    // super
NvU32 __nvoc_down_thunk_KernelFalcon_kcrashcatEngineMaskDmemAddr(struct OBJGPU *pGpu, struct KernelCrashCatEngine *pKernelFlcn, NvU32 addr);    // super
NV_STATUS __nvoc_down_thunk_GenericKernelFalcon_kflcnResetHw(struct OBJGPU *pGpu, struct KernelFalcon *pGenKernFlcn);    // this
void __nvoc_down_thunk_GenericKernelFalcon_intrservRegisterIntrService(struct OBJGPU *arg1, struct IntrService *arg_this, IntrServiceRecord arg3[179]);    // this
NV_STATUS __nvoc_down_thunk_GenericKernelFalcon_intrservServiceNotificationInterrupt(struct OBJGPU *arg1, struct IntrService *arg_this, IntrServiceServiceNotificationInterruptArguments *arg3);    // this

// Up-thunk(s) to bridge GenericKernelFalcon methods to ancestors (if any)
NvBool __nvoc_up_thunk_KernelCrashCatEngine_kflcnConfigured(struct KernelFalcon *arg_this);    // super
void __nvoc_up_thunk_KernelCrashCatEngine_kflcnUnload(struct KernelFalcon *arg_this);    // super
void __nvoc_up_thunk_KernelCrashCatEngine_kflcnVprintf(struct KernelFalcon *arg_this, NvBool bReportStart, const char *fmt, va_list args);    // super
NvU32 __nvoc_up_thunk_KernelCrashCatEngine_kflcnPriRead(struct KernelFalcon *arg_this, NvU32 offset);    // super
void __nvoc_up_thunk_KernelCrashCatEngine_kflcnPriWrite(struct KernelFalcon *arg_this, NvU32 offset, NvU32 data);    // super
void * __nvoc_up_thunk_KernelCrashCatEngine_kflcnMapBufferDescriptor(struct KernelFalcon *arg_this, CrashCatBufferDescriptor *pBufDesc);    // super
void __nvoc_up_thunk_KernelCrashCatEngine_kflcnUnmapBufferDescriptor(struct KernelFalcon *arg_this, CrashCatBufferDescriptor *pBufDesc);    // super
void __nvoc_up_thunk_KernelCrashCatEngine_kflcnSyncBufferDescriptor(struct KernelFalcon *arg_this, CrashCatBufferDescriptor *pBufDesc, NvU32 offset, NvU32 size);    // super
void __nvoc_up_thunk_KernelCrashCatEngine_kflcnReadDmem(struct KernelFalcon *arg_this, NvU32 offset, NvU32 size, void *pBuf);    // super
void __nvoc_up_thunk_KernelCrashCatEngine_kflcnReadEmem(struct KernelFalcon *arg_this, NvU64 offset, NvU64 size, void *pBuf);    // super
const NvU32 * __nvoc_up_thunk_KernelCrashCatEngine_kflcnGetScratchOffsets(struct KernelFalcon *arg_this, NV_CRASHCAT_SCRATCH_GROUP_ID scratchGroupId);    // super
NvU32 __nvoc_up_thunk_KernelCrashCatEngine_kflcnGetWFL0Offset(struct KernelFalcon *arg_this);    // super
NvU32 __nvoc_up_thunk_KernelFalcon_gkflcnRegRead(struct OBJGPU *pGpu, struct GenericKernelFalcon *pKernelFlcn, NvU32 offset);    // this
void __nvoc_up_thunk_KernelFalcon_gkflcnRegWrite(struct OBJGPU *pGpu, struct GenericKernelFalcon *pKernelFlcn, NvU32 offset, NvU32 data);    // this
NvU32 __nvoc_up_thunk_KernelFalcon_gkflcnMaskDmemAddr(struct OBJGPU *pGpu, struct GenericKernelFalcon *pKernelFlcn, NvU32 addr);    // this
NvBool __nvoc_up_thunk_KernelCrashCatEngine_gkflcnConfigured(struct GenericKernelFalcon *arg_this);    // this
void __nvoc_up_thunk_KernelCrashCatEngine_gkflcnUnload(struct GenericKernelFalcon *arg_this);    // this
void __nvoc_up_thunk_KernelCrashCatEngine_gkflcnVprintf(struct GenericKernelFalcon *arg_this, NvBool bReportStart, const char *fmt, va_list args);    // this
NvU32 __nvoc_up_thunk_KernelCrashCatEngine_gkflcnPriRead(struct GenericKernelFalcon *arg_this, NvU32 offset);    // this
void __nvoc_up_thunk_KernelCrashCatEngine_gkflcnPriWrite(struct GenericKernelFalcon *arg_this, NvU32 offset, NvU32 data);    // this
void * __nvoc_up_thunk_KernelCrashCatEngine_gkflcnMapBufferDescriptor(struct GenericKernelFalcon *arg_this, CrashCatBufferDescriptor *pBufDesc);    // this
void __nvoc_up_thunk_KernelCrashCatEngine_gkflcnUnmapBufferDescriptor(struct GenericKernelFalcon *arg_this, CrashCatBufferDescriptor *pBufDesc);    // this
void __nvoc_up_thunk_KernelCrashCatEngine_gkflcnSyncBufferDescriptor(struct GenericKernelFalcon *arg_this, CrashCatBufferDescriptor *pBufDesc, NvU32 offset, NvU32 size);    // this
void __nvoc_up_thunk_KernelCrashCatEngine_gkflcnReadDmem(struct GenericKernelFalcon *arg_this, NvU32 offset, NvU32 size, void *pBuf);    // this
void __nvoc_up_thunk_KernelCrashCatEngine_gkflcnReadEmem(struct GenericKernelFalcon *arg_this, NvU64 offset, NvU64 size, void *pBuf);    // this
const NvU32 * __nvoc_up_thunk_KernelCrashCatEngine_gkflcnGetScratchOffsets(struct GenericKernelFalcon *arg_this, NV_CRASHCAT_SCRATCH_GROUP_ID scratchGroupId);    // this
NvU32 __nvoc_up_thunk_KernelCrashCatEngine_gkflcnGetWFL0Offset(struct GenericKernelFalcon *arg_this);    // this
NvBool __nvoc_up_thunk_IntrService_gkflcnClearInterrupt(struct OBJGPU *pGpu, struct GenericKernelFalcon *pIntrService, IntrServiceClearInterruptArguments *pParams);    // this
NvU32 __nvoc_up_thunk_IntrService_gkflcnServiceInterrupt(struct OBJGPU *pGpu, struct GenericKernelFalcon *pIntrService, IntrServiceServiceInterruptArguments *pParams);    // this

const struct NVOC_CLASS_DEF __nvoc_class_def_GenericKernelFalcon = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(GenericKernelFalcon),
        /*classId=*/            classId(GenericKernelFalcon),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "GenericKernelFalcon",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_GenericKernelFalcon,
    /*pCastInfo=*/          &__nvoc_castinfo__GenericKernelFalcon,
    /*pExportInfo=*/        &__nvoc_export_info__GenericKernelFalcon
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__GenericKernelFalcon __nvoc_metadata__GenericKernelFalcon = {
    .rtti.pClassDef = &__nvoc_class_def_GenericKernelFalcon,    // (gkflcn) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_GenericKernelFalcon,
    .rtti.offset    = 0,
    .metadata__KernelFalcon.rtti.pClassDef = &__nvoc_class_def_KernelFalcon,    // (kflcn) super
    .metadata__KernelFalcon.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__KernelFalcon.rtti.offset    = NV_OFFSETOF(GenericKernelFalcon, __nvoc_base_KernelFalcon),
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.rtti.pClassDef = &__nvoc_class_def_KernelCrashCatEngine,    // (kcrashcatEngine) super^2
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.rtti.offset    = NV_OFFSETOF(GenericKernelFalcon, __nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine),
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.metadata__CrashCatEngine.rtti.pClassDef = &__nvoc_class_def_CrashCatEngine,    // (crashcatEngine) super^3
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.metadata__CrashCatEngine.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.metadata__CrashCatEngine.rtti.offset    = NV_OFFSETOF(GenericKernelFalcon, __nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine.__nvoc_base_CrashCatEngine),
    .metadata__IntrService.rtti.pClassDef = &__nvoc_class_def_IntrService,    // (intrserv) super
    .metadata__IntrService.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__IntrService.rtti.offset    = NV_OFFSETOF(GenericKernelFalcon, __nvoc_base_IntrService),
    .metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super
    .metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Object.rtti.offset    = NV_OFFSETOF(GenericKernelFalcon, __nvoc_base_Object),

    .vtable.__gkflcnResetHw__ = &gkflcnResetHw_IMPL,    // virtual override (kflcn) base (kflcn)
    .metadata__KernelFalcon.vtable.__kflcnResetHw__ = &__nvoc_down_thunk_GenericKernelFalcon_kflcnResetHw,    // pure virtual
    .vtable.__gkflcnRegisterIntrService__ = &gkflcnRegisterIntrService_IMPL,    // virtual override (intrserv) base (intrserv)
    .metadata__IntrService.vtable.__intrservRegisterIntrService__ = &__nvoc_down_thunk_GenericKernelFalcon_intrservRegisterIntrService,    // virtual
    .vtable.__gkflcnServiceNotificationInterrupt__ = &gkflcnServiceNotificationInterrupt_IMPL,    // virtual override (intrserv) base (intrserv)
    .metadata__IntrService.vtable.__intrservServiceNotificationInterrupt__ = &__nvoc_down_thunk_GenericKernelFalcon_intrservServiceNotificationInterrupt,    // virtual
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.vtable.__kcrashcatEngineRegRead__ = &__nvoc_down_thunk_KernelFalcon_kcrashcatEngineRegRead,    // pure virtual
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.vtable.__kcrashcatEngineRegWrite__ = &__nvoc_down_thunk_KernelFalcon_kcrashcatEngineRegWrite,    // pure virtual
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.vtable.__kcrashcatEngineMaskDmemAddr__ = &__nvoc_down_thunk_KernelFalcon_kcrashcatEngineMaskDmemAddr,    // pure virtual
    .vtable.__gkflcnConfigured__ = &__nvoc_up_thunk_KernelCrashCatEngine_gkflcnConfigured,    // virtual inherited (kcrashcatEngine) base (kflcn)
    .metadata__KernelFalcon.vtable.__kflcnConfigured__ = &__nvoc_up_thunk_KernelCrashCatEngine_kflcnConfigured,    // virtual inherited (kcrashcatEngine) base (kcrashcatEngine)
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.vtable.__kcrashcatEngineConfigured__ = &kcrashcatEngineConfigured_IMPL,    // virtual override (crashcatEngine) base (crashcatEngine)
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.metadata__CrashCatEngine.vtable.__crashcatEngineConfigured__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineConfigured,    // pure virtual
    .vtable.__gkflcnUnload__ = &__nvoc_up_thunk_KernelCrashCatEngine_gkflcnUnload,    // virtual inherited (kcrashcatEngine) base (kflcn)
    .metadata__KernelFalcon.vtable.__kflcnUnload__ = &__nvoc_up_thunk_KernelCrashCatEngine_kflcnUnload,    // virtual inherited (kcrashcatEngine) base (kcrashcatEngine)
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.vtable.__kcrashcatEngineUnload__ = &kcrashcatEngineUnload_IMPL,    // virtual override (crashcatEngine) base (crashcatEngine)
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.metadata__CrashCatEngine.vtable.__crashcatEngineUnload__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineUnload,    // virtual
    .vtable.__gkflcnVprintf__ = &__nvoc_up_thunk_KernelCrashCatEngine_gkflcnVprintf,    // virtual inherited (kcrashcatEngine) base (kflcn)
    .metadata__KernelFalcon.vtable.__kflcnVprintf__ = &__nvoc_up_thunk_KernelCrashCatEngine_kflcnVprintf,    // virtual inherited (kcrashcatEngine) base (kcrashcatEngine)
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.vtable.__kcrashcatEngineVprintf__ = &kcrashcatEngineVprintf_IMPL,    // virtual override (crashcatEngine) base (crashcatEngine)
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.metadata__CrashCatEngine.vtable.__crashcatEngineVprintf__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineVprintf,    // pure virtual
    .vtable.__gkflcnPriRead__ = &__nvoc_up_thunk_KernelCrashCatEngine_gkflcnPriRead,    // virtual inherited (kcrashcatEngine) base (kflcn)
    .metadata__KernelFalcon.vtable.__kflcnPriRead__ = &__nvoc_up_thunk_KernelCrashCatEngine_kflcnPriRead,    // virtual inherited (kcrashcatEngine) base (kcrashcatEngine)
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.vtable.__kcrashcatEnginePriRead__ = &kcrashcatEnginePriRead_IMPL,    // virtual override (crashcatEngine) base (crashcatEngine)
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.metadata__CrashCatEngine.vtable.__crashcatEnginePriRead__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEnginePriRead,    // pure virtual
    .vtable.__gkflcnPriWrite__ = &__nvoc_up_thunk_KernelCrashCatEngine_gkflcnPriWrite,    // virtual inherited (kcrashcatEngine) base (kflcn)
    .metadata__KernelFalcon.vtable.__kflcnPriWrite__ = &__nvoc_up_thunk_KernelCrashCatEngine_kflcnPriWrite,    // virtual inherited (kcrashcatEngine) base (kcrashcatEngine)
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.vtable.__kcrashcatEnginePriWrite__ = &kcrashcatEnginePriWrite_IMPL,    // virtual override (crashcatEngine) base (crashcatEngine)
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.metadata__CrashCatEngine.vtable.__crashcatEnginePriWrite__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEnginePriWrite,    // pure virtual
    .vtable.__gkflcnMapBufferDescriptor__ = &__nvoc_up_thunk_KernelCrashCatEngine_gkflcnMapBufferDescriptor,    // virtual inherited (kcrashcatEngine) base (kflcn)
    .metadata__KernelFalcon.vtable.__kflcnMapBufferDescriptor__ = &__nvoc_up_thunk_KernelCrashCatEngine_kflcnMapBufferDescriptor,    // virtual inherited (kcrashcatEngine) base (kcrashcatEngine)
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.vtable.__kcrashcatEngineMapBufferDescriptor__ = &kcrashcatEngineMapBufferDescriptor_IMPL,    // virtual override (crashcatEngine) base (crashcatEngine)
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.metadata__CrashCatEngine.vtable.__crashcatEngineMapBufferDescriptor__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineMapBufferDescriptor,    // pure virtual
    .vtable.__gkflcnUnmapBufferDescriptor__ = &__nvoc_up_thunk_KernelCrashCatEngine_gkflcnUnmapBufferDescriptor,    // virtual inherited (kcrashcatEngine) base (kflcn)
    .metadata__KernelFalcon.vtable.__kflcnUnmapBufferDescriptor__ = &__nvoc_up_thunk_KernelCrashCatEngine_kflcnUnmapBufferDescriptor,    // virtual inherited (kcrashcatEngine) base (kcrashcatEngine)
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.vtable.__kcrashcatEngineUnmapBufferDescriptor__ = &kcrashcatEngineUnmapBufferDescriptor_IMPL,    // virtual override (crashcatEngine) base (crashcatEngine)
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.metadata__CrashCatEngine.vtable.__crashcatEngineUnmapBufferDescriptor__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineUnmapBufferDescriptor,    // pure virtual
    .vtable.__gkflcnSyncBufferDescriptor__ = &__nvoc_up_thunk_KernelCrashCatEngine_gkflcnSyncBufferDescriptor,    // virtual inherited (kcrashcatEngine) base (kflcn)
    .metadata__KernelFalcon.vtable.__kflcnSyncBufferDescriptor__ = &__nvoc_up_thunk_KernelCrashCatEngine_kflcnSyncBufferDescriptor,    // virtual inherited (kcrashcatEngine) base (kcrashcatEngine)
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.vtable.__kcrashcatEngineSyncBufferDescriptor__ = &kcrashcatEngineSyncBufferDescriptor_IMPL,    // virtual override (crashcatEngine) base (crashcatEngine)
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.metadata__CrashCatEngine.vtable.__crashcatEngineSyncBufferDescriptor__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineSyncBufferDescriptor,    // pure virtual
    .vtable.__gkflcnReadDmem__ = &__nvoc_up_thunk_KernelCrashCatEngine_gkflcnReadDmem,    // virtual halified (singleton optimized) inherited (kcrashcatEngine) base (kflcn)
    .metadata__KernelFalcon.vtable.__kflcnReadDmem__ = &__nvoc_up_thunk_KernelCrashCatEngine_kflcnReadDmem,    // virtual halified (singleton optimized) inherited (kcrashcatEngine) base (kcrashcatEngine)
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.vtable.__kcrashcatEngineReadDmem__ = &kcrashcatEngineReadDmem_TU102,    // virtual halified (singleton optimized)
    .vtable.__gkflcnReadEmem__ = &__nvoc_up_thunk_KernelCrashCatEngine_gkflcnReadEmem,    // virtual halified (singleton optimized) inherited (kcrashcatEngine) base (kflcn)
    .metadata__KernelFalcon.vtable.__kflcnReadEmem__ = &__nvoc_up_thunk_KernelCrashCatEngine_kflcnReadEmem,    // virtual halified (singleton optimized) inherited (kcrashcatEngine) base (kcrashcatEngine)
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.vtable.__kcrashcatEngineReadEmem__ = &kcrashcatEngineReadEmem_2fced3,    // virtual halified (singleton optimized)
    .vtable.__gkflcnGetScratchOffsets__ = &__nvoc_up_thunk_KernelCrashCatEngine_gkflcnGetScratchOffsets,    // virtual halified (singleton optimized) inherited (kcrashcatEngine) base (kflcn)
    .metadata__KernelFalcon.vtable.__kflcnGetScratchOffsets__ = &__nvoc_up_thunk_KernelCrashCatEngine_kflcnGetScratchOffsets,    // virtual halified (singleton optimized) inherited (kcrashcatEngine) base (kcrashcatEngine)
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.vtable.__kcrashcatEngineGetScratchOffsets__ = &kcrashcatEngineGetScratchOffsets_TU102,    // virtual halified (singleton optimized) override (crashcatEngine) base (crashcatEngine)
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.metadata__CrashCatEngine.vtable.__crashcatEngineGetScratchOffsets__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineGetScratchOffsets,    // pure virtual
    .vtable.__gkflcnGetWFL0Offset__ = &__nvoc_up_thunk_KernelCrashCatEngine_gkflcnGetWFL0Offset,    // virtual halified (singleton optimized) inherited (kcrashcatEngine) base (kflcn)
    .metadata__KernelFalcon.vtable.__kflcnGetWFL0Offset__ = &__nvoc_up_thunk_KernelCrashCatEngine_kflcnGetWFL0Offset,    // virtual halified (singleton optimized) inherited (kcrashcatEngine) base (kcrashcatEngine)
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.vtable.__kcrashcatEngineGetWFL0Offset__ = &kcrashcatEngineGetWFL0Offset_TU102,    // virtual halified (singleton optimized) override (crashcatEngine) base (crashcatEngine)
    .metadata__KernelFalcon.metadata__KernelCrashCatEngine.metadata__CrashCatEngine.vtable.__crashcatEngineGetWFL0Offset__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineGetWFL0Offset,    // pure virtual
    .vtable.__gkflcnClearInterrupt__ = &__nvoc_up_thunk_IntrService_gkflcnClearInterrupt,    // virtual inherited (intrserv) base (intrserv)
    .metadata__IntrService.vtable.__intrservClearInterrupt__ = &intrservClearInterrupt_IMPL,    // virtual
    .vtable.__gkflcnServiceInterrupt__ = &__nvoc_up_thunk_IntrService_gkflcnServiceInterrupt,    // virtual inherited (intrserv) base (intrserv)
    .metadata__IntrService.vtable.__intrservServiceInterrupt__ = &intrservServiceInterrupt_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__GenericKernelFalcon = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__GenericKernelFalcon.rtti,    // [0]: (gkflcn) this
        &__nvoc_metadata__GenericKernelFalcon.metadata__KernelFalcon.rtti,    // [1]: (kflcn) super
        &__nvoc_metadata__GenericKernelFalcon.metadata__KernelFalcon.metadata__KernelCrashCatEngine.rtti,    // [2]: (kcrashcatEngine) super^2
        &__nvoc_metadata__GenericKernelFalcon.metadata__KernelFalcon.metadata__KernelCrashCatEngine.metadata__CrashCatEngine.rtti,    // [3]: (crashcatEngine) super^3
        &__nvoc_metadata__GenericKernelFalcon.metadata__IntrService.rtti,    // [4]: (intrserv) super
        &__nvoc_metadata__GenericKernelFalcon.metadata__Object.rtti,    // [5]: (obj) super
    }
};

// 3 down-thunk(s) defined to bridge methods in GenericKernelFalcon from superclasses

// gkflcnResetHw: virtual override (kflcn) base (kflcn)
NV_STATUS __nvoc_down_thunk_GenericKernelFalcon_kflcnResetHw(struct OBJGPU *pGpu, struct KernelFalcon *pGenKernFlcn) {
    return gkflcnResetHw(pGpu, (struct GenericKernelFalcon *)(((unsigned char *) pGenKernFlcn) - NV_OFFSETOF(GenericKernelFalcon, __nvoc_base_KernelFalcon)));
}

// gkflcnRegisterIntrService: virtual override (intrserv) base (intrserv)
void __nvoc_down_thunk_GenericKernelFalcon_intrservRegisterIntrService(struct OBJGPU *arg1, struct IntrService *arg_this, IntrServiceRecord arg3[179]) {
    gkflcnRegisterIntrService(arg1, (struct GenericKernelFalcon *)(((unsigned char *) arg_this) - NV_OFFSETOF(GenericKernelFalcon, __nvoc_base_IntrService)), arg3);
}

// gkflcnServiceNotificationInterrupt: virtual override (intrserv) base (intrserv)
NV_STATUS __nvoc_down_thunk_GenericKernelFalcon_intrservServiceNotificationInterrupt(struct OBJGPU *arg1, struct IntrService *arg_this, IntrServiceServiceNotificationInterruptArguments *arg3) {
    return gkflcnServiceNotificationInterrupt(arg1, (struct GenericKernelFalcon *)(((unsigned char *) arg_this) - NV_OFFSETOF(GenericKernelFalcon, __nvoc_base_IntrService)), arg3);
}


// 17 up-thunk(s) defined to bridge methods in GenericKernelFalcon to superclasses

// gkflcnRegRead: virtual halified (2 hals) inherited (kflcn) base (kflcn) body
NvU32 __nvoc_up_thunk_KernelFalcon_gkflcnRegRead(struct OBJGPU *pGpu, struct GenericKernelFalcon *pKernelFlcn, NvU32 offset) {
    return kflcnRegRead(pGpu, (struct KernelFalcon *)(((unsigned char *) pKernelFlcn) + NV_OFFSETOF(GenericKernelFalcon, __nvoc_base_KernelFalcon)), offset);
}

// gkflcnRegWrite: virtual halified (2 hals) inherited (kflcn) base (kflcn) body
void __nvoc_up_thunk_KernelFalcon_gkflcnRegWrite(struct OBJGPU *pGpu, struct GenericKernelFalcon *pKernelFlcn, NvU32 offset, NvU32 data) {
    kflcnRegWrite(pGpu, (struct KernelFalcon *)(((unsigned char *) pKernelFlcn) + NV_OFFSETOF(GenericKernelFalcon, __nvoc_base_KernelFalcon)), offset, data);
}

// gkflcnMaskDmemAddr: virtual halified (3 hals) inherited (kflcn) base (kflcn) body
NvU32 __nvoc_up_thunk_KernelFalcon_gkflcnMaskDmemAddr(struct OBJGPU *pGpu, struct GenericKernelFalcon *pKernelFlcn, NvU32 addr) {
    return kflcnMaskDmemAddr(pGpu, (struct KernelFalcon *)(((unsigned char *) pKernelFlcn) + NV_OFFSETOF(GenericKernelFalcon, __nvoc_base_KernelFalcon)), addr);
}

// gkflcnConfigured: virtual inherited (kcrashcatEngine) base (kflcn)
NvBool __nvoc_up_thunk_KernelCrashCatEngine_gkflcnConfigured(struct GenericKernelFalcon *arg_this) {
    return kcrashcatEngineConfigured((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) + NV_OFFSETOF(GenericKernelFalcon, __nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine)));
}

// gkflcnUnload: virtual inherited (kcrashcatEngine) base (kflcn)
void __nvoc_up_thunk_KernelCrashCatEngine_gkflcnUnload(struct GenericKernelFalcon *arg_this) {
    kcrashcatEngineUnload((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) + NV_OFFSETOF(GenericKernelFalcon, __nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine)));
}

// gkflcnVprintf: virtual inherited (kcrashcatEngine) base (kflcn)
void __nvoc_up_thunk_KernelCrashCatEngine_gkflcnVprintf(struct GenericKernelFalcon *arg_this, NvBool bReportStart, const char *fmt, va_list args) {
    kcrashcatEngineVprintf((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) + NV_OFFSETOF(GenericKernelFalcon, __nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine)), bReportStart, fmt, args);
}

// gkflcnPriRead: virtual inherited (kcrashcatEngine) base (kflcn)
NvU32 __nvoc_up_thunk_KernelCrashCatEngine_gkflcnPriRead(struct GenericKernelFalcon *arg_this, NvU32 offset) {
    return kcrashcatEnginePriRead((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) + NV_OFFSETOF(GenericKernelFalcon, __nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine)), offset);
}

// gkflcnPriWrite: virtual inherited (kcrashcatEngine) base (kflcn)
void __nvoc_up_thunk_KernelCrashCatEngine_gkflcnPriWrite(struct GenericKernelFalcon *arg_this, NvU32 offset, NvU32 data) {
    kcrashcatEnginePriWrite((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) + NV_OFFSETOF(GenericKernelFalcon, __nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine)), offset, data);
}

// gkflcnMapBufferDescriptor: virtual inherited (kcrashcatEngine) base (kflcn)
void * __nvoc_up_thunk_KernelCrashCatEngine_gkflcnMapBufferDescriptor(struct GenericKernelFalcon *arg_this, CrashCatBufferDescriptor *pBufDesc) {
    return kcrashcatEngineMapBufferDescriptor((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) + NV_OFFSETOF(GenericKernelFalcon, __nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine)), pBufDesc);
}

// gkflcnUnmapBufferDescriptor: virtual inherited (kcrashcatEngine) base (kflcn)
void __nvoc_up_thunk_KernelCrashCatEngine_gkflcnUnmapBufferDescriptor(struct GenericKernelFalcon *arg_this, CrashCatBufferDescriptor *pBufDesc) {
    kcrashcatEngineUnmapBufferDescriptor((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) + NV_OFFSETOF(GenericKernelFalcon, __nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine)), pBufDesc);
}

// gkflcnSyncBufferDescriptor: virtual inherited (kcrashcatEngine) base (kflcn)
void __nvoc_up_thunk_KernelCrashCatEngine_gkflcnSyncBufferDescriptor(struct GenericKernelFalcon *arg_this, CrashCatBufferDescriptor *pBufDesc, NvU32 offset, NvU32 size) {
    kcrashcatEngineSyncBufferDescriptor((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) + NV_OFFSETOF(GenericKernelFalcon, __nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine)), pBufDesc, offset, size);
}

// gkflcnReadDmem: virtual halified (singleton optimized) inherited (kcrashcatEngine) base (kflcn)
void __nvoc_up_thunk_KernelCrashCatEngine_gkflcnReadDmem(struct GenericKernelFalcon *arg_this, NvU32 offset, NvU32 size, void *pBuf) {
    kcrashcatEngineReadDmem((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) + NV_OFFSETOF(GenericKernelFalcon, __nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine)), offset, size, pBuf);
}

// gkflcnReadEmem: virtual halified (singleton optimized) inherited (kcrashcatEngine) base (kflcn)
void __nvoc_up_thunk_KernelCrashCatEngine_gkflcnReadEmem(struct GenericKernelFalcon *arg_this, NvU64 offset, NvU64 size, void *pBuf) {
    kcrashcatEngineReadEmem((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) + NV_OFFSETOF(GenericKernelFalcon, __nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine)), offset, size, pBuf);
}

// gkflcnGetScratchOffsets: virtual halified (singleton optimized) inherited (kcrashcatEngine) base (kflcn)
const NvU32 * __nvoc_up_thunk_KernelCrashCatEngine_gkflcnGetScratchOffsets(struct GenericKernelFalcon *arg_this, NV_CRASHCAT_SCRATCH_GROUP_ID scratchGroupId) {
    return kcrashcatEngineGetScratchOffsets((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) + NV_OFFSETOF(GenericKernelFalcon, __nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine)), scratchGroupId);
}

// gkflcnGetWFL0Offset: virtual halified (singleton optimized) inherited (kcrashcatEngine) base (kflcn)
NvU32 __nvoc_up_thunk_KernelCrashCatEngine_gkflcnGetWFL0Offset(struct GenericKernelFalcon *arg_this) {
    return kcrashcatEngineGetWFL0Offset((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) + NV_OFFSETOF(GenericKernelFalcon, __nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine)));
}

// gkflcnClearInterrupt: virtual inherited (intrserv) base (intrserv)
NvBool __nvoc_up_thunk_IntrService_gkflcnClearInterrupt(struct OBJGPU *pGpu, struct GenericKernelFalcon *pIntrService, IntrServiceClearInterruptArguments *pParams) {
    return intrservClearInterrupt(pGpu, (struct IntrService *)(((unsigned char *) pIntrService) + NV_OFFSETOF(GenericKernelFalcon, __nvoc_base_IntrService)), pParams);
}

// gkflcnServiceInterrupt: virtual inherited (intrserv) base (intrserv)
NvU32 __nvoc_up_thunk_IntrService_gkflcnServiceInterrupt(struct OBJGPU *pGpu, struct GenericKernelFalcon *pIntrService, IntrServiceServiceInterruptArguments *pParams) {
    return intrservServiceInterrupt(pGpu, (struct IntrService *)(((unsigned char *) pIntrService) + NV_OFFSETOF(GenericKernelFalcon, __nvoc_base_IntrService)), pParams);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__GenericKernelFalcon = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_KernelFalcon(KernelFalcon*);
void __nvoc_dtor_IntrService(IntrService*);
void __nvoc_dtor_Object(Object*);
void __nvoc_dtor_GenericKernelFalcon(GenericKernelFalcon *pThis) {
    __nvoc_dtor_KernelFalcon(&pThis->__nvoc_base_KernelFalcon);
    __nvoc_dtor_IntrService(&pThis->__nvoc_base_IntrService);
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_GenericKernelFalcon(GenericKernelFalcon *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_KernelFalcon(KernelFalcon* , RmHalspecOwner* );
NV_STATUS __nvoc_ctor_IntrService(IntrService* );
NV_STATUS __nvoc_ctor_Object(Object* );
NV_STATUS __nvoc_ctor_GenericKernelFalcon(GenericKernelFalcon *pThis, RmHalspecOwner *pRmhalspecowner, struct OBJGPU * arg_pGpu, KernelFalconEngineConfig * arg_pFalconConfig) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_KernelFalcon(&pThis->__nvoc_base_KernelFalcon, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_ctor_GenericKernelFalcon_fail_KernelFalcon;
    status = __nvoc_ctor_IntrService(&pThis->__nvoc_base_IntrService);
    if (status != NV_OK) goto __nvoc_ctor_GenericKernelFalcon_fail_IntrService;
    status = __nvoc_ctor_Object(&pThis->__nvoc_base_Object);
    if (status != NV_OK) goto __nvoc_ctor_GenericKernelFalcon_fail_Object;
    __nvoc_init_dataField_GenericKernelFalcon(pThis);

    status = __nvoc_gkflcnConstruct(pThis, arg_pGpu, arg_pFalconConfig);
    if (status != NV_OK) goto __nvoc_ctor_GenericKernelFalcon_fail__init;
    goto __nvoc_ctor_GenericKernelFalcon_exit; // Success

__nvoc_ctor_GenericKernelFalcon_fail__init:
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
__nvoc_ctor_GenericKernelFalcon_fail_Object:
    __nvoc_dtor_IntrService(&pThis->__nvoc_base_IntrService);
__nvoc_ctor_GenericKernelFalcon_fail_IntrService:
    __nvoc_dtor_KernelFalcon(&pThis->__nvoc_base_KernelFalcon);
__nvoc_ctor_GenericKernelFalcon_fail_KernelFalcon:
__nvoc_ctor_GenericKernelFalcon_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_GenericKernelFalcon_1(GenericKernelFalcon *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    // gkflcnRegRead -- virtual halified (2 hals) inherited (kflcn) base (kflcn) body
    pThis->__gkflcnRegRead__ = &__nvoc_up_thunk_KernelFalcon_gkflcnRegRead;

    // gkflcnRegWrite -- virtual halified (2 hals) inherited (kflcn) base (kflcn) body
    pThis->__gkflcnRegWrite__ = &__nvoc_up_thunk_KernelFalcon_gkflcnRegWrite;

    // gkflcnMaskDmemAddr -- virtual halified (3 hals) inherited (kflcn) base (kflcn) body
    pThis->__gkflcnMaskDmemAddr__ = &__nvoc_up_thunk_KernelFalcon_gkflcnMaskDmemAddr;
} // End __nvoc_init_funcTable_GenericKernelFalcon_1 with approximately 7 basic block(s).


// Initialize vtable(s) for 20 virtual method(s).
void __nvoc_init_funcTable_GenericKernelFalcon(GenericKernelFalcon *pThis) {

    // Initialize vtable(s) with 3 per-object function pointer(s).
    __nvoc_init_funcTable_GenericKernelFalcon_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__GenericKernelFalcon(GenericKernelFalcon *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_CrashCatEngine = &pThis->__nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine.__nvoc_base_CrashCatEngine;    // (crashcatEngine) super^3
    pThis->__nvoc_pbase_KernelCrashCatEngine = &pThis->__nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine;    // (kcrashcatEngine) super^2
    pThis->__nvoc_pbase_KernelFalcon = &pThis->__nvoc_base_KernelFalcon;    // (kflcn) super
    pThis->__nvoc_pbase_IntrService = &pThis->__nvoc_base_IntrService;    // (intrserv) super
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;    // (obj) super
    pThis->__nvoc_pbase_GenericKernelFalcon = pThis;    // (gkflcn) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__KernelFalcon(&pThis->__nvoc_base_KernelFalcon, pRmhalspecowner);
    __nvoc_init__IntrService(&pThis->__nvoc_base_IntrService);
    __nvoc_init__Object(&pThis->__nvoc_base_Object);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine.__nvoc_base_CrashCatEngine.__nvoc_metadata_ptr = &__nvoc_metadata__GenericKernelFalcon.metadata__KernelFalcon.metadata__KernelCrashCatEngine.metadata__CrashCatEngine;    // (crashcatEngine) super^3
    pThis->__nvoc_base_KernelFalcon.__nvoc_base_KernelCrashCatEngine.__nvoc_metadata_ptr = &__nvoc_metadata__GenericKernelFalcon.metadata__KernelFalcon.metadata__KernelCrashCatEngine;    // (kcrashcatEngine) super^2
    pThis->__nvoc_base_KernelFalcon.__nvoc_metadata_ptr = &__nvoc_metadata__GenericKernelFalcon.metadata__KernelFalcon;    // (kflcn) super
    pThis->__nvoc_base_IntrService.__nvoc_metadata_ptr = &__nvoc_metadata__GenericKernelFalcon.metadata__IntrService;    // (intrserv) super
    pThis->__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__GenericKernelFalcon.metadata__Object;    // (obj) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__GenericKernelFalcon;    // (gkflcn) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_GenericKernelFalcon(pThis);
}

NV_STATUS __nvoc_objCreate_GenericKernelFalcon(GenericKernelFalcon **ppThis, Dynamic *pParent, NvU32 createFlags, struct OBJGPU * arg_pGpu, KernelFalconEngineConfig * arg_pFalconConfig)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    GenericKernelFalcon *pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(GenericKernelFalcon), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(GenericKernelFalcon));

    pThis->__nvoc_base_Object.createFlags = createFlags;

    // pParent must be a valid object that derives from a halspec owner class.
    NV_ASSERT_OR_RETURN(pParent != NULL, NV_ERR_INVALID_ARGUMENT);

    // Link the child into the parent unless flagged not to do so.
    if (!(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_Object.pParent = NULL;
    }

    if ((pRmhalspecowner = dynamicCast(pParent, RmHalspecOwner)) == NULL)
        pRmhalspecowner = objFindAncestorOfType(RmHalspecOwner, pParent);
    NV_ASSERT_OR_RETURN(pRmhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT);

    __nvoc_init__GenericKernelFalcon(pThis, pRmhalspecowner);
    status = __nvoc_ctor_GenericKernelFalcon(pThis, pRmhalspecowner, arg_pGpu, arg_pFalconConfig);
    if (status != NV_OK) goto __nvoc_objCreate_GenericKernelFalcon_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_GenericKernelFalcon_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(GenericKernelFalcon));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_GenericKernelFalcon(GenericKernelFalcon **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct OBJGPU * arg_pGpu = va_arg(args, struct OBJGPU *);
    KernelFalconEngineConfig * arg_pFalconConfig = va_arg(args, KernelFalconEngineConfig *);

    status = __nvoc_objCreate_GenericKernelFalcon(ppThis, pParent, createFlags, arg_pGpu, arg_pFalconConfig);

    return status;
}

