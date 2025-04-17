#define NVOC_KERNEL_CRASHCAT_ENGINE_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_kernel_crashcat_engine_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0xc37aef = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelCrashCatEngine;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_CrashCatEngine;

// Forward declarations for KernelCrashCatEngine
void __nvoc_init__CrashCatEngine(CrashCatEngine*);
void __nvoc_init__KernelCrashCatEngine(KernelCrashCatEngine*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_funcTable_KernelCrashCatEngine(KernelCrashCatEngine*, RmHalspecOwner *pRmhalspecowner);
NV_STATUS __nvoc_ctor_KernelCrashCatEngine(KernelCrashCatEngine*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_dataField_KernelCrashCatEngine(KernelCrashCatEngine*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_dtor_KernelCrashCatEngine(KernelCrashCatEngine*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__KernelCrashCatEngine;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelCrashCatEngine;

// Down-thunk(s) to bridge KernelCrashCatEngine methods from ancestors (if any)
NvBool __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineConfigured(struct CrashCatEngine *arg_this);    // this
void __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineUnload(struct CrashCatEngine *arg_this);    // this
void __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineVprintf(struct CrashCatEngine *arg_this, NvBool bReportStart, const char *fmt, va_list args);    // this
NvU32 __nvoc_down_thunk_KernelCrashCatEngine_crashcatEnginePriRead(struct CrashCatEngine *arg_this, NvU32 offset);    // this
void __nvoc_down_thunk_KernelCrashCatEngine_crashcatEnginePriWrite(struct CrashCatEngine *arg_this, NvU32 offset, NvU32 data);    // this
void * __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineMapBufferDescriptor(struct CrashCatEngine *arg_this, CrashCatBufferDescriptor *pBufDesc);    // this
void __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineUnmapBufferDescriptor(struct CrashCatEngine *arg_this, CrashCatBufferDescriptor *pBufDesc);    // this
void __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineSyncBufferDescriptor(struct CrashCatEngine *arg_this, CrashCatBufferDescriptor *pBufDesc, NvU32 offset, NvU32 size);    // this
const NvU32 * __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineGetScratchOffsets(struct CrashCatEngine *arg_this, NV_CRASHCAT_SCRATCH_GROUP_ID scratchGroupId);    // this
NvU32 __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineGetWFL0Offset(struct CrashCatEngine *arg_this);    // this

// Up-thunk(s) to bridge KernelCrashCatEngine methods to ancestors (if any)

// Not instantiable because it's not derived from class "Object"
// Not instantiable because it's an abstract class with following pure virtual functions:
//  kcrashcatEngineRegRead
//  kcrashcatEngineRegWrite
//  kcrashcatEngineMaskDmemAddr
const struct NVOC_CLASS_DEF __nvoc_class_def_KernelCrashCatEngine = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(KernelCrashCatEngine),
        /*classId=*/            classId(KernelCrashCatEngine),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "KernelCrashCatEngine",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) NULL,
    /*pCastInfo=*/          &__nvoc_castinfo__KernelCrashCatEngine,
    /*pExportInfo=*/        &__nvoc_export_info__KernelCrashCatEngine
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__KernelCrashCatEngine __nvoc_metadata__KernelCrashCatEngine = {
    .rtti.pClassDef = &__nvoc_class_def_KernelCrashCatEngine,    // (kcrashcatEngine) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelCrashCatEngine,
    .rtti.offset    = 0,
    .metadata__CrashCatEngine.rtti.pClassDef = &__nvoc_class_def_CrashCatEngine,    // (crashcatEngine) super
    .metadata__CrashCatEngine.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__CrashCatEngine.rtti.offset    = NV_OFFSETOF(KernelCrashCatEngine, __nvoc_base_CrashCatEngine),

    .vtable.__kcrashcatEngineConfigured__ = &kcrashcatEngineConfigured_IMPL,    // virtual override (crashcatEngine) base (crashcatEngine)
    .metadata__CrashCatEngine.vtable.__crashcatEngineConfigured__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineConfigured,    // pure virtual
    .vtable.__kcrashcatEngineUnload__ = &kcrashcatEngineUnload_IMPL,    // virtual override (crashcatEngine) base (crashcatEngine)
    .metadata__CrashCatEngine.vtable.__crashcatEngineUnload__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineUnload,    // virtual
    .vtable.__kcrashcatEngineVprintf__ = &kcrashcatEngineVprintf_IMPL,    // virtual override (crashcatEngine) base (crashcatEngine)
    .metadata__CrashCatEngine.vtable.__crashcatEngineVprintf__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineVprintf,    // pure virtual
    .vtable.__kcrashcatEngineRegRead__ = NULL,    // pure virtual
    .vtable.__kcrashcatEngineRegWrite__ = NULL,    // pure virtual
    .vtable.__kcrashcatEngineMaskDmemAddr__ = NULL,    // pure virtual
    .vtable.__kcrashcatEnginePriRead__ = &kcrashcatEnginePriRead_IMPL,    // virtual override (crashcatEngine) base (crashcatEngine)
    .metadata__CrashCatEngine.vtable.__crashcatEnginePriRead__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEnginePriRead,    // pure virtual
    .vtable.__kcrashcatEnginePriWrite__ = &kcrashcatEnginePriWrite_IMPL,    // virtual override (crashcatEngine) base (crashcatEngine)
    .metadata__CrashCatEngine.vtable.__crashcatEnginePriWrite__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEnginePriWrite,    // pure virtual
    .vtable.__kcrashcatEngineMapBufferDescriptor__ = &kcrashcatEngineMapBufferDescriptor_IMPL,    // virtual override (crashcatEngine) base (crashcatEngine)
    .metadata__CrashCatEngine.vtable.__crashcatEngineMapBufferDescriptor__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineMapBufferDescriptor,    // pure virtual
    .vtable.__kcrashcatEngineUnmapBufferDescriptor__ = &kcrashcatEngineUnmapBufferDescriptor_IMPL,    // virtual override (crashcatEngine) base (crashcatEngine)
    .metadata__CrashCatEngine.vtable.__crashcatEngineUnmapBufferDescriptor__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineUnmapBufferDescriptor,    // pure virtual
    .vtable.__kcrashcatEngineSyncBufferDescriptor__ = &kcrashcatEngineSyncBufferDescriptor_IMPL,    // virtual override (crashcatEngine) base (crashcatEngine)
    .metadata__CrashCatEngine.vtable.__crashcatEngineSyncBufferDescriptor__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineSyncBufferDescriptor,    // pure virtual
    .vtable.__kcrashcatEngineReadDmem__ = &kcrashcatEngineReadDmem_TU102,    // virtual halified (singleton optimized)
    .vtable.__kcrashcatEngineReadEmem__ = &kcrashcatEngineReadEmem_2fced3,    // virtual halified (singleton optimized)
    .vtable.__kcrashcatEngineGetScratchOffsets__ = &kcrashcatEngineGetScratchOffsets_TU102,    // virtual halified (singleton optimized) override (crashcatEngine) base (crashcatEngine)
    .metadata__CrashCatEngine.vtable.__crashcatEngineGetScratchOffsets__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineGetScratchOffsets,    // pure virtual
    .vtable.__kcrashcatEngineGetWFL0Offset__ = &kcrashcatEngineGetWFL0Offset_TU102,    // virtual halified (singleton optimized) override (crashcatEngine) base (crashcatEngine)
    .metadata__CrashCatEngine.vtable.__crashcatEngineGetWFL0Offset__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineGetWFL0Offset,    // pure virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__KernelCrashCatEngine = {
    .numRelatives = 2,
    .relatives = {
        &__nvoc_metadata__KernelCrashCatEngine.rtti,    // [0]: (kcrashcatEngine) this
        &__nvoc_metadata__KernelCrashCatEngine.metadata__CrashCatEngine.rtti,    // [1]: (crashcatEngine) super
    }
};

// 10 down-thunk(s) defined to bridge methods in KernelCrashCatEngine from superclasses

// kcrashcatEngineConfigured: virtual override (crashcatEngine) base (crashcatEngine)
NvBool __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineConfigured(struct CrashCatEngine *arg_this) {
    return kcrashcatEngineConfigured((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) - NV_OFFSETOF(KernelCrashCatEngine, __nvoc_base_CrashCatEngine)));
}

// kcrashcatEngineUnload: virtual override (crashcatEngine) base (crashcatEngine)
void __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineUnload(struct CrashCatEngine *arg_this) {
    kcrashcatEngineUnload((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) - NV_OFFSETOF(KernelCrashCatEngine, __nvoc_base_CrashCatEngine)));
}

// kcrashcatEngineVprintf: virtual override (crashcatEngine) base (crashcatEngine)
void __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineVprintf(struct CrashCatEngine *arg_this, NvBool bReportStart, const char *fmt, va_list args) {
    kcrashcatEngineVprintf((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) - NV_OFFSETOF(KernelCrashCatEngine, __nvoc_base_CrashCatEngine)), bReportStart, fmt, args);
}

