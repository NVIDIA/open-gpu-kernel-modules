#define NVOC_SEC2_UTILS_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_sec2_utils_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0x7716b1 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Sec2Utils;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

// Forward declarations for Sec2Utils
void __nvoc_init__Object(Object*);
void __nvoc_init__Sec2Utils(Sec2Utils*);
void __nvoc_init_funcTable_Sec2Utils(Sec2Utils*);
NV_STATUS __nvoc_ctor_Sec2Utils(Sec2Utils*, struct OBJGPU *arg_pGpu, KERNEL_MIG_GPU_INSTANCE *arg_pKernelMIGGPUInstance);
void __nvoc_init_dataField_Sec2Utils(Sec2Utils*);
void __nvoc_dtor_Sec2Utils(Sec2Utils*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__Sec2Utils;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__Sec2Utils;

// Down-thunk(s) to bridge Sec2Utils methods from ancestors (if any)

// Up-thunk(s) to bridge Sec2Utils methods to ancestors (if any)

const struct NVOC_CLASS_DEF __nvoc_class_def_Sec2Utils = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(Sec2Utils),
        /*classId=*/            classId(Sec2Utils),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "Sec2Utils",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_Sec2Utils,
    /*pCastInfo=*/          &__nvoc_castinfo__Sec2Utils,
    /*pExportInfo=*/        &__nvoc_export_info__Sec2Utils
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__Sec2Utils __nvoc_metadata__Sec2Utils = {
    .rtti.pClassDef = &__nvoc_class_def_Sec2Utils,    // (sec2utils) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_Sec2Utils,
    .rtti.offset    = 0,
    .metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super
    .metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Object.rtti.offset    = NV_OFFSETOF(Sec2Utils, __nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__Sec2Utils = {
    .numRelatives = 2,
    .relatives = {
        &__nvoc_metadata__Sec2Utils.rtti,    // [0]: (sec2utils) this
        &__nvoc_metadata__Sec2Utils.metadata__Object.rtti,    // [1]: (obj) super
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__Sec2Utils = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_Object(Object*);
void __nvoc_dtor_Sec2Utils(Sec2Utils *pThis) {
    __nvoc_sec2utilsDestruct(pThis);
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_Sec2Utils(Sec2Utils *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_Object(Object* );
NV_STATUS __nvoc_ctor_Sec2Utils(Sec2Utils *pThis, struct OBJGPU * arg_pGpu, KERNEL_MIG_GPU_INSTANCE * arg_pKernelMIGGPUInstance) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_Object(&pThis->__nvoc_base_Object);
    if (status != NV_OK) goto __nvoc_ctor_Sec2Utils_fail_Object;
    __nvoc_init_dataField_Sec2Utils(pThis);

    status = __nvoc_sec2utilsConstruct(pThis, arg_pGpu, arg_pKernelMIGGPUInstance);
    if (status != NV_OK) goto __nvoc_ctor_Sec2Utils_fail__init;
    goto __nvoc_ctor_Sec2Utils_exit; // Success

__nvoc_ctor_Sec2Utils_fail__init:
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
__nvoc_ctor_Sec2Utils_fail_Object:
__nvoc_ctor_Sec2Utils_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_Sec2Utils_1(Sec2Utils *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_Sec2Utils_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_Sec2Utils(Sec2Utils *pThis) {
    __nvoc_init_funcTable_Sec2Utils_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__Sec2Utils(Sec2Utils *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;    // (obj) super
    pThis->__nvoc_pbase_Sec2Utils = pThis;    // (sec2utils) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__Object(&pThis->__nvoc_base_Object);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__Sec2Utils.metadata__Object;    // (obj) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__Sec2Utils;    // (sec2utils) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_Sec2Utils(pThis);
}

NV_STATUS __nvoc_objCreate_Sec2Utils(Sec2Utils **ppThis, Dynamic *pParent, NvU32 createFlags, struct OBJGPU * arg_pGpu, KERNEL_MIG_GPU_INSTANCE * arg_pKernelMIGGPUInstance)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    Sec2Utils *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(Sec2Utils), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(Sec2Utils));

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

    __nvoc_init__Sec2Utils(pThis);
    status = __nvoc_ctor_Sec2Utils(pThis, arg_pGpu, arg_pKernelMIGGPUInstance);
    if (status != NV_OK) goto __nvoc_objCreate_Sec2Utils_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_Sec2Utils_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(Sec2Utils));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_Sec2Utils(Sec2Utils **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct OBJGPU * arg_pGpu = va_arg(args, struct OBJGPU *);
    KERNEL_MIG_GPU_INSTANCE * arg_pKernelMIGGPUInstance = va_arg(args, KERNEL_MIG_GPU_INSTANCE *);

    status = __nvoc_objCreate_Sec2Utils(ppThis, pParent, createFlags, arg_pGpu, arg_pKernelMIGGPUInstance);

    return status;
}

