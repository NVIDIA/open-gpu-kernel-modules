#define NVOC_GPU_ACCESS_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_gpu_access_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__40549c = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_IoAperture;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RegisterAperture;

// Forward declarations for IoAperture
void __nvoc_init__Object(Object*);
void __nvoc_init__RegisterAperture(RegisterAperture*);
void __nvoc_init__IoAperture(IoAperture*);
void __nvoc_init_funcTable_IoAperture(IoAperture*);
NV_STATUS __nvoc_ctor_IoAperture(IoAperture*, struct IoAperture *pParentAperture, OBJGPU *pGpu, NvU32 deviceIndex, NvU32 deviceInstance, DEVICE_MAPPING *pMapping, NvU32 mappingStartAddr, NvU32 offset, NvU64 length);
void __nvoc_init_dataField_IoAperture(IoAperture*);
void __nvoc_dtor_IoAperture(IoAperture*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__IoAperture;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__IoAperture;

// Down-thunk(s) to bridge IoAperture methods from ancestors (if any)
NvU8 __nvoc_down_thunk_IoAperture_regaprtReadReg08(struct RegisterAperture *pAperture, NvU32 addr);    // this
NvU16 __nvoc_down_thunk_IoAperture_regaprtReadReg16(struct RegisterAperture *pAperture, NvU32 addr);    // this
NvU32 __nvoc_down_thunk_IoAperture_regaprtReadReg32(struct RegisterAperture *pAperture, NvU32 addr);    // this
void __nvoc_down_thunk_IoAperture_regaprtWriteReg08(struct RegisterAperture *pAperture, NvU32 addr, NvV8 value);    // this
void __nvoc_down_thunk_IoAperture_regaprtWriteReg16(struct RegisterAperture *pAperture, NvU32 addr, NvV16 value);    // this
void __nvoc_down_thunk_IoAperture_regaprtWriteReg32(struct RegisterAperture *pAperture, NvU32 addr, NvV32 value);    // this
void __nvoc_down_thunk_IoAperture_regaprtWriteReg32Uc(struct RegisterAperture *pAperture, NvU32 addr, NvV32 value);    // this
NvBool __nvoc_down_thunk_IoAperture_regaprtIsRegValid(struct RegisterAperture *pAperture, NvU32 addr);    // this

// Up-thunk(s) to bridge IoAperture methods to ancestors (if any)

// Class-specific details for IoAperture
const struct NVOC_CLASS_DEF __nvoc_class_def_IoAperture = 
{
    .classInfo.size =               sizeof(IoAperture),
    .classInfo.classId =            classId(IoAperture),
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "IoAperture",
#endif
    .objCreatefn =        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_IoAperture,
    .pCastInfo =          &__nvoc_castinfo__IoAperture,
    .pExportInfo =        &__nvoc_export_info__IoAperture
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__IoAperture __nvoc_metadata__IoAperture = {
    .rtti.pClassDef = &__nvoc_class_def_IoAperture,    // (ioaprt) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_IoAperture,
    .rtti.offset    = 0,
    .metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super
    .metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Object.rtti.offset    = NV_OFFSETOF(IoAperture, __nvoc_base_Object),
    .metadata__RegisterAperture.rtti.pClassDef = &__nvoc_class_def_RegisterAperture,    // (regaprt) super
    .metadata__RegisterAperture.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RegisterAperture.rtti.offset    = NV_OFFSETOF(IoAperture, __nvoc_base_RegisterAperture),

    .vtable.__ioaprtReadReg08__ = &ioaprtReadReg08_IMPL,    // virtual override (regaprt) base (regaprt)
    .metadata__RegisterAperture.vtable.__regaprtReadReg08__ = &__nvoc_down_thunk_IoAperture_regaprtReadReg08,    // pure virtual
    .vtable.__ioaprtReadReg16__ = &ioaprtReadReg16_IMPL,    // virtual override (regaprt) base (regaprt)
    .metadata__RegisterAperture.vtable.__regaprtReadReg16__ = &__nvoc_down_thunk_IoAperture_regaprtReadReg16,    // pure virtual
    .vtable.__ioaprtReadReg32__ = &ioaprtReadReg32_IMPL,    // virtual override (regaprt) base (regaprt)
    .metadata__RegisterAperture.vtable.__regaprtReadReg32__ = &__nvoc_down_thunk_IoAperture_regaprtReadReg32,    // pure virtual
    .vtable.__ioaprtWriteReg08__ = &ioaprtWriteReg08_IMPL,    // virtual override (regaprt) base (regaprt)
    .metadata__RegisterAperture.vtable.__regaprtWriteReg08__ = &__nvoc_down_thunk_IoAperture_regaprtWriteReg08,    // pure virtual
    .vtable.__ioaprtWriteReg16__ = &ioaprtWriteReg16_IMPL,    // virtual override (regaprt) base (regaprt)
    .metadata__RegisterAperture.vtable.__regaprtWriteReg16__ = &__nvoc_down_thunk_IoAperture_regaprtWriteReg16,    // pure virtual
    .vtable.__ioaprtWriteReg32__ = &ioaprtWriteReg32_IMPL,    // virtual override (regaprt) base (regaprt)
    .metadata__RegisterAperture.vtable.__regaprtWriteReg32__ = &__nvoc_down_thunk_IoAperture_regaprtWriteReg32,    // pure virtual
    .vtable.__ioaprtWriteReg32Uc__ = &ioaprtWriteReg32Uc_IMPL,    // virtual override (regaprt) base (regaprt)
    .metadata__RegisterAperture.vtable.__regaprtWriteReg32Uc__ = &__nvoc_down_thunk_IoAperture_regaprtWriteReg32Uc,    // pure virtual
    .vtable.__ioaprtIsRegValid__ = &ioaprtIsRegValid_IMPL,    // virtual override (regaprt) base (regaprt)
    .metadata__RegisterAperture.vtable.__regaprtIsRegValid__ = &__nvoc_down_thunk_IoAperture_regaprtIsRegValid,    // pure virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__IoAperture = {
    .numRelatives = 3,
    .relatives = {
        &__nvoc_metadata__IoAperture.rtti,    // [0]: (ioaprt) this
        &__nvoc_metadata__IoAperture.metadata__Object.rtti,    // [1]: (obj) super
        &__nvoc_metadata__IoAperture.metadata__RegisterAperture.rtti,    // [2]: (regaprt) super
    }
};

// 8 down-thunk(s) defined to bridge methods in IoAperture from superclasses

// ioaprtReadReg08: virtual override (regaprt) base (regaprt)
NvU8 __nvoc_down_thunk_IoAperture_regaprtReadReg08(struct RegisterAperture *pAperture, NvU32 addr) {
    return ioaprtReadReg08((struct IoAperture *)(((unsigned char *) pAperture) - NV_OFFSETOF(IoAperture, __nvoc_base_RegisterAperture)), addr);
}

// ioaprtReadReg16: virtual override (regaprt) base (regaprt)
NvU16 __nvoc_down_thunk_IoAperture_regaprtReadReg16(struct RegisterAperture *pAperture, NvU32 addr) {
    return ioaprtReadReg16((struct IoAperture *)(((unsigned char *) pAperture) - NV_OFFSETOF(IoAperture, __nvoc_base_RegisterAperture)), addr);
}

// ioaprtReadReg32: virtual override (regaprt) base (regaprt)
NvU32 __nvoc_down_thunk_IoAperture_regaprtReadReg32(struct RegisterAperture *pAperture, NvU32 addr) {
    return ioaprtReadReg32((struct IoAperture *)(((unsigned char *) pAperture) - NV_OFFSETOF(IoAperture, __nvoc_base_RegisterAperture)), addr);
}

// ioaprtWriteReg08: virtual override (regaprt) base (regaprt)
void __nvoc_down_thunk_IoAperture_regaprtWriteReg08(struct RegisterAperture *pAperture, NvU32 addr, NvV8 value) {
    ioaprtWriteReg08((struct IoAperture *)(((unsigned char *) pAperture) - NV_OFFSETOF(IoAperture, __nvoc_base_RegisterAperture)), addr, value);
}

// ioaprtWriteReg16: virtual override (regaprt) base (regaprt)
void __nvoc_down_thunk_IoAperture_regaprtWriteReg16(struct RegisterAperture *pAperture, NvU32 addr, NvV16 value) {
    ioaprtWriteReg16((struct IoAperture *)(((unsigned char *) pAperture) - NV_OFFSETOF(IoAperture, __nvoc_base_RegisterAperture)), addr, value);
}

// ioaprtWriteReg32: virtual override (regaprt) base (regaprt)
void __nvoc_down_thunk_IoAperture_regaprtWriteReg32(struct RegisterAperture *pAperture, NvU32 addr, NvV32 value) {
    ioaprtWriteReg32((struct IoAperture *)(((unsigned char *) pAperture) - NV_OFFSETOF(IoAperture, __nvoc_base_RegisterAperture)), addr, value);
}

// ioaprtWriteReg32Uc: virtual override (regaprt) base (regaprt)
void __nvoc_down_thunk_IoAperture_regaprtWriteReg32Uc(struct RegisterAperture *pAperture, NvU32 addr, NvV32 value) {
    ioaprtWriteReg32Uc((struct IoAperture *)(((unsigned char *) pAperture) - NV_OFFSETOF(IoAperture, __nvoc_base_RegisterAperture)), addr, value);
}

// ioaprtIsRegValid: virtual override (regaprt) base (regaprt)
NvBool __nvoc_down_thunk_IoAperture_regaprtIsRegValid(struct RegisterAperture *pAperture, NvU32 addr) {
    return ioaprtIsRegValid((struct IoAperture *)(((unsigned char *) pAperture) - NV_OFFSETOF(IoAperture, __nvoc_base_RegisterAperture)), addr);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__IoAperture = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct IoAperture object.
void __nvoc_ioaprtDestruct(IoAperture*);
void __nvoc_dtor_Object(Object*);
void __nvoc_dtor_RegisterAperture(RegisterAperture*);
void __nvoc_dtor_IoAperture(IoAperture* pThis) {

// Call destructor.
    __nvoc_ioaprtDestruct(pThis);

// Recurse to superclass destructors.
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
    __nvoc_dtor_RegisterAperture(&pThis->__nvoc_base_RegisterAperture);

    PORT_UNREFERENCED_VARIABLE(pThis);
}
void __nvoc_init_dataField_IoAperture(IoAperture *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct IoAperture object.
NV_STATUS __nvoc_ctor_Object(Object *);
NV_STATUS __nvoc_ctor_RegisterAperture(RegisterAperture *);
NV_STATUS __nvoc_ctor_IoAperture(IoAperture *pAperture, struct IoAperture *pParentAperture, OBJGPU *pGpu, NvU32 deviceIndex, NvU32 deviceInstance, DEVICE_MAPPING *pMapping, NvU32 mappingStartAddr, NvU32 offset, NvU64 length) {
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_Object(&pAperture->__nvoc_base_Object);
    if (status != NV_OK) goto __nvoc_ctor_IoAperture_fail_Object;
    status = __nvoc_ctor_RegisterAperture(&pAperture->__nvoc_base_RegisterAperture);
    if (status != NV_OK) goto __nvoc_ctor_IoAperture_fail_RegisterAperture;

    // Initialize data fields.
    __nvoc_init_dataField_IoAperture(pAperture);

    // Call the constructor for this class.
    status = __nvoc_ioaprtConstruct(pAperture, pParentAperture, pGpu, deviceIndex, deviceInstance, pMapping, mappingStartAddr, offset, length);
    if (status != NV_OK) goto __nvoc_ctor_IoAperture_fail__init;
    goto __nvoc_ctor_IoAperture_exit; // Success

    // Unwind on error.
__nvoc_ctor_IoAperture_fail__init:
    __nvoc_dtor_RegisterAperture(&pAperture->__nvoc_base_RegisterAperture);
__nvoc_ctor_IoAperture_fail_RegisterAperture:
    __nvoc_dtor_Object(&pAperture->__nvoc_base_Object);
__nvoc_ctor_IoAperture_fail_Object:
__nvoc_ctor_IoAperture_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_IoAperture_1(IoAperture *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_IoAperture_1


// Initialize vtable(s) for 8 virtual method(s).
void __nvoc_init_funcTable_IoAperture(IoAperture *pThis) {
    __nvoc_init_funcTable_IoAperture_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__IoAperture(IoAperture *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;    // (obj) super
    pThis->__nvoc_pbase_RegisterAperture = &pThis->__nvoc_base_RegisterAperture;    // (regaprt) super
    pThis->__nvoc_pbase_IoAperture = pThis;    // (ioaprt) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__Object(&pThis->__nvoc_base_Object);
    __nvoc_init__RegisterAperture(&pThis->__nvoc_base_RegisterAperture);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__IoAperture.metadata__Object;    // (obj) super
    pThis->__nvoc_base_RegisterAperture.__nvoc_metadata_ptr = &__nvoc_metadata__IoAperture.metadata__RegisterAperture;    // (regaprt) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__IoAperture;    // (ioaprt) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_IoAperture(pThis);
}

NV_STATUS __nvoc_objCreate_IoAperture(IoAperture **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct IoAperture *pParentAperture, OBJGPU *pGpu, NvU32 deviceIndex, NvU32 deviceInstance, DEVICE_MAPPING *pMapping, NvU32 mappingStartAddr, NvU32 offset, NvU64 length)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    IoAperture *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(IoAperture));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(IoAperture));

    __nvoc_pThis->__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__IoAperture(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_IoAperture(__nvoc_pThis, pParentAperture, pGpu, deviceIndex, deviceInstance, pMapping, mappingStartAddr, offset, length);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_IoAperture_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_IoAperture_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(IoAperture));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_IoAperture(IoAperture **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    NV_STATUS __nvoc_status;
    struct IoAperture *pParentAperture = va_arg(__nvoc_args, struct IoAperture *);
    OBJGPU *pGpu = va_arg(__nvoc_args, OBJGPU *);
    NvU32 deviceIndex = va_arg(__nvoc_args, NvU32);
    NvU32 deviceInstance = va_arg(__nvoc_args, NvU32);
    DEVICE_MAPPING *pMapping = va_arg(__nvoc_args, DEVICE_MAPPING *);
    NvU32 mappingStartAddr = va_arg(__nvoc_args, NvU32);
    NvU32 offset = va_arg(__nvoc_args, NvU32);
    NvU64 length = va_arg(__nvoc_args, NvU64);

    __nvoc_status = __nvoc_objCreate_IoAperture(__nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pParentAperture, pGpu, deviceIndex, deviceInstance, pMapping, mappingStartAddr, offset, length);

    return __nvoc_status;
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__6d0f88 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_SwBcAperture;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RegisterAperture;

// Forward declarations for SwBcAperture
void __nvoc_init__Object(Object*);
void __nvoc_init__RegisterAperture(RegisterAperture*);
void __nvoc_init__SwBcAperture(SwBcAperture*);
void __nvoc_init_funcTable_SwBcAperture(SwBcAperture*);
NV_STATUS __nvoc_ctor_SwBcAperture(SwBcAperture*, struct IoAperture *pApertures, NvU32 numApertures);
void __nvoc_init_dataField_SwBcAperture(SwBcAperture*);
void __nvoc_dtor_SwBcAperture(SwBcAperture*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__SwBcAperture;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__SwBcAperture;

// Down-thunk(s) to bridge SwBcAperture methods from ancestors (if any)
NvU8 __nvoc_down_thunk_SwBcAperture_regaprtReadReg08(struct RegisterAperture *pAperture, NvU32 addr);    // this
NvU16 __nvoc_down_thunk_SwBcAperture_regaprtReadReg16(struct RegisterAperture *pAperture, NvU32 addr);    // this
NvU32 __nvoc_down_thunk_SwBcAperture_regaprtReadReg32(struct RegisterAperture *pAperture, NvU32 addr);    // this
void __nvoc_down_thunk_SwBcAperture_regaprtWriteReg08(struct RegisterAperture *pAperture, NvU32 addr, NvV8 value);    // this
void __nvoc_down_thunk_SwBcAperture_regaprtWriteReg16(struct RegisterAperture *pAperture, NvU32 addr, NvV16 value);    // this
void __nvoc_down_thunk_SwBcAperture_regaprtWriteReg32(struct RegisterAperture *pAperture, NvU32 addr, NvV32 value);    // this
void __nvoc_down_thunk_SwBcAperture_regaprtWriteReg32Uc(struct RegisterAperture *pAperture, NvU32 addr, NvV32 value);    // this
NvBool __nvoc_down_thunk_SwBcAperture_regaprtIsRegValid(struct RegisterAperture *pAperture, NvU32 addr);    // this

// Up-thunk(s) to bridge SwBcAperture methods to ancestors (if any)

// Class-specific details for SwBcAperture
const struct NVOC_CLASS_DEF __nvoc_class_def_SwBcAperture = 
{
    .classInfo.size =               sizeof(SwBcAperture),
    .classInfo.classId =            classId(SwBcAperture),
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "SwBcAperture",
#endif
    .objCreatefn =        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_SwBcAperture,
    .pCastInfo =          &__nvoc_castinfo__SwBcAperture,
    .pExportInfo =        &__nvoc_export_info__SwBcAperture
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__SwBcAperture __nvoc_metadata__SwBcAperture = {
    .rtti.pClassDef = &__nvoc_class_def_SwBcAperture,    // (swbcaprt) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_SwBcAperture,
    .rtti.offset    = 0,
    .metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super
    .metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Object.rtti.offset    = NV_OFFSETOF(SwBcAperture, __nvoc_base_Object),
    .metadata__RegisterAperture.rtti.pClassDef = &__nvoc_class_def_RegisterAperture,    // (regaprt) super
    .metadata__RegisterAperture.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RegisterAperture.rtti.offset    = NV_OFFSETOF(SwBcAperture, __nvoc_base_RegisterAperture),

    .vtable.__swbcaprtReadReg08__ = &swbcaprtReadReg08_IMPL,    // virtual override (regaprt) base (regaprt)
    .metadata__RegisterAperture.vtable.__regaprtReadReg08__ = &__nvoc_down_thunk_SwBcAperture_regaprtReadReg08,    // pure virtual
    .vtable.__swbcaprtReadReg16__ = &swbcaprtReadReg16_IMPL,    // virtual override (regaprt) base (regaprt)
    .metadata__RegisterAperture.vtable.__regaprtReadReg16__ = &__nvoc_down_thunk_SwBcAperture_regaprtReadReg16,    // pure virtual
    .vtable.__swbcaprtReadReg32__ = &swbcaprtReadReg32_IMPL,    // virtual override (regaprt) base (regaprt)
    .metadata__RegisterAperture.vtable.__regaprtReadReg32__ = &__nvoc_down_thunk_SwBcAperture_regaprtReadReg32,    // pure virtual
    .vtable.__swbcaprtWriteReg08__ = &swbcaprtWriteReg08_IMPL,    // virtual override (regaprt) base (regaprt)
    .metadata__RegisterAperture.vtable.__regaprtWriteReg08__ = &__nvoc_down_thunk_SwBcAperture_regaprtWriteReg08,    // pure virtual
    .vtable.__swbcaprtWriteReg16__ = &swbcaprtWriteReg16_IMPL,    // virtual override (regaprt) base (regaprt)
    .metadata__RegisterAperture.vtable.__regaprtWriteReg16__ = &__nvoc_down_thunk_SwBcAperture_regaprtWriteReg16,    // pure virtual
    .vtable.__swbcaprtWriteReg32__ = &swbcaprtWriteReg32_IMPL,    // virtual override (regaprt) base (regaprt)
    .metadata__RegisterAperture.vtable.__regaprtWriteReg32__ = &__nvoc_down_thunk_SwBcAperture_regaprtWriteReg32,    // pure virtual
    .vtable.__swbcaprtWriteReg32Uc__ = &swbcaprtWriteReg32Uc_IMPL,    // virtual override (regaprt) base (regaprt)
    .metadata__RegisterAperture.vtable.__regaprtWriteReg32Uc__ = &__nvoc_down_thunk_SwBcAperture_regaprtWriteReg32Uc,    // pure virtual
    .vtable.__swbcaprtIsRegValid__ = &swbcaprtIsRegValid_IMPL,    // virtual override (regaprt) base (regaprt)
    .metadata__RegisterAperture.vtable.__regaprtIsRegValid__ = &__nvoc_down_thunk_SwBcAperture_regaprtIsRegValid,    // pure virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__SwBcAperture = {
    .numRelatives = 3,
    .relatives = {
        &__nvoc_metadata__SwBcAperture.rtti,    // [0]: (swbcaprt) this
        &__nvoc_metadata__SwBcAperture.metadata__Object.rtti,    // [1]: (obj) super
        &__nvoc_metadata__SwBcAperture.metadata__RegisterAperture.rtti,    // [2]: (regaprt) super
    }
};

// 8 down-thunk(s) defined to bridge methods in SwBcAperture from superclasses

// swbcaprtReadReg08: virtual override (regaprt) base (regaprt)
NvU8 __nvoc_down_thunk_SwBcAperture_regaprtReadReg08(struct RegisterAperture *pAperture, NvU32 addr) {
    return swbcaprtReadReg08((struct SwBcAperture *)(((unsigned char *) pAperture) - NV_OFFSETOF(SwBcAperture, __nvoc_base_RegisterAperture)), addr);
}

// swbcaprtReadReg16: virtual override (regaprt) base (regaprt)
NvU16 __nvoc_down_thunk_SwBcAperture_regaprtReadReg16(struct RegisterAperture *pAperture, NvU32 addr) {
    return swbcaprtReadReg16((struct SwBcAperture *)(((unsigned char *) pAperture) - NV_OFFSETOF(SwBcAperture, __nvoc_base_RegisterAperture)), addr);
}

// swbcaprtReadReg32: virtual override (regaprt) base (regaprt)
NvU32 __nvoc_down_thunk_SwBcAperture_regaprtReadReg32(struct RegisterAperture *pAperture, NvU32 addr) {
    return swbcaprtReadReg32((struct SwBcAperture *)(((unsigned char *) pAperture) - NV_OFFSETOF(SwBcAperture, __nvoc_base_RegisterAperture)), addr);
}

// swbcaprtWriteReg08: virtual override (regaprt) base (regaprt)
void __nvoc_down_thunk_SwBcAperture_regaprtWriteReg08(struct RegisterAperture *pAperture, NvU32 addr, NvV8 value) {
    swbcaprtWriteReg08((struct SwBcAperture *)(((unsigned char *) pAperture) - NV_OFFSETOF(SwBcAperture, __nvoc_base_RegisterAperture)), addr, value);
}

// swbcaprtWriteReg16: virtual override (regaprt) base (regaprt)
void __nvoc_down_thunk_SwBcAperture_regaprtWriteReg16(struct RegisterAperture *pAperture, NvU32 addr, NvV16 value) {
    swbcaprtWriteReg16((struct SwBcAperture *)(((unsigned char *) pAperture) - NV_OFFSETOF(SwBcAperture, __nvoc_base_RegisterAperture)), addr, value);
}

// swbcaprtWriteReg32: virtual override (regaprt) base (regaprt)
void __nvoc_down_thunk_SwBcAperture_regaprtWriteReg32(struct RegisterAperture *pAperture, NvU32 addr, NvV32 value) {
    swbcaprtWriteReg32((struct SwBcAperture *)(((unsigned char *) pAperture) - NV_OFFSETOF(SwBcAperture, __nvoc_base_RegisterAperture)), addr, value);
}

// swbcaprtWriteReg32Uc: virtual override (regaprt) base (regaprt)
void __nvoc_down_thunk_SwBcAperture_regaprtWriteReg32Uc(struct RegisterAperture *pAperture, NvU32 addr, NvV32 value) {
    swbcaprtWriteReg32Uc((struct SwBcAperture *)(((unsigned char *) pAperture) - NV_OFFSETOF(SwBcAperture, __nvoc_base_RegisterAperture)), addr, value);
}

// swbcaprtIsRegValid: virtual override (regaprt) base (regaprt)
NvBool __nvoc_down_thunk_SwBcAperture_regaprtIsRegValid(struct RegisterAperture *pAperture, NvU32 addr) {
    return swbcaprtIsRegValid((struct SwBcAperture *)(((unsigned char *) pAperture) - NV_OFFSETOF(SwBcAperture, __nvoc_base_RegisterAperture)), addr);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__SwBcAperture = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct SwBcAperture object.
void __nvoc_dtor_Object(Object*);
void __nvoc_dtor_RegisterAperture(RegisterAperture*);
void __nvoc_dtor_SwBcAperture(SwBcAperture* pThis) {

// Recurse to superclass destructors.
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
    __nvoc_dtor_RegisterAperture(&pThis->__nvoc_base_RegisterAperture);

    PORT_UNREFERENCED_VARIABLE(pThis);
}
void __nvoc_init_dataField_SwBcAperture(SwBcAperture *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct SwBcAperture object.
NV_STATUS __nvoc_ctor_Object(Object *);
NV_STATUS __nvoc_ctor_RegisterAperture(RegisterAperture *);
NV_STATUS __nvoc_ctor_SwBcAperture(SwBcAperture *pAperture, struct IoAperture *pApertures, NvU32 numApertures) {
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_Object(&pAperture->__nvoc_base_Object);
    if (status != NV_OK) goto __nvoc_ctor_SwBcAperture_fail_Object;
    status = __nvoc_ctor_RegisterAperture(&pAperture->__nvoc_base_RegisterAperture);
    if (status != NV_OK) goto __nvoc_ctor_SwBcAperture_fail_RegisterAperture;

    // Initialize data fields.
    __nvoc_init_dataField_SwBcAperture(pAperture);

    // Call the constructor for this class.
    status = __nvoc_swbcaprtConstruct(pAperture, pApertures, numApertures);
    if (status != NV_OK) goto __nvoc_ctor_SwBcAperture_fail__init;
    goto __nvoc_ctor_SwBcAperture_exit; // Success

    // Unwind on error.
__nvoc_ctor_SwBcAperture_fail__init:
    __nvoc_dtor_RegisterAperture(&pAperture->__nvoc_base_RegisterAperture);
__nvoc_ctor_SwBcAperture_fail_RegisterAperture:
    __nvoc_dtor_Object(&pAperture->__nvoc_base_Object);
__nvoc_ctor_SwBcAperture_fail_Object:
__nvoc_ctor_SwBcAperture_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_SwBcAperture_1(SwBcAperture *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_SwBcAperture_1


// Initialize vtable(s) for 8 virtual method(s).
void __nvoc_init_funcTable_SwBcAperture(SwBcAperture *pThis) {
    __nvoc_init_funcTable_SwBcAperture_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__SwBcAperture(SwBcAperture *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;    // (obj) super
    pThis->__nvoc_pbase_RegisterAperture = &pThis->__nvoc_base_RegisterAperture;    // (regaprt) super
    pThis->__nvoc_pbase_SwBcAperture = pThis;    // (swbcaprt) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__Object(&pThis->__nvoc_base_Object);
    __nvoc_init__RegisterAperture(&pThis->__nvoc_base_RegisterAperture);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__SwBcAperture.metadata__Object;    // (obj) super
    pThis->__nvoc_base_RegisterAperture.__nvoc_metadata_ptr = &__nvoc_metadata__SwBcAperture.metadata__RegisterAperture;    // (regaprt) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__SwBcAperture;    // (swbcaprt) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_SwBcAperture(pThis);
}

NV_STATUS __nvoc_objCreate_SwBcAperture(SwBcAperture **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct IoAperture *pApertures, NvU32 numApertures)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    SwBcAperture *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(SwBcAperture));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(SwBcAperture));

    __nvoc_pThis->__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__SwBcAperture(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_SwBcAperture(__nvoc_pThis, pApertures, numApertures);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_SwBcAperture_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_SwBcAperture_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(SwBcAperture));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_SwBcAperture(SwBcAperture **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    NV_STATUS __nvoc_status;
    struct IoAperture *pApertures = va_arg(__nvoc_args, struct IoAperture *);
    NvU32 numApertures = va_arg(__nvoc_args, NvU32);

    __nvoc_status = __nvoc_objCreate_SwBcAperture(__nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pApertures, numApertures);

    return __nvoc_status;
}