// kcrashcatEnginePriRead: virtual override (crashcatEngine) base (crashcatEngine)
NvU32 __nvoc_down_thunk_KernelCrashCatEngine_crashcatEnginePriRead(struct CrashCatEngine *arg_this, NvU32 offset) {
    return kcrashcatEnginePriRead((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) - NV_OFFSETOF(KernelCrashCatEngine, __nvoc_base_CrashCatEngine)), offset);
}

// kcrashcatEnginePriWrite: virtual override (crashcatEngine) base (crashcatEngine)
void __nvoc_down_thunk_KernelCrashCatEngine_crashcatEnginePriWrite(struct CrashCatEngine *arg_this, NvU32 offset, NvU32 data) {
    kcrashcatEnginePriWrite((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) - NV_OFFSETOF(KernelCrashCatEngine, __nvoc_base_CrashCatEngine)), offset, data);
}

// kcrashcatEngineMapBufferDescriptor: virtual override (crashcatEngine) base (crashcatEngine)
void * __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineMapBufferDescriptor(struct CrashCatEngine *arg_this, CrashCatBufferDescriptor *pBufDesc) {
    return kcrashcatEngineMapBufferDescriptor((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) - NV_OFFSETOF(KernelCrashCatEngine, __nvoc_base_CrashCatEngine)), pBufDesc);
}

