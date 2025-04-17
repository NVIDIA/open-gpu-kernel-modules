#define NVOC_CRASHCAT_ENGINE_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_crashcat_engine_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0x654166 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_CrashCatEngine;

// Forward declarations for CrashCatEngine
void __nvoc_init__CrashCatEngine(CrashCatEngine*);
void __nvoc_init_funcTable_CrashCatEngine(CrashCatEngine*);
NV_STATUS __nvoc_ctor_CrashCatEngine(CrashCatEngine*);
void __nvoc_init_dataField_CrashCatEngine(CrashCatEngine*);
void __nvoc_dtor_CrashCatEngine(CrashCatEngine*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__CrashCatEngine;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__CrashCatEngine;

// Down-thunk(s) to bridge CrashCatEngine methods from ancestors (if any)

// Up-thunk(s) to bridge CrashCatEngine methods to ancestors (if any)

// Not instantiable because it's not derived from class "Object"
// Not instantiable because it's an abstract class with following pure virtual functions:
//  crashcatEngineConfigured
//  crashcatEngineVprintf
//  crashcatEnginePriRead
//  crashcatEnginePriWrite
//  crashcatEngineMapBufferDescriptor
//  crashcatEngineUnmapBufferDescriptor
//  crashcatEngineSyncBufferDescriptor
//  crashcatEngineGetScratchOffsets
//  crashcatEngineGetWFL0Offset
const struct NVOC_CLASS_DEF __nvoc_class_def_CrashCatEngine = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(CrashCatEngine),
        /*classId=*/            classId(CrashCatEngine),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "CrashCatEngine",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) NULL,
    /*pCastInfo=*/          &__nvoc_castinfo__CrashCatEngine,
    /*pExportInfo=*/        &__nvoc_export_info__CrashCatEngine
};


// Metadata with per-class RTTI and vtable
static const struct NVOC_METADATA__CrashCatEngine __nvoc_metadata__CrashCatEngine = {
    .rtti.pClassDef = &__nvoc_class_def_CrashCatEngine,    // (crashcatEngine) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_CrashCatEngine,
    .rtti.offset    = 0,

    .vtable.__crashcatEngineUnload__ = &crashcatEngineUnload_IMPL,    // virtual
    .vtable.__crashcatEngineConfigured__ = NULL,    // pure virtual
    .vtable.__crashcatEngineVprintf__ = NULL,    // pure virtual
    .vtable.__crashcatEnginePriRead__ = NULL,    // pure virtual
    .vtable.__crashcatEnginePriWrite__ = NULL,    // pure virtual
    .vtable.__crashcatEngineMapBufferDescriptor__ = NULL,    // pure virtual
    .vtable.__crashcatEngineUnmapBufferDescriptor__ = NULL,    // pure virtual
    .vtable.__crashcatEngineSyncBufferDescriptor__ = NULL,    // pure virtual
    .vtable.__crashcatEngineGetScratchOffsets__ = NULL,    // pure virtual
    .vtable.__crashcatEngineGetWFL0Offset__ = NULL,    // pure virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__CrashCatEngine = {
    .numRelatives = 1,
    .relatives = {
        &__nvoc_metadata__CrashCatEngine.rtti,    // [0]: (crashcatEngine) this
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__CrashCatEngine = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_CrashCatEngine(CrashCatEngine *pThis) {
    __nvoc_crashcatEngineDestruct(pThis);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_CrashCatEngine(CrashCatEngine *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_CrashCatEngine(CrashCatEngine *pThis) {
    NV_STATUS status = NV_OK;
    __nvoc_init_dataField_CrashCatEngine(pThis);

    status = __nvoc_crashcatEngineConstruct(pThis);
    if (status != NV_OK) goto __nvoc_ctor_CrashCatEngine_fail__init;
    goto __nvoc_ctor_CrashCatEngine_exit; // Success

__nvoc_ctor_CrashCatEngine_fail__init:
__nvoc_ctor_CrashCatEngine_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_CrashCatEngine_1(CrashCatEngine *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_CrashCatEngine_1


// Initialize vtable(s) for 10 virtual method(s).
void __nvoc_init_funcTable_CrashCatEngine(CrashCatEngine *pThis) {
    __nvoc_init_funcTable_CrashCatEngine_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__CrashCatEngine(CrashCatEngine *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_CrashCatEngine = pThis;    // (crashcatEngine) this

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__CrashCatEngine;    // (crashcatEngine) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_CrashCatEngine(pThis);
}

