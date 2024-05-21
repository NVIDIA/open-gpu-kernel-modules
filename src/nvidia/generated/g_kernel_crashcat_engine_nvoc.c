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

// 10 down-thunk(s) defined to bridge methods in KernelCrashCatEngine from superclasses

// kcrashcatEngineConfigured: virtual override (crashcatEngine) base (crashcatEngine)
static NvBool __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineConfigured(struct CrashCatEngine *arg_this) {
    return kcrashcatEngineConfigured((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) - __nvoc_rtti_KernelCrashCatEngine_CrashCatEngine.offset));
}

// kcrashcatEngineUnload: virtual override (crashcatEngine) base (crashcatEngine)
static void __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineUnload(struct CrashCatEngine *arg_this) {
    kcrashcatEngineUnload((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) - __nvoc_rtti_KernelCrashCatEngine_CrashCatEngine.offset));
}

// kcrashcatEngineVprintf: virtual override (crashcatEngine) base (crashcatEngine)
static void __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineVprintf(struct CrashCatEngine *arg_this, NvBool bReportStart, const char *fmt, va_list args) {
    kcrashcatEngineVprintf((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) - __nvoc_rtti_KernelCrashCatEngine_CrashCatEngine.offset), bReportStart, fmt, args);
}

// kcrashcatEnginePriRead: virtual override (crashcatEngine) base (crashcatEngine)
static NvU32 __nvoc_down_thunk_KernelCrashCatEngine_crashcatEnginePriRead(struct CrashCatEngine *arg_this, NvU32 offset) {
    return kcrashcatEnginePriRead((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) - __nvoc_rtti_KernelCrashCatEngine_CrashCatEngine.offset), offset);
}

// kcrashcatEnginePriWrite: virtual override (crashcatEngine) base (crashcatEngine)
static void __nvoc_down_thunk_KernelCrashCatEngine_crashcatEnginePriWrite(struct CrashCatEngine *arg_this, NvU32 offset, NvU32 data) {
    kcrashcatEnginePriWrite((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) - __nvoc_rtti_KernelCrashCatEngine_CrashCatEngine.offset), offset, data);
}

// kcrashcatEngineMapBufferDescriptor: virtual override (crashcatEngine) base (crashcatEngine)
static void * __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineMapBufferDescriptor(struct CrashCatEngine *arg_this, CrashCatBufferDescriptor *pBufDesc) {
    return kcrashcatEngineMapBufferDescriptor((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) - __nvoc_rtti_KernelCrashCatEngine_CrashCatEngine.offset), pBufDesc);
}

// kcrashcatEngineUnmapBufferDescriptor: virtual override (crashcatEngine) base (crashcatEngine)
static void __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineUnmapBufferDescriptor(struct CrashCatEngine *arg_this, CrashCatBufferDescriptor *pBufDesc) {
    kcrashcatEngineUnmapBufferDescriptor((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) - __nvoc_rtti_KernelCrashCatEngine_CrashCatEngine.offset), pBufDesc);
}

// kcrashcatEngineSyncBufferDescriptor: virtual override (crashcatEngine) base (crashcatEngine)
static void __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineSyncBufferDescriptor(struct CrashCatEngine *arg_this, CrashCatBufferDescriptor *pBufDesc, NvU32 offset, NvU32 size) {
    kcrashcatEngineSyncBufferDescriptor((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) - __nvoc_rtti_KernelCrashCatEngine_CrashCatEngine.offset), pBufDesc, offset, size);
}

// kcrashcatEngineGetScratchOffsets: virtual halified (singleton optimized) override (crashcatEngine) base (crashcatEngine)
static const NvU32 * __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineGetScratchOffsets(struct CrashCatEngine *arg_this, NV_CRASHCAT_SCRATCH_GROUP_ID scratchGroupId) {
    return kcrashcatEngineGetScratchOffsets((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) - __nvoc_rtti_KernelCrashCatEngine_CrashCatEngine.offset), scratchGroupId);
}