// kcrashcatEngineUnmapBufferDescriptor: virtual override (crashcatEngine) base (crashcatEngine)
void __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineUnmapBufferDescriptor(struct CrashCatEngine *arg_this, CrashCatBufferDescriptor *pBufDesc) {
    kcrashcatEngineUnmapBufferDescriptor((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) - NV_OFFSETOF(KernelCrashCatEngine, __nvoc_base_CrashCatEngine)), pBufDesc);
}

// kcrashcatEngineSyncBufferDescriptor: virtual override (crashcatEngine) base (crashcatEngine)
void __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineSyncBufferDescriptor(struct CrashCatEngine *arg_this, CrashCatBufferDescriptor *pBufDesc, NvU32 offset, NvU32 size) {
    kcrashcatEngineSyncBufferDescriptor((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) - NV_OFFSETOF(KernelCrashCatEngine, __nvoc_base_CrashCatEngine)), pBufDesc, offset, size);
}

// kcrashcatEngineGetScratchOffsets: virtual halified (singleton optimized) override (crashcatEngine) base (crashcatEngine)
const NvU32 * __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineGetScratchOffsets(struct CrashCatEngine *arg_this, NV_CRASHCAT_SCRATCH_GROUP_ID scratchGroupId) {
    return kcrashcatEngineGetScratchOffsets((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) - NV_OFFSETOF(KernelCrashCatEngine, __nvoc_base_CrashCatEngine)), scratchGroupId);
}

// kcrashcatEngineGetWFL0Offset: virtual halified (singleton optimized) override (crashcatEngine) base (crashcatEngine)
NvU32 __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineGetWFL0Offset(struct CrashCatEngine *arg_this) {
    return kcrashcatEngineGetWFL0Offset((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) - NV_OFFSETOF(KernelCrashCatEngine, __nvoc_base_CrashCatEngine)));
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelCrashCatEngine = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_CrashCatEngine(CrashCatEngine*);
void __nvoc_dtor_KernelCrashCatEngine(KernelCrashCatEngine *pThis) {
    __nvoc_dtor_CrashCatEngine(&pThis->__nvoc_base_CrashCatEngine);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_KernelCrashCatEngine(KernelCrashCatEngine *pThis, RmHalspecOwner *pRmhalspecowner) {
    ChipHal *chipHal = &pRmhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);
}

NV_STATUS __nvoc_ctor_CrashCatEngine(CrashCatEngine* );
NV_STATUS __nvoc_ctor_KernelCrashCatEngine(KernelCrashCatEngine *pThis, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_CrashCatEngine(&pThis->__nvoc_base_CrashCatEngine);
    if (status != NV_OK) goto __nvoc_ctor_KernelCrashCatEngine_fail_CrashCatEngine;
    __nvoc_init_dataField_KernelCrashCatEngine(pThis, pRmhalspecowner);
    goto __nvoc_ctor_KernelCrashCatEngine_exit; // Success

__nvoc_ctor_KernelCrashCatEngine_fail_CrashCatEngine:
__nvoc_ctor_KernelCrashCatEngine_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_KernelCrashCatEngine_1(KernelCrashCatEngine *pThis, RmHalspecOwner *pRmhalspecowner) {
    ChipHal *chipHal = &pRmhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);
} // End __nvoc_init_funcTable_KernelCrashCatEngine_1


// Initialize vtable(s) for 15 virtual method(s).
void __nvoc_init_funcTable_KernelCrashCatEngine(KernelCrashCatEngine *pThis, RmHalspecOwner *pRmhalspecowner) {
    __nvoc_init_funcTable_KernelCrashCatEngine_1(pThis, pRmhalspecowner);
}

// Initialize newly constructed object.
void __nvoc_init__KernelCrashCatEngine(KernelCrashCatEngine *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_CrashCatEngine = &pThis->__nvoc_base_CrashCatEngine;    // (crashcatEngine) super
    pThis->__nvoc_pbase_KernelCrashCatEngine = pThis;    // (kcrashcatEngine) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__CrashCatEngine(&pThis->__nvoc_base_CrashCatEngine);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_CrashCatEngine.__nvoc_metadata_ptr = &__nvoc_metadata__KernelCrashCatEngine.metadata__CrashCatEngine;    // (crashcatEngine) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__KernelCrashCatEngine;    // (kcrashcatEngine) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_KernelCrashCatEngine(pThis, pRmhalspecowner);
}

