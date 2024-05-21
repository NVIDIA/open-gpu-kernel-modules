#define NVOC_CRASHCAT_ENGINE_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_crashcat_engine_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x654166 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_CrashCatEngine;

void __nvoc_init_CrashCatEngine(CrashCatEngine*);
void __nvoc_init_funcTable_CrashCatEngine(CrashCatEngine*);
NV_STATUS __nvoc_ctor_CrashCatEngine(CrashCatEngine*);
void __nvoc_init_dataField_CrashCatEngine(CrashCatEngine*);
void __nvoc_dtor_CrashCatEngine(CrashCatEngine*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_CrashCatEngine;

static const struct NVOC_RTTI __nvoc_rtti_CrashCatEngine_CrashCatEngine = {
    /*pClassDef=*/          &__nvoc_class_def_CrashCatEngine,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_CrashCatEngine,
    /*offset=*/             0,
};

static const struct NVOC_CASTINFO __nvoc_castinfo_CrashCatEngine = {
    /*numRelatives=*/       1,
    /*relatives=*/ {
        &__nvoc_rtti_CrashCatEngine_CrashCatEngine,
    },
};

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
    /*pCastInfo=*/          &__nvoc_castinfo_CrashCatEngine,
    /*pExportInfo=*/        &__nvoc_export_info_CrashCatEngine
};

const struct NVOC_EXPORT_INFO __nvoc_export_info_CrashCatEngine = 
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

    // crashcatEngineUnload -- virtual
    pThis->__crashcatEngineUnload__ = &crashcatEngineUnload_IMPL;

    // crashcatEngineConfigured -- pure virtual
    pThis->__crashcatEngineConfigured__ = NULL;

    // crashcatEngineVprintf -- pure virtual
    pThis->__crashcatEngineVprintf__ = NULL;

    // crashcatEnginePriRead -- pure virtual
    pThis->__crashcatEnginePriRead__ = NULL;

    // crashcatEnginePriWrite -- pure virtual
    pThis->__crashcatEnginePriWrite__ = NULL;

    // crashcatEngineMapBufferDescriptor -- pure virtual
    pThis->__crashcatEngineMapBufferDescriptor__ = NULL;

    // crashcatEngineUnmapBufferDescriptor -- pure virtual
    pThis->__crashcatEngineUnmapBufferDescriptor__ = NULL;

    // crashcatEngineSyncBufferDescriptor -- pure virtual
    pThis->__crashcatEngineSyncBufferDescriptor__ = NULL;

    // crashcatEngineGetScratchOffsets -- pure virtual
    pThis->__crashcatEngineGetScratchOffsets__ = NULL;

    // crashcatEngineGetWFL0Offset -- pure virtual
    pThis->__crashcatEngineGetWFL0Offset__ = NULL;
} // End __nvoc_init_funcTable_CrashCatEngine_1 with approximately 10 basic block(s).


// Initialize vtable(s) for 10 virtual method(s).
void __nvoc_init_funcTable_CrashCatEngine(CrashCatEngine *pThis) {

    // Initialize vtable(s) with 10 per-object function pointer(s).
    __nvoc_init_funcTable_CrashCatEngine_1(pThis);
}

void __nvoc_init_CrashCatEngine(CrashCatEngine *pThis) {
    pThis->__nvoc_pbase_CrashCatEngine = pThis;
    __nvoc_init_funcTable_CrashCatEngine(pThis);
}

