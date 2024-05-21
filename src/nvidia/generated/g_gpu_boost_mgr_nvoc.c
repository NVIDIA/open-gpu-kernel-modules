#define NVOC_GPU_BOOST_MGR_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_gpu_boost_mgr_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x9f6bbf = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJGPUBOOSTMGR;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

void __nvoc_init_OBJGPUBOOSTMGR(OBJGPUBOOSTMGR*);
void __nvoc_init_funcTable_OBJGPUBOOSTMGR(OBJGPUBOOSTMGR*);
NV_STATUS __nvoc_ctor_OBJGPUBOOSTMGR(OBJGPUBOOSTMGR*);
void __nvoc_init_dataField_OBJGPUBOOSTMGR(OBJGPUBOOSTMGR*);
void __nvoc_dtor_OBJGPUBOOSTMGR(OBJGPUBOOSTMGR*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_OBJGPUBOOSTMGR;

static const struct NVOC_RTTI __nvoc_rtti_OBJGPUBOOSTMGR_OBJGPUBOOSTMGR = {
    /*pClassDef=*/          &__nvoc_class_def_OBJGPUBOOSTMGR,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_OBJGPUBOOSTMGR,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_OBJGPUBOOSTMGR_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(OBJGPUBOOSTMGR, __nvoc_base_Object),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_OBJGPUBOOSTMGR = {
    /*numRelatives=*/       2,
    /*relatives=*/ {
        &__nvoc_rtti_OBJGPUBOOSTMGR_OBJGPUBOOSTMGR,
        &__nvoc_rtti_OBJGPUBOOSTMGR_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_OBJGPUBOOSTMGR = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(OBJGPUBOOSTMGR),
        /*classId=*/            classId(OBJGPUBOOSTMGR),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "OBJGPUBOOSTMGR",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_OBJGPUBOOSTMGR,
    /*pCastInfo=*/          &__nvoc_castinfo_OBJGPUBOOSTMGR,
    /*pExportInfo=*/        &__nvoc_export_info_OBJGPUBOOSTMGR
};

const struct NVOC_EXPORT_INFO __nvoc_export_info_OBJGPUBOOSTMGR = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_Object(Object*);
void __nvoc_dtor_OBJGPUBOOSTMGR(OBJGPUBOOSTMGR *pThis) {
    __nvoc_gpuboostmgrDestruct(pThis);
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_OBJGPUBOOSTMGR(OBJGPUBOOSTMGR *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_Object(Object* );
NV_STATUS __nvoc_ctor_OBJGPUBOOSTMGR(OBJGPUBOOSTMGR *pThis) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_Object(&pThis->__nvoc_base_Object);
    if (status != NV_OK) goto __nvoc_ctor_OBJGPUBOOSTMGR_fail_Object;
    __nvoc_init_dataField_OBJGPUBOOSTMGR(pThis);

    status = __nvoc_gpuboostmgrConstruct(pThis);
    if (status != NV_OK) goto __nvoc_ctor_OBJGPUBOOSTMGR_fail__init;
    goto __nvoc_ctor_OBJGPUBOOSTMGR_exit; // Success

__nvoc_ctor_OBJGPUBOOSTMGR_fail__init:
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
__nvoc_ctor_OBJGPUBOOSTMGR_fail_Object:
__nvoc_ctor_OBJGPUBOOSTMGR_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_OBJGPUBOOSTMGR_1(OBJGPUBOOSTMGR *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_OBJGPUBOOSTMGR_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_OBJGPUBOOSTMGR(OBJGPUBOOSTMGR *pThis) {
    __nvoc_init_funcTable_OBJGPUBOOSTMGR_1(pThis);
}

void __nvoc_init_Object(Object*);
void __nvoc_init_OBJGPUBOOSTMGR(OBJGPUBOOSTMGR *pThis) {
    pThis->__nvoc_pbase_OBJGPUBOOSTMGR = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;
    __nvoc_init_Object(&pThis->__nvoc_base_Object);
    __nvoc_init_funcTable_OBJGPUBOOSTMGR(pThis);
}

NV_STATUS __nvoc_objCreate_OBJGPUBOOSTMGR(OBJGPUBOOSTMGR **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    OBJGPUBOOSTMGR *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(OBJGPUBOOSTMGR), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(OBJGPUBOOSTMGR));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_OBJGPUBOOSTMGR);

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

    __nvoc_init_OBJGPUBOOSTMGR(pThis);
    status = __nvoc_ctor_OBJGPUBOOSTMGR(pThis);
    if (status != NV_OK) goto __nvoc_objCreate_OBJGPUBOOSTMGR_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_OBJGPUBOOSTMGR_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(OBJGPUBOOSTMGR));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_OBJGPUBOOSTMGR(OBJGPUBOOSTMGR **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_OBJGPUBOOSTMGR(ppThis, pParent, createFlags);

    return status;
}

