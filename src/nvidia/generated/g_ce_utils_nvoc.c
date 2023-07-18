#define NVOC_CE_UTILS_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_ce_utils_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x8b8bae = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_CeUtils;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

void __nvoc_init_CeUtils(CeUtils*);
void __nvoc_init_funcTable_CeUtils(CeUtils*);
NV_STATUS __nvoc_ctor_CeUtils(CeUtils*, struct OBJGPU * arg_pGpu, KERNEL_MIG_GPU_INSTANCE * arg_pKernelMIGGPUInstance, NV0050_ALLOCATION_PARAMETERS * arg_pAllocParams);
void __nvoc_init_dataField_CeUtils(CeUtils*);
void __nvoc_dtor_CeUtils(CeUtils*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_CeUtils;

static const struct NVOC_RTTI __nvoc_rtti_CeUtils_CeUtils = {
    /*pClassDef=*/          &__nvoc_class_def_CeUtils,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_CeUtils,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_CeUtils_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(CeUtils, __nvoc_base_Object),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_CeUtils = {
    /*numRelatives=*/       2,
    /*relatives=*/ {
        &__nvoc_rtti_CeUtils_CeUtils,
        &__nvoc_rtti_CeUtils_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_CeUtils = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(CeUtils),
        /*classId=*/            classId(CeUtils),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "CeUtils",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_CeUtils,
    /*pCastInfo=*/          &__nvoc_castinfo_CeUtils,
    /*pExportInfo=*/        &__nvoc_export_info_CeUtils
};

const struct NVOC_EXPORT_INFO __nvoc_export_info_CeUtils = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_Object(Object*);
void __nvoc_dtor_CeUtils(CeUtils *pThis) {
    __nvoc_ceutilsDestruct(pThis);
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_CeUtils(CeUtils *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_Object(Object* );
NV_STATUS __nvoc_ctor_CeUtils(CeUtils *pThis, struct OBJGPU * arg_pGpu, KERNEL_MIG_GPU_INSTANCE * arg_pKernelMIGGPUInstance, NV0050_ALLOCATION_PARAMETERS * arg_pAllocParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_Object(&pThis->__nvoc_base_Object);
    if (status != NV_OK) goto __nvoc_ctor_CeUtils_fail_Object;
    __nvoc_init_dataField_CeUtils(pThis);

    status = __nvoc_ceutilsConstruct(pThis, arg_pGpu, arg_pKernelMIGGPUInstance, arg_pAllocParams);
    if (status != NV_OK) goto __nvoc_ctor_CeUtils_fail__init;
    goto __nvoc_ctor_CeUtils_exit; // Success

__nvoc_ctor_CeUtils_fail__init:
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
__nvoc_ctor_CeUtils_fail_Object:
__nvoc_ctor_CeUtils_exit:

    return status;
}

static void __nvoc_init_funcTable_CeUtils_1(CeUtils *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_funcTable_CeUtils(CeUtils *pThis) {
    __nvoc_init_funcTable_CeUtils_1(pThis);
}

void __nvoc_init_Object(Object*);
void __nvoc_init_CeUtils(CeUtils *pThis) {
    pThis->__nvoc_pbase_CeUtils = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;
    __nvoc_init_Object(&pThis->__nvoc_base_Object);
    __nvoc_init_funcTable_CeUtils(pThis);
}

NV_STATUS __nvoc_objCreate_CeUtils(CeUtils **ppThis, Dynamic *pParent, NvU32 createFlags, struct OBJGPU * arg_pGpu, KERNEL_MIG_GPU_INSTANCE * arg_pKernelMIGGPUInstance, NV0050_ALLOCATION_PARAMETERS * arg_pAllocParams) {
    NV_STATUS status;
    Object *pParentObj;
    CeUtils *pThis;

    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(CeUtils), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    portMemSet(pThis, 0, sizeof(CeUtils));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_CeUtils);

    pThis->__nvoc_base_Object.createFlags = createFlags;

    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init_CeUtils(pThis);
    status = __nvoc_ctor_CeUtils(pThis, arg_pGpu, arg_pKernelMIGGPUInstance, arg_pAllocParams);
    if (status != NV_OK) goto __nvoc_objCreate_CeUtils_cleanup;

    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_CeUtils_cleanup:
    // do not call destructors here since the constructor already called them
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(CeUtils));
    else
        portMemFree(pThis);

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_CeUtils(CeUtils **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct OBJGPU * arg_pGpu = va_arg(args, struct OBJGPU *);
    KERNEL_MIG_GPU_INSTANCE * arg_pKernelMIGGPUInstance = va_arg(args, KERNEL_MIG_GPU_INSTANCE *);
    NV0050_ALLOCATION_PARAMETERS * arg_pAllocParams = va_arg(args, NV0050_ALLOCATION_PARAMETERS *);

    status = __nvoc_objCreate_CeUtils(ppThis, pParent, createFlags, arg_pGpu, arg_pKernelMIGGPUInstance, arg_pAllocParams);

    return status;
}

