#define NVOC_KERNEL_SCHED_MGR_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_kernel_sched_mgr_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xea0970 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelSchedMgr;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

void __nvoc_init_KernelSchedMgr(KernelSchedMgr*);
void __nvoc_init_funcTable_KernelSchedMgr(KernelSchedMgr*);
NV_STATUS __nvoc_ctor_KernelSchedMgr(KernelSchedMgr*);
void __nvoc_init_dataField_KernelSchedMgr(KernelSchedMgr*);
void __nvoc_dtor_KernelSchedMgr(KernelSchedMgr*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelSchedMgr;

static const struct NVOC_RTTI __nvoc_rtti_KernelSchedMgr_KernelSchedMgr = {
    /*pClassDef=*/          &__nvoc_class_def_KernelSchedMgr,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelSchedMgr,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_KernelSchedMgr_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelSchedMgr, __nvoc_base_Object),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_KernelSchedMgr = {
    /*numRelatives=*/       2,
    /*relatives=*/ {
        &__nvoc_rtti_KernelSchedMgr_KernelSchedMgr,
        &__nvoc_rtti_KernelSchedMgr_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_KernelSchedMgr = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(KernelSchedMgr),
        /*classId=*/            classId(KernelSchedMgr),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "KernelSchedMgr",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_KernelSchedMgr,
    /*pCastInfo=*/          &__nvoc_castinfo_KernelSchedMgr,
    /*pExportInfo=*/        &__nvoc_export_info_KernelSchedMgr
};

const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelSchedMgr = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_Object(Object*);
void __nvoc_dtor_KernelSchedMgr(KernelSchedMgr *pThis) {
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_KernelSchedMgr(KernelSchedMgr *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    pThis->bIsSchedSwEnabled = ((NvBool)(0 != 0));
}

NV_STATUS __nvoc_ctor_Object(Object* );
NV_STATUS __nvoc_ctor_KernelSchedMgr(KernelSchedMgr *pThis) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_Object(&pThis->__nvoc_base_Object);
    if (status != NV_OK) goto __nvoc_ctor_KernelSchedMgr_fail_Object;
    __nvoc_init_dataField_KernelSchedMgr(pThis);
    goto __nvoc_ctor_KernelSchedMgr_exit; // Success

__nvoc_ctor_KernelSchedMgr_fail_Object:
__nvoc_ctor_KernelSchedMgr_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_KernelSchedMgr_1(KernelSchedMgr *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_KernelSchedMgr_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_KernelSchedMgr(KernelSchedMgr *pThis) {
    __nvoc_init_funcTable_KernelSchedMgr_1(pThis);
}

void __nvoc_init_Object(Object*);
void __nvoc_init_KernelSchedMgr(KernelSchedMgr *pThis) {
    pThis->__nvoc_pbase_KernelSchedMgr = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;
    __nvoc_init_Object(&pThis->__nvoc_base_Object);
    __nvoc_init_funcTable_KernelSchedMgr(pThis);
}

NV_STATUS __nvoc_objCreate_KernelSchedMgr(KernelSchedMgr **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    KernelSchedMgr *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(KernelSchedMgr), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(KernelSchedMgr));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_KernelSchedMgr);

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

    __nvoc_init_KernelSchedMgr(pThis);
    status = __nvoc_ctor_KernelSchedMgr(pThis);
    if (status != NV_OK) goto __nvoc_objCreate_KernelSchedMgr_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_KernelSchedMgr_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(KernelSchedMgr));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelSchedMgr(KernelSchedMgr **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_KernelSchedMgr(ppThis, pParent, createFlags);

    return status;
}

