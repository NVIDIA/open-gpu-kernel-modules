#define NVOC_KERNEL_CRASHCAT_ENGINE_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_kernel_crashcat_engine_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xc37aef = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelCrashCatEngine;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_CrashCatEngine;

void __nvoc_init_KernelCrashCatEngine(KernelCrashCatEngine*, RmHalspecOwner* );
void __nvoc_init_funcTable_KernelCrashCatEngine(KernelCrashCatEngine*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_KernelCrashCatEngine(KernelCrashCatEngine*, RmHalspecOwner* );
void __nvoc_init_dataField_KernelCrashCatEngine(KernelCrashCatEngine*, RmHalspecOwner* );
void __nvoc_dtor_KernelCrashCatEngine(KernelCrashCatEngine*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelCrashCatEngine;

static const struct NVOC_RTTI __nvoc_rtti_KernelCrashCatEngine_KernelCrashCatEngine = {
    /*pClassDef=*/          &__nvoc_class_def_KernelCrashCatEngine,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelCrashCatEngine,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_KernelCrashCatEngine_CrashCatEngine = {
    /*pClassDef=*/          &__nvoc_class_def_CrashCatEngine,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelCrashCatEngine, __nvoc_base_CrashCatEngine),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_KernelCrashCatEngine = {
    /*numRelatives=*/       2,
    /*relatives=*/ {
        &__nvoc_rtti_KernelCrashCatEngine_KernelCrashCatEngine,
        &__nvoc_rtti_KernelCrashCatEngine_CrashCatEngine,
    },
};

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
    /*pCastInfo=*/          &__nvoc_castinfo_KernelCrashCatEngine,
    /*pExportInfo=*/        &__nvoc_export_info_KernelCrashCatEngine
};

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


// Up-thunk(s) to bridge KernelCrashCatEngine methods to ancestors (if any)

const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelCrashCatEngine = 
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

    // Per-class vtable definition
    static const struct NVOC_VTABLE__KernelCrashCatEngine vtable = {
        .__kcrashcatEngineConfigured__ = &kcrashcatEngineConfigured_IMPL,    // virtual override (crashcatEngine) base (crashcatEngine)
        .CrashCatEngine.__crashcatEngineConfigured__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineConfigured,    // pure virtual
        .__kcrashcatEngineUnload__ = &kcrashcatEngineUnload_IMPL,    // virtual override (crashcatEngine) base (crashcatEngine)
        .CrashCatEngine.__crashcatEngineUnload__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineUnload,    // virtual
        .__kcrashcatEngineVprintf__ = &kcrashcatEngineVprintf_IMPL,    // virtual override (crashcatEngine) base (crashcatEngine)
        .CrashCatEngine.__crashcatEngineVprintf__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineVprintf,    // pure virtual
        .__kcrashcatEngineRegRead__ = NULL,    // pure virtual
        .__kcrashcatEngineRegWrite__ = NULL,    // pure virtual
        .__kcrashcatEngineMaskDmemAddr__ = NULL,    // pure virtual
        .__kcrashcatEnginePriRead__ = &kcrashcatEnginePriRead_IMPL,    // virtual override (crashcatEngine) base (crashcatEngine)
        .CrashCatEngine.__crashcatEnginePriRead__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEnginePriRead,    // pure virtual
        .__kcrashcatEnginePriWrite__ = &kcrashcatEnginePriWrite_IMPL,    // virtual override (crashcatEngine) base (crashcatEngine)
        .CrashCatEngine.__crashcatEnginePriWrite__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEnginePriWrite,    // pure virtual
        .__kcrashcatEngineMapBufferDescriptor__ = &kcrashcatEngineMapBufferDescriptor_IMPL,    // virtual override (crashcatEngine) base (crashcatEngine)
        .CrashCatEngine.__crashcatEngineMapBufferDescriptor__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineMapBufferDescriptor,    // pure virtual
        .__kcrashcatEngineUnmapBufferDescriptor__ = &kcrashcatEngineUnmapBufferDescriptor_IMPL,    // virtual override (crashcatEngine) base (crashcatEngine)
        .CrashCatEngine.__crashcatEngineUnmapBufferDescriptor__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineUnmapBufferDescriptor,    // pure virtual
        .__kcrashcatEngineSyncBufferDescriptor__ = &kcrashcatEngineSyncBufferDescriptor_IMPL,    // virtual override (crashcatEngine) base (crashcatEngine)
        .CrashCatEngine.__crashcatEngineSyncBufferDescriptor__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineSyncBufferDescriptor,    // pure virtual
        .__kcrashcatEngineReadDmem__ = &kcrashcatEngineReadDmem_TU102,    // virtual halified (singleton optimized)
        .__kcrashcatEngineReadEmem__ = &kcrashcatEngineReadEmem_2fced3,    // virtual halified (singleton optimized)
        .__kcrashcatEngineGetScratchOffsets__ = &kcrashcatEngineGetScratchOffsets_TU102,    // virtual halified (singleton optimized) override (crashcatEngine) base (crashcatEngine)
        .CrashCatEngine.__crashcatEngineGetScratchOffsets__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineGetScratchOffsets,    // pure virtual
        .__kcrashcatEngineGetWFL0Offset__ = &kcrashcatEngineGetWFL0Offset_TU102,    // virtual halified (singleton optimized) override (crashcatEngine) base (crashcatEngine)
        .CrashCatEngine.__crashcatEngineGetWFL0Offset__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineGetWFL0Offset,    // pure virtual
    };

    // Pointer(s) to per-class vtable(s)
    pThis->__nvoc_base_CrashCatEngine.__nvoc_vtable = &vtable.CrashCatEngine;    // (crashcatEngine) super
    pThis->__nvoc_vtable = &vtable;    // (kcrashcatEngine) this
    __nvoc_init_funcTable_KernelCrashCatEngine_1(pThis, pRmhalspecowner);
}

void __nvoc_init_CrashCatEngine(CrashCatEngine*);
void __nvoc_init_KernelCrashCatEngine(KernelCrashCatEngine *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_KernelCrashCatEngine = pThis;
    pThis->__nvoc_pbase_CrashCatEngine = &pThis->__nvoc_base_CrashCatEngine;
    __nvoc_init_CrashCatEngine(&pThis->__nvoc_base_CrashCatEngine);
    __nvoc_init_funcTable_KernelCrashCatEngine(pThis, pRmhalspecowner);
}

