#define NVOC_CRASHCAT_WAYFINDER_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_crashcat_wayfinder_nvoc.h"

void __nvoc_init_halspec_CrashCatWayfinderHal(CrashCatWayfinderHal *pCrashCatWayfinderHal, NV_CRASHCAT_WAYFINDER_VERSION version)
{
    // V1
    if(version == 0x1)
    {
        pCrashCatWayfinderHal->__nvoc_HalVarIdx = 0;
    }
}

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x085e32 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_CrashCatWayfinder;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

void __nvoc_init_CrashCatWayfinder(CrashCatWayfinder*,
        NV_CRASHCAT_WAYFINDER_VERSION CrashCatWayfinderHal_version);
void __nvoc_init_funcTable_CrashCatWayfinder(CrashCatWayfinder*);
NV_STATUS __nvoc_ctor_CrashCatWayfinder(CrashCatWayfinder*, NvU32 arg_wfl0);
void __nvoc_init_dataField_CrashCatWayfinder(CrashCatWayfinder*);
void __nvoc_dtor_CrashCatWayfinder(CrashCatWayfinder*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_CrashCatWayfinder;

static const struct NVOC_RTTI __nvoc_rtti_CrashCatWayfinder_CrashCatWayfinder = {
    /*pClassDef=*/          &__nvoc_class_def_CrashCatWayfinder,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_CrashCatWayfinder,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_CrashCatWayfinder_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(CrashCatWayfinder, __nvoc_base_Object),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_CrashCatWayfinder = {
    /*numRelatives=*/       2,
    /*relatives=*/ {
        &__nvoc_rtti_CrashCatWayfinder_CrashCatWayfinder,
        &__nvoc_rtti_CrashCatWayfinder_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_CrashCatWayfinder = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(CrashCatWayfinder),
        /*classId=*/            classId(CrashCatWayfinder),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "CrashCatWayfinder",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_CrashCatWayfinder,
    /*pCastInfo=*/          &__nvoc_castinfo_CrashCatWayfinder,
    /*pExportInfo=*/        &__nvoc_export_info_CrashCatWayfinder
};

const struct NVOC_EXPORT_INFO __nvoc_export_info_CrashCatWayfinder = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_Object(Object*);
void __nvoc_dtor_CrashCatWayfinder(CrashCatWayfinder *pThis) {
    __nvoc_crashcatWayfinderDestruct(pThis);
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_CrashCatWayfinder(CrashCatWayfinder *pThis) {
    CrashCatWayfinderHal *wayfinderHal = &pThis->wayfinderHal;
    const unsigned long wayfinderHal_HalVarIdx = (unsigned long)wayfinderHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(wayfinderHal);
    PORT_UNREFERENCED_VARIABLE(wayfinderHal_HalVarIdx);
}

NV_STATUS __nvoc_ctor_Object(Object* );
NV_STATUS __nvoc_ctor_CrashCatWayfinder(CrashCatWayfinder *pThis, NvU32 arg_wfl0) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_Object(&pThis->__nvoc_base_Object);
    if (status != NV_OK) goto __nvoc_ctor_CrashCatWayfinder_fail_Object;
    __nvoc_init_dataField_CrashCatWayfinder(pThis);

    status = __nvoc_crashcatWayfinderConstruct(pThis, arg_wfl0);
    if (status != NV_OK) goto __nvoc_ctor_CrashCatWayfinder_fail__init;
    goto __nvoc_ctor_CrashCatWayfinder_exit; // Success

__nvoc_ctor_CrashCatWayfinder_fail__init:
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
__nvoc_ctor_CrashCatWayfinder_fail_Object:
__nvoc_ctor_CrashCatWayfinder_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_CrashCatWayfinder_1(CrashCatWayfinder *pThis) {
    CrashCatWayfinderHal *wayfinderHal = &pThis->wayfinderHal;
    const unsigned long wayfinderHal_HalVarIdx = (unsigned long)wayfinderHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(wayfinderHal);
    PORT_UNREFERENCED_VARIABLE(wayfinderHal_HalVarIdx);
} // End __nvoc_init_funcTable_CrashCatWayfinder_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_CrashCatWayfinder(CrashCatWayfinder *pThis) {
    __nvoc_init_funcTable_CrashCatWayfinder_1(pThis);
}

void __nvoc_init_Object(Object*);
void __nvoc_init_CrashCatWayfinder(CrashCatWayfinder *pThis,
        NV_CRASHCAT_WAYFINDER_VERSION CrashCatWayfinderHal_version) {
    pThis->__nvoc_pbase_CrashCatWayfinder = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;
    __nvoc_init_Object(&pThis->__nvoc_base_Object);
    __nvoc_init_halspec_CrashCatWayfinderHal(&pThis->wayfinderHal, CrashCatWayfinderHal_version);
    __nvoc_init_funcTable_CrashCatWayfinder(pThis);
}

NV_STATUS __nvoc_objCreate_CrashCatWayfinder(CrashCatWayfinder **ppThis, Dynamic *pParent, NvU32 createFlags,
        NV_CRASHCAT_WAYFINDER_VERSION CrashCatWayfinderHal_version, NvU32 arg_wfl0)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    CrashCatWayfinder *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(CrashCatWayfinder), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(CrashCatWayfinder));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_CrashCatWayfinder);

    pThis->__nvoc_base_Object.createFlags = createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init_CrashCatWayfinder(pThis, CrashCatWayfinderHal_version);
    status = __nvoc_ctor_CrashCatWayfinder(pThis, arg_wfl0);
    if (status != NV_OK) goto __nvoc_objCreate_CrashCatWayfinder_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_CrashCatWayfinder_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(CrashCatWayfinder));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_CrashCatWayfinder(CrashCatWayfinder **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    NV_CRASHCAT_WAYFINDER_VERSION CrashCatWayfinderHal_version = va_arg(args, NV_CRASHCAT_WAYFINDER_VERSION);
    NvU32 arg_wfl0 = va_arg(args, NvU32);

    status = __nvoc_objCreate_CrashCatWayfinder(ppThis, pParent, createFlags, CrashCatWayfinderHal_version, arg_wfl0);

    return status;
}