// kcrashcatEngineGetWFL0Offset: virtual halified (singleton optimized) override (crashcatEngine) base (crashcatEngine)
static NvU32 __nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineGetWFL0Offset(struct CrashCatEngine *arg_this) {
    return kcrashcatEngineGetWFL0Offset((struct KernelCrashCatEngine *)(((unsigned char *) arg_this) - __nvoc_rtti_KernelCrashCatEngine_CrashCatEngine.offset));
}


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

    // kcrashcatEngineConfigured -- virtual override (crashcatEngine) base (crashcatEngine)
    pThis->__kcrashcatEngineConfigured__ = &kcrashcatEngineConfigured_IMPL;
    pThis->__nvoc_base_CrashCatEngine.__crashcatEngineConfigured__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineConfigured;

    // kcrashcatEngineUnload -- virtual override (crashcatEngine) base (crashcatEngine)
    pThis->__kcrashcatEngineUnload__ = &kcrashcatEngineUnload_IMPL;
    pThis->__nvoc_base_CrashCatEngine.__crashcatEngineUnload__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineUnload;

    // kcrashcatEngineVprintf -- virtual override (crashcatEngine) base (crashcatEngine)
    pThis->__kcrashcatEngineVprintf__ = &kcrashcatEngineVprintf_IMPL;
    pThis->__nvoc_base_CrashCatEngine.__crashcatEngineVprintf__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineVprintf;

    // kcrashcatEngineRegRead -- pure virtual
    pThis->__kcrashcatEngineRegRead__ = NULL;

    // kcrashcatEngineRegWrite -- pure virtual
    pThis->__kcrashcatEngineRegWrite__ = NULL;

    // kcrashcatEngineMaskDmemAddr -- pure virtual
    pThis->__kcrashcatEngineMaskDmemAddr__ = NULL;

    // kcrashcatEnginePriRead -- virtual override (crashcatEngine) base (crashcatEngine)
    pThis->__kcrashcatEnginePriRead__ = &kcrashcatEnginePriRead_IMPL;
    pThis->__nvoc_base_CrashCatEngine.__crashcatEnginePriRead__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEnginePriRead;

    // kcrashcatEnginePriWrite -- virtual override (crashcatEngine) base (crashcatEngine)
    pThis->__kcrashcatEnginePriWrite__ = &kcrashcatEnginePriWrite_IMPL;
    pThis->__nvoc_base_CrashCatEngine.__crashcatEnginePriWrite__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEnginePriWrite;

    // kcrashcatEngineMapBufferDescriptor -- virtual override (crashcatEngine) base (crashcatEngine)
    pThis->__kcrashcatEngineMapBufferDescriptor__ = &kcrashcatEngineMapBufferDescriptor_IMPL;
    pThis->__nvoc_base_CrashCatEngine.__crashcatEngineMapBufferDescriptor__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineMapBufferDescriptor;

    // kcrashcatEngineUnmapBufferDescriptor -- virtual override (crashcatEngine) base (crashcatEngine)
    pThis->__kcrashcatEngineUnmapBufferDescriptor__ = &kcrashcatEngineUnmapBufferDescriptor_IMPL;
    pThis->__nvoc_base_CrashCatEngine.__crashcatEngineUnmapBufferDescriptor__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineUnmapBufferDescriptor;

    // kcrashcatEngineSyncBufferDescriptor -- virtual override (crashcatEngine) base (crashcatEngine)
    pThis->__kcrashcatEngineSyncBufferDescriptor__ = &kcrashcatEngineSyncBufferDescriptor_IMPL;
    pThis->__nvoc_base_CrashCatEngine.__crashcatEngineSyncBufferDescriptor__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineSyncBufferDescriptor;

    // kcrashcatEngineReadDmem -- virtual halified (singleton optimized)
    pThis->__kcrashcatEngineReadDmem__ = &kcrashcatEngineReadDmem_TU102;

    // kcrashcatEngineReadEmem -- virtual halified (singleton optimized)
    pThis->__kcrashcatEngineReadEmem__ = &kcrashcatEngineReadEmem_2fced3;

    // kcrashcatEngineGetScratchOffsets -- virtual halified (singleton optimized) override (crashcatEngine) base (crashcatEngine)
    pThis->__kcrashcatEngineGetScratchOffsets__ = &kcrashcatEngineGetScratchOffsets_TU102;
    pThis->__nvoc_base_CrashCatEngine.__crashcatEngineGetScratchOffsets__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineGetScratchOffsets;

    // kcrashcatEngineGetWFL0Offset -- virtual halified (singleton optimized) override (crashcatEngine) base (crashcatEngine)
    pThis->__kcrashcatEngineGetWFL0Offset__ = &kcrashcatEngineGetWFL0Offset_TU102;
    pThis->__nvoc_base_CrashCatEngine.__crashcatEngineGetWFL0Offset__ = &__nvoc_down_thunk_KernelCrashCatEngine_crashcatEngineGetWFL0Offset;
} // End __nvoc_init_funcTable_KernelCrashCatEngine_1 with approximately 25 basic block(s).


// Initialize vtable(s) for 15 virtual method(s).
void __nvoc_init_funcTable_KernelCrashCatEngine(KernelCrashCatEngine *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize vtable(s) with 15 per-object function pointer(s).
    __nvoc_init_funcTable_KernelCrashCatEngine_1(pThis, pRmhalspecowner);
}

void __nvoc_init_CrashCatEngine(CrashCatEngine*);
void __nvoc_init_KernelCrashCatEngine(KernelCrashCatEngine *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_KernelCrashCatEngine = pThis;
    pThis->__nvoc_pbase_CrashCatEngine = &pThis->__nvoc_base_CrashCatEngine;
    __nvoc_init_CrashCatEngine(&pThis->__nvoc_base_CrashCatEngine);
    __nvoc_init_funcTable_KernelCrashCatEngine(pThis, pRmhalspecowner);
}

