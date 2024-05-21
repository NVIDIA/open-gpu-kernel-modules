#define NVOC_GPU_ACCT_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_gpu_acct_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x0f1350 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuAccounting;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

void __nvoc_init_GpuAccounting(GpuAccounting*);
void __nvoc_init_funcTable_GpuAccounting(GpuAccounting*);
NV_STATUS __nvoc_ctor_GpuAccounting(GpuAccounting*);
void __nvoc_init_dataField_GpuAccounting(GpuAccounting*);
void __nvoc_dtor_GpuAccounting(GpuAccounting*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_GpuAccounting;

static const struct NVOC_RTTI __nvoc_rtti_GpuAccounting_GpuAccounting = {
    /*pClassDef=*/          &__nvoc_class_def_GpuAccounting,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_GpuAccounting,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_GpuAccounting_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(GpuAccounting, __nvoc_base_Object),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_GpuAccounting = {
    /*numRelatives=*/       2,
    /*relatives=*/ {
        &__nvoc_rtti_GpuAccounting_GpuAccounting,
        &__nvoc_rtti_GpuAccounting_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_GpuAccounting = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(GpuAccounting),
        /*classId=*/            classId(GpuAccounting),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "GpuAccounting",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_GpuAccounting,
    /*pCastInfo=*/          &__nvoc_castinfo_GpuAccounting,
    /*pExportInfo=*/        &__nvoc_export_info_GpuAccounting
};

const struct NVOC_EXPORT_INFO __nvoc_export_info_GpuAccounting = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_Object(Object*);
void __nvoc_dtor_GpuAccounting(GpuAccounting *pThis) {
    __nvoc_gpuacctDestruct(pThis);
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_GpuAccounting(GpuAccounting *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_Object(Object* );
NV_STATUS __nvoc_ctor_GpuAccounting(GpuAccounting *pThis) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_Object(&pThis->__nvoc_base_Object);
    if (status != NV_OK) goto __nvoc_ctor_GpuAccounting_fail_Object;
    __nvoc_init_dataField_GpuAccounting(pThis);

    status = __nvoc_gpuacctConstruct(pThis);
    if (status != NV_OK) goto __nvoc_ctor_GpuAccounting_fail__init;
    goto __nvoc_ctor_GpuAccounting_exit; // Success

__nvoc_ctor_GpuAccounting_fail__init:
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
__nvoc_ctor_GpuAccounting_fail_Object:
__nvoc_ctor_GpuAccounting_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_GpuAccounting_1(GpuAccounting *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_GpuAccounting_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_GpuAccounting(GpuAccounting *pThis) {
    __nvoc_init_funcTable_GpuAccounting_1(pThis);
}

void __nvoc_init_Object(Object*);
void __nvoc_init_GpuAccounting(GpuAccounting *pThis) {
    pThis->__nvoc_pbase_GpuAccounting = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;
    __nvoc_init_Object(&pThis->__nvoc_base_Object);
    __nvoc_init_funcTable_GpuAccounting(pThis);
}

NV_STATUS __nvoc_objCreate_GpuAccounting(GpuAccounting **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    GpuAccounting *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(GpuAccounting), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(GpuAccounting));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_GpuAccounting);

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

    __nvoc_init_GpuAccounting(pThis);
    status = __nvoc_ctor_GpuAccounting(pThis);
    if (status != NV_OK) goto __nvoc_objCreate_GpuAccounting_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_GpuAccounting_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(GpuAccounting));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_GpuAccounting(GpuAccounting **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_GpuAccounting(ppThis, pParent, createFlags);

    return status;
}

