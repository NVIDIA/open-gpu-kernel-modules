#define NVOC_KERNEL_VGPU_MGR_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_kernel_vgpu_mgr_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xa793dd = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelVgpuMgr;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

void __nvoc_init_KernelVgpuMgr(KernelVgpuMgr*);
void __nvoc_init_funcTable_KernelVgpuMgr(KernelVgpuMgr*);
NV_STATUS __nvoc_ctor_KernelVgpuMgr(KernelVgpuMgr*);
void __nvoc_init_dataField_KernelVgpuMgr(KernelVgpuMgr*);
void __nvoc_dtor_KernelVgpuMgr(KernelVgpuMgr*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelVgpuMgr;

static const struct NVOC_RTTI __nvoc_rtti_KernelVgpuMgr_KernelVgpuMgr = {
    /*pClassDef=*/          &__nvoc_class_def_KernelVgpuMgr,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelVgpuMgr,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_KernelVgpuMgr_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelVgpuMgr, __nvoc_base_Object),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_KernelVgpuMgr = {
    /*numRelatives=*/       2,
    /*relatives=*/ {
        &__nvoc_rtti_KernelVgpuMgr_KernelVgpuMgr,
        &__nvoc_rtti_KernelVgpuMgr_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_KernelVgpuMgr = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(KernelVgpuMgr),
        /*classId=*/            classId(KernelVgpuMgr),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "KernelVgpuMgr",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_KernelVgpuMgr,
    /*pCastInfo=*/          &__nvoc_castinfo_KernelVgpuMgr,
    /*pExportInfo=*/        &__nvoc_export_info_KernelVgpuMgr
};

const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelVgpuMgr = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_Object(Object*);
void __nvoc_dtor_KernelVgpuMgr(KernelVgpuMgr *pThis) {
    __nvoc_kvgpumgrDestruct(pThis);
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_KernelVgpuMgr(KernelVgpuMgr *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_Object(Object* );
NV_STATUS __nvoc_ctor_KernelVgpuMgr(KernelVgpuMgr *pThis) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_Object(&pThis->__nvoc_base_Object);
    if (status != NV_OK) goto __nvoc_ctor_KernelVgpuMgr_fail_Object;
    __nvoc_init_dataField_KernelVgpuMgr(pThis);

    status = __nvoc_kvgpumgrConstruct(pThis);
    if (status != NV_OK) goto __nvoc_ctor_KernelVgpuMgr_fail__init;
    goto __nvoc_ctor_KernelVgpuMgr_exit; // Success

__nvoc_ctor_KernelVgpuMgr_fail__init:
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
__nvoc_ctor_KernelVgpuMgr_fail_Object:
__nvoc_ctor_KernelVgpuMgr_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_KernelVgpuMgr_1(KernelVgpuMgr *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_KernelVgpuMgr_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_KernelVgpuMgr(KernelVgpuMgr *pThis) {
    __nvoc_init_funcTable_KernelVgpuMgr_1(pThis);
}

void __nvoc_init_Object(Object*);
void __nvoc_init_KernelVgpuMgr(KernelVgpuMgr *pThis) {
    pThis->__nvoc_pbase_KernelVgpuMgr = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;
    __nvoc_init_Object(&pThis->__nvoc_base_Object);
    __nvoc_init_funcTable_KernelVgpuMgr(pThis);
}

NV_STATUS __nvoc_objCreate_KernelVgpuMgr(KernelVgpuMgr **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    KernelVgpuMgr *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(KernelVgpuMgr), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(KernelVgpuMgr));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_KernelVgpuMgr);

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

    __nvoc_init_KernelVgpuMgr(pThis);
    status = __nvoc_ctor_KernelVgpuMgr(pThis);
    if (status != NV_OK) goto __nvoc_objCreate_KernelVgpuMgr_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_KernelVgpuMgr_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(KernelVgpuMgr));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelVgpuMgr(KernelVgpuMgr **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_KernelVgpuMgr(ppThis, pParent, createFlags);

    return status;
}

